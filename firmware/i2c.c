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

#include <avr/io.h>

#define I2C_PORT_DIR    _SFR_IO8(0x29)
#define I2C_PORT_IN     _SFR_IO8(0x2a)
#define I2C_PORT_OUT    _SFR_IO8(0x2b)

#define I2C_CLK_PIN     7
#define I2C_DAT_PIN     6

#define I2C_CLK_HIGH()    I2C_PORT_DIR &= ~(1 << I2C_CLK_PIN); I2C_PORT_OUT |= (1 << I2C_CLK_PIN)
#define I2C_CLK_LOW()     I2C_PORT_DIR |= (1 << I2C_CLK_PIN); I2C_PORT_OUT &= ~(1 << I2C_CLK_PIN)
#define I2C_CLK_READ()    ((I2C_PORT_IN & (1 << I2C_CLK_PIN)) ? 1 : 0)
#define I2C_DAT_HIGH()    I2C_PORT_DIR &= ~(1 << I2C_DAT_PIN); I2C_PORT_OUT |= (1 << I2C_DAT_PIN)
#define I2C_DAT_LOW()     I2C_PORT_DIR |= (1 << I2C_DAT_PIN); I2C_PORT_OUT &= ~(1 << I2C_DAT_PIN)
#define I2C_DAT_READ()    ((I2C_PORT_IN & (1 << I2C_DAT_PIN)) ? 1 : 0)

static __attribute__((noinline)) void delay_quarter_bit()
{
	// call 3-clocks, lpm 3-clocks, ret 4-clocks = 10 clocks = 0.625uS @ 16MHz
	asm volatile ("nop");
}

static __attribute__((noinline)) void delay_half_bit()
{
	// call 3-clocks, 4xlpm 12-clocks, nop 1-clock, ret 4-clocks = 20 clocks = 1.25uS @ 16MHz
	asm volatile ("lpm");
	asm volatile ("nop");
}

void i2c_init()
{
	I2C_PORT_DIR &= (1 << I2C_CLK_PIN) | (1 << I2C_DAT_PIN);
	I2C_PORT_OUT &= (1 << I2C_CLK_PIN) | (1 << I2C_DAT_PIN);
}

unsigned char i2c_start()
{
	// start with clock and data high
	I2C_CLK_HIGH();
	I2C_DAT_HIGH();
	delay_half_bit();

	// ensure no one else has the bus (clock should remain high)
	if(!I2C_CLK_READ())
		return 1;

	// pull data low
	I2C_DAT_LOW();
	delay_half_bit();

	// pull clock low
	I2C_CLK_LOW();
	delay_half_bit();

	//return data to high
	I2C_DAT_HIGH();
	delay_half_bit();

	delay_half_bit();
	delay_half_bit();

	return 0;
}

void i2c_stop()
{
	//start with clock and data low
	I2C_CLK_LOW();
	I2C_DAT_LOW();
	delay_half_bit();
	delay_half_bit();

	// return clock to high
	I2C_CLK_HIGH();
	while(!I2C_CLK_READ())
		;
	delay_half_bit();

	// return data to high
	I2C_DAT_HIGH();
	delay_half_bit();
	delay_half_bit();
}

unsigned char i2c_write(unsigned char x)
{
	unsigned char i;

	for(i = 0; i < 8; ++i)
	{
		if(x & 0x80)
		{
			I2C_DAT_HIGH();
		}
		else
		{
			I2C_DAT_LOW();
		}
		x <<= 1;

		delay_quarter_bit();
		I2C_CLK_HIGH();
		while(!I2C_CLK_READ())
			;
		delay_half_bit();
		I2C_CLK_LOW();
		delay_quarter_bit();
	}

	// now read ack bit
	I2C_DAT_HIGH();
	delay_quarter_bit();
	I2C_CLK_HIGH();
	while(!I2C_CLK_READ())
		;
	delay_half_bit();
	x = I2C_DAT_READ();
	I2C_CLK_LOW();
	delay_quarter_bit();

	return x;
}

unsigned char i2c_read()
{
	unsigned char i;
	unsigned char x = 0;

for(i = 0; i < 16; ++i)
	delay_half_bit();

	for(i = 0; i < 8; ++i)
	{
		delay_quarter_bit();
		I2C_CLK_HIGH();
		while(!I2C_CLK_READ())
			;
		delay_half_bit();
		x = (x << 1) | I2C_DAT_READ();
		I2C_CLK_LOW();
		delay_quarter_bit();
	}

for(i = 0; i < 16; ++i)
	delay_half_bit();

	// Send NACK
	I2C_DAT_LOW();
	delay_quarter_bit();
	I2C_CLK_HIGH();
	while(!I2C_CLK_READ())
		;
	delay_half_bit();
	I2C_CLK_LOW();
	delay_quarter_bit();

	return x;
}

void i2c_ack(unsigned char x)
{
	if(x)
	{
		I2C_DAT_HIGH();
	}
	else
	{
		I2C_DAT_LOW();
	}
	delay_quarter_bit();
	I2C_CLK_HIGH();
	delay_half_bit();
	I2C_CLK_LOW();
	delay_quarter_bit();
}

