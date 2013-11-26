#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <avr/pgmspace.h>

//-----------------------------------------------------------------
// Prototypes:
//-----------------------------------------------------------------
void serial_init (void);
int serial_putchar(char ch);
int serial_getchar(void);
int serial_haschar();
void serial_putstr(char *str);
void serial_putstr_PSTR(const char *str);
void serial_putnum( int n );

#endif // __SERIAL_H__
