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
		uint16_t temporalSmoothingRatio;
		uint16_t address = AMBILIGHT_BASE_ADDR_DELAY;
		AMBILIGHT_ADDR_HIGH = address >> 8;
		AMBILIGHT_ADDR_LOW  = address & 0xff;
		asm("nop");
		frames = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 1) & 0xff;
		asm("nop");
		ticks = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 2) & 0xff;
		asm("nop");
		ticks = (ticks << 8) | AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 3) & 0xff;
		asm("nop");
		ticks = (ticks << 8) | AMBILIGHT_DATA;
		ticks /= 16;
		AMBILIGHT_ADDR_LOW = (address + 4) & 0xff;
		asm("nop");
		temporalSmoothingRatio = AMBILIGHT_DATA;
		AMBILIGHT_ADDR_LOW = (address + 5) & 0xff;
		asm("nop");
		temporalSmoothingRatio = (temporalSmoothingRatio << 8) | AMBILIGHT_DATA;

		printf_P(PSTR("%d %ld %d\n"), frames, ticks, temporalSmoothingRatio);
	}
	//else
		//printf("err: GD\n");
}

void cmdSetDelay(uint8_t argc, char** argv)
{
	if(argc == 4)
	{
		uint8_t frames = getint(&argv[1]);
		uint32_t ticks = getint(&argv[2]);
		uint16_t temporalSmoothingRatio = getint(&argv[3]);
		uint16_t address = AMBILIGHT_BASE_ADDR_DELAY;
		AMBILIGHT_ADDR_HIGH = address >> 8;
		AMBILIGHT_ADDR_LOW  = address & 0xff;
		AMBILIGHT_DATA = frames;

		ticks *= 16;
		AMBILIGHT_ADDR_LOW = (address + 1) & 0xff;
		AMBILIGHT_DATA = 0;
		AMBILIGHT_ADDR_LOW = (address + 2) & 0xff;
		AMBILIGHT_DATA = ticks >> 8;
		AMBILIGHT_ADDR_LOW = (address + 3) & 0xff;
		AMBILIGHT_DATA = ticks & 0xff;

		AMBILIGHT_ADDR_LOW = (address + 4) & 0xff;
		AMBILIGHT_DATA = temporalSmoothingRatio >> 8;
		AMBILIGHT_ADDR_LOW = (address + 5) & 0xff;
		AMBILIGHT_DATA = temporalSmoothingRatio & 0xff;
	}
	//else
		//printf("err: SD num_frames num_ticks\n");
}

void cmdCfgDelay(uint8_t argc, char** argv)
{
	
}

