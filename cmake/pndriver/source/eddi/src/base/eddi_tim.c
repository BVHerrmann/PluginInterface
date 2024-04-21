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
/*  F i l e               &F: eddi_tim.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-Timer functions                              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_time.h"

#define EDDI_MODULE_ID     M_ID_EDDI_TIM
#define LTRC_ACT_MODUL_ID  13

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/*===========================================================================*/
/*=====  Timer -functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_TimerInfoFree()                        +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees Timermanagement                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TimerInfoFree( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_TIMER_TYPE  *  pTimer;
    LSA_RESULT          Status;
    LSA_UINT16          Ctr;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TimerInfoFree->");

    for (Ctr = 0; Ctr < EDDI_CFG_MAX_TIMER; Ctr++)
    {
        pTimer = &g_pEDDI_Info ->Timer[Ctr];

        if (!pTimer->used)
        {
            continue;
        }

        if (pTimer->pDDB != pDDB)
        {
            continue;
        }

        pTimer->used = 0;

        EDDI_STOP_TIMER(&Status, pTimer->SysTimerID);
        switch (Status)
        {
            case EDDI_STS_OK_TIMER_NOT_RUNNING:
            case EDD_STS_OK:
                break;

            default:
                EDDI_Excp("EDDI_TimerInfoFree", EDDI_FATAL_ERR_EXCP, Status, Ctr);
                return;
        }

        EDDI_FREE_TIMER(&Status, pTimer->SysTimerID);
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_TimerInfoFree", EDDI_FATAL_ERR_EXCP, Status, Ctr);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_AllocTimer()                           +*/
/*+  Input/Output          :                                                +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID put into timer-management                    +*/
/*+  pIntRQB      Pointer to intenal RQB put into timer-management          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Puts new Timer into Timer-management                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_AllocTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                  LSA_UINT16            *  eddi_timer_id_ptr,
                                                  void                         *  context,
                                                  const  EDDI_TIMEOUT_CBF         cbf,
                                                  LSA_UINT16                      TimerType,
                                                  LSA_UINT16                      TimeBase,
                                                  const  LSA_UINT32               QueueIdx )
{
    LSA_RESULT             Status;
    LSA_TIMER_ID_TYPE      SysTimerID;
    LSA_UINT16             Ctr;
    EDDI_TIMER_TYPE     *  pTimer = EDDI_NULL_PTR;
    EDDI_SHED_OBJ_TYPE  *  pShedObj;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_AllocTimer");

    for (Ctr = 0; Ctr < EDDI_CFG_MAX_TIMER; Ctr++)
    {
        pTimer = &g_pEDDI_Info ->Timer[Ctr];

        if (pTimer->used)
        {
            continue;
        }

        pTimer->used    = 1;
        pTimer->pDDB    = pDDB;
        pTimer->context = context;
        pTimer->cbf     = cbf;
        break;
    }

    if (Ctr == EDDI_CFG_MAX_TIMER)
    {
        EDDI_Excp("EDDI_AllocTimer, no free timer available", EDDI_FATAL_ERR_EXCP, Ctr, 0);
        return EDD_STS_ERR_RESOURCE;
    }

    EDDI_ALLOC_TIMER(&Status, &SysTimerID, TimerType, TimeBase);
    if (Status != EDD_STS_OK)
    {
        LSA_UNUSED_ARG(QueueIdx); //satisfy lint!

        EDDI_Excp("EDDI_AllocTimer, unexpected status from EDDI_ALLOC_TIMER()", EDDI_FATAL_ERR_EXCP, Status, 0);
        return Status;
    }

    pTimer->SysTimerID = SysTimerID;

    pShedObj         = &pTimer->ShedObj;
    pShedObj->bUsed  = LSA_FALSE;
    pShedObj->Para   = (LSA_UINT32)pTimer;

    pShedObj->IntRQB.internal_context   = pShedObj;
    pShedObj->IntRQB.internal_context_1 = (LSA_UINT32)pDDB;
    EDD_RQB_SET_OPCODE(&pShedObj->IntRQB, EDD_OPC_REQUEST);

    EDD_RQB_SET_SERVICE(&pShedObj->IntRQB, EDDI_SRV_EV_TIMER_REST);

    *eddi_timer_id_ptr = Ctr;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_StartTimer()                           +*/
/*+  Input/Output          :    LSA_UINT16                      TimerID     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_StartTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                  LSA_UINT16        const  eddi_timer_id,
                                                  LSA_UINT16               const  TimeTics )
{
    LSA_RESULT           Status;
    EDDI_TIMER_TYPE   *  pTimer;
    LSA_USER_ID_TYPE     user_id;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_StartTimer->eddi_timer_id:0x%X", eddi_timer_id);

    if (eddi_timer_id >= EDDI_CFG_MAX_TIMER)
    {
        EDDI_Excp("EDDI_StartTimer, eddi_timer_id >= EDDI_CFG_MAX_TIMER", EDDI_FATAL_ERR_EXCP, pDDB, eddi_timer_id);
        return EDD_STS_ERR_RESOURCE;
    }

    pTimer = &g_pEDDI_Info ->Timer[eddi_timer_id];
    if (pTimer->used == 0)
    {
        EDDI_Excp("EDDI_StartTimer, pTimer->used == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (TimeTics == 0)
    {
        EDDI_Excp("EDDI_StartTimer, TimeTics == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_PARAM;
    }

    user_id.uvar32 = eddi_timer_id;
    EDDI_START_TIMER(&Status, pTimer->SysTimerID, user_id, TimeTics);

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_StopTimer()                            +*/
/*+  Input/Output          :    LSA_UINT16                      TimerID     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID to free in management                        +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_StopTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                 LSA_UINT16        const  eddi_timer_id )
{
    LSA_RESULT          Status;
    EDDI_TIMER_TYPE  *  pTimer;

    if (eddi_timer_id  >= EDDI_CFG_MAX_TIMER)
    {
        EDDI_Excp("EDDI_StopTimer, eddi_timer_id  >= EDDI_CFG_MAX_TIMER", EDDI_FATAL_ERR_EXCP, pDDB, eddi_timer_id);
        return(EDD_STS_ERR_RESOURCE);
    }

    pTimer = &g_pEDDI_Info ->Timer[eddi_timer_id];
    if (pTimer->used == 0)
    {
        EDDI_Excp("EDDI_StopTimer, pTimer->used == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return(EDD_STS_ERR_SEQUENCE);
    }

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_StopTimer, call EDDI_STOP_TIMER()");
    EDDI_STOP_TIMER(&Status, pTimer->SysTimerID);
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    eddi_timeout()                              +*/
/*+  Input/Output          :    LSA_TIMER_ID_TYPE  timer_id                 +*/
/*+                             LSA_USER_ID_TYPE   user_id                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  timer_id     : ID of Timer                                             +*/
/*+  user_id      : User-ID of Timer                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called from systemadaption on timerevent. May be called   +*/
/*+               within ISR-Handler.                                       +*/
/*+                                                                         +*/
/*+               LSA-Systemfunction                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  eddi_timeout( LSA_TIMER_ID_TYPE  const timer_id,
                                                 LSA_USER_ID_TYPE   const user_id )
{
    EDDI_TIMER_TYPE     *  pTimer;
    LSA_UINT32             eddi_timer_id;
    EDDI_SHED_OBJ_TYPE  *  pShedObj;

    eddi_timer_id = user_id.uvar32;

    if (eddi_timer_id  >= EDDI_CFG_MAX_TIMER)
    {
        LSA_UNUSED_ARG(timer_id); //satisfy lint!

        EDDI_Excp("eddi_timeout, eddi_timer_id  >= EDDI_CFG_MAX_TIMER", EDDI_FATAL_ERR_EXCP, eddi_timer_id, 0);
        return;
    }

    pTimer = &g_pEDDI_Info ->Timer[eddi_timer_id];

    if (!pTimer->pDDB->Glob.HWIsSetup)
    {
        return;
    }

    //do not send timer-rqbs if all channels are closed
    if (0 == pTimer->pDDB->Glob.OpenCount)
    {
        return;
    }

    if (pTimer->used == 0)
    {
        //can occur due to racecondition
        return;
    }

    pShedObj = &pTimer->ShedObj;

    if (pShedObj->bUsed)
    {
        return;
    }

    EDDI_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "eddi_timeout, eddi_timer_id:0x%X user_id.svar32:0x%X", eddi_timer_id, user_id.svar32);

    //do not send rqb anymore if all channels are closed
    if (0 == pTimer->pDDB->Glob.OpenCount)
    {
        return;
    }

    pShedObj->bUsed = LSA_TRUE;

    EDDI_DO_EV_TIMER_REST(&pShedObj->IntRQB);
} 
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_tim.c                                                   */
/*****************************************************************************/

