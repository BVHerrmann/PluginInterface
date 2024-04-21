/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/* project    : TCP-Stack for Profinet                                                            */
/* file       : obsd_kernel_BSDStackPort.h                                                        */
/* description: include file with macros and constants that are valid for the whole stack port,   */
/*              this code will help us to solve problems that result from cutting off the         */
/*              IP-stack off the OpenBSD OS                                                       */
/* compiler   : ANSI C                                                                            */
/* maschine   : miscellaneous                                                                     */
/*                                                                                                */
/* history:                                                                                       */
/* version  date      author            change                                                    */
/* ======= ========== ================  ========================================================= */
/*  0.01         2012 Gerlach, ATS 1    initial coding                                            */
/*  1.00   13.03.2013 Gerlach, ATS 1    intoduced version header                                  */
/*  1.01   18.03.2013 Gerlach, ATS 1    function renaming for strncpy and abort                   */
/*                                      (requested by Sandra)                                     */
/*  1.02   30.04.2013 Gerlach, ATS 1    removed mmap and munmap                                   */
/*  1.03   25.07.2013 Gerlach, ATS 1    enabled enhanced kernel API for the stack test variant    */
/*  1.04   31.01.2014 Gerlach, ATS 1    added OBSD_UNUSED_ARG                                     */
/*  1.05   05.02.2014 Gerlach, ATS 1    added warning pragma (in a comment)                       */
/*  1.06   15.05.2014 Gerlach, ATS 1    added renaming for obsd_kernel_strlcpy                    */
/*  1.07   04.06.2014 Gerlach, ATS TM4  added userland renaming for obsd_userland_strcasecmp      */
/*  1.08   24.06.2014 Gerlach, ATS TM4  added #include for stack config file and moved constants  */
/*                                      for stack configuration to this file                      */
/* ---------------------------------------------------------------------------------------------- */

#ifndef __OBSD_BSD_STACKPORT_H__
#define	__OBSD_BSD_STACKPORT_H__

#include "tcip_obsd_cfg.h"
#include "obsd_platform_stack_config.h"                                 /* stack configuration    */
/* ##### this file is intended to be included from GENERIC\machine\cdefs.h ###################### */

#include "obsd_platform_cal_compiler_specifics.h"
#include "obsd_kernel_usr.h"

/* <TBD>: maybe we should have a separate BSDStackPort for Kernel and Userland and a common one   */
/*       (with va_start and so on)                                                                */


#ifndef BSD_STACKPORT
#define BSD_STACKPORT
#endif

#define OBSD_UNUSED_ARG(x) {(void)(x);}

/* v===================== type stuff ===========================================================v */
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define __wchar_t ___wchar_t

#ifndef __signed
#define __signed signed
#endif
/* ^============================================================================================^ */


/* v===================== userland and kernel function separation to avoid naming collicions ===v */
#ifdef    _KERNEL
/* kernel definitions */
#define putchar     obsd_kernel_putchar
#define __assert    obsd_kernel__assert
#define vprintf     obsd_kernel_vprintf 
#define malloc      obsd_kernel_malloc
#define free        obsd_kernel_free
#define memset      obsd_kernel_memset
#define snprintf    obsd_kernel_snprintf
#define db_printf   obsd_kernel_db_printf
#define strncpy     obsd_kernel_strncpy
#define strlcpy     obsd_kernel_strlcpy
#define strncmp     obsd_kernel_strncmp /* collision VS2010 */
#define memcmp      obsd_kernel_memcmp /* collision with C4 (see 2710554) compiler */
#define memcpy      obsd_kernel_memcpy /* collision with C4 (see 2710554) compiler */
#define strlen      obsd_kernel_strlen /* collision with C4 (see 2710554) compiler */
#define strcmp      obsd_kernel_strcmp /* collision with C4 (see 2710554) compiler */
#define vsnprintf   obsd_kernel_vsnprintf
#define bzero		obsd_kernel_bzero /* collision with os_infra library from C1 (see RQ2710554) */
#define bcopy		obsd_kernel_bcopy /* collision with os_infra library C1 (see RQ2710554) */
#define memchr      obsd_kernel_memchr /* collision with os_infra library C1 (see RQ2710554) */
#define strtoll     obsd_userland_strtoll /* collision with os_infra library C1 (see RQ2710554) */
#define qsort		obsd_userland_qsort
#define size_t		obsd_size_t /* sado -- Greenhills compiler warning 867 - declaration of size_t does not match the expected type */
#define SHA1Update	obsd_kernel_SHA1Update /* collision with library C1 (see RQ2710554) */
#define SHA1Init	obsd_kernel_SHA1Init /* collision with library C1 (see RQ2710554) */
#define SHA1Final	obsd_kernel_SHA1Final /* collision with library C1 (see RQ2710554) */
#else   /* _KERNEL */
/* userland definitions */
#define __assert    obsd_userland__assert
#define bzero		obsd_kernel_bzero /* collision with os_infra library C1 (see RQ2710554) */
#define bcopy		obsd_kernel_bcopy /* collision with os_infra library C1 (see RQ2710554) */
#define memchr      obsd_kernel_memchr /* collision with os_infra library C1 (see RQ2710554) */
#define strtoll     obsd_userland_strtoll /* collision with os_infra library C1 (see RQ2710554) */
#define vprintf     obsd_userland_vprintf 
#define fprintf     obsd_userland_fprintf 
#define snprintf    obsd_kernel_snprintf
#define malloc      obsd_userland_malloc
#define calloc      obsd_userland_calloc
#define realloc     obsd_userland_realloc
#define reallocarray     obsd_userland_reallocarray
#define free        obsd_userland_free
#define memset      obsd_kernel_memset
#define memmove     obsd_userland_memmove
#define memcpy      obsd_kernel_memcpy
#define strncpy     obsd_userland_strncpy
#define strlcpy     obsd_kernel_strlcpy
#define strcmp      obsd_kernel_strcmp
#ifndef strdup      /* sado - pntest does define strdup... */
#define strdup      obsd_userland_strdup
#endif
#define strlen		obsd_kernel_strlen
#define strsep		obsd_userland_strsep
#ifndef strcasecmp  /* sado - pntest does define strcasecmp... */
#define strcasecmp  obsd_userland_strcasecmp
#endif
#define exit        OSAL_event_panic_reboot
#define abort       obsd_userland_abort      /* abort seems to lead to a clib collision in VS2010 */
#define qsort		obsd_userland_qsort
#define size_t		obsd_size_t /* sado -- Greenhills compiler warning 867 - declaration of size_t does not match the expected type */
#endif  /* _KERNEL */

#ifndef    _KERNEL
/* userland definitions */
#define STRRCHR             /* normally defined in compiler cmd line options but requested by PN  */
                            /* development to remove it there                                     */
#define STRCHR              /* same */

/* functions often used in userland and redefined via __weak_alias in original OBSD source */
#define    _warnx warnx
#define    _warn  warn
#define    _err   err
#define    _errx  errx

/* also give them a prefix to avoid naming collisions with PN code and others because short names */
/* like err and warn could be in use in other software components                                 */
#define    warnx  obsd_userland_warnx
#define    warn   obsd_userland_warn
#define    err    obsd_userland_err
#define    errx   obsd_userland_errx

#endif  /* ! _KERNEL */
/* ^============================================================================================^ */

/* v===================== syscall renaming like necessary for PN integration e.g. ==============v */

#if !defined _KERNEL || defined SYSCALLS_INTERFACE    /* for userland and syscalls interface impl.*/
#include "obsd_platform_cal_rename_syscalls.h"
#endif  /* !_KERNEL */

/* ^============================================================================================^ */

/* v===================== clib stuff ===========================================================v */

/* see lib/libkern/explicit_bzero.c */
#define explicit_bzero(p, n) bzero(p, n)

/* ^============================================================================================^ */


#endif  /* __OBSD_BSD_STACKPORT_H__ */
