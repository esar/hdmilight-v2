#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>

//-----------------------------------------------------------------
// Types:
//-----------------------------------------------------------------
typedef int (*FP_OUTCHAR)(char c);

#ifdef LIBSTD_SIZE_T_2
	typedef long unsigned int libsize_t;
#else
	typedef unsigned int libsize_t;
#endif

//-----------------------------------------------------------------
// Structures
//-----------------------------------------------------------------
struct vbuf
{
	FP_OUTCHAR  function;
	char *		buffer;
	int			offset;
	int			max_length;
};

//-----------------------------------------------------------------
// Prototypes:
//-----------------------------------------------------------------
int		printf(const char* ctrl1, ... );
void	printf_register(FP_OUTCHAR f);
int		vsprintf(char *s, const char *format, va_list arg);
int		vsnprintf(char *s, libsize_t maxlen, const char *format, va_list arg);
int		sprintf(char *s, const char *format, ...);
int		snprintf(char *s, libsize_t maxlen, const char *format, ...);
int		vbuf_printf(struct vbuf *buf, const char* ctrl1, va_list argp);

#define PRINTF		printf

#endif // __PRINTF_H__
