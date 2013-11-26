#ifndef __TIMER_H__
#define __TIMER_H__

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
typedef unsigned long t_time;

//-----------------------------------------------------------------
// Prototypes:
//-----------------------------------------------------------------

// General timer
void		timer_init(void);
t_time		timer_now(void);
static long	timer_diff(t_time a, t_time b) { return (long)(a - b); } 

void		timer_sleep(int timeMs);

#endif
