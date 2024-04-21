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
/*  F i l e               &F: eddi_nrt_tx.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI: Transmit           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  16.07.07    JS    added support for SYNC-Send (mapped to NRT-Send)       */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_nrt_tx.h"
#include "eddi_nrt_q.h"
#include "eddi_nrt_inc.h"

#if defined (EDDI_CFG_REV5)
#include "eddi_nrt_ts.h"
#include "eddi_Tra.h"
#include "eddi_swi_ext.h"
#endif

#if defined (EDDI_CFG_REV5)
    #include "eddi_sync_usr.h"
#endif
//#include "eddi_nrt_ini.h"

#define EDDI_MODULE_ID     M_ID_NRT_TX
#define LTRC_ACT_MODUL_ID  205

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSendWithLimit()                          */
/*                                                                         */
/* D e s c r i p t i o n: try to reload queued NRT-Sends from              */
/*                        EDDI Send-queue into DMACW-queue                 */
/*                        ATTENTION: this function  must always be called  */
/*                                   under TX lock!                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSendWithLimit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                      EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    NRT_DSCR_PTR_TYPE  pReq = pIF->Tx.Dscr.pReq;
    #if defined (EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT)
    LSA_UINT8          PCIWriteCtr = EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT;
    #endif

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSendWithLimit->");

    for (;;)
    {
        EDD_SERVICE                        Service;
        LSA_UINT32                         LocalL0;
        volatile  EDDI_SER_DMACW_PTR_TYPE  pDMACW;
        EDD_UPPER_RQB_PTR_TYPE             pRQB;
        EDD_UPPER_NRT_SEND_PTR_TYPE        pRQBSnd;
        LSA_UINT16                         UsrPortId;
        LSA_UINT32                         pDataBuffer;
        EDD_UPPER_MEM_U8_PTR_TYPE          pSndBuffer; //user size
        LSA_UINT32                         SndLength;
        #if defined (EDDI_CFG_FRAG_ON)
        LSA_UINT32                         internal_context_1;
        #endif

        if //no free DMACW available?
           (!(pReq->pSndRQB == EDDI_NULL_PTR)) //= RQB is not yet returned by EDDI (by TX-Done-Interrupt)
        {
            //DMACW-ring must be completely occupied (IRTE or EDDI hasn't processed any DMACW yet)!
            //initiate DMACW check in IRTE for the case, that IRTE hasn't processed any DMACW yet
            //IRTE-Trigger TX-Enable to check the DMACW-queue
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE);
            return;
        }

        if //no further rqb is queued?
           (pIF->Tx.QueuedSndReq == 0)
        {
            //IRTE-Trigger TX-Enable to check the DMACW-queue
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE);
            return;
        }

        //remove rqb from queue with checking of SLL (send load limitation) and NRT TX fragmentation semaphore
        pRQB = EDDI_NrtGetFromPrioQueueWithLimit(pDDB, pIF);
        if //no rqb available to be put to DMACW-ring (send-limit reached or NRT TX fragmentation semaphore not free)?
           //(rqb available is already checked above)
           (pRQB == EDDI_NULL_PTR)
        {
            //IRTE-Trigger TX-Enable to check the DMACW-queue
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE);
            return;
        }

        //init local DMACW L0
        LocalL0 = 0;

        pRQBSnd   = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;
        UsrPortId = pRQBSnd->PortID;
        pDMACW    = pReq->pDMACW;

        #if defined (EDDI_CFG_FRAG_ON)
        internal_context_1 = pRQB->internal_context_1;
        if //FRAG RQB?
           (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
        {
            EDDI_NRT_TX_FRAG_DDB_TYPE  *  const  pNrtTxFragDataDDB = &pDDB->NRT.TxFragData;

            //TX semaphore is always locked here

            pSndBuffer = pNrtTxFragDataDDB->pSndBufferCurrent;
            SndLength  = pNrtTxFragDataDDB->SndLengthCurrent;

            //insert FragmentFrameIdLow to fragment
            pSndBuffer[pNrtTxFragDataDDB->PDUFragmentFrameIdLowOffset] = pNrtTxFragDataDDB->FragmentFrameIdLow;

            if //last NRT TX fragment of a NRT send service?
               (pNrtTxFragDataDDB->bLastFragmentDmacwEntry)
            {
                //update FragmentFrameIdLow
                pNrtTxFragDataDDB->FragmentFrameIdLow = (pNrtTxFragDataDDB->FragmentFrameIdLow + (LSA_UINT8)1) & (LSA_UINT8)0x8F; //increment low nibble

                #if defined (EDDI_CFG_REV5)
                #error "NRT TX fragmentation: function Send-Response is not available on REV5!"
                #elif !defined (EDDI_CFG_REV5)
                //the last NRT TX fragment of a NRT TX fragmentation may not be confirmed until this fragment is given to the mac!
                //this is necessary because the confirmation triggers the releasing of the TX fragment semaphore!
                EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_Send_Response_25);
                #endif
            }
        }
        else //no FRAG RQB
        #endif
        {
            //Manipulating of framebuffer is only allowed for NRT-Send Service and not for a fragment!
            if (EDD_SRV_NRT_SEND == EDD_RQB_GET_SERVICE(pRQB))
            {
                EDDI_NRT_SEND_HOOK(pDDB->hSysDev, pRQB);
            }

            pSndBuffer = pRQBSnd->pBuffer;
            SndLength  = pRQBSnd->Length;
        }

        //init DMACW-Data-Buffer
        pDataBuffer         = pIF->adr_to_asic(pDDB->hSysDev, pSndBuffer, pDDB->ERTEC_Version.Location);
        pDMACW->pDataBuffer = EDDI_HOST2IRTE32(pDataBuffer);

        #if defined (EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)
        //correct HW-Autopadding (IRTE error)
        if (SndLength < EDDI_NRT_FRAME_BUFFER_MIN_LENGTH)
        {        
            LSA_UINT8  *  pSndEndBuf = pSndBuffer + SndLength;
            
            switch (SndLength & 0x3)
            {
                case 1:
                {
                    *pSndEndBuf++ = 0;
                    //no break!
                }
                //lint -fallthrough
                case 2:
                {
                    *pSndEndBuf++ = 0;
                    //no break!
                }
                //lint -fallthrough
                case 3:
                {            
                    *pSndEndBuf = 0;
                    break;
                }
                default:
                {
                }
            }
        }
        #endif //EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT

        NRT_SET_FrameLength(LocalL0, SndLength);

        if (EDD_PORT_ID_AUTO != UsrPortId)
        {
            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_SND_RawMode_11);

            if (EDD_PORT_ID_LOCAL == UsrPortId)
            {
                EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_LocalCH_15);
            }
            else
            {
                NRT_SET_SND_Port(LocalL0, pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId]);
            }
        }

        Service = EDD_RQB_GET_SERVICE(pRQB);

        //for SYNC we send with or without timestamp depending on TxTimePresent set
        //TxTimePresent was set by EDDI on request entry depending on FrameID

        //specific TS-Handling
        if (   (Service == EDD_SRV_NRT_SEND_TIMESTAMP)
            || ((Service == EDD_SRV_SYNC_SEND) && (pRQBSnd->TxTimePresent)))
        {
            //IRTE-Service NRT_SEND_TIMESTAMP

            #if defined (EDDI_CFG_FRAG_ON)
            #if defined (EDDI_TX_FRAG_DEBUG_MODE)
            if //FRAG RQB?
               (internal_context_1 & EDDI_TX_FRAG_RQB_MARK)
            {
                EDDI_Excp("EDDI_NRTSendWithLimit, Error: NRT TX fragmentation: NRT_SEND_TIMESTAMP as NRT fragment!", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
            #endif
            #endif

            #if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
            if //Mirror-Mode active? (Mirror-Mode can only be activated with compiler-switch "EDDI_CFG_MIRROR_MODE_INCLUDE")
               (IO_x32(MONITOR_CTRL))
            {
                //Mirror-Mode active -> sending not allowed -> rqb is send back immediately!
                //EDDI_NrtRevertSendLimit(pDDB, pRQB, pIF);
                #if defined (EDDI_CFG_REV5)
                EDDI_NRTTimeStamp(pDDB, EDDI_TS_CALLER_MIRROR_MODE);
                continue;
                #else
                {
                    EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

                    if (0 == pHDB->Cbf)
                    {
                        EDDI_Excp("EDDI_NRTSendWithLimit, no Cbf in HDB!", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }

                    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_NOT_SUPPORTED);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NRT_TS_MIRROR_MODE);
                    EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);
                    EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);
                    continue;
                }
                #endif
            }
            #endif //EDDI_CFG_MIRROR_MODE_INCLUDE

            #if defined (EDDI_CFG_REV5)
            {
                LSA_UINT32  const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
                LSA_UINT8   const  LinkStatus  = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus;

                switch (LinkStatus)
                {
                    case EDD_LINK_UP:
                    case EDD_LINK_UP_CLOSED:
                    {
                        LSA_UINT32  PhyBasicStatus;

                        PhyBasicStatus = EDDI_SwiMiscGetRegisterNoSwap(HwPortIndex, REG_PHY_STAT, pDDB);

                        //Link is up
                        if (1UL == EDDI_GetBitField32(PhyBasicStatus, EDDI_PHY_BASIC_STATUS_BIT__LinkStat))
                        {
                            //deactivate timeout
                            pDDB->NRT.TimeStamp.cTimeStamp = 0;
                            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_SND_PTFrame_26);
                            #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
                            EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTSendWithLimit, SPECIAL_TS_TRACE, IRTE-Service NRT_SEND_TIMESTAMP is put into a send-DMACW, IF-QueueIdx:0x%X ClockCntVal:0x%X",
                                                pIF->QueueIdx, IO_R32(CLK_COUNT_VALUE));
                            #endif
                            break;
                        }
                    }
                    //lint -fallthrough
                    default:
                    {
                        //no Link up -> sending not possible -> rqb is send back immediately!
                        //EDDI_NrtRevertSendLimit(pDDB, pRQB, pIF);
                        EDDI_NRTTimeStamp(pDDB, EDDI_TS_CALLER_LINK_DOWN);
                        continue;
                    }
                }
            }
            #else
            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_SND_PTFrame_26);
            #endif
        } //end of specific TS-Handling

        if (pRQBSnd->Priority >= EDD_NRT_SEND_PRIO_1)
        {
            EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_SND_SRTPrio_30);
        }

        //Disable 'Learning' of source MAC address on all outbound NRT frames.
        //Option not available on IRTE REV5.
        #if !defined (EDDI_CFG_REV5)
        EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_SND_LearningDisable_23);
        #endif

        #if defined (EDDI_CFG_REV5)
        /* If we send a Sync without FollowUp we must correct a possible set  */
        /* linedelay within ERTEC400 (which is added to the delayfield)       */
        /* We only modify if we send a sync without FollowUp! (in other cases */
        /* the linedelay will be corrected by GSY within FollowUP!            */
        if (Service == EDD_SRV_SYNC_SEND)
        {
            EDDI_SyncSnd_SyncFrameCCDELCorrection(pDDB, pRQB);
        }
        #endif

        //ATTENTION: do not change!
        #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
        //writeback nrt-cache
        EDDI_SYNC_CACHE_NRT_SEND(pSndBuffer, SndLength);
        #endif

        //deliver DMACW to IRTE
        EDDI_SET_SINGLE_BIT(LocalL0, BM_S_DMACW_OwnerHW_29);
        pDMACW->L0.Reg = LocalL0;

        //insert RQB into EDDI-send-descriptor (ring)
        pReq->pSndRQB = pRQB;

        //switch to next entry (DMACW)
        pReq              = pReq->next_dscr_ptr;
        pIF->Tx.Dscr.pReq = pReq;

        pIF->Tx.BalanceSndReq++;

        //IRTE-Trigger TX-Enable every nth time!
        EDDI_NRT_ENABLE_NRTRXTX_CHANNEL(pIF->Value_for_IO_NRT_ENABLE, (NRT_CHANNEL_ENABLE_BIT | NRT_TX_ENABLE)); 
    } //end of for-loop
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_tx.c                                                */
/*****************************************************************************/

