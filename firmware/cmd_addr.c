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
	if(argc == 3)
	{
		uint8_t* addr = (uint8_t*)getint(&argv[1]) + 0x8000;
		uint8_t  count = getint(&argv[2]);

		while(count--)
			printf_P(PSTR("%02x "), *addr++);
	}
}

void cmdSetAddr(uint8_t argc, char** argv)
{
	if(argc > 2)
	{
		uint8_t* address = (uint8_t*)getint(&argv[1]) + 0x8000;

		argv += 2;
		argc -= 2;
		while(argc > 0)
		{
			*address++ = getint(&argv[0]);
			++argv;
			--argc;
		}
	}
}

