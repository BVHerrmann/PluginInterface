/*	$OpenBSD: subr_prf.c,v 1.76 2011/04/03 16:46:19 drahn Exp $	*/
/*	$NetBSD: subr_prf.c,v 1.45 1997/10/24 18:14:25 chuck Exp $	*/

/*-
 * Copyright (c) 1986, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)subr_prf.c	8.3 (Berkeley) 1/21/94
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_prf_act_module) */
#define LTRC_ACT_MODUL_ID 4006 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_prf_act_module) */
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_buf.h>
#include <sys/obsd_kernel_conf.h>
#include <sys/obsd_kernel_reboot.h>
#include <sys/obsd_kernel_msgbuf.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_vnode.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_tty.h>
#include <sys/obsd_kernel_tprintf.h>
#include <sys/obsd_kernel_syslog.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_pool.h>

#include <dev/obsd_kernel_cons.h>

/*
 * note that stdarg.h and the ansi style va_start macro is used for both
 * ansi and traditional c compilers.
 */
#include <sys/obsd_kernel_stdarg.h>

#ifdef KGDB
#include <sys/obsd_kernel_kgdb.h>
#include <machine/obsd_kernel_cpu.h>
#endif

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

/*
 * defines
 */

/* flags for kprintf */
#define TOCONS		0x01	/* to the console */
#define TOTTY		0x02	/* to the process' tty */
#define TOLOG		0x04	/* to the kernel message buffer */
#define TOBUFONLY	0x08	/* to the buffer (only) [for snprintf] */
#define TODDB		0x10	/* to ddb console */
#define TOCOUNT		0x20	/* act like [v]snprintf */

/* max size buffer kprintf needs to print quad_t [size in base 8 + \0] */
#define KPRINTF_BUFSIZE		(sizeof(quad_t) * NBBY / 3 + 2)


/*
 * local prototypes
 */

int	 kprintf(const char *, int, void *, char *, va_list);
void	 kputchar(int, int, struct tty *);


/*
 * globals
 */

extern struct	tty *constty;	/* pointer to console "window" tty */
extern	int log_open;	/* subr_log: is /dev/klog open? */
const	char *panicstr; /* arg to first call to panic (used as a flag
			   to indicate that panic has already been called). */

/*
 * panic on spl assertion failure?
 */
int splassert_ctl = 1;

/*
 * v_putc: routine to putc on virtual console
 *
 * the v_putc pointer can be used to redirect the console cnputc elsewhere
 * [e.g. to a "virtual console"].
 */

void (*v_putc)(int) = cnputc;	/* start with cnputc (normal cons) */


/*
 * functions
 */

/*
 *	Partial support (the failure case) of the assertion facility
 *	commonly found in userland.
 */
void
__assert(const char *t, const char *f, int l, const char *e)
{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_prf_001) */
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_FATAL, "Assertion failed, line %d", l);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_prf_001) */
	panic("kernel %sassertion \"%s\" failed: file \"%s\", line %d",
		t, e, f, l);
}

/*
 * tablefull: warn that a system table is full
 */

void
tablefull(const char *tab)
{
	OBSD_UNUSED_ARG(tab);
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_prf_002) */
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "Table is full");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_prf_002) */
	log(LOG_ERR, "%s: table is full\n", tab);
}

/*
 * panic: handle an unresolvable fatal error
 *
 * prints "panic: <message>" and reboots.   if called twice (i.e. recursive
 * call) we avoid trying to sync the disk and just reboot (to avoid
 * recursive panics).
 */

#if    OBSD_POOL_MALLOC_NO_SLEEP != 0
/* gh2289n: in case of OBSD_POOL_MALLOC_NO_SLEEP != 0 make sure not to use any fd within panic() because we also     */
/*          panic in case of missing memory for the fd table, see Kernel_init()                                      */
#endif /* OBSD_POOL_MALLOC_NO_SLEEP */

void
panic(const char *fmt, ...)
{
	OBSD_UNUSED_ARG(fmt);
	boot(0);
}

/*
 * We print only the function name. The file name is usually very long and
 * would eat tons of space in the kernel.
 */
void
splassert_fail(int wantipl, int haveipl, const char *func)
{
	OBSD_UNUSED_ARG(wantipl);
	OBSD_UNUSED_ARG(haveipl);
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_prf_003) */
	TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR,"splassert: want %d have %d", wantipl, haveipl);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_prf_003) */
	printf("splassert: %s: want %d have %d\n", func, wantipl, haveipl);
	switch (splassert_ctl) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_prf_004) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL,"spl assertion failure");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_prf_004) */
		panic("spl assertion failure in %s", func);
	}
}


/*
 * kputchar: print a single character on console or user terminal.
 *
 * => if console, then the last MSGBUFS chars are saved in msgbuf
 *	for inspection later (e.g. dmesg/syslog)
 */
void
kputchar(int c, int flags, struct tty *tp)
{
	OBSD_UNUSED_ARG(tp);/* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* if ( flags & ~(TOCONS | TOLOG | TODDB) )
	 *{   / * unknown log flag * /
	 *	volatile int a = 3; / * for debugging only * /
	 *}
	 */
	
	if ( ((flags & TOCONS) || (flags & TOLOG)) && c != '\0')
		(*v_putc)(c);
}


/*
 * uprintf: print to the controlling tty of the current process
 *
 * => we may block if the tty queue is full
 * => no message is printed if the queue doesn't clear in a reasonable
 *	time
 */

void
uprintf(const char *fmt, ...)
{
	struct process *pr = curproc->p_p;
	va_list ap;

	if (pr->ps_flags & PS_CONTROLT && pr->ps_session->s_ttyvp) {
		va_start(ap, fmt);
		kprintf(fmt, TOTTY, pr->ps_session->s_ttyp, NULL, ap);
		va_end(ap);
	}
}

#if defined(NFSSERVER) || defined(NFSCLIENT)

/*
 * tprintf functions: used to send messages to a specific process
 *
 * usage:
 *   get a tpr_t handle on a process "p" by using "tprintf_open(p)"
 *   use the handle when calling "tprintf"
 *   when done, do a "tprintf_close" to drop the handle
 */

/*
 * tprintf_open: get a tprintf handle on a process "p"
 * XXX change s/proc/process
 *
 * => returns NULL if process can't be printed to
 */

tpr_t
tprintf_open(struct proc *p)
{
	struct process *pr = p->p_p;

	if (pr->ps_flags & PS_CONTROLT && pr->ps_session->s_ttyvp) {
		SESSHOLD(pr->ps_session);
		return ((tpr_t)pr->ps_session);
	}
	return ((tpr_t) NULL);
}

/*
 * tprintf_close: dispose of a tprintf handle obtained with tprintf_open
 */

void
tprintf_close(tpr_t sess)
{

	if (sess)
		SESSRELE((struct session *) sess);
}

/*
 * tprintf: given tprintf handle to a process [obtained with tprintf_open],
 * send a message to the controlling tty for that process.
 *
 * => also sends message to /dev/klog
 */
void
tprintf(tpr_t tpr, const char *fmt, ...)
{
	struct session *sess = (struct session *)tpr;
	struct tty *tp = NULL;
	int flags = TOLOG;
	va_list ap;

	logpri(LOG_INFO);
	if (sess && sess->s_ttyvp && ttycheckoutq(sess->s_ttyp, 0)) {
		flags |= TOTTY;
		tp = sess->s_ttyp;
	}
	va_start(ap, fmt);
	kprintf(fmt, flags, tp, NULL, ap);
	va_end(ap);
	logwakeup();
}

#endif	/* NFSSERVER || NFSCLIENT */


/*
 * ttyprintf: send a message to a specific tty
 *
 * => should be used only by tty driver or anything that knows the
 *	underlying tty will not be revoked(2)'d away.  [otherwise,
 *	use tprintf]
 */
void
ttyprintf(struct tty *tp, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	kprintf(fmt, TOTTY, tp, NULL, ap);
	va_end(ap);
}

/*
 * snprintf: print a message to a buffer
 */
int
snprintf(char *buf, size_t size, const char *fmt, ...)
{
	int retval;
	va_list ap;
	char *p;

	p = buf + size - 1;
	if (size < 1)
		p = buf;
	va_start(ap, fmt);
	retval = kprintf(fmt, TOBUFONLY | TOCOUNT, &p, buf, ap);
	va_end(ap);
	if (size > 0)
		*(p) = 0;	/* null terminate */
	return(retval);
}

/*
 * vsnprintf: print a message to a buffer [already have va_alist]
 */
int
vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
	int retval;
	char *p;

	p = buf + size - 1;
	if (size < 1)
		p = buf;
	retval = kprintf(fmt, TOBUFONLY | TOCOUNT, &p, buf, ap);
	if (size > 0)
		*(p) = 0;	/* null terminate */
	return(retval);
}

/*
 * kprintf: scaled down version of printf(3).
 *
 * this version based on vfprintf() from libc which was derived from
 * software contributed to Berkeley by Chris Torek.
 *
 * The additional format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	kprintf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 *
 * To support larger integers (> 32 bits), %b formatting will also accept
 * control characters in the region 0x80 - 0xff.  0x80 refers to bit 0,
 * 0x81 refers to bit 1, and so on.  The equivalent string to the above is:
 *
 *	kprintf("reg=%b\n", 3, "\10\201BITTWO\200BITONE\n");
 *
 * and would produce the same output.
 *
 * Like the rest of printf, %b can be prefixed to handle various size
 * modifiers, eg. %b is for "int", %lb is for "long", and %llb supports
 * "long long".
 *
 * This code is large and complicated...
 */

/*
 * macros for converting digits to letters and vice versa
 */
#define	to_digit(c)	((c) - '0')
#define is_digit(c)	((unsigned)to_digit(c) <= 9)
#define	to_char(n)	((n) + '0')

/*
 * flags used during conversion.
 */
#define	ALT		0x001		/* alternate form */
#define	HEXPREFIX	0x002		/* add 0x or 0X prefix */
#define	LADJUST		0x004		/* left adjustment */
#define	LONGDBL		0x008		/* long double; unimplemented */
#define	LONGINT		0x010		/* long integer */
#define	QUADINT		0x020		/* quad integer */
#define	SHORTINT	0x040		/* short integer */
#define	ZEROPAD		0x080		/* zero (as opposed to blank) pad */
#define FPT		0x100		/* Floating point number */
#define SIZEINT		0x200		/* (signed) size_t */

	/*
	 * To extend shorts properly, we need both signed and unsigned
	 * argument extraction methods.
	 */
#define	SARG() \
	(flags&QUADINT ? va_arg(ap, quad_t) : \
	    flags&LONGINT ? va_arg(ap, long) : \
	    flags&SIZEINT ? va_arg(ap, ssize_t) : \
	    flags&SHORTINT ? (long)(short)va_arg(ap, int) : \
	    (long)va_arg(ap, int))
#define	UARG() \
	(flags&QUADINT ? va_arg(ap, u_quad_t) : \
	    flags&LONGINT ? va_arg(ap, u_long) : \
	    flags&SIZEINT ? va_arg(ap, size_t) : \
	    flags&SHORTINT ? (u_long)(u_short)va_arg(ap, int) : \
	    (u_long)va_arg(ap, u_int))

#define KPRINTF_PUTCHAR(C) do {					\
	int chr = (C);							\
	ret += 1;							\
	if (oflags & TOBUFONLY) {					\
		if ((vp != NULL) && (sbuf == tailp)) {			\
			if (!(oflags & TOCOUNT))				\
				goto overflow;				\
		} else							\
			*sbuf++ = (char)chr;					 /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */\
	} else {							\
		kputchar(chr, oflags, (struct tty *)vp);			\
	}								\
} while(0)

int
kprintf(const char *fmt0, int oflags, void *vp, char *sbuf, va_list ap)
{
	char *fmt;		/* format string */
	int ch;			/* character from fmt */
	int n;			/* handy integer (short term usage) */
	char *cp = NULL;	/* handy char pointer (short term usage) */
	int flags;		/* flags as above */
	int ret;		/* return value accumulator */
	int width;		/* width from format (%8d), or 0 */
	int prec;		/* precision from format (%.3d), or -1 */
	char sign;		/* sign prefix (' ', '+', '-', or \0) */

	u_quad_t _uquad;	/* integer arguments %[diouxX] */
	enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
	int dprec;		/* a copy of prec if [diouxX], 0 otherwise */
	int realsz;		/* field size expanded by dprec */
	int size = 0;		/* size of converted field or string */
	char *xdigs = NULL;	/* digits for [xX] conversion */
	char buf[KPRINTF_BUFSIZE]; /* space for %c, %[diouxX] */
	char *tailp = NULL;	/* tail pointer for snprintf */

	if ((oflags & TOBUFONLY) && (vp != NULL))
		tailp = *(char **)vp;

	fmt = (char *)fmt0;
	ret = 0;

	/*
	 * Scan the format for conversions (`%' character).
	 */
	for (;;) {
		while (*fmt != '%' && *fmt) {
			KPRINTF_PUTCHAR(*fmt++);
		}
		if (*fmt == 0)
			goto done;

		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
		width = 0;
		prec = -1;
		sign = '\0';

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
		/* XXX: non-standard '%b' format */
		case 'b': {
			char *b, *z;
			int tmp;
			_uquad = UARG();
			b = va_arg(ap, char *);
			if (*b == 8)
				snprintf(buf, sizeof buf, "%llo", _uquad);
			else if (*b == 10)
				snprintf(buf, sizeof buf, "%lld", _uquad);
			else if (*b == 16)
				snprintf(buf, sizeof buf, "%llx", _uquad);
			else
				break;
			b++;

			z = buf;
			while (*z) {
				KPRINTF_PUTCHAR(*z++);
			}

			if (_uquad) {
				tmp = 0;
				while ((n = *b++) != 0) {
					if (n & 0x80)
						n &= 0x7f;
					else if (n <= ' ')
						n = n - 1;
					if (_uquad & (1LL << n)) {
						KPRINTF_PUTCHAR(tmp ? ',':'<');
						while (*b > ' ' &&
						    (*b & 0x80) == 0) {
							KPRINTF_PUTCHAR(*b);
							b++;
						}
						tmp = 1;
					} else {
						while (*b > ' ' &&
						    (*b & 0x80) == 0)
							b++;
					}
				}
				if (tmp) {
					KPRINTF_PUTCHAR('>');
				}
			}
			continue;	/* no output */
		}

		case ' ':
			/*
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign)
				sign = ' ';
			goto rflag;
		case '#':
			flags |= ALT;
			goto rflag;
		case '*':
			/*
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			if ((width = va_arg(ap, int)) >= 0)
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if ((ch = *fmt++) == '*') {
				n = va_arg(ap, int);
				prec = n < 0 ? -1 : n;
				goto rflag;
			}
			n = 0;
			while (is_digit(ch)) {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			}
			prec = n < 0 ? -1 : n;
			goto reswitch;
		case '0':
			/*
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= ZEROPAD;
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			} while (is_digit(ch));
			width = n;
			goto reswitch;
		case 'h':
			flags |= SHORTINT;
			goto rflag;
		case 'l':
			if (*fmt == 'l') {
				fmt++;
				flags |= QUADINT;
			} else {
				flags |= LONGINT;
			}
			goto rflag;
		case 'q':
			flags |= QUADINT;
			goto rflag;
		case 'z':
			flags |= SIZEINT;
			goto rflag;
		case 'c':
			*(cp = buf) = (char) (va_arg(ap, int)); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
			size = 1;
			sign = '\0';
			break;
		case 'D':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG();
			if ((quad_t)_uquad < 0) {
				_uquad = -((quad_t)_uquad);/* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4146 */
				sign = '-';
			}
			base = DEC;
			goto number;
		case 'n':
			if (flags & QUADINT)
				*va_arg(ap, quad_t *) = ret;
			else if (flags & LONGINT)
				*va_arg(ap, long *) = ret;
			else if (flags & SHORTINT)
				*va_arg(ap, short *) = (short) ret; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
			else if (flags & SIZEINT)
				*va_arg(ap, ssize_t *) = ret;
			else
				*va_arg(ap, int *) = ret;
			continue;	/* no output */
		case 'O':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG();
			base = OCT;
			goto nosign;
		case 'p':
			/*
			 * ``The argument shall be a pointer to void.  The
			 * value of the pointer is converted to a sequence
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (u_long)va_arg(ap, void *);
			base = HEX;
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
			ch = 'x';
			goto nosign;
		case 's':
			if ((cp = va_arg(ap, char *)) == NULL)
				cp = "(null)";
			if (prec >= 0) {
				/*
				 * can't use strlen; can only look for the
				 * NUL in the first `prec' characters, and
				 * strlen() will go further.
				 */
				char *p = memchr(cp, 0, prec);

				if (p != NULL) {
					size = (int)(p - cp); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
					if (size > prec)
						size = prec;
				} else
					size = prec;
			} else
				size = (int) strlen(cp); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
			sign = '\0';
			break;
		case 'U':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG();
			base = DEC;
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG();
			base = HEX;
			/* leading 0x/X only if non-zero */
			if (flags & ALT && _uquad != 0)
				flags |= HEXPREFIX;

			/* unsigned conversions */
nosign:			sign = '\0';
			/*
			 * ``... diouXx conversions ... if a precision is
			 * specified, the 0 flag will be ignored.''
			 *	-- ANSI X3J11
			 */
number:			if ((dprec = prec) >= 0)
				flags &= ~ZEROPAD;

			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + KPRINTF_BUFSIZE;
			if (_uquad != 0 || prec != 0) {
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
				case OCT:
					do {
						*--cp = to_char((char)(_uquad & 7));/* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
						_uquad >>= 3;
					} while (_uquad);
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
						*--cp = '0';
					break;

				case DEC:
					/* many numbers are 1 digit */
					while (_uquad >= 10) {
						*--cp = to_char((char)(_uquad % 10));/* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
						_uquad /= 10;
					}
					*--cp = to_char((char)(_uquad));/* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
					break;

				case HEX:
					do {
						*--cp = xdigs[_uquad & 15];
						_uquad >>= 4;
					} while (_uquad);
					break;

				default:
					cp = "bug in kprintf: bad base";
					size = (int) strlen(cp);/* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
					goto skipsize;
				}
			}
			size = (int)(buf + KPRINTF_BUFSIZE - cp); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
		skipsize:
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = (char) ch;/* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
			size = 1;
			sign = '\0';
			break;
		}

		/*
		 * All reasonable formats wind up here.  At this point, `cp'
		 * points to a string which (if not flags&LADJUST) should be
		 * padded out to `width' places.  If flags&ZEROPAD, it should
		 * first be prefixed by any sign or other prefix; otherwise,
		 * it should be blank padded before the prefix is emitted.
		 * After any left-hand padding and prefixing, emit zeroes
		 * required by a decimal [diouxX] precision, then print the
		 * string proper, then emit zeroes required by any leftover
		 * floating precision; finally, if LADJUST, pad with blanks.
		 *
		 * Compute actual size, so we know how much to pad.
		 * size excludes decimal prec; realsz includes it.
		 */
		realsz = dprec > size ? dprec : size;
		if (sign)
			realsz++;
		else if (flags & HEXPREFIX)
			realsz+= 2;

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0) {
			n = width - realsz;
			while (n-- > 0)
				KPRINTF_PUTCHAR(' ');
		}

		/* prefix */
		if (sign) {
			KPRINTF_PUTCHAR(sign);
		} else if (flags & HEXPREFIX) {
			KPRINTF_PUTCHAR('0');
			KPRINTF_PUTCHAR(ch);
		}

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD) {
			n = width - realsz;
			while (n-- > 0)
				KPRINTF_PUTCHAR('0');
		}

		/* leading zeroes from decimal precision */
		n = dprec - size;
		while (n-- > 0)
			KPRINTF_PUTCHAR('0');

		/* the string or number proper */
		while (size--)
			KPRINTF_PUTCHAR(*cp++);
		/* left-adjusting padding (always blank) */
		if (flags & LADJUST) {
			n = width - realsz;
			while (n-- > 0)
				KPRINTF_PUTCHAR(' ');
		}
	}

done:
	if ((oflags & TOBUFONLY) && (vp != NULL))
		*(char **)vp = sbuf;

overflow:
	return (ret);
	/* NOTREACHED */
}

#if __GNUC_PREREQ__(2,96)
/*
 * XXX - these functions shouldn't be in the kernel, but gcc 3.X feels like
 *       translating some printf calls to puts and since it doesn't seem
 *       possible to just turn off parts of those optimizations (some of
 *       them are really useful), we have to provide a dummy puts and putchar
 *	 that are wrappers around printf.
 */
int	puts(const char *);
int	putchar(int c);

int
puts(const char *str)
{
	printf("%s\n", str);

	return (0);
}

int
putchar(int c)
{
	printf("%c", c);

	return (c);
}


#endif
