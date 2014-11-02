/*

   Copyright (C) 2013 Stephen Robinson
  
   This file is part of HDMI-Light
  
   HDMI-Light is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.
  
   HDMI-Light is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this code (see the file names COPING).  
   If not, see <http://www.gnu.org/licenses/>.
  
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"

#include "config_hdmi.h"
#include "config_lights.h"


#define BELL    '\a'

#define AMBILIGHT_ADDR_LOW  _SFR_IO8(0x26)
#define AMBILIGHT_ADDR_HIGH _SFR_IO8(0x27)
#define AMBILIGHT_DATA      _SFR_IO8(0x28)

#define AMBILIGHT_BASE_ADDR_OUTPUT 0x0000
#define AMBILIGHT_BASE_ADDR_COLOUR 0x2000
#define AMBILIGHT_BASE_ADDR_AREA   0x3000
#define AMBILIGHT_BASE_ADDR_GAMMAR 0x4000
#define AMBILIGHT_BASE_ADDR_GAMMAG 0x4800
#define AMBILIGHT_BASE_ADDR_GAMMAB 0x5000
#define AMBILIGHT_BASE_ADDR_RESULT 0x5800
#define AMBILIGHT_BASE_ADDR_STATUS 0x6000
#define AMBILIGHT_BASE_ADDR_DELAY  0x6800


static uint8_t silent = 0;


char readcmd(char** argv, char maxargs)
{
	static char cmdbuf[256] = {0, 0};
	static unsigned char current = 0;
	unsigned char history = 0;
	unsigned char pos = current + 1;
	unsigned char end = current + 2;
	unsigned char arg;

	while(1)
	{
		int c = serial_getchar();
		if(c == -1)
			continue;

		switch(c)
		{
			case 0x1B:
				while((c = serial_getchar()) == -1)
					;
				if(c == '[')
				{
					while((c = serial_getchar()) == -1)
						;
					switch(c)
					{
						case 'A': goto UP;
						case 'B': goto DOWN;
						case 'C': goto RIGHT;
						case 'D': goto LEFT;
					}
				}
				break;
			LEFT:
			case 0x02: // left
				if(pos - 1 != current)
				{
					--pos;
					printf_P(PSTR("\x1B[D")); // move left
				}
				else
					serial_putchar(BELL);
				break;
			case 0x08: // backspace
			case 0x7f:
				if(pos > 0)
				{
					memmove(cmdbuf + pos - 1, cmdbuf + pos, end - pos);
					--pos; --end;
					printf_P(PSTR("\x1B[D%s \x1B[%dD"), cmdbuf + pos, end - pos); // move left
				}
				else
					serial_putchar(BELL);
				break;
			RIGHT:
			case 0x06: // right
				if(pos + 1 != end)
				{
					++pos;
					printf_P(PSTR("\x1B[C")); // move right
				}
				else
					serial_putchar(BELL);
				break;
			case '\n':
			case '\r':
				goto DONE;
			UP:
			case 0x10:
				if(1)
				{
					unsigned char p = current;
					unsigned char len = 0;
					unsigned char i;

					for(i = 0; i < history + 1; ++i)
					{
						len = cmdbuf[p];
						if(len == 0)
							break;
						p -= len;
					}
					if(i == history + 1)
					{
						++history;
						for(i = 0; i < len - 1; ++i)
							cmdbuf[current + 1 + i] = (cmdbuf[p + 1 + i] == '\0' ? 
							                           ' ' : cmdbuf[p + 1 + i]);
						cmdbuf[current + len - 1] = '\0';
						printf_P(PSTR(" \x1B[%dD%s"), pos - current, cmdbuf + current + 1);
						if(end - (current + len) > 0)
							printf_P(PSTR("%*s\x1B[%dD"), end - (current + len), "", end - (current + len));
						end = current + len;
						pos = end - 1;
					}
					else
						serial_putchar(BELL);
				}
				break;
			DOWN:
			case 0x0E:
				if(history > 1)
				{
					unsigned char p = current;
					unsigned char len;
					unsigned char i;

					--history;
					for(i = 0; i < history; ++i)
					{
						len = cmdbuf[p];
						if(len == 0)
							break;
						p -= len;
					}

					for(i = 0; i < len - 1; ++i)
						cmdbuf[current + 1 + i] = (cmdbuf[p + 1 + i] == '\0' ? 
									   ' ' : cmdbuf[p + 1 + i]);
					cmdbuf[current + len - 1] = '\0';
					printf_P(PSTR(" \x1B[%dD%s"), pos - current, cmdbuf + current + 1);
					if(end - (current + len) > 0)
						printf_P(PSTR("%*s\x1B[%dD"), end - (current + len), "", end - (current + len));
					end = current + len;
					pos = end - 1;
				}
				else if(history == 1)
				{
				}
				else
					serial_putchar(BELL);
				break;
			default:
				if(end + 1 != current)
				{
					int i;
					for(i = end; i > pos; --i)
						cmdbuf[i] = cmdbuf[i - 1];
					cmdbuf[pos] = c;
					printf_P(PSTR("%s \x1B[%dD"), cmdbuf + pos, end - pos);
					++pos;
					++end;

				}
				else
					serial_putchar(BELL);
				break;
		}
	}

DONE:
	pos = current + 1;
	for(arg = 0; arg < maxargs; ++arg)
	{
		if(cmdbuf[pos] == '\0')
			break; 

		while(cmdbuf[pos] == ' ')
			cmdbuf[pos++] = '\0';

		argv[arg] = cmdbuf + pos;

		while(cmdbuf[pos] != ' ' && cmdbuf[pos] != '\0')
			++pos;
	}
	
	cmdbuf[end] = end - current;
	cmdbuf[end + 1] = '\0';
	current = end;
	
	return arg;
}

int getint(char** str)
{
	char* s = *str;
	uint8_t neg = 0;
	uint8_t x = 0;

	if(s[0] == '-')
	{
		neg = 1;
		++s;
	}

	if(s[0] == '0' && s[1] == 'x')
	{
		s += 2;
		while(1)
		{
			if(*s >= '0' && *s <= '9')
				x = (x << 4) + (*s - '0');
			else if(*s >= 'A' && *s <= 'F')
				x = (x << 4) + (*s - 'A' + 10);
			else if(*s >= 'a' && *s <= 'f')
				x = (x << 4) + (*s - 'a' + 10);
			else
				break;
			++s;
		}
	}
	else
	{
		while(*s >= '0' && *s <= '9')
			x = (x * 10) + (*s++ - '0');
	}
	
	*str = s;
	return neg ? 0 - x : x;
}

void getrange(char* str, uint8_t* min, uint8_t* max)
{
	if(*str == '*')
	{
		*min = 0;
		*max = 255;
		return;
	}
	else
	{
		*min = getint(&str);
		if(*str == '-')
		{
			++str;
			*max = getint(&str);
		}
		else
			*max = *min;
	}
}
	
void setGamma(uint8_t channel, uint8_t table, uint8_t index, uint8_t value)
{
	uint16_t address = AMBILIGHT_BASE_ADDR_GAMMAR;

	if(channel == 0)
		address = AMBILIGHT_BASE_ADDR_GAMMAR;
	else if(channel == 1)
		address = AMBILIGHT_BASE_ADDR_GAMMAG;
	else if(channel == 2)
		address = AMBILIGHT_BASE_ADDR_GAMMAB;

	address += (uint16_t)table * 256;
	address += index;

	AMBILIGHT_ADDR_HIGH = address >> 8;
	AMBILIGHT_ADDR_LOW  = address & 0xff;
	AMBILIGHT_DATA = value;
}

void setOutput(uint8_t output, uint16_t light, uint8_t area, uint8_t coef, uint8_t gamma, uint8_t enabled)
{
	//  15    12    9                 0
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |E| coe | gam | area            |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |               |               |

	uint16_t address = AMBILIGHT_BASE_ADDR_OUTPUT;
	address += (uint16_t)output * 1024;
	address += light * 2;

	if(enabled)
		enabled = 0x80;

	AMBILIGHT_ADDR_HIGH = address >> 8;
	AMBILIGHT_ADDR_LOW  = address & 0xff;
	AMBILIGHT_DATA = area & 0xff;
	AMBILIGHT_ADDR_LOW  = (address + 1) & 0xff;
	AMBILIGHT_DATA = enabled | ((coef & 7) << 4) | ((gamma & 7) << 1) | ((area >> 8) & 1);
}

void getOutput(uint8_t output, uint16_t light, int* area, int* coef, int* gamma, int* enabled)
{
	uint8_t bytes[2];
	uint16_t address = AMBILIGHT_BASE_ADDR_OUTPUT;
	address += (uint16_t)output * 1024;
	address += light * 2;

	AMBILIGHT_ADDR_HIGH = address >> 8;
	AMBILIGHT_ADDR_LOW  = address & 0xff;
	asm("nop");
	bytes[0] = AMBILIGHT_DATA;
	AMBILIGHT_ADDR_LOW = (address + 1) & 0xff;
	asm("nop");
	bytes[1] = AMBILIGHT_DATA;

	*area = (((uint16_t)bytes[1] & 1) << 8) | bytes[0];
	*gamma = (bytes[1] >> 1) & 7;
	*coef = (bytes[1] >> 4) & 7;
	*enabled = bytes[1] & 0x80;
}

void setColour(uint8_t index, uint8_t row, int ri, int rf, int gi, int gf, int bi, int bf)
{
	uint16_t bits;
	uint8_t bytes[8];

	//       54                45                36                27                18                 9                 0
	// +-/\+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |       | Bint            | Bfract          | Gint            | Gfract          | Rint            | Rfract          |
	// +-\/+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |   |   byte 6      |    byte 5     |    byte 4     |    byte 3     |    byte 2     |    byte 1     |    byte 0     |

	ri &= 0x1ff;
	rf &= 0x1ff;
	gi &= 0x1ff;
	gf &= 0x1ff;
	bi &= 0x1ff;
	bf &= 0x1ff;

	bits = rf;
	bytes[0] = bits & 0xff;
	bits = (bits >> 8) | (ri << 1);
	bytes[1] = bits & 0xff;
	bits = (bits >> 8) | (gf << 2);
	bytes[2] = bits & 0xff;
	bits = (bits >> 8) | (gi << 3);
	bytes[3] = bits & 0xff;
	bits = (bits >> 8) | (bf << 4);
	bytes[4] = bits & 0xff;
	bits = (bits >> 8) | (bi << 5);
	bytes[5] = bits & 0xff;
	bytes[6] = bits >> 8;
	bytes[7] = 0;

	printf_P(PSTR("%d %d %d %d %d %d %d %d\n"), bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);
	for(bits = 0; bits < 8; ++bits)
	{
		uint16_t address = AMBILIGHT_BASE_ADDR_COLOUR;
		address += (uint16_t)index * 32;
		address += row * 8;
		address += bits;
		AMBILIGHT_ADDR_HIGH = address >> 8;
		AMBILIGHT_ADDR_LOW  = address & 0xff;
		AMBILIGHT_DATA = bytes[bits];
	}
}

void getColour(uint8_t index, uint8_t row, int* ri, int* rf, int* gi, int* gf, int* bi, int* bf)
{
	int i;
	uint8_t bytes[8];
	uint16_t bits;
	
	for(i = 0; i < 8; ++i)
	{
		uint16_t address = AMBILIGHT_BASE_ADDR_COLOUR;
		address += (uint16_t)index * 32;
		address += row * 8;
		address += i;
		AMBILIGHT_ADDR_HIGH = address >> 8;
		AMBILIGHT_ADDR_LOW  = address & 0xff;
		asm("nop");
		bytes[i] = AMBILIGHT_DATA;
	}

	bits = (bytes[1] << 8) | bytes[0];
	*rf = (bits & 0x100) ? (bits & 0x1ff) | 0xfe00 : (bits & 0xff);
	bits = (bits >> 9) | (bytes[2] << 7);
	*ri = (bits & 0x100) ? (bits & 0x1ff) | 0xfe00 : (bits & 0xff);
	bits = (bits >> 9) | (bytes[3] << 6);
	*gf = (bits & 0x100) ? (bits & 0x1ff) | 0xfe00 : (bits & 0xff);
	bits = (bits >> 9) | (bytes[4] << 5);
	*gi = (bits & 0x100) ? (bits & 0x1ff) | 0xfe00 : (bits & 0xff);
	bits = (bits >> 9) | (bytes[5] << 4);
	*bf = (bits & 0x100) ? (bits & 0x1ff) | 0xfe00 : (bits & 0xff);
	bits = (bits >> 9) | (bytes[6] << 3);
	*bi = (bits & 0x100) ? (bits & 0x1ff) | 0xfe00 : (bits & 0xff);
}


void setLight(unsigned int light, 
              unsigned char xmin, unsigned char xmax,
              unsigned char ymin, unsigned char ymax,
              unsigned char shift, unsigned char output)
{
	uint16_t address = AMBILIGHT_BASE_ADDR_AREA;
	address += (uint16_t)light * 4;

	//  31      28      24          18          12          6           0
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// | | out   | shift | ymax      | ymin      | xmax      | xmin      |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |                 |               |               |               |

	light <<= 2;
	AMBILIGHT_ADDR_HIGH = address >> 8;
	AMBILIGHT_ADDR_LOW  = address & 0xff;
	AMBILIGHT_DATA = (xmin & 0x3f) | (xmax << 6);
	AMBILIGHT_ADDR_LOW = (address + 1) & 0xff;
	AMBILIGHT_DATA = ((xmax & 0x3f) >> 2) | (ymin << 4);
	AMBILIGHT_ADDR_LOW = (address + 2) & 0xff;
	AMBILIGHT_DATA = ((ymin & 0x3f) >> 4) | (ymax << 2);
	AMBILIGHT_ADDR_LOW = (address + 3) & 0xff;
	AMBILIGHT_DATA = (shift & 0xf) | (output << 4);
}

void cmdSetLight(uint8_t argc, char** argv)
{
	if(argc == 8)
	{
		uint8_t index, maxIndex;
		uint8_t xmin, xmax, ymin, ymax, shift, output;
		
		xmin = getint(&argv[2]);
		xmax = getint(&argv[3]);
		ymin = getint(&argv[4]);
		ymax = getint(&argv[5]);
		shift = getint(&argv[6]);
		output = getint(&argv[7]);

		getrange(argv[1], &index, &maxIndex);
		do
		{
			setLight(index, xmin, xmax, ymin, ymax, shift, output);

		} while(index++ < maxIndex);
	}
	//else
		//printf("err: SL light xmin xmax ymin ymax shift output\n");
}

void cmdGetDelay(uint8_t argc, char** argv)
{
	if(argc == 1)
	{
		int frames;
		uint32_t ticks;
		uint16_t address = AMBILIGHT_BASE_ADDR_DELAY;
		AMBILIGHT_ADDR_HIGH = address >> 8;
		AMBILIGHT_ADDR_LOW  = address & 0xff;
		asm("nop");
		frames = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 1) & 0xff;
		asm("nop");
		ticks = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 2) & 0xff;
		asm("nop");
		ticks = (ticks << 8) | AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 3) & 0xff;
		asm("nop");
		ticks = (ticks << 8) | AMBILIGHT_DATA;
		ticks /= 16;

		printf_P(PSTR("%d %ld\n"), frames, ticks);
	}
	//else
		//printf("err: GD\n");
}

void cmdSetDelay(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		uint8_t frames = getint(&argv[1]);
		uint32_t ticks = getint(&argv[2]);
		uint16_t address = AMBILIGHT_BASE_ADDR_DELAY;
		AMBILIGHT_ADDR_HIGH = address >> 8;
		AMBILIGHT_ADDR_LOW  = address & 0xff;
		AMBILIGHT_DATA = frames;

		ticks *= 16;
		AMBILIGHT_ADDR_LOW = (address + 1) & 0xff;
		AMBILIGHT_DATA = 0;
		AMBILIGHT_ADDR_LOW = (address + 2) & 0xff;
		AMBILIGHT_DATA = ticks >> 8;
		AMBILIGHT_ADDR_LOW = (address + 3) & 0xff;
		AMBILIGHT_DATA = ticks & 0xff;
	}
	//else
		//printf("err: SD num_frames num_ticks\n");
}

void cmdSetOutput(uint8_t argc, char** argv)
{
	if(argc == 7)
	{
		int area, coef, gamma, enabled;
		uint8_t output, maxOutput;
		uint8_t light, minLight, maxLight;

		area = getint(&argv[3]);
		coef = getint(&argv[4]);
		gamma = getint(&argv[5]);
		enabled = getint(&argv[6]);
		
		getrange(argv[1], &output, &maxOutput);
		getrange(argv[2], &minLight, &maxLight);
		do
		{
			light = minLight;
			do
			{
				setOutput(output, light, area, coef, gamma, enabled);
				
			} while(light++ < maxLight);
			
		} while(output++ < maxOutput);
	}
	//else
		//printf("err: SO output light area coef gamma enabled\n");
}

void cmdGetOutput(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		uint8_t output, maxOutput;
		uint8_t light, minLight, maxLight;

		getrange(argv[1], &output, &maxOutput);
		getrange(argv[2], &minLight, &maxLight);
		do
		{
			light = minLight;
			do
			{
				int area, coef, gamma, enabled;

				getOutput(output, light, &area, &coef, &gamma, &enabled);

				printf_P(PSTR("%d: %d: %d %d %d %d\n"), output, light, area, coef, gamma, enabled);
				
			} while(light++ < maxLight);
			
		} while(output++ < maxOutput);
	}
	//else
		//printf("err: GO output light\n");
}

void cmdSetColour(uint8_t argc, char** argv)
{
	if(argc == 9)
	{
		int ri, rf, gi, gf, bi, bf;
		uint8_t index, maxIndex;
		uint8_t row, minRow, maxRow;

		ri = getint(&argv[3]);
		rf = getint(&argv[4]);
		gi = getint(&argv[5]);
		gf = getint(&argv[6]);
		bi = getint(&argv[7]);
		bf = getint(&argv[8]);
		
		getrange(argv[1], &index, &maxIndex);
		getrange(argv[2], &minRow, &maxRow);
		do
		{
			row = minRow;
			do
			{
				setColour(index, row, ri, rf, gi, gf, bi, bf);
				
			} while(row++ < maxRow);
			
		} while(index++ < maxIndex);
	}
	//else
		//printf("err: SC index row ah al bh bl ch cl\n");
}

void cmdGetColour(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		uint8_t index, maxIndex;
		uint8_t row, minRow, maxRow;

		getrange(argv[1], &index, &maxIndex);
		getrange(argv[2], &minRow, &maxRow);
		do
		{
			row = minRow;
			do
			{
				int ri, rf, gi, gf, bi, bf;

				getColour(index, row, &ri, &rf, &gi, &gf, &bi, &bf);

				printf_P(PSTR("%d: %d: %d:%d %d:%d %d:%d\n"), index, row, ri, rf, gi, gf, bi, bf);
				
			} while(row++ < maxRow);
			
		} while(index++ < maxIndex);
	}
	//else
		//printf("err: GC index row\n");
}

void cmdGetLight(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		uint8_t index, maxIndex;

		getrange(argv[1], &index, &maxIndex);
		do
		{
			uint8_t i;
			uint8_t values[4];
			int x;

			for(i = 0; i < 4; ++i)
			{
				uint16_t address = AMBILIGHT_BASE_ADDR_AREA + ((uint16_t)index * 4) + i;
				AMBILIGHT_ADDR_HIGH = address >> 8;
				AMBILIGHT_ADDR_LOW  = address & 0xff;
				asm("nop");
				values[i] = AMBILIGHT_DATA;
			}

			x = values[0] & 0x3f; // xmin
			printf_P(PSTR("%d: %d "), index, x);
			x = (values[0] >> 6) | ((values[1] & 0x0f) << 2); // xmax
			printf_P(PSTR("%d "), x);
			x = (values[1] >> 4) | ((values[2] & 3) << 4); // ymin
			printf_P(PSTR("%d "), x);
			x = (values[2] >> 2); // ymax
			printf_P(PSTR("%d "), x);
			x = values[3] & 0xf; // shift
			printf_P(PSTR("%d "), x);
			x = values[3] >> 4; // output
			printf_P(PSTR("%d\n"), x);

		} while(index++ < maxIndex);
	}
	//else
		//printf("err: GL light\n");
}

void cmdGetStatus(uint8_t argc, char** argv)
{
	unsigned int status;
	uint16_t address = AMBILIGHT_BASE_ADDR_STATUS;
	AMBILIGHT_ADDR_HIGH = address >> 8;
	AMBILIGHT_ADDR_LOW = address & 0xff;
	asm("nop");
	status = AMBILIGHT_DATA;
	printf_P(PSTR("status: %d\n"), status);
}

void cmdGetResult(uint8_t argc, char** argv)
{
	if(argc == 2 || argc == 3)
	{
		uint8_t index, minIndex, maxIndex, repeat;

		if(argc == 3)
			repeat = getint(&argv[2]);
		else
			repeat = 1;

		getrange(argv[1], &minIndex, &maxIndex);

		while(repeat--)
		{
		index = minIndex;
		do
		{
			uint8_t i;
			uint16_t address;
			int value;

			address = AMBILIGHT_BASE_ADDR_RESULT;
			AMBILIGHT_ADDR_HIGH = address >> 8;
			AMBILIGHT_ADDR_LOW  = address & 0xff;
			AMBILIGHT_DATA = index;

			address = AMBILIGHT_BASE_ADDR_STATUS;
			AMBILIGHT_ADDR_HIGH = address >> 8;
			AMBILIGHT_ADDR_LOW  = address & 0xff;
			asm("nop");
			do { value = AMBILIGHT_DATA; } while((value & 1) == 0);

			printf_P(PSTR("%d: "), index);
			for(i = 0; i < 12; ++i)
			{
				address = AMBILIGHT_BASE_ADDR_RESULT;
				address += 4 + i;
				AMBILIGHT_ADDR_HIGH = address >> 8;
				AMBILIGHT_ADDR_LOW  = address & 0xff;
				asm("nop");
				value = AMBILIGHT_DATA;
				printf_P(PSTR("%d "), value);
			}
			printf_P(PSTR(" \n"));

		} while(index++ < maxIndex);
		}
	}
	//else
		//printf("err: GR index\n");
}

void cmdSetPort(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		unsigned char addr = getint(&argv[1]);
		unsigned char val  = getint(&argv[2]);
		_SFR_IO8(addr) = val;
		printf_P(PSTR("OK\n"));
	}
	//else
		//printf("err: SP addr value\n");
}

void cmdGetPort(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		unsigned char addr = getint(&argv[1]);
		unsigned char val = _SFR_IO8(addr);
		printf_P(PSTR("%d=%d\n"), (int)addr, (int)val);
	}
	//else
		//printf("err: GP addr\n");
}

void cmdCfgLight(uint8_t argc, char** argv)
{
	int i;

	for(i = 0; pgm_read_dword(&g_lightTable[i]) != 0xffffffff; ++i)
	{
		unsigned long light = pgm_read_dword(&g_lightTable[i]);
		setLight(i, 
		         LIGHT_XMIN(light), LIGHT_XMAX(light),
		         LIGHT_YMIN(light), LIGHT_YMAX(light),
		         LIGHT_SHIFT(light), LIGHT_OUTPUT(light));
		if(!silent)
			printf_P(PSTR("OK\n"));
	}
}

void cmdSetI2C(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		i2c_start();
		i2c_write(0x98);
		i2c_write(getint(&argv[1]));
		i2c_write(getint(&argv[2]));
		i2c_stop();
		printf_P(PSTR("OK\n"));
	}
	//else
		//printf("err: SI addr value\n");
}

void cmdGetI2C(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		int val;
		i2c_start();
		i2c_write(0x98);
		i2c_write(getint(&argv[1]));
		i2c_start();
		i2c_write(0x99);
		val = i2c_read(0);
		i2c_stop();

		printf_P(PSTR("Read: %d\n"), val);
	}
	//else
		//printf("err: GI addr\n");
}

void cmdCfgI2C(uint8_t argc, char** argv)
{
	const struct ConfigTable* p;

	for(p = g_configTable; pgm_read_byte(&p->address) != 0; ++p)
	{
		unsigned char address = pgm_read_byte(&p->address);
		unsigned char subaddress = pgm_read_byte(&p->subaddress);
		unsigned char data = pgm_read_byte(&p->data);

		if(!silent)
		{
			printf_P(PSTR("%d %d %d : "), address, subaddress, data);
			i2c_start();
			printf_P(PSTR("%s "), i2c_write(address)    ? "ACK" : "NACK");
			printf_P(PSTR("%s "), i2c_write(subaddress) ? "ACK" : "NACK");
			printf_P(PSTR("%s\n"), i2c_write(data)      ? "ACK" : "NACK");
			i2c_stop();
		}
		else
		{
			i2c_start();
			i2c_write(address);
			i2c_write(subaddress);
			i2c_write(data);
			i2c_stop();
		}
	}
}

void cmdGetAddr(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		uint16_t addr;
		uint8_t  count;

		addr = getint(&argv[1]);
		count = getint(&argv[2]);
		while(count--)
		{
			AMBILIGHT_ADDR_HIGH = addr >> 8;
			AMBILIGHT_ADDR_LOW  = addr & 0xff;
			asm("nop");
			printf_P(PSTR("%02x "), AMBILIGHT_DATA);
			++addr;
		}
	}
}

void cmdSetAddr(uint8_t argc, char** argv)
{
	if(argc > 2)
	{
		uint16_t address = getint(&argv[1]);

		argv += 2;
		argc -= 2;
		while(argc > 0)
		{
			AMBILIGHT_ADDR_HIGH = address >> 8;
			AMBILIGHT_ADDR_LOW  = address & 0xff;
			AMBILIGHT_DATA = getint(&argv[0]);
			++argv;
			--argc;
		}
	}
}

void cmdGetGamma(uint8_t argc, char** argv)
{
	if(argc == 4)
	{
		uint8_t table, maxTable;
		uint8_t channel, minChannel, maxChannel;
		uint8_t index, minIndex, maxIndex;

		getrange(argv[1], &table, &maxTable);
		getrange(argv[2], &minChannel, &maxChannel);
		getrange(argv[3], &minIndex, &maxIndex);
		do
		{
			channel = minChannel;
			do
			{
				index = minIndex;
				do
				{
					int value;
					uint16_t address = AMBILIGHT_BASE_ADDR_GAMMAR;
					if(channel == 0)
						address = AMBILIGHT_BASE_ADDR_GAMMAR;
					else if(channel == 1)
						address = AMBILIGHT_BASE_ADDR_GAMMAG;
					else if(channel == 2)
						address = AMBILIGHT_BASE_ADDR_GAMMAB;
					address += (uint16_t)table * 256;
					address += index;

					AMBILIGHT_ADDR_HIGH = address >> 8;
					AMBILIGHT_ADDR_LOW  = address & 0xff;
					asm("nop");
					value = AMBILIGHT_DATA;

					printf_P(PSTR("%d: %d: %d: %d\n"), table, channel, index, value);

				} while(index++ < maxIndex);
				
			} while(channel++ < maxChannel);
			
		} while(table++ < maxTable);
	}
	//else
		//printf("err: GG table channel index\n");
}

void cmdSetGamma(uint8_t argc, char** argv)
{
	if(argc == 9)
	{
		uint8_t value;
		uint8_t table, maxTable;
		uint8_t channel, minChannel, maxChannel;
		uint8_t index, minIndex, maxIndex;

		value = getint(&argv[4]);
		
		getrange(argv[1], &table, &maxTable);
		getrange(argv[2], &minChannel, &maxChannel);
		getrange(argv[3], &minIndex, &maxIndex);
		do
		{
			channel = minChannel;
			do
			{
				index = minIndex;
				do
				{
					setGamma(channel, table, index, value);

				} while(index++ < maxIndex);
				
			} while(channel++ < maxChannel);
			
		} while(table++ < maxTable);
	}
	//else
		//printf("err: SG table channel index value\n");
}

void cmdCfgGamma(uint8_t argc, char** argv)
{
	int table;
	for(table = 0; table < 8; ++table)
	{
		int channel;
		for(channel = 0; channel < 3; ++channel)
		{
			int index;
			for(index = 0; index < 256; ++index)
				setGamma(channel, table, index, index);
		}
	}
}

void cmdCfgOutput(uint8_t argc, char** argv)
{
	int output;

	for(output = 0; output < 8; ++output)
	{
		int light;

		for(light = 0; light < 512; ++light)
			setOutput(output, light, light & 0xff, 0, 0, (light & 0x100) == 0);
	}
}

void cmdCfgColour(uint8_t argc, char** argv)
{
	int i;
	for(i = 0; i < 8; ++i)
	{
		setColour(i, 0, 1, 0, 0, 0, 0, 0);
		setColour(i, 1, 0, 0, 1, 0, 0, 0);
		setColour(i, 2, 0, 0, 0, 0, 1, 0);
		setColour(i, 3, 0, 0, 0, 0, 0, 0);
	}
}

void cmdCfgDelay(uint8_t argc, char** argv)
{
	
}

void cmdCfgAll(uint8_t argc, char** argv)
{
	cmdCfgI2C(argc, argv);
	cmdCfgLight(argc, argv);
	cmdCfgColour(argc, argv);
	cmdCfgGamma(argc, argv);
	cmdCfgOutput(argc, argv);
	cmdCfgDelay(argc, argv);
}

char cmdBlankUsage[] PROGMEM = "";
char cmdGetAddrUsage[] PROGMEM = "addr count";
char cmdSetAddrUsage[] PROGMEM = "addr byte0 [byte1] [...]";
char cmdGetColourUsage[] PROGMEM = "index row";
char cmdSetColourUsage[] PROGMEM = "index row Ri Rf Gi Gf Bi Bf";
char cmdSetDelayUsage[] PROGMEM = "num_frames num_ticks";
char cmdGetGammaUsage[] PROGMEM = "table channel index";
char cmdSetGammaUsage[] PROGMEM = "table channel index value";
char cmdGetI2CUsage[] PROGMEM = "addr";
char cmdSetI2CUsage[] PROGMEM = "addr value";
char cmdGetLightUsage[] PROGMEM = "index";
char cmdSetLightUsage[] PROGMEM = "index xmin xmax ymin ymax shift output";
char cmdGetOutputUsage[] PROGMEM = "output light";
char cmdSetOutputUsage[] PROGMEM = "output light area coef gamma enable";
char cmdGetPortUsage[] PROGMEM = "addr";
char cmdSetPortUsage[] PROGMEM = "addr value";
char cmdGetResultUsage[] PROGMEM = "index";


int main()
{
	static struct
	{
		const char* cmd;
		void (*handler)(uint8_t argc, char** argv);
		char* usage;

	} cmds[] = 
	{
		{ "GA", cmdGetAddr,   cmdGetAddrUsage   },
		{ "SA", cmdSetAddr,   cmdSetAddrUsage   },
		{ "GC", cmdGetColour, cmdGetColourUsage },
		{ "SC", cmdSetColour, cmdSetColourUsage },
		{ "CC", cmdCfgColour, cmdBlankUsage     },
		{ "GD", cmdGetDelay,  cmdBlankUsage     },
		{ "SD", cmdSetDelay,  cmdSetDelayUsage  },
		{ "CD", cmdCfgDelay,  cmdBlankUsage     },
		{ "GG", cmdGetGamma,  cmdGetGammaUsage  },
		{ "SG", cmdSetGamma,  cmdSetGammaUsage  },
		{ "CG", cmdCfgGamma,  cmdBlankUsage     },
		{ "GI", cmdGetI2C,    cmdGetI2CUsage    },
		{ "SI", cmdSetI2C,    cmdSetI2CUsage    },
		{ "CI", cmdCfgI2C,    cmdBlankUsage     },
		{ "GL", cmdGetLight,  cmdGetLightUsage  },
		{ "SL", cmdSetLight,  cmdSetLightUsage  },
		{ "CL", cmdCfgLight,  cmdBlankUsage     },
		//{ "GM", cmdGetMem                     },
		//{ "SM", cmdSetMem                     },
		{ "GO", cmdGetOutput, cmdGetOutputUsage },
		{ "SO", cmdSetOutput, cmdSetOutputUsage },
		{ "CO", cmdCfgOutput, cmdBlankUsage     },
		{ "GP", cmdGetPort,   cmdGetPortUsage   },
		{ "SP", cmdSetPort,   cmdSetPortUsage   },
		{ "GR", cmdGetResult, cmdGetResultUsage },
		{ "GS", cmdGetStatus, cmdBlankUsage     },
		{ "CA", cmdCfgAll,    cmdBlankUsage     },
	};

	int i;
	char* argv[12];
	int argc;

	serial_init();
	//printf_register(serial_putchar);
	
	//printf("waiting...");

	// Wait a short while
	for(i = 0; i < 10000; ++i)
		asm volatile ("nop");

	//printf("configuring...");

	i2c_init();
	//silent = 1;
	//cmdCfgAll(1, argv);
	//silent = 0;

	//printf("done.\n");

	while (1)
	{
		printf_P(PSTR("\n> "));
		argc = readcmd(argv, 12);
		printf_P(PSTR("\n"));

		if(argc > 0)
		{
			if(strcmp(argv[0], "?") == 0)
			{
				for(i = 0; i < sizeof(cmds) / sizeof(*cmds); ++i)
					printf_P(PSTR("%s %S\n"), cmds[i].cmd, cmds[i].usage);
				continue;
			}

			for(i = 0; i < sizeof(cmds) / sizeof(*cmds); ++i)
			{
				if(strcmp(argv[0], cmds[i].cmd) == 0)
				{
					if(argc == 2 && strcmp(argv[1], "?") == 0)
						printf_P(PSTR("%s %S\n"), cmds[i].cmd, cmds[i].usage);
					else
						cmds[i].handler(argc, argv);
					break;
				}
			}

			if(i >= sizeof(cmds) / sizeof(*cmds))
				printf("err\n");
		}

	}
}
