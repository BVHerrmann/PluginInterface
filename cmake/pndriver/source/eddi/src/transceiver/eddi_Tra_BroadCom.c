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
/*  F i l e               &F: eddi_Tra_Broadcom.c                       :F&  */
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

#if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)

#include "eddi_dev.h"
#include "eddi_swi_ext.h"
#include "eddi_Tra.h"
#include "eddi_ext.h"

#define EDDI_MODULE_ID     M_ID_TRA_BROADCOM
#define LTRC_ACT_MODUL_ID  504

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  Transceiver: BROADCOM (BCM5221)                                           */
/*               ==================                                           */
/*                                                                            */
/*                                                                            */
/*  Library for PHY specific routines                                         */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetPowerDown_Broadcom()                  */
/*                                                                         */
/* D e s c r i p t i o n: Switch PHY to PowerDown                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex,
                                                                LSA_UINT8                const  PHYPower,
                                                                LSA_BOOL                 const  bRaw)

{
    LSA_UINT32  Phy_Aux_Multi_Phy_Reg;
    LSA_BOOL    PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetPowerDown_Broadcom->HwPortIndex:0x%X PhyPower:0x%X", HwPortIndex, PHYPower);

    switch (PHYPower)
    {
        case EDDI_PHY_POWER_OFF:
        {
            if (!bRaw)
            {
                //EDDI_PHY_POWER_OFF is already executed
                if (pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive)
                {
                    return EDD_STS_OK;
                }
            }
            EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_AUX_MULTI_PHY_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Multi_Phy_Reg, pDDB);
            EDDI_SetBitField32(&Phy_Aux_Multi_Phy_Reg, EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__SuperIsolate, 1);
            EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_AUX_MULTI_PHY_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Multi_Phy_Reg, pDDB);

            if (!bRaw)
            {
                //save the last link state
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus;
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkSpeed  = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed;
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkMode   = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode;

                //During simulate link down a real link down was coming e.g. cable disconnect
                if (pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus == EDD_LINK_DOWN)
                {
                    pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive = LSA_TRUE;
                    return EDD_STS_OK;
                }

                //simulate link down
                pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus = EDD_LINK_DOWN;
                pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed  = EDD_LINK_UNKNOWN;
                pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode   = EDD_LINK_UNKNOWN;

                //action for link down
                EDDI_SwiPhyActionForLinkDown(HwPortIndex, pDDB);

                pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive = LSA_TRUE;
            }
        }
        break;

        case EDDI_PHY_POWER_ON:
        {
            SER_SWI_LINK_TYPE  LinkState;

            if (!bRaw)
            {
                //EDDI_PHY_POWER_ON is already executed
                if (!pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive)
                {
                    return EDD_STS_OK;
                }
            }

            EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_AUX_MULTI_PHY_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Multi_Phy_Reg, pDDB);
            EDDI_SetBitField32(&Phy_Aux_Multi_Phy_Reg, EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__SuperIsolate, 0);
            EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_AUX_MULTI_PHY_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Multi_Phy_Reg, pDDB);

            if (!bRaw)
            {
                pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive = LSA_FALSE;

                //during simulate link down a real link down was coming e.g. cable disconnect
                if (pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus == EDD_LINK_DOWN)
                {
                    return EDD_STS_OK;
                }

                pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus = pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus;
                pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed  = pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkSpeed;
                pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode   = pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkMode;

                LinkState.LinkStatus = pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus;
                LinkState.LinkSpeed  = pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkSpeed;
                LinkState.LinkMode   = pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkMode;

                //action for Link up
                EDDI_SwiPhyActionForLinkUp(HwPortIndex,
                                           pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config,
                                           &LinkState,
                                           pDDB);

            }
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_TRASetPowerDown_Broadcom, PHYPower", EDDI_FATAL_ERR_EXCP, PHYPower, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    if (!bRaw)
    {
        //generate a link indication
        PortChanged[HwPortIndex] = LSA_TRUE;

        EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckPowerDown_Broadcom()                */
/*                                                                         */
/* D e s c r i p t i o n: Check PHY PowerDown                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                        pIsPowerDown if the return value is EDD_STS_OK   */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex,
                                                                  LSA_BOOL                      * pIsPowerDown)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pIsPowerDown);
    LSA_UNUSED_ARG(HwPortIndex);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckPowerDown_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAStartPhy_Broadcom()                      */
/*                                                                         */
/* D e s c r i p t i o n: Actions to take before PHY-reset is released     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_BOOL                 const  bPlugCommit)
{
    LSA_UINT32  Phy_Aux_Mode_2_Reg;

    LSA_UNUSED_ARG(bPlugCommit);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRAStartPhy_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    //switch activity-led to traffic-meter-mode (led remains on during heavier traffic)
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_AUX_MODE2_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Mode_2_Reg, pDDB);
    EDDI_SetBitField32(&Phy_Aux_Mode_2_Reg, EDDI_PHY_BCM_AUX_MODE2_BIT__TrafficMeterLEDMode, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_AUX_MODE2_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Mode_2_Reg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAGetSpMoAutoNegOn_Broadcom()              */
/*                                                                         */
/* D e s c r i p t i o n: Gets Speed/Mode for Autoneg ON                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                        LSA_UINT32               const  HwPortIndex,
                                                                                        SER_SWI_LINK_PTR_TYPE    const  pBasePx )
{
    LSA_UINT32  Phy_Basic_Status_Reg;
    LSA_UINT32  Phy_Autoneg_Link_Partner_Ability_Reg;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAGetSpMoAutoNegOn_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    Phy_Basic_Status_Reg = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_STAT, pDDB);

    //No AutoNeg any more
    if (0 == EDDI_GetBitField32(Phy_Basic_Status_Reg, EDDI_PHY_BASIC_STATUS_BIT__AutoNegComp))
    {
        return SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO;
    }

    //Read according to IEEE
    EDDI_SwiPhyReadMDCA(EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Autoneg_Link_Partner_Ability_Reg, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X Autoneg_Link_Partner_Ability_Reg:0x%X", HwPortIndex, Phy_Autoneg_Link_Partner_Ability_Reg);

    if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100TXFDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_FULL", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
        pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100T4))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X, Base100T4", HwPortIndex);
        EDDI_Excp("EDDI_TRAGetSpMoAutoNegOn_Broadcom, Base100T4", EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100T4), 0);
        return SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100TXHDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_HALF", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
        pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base10TFDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_FULL", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
        pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base10THDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_HALF", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
        pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
    }
    else
    {
        //Linkchange UP->DOWN occured
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_Broadcom, HwPortIndex:0x%X, EDD_LINK_UNKNOWN, EDD_LINK_UNKNOWN", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_UNKNOWN;
        pBasePx->LinkMode  = EDD_LINK_UNKNOWN;
    }

    return SWI_AUTONEG_GET_SPEEDMODE_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckSpeedModeCapability_Broadcom()      */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE                     const  pDDB,
                                                                            LSA_UINT32                                  const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR    const *  const  pAutonegCapability )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckSpeedModeCapability_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            //at least the following modes shall be supported:
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

            //1) no autonegotiation is supported.
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
            EDDI_Excp("EDDI_TRACheckSpeedModeCapability_Broadcom, MediaType:", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegMappingCapability_Broadcom()      */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const *  const  pConfiguredLinkSpeedAndMode )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegMappingCapability_Broadcom->HwPortIndex:0x%X", HwPortIndex);

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
            EDDI_Excp("EDDI_TRAAutonegMappingCapability_Broadcom, MediaType:", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetMDIX_Broadcom()                       */
/*                                                                         */
/* D e s c r i p t i o n: Controls the MDIX feature of the PHY.            */
/*                        (Manual control of crossover)                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_BOOL                 const  bMDIX )
{
    LSA_UINT32  Phy_Aux_Error_and_Gen_Status_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetMDIX_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_AUX_ERROR_AND_GEN_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Error_and_Gen_Status_Reg, pDDB);

    if (bMDIX) //force crossing requested?
    {
        //If Auto-Neg is off: Rx and Tx lines are crossed
        EDDI_SetBitField32(&Phy_Aux_Error_and_Gen_Status_Reg, EDDI_PHY_BCM_AUX_ERROR_AND_GEN_STATUS_BIT__MDIXManualSwap, 1);
    }
    else //force crossing not requested
    {
        //If Auto-Neg is off: Rx and Tx lines are not crossed (straight)
        EDDI_SetBitField32(&Phy_Aux_Error_and_Gen_Status_Reg, EDDI_PHY_BCM_AUX_ERROR_AND_GEN_STATUS_BIT__MDIXManualSwap, 0);
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_AUX_ERROR_AND_GEN_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Aux_Error_and_Gen_Status_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRARestoreAutoMDIX_Broadcom()               */
/*                                                                         */
/* D e s c r i p t i o n: Restores the AutoMDIX feature of the PHY.        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  HwPortIndex )
{

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRARestoreAutoMDIX_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    // This PHY doesn't support an AutoMDIX control bit, hence the function doesn't do anything
    LSA_UNUSED_ARG(pDDB);
	LSA_UNUSED_ARG(HwPortIndex);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TraSetFXMode_Broadcom()                     */
/*                                                                         */
/* D e s c r i p t i o n: Controls the FXMode feature of the PHY.          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bFXModeON )
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TraSetFXMode_Broadcom->HwPortIndex:0x%X  FXModeON:0x%X", HwPortIndex, bFXModeON);

    LSA_UNUSED_ARG(pDDB);           //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex);    //satisfy lint!
    LSA_UNUSED_ARG(bFXModeON);      //satisfy lint!

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetSoftReset_Broadcom()                  */
/*                                                                         */
/* D e s c r i p t i o n: Set Softreset of the PHY.                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetSoftReset_Broadcom->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    return EDD_STS_ERR_NOT_IMPL;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  LED blink (show location) implementation for BROADCOM PHY BCM5221         */
/*                                                                            */
/******************************************************************************/

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBeginExternal_Broadcom()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_BEGIN(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_BROADCOM);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetModeExternal_Broadcom()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_UINT32               const  HwPortIndex,
                                                                         LSA_UINT16               const  LEDMode )
{
    EDDI_LL_LED_BLINK_SET_MODE(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_BROADCOM, LEDMode);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEndExternal_Broadcom()
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                     LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_END(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_BROADCOM);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBegin_Broadcom()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex )
{
    //no actions necessary!

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetMode_Broadcom()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex,
                                                                 LSA_UINT16               const  LEDMode )
{
    LSA_UINT32  PhyBroadcomTestReg;
    LSA_UINT32  PhyAuxMod4Reg;

    //enable access to the PHY shadow registers
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);
    EDDI_SetBitField32(&PhyBroadcomTestReg, EDDI_PHY_BCM_TEST_BIT__ShadowRegEnable, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);

    //control force-led-function via PHY shadow register Auxiliary Mode 4
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_AUX_MODE4_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyAuxMod4Reg, pDDB);
    EDDI_SetBitField32(&PhyAuxMod4Reg, EDDI_PHY_BCM_AUX_MODE4_BIT__ForceLED, (LEDMode == EDDI_LED_MODE_ON) ? 1 : 2); //1 = LEDs On
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_AUX_MODE4_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyAuxMod4Reg, pDDB);

    //disable access to the PHY shadow registers
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);
    EDDI_SetBitField32(&PhyBroadcomTestReg, EDDI_PHY_BCM_TEST_BIT__ShadowRegEnable, 0);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEnd_Broadcom()
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  PhyBroadcomTestReg;
    LSA_UINT32  PhyAuxMod4Reg;

    //enable access to the PHY shadow registers
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);
    EDDI_SetBitField32(&PhyBroadcomTestReg, EDDI_PHY_BCM_TEST_BIT__ShadowRegEnable, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);

    //disable force-led-function via PHY shadow register Auxiliary Mode 4
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_AUX_MODE4_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyAuxMod4Reg, pDDB);
    EDDI_SetBitField32(&PhyAuxMod4Reg, EDDI_PHY_BCM_AUX_MODE4_BIT__ForceLED, 0);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_AUX_MODE4_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyAuxMod4Reg, pDDB);

    //disable access to the PHY shadow registers
    EDDI_SwiPhyReadMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);
    EDDI_SetBitField32(&PhyBroadcomTestReg, EDDI_PHY_BCM_TEST_BIT__ShadowRegEnable, 0);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BCM_TEST_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyBroadcomTestReg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM

#endif //EDDI_CFG_PHY_TRANSCEIVER_BROADCOM


/*****************************************************************************/
/*  end of file eddi_Tra_Broadcom.c                                          */
/*****************************************************************************/

