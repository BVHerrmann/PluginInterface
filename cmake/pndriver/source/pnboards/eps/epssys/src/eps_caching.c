/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_caching.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Cache                                                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20009
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>                  /* Types / Prototypes / Fucns  */
#include <eps_rtos.h>                 /* Driver Interface            */
#include <eps_caching.h>              /* Own Header                  */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
#include <s12i_tlb_config.h>          /* Soc1 offset cached/uncached */
#endif

#ifdef EPS_USE_RTOS_ADONIS
#include <eps_adonis_cache_drv.h>     /* Adonis EPS Cache Driver     */

#define EPS_CACHE_IOCTL_MEM_CACHE_INVALIDATE    EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_INVALIDATE
#define EPS_CACHE_IOCTL_MEM_CACHE_WRITEBACK     EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_WRITEBACK
#define EPS_CACHE_IOCTL_MEM_CACHE_WB_INV        EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_WB_INV
#define EPS_CACHE_IOCTL_MEM_CMP_UNCACHED        EPS_ADONIS_CACHE_IOCTL_MEM_CMP_UNCACHED
#else
// Placeholders for other OS and other drivers, e.g. Linux
#define EPS_CACHE_IOCTL_MEM_CACHE_INVALIDATE    1
#define EPS_CACHE_IOCTL_MEM_CACHE_WRITEBACK     2
#define EPS_CACHE_IOCTL_MEM_CACHE_WB_INV        3
#define EPS_CACHE_IOCTL_MEM_CMP_UNCACHED        4
#endif

typedef struct
{
	LSA_BOOL    bInit;
	int32_t     fdCacheDriver;
} EPS_CACHE_TYPE, *EPS_CACHE_PTR_TYPE;

EPS_CACHE_TYPE g_EpsCache;
EPS_CACHE_PTR_TYPE g_pEpsCache = LSA_NULL;

/**
 * Installation function of eps cache
 * 
 * Inits global eps cache structure "g_EpsCache" and stores it address in "g_pEpsCache".
 * Installs eps adonis cache driver and opens it.
 * Upon successful installation file descriptor to cache driver is stored in global eps cache structure.
 */
LSA_VOID eps_cache_install(LSA_VOID)
{
	eps_memset(&g_EpsCache, 0, sizeof(g_EpsCache));
	g_pEpsCache = &g_EpsCache;

    #ifdef EPS_USE_RTOS_ADONIS
	{
	    LSA_INT retVal = eps_adonis_cache_drv_install();
        EPS_ASSERT(retVal == 0);
    
        g_pEpsCache->fdCacheDriver = open(EPS_ADONIS_CACHE_DRV_NAME, O_RDWR);
        EPS_ASSERT(g_pEpsCache->fdCacheDriver != -1);
	}
    #endif

	g_pEpsCache->bInit = LSA_TRUE;
}

/**
 * Deinstallation function of eps cache
 * 
 * Closes eps adonis cache driver and deinstalls it.
 * Sets pointer to global eps cache structure "g_pEpsCache" to LSA_NULL.
 */
LSA_VOID eps_cache_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsCache->bInit == LSA_TRUE);

    #ifdef EPS_USE_RTOS_ADONIS
	{
	    LSA_INT retVal;
        close(g_pEpsCache->fdCacheDriver);
        
        retVal = eps_adonis_cache_drv_uninstall();
        EPS_ASSERT(retVal == 0);
	}
    #endif

	g_pEpsCache->bInit = LSA_FALSE;

	g_pEpsCache = LSA_NULL;
}

/**
 * Does a cache synchronization (in usermode)
 * 
 * @param address   Begin of address to be synchronized
 * @param len       Length in bytes
 */
LSA_VOID eps_cache_sync(LSA_VOID * address, LSA_UINT32 len)
{
    EPS_ASSERT(g_pEpsCache != LSA_NULL);
    EPS_ASSERT(g_pEpsCache->fdCacheDriver != -1);

    #if defined (EPS_USE_RTOS_ADONIS) && (EPS_PLF == EPS_PLF_SOC_MIPS)
    {
        LSA_INT retVal = eps_adonis_cache_sync(address, len);
        EPS_ASSERT(retVal == 0);
    }
    #else
    {
        // no usermode synchronization supported
        eps_cache_wb(address, len);
    }
    #endif
}

/**
 * Does a cache invalidation
 * 
 * @param address   Begin of address to be invalidated
 * @param len       Length in bytes
 */
LSA_VOID eps_cache_inv(LSA_VOID * address, LSA_UINT32 len)
{
    LSA_INT retVal;
    EPS_CACHE_MEM_INFO_TYPE memattr;
    
    EPS_ASSERT(g_pEpsCache != LSA_NULL);
    EPS_ASSERT(g_pEpsCache->fdCacheDriver != -1);
    
    memattr.address = address;
    memattr.len     = len;

    retVal = ioctl(g_pEpsCache->fdCacheDriver, EPS_CACHE_IOCTL_MEM_CACHE_INVALIDATE, &memattr);        
    EPS_ASSERT(retVal == 0);
}

/**
 * Does a cache write back
 * 
 * @param address   Begin of address for cache write back
 * @param len       Length in bytes
 */
LSA_VOID eps_cache_wb(LSA_VOID * address, LSA_UINT32 len)
{
    LSA_INT retVal;
    EPS_CACHE_MEM_INFO_TYPE memattr;
    
    EPS_ASSERT(g_pEpsCache != LSA_NULL);
    EPS_ASSERT(g_pEpsCache->fdCacheDriver != -1);
    
    memattr.address = address;
    memattr.len     = len;

    retVal = ioctl(g_pEpsCache->fdCacheDriver, EPS_CACHE_IOCTL_MEM_CACHE_WRITEBACK, &memattr);        
    EPS_ASSERT(retVal == 0);
}

/**
 * Does a cache write back and invalidation
 * 
 * @param address   Begin of address for cache write back and invalidation
 * @param len       Length in bytes
 */
LSA_VOID eps_cache_wb_inv(LSA_VOID * address, LSA_UINT32 len)
{
    LSA_INT retVal;
    EPS_CACHE_MEM_INFO_TYPE memattr;
    
    EPS_ASSERT(g_pEpsCache != LSA_NULL);
    EPS_ASSERT(g_pEpsCache->fdCacheDriver != -1);
    
    memattr.address = address;
    memattr.len     = len;

    retVal = ioctl(g_pEpsCache->fdCacheDriver, EPS_CACHE_IOCTL_MEM_CACHE_WB_INV, &memattr);        
    EPS_ASSERT(retVal == 0);
}

/**
 * Maps given address uncached and compares cached with uncached memory
 * 
 * @param address   Begin of address to be compared with uncached memory
 * @param len       Length in bytes
 * @return memcmp return value
 */
LSA_INT eps_cache_cmp_uncached(LSA_VOID * address, LSA_UINT32 len)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsCache != LSA_NULL);
    EPS_ASSERT(g_pEpsCache->fdCacheDriver != -1);
    #if defined (EPS_USE_RTOS_ADONIS) && (EPS_PLF == EPS_PLF_SOC_MIPS)
	retVal = memcmp(address, (address + EPS_ADONIS_SDRAM_CACHED_UNCACHED_OFFSET), len);
    #else
	{
        EPS_CACHE_MEM_INFO_TYPE memattr;
        memattr.address = address;
        memattr.len     = len;
        retVal = ioctl(g_pEpsCache->fdCacheDriver, EPS_CACHE_IOCTL_MEM_CMP_UNCACHED, &memattr);
	}
    #endif
    
    return retVal;
}
