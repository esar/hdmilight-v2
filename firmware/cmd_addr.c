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
#include "ambilight.h"


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

