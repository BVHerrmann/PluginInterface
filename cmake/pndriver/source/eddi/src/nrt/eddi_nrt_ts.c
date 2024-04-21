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
/*  F i l e               &F: eddi_nrt_ts.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI: Timestamp          */
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

#if defined (EDDI_CFG_REV5)
#include "eddi_nrt_q.h"
#include "eddi_profile.h"
#include "eddi_swi_ext.h"
#include "eddi_nrt_tx.h"
#endif

#include "eddi_nrt_ts.h"

#define EDDI_MODULE_ID     M_ID_NRT_TS
#define LTRC_ACT_MODUL_ID  209

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTTimeStampInt()                           */
/*                                                                         */
/* D e s c r i p t i o n: executes a TS-interrupt                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTTimeStampInt( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  Caller )
{
    EDDI_NRT_TRACE_01(((pDDB)?pDDB->Glob.TraceIdx:0), LSA_TRACE_LEVEL_NOTE, "EDDI_NRTTimeStampInt->Caller:0x%X", Caller);

    EDDI_Excp("EDDI_NRTTimeStampInt, not allowed with REV6/7", EDDI_FATAL_ERR_EXCP, pDDB, Caller);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif defined (EDDI_CFG_REV5)

#define EDDI_MASK_LISTSTATUS  0x0300

/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTTimeStampInt()                           */
/*                                                                         */
/* D e s c r i p t i o n: executes a TS-interrupt                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTTimeStampInt( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  Caller )
{
    #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pTimeStampIF = pDDB->NRT.pTS_IF;
    #endif

    EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTTimeStampInt->Caller:0x%X", Caller);

    //Caller is always "EDDI_TS_CALLER_TS_INT" here!

    #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
    pTimeStampIF->Tx.LockFct(pTimeStampIF);
    #endif
    EDDI_NRTTimeStamp(pDDB, Caller);
    #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
    pTimeStampIF->Tx.UnLockFct(pTimeStampIF);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTTimeStamp()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTTimeStamp( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                  LSA_UINT32               const  Caller )
{
    EDD_UPPER_NRT_SEND_PTR_TYPE  pRQBSnd;
    EDDI_LOCAL_HDB_PTR_TYPE      pHDB;
    EDD_UPPER_RQB_PTR_TYPE       pActTimeStampReq;
    LSA_UINT16                   PortID;
    LSA_BOOL                     bTwoTSIntWithSameTimeOccured = LSA_FALSE;
    volatile  LSA_UINT32         TimeStamp                    = 0; //satisfy lint!
    LSA_UINT32                   HwPortIndex;

    EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTTimeStamp->Caller:0x%X TS-State:0x%X", Caller, pDDB->NRT.TimeStamp.State);

    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);

    if //TimeStamp-Interrupt?
       (EDDI_TS_CALLER_TS_INT == Caller)
    {
        //read current send-timestamp
        TimeStamp = IO_R32(CLK_COUNT_VALUE_SND);

        if //current send-timestamp is equal to the last send-timestamp?
           (   (pDDB->NRT.TimeStamp.bLastTxTimeStored)
            && (TimeStamp == pDDB->NRT.TimeStamp.LastTxTime))
        {
            EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
            bTwoTSIntWithSameTimeOccured = LSA_TRUE;
            #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
            pDDB->SII.TwoTSIntWithSameTimeCnt++;
            #endif

            #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
            EDDI_LOWER_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTTimeStamp, SPECIAL_TS_TRACE, Zombie TS-INT (2 TS-INT with same timestamp) detected, TimeStamp:0x%X TS-State:0x%X TwoTSIntWithSameTimeCnt:0x%X Mirror-Mode-Register:0x%X ClockCntVal:0x%X",
                                TimeStamp, pDDB->NRT.TimeStamp.State, pDDB->SII.TwoTSIntWithSameTimeCnt, IO_x32(MONITOR_CTRL), IO_R32(CLK_COUNT_VALUE));
            #else
            EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTTimeStamp, Zombie TS-INT (2 TS-INT with same timestamp) detected, TimeStamp:0x%X TS-State:0x%X Mirror-Mode-Register:0x%X",
                              TimeStamp, pDDB->NRT.TimeStamp.State, IO_x32(MONITOR_CTRL));
            #endif
        }
        else //current send-timestamp and last send-timestamp are different
        {
            //store current send-timestamp
            pDDB->NRT.TimeStamp.LastTxTime        = TimeStamp;
            pDDB->NRT.TimeStamp.bLastTxTimeStored = LSA_TRUE;

            #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
            EDDI_LOWER_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTTimeStamp, SPECIAL_TS_TRACE, TS-INT with new TimeStamp detected, TimeStamp:0x%X TS-State:0x%X TwoTSIntWithSameTimeCnt:0x%X Mirror-Mode-Register:0x%X ClockCntVal:0x%X",
                                TimeStamp, pDDB->NRT.TimeStamp.State, pDDB->SII.TwoTSIntWithSameTimeCnt, IO_x32(MONITOR_CTRL), IO_R32(CLK_COUNT_VALUE));
            #else
            EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTTimeStamp, TS-INT with new TimeStamp detected, TimeStamp:0x%X TS-State:0x%X Mirror-Mode-Register:0x%X",
                              TimeStamp, pDDB->NRT.TimeStamp.State, IO_x32(MONITOR_CTRL));
            #endif
        }
    }

    pDDB->NRT.TimeStamp.cTimeStamp = 0; //stop Timeout-Timer

    pActTimeStampReq = pDDB->NRT.TimeStamp.pActReq;

    if (pActTimeStampReq == (EDD_UPPER_RQB_PTR_TYPE)0)
    {
        //no TS-RQB available

        if (   (pDDB->NRT.TimeStamp.State == EDDI_TS_STATE_CHECK_LIST_STATUS)
            || (pDDB->NRT.TimeStamp.State == EDDI_TS_STATE_RECOVER))
        {
            //this interrupt has been delayed -> check if there is still something to do
            pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_NO_REQ;
            EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
            EDDI_NRTReloadTimeStamp(pDDB, Caller);
            return;
        }

        #if defined (EDDI_CFG_DONT_ACCEPT_TS_ZOMBIE_INT)
        //don't accept TS-Zombie-interrupt (TS-INT but no TS-RQB)!
        EDDI_NRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTTimeStamp, no TS-RQB, Zombie TS-INT (TS-INT but no TS-RQB) is not accepted, Caller:0x%X TS-State:0x%X bTwoTSIntWithSameTimeOccured:0x%X Mirror-Mode-Register:0x%X",
                          Caller, pDDB->NRT.TimeStamp.State, (LSA_UINT32)bTwoTSIntWithSameTimeOccured, IO_x32(MONITOR_CTRL));
        EDDI_NRT_TS_COPY_TS_TRACE();
        EDDI_Excp("EDDI_NRTTimeStamp, no TS-RQB, Zombie TS-INT (TS-INT but no TS-RQB) is not accepted, Caller: TS-State:", EDDI_FATAL_ERR_EXCP, Caller, pDDB->NRT.TimeStamp.State);
        #elif !defined (EDDI_CFG_DONT_ACCEPT_TS_ZOMBIE_INT)
        //accept/ignore TS-Zombie-interrupt (TS-INT but no TS-RQB)!
        #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
        EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTTimeStamp, SPECIAL_TS_TRACE, no TS-RQB, Zombie TS-INT (TS-INT but no TS-RQB) is ignored, Caller:0x%X TS-State:0x%X bTwoTSIntWithSameTimeOccured:0x%X Mirror-Mode-Register:0x%X",
                            Caller, pDDB->NRT.TimeStamp.State, (LSA_UINT32)bTwoTSIntWithSameTimeOccured, IO_x32(MONITOR_CTRL));
        #else
        EDDI_NRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTTimeStamp, no TS-RQB, Zombie TS-INT (TS-INT but no TS-RQB) is ignored, Caller:0x%X TS-State:0x%X bTwoTSIntWithSameTimeOccured:0x%X Mirror-Mode-Register:0x%X",
                          Caller, pDDB->NRT.TimeStamp.State, (LSA_UINT32)bTwoTSIntWithSameTimeOccured, IO_x32(MONITOR_CTRL));
        #endif
        #endif
        return;
    }

    //pActTimeStampReq always points to a valid TS-RQB

    pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE)pActTimeStampReq->pParam;
    pHDB    = (EDDI_LOCAL_HDB_PTR_TYPE)pActTimeStampReq->internal_context;

    switch (Caller)
    {
        case EDDI_TS_CALLER_TS_INT: //TimeStamp-Interrupt
        {
            #if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
            if //Mirror-Mode active? (Mirror-Mode can only be activated with compiler-switch "EDDI_CFG_MIRROR_MODE_INCLUDE")
               (IO_x32(MONITOR_CTRL))
            {
                EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
                EDD_RQB_SET_RESPONSE(pActTimeStampReq, EDD_STS_ERR_NOT_SUPPORTED);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_MIRROR_MODE);
                //return immediately
                break;
            }
            #endif

            if (bTwoTSIntWithSameTimeOccured)
            {
                #if defined (EDDI_CFG_DONT_ACCEPT_TS_ZOMBIE_INT)
                //don't accept TS-Zombie-interrupt (2 TS-INT with same timestamp)!
                EDDI_NRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTTimeStamp, TS-RQB available, Zombie TS-INT (2 TS-INT with same timestamp) is not accepted, Caller:0x%X TS-State:0x%X CurrentTS:0x%X Mirror-Mode-Register:0x%X",
                                  Caller, pDDB->NRT.TimeStamp.State, TimeStamp, IO_x32(MONITOR_CTRL));
                EDDI_NRT_TS_COPY_TS_TRACE();
                EDDI_Excp("EDDI_NRTTimeStamp, TS-RQB available, Zombie TS-INT (2 TS-INT with same timestamp) is not accepted, TS-State: Caller:", EDDI_FATAL_ERR_EXCP, pDDB->NRT.TimeStamp.State, Caller);
                #elif !defined (EDDI_CFG_DONT_ACCEPT_TS_ZOMBIE_INT)
                //accept/ignore TS-Zombie-interrupt (2 TS-INT with same timestamp)!
                #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
                EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTTimeStamp, SPECIAL_TS_TRACE, TS-RQB available, Zombie TS-INT (2 TS-INT with same timestamp) is ignored, Caller:0x%X TS-State:0x%X Mirror-Mode-Register:0x%X",
                                    Caller, pDDB->NRT.TimeStamp.State, IO_x32(MONITOR_CTRL));
                #else
                EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTTimeStamp, TS-RQB available, Zombie TS-INT (2 TS-INT with same timestamp) is ignored, Caller:0x%X TS-State:0x%X Mirror-Mode-Register:0x%X",
                                  Caller, pDDB->NRT.TimeStamp.State, IO_x32(MONITOR_CTRL));
                #endif
                #endif
                return;
            }

            //reading send-timestamp already executed above
            pRQBSnd->TxTime = TimeStamp;
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NRTTimeStamp, store Timestamp in TS-RQB, Timestamp:0x%X", TimeStamp);

            switch (pDDB->NRT.TimeStamp.State)
            {
                case EDDI_TS_STATE_REQ_STARTED_AWAIT_INTS:
                {
                    pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_TS_INT_AWAIT_TX_DONE;
                    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
                    //to be continued in TxDone Interrupt ...
                    return;
                }

                case EDDI_TS_STATE_TX_DONE_AWAIT_TS_INT_OR_TIMEOUT:
                {
                    //TS-RQB is ready and can be send back
                    break;
                }

                case EDDI_TS_STATE_CHECK_LIST_STATUS:
                case EDDI_TS_STATE_RECOVER:
                {
                    //can happen, eg. 10MBit, cycle 125 us
                    pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_NO_REQ;
                    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
                    EDD_RQB_SET_RESPONSE(pActTimeStampReq, EDD_STS_OK);
                    break;
                }

                case EDDI_TS_STATE_CLOSING:
                {
                    //Close running -> TS-RQB was already sent back
                    return;
                }

                case EDDI_TS_STATE_NO_REQ:
                case EDDI_TS_STATE_TS_INT_AWAIT_TX_DONE:
                default:
                {
                    EDDI_NRT_TS_COPY_TS_TRACE();
                    EDDI_Excp("EDDI_NRTTimeStamp, invalid TS-State:", EDDI_FATAL_ERR_EXCP, pDDB->NRT.TimeStamp.State, 0);
                    return;
                }
            }

            break;
        }

        case EDDI_TS_CALLER_TIMEOUT:
        {
            HwPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, pRQBSnd->PortID);
            EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTTimeStamp->EDD_STS_ERR_TX, EDDI_TS_CALLER_TIMEOUT, HwPortIndex:%u, pDDB->NRT.TimeStamp.State:0x%X",
                    HwPortIndex, pDDB->NRT.TimeStamp.State);
            pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_CHECK_LIST_STATUS;
            pDDB->NRT.TimeStamp.HwPortIndex = HwPortIndex;
            EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
            EDD_RQB_SET_RESPONSE(pActTimeStampReq, EDD_STS_ERR_TX);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_TIMEOUT);
            //back, but the request can still be announced later by interrupt
            pDDB->NRT.TimeStamp.pActReq = EDDI_NULL_PTR;
            break;
        }

        case EDDI_TS_CALLER_LINK_DOWN:
        {
            HwPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, pRQBSnd->PortID);
            EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTTimeStamp->EDD_STS_ERR_TX, EDDI_TS_CALLER_LINK_DOWN, HwPortIndex:%u, pDDB->NRT.TimeStamp.State:0x%X",
                    HwPortIndex, pDDB->NRT.TimeStamp.State);
            EDD_RQB_SET_RESPONSE(pActTimeStampReq, EDD_STS_ERR_TX);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_LINK_DOWN);
            //return immediately. This should be no problem, as the request has never been turned over to IRTE.
            break;
        }

        #if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
        case EDDI_TS_CALLER_MIRROR_MODE:
        {
            EDD_RQB_SET_RESPONSE(pActTimeStampReq, EDD_STS_ERR_NOT_SUPPORTED);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_MIRROR_MODE);
            //return immediately. This should be no problem, as the request has never been turned over to IRTE.
            break;
        }
        #endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

        case EDDI_TS_CALLER_TX_DONE:
        {
            //This code is only called, if TimeStamp-Interrupt had already occured! The order of NRTTxDone-Interrupt and TimeStamp-Interrupt is not deterministic!
            //TS-RQB is ready and can be send back
            break;
        }

        default:
        {
            EDDI_NRT_TS_COPY_TS_TRACE();
            EDDI_Excp("EDDI_NRTTimeStamp, invalid Caller:", EDDI_FATAL_ERR_EXCP, Caller, 0);
            return;
        }
    } //end of "switch (Caller)"

    if (0 == pHDB->Cbf)
    {
        EDDI_Excp("EDDI_NRTTimeStamp, no Cbf", EDDI_FATAL_ERR_EXCP, Caller, 0);
        return;
    }

    PortID = pRQBSnd->PortID;

    if ((PortID != EDD_PORT_ID_AUTO) && (PortID <= EDDI_MAX_IRTE_PORT_CNT))
    {
        HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[PortID];

        pRQBSnd->LineDelayInTicksHardware = pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LineDelayInTicksHardware;
    }
    else
    {
        pRQBSnd->LineDelayInTicksHardware = 0xFFFFFFFFUL;
    }

    EDD_RQB_SET_HANDLE(pActTimeStampReq, pHDB->UpperHandle);
    EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pActTimeStampReq, pHDB->pSys);
   
    pHDB->TxCount--;

    if (    (0 == pHDB->TxCount)
        &&  (0 == pHDB->intRxRqbCount)
        &&  (0 == pHDB->intTxRqbCount)
        &&  (pHDB->pRQBTxCancelPending) )
    {
        EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

        pHDB->pRQBTxCancelPending = LSA_NULL;

        //Inform User thats all Frames are sent. The TxCancel can give back now!
        EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
        EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxSendQueues1If-> Cancel in Progress <---");
    }
    else if (pHDB->TxCount < 0)
    {
        EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
    }

    EDDI_NRTReloadTimeStamp(pDDB, Caller);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTReloadTimeStamp()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReloadTimeStamp( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  Caller )
{
    EDDI_LOCAL_HDB_PTR_TYPE     pHDB;
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;
    EDD_UPPER_RQB_PTR_TYPE      pActTimeStampReq;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTReloadTimeStamp->");

    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);

    if (EDDI_TS_STATE_CHECK_LIST_STATUS == pDDB->NRT.TimeStamp.State)
    {
        LSA_UINT32  const  HwPortIndex = pDDB->NRT.TimeStamp.HwPortIndex;
        LSA_UINT32         ListStatus;

        if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus != EDD_LINK_DOWN)
        {
            //is there something in the port sendlist?
            ListStatus = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_LIST_STATUS_Px, pDDB);

            if (ListStatus & EDDI_MASK_LISTSTATUS) //Bit 8 or 9 -> only these priorities (ORG) can block the NRT linedelay measurement
            {
                //return everything with status "Timeout"
                EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTReloadTimeStamp->EDD_STS_ERR_TX, HwPortIndex:%u, ListStatus:0x%X",
                        HwPortIndex, ListStatus);
                for (;;)
                {
                    pActTimeStampReq = EDDI_RemoveFromQueue(pDDB, &pDDB->NRT.TimeStamp.ReqQueue);
                    if (pActTimeStampReq == (EDD_UPPER_RQB_PTR_TYPE)0)
                    {
                        return; //no more TS-rqb available!
                    }
                    pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pActTimeStampReq->internal_context;
                    EDD_RQB_SET_RESPONSE(pActTimeStampReq, EDD_STS_ERR_TX);
                    EDD_RQB_SET_HANDLE(pActTimeStampReq, pHDB->UpperHandle);
                    EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pActTimeStampReq, pHDB->pSys);

                    pHDB->TxCount--;

                    if (    (0 == pHDB->TxCount)
                        &&  (0 == pHDB->intRxRqbCount)
                        &&  (0 == pHDB->intTxRqbCount)
                        &&  (pHDB->pRQBTxCancelPending)  )
                    {
                        EDD_UPPER_RQB_PTR_TYPE const pRQB_tmp = pHDB->pRQBTxCancelPending;

                        pHDB->pRQBTxCancelPending = LSA_NULL;

                        //Inform User thats all Frames are sent. The TxCancel can give back now!
                        EDDI_RequestFinish(pHDB, pRQB_tmp, EDD_STS_OK);
                        EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtCleanupTxSendQueues1If-> Cancel in Progress <---");
                    }
                    else if (pHDB->TxCount < 0)
                    {
                        EDDI_Excp("FATAL, pHDB->TxCount is negativ", EDDI_FATAL_ERR_EXCP, 0, 0);
                    }
                }
            }

            //wait. Theoretically the TimeStamp-Int can be pending and has just not been handled -> released with EDDI_NRTTimeStampTimeOut()
            return;
        }
    }

    pActTimeStampReq = EDDI_RemoveFromQueue(pDDB, &pDDB->NRT.TimeStamp.ReqQueue);
    pDDB->NRT.TimeStamp.pActReq = pActTimeStampReq;
    if (pActTimeStampReq == (EDD_UPPER_RQB_PTR_TYPE)0)
    {
        pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_NO_REQ;
        EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
        return; //nothing more to handle!
    }

    pDDB->NRT.TimeStamp.State = EDDI_TS_STATE_REQ_STARTED_AWAIT_INTS;
    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);

    pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pActTimeStampReq->internal_context;
    pIF  = pHDB->pIF;

    #if defined (EDDI_CFG_FRAG_ON)
    #error "REV5 Timestamp handling not implemented here in connection with TX fragmentation!"
    #endif

    EDDI_NrtAddToPrioQueue(pDDB, pIF, pActTimeStampReq);

    //When Caller=EDDI_TS_CALLER_LINK_DOWN or EDDI_TS_CALLER_MIRROR_MODE, Tx-Send-Function is already running! Avoid recursive function-call!
    #if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
    if ((EDDI_TS_CALLER_LINK_DOWN != Caller) && (EDDI_TS_CALLER_MIRROR_MODE != Caller))
    #else
    if (EDDI_TS_CALLER_LINK_DOWN != Caller)
    #endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE
    {
        EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTTimeStampTimeOut()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTTimeStampTimeOut( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB   = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    LSA_UINT32               const  Caller = EDDI_TS_CALLER_TIMEOUT;

    EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTTimeStampTimeOut->TS-State:0x%X, HwPortIndex:%u", 
                      pDDB->NRT.TimeStamp.State, pDDB->NRT.TimeStamp.HwPortIndex);

    EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);

    switch (pDDB->NRT.TimeStamp.State)
    {
        case EDDI_TS_STATE_TX_DONE_AWAIT_TS_INT_OR_TIMEOUT:
        {
            if (pDDB->NRT.TimeStamp.cTimeStamp > pDDB->NRT.TimeStamp.Timeout)
            {
                EDDI_NRTTimeStamp(pDDB, Caller);
                pDDB->NRT.TimeStamp.cTimeStamp = pDDB->NRT.TimeStamp.Timeout + 1UL; //Restart Timeout-Timer. From now on Timer will occur every NewCycle!
            }
            else
            {
                pDDB->NRT.TimeStamp.cTimeStamp++;
            }
            break;
        }

        case EDDI_TS_STATE_CHECK_LIST_STATUS:
        {
            LSA_UINT32  const  HwPortIndex = pDDB->NRT.TimeStamp.HwPortIndex;
            LSA_UINT32         ListStatus;

            //is there something in the PortSendList?
            ListStatus = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_LIST_STATUS_Px, pDDB);

            if (ListStatus & EDDI_MASK_LISTSTATUS) //Bit 8 oder 9 (Prio ORG) -> only these can block NRT linedelay measurement
            {
                break; //leave Timeout-Timer activated (will occur each NewCycle from now on)
            }

            //release blocking with the next timer event
            pDDB->NRT.TimeStamp.cTimeStamp = 1; //restart Timeout-Timer
            pDDB->NRT.TimeStamp.State      = EDDI_TS_STATE_RECOVER;
            EDDI_NRT_TS_TRACE_ENTRY(pDDB, EDDI_FILE_INDEX_NRT_TS_C, Caller, pDDB->NRT.TimeStamp.State);
            break;
        }

        case EDDI_TS_STATE_RECOVER:
        {
            //now we are ready to continue -> if we were waiting for an interrupt, it has arrived now
            pDDB->NRT.TimeStamp.cTimeStamp = 0; //stop Timeout-Timer
            EDDI_NRTReloadTimeStamp(pDDB, Caller);
            break;
        }

        case EDDI_TS_STATE_NO_REQ:
        case EDDI_TS_STATE_REQ_STARTED_AWAIT_INTS:
        case EDDI_TS_STATE_TS_INT_AWAIT_TX_DONE:
        case EDDI_TS_STATE_CLOSING:
        default:
            break;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CopyTimestampTraceToLSATrace()              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_NRT_TS_TRACE_DEPTH)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CopyTimestampTraceToLSATrace( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Ctr;

    EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CopyTimestampTraceToLSATrace, TS-Trace (beginning with index 0), NextIndex:0x%X TraceDepth:0x%X",
                      pDDB->NRT.TimeStampTrace.NextIndex, (LSA_UINT32)EDDI_CFG_NRT_TS_TRACE_DEPTH);

    for (Ctr = 0; Ctr < EDDI_CFG_NRT_TS_TRACE_DEPTH; Ctr++)
    {
        #if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
        EDDI_NRT_TS_TRACE_ENTRY_TYPE  *  const  pTraceEntry = &(pDDB->NRT.TimeStampTrace.Trace[Ctr]);
        #endif
        EDDI_NRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CopyTimestampTraceToLSATrace, TS-Trace-Entry, ClockCntVal:0x%X Detail:0x%X Location:0x%X Caller:0x%X TimeStampState:0x%X", 
                          pTraceEntry->ClockCntVal, pTraceEntry->Detail, pTraceEntry->Location, pTraceEntry->Caller, pTraceEntry->TimeStampState);
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_REV5


/*****************************************************************************/
/*  end of file eddi_nrt_ts.c                                                */
/*****************************************************************************/



