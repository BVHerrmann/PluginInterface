/*	$OpenBSD: uipc_socket2.c,v 1.52 2011/04/04 21:11:22 claudio Exp $	*/
/*	$NetBSD: uipc_socket2.c,v 1.11 1996/02/04 02:17:55 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1988, 1990, 1993
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
 *	@(#)uipc_socket2.c	8.1 (Berkeley) 6/10/93
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_uipc_socket2_act_module) */
#define LTRC_ACT_MODUL_ID 4010 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_uipc_socket2_act_module) */
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
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_buf.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_protosw.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_signalvar.h>
#include <sys/obsd_kernel_event.h>
#include <sys/obsd_kernel_pool.h>
/* OBSD_EVENTS_PATCH_START - patch no. (obsd_include_userland_types) */
#include <obsd_userland_pnio_types_def.h>
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_include_userland_types) */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

/*
 * Primitive routines for operating on sockets and socket buffers
 */

u_long	sb_max = SB_MAX;		/* patchable */

extern struct pool mclpools[];
extern struct pool mbpool;

/*
 * Procedures to manipulate state flags of socket
 * and do appropriate wakeups.  Normal sequence from the
 * active (originating) side is that soisconnecting() is
 * called during processing of connect() call,
 * resulting in an eventual call to soisconnected() if/when the
 * connection is established.  When the connection is torn down
 * soisdisconnecting() is called during processing of disconnect() call,
 * and soisdisconnected() is called when the connection to the peer
 * is totally severed.  The semantics of these routines are such that
 * connectionless protocols can call soisconnected() and soisdisconnected()
 * only, bypassing the in-progress calls when setting up a ``connection''
 * takes no time.
 *
 * From the passive side, a socket is created with
 * two queues of sockets: so_q0 for connections in progress
 * and so_q for connections already made and awaiting user acceptance.
 * As a protocol is preparing incoming connections, it creates a socket
 * structure queued on so_q0 by calling sonewconn().  When the connection
 * is established, soisconnected() is called, and transfers the
 * socket structure to so_q, making it available to accept().
 *
 * If a socket is closed with sockets on either
 * so_q0 or so_q, these sockets are dropped.
 *
 * If higher level protocols are implemented in
 * the kernel, the wakeups done here will sometimes
 * cause software-interrupt process scheduling.
 */

void
soisconnecting(struct socket *so)
{

	so->so_state &= ~(SS_ISCONNECTED|SS_ISDISCONNECTING);
	so->so_state |= SS_ISCONNECTING;
}

void
soisconnected(struct socket *so)
{
	struct socket *head = so->so_head;

	so->so_state &= ~(SS_ISCONNECTING|SS_ISDISCONNECTING|SS_ISCONFIRMING);
	so->so_state |= SS_ISCONNECTED;
	if (head && soqremque(so, 0)) {
		soqinsque(head, so, 1);
		sorwakeup(head);
		wakeup_one(&head->so_timeo);
	} else {
		wakeup(&so->so_timeo);
		sorwakeup(so);
		sowwakeup(so);
/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_uipc_socket2_events_001) */
		obsd_pnio_add_socket_connect_event(so); /* hu: connect makes socket 'writable'  */
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_uipc_socket2_events_001) */
	}
}

void
soisdisconnecting(struct socket *so)
{

	so->so_state &= ~SS_ISCONNECTING;
	so->so_state |= (SS_ISDISCONNECTING|SS_CANTRCVMORE|SS_CANTSENDMORE);
	wakeup(&so->so_timeo);
	sowwakeup(so);
	sorwakeup(so);
}

void
soisdisconnected(struct socket *so)
{

	so->so_state &= ~(SS_ISCONNECTING|SS_ISCONNECTED|SS_ISDISCONNECTING);
	so->so_state |= (SS_CANTRCVMORE|SS_CANTSENDMORE|SS_ISDISCONNECTED);
	wakeup(&so->so_timeo);
	sowwakeup(so);
	sorwakeup(so);
/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_uipc_socket2_events_002) */
	obsd_pnio_add_socket_read_event(so); /* hu: disconnect makes socket 'readable' */
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_uipc_socket2_events_002) */
}

/*
 * When an attempt at a new connection is noted on a socket
 * which accepts connections, sonewconn is called.  If the
 * connection is possible (subject to space constraints, etc.)
 * then we allocate a new structure, properly linked into the
 * data structure of the original socket, and return this.
 * Connstatus may be 0, or SS_ISCONFIRMING, or SS_ISCONNECTED.
 *
 * Must be called at splsoftnet()
 */
struct socket *
sonewconn(struct socket *head, int connstatus)
{
	struct socket *so;
	int soqueue = connstatus ? 1 : 0;

	if (mclpools[0].pr_nout > mclpools[0].pr_hardlimit * 95 / 100)
		return ((struct socket *)0);
	if (head->so_qlen + head->so_q0len > head->so_qlimit * 3)
		return ((struct socket *)0);
	so = pool_get(&socket_pool, PR_NOWAIT|PR_ZERO);
	if (so == NULL)
		return ((struct socket *)0);
	so->so_type = head->so_type;
	so->so_options = head->so_options &~ SO_ACCEPTCONN;
	so->so_linger = head->so_linger;
	so->so_state = head->so_state | SS_NOFDREF;
	so->so_proto = head->so_proto;
	so->so_timeo = head->so_timeo;
	so->so_pgid = head->so_pgid;
	so->so_euid = head->so_euid;
	so->so_ruid = head->so_ruid;
#ifdef OBSD_MULTITHREADING
	so->so_egid = head->so_egid;
	so->so_rgid = head->so_rgid;
	so->so_cpid = head->so_cpid;
#endif
	so->so_siguid = head->so_siguid;
	so->so_sigeuid = head->so_sigeuid;
/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_uipc_socket2_events_003) */
#ifdef BSD_STACKPORT
	so->event_list_index = -1;
	so->callback_fct = NULL;
	so->callback_ctx_ptr = NULL;
#endif
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_uipc_socket2_events_003) */

	/*
	 * Inherit watermarks but those may get clamped in low mem situations.
	 */
	if (soreserve(so, head->so_snd.sb_hiwat, head->so_rcv.sb_hiwat)) {
		pool_put(&socket_pool, so);
		return ((struct socket *)0);
	}
	so->so_snd.sb_wat = head->so_snd.sb_wat;
	so->so_snd.sb_lowat = head->so_snd.sb_lowat;
	so->so_snd.sb_timeo = head->so_snd.sb_timeo;
	so->so_rcv.sb_wat = head->so_rcv.sb_wat;
	so->so_rcv.sb_lowat = head->so_rcv.sb_lowat;
	so->so_rcv.sb_timeo = head->so_rcv.sb_timeo;

	soqinsque(head, so, soqueue);
	if ((*so->so_proto->pr_usrreq)(so, PRU_ATTACH, NULL, NULL, NULL,
	    curproc)) {
		(void) soqremque(so, soqueue);
		pool_put(&socket_pool, so);
		return ((struct socket *)0);
	}
	if (connstatus) {
		sorwakeup(head);
/* OBSD_EVENTS_PATCH_START - patch no. (obsd_kernel_uipc_socket2_events_004) */
		obsd_pnio_add_socket_connect_event(head); /* hu: connect makes socket 'writable'  */
/* OBSD_EVENTS_PATCH_END - patch no. (obsd_kernel_uipc_socket2_events_004) */
		wakeup(&head->so_timeo);
		so->so_state |= connstatus;
	}
	return (so);
}

void
soqinsque(struct socket *head, struct socket *so, int q)
{
	so->so_head = head;
	if (q == 0) {
		head->so_q0len++;
		so->so_onq = &head->so_q0;
	} else {
		head->so_qlen++;
		so->so_onq = &head->so_q;
	}
	TAILQ_INSERT_TAIL(so->so_onq, so, so_qe);
}

int
soqremque(struct socket *so, int q)
{
	struct socket *head;

	head = so->so_head;
	if (q == 0) {
		if (so->so_onq != &head->so_q0)
			return (0);
		head->so_q0len--;
	} else {
		if (so->so_onq != &head->so_q)
			return (0);
		head->so_qlen--;
	}
	TAILQ_REMOVE(so->so_onq, so, so_qe);
	so->so_onq = NULL;
	so->so_head = NULL;
	return (1);
}

/*
 * Socantsendmore indicates that no more data will be sent on the
 * socket; it would normally be applied to a socket when the user
 * informs the system that no more data is to be sent, by the protocol
 * code (in case PRU_SHUTDOWN).  Socantrcvmore indicates that no more data
 * will be received, and will normally be applied to the socket by a
 * protocol when it detects that the peer will send no more data.
 * Data queued for reading in the socket may yet be read.
 */

void
socantsendmore(struct socket *so)
{

	so->so_state |= SS_CANTSENDMORE;
	sowwakeup(so);
}

void
socantrcvmore(struct socket *so)
{

	so->so_state |= SS_CANTRCVMORE;
	sorwakeup(so);
}

/*
 * Wait for data to arrive at/drain from a socket buffer.
 */
int
sbwait(struct sockbuf *sb)
{

	sb->sb_flags |= SB_WAIT;
	return (tsleep(&sb->sb_cc,
	    (sb->sb_flags & SB_NOINTR) ? PSOCK : PSOCK | PCATCH, "netio",
	    sb->sb_timeo));
}

/*
 * Lock a sockbuf already known to be locked;
 * return any error returned from sleep (EINTR).
 */
int
sb_lock(struct sockbuf *sb)
{
	int error;

	while (sb->sb_flags & SB_LOCK) {
		sb->sb_flags |= SB_WANT;
		error = tsleep(&sb->sb_flags,
		    (sb->sb_flags & SB_NOINTR) ?
		    PSOCK : PSOCK|PCATCH, "netlck", 0);
		if (error)
			return (error);
	}
	sb->sb_flags |= SB_LOCK;
	return (0);
}

/*
 * Wakeup processes waiting on a socket buffer.
 * Do asynchronous notification via SIGIO
 * if the socket has the SS_ASYNC flag set.
 */
void
sowakeup(struct socket *so, struct sockbuf *sb)
{
	selwakeup(&sb->sb_sel);
	sb->sb_flags &= ~SB_SEL;
	if (sb->sb_flags & SB_WAIT) {
		sb->sb_flags &= ~SB_WAIT;
		wakeup(&sb->sb_cc);
	}
	if (so->so_state & SS_ASYNC)
		csignal(so->so_pgid, SIGIO, so->so_siguid, so->so_sigeuid);
}

/*
 * Socket buffer (struct sockbuf) utility routines.
 *
 * Each socket contains two socket buffers: one for sending data and
 * one for receiving data.  Each buffer contains a queue of mbufs,
 * information about the number of mbufs and amount of data in the
 * queue, and other fields allowing select() statements and notification
 * on data availability to be implemented.
 *
 * Data stored in a socket buffer is maintained as a list of records.
 * Each record is a list of mbufs chained together with the m_next
 * field.  Records are chained together with the m_nextpkt field. The upper
 * level routine soreceive() expects the following conventions to be
 * observed when placing information in the receive buffer:
 *
 * 1. If the protocol requires each message be preceded by the sender's
 *    name, then a record containing that name must be present before
 *    any associated data (mbuf's must be of type MT_SONAME).
 * 2. If the protocol supports the exchange of ``access rights'' (really
 *    just additional data associated with the message), and there are
 *    ``rights'' to be received, then a record containing this data
 *    should be present (mbuf's must be of type MT_CONTROL).
 * 3. If a name or rights record exists, then it must be followed by
 *    a data record, perhaps of zero length.
 *
 * Before using a new socket structure it is first necessary to reserve
 * buffer space to the socket, by calling sbreserve().  This should commit
 * some of the available buffer space in the system buffer pool for the
 * socket (currently, it does nothing but enforce limits).  The space
 * should be released by calling sbrelease() when the socket is destroyed.
 */

int
soreserve(struct socket *so, u_long sndcc, u_long rcvcc)
{

	if (sbreserve(&so->so_snd, sndcc))
		goto bad;
	if (sbreserve(&so->so_rcv, rcvcc))
		goto bad2;
	so->so_snd.sb_wat = sndcc;
	so->so_rcv.sb_wat = rcvcc;
	if (so->so_rcv.sb_lowat == 0)
		so->so_rcv.sb_lowat = 1;
	if (so->so_snd.sb_lowat == 0)
		so->so_snd.sb_lowat = MCLBYTES;
	if ((u_long)so->so_snd.sb_lowat > so->so_snd.sb_hiwat) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>' : signed/unsigned mismatch */
		so->so_snd.sb_lowat = so->so_snd.sb_hiwat;
	return (0);
bad2:
	sbrelease(&so->so_snd);
bad:
	return (ENOBUFS);
}

/*
 * Allot mbufs to a sockbuf.
 * Attempt to scale mbmax so that mbcnt doesn't become limiting
 * if buffering efficiency is near the normal case.
 */
int
sbreserve(struct sockbuf *sb, u_long cc)
{

	if (cc == 0 || cc > sb_max)
		return (1);
	sb->sb_hiwat = cc;
	sb->sb_mbmax = min(cc * 2, sb_max + (sb_max / MCLBYTES) * MSIZE);
	if ((u_long)sb->sb_lowat > sb->sb_hiwat) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>' : signed/unsigned mismatch */
		sb->sb_lowat = sb->sb_hiwat;
	return (0);
}

/*
 * In low memory situation, do not accept any greater than normal request.
 */
int
sbcheckreserve(u_long cnt, u_long defcnt)
{
	if (cnt > defcnt && sbchecklowmem())
		return (ENOBUFS);
	return (0);
}

int
sbchecklowmem(void)
{
	static int sblowmem;

	if (mclpools[0].pr_nout < mclpools[0].pr_hardlimit * 60 / 100 ||
	    mbpool.pr_nout < mbpool.pr_hardlimit * 60 / 100)
		sblowmem = 0;
	if (mclpools[0].pr_nout > mclpools[0].pr_hardlimit * 80 / 100 ||
	    mbpool.pr_nout > mbpool.pr_hardlimit * 80 / 100)
		sblowmem = 1;
	return (sblowmem);
}

/*
 * Free mbufs held by a socket, and reserved mbuf space.
 */
void
sbrelease(struct sockbuf *sb)
{

	sbflush(sb);
	sb->sb_hiwat = sb->sb_mbmax = 0;
}

/*
 * Routines to add and remove
 * data from an mbuf queue.
 *
 * The routines sbappend() or sbappendrecord() are normally called to
 * append new mbufs to a socket buffer, after checking that adequate
 * space is available, comparing the function sbspace() with the amount
 * of data to be added.  sbappendrecord() differs from sbappend() in
 * that data supplied is treated as the beginning of a new record.
 * To place a sender's address, optional access rights, and data in a
 * socket receive buffer, sbappendaddr() should be used.  To place
 * access rights and data in a socket receive buffer, sbappendrights()
 * should be used.  In either case, the new data begins a new record.
 * Note that unlike sbappend() and sbappendrecord(), these routines check
 * for the caller that there will be enough space to store the data.
 * Each fails if there is not enough space, or if it cannot find mbufs
 * to store additional information in.
 *
 * Reliable protocols may use the socket send buffer to hold data
 * awaiting acknowledgement.  Data is normally copied from a socket
 * send buffer in a protocol with m_copy for output to a peer,
 * and then removing the data from the socket buffer with sbdrop()
 * or sbdroprecord() when the data is acknowledged by the peer.
 */


#define	SBLINKRECORD(sb, m0)						\
do {									\
	if ((sb)->sb_lastrecord != NULL)				\
		(sb)->sb_lastrecord->m_nextpkt = (m0);			\
	else								\
		(sb)->sb_mb = (m0);					\
	(sb)->sb_lastrecord = (m0);					\
} while (/*CONSTCOND*/0)

#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
void sbfill_histogram_update(struct sockbuf *sb)
{
    long int fill = sb->sb_cc;

    if (fill <= 0)  /* check also negatives to be sure */
    {
        sb->histogram.ui0ByteCnt++;         /*    0 Bytes in socket buf */
    }
    else /* >= 1 */
    {
        if (fill <= 2048)                   /* 1...2048 */
        {
            if (fill <= 512)                /* 1...512 */
            {
                if (fill <= 256)            /* 1...256 */
                {
                    if (fill <= 128)        /* 1...128 */
                    {
                        sb->histogram.ui128ByteCnt++;      /*    1...  128 Bytes in socket buf */
                    }
                    else                    /* 129...256 */
                    {
                        sb->histogram.ui256ByteCnt++;       /*  129 ... 256 Bytes */
                    }
                }
                else                        /* 257...512 */
                {
                    sb->histogram.ui512ByteCnt++;       /*  257 ... 512 Bytes */
                }
            }
            else                            /* 513...2048 */
            {
                if (fill <= 1024)           /* 513...1024 */
                    sb->histogram.ui1024ByteCnt++;      /*  513 ...1024 Bytes */
                else                        /* 1025...2048 */
                    sb->histogram.ui2048ByteCnt++;      /* 1025 ...2048 Bytes */
            } /* 513...2048 */
        }
        else /* >  2048 */
        { 
            if (fill <= 8192)               /* 2049...8192 */
            {
                if (fill <= 4096)           /* 2049...4096 */
                    sb->histogram.ui4096ByteCnt++; /* 2049 ...4096 Bytes */
                else                        /* 4097...8192 */
                    sb->histogram.ui8192ByteCnt++; /* 4097 ...8192 Bytes */
            }
            else /* > 8192 */
            {
                if (fill <= 16384)          /* 8193...16384 */
                    sb->histogram.ui16384ByteCnt++; /* 8193 ..16384 Bytes */
                else                        /* > 16384 */
                    sb->histogram.uiOverflowByteCnt++; /* >.16384 Bytes      */
            } /* > 8192 */
        } /* >  2048 */
    }
    return;
}
#endif /* OBSD_SOCKET_BUFFER_FILL_HISTROGRAM */

/*
 * Append mbuf chain m to the last record in the
 * socket buffer sb.  The additional space associated
 * the mbuf chain is recorded in sb.  Empty mbufs are
 * discarded and mbufs are compacted where possible.
 */
void
sbappend(struct sockbuf *sb, struct mbuf *m)
{
	struct mbuf *n;

	if (m == NULL)
		return;

	SBLASTRECORDCHK(sb, "sbappend 1");

	if ((n = sb->sb_lastrecord) != NULL) {
		/*
		 * XXX Would like to simply use sb_mbtail here, but
		 * XXX I need to verify that I won't miss an EOR that
		 * XXX way.
		 */
		do {
			if (n->m_flags & M_EOR) {
				sbappendrecord(sb, m); /* XXXXXX!!!! */
				return;
			}
		} while (n->m_next && ((n = n->m_next) != NULL));      /* gh2289n: added NULL comparison to avoid PN compiler error */
	} else {
		/*
		 * If this is the first record in the socket buffer, it's
		 * also the last record.
		 */
		sb->sb_lastrecord = m;
	}
	sbcompress(sb, m, n);
	SBLASTRECORDCHK(sb, "sbappend 2");
}

/*
 * This version of sbappend() should only be used when the caller
 * absolutely knows that there will never be more than one record
 * in the socket buffer, that is, a stream protocol (such as TCP).
 */
void
sbappendstream(struct sockbuf *sb, struct mbuf *m)
{

	KDASSERT(m->m_nextpkt == NULL);
	KASSERT(sb->sb_mb == sb->sb_lastrecord);

	SBLASTMBUFCHK(sb, __func__);

	sbcompress(sb, m, sb->sb_mbtail);

	sb->sb_lastrecord = sb->sb_mb;
	SBLASTRECORDCHK(sb, __func__);
#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
	sbfill_histogram_update(sb);
#endif /* OBSD_SOCKET_BUFFER_FILL_HISTROGRAM */
}



/*
 * As above, except the mbuf chain
 * begins a new record.
 */
void
sbappendrecord(struct sockbuf *sb, struct mbuf *m0)
{
	struct mbuf *m;

	if (m0 == NULL)
		return;

	/*
	 * Put the first mbuf on the queue.
	 * Note this permits zero length records.
	 */
	sballoc(sb, m0);
	SBLASTRECORDCHK(sb, "sbappendrecord 1");
	SBLINKRECORD(sb, m0);
	m = m0->m_next;
	m0->m_next = NULL;
	if (m && (m0->m_flags & M_EOR)) {
		m0->m_flags &= ~M_EOR;
		m->m_flags |= M_EOR;
	}
	sbcompress(sb, m, m0);
	SBLASTRECORDCHK(sb, "sbappendrecord 2");
}

/*
 * As above except that OOB data
 * is inserted at the beginning of the sockbuf,
 * but after any other OOB data.
 */
void
sbinsertoob(struct sockbuf *sb, struct mbuf *m0)
{
	struct mbuf *m, **mp;

	if (m0 == NULL)
		return;

	SBLASTRECORDCHK(sb, "sbinsertoob 1");

	for (mp = &sb->sb_mb; (m = *mp) != NULL; mp = &((*mp)->m_nextpkt)) {
	    again:
		switch (m->m_type) {

		case MT_OOBDATA:
			continue;		/* WANT next train */

		case MT_CONTROL:
			if ((m = m->m_next) != NULL)
				goto again;	/* inspect THIS train further */
		}
		break;
	}
	/*
	 * Put the first mbuf on the queue.
	 * Note this permits zero length records.
	 */
	sballoc(sb, m0);
	m0->m_nextpkt = *mp;
	if (*mp == NULL) {
		/* m0 is actually the new tail */
		sb->sb_lastrecord = m0;
	}
	*mp = m0;
	m = m0->m_next;
	m0->m_next = NULL;
	if (m && (m0->m_flags & M_EOR)) {
		m0->m_flags &= ~M_EOR;
		m->m_flags |= M_EOR;
	}
	sbcompress(sb, m, m0);
	SBLASTRECORDCHK(sb, "sbinsertoob 2");
}

/*
 * Append address and data, and optionally, control (ancillary) data
 * to the receive queue of a socket.  If present,
 * m0 must include a packet header with total length.
 * Returns 0 if no space in sockbuf or insufficient mbufs.
 */
int
sbappendaddr(struct sockbuf *sb, struct sockaddr *asa, struct mbuf *m0,
    struct mbuf *control)
{
	struct mbuf *m, *n, *nlast;
	int space = asa->sa_len;

	if (m0 && (m0->m_flags & M_PKTHDR) == 0)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_uipc_socket2_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "sbappendaddr");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_uipc_socket2_001) */
		panic("sbappendaddr");
	}
	if (m0)
		space += m0->m_pkthdr.len;
	for (n = control; n; n = n->m_next) {
		space += n->m_len;
		if (n->m_next == NULL)	/* keep pointer to last control buf */
			break;
	}
	if (space > sbspace(sb))
		return (0);
	if (asa->sa_len > MLEN)
		return (0);
	MGET(m, M_DONTWAIT, MT_SONAME);
	if (m == NULL)
		return (0);
	m->m_len = asa->sa_len;
	bcopy(asa, mtod(m, caddr_t), asa->sa_len);
	if (n)
		n->m_next = m0;		/* concatenate data to control */
	else
		control = m0;
	m->m_next = control;

	SBLASTRECORDCHK(sb, "sbappendaddr 1");

	for (n = m; n->m_next != NULL; n = n->m_next)
		sballoc(sb, n);
	sballoc(sb, n);
	nlast = n;
	SBLINKRECORD(sb, m);

	sb->sb_mbtail = nlast;
	SBLASTMBUFCHK(sb, "sbappendaddr");

	SBLASTRECORDCHK(sb, "sbappendaddr 2");
#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
	sbfill_histogram_update(sb);
#endif /* OBSD_SOCKET_BUFFER_FILL_HISTROGRAM */

	return (1);
}

int
sbappendcontrol(struct sockbuf *sb, struct mbuf *m0, struct mbuf *control)
{
	struct mbuf *m, *mlast, *n;
	int space = 0;

	if (control == NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_uipc_socket2_002) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "sbappendcontrol");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_uipc_socket2_002) */
		panic("sbappendcontrol");
	}
	for (m = control; ; m = m->m_next) {
		space += m->m_len;
		if (m->m_next == NULL)
			break;
	}
	n = m;			/* save pointer to last control buffer */
	for (m = m0; m; m = m->m_next)
		space += m->m_len;
	if (space > sbspace(sb))
		return (0);
	n->m_next = m0;			/* concatenate data to control */

	SBLASTRECORDCHK(sb, "sbappendcontrol 1");

	for (m = control; m->m_next != NULL; m = m->m_next)
		sballoc(sb, m);
	sballoc(sb, m);
	mlast = m;
	SBLINKRECORD(sb, control);

	sb->sb_mbtail = mlast;
	SBLASTMBUFCHK(sb, "sbappendcontrol");

	SBLASTRECORDCHK(sb, "sbappendcontrol 2");

	return (1);
}

/*
 * Compress mbuf chain m into the socket
 * buffer sb following mbuf n.  If n
 * is null, the buffer is presumed empty.
 */
void
sbcompress(struct sockbuf *sb, struct mbuf *m, struct mbuf *n)
{
	int eor = 0;
	struct mbuf *o;

	while (m) {
		eor |= m->m_flags & M_EOR;
		if (m->m_len == 0 &&
		    (eor == 0 ||
		    ((((o = m->m_next) != NULL) || ((o = n) != NULL)) &&      /* gh2289n: added NULL comparison to avoid PN compiler error */
		    o->m_type == m->m_type))) {
			if (sb->sb_lastrecord == m)
				sb->sb_lastrecord = m->m_next;
			m = m_free(m);
			continue;
		}
		if (n && (n->m_flags & M_EOR) == 0 &&
		    /* M_TRAILINGSPACE() checks buffer writeability */
		    m->m_len <= MCLBYTES / 4 && /* XXX Don't copy too much */
		    m->m_len <= (u_int)M_TRAILINGSPACE(n) &&  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '<=' : signed/unsigned mismatch */
		    n->m_type == m->m_type) {
			bcopy(mtod(m, caddr_t), mtod(n, caddr_t) + n->m_len,
			    (unsigned)m->m_len);
			n->m_len += m->m_len;
			sb->sb_cc += m->m_len;
			if (m->m_type != MT_CONTROL && m->m_type != MT_SONAME)
				sb->sb_datacc += m->m_len;
			m = m_free(m);
			continue;
		}
		if (n)
			n->m_next = m;
		else
			sb->sb_mb = m;
		sb->sb_mbtail = m;
		sballoc(sb, m);
		n = m;
		m->m_flags &= ~M_EOR;
		m = m->m_next;
		n->m_next = NULL;
	}
	if (eor) {
		if (n)
			n->m_flags |= eor;
		else
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_uipc_socket2_003) */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR,"semi-panic: sbcompress");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_uipc_socket2_003) */
			printf("semi-panic: sbcompress");
		}
	}
	SBLASTMBUFCHK(sb, __func__);
}

/*
 * Free all mbufs in a sockbuf.
 * Check that all resources are reclaimed.
 */
void
sbflush(struct sockbuf *sb)
{

	KASSERT((sb->sb_flags & SB_LOCK) == 0);

	while (sb->sb_mbcnt)
		sbdrop(sb, (int)sb->sb_cc);

	KASSERT(sb->sb_cc == 0);
	KASSERT(sb->sb_datacc == 0);
	KASSERT(sb->sb_mb == NULL);
	KASSERT(sb->sb_mbtail == NULL);
	KASSERT(sb->sb_lastrecord == NULL);
}

/*
 * Drop data from (the front of) a sockbuf.
 */
void
sbdrop(struct sockbuf *sb, int len)
{
	struct mbuf *m, *mn;
	struct mbuf *next;

	next = ((m = sb->sb_mb) != NULL) ? m->m_nextpkt : 0;      /* gh2289n: added NULL comparison to avoid PN compiler error */
	while (len > 0) {
		if (m == NULL) {
			if (next == NULL)
			{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_uipc_socket2_004) */
				TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "sbdrop");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_uipc_socket2_004) */
				panic("sbdrop");
			}
			m = next;
			next = m->m_nextpkt;
			continue;
		}
		if (m->m_len > (u_int)len) { /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>' : signed/unsigned mismatch */
			m->m_len -= len;
			m->m_data += len;
			sb->sb_cc -= len;
			if (m->m_type != MT_CONTROL && m->m_type != MT_SONAME)
				sb->sb_datacc -= len;
			break;
		}
		len -= m->m_len;
		sbfree(sb, m);
		MFREE(m, mn);
		m = mn;
	}
	while (m && m->m_len == 0) {
		sbfree(sb, m);
		MFREE(m, mn);
		m = mn;
	}
	if (m) {
		sb->sb_mb = m;
		m->m_nextpkt = next;
	} else
		sb->sb_mb = next;
	/*
	 * First part is an inline SB_EMPTY_FIXUP().  Second part
	 * makes sure sb_lastrecord is up-to-date if we dropped
	 * part of the last record.
	 */
	m = sb->sb_mb;
	if (m == NULL) {
		sb->sb_mbtail = NULL;
		sb->sb_lastrecord = NULL;
	} else if (m->m_nextpkt == NULL)
		sb->sb_lastrecord = m;
}

/*
 * Drop a record off the front of a sockbuf
 * and move the next record to the front.
 */
void
sbdroprecord(struct sockbuf *sb)
{
	struct mbuf *m, *mn;

	m = sb->sb_mb;
	if (m) {
		sb->sb_mb = m->m_nextpkt;
		do {
			sbfree(sb, m);
			MFREE(m, mn);
		} while ((m = mn) != NULL);
	}
	SB_EMPTY_FIXUP(sb);
}

/*
 * Create a "control" mbuf containing the specified data
 * with the specified type for presentation on a socket buffer.
 */
struct mbuf *
sbcreatecontrol(caddr_t p, int size, int type, int level)
{
	struct cmsghdr *cp;
	struct mbuf *m;

	if (CMSG_SPACE(size) > MCLBYTES) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_uipc_socket2_005) */
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "sbcreatecontrol: message too large %d", size);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_uipc_socket2_005) */
		printf("sbcreatecontrol: message too large %d\n", size);
		return NULL;
	}

	if ((m = m_get(M_DONTWAIT, MT_CONTROL)) == NULL)
		return ((struct mbuf *) NULL);
	if (CMSG_SPACE(size) > MLEN) {
		MCLGET(m, M_DONTWAIT);
		if ((m->m_flags & M_EXT) == 0) {
			m_free(m);
			return NULL;
		}
	}
	cp = mtod(m, struct cmsghdr *);
	bcopy(p, CMSG_DATA(cp), size);
	m->m_len = CMSG_SPACE(size);
	cp->cmsg_len = CMSG_LEN(size);
	cp->cmsg_level = level;
	cp->cmsg_type = type;
	return (m);
}
