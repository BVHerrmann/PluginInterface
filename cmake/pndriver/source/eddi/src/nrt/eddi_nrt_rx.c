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
/*  F i l e               &F: eddi_nrt_rx.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI: Receive            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.07.07    JS    added Support for SYNC-frames. Hook to SYNC-Function   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_nrt_inc.h"
//#include "eddi_profile.h"
#include "eddi_nrt_filter.h"
#include "eddi_nrt_rx.h"

#include "eddi_sync_usr.h"

#if defined (EDDI_CFG_ERTEC_200)
#include "eddi_time.h"
#endif
//#include "eddi_nrt_ini.h"

#if defined (EDDI_CFG_DEFRAG_ON)
#include "eddi_nrt_frag_rx.h"
#endif

#define EDDI_MODULE_ID     M_ID_NRT_RX
#define LTRC_ACT_MODUL_ID  204

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

//#define EDDI_CFG_SPECIAL_NRT_DROP_TRACE_ON  //Optional LSA-Trace-Entries for dropped ARP-Frames and dropped IP-UDP-Frames! Only for Testing!
                                              //Also all received ARP-Frames are traced.


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n: Optional LSA-Trace-Entries for                   */
/*                        dropped ARP-Frames and dropped IP-UDP-Frames     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SPECIAL_NRT_DROP_TRACE_ON)
//temporarily_disabled_lint -esym(751,  EDDI_ARP_FRAME_TYPE)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Hrd)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Pro)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Hln)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Pln)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Op)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Sha)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Tha)
//temporarily_disabled_lint -esym(754, _EDDI_ARP_FRAME_TYPE::Tpa)

#if !defined (EDDI_CFG_REV5)
typedef struct _EDDI_ARP_FRAME_TYPE
{
    LSA_UINT16      Hrd;                      /* Headertype (1)         */
    LSA_UINT16      Pro;                      /* Protocoltype (0x800)   */
    LSA_UINT8       Hln;                      /* Hardware addr. len (6) */
    LSA_UINT8       Pln;                      /* Protocol len (4)       */
    LSA_UINT16      Op;                       /* Opcode (1,2)           */
    LSA_UINT8       Sha[EDD_MAC_ADDR_SIZE];   /* Sender Hardware addr.  */
    LSA_UINT8       Spa[EDD_IP_ADDR_SIZE];    /* Sender Protocol addr   */
    LSA_UINT8       Tha[EDD_MAC_ADDR_SIZE];   /* Target Hardware addr.  */
    LSA_UINT8       Tpa[EDD_IP_ADDR_SIZE];    /* Target Protocol addr.  */

} EDDI_ARP_FRAME_TYPE;

typedef struct _EDDI_ARP_FRAME_TYPE  EDD_UPPER_MEM_ATTR * EDDI_ARP_FRAME_PTR_TYPE;
#endif
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_special_lsa_drop_trace( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                                    LSA_UINT32                const  FilterIndex,
                                                                    EDDI_DEV_MEM_U8_PTR_TYPE  const  pDataBuffer,
                                                                    LSA_UINT32                const  UserDataOffset )
{
    LSA_UINT32     Ctr;
    LSA_UINT8   *  pHelp;
    LSA_UINT8   *  pSrcIP;
    #if (EDD_MAC_ADDR_SIZE == 6)
    LSA_UINT32     SrcMac1;
    LSA_UINT32     SrcMac2 = 0;
    #else
    #error "Check Implementation!"
    #endif
    #if (EDD_IP_ADDR_SIZE == 4)
    LSA_UINT32     SrcIP;
    #else
    #error "Check Implementation!"
    #endif

    #if (EDD_MAC_ADDR_SIZE == 6)
    //fetch SrcMac
    pHelp = (LSA_UINT8 *)(void *)&SrcMac1;
    for (Ctr = 0; Ctr < 4UL; Ctr++)
    {
        *pHelp = pDataBuffer[Ctr+6UL];
        pHelp++;
    }
    pHelp = (LSA_UINT8 *)(void *)&SrcMac2;
    for (; Ctr < EDD_MAC_ADDR_SIZE; Ctr++)
    {
        *pHelp = pDataBuffer[Ctr+6UL];
        pHelp++;
    }
    #else
    #error "Check Implementation!"
    #endif

    if (EDDI_NRT_FILTER_INDEX_ARP == FilterIndex)
    {
        EDDI_ARP_FRAME_PTR_TYPE  const  pARPFrame = (EDDI_ARP_FRAME_PTR_TYPE)(void *)&pDataBuffer[UserDataOffset];

        pSrcIP = &pARPFrame->Spa[0];
    }
    else //FilterIndex == EDDI_NRT_FILTER_INDEX_IP_UDP
    {
        EDDI_IP_HEADER_TYPE  *  const  pIPHeader = (EDDI_IP_HEADER_TYPE *)(void *)&pDataBuffer[UserDataOffset];

        pSrcIP = &pIPHeader->SrcIP.b.IP[0];
    }

    #if (EDD_IP_ADDR_SIZE == 4)
    //fetch SrcIP
    pHelp = (LSA_UINT8 *)(void *)&SrcIP;
    for (Ctr = 0; Ctr < EDD_IP_ADDR_SIZE; Ctr++)
    {
        *pHelp = *pSrcIP;
        pHelp++;
        pSrcIP++;
    }
    #else
    #error "Check Implementation!"
    #endif

    if (EDDI_NRT_FILTER_INDEX_ARP == FilterIndex)
    {
        EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_special_lsa_drop_trace, ARP-Frame is dropped, SrcMac1:0x%X SrcMac2:0x%X SrcIP:0x%X", SrcMac1, SrcMac2, SrcIP);
    }
    else //FilterIndex == EDDI_NRT_FILTER_INDEX_IP_UDP
    {
        EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_special_lsa_drop_trace, IP-UDP-Frame is dropped, SrcMac1:0x%X SrcMac2:0x%X SrcIP:0x%X", SrcMac1, SrcMac2, SrcIP);
    }
}

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_special_lsa_arp_trace( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                                   EDDI_DEV_MEM_U8_PTR_TYPE  const  pDataBuffer,
                                                                   LSA_UINT32                const  UserDataOffset )
{
    LSA_UINT32                      Ctr;
    LSA_UINT8                    *  pHelp;
    EDDI_ARP_FRAME_PTR_TYPE  const  pARPFrame = (EDDI_ARP_FRAME_PTR_TYPE)(void *)&pDataBuffer[UserDataOffset];
    LSA_UINT8                    *  pSrcIP    = &pARPFrame->Spa[0];
    #if (EDD_IP_ADDR_SIZE == 4)
    LSA_UINT32                      SrcIP;
    #else
    #error "Check Implementation!"
    #endif

    #if (EDD_IP_ADDR_SIZE == 4)
    {
        //fetch SrcIP
        pHelp = (LSA_UINT8 *)(void *)&SrcIP;
        for (Ctr = 0; Ctr < EDD_IP_ADDR_SIZE; Ctr++)
        {
            *pHelp = *pSrcIP;
            pHelp++;
            pSrcIP++;
        }
    }
    #else
    #error "Check Implementation!"
    #endif

    EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_special_lsa_arp_trace, ARP-Frame is received, SrcIP:0x%X Opcode:0x%X", SrcIP, (LSA_UINT32)pARPFrame->Op);
}

#define EDDI_SPECIAL_LSA_DROP_TRACE(pDDB_, FilterIndex_, pDataBuffer_, UserDataOffset_)     \
{                                                                                           \
    switch (FilterIndex_)                                                                   \
    {                                                                                       \
        case EDDI_NRT_FILTER_INDEX_ARP:                                                     \
        case EDDI_NRT_FILTER_INDEX_IP_UDP:                                                  \
        {                                                                                   \
            eddi_special_lsa_drop_trace((pDDB_), (FilterIndex_), (pDataBuffer_), (UserDataOffset_));\
            break;                                                                          \
        }                                                                                   \
        default: break;                                                                     \
    }                                                                                       \
}

#define EDDI_SPECIAL_LSA_ARP_TRACE(pDDB_, FilterIndex_, pDataBuffer_, UserDataOffset_)      \
{                                                                                           \
    switch (FilterIndex_)                                                                   \
    {                                                                                       \
        case EDDI_NRT_FILTER_INDEX_ARP:                                                     \
        {                                                                                   \
            eddi_special_lsa_arp_trace((pDDB_), (pDataBuffer_), (UserDataOffset_));         \
            break;                                                                          \
        }                                                                                   \
        default: break;                                                                     \
    }                                                                                       \
}

#else //!EDDI_CFG_SPECIAL_NRT_DROP_TRACE_ON
#define EDDI_SPECIAL_LSA_DROP_TRACE(pDDB_, FilterIndex_, pDataBuffer_, UserDataOffset_)
#define EDDI_SPECIAL_LSA_ARP_TRACE(pDDB_, FilterIndex_, pDataBuffer_, UserDataOffset_)
#endif //EDDI_CFG_SPECIAL_NRT_DROP_TRACE_ON
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRxDoneInt()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxDoneInt( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                  LSA_UINT32               const  QueueIdx )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[QueueIdx];

    pIF->Rx.LockFct(pIF);
    pIF->Rx.DoneFct(pDDB, pIF);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRxDoneLoadLimit()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxDoneLoadLimit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    NRT_CHX_SSX_TYPE   *  const  pDscr    = &pIF->Rx.Dscr;
    NRT_DSCR_PTR_TYPE            pReqDone = pDscr->pReqDone;
    LSA_UINT32                   Ctr                  = 0;
    LSA_UINT32                   CbfCnt               = 0;
    #if defined (EDDI_CFG_DEFRAG_ON)    
    LSA_UINT32                   FragmentsToCopyCtr   = 0;
    #endif
    #if defined (EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT)
    LSA_UINT8                    PCIWriteCtr = EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT;
    #endif
    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTRxDoneLoadLimit->");

    //NRT-RX-Lock-state is always set here!

    //for-loop over RX-DMACWs
    for (; ; Ctr++)
    {
        volatile  EDDI_SER_DMACW_PTR_TYPE  const  pDMACW = pReqDone->pDMACW;
        LSA_UINT32                                LocalL0;
        EDD_RQB_TYPE                           *  pRQB;
        EDD_UPPER_NRT_RECV_PTR_TYPE               pRQBRcv;
        EDD_UPPER_MEM_U8_PTR_TYPE                 pDataBuffer;
        EDDI_LOCAL_HDB_PTR_TYPE                   pHDB;
        LSA_UINT32                                FrameLen;
        LSA_UINT32                                HwPortIndex;
        LSA_UINT32                                UsrPortID;
        LSA_UINT32                                Status;
        LSA_UINT32                                FilterIndex;
        EDDI_NRT_RX_FILTER_TYPE                *  pFilter;
        EDDI_NRT_RX_USER_TYPE                  *  pFilterRxUser;
        LSA_UINT32                                UserDataOffset;
        LSA_BOOL                                  bSyncID1;
        LSA_BOOL                                  IsUDPUnicast;
        LSA_BOOL                                  IsUDPBroadcast;
        LSA_UINT32                                MACTyp;

        #if defined (EDDI_CFG_DEFRAG_ON)
        LSA_BOOL                                  bIsDeFragFrame = LSA_FALSE;
        #endif
        
		//acknowledge nrt-rx-done-interrupt
        EDDI_SII_IO_x32(pDDB->SII.IAR_NRT_Adr) = pIF->Value_for_IO_NRT_RX_Interrupt_IRTE;

        //read DMACW L0
        LocalL0 = pDMACW->L0.Reg;

        if //owner of the DMACW is still IRTE (= RX-DMACW empty)?
           (LocalL0 & BM_S_DMACW_OwnerHW_29)
        {
            //IRTE-Trigger RX-Enable for security
            //Macro EDDI_NRT_ENABLE_NRTRXTX_CHANNEL cannot be used here!
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE);
            pIF->Rx.UnLockFct(pIF);
            return;
        }

        if //receive-limit reached?
           (     (Ctr >= pIF->Rx.MaxRcvFrame_ToLookUp) 
              || (CbfCnt >= pIF->Rx.MaxRcvFrame_SendToUser)
              #if defined (EDDI_CFG_DEFRAG_ON)
              || (FragmentsToCopyCtr >= pIF->Rx.MaxRcvFragments_ToCopy)
              #endif
           )
        {
            //NRT-RX-overload present
            //do not generate a further nrt-rx-interrupt, if no nrt-channels are open any more

            EDDI_DETECTED_RECEIVE_LIMIT(pDDB->hDDB, pDDB->hSysDev, pIF->NRTChannel);

            if (pIF->CntOpenChannel)
            {
                //generate nrt-rx-interrupt by SW
                EDDI_SII_IO_x32(pDDB->SII.IRR_NRT_Adr) = pIF->Value_for_IO_NRT_RX_Interrupt_IRTE;
                #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) || defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
                if //NRT-RX-timeslice-emulation selected for this NRT-Channel?
                   (pIF->NewCycleReductionFactorForNRTCheckRxLimit)
                {
                    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_RX_DONE_LOOP);
                    *pIF->pNewCycleReductionCtrForNRTCheckRxLimit = pIF->NewCycleReductionFactorForNRTCheckRxLimit;
                    //set selected NRT-RX-Done-interrupt-bit in RX-Limit-Mask
                    *pIF->pNRT_RX_Limit_MaskIRTE |= pIF->Value_for_IO_NRT_RX_Interrupt_IRTE;
                    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_RX_DONE_LOOP);
                    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                    EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTRxDoneLoadLimit, NRT-RX-Limitation/Timeslicing is started, NRT-Queue-Index:0x%X NewCycleReductionFactorForNRTCheckRxLimit:0x%X", pIF->QueueIdx, pIF->NewCycleReductionFactorForNRTCheckRxLimit);
                    #endif
                }
                #endif
            }
            //IRTE-Trigger RX-Enable for security
            //Macro EDDI_NRT_ENABLE_NRTRXTX_CHANNEL cannot be used here!
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE);
            pIF->Rx.UnLockFct(pIF);
            return;
        }

        #if defined (EDDI_CFG_REV6)
        //read DMACW L0 again to make sure that the DMACW has been read consistently (LBU 16bit mode!)
        LocalL0 = pDMACW->L0.Reg;
        #endif

        NRT_GET_Status(Status, LocalL0);
        if (Status)
        {
            EDDI_Excp("EDDI_NRTRxDoneLoadLimit, invalid DMACW-Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
            pIF->Rx.UnLockFct(pIF);
            return;
        }

        pDataBuffer = pReqDone->pRcvDataBuffer;

        NRT_GET_FrameLength(FrameLen, LocalL0);

        #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
        EDDI_SYNC_CACHE_NRT_RECEIVE_JIT(pDataBuffer, FrameLen);
        #endif

        FilterIndex = EDDI_NRTRxCheckFrame(pDataBuffer, pIF, FrameLen, &UserDataOffset, pDDB, &bSyncID1);
        
        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        if (pDDB->RTOUDP.pActConsumer)
        {
            //exchange data buffer
            pReqDone->pRcvDataBuffer  = pDDB->RTOUDP.pActConsumer->Udp.pData;
            pDMACW->pDataBuffer       = EDDI_HOST2IRTE32(pIF->adr_to_asic(pDDB->hSysDev, pReqDone->pRcvDataBuffer, pDDB->ERTEC_Version.Location));
            pDDB->RTOUDP.pActConsumer = EDDI_NULL_PTR;
        }
        #endif

        #if defined (EDDI_CFG_DEFRAG_ON)
        //Build NRT Frag together
        NRT_GET_Rcv_Port(HwPortIndex, LocalL0);

        if (   (EDDI_NRT_FILTER_INDEX_NRT_FRAG == FilterIndex) 
            && (pDDB->NRT.RxFragData.bDefragOn))
        {
            EDDI_NRT_FRAGMENT_STATE  StateFrame;

            pDDB->NRT.RxFragData.FrameLen[pIF->QueueIdx] = FrameLen;                  
            StateFrame = EDDI_NRTRxFragBuildFrame(pDDB, pDataBuffer, pIF, &UserDataOffset, &bSyncID1);

            switch (StateFrame)
            {                              
                case EDDI_NRT_RECEIVED_FRAGMENTED_FRAMES:
                case EDDI_NRT_RECEIVED_FRAGMENTED_FRAME:
                {
                    //NRT-Frag Frame OK
                    if (pDDB->NRT.RxFragData.bLastFrame[pIF->QueueIdx])
                    {
                        //Fragment n
                        //EDDI_MEMCOPY(pDataBuffer, pDDB->NRT.RxFragData.pNRTDeFragFrame[pIF->QueueIdx], (LSA_UINT)pDDB->NRT.RxFragData.FrameLen[pIF->QueueIdx]);
                        
                        FrameLen    = pDDB->NRT.RxFragData.FrameLen[pIF->QueueIdx];
                        FilterIndex = EDDI_NRTRxCheckFrame(pDDB->NRT.RxFragData.pNRTDeFragFrame[pIF->QueueIdx], pIF, FrameLen, &UserDataOffset, pDDB, &bSyncID1);

                        //Add last as Fragment
                        if (EDDI_NRTRxFragSetStatitics(pDDB, HwPortIndex, EDDI_NRT_RECEIVED_FRAGMENTED_FRAMES))
                        {
                        }
                        
                        bIsDeFragFrame = LSA_TRUE;
                    }
                    else
                    {
                        //Fragment 0 to n-1
                        //Discard NRT-Fragment, the buffer was saved before
                        FilterIndex = EDDI_NRT_FILTER_INDEX_GARBAGE;
                    }
                    
                    FragmentsToCopyCtr++;
                    
                    break;
                }
                case EDDI_NRT_DISCARDED_RX_FRAGMENTS:
                {
                    //Bad NRT-Frag Frame                                  
                    FilterIndex = EDDI_NRT_FILTER_INDEX_GARBAGE;
                    FrameLen    = 0;
                    
                    if (EDDI_NRTRxFragSetStatitics(pDDB, HwPortIndex, StateFrame))
                    {
                    } 
                    
                    break;
                }
                case EDDI_NRT_RECEIVED_NON_FRAGMENTED_FRAMES:
                case EDDI_NRT_DISCARDED_RX_FRAMES:
                default:
                {
                    //do nothing
                    break;
                }  
            }
        }
        else
        {
            if (EDDI_NRTRxFragSetStatitics(pDDB, HwPortIndex, EDDI_NRT_RECEIVED_NON_FRAGMENTED_FRAMES))
            {
            }
        }
        #endif //EDDI_CFG_DEFRAG_ON

        pFilter       = EDDI_NULL_PTR;
        pFilterRxUser = EDDI_NULL_PTR;
        //select specific filter
        if (FilterIndex < EDDI_NRT_FILTER_INDEX_LAST)
        {
            pFilter       = &pIF->Rx.Filter[FilterIndex];
            pFilterRxUser = pFilter->pRxUser;
        }

        //for UDP, pass only frames which doesn't meet the conditions in EDDI_IsUDPFrameToDrop
        if (EDDI_NRT_FILTER_INDEX_IP_UDP == FilterIndex) 
        { 
            IsUDPUnicast=!EDDI_IS_MC_MAC(pDataBuffer) && (pIF->RxFilterUDP_Unicast);
            IsUDPBroadcast=EDDI_IS_BC_MAC(pDataBuffer) && (pIF->RxFilterUDP_Broadcast);

            if (IsUDPUnicast || IsUDPBroadcast)  //Unicast or Broadcast
            {
                if(IsUDPUnicast)
                {
                    MACTyp=EDDI_UDP_FILTER_MAC_UNICAST;
                }
                else 
                {
                    MACTyp=EDDI_UDP_FILTER_MAC_BROADCAST;
                }
                #if defined (EDDI_CFG_DEFRAG_ON)
                if (bIsDeFragFrame)
                {
                    pDataBuffer = pDDB->NRT.RxFragData.pNRTDeFragFrame[pIF->QueueIdx];
                }
                #endif

                if (EDDI_IsUDPFrameToDrop(pDDB, MACTyp, pDataBuffer, UserDataOffset))
                {
                    //Drop UDP Frame
                    pFilter       = EDDI_NULL_PTR;
                    pFilterRxUser = EDDI_NULL_PTR;
                }
            }
        }

        if //no user available for the specific filter?
            (!pFilterRxUser)
        {
            //ATTENTION: do not change!
            #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
            //invalidate nrt-cache
            EDDI_SYNC_CACHE_NRT_RECEIVE(pDataBuffer, FrameLen);
            #endif
            //drop frame
            //free DMACW = deliver DMACW to IRTE (set owner to HW)
            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_OwnerHW_29);
            pDMACW->L0.Reg = LocalL0;
            //switch to next entry (DMACW)
            pReqDone        = pReqDone->next_dscr_ptr;
            pDscr->pReqDone = pReqDone;
            //IRTE-Trigger RX-Enable
            EDDI_NRT_ENABLE_NRTRXTX_CHANNEL(pIF->Value_for_IO_NRT_ENABLE, (NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE)); 
            EDDI_SPECIAL_LSA_DROP_TRACE(pDDB, FilterIndex, pDataBuffer, UserDataOffset);
            continue;
        }

        pRQB = pFilterRxUser->pFirstRQB;
        if //no user-resource available?
           (pRQB == (void *)0)
        {
            //ATTENTION: do not change!
            #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
            //invalidate nrt-cache
            EDDI_SYNC_CACHE_NRT_RECEIVE(pDataBuffer, FrameLen);
            #endif
            //drop frame
            //free DMACW = deliver DMACW to IRTE (set owner to HW)
            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_OwnerHW_29);
            pDMACW->L0.Reg = LocalL0;
            //switch to next entry (DMACW)
            pReqDone        = pReqDone->next_dscr_ptr;
            pDscr->pReqDone = pReqDone;
            //IRTE-Trigger RX-Enable
            EDDI_NRT_ENABLE_NRTRXTX_CHANNEL(pIF->Value_for_IO_NRT_ENABLE, (NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE)); 
            EDDI_SPECIAL_LSA_DROP_TRACE(pDDB, FilterIndex, pDataBuffer, UserDataOffset);
            continue;
        }

        EDDI_SPECIAL_LSA_ARP_TRACE(pDDB, FilterIndex, pDataBuffer, UserDataOffset);

        pIF->Rx.Queued_RxCount--;

        #if defined (EDDI_DEACT_NRT_CHANNEL_AT_NO_RX_RESOURCE)
        if (pIF->Rx.Queued_RxCount == 0)
        {
            #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
            if (pDDB->pLocal_RTOUDP->EnableRcv_CHA0 == 0)
            #endif
            {
                //disable NRT channel
                IO_x32(SS_QUEUE_DISABLE) = pIF->Value_for_NRT_Queue_DISABLE;
            }
        }
        #endif

        //remove RQB from queue
        pFilterRxUser->pFirstRQB = (EDD_RQB_TYPE *)EDD_RQB_GET_NEXT_RQB_PTR(pRQB);

        {
            pRQBRcv = (EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB->pParam;

            #if defined (EDDI_CFG_DEFRAG_ON)
            if (bIsDeFragFrame)
            {
                EDD_UPPER_MEM_U8_PTR_TYPE  pTempPointer;

                //Exchange NRT-Defrag DataBuffer with UserBuffer (not with DMACW Buffer)
                pTempPointer                                        = pRQBRcv->pBuffer;
                pRQBRcv->pBuffer                                    = pDDB->NRT.RxFragData.pNRTDeFragFrame[pIF->QueueIdx];
                pDDB->NRT.RxFragData.pNRTDeFragFrame[pIF->QueueIdx] = pTempPointer;  
            }
            else
            #endif
            {           
                //exchange data buffer
                pReqDone->pRcvDataBuffer = pRQBRcv->pBuffer;
                EDDI_HOST2IRTE32x(pDMACW->pDataBuffer, pIF->adr_to_asic(pDDB->hSysDev, pRQBRcv->pBuffer, pDDB->ERTEC_Version.Location));

                //init pRQBRcv
                pRQBRcv->pBuffer         = pDataBuffer;
            }

            if (pFilter != EDDI_NULL_PTR)
            {
                LSA_UINT32 const TimeLoc = pDMACW->Time;
                pRQBRcv->IOCount         = (LSA_UINT16)FrameLen;
                pRQBRcv->UserDataOffset  = (LSA_UINT16)UserDataOffset;
                pRQBRcv->UserDataLength  = (LSA_UINT16)(FrameLen - UserDataOffset);
                pRQBRcv->RxTime          = EDDI_IRTE2HOST32(TimeLoc);
                pRQBRcv->FrameFilter     = pFilter->Filter;
                pRQBRcv->FrameFilterInfo = pFilter->FrameFilterInfo;
            }

            if (LocalL0 & BM_S_DMACW_LocalCH_15)
            {
                pRQBRcv->PortID = EDD_PORT_ID_LOCAL;
            }
            else
            {
                #if !defined (EDDI_CFG_DEFRAG_ON)
                NRT_GET_Rcv_Port(HwPortIndex, LocalL0);
                #endif
                UsrPortID = pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex];

                switch (UsrPortID)
                {
                    case 0x1:
                        pRQBRcv->PortID = 1;
                        pDDB->LocalIFStats.RxGoodCtr++;
                        break;
                    case 0x2:
                        pRQBRcv->PortID = 2;
                        pDDB->LocalIFStats.RxGoodCtr++;
                        break;
                    case 0x3:
                        pRQBRcv->PortID = 3;
                        pDDB->LocalIFStats.RxGoodCtr++;
                        break;
                    case 0x4:
                        pRQBRcv->PortID = 4;
                        pDDB->LocalIFStats.RxGoodCtr++;
                        break;
                    default :
                    {
                        EDDI_Excp("EDDI_NRTRxDoneLoadLimit, HwPortIndex: UsrPortID:", EDDI_FATAL_ERR_EXCP, HwPortIndex, UsrPortID);
                        pIF->Rx.UnLockFct(pIF);
                        return;
                    }
                }

                #if defined (EDDI_CFG_ERTEC_200)
                if (   (pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].StateDeterminePhaseShift == EDDI_STATE_PHASE_SHIFT_CHECK)
                    && (!pDDB->Glob.bPhyExtern))
                {
                    pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].StateDeterminePhaseShift = EDDI_STATE_PHASE_SHIFT_FIRE_IND;
                    (void)EDDI_StartTimer(pDDB, pDDB->pLocal_SWITCH->PhyPhaseShift_TimerId, 1UL);

                    //EDDI_SwiPhyE200DeterminePhaseShift(HwPortIndex, pDDB);
                }
                #endif
            }

            EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);

            switch (Status)
            {
                case 0x0:
                    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);
                    break;
                case 0x1:
                case 0x2:
                default:
                {
                    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_RESOURCE);
                    EDDI_Excp("EDDI_NRTRxDoneLoadLimit, invalid DMACW-Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
                    pIF->Rx.UnLockFct(pIF);
                    return;
                }
            }

            //free DMACW = deliver DMACW to IRTE (set owner to HW)
            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_OwnerHW_29);
            pDMACW->L0.Reg = LocalL0;
            //switch to next entry (DMACW)
            pReqDone        = pReqDone->next_dscr_ptr;
            pDscr->pReqDone = pReqDone;
            //IRTE-Trigger RX-Enable
            EDDI_NRT_ENABLE_NRTRXTX_CHANNEL(pIF->Value_for_IO_NRT_ENABLE, (NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE)); 

            pFilterRxUser->RxQueueReqCnt--;

            pRQBRcv->RequestCnt = pFilterRxUser->RxQueueReqCnt;

            pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

            EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

            if (!(0 == pHDB->Cbf))
            {
                //If this was a SYNC-Frame we call a hook-function to fill up remaining RQB entries not set yet.
                switch (FilterIndex)
                {
                    case EDDI_NRT_FILTER_INDEX_PTCP_SYNC:
                    {
                        EDDI_SyncRcvNRTDoneHook(pDDB, pRQB, bSyncID1);
                        break;
                    }
                    case EDDI_NRT_FILTER_INDEX_PTCP_DELAY:
                    {
                        LSA_UINT32  DestUsrPortId;

                        for (DestUsrPortId = 0; DestUsrPortId < pDDB->PM.PortMap.PortCnt; DestUsrPortId++)
                        {
                            pRQBRcv->doSwFwd[DestUsrPortId] = EDD_NRT_RECV_DO_NOT_FWD_BY_UPPER_LAYER;
                        }
                        break;
                    }
                    case EDDI_NRT_FILTER_INDEX_PTCP_ANNO:
                    {
                        LSA_UINT32  DestUsrPortId;

                        for (DestUsrPortId = 0; DestUsrPortId < pDDB->PM.PortMap.PortCnt; DestUsrPortId++)
                        {
                            pRQBRcv->doSwFwd[DestUsrPortId] = (bSyncID1) ? EDD_NRT_RECV_DO_FWD_BY_UPPER_LAYER : EDD_NRT_RECV_DO_NOT_FWD_BY_UPPER_LAYER;   //Announce frames for SyncID0 are forwarded by HW (depending on boundaries)
                                                                                                   //Announce frames for SyncID1 are to be forwarded by SW
                        }
                        break;
                    }
                    default: break;
                }

                EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);
                pHDB->RxCount--;
                CbfCnt++;
            }
            else
            {
                EDDI_Excp("EDDI_NRTRxDoneLoadLimit, no CBF!", EDDI_FATAL_ERR_EXCP, 0, 0);
                pIF->Rx.UnLockFct(pIF);
                return;
            }
        }
    } //end for
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_rx.c                                                */
/*****************************************************************************/



