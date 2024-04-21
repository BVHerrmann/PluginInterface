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
/*  F i l e               &F: eddi_nrt_ini.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI.                    */
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

#include "eddi_nrt_inc.h"
#include "eddi_nrt_ini.h"
//#include "eddi_nrt_tx.h"
#include "eddi_nrt_rx.h"
#include "eddi_kram.h"
#include "eddi_lock.h"
#include "eddi_ser.h"

#if defined (EDDI_CFG_FRAG_ON)
#include "eddi_nrt_frag_tx.h"
#endif

#if defined (EDDI_CFG_REV5)
#include "eddi_time.h"
#include "eddi_isr_nrt.h"
#endif

#define EDDI_MODULE_ID     M_ID_NRT_INI
#define LTRC_ACT_MODUL_ID  203

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_NRTInitTRxIF( NRT_CHX_SSX_PTR_TYPE      const pDscr,
                                                       EDDI_NRT_CHX_SS_IF_TYPE * const pIF,
                                                       LSA_UINT32                const CntDscrIF,
                                                       EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                       LSA_BOOL                  const bAllocDataBuffer,
                                                       EDDI_USERMEMID_TYPE       const UserMemID );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_NrtCheckDeferredAlloc( EDDI_NRT_IF_INI_TYPE    const * const pIni,
                                                                EDDI_NRT_CHX_SS_IF_TYPE       * const pIF );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_NRTInitLimit( EDDI_NRT_IF_INI_TYPE    * const pInterface,
                                                       EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                       EDDI_NRT_CHX_SS_IF_TYPE * const pIF );

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_NrtSetPrioLimit( LSA_UINT32 const LimitBytesPerMilliSecond,
                                                            LSA_UINT32 const CycleLength_us );

static LSA_UINT8 const PrioQueueArrayIndexIFA[NRT_MAX_PRIO_QUEUE] = {NRT_HIGH_PRIO_INDEX, NRT_MEDIUM_PRIO_INDEX, NRT_LOW_PRIO_INDEX};
static LSA_UINT8 const PrioQueueArrayIndexIFB[NRT_MAX_PRIO_QUEUE] = {NRT_MGMT_HIGH_PRIO_INDEX, NRT_HSYNC1_HIGH_PRIO_INDEX, NRT_MGMT_LOW_PRIO_INDEX, NRT_HSYNC1_LOW_PRIO_INDEX };
static LSA_UINT8 const MaxPrioQueueArrayIndexIFA = 2;
static LSA_UINT8 const MaxPrioQueueArrayIndexIFB = 3;

/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRelComponent()                           */
/*                                                                         */
/* D e s c r i p t i o n: function is called by EDDI_DeviceClose()         */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         Index, Cnt;
    NRT_DSCR_PTR_TYPE  pDscr;
    LSA_UINT16         ret_val;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTRelComponent->");

    if (pDDB->pLocal_NRT == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTRelComponent, pDDB->pLocal_NRT == EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //TX
    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[Index];

        if (pIF->Tx.Dscr.Cnt == 0)
        {
            continue;
        }

        if //at least 1 occupied TX-DMACW is existent?
           (pIF->Tx.BalanceSndReq)
        {
            EDDI_Excp("EDDI_NRTRelComponent, error: occupied TX-DMACW existent", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        //TX-FRAG
        #if defined (EDDI_CFG_FRAG_ON)
        {
            EDDI_RQB_QUEUE_TYPE     *  const  pFragDeallocQueue = &pIF->Tx.FragDeallocQueue;
            EDD_UPPER_RQB_PTR_TYPE            pHelpRQB; //here big fragmentation buffers are handled as RQBs

            //check old big fragmentation buffers in Frag Dealloc Queue of this interface
            while ((pHelpRQB = EDDI_RemoveFromQueue(pDDB, pFragDeallocQueue)) != EDDI_NULL_PTR)
            {
                LSA_UINT32  const  PrioIndexHelp = pHelpRQB->internal_context_Prio;

                //deallocate big fragmentation buffer
                EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR)pHelpRQB, pIF->UserMemIDTXFragArray[PrioIndexHelp]);
            }
        }
        #endif //EDDI_CFG_FRAG_ON

        EDDI_FREE_LOCAL_MEM(&ret_val, pIF->Tx.Dscr.pNRTDscrArray);
        if (ret_val != LSA_RET_OK)
        {
            EDDI_Excp("EDDI_NRTRelComponent, error at EDDI_FREE_LOCAL_MEM of TX", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_DEVFreeMode(pDDB, pIF->MemModeDMACW, pIF->Tx.Dscr.pDMACWArray, pIF->UserMemIDDMACWDefault);
    }

    //RX
    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[Index];

        if (pIF->Rx.Dscr.Cnt == 0)
        {
            continue;
        }

        pDscr = pIF->Rx.Dscr.pTop;

        for (Cnt = 0; Cnt < pIF->Rx.Dscr.Cnt; Cnt++)
        {
            if (pIF->StateDeferredAlloc == DEF_ALLOC_DONT_CARE)
            {
                EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, pDscr->pRcvDataBuffer, pIF->UserMemIDRXDefault);
            }

            //set to next descriptor
            pDscr = pDscr->next_dscr_ptr;
        }

        EDDI_FREE_LOCAL_MEM(&ret_val, pIF->Rx.Dscr.pNRTDscrArray);
        if (ret_val != LSA_RET_OK)
        {
            EDDI_Excp("EDDI_NRTRelComponent, error at EDDI_FREE_LOCAL_MEM of RX", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_DEVFreeMode(pDDB, pIF->MemModeDMACW, pIF->Rx.Dscr.pDMACWArray, pIF->UserMemIDDMACWDefault);
    }

    #if defined (EDDI_CFG_DEFRAG_ON)
    //release NRT Frag RX
    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[Index];
        
        EDDI_DEVFreeMode(pDDB, pIF->MemModeBuffer, pDDB->NRT.RxFragData.pNRTDeFragFrame[Index], pIF->UserMemIDRXDefault);
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtCheckDeferredAlloc()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtCheckDeferredAlloc( EDDI_NRT_IF_INI_TYPE     const *  const  pIni,
                                                                   EDDI_NRT_CHX_SS_IF_TYPE        *  const  pIF )
{
    pIF->StateDeferredAlloc = DEF_ALLOC_DONT_CARE;

    if (pIni)
    {}

    return;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitLimit()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitLimit( EDDI_NRT_IF_INI_TYPE     *  const  pInterface,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitLimit->");

//??  - wird es noch gebraucht? sind die globalen Werte schon gesetzt oder sollte man es hier tun ?
    LSA_UNUSED_ARG(pInterface);
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pIF);
#if 0
    pTx->Ini_DBSndLimitPerMilliSecond = pInterface->DB_SndLimitPerMilliSecond;

    if //number of TX-DMACWs > 0?
       (pInterface->TxCntDscr)
    {
        if (pInterface->DB_SndLimitPerMilliSecond.Overall == 0)
        {
            //a NRT-send-limitation is mandatory
            EDDI_Excp("EDDI_NRTInitLimit, pInterface->DB_SndLimitPerMilliSecond.Overall == 0",
                      EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if (pTx->Ini_DBSndLimitPerMilliSecond.Prio_Low > pInterface->DB_SndLimitPerMilliSecond.Overall)
        {
            EDDI_Excp("EDDI_NRTInitLimit, Prio_Low",
                      EDDI_FATAL_ERR_EXCP, pInterface->DB_SndLimitPerMilliSecond.Overall,
                      pTx->Ini_DBSndLimitPerMilliSecond.Prio_Low);
            return;
        }

        if (pTx->Ini_DBSndLimitPerMilliSecond.Prio_Medium > pInterface->DB_SndLimitPerMilliSecond.Overall)
        {
            EDDI_Excp("EDDI_NRTInitLimit, Prio_Medium",
                      EDDI_FATAL_ERR_EXCP, pInterface->DB_SndLimitPerMilliSecond.Overall,
                      pTx->Ini_DBSndLimitPerMilliSecond.Prio_Medium);
            return;
        }

        if (pTx->Ini_DBSndLimitPerMilliSecond.Prio_High > pInterface->DB_SndLimitPerMilliSecond.Overall)
        {
            EDDI_Excp("EDDI_NRTInitLimit, Prio_High",
                      EDDI_FATAL_ERR_EXCP, pInterface->DB_SndLimitPerMilliSecond.Overall,
                      pTx->Ini_DBSndLimitPerMilliSecond.Prio_High);
            return;
        }

        if (pTx->Ini_DBSndLimitPerMilliSecond.Prio_Mgmt_Low > pInterface->DB_SndLimitPerMilliSecond.Overall)
        {
            EDDI_Excp("EDDI_NRTInitLimit, Prio_Mgmt_Low",
                      EDDI_FATAL_ERR_EXCP, pInterface->DB_SndLimitPerMilliSecond.Overall,
                      pTx->Ini_DBSndLimitPerMilliSecond.Prio_Mgmt_Low);
            return;
        }

        if (pTx->Ini_DBSndLimitPerMilliSecond.Prio_Mgmt_High > pInterface->DB_SndLimitPerMilliSecond.Overall)
        {
            EDDI_Excp("EDDI_NRTInitLimit, Prio_Mgmt_High",
                      EDDI_FATAL_ERR_EXCP, pInterface->DB_SndLimitPerMilliSecond.Overall,
                      pTx->Ini_DBSndLimitPerMilliSecond.Prio_Mgmt_High);
            return;
        }
        //pTx->SndLimit.DBsPerCycle = 0; //for EDDI_NRTReloadSendList() and EDDI_NRTReloadSendListTs()!
    }
#endif

    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetMaxIFFrameLen()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetMaxIFFrameLen( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Index, Ctr;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSetMaxIFFrameLen->");

    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[Index];

        for (Ctr = 0; Ctr < NRT_MAX_PRIO_QUEUE; Ctr++)
        {
            pIF->Tx.SndLimit.MaxFrameLen[Ctr] = pDDB->NRT.MaxSndLen;
        }

        if (pDDB->NRT.bFeedInLoadLimitationActive)
        {
            if (pIF->Tx.SndLimit.MaxFrameLen[NRT_MGMT_HIGH_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_MGMT_HIGH_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
            {
                pIF->Tx.SndLimit.MaxFrameLen[NRT_MGMT_HIGH_PRIO_INDEX]   =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_MGMT_HIGH_PRIO_INDEX]   - EDDI_NRT_FRAME_HEADER_SIZE;
            }
            if (pIF->Tx.SndLimit.MaxFrameLen[NRT_MGMT_LOW_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_MGMT_LOW_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
            {
                pIF->Tx.SndLimit.MaxFrameLen[NRT_MGMT_LOW_PRIO_INDEX]    =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_MGMT_LOW_PRIO_INDEX]    - EDDI_NRT_FRAME_HEADER_SIZE;
            }
            if (pIF->Tx.SndLimit.MaxFrameLen[NRT_HIGH_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_HIGH_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
            {
                pIF->Tx.SndLimit.MaxFrameLen[NRT_HIGH_PRIO_INDEX]        =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_HIGH_PRIO_INDEX]        - EDDI_NRT_FRAME_HEADER_SIZE;
            }
            if (pIF->Tx.SndLimit.MaxFrameLen[NRT_MEDIUM_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_MEDIUM_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
            {
                pIF->Tx.SndLimit.MaxFrameLen[NRT_MEDIUM_PRIO_INDEX]      =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_MEDIUM_PRIO_INDEX]      - EDDI_NRT_FRAME_HEADER_SIZE;
            }
            if (pIF->Tx.SndLimit.MaxFrameLen[NRT_LOW_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_LOW_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
            {
                pIF->Tx.SndLimit.MaxFrameLen[NRT_LOW_PRIO_INDEX]         =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_LOW_PRIO_INDEX]         - EDDI_NRT_FRAME_HEADER_SIZE;
            }
        }
        
        //HSYNC is always 3kB/ms per port
        if (pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC1_HIGH_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_HSYNC1_HIGH_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
        {
            pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC1_HIGH_PRIO_INDEX] =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_HSYNC1_HIGH_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE;
            pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC1_LOW_PRIO_INDEX ] =  pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC1_HIGH_PRIO_INDEX];
        }
        if (pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC2_HIGH_PRIO_INDEX] > (pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_HSYNC2_HIGH_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE))
        {
            pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC2_HIGH_PRIO_INDEX] =  pIF->Tx.SndLimit.Prio_x_BytesPerMs[NRT_HSYNC2_HIGH_PRIO_INDEX] - EDDI_NRT_FRAME_HEADER_SIZE;
            pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC2_LOW_PRIO_INDEX ] =  pIF->Tx.SndLimit.MaxFrameLen[NRT_HSYNC2_HIGH_PRIO_INDEX];
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitComponent()                          */
/*                                                                         */
/* D e s c r i p t i o n: called by EDDI_DeviceCompIni()                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitComponent( EDDI_UPPER_CMP_NRT_INI_PTR_TYPE  const  pRqbCMP,
                                                      EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB )
{
    LSA_UINT32                               Index;
    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = &pDDB->NRT;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitComponent->");

    //check if not already initialized. If so report error.
    if (!(pDDB->pLocal_NRT == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_NRTInitComponent", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //init with 0
    //EDDI_MemSet(pNRTComp, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_COMP_NRT_TYPE));

    pDDB->pLocal_NRT = pNRTComp;

    //already 0 pNRTComp->HandleCnt = 0; /* currently no handles open */

    pNRTComp->HOL_CH_Limit = EDDI_NRT_HYST_STS_UP_NORMAL;

    pNRTComp->TimeStamp.TimerID                 = 0xFFFF;
    pNRTComp->TimeStamp.Timeout_ticks_in10ms    = 5;
    #if defined (EDDI_CFG_REV5)
    pNRTComp->TimeStamp.State                   = EDDI_TS_STATE_NO_REQ;
    pNRTComp->TimeStamp.bLastTxTimeStored       = LSA_FALSE;
    //already 0 pNRTComp->TimeStamp.cTimeStamp  = 0;
    //already 0 pNRTComp->TimeStamp.HwPortIndex = 0;
    #endif

    #if defined (EDDI_CFG_REV5)
    if (   (EDDI_LOC_ERTEC400       == pDDB->ERTEC_Version.Location)
        || (EDDI_LOC_FPGA_XC2_V8000 == pDDB->ERTEC_Version.Location))
    {
        pNRTComp->pTS_IF = &pNRTComp->IF[EDDI_NRT_CHB_IF_0];
        EDDI_NRT_TS_TRACE_INIT(pDDB);
    }
    else
    {
        EDDI_Excp("EDDI_NRTInitComponent, wrong location for ERTEC400", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    #endif //EDDI_CFG_REV5

    #if defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
    pDDB->SII.NRT_RX_Limit_ORG_MaskIRTE     = 0;
    #endif
    #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION)
    pDDB->SII.NRT_RX_Limit_NRT_LOW_MaskIRTE = 0;
    #endif

    #if defined (EDDI_CFG_FRAG_ON)
    //initialize data for NRT TX fragmentation
    EDDI_NRTFragInitStartup(pDDB);
    #endif

    #if defined (EDDI_CFG_DEFRAG_ON)
    //Alloc 3x 1.5k buffer for NRT defragmentation  
    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_NRT_IF_INI_TYPE  *  const  pIni = &pRqbCMP->u.Interface[Index];

        EDDI_DEVAllocMode(pDDB, pIni->MemModeBuffer, (EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR *)&pDDB->NRT.RxFragData.pNRTDeFragFrame[Index], (LSA_UINT32)EDD_FRAME_BUFFER_LENGTH, pIni->UserMemIDRXDefault);
        if (pDDB->NRT.RxFragData.pNRTDeFragFrame[Index] == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_NRTInitComponent, NRTDeFragFrame, EDDI_DEVAllocMode", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }
    }
    #endif

    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_SER_DMACW_PTR_TYPE            pDMACW;
        SER_TOP_DMACW_ADR_TYPE   *         pTopAdrDMACW;
        EDDI_NRT_IF_INI_TYPE     *  const  pIni        = &pRqbCMP->u.Interface[Index];
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF         = &pNRTComp->IF[Index];
        LSA_UINT32                  const  TxCntDscrIF = pIni->TxCntDscr;
        LSA_UINT32                  const  RxCntDscrIF = pIni->RxCntDscr;
        LSA_UINT8                          PrioQueueArrayIndex;

        pIF->QueueIdx = Index;
        pIF->Tx.PrioQueueArrayStartIndex = NRT_MAX_PRIO_QUEUE; //Start with highest+1. If a RQB is sent, the index will be lowered.
        pIF->Tx.MaxPrioQueueArrayIndex = (EDDI_NRT_CHA_IF_0 == Index) ? MaxPrioQueueArrayIndexIFA : MaxPrioQueueArrayIndexIFB;

        //initialize prioqueue translation
        for (PrioQueueArrayIndex = 0; PrioQueueArrayIndex < NRT_MAX_PRIO_QUEUE; PrioQueueArrayIndex++)
        {
            pIF->Tx.RevPrioQueueArrayIndex[PrioQueueArrayIndex] = NRT_MAX_PRIO_QUEUE; //init with highest+1 for error detection
        }

        for (PrioQueueArrayIndex = 0; PrioQueueArrayIndex <= pIF->Tx.MaxPrioQueueArrayIndex; PrioQueueArrayIndex++)
        {
            LSA_UINT8 const PrioIndex = (EDDI_NRT_CHA_IF_0 == Index) ? PrioQueueArrayIndexIFA[PrioQueueArrayIndex] : PrioQueueArrayIndexIFB[PrioQueueArrayIndex];

            pIF->Tx.PrioQueueArrayIndex[PrioQueueArrayIndex] = PrioIndex; 
            pIF->Tx.RevPrioQueueArrayIndex[PrioIndex]        = PrioQueueArrayIndex; 
            if (NRT_HSYNC1_HIGH_PRIO_INDEX == PrioIndex)
            {
                //additional entry
                pIF->Tx.RevPrioQueueArrayIndex[NRT_HSYNC2_HIGH_PRIO_INDEX] = PrioQueueArrayIndex;
            }
            else if (NRT_HSYNC1_LOW_PRIO_INDEX == PrioIndex)
            {
                //additional entry
                pIF->Tx.RevPrioQueueArrayIndex[NRT_HSYNC2_LOW_PRIO_INDEX] = PrioQueueArrayIndex;
            }
        }

        pIF->Tx.NextHSYNCPrioQueueHigh = NRT_HSYNC1_HIGH_PRIO_INDEX;
        pIF->Tx.NextHSYNCPrioQueueLow  = NRT_HSYNC1_LOW_PRIO_INDEX;

        EDDI_NRTInitLimit(pIni, pDDB, pIF);
        EDDI_NrtCheckDeferredAlloc(pIni, pIF);
        EDDI_IniNrtLockFct(pIF);

        pIF->MemModeBuffer = pIni->MemModeBuffer;
        pIF->MemModeDMACW  = pIni->MemModeDMACW;

        pIF->UserMemIDDMACWDefault                          = pIni->UserMemIDDMACWDefault;
        pIF->UserMemIDTXDefault                             = pIni->UserMemIDTXDefault;
        pIF->UserMemIDRXDefault                             = pIni->UserMemIDRXDefault;
        pIF->UserMemIDTXFragArray[NRT_LOW_PRIO_INDEX]       = pIni->UserMemIDTXLowFrag;
        pIF->UserMemIDTXFragArray[NRT_MEDIUM_PRIO_INDEX]    = pIni->UserMemIDTXMidFrag;
        pIF->UserMemIDTXFragArray[NRT_HIGH_PRIO_INDEX]      = pIni->UserMemIDTXHighFrag;
        pIF->UserMemIDTXFragArray[NRT_MGMT_LOW_PRIO_INDEX]  = pIni->UserMemIDTXMgmtLowFrag;
        pIF->UserMemIDTXFragArray[NRT_MGMT_HIGH_PRIO_INDEX] = pIni->UserMemIDTXMgmtHighFrag;

        pIF->Rx.MaxRcvFrame_ToLookUp   = pIni->MaxRcvFrame_ToLookUp;
        pIF->Rx.MaxRcvFrame_SendToUser = pIni->MaxRcvFrame_SendToUser;

        #if defined (EDDI_CFG_DEFRAG_ON)        
        pIF->Rx.MaxRcvFragments_ToCopy                      = pIni->MaxRcvFragments_ToCopy;               
        #endif

        pIF->Rx.DoneFct = EDDI_NRTRxDoneLoadLimit;

        if (   (   (EDD_SYS_UDP_WHITELIST_FILTER_OFF != pIni->RxFilterUDP_Broadcast)
                && (EDD_SYS_UDP_WHITELIST_FILTER_ON != pIni->RxFilterUDP_Broadcast) )
            || (   (EDD_SYS_UDP_WHITELIST_FILTER_OFF != pIni->RxFilterUDP_Unicast)
                 && (EDD_SYS_UDP_WHITELIST_FILTER_ON != pIni->RxFilterUDP_Unicast) )
            || (   (EDD_FEATURE_DISABLE != pIni->RxFilterIFMACAddr)
                && (EDD_FEATURE_ENABLE != pIni->RxFilterIFMACAddr) )
            )
        {
            EDDI_Excp("EDDI_NRTInitComponent, illegal values for pIF->Rx.RxFilterUDP_Unicast and/or pIF->Rx.RxFilterUDP_Broadcast",
                EDDI_FATAL_ERR_EXCP, pIni->RxFilterUDP_Broadcast, pIni->RxFilterUDP_Unicast);
            return;
        }

        pIF->RxFilterUDP_Unicast    = (EDD_SYS_UDP_WHITELIST_FILTER_ON == pIni->RxFilterUDP_Unicast)?LSA_TRUE:LSA_FALSE;
        pIF->RxFilterUDP_Broadcast  = (EDD_SYS_UDP_WHITELIST_FILTER_ON == pIni->RxFilterUDP_Broadcast)?LSA_TRUE:LSA_FALSE;
        pIF->RxFilterIFMACAddr      = (EDD_FEATURE_ENABLE              == pIni->RxFilterIFMACAddr)?LSA_TRUE:LSA_FALSE;

        if (   (RxCntDscrIF)
            && (   (pIF->Rx.MaxRcvFrame_ToLookUp   == 0)
                || (pIF->Rx.MaxRcvFrame_SendToUser == 0)))
        {
            //a NRT-receive-limitation is mandatory
            EDDI_Excp("EDDI_NRTInitComponent, pIF->Rx.MaxRcvFrame_ToLookUp and/or pIF->Rx.MaxRcvFrame_SendToUser == 0",
                      EDDI_FATAL_ERR_EXCP, pIF->Rx.MaxRcvFrame_ToLookUp, pIF->Rx.MaxRcvFrame_SendToUser);
            return;
        }

        #if defined (EDDI_CFG_DEFRAG_ON)   
        if (   (RxCntDscrIF)
            && (pIF->Rx.MaxRcvFragments_ToCopy == 0))
        {
            //a NRT-receive-limitation is mandatory
            EDDI_Excp("EDDI_NRTInitComponent, pIF->Rx.MaxRcvFragments_ToCopy  == 0",
                      EDDI_FATAL_ERR_EXCP, pIF->Rx.MaxRcvFragments_ToCopy, 0);
            return;
        }
        #endif

        switch (pIF->MemModeBuffer)
        {
            case MEMORY_SDRAM_ERTEC:
            {
                pIF->adr_to_asic          = pDDB->sdram_mem_adr_to_asic;
                pIF->LimitDown_BufferAddr = pDDB->NRTMEM_LowerLimit;
                pIF->LimitUp_BufferAddr   = pDDB->NRTMEM_UpperLimit;
                break;
            }

            case MEMORY_SHARED_MEM:
            {
                pIF->adr_to_asic          = pDDB->shared_mem_adr_to_asic;
                pIF->LimitDown_BufferAddr = pDDB->NRTMEM_LowerLimit;
                pIF->LimitUp_BufferAddr   = pDDB->NRTMEM_UpperLimit;
                break;
            }

            default:
                EDDI_Excp("EDDI_NRTInitComponent, invalid MemModeBuffer:", EDDI_FATAL_ERR_EXCP, pIF->MemModeBuffer, 0);
                return;
        }

        switch (pIF->MemModeDMACW)
        {
            case MEMORY_SDRAM_ERTEC:
                pIF->adr_to_asic_dmacw = pDDB->sdram_mem_adr_to_asic;
                break;

            case MEMORY_SHARED_MEM:
                pIF->adr_to_asic_dmacw = pDDB->shared_mem_adr_to_asic;
                break;

            default:
                EDDI_Excp("EDDI_NRTInitComponent, invalid MemModeDMACW:", EDDI_FATAL_ERR_EXCP, pIF->MemModeDMACW, 0);
                return;
        }

        EDDI_SetBitField32(&pIF->Value_for_NRT_Queue_DISABLE, EDDI_SER_NRT_API_CTRL_BIT__S_R, 1); //set disable bit

        switch (Index)
        {
            case EDDI_NRT_CHA_IF_0:
            {
                EDDI_SetBitField32(&pIF->Value_for_NRT_Queue_ENABLE,  EDDI_SER_NRT_API_CTRL_BIT__CHA0, 1);
                EDDI_SetBitField32(&pIF->Value_for_NRT_Queue_DISABLE, EDDI_SER_NRT_API_CTRL_BIT__CHA0, 1);

                pIF->Value_for_IO_NRT_ENABLE            = NRT_ENABLE_CHA0;
                pIF->Value_for_IO_HOL_COUNT_CH          = HOL_COUNT_CHA_SS0;
                pIF->Value_for_IO_NRT_SND_DESCRIPTOR    = NRT_SND_DESCRIPTOR_CHA0;
                pIF->Value_for_IO_NRT_TX_Interrupt_IRTE = IRQ_CHA0_SND_DONE;
                pIF->Value_for_IO_NRT_RX_Interrupt_IRTE = IRQ_CHA0_RCV_DONE;

                #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION)
                pIF->pNewCycleReductionCtrForNRTCheckRxLimit        = &pDDB->SII.NewCycleReductionCtrForNRTCheckRxLimitA0;
                pDDB->SII.NewCycleReductionCtrForNRTCheckRxLimitA0  = 0;    //= no NRT-RX-overload present
                pIF->pNRT_RX_Limit_MaskIRTE                         = &pDDB->SII.NRT_RX_Limit_NRT_LOW_MaskIRTE;
                #if (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION == 0)
                pIF->NewCycleReductionFactorForNRTCheckRxLimit      = (LSA_UINT32)0x1000; //mark for special usecase!
                #else
                pIF->NewCycleReductionFactorForNRTCheckRxLimit      = (LSA_UINT32)EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION;
                #endif
                #elif defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
                pIF->NewCycleReductionFactorForNRTCheckRxLimit      = 0;
                #endif

                break;
            }

            case EDDI_NRT_CHB_IF_0:
            {
                EDDI_SetBitField32(&pIF->Value_for_NRT_Queue_ENABLE,  EDDI_SER_NRT_API_CTRL_BIT__CHB0, 1);
                EDDI_SetBitField32(&pIF->Value_for_NRT_Queue_DISABLE, EDDI_SER_NRT_API_CTRL_BIT__CHB0, 1);

                pIF->Value_for_IO_NRT_ENABLE            = NRT_ENABLE_CHB0;
                pIF->Value_for_IO_HOL_COUNT_CH          = HOL_COUNT_CHB_SS0;
                pIF->Value_for_IO_NRT_SND_DESCRIPTOR    = NRT_SND_DESCRIPTOR_CHB0;
                pIF->Value_for_IO_NRT_TX_Interrupt_IRTE = IRQ_CHB0_SND_DONE;
                pIF->Value_for_IO_NRT_RX_Interrupt_IRTE = IRQ_CHB0_RCV_DONE;

                #if defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
                pIF->pNewCycleReductionCtrForNRTCheckRxLimit        = &pDDB->SII.NewCycleReductionCtrForNRTCheckRxLimitB0;
                pDDB->SII.NewCycleReductionCtrForNRTCheckRxLimitB0  = 0;    //= no NRT-RX-overload present
                pIF->pNRT_RX_Limit_MaskIRTE                         = &pDDB->SII.NRT_RX_Limit_ORG_MaskIRTE;
                #if (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION == 0)
                pIF->NewCycleReductionFactorForNRTCheckRxLimit      = (LSA_UINT32)0x1000; //mark for special usecase!
                #else
                pIF->NewCycleReductionFactorForNRTCheckRxLimit      = (LSA_UINT32)EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION;
                #endif
                #elif defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION)
                pIF->NewCycleReductionFactorForNRTCheckRxLimit      = 0;
                #endif

                break;
            }

            default:
                EDDI_Excp("EDDI_NRTInitComponent, invalid NRT-CH-Index:", EDDI_FATAL_ERR_EXCP, Index, 0);
                return;
        }

        pTopAdrDMACW = &pDDB->Glob.LLHandle.TopAdrDMACW[Index];

        pTopAdrDMACW->Tx = 0;

        if (TxCntDscrIF)
        {
            if (TxCntDscrIF < 40UL)
            {
                EDDI_Excp("EDDI_NRTInitComponent, TxCntDscr < 40", EDDI_FATAL_ERR_EXCP, TxCntDscrIF, 0);
                return;
            }

            EDDI_NRTInitTRxIF(&pIF->Tx.Dscr, pIF, TxCntDscrIF, pDDB, LSA_FALSE, pIF->UserMemIDTXDefault);

            pDMACW = pIF->Tx.Dscr.pReq->pDMACW;

            pTopAdrDMACW->Tx = EDDI_SWAP_32_FCT(pIF->adr_to_asic_dmacw(pDDB->hSysDev, pDMACW, pDDB->ERTEC_Version.Location));
        }

        pTopAdrDMACW->Rx = 0;

        if (RxCntDscrIF)
        {
            LSA_BOOL  bAllocDataBuffer = LSA_TRUE;

            if (pIF->StateDeferredAlloc != DEF_ALLOC_DONT_CARE)
            {
                bAllocDataBuffer = LSA_FALSE;
            }

            EDDI_NRTInitTRxIF(&pIF->Rx.Dscr, pIF, RxCntDscrIF, pDDB, bAllocDataBuffer, pIF->UserMemIDRXDefault);

            pDMACW = pIF->Rx.Dscr.pReq->pDMACW;

            pTopAdrDMACW->Rx = EDDI_SWAP_32_FCT(pIF->adr_to_asic_dmacw(pDDB->hSysDev, pDMACW, pDDB->ERTEC_Version.Location));
        }
    } //end of for-loop

    if (   (  (EDD_SYS_IO_CONFIGURED_ON != pRqbCMP->bIO_Configured)
            &&  (EDD_SYS_IO_CONFIGURED_OFF != pRqbCMP->bIO_Configured) )
        ||  (  (EDD_SYS_FILL_ACTIVE != pRqbCMP->bFeedInLoadLimitationActive) 
             && (EDD_SYS_FILL_INACTIVE != pRqbCMP->bFeedInLoadLimitationActive) ) )
    {
        EDDI_Excp("EDDI_NRTInitComponent, illegal values for bIO_Configured and/or bFeedInLoadLimitationActive!", EDDI_FATAL_ERR_EXCP, pRqbCMP->bIO_Configured, pRqbCMP->bFeedInLoadLimitationActive);
        return;
    }
    else if (   (EDD_SYS_IO_CONFIGURED_ON == pRqbCMP->bIO_Configured)
             && (EDD_SYS_FILL_ACTIVE != pRqbCMP->bFeedInLoadLimitationActive) )
    {
        EDDI_Excp("EDDI_NRTInitComponent, IO_Configured=True and FILL=False is not allowed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pNRTComp->bFeedInLoadLimitationActiveDefault = (EDD_SYS_FILL_ACTIVE == pRqbCMP ->bFeedInLoadLimitationActive)?LSA_TRUE:LSA_FALSE;
    pNRTComp->bFeedInLoadLimitationActive        = pNRTComp->bFeedInLoadLimitationActiveDefault;
    pNRTComp->bIO_ConfiguredDefault              = (EDD_SYS_IO_CONFIGURED_ON == pRqbCMP ->bIO_Configured)?LSA_TRUE:LSA_FALSE;
    pNRTComp->bIO_Configured                     = pNRTComp->bIO_ConfiguredDefault;
    EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTInitComponent : LoadLimitationActive(%u), IO_Configured(%u)", 
                      pNRTComp->bFeedInLoadLimitationActive, pNRTComp->bIO_Configured); 

    if (!pRqbCMP->bFeedInLoadLimitationActive  &&  (pDDB->HSYNCRole == EDDI_HSYNC_ROLE_APPL_SUPPORT))
    {
        EDDI_PRM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_NRTInitComponent : LoadLimitationActive(%u) == FALSE  BUT  HSYNC-Role(%u) == APPL_SUPPORT", 
                          pNRTComp->bFeedInLoadLimitationActive, pDDB->HSYNCRole); 
        EDDI_Excp("EDDI_NRTInitComponent, FILL=False  AND  HSyncRole is APPL_SUPPORT is not allowed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTAllocDataBuffer()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTAllocDataBuffer( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                        EDDI_NRT_CHX_SS_IF_TYPE  const *  const  pIF,
                                                        NRT_DSCR_PTR_TYPE                 const  pNRTDscr,
                                                        EDDI_USERMEMID_TYPE               const  UserMemID )
{
    void  *  pDataBuffer;

    EDDI_DEVAllocMode(pDDB, pIF->MemModeBuffer, &pDataBuffer, (LSA_UINT32)EDD_FRAME_BUFFER_LENGTH, UserMemID);

    EDDI_HOST2IRTE32x(pNRTDscr->pDMACW->pDataBuffer, pIF->adr_to_asic(pDDB->hSysDev, pDataBuffer, pDDB->ERTEC_Version.Location));
    pNRTDscr->pRcvDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)pDataBuffer;

    NRT_SET_BufferLength(pNRTDscr->pDMACW->L0.Reg, (EDDI_FRAME_BUFFER_LENGTH >> 2));
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitTRxIF()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     CntDscrIF > 0 is already checked!                */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#define EDDI_DMACW_ALIGN_MASK   0x0FUL
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitTRxIF( NRT_CHX_SSX_PTR_TYPE        const  pDscr,
                                                          EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                          LSA_UINT32                  const  CntDscrIF,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          LSA_BOOL                    const  bAllocDataBuffer,
                                                          EDDI_USERMEMID_TYPE         const  UserMemID )
{
    LSA_UINT                 Index;
    NRT_DSCR_PTR_TYPE        pNRTDscr, pPrevNRTDscr;
    EDDI_SER_DMACW_PTR_TYPE  pDMACW;
    LSA_UINT32               Size;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitTRxIF->");

    /*-------------------------------------------------------------------------*/
    /* Allocate memory for NRT-Descriptors                                     */
    /*-------------------------------------------------------------------------*/
    //already 0 pDscr->pTop = EDDI_NULL_PTR;
    //already 0 pDscr->Cnt  = 0;

    //allocate NRT-Dscr-memory
    Size = CntDscrIF * sizeof(NRT_DSCR_TYPE);
    EDDI_AllocLocalMem((void * *)&pDscr->pNRTDscrArray, Size);
    if (pDscr->pNRTDscrArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTInitTRxIF, EDDI_AllocLocalMem", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //allocate NRT-DMACW-buffer-memory
    Size = (CntDscrIF * sizeof(EDDI_SER_DMACW_TYPE)) + EDDI_DMACW_ALIGN_MASK;
    EDDI_DEVAllocMode(pDDB, pIF->MemModeDMACW, (void * *)&pDscr->pDMACWArray, Size, pIF->UserMemIDDMACWDefault);
    if (pDscr->pDMACWArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_NRTInitTRxIF, EDDI_DEVAllocMode", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pPrevNRTDscr = EDDI_NULL_PTR;
    pNRTDscr     = pDscr->pNRTDscrArray;
    pDMACW       = pDscr->pDMACWArray;

    pDMACW = (EDDI_SER_DMACW_PTR_TYPE)(void *)(((LSA_UINT32)pDMACW + EDDI_DMACW_ALIGN_MASK) & (~EDDI_DMACW_ALIGN_MASK));

    for (Index = 0; Index < CntDscrIF; Index++)
    {
        pDMACW->L0.Reg       = 0;
        pNRTDscr->pDMACW     = pDMACW;
        pNRTDscr->pAsicDMACW = pIF->adr_to_asic_dmacw(pDDB->hSysDev, pDMACW, pDDB->ERTEC_Version.Location);

        if (bAllocDataBuffer)
        {
            //allocate NRT-data-buffer
            EDDI_NRTAllocDataBuffer(pDDB, pIF, pNRTDscr, UserMemID);
            //rest in DeviceSetup -> EDDI_SERSetupNRT()
        }

        pNRTDscr->pSndRQB = EDDI_NULL_PTR;   /* Auftragsqueue Sende für alle Open eine */

        //queue Dscr
        if (pPrevNRTDscr == EDDI_NULL_PTR)
        {
            pDscr->pTop = pNRTDscr;
        }
        else
        {
            pPrevNRTDscr->pDMACW->pNext = EDDI_HOST2IRTE32(pNRTDscr->pAsicDMACW);
            pPrevNRTDscr->next_dscr_ptr = pNRTDscr;
        }

        pNRTDscr->prev_dscr_ptr = pPrevNRTDscr;
        pPrevNRTDscr            = pNRTDscr;

        pNRTDscr++;
        pDMACW++;
    }

    pNRTDscr--;

    pNRTDscr->next_dscr_ptr    = pDscr->pTop;
    pDscr->pTop->prev_dscr_ptr = pNRTDscr;

    //set descriptor counter
    pDscr->Cnt = CntDscrIF;

    //close DMACW ring
    pNRTDscr->pDMACW->pNext = EDDI_SWAP_32_FCT(pIF->adr_to_asic_dmacw(pDDB->hSysDev, pDscr->pTop->pDMACW, pDDB->ERTEC_Version.Location));

    //init pNRTTxIF -> dynamic pointer to the ring
    pDscr->pReq     = pDscr->pTop;
    pDscr->pReqDone = pDscr->pTop;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTDeviceSetup()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTDeviceSetup( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    EDDI_UPPER_DSB_PTR_TYPE  const  pDSB )
{
    LSA_UINT32                         Index, UsrPortIndex;
    EDDI_SER_NRT_HEAD_PTR_TYPE         pCTRLDevBaseAddr;
    EDDI_SER_NRT_PRIO_PTR_TYPE         pNRTPrio;
    EDDI_SER_NRT_FCW_PTR_TYPE          pDev_NRTFcw;
    EDDI_SER_NRT_DB_PTR_TYPE           pDev_NRTDb;
    EDDI_SER_NRT_FREE_CCW_PTR_TYPE     pCCWDevBaseAddr;
    LSA_UINT32                         length, ret;
    LSA_UINT32                         cDB_FCW, needed_DB_FCW;
    LSA_UINT32                         size_needed_kram, reserve_IRT, alloc_size;
    LSA_UINT8                       *  pKRam;
    EDDI_MEM_BUF_EL_H               *  pMemHeader;
    LSA_UINT32                         TmpAdrHostFormat;
    
    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTDeviceSetup->");

    pCCWDevBaseAddr = pDDB->Glob.LLHandle.pCCWDevBaseAddr;

    if (!(pDDB->Glob.LLHandle.pCTRLDevBaseAddr == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_NRTDeviceSetup", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_KramCheckFreeBuffer(pDDB);

    size_needed_kram  = sizeof(EDDI_SER_NRT_HEAD_TYPE);
    size_needed_kram += sizeof(EDDI_SER_NRT_PRIO_TYPE) * 2;  //CHA + CHB
    size_needed_kram += sizeof(EDDI_SER_NRT_PRIO_TYPE) * pDDB->PM.PortMap.PortCnt;

    reserve_IRT = EDDI_CONV_KRAM_RESERVE;
 
    if (pDDB->pLocal_CRT)
    {
        reserve_IRT += pDDB->pLocal_CRT->KRAMSize_xRT_Trees;
        reserve_IRT += sizeof(EDDI_SER10_SRT_FCW_TYPE) * pDSB->SRTPara.SRT_FCW_Count;
        reserve_IRT += EDDI_SerGetAcwKramSize(pDDB->CRT.MetaInfo.AcwConsumerCnt);
    }

    reserve_IRT += (SER_STATISTIC_PORT_SIZE * pDDB->PM.HwTypeMaxPortCnt) + SER_STATISTIC_BASE_OFFSET;

    size_needed_kram += reserve_IRT;

    if (pDDB->free_KRAM_buffer < size_needed_kram)
    {
        EDDI_Excp("EDDI_NRTDeviceSetup", EDDI_FATAL_ERR_EXCP, pDDB->free_KRAM_buffer, size_needed_kram);
        return;
    }

    cDB_FCW = pDDB->free_KRAM_buffer - size_needed_kram;

    cDB_FCW = cDB_FCW / (sizeof(EDDI_SER_NRT_FCW_TYPE) + sizeof(EDDI_SER_NRT_DB_TYPE));

    needed_DB_FCW = (pDDB->PM.PortMap.PortCnt + 2) * (3 * EDDI_MAX_DB_PER_FRAME);  //3 telegrams per port + CHA + CHB

    if (cDB_FCW < needed_DB_FCW)
    {
        EDDI_Excp("EDDI_NRTDeviceSetup, cDB_FCW < needed_DB_FCW, Not enough free KRAM", EDDI_FATAL_ERR_EXCP, needed_DB_FCW, cDB_FCW);
        return;
    }

    size_needed_kram += sizeof(EDDI_SER_NRT_FCW_TYPE) * pDSB->NRTPara.NRT_FCW_Count;
    size_needed_kram += sizeof(EDDI_SER_NRT_DB_TYPE)  * pDSB->NRTPara.NRT_DB_Count;

    if (pDDB->free_KRAM_buffer < size_needed_kram)
    {
        EDDI_Excp("EDDI_NRTDeviceSetu p, Not enough free KRAM", EDDI_FATAL_ERR_EXCP, size_needed_kram, pDDB->free_KRAM_buffer);
        return;
    }

    alloc_size = size_needed_kram - reserve_IRT;
    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pMemHeader, alloc_size);
    if (ret != EDDI_MEM_RET_OK)
    {
        if (ret == EDDI_MEM_RET_NO_FREE_MEMORY)
        {
            EDDI_Excp("EDDI_NRTDeviceSetup, Not enough free KRAM", EDDI_FATAL_ERR_EXCP, ret, size_needed_kram);
            return;
        }
        else
        {
            EDDI_Excp("EDDI_NRTDeviceSetup, EDDI_MEMGetBuffer see eddi_mem.h", EDDI_FATAL_ERR_EXCP, ret, size_needed_kram);
            return;
        }
    }

    pKRam                    = pMemHeader->pKRam;
    pDDB->KramRes.Nrt_DB_FCW = alloc_size;

    /*-------------------------------------------------------------------------*/
    /* Setup NRT-Structs                                                       */
    /*-------------------------------------------------------------------------*/
    length           = sizeof(EDDI_SER_NRT_HEAD_TYPE);
    pCTRLDevBaseAddr = (EDDI_SER_NRT_HEAD_PTR_TYPE)(void *)pKRam;
    pKRam           += length;

    //init NRT-CTRL-LIST
    //init CHA
    length   = sizeof(EDDI_SER_NRT_PRIO_TYPE);
    pNRTPrio = (EDDI_SER_NRT_PRIO_PTR_TYPE)(void *)pKRam;
    pKRam   += length;

    pCTRLDevBaseAddr->Hw0.CH.A = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pNRTPrio, pDDB));

    //init CHB
    pNRTPrio = (EDDI_SER_NRT_PRIO_PTR_TYPE)(void *)pKRam;
    pKRam   += length;

    pCTRLDevBaseAddr->Hw0.CH.B = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pNRTPrio, pDDB));

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pNRTPrio = (EDDI_SER_NRT_PRIO_PTR_TYPE)(void *)pKRam;
        pKRam   += length;

        switch (HwPortIndex)
        {
            case 0:
                pCTRLDevBaseAddr->Hw1.P.N0 = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pNRTPrio, pDDB));
                break;
            case 1:
                pCTRLDevBaseAddr->Hw1.P.N1 = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pNRTPrio, pDDB));
                break;

            #if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_SOC)
            case 2:
            {
                #if defined (EDDI_CFG_SOC)
                if (EDDI_LOC_SOC1 == pDDB->ERTEC_Version.Location)
                #endif
                {
                    pCTRLDevBaseAddr->Hw2.P.N2 = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pNRTPrio, pDDB));
                    break;
                }
            }
            #endif

            #if defined (EDDI_CFG_ERTEC_400)
            case 3:
                pCTRLDevBaseAddr->Hw2.P.N3 = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pNRTPrio, pDDB));
                break;
            #endif

            //lint -fallthrough         //fallthrough from case 2:  EDDI_CFG_SOC defined
            default:
                EDDI_Excp("EDDI_NRTDeviceSetup", EDDI_FATAL_ERR_EXCP, UsrPortIndex, 0);
                return;
        }
    }

    //Alloc free NRT-FCWs in one piece to optimize memory usage
    length       = sizeof(EDDI_SER_NRT_FCW_TYPE) * pDSB->NRTPara.NRT_FCW_Count;
    pDev_NRTFcw  = (EDDI_SER_NRT_FCW_PTR_TYPE)(void *)pKRam;
    pKRam       += length;

    pCCWDevBaseAddr->NRTFcw.p.Bottom = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pDev_NRTFcw, pDDB));

    for (Index = 1; Index < pDSB->NRTPara.NRT_FCW_Count; Index++)
    {
        TmpAdrHostFormat = DEV_kram_adr_to_asic_register(pDev_NRTFcw, pDDB);
        pDev_NRTFcw++;

        //Optimization for startup
        NRT_SET_NRT_LL0_FCW_BIT2__pNext(pDev_NRTFcw->Hw0.Value.U32_1, TmpAdrHostFormat);
    }

    TmpAdrHostFormat              = DEV_kram_adr_to_asic_register(pDev_NRTFcw, pDDB);
    pCCWDevBaseAddr->NRTFcw.p.Top = EDDI_HOST2IRTE32(TmpAdrHostFormat);

    //Alloc free NRT-DBs in one piece to optimize memory usage
    length      = sizeof(EDDI_SER_NRT_DB_TYPE) * pDSB->NRTPara.NRT_DB_Count;
    pDev_NRTDb  = (EDDI_SER_NRT_DB_PTR_TYPE)(void *)pKRam;

    //Optimize startup
    pCCWDevBaseAddr->NRTDb.p.Bottom = EDDI_SWAP_32_FCT(DEV_kram_adr_to_asic_register(pDev_NRTDb, pDDB));

    for (Index = 1; Index < pDSB->NRTPara.NRT_DB_Count; Index++)
    {
        TmpAdrHostFormat = DEV_kram_adr_to_asic_register(pDev_NRTDb, pDDB);
        pDev_NRTDb++;

        //Optimize startup
        NRT_SET_NRT_LL0_DB_BIT__pNext(pDev_NRTDb->Hw0.Value.U32_0, TmpAdrHostFormat);
    }

    TmpAdrHostFormat             = DEV_kram_adr_to_asic_register(pDev_NRTDb, pDDB);
    pCCWDevBaseAddr->NRTDb.p.Top = EDDI_HOST2IRTE32(TmpAdrHostFormat);

    //NRT-Structs
    pDDB->Glob.LLHandle.pCTRLDevBaseAddr = pCTRLDevBaseAddr;
    pDDB->Glob.LLHandle.NRT_FCW_Count    = pDSB->NRTPara.NRT_FCW_Count;
    pDDB->Glob.LLHandle.NRT_DB_Count     = pDSB->NRTPara.NRT_DB_Count;
    pDDB->NRT.NRT_DB_CHx                 = (pDSB->NRTPara.NRT_DB_Count / 2) / pDDB->PM.PortMap.PortCnt;

    #if defined (EDDI_CFG_ENABLE_FAST_SB_TIMING)
    pDDB->Glob.LLHandle.pCTRLDevBaseAddr->TimeMin = 0xFFFFFFFFUL;
    pDDB->Glob.LLHandle.pCTRLDevBaseAddr->Time    = 0;
    pDDB->Glob.LLHandle.pCTRLDevBaseAddr->TimeMax = 0;
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtSetSendLimit()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtSetSendLimit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         Index;
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON)
    LSA_UINT32  const  CycleLength_us = pDDB->SII.SII_ExtTimerInterval;
    #elif defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    LSA_UINT32         CycleLength_us;
    #else
    LSA_UINT32  const  CycleLength_us = pDDB->CycCount.CycleLength_us;
    #endif

    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    if //ExtTimerMode on?
       (pDDB->SII.SII_ExtTimerInterval)
    {
        CycleLength_us = pDDB->SII.SII_ExtTimerInterval;
    }
    else //ExtTimerMode off
    {
        CycleLength_us = pDDB->CycCount.CycleLength_us;
    }
    #endif

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtSetSendLimit->");

    for (Index = EDDI_NRT_CHA_IF_0; Index < EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF                      = &pDDB->NRT.IF[Index];
        EDDI_NRT_SND_IF_TYPE     *  const  pTx                      = &pIF->Tx;

        if (CycleLength_us >= 1000UL)
        {
            pTx->SndLimit.SllReduction = 1UL;
        }
        else if (CycleLength_us >= 500UL)
        {
            pTx->SndLimit.SllReduction = 2UL;
        }
        else if (CycleLength_us >= 250UL)
        {
            pTx->SndLimit.SllReduction = 4UL;
        }
        else if (CycleLength_us >= 125UL)
        {
            pTx->SndLimit.SllReduction = 8UL;
        }
        else if (CycleLength_us >= 63UL)
        {
            pTx->SndLimit.SllReduction = 16UL;
        }
        else if (CycleLength_us >= 31UL)
        {
            pTx->SndLimit.SllReduction = 32UL;
        }
        else
        {
            EDDI_Excp("EDDI_NrtSetSendLimit, CycleLength_us:", EDDI_FATAL_ERR_EXCP, CycleLength_us, 0);
            return;
        }

        //FILL for Prio_Low/Medium/High is always limited by the HOL limit. Otherwise frames will by dropped by the switch matrix.
        pTx->SndLimit.BytesPerCycleLMH                             = EDDI_NrtSetPrioLimit((pDDB->Glob.LLHandle.HOL_Limit_Port_Up / 2) * EDDI_NRT_DB_LEN, CycleLength_us);

        if (pDDB->NRT.bFeedInLoadLimitationActive)
        {
            pTx->SndLimit.BytesPerCycle                                 = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_OVERALL,        CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_LOW_PRIO_INDEX]         = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_PRIO_LOW,       CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_MEDIUM_PRIO_INDEX]      = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_PRIO_MEDIUM,    CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HIGH_PRIO_INDEX]        = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_PRIO_HIGH,      CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_MGMT_LOW_PRIO_INDEX]    = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_PRIO_MGMT_LOW,  CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_MGMT_HIGH_PRIO_INDEX]   = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_PRIO_MGMT_HIGH, CycleLength_us);
            
            //Each port has 3kb/ms HSYNC limit, together for High and LOW. The counters for LOW are initialized for sanity only, but not used.
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC1_HIGH_PRIO_INDEX] = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_Prio_HSYNC_1,   CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC2_HIGH_PRIO_INDEX] = EDDI_NrtSetPrioLimit(EDDI_NRT_FILLSIZE_Prio_HSYNC_2,   CycleLength_us);
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC1_LOW_PRIO_INDEX ] = EDDI_NRT_FRAME_HEADER_SIZE;    //Not used
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC2_LOW_PRIO_INDEX ] = EDDI_NRT_FRAME_HEADER_SIZE;    //Not used
            //Attention: If new counters are added, check EDDI_NRTReloadSendList for resetting!
        }
        else
        {
            pTx->SndLimit.BytesPerCycle                                 = EDDI_NRT_FILLSIZE_UNRESTRICTED_OVERALL;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_LOW_PRIO_INDEX]         = EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_LOW;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_MEDIUM_PRIO_INDEX]      = EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_MEDIUM;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HIGH_PRIO_INDEX]        = EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_HIGH;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_MGMT_LOW_PRIO_INDEX]    = EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_MGMT_LOW;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_MGMT_HIGH_PRIO_INDEX]   = EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_MGMT_HIGH;
            
            //HSYNC application is not allowed with FILL=OFF.
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC1_HIGH_PRIO_INDEX] = EDDI_NRT_FILLSIZE_UNRESTRICTED_Prio_HSYNC_1;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC2_HIGH_PRIO_INDEX] = EDDI_NRT_FILLSIZE_UNRESTRICTED_Prio_HSYNC_2;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC1_LOW_PRIO_INDEX ] = EDDI_NRT_FILLSIZE_UNRESTRICTED_Prio_HSYNC_1;
            pTx->SndLimit.Prio_x_BytesPerMs[NRT_HSYNC2_LOW_PRIO_INDEX ] = EDDI_NRT_FILLSIZE_UNRESTRICTED_Prio_HSYNC_2;
        }

        pTx->SndLimit.ActRedCnt = 1;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtSetPrioLimit()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_NrtSetPrioLimit( LSA_UINT32  const  LimitBytesPerMilliSecond,
                                                               LSA_UINT32  const  CycleLength_us )
{
    LSA_UINT32  LimitBytesPerCycle;

    if (LimitBytesPerMilliSecond > 0x80000000UL)
    {
        return LimitBytesPerMilliSecond;
    }
    if (CycleLength_us >= 1000UL)
    {
        #if defined (EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION)
        LimitBytesPerCycle = LimitBytesPerMilliSecond;
        #else
        LimitBytesPerCycle = (LimitBytesPerMilliSecond * CycleLength_us) / 1000UL;
        #endif
        return LimitBytesPerCycle;
    }
    if (CycleLength_us >= 500UL)
    {
        LimitBytesPerCycle = (2UL * LimitBytesPerMilliSecond * CycleLength_us) / 1000UL;
        return LimitBytesPerCycle;
    }
    if (CycleLength_us >= 250UL)
    {
        LimitBytesPerCycle = (4UL * LimitBytesPerMilliSecond * CycleLength_us) / 1000UL;
        return LimitBytesPerCycle;
    }
    if (CycleLength_us >= 125UL)
    {
        LimitBytesPerCycle = (8UL * LimitBytesPerMilliSecond * CycleLength_us) / 1000UL;
        return LimitBytesPerCycle;
    }
    if (CycleLength_us >= 63UL)
    {
        LimitBytesPerCycle = (16UL * LimitBytesPerMilliSecond * CycleLength_us) / 1000UL;
        return LimitBytesPerCycle;
    }
    if (CycleLength_us >= 31UL)
    {
        LimitBytesPerCycle = (32UL * LimitBytesPerMilliSecond * CycleLength_us) / 1000UL;
        return LimitBytesPerCycle;
    }

    EDDI_Excp("EDDI_NrtSetPrioLimit", EDDI_FATAL_ERR_EXCP, CycleLength_us, 0);
    return 0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_ini.c                                               */
/*****************************************************************************/



