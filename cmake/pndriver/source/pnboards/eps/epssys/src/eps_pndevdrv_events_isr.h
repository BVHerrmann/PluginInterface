#ifndef EPS_PNDEVDRV_EVENT_ISR_H_       /* ----- reinclude-protection ----- */
#define EPS_PNDEVDRV_EVENT_ISR_H_

#ifdef __cplusplus                      /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_pndevdrv_events_isr.h                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PnDevDrv Driver                                                      */
/*                                                                           */
/*****************************************************************************/

#include "psi_inc.h" /*7*/
#include "eps_pndevdrv_types.h"

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID eps_pndevdrv_event_isr_init(uPNDEV_DRIVER_DESC *pDrvHandle);
LSA_VOID eps_pndevdrv_event_isr_undo_init(LSA_VOID);

LSA_UINT16 eps_pndevdrv_enable_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16 eps_pndevdrv_disable_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);

//REFACT - TODO: Used by eps_pndevdrv.c internally
LSA_UINT16 eps_pndevdrv_disable_interrupt_intern(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);

LSA_UINT16 eps_pndevdrv_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_pndevdrv_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_pndevdrv_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);

void __stdcall eps_pndevdrv_event_cbf(const HANDLE hDeviceIn, const ePNDEV_EVENT_CLASS eEventClassIn,PNDEV_CHAR* sEventIn, const void* pCbfParUserIn);
void __stdcall eps_pndevdrv_isr_cbf(const HANDLE hDeviceIn, const BOOLEAN bDeviceTestIn, const ePNDEV_INT_SRC eIntSrcIn, const void* pCbfParUserIn);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDEVDRV_EVENT_ISR_H_ */
