#include "printf.h"
#include "string.h"

//----------------------------------------------------
// Locals
//----------------------------------------------------
static FP_OUTCHAR	_stdout = 0;

//----------------------------------------------------
// printf: Console based printf
//----------------------------------------------------
int printf( const char* ctrl1, ... )
{
	int res = 0;
	va_list argp;
	struct vbuf buf;

	if (_stdout && ctrl1)
	{
		va_start( argp, ctrl1);
		
		// Setup target to be stdout function
		buf.function = _stdout;
		buf.buffer = 0;
		buf.offset = 0;
		buf.max_length = 0;

		res = vbuf_printf(&buf, ctrl1, argp);

		va_end( argp);
	}

	return res;
}
//----------------------------------------------------
// printf_register: Assign printf output function
//----------------------------------------------------
void printf_register(FP_OUTCHAR f)
{
	_stdout = f;
}
//----------------------------------------------------
// vsprintf: 
//----------------------------------------------------
int vsprintf(char *s, const char *format, va_list arg)
{
	struct vbuf buf;

	if (!s || !format)
		return 0;

	// Setup buffer to be target
	buf.function = 0;
	buf.buffer = s;
	buf.offset = 0;
	buf.max_length = 32768; // default

	vbuf_printf(&buf, format, arg);

	// Null terminate at end of string
	buf.buffer[buf.offset] = 0;

	return buf.offset;
}
//----------------------------------------------------
// vsnprintf: 
//----------------------------------------------------
int vsnprintf( char *s, libsize_t maxlen, const char *format, va_list arg)
{
	struct vbuf buf;

	if (!s || !format || !maxlen)
		return 0;

	// Setup buffer to be target
	buf.function = 0;
	buf.buffer = s;
	buf.offset = 0;
	buf.max_length = maxlen;

	vbuf_printf(&buf, format, arg);

	// Null terminate at end of string
	buf.buffer[buf.offset] = 0;

	return buf.offset;
}
//----------------------------------------------------
// sprintf: 
//----------------------------------------------------
int sprintf(char *s, const char *format, ...)
{
	va_list argp;
	struct vbuf buf;

	if (!s || !format)
		return 0;

	va_start( argp, format);

	// Setup buffer to be target
	buf.function = 0;
	buf.buffer = s;
	buf.offset = 0;
	buf.max_length = 32768; // default

	vbuf_printf(&buf, format, argp);

	// Null terminate at end of string
	buf.buffer[buf.offset] = 0;

	va_end( argp);

	return buf.offset;
}
//----------------------------------------------------
// snprintf: 
//----------------------------------------------------
int snprintf(char *s, libsize_t maxlen, const char *format, ...)
{
	va_list argp;
	struct vbuf buf;

	if (!maxlen || !s || !format)
		return 0;

	va_start( argp, format);

	// Setup buffer to be target
	buf.function = 0;
	buf.buffer = s;
	buf.offset = 0;
	buf.max_length = maxlen;

	vbuf_printf(&buf, format, argp);

	// Null terminate
	if (buf.offset < buf.max_length)
		buf.buffer[buf.offset] = 0;
	else
		buf.buffer[buf.max_length-1] = 0;

	va_end( argp);

	return buf.offset;
}
//----------------------------------------------------
// puts:
//----------------------------------------------------
int puts( const char * str )
{
	if (!_stdout)
		return -1;

	while (*str)
		_stdout(*str++);

	return _stdout('\n');
}

//----------------------------------------------------
//			Printf Implementation
//----------------------------------------------------

//----------------------------------------------------
// Structures
//----------------------------------------------------
typedef struct params_s 
{
    long len;
    long num1;
    long num2;
    char pad_character;
    int do_padding;
    int left_flag;
} params_t;

//----------------------------------------------------
// vbuf_putchar: vbuf_printf output function (directs
// output to either function or buffer.
//----------------------------------------------------
static void vbuf_putchar(struct vbuf *buf, char c)
{
	// Function is target
	if (buf->function)
		buf->function(c);
	// Buffer is target
	else if (buf->buffer)
	{
		if (buf->offset < buf->max_length)
			buf->buffer[buf->offset++] = c;
	}
}
//----------------------------------------------------
/*                                                   */
/* This routine puts pad characters into the output  */
/* buffer.                                           */
/*                                                   */
//----------------------------------------------------
static void padding( struct vbuf *buf, const long l_flag, params_t *par)
{
    long i;

    if (par->do_padding && l_flag && (par->len < par->num1))
        for (i=par->len; i<par->num1; i++)
            vbuf_putchar(buf, par->pad_character);
}
//----------------------------------------------------
/*                                                   */
/* This routine moves a string to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
//----------------------------------------------------
static void outs( struct vbuf *buf, char* lp, params_t *par)
{
    /* pad on left if needed                         */
    par->len = strlen( lp);
    padding(buf, !(par->left_flag), par);

    /* Move string to the buffer                     */
    while (*lp && (par->num2)--)
        vbuf_putchar(buf, *lp++);

    /* Pad on right if needed                        */
    par->len = strlen( lp);
    padding(buf, par->left_flag, par);
}
//----------------------------------------------------
/*                                                   */
/* This routine moves a number to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
//----------------------------------------------------
static void outnum( struct vbuf *buf, const long n, const long base, params_t *par)
{
    char* cp;
    int negative;
    char outbuf[32];
    const char digits[] = "0123456789ABCDEF";
    unsigned long num;

    /* Check if number is negative                   */
    if (base == 10 && n < 0L) {
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
        *cp++ = digits[(long)(num % base)];
    } while ((num /= base) > 0);
    if (negative)
        *cp++ = '-';
    *cp-- = 0;

    /* Move the converted number to the buffer and   */
    /* add in the padding where needed.              */
    par->len = strlen(outbuf);
    padding(buf, !(par->left_flag), par);
    while (cp >= outbuf)
        vbuf_putchar(buf, *cp--);
    padding(buf, par->left_flag, par);
}
//----------------------------------------------------
/*                                                   */
/* This routine gets a number from the format        */
/* string.                                           */
/*                                                   */
//----------------------------------------------------
static long getnum( char** linep)
{
    long n;
    char* cp;

    n = 0;
    cp = *linep;
    while (((*cp) >= '0' && (*cp) <= '9'))
        n = n*10 + ((*cp++) - '0');
    *linep = cp;
    return(n);
}
//----------------------------------------------------
/*                                                   */
/* This routine operates just like a printf/sprintf  */
/* routine. It outputs a set of data under the       */
/* control of a formatting string. Not all of the    */
/* standard C format control are supported. The ones */
/* provided are primarily those needed for embedded  */
/* systems work. Primarily the floaing point         */
/* routines are omitted. Other formats could be      */
/* added easily by following the examples shown for  */
/* the supported formats.                            */
/*                                                   */
//----------------------------------------------------
int vbuf_printf(struct vbuf *buf, const char* ctrl1, va_list argp)
{
    int long_flag;
    int dot_flag;
	int res = 0;

    params_t par;

    char ch;
    char* ctrl = (char*)ctrl1;

    for ( ; *ctrl; ctrl++) 
	{
        /* move format string chars to buffer until a  */
        /* format control is found.                    */
        if (*ctrl != '%') 
		{
			vbuf_putchar(buf, *ctrl);
            continue;
        }

        /* initialize all the flags for this format.   */
        dot_flag   = long_flag = par.left_flag = par.do_padding = 0;
        par.pad_character = ' ';
        par.num2=32767;

 try_next:
        ch = *(++ctrl);

        if ((ch >= '0' && ch <= '9')) 
		{
            if (dot_flag)
                par.num2 = getnum(&ctrl);
            else {
                if (ch == '0')
                    par.pad_character = '0';

		if(*ctrl == '*')
			par.num1 = va_arg(argp, int);
		else
	                par.num1 = getnum(&ctrl);
                par.do_padding = 1;
            }
            ctrl--;
            goto try_next;
        }

        switch (((ch >= 'A' && ch <= 'Z') ? ch + 32: ch)) 
		{
            case '%':
				vbuf_putchar(buf, '%');
                continue;

            case '-':
                par.left_flag = 1;
                break;

            case '.':
                dot_flag = 1;
                break;

            case 'l':
                long_flag = 1;
                break;

            case 'd':
                if (long_flag || ch == 'D') {
                    outnum(buf, va_arg(argp, long), 10L, &par);
                    continue;
                }
                else {
                    outnum(buf, va_arg(argp, int), 10L, &par);
                    continue;
                }
            case 'x':
            case 'p':
                outnum(buf, (long)va_arg(argp, long), 16L, &par);
                continue;

            case 's':
                outs(buf, va_arg( argp, char*), &par);
                continue;

            case 'c':
				vbuf_putchar(buf, va_arg( argp, long));
                continue;

            case '\\':
                switch (*ctrl) {
                    case 'a':
                        vbuf_putchar(buf, 0x07);
                        break;
                    case 'h':
                        vbuf_putchar(buf, 0x08);
                        break;
                    case 'r':
                        vbuf_putchar(buf, 0x0D);
                        break;
                    case 'n':
                        vbuf_putchar(buf, 0x0A);
                        break;
                    default:
                        vbuf_putchar(buf, *ctrl);
                        break;
                }
                ctrl++;
                break;

            default:
                continue;
        }
        goto try_next;
    }

	return res;
}
