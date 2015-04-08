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

#ifdef PORTD
#undef PORTD
#endif

#ifdef PIND
#undef PIND
#endif

#ifdef DDRD
#undef DDRD
#endif

#define DDRD     _SFR_IO8(0x29)
#define PIND     _SFR_IO8(0x2a)
#define PORTD    _SFR_IO8(0x2b)

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

#define DMA_FLASH_ADDR_H    _SFR_IO8(0x2c)
#define DMA_FLASH_ADDR_M    _SFR_IO8(0x2d)
#define DMA_FLASH_ADDR_L    _SFR_IO8(0x2e)
#define DMA_SRAM_ADDR_H     _SFR_IO8(0x2f)
#define DMA_SRAM_ADDR_L     _SFR_IO8(0x30)
#define DMA_LEN_H           _SFR_IO8(0x31)
#define DMA_LEN_L           _SFR_IO8(0x32)
#define DMA_START           _SFR_IO8(0x33)
#define INT_CLEAR           _SFR_IO8(0x34)

#define ACTION_NONE           0
#define ACTION_POWER_TOGGLE    1
#define ACTION_CONFIG_AUTO     2
#define ACTION_CONFIG_0        3
#define ACTION_CONFIG_1        4
#define ACTION_CONFIG_2        5
#define ACTION_CONFIG_3        6
#define ACTION_CONFIG_4        7
#define ACTION_CONFIG_5        8
#define ACTION_CONFIG_6        9
#define ACTION_CONFIG_7        10
#define ACTION_CONFIG_8        11
#define ACTION_CONFIG_CYCLE    12
#define ACTION_ADJUST_CYCLE    13
#define ACTION_ADJUST_UP       14
#define ACTION_ADJUST_DOWN     15
#define ACTION_ADJUST_BRIGHTNESS 16
#define ACTION_ADJUST_CONTRAST   17
#define ACTION_ADJUST_SATURATION 18
#define ACTION_ADJUST_HUE        19

#define GPIO_POWER_PIN         0
#define DEBOUNCE_TICK_COUNT    16


extern uint8_t silent;
extern volatile uint8_t g_formatChanged;
extern volatile uint8_t g_cecMessage[16];
extern volatile uint8_t g_cecMessageLength;


char readcmd(char** argv, char maxargs);
int getint(char** str);
void getrange(char* str, uint8_t* min, uint8_t* max);

uint32_t getfixed_9_9(const char* p);
int fixed_9_9_fract(int32_t x, int numDigits);
int fixed_9_9_int(int32_t x);

void dmaRead(uint8_t section, uint16_t src, uint16_t dst, uint16_t len);

uint8_t i2cRead(uint8_t addr, uint8_t subaddr);
void i2cWrite(uint8_t addr, uint8_t subaddr, uint8_t value);

void changeFormat();

void processCecMessage();
void powerOn();
void powerOff();

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

void cmdSetKeys(uint8_t argc, char** argv);
void cmdRstKeys(uint8_t argc, char** argv);

void cmdGetMem(uint8_t argc, char** argv);

void cmdGetOutput(uint8_t argc, char** argv);
void cmdSetOutput(uint8_t argc, char** argv);
void cmdRstOutput(uint8_t argc, char** argv);

void cmdGetPort(uint8_t argc, char** argv);
void cmdSetPort(uint8_t argc, char** argv);

void cmdGetResult(uint8_t argc, char** argv);

void cmdGetStatus(uint8_t argc, char** argv);

void cmdDisFormat(uint8_t argc, char** argv);
void cmdEnaFormat(uint8_t argc, char** argv);
void cmdGetFormat(uint8_t argc, char** argv);

void cmdGetStack(uint8_t argc, char** argv);

