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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-system input-functions                      */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  26.09.03    JS    bugfix in edds_version. changed implementation.        */
/*                    (was wrong if we have fill-bytes within version-struct)*/
/*  20.01.04    JS    edds_SRTProviderGetCycleCounter added                  */
/*  28.01.05    JS    LTRC support                                           */
/*                    use LSA_UINT16 instead of LSA_RESULT with edds_version()*/
/*  06.07.05    JS    added multiport support                                */
/*  04.10.05    JS    added LED-blink support                                */
/*  09.03.06    JS    added endianes check                                   */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  1
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SYS */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_init                                   +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the EDDS.                                     +*/
/*+               The number of handles the edds can manage is defined by   +*/
/*+               EDDS_CFG_HANDLE_CNT. The memory for handle-management is  +*/
/*+               static or will be allocated depending on                  +*/
/*+               EDDS_LOCAL_MEM_STATIC.                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_init(LSA_VOID)
{
    LSA_RESULT              Status = EDD_STS_OK;
    LSA_UINT16              i;

    EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_init()");

    EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"--> edds_init()");

    /* check for correct endianess define */

    i = 0x1234;
    #ifndef EDDS_CFG_BIG_ENDIAN
    if ( *(LSA_UINT8*)&i != 0x34 )
    {
        #else
    if ( *(LSA_UINT8*)&i != 0x12 )
    {
        #endif
        EDDS_FatalError(EDDS_FATAL_ERR_ENDIANES,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_NOTE,"<-- edds_init(). Response: 0x%X",Status);

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_init(), Status: 0x%X",
                           Status);

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_undo_init                              +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes EDDS.                                       +*/
/*+               This is only possible if no device is open                +*/
/*+               EDD_STS_ERR_SEQUENCE if device is open.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_undo_init(LSA_VOID)
{
    LSA_RESULT Status = EDD_STS_OK;

    EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_undo_init()");

    EDDS_SYSTEM_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_undo_init(), Status: 0x%X",
                           Status);

    return Status;

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_SRTProviderGetCycleCounter             +*/
/*+  Input                      EDDS_HANDLE    hDDB                         +*/
/*+                             EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to Device Description Block (Device-management) +*/
/*+  pCycleCounter: Pointer to address for CycleCounter                     +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_SEQUENCE                        +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:  Gets actual provider CycleCounter of cyclic SRT.         +*/
/*+                                                                         +*/
/*+                The CycleCounter is based on 31.25us and wraps around    +*/
/*+                if 32Bit is reached. The function can be alled anytime   +*/
/*+                except for startup/shutdown of device/srt-component      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_SYSTEM_IN_FCT_ATTR edds_SRTProviderGetCycleCounter(
    EDDS_HANDLE                 hDDB,
    EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter)
{

    LSA_RESULT              Status;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;

    pDDB = (EDDS_LOCAL_DDB_PTR_TYPE)hDDB;

    EDDS_SYSTEM_TRACE_02(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_SRTProviderGetCycleCounter(hDDB: 0x%X,pCycleCounter: 0x%X)",
                           hDDB,
                           pCycleCounter);

    if (LSA_HOST_PTR_ARE_EQUAL(pCycleCounter, LSA_NULL))
    {
        EDDS_SYSTEM_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_FATAL,
                           "edds_SRTProviderGetCycleCounter, pCycleCounter is NULL_PTR");
        return EDD_STS_ERR_EXCP;
    }

    Status = EDD_STS_OK;


    if ( (!LSA_HOST_PTR_ARE_EQUAL(pDDB,LSA_NULL)) && (!LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT,LSA_NULL)) )
    {
        Status = EDDS_RTProviderGetCycleCounter(pDDB->pSRT,pCycleCounter);
    }
    else
    {
        Status = EDD_STS_ERR_SEQUENCE;
    }

    EDDS_SYSTEM_TRACE_03(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_SRTProviderGetCycleCounter(High: 0x%X, Low: 0x%X), Status: 0x%X",
                           pCycleCounter->High,
                           pCycleCounter->Low,
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_InitDPBWithDefaults                    +*/
/*+  Input/Output          :    pDPB (Output)                               +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Write default settings into pDPB.                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
EDD_RSP EDDS_LOCAL_FCT_ATTR edds_InitDPBWithDefaults(
    EDDS_UPPER_DPB_PTR_TYPE     pDPB)
{
    EDD_RSP   Response = EDD_STS_OK;
    EDDS_UPPER_DPB_NRT_PTR_TYPE pNRTIni;
    EDDS_UPPER_DPB_CSRT_PTR_TYPE pSRTIni;
    LSA_UINT16 i;

    pNRTIni = &pDPB->NRT;
    pSRTIni = &pDPB->CSRT;

    // Device Open
    pDPB->ZeroCopyInterface = EDDS_DPB_USE_ZERO_COPY_IF; /* use zero copy interface */
    pDPB->RxBufferCnt = 2 * 149;    // 100MBit, 1ms scheduler trigger
    //pDPB->RxBufferCnt = 2*1489;    // 1GBit, 1ms scheduler trigger (wirespeed with minimal packets)
    pDPB->TxBufferCnt = 2 * 149;    // 100MBit, 1ms scheduler trigger
    //pDPB->TxBufferCnt = 2*1489;    // 1GBit, 1ms scheduler trigger (wirespeed with minimal packets)
    pDPB->schedulerRecurringTaskMS = EDDS_RECURRING_REDUCTION_MS;   // try to execute recurring task (LLIF) every 25ms
    pDPB->schedulerCycleIO = 1;   // default for IO: 1 ms for Adonis and 32 ms for Windows
    pDPB->schedulerCycleNRT = 1;  // default for NRT: 1 ms for Adonis and 32 ms for Windows
    pDPB->RxLimitationPackets_IO = 0;  // unlimited processed packets per millisecond in IO mode (only limited by bandwidth)
    pDPB->RxLimitationPackets_NRT = 0; // unlimited processed packets per millisecond in NRT mode (only limited by bandwidth)

    pDPB->MaxInterfaceCntOfAllEDD = EDD_CFG_MAX_INTERFACE_CNT;
    pDPB->MaxPortCntOfAllEDD = EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE;

    pDPB->SWI.MRPDefaultRoleInstance0 = EDD_MRP_ROLE_NONE;
    pDPB->SWI.MRPSupportedMultipleRole = EDD_MRP_ROLE_NONE;
    pDPB->SWI.MRPSupportedRole = EDD_MRP_ROLE_NONE;
    pDPB->SWI.MRAEnableLegacyMode = EDD_MRA_DISABLE_LEGACY_MODE;
    pDPB->SWI.MaxMRP_Instances = 0;
    for (i = 0; i < (sizeof(pDPB->SWI.MRPRingPort) / sizeof(pDPB->SWI.MRPRingPort[0]) ) ;
                ++i)
    {
        pDPB->SWI.MRPRingPort[i] = EDD_MRP_NO_RING_PORT;
    }
    pDPB->SWI.SupportedMRPInterconnRole   = EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_NONE;
    pDPB->SWI.MaxMRPInterconn_Instances = 0;
    for (i = 0; i < (sizeof(pDPB->SWI.SupportsMRPInterconnPortConfig) / sizeof(pDPB->SWI.SupportsMRPInterconnPortConfig[0]) ) ;
                ++i)
    {
        pDPB->SWI.SupportsMRPInterconnPortConfig[i] = EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_NO;
    }
    pDPB->FeatureSupport.AdditionalForwardingRulesSupported = EDD_FEATURE_DISABLE; //no HSYNC support per default
    pDPB->FeatureSupport.ApplicationExist                   = EDD_FEATURE_DISABLE; //EDDS is only FORDWARDER for HSYNC
    pDPB->FeatureSupport.ClusterIPSupport                   = EDD_FEATURE_DISABLE;
    pDPB->FeatureSupport.MRPInterconnFwdRulesSupported      = EDD_FEATURE_DISABLE;
    pDPB->FeatureSupport.MRPInterconnOriginatorSupported    = EDD_FEATURE_DISABLE;
    // NRT Init
    pNRTIni->RxFilterUDP_Unicast = EDD_SYS_UDP_WHITELIST_FILTER_ON;
    pNRTIni->RxFilterUDP_Broadcast = EDD_SYS_UDP_WHITELIST_FILTER_ON;
    pNRTIni->FeedInLoadLimitationActive = EDD_SYS_FILL_INACTIVE; /* FILL inactive */
    pNRTIni->IO_Configured = EDD_SYS_IO_CONFIGURED_OFF; /* default is LSA_FALSE */
    pNRTIni->FeedInLoadLimitationNRTPacketsMS = 0;  /* no limit */

    // The following NRT values must be larger than the corresponding CRT_and_NRT values.
    // This is checked in EDDS startup.

    // NRT Only -> we need more rx-buffers
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntARPIP =
                156; /* number of rx-buffers ARP */
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntOther =
                78; /* number of rx-buffers */
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntASRTAlarm =
                78; /* number of rx-buffers for alarms*/
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntASRTOther =
                78; /* number of rx-buffers for other asynchronous RT telegrams*/

    // CRT and NRT - will be used directly after startup
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntARPIP =
                60; /* number of rx-buffers ARP */
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntOther =
                30; /* number of rx-buffers */
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntASRTAlarm =
                30; /* number of rx-buffers for alarms*/
    pNRTIni->parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntASRTOther =
                30; /* number of rx-buffers for other asynchronous RT telegrams*/

    // SRT Init
    pSRTIni->CycleBaseFactor = 32; /* 32: = 1ms, 64: = 2ms, 128: = 4ms; 512: = 16ms */
    pSRTIni->ConsumerFrameIDBaseClass1 = 0xC000;
    pSRTIni->ConsumerCntClass1 = EDDS_DEFAULT_CONSUMER_COUNT; /*0xC000..0xC03F*/
    pSRTIni->ConsumerFrameIDBaseClass2 = 0x8000;
    pSRTIni->ConsumerCntClass2 = EDDS_DEFAULT_CONSUMER_COUNT; /*0x8000..0x803F*/
    pSRTIni->ProviderCnt = pSRTIni->ConsumerCntClass1
                + pSRTIni->ConsumerCntClass2;
    pSRTIni->ProviderCycleDropCnt = 0; /* no additional cycles to drop after Provider Overload */
    pSRTIni->hIOBufferManagement = LSA_NULL;

    return Response;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_InitDSBWithDefaults                    +*/
/*+  Input/Output          :    pDSB (Output)                               +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Write default settings into pDSB.                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
EDD_RSP EDDS_LOCAL_FCT_ATTR edds_InitDSBWithDefaults(
    EDDS_UPPER_DSB_PTR_TYPE     pDSB)
{
    EDD_RSP   Response = EDD_STS_OK;
    LSA_UINT16 i;

    // Setup Device
    for (i=0; i<=EDD_CFG_MAX_PORT_CNT;i++)
    {
        EDDS_MEMSET_LOCAL(&(pDSB->MACAddress[i]), 0, sizeof(EDD_MAC_ADR_TYPE));
    }

    return Response;
}

#ifdef EDDS_CFG_SYSRED_API_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  function name:  edds_ProviderGetPrimaryAR                                              +*/
/*+                                                                                         +*/
/*+  function     :  Get the primary AR for a specific ARSet ( determined by ARSetID)       +*/
/*+                                                                                         +*/
/*+  parameters   :  hDDB           Device handle (see EDDS_SRV_DEV_OPEN service)           +*/
/*+                  ARSetID        ID of the ARSet from which the current AR in state      +*/
/*+                                 PRIMARY (= PrimaryARID) shall be set.                   +*/
/*+                                 Range:                                                  +*/
/*+                                      1... EDD_CFG_MAX_NR_ARSETS                         +*/
/*+                                                                                         +*/
/*+                  pPrimaryARID   Pointer to where the return value shall be written to   +*/
/*+                                                                                         +*/
/*+                                                                                         +*/
/*+  result       :  EDD_STS_OK                                                             +*/
/*+                  EDD_STS_ERR_PARAM                                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT edds_ProviderGetPrimaryAR (
    EDDS_HANDLE                         hDDB,
    LSA_UINT16                 const    ARSetID,
    LSA_UINT16                *const    pPrimaryARID)
{
    LSA_RESULT                          result;
    EDDS_DDB_TYPE                       *pDDB;
    EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR    pMngInfoBlock;
    result                  = EDD_STS_OK;
    pDDB                    = (EDDS_DDB_TYPE*)hDDB;
    pMngInfoBlock           = LSA_NULL;
    if(  (LSA_HOST_PTR_ARE_EQUAL(pDDB,LSA_NULL)) || (ARSetID < 1) || (ARSetID > EDD_CFG_MAX_NR_ARSETS)  )
    {
        result = EDD_STS_ERR_PARAM;
    }
    else
    {
        pMngInfoBlock  = (EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR) (pDDB->pSRT->pIOBufferManagement);
        *pPrimaryARID  = LE_TO_H_S (pMngInfoBlock->PrimaryARIDSetTable.PrimaryIDs[ARSetID-1]);
    }
    return result;
}
#endif /*EDDS_CFG_SYSRED_API_SUPPORT*/
/*****************************************************************************/
/*  end of file EDDS_SYS.C                                                   */
/*****************************************************************************/

