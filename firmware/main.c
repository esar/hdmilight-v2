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


// If this line is commented out, the ADV7611 will not be started
// at power on and HDMI handshaking will not occur, in which case "R"
// needs to be run from the serial console to start the ADV7611
#define AUTO_INITIALIZATION

// Uncomment to enable power on/off via physical switch.
// Requires a pull-up resistor and switch to be added.
//#define ENABLE_POWER_SWITCH

#define BELL    '\a'


uint8_t silent = 0;
volatile uint8_t g_formatChanged = 0;
volatile uint8_t g_cecMessage[16];
volatile uint8_t g_cecMessageLength;


ISR(_VECTOR(1))
{
	g_formatChanged = 1;
	INT_CLEAR = 1;
}

ISR(_VECTOR(2))
{
	uint8_t cecRxIntState = i2cRead(0x98, 0x93);

	// if this is a CEC RX message 0 ready interupt
	if(cecRxIntState & 8)
	{
		int i;

		// copy the received message
		g_cecMessageLength = i2cRead(0x80, 0x25);
		if(g_cecMessageLength > sizeof(g_cecMessage))
			g_cecMessageLength = sizeof(g_cecMessage);
		for(i = 0; i < g_cecMessageLength; ++i)
			g_cecMessage[i] = i2cRead(0x80, 0x15 + i);

		// clear the message buffer ready for a new message
		i2cWrite(0x80, 0x2C, 0x02);

		// clear the interrupt
		i2cWrite(0x98, 0x94, 0x08);
	}
}

void idle()
{
#ifdef ENABLE_POWER_SWITCH
	static uint8_t debounceValue = 0;
	static uint8_t debounceTicks = 0;
#endif // ENABLE_POWER_SWITCH

	if(g_formatChanged)
	{
		changeFormat();
		g_formatChanged = 0;
	}

	if(g_cecMessageLength != 0)
	{
		cli();
		processCecMessage();
		g_cecMessageLength = 0;
		sei();
	}

#ifdef ENABLE_POWER_SWITCH
	if((PIND & (1 << GPIO_POWER_PIN)) == debounceValue)
	{
		if(debounceTicks == DEBOUNCE_TICK_COUNT)
		{
			if(debounceValue)
				powerOn();
			else
				powerOff();
			++debounceTicks;
		}
		else if(debounceTicks < DEBOUNCE_TICK_COUNT)
			++debounceTicks;
	}
	else
	{
		debounceValue = PIND & (1 << GPIO_POWER_PIN);
		debounceTicks = 0;
	}
#endif // ENABLE_POWER_SWITCH
}

// Print a section of the ring buffer to stdout, wrapping around when passing the end.
// Also replacing NULLs with spaces as old commands have had spaces replaced with NULLs
// when they were being split into args
void ringBufPrint(char* buf, uint8_t pos, int len)
{
	while(len--)
	{
		if(buf[pos] == '\0')
			fputc(' ', stdout);
		else
			fputc(buf[pos], stdout);
		++pos;
	}
}

void ringBufCopy(char* buf, uint8_t dst, uint8_t src, int len)
{
	while(len--)
		buf[dst++] = buf[src++];
}

void ringBufCopyReverse(char* buf, uint8_t dst, uint8_t src, int len)
{
	dst += len;
	src += len;
	while(len--)
		buf[--dst] = buf[--src];
}

char readcmd(char** argv, char maxargs)
{
	static char cmdbuf[256] = {0, 0};
	static unsigned char current = 0;

	unsigned char history = 0;
	unsigned char pos = current + 1;
	unsigned char end = current + 1;
	unsigned char arg;


	while(1)
	{
		int c = serial_getchar();
		if(c == -1)
		{
			idle();
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
					// move partial line left in buffer to fill hole
					ringBufCopy(cmdbuf, pos - 1, pos, end - pos);
					--pos; --end;

					// move cursor left one place and print the partial line
					printf_P(PSTR("\x1B[D"));
					ringBufPrint(cmdbuf, pos, end - pos);

					// move the cursor left, back to the edit position
					printf_P(PSTR(" \x1B[%dD"), end - pos + 1);
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

						// lines in the history are NULL terminated, remove the termination
						--len;

						// copy the old line into the current position in the buffer
						ringBufCopy(cmdbuf, current + 1, p + 1, len - 1);
						cmdbuf[current + len] = '\0';

						// move the cursor to the beginning of the line and print the copied line
						printf_P(PSTR(" \x1B[%dD"), pos - current);
						ringBufPrint(cmdbuf, current + 1, len - 1);

						// if the copied line is shorter then overwrite any extra characters with spaces
						if(end - (current + len) > 0)
						{
							for(i = 0; i < end - (current + len); ++i)
								serial_putchar(' ');
							printf_P(PSTR("\x1B[%dD"), end - (current + len));
						}
						end = current + len;
						pos = end;
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

					// the lines in the history are NULL terminated, remove the termination
					--len;

					// copy the old line to the current position in the buffer
					ringBufCopy(cmdbuf, current + 1, p + 1, len - 1);
					cmdbuf[current + len] = '\0';

					// move the cursor to the beginning of the line and print the copied line
					printf_P(PSTR(" \x1B[%dD"), pos - current);
					ringBufPrint(cmdbuf, current + 1, len - 1);

					// if the copied line is shorter then overwrite the extra characters with spaces
					if(end - (current + len) > 0)
					{
						for(i = 0; i < end - (current + len); ++i)
							serial_putchar(' ');
						printf_P(PSTR("\x1B[%dD"), end - (current + len));
					}
					end = current + len;
					pos = end;
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
					// copy any characters after the current edit point to the right to make room
					ringBufCopyReverse(cmdbuf, pos + 1, pos, end - pos);

					// add the new character
					cmdbuf[pos] = c;

					// print the buffer from the current edit point to the end
					ringBufPrint(cmdbuf, pos, end - pos + 1);

					// move the cursor back to the edit point
					printf_P(PSTR(" \x1B[%dD"), end - pos + 1);
					++pos;
					++end;

				}
				else
					serial_putchar(BELL);
				break;
		}
	}

DONE:
	// if the current line wraps around the end of the buffer then rotate the
	// buffer so that the line is all in one piece
	if(end < current)
	{
		uint8_t offset = 0x100 - current;
		ringBufCopyReverse(cmdbuf, end + offset, end, 0x100);
		pos += offset;
		end += offset;
		current += offset;
	}

	// NULL terminate the current line
	cmdbuf[end] = '\0';

	// split the line into args
	pos = current + 1;
	for(arg = 0; arg < maxargs; ++arg)
	{
		if(pos == end)
			break; 

		while((cmdbuf[pos] == ' ' || cmdbuf[pos] == '\0') && pos != end)
			cmdbuf[pos++] = '\0';

		argv[arg] = cmdbuf + pos;

		while(cmdbuf[pos] != ' ' && cmdbuf[pos] != '\0' && pos != end)
			++pos;
	}
	
	// account for added NULL termination of the line
	++end;

	// store the length of the line for history navigation
	cmdbuf[end] = end - current;

	// setup ready for the next line
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

void cmdRstAll(uint8_t argc, char** argv)
{
	cmdRstI2C(argc, argv);
	cmdRstArea(argc, argv);
	cmdRstColour(argc, argv);
	cmdRstGamma(argc, argv);
	cmdRstOutput(argc, argv);
	cmdRstDelay(argc, argv);
}

const char cmdBlankUsage[] PROGMEM = "";
const char cmdGetAreaUsage[] PROGMEM   = "Get Area:    GA index";
const char cmdSetAreaUsage[] PROGMEM   = "Set Area:    SA index xmin xmax ymin ymax shift output";
const char cmdRstAreaUsage[] PROGMEM   = "Rst Area:    RA";
const char cmdGetAddrUsage[] PROGMEM   = "Get Address: GX addr count";
const char cmdSetAddrUsage[] PROGMEM   = "Set Address: SX addr byte0 [byte1] [...]";
const char cmdGetColourUsage[] PROGMEM = "Get Colour:  GC index row";
const char cmdSetColourUsage[] PROGMEM = "Set Colour:  SC index row r g b";
const char cmdRstColourUsage[] PROGMEM = "Rst Colour:  RC";
const char cmdGetDelayUsage[] PROGMEM  = "Get Delay:   GD";
const char cmdSetDelayUsage[] PROGMEM  = "Set Delay:   SD num_frames num_ticks smooth_ratio";
const char cmdRstDelayUsage[] PROGMEM  = "Rst Delay:   RD";
const char cmdDisFormatUsage[] PROGMEM = "Dis Format:  DF";
const char cmdEnaFormatUsage[] PROGMEM = "Ena Format:  EF";
const char cmdGetFormatUsage[] PROGMEM = "Get Format:  GF";
const char cmdGetGammaUsage[] PROGMEM  = "Get Gamma:   GG table channel index";
const char cmdSetGammaUsage[] PROGMEM  = "Set Gamma:   SG table channel index value";
const char cmdRstGammaUsage[] PROGMEM  = "Rst Gamma:   RG";
const char cmdGetI2CUsage[] PROGMEM    = "Get I2C:     GI addr sub_addr [bit_range]";
const char cmdSetI2CUsage[] PROGMEM    = "Set I2C:     SI addr sub_addr value";
const char cmdRstI2CUsage[] PROGMEM    = "Rst I2C:     RI";
const char cmdSetKeysUsage[] PROGMEM   = "Set Keys:    SK key_code";
const char cmdRstKeysUsage[] PROGMEM   = "Rst Keys:    RK";
const char cmdGetMemUsage[] PROGMEM    = "Get Memory:  GM index";
const char cmdGetOutputUsage[] PROGMEM = "Get Output:  GO output light";
const char cmdSetOutputUsage[] PROGMEM = "Set Output:  SO output light area coef gamma enable";
const char cmdRstOutputUsage[] PROGMEM = "Rst Output:  RO";
const char cmdGetPortUsage[] PROGMEM   = "Get Port:    GP addr";
const char cmdSetPortUsage[] PROGMEM   = "Set Port:    SP addr value";
const char cmdGetResultUsage[] PROGMEM = "Get Result:  GR index";
const char cmdGetStatusUsage[] PROGMEM = "Get Status:  GS";
const char cmdGetStackUsage[] PROGMEM  = "Get Stack:   GZ";
const char cmdRstAllUsage[] PROGMEM    = "Rst All:     R";

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

	cli();
	DMA_START = 0;
	sei();
}

int main()
{
	const struct
	{
		const char* cmd;
		void (*handler)(uint8_t argc, char** argv);
		PGM_P usage;

	} cmds[] = 
	{
		{ "GA", cmdGetArea,   cmdGetAreaUsage   },
		{ "SA", cmdSetArea,   cmdSetAreaUsage   },
		{ "RA", cmdRstArea,   cmdRstAreaUsage   },
		{ "GC", cmdGetColour, cmdGetColourUsage },
		{ "SC", cmdSetColour, cmdSetColourUsage },
		{ "RC", cmdRstColour, cmdRstColourUsage },
		{ "GD", cmdGetDelay,  cmdGetDelayUsage  },
		{ "SD", cmdSetDelay,  cmdSetDelayUsage  },
		{ "RD", cmdRstDelay,  cmdRstDelayUsage  },
		{ "DF", cmdDisFormat, cmdDisFormatUsage },
		{ "EF", cmdEnaFormat, cmdEnaFormatUsage },
		{ "GF", cmdGetFormat, cmdGetFormatUsage },
		{ "GG", cmdGetGamma,  cmdGetGammaUsage  },
		{ "SG", cmdSetGamma,  cmdSetGammaUsage  },
		{ "RG", cmdRstGamma,  cmdRstGammaUsage  },
		{ "GI", cmdGetI2C,    cmdGetI2CUsage    },
		{ "SI", cmdSetI2C,    cmdSetI2CUsage    },
		{ "RI", cmdRstI2C,    cmdRstI2CUsage    },
		{ "SK", cmdSetKeys,   cmdSetKeysUsage   },
		{ "RK", cmdRstKeys,   cmdRstKeysUsage   },
		{ "GM", cmdGetMem,    cmdGetMemUsage    },
		{ "GO", cmdGetOutput, cmdGetOutputUsage },
		{ "SO", cmdSetOutput, cmdSetOutputUsage },
		{ "RO", cmdRstOutput, cmdRstOutputUsage },
		{ "GP", cmdGetPort,   cmdGetPortUsage   },
		{ "SP", cmdSetPort,   cmdSetPortUsage   },
		{ "GR", cmdGetResult, cmdGetResultUsage },
		{ "GS", cmdGetStatus, cmdGetStatusUsage },
		{ "GX", cmdGetAddr,   cmdGetAddrUsage   },
		{ "SX", cmdSetAddr,   cmdSetAddrUsage   },
		{ "GZ", cmdGetStack,  cmdGetStackUsage  },
		{ "R",  cmdRstAll,    cmdRstAllUsage    },
	};

	int i;
	char* argv[12];
	int argc;

	serial_init();
	
	// Wait a short while
	for(i = 0; i < 10000; ++i)
		asm volatile ("nop");

	i2c_init();

#ifdef AUTO_INITIALIZATION
	silent = 1;
	argv[0] = "R";
	cmdRstAll(1, argv);
	silent = 0;
#endif

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
					printf_P(PSTR("%S\n"), cmds[i].usage);
				continue;
			}

			for(i = 0; i < sizeof(cmds) / sizeof(*cmds); ++i)
			{
				if(strcmp(argv[0], cmds[i].cmd) == 0)
				{
					if(argc == 2 && strcmp(argv[1], "?") == 0)
						printf_P(PSTR("%S\n"), cmds[i].usage);
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

