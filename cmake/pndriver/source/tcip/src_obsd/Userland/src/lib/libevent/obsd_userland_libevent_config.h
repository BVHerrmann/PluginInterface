/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_libevent_config.h
 *
 * @brief       libevent configuration
 * @details     
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.01
 * @date        created: 07.05.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef    __OBSD_USERLAND_LIBEVENT_CONFIG_H__
#define    __OBSD_USERLAND_LIBEVENT_CONFIG_H__
#define HAVE_SYS_TIME_H
#define HAVE_STRTOLL
#define HAVE_GETTIMEOFDAY
#define HAVE_SYS_SOCKET_H
#define HAVE_UNISTD_H
#define HAVE_FCNTL_H
#define HAVE_STDLIB_H
#define HAVE_FD_MASK
#define HAVE_SYS_SELECT_H
#define HAVE_SETFD
#define HAVE_SELECT
#define HAVE_SIGACTION

/*#define USE_DEBUG */
#undef _MSC_VER
#undef WIN32                   /* for those who can't delete WIN32 in their project configuration */
#define inline 
#endif  /* __OBSD_USERLAND_LIBEVENT_CONFIG_H__ */


