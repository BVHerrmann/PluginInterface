/*	$OpenBSD: udp.h,v 1.5 2003/06/02 23:28:15 millert Exp $	*/
/*	$NetBSD: udp.h,v 1.6 1995/04/13 06:37:10 cgd Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)udp.h	8.1 (Berkeley) 6/10/93
 */

#ifndef _NETINET_UDP_H_
#define _NETINET_UDP_H_

#define   OBSD_CAL_PACK_SIZE     1  /* byte packing/alignment*/
#include "obsd_platform_cal_pack_on.h"


#define obsd_udphdr_uhsum_offset 6 /* sado -- Greenhills compiler warning 1545 - address taken of a packed structure member with insufficient alignment*/

/* sado -- IMPORTANT: if the udphdr structure is altered, the define for obsd_udphdr_uhsum_offset has to be adapted */

/*
 * Udp protocol header.
 * Per RFC 768, September, 1981.
 */
PNIO_PACKED_ATTRIBUTE_PRE struct udphdr {
	u_int16_t uh_sport;		/* source port */
	u_int16_t uh_dport;		/* destination port */
	u_int16_t uh_ulen;		/* udp length */
	u_int16_t uh_sum;		/* udp checksum */
} PNIO_PACKED_ATTRIBUTE_POST;

#include "obsd_platform_cal_pack_off.h"

#endif /* _NETINET_UDP_H_ */
