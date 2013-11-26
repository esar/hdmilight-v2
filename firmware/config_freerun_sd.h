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

struct _ConfigData
{
	unsigned char address;
	unsigned char subaddress;
	unsigned char data;

} g_configData[] = 
{
	//{ 0x98, 0xFF, 0x80 },
	{ 0x98, 0xFD, 0x44 },
	{ 0x98, 0x00, 0x0B }, //0x1E },
	{ 0x98, 0x01, 0x05 },
	{ 0x98, 0x03, 0x42 },
	{ 0x98, 0x05, 0x28 },
	{ 0x98, 0x0B, 0x44 },
	{ 0x98, 0x0C, 0x42 },
	{ 0x98, 0x14, 0x7F },
	{ 0x98, 0x15, 0x80 },
	{ 0x98, 0x19, 0x83 },
	{ 0x98, 0x33, 0x40 },
	{ 0x44, 0xBA, 0x01 },
	{ 0x44, 0xBF, 0x17 },
	{ 0x44, 0xC0, 0x00 },
	{ 0x44, 0xC1, 0x00 },
	{ 0x44, 0xC2, 0xA0 },
	{ 0x44, 0xC9, 0x05 },
	{ 0x00 }
};

