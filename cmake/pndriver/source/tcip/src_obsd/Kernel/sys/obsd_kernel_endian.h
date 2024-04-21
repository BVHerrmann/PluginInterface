/*	$OpenBSD: endian.h,v 1.19 2011/06/24 22:44:59 deraadt Exp $	*/

/*-
 * Copyright (c) 1997 Niklas Hallqvist.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Generic definitions for little- and big-endian systems.  Other endianesses
 * has to be dealt with in the specific machine/endian.h file for that port.
 *
 * This file is meant to be included from a little- or big-endian port's
 * machine/endian.h after setting _BYTE_ORDER to either 1234 for little endian
 * or 4321 for big..
 */

#ifndef _SYS_ENDIAN_H_
#define _SYS_ENDIAN_H_

#include "obsd_pnio_endianess.h"
#include <sys/obsd_kernel_cdefs.h>

#define _LITTLE_ENDIAN	1234
#define _BIG_ENDIAN	4321
#define _PDP_ENDIAN	3412

#if __BSD_VISIBLE
#define LITTLE_ENDIAN	_LITTLE_ENDIAN
#define BIG_ENDIAN	_BIG_ENDIAN
#define PDP_ENDIAN	_PDP_ENDIAN
#define BYTE_ORDER	_BYTE_ORDER
#endif

/* gh2289n: only the needed generic swap macros */
#define __swap16(x)							\
    (__uint16_t)(((__uint16_t)(x) & 0xffU) << 8 | ((__uint16_t)(x) & 0xff00U) >> 8)

#define __swap32(x)							\
    (__uint32_t)(((__uint32_t)(x) & 0xff) << 24 |			\
    ((__uint32_t)(x) & 0xff00) << 8 | ((__uint32_t)(x) & 0xff0000) >> 8 |\
    ((__uint32_t)(x) & 0xff000000) >> 24)

#define __swap16_multi(v, n) do {						\
	__size_t __swap16_multi_n = (n);				\
	__uint16_t *__swap16_multi_v = (v);				\
									\
	while (__swap16_multi_n) {					\
		*__swap16_multi_v = swap16(*__swap16_multi_v);		\
		__swap16_multi_v++;					\
		__swap16_multi_n--;					\
	}								\
} while (0)

#if __BSD_VISIBLE
#define swap16 __swap16
#define swap32 __swap32
#define swap64 __swap64
#define swap16_multi __swap16_multi

__BEGIN_DECLS
__uint64_t	htobe64(__uint64_t);
__uint32_t	htobe32(__uint32_t);
__uint16_t	htobe16(__uint16_t);
__uint64_t	betoh64(__uint64_t);
__uint32_t	betoh32(__uint32_t);
__uint16_t	betoh16(__uint16_t);

__uint64_t	htole64(__uint64_t);
__uint32_t	htole32(__uint32_t);
__uint16_t	htole16(__uint16_t);
__uint64_t	letoh64(__uint64_t);
__uint32_t	letoh32(__uint32_t);
__uint16_t	letoh16(__uint16_t);
__END_DECLS
#endif /* __BSD_VISIBLE */

#if _BYTE_ORDER == _LITTLE_ENDIAN

/* Can be overridden by machine/endian.h before inclusion of this file.  */
#ifndef _QUAD_HIGHWORD
#define _QUAD_HIGHWORD 1
#endif
#ifndef _QUAD_LOWWORD
#define _QUAD_LOWWORD 0
#endif

#if __BSD_VISIBLE
#define htobe16 __swap16
#define htobe32 __swap32
#define htobe64 __swap64
#define betoh16 __swap16
#define betoh32 __swap32
#define betoh64 __swap64

#define htole16(x) ((__uint16_t)(x))
#define htole32(x) ((__uint32_t)(x))
#define htole64(x) ((__uint64_t)(x))
#define letoh16(x) ((__uint16_t)(x))
#define letoh32(x) ((__uint32_t)(x))
#define letoh64(x) ((__uint64_t)(x))
#endif /* __BSD_VISIBLE */

#define htons(x) __swap16(x)
#define htonl(x) __swap32(x)
#define ntohs(x) __swap16(x)
#define ntohl(x) __swap32(x)

#endif /* _BYTE_ORDER */

#if _BYTE_ORDER == _BIG_ENDIAN

/* Can be overridden by machine/endian.h before inclusion of this file.  */
#ifndef _QUAD_HIGHWORD
#define _QUAD_HIGHWORD 0
#endif
#ifndef _QUAD_LOWWORD
#define _QUAD_LOWWORD 1
#endif

#if __BSD_VISIBLE
#define htole16 __swap16
#define htole32 __swap32
#define htole64 __swap64
#define letoh16 __swap16
#define letoh32 __swap32
#define letoh64 __swap64

#define htobe16(x) ((__uint16_t)(x))
#define htobe32(x) ((__uint32_t)(x))
#define htobe64(x) ((__uint64_t)(x))
#define betoh16(x) ((__uint16_t)(x))
#define betoh32(x) ((__uint32_t)(x))
#define betoh64(x) ((__uint64_t)(x))
#endif /* __BSD_VISIBLE */

#define htons(x) ((__uint16_t)(x))
#define htonl(x) ((__uint32_t)(x))
#define ntohs(x) ((__uint16_t)(x))
#define ntohl(x) ((__uint32_t)(x))

#endif /* _BYTE_ORDER */

#if __BSD_VISIBLE
#define	NTOHL(x) (x) = ntohl((u_int32_t)(x))
#define	NTOHS(x) (x) = ntohs((u_int16_t)(x))
#define	HTONL(x) (x) = htonl((u_int32_t)(x))
#define	HTONS(x) (x) = htons((u_int16_t)(x))
#endif

#endif /* _SYS_ENDIAN_H_ */
