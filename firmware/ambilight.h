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

#define BELL    '\a'

#define AMBILIGHT_ADDR_LOW  _SFR_IO8(0x26)
#define AMBILIGHT_ADDR_HIGH _SFR_IO8(0x27)
#define AMBILIGHT_DATA      _SFR_IO8(0x28)

#define AMBILIGHT_BASE_ADDR_OUTPUT 0x0000
#define AMBILIGHT_BASE_ADDR_COLOUR 0x2000
#define AMBILIGHT_BASE_ADDR_AREA   0x3000
#define AMBILIGHT_BASE_ADDR_GAMMAR 0x4000
#define AMBILIGHT_BASE_ADDR_GAMMAG 0x4800
#define AMBILIGHT_BASE_ADDR_GAMMAB 0x5000
#define AMBILIGHT_BASE_ADDR_RESULT 0x5800
#define AMBILIGHT_BASE_ADDR_STATUS 0x6000
#define AMBILIGHT_BASE_ADDR_DELAY  0x6800


extern uint8_t silent;


char readcmd(char** argv, char maxargs);
int getint(char** str);
void getrange(char* str, uint8_t* min, uint8_t* max);

void cmdGetAddr(uint8_t argc, char** argv);
void cmdSetAddr(uint8_t argc, char** argv);

void cmdGetColour(uint8_t argc, char** argv);
void cmdSetColour(uint8_t argc, char** argv);
void cmdCfgColour(uint8_t argc, char** argv);

void cmdGetDelay(uint8_t argc, char** argv);
void cmdSetDelay(uint8_t argc, char** argv);
void cmdCfgDelay(uint8_t argc, char** argv);

void cmdGetGamma(uint8_t argc, char** argv);
void cmdSetGamma(uint8_t argc, char** argv);
void cmdCfgGamma(uint8_t argc, char** argv);

void cmdGetI2C(uint8_t argc, char** argv);
void cmdSetI2C(uint8_t argc, char** argv);
void cmdCfgI2C(uint8_t argc, char** argv);

void cmdGetLight(uint8_t argc, char** argv);
void cmdSetLight(uint8_t argc, char** argv);
void cmdCfgLight(uint8_t argc, char** argv);

void cmdGetOutput(uint8_t argc, char** argv);
void cmdSetOutput(uint8_t argc, char** argv);
void cmdCfgOutput(uint8_t argc, char** argv);

void cmdGetPort(uint8_t argc, char** argv);
void cmdSetPort(uint8_t argc, char** argv);

void cmdGetResult(uint8_t argc, char** argv);

void cmdGetStatus(uint8_t argc, char** argv);
