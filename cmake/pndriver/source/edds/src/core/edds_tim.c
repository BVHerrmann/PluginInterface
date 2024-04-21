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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_tim.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Timer functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  10.08.04    JS    use of EDDS_FREE_CHECK macro                           */
/*  28.01.05    JS    LTRC support                                           */
/*  09.02.05    JS    added optional reentrance check                        */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  5
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_TIM */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*===========================================================================*/
/*=====  Timer -functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_StartTimer                             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE pDDB                +*/
/*+                             EDDS_TIMER_ENUM timerID                     +*/
/*+                             LSA_UINT64 firstExpireTime                  +*/
/*+ LSA_UINT32 cycleTime, LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATT        +*/
/*+        TimeoutMethod)(EDDS_LOCAL_DDB_PTR_TYPE pDDB)                     +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                  : DDB                                            +*/
/*+  timerID               : EDDS-Timer ID                                  +*/
/*+  firstExpireTime       : LSA_UINT64                                     +*/
/*+  fctPtr                : function pointer                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts timer in timer management                          +*/
/*+  Note:        This function may only be called by EDDS_DeviceSetup      +*/
/*+               before pDDB->pGlob->HWIsSetup is set to LSA_TRUE, or      +*/
/*+               by scheduler context.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_StartTimer(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
            EDDS_TIMER_ENUM timerID,
            LSA_UINT64 firstExpireTime,
            LSA_UINT32 cycleTime,
            LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, TimeoutMethod)(EDDS_LOCAL_DDB_PTR_TYPE pDDB)
)
{
    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_StartTimer(EDDSTimerID: 0x%X)",
                           timerID);

    if (timerID < EDDS_TIMER_MAX)
    {
        /* NOTE: set nsExpireTime to 0 at last, that enables the timer, see also EDDS_CheckInternalTimer */

        pDDB->timerMgmt[timerID].nsCycleTimeBase = cycleTime;
        pDDB->timerMgmt[timerID].TimeoutMethod = TimeoutMethod;
        pDDB->timerMgmt[timerID].nsExpireTime = firstExpireTime;
    }

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_StartTimer()");
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_StopTimer                              +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE pDDB                +*/
/*+                             EDDS_TIMER_ENUM timerID                     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                  : DDB                                            +*/
/*+  timerID               : EDDS-Timer ID                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops timer in timer management                           +*/
/*+                                                                         +*/
/*+  Important: Just stop the timer. The timer shall still be initialized.  +*/
/*+             That is because the scheduler is checking timers in         +*/
/*+             parallel.                                                   +*/
/*+  Note:        This function may only be called by EDDS_DeviceShutdown   +*/
/*+               after pDDB->pGlob->HWIsSetup was set to LSA_FALSE, or     +*/
/*+               by scheduler context.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_StopTimer(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDDS_TIMER_ENUM timerID)
{
    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_StopTimer(EDDSTimerID: 0x%X)",
                           timerID);

    if (timerID < EDDS_TIMER_MAX)
    {
        /* NOTE: just stop the timer */

        pDDB->timerMgmt[timerID].nsExpireTime = 0;
    }

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_StopTimer()");

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ResetAllTimer                          +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE pDDB                +*/
/*+                             EDDS_TIMER_ENUM timerID                     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                  : DDB                                            +*/
/*+  timerID               : EDDS-Timer ID                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops timer in timer management                           +*/
/*+  Note:        This function may only be called in scheduler context     +*/
/*+               or by EDDS_DeviceOpne/EDDS_DeviceClose in RQB context.    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ResetAllTimer(EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    LSA_UINT32 i;

    /* cannot use TraceIdx since it is initiated after this function is called for the first time (s. EDDS_DeviceOpen) */
    EDDS_CORE_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_ResetAllTimer");

    for (i=0; i<EDDS_TIMER_MAX;i++)
    {
        /* NOTE: set nsExpireTime to 0 at first, that disables the timer, see also EDDS_CheckInternalTimer */

        pDDB->timerMgmt[i].nsExpireTime = 0;
        pDDB->timerMgmt[i].nsCycleTimeBase = 0;
        pDDB->timerMgmt[i].TimeoutMethod = 0;
    }

    EDDS_CORE_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_ResetAllTimer()");
}


/*****************************************************************************/
/*  end of file EDDS_TIM.C                                                   */
/*****************************************************************************/

