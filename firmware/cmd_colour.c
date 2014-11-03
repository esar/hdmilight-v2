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

