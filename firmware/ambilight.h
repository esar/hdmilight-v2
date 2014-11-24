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

#define AMBILIGHT_BASE_ADDR_OUTPUT ((void*)0x8000)
#define AMBILIGHT_BASE_ADDR_COLOUR ((void*)0xa000)
#define AMBILIGHT_BASE_ADDR_AREA   ((void*)0xb000)
#define AMBILIGHT_BASE_ADDR_GAMMAR ((void*)0xc000)
#define AMBILIGHT_BASE_ADDR_GAMMAG ((void*)0xc800)
#define AMBILIGHT_BASE_ADDR_GAMMAB ((void*)0xd000)
#define AMBILIGHT_BASE_ADDR_RESULT ((void*)0xd800)
#define AMBILIGHT_BASE_ADDR_STATUS ((void*)0xe000)
#define AMBILIGHT_BASE_ADDR_DELAY  ((void*)0xe800)
#define AMBILIGHT_BASE_ADDR_FORMAT ((void*)0xf000)


extern uint8_t silent;
extern volatile uint8_t g_formatChanged;


char readcmd(char** argv, char maxargs);
int getint(char** str);
void getrange(char* str, uint8_t* min, uint8_t* max);

uint32_t getfixed_9_9(const char* p);
int fixed_9_9_fract(int32_t x, int numDigits);
int fixed_9_9_int(int32_t x);

void dmaRead(uint8_t section, uint16_t src, uint16_t dst, uint16_t len);

uint8_t i2cRead(uint8_t addr, uint8_t subaddr);

void changeFormat();

void cmdGetArea(uint8_t argc, char** argv);
void cmdSetArea(uint8_t argc, char** argv);
void cmdRstArea(uint8_t argc, char** argv);

void cmdGetAddr(uint8_t argc, char** argv);
void cmdSetAddr(uint8_t argc, char** argv);

void cmdGetColour(uint8_t argc, char** argv);
void cmdSetColour(uint8_t argc, char** argv);
void cmdRstColour(uint8_t argc, char** argv);

void cmdGetDelay(uint8_t argc, char** argv);
void cmdSetDelay(uint8_t argc, char** argv);
void cmdRstDelay(uint8_t argc, char** argv);

void cmdGetGamma(uint8_t argc, char** argv);
void cmdSetGamma(uint8_t argc, char** argv);
void cmdRstGamma(uint8_t argc, char** argv);

void cmdGetI2C(uint8_t argc, char** argv);
void cmdSetI2C(uint8_t argc, char** argv);
void cmdRstI2C(uint8_t argc, char** argv);

void cmdGetMem(uint8_t argc, char** argv);

void cmdGetOutput(uint8_t argc, char** argv);
void cmdSetOutput(uint8_t argc, char** argv);
void cmdRstOutput(uint8_t argc, char** argv);

void cmdGetPort(uint8_t argc, char** argv);
void cmdSetPort(uint8_t argc, char** argv);

void cmdGetResult(uint8_t argc, char** argv);

void cmdGetStatus(uint8_t argc, char** argv);

void cmdGetFormat(uint8_t argc, char** argv);
