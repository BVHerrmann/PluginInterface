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
/*  F i l e               &F: eddi_Tra_TI.c                             :F&  */
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

#if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)

#include "eddi_dev.h"
#include "eddi_swi_ext.h"
#include "eddi_Tra.h"

#define EDDI_MODULE_ID     M_ID_TRA_TI
#define LTRC_ACT_MODUL_ID  507

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  Transceiver: TI (TLK111)                                                  */
/*               ===========                                                  */
/*                                                                            */
/*                                                                            */
/*  Library for PHY specific routines                                         */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAGetSpMoAutoNegOn_TI()                    */
/*                                                                         */
/* D e s c r i p t i o n: Gets Speed/Mode for Autoneg ON                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                  LSA_UINT32               const  HwPortIndex,
                                                                                  SER_SWI_LINK_PTR_TYPE    const  pBasePx )
{
    LSA_UINT32  Phy_Basic_Status_Reg;
    LSA_UINT32  Phy_Autoneg_Link_Partner_Ability_Reg;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAGetSpMoAutoNegOn_TI->HwPortIndex:0x%X", HwPortIndex);

    Phy_Basic_Status_Reg = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_STAT, pDDB);

    //No AutoNeg any more
    if (0 == EDDI_GetBitField32(Phy_Basic_Status_Reg, EDDI_PHY_BASIC_STATUS_BIT__AutoNegComp))
    {
        return SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO;
    }

    //Read according to IEEE
    EDDI_SwiPhyReadMDCA(EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Autoneg_Link_Partner_Ability_Reg, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X Autoneg_Link_Partner_Ability_Reg:0x%X", HwPortIndex, Phy_Autoneg_Link_Partner_Ability_Reg);

    if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100TXFDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_FULL", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
        pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100T4))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X, Base100T4", HwPortIndex);
        EDDI_Excp("EDDI_TRAGetSpMoAutoNegOn_TI, Base100T4", EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100T4), 0);
        return SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100TXHDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_HALF", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
        pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base10TFDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_FULL", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
        pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
    }
    else if (EDDI_GetBitField32(Phy_Autoneg_Link_Partner_Ability_Reg, EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base10THDup))
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_HALF", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
        pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
    }
    else
    {
        //Linkchange UP->DOWN occured
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAGetSpMoAutoNegOn_TI, HwPortIndex:0x%X, EDD_LINK_UNKNOWN, EDD_LINK_UNKNOWN", HwPortIndex);
        pBasePx->LinkSpeed = EDD_LINK_UNKNOWN;
        pBasePx->LinkMode  = EDD_LINK_UNKNOWN;
    }

    return SWI_AUTONEG_GET_SPEEDMODE_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetPowerDown_TI()                        */
/*                                                                         */
/* D e s c r i p t i o n: Switch PHY to PowerDown                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_UINT8                const  PHYPower,
                                                          LSA_BOOL                 const  bRaw)

{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetPowerDown_TI->HwPortIndex:0x%X PHYPower:0x%X", HwPortIndex, PHYPower);

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
/* F u n c t i o n:       EDDI_TRACheckPowerDown_TI()                      */
/*                                                                         */
/* D e s c r i p t i o n: Switch PHY to PowerDown                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                        pIsPowerDown if the return value is EDD_STS_OK   */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_BOOL                      * pIsPowerDown)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pIsPowerDown);
    LSA_UNUSED_ARG(HwPortIndex);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckPowerDown_TI->HwPortIndex:0x%X", HwPortIndex);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAStartPhy_TI()                            */
/*                                                                         */
/* D e s c r i p t i o n: Actions to take before PHY-reset is released     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    LSA_UINT32               const  HwPortIndex,
                                                    LSA_BOOL                 const  bPlugCommit)
{
    LSA_UINT32  Phy_Identifier_Reg_1;
    LSA_UINT32  Phy_Sw_Strap_Control_Reg_1;
    LSA_UINT32  Phy_Phy_Control_Reg;
    LSA_UINT32  Timeout = 0x20000UL;

    //ATTENTION: this function is optimized!

    LSA_UNUSED_ARG(bPlugCommit);

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRAStartPhy_TI->HwPortIndex:0x%X", HwPortIndex);

    //await end of PHY reset (SW strapping)
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Await end of PHY reset (SW strapping), HwPortIndex:0x%X", HwPortIndex);
    do
    {
        /* There is a macro EDDI_PHY_ID_R2_REG_ADR too which has an different value. */
        /* In this example only _R1_ is used. */
        EDDI_SwiPhyReadMDCA(EDDI_PHY_ID_R1_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Identifier_Reg_1, pDDB);
        Timeout--;
        if (!Timeout)
        {
            EDDI_Excp("EDDI_TRAStartPhy_TI, Timeout occured", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }
    }
    while (Phy_Identifier_Reg_1 == 0xFFFFUL);
    if //error occured?
       (Phy_Identifier_Reg_1 != 0x2000UL)
    {
        EDDI_Excp("EDDI_TRAStartPhy_TI, invalid PhyIdentifierReg1:", EDDI_FATAL_ERR_EXCP, Phy_Identifier_Reg_1, 0);
        return;
    }

    //enable Fast RXDV Detection (JK optimization)
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Enable Fast RXDV Detection (JK optimization), HwPortIndex:0x%X", HwPortIndex);
    EDDI_SwiPhyReadMDCA(EDDI_PHY_TI_SW_STRAP_CONTROL1_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Sw_Strap_Control_Reg_1, pDDB);
    EDDI_SetBitField32(&Phy_Sw_Strap_Control_Reg_1, EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__Fast_RXDV_Detection, 1);

    //check SW-Strap-Config-Done
    if (EDDI_GetBitField32(Phy_Sw_Strap_Control_Reg_1, EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__SW_Strap_Config_Done))
    {
        EDDI_Excp("EDDI_TRAStartPhy_TI, invalid SW-Strap-Config-Done-Flag", EDDI_FATAL_ERR_EXCP, Phy_Sw_Strap_Control_Reg_1, 0);
        return;
    }

    //select LED configuration Mode 1
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Select LED configuration Mode 1, HwPortIndex:0x%X", HwPortIndex);
    EDDI_SetBitField32(&Phy_Sw_Strap_Control_Reg_1, EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__LED_Cfg, 1);

    //enable Enhanced CRS_DV Operation for RMII (RMII Enhanced Mode)
    #if !defined (EDDI_CFG_NO_ENHANCED_CRS_DV)
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Enable Enhanced CRS_DV for RMII (RMII Enhanced Mode), HwPortIndex:0x%X", HwPortIndex);
    EDDI_SetBitField32(&Phy_Sw_Strap_Control_Reg_1, EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__RMII_Enhanced, 1);
    #endif

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_SW_STRAP_CONTROL1_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Sw_Strap_Control_Reg_1, pDDB);

    //signal end of configuration (end of SW strapping)
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Signal end of configuration (end of SW strapping), HwPortIndex:0x%X", HwPortIndex);
    EDDI_SetBitField32(&Phy_Sw_Strap_Control_Reg_1, EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__SW_Strap_Config_Done, 1);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_SW_STRAP_CONTROL1_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Sw_Strap_Control_Reg_1, pDDB);

    //await PHY wake up time
    EDDI_WAIT_10_NS(pDDB->hSysDev, 20000UL); //wait 200us

    //enable AutoMDIX if Autoneg is selected
    EDDI_SwiPhyReadMDCA(EDDI_PHY_TI_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    if (EDD_LINK_AUTONEG == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Enable AutoMDIX (Autoneg enabled), HwPortIndex:0x%X", HwPortIndex);
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_TI_PHY_CONTROL_BIT__Auto_MDIX_Enable, 1);
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAStartPhy_TI, Disable AutoMDIX (Autoneg disabled), HwPortIndex:0x%X", HwPortIndex);
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_TI_PHY_CONTROL_BIT__Auto_MDIX_Enable, 0);
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckSpeedModeCapability_TI()            */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_TI( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                      LSA_UINT32                                const  HwPortIndex,
                                                                      LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pAutonegCapability )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckSpeedModeCapability_TI->HwPortIndex:0x%X", HwPortIndex);

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
            EDDI_Excp("EDDI_TRACheckSpeedModeCapability_TI, invalid MediaType:", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegMappingCapability_TI()            */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_TI( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                      LSA_UINT32                                const  HwPortIndex,
                                                                      LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pConfiguredLinkSpeedAndMode )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegMappingCapability_TI->HwPortIndex:0x%X", HwPortIndex);

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
            EDDI_Excp("EDDI_TRAAutonegMappingCapability_TI, invalid MediaType:", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetMDIX_TI()                             */
/*                                                                         */
/* D e s c r i p t i o n: Controls the MDIX feature of the PHY.            */
/*                        (Manual control of crossover)                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex,
                                                     LSA_BOOL                 const  bMDIX )
{
    LSA_UINT32  Phy_Phy_Control_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetMDIX_TI->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_TI_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    //If MDIX is on Auto MDIX must be off
    EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_TI_PHY_CONTROL_BIT__Auto_MDIX_Enable, 0);

    if (bMDIX) //force crossing requested?
    {
        //If Auto-Neg is off: Rx and Tx lines are crossed
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_TI_PHY_CONTROL_BIT__Force_MDIX, 1);
    }
    else //force crossing not requested
    {
        //If Auto-Neg is off: Rx and Tx lines are not crossed (straight)
        EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_TI_PHY_CONTROL_BIT__Force_MDIX, 0);
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRARestoreAutoMDIX_TI()                     */
/*                                                                         */
/* D e s c r i p t i o n: Restores the AutoMDIX feature of the PHY.        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  Phy_Phy_Control_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRARestoreAutoMDIX_TI->HwPortIndex:0x%X", HwPortIndex);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_TI_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    // enable AutoMDIX
    EDDI_SetBitField32(&Phy_Phy_Control_Reg, EDDI_PHY_TI_PHY_CONTROL_BIT__Auto_MDIX_Enable, 1);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_PHY_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Phy_Control_Reg, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TraSetFXMode_TI()                           */
/*                                                                         */
/* D e s c r i p t i o n: Controls the FXMode feature of the PHY.          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex,
                                                       LSA_BOOL                 const  bFXModeON )
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TraSetFXMode_TI->HwPortIndex:0x%X  FXModeON:0x%X", HwPortIndex, bFXModeON);

    LSA_UNUSED_ARG(pDDB);           //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex);    //satisfy lint!
    LSA_UNUSED_ARG(bFXModeON);      //satisfy lint!

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetSoftReset_TI()                        */
/*                                                                         */
/* D e s c r i p t i o n: Set Softreset of the PHY.                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetSoftReset_TI->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    return EDD_STS_ERR_NOT_IMPL;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  LED blink (show location) implementation for TI PHY TLK111                */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBeginExternal_TI()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_BEGIN(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_TI);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetModeExternal_TI()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  HwPortIndex,
                                                                   LSA_UINT16               const  LEDMode )
{
    EDDI_LL_LED_BLINK_SET_MODE(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_TI, LEDMode);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEndExternal_TI()
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_END(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_TI);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBegin_TI()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  HwPortIndex )
{
    //no actions necessary!

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetMode_TI()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT16               const  LEDMode )
{
    LSA_UINT32  PhyLEDControlReg;

    //control force-led-function via PHY register LED Control
    EDDI_SwiPhyReadMDCA(EDDI_PHY_TI_LED_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDControlReg, pDDB);
    EDDI_SetBitField32(&PhyLEDControlReg, EDDI_PHY_TI_LED_CONTROL_BIT__DRV_LINKLED, 1);
    EDDI_SetBitField32(&PhyLEDControlReg, EDDI_PHY_TI_LED_CONTROL_BIT__DRV_ACTLED, 1);
    EDDI_SetBitField32(&PhyLEDControlReg, EDDI_PHY_TI_LED_CONTROL_BIT__LINKLEDSET, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1); //0 = LED On
    EDDI_SetBitField32(&PhyLEDControlReg, EDDI_PHY_TI_LED_CONTROL_BIT__ACTLEDSET, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1); //0 = LED On
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_LED_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDControlReg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEnd_TI()
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  PhyLEDControlReg;

    //disable force-led-function via PHY register LED Control
    EDDI_SwiPhyReadMDCA(EDDI_PHY_TI_LED_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDControlReg, pDDB);
    EDDI_SetBitField32(&PhyLEDControlReg, EDDI_PHY_TI_LED_CONTROL_BIT__DRV_LINKLED, 0);
    EDDI_SetBitField32(&PhyLEDControlReg, EDDI_PHY_TI_LED_CONTROL_BIT__DRV_ACTLED, 0);
    EDDI_SwiPhyWriteMDCA(EDDI_PHY_TI_LED_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhyLEDControlReg, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_TI

#endif //EDDI_CFG_PHY_TRANSCEIVER_TI


/*****************************************************************************/
/*  end of file eddi_Tra_TI.c                                                */
/*****************************************************************************/

