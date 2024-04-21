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
/*  F i l e               &F: eddi_rto_udp.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
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
#include "eddi_rto_udp.h"

#define EDDI_MODULE_ID     M_ID_RTO_UDP
#define LTRC_ACT_MODUL_ID  128

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_sb.h"
#include "eddi_nrt_q.h"
#include "eddi_nrt_tx.h"

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RtoIniSndTree( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                        EDDI_DDB_COMP_RTOUDP_TYPE * const pRTOUDP );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RtoSendFrame( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                       EDDI_CRT_PROVIDER_PTR_TYPE const pProv );
#endif


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_DDB_COMP_RTOUDP_TYPE  *  pRTOUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoInitComponent->");

    pRTOUDP = &pDDB->RTOUDP;

    //EDDI_MemSet(pRTOUDP, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_COMP_RTOUDP_TYPE));

    if (!(pDDB->pLocal_RTOUDP == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_RtoInitComponent", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pRTOUDP->cProvider      = 0;
    pRTOUDP->cConsumer      = 0;
    pRTOUDP->EnableRcv_CHA0 = 0;

    pRTOUDP->TOS_DSCP       = pDDB->pConstValues->TOSDSCP;

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

    pDDB->pLocal_RTOUDP = pRTOUDP;

    EDDI_RtoSBInit(pDDB);

    pRTOUDP->pIFNrtUDP = &pDDB->NRT.IF[EDDI_NRT_CHA_IF_0];

    if (pRTOUDP->pIFNrtUDP->Tx.Dscr.Cnt == 0)
    {
        EDDI_Excp("EDDI_RtoInitComponent, pIF->Tx.CntDscr == 0",  EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_RtoIniSndTree(pDDB, pRTOUDP);

    #endif

    pDDB->pLocal_RTOUDP = pRTOUDP;
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
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RtoRelComponent(EDDI_LOCAL_DDB_PTR_TYPE  const   pDDB)
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoRelComponent->");

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    /* check if initialized. If not return with ok */
    if (pDDB->pLocal_RTOUDP == EDDI_NULL_PTR)
    {
        return;
    }

    EDDI_TreeFree(pDDB->pLocal_RTOUDP->pTreeSB);
    EDDI_TreeFree(pDDB->pLocal_RTOUDP->pTreeSnd);
    #else
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoIniSndTree( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                           EDDI_DDB_COMP_RTOUDP_TYPE  *  const  pRTOUDP )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoIniSndTree->");

    EDDI_TreeIni(EDDI_UDP_MAX_BINARY_REDUCTION, &pRTOUDP->pTreeSnd);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProcessXRTCyclic( EDD_UPPER_MEM_U8_PTR_TYPE    const  pFrame,
                                                         LSA_UINT32                   const  FrameID,
                                                         EDDI_UDP_XRT_FRAME_PTR_TYPE  const  pHeader,
                                                         EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_CRT_CONSUMER_PTR_TYPE  pConsumer;
    LSA_UINT32                  i, Cnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProcessXRTCyclic->");

    //search consumer by FrameId
    Cnt       = pDDB->CRT.UdpConsQueue.Cnt;
    pConsumer = pDDB->CRT.UdpConsQueue.pTop;

    for (i = 0; i < Cnt; i++)
    {
        if (   (pConsumer->LowerParams.FrameId  == FrameID)
            && (pConsumer->LowerParams.SrcIP.dw == pHeader->Ip.SrcIP.dw))
        {
            //Normally the buffer from Next i taken -> can be empty if user just fetched a buffer from Next
            //Next will stay empty until the next reception
            pConsumer->Udp.pData = pConsumer->Udp.pNext;
            pConsumer->Udp.pNext = pFrame;

            if (!pConsumer->Udp.pData)
            {
                pConsumer->Udp.pData = pConsumer->Udp.pFree;
            }

            if (pConsumer->Udp.Network_Ip_TotalLength != pHeader->Ip.TotalLength)
            {
                //invalid Length
                break;
            }

            pConsumer->Udp.pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pFrame + pConsumer->Udp.OffsetAPDU);

            pConsumer->Udp.StateFct(EDDI_SB_RTO_EV_FRAME, pConsumer, pDDB);

            pDDB->RTOUDP.pActConsumer = pConsumer;
            break;
        }

        pConsumer = pConsumer->pNext;
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoEnableRcvIF( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoEnableRcvIF->");

    pDDB->pLocal_RTOUDP->EnableRcv_CHA0 = 1;

    pIF = pDDB->RTOUDP.pIFNrtUDP;

    if (pIF->Rx.Queued_RxCount == 0)
    {
        //enable nrt-channel for receiving
        IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE);
        //enable NRT-SS-QUEUE (for sending and receiving)
        IO_x32(SS_QUEUE_DISABLE) = pIF->Value_for_NRT_Queue_ENABLE;
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoChangeIP( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_CRT_PROVIDER_PTR_TYPE     pProvider;
    LSA_UINT32                     i, Cnt;
    EDDI_NRT_CHX_SS_IF_TYPE     *  pIF;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoChangeIP->");

    //search consumer by FrameID
    Cnt       = pDDB->CRT.UdpProvQueue.Cnt;
    pProvider = pDDB->CRT.UdpProvQueue.pTop;

    pIF  = pDDB->RTOUDP.pIFNrtUDP;

    pIF->Tx.LockFct(pIF);

    for (i = 0; i < Cnt; i++)
    {
        pProvider->Udp.Buf1.pIPHeader->Ip.SrcIP.dw = pDDB->Glob.LocalIP.dw;
        pProvider->Udp.Buf2.pIPHeader->Ip.SrcIP.dw = pDDB->Glob.LocalIP.dw;
        pProvider->Udp.Buf3.pIPHeader->Ip.SrcIP.dw = pDDB->Glob.LocalIP.dw;

        pProvider = pProvider->pNext;
    }

    pIF->Tx.UnLockFct(pIF);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoSnd( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                            LSA_UINT32               const  para_1 )
{
    E_TREE                      *  pTree;
    LSA_UINT32                     Ctr1, Ctr2, CntEntries, LastIndex;
    EDDI_CRT_PROVIDER_PTR_TYPE     pProv;
    LSA_UINT16                     CycleCntVal, Diff;
    EDDI_DDB_COMP_RTOUDP_TYPE   *  pRTOUDP;
    EDDI_NRT_CHX_SS_IF_TYPE     *  pIF;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoSnd->");

    pRTOUDP = &pDDB->RTOUDP;

    pTree = pRTOUDP->pTreeSnd;
    pIF   = pRTOUDP->pIFNrtUDP;

    pIF->Tx.LockFct(pIF);

    EDDI_TreeGetPathList(pTree);

    LastIndex = pTree->CntActPath;

    CycleCntVal = (LSA_UINT16)IO_R32(CYCL_COUNT_VALUE);

    Diff = (LSA_UINT16)(CycleCntVal - pRTOUDP->OldCycleVal);

    Diff = (LSA_UINT16)(Diff / 32);

    pRTOUDP->OldCycleVal = CycleCntVal;

    pRTOUDP->CycleCntInMs        = (LSA_UINT16)(pRTOUDP->CycleCntInMs + Diff);
    pRTOUDP->NetWorkCycleCntInMs = EDDI_HTONS(pRTOUDP->CycleCntInMs);

    #if defined (EDDI_CFG_FRAG_ON)
    if //fragmentation cleanup not active?
       (!pDDB->NRT.TxFragData.bFragCleanupActive)
    #endif
    {
        for (Ctr1 = 0; Ctr1 < LastIndex; Ctr1++)
        {
            pProv = (EDDI_CRT_PROVIDER_PTR_TYPE )pTree->ActPath[Ctr1].pList;

            CntEntries = pTree->ActPath[Ctr1].CntEntries;

            for (Ctr2 = 0; Ctr2 < CntEntries; Ctr2++)
            {
                if (!pProv)
                {
                    EDDI_Excp("EDDI_RtoSnd", EDDI_FATAL_ERR_EXCP, Ctr1, Ctr2);
                    pIF->Tx.UnLockFct(pIF);
                    LSA_UNUSED_ARG(para_1); //satisfy lint
                    return;
                }

                if (pProv->Status != EDDI_CRT_CONS_STS_ACTIVE)
                {
                    pProv = pProv->Link.pNext;
                    continue;
                }

                EDDI_RtoSendFrame(pDDB, pProv);
                pProv = pProv->Link.pNext;
            } //end for-loop
        } //end for-loop
    }

    EDDI_RtoSBCheck(pDDB);
    pIF->Tx.UnLockFct(pIF);
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoSendFrame( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          EDDI_CRT_PROVIDER_PTR_TYPE  const  pProv )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = pDDB->RTOUDP.pIFNrtUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoSendFrame->");

    if (pProv->Udp.ParamSnd.LineDelayInTicksHardware == 0)
    {
        //Frame not in list yet, i.e. not yet sent
        return;
    }

    pProv->Udp.ParamSnd.LineDelayInTicksHardware = 0;

    if (pProv->Udp.pNext == EDDI_NULL_PTR)
    {
        //no exchange
    }
    else
    {
        pProv->Udp.pFree = pProv->Udp.pData;     // D -> F
        pProv->Udp.pData = pProv->Udp.pNext;     // N -> D
        pProv->Udp.pNext = EDDI_NULL_PTR;

        pProv->Udp.ParamSnd.pBuffer = pProv->Udp.pData->pFrame;
    }

    pProv->Udp.pData->pAPDU->Detail.CycleCnt = pDDB->RTOUDP.NetWorkCycleCntInMs;

    (LSA_VOID)EDDI_NrtAddToPrioQueue(pDDB, pIF, &pProv->Udp.RQB);
    EDDI_NRTSendWithLimit(pDDB, pIF); //try to reload!
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_XRT_OVER_UDP_SOFTWARE


/*****************************************************************************/
/*  end of file eddi_rto_udp.c                                               */
/*****************************************************************************/
