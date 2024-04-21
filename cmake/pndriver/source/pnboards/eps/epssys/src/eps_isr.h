#ifndef EPS_ISR_H_                  /* ----- reinclude-protection ----- */
#define EPS_ISR_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_isr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Interrupt handling                                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_eddi_exttimer_start ( PSI_SYS_HANDLE hSysDev );
LSA_VOID eps_eddi_exttimer_stop ( PSI_SYS_HANDLE hSysDev );
LSA_VOID eps_enable_pnio_event ( PSI_SYS_HANDLE const hSysDev );
LSA_VOID eps_disable_pnio_event ( PSI_SYS_HANDLE const hSysDev );
LSA_RESULT eps_enable_iso_interrupt ( PSI_SYS_HANDLE hSysDev, LSA_VOID(*func_ptr)(LSA_INT param, LSA_VOID* args), LSA_INT32 param, LSA_VOID* args );
LSA_RESULT eps_disable_iso_interrupt ( PSI_SYS_HANDLE hSysDev );
LSA_BOOL eps_edds_notify_fill_settings ( PSI_SYS_HANDLE const hSysDev, LSA_BOOL const bFillActive, LSA_BOOL const bIOConfigured, LSA_BOOL const bHsyncModeActive );
LSA_VOID eps_edds_do_notify_scheduler ( PSI_SYS_HANDLE const hSysDev );
LSA_VOID eps_edds_do_retrigger_scheduler ( PSI_SYS_HANDLE const hSysDev );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of EPS_ISR_H_ */
