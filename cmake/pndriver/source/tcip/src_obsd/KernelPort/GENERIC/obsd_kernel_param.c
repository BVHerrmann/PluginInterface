/*	$OpenBSD: param.c,v 1.33 2011/08/08 19:34:25 deraadt Exp $	*/
/*	$NetBSD: param.c,v 1.16 1996/03/12 03:08:40 mrg Exp $	*/

/*
 * Copyright (c) 1980, 1986, 1989 Regents of the University of California.
 * All rights reserved.
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
 *	@(#)param.c	7.20 (Berkeley) 6/27/91
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
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_vnode.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_timeout.h>
#include <sys/obsd_kernel_mbuf.h>
#include <obsd_platform_hal.h>
#define MAXUSERS 1                /* normally in ..\GENERIC\Makefile */

#ifdef SYSVSHM
#include <machine/obsd_kernel_vmparam.h>
#include <sys/obsd_kernel_shm.h>
#endif
#ifdef SYSVSEM
#include <sys/obsd_kernel_sem.h>
#endif
#ifdef SYSVMSG
#include <sys/obsd_kernel_msg.h>
#endif

/*
 * System parameter formulae.
 *
 * This file is copied into each directory where we compile
 * the kernel; it should be modified there to suit local taste
 * if necessary.
 *
 * Compiled with -DHZ=xx -DTIMEZONE=x -DDST=x -DMAXUSERS=xx
 */

#ifndef TIMEZONE
# define TIMEZONE 0
#endif
#ifndef DST
# define DST 0
#endif

const int	hz = HAL_CLOCK_RATE_HZ;
const int	tick = 1000000 / HAL_CLOCK_RATE_HZ;
int	tickadj = 240000 / (60 * HAL_CLOCK_RATE_HZ);		/* can adjust 240ms in 60s */
struct	timezone tz = { TIMEZONE, DST };
#define NPROC 1

#define	NTEXT (80 + NPROC / 8)			/* actually the object cache */
#define	NVNODE (NPROC * 2 + NTEXT + 100)	 
int	desiredvnodes = NVNODE;
int	maxproc = NPROC;

/* OBSD_ITGR -- single threaded version, NPROC = 1, maxfiles = 90; Number too low for PNIO */
/* maxfiles is the maximum number of possible open files; this value cannot be adjusted by setrlimit */
/* it is used as maximum which can never be overcome */
int maxfiles = NOFILE_MAX;

int	nmbclust = NMBCLUSTERS;
int nmbclust_idle = NMBCLUSTERS_IDLE;

#ifndef MBLOWAT
#define MBLOWAT		16
#endif
int	mblowat = MBLOWAT;

#ifndef MCLLOWAT
#define MCLLOWAT	8
#endif
int	mcllowat = MCLLOWAT;

#ifndef BUFCACHEPERCENT
#define BUFCACHEPERCENT	20
#endif
int     bufcachepercent = BUFCACHEPERCENT;

#ifndef  BUFPAGES
#define BUFPAGES	0
#endif
long     bufpages = BUFPAGES;

int	fscale = FSCALE;	/* kernel uses `FSCALE', user uses `fscale' */

/*
 * Values in support of System V compatible shared memory.	XXX
 */
#ifdef SYSVSHM
#define	SHMMAX	SHMMAXPGS	/* shminit() performs a `*= PAGE_SIZE' */
#define	SHMMIN	1
#define	SHMMNI	128		/* <64k, see IPCID_TO_IX in ipc.h */
#define	SHMSEG	128
#define	SHMALL	(SHMMAXPGS)

struct	shminfo shminfo = {
	SHMMAX,
	SHMMIN,
	SHMMNI,
	SHMSEG,
	SHMALL
};
#endif

/*
 * Values in support of System V compatible semaphores.
 */
#ifdef SYSVSEM
struct	seminfo seminfo = {
	SEMMNI,		/* # of semaphore identifiers */
	SEMMNS,		/* # of semaphores in system */
	SEMMNU,		/* # of undo structures in system */
	SEMMSL,		/* max # of semaphores per id */
	SEMOPM,		/* max # of operations per semop call */
	SEMUME,		/* max # of undo entries per process */
	SEMUSZ,		/* size in bytes of undo structure */
	SEMVMX,		/* semaphore maximum value */
	SEMAEM		/* adjust on exit max value */
};
#endif

/*
 * This has to be allocated somewhere; allocating
 * them here forces loader errors if this file is omitted
 * (if they've been externed everywhere else; hah!).
 */