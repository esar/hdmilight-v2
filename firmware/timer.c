#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

//-----------------------------------------------------------------
// Registers
//-----------------------------------------------------------------
#define TMR_T0_A_LATCH		_SFR_IO8(0x22)
#define TMR_T0_B			_SFR_IO8(0x23)
#define TMR_T0_C			_SFR_IO8(0x24)
#define TMR_T0_D			_SFR_IO8(0x25)

//--------------------------------------------------------------------------
// Locals
//--------------------------------------------------------------------------
static volatile unsigned long timer_ticks;
static volatile unsigned long timer_sec;
static volatile unsigned long timer_ms;

//--------------------------------------------------------------------------
// timer_init: [Timer 1]
//--------------------------------------------------------------------------
void timer_init(void)
{
	timer_ticks = 0;
	timer_ms = 0;
	timer_sec = 0;
}
//--------------------------------------------------------------------------
// timer_now: [Timer 1]
//--------------------------------------------------------------------------
t_time timer_now(void)
{
	t_time retval;

	// Write to timer register 0x22 latches timer
	TMR_T0_A_LATCH = 0;

	retval = TMR_T0_D;
	retval <<= 8;
	retval|= TMR_T0_C;
	retval <<= 8;
	retval|= TMR_T0_B;
	retval <<= 8;
	retval|= TMR_T0_A_LATCH;

	return retval;
}
//--------------------------------------------------------------------------
// timer_sleep:
//--------------------------------------------------------------------------
void timer_sleep(int timeMs)
{
	t_time t = timer_now();

	while (timer_diff(timer_now(), t) < timeMs)
		;
}
