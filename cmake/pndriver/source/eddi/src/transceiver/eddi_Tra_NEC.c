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
/*  F i l e               &F: eddi_Tra_NEC.c                            :F&  */
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

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)

#include "eddi_dev.h"
#include "eddi_swi_ext.h"
#include "eddi_Tra.h"

#define EDDI_MODULE_ID     M_ID_TRA_NEC
#define LTRC_ACT_MODUL_ID  505

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  Transceiver: NEC (AATHPHYC2)                                              */
/*               ===============                                              */
/*                                                                            */
/*                                                                            */
/*  Library for PHY specific routines                                         */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetPowerDown_NEC()                       */
/*                                                                         */
/* D e s c r i p t i o n: Switch PHY to PowerDown                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT8                const  PHYPower,
                                                           LSA_BOOL                 const  bRaw)
{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetPowerDown_NEC->HwPortIndex:0x%X PhyPower:0x%X", HwPortIndex, PHYPower);

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
/* F u n c t i o n:       EDDI_TRACheckPowerDown_NEC()                     */
/*                                                                         */
/* D e s c r i p t i o n: Check PHY PowerDown                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                        pIsPowerDown if the return value is EDD_STS_OK   */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                      * pIsPowerDown)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pIsPowerDown);
    LSA_UNUSED_ARG(HwPortIndex);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckPowerDown_NEC->HwPortIndex:0x%X", HwPortIndex);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAStartPhy_NEC()                           */
/*                                                                         */
/* D e s c r i p t i o n: Actions to take before PHY-reset is released     */
/*                        Function isn't called at ERTEC200 internal PHY!  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex,
                                                     LSA_BOOL                 const  bPlugCommit)
{
    LSA_UNUSED_ARG(bPlugCommit);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRAStartPhy_NEC->HwPortIndex:0x%X", HwPortIndex);

    if //media type Fiber?
       (pDDB->Glob.PortParams[HwPortIndex].MediaType == EDD_MEDIATYPE_FIBER_OPTIC_CABLE)
    {
        LSA_UINT32  Phy_Special_Modes_Reg;

        //check FX mode enabled
        EDDI_SwiPhyReadMDCA(EDDI_PHY_NEC_SPECIAL_MODES_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Special_Modes_Reg, pDDB);
        if (!EDDI_GetBitField32(Phy_Special_Modes_Reg, EDDI_PHY_NEC_SPECIAL_MODES_BIT__FX_MODE))
        {
            EDDI_Excp("EDDI_TRAStartPhy_NEC, no FX mode enabled at Fiber port", EDDI_FATAL_ERR_EXCP, Phy_Special_Modes_Reg, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAGetSpMoAutoNegOn_NEC()                   */
/*                                                                         */
/* D e s c r i p t i o n: Gets Speed/Mode for Autoneg ON                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                   LSA_UINT32               const  HwPortIndex,
                                                                                   SER_SWI_LINK_PTR_TYPE    const  pBasePx )
{
    LSA_UINT32  Phy_Phy_Special_Control_Status_Reg;

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Special_Control_Status_Reg, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NEC->HwPortIndex:0x%X Phy_Special_Control_Status_Reg:0x%X", HwPortIndex, Phy_Phy_Special_Control_Status_Reg);

    //LinkDown again
    if (0 == EDDI_GetBitField32(Phy_Phy_Special_Control_Status_Reg, EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Autodone))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                          "EDDI_TRAGetSpMoAutoNegOn_NEC, 0 == Phy_Special_Control_Status_Reg.---.Autodone Phy_Special_Control_Status_Reg:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_HALF", Phy_Phy_Special_Control_Status_Reg);
        pBasePx->LinkSpeed = EDD_LINK_UNKNOWN;
        pBasePx->LinkMode  = EDD_LINK_UNKNOWN;
        return SWI_AUTONEG_GET_SPEEDMODE_RESTART; //Restart again
    }

    switch (EDDI_GetBitField32(Phy_Phy_Special_Control_Status_Reg, EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Speed_Indication))
    {
        //001, 10Mbps half-duplex
        case 1:
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NEC, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_HALF", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
            pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
            break;
        //101, 10Mbps full-duplex
        case 5:
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NEC, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_FULL", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
            pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
            break;
        //010, 100BASE-TX half-duplex
        case 2:
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NEC, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_HALF", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
            pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
            break;
        //110, 100BASE-TX full-duplex
        case 6:
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_NEC, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_FULL", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
            pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
            break;
        default:
            EDDI_Excp("EDDI_TRAGetSpMoAutoNegOn_NEC, Phy_Special_Control_Status_Reg", EDDI_FATAL_ERR_LL, Phy_Phy_Special_Control_Status_Reg, 0);
            break;
    }

    return SWI_AUTONEG_GET_SPEEDMODE_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckSpeedModeCapability_NEC()           */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_NEC( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       LSA_UINT32                               const  HwPortIndex,
                                                                       LSA_UINT32 EDDI_LOCAL_MEM_ATTR const  *  const  pAutonegCapability )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckSpeedModeCapability_NEC->HwPortIndex:0x%X", HwPortIndex);

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            //At least the following modes shall be supported:
            if ( 0 == ((EDD_AUTONEG_CAP_10BASET   + 
                        EDD_AUTONEG_CAP_10BASETFD + 
                        EDD_AUTONEG_CAP_100BASETX + 
                        EDD_AUTONEG_CAP_100BASETXFD) & *pAutonegCapability) )
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
            EDDI_Excp("EDDI_TRACheckSpeedModeCapability_NEC, MediaType", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegMappingCapability_NEC()           */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_NEC( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       LSA_UINT32                               const  HwPortIndex,
                                                                       LSA_UINT32 EDDI_LOCAL_MEM_ATTR const  *  const  pConfiguredLinkSpeedAndMode )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegMappingCapability_NEC->HwPortIndex:0x%X", HwPortIndex);

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            if (EDD_LINK_AUTONEG  != *pConfiguredLinkSpeedAndMode)
            {
                return EDD_STS_ERR_PARAM;
            }
        }
        break;

        //Fiber
        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
        {
            if (   (EDD_LINK_100MBIT_HALF != *pConfiguredLinkSpeedAndMode) && (EDD_LINK_100MBIT_FULL != *pConfiguredLinkSpeedAndMode)
                && (EDD_LINK_10MBIT_HALF  != *pConfiguredLinkSpeedAndMode) && (EDD_LINK_10MBIT_FULL  != *pConfiguredLinkSpeedAndMode))
            {
                return EDD_STS_ERR_PARAM;
            }
        }
        break;

        //other values are not allowed
        default:
        {
            EDDI_Excp("EDDI_TRAAutonegMappingCapability_NEC, MediaType:", EDDI_FATAL_ERR_EXCP,
                      pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetMDIX_NEC()                            */
/*                                                                         */
/* D e s c r i p t i o n: Controls the MDIX feature of the PHY.            */
/*                        (Manual control of crossover)                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  HwPortIndex,
                                                      LSA_BOOL                 const  bMDIX )
{
    LSA_UINT32  Phy_Mode_Control_Status_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetMDIX_NEC->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NEC_MODE_CONTROL_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Mode_Control_Status_Reg, pDDB);

    if (bMDIX) //force crossing requested?
    {
        //If Auto-Neg is off: Rx and Tx lines are crossed
        EDDI_SetBitField32(&Phy_Mode_Control_Status_Reg, EDDI_PHY_NEC_MODE_CONTROL_STATUS_BIT__MDIMode, 1);
    }
    else //force crossing not requested
    {
        //If Auto-Neg is off: Rx and Tx lines are not crossed (straight)
        EDDI_SetBitField32(&Phy_Mode_Control_Status_Reg, EDDI_PHY_NEC_MODE_CONTROL_STATUS_BIT__MDIMode, 0);
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NEC_MODE_CONTROL_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Mode_Control_Status_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRARestoreAutoMDIX_NEC()                    */
/*                                                                         */
/* D e s c r i p t i o n: Restores the AutoMDIX feature of the PHY.        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT32               const  HwPortIndex )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRARestoreAutoMDIX_NEC->HwPortIndex:0x%X", HwPortIndex);

    // This PHY doesn't support an AutoMDIX control bit, hence the function doesn't do anything
    LSA_UNUSED_ARG(pDDB);
	LSA_UNUSED_ARG(HwPortIndex);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TraSetFXMode_NEC()                          */
/*                                                                         */
/* D e s c r i p t i o n: Controls the FXMode feature of the PHY.          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  bFXModeON )
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TraSetFXMode_NEC->HwPortIndex:0x%X  FXModeON:0x%X", HwPortIndex, bFXModeON);

    LSA_UNUSED_ARG(pDDB);           //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex);    //satisfy lint!
    LSA_UNUSED_ARG(bFXModeON);      //satisfy lint!

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetSoftReset_NEC()                       */
/*                                                                         */
/* D e s c r i p t i o n: Set Softreset of the PHY.                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  Phy_Basic_Control_Reg;
    LSA_UINT32  start_ticks_10ns;
    LSA_UINT32  tick_cnt = 0;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetSoftReset_NEC->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);
    EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__Reset, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    start_ticks_10ns = IO_GET_CLK_COUNT_VALUE_10NS;
    
    //Wait until the EDDI_PHY_BASIC_CONTROL_BIT__Reset is 0 or until timeout (=500µs)
    while (tick_cnt < EDDI_TIMEOUT_POLL_FOR_PHY_RESET_10NS) 
    {
        EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

        if (0 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__Reset))
        {
            return EDD_STS_OK;
        }

        //Check overflow
        if (IO_GET_CLK_COUNT_VALUE_10NS < start_ticks_10ns)
        {
            tick_cnt += IO_GET_CLK_COUNT_VALUE_10NS;  //Beginning IO_GET_CLK_COUNT_VALUE_10NS from 0
        }
        else
        {        
            tick_cnt += IO_GET_CLK_COUNT_VALUE_10NS - start_ticks_10ns;
        }
    }   

    EDDI_Excp("EDDI_TRASetSoftReset_NEC, PHY was not reset, tick_cnt:0x%X", EDDI_FATAL_ERR_EXCP, tick_cnt, 0);
    return EDD_STS_ERR_EXCP;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  LED blink (show location) implementation for NEC PHY                      */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBeginExternal_NEC()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_BEGIN(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_NEC);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetModeExternal_NEC()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  HwPortIndex,
                                                                    LSA_UINT16               const  LEDMode )
{
    EDDI_LL_LED_BLINK_SET_MODE(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_NEC, LEDMode);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEndExternal_NEC()
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_END(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_NEC);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBegin_NEC()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetMode_NEC()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_UINT16               const  LEDMode )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
    LSA_UNUSED_ARG(LEDMode); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEnd_NEC()
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_NEC

#endif //EDDI_CFG_PHY_TRANSCEIVER_NEC


/*****************************************************************************/
/*  end of file eddi_Tra_NEC.c                                               */
/*****************************************************************************/

