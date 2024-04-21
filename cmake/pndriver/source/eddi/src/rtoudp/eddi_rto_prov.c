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
/*  F i l e               &F: eddi_rto_prov.c                           :F&  */
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

#define EDDI_MODULE_ID     M_ID_RTO_PROV
#define LTRC_ACT_MODUL_ID  126

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_RTO_PROV) //satisfy lint!
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_dev.h"
#include "eddi_crt_check.h"
#include "eddi_rto_check.h"
#include "eddi_crt_com.h"

#include "eddi_rto_prov.h"
#include "eddi_rto_frame.h"
#include "eddi_nrt_inc.h"

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RtoIniProvFrame( EDDI_CRT_PROVIDER_PTR_TYPE     const pProvider,
                                                          EDDI_LOCAL_DDB_PTR_TYPE        const pDDB);

static LSA_UINT16 EDDI_PChecksum ( EDD_UPPER_MEM_U16_PTR_TYPE   pBuf);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProvAddToQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                               EDDI_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                               EDDI_PROV_QUEUE_TYPE        *  const  pQueue );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProvRemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                                    EDDI_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                                    EDDI_PROV_QUEUE_TYPE        *  const  pQueue );


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoProvIni()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProvIni( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                  EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_UINT32                  const  LengthFrameID = 2;
    EDD_RQB_NRT_SEND_TYPE           *  pParamSnd;
    LSA_UINT32                         OffsetAPDU, OffsetRTData;
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP     = pDDB->RTOUDP.pIFNrtUDP;
    EDDI_MEMORY_MODE_TYPE       const  MemModeBuffer = pIFNrtUDP->MemModeBuffer;
    EDDI_USERMEMID_TYPE         const  UserMemID     = pIFNrtUDP->UserMemIDTXDefault;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProvIni->");

    OffsetRTData = EDDI_FRAME_HEADER_SIZE_WITH_VLAN + sizeof(EDDI_IP_HEADER_TYPE) + sizeof(EDDI_UDP_HEADER_TYPE) + LengthFrameID;

    pProvider->Udp.FrameLength = OffsetRTData + pProvider->LowerParams.DataLen;

    OffsetAPDU = pProvider->Udp.FrameLength;

    pProvider->Udp.FrameLength += sizeof(EDDI_CRT_DATA_APDU_STATUS);

    EDDI_DEVAllocMode(pDDB, MemModeBuffer, (void * *)&pProvider->Udp.Buf1.pFrame, pProvider->Udp.FrameLength, UserMemID);
    if (pProvider->Udp.Buf1.pFrame == EDDI_NULL_PTR)
    {
        return EDD_STS_ERR_RESOURCE;
    }

    EDDI_DEVAllocMode(pDDB, MemModeBuffer, (void * *)&pProvider->Udp.Buf2.pFrame, pProvider->Udp.FrameLength, UserMemID);
    if (pProvider->Udp.Buf2.pFrame == EDDI_NULL_PTR)
    {
        EDDI_DEVFreeMode(pDDB, MemModeBuffer, pProvider->Udp.Buf1.pFrame, UserMemID);
        return EDD_STS_ERR_RESOURCE;
    }

    EDDI_DEVAllocMode(pDDB, MemModeBuffer, (void * *)&pProvider->Udp.Buf3.pFrame, pProvider->Udp.FrameLength, UserMemID);
    if (pProvider->Udp.Buf3.pFrame == EDDI_NULL_PTR)
    {
        EDDI_DEVFreeMode(pDDB, MemModeBuffer, pProvider->Udp.Buf1.pFrame, UserMemID);
        EDDI_DEVFreeMode(pDDB, MemModeBuffer, pProvider->Udp.Buf2.pFrame, UserMemID);
        return EDD_STS_ERR_RESOURCE;
    }

    pProvider->Udp.Buf1.pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pProvider->Udp.Buf1.pFrame + OffsetAPDU);
    pProvider->Udp.Buf2.pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pProvider->Udp.Buf2.pFrame + OffsetAPDU);
    pProvider->Udp.Buf3.pAPDU = (EDDI_CRT_DATA_APDU_STATUS *)(void *)(pProvider->Udp.Buf3.pFrame + OffsetAPDU);

    pProvider->Udp.Buf1.pRTDataBuffer = pProvider->Udp.Buf1.pFrame + OffsetRTData;
    pProvider->Udp.Buf2.pRTDataBuffer = pProvider->Udp.Buf2.pFrame + OffsetRTData;
    pProvider->Udp.Buf3.pRTDataBuffer = pProvider->Udp.Buf3.pFrame + OffsetRTData;

    pProvider->Udp.Buf1.pIPHeader = (EDDI_UDP_XRT_FRAME_PTR_TYPE)(void *)&pProvider->Udp.Buf1.pFrame[EDDI_FRAME_HEADER_SIZE_WITH_VLAN];
    pProvider->Udp.Buf2.pIPHeader = (EDDI_UDP_XRT_FRAME_PTR_TYPE)(void *)&pProvider->Udp.Buf2.pFrame[EDDI_FRAME_HEADER_SIZE_WITH_VLAN];
    pProvider->Udp.Buf3.pIPHeader = (EDDI_UDP_XRT_FRAME_PTR_TYPE)(void *)&pProvider->Udp.Buf3.pFrame[EDDI_FRAME_HEADER_SIZE_WITH_VLAN];

    pProvider->Udp.pData = &pProvider->Udp.Buf1;
    pProvider->Udp.pUser = &pProvider->Udp.Buf2;
    pProvider->Udp.pNext = EDDI_NULL_PTR;
    pProvider->Udp.pFree = &pProvider->Udp.Buf1;

    EDDI_RtoIniProvFrame(pProvider, pDDB);

    EDDI_MemCopy(pProvider->Udp.Buf2.pFrame, pProvider->Udp.Buf1.pFrame, OffsetRTData );
    EDDI_MemCopy(pProvider->Udp.Buf3.pFrame, pProvider->Udp.Buf1.pFrame, OffsetRTData );

    EDD_RQB_SET_SERVICE(&pProvider->Udp.RQB, EDD_SRV_NRT_SEND);
    pProvider->Udp.RQB.internal_context = EDDI_NULL_PTR;

    pParamSnd = &pProvider->Udp.ParamSnd;
    pParamSnd->pBuffer  = pProvider->Udp.pData->pFrame;
    pParamSnd->PortID   = EDD_PORT_ID_AUTO;
    pParamSnd->Length   = pProvider->Udp.FrameLength;
    pParamSnd->Priority = EDD_NRT_SEND_PRIO_0;

    // Status frame is not in sendlist yet
    pParamSnd->LineDelayInTicksHardware = 1;

    pProvider->Udp.RQB.pParam = pParamSnd;

    {
        //calculate necessary DBs for this service
        LSA_UINT32  const  FrameLenInDBs = (pParamSnd->Length + (EDDI_NRT_DB_LEN - 1)) / EDDI_NRT_DB_LEN;
        LSA_UINT32  const  PrioIndex     = NRT_HIGH_PRIO_INDEX;

        pProvider->Udp.RQB.internal_context_1 = (LSA_UINT32)(FrameLenInDBs | (PrioIndex<<24));
    }

    pIFNrtUDP->Tx.LockFct(pIFNrtUDP);

    EDDI_TreeAdd(pDDB, pDDB->RTOUDP.pTreeSnd,
                 (LSA_UINT32)pProvider->LowerParams.CycleReductionRatio,
                 (LSA_UINT32)pProvider->LowerParams.CyclePhase,
                 (E_TREE_LIST *)(void *)pProvider);

    pIFNrtUDP->Tx.UnLockFct(pIFNrtUDP);

    pProvider->Udp.pDDB = pDDB;

    EDDI_RtoProvAddToQueue(pDDB, pProvider, &pDDB->CRT.UdpProvQueue);

    return EDD_STS_OK;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoIniProvFrame( EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                             EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDD_UPPER_MEM_U8_PTR_TYPE    pBuf0 = pProvider->Udp.pData->pFrame;
    LSA_UINT32                   i;
    LSA_UINT16                   VLANTag, Len;
    EDDI_UDP_XRT_FRAME_PTR_TYPE  pIPHeader;

    // Dest Mac
    for (i = 0; i < EDD_MAC_ADDR_SIZE; i++)
    {
        *pBuf0 = pProvider->LowerParams.DstMAC.MacAdr[i];
        pBuf0++;
    }

    // Src Mac
    for (i = 0; i < EDD_MAC_ADDR_SIZE; i++)
    {
        *pBuf0 = pDDB->Glob.LLHandle.xRT.MACAddressSrc.MacAdr[i];
        pBuf0++;
    }

    /* Setup VLAN-Tag with VLANId 0 and Priority */
    VLANTag = EDDI_SRT_VLAN_PRIO_CSRT << EDDI_SRT_VLAN_PRIO_SHIFT;

    pBuf0 = pProvider->Udp.pData->pFrame;

    EDDI_UDP_FRAME_SET_VLAN_TAG(pBuf0) = EDDI_VLAN_TAG;
    EDDI_UDP_FRAME_SET_VLAN_ID(pBuf0)  = EDDI_HTONS(VLANTag);
    EDDI_UDP_FRAME_SET_TYPELEN(pBuf0)  = EDDI_IP_TAG;

    pIPHeader = pProvider->Udp.pData->pIPHeader;

    Len = (LSA_UINT16)(pProvider->Udp.FrameLength - EDDI_FRAME_HEADER_SIZE_WITH_VLAN);

    /* insert IP-header */
    pIPHeader->Ip.VersionIHL     = EDDI_IP_XRT_VER_IHL;
    pIPHeader->Ip.TOS_DSCP       = pDDB->RTOUDP.TOS_DSCP;
    pIPHeader->Ip.TotalLength    = EDDI_HTONS(Len);
    pIPHeader->Ip.Identification = 0;
    pIPHeader->Ip.FragmentOffset = EDDI_HTONS(EDDI_IP_DF); //temporarily_disabled_lint !e778
    pIPHeader->Ip.TTL            = EDD_DEFAULT_TTL;  //Time to live
    pIPHeader->Ip.Protocol       = EDDI_IP_UDP;
    pIPHeader->Ip.Checksum       = 0;

    for (i = 0; i < EDD_IP_ADDR_SIZE; i++)
    {
        pIPHeader->Ip.SrcIP.b.IP[i]  = pDDB->Glob.LocalIP.b.IP[i];
        pIPHeader->Ip.DestIP.b.IP[i] = pProvider->LowerParams.UPD_DstIPAdr.b.IP[i];
    }

    pIPHeader->Ip.Checksum = EDDI_PChecksum((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)&pIPHeader->Ip);

    Len -= sizeof(pIPHeader->Ip);

    /* insert UDP-header */
    pIPHeader->Udp.SrcPort       = EDDI_xRT_UDP_PORT;
    pIPHeader->Udp.DstPort       = EDDI_xRT_UDP_PORT;
    pIPHeader->Udp.Length        = EDDI_HTONS(Len);
    pIPHeader->Udp.Checksum      = 0; /* not used */

    /* insert Frame-ID */
    pIPHeader->FrameID           = EDDI_HTONS(pProvider->LowerParams.FrameId);

    /* cyclecounter and datastatus will be inserted with every send */
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_PChecksum                              +*/
/*+  Input                      EDD_UPPER_MEM_U16_PTR_TYPE    pBuf          +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuf       : Pointer to start of 20 Bytes IP-Header                    +*/
/*+  Result     : Checksum                                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates IP-Header checksum.                            +*/
/*+               if result is 0 the checksum of the IP-Header is valid     +*/
/*+               Note: pBuffer points to a WORD aligned address            +*/
/*+               Note: There is a "optimized" checksum function within edd +*/
/*+                     (EDDI_PChecksumAndGetHeader()) used to check Header! +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_UINT16  EDDI_PChecksum( EDD_UPPER_MEM_U16_PTR_TYPE  pBuf )
{
    LSA_UINT32  Sum = 0;
    LSA_UINT32  Ctr;

    for (Ctr = 0; Ctr < (EDDI_IP_HEADER_SIZE / 2); Ctr++)
    {
        Sum += *pBuf++;
    }

    /* add carrys so we get ones-complement */
    Sum  = (Sum >> 16) + (Sum & 0xFFFF);
    Sum += (Sum >> 16);

    return ((LSA_UINT16)~Sum);
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProviderListAddEntry( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                   Status;
    EDDI_CRT_PROVIDER_PTR_TYPE                   pProvider;
    LSA_UINT8                                    ListType;
    LSA_UINT8                                    ProviderType;
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE      const  pCRTComp   = pDDB->pLocal_CRT;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProviderListAddEntry->");

    ListType     = EDDI_CRTGetListType(pProvParam->Properties, pProvParam->FrameID);
    ProviderType = EDDI_CRTGetProviderType(pDDB, pRQB, ListType);

    if (ProviderType == EDDI_RT_PROVIDER_TYPE_INVALID)
    {
        return EDD_STS_ERR_PARAM;
    }

    Status = EDDI_RtoProviderAddCheckRQB(pRQB, pDDB, ListType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    //search for free ProviderEntry in ProviderList
    Status = EDDI_CRTProviderListReserveFreeEntry(pDDB, &pProvider, pCRTComp, ProviderType, LSA_FALSE /*bIsDfp*/);
    if (Status != EDD_STS_OK)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoProviderListAddEntry, ERROR finding free Entry, Status:0x%X", Status);
        return Status;
    }

    Status = EDDI_CRTProviderInit(pDDB, pProvider, pProvParam, ListType, ProviderType);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    // Update LoadValue
    EDDI_RedTreeUpdatePathLoad(&pDDB->CRT.SrtRedTree,
                               pProvider->LowerParams.CycleReductionRatio,
                               pProvider->LowerParams.CyclePhase,
                               (LSA_INT32)pProvider->LowerParams.DataLen);

    // Set new Status
    pProvider->Status = EDDI_CRT_PROV_STS_INACTIVE;

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoProviderRemoveEvent()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProviderRemoveEvent( EDDI_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                              LSA_BOOL                    *  const  pIndicate )
{
    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB          = pProvider->Udp.pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP     = pDDB->RTOUDP.pIFNrtUDP;
    EDDI_MEMORY_MODE_TYPE       const  MemModeBuffer = pIFNrtUDP->MemModeBuffer;
    EDDI_USERMEMID_TYPE         const  UserMemID     = pIFNrtUDP->UserMemIDTXDefault;

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProviderRemoveEvent->pProvider->Status:0x%X", pProvider->Status);

    *pIndicate = LSA_TRUE;

    //check Status-Conditions
    if (   (pProvider->Status != EDDI_CRT_PROV_STS_INACTIVE)
        && (pProvider->Status != EDDI_CRT_PROV_STS_ACTIVE))
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    pIFNrtUDP->Tx.LockFct(pIFNrtUDP);

    EDDI_TreeRemove(pDDB, (E_TREE_LIST *)(void *)pProvider);

    pIFNrtUDP->Tx.UnLockFct(pIFNrtUDP);

    EDDI_DEVFreeMode(pDDB, MemModeBuffer, pProvider->Udp.Buf1.pFrame, UserMemID);
    EDDI_DEVFreeMode(pDDB, MemModeBuffer, pProvider->Udp.Buf2.pFrame, UserMemID);
    EDDI_DEVFreeMode(pDDB, MemModeBuffer, pProvider->Udp.Buf3.pFrame, UserMemID);

    EDDI_RtoProvRemoveFromQueue(pDDB, pProvider, &pDDB->CRT.UdpProvQueue);

    return EDD_STS_OK;
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProviderListSetGroupStatus( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProviderListSetGroupStatus->");

    if (pRQB || pDDB)
    {
    }

    return EDD_STS_OK;
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProvGetAPDUStatus( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,   
                                                          EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider )
{
    EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE  const  pRQBParam = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE)pRQB->pParam;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProvGetAPDUStatus->");

    pRQBParam->APDUStatus.CycleCnt = EDDI_NTOHS(pProvider->Udp.pData->pAPDU->Detail.CycleCnt);

    pRQBParam->Present = EDD_CSRT_APDU_STATUS_NOT_PRESENT;

    // Patch Bit_3

    pRQBParam->APDUStatus.DataStatus  =  pProvider->Udp.pData->pAPDU->Detail.DataStatus;

    EDDI_SET_DS_BIT__Bit_3(&pRQBParam->APDUStatus.DataStatus,  0);
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProviderChangePhase( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                              EDDI_CRT_PROVIDER_TYPE   *  const  pProvider,
                                                              LSA_UINT16                  const  newPhase,
                                                              LSA_UINT32                  const  newPhaseSequence )
{
    EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIFNrtUDP = pDDB->RTOUDP.pIFNrtUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProviderChangePhase->");

    // Check Params
    if (   (newPhase >  pProvider->LowerParams.CycleReductionRatio)
        || (newPhase == 0))
    {
        return EDD_STS_ERR_PARAM;
    }

    if (newPhase == pProvider->LowerParams.CyclePhase)
    {
        if (newPhaseSequence == pProvider->LowerParams.CyclePhaseSequence)
        {
            return EDD_STS_ERR_PARAM;
        }

        return EDD_STS_OK; // CyclePhaseSequence vorerst ignorieren
    }

    // Update Pathload
    EDDI_RedTreeUpdatePathLoad(&pDDB->CRT.SrtRedTree,
                               pProvider->LowerParams.CycleReductionRatio,
                               pProvider->LowerParams.CyclePhase,
                               (-1L) * pProvider->LowerParams.DataLen);

    // Change PhaseInfo of LowerParams
    pProvider->LowerParams.CyclePhase         = newPhase;
    pProvider->LowerParams.CyclePhaseSequence = newPhaseSequence;

    EDDI_RedTreeUpdatePathLoad(&pDDB->CRT.SrtRedTree,
                               pProvider->LowerParams.CycleReductionRatio,
                               pProvider->LowerParams.CyclePhase,
                               (LSA_INT32)pProvider->LowerParams.DataLen);

    pIFNrtUDP->Tx.LockFct(pIFNrtUDP);

    EDDI_TreeRemove(pDDB, (E_TREE_LIST *)(void *)pProvider);

    EDDI_TreeAdd(pDDB, pDDB->RTOUDP.pTreeSnd,
                 (LSA_UINT32)pProvider->LowerParams.CycleReductionRatio,
                 (LSA_UINT32)pProvider->LowerParams.CyclePhase,
                 (E_TREE_LIST *)(void *)pProvider);

    pIFNrtUDP->Tx.UnLockFct(pIFNrtUDP);

    return EDD_STS_OK;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProvAddToQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                               EDDI_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                               EDDI_PROV_QUEUE_TYPE        *  const  pQueue )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProvAddToQueue->");

    if (pQueue->pTop == EDDI_NULL_PTR)
    {
        pQueue->pTop = pProvider;
    }
    else
    {
        pQueue->pBottom->pNext = pProvider;
    }

    pQueue->Cnt++;

    pQueue->pBottom  = pProvider;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProvRemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB, 
                                                                    EDDI_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                                    EDDI_PROV_QUEUE_TYPE        *  const  pQueue )
{
    LSA_UINT32                  Ctr, number;
    EDDI_CRT_PROVIDER_PTR_TYPE  pAct, pPrev;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoProvRemoveFromQueue->");

    pPrev  = EDDI_NULL_PTR;
    pAct   = pQueue->pTop;

    number = pQueue->Cnt;

    for (Ctr = 0; Ctr < number; Ctr++)
    {
        if (pAct != pProvider)
        {
            pPrev = pAct;
            pAct  = pAct->pNext;
            continue;
        }

        //remove
        if (pPrev)
        {
            pPrev->pNext = pAct->pNext;

            if (pAct->pNext == EDDI_NULL_PTR)
            {
                pQueue->pBottom = pPrev;
            }
        }
        else
        {
            //first one -> update pFirst
            pQueue->pTop = pAct->pNext;
        }

        pQueue->Cnt--;
        return; //success
    }

    LSA_UNUSED_ARG(pDDB);
    EDDI_Excp("EDDI_RtoProvRemoveFromQueue", EDDI_FATAL_ERR_ISR, pProvider->LowerParams.FrameId, number);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif


/*****************************************************************************/
/*  end of file eddi_rto_prov.c                                              */
/*****************************************************************************/
