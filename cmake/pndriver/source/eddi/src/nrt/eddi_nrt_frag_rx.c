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
/*  F i l e               &F: eddi_nrt_frag_rx.c                        :F&  */
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
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#if defined (EDDI_CFG_DEFRAG_ON)
//#include "eddi_nrt_inc.h"
//#include "eddi_profile.h"
#include "eddi_nrt_filter.h"
//#include "eddi_nrt_rx.h"

#include "eddi_nrt_frag_rx.h"
//#include "eddi_swi_ext.h"

//#include "eddi_sync_usr.h"

#include "eddi_lock.h"

#if defined (EDDI_CFG_ERTEC_200)
#include "eddi_time.h"
#endif
//#include "eddi_nrt_ini.h"

#define EDDI_MODULE_ID     M_ID_NRT_FRAG_RX
#define LTRC_ACT_MODUL_ID  210

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitRxFrag()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_NRTInitRxFrag( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_NRT_RX_FRAG_TYPE  *  const  pNrtRxFragData = &pDDB->NRT.RxFragData;
    LSA_UINT32                       Index;
    LSA_UINT32                       SubIndex;
      
    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitRxFrag->");

    for (Index=EDDI_NRT_CHA_IF_0; Index<EDDI_NRT_IF_CNT; Index++) //A0, B0
    {
        pNrtRxFragData->NextExpectedFragmentNumber[Index] = 0;
        pNrtRxFragData->LastOffset[Index]                 = 0;
        pNrtRxFragData->LastFrameID[Index]                = 0;
        pNrtRxFragData->LastVLAN[Index]                   = 0;           
        pNrtRxFragData->LastFilterIndex[Index]            = 0;
        pNrtRxFragData->DstPayLoadDataOffset[Index]       = 0;
        pNrtRxFragData->SrcPayLoadDataOffset[Index]       = 0; 
        pNrtRxFragData->OriginalFilterIndex[Index]        = 0;
        pNrtRxFragData->ClaimedInterfaceNr[Index]         = EDDI_NRT_IF_CNT;
        
        pNrtRxFragData->bLastFrame[Index]                 = LSA_FALSE;  
        pNrtRxFragData->FrameLen[Index]                   = 0;  
        
        for (SubIndex = 0; SubIndex < EDD_MAC_ADDR_DA_SA_SIZE; SubIndex++)
        {
            pNrtRxFragData->LastMacDASA[Index][SubIndex] = 0;
        }  
    }
    
    for (Index = 0; Index <= EDDI_MAX_IRTE_PORT_CNT; Index++)
    {
        pNrtRxFragData->Statistic[Index].DiscardedRXFragments = 0;
        pNrtRxFragData->Statistic[Index].DiscardedRXFrames = 0;
        pNrtRxFragData->Statistic[Index].DiscardedRXFragments = 0;
        pNrtRxFragData->Statistic[Index].DiscardedRXFragments = 0;
    }

    pNrtRxFragData->bDefragOn = LSA_FALSE;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitRxFrag<-");
}                                                                                             
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRxFragBuildFrame()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDDI_NRT_FRAGMENT_STATE                          */
/*                                                                         */
/***************************************************************************/
EDDI_NRT_FRAGMENT_STATE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxFragBuildFrame( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                        EDD_UPPER_MEM_U8_PTR_TYPE     const  pDataBuffer,
                                                                        EDDI_NRT_CHX_SS_IF_TYPE    *  const  pIF,
                                                                        LSA_UINT32                 *  const  pUserDataOffset, //OUT
                                                                        LSA_BOOL                   *  const  pbSyncID1 )
{
    EDDI_NRT_RX_FRAG_TYPE        *  const  pNrtRxFragData = &pDDB->NRT.RxFragData;
    LSA_UINT8                              VLANSize;
    EDDI_TYPE_LEN_FRAME_ID_TYPE            FrameData;
    LSA_UINT8                              FragDataLength;
    LSA_UINT8                              FragStatus; 
    LSA_UINT8                              CurrentFragmentNumber; 
    LSA_UINT16                             CurrentFrameID;         
    LSA_UINT32                             CurrentFilterIndex;     
    LSA_UINT32                             Index;   

    FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)(void *)pDataBuffer)[3]; //Dest and Src start at offset 12

    /* -----------------------------------------------------------------------*/
    /* Now check for the Frametype                                            */
    /* -----------------------------------------------------------------------*/
    if (FrameData.w.TypeLen == EDDI_VLAN_TAG)
    {
        VLANSize = EDDI_NRT_FRAG_VLAN_SIZE;   
    }
    else
    {
        VLANSize = 0;
    }

    //Get Data from Fragment Header Frame
    FragDataLength             = ((EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pDataBuffer)[16 + VLANSize];
    FragStatus                 = ((EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pDataBuffer)[17 + VLANSize];
    CurrentFragmentNumber      = FragStatus & 0x1F;
    pNrtRxFragData->bLastFrame[pIF->QueueIdx] = !((FragStatus >> 7) & 1);
    CurrentFrameID             = ((EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pDataBuffer)[14 + VLANSize];
    CurrentFrameID             = (LSA_UINT16)(CurrentFrameID << 8 | (((EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pDataBuffer)[15 + VLANSize]));
    
    EDDI_NRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTRxFragBuildFrame-> FragNo: 0x%X, NextExeptFragNo: 0x%X, QueueIdx: 0x%X, LastFrag: 0x%X, FrameID: 0x%X", CurrentFragmentNumber, pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx], pIF->QueueIdx, pNrtRxFragData->bLastFrame[pIF->QueueIdx], CurrentFrameID);    
    
    //******************************************************************************************************************
    // 1. Check Fragment
    //******************************************************************************************************************
    
    //Not allowed smaller FragDataLength than 5 
    if (FragDataLength<5)
    {
        pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
        EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTRxFragBuildFrame: Not allowed smaller FragDataLength than 5 (FragDataLength: %X)", FragDataLength);
        return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
    }
    
    //Checks for fragment 0     
    //Dicard Fragment, if the 1st fragment was not received
    if (   (pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] != CurrentFragmentNumber)
        && (pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] == 0))
    {
        pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
        return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
    }

    //Checks for fragments 1..n   
    if (CurrentFragmentNumber)
    {       
        //Dicard Fragment, if next packet was not received
        if (pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] != CurrentFragmentNumber)
        {
            pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
            return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
        }
        
        //Dicard Fragment, if next MAC (DA, SA)does not match
        for (Index=0; Index < EDD_MAC_ADDR_SIZE*2; Index++)
        {
            if (pNrtRxFragData->LastMacDASA[pIF->QueueIdx][Index] != pDataBuffer[Index])
            {
                pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
                return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
            }
        }

        //Dicard Fragment, if next VLAN does not match
        if (pNrtRxFragData->LastVLAN[pIF->QueueIdx] != VLANSize)
        {
            pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
            return EDDI_NRT_DISCARDED_RX_FRAGMENTS;   
        }

        //Dicard Fragment, if the last FrameID was not the same
        if (CurrentFrameID != pNrtRxFragData->LastFrameID[pIF->QueueIdx])
        {
            pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
            return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
        } 
    }

    //Check for fragment n
    //Check Framelength without Header against FragDatasize, only the last can be other than 8 bit aligned
    if (!pNrtRxFragData->bLastFrame[pIF->QueueIdx])
    {
        if ( ((pNrtRxFragData->FrameLen[pIF->QueueIdx] - (EDDI_NRT_FRAG_DA_SA_MAC_SIZE + VLANSize + EDDI_NRT_ONLY_FRAG_HEADER)) % 8) != 0 )
        {
            pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTRxFragBuildFrame: FragmentNumber 0...n-1 is not 8 bit aligned (CurrentFragmentNumber: %X)", CurrentFragmentNumber);
            return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
        }    
    }
 
    //******************************************************************************************************************
    // 2. Build Fragment
    //******************************************************************************************************************

    //New Frame (fragment no = 0) is received, begin of frame
    if (CurrentFragmentNumber == 0)
    {
        //Fragment 0
                   
        //EDDI_MEMSET(pNrtRxFragData->NRTDeFragFrame, 0, EDDI_NRT_FRAG_MAX_FRAME_SIZE);
        pNrtRxFragData->LastOffset[pIF->QueueIdx]                 = 0;
        pNrtRxFragData->LastFrameID[pIF->QueueIdx]                = CurrentFrameID;
        pNrtRxFragData->ClaimedInterfaceNr[pIF->QueueIdx]         = pIF->QueueIdx;
        pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
        
        //Build Header
        //Copy DstMac.MacAdr and //Copy SrcMac.MacAdr
        EDDI_MEMCOPY(pNrtRxFragData->pNRTDeFragFrame[pIF->QueueIdx], pDataBuffer, EDDI_NRT_FRAG_DA_SA_MAC_SIZE);
        
        if (VLANSize)
        {
            //Copy VLAN
            EDDI_MEMCOPY(pNrtRxFragData->pNRTDeFragFrame[pIF->QueueIdx] + EDDI_NRT_FRAG_DA_SA_MAC_SIZE, pDataBuffer + EDDI_NRT_FRAG_DA_SA_MAC_SIZE, EDDI_NRT_FRAG_VLAN_SIZE);
        }

        //Copy Ethertype
        EDDI_MEMCOPY(pNrtRxFragData->pNRTDeFragFrame[pIF->QueueIdx] + EDDI_NRT_FRAG_DA_SA_MAC_SIZE + VLANSize, pDataBuffer + EDDI_NRT_FRAG_DA_SA_MAC_SIZE + VLANSize + EDDI_NRT_ONLY_FRAG_HEADER, EDDI_NRT_FRAG_ETHERTYPE_SIZE);

        pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx] = EDDI_NRT_FRAG_FIRST_HEADER_WITHOUT_VLAN + VLANSize;
        pNrtRxFragData->DstPayLoadDataOffset[pIF->QueueIdx] = EDDI_NRT_HEADER_WITHOUT_VLAN + VLANSize;

        //Copy PayLoad                                 
        EDDI_MEMCOPY(pNrtRxFragData->pNRTDeFragFrame[pIF->QueueIdx] + pNrtRxFragData->DstPayLoadDataOffset[pIF->QueueIdx], pDataBuffer + pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx], (LSA_UINT)(pNrtRxFragData->FrameLen[pIF->QueueIdx] - pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx]));

        //Get the extracted FrameIndex (Real FrameIndex), only on 1st Frame!
        CurrentFilterIndex = EDDI_NRTRxCheckFrame(pNrtRxFragData->pNRTDeFragFrame[pIF->QueueIdx], pIF, pNrtRxFragData->FrameLen[pIF->QueueIdx], pUserDataOffset, pDDB, pbSyncID1);

        //Discard Fragment, if PTCP and MRP was fragmented
        if (   (EDDI_NRT_FILTER_INDEX_PTCP_SYNC == CurrentFilterIndex)
            || (EDDI_NRT_FILTER_INDEX_MRP       == CurrentFilterIndex))
        {
            //Error. PTCP and MRP is not alowed to fragmentaion
            pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
            EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_NRTRxFragBuildFrame: PTCP or MRP was fragmented (CurrentFilterIndex: %X)", CurrentFilterIndex);
            return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
        }
        
        pNrtRxFragData->LastFilterIndex[pIF->QueueIdx] = CurrentFilterIndex; 
        pNrtRxFragData->LastFrameID[pIF->QueueIdx]     = CurrentFrameID;
        pNrtRxFragData->LastVLAN[pIF->QueueIdx]        = VLANSize;   
        //Copy MAC DA & SA for check 1..n fragments
        EDDI_MEMCOPY(pNrtRxFragData->LastMacDASA[pIF->QueueIdx], pDataBuffer, (LSA_UINT)EDD_MAC_ADDR_DA_SA_SIZE);
    }
    else
    {
        LSA_UINT32  FrameLenLoc;

        //Frames 1..n
        pNrtRxFragData->DstPayLoadDataOffset[pIF->QueueIdx] = 0;
        pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx] = EDDI_NRT_FRAG_DA_SA_MAC_SIZE + VLANSize + EDDI_NRT_ONLY_FRAG_HEADER;

        //Check resulting framelen 
        FrameLenLoc = pNrtRxFragData->LastOffset[pIF->QueueIdx] + /* pNrtRxFragData->DstPayLoadDataOffset[pIF->QueueIdx] omit (0) + */ pNrtRxFragData->FrameLen[pIF->QueueIdx] - pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx];
        if (FrameLenLoc > EDD_FRAME_BUFFER_LENGTH)
        {
            //Discard
            pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0;
            EDDI_NRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_NRTRxFragBuildFrame: Defrag framesize (%d) would exceed EDD_FRAME_BUFFER_LENGTH. FragmentNumber 0x%X, FrameID 0x%X, QueueIdx %d", 
                FrameLenLoc, CurrentFragmentNumber, CurrentFrameID, pIF->QueueIdx);
            return EDDI_NRT_DISCARDED_RX_FRAGMENTS;
        }
        else
        {
            //Copy PayLoad
            EDDI_MEMCOPY(pNrtRxFragData->pNRTDeFragFrame[pIF->QueueIdx] + pNrtRxFragData->LastOffset[pIF->QueueIdx], pDataBuffer + pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx], (LSA_UINT)(pNrtRxFragData->FrameLen[pIF->QueueIdx] - pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx]));  
        } 
    } 

    pNrtRxFragData->LastOffset[pIF->QueueIdx] = pNrtRxFragData->LastOffset[pIF->QueueIdx] + pNrtRxFragData->DstPayLoadDataOffset[pIF->QueueIdx] + pNrtRxFragData->FrameLen[pIF->QueueIdx] - pNrtRxFragData->SrcPayLoadDataOffset[pIF->QueueIdx];

    if (pNrtRxFragData->bLastFrame[pIF->QueueIdx])
    {                 
        pNrtRxFragData->FrameLen[pIF->QueueIdx]                   = pNrtRxFragData->LastOffset[pIF->QueueIdx];
        pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx] = 0; 
        pNrtRxFragData->LastOffset[pIF->QueueIdx]                 = 0;

        return EDDI_NRT_RECEIVED_FRAGMENTED_FRAMES;
    }
    else
    {
        pNrtRxFragData->NextExpectedFragmentNumber[pIF->QueueIdx]++;
    }

    return EDDI_NRT_RECEIVED_FRAGMENTED_FRAME;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRxFragSetStatitics()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxFragSetStatitics( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             EDDI_NRT_FRAGMENT_STATE  const  StateFrame )
{
    LSA_RESULT                       Result         = EDD_STS_OK;                                            
    EDDI_NRT_RX_FRAG_TYPE  *  const  pNrtRxFragData = &pDDB->NRT.RxFragData;

    EDDI_ENTER_CRITICAL_S();

    switch (StateFrame)
    {
        case EDDI_NRT_RECEIVED_FRAGMENTED_FRAMES:
        {
            pNrtRxFragData->Statistic[HwPortIndex].ReceivedFragmentedFrames++;
            break;
        }
        case EDDI_NRT_RECEIVED_NON_FRAGMENTED_FRAMES:
        {
            pNrtRxFragData->Statistic[HwPortIndex].ReceivedNonFragmentedFrames++;        
            break;
        }
        case EDDI_NRT_DISCARDED_RX_FRAMES:
        {
            pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFrames++;
            break;
        }
        case EDDI_NRT_DISCARDED_RX_FRAGMENTS:
        {
            pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFragments++;
            break;
        }
        case EDDI_NRT_RECEIVED_FRAGMENTED_FRAME:
        {
            //do nothing!
            break;
        }
        default:
        {
            Result = EDD_STS_ERR_PARAM;
            break;
        }                
    }
    
    EDDI_EXIT_CRITICAL_S();
       
    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*****************************************************************************/
/*  end of file eddi_nrt_frag_rx.c                                           */
/*****************************************************************************/



