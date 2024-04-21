#define LTRC_ACT_MODUL_ID 4070 /* OBSD_ITGR added for LSA traces */

#include <obsd_userland_stdio.h>
#include <obsd_userland_string.h>

#include <obsd_userland_errno.h>     /* errno var and EFAULT */
#include "obsd_platform_osal.h"

#include <sys/obsd_kernel_malloc.h>  /* M_ZERO               */
#include <sys/obsd_kernel_mman.h>    /* errno var and EFAULT */
#include <obsd_userland_netdb.h>     /* gh2289n: struct netent, struct hostent */

#include <tcip_obsd_trc.h>

/* ------------------------------------------------------------------------------------ */
/* normally __progname comes from every userland program's crt0. But we don't have      */
/* different userland processes with executeable programs each having their own crt0    */
/* and there own name.                                                                  */
/* So our only chance later would be to link threads to proc's, give every proc a name  */
/* and return the name in the proc structure to the userland. This would require a new  */
/* kernel API. And it would require that a thread typically execute a dedicated         */
/* "program" (e.g. ifconfig") and this program sets its own name via a (new) syscall.   */
/* In this case we would define __progname to a (new) syscall.                          */
/* So far we dont have all of this stuff but functions like verr.c, verrx.c, vwarn.c,   */
/* vwarnx.c use __progname for error messages today.                                    */
#ifndef    _KERNEL    /* only in userland, kernel should never use __progname           */
char* __progname  = "IP-Stack-Appl"; /* all our code runs in the same proc, we can't   */
                                      /* have real program names now                    */
#endif  /* ! _KERNEL */


/* ------------------------------------------------------------------------------------ */
int __isthreaded = 0;  /* gh2289n: normally in libc\stdlib\exit.c, currently our stdio is not thread safe maybe */

void obsd_userland_abort(void)
{
	exit(0);
}

/* ------------------------------------------------------------------------------------ */
void    *obsd_userland_memmove(void *dst, const void *src, size_t size)
{
	return memcpy(dst,src,size);
}

/* ------------------------------------------------------------------------------------ */
/* very simple malloc/free implementation, let it alloc/free by kernel */
void    *obsd_userland_malloc(size_t size)
{   
	void *ptr = obsd_mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, (off_t)0);
#ifdef OBSD_MEMORY_TRACING
#if OBSD_MEMORY_TRACING_DETAILS != 0
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_NOTE_HIGH, "userland_malloc: 0x%x", (uint32_t)ptr);
#endif
#endif
	return ptr;
}

void     obsd_userland_free(void * addr)
{
#ifdef OBSD_MEMORY_TRACING
#if OBSD_MEMORY_TRACING_DETAILS != 0
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_NOTE_HIGH, "userland_free: 0x%x", (uint32_t)addr);
#endif
#endif
	obsd_munmap(addr, 0);   /* set len = 0 because we don't know it better */
}

void *   obsd_userland_calloc(size_t nelem, size_t elsize)
{
    return obsd_mmap(NULL, nelem * elsize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE | M_ZERO, -1, (off_t) 0);
}

/*   man 3 malloc:
     The realloc() function changes the size of the object pointed to by ptr
     to size bytes and returns a pointer to the (possibly moved) object.  The
     contents of the object are unchanged up to the lesser of the new and old
     sizes.  If the new size is larger, the value of the newly allocated
     portion of the object is indeterminate and uninitialized.  If ptr is a
     null pointer, the realloc() function behaves like the malloc() function
     for the specified size.  If the space cannot be allocated, the object
     pointed to by ptr is unchanged.  If size is zero and ptr is not a null
     pointer, the object it points to is freed and a new zero size object is
     returned.
*/
/* partly implemented realloc function (no real reallocation implemented */
void    *obsd_userland_realloc(void *ptr, size_t size)
{   /* If ptr is a null pointer, the realloc() function behaves like the malloc() function for the specified size. */

	void * new_mem = NULL;

    if (ptr == NULL) 
    {
        new_mem = obsd_userland_malloc(size);
    }
    else  /* ptr != NULL */
    {
        if (size > 0)
        {  
			new_mem = obsd_userland_malloc(size);
            if (new_mem != NULL)
            {	
				int allocated_size = obsd_get_mmap_size(ptr);

				if (allocated_size < 0) /* pointer not found in memory management */
				{
					obsd_userland_free(new_mem);
					new_mem = NULL;
					errno = EINVAL;
				}
				else if ((unsigned int)allocated_size <= size)
				{
					bcopy(ptr, new_mem, allocated_size);
				}
				else
				{
					bcopy(ptr, new_mem, size);
				}
			}
            else
            {
				errno = ENOMEM;
            }
        }
    }

#ifdef OBSD_MEMORY_TRACING
#if OBSD_MEMORY_TRACING_DETAILS != 0
	TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_NOTE_HIGH, "realloc: ptr = 0x%x; new_mem =0x%x", (uint32_t)ptr, (uint32_t)new_mem);
#endif
#endif

	if (ptr) obsd_userland_free(ptr);
	return new_mem;
}

#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))

void *obsd_userland_reallocarray(void *optr, size_t nmemb, size_t size)
{
	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
		nmemb > 0 && SIZE_MAX / nmemb < size) {
		errno = ENOMEM;
		return NULL;
	}
	return realloc(optr, size * nmemb);
}

/* ------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------ */
/* network functions, normally found in files under \lib\libc\net                       */
struct hostent  *gethostbyname(const char * name)
{
    OBSD_UNUSED_ARG(name);  /* avoid compiler warning */
    return NULL;   /* no DNS so far */
}

struct hostent * gethostbyaddr(const void *addr, socklen_t len, int af)
{
    OBSD_UNUSED_ARG(addr);  /* avoid compiler warning */
    OBSD_UNUSED_ARG(len);   /* avoid compiler warning */
    OBSD_UNUSED_ARG(af);   /* avoid compiler warning */
    return NULL;   /* no DNS so far */
}

struct netent   *getnetbyname(const char * name)
{
    OBSD_UNUSED_ARG(name);  /* avoid compiler warning */
    return NULL;   /* no DNS so far */
}

struct netent * getnetbyaddr(in_addr_t net, int type)
{
    OBSD_UNUSED_ARG(net);   /* avoid compiler warning */
    OBSD_UNUSED_ARG(type);  /* avoid compiler warning */
    return NULL;   /* no DNS so far */
}

int gethostname(char *name, size_t namelen)
{
    OBSD_UNUSED_ARG(name);     /* avoid compiler warning */
    OBSD_UNUSED_ARG(namelen);  /* avoid compiler warning */
    errno = EFAULT;
    return -1;
}
