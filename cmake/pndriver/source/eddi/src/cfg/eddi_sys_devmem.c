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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_sys_devmem.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* examples for memory-conversion-functions.                                 */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  10.09.08    UL    created                                                */
/*                                                                           */
/*****************************************************************************/ 

#include "lsa_cfg.h"
//#include "lsa_usr.h"
#include "lsa_sys.h"

#include "edd_cfg.h"
#include "eddi_cfg.h"
#include "edd_usr.h"
#include "eddi_usr.h"

#ifdef __cplusplus //If C++ - compiler: Use C linkage
extern "C"
{
#endif

/**********************************************************************************************************************
Translation of addresses:
A) PCI-Host (NRT-memory is shared memory)

PC-Adrspace                      PCI        IRTE-Adressspace
virt addr         PHY addr        |       AHB-Address   
                                  |       
            +---+ 1GB <---------- |------ 0xFFFFFFFF
            |   |                 |
            |...|                 |
            -----                 |
            |   |                 |
      ptr-> |   |                 |
            |   |                 |
0x80120000  ----- 0x12340000 <----|------ 0xD2340000
            |...|                 |
            |   |                 |
            +---+ 0 <------------ |------ 0xC0000000
                                  |
In the example above is ...
pNRTMemVirt = 0x80120000
SharedNRTMemAHBAddr = 0xD2340000
so any AHB-Address in the window above can be calulated as:
 AHBAddr = (ptr - pNRTMemVirt) + SharedNRTMemAHBAddr
Please note that SharedNRTMemAHBAddr contains the sum of the AHB-Base of the PCI-Space AND the physical base address of the allocated shared memory block!
######################################################################################################################
B) NRT memory is local memory

  PC-Addressspace     PCI        IRTE-Addressspace
       virt address    |        AHB address
                       |      +---+ 256MB 0x4FFFFFFF
                       |      |   |                
                       |      |...|                
                       |      -----                
                       |      |   |                
          ptr ---------|----->|   |                
                       |      |   |                
   0x80120000 ---------|----->----- 0x42340000 
                       |      |...|                
                       |      |   |                
                       |      +---+ 0     0x40000000
                       | 
                       |
In the example above is ...
pNRTMemVirt = 0x80120000
LocalNRTMemAHBAddr = 0x42340000
so any AHB-Address in the window above can be calulated as:
 AHBAddr = (ptr - pNRTMemVirt) + LocalNRTMemAHBAddr
**********************************************************************************************************************/
                                  
                                  
/*=============================================================================*/
/*        Global Vars                                                          */
/*=============================================================================*/
LSA_UINT32   SWIRegVirt;                    //Base-addr of IRTE-regs (virt.)
LSA_UINT32   SharedNRTMemVirt;              //Base-addr of NRTmem (virt.)
LSA_UINT32   LocalNRTMemVirt;               //Base-addr of NRTmem (virt.)
LSA_UINT32   SharedNRTMemAHBAddr;           //Base-addr of sharedmem (phys.)
LSA_UINT32   LocalNRTMemAHBAddr;            //Base-addr of localmem connected to ERTEC200/400 (phys.)
LSA_UINT32   SWIRegAHBAddr;                 //AHB-Base-addr of IRTE-regs

/*=============================================================================*/
/*        System Resources                                                     */
/*=============================================================================*/

//this function has to be called during initialization
void  EDDI_SysSetDevBasePtrs( void        *  pSWIRegVirt,                   //virtual base address of the IRTE registers
                              LSA_UINT32     pSWIRegAHBAddr,                //physical (AHB) base address of the IRTE registers
                              void        *  pSharedNRTMemVirt,             //virtual base address of nrt memory if shared memory
                              void        *  pLocalNRTMemVirt,              //virtual base address of nrt memory if local memory
                              LSA_UINT32     pSharedNRTMemAHBAddr,          //physical (AHB) address of nrt memory !!!includes PCI address translation!!!
                              LSA_UINT32     pLocalNRTMemAHBAddr)           //physical (AHB) address of nrt memory !!!excludes PCI address translation!!!
{
    SWIRegVirt                   = (LSA_UINT32)pSWIRegVirt;
    SWIRegAHBAddr                = (LSA_UINT32)pSWIRegAHBAddr;
    SharedNRTMemVirt             = (LSA_UINT32)pSharedNRTMemVirt;
    LocalNRTMemVirt              = (LSA_UINT32)pLocalNRTMemVirt;
    SharedNRTMemAHBAddr          = (LSA_UINT32)pSharedNRTMemAHBAddr;
    LocalNRTMemAHBAddr           = (LSA_UINT32)pLocalNRTMemAHBAddr;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================*/
/*        Conversion functions                                                 */
/*=============================================================================*/

/***************************************************************************
* F u n c t i o n:        EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA()
*
* D e s c r i p t i o n:  calculate KRAM-address from virtual addr.
*                         used for: Trace-DMA (address of TCW)
*
* A r g u m e n t s:      p:        virtual addr
*
* Return Value:           KRAM-Address (AHB-Adress)
*
***************************************************************************/
//corresponds to: kram_adr_loc_to_asic_dma in older EDDI_ERTEC-Versions
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA( EDDI_SYS_HANDLE            hSysDev,
                                                                           LSA_VOID         *  const  p,
                                                                           LSA_UINT32          const  location )
{
    //!!!!!!!!!!!!!!!!!!!
    // Attention: this example assumes that the TCW is located in KRAM. If located elsewhere, the calculation from
    // virtual to AHB-address has to be adapted!
    //!!!!!!!!!!!!!!!!!!!

    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(location);

    // old calculations:
    // FPGA:    (p - IRTE_SWI_BaseAdr) | 0x00400000UL
    // SOC:     (p - IRTE_SWI_BaseAdr) | 0x1D200000UL
    // 400/200: (p - IRTE_SWI_BaseAdr) | 0x10000000UL

    //calculate KRAM-AHB-address from virtual address
    return((((LSA_UINT32)p - SWIRegVirt) + SWIRegAHBAddr)); // old | 0x00400000UL
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:        EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC()
*
* D e s c r i p t i o n:  calculate AHB-addr for local RAM-access (i.e. RAM connected directly to ERTEC200/400)
*                         from virtual addr.
*                         used for: mirror-buffer-access (IO-DMA from/to KRAM), NRT-buffers if
*                         MemModeDMA==MEMORY_SDRAM_ERTEC or MemModeBuffer==MEMORY_SDRAM_ERTEC is selected
*
* A r g u m e n t s:      p:        virtual addr
*
* Return Value:           AHB-Address (IRTE-sight)
*
***************************************************************************/
//corresponds to: sdram_mem_adr_to_asic in older EDDI_ERTEC-Versions
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC( EDDI_SYS_HANDLE            hSysDev,
                                                                         LSA_VOID         *  const  p,
                                                                         LSA_UINT32          const  location )
{
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(location);

    // old calculations:
    // FPGA_SOC:      (p - BOARD_SDRAM_BaseAddr) | 0x20000000UL
    // FPGA_400/200:  (p - BOARD_SDRAM_BaseAddr) | 0x80000000UL
    // SOC:           EDDI_DEV_CALCADR_SOC1_DDR2_TO_AHB__(p)
    // 400/200:       (p - BOARD_SDRAM_BaseAddr) | 0x20000000UL

    return(((LSA_UINT32)p - LocalNRTMemVirt) + LocalNRTMemAHBAddr); //old: | 0x20000000UL
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:        EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC()
*
* D e s c r i p t i o n:  calculate AHB-addr for shared RAM-access (i.e. RAM connected directly to SOC or accessed via PCI)
*                         from virtual addr.
*                         used for: mirror-buffer-access (IO-DMA from/to KRAM), NRT-buffers if
*                         MemModeDMA==MEMORY_SHARED_MEM or MemModeBuffer==MEMORY_SHARED_MEM is selected
*
* A r g u m e n t s:      p:        virtual addr
*
* Return Value:           AHB-Address (IRTE-sight)
*
***************************************************************************/
//corresponds to: shared_mem_adr_to_asic in older EDDI_ERTEC-Versions
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC( EDDI_SYS_HANDLE            hSysDev,
                                                                            LSA_VOID         *  const  p,
                                                                            LSA_UINT32          const  location )

{
    LSA_UNUSED_ARG(hSysDev);

    // old calculations:
    // FPGA:    (p - PC_HostVirt_BaseAdrSharedMem  + PC_HostPhy_BaseAdrSharedMem)   might be wrong, but FPGA cannot act as PCI master!
    // SOC/400: (p - PC_HostVirt_BaseAdrSharedMem  + PC_HostPhy_BaseAdrSharedMem) + 0xC0000000UL
    // 200:     (p - PC_HostVirt_BaseAdrSharedMem  + PC_HostPhy_BaseAdrSharedMem) | 0x33000000UL 

    // SOC/400:
    // Calculations assume access of shared memory via PCI:
    // PC_HostPhy_BaseAdrSharedMem stores the physical PCI addr. (PC: lowest 1GB)
    // Mapping: PhysPCI_PC 0x0000:0000 - 0x3FFF:FFFF
    //          SOC-AHB    0xC000:0000 - 0xFFFF:FFFF
    
    // 200:
    // Calculations assume access of shared memory via LOCALBUS
    
    if (EDDI_LOC_FPGA_XC2_V8000 == location)
    {
        return(0); //FPGA cannot act as PCI master!
    }
    else
    {
        return (((LSA_UINT32)p - SharedNRTMemVirt) + SharedNRTMemAHBAddr);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:        EDDI_DEV_SHAREDMEM_OFFSET()
*
* D e s c r i p t i o n:  calculate offset in shared-mem to start of shared-mem
*
* A r g u m e n t s:      p:        virtual adr.
*
* Return Value:           offset
*
***************************************************************************/
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_SHAREDMEM_OFFSET( EDDI_SYS_HANDLE            hSysDev,
                                                                 LSA_VOID         *  const  p,
                                                                 LSA_UINT32          const  location )
{
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(location);

    return((LSA_UINT32)p - SharedNRTMemVirt);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:        EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL()
*
* D e s c r i p t i o n:  calculate virtual-addr for KRAM/IRTE-REG-access
*                         from offset to start of IRTE-Regs.
*
* A r g u m e n t s:      p:        Offset to start of IRTE-Regs
*
* Return Value:           virtual addr.
*
***************************************************************************/
//corresponds to: kram_adr_asic_to_local in older EDDI_ERTEC-Versions
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL( EDDI_SYS_HANDLE         hSysDev,
                                                                       LSA_UINT32       const  p,
                                                                       LSA_UINT32       const  location )
{
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(location);

    return(SWIRegVirt + p);
}
/*---------------------- end [subroutine] ---------------------------------*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif


/*****************************************************************************/
/*  end of file eddi_sys_devmem.c                                            */
/*****************************************************************************/

