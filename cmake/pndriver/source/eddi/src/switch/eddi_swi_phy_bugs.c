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
/*  F i l e               &F: eddi_swi_phy_bugs.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
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
#include "eddi_swi_ext.h"
#include "eddi_ext.h"
#include "eddi_Tra.h"
#include "eddi_sync_usr.h"
#include "eddi_prm_state.h"

#define EDDI_MODULE_ID     M_ID_SWI_PHY_BUGS
#define LTRC_ACT_MODUL_ID  319

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugTimeout( SWI_PHY_BUGS_PORT  *  const  pBugsPort );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugsPowerUp( LSA_VOID  *  const  context );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugResetValuesDummy( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex,
                                                                       SER_SWI_LINK_PTR_TYPE    const  pEDDILinkStatusPx );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugResetValues( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex,
                                                                  SER_SWI_LINK_PTR_TYPE    const  pEDDILinkStatusPx );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugTimer( LSA_VOID  *  const  context );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugTimerDummy( LSA_VOID  *  const  context );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugsForceLinkDown( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                     SWI_PHY_BUGS_PORT        *  const  pBugsPort );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugFireIndication( SWI_PHY_BUGS_PORT  *  const  pBugsPort );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugAction( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                             SWI_PHY_BUGS_PORT        *  const  pBugsPort );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SetPhyReset( LSA_UINT32               const  HwPortIndex,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#define MYSTIC_COM_ERR_PHY_REG26   26

#define EDDI_PHY_BUGS_START_DELAY_MS   (100 * 1000 * 400)   // 400ms

#define EDDI_BUG_LINK_UP             0x10
#define EDDI_BUG_LINK_DOWN_SIMULATE  0x11
#define EDDI_BUG_LINK_UP_FIRE_IND    0x12
#define EDDI_BUG_LINK_DOWN_NORMAL    0x13
#define EDDI_BUG_LINK_UP_10MBIT      0x14
#define EDDI_BUG_LINK_UP_OPTICAL     0x15

#define EDDI_SER10_FLOW_CTRL_BIT__SR                EDDI_BIT_MASK_PARA(0, 0)
#define EDDI_SER10_FLOW_CTRL_BIT__PORT0             EDDI_BIT_MASK_PARA(1, 1)
#define EDDI_SER10_FLOW_CTRL_BIT__PORT1             EDDI_BIT_MASK_PARA(2, 2)
#define EDDI_SER10_FLOW_CTRL_BIT__PORT2             EDDI_BIT_MASK_PARA(3, 3)
#define EDDI_SER10_FLOW_CTRL_BIT__PORT3             EDDI_BIT_MASK_PARA(4, 4)

//temporarily_disabled_lint -esym(754, _EDDI_SER10_FLOW_CTRL_BIT*)


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiIniPhyBugs()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiIniPhyBugs( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                   EDDI_RQB_CMP_PHY_BUGFIX_INI_TYPE  *  const  pPhyBugs )
{
    SWI_PHY_BUGS       *  const  pBugs = &pDDB->pLocal_SWITCH->PhyBugs;
    LSA_UINT32                   UsrPortIndex;
    LSA_RESULT                   Status;
    SWI_PHY_BUGS_PORT         *  pBugsPort;
    LSA_UINT32            const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiIniPhyBugs->");

    //At startup everything is switched off
    pBugs->EDDI_SwiPhyBugTimerFct = EDDI_SwiPhyBugTimerDummy;
    pBugs->SwiPhyBugsResetValFct  = EDDI_SwiPhyBugResetValuesDummy;

    if (pDDB->Glob.bPhyExtern)
    {
        return;
    }

    if (pDDB->ERTEC_Version.Location != EDDI_LOC_ERTEC200)
    {
        return;
    }

    if (EDD_FEATURE_ENABLE != pPhyBugs->MysticCom.bActivate)
    {
        return;
    }

    pBugs->EDDI_SwiPhyBugTimerFct = EDDI_SwiPhyBugTimer;
    pBugs->SwiPhyBugsResetValFct  = EDDI_SwiPhyBugResetValues;

    pDDB->pLocal_SWITCH->PhyBugs.Fix = *pPhyBugs;

    if (EDD_FEATURE_ENABLE != pPhyBugs->MysticCom.bReplaceDefaultSettings)
    {
        pPhyBugs->MysticCom.Diff_ESD              = 15;            // Triggerlevel for ESD detection
        pPhyBugs->MysticCom.T_LinkDown_NoRcv_InMs = 1000 * 60 * 3; // 3 Min
        pPhyBugs->MysticCom.Tp_RcvFrame_InMs      = 30;            // 30 ms
    }

    pBugs->Fix = *pPhyBugs;

    pBugs->Tp_RcvFrame_in_10ns = pBugs->Fix.MysticCom.Tp_RcvFrame_InMs * 1000 * 100; //Conversion ms to ns

    if (pBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs != 0)
    {
        if (pBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs <= pBugs->Fix.MysticCom.Tp_RcvFrame_InMs)
        {
            EDDI_Excp("EDDI_SwiIniPhyBugs, pBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs <= pBugs->Fix.MysticCom.Tp_RcvFrame_InMs",
                      EDDI_FATAL_ERR_EXCP, pBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs, pBugs->Fix.MysticCom.Tp_RcvFrame_InMs);
            return;
        }
    }

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pBugsPort                      = &pBugs->port[HwPortIndex];
        pBugsPort->HwPortIndex         = HwPortIndex;
        pBugsPort->pDDB                = pDDB;
        pBugsPort->LinkStatus          = EDDI_BUG_LINK_DOWN_NORMAL;
        pBugsPort->bPhyBugs_WA_Running = LSA_FALSE;
        pBugsPort->bPhyReset           = LSA_FALSE;
        pBugsPort->pPhyResetRQB        = LSA_NULL;
               
        Status = EDDI_AllocTimer(pDDB,
                                 &pBugsPort->PowerUpDelayTimerID,
                                 pBugsPort,
                                 (EDDI_TIMEOUT_CBF)EDDI_SwiPhyBugsPowerUp,
                                 EDDI_TIMER_TYPE_ONE_SHOT,
                                 EDDI_TIME_BASE_100MS,
                                 EDDI_TIMER_REST);

        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_SwiIniPhyBugs, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, UsrPortIndex);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugTimer()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugTimer( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB     = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    SWI_PHY_BUGS             *  const  pPhyBugs = &pDDB->pLocal_SWITCH->PhyBugs;
    LSA_UINT32                         diff;
    LSA_UINT32                         UsrPortIndex;
    SWI_PHY_BUGS_PORT               *  pBugsPort;
    LSA_UINT32                  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugTimer->");

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pBugsPort = &pPhyBugs->port[HwPortIndex];

        switch (pBugsPort->LinkStatus)
        {
            case EDDI_BUG_LINK_DOWN_NORMAL:
            case EDDI_BUG_LINK_DOWN_SIMULATE:
            case EDDI_BUG_LINK_UP_OPTICAL:
            case EDDI_BUG_LINK_UP_10MBIT:
                continue;

            default:
                break;
        }

        if (pDDB->ClkCounter_in_10ns_ticks_NewCycle > pBugsPort->LastTicks_in_10ns)
        {
            diff = pDDB->ClkCounter_in_10ns_ticks_NewCycle - pBugsPort->LastTicks_in_10ns;
        }
        else
        {
            diff = pBugsPort->LastTicks_in_10ns - pDDB->ClkCounter_in_10ns_ticks_NewCycle;
        }

        if (diff > pBugsPort->ActTimerTicks_in_10ns)
        {
            pBugsPort->LastTicks_in_10ns = pDDB->ClkCounter_in_10ns_ticks_NewCycle;

            EDDI_SwiPhyBugAction(pDDB, pBugsPort);
        }
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugTimer<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugAction()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugAction( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                             SWI_PHY_BUGS_PORT         *  const  pBugsPort )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugAction->");

    EDDI_SwiPhyBugTimeout(pBugsPort);

    if (pBugsPort->LinkStatus == EDDI_BUG_LINK_UP_FIRE_IND)
    {
        EDDI_SwiPhyBugFireIndication(pBugsPort);
    }

    LSA_UNUSED_ARG(pDDB);
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugAction<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugTimerDummy()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugTimerDummy( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPhyBugTimerDummy->");

    LSA_UNUSED_ARG(pDDB);
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPhyBugTimerDummy<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugTimeout()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugTimeout( SWI_PHY_BUGS_PORT  *  const  pBugsPort )
{
    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB     = pBugsPort->pDDB;
    SWI_PHY_BUGS             *  const  pPhyBugs = &pDDB->pLocal_SWITCH->PhyBugs;
    LSA_UINT32                         diff;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugTimeout->");

    pBugsPort->Rcv_New = IO_R32(ERTEC_RegAdr[REG_STAT_CTRL_GFRX].HWPort[pBugsPort->HwPortIndex]);

    EDDI_SwiPhyReadMDCA(MYSTIC_COM_ERR_PHY_REG26, pDDB->Glob.PortParams[pBugsPort->HwPortIndex].PhyAdr, &pBugsPort->RxErr_New, pDDB);

    pBugsPort->RxErr_New = EDDI_IRTE2HOST32(pBugsPort->RxErr_New);

    if (pBugsPort->RxErr_New > pBugsPort->RxErr_Old)
    {
        diff = pBugsPort->RxErr_New - pBugsPort->RxErr_Old;
    }
    else
    {
        diff = pBugsPort->RxErr_Old - pBugsPort->RxErr_New;
    }

    if (diff > 60) // Jump if a long cable is broken -> counter will not count any more
                   // Link Down Interrupt will come much later
    {
        EDDI_SWI_TRACE_12(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugTimeout, diff > 60, BugHwPortIndex:0x%X diff:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X T_LinkDown_NoRcv_InMs:0x%X Diff_ESD:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                          pBugsPort->HwPortIndex,
                          diff,
                          pBugsPort->RxErr_New,
                          pBugsPort->RxErr_Old,
                          pBugsPort->RxErr_Start,
                          pBugsPort->t_NoRcvInMs,
                          pBugsPort->Rcv_New,
                          pBugsPort->Rcv_Old,
                          pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs,
                          pPhyBugs->Fix.MysticCom.Diff_ESD,
                          pBugsPort->EDDILinkStatusPx.LinkStatus,
                          pBugsPort->LinkStatus);

        EDDI_SwiPhyBugsForceLinkDown(pDDB, pBugsPort);
        return;
    }

    if (pBugsPort->Rcv_New != pBugsPort->Rcv_Old)
    {
        if (diff > 0)
        {
            EDDI_SWI_TRACE_12(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugTimeout, diff > 0, BugHwPortIndex:0x%X diff:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X T_LinkDown_NoRcv_InMs:0x%X Diff_ESD:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                              pBugsPort->HwPortIndex,
                              diff,
                              pBugsPort->RxErr_New,
                              pBugsPort->RxErr_Old,
                              pBugsPort->RxErr_Start,
                              pBugsPort->t_NoRcvInMs,
                              pBugsPort->Rcv_New,
                              pBugsPort->Rcv_Old,
                              pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs,
                              pPhyBugs->Fix.MysticCom.Diff_ESD,
                              pBugsPort->EDDILinkStatusPx.LinkStatus,
                              pBugsPort->LinkStatus);
        }

        if (diff == 0) //Error occured already but with a ESD disturbance too small. Retrigger timer.
        {
            pBugsPort->RxErr_Start = pBugsPort->RxErr_New;
        }

        //Everything received ok
        pBugsPort->Rcv_Old     = pBugsPort->Rcv_New;
        pBugsPort->RxErr_Old   = pBugsPort->RxErr_New;
        pBugsPort->t_NoRcvInMs = 0;
        return;
    }

    if (diff != 0)
    {
        if (diff > pPhyBugs->Fix.MysticCom.Diff_ESD)
        {
            EDDI_SWI_TRACE_12(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugTimeout, diff > 0, BugHwPortIndex:0x%X diff:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X T_LinkDown_NoRcv_InMs:0x%X Diff_ESD:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                              pBugsPort->HwPortIndex,
                              diff,
                              pBugsPort->RxErr_New,
                              pBugsPort->RxErr_Old,
                              pBugsPort->RxErr_Start,
                              pBugsPort->t_NoRcvInMs,
                              pBugsPort->Rcv_New,
                              pBugsPort->Rcv_Old,
                              pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs,
                              pPhyBugs->Fix.MysticCom.Diff_ESD,
                              pBugsPort->EDDILinkStatusPx.LinkStatus,
                              pBugsPort->LinkStatus);

            EDDI_SwiPhyBugsForceLinkDown(pDDB, pBugsPort);
            return;
        }

        EDDI_SWI_TRACE_12(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugTimeout, diff <> 0, BugHwPortIndex:0x%X diff:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X T_LinkDown_NoRcv_InMs:0x%X Diff_ESD:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                          pBugsPort->HwPortIndex,
                          diff,
                          pBugsPort->RxErr_New,
                          pBugsPort->RxErr_Old,
                          pBugsPort->RxErr_Start,
                          pBugsPort->t_NoRcvInMs,
                          pBugsPort->Rcv_New,
                          pBugsPort->Rcv_Old,
                          pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs,
                          pPhyBugs->Fix.MysticCom.Diff_ESD,
                          pBugsPort->EDDILinkStatusPx.LinkStatus,
                          pBugsPort->LinkStatus);

        //ESD disturbance
        pBugsPort->RxErr_Old = pBugsPort->RxErr_New;
        return;
    }

    if (   (pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs == 0)
        || (pBugsPort->RxErr_New == pBugsPort->RxErr_Start))
    {
        //switched off or no problem
        return;
    }

    EDDI_SWI_TRACE_12(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugTimeout 4, BugHwPortIndex:0x%X diff:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X T_LinkDown_NoRcv_InMs:0x%X Diff_ESD:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                      pBugsPort->HwPortIndex,
                      diff,
                      pBugsPort->RxErr_New,
                      pBugsPort->RxErr_Old,
                      pBugsPort->RxErr_Start,
                      pBugsPort->t_NoRcvInMs,
                      pBugsPort->Rcv_New,
                      pBugsPort->Rcv_Old,
                      pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs,
                      pPhyBugs->Fix.MysticCom.Diff_ESD,
                      pBugsPort->EDDILinkStatusPx.LinkStatus,
                      pBugsPort->LinkStatus);

    //potential PHY hangup. A frame has to be received soon or...
    pBugsPort->t_NoRcvInMs += pPhyBugs->Fix.MysticCom.Tp_RcvFrame_InMs;

    if (pBugsPort->t_NoRcvInMs > pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs)
    {
        EDDI_SWI_TRACE_12(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugTimeout 5, BugHwPortIndex:0x%X diff:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X T_LinkDown_NoRcv_InMs:0x%X Diff_ESD:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                          pBugsPort->HwPortIndex,
                          diff,
                          pBugsPort->RxErr_New,
                          pBugsPort->RxErr_Old,
                          pBugsPort->RxErr_Start,
                          pBugsPort->t_NoRcvInMs,
                          pBugsPort->Rcv_New,
                          pBugsPort->Rcv_Old,
                          pPhyBugs->Fix.MysticCom.T_LinkDown_NoRcv_InMs,
                          pPhyBugs->Fix.MysticCom.Diff_ESD,
                          pBugsPort->EDDILinkStatusPx.LinkStatus,
                          pBugsPort->LinkStatus);

        return;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugTimeout<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugFireIndication()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugFireIndication( SWI_PHY_BUGS_PORT  *  const  pBugsPort )
{
    SWI_LINK_PARAM_TYPE               *  pLinkPx;
    LSA_UINT32                           Flow;
    EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB            = pBugsPort->pDDB;
    SWI_PHY_BUGS               *  const  pPhyBugs        = &pDDB->pLocal_SWITCH->PhyBugs;
    LSA_UINT32                    const  BugsHwPortIndex = pBugsPort->HwPortIndex;
    LSA_BOOL                             PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugFireIndication->");

    Flow = IO_R32(FLOW_CTRL);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugFireIndication 1, pBugsPort->HwPortIndex:0x%X Flow:0x%X",
                      BugsHwPortIndex, Flow);

    switch (BugsHwPortIndex)
    {
        case 0:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT0, 1);
            break;
        case 1:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT1, 1);
            break;
        case 2:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT2, 1);
            break;
        case 3:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT3, 1);
            break;
        default:
        {
            EDDI_Excp("EDDI_SwiPhyBugFireIndication, pBugsPort->HwPortIndex", EDDI_FATAL_ERR_EXCP, BugsHwPortIndex, 0);
            return;
        }
    }

    //Reset the Pause bit for ALL PORTS!
    IO_x32(FLOW_CTRL) = Flow;

    pBugsPort->ActTimerTicks_in_10ns = pPhyBugs->Tp_RcvFrame_in_10ns;

    EDDI_SWI_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugFireIndication, BugHwPortIndex:0x%X BugLinkStatus:0x%X EDDILinkStatus:0x%X EDDILinkSpeed:0x%X pBugsPort->HwPortIndex:0x%X Flow:0x%X",
                      BugsHwPortIndex, pBugsPort->LinkStatus,
                      pBugsPort->EDDILinkStatusPx.LinkStatus, pBugsPort->EDDILinkStatusPx.LinkSpeed,
                      BugsHwPortIndex, Flow);

    pBugsPort->LinkStatus = EDDI_BUG_LINK_UP;

    //Take over stored values
    pLinkPx             = &pDDB->pLocal_SWITCH->LinkPx[BugsHwPortIndex];
    pLinkPx->LinkStatus = pBugsPort->EDDILinkStatusPx.LinkStatus;
    pLinkPx->LinkSpeed  = pBugsPort->EDDILinkStatusPx.LinkSpeed;
    pLinkPx->LinkMode   = pBugsPort->EDDILinkStatusPx.LinkMode;

    //inform port state machines
    EDDI_SyncPortStmsLinkChange(pDDB,
                                BugsHwPortIndex,
                                pLinkPx->LinkStatus,
                                pLinkPx->LinkSpeed,
                                pLinkPx->LinkMode);

    PortChanged[BugsHwPortIndex] = LSA_TRUE;

    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugFireIndication<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugsPowerUp()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugsPowerUp( LSA_VOID  *  const  context )
{
    SWI_PHY_BUGS_PORT        *  const  pBugsPort = (SWI_PHY_BUGS_PORT *)context;
    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB      = pBugsPort->pDDB;
    LSA_RESULT                         Status;
    LSA_UINT32                         Flow;
    LSA_UINT32                         Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugsPowerUp->");

    Flow = IO_R32(FLOW_CTRL);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugsPowerUp 1, Flow:0x%X BugLinkStatus:0x%X",
                      Flow, pBugsPort->LinkStatus);

    EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__SR, 1);

    switch (pBugsPort->HwPortIndex)
    {
        case 0:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT0, 1);
            break;
        case 1:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT1, 1);
            break;
        case 2:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT2, 1);
            break;
        case 3:
            EDDI_SetBitField32(&Flow, EDDI_SER10_FLOW_CTRL_BIT__PORT3, 1);
            break;
        default:
        {
            EDDI_Excp("EDDI_SwiPhyBugsPowerUp-> pBugsPort->HwPortIndex", EDDI_FATAL_ERR_EXCP, pBugsPort->HwPortIndex, 0);
            return;
        }
    }

    //Set the Pause bits
    IO_x32(FLOW_CTRL) = Flow;

    //no LinkInd due to PhyPower-change for ssd-workaround!
    pBugsPort->bPhyBugs_WA_Running = LSA_FALSE; //accept Link-Inds from now on!
    
    if (pDDB->pLocal_SWITCH->PhyBugs.port[pBugsPort->HwPortIndex].bPhyReset)
    {
        pDDB->pLocal_SWITCH->PhyBugs.port[pBugsPort->HwPortIndex].bPhyReset = LSA_FALSE;
        Status = EDDI_SetPhyReset(pBugsPort->HwPortIndex, pDDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugsPowerUp, EDDI_SetPhyReset, HwPortIndex:0x%X Status:0x%X", pBugsPort->HwPortIndex, Status);
            return;
        }

        EDDI_RequestFinish( (EDDI_LOCAL_HDB_PTR_TYPE)pDDB->pLocal_SWITCH->PhyBugs.port[pBugsPort->HwPortIndex].pPhyResetRQB->internal_context, 
                            pDDB->pLocal_SWITCH->PhyBugs.port[pBugsPort->HwPortIndex].pPhyResetRQB, 
                            EDD_STS_OK);	
                            
        pDDB->pLocal_SWITCH->PhyBugs.port[pBugsPort->HwPortIndex].pPhyResetRQB = LSA_NULL;
    }

    if (EDDI_PRMChangePortState_IsPlugged(pBugsPort->pDDB, pBugsPort->HwPortIndex))
    {
        Status = EDDI_TRASetPowerDown(pBugsPort->pDDB, pBugsPort->HwPortIndex, EDDI_PHY_POWER_ON, LSA_FALSE /*bRaw*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_SwiPhyBugsPowerUp-> EDDI_TRASetPowerDown", EDDI_FATAL_ERR_EXCP, Status, 0);
            return;
        }

        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugsPowerUp 2, BugHwPortIndex:0x%X BugLinkStatus:0x%X",
                          pBugsPort->HwPortIndex, pBugsPort->LinkStatus);

        EDDI_WAIT_10_NS(pDDB->hSysDev, 40000UL); //wait 400us

        EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[pBugsPort->HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

        EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__RestAutoNeg, 1);

        EDDI_SwiPhyWriteMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[pBugsPort->HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugsPowerUp, Flow:0x%X BugLinkStatus:0x%X",
                          Flow, pBugsPort->LinkStatus);

        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugsPowerUp<-");
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugsForceLinkDown()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugsForceLinkDown( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                     SWI_PHY_BUGS_PORT        *  const  pBugsPort )
{
    LSA_RESULT  Status;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugsForceLinkDown->");

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugsForceLinkDown, BugHwPortIndex:0x%X BugLinkStatus:0x%X",
                      pBugsPort->HwPortIndex, pBugsPort->LinkStatus);

    pBugsPort->LinkStatus  = EDDI_BUG_LINK_DOWN_SIMULATE;

    pBugsPort->bPhyBugs_WA_Running = LSA_TRUE; //ignore Link-Ind from now on!
    EDDI_SwiPortSetPhyStatus(pBugsPort->HwPortIndex, EDDI_PHY_POWER_OFF, LSA_FALSE, pDDB);
    Status = EDDI_TRASetPowerDown(pDDB, pBugsPort->HwPortIndex, EDDI_PHY_POWER_OFF, LSA_FALSE /*bRaw*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_SwiPhyBugsForceLinkDown, EDDI_TRASetPowerDown", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }
    //no LinkInd due to PhyPower-change for ssd-workaround!
    //EDDI_SwiPortSetPhyStatus(pBugsPort->HwPortIndex, EDD_PHY_POWER_OFF, LSA_TRUE, pDDB);

    Status = EDDI_StartTimer(pDDB, pBugsPort->PowerUpDelayTimerID, (LSA_UINT16)EDDI_TIMEOUT_POWERUP_DELAY_FOR_SSD_WA_100MS);  // 3 s
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_SwiPhyBugsForceLinkDown, PowerUpDelayTimerID", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugResetValues()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugResetValues( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex,
                                                                  SER_SWI_LINK_PTR_TYPE    const  pEDDILinkStatusPx )
{
    LSA_UINT32                        PhyStatus;
    SWI_PHY_BUGS_PORT       *  const  pPhyBugsPort = &pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex];

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugResetValues->");

    PhyStatus = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_STAT, pDDB);

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, HwPortIndex:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X PhyStatus:0x%X",
                      HwPortIndex, pEDDILinkStatusPx->LinkStatus, pPhyBugsPort->LinkStatus, PhyStatus);

    if (EDDI_SwiPhyIsFiber(HwPortIndex, pDDB))
    {
        pPhyBugsPort->LinkStatus = EDDI_BUG_LINK_UP_OPTICAL;
        return;
    }

    if (pPhyBugsPort->bPhyBugs_WA_Running && (EDDI_BUG_LINK_DOWN_SIMULATE == pPhyBugsPort->LinkStatus))
    {
        //do nothing, wait for timeout!
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, LinkInd during EDD_BUG_LINK_DOWN_SIMULATE");
        return;
    }
    else if (pPhyBugsPort->bPhyBugs_WA_Running)
    {
        //invalid state?
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SwiPhyBugResetValues, bPhyBugs_WA_Running=TRUE, StateError");
        pPhyBugsPort->bPhyBugs_WA_Running = LSA_FALSE;
    }

    LSA_UNUSED_ARG(PhyStatus);

    //Evaluate LinkDown
    if (pEDDILinkStatusPx->LinkStatus != EDD_LINK_UP)
    {
        //Directly after a LinkUp "EDDI_BUG_LINK_UP_FIRE_IND" a new LinkDown will occur
        if (pPhyBugsPort->LinkStatus == EDDI_BUG_LINK_UP_FIRE_IND)
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, EDDI_BUG_LINK_DOWN_SIMULATE 1, HwPortIndex:0x%X BugLinkStatus:0x%X",
                              HwPortIndex, pPhyBugsPort->LinkStatus);
            pPhyBugsPort->LinkStatus = EDDI_BUG_LINK_DOWN_SIMULATE;
            return;
        }

        //Interrupt -> no forced Link Down : Transition  EDDI_BUG_LINK_UP -> EDDI_BUG_LINK_DOWN_NORMAL
        if (pPhyBugsPort->LinkStatus != EDDI_BUG_LINK_DOWN_SIMULATE)
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, EDDI_BUG_LINK_DOWN_NORMAL 1, HwPortIndex:0x%X BugLinkStatus:0x%X",
                              HwPortIndex, pPhyBugsPort->LinkStatus);
            pPhyBugsPort->LinkStatus = EDDI_BUG_LINK_DOWN_NORMAL;
        }

        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, LinkStatus <> EDD_LINK_UP, HwPortIndex:0x%X BugLinkStatus:0x%X",
                          HwPortIndex, pPhyBugsPort->LinkStatus);
        return;
    }

    //from here Link UP

    if (pEDDILinkStatusPx->LinkSpeed == EDD_LINK_SPEED_10)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, EDD_LINK_SPEED_10, HwPortIndex:0x%X BugLinkStatus:0x%X",
                          HwPortIndex, pPhyBugsPort->LinkStatus);
        pPhyBugsPort->LinkStatus = EDDI_BUG_LINK_UP_10MBIT;
        return;
    }

    switch (pPhyBugsPort->LinkStatus)
    {
        case EDDI_BUG_LINK_UP:
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, EDDI_BUG_LINK_UP, HwPortIndex:0x%X BugLinkStatus:0x%X",
                              HwPortIndex, pPhyBugsPort->LinkStatus);
            // 2nd call EDDI_SWILinkInterrupt by SM -> nothing to do
            return;
        }

        case EDDI_BUG_LINK_DOWN_NORMAL:
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, EDDI_BUG_LINK_DOWN_NORMAL 2, HwPortIndex:0x%X BugLinkStatus:0x%X",
                              HwPortIndex, pPhyBugsPort->LinkStatus);
            //Indication will come imediately
            pPhyBugsPort->ActTimerTicks_in_10ns = pDDB->pLocal_SWITCH->PhyBugs.Tp_RcvFrame_in_10ns;
            pPhyBugsPort->LinkStatus            = EDDI_BUG_LINK_UP;
            break;
        }

        case EDDI_BUG_LINK_DOWN_SIMULATE:
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues, EDDI_BUG_LINK_DOWN_SIMULATE 2, HwPortIndex:0x%X BugLinkStatus:0x%X",
                              HwPortIndex, pPhyBugsPort->LinkStatus);

            //Interrupt: Patch Link Status that no indication will be sent to the user
            pEDDILinkStatusPx->Changed = LSA_FALSE;

            pPhyBugsPort->LinkStatus = EDDI_BUG_LINK_UP_FIRE_IND;
            //Remember values for following LinkIndication
            pPhyBugsPort->EDDILinkStatusPx = *pEDDILinkStatusPx;

            //Set new timervalue for 0-point
            pPhyBugsPort->LastTicks_in_10ns     = pDDB->ClkCounter_in_10ns_ticks_NewCycle;
            pPhyBugsPort->ActTimerTicks_in_10ns = EDDI_PHY_BUGS_START_DELAY_MS;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_SwiPhyBugResetValues, pPhyBugsPort->LinkStatus:", EDDI_FATAL_ERR_EXCP, pPhyBugsPort->LinkStatus, 0);
            break;
        }
    }

    pPhyBugsPort->Rcv_New = IO_R32(ERTEC_RegAdr[REG_STAT_CTRL_GFRX].HWPort[HwPortIndex]);

    pPhyBugsPort->Rcv_Old = pPhyBugsPort->Rcv_New;

    EDDI_SwiPhyReadMDCA(MYSTIC_COM_ERR_PHY_REG26, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &pPhyBugsPort->RxErr_Start, pDDB);

    pPhyBugsPort->RxErr_Start = EDDI_IRTE2HOST32(pPhyBugsPort->RxErr_Start);

    pPhyBugsPort->RxErr_Old   = pPhyBugsPort->RxErr_Start;
    pPhyBugsPort->RxErr_New   = pPhyBugsPort->RxErr_Start;
    pPhyBugsPort->t_NoRcvInMs = 0;

    EDDI_SWI_TRACE_09(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyBugResetValues<-BugHwPortIndex:0x%X RxErr_New:0x%X RxErr_Old:0x%X RxErr_Start:0x%X t_NoRcvInMs:0x%X Rcv_New:0x%X Rcv_Old:0x%X EDDILinkStatus:0x%X BugLinkStatus:0x%X",
                      pPhyBugsPort->HwPortIndex,
                      pPhyBugsPort->RxErr_New,
                      pPhyBugsPort->RxErr_Old,
                      pPhyBugsPort->RxErr_Start,
                      pPhyBugsPort->t_NoRcvInMs,
                      pPhyBugsPort->Rcv_New,
                      pPhyBugsPort->Rcv_Old,
                      pPhyBugsPort->EDDILinkStatusPx.LinkStatus,
                      pPhyBugsPort->LinkStatus);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyBugResetValuesDummy()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyBugResetValuesDummy( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex,
                                                                       SER_SWI_LINK_PTR_TYPE    const  pEDDILinkStatusPx )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyBugResetValuesDummy->");

    LSA_UNUSED_ARG(pDDB); //satisfy lint
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint
    LSA_UNUSED_ARG(pEDDILinkStatusPx); //satisfy lint
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV6)
/***************************************************************************
 * F u n c t i o n:       EDDI_Gen10HDXPhyBugReset()
 *
 * D e s c r i p t i o n:
 *
 * A r g u m e n t s:
 *
 * Return Value:          LSA_RESULT
 *
 ***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_Gen10HDXPhyBugReset( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32                      HwPortIndex,
                                                           LSA_BOOL                     *  bIndicate )
{                                                                    
    //Check if the PhyReset is not finished
    if (pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].pPhyResetRQB)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_Gen10HDXPhyBugReset, the PhyReset Service is runnig, HwPortIndex:0x%X", HwPortIndex);
        return EDD_STS_ERR_SEQUENCE;
    }
    
    pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].bPhyReset = LSA_FALSE;
       
    if (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_Gen10HDXPhyBugReset, for PhyReset is FIBER_OPTIC_CABLE not allowed");
        return EDD_STS_ERR_PARAM; 
    }
    
    if (!pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].bPhyBugs_WA_Running) //SSD is not running
    {
        LSA_RESULT  Status;
        
        pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].pPhyResetRQB = LSA_NULL;
        pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].bPhyReset    = LSA_FALSE;
        
        Status = EDDI_TRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_OFF, LSA_FALSE /*bRaw*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_Gen10HDXPhyBugReset, EDDI_TRASetPowerDown", EDDI_FATAL_ERR_EXCP, Status, 0);
            return Status;
        }

        Status = EDDI_SetPhyReset(HwPortIndex, pDDB);  
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_Gen10HDXPhyBugReset, EDDI_SetPhyReset", EDDI_FATAL_ERR_EXCP, HwPortIndex, Status);
            return Status;
        }
    }
    else //SSD is running
    {
        pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].pPhyResetRQB = pRQB;
        pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].pPhyResetRQB->internal_context = pRQB;
        pDDB->pLocal_SWITCH->PhyBugs.port[HwPortIndex].bPhyReset = LSA_TRUE;
        *bIndicate = LSA_FALSE; 
    }
    return EDD_STS_OK; 
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/***************************************************************************
 * F u n c t i o n:       EDDI_SetPhyReset()
 *
 * D e s c r i p t i o n:
 *
 * A r g u m e n t s:
 *
 * Return Value:          LSA_RESULT
 *
 ***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SetPhyReset( LSA_UINT32               const  HwPortIndex,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT  Status;

    //Disable polling for Phy (PHY_CMD_Px)
    EDDI_SwiPhyEnableLinkIRQ(HwPortIndex, LSA_FALSE, pDDB);

    //Set Phy in SoftReset (PhyControl-Register 0.15 = 1)
    Status = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetSoftReset(pDDB, HwPortIndex);
    if (EDD_STS_OK != Status)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SetPhyReset, call via pTRASetSoftReset, HwPortIndex:0x%X Status:0x%X", HwPortIndex, Status);
        return Status;
    }

    //Restore Phy values       
    EDDI_SwiPhySetSpMo(HwPortIndex,
                       (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config == EDD_LINK_AUTONEG)?LSA_TRUE:LSA_FALSE,
                       pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed,
                       pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode,
                       pDDB);

    //Set Phy in POWER_ON
    if (!pDDB->Glob.PhyPowerOff[HwPortIndex])
    {
        Status = EDDI_TRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_ON, LSA_FALSE /*bRaw*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_SetPhyReset, EDDI_TRASetPowerDown", EDDI_FATAL_ERR_EXCP, HwPortIndex, Status);
            return Status;
        }
    }

    //Enable polling for Phy (PHY_CMD_Px)
    EDDI_SwiPhyEnableLinkIRQ(HwPortIndex, LSA_TRUE, pDDB);

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_swi_phy_bugs.c                                          */
/*****************************************************************************/

