#ifndef EPS_TRACE_TIMER_IF_H_   /* ----- reinclude-protection ----- */
#define EPS_TRACE_TIMER_IF_H_

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_trace_timer_if.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Trace Timer Interface				                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
    
    
/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef LSA_UINT16 (*EPS_TRACE_TIMER_START_FCT)    (LSA_VOID);
typedef LSA_UINT16 (*EPS_TRACE_TIMER_STOP_FCT)     (LSA_VOID);
typedef LSA_UINT64 (*EPS_TRACE_TIMER_GET_TIME_FCT) (LSA_VOID);
typedef LSA_UINT16 (*EPS_TRACE_TIMER_UNINSTALL_FCT)(LSA_VOID);


typedef struct
{
   EPS_TRACE_TIMER_START_FCT      start;        // interface to registered Trace Timer
   EPS_TRACE_TIMER_STOP_FCT       stop;         // interface to registered Trace Timer
   EPS_TRACE_TIMER_GET_TIME_FCT   get_time;     // interface to registered Trace Timer
   EPS_TRACE_TIMER_UNINSTALL_FCT  uninstall;    // interface to registered Trace Timer
}  EPS_TRACE_TIMER_IF_TYPE, *EPS_TRACE_TIMER_IF_PTR_TYPE;
typedef EPS_TRACE_TIMER_IF_TYPE const* EPS_TRACE_TIMER_IF_CONST_PTR_TYPE;
   
/*****************************************************************************/
/*  Prototypes for Trace Timer (User) Interface                              */
/*****************************************************************************/

LSA_VOID   eps_trace_timer_if_register (EPS_TRACE_TIMER_IF_CONST_PTR_TYPE pTtIf);
LSA_VOID   eps_trace_timer_start (LSA_VOID);
LSA_VOID   eps_trace_timer_stop (LSA_VOID);
LSA_VOID   eps_trace_timer_init (LSA_VOID);
LSA_VOID   eps_trace_timer_undo_init (LSA_VOID);
LSA_UINT64 eps_trace_timer_get_time (LSA_VOID);

    
    
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_TRACE_TIMER_IF_H_ */
