#ifndef EPS_ENUMS_H_
#define EPS_ENUMS_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_enums.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS enums                                                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
    
/*---------------------------------------------------------------------------
     Defines for EPS Interrupt Mode for Basic Variants
---------------------------------------------------------------------------*/         
    
#define     EPS_ISR_MODE_POLL               1
#define     EPS_ISR_MODE_IR_USERMODE        2
#define     EPS_ISR_MODE_IR_KERNELMODE      3
    
/*-------------------------------------------------------------------------*/ 
/*---------------------------------------------------------------------------
     Defines for EPS IPC Acknowledge Mode
---------------------------------------------------------------------------*/
        
#define     EPS_IPC_ACK_MODE_ALL            0 // acknowlege all ipc interrupts with ping/pong mechanism
#define     EPS_IPC_ACK_MODE_NO_HERA        1 // fire and forget hera msix ipc interrupts between hera and host
#define     EPS_IPC_ACK_MODE_NONE           2 // fire and forget all ipc interrupts

/*-------------------------------------------------------------------------*/ 
    
/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef enum
{
	EPS_PNDEV_ASIC_ERTEC200  = 0,
	EPS_PNDEV_ASIC_ERTEC400  = 1,
	EPS_PNDEV_ASIC_IRTEREV7  = 2,
	EPS_PNDEV_ASIC_ERTEC200P = 3,
	EPS_PNDEV_ASIC_INTEL_XX  = 4,
    EPS_PNDEV_ASIC_KSZ88XX   = 5,
	EPS_PNDEV_ASIC_TI_XX     = 6,
	EPS_PNDEV_ASIC_HERA 	 = 7
} EPS_PNDEV_ASIC_TYPE, *EPS_PNDEV_ASIC_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_ENUMS_H_ */
