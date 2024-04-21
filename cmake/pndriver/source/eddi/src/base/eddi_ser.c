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
/*  F i l e               &F: eddi_ser.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD interface to SER (Ethernetcontroller)        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  11.02.08    JS    added "IsWireless" support                             */
/*  10.04.08    AB    added "IsMDIX" support                                 */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_ser_ext.h"

#include "eddi_ser.h"

#include "eddi_swi_ext.h"
#include "eddi_nrt_ini.h"
#include "eddi_Tra.h"
//#include "eddi_swi_ptp.h"

#define EDDI_MODULE_ID     M_ID_EDDI_SER
#define LTRC_ACT_MODUL_ID  7

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_ENABLE_MC_FDB)
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_GetAlignSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  dwAlignSize,
                                                            LSA_UINT32               const  dwChangeToAlignVAlue );
#endif

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerIniGlob( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDDI_UPPER_DSB_PTR_TYPE  const  pDSB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerIniGlob2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceSetup_srt(       EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                             EDDI_UPPER_DSB_PTR_TYPE         const  pDSB,
                                                                   EDDI_SER_NRT_FREE_CCW_PTR_TYPE  const  pCCWDevBaseAddr );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceSetup_acw( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );


/***************************************************************************/
/* F u n c t i o n:       EDDI_GetAlignSize()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_ENABLE_MC_FDB)
static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_GetAlignSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  dwAlignSize,
                                                         LSA_UINT32               const  dwChangeToAlignVAlue )
{
    LSA_UINT32  dwAlignResult;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GetAlignSize->dwAlignSize:0x%X dwChangeToAlignVAlue:0x%X",
                           dwAlignSize, dwChangeToAlignVAlue);

    dwAlignResult = dwChangeToAlignVAlue % dwAlignSize;

    //check alignment
    if (0 == dwAlignResult)
    {
        return dwChangeToAlignVAlue;
    }

    dwAlignResult = dwChangeToAlignVAlue + (dwAlignSize - dwAlignResult);

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GetAlignSize<-dwAlignResult:0x%X", dwAlignResult);

    LSA_UNUSED_ARG(pDDB);

    return dwAlignResult;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_SerIniGlob()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerIniGlob( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDDI_UPPER_DSB_PTR_TYPE  const  pDSB )
{
    LSA_UINT32  UsrPortIndex;
    LSA_UINT32  HwPortIndex;
    LSA_UINT32  MACIndex;

    pDDB->Glob.LLHandle.AlterAddressTableWait = 0;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    pDDB->Glob.LLHandle.AlterAddressTableWait = 0; //stays 0 until a usecase is found
    #endif

    pDDB->Glob.LLHandle.xRT   = pDSB->GlobPara.xRT;
    pDDB->Glob.MaxBridgeDelay = pDSB->GlobPara.MaxBridgeDelay;

    for (HwPortIndex = 0; HwPortIndex < EDD_CFG_MAX_PORT_CNT; HwPortIndex++)
    {
        pDDB->Glob.PortParams[HwPortIndex].PhyTransceiver = EDDI_PHY_TRANSCEIVER_NOT_DEFINED; //is set later!
    }

    //DSB -> no Port-Mapping -> User view
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  AutonegCap;
        LSA_UINT32  AutonegMapCap;
        LSA_UINT16  MAUType;
        LSA_UINT8   MediaType;
        LSA_UINT8   IsPOF;
        LSA_UINT8   FXTransceiverType;        

        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //Get current MAUType, MediaType and Capabilities
        EDDI_LL_GET_PHY_PARAMS(pDDB->hSysDev, HwPortIndex, EDD_LINK_UNKNOWN /*Speed*/, EDD_LINK_UNKNOWN /*Mode*/, &AutonegCap, &AutonegMapCap, &MAUType, &MediaType, &IsPOF, &FXTransceiverType);

        //plausible MediaType
        switch (MediaType)
        {
            case EDD_MEDIATYPE_COPPER_CABLE:
            case EDD_MEDIATYPE_RADIO_COMMUNICATION:
            case EDD_MEDIATYPE_UNKNOWN: 
            {
                break;
            }
            case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
            {
                //plausible IsPOF
                if (   (EDD_PORT_OPTICALTYPE_ISPOF != IsPOF)
                    && (EDD_PORT_OPTICALTYPE_ISNONPOF != IsPOF))
                {
                    EDDI_Excp("EDDI_SerIniGlob, EDDI_LL_GET_PHY_PARAMS, IsPof illegal value", EDDI_FATAL_ERR_EXCP, HwPortIndex, IsPOF);
                    return;
                }

                if (EDD_LINK_100MBIT_FULL != AutonegMapCap)
                {
                    EDDI_Excp("EDDI_SerIniGlob, EDDI_LL_GET_PHY_PARAMS, AutonegMapCap with Fiber Optic Ports is not EDD_LINK_100MBIT_FULL", EDDI_FATAL_ERR_EXCP, AutonegMapCap, 0);
                    return;   
                }

                if (EDD_FX_TRANSCEIVER_MAX <= FXTransceiverType)
                {
                    EDDI_Excp("EDDI_SerIniGlob, EDDI_LL_GET_PHY_PARAMS, FXTransceiverType for Fiber Optic UsrPortIndex is invalid", EDDI_FATAL_ERR_EXCP, UsrPortIndex, FXTransceiverType);
                    return;   
                }

                break;
            }
            default:
            {
                EDDI_Excp("EDDI_SerIniGlob, EDDI_LL_GET_PHY_PARAMS, invalid MediaType", EDDI_FATAL_ERR_EXCP, MediaType, 0);
                return;
            }
        }

        pDDB->Glob.PortParams[HwPortIndex].MediaType                = MediaType;
        pDDB->Glob.PortParams[HwPortIndex].IsPOF                    = IsPOF;
        pDDB->Glob.PortParams[HwPortIndex].FXTransceiverType        = FXTransceiverType;
        pDDB->Glob.PortParams[HwPortIndex].PhyAdr                   = pDSB->GlobPara.PortParams[UsrPortIndex].PhyAdr;

        pDDB->Glob.PortParams[HwPortIndex].AutonegCapAdvertised     = AutonegCap;
        pDDB->Glob.PortParams[HwPortIndex].AutonegMappingCapability = AutonegMapCap;

        pDDB->Glob.PortParams[HwPortIndex].IsWireless               = pDSB->GlobPara.PortParams[UsrPortIndex].IsWireless;
        pDDB->Glob.PortParams[HwPortIndex].IsMDIXDSB                = pDSB->GlobPara.PortParams[UsrPortIndex].IsMDIX;
        pDDB->Glob.PortParams[HwPortIndex].bBC5221_MCModeSet        = pDSB->GlobPara.PortParams[UsrPortIndex].bBC5221_MCModeSet;
        pDDB->Glob.PortParams[HwPortIndex].IsMDIX                   = (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == MediaType)?EDD_PORT_MDIX_DISABLED:pDSB->GlobPara.PortParams[UsrPortIndex].IsMDIX; 
        pDDB->Glob.PortParams[HwPortIndex].IsPulled                 = pDDB->pConstValues->IsPulled;

        //======== portspecific MAC-Adr =======
        for (MACIndex = 0; MACIndex < EDD_MAC_ADDR_SIZE; MACIndex++)
        {
            pDDB->Glob.PortParams[HwPortIndex].MACAddress.MacAdr[MACIndex] = pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[MACIndex];
        }

        pDDB->Glob.LinkDownSimulate[HwPortIndex].bActive    = LSA_FALSE;
        pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkStatus = EDD_LINK_DOWN;
        pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkMode   = EDD_LINK_UNKNOWN;
        pDDB->Glob.LinkDownSimulate[HwPortIndex].LinkSpeed  = EDD_LINK_UNKNOWN;
        pDDB->Glob.PhyPowerOff[HwPortIndex]                 = LSA_FALSE;

        pDDB->Glob.PortModuleSm[HwPortIndex].State          = pDDB->pConstValues->IsPulled ? EDDI_PORT_MODULE_SM_STATE_PULLED : EDDI_PORT_MODULE_SM_STATE_PLUGGED;
    }

    //Flash LED
    pDDB->Glob.LED_OnOffDuration_TimerID      = 0;
    pDDB->Glob.LED_IsRunning                  = LSA_FALSE;
    pDDB->Glob.LED_OnOffDuration_Status       = EDDI_LED_MODE_OFF;
    pDDB->Glob.LED_TotalBlink_Cnt_100ms       = 0;
    pDDB->Glob.LED_OnOff_Cnt_100ms            = 0;
    pDDB->Glob.LED_TotalBlink_Duration_100ms  = 0;
    pDDB->Glob.LED_OnOff_Duration_100ms       = 0;

    /*-------------------------------------------------------------------------*/
    /* Global dynamic table for service EDD_SRV_MULTICAST                      */
    /*-------------------------------------------------------------------------*/
    pDDB->Glob.MCMACTabelleLocal.pBottom = EDDI_NULL_PTR;
    pDDB->Glob.MCMACTabelleLocal.pTop    = EDDI_NULL_PTR;

    /*-------------------------------------------------------------------------*/
    /* Store DSB params for later use                                          */
    /*-------------------------------------------------------------------------*/
    pDDB->Glob.DSBStored.Glob = pDSB->GlobPara;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SerIniGlob2()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerIniGlob2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_UINT32  UsrPortIndex;

    //pDSB -> no Port-Mapping -> User view
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        EDDI_PHY_TRANSCEIVER_TYPE            PhyTransceiver;
        LSA_UINT32                    const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        EDDI_TRA_FCTPTR_TYPE       *  const  pTraFctPtrs = &pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex];
        LSA_UINT8                     const  MediaType   = pDDB->Glob.PortParams[HwPortIndex].MediaType;
        LSA_UINT8                     const  IsPOF       = pDDB->Glob.PortParams[HwPortIndex].IsPOF;
        LSA_UINT8                     const  FXTransceiverType  = pDDB->Glob.PortParams[HwPortIndex].FXTransceiverType;

        PhyTransceiver = EDDI_TRAEvaluatePhyType(pDDB, HwPortIndex);

        pDDB->Glob.PortParams[HwPortIndex].PhySupportsHotPlugging = EDD_FEATURE_DISABLE;

        switch (PhyTransceiver)
        {
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            case EDDI_PHY_TRANSCEIVER_BROADCOM:     //Broadcom-transceiver
            {
                pTraFctPtrs->pTRASetPowerDown             = EDDI_TRASetPowerDown_Broadcom;
                pTraFctPtrs->pTRACheckPowerDown           = EDDI_TRACheckPowerDown_Broadcom;
                pTraFctPtrs->pTRAStartPhy                 = EDDI_TRAStartPhy_Broadcom;
                pTraFctPtrs->pTRAGetSpMoAutoNegOn         = EDDI_TRAGetSpMoAutoNegOn_Broadcom;
                pTraFctPtrs->pTRACheckSpeedModeCapability = EDDI_TRACheckSpeedModeCapability_Broadcom;
                pTraFctPtrs->pTRAAutonegMappingCapability = EDDI_TRAAutonegMappingCapability_Broadcom;
                pTraFctPtrs->pTRASetMDIX                  = EDDI_TRASetMDIX_Broadcom;
                pTraFctPtrs->pTRARestoreAutoMDIX          = EDDI_TRARestoreAutoMDIX_Broadcom;
                pTraFctPtrs->pTRASetFXMode                = EDDI_TraSetFXMode_Broadcom;
                pTraFctPtrs->pTRASetSoftReset             = EDDI_TRASetSoftReset_Broadcom;
                #if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBeginExternal_Broadcom;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetModeExternal_Broadcom;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEndExternal_Broadcom;
                #else
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBegin_Broadcom;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetMode_Broadcom;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEnd_Broadcom;
                #endif
                break;
            }
            #endif
        
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            case EDDI_PHY_TRANSCEIVER_NEC:          //NEC-transceiver
            {
                pTraFctPtrs->pTRASetPowerDown             = EDDI_TRASetPowerDown_NEC;
                pTraFctPtrs->pTRACheckPowerDown           = EDDI_TRACheckPowerDown_NEC;
                pTraFctPtrs->pTRAStartPhy                 = EDDI_TRAStartPhy_NEC;
                pTraFctPtrs->pTRAGetSpMoAutoNegOn         = EDDI_TRAGetSpMoAutoNegOn_NEC;
                pTraFctPtrs->pTRACheckSpeedModeCapability = EDDI_TRACheckSpeedModeCapability_NEC;
                pTraFctPtrs->pTRAAutonegMappingCapability = EDDI_TRAAutonegMappingCapability_NEC;
                pTraFctPtrs->pTRASetMDIX                  = EDDI_TRASetMDIX_NEC;
                pTraFctPtrs->pTRARestoreAutoMDIX          = EDDI_TRARestoreAutoMDIX_NEC;
                pTraFctPtrs->pTRASetFXMode                = EDDI_TraSetFXMode_NEC;
                pTraFctPtrs->pTRASetSoftReset             = EDDI_TRASetSoftReset_NEC;
                #if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC)
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBeginExternal_NEC;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetModeExternal_NEC;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEndExternal_NEC;
                #else
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBegin_NEC;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetMode_NEC;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEnd_NEC;
                #endif
                break;
            }
            #endif

            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            case EDDI_PHY_TRANSCEIVER_NSC:          //NSC-transceiver
            {
                pTraFctPtrs->pTRASetPowerDown             = EDDI_TRASetPowerDown_NSC;
                pTraFctPtrs->pTRACheckPowerDown           = EDDI_TRACheckPowerDown_NSC;
                pTraFctPtrs->pTRAStartPhy                 = EDDI_TRAStartPhy_NSC;
                pTraFctPtrs->pTRAGetSpMoAutoNegOn         = EDDI_TRAGetSpMoAutoNegOn_NSC;
                pTraFctPtrs->pTRACheckSpeedModeCapability = EDDI_TRACheckSpeedModeCapability_NSC;
                pTraFctPtrs->pTRAAutonegMappingCapability = EDDI_TRAAutonegMappingCapability_NSC;
                pTraFctPtrs->pTRASetMDIX                  = EDDI_TRASetMDIX_NSC;
                pTraFctPtrs->pTRARestoreAutoMDIX          = EDDI_TRARestoreAutoMDIX_NSC;
                pTraFctPtrs->pTRASetFXMode                = EDDI_TraSetFXMode_NSC;
                pTraFctPtrs->pTRASetSoftReset             = EDDI_TRASetSoftReset_NSC;
                #if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBeginExternal_NSC;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetModeExternal_NSC;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEndExternal_NSC;
                #else
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBegin_NSC;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetMode_NSC;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEnd_NSC;
                #endif

                pDDB->Glob.PortParams[HwPortIndex].PhySupportsHotPlugging = EDD_FEATURE_ENABLE;
                break;
            }
            #endif

            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            case EDDI_PHY_TRANSCEIVER_TI:           //TI-transceiver
            {
                pTraFctPtrs->pTRASetPowerDown             = EDDI_TRASetPowerDown_TI;
                pTraFctPtrs->pTRACheckPowerDown           = EDDI_TRACheckPowerDown_TI;
                pTraFctPtrs->pTRAStartPhy                 = EDDI_TRAStartPhy_TI;
                pTraFctPtrs->pTRAGetSpMoAutoNegOn         = EDDI_TRAGetSpMoAutoNegOn_TI;
                pTraFctPtrs->pTRACheckSpeedModeCapability = EDDI_TRACheckSpeedModeCapability_TI;
                pTraFctPtrs->pTRAAutonegMappingCapability = EDDI_TRAAutonegMappingCapability_TI;
                pTraFctPtrs->pTRASetMDIX                  = EDDI_TRASetMDIX_TI;
                pTraFctPtrs->pTRARestoreAutoMDIX          = EDDI_TRARestoreAutoMDIX_TI;
                pTraFctPtrs->pTRASetFXMode                = EDDI_TraSetFXMode_TI;
                pTraFctPtrs->pTRASetSoftReset             = EDDI_TRASetSoftReset_TI;
                #if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBeginExternal_TI;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetModeExternal_TI;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEndExternal_TI;
                #else
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBegin_TI;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetMode_TI;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEnd_TI;
                #endif
                break;
            }
            #endif

            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            case EDDI_PHY_TRANSCEIVER_USERSPEC:     //user specific transceiver (PHY)
            {
                pTraFctPtrs->pTRASetPowerDown             = EDDI_TRASetPowerDown_Userspec;
                pTraFctPtrs->pTRACheckPowerDown           = EDDI_TRACheckPowerDown_Userspec;
                pTraFctPtrs->pTRAStartPhy                 = EDDI_TRAStartPhy_Userspec;
                pTraFctPtrs->pTRAGetSpMoAutoNegOn         = EDDI_TRAGetSpMoAutoNegOn_Userspec;
                pTraFctPtrs->pTRACheckSpeedModeCapability = EDDI_TRACheckSpeedModeCapability_Userspec;
                pTraFctPtrs->pTRAAutonegMappingCapability = EDDI_TRAAutonegMappingCapability_Userspec;
                pTraFctPtrs->pTRASetMDIX                  = EDDI_TRASetMDIX_Userspec;
                pTraFctPtrs->pTRARestoreAutoMDIX          = EDDI_TRARestoreAutoMDIX_Userspec;
                pTraFctPtrs->pTRASetFXMode                = EDDI_TRASetFXMode_Userspec;
                pTraFctPtrs->pTRASetSoftReset             = EDDI_TRASetSoftReset_Userspec;
                #if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBeginExternal_Userspec;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetModeExternal_Userspec;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEndExternal_Userspec;
                #else
                pTraFctPtrs->pTRALedBlinkBegin            = EDDI_TRALedBlinkBegin_Userspec;
                pTraFctPtrs->pTRALedBlinkSetMode          = EDDI_TRALedBlinkSetMode_Userspec;
                pTraFctPtrs->pTRALedBlinkEnd              = EDDI_TRALedBlinkEnd_Userspec;
                #endif
                break;
            }
            #endif

            default:
                EDDI_Excp("EDDI_SerIniGlob2, EDDI_TRAEvaluatePhyType, invalid PhyTransceiver", EDDI_FATAL_ERR_EXCP, PhyTransceiver, 0);
                return;
        }

        pDDB->Glob.PortParams[HwPortIndex].PhyTransceiver = PhyTransceiver;

        EDDI_SetDDBDelayParams(pDDB, HwPortIndex, PhyTransceiver, MediaType, FXTransceiverType);

        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LastMediaType   = MediaType;
        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LastIsPOF       = IsPOF;

        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].PortTxDelay     = pDDB->Glob.PortParams[HwPortIndex].DelayParams.PortTxDelay;
        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LastPortTxDelay = pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].PortTxDelay;
        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].PortRxDelay     = pDDB->Glob.PortParams[HwPortIndex].DelayParams.PortRxDelay;
        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LastPortRxDelay = pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].PortRxDelay;

        pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].SyncId0_TopoOk  = EDD_PORT_SYNCID0_TOPO_NOT_OK;

        #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
        /*-------------------------------------------------------------------------*/
        /* NSC Autoneg BUG                                                         */
        /*-------------------------------------------------------------------------*/
        EDDI_TRAAutonegBugInitNSC(pDDB, HwPortIndex);
        #endif
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetDDBDelayParams()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetDDBDelayParams( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                       LSA_UINT32                 const  HwPortIndex,
                                                       EDDI_PHY_TRANSCEIVER_TYPE  const  PhyTransceiver,
                                                       LSA_UINT8                  const  MediaType,
                                                       LSA_UINT8                  const  FXTransceiverType )
{
    EDDI_PHY_DELAY_PARAMS_TYPE  *  pDelayParams;
    EDDI_PHY_TRANSCEIVER_TYPE      PhyTransceiverLocal = PhyTransceiver;
    
    #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
    //adapt BC5221 mode
    if (   (EDDI_PORT_BC5221_MEDIACONV_STRAPPED_LOW == pDDB->Glob.PortParams[HwPortIndex].bBC5221_MCModeSet)
        && (EDDI_PHY_TRANSCEIVER_BROADCOM == PhyTransceiver)    )
    {
        //MEDIA_CONV# pins of BC5221 is strapped to 0, use special parameter set.
        PhyTransceiverLocal = EDDI_PHY_TRANSCEIVER_BROADCOM_MC;
    }
    #endif //(EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)

    pDelayParams = &pDDB->Glob.DSBStored.Glob.DelayParamsCopper[PhyTransceiverLocal];

    //MediaType and PhyTransceiver are already plausibled
    switch (MediaType)
    {
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            //already preset
            break;
        }
        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
        {
            if (EDD_FX_TRANSCEIVER_MAX > FXTransceiverType)
            {
                pDelayParams = &pDDB->Glob.DSBStored.Glob.DelayParamsFX[FXTransceiverType][PhyTransceiverLocal];
            }
            else
            {
                EDDI_Excp("EDDI_SetDDBDelayParams, FXTransceiverType for Fiber Optic HwPortIndex is invalid", EDDI_FATAL_ERR_EXCP, HwPortIndex, FXTransceiverType);
            }
            break;
        }
        case EDD_MEDIATYPE_RADIO_COMMUNICATION:
        {
            //already preset
            break;
        }
        default: break;
    }

    //check delay parameters
    if (   (0 == pDelayParams->PortTxDelay)
        || (0 == pDelayParams->PortRxDelay)
        || (0 == pDelayParams->MaxPortTxDelay)
        || (0 == pDelayParams->MaxPortRxDelay))
    {
        EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SetDDBDelayParams, no delayparams available for HwPortIndex %d, PhyTransceiver %d, MediaType %d, FXTransceiverType %d",
            HwPortIndex, PhyTransceiverLocal, MediaType, FXTransceiverType);
        EDDI_Excp("EDDI_SetDDBDelayParams, PortDelay's in DSB are wrong, PortDelay = 0 is not allowed for IRTE", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //set delay parameters in DDB
    pDDB->Glob.PortParams[HwPortIndex].DelayParams.PortTxDelay    = pDelayParams->PortTxDelay;
    pDDB->Glob.PortParams[HwPortIndex].DelayParams.PortRxDelay    = pDelayParams->PortRxDelay;
    pDDB->Glob.PortParams[HwPortIndex].DelayParams.MaxPortTxDelay = pDelayParams->MaxPortTxDelay;
    pDDB->Glob.PortParams[HwPortIndex].DelayParams.MaxPortRxDelay = pDelayParams->MaxPortRxDelay;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_DeviceSetupSER()                       +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE   pDDB              +*/
/*+                             EDDI_UPPER_DSB_PTR_TYPE   pDSB              +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to Device Description Block (Device-management)   +*/
/*+  pDSB       : Pointer to Device Setup Block                             +*/
/*+                                                                         +*/
/*+  Result     : LSA_VOID                                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup                                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceSetupSER( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    EDDI_UPPER_DSB_PTR_TYPE  const  pDSB )
{
    LSA_UINT32                                Mask;
    EDDI_SER_NRT_FREE_CCW_PTR_TYPE            pCCWDevBaseAddr;
    EDDI_SER_UCMC_PTR_TYPE                    pUCMCDevBaseAddr;
    LSA_UINT32                                length, ret;
    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    EDDI_SER_MC_TABLE_HEADER_ENTRY_PTR_TYPE   pMC_Dev_Table_Base;
    EDDI_SER_MC_TABLE_DESC_ENTRY_PTR_TYPE     pMC_Dev_Table_Description;
    #endif
    EDDI_MEM_BUF_EL_H                      *  pMemHeader;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DeviceSetupSER->");

    if (!(pDDB->Glob.LLHandle.pCTRLDevBaseAddr == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetupSER", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (!(pDDB->SRT.pACWRxHeadCtrl == EDDI_NULL_PTR))   
    {
        EDDI_Excp("EDDI_DeviceSetupSER", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (!(pDDB->SRT.pACWTxHeadCtrl == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetupSER", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (!(pDDB->SRT.pACWSerBase == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetupSER", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pUCMCDevBaseAddr   = EDDI_NULL_PTR;
    //pStatisticBaseAddr = EDDI_NULL_PTR;

    //ini NRT-CTRL-LIST
    length = sizeof(EDDI_SER_NRT_FREE_CCW_TYPE);

    pCCWDevBaseAddr = &pDDB->pKramFixMem->NRT_FREE_CCW;

    pDDB->KramRes.NrtCTRL = length;

    //already set to 0xFF by "EDDI_MemSet(pCCWDevBaseAddr, (LSA_UINT8)0xFF, length)"
    pDDB->Glob.LLHandle.pCCWDevBaseAddr = pCCWDevBaseAddr;

    /*-------------------------------------------------------------------------*/
    /* Setup clock management                                                  */
    /*-------------------------------------------------------------------------*/
    Mask = 0x0000000FUL;

    if (EDDI_PmIsValidHwPortIndex(pDDB, 0))
    {
        Mask &= ~0x1UL;
    }

    if (EDDI_PmIsValidHwPortIndex(pDDB, 1))
    {
        Mask &= ~0x2UL;
    }

    if (EDDI_PmIsValidHwPortIndex(pDDB, 2))
    {
        Mask &= ~0x4UL;
    }

    if (EDDI_PmIsValidHwPortIndex(pDDB, 3))
    {
        Mask &= ~0x8UL;
    }

    IO_W32(Clk_Control, Mask);

    /*-------------------------------------------------------------------------*/
    /* Setup SMI (MDC Clock Frequency)                                         */
    /*-------------------------------------------------------------------------*/
    #if defined (EDDI_CFG_ERTEC_200)
    //set SMI-frequency to 1,67 MHz
    IO_W32(SMI_CONFIGURATION, 0x00000002UL);
    #endif

    //Alloc memory for UCMC table, nr of entries in FDB table
    length = sizeof(EDDI_SER_UCMC_TYPE) * (pDDB->pConstValues->UCMCTableMaxEntry);
    ret    = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pMemHeader, length);

    if (ret != EDDI_MEM_RET_OK)
    {
        if (ret == EDDI_MEM_RET_NO_FREE_MEMORY)
        {
            EDDI_Excp("EDDI_DeviceSetupSER, Not enough free KRAM!", EDDI_FATAL_ERR_EXCP, ret, 0);
        }
        else
        {
            EDDI_Excp("EDDI_DeviceSetupSER, EDDI_MEMGetBuffer see eddi_mem.h!", EDDI_FATAL_ERR_EXCP, ret, 0);
        }
        return;
    }

    pUCMCDevBaseAddr = (EDDI_SER_UCMC_PTR_TYPE)(void *)pMemHeader->pKRam;

    pDDB->KramRes.UCMCTable = length;

    //moved to EDDI_SerSetStatFDBEntries -- EDDI_MemSet(pUCMCDevBaseAddr, (LSA_UINT8)0x0, length);

    //Alloc Memory for MC-Table
    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    pMC_Dev_Table_Base        = EDDI_NULL_PTR;
    pMC_Dev_Table_Description = EDDI_NULL_PTR;

    if (   (pDDB->pConstValues->MC_MaxEntriesPerSubTable > 0)
        && (pDDB->pConstValues->MC_MaxSubTable > 0))
    {
        LSA_UINT32  dwSize, dwSizeAlign8_Header, dwSizeOneTable, dwSizeAllTable, Index;
        EDDI_MEM_BUF_EL_H  *  pHeader;
            
        //Size of table header has to be a multiple of 8Byte
        dwSize = sizeof(EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE) * pDDB->pConstValues->MC_MaxSubTable;

        dwSizeAlign8_Header = EDDI_GetAlignSize(pDDB, 8UL, dwSize);

        //Size of a table without header has to be a multiple of 8Byte
        dwSizeOneTable = sizeof(EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE) * pDDB->pConstValues->MC_MaxEntriesPerSubTable;

        //Size of all tables has to be a multiple of 8Byte + 1 entry (to dynamically add static MC entries at runtime)
        dwSizeAllTable = dwSizeOneTable * (pDDB->pConstValues->MC_MaxSubTable + 1);

        if (dwSizeAllTable > SWI_MC_TABLE_MAX_SIZE)
        {
            EDDI_Excp("EDDI_DeviceSetupSER, pMC_Dev_Table_Base - MCTable too big!", EDDI_FATAL_ERR_EXCP, SWI_MC_TABLE_MAX_SIZE, dwSizeAllTable);
            return;
        }

        //Overall KRAM range for the complete MC table
        length = dwSizeAllTable + dwSizeAlign8_Header;
        ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pHeader, length);

        if (ret != EDDI_MEM_RET_OK)
        {
            if (ret == EDDI_MEM_RET_NO_FREE_MEMORY)
            {
                EDDI_Excp("EDDI_DeviceSetupSER, pMC_Dev_Table_Base - Not enough free KRAM!", EDDI_FATAL_ERR_EXCP, ret, dwSizeAllTable);
            }
            else
            {
                EDDI_Excp("EDDI_DeviceSetupSER, pMC_Dev_Table_Base - EDDI_MEMGetBuffer see eddi_mem.h!", EDDI_FATAL_ERR_EXCP, ret, dwSizeAllTable);
            }
            return;
        }

        pMC_Dev_Table_Base    = (EDDI_SER_MC_TABLE_HEADER_ENTRY_PTR_TYPE)(void *)pHeader->pKRam;
        pDDB->KramRes.MCTable = length;

        EDDI_MemSet(pMC_Dev_Table_Base, (LSA_UINT8)0x0, length );

        //table description  + 1 entry (to dynamically add static MC entries at runtime)
        length = sizeof(EDDI_SER_MC_TABLE_DESC_ENTRY_TYPE) * (pDDB->pConstValues->MC_MaxSubTable + 1);
        EDDI_AllocLocalMem ((void * *)&pMC_Dev_Table_Description, length);

        if (pMC_Dev_Table_Description == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_DeviceSetupSER, pMC_Dev_Table_Description, EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_MemSet(pMC_Dev_Table_Description, (LSA_UINT8)0, length);

        //concatenate offset to header
        dwSize = dwSizeAlign8_Header;

        for (Index = 0; Index < (pDDB->pConstValues->MC_MaxSubTable + 1); Index++)
        {
            if (Index < pDDB->pConstValues->MC_MaxSubTable)
            {
                //set header in MC table
                EDDI_SetBitField16(&pMC_Dev_Table_Base[Index].Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr, g_MC_Table_Ptr_FREE);
                EDDI_SetBitField16(&pMC_Dev_Table_Base[Index].Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__Res                  , 0);
                EDDI_SetBitField16(&pMC_Dev_Table_Base[Index].Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index        , 0);
            }

            //generate table description + 1 reserve
            pMC_Dev_Table_Description[Index].SubTable_Offset        = (LSA_UINT16)(dwSize >> 3); //Offset in units of 8Byte
            pMC_Dev_Table_Description[Index].SubTable_LFSR          = g_MC_Table_Index_FREE;
            pMC_Dev_Table_Description[Index].pSubTable_Base_Adress  = (EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE)((LSA_UINT32) pMC_Dev_Table_Base + dwSize);
            dwSize += dwSizeOneTable;
        }
    
        pDDB->Glob.LLHandle.bMCTableEnabled = LSA_TRUE;
    }
    else
    {
        pDDB->Glob.LLHandle.bMCTableEnabled = LSA_FALSE;
    }

    pDDB->Glob.LLHandle.pMC_SubTableDescr              = pMC_Dev_Table_Description;
    pDDB->Glob.LLHandle.pDev_MC_Table_Base             = pMC_Dev_Table_Base;
    #endif //(EDDI_CFG_ENABLE_MC_FDB)
    
    /*-------------------------------------------------------------------------*/
    /* Init EDDI-Internal-Structs                                              */
    /*-------------------------------------------------------------------------*/
    //GLOB
    
    //HW sees offsets only, and MAX-Entry-1 (and not nr of entries)
    pDDB->Glob.LLHandle.UCMC_Table_Length      = (LSA_UINT16)(pDDB->pConstValues->UCMCTableMaxEntry - 1);
    pDDB->Glob.LLHandle.pDev_UCMC_Table_Base   = pUCMCDevBaseAddr;
    pDDB->Glob.LLHandle.UCMC_Table_KRAM_Size   = sizeof(EDDI_SER_UCMC_TYPE) * pDDB->pConstValues->UCMCTableMaxEntry;
    pDDB->Glob.LLHandle.UCMC_LFSR_Mask         = pDDB->pConstValues->UCMCLFSRMask;
    pDDB->Glob.LLHandle.UCMC_Table_Range       = pDDB->pConstValues->UCMCTableRange;

    //SWI-Structs part 1
    EDDI_SerIniGlob(pDDB, pDSB);

    EDDI_NRTDeviceSetup(pDDB, pDSB);

    pDDB->Glob.LLHandle.NRT_FCW_Limit_Up       = 0xFFF; //Abschalten, kein INT fuer Limit_Up ausgeloest
    pDDB->Glob.LLHandle.NRT_FCW_Limit_Down     = pDSB->NRTPara.NRT_FCW_Limit_Down;

    pDDB->Glob.LLHandle.NRT_DB_Limit_Up        = 0xFFF; //Abschalten, kein INT fuer Limit_Up ausgeloest
    pDDB->Glob.LLHandle.NRT_DB_Limit_Down      = pDSB->NRTPara.NRT_DB_Limit_Down;

    pDDB->Glob.LLHandle.HOL_Limit_CH_Up        = pDSB->NRTPara.HOL_Limit_CH_Up;
    pDDB->Glob.LLHandle.HOL_Limit_CH_Down      = pDDB->pConstValues->HOLLimitCHDown;

    pDDB->Glob.LLHandle.HOL_Limit_Port_Up      = pDSB->NRTPara.HOL_Limit_Port_Up;
    pDDB->Glob.LLHandle.HOL_Limit_Port_Down    = pDDB->pConstValues->HOLLimitPortDown;

    //do not move -> has to run if in configuration mode
    EDDI_DeviceSetup_srt(pDDB, pDSB, pCCWDevBaseAddr);

    /*-------------------------------------------------------------------------*/
    /* switch macros SETUP and SWITCH to run mode                              */
    /*-------------------------------------------------------------------------*/
    EDDI_SERSetup(&pDDB->Glob.LLHandle, pDSB, pDDB);  //start counters/timers

    //SWI-Structs part 2 (SMI is already running here)
    EDDI_SerIniGlob2(pDDB);

    EDDI_DeviceSetup_acw(pDDB);

    /*--------------------------------------------------------------------------*/
    /* Setup ACW                                                                */
    /* Acws always needed (for SyncSlaves)                                      */
    /*--------------------------------------------------------------------------*/
    EDDI_SERAcwSetup(pDDB);

    /*-------------------------------------------------------------------------*/
    /* Start recording of IRT/SRT-TRACE                                        */
    /*-------------------------------------------------------------------------*/
    IO_x32(TRC_MODE) = TRC_START | TRC_BUF_RING_MODE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SerSetStatFDBEntries()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerSetStatFDBEntries( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          EDDI_UPPER_DSB_PTR_TYPE  const  pDSB )
{
    LSA_UINT32  Index;
    LSA_RESULT  Status;

    EDDI_MemSet(pDDB->Glob.LLHandle.pDev_UCMC_Table_Base, (LSA_UINT8)0, pDDB->Glob.LLHandle.UCMC_Table_KRAM_Size);

    /*-------------------------------------------------------------------------*/
    /* Setup MAC-Address                                                       */
    /*-------------------------------------------------------------------------*/
    //write predefined MAC addresses (by system adaption) into FDB table
    for (Index = 0; Index < pDSB->SWIPara.Sys_StatFDB_CntEntry; Index++)
    {
        //user view no port mapping
        EDDI_SERSetFDBEntryConfig(&pDDB->Glob.LLHandle, &pDSB->SWIPara.pSys_StatFDB_CntEntry[Index], pDDB);
    }

    //the local MACAdr of this station is only enabled on channel A
    Status = EDDI_SERSetOwnMACAdr(&pDDB->Glob.LLHandle.xRT.MACAddressSrc, SER_SWI_CONFIG_MODE, pDDB);
    if (EDD_STS_OK != Status)
    {
        EDDI_Excp("EDDI_SerSetStatFDBEntries, error in EDDI_SERSetOwnMACAdr, Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    //PTP MAC addresses are added to FDB as static entries (only necessary for PNIO devices)
    EDDI_SwiPNPNToFDB(pDDB, pDSB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_UndoDeviceSetupSER()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_UndoDeviceSetupSER( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16  ret16_val;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_UndoDeviceSetupSER->");

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.pMC_SubTableDescr)
    {
        EDDI_FREE_LOCAL_MEM(&ret16_val, pDDB->Glob.LLHandle.pMC_SubTableDescr);
        if (ret16_val != LSA_RET_OK)
        {
            EDDI_Excp("EDDI_UndoDeviceSetupSER, pMC_Dev_Table_Description", EDDI_FATAL_ERR_EXCP, ret16_val, 0);
            return;
        }
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)

    EDDI_FREE_LOCAL_MEM(&ret16_val, pDDB->SRT.pACWRxHeadCtrl);
    if (ret16_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_UndoDeviceSetupSER, SRT.pACWRxHeadCtrl", EDDI_FATAL_ERR_EXCP, ret16_val, 0);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret16_val, pDDB->SRT.pACWTxHeadCtrl);
    if (ret16_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_UndoDeviceSetupSER, SRT.pACWTxHeadCtrl", EDDI_FATAL_ERR_EXCP, ret16_val, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_DeviceSetup_srt()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceSetup_srt(       EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB,
                                                             EDDI_UPPER_DSB_PTR_TYPE         const  pDSB,
                                                                   EDDI_SER_NRT_FREE_CCW_PTR_TYPE  const  pCCWDevBaseAddr )
{
    LSA_UINT32                  Index;
    LSA_UINT32                  length, ret;
    EDDI_SER10_SRT_FCW_TYPE  *  pDev_SRTFCWs;
    EDDI_MEM_BUF_EL_H        *  pMemHeader;
    LSA_UINT32                  TmpAdrHostFormat;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DeviceSetup_srt->");

    //Set iSRT time values
    pDDB->SRT.iSRTRelTime = pDDB->pConstValues->iSRTRealTime;
    IO_W32(ISRT_TIME_VALUE, pDDB->pConstValues->iSRTRealTime);

    pDDB->SRT.SRT_FCW_Count = pDSB->SRTPara.SRT_FCW_Count;

    //Prepare SRT_FCW-LIST if needed

    if (pDSB->SRTPara.SRT_FCW_Count == 0)
    {
        //No SRT_FCWs needed
        IO_W32(SRT_FCW_COUNT, 0);
        return;
    }

    //Alloc Free SRT-FCW
    length = sizeof(EDDI_SER10_SRT_FCW_TYPE) * pDSB->SRTPara.SRT_FCW_Count;
    ret    = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pMemHeader, length);

    if (ret != EDDI_MEM_RET_OK)
    {
        if (ret == EDDI_MEM_RET_NO_FREE_MEMORY)
        {
            EDDI_Excp("EDDI_DeviceSetup_srt, Not enough free KRAM!",
                      EDDI_FATAL_ERR_EXCP, ret, pDSB->SRTPara.SRT_FCW_Count);
        }
        else
        {
            EDDI_Excp("EDDI_DeviceSetup_srt, EDDI_MEMGetBuffer see eddi_mem.h!",
                      EDDI_FATAL_ERR_EXCP, ret, pDSB->SRTPara.SRT_FCW_Count);
        }
        return;
    }

    pDev_SRTFCWs         = (EDDI_SER10_SRT_FCW_TYPE *)(void *)pMemHeader->pKRam;
    pDDB->KramRes.SrtFcw = length;

    TmpAdrHostFormat = 0xFFFFFFFFUL;

    for (Index = 0; Index < pDSB->SRTPara.SRT_FCW_Count; Index++)
    {
        //EDDI_SetBitField32(&pDev_SRTFCWs->Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_SRT_DATA);
        //pCCWDevBaseAddr->SRTFcw.p.Top == 0xFFFFFFFF for starting
        if (TmpAdrHostFormat >= SER10_NULL_PTR)
        {
            //insert first element -> becomes the last element in the chain
            EDDI_SetBitField32(&pDev_SRTFCWs->Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, SER10_NULL_PTR);

            pCCWDevBaseAddr->SRTFcw.p.Bottom = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pDev_SRTFCWs, pDDB));
        }
        else
        {
            EDDI_SetBitField32(&pDev_SRTFCWs->Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, TmpAdrHostFormat);
        }

        TmpAdrHostFormat = DEV_kram_adr_to_asic_register(pDev_SRTFCWs, pDDB);
        pDev_SRTFCWs++;

    }
    pCCWDevBaseAddr->SRTFcw.p.Top = EDDI_HOST2IRTE32(TmpAdrHostFormat);

    IO_W32(SRT_FCW_COUNT, pDSB->SRTPara.SRT_FCW_Count);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_DeviceSetup_acw()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceSetup_acw( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16                     shifter, Index, RxEntries;
    LSA_UINT16                     ACWTotalHeads; //total number of reserved-, AcwTx- and AcwRx-Headers
    EDDI_SER_ACW_HEAD_PTR_TYPE     pACWSerRx;
    EDDI_SER_ACW_HEAD_PTR_TYPE     pACWSerBase;
    LSA_UINT32                     ret, Size;
    EDDI_MEM_BUF_EL_H           *  pMemHeader;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DeviceSetup_acw->");

    pACWSerRx   = EDDI_NULL_PTR;
    pACWSerBase = EDDI_NULL_PTR;
    RxEntries   = 0;

    //Calculate next binary value after ConsumerCnt
    //We start with the shifter 5 to guarantee a ACWRxHeads-Value devidable by 2
    
    for (shifter = 5; shifter < 16 ; shifter++)
    {
        RxEntries = (LSA_UINT16)((LSA_UINT32)0x1 << shifter);
        if (RxEntries >= pDDB->CRT.MetaInfo.AcwConsumerCnt)
        {
            //Ok we found the correct Value for AcwRxHeads
            break;
        }
    }

    pDDB->SRT.ACWRxHeads = (LSA_UINT16)(RxEntries >> 0x03); // Maximum 8 Entries per Header

    pDDB->SRT.ACWRxMask  = (LSA_UINT16)((pDDB->SRT.ACWRxHeads) - 1);

    //Alloc Mem for ACW-Table
    ACWTotalHeads = (LSA_UINT16)(pDDB->SRT.ACWRxHeads + 2UL); // add a reserved Head and the AcwTx-List-Head

    //Alloc and Reset Control-Structures
    EDDI_AllocLocalMem((void * *)&pDDB->SRT.pACWRxHeadCtrl,
                       (LSA_UINT32)(pDDB->SRT.ACWRxHeads * sizeof(EDDI_CCW_CTRL_HEAD_TYPE)));

    EDDI_AllocLocalMem((void * *)&pDDB->SRT.pACWTxHeadCtrl,
                       (LSA_UINT32)(pDDB->SRT.ACWRxHeads * sizeof(EDDI_CCW_CTRL_HEAD_TYPE)));

    if ((pDDB->SRT.pACWRxHeadCtrl == EDDI_NULL_PTR) || (pDDB->SRT.pACWTxHeadCtrl == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetup_acw", EDDI_FATAL_ERR_EXCP, pDDB->SRT.pACWRxHeadCtrl, 0);
        return;
    }

    EDDI_MemSet(pDDB->SRT.pACWRxHeadCtrl, (LSA_UINT8)0, (LSA_UINT32)(pDDB->SRT.ACWRxHeads * sizeof(EDDI_CCW_CTRL_HEAD_TYPE)));
    EDDI_MemSet(pDDB->SRT.pACWTxHeadCtrl, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_CCW_CTRL_HEAD_TYPE));

    //Alloc and Reset KRAM-Structures
    Size = (LSA_UINT32)(sizeof(EDDI_SER_ACW_HEAD_TYPE) + (sizeof(EDDI_SER_ACW_HEAD_TYPE) * (ACWTotalHeads / 2)));

    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pMemHeader, Size);

    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_DeviceSetup_acw", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }

    pACWSerBase           = (EDDI_SER_ACW_HEAD_PTR_TYPE)(void *)pMemHeader->pKRam;
    pDDB->KramRes.AcwHead = Size;

    pACWSerRx = (EDDI_SER_ACW_HEAD_PTR_TYPE)((LSA_UINT32)pACWSerBase + sizeof(EDDI_SER_ACW_HEAD_TYPE));

    EDDI_MemSet(pACWSerBase, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER_ACW_HEAD_TYPE));
    EDDI_MemSet(pACWSerRx, (LSA_UINT8)0, (LSA_UINT32)(sizeof(EDDI_SER_ACW_HEAD_TYPE) * (pDDB->SRT.ACWRxHeads / 2)));

    //Init EDDI_SER_ACW_HEAD_TYPE for Rx  in Ctrl-Struct
    for (Index = 0; Index < (pDDB->SRT.ACWRxHeads / 2); Index++)
    {
        pACWSerRx[Index].Head[0] = SER10_NULL_PTR_SWAP;
        pACWSerRx[Index].Head[1] = SER10_NULL_PTR_SWAP;
    }

    //Init KRAM-Tx-Header
    pACWSerBase->Head[0] = SER10_NULL_PTR_SWAP; // The future send header, will point to the first entry in Sendlist
    pACWSerBase->Head[1] = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pACWSerRx, pDDB)); // Reserved and not used but must be set to SER10_NULL_PTR !!

    //Store EDDI_SER_ACW_HEAD_TYPE for Tx (only one head) in Ctrl-Struct
    pDDB->SRT.pACWTxHeadCtrl->pKramListHead = &pACWSerBase->Head[0];

    //Store  EDDI_ACW_HEAD_TYPE
    for (Index = 0; Index < pDDB->SRT.ACWRxHeads; Index++)
    {
        pDDB->SRT.pACWRxHeadCtrl[Index].pBottom       = EDDI_NULL_PTR;
        pDDB->SRT.pACWRxHeadCtrl[Index].pTop          = EDDI_NULL_PTR;
        pDDB->SRT.pACWRxHeadCtrl[Index].pKramListHead = &pACWSerRx[Index >> 1].Head[Index & 1];
    }

    //ACW-Structs
    pDDB->SRT.pACWSerBase = pACWSerBase;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SerGetAcwKramSize()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SerGetAcwKramSize( LSA_UINT32  const AcwConsumerCnt )
{
    LSA_UINT32  shifter, RxEntries, ACWRxHeads;
    LSA_UINT32  Size, ACWTotalHeads;

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SerGetAcwKramSize->");

    RxEntries = 0;
    // Calculate next binary value after ConsumerCnt
    // We start with the shifter 5 to garanty a ACWRxHeads-Value devidable by 2
    for (shifter = 5; shifter < 16 ; shifter++)
    {
        RxEntries = (LSA_UINT16)((LSA_UINT32)0x1 << shifter);

        if (RxEntries >= AcwConsumerCnt)
        {
            // Ok we found the correct Value for AcwRxHeads
            break;
        }
    }

    ACWRxHeads = (LSA_UINT16)(RxEntries >> 0x03);  // Maximum 8 Entries per Header

    //Alloc Mem for ACW-Table
    ACWTotalHeads = (LSA_UINT16)(ACWRxHeads + 2UL);  // add a reserved Head and the AcwTx-List-Head

    //Alloc and Reset KRAM-Structures
    Size = (sizeof(EDDI_SER_ACW_HEAD_TYPE) + (sizeof(EDDI_SER_ACW_HEAD_TYPE) * (ACWTotalHeads / 2)));

    return Size;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERSetOwnMACAdr()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetOwnMACAdr( EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pMACAdress,
                                                       EDDI_SER_SWI_MODE_TYPE       const  SwiMode,
                                                       EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_RESULT                       Status = EDD_STS_OK;
    LSA_UINT32                       UsrPortIndex;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE  UsrLocalMAC;

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetOwnMACAdr->SwiMode:0x%X", SwiMode);

    UsrLocalMAC.MACAddress = *pMACAdress;

    //no port-mapping -> user view
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        UsrLocalMAC.PortID[UsrPortIndex] = EDD_FEATURE_DISABLE;
    }

    UsrLocalMAC.CHA        = EDD_FEATURE_ENABLE;
    UsrLocalMAC.CHB        = EDD_FEATURE_DISABLE;

    UsrLocalMAC.Prio       = pDDB->pConstValues->XRTPrio; 
    UsrLocalMAC.Pause      = EDD_FEATURE_DISABLE;
    UsrLocalMAC.Filter     = EDD_FEATURE_DISABLE;
    UsrLocalMAC.Borderline = EDD_FEATURE_DISABLE;

    //user view no port mapping
    if (SwiMode == SER_SWI_CONFIG_MODE)
    {
        EDDI_SERSetFDBEntryConfig(&pDDB->Glob.LLHandle, &UsrLocalMAC, pDDB);
    }
    else
    {
        Status = EDDI_SERSetFDBEntryRun(&pDDB->Glob.LLHandle, &UsrLocalMAC, LSA_TRUE, pDDB);
    }

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetOwnMACAdr<-SwiMode:0x%X Status:0x%X", SwiMode, Status);

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ser.c                                                   */
/*****************************************************************************/

