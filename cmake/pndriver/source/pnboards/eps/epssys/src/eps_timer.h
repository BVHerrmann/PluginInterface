#ifndef EPS_TIMER_H_
#define EPS_TIMER_H_

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
/*  F i l e               &F: eps_timer.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS LSA Timer adaption                                                   */
/*                                                                           */
/*****************************************************************************/
#include <eps_rtos.h>       // definition of sigval

/*****************************************************************************/
/*  Macros                                                                   */
/*****************************************************************************/

/* return the number of elements in an array of any type*/
#if !defined(countof)
#define countof(a) (sizeof(a) / sizeof((a)[0]))
#endif

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef LSA_VOID (*EPS_TIMER_CBF) (LSA_TIMER_ID_TYPE, LSA_USER_ID_TYPE);
typedef LSA_VOID (*EPS_SYS_TIMER_CBF) (union sigval);

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_UINT16 eps_timer_init_tgroup0      ( LSA_VOID );
LSA_VOID   eps_timer_undo_init_tgroup0 ( LSA_VOID );
LSA_UINT16 eps_timer_allocate_tgroup0  ( LSA_TIMER_ID_TYPE * timer_id_ptr,  LSA_UINT16 timer_type, LSA_UINT16 time_base, EPS_TIMER_CBF callback_function  );

LSA_UINT16 eps_timer_init_tgroup1      ( LSA_VOID );
LSA_VOID   eps_timer_undo_init_tgroup1 ( LSA_VOID );
LSA_UINT16 eps_timer_allocate_tgroup1  ( LSA_TIMER_ID_TYPE * timer_id_ptr,  LSA_UINT16 timer_type, LSA_UINT16 time_base, EPS_TIMER_CBF callback_function  );
LSA_UINT16 eps_timer_start             ( LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id, LSA_UINT16 time  );
LSA_UINT16 eps_timer_stop              ( LSA_TIMER_ID_TYPE timer_id );
LSA_UINT16 eps_timer_free              ( LSA_TIMER_ID_TYPE timer_id );

LSA_VOID            eps_timer_init_sys_timer     ( LSA_VOID );
LSA_VOID            eps_timer_undo_init_sys_timer( LSA_VOID );
LSA_RESPONSE_TYPE   eps_timer_allocate_sys_timer ( LSA_UINT16* sys_timer_id_ptr, const LSA_CHAR* pTimerName, LSA_UINT32 lTimerCycleMs, EPS_SYS_TIMER_CBF callback_function, LSA_VOID* pArgs );
LSA_RESPONSE_TYPE   eps_timer_change_sys_timer   ( LSA_UINT16  sys_timer_id, LSA_UINT32 lTimerCycleMs);
LSA_RESPONSE_TYPE   eps_timer_free_sys_timer     ( LSA_UINT16  sys_timer_id );

#ifdef _TEST
LSA_UINT16 getEpsTimerMutex0(void);
LSA_UINT16 getEpsTimerMutex1(void);
timer_t gettid0(void);
timer_t gettid1(void);
#endif

#ifdef EPS_TEST_MEASURE_PERFORMANCE


	LSA_UINT32 getmeasureResultInNsCount(void);
	LSA_UINT32 *getmeasureResultInNs(void);
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_TIMER_H_ */
