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


void setColour(uint8_t index, uint8_t row, uint32_t r, uint32_t g, uint32_t b)
{
	uint8_t* address = AMBILIGHT_BASE_ADDR_COLOUR;
	address += (uint16_t)index * 32;
	address += row * 8;

	//       54                45                36                27                18                 9                 0
	// +-/\+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |       | Bint            | Bfract          | Gint            | Gfract          | Rint            | Rfract          |
	// +-\/+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |   |   byte 6      |    byte 5     |    byte 4     |    byte 3     |    byte 2     |    byte 1     |    byte 0     |

	address[0] = address[1] = address[2] = address[3] = address[4] = address[5] = address[6] = address[7] = 0;
	*((uint32_t*)(address + 0))  = (r & 0x3FFFFUL);
	*((uint32_t*)(address + 2)) |= (g & 0x3FFFFUL) << 2;
	*((uint32_t*)(address + 4)) |= (b & 0x3FFFFUL) << 4;
}

void getColour(uint8_t index, uint8_t row, uint32_t* r, uint32_t* g, uint32_t* b)
{
	uint8_t* address = AMBILIGHT_BASE_ADDR_COLOUR;
	address += (uint16_t)index * 32;
	address += row * 8;
	
	*r = *((uint32_t*)(address + 0)) & 0x3FFFFUL;
	*g = (*((uint32_t*)(address + 2)) >> 2) & 0x3FFFFUL;
	*b = (*((uint32_t*)(address + 4)) >> 4) & 0x3FFFFUL;
}

void cmdSetColour(uint8_t argc, char** argv)
{
	if(argc == 6)
	{
		uint32_t r, g, b;
		uint8_t index, maxIndex;
		uint8_t row, minRow, maxRow;

		r = getfixed_9_9(argv[3]);
		g = getfixed_9_9(argv[4]);
		b = getfixed_9_9(argv[5]);
		
		getrange(argv[1], &index, &maxIndex);
		getrange(argv[2], &minRow, &maxRow);
		do
		{
			row = minRow;
			do
			{
				setColour(index, row, r, g, b);
				
			} while(row++ < maxRow);
			
		} while(index++ < maxIndex);
	}
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
				uint32_t r, g, b;

				getColour(index, row, &r, &g, &b);

				printf_P(PSTR("%d %d: %d.%03d %d.%03d %d.%03d\n"), index, row, 
				         fixed_9_9_int(r), fixed_9_9_fract(r, 3),
				         fixed_9_9_int(g), fixed_9_9_fract(g, 3),
				         fixed_9_9_int(b), fixed_9_9_fract(b, 3));
				
			} while(row++ < maxRow);
			
		} while(index++ < maxIndex);
	}
}

void cmdRstColour(uint8_t argc, char** argv)
{
	int i;
	for(i = 0; i < 8; ++i)
	{
		setColour(i, 0, 0x200, 0,     0);
		setColour(i, 1, 0,     0x200, 0);
		setColour(i, 2, 0,     0,     0x200);
		setColour(i, 3, 0,     0,     0);
	}
}

