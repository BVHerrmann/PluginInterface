/*	$OpenBSD: sys_socket.c,v 1.14 2009/02/22 07:47:22 otto Exp $	*/
/*	$NetBSD: sys_socket.c,v 1.13 1995/08/12 23:59:09 mycroft Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1990, 1993
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
 *	@(#)sys_socket.c	8.1 (Berkeley) 6/10/93
 */

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
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_protosw.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_poll.h>
#include <sys/obsd_kernel_stat.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_route.h>

struct	fileops socketops = {
	soo_read, soo_write, soo_ioctl, soo_poll, soo_kqfilter,
	soo_stat, soo_close
};

/* ARGSUSED */
int
soo_read(struct file *fp, off_t *poff, struct uio *uio, struct ucred *cred)
{
	OBSD_UNUSED_ARG(poff); 
	OBSD_UNUSED_ARG(cred); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	return (soreceive((struct socket *)fp->f_data, (struct mbuf **)0,
		uio, (struct mbuf **)0, (struct mbuf **)0, (int *)0,
		(socklen_t)0));
}

/* ARGSUSED */
int
soo_write(struct file *fp, off_t *poff, struct uio *uio, struct ucred *cred)
{
	OBSD_UNUSED_ARG(poff); 
	OBSD_UNUSED_ARG(cred); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	return (sosend((struct socket *)fp->f_data, (struct mbuf *)0,
		uio, (struct mbuf *)0, (struct mbuf *)0, 0));
}

int
soo_ioctl(struct file *fp, u_long cmd, caddr_t data, struct proc *p)
{
	struct socket *so = (struct socket *)fp->f_data;

	switch (cmd) {

	case FIONBIO:
		if (*(int *)data)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		return (0);

	case FIOASYNC:
		if (*(int *)data) {
			so->so_state |= SS_ASYNC;
			so->so_rcv.sb_flags |= SB_ASYNC;
			so->so_snd.sb_flags |= SB_ASYNC;
		} else {
			so->so_state &= ~SS_ASYNC;
			so->so_rcv.sb_flags &= ~SB_ASYNC;
			so->so_snd.sb_flags &= ~SB_ASYNC;
		}
		return (0);

	case FIONREAD:
		*(int *)data = so->so_rcv.sb_datacc;
		return (0);

	case SIOCSPGRP:
		so->so_pgid = *(int *)data;
		so->so_siguid = p->p_cred->p_ruid;
		so->so_sigeuid = p->p_ucred->cr_uid;
		return (0);

	case SIOCGPGRP:
		*(int *)data = so->so_pgid;
		return (0);

	case SIOCATMARK:
		*(int *)data = (so->so_state&SS_RCVATMARK) != 0;
		return (0);
	}
	/*
	 * Interface/routing/protocol specific ioctls:
	 * interface and routing ioctls should have a
	 * different entry since a socket's unnecessary
	 */
	if (IOCGROUP(cmd) == 'i')
		return (ifioctl(so, cmd, data, p));
	if (IOCGROUP(cmd) == 'r')
		return (rtioctl(cmd, data, p));
	return ((*so->so_proto->pr_usrreq)(so, PRU_CONTROL, 
	    (struct mbuf *)cmd, (struct mbuf *)data, (struct mbuf *)0, p));
}

int
soo_poll(struct file *fp, int events, struct proc *p)
{
	struct socket *so = (struct socket *)fp->f_data;
	int revents = 0;

	if (events & (POLLIN | POLLRDNORM)) {
		if (soreadable(so))
			revents |= events & (POLLIN | POLLRDNORM);
	}
	if (events & (POLLOUT | POLLWRNORM)) {
		if (sowriteable(so))
			revents |= events & (POLLOUT | POLLWRNORM);
	}
	if (events & (POLLPRI | POLLRDBAND)) {
		if (so->so_oobmark || (so->so_state & SS_RCVATMARK))
			revents |= events & (POLLPRI | POLLRDBAND);
	}
	if (revents == 0) {
		if (events & (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND)) {
			selrecord(p, &so->so_rcv.sb_sel);
			so->so_rcv.sb_flags |= SB_SEL;
		}
		if (events & (POLLOUT | POLLWRNORM)) {
			selrecord(p, &so->so_snd.sb_sel);
			so->so_snd.sb_flags |= SB_SEL;
		}
	}
	return (revents);
}

int
soo_stat(struct file *fp, struct stat *ub, struct proc *p)
{
	struct socket *so = (struct socket *)fp->f_data;

	bzero((caddr_t)ub, sizeof (*ub));
	ub->st_mode = S_IFSOCK;
	if ((so->so_state & SS_CANTRCVMORE) == 0 ||
	    so->so_rcv.sb_cc != 0)
		ub->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
	if ((so->so_state & SS_CANTSENDMORE) == 0)
		ub->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
	ub->st_uid = so->so_euid;
#ifdef OBSD_MULTITHREADING
	ub->st_gid = so->so_egid;
#endif
	(void) ((*so->so_proto->pr_usrreq)(so, PRU_SENSE,
	    (struct mbuf *)ub, (struct mbuf *)0, 
	    (struct mbuf *)0, p));
	return (0);
}

/* ARGSUSED */
int
soo_close(struct file *fp, struct proc *p)
{
	int error = 0;

	OBSD_UNUSED_ARG(p); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	if (fp->f_data)
		error = soclose((struct socket *)fp->f_data);
	fp->f_data = 0;
	return (error);
}
