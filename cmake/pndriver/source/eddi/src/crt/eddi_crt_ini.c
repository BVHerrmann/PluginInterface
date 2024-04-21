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
/*  F i l e               &F: eddi_crt_ini.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* SRT (soft real time) for EDDI.                                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_crt_ext.h"
#include "eddi_crt_phase_ext.h"

#include "eddi_ser_ext.h"
#include "eddi_ser_cmd.h"

#include "eddi_pool.h"

#include "eddi_crt_ini.h"
#include "eddi_crt_check.h"

#include "eddi_irt_tree.h"
//#include "eddi_crt_dfp.h"

#define EDDI_MODULE_ID     M_ID_CRT_INI
#define LTRC_ACT_MODUL_ID  104

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTInitPools( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CRTCheckPara( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp,
                                                         EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTInitProcessImage( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp,
                                                              EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CheckFrameIDRange( LSA_UINT8   const  RtClass,
                                                            LSA_UINT16  const  FrameIDStart,
                                                            LSA_UINT16  const  FrameIDCount );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTClosePools( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                        EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTReserveKramPools( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp,
                                                              EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB );


/*---------------------------------------------------------------------------*/
/* Ini/Rel-CRTComponent                                                      */
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_CRTInitComponent()                     +*/
/*+  Input/Output          :    ...                                         +*/
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
/*+   EDD_SERVICE       Service: EDDI_SRV_COMP_SRT_INI                      +*/
/*+   LSA_RESULT        Status:  Return status                              +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_CMP_SRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE       Service: EDDI_SRV_COMP_SRT_INI                      +*/
/*+   LSA_RESULT        Status:  EDD_STS_OK                                 +*/
/*+                              EDD_STS_ERR_SEQUENCE                       +*/
/*+                              EDDI_STS_ERR_RESOURCES                     +*/
/*+                              EDD_STS_ERR_PARAM                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the SRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTInitComponent( EDDI_CRT_INI_PTR_TYPE    const  pRqbCMP,
                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp = &pDDB->CRT;
    LSA_UINT32                               UsrPortIndex;
    LSA_RESULT                               Status;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTInitComponent");

    if (!(pDDB->pLocal_CRT == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_CRTInitComponent",  EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    pCRTComp->SERIniRcwFct = EDDI_SERIniRcwSetup;
    #endif

    //ini CRTComp,Producer-List and Consumer-List

    /************************************************************************/
    /* ini CRTComp                                                          */
    /************************************************************************/

    pCRTComp->CfgPara = *pRqbCMP; //copy all Ini - Parameters.
    pCRTComp->SyncImage.ImageExpert.TriggerDeadlineIn10ns = 0;
    pCRTComp->SyncImage.ImageExpert.TriggerMode           = 0;

    Status = EDDI_CRTCheckPara(pCRTComp, pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_CRTInitComponent EDDI_CRTCheckPara",  EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    //Init Meta-Info
    pCRTComp->MetaInfo.AcwProviderCnt = pCRTComp->CfgPara.SRT.ProviderCnt;
    pCRTComp->MetaInfo.FcwProviderCnt = pCRTComp->CfgPara.IRT.ProviderCnt;
    pCRTComp->MetaInfo.AcwConsumerCnt = pCRTComp->CfgPara.SRT.ConsumerCntClass12;
    pCRTComp->MetaInfo.FcwConsumerCnt = pCRTComp->CfgPara.IRT.ConsumerCntClass3;

    //RTC1/2 and RTC3 consumers are kept together to limit the SB size (small SCF)
    //RTC1/2 and RTC3 providers are kept separate to ease allocation
    pCRTComp->MetaInfo.RTC123IDStart    = 0;  
    pCRTComp->MetaInfo.RTC123IDEnd      = pCRTComp->MetaInfo.RTC123IDStart + pCRTComp->MetaInfo.FcwConsumerCnt + pCRTComp->MetaInfo.AcwConsumerCnt;
    pCRTComp->MetaInfo.RTC3IDStartProv  = 0;
    pCRTComp->MetaInfo.RTC3IDEndProv    = pCRTComp->MetaInfo.FcwProviderCnt;
    pCRTComp->MetaInfo.RTC12IDStartProv = pCRTComp->MetaInfo.RTC3IDEndProv;
    pCRTComp->MetaInfo.RTC12IDEndProv   = pCRTComp->MetaInfo.RTC12IDStartProv + pCRTComp->MetaInfo.AcwProviderCnt;
    #if defined (EDDI_CFG_DFP_ON)
    pCRTComp->MetaInfo.DFPIDStart       = pCRTComp->MetaInfo.RTC123IDEnd;
	pCRTComp->MetaInfo.DFPIDEnd         = pCRTComp->MetaInfo.DFPIDStart + pCRTComp->MetaInfo.FcwConsumerCnt;
    pCRTComp->MetaInfo.DFPIDStartProv   = pCRTComp->MetaInfo.RTC12IDEndProv;
	pCRTComp->MetaInfo.DFPIDEndProv     = pCRTComp->MetaInfo.DFPIDStartProv + pCRTComp->MetaInfo.FcwProviderCnt;
    #endif

    EDDI_CRTReserveKramPools(pCRTComp, pDDB);

    EDDI_CRTInitProcessImage(pCRTComp, pDDB);

    //Init IndicationLIst
    //schon 0  pCRTComp->FreeIndicationQueue.Count  = 0;
    //schon 0  pCRTComp->FreeIndicationQueue.pFirst = EDDI_NULL_PTR;
    //schon 0  pCRTComp->FreeIndicationQueue.pLast  = EDDI_NULL_PTR;

    // Init Remote-Provider-Surveillance
    EDDI_CRTRpsInit(&pCRTComp->Rps, pDDB);

    EDDI_CRTCreateProviderList(pDDB, &pCRTComp->ProviderList,
                               pCRTComp->MetaInfo.AcwProviderCnt,
                               pCRTComp->MetaInfo.FcwProviderCnt,
                               EDD_CFG_CSRT_MAX_PROVIDER_GROUP);
    
    pCRTComp->ProviderList.MaxUsedReduction = 0;

    EDDI_CRTCreateConsumerList(pDDB, &pCRTComp->ConsumerList,
                               pCRTComp->MetaInfo.AcwConsumerCnt,
                               pCRTComp->MetaInfo.FcwConsumerCnt);
    EDDI_CRT_CONSTRACE_ALLOC_TABLE(pDDB);

    EDDI_CrtPhaseClass2Init(&pCRTComp->RTClass2Interval, pDDB);

    EDDI_RedTreeInit(pDDB, &pCRTComp->SrtRedTree,
                     EDDI_TREE_RT_CLASS_ACW_TX,
                     pCRTComp->CycleBaseFactor,
                     EDDI_TREE_MAX_BINARY_REDUCTION,
                     pCRTComp->MetaInfo.AcwProviderCnt);

    //reservieren, da erst später allokiert wird
    pDDB->CRT.KRAMSize_xRT_Trees += pCRTComp->SrtRedTree.KramSize;

    //Reset Global RTClass2_Bandwidth
    pCRTComp->RTClass2Interval.Status = EDD_RTCLASS2_NOT_SUPPORTED;
    //schon 0  pCRTComp->RTClass2Interval.ReservedIntervalBegin = 0;
    //schon 0  pCRTComp->RTClass2Interval.ReservedIntervalBegin = 0;

    pDDB->pLocal_CRT = pCRTComp;

    #if defined (EDDI_CFG_USE_SW_RPS)
    EDDI_TreeIni(EDDI_TREE_MAX_BINARY_REDUCTION, &pCRTComp->pSBTree);
    #endif


    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        EDDI_CrtPhaseTxInit(pDDB, &pCRTComp->PhaseTx[HwPortIndex], HwPortIndex);
        EDDI_CrtPhaseRxInit(pDDB, &pCRTComp->PhaseRx[HwPortIndex], HwPortIndex);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTEDDI_RedTreeInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp  = pDDB->pLocal_CRT;
    LSA_UINT32                        const  StartMask = 0x7FFFUL;

    if (!pCRTComp)
    {
        return;
    }

    EDDI_CRTInitPools(pDDB);

    if (pCRTComp->MetaInfo.AcwProviderCnt)
    {
        EDDI_LOCAL_EOL_PTR_TYPE  pEOLn[1];

        pEOLn[0] = pCRTComp->RTClass2Interval.pEOL;
        EDDI_RedTreeBuild(pDDB,
                          &pCRTComp->SrtRedTree,
                          pCRTComp->CycleBaseFactor,
                          (EDDI_SER_CCW_PTR_TYPE)(void *)pCRTComp->RTClass2Interval.pRootAcw,
                          &pEOLn[0]);

        EDDI_SERSingleDirectCmd(FCODE_ENABLE_ACW_TX_LIST, StartMask, 0, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTInitProcessImage()                       */
/*                                                                         */
/* D e s c r i p t i o n: Initializes KRAM/HOST-Memory, Pointers,          */
/*                        DMA-Registers etc needed for                     */
/*                        SYNC- and ASYNC- IN/OUT-Images                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTInitProcessImage( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    // Set ImageMode-Register and DMA-Controller-Settings
    EDDI_SERImageDMASetup(pDDB, &pCRTComp->SyncImage);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CheckFrameIDRange()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CheckFrameIDRange( LSA_UINT8   const  RtClass,
                                                               LSA_UINT16  const  FrameIDStart,
                                                               LSA_UINT16  const  FrameIDCount )
{
    LSA_UINT16  const  FrameIDStop = (LSA_UINT16)(FrameIDStart + (FrameIDCount - 1));

    if (FrameIDCount == 0)
    {
        return;
    }

    switch (RtClass)
    {
        case EDDI_RTC1:
        {
            if //FrameIDStart invalid?
               ((FrameIDStart < EDDI_SRT_FRAMEID_RT_CLASS1_START) || (FrameIDStart > EDDI_SRT_FRAMEID_RT_CLASS1_STOP))
            {
                EDDI_Excp("EDDI_CheckFrameIDRange, FrameIDStart is invalid within this RtClass", EDDI_FATAL_ERR_EXCP, RtClass, FrameIDStart);
                return;
            }
            if //FrameIDStop invalid?
               ((FrameIDStop < EDDI_SRT_FRAMEID_RT_CLASS1_START) || (FrameIDStop > EDDI_SRT_FRAMEID_RT_CLASS1_STOP))
            {
                EDDI_Excp("EDDI_CheckFrameIDRange, FrameIDStop is invalid within this RtClass", EDDI_FATAL_ERR_EXCP, RtClass, FrameIDStop);
                return;
            }
            return;
        }
        case EDDI_RTC2:
        {
            if //FrameIDStart invalid?
               ((FrameIDStart < EDDI_SRT_FRAMEID_RT_CLASS2_START) || (FrameIDStart > EDDI_SRT_FRAMEID_RT_CLASS2_STOP))
            {
                EDDI_Excp("EDDI_CheckFrameIDRange, FrameIDStart is invalid within this RtClass", EDDI_FATAL_ERR_EXCP, RtClass, FrameIDStart);
                return;
            }
            if //FrameIDStop invalid?
               ((FrameIDStop < EDDI_SRT_FRAMEID_RT_CLASS2_START) || (FrameIDStop > EDDI_SRT_FRAMEID_RT_CLASS2_STOP))
            {
                EDDI_Excp("EDDI_CheckFrameIDRange, FrameIDStop is invalid within this RtClass", EDDI_FATAL_ERR_EXCP, RtClass, FrameIDStop);
                return;
            }
            return;
        }
        default:
        {
            EDDI_Excp("EDDI_CheckFrameIDRange, Invalid RtClass", EDDI_FATAL_ERR_EXCP, RtClass, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTInitPools( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                                  BufferCount;
    EDDI_KRAM_RES_POOL_TYPE           *  const  pPool    = &(pDDB->KramRes.pool);
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE     const  pCRTComp = &(pDDB->CRT);

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTInitPools");

    pPool->srt_noop = 0;
    pPool->rcw      = 0;
    pPool->bcw_mod  = 0;

    #if defined (EDDI_CFG_REV7)
    if (pCRTComp->PAEA_Para.PAEA_BaseAdr)
    {
        (void)EDDI_MemCalcApduBuffer(pDDB);
        //We setup a Consumer indirecton level (XCHANGE_BUF)?

        (void)EDDI_MemIniApduConsumerIndirectionLayer(pDDB);
    }
    #endif

    //KRAM-MemPool for ACW-Provider
    pCRTComp->hPool.AcwProvider = 0;
    BufferCount                 = pCRTComp->MetaInfo.AcwProviderCnt;
    if (BufferCount)
    {
        BufferCount++;  //Reserve one more for ChangePhase-Function
        pPool->acw_snd = EDDI_MemIniPool(BufferCount,
                                          (LSA_UINT32)sizeof(EDDI_SER10_ACW_SND_TYPE),
                                          EDDI_POOL_TYPE_KRAM_8BA,
                                          EDDI_AllocLocalMem,
                                          EDDI_FREELOCALMEM,
                                          EDDI_MEM_DO_NO_PRESET,
                                          (LSA_UINT8)0,  //Preset Value
                                          pDDB,
                                          &pCRTComp->hPool.AcwProvider);  //OUT
    }

    //KRAM-MemPool for ACW-Consumer
    pCRTComp->hPool.AcwConsumer = 0;
    BufferCount                 = pCRTComp->MetaInfo.AcwConsumerCnt + 2UL; // Reserve 2 more for SyncSlave
    pPool->acw_rcv = EDDI_MemIniPool(BufferCount ,
                                      (LSA_UINT32)sizeof(EDDI_SER10_ACW_RCV_TYPE),
                                      EDDI_POOL_TYPE_KRAM_8BA,
                                      EDDI_AllocLocalMem,
                                      EDDI_FREELOCALMEM,
                                      EDDI_MEM_DO_NO_PRESET,
                                      (LSA_UINT8)0,  //Preset Value
                                      pDDB,
                                      &pCRTComp->hPool.AcwConsumer);  //OUT

    pCRTComp->hPool.CCWCTRL = 0;
    (void)EDDI_MemIniPool(BufferCount,
                           (LSA_UINT32)sizeof(EDDI_CCW_CTRL_TYPE),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_NO_PRESET,
                           (LSA_UINT8)0,  //Preset Value
                           pDDB,
                           &pCRTComp->hPool.CCWCTRL);  //OUT

    //init memory-pool for APDU-buffer for RTC3-AUX-Consumer
    pCRTComp->hPool.ApduAuxConsumer = 0;
    BufferCount                     = pCRTComp->MetaInfo.FcwConsumerCnt;
    if (BufferCount)
    {
        //round up to a 8-byte-aligned total length
        BufferCount = (BufferCount + 1) & 0xFFFFFFFEUL;
        (void)EDDI_MemIniPool(BufferCount,
                               (LSA_UINT32)sizeof(EDDI_CRT_DATA_APDU_STATUS),
                               EDDI_POOL_TYPE_KRAM_4BA,
                               EDDI_AllocLocalMem,
                               EDDI_FREELOCALMEM,
                               EDDI_MEM_DO_NO_PRESET,
                               (LSA_UINT8)0,  //Preset Value
                               pDDB,
                               &pCRTComp->hPool.ApduAuxConsumer);  //OUT
    }

    //init memory pool for IO-indirections for 3B-ProvIF in SW
    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    pCRTComp->hPool.IOCWRef = 0;
    BufferCount             = pCRTComp->MetaInfo.AcwProviderCnt + pCRTComp->MetaInfo.FcwProviderCnt;
    if (BufferCount)
    {
        //round up to a 8-byte-aligned total length
        BufferCount = (BufferCount + 1) & 0xFFFFFFFEUL;
        (void)EDDI_MemIniPool(BufferCount,
                               (LSA_UINT32)sizeof(LSA_UINT32),
                               EDDI_POOL_TYPE_KRAM_4BA,
                               EDDI_AllocLocalMem,
                               EDDI_FREELOCALMEM,
                               EDDI_MEM_DO_PRESET,
                               (LSA_UINT8)3,  //Preset Value
                               pDDB,
                               &pCRTComp->hPool.IOCWRef);  //OUT
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTReserveKramPools( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    LSA_UINT32  BufferCount;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTReserveKramPools");

    // KRAM-MemPool for ACW-Provider
    pCRTComp->hPool.AcwProvider      = 0x0UL;
    BufferCount                      = pCRTComp->MetaInfo.AcwProviderCnt;
    pDDB->CRT.KRAMSize_xRT_Trees    += BufferCount * sizeof(EDDI_SER10_ACW_SND_TYPE);

    // KRAM-MemPool for ACW-Consumer
    pCRTComp->hPool.AcwConsumer      = 0x0UL;
    BufferCount                      = pCRTComp->MetaInfo.AcwConsumerCnt + 2UL; // Reserve 2 more for SyncSlave
    pDDB->CRT.KRAMSize_xRT_Trees    += BufferCount * sizeof(EDDI_SER10_ACW_RCV_TYPE);

    pCRTComp->hPool.ApduAuxConsumer  = 0x0UL;
    BufferCount                      = pCRTComp->MetaInfo.FcwConsumerCnt;
    //round up to a 8-byte-aligned total length
    BufferCount = (BufferCount + 1) & 0xFFFFFFFEUL;
    pDDB->CRT.KRAMSize_xRT_Trees    += BufferCount * sizeof(EDDI_CRT_DATA_APDU_STATUS);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTClosePools()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTClosePools( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                           EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp )
{
    LSA_UNUSED_ARG(pDDB);

    EDDI_MemClosePool(pCRTComp->hPool.CCWCTRL);
    EDDI_MemClosePool(pCRTComp->hPool.AcwConsumer);
    EDDI_MemClosePool(pCRTComp->hPool.AcwProvider);
    EDDI_MemClosePool(pCRTComp->hPool.ApduAuxConsumer);
    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    EDDI_MemClosePool(pCRTComp->hPool.IOCWRef);
    #endif
    #if defined (EDDI_CFG_REV7)
    EDDI_MemCloseApduBuffer(pDDB, pCRTComp);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTCheckPara()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckPara( EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp,
                                                            EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    EDDI_CRT_INI_TYPE  *  const  pCrtPara = &(pCRTComp->CfgPara);

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCheckPara");

    //Check Consumer-FrameID-Range of RT-Class1
    EDDI_CheckFrameIDRange(EDDI_RTC1,
                           pCrtPara->SRT.ConsumerFrameIDBaseClass1,
                           pCrtPara->SRT.ConsumerCntClass12);

    //Check Consumer-FrameID-Range of RT-Class2
    EDDI_CheckFrameIDRange(EDDI_RTC2,
                           pCrtPara->SRT.ConsumerFrameIDBaseClass2,
                           pCrtPara->SRT.ConsumerCntClass12);

    if (   (EDD_FEATURE_DISABLE != pCrtPara->bUseTransferEnd)
        && (EDD_FEATURE_ENABLE != pCrtPara->bUseTransferEnd) )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCheckPara: illegal value for bUseTransferEnd(%d)",
            pCrtPara->bUseTransferEnd);
        return EDD_STS_ERR_PARAM;
    }

    //Check nr of providers against define
    #if (0)
    if ((pCrtPara->SRT.ProviderCnt + pCrtPara->IRT.ProviderCnt) > pIOCB->MaxNrProviders)
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCheckPara: SRT.ProviderCnt(%d) + IRT.ProviderCnt(%d) > EDDI_CFG_MAX_NR_PROVIDERS(%d)",
                          pCrtPara->SRT.ProviderCnt, pCrtPara->IRT.ProviderCnt, EDDI_CFG_MAX_NR_PROVIDERS);
        return EDD_STS_ERR_PARAM;
    }
    #endif //(0)
    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_CRTRelComponent()                      +*/
/*+  Input/Output          :    ...                                         +*/
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
/*+   EDD_SERVICE    Service: EDDI_SRV_COMP_SRT_INI                         +*/
/*+   LSA_RESULT    Status:     Return status                               +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_CMP_SRT_REL_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE       Service:    EDDI_SRV_COMP_NRT_REL                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will release the SRT-Component management   +*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be freed.                                            +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the any channel      +*/
/*+               uses this component and the device is still setup         +*/
/*+               (device-shutdown has to be called first!)                 +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CRTRelComponent");

    if (pDDB->pLocal_CRT == EDDI_NULL_PTR)
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTRelComponent, Pointer to CRT-Struct is already Null");
        return;
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    EDDI_CrtPhaseReset(pDDB);
    #endif

    //Now Start the CleanUp
    EDDI_CRTDeleteProviderList(&pDDB->pLocal_CRT->ProviderList);
    EDDI_CRTDeleteConsumerList(pDDB, &pDDB->pLocal_CRT->ConsumerList);
    EDDI_CRT_CONSTRACE_DEALLOC_TABLE(pDDB);

    if (pDDB->pLocal_CRT->MetaInfo.AcwProviderCnt)
    {
        EDDI_RedTreeClose(pDDB, &pDDB->CRT.SrtRedTree);
    }

    //CrtPhase_Class2DeInit(&pDDB->CRT.RTClass2Interval, pDDB);

    if (!(pDDB->pLocal_SYNC == EDDI_NULL_PTR))
    {
        EDDI_IRTTreeCloseAx(pDDB);
    }

    EDDI_SERStopIrtGlobalAndIrtPorts(pDDB);

    EDDI_CRTClosePools(pDDB, pDDB->pLocal_CRT);
    #if defined (EDDI_CFG_USE_SW_RPS)
    EDDI_TreeFree(pDDB->pLocal_CRT->pSBTree);
    #endif

    pDDB->pLocal_CRT = EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_ini.c                                               */
/*****************************************************************************/

