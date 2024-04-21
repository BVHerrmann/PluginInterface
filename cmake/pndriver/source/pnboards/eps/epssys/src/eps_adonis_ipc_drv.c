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
/*  F i l e               &F: eps_adonis_ipc_drv.c                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Adonis Inter Processor Communication driver                          */
/*                                                                           */
/*****************************************************************************/

/*********************************************************************
   Includes
*********************************************************************/

#include <psi_inc.h>

#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <adonis/interrupt.h>
#include <adonis/driver.h>
#include <stropts.h>
#include <string.h>
#include <rt/fmq.h>
#include <signal.h>
#include <eps_sys.h>
#include <eps_adonis_ipc_drv.h>
#include <eps_ipc_drv_if.h>
#include <eps_register.h>
#include <eps_plf.h>
#include <eps_tasks.h>
#include <adonis/pci.h>
#include <sys/mman.h>
#include <memory_management.h>
#include <adonis/interrupt.h>

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*********************************************************************
   Defines
*********************************************************************/
/* This define represents the size of the memory mapped area for the notify_info */
#define EPS_IPC_DRV_MMAP_SIZE_NOTIFY_INFO 0x1000

/* Set defines controlling the driver features */
#if (EPS_PLF == EPS_PLF_SOC_MIPS)
	#define EPS_IPC_DRV_MIPS_SOC1 /* Driver Variant Soc1 onboard      */
#elif (EPS_PLF == EPS_PLF_ERTEC400_ARM9)
	#define EPS_IPC_DRV_ERTEC400  /* Driver Variant Ertec400 onboard  */
#elif (EPS_PLF == EPS_PLF_ERTEC200_ARM9)
	#define EPS_IPC_DRV_ERTEC200  /* Driver Variant Ertec200 onboard  */
#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)
	#define EPS_IPC_DRV_ERTEC200P /* Driver Variant Ertec200P onboard */
#elif (EPS_PLF == EPS_PLF_PNIP_A53)
    #define EPS_IPC_DRV_HERA /* Driver Variant Hera onboard */
#elif (EPS_PLF == EPS_PLF_ADONIS_X86)
	#define EPS_IPC_DRV_X86       /* Driver Variant X86               */
#endif

#if defined EPS_IPC_DRV_MIPS_SOC1 || defined EPS_IPC_DRV_X86 || defined EPS_IPC_DRV_HERA
#define EPS_IPC_DRV_SUPPORTED
#endif

#if defined EPS_IPC_DRV_MIPS_SOC1 || defined EPS_IPC_DRV_X86
#define EPS_IPC_IRQ_VECTOR_SUPPORTED // allocate irq vector from irq pool and use it for ipc interrupts
#endif

#ifdef EPS_IPC_DRV_X86
	#define EPS_IPC_DRV_MAX_IPC_INTERRUPTS 10 /*EPS_IPC_DRV_MAX_IPC_INTERRUPTS <= EPS_IPC_MAX_INSTANCES*/
#else
	#define EPS_IPC_DRV_MAX_IPC_INTERRUPTS 1
#endif

/*********************************************************************
   Types / Variables
*********************************************************************/

typedef struct
{
	LSA_BOOL bUsed;
	LSA_BOOL bIntEnabled;
	EPS_IPC_DRV_NOTIFY_INFO_TYPE sNotifyInfo;
	EPS_IPC_DRV_INT_INFO_TYPE sIntInfo;
	LSA_UINT32 uIntCount;
	#ifdef EPS_IPC_IRQ_VECTOR_SUPPORTED 
	adn_irq_vector_t irq_vector;
	#endif
} EPS_ADONIS_IPC_DRV_INT_TYPE, *EPS_ADONIS_IPC_DRV_INT_PTR_TYPE;

typedef struct
{
	LSA_BOOL   bInit;
	LSA_UINT16 uCntInst;
	EPS_ADONIS_IPC_DRV_INT_TYPE IntData[EPS_IPC_DRV_MAX_IPC_INTERRUPTS];
} EPS_ADONIS_IPC_DRV_TYPE, *EPS_ADONIS_IPC_DRV_PTR_TYPE;

EPS_ADONIS_IPC_DRV_TYPE g_EpsAdonisIpcDrv = {0};
EPS_ADONIS_IPC_DRV_PTR_TYPE g_pEpsAdonisIpcDrv = LSA_NULL;

/*********************************************************************
   Implementation
*********************************************************************/

#ifdef EPS_IPC_DRV_SUPPORTED

/**
 * Interrupt service routine which is called when ipc SW interrupt was triggered
 * Sends signal to wake up ipc event thread (@see eps_tasks_thread_proc_internal).
 * After signal for interrupt has arrived ipc isr receive thread routine (@see eps_ipc_isr_rx_rcv_thread) is executed. 
 * 
 * @param param unused
 * @param ptr pointer to ipc driver interrupt info
 */
void eps_adonis_ipc_drv_int_handler (int32_t param, void *ptr)
{
    LSA_UNUSED_ARG(param);
    
	EPS_ADONIS_IPC_DRV_INT_PTR_TYPE pInt = (EPS_ADONIS_IPC_DRV_INT_PTR_TYPE)ptr;

	EPS_ASSERT(pInt != LSA_NULL);

	pInt->uIntCount++;

	eps_tasks_trigger_event_thread(pInt->sIntInfo.hThread);

#if defined(EPS_IPC_DRV_HERA)
	// clear interrupt in scrb register INT_SW_CLEAR_REG0 right after receiving it, to allow new ipc interrupts to be send!
	EPS_REG32_WRITE(((LSA_UINT8*)EPS_IPC_HERA_SCRB_PHY_ADDR + EPS_IPC_HERA_SCRB_CLR_IRQ_REG_OFFSET), EPS_IPC_HERA_BOARD_LOCAL_IRQ_VAL);
#endif
	
	return;
}

/**
 * Searches for a free ipc driver notification info instance and allocates it.
 * 
 * Requests an interrupt vector from pool.
 * Stores handle to interrupt vector in pInfo->hIntHandle.
 * Stores 32 bit local address of interrupt vector in pInfo->uAddress.
 * Stores 32 bit translated pci address in pInfo->uPciAddress.
 * Stores trigger value of interrupt vector in pInfo->uValue.
 * 
 * @param pInfo pointer to ipc notification info instance
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
static int32_t eps_adonis_ipc_drv_alloc_local_notify_info(EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE pInfo)
{
#ifdef EPS_IPC_IRQ_VECTOR_SUPPORTED
	LSA_UINT32 allocated_vector_nr;
#endif
	EPS_ADONIS_IPC_DRV_INT_PTR_TYPE pInt;
	LSA_UINT32 i;

	EPS_ASSERT(pInfo != LSA_NULL);

	for(i=0; i<EPS_IPC_DRV_MAX_IPC_INTERRUPTS; i++)
	{
		pInt = &g_pEpsAdonisIpcDrv->IntData[i];
		if(pInt->bUsed == LSA_FALSE)
		{
			pInt->bUsed = LSA_TRUE;
			pInfo->hIntHandle = i;
			break;
		}
		pInt = LSA_NULL;
	}

	if(pInt == LSA_NULL)
	{
		return ADN_ERROR;
	}
#ifdef EPS_IPC_IRQ_VECTOR_SUPPORTED 
	// request interrupt vectors from pool
	allocated_vector_nr = adn_irq_request_vector(&pInt->irq_vector, 1, ADN_IRQ_POOL_ALLOC_RANDOM);
	if (allocated_vector_nr != 1) 
	{
		return ADN_ERROR;
	}

	pInfo->uPhyAddress 	= pInt->irq_vector.base_address_low; // physical irq address: pci address for soc1 / local address for x32
	pInfo->uValue       = pInt->irq_vector.trigger_value;
#elif defined EPS_IPC_DRV_HERA
    pInfo->uPhyAddress  = (EPS_IPC_HERA_SCRB_PHY_ADDR + EPS_IPC_HERA_SCRB_SET_IRQ_REG_OFFSET); // hera SCRB Register Offset INT_SW_SET_REG0 for setting INT_SW_0-31
    pInfo->uValue       = EPS_IPC_HERA_BOARD_LOCAL_IRQ_VAL;
#endif
	pInfo->uAddress 	= pInfo->uPhyAddress;

	pInt->sNotifyInfo = *pInfo;

	return ADN_OK;
}

/**
 * Free allocated free ipc driver notification info instance and interrupt vector.
 * 
 * @param pInfo pointer to ipc notification info
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_ipc_drv_free_local_notify_info(EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE pInfo)
{
	EPS_ADONIS_IPC_DRV_INT_PTR_TYPE pInt;

	EPS_ASSERT(pInfo != LSA_NULL);
	EPS_ASSERT(pInfo->hIntHandle < EPS_IPC_DRV_MAX_IPC_INTERRUPTS);
	
	pInt = &g_pEpsAdonisIpcDrv->IntData[pInfo->hIntHandle];

	EPS_ASSERT(pInt->bIntEnabled == LSA_FALSE);
	EPS_ASSERT(pInt->bUsed == LSA_TRUE);

#ifdef EPS_IPC_IRQ_VECTOR_SUPPORTED
    adn_irq_free_vector(&pInt->irq_vector, 1);
#elif defined EPS_IPC_DRV_HERA
    // for hera we don't use irq vectors, we use one of the scrb interrupts for ipc instead.
#endif

	pInt->bUsed = LSA_FALSE;

	return ADN_OK;
}

/**
 * Free allocated free ipc driver notification info instance and interrupt vector.
 * 
 * @param pInfo pointer to ipc notification info
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_ipc_drv_free_remote_notify_info(EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE pInfo)
{
    EPS_ASSERT(pInfo != LSA_NULL);
    EPS_ASSERT(pInfo->hIntHandle < EPS_IPC_DRV_MAX_IPC_INTERRUPTS);
#ifdef EPS_IPC_DRV_HERA
    // hera uses scrb interrupts for ipc, no address translation necassary host and board see scrb registers.
#else
    adn_mem_mmap_remove((void*)pInfo->uMmapAddress, EPS_IPC_DRV_MMAP_SIZE_NOTIFY_INFO);
#endif

    return ADN_OK;
}

/**
 * Adds logical address from uPciAddress in remote notification info to adonis memory mapping
 * and stores virtual address into uAddress in remote notification info.
 * 
 * @param pInfo pointer to ipc remote notification info
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_ipc_drv_translate_notify_info(EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE pInfo)
{
#ifndef EPS_IPC_DRV_HERA
	uint32_t physical_addr;
#endif
	int32_t ret_val = ADN_OK;

	EPS_ASSERT(pInfo != LSA_NULL);

#ifdef EPS_IPC_DRV_HERA
    // hera uses scrb interrupts for ipc
	EPS_ASSERT(pInfo->uPhyAddress == (EPS_IPC_HERA_SCRB_PHY_ADDR + EPS_IPC_HERA_SCRB_SET_IRQ_REG_OFFSET));
	pInfo->uAddress = pInfo->uPhyAddress;
#else
	physical_addr = pInfo->uPhyAddress; // physical irq address: pci address for soc1 / local address for x32 / SCRB Reg INT_SW_SET_REG0 for hera
	// map physical address to virtual
	pInfo->uAddress = (LSA_UINT32)(uint32_t *)adn_mem_mmap_add(physical_addr, (void *)0, EPS_IPC_DRV_MMAP_SIZE_NOTIFY_INFO, PROT_READ|PROT_WRITE, MAP_SHARED, ADN_MEM_UNCACHED);
	pInfo->uMmapAddress = pInfo->uAddress; // we have to save this address ti make an unmap later
#endif
	
	if(pInfo->uAddress == 0)
	{
	    ret_val = ADN_ERROR;
	}

	return ret_val;
}


/**
 * Registers ipc interrupt in adonis and enables it.
 * 
 * @param pIntInfo pointer to ipc interrupt info
 * 
 * @return ADN_OK
 */
static int32_t eps_adonis_ipc_drv_local_int_enable(EPS_IPC_DRV_INT_INFO_PTR_TYPE pIntInfo)
{
	EPS_ADONIS_IPC_DRV_INT_PTR_TYPE pInt;
	int32_t ret_val = ADN_OK;

	EPS_ASSERT(pIntInfo != LSA_NULL);
	EPS_ASSERT(pIntInfo->hIntHandle < EPS_IPC_DRV_MAX_IPC_INTERRUPTS);

	pInt = &g_pEpsAdonisIpcDrv->IntData[pIntInfo->hIntHandle];

	EPS_ASSERT(pInt->bUsed == LSA_TRUE);
	EPS_ASSERT(pInt->bIntEnabled == LSA_FALSE);
	
	pInt->sIntInfo = *pIntInfo;
#ifdef EPS_IPC_IRQ_VECTOR_SUPPORTED
	ret_val = adn_irq_register(pInt->irq_vector.local_int_id, &eps_adonis_ipc_drv_int_handler, 0, (LSA_VOID*)pInt);
#elif defined EPS_IPC_DRV_HERA
	ret_val = adn_irq_register(EPS_IPC_HERA_BOARD_LOCAL_IRQ_NR, &eps_adonis_ipc_drv_int_handler, 0, (LSA_VOID*)pInt);
	ADN_IRQ_ENABLE(EPS_IPC_HERA_BOARD_LOCAL_IRQ_NR);
	adn_irq_set_core_affinity(EPS_IPC_HERA_BOARD_LOCAL_IRQ_NR, 1);
#endif
	EPS_ASSERT(ret_val == ADN_OK);


	pInt->bIntEnabled = LSA_TRUE;

	return ret_val;
}

/**
 * Disables ipc interrupt and unregisters it in adonis.
 * 
 * @param pIntInfo pointer to ipc interrupt info
 *
 * @return ADN_OK
 */
static int32_t eps_adonis_ipc_drv_local_int_disable(EPS_IPC_DRV_INT_INFO_PTR_TYPE pIntInfo)
{
	EPS_ADONIS_IPC_DRV_INT_PTR_TYPE pInt;
	int32_t ret_val = ADN_OK;

	EPS_ASSERT(pIntInfo != LSA_NULL);
	EPS_ASSERT(pIntInfo->hIntHandle < EPS_IPC_DRV_MAX_IPC_INTERRUPTS);

	pInt = &g_pEpsAdonisIpcDrv->IntData[pIntInfo->hIntHandle];

	EPS_ASSERT(pInt->bUsed == LSA_TRUE);
	EPS_ASSERT(pInt->bIntEnabled == LSA_TRUE);

#ifdef EPS_IPC_IRQ_VECTOR_SUPPORTED
	ret_val = adn_irq_unregister(pInt->irq_vector.local_int_id);
#elif defined EPS_IPC_DRV_HERA
	ADN_IRQ_DISABLE(EPS_IPC_HERA_BOARD_LOCAL_IRQ_NR);
    ret_val = adn_irq_unregister(EPS_IPC_HERA_BOARD_LOCAL_IRQ_NR);
#endif
	EPS_ASSERT(ret_val == ADN_OK);

	pInt->bIntEnabled = LSA_FALSE;

	return ret_val;
}

/**
 * Ioctl-Function for eps adonis ipc driver.
 *
 * @param pDrvInfo  Pointer to the driver info object, supplied by the IO system
 * @param cmd       Command for IO control
 * @param arg       Generic pointer to argument for IO control command
 *
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
extern int32_t eps_adonis_ipc_drv_ioctl (adn_drv_info_t* pDrvInfo, int cmd, void* arg)
{
	int32_t ret_val = ADN_ERROR;

	EPS_ASSERT(g_pEpsAdonisIpcDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsAdonisIpcDrv->bInit == LSA_TRUE);

	LSA_UNUSED_ARG(pDrvInfo);

	switch(cmd)
	{
		case EPS_ADONIS_IPC_DRV_ALLOC_LOCAL_NOTIFY_INFO:
			ret_val = eps_adonis_ipc_drv_alloc_local_notify_info((EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE)arg);
			break;
		case EPS_ADONIS_IPC_DRV_FREE_LOCAL_NOTIFY_INFO:
			ret_val = eps_adonis_ipc_drv_free_local_notify_info((EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE)arg);
			break;
		case EPS_ADONIS_IPC_DRV_TRANSLATE_NOTIFY_INFO:
			ret_val = eps_adonis_ipc_drv_translate_notify_info((EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE)arg);
			break;
		case EPS_ADONIS_IPC_DRV_LOCAL_INT_ENABLE:
			ret_val = eps_adonis_ipc_drv_local_int_enable((EPS_IPC_DRV_INT_INFO_PTR_TYPE)arg);
			break;
		case EPS_ADONIS_IPC_DRV_LOCAL_INT_DISABLE:
			ret_val = eps_adonis_ipc_drv_local_int_disable((EPS_IPC_DRV_INT_INFO_PTR_TYPE)arg);
			break;
        case EPS_ADONIS_IPC_DRV_FREE_REMOTE_NOTIFY_INFO:
            ret_val = eps_adonis_ipc_drv_free_remote_notify_info((EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE)arg);
            break;
		default:
			break;
	}

	return ret_val;
}

/**
 * Opens the eps adonis ipc driver.
 *
 * @param pDrvInfo Pointer to the driver info object, supplied by the IO system
 * @param oflag    Open flags (Unused since no read/write functionality in driver)
 *
 * @return ADN_OK 
 */
extern int32_t eps_adonis_ipc_drv_open (adn_drv_info_t* pDrvInfo, int oflag)
{
	LSA_UNUSED_ARG(oflag);

	EPS_ASSERT(g_pEpsAdonisIpcDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsAdonisIpcDrv->bInit == LSA_TRUE);
	
	g_pEpsAdonisIpcDrv->uCntInst++;

	pDrvInfo->state = ADN_DRV_STATE_BUSY;
	
	/* nothing to do */
	return ADN_OK;
}

/**
 * Closes the eps adonis ipc driver.
 *
 * @param pDrvInfo Pointer to the driver info object, supplied by the IO system
 *
 * @return ADN_OK
 */
extern int32_t eps_adonis_ipc_drv_close (adn_drv_info_t* pDrvInfo)
{
	EPS_ASSERT(g_pEpsAdonisIpcDrv->uCntInst > 0);
	/// Last close?
	if (--g_pEpsAdonisIpcDrv->uCntInst == 0)
	{
		pDrvInfo->state = ADN_DRV_STATE_FREE;
	}
	
	/* nothing to do */
	return ADN_OK;
}

#endif

/**
 * Installation function of eps adonis inter processor communication driver
 * Registers driver functions to adonis
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
LSA_INT32 eps_adonis_ipc_drv_install(void)
{
#ifdef EPS_IPC_DRV_SUPPORTED

	int32_t ret_val = ADN_OK;

	memset(&g_EpsAdonisIpcDrv, 0, sizeof(g_EpsAdonisIpcDrv));

	g_pEpsAdonisIpcDrv = &g_EpsAdonisIpcDrv;

	adn_drv_func_t eps_adonis_ipc_drv_func = {eps_adonis_ipc_drv_open,  LSA_NULL,
	                                          LSA_NULL, eps_adonis_ipc_drv_close,
	                                          eps_adonis_ipc_drv_ioctl, LSA_NULL};

	/* Install Driver */
	ret_val = adn_drv_register(EPS_ADONIS_IPC_DRV_NAME, &eps_adonis_ipc_drv_func);
	if(ret_val != ADN_OK)
	{
	  return ADN_ERROR;
	}
	
	g_pEpsAdonisIpcDrv->bInit = LSA_TRUE;	
	
	return ADN_OK;

#else
	return ADN_ERROR;
#endif
}

/**
 * Deinstallation function of eps adonis inter processor communication driver
 * Unregisters driver functions to adonis
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
LSA_INT32 eps_adonis_ipc_drv_uninstall(void)
{
#ifdef EPS_IPC_DRV_SUPPORTED
	int32_t ret_val = ADN_OK;

	ret_val = adn_drv_unregister(EPS_ADONIS_IPC_DRV_NAME);
	
	g_pEpsAdonisIpcDrv->bInit = LSA_FALSE;
	g_pEpsAdonisIpcDrv = LSA_NULL;
	
	if(ret_val != ADN_OK)
	{
	  return ADN_ERROR;
	}
	return ADN_OK;
#else
	return ADN_ERROR;
#endif
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
