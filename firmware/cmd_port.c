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


void cmdSetPort(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		unsigned char addr = getint(&argv[1]);
		unsigned char val  = getint(&argv[2]);
		_SFR_IO8(addr) = val;
		printf_P(PSTR("OK\n"));
	}
	//else
		//printf("err: SP addr value\n");
}

void cmdGetPort(uint8_t argc, char** argv)
{
	if(argc == 2)
	{
		unsigned char addr = getint(&argv[1]);
		unsigned char val = _SFR_IO8(addr);
		printf_P(PSTR("%d=%d\n"), (int)addr, (int)val);
	}
	//else
		//printf("err: GP addr\n");
}

