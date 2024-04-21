/**
 * @file        obsd_platform_osal.c
 *
 * @brief       (OS) (A)daption (L)ayer main file. Contains functions that needs to be adapted to the target's OS
 * @details     Functions for usage from the stack port level / platform independent code part (ObsdPort).
 *
 * @author      Hendrik Gerlach 
 *
 * @version     V0.16
 * @date        created: 06.11.2012
 */

#include <tcip_inc.h>
#include <obsd_kernel_BSDStackPort.h>
#include <obsd_platform_osal.h>

#define TCIP_MODULE_ID 3002

/* ================================= memory management ========================================== */

/* comment out the declarations below to avoid the warning from compiler "i586-poky-linux-gcc": */
/*
    Unused declaration of function 'OSAL_kern_free'
    Unused declaration of function 'OSAL_kern_malloc'
    Unused declaration of function 'OSAL_km_alloc'
    Unused declaration of function 'OSAL_km_free'
*/

/* (OSAL_)km_alloc expects pages aligned to OBSD's PAGE_SIZE */
//void * OSAL_km_alloc(unsigned int Size);

/* must fit to OSAL_km_alloc */
//void OSAL_km_free( void *pMem, unsigned int Size);

//void * OSAL_kern_malloc(unsigned long size, int type, int flags);

//void OSAL_kern_free(void *addr, int type);

void *obsd_kernel_memcpy(void *s1, const void *s2, unsigned int n)
{
	TCIP_MEMCPY(s1, (void *)s2, n);
	return s1;
}

void *obsd_kernel_memset(void *dst0, int c0, unsigned int length)
{
	TCIP_MEMSET(dst0, c0, length);
	return dst0;
}
/* ================================= char IO ==================================================== */

void OSAL_kernel_putchar(int c)
{
	LSA_UNUSED_ARG(c);
	/* OBSD_ITGR */
}



/* ================================= misc ======================================================= */

void OSAL_event_panic_reboot(int howto)
{
	LSA_UNUSED_ARG(howto);
	TCIP_FATAL(); /* OBSD_ITGR */
}

void OSAL_NotImplemented(const char * pchFuncName, const char * pchFileName)
{
	LSA_UNUSED_ARG(pchFuncName);
	LSA_UNUSED_ARG(pchFileName);
	/* OBSD_ITGR */
}

#if (defined(OBSD_TCIP_INFO_INTF))

/************************************************************************************************************************************/
/* Task 2609759 - TCIP Info Interface 				
*   
*	Establishes info interface for memory statistics for malloc & free (OSAL_kern_malloc & OSAL_kern_free)	
*   Usage:
*			obsd_osal_kern_mem_stat_init()		- call once to init the internal mem statistic
*			obsd_osal_kern_malloc_stat()		- call in OSAL_kern_malloc() and supply with pmem, size and type of alloc memory
*			obsd_osal_kern_free_stat			- call in OSAL_kern_free() and supply with pmem - memory to be freed
*
*			obsd_osal_kern_mem_stat_type()		- get "s_localmem_stat" struct of an spezific type of allocated memory
*			obsd_osal_kern_mem_stat_item_get()	- get "s_localmem_stat" struct members.  
*
*/
#define	OSAL_M_LAST			146			/* must fit to M_LAST Flag in OpenBSD */
#define	M_SHADOW_CNT	2000			/* maximum internal mem management structs */

/* internal mem management struct */
struct s_mem_shadow
{
	void * pmem;			/* pointer to allocated mem */
	unsigned long	nSize;	/* size of alloc mem */
	unsigned long	nType;	/* type of alloc mem like defined in „sys/obsd_kernel_malloc.h" : INITKMEMNAMES  */
};

/* malloc statistic struct - one per mem type */
struct	s_localmem_stat
{
	long	nAlloc;					/* malloc() counts */					
	long	nFree;					/* free() counts */
	long	nErr;					/* malloc() error count */
	long	mem;					/* total size of allocated memory of the specific type */
	long	mem_hiWat;				/* total malloc size watermark */
	long	mem_maxunit_alloc;		/* biggest malloc() size */
	long	nErrIntern;				/* internal error - M_SHADOW_CNT too small - has to be adjuste/increased */
};

/* internal shadow memory mamagement and memory statistic - put it together */
struct s_localmem
{
	struct	s_localmem_stat		ar_localmen[OSAL_M_LAST];
	struct	s_mem_shadow		ar_mem_shadow[M_SHADOW_CNT];
};

struct	s_localmem		localmen_status;			/* THE statistic instance */

/* apply an internal shadow mem.managm. item */
unsigned long	obsd_malloc_shadow(void * pmem, unsigned long size, unsigned long type)
{
	int i;

	for (i = 0; i<M_SHADOW_CNT; i++)
	{
		if (localmen_status.ar_mem_shadow[i].nSize == 0)
		{
			localmen_status.ar_mem_shadow[i].pmem = pmem;
			localmen_status.ar_mem_shadow[i].nSize = size;
			localmen_status.ar_mem_shadow[i].nType = type;
			return size;
		}
	}

	return 0;
}
/* remove an internal shadow mem.managm. item */
unsigned long	obsd_free_shadow(void * pmem, unsigned long * ptype_shadow)
{
	unsigned long size;
	int i;

	/* no "ptype_shadow" null ptr sanity check */

	for (i = 0; i<M_SHADOW_CNT; i++)
	{
		if (localmen_status.ar_mem_shadow[i].pmem == pmem)
		{
			localmen_status.ar_mem_shadow[i].pmem = LSA_NULL;
			size = localmen_status.ar_mem_shadow[i].nSize;
			*ptype_shadow = localmen_status.ar_mem_shadow[i].nType;
			localmen_status.ar_mem_shadow[i].nSize = 0;
			localmen_status.ar_mem_shadow[i].nType = 0;
			return size;
		}
	}

	return 0;
}

/* Init
*
*/
extern void obsd_osal_kern_mem_stat_init(void)
{
	int i;

	for (i = 0; i<OSAL_M_LAST; i++)						// M_LAST
	{
		localmen_status.ar_localmen[i].nAlloc = 0;
		localmen_status.ar_localmen[i].nFree = 0;
		localmen_status.ar_localmen[i].nErr = 0;
		localmen_status.ar_localmen[i].mem = 0;
		localmen_status.ar_localmen[i].mem_hiWat = 0;
		localmen_status.ar_localmen[i].mem_maxunit_alloc = 0;
		localmen_status.ar_localmen[i].nErrIntern = 0;

	}
	for (i = 0; i<M_SHADOW_CNT; i++)
	{
		localmen_status.ar_mem_shadow[i].pmem = LSA_NULL;
		localmen_status.ar_mem_shadow[i].nSize = 0;
		localmen_status.ar_mem_shadow[i].nType = 0;
	}
}

/* Call in OSAL_kern_malloc() and supply with pmem, size and type of alloc memory
*  
*	void * pmem			- Pointer to allocated memory from malloc();
*	unsigned long size	- Size of allocated memory from malloc()
*	int type			- Type of allocated memory from malloc()
*/
extern void obsd_osal_kern_malloc_stat(void * pmem, unsigned long size, int type)
{
	if ((type >= OSAL_M_LAST) || (type < 0))			// M_LAST
		return;

	if (LSA_NULL != pmem)
	{
		if (obsd_malloc_shadow(pmem, size, type) == size)
		{
			localmen_status.ar_localmen[type].nAlloc++;
			localmen_status.ar_localmen[type].mem += size;
			if (localmen_status.ar_localmen[type].mem_hiWat < localmen_status.ar_localmen[type].mem)
				localmen_status.ar_localmen[type].mem_hiWat = localmen_status.ar_localmen[type].mem;
			if (localmen_status.ar_localmen[type].mem_maxunit_alloc < size)
				localmen_status.ar_localmen[type].mem_maxunit_alloc = size;
		}
		else
		{
			localmen_status.ar_localmen[type].nErrIntern++;
		}
	}
	else
	{
		localmen_status.ar_localmen[type].nErr++;
	}
}

/* Call in OSAL_kern_free() and supply with pmem off memory to be freed
*  
*	void * pmem			- Pointer to memory to be freed();
*/
extern void obsd_osal_kern_free_stat(void * pmem)
{
	unsigned long size_shadow;
	unsigned long type_shadow;

	if ((size_shadow = obsd_free_shadow(pmem, &type_shadow)) != 0)
	{
		localmen_status.ar_localmen[type_shadow].nFree++;
		localmen_status.ar_localmen[type_shadow].mem -= size_shadow;
	}
}

/* Get "s_localmem_stat" struct of an specific type of allocated memory
*	This struct element holds the status info of the specific type of allocated memory
*
*  int type - Type of the memory
*/
extern void * obsd_osal_kern_mem_stat_type(int type)
{
	if (localmen_status.ar_localmen[type].nAlloc || localmen_status.ar_localmen[type].nErr)
		return((void *)&localmen_status.ar_localmen[type]);

	return(LSA_NULL);
}

/* Get "s_localmem_stat" struct members => mem status info.
*
*	void * pp	- "s_localmem_stat" struct
*	int item	- member of "s_localmem_stat" struct - see code
*/
extern unsigned long obsd_osal_kern_mem_stat_item_get(void * pp, int item)
{
	struct	s_localmem_stat * plms = (struct s_localmem_stat *)pp;

	switch (item)
	{
	case 1:
		return plms->nAlloc;
	case 2:
		return plms->nFree;
	case 3:
		return plms->nErr;
	case 4:
		return plms->mem;
	case 5:
		return plms->mem_hiWat;
	case 6:
		return plms->mem_maxunit_alloc;
	case 7:
		return plms->nErrIntern;
	default:
		return -2;
	}
	return -1;
}
#endif /* OBSD_TCIP_INFO_INTF */

/************************************************************************************************************************************/

