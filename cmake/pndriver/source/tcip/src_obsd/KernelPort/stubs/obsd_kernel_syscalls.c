/**
 * @file        obsd_kernel_syscalls.c
 *
 * @brief       Public Kernel API.
 * @details     emulation for the syscall system normally implemented via kern/syscalls.c.
 *              kern/init_sysent.c, sys/syscallargs.h, sys/syscall.h and /sys/kern/syscalls.master
 * @author      Hendrik Gerlach / gh2289n
 *
 * @version     V0.14
 * @date        created: 06.11.2012
 * @defgroup    public_kernel_api Public Kernel API.
@{
*/

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_act_module) */
#define LTRC_ACT_MODUL_ID 4034 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_act_module) */
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#define SYSCALLS_INTERFACE          /* activate syscall renaming via obsd_kernel_BSDStackPort.h / */
                                    /* obsd_platform_cal_rename_syscalls.h                        */
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_poll.h>

#include <sys/obsd_kernel_syscallargs.h>

#include <machine/obsd_kernel_cpu.h>
#include <obsd_userland_errno.h>   /* obsd_errno */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

/* for system call numbers see:                                                                   */
/*   /usr/src/sys/kern/syscalls.c (table of names of system calls.)                               */
/*   /usr/src/sys/kern/init_sysent.c (containing the table sysent)                                */
/*   /usr/src/sys/sys/syscallargs.h  (containing system call argument lists.)                     */
/*   /usr/src/sys/sys/syscall.h      (containing system call numbers)                             */
/*   /usr/src/sys/kern/syscalls.master                                                            */


/* 3 = read */
int read(int fd, void *buf, size_t nbyte)
{
	struct proc *p;
	struct sys_read_args v;
	int retval = 0;
	int res=0;

	v.fd=fd;
	v.buf=buf;
	v.nbyte = nbyte;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_read(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_001) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_read on socket %u failed with error %u", fd, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_001) */
			p->p_upls.pls_errno = res;   /* errno = res;*/
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_002) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_read on socket %u successful", fd);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_002) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 4 = write */
ssize_t  write(int fildes, const void* buf, size_t nbyte)
{
	struct proc *p;
	struct sys_write_args v;
	register_t retval	= 0;
	int res				= 0;

	v.fd	= fildes;
	v.buf	= buf;
	v.nbyte	= nbyte;
	
	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res= sys_write(p,&v,&retval);
		if(res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_003) */
			if (res != EWOULDBLOCK && res != EINPROGRESS && res != ESRCH) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_write on socket %u failed with error %u", fildes, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_003) */
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_004) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_write on socket %u successful", fildes);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_004) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 6 = close */
int close(int fildes)
{
	struct proc *p;
	struct sys_close_args v;
	register_t retval	= 0;
	int res		= 0;

	v.fd = fildes;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_close(p,&v,NULL);      /* retval not used in sys_close */
		if(res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_005) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_close on socket %u failed with error %u", fildes, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_005) */
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_006) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_close on socket %u successful", fildes);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_006) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 20 = getpid */
pid_t getpid(void)
{
	return 1;  /* in case of single thread operation always return pid=1 */
}

/* 27 = recvmsg */
ssize_t	recvmsg(int s, struct msghdr *msg, int flags)
{
	struct proc *p;
	struct sys_recvmsg_args v;
	register_t retval	= 0;
	int res = 0;
	v.s=s;
	v.msg=msg;
	v.flags=flags;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_recvmsg(p,&v,&retval);
		if (res!=0)
		{
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 28 = sendmsg */
ssize_t	sendmsg(int s, const struct msghdr *msg, int flags)
{
	struct proc *p;
	struct sys_sendmsg_args v;
	register_t retval	= 0;
	int res;
	v.s=s;
	v.msg=msg;
	v.flags=flags;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_sendmsg(p,&v,&retval);
		if (res != 0)
		{
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* 29 = recvfrom */
ssize_t	recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen)
{
	struct proc *p;
	struct sys_recvfrom_args v;
	register_t retval	= 0;
	int res=0;

	v.s				= s;
	v.buf			= buf;
	v.len			= len;
	v.flags			= flags;
	v.from			= from;
	v.fromlenaddr	= fromlen;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_recvfrom(p,&v,&retval);
		if (res!=0)
		{
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_007) */
	if (retval > 0)
	{
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_receive: received %u bytes", retval);
	}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_007) */
	return retval;
}

/* 30 = accept */
int	accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	struct proc *p;
	struct sys_accept_args v;
	register_t retval = 0;
	int res=0;

	v.s=s;
	v.name=addr;
	v.anamelen=addrlen;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_accept(p,&v,&retval);

		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_008) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_accept on socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_008) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_009) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_accept on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_009) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* 32 = getsockname */
int	getsockname(int s, struct sockaddr *name, socklen_t *name_len)
{
	struct proc *p;
	struct sys_getsockname_args v;
	register_t retval = 0;
	int res;
	v.fdes=s;
	v.asa=name;
	v.alen=name_len;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_getsockname(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_010) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_getsockname of socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_010) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_011) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_getsockname on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_011) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 46 = sigaction */
int sigaction(int sig, const struct sigaction* act, struct sigaction* oact)
{
	struct proc *p;
	struct sys_sigaction_args v;
	register_t retval	= 0;
	int res				= 0;

	v.signum	= sig;
	v.nsa		= act;
	v.osa		= oact;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_sigaction(p,&v,&retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_012) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_sigaction of signal %u failed with error %u", sig, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_012) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_013) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_sigaction of signal %u successful", sig);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_013) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 54 = ioctl */
int	ioctl(int dev, unsigned long cmd,caddr_t addr)   /* low level control */
{
	struct proc *p;
	struct sys_ioctl_args v;
	int retval=0;
	int res=0;

	v.fd=dev;
	v.data=addr;
	v.com=cmd;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_ioctl(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_014) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				if (res != EADDRNOTAVAIL && res != ENXIO) TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "sys_ioctl for device %u with command 0x%lx failed with error %u", dev, cmd, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_014) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_015) */
		else
		{
			TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_CHAT, "sys_ioctl for device %u with command 0x%lx successful", dev, cmd);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_015) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 73 = munmap */
int munmap(void *addr, size_t len)
{
	struct proc *p;
	struct sys_munmap_args v;
	register_t retval	= 0;
	int res				= 0;

	v.addr = addr;
	v.len  = len;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_munmap(p, &v, &retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_016) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_munmap of address 0x%x failed with error %u", (uint32_t)addr, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_016) */
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 92 = fcntl */
int fcntl(int fd, int cmd, ...)
{
	struct proc *p;
	struct sys_fcntl_args v;
	register_t retval = 0;
	int res = 0;
	void* arg;

	va_list ap;

	va_start(ap, cmd);
	arg = va_arg(ap, void* );
	va_end(ap);	

	v.fd	= fd;
	v.cmd	= cmd;
	v.arg	= arg;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_fcntl(p,&v,&retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_017) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "sys_fcntl for socket %u with command 0x%lx failed with error %u", fd, cmd, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_017) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_018) */
		else
		{
			TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_CHAT, "sys_fcntl for socket %u with command 0x%lx successful", fd, cmd);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_018) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 93 = select */
int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout)
{
	struct proc *p;
	struct sys_select_args v;
	register_t retval = 0;
	int res				= 0;
	
	v.nd	= nfds;
	v.in	= readfds;
	v.ou	= writefds;
	v.ex	= exceptfds;
	v.tv	= timeout;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_select(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_019) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_select on socket %u failed with error %u", nfds, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_019) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
		/* No tracing due to high traffic */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 97 = socket */
int	socket(int af, int type, int protocol)
{
	struct proc *p;
	struct sys_socket_args v;
	register_t retval = 0;
	int res=0;

	v.domain=af;
	v.protocol=protocol;
	v.type=type;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_socket(p, &v, &retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_020) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "sys_socket failed with error %u", res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_020) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_021) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_socket: socket %u successfully created", retval);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_021) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 98 = connect */
int	connect(int s, const struct sockaddr *name, socklen_t name_len)
{
	struct proc *p;
	struct sys_connect_args v;
	register_t retval = 0;
	int res;
	v.s=s;
	v.name=name;
	v.namelen=name_len;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_connect(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_022) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_connect on socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_022) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_023) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_connect on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_023) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* 104 = bind */
int	bind(int s, const struct sockaddr *name, socklen_t name_len)
{
	struct proc *p;
	struct sys_bind_args v;
	register_t retval = 0;
	int res;
	v.s=s;
	v.name=name;
	v.namelen=name_len;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_bind(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_024) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_bind on socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_024) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_025) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_bind on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_025) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 105 = setsockopt */
int	setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
	struct proc *p;
	struct sys_setsockopt_args  v;
	register_t retval = 0;
	int res;
	v.s=s;
	v.level=level;
	v.name=optname;
	v.val=optval;
	v.valsize=optlen;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_setsockopt(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_026) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "sys_setsockopt on socket %u and option %u failed with error %u", s, optname, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_026) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_027) */
		else
		{
			TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_CHAT, "sys_setsockopt on socket %u and option %u successful", s, optname);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_027) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 106 = listen */
int	listen(int s, int backlog)
{
	struct proc *p;
	struct sys_listen_args  v;
	register_t retval = 0;
	int res;
	v.s=s;
	v.backlog=backlog;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_listen(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_028) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_listen on socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_028) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_029) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_listen on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_029) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 116 = gettimeofday */
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	struct proc *p;
	struct sys_gettimeofday_args v;
	register_t retval	= 0;
	int res				= 0;
	
	v.tp		= tp;
	v.tzp		= tzp;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_gettimeofday(p,&v,&retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_030) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "sys_gettimeofday failed with error %u", res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_030) */
			p->p_upls.pls_errno = res;   /* errno	= res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 118 = getsockopt */
int	getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
	struct proc *p;
	struct sys_getsockopt_args  v;
	register_t retval	= 0;
	int res;
	v.s=s;
	v.level=level;
	v.name=optname;
	v.val=optval;
	v.avalsize=optlen;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_getsockopt(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_031) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "sys_getsockopt on socket %u and option %u failed with error %u", s, optname, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_031) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_032) */
		else
		{
			TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_CHAT, "sys_getsockopt on socket %u and option %u successful", s, optname);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_032) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* 133 = sendto */
ssize_t	sendto(int s, const void *buf,
		size_t len, int flags, const struct sockaddr *to, socklen_t tolen)
{
	struct proc *p;
	struct sys_sendto_args v;
	register_t retval	= 0;
	int res=0;
	v.s=s;
	v.buf=buf;
	v.len=len;
	v.flags=flags;
	v.to=to;
	v.tolen=tolen;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_sendto(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_033) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_sendto on socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_033) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_034) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_sendto on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_034) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_035) */
	if (retval > 0)
	{
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_send: sent %u bytes", retval);
	}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_035) */
	return retval;
}

/* 134 = shutdown */
int	shutdown(int s, int flags)
{
	struct proc *p;
	struct sys_shutdown_args v;
	register_t retval	= 0;
	int res=0;
	v.s=s;
	v.how=flags;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res=sys_shutdown(p, &v, NULL);  /* retval not used in sys_shutdown */
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_036) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_shutdown on socket %u failed with error %u", s, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_036) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_037) */
		else
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys_shutdown on socket %u successful", s);
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_037) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 195 = setrlimit */
int setrlimit(int resource, const struct rlimit *rlp)
{
	struct proc *p;
	struct sys_setrlimit_args v;
	int retval = 0;
	int res=0;

	v.which = resource;
	v.rlp = rlp;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_setrlimit(p,&v,&retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_038) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "sys_setrlimit for resource %u failed with error %u", resource, res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_038) */
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* 197 = mmap */
void * mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	struct proc *p;
	struct sys_mmap_args v;
	void * 		retval	= NULL;
	int 		res		= 0;

	v.addr  = addr;
	v.len   = len;
	v.prot  = prot;
	v.flags = flags;
	v.fd    = fd;
	v.pad   = 0;
	v.pos   = offset;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_mmap(p, &v, &retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_039) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "sys_mmap failed with error %u", res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_039) */
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = NULL;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = NULL;
	}
	/* sys_kernel_leave(); */
	return retval;
}

/* 202 = __sysctl */
int __sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	struct proc *p;
	struct sys___sysctl_args v;
	register_t retval = 0;
	int res=0;

	v.name		= name;
	v.namelen	= namelen;
	v.old		= oldp;
	v.oldlenp	= oldlenp;
	v.new		= newp;
	v.newlen	= newlen;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys___sysctl(p,&v,&retval);
		if (res!=0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_040) */
			if (res != EWOULDBLOCK && res != EINPROGRESS && res != ENOENT) {
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "sys__sysctl returned response %u", res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_040) */
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
		/* No tracing -- tracing shifted in protocol-specific sysctl */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}

 /* 232 = clock_gettime */
int clock_gettime(clockid_t clock_id, struct timespec* tp)
{
	struct proc *p;
	struct sys_clock_gettime_args v;
	register_t retval	= 0;
	int res				= 0;

	v.clock_id	= clock_id;
	v.tp		= tp;
	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_clock_gettime(p,&v,&retval);
		if (res != 0)
		{
			p->p_upls.pls_errno = res;   /* errno=res; */
			retval = -1;
		}
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* 252 = poll */
int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    struct proc *p;
    struct sys_poll_args v;
    register_t retval	= 0;
    int res				= 0;

    v.fds       = fds;
    v.nfds      = nfds;
    v.timeout   = timeout;

    /* sys_kernel_enter(); */
    p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
    if (p != NULL)
    {
        res = sys_poll(p,&v,&retval);
        if (res != 0)
        {
            p->p_upls.pls_errno = res;   /* errno = res; */
            retval = -1;
        }
        userret(p);
    }
    else
    {   /* thread/proc unknown and not able to create a new proc for the thread */
        /* assume we have no process and so no process specific upls/errno, use global errno in this case */
        obsd_errno = ESRCH;   /* errno = ESRCH; */
        retval = -1;
    }
    /* sys_kernel_leave(); */
    return retval;
}

/* 311 = getrtable */
int getrtable(void)
{
	struct proc *p;
	register_t retval	= 0;
	int res				= 0;

	/* sys_kernel_enter(); */
	p = sys_proc_register(NULL, NULL);  /* get proc pointer if thread is known or create a new proc for the thread otherwise */
	if (p != NULL)
	{
		res = sys_getrtable(p,NULL,&retval);
		if (res != 0)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_041) */
			if (res != EWOULDBLOCK && res != EINPROGRESS) {
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "sys_getrtable failed with error %u", res);
			}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_041) */
			p->p_upls.pls_errno = res;   /* errno = res; */
			retval = -1;
		}
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_syscalls_042) */
		else
		{
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "sys_getrtable successful");
		}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_syscalls_042) */
		userret(p);
	}
	else
	{   /* thread/proc unknown and not able to create a new proc for the thread */
		/* assume we have no process and so no process specific upls/errno, use global errno in this case */
		obsd_errno = ESRCH;   /* errno = ESRCH; */
		retval = -1;
	}
	/* sys_kernel_leave(); */
	return retval;
}


/* ============ additional "systemcalls" for out stack port ================ */
/* note: obsd_get_upls may return NULL if the process is not known and cannot be registered       */
upls_t * obsd_get_upls(void)
{
	struct proc *p = &proc0;
	if (p != NULL)
		return &(p->p_upls);
	else
		return NULL;
}

int obsd_get_mmap_size(void *addr)
{
	struct proc *p = &proc0;
	if (p != NULL)
		return sys_get_mmap_size(p, addr);
	else
		return -1;
}



/** @} */ /* group public_kernel_api */


