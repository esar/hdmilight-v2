/*

   Copyright (C) 2015 Stephen Robinson
  
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
#include "cec.h"

#define ADJUST_FIRST         0x3A
#define ADJUST_CONTRAST      0x3A
#define ADJUST_SATURATION    0x3B
#define ADJUST_BRIGHTNESS    0x3C
#define ADJUST_HUE           0x3D
#define ADJUST_LAST          0x3D


static uint8_t on = 1;
static uint8_t currentAdjust = ADJUST_FIRST;

void adjustUp()
{
	uint8_t x = i2cRead(0x44, currentAdjust);
	x = (int16_t)x + 8 > 255 ? 255 : x + 8;
	i2cWrite(0x44, currentAdjust, x);

	printf_P(PSTR("adjust up: %u\n"), x);
}

void adjustDown()
{
	uint8_t x = i2cRead(0x44, currentAdjust);
	x = (int16_t)x - 8 < 0 ? 0 : x - 8;
	i2cWrite(0x44, currentAdjust, x);

	printf_P(PSTR("adjust down: %u\n"), x);
}

void adjustCycle()
{
	currentAdjust += 1;
	if(currentAdjust > ADJUST_LAST)
		currentAdjust = ADJUST_FIRST;

	printf_P(PSTR("adjust cycle: %02x\n"), currentAdjust);
}

void powerOn()
{
	uint8_t x;

	// turn off forced free run mode and manual colour selection
	x = i2cRead(0x44, 0xBF);
	x &= ~5;
	i2cWrite(0x44, 0xBF, x);
	
	printf_P(PSTR("power on\n"));
}

void powerOff()
{
	uint8_t x;

	// set free run colour to black
	i2cWrite(0x44, 0xC0, 0);
	i2cWrite(0x44, 0xC1, 0);
	i2cWrite(0x44, 0xC2, 0);

	// force free run mode with manual colour selection
	x = i2cRead(0x44, 0xBF);
	x |= 5;
	i2cWrite(0x44, 0xBF, x);

	printf_P(PSTR("power off\n"));
}

void togglePower()
{
	if(on)
		powerOff();
	else
		powerOn();

	on ^= 1;
}

void processCecMessage()
{
	uint8_t i;

	printf_P(PSTR("CEC %u: "), g_cecMessageLength);
	for(i = 0; i < g_cecMessageLength; ++i)
		printf("%02x ", g_cecMessage[i]);
	printf("\n");

	if(g_cecMessage[1] == CEC_OP_USER_CONTROL_PRESSED)
	{
		uint8_t action;

		dmaRead(0, 0x7f00 + g_cecMessage[2], (uint16_t)&action, sizeof(action));
		
		if(action == ACTION_POWER_TOGGLE)
			togglePower();
		else if(action == ACTION_CONFIG_CYCLE)
			printf_P(PSTR("cycle preset\n"));
		else if(action == ACTION_ADJUST_UP)
			adjustUp();
		else if(action == ACTION_ADJUST_DOWN)
			adjustDown();
		else if(action == ACTION_ADJUST_CYCLE)
			adjustCycle();
		else if(action == ACTION_CONFIG_AUTO)
			cmdEnaFormat(0, NULL);
		else if(action >= ACTION_CONFIG_0 && action <= ACTION_CONFIG_8)
		{
			uint32_t config = action - ACTION_CONFIG_0;

			printf_P(PSTR("config %lu\n"), config);

			config += 1;
			config *= 0x8000;
			cmdDisFormat(0, NULL);
			dmaRead(config >> 16, config & 0xffff, 0x8000, 0x8000);
		}
	}
}


void cmdSetKeys(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		g_cecMessage[0] = 0xFF;
		g_cecMessage[1] = 0x44;
		g_cecMessage[2] = getint(&argv[1]);
		g_cecMessageLength = 3;
	}
}

void cmdRstKeys(uint8_t argc, char** argv)
{
	uint8_t x;

	i2cWrite(0x80, 0x2A, 0x01);	// power up CEC
	i2cWrite(0x98, 0x96, 63);	// enable all CEC interrupts on INT1
	i2cWrite(0x98, 0x94, 63);	// clear all CEC interrupts
	i2cWrite(0x80, 0x28, 0);        // logical address 0 = 0 (TV)
	//i2cWrite(0x80, 0x77, 1);	// use all CEC RX buffers

	x = i2cRead(0x44, 0x3E);        // enable colour adjustment controls
	x |= 0x80;
	i2cWrite(0x44, 0x3E, x);
}

