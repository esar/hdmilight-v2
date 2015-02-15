/***************************************************************************
 * stackmon.h: Stack usage monitoring for ATmega88/168.
 * Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is PUBLIC DOMAIN.
 * This means that there is no copyright and anyone is able to take a copy
 * for free and use it as they wish, with or without modifications, and in
 * any context, commercially or otherwise. The only limitation is that I
 * don't guarantee that the software is fit for any purpose or accept any
 * liability for it's use or misuse - this software is without warranty.
 ***************************************************************************/

#ifndef STACKMON_H
#define STACKMON_H

/**************************************************************************
 * Nested Include Files
 **************************************************************************/

#include <stdint.h>

/**************************************************************************
 * Manifest Constants
 **************************************************************************/

/**************************************************************************
 * Type Definitions
 **************************************************************************/

/**************************************************************************
 * Macros
 **************************************************************************/

/**************************************************************************
 * Function Prototypes
 **************************************************************************/

#if !defined(ON_PC)
uint16_t StackCount(void);
#else
#define StackCount()    0
#endif

#endif /* STACKMON_H */

/* END OF FILE */

