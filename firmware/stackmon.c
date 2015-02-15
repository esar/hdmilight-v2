/***************************************************************************
 * stackmon.c: Stack usage monitoring for ATmega88/168.
 * Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is PUBLIC DOMAIN.
 * This means that there is no copyright and anyone is able to take a copy
 * for free and use it as they wish, with or without modifications, and in
 * any context, commercially or otherwise. The only limitation is that I
 * don't guarantee that the software is fit for any purpose or accept any
 * liability for it's use or misuse - this software is without warranty.
 ***************************************************************************/

/**************************************************************************
 * Include Files
 **************************************************************************/

#include <stdint.h>
#include "stackmon.h"

/**************************************************************************
 * Manifest Constants
 **************************************************************************/

/** Magic value for testing if the stack has overran the program variables.
 */
#define STACK_CANARY 0xc5

/**************************************************************************
 * Type Definitions
 **************************************************************************/

/**************************************************************************
 * Variables
 **************************************************************************/

/** Gain access to linker symbol for end of variable section.
 * This symbol is defined to be the address of the first unused byte of SRAM.
 * The stack is by convention at the top of the SRAM, growing down towards
 * the address at which this variable resides.
 */
extern uint8_t _end;

/** Gain access to linker symbol for base of the stack.
 * This symbol is defined to be the address at the bootom of the stack.
 */
extern uint8_t __stack;

/**************************************************************************
 * Macros
 **************************************************************************/

/**************************************************************************
 * Local Functions
 **************************************************************************/

void StackPaint(void) __attribute__ ((naked)) __attribute__ ((section (".init1")));

/** Fill the stack space with a known pattern.
 * This fills all stack bytes with the 'canary' pattern to allow stack usage
 * to be later estimated.  This runs in the .init1 section, before normal
 * stack initialisation and unfortunately before __zero_reg__ has been
 * setup.  The C code is therefore replaced with inline assembly to ensure
 * the zero reg is not used by compiled code.
 *
 * \note This relies on the linker defining \a _end and \a __stack to define
 *        the bottom of the stack (\a __stack) and the top most address
 *        (\a _end).
 */
void StackPaint(void)
{
#if 0
    uint8_t *p = &_end;

    while(p <= &__stack)
    {
        *p = STACK_CANARY;
        p++;
    }
#else
    __asm volatile ("    ldi r30,lo8(_end)\n"
                    "    ldi r31,hi8(_end)\n"
                    "    ldi r24,lo8(0xc5)\n" /* STACK_CANARY = 0xc5 */
                    "    ldi r25,hi8(__stack)\n"
                    "    rjmp .cmp\n"
                    ".loop:\n"
                    "    st Z+,r24\n"
                    ".cmp:\n"
                    "    cpi r30,lo8(__stack)\n"
                    "    cpc r31,r25\n"
                    "    brlo .loop\n"
                    "    breq .loop"::);
#endif
}


/**************************************************************************
 * Global Functions
 **************************************************************************/

/** Count unused stack space.
 * This examines the stack space, and determines how many bytes have never
 * been overwritten.
 *
 * \returns  The count of bytes likely to have never been used by the stack.
 */
uint16_t StackCount(void)
{
    const uint8_t *p = &_end;
    uint16_t       c = 0;

    while(*p == STACK_CANARY && p <= &__stack)
    {
        p++;
        c++;
    }

    return c;
}

/* END OF FILE */
