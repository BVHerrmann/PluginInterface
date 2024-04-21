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
/*  F i l e               &F: eddi_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD user-interface functions with RQBs           */
/*                                                                           */
/*                          - eddi_system()                                  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  10.03.08    JS    Default for Multicast_Bridge_IEEE802RSTP_Forward       */
/*                    changed from FALSE to TRUE                             */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ext.h"
#include "eddi_swi_ext.h"
#include "eddi_csrt_ext.h"
#include "eddi_trac_ext.h"

#include "eddi_ser_ext.h"
#include "eddi_ser.h"
#include "eddi_time.h"

#include "eddi_kram.h"

#include "eddi_rto_udp.h"

#include "eddi_profile.h"
#include "eddi_crt_ini.h"
#include "eddi_nrt_ini.h"
#include "eddi_nrt_arp.h"
#include "eddi_nrt_usr.h"

#if defined (EDDI_CFG_DEFRAG_ON)
#include "eddi_nrt_frag_rx.h"
#include "eddi_nrt_q.h"
#endif

#if defined (EDDI_CFG_REV5)
#include "eddi_nrt_ts.h"
#endif

#include "eddi_sync_ini.h"
#include "eddi_crt_check.h"
#include "eddi_boot.h"

//#include "eddi_Tra.h"
#include "eddi_prm_state.h"

#include "eddi_lock.h"

#include "eddi_crt_dfp.h"

#define EDDI_MODULE_ID     M_ID_EDDI_USR
#define LTRC_ACT_MODUL_ID  10

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_DeviceOpen( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_DeviceSetup( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_DeviceClose( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_CheckDSB( EDDI_UPPER_DSB_PTR_TYPE  const  pDSB,
                                                    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_CheckBaseAddr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_ValidateKram( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                        EDDI_UPPER_DPB_PTR_TYPE     const  pDPB,
                                                        LSA_UINT32                  const  MaxKramSize,
                                                        LSA_UINT32               *  const  pUsedKramSize );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_KramRes( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_StartDeviceSetUpTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_IniProfKRamInfo( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           EDDI_UPPER_DPB_PTR_TYPE  const  pDPB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_CheckDSBMRPUsed( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           EDDI_UPPER_DSB_PTR_TYPE  const  pDSB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_DeviceCompIni( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckDPBFeatures( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                EDDI_UPPER_DPB_PTR_TYPE  const  pDPB );


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_DeviceCompIni()                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE       Service: EDDI_SRV_DEV_COMP_INI                      +*/
/*+   LSA_RESULT        Status:     Return status                           +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_DDB_INI_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     pDDB     : Pointer to device-specific initialization parameters     +*/
/*+                (input - variables !)                                    +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE    Service:                                               +*/
/*+   LSA_RESULT    Status:  EDD_STS_OK                                     +*/
/*+                          EDD_STS_ERR_PARAM                              +*/
/*+                          EDD_STS_ERR_SEQUENCE                           +*/
/*+                          EDD_STS_ERR_RESOURCE                           +*/
/*+                          EDD_STS_ERR_TIMEOUT                            +*/
/*+                          EDD_STS_ERR_NO_TIMER                           +*/
/*+                          EDD_STS_ERR_HW                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Init the component with const value                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceCompIni( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_RQB_COMP_INI_TYPE   *   const  pIni = (EDDI_RQB_COMP_INI_TYPE *)pRQB->pParam;
    EDDI_LOCAL_DDB_PTR_TYPE             pDDB;
    LSA_RESULT                          Status;
    
    Status = EDDI_GetDDB(pIni->hDDB, &pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_DeviceCompIni, EDDI_GetDDB() failed", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    //New KRAM-structure
    //EDDI_MemCopy(&pDDB->pProfKRamInfo->info.NRT_ini, &pIni->nrt, (LSA_UINT32)sizeof(EDDI_RQB_CMP_NRT_INI_TYPE));
    //EDDI_MemCopy(&pDDB->pProfKRamInfo->Swi_AgePollTime, &pIni->swi, (LSA_UINT32)sizeof(LSA_UINT16));
    
    //ini CSRT-Parameter for SRT-COMP-INI
    pDDB->CRT.CycleBaseFactor            = pDDB->pConstValues->CycleBaseFactor;
    pDDB->pProfKRamInfo->Swi_AgePollTime = pDDB->pConstValues->AgePollTime;

    EDDI_NRTInitComponent(&pIni->NRT, pDDB);
    EDDI_SwiUsrInitComponent(pDDB);
    EDDI_SwiIniPhyBugs(pDDB, &pIni->PhyBugfix);
               
    #if defined (EDDI_CFG_SOC)
    if (   (pIni->CycleComp.PDControlPllMode != EDDI_CYCLE_INI_PLL_MODE_NOTUSED)
        && (pDDB->ERTECx00_SCRB_BaseAddr     != EDDI_NULL_PTR))
    #else
    if (   (pIni->CycleComp.PDControlPllMode != EDDI_CYCLE_INI_PLL_MODE_NOTUSED)
        && (pDDB->ERTECx00_SCRB_BaseAddr     == EDDI_NULL_PTR))
    #endif
    {
        EDDI_Excp("EDDI_DeviceCompIni, PDControlPllMode: ERTECx00_SCRB_BaseAddr:", EDDI_FATAL_ERR_EXCP, pIni->CycleComp.PDControlPllMode, pDDB->ERTECx00_SCRB_BaseAddr);
        return;
    }

    EDDI_CycInit(&pIni->CycleComp, &pIni->CRT, pDDB);
    EDDI_CRTInitComponent(&pIni->CRT, pDDB);
    EDDI_SRTInitComponent(pDDB);

    EDDI_SYNCInitComponent(pDDB);
    EDDI_TraceInitComponent(pDDB);
    EDDI_RtoInitComponent(pDDB);

    EDD_RQB_SET_RESPONSE(pRQB, Status);

    if (!(0 == pIni->Cbf))
    {
        pIni->Cbf(pRQB); //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

                                                                  //Check IRTPU features

/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckDPBFeatures( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                EDDI_UPPER_DPB_PTR_TYPE  const  pDPB )
{
    #if defined (EDDI_CFG_REV6)
    if (EDD_FEATURE_ENABLE == pDPB->FeatureSupport.bMRPDSupported)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDPBFeatures, no MRPD support for ERTEC200, bMRPDSupported:0x%X ", pDPB->FeatureSupport.bMRPDSupported);
        EDDI_Excp("EDDI_CheckDPBFeatures, no MRPD support for ERTEC200", EDDI_FATAL_ERR_EXCP, pDPB->FeatureSupport.bMRPDSupported, 0);
        return (EDD_STS_ERR_EXCP);
    }
    #endif

    if (   (EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_NO     != pDPB->FeatureSupport.FragmentationtypeSupported)
        #if defined (EDDI_CFG_FRAG_ON)
        && (EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_STATIC != pDPB->FeatureSupport.FragmentationtypeSupported) 
        #endif
        )
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDPBFeatures, illegal value for FragmentationtypeSupported:%d ", pDPB->FeatureSupport.FragmentationtypeSupported);
        EDDI_Excp("EDDI_CheckDPBFeatures, illegal value for FragmentationtypeSupported", EDDI_FATAL_ERR_EXCP, pDPB->FeatureSupport.FragmentationtypeSupported, 0);
        return (EDD_STS_ERR_EXCP);
    }

    if (   (   (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.bMRPDSupported)
            && (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.bMRPDSupported) )
        || (   (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.AdditionalForwardingRulesSupported)
            && (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.AdditionalForwardingRulesSupported) )
        || (   (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.ApplicationExist)
            && (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.ApplicationExist) )
        || (   (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.MRPInterconnFwdRulesSupported)     /* More checks in EDDI_CheckDSB */
            && (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.MRPInterconnFwdRulesSupported) )
        ||     (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.MRPInterconnOriginatorSupported)
        )
    {
        EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckIRTPUFeatures, illegal value(s) for bMRPDSupported(%d), AdditionalForwardingRulesSupported(%d), ApplicationExist(%d), MRPInterconnFwdRulesSupported(%d), MRPInterconnOriginatorSupported(%d)", 
            pDPB->FeatureSupport.bMRPDSupported, pDPB->FeatureSupport.AdditionalForwardingRulesSupported, pDPB->FeatureSupport.ApplicationExist, pDPB->FeatureSupport.MRPInterconnFwdRulesSupported, pDPB->FeatureSupport.MRPInterconnOriginatorSupported);
        EDDI_Excp("EDDI_CheckIRTPUFeatures, illegal value for bMRPDSupported, AdditionalForwardingRulesSupported, ApplicationExist, MRPInterconnFwdRulesSupported, MRPInterconnOriginatorSupported", EDDI_FATAL_ERR_EXCP, pDPB->FeatureSupport.bMRPDSupported, 0);
        return (EDD_STS_ERR_EXCP);
    }

    if (   (EDD_FEATURE_DISABLE != pDPB->FeatureSupport.ClusterIPSupport)
        #if defined (EDDI_CFG_REV7)
        && (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.ClusterIPSupport)
        #endif
        )
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckIRTPUFeatures, illegal value for ClusterIPSupport(%d) ", 
            pDPB->FeatureSupport.ClusterIPSupport);
        EDDI_Excp("EDDI_CheckIRTPUFeatures, illegal value for ClusterIPSupport", EDDI_FATAL_ERR_EXCP, pDPB->FeatureSupport.ClusterIPSupport, 0);
        return (EDD_STS_ERR_EXCP);
    }

    if (!(EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE & pDPB->FeatureSupport.IRTForwardingModeSupported))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDPBFeatures, only EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE allowed, IRTForwardingModeSupported:%d ", pDPB->FeatureSupport.IRTForwardingModeSupported);
        EDDI_Excp("EDDI_CheckDPBFeatures, only EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE allowed", EDDI_FATAL_ERR_EXCP, pDPB->FeatureSupport.IRTForwardingModeSupported, 0);
        return (EDD_STS_ERR_EXCP);
    }

    if (   (0                                    != pDPB->FeatureSupport.MaxDFPFrames)
        #if defined (EDDI_CFG_DFP_ON)
        && (EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL != pDPB->FeatureSupport.MaxDFPFrames) 
        #endif
         )
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDPBFeatures, illegal value for MaxDFPFrames:%d ", pDPB->FeatureSupport.MaxDFPFrames);
        EDDI_Excp("EDDI_CheckDPBFeatures, illegal value for MaxDFPFrames", EDDI_FATAL_ERR_EXCP, pDPB->FeatureSupport.MaxDFPFrames, 0);
        return (EDD_STS_ERR_EXCP);
    }

    if ((EDD_FEATURE_ENABLE != pDPB->FeatureSupport.AdditionalForwardingRulesSupported)  &&  (EDD_FEATURE_ENABLE == pDPB->FeatureSupport.ApplicationExist))
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDPBFeatures, illegal combination for ApplicationExist (%d) and ForwardingRulesSupported(%d)",
                              pDPB->FeatureSupport.ApplicationExist, pDPB->FeatureSupport.AdditionalForwardingRulesSupported);
        EDDI_Excp("EDDI_CheckDPBFeatures,  illegal combination for ApplicationExist and ForwardingRulesSupported", EDDI_FATAL_ERR_EXCP,
                              pDPB->FeatureSupport.ApplicationExist, pDPB->FeatureSupport.AdditionalForwardingRulesSupported);
        return (EDD_STS_ERR_EXCP);
    }

    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)      // HSYNC not supported for ERTEC_200 and ERTEC_400
        if ((EDD_FEATURE_ENABLE == pDPB->FeatureSupport.AdditionalForwardingRulesSupported)  ||  (EDD_FEATURE_ENABLE == pDPB->FeatureSupport.ApplicationExist))
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDPBFeatures, ERTEC200/400 - HSYNC not supported! ApplicationExist (%d), ForwardingRulesSupported(%d)",
                                  pDPB->FeatureSupport.ApplicationExist, pDPB->FeatureSupport.AdditionalForwardingRulesSupported);
            EDDI_Excp("EDDI_CheckDPBFeatures,  HSYNC not supported!", EDDI_FATAL_ERR_EXCP,
                                  pDPB->FeatureSupport.ApplicationExist, pDPB->FeatureSupport.AdditionalForwardingRulesSupported);
            return (EDD_STS_ERR_EXCP);
        }
    #endif

    LSA_UNUSED_ARG(pDDB);
    return (EDD_STS_OK);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_DeviceOpen()                           +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE       Service: EDDI_SRV_DEV_OPEN                          +*/
/*+   LSA_RESULT        Status:     Return status                           +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_DDB_INI_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     pDDB     : Pointer to device-specific initialization parameters     +*/
/*+                (input - variables !)                                    +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE    Service:                                               +*/
/*+   LSA_RESULT    Status:  EDD_STS_OK                                     +*/
/*+                          EDD_STS_ERR_PARAM                              +*/
/*+                          EDD_STS_ERR_SEQUENCE                           +*/
/*+                          EDD_STS_ERR_RESOURCE                           +*/
/*+                          EDD_STS_ERR_TIMEOUT                            +*/
/*+                          EDD_STS_ERR_NO_TIMER                           +*/
/*+                          EDD_STS_ERR_HW                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a device. This request is the first request*/
/*+               needed for initialization of the device. It returns an    +*/
/*+               device-handle which have to be used in further calls.     +*/
/*+                                                                         +*/
/*+               The request is finished by calling the specified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceOpen( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT                          Status;
    EDDI_UPPER_DDB_INI_PTR_TYPE  const  pRqbDDBRel = (EDDI_UPPER_DDB_INI_PTR_TYPE)pRQB->pParam;
    EDDI_UPPER_DPB_PTR_TYPE      const  pDPB       = pRqbDDBRel->pDPB; //pointer to device parameter block
    EDDI_LOCAL_DDB_PTR_TYPE             pDDB;
    LSA_UINT32                          UsedKramSize = 0;
    LSA_UINT32                          MaxKramSize, v;

    //Check for valid parameters
    Status = EDDI_CreateDDB(&pDDB, pDPB->IRTE_SWI_BaseAdr);
    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, Status);
        if (Status != EDD_STS_ERR_SEQUENCE)
        {
            EDDI_Excp("EDDI_DeviceOpen", EDDI_FATAL_ERR_EXCP, Status, 0);
            return;
        }

        if (!(0 == pRqbDDBRel->Cbf))
        {
            pRqbDDBRel->Cbf(pRQB);   //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
        }

        return;
    }
       
    //Get upper device handle from DPB
    pDDB->hSysDev = pRqbDDBRel->hSysDev;

    pDDB->Glob.OpenCount              = 0;
    pDDB->Glob.pCloseDevicePendingRQB = (EDD_UPPER_RQB_PTR_TYPE)0;

    if (   (   (EDD_FEATURE_DISABLE != pDPB->EnableReset)
            && (EDD_FEATURE_ENABLE != pDPB->EnableReset) )
        || (   (EDD_FEATURE_DISABLE != pDPB->bSupportIRTflex)
            && (EDD_FEATURE_ENABLE != pDPB->bSupportIRTflex) )
        || (   (EDDI_DPB_ENABLE_MAX_PORTDELAY_CHECK != pDPB->bDisableMaxPortDelayCheck)
            && (EDDI_DPB_DISABLE_MAX_PORTDELAY_CHECK != pDPB->bDisableMaxPortDelayCheck) )
        )
    {
        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DeviceOpen, illegal value EnableReset (%d), bSupportIRTflex (%d), bDisableMaxPortDelayCheck (%d)", 
            pDPB->EnableReset, pDPB->bSupportIRTflex, pDPB->bDisableMaxPortDelayCheck);
        EDDI_Excp("EDDI_DeviceOpen, illegal value EnableReset, bSupportIRTflex, bDisableMaxPortDelayCheck", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    
    if (EDD_FEATURE_ENABLE == pDPB->EnableReset)
    {
        pDDB->Glob.SetMode = EDDI_HW_SETUP_ALL;
    }
    else
    {
        pDDB->Glob.SetMode = EDDI_HW_SETUP_PARTIAL;
    }

    pDDB->IRTE_SWI_BaseAdr       = pDPB->IRTE_SWI_BaseAdr;
    pDDB->IRTE_SWI_BaseAdr_32Bit = pDPB->IRTE_SWI_BaseAdr;
    #if !defined (EDDI_CFG_3BIF_2PROC)
    pDDB->pGSharedRAM            = (EDDI_GSHAREDMEM_TYPE *)pDPB->GSharedRAM_BaseAdr;
    #endif
    pDDB->NRTMEM_LowerLimit      = pDPB->NRTMEM_LowerLimit;
    pDDB->NRTMEM_UpperLimit      = pDPB->NRTMEM_UpperLimit;

    v = IO_x32(SWI_VERSION);
    pDDB->ERTEC_Version.Debug         = EDDI_GetBitField32(v, EDDI_SER_VERSION_BIT__Debug);
    pDDB->ERTEC_Version.Location      = EDDI_GetBitField32(v, EDDI_SER_VERSION_BIT__Location);
    pDDB->ERTEC_Version.MacroRevision = EDDI_GetBitField32(v, EDDI_SER_VERSION_BIT__MacroRevision);
    pDDB->ERTEC_Version.Variante      = EDDI_GetBitField32(v, EDDI_SER_VERSION_BIT__Variante);
    pDDB->ERTEC_Version.Version       = EDDI_GetBitField32(v, EDDI_SER_VERSION_BIT__Version);
                           
    switch (pDDB->ERTEC_Version.Location)
    {
        case EDDI_LOC_FPGA_XC2_V8000_0:
        {
            pDDB->ERTEC_Version.Location = EDDI_LOC_FPGA_XC2_V8000;
        }
        //no break;
        //lint -fallthrough
        case EDDI_LOC_FPGA_XC2_V8000:
        {           
            switch (pDDB->ERTEC_Version.MacroRevision)
            {
                case  /*Rev5*/
                      0x05:  
                {
                    MaxKramSize = EDDI_KRAM_SIZE_FPGA400;
                    break;
                }
                case  /*Rev6*/
                      0x06:  
                {
                    MaxKramSize = EDDI_KRAM_SIZE_FPGA200;
                    break;
                }
                case  /*Rev7*/
                      0x07:  
                {
                    MaxKramSize = EDDI_KRAM_SIZE_FPGASOC;
                    break;
                }
                default:  
                {
                    MaxKramSize = 0;
                    Status      = EDD_STS_ERR_HW;
                }
            }
            break;
        }
        case EDDI_LOC_ERTEC400:
        {
            MaxKramSize = EDDI_KRAM_SIZE_ERTEC400;
            break;
        }
        case EDDI_LOC_ERTEC200:
        {
            MaxKramSize = EDDI_KRAM_SIZE_ERTEC200;
            if (pDPB->IRTE_SWI_BaseAdr_LBU_16Bit)
            {
                pDDB->IRTE_SWI_BaseAdr = pDPB->IRTE_SWI_BaseAdr_LBU_16Bit;
            }
            break;
        }
        case EDDI_LOC_SOC1:
        case EDDI_LOC_SOC2:
        {
            MaxKramSize = EDDI_KRAM_SIZE_SOC;
            break;
        }
        default:
        {
            MaxKramSize = 0;
            Status      = EDD_STS_ERR_HW;
        }
    }

    //Set the Const values
    pDDB->pConstValues = (EDDI_INI_PARAMS_FIXED_TYPE*)&EDDIConstValues[0];  /*lint !e1773*/

    EDDI_ValidateKram(pDDB, pDPB, MaxKramSize, &UsedKramSize);

    EDDI_PmCheck(pDDB, pDPB);

    pDDB->ERTECx00_SCRB_BaseAddr    = pDPB->ERTECx00_SCRB_BaseAddr;

    EDDI_CheckBaseAddr(pDDB);

    pDDB->Glob.InterfaceID          = pDPB->InterfaceID;
    pDDB->Glob.TraceIdx             = pDPB->TraceIdx; 

    pDDB->Glob.bSupportIRTflex = (EDD_FEATURE_ENABLE == pDPB->bSupportIRTflex) ? LSA_TRUE : LSA_FALSE;
    pDDB->Glob.bDisableMaxPortDelayCheck = (EDDI_DPB_DISABLE_MAX_PORTDELAY_CHECK == pDPB->bDisableMaxPortDelayCheck) ? LSA_TRUE : LSA_FALSE;

    if (pDPB->MaxInterfaceCntOfAllEDD > EDD_CFG_MAX_INTERFACE_CNT)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DeviceOpen, MaxInterfaceCntOfAllEDD:0x%X is bigger then EDD_CFG_MAX_INTERFACE_CNT:0x%X", pDPB->MaxInterfaceCntOfAllEDD, EDD_CFG_MAX_INTERFACE_CNT);
        EDDI_Excp("EDDI_DeviceOpen, MaxInterfaceCntOfAllEDD is bigger then EDD_CFG_MAX_INTERFACE_CNT", EDDI_FATAL_ERR_EXCP, pDPB->MaxInterfaceCntOfAllEDD, EDD_CFG_MAX_INTERFACE_CNT);
        return;
    }
    else
    {
        pDDB->Glob.MaxInterfaceCntOfAllEDD = pDPB->MaxInterfaceCntOfAllEDD;
    }

    if (pDPB->MaxPortCntOfAllEDD > EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DeviceOpen, MaxPortCntOfAllEDD:0x%X is bigger then EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE:0x%X", pDPB->MaxPortCntOfAllEDD, EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE);
        EDDI_Excp("EDDI_DeviceOpen, MaxPortCntOfAllEDD is bigger then EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE", EDDI_FATAL_ERR_EXCP, pDPB->MaxPortCntOfAllEDD, EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE);
        return;
    }
    else
    {
        pDDB->Glob.MaxPortCntOfAllEDD = pDPB->MaxPortCntOfAllEDD;
    }

    if (EDD_STS_OK == EDDI_CheckDPBFeatures(pDDB, pDPB))
    {
        pDDB->FeatureSupport = pDPB->FeatureSupport;

        if (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.AdditionalForwardingRulesSupported)
        {
            pDDB->HSYNCRole = EDDI_HSYNC_ROLE_NONE;
        }
        else if (EDD_FEATURE_ENABLE != pDPB->FeatureSupport.ApplicationExist)
        {
            pDDB->HSYNCRole = EDDI_HSYNC_ROLE_FORWARDER;
        }
        else
        {
            pDDB->HSYNCRole = EDDI_HSYNC_ROLE_APPL_SUPPORT;
        }
    }

    EDDI_SYSTEM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_DeviceOpen->");

    //EDDI_MemSet(&pDDB->Glob.ErrInt, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DEVICE_ERR_INTS));
    //returnvalue: DDB handle
    pRqbDDBRel->hDDB = pDDB->hDDB;

    #if defined (EDDI_CFG_REV6)
    EDDI_SwiPhyE200SetPhyExtern(pDDB);
    #else
    pDDB->Glob.bPhyExtern = LSA_TRUE;
    #endif

    pDDB->Glob.HardwareType = EDD_HW_TYPE_FEATURE_TIMESTAMP_SUPPORT              |
                              EDD_HW_TYPE_FEATURE_DELAY_SUPPORT                  |
                              EDD_HW_TYPE_FEATURE_CT_SUPPORT                     |
                              EDD_HW_TYPE_FEATURE_SYNC_DELAY_SW_SUPPORT          |
                              EDD_HW_TYPE_FEATURE_SYNC_SLAVE_SW_SUPPORT          |
                              EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT;

    #if defined (EDDI_CFG_REV6)
    if (pDDB->ERTEC_Version.MacroRevision != 0x06)
    {
        Status = EDD_STS_ERR_HW;
    }
    pDDB->Glob.HardwareType |= EDD_HW_TYPE_USED_ERTEC_200                              |
                               EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_1_UDP_SUPPORT |
                               EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_SUPPORT       |
                               EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_3_SUPPORT     |
                               EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT  |
                               EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT     |
                               EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT      |
                               EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE              |
                               EDD_HW_TYPE_FEATURE_SYNC_MASTER_NO_SUPPORT              |
                               EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT      |
                               EDD_HW_TYPE_FEATURE_TIME_SLAVE_NO_SUPPORT               |
                               EDD_HW_TYPE_FEATURE_TIME_MASTER_NO_SUPPORT; 
    #elif defined (EDDI_CFG_REV7)
    if (pDDB->ERTEC_Version.MacroRevision != 0x07)
    {
        Status = EDD_STS_ERR_HW;
    }
    pDDB->Glob.HardwareType |= EDD_HW_TYPE_USED_SOC                                 |
                               EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_SUPPORT |
                               EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_SUPPORT    |
                               EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_SUPPORT     |
                               EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_SUPPORT  |
                               EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_SUPPORT     |
                               EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_SUPPORT      |
                               EDD_HW_TYPE_FEATURE_SYNC_MASTER_SW_SUPPORT           |
                               EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT   |
                               EDD_HW_TYPE_FEATURE_TIME_SLAVE_NO_SUPPORT            |
                               EDD_HW_TYPE_FEATURE_TIME_MASTER_SUPPORT; 

    pDDB->Glob.HardwareType |= ((0 != pDDB->CRT.PAEA_Para.PAEA_BaseAdr)?EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE:EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE);
    #else
    if (pDDB->ERTEC_Version.MacroRevision != 0x05)
    {
        Status = EDD_STS_ERR_HW;
    }
    pDDB->Glob.HardwareType |= EDD_HW_TYPE_USED_ERTEC_400                              |
                               EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_1_UDP_SUPPORT |
                               EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_12_SUPPORT    |
                               EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_3_SUPPORT     |
                               EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT  |
                               EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT     |
                               EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT      |
                               EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE              |
                               EDD_HW_TYPE_FEATURE_SYNC_MASTER_SW_SUPPORT              |
                               EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT      |
                               EDD_HW_TYPE_FEATURE_TIME_SLAVE_NO_SUPPORT               |
                               EDD_HW_TYPE_FEATURE_TIME_MASTER_NO_SUPPORT; 
    #endif

    #if defined (EDDI_CFG_FRAG_ON)
    if (EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_NO != pDDB->FeatureSupport.FragmentationtypeSupported)
    {
        pDDB->Glob.HardwareType |= EDD_HW_TYPE_FEATURE_FRAGMENTATION_SUPPORT;
    }
    else
    #endif
    {
        pDDB->Glob.HardwareType |= EDD_HW_TYPE_FEATURE_NO_FRAGMENTATION_SUPPORT;
    }

    pDDB->kram_adr_asic_to_local   = EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL;
    pDDB->shared_mem_adr_to_asic   = EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC;
    pDDB->sdram_mem_adr_to_asic    = EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC;
    pDDB->kram_adr_loc_to_asic_dma = EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA;

    switch (pDDB->ERTEC_Version.Location)
    {
        case EDDI_LOC_FPGA_XC2_V8000:
        {
            #if defined (EDDI_WITH_OLD_KRAM_SEGMENTATION)
            #if defined (EDDI_CFG_REV7)
            if ((0 != pDDB->CRT.PAEA_Para.PAEA_BaseAdr) && (UsedKramSize != MaxKramSize))
            #else
            if (UsedKramSize != MaxKramSize)
            #endif
            #else
            #if defined (EDDI_CFG_REV7)
            if ((0 != pDDB->CRT.PAEA_Para.PAEA_BaseAdr) && (UsedKramSize > MaxKramSize))
            #else
            if (UsedKramSize > MaxKramSize)
            #endif
            #endif
            {
                EDDI_Excp("EDDI_DeviceOpen, EDDI_KRAM_SIZE_FPGA", EDDI_FATAL_ERR_EXCP, UsedKramSize, 0);
                return;
            }

            break;
        }

        case EDDI_LOC_ERTEC400:
        {
            #if defined (EDDI_WITH_OLD_KRAM_SEGMENTATION)
            if (UsedKramSize != MaxKramSize)
            #else
            if (UsedKramSize > MaxKramSize)
            #endif
            {
                EDDI_Excp("EDDI_DeviceOpen, EDDI_KRAM_SIZE_ERTEC400", EDDI_FATAL_ERR_EXCP, UsedKramSize, 0);
                return;
            }

            break;
        }

        case EDDI_LOC_ERTEC200:
        {
            #if defined (EDDI_WITH_OLD_KRAM_SEGMENTATION)
            if (UsedKramSize != MaxKramSize)
            #else
            if (UsedKramSize > MaxKramSize)
            #endif
            {
                EDDI_Excp("EDDI_DeviceOpen, EDDI_KRAM_SIZE_ERTEC200", EDDI_FATAL_ERR_EXCP, UsedKramSize, 0);
                return;
            }

            break;
        }

        case EDDI_LOC_SOC1:
        {
            #if defined (EDDI_WITH_OLD_KRAM_SEGMENTATION)
            if (UsedKramSize != MaxKramSize)
            #else
            if (UsedKramSize > MaxKramSize)
            #endif
            {
                EDDI_Excp("EDDI_DeviceOpen, KRAM_SIZE_SOC1", EDDI_FATAL_ERR_EXCP, UsedKramSize, 0);
                return;
            }

            break;
        }

        case EDDI_LOC_SOC2:
        {
            #if defined (EDDI_WITH_OLD_KRAM_SEGMENTATION)
            if (UsedKramSize != MaxKramSize)
            #else
            if (UsedKramSize > MaxKramSize)
            #endif
            {
                EDDI_Excp("EDDI_DeviceOpen, KRAM_SIZE_SOC2", EDDI_FATAL_ERR_EXCP, UsedKramSize, 0);
                return;
            }

            break;
        }

        default:
        {
            Status = EDD_STS_ERR_HW;
        }
    }

    #if defined (EDDI_CFG_REV6)
    EDDI_SwiPhyE200Reset(pDDB); //from now on the 6ms are running!
    #endif

    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_DeviceOpen, invalid Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    EDDI_DisableBootLoader(pDDB, &pDPB->Bootloader, pDPB->BOARD_SDRAM_BaseAddr);

    pDDB->CloseReq.pBottom = EDDI_NULL_PTR;
    pDDB->CloseReq.pTop    = EDDI_NULL_PTR;

    if (pDDB->Glob.SetMode == EDDI_HW_SETUP_ALL)
    {
        EDDI_SERReset(pDDB, LSA_TRUE /*bCalledForOpen*/);
    }

    EDDI_MEMIni(0x28UL,
                0x08UL,
                pDDB->SizeKRamEDDI,
                pDDB->pKRamEDDI,
                EDDI_MEM_DO_NO_PRESET,
                pDDB,
                &pDDB->KRamMemHandle);

    EDDI_IniProfKRamInfo(pDDB, pDPB);

    //EDDI_MemSet(pDDB->pKRam, 0xAA, pDPB->KRam.offset_ProcessImageEnd);

    EDDI_PROFIni(pDDB);
    EDDI_PrmIni(pDDB);
    EDDI_IniIsrServiceTable(pDDB);

    EDDI_SIIInit(pDDB);

    EDD_RQB_SET_RESPONSE(pRQB, Status);
    if (!(0 == pRqbDDBRel->Cbf))
    {
        pRqbDDBRel->Cbf(pRQB);  //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckBaseAddr()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CheckBaseAddr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;
    LSA_UINT32                  Ctr;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CheckBaseAddr->");

    if (   (0                 == pDDB->ERTECx00_SCRB_BaseAddr)
        && (EDDI_LOC_ERTEC200 == pDDB->ERTEC_Version.Location))
    {
        EDDI_Excp("EDDI_CheckBaseAddr, ERTECx00_SCRB_BaseAddr = 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (pDDB->pLocal_NRT == EDDI_NULL_PTR)
    {
        return;
    }

    for (Ctr = EDDI_NRT_CHA_IF_0; Ctr < EDDI_NRT_IF_CNT; Ctr++) //A0, B0
    {
        pIF = &pDDB->NRT.IF[Ctr];

        if (pIF->StateDeferredAlloc == DEF_ALLOC_DONT_CARE)
        {
            continue;
        }

        pIF->LimitDown_BufferAddr = pDDB->NRTMEM_LowerLimit;
        pIF->LimitUp_BufferAddr   = pDDB->NRTMEM_UpperLimit;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_ValidateKram()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_ValidateKram( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_UPPER_DPB_PTR_TYPE     const  pDPB,
                                                          LSA_UINT32                  const  MaxKramSize,
                                                          LSA_UINT32               *  const  pUsedKramSize )  //OUT
{
    LSA_UINT32  const  KramSizeReservedForEdd = pDPB->KRam.size_reserved_for_eddi;
    LSA_UINT32  const  ProcessImageLen        = pDPB->KRam.offset_ProcessImageEnd;
    LSA_UINT32  const  UsedKramSize           = ProcessImageLen + KramSizeReservedForEdd;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_ValidateKram->");

    pDDB->KramRes.ProcessImage = ProcessImageLen;

    pDDB->KramRes.ArpDcpCCWLen           = 0;
    pDDB->KramRes.ArpFilterTableLen      = 0;
    pDDB->KramRes.DCP_FilterStringBufLen = 0;
    pDDB->KramRes.DCP_FilterTableLen     = 0;
    pDDB->KramRes.TimeMasterListLen      = 0;
    pDDB->KramRes.MCTable                = 0;

    #if defined (EDDI_CFG_REV7)
    if (0 != pDPB->PAEA_BaseAdr)
    {
        pDDB->CRT.PAEA_Para.PAEA_BaseAdr = pDPB->PAEA_BaseAdr;
    }
    #endif

    if (KramSizeReservedForEdd < 0x8000)
    {
        EDDI_Excp("EDDI_ValidateKram, KRam.size_reserved_for_eddi < 0x8000", EDDI_FATAL_ERR_EXCP, KramSizeReservedForEdd, 0);
        return;
    }

    if (KramSizeReservedForEdd & 3)
    {
        EDDI_Excp("EDDI_ValidateKram, KRam.size_reserved_for_eddi Alignment Problem", EDDI_FATAL_ERR_EXCP, KramSizeReservedForEdd, 0);
        return;
    }

    #if !defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)
    pDDB->ProcessImage.pKRAMMirror             = EDDI_NULL_PTR;
    #endif //EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK

    #if defined (EDDI_CFG_REV7)
    pDDB->CRT.PAEA_Para.pConsumerMirrorArray   = EDDI_NULL_PTR;
    pDDB->CRT.PAEA_Para.ConsumerMirrorArrayLen = 0;
    #endif
   
    if (ProcessImageLen & 3)
    {
        EDDI_Excp("EDDI_ValidateKram, KRam.offset_ProcessImageEnd Alignment Problem", EDDI_FATAL_ERR_EXCP, ProcessImageLen, 0);
        return;
    }

    #if !defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)
    if (ProcessImageLen >= 4)
    {
        LSA_UINT32  const  ProcessImageLenDiv4 = ProcessImageLen / 4;

        EDDI_AllocLocalMem((void * *)&pDDB->ProcessImage.pKRAMMirror, ProcessImageLenDiv4);
        if (pDDB->ProcessImage.pKRAMMirror == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_ValidateKram, alloc pDDB->ProcessImage.pKRAMMirror", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }
        EDDI_MemSet(pDDB->ProcessImage.pKRAMMirror, (LSA_UINT8)CRT_MIRROR_REMOVE, ProcessImageLenDiv4);
    }
    #endif //EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK   
            
    pDDB->pKRam = (LSA_UINT8 *)(pDDB->IRTE_SWI_BaseAdr + 0x100000UL);

    pDDB->ProcessImage.pUserDMA = pDDB->pKRam;

    //reserved KRAM for ProcessImage
    pDDB->ProcessImage.offset_ProcessImageEnd = ProcessImageLen;

    //fix Memoryverwaltung for EDD
    pDDB->pKramFixMem = (EDDI_KRAM_MEM_TYPE_T *)(void *)(pDDB->pKRam + (MaxKramSize - KramSizeReservedForEdd));

    pDDB->PABorderLine.pBoLiKram = &pDDB->pKramFixMem->BorderLine;

    //at end of KRAM
    pDDB->pProfKRamInfo = &g_pEDDI_Info ->ProfInfo[pDDB->DeviceNr];

    //free Memory for EDD
    pDDB->pKRamEDDI    = (LSA_UINT8 *)(void *)pDDB->pKramFixMem + sizeof(EDDI_KRAM_MEM_TYPE_T);
    pDDB->SizeKRamEDDI = KramSizeReservedForEdd - (sizeof(EDDI_KRAM_MEM_TYPE_T));

    pDDB->KramRes.PABorderLine = sizeof(EDDI_DDB_PA_BORDERLINE_KRAM_TYPE);

    //plausible SII configuration parameters
    {
        pDDB->SII.SII_IrqSelector = pDPB->SII_IrqSelector;

        switch (pDDB->SII.SII_IrqSelector)
        {
            case EDDI_SII_IRQ_SP:
            case EDDI_SII_IRQ_HP:
            {
                break;
            }
            default:
            {
                EDDI_Excp("EDDI_ValidateKram, invalid SII_IrqSelector:", EDDI_FATAL_ERR_EXCP, pDDB->SII.SII_IrqSelector, 0);
                return;
            }
        }

        pDDB->SII.SII_IrqNumber = pDPB->SII_IrqNumber;

        switch (pDDB->SII.SII_IrqNumber)
        {
            case EDDI_SII_IRQ_0:
            case EDDI_SII_IRQ_1:
            {
                break;
            }
            default:
            {
                EDDI_Excp("EDDI_ValidateKram, invalid SII_IrqNumber:", EDDI_FATAL_ERR_EXCP, pDDB->SII.SII_IrqNumber, 0);
                return;
            }
        }

        //check range of EOI inactive time
        #if !defined (EDDI_SII_MAX_EOI_INACTIVE_TIME) //MaxEoiInactiveTime not constant
        {
            #if !defined (EDDI_CFG_SII_USE_SPECIAL_EOI) || (EDDI_CFG_SII_USE_SPECIAL_EOI > 0)
            LSA_UINT32  MaxEoiInactiveTime = 0;

            #if defined (EDDI_CFG_REV6)
            if (pDDB->SII.SII_IrqSelector == EDDI_SII_IRQ_SP)
            {
                MaxEoiInactiveTime = (LSA_UINT32)EDDI_SII_MAX_EOI_INACTIVE_TIME_REV6_IRQ_SP; //for IRTE EOI register
            }
            else
            {
                MaxEoiInactiveTime = (LSA_UINT32)EDDI_SII_MAX_EOI_INACTIVE_TIME_REV6_IRQ_HP; //for IRTE EOI register
            }
            #endif
            #endif

            #if defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
            //check constant EoiInactiveTime only here
            #if (EDDI_CFG_SII_USE_SPECIAL_EOI > 0) //satisfy lint!
            if (EDDI_CFG_SII_USE_SPECIAL_EOI > MaxEoiInactiveTime)
            {
                EDDI_Excp("EDDI_ValidateKram, invalid EDDI_CFG_SII_USE_SPECIAL_EOI:", EDDI_FATAL_ERR_EXCP, EDDI_CFG_SII_USE_SPECIAL_EOI, MaxEoiInactiveTime);
                return;
            }
            #endif
            #else
            //store MaxEoiInactiveTime for later checks
            pDDB->SII.SII_MaxEoiInactiveTime = MaxEoiInactiveTime;
            #endif
        }
        #endif

        #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
        pDDB->SII.SII_ExtTimerInterval = pDPB->SII_ExtTimerInterval;

        switch (pDDB->SII.SII_ExtTimerInterval)
        {
            #if defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
            case 0: break;  //= ExtTimerMode off
            #endif
            case 250UL:     //= ExtTimerMode on
            case 500UL:     //= ExtTimerMode on
            case 1000UL:    //= ExtTimerMode on
            {
                #if defined (EDDI_CFG_APPLSYNC_SHARED) || defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)
                //ApplSyncSharedMode not allowed with ExtTimerMode!
                EDDI_Excp("EDDI_ValidateKram, ExtTimerMode, ApplSyncSharedMode not allowed with ExtTimerMode", EDDI_FATAL_ERR_EXCP, pDDB->SII.SII_ExtTimerInterval, 0);
                return;
                #else
                break;
                #endif
            }
            default:
            {
                EDDI_Excp("EDDI_ValidateKram, ExtTimerMode, invalid SII_ExtTimerInterval:", EDDI_FATAL_ERR_EXCP, pDDB->SII.SII_ExtTimerInterval, 0);
                return;
            }
        }
        #endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX

        #if defined (EDDI_CFG_SII_FLEX_MODE)
        pDDB->SII.SII_Mode = pDPB->SII_Mode;

        switch (pDDB->SII.SII_Mode)
        {
            case EDDI_SII_STANDARD_MODE:
            case EDDI_SII_POLLING_MODE:
            {
                break;
            }
            default:
            {
                EDDI_Excp("EDDI_ValidateKram, SII-Flex-Mode, invalid SII_Mode:", EDDI_FATAL_ERR_EXCP, pDDB->SII.SII_Mode, 0);
                return;
            }
        }
        #endif //EDDI_CFG_SII_FLEX_MODE
    }

    //plausible I2C configuration parameters
    {
        pDDB->I2C.I2C_Type = pDPB->I2C_Type;
        pDDB->I2C.I2CMuxSelectStored  = 0;
        pDDB->I2C.bI2CMuxSelectIgnore = LSA_FALSE;

        switch (pDDB->I2C.I2C_Type)
        {
            case EDDI_I2C_TYPE_NONE:
            case EDDI_I2C_TYPE_GPIO:
            #if defined (EDDI_CFG_REV7)
		    case EDDI_I2C_TYPE_SOC1_HW:
            #endif
            {
                break;
            }
            default:
            {
                EDDI_Excp("EDDI_ValidateKram, invalid I2C_Type:", EDDI_FATAL_ERR_EXCP, pDDB->I2C.I2C_Type, 0);
                return;
            }
        }
    }

    *pUsedKramSize = UsedKramSize;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IniProfKRamInfo()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IniProfKRamInfo( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             EDDI_UPPER_DPB_PTR_TYPE  const  pDPB )
{
    LSA_UINT32  i;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IniProfKRamInfo->");

    for (i = 0; i < EDDI_BORDERLINE_ELEMENTS; i++)
    {
        pDDB->PABorderLine.pBoLiKram->dw[i] = EDDI_BORDERLINE_PAT;
    }

    //Diagnose Eintrag ins KRAM
    //Initialization needed due to CycleHistory Array
    EDDI_MemSet(pDDB->pProfKRamInfo, (LSA_UINT8)0, (LSA_UINT32)sizeof(PROF_KRAM_INFO));
    EDDI_KramSyncDpbContext(pDDB, pDPB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_DeviceSetup()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceSetup( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT                            Status;
    EDDI_UPPER_DDB_SETUP_PTR_TYPE  const  pRqbDDBRel = (EDDI_UPPER_DDB_SETUP_PTR_TYPE)pRQB->pParam;
    EDDI_LOCAL_DDB_PTR_TYPE               pDDB;
    EDDI_UPPER_DSB_PTR_TYPE               pDSB; /* Pointer to device setup-parameter */

    Status = EDDI_GetDDB(pRqbDDBRel->hDDB, &pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_DeviceSetup", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }
    
    EDDI_SYSTEM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeviceSetup->");
    
    pDSB = pRqbDDBRel->pDSB; /* pointer to hardwarespecific parameters */

    /* Check if already setup or not all components initialized. */
    /* If so we signal a sequence-error.                         */

    if ((pDDB->Glob.HWIsSetup) || (pDDB->pLocal_NRT == EDDI_NULL_PTR) || (pDDB->pLocal_SWITCH == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetup", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if ((pDDB->pLocal_SYNC == EDDI_NULL_PTR) || (pDDB->pLocal_IRT == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetup, Forgot to call EDDI_SRV_DEV_COMP_INI ?", EDDI_FATAL_ERR_EXCP, pDDB->pLocal_SYNC, pDDB->pLocal_IRT);
        return;
    }

    if ((pDDB->pLocal_SRT == EDDI_NULL_PTR) || (pDDB->pLocal_CRT == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_DeviceSetup, Forgot to call EDDI_SRV_DEV_COMP_INI ?", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (0 == pDDB->CRT.CfgPara.IRT.ConsumerCntClass3)
    {
        EDDI_Excp("EDDI_DeviceSetup, CRT-IRT-ConsumerCntClass3 is 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (0 == pDDB->CRT.CfgPara.IRT.ForwarderCnt)
    {
        EDDI_Excp("EDDI_DeviceSetup, CRT-IRT-ForwarderCnt is 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (0 == pDDB->CRT.CfgPara.IRT.ProviderCnt)
    {
        EDDI_Excp("EDDI_DeviceSetup, CRT-IRT-ProviderCnt is 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    /* -------------------------------------------------------------------------*/
    /* check DSB if MRP is used and a valid combination set variable            */
    /* bMRPRedundantPortsUsed within SWITCH-management                          */
    /* -------------------------------------------------------------------------*/
    EDDI_CheckDSBMRPUsed(pDDB, pDSB);

    EDDI_DeviceSetupSER(pDDB, pDSB);

    EDDI_SYNCInitComponentPart2(pDDB);

    #if defined (EDDI_CFG_DFP_ON)
    //Init and alloc PF and SF memory
    EDDI_CRTDFPDataInitComponent(pDDB);
    #endif

    EDDI_CheckDSB(pDSB, pDDB);

    #if defined (EDDI_CFG_REV7)
    IO_W32(UDP_PNIO_PORT0, EDDI_ACW_xRT_UDP_PORT);
    IO_W32(IRTE_R7_IP_TTL, EDDI_IP_TTL_UDP_DEFAULT);
    #endif

    //entries are not set via command interface, because learning is not possible at the moment
    EDDI_SerSetStatFDBEntries(pDDB, pDSB);

    EDDI_CRTEDDI_RedTreeInit(pDDB);

    EDDI_SYNCBuildIRT(pDDB);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    pDDB->CRT.SERIniRcwFct = EDDI_SERIniRcwChange;
    #endif

    EDD_RQB_SET_RESPONSE(pRQB, Status);

    EDDI_KramCheckFreeBuffer(pDDB);

    pDSB->free_internal_KRAM_buffer = pDDB->free_KRAM_buffer;

    pDDB->KramRes.FreeBytes = pDDB->free_KRAM_buffer;

    EDDI_KramRes(pDDB);

    pDDB->NramRes.LocalMem = g_LocalMem;

    pDDB->Glob.HWIsSetup = LSA_TRUE;

    pDDB->IntStat.last_in_10ns_ticks = IO_GET_CLK_COUNT_VALUE_10NS;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_GenR6Ini(pDDB);
    #endif

    EDDI_NRTInitARPFilter(pDDB);
    EDDI_NRTInitDCPFilter(pDDB);
    EDDI_NRTInitDCPHelloFilter(pDDB);
    EDDI_NrtSetSendLimit(pDDB);

    #if defined (EDDI_CFG_DEFRAG_ON)
    EDDI_NRTInitRxFrag(pDDB);
    #endif
    
    EDDI_SERIniStatistic(pDDB);

    EDDI_NRTSetMaxIFFrameLen(pDDB);

    EDDI_StartDeviceSetUpTimer(pDDB);

    pDSB->Ticks_till_PhyReset = EDDI_SWIStartPhys(pDDB);

    pDDB->pProfKRamInfo->Ticks_till_PhyReset = pDSB->Ticks_till_PhyReset;

    EDDI_SIIStartInterrupts(pDDB);

    if (!(0 == pRqbDDBRel->Cbf))
    {
        pRqbDDBRel->Cbf(pRQB); //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_StartDeviceSetUpTimer()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_StartDeviceSetUpTimer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT            Status;
    void        *  const  context = pDDB;

    EDDI_SYSTEM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_StartDeviceSetUpTimer->");

    #if defined (EDDI_CFG_FRAG_ON)
    /*-------------------------------------------------------------------------*/
    /* Timer for NRT Pre Queues                                                */
    /*-------------------------------------------------------------------------*/
    Status = EDDI_AllocTimer(pDDB,
                             &pDDB->NRT.NRTPreQueuesTimerID,
                             (void *)pDDB /*context*/,
                             (EDDI_TIMEOUT_CBF)EDDI_NrtFragQueuesTimerCBF,
                             EDDI_TIMER_TYPE_CYCLIC,
                             EDDI_TIME_BASE_100MS,
                             EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_StartDeviceSetUpTimer, EDDI_AllocTimer FAILED, Status:0x%X TimerID:0x%X",
                          Status, pDDB->NRT.NRTPreQueuesTimerID);
        EDDI_Excp("EDDI_StartDeviceSetUpTimer, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->NRT.NRTPreQueuesTimerID);
        return;
    }
    #endif
 
    //timer for LED-blinking-frequency
    Status = EDDI_AllocTimer(pDDB,
                             &pDDB->Glob.LED_OnOffDuration_TimerID,
                             context,
                             (EDDI_TIMEOUT_CBF)EDDI_GenLEDOnOffDuration,
                             EDDI_TIMER_TYPE_CYCLIC,
                             EDDI_TIME_BASE_100MS,
                             EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_StartDeviceSetUpTimer, LED_OnOffDuration_TimerID, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->Glob.LED_OnOffDuration_TimerID);
        return;
    }

    EDDI_PrmIniTimer(pDDB);
    EDDI_SwiUsrIniTimer(pDDB);

    Status = EDDI_AllocTimer(pDDB, &pDDB->CloseReqTimerID, (void *)pDDB, (EDDI_TIMEOUT_CBF)EDDI_DeferredCloseChannelReq, EDDI_TIMER_TYPE_ONE_SHOT, EDDI_TIME_BASE_100MS, EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_StartDeviceSetUpTimer, EDDI_AllocTimer pDDB->CloseReqTimerID", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    Status = EDDI_AllocTimer(pDDB, &pDDB->Glob.CycleCountTimerID, context, (EDDI_TIMEOUT_CBF)EDDI_CycTimerCbfSoftwareCounter, EDDI_TIMER_TYPE_CYCLIC, EDDI_TIME_BASE_100MS, EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_StartDeviceSetUpTimer, CycleCountTimerID", EDDI_FATAL_ERR_EXCP, Status, pDDB->Glob.CycleCountTimerID);
        return;
    }

    (LSA_VOID)EDDI_StartTimer(pDDB, pDDB->Glob.CycleCountTimerID, (LSA_UINT16)5); /* 500ms 4 fach Abtastung */

    /*-------------------------------------------------------------------------*/
    /* Age timer for FDB-table                                                 */
    /*-------------------------------------------------------------------------*/
    if ((pDDB->pLocal_SWITCH->AgePollTimeBase) && (pDDB->pLocal_SWITCH->AgePollTime))
    {
        Status = EDDI_StartTimer(pDDB, pDDB->pLocal_SWITCH->AgePollTimerId, pDDB->pLocal_SWITCH->AgePollTime);
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_StartDeviceSetUpTimer, AgePollTimerId", EDDI_FATAL_ERR_EXCP, Status, 0);
            return;
        }
    }

    #if defined (EDDI_CFG_REV6)
    Status = EDDI_AllocTimer(pDDB, &pDDB->SWITCH.PhyPhaseShift_TimerId, (void *)pDDB,
                             (EDDI_TIMEOUT_CBF)EDDI_SwiPhyPhaseShift_Timer,
                             EDDI_TIMER_TYPE_ONE_SHOT, EDDI_TIME_BASE_10MS, EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_StartDeviceSetUpTimer, PhyPhaseShift_TimerId", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }
    #endif

    //DFP SW-WD
    EDDI_DFPWatchDogTimerAlloc(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckDSBMRPUsed()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*    Checks if MRP is used by parsing the DSB and setup the variable      */
/*    bMRPRedundantPortsUsed. On inconsistency we set a fatal error        */
/*    Setup MRP_PortState for LinkState/PortState.                         */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CheckDSBMRPUsed( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             EDDI_UPPER_DSB_PTR_TYPE  const  pDSB )
{
    LSA_UINT32                    UsrPortIndex, HwPortIndex;
    LSA_UINT32                    MRPDefaultRingPortCnt = 0;
    LSA_UINT32                    MRPRoleNonePortCnt    = 0;
    EDDI_PORT_PARAMS_TYPE      *  pPortParams;
    LSA_UINT32             const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CheckDSBMRPUsed->");

    EDDI_MEMSET(pDDB->SWITCH.HSyncUserPortID, 0, sizeof(pDDB->SWITCH.HSyncUserPortID));

    //check if we use MRP
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        pPortParams = &pDSB->GlobPara.PortParams[UsrPortIndex];

        /* --------------------------------------------------------------------*/
        /* Is this a Port used with MRP ? . Setup variables.                   */
        /* --------------------------------------------------------------------*/

        if (EDD_MRP_RING_PORT_DEFAULT == pPortParams->MRPRingPort)
        {
            if (MRPDefaultRingPortCnt < EDDI_MAX_HSYNC_PORT_CNT)
            {
                pDDB->SWITCH.HSyncUserPortID[MRPDefaultRingPortCnt] = (LSA_UINT16) UsrPortIndex + 1;
            }

            MRPDefaultRingPortCnt++;
            pDDB->SWITCH.LinkPx[HwPortIndex].bMRPRedundantPort = LSA_TRUE;  /* configuration state (DSB) */
            pDDB->Glob.PortParams[HwPortIndex].MRPRingPort     = EDD_MRP_RING_PORT_DEFAULT;
        }
        else if (   (EDD_MRP_RING_PORT    == pPortParams->MRPRingPort)
                 || (EDD_MRP_NO_RING_PORT == pPortParams->MRPRingPort))
        {
            if (EDD_MRP_NO_RING_PORT == pPortParams->MRPRingPort)
            {
                MRPRoleNonePortCnt++;
            }
            pDDB->SWITCH.LinkPx[HwPortIndex].bMRPRedundantPort = LSA_FALSE; /* configuration state (DSB) */
            pDDB->Glob.PortParams[HwPortIndex].MRPRingPort     = pPortParams->MRPRingPort;
        }
        else
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, illegal value 0x%X for MRPRingPort on UsrPort %d", 
                                  pPortParams->MRPRingPort, UsrPortIndex);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, illegal value for MRPRingPort!", EDDI_FATAL_ERR_EXCP, pPortParams->MRPRingPort, UsrPortIndex);
            return;
        }
        
        if (EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_NO == pPortParams->SupportsMRPInterconnPortConfig)
        {
            pDDB->Glob.PortParams[HwPortIndex].SupportsMRPInterconnPortConfig = pPortParams->SupportsMRPInterconnPortConfig; 
        }
        else
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, illegal value 0x%X for SupportsMRPInterconnPortConfig on UsrPort %d", 
                pPortParams->SupportsMRPInterconnPortConfig, UsrPortIndex);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, illegal value for MRPInPort!", EDDI_FATAL_ERR_EXCP, pPortParams->SupportsMRPInterconnPortConfig, UsrPortIndex);
            return;
        }
    } //end of for-loop

    /* Checks for MRP automanager */
    if (   (EDD_MRA_ENABLE_LEGACY_MODE  != pDSB->GlobPara.MRAEnableLegacyMode)
        && (EDD_MRA_DISABLE_LEGACY_MODE != pDSB->GlobPara.MRAEnableLegacyMode))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MRAEnableLegacyMode illegal value (%u)",
            pDSB->GlobPara.MRAEnableLegacyMode);
        EDDI_Excp("EDDI_CheckDSBMRPUsed, MRAEnableLegacyMode illegal value", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MRAEnableLegacyMode, 0);
    }
    else
    {
        pDDB->SWITCH.MRAEnableLegacyMode = pDSB->GlobPara.MRAEnableLegacyMode;
    }

    /* Checks for MRP */
    if (pDSB->GlobPara.MaxMRP_Instances > EDD_CFG_MAX_MRP_INSTANCE_CNT)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MaxMRP_Instances is not valid, MaxMRP_Instances:0x%X, EDD_CFG_MAX_MRP_INSTANCE_CNT:0x%X", 
                              pDSB->GlobPara.MaxMRP_Instances, EDD_CFG_MAX_MRP_INSTANCE_CNT);
        EDDI_Excp("EDDI_CheckDSBMRPUsed, MaxMRP_Instances is not valid, MaxMRP_Instances, EDD_CFG_MAX_MRP_INSTANCE_CNT", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MaxMRP_Instances, EDD_CFG_MAX_MRP_INSTANCE_CNT);
        return;
    }

    if (   (0 == pDSB->GlobPara.MaxMRP_Instances) 
        && (   (0 != pDSB->GlobPara.MRPSupportedRole)
            || (0 != pDSB->GlobPara.MRPDefaultRoleInstance0)
            || (PortMapCnt != MRPRoleNonePortCnt)
           )
       )
    {
        EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, 
                              "EDDI_CheckDSBMRPUsed, MaxMRP_Instances is %u, MRPSupportedRole is: 0x%X, MRPDefaultRoleInstance0 is: 0x%X, MRPRoleNonePortCnt is: %u", 
                              pDSB->GlobPara.MaxMRP_Instances, pDSB->GlobPara.MRPSupportedRole, pDSB->GlobPara.MRPDefaultRoleInstance0, MRPRoleNonePortCnt);
        if (0 != pDSB->GlobPara.MRPSupportedRole)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MaxMRP_Instances is %u, MRPSupportedRole is: 0x%X and should be 0!", 
                                  pDSB->GlobPara.MaxMRP_Instances, pDSB->GlobPara.MRPSupportedRole);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, MaxMRP_Instances is %u, MRPSupportedRole is: 0x%X and should be 0!", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MaxMRP_Instances, pDSB->GlobPara.MRPSupportedRole);
        }

        if(0 != pDSB->GlobPara.MRPDefaultRoleInstance0)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MaxMRP_Instances is %u, MRPDefaultRoleInstance0 is: 0x%X and should be 0!", 
                                  pDSB->GlobPara.MaxMRP_Instances, pDSB->GlobPara.MRPDefaultRoleInstance0);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, MaxMRP_Instances is %u, MRPDefaultRoleInstance0 is: 0x%X and should be 0!", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MaxMRP_Instances, pDSB->GlobPara.MRPDefaultRoleInstance0);
        }

        if(PortMapCnt != MRPRoleNonePortCnt)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, Not all Ports are not set to EDD_MRP_NO_RING_PORT! (MRPRoleNonePortCnt: %u, PortMapCnt: %u)", 
                                  MRPRoleNonePortCnt, PortMapCnt);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, Not all Ports are not set to EDD_MRP_NO_RING_PORT! (MRPRoleNonePortCnt: %u, PortMapCnt: %u)", EDDI_FATAL_ERR_EXCP, MRPRoleNonePortCnt, PortMapCnt);
        }

        return;
    }

    pDDB->SWITCH.MRPDefaultRoleInstance0   = EDD_MRP_ROLE_NONE;
    pDDB->SWITCH.MRPSupportedRole          = 0;
    pDDB->SWITCH.MaxMRP_Instances          = 0;
    pDDB->SWITCH.MRPSupportedMultipleRole  = 0;
    pDDB->SWITCH.MaxMRPInterconn_Instances = 0;
    pDDB->SWITCH.SupportedMRPInterconnRole = 0;

    if (0 == pDSB->GlobPara.MaxMRP_Instances) //MRP is off?
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckDSBMRPUsed, MRP is not used, pDSB->GlobPara.MaxMRP_Instances: %u", pDSB->GlobPara.MaxMRP_Instances);
        pDDB->SWITCH.bMRPRedundantPortsUsed = LSA_FALSE;

        // HSYNC - MRP restrictions
        if (pDDB->FeatureSupport.AdditionalForwardingRulesSupported)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, AdditionalForwardingRulesSupported is configured:0x%X, but MaxMRP_Instances is:0x%X", 
                       pDDB->FeatureSupport.AdditionalForwardingRulesSupported, pDSB->GlobPara.MaxMRP_Instances);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, AdditionalForwardingRulesSupported is configured:0x%X, but MaxMRP_Instances is:0x%X", EDDI_FATAL_ERR_EXCP,
                       pDDB->FeatureSupport.AdditionalForwardingRulesSupported, pDSB->GlobPara.MaxMRP_Instances);
            return;
        }
    }
    else
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CheckDSBMRPUsed, MRP is used");
        pDDB->SWITCH.bMRPRedundantPortsUsed = LSA_TRUE;

        if (0 != (MRPDefaultRingPortCnt & 1))
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MRPDefaultRingPortCnt is not even, MRPDefaultRingPortCnt:0x%X MRPDefaultRoleInstance0:0x%X", 
                                  MRPDefaultRingPortCnt, pDDB->SWITCH.MRPDefaultRoleInstance0);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, MRPDefaultRingPortCnt is not even, MRPDefaultRingPortCnt:0x%X MRPDefaultRoleInstance0:0x%X", EDDI_FATAL_ERR_EXCP, MRPDefaultRingPortCnt, pDDB->SWITCH.MRPDefaultRoleInstance0);
            return;
        }
        
        if ((MRPDefaultRingPortCnt / 2) > pDSB->GlobPara.MaxMRP_Instances) 
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MaxMRP_Instances is not valid, MaxMRP_Instances:0x%X EDD_CFG_MAX_MRP_INSTANCE_CNT:0x%X", 
                                  pDDB->SWITCH.MaxMRP_Instances, EDD_CFG_MAX_MRP_INSTANCE_CNT);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, MaxMRP_Instances is not valid, MaxMRP_Instances:0x%X EDD_CFG_MAX_MRP_INSTANCE_CNT:0x%X", EDDI_FATAL_ERR_EXCP, pDDB->SWITCH.MaxMRP_Instances, EDD_CFG_MAX_MRP_INSTANCE_CNT);
            return;
        }

        if (pDSB->GlobPara.MRPSupportedRole & (~(EDD_MRP_ROLE_CAP_MANAGER | EDD_MRP_ROLE_CAP_CLIENT | EDD_MRP_ROLE_CAP_AUTOMANAGER)) )
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, illegal MRPSupportedRole, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", 
                                  pDSB->GlobPara.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
            EDDI_Excp("EDDI_CheckDSBMRPUsed, illegal MRPSupportedRole, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
            return;
        }

        pDDB->SWITCH.MRPSupportedRole = pDSB->GlobPara.MRPSupportedRole;
        pDDB->SWITCH.MaxMRP_Instances = pDSB->GlobPara.MaxMRP_Instances;    

        switch (pDSB->GlobPara.MRPDefaultRoleInstance0)
        {
            case EDD_MRP_ROLE_MANAGER:
            {
                pDDB->SWITCH.MRPDefaultRoleInstance0 = EDD_MRP_ROLE_MANAGER;
                if (!(pDDB->SWITCH.MRPSupportedRole & EDD_MRP_ROLE_CAP_MANAGER))
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MRPSupportedRole does not match MRPDefaultRoleInstance0, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", 
                                          pDDB->SWITCH.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
                    EDDI_Excp("EDDI_CheckDSBMRPUsed, MRPSupportedRole does not match MRPDefaultRoleInstance0, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X",EDDI_FATAL_ERR_EXCP, pDDB->SWITCH.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
                    return;
                }
                break;
            }
            case EDD_MRP_ROLE_CLIENT:
            {
                pDDB->SWITCH.MRPDefaultRoleInstance0 = EDD_MRP_ROLE_CLIENT;
                if (!(pDDB->SWITCH.MRPSupportedRole & EDD_MRP_ROLE_CAP_CLIENT))
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MRPSupportedRole does not match MRPDefaultRoleInstance0, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", 
                                          pDDB->SWITCH.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
                    EDDI_Excp("EDDI_CheckDSBMRPUsed, MRPSupportedRole does not match MRPDefaultRoleInstance0, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X",EDDI_FATAL_ERR_EXCP, pDDB->SWITCH.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
                    return;
                }
                break;
            }
            case EDD_MRP_ROLE_AUTOMANAGER:
            {
                pDDB->SWITCH.MRPDefaultRoleInstance0 = EDD_MRP_ROLE_AUTOMANAGER;
                if (!(pDDB->SWITCH.MRPSupportedRole & EDD_MRP_ROLE_CAP_AUTOMANAGER))
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MRPSupportedRole does not match MRPDefaultRoleInstance0, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", 
                              pDDB->SWITCH.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
                    EDDI_Excp("EDDI_CheckDSBMRPUsed, MRPSupportedRole does not match MRPDefaultRoleInstance0, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X",EDDI_FATAL_ERR_EXCP, pDDB->SWITCH.MRPSupportedRole, pDDB->SWITCH.MRPDefaultRoleInstance0);
                    return;
                }
                break;
            }
            case 0:
            {
                pDDB->SWITCH.MRPDefaultRoleInstance0 = 0;
                break;
            }
            default:
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, wrong role for Nr.red.Ports!=0 configured, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", 
                              pDSB->GlobPara.MRPSupportedRole, pDSB->GlobPara.MRPDefaultRoleInstance0);
                EDDI_Excp("EDDI_CheckDSBMRPUsed, wrong role for Nr.red.Ports!=0 configured, MRPSupportedRole:0x%X MRPDefaultRoleInstance0:0x%X", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MRPSupportedRole, pDSB->GlobPara.MRPDefaultRoleInstance0);
                return;
            }
        }
        
        pDDB->SWITCH.MRPDefaultRoleInstance0  = pDSB->GlobPara.MRPDefaultRoleInstance0;  
        pDDB->SWITCH.MRPSupportedMultipleRole = pDSB->GlobPara.MRPSupportedMultipleRole;

        // HSYNC - MRP restrictions  EDDI_HSYNC_ROLE_APPL_SUPPORT == pDDB->HSYNCRole
        if (pDDB->FeatureSupport.ApplicationExist  &&  pDDB->FeatureSupport.AdditionalForwardingRulesSupported)
        {
            if (   (1                 !=  pDSB->GlobPara.MaxMRP_Instances)
                || (EDD_MRP_ROLE_NONE ==  pDDB->SWITCH.MRPSupportedRole)
                || (2                 !=  MRPDefaultRingPortCnt)
                || (PortMapCnt        != (MRPDefaultRingPortCnt + MRPRoleNonePortCnt)))
            {
                EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                           "EDDI_CheckDSBMRPUsed, HSYNC_ROLE_APPL_SUPPORT is configured, but MaxMRP_Instances(0x%X)!=1  OR  MRPSupportedRole(0x%X)==0  OR  #MRP_RING_PORT_DEFAULT(0x%X)!=2  OR  #MRP_RING_PORT(0x%X)!=0", 
                            pDSB->GlobPara.MaxMRP_Instances, pDDB->SWITCH.MRPSupportedRole, MRPDefaultRingPortCnt, PortMapCnt - MRPDefaultRingPortCnt - MRPRoleNonePortCnt);
                EDDI_Excp("EDDI_CheckDSBMRPUsed, HSYNC_ROLE_APPL_SUPPORT is configured, but MaxMRP_Instances(0x%X)!=1  OR  MRPSupportedRole(0x%X)==0  OR  #MRP_RING_PORT_DEFAULT(?)!=2  OR  #MRP_RING_PORT(?)!=0",
                           EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MaxMRP_Instances, pDDB->SWITCH.MRPSupportedRole);
                return;
            }
        }
    }

    /* Checks for MRP interconnect */
    if (   (pDSB->GlobPara.MaxMRPInterconn_Instances > EDD_CFG_MAX_MRP_IN_INSTANCE_CNT)
        || (pDSB->GlobPara.MaxMRPInterconn_Instances > pDSB->GlobPara.MaxMRP_Instances) )
    {
        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MaxMRPInterconn_Instances is not valid, MaxMRPInterconn_Instances:0x%X, MaxMRP_Instances:0x%X, EDD_CFG_MAX_MRP_IN_INSTANCE_CNT:0x%X", 
            pDSB->GlobPara.MaxMRPInterconn_Instances, pDSB->GlobPara.MaxMRP_Instances, EDD_CFG_MAX_MRP_IN_INSTANCE_CNT);
        EDDI_Excp("EDDI_CheckDSBMRPUsed, MaxMRPInterconn_Instances is not valid, MaxMRPInterconn_Instances, MaxMRP_Instances", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MaxMRPInterconn_Instances, pDSB->GlobPara.MaxMRP_Instances);
        return;
    }
    else if (   (   (0 == pDSB->GlobPara.MaxMRPInterconn_Instances)
                 || (PortMapCnt < 2) )
             && (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported) )
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, MaxMRPInterconn_Instances=%d, PortMapCnt=%d but MRPInterconnFwdRulesSupported is ENABLED",
            pDSB->GlobPara.MaxMRPInterconn_Instances, PortMapCnt);
        EDDI_Excp("EDDI_CheckDSBMRPUsed, EDDI_CheckDSBMRPUsed, MaxMRPInterconn_Instances / PortMapCnt but MRPInterconnFwdRulesSupported is ENABLEd", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.MaxMRPInterconn_Instances, PortMapCnt);
        return;
    }

    if (EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_NONE != pDSB->GlobPara.SupportedMRPInterconnRole)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CheckDSBMRPUsed, SupportedMRPInterconnRole(0x%X): only NONE is allowed.", 
            pDSB->GlobPara.SupportedMRPInterconnRole);
        EDDI_Excp("EDDI_CheckDSBMRPUsed, EDDI_CheckDSBMRPUsed, SupportedMRPInterconnRole only NONE is allowed.", EDDI_FATAL_ERR_EXCP, pDSB->GlobPara.SupportedMRPInterconnRole, 0);
        return;
    }
    pDDB->SWITCH.MaxMRPInterconn_Instances  = pDSB->GlobPara.MaxMRPInterconn_Instances;
    pDDB->SWITCH.SupportedMRPInterconnRole  = pDSB->GlobPara.SupportedMRPInterconnRole;

}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_KramRes()                                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_KramRes( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                         Ctr;
    EDDI_PROF_KRAM_RES_TYPE  *  const  pKramRes = &pDDB->KramRes;
    LSA_UINT32                      *  p;
    LSA_UINT32                         Sum = 0;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_KramRes->");

    p = &pKramRes->UsedBytes;
    for (Ctr = 0; Ctr < (sizeof(EDDI_PROF_KRAM_RES_TYPE) / sizeof(LSA_UINT32)); Ctr++)
    {
        Sum += *p++;  
    }
    pKramRes->UsedBytes = Sum;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckDSB()                                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CheckDSB( EDDI_UPPER_DSB_PTR_TYPE  const  pDSB,
                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortIndex;
    LSA_INT32   Index, CntMACNull;
    LSA_RESULT  Status;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CheckDSB->");

    // MACAddressSrc shall not be MC Mac!
	if (EDDI_IS_MC_MAC(pDSB->GlobPara.xRT.MACAddressSrc.MacAdr))
	{
        EDDI_Excp("EDDI_CheckDSB, MACAddressSrc is MC Mac", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
	}

    //pDSB -> no port mapping -> user view

    //check SpanningTreeState and port params
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32                const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        EDDI_PORT_PARAMS_TYPE  *  const  pPortParams = &pDSB->GlobPara.PortParams[UsrPortIndex];

        //do not map UsrPortIndex -> user view

        //not allowed!
        if (pPortParams->PhyAdr == EDDI_PhyAdr_NOT_CONNECTED)
        {
            EDDI_Excp("EDDI_CheckDSB, PortParams.PhyAdr not defined", EDDI_FATAL_ERR_EXCP, pPortParams->PhyAdr, UsrPortIndex);
            return;
        }

        if (EDD_MEDIATYPE_UNKNOWN != pDDB->Glob.PortParams[HwPortIndex].MediaType)
        {
            Status = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRACheckSpeedModeCapability(pDDB, HwPortIndex, &(pDDB->Glob.PortParams[HwPortIndex].AutonegCapAdvertised));
            if (EDD_STS_OK != Status)
            {
                EDDI_Excp("EDDI_CheckDSB, call via pTRACheckSpeedModeCapability", EDDI_FATAL_ERR_EXCP, Status, 0);
                return;
            }

            Status = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRAAutonegMappingCapability(pDDB, HwPortIndex, &(pDDB->Glob.PortParams[HwPortIndex].AutonegMappingCapability));
            if (EDD_STS_OK != Status)
            {
                EDDI_Excp("EDDI_CheckDSB, call via pTRAAutonegMappingCapability", EDDI_FATAL_ERR_EXCP, Status, 0);
                return;
            }
        }

        CntMACNull = 0;
        for (Index = 0; Index < EDD_MAC_ADDR_SIZE; Index++)
        {
            if (pPortParams->MACAddress.MacAdr[Index] == 0)
            {
                CntMACNull++;
            }
        }

        if (EDD_MAC_ADDR_SIZE == CntMACNull)
        {
            EDDI_Excp("EDDI_CheckDSB, all PortParams.MACAddress are 0", EDDI_FATAL_ERR_EXCP, UsrPortIndex, 0);
            return;
        }

        // pPortParams->MACAddress shall not be MC Mac!
        if (EDDI_IS_MC_MAC(pPortParams->MACAddress.MacAdr))
        {
            EDDI_Excp("EDDI_CheckDSB, Port MACAddress is MC Mac", EDDI_FATAL_ERR_EXCP, UsrPortIndex, 0);
            return;
        }

        if (   (   (EDD_PORT_IS_NOT_WIRELESS != pPortParams->IsWireless)
                && (EDD_PORT_IS_WIRELESS != pPortParams->IsWireless))
            || (   (EDD_PORT_MDIX_DISABLED != pPortParams->IsMDIX)
                && (EDD_PORT_MDIX_ENABLED != pPortParams->IsMDIX)) )
        {
            EDDI_Excp("EDDI_CheckDSB, illegal value for IsWireLess or IsMDIX", EDDI_FATAL_ERR_EXCP, pPortParams->IsWireless, pPortParams->IsMDIX);
            return;
        }
    }

    /*======= EDDI_SWI_PARA_TYPE =======*/

#if defined (EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    if (   (   (EDD_FEATURE_DISABLE != pDSB->SWIPara.bResetIRTCtrl)
            && (EDD_FEATURE_ENABLE != pDSB->SWIPara.bResetIRTCtrl) )
        || (   (EDD_FEATURE_DISABLE != pDSB->SWIPara.Multicast_Bridge_IEEE802RSTP_Forward )
            && (EDD_FEATURE_ENABLE != pDSB->SWIPara.Multicast_Bridge_IEEE802RSTP_Forward ) )
        )
    {
        EDDI_Excp("EDDI_CheckDSB, illegal values for bResetIRTCtrl an/or Multicast_Bridge_IEEE802RSTP_Forward", EDDI_FATAL_ERR_EXCP, pDSB->SWIPara.bResetIRTCtrl, pDSB->SWIPara.Multicast_Bridge_IEEE802RSTP_Forward);
        return;
    }
#endif

    //check whether FDB table is sufficient for remaining entries
    //PNIO-addresses + addresses from system adaption + own MAC-address
    if ((pDSB->SWIPara.Sys_StatFDB_CntEntry) && (LSA_HOST_PTR_ARE_EQUAL(pDSB->SWIPara.pSys_StatFDB_CntEntry, LSA_NULL)))
    {
        EDDI_Excp("EDDI_CheckDSB, LSA_NULL == pDSB->SWIPara.pSys_StatFDB_CntEntry", EDDI_FATAL_ERR_EXCP, pDSB->SWIPara.Sys_StatFDB_CntEntry, 0);
        return;
    }

    //check HW-Bug in REV5
    if (pDDB->ERTEC_Version.MacroRevision == 0x05)
    {
        LSA_UINT32  const  Needed_DB_FCW = (pDDB->PM.PortMap.PortCnt + 2) * (3 * EDDI_MAX_DB_PER_FRAME);  //3 telegrams per port + CHA + CHB

        if (Needed_DB_FCW > pDSB->NRTPara.NRT_DB_Count)
        {
            EDDI_Excp("EDDI_CheckDSB, REV5: Needed_DB_FCW > pDSB->ExpertNRTExtPara.NRT_DB_Count", EDDI_FATAL_ERR_EXCP, Needed_DB_FCW, pDSB->NRTPara.NRT_DB_Count);
            return;
        }

        if (pDSB->NRTPara.NRT_FCW_Limit_Down < EDDI_RES_FCW_LIMIT_DOWN)
        {
            EDDI_Excp("EDDI_CheckDSB, REV5: pDSB->ExpertNRTExtPara.NRT_FCW_Limit_Down < EDDI_RES_FCW_LIMIT_DOWN", EDDI_FATAL_ERR_EXCP, pDSB->NRTPara.NRT_FCW_Limit_Down, EDDI_RES_FCW_LIMIT_DOWN);
            return;
        }
        
        if (pDSB->NRTPara.NRT_DB_Limit_Down < EDDI_RES_DB_LIMIT_DOWN)
        {
            EDDI_Excp("EDDI_CheckDSB, REV5: pDSB->ExpertNRTExtPara.NRT_DB_Limit_Down < EDDI_RES_DB_LIMIT_DOWN", EDDI_FATAL_ERR_EXCP, pDSB->NRTPara.NRT_DB_Limit_Down, EDDI_RES_DB_LIMIT_DOWN);
            return;
        }
    } //end of "check HW-Bug in REV5"

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    //MC-FDB-table available?
    if ((pDDB->pConstValues->MC_MaxEntriesPerSubTable) && (pDDB->pConstValues->MC_MaxSubTable > 0))
    {
        LSA_UINT32  CntEntry = EDDI_SwiPNPNToFDBGetSize() + pDSB->SWIPara.Sys_StatFDB_CntEntry + 1;

        //range 1..16
        if (pDDB->pConstValues->MC_MaxEntriesPerSubTable > g_MC_Table_Length_MAC)
        {
            EDDI_Excp("EDDI_CheckDSB, MC_MaxEntriesPerSubTable > g_MC_Table_Length_MAC", EDDI_FATAL_ERR_EXCP, pDDB->pConstValues->MC_MaxEntriesPerSubTable, g_MC_Table_Length_MAC);
            return;
        }

        //range 1..256
        if (pDDB->pConstValues->MC_MaxSubTable > g_MC_MAX_Table_Number)
        {
            EDDI_Excp("EDDI_CheckDSB, MC_MaxSubTable > g_MC_MAX_Table_Number", EDDI_FATAL_ERR_EXCP, pDDB->pConstValues->MC_MaxSubTable, g_MC_MAX_Table_Number);
            return;
        }

        //check low order range
        if (!EDDI_RedIsBinaryValue((LSA_UINT32)pDDB->pConstValues->MC_MaxSubTable))
        {
            EDDI_Excp("EDDI_CheckDSB, MC_Table_MaxEntry: no power of 2", EDDI_FATAL_ERR_EXCP, pDDB->pConstValues->MC_MaxSubTable, 0);
            return;
        }

        //check whether FDB table is sufficient for remaining entries
        //PNIO-addresses
        CntEntry = EDDI_SwiPNPNToFDBGetSize();

        //check whether FDB table is sufficient for remaining entries, but cannot be calculated exactly -> LFSR
        if (((LSA_UINT32)(pDDB->pConstValues->MC_MaxEntriesPerSubTable * pDDB->pConstValues->MC_MaxSubTable)) < CntEntry)
        {
            EDDI_Excp("EDDI_CheckDSB, pDDB->pConstValues->UCMCTableMaxEntry < CntEntry", EDDI_FATAL_ERR_EXCP, pDDB->pConstValues->UCMCTableMaxEntry, CntEntry);
            return;
        }
    }
    #endif //EDDI_CFG_ENABLE_MC_FDB

    //resources
    if (pDSB->NRTPara.NRT_DB_Count < (pDDB->PM.PortMap.PortCnt * (EDDI_MAX_DB_PER_FRAME + 1)))
    {
        EDDI_Excp("EDDI_CheckDSB, pDSB->ExpertNRTExtPara.NRT_DB_Count", EDDI_FATAL_ERR_EXCP, pDSB->NRTPara.NRT_DB_Count, 0);
        return;
    }

    //resources
    if (pDSB->NRTPara.NRT_FCW_Count < (pDDB->PM.PortMap.PortCnt * (EDDI_MAX_DB_PER_FRAME + 1)))
    {
        EDDI_Excp("EDDI_CheckDSB, pDSB->ExpertNRTExtPara.NRT_FCW_Count", EDDI_FATAL_ERR_EXCP, pDSB->NRTPara.NRT_FCW_Count, 0);
        return;
    }

    //threshold for LowWaterMark is already checked above

    if (pDSB->NRTPara.HOL_Limit_Port_Up < (EDDI_MAX_DB_PER_FRAME + 1))
    {
        EDDI_Excp("EDDI_CheckDSB, pDSB->ExpertNRTExtPara.HOL_Limit_Port_Up", EDDI_FATAL_ERR_EXCP, pDSB->NRTPara.HOL_Limit_Port_Up, 0);
        return;
    }

    if (pDSB->NRTPara.HOL_Limit_CH_Up < (EDDI_MAX_DB_PER_FRAME + 1))
    {
        EDDI_Excp("EDDI_CheckDSB, pDSB->ExpertNRTExtPara.HOL_Limit_CH_Up", EDDI_FATAL_ERR_EXCP, pDSB->NRTPara.HOL_Limit_CH_Up, 0);
        return;
    }


}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_DeviceClose()                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE    Service: EDDI_SRV_DEV_CLOSE                            +*/
/*+   LSA_RESULT    Status:     Return status                               +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_DDB_REL_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDevice  : Valid Device-Handle from device-open                     +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE    Service:                                               +*/
/*+   LSA_RESULT    Status: EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDD_STS_ERR_SEQUENCE                            +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                         or others                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to close device. This request finishes all        +*/
/*+               device-handling by terminating all pending requests and   +*/
/*+               shut down the device.                                     +*/
/*+                                                                         +*/
/*+               If handles still open, we return EDD_STS_ERR_SEQUENCE.    +*/
/*+                                                                         +*/
/*+               When closing the device, the systemadaption have to make  +*/
/*+               sure, that no more interrupts come in for this device.    +*/
/*+               (i.e. with this DDB)                                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_TIMEOUT is set if a timeout on hardwareaccess +*/
/*+               occured, so the hardware couldn't shut down, but the      +*/
/*+               device managment was closed.                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceClose( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT                          Status;
    EDDI_UPPER_DDB_REL_PTR_TYPE  const  pRqbDDBRel = (EDDI_UPPER_DDB_REL_PTR_TYPE)pRQB->pParam;
    EDDI_LOCAL_DDB_PTR_TYPE             pDDB;

    Status = EDDI_GetDDB(pRqbDDBRel->hDDB, &pDDB);
    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, Status);
        if (!(0 == pRqbDDBRel->Cbf))
        {
            pRqbDDBRel->Cbf(pRQB); //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
        }
        return;
    }

    EDDI_SYSTEM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeviceClose->DevNr:%d pCBF:0x%X", pDDB->DeviceNr, pRqbDDBRel->Cbf);

    if (pDDB->Glob.pCloseDevicePendingRQB)
    {
        //User is polling (no cbf)

        if (pRQB != pDDB->Glob.pCloseDevicePendingRQB)
        {
            //This is forbidden! The user must always use the same RQB for polling!
            //Otherwise pending timers will use a nonallocated RQB!
            EDDI_Excp("EDDI_DeviceClose, error in service polling!", EDDI_FATAL_ERR_EXCP, pRQB, pDDB->Glob.pCloseDevicePendingRQB);
            return;
        }

        //User is polling (no cbf): continue directly with part2
        EDDI_DeviceClosePart2(pDDB, pRQB);
        return;
    }

    if (pDDB->Glob.OpenCount)
    {
        EDDI_SYSTEM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DeviceClose->DevNr:%d pCBF:0x%X Still %d OPEN channels!", pDDB->DeviceNr, pRqbDDBRel->Cbf, pDDB->Glob.OpenCount);

        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_SEQUENCE);
        if (!(0 == pRqbDDBRel->Cbf))
        {
            pRqbDDBRel->Cbf(pRQB); //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
        }
        return;
    }

    if (pDDB->Glob.HWIsSetup)
    {
        EDDI_PROFStop(pDDB);

        EDDI_SIIStopInterrupts(pDDB);

        EDDI_SERReset(pDDB, LSA_FALSE /*bCalledForOpen*/);

        pDDB->Glob.HWIsSetup = LSA_FALSE;
    }
    else
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_SEQUENCE);
        if (!(0 == pRqbDDBRel->Cbf))
        {
            pRqbDDBRel->Cbf(pRQB); //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
        }
        return;
    }

    #if !defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)
    if (!(pDDB->ProcessImage.pKRAMMirror == EDDI_NULL_PTR))
    {
        LSA_UINT16  ret_val;

        EDDI_FREE_LOCAL_MEM(&ret_val, pDDB->ProcessImage.pKRAMMirror);
        if (ret_val != LSA_RET_OK)
        {
            EDDI_Excp("EDDI_DeviceClose, Error in freeing pDDB->ProcessImage.pKRAMMirror", EDDI_FATAL_ERR_EXCP, ret_val, 0);
            return;
        }
    }
    #endif //EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK

    EDDI_RtoRelComponent(pDDB);
    EDDI_TraceRelComponent(pDDB);
    EDDI_SRTRelComponent(pDDB);
    EDDI_CRTRelComponent(pDDB);
    EDDI_SwiUsrRelComponent(pDDB);

    #if defined (EDDI_CFG_DFP_ON)
    //Deallocate PF and SF memory
    EDDI_DFPRelComponent(pDDB);
    #endif

    EDDI_SYNCRelComponent(pDDB);
    EDDI_NRTRelComponent(pDDB);

    Status = EDDI_GenDelAllDynMCMACTab(pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_DeviceClose, Error in EDDI_GenDelAllDynMCMACTab", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    EDDI_UndoDeviceSetupSER(pDDB);

    EDDI_MEMClose(pDDB->KRamMemHandle);

    //immediately call part 2
    EDDI_DeviceClosePart2(pDDB, pRQB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_DeviceClosePart2()                     +*/
/*+  Input/Output          :                                                +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishes all timers and closes the DDB if all internal    +*/
/*+               RQBs have been stopped.                                   +*/
/*+                                                                         +*/
/*+               If internal RQBs are still pending and no cbf is          +*/
/*+               supplied, EDD_STS_OK_PENDING is returned.                 +*/
/*+               The user then has to call DEV_CLOSE again!                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeviceClosePart2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      EDD_UPPER_RQB_PTR_TYPE   const  pRQB )
{
    LSA_UINT32                          Ctr;
    LSA_BOOL                            bPendingRQBs = LSA_FALSE;
    EDDI_UPPER_DDB_REL_PTR_TYPE  const  pRqbDDBRel   = (EDDI_UPPER_DDB_REL_PTR_TYPE)pRQB->pParam;

    EDDI_SYSTEM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeviceClosePart2->");

    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_DEVICE_CLOSE_P2);

    //check whether all timer-RQBs for this device are stopped
    for (Ctr = 0; Ctr < (LSA_UINT32)EDDI_CFG_MAX_TIMER; Ctr++)
    {
        EDDI_TIMER_TYPE  *  const  pTimer = &g_pEDDI_Info ->Timer[Ctr];

        if (   (pTimer->used)
            && (pTimer->ShedObj.bUsed)
            && ((LSA_UINT32)pTimer->pDDB == (LSA_UINT32)pDDB))
        {
            bPendingRQBs                      = LSA_TRUE;
            pDDB->Glob.pCloseDevicePendingRQB = pRQB;
            EDDI_SYSTEM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeviceClosePart2->Still waiting for timer RQBs");
            break; //leave for-loop
        }
    }

    //check whether all SII-RQBs are stopped
    for (Ctr = 0; Ctr < (LSA_UINT32)EDDI_SII_INT_GROUP_CNT; Ctr++)
    {
        if (EDDI_SII_RQB_FREE != pDDB->SII.INTGroup[Ctr].IntRQB.internal_context)
        {
            bPendingRQBs                      = LSA_TRUE;
            pDDB->Glob.pCloseDevicePendingRQB = pRQB;
            EDDI_SYSTEM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeviceClosePart2->Still waiting for SII RQBs");
            break; //leave for-loop
        }
    }

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_DEVICE_CLOSE_P2);

    if (bPendingRQBs)
    {
        //we have to wait until all internal static RQBs are stopped!

        if (0 == pRqbDDBRel->Cbf)
        {
            //direct confirmation = User is polling (no cbf)
            EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK_PENDING);
        }
        return;
    }

    EDDI_SIIStopInterrupts(pDDB);

    //close all timers
    EDDI_TimerInfoFree(pDDB);

    EDDI_SYSTEM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeviceClosePart2, finished with positive confirmation, DevNr:%d", pDDB->DeviceNr);

    EDDI_CloseDDB(pRqbDDBRel->hDDB);

    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);

    if (!(0 == pRqbDDBRel->Cbf))
    {
        pRqbDDBRel->Cbf(pRQB); //call of EDDI_REQUEST_UPPER_DONE not possible here, no sys_ptr!
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    eddi_system()                               +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:     Request-dependend                       +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE    Service: EDDI_SRV_DEV_xxxx                             +*/
/*+   LSA_RESULT     Status:  Return status                                 +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+     valid services: all EDDI_SRV_DEV_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values in Requestblock                                      +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:     unchanged                               +*/
/*+   EDD_SERVICE    Service:    unchanged                                  +*/
/*+   LSA_RESULT    Status:  EDD_STS_OK                                     +*/
/*+                          EDD_STS_ERR_PARAM                              +*/
/*+                          EDD_STS_ERR_SERVICE                            +*/
/*+                          EDD_STS_ERR_OPCODE                             +*/
/*+                          others                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles devicerequest which will be used in systemadaption+*/
/*+               This functions have a RQB-Parameter structure.            +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDI_RQB_ERROR with the RQB +*/
/*+               to notify this error. This errors are most likely caused  +*/
/*+               by an implementation error and are of the type            +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_system( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    EDDI_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eddi_system->");

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we can't use the call-back-function      */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if (EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_OPCODE);
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    if (g_pEDDI_Info ->Excp.Error != EDDI_FATAL_NO_ERROR)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_SEQUENCE);
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    EDDI_EnterIntern();

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        /*-----------------------------------------------------------------------*/
        /* Device open/close/setup                                               */
        /*-----------------------------------------------------------------------*/
        case EDDI_SRV_DEV_OPEN:
            EDDI_DeviceOpen(pRQB);
            break;
        case EDDI_SRV_DEV_SETUP:
            EDDI_DeviceSetup(pRQB);
            break;
        case EDDI_SRV_DEV_CLOSE:
            EDDI_DeviceClose(pRQB);
            break;
        /*-----------------------------------------------------------------------*/
        /* BASIC-Component                                                       */
        /*-----------------------------------------------------------------------*/
        case EDDI_SRV_DEV_COMP_INI:
        {
           EDDI_DeviceCompIni(pRQB);
           break;
        }
        /*-----------------------------------------------------------------------*/
        default:
        {
            EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_SERVICE);
            EDDI_RQB_ERROR(pRQB);
        }
    }

    EDDI_ExitIntern();
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_usr.c                                                   */
/*****************************************************************************/

