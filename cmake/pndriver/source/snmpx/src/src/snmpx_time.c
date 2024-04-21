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
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)             :C&  */
/*                                                                           */
/*  F i l e               &F: snmpx_time.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX-Timer functions                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  24.04.12    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  40
#define SNMPX_MODULE_ID     LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_TIME */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles  */
#include "snmpx_int.h"            /* internal header    */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

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
/*+  Functionname          :    SNMPX_TimerCB                               +*/
/*+  Input                      SNMPX_INTERNAL_REQUEST_PTR_TYPE  pIntRQB    +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to internal RQB-mgm.                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Timerevent for Cylcic Timer.                      +*/
/*+               Called by SNMPX-Timerhandler                              +*/
/*+                                                                         +*/
/*+               It can be configured if we should call the output macro   +*/
/*+               SNMPX_DO_TIMER_REQUEST for doing a context change or if   +*/
/*+               we should do the handling inside the function.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_TimerCB(
    SNMPX_INTERNAL_REQUEST_PTR_TYPE   pIntRQB)
{
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                          "IN : SNMPX_TimerCB(pIntRQB: 0x%X)",
                          pIntRQB);

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup! */
        pIntRQB->InUse = LSA_TRUE;

#ifndef SNMPX_CFG_HANDLE_DO_TIMEOUT_INTERN
        SNMPX_DO_TIMER_REQUEST(pIntRQB->pRQB); /* at least calls SNMPX_RequestTimer() */
#else
        SNMPX_RequestTimer(pIntRQB->pRQB);     /* in another thread, ENTER and EXIT */
#endif
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_TimerCB");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   SNMPX_TimerSet                               +*/
/*+  Input/Output          :   LSA_UINT16                     TimerID       +*/
/*+                            SNMPX_UPPER_RQB_PTR_TYPE       pRQB          +*/
/*+                            LSA_FCT_PTR(ATTR, Cbf) (..)                  +*/
/*+                            SNMPX_LOCAL_MEM_U16_PTR_TYPE   pSNMPXTimerID +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerID      LSA-Timer ID put into timer-management                    +*/
/*+  pRQB         Pointer to intenal RQB put into timer-management          +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+  pSNMPXTimerID  SNMPX internal Timer ID (= index ) returned             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Puts new Timer into Timer-management                      +*/
/*+               if a Cbf is present, it is called if the timer elapses.   +*/
/*+               if no Cbf is present, when the timer elapses the output   +*/
/*+               Macro SNMPX_DO_TIMER_REQUEST is called with the rqb       +*/
/*+               spezified (only if rqb is not underway).                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID SNMPX_TimerSet(
    LSA_TIMER_ID_TYPE               TimerID,
    SNMPX_UPPER_RQB_PTR_TYPE        pRQB,
    LSA_VOID                        LSA_FCT_PTR(/*ATTR*/, Cbf)
    (SNMPX_INTERNAL_REQUEST_PTR_TYPE   pIntRQB),
    SNMPX_LOCAL_MEM_U16_PTR_TYPE     pSNMPXTimerID)
{
    SNMPX_TIMER_INFO_PTR_TYPE       pTimer;
    LSA_UINT16                      i;
    LSA_BOOL                        Found;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                          "IN : SNMPX_TimerSet(TimerID: 0x%X, pRQB: 0x%X, Cbf: 0x%X)",
                          TimerID,pRQB,Cbf);

    pTimer  = SNMPX_GET_GLOB_PTR()->Timer;

    i       = 0;
    Found   = LSA_FALSE;

    while (( i <SNMPX_MAX_TIMER ) && ( ! Found ))
    {

        if (! pTimer[i].InUse )
        {
            Found = LSA_TRUE;
            pTimer[i].TimerID       = TimerID;
            pTimer[i].IntRQB.pRQB   = pRQB;
            pTimer[i].IntRQB.InUse  = LSA_FALSE;
            pTimer[i].Cbf           = Cbf;

            /* setup RQB */
            SNMPX_RQB_SET_OPCODE(pRQB,SNMPX_OPC_TIMER);
            pRQB->args.Timer.TimerID = i;  /* SNMPX-Timer ID */

            /* set InUse last to be sure other entrys are */
            /* valid.                                     */
            pTimer[i].InUse         = LSA_TRUE;
            *pSNMPXTimerID          = i;
        };
        i++;
    }

    if ( ! Found )
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"SNMPX_TimerSet(): No more Timer-Resources. SNMPX_CFG_MAX_TIMER wrong?");
        SNMPX_FATAL(SNMPX_FATAL_ERR_TIMER_RESOURCE);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_TimerSet");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname        :   SNMPX_TimerAlloc                               +*/
/*+  Input/Output            LSA_UINT16                      TimerType      +*/
/*+                          LSA_UINT16                      TimerBase      +*/
/*+                          LSA_UINT32                      UserID         +*/
/*+                          LSA_FCT_PTR(ATTR, Cbf) (..)                    +*/
/*+                          SNMPX_LOCAL_MEM_U16_PTR_TYPE    pSNMPXTimerID  +*/
/*+                                                                         +*/
/*+  Result              :   SNMPX_OK                                       +*/
/*+                          SNMPX_ERR_RESOURCE                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  TimerType    LSA-TimerType                                             +*/
/*+               LSA_TIMER_TYPE_ONE_SHOT                                   +*/
/*+               LSA_TIMER_TYPE_CYCLIC                                     +*/
/*+  TimerBase    LSA-TimerBase                                             +*/
/*+               LSA_TIME_BASE_1MS etc                                     +*/
/*+  UserID       UserID put into RQB send on timer-event.                  +*/
/*+  Cbf          Optional Callbackfunction called when timer elapses.      +*/
/*+               Gets pointer to IntRQB as parameter                       +*/
/*+               LSA_NULL for standardhandling                             +*/
/*+  pSNMPXTimerID  SNMPX internal Timer ID (= index ) returned             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates LSA-Timer and setup SNMPX-Timer management      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_TimerAlloc(
    LSA_UINT16                      TimerType,
    LSA_UINT16                      TimerBase,
    LSA_UINT32                      UserID,
    LSA_VOID                        LSA_FCT_PTR(/*ATTR*/, Cbf)
    (SNMPX_INTERNAL_REQUEST_PTR_TYPE   pIntRQB),
    SNMPX_LOCAL_MEM_U16_PTR_TYPE     pSNMPXTimerID)
{
    LSA_UINT16             Response;

    SNMPX_UPPER_RQB_PTR_TYPE pRQB = LSA_NULL;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                          "IN : SNMPX_TimerAlloc(Type: 0x%X, Base: 0x%X, UserID: 0x%X)",
                          TimerType,TimerBase,UserID);

    Response = SNMPX_OK;

    SNMPX_ALLOC_UPPER_RQB_LOCAL(&pRQB,sizeof(SNMPX_RQB_TYPE));

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        Response = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_ERROR,"SNMPX_TimerAlloc(). Allocation RQB memory failed.");
    }
    else
    {
        LSA_UINT16             RetVal  = LSA_RET_OK;
        LSA_TIMER_ID_TYPE      TimerID = 0;

        SNMPX_SYSTEM_TRACE_02(LSA_TRACE_LEVEL_NOTE,"-->: Calling SNMPX_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",TimerType,TimerBase);

        SNMPX_ALLOC_TIMER(&RetVal, &TimerID, TimerType, TimerBase);

        if ( RetVal == LSA_RET_OK)
        {
            SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_NOTE,"<--: Calling SNMPX_ALLOC_TIMER success. TimerID: %d.",TimerID);

            /* setup RQB-user-ID */
            pRQB->args.Timer.UserID  = UserID;

            SNMPX_TimerSet(TimerID,pRQB,Cbf,pSNMPXTimerID);
        }
        else
        {
            Response = SNMPX_ERR_RESOURCE;
            SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_ERROR,"<--: Calling SNMPX_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
            SNMPX_FREE_UPPER_RQB_LOCAL(&RetVal,pRQB);
            SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
        }
    }

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                          "OUT: SNMPX_TimerAlloc(0x%X) SNMPXTimerID: 0x%X",
                          Response,*pSNMPXTimerID);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_TimerFree                             +*/
/*+  Input/Output          :    LSA_UINT16                 SNMPXTimerID     +*/
/*+  Result                :    SNMPX_OK                                    +*/
/*+                             SNMPX_ERR_SEQUENCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXTimerID   SNMPX-Timer ID to free in management                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees timer in timermanagement and releases LSA-Timer     +*/
/*+               Fees internal RQB. If RQB still in use we abort with error+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_TimerFree(LSA_UINT16 SNMPXTimerID)
{
    SNMPX_TIMER_INFO_PTR_TYPE      pTimer;
    LSA_TIMER_ID_TYPE              TimerID;
    LSA_UINT16                     Response;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                          "IN : SNMPX_TimerFree(SNMPXTimerID: 0x%X)",
                          SNMPXTimerID);

    Response = SNMPX_OK;

    if ( SNMPXTimerID < SNMPX_MAX_TIMER )
    {
        pTimer = &SNMPX_GET_GLOB_PTR()->Timer[SNMPXTimerID];

        if ( pTimer->InUse )
        {
            if ( ! pTimer->IntRQB.InUse )
            {
                LSA_UINT16  RetVal = LSA_RET_OK;

                TimerID = pTimer->TimerID;

                SNMPX_STOP_TIMER(&RetVal,TimerID); /* just for sure. dont matter if */
                /* already stopped               */
                SNMPX_FREE_TIMER(&RetVal,TimerID);

                pTimer->InUse = LSA_FALSE;

                /* free internal RQB for Timer */
                SNMPX_FREE_UPPER_RQB_LOCAL(&RetVal,pTimer->IntRQB.pRQB);
                SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

                pTimer->IntRQB.pRQB  = LSA_NULL;
                pTimer->IntRQB.InUse = LSA_FALSE;
            }
            else
            {
                Response = SNMPX_ERR_SEQUENCE;
                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_TimerFree(): Timer-ID(0x%X), RQB still in use!.",SNMPXTimerID);
            }
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_TimerFree(): Invalid Timer-ID(0x%X)",SNMPXTimerID);
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_TimerFree(0x%X)",
                            Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_TimerStart                            +*/
/*+  Input/Output          :    LSA_UINT16                 SNMPXTimerID     +*/
/*+                             LSA_UINT16                 Time             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXTimerID    SNMPX-Timer ID to free in management                   +*/
/*+  Time          running time                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_TimerStart(
    LSA_UINT16 SNMPXTimerID,
    LSA_UINT16 Time)
{
    SNMPX_TIMER_INFO_PTR_TYPE       pTimer;
    LSA_USER_ID_TYPE                Dummy;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_TimerStart(SNMPXTimerID: 0x%X, Time: 0x%X)",
                            SNMPXTimerID,Time);

    if ( SNMPXTimerID < SNMPX_MAX_TIMER )
    {
        pTimer = &SNMPX_GET_GLOB_PTR()->Timer[SNMPXTimerID];

        if ( pTimer->InUse )
        {
            LSA_UINT16 RetVal = LSA_RET_OK;

            Dummy.uvar32 = SNMPXTimerID; /* we use user-id for SNMPX-Timer ID */

            SNMPX_START_TIMER(&RetVal,
                              pTimer->TimerID,
                              Dummy,
                              Time);

            if ( RetVal == LSA_RET_ERR_PARAM )
            {
                SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"<--: Calling SNMPX_START_TIMER failed (Response: 0x%X).",RetVal);
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            }
        }
        else
        {
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_TimerStart(): Timer not in use (0x%X).",SNMPXTimerID);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_TimerStart(): Wrong Timer ID (0x%X).",SNMPXTimerID);
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_TimerStart()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_TimerStop                             +*/
/*+  Input/Output          :    LSA_UINT16                  SNMPXTimerID    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXTimerID   SNMPX-Timer ID to free in management                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts Timer.                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_TimerStop(
    LSA_UINT16 SNMPXTimerID)

{
    LSA_UINT16                     RetVal;
    SNMPX_TIMER_INFO_PTR_TYPE      pTimer;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_TimerStop(SNMPXTimerID: 0x%X)",
                            SNMPXTimerID);

    if ( SNMPXTimerID < SNMPX_MAX_TIMER )
    {
        pTimer = &SNMPX_GET_GLOB_PTR()->Timer[SNMPXTimerID];

        if ( pTimer->InUse )
        {
            SNMPX_STOP_TIMER(&RetVal, pTimer->TimerID);
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_TimerStart(): Wrong Timer ID (0x%X).",SNMPXTimerID);
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_TimerStop()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_TimerRQBFree                          +*/
/*+  Input/Output          :    LSA_UINT16                 SNMPXTimerID     +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXTimerID   SNMPX-Timer ID to free in management                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after RQB was received and is free for further use +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_TimerRQBFree(
    LSA_UINT16 SNMPXTimerID)

{
    SNMPX_TIMER_INFO_PTR_TYPE   pTimer;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_TimerRQBFree(SNMPXTimerID: 0x%X)",
                            SNMPXTimerID);

    pTimer = &SNMPX_GET_GLOB_PTR()->Timer[SNMPXTimerID];
    pTimer->IntRQB.InUse = LSA_FALSE;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_TimerRQBFree()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_TimerRQBCheckFree                     +*/
/*+  Input/Output          :    LSA_UINT16                  SNMPXTimerID    +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXTimerID   SNMPX-Timer ID to free in management                    +*/
/*+                                                                         +*/
/*+  Result : LSA_TRUE: Timer RQB is not in use                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check if Timer RQB is free.                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_BOOL SNMPX_TimerRQBCheckFree(
    LSA_UINT16 SNMPXTimerID)

{
    SNMPX_TIMER_INFO_PTR_TYPE   pTimer;
    LSA_BOOL                    Status;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_TimerRQBCheckFree(SNMPXTimerID: 0x%X)",
                            SNMPXTimerID);

    pTimer = &SNMPX_GET_GLOB_PTR()->Timer[SNMPXTimerID];
    if (pTimer->IntRQB.InUse ) Status = LSA_FALSE;
    else  Status = LSA_TRUE;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_TimerRQBCheckFree(0x%X)",
                            Status);

    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_timeout                               +*/
/*+  Input/Output          :    LSA_UINT16        timer_id                  +*/
/*+                             LSA_USER_ID_TYPE  user_id                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  timer_id     : ID of Timer                                             +*/
/*+  user_id      : Used for SNMPXTimerID                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called from systemadaption on timerevent. May be called   +*/
/*+               within ISR-Handler.                                       +*/
/*+                                                                         +*/
/*+               LSA-Systemfunction                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID snmpx_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id)
{

    SNMPX_TIMER_INFO_PTR_TYPE   pTimer;
    LSA_UINT32                  SNMPXTimerID;

    SNMPXTimerID = (LSA_UINT16) user_id.uvar32;

#ifdef SNMPX_CFG_USE_ENTER_EXIT_TIMEOUT
    SNMPX_ENTER();
#endif

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_timeout(timer_id: 0x%X, user_id: 0x%X)",
                            timer_id,user_id.uvar32);


    if ( SNMPXTimerID < SNMPX_MAX_TIMER )
    {
        pTimer = &SNMPX_GET_GLOB_PTR()->Timer[SNMPXTimerID];

        if (( pTimer->InUse ) &&
            ( pTimer->TimerID == timer_id ))
        {
            /* if a callbackfunction is present, call it */

            if (pTimer->Cbf)
            {
                pTimer->Cbf(&pTimer->IntRQB);
            }
            else /* do standard handling..*/
            {
                if ( ! pTimer->IntRQB.InUse )
                {
                    /* SNMPX_DBG_ACT("Calling SNMPX_DO_TIMER_REQUEST(pRQB: %Xh)\r\n",(LSA_UINT32)pTimer->IntRQB.pRQB,0,0); */
                    /* Note that the RQB is already filled up in startup! */
                    pTimer->IntRQB.InUse = LSA_TRUE;
                    SNMPX_DO_TIMER_REQUEST(pTimer->IntRQB.pRQB);
                }
            }
        }
        else
        {
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"snmpx_timeout(): Invalid Timer-ID (0x%X).",SNMPXTimerID);
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"snmpx_timeout(): Invalid Timer-ID (0x%X).",SNMPXTimerID);
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_timeout()");

#ifdef SNMPX_CFG_USE_ENTER_EXIT_TIMEOUT
    SNMPX_EXIT();
#endif

    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestCyclicTimer                    +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE     pRQB           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     SNMPX_OPC_TIMER                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error       Addition errorcodes from LowerL.+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Timer: SNMPX_RQB_TIMER_TYPE            +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : SNMPX-Timer ID                                           +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from SNMPX within snmpx_timeout().      +*/
/*+               Handles Cyclic Timer event.                               +*/
/*+                                                                         +*/
/*+               Cyclic checks for TimerExpired of snmp receive requests.  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_RequestCyclicTimer(SNMPX_UPPER_RQB_PTR_TYPE pRQB)
{
    SNMPX_HDB_USER_PTR_TYPE      pHDB = SNMPX_GET_HUSER_PTR(0);
    LSA_UINT32                   i;
    LSA_UINT32                   TimerCnt;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestTimer(pRQB: 0x%X)",
                            pRQB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

#if 0  /* check does not work, Timer.UserID=pHDBUser->ThisHandle and Timer.UserID=SNMPX_TIMER_USER_ID_CYLIC overlaps! */
    if (pRQB->args.Timer.UserID != SNMPX_TIMER_USER_ID_CYLIC )
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_RequestCyclicTimer. Invalid Timer-UserID (0x%X)",pRQB->args.Timer.UserID);
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }
#endif

    SNMPX_TimerRQBFree(pRQB->args.Timer.TimerID);

    TimerCnt = ++SNMPX_GET_GLOB_PTR()->CyclicTimerTick;

    /* For all User-Channels we do the timer handling. */

    for (i=0; i<SNMPX_CFG_MAX_CHANNELS; i++, pHDB++)
    {
        LSA_UINT32    SessionIndex;

        /* check if we are in waiting for Timer to close the channel. */
        /* If so we finish the close-handling.                        */
        /* Note: Currently we only have one User-Handle!!             */

        /* only if the CheckInterval (manager) is in use */
        if (snmpx_is_null(pHDB->Params.pDetail) || pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_MANAGER)
        {
            continue;
        }

        switch (pHDB->UserState)
        {
            case SNMPX_HANDLE_STATE_CLOSE_WAIT_TIMER:
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"Timer event while closing. continue.");

                if ( ! LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) )
                {
					SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE Cbf = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
					LSA_SYS_PTR_TYPE             pSys     = pHDB->Params.pSys;
					SNMPX_DETAIL_PTR_TYPE        pDetail  = pHDB->Params.pDetail;
					SNMPX_UPPER_RQB_PTR_TYPE     pRQBUser = pHDB->pOCReq;

                    /* timer RQB is free now. */
                    /* first Cancel the ReceiveRQB's, then close the channel ! */
                    LSA_UINT16 Response = SNMPX_UserCancelReceive(pHDB);
                    if (Response != SNMPX_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);

				    SNMPX_RQB_SET_HANDLE(pRQBUser, pHDB->Params.HandleUpper);

                    /* we release the HDB. Handle close now */
                    SNMPX_UserReleaseHDB(pHDB);

					SNMPX_RQBSetResponse(pRQBUser,Response);
					SNMPX_UPPER_TRACE_04(LSA_TRACE_LEVEL_NOTE,"<<<: Request finished. Opcode: 0x%X, Handle: 0x%Xh,UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),pHDB->ThisHandle,SNMPX_RQB_GET_HANDLE(pRQB),Response);
					SNMPX_CallCbf(Cbf,pRQBUser,pSys);

                    /* release PATH-Info ! */
                    SNMPX_RELEASE_PATH_INFO(&Response,pSys,pDetail);
                    if (Response != LSA_RET_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_RELEASE_PATH_INFO);

                    /* further actions taken in request handler */
                    /* SNMPX_SOCKManagerRequestHandler(pHDB, LSA_NULL, Response); */
                }
                else
                {
                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"pOCReq is NULL. Cannot Close channel");
                    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                }
                break;

            case SNMPX_HANDLE_STATE_OPENED:
				for (SessionIndex=0; SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS; SessionIndex++) /* for all sessions */
				{
					if (pHDB->MngrSession[SessionIndex].PortState == SNMPX_SNMP_STATE_WF_CNF)   /* send request is pending (send.con outstanding) */
					{
						continue;
					}

					if (pHDB->MngrSession[SessionIndex].PortState == SNMPX_SNMP_STATE_OPEN)
					{
                        /* ----------------------------------------------- */
                        /* check for outstanding receive requests          */
                        /* ----------------------------------------------- */
						SNMPX_UPPER_RQB_PTR_TYPE pRQBUser = pHDB->MngrSession[SessionIndex].pUserRQB;

						if(snmpx_is_null(pRQBUser))
						{
							continue;
						}

						/**************** SNMPX_OPC_REQUEST_MANAGER ****************/
						if(SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_REQUEST_MANAGER)
						{
							/* send timeout reached ? */
							if ( pHDB->MngrSession[SessionIndex].NextSend == TimerCnt )
							{
								LSA_UINT16 Response;

								if (pHDB->MngrSession[SessionIndex].RetryCnt == 0)  /* confirm the user */
								{
									/* the response is alerady written in recv.done */
									pHDB->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
									SNMPX_UserRequestFinish(pHDB,pRQBUser,SNMPX_ERR_TIMEOUT);
								}
								else
								{
									Response = SNMPX_DoRequestManager(pRQBUser, pHDB);
									if (Response != SNMPX_OK)   /* stop the retries */
									{
										pHDB->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
										SNMPX_UserRequestFinish(pHDB,pRQBUser,Response);
									}
								}
							}
						}
                    }
                }
                break;

            default:    /* ignore timer */
                break;
        }
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestCyclicTimer()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestTimer                          +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE     pRQB           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     SNMPX_OPC_TIMER                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response    Response                        +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error       Addition errorcodes from LowerL.+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Timer: SNMPX_RQB_TIMER_TYPE            +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : SNMPX-Timer ID                                           +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from SNMPX within snmpx_timeout().      +*/
/*+               Dispatches Timer event.                                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_RequestTimer(SNMPX_UPPER_RQB_PTR_TYPE pRQB)
{
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestTimer(pRQB: 0x%X)",
                            pRQB);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    if (pRQB->args.Timer.TimerID == SNMPX_TIMER_ID_INVALID)
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_RequestTimer. Invalid TimerID (0x%X)",pRQB->args.Timer.TimerID);
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    /* the cyclic timer for snmp receive ? */
    if (SNMPX_GET_GLOB_PTR()->CyclicTimerID == pRQB->args.Timer.TimerID)
    {
        SNMPX_RequestCyclicTimer(pRQB); /* the cyclic timer for snmp receive */

        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestTimer(TimerID (0x%X))", pRQB->args.Timer.TimerID);
        return;
    }

    SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"OUT: SNMPX_RequestTimer. TimerID (0x%X) not found",pRQB->args.Timer.TimerID);
    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_CancelCyclicTimer                     +*/
/*+  Input/Output          :    SNMPX_HDB_USER_PTR_TYPE      pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel handles a delay for PTCP diagnosis   +*/
/*+               and the timer is started -> stop it.                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_CancelCyclicTimer(
    SNMPX_HDB_USER_PTR_TYPE        pHDBUser,
    SNMPX_LOCAL_MEM_BOOL_PTR_TYPE  pFinish
)
{
    LSA_BOOL    Finish    = LSA_TRUE;
    LSA_BOOL    WaitTimer = LSA_FALSE;
    LSA_UINT16  RetVal    = SNMPX_OK;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CancelCyclicTimer(pHDBUser: 0x%X)",
                            pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);
    SNMPX_ASSERT_NULL_PTR(pFinish);

    /* if we are not in state OPEN, a previos attempt to close */
    /* this handle failed and we must not stop the timer again */
    if ( pHDBUser->UserState == SNMPX_HANDLE_STATE_OPENED )
    {
        /* if this is the last user, we stop the timer */
        if ( SNMPX_GET_GLOB_PTR()->CyclicTimerUserCnt == 1 )
        {
            SNMPX_TimerStop(SNMPX_GET_GLOB_PTR()->CyclicTimerID);
            WaitTimer = LSA_TRUE;
        }

        SNMPX_GET_GLOB_PTR()->CyclicTimerUserCnt--;
    }

    /* check if timer RQB is free. if not we have to wait if we are    */
    /* the last one using the timer                                    */
    /* (the timer RQB maybe pending because a timer event occured just */
    /* before we stopped the timer)                                    */
    if ( ! WaitTimer ||
         ( SNMPX_TimerRQBCheckFree(SNMPX_GET_GLOB_PTR()->CyclicTimerID)))
    {
        /* dont need to wait for timer. continue... */
        /* SNMPX_UserChannelStateMachine(pHDB,SNMPX_OK); */
    }
    else
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"Have to wait for Timer before closing channel.");

        /* Request is underway now. check in timer-hanling again and  */
        /* finish the request.                                        */
        Finish = LSA_FALSE; /* a timer request is active */
    }

    *pFinish = Finish;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CancelCyclicTimer(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/*  end of file SNMPX_TIME.C                                                 */
/*****************************************************************************/


