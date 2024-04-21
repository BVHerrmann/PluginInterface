/*	$OpenBSD: kern_sysctl.c,v 1.212 2012/01/21 16:30:31 guenther Exp $	*/
/*	$NetBSD: kern_sysctl.c,v 1.17 1996/05/20 17:49:05 mrg Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Karels at Berkeley Software Design, Inc.
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
 *	@(#)kern_sysctl.c	8.4 (Berkeley) 4/14/94
 */

/*
 * sysctl system call.
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
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_resourcevar.h>
#include <sys/obsd_kernel_signalvar.h>
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_filedesc.h>
#include <sys/obsd_kernel_vnode.h>
#include <sys/obsd_kernel_unistd.h>
#include <sys/obsd_kernel_buf.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_tty.h>
#include <sys/obsd_kernel_disk.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_msgbuf.h>
#include <sys/obsd_kernel_dkstat.h>
#include <sys/obsd_kernel_vmmeter.h>
#include <sys/obsd_kernel_namei.h>
#include <sys/obsd_kernel_exec.h>
#include <sys/obsd_kernel_mbuf.h>
#include <sys/obsd_kernel_socketvar.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_domain.h>
#include <sys/obsd_kernel_protosw.h>
#ifdef __HAVE_TIMECOUNTER
#include <sys/obsd_kernel_timetc.h>
#endif
#include <sys/obsd_kernel_evcount.h>
#include <sys/obsd_kernel_unpcb.h>

#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_syscallargs.h>

#include <dev/obsd_kernel_cons.h>
#include <dev/obsd_kernel_rndvar.h>

#include <net/obsd_kernel_route.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_in_pcb.h>
#include <netinet/obsd_kernel_tcp.h>
#include <netinet/obsd_kernel_tcp_timer.h>
#include <netinet/obsd_kernel_tcp_var.h>
#include <netinet/obsd_kernel_ip6.h>
#include <netinet6/obsd_kernel_ip6_var.h>

#ifdef SYSVMSG
#include <sys/obsd_kernel_msg.h>
#endif
#ifdef SYSVSEM
#include <sys/obsd_kernel_sem.h>
#endif
#ifdef SYSVSHM
#include <sys/obsd_kernel_shm.h>
#endif

extern struct forkstat forkstat;
extern struct nchstats nchstats;
extern int nselcoll, fscale;
extern struct disklist_head disklist;
extern fixpt_t ccpu;
extern  long numvnodes;
extern u_int mcllivelocks;

extern void nmbclust_update(void);

int sysctl_diskinit(int, struct proc *);
int sysctl_proc_args(int *, u_int, void *, size_t *, struct proc *);
int sysctl_proc_cwd(int *, u_int, void *, size_t *, struct proc *);
int sysctl_intrcnt(int *, u_int, void *, size_t *);
int sysctl_sensors(int *, u_int, void *, size_t *, void *, size_t);
int sysctl_emul(int *, u_int, void *, size_t *, void *, size_t);
int sysctl_cptime2(int *, u_int, void *, size_t *, void *, size_t);

int (*cpu_cpuspeed)(int *);
void (*cpu_setperf)(int);
int perflevel = 100;

int rthreads_enabled = 0;

/*
 * Lock to avoid too many processes vslocking a large amount of memory
 * at the same time.
 */
struct rwlock sysctl_lock = RWLOCK_INITIALIZER("sysctllk");
struct rwlock sysctl_disklock = RWLOCK_INITIALIZER("sysctldlk");

int
sys___sysctl(struct proc *p, void *v, register_t *retval)
{
	struct sys___sysctl_args /* {
		syscallarg(int *) name;
		syscallarg(u_int) namelen;
		syscallarg(void *) old;
		syscallarg(size_t *) oldlenp;
		syscallarg(void *) new;
		syscallarg(size_t) newlen;
	} */ *uap = v;
	int error /*, dolock = 1*/;        /* gerlach/gh2289n: removed unused variable to avoid compiler warning C4189 */
	size_t /*savelen = 0,*/ oldlen = 0;/* gerlach/gh2289n: removed unused variable to avoid compiler warning C4189 */
	sysctlfn *fn;
	int name[CTL_MAXNAME];

	OBSD_UNUSED_ARG(retval);          /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	if (SCARG(uap, new) != NULL &&
	    ((error = suser(p, 0)) != 0))      /* gh2289n: added 0 comparison to avoid PN compiler error */
		return (error);
	/*
	 * all top-level sysctl names are non-terminal
	 */
	if (SCARG(uap, namelen) > CTL_MAXNAME || SCARG(uap, namelen) < 2)
		return (EINVAL);
	error = copyin(SCARG(uap, name), name,
		       SCARG(uap, namelen) * sizeof(int));
	if (error)
		return (error);

	switch (name[0]) {
	case CTL_KERN:
		fn = kern_sysctl;
		break;
	case CTL_HW:
		fn = hw_sysctl;
		break;
	case CTL_NET:
		fn = net_sysctl;
		break;
	default:
		return (EOPNOTSUPP);
	}

	if (SCARG(uap, oldlenp) &&
	    ((error = copyin(SCARG(uap, oldlenp), &oldlen, sizeof(oldlen))) != 0))      /* gh2289n: added NULL comparison to avoid PN compiler error */
		return (error);
	error = (*fn)(&name[1], SCARG(uap, namelen) - 1, SCARG(uap, old),
	    &oldlen, SCARG(uap, new), SCARG(uap, newlen), p);
	if (error)
		return (error);
	if (SCARG(uap, oldlenp))
		error = copyout(&oldlen, SCARG(uap, oldlenp), sizeof(oldlen));
	return (error);
}

/*
 * Attributes stored in the kernel.
 */
char hostname[MAXHOSTNAMELEN];
int hostnamelen;
char domainname[MAXHOSTNAMELEN];
int domainnamelen;
long hostid;
char *disknames = NULL;
struct diskstats *diskstats = NULL;
#ifdef INSECURE
int securelevel = -1;
#else
int securelevel;
#endif

/*
 * kernel related system variables.
 */
int
kern_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlen, struct proc *p)
{
	OBSD_UNUSED_ARG(newlen);
	OBSD_UNUSED_ARG(newp);

	/* all sysctl names at this level are terminal except a ton of them */
	if (namelen != 1) {
		switch (name[0]) {
		case KERN_FILE2:
			break;
		default:
			return (ENOTDIR);	/* overloaded */
		}
	}

	switch (name[0]) {
	case KERN_FILE2:
		return (sysctl_file2(name + 1, namelen - 1, oldp, oldlenp, p));
	default:
		return (EOPNOTSUPP);
	}
	/* NOTREACHED */
}

/*
 * hardware related system variables.
 */
char *hw_vendor, *hw_prod, *hw_uuid, *hw_serial, *hw_ver;
int allowpowerdown = 1;

int
hw_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlen, struct proc *p)
{
	/*extern char machine[], cpu_model[]; int err, cpuspeed; *//* gerlach/gh2289n: avoid compiler warning C4101: unreferenced local variable */

	OBSD_UNUSED_ARG(p);          /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
	OBSD_UNUSED_ARG(newlen);     /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* all sysctl names at this level except sensors are terminal */
	if (name[0] != HW_SENSORS && namelen != 1)
		return (ENOTDIR);		/* overloaded */

	switch (name[0]) {
	case HW_PAGESIZE:
		return (sysctl_rdint(oldp, oldlenp, newp, PAGE_SIZE));
	default:
		return (EOPNOTSUPP);
	}
	/* NOTREACHED */
}

/*
 * Validate parameters and get old / set new parameters
 * for an integer-valued sysctl function.
 */
int
sysctl_int(void *oldp, size_t *oldlenp, void *newp, size_t newlen, int *valp)
{
	int error = 0;

	if (oldp && *oldlenp < sizeof(int))
		return (ENOMEM);
	if (newp && newlen != sizeof(int))
		return (EINVAL);
	*oldlenp = sizeof(int);
	if (oldp)
		error = copyout(valp, oldp, sizeof(int));
	if (error == 0 && newp)
		error = copyin(newp, valp, sizeof(int));
	return (error);
}

/*
 * As above, but read-only.
 */
int
sysctl_rdint(void *oldp, size_t *oldlenp, void *newp, int val)
{
	int error = 0;

	if (oldp && *oldlenp < sizeof(int))
		return (ENOMEM);
	if (newp)
		return (EPERM);
	*oldlenp = sizeof(int);
	if (oldp)
		error = copyout((caddr_t)&val, oldp, sizeof(int));
	return (error);
}

/*
 * Array of integer values.
 */
int
sysctl_int_arr(int **valpp, int *name, u_int namelen, void *oldp,
    size_t *oldlenp, void *newp, size_t newlen)
{
	if (namelen > 1)
		return (ENOTDIR);
	if (name[0] < 0 || valpp[name[0]] == NULL)
		return (EOPNOTSUPP);
	return (sysctl_int(oldp, oldlenp, newp, newlen, valpp[name[0]]));
}

/*
 * Validate parameters and get old / set new parameters
 * for an integer-valued sysctl function.
 */
int
sysctl_quad(void *oldp, size_t *oldlenp, void *newp, size_t newlen,
    int64_t *valp)
{
	int error = 0;

	if (oldp && *oldlenp < sizeof(int64_t))
		return (ENOMEM);
	if (newp && newlen != sizeof(int64_t))
		return (EINVAL);
	*oldlenp = sizeof(int64_t);
	if (oldp)
		error = copyout(valp, oldp, sizeof(int64_t));
	if (error == 0 && newp)
		error = copyin(newp, valp, sizeof(int64_t));
	return (error);
}

/*
 * As above, but read-only.
 */
int
sysctl_rdquad(void *oldp, size_t *oldlenp, void *newp, int64_t val)
{
	int error = 0;

	if (oldp && *oldlenp < sizeof(int64_t))
		return (ENOMEM);
	if (newp)
		return (EPERM);
	*oldlenp = sizeof(int64_t);
	if (oldp)
		error = copyout((caddr_t)&val, oldp, sizeof(int64_t));
	return (error);
}

/*
 * Validate parameters and get old / set new parameters
 * for a string-valued sysctl function.
 */
int
sysctl_string(void *oldp, size_t *oldlenp, void *newp, size_t newlen, char *str,
    int maxlen)
{
	return sysctl__string(oldp, oldlenp, newp, newlen, str, maxlen, 0);
}

int
sysctl_tstring(void *oldp, size_t *oldlenp, void *newp, size_t newlen,
    char *str, int maxlen)
{
	return sysctl__string(oldp, oldlenp, newp, newlen, str, maxlen, 1);
}

int
sysctl__string(void *oldp, size_t *oldlenp, void *newp, size_t newlen,
    char *str, int maxlen, int trunc)
{
	int /* len ,*/ error = 0;
	size_t len;               /* gerlach/gh2289n: changed type to size_t to avoid compiler warning C4267 & C4018 */

	len = strlen(str) + 1;
	if (oldp && *oldlenp < len) {
		if (trunc == 0 || *oldlenp == 0)
			return (ENOMEM);
	}
	if (newp && newlen >= (size_t)maxlen) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>=' : signed/unsigned mismatch */
		return (EINVAL);
	if (oldp) {
		if (trunc && *oldlenp < len) {
			len = *oldlenp;
			error = copyout(str, oldp, len - 1);
			if (error == 0)
				error = copyout("", (char *)oldp + len - 1, 1);
		} else {
			error = copyout(str, oldp, len);
		}
	}
	*oldlenp = len;
	if (error == 0 && newp) {
		error = copyin(newp, str, newlen);
		str[newlen] = 0;
	}
	return (error);
}

/*
 * As above, but read-only.
 */
int
sysctl_rdstring(void *oldp, size_t *oldlenp, void *newp, const char *str)
{
	int /* len,*/ error = 0;
	size_t len;               /* gerlach/gh2289n: changed type to size_t to avoid compiler warning C4267 & C4018 */

	len = strlen(str) + 1;
	if (oldp && *oldlenp < len)
		return (ENOMEM);
	if (newp)
		return (EPERM);
	*oldlenp = len;
	if (oldp)
		error = copyout(str, oldp, len);
	return (error);
}

/*
 * Validate parameters and get old / set new parameters
 * for a structure oriented sysctl function.
 */
int
sysctl_struct(void *oldp, size_t *oldlenp, void *newp, size_t newlen, void *sp,
    int len)
{
	int error = 0;

	if (oldp && *oldlenp < (size_t)len) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '<' : signed/unsigned mismatch */
		return (ENOMEM);
	if (newp && newlen > (size_t)len)   /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '>' : signed/unsigned mismatch */
		return (EINVAL);
	if (oldp) {
		*oldlenp = len;
		error = copyout(sp, oldp, len);
	}
	if (error == 0 && newp)
		error = copyin(newp, sp, len);
	return (error);
}

/*
 * Validate parameters and get old parameters
 * for a structure oriented sysctl function.
 */
int
sysctl_rdstruct(void *oldp, size_t *oldlenp, void *newp, const void *sp,
    int len)
{
	int error = 0;

	if (oldp && *oldlenp < (size_t)len) /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4018: '<' : signed/unsigned mismatch */
		return (ENOMEM);
	if (newp)
		return (EPERM);
	*oldlenp = len;
	if (oldp)
		error = copyout(sp, oldp, len);
	return (error);
}


void
fill_file2(struct kinfo_file2 *kf, struct file *fp, struct filedesc *fdp,
	  int fd, struct vnode *vp, struct proc *pp, struct proc *p)
{
	OBSD_UNUSED_ARG(pp);

	memset(kf, 0, sizeof(*kf));

	kf->fd_fd = fd;		/* might not really be an fd */

	if (fp != NULL) {
		kf->f_fileaddr = PTRTOINT64(fp);
		kf->f_flag = fp->f_flag;
		kf->f_iflags = fp->f_iflags;
		kf->f_type = fp->f_type;
		kf->f_count = fp->f_count;
		kf->f_msgcount = fp->f_msgcount;
		kf->f_ucred = PTRTOINT64(fp->f_cred);
		kf->f_uid = fp->f_cred->cr_uid;
		kf->f_gid = fp->f_cred->cr_gid;
		kf->f_ops = PTRTOINT64(fp->f_ops);
		kf->f_data = PTRTOINT64(fp->f_data);
		kf->f_usecount = fp->f_usecount;

		if (suser(p, 0) == 0 || p->p_ucred->cr_uid == fp->f_cred->cr_uid) {
			kf->f_offset = fp->f_offset;
			kf->f_rxfer = fp->f_rxfer;
			kf->f_rwfer = fp->f_wxfer;
			kf->f_seek = fp->f_seek;
			kf->f_rbytes = fp->f_rbytes;
			kf->f_wbytes = fp->f_rbytes;
		} else
			kf->f_offset = -1;
	} else if (vp != NULL) {
		/* fake it */
		kf->f_type = DTYPE_VNODE;
		kf->f_flag = FREAD;
		if (fd == KERN_FILE_TRACE)
			kf->f_flag |= FWRITE;
	}

	/* information about the object associated with this file */
	switch (kf->f_type) {
	case DTYPE_SOCKET: {
		struct socket *so = (struct socket *)fp->f_data;

		kf->so_type = so->so_type;
		kf->so_state = so->so_state;
		kf->so_pcb = PTRTOINT64(so->so_pcb);
		kf->so_protocol = so->so_proto->pr_protocol;
		kf->so_family = so->so_proto->pr_domain->dom_family;
#ifdef SOCKET_SPLICE
		if (so->so_splice) {
			kf->so_splice = PTRTOINT64(so->so_splice);
			kf->so_splicelen = so->so_splicelen;
		} else if (so->so_spliceback)
			kf->so_splicelen = -1;
#endif
		if (!so->so_pcb)
			break;
		switch (kf->so_family) {
		case AF_INET: {
			struct inpcb *inpcb = so->so_pcb;

			kf->inp_ppcb = PTRTOINT64(inpcb->inp_ppcb);
			kf->inp_lport = inpcb->inp_lport;
			kf->inp_laddru[0] = inpcb->inp_laddr.s_addr;
			kf->inp_fport = inpcb->inp_fport;
			kf->inp_faddru[0] = inpcb->inp_faddr.s_addr;
			kf->inp_rtableid = inpcb->inp_rtableid;
			if (so->so_proto->pr_protocol == IPPROTO_TCP) {
				struct tcpcb *tcpcb = (void *)inpcb->inp_ppcb;
				kf->t_state = tcpcb->t_state;
			}
			break;
		    }
		case AF_INET6: {
			struct inpcb *inpcb = so->so_pcb;

			kf->inp_ppcb = PTRTOINT64(inpcb->inp_ppcb);
			kf->inp_lport = inpcb->inp_lport;
			kf->inp_laddru[0] = inpcb->inp_laddr6.s6_addr32[0];
			kf->inp_laddru[1] = inpcb->inp_laddr6.s6_addr32[1];
			kf->inp_laddru[2] = inpcb->inp_laddr6.s6_addr32[2];
			kf->inp_laddru[3] = inpcb->inp_laddr6.s6_addr32[3];
			kf->inp_fport = inpcb->inp_fport;
			kf->inp_faddru[0] = inpcb->inp_faddr6.s6_addr32[0];
			kf->inp_faddru[1] = inpcb->inp_faddr6.s6_addr32[1];
			kf->inp_faddru[2] = inpcb->inp_faddr6.s6_addr32[2];
			kf->inp_faddru[3] = inpcb->inp_faddr6.s6_addr32[3];
			kf->inp_rtableid = inpcb->inp_rtableid;
			break;
		    }
		case AF_UNIX: {
			struct unpcb *unpcb = so->so_pcb;

			kf->unp_conn = PTRTOINT64(unpcb->unp_conn);
			break;
		    }
		}
		break;
	    }
	}

	if (fdp != NULL)
		kf->fd_ofileflags = fdp->fd_ofileflags[fd];
}

/*
 * Get file structures.
 */
int
sysctl_file2(int *name, u_int namelen, char *where, size_t *sizep,
    struct proc *p)
{
	struct kinfo_file2 *kf;
	struct file *fp;
	size_t buflen, outsize;
	int elem_size, elem_count; /* sado -- Greenhills compiler warning 186 - pointless comparison of unsigned integer with zero */
	char *dp = where;
	int arg, error = 0, needed = 0;
	u_int op;

	if (namelen > 4)
		return (ENOTDIR);
	if (namelen < 4 || name[2] > sizeof(*kf))
		return (EINVAL);

	buflen = where != NULL ? *sizep : 0;
	op = name[0];
	arg = name[1];
	elem_size = name[2];
	elem_count = name[3];
	outsize = MIN(sizeof(*kf), elem_size);

	if (elem_size < 1 || elem_count < 0)
		return (EINVAL);

	kf = malloc(sizeof(*kf), M_TEMP, M_WAITOK);

#define FILLIT(fp, fdp, i, vp, pp) do {				\
	if ((int)buflen >= elem_size && elem_count > 0) {		\
		fill_file2(kf, fp, fdp, i, vp, pp, p);		\
		error = copyout(kf, dp, outsize);		\
		if (error)					\
			break;					\
		dp += elem_size;				\
		buflen = (int)buflen - elem_size;				\
		elem_count--;					\
	}							\
	needed += elem_size;					\
} while (0)

	switch (op) {
	case KERN_FILE_BYFILE:
		if (arg != 0) {
			/* no arg in file mode */
			error = EINVAL;
			break;
		}
		LIST_FOREACH(fp, &filehead, f_list) {
			if (fp->f_count == 0)
				continue;
			FILLIT(fp, NULL, 0, NULL, NULL);
		}
		break;
	default:
		error = EINVAL;
		break;
	}
	free(kf, M_TEMP);

	if (!error) {
		if (where == NULL)
			needed += KERN_FILESLOP * elem_size;
		else if (*sizep < (size_t)needed)
			error = ENOMEM;
		*sizep = needed;
	}

	return (error);
}

