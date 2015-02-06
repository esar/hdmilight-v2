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
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include "ambilight.h"

#define RATIO_FUZZ    (0x100 / 10)
#define NOBAR_FUZZ    10
#define EVENBAR_FUZZ  20
#define X_RES_FUZZ    8
#define Y_RES_FUZZ    8
#define X_ACTIVE_FUZZ 4
#define Y_ACTIVE_FUZZ 4

#define IS_CENTERED(preActive, postActive)  ((preActive) >= (postActive) - EVENBAR_FUZZ && \
		                             (preActive) <= (postActive) + EVENBAR_FUZZ)
#define IS_FULLSCREEN(preActive, postActive) ((preActive) + (postActive) <= NOBAR_FUZZ)

typedef struct
{
	uint16_t width;
	uint16_t height;
	uint16_t ratio;
	uint16_t config;
	
} __attribute__((packed)) FormatConfig;

typedef struct
{
	uint16_t ratio;
	char* name;
	
} KnownRatio;

KnownRatio g_ratios[] = 
{
	{ (uint16_t)(1.78 * 256), "1.77" },
	{ (uint16_t)(2.40 * 256), "2.40" },
	{ (uint16_t)(1.33 * 256), "1.33" },
	{ 0, NULL }
};

static uint8_t g_formatChangeEnabled = 1;
static uint16_t g_currentRatio   = 0;
static uint16_t g_currentWidth  = 0xffff;
static uint16_t g_currentHeight = 0xffff;

uint16_t getConfig(uint16_t width, uint16_t height, uint16_t ratio)
{
	FormatConfig config;
	int offset = 0;
	
	do
	{
		dmaRead(0, offset, (uint16_t)&config, sizeof(config));
		if(config.width  >= width  - X_RES_FUZZ && config.width  <= width  + X_RES_FUZZ && 
		   config.height >= height - Y_RES_FUZZ && config.height <= height + Y_RES_FUZZ &&
		   config.ratio == ratio)
		{
			return config.config;
		}

		offset += sizeof(config);
		
	} while(config.width != 0);

	return 0xffff;
}

uint16_t getRatio(uint16_t width, uint16_t height)
{
	int i;
	uint32_t ratio = ((uint32_t)width * 0x100) / height;

	for(i = 0; i < sizeof(g_ratios) / sizeof(*g_ratios); ++i)
		if(ratio > g_ratios[i].ratio - RATIO_FUZZ && ratio < g_ratios[i].ratio + RATIO_FUZZ)
			break;

	if(i < sizeof(g_ratios) / sizeof(*g_ratios))
		return i;
	else
		return 0xffff;
}

void changeFormat()
{
	static uint16_t xMinPreActive  = 0;
	static uint16_t xMinPostActive = 0;
	static uint16_t yMinPreActive  = 0;
	static uint16_t yMinPostActive = 0;

	uint8_t hasChanged = 0;

	struct
	{
		uint16_t xSize;
		uint16_t xPreActive;
		uint16_t xPostActive;
		uint16_t ySize;
		uint16_t yPreActive;
		uint16_t yPostActive;

	} __attribute__((packed)) *format = AMBILIGHT_BASE_ADDR_FORMAT;


	if(format->xSize < g_currentWidth  - X_RES_FUZZ ||
	   format->xSize > g_currentWidth  + X_RES_FUZZ ||
	   format->ySize < g_currentHeight - Y_RES_FUZZ ||
	   format->ySize > g_currentHeight + Y_RES_FUZZ ||
	   format->xPreActive  < xMinPreActive  ||
	   format->xPostActive < xMinPostActive ||
	   format->yPreActive  < yMinPreActive  ||
	   format->yPostActive < yMinPostActive)
	{
		// Either the resolution has changed or the active
		// area has left the bounding box of the current ratio
		// so return to the native ratio for the current resolution

		//printf_P(PSTR("reset: %d %d %d   %d %d %d\n"), format->xSize, format->xPreActive, format->xPostActive, format->ySize, format->yPreActive, format->yPostActive);
		g_currentRatio   = 0;
		g_currentWidth   = format->xSize;
		g_currentHeight  = format->ySize;
		xMinPreActive  = 0;
		xMinPostActive = 0;
		yMinPreActive  = 0;
		yMinPostActive = 0;
		hasChanged     = 1;
	}

	if(IS_FULLSCREEN(format->xPreActive, format->xPostActive) &&
	   IS_CENTERED(format->yPreActive, format->yPostActive))
	{
		uint16_t ratio = getRatio(format->xSize - format->xPreActive - format->xPostActive,
		                          format->ySize - format->yPreActive - format->yPostActive);
		if(ratio != 0xffff && ratio != g_currentRatio)
		{
			g_currentRatio   = ratio;
			xMinPreActive  = 0;
			xMinPostActive = 0;
			yMinPreActive  = format->yPreActive - Y_ACTIVE_FUZZ;
			yMinPostActive = format->yPostActive - Y_ACTIVE_FUZZ;
			hasChanged     = 1;
		}
	}
	else if(IS_FULLSCREEN(format->yPreActive, format->yPostActive) &&
	        IS_CENTERED(format->xPreActive, format->xPostActive))
	{
		uint16_t ratio = getRatio(format->xSize - format->xPreActive - format->xPostActive,
		                          format->ySize - format->yPreActive - format->yPostActive);
		if(ratio != 0xffff && ratio != g_currentRatio)
		{
			g_currentRatio   = ratio;
			xMinPreActive  = format->xPreActive - X_ACTIVE_FUZZ;
			xMinPostActive = format->xPostActive - X_ACTIVE_FUZZ;
			yMinPreActive  = 0;
			yMinPostActive = 0;
			hasChanged     = 1;
		}
	}
	
	if(hasChanged)
	{
		uint32_t config;

		//printf_P(PSTR("format changed: %dx%d (%d): "), g_currentWidth, g_currentHeight, g_currentRatio);
		//printf_P(PSTR("%s\n"), g_ratios[g_currentRatio].name);
			
		config = getConfig(g_currentWidth, g_currentHeight, g_currentRatio);
		if(config != 0xffff && g_formatChangeEnabled)
		{
			config += 1;
			config *= 0x8000;
			dmaRead(config >> 16, config & 0xffff, 0x8000, 0x8000);
		}
	}
}	

void cmdDisFormat(uint8_t argc, char** argv)
{
	if(argc == 1)
		g_formatChangeEnabled = 0;
}

void cmdEnaFormat(uint8_t argc, char** argv)
{
	if(argc == 1)
		g_formatChangeEnabled = 1;
}

void cmdGetFormat(uint8_t argc, char** argv)
{
	if(argc == 1)
	{
		uint16_t* address = AMBILIGHT_BASE_ADDR_FORMAT;
		printf("raw: %d %d %d %d %d %d\n", address[0], address[1], address[2], address[3], address[4], address[5]);
		printf_P(PSTR("fmt: %dx%d %s (%d)\n"), g_currentWidth, g_currentHeight, 
		                                       g_ratios[g_currentRatio].name, g_currentRatio);
	}
}
