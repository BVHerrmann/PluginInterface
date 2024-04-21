/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/* project    : TCP-Stack for Profinet                                                            */
/* file       : obsd_platform_cal_rename_syscalls.h                                               */
/* description: include file that allows renaming of userland Stack functions with the special    */
/*              intention to be used for renaming the userland syscall-functions in syscalls.c    */
/*              and all of it's users (note: this file is only included if the macro              */
/*              _KERNEL is not defined, that means userland code only)                            */
/* compiler   : ANSI C                                                                            */
/* maschine   : miscellaneous                                                                     */
/*                                                                                                */
/* history:                                                                                       */
/* version  date      author            change                                                    */
/* ======= ========== ================  ========================================================= */
/*  1.00   13.03.2013 Gerlach, ATS 1    initial coding                                            */
/*  1.01   04.06.2013 Gerlach, ATS 1    added sigaction call                                      */
/*  1.02   06.06.2013 Gerlach, ATS 1    added fcntl + setrlimit                                   */
/*  1.03   10.06.2013 Gerlach, ATS 1    added clock_gettime                                       */
/*  1.04   13.09.2013 Gerlach, ATS 1    added kill                                                */
/* ---------------------------------------------------------------------------------------------- */

#ifndef __OBSD_PLATFORM_CAL_RENAME_SYSCALLS_H__
#define __OBSD_PLATFORM_CAL_RENAME_SYSCALLS_H__

/* v===================== rename the syscall functions =========================================v */
#define _exit           obsd_exit                                       /* 1 = exit               */ /*  1 */
#define fork_ex         obsd_fork_ex                                    /* 2 = fork               */ /*  2 */
#define read            obsd_read                                       /* 3 = read               */ /*  3 */
#define write           obsd_write                                      /* 4 = write              */ /*  4 */
#define close           obsd_close                                      /* 6 = close              */ /*  5 */
#define getpid          obsd_getpid                                     /* 20 = getpid            */ /*  6 */
#define getuid          obsd_getuid                                     /* 24 = getuid            */ /*  7 */
#define geteuid         obsd_geteuid                                    /* 25 = geteuid           */ /*  8 */
#define recvmsg         obsd_recvmsg                                    /* 27 = recvmsg           */ /*  9 */
#define sendmsg         obsd_sendmsg                                    /* 28 = sendmsg           */ /* 10 */
#define recvfrom        obsd_recvfrom                                   /* 29 = recvfrom          */ /* 11 */
#define accept          obsd_accept                                     /* 30 = accept            */ /* 12 */
#define getpeername     obsd_getpeername                                /* 31 = getpeername       */ /* 13 */
#define getsockname     obsd_getsockname                                /* 32 = getsockname       */ /* 14 */
#define kill            obsd_kill                                       /* 37 = kill              */ /* 15 */
#define sigaction       obsd_sigaction                                  /* 46 = sigaction         */ /* 16 */
#define ioctl           obsd_ioctl                                      /* 54 = ioctl             */ /* 17 */
#define vfork_ex        obsd_vfork_ex                                   /* 66 = vfork             */ /* 18 */
#define munmap          obsd_munmap                                     /* 73 = munmap            */ /* 19 */
#define select          obsd_select                                     /* 93 = select            */ /* 20 */
#define socket          obsd_socket                                     /* 97 = socket            */ /* 21 */
#define connect         obsd_connect                                    /* 98 = connect           */ /* 22 */
#define bind            obsd_bind                                       /* 104 = bind             */ /* 23 */
#define setsockopt      obsd_setsockopt                                 /* 105 = setsockopt       */ /* 24 */
#define listen          obsd_listen                                     /* 106 = listen           */ /* 25 */
#define gettimeofday    obsd_gettimeofday                               /* 116 = gettimeofday     */ /* 26 */
#define getsockopt      obsd_getsockopt                                 /* 118 = getsockopt       */ /* 27 */
#define writev          obsd_writev                                     /* 121 = writev           */ /* 28 */
#define sendto          obsd_sendto                                     /* 133 = sendto           */ /* 29 */
#define shutdown        obsd_shutdown                                   /* 134 = shutdown         */ /* 30 */
#define socketpair      obsd_socketpair                                 /* 135 = socketpair       */ /* 31 */
#define getrlimit       obsd_getrlimit                                  /* 194 = getrlimit        */ /* 32 */
#define mmap            obsd_mmap                                       /* 197 = mmap             */ /* 34 */
#define lseek           obsd_lseek                                      /* 199 = lseek            */ /* 35 */
#define __sysctl        obsd___sysctl                                   /* 202 = __sysctl         */ /* 36 */
#define nanosleep       obsd_nanosleep                                  /* 240 = nanosleep        */ /* 38 */
#define poll			obsd_poll										/* 252 = poll             */ /* 39 */
#define fstat           obsd_fstat                                      /* 292 = fstat            */ /* 40 */
#define getrtable       obsd_getrtable                                  /* 311 = getrtable        */ /* 41 */

/* system calls for the enhanced API (only enabled for the test-variant) */
#define fcntl           obsd_fcntl                                      /* 92 = fcntl             */ /* 19 */
#define setrlimit       obsd_setrlimit                                  /* 195 = setrlimit        */ /* 33 */
#define clock_gettime   obsd_clock_gettime                              /* 232 = clock_gettime    */ /* 37 */


/* ^===================== rename the syscall functions =========================================^ */

#endif /* __OBSD_PLATFORM_CAL_RENAME_SYSCALLS_H__ */

