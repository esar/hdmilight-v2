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


void cmdGetDelay(uint8_t argc, char** argv)
{
	if(argc == 1)
	{
		int frames;
		uint32_t ticks;
		uint32_t temporalSmoothingRatio;
		uint8_t* address = (uint8_t*)AMBILIGHT_BASE_ADDR_DELAY;
		frames = address[0];
		ticks  = address[1];
		ticks  = (ticks << 8) | address[2]; 
		ticks  = (ticks << 8) | address[3];
		ticks /= 16;
		temporalSmoothingRatio = address[4];
		temporalSmoothingRatio = (temporalSmoothingRatio << 8) | address[5];

		printf_P(PSTR("%d %ld %d.%03d\n"), frames, ticks, 
		         fixed_9_9_int(temporalSmoothingRatio), fixed_9_9_fract(temporalSmoothingRatio, 3));
	}
}

void cmdSetDelay(uint8_t argc, char** argv)
{
	if(argc == 4)
	{
		uint8_t frames = getint(&argv[1]);
		uint32_t ticks = getint(&argv[2]);
		uint32_t temporalSmoothingRatio = getfixed_9_9(argv[3]);
		uint8_t* address = (uint8_t*)AMBILIGHT_BASE_ADDR_DELAY;

		address[0] = frames;

		ticks *= 16;
		address[1] = 0;
		address[2] = ticks >> 8;
		address[3] = ticks & 0xff;

		address[4] = temporalSmoothingRatio >> 8;
		address[5] = temporalSmoothingRatio & 0xff;
	}
}

void cmdRstDelay(uint8_t argc, char** argv)
{
	memset(AMBILIGHT_BASE_ADDR_DELAY, 0, 8); 
}

