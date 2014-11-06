#include <stdint.h>
#include "ambilight.h"


int fixed_9_9_fract(int32_t x, int numDigits)
{
	int multiplier = 1;
	while(numDigits--)
		multiplier *= 10;
	
	// x = abs(x)
	if(x & ((uint32_t)1 << 17))
		x = 0 - x;
	
	// mask so we only have the fractional part
	x &= (1 << 9) - 1;

	// multiply by 10^^numDigits (10, 100, 1000, etc.)
	x *= multiplier;

	// round up if the division has a remainder
	if(x & 0x1ff)
		return (x >> 9) + 1;
	else
		return x >> 9;
}

int fixed_9_9_int(int32_t x)
{
	if(x & ((uint32_t)1 << 17))
	{
		x = 0 - x;
		return 0 - ((x >> 9) & ((1 << 9) - 1));
	}
	else
		return (x >> 9) & ((1 << 9) - 1);
}

uint32_t getfixed_9_9(const char* p)
{
	uint32_t accum = 0;
	int divisor = 1;
	int gotPoint = 0;
	int neg = 0;

	if(*p == '-')
	{
		neg = 1;
		++p;
	}
	
	while(1)
	{
		if(*p == '.')
		{
			if(gotPoint)
				break;
			gotPoint = 1;
		}
		else
		{
			if(*p >= '0' && *p <= '9')
				accum = (accum * 10) + (*p - '0');
			else
				break;

			if(gotPoint)
				divisor *= 10;
		}
		++p;
	}

	accum = (accum << 9) / divisor;
	return neg ? 0 - accum : accum;
}

