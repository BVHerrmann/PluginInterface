/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/* project    : TCP-Stack for Profinet                                                            */
/* file       : obsd_platform_cal_stdarg.h                                                        */
/* description: include file that allows to define the source of the var-arg types, macros and    */
/*              functions                                                                         */
/* compiler   : ANSI C                                                                            */
/* maschine   : miscellaneous                                                                     */
/*                                                                                                */
/* history:                                                                                       */
/* version  date      author            change                                                    */
/* ======= ========== ================  ========================================================= */
/*  1.00   13.03.2013 Gerlach, ATS 1    initial coding                                            */
/* ---------------------------------------------------------------------------------------------- */

#ifndef __OBSD_PLATFORM_CAL_STDARG_H__
#define __OBSD_PLATFORM_CAL_STDARG_H__


/* v===================== vararg stuff =========================================================v */

#ifdef _MSC_VER /* Microsoft C Compiler */

#if _MSC_VER == 1600 || _MSC_VER == 1700 || _MSC_VER == 1800 /* Visual Studio 2010, 2012, 2013 -- \Microsoft Visual Studio 10.0\VC\include\; stdarg.h; vadefs.h */
#define _ADDRESSOF(v)   ( &(v) )
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define _crt_va_start(ap,v)  ( ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
#define _crt_va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define _crt_va_end(ap)      ( ap = (va_list)0 )
#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end
#elif _MSC_VER >= 1900 && _MSC_VER < 1919 /* Visual Studio 2015/17 -- \Microsoft Visual Studio 14.0\VC\include\; stdarg.h; vadefs.h */
#define _ADDRESSOF(v) (&(v))
#define _INTSIZEOF(n)          ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define __crt_va_start_a(ap, v) ((void)(ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v)))
#define __crt_va_arg(ap, t)     (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define __crt_va_end(ap)        ((void)(ap = (va_list)0))
#define __crt_va_start(ap, x) __crt_va_start_a(ap, x)
#define va_start __crt_va_start
#define va_arg   __crt_va_arg
#define va_end   __crt_va_end
#define va_copy(destination, source) ((destination) = (source))
#else
#error "Undefined compiler"
#endif

#elif defined __GNUC__
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)
#define __va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list    va_list;

#elif defined __ghs__
// Set macros according to Green Hills ARM compiler ccarm. The implementation
// was determined by examining preprocessor output produced by preprocessing
// Green Hills file ansi\stdarg.h.

typedef char *__gh_va_list;

# define va_end(list)  ((void)0)
# ifndef __GHS_NO_VACOPY
#   ifdef __va_copy_as_struct
#     define va_copy(dest, src) ((dest)[0] = (src)[0])
#   else
#     define va_copy(dest, src) ((dest) = (src))
#   endif
# endif /* __GHS_NO_VACOPY */

# define va_start(list,lstprm) list = (char *) (&__va_ansiarg)
# define va_arg(list,typ) (((sizeof(typ) < 4) && __va_regtyp(typ))?*((typ *)((list += 4)-4)):*((typ *)((list += ((sizeof(typ)+3) & ~3))-((sizeof(typ)+3) & ~3))))
# if !defined(__va_list_defined)
#   define __va_list_defined
    typedef __gh_va_list va_list;
# endif

#define __func__  __FUNCTION__

#else
#error Undefined compiler
#endif

/* ^===================== vararg stuff =========================================================^ */

#endif /* __OBSD_PLATFORM_CAL_STDARG_H__ */

