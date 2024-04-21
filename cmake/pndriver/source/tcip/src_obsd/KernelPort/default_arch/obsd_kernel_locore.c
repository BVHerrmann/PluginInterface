
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */


/* functions normally located in \sys\arch\i386\i386\locore.s */

#include <sys/obsd_kernel_param.h>      /* needed for systm.h */
#include <sys/obsd_kernel_systm.h>      /* prototypes */
#include <sys/obsd_kernel_filedesc.h>   /* requested from PNIO integration because GCC reports errors otherwise */
#include <sys/obsd_kernel_user.h>       /* struct user */
#include "obsd_kernel_init_main.h"      /* Kernel_init, Kernel_deinit */

extern void init386(void);              /* OBSD has no header file for this */
extern void deinit386(void);            /* no header file for this so far   */

static struct user UserProc0;
struct user *proc0paddr;

/*
 * The following primitives are used to copy data in and out of the user's
 * address space.
 * We have no real kernel / userland separation, so all of them lead to a normal copy operation.
 */


/* Copy len bytes into the user's address space. */
int copyout(const void* from, void* to, size_t len)
{
    memcpy(to, from, len);
    return 0;
}

/* Copy len bytes from the user's address space. */
int copyin(const void* from, void* to, size_t len)
{
    memcpy(to, from, len);
    return 0;
}

/*
 * copyoutstr(caddr_t from, caddr_t to, size_t maxlen, size_t *lencopied);
 * Copy a NUL-terminated string, at most maxlen characters long, into the
 * user's address space.  Return the number of characters copied (including the
 * NUL) in *lencopied.  If the string is too long, return ENAMETOOLONG; else
 * return 0 or EFAULT.
 */
int copyoutstr(const void *from, void *to, size_t maxlen, size_t *lencopied)
{
  return copystr( from, to, maxlen, lencopied);
}


/*
 * copyinstr(caddr_t from, caddr_t to, size_t maxlen, size_t *lencopied);
 * Copy a NUL-terminated string, at most maxlen characters long, from the
 * user's address space.  Return the number of characters copied (including the
 * NUL) in *lencopied.  If the string is too long, return ENAMETOOLONG; else
 * return 0 or EFAULT.
 */
int copyinstr(const void * from, void * to, size_t maxlen, size_t *lencopied)
{
  return copystr( from, to, maxlen, lencopied);
}


/*
 * copystr(caddr_t from, caddr_t to, size_t maxlen, size_t *lencopied);
 * Copy a NUL-terminated string, at most maxlen characters long.  Return the
 * number of characters copied (including the NUL) in *lencopied.  If the
 * string is too long, return ENAMETOOLONG; else return 0.
 */
int copystr(const void* from, void* to, size_t maxlen, size_t *lencopied)   /* see sys\arch\powerpc\powerpc\copystr.c */
{
    size_t inCopied;
    caddr_t _from = (caddr_t) from;                    /* alternative code for PN compilers */
    caddr_t _to   = (caddr_t) to;                      /* alternative code for PN compilers */


    for (inCopied = 0; maxlen-- > 0; inCopied++) 
    {
        /* gh2289n: avoid PN compiler warnings */
        /* if (!(*((caddr_t)to)++ = *((caddr_t)from)++))*/ /* this variant is assumed to be more efficient, but leads */
                                                           /* to a PN compiler error/abort (see OBSDPN-83 for details)*/
        if (!(*_to++ = *_from++))                          /* alternative code for PN compilers */
        {   /* normal end of string */
            if (lencopied)
                *lencopied = inCopied + 1;
            return (0);
        }
    }
    if (lencopied)
        *lencopied = inCopied;
    return (ENAMETOOLONG);
}


/*
 * kcopy(caddr_t from, caddr_t to, size_t len);
 * Copy len bytes, abort on fault.
 */
int kcopy(const void * from, void * to, size_t len)
{
    memcpy(to, from, len);
    return 0;
}

void obsd_stack_start(void)   /* normally start: label in locore.s */
{
    /* local init */
    memset(&UserProc0, sizeof(struct user), 0);
    proc0paddr = &UserProc0;

    init386();       
    Kernel_init();    
}

void obsd_stack_stop(void)
{
    /* local deinit */
    Kernel_deinit();    
    deinit386();       

    proc0paddr = NULL;
}

