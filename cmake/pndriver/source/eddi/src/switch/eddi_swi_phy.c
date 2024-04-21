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
/*  F i l e               &F: eddi_swi_phy.c                            :F&  */
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
#include "eddi_swi_ext.h"

#if defined (EDDI_CFG_ERTEC_200)
#include "eddi_ext.h"
#include "eddi_ser_e200.h"
#endif

//#include "eddi_ser_ext.h"
#include "eddi_sync_ir.h"
#include "eddi_Tra.h"

#if defined (EDDI_RED_PHASE_SHIFT_ON)
#include "eddi_lock.h"
#include "eddi_crt_phase_ext.h"
#endif

#define EDDI_MODULE_ID     M_ID_SWI_PHY
#define LTRC_ACT_MODUL_ID  312

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_ERTEC_200)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200DeterminePhaseShift( LSA_UINT32              const HwPortIndex,
                                                                        EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif

/*===========================================================================*/
/*                                 definition                                */
/*===========================================================================*/
#define MAX_READ        1000
//#define PHY_TIMEOUT   0x00000012CUL  //CMD-Timeout

/*===========================================================================*/
/*                                 locals                                    */
/*===========================================================================*/
#if defined (EDDI_CFG_ERTEC_200)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200ReadyPHY( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB );
#endif

/*===========================================================================*/
/*                            local function                                 */
/*===========================================================================*/
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyReadWriteMDCATimeout( LSA_UINT16               const  Adress_Register,
                                                                       LSA_UINT16               const  PhyAddr,
                                                                       LSA_UINT32               const  MD_Control,
                                                                       LSA_UINT32               const  lCountRead,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyGetSpMoAutoNegOff( LSA_UINT32              const Port,
                                                                    SER_SWI_LINK_PTR_TYPE         pBasePx,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyAction4AutoNegOff( LSA_UINT32                    HwPortIndex,
                                                                  LSA_UINT32                    PhyBasicStatus,
                                                                  SER_SWI_LINK_PTR_TYPE         pBasePx,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyAction4AutoNegOn1( LSA_UINT32              const HwPortIndex,
                                                                  SER_SWI_LINK_PTR_TYPE   const pBasePx,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyRestart( LSA_UINT32              const HwPortIndex,
                                                        EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

#if defined (EDDI_CFG_ERTEC_200)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200BugAutoneg( LSA_UINT32              const HwPortIndex,
                                                               LSA_BOOL                const bAutoNegOn,
                                                               EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiPhyE200CheckPHY( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                             LSA_UINT32              const HwPortIndex );
#endif


/*=============================================================================
* function name: EDDI_SwiPhySetIsolate()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
/*
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhySetIsolate( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32              HwPortIndex;
    EDDI_SER_PHY_CTRL_TYPE  Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhySetIsolate->");

    for (HwPortIndex = 0; HwPortIndex < pDDB->PM.HwTypeMaxPortIndex; HwPortIndex++)
    {
        EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

        Phy_Basic_Control_Reg.---.Isolate = 1;

        EDDI_SwiPhyWriteMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhySetIsolate<-");
} */
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SWIStartPhys()
*
* function:      Starts all PHYs.
*
* parameters:
*
* return value:  LSA_UINT32
*
*===========================================================================*/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SWIStartPhys( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                            Result;
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                            ActTicks;
    LSA_UINT32                            DiffTicks;
    EDDI_RQB_SWI_SET_PORT_SPEEDMODE_TYPE  UsrSpeedMod;
    #if defined (EDDI_CFG_ERTEC_200)
    LSA_UINT8                             LinkSpeed;
    LSA_UINT8                             LinkMode;
    LSA_BOOL                              bAutoNeg;
    #endif

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWIStartPhys->");

    ActTicks = IO_GET_CLK_COUNT_VALUE_10NS;

    //Do not enable PHY
    if (pDDB->pLocal_SWITCH->DisableLinkChangePHYSMI)
    {
        return ActTicks;
    }

    //Keep to waiting time, as PHY will not be initialized otherwise
    #if defined (EDDI_CFG_ERTEC_200)
    if (!pDDB->Glob.bPhyExtern)
    {
        //Internal PHY ERTEC200

        EDDI_SwiPhyE200ReadyPHY(pDDB);

        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            if (EDD_LINK_AUTONEG != pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config)
            {
                //Convert SpeedMode to AutoNeg or LinkSpeed and LinkMode
                EDDI_SwiMiscChangeSpMoConfigToSpMoPhy(pDDB, pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config,
                                                      &bAutoNeg, &LinkSpeed, &LinkMode);

                //Adjust Speed and Mode in MAC and NRT-IF
                EDDI_SwiPortSetSpMoNRTMAC(HwPortIndex, LinkSpeed, LinkMode, pDDB);
            }

            if //media type Fiber?
               (pDDB->Glob.PortParams[HwPortIndex].MediaType == EDD_MEDIATYPE_FIBER_OPTIC_CABLE)
            {
                #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
                LSA_UINT32  Phy_Special_Modes_Reg;

                //check FX mode enabled
                EDDI_SwiPhyReadMDCA(EDDI_PHY_NEC_SPECIAL_MODES_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Special_Modes_Reg, pDDB);
                if (!EDDI_GetBitField32(Phy_Special_Modes_Reg, EDDI_PHY_NEC_SPECIAL_MODES_BIT__FX_MODE))
                {
                    EDDI_Excp("EDDI_SWIStartPhys, ERTEC200 internal NEC PHY, no FX mode enabled at Fiber port", EDDI_FATAL_ERR_EXCP, Phy_Special_Modes_Reg, 0);
                    return 0;
                }
                #endif
            }
        }
    }
    else
    {
        //External PHY ERTEC200

        DiffTicks = ActTicks - pDDB->pLocal_SWITCH->StartErtecTicks;
        if (DiffTicks < 150000UL)
        {
            EDDI_WAIT_10_NS(pDDB->hSysDev, 150000UL- DiffTicks);
        }

        //Set Autoneg or Speed and Mode before polling of the PHY is switched on
        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            //start PHY
            if (EDD_FEATURE_ENABLE == pDDB->Glob.PortParams[HwPortIndex].PhySupportsHotPlugging)
            {
            LSA_RESULT Response;

                //Step 1: Set FX-Mode (supported transceivers only)
                Response = EDDI_TRASetFXMode(pDDB, HwPortIndex, (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType));
                if (EDD_STS_OK != Response)
                {
                    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWIStartPhys: EDDI_TRASetFXMode returned 0x%X HWPortIndex:%d", 
                                        Response, HwPortIndex);
                    EDDI_Excp("EDDI_SWIStartPhys: EDDI_TRASetFXMode returned an error. ", Response, HwPortIndex, 0);
                    return Response;
                }
            }
            //Step 2: Start transceiver
            pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRAStartPhy(pDDB, HwPortIndex, LSA_FALSE /*bPlugCommit*/);

            UsrSpeedMod.PortIDLinkSpeedMode[UsrPortIndex] = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config;
        }

        //user view no port mapping
        if (EDD_STS_OK != EDDI_SwiPortSetSpMoConfig(&UsrSpeedMod, pDDB))
        {
            EDDI_Excp("EDDI_SWIStartPhys, EDDI_SwiPortSetSpMoConfig", EDDI_FATAL_ERR_LL, 0, 0);
            return 0;
        }
    }
    #else
    //External PHY

    DiffTicks = ActTicks - pDDB->pLocal_SWITCH->StartErtecTicks;
    if (DiffTicks < 150000UL)
    {
        EDDI_WAIT_10_NS(pDDB->hSysDev, 150000UL- DiffTicks);
    }

    //Set Autoneg or Speed and Mode before polling of the PHY is switched on
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //start transceiver 
        if (EDD_FEATURE_ENABLE == pDDB->Glob.PortParams[HwPortIndex].PhySupportsHotPlugging)
        {
        LSA_RESULT Response;

            //Step 1: Set FX-Mode (supported transceivers only)
            Response = EDDI_TRASetFXMode(pDDB, HwPortIndex, (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType));
            if (EDD_STS_OK != Response)
            {
                EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWIStartPhys: EDDI_TRASetFXMode returned 0x%X HWPortIndex:%d", 
                                    Response, HwPortIndex);
                EDDI_Excp("EDDI_SWIStartPhys: EDDI_TRASetFXMode returned an error. ", Response, HwPortIndex, 0);
                return Response;
            }
        }
        //Step 2: Start transceiver
        pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRAStartPhy(pDDB, HwPortIndex, LSA_FALSE /*bPlugCommit*/);

        UsrSpeedMod.PortIDLinkSpeedMode[UsrPortIndex] = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config;
    }

    //user view no port mapping
    if (EDD_STS_OK != EDDI_SwiPortSetSpMoConfig(&UsrSpeedMod, pDDB))
    {
        EDDI_Excp("EDDI_SWIStartPhys, EDDI_SwiPortSetSpMoConfig", EDDI_FATAL_ERR_LL, 0, 0);
        return 0;
    }
    #endif

    //Set PowerDown for PHY
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].PhyPowerOff)
        {
            //only record change, do not generate indication
            EDDI_SwiPortSetPhyStatus(HwPortIndex, EDDI_PHY_POWER_OFF, LSA_FALSE, pDDB);
            Result = EDDI_TRASetPowerDown(pDDB, HwPortIndex, (LSA_UINT8)EDDI_PHY_POWER_OFF, LSA_FALSE /*bRaw*/);
            if (EDD_STS_OK != Result)
            {
                EDDI_Excp("EDDI_SWIStartPhys, EDDI_TRASetPowerDown, HwPortIndex: Result:", EDDI_FATAL_ERR_LL, HwPortIndex, Result);
                return 0;
            }
            EDDI_SwiPortSetPhyStatus(HwPortIndex, EDDI_PHY_POWER_OFF, LSA_TRUE, pDDB);
        }
        else
        {
            EDDI_SwiPortSetPhyStatus(HwPortIndex, EDDI_PHY_POWER_ON, LSA_TRUE, pDDB);
        }
    }

    //Reenable PHY polling after switching the PHY on
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        EDDI_SwiPhyEnableLinkIRQ(HwPortIndex, LSA_TRUE, pDDB);
    }

    return ActTicks;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyGetSpMoAutoNegOff()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyGetSpMoAutoNegOff( LSA_UINT32               const  HwPortIndex,
                                                                     SER_SWI_LINK_PTR_TYPE           pBasePx,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyGetSpMoAutoNegOff->");

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    if (   (1 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB))
        && (1 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB)))
    {
        //reserved
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetSpMoAutoNegOff, HwPortIndex:0x%X, EDD_LINK_UNKNOWN, MSB=1 + LSB=1", HwPortIndex);
        EDDI_Excp("EDDI_SwiPhyGetSpMoAutoNegOff 1", EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode),
                  EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB));
        return;
    }
    else if (   (1 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB))
             && (0 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB)))
    {
        //1000 Mb/s
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetSpMoAutoNegOff, HwPortIndex:0x%X, EDD_LINK_UNKNOWN, 1000 Mbs", HwPortIndex);
        EDDI_Excp("EDDI_SwiPhyGetSpMoAutoNegOff 2", EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode),
                  EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB));
        return;
    }
    else if (   (0 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB))
             && (1 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB)))
    {
        //100Mb/s
        if (1 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode))
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetSpMoAutoNegOff, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_FULL", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
            pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
        }
        else
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetSpMoAutoNegOff, HwPortIndex:0x%X, EDD_LINK_SPEED_100, EDD_LINK_MODE_HALF", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_100;
            pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
        }
    }
    else if (   (0 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB))
             && (0 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB)))
    {
        //10Mb/s
        if (1 == EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode))
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetSpMoAutoNegOff, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_FULL", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
            pBasePx->LinkMode  = EDD_LINK_MODE_FULL;
        }
        else
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetSpMoAutoNegOff, HwPortIndex:0x%X, EDD_LINK_SPEED_10, EDD_LINK_MODE_HALF", HwPortIndex);
            pBasePx->LinkSpeed = EDD_LINK_SPEED_10;
            pBasePx->LinkMode  = EDD_LINK_MODE_HALF;
        }
    }
    else
    {
        EDDI_Excp("EDDI_SwiPhyGetSpMoAutoNegOff 3", EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode),
                  EDDI_GetBitField32(Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB));
        return;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyGetSpMoAutoNegOff<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyReadMDCA()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyReadMDCA( LSA_UINT16               const  Adress_Register,
                                                    LSA_UINT16               const  PhyAddr,
                                                    LSA_UINT32                   *  pMD_Data,
                                                    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    unsigned  long  lCountRead;
    LSA_UINT32      MD_Control;
    LSA_UINT32      PreamSupress_Save;

    MD_Control = IO_x32(MD_CA); //0x00015004 Station Management Data Control and Adress

    PreamSupress_Save = EDDI_GetBitField32(MD_Control, EDDI_SER_MD_CA_BIT__PreamSupress); //store current setting for the access

    lCountRead = 0;
    while ((EDDI_GetBitField32(MD_Control, EDDI_SER_MD_CA_BIT__Busy) == 1) && (lCountRead < MAX_READ))
    {
        lCountRead++;
        MD_Control = IO_x32(MD_CA);
    }

    //EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyReadMDCA, lCountRead:%08X MAX_READ:%08X", lCountRead, MAX_READ);

    if (lCountRead >= MAX_READ)
    {
        EDDI_Excp("EDDI_SwiPhyReadMDCA, MD_Control.---.Busy, 1", EDDI_FATAL_ERR_EXCP, lCountRead, MD_Control);
        return;
    }

    MD_Control = 0;
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__Address, 0x1F & Adress_Register);

    //EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyReadMDCA, HwPortIndex:0x%X PhyAdr:0x%X", HwPortIndex, PhyAdr);

    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__PhyAddr       , 0x1F & PhyAddr);
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__PreamSupress  , 0x01 & PreamSupress_Save);
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__Write         , 0);
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__Busy          , 1);

    IO_x32(MD_CA) = MD_Control;

    lCountRead = 0;
    do
    {
        MD_Control = IO_x32(MD_CA);
        if (1 == EDDI_GetBitField32(MD_Control, EDDI_SER_MD_CA_BIT__Busy))
        {
            //Wait for 1us
            EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);
            lCountRead++;
        }
        else
        {
            break;
        }
    }
    while (lCountRead < MAX_READ);

    if (lCountRead >= MAX_READ)
    {
        EDDI_SwiPhyReadWriteMDCATimeout(Adress_Register, PhyAddr, MD_Control, lCountRead, pDDB);
        EDDI_Excp("EDDI_SwiPhyReadMDCA, MD_Control.---.Busy, 2", EDDI_FATAL_ERR_EXCP, lCountRead, MD_Control);
        return;
    }

    *pMD_Data = IO_x32(MD_DATA);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyWriteMDCA()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyWriteMDCA( LSA_UINT16                         const  Adress_Register,
                                                     LSA_UINT16                         const  PhyAddr,
                                                     LSA_UINT32               const  *  const  pMD_Data,
                                                     EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    unsigned  long  lCountRead;
    LSA_UINT32      MD_Control;
    LSA_UINT32      PreamSupress_Save;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA->Register:0x%X PhyAdr:0x%X", 0x1F & Adress_Register, 0x1F & PhyAddr);

    MD_Control = IO_x32(MD_CA); //0x00015004 Station Management Data Control and Adress

    PreamSupress_Save = EDDI_GetBitField32(MD_Control, EDDI_SER_MD_CA_BIT__PreamSupress); //Store current settings for access
    
    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA, MD_Control:%08X", MD_Control);

    lCountRead = 0;
    while ((EDDI_GetBitField32(MD_Control, EDDI_SER_MD_CA_BIT__Busy) == 1) && (lCountRead < MAX_READ))
    {
        lCountRead++;
        MD_Control = IO_x32(MD_CA);
    }

    //EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA, lCountRead:%08X MAX_READ:%08X 1", lCountRead, MAX_READ);

    if (lCountRead >= MAX_READ)
    {
        EDDI_Excp("EDDI_SwiPhyWriteMDCA 0 MD_Control.---.Busy, 1", EDDI_FATAL_ERR_EXCP, lCountRead, MD_Control);
        return;
    }

    IO_x32(MD_DATA) = *pMD_Data;

    //Send command to PHY
    MD_Control = 0;
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__Address, 0x1F & Adress_Register);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA, PhyAdr:0x%X", PhyAddr);

    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__PhyAddr       , 0x1F & PhyAddr);
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__PreamSupress  , 0x01 & PreamSupress_Save);
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__Write         , 1);
    EDDI_SetBitField32(&MD_Control, EDDI_SER_MD_CA_BIT__Busy          , 1);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA, MD_Control:%08X", MD_Control);

    IO_x32(MD_CA) = MD_Control;

    lCountRead = 0;
    do
    {
        MD_Control = IO_x32(MD_CA);
        if (1 == EDDI_GetBitField32(MD_Control, EDDI_SER_MD_CA_BIT__Busy))
        {
            //Wait for 1us
            EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);
            lCountRead++;
        }
        else
        {
            break;
        }
    }
    while (lCountRead < MAX_READ);

    //EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA, lCountRead:%08X MAX_READ:%08X", lCountRead, MAX_READ);

    if (lCountRead >= MAX_READ)
    {
        EDDI_SwiPhyReadWriteMDCATimeout(Adress_Register, PhyAddr, MD_Control, lCountRead, pDDB);
        EDDI_Excp("EDDI_SwiPhyWriteMDCA 1 MD_Control.---.Busy, 2", EDDI_FATAL_ERR_EXCP, lCountRead, MD_Control);
        return;
    }

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyWriteMDCA<-pMD_Data:%08X", *pMD_Data);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyActionForLinkUp()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyActionForLinkUp( LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT16               const  LinkSpeedMode_Config,
                                                           SER_SWI_LINK_PTR_TYPE    const  pBasePx,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyActionForLinkUp->CycleHigh:0x%X CycleLow:0x%X",
                      pDDB->CycCount.SyncGet.CycleCounterHigh, pDDB->CycCount.SyncGet.CycleCounterHigh);

    //if LinkDownSimulate is active, only saving current Speed and Mode is allowed, no other action
    if (pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive)
    {
        return;
    }

    EDDI_SwiPortSetSpanningTree(HwPortIndex, pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState, pDDB);

    #if defined (EDDI_CFG_ERTEC_200)
    if (!pDDB->Glob.bPhyExtern)
    {
        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].StateDeterminePhaseShift = EDDI_STATE_PHASE_SHIFT_CHECK;
    }
    #endif

    if (EDD_LINK_AUTONEG == LinkSpeedMode_Config)
    {
        //set NRT Speed and Mode
        EDDI_SwiPortSetSpMoNRTMAC(HwPortIndex, pBasePx->LinkSpeed, pBasePx->LinkMode, pDDB);
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    //nothing to do here
    #else
    if (pDDB->SYNC.IrtActivity)
    {
        EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine = &(pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex]);

        //reopen IRT port

        if (   (pRtClass3_Machine->RtClass3_OutputState != EDDI_RT_CLASS3_STATE_OFF)
            && (pDDB->PRM.PDIRData.pRecordSet_A->MetaData.IrtPortUsrTxCnt[HwPortIndex]))
        {
            //enable IRT port
            EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_TRUE, pDDB);
        }
    }
    #endif

    pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].bFireLinkDownAction = LSA_TRUE;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyActionForLinkUp<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyActionForLinkDown()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyActionForLinkDown( LSA_UINT32               const  HwPortIndex,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SwiPhyActionForLinkDown->HwPortIndex:0x%X CycleLow:0x%X",
                      HwPortIndex, pDDB->CycCount.SyncGet.CycleCounterLow);

    //if LinkDownSimulate is active, only saving current Speed and Mode is allowed, no other action
    if (pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive)
    {
        return;
    }

    if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].bFireLinkDownAction)
    {
        LSA_UINT32  WaitTime10ns;

        //always wait 1.5ms, when Link is down and Phy-IF is RMII, the clock delivered is only 10MHz
        WaitTime10ns = 150000UL; //1,5ms have to pass = 1 frame with 10Mbit

        //Do not use EDDI_SwiMiscSetDisable, that EDDI_SwiPortGetSpanningTree is well defined again
        EDDI_SwiPortSetSpanningTree(HwPortIndex, (LSA_UINT16)EDD_PORT_STATE_DISABLE, pDDB);

        //If IRT Port is active, switch off IRTControl-Bit
        if (pDDB->SYNC.IrtActivity)
        {
            EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine = &pDDB->SYNC.PortMachines.RtClass3_Machine[HwPortIndex];

            if (pRtClass3_Machine->RtClass3_OutputState != EDDI_RT_CLASS3_STATE_OFF)
            {
                LSA_UINT32  IrtMaxTimeTicks;

                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyActionForLinkDown, Switching off IRT-TX, HwPortIndex:0x%X", HwPortIndex);

                //disable IRT port
                #if defined (EDDI_RED_PHASE_SHIFT_ON)
                EDDI_ENTER_SYNC_S();
                {
                    EDDI_CRT_PHASE_TX_MACHINE  *  const  pTxMachine = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];

                    //finish a TX-red-shifting-sequence (SOL running)
                    EDDI_CrtPhaseTxTriggerFinish(pTxMachine, pDDB);
                    //switch off and unshift TX
                    EDDI_CrtPhaseTxTrigger(pTxMachine, EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT, pDDB);

                    // In asynchronous mode finishing of IRTTxPort will be done later (in next new cycle Interrupt).
                    // That means IRTTxPort is still active now (!) but we can guarantee that
                    // no IRT Frame will be sent during EDDI_SwiMiscResetPortSendQueue() because:
                    // 1) the FCW-List was switched off via IRT_CONTROL-Register or at least
                    //    queued out with EDDI_CrtPhaseTxTrigger() (by queuing a SOL instead)
                    // 2) we wait for the last FCW to be finished (see IrtMaxTimeTicks below)
                    // 3) The Rest of the IRTClass3Port-STM must be done later -> see EDDI_IrtClass3StmsTrigger
                    //    bHandleAsyncLinkDown is used to inform the IRTClass3Port-STM about this special use case.
                    pRtClass3_Machine->bHandleAsyncLinkDown = LSA_TRUE;
                }
                EDDI_EXIT_SYNC_S();
                #else
                EDDI_SERSetIrtPortActivityTx(HwPortIndex, LSA_FALSE, pDDB);
                #endif

                IrtMaxTimeTicks = EDDI_SyncIrGetTxMaxTimeOfAllPorts(pDDB, pDDB->PRM.PDIRData.pRecordSet_A) / 10;

                // Wait for EOL time to pass, if the IRT phase is longer than the max. NRT Framelength!
                // IRT sendlists have to be deactivated prior to switching the PHY off,
                // that the IRT sendunit has to finish a running IRT frame, thus avoiding a hangup.
                // Background:
                // The actions in the sendlists are coupled to the MAC statemachine. If an IRT/NRT/SRT frame 
                // is hanging in the MAC, while the PHY clock is switched off, the control unit for resetting 
                // the sendlists will hang up.
                // Note:
                // When connected by MII the TX-MAC clock is derived from the PHY 
                // When connected by RMII the TX-MAC clock is generated by itself 

                EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyActionForLinkDown, IrtMaxTimeTicks:0x%X WaitTime10ns:0x%X", 
                                  IrtMaxTimeTicks, WaitTime10ns);
                WaitTime10ns = EDDI_MAX(IrtMaxTimeTicks, WaitTime10ns);
            }
        }

        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPhyActionForLinkDown, Waiting, HwPortIndex:0x%X WaitTime10ns:0x%X",
                          HwPortIndex, WaitTime10ns);
        EDDI_WAIT_10_NS(pDDB->hSysDev, WaitTime10ns);

        //reset send queues
        EDDI_SwiMiscResetPortSendQueue(HwPortIndex, pDDB);
    }

    pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].bFireLinkDownAction = LSA_TRUE;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyActionForLinkDown<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*                            extern function definition                     */
/*===========================================================================*/

/*=============================================================================
* function name: EDDI_SwiPhySetSpMo()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhySetSpMo( LSA_UINT32               const  HwPortIndex,
                                                   LSA_BOOL                 const  bAutoNeg,
                                                   LSA_UINT8                const  LinkSpeed,
                                                   LSA_UINT8                const  LinkMode,
                                                   EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhySetSpMo->HwPortIndex:%08X AutoNeg:%08X LinkMode:%08X LinkSpeed:%08X", 
                      HwPortIndex, bAutoNeg, LinkMode, LinkSpeed);

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhySetSpMo, PHY_Basis_Control_Register:%08X", Phy_Basic_Control_Reg);

    if (bAutoNeg)
    {
        EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__AutoNegEnable, 1);

        #if defined (EDDI_CFG_ERTEC_200)
        EDDI_SwiPhyE200BugAutoneg(HwPortIndex, LSA_TRUE, pDDB);
        #endif

		/* we have to restore AutoMDIX because it will be disabled during forced speed and mode for PHYs with an AutoMDIX control bit */
		if (LSA_OK != pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRARestoreAutoMDIX(pDDB, HwPortIndex))
        {
            EDDI_Excp("EDDI_SwiPhySetSpMo", EDDI_FATAL_ERR_EXCP, HwPortIndex, LinkMode); /* NOTREACHED */
            return;
        }
    }
    else
    {
        EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__AutoNegEnable, 0);

        #if defined (EDDI_CFG_ERTEC_200)
        EDDI_SwiPhyE200BugAutoneg(HwPortIndex, LSA_FALSE, pDDB);
        #endif

		/* The SetMDIX function will disable the AutoMDIX control bit (if available) during forced speed and mode */
        if (LSA_OK != pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetMDIX(pDDB, HwPortIndex, pDDB->Glob.PortParams[HwPortIndex].IsMDIX))
        {
            EDDI_Excp("EDDI_SwiPhySetSpMo", EDDI_FATAL_ERR_EXCP, HwPortIndex, LinkMode);
            return;
        }

        switch (LinkSpeed)
        {
            case EDD_LINK_SPEED_100:
            {
                EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB, 0);
                EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB, 1);
            }
            break;

            case EDD_LINK_SPEED_10:
            {
                EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB, 0);
                EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB, 0);
            }
            break;

            default:
            {
                EDDI_Excp("EDDI_SwiPhySetSpMo", EDDI_FATAL_ERR_EXCP, HwPortIndex, LinkSpeed);
                return;
            }
        }

        switch (LinkMode)
        {
            case EDD_LINK_MODE_FULL:
            {
                EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode, 1);
            }
            break;

            case EDD_LINK_MODE_HALF:
            {
                EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode, 0);
            }
            break;

            default:
            {
                EDDI_Excp("EDDI_SwiPhySetSpMo", EDDI_FATAL_ERR_EXCP, HwPortIndex, LinkMode);
                return;
            }
        }
    }

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    //Data to be sent to PHY
    pDDB->pProfKRamInfo->info.PHY_Basic_Control_Register[HwPortIndex] = Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhySetSpMo<-PHY_Basic_Control_Register:0x%X", Phy_Basic_Control_Reg);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyRestart()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyRestart( LSA_UINT32               const  HwPortIndex,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyRestart->");

    EDDI_SwiPhyReadMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyRestart, PHY_Basic_Control_Register:%08X", Phy_Basic_Control_Reg);

    EDDI_SetBitField32(&Phy_Basic_Control_Reg, EDDI_PHY_BASIC_CONTROL_BIT__RestAutoNeg, 1);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_BASIC_CONTROL_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Basic_Control_Reg, pDDB);

    //Data to be sent to PHY
    pDDB->pProfKRamInfo->info.PHY_Basic_Control_Register[HwPortIndex] = Phy_Basic_Control_Reg;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyRestart<-PHY_Basic_Control_Register:0x%X", Phy_Basic_Control_Reg);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyGetAutoNegReady()
*
* function:      Get LinkState
*
* parameters:
*
* return value:  LSA_BOOL
*
*
*==========================================================================*/
/*static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyGetAutoNegReady( LSA_UINT32                      HwPortIndex,
                                                                     EDDI_SER_PHY_STAT_TYPE          PhyStatus,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
  LSA_UINT32 TimeOut, ClkCountValue1, ClkCountValue2;

  ClkCountValue1 = IO_GET_CLK_COUNT_VALUE_10NS;

  EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetAutoNegReady->HwPortIndex:%08X PhyStatus:%08X EDD_LINK_AUTONEG", HwPortIndex, PhyStatus);

  for (TimeOut = 0; TimeOut < PHY_TIMEOUT; TimeOut++)
  {
    //Auto-Negoation Process completed
    if ( 1 == PhyStatus.---.AutoNegComp )
    {
      return LSA_TRUE;
    }

    if ( 0 == PhyStatus.---.LinkStat )
    {
      // Link is down again
      return LSA_FALSE;
    }

    EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);

    PhyStatus = EDDI_SwiMiscGetRegister (HwPortIndex, REG_PHY_STAT, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetAutoNegReady, HwPortIndex:%08X PhyStatus:%08X EDD_LINK_AUTONEG", HwPortIndex, PhyStatus);
  }

  ClkCountValue2 = IO_GET_CLK_COUNT_VALUE_10NS;

  if (TimeOut == PHY_TIMEOUT)
  {
    EDDI_Excp("EDDI_SwiPhyGetAutoNegReady, EDD_LINK_AUTONEG, AutoNeg not completed, ", EDDI_FATAL_ERR_LL, HwPortIndex, PhyStatus);
  }

  return LSA_FALSE;
} */
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyAction4AutoNegOn1()
*
* function:      Get SpeedMode
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyAction4AutoNegOn1( LSA_UINT32               const  HwPortIndex,
                                                                     SER_SWI_LINK_PTR_TYPE    const  pBasePx,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    SWI_AUTONEG_GET_SPEEDMODE_TYPE  AutonegGetSpeedMode;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyAction4AutoNegOn1->HwPortIndex:0x%X", HwPortIndex);

    AutonegGetSpeedMode = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRAGetSpMoAutoNegOn(pDDB, HwPortIndex, &pBasePx[HwPortIndex]);

    switch (AutonegGetSpeedMode)
    {
        case SWI_AUTONEG_GET_SPEEDMODE_CALL_ASYNC:
        {
            return;
        }
        case SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO:
        case SWI_AUTONEG_GET_SPEEDMODE_LINKDOWN:
        {
            EDDI_SwiPhyActionForLinkDown(HwPortIndex, pDDB);
            EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPhyAction4AutoNegOn1, Link is down");
            return;
        }
        case SWI_AUTONEG_GET_SPEEDMODE_RESTART:
        case SWI_AUTONEG_GET_SPEEDMODE_OK:
        {
            break; //calling EDDI_SwiPhyAction4AutoNegOn2
        }
        case SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT:
        case SWI_AUTONEG_GET_SPEEDMODE_ERROR:
        default:
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPhyAction4AutoNegOn1, HwPortIndex:0x%X AutonegGetSpeedMode:0x%X", 
                              HwPortIndex, AutonegGetSpeedMode);
            EDDI_Excp("EDDI_SwiPhyAction4AutoNegOn1, HwPortIndex: AutonegGetSpeedMode:", EDDI_FATAL_ERR_LL, HwPortIndex, AutonegGetSpeedMode);
            return;
        }
    }

    //Restart and SpeedMode ok
    EDDI_SwiPhyAction4AutoNegOn2(HwPortIndex, pBasePx, pDDB);

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyAction4AutoNegOn1<-HwPortIndex:0x%X", HwPortIndex);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyAction4AutoNegOn2()
*
* function:      Get LinkState
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyAction4AutoNegOn2( LSA_UINT32                      HwPortIndex,
                                                             SER_SWI_LINK_PTR_TYPE           pBasePx,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  PhyBasicStatus;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyAction4AutoNegOn2->HwPortIndex:0x%X", HwPortIndex);

    if //Speed and Mode can be determined?
       ((EDD_LINK_UNKNOWN != pBasePx[HwPortIndex].LinkSpeed) && (EDD_LINK_UNKNOWN != pBasePx[HwPortIndex].LinkMode))
    {
        //action for Link up
        EDDI_SwiPhyActionForLinkUp(HwPortIndex,
                                   pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config,
                                   &pBasePx[HwPortIndex],
                                   pDDB);

        pBasePx[HwPortIndex].LinkStatus = EDD_LINK_UP;
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPhyAction4AutoNegOn2, Link is up");
        return;
    }

    PhyBasicStatus = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_STAT, pDDB);

    //Link is down again
    if (0 == EDDI_GetBitField32(PhyBasicStatus, EDDI_PHY_BASIC_STATUS_BIT__LinkStat))
    {
        EDDI_SwiPhyActionForLinkDown(HwPortIndex, pDDB);
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPhyAction4AutoNegOn2, Link is down, PhyBasicStatus:0x%X", PhyBasicStatus);
        return;
    }

    //LinkUp and no Speed and Mode exist->Restart PHY
    EDDI_SwiPhyRestart(HwPortIndex, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyAction4AutoNegOn2<-HwPortIndex:0x%X PhyBasicStatus:0x%X", HwPortIndex, PhyBasicStatus);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyAction4AutoNegOff()
*
* function:      Get LinkState
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyAction4AutoNegOff( LSA_UINT32                      HwPortIndex,
                                                                     LSA_UINT32                      PhyBasicStatus,
                                                                     SER_SWI_LINK_PTR_TYPE           pBasePx,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyAction4AutoNegOff->HwPortIndex:0x%X PhyBasicStatus:0x%X", HwPortIndex, PhyBasicStatus);

    if (1UL == EDDI_GetBitField32(PhyBasicStatus, EDDI_PHY_BASIC_STATUS_BIT__AutoNegComp))
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SwiPhyAction4AutoNegOff, No Autoneg parametrized, but Bit AutoNegCompleted set, HwPortIndex:0x%X PhyBasicStatus:0x%X", HwPortIndex, PhyBasicStatus);
    }

    //read Speed and Mode
    EDDI_SwiPhyGetSpMoAutoNegOff(HwPortIndex, &pBasePx[HwPortIndex], pDDB);

    //action for Link up
    EDDI_SwiPhyActionForLinkUp(HwPortIndex,
                               pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config,
                               &pBasePx[HwPortIndex],
                               pDDB);

    pBasePx[HwPortIndex].LinkStatus = EDD_LINK_UP;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyAction4AutoNegOff<-HwPortIndex:0x%X PhyBasicStatus:0x%X", HwPortIndex, PhyBasicStatus);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyGetLinkState()
*
* function:      Get LinkState
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyGetLinkState( SER_SWI_LINK_PTR_TYPE           pBasePx,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32            UsrPortIndex;
    volatile  LSA_UINT32  LinkReg;
    LSA_UINT32            PhyBasicStatus;

    LinkReg = IO_R32(LINK_CHANGE); //detect link-change
    //Read the register a second time. Workaround for AP00630318.
    LinkReg = IO_R32(LINK_CHANGE); //detect link-change

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetLinkState->LinkReg:0x%X", LinkReg);

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //default-values for LinkDown
        pBasePx[HwPortIndex].Changed    = LSA_FALSE;
        pBasePx[HwPortIndex].LinkStatus = EDD_LINK_DOWN;
        pBasePx[HwPortIndex].LinkSpeed  = EDD_LINK_UNKNOWN;
        pBasePx[HwPortIndex].LinkMode   = EDD_LINK_UNKNOWN;

        //Linkchange has occured. UsrPortIndex is used here on purpose!
        if (0 != (pDDB->PM.PortMask_01_08[UsrPortIndex] & LinkReg))
        {
            pBasePx[HwPortIndex].Changed = LSA_TRUE;

            PhyBasicStatus = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_STAT, pDDB);

            if (0xFFFF == EDDI_IRTE2HOST32(PhyBasicStatus))
            {
                EDDI_Excp("EDDI_SwiPhyGetLinkState, PHY not exist on Port[PortID]:, PhyBasicStatus:", EDDI_FATAL_ERR_LL, HwPortIndex, PhyBasicStatus);
                return;
            }

            //Link is up
            if (1UL == EDDI_GetBitField32(PhyBasicStatus, EDDI_PHY_BASIC_STATUS_BIT__LinkStat))
            {
                EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetLinkState, LinkUp == 1, PhyBasicStatus:0x%X LinkSpeedMode_Config:0x%X",
                                  PhyBasicStatus, pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config);

                if (EDD_LINK_AUTONEG != pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config)
                {
                    /*** Autoneg switched off ***/
                    EDDI_SwiPhyAction4AutoNegOff(HwPortIndex, PhyBasicStatus, pBasePx, pDDB);
                }
                else
                {
                    /*** Autoneg switched on ***/
                    EDDI_SwiPhyAction4AutoNegOn1(HwPortIndex, pBasePx, pDDB);
                }
            }
            else
            {
                //Link is down
                EDDI_SwiPhyActionForLinkDown(HwPortIndex, pDDB);

                #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
                //NSC Problem with the autoneg calculation
                (LSA_VOID)pDDB->SWITCH.NSCAutoNegBug.Port[HwPortIndex].SwiNSCAutoNegBugStateFct(pDDB, HwPortIndex, SWI_NSC_AUTONEG_BUGS_EVENT_LINKDOWN);
                #endif
            }

            //patch flag, if the link indication has to be suppressed -> indication is generated later (swi_phy_bug.c)
            //if no singleside cable disturbance exists
            pDDB->pLocal_SWITCH->PhyBugs.SwiPhyBugsResetValFct(pDDB, HwPortIndex, &pBasePx[HwPortIndex]);

            //if LinkDownSimulate is active, only saving current Speed and Mode is allowed, no other action
            //save link status for POWERON
            if (pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive)
            {
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus = pBasePx[HwPortIndex].LinkStatus;
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkSpeed  = pBasePx[HwPortIndex].LinkSpeed;
                pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkMode   = pBasePx[HwPortIndex].LinkMode;
                pBasePx[HwPortIndex].Changed                        = LSA_FALSE;
                pBasePx[HwPortIndex].LinkStatus                     = EDD_LINK_DOWN;
                pBasePx[HwPortIndex].LinkSpeed                      = EDD_LINK_UNKNOWN;
                pBasePx[HwPortIndex].LinkMode                       = EDD_LINK_UNKNOWN;
            }
        }

        EDDI_SWI_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyGetLinkState, UsrPortIndex:%08X Changed:%08X LinkStatus:%08X LinkSpeed:%08X LinkMode:%08X",
                          UsrPortIndex, pBasePx[HwPortIndex].Changed, pBasePx[HwPortIndex].LinkStatus, pBasePx[HwPortIndex].LinkSpeed, pBasePx[HwPortIndex].LinkMode);
    } // end for (Port = 1, UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyGetLinkState<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyEnableLinkIRQ()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyEnableLinkIRQ( LSA_UINT32               const  HwPortIndex,
                                                         LSA_BOOL                 const  bEnableLink,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16  PhyAdr;
    LSA_UINT32  PhyCmd;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyEnableLinkIRQ->");

    PhyCmd = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_CMD, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyEnableLinkIRQ, HwPortIndex:%08X PhyCmd:%08X", HwPortIndex, PhyCmd);

    PhyAdr = pDDB->Glob.PortParams[HwPortIndex].PhyAdr;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyEnableLinkIRQ, HwPortIndex:0x%X PhyAdr:0x%X", HwPortIndex, PhyAdr);

    EDDI_SetBitField32(&PhyCmd, EDDI_SER_PHY_CMD_BIT__PhyAddr, 0x1F & PhyAdr);
    EDDI_SetBitField32(&PhyCmd, EDDI_SER_PHY_CMD_BIT__Enable, (bEnableLink)?1:0);

    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_PHY_CMD, PhyCmd, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyEnableLinkIRQ<-HwPortIndex:0x%X PhyCmd:0x%X", HwPortIndex, PhyCmd);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyIsFiber()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
*==========================================================================*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyIsFiber( LSA_UINT32               const  HwPortIndex,
                                                   EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyIsFiber->HwPortIndex:%X MediaType:0x%X",
                      HwPortIndex, pDDB->Glob.PortParams[HwPortIndex].MediaType);

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        case EDD_MEDIATYPE_COPPER_CABLE:
            return LSA_FALSE;

        default:
            return LSA_TRUE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*************************************************************************
* F u n c t i o n:       EDDI_SwiPhyAreAllPhysON()
*
* D e s c r i p t i o n: Global function to check if all PHYs are in PowerUp-state.
*
* A r g u m e n t s:
*
* Return Value:          Returns LSA_FALSE if one of the PHYs is in PowerOff-state.
*                        Otherwise returns LSA_TRUE.
*
***************************************************************************/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyAreAllPhysON( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiPhyAreAllPhysON->");

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32  const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];

        if (pDDB->Glob.PhyPowerOff[HwPortIndex])
        {
            return LSA_FALSE;
        }
    }

    //all PHYs are in PowerUp-state
    return LSA_TRUE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_ERTEC_200)

/*=============================================================================
* function name: SER_E200PhyReset()
*
* comment:       only for ERTEC200 internal PHY
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200Reset( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    volatile  ERTEC200_SCRB_T   *  const  pERTEC200_SCRB = (volatile ERTEC200_SCRB_T *)pDDB->ERTECx00_SCRB_BaseAddr;
    volatile  LSA_UINT32        *         pPHY_Config;
    volatile  LSA_UINT32        *         pPHY_Status;
    LSA_UINT32                            TmpPHY_Config;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200Reset->Location:0x%X bPhyExtern:0x%X", pDDB->ERTEC_Version.Location, pDDB->Glob.bPhyExtern);

    if (pDDB->ERTEC_Version.Location != EDDI_LOC_ERTEC200)
    {
        return;
    }

    if (pDDB->Glob.bPhyExtern)
    {
        return;
    }

    pPHY_Config = (LSA_UINT32 *)(void *)&pERTEC200_SCRB->x000005c_PHY_CONFIG;
    pPHY_Status = (LSA_UINT32 *)(void *)&pERTEC200_SCRB->x0000060_PHY_STATUS;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200Reset, pPHY_Config->reg:0x%X pPHY_Status->reg:0x%X", *pPHY_Config, *pPHY_Status);

    //activate PHY-Reset
    TmpPHY_Config = 0;
    EDDI_SetBitField32(&TmpPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__PHY_RES_SEL, 1);

    *pPHY_Config = TmpPHY_Config;

    //Wait 2ms, as no HW-Counter is running yet
    //EDDI_WAIT_10_NS(pDDB->hSysDev, 200000UL);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200Reset, pPHY_Config->reg:0x%X pPHY_Status->reg:0x%X",
                      *pPHY_Config, *pPHY_Status);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyE200ConfigPHY()
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200ConfigPHY( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    volatile  ERTEC200_SCRB_T    *  const  pERTEC200_SCRB = (volatile ERTEC200_SCRB_T *)pDDB->ERTECx00_SCRB_BaseAddr;
    volatile  LSA_UINT32         *  pPHY_Config;
    volatile  LSA_UINT32         *  pPHY_Status;
    LSA_UINT32                      SwapPHY_Config;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyE200ConfigPHY->");

    if (pDDB->ERTEC_Version.Location != EDDI_LOC_ERTEC200)
    {
        return;
    }

    pPHY_Config = (LSA_UINT32 *)(void *)&pERTEC200_SCRB->x000005c_PHY_CONFIG;
    pPHY_Status = (LSA_UINT32 *)(void *)&pERTEC200_SCRB->x0000060_PHY_STATUS;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200ConfigPHY, pPHY_Config->reg:0x%X pPHY_Status->reg:0x%X", *pPHY_Config, *pPHY_Status);

    SwapPHY_Config = *pPHY_Config;

    if (!pDDB->Glob.bPhyExtern)
    {
        //PHY Reset connected
        if (0 == EDDI_GetBitField32(SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__PHY_RES_SEL))
        {
            EDDI_Excp("EDDI_SwiPhyE200ConfigPHY, PHY_RES_SEL not switched on" ,EDDI_FATAL_ERR_LL, SwapPHY_Config, *pPHY_Status);
            return;
        }

        if (EDDI_PmIsValidHwPortIndex(pDDB, 0))
        {
            //check phy speed and mode capability again mautype
            if (!EDDI_IfLinkSpeedModeInCapability(0,
                                                  pDDB->pLocal_SWITCH->LinkPx[0].LinkSpeedMode_Config,
                                                  pDDB))
            {
                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPhyE200ConfigPHY, HwPortIndex:0x0 LinkSpeedMode_Config:0x%X",
                                  pDDB->pLocal_SWITCH->LinkPx[0].LinkSpeedMode_Config);
                EDDI_Excp("EDDI_SwiPhyE200ConfigPHY, HwPortIndex: LinkSpeedMode_Config:", EDDI_FATAL_ERR_LL,
                          0, pDDB->pLocal_SWITCH->LinkPx[0].LinkSpeedMode_Config);
                return;
            }

            //set speed and mode
            switch (pDDB->pLocal_SWITCH->LinkPx[0].LinkSpeedMode_Config)
            {
                case EDD_LINK_AUTONEG:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN, 0x1);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE,    0x7); //AutoNeg
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB,     0x1);
                    break;

                case EDD_LINK_100MBIT_FULL:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE     , 0x3); //100-Full
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_100MBIT_HALF:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE     , 0x2); //100-Halb
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_10MBIT_FULL:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE     , 0x1); //10-Full
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_10MBIT_HALF:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE     , 0x0); //10-Halb
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB      , 0x1);
                    break;

                default:
                    EDDI_Excp("EDDI_SwiPhyE200ConfigPHY, LinkSpeedMode_Config[0]" ,EDDI_FATAL_ERR_LL, pDDB->pLocal_SWITCH->LinkPx[0].LinkSpeedMode_Config, 0);
                    return;
            }

            //Settings for optical PHY and POF-Transceiver
            if (EDDI_SwiPhyIsFiber((LSA_UINT32)0, pDDB))
            {
                EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_FX_MODE, 0x1); //enable FX
            }
            else
            {
                EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P1_FX_MODE, 0x0); //disable FX
            }
        }

        if (EDDI_PmIsValidHwPortIndex(pDDB, 1))
        {
            /* check phy speed and mode capability again mautype */
            if (!EDDI_IfLinkSpeedModeInCapability(1,
                                                  pDDB->pLocal_SWITCH->LinkPx[1].LinkSpeedMode_Config,
                                                  pDDB))
            {
                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPhyE200ConfigPHY, HwPortIndex:0x1 LinkSpeedMode_Config:0x%X",
                                  pDDB->pLocal_SWITCH->LinkPx[1].LinkSpeedMode_Config);
                EDDI_Excp("EDDI_SwiPhyE200ConfigPHY, HwPortIndex: LinkSpeedMode_Config:", EDDI_FATAL_ERR_LL,
                          1, pDDB->pLocal_SWITCH->LinkPx[1].LinkSpeedMode_Config);
                return;
            }

            //set speed and mode
            switch (pDDB->pLocal_SWITCH->LinkPx[1].LinkSpeedMode_Config)
            {
                case EDD_LINK_AUTONEG:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN  , 0x1);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE     , 0x7); //AutoNeg
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_100MBIT_FULL:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE     , 0x3); //100-Full
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_100MBIT_HALF:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE     , 0x2); //100-Half
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_10MBIT_FULL:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE     , 0x1); //10-Full
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB      , 0x1);
                    break;

                case EDD_LINK_10MBIT_HALF:
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN  , 0x0);
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE     , 0x0); //10-Half
                    EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB      , 0x1);
                    break;

                default:
                    EDDI_Excp("EDDI_SwiPhyE200ConfigPHY, LinkSpeedMode_Config[1]" ,EDDI_FATAL_ERR_LL,
                              pDDB->pLocal_SWITCH->LinkPx[1].LinkSpeedMode_Config, 0);
                    return;
            }

            //Settings for optical PHY and POF-Transceiver
            if (EDDI_SwiPhyIsFiber((LSA_UINT32)1, pDDB))
            {
                EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_FX_MODE, 0x1); //enable FX
            }
            else
            {
                EDDI_SetBitField32(&SwapPHY_Config, ERTEC200_AUX_PHY_CONFIG_BIT__P2_FX_MODE, 0x0); //disable FX
            }
        }
    }
    else
    {
        SwapPHY_Config = 0;
    }

    *pPHY_Config = SwapPHY_Config;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200ConfigPHY<-SwapPHY_Config.reg:0x%X pPHY_Status->reg:0x%X", SwapPHY_Config, *pPHY_Status);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyE200ReadyPHY()
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200ReadyPHY( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyE200ReadyPHY->");

    if (EDDI_PmIsValidHwPortIndex(pDDB, 0))
    {
        EDDI_SwiPhyE200CheckPHY(pDDB, 0);
    }

    if (EDDI_PmIsValidHwPortIndex(pDDB, 1))
    {
        EDDI_SwiPhyE200CheckPHY(pDDB, 1);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyE200CheckPHY()
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200CheckPHY( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex )
{
    volatile  ERTEC200_SCRB_T   *  const  pERTEC200_SCRB = (volatile ERTEC200_SCRB_T *)pDDB->ERTECx00_SCRB_BaseAddr;
    volatile  LSA_UINT32        *         pPHY_Config;
    volatile  LSA_UINT32        *         pPHY_Status;
    LSA_UINT32                            SwapPHY_Status;
    LSA_UINT32                            ActTicks, DiffTicks, val, check_value, i;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyE200CheckPHY->");

    pPHY_Config = (LSA_UINT32  *)(void *)&pERTEC200_SCRB->x000005c_PHY_CONFIG;
    pPHY_Status = (LSA_UINT32  *)(void *)&pERTEC200_SCRB->x0000060_PHY_STATUS;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200CheckPHY, pPHY_Config->reg:0x%X pPHY_Status->reg:0x%X", *pPHY_Config, *pPHY_Status);

    ActTicks = IO_GET_CLK_COUNT_VALUE_10NS;

    DiffTicks = ActTicks - pDDB->pLocal_SWITCH->StartErtecTicks;

    if (!pDDB->Glob.bPhyExtern)
    {
        check_value = 0;
    }
    else
    {
        check_value = 1;
    }

    if (DiffTicks < 600000UL)   // 6ms
    {
        DiffTicks = 600000UL - DiffTicks;
        DiffTicks = DiffTicks / 100;
    }
    else
    {
        DiffTicks = 0;
    }

    for (i = 0; i <= DiffTicks ; i++)
    {
        SwapPHY_Status = *pPHY_Status;
        if (HwPortIndex == 0)
        {
            val = EDDI_GetBitField32(SwapPHY_Status, ERTEC200_AUX_PHY_STATUS_BIT__P1_PWRUPRST);
        }
        else
        {
            val = EDDI_GetBitField32(SwapPHY_Status, ERTEC200_AUX_PHY_STATUS_BIT__P2_PWRUPRST);
        }

        if (check_value != val)
        {
            return;
        }

        EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL); //wait 1us
    }

    EDDI_Excp("EDDI_SwiPhyE200CheckPHY, Phy not ready yet", EDDI_FATAL_ERR_LL, *pPHY_Config, *pPHY_Status);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPhyE200SetPhyExtern()
*
* return value:  LSA_VOID
*
*======================================================== ===================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200SetPhyExtern( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    volatile  ERTEC200_SCRB_T  *  const  pERTEC200_SCRB = (volatile ERTEC200_SCRB_T *)pDDB->ERTECx00_SCRB_BaseAddr;
    volatile  LSA_UINT32       *         pConfig_Reg;
    LSA_UINT32                           tmp;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyE200SetPhyExtern->");

    if (EDDI_LOC_FPGA_XC2_V8000 != pDDB->ERTEC_Version.Location)
    {
        pConfig_Reg = (LSA_UINT32 *)(void *)&pERTEC200_SCRB->x0000008_CONFIG_REG;

        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200SetPhyExtern, pConfig_Reg->reg:0x%X", *pConfig_Reg);

        tmp = *pConfig_Reg;

        //an external PHY is connected
        if (   (1 == EDDI_GetBitField32(tmp, ERTEC200_AUX_CONFIG_REG_BIT__Config_6))
            && (1 == EDDI_GetBitField32(tmp, ERTEC200_AUX_CONFIG_REG_BIT__Config_5))
            && (1 == EDDI_GetBitField32(tmp, ERTEC200_AUX_CONFIG_REG_BIT__Config_2)))
        {
            pDDB->Glob.bPhyExtern = LSA_TRUE;
        }
        else
        {
            pDDB->Glob.bPhyExtern = LSA_FALSE;
        }
    }
    else
    {
        //FPGA-Board always has an external PHY
        pDDB->Glob.bPhyExtern = LSA_TRUE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyPhaseShift_Timer()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyPhaseShift_Timer( LSA_VOID  *  const  context )
{
    LSA_UINT32                      UsrPortIndex;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].StateDeterminePhaseShift == EDDI_STATE_PHASE_SHIFT_FIRE_IND)
        {
            EDDI_SwiPhyE200DeterminePhaseShift(HwPortIndex, pDDB);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyE200DeterminePhaseShift()             */
/*                                                                         */
/* D e s c r i p t i o n: calculate and correct phase-shift-delay of       */
/*                        older PHYs in RX-delay                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200DeterminePhaseShift( LSA_UINT32               const  HwPortIndex,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  PhaseIndication;
    LSA_BOOL    PortChanged[EDDI_MAX_IRTE_PORT_CNT] = {LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE};

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyE200DeterminePhaseShift->");

    //only for internal PHY-Phase-Shift-Delay determination allowed
    if (pDDB->Glob.bPhyExtern)
    {
        return;
    }

    if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus == EDD_LINK_DOWN)
    {
        return;
    }

    PortChanged[HwPortIndex] = LSA_TRUE;

    //reset flag
    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].StateDeterminePhaseShift = EDDI_STATE_PHASE_SHIFT_DO_NOTHING;

    //Register PhaseIndication
    EDDI_SwiPhyReadMDCA((LSA_UINT16)27, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &PhaseIndication, pDDB);

    //Bit 8-10
    PhaseIndication = EDDI_GetBitField32(PhaseIndication, 10, 8);
    //  PhaseIndication >>=8;
    //  PhaseIndication &=0x7;
    PhaseIndication *=8; //8ns step

    //generate LinkIndExt
    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].PortRxDelay = pDDB->Glob.PortParams[HwPortIndex].DelayParams.PortRxDelay + PhaseIndication;
    EDDI_GenLinkInterrupt(&PortChanged[0], pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200DeterminePhaseShift<-HwPortIndex:0x%X PhaseIndication:0x%X", HwPortIndex, PhaseIndication);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPhyE200BugAutoneg()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyE200BugAutoneg( LSA_UINT32               const  HwPortIndex,
                                                                  LSA_BOOL                 const  bAutoNegOn,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Phy_Mode_Control_Status_Reg;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPhyE200BugAutoneg->");

    //only allowed for internal ERTEC200 PHY
    if (pDDB->Glob.bPhyExtern)
    {
        return;
    }

    EDDI_SwiPhyReadMDCA(EDDI_PHY_NEC_MODE_CONTROL_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Mode_Control_Status_Reg, pDDB);

    EDDI_SetBitField32(&Phy_Mode_Control_Status_Reg, EDDI_PHY_NEC_MODE_CONTROL_STATUS_BIT__AutoMDIXen, (bAutoNegOn)?1:0);

    EDDI_SwiPhyWriteMDCA(EDDI_PHY_NEC_MODE_CONTROL_STATUS_REG_ADR, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, &Phy_Mode_Control_Status_Reg, pDDB);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPhyE200BugAutoneg<-HwPortIndex:0x%X bAutoNegOn:0x%X", HwPortIndex, bAutoNegOn);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_ERTEC_200

/*=============================================================================
* function name: EDDI_SwiPhyReadWriteMDCATimeout()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*==========================================================================*/
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPhyReadWriteMDCATimeout( LSA_UINT16               const  Adress_Register,
                                                                       LSA_UINT16               const  PhyAddr,
                                                                       LSA_UINT32               const  MD_Control,
                                                                       LSA_UINT32               const  lCountRead,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32 lCountReadLocal;
    #if defined (EDDI_CFG_REV6)
    volatile LSA_UINT32 SMIConfig   = IO_x32(SMI_CONFIGURATION);
    #else
    LSA_UINT32 const SMIConfig   = 0;
    #endif
    volatile LSA_UINT32 ClkCtrl     = IO_x32(Clk_Control);
    volatile LSA_UINT32 SWICtrl     = IO_x32(SWI_CTRL);
    volatile LSA_UINT32 SWIStatus   = IO_x32(SWI_STATUS);
    volatile LSA_UINT32 MD_ControlLocal;

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPhyReadWriteMDCA, TIMEOUT Adress_Register:%d, PhyAddr:%d, lCountRead:%d MAX_READ:%d)", 
        Adress_Register, PhyAddr, lCountRead, MAX_READ);
    EDDI_SWI_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPhyReadWriteMDCA, TIMEOUT MD_Control:0x%X, SMIConfig:0x%X, ClkCtrl:0x%X, SWICtrl:0x%X, SWIStatus:0x%X)", 
        MD_Control, SMIConfig, ClkCtrl, SWICtrl, SWIStatus);

    lCountReadLocal = 0;
    do
    {
        MD_ControlLocal = IO_x32(MD_CA);
        if (1 == EDDI_GetBitField32(MD_ControlLocal, EDDI_SER_MD_CA_BIT__Busy))
        {
            //Wait for 1us
            EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);
            lCountReadLocal++;
        }
        else
        {
            break;
        }
    }
    while (lCountReadLocal < (MAX_READ*10));

    #if defined (EDDI_CFG_REV6)
    SMIConfig   = IO_x32(SMI_CONFIGURATION);
    #endif
    ClkCtrl     = IO_x32(Clk_Control);
    SWICtrl     = IO_x32(SWI_CTRL);
    SWIStatus   = IO_x32(SWI_STATUS);
    EDDI_SWI_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SwiPhyReadWriteMDCA, REREAD MD_Control:0x%X, SMIConfig:0x%X, ClkCtrl:0x%X, SWICtrl:0x%X, SWIStatus:0x%X, lCountRead:%d, MAX_READ:%d", 
        MD_ControlLocal, SMIConfig, ClkCtrl, SWICtrl, SWIStatus, lCountReadLocal, MAX_READ*10);

	LSA_UNUSED_ARG (PhyAddr);
	LSA_UNUSED_ARG (MD_Control);
	LSA_UNUSED_ARG (lCountRead);
	LSA_UNUSED_ARG (SWICtrl);
	LSA_UNUSED_ARG (SWIStatus);
	LSA_UNUSED_ARG (Adress_Register);
	LSA_UNUSED_ARG (SMIConfig);
	LSA_UNUSED_ARG (ClkCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*****************************************************************************/
/*  end of file eddi_swi_phy.c                                               */
/*****************************************************************************/

