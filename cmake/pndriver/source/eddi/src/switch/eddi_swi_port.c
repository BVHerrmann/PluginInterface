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
/*  F i l e               &F: eddi_swi_port.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Ser10 File Data Base for EDDI.                   */
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
#include "eddi_ext.h"
#include "eddi_ser_cmd.h"
#include "eddi_lock.h"

#include "eddi_time.h"
#include "eddi_swi_ext.h"
#include "eddi_Tra.h"

#include "eddi_sync_usr.h"
#include "eddi_prm_state.h"

#define EDDI_MODULE_ID     M_ID_SWI_PORT
#define LTRC_ACT_MODUL_ID  313

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                 definition                                */
/*===========================================================================*/

/*===========================================================================*/
/*                                 typedefs                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                            local function declaration                     */
/*===========================================================================*/
static LSA_UINT16 EDDI_LOCAL_FCT_ATTR EDDI_SwiPortGetSpanningTree( LSA_UINT32              const HwPortIndex,
                                                                   LSA_UINT16              const CurrentSpanningTreeState,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

/*===========================================================================*/
/*                            local function definition                      */
/*===========================================================================*/


/*=============================================================================
* function name: EDDI_SwiPortGetSpanningTree()
*
* function:
*
* parameters:
*
* return value:  LSA_UINT16
*
*==========================================================================*/
static  LSA_UINT16  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortGetSpanningTree( LSA_UINT32               const  HwPortIndex,
                                                                      LSA_UINT16               const  CurrentSpanningTreeState,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  NRT_Control;
    LSA_BOOL    Closed;
    LSA_BOOL    Disabled;
    LSA_BOOL    Learning;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortGetSpanningTree->HwPortIndex:0x%X CurrentSpanningTreeState:0x%X",
                      HwPortIndex, CurrentSpanningTreeState);

    NRT_Control = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);

    Learning = (LSA_BOOL)((EDDI_GetBitField32(NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning) == 1) ? LSA_TRUE : LSA_FALSE);

    EDDI_SwiMiscGetClosedDisabled(HwPortIndex, &Closed, &Disabled, pDDB);

    EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPortGetSpanningTree, Closed:0x%X Disabled:0x%X Learning:0x%X", Closed, Disabled, Learning);

    //Closed don't care
    if ((Disabled) && (!Learning))
    {
        return EDD_PORT_STATE_DISABLE;
    }

    if ((Closed) && (!Disabled) && (Learning))
    {
        return EDD_PORT_STATE_LEARNING;
    }

    if ((!Closed) && (!Disabled) && (Learning))
    {
        return EDD_PORT_STATE_FORWARDING;
    }

    //Blocking or Listening
    if ((Closed) && (!Disabled) && (!Learning))
    {
        if (CurrentSpanningTreeState == EDD_PORT_STATE_LISTENING)
        {
            return EDD_PORT_STATE_LISTENING;
        }
        else
        {
            return EDD_PORT_STATE_BLOCKING;
        }
    }

    //Closed, Disabled and Learning-Enable all 0
    if ((!Disabled) && (!Learning) && (!Closed))
    {
        return EDD_PORT_STATE_FORWARDING_WITHOUT_LEARNING;
    }

    EDDI_Excp("EDDI_SwiPortGetPortState, Learning: Closed:", EDDI_FATAL_ERR_EXCP, Learning, Closed);

    return EDD_PORT_STATE_FORWARDING_WITHOUT_LEARNING;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortSetSpanningTree()
*
* function:      Only Unicast
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetSpanningTree( LSA_UINT32               const  HwPortIndex,
                                                            LSA_UINT16               const  HwPortState,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  NRT_Control;
    LSA_BOOL    Closed;
    LSA_BOOL    Disabled;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetSpanningTree->HwPortIndex:0x%X HwPortState:0x%X", HwPortIndex, HwPortState);

    //store PortState for Ext Indication
    pDDB->SWITCH.LinkIndExtPara[HwPortIndex].PortState = HwPortState;

    NRT_Control = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);

    switch (HwPortState)
    {
        case EDD_PORT_STATE_DISABLE:
        {
            Closed   = LSA_FALSE; //don't care, because of compiler
            Disabled = LSA_TRUE;
            EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning, 0);
        }
        break;

        case EDD_PORT_STATE_BLOCKING:
        {
            Closed   = LSA_TRUE;
            Disabled = LSA_FALSE;
            EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning, 0);
        }
        break;

        case EDD_PORT_STATE_LISTENING:
        {
            Closed   = LSA_TRUE;
            Disabled = LSA_FALSE;
            EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning, 0);
        }
        break;

        case EDD_PORT_STATE_LEARNING:
        {
            Closed   = LSA_TRUE;
            Disabled = LSA_FALSE;
            EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning, 1);
        }
        break;

        case EDD_PORT_STATE_FORWARDING:
        {
            Closed   = LSA_FALSE;
            Disabled = LSA_FALSE;
            EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning, 1);
        }
        break;

        case EDD_PORT_STATE_FORWARDING_WITHOUT_LEARNING:
        {
            Closed   = LSA_FALSE;
            Disabled = LSA_FALSE;
            EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnLearning, 0);
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_SwiPortSetSpanningTree", EDDI_FATAL_ERR_EXCP, HwPortIndex, HwPortState);
            return;
        }
    }

    if (HwPortState == EDD_PORT_STATE_DISABLE)
    {
        EDDI_SwiMiscSetDisable(HwPortIndex, Disabled, pDDB);
    }
    else
    {
        EDDI_SwiMiscSetClosedDisabled(HwPortIndex, Closed, Disabled, pDDB);
    }

    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, NRT_Control, pDDB);

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetSpanningTree<-");
}
/*---------------------- end [subroutine] ---------------------------------*/

/*===========================================================================*/
/*                            extern function definition                     */
/*===========================================================================*/

#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)

/*=============================================================================
* function name: EDDI_SwiPortSetPortMonitor()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetPortMonitor( EDDI_UPPER_SWI_SET_PORT_MONITOR_PTR_TYPE  const  pUsrSetPortMonitor,
                                                           EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB )
{
    LSA_UINT32                     UsrPortIndex;
    LSA_UINT32              const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                     NRT_Control;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetPortMonitor->");

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        NRT_Control = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);

        // no Mapping User structure
        EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnMonitorAcyclic,
                            (pUsrSetPortMonitor->PortIDMonitorAcyclic[UsrPortIndex]) ? 1 : 0);

        // no Mapping User structure
        EDDI_SetBitField32(&NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnMonitorCyclic,
                            (pUsrSetPortMonitor->PortIDMonitorCyclic[UsrPortIndex]) ? 1 : 0);

        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, NRT_Control, pDDB);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetPortMonitor<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortGetPortMonitor()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortGetPortMonitor( EDDI_UPPER_SWI_GET_PORT_MONITOR_PTR_TYPE  const  pUsrGetPortMonitor,
                                                           EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB )
{
    LSA_UINT32                     UsrPortIndex;
    LSA_UINT32              const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                     NRT_Control;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortGetPortMonitor->");

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        NRT_Control = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);

        // no Mapping User structure
        pUsrGetPortMonitor->PortIDMonitorAcyclic[UsrPortIndex] =
            (LSA_BOOL)((EDDI_GetBitField32(NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnMonitorAcyclic) == 1) ? LSA_TRUE : LSA_FALSE);

        // no Mapping User structure
        pUsrGetPortMonitor->PortIDMonitorCyclic [UsrPortIndex] =
            (LSA_BOOL)((EDDI_GetBitField32(NRT_Control, EDDI_SER_NRT_CTRL_BIT__EnMonitorCyclic) == 1) ? LSA_TRUE : LSA_FALSE);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortGetPortMonitor<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortSetMirrorPort()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetMirrorPort( EDDI_UPPER_SWI_SET_MIRROR_PORT_PTR_TYPE  const  pUsrSetMirrorPort,
                                                            EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB )
{
    LSA_UINT32                         UsrPortIndex;
    LSA_UINT32                  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                         Monitor_Control;
    LSA_UINT16                         CntPorts;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetMirrorPort >");

    CntPorts            = 0;
    Monitor_Control = 0;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        // no Mapping User structure
        if (pUsrSetMirrorPort->PortID[UsrPortIndex])
        {
            CntPorts++;
            switch (HwPortIndex)
            {
                case 0:
                    EDDI_SetBitField32(&Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P0, 1);
                    break;
                case 1:
                    EDDI_SetBitField32(&Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P1, 1);
                    break;
                case 2:
                    EDDI_SetBitField32(&Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P2, 1);
                    break;
                case 3:
                    EDDI_SetBitField32(&Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P3, 1);
                    break;
                default:
                    break;
            }
        }
    }

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPortSetMirrorPort, < ->Monitor_Control:0x%X CntPorts:0x%X",
                      Monitor_Control, CntPorts);

    if (CntPorts > 1)
    {
        return EDD_STS_ERR_PARAM;
    }

    IO_x32(MONITOR_CTRL) = Monitor_Control;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortGetMirrorPort()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortGetMirrorPort( EDDI_UPPER_SWI_GET_MIRROR_PORT_PTR_TYPE  const  pUsrGetMirrorPort,
                                                          EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB )
{
    LSA_UINT32                         UsrPortIndex;
    LSA_UINT32                  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                         Monitor_Control;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortGetMirrorPort->");

    Monitor_Control = IO_x32(MONITOR_CTRL);

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        LSA_BOOL           bMirrorPort;

        switch (HwPortIndex)
        {
            case 0:
            {
                bMirrorPort =  (LSA_BOOL)((0 != EDDI_GetBitField32(Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P0)) ? LSA_TRUE : LSA_FALSE);
                break;
            }

            case 1:
            {
                bMirrorPort =  (LSA_BOOL)((0 != EDDI_GetBitField32(Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P1)) ? LSA_TRUE : LSA_FALSE);
                break;
            }

            case 2:
            {
                bMirrorPort =  (LSA_BOOL)((0 != EDDI_GetBitField32(Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P2)) ? LSA_TRUE : LSA_FALSE);
                break;
            }

            case 3:
            {
                bMirrorPort =  (LSA_BOOL)((0 != EDDI_GetBitField32(Monitor_Control, EDDI_SER_MONITOR_CTRL_BIT__P3)) ? LSA_TRUE : LSA_FALSE);
                break;
            }

            default:
                EDDI_Excp("EDDI_SwiPortGetMirrorPort", EDDI_FATAL_ERR_EXCP, HwPortIndex, 0);
                return;
        }

        //User structure
        pUsrGetMirrorPort->PortID[UsrPortIndex] = bMirrorPort;
    }

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortGetMirrorPort<-Monitor_Control:0x%X", Monitor_Control);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE


/*=============================================================================
* function name: EDDI_SwiPortSetSpMoConfig()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetSpMoConfig( EDDI_SWI_SET_PORT_SPEEDMODE_PTR_TYPE  const  pUsrSetPortSpeedMode,
                                                            EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB )
{
    LSA_BOOL           EDDI_GenLinkInterrupt2Execute;
    LSA_BOOL           PortSpeedMode[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT8          LinkSpeed;
    LSA_UINT8          LinkMode;
    LSA_BOOL           bAutoNeg, bRestartConnect;
    LSA_RESULT         Result;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetSpMoConfig->");

    //ParameterCheck
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        /* user view - no port mapping */
        switch (pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex])
        {
            case EDD_LINK_UNCHANGED:
            case EDD_LINK_AUTONEG:
            case EDD_LINK_100MBIT_HALF:
            case EDD_LINK_100MBIT_FULL:
            case EDD_LINK_10MBIT_HALF:
            case EDD_LINK_10MBIT_FULL:
            case EDD_LINK_1GBIT_HALF:
            case EDD_LINK_1GBIT_FULL:
            case EDD_LINK_10GBIT_HALF:
            case EDD_LINK_10GBIT_FULL:
                break;
            default:
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_PORT_SPEEDMODE);
                EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPortSetSpMoConfig, UsrPortIndex:0x%X PortIDLinkSpeedMode:0x%X",
                                  UsrPortIndex, pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex]);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    EDDI_GenLinkInterrupt2Execute = LSA_FALSE;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //no Mapping user structure
        if (EDD_LINK_UNCHANGED == pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex])
        {
            continue;
        }

        //check LinkSpeedMode allowed
        if (!EDDI_IfLinkSpeedModeInCapability(HwPortIndex, pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex], pDDB))
        {
            EDDI_SET_DETAIL_ERR (pDDB, EDDI_ERR_PHY_CAPABILITIES);
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPortSetSpMoConfig, EDDI_IfLinkSpeedModeInCapability, PortIDLinkSpeedMode:0x%X AutonegCapAdvertised:0x%X",
                              pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex], pDDB->Glob.PortParams[HwPortIndex].AutonegCapAdvertised);
            return EDD_STS_ERR_PARAM;
        }

        //Force a LinkDown when changing from Autoneg to 100MBFD -> parallel detection
        if ( (pDDB->SWITCH.LinkPx[HwPortIndex].LinkStatus                   == EDD_LINK_UP          ) &&
             (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config == EDD_LINK_AUTONEG     ) &&
             ( (pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex]     == EDD_LINK_100MBIT_FULL) ||
               (pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex]     == EDD_LINK_100MBIT_HALF) ) )
        {
            bRestartConnect = LSA_TRUE;
        }
        else
        {
            bRestartConnect = LSA_FALSE;
        }

        //store Autoneg or Speed and Mode
        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config = pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex];

        #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
        (LSA_VOID)pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct(pDDB,
                                                                                        HwPortIndex,
                                                                                        (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config == EDD_LINK_AUTONEG) ?
                                                                                        SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_ON : SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_OFF);
        #endif

        //PHY is still switched off, just store settings
        if (pDDB->pLocal_SWITCH->DisableLinkChangePHYSMI)
        {
            continue;
        }

        //Convert SpeedMode to AutoNeg or LinkSpeed and LinkMode
        EDDI_SwiMiscChangeSpMoConfigToSpMoPhy(pDDB, pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex],
                                              &bAutoNeg, &LinkSpeed, &LinkMode);

        //Set Speed and Mode or AutoNeg at PHY 
        EDDI_SwiPhySetSpMo(HwPortIndex, bAutoNeg, LinkSpeed, LinkMode, pDDB);

        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].Autoneg = (LSA_UINT8)(((bAutoNeg) ? EDD_AUTONEG_ON : EDD_AUTONEG_OFF));

        EDDI_GenLinkInterrupt2Execute = LSA_TRUE;
        PortSpeedMode[HwPortIndex]    = LSA_TRUE;

        //Set Speed and Mode only if AutoNeg is switched off, otherwise use LinkChange_INT
        if (EDD_LINK_AUTONEG != pUsrSetPortSpeedMode->PortIDLinkSpeedMode[UsrPortIndex])
        {
            //Set Speed and Mode at MAC and NRT-IF
            EDDI_SwiPortSetSpMoNRTMAC(HwPortIndex, LinkSpeed, LinkMode, pDDB);

            //if the speed does not change, the fixed settings are used -> no LinkChange_Int will be generated.
            if ( (EDD_LINK_UP == pDDB->SWITCH.LinkPx[HwPortIndex].LinkStatus) &&
                 (LinkSpeed   == pDDB->SWITCH.LinkPx[HwPortIndex].LinkSpeed ) &&
                 (LinkMode    != pDDB->SWITCH.LinkPx[HwPortIndex].LinkMode  ) )
            {
                pDDB->SWITCH.LinkPx[HwPortIndex].LinkSpeed = LinkSpeed;
                pDDB->SWITCH.LinkPx[HwPortIndex].LinkMode  = LinkMode;
            }

            /* When using a Broadcom-PHY, it is not possible to set PowerDown, only by using the Superisolate-Bit      */
            /* A Powerdown is simulatet, the following PowerOn will not generate a further Link-INT, as the Link still */
            /* exists.                                                                                                 */
            /* pDDB->SWITCH.LinkPx[HwPortIndex].LinkStatus is still auf EDD_LINK_DOWN                                  */
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            if (   (pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive)
                && (EDDI_PHY_TRANSCEIVER_BROADCOM == pDDB->Glob.PortParams[HwPortIndex].PhyTransceiver))
            {
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkSpeed = LinkSpeed;
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkMode  = LinkMode;
            }
            #endif
        }

        if (EDDI_PRMChangePortState_IsPlugged(pDDB, HwPortIndex)  &&  bRestartConnect)
        {
            Result = EDDI_TRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_OFF, LSA_FALSE /*bRaw*/);
            if (Result != EDD_STS_OK)
            {
                EDDI_Excp("EDDI_SwiPortSetSpMoConfig, EDDI_TRASetPowerDown 1", EDDI_FATAL_ERR_EXCP, Result, 0);
                return Result;
            }

            EDDI_WAIT_10_NS(pDDB->hSysDev,  8000UL ); //80us waiting time

            Result = EDDI_TRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_ON, LSA_FALSE /*bRaw*/);
            if (Result != EDD_STS_OK)
            {
                EDDI_Excp("EDDI_SwiPortSetSpMoConfig, EDDI_TRASetPowerDown 2", EDDI_FATAL_ERR_EXCP, Result, 0);
                return Result;
            }
            EDDI_SwiPortSetPhyStatus(HwPortIndex, EDDI_PHY_POWER_ON, LSA_TRUE, pDDB);
        }
    }

    if (EDDI_GenLinkInterrupt2Execute)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortSetSpMoConfig, simulate LinkInterrupt");

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            /* inform port state machines */
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            if (PortSpeedMode[HwPortIndex])
            {
                EDDI_SyncPortStmsLinkChange(pDDB,
                                            HwPortIndex,
                                            pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus,
                                            pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed,
                                            pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode);
            }
        }

        EDDI_GenLinkInterrupt(&PortSpeedMode[0], pDDB);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetSpMoConfig<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortSetPortState()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetPortState( EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE  const  pRQBSetPortStat,
                                                           EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB )
{
    LSA_BOOL           PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPortSetPortState->");

    //ParameterCheck
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        //user view no port mapping
        switch (pRQBSetPortStat->PortIDState[UsrPortIndex])
        {
            //allowed states
            case EDD_PORT_STATE_DISABLE:
            case EDD_PORT_STATE_BLOCKING:
            case EDD_PORT_STATE_LISTENING:
            case EDD_PORT_STATE_LEARNING:
            case EDD_PORT_STATE_FORWARDING:
            case EDD_PORT_STATE_UNCHANGED:
            case EDD_PORT_STATE_FORWARDING_WITHOUT_LEARNING:
                break;
            default:
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_PORT_STATUS);
                EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPortSetPortState ->UsrPortIndex:0x%X PortIDState:0x%X",
                                  UsrPortIndex, pRQBSetPortStat->PortIDState[UsrPortIndex]);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (EDD_PORT_STATE_UNCHANGED == pRQBSetPortStat->PortIDState[UsrPortIndex])
        {
            continue;
        }
        else
        {
            PortChanged[HwPortIndex] = LSA_TRUE;
        }

        /* Before we activate a PortState which sets the Port to "CLOSED" we switch */
        /* to ORG-Prio so no frame will be lost on transition to CLOSED             */
        if ((pRQBSetPortStat->PortIDState[UsrPortIndex] == EDD_PORT_STATE_BLOCKING  ) ||
            (pRQBSetPortStat->PortIDState[UsrPortIndex] == EDD_PORT_STATE_LISTENING ) ||
            (pRQBSetPortStat->PortIDState[UsrPortIndex] == EDD_PORT_STATE_LEARNING  ))
        {
            EDDI_SwiPNSetSyncMACPrio(pDDB,EDDI_SWI_FDB_PRIO_ORG);   
        }

        EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetPortState, UsrPortID:%u CurrentState:%u SetState:%u LinkStatus:%u",
                          UsrPortIndex, pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState, pRQBSetPortStat->PortIDState[UsrPortIndex], pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus);

        /* if CLEAR_TX_QUEUE is currently running, store the new portstate only! */
        if (pDDB->pLocal_SWITCH->ResetSQ_Reestablish_SpanningTreeState)
        {
            pDDB->pLocal_SWITCH->ResetSQ_SpanningTreeState[HwPortIndex] = pRQBSetPortStat->PortIDState[UsrPortIndex];
        }
        else
        {
            /* change the PortState only if not Linkdown. If LinkDown we only store the */
            /* state for a later LinkUp. While Link Down the DISABLED-State shall not   */
            /* be changed!                                                              */
            if (   (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus != EDD_LINK_DOWN)
                && (!pDDB->Glob.PhyPowerOff[HwPortIndex]))
            {
                EDDI_SwiPortSetSpanningTree(HwPortIndex, pRQBSetPortStat->PortIDState[UsrPortIndex], pDDB);
            }

            //The states of Closed, Disabled and Learning have to be reestablished after a transition from LinkDown to LinkUp.
            //Therefore store current state here.
            pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState = pRQBSetPortStat->PortIDState[UsrPortIndex];
        }
    }

    //Maybe no Port is on BLOCKING anymore. Update the PRIO if so.
    EDDI_SwiPNUpdateSyncMACPrio(pDDB);

    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortSetPortState<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortGetPortState()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortGetPortState( EDD_UPPER_SWI_GET_PORT_STATE_PTR_TYPE         pRQBGetPortStat,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB )
{
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortGetPortState->");

    //The PortState is changed by a Link-Down without calling EDDI_SwiPortSetPortState.
    //Because of this, the current state has to be determined from the ports,
    //the SpanningTreeState can not be used!
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //Old SpanningTreeState is needed to differentiate Blocking and Listening
        pRQBGetPortStat->PortIDState[UsrPortIndex] = EDDI_SwiPortGetSpanningTree(HwPortIndex, pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState, pDDB);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortGetPortState<-");
}
/*---------------------- end [subroutine] ---------------------------------*/




/*=============================================================================
* function name: EDDI_SwiPortFlushFilterDB()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortFlushFilterDB( EDD_UPPER_RQB_PTR_TYPE             pRQB,
                                                            LSA_BOOL                 *  const  bIndicate,
                                                            EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_UINT32        UsrPortIndex;
    LSA_UINT32        HwPortIndex;
    LSA_UINT32        const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32        QueuePort;
    LSA_UINT32        ResetSendQueueCmd;
    LSA_RESULT        ret;
    LSA_UINT32        Tmp, Ctr;
    LSA_BOOL          bQuit = LSA_FALSE;

    EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE const pRQBParam = (EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE)pRQB->pParam;

    //Call CBF immediately
    *bIndicate = LSA_TRUE;

    //check RQB
    if (   (pRQBParam->PortCnt < 1)
        || (pRQBParam->PortCnt > EDD_CFG_MAX_PORT_CNT))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPortFlushFilterDB, illegal Portcnt (%u)!", pRQBParam->PortCnt);
        return EDD_STS_ERR_PARAM;
    }

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPortFlushFilterDB, ->ResetSQ_Cmd_Fcode:0x%X", (LSA_UINT32)pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode);

    //Is EDD_SRV_SWITCH_FLUSH_FILTERING_DB still running
    if (pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode != SWI_FCODE_NOT_DEFINIED)
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    //*** Clear all relevant ringports ***
    //Check ports
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //1. store current state of port to reestablish it after the command has finished.  
        //   here, ALL ports have to be stored, to take provision for changing ringports during the running command
        pDDB->pLocal_SWITCH->ResetSQ_SpanningTreeState[HwPortIndex] = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState;
    }

    Tmp = 0;
    for (Ctr=0; Ctr<pRQBParam->PortCnt; Ctr++)
    {
        if (   (pRQBParam->PortID[Ctr] < 1)
            || (pRQBParam->PortID[Ctr] > PortMapCnt))
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPortFlushFilterDB, pRQBParam->PortID[%u]=%u is no valid UsrPortID!", 
                              Ctr, pRQBParam->PortID[Ctr]);
            bQuit = LSA_TRUE;
        }
        else
        {
            UsrPortIndex = pRQBParam->PortID[Ctr]-1;
            HwPortIndex  = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            if (   (EDD_PORT_STATE_BLOCKING != pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState)
                && (EDD_PORT_STATE_DISABLE  != pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState) )
            {
                EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPortFlushFilterDB, UsrPortIdx/HwPortIdx %u/%u in command is in state %u (expected: BLOCKING or DISABLED)!", 
                                  UsrPortIndex, HwPortIndex, pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState);
                bQuit = LSA_TRUE;
            }
            else
            {
                //2. Port is set to BLOCKING ==> Add this port to the command  
                QueuePort = EDDI_PmGetMask0210(pDDB, UsrPortIndex);
                Tmp |= QueuePort;
            }
        }
    }

    if (bQuit)
    {
        return EDD_STS_ERR_PARAM;
    }

    //call CBF after timer expires
    *bIndicate = LSA_FALSE;

    pDDB->pLocal_SWITCH->ResetSQ_Reestablish_SpanningTreeState = LSA_TRUE;

    ResetSendQueueCmd = EDDI_HOST2IRTE32(Tmp);

    //Select type Port-Queue 
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Qu_Type, 0);

    //Clear all send queues
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_0, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_1, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_2, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_3, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_4, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_5, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_6, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_7, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_8, 1);
    EDDI_SetBitField32(&ResetSendQueueCmd, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_9, 1);

    pDDB->pLocal_SWITCH->ResetSQ_pRQB       = pRQB;
    pDDB->pLocal_SWITCH->ResetSQ_Cmd        = EDDI_HOST2IRTE32(ResetSendQueueCmd);   //in BE environment, Reg has to be swapped again
    pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode  = SWI_FLUSH_FILTERING_DB_TX_FCODE_RESET_TX_QUEUE;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPortFlushFilterDB, ResetSQ_pRQB:0x%X ResetSQ_Cmd:0x%X",
                      (LSA_UINT32)pDDB->pLocal_SWITCH->ResetSQ_pRQB, pDDB->pLocal_SWITCH->ResetSQ_Cmd);

    //EDDI_SwiPortFlushFilterDB Step 1: Start Timer (20ms), on expiry EDDI_SwiPortResetSQTimeout() is called
    //a delay of at least 1,5ms is needed after closing the port, to allow for finishing and queuing
    //of a currently running frame-receive
    //Timer runs with a granularity of 10ms -> wait 10...20ms
    ret = EDDI_StartTimer(pDDB, pDDB->pLocal_SWITCH->ResetSQ_TimerId, (LSA_UINT16)2);
    if (EDD_STS_OK != ret)
    {
        EDDI_Excp("EDDI_SwiPortFlushFilterDB, EDDI_StartTimer ResetSQ_TimerId", EDDI_FATAL_ERR_EXCP, ret, 0);
        return EDD_STS_ERR_RESOURCE;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortResetSQTimeout()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortResetSQTimeout( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    LSA_RESULT                      ret;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortResetSQTimeout->");

    //stop timer
    ret = EDDI_StopTimer(pDDB, pDDB->pLocal_SWITCH->ResetSQ_TimerId);
    if (EDD_STS_OK != ret)
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortResetSQTimeout, StopTimer Retval:0x%X", ret);
    }

    if (EDDI_NULL_PTR == context)
    {
        EDDI_Excp("EDDI_SwiPortResetSQTimeout, EDDI_NULL_PTR == context", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //check if channel has not been closed in the meantime
    if (   (0 == pDDB->pLocal_SWITCH->ResetSQ_Cmd)
        && (SWI_FCODE_NOT_DEFINIED == pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
        && !pDDB->pLocal_SWITCH->ResetSQ_Reestablish_SpanningTreeState )
    {
        //channel has been closed in the meantime, command 
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortResetSQTimeout, ResetSQ_Cmd is 0, LSA channel has been closed. RQB already returned. pRQB:0x%X, Service:0x%X", 
                pDDB->pLocal_SWITCH->ResetSQ_pRQB, pDDB->pLocal_SWITCH->ServiceCanceled);
    }
    else
    {
        if (SWI_FCODE_NOT_DEFINIED == pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
        {
            EDDI_Excp("EDDI_SwiPortResetSQTimeout, SWI_FCODE_NOT_DEFINIED == pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if (EDDI_NULL_PTR == pDDB->pLocal_SWITCH->ResetSQ_pRQB)
        {
            EDDI_Excp("EDDI_SwiPortResetSQTimeout, EDDI_NULL_PTR == ResetSQ_pRQB", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if (0 == pDDB->pLocal_SWITCH->ResetSQ_Cmd)
        {
            EDDI_Excp("EDDI_SwiPortResetSQTimeout 3 0 == ResetSQ_Cmd", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_ENTER_COM_S();
        if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
        {
            EDDI_EXIT_COM_S();
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortResetSQTimeout, CmdIF.State <> EDDI_CMD_IF_FREE, State:0x%X Service:0x%X", pDDB->CmdIF.State, pDDB->CmdIF.pActAsyncRQB->Service);
    
            //Queue is not free, enqueue request
            EDDI_SERSheduledRequest(pDDB, pDDB->pLocal_SWITCH->ResetSQ_pRQB, (LSA_UINT32)EDDI_SwiPortResetSQTimeout);
            return;
        }

        //EDDI_SwiPortFlushFilterDB Step 2: Execute Command "RESET_TX_QUEUE"
        EDDI_SERAsyncCmd(FCODE_RESET_TX_QUEUE, pDDB->pLocal_SWITCH->ResetSQ_Cmd, 0UL, pDDB, pDDB->pLocal_SWITCH->ResetSQ_pRQB, LSA_FALSE /*bLock*/);
        EDDI_EXIT_COM_S();
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortResetSQTimeout<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortResetSQTimeoutConf()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortResetSQTimeoutConf( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_SWI_TRACE_02(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortResetSQTimeoutConf->ResetSQ_Cmd_Fcode:0x%X ResetSQ_pRQB:0x%X",
                      pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode, (LSA_UINT32)pRQB);

    if (SWI_CLEAR_TX_QUEUES_FCODE_RESET_TX_QUEUE == pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
    {
        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);

        pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd   = 0;
        pHDB->pDDB->pLocal_SWITCH->ResetSQ_pRQB      = EDDI_NULL_PTR;
        pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode = SWI_FCODE_NOT_DEFINIED;
    }
    else if (SWI_FLUSH_FILTERING_DB_TX_FCODE_RESET_TX_QUEUE == pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
    {
        //EDDI_SwiPortFlushFilterDB Step 3: Execute Command "CLEAR_FDB"

        EDDI_ENTER_COM_S();
        if (pHDB->pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
        {
            EDDI_EXIT_COM_S();
            EDDI_SWI_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortResetSQTimeoutConf, wants to execute FCODE_CLEAR_FDB, CmdIF.State <> EDDI_CMD_IF_FREE ->State:0x%X", pHDB->pDDB->CmdIF.State);

            //Queue is not free, enqueue request
            EDDI_SERSheduledRequest(pHDB->pDDB, pRQB, (LSA_UINT32)EDDI_SwiPortResetSQTimeoutConf);
            return;
        }

        EDDI_SERAsyncCmd(FCODE_CLEAR_FDB, pHDB->pDDB->Glob.LLHandle.AlterAddressTableWait, 0UL, pHDB->pDDB, pRQB, LSA_FALSE /*bLock*/);
        EDDI_EXIT_COM_S();

        //Service EDD_SRV_SWITCH_FLUSH_FILTERING_DB
        pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode = SWI_FLUSH_FILTERING_DB_TX_FCODE_CLEAR_FDB;
    }
    else
    {
        EDDI_Excp("EDDI_SwiPortResetSQTimeoutConf, ResetSQ_Cmd_Fcode", EDDI_FATAL_ERR_EXCP, pHDB->pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode, 0);
    }

    EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPortResetSQTimeoutConf<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortSetSpMoNRTMAC()
*
* function:
*
* parameters:
*
* return value: LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetSpMoNRTMAC( LSA_UINT32               const  HwPortIndex,
                                                          LSA_UINT8                const  LinkSpeed,
                                                          LSA_UINT8                const  LinkMode,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  NRTCtrl;
    LSA_UINT32  MACCtrl;
    LSA_UINT32  MinPream;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetSpMoNRTMAC->LinkSpeed:0x%X LinkMode:0x%X", LinkSpeed, LinkMode);

    NRTCtrl  = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);
    MACCtrl  = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_MAC_CONTROL, pDDB);
    MinPream = 0;

    switch (LinkSpeed)
    {
        case EDD_LINK_SPEED_10:
            EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__Baudrate, 1);
            EDDI_SetBitField32(&MinPream, EDDI_SER_MIN_PREAMBLE_BIT__MinPreamble, pDDB->pLocal_SWITCH->MinPreamble10Mbit);
            break;

        case EDD_LINK_SPEED_100:
            EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__Baudrate, 0);
            EDDI_SetBitField32(&MinPream, EDDI_SER_MIN_PREAMBLE_BIT__MinPreamble, pDDB->pLocal_SWITCH->MinPreamble100Mbit);
            break;

        default:
            EDDI_Excp("EDDI_SwiPortSetSpMoNRTMAC 0", EDDI_FATAL_ERR_EXCP, HwPortIndex, LinkSpeed);
            return;
    }

    switch (LinkMode)
    {
        case EDD_LINK_MODE_HALF:
            EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__FullDuplex, 0);
            EDDI_SetBitField32(&MACCtrl, EDDI_SER_MAC_CTRL_BIT__FullDuplex, 0);
            break;

        case EDD_LINK_MODE_FULL:
            EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__FullDuplex, 1);
            EDDI_SetBitField32(&MACCtrl, EDDI_SER_MAC_CTRL_BIT__FullDuplex, 1);
            break;

        default:
            EDDI_Excp("EDDI_SwiPortSetSpMoNRTMAC 1", EDDI_FATAL_ERR_EXCP, HwPortIndex, LinkMode);
            return;
    }

    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL,  NRTCtrl,  pDDB);
    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_MAC_CONTROL,  MACCtrl,  pDDB);
    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_MIN_PREAMBLE, MinPream, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetSpMoNRTMAC<-NRTCtrl:0x%X MACCtrl:0x%X",
                      NRTCtrl, MACCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortSetiSRTMode()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetiSRTMode( LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  Enable_iSRT,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  NRTCtrl;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetiSRTMode->HwPortIndex:0x%X Enable_iSRT:0x%X", HwPortIndex, Enable_iSRT);

    NRTCtrl = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);

    if (Enable_iSRT)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortSetiSRTMode, Activating   iSRT-Bit, HwPortIndex:0x%X Enable_iSRT:0x%X",
                          HwPortIndex, Enable_iSRT);
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnISRT, 1);
    }
    else
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortSetiSRTMode, Deactivating iSRT-Bit, HwPortIndex:0x%X Enable_iSRT:0x%X",
                          HwPortIndex, Enable_iSRT);
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnISRT, 0);
    }

    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, NRTCtrl, pDDB);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetiSRTMode<-NRTCtrl:0x%X", NRTCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_FRAG_ON)
/*=============================================================================
* function name: EDDI_SwiPortSetEnableSRTPrio()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetEnableSRTPrio( LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  Enable_SRTPrio,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  NRTCtrl;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetEnableSRTPrio, > ->HwPortIndex:0x%X Enable_SRTPrio:0x%X", HwPortIndex, Enable_SRTPrio);

    NRTCtrl = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, pDDB);

    if (Enable_SRTPrio)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortSetEnableSRTPrio,  Activating   iSRT-Bit  HwPortIndex:0x%X Enable_SRTPrio:0x%X",
                          HwPortIndex, Enable_SRTPrio);
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnSRTPriority, 1);
    }
    else
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPortSetEnableSRTPrio,  Deactivating iSRT-Bit  HwPortIndex:0x%X Enable_SRTPrio:0x%X",
                          HwPortIndex, Enable_SRTPrio);
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnSRTPriority, 0);
    }

    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL, NRTCtrl, pDDB);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPortSetEnableSRTPrio, < ->NRTCtrl:0x%X", NRTCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif

/*=============================================================================
* function name: EDDI_SwiPortSetPhyStatus()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortSetPhyStatus( LSA_UINT32               const  HwPortIndex,
                                                         LSA_UINT8                const  PhyPower,
                                                         LSA_BOOL                 const  bGenInd,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    SWI_LINK_IND_EXT_PARAM_TYPE  *  const  pLinkIndExtPara = &pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex];

    switch (PhyPower)
    {
        case EDDI_PHY_POWER_ON:
        {
            //phy switched on
            pLinkIndExtPara->PhyStatus = EDD_PHY_STATUS_ENABLED;
            break;
        }
        case EDDI_PHY_POWER_OFF:
        {
            //phy switched off
            pLinkIndExtPara->PhyStatus = EDD_PHY_STATUS_DISABLED;
            break;
        }
        default: break;
    }

    //check for change
    if (   (bGenInd)
        && (pLinkIndExtPara->PhyStatus != pLinkIndExtPara->LastPhyStatus))
    {
        LSA_BOOL  PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};

        PortChanged[HwPortIndex] = LSA_TRUE;
        EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPortGetDropCtr()
*
* function:
*
* parameters:
*
* return value:  LSA_UINT32
*
*==========================================================================*/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPortGetDropCtr( LSA_UINT32               const  HwPortIndex,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    if (pDDB->LocalIFStats.bOpen[HwPortIndex])
    {
        //Port is OPEN: use corrected dropctr
        return (EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FDROP, pDDB) - pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex]);
    }
    else
    {
        //Port is CLOSED: use last corrected dropctr before entering CLOSED state
        return (pDDB->LocalIFStats.SubstClosedDropCtr[HwPortIndex]);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_swi_port.c                                              */
/*****************************************************************************/
