/*

   Copyright (C) 2014 Stephen Robinson
  
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
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "ambilight.h"


void cmdGetMem(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		uint32_t config = getint(&argv[1]);
		config += 1;
		config *= 0x8000;

		printf_P(PSTR("loading config...\n"));
		dmaRead(config >> 16, config & 0xffff, 0x8000, 0x8000);
		printf("done.\n");
	}
	else if(argc == 5)
	{
		uint16_t sec = getint(&argv[1]);
		uint16_t src = getint(&argv[2]);
		uint16_t dst = getint(&argv[3]);
		uint16_t len = getint(&argv[4]);


		if(dst != 0)
		{
			printf_P(PSTR("copying %04x bytes from %04x%04x to %04x...\n"), len, sec, src, dst);
			dmaRead(sec, src, dst, len);
		}
		else
		{
			static uint8_t buf[18];
			uint16_t pos;

			printf_P(PSTR("dumping %04x%04x bytes from %04x...\n"), sec, len, src);

			dst = (uint16_t)(&buf);
			for(pos = 0; pos < len; pos += 16, src += 16)
			{
				int i;

				dmaRead(sec, src, dst, 16);

				printf("%04x ", pos);
				for(i = 0; i < 16; ++i)
					printf("%02x ", (unsigned int)buf[i]);
				printf("\n");
			}
		}

		printf("\ndone.\n");
	}
}
