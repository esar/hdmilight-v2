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

struct ConfigTable
{
	unsigned char address;
	unsigned char subaddress;
	unsigned char data;

};

static const struct ConfigTable g_configTablePreEdid[] PROGMEM = 
{

	{ 0x98, 0xF4, 0x80 },  // CEC
	{ 0x98, 0xF5, 0x7C },  // INFOFRAME
	{ 0x98, 0xF8, 0x4C },  // DPLL
	{ 0x98, 0xF9, 0x64 },  // KSV
	{ 0x98, 0xFA, 0x6C },  // EDID
	{ 0x98, 0xFB, 0x68 },  // HDMI
	{ 0x98, 0xFD, 0x44 },  // CP

	{ 0x64, 0x77, 0x00 },  // Disable the Internal EDID

	{ 0x00 }
};

static const struct ConfigTable g_configTablePostEdid[] PROGMEM = 
{
	{ 0x64, 0x77, 0x00 },  // Set the Most Significant Bit of the SPA location to 0
	{ 0x64, 0x52, 0x20 },  // Set the SPA for port B.
	{ 0x64, 0x53, 0x00 },  // Set the SPA for port B.
	{ 0x64, 0x70, 0x9E },  // Set the Least Significant Byte of the SPA location
	{ 0x64, 0x74, 0x03 },  // Enable the Internal EDID for Ports

	{ 0x98, 0x01, 0x06 },  // Prim_Mode =110b HDMI-GR
	{ 0x98, 0x02, 0xF2 },  // Auto CSC, YCrCb out, Set op_656 bit
	{ 0x98, 0x03, 0x40 },  // 24 bit SDR 444 Mode 0 
	{ 0x98, 0x05, 0x28 },  // AV Codes Off
	{ 0x98, 0x0B, 0x44 },  // Power up part
	{ 0x98, 0x0C, 0x42 },  // Power up part
	{ 0x98, 0x14, 0x55 },  // Min Drive Strength
	{ 0x98, 0x15, 0x80 },  // Disable Tristate of Pins
	{ 0x98, 0x19, 0x85 },  // LLC DLL phase
	{ 0x98, 0x33, 0x40 },  // LLC DLL enable
	{ 0x44, 0xBA, 0x01 },  // Set HDMI FreeRun
	{ 0x64, 0x40, 0x81 },  // Disable HDCP 1.1 features
	{ 0x68, 0x9B, 0x03 },  // ADI recommended setting
	{ 0x68, 0xC1, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC2, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC3, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC4, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC5, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC6, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC7, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC8, 0x01 },  // ADI recommended setting
	{ 0x68, 0xC9, 0x01 },  // ADI recommended setting
	{ 0x68, 0xCA, 0x01 },  // ADI recommended setting
	{ 0x68, 0xCB, 0x01 },  // ADI recommended setting
	{ 0x68, 0xCC, 0x01 },  // ADI recommended setting
	{ 0x68, 0x00, 0x00 },  // Set HDMI Input Port A
	{ 0x68, 0x83, 0xFE },  // Enable clock terminator for port A
	{ 0x68, 0x6F, 0x0C },  // ADI recommended setting
	{ 0x68, 0x85, 0x1F },  // ADI recommended setting
	{ 0x68, 0x87, 0x70 },  // ADI recommended setting
	{ 0x68, 0x8D, 0x04 },  // LFG
	{ 0x68, 0x8E, 0x1E },  // HFG
	{ 0x68, 0x1A, 0x8A },  // unmute audio
	{ 0x68, 0x57, 0xDA },  // ADI recommended setting
	{ 0x68, 0x58, 0x01 },  // ADI recommended setting
	{ 0x68, 0x75, 0x10 },  // DDC drive strength
	{ 0x98, 0x40, 0xE2 },  // INT1 active high, active until cleared

	{ 0x00 }
};

