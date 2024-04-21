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
/*  F i l e               &F: eddi_Tra_NSC.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-system input-functions                      */
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

#if defined (TOOL_CHAIN_MS_WINDOWS)
#pragma warning( disable : 4206 ) //empty file
#endif

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)

#include "eddi_dev.h"
#include "eddi_swi_ext.h"
#include "eddi_Tra.h"
#include "eddi_time.h"
#include "eddi_ext.h"
#include "eddi_sync_usr.h"

#define EDDI_MODULE_ID     M_ID_TRA_NSC
#define LTRC_ACT_MODUL_ID  506

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  Transceiver: NSC (DP83849I) dual                                          */
/*               ===================                                          */
/*                                                                            */
/*                                                                            */
/*  Library for PHY specific routines                                         */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/

#define EDDI_PHY_NSC_BUG_AUTONEG_TIMER_TOLERATED_100ms_TICS    21  //4200 ms
#define EDDI_PHY_NSC_BUG_AUTONEG_TIMER_100ms_TICS               2  // 200 ms

/*===========================================================================*/
/*                                  typedefs                                 */
/*===========================================================================*/

//structure for the NSC Bug, Speed and Mode calulating during autoneg
typedef enum _SWI_NSC_GET_SPEEDMODE_TYPE
{
    SWI_NSC_GET_SPEEDMODE_OK,
    SWI_NSC_GET_SPEEDMODE_LINKDOWN,
    SWI_NSC_GET_SPEEDMODE_AUTONEGCOMPLETE_NO

} SWI_NSC_GET_SPEEDMODE_TYPE;

/*===========================================================================*/
/*                            local function declaration                     */
/*===========================================================================*/
static SWI_AUTONEG_GET_SPEEDMODE_TYPE EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugStateDoNothing( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                                            LSA_UINT32                      const HwPortIndex,
                                                                                            SWI_NSC_AUTONEG_BUGS_EVENT_TYPE const AppearEvent );

static SWI_AUTONEG_GET_SPEEDMODE_TYPE EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugStateAutonegOffNSC( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                                                LSA_UINT32                      const HwPortIndex,
                                                                                                SWI_NSC_AUTONEG_BUGS_EVENT_TYPE const AppearEvent );

static SWI_AUTONEG_GET_SPEEDMODE_TYPE EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugStateWaitingForLinkUpNSC( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                                                      LSA_UINT32                      const HwPortIndex,
                                                                                                      SWI_NSC_AUTONEG_BUGS_EVENT_TYPE const AppearEvent );

static SWI_AUTONEG_GET_SPEEDMODE_TYPE EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                                                                         LSA_UINT32                      const HwPortIndex,
                                                                                                         SWI_NSC_AUTONEG_BUGS_EVENT_TYPE const AppearEvent );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugStartTimerNSC( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                     LSA_UINT32              const HwPor );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugStopTimerNSC( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                    LSA_UINT32              const HwPortIndex );

static SWI_NSC_GET_SPEEDMODE_TYPE EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugGetSpeedModeNSC( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                                         LSA_UINT32              const HwPortIndex );

static LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugIfExpiredTimerNSC( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                         LSA_UINT32              const HwPortIndex );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_TRAAutonegBugInitDataNSC( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                   LSA_UINT32              const HwPortIndex );


/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugInitNSC()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugInitNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex )
{
    EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAAutonegBugInitNSC->HwPortIndex:0x%X", HwPortIndex);

    if (pDDB->Glob.PortParams[HwPortIndex].PhyTransceiver == EDDI_PHY_TRANSCEIVER_NSC)
    {
        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateAutonegOffNSC;
    }
    else
    {
        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateDoNothing;
    }

    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimerState = SWI_NSC_TIMER_STATE_STOPPED;

    EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugInitDataNSC()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugInitDataNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      LSA_UINT32               const  HwPortIndex )
{
    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimer100ms_Count = 0;

    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.Changed             = LSA_FALSE;
    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkStatus          = EDD_LINK_UNKNOWN;
    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkSpeed           = EDD_LINK_UNKNOWN;
    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkMode            = EDD_LINK_UNKNOWN;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugGetSpeedModeNSC()              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_NSC_GET_SPEEDMODE_TYPE                       */
/*                                                                         */
/***************************************************************************/
static  SWI_NSC_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugGetSpeedModeNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                            LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  Phy_Phy_Status_Reg;

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PHY_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Status_Reg, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugGetSpeedModeNSC, HwPortIndex:0x%X Phy_Status_Reg:0x%X", HwPortIndex, Phy_Phy_Status_Reg);

    //Again LinkDown
    if (0 == EDDI_GetBitField32(Phy_Phy_Status_Reg, EDDI_PHY_NSC_PHY_STATUS_BIT__LinkStatus))
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_TRAAutonegBugGetSpeedModeNSC, LINKDOWN -> LinkDown, HwPortIndex:0x%X Phy_Status_Reg:0x%X", HwPortIndex, Phy_Phy_Status_Reg);
        return SWI_NSC_GET_SPEEDMODE_LINKDOWN;
    }

    //AutoNeg Completed
    if (0 == EDDI_GetBitField32(Phy_Phy_Status_Reg, EDDI_PHY_NSC_PHY_STATUS_BIT__AutonegComplete))
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_TRAAutonegBugGetSpeedModeNSC, AUTONEGCOMPLETE_NO -> LinkDown, HwPortIndex:0x%X Phy_Status_Reg:0x%X", HwPortIndex, Phy_Phy_Status_Reg);
        return SWI_NSC_GET_SPEEDMODE_AUTONEGCOMPLETE_NO;
    }

    if (EDDI_GetBitField32(Phy_Phy_Status_Reg, EDDI_PHY_NSC_PHY_STATUS_BIT__Speed10))
    {
        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkSpeed = EDD_LINK_SPEED_10;
    }
    else
    {
        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkSpeed = EDD_LINK_SPEED_100;
    }

    if (EDDI_GetBitField32(Phy_Phy_Status_Reg, EDDI_PHY_NSC_PHY_STATUS_BIT__Duplex))
    {
        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkMode = EDD_LINK_MODE_FULL;
    }
    else
    {
        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkMode = EDD_LINK_MODE_HALF;
    }

    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.Changed    = LSA_TRUE;
    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkStatus = EDD_LINK_UP;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugGetSpeedModeNSC, SWI_AUTONEG_GET_SPEEDMODE_OK, HwPortIndex:0x%X Phy_Status_Reg:0x%X", HwPortIndex, Phy_Phy_Status_Reg);

    return SWI_NSC_GET_SPEEDMODE_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugIfExpiredTimerNSC()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugIfExpiredTimerNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                            LSA_UINT32               const  HwPortIndex )
{
    if (pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimer100ms_Count <= EDDI_PHY_NSC_BUG_AUTONEG_TIMER_TOLERATED_100ms_TICS)
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugStartTimerNSC()                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugStartTimerNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                        LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT    UsrPortIndex;
    LSA_RESULT  Status;

    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimerState = SWI_NSC_TIMER_STATE_RUNNUING;

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex2  = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (   (pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex2].SwiNSCAutoNegTimerState == SWI_NSC_TIMER_STATE_RUNNUING)
            && (HwPortIndex != HwPortIndex2))
        {
            return; /* A timer is already running */
        }
    }

    Status = EDDI_StartTimer(pDDB, pDDB->pLocal_SWITCH->NSCAutoNegBug.AutonegBugTimer100ms_id, EDDI_PHY_NSC_BUG_AUTONEG_TIMER_100ms_TICS);
    if (Status != EDD_STS_OK)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_TRAAutonegBugStartTimerNSC, HwPortIndex:0x%X AutonegBugTimer100ms_id:0x%X",
                          HwPortIndex, pDDB->pLocal_SWITCH->NSCAutoNegBug.AutonegBugTimer100ms_id);
        EDDI_Excp("EDDI_TRAAutonegBugStartTimerNSC, AutonegBugTimer100ms_id", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugStopTimerNSC()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugStopTimerNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT    UsrPortIndex;
    LSA_RESULT  Status;

    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimerState = SWI_NSC_TIMER_STATE_STOPPED;

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex2  = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex2].SwiNSCAutoNegTimerState == SWI_NSC_TIMER_STATE_RUNNUING)
        {
            return;
        }
    }

    Status = EDDI_StopTimer(pDDB, pDDB->pLocal_SWITCH->NSCAutoNegBug.AutonegBugTimer100ms_id);
    if (Status != EDD_STS_OK)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_TRAAutonegBugStopTimerNSC, HwPortIndex:0x%X AutonegBugTimer100ms_id:0x%X",
                          HwPortIndex, pDDB->pLocal_SWITCH->NSCAutoNegBug.AutonegBugTimer100ms_id);
        EDDI_Excp("EDDI_TRAAutonegBugStopTimerNSC, AutonegBugTimer100ms_id", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
* function name: EDDI_SwiNSCAutonegBugTimeout()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiNSCAutonegBugTimeout( LSA_VOID  *  const  context )
{
    LSA_UINT                        UsrPortIndex;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiNSCAutonegBugTimeout->");

    if (EDDI_NULL_PTR == context)
    {
        EDDI_Excp("EDDI_SwiNSCAutonegBugTimeout, EDDI_NULL_PTR == context", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiNSCAutonegBugTimeout");

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimerState == SWI_NSC_TIMER_STATE_STOPPED)
        {
            continue;
        }

        pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimer100ms_Count++;

        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiNSCAutonegBugTimeout, HwPortIndex:0x%X SwiNSCAutoNegTimer100ms_Count:0x%X",
                          HwPortIndex, pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegTimer100ms_Count);

        (LSA_VOID)pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct(pDDB, HwPortIndex, SWI_NSC_AUTONEG_BUGS_EVENT_TIMER);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiNSCAutonegBugTimeout<-");
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugStateDoNothing()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
static  SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugStateDoNothing( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                                               LSA_UINT32                       const  HwPortIndex,
                                                                                               SWI_NSC_AUTONEG_BUGS_EVENT_TYPE  const  AppearEvent )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAAutonegBugStateDoNothing->");

    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
    LSA_UNUSED_ARG(AppearEvent); //satisfy lint!
    LSA_UNUSED_ARG(pDDB);

    return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugStateAutonegOffNSC()           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
static  SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugStateAutonegOffNSC( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                                                   LSA_UINT32                       const  HwPortIndex,
                                                                                                   SWI_NSC_AUTONEG_BUGS_EVENT_TYPE  const  AppearEvent )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAAutonegBugStateAutonegOffNSC->");

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateAutonegOffNSC, HwPortIndex:0x%X AppearEvent:0x%X", HwPortIndex, AppearEvent);

    switch (AppearEvent)
    {
        case SWI_NSC_AUTONEG_BUGS_EVENT_TIMER:  //normally the timer may not come,
                                                //but in the most time is not possible for the environment
        case SWI_NSC_AUTONEG_BUGS_EVENT_LINKDOWN:
        case SWI_NSC_AUTONEG_BUGS_EVENT_LINKUP:
        case SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_OFF:
        {
            //nothing to do
            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_ON:
        {
            EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

            pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateWaitingForLinkUpNSC;

            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        default:
        {
            EDDI_Excp("EDDI_TRAAutonegBugStateAutonegOffNSC, HwPortIndex: AppearEvent:", EDDI_FATAL_ERR_EXCP, HwPortIndex, AppearEvent);
            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugStateWaitingForLinkUpNSC()     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
static  SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugStateWaitingForLinkUpNSC( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                                                         LSA_UINT32                       const  HwPortIndex,
                                                                                                         SWI_NSC_AUTONEG_BUGS_EVENT_TYPE  const  AppearEvent )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAAutonegBugStateWaitingForLinkUpNSC->");

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, HwPortIndex:0x%X AppearEvent:0x%X", HwPortIndex, AppearEvent);

    switch (AppearEvent)
    {
        case SWI_NSC_AUTONEG_BUGS_EVENT_TIMER: //normally the timer may not come,
                                               //but in the most time is not possible for the environment
        case SWI_NSC_AUTONEG_BUGS_EVENT_LINKDOWN:
        case SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_ON:
        {
            //nothing to do
            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_OFF:
        {
            EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

            pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateAutonegOffNSC;
            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_LINKUP:
        {
            EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

            switch (EDDI_TRAAutonegBugGetSpeedModeNSC(pDDB, HwPortIndex))
            {
                case SWI_NSC_GET_SPEEDMODE_OK:        /* Speed and Mode exist, normal exit*/
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, SWI_AUTONEG_GET_SPEEDMODE_OK, HwPortIndex:0x%X AppearEvent:0x%X",
                                          HwPortIndex, AppearEvent);
                    return SWI_AUTONEG_GET_SPEEDMODE_OK;
                }

                case SWI_NSC_GET_SPEEDMODE_LINKDOWN: /* linkup not exist, start a restart autoneg*/
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, SWI_AUTONEG_GET_SPEEDMODE_LINKDOWN, HwPortIndex:0x%X AppearEvent:0x%X",
                                          HwPortIndex, AppearEvent);
                    return SWI_AUTONEG_GET_SPEEDMODE_RESTART;
                }

                case SWI_NSC_GET_SPEEDMODE_AUTONEGCOMPLETE_NO: /* linkup exits, but not still autoneg, start timer */
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO, HwPortIndex:0x%X AppearEvent:0x%X",
                                          HwPortIndex, AppearEvent);
                    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC;

                    EDDI_TRAAutonegBugStartTimerNSC(pDDB, HwPortIndex);

                    return SWI_AUTONEG_GET_SPEEDMODE_CALL_ASYNC;
                }

                default:
                {
                    EDDI_Excp("EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, HwPortIndex, AppearEvent", EDDI_FATAL_ERR_EXCP, HwPortIndex, AppearEvent);
                    return SWI_AUTONEG_GET_SPEEDMODE_ERROR;
                }
            }
        }

        default:
        {
            EDDI_Excp("EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, HwPortIndex, AppearEvent",
                      EDDI_FATAL_ERR_EXCP, HwPortIndex, AppearEvent);
            return SWI_AUTONEG_GET_SPEEDMODE_ERROR;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC()  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
static  SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                                                            LSA_UINT32                       const  HwPortIndex,
                                                                                                            SWI_NSC_AUTONEG_BUGS_EVENT_TYPE  const  AppearEvent )
{
    EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC->");

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC, HwPortIndex:0x%X AppearEvent:0x%X", HwPortIndex, AppearEvent);

    switch (AppearEvent)
    {
        case SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_ON:
        {
            //nothing to do
            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_OFF:
        {
            EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

            EDDI_TRAAutonegBugStopTimerNSC(pDDB, HwPortIndex);

            pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateAutonegOffNSC;

            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_LINKDOWN:
        {
            EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

            EDDI_TRAAutonegBugStopTimerNSC(pDDB, HwPortIndex);

            pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateWaitingForLinkUpNSC;

            return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_TIMER:
        {
            switch (EDDI_TRAAutonegBugGetSpeedModeNSC(pDDB, HwPortIndex))
            {
                case SWI_NSC_GET_SPEEDMODE_OK: /* Speed and Mode exist, ok and exit */
                {
                    SWI_LINK_PARAM_TYPE  *  const  pLinkPx = &pDDB->pLocal_SWITCH->LinkPx[HwPortIndex];
                    SER_SWI_LINK_TYPE              LinkStatusPx[EDDI_MAX_IRTE_PORT_CNT];
                    LSA_BOOL                       PortChanged [EDDI_MAX_IRTE_PORT_CNT];
                    LSA_UINT32                     UsrPortIndex;

                    EDDI_TRAAutonegBugStopTimerNSC(pDDB, HwPortIndex);

                    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
                    {
                        LSA_UINT32  const  HwPortIndex2 = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                        //Defaultvalue LinkDown
                        LinkStatusPx[HwPortIndex2].Changed    = LSA_FALSE;
                        LinkStatusPx[HwPortIndex2].LinkStatus = EDD_LINK_DOWN;
                        LinkStatusPx[HwPortIndex2].LinkSpeed  = EDD_LINK_UNKNOWN;
                        LinkStatusPx[HwPortIndex2].LinkMode   = EDD_LINK_UNKNOWN;
                    }

                    LinkStatusPx[HwPortIndex].LinkStatus = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkStatus;
                    LinkStatusPx[HwPortIndex].LinkSpeed  = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkSpeed;
                    LinkStatusPx[HwPortIndex].LinkMode   = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkMode;

                    /* Call the rest of the normal linkup call */
                    EDDI_SwiPhyAction4AutoNegOn2(HwPortIndex,
                                                 &LinkStatusPx[0],
                                                 pDDB);

                    pLinkPx->LinkStatus = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkStatus;
                    pLinkPx->LinkSpeed  = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkSpeed;
                    pLinkPx->LinkMode   = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkMode;

                    //inform port state machines
                    EDDI_SyncPortStmsLinkChange(pDDB,
                                                HwPortIndex,
                                                pLinkPx->LinkStatus,
                                                pLinkPx->LinkSpeed,
                                                pLinkPx->LinkMode);

                    PortChanged[HwPortIndex] = LSA_TRUE;

                    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

                    EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

                    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateWaitingForLinkUpNSC;

                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC, SWI_AUTONEG_GET_SPEEDMODE_OK, HwPortIndex:0x%X AppearEvent:0x%X",
                                          HwPortIndex, AppearEvent);

                    return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
                }

                case SWI_NSC_GET_SPEEDMODE_LINKDOWN: /* no link more and exit */
                {
                    EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

                    EDDI_TRAAutonegBugStopTimerNSC(pDDB, HwPortIndex);

                    pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct = EDDI_TRAAutonegBugStateWaitingForLinkUpNSC;

                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC, SWI_AUTONEG_GET_SPEEDMODE_LINKDOWN, HwPortIndex:0x%X AppearEvent:0x%X",
                                          HwPortIndex, AppearEvent);

                    return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
                }

                case SWI_NSC_GET_SPEEDMODE_AUTONEGCOMPLETE_NO: /* Speed and Mode not stil exist, time out */
                {
                    if (!EDDI_TRAAutonegBugIfExpiredTimerNSC(pDDB, HwPortIndex))
                    {
                        LSA_UINT32        UsrPortIndex;
                        SER_SWI_LINK_TYPE LinkStatusPx[EDDI_MAX_IRTE_PORT_CNT];

                        EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

                        EDDI_TRAAutonegBugStopTimerNSC(pDDB, HwPortIndex);

                        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++ )
                        {
                            LSA_UINT32  const  HwPortIndex2 = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                            //Defaultvalue LinkDown
                            LinkStatusPx[HwPortIndex2].Changed    = LSA_FALSE;
                            LinkStatusPx[HwPortIndex2].LinkStatus = EDD_LINK_DOWN;
                            LinkStatusPx[HwPortIndex2].LinkSpeed  = EDD_LINK_UNKNOWN;
                            LinkStatusPx[HwPortIndex2].LinkMode   = EDD_LINK_UNKNOWN;
                        }

                        //Restart Autoneg
                        EDDI_SwiPhyAction4AutoNegOn2(HwPortIndex,
                                                     &LinkStatusPx[0],
                                                     pDDB);

                        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC, SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO, HwPortIndex:0x%X AppearEvent:0x%X",
                                              HwPortIndex, AppearEvent);

                        return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT; //Restart
                    }
                    else
                    {
                        //Timer is still running
                        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC, SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO, HwPortIndex:0x%X AppearEvent:0x%X",
                                              HwPortIndex, AppearEvent);

                        return SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT;
                    }
                }

                default:
                {
                    EDDI_Excp("EDDI_TRAAutonegBugStateWaitingForLinkUpNSC, HwPortIndex, AppearEvent", EDDI_FATAL_ERR_EXCP, HwPortIndex, AppearEvent);
                    return SWI_AUTONEG_GET_SPEEDMODE_ERROR;
                }
            }
        }

        case SWI_NSC_AUTONEG_BUGS_EVENT_LINKUP:
        default:
        {
            EDDI_Excp("EDDI_TRAAutonegBugStateWaitingForSpeedModeNSC, HwPortIndex, AppearEvent", EDDI_FATAL_ERR_EXCP, HwPortIndex, AppearEvent);
            return SWI_AUTONEG_GET_SPEEDMODE_ERROR;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAGetSpMoAutoNegOn_NSC()                   */
/*                                                                         */
/* D e s c r i p t i o n: Gets Speed/Mode for Autoneg ON                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                   LSA_UINT32               const  HwPortIndex,
                                                                                   SER_SWI_LINK_PTR_TYPE    const  pBasePx )
{
    SWI_AUTONEG_GET_SPEEDMODE_TYPE  AutonegSpeedMode;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NSC->HwPortIndex:0x%X", HwPortIndex);

    EDDI_TRAAutonegBugInitDataNSC(pDDB, HwPortIndex);

    AutonegSpeedMode = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct(pDDB, HwPortIndex, SWI_NSC_AUTONEG_BUGS_EVENT_LINKUP);

    pBasePx->Changed    = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.Changed;
    pBasePx->LinkStatus = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkStatus;
    pBasePx->LinkSpeed  = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkSpeed;
    pBasePx->LinkMode   = pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].LinkState.LinkMode;

    EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NSC<-HwPortIndex:0x%X Changed:0x%X LinkStatus:0x%X LinkSpeed:0x%X LinkMode:0x%X",
                          HwPortIndex, pBasePx->Changed, pBasePx->LinkStatus, pBasePx->LinkSpeed, pBasePx->LinkMode);

    return AutonegSpeedMode;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetPowerDown_NSC()                       */
/*                                                                         */
/* D e s c r i p t i o n: Switch PHY to PowerDown                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT8                const  PHYPower,
                                                           LSA_BOOL                 const  bRaw)

{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetPowerDown_NSC->HwPortIndex:0x%X PhyPower:0x%X", HwPortIndex, PHYPower);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    //Do not take any options during LinkDown after PowerDown
    if (   (PHYPower == EDDI_PHY_POWER_OFF)
        && (EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__PowerDown) == 0)
        && !bRaw)
    {
        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].bFireLinkDownAction = LSA_TRUE;

        EDDI_SwiPhyActionForLinkDown(HwPortIndex, pDDB);

        //No actions after LinkDown
        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].bFireLinkDownAction = LSA_FALSE;
    }

    EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__PowerDown, (PHYPower == EDDI_PHY_POWER_OFF) ? 1 : 0);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckPowerDown_NSC()                     */
/*                                                                         */
/* D e s c r i p t i o n: check PHY to PowerDown                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                        pIsPowerDown if the return value is EDD_STS_OK   */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                      * pIsPowerDown)
{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    if (EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__PowerDown) == 0)
    {
        *pIsPowerDown = LSA_FALSE;
    }
    else
    {
        *pIsPowerDown = LSA_TRUE;
    }

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetPowerDown_NSC->HwPortIndex:0x%X, IsPowerDown:%d", HwPortIndex, *pIsPowerDown);
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAStartPhy_NSC()                           */
/*                                                                         */
/* D e s c r i p t i o n: Actions to take before PHY-reset is released     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex,
                                                     LSA_BOOL                 const  bPlugCommit)
{
    LSA_UINT32  Phy_PCS_Config_Status_Reg;
    LSA_UINT32  Phy_10BaseT_Status_Control_Reg;
    #if !defined (EDDI_CFG_NO_ENHANCED_CRS_DV)
    LSA_UINT32  Phy_Page_Select_Reg;
    LSA_UINT32  Phy_PMD_Cnfg_Reg;
    #endif
    LSA_UINT32  Phy_Phy_Control_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRAStartPhy_NSC->HwPortIndex:0x%X", HwPortIndex);

    #if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
    // In case of bPlugCommit, EDDI_LL_NSC_PRESTART_ACTION() was already called, it must not called again
    if (!bPlugCommit)
    {
        LSA_UINT16 PhyAdr = pDDB->Glob.PortParams[HwPortIndex].PhyAdr;
        // Execute applicationspecific actions prior to initializing the transceiver
        EDDI_LL_NSC_PRESTART_ACTION(pDDB->hSysDev, HwPortIndex, EDDI_NSC_PRESTART_ACTION_CALLED_FROM_STARTUP, &PhyAdr);
        // Changing of the transceiver address is prohibited at startup
        if (PhyAdr != pDDB->Glob.PortParams[HwPortIndex].PhyAdr)
        {
            EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_TRAStartPhy_NSC, EDDI_LL_NSC_PRESTART_ACTION, HwPortIndex:0x%X PhyAdr:0x%X PhyAdr(expected):0x%X",
                              HwPortIndex, PhyAdr, pDDB->Glob.PortParams[HwPortIndex].PhyAdr);
            EDDI_Excp("EDDI_TRAStartPhy_NSC, EDDI_LL_NSC_PRESTART_ACTION, HwPortIndex, PhyAdr", EDDI_FATAL_ERR_EXCP, HwPortIndex, PhyAdr);
            return;
        }
    }
    #else
    LSA_UNUSED_ARG(bPlugCommit);
    #endif

    //Workaround for single side receive
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PCS_CONFIG_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_PCS_Config_Status_Reg, pDDB);

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_NSC, HwPortIndex:0x%X PCS_Config_Status_Reg:0x%X", HwPortIndex, Phy_PCS_Config_Status_Reg);

    EDDI_SetBitField32(&Phy_PCS_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__Signal_Detect_Option, 0);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PCS_CONFIG_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_PCS_Config_Status_Reg, pDDB);

    //Workaround for single side link when change partner from 10Mbit to 100Mbit and this site has autoneg
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_10BASET_STATUS_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_10BaseT_Status_Control_Reg, pDDB);

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_NSC, HwPortIndex:0x%X 10BaseT_Status_Control_Reg:0x%X", HwPortIndex, Phy_10BaseT_Status_Control_Reg);

    EDDI_SetBitField32(&Phy_10BaseT_Status_Control_Reg, EDDI_PHY_NSC_10BASET_STATUS_CONTROL_BIT__REJECT100_BASET, 1);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_10BASET_STATUS_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_10BaseT_Status_Control_Reg, pDDB);

    //Set Enhanced CRS_DV Operation for RMII
    #if !defined (EDDI_CFG_NO_ENHANCED_CRS_DV)

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_NSC, Set Enhanced CRS_DV for RMII, HwPortIndex:0x%X", HwPortIndex);

    //switch Page Select Register to "Test Mode Register Page 1"
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Page_Select_Reg, pDDB);
    EDDI_SetBitField32(&Phy_Page_Select_Reg, EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Page_Select_Reg, pDDB);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PMD_CNFG_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_PMD_Cnfg_Reg, pDDB);
    EDDI_SetBitField32(&Phy_PMD_Cnfg_Reg, EDDI_PHY_NSC_PMD_CNFG_BIT__CRS_DV, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PMD_CNFG_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_PMD_Cnfg_Reg, pDDB);

    //switch Page Select Register back to "Extended Registers Page 0"
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Page_Select_Reg, pDDB);
    EDDI_SetBitField32(&Phy_Page_Select_Reg, EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL, 0);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Page_Select_Reg, pDDB);

    #endif

    //enable AutoMDIX if Autoneg is selected
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);
    if (EDD_LINK_AUTONEG == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config)
    {
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__MDIX_EN, 1);
    }
    else
    {
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__MDIX_EN, 0);
    }
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckSpeedModeCapability_NSC()           */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_NSC( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                       LSA_UINT32                                const  HwPortIndex,
                                                                       LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pAutonegCapability )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckSpeedModeCapability_NSC->HwPortIndex:0x%X", HwPortIndex);

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            //At least the following modes shall be supported:
            if (0 == ((EDD_AUTONEG_CAP_10BASET   + 
                       EDD_AUTONEG_CAP_10BASETFD + 
                       EDD_AUTONEG_CAP_100BASETX + 
                       EDD_AUTONEG_CAP_100BASETXFD) & *pAutonegCapability))
            {
                return EDD_STS_ERR_PARAM;
            }
        }
        break;

        //Fiber
        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
        {
            LSA_UINT32  const  cap = EDDI_AUTONEG_CAP_100MBIT_HALF + EDDI_AUTONEG_CAP_100MBIT_FULL + EDDI_AUTONEG_CAP_10MBIT_HALF + EDDI_AUTONEG_CAP_10MBIT_FULL;

            //1) no-autonegotiation is supported.
            if(EDD_AUTONEG_CAP_NONE==*pAutonegCapability)
            {
                break;
            }
            //2) no other bit may be set
            if (*pAutonegCapability & (~cap))
            {
                return EDD_STS_ERR_PARAM;
            }
            //3) any of the bits shall be set
            if (0 == (*pAutonegCapability & cap))
            {
                return EDD_STS_ERR_PARAM;
            }
        }
        break;

        //other values are not allowed
        default:
        {
            EDDI_Excp("EDDI_TRACheckSpeedModeCapability_NSC, invalid MediaType:", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegMappingCapability_NSC()           */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_NSC( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                       LSA_UINT32                                const  HwPortIndex,
                                                                       LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pConfiguredLinkSpeedAndMode  )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegMappingCapability_NSC->HwPortIndex:0x%X", HwPortIndex);

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            if (EDD_LINK_AUTONEG  != *pConfiguredLinkSpeedAndMode )
            {
                return EDD_STS_ERR_PARAM;
            }
        }
        break;

        //Fiber
        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
        {
            if (   (EDD_LINK_100MBIT_HALF != *pConfiguredLinkSpeedAndMode ) && (EDD_LINK_100MBIT_FULL != *pConfiguredLinkSpeedAndMode )
                && (EDD_LINK_10MBIT_HALF  != *pConfiguredLinkSpeedAndMode ) && (EDD_LINK_10MBIT_FULL  != *pConfiguredLinkSpeedAndMode ))
            {
                return EDD_STS_ERR_PARAM;
            }
        }
        break;

        //other values are not allowed
        default:
        {
            EDDI_Excp("EDDI_TRAAutonegMappingCapability_NSC, invalid MediaType:", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetMDIX_NSC()                            */
/*                                                                         */
/* D e s c r i p t i o n: Controls the MDIX feature of the PHY.            */
/*                        (Manual control of crossover)                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  HwPortIndex,
                                                      LSA_BOOL                 const  bMDIX )
{
    LSA_UINT32  Phy_Phy_Control_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetMDIX_NSC->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    //This function is only called for Autoneg OFF: Switch off AutoMDIX
    EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__MDIX_EN, 0);

    if (bMDIX) //force crossing requested?
    {
        /* If Auto-Neg is off: Rx and Tx lines are crossed */
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__FORCE_MDIX, 1);
    }
    else //force crossing not requested
    {
        /* If Auto-Neg is off: Rx and Tx lines are not crossed (straight) */
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__FORCE_MDIX, 0);
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRARestoreAutoMDIX_NSC()                    */
/*                                                                         */
/* D e s c r i p t i o n: Restores the AutoMDIX feature of the PHY.        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  Phy_Phy_Control_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRARestoreAutoMDIX_NSC->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    //This function is only called for Autoneg ON: Switch on AutoMDIX, Switch off ForceMDIX
    EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__MDIX_EN, 1);
    EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_NSC_PHY_CONTROL_BIT__FORCE_MDIX, 0);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TraSetFXMode_NSC()                          */
/*                                                                         */
/* D e s c r i p t i o n: Controls the FXMode feature of the PHY.          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  bFXModeON )
{
    LSA_UINT32  Phy_Phy_Config_Status_Reg;
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TraSetFXMode_NSC->HwPortIndex:0x%X  FXModeON:0x%X", HwPortIndex, bFXModeON);

    EDDI_SwiPhyReadMDCA( EDDI_PHY_NSC_PCS_CONFIG_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Config_Status_Reg, pDDB);

    if (bFXModeON)
    {
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__FX_EN, 1);
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__FEFI_EN , 1);
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__SCRAM_BYPASS , 1);
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__DESCRAM_BYPASS , 1);
    }
    else
    {
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__FX_EN, 0);
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__FEFI_EN , 0);
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__SCRAM_BYPASS , 0);
        EDDI_SetBitField32(&Phy_Phy_Config_Status_Reg, EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__DESCRAM_BYPASS , 0);
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PCS_CONFIG_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Config_Status_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetSoftReset_NSC()                       */
/*                                                                         */
/* D e s c r i p t i o n: Set Softreset of the PHY.                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetSoftReset_NSC->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    return EDD_STS_ERR_NOT_IMPL;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  LED blink (show location) implementation for NSC PHY                      */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBeginExternal_NSC()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_BEGIN(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_NSC);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetModeExternal_NSC()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  HwPortIndex,
                                                                    LSA_UINT16               const  LEDMode )
{
    EDDI_LL_LED_BLINK_SET_MODE(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_NSC, LEDMode);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEndExternal_NSC()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() after blinking of LED(s) has finished.
 *                 Can be used e.g. to re-enable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_END(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_NSC);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBegin_NSC()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex )
{
    //no actions necessary!

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetMode_NSC()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_UINT16               const  LEDMode )
{
    LSA_UINT32  PhyLEDDirectControlReg;

    //control force-led-function via PHY register LED Direct Control
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDDirectControlReg, pDDB);
    EDDI_SetBitField32(&PhyLEDDirectControlReg, EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_LNKLED, 1);
    EDDI_SetBitField32(&PhyLEDDirectControlReg, EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_ACTLED, 1);
    EDDI_SetBitField32(&PhyLEDDirectControlReg, EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__LNKLED, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1); //0 = LED On
    EDDI_SetBitField32(&PhyLEDDirectControlReg, EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__ACTLED, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1); //0 = LED On
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDDirectControlReg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEnd_NSC()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() after blinking of LED(s) has finished.
 *                 Can be used e.g. to re-enable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  PhyLEDDirectControlReg;

    //disable force-led-function via PHY register LED Direct Control
    EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDDirectControlReg, pDDB);
    EDDI_SetBitField32(&PhyLEDDirectControlReg, EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_LNKLED, 0);
    EDDI_SetBitField32(&PhyLEDDirectControlReg, EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_ACTLED, 0);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDDirectControlReg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_NSC

#if defined (EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)
/*=============================================================================
 * function name:  EDDI_TRARegDump_NSC()
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRARegDump_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{

    struct _EDDI_PHY_REG_DUMP_NSC
    {
        //Standard regs
        LSA_UINT32  BMCR;     //00h Basic Mode Control Register
        LSA_UINT32  BMSR;     //01h Basic Mode Status Register
        LSA_UINT32  ANAR;     //04h Auto-Negotiation Advertisement Register
        LSA_UINT32  ANLPAR;   //05h Auto-Negotiation Link Partner Ability Register (Base Page)
        LSA_UINT32  ANER;     //06h Auto-Negotiation Expansion Register
        LSA_UINT32  ANNPTR;   //07h Auto-Negotiation Next Page TX
        LSA_UINT32  PHYSTS;  //10h PHYSTS PHY Status Register

        //Page 0
        LSA_UINT32  FCSCR;    //14h False Carrier Sense Counter Register
        LSA_UINT32  RECR;     //15h Receive Error Counter Register
        LSA_UINT32  PCSR;     //16h PCS Sub-Layer Configuration and Status Register
        LSA_UINT32  RBR;      //17h RMII and Bypass Register
        LSA_UINT32  PHYCR;    //19h PHY Control Register
        LSA_UINT32  TENBTSCR;  //1Ah 10Base-T Status/Control Register
        LSA_UINT32  CDCTRL1;  //1Bh CD Test Control Register and BIST Extensions Register
        LSA_UINT32  PHYCR2;   //1Ch Phy Control Register 2
        LSA_UINT32  EDCR;     //1Dh Energy    

        //Page 1
        LSA_UINT32  PMD_CNFG;  //14h

        //Page 2  
        LSA_UINT32  LEN100_DET; //14h 100Mb Length Detect Register
        LSA_UINT32  FREQ100;    //15h 100Mb Frequency Offset Indication Register
        LSA_UINT32  TDR_CTRL;   //16h TDR Control Register
        LSA_UINT32  TDR_WIN;    //17h TDR Window Register
        LSA_UINT32  TDR_PEAK;   //18h TDR Peak Measurement Register
        LSA_UINT32  TDR_THR;    //19h TDR Threshold Measurement Register
        LSA_UINT32  VAR_CTRL;   //1Ah Variance Control Register
        LSA_UINT32  VAR_DAT;    //1Bh Variance Data Register
        LSA_UINT32  LQMR;       //1Dh Link Quality Monitor Register
        LSA_UINT32  LQDR1Eh;    //1Eh Link Quality Data Register
    } RegDumpNSC;
 
    LSA_UINT32  HwPortIndex, UsrPortId;
  
    for (UsrPortId = 1; UsrPortId <= pDDB->PM.PortMap.PortCnt; UsrPortId++)
    {
        LSA_UINT32 PageSel;

        HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
      
        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "!!!Dump NSC PHY Regs HwPortIndex:%d, UsrPortId:%d, PhyAdr:0x%X", 
            HwPortIndex, UsrPortId, pDDB->Glob.PortParams[HwPortIndex].PhyAdr);

        //Read basic regs
        EDDI_SwiPhyReadMDCA(0x00, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.BMCR, pDDB);
        EDDI_SwiPhyReadMDCA(0x01, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.BMSR, pDDB);
        EDDI_SwiPhyReadMDCA(0x04, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.ANAR, pDDB);
        EDDI_SwiPhyReadMDCA(0x05, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.ANLPAR, pDDB);
        EDDI_SwiPhyReadMDCA(0x06, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.ANER, pDDB);
        EDDI_SwiPhyReadMDCA(0x07, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.ANNPTR, pDDB);
        EDDI_SwiPhyReadMDCA(0x10, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.PHYSTS, pDDB);

        //Read page 0
        EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);
        EDDI_SetBitField32(&PageSel, EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL, 0);
        EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);

        EDDI_SwiPhyReadMDCA(0x14, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.FCSCR, pDDB);
        EDDI_SwiPhyReadMDCA(0x15, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.RECR, pDDB);
        EDDI_SwiPhyReadMDCA(0x16, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.PCSR, pDDB);
        EDDI_SwiPhyReadMDCA(0x17, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.RBR, pDDB);
        EDDI_SwiPhyReadMDCA(0x19, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.PHYCR, pDDB);
        EDDI_SwiPhyReadMDCA(0x1A, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.TENBTSCR, pDDB);
        EDDI_SwiPhyReadMDCA(0x1B, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.CDCTRL1, pDDB);
        EDDI_SwiPhyReadMDCA(0x1C, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.PHYCR2, pDDB);
        EDDI_SwiPhyReadMDCA(0x1D, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.EDCR, pDDB);

        //Read page 1
        EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);
        EDDI_SetBitField32(&PageSel, EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL, 1);
        EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);

        EDDI_SwiPhyReadMDCA(0x14, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.PMD_CNFG, pDDB);

        //Read page 2
        EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);
        EDDI_SetBitField32(&PageSel, EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL, 2);
        EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);

        EDDI_SwiPhyReadMDCA(0x14, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.LEN100_DET, pDDB);
        EDDI_SwiPhyReadMDCA(0x15, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.FREQ100, pDDB);
        EDDI_SwiPhyReadMDCA(0x16, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.TDR_CTRL, pDDB);
        EDDI_SwiPhyReadMDCA(0x17, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.TDR_WIN, pDDB);
        EDDI_SwiPhyReadMDCA(0x18, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.TDR_PEAK, pDDB);
        EDDI_SwiPhyReadMDCA(0x19, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.TDR_THR, pDDB);
        EDDI_SwiPhyReadMDCA(0x1A, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.VAR_CTRL, pDDB);
        EDDI_SwiPhyReadMDCA(0x1B, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.VAR_DAT, pDDB);
        EDDI_SwiPhyReadMDCA(0x1D, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.LQMR, pDDB);
        EDDI_SwiPhyReadMDCA(0x1E, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &RegDumpNSC.LQDR1Eh, pDDB);

        //Restore to page 0
        EDDI_SwiPhyReadMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);
        EDDI_SetBitField32(&PageSel, EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL, 0);
        EDDI_SwiPhyWriteMDCA(EDDI_PHY_NSC_PAGE_SELECT_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PageSel, pDDB);
      
        //Dump Regs
        EDDI_PROGRAM_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "BMCR:0x%X BMSR:0x%X ANAR:0x%X ANLPAR:0x%X ANER:0x%X ANNPTR:0x%X PHYSTS:0x%X FCSCR:0x%X",
            RegDumpNSC.BMCR, RegDumpNSC.BMSR, RegDumpNSC.ANAR, RegDumpNSC.ANLPAR, RegDumpNSC.ANER, RegDumpNSC.ANNPTR, RegDumpNSC.PHYSTS, RegDumpNSC.FCSCR);
        EDDI_PROGRAM_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "RECR:0x%X PCSR:0x%X RBR:0x%X PHYCR:0x%X 10BTSCR:0x%X CDCTRL1:0x%X PHYCR2:0x%X EDCR:0x%X",
            RegDumpNSC.RECR, RegDumpNSC.PCSR, RegDumpNSC.RBR, RegDumpNSC.PHYCR, RegDumpNSC.TENBTSCR, RegDumpNSC.CDCTRL1, RegDumpNSC.PHYCR2, RegDumpNSC.EDCR);
        EDDI_PROGRAM_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "LEN100_DET:0x%X FREQ100:0x%X TDR_CTRL:0x%X TDR_WIN:0x%X TDR_PEAK:0x%X TDR_THR:0x%X VAR_CTRL:0x%X VAR_DAT:0x%X",
            RegDumpNSC.LEN100_DET, RegDumpNSC.FREQ100, RegDumpNSC.TDR_CTRL, RegDumpNSC.TDR_WIN, RegDumpNSC.TDR_PEAK, RegDumpNSC.TDR_THR, RegDumpNSC.VAR_CTRL, RegDumpNSC.VAR_DAT);
        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "LQMR:0x%X LQDR1Eh:0x%X, PMD_CNFG:0x%X",
            RegDumpNSC.LQMR, RegDumpNSC.LQDR1Eh, RegDumpNSC.PMD_CNFG);
    }    
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //(EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)

#endif //EDDI_CFG_PHY_TRANSCEIVER_NSC


/*****************************************************************************/
/*  end of file eddi_Tra_NSC.c                                               */
/*****************************************************************************/

