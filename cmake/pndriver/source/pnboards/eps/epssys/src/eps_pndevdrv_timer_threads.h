#ifndef EPS_PNDEVDRV_TIMER_THREADS_H_   /* ----- reinclude-protection ----- */
#define EPS_PNDEVDRV_TIMER_THREADS_H_

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
/*  F i l e               &F: eps_pndevdrv_timer_threads.h              :F&  */
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

//lint --e(537) Repeated include file - Defines are necessary, but other C files directly include psi_inc.h
#include "psi_inc.h" /*9*/
#include "eps_pndevdrv_types.h"

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
typedef struct eps_pndevdrv_poll_tag
{
	LSA_BOOL                        bRunning;
	LSA_UINT32                      uCntUsers;
	LSA_UINT32                      hThread;
	EPS_PNDEV_INTERRUPT_DESC_TYPE   uIntSrc[EPS_CFG_PNDEVDRV_MAX_BOARDS];
	EPS_PNDEVDRV_BOARD_PTR_TYPE     pBoard[EPS_CFG_PNDEVDRV_MAX_BOARDS];
} EPS_PNDEVDRV_POLL_TYPE, *EPS_PNDEVDRV_POLL_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID   eps_pndevdrv_timer_threads_init(uPNDEV_DRIVER_DESC *pDrvHandle);
LSA_VOID eps_pndevdrv_timer_threads_undo_init(LSA_VOID);

// Thread specific
LSA_UINT16 eps_pndevdrv_create_pn_gathered_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard);
// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_pndevdrv_create_pn_gathered_polling_thread)
//lint -esym(759, eps_pndevdrv_create_pn_gathered_polling_thread)
//lint -esym(765, eps_pndevdrv_create_pn_gathered_polling_thread)
LSA_UINT16 eps_pndevdrv_kill_pn_gathered_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard);
// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_pndevdrv_kill_pn_gathered_polling_thread)
//lint -esym(759, eps_pndevdrv_kill_pn_gathered_polling_thread)
//lint -esym(765, eps_pndevdrv_kill_pn_gathered_polling_thread)
LSA_UINT16 eps_pndevdrv_kill_pn_nrt_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard);
// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_pndevdrv_kill_pn_nrt_polling_thread)
//lint -esym(759, eps_pndevdrv_kill_pn_nrt_polling_thread)
//lint -esym(765, eps_pndevdrv_kill_pn_nrt_polling_thread)
LSA_UINT16 eps_pndevdrv_create_pn_nrt_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard);
// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_pndevdrv_create_pn_nrt_polling_thread)
//lint -esym(759, eps_pndevdrv_create_pn_nrt_polling_thread)
//lint -esym(765, eps_pndevdrv_create_pn_nrt_polling_thread)
LSA_UINT16 eps_pndevdrv_create_file_transfer_poll_thread(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard);
LSA_VOID   eps_pndevdrv_kill_file_transfer_poll_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard);

// Timer specific
LSA_UINT16  eps_pndevdrv_timer_ctrl_start       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16  eps_pndevdrv_timer_ctrl_stop        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_pndevdrv_open_pn_gathered_timer (EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard);
LSA_UINT16 eps_pndevdrv_close_pn_gathered_timer(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard);
LSA_UINT16 eps_pndevdrv_open_pn_nrt_timer      (EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard);
LSA_UINT16 eps_pndevdrv_close_pn_nrt_timer     (EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard);

//REFACT - TODO: Used by eps_pndevdrv.c internally
LSA_UINT16 eps_pndevdrv_timer_ctrl_stop_intern (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDEVDRV_TIMER_THREADS_H_ */
