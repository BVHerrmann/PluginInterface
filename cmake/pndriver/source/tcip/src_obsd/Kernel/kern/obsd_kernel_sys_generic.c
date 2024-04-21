/*	$OpenBSD: sys_generic.c,v 1.73 2011/11/06 12:10:04 guenther Exp $	*/
/*	$NetBSD: sys_generic.c,v 1.24 1996/03/29 00:25:32 cgd Exp $	*/

/*
 * Copyright (c) 1996 Theo de Raadt
 * Copyright (c) 1982, 1986, 1989, 1993
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
 *	@(#)sys_generic.c	8.5 (Berkeley) 1/21/94
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
#include <sys/obsd_kernel_filedesc.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_resourcevar.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_signalvar.h>
#include <sys/obsd_kernel_uio.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_stat.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_poll.h>
#ifdef KTRACE
#include <sys/obsd_kernel_ktrace.h>
#endif
#include <sys/obsd_kernel_sched.h>

#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_syscallargs.h>

int selscan(struct proc *, fd_set *, fd_set *, int, int, register_t *);
void pollscan(struct proc *, struct pollfd *, u_int, register_t *);
int pollout(struct pollfd *, struct pollfd *, u_int);

#if       OBSD_POOL_MALLOC_NO_SLEEP != 0
#define OBSD_PNIO_NO_SELECT_MALLOC 1 /* gh2289n: avoid malloc/free to save runtime and avoid waiting for memory        */
                                     /*          ! Attention: allocates relevant amounts of memory in task stack !     */
                                     /*          (depending from value of FD_SETSIZE)                                  */
#endif /* OBSD_POOL_MALLOC_NO_SLEEP */

/*
 * Read system call.
 */
/* ARGSUSED */
int
sys_read(struct proc *p, void *v, register_t *retval)
{
	struct sys_read_args /* {
		syscallarg(int) fd;
		syscallarg(void *) buf;
		syscallarg(size_t) nbyte;
	} */ *uap = v;
	struct iovec iov;
	int fd = SCARG(uap, fd);
	struct file *fp;
	struct filedesc *fdp = p->p_fd;

	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);
	if ((fp->f_flag & FREAD) == 0)
		return (EBADF);

	iov.iov_base = SCARG(uap, buf);
	iov.iov_len = SCARG(uap, nbyte);

	FREF(fp);

	/* dofilereadv() will FRELE the descriptor for us */
	return (dofilereadv(p, fd, fp, &iov, 1, 0, &fp->f_offset, retval));
}

#if 0 /* sado -- not used */
/*
 * Scatter read system call.
 */
int
sys_readv(struct proc *p, void *v, register_t *retval)
{
	struct sys_readv_args /* {
		syscallarg(int) fd;
		syscallarg(const struct iovec *) iovp;
		syscallarg(int) iovcnt;
	} */ *uap = v;
	int fd = SCARG(uap, fd);
	struct file *fp;
	struct filedesc *fdp = p->p_fd;

	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);
	if ((fp->f_flag & FREAD) == 0)
		return (EBADF);

	FREF(fp);

	/* dofilereadv() will FRELE the descriptor for us */
	return (dofilereadv(p, fd, fp, SCARG(uap, iovp), SCARG(uap, iovcnt), 1,
	    &fp->f_offset, retval));
}
#endif

int
dofilereadv(struct proc *p, int fd, struct file *fp, const struct iovec *iovp,
    int iovcnt, int userspace, off_t *offset, register_t *retval)
{
	struct iovec aiov[UIO_SMALLIOV];
	struct uio auio;
	struct iovec *iov;
	struct iovec *needfree = NULL;
	long i, /*cnt,*/ error = 0;
	size_t cnt;   /* gerlach/gh2289n: changed type from long to size_t to avoid compiler warning C4389 & C4267 */
	u_int iovlen;
#ifdef KTRACE
	struct iovec *ktriov = NULL;
#endif

	OBSD_UNUSED_ARG(fd); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* note: can't use iovlen until iovcnt is validated */
	iovlen = iovcnt * sizeof(struct iovec);

	/*
	 * If the iovec array exists in userspace, it needs to be copied in;
	 * otherwise, it can be used directly.
	 */
	if (userspace) {
		if ((u_int)iovcnt > UIO_SMALLIOV) {
			if ((u_int)iovcnt > IOV_MAX) {
				error = EINVAL;
				goto out;
			}
			if ((iov = needfree = malloc(iovlen, M_IOV, M_NOWAIT)) == NULL)
			{
				error = ENOMEM;
				goto out;
			}
		} else if ((u_int)iovcnt > 0) {
			iov = aiov;
			needfree = NULL;
		} else {
			error = EINVAL;
			goto out;
		}
		if (((error = copyin(iovp, iov, iovlen)) != 0))      /* gh2289n: added 0 comparison to avoid PN compiler error */
			goto done;
	} else {
		iov = (struct iovec *)iovp;		/* de-constify */
	}

	auio.uio_iov = iov;
	auio.uio_iovcnt = iovcnt;
	auio.uio_rw = UIO_READ;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_procp = p;
	auio.uio_resid = 0;
	for (i = 0; i < iovcnt; i++) {
		auio.uio_resid += iov->iov_len;
		/*
		 * Reads return ssize_t because -1 is returned on error.
		 * Therefore we must restrict the length to SSIZE_MAX to
		 * avoid garbage return values.  Note that the addition is
		 * guaranteed to not wrap because SSIZE_MAX * 2 < SIZE_MAX.
		 */
		if (iov->iov_len > SSIZE_MAX || auio.uio_resid > SSIZE_MAX) {
			error = EINVAL;
			goto done;
		}
		iov++;
	}
#ifdef KTRACE
	/*
	 * if tracing, save a copy of iovec
	 */
	if (KTRPOINT(p, KTR_GENIO)) {
		ktriov = malloc(iovlen, M_TEMP, M_WAITOK);
		bcopy(auio.uio_iov, ktriov, iovlen);
	}
#endif
	cnt = auio.uio_resid;
	error = (*fp->f_ops->fo_read)(fp, offset, &auio, fp->f_cred);
	if (error)
		if (auio.uio_resid != cnt && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
	cnt -= auio.uio_resid;

	fp->f_rxfer++;
	fp->f_rbytes += cnt;
#ifdef KTRACE
	if (ktriov != NULL) {
		if (error == 0)
			ktrgenio(p, fd, UIO_READ, ktriov, cnt,
			    error);
		free(ktriov, M_TEMP);
	}
#endif
	*retval = (register_t)cnt; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 ( possible loss of data) */
 done:
	if (needfree)
		free(needfree, M_IOV);
 out:
	FRELE(fp);
	return (error);
}

/*
 * Write system call
 */
int
sys_write(struct proc *p, void *v, register_t *retval)
{
	struct sys_write_args /* {
		syscallarg(int) fd;
		syscallarg(const void *) buf;
		syscallarg(size_t) nbyte;
	} */ *uap = v;
	struct iovec iov;
	int fd = SCARG(uap, fd);
	struct file *fp;
	struct filedesc *fdp = p->p_fd;

	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);
	if ((fp->f_flag & FWRITE) == 0)
		return (EBADF);

	iov.iov_base = (void *)SCARG(uap, buf);
	iov.iov_len = SCARG(uap, nbyte);

	FREF(fp);

	/* dofilewritev() will FRELE the descriptor for us */
	return (dofilewritev(p, fd, fp, &iov, 1, 0, &fp->f_offset, retval));
}

#if 0 /* sado -- not needed */
/*
 * Gather write system call
 */
int
sys_writev(struct proc *p, void *v, register_t *retval)
{
	struct sys_writev_args /* {
		syscallarg(int) fd;
		syscallarg(const struct iovec *) iovp;
		syscallarg(int) iovcnt;
	} */ *uap = v;
	int fd = SCARG(uap, fd);
	struct file *fp;
	struct filedesc *fdp = p->p_fd;

	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);
	if ((fp->f_flag & FWRITE) == 0)
		return (EBADF);

	FREF(fp);

	/* dofilewritev() will FRELE the descriptor for us */
	return (dofilewritev(p, fd, fp, SCARG(uap, iovp), SCARG(uap, iovcnt), 1,
	    &fp->f_offset, retval));
}
#endif

int
dofilewritev(struct proc *p, int fd, struct file *fp, const struct iovec *iovp,
    int iovcnt, int userspace, off_t *offset, register_t *retval)
{
	struct iovec aiov[UIO_SMALLIOV];
	struct uio auio;
	struct iovec *iov;
	struct iovec *needfree = NULL;
	long i, /* cnt,*/ error = 0;
	size_t cnt;   /* gerlach/gh2289n: changed type from long to size_t to avoid compiler warning C4389 & C4267 */
	u_int iovlen;
#ifdef KTRACE
	struct iovec *ktriov = NULL;
#endif

	OBSD_UNUSED_ARG(fd); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* note: can't use iovlen until iovcnt is validated */
	iovlen = iovcnt * sizeof(struct iovec);

	/*
	 * If the iovec array exists in userspace, it needs to be copied in;
	 * otherwise, it can be used directly.
	 */
	if (userspace) {
		if ((u_int)iovcnt > UIO_SMALLIOV) {
			if ((u_int)iovcnt > IOV_MAX) {
				error = EINVAL;
				goto out;
			}
			if ((iov = needfree = malloc(iovlen, M_IOV, M_NOWAIT)) == NULL)
			{
				error = ENOMEM;
				goto out;
			}
		} else if ((u_int)iovcnt > 0) {
			iov = aiov;
			needfree = NULL;
		} else {
			error = EINVAL;
			goto out;
		}
		if (((error = copyin(iovp, iov, iovlen)) != 0))      /* gh2289n: added 0 comparison to avoid PN compiler error */
			goto done;
	} else {
		iov = (struct iovec *)iovp;		/* de-constify */
	}

	auio.uio_iov = iov;
	auio.uio_iovcnt = iovcnt;
	auio.uio_rw = UIO_WRITE;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_procp = p;
	auio.uio_resid = 0;
	for (i = 0; i < iovcnt; i++) {
		auio.uio_resid += iov->iov_len;
		/*
		 * Writes return ssize_t because -1 is returned on error.
		 * Therefore we must restrict the length to SSIZE_MAX to
		 * avoid garbage return values.  Note that the addition is
		 * guaranteed to not wrap because SSIZE_MAX * 2 < SIZE_MAX.
		 */
		if (iov->iov_len > SSIZE_MAX || auio.uio_resid > SSIZE_MAX) {
			error = EINVAL;
			goto done;
		}
		iov++;
	}
#ifdef KTRACE
	/*
	 * if tracing, save a copy of iovec
	 */
	if (KTRPOINT(p, KTR_GENIO)) {
		ktriov = malloc(iovlen, M_TEMP, M_WAITOK);
		bcopy(auio.uio_iov, ktriov, iovlen);
	}
#endif
	cnt = auio.uio_resid;
	error = (*fp->f_ops->fo_write)(fp, offset, &auio, fp->f_cred);
	if (error) {
		if (auio.uio_resid != cnt && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
		if (error == EPIPE)
			ptsignal(p, SIGPIPE, STHREAD);
	}
	cnt -= auio.uio_resid;

	fp->f_wxfer++;
	fp->f_wbytes += cnt;
#ifdef KTRACE
	if (ktriov != NULL) {
		if (error == 0)
			ktrgenio(p, fd, UIO_WRITE, ktriov, cnt, error);
		free(ktriov, M_TEMP);
	}
#endif
	*retval = (register_t)cnt;  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 ( possible loss of data) */
 done:
	if (needfree)
		free(needfree, M_IOV);
 out:
	FRELE(fp);
	return (error);
}

/*
 * Ioctl system call
 */
/* ARGSUSED */
int
sys_ioctl(struct proc *p, void *v, register_t *retval)
{
	struct sys_ioctl_args /* {
		syscallarg(int) fd;
		syscallarg(u_long) com;
		syscallarg(void *) data;
	} */ *uap = v;
	struct file *fp;
	struct filedesc *fdp;
	u_long com;
	int error;
	u_int size;
	caddr_t data, memp;
	int tmp;
#define STK_PARAMS	128
	long long stkbuf[STK_PARAMS / sizeof(long long)];

	OBSD_UNUSED_ARG(retval); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	fdp = p->p_fd;
	if ((fp = fd_getfile(fdp, SCARG(uap, fd))) == NULL)
		return (EBADF);

	if ((fp->f_flag & (FREAD | FWRITE)) == 0)
		return (EBADF);

	switch (com = SCARG(uap, com)) {
	case FIONCLEX:
		fdp->fd_ofileflags[SCARG(uap, fd)] &= ~UF_EXCLOSE;
		return (0);
	case FIOCLEX:
		fdp->fd_ofileflags[SCARG(uap, fd)] |= UF_EXCLOSE;
		return (0);
	}

	/*
	 * Interpret high order word to find amount of data to be
	 * copied to/from the user's address space.
	 */
	size = IOCPARM_LEN(com);
	if (size > IOCPARM_MAX)
		return (ENOTTY);
	FREF(fp);
	memp = NULL;
	if (size > sizeof (stkbuf)) {
		if ((memp = (caddr_t)malloc((u_long)size, M_IOCTLOPS, M_WAITOK)) == NULL)
		{
			error = ENOMEM;
			goto out;
		}
		data = memp;
	} else
		data = (caddr_t)stkbuf;
	if (com&IOC_IN) {
		if (size) {
			error = copyin(SCARG(uap, data), data, (u_int)size);
			if (error) {
				goto out;
			}
		} else
			*(caddr_t *)data = SCARG(uap, data);
	} else if ((com&IOC_OUT) && size)
		/*
		 * Zero the buffer so the user always
		 * gets back something deterministic.
		 */
		bzero(data, size);
	else if (com&IOC_VOID)
		*(caddr_t *)data = SCARG(uap, data);

	switch (com) {

	case FIONBIO:
		if ((tmp = *(int *)data) != 0)
			fp->f_flag |= FNONBLOCK;
		else
			fp->f_flag &= ~FNONBLOCK;
		error = (*fp->f_ops->fo_ioctl)(fp, FIONBIO, (caddr_t)&tmp, p);
		break;

	case FIOASYNC:
		if ((tmp = *(int *)data) != 0)
			fp->f_flag |= FASYNC;
		else
			fp->f_flag &= ~FASYNC;
		error = (*fp->f_ops->fo_ioctl)(fp, FIOASYNC, (caddr_t)&tmp, p);
		break;

	case FIOGETOWN:
		if (fp->f_type == DTYPE_SOCKET) {
			error = 0;
			*(int *)data = ((struct socket *)fp->f_data)->so_pgid;
			break;
		}
		error = (*fp->f_ops->fo_ioctl)(fp, TIOCGPGRP, data, p);
		*(int *)data = -*(int *)data;
		break;

	default:
		error = (*fp->f_ops->fo_ioctl)(fp, com, data, p);
		break;
	}
	/*
	 * Copy any data to user, size was
	 * already set and checked above.
	 */
	if (error == 0 && (com&IOC_OUT) && size)
		error = copyout(data, SCARG(uap, data), (u_int)size);
out:
	FRELE(fp);
	if (memp)
		free(memp, M_IOCTLOPS);
	return (error);
}

int	selwait, nselcoll = 0;		/* mh2290: LINT 729   Symbol not explicitly initialized */

/*
 * Select system call.
 */
int
sys_select(struct proc *p, void *v, register_t *retval)
{
	struct sys_select_args /* {
		syscallarg(int) nd;
		syscallarg(fd_set *) in;
		syscallarg(fd_set *) ou;
		syscallarg(fd_set *) ex;
		syscallarg(struct timeval *) tv;
	} */ *uap = v;
#if      !OBSD_PNIO_NO_SELECT_MALLOC
	fd_mask bits[6];
#else  /* OBSD_PNIO_NO_SELECT_MALLOC */
	fd_mask bits[(howmany(FD_SETSIZE, NFDBITS)) * 6];  /* for FD_SETSIZE=1024 -> fd_mask bit[32*6] -> fd_mask bit[192] -> 4*192Byte */
#endif /* OBSD_PNIO_NO_SELECT_MALLOC */
	fd_set *pibits[3], *pobits[3];
	struct timeval atv, rtv, ttv;
	int ncoll, error = 0, timo;

	u_int /*nd,*/ ni;
	int nd;   /* gerlach/gh2289n: changed type from long to size_t to avoid compiler warning C4018 */

	nd = SCARG(uap, nd);
	if (nd > p->p_fd->fd_nfiles) {
		/* forgiving; slightly wrong */
		nd = p->p_fd->fd_nfiles;
	}
	ni = howmany(nd, NFDBITS) * sizeof(fd_mask);

	/* gh2289n: solution for pool sleep (OBSD_POOL_MALLOC_NO_SLEEP).                                    */
	/* Because of pobits is used behind label done in copyout() we need an other solution than jumping  */
	/* to done (because it let's pobits uninitialized).                                                 */
	/* We use a variant that allocates memory on stack instead from heap. So there's no need to wait    */
	/* for memory. This would improve performance too when our underlying memory management is slow ... */
#if      !OBSD_PNIO_NO_SELECT_MALLOC
	if (nd > sizeof(bits[0])) {  /* gh2289n: this comparison is not optimal, it's an error in OBSD      */
	                             /* Code 5.1: should be: ni > sizeof(bits[0])), changed with File       */
	                             /* version version 1.77                                                */
		caddr_t mbits;

		mbits = malloc(ni * 6, M_TEMP, M_WAITOK|M_ZERO);
		pibits[0] = (fd_set *)&mbits[ni * 0];
		pibits[1] = (fd_set *)&mbits[ni * 1];
		pibits[2] = (fd_set *)&mbits[ni * 2];
		pobits[0] = (fd_set *)&mbits[ni * 3];
		pobits[1] = (fd_set *)&mbits[ni * 4];
		pobits[2] = (fd_set *)&mbits[ni * 5];
	} else {
		bzero(bits, sizeof(bits));
		pibits[0] = (fd_set *)&bits[0];
		pibits[1] = (fd_set *)&bits[1];
		pibits[2] = (fd_set *)&bits[2];
		pobits[0] = (fd_set *)&bits[3];
		pobits[1] = (fd_set *)&bits[4];
		pobits[2] = (fd_set *)&bits[5];
	}
#else  /* OBSD_PNIO_NO_SELECT_MALLOC */
	/* e.g. for FD_SETSIZE=1024: ni<=32*4 Byte -> ni <= 128 Byte*/
	{
		int idx = howmany(nd, NFDBITS); /*  e.g. for FD_SETSIZE=1024: idx <= 32 */
		if (idx <= howmany(FD_SETSIZE, NFDBITS)) {
			bzero(bits, sizeof(bits[0])* idx *6);
			pibits[0] = (fd_set *)&bits[idx * 0];
			pibits[1] = (fd_set *)&bits[idx * 1];
			pibits[2] = (fd_set *)&bits[idx * 2];
			pobits[0] = (fd_set *)&bits[idx * 3];
			pobits[1] = (fd_set *)&bits[idx * 4];
			pobits[2] = (fd_set *)&bits[idx * 5];
		}
		else
			return E2BIG;  /* gh2289n: should never happen */
	}
#endif /* OBSD_PNIO_NO_SELECT_MALLOC */

#define	getbits(name, x) \
	if (SCARG(uap, name) && ((error = copyin(SCARG(uap, name), \
	    pibits[x], ni)) != 0))       /* gh2289n: added 0 comparison to avoid PN compiler error */\
		goto done;
	getbits(in, 0);
	getbits(ou, 1);
	getbits(ex, 2);
#undef	getbits

	if (SCARG(uap, tv)) {
		error = copyin(SCARG(uap, tv), &atv, sizeof (atv));
		if (error)
			goto done;
		if (itimerfix(&atv)) {
			error = EINVAL;
			goto done;
		}
		getmicrouptime(&rtv);
		timeradd(&atv, &rtv, &atv);
	} else {
		atv.tv_sec = 0;
		atv.tv_usec = 0;
	}
	timo = 0;

retry:
	ncoll = nselcoll;
	atomic_setbits_int((u_int32_t *)(&p->p_flag), P_SELECT); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	error = selscan(p, pibits[0], pobits[0], nd, ni, retval);
	if (error || *retval)
		goto done;
	if (SCARG(uap, tv)) {
		getmicrouptime(&rtv);
		if (timercmp(&rtv, &atv, >=))
			goto done;
		ttv = atv;
		timersub(&ttv, &rtv, &ttv);
		timo = ttv.tv_sec > 24 * 60 * 60 ?
			24 * 60 * 60 * hz : tvtohz(&ttv);
	}
	if ((p->p_flag & P_SELECT) == 0 || nselcoll != ncoll) {
		goto retry;
	}
	atomic_clearbits_int((u_int32_t *)(&p->p_flag), P_SELECT); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	error = tsleep(&selwait, PSOCK | PCATCH, "select", timo);
	if (error == 0)
		goto retry;
done:
	atomic_clearbits_int((u_int32_t *)(&p->p_flag), P_SELECT); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	/* select is not restarted after signals... */
	if (error == ERESTART)
		error = EINTR;
	if (error == EWOULDBLOCK)
		error = 0;
#define	putbits(name, x) \
	if (SCARG(uap, name) && ((error2 = copyout(pobits[x], \
	    SCARG(uap, name), ni)) != 0))       /* gh2289n: added 0 comparison to avoid PN compiler error */\
		error = error2;
	if (error == 0) {
		int error2;

		putbits(in, 0);
		putbits(ou, 1);
		putbits(ex, 2);
#undef putbits
	}

#if     ! OBSD_PNIO_NO_SELECT_MALLOC        /* gh2289n: free it only if we use malloc, otherwise not necessary               */
	if (pibits[0] != (fd_set *)&bits[0])    /*          comparison is only valid for malloc case (depends from idx otherwise)*/
		free(pibits[0], M_TEMP);
#endif /* OBSD_PNIO_NO_SELECT_MALLOC */
	return (error);
}

int
selscan(struct proc *p, fd_set *ibits, fd_set *obits, int nfd, int ni,
    register_t *retval)
{
	caddr_t cibits = (caddr_t)ibits, cobits = (caddr_t)obits;
	struct filedesc *fdp = p->p_fd;
	int msk, i, j, fd;
	fd_mask bits;
	struct file *fp;
	int n = 0;
	static const int flag[3] = { POLLIN, POLLOUT, POLLPRI };

	for (msk = 0; msk < 3; msk++) {
		fd_set *pibits = (fd_set *)&cibits[msk*ni];
		fd_set *pobits = (fd_set *)&cobits[msk*ni];

		for (i = 0; i < nfd; i += NFDBITS) {
			bits = pibits->fds_bits[i/NFDBITS];
			while (((j = ffs(bits)) != 0) && (fd = i + --j) < nfd) {      /* gh2289n: added 0 comparison to avoid PN compiler error */
				bits &= ~(1 << j);
				if ((fp = fd_getfile(fdp, fd)) == NULL)
					return (EBADF);
				FREF(fp);
				if ((*fp->f_ops->fo_poll)(fp, flag[msk], p)) {
					FD_SET(fd, pobits);
					n++;
				}
				FRELE(fp);
			}
		}
	}
	*retval = n;
	return (0);
}

/*ARGSUSED*/
int
seltrue(dev_t dev, int events, struct proc *p)
{
	OBSD_UNUSED_ARG(dev);    /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	OBSD_UNUSED_ARG(p);      /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	return (events & (POLLIN | POLLOUT | POLLRDNORM | POLLWRNORM));
}

int
selfalse(dev_t dev, int events, struct proc *p)
{
	OBSD_UNUSED_ARG(dev);    /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	OBSD_UNUSED_ARG(events); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	OBSD_UNUSED_ARG(p);      /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	return (0);
}

/*
 * Record a select request.
 */
void
selrecord(struct proc *selector, struct selinfo *sip)
{
	struct proc *p;
	pid_t mypid;

	mypid = selector->p_pid;
	if (sip->si_selpid == mypid)
		return;
	if (sip->si_selpid && ((p = pfind(sip->si_selpid)) != NULL) &&      /* gh2289n: added NULL comparison to avoid PN compiler error */
	    p->p_wchan == (caddr_t)&selwait)
		sip->si_flags |= SI_COLL;
	else
		sip->si_selpid = mypid;
}

/*
 * Do a wakeup when a selectable event occurs.
 */
void
selwakeup(struct selinfo *sip)
{
	OBSD_UNUSED_ARG(sip); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

    wakeup(&selwait);
}

void
pollscan(struct proc *p, struct pollfd *pl, u_int nfd, register_t *retval)
{
	struct filedesc *fdp = p->p_fd;
	struct file *fp;
	u_int i;
	int n = 0;

	for (i = 0; i < nfd; i++, pl++) {
		/* Check the file descriptor. */
		if (pl->fd < 0) {
			pl->revents = 0;
			continue;
		}
		if ((fp = fd_getfile(fdp, pl->fd)) == NULL) {
			pl->revents = POLLNVAL;
			n++;
			continue;
		}
		FREF(fp);
		pl->revents = (short)((*fp->f_ops->fo_poll)(fp, pl->events, p)); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
		FRELE(fp);
		if (pl->revents != 0)
			n++;
	}
	*retval = n;
}

/*
 * Only copyout the revents field.
 */
int
pollout(struct pollfd *pl, struct pollfd *upl, u_int nfds)
{
	int error = 0;
	u_int i = 0;

	while (!error && i++ < nfds) {
		error = copyout(&pl->revents, &upl->revents,
		    sizeof(upl->revents));
		pl++;
		upl++;
	}

	return (error);
}

/*
 * We are using the same mechanism as select only we encode/decode args
 * differently.
 */
int
sys_poll(struct proc *p, void *v, register_t *retval)
{
	struct sys_poll_args /* {
		syscallarg(struct pollfd *) fds;
		syscallarg(u_int) nfds;
		syscallarg(int) timeout;
	} */ *uap = v;
	size_t sz;
	struct pollfd pfds[4], *pl = pfds;
	int msec = SCARG(uap, timeout);
	struct timeval atv, rtv, ttv;
	int timo, ncoll, /*i,*/ error;

	u_int i;   /* gerlach/gh2289n: changed type from long to size_t to avoid compiler warning C4018 */
	extern int nselcoll, selwait;
	u_int nfds = SCARG(uap, nfds);

	/* Standards say no more than MAX_OPEN; this is possibly better. */
	if (nfds > min((int)p->p_rlimit[RLIMIT_NOFILE].rlim_cur, maxfiles))
		return (EINVAL);

	sz = sizeof(struct pollfd) * nfds;

	/* optimize for the default case, of a small nfds value */
	if (sz > sizeof(pfds))
	{
		if ((pl = (struct pollfd *) malloc((unsigned long) sz, M_TEMP, M_NOWAIT)) == NULL)
			return (ENOMEM); /* gh2289n: no goto bad; because pl is NULL and so a NULL will be freed at label bad: */
	}

	if ((error = copyin(SCARG(uap, fds), pl, sz)) != 0)
		goto bad;

	for (i = 0; i < nfds; i++)
		pl[i].revents = 0;

	if (msec != INFTIM) {
		atv.tv_sec = msec / 1000;
		atv.tv_usec = (msec - (atv.tv_sec * 1000)) * 1000;

		if (itimerfix(&atv)) {
			error = EINVAL;
			goto done;
		}
		getmicrouptime(&rtv);
		timeradd(&atv, &rtv, &atv);
	} else {
		atv.tv_sec = 0;
		atv.tv_usec = 0;
	}
	timo = 0;

retry:
	ncoll = nselcoll;
	atomic_setbits_int((u_int32_t *)(&p->p_flag), P_SELECT);  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	pollscan(p, pl, nfds, retval);
	if (*retval)
		goto done;
	if (msec != INFTIM) {
		getmicrouptime(&rtv);
		if (timercmp(&rtv, &atv, >=))
			goto done;
		ttv = atv;
		timersub(&ttv, &rtv, &ttv);
		timo = ttv.tv_sec > 24 * 60 * 60 ?
			24 * 60 * 60 * hz : tvtohz(&ttv);
	}
	if ((p->p_flag & P_SELECT) == 0 || nselcoll != ncoll) {
		goto retry;
	}
	atomic_clearbits_int((u_int32_t *)(&p->p_flag), P_SELECT);  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	error = tsleep(&selwait, PSOCK | PCATCH, "poll", timo);
	if (error == 0)
		goto retry;

done:
	atomic_clearbits_int((u_int32_t *)(&p->p_flag), P_SELECT);  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	/*
	 * NOTE: poll(2) is not restarted after a signal and EWOULDBLOCK is
	 *       ignored (since the whole point is to see what would block).
	 */
	switch (error) {
	case ERESTART:
		error = pollout(pl, SCARG(uap, fds), nfds);
		if (error == 0)
			error = EINTR;
		break;
	case EWOULDBLOCK:
	case 0:
		error = pollout(pl, SCARG(uap, fds), nfds);
		break;
	}
bad:
	if (pl != pfds)
		free(pl, M_TEMP);
	return (error);
}
