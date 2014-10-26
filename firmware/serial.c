#include <avr/io.h>
#include <stdio.h>
#include "serial.h"

//-----------------------------------------------------------------
// Registers
//-----------------------------------------------------------------
#define UART_USR	_SFR_IO8(0x20)
	#define UART_RX_AVAIL	(1<<0)
	#define UART_TX_AVAIL	(1<<1)
	#define UART_RX_FULL	(1<<2)
	#define UART_TX_BUSY	(1<<3)
	#define UART_RX_ERROR	(1<<4)
#define UART_UDR	_SFR_IO8(0x21)


int serial_stdio_rx(FILE* stream)
{
	// Read character in from UART0 Recieve Buffer and return
	if (serial_haschar())
		return UART_UDR;
	else
		return -1;
}

int serial_stdio_tx(char c, FILE* stream)
{
	if (c == '\n')
		serial_stdio_tx('\r', stream);

	UART_UDR = c;
	while (UART_USR & UART_TX_BUSY);

	return 0;
}

//-------------------------------------------------------------
// serial_init: 
//-------------------------------------------------------------
void serial_init (void)
{
	static FILE serial_stream = FDEV_SETUP_STREAM(serial_stdio_tx, serial_stdio_rx, _FDEV_SETUP_RW);
	stdout = stdin = &serial_stream;
}

//-------------------------------------------------------------
// serial_putchar: Write character to Serial Port (used by printf)
//-------------------------------------------------------------
int serial_putchar(char ch)   
{   
	if (ch == '\n')
		serial_putchar('\r');
	
	UART_UDR = ch;
	while (UART_USR & UART_TX_BUSY);

	return 0;
}
//-------------------------------------------------------------
// serial_getchar: Read character from Serial Port  
//-------------------------------------------------------------
int serial_getchar (void)           
{     
	// Read character in from UART0 Recieve Buffer and return
	if (serial_haschar())
		return UART_UDR;
	else
		return -1;
}
//-------------------------------------------------------------
// serial_haschar:
//-------------------------------------------------------------
int serial_haschar()
{
	return (UART_USR & UART_RX_AVAIL);
}
//-------------------------------------------------------------
// serial_putstr:
//-------------------------------------------------------------
void serial_putstr(char *str)
{
	while (*str)
		serial_putchar(*str++);
}
//-------------------------------------------------------------
// serial_putstr_PSTR:
//-------------------------------------------------------------
void serial_putstr_PSTR(const char *str)
{
	while (pgm_read_byte(str))
		serial_putchar(pgm_read_byte(str++));
}
//-------------------------------------------------------------
// serial_putnum:
//-------------------------------------------------------------
void serial_putnum( int n )
{
    char* cp;
    int negative;
    char outbuf[32];
    const char digits[] = "0123456789ABCDEF";
    unsigned long num;

   /* Check if number is negative                   */
    if (n < 0L) {
        negative = 1;
        num = -(n);
    }
    else{
        num = (n);
        negative = 0;
    }
   
    /* Build number (backwards) in outbuf            */
    cp = outbuf;
    do {
        *cp++ = digits[(int)(num % 10)];
    } while ((num /= 10) > 0);
    if (negative)
        *cp++ = '-';
    *cp-- = 0;
    
    while (cp >= outbuf)
        serial_putchar(*cp--);
}
