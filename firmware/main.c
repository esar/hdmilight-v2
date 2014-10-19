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
#include <string.h>
#include "serial.h"
#include "printf.h"
#include "i2c.h"

#include "config_hdmi.h"
#include "config_lights.h"


#define BELL    '\a'

#define AMBILIGHT_ADDR_LOW  _SFR_IO8(0x26)
#define AMBILIGHT_ADDR_HIGH _SFR_IO8(0x27)
#define AMBILIGHT_DATA      _SFR_IO8(0x28)

/*
struct _ConfigData
{
	unsigned char address;
	unsigned char subaddress;
	unsigned char data;

} g_configData[] = 
{
	//{ 0x98, 0xFF, 0x80 },
	{ 0x98, 0xFD, 0x44 },
	{ 0x98, 0x00, 0x0B }, //0x1E },
	{ 0x98, 0x01, 0x05 },
	{ 0x98, 0x03, 0x42 },
	{ 0x98, 0x05, 0x28 },
	{ 0x98, 0x0B, 0x44 },
	{ 0x98, 0x0C, 0x42 },
	{ 0x98, 0x14, 0x7F },
	{ 0x98, 0x15, 0x80 },
	{ 0x98, 0x19, 0x83 },
	{ 0x98, 0x33, 0x40 },
	{ 0x44, 0xBA, 0x01 },
	{ 0x44, 0xBF, 0x17 },
	{ 0x44, 0xC0, 0x00 },
	{ 0x44, 0xC1, 0x00 },
	{ 0x44, 0xC2, 0xA0 },
	{ 0x44, 0xC9, 0x05 },
	{ 0x00 }
};
*/

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
					printf("\x1B[D"); // move left
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
					printf("\x1B[D%s \x1B[%dD", cmdbuf + pos, end - pos); // move left
				}
				else
					serial_putchar(BELL);
				break;
			RIGHT:
			case 0x06: // right
				if(pos + 1 != end)
				{
					++pos;
					printf("\x1B[C"); // move right
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
						printf(" \x1B[%dD%s", pos - current, cmdbuf + current + 1);
						if(end - (current + len) > 0)
							printf("%*s\x1B[%dD", end - (current + len), "", end - (current + len));
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
					printf(" \x1B[%dD%s", pos - current, cmdbuf + current + 1);
					if(end - (current + len) > 0)
						printf("%*s\x1B[%dD", end - (current + len), "", end - (current + len));
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
					printf("%s \x1B[%dD", cmdbuf + pos, end - pos);
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

/*
char readcmd(char** argv, char maxargs)
{
	static char cmdbuf[32];
	int i, arg;

	i = 0;
	while(1)
	{
		int c = serial_getchar();
		if(c == -1)
			continue;

		serial_putchar(c);
		if(c == '\n' || c == '\r')
			break;

		cmdbuf[i++] = c;
		if(i >= sizeof(cmdbuf))
			i = sizeof(cmdbuf) - 1;
	}
	cmdbuf[i] = '\0';

	i = 0;
	for(arg = 0; arg < maxargs; ++arg)
	{
		while(cmdbuf[i] == ' ')
			cmdbuf[i++] = '\0';

		if(cmdbuf[i] == '\0')
			break;

		argv[arg] = cmdbuf + i;

		while(cmdbuf[i] != ' ' && cmdbuf[i] != '\0')
			++i;
	}
		
	return arg;
}
*/

unsigned char strtoint(char* str)
{
	unsigned char x;

	for(x = 0; *str != '\0'; ++str)
	{
		x *= 10;
		x += (*str) - '0';
	}

	return x;
}
	
void setLight(unsigned int light, 
              unsigned char xmin, unsigned char xmax,
              unsigned char ymin, unsigned char ymax,
              unsigned char shift, unsigned char output)
{
	//  31      28      24          18          12          6           0
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// | | out   | shift | ymax      | ymin      | xmax      | xmin      |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |                 |               |               |               |

	light <<= 2;
	AMBILIGHT_ADDR_HIGH = light >> 8;
	light &= 0xff;
	AMBILIGHT_ADDR_LOW = light;
	AMBILIGHT_DATA = (xmin & 0x3f) | (xmax << 6);
	AMBILIGHT_ADDR_LOW = light + 1;
	AMBILIGHT_DATA = ((xmax & 0x3f) >> 2) | (ymin << 4);
	AMBILIGHT_ADDR_LOW = light + 2;
	AMBILIGHT_DATA = ((ymin & 0x3f) >> 4) | (ymax << 2);
	AMBILIGHT_ADDR_LOW = light + 3;
	AMBILIGHT_DATA = (shift & 0xf) | (output << 4);
}

void getLight(unsigned int light,
              unsigned char* xmin, unsigned char* xmax,
              unsigned char* ymin, unsigned char* ymax,
              unsigned char* shift, unsigned char* output)
{
	unsigned char a, b, c, d;

	light <<= 2;
	AMBILIGHT_ADDR_HIGH = light >> 8;
	light &= 0xff;
	AMBILIGHT_ADDR_LOW = light;
	asm("nop");
	a = AMBILIGHT_DATA;
	AMBILIGHT_ADDR_LOW = light + 1;
	asm("nop");
	b = AMBILIGHT_DATA;
	AMBILIGHT_ADDR_LOW = light + 2;
	asm("nop");
	c = AMBILIGHT_DATA;
	AMBILIGHT_ADDR_LOW = light + 3;
	asm("nop");
	d = AMBILIGHT_DATA;

	*xmin = a & 0x3f;
	*xmax = (a >> 6) | ((b & 0x0f) << 2);
	*ymin = (b >> 4) | ((c & 3) << 4);
	*ymax = c >> 2;
	*shift = d & 0xf;
	*output = d >> 4;
}

void doSet(char** argv, char argc)
{
	if(argc == 8)
	{
		setLight(strtoint(argv[1]),
		         strtoint(argv[2]),
		         strtoint(argv[3]),
		         strtoint(argv[4]),
		         strtoint(argv[5]),
		         strtoint(argv[6]),
		         strtoint(argv[7]));
		printf("OK\n");
	}
	else
		printf("err: light xmin xmax ymin ymax shift output\n");
}

void doDelay(char** argv, char argc)
{
	if(argc == 2)
	{
		if(argv[1][0] == 'S')
		{
			AMBILIGHT_ADDR_HIGH = 0x08;
			AMBILIGHT_ADDR_LOW = 0x00;
			AMBILIGHT_DATA = strtoint(argv[1]);
		}
		else if(argv[1][0] == 'G')
		{
			unsigned int val;
			AMBILIGHT_ADDR_HIGH = 0x08;
			AMBILIGHT_ADDR_LOW = 0x00;
			asm("nop");
			val = AMBILIGHT_DATA;
			printf("%d\n", val);
		}
	}
	else
		printf("err: [G|S] num_frames\n");
}

void doGet(char** argv, char argc)
{
	if(argc == 2)
	{
		unsigned int light = strtoint(argv[1]);
		unsigned char xmin;
		unsigned char xmax;
		unsigned char ymin;
		unsigned char ymax;
		unsigned char shift;
		unsigned char output;

		getLight(light, &xmin, &xmax, &ymin, &ymax, &shift, &output);
		printf("%d: %d %d %d %d %d %d\n", light, xmin, xmax, ymin, ymax, shift, output);
	}
	else
		printf("err: light\n");
}

void doStatus(char** argv, char argc)
{
	unsigned int status;
	AMBILIGHT_ADDR_HIGH = 0x18;
	AMBILIGHT_ADDR_LOW = 0;
	asm("nop");
	status = AMBILIGHT_DATA;
	printf("status: %d\n", status);
}

void doResult(char** argv, char argc)
{
	unsigned int light;
	unsigned int repeat;
	unsigned int i;

	if(argc == 2)
	{
		light = strtoint(argv[1]);
		repeat = 1;
	}
	else if(argc == 3)
	{
		light = strtoint(argv[1]);
		repeat = strtoint(argv[2]);
	}
	else
	{
		printf("err: light\n");
		return;
	}

	for(i = 0; i < repeat; ++i)
	{
		unsigned int cr;
		unsigned int cb;
		unsigned int y;
		AMBILIGHT_ADDR_HIGH = 0x18;
		AMBILIGHT_ADDR_LOW = 0x00;
		asm("nop");
		do { cr = AMBILIGHT_DATA; } while((cr & 1) == 0);

		light <<= 2;
		AMBILIGHT_ADDR_HIGH = 0x10 | (light >> 8); 
		AMBILIGHT_ADDR_LOW = light;
		asm("nop");
		cr = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = light + 1;
		asm("nop");
		cb = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = light + 2;
		asm("nop");
		y = AMBILIGHT_DATA;
		light >>= 2;

		printf("%d: %d %d %d\n", light, cr, cb, y);
	}
}

void doWrite(char** argv, char argc)
{
	if(argc == 3)
	{
		unsigned char addr = strtoint(argv[1]);
		unsigned char val  = strtoint(argv[2]);
		_SFR_IO8(addr) = val;
		printf("OK\n");
	}
	else
		printf("err: addr value\n");
}

void doRead(char** argv, char argc)
{
	if(argc == 2)
	{
		unsigned char addr = strtoint(argv[1]);
		unsigned char val = _SFR_IO8(addr);
		printf("%d=%d\n", (int)addr, (int)val);
	}
	else
		printf("err: addr\n");
}

void doConfig(char** argv, char argc)
{
	const struct ConfigTable* p;
	int i;
	int silent = (argc == 1 && argv[0][0] == '1' && argv[0][1] == '\0');

	for(i = 0; pgm_read_dword(&g_lightTable[i]) != 0xffffffff; ++i)
	{
		unsigned long light = pgm_read_dword(&g_lightTable[i]);
		setLight(i, 
		         LIGHT_XMIN(light), LIGHT_XMAX(light),
		         LIGHT_YMIN(light), LIGHT_YMIN(light),
		         LIGHT_SHIFT(light), LIGHT_OUTPUT(light));
		if(!silent)
			printf("OK\n");
	}

	for(p = g_configTable; pgm_read_byte(&p->address) != 0; ++p)
	{
		unsigned char address = pgm_read_byte(&p->address);
		unsigned char subaddress = pgm_read_byte(&p->subaddress);
		unsigned char data = pgm_read_byte(&p->data);

		if(!silent)
		{
			printf("%d %d %d : ", address, subaddress, data);
			i2c_start();
			printf("%s ", i2c_write(address)    ? "ACK" : "NACK");
			printf("%s ", i2c_write(subaddress) ? "ACK" : "NACK");
			printf("%s\n", i2c_write(data)      ? "ACK" : "NACK");
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

void doI2CWrite(char** argv, char argc)
{
	if(argc == 3)
	{
		i2c_start();
		i2c_write(0x98);
		i2c_write(strtoint(argv[1]));
		i2c_write(strtoint(argv[2]));
		i2c_stop();
		printf("OK\n");
	}
	else
		printf("err: addr1 val1 addr2 val2\n");
}

void doI2CRead(char** argv, char argc)
{
	if(argc == 2)
	{
		int val;
		i2c_start();
		i2c_write(0x98);
		i2c_write(strtoint(argv[1]));
		i2c_start();
		i2c_write(0x99);
		val = i2c_read(0);
		i2c_stop();

		printf("Read: %d\n", val);
	}
	else
		printf("err: addr1 val1 addr2\n");
}

int main()
{
	int i;
	char* argv[8];
	int argc;

	printf_register(serial_putchar);
	
	//printf("waiting...");

	// Wait a short while
	for(i = 0; i < 10000; ++i)
		asm volatile ("nop");

	//printf("configuring...");

	//i2c_init();
	//argv[0] = "1";
	//argc = 1;
	//doConfig(argv, argc);

	//printf("done.\n");

	while (1)
	{
		printf("\n> ");

		argc = readcmd(argv, 8);
		printf("\r\n> ");

		if(argc > 0)
		{
			switch(argv[0][0])
			{
				case 'S':
					doSet(argv, argc);
					break;
				case 'G':
					doGet(argv, argc);
					break;
				case 'R':
					doResult(argv, argc);
					break;
				case 'W':
					doWrite(argv, argc);
					break;
				case 'X':
					doRead(argv, argc);
					break;
				case 'Z':
					doStatus(argv, argc);
					break;
				case 'I':
					switch(argv[0][1])
					{
						case 'I':
							i2c_init();
							break;
						case 'W':
							doI2CWrite(argv, argc);
							break;
						case 'R':
							doI2CRead(argv, argc);
							break;
						case 'C':
							doConfig(argv, argc);
							break;
					}
					break;
			}
		}

	}
}
