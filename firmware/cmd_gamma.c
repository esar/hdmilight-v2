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

	
void setGamma(uint8_t channel, uint8_t table, uint8_t index, uint8_t value)
{
	uint8_t* address = AMBILIGHT_BASE_ADDR_GAMMAR;

	if(channel == 0)
		address = AMBILIGHT_BASE_ADDR_GAMMAR;
	else if(channel == 1)
		address = AMBILIGHT_BASE_ADDR_GAMMAG;
	else if(channel == 2)
		address = AMBILIGHT_BASE_ADDR_GAMMAB;

	address += (uint16_t)table * 256;
	address += index;

	*address = value;
}

void cmdGetGamma(uint8_t argc, char** argv)
{
	if(argc == 4)
	{
		uint8_t table, maxTable;
		uint8_t channel, minChannel, maxChannel;
		uint8_t index, minIndex, maxIndex;

		getrange(argv[1], &table, &maxTable);
		getrange(argv[2], &minChannel, &maxChannel);
		getrange(argv[3], &minIndex, &maxIndex);
		do
		{
			channel = minChannel;
			do
			{
				index = minIndex;
				do
				{
					int value;
					uint8_t* address = AMBILIGHT_BASE_ADDR_GAMMAR;
					if(channel == 0)
						address = AMBILIGHT_BASE_ADDR_GAMMAR;
					else if(channel == 1)
						address = AMBILIGHT_BASE_ADDR_GAMMAG;
					else if(channel == 2)
						address = AMBILIGHT_BASE_ADDR_GAMMAB;
					address += (uint16_t)table * 256;
					address += index;

					value = *address;

					printf_P(PSTR("%d: %d: %d: %d\n"), table, channel, index, value);

				} while(index++ < maxIndex);
				
			} while(channel++ < maxChannel);
			
		} while(table++ < maxTable);
	}
}

void cmdSetGamma(uint8_t argc, char** argv)
{
	if(argc == 9)
	{
		uint8_t value;
		uint8_t table, maxTable;
		uint8_t channel, minChannel, maxChannel;
		uint8_t index, minIndex, maxIndex;

		value = getint(&argv[4]);
		
		getrange(argv[1], &table, &maxTable);
		getrange(argv[2], &minChannel, &maxChannel);
		getrange(argv[3], &minIndex, &maxIndex);
		do
		{
			channel = minChannel;
			do
			{
				index = minIndex;
				do
				{
					setGamma(channel, table, index, value);

				} while(index++ < maxIndex);
				
			} while(channel++ < maxChannel);
			
		} while(table++ < maxTable);
	}
}

void cmdRstGamma(uint8_t argc, char** argv)
{
	int table;
	for(table = 0; table < 8; ++table)
	{
		int channel;
		for(channel = 0; channel < 3; ++channel)
		{
			int index;
			for(index = 0; index < 256; ++index)
				setGamma(channel, table, index, index);
		}
	}
}

