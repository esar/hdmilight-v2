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

#include "config_lights.h"


void setLight(unsigned int light, 
              unsigned char xmin, unsigned char xmax,
              unsigned char ymin, unsigned char ymax,
              unsigned char shift, unsigned char output)
{
	uint8_t* address = AMBILIGHT_BASE_ADDR_AREA;
	address += (uint16_t)light * 4;

	//  31      28      24          18          12          6           0
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// | | out   | shift | ymax      | ymin      | xmax      | xmin      |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |                 |               |               |               |

	light <<= 2;
	address[0] = (xmin & 0x3f) | (xmax << 6);
	address[1] = ((xmax & 0x3f) >> 2) | (ymin << 4);
	address[2] = ((ymin & 0x3f) >> 4) | (ymax << 2);
	address[3] = (shift & 0xf) | (output << 4);
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

void cmdGetLight(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		uint8_t index, maxIndex;

		getrange(argv[1], &index, &maxIndex);
		do
		{
			uint8_t* address = AMBILIGHT_BASE_ADDR_AREA + ((uint16_t)index * 4);
			int x;

			x = address[0] & 0x3f; // xmin
			printf_P(PSTR("%d: %d "), index, x);
			x = (address[0] >> 6) | ((address[1] & 0x0f) << 2); // xmax
			printf_P(PSTR("%d "), x);
			x = (address[1] >> 4) | ((address[2] & 3) << 4); // ymin
			printf_P(PSTR("%d "), x);
			x = (address[2] >> 2); // ymax
			printf_P(PSTR("%d "), x);
			x = address[3] & 0xf; // shift
			printf_P(PSTR("%d "), x);
			x = address[3] >> 4; // output
			printf_P(PSTR("%d\n"), x);

		} while(index++ < maxIndex);
	}
	//else
		//printf("err: GL light\n");
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

