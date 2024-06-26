/*	$OpenBSD: filedesc.h,v 1.20 2008/09/19 12:24:55 art Exp $	*/
/*	$NetBSD: filedesc.h,v 1.14 1996/04/09 20:55:28 cgd Exp $	*/

/*
 * Copyright (c) 1990, 1993
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
 *	@(#)filedesc.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _SYS_FILEDESC_H_    /* gh2289n: added include guard, requested by OBSD_ITGR */
#define _SYS_FILEDESC_H_

#include <sys/obsd_kernel_rwlock.h>
/*
 * This structure is used for the management of descriptors.  It may be
 * shared by multiple processes.
 *
 * A process is initially started out with NDFILE descriptors stored within
 * this structure, selected to be enough for typical applications based on
 * the historical limit of 20 open files (and the usage of descriptors by
 * shells).  If these descriptors are exhausted, a larger descriptor table
 * may be allocated, up to a process' resource limit; the internal arrays
 * are then unused.  The initial expansion is set to NDEXTENT; each time
 * it runs out, it is doubled until the resource limit is reached. NDEXTENT
 * should be selected to be the biggest multiple of OFILESIZE (see below)
 * that will fit in a power-of-two sized piece of memory.
 */
#define NDFILE		20
#define NDEXTENT	50		/* 250 bytes in 256-byte alloc. */
#define NDENTRIES	32		/* 32 fds per entry */
#define NDENTRYMASK	(NDENTRIES - 1)
#define NDENTRYSHIFT	5		/* bits per entry */
#define NDREDUCE(x)	(((x) + NDENTRIES - 1) >> NDENTRYSHIFT)
#define NDHISLOTS(x)	(NDREDUCE(NDREDUCE(x)))
#define NDLOSLOTS(x)	(NDHISLOTS(x) << NDENTRYSHIFT)

struct filedesc {
	struct	file **fd_ofiles;	/* file structures for open files */
	char	*fd_ofileflags;		/* per-process open file flags */
	struct	vnode *fd_cdir;		/* current directory */
	struct	vnode *fd_rdir;		/* root directory */
	int	fd_nfiles;		/* number of open files allocated */
	u_int	*fd_himap;		/* each bit points to 32 fds */
	u_int	*fd_lomap;		/* bitmap of free fds */
	int	fd_lastfile;		/* high-water mark of fd_ofiles */
	int	fd_freefile;		/* approx. next free file */
	u_short	fd_cmask;		/* mask for file creation */
	u_short	fd_refcnt;		/* reference count */
	struct rwlock fd_lock;		/* lock for the file descs */

	int	fd_knlistsize;		/* size of knlist */
	struct	klist *fd_knlist;	/* list of attached knotes */
	u_long	fd_knhashmask;		/* size of knhash */
	struct	klist *fd_knhash;	/* hash table for attached knotes */

	int fd_flags;			/* flags on the file descriptor table */
};

/*
 * Basic allocation of descriptors:
 * one of the above, plus arrays for NDFILE descriptors.
 */
struct filedesc0 {
	struct	filedesc fd_fd;
	/*
	 * These arrays are used when the number of open files is
	 * <= NDFILE, and are then pointed to by the pointers above.
	 */
	struct	file *fd_dfiles[NDFILE];
	char	fd_dfileflags[NDFILE];
	/*
	 * There arrays are used when the number of open files is
	 * <= 1024, and are then pointed to by the pointers above.
	 */
	u_int   fd_dhimap[NDENTRIES >> NDENTRYSHIFT];
	u_int   fd_dlomap[NDENTRIES];
};

/*
 * Per-process open flags.
 */
#define	UF_EXCLOSE 	0x01		/* auto-close on exec */

/*
 * Flags on the file descriptor table.
 */
#define FD_ADVLOCK	0x01		/* May hold a POSIX adv. lock. */

/*
 * Storage required per open file descriptor.
 */
#define OFILESIZE (sizeof(struct file *) + sizeof(char))

#ifdef _KERNEL
/*
 * Kernel global variables and routines.
 */
void	filedesc_init(void);
int	dupfdopen(struct filedesc *fdp, int indx, int dfd, int mode,
	    int error);
int	fdalloc(struct proc *p, int want, int *result);
void	fdexpand(struct proc *);
int	falloc(struct proc *p, struct file **resultfp, int *resultfd);
struct	filedesc *fdinit(struct proc *p);
struct	filedesc *fdshare(struct proc *p);
struct	filedesc *fdcopy(struct proc *p);
void	fdfree(struct proc *p);
int	fdrelease(struct proc *p, int);
void	fdremove(struct filedesc *, int);
void	fdcloseexec(struct proc *);
struct file *fd_getfile(struct filedesc *, int fd);

int	closef(struct file *, struct proc *);
int	getsock(struct filedesc *, int, struct file **);

extern int prepare_userland_stdio(struct proc * p);     /* make stdio for a process working       */
extern void filedesc_deinit(void);                      /* missing deinitialization               */

#define	fdplock(fdp)	rw_enter_write(&(fdp)->fd_lock)
#define	fdpunlock(fdp)	rw_exit_write(&(fdp)->fd_lock)
#endif

#endif /* _SYS_FILEDESC_H_ */
