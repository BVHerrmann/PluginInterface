
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_portsys_act_module) */
#define LTRC_ACT_MODUL_ID 4037 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_portsys_act_module) */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_proc.h>       /* pfind */
#include <sys/obsd_kernel_queue.h>

#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_syscallargs.h>

#include <sys/obsd_kernel_filedesc.h>   /* for stdio / FREAD and so on */
#include <sys/obsd_kernel_signalvar.h>  /* requested from PNIO integration because GCC reports errors otherwise */
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_stat.h>       /* S_IFCHR and struct stat */

#include <sys/obsd_kernel_termios.h>    /* struct termios */

#include <machine/obsd_kernel_cpu.h>
#include "obsd_kernel_portsys.h"        /* own interface  */
#include "obsd_platform_osal.h"         /* OSAL_GetCurPid */
#include "obsd_platform_hal.h"

/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */



#define MAX_PANIC_STRING_LEN    128
#define MAX_LOG_STRING_LEN      256
#define OBUFSIZ                 128

struct umalloc_item_list_item  /* userland malloc item list item */
{
    struct umlist_head umlist_list_head;
    SLIST_ENTRY(umalloc_item_list_item) next;
};

static int          FreePendingUserlandMallocs(void);

/* message buf pointer */
/* currently we don't use OBSD's kernel logging system (kern/subr_log.c, kern/subr_prf). so we don't  */
/* have a logging buffer (normally accessable via dmesg) and that why we can't use the logging buffer */
/* as input for the entropy pool in dev/rnd.c                                                         */
void *msgbufp = NULL;

int cold = 1;  /* cold boot var for the OpenBSD kernel, reset to 0 after cold boot is done */

/* list of pending userland mallocs after a process was destroyed */
static SLIST_HEAD(umalloc_item_list_item_head, umalloc_item_list_item) s_umalloc_list_list_head;    /* list of list with userland malloc items */


void StartStackKernel(void)  /* entrypoint to start the whole stack */
{
    SLIST_INIT(&s_umalloc_list_list_head);
    memset(&s_umalloc_list_list_head, 0, sizeof(struct umalloc_item_list_item_head));
    obsd_stack_start();
}

void StopStackKernel(void)   /* stop the whole stack */
{
    obsd_stack_stop();
    FreePendingUserlandMallocs();
}



/* --------------- sys\kern\kern_event.c --------------------------------------*/
/*
 * walk down a list of knotes, activating them if their event has triggered.
 */
void knote(struct klist *list, long hint)
{
    OBSD_UNUSED_ARG(list); 
    OBSD_UNUSED_ARG(hint);         /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
    /* dummy for knote. maybe we need the kernel event system (kern_event.c) later */
    OSAL_NotImplemented(__func__, __FILE__);
}

/*
 * remove all knotes referencing a specified fd
 */
void knote_fdclose(struct proc *p, int fd)
{
    OBSD_UNUSED_ARG(p); 
    OBSD_UNUSED_ARG(fd);              /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
    /* dummy for knote_fdclose. maybe we need the kernel event system (kern_event.c) later */
    OSAL_NotImplemented(__func__, __FILE__);
}


/* --------------- sys\kern\kern_sig.c --------------------------------------*/
/*
 * Deliver signum to pgid, but first check uid/euid against each
 * process and see if it is permitted.
 */
int csignals = 0;
void csignal(pid_t pgid, int signum, uid_t uid, uid_t euid)
{
    OBSD_UNUSED_ARG(pgid); 
    OBSD_UNUSED_ARG(signum); 
    OBSD_UNUSED_ARG(uid); 
    OBSD_UNUSED_ARG(euid); /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */
    csignals++;
    OSAL_NotImplemented(__func__, __FILE__);
}



/* --------------- sys\kern\vfs_vops.c --------------------------------------*/

/*int VOP_ADVLOCK(struct vnode *vp, void *id, int op, struct flock *fl, int flags)
 *{
 *    return (EOPNOTSUPP);
 *}
 */

void	boot(int howto)
{
	OSAL_event_panic_reboot(howto);
}

void cnputc(int c)
{
    OSAL_kernel_putchar(c);
}


int log_open = 1;	/* subr_log: is /dev/klog open? */ /* gh2289n: always open */

void logwakeup(void)
{
    /* currently not supported, no log sockets */
}


/* --------------- sys\uvm\uvm_km.c ----------------------------------------*/

const struct kmem_pa_mode kp_dma_contig = {
	0
};

/* -------------------------------------------------------------------------*/
/* v===================== kern_malloc.c ==================================v */
void *obsd_kernel_malloc(unsigned long size, int type, int flags)
{
    return OSAL_kern_malloc(size, type, flags);
}

void obsd_kernel_free(void *addr, int type)
{
    OSAL_kern_free(addr, type);
}

/* ^===================== kern_malloc.c ==================================^ */


/* v===================== uvm_map.c ======================================v */

int AddPendingUserlandMallocs(struct umlist_head * item_list_head)
{
    struct umalloc_item_list_item *plist_list_item = OSAL_kern_malloc(sizeof (struct umalloc_item_list_item), M_TEMP, 0);

    if (plist_list_item != NULL)
    {
        memset(plist_list_item, 0, sizeof(struct umalloc_item_list_item));
        plist_list_item->umlist_list_head  = *item_list_head;

        SLIST_INSERT_HEAD(&s_umalloc_list_list_head, plist_list_item, next);
    }
    return 0;
}

static int FreePendingUserlandMallocs(void)
{
    SLIST_HEAD(umlist_head_flat, umalloc_item) umalloc_list_head_flat;    /* new flat list of userland malloc items */
    struct umalloc_item_list_item_head * pumalloc_list_list_head = &s_umalloc_list_list_head;
    int i,j;
    /* int k=1; */

    SLIST_INIT(&umalloc_list_head_flat);

    /* loop over the list of pending malloc's list */
    for (i=1;;i++)
    {
        struct umlist_head  * headp = NULL;

        if (SLIST_EMPTY(pumalloc_list_list_head))
        {
            break;
        }
        headp = (struct umlist_head  *) (SLIST_FIRST(pumalloc_list_list_head));       /* get the head of the list with pending mallocs */
        SLIST_REMOVE_HEAD(  pumalloc_list_list_head, next); /* remove it from head list */

        /* loop over all elements in the list of alloc'ed memory areas */
        /* headp is the head of the list of pending mallocs */
        for (j=1;;j++)
        {
            struct umalloc_item * item;


            if (SLIST_EMPTY(headp))
            {
                break;
            }

            item = SLIST_FIRST(headp);   /* get the item from the list */
            SLIST_REMOVE_HEAD(headp, next);

            /* insert the items from the different list into one flat list to have the chance to  */
            /* find double entries, the new list is not shared so no locking is needed            */
            SLIST_INSERT_HEAD(&umalloc_list_head_flat, item, next);
            /*OSAL_printf("list %d, item %02d (#%d, 0x%p), 0x%p:%d\n", i , j, k++, item, item->mem_addr, item->mem_size);*/
        } /* for */
        OSAL_kern_free((void *)headp, M_TEMP);                  /* free the head list item  */
    } /* for */

    {   /* now we have a flat list of allocated memory items, free the associated memory */
        struct umalloc_item * item;
        int double_items              = 0;
        int inconsistent_double_items = 0;
        /* int k                         = 1; */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_portsys_001) */
		if (SLIST_EMPTY(&umalloc_list_head_flat))
		{
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_NOTE_HIGH, "FreePendingUserlandMallocs: List is empty ;-)");
		}
		else
		{
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "FreePendingUserlandMallocs: Allocated Memory remaining --> Memory Leak??");
		}
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_portsys_001) */

        while(!SLIST_EMPTY(&umalloc_list_head_flat))
        {
            /* int is_double_item = 0; sado -- Greenhills compiler warning 550 - set, but never used */

            item = SLIST_FIRST(&umalloc_list_head_flat);   /* get the item from the list */
									
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_portsys_003) */
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "FreePendingUserlandMallocs: 0x%x is left", (uint32_t)item->mem_addr);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_portsys_003) */

            SLIST_REMOVE_HEAD(&umalloc_list_head_flat, next);
            /*OSAL_printf("%d (item=0x%p) 0x%p:%d\n", k++, item, item->mem_addr, item->mem_size);*/

            {   /* check if there is an double item */
                struct umalloc_item * double_item, *double_item_temp;

                SLIST_FOREACH_SAFE(double_item, &umalloc_list_head_flat, next, double_item_temp)
                {
                    if (item->mem_addr == double_item->mem_addr)
                    {
                        if (item->mem_size == double_item->mem_size)
                            double_items++;
                        else
                            inconsistent_double_items++;

                        /* is_double_item = 1;  sado -- Greenhills compiler warning 550 - set, but never used */
                        /* remove double items silently without freeing the memory at mem_addr    */
                        /* (would cause heap corruption)                                          */
                        /* assume double items are from global buffers shared between             */
                        /* different processes, a typical case is the buf allocated in setvbuf    */
                        SLIST_REMOVE(&umalloc_list_head_flat, double_item, umalloc_item, next);
                        OSAL_kern_free((void *)double_item, M_TEMP);     /* free list item itself */
                    }
                }
            }
            /* if (!is_double_item)  */          /* use this to find the cause of the double item */
            if (1)    /* always free the latest of the double items */
            {   /* disable this (e.g. by suited if condition) if we wan't to find memory leaks    */
                /* using an external memory checker tool like Memory Validator                    */
                OSAL_kern_free((void *)item->mem_addr, M_TEMP);       /* free allocated memory    */
            }
            else
            {
            }
            OSAL_kern_free((void *)item, M_TEMP);                 /* free the list item itself    */
        }
    }
    return 0;
}


/*
 * sys_mmap: mmap system call.
 * only len parameter is supported so far 
 */

int sys_mmap(struct proc *p, void *v, void **retval)
{
	struct sys_mmap_args /* {
		syscallarg(void *) addr;
		syscallarg(size_t) len;
		syscallarg(int) prot;
		syscallarg(int) flags;
		syscallarg(int) fd;
		syscallarg(long) pad;
		syscallarg(off_t) pos;
	} */ *uap = v;
	size_t size;
	int flags;
	void *pmem = NULL;

	/* first, extract syscall args from the uap.*/
	size    = (vsize_t) SCARG(uap, len);
	flags   = (int)     SCARG(uap, flags);
	pmem = OSAL_kern_malloc((unsigned long) size, M_TEMP, (flags & M_ZERO) != 0 ? OSAL_M_ZERO : 0); /* only M_ZERO supported so far */

	if (pmem != NULL)
	{
		/* record the allocation to the process's allocations list */
		struct umlist_head  * headp = &p->p_upls.umalloc_list_head;
		struct umalloc_item * item = OSAL_kern_malloc(sizeof (struct umalloc_item), M_TEMP, 0);

		if (item)
		{
			item->mem_addr = pmem;
			item->mem_size = size;
			/* because each thread / process has it's own list during list manipulation here's no */
			/* further protection against thread switch necessary                                 */
			SLIST_INSERT_HEAD(headp, item, next);
#ifdef    UMLIST_CHECK
			umlist_check(headp);
#endif /* UMLIST_CHECK */
		}
	}
	*retval = pmem;
	return 0;
}


/*
 * sys_munmap: unmap a users memory
 */
int sys_munmap(struct proc *p, void *v, register_t *retval)
{
	struct sys_munmap_args /* {
		syscallarg(void *) addr;
		syscallarg(size_t) len;
	} */ *uap = v;
	void * addr; 
	/*vsize_t size; */
	int AddrFound = 0;

	struct umlist_head  * headp = &p->p_upls.umalloc_list_head;
	struct umalloc_item * item, *item_temp;

	OBSD_UNUSED_ARG(retval);   /* gerlach/gh2289n: avoid compiler warning C4100: unreferenced formal parameter */

	/* get syscall args */
	addr = SCARG(uap, addr);
	/* size = (vsize_t) SCARG(uap, len);   currently we have no len argument */

	OSAL_kern_free((void *)addr, M_TEMP);

	/* remove the entry in the userland malloc list */
#ifdef    UMLIST_CHECK
	umlist_check(headp);
#endif /* UMLIST_CHECK */

	/* Following solution is simple because we only expect a few memory allocations within    */
	/* each userland process. If we later will have processes with many userland memory       */
	/* allocation this could result in bad performance here and so we should find a better    */
	/* solution than a simple list implementation / lookup here                               */
	SLIST_FOREACH_SAFE(item, headp, next, item_temp) 
	{
		if (item->mem_addr == addr)
		{
			SLIST_REMOVE(headp, item, umalloc_item, next);
			OSAL_kern_free((void *)item, M_TEMP);
			AddrFound = 1;
		}
	}
	if (!AddrFound && (addr != NULL))
	{
		/* possible free of a address not malloc'd by us */
		/*int a=3;*/ /* possible breakpoint */
	}

	return 0;
}

/*
 * sys_get_mmap_size: return size of memory allocated by mmap
 */
int sys_get_mmap_size(struct proc *p, void *addr)
{
	struct umlist_head  * headp = &p->p_upls.umalloc_list_head;
	struct umalloc_item * item;

	SLIST_FOREACH(item, headp, next) 
	{
		if (item->mem_addr == addr)
		{
			return item->mem_size;
		}
	}

	return -1;
}

/* ^===================== uvm_map.c ======================================^ */


void bzero(void *buf, size_t len)
{
	memset(buf,0,len);
}

int bcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1,s2,n);
}

void ovbcopy(const void *src, void *dest, size_t n)
{
	bcopy((void *)src,(void*)dest,n);
}


/* a simple bcopy implementation that should be enough for us */
void bcopy(const void *src, void *dst, size_t len)
{
	memcpy(dst,src,len);    /* our memcpy in libkern is overlapping safe like necessary for bcopy */
}


/* v============== dummy ioconf data (needed by subr_autoconf.c)==========v */
struct cfdata cfdata[] = {
        {(struct cfattach *)-1}
};

short cfroots[] = {
	-1
};

/* ^======================================================================^ */

