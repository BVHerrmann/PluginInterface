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
/*  F i l e               &F: eddi_ser_int.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDI.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  01.03.04    ZR    new interface for SERIniProviderFcw/SERIniConsumerFcw  */
/*  30.10.07    JS    removed check for EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B*/
/*                    because only used as dummy Frameid within framehandler!*/
/*                    (will not occur as provider/consumer fcw!)             */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ser_ext.h"
#include "eddi_ser_cmd.h"

#include "eddi_swi_ext.h"

#include "eddi_pool.h"
#include "eddi_nrt_inc.h"

#include "eddi_crt_check.h"

#include "eddi_crt_ext.h"

#if defined (EDDI_RTC1) //satisfy lint!
#endif

#define EDDI_MODULE_ID     M_ID_SER_INT
#define LTRC_ACT_MODUL_ID  302

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniLMW( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       EDDI_LOCAL_LMW_PTR_TYPE  const  pLMW );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERAddRxAcw(EDDI_CCW_CTRL_HEAD_PTR_TYPE      const pACWHead,
                                                     EDDI_SER_CCW_PTR_TYPE            const pCCWLocal,
                                                     EDDI_SER_CCW_PTR_TYPE            const pCCWDev,
                                                     EDDI_CCW_CTRL_PTR_TYPE           const pACWCtrl,
                                                     EDDI_LOCAL_DDB_PTR_TYPE          const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERCcwAddToList(EDDI_CCW_CTRL_HEAD_PTR_TYPE  const pCCWHead,
                                                         EDDI_SER_CCW_PTR_TYPE        const pCCWLocal,
                                                         EDDI_SER_CCW_PTR_TYPE        const pCCWDev,
                                                         EDDI_CCW_CTRL_PTR_TYPE       const pCCWCtrl,
                                                         EDDI_LOCAL_DDB_PTR_TYPE      const pDDB );

#if defined (EDDI_RED_PHASE_SHIFT_ON)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniSOL( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,  
                                                       EDDI_LOCAL_SOL_PTR_TYPE  const  pSOL );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniDummyTxFcw(EDDI_SER10_FCW_SND_TYPE   * const pFcwSnd,
                                                          EDDI_DEV_MEM_U8_PTR_TYPE    const pDataBuffer,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERIniDummyRxFcw(EDDI_SER10_FCW_RCV_TYPE   * const pFcwRcv,
                                                          EDDI_DEV_MEM_U8_PTR_TYPE    const pDataBuffer,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );
#endif

static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetDestPort( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                              LSA_UINT32               const  SndPort );



/***************************************************************************/
/* F u n c t i o n:       EDDI_SERIniMACAdr()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniMACAdr( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                  EDDI_SER10_LL2_COMMON_TYPE         *  const  pHw2,
                                                  EDD_MAC_ADR_TYPE            const  *  const  pMAC )
{
    LSA_UINT32     i;
    LSA_UINT8   *  p;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniMACAdr");

    p = (LSA_UINT8 *)(void *)&pHw2->Value.U32_0;
    for (i = 0; i < EDD_MAC_ADDR_SIZE; i++)
    {
        *p = pMAC->MacAdr[i];
        p++;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERIniProviderACWFrameID()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniProviderXCWDstMacIP( EDDI_LOCAL_DDB_PTR_TYPE            const   pDDB,
                                                               EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const   pProvider)
{
    switch (pProvider->LowerParams.ProviderType)
    {
        case EDDI_RTC3_AUX_PROVIDER:
        {
            //use dst mac addr from main provider lower params
            EDDI_SERIniMACAdr(pDDB, &pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw2, &pProvider->IRTtopCtrl.pAscProvider->LowerParams.DstMAC);

            break;
        }
        case EDDI_RTC1_PROVIDER:
        case EDDI_RTC2_PROVIDER:
        {
            EDDI_SERIniMACAdr(pDDB, &pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw2, &pProvider->LowerParams.DstMAC);

            break;
        }

        #if defined (EDDI_CFG_REV7)
        case EDDI_UDP_PROVIDER:
        {
            EDDI_SetBitField32(&pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd.Hw3.Value.U32_0, EDDI_SER10_LLx_UDP_ACW_SND_BIT__DestIPAddr, EDDI_HTONL(pProvider->LowerParams.UPD_DstIPAdr.dw));

            break;
        }
        #endif
        default:
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERIniProviderXCWDstMacIP, unexpected ProviderType %d, ProvID:0x%X, FrameID:0x%X", 
                pProvider->LowerParams.ProviderType, pProvider->ProviderId, pProvider->LowerParams.FrameId);
            EDDI_Excp("EDDI_SERIniProviderXCWDstMacIP, unexpected ProviderType", EDDI_FATAL_ERR_EXCP, pProvider->LowerParams.ProviderType, pProvider->ProviderId);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERIniProviderACWFrameID()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniProviderACWFrameID(EDDI_CONST_CRT_PROVIDER_PTR_TYPE const  pProvider) 
{
    EDDI_SER10_ACW_SND_TYPE  *  const  pAcwSnd    = &pProvider->pLowerCtrlACW->pCW->FcwAcw.AcwSnd;

    EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId, pProvider->LowerParams.FrameId);                /* 16 Bit */
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniSleepingProviderFcw()
*
* function:      create structs for IRT-Communication
*                ATTENTION: direct-mode:       initialize FCW direct (without help-buffer)!
*                ATTENTION: reset-before-mode: FCW must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniSleepingProviderFcw( SER_IRT_INI_FCW_TYPE     const  *  const  pComCfg,
                                                               EDDI_SER10_CCW_TYPE             *  const  pCCW,
                                                               EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    EDDI_SER10_FCW_SND_TYPE  *  const  pFcwSnd    = &pCCW->FcwAcw.FcwSnd;
    LSA_UINT32                  const  FcwDataLen = pComCfg->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniSleepingProviderFcw");

    // Hw0 64 Bit

    // Fcw will be init with Opcode NOOP here !
    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);
    //EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__IdxEthType , 0x0);           /*  2-Bit */
    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId, pComCfg->FrameID);         /* 16 Bit */

    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0, FcwDataLen);   /*  3 Bit */

    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, FcwDataLen >> 3); /*  8 Bit */
    //EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__AppStatSel , 0);            /*  3 Bit */

    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, SER10_NULL_PTR);         /* 21 Bit */

    // Hw1 64 Bit

    //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__pDB0, EDDI_NULL_PTR); /* 21 Bit */

    //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Sync, 0);    /*  1 Bit */
    //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Async, 0); /*  1 Bit */

    EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_1, EDDI_SER10_LL1_FCW_SND_BIT2__Time, pComCfg->Time);         /* 32 Bit */

    // Hw2 64 Bit

    //EDDI_SERIniMACAdr(pDDB, &pFcwSnd->Hw2, &NullAddrMAC);

    //EDDI_SetBitField32(&pFcwSnd->Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0);         /* 16 Bit */

    if (pComCfg->FrameID == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
    {
        // For IRT-Sync-Masters, DelayCtr = 1
        // --> cc_del will be calculated: cc_del = cc_rcv - Sendtimestamp,
        //     we need this to use the cc_rcv-field to timestamp the user-data(PTCP-Information)
        //     So cc_rcv must be updated by the user each time when PTCP-Information
        //     changes.
        EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__DelayCtr, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__DelayCtr, 0);
    }

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pDDB->IRT.GlobalDestination)
    {
        EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__GlobDest, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__GlobDest, 0);
    }
    #endif
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIrtProviderBufferAdd()
*
* function:      create structs for IRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIrtProviderBufferAdd( EDDI_SER10_CCW_TYPE              *  const  pCCW,
                                                             LSA_UINT8                           const  ImageMode,
                                                             EDDI_DEV_MEM_U8_PTR_TYPE            const  pDataBuffer,
                                                             EDD_MAC_ADR_TYPE          const  *  const  pDstMacAdr,
                                                             EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             LSA_BOOL                            const  bXCW_DBInitialized)
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIrtProviderBufferAdd");

    //Fcw will still keep Opcode == NOOP here!

    if (!bXCW_DBInitialized)
    {
        //DB0- and Imagemode-fields will be updated again in EDDI_CRTProviderSetBuffer
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__pDB0,
                            DEV_kram_adr_to_asic_register(pDataBuffer, pDDB)); /* 21 Bit */
    }
    EDDI_SetBitField32(&pCCW->FcwAcw.FcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Sync,
                        (ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);    /*  1 Bit */

    EDDI_SetBitField32(&pCCW->FcwAcw.FcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Async,
                        (ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0); /*  1 Bit */

    #if defined (EDDI_CFG_REV7)
    if (pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
    {
        LSA_UINT32  const  FrameID = EDDI_GetBitField32(pCCW->FcwAcw.AcwSnd.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId);

        if (EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3 != FrameID)
        {
            //Only Set EXT-Bit for ASYNC ADATA-FRAMES. SYNC/STANDALONE frames reside in KRAM
            EDDI_SetBitField32(&pCCW->FcwAcw.FcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__EXT, (ImageMode & EDDI_IMAGE_MODE_SYNC)?0:1);
        }
    }

    #if defined(EDDI_CFG_ENABLE_PADDING_CLASS3)
    if (pDDB->NRT.PaddingEnabled)
    {
        //enable padding
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__PAD, 1);
    }
    #endif
    #endif

    EDDI_SERIniMACAdr(pDDB, &pCCW->FcwAcw.FcwSnd.Hw2, pDstMacAdr);
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
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetDestPort( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT32               const  SndPort )
{
    LSA_UINT32  DesPort;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERGetDestPort > ->SndPort:0x%X", SndPort);

    DesPort  = (SndPort & EDDI_CRT_PORT_0) ? 0x01 : 0x00;
    DesPort |= (SndPort & EDDI_CRT_PORT_1) ? 0x02 : 0x00;
    DesPort |= (SndPort & EDDI_CRT_PORT_2) ? 0x04 : 0x00;
    DesPort |= (SndPort & EDDI_CRT_PORT_3) ? 0x08 : 0x00;

    LSA_UNUSED_ARG(pDDB);
    return DesPort;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
* function name: EDDI_SERIniSleepingConsumerFcw()
*
* function:      create structs for IRT-Communication
*                - final init in EDDI_SERIrtConsumerBufferAdd() at a pure Consumer
*                ATTENTION: Function is only used for a pure Consumer (without Forwarding)!
*                ATTENTION: direct-mode:       initialize FCW direct (without help-buffer)!
*                ATTENTION: reset-before-mode: FCW must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniSleepingConsumerFcw( SER_IRT_INI_FCW_TYPE     const  *  const  pComCfg,
                                                               LSA_UINT16                         const  TxPorts,
                                                               EDDI_SER10_CCW_TYPE             *  const  pCCW,
                                                               EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    EDDI_SER10_FCW_RCV_TYPE  *  const  pFcwRcv    = &pCCW->FcwAcw.FcwRcv;
    LSA_UINT32                  const  FcwDataLen = pComCfg->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniSleepingConsumerFcw");

    // Hw0 64 Bit

    //do not change OPC!
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Opc, SER_IRT_DATA);   /*  4 Bit */

    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__IdxEthType, 0x0);            /*  2-Bit */

    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__DstSrcPort, EDDI_SERGetDestPort(pDDB, TxPorts));                                /*  6 Bit */
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 0);  /*  1-Bit */
    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__FrameId, pComCfg->FrameID);                       /* 16 Bit */
    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Len_2_0, FcwDataLen);       /*  3 Bit */
    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__Len_10_3, FcwDataLen >> 3); /*  8 Bit */

    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__pNext, SER10_NULL_PTR);         /* 21 Bit */

    // Hw1 64 Bit

    //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Sync, 0);  //  1 Bit
    //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Async, 0); //  1 Bit

    // Set EnableDiag-Flag depending on TRACE.CfgPara-Settings
    if (pDDB->PRM.TRACE.RcvDataTimeSubEnable.IRTConsumer)
    {
        EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EnDiag, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EnDiag, 0);
    }

    EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_1, EDDI_SER10_LL1_FCW_RCV_BIT2__Time, pComCfg->Time);         /* 32 Bit */

    //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr, 0);

    // Hw2 64 Bit

    //EDDI_SERIniMACAdr(pDDB, &pFcwRcv->Hw2, &NullAddrMAC);

    //EDDI_SetBitField32(&pFcwRcv->Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0);         /* 16 Bit */
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIrtConsumerBufferAdd()
*
* function:      create structs for IRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIrtConsumerBufferAdd( EDDI_SER10_CCW_TYPE               *  const  pCCW,
                                                             EDDI_CRT_CONSUMER_PARAM_PTR_TYPE     const  pLowerParams,
                                                             EDDI_DEV_MEM_U8_PTR_TYPE             const  pDataBuffer,
                                                             EDDI_SER10_IRQ_QUALIFIER_ENUM        const  IrqQualifier,
                                                             EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB )
{
    LSA_UINT32           const  ui32SBTimerAdr = pLowerParams->ui32SBTimerAdr;
    LSA_UINT16           const  WDReloadVal    = (LSA_UINT16)pLowerParams->WDReloadVal;
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LSA_UINT16           const  DHReloadVal    = (LSA_UINT16)pLowerParams->DHReloadVal;
    #endif
    EDD_MAC_ADR_TYPE  *  const  pSrcMAC        = &pLowerParams->SrcMAC;
    LSA_UINT8            const  ImageMode      = pLowerParams->ImageMode;
    #if defined (EDDI_CFG_REV7)
    LSA_UINT32           const  FcwDataLen     = pLowerParams->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS); //needs to be set again as for a DFP-PF the datalen is not known at ADD-time 
    #endif

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIrtConsumerBufferAdd");

    EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__pDB0,
                        DEV_kram_adr_to_asic_register(pDataBuffer, pDDB)); //  21 Bit

    //Set to 1 only, to keep settings for SOL-Dummy-FCW!
    if (ImageMode & EDDI_IMAGE_MODE_SYNC)
    {
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Sync, 1);  // 1 Bit
    }

    //Set to 1 only, to keep settings for SOL-Dummy-FCW!
    if (ImageMode & EDDI_IMAGE_MODE_ASYNC)
    {
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Async, 1);  // 1 Bit
    }

    //Take provisions that the Sync_SlaveTrigger is called directly after reception
    if (IrqQualifier == EDDI_SER10_IRQ_QUALIFIER_RCV1)
    {
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Int1, 1);
        //EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Int2, 0);
    }
    else if (IrqQualifier == EDDI_SER10_IRQ_QUALIFIER_RCV2)
    {
        //EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Int1, 0);
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Int2, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Int1, 0);
        //EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Int2, 0);
    }

    EDDI_SERIniMACAdr(pDDB, &pCCW->FcwAcw.FcwRcv.Hw2, pSrcMAC);

    //Do not check source adress, if MAC adress is 00:00:00:00:00:00
    if (   (pSrcMAC->MacAdr[0] == 0)
        && (pSrcMAC->MacAdr[1] == 0)
        && (pSrcMAC->MacAdr[2] == 0)
        && (pSrcMAC->MacAdr[3] == 0)
        && (pSrcMAC->MacAdr[4] == 0)
        && (pSrcMAC->MacAdr[5] == 0))
    {
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr, 0);
    }
    else
    {
        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr, 1);
    }

    // Hw3 64 Bit
    EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__pTimerSB, ui32SBTimerAdr);

    #if defined (EDDI_CFG_REV7)
    {
        LSA_UINT32  FrameID;

        EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__WDReloadVal, WDReloadVal);

        FrameID = EDDI_GetBitField32(pCCW->FcwAcw.AcwRcv.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId);

        if (EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3 != FrameID)
        {
            //Only Set EXT-Bit for DATA-FRAMES
            if (pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
            {
                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EXT, (ImageMode & EDDI_IMAGE_MODE_SYNC)?0:1);
            }

            EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Len_2_0, FcwDataLen);       /*  3 Bit */
            EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__Len_10_3, FcwDataLen >> 3); /*  8 Bit */

            if (pLowerParams->Partial_DataLen)
            {
                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__PartDataLength,
                                    pLowerParams->Partial_DataLen);

                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__PartDataStart,
                                    pLowerParams->Partial_DataOffset);

                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__PD, 1);
            }
            else
            {
                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__PartDataLength,
                                    0);

                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__PartDataStart,
                                    0);

                EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__PD, 0);
            }
        }
    }
    #else
    EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__WDReloadVal, WDReloadVal);
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__DHReloadVal, DHReloadVal);
    EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__DHStatus, pLowerParams->DataHoldStatus);
    #endif

    //EDDI_SetBitField32(&pCCW->FcwAcw.FcwRcv.Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 0);
    //Local Receiving is switched on with EDDI_RedTreeIrtSetRxLocalActivity() afterwards
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIrtConsumerBufferRemove()
*
* function:      create structs for IRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIrtConsumerBufferRemove( EDDI_SER_CCW_PTR_TYPE    const  pCW,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  DeadLine, Length;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SerIrtConsumerBufferRemove");

    //ATTENTION: FCW is not preset with 0!

    //1) Local Receiving is already switched off with EDDI_RedTreeIrtSetRxLocalActivity() before
    //2) Wait for Deadline of this FCW before we remove BufferPtr and Scoreboard-Ptr
    //   Get Deadline of Frame
    Length = 32;
    Length += EDDI_GetBitField32(pCW->FcwAcw.FcwRcv.Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Len_2_0);
    Length += EDDI_GetBitField32(pCW->FcwAcw.FcwRcv.Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__Len_10_3) << 3;

    DeadLine = (8 * Length) + EDDI_GetBitField32(pCW->FcwAcw.FcwRcv.Hw1.Value.U32_1, EDDI_SER10_LL1_FCW_RCV_BIT2__Time); /* 32 Bit */

    EDDI_RedTreeIrtWaitForDeadline(DeadLine, pDDB);

    //3) Now we can remove all Ptrs
    EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__pDB0, SER10_NULL_PTR);
    //EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Sync,  0);   /*  1 Bit */
    //EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Async, 0);   /*  1 Bit */

    EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__pTimerSB, SER10_NULL_PTR);

    #if defined (EDDI_CFG_REV7)
    //EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__WDReloadVal, 0);
    #else
    //EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__WDReloadVal, 0);
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__DHReloadVal, 0);  /* 13 Bit */
    //EDDI_SetBitField32(&pCW->FcwAcw.FcwRcv.Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__DHStatus, 0);  // 1: Primary  ???? Joerg
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniForwarderFcw()
*
* function:      create structs for IRT-Communication
*                - final init in EDDI_SERIrtConsumerBufferAdd() at a Forwarder-Consumer
*                ATTENTION: Function is used for a pure Forwarder (no local receive) and a Forwarder-Consumer!
*                ATTENTION: direct-mode:       initialize FCW direct (without help-buffer)!
*                ATTENTION: reset-before-mode: FCW must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniForwarderFcw( SER_IRT_INI_FCW_TYPE     const  *  const  pComCfg,
                                                        LSA_UINT32                         const  TxPorts,
                                                        EDDI_SER10_CCW_TYPE             *  const  pCCW,
                                                        EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    EDDI_SER10_FCW_RCV_FORWARDER_TYPE  *  const  pFcwRcvForwarder = &pCCW->FcwAcw.FcwRcvForwarder;
    LSA_UINT32                            const  FcwDataLen       = pComCfg->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniForwarderFcw >");

    // Hw0 64 Bit

    //EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Opc, SER_IRT_DATA);   /*  4 Bit */
    //EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__IdxEthType, 0x0);            /*  2-Bit */

    EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__DstSrcPort, EDDI_SERGetDestPort(pDDB, TxPorts));  /*  6 Bit */
    //EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 0);                  /*  1-Bit */
    EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__FrameId, pComCfg->FrameID);   /* 16 Bit */

    EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Len_2_0, FcwDataLen);       /*  3 Bit */

    EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__Len_10_3, FcwDataLen >> 3); /*  8 Bit */

    EDDI_SetBitField32(&pFcwRcvForwarder->Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__pNext, SER10_NULL_PTR);     /* 21 Bit */

    // Hw1 64 Bit

    EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__pDB0, SER10_NULL_PTR); /* 21 Bit */

    //EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Sync, 0);    /*  1 Bit */
    //EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Async, 0);    /*  1 Bit */

    //Set EnableDiag-Flag depending on TRACE.CfgPara-Settings
    if (pDDB->PRM.TRACE.RcvDataTimeSubEnable.IRTForwarder)
    {
        EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EnDiag, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EnDiag, 0);
    }

    EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_1, EDDI_SER10_LL1_FCW_RCV_BIT2__Time, pComCfg->Time);         /* 32 Bit */

    //EDDI_SetBitField32(&pFcwRcvForwarder->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr, 0);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*
 * Nimmt ein vorbereitetes lokales ControlWord (pCCWLocal) entgegen.
 * Bildet eine SW-Vertreter und ...
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERCcwAdd( EDDI_SER_CCW_PTR_TYPE           pCCWLocal,
                                               EDDI_CCW_CTRL_HEAD_PTR_TYPE     pCCWHead,
                                               EDDI_CCW_CTRL_PTR_TYPE       *  ppCCWCtrl,
                                               EDDI_LOCAL_DDB_PTR_TYPE         pDDB )
{
    EDDI_SER_CCW_PTR_TYPE  pCCWDev;
    LSA_UINT32             hKramPool, hCtrlPool;
    LSA_UINT32             Size;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERCcwAdd");

    if (pCCWHead == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SERCcwAdd-  pCCWHead", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //Size = SERCcwSize((LSA_UINT16)pCCWLocal->FcwAcw.Common.---.Opc);

    // Get MemPools for KRAM-FCW and Ctrl-CCW
    switch (EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc))
    {
        case SER_ACW_RX:
        {
            hCtrlPool = pDDB->CRT.hPool.CCWCTRL;
            hKramPool = pDDB->CRT.hPool.AcwConsumer;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_SERCcwAdd", EDDI_FATAL_ERR_EXCP, EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc), 0);
            return;
        }
    }

    // Get Memory and Size for KRAM-FCW
    EDDI_MemGetPoolBuffer(hKramPool, (void * *)&pCCWDev);
    Size = EDDI_MemGetPoolBuffersize(hKramPool);
    // duch EDDI_MemGetPoolBuffer EDDI_MemSet(pCCWDev, 0, Size );

    // Get Memory for CCW Control.
    EDDI_MemGetPoolBuffer(hCtrlPool, (void * *)ppCCWCtrl);

    // Initialize CCW Control.
    (*ppCCWCtrl)->Opc     = (LSA_UINT16)EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc);
    (*ppCCWCtrl)->Size    = (LSA_UINT16)Size;
    // Note: This cast is not type safe.
    (*ppCCWCtrl)->pKramCW = (EDDI_SER10_FCW_ACW_PTR_TYPE)pCCWDev;  /* stored ptr to KRAM FCW/ACW */
    (*ppCCWCtrl)->pNext   = EDDI_NULL_PTR;

    // Initialize Control-FCW.
    switch (EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc))
    {
        case SER_ACW_RX:
        {
            //      (*ppCCWCtrl)->Time         = (LSA_UINT32)pCCWLocal->FcwAcw.AcwSnd.Hw1.---.Time;
            (*ppCCWCtrl)->FrameId  =
                (LSA_UINT16)EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId);

            (*ppCCWCtrl)->DataLen  =
                (LSA_UINT16)EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0);

            (*ppCCWCtrl)->DataLen |=
                (LSA_UINT16)(EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3) << 3);
            break;
        }

        case SER_EOL:
        case SER_EOC_ID:
        case SER_SOL:
        case SER_SOC_ID:
        case SER_ACW_TX: // not allowed any more --> Will be added in ser_tree now!!
        default:
        {
            EDDI_Excp("EDDI_SERCcwAdd, Unsupportet Opcode!", EDDI_FATAL_ERR_EXCP, EDDI_GetBitField32(pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc), 0);
            return;
        }
    }

    // Insert the CCW into the FCW-list.
    EDDI_SERCcwAddToList(pCCWHead, pCCWLocal, pCCWDev, *ppCCWCtrl, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


//////////////////////////////////////////////////////////////////////////////
///
/// Function is implemented only for deleting without searching a CCWCtrl.
/// It means that pCCWctrl must be different than EDDI_NULL_PTR.
///
/// @param pCCWCtrl                 pointer to the CCWCtrl which is to be deleted from the list
/// @param pKRAMSmallApduBuffer     pointer to a APDU-Buffer or EDDI_NULL_PTR
///
//////////////////////////////////////////////////////////////////////////////
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERCcwRemove( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                  EDDI_CCW_CTRL_PTR_TYPE      const  pCCWCtrl,
                                                  void                     *  const  pKRAMSmallApduBuffer )
{
    LSA_UINT32                          AdrPrevCCW;
    EDDI_CCW_CTRL_HEAD_PTR_TYPE  const  pCCWHead = pCCWCtrl->pCCWHead;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERCcwRemove");

    if (pCCWCtrl == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SERCcwRemove pCCWCtrl", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (pCCWHead == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SERCcwRemove pCCWCtrl->pCCWHead", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (pCCWHead->pTop == pCCWCtrl)
    {
        // Removing the first element of the list
        // requires the list header to be updated.
        AdrPrevCCW = DEV_kram_adr_to_asic_register(pCCWHead->pKramListHead, pDDB) | PREV_PTR_IS_LIST_HEADER;
    }
    else
    {
        // It's not the first element.
        AdrPrevCCW = DEV_kram_adr_to_asic_register(pCCWCtrl->pPrev->pKramCW, pDDB);
    }

    switch (pCCWCtrl->Opc)
    {
        case SER_ACW_RX:
        {
            EDDI_SERSingleCommand(FCODE_REMOVE_ACW_RX, AdrPrevCCW, 0UL, pDDB);
            break;
        }

        case SER_ACW_TX:
        case SER_EOL:
        case SER_EOC_ID:
        case SER_SOL:
        case SER_SOC_ID:
        default:
            EDDI_Excp("EDDI_SERCcwRemove pCCWCtrl->Opc 0", EDDI_FATAL_ERR_EXCP, pCCWCtrl->Opc, 0);
            return;
    }

    // Update the CCWCtrl list
    if (LSA_HOST_PTR_ARE_EQUAL(pCCWCtrl, pCCWHead->pTop))
    {
        // The first element has to be deleted
        pCCWHead->pTop = pCCWCtrl->pNext;
        if (!(pCCWCtrl->pNext == EDDI_NULL_PTR))
        {
            // The deleted element has a successor.
            pCCWCtrl->pNext->pPrev = EDDI_NULL_PTR;
        }
        else
        {
            // The list is empty now.
            pCCWHead->pBottom = EDDI_NULL_PTR;
            (*pCCWHead->pKramListHead) &= ~(SER10_PTR_VALID_SWAP);
        }
    }
    else
    {
        if (!(pCCWCtrl->pNext == EDDI_NULL_PTR))
        {
            // The CCW to be deleted is not the last element in the list.
            pCCWCtrl->pNext->pPrev = pCCWCtrl->pPrev;
            pCCWCtrl->pPrev->pNext = pCCWCtrl->pNext;
        }
        else
        {
            // Delete the last element of the list.
            pCCWCtrl->pPrev->pNext = EDDI_NULL_PTR;
            pCCWHead->pBottom      = pCCWCtrl->pPrev;
        }
    }

    // Free Resources
    switch (pCCWCtrl->Opc)
    {
        case SER_ACW_RX:
        {
            EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.AcwConsumer, pCCWCtrl->pKramCW);
            EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.CCWCTRL,     pCCWCtrl);
            if (pKRAMSmallApduBuffer != EDDI_NULL_PTR)
            {
                EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.ApduAuxConsumer, pKRAMSmallApduBuffer);
            }
            break;
        }

        default:
            EDDI_Excp("EDDI_SERCcwRemove pCCWCtrl->Opc", EDDI_FATAL_ERR_EXCP, pCCWCtrl->Opc, 0);
            return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*
 * Insert a control word (FCW, EOL, EOC, SOL, SOC, BCW_ID, BCW_MOD,
 * LMW, BMW into the corresponding FCW-list.
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERCcwAddToList( EDDI_CCW_CTRL_HEAD_PTR_TYPE  const  pCCWHead,
                                                             EDDI_SER_CCW_PTR_TYPE        const  pCCWLocal,
                                                             EDDI_SER_CCW_PTR_TYPE        const  pCCWDev,
                                                             EDDI_CCW_CTRL_PTR_TYPE       const  pCCWCtrl,
                                                             EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_UINT32  pPrevCCW;
    LSA_UINT32  pNewCCW;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERCcwAddToList");

    // The head pointer is needed when removing the CCW.
    pCCWCtrl->pCCWHead = pCCWHead;

    // Insert element (CCW: FCW, ACW, EOL, EOC, SOL, SOC, BCW_ID, BCW_MOD, LMW, BMW)
    // Check if this is the first element of the list
    if (   (pCCWHead->pBottom == EDDI_NULL_PTR)
        || (pCCWHead->pTop    == EDDI_NULL_PTR))
    {
        if (pCCWHead->pBottom != pCCWHead->pTop)
        {
            EDDI_Excp("EDDI_SERCcwAddToList 0", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        if ((*pCCWHead->pKramListHead) != SER10_NULL_PTR_SWAP)
        {
            EDDI_Excp("EDDI_SERCcwAddToList 1", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        // First element of Ctrl-List
        pCCWHead->pBottom = pCCWCtrl;
        pCCWHead->pTop    = pCCWCtrl;

        // Insert as NOOP, Opcode will be set later to SER_ACW_RX in ConsumerControl
        EDDI_SetBitField32(&pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);

        // Copy local CCW into device CCW.
        DEV32_MEMCOPY(pCCWDev, pCCWLocal, pCCWCtrl->Size);

        // Set a new list header.
        // Using the command interface for SER_ACW_TX and SER_ACW_RX.
        switch (pCCWCtrl->Opc)
        {
            case SER_ACW_RX:
            {
                pPrevCCW = DEV_kram_adr_to_asic_register(pCCWHead->pKramListHead, pDDB) | PREV_PTR_IS_LIST_HEADER;
                pNewCCW  = DEV_kram_adr_to_asic_register(pCCWDev, pDDB);
                EDDI_SERCommand(FCODE_INSERT_ACW_RX, pPrevCCW, pNewCCW, 0, pDDB);
                break;
            }

            default:
            {
                EDDI_Excp("EDDI_SERCcwAddToList Unhandled Opcode", EDDI_FATAL_ERR_EXCP, pCCWCtrl->Opc, 0);
                return;
            }
        }

        // Do not forget to set the Valid-Bit in Listheader
        (*pCCWHead->pKramListHead) |= SER10_PTR_VALID_SWAP;
        return;
    }

    // Only a linear list (no branch) has to be handled.
    switch (pCCWCtrl->Opc)
    {
        case SER_ACW_RX:
        {
            EDDI_SERAddRxAcw(pCCWHead, pCCWLocal, pCCWDev, pCCWCtrl, pDDB);
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_SERCcwAddToList 3", EDDI_FATAL_ERR_EXCP, pCCWCtrl->Opc, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*---------------------------------------------------------------------------*/
/* ACW-Functions                                                             */
/*---------------------------------------------------------------------------*/

/*=============================================================================
* function name: EDDI_SERIniProviderACW()
*
* function:      ini the ACW for IRT/SRT-Communication
*                ATTENTION: direct-mode: initialize ACW direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniProviderACW( EDDI_SER10_FCW_ACW_PTR_TYPE             pFcwAcw,
                                                 const EDDI_CRT_PROVIDER_PARAM_TYPE  *  const  pLowerParams,
                                                       LSA_BOOL                         const  bActive,
                                                       LSA_BOOL                         const  bInitBuf,
                                                       EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB )
{
    EDDI_SER10_ACW_SND_TYPE  *  const  pAcwSnd    = &pFcwAcw->AcwSnd;
    LSA_UINT32                  const  AcwDataLen = pLowerParams->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniProviderACW");

    EDDI_MemSet(pAcwSnd, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_ACW_SND_TYPE));

    // Hw0 64 Bit DEFAULT

    if (bActive)
    {
        EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_ACW_TX);
    }
    else
    {
        EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);
        // ON initialization Provider will be inactive first !!     /*  4 Bit */
        // Opcode will later be set to SER_ACW_TX with ProviderCtrl-Service.
    }

    if (pLowerParams->SrtRawPortMask != 0)
    {
        // Special Feature To Test SRTRawMode
        EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__DstSrcPort, pLowerParams->SrtRawPortMask);
        EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Raw, 1);
    }

    //EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__IdxEthType, 0x0);                                  /*  2-Bit */
    EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId, pLowerParams->FrameId);                /* 16 Bit */

    EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0, AcwDataLen);   /*  3 Bit */

    EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, AcwDataLen >> 3); /*  8 Bit */

    EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, SER10_NULL_PTR);                       /* 21 Bit */

    // Hw1 64 Bit

    //bInitbuf is only TRUE when changing the phase!
    if (bInitBuf)
    {
        //set bufferparams only when changing the phase
        #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        if (EDD_DATAOFFSET_UNDEFINED != pLowerParams->DataOffset)
        {
            LSA_UINT32  length = pLowerParams->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    
            // Preserve 4 Byte-Alignment
            length = (length + 0x03) & ~0x00000003;
    
            if (EDDI_RTC3_AUX_PROVIDER == pLowerParams->ProviderType)
            {
                //3 buffers are not necessary for AUX, use only buffer #0
                EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                                    DEV_kram_adr_to_asic_register((pLowerParams->pKRAMDataBuffer), pDDB)); /* 21 Bit */
            }
            else
            {
                //the user provided a set of 3 buffers, #0 is USER, #1 is DATA (set to DB0 in ACW), #2 is FREE
                EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                                    DEV_kram_adr_to_asic_register((pLowerParams->pKRAMDataBuffer + length), pDDB)); /* 21 Bit */
            }
        }
        #else
        EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                            DEV_kram_adr_to_asic_register(pLowerParams->pKRAMDataBuffer, pDDB)); /* 21 Bit */
        #endif
    }

    //EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Int2, 0); // 1 will trigger an interrupt
    //EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Int1, 0); // 1 will trigger an interrupt

    //image mode will be updated again in EDDI_CRTProviderSetBuffer
    EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Sync,
                        (pLowerParams->ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);   /*  1 Bit */

    EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Async,
                        (pLowerParams->ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0);  /*  1 Bit */

    //EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_SND_BIT2__Time, 0);

    // Hw2 64 Bit

    EDDI_SERIniMACAdr(pDDB, &pAcwSnd->Hw2, &pLowerParams->DstMAC);

    EDDI_SetBitField32(&pAcwSnd->Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0xC000); /*  VlanId=0 + Prio6; */              /*  8 Bit */

    EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Tag, 1); // TagMode for SRT is always enabled !!

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pLowerParams->ProviderType == EDDI_RTC3_AUX_PROVIDER)
    {
        //do not use 3B-Interface!
    }
    else
    {
        //no 3B-IF!
        EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__IIIB, 0);
        EDDI_SetBitField32(&pAcwSnd->Hw34.Value.U32_0, EDDI_SER10_LLx_3B_IF_BIT__FundDB0, SER_FUND_FREE);  // 11b -> D (Data)
        EDDI_SetBitField32(&pAcwSnd->Hw34.Value.U32_0, EDDI_SER10_LLx_3B_IF_BIT__FundDB1, SER_FUND_FREE);  // 01b -> U (User)
        EDDI_SetBitField32(&pAcwSnd->Hw34.Value.U32_0, EDDI_SER10_LLx_3B_IF_BIT__FundDB2, SER_FUND_FREE);  // 00b -> F (Free)
    }
    #endif

    #if defined (EDDI_CFG_REV7)
    /* Grouping ab Rev7 */
    EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__GN_Snd, 0);

    // Point pDB0 to PAEA-RAM
    // will be changed again in EDDI_CRTProviderSetBuffer if it is an AUX prov. with a SYNC-RTC3 prov.
    EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__EXT, (pDDB->CRT.PAEA_Para.PAEA_BaseAdr) ?1:0);

    if (pLowerParams->RT_Type == EDDI_RT_TYPE_UDP)
    {
        EDDI_SetBitField32(&pAcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__UDP, 1);
        //EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__EXT, 0);
        //EDDI_SetBitField32(&pAcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__RMA_Snd, 0);
        //EDDI_SetBitField32(&pAcwSnd->Hw3.Value.U32_1, EDDI_SER10_LLx_UDP_ACW_SND_BIT2__IP_Select, 0);  // IP - Adresss will be taken from Local_IP_0
        //EDDI_SetBitField32(&pAcwSnd->Hw3.Value.U32_1, EDDI_SER10_LLx_UDP_ACW_SND_BIT2__UP_Select, 0);  // UDP_PORT0_Register

        EDDI_SetBitField32(&pAcwSnd->Hw3.Value.U32_0, EDDI_SER10_LLx_UDP_ACW_SND_BIT__DestIPAddr, EDDI_HTONL(pLowerParams->UPD_DstIPAdr.dw));

        EDDI_SetBitField32(&pAcwSnd->Hw3.Value.U32_1, EDDI_SER10_LLx_UDP_ACW_SND_BIT2__UDPDestPort, EDDI_ACW_xRT_UDP_PORT);
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniRootAcw()
*
* function:      ini the ACW for SRT-Communication, used to set the Begintime
*                of iSRT/SRT-Transferphase
*                ATTENTION: indirect-mode: initialize ACW indirect (via help-buffer) and copy it!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniRootAcw( EDDI_SER10_ACW_SND_TYPE  *  const  pAcwSnd,
                                                   LSA_UINT8                *  const  pDummyBuffer,
                                                   EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDD_MAC_ADR_TYPE         DstMac;
    EDDI_SER10_ACW_SND_TYPE  LocalAcwSnd;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniRootAcw");

    EDDI_MemSet(&LocalAcwSnd, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_ACW_SND_TYPE));

    //Hw0 64 Bit

    EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_ACW_TX);

    //EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__DstSrcPort, 0);
    //EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Raw, 0);

    //EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__IdxEthType , 0);                                  /*  2-Bit */
    EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId, 0xEEEE);                       /* 16 Bit */

    EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0, 4);      /*  3 Bit */
    //EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, 4 >> 3); /*  8 Bit */
    EDDI_SetBitField32(&LocalAcwSnd.Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, SER10_NULL_PTR);                       /* 21 Bit */

    //Hw1 64 Bit

    EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                        DEV_kram_adr_to_asic_register(pDummyBuffer, pDDB)); /* 21 Bit */

    //EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Int2, 0); // 1 löst Interrupt aus
    //EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Int1, 0); // 1 löst Interrupt aus

    EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Sync, 1);
    EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Async, 1);

    //EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_SND_BIT2__Time, 0);

    //We intentionally send to our own MAC-Adress !!
    //This is a special trick to get a RootAcw to delay the processing of all following TX-ACWs.
    //No Frame will be produced by this ACW.

    //Hw2 64 Bit

    //A DstMac is used, that is filtered via mac-table!
    DstMac = EDDI_SwiPNGetRT3DA(pDDB);
    EDDI_SERIniMACAdr(pDDB, &LocalAcwSnd.Hw2, &DstMac);

    //EDDI_SetBitField32(&LocalAcwSnd.Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0); /*  VlanId + Prio6;  */             /*  8 Bit */

    //EDDI_SetBitField32(&LocalAcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__Tag, 0); // TagMode für SRT ist immer an !!

    DEV32_MEMCOPY((void *)pAcwSnd, (void *)&LocalAcwSnd, sizeof(EDDI_SER10_ACW_SND_TYPE));
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERUpdateRootAcw()
*
* function:      Update Time of RootAcw
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERUpdateRootAcw( EDDI_SER10_ACW_SND_TYPE  *  const  pAcw,
                                                      LSA_UINT32                  const  TimeTicks_10ns,
                                                      EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_UINT32  CycleLength_10ns;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERUpdateRootAcw");

    CycleLength_10ns = pDDB->CycCount.CycleLength_10ns;

    if (TimeTicks_10ns >= CycleLength_10ns)
    {
        EDDI_Excp("EDDI_SERUpdateRootAcw TimeTicks >= CycleLengthTicks", EDDI_FATAL_ERR_EXCP, TimeTicks_10ns, CycleLength_10ns);
        return;
    }

    EDDI_SetBitField32(&pAcw->Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_SND_BIT2__Time, TimeTicks_10ns);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniConsumerACW()
*
* function:      ini the ACW for IRT/SRT-Communication
*                ATTENTION: direct-mode: initialize ACW direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniConsumerACW( EDDI_SER10_CCW_TYPE               *  const  pCCW,
                                                       EDDI_CRT_CONSUMER_PARAM_PTR_TYPE     const  pLowerParams,
                                                       EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB )
{
    EDDI_SER10_ACW_RCV_TYPE  *  const  pAcwRcv    = &pCCW->FcwAcw.AcwRcv;
    LSA_UINT32                  const  AcwDataLen = pLowerParams->DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniConsumerACW");

    EDDI_MemSet(pAcwRcv, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_ACW_RCV_TYPE));

    // Hw0 64 Bit    DEFAULT

    EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc,  SER_ACW_RX);                  /*  4 Bit */
    //EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__IdxEthType, 0x0);                 /*  2-Bit */
    EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId, pLowerParams->FrameId);    /* 16 Bit */

    EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0, AcwDataLen);               /*  3 Bit */

    EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, AcwDataLen >> 3);        /*  8 Bit */

    EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, SER10_NULL_PTR);            /* 21 Bit */

    // Hw1 64 Bit

    EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__pDB0,
                        DEV_kram_adr_to_asic_register(pLowerParams->pKRAMDataBuffer, pDDB));                      /* 21 Bit */

    EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__Sync,
                        (pLowerParams->ImageMode & EDDI_IMAGE_MODE_SYNC) ? 1:0);                                  /*  1 Bit */

    EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__Async,
                        (pLowerParams->ImageMode & EDDI_IMAGE_MODE_ASYNC) ? 1:0);                                 /*  1 Bit */

    if (   (pLowerParams->SrcMAC.MacAdr[0] == 0)
        && (pLowerParams->SrcMAC.MacAdr[1] == 0)
        && (pLowerParams->SrcMAC.MacAdr[2] == 0)
        && (pLowerParams->SrcMAC.MacAdr[3] == 0)
        && (pLowerParams->SrcMAC.MacAdr[4] == 0)
        && (pLowerParams->SrcMAC.MacAdr[5] == 0))
    {
        EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__CSrcAdr, 0);
    }
    else
    {
        EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__CSrcAdr, 1);
    }

    // Hw2 64 Bit

    EDDI_SERIniMACAdr(pDDB, &pAcwRcv->Hw2, &pLowerParams->SrcMAC);

    //EDDI_SetBitField32(&pAcwRcv->Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0); /* only used by Producers; ...*/  /*  8 Bit */

    // Set EnableDiag-Flag depending on TRACE.CfgPara-Settings
    if (pDDB->PRM.TRACE.RcvDataTimeSubEnable.SRTConsumer)
    {
        EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__EnDiag, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__EnDiag, 0);
    }

    // Hw3 64 Bit

    EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__pTimerSB, pLowerParams->ui32SBTimerAdr);      /* 21 Bit */

    #if defined (EDDI_CFG_REV7)
    EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__WDReloadVal, pLowerParams->WDReloadVal);
    #else
    EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__WDReloadVal, pLowerParams->WDReloadVal);
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pLowerParams->DataHoldFactor > 1)
    {
        // Setting the DataHoldReload Value
        EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__DHReloadVal, pLowerParams->DHReloadVal);
        EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__DHStatus, pLowerParams->DataHoldStatus);
    }

    //Controller-Controller communication - partial data access
    //length 0 cannot be set here!
    if (pLowerParams->Partial_DataLen)
    {
        EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_RCV_BIT2__PD, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_RCV_BIT2__PD, 0);
    }

    EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_RCV_BIT2__PartDataStart, pLowerParams->Partial_DataOffset);
    EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_1, EDDI_SER10_LL1_ACW_RCV_BIT2__PartDataLength, pLowerParams->Partial_DataLen);

    //EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__DHStatus, 0);
    //EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__Again,       0);

    //EDDI_SetBitField32(&pAcwRcv->Hw3.Value.U32_1, EDDI_SER10_LL3_xCW_RCV_BIT2__DHReloadVal, 0);
    #endif //EDDI_CFG_REV6 or EDDI_CFG_REV7

    #if defined (EDDI_CFG_REV7)
    if (   (pLowerParams->ConsumerType == EDDI_RTC3_AUX_CONSUMER)
        && (EDD_DATAOFFSET_UNDEFINED == pLowerParams->DataOffset) )
    {
        //do not use 3B-Interface!
        //APDU-Buffer for EDDI_RTC3_AUX_CONSUMER must be single-buffered and is managed by EDD
    }
    else //ConsumerType != EDDI_RTC3_AUX_CONSUMER
    {
        if (pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
        {
            //Activate user-data in PAEA-RAM
            //RTC1/2 consumers always reside in PAEARAM, except AUX
            EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__EXT, (pLowerParams->ConsumerType == EDDI_RTC3_AUX_CONSUMER)?0:1);
        }
        else
        {
            //Buffer in KRAM belongs to standalone image
        }
    }

    //Grouping ab Rev7
    EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__GN_Rcv, 0);
      
    EDDI_SetBitField32(&pLowerParams->pSBTimerEntry->Reg[0], EDDI_SER10_TIMER_SCORB_BIT__SCRR_ReloadValue, pLowerParams->SCRR_ReloadValue);

    if (pLowerParams->RT_Type == EDDI_RT_TYPE_UDP)
    {
        EDDI_SetBitField32(&pAcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__UDP, 1);

        //EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__EXT, 0);
        //EDDI_SetBitField32(&pAcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__RMA_Rcv, 0);

        EDDI_SetBitField32(&pAcwRcv->Hw4.Value.U32_1, EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__CheckSrcIPAddr, 1);
        EDDI_SetBitField32(&pAcwRcv->Hw4.Value.U32_1, EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__CheckSrcUDPPort, 1);

        EDDI_SetBitField32(&pAcwRcv->Hw4.Value.U32_0, EDDI_SER10_LLx_UDP_ACW_RCV_BIT__SourceIPAddr,
                            EDDI_HTONL(pLowerParams->SrcIP.dw));

        EDDI_SetBitField32(&pAcwRcv->Hw4.Value.U32_1, EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__UDPSourcePort, EDDI_ACW_xRT_UDP_PORT);
    }
    #endif //EDDI_CFG_REV7
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniNOOP()
*
* function:      Initialize a NOOP for IRT/SRT-Communication
*                ATTENTION: direct-mode: initialize NOOP direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniNOOP( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                EDDI_LOCAL_NOOP_PTR_TYPE  const  pNOOP )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniNOOP->");

    //initialize NOOP direct: 64 Bit

    //regular: EDDI_SetBitField32(&LocalNOOP.Value.U32_0, EDDI_SER10_NOOP_BIT__Opc, SER_NOOP);   /* 4 Bit */
    //regular: EDDI_SetBitField32(&LocalNOOP.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, SER10_NULL_PTR);   /* 21 Bit */

    pNOOP->Value.U32_0 = EDDI_HOST2IRTE32(SER_NOOP);
    pNOOP->Value.U32_1 = EDDI_HOST2IRTE32(SER10_NULL_PTR_HELP);
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniEOL()
*
* function:      Initialize a EOL for IRT/SRT-Communication
*                ATTENTION: direct-mode: initialize EOL direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniEOL( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                               EDDI_LOCAL_EOL_PTR_TYPE  const  pEOL )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniEOL->");

    //Initialize EOL direct: 64 Bit

    //regular: EDDI_SetBitField32(&LocalEOL.Value.U32_0, EDDI_SER10_EOL_BIT__Opc, SER_EOL);   /* 4 Bit */
    //regular: EDDI_SetBitField32(&LocalEOL.Value.U32_1, EDDI_SER10_EOL_BIT2__pNext, SER10_NULL_PTR);   /* 21 Bit */

    pEOL->Value.U32_0 = EDDI_HOST2IRTE32(SER_EOL);
    pEOL->Value.U32_1 = EDDI_HOST2IRTE32(SER10_NULL_PTR_HELP);

    //Time can be changed with EDDI_SERUpdateEOL()!
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERUpdateEOL()
*
* function:      Update a EOL for IRT/SRT-Communication
*                ATTENTION: direct-mode: update EOL direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERUpdateEOL( EDDI_SER_EOL_PTR_TYPE  const  pEOL,
                                                  LSA_UINT32             const  Time )
{
    EDDI_SetBitField32(&pEOL->Value.U32_0, EDDI_SER10_EOL_BIT__Time_21_0,   Time);
    EDDI_SetBitField32(&pEOL->Value.U32_1, EDDI_SER10_EOL_BIT2__Time_31_22, (Time >> 22));
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_RED_PHASE_SHIFT_ON)

/*=============================================================================
* function name: EDDI_SERIniSOL()
*
* function:      Initialize a SOL for IRT/SRT-Communication
*                ATTENTION: direct-mode: initialize SOL direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniSOL( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       EDDI_LOCAL_SOL_PTR_TYPE  const  pSOL )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniSOL");

    //initialize SOL direct: 64 Bit

    //regular: EDDI_SetBitField32(&LocalSOL.Value.U32_0, EDDI_SER10_SOL_BIT__Opc, SER_SOL);  /*  4 Bit */
    //regular: EDDI_SetBitField32(&LocalSOL.Value.U32_1, EDDI_SER10_SOL_BIT2__pNext, SER10_NULL_PTR);  /* 21 Bit */

    pSOL->Value.U32_0 = EDDI_HOST2IRTE32(SER_SOL);
    pSOL->Value.U32_1 = EDDI_HOST2IRTE32(SER10_NULL_PTR_HELP);

    //Time can be changed with EDDI_SERUpdateSOL()!
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERUpdateSOL()
*
* function:      Update a SOL for IRT/SRT-Communication
*                ATTENTION: direct-mode: update SOL direct (without help-buffer)!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERUpdateSOL( EDDI_LOCAL_SOL_PTR_TYPE  const  pSOL,
                                                  LSA_UINT32               const  Time )
{
    EDDI_SetBitField32(&pSOL->Value.U32_0, EDDI_SER10_SOL_BIT__Time_21_0,   Time);
    EDDI_SetBitField32(&pSOL->Value.U32_1, EDDI_SER10_SOL_BIT2__Time_31_22, (Time >> 22));
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniDummyTxFcw()
*
* function:      create structs for IRT-Communication
*                ATTENTION: direct-mode:       initialize TX-FCW direct (without help-buffer)!
*                ATTENTION: reset-before-mode: TX-FCW must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniDummyTxFcw( EDDI_SER10_FCW_SND_TYPE   *  const  pFcwSnd,
                                                              EDDI_DEV_MEM_U8_PTR_TYPE     const  pDataBuffer,
                                                              EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDD_MAC_ADR_TYPE         DstMac;
    LSA_UINT32        const  FcwDataLen = 100; //do not change!

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniDummyTxFcw");

    //Hw0 64 Bit

    //opcode is already set to SER_IRT_DATA
    //EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_IRT_DATA);
    //EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__IdxEthType , 0x0);           /*  2-Bit */
    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId, 0x998);         /* 16 Bit */

    //100 Bytes (> 64 Bytes) must be send to avoid an HW-Bug with SOL-shifting in connection with irt-forwarding (internal 64-Byte-Buffer)
    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0, FcwDataLen);   /*  3 Bit */
    EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, FcwDataLen >> 3); /*  8 Bit */

    //EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__AppStatSel , 0);            /*  3 Bit */

    //pNext is set later!
    //EDDI_SetBitField32(&pFcwSnd->Hw0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, SER10_NULL_PTR);         /* 21 Bit */

    //Hw1 64 Bit

    EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__pDB0, DEV_kram_adr_to_asic_register(pDataBuffer, pDDB)); /* 21 Bit */

    //Dummy-Buffer is not Part of the ProcessImage!
    EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Sync, 1);  //1 Bit
    EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__Async, 1); //1 Bit

    //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_1, EDDI_SER10_LL1_FCW_SND_BIT2__Time, 0);         /* 32 Bit */

    //Hw2 64 Bit

    DstMac = EDDI_SwiPNGetRT3INVALIDDA(pDDB);
    EDDI_SERIniMACAdr(pDDB, &pFcwSnd->Hw2, &DstMac);

    //EDDI_SetBitField32(&pFcwSnd->Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0);         /* 16 Bit */

    //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__DelayCtr, 0);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pDDB->IRT.GlobalDestination)
    {
        EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__GlobDest, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pFcwSnd->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_SND_BIT__GlobDest, 0);
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniDummyRxFcw()
*
* function:      create structs for IRT-Communication
*                ATTENTION: direct-mode:       initialize RX-FCW direct (without help-buffer)!
*                ATTENTION: reset-before-mode: RX-FCW must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniDummyRxFcw( EDDI_SER10_FCW_RCV_TYPE   *  const  pFcwRcv,
                                                              EDDI_DEV_MEM_U8_PTR_TYPE     const  pDataBuffer,
                                                              EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERIniDummyRxFcw");

    //Hw0 64 Bit

    //opcode is already set to SER_IRT_DATA (= 0)
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Opc, SER_IRT_DATA);          /*  4 Bit */
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__IdxEthType, 0x0);            /*  2-Bit */
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__DstSrcPort, 0);              /*  6 Bit */
    #if defined (EDDI_OLD_RED_RX_SHIFT_LIST_VERSION)
    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 1);                   /*  1-Bit */
    #else
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 0);                   /*  1-Bit */
    #endif
    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__FrameId, 0x999);  /* 16 Bit */
    EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Len_2_0, 4);       /*  3 Bit */  //do not change!
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__Len_10_3, 4 >> 3); /*  8 Bit */

    //pNext is set later!
    //EDDI_SetBitField32(&pFcwRcv->Hw0.Value.U32_1, EDDI_SER10_LL0_FCW_RCV_BIT2__pNext, SER10_NULL_PTR);         /* 21 Bit */

    EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__pDB0, DEV_kram_adr_to_asic_register(pDataBuffer, pDDB)); /* 21 Bit */

    //Hw1 64 Bit

    //Dummy-Buffer is not Part of the ProcessImage!
    EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Sync, 1);  //1 Bit
    EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__Async, 1); //1 Bit

    //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__EnDiag, 0);

    //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_1, EDDI_SER10_LL1_FCW_RCV_BIT2__Time, 0);         /* 32 Bit */

    #if defined (EDDI_OLD_RED_RX_SHIFT_LIST_VERSION)
    EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr, 1);
    #else
    //EDDI_SetBitField32(&pFcwRcv->Hw1.Value.U32_0, EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr, 0);
    #endif

    //Hw2 64 Bit

    //EDDI_SERIniMACAdr(pDDB, &pFcwRcv->Hw2, &NullAddrMAC);

    //EDDI_SetBitField32(&pFcwRcv->Hw2.Value.U32_1, EDDI_SER10_LL2_COMMON_BIT2__Vlan, 0);         /* 16 Bit */

    //Hw3 64 Bit

    EDDI_SetBitField32(&pFcwRcv->Hw3.Value.U32_0, EDDI_SER10_LL3_xCW_RCV_BIT__pTimerSB, SER10_NULL_PTR);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniIrtPortStartTimeTxBlock()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniIrtPortStartTimeTxBlock( EDDI_LOCAL_DDB_PTR_TYPE                            const  pDDB,
                                                             const EDDI_TREE_TYPE                                  *  const  pTree,
                                                                   LSA_UINT32                                         const  HwPortIndex,
                                                                   EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE     const  pIrtPortStartTimeTxBlock )
{
    LSA_UINT32                                              pCwAsic;
    EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE            LocalIrtPortStartTimeTxBlock;
    EDDI_CRT_PHASE_TX_MACHINE                     *  const  pMachine     = &pDDB->pLocal_CRT->PhaseTx[HwPortIndex];
    EDDI_DEV_MEM_U8_PTR_TYPE                         const  pFrameBuffer = (EDDI_DEV_MEM_U8_PTR_TYPE)(pDDB->IRTE_SWI_BaseAdr + 0x00100000UL);  //use start of KRAM for send-buffer

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERIniIrtPortStartTimeTxBlock");

    EDDI_MemSet(&LocalIrtPortStartTimeTxBlock, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE));

    EDDI_SERIniLMW(pDDB, &LocalIrtPortStartTimeTxBlock.Lmw);
    EDDI_SERIniDummyTxFcw(&LocalIrtPortStartTimeTxBlock.FcwSnd, pFrameBuffer, pDDB);
    EDDI_SERIniSOL(pDDB, &LocalIrtPortStartTimeTxBlock.Sol);

    //connect elements
    //LMW is connected with FCW later!
    EDDI_SERConnectCwToNext(pDDB,
                            (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&LocalIrtPortStartTimeTxBlock.FcwSnd,  //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
                            (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pIrtPortStartTimeTxBlock->Sol);       //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
    //SOL is connected with EOLmax later!

    //complete LMW
    //select CW to be modified by LMW
    pCwAsic = DEV_kram_adr_to_asic_register(&pIrtPortStartTimeTxBlock->Lmw, pDDB);  //LMW modifies it's own CW_Link
    EDDI_SetBitField32(&LocalIrtPortStartTimeTxBlock.Lmw.Hw1.Value.U32_0, EDDI_SER10_LMW_BIT__pAddr, pCwAsic);
    //set new CW_Link in LMW
    EDDI_SERConnectCwToBranch(pDDB,
                              (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&LocalIrtPortStartTimeTxBlock.Lmw,
                              pTree->pRootCw);                                           //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
    //pRootCw=NOOP

    //store start of Shift-List in asic-format with the right endianess
    pMachine->pShiftListAsic = EDDI_HOST2IRTE32(pCwAsic);

    //store SOL in asic-format with the right endianess
    pMachine->pShiftSolAsic = EDDI_HOST2IRTE32(DEV_kram_adr_to_asic_register(&pIrtPortStartTimeTxBlock->Sol, pDDB));

    //store Root-CW (NOOP)
    pMachine->pRootCw = pTree->pRootCw;

    //store Root-CW (NOOP) in asic-format with the right endianess
    pMachine->pRootCwAsic = EDDI_HOST2IRTE32(DEV_kram_adr_to_asic_register(pTree->pRootCw, pDDB));

    //store EOLmax = EOL with the maximum start-time
    //pMachine->pEOLMax is set to a valid value later! Here it is set to NULL!
    pMachine->pEOLMax = pTree->pEOLMax;

    //store Irt-Port-StartTime-TxBlock
    pMachine->pIrtPortStartTimeTxBlock = pIrtPortStartTimeTxBlock;

    switch (HwPortIndex)
    {
        case 0:
            pMachine->IrtStartTimeSndRegAdr = IRT_START_TIME_SND_P0;
            break;
        case 1:
            pMachine->IrtStartTimeSndRegAdr = IRT_START_TIME_SND_P1;
            break;
        case 2:
            pMachine->IrtStartTimeSndRegAdr = IRT_START_TIME_SND_P2;
            break;
        case 3:
            pMachine->IrtStartTimeSndRegAdr = IRT_START_TIME_SND_P3;
            break;
        default:
            break;
    }

    DEV32_MEMCOPY((void *)pIrtPortStartTimeTxBlock, (void *)&LocalIrtPortStartTimeTxBlock, sizeof(EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE));
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERIniIrtPortStartTimeRxBlock()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniIrtPortStartTimeRxBlock( EDDI_LOCAL_DDB_PTR_TYPE                            const  pDDB,
                                                             const EDDI_TREE_TYPE                                  *  const  pTree,
                                                                   LSA_UINT32                                         const  HwPortIndex,
                                                                   EDDI_SER_IRT_PORT_START_TIME_RX_BLOCK_PTR_TYPE     const  pIrtPortStartTimeRxBlock )
{
    LSA_UINT32                                              pCwAsic;
    EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE            LocalIrtPortStartTimeRxBlock;
    EDDI_CRT_PHASE_RX_MACHINE                     *  const  pMachine = &(pDDB->pLocal_CRT->PhaseRx[HwPortIndex]);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERIniIrtPortStartTimeRxBlock");

    EDDI_MemSet(&LocalIrtPortStartTimeRxBlock, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE));

    EDDI_SERIniLMW(pDDB, &LocalIrtPortStartTimeRxBlock.Lmw);
    //Caution: Using own FCW to store DataBuffer (4 Byte for APDU only)
    EDDI_SERIniDummyRxFcw(&LocalIrtPortStartTimeRxBlock.FcwRcv, (EDDI_DEV_MEM_U8_PTR_TYPE)(void *)&pIrtPortStartTimeRxBlock->FcwRcv.Hw2.Value.U32_0, pDDB);
    EDDI_SERIniSOL(pDDB, &LocalIrtPortStartTimeRxBlock.Sol);

    //connect elements
    //LMW is connected with FCW later!
    EDDI_SERConnectCwToNext(pDDB,
                            (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&LocalIrtPortStartTimeRxBlock.FcwRcv,  //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
                            (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pIrtPortStartTimeRxBlock->Sol);       //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
    //SOL was already connected with SER10_NULL_PTR in EDDI_SERIniSOL()!
    //EDDI_SERConnectCwToNext(pDDB,
    //                   (EDDI_SER_CCW_PTR_TYPE)&LocalIrtPortStartTimeRxBlock.Sol,
    //                   (EDDI_SER_CCW_PTR_TYPE)EDDI_NULL_PTR);                      

    //complete LMW
    //select CW to be modified by LMW
    pCwAsic = DEV_kram_adr_to_asic_register(&pIrtPortStartTimeRxBlock->Lmw, pDDB);  //LMW modifies it's own CW_Link

    EDDI_SetBitField32(&LocalIrtPortStartTimeRxBlock.Lmw.Hw1.Value.U32_0, EDDI_SER10_LMW_BIT__pAddr, pCwAsic);

    //set new CW_Link in LMW
    EDDI_SERConnectCwToBranch(pDDB,
                              (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&LocalIrtPortStartTimeRxBlock.Lmw,
                              pTree->pRootCw);                                           //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
    //pRootCw=NOOP in phase-tree

    //Bit9..1 of NEW_CW_Link are not allowed to be all 0 (AP00694034) => set bit 1 (bit2..0 are ignored by asic anyway)
    EDDI_SetBitField32(&LocalIrtPortStartTimeRxBlock.Lmw.Hw0.Value.U32_0, EDDI_SER10_LMW_BIT__pBranch_1, 0x1UL);

    //store start of Shift-List in asic-format with the right endianess
    pMachine->pShiftListAsic = EDDI_HOST2IRTE32(pCwAsic);

    //store SOL in asic-format with the right endianess
    pMachine->pShiftSolAsic = EDDI_HOST2IRTE32(DEV_kram_adr_to_asic_register(&pIrtPortStartTimeRxBlock->Sol, pDDB));

    //store Root-CW (NOOP) in asic-format with the right endianess
    pMachine->pRootCwAsic = EDDI_HOST2IRTE32(DEV_kram_adr_to_asic_register(pTree->pRootCw, pDDB));

    //store Irt-Port-StartTime-RxBlock
    pMachine->pIrtPortStartTimeRxBlock = pIrtPortStartTimeRxBlock;

    switch (HwPortIndex)
    {
        case 0:
            pMachine->IrtStartTimeRcvRegAdr = IRT_START_TIME_RCV_P0;
            break;
        case 1:
            pMachine->IrtStartTimeRcvRegAdr = IRT_START_TIME_RCV_P1;
            break;
        case 2:
            pMachine->IrtStartTimeRcvRegAdr = IRT_START_TIME_RCV_P2;
            break;
        case 3:
            pMachine->IrtStartTimeRcvRegAdr = IRT_START_TIME_RCV_P3;
            break;
        default:
            break;
    }

    DEV32_MEMCOPY((void *)pIrtPortStartTimeRxBlock, (void *)&LocalIrtPortStartTimeRxBlock, sizeof(EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE));
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_RED_PHASE_SHIFT_ON


/*=============================================================================
* function name: EDDI_SERIniLMW()
*
* function:      Initialize the LMW for IRT/SRT-Communication
*                ATTENTION: direct-mode:       initialize LMW direct (without help-buffer)!
*                ATTENTION: reset-before-mode: LMW must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniLMW( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       EDDI_LOCAL_LMW_PTR_TYPE  const  pLMW )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERIniLMW");

    //initialize LMW direct
    EDDI_SetBitField32(&pLMW->Hw0.Value.U32_0, EDDI_SER10_LMW_BIT__Opc          , SER_LMW);
    EDDI_SetBitField32(&pLMW->Hw0.Value.U32_0, EDDI_SER10_LMW_BIT__pBranch_12_0 , SER10_NULL_PTR_12_0);

    EDDI_SetBitField32(&pLMW->Hw0.Value.U32_1, EDDI_SER10_LMW_BIT__pBranch_20_13, SER10_NULL_PTR_20_13);
    EDDI_SetBitField32(&pLMW->Hw0.Value.U32_1, EDDI_SER10_LMW_BIT__pNext        , SER10_NULL_PTR);
    EDDI_SetBitField32(&pLMW->Hw1.Value.U32_0, EDDI_SER10_LMW_BIT__pAddr        , SER10_NULL_PTR);
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/



/*=============================================================================
* function name: EDDI_SERSingleShotSndIni()
*
* function:      Initialize a Single-Shot-Element for IRT/SRT-Communication
*                ATTENTION: direct-mode:       initialize Single-Shot-Element direct (without help-buffer)!
*                ATTENTION: reset-before-mode: Single-Shot-Element must be reset (0) before!
*
* parameters:
*
* return value:  LSA_VOID
*
*==============================================================================
*
*  SingleShot-Element (inactive) --> Only the first NOOP is connected.
*
*  ------->  | NOOP | -----------------------------+-------> | Next |
*                        pNext                     |
*                                                  |
*                                                  |
*            | FCW  | --------->  | LMW |   -------+
*                        pNext               pNext
*
*==============================================================================
*
*  SingleShot-Element ("loaded") --> pNextPtr of Noop Points to SndFcw
*                                --> 1) IRTE will use SndFcw for one "single shot"
*                                --> 2) LMW will move pNextPtr back
*                                       to Next-Element.
*
*  ------->  | NOOP | ---+.........................+-------> | Next |
*                        |                         |
*                        |pNext <.....             |
*        +---------------+           :             |
*        |                           : modifies    |
*        |                           :             |
*        +-> | FCW  | ----------> | LMW | ---------+
*                        pNext               pNext
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSingleShotSndIni( SER_IRT_INI_FCW_TYPE     const  *  const  pComCfg,
                                                         EDDI_SER10_CCW_TYPE             *  const  pCCW,
                                                         EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    EDDI_SER10_SINGLE_SHOT_SND_TYPE  *  const  pSingleShotSnd = &pCCW->SingleShotSnd;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERSingleShotSndIni");

    EDDI_SERIniNOOP(pDDB, &pSingleShotSnd->Noop);
    EDDI_SERIniLMW(pDDB, &pSingleShotSnd->Lmw);
    EDDI_SERIniSleepingProviderFcw(pComCfg, (EDDI_SER10_CCW_TYPE *)(void *)&pSingleShotSnd->FcwSnd, pDDB);  //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

    //external and internal connections (LMW !!) will be done later
}
/*---------------------- end [subroutine] ---------------------------------*/


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSingleShotSndIniTake2( EDDI_SER10_SINGLE_SHOT_SND_TYPE  *  const  pSingleShotCw,
                                                              EDDI_DEV_MEM_U8_PTR_TYPE            const  pDataBuffer,
                                                        const EDD_MAC_ADR_TYPE                 *  const  pDstMacAdr,
                                                              EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    LSA_UINT32  pNextAsic;
    LSA_UINT32  pAddrAsic;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERSingleShotSndIniTake2");

    //Complete SndFcw
    EDDI_SERIrtProviderBufferAdd((EDDI_SER10_CCW_TYPE *)(void *)&pSingleShotCw->FcwSnd,
                                 EDDI_IMAGE_MODE_SYNC | EDDI_IMAGE_MODE_ASYNC, //ImageMode == Standalone
                                 pDataBuffer,
                                 pDstMacAdr,
                                 pDDB,
                                 LSA_FALSE /*bXCW_DBInitialized*/); //temporarily_disabled_lint !e826

    //Activate FcwSnd
    EDDI_SetBitField32(&pSingleShotCw->FcwSnd.Hw0.Value.U32_0, EDDI_SER10_NOOP_BIT__Opc, SER_IRT_DATA);

    //get Next Pointer of NOOP (NOOP must be already connected)
    pNextAsic = EDDI_GetBitField32(pSingleShotCw->Noop.Value.U32_1, EDDI_SER10_LMW_BIT__pNext);

    //Complete LMW
    EDDI_SetBitField32(&pSingleShotCw->Lmw.Hw0.Value.U32_1, EDDI_SER10_LMW_BIT__pNext, pNextAsic);

    pAddrAsic = DEV_kram_adr_to_asic_register(&pSingleShotCw->Noop, pDDB);
    EDDI_SetBitField32(&pSingleShotCw->Lmw.Hw1.Value.U32_0, EDDI_SER10_LMW_BIT__pAddr, pAddrAsic);

    EDDI_SERConnectCwToBranch(pDDB,
                              (EDDI_SER_CCW_PTR_TYPE)(void *)&pSingleShotCw->Lmw,
                              (EDDI_SER_CCW_PTR_TYPE)(void *)EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(pDDB->hSysDev, pNextAsic, pDDB->ERTEC_Version.Location)); //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

    //Connect SndFcw with LMW
    EDDI_SERConnectCwToNext(pDDB,
                            (EDDI_SER_CCW_PTR_TYPE)(void *)&pSingleShotCw->FcwSnd,
                            (EDDI_SER_CCW_PTR_TYPE)(void *)&pSingleShotCw->Lmw); //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
}
/*---------------------- end [subroutine] ---------------------------------*/


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSingleShotSndLoad( EDDI_SER10_SINGLE_SHOT_SND_TYPE  *  const  pSingleShotCw,
                                                          EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERSingleShotSndLoad");

    //Set pNext-Pointer of NOOP to FCW
    EDDI_SERConnectCwToNext(pDDB,
                            (EDDI_SER_CCW_PTR_TYPE)(void *)&pSingleShotCw->Noop,    
                            (EDDI_SER_CCW_PTR_TYPE)(void *)&pSingleShotCw->FcwSnd); //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniRcwSetup( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                    EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW,
                                                    LSA_UINT32                   const  PhaseCnt,
                                                    LSA_UINT32                   const  SendClock,
                                                    LSA_UINT32                   const  ReductionRate,
                                                    LSA_UINT32                   const  SendClockShift )
{
    EDDI_SER10_RCW_RED_TYPE  LocalRcw;

    LocalRcw.Value.U32_0 = 0;

    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "SERIniRCW  SendClock: 0x%X ReductionRate:0x%X, SendClockShift:0x%X PhaseCnt:0x%X", SendClock, ReductionRate, SendClockShift, PhaseCnt);

    if (PhaseCnt & 0x1)
    {
        EDDI_Excp("RedTreeRcw_BuildAllLayers PhaseCnt must be aeven-numbered (PhaseCnt, ReductionRate)", EDDI_FATAL_ERR_EXCP, PhaseCnt, ReductionRate);
        return;
    }

    //RCW is initialized with 0xFF

    //Initialize RCW_RED: 64 Bit
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__Opc            , SER_RCW);
    #if defined (EDDI_CFG_REV7)
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__Udp            , 0);
    #endif
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__SendClockShift , SendClockShift);
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__ReductionRate  , ReductionRate);
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__SendClock      , SendClock);

    pRCW->Value.U32_0 = LocalRcw.Value.U32_0;
    LSA_UNUSED_ARG(pDDB);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniRcwChange( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                     EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW,
                                                     LSA_UINT32                   const  PhaseCnt,
                                                     LSA_UINT32                   const  SendClock,
                                                     LSA_UINT32                   const  ReductionRate,
                                                     LSA_UINT32                   const  SendClockShift )
{
    //EDDI_SER10_RCW_PHS_TYPE    LocalRcwPhs;
    EDDI_LOCAL_RCW_PHS_PTR_TYPE  pRcwPhs;
    LSA_UINT32                   Ctr;
    EDDI_SER10_RCW_RED_TYPE      LocalRcw;

    LocalRcw.Value.U32_0 = 0;

    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "SERIniRCW  SendClock:0x%X ReductionRate:0x%X, SendClockShift:0x%X PhaseCnt:0x%X", SendClock, ReductionRate, SendClockShift, PhaseCnt);

    if (PhaseCnt & 0x1)
    {
        EDDI_Excp("RedTreeRcw_BuildAllLayers PhaseCnt must be even-numbered (PhaseCnt, ReductionRate)", EDDI_FATAL_ERR_EXCP, PhaseCnt, ReductionRate);
        return;
    }

    //Init RCW-Header
    //EDDI_MemSet(pRCW, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_RCW_RED_TYPE));

    //Initialize RCW_RED: 64 Bit
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__Opc            , SER_RCW);

    #if defined (EDDI_CFG_REV7)
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__Udp            , 0);
    #endif

    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__SendClockShift , SendClockShift);
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__ReductionRate  , ReductionRate);
    EDDI_SetBitField32(&LocalRcw.Value.U32_0,  EDDI_SER10_RCW_RED_BIT__SendClock      , SendClock);

    pRCW->Value.U32_0 = LocalRcw.Value.U32_0;
    pRCW->Value.U32_1 = 0xFFFFFFFFUL;

    pRcwPhs = (EDDI_LOCAL_RCW_PHS_PTR_TYPE)(void *)(((EDDI_LOCAL_MEM_U8_PTR_TYPE)(void *)pRCW) + sizeof(EDDI_SER10_RCW_RED_TYPE));

    //Init RCW-Tail
    //LocalRcwPhs.Value = 0;
    //EDDI_SetBitField32(&LocalRcwPhs.Value, EDDI_SER10_LIST_HEAD_BIT__pNext, SER10_NULL_PTR);

    for (Ctr = 0; Ctr < PhaseCnt; Ctr++)
    {
        //(pRcwPhs + Ctr)->Value = LocalRcwPhs.Value;
        (pRcwPhs + Ctr)->Value = 0xFFFFFFFFUL;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERUpdateRcwSendClock()                     */
/*                                                                         */
/* D e s c r i p t i o n: Update Sendclock-Value of specified RCW          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          Pointer to Next RCW                              */
/*                                                                         */
/***************************************************************************/
EDDI_LOCAL_RCW_RED_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SERUpdateRcwSendClock( EDDI_DDB_TYPE                *  const  pDDB,
                                                                              EDDI_LOCAL_RCW_RED_PTR_TYPE     const  pRCW,
                                                                              LSA_UINT8                       const  SendClock )
{
    LSA_UINT32  pCwNextAsic;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "SERUpdateRCW  SendClock:0x%X", SendClock);

    if (SER_RCW != EDDI_GetBitField32(pRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__Opc))
    {
        EDDI_Excp("SERUpdateRCW Wrong Opcode", EDDI_FATAL_ERR_EXCP, EDDI_GetBitField32(pRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__Opc), SER_RCW);
        return EDDI_NULL_PTR;
    }

    //Update Sendclock of RCW_RED:  64 Bit
    EDDI_SetBitField32(&pRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__SendClock, SendClock);

    //Get Next Rcw
    pCwNextAsic = EDDI_GetBitField32(pRCW->Value.U32_1, EDDI_SER10_RCW_RED_BIT2__pNext);

    if (pCwNextAsic == SER10_NULL_PTR)  // passt kein SWAP
    {
        return EDDI_NULL_PTR;
    }
    else
    {
        EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pNextRCW = (EDDI_LOCAL_RCW_RED_PTR_TYPE)pDDB->kram_adr_asic_to_local(pDDB->hSysDev, pCwNextAsic, pDDB->ERTEC_Version.Location);

        if (SER_RCW != EDDI_GetBitField32(pNextRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__Opc))
        {
            return EDDI_NULL_PTR;
        }
        else
        {
            return pNextRCW;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERGetRcwSendClock()                        */
/*                                                                         */
/* D e s c r i p t i o n: Update Sendclock-Value of specified RCW          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_UINT8  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetRcwSendClock( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                         EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SERGetRCWSendClock");

    if (SER_RCW != EDDI_GetBitField32(pRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__Opc))
    {
        EDDI_Excp("SERGetRCWSendClock Wrong Opcode", EDDI_FATAL_ERR_EXCP, EDDI_GetBitField32(pRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__Opc), SER_RCW);
        return 0xFF;
    }

    //Get Sendclock of RCW_RED:  64 Bit
    LSA_UNUSED_ARG(pDDB);
    return (LSA_UINT8)EDDI_GetBitField32(pRCW->Value.U32_0, EDDI_SER10_RCW_RED_BIT__SendClock);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*=============================================================================
* function name: EDDI_SERAddRxAcw()
*
* function:      create structs for XRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERAddRxAcw( EDDI_CCW_CTRL_HEAD_PTR_TYPE  const  pACWHead,
                                                         EDDI_SER_CCW_PTR_TYPE        const  pCCWLocal,
                                                         EDDI_SER_CCW_PTR_TYPE        const  pCCWDev,
                                                         EDDI_CCW_CTRL_PTR_TYPE       const  pACWCtrl,
                                                         EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_CCW_CTRL_PTR_TYPE  pACWPrvCtrl;
    EDDI_CCW_CTRL_PTR_TYPE  pACWActCtrl;
    LSA_UINT32              pPrevCCW;
    LSA_UINT32              pNewCCW;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERAddRxAcw");

    //Add new element to the end of the ACW-list.
    pACWPrvCtrl = EDDI_NULL_PTR;
    pACWActCtrl = pACWHead->pTop;

    //================= Search for an existing element ================

    while (   (!(pACWActCtrl == EDDI_NULL_PTR))
           && (pACWActCtrl->FrameId != pACWCtrl->FrameId))
    {
        pACWPrvCtrl = pACWActCtrl;
        pACWActCtrl = pACWActCtrl->pNext;
    }

    // Element already exists => pACWActCtrl != EDDI_NULL_PTR
    if (!(pACWActCtrl == EDDI_NULL_PTR))
    {
        EDDI_MemFreePoolBuffer(pDDB->pLocal_CRT->hPool.CCWCTRL, pACWCtrl);
        //ret = EDDI_MEM_free(pCCWDev);   ??
        EDDI_Excp("EDDI_SERAddRxAcw kram_free", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    // pPrvCtrl == EDDI_NULL_PTR => FATAL_ERROR
    if (pACWPrvCtrl == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SERAddRxAcw 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    // Add new element to the end of the ACW-list

    pACWPrvCtrl = pACWHead->pBottom;

    // Insert as NOOP, Opcode will be set later to SER_ACW_RX in ConsumerControl
    EDDI_SetBitField32(&pCCWLocal->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);

    DEV32_MEMCOPY(pCCWDev, pCCWLocal, pACWCtrl->Size);

    pPrevCCW = DEV_kram_adr_to_asic_register(pACWPrvCtrl->pKramCW, pDDB);
    pNewCCW  = DEV_kram_adr_to_asic_register(pCCWDev, pDDB);

    // Note: We can call an arbitrary SERInsertXxXCW-command.
    EDDI_SERCommand(FCODE_INSERT_ACW_RX, pPrevCCW, pNewCCW, 0, pDDB);

    pACWCtrl->pPrev      = pACWPrvCtrl;
    pACWPrvCtrl->pNext   = pACWCtrl;
    pACWHead->pBottom    = pACWCtrl;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSrtConsumerSetActivity()
*
* function:      ini the ACW for IRT/SRT-Communication
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSrtConsumerSetActivity( EDDI_CCW_CTRL_PTR_TYPE  const  pLowerCtrlACW,
                                                               LSA_BOOL                const  bActivate )
{
    if (bActivate)
    {
        EDDI_SetBitField32(&pLowerCtrlACW->pKramCW->Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_ACW_RX);
    }
    else
    {
        EDDI_SetBitField32(&pLowerCtrlACW->pKramCW->Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERXchangeBuffer()
*
* function:      ini the ACW for IRT/SRT-Communication
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERXchangeBuffer( EDDI_DDB_TYPE               *  const  pDDB,
                                                      EDDI_SER_CCW_PTR_TYPE          const  pCW,
                                                      EDDI_LOCAL_MEM_U8_PTR_TYPE     const  pNewBuffer )
{
    LSA_UINT32  pNewBufferAsic;

    pNewBufferAsic = DEV_kram_adr_to_asic_register(pNewBuffer, pDDB);

    EDDI_SetBitField32(&pCW->FcwAcw.AcwSnd.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_SND_BIT__pDB0, pNewBufferAsic);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERInsertCw()                               */
/*                                                                         */
/* D e s c r i p t i o n: Always link by CW_Link!                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_REV5)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERInsertCw( EDDI_DDB_TYPE          *  const  pDDB,
                                                 EDDI_SER_CCW_PTR_TYPE     const  pCwPrev,
                                                 EDDI_SER_CCW_PTR_TYPE     const  pCwNew )
{
    LSA_UINT32  pCwNewAsic;

    if (pCwNew == EDDI_NULL_PTR)
    {
        pCwNewAsic = SER10_NULL_PTR;
    }
    else
    {
        //Copy NextPtr from CwPrev to CwNext
        EDDI_SetBitField32(&pCwNew->Noop.Value.U32_1,
                            EDDI_SER10_NOOP_BIT2__pNext,
                            EDDI_GetBitField32(pCwPrev->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext));

        pCwNewAsic = DEV_kram_adr_to_asic_register(pCwNew, pDDB);
    }

    EDDI_SetBitField32(&pCwPrev->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pCwNewAsic);
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERInsertCwDyn()                            */
/*                                                                         */
/* D e s c r i p t i o n: CWNew is inserted between CWPrev and CWNext      */
/*                        Dynamic link by CW_Link or CW_Branch!            */
/*                        ATTENTION: CWNew mustn't have a CW_Branch!       */
/*                                                                         */
/* A r g u m e n t s:     bCwBranchLinking: Linking of CWPrev!             */
/*                                          LSA_FALSE: Link by CW_Link     */
/*                                          LSA_TRUE:  Link by CW_Branch   */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV5)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERInsertCwDyn( EDDI_DDB_TYPE          *  const  pDDB,
                                                    EDDI_SER_CCW_PTR_TYPE     const  pCwPrev,
                                                    EDDI_SER_CCW_PTR_TYPE     const  pCwNext,
                                                    EDDI_SER_CCW_PTR_TYPE     const  pCwNew,
                                                    LSA_BOOL                  const  bCwBranchLinking )
{
    LSA_UINT32  pCwNewAsic;

    if (pCwNew == EDDI_NULL_PTR)
    {
        pCwNewAsic = SER10_NULL_PTR;
    }
    else
    {
        LSA_UINT32  pCwNextAsic;

        if (pCwNext == EDDI_NULL_PTR)
        {
            pCwNextAsic = SER10_NULL_PTR;
        }
        else
        {
            pCwNextAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);
        }

        //ATTENTION: CWNew mustn't have a CW_Branch!
        NRT_SET_NOOP_BIT2__pNext(pCwNew->Noop.Value.U32_1, pCwNextAsic);
        //regular: EDDI_SetBitField32(&pCwNew->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pCwNextAsic);

        pCwNewAsic = DEV_kram_adr_to_asic_register(pCwNew, pDDB);
    }

    if //Linking by CW_Branch requested
       (bCwBranchLinking)
    {
        EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_0, EDDI_SER10_BCW_MOD_BIT__pBranch_12_0, (pCwNewAsic & 0x00001FFFUL));
        EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_1, EDDI_SER10_BCW_MOD_BIT2__pBranch_20_13, ((pCwNewAsic >> 13) & 0x000000FFUL));
    }
    else //Linking by CW_Link requested
    {
        NRT_SET_NOOP_BIT2__pNext(pCwPrev->Noop.Value.U32_1, pCwNewAsic);
        //regular: EDDI_SetBitField32(&pCwPrev->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pCwNewAsic);
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERConnectCwToNext()                        */
/*                                                                         */
/* D e s c r i p t i o n: Always link by CW_Link!                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERConnectCwToNext( EDDI_DDB_TYPE          *  const  pDDB,
                                                        EDDI_SER_CCW_PTR_TYPE     const  pCwPrev,
                                                        EDDI_SER_CCW_PTR_TYPE     const  pCwNext )
{
    LSA_UINT32  pCwNextAsic;

    if (pCwNext == EDDI_NULL_PTR)
    {
        pCwNextAsic = SER10_NULL_PTR;
    }
    else
    {
        pCwNextAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);
    }

    NRT_SET_NOOP_BIT2__pNext(pCwPrev->Noop.Value.U32_1, pCwNextAsic);
    //regular: EDDI_SetBitField32(&pCwPrev->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pCwNextAsic);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERConnectCwToHeader( EDDI_DDB_TYPE                  *  const  pDDB,
                                                          EDDI_LOCAL_LIST_HEAD_PTR_TYPE     const  pKramListHead,
                                                          EDDI_SER_CCW_PTR_TYPE             const  pCwNext )
{
    LSA_UINT32  pCwNextAsic;

    if (pCwNext == EDDI_NULL_PTR)
    {
        pCwNextAsic = SER10_NULL_PTR_SWAP;
    }
    else
    {
        pCwNextAsic = EDDI_HOST2IRTE32(DEV_kram_adr_to_asic_register(pCwNext, pDDB));
    }

    pKramListHead->Value = pCwNextAsic | SER10_PTR_VALID_SWAP;  //SER10_PTR_VALID_SWAP only necessary for ACW-List-Header!
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
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERConnectCwToRcwPhase( EDDI_DDB_TYPE                  *  const  pDDB,
                                                            EDDI_LOCAL_LIST_HEAD_PTR_TYPE     const  pKramListHead,
                                                            EDDI_SER_CCW_PTR_TYPE             const  pCwNext )
{
    LSA_UINT32  pCwNextAsic;

    if (pCwNext == EDDI_NULL_PTR)
    {
        pCwNextAsic = SER10_NULL_PTR;
    }
    else
    {
        // Now get the new NextPtr
        pCwNextAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);
    }

    pKramListHead->Value = EDDI_HOST2IRTE32(pCwNextAsic); // *NO* SER10_PTR_VALID Bit here !!
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERInsertCwToRcwPhase()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERInsertCwToRcwPhase( EDDI_DDB_TYPE                  *  const  pDDB,
                                                           EDDI_LOCAL_LIST_HEAD_PTR_TYPE     const  pKramListHead,
                                                           EDDI_SER_CCW_PTR_TYPE             const  pCwNext )
{
    LSA_UINT32  pCwNextAsic;

    if (pCwNext == EDDI_NULL_PTR)
    {
        pCwNextAsic = SER10_NULL_PTR;
    }
    else
    {
        //Copy NextPtr from CwPrev to CwNext
        EDDI_SetBitField32(&pCwNext->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pKramListHead->Value);

        //Now get the new NextPtr
        pCwNextAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);
    }

    pKramListHead->Value = EDDI_HOST2IRTE32(pCwNextAsic); // *NO* SER10_PTR_VALID Bit here !!
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERDisconnectCwFromRcwPhase()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERDisconnectCwFromRcwPhase( EDDI_LOCAL_LIST_HEAD_PTR_TYPE  const  pKramListHead,
                                                                 EDDI_SER_CCW_PTR_TYPE          const  pCw )
{
    LSA_UINT32  pCwAsic;

    if (pCw == EDDI_NULL_PTR)
    {
        pCwAsic = SER10_NULL_PTR;
    }
    else
    {
        pCwAsic = EDDI_GetBitField32(pCw->FcwAcw.Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext);
    }

    pKramListHead->Value = EDDI_HOST2IRTE32(pCwAsic); // *NO* SER10_PTR_VALID Bit here !!
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERDisconnectCw()                           */
/*                                                                         */
/* D e s c r i p t i o n: Always unlink by CW_Link!                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_REV5)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERDisconnectCw( EDDI_SER_CCW_PTR_TYPE  const  pCwPrev,
                                                     EDDI_SER_CCW_PTR_TYPE  const  pCw )
{
    LSA_UINT32  pCwAsic;

    if (pCw == EDDI_NULL_PTR)
    {
        pCwAsic = SER10_NULL_PTR;
    }
    else
    {
        pCwAsic = EDDI_GetBitField32(pCw->FcwAcw.Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext);
    }

    EDDI_SetBitField32(&pCwPrev->FcwAcw.Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, pCwAsic);
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERDisconnectCwDyn()                        */
/*                                                                         */
/* D e s c r i p t i o n: CW is removed between CWPrev and CWNext          */
/*                        Dynamic unlink by CW_Link or CW_Branch!          */
/*                                                                         */
/* A r g u m e n t s:     bCwBranchLinking: Linking of CWPrev!             */
/*                                          LSA_FALSE: Unlink by CW_Link   */
/*                                          LSA_TRUE:  Unlink by CW_Branch */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV5)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERDisconnectCwDyn( EDDI_DDB_TYPE          *  const  pDDB,
                                                        EDDI_SER_CCW_PTR_TYPE     const  pCwPrev,
                                                        EDDI_SER_CCW_PTR_TYPE     const  pCwNext,
                                                        EDDI_SER_CCW_PTR_TYPE     const  pCw,
                                                        LSA_BOOL                  const  bCwBranchLinking )
{
    LSA_UINT32  pCwNextAsic;

    if (   (pCw     == EDDI_NULL_PTR)
        || (pCwNext == EDDI_NULL_PTR))
    {
        pCwNextAsic = SER10_NULL_PTR;
    }
    else
    {
        pCwNextAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);
    }

    if //Linking by CW_Branch requested
       (bCwBranchLinking)
    {
        EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_0, EDDI_SER10_BCW_MOD_BIT__pBranch_12_0, (pCwNextAsic & 0x00001FFFUL));
        EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_1, EDDI_SER10_BCW_MOD_BIT2__pBranch_20_13, ((pCwNextAsic >> 13) & 0x000000FFUL));
    }
    else //Linking by CW_Link requested
    {
        NRT_SET_NOOP_BIT2__pNext(pCwPrev->FcwAcw.Common.Value.U32_1, pCwNextAsic);
        //regular: EDDI_SetBitField32(&pCwPrev->FcwAcw.Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, pCwNextAsic);
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:        EDDI_SERConnectCwToBranch()                     */
/*                                                                         */
/* D e s c r i p t i o n:  can be used for BCWs, BMWs and LMWs             */
/*                         Link by CW_Branch!                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:           LSA_VOID                                        */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERConnectCwToBranch( EDDI_DDB_TYPE          *  const  pDDB,
                                                          EDDI_SER_CCW_PTR_TYPE     const  pCwPrev,
                                                          EDDI_SER_CCW_PTR_TYPE     const  pCwNext )
{
    LSA_UINT32  const  pBranchAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);

    EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_0, EDDI_SER10_BCW_MOD_BIT__pBranch_12_0, (pBranchAsic & 0x00001FFFUL));
    EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_1, EDDI_SER10_BCW_MOD_BIT2__pBranch_20_13, ((pBranchAsic >> 13) & 0x000000FFUL));
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERConnectCwToNextDyn()                     */
/*                                                                         */
/* D e s c r i p t i o n: Dynamic link by CW_Link or CW_Branch!            */
/*                                                                         */
/* A r g u m e n t s:     bCwBranchLinking: Linking of CWPrev!             */
/*                                          LSA_FALSE: Link by CW_Link     */
/*                                          LSA_TRUE:  Link by CW_Branch   */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV5)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERConnectCwToNextDyn( EDDI_DDB_TYPE          *  const  pDDB,
                                                           EDDI_SER_CCW_PTR_TYPE     const  pCwPrev,
                                                           EDDI_SER_CCW_PTR_TYPE     const  pCwNext,
                                                           LSA_BOOL                  const  bCwBranchLinking )
{
    LSA_UINT32  pCwNextAsic;

    if (pCwNext == EDDI_NULL_PTR)
    {
        pCwNextAsic = SER10_NULL_PTR;
    }
    else
    {
        pCwNextAsic = DEV_kram_adr_to_asic_register(pCwNext, pDDB);
    }

    if //Linking by CW_Branch requested
       (bCwBranchLinking)
    {
        EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_0, EDDI_SER10_BCW_MOD_BIT__pBranch_12_0, (pCwNextAsic & 0x00001FFFUL));
        EDDI_SetBitField32(&pCwPrev->BcwMod.Value.U32_1, EDDI_SER10_BCW_MOD_BIT2__pBranch_20_13, ((pCwNextAsic >> 13) & 0x000000FFUL));
    }
    else //Linking by CW_Link requested
    {
        NRT_SET_NOOP_BIT2__pNext(pCwPrev->Noop.Value.U32_1, pCwNextAsic);
        //regular: EDDI_SetBitField32(&pCwPrev->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pCwNextAsic);
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV5)
/*=============================================================================
* function name: SERAlterBCWMOD()
*
* function:      Initialize the BCM for IRT/SRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  SERAlterBCWMOD( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                               EDDI_LOCAL_BCW_MOD_PTR_TYPE  const  pBCW,
                                               LSA_UINT32                   const  ModMask )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SERAlterBCWMOD");

    //Initialize BCW_MOD: 64 Bit
    EDDI_SetBitField32(&pBCW->Value.U32_0, EDDI_SER10_BCW_MOD_BIT__ModMask, ModMask);                /* 15 Bit */
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: SERIniBCWMOD()
*
* function:      Initialize the BCW for IRT/SRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  SERIniBCWMOD( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                             EDDI_LOCAL_BCW_MOD_PTR_TYPE  const  pBCW,
                                             LSA_UINT32                   const  ModMask )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SERIniBCWMOD");

    //Initialize BCW_MOD: 64 Bit
    pBCW->Value.U32_0 = EDDI_HOST2IRTE32(  SER_BCW_MOD           //Opcode
                                         | 0xFFF80000UL);        //CW_Branch part 1
    pBCW->Value.U32_1 = EDDI_HOST2IRTE32(  0x000000FFUL          //CW_Branch part 2
                                         | SER10_NULL_PTR_HELP); //CW_Link
    EDDI_SetBitField32(&pBCW->Value.U32_0, EDDI_SER10_BCW_MOD_BIT__ModMask, ModMask);                /* 15 Bit */
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*****************************************************************************/
/*  end of file eddi_ser_int.c                                               */
/*****************************************************************************/
