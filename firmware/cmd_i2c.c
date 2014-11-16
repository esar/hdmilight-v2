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
#include "i2c.h"

#include "config_hdmi.h"
#include "config_edid.h"


uint8_t i2cRead(uint8_t addr, uint8_t subaddr)
{
	int val;
	i2c_start();
	i2c_write(addr);
	i2c_write(subaddr);
	i2c_start();
	i2c_write(0x99);
	val = i2c_read(0);
	i2c_stop();

	return val;
}

void cmdSetI2C(uint8_t argc, char** argv)
{
	if(argc == 4)
	{
		i2c_start();
		i2c_write(getint(&argv[1]));
		i2c_write(getint(&argv[2]));
		i2c_write(getint(&argv[3]));
		i2c_stop();
		printf_P(PSTR("OK\n"));
	}
	//else
		//printf("err: SI addr subaddr value\n");
}

void cmdGetI2C(uint8_t argc, char** argv)
{
	if(argc == 3)
	{
		int val = i2cRead(getint(&argv[1]), getint(&argv[2]));
		printf_P(PSTR("Read: %d\n"), val);
	}
	//else
		//printf("err: GI addr\n");
}

void writeEdid(const char* edid, int length)
{
	int i;

	for(i = 0; i < length; ++i)
	{
		uint8_t data = pgm_read_byte(&edid[i]);

		i2c_start();
		i2c_write(0x6c);
		i2c_write(i);
		i2c_write(data);
		i2c_stop();
	}
}

void writeConfig(const struct ConfigTable* table)
{
	const struct ConfigTable* p;

	for(p = table; pgm_read_byte(&p->address) != 0; ++p)
	{
		unsigned char address = pgm_read_byte(&p->address);
		unsigned char subaddress = pgm_read_byte(&p->subaddress);
		unsigned char data = pgm_read_byte(&p->data);

		if(!silent)
		{
			printf_P(PSTR("%d %d %d : "), address, subaddress, data);
			i2c_start();
			printf_P(PSTR("%s "), i2c_write(address)    ? "ACK" : "NACK");
			printf_P(PSTR("%s "), i2c_write(subaddress) ? "ACK" : "NACK");
			printf_P(PSTR("%s\n"), i2c_write(data)      ? "ACK" : "NACK");
			i2c_stop();
		}
		else
		{
			i2c_start();
			i2c_write(address);
			i2c_write(subaddress);
			i2c_write(data);
			i2c_stop();
		}
	}
}

void cmdCfgI2C(uint8_t argc, char** argv)
{
	writeConfig(g_configTablePreEdid);
	writeEdid(g_edid, sizeof(g_edid));
	writeConfig(g_configTablePostEdid);
}
