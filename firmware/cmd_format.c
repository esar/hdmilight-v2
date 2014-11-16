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

struct
{
	uint16_t width;
	uint16_t height;

} g_hdmiVIC[] PROGMEM = 
{
	{ 0,    0    },  // 0
	{ 640,  480  },  // 1
	{ 720,  480  },  // 2
	{ 720,  480  },  // 3
	{ 1280, 720  },  // 4
	{ 1920, 1080 },  // 5
	{ 720,  480  },  // 6
	{ 720,  480  },  // 7
        { 720,  240  },  // 8
	{ 720,  240  },  // 9
	{ 2880, 480  },  // 10
	{ 2880, 480  },  // 11
	{ 2880, 240  },  // 12
	{ 2880, 240  },  // 13
	{ 1440, 480  },  // 14
	{ 1440, 480  },  // 15
	{ 1920, 1080 },  // 16
	{ 720,  576  },  // 17
	{ 720,  576  },  // 18
	{ 1280, 720  },  // 19
	{ 1920, 1080 },  // 20
	{ 720,  576  },  // 21
	{ 720,  576  },  // 22
	{ 720,  288  },  // 23
	{ 720,  288  },  // 24
	{ 2880, 576  },  // 25
	{ 2880, 576  },  // 26
	{ 2880, 288  },  // 27
	{ 2880, 288  },  // 28
	{ 1440, 576  },  // 29
	{ 1440, 576  },  // 30
	{ 1920, 1080 },  // 31
	{ 1920, 1080 },  // 32
	{ 1920, 1080 },  // 33
	{ 1920, 1080 },  // 34
	{ 2880, 480  },  // 35
	{ 2880, 480  },  // 36
	{ 2880, 576  },  // 37
	{ 2880, 576  },  // 38
	{ 1920, 1080 },  // 39
	{ 1920, 1080 },  // 40
	{ 1280, 720  },  // 41
	{ 720,  576  },  // 42
	{ 720,  576  },  // 43
	{ 720,  576  },  // 44
	{ 720,  576  },  // 45
	{ 1920, 1080 },  // 46
	{ 1280, 720  },  // 47
	{ 720,  480  },  // 48
	{ 720,  480  },  // 49
	{ 720,  480  },  // 50
	{ 720,  480  },  // 51
	{ 720,  576  },  // 52
	{ 720,  576  },  // 53
	{ 720,  576  },  // 54
	{ 720,  576  },  // 55
	{ 720,  480  },  // 56
	{ 720,  480  },  // 57
	{ 720,  480  },  // 58
	{ 720,  480  },  // 59
	{ 1280, 720  },  // 60
	{ 1280, 720  },  // 61
	{ 1280, 720  },  // 62
	{ 1920, 1080 },  // 63
	{ 1920, 1080 },  // 64
};

struct
{
	uint16_t ratio;
	char* name;

} g_ratios[] = 
{
	{ (uint16_t)(1.33 * 256), "1.33" },
	{ (uint16_t)(1.78 * 256), "1.77" },
	{ (uint16_t)(1.85 * 256), "1.85" },
	{ (uint16_t)(2.40 * 256), "2.40" },
	{ 0, NULL }
};


void cmdGetFormat(uint8_t argc, char** argv)
{
	static int last = 0xff;

	uint16_t* address = AMBILIGHT_BASE_ADDR_FORMAT;
	uint16_t width  = (address[0] - address[1] - address[2]) * 2;
	uint16_t height = address[3] - address[4] - address[5];
	uint32_t ratio  = ((uint32_t)width * 256) / height;
	int i;

	for(i = 0; i < sizeof(g_ratios) / sizeof(*g_ratios); ++i)
		if(ratio > g_ratios[i].ratio - 25 && ratio < g_ratios[i].ratio + 25)
			break;

	if(i < sizeof(g_ratios) / sizeof(*g_ratios))
	{
		if(i != last)
		{
			uint8_t vic = i2cRead(0x7c, 0x04);	
			if(vic < sizeof(g_hdmiVIC) / sizeof(*g_hdmiVIC))
			{
				uint16_t vicWidth = pgm_read_word(&g_hdmiVIC[vic].width);
				uint16_t vicHeight = pgm_read_word(&g_hdmiVIC[vic].height);
				int pass = (width > vicWidth - (vicWidth >> 7) && width < vicWidth + (vicWidth >> 7));
				pass |= (height > vicHeight - (vicHeight >> 7) && height < vicHeight + (vicHeight >> 7));
				if(!pass)
					return;
			}
			
			printf_P(PSTR("format changed: %dx%d (%d): "), width, height, (uint16_t)ratio);
			printf_P(PSTR("%s\n"), g_ratios[i].name);
			last = i;
		}
	}

/*
	if(argc == 1)
	{
		uint16_t* address = AMBILIGHT_BASE_ADDR_FORMAT;
		printf("%d %d %d %d %d %d\n", address[0], address[1], address[2], address[3], address[4], address[5]);
	}
*/
}
