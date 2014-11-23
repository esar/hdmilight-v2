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
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "ambilight.h"
#include "serial.h"
#include "i2c.h"

#include "config_hdmi.h"
#include "config_lights.h"


#define BELL    '\a'


uint8_t silent = 0;
volatile uint8_t g_formatChanged = 0;

ISR(_VECTOR(1))
{
	g_formatChanged = 1;
}

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
		{
			if(g_formatChanged)
			{
				changeFormat();
				g_formatChanged = 0;
			}
			continue;
		}

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
	uint16_t x = 0;

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
char cmdSetDelayUsage[] PROGMEM = "num_frames num_ticks smooth_ratio";
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
char cmdGetFlashUsage[] PROGMEM = "src dst len";

#define DMA_FLASH_ADDR_H    _SFR_IO8(0x2c)
#define DMA_FLASH_ADDR_M    _SFR_IO8(0x2d)
#define DMA_FLASH_ADDR_L    _SFR_IO8(0x2e)
#define DMA_SRAM_ADDR_H     _SFR_IO8(0x2f)
#define DMA_SRAM_ADDR_L     _SFR_IO8(0x30)
#define DMA_LEN_H           _SFR_IO8(0x31)
#define DMA_LEN_L           _SFR_IO8(0x32)
#define DMA_START           _SFR_IO8(0x33)

void dmaRead(uint8_t section, uint16_t src, uint16_t dst, uint16_t len)
{
	section += 6;
	if(section < 6)
		section = 6;

	DMA_FLASH_ADDR_H = section;
	DMA_FLASH_ADDR_M = src >> 8;
	DMA_FLASH_ADDR_L = src & 0xff;
	DMA_SRAM_ADDR_H = dst >> 8;
	DMA_SRAM_ADDR_L = dst & 0xff;
	DMA_LEN_H = len >> 8;
	DMA_LEN_L = len & 0xff;
	DMA_START = 0;
}

void cmdGetFlash(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		int section = getint(&argv[1]);

		printf_P(PSTR("loading config...\n"));
		dmaRead(section, 0, 0x8000, 0x8000);
		printf("done.\n");
	}
	else if(argc == 4)
	{
		uint16_t src = getint(&argv[1]);
		uint16_t dst = getint(&argv[2]);
		uint16_t len = getint(&argv[3]);


		if(dst != 0)
		{
			printf_P(PSTR("copying %04x bytes from %04x to %04x...\n"), len, src, dst);
			dmaRead(0, src, dst, len);
		}
		else
		{
			static uint8_t buf[18];
			uint16_t pos;

			printf_P(PSTR("dumping %04x bytes from %04x...\n"), len, src);

			dst = (uint16_t)(&buf);
			for(pos = 0; pos < len; pos += 16, src += 16)
			{
				int i;

				dmaRead(0, src, dst, 16);

				printf("%04x ", pos);
				for(i = 0; i < 16; ++i)
					printf("%02x ", (unsigned int)buf[i]);
				printf("\n");
			}
		}

		printf("\ndone.\n");
	}
}

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
		{ "GF", cmdGetFlash,  cmdGetFlashUsage  },
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
		{ "GX", cmdGetFormat, cmdBlankUsage     },
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

	sei();

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

