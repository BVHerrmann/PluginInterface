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
/*  F i l e               &F: eddi_Tra.c                                :F&  */
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
#include "eddi_dev.h"

#include "eddi_swi_ext.h"

#include "eddi_Tra.h"
//#include "eddi_sync_usr.h"

#define EDDI_MODULE_ID     M_ID_TRA
#define LTRC_ACT_MODUL_ID  503

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetPowerDown()                           */
/*                                                                         */
/* D e s c r i p t i o n: Switch the transceiver into state "PowerDown"    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex,
                                                       LSA_UINT8                const  PHYPower,
                                                       LSA_BOOL                 const  bRaw)
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRASetPowerDown->HwPortIndex:0x%X PHYPower:0x%X",
                          HwPortIndex, PHYPower);

    switch (PHYPower)
    {
        case EDDI_PHY_POWER_UNCHANGED:
            return EDD_STS_OK;

        case EDDI_PHY_POWER_ON:
            pDDB->Glob.PhyPowerOff[HwPortIndex] = LSA_FALSE;
            break;

        case EDDI_PHY_POWER_OFF:
            pDDB->Glob.PhyPowerOff[HwPortIndex] = LSA_TRUE;
            break;

        default:
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_PHYPOWER_NOT_DEFINED);
            return EDD_STS_ERR_NOT_IMPL;
    }

    //call PHY specific PowerDown method
    return pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetPowerDown(pDDB, HwPortIndex, PHYPower, bRaw);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckPowerDown()                         */
/*                                                                         */
/* D e s c r i p t i o n: check PHY to PowerDown                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                        pIsPowerDown if the return value is EDD_STS_OK   */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  HwPortIndex,
                                                         LSA_BOOL                      * pIsPowerDown)
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRACheckPowerDown->HwPortIndex:0x%X",
                          HwPortIndex);
    //call PHY specific CheckPowerDown method
    return pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRACheckPowerDown(pDDB, HwPortIndex, pIsPowerDown);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetFXMode()                              */
/*                                                                         */
/* D e s c r i p t i o n: Controls the FXMode feature of the PHY.          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetFXMode(     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  bFXModeON )
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TraSetFXMode->HwPortIndex:0x%X bFXModeON:%d",
                          HwPortIndex, bFXModeON);
    //call PHY specific SetFXMode method
    return pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetFXMode(pDDB, HwPortIndex, bFXModeON);
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAEvaluatePhyType()                        */
/*                                                                         */
/* D e s c r i p t i o n: Evaluates PHY type.                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_PHY_TRANSCEIVER_TYPE                        */
/*                                                                         */
/***************************************************************************/
EDDI_PHY_TRANSCEIVER_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAEvaluatePhyType( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  Phy_OUI;
    LSA_UINT32  Phy_Id_R1_Reg;
    LSA_UINT32  Phy_Id_R2_Reg;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAEvaluatePhyType->HwPortIndex:0x%X", HwPortIndex);

    #if defined (EDDI_CFG_ERTEC_200)
    if (!pDDB->Glob.bPhyExtern)
    {
        #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
        //ERTEC200 internal PHY
        return EDDI_PHY_TRANSCEIVER_NEC;
        #else
        #error "EDDI_CFG_PHY_TRANSCEIVER_NEC must be defined, if ERTEC200 is using its internal PHY!"
        #endif
    }
    #endif

    EDDI_SwiPhyReadMDCA(EDDI_PHY_ID_R1_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Id_R1_Reg, pDDB);
    EDDI_SwiPhyReadMDCA(EDDI_PHY_ID_R2_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Id_R2_Reg, pDDB);

    Phy_OUI  = EDDI_GetBitField32NoSwap(Phy_Id_R1_Reg, 15, 0) << 6UL;
    Phy_OUI |= EDDI_GetBitField32NoSwap(Phy_Id_R2_Reg, 15, 10);

    switch (Phy_OUI)
    {
        #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
        case EDDI_PHY_BCM_REGVALUE_OUI:     //Broadcom-transceiver
        {
            return EDDI_PHY_TRANSCEIVER_BROADCOM;
        }
        #endif

        #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
        case EDDI_PHY_NEC_REGVALUE_OUI:     //NEC-transceiver
        {
            return EDDI_PHY_TRANSCEIVER_NEC;
        }
        #endif

        #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
        case EDDI_PHY_NSC_REGVALUE_OUI:     //NSC-transceiver
        {
            return EDDI_PHY_TRANSCEIVER_NSC;
        }
        #endif

        #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
        case EDDI_PHY_TI_REGVALUE_OUI:      //TI-transceiver
        {
            return EDDI_PHY_TRANSCEIVER_TI;
        }
        #endif

        #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
        case EDDI_CFG_PHY_REGVALUE_OUI_USERSPEC:    //user specific transceiver
        {
            return EDDI_PHY_TRANSCEIVER_USERSPEC;
        }
        #endif

        default:
            EDDI_Excp("EDDI_TRAEvaluatePhyType, invalid Phy_OUI, PHY not supported", EDDI_FATAL_ERR_EXCP, Phy_OUI, 0);
            return EDDI_PHY_TRANSCEIVER_NOT_DEFINED;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_Tra.c                                                   */
/*****************************************************************************/

