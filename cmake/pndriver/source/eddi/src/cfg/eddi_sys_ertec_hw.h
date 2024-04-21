#ifndef EDDI_SYS_ERTEC_HW_H     //reinclude-protection
#define EDDI_SYS_ERTEC_HW_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_sys_ertec_hw.h                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* prototypes for ERTEC-specific functions                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.06.08    UL    created                                                */
/*                                                                           */
/*****************************************************************************/ 


/*=============================================================================*/
/*        GPIOs                                                                */
/*=============================================================================*/
#define SYS_ERTEC_ERTEC400_PLLGPIO_OUT  25
#define SYS_ERTEC_ERTEC200_PLLGPIO_OUT  25
#define SYS_ERTEC_ERTEC200_RS485_USED   LSA_TRUE
#define SYS_ERTEC_ERTEC400_RS485_USED   LSA_TRUE

extern LSA_RESULT EDDI_CpErtecSetPllPort( EDDI_SYS_HANDLE hSysDev,
                                          LSA_UINT32      pIRTE,
                                          LSA_UINT32      location,
                                          LSA_UINT16      pllmode );

extern LSA_VOID EDDI_SysSetErtecBasePtr( LSA_UINT32 pSWIReg,
                                         LSA_BOOL   isERTEC200,
                                         LSA_UINT32 pGPIO );

extern LSA_VOID EDDI_SysSetSendClockChange( LSA_UINT32 const scf );

extern LSA_VOID EDDI_SysE200GpioSetActivityLinkLed( LSA_VOID );

typedef struct cp_mem_s
{
    LSA_UINT32      pIRTE;
    LSA_UINT32      pGPIO;
    LSA_BOOL        isERTEC200;

} cp_mem_t;

extern cp_mem_t cp_mem;

LSA_VOID EDDI_SysSetDevBasePtrs( void       * pSWIRegVirt,                  //virtual base address of the IRTE registers
                                 LSA_UINT32   pSWIRegAHBAddr,               //physical (AHB) base address of the IRTE registers
                                 void       * pSharedNRTMemVirt,            //virtual base address of nrt memory if shared memory
                                 void       * pLocalNRTMemVirt,             //virtual base address of nrt memory if local memory
                                 LSA_UINT32   pSharedNRTMemAHBAddr,         //physical (AHB) address of nrt memory !!!includes PCI address 
                                 LSA_UINT32   pLocalNRTMemAHBAddr );        //physical (AHB) address of nrt memory !!!excludes PCI address 

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYS_ERTEC_HW_H


/*****************************************************************************/
/*  end of file eddi_sys_ertec_hw.h                                          */
/*****************************************************************************/
