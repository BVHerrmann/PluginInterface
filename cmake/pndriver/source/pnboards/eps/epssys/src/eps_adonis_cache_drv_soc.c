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
/*  F i l e               &F: eps_adonis_cache_drv_soc.c                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Adonis Soc1 Cache driver                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20046
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/**********************************************************************
 * Includes
 *********************************************************************/

#include <psi_inc.h>

#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stropts.h>
#include <string.h>
#include <rt/fmq.h>
#include <signal.h>
#include <eps_sys.h>
#include <eps_plf.h>
#include <eps_adonis_cache_drv.h>
#include <eps_caching.h>
#include <adonis/interrupt.h>
#include <adonis/driver.h>
#include <sys/mman.h>
#include <memory_management.h>

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
// needed for cache handling on mips
#include <intrinsic.h>
#endif

/**********************************************************************
 * Defines
 *********************************************************************/

/// for MIPS 24kf 32 Byte cache per line
#define EPS_ADONIS_CACHE_DRV_CACHE_LINE_SIZE    32

/**********************************************************************
 * Prototypes
 *********************************************************************/

int32_t eps_adonis_cache_drv_open(adn_drv_info_t *eps_adonis_cache_drv_info, int32_t flag);
int32_t eps_adonis_cache_drv_close(adn_drv_info_t *eps_adonis_cache_drv_info);
int32_t eps_adonis_cache_drv_ioctl (adn_drv_info_t *eps_adonis_cache_drv_info, int32_t cmd, void *arg);

/**********************************************************************
 * Types / Variables
 *********************************************************************/
/* structure with driver functions */
adn_drv_func_t eps_adonis_cache_drv_funcs = {
        eps_adonis_cache_drv_open,  /* open  */
        NULL,                       /* read  */
        NULL,                       /* write */
        eps_adonis_cache_drv_close, /* close */
        eps_adonis_cache_drv_ioctl, /* ioctl */
        NULL                        /* mmap  */
    };

/**
 * Opens the eps adonis ipc driver.
 *
 * @param eps_adonis_cache_drv_info Pointer to the driver info object, supplied by the IO system
 * @param oflag                     Open flags (Unused since no read/write functionality in driver)
 *
 * @return ADN_OK 
 */
int32_t eps_adonis_cache_drv_open(adn_drv_info_t *eps_adonis_cache_drv_info, int32_t oflag)
{
    /* Only allow one driver instance at one time */
    if (eps_adonis_cache_drv_info->state == ADN_DRV_STATE_BUSY)
    {
        return ADN_ERROR;
    }
    
    /* mark interface as busy */
    eps_adonis_cache_drv_info->state = ADN_DRV_STATE_BUSY;
    
    return ADN_OK;
}

/**
 * Closes the eps adonis cache driver.
 *
 * @param eps_adonis_cache_drv_info Pointer to the driver info object, supplied by the IO system
 *
 * @return ADN_OK
 */
int32_t eps_adonis_cache_drv_close(adn_drv_info_t *eps_adonis_cache_drv_info)
{
    // Check if interface open
    if (eps_adonis_cache_drv_info->state == ADN_DRV_STATE_FREE)
    {
        return ADN_ERROR;
    }
    
    // Mark driver as free
    eps_adonis_cache_drv_info->state = ADN_DRV_STATE_FREE;
    
    return ADN_OK;
}


/**
 * Does cache write back on soc1
 * 
 * @param memattr -> Includes start address and size
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_cache_drv_soc1_wb(EPS_CACHE_MEM_INFO_PTR_TYPE memattr)
{
    /// only in kernel mode!
    uintptr_t cache_start       = (uintptr_t)memattr->address; // cache start is already cache aligned!
    const uintptr_t cache_end   = cache_start + (uintptr_t)memattr->len;
    const uintptr_t cache_step  = EPS_ADONIS_CACHE_DRV_CACHE_LINE_SIZE;
    
    while(cache_start < cache_end)
    {
        __CACHE((CACHE_HIT_WRITE_BACK|CACHE_S), cache_start);
        __CACHE((CACHE_HIT_WRITE_BACK|CACHE_D), cache_start);
        cache_start += cache_step;
    }
    
    // Memory barrier
    asm volatile ("sync" ::: "memory");
    
    return ADN_OK;
}

/**
 * Does cache invalidate on soc1
 * 
 * @param memattr -> Includes start address and size
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_cache_drv_soc1_inv(EPS_CACHE_MEM_INFO_PTR_TYPE memattr)
{
    /// only in kernel mode!
    uintptr_t cache_start       = (uintptr_t)memattr->address; // cache start is already cache aligned!
    const uintptr_t cache_end   = cache_start + (uintptr_t)memattr->len;
    const uintptr_t cache_step  = EPS_ADONIS_CACHE_DRV_CACHE_LINE_SIZE;
    
    while(cache_start < cache_end)
    {
        __CACHE((CACHE_HIT_INV|CACHE_S), cache_start);
        __CACHE((CACHE_HIT_INV|CACHE_D), cache_start);
        cache_start += cache_step;
    }
    
    // Memory barrier
    asm volatile ("sync" ::: "memory");
    
    return ADN_OK;
}

/**
 * Does cache write back and invalidate on soc1
 * 
 * @param memattr -> Includes start address and size
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_cache_drv_soc1_wb_inv(EPS_CACHE_MEM_INFO_PTR_TYPE memattr)
{
    /// only in kernel mode!
    uintptr_t cache_start       = (uintptr_t)memattr->address; // cache start is already cache aligned!
    const uintptr_t cache_end   = cache_start + (uintptr_t)memattr->len;
    const uintptr_t cache_step  = EPS_ADONIS_CACHE_DRV_CACHE_LINE_SIZE;
    
    while(cache_start < cache_end)
    {
        __CACHE((CACHE_HIT_WRITE_BACK_INV|CACHE_S), cache_start);
        __CACHE((CACHE_HIT_WRITE_BACK_INV|CACHE_D), cache_start);
        cache_start += cache_step;
    }
    
    // Memory barrier
    asm volatile ("sync" ::: "memory");
    
    return ADN_OK;
}


/**
 * Ioctl-Function for eps adonis cache driver.
 *
 * @param eps_adonis_cache_drv_info  Pointer to the driver info object, supplied by the IO system
 * @param cmd                        Command for IO control
 * @param arg                        Generic pointer to argument for IO control command
 *
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
int32_t eps_adonis_cache_drv_ioctl (adn_drv_info_t *eps_adonis_cache_drv_info, int32_t cmd, void *arg)
{
    int32_t ret_val = ADN_ERROR;
    
    EPS_ASSERT(arg != LSA_NULL);
    
    switch (cmd)
    {
        case EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_INVALIDATE:
        {
            ret_val = eps_adonis_cache_drv_soc1_inv((EPS_CACHE_MEM_INFO_PTR_TYPE)arg);
            break;
        }
        case EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_WRITEBACK:
        {
            ret_val = eps_adonis_cache_drv_soc1_wb((EPS_CACHE_MEM_INFO_PTR_TYPE)arg);
            break;
        }
        case EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_WB_INV:
        {
            ret_val = eps_adonis_cache_drv_soc1_wb_inv((EPS_CACHE_MEM_INFO_PTR_TYPE)arg);
            break;
        }
        case EPS_ADONIS_CACHE_IOCTL_MEM_CMP_UNCACHED:
        {
            EPS_CACHE_MEM_INFO_PTR_TYPE req_ptr    = (EPS_CACHE_MEM_INFO_PTR_TYPE)(arg);
            uintptr_t cached_virt_addr  = (uintptr_t)(req_ptr->address);
            void *uncached_virt_addr    = NULL;
            uintptr_t physical_addr     = adn_mem_mmu_translate(cached_virt_addr);
            
            EPS_ASSERT(!(0xFFFFFFFF == physical_addr || (0 == physical_addr)));
            
            /// map it to an uncached area
            uncached_virt_addr = adn_mem_mmap_add(physical_addr, NULL, req_ptr->len , (PROT_READ | PROT_WRITE), MAP_SHARED, ADN_MEM_UNCACHED);
            
            /// Compare cached and uncached memory
            ret_val = memcmp(req_ptr->address, uncached_virt_addr, req_ptr->len);
            
            /// remove mapping
            adn_mem_mmap_remove(uncached_virt_addr, req_ptr->len);
            break;
        }
        default:
        {
            break;
        }
    }
    return ret_val;
}

/**
 * Does a cache synchronization (in usermode)
 * 
 * @param address   Begin of address to be synchronized
 * @param len       Length in bytes
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
LSA_INT32 eps_adonis_cache_sync(LSA_VOID * address, LSA_UINT32 len)
{
  /// MIPS SYNC I Sequence
  /// can be done in user mode!
  __asm__ __volatile__ (
    "move $12, %0"         "\n\t" /* Start address of new instruction stream */
    "move $13, %1"         "\n\t" /* Size, in bytes, of new instruction stream */
    "addu $13, $12, $13"   "\n\t" /* Calculate end address + 1 */
    "rdhwr $10, $1"        "\n\t" /* Get step size for SYNCI from new */
                                  /* Release 2 instruction */
    "beq $10, $0, 20f"     "\n\t" /* If no caches require synchronization, */
                                  /* branch around */
    "10:  synci 0($12)"    "\n\t" /* Synchronize all caches around address */
    "sltu $11, $12, $13"   "\n\t" /* Compare current with end address */
    "addu $12, $12, $10"   "\n\t" /* Add step size in delay slot */
    "bne $11, $0, 10b"     "\n\t" /* Branch if more to do */
    "sync"                 "\n\t" /* Clear memory hazards */
    "move $10, $31"        "\n\t"
    "jal 15f"              "\n\t"
    "move $31, $10"        "\n\t"
    "j 20f"                "\n\t"
    "15: jr.hb $31"        "\n\t"
    "20:"
    : : "r" ((LSA_UINT32)address),
        "r" ((LSA_UINT32)(len))
  );
  return ADN_OK;
}

/**
 * Installation function of eps adonis cache driver
 * Registers driver functions to adonis
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
LSA_INT32 eps_adonis_cache_drv_install(LSA_VOID)
{
   return (adn_drv_register(EPS_ADONIS_CACHE_DRV_NAME, &eps_adonis_cache_drv_funcs));
}

/**
 * Deinstallation function of eps adonis cache driver
 * Unregisters driver functions to adonis
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
LSA_INT32 eps_adonis_cache_drv_uninstall(LSA_VOID)
{
   return (adn_drv_unregister(EPS_ADONIS_CACHE_DRV_NAME));
}

