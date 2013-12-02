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

#define AMBILIGHT_LIGHT     _SFR_IO8(0x26)
#define AMBILIGHT_COMPONENT _SFR_IO8(0x27)
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
					unsigned char len;
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

char strtoint(char* str)
{
	char x;

	for(x = 0; *str != '\0'; ++str)
	{
		x *= 10;
		x += (*str) - '0';
	}

	return x;
}
	
void doSet(char** argv, char argc)
{
	if(argc == 7)
	{
		AMBILIGHT_LIGHT = strtoint(argv[1]);
		AMBILIGHT_COMPONENT = 0;
		AMBILIGHT_DATA = strtoint(argv[2]);
		AMBILIGHT_COMPONENT = 1;
		AMBILIGHT_DATA = strtoint(argv[3]);
		AMBILIGHT_COMPONENT = 2;
		AMBILIGHT_DATA = strtoint(argv[4]);
		AMBILIGHT_COMPONENT = 3;
		AMBILIGHT_DATA = strtoint(argv[5]);
		AMBILIGHT_COMPONENT = 4;
		AMBILIGHT_DATA = strtoint(argv[6]);
		printf("OK\n");
	}
	else
		printf("err: light xmin xmax ymin ymax shift\n");
}

void doGet(char** argv, char argc)
{
	if(argc == 2)
	{
		unsigned int light = strtoint(argv[1]);
		unsigned int xmin;
		unsigned int xmax;
		unsigned int ymin;
		unsigned int ymax;
		unsigned int shift;
		AMBILIGHT_LIGHT = light;
		AMBILIGHT_COMPONENT = 0;
		asm("nop");
		xmin = AMBILIGHT_DATA;
		AMBILIGHT_COMPONENT = 1;
		asm("nop");
		xmax = AMBILIGHT_DATA;
		AMBILIGHT_COMPONENT = 2;
		asm("nop");
		ymin = AMBILIGHT_DATA;
		AMBILIGHT_COMPONENT = 3;
		asm("nop");
		ymax = AMBILIGHT_DATA;
		AMBILIGHT_COMPONENT = 4;
		asm("nop");
		shift = AMBILIGHT_DATA;
		printf("%d: %d %d %d %d %d\n", light, xmin, xmax, ymin, ymax, shift);
	}
	else
		printf("err: light\n");
}

void doStatus(char** argv, char argc)
{
	unsigned int status;
	AMBILIGHT_COMPONENT = 15;
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
		AMBILIGHT_LIGHT = light;
		AMBILIGHT_COMPONENT = 15;
		asm("nop");
		do { cr = AMBILIGHT_DATA; } while((cr & 1) == 0);
		AMBILIGHT_COMPONENT = 8;
		asm("nop");
		cr = AMBILIGHT_DATA;
		AMBILIGHT_COMPONENT = 9;
		asm("nop");
		cb = AMBILIGHT_DATA;
		AMBILIGHT_COMPONENT = 10;
		asm("nop");
		y = AMBILIGHT_DATA;
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
	struct ConfigTable* p;
	int i;
	int silent = (argc == 1 && argv[0][0] == '1' && argv[0][1] == '\0');

	for(i = 0; pgm_read_dword(&g_lightTable[i]) != 0xffffffff; ++i)
	{
		unsigned long light = pgm_read_dword(&g_lightTable[i]);
		AMBILIGHT_LIGHT = i;
		AMBILIGHT_COMPONENT = 0;
		AMBILIGHT_DATA = LIGHT_XMIN(light);
		AMBILIGHT_COMPONENT = 1;
		AMBILIGHT_DATA = LIGHT_XMAX(light);
		AMBILIGHT_COMPONENT = 2;
		AMBILIGHT_DATA = LIGHT_YMIN(light);
		AMBILIGHT_COMPONENT = 3;
		AMBILIGHT_DATA = LIGHT_YMAX(light);
		AMBILIGHT_COMPONENT = 4;
		AMBILIGHT_DATA = LIGHT_SHIFT(light);
		AMBILIGHT_COMPONENT = 5;
		AMBILIGHT_DATA = LIGHT_OUTPUT(light);
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
	
	printf("waiting...");

	// Wait a short while
	for(i = 0; i < 10000; ++i)
		asm volatile ("nop");

	printf("configuring...");

	i2c_init();
	argv[0] = "1";
	argc = 1;
	doConfig(argv, argc);

	printf("done.\n");

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
