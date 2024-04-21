/*	$OpenBSD: kern_descrip.c,v 1.88 2011/07/08 21:26:27 matthew Exp $	*/
/*	$NetBSD: kern_descrip.c,v 1.42 1996/03/30 22:24:38 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
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
 *	@(#)kern_descrip.c	8.6 (Berkeley) 4/19/94
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
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_vnode.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_stat.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_fcntl.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_syslog.h>
#include <sys/obsd_kernel_ucred.h>
#include <sys/obsd_kernel_unistd.h>
#include <sys/obsd_kernel_resourcevar.h>
#include <sys/obsd_kernel_conf.h>
#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_syscallargs.h>
#include <sys/obsd_kernel_event.h>
#include <sys/obsd_kernel_pool.h>
#include <sys/obsd_kernel_ktrace.h>

/*
 * Descriptor management.
 */
struct filelist filehead;	/* head of list of open files */
int nfiles;			/* actual number of open files */

static __inline void fd_used(struct filedesc *, int);
static __inline void fd_unused(struct filedesc *, int);
static __inline int find_next_zero(u_int *, int, u_int);
int finishdup(struct proc *, struct file *, int, int, register_t *);
int find_last_set(struct filedesc *, int);

struct pool file_pool;
struct pool fdesc_pool;

void
filedesc_init(void)
{
	pool_init(&file_pool, sizeof(struct file), 0, 0, 0, "filepl",
		&pool_allocator_nointr);
	pool_init(&fdesc_pool, sizeof(struct filedesc0), 0, 0, 0, "fdescpl",
		&pool_allocator_nointr);
	LIST_INIT(&filehead);
}

void filedesc_deinit(void)
{
	/* assume we are called under splsched */

	/* destroy the pools and free it's ressources */
	pool_flush_pages(&fdesc_pool);
	pool_destroy(    &fdesc_pool);

	pool_flush_pages(&file_pool);
	pool_destroy(    &file_pool);
}

static __inline int
find_next_zero (u_int *bitmap, int want, u_int bits)
{
	int i, off, maxoff;
	u_int sub;

	if ((u_int)want > bits) /* gerlach/gh2289n: avoid compiler warning C4018: '>=' : signed/unsigned mismatch */
		return -1;

	off = want >> NDENTRYSHIFT;
	i = want & NDENTRYMASK;
	if (i) {
		sub = bitmap[off] | ((u_int)~0 >> (NDENTRIES - i));
		if (sub != ~0)
			goto found;
		off++;
	}

	maxoff = NDLOSLOTS(bits);
	while (off < maxoff) {
		if ((sub = bitmap[off]) != ~0)
			goto found;
		off++;
	}

	return -1;

 found:
	return (off << NDENTRYSHIFT) + ffs(~sub) - 1;
}

int
find_last_set(struct filedesc *fd, int last)
{
	int off, i;
	struct file **ofiles = fd->fd_ofiles;
	u_int *bitmap = fd->fd_lomap;

	off = (last - 1) >> NDENTRYSHIFT;

	while (off >= 0 && !bitmap[off])
		off--;
	if (off < 0)
		return 0;

	i = ((off + 1) << NDENTRYSHIFT) - 1;
	if (i >= last)
		i = last - 1;

	while (i > 0 && ofiles[i] == NULL)
		i--;
	return i;
}

static __inline void
fd_used(struct filedesc *fdp, int fd)
{
	u_int off = fd >> NDENTRYSHIFT;

	fdp->fd_lomap[off] |= 1 << (fd & NDENTRYMASK);
	if (fdp->fd_lomap[off] == ~0)
		fdp->fd_himap[off >> NDENTRYSHIFT] |= 1 << (off & NDENTRYMASK);

	if (fd > fdp->fd_lastfile)
		fdp->fd_lastfile = fd;
}

static __inline void
fd_unused(struct filedesc *fdp, int fd)
{
	u_int off = fd >> NDENTRYSHIFT;

	if (fd < fdp->fd_freefile)
		fdp->fd_freefile = fd;

	if (fdp->fd_lomap[off] == ~0)
		fdp->fd_himap[off >> NDENTRYSHIFT] &= ~(1 << (off & NDENTRYMASK));
	fdp->fd_lomap[off] &= ~(1 << (fd & NDENTRYMASK));

	if (fd == fdp->fd_lastfile)
		fdp->fd_lastfile = find_last_set(fdp, fd);
}

struct file *
fd_getfile(struct filedesc *fdp, int fd)
{
	struct file *fp;

	if ((u_int)fd >= (u_int)fdp->fd_nfiles || (fp = fdp->fd_ofiles[fd]) == NULL) /* gerlach/gh2289n: avoid compiler warning C4018: '>=' : signed/unsigned mismatch */
		return (NULL);

	if (!FILE_IS_USABLE(fp))
		return (NULL);

	return (fp);
}

/*
 * The file control system call.
 */
/* ARGSUSED */
int
sys_fcntl(struct proc *p, void *v, register_t *retval)
{
	struct sys_fcntl_args /* {
		syscallarg(int) fd;
		syscallarg(int) cmd;
		syscallarg(void *) arg;
	} */ *uap = v;
	int fd = SCARG(uap, fd);
	struct filedesc *fdp = p->p_fd;
	struct file *fp;
	struct vnode *vp;
	int i, tmp, newmin, flg = F_POSIX;
	struct flock fl;
	int error = 0;

restart:
	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);
	FREF(fp);
	switch (SCARG(uap, cmd)) {

	case F_DUPFD:
	case F_DUPFD_CLOEXEC:
		newmin = (long)SCARG(uap, arg);
		if ((u_int)newmin >= p->p_rlimit[RLIMIT_NOFILE].rlim_cur ||
		    (u_int)newmin >= (u_int) maxfiles) {  /* OBSD_ITGR -- omit warning */
			error = EINVAL;
			break;
		}
		fdplock(fdp);
		if ((error = fdalloc(p, newmin, &i)) != 0) {
			if (error == ENOSPC) {
				int o_nfiles = p->p_fd->fd_nfiles;
				fdexpand(p);
				FRELE(fp);
				fdpunlock(fdp);
				if (p->p_fd->fd_nfiles == o_nfiles) /* gh2289n: did fdexpand it's job ?                     */
					return (error);
				goto restart;
			}
		}
		/* finishdup will FRELE for us. */
		if (!error)
			error = finishdup(p, fp, fd, i, retval);
		else
			FRELE(fp);

		if (!error && SCARG(uap, cmd) == F_DUPFD_CLOEXEC)
			fdp->fd_ofileflags[i] |= UF_EXCLOSE;

		fdpunlock(fdp);
		return (error);

	case F_GETFD:
		*retval = fdp->fd_ofileflags[fd] & UF_EXCLOSE ? 1 : 0;
		break;

	case F_SETFD:
		if ((long)SCARG(uap, arg) & 1)
			fdp->fd_ofileflags[fd] |= UF_EXCLOSE;
		else
			fdp->fd_ofileflags[fd] &= ~UF_EXCLOSE;
		break;

	case F_GETFL:
		*retval = OFLAGS(fp->f_flag);
		break;

	case F_SETFL:
		fp->f_flag &= ~FCNTLFLAGS;
		fp->f_flag |= FFLAGS((long)SCARG(uap, arg)) & FCNTLFLAGS;
		tmp = fp->f_flag & FNONBLOCK;
		error = (*fp->f_ops->fo_ioctl)(fp, FIONBIO, (caddr_t)&tmp, p);
		if (error)
			break;
		tmp = fp->f_flag & FASYNC;
		error = (*fp->f_ops->fo_ioctl)(fp, FIOASYNC, (caddr_t)&tmp, p);
		if (!error)
			break;
		fp->f_flag &= ~FNONBLOCK;
		tmp = 0;
		(void) (*fp->f_ops->fo_ioctl)(fp, FIONBIO, (caddr_t)&tmp, p);
		break;

	case F_GETOWN:
		if (fp->f_type == DTYPE_SOCKET) {
			*retval = ((struct socket *)fp->f_data)->so_pgid;
			break;
		}
		error = (*fp->f_ops->fo_ioctl)
			(fp, TIOCGPGRP, (caddr_t)&tmp, p);
		*retval = -tmp;
		break;

	case F_SETOWN:
		if (fp->f_type == DTYPE_SOCKET) {
			struct socket *so = (struct socket *)fp->f_data;

			so->so_pgid = (long)SCARG(uap, arg);
			so->so_siguid = p->p_cred->p_ruid;
			so->so_sigeuid = p->p_ucred->cr_uid;
			break;
		}
		if ((long)SCARG(uap, arg) <= 0) {
			SCARG(uap, arg) = (void *)(-(long)SCARG(uap, arg));
		} else {
			error = ESRCH;
			break;
		}
		error = ((*fp->f_ops->fo_ioctl)
			(fp, TIOCSPGRP, (caddr_t)&SCARG(uap, arg), p));
		break;

	case F_SETLKW:
		flg |= F_WAIT;
		/* FALLTHROUGH */

	case F_SETLK:
		if (fp->f_type != DTYPE_VNODE) {
			error = EBADF;
			break;
		}
		vp = (struct vnode *)fp->f_data;
		/* Copy in the lock structure */
		error = copyin((caddr_t)SCARG(uap, arg), (caddr_t)&fl,
		    sizeof (fl));
		if (error)
			break;
		if (fl.l_whence == SEEK_CUR) {
			if (fl.l_start == 0 && fl.l_len < 0) {
				/* lockf(3) compliance hack */
				fl.l_len = -fl.l_len;
				fl.l_start = fp->f_offset - fl.l_len;
			} else
				fl.l_start += fp->f_offset;
		}
		switch (fl.l_type) {

		case F_RDLCK:
			if ((fp->f_flag & FREAD) == 0) {
				error = EBADF;
				goto out;
			}
			atomic_setbits_int((u_int32_t*)&fdp->fd_flags, FD_ADVLOCK); /* OBSD_ITGR: added explicit type cast to avoid compiler warning C4133 */
			error = VOP_ADVLOCK(vp, fdp, F_SETLK, &fl, flg);
			break;

		case F_WRLCK:
			if ((fp->f_flag & FWRITE) == 0) {
				error = EBADF;
				goto out;
			}
			atomic_setbits_int((u_int32_t*)&fdp->fd_flags, FD_ADVLOCK); /* OBSD_ITGR: added explicit type cast to avoid compiler warning C4133 */
			error = VOP_ADVLOCK(vp, fdp, F_SETLK, &fl, flg);
			break;

		case F_UNLCK:
			error = VOP_ADVLOCK(vp, fdp, F_UNLCK, &fl, F_POSIX);
			goto out;

		default:
			error = EINVAL;
			goto out;
		}

		if (fp != fd_getfile(fdp, fd)) {
			/*
			 * We have lost the race with close() or dup2();
			 * unlock, pretend that we've won the race and that
			 * lock had been removed by close()
			 */
			fl.l_whence = SEEK_SET;
			fl.l_start = 0;
			fl.l_len = 0;
			VOP_ADVLOCK(vp, fdp, F_UNLCK, &fl, F_POSIX);
			fl.l_type = F_UNLCK;
		}
		goto out;


	case F_GETLK:
		if (fp->f_type != DTYPE_VNODE) {
			error = EBADF;
			break;
		}
		vp = (struct vnode *)fp->f_data;
		/* Copy in the lock structure */
		error = copyin((caddr_t)SCARG(uap, arg), (caddr_t)&fl,
		    sizeof (fl));
		if (error)
			break;
		if (fl.l_whence == SEEK_CUR) {
			if (fl.l_start == 0 && fl.l_len < 0) {
				/* lockf(3) compliance hack */
				fl.l_len = -fl.l_len;
				fl.l_start = fp->f_offset - fl.l_len;
			} else
				fl.l_start += fp->f_offset;
		}
		if (fl.l_type != F_RDLCK &&
		    fl.l_type != F_WRLCK &&
		    fl.l_type != F_UNLCK &&
		    fl.l_type != 0) {
			error = EINVAL;
			break;
		}
		error = VOP_ADVLOCK(vp, fdp, F_GETLK, &fl, F_POSIX);
		if (error)
			break;
		error = (copyout((caddr_t)&fl, (caddr_t)SCARG(uap, arg),
		    sizeof (fl)));
		break;

	default:
		error = EINVAL;
		break;
	}
out:
	FRELE(fp);
	return (error);	
}

/*
 * Common code for dup, dup2, and fcntl(F_DUPFD).
 */
int
finishdup(struct proc *p, struct file *fp, int old, int new, register_t *retval)
{
	struct file *oldfp;
	struct filedesc *fdp = p->p_fd;

	if (fp->f_count == LONG_MAX-2) {
		FRELE(fp);
		return (EDEADLK);
	}

	/*
	 * Don't fd_getfile here. We want to closef LARVAL files and
	 * closef can deal with that.
	 */
	oldfp = fdp->fd_ofiles[new];
	if (oldfp != NULL)
		FREF(oldfp);

	fdp->fd_ofiles[new] = fp;
	fdp->fd_ofileflags[new] = fdp->fd_ofileflags[old] & ~UF_EXCLOSE;
	fp->f_count++;
	FRELE(fp);
	if (oldfp == NULL)
		fd_used(fdp, new);
	*retval = new;

	if (oldfp != NULL) {
		if (new < fdp->fd_knlistsize)
			knote_fdclose(p, new);
		closef(oldfp, p);
	}

	return (0);
}

void
fdremove(struct filedesc *fdp, int fd)
{
	fdp->fd_ofiles[fd] = NULL;
	fd_unused(fdp, fd);
}

int
fdrelease(struct proc *p, int fd)
{
	struct filedesc *fdp = p->p_fd;
	struct file **fpp, *fp;

	/*
	 * Don't fd_getfile here. We want to closef LARVAL files and closef
	 * can deal with that.
	 */
	fpp = &fdp->fd_ofiles[fd];
	fp = *fpp;
	if (fp == NULL)
		return (EBADF);
	FREF(fp);
	*fpp = NULL;
	fdp->fd_ofileflags[fd] = 0;
	fd_unused(fdp, fd);
	if (fd < fdp->fd_knlistsize)
		knote_fdclose(p, fd);
	return (closef(fp, p));
}

/*
 * Close a file descriptor.
 */
/* ARGSUSED */
int
sys_close(struct proc *p, void *v, register_t *retval)
{
	struct sys_close_args /* {
		syscallarg(int) fd;
	} */ *uap = v;
	int fd = SCARG(uap, fd), error;
	struct filedesc *fdp = p->p_fd;

	OBSD_UNUSED_ARG(retval); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	if (fd_getfile(fdp, fd) == NULL)
		return (EBADF);
	fdplock(fdp);
	error = fdrelease(p, fd);
	fdpunlock(fdp);

	return (error);
}

/*
 * Return status information about a file descriptor.
 */
/* ARGSUSED */
int
sys_fstat(struct proc *p, void *v, register_t *retval)
{
	struct sys_fstat_args /* {
		syscallarg(int) fd;
		syscallarg(struct stat *) sb;
	} */ *uap = v;
	int fd = SCARG(uap, fd);
	struct filedesc *fdp = p->p_fd;
	struct file *fp;
	struct stat ub;
	int error;

	OBSD_UNUSED_ARG(retval); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);
	FREF(fp);
	error = (*fp->f_ops->fo_stat)(fp, &ub, p);
	FRELE(fp);
	if (error == 0) {
		/* 
		 * Don't let non-root see generation numbers
		 * (for NFS security)
		 */
		if (suser(p, 0))
			ub.st_gen = 0;
		error = copyout((caddr_t)&ub, (caddr_t)SCARG(uap, sb),
		    sizeof (ub));
	}
#ifdef KTRACE
	if (error == 0 && KTRPOINT(p, KTR_STRUCT))
		ktrstat(p, &ub);
#endif
	return (error);
}

/*
 * Allocate a file descriptor for the process.
 */
int
fdalloc(struct proc *p, int want, int *result)
{
	struct filedesc *fdp = p->p_fd;
	int lim, last, i;
	int off; /* sado -- signed/unsigned mismatch */
	int new; /* sado -- Greenhills compiler warning 68 - integer conversion resulted in a change of sign */

	/*
	 * Search for a free descriptor starting at the higher
	 * of want or fd_freefile.  If that fails, consider
	 * expanding the ofile array.
	 */
restart:
	lim = min((int)p->p_rlimit[RLIMIT_NOFILE].rlim_cur, maxfiles);
	last = min(fdp->fd_nfiles, lim);
	if ((i = want) < fdp->fd_freefile)
		i = fdp->fd_freefile;
	off = i >> NDENTRYSHIFT;
	new = find_next_zero(fdp->fd_himap, off,
	    (last + NDENTRIES - 1) >> NDENTRYSHIFT);
	if (new != -1) {
		i = find_next_zero(&fdp->fd_lomap[new], 
				   new > (int)off ? 0 : i & NDENTRYMASK,
				   NDENTRIES);
		if (i == -1) {
			/*
			 * Free file descriptor in this block was
			 * below want, try again with higher want.
			 */
			want = (new + 1) << NDENTRYSHIFT;
			goto restart;
		}
		i += (new << NDENTRYSHIFT);
		if (i < last) {
			fd_used(fdp, i);
			if (want <= fdp->fd_freefile)
				fdp->fd_freefile = i;
			*result = i;
			return (0);
		}
	}
	if (fdp->fd_nfiles >= lim)
		return (EMFILE);

	return (ENOSPC);
}

void
fdexpand(struct proc *p)
{
	struct filedesc *fdp = p->p_fd;
	int l_nfiles, i; /* sado -- renamed nfiles in l_nfiles due to compiler warning "global declaration hiding" */
	struct file **newofile;
	char *newofileflags;
	u_int *newhimap, *newlomap;

	/*
	 * No space in current array.
	 */
	if (fdp->fd_nfiles < NDEXTENT)
		l_nfiles = NDEXTENT;
	else
		l_nfiles = 2 * fdp->fd_nfiles;

	if ((newofile = malloc(l_nfiles * OFILESIZE, M_FILEDESC, M_NOWAIT)) == NULL)
		return;                          /* gh2289n: let fdp->fd_nfiles unchanged                           */
	newofileflags = (char *) &newofile[l_nfiles];

	/*
	 * Copy the existing ofile and ofileflags arrays
	 * and zero the new portion of each array.
	 */
	bcopy(fdp->fd_ofiles, newofile,
		(i = sizeof(struct file *) * fdp->fd_nfiles));
	bzero((char *)newofile + i, l_nfiles * sizeof(struct file *) - i);
	bcopy(fdp->fd_ofileflags, newofileflags,
		(i = sizeof(char) * fdp->fd_nfiles));
	bzero(newofileflags + i, l_nfiles * sizeof(char) - i);

	if (NDHISLOTS(l_nfiles) > NDHISLOTS(fdp->fd_nfiles)) {
		if ((newhimap = malloc(NDHISLOTS(l_nfiles) * sizeof(u_int), M_FILEDESC, M_NOWAIT)) == NULL)
		{
			free(newofile, M_FILEDESC);
			return;
		}
		if ((newlomap = malloc(NDLOSLOTS(l_nfiles) * sizeof(u_int), M_FILEDESC, M_NOWAIT)) == NULL)
		{
			free(newofile, M_FILEDESC);
			free(newhimap, M_FILEDESC);
			return;
		}

		bcopy(fdp->fd_himap, newhimap,
		    (i = NDHISLOTS(fdp->fd_nfiles) * sizeof(u_int)));
		bzero((char *)newhimap + i,
		    NDHISLOTS(l_nfiles) * sizeof(u_int) - i);

		bcopy(fdp->fd_lomap, newlomap,
		    (i = NDLOSLOTS(fdp->fd_nfiles) * sizeof(u_int)));
		bzero((char *)newlomap + i,
		    NDLOSLOTS(l_nfiles) * sizeof(u_int) - i);

		if (NDHISLOTS(fdp->fd_nfiles) > NDHISLOTS(NDFILE)) {
			free(fdp->fd_himap, M_FILEDESC);
			free(fdp->fd_lomap, M_FILEDESC);
		}
		fdp->fd_himap = newhimap;
		fdp->fd_lomap = newlomap;
	}
	if (fdp->fd_nfiles > NDFILE)
		free(fdp->fd_ofiles, M_FILEDESC);
	fdp->fd_ofiles = newofile;
	fdp->fd_ofileflags = newofileflags;
	fdp->fd_nfiles = l_nfiles;	
}

/*
 * Create a new open file structure and allocate
 * a file descriptor for the process that refers to it.
 */
int
falloc(struct proc *p, struct file **resultfp, int *resultfd)
{
	struct file *fp, *fq;
	int error, i;

restart:
	if ((error = fdalloc(p, 0, &i)) != 0) {
		if (error == ENOSPC) {
			int o_nfiles = p->p_fd->fd_nfiles;
			fdexpand(p);
			if (p->p_fd->fd_nfiles > o_nfiles) /* gh2289n: did fdexpand it's job ?                          */
			goto restart;
		}
		return (error);
	}
	if (nfiles >= maxfiles) {
		fd_unused(p->p_fd, i);
		tablefull("file");
		return (ENFILE);
	}
	/*
	 * Allocate a new file descriptor.
	 * If the process has file descriptor zero open, add to the list
	 * of open files at that point, otherwise put it at the front of
	 * the list of open files.
	 */
	nfiles++;
	if ((fp = pool_get(&file_pool, PR_NOWAIT|PR_ZERO)) == NULL)
		return (ENOMEM);
	fp->f_iflags = FIF_LARVAL;
	if ((fq = p->p_fd->fd_ofiles[0]) != NULL) {
		LIST_INSERT_AFTER(fq, fp, f_list);
	} else {
		LIST_INSERT_HEAD(&filehead, fp, f_list);
	}
	p->p_fd->fd_ofiles[i] = fp;
	fp->f_count = 1;
	fp->f_cred = p->p_ucred;
	crhold(fp->f_cred);
	if (resultfp)
		*resultfp = fp;
	if (resultfd)
		*resultfd = i;
	FREF(fp);
	return (0);
}

/*
 * Build a new filedesc structure.
 */
struct filedesc *
fdinit(struct proc *p)
{
	struct filedesc0 *newfdp;
	extern int cmask;

	newfdp = pool_get(&fdesc_pool, PR_NOWAIT|PR_ZERO);
	if (newfdp == NULL)
		return (struct filedesc*)NULL;      /* gh2289n: !!! NOTE: all callers of fdinit() have to handle this case.  */
		                                    /*              Normally this is not the case !!!                        */
	if (p != NULL) {
		struct filedesc *fdp = p->p_fd;

		newfdp->fd_fd.fd_cdir = fdp->fd_cdir;
		vref(newfdp->fd_fd.fd_cdir);
		newfdp->fd_fd.fd_rdir = fdp->fd_rdir;
		if (newfdp->fd_fd.fd_rdir)
			vref(newfdp->fd_fd.fd_rdir);
	}
	rw_init(&newfdp->fd_fd.fd_lock, "fdlock");

	/* Create the file descriptor table. */
	newfdp->fd_fd.fd_refcnt = 1;
	newfdp->fd_fd.fd_cmask = (u_short) cmask; /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
	newfdp->fd_fd.fd_ofiles = newfdp->fd_dfiles;
	newfdp->fd_fd.fd_ofileflags = newfdp->fd_dfileflags;
	newfdp->fd_fd.fd_nfiles = NDFILE;
	newfdp->fd_fd.fd_himap = newfdp->fd_dhimap;
	newfdp->fd_fd.fd_lomap = newfdp->fd_dlomap;
	newfdp->fd_fd.fd_knlistsize = -1;

	newfdp->fd_fd.fd_freefile = 0;
	newfdp->fd_fd.fd_lastfile = 0;

	return (&newfdp->fd_fd);
}

/*
 * Share a filedesc structure.
 */
struct filedesc *
fdshare(struct proc *p)
{
	p->p_fd->fd_refcnt++;
	return (p->p_fd);
}

/*
 * Copy a filedesc structure.
 */
struct filedesc *
fdcopy(struct proc *p)
{
	struct filedesc *newfdp, *fdp = p->p_fd;
	struct file **fpp;
	int i;

	newfdp = pool_get(&fdesc_pool, PR_NOWAIT);
	if (newfdp == NULL)
		return (struct filedesc*)NULL;      /* gh2289n: !!! NOTE: all callers of fdcopy() have to handle this case.  */
		                                    /*              Normally this is not the case !!!                        */
	bcopy(fdp, newfdp, sizeof(struct filedesc));
	if (newfdp->fd_cdir)
		vref(newfdp->fd_cdir);
	if (newfdp->fd_rdir)
		vref(newfdp->fd_rdir);
	newfdp->fd_refcnt = 1;

	/*
	 * If the number of open files fits in the internal arrays
	 * of the open file structure, use them, otherwise allocate
	 * additional memory for the number of descriptors currently
	 * in use.
	 */
	if (newfdp->fd_lastfile < NDFILE) {
		newfdp->fd_ofiles = ((struct filedesc0 *) newfdp)->fd_dfiles;
		newfdp->fd_ofileflags =
		    ((struct filedesc0 *) newfdp)->fd_dfileflags;
		i = NDFILE;
	} else {
		/*
		 * Compute the smallest multiple of NDEXTENT needed
		 * for the file descriptors currently in use,
		 * allowing the table to shrink.
		 */
		i = newfdp->fd_nfiles;
		while (i >= 2 * NDEXTENT && i > newfdp->fd_lastfile * 2)
			i /= 2;
		newfdp->fd_ofiles = malloc(i * OFILESIZE, M_FILEDESC, M_NOWAIT);
		if (newfdp->fd_ofiles == NULL)
		{   /* need to rollback relevant former changes (e.g. alloc's) */
			pool_put(&fdesc_pool, newfdp);
			return (struct filedesc*)NULL;      /* gh2289n: !!! NOTE: all callers of fdcopy() have to handle this case.  */
			                                    /*              Normally this is not the case !!!                        */
		}
		newfdp->fd_ofileflags = (char *) &newfdp->fd_ofiles[i];
	}
	if (NDHISLOTS(i) <= NDHISLOTS(NDFILE)) {
		newfdp->fd_himap =
			((struct filedesc0 *) newfdp)->fd_dhimap;
		newfdp->fd_lomap =
			((struct filedesc0 *) newfdp)->fd_dlomap;
	} else {
		newfdp->fd_himap = malloc(NDHISLOTS(i) * sizeof(u_int), M_FILEDESC, M_NOWAIT);
		if (newfdp->fd_himap == NULL)
		{   /* need to rollback relevant former changes (e.g. alloc's) */
			if (newfdp->fd_nfiles >= NDFILE)
				free(newfdp->fd_ofiles, M_FILEDESC);
			pool_put(&fdesc_pool, newfdp);
			return (struct filedesc*)NULL;      /* gh2289n: !!! NOTE: all callers of fdcopy() have to handle this case.  */
			                                    /*              Normally this is not the case !!!                        */
		}
		newfdp->fd_lomap = malloc(NDLOSLOTS(i) * sizeof(u_int), M_FILEDESC, M_NOWAIT);
		if (newfdp->fd_lomap == NULL)
		{   /* need to rollback relevant former changes (e.g. alloc's) */
			if (newfdp->fd_nfiles >= NDFILE)
				free(newfdp->fd_ofiles, M_FILEDESC);
			free(newfdp->fd_himap, M_FILEDESC);
			pool_put(&fdesc_pool, newfdp);
			return (struct filedesc*)NULL;      /* gh2289n: !!! NOTE: all callers of fdcopy() have to handle this case.  */
			                                    /*              Normally this is not the case !!!                        */
		}
	}
	newfdp->fd_nfiles = i;
	bcopy(fdp->fd_ofiles, newfdp->fd_ofiles, i * sizeof(struct file **));
	bcopy(fdp->fd_ofileflags, newfdp->fd_ofileflags, i * sizeof(char));
	bcopy(fdp->fd_himap, newfdp->fd_himap, NDHISLOTS(i) * sizeof(u_int));
	bcopy(fdp->fd_lomap, newfdp->fd_lomap, NDLOSLOTS(i) * sizeof(u_int));

	/*
	 * kq descriptors cannot be copied.
	 */
	if (newfdp->fd_knlistsize != -1) {
		fpp = newfdp->fd_ofiles;
		for (i = 0; i <= newfdp->fd_lastfile; i++, fpp++)
			if (*fpp != NULL && (*fpp)->f_type == DTYPE_KQUEUE)
				fdremove(newfdp, i);
		newfdp->fd_knlist = NULL;
		newfdp->fd_knlistsize = -1;
		newfdp->fd_knhash = NULL;
		newfdp->fd_knhashmask = 0;
	}

	fpp = newfdp->fd_ofiles;
	for (i = 0; i <= newfdp->fd_lastfile; i++, fpp++)
		if (*fpp != NULL) {
			/*
			 * XXX Gruesome hack. If count gets too high, fail
			 * to copy an fd, since fdcopy()'s callers do not
			 * permit it to indicate failure yet.
			 */
			if ((*fpp)->f_count == LONG_MAX-2)
				fdremove(newfdp, i);
			else
				(*fpp)->f_count++;
		}
	return (newfdp);
}

/*
 * Release a filedesc structure.
 */
void
fdfree(struct proc *p)
{
	struct filedesc *fdp = p->p_fd;
	struct file **fpp, *fp;
	int i;

	if (--fdp->fd_refcnt > 0)
		return;
	fpp = fdp->fd_ofiles;
	for (i = fdp->fd_lastfile; i >= 0; i--, fpp++) {
		fp = *fpp;
		if (fp != NULL) {
			FREF(fp);
			*fpp = NULL;
			(void) closef(fp, p);
		}
	}
	p->p_fd = NULL;
	if (fdp->fd_nfiles > NDFILE)
		free(fdp->fd_ofiles, M_FILEDESC);
	if (NDHISLOTS(fdp->fd_nfiles) > NDHISLOTS(NDFILE)) {
		free(fdp->fd_himap, M_FILEDESC);
		free(fdp->fd_lomap, M_FILEDESC);
	}
	if (fdp->fd_cdir)
		vrele(fdp->fd_cdir);
	if (fdp->fd_rdir)
		vrele(fdp->fd_rdir);
	if (fdp->fd_knlist)
		free(fdp->fd_knlist, M_TEMP);
	if (fdp->fd_knhash)
		free(fdp->fd_knhash, M_TEMP);
	pool_put(&fdesc_pool, fdp);
}


/*
 * Internal form of close.
 * Decrement reference count on file structure.
 * Note: p may be NULL when closing a file
 * that was being passed in a message.
 *
 * The fp must have its usecount bumped and will be FRELEd here.
 */
int
closef(struct file *fp, struct proc *p)
{
	struct filedesc *fdp;
	int references_left;
	int error;

	if (fp == NULL)
		return (0);

	/*
	 * Some files passed to this function could be accessed
	 * without a FILE_IS_USABLE check (and in some cases it's perfectly
	 * legal), we must beware of files where someone already won the
	 * race to FIF_WANTCLOSE.
	 */
	if ((fp->f_iflags & FIF_WANTCLOSE) != 0 ||
	    --fp->f_count > 0) {
		references_left = 1;
	} else {
		references_left = 0;

		/* Wait for the last usecount to drain. */
		fp->f_iflags |= FIF_WANTCLOSE;
		while (fp->f_usecount > 1)
			tsleep(&fp->f_usecount, PRIBIO, "closef", 0);
	}

	/*
	 * POSIX record locking dictates that any close releases ALL
	 * locks owned by this process.  This is handled by setting
	 * a flag in the unlock to free ONLY locks obeying POSIX
	 * semantics, and not to free BSD-style file locks.
	 * If the descriptor was in a message, POSIX-style locks
	 * aren't passed with the descriptor.
	 */
	if (p && ((fdp = p->p_fd) != NULL) &&
	    (fdp->fd_flags & FD_ADVLOCK) &&
	    fp->f_type == DTYPE_VNODE) {
		struct vnode *vp = fp->f_data;
		struct flock lf;

		lf.l_whence = SEEK_SET;
		lf.l_start = 0;
		lf.l_len = 0;
		lf.l_type = F_UNLCK;
		(void) VOP_ADVLOCK(vp, fdp, F_UNLCK, &lf, F_POSIX);
	}

	if (references_left) {
		FRELE(fp);
		return (0);
	}

	if (fp->f_ops)
		error = (*fp->f_ops->fo_close)(fp, p);
	else
		error = 0;

	/* Free fp */
	LIST_REMOVE(fp, f_list);
	crfree(fp->f_cred);
	nfiles--;
	pool_put(&file_pool, fp);

	return (error);
}
