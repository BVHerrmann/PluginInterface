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
/*  F i l e               &F: eps_plf_adonis.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PLF Adonis Adaption                                                  */
/*  for Ertec200(EB200), Ertec400(CP1616), Soc1, Ertec200P(FPGA, EB200P)     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20129
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include <psi_inc.h>

#include <eps_rtos.h>        /* Types / Prototypes / Funcs        */ 
#include <eps_sys.h>         /* Types / Prototypes / Funcs        */
#include <eps_tasks.h>       /* Thread if                         */
#include <eps_trc.h>         /* Tracing                           */
#include <eps_cp_hw.h>       /* Pn Device Hw Adaption Layer       */
#include <eps_pn_drv_if.h>   /* Pn Device Drv Interface           */
#include <eps_shm_if.h>      /* Shared Memory Interface           */
#include <eps_hif_drv_if.h>  /* HIF Driver Interface              */
#include <eps_plf.h>         /* Own Header                        */

#include <eps_pncore.h>            /* PN Core Interface driver          */
#include <eps_pn_eb200p_drv.h>     /* Pn EB200P   Drv                   */
#include <eps_pn_ertec200_drv.h>   /* Pn Ertec200 Drv                   */
#include <eps_pn_ertec400_drv.h>   /* Pn Ertec400 Drv                   */
#include <eps_pn_soc1_drv.h>       /* Pn Soc1     Drv                   */
#include <eps_pn_hera_drv.h>       /* Pn HERA     Drv                   */
#include <eps_hif_universal_drv.h> /* Generic HIF-Driver                */
#include <eps_hif_ipcsiedma_drv.h> /* Siemens DMA IPC HIF-Driver        */
#include <eps_hif_pncorestd_drv.h> /* Siemens PnCore Hif-Driver         */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#ifdef EPS_USE_RTOS_ADONIS

#include <eps_adonis_isr_drv.h>   /* EPS Adonis Isr Driver If             */
#include <eps_pndrvif.h>          /* Adonis PN Interrupt driver interface */
#include <pnip_drv.h>             /* Adonis PNIP Interrupt Driver         */
 
#if (EPS_PLF == EPS_PLF_ADONIS_X86)
#include <serial.h>              /* Com Port Driver                */
#include <eps_pndevdrv.h>        /* PnDevDrv                       */
#include <eps_vdd.h>             /* VDD Interface                  */
#include <eps_shm_file.h>        /* Shared Memory Interface        */
#endif

#include <pntrc_inc.h>

extern EPS_PLF_ADONIS_TYPE g_EpsPlfAdonis;
extern EPS_SHM_HW_PTR_TYPE g_pEpsPlfShmHw;

/**
 * Disable interrupts in eps globaly
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID EPS_PLF_DISABLE_INTERRUPTS(LSA_VOID)
{
	LSA_INT ret_val;
	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_ICU_GLOB_DISABLE, 0);
	EPS_ASSERT(ret_val == 0);	
}

/**
 * Enable interrupts in eps globaly
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID EPS_PLF_ENABLE_INTERRUPTS(LSA_VOID)
{
	LSA_INT ret_val;
	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_ICU_GLOB_ENABLE, 0);
	EPS_ASSERT(ret_val == 0);
}

/**
 * register SharedMemoryInterface
 * 
 * @param pShmIf pointer to if configuration
 * @return
 */
LSA_VOID EPS_PLF_REGISTER_SHM_IF(LSA_VOID* pShmIf)
{
	g_pEpsPlfShmHw = (EPS_SHM_HW_PTR_TYPE)pShmIf;
}

/**
 * assigns the value of lVal to the variable that pAddr points to
 * 
 * @param pAddr pointer, where lVal will assigned to
 * @param lVal	Value
 */
long EPS_PLF_EXCHANGE_LONG(long volatile *pAddr, long lVal)
{
	#if ((EPS_PLF == EPS_PLF_ERTEC400_ARM9) || (EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_PNIP_ARM9))
		return ATOMIC_SWAP(pAddr, lVal);
    #else
		return ADN_SYNC_LOCK_TEST_AND_SET(pAddr, lVal);
	#endif
}

#if ((EPS_PLF == EPS_PLF_ADONIS_X86) || (EPS_PLF == EPS_PLF_SOC_MIPS))

/**
 * translate virtual address to physical address
 * 
 * @param pVirtAddress pointer of virtual address
 * @return pointer of physical address
 */
LSA_UINT8* EPS_PLF_VIRTUAL_TO_PHYSICAL_ADDRESS(LSA_UINT8* pVirtAddress)
{
	LSA_INT                             ret_val;
	EPS_ADONIS_ISR_DRV_PTR_TO_ADDR_TYPE AddressArgs;

	AddressArgs.pSrcAddress = pVirtAddress;
	AddressArgs.pPhysAddress = (LSA_UINT8*)0xffffffff;

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PTR_TO_PHYSICAL_ADDR, &AddressArgs);
	EPS_ASSERT(ret_val == 0);			

	EPS_ASSERT(AddressArgs.pPhysAddress != (LSA_UINT8*)0xffffffff);

	return AddressArgs.pPhysAddress;
}

/**
 * Search for pci devices 
 * (used for IMCEA and Stdmacdrv)
 * 
 * @param uVendorId	Value of VendorId
 * @param uDeviceId Value of DeviceId
 * @param pConfigSpaceOut pointer to sturctur of PciConfigSpace
 * @param uMaxDevice Value of maximum existent devices
 * @param pFoundDevices pointer to Counter of found Devices
 * @return
 */
LSA_VOID EPS_PLF_PCI_GET_DEVICES(LSA_UINT16 uVendorId, LSA_UINT16 uDeviceId, EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpaceOut, LSA_UINT16 uMaxDevice, LSA_UINT16 *pFoundDevices)
{
	LSA_INT ret_val;

	EPS_ADONIS_ISR_DRV_PCI_GET_DEVICE_TYPE sGetDeviceArgs;

	eps_memset(&sGetDeviceArgs, 0, sizeof(sGetDeviceArgs));

	sGetDeviceArgs.uVendorId     = uVendorId;
	sGetDeviceArgs.uDeviceId     = uDeviceId;
	sGetDeviceArgs.pConfigSpaces = (LSA_VOID*)pConfigSpaceOut;
	sGetDeviceArgs.uMaxDevice    = uMaxDevice;
	sGetDeviceArgs.pFoundDevices = pFoundDevices;

	EPS_ASSERT(g_EpsPlfAdonis.fdEpsAdonisIsr != -1);

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PCI_GET_DEVICES, (LSA_VOID*)&sGetDeviceArgs);
	EPS_ASSERT(ret_val == 0);
}

/**
 * Enable Pci interrupt at adonis
 * 
 * @param pConfigSpace pointer to ConfigSpace of corresponding PciDevice
 * @param pCbf pointer to function, which will be called in InterruptContext
 * @return
 */
LSA_VOID EPS_PLF_PCI_ENA_INTERRUPT(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpace, EPS_PLF_PCI_ISR_CBF_PTR_TYPE pCbf)
{
	LSA_INT                                      ret_val;
	EPS_ADONIS_ISR_DRV_PCI_ENABLE_INTERRUPT_TYPE IsrEnableArgs;

	IsrEnableArgs.uBusNum    = pConfigSpace->uBusNr;
	IsrEnableArgs.uDeviceNum = pConfigSpace->uDeviceNr;
	IsrEnableArgs.uFuncNum   = pConfigSpace->uFunctionNr;
	IsrEnableArgs.cbf.pCbf   = pCbf->pCbf;
	IsrEnableArgs.cbf.uParam = pCbf->uParam;
	IsrEnableArgs.cbf.pArgs  = pCbf->pArgs;

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PCI_ENABLE_INTERRUPT, (LSA_VOID*)&IsrEnableArgs);
	EPS_ASSERT(ret_val == 0);
}

/**
 * Disable Pci interrupt at adonis
 * 
 * @param pConfigSpace pointer to ConfigSpace of corresponding PciDevice
 * @return
 */
LSA_VOID EPS_PLF_PCI_DIA_INTERRUPT(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpace)
{
	LSA_INT                              ret_val;
	EPS_ADONIS_ISR_DRV_PCI_LOCATION_TYPE Args;

	Args.uBusNum    = pConfigSpace->uBusNr;
	Args.uDeviceNum = pConfigSpace->uDeviceNr;
	Args.uFuncNum   = pConfigSpace->uFunctionNr;

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PCI_DISABLE_INTERRUPT, (LSA_VOID*)&Args);
	EPS_ASSERT(ret_val == 0);
}

/**
 * Reads a 8 bit value from the configuration space of a pci device
 * 
 * @param uBusNum		PCI bus number
 * @param uDeviceNum	Device number on PCI bus
 * @param uFuncNum		Function number of device
 * @param uOffset		Offset in specific configuration space
 * @param uVal			Pointer to data to read value in
 * @return
 */
LSA_VOID EPS_PLF_PCI_READ_BYTE(LSA_UINT32 uBusNum, LSA_UINT32 uDeviceNum, LSA_UINT32 uFuncNum, LSA_UINT32 uOffset, LSA_UINT8* uVal)
{
	LSA_INT                             ret_val;
	EPS_ADONIS_ISR_DRV_PCI_READ_U8_TYPE sPCIReadU8Args;

	eps_memset(&sPCIReadU8Args, 0, sizeof(sPCIReadU8Args));

	sPCIReadU8Args.uBusNum    = uBusNum;
	sPCIReadU8Args.uDeviceNum = uDeviceNum;
	sPCIReadU8Args.uFuncNum   = uFuncNum;
	sPCIReadU8Args.uOffset    = uOffset;
	sPCIReadU8Args.pVal       = uVal;

	EPS_ASSERT(g_EpsPlfAdonis.fdEpsAdonisIsr != -1);

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PCI_READ_U8, (LSA_VOID*)&sPCIReadU8Args);
	EPS_ASSERT(ret_val == 0);
}

/**
 * Reads a 16 bit value from the configuration space of a pci device
 * 
 * @param uBusNum		PCI bus number
 * @param uDeviceNum	Device number on PCI bus
 * @param uFuncNum		Function number of device
 * @param uOffset		Offset in specific configuration space
 * @param uVal			Pointer to data to read value in
 * @return
 */
LSA_VOID EPS_PLF_PCI_READ_DOUBLE_WORD(LSA_UINT32 uBusNum, LSA_UINT32 uDeviceNum, LSA_UINT32 uFuncNum, LSA_UINT32 uOffset, LSA_UINT32* uVal)
{
	LSA_INT                              ret_val;
	EPS_ADONIS_ISR_DRV_PCI_READ_U32_TYPE sPCIReadU32Args;

	eps_memset(&sPCIReadU32Args, 0, sizeof(sPCIReadU32Args));

	sPCIReadU32Args.uBusNum    = uBusNum;
	sPCIReadU32Args.uDeviceNum = uDeviceNum;
	sPCIReadU32Args.uFuncNum   = uFuncNum;
	sPCIReadU32Args.uOffset    = uOffset;
	sPCIReadU32Args.pVal       = uVal;

	EPS_ASSERT(g_EpsPlfAdonis.fdEpsAdonisIsr != -1);

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PCI_READ_U32, (LSA_VOID*)&sPCIReadU32Args);
	EPS_ASSERT(ret_val == 0);
}

/**
 * map memmory
 * 
 * @param pBase pointer to address, where the mapping will take place
 * @param uBase LocalAddress to memory, which will be mapped 
 * @param uSize	Size of mapped memory
 * @return LSA_TRUE or LSA_NULL if no memory was mapped
 */
LSA_BOOL EPS_PLF_MAP_MEMORY(LSA_UINT8** pBase, LSA_UINT32 uBase, LSA_UINT32 uSize)
{
	int     fd;
	LSA_INT ret_val;

	fd = g_EpsPlfAdonis.fdEpsAdonisIsr;

	EPS_PLF_DISABLE_INTERRUPTS();

	ret_val = ioctl(fd, EPS_ADONIS_ISR_DRV_MMAP_PREPARE, (LSA_VOID*)uBase);
	EPS_ASSERT(ret_val == 0);	

	EPS_PLF_ENABLE_INTERRUPTS();

	*pBase = (LSA_UINT8*) mmap (NULL, uSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(*pBase == MAP_FAILED)
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/**
 * unmap memmory
 * 
 * @param pBase pointer to address, where the unmapping will take place
 * @param uSize Size of mapped memory
 * @return LSA_TRUE or LSA_FALSE if memory was not unmapped
 */
LSA_BOOL EPS_PLF_UNMAP_MEMORY(LSA_UINT8* pBase, LSA_UINT32 uSize)
{
    int ret_val;
    
    ret_val = munmap((void*) pBase, uSize);
    
    if(ret_val == 0)
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}

/**
 * translate PciAdress to LocalAddress
 * 
 * @param pPciAddress pointer to PciAddress
 * @return pointer to LocalAddress
 */
LSA_UINT8* EPS_PLF_PCI_TRANSLATE_PCI_TO_LOCAL_ADDR(LSA_UINT8* pPciAddress)
{
	LSA_INT                                   ret_val;
	EPS_ADONIS_ISR_DRV_PCI_TO_LOCAL_ADDR_TYPE AddressArgs;

	AddressArgs.pPciAddress  = pPciAddress;
	AddressArgs.pPhysAddress = (LSA_UINT8*)0xffffffff;

	ret_val = ioctl(g_EpsPlfAdonis.fdEpsAdonisIsr, EPS_ADONIS_ISR_DRV_PCI_TO_LOCAL_ADDR, &AddressArgs);
	EPS_ASSERT(ret_val == 0);			

	EPS_ASSERT(AddressArgs.pPhysAddress != (LSA_UINT8*)0xffffffff);

	return AddressArgs.pPhysAddress;
}

#endif

#endif
