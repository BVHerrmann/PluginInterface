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
/*  F i l e               &F: edds_srt_prov.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDS. provider          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  145
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SRT_PROV */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

#include "edds_iobuf_int.h"

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                    Macros                                 */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderAddToSendList                +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             EDDS_SRT_CHAIN_BASE_TYPE* pSendList         +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProvider       +*/
/*+                             LSA_UINT16 ProviderIdx                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp       : Pointer to SRT management                            +*/
/*+  pSendList       : Pointer to send list                                 +*/
/*+  pProvider       : Pointer to provider                                  +*/
/*+  ProviderIdx     : index of provider                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: add a provider into a send list                           +*/
/*+                                                                         +*/
/*+               NOTE: A 16 bit write access is expected as an atomic      +*/
/*+                     operation.                                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderAddToSendList(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TO_CONST_TYPE pCSRTComp,
    EDDS_SRT_CHAIN_BASE_TYPE* pSendList,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProvider,
    LSA_UINT16 ProviderIdx)
{
    /* empty list */
    if( EDDS_SRT_CHAIN_IDX_END == pSendList->BottomIdx)
    {
        pProvider->SendLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pProvider->SendLink.Prev = EDDS_SRT_CHAIN_IDX_END;
        pSendList->TopIdx = ProviderIdx;

        /* THIS must be the last action. Adding the Provider to the    */
        /* Knot-Send-List.                                             */
        /* With this sequence we dont have a conflict situation if     */
        /* we currently sending from this list. because the sender     */
        /* only use the BottomIdx and Next and so the addition here is */
        /* the only point of accessing the same memory location. but   */
        /* this doesnt matter. Maybe the sender will use this new      */
        /* provider or not.                                            */
        pSendList->BottomIdx = ProviderIdx;
    }
    else
    {
        if (EDD_CYCLE_PHASE_SEQUENCE_LAST == pProvider->CyclePhaseSequence)
        {
            LSA_UINT16 IdxTopOld;
            /* insert at the end */
            pProvider->SendLink.Next = EDDS_SRT_CHAIN_IDX_END;
            pProvider->SendLink.Prev = pSendList->TopIdx;

            /* save old TopIdx */
            IdxTopOld = pSendList->TopIdx;

            pSendList->TopIdx = ProviderIdx;

            /* THIS must be the last action. Adding the Provider to the    */
            /* Knot-Send-List.                                             */
            /* With this sequence we dont have a conflict situation if     */
            /* we currently sending from this list. because the sender     */
            /* only use the BottomIdx and Next and so the addition here is */
            /* the only point of accessing the same memory location. but   */
            /* this doesnt matter. Maybe the sender will use this new      */
            /* provider or not.                                            */
            pCSRTComp->Prov.pProvTbl[IdxTopOld].SendLink.Next = ProviderIdx;
        }
        else
        {

            EDDS_CSRT_PROVIDER_TYPE_PTR pPrevProv = &(pCSRTComp->Prov.pProvTbl[pSendList->BottomIdx]);
            LSA_UINT16  IdxPrevProv = pSendList->BottomIdx;
            /* if provider is new first element */
            if(pProvider->CyclePhaseSequence < pPrevProv->CyclePhaseSequence)
            {
                pProvider->SendLink.Next = IdxPrevProv;
                pProvider->SendLink.Prev = EDDS_SRT_CHAIN_IDX_END;

                /* previously first provider references now new provider as Prev */
                pPrevProv->SendLink.Prev = ProviderIdx;

                /* THIS must be the last action. Adding the Provider to the    */
                /* Knot-Send-List.                                             */
                /* With this sequence we dont have a conflict situation if     */
                /* we currently sending from this list. because the sender     */
                /* only use the BottomIdx and Next and so the addition here is */
                /* the only point of accessing the same memory location. but   */
                /* this doesnt matter. Maybe the sender will use this new      */
                /* provider or not.                                            */
                pSendList->BottomIdx = ProviderIdx;
            }
            else
            {
                /* search index */
                while(EDDS_SRT_CHAIN_IDX_END != pPrevProv->SendLink.Next )
                {
                    EDDS_CSRT_PROVIDER_TYPE_PTR pNextProv = &(pCSRTComp->Prov.pProvTbl[pPrevProv->SendLink.Next]);
                    if(pProvider->CyclePhaseSequence < pNextProv->CyclePhaseSequence)
                    {
                        /* found */
                        break;
                    }
                    /* save idx of next provider in list */
                    IdxPrevProv = pPrevProv->SendLink.Next;
                    /* save next provider */
                    pPrevProv = pNextProv;

                }
                /* insert behind pLastProv */
                pProvider->SendLink.Next = pPrevProv->SendLink.Next;
                /* if pProvider is not the last element */
                if(EDDS_SRT_CHAIN_IDX_END != pPrevProv->SendLink.Next)
                {
                    pCSRTComp->Prov.pProvTbl[pPrevProv->SendLink.Next].SendLink.Prev = ProviderIdx;
                }
                else
                {
                    /* new provider is last provider now */
                    pSendList->TopIdx = ProviderIdx;
                }
                pProvider->SendLink.Prev = IdxPrevProv;

                /* THIS must be the last action. Adding the Provider to the    */
                /* Knot-Send-List.                                             */
                /* With this sequence we dont have a conflict situation if     */
                /* we currently sending from this list. because the sender     */
                /* only use the BottomIdx and Next and so the addition here is */
                /* the only point of accessing the same memory location. but   */
                /* this doesnt matter. Maybe the sender will use this new      */
                /* provider or not.                                            */
                pPrevProv->SendLink.Next = ProviderIdx;
            }
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetupFrameSrcMAC             +*/
/*+  Input                 :    EDD_UPPER_MEM_PTR_TYPE      pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE  pSrcMAC         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup a Framebuffer for provider with the given values    +*/
/*+               (parameters not checked!)                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pSrcMAC as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetupFrameSrcMAC(
    EDD_UPPER_MEM_PTR_TYPE              pBuffer,
    const EDDS_LOCAL_MEM_U8_PTR_TYPE    pSrcMAC)
{

    LSA_UINT32  i;

    for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[i+EDD_MAC_ADDR_SIZE]  = pSrcMAC[i]; //JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    }

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetupFrameXRT                +*/
/*+  Input                 :    EDD_UPPER_MEM_PTR_TYPE      pBuffer         +*/
/*+                             LSA_UINT32                  DataLen         +*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE  pDstMAC         +*/
/*+                             LSA_UINT16                  FrameID         +*/
/*+                             LSA_UINT16                  VLANID          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup a framebuffer for provider with the given values    +*/
/*+               (parameters not checked!)                                 +*/
/*+               Transferstatus is set to 0                                +*/
/*+               VLANID is a part of the VLAN-Tag.                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDstMAC as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetupFrameXRT(
    EDD_UPPER_MEM_PTR_TYPE              pBuffer,
    LSA_UINT32                          DataLen,
    const EDDS_LOCAL_MEM_U8_PTR_TYPE    pDstMAC,
    LSA_UINT16                          FrameID,
    LSA_UINT16                          VLANID)
{

    LSA_UINT32  i;
    LSA_UINT16  VLANTag;

    for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[i]  = pDstMAC[i]; //JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    }

    /* Setup VLAN-Tag with VLANId and Priority */

    VLANTag = (LSA_UINT16) ((( VLANID << EDDS_SRT_VLAN_ID_SHIFT ) & EDDS_SRT_VLAN_ID_MSK) |  //lint !e835 JB 18/11/2014 #def-based
                            ( EDDS_SRT_VLAN_PRIO_CSRT << EDDS_SRT_VLAN_PRIO_SHIFT));

    EDDS_SRT_FRAME_ACC_VLAN_TAG(pBuffer) = EDDS_VLAN_TAG; // JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    EDDS_SRT_FRAME_ACC_VLAN_ID(pBuffer) = EDDS_HTONS(VLANTag); //JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    EDDS_SRT_FRAME_ACC_TYPELEN(pBuffer) = EDDS_xRT_TAG; //JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    EDDS_SRT_FRAME_ACC_FRAMEID(pBuffer) = EDDS_HTONS(FrameID); //JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call

    /* cyclecounter and datastatus will be inserted with every send */

    /* TransferStatus */
    ((EDD_UPPER_MEM_U8_PTR_TYPE  )pBuffer)[EDDS_CSRT_FRAME_IO_START_WITH_VLAN+DataLen+EDDS_CSRT_FRAME_TRANSFER_STATUS_OFFSET] = 0; //JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call

}


#ifdef EDDS_XRT_OVER_UDP_INCLUDE

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderIPChecksum                   +*/
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
/*+               Note: There is a "optimized" checksum function within edds +*/
/*+                     (EDDS_IPChecksumAndGetHeader()) used to check Header! +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_UINT16 EDDS_RTProviderIPChecksum (
    EDD_UPPER_MEM_U16_PTR_TYPE pBuf)
{
    LSA_UINT32  Sum;
    LSA_UINT32  i;
    Sum = 0;

    for ( i=0; i < (EDDS_IP_HEADER_SIZE/2); i++ )
        Sum += *pBuf++;

    /* add carrys so we get ones-complement */
    Sum  = (Sum >> 16) + (Sum & 0xffff);
    Sum += (Sum >> 16);

    return((LSA_UINT16)~Sum);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetupFrameUDP                +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE      pBuffer         +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProv           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup a Framebuffer for provider with provider values for +*/
/*+               xRToverUDP                                                +*/
/*+               Transferstatus is set to 0                                +*/
/*+               VLANID is a part of the VLAN-Tag.                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetupFrameUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    EDD_UPPER_MEM_PTR_TYPE      pBuffer,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv)
{

    LSA_UINT32                 i;
    LSA_UINT16                 VLANTag;
    EDDS_UDP_XRT_FRAME_PTR_TYPE pIPHeader;
    LSA_UINT16                 Len;

    for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[i]  = pProv->DstMAC.MacAdr[i];
    }

    /* Setup VLAN-Tag with VLANId and Priority */

    VLANTag = (LSA_UINT16) ((( pProv->VLANID << EDDS_SRT_VLAN_ID_SHIFT ) & EDDS_SRT_VLAN_ID_MSK) |
                            ( EDDS_SRT_VLAN_PRIO_CSRT << EDDS_SRT_VLAN_PRIO_SHIFT));

    EDDS_SRT_FRAME_ACC_VLAN_TAG(pBuffer) = EDDS_VLAN_TAG;
    EDDS_SRT_FRAME_ACC_VLAN_ID(pBuffer) = EDDS_HTONS(VLANTag);
    EDDS_SRT_FRAME_ACC_TYPELEN(pBuffer) = EDDS_IP_TAG;

    pIPHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[EDDS_FRAME_HEADER_SIZE_WITH_VLAN];

    Len = sizeof(pIPHeader->Ip) + sizeof(pIPHeader->Udp) + sizeof(pIPHeader->FrameID) + pProv->DataLen;

    /* insert Transfer Status */
    ((EDD_UPPER_MEM_U8_PTR_TYPE  )pBuffer)[EDDS_FRAME_HEADER_SIZE_WITH_VLAN + Len + EDDS_CSRT_FRAME_TRANSFER_STATUS_OFFSET] = 0; /* TransferStatus */

    Len += EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE; /* IP-Len */

    /* insert IP-header */
    pIPHeader->Ip.VersionIHL     = EDDS_IP_XRT_VER_IHL;
    pIPHeader->Ip.TOS            = EDDS_IP_XRT_TOS;
    pIPHeader->Ip.TotalLength    = EDDS_HTONS(Len);
    pIPHeader->Ip.Identification = 0;
    pIPHeader->Ip.FragmentOffset = EDDS_HTONS(EDDS_IP_DF);
    pIPHeader->Ip.TTL            = pCSRTComp->IPParams.TTL;
    pIPHeader->Ip.Protocol       = EDDS_IP_UDP;
    pIPHeader->Ip.Checksum       = 0;

    for (i=0; i<EDD_IP_ADDR_SIZE; i++)
    {
        pIPHeader->Ip.SrcIP.b.IP[i]  = pProv->SrcIP.b.IP[i];
        pIPHeader->Ip.DestIP.b.IP[i] = pProv->DstIP.IPAdr[i];
    }

    pIPHeader->Ip.Checksum = EDDS_RTProviderIPChecksum((EDD_UPPER_MEM_U16_PTR_TYPE)&pIPHeader->Ip);

    Len -= sizeof(pIPHeader->Ip);

    /* insert UDP-header */
    pIPHeader->Udp.SrcPort       = EDDS_xRT_UDP_PORT;
    pIPHeader->Udp.DstPort       = EDDS_xRT_UDP_PORT;
    pIPHeader->Udp.Length        = EDDS_HTONS(Len);
    pIPHeader->Udp.Checksum      = 0; /* not used */

    /* insert Frame-ID */
    pIPHeader->FrameID           = EDDS_HTONS(pProv->FrameID);

    /* cyclecounter and datastatus will be inserted with every send */

}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetupCycleStart              +*/
/*+  Input                 :    EDDS_LOCAL_DDB_PTR_TYPE pDDB                +*/
/*+                             EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets ProviderCycle variables to start a Provider Cycle.   +*/
/*+               After this is done a TransmitTrigger will cause providers +*/
/*+               to be send. If link is down we dont start the cycle.      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pPCtrl as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetupCycleStart(
    EDDS_LOCAL_DDB_PTR_TYPE                   pDDB,
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR     pPCtrl)
{

    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE     pCSRTComp;
    LSA_UINT32                           Mask,KnotIndex;

    /* we only start cycle if the link is up. If the link is down, we */
    /* dont send anything. This is for optimization because if the    */
    /* link is down sending lasts very long and causes overruns which */
    /* delays other handling (i.g. NRT).                              */

    pCSRTComp = pDDB->pSRT;
    
#if !defined(EDDS_CFG_INTERNAL_IGNORE_AUTOPORT_DOWN)
    if ( pDDB->pGlob->LinkStatus[0].Status == EDD_LINK_UP ) /* check link of AUTO -> if any port is UP */
#endif
    {
        Mask                       = pPCtrl->MinInterval-1;
        KnotIndex                  = (Mask) + ((pCSRTComp->Prov.ProviderCycle.CycleCounter) & Mask);

        /* if knot not used set ActProvIdx to SRT_CHAIN_IDX_END */
        if ( pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx == EDDS_SRT_IDX_NOT_USED )
        {
            pPCtrl->ActProvIdx     = EDDS_SRT_CHAIN_IDX_END;
        }
        else
        {
            if (pPCtrl->Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
            {
                pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListXRT.BottomIdx;
            }
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            else /* udp */
            {
                pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListUDP.BottomIdx;
            }
            #endif
        }

        pPCtrl->ActInterval        = pPCtrl->MinInterval;
        pPCtrl->CycleDoSend        = LSA_TRUE;
        pPCtrl->CycleInProgress    = LSA_TRUE;
    }

    pPCtrl->CycleSendStarted   = LSA_FALSE;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderGetToSend                    +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR  pCSRTComp      +*/
/*+                             EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl +*/
/*+  Result                :    EDDS_CSRT_PROVIDER_TYPE_PTR                 +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to CSRT Management                                +*/
/*+  pPCtrl     : Pointer to provider control structure                     +*/
/*+                                                                         +*/
/*+  Results    : LSA_NULL: No provider found to send                       +*/
/*+               else pointer to provider to send.                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get next provider to send for the actual cycle.           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp or pPCtrl as null ptr's!
static EDDS_CSRT_PROVIDER_TYPE_PTR EDDS_LOCAL_FCT_ATTR EDDS_RTProviderGetToSend(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE          pCSRTComp,
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR     pPCtrl)
{

    LSA_BOOL                             Finished,Found;
    LSA_UINT32                           Mask,i,KnotIndex;
    EDDS_CSRT_PROVIDER_TYPE_PTR          pProv;
    EDDS_CSRT_CONSUMER_TYPE_PTR          pCons;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR    pConsFrame;

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetToSend(pCSRTComp: 0x%X, pPCtrl: 0x%X)",
                           pCSRTComp,
                           pPCtrl);

    pPCtrl->CycleSendStarted = LSA_TRUE;

    Found       = LSA_FALSE;
    Finished    = LSA_FALSE;
    pProv       = LSA_NULL;

    /* --------------------------------------------------------------------*/
    /* Search for next Provider to send or finished cycle.                 */
    /* --------------------------------------------------------------------*/

    while (( ! Found ) && ( ! Finished ))
    {

        /* Check the current SendList of Knot for more Provider to be send */
        /* If no more present we have to switch to the next knot..         */
        /* If one is present, we set Found to TRUE. and break the loop.    */

        if ( pPCtrl->ActProvIdx != EDDS_SRT_CHAIN_IDX_END )
        {
            pProv = &pCSRTComp->Prov.pProvTbl[pPCtrl->ActProvIdx];

            /* only if provider is active and correct type */
            if (pProv->Active)
            {
                /* must be the right provider !*/
                EDDS_ASSERT(pProv->Type == pPCtrl->Type);

                /* check if this provider has Autostop. If so we only send if */
                /* the Autostop is enabled and the Consumer is not in MISS    */

                if ( pProv->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
                {
                    EDDS_ASSERT(pProv->AutoStopConsumerID < pCSRTComp->Cons.ConsumerCnt);
                    pCons      = &pCSRTComp->Cons.pConsTbl[pProv->AutoStopConsumerID];
                    pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[pProv->AutoStopConsumerID];

                    /* Autostop enabled within Consumer? (Consumer is in use!)*/
                    if ( pCons->ProviderAutoStopEnabled )
                    {
                        /* check if Consumer Active (and no MISS detected)  */
                        if (pConsFrame->State >= EDDS_SRT_CONS_STATE_ON_0)
                        {
                            Found = LSA_TRUE; /* No Provider Autostop */
                        }
                        else
                        {
                            /* Autostop. Dont send Provider if consumer not active or MISS detected */
                            EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                                       "EDDS_RTProviderGetToSend: Provider Autostop (ProvID: %d,ConsID: %d)",
                                       pPCtrl->ActProvIdx,
                                       pProv->AutoStopConsumerID);
                        }
                    }
                    else
                    {
                        Found = LSA_TRUE; /* Autostop not enabled */
                    }
                }
                else /* no autostop */
                {
                    Found = LSA_TRUE;
                }

                if ( Found )
                {
                    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                                          "EDDS_RTProviderGetToSend: Provider to send: %d",
                                          pPCtrl->ActProvIdx);
                }

            }

            pPCtrl->ActProvIdx = pProv->SendLink.Next;

        }
        else
        {
            /* Switch to next knot.                                          */
            /* If this is the last knot we finish the current cycle.         */
            /* by setting Finished to LSA_TRUE                               */
            /* Note that MaxInterval maybe < ActInterval because MaxInterval */
            /*      may have been changed while handling this. So dont use   */
            /*      a == to detect end of loop                               */

            if ( pPCtrl->ActInterval < pPCtrl->MaxInterval )
            {

                i           = pPCtrl->ActInterval;
                i           = i*2;
                Mask        = i-1;
                KnotIndex   = (Mask) + ((pCSRTComp->Prov.ProviderCycle.CycleCounter) & Mask);


                EDDS_CRT_TRACE_03(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                                      "EDDS_RTProviderGetToSend: Switch to KnotIDX: %d, Int: %d, CyCnt: %d",
                                      KnotIndex,
                                      pPCtrl->ActInterval,
                                      pCSRTComp->Prov.ProviderCycle.CycleCounter);


                pPCtrl->ActInterval = (LSA_UINT16) i;

                /* if knot not used set ActProvIdx to SRT_CHAIN_IDX_END */
                if ( pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx == EDDS_SRT_IDX_NOT_USED )
                {
                    pPCtrl->ActProvIdx = EDDS_SRT_CHAIN_IDX_END;
                }
                else
                {
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    if ( pPCtrl->Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
                        #endif
                    {
                        pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListXRT.BottomIdx;
                    }
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    else  /* udp */
                    {
                        pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListUDP.BottomIdx;
                    }
                    #endif
                }

            }
            else /* all done for this cycle. No sends pending */
            {
                pPCtrl->CycleDoSend = LSA_FALSE;

                /* if the counter is already 0 (maybe because no sends neccessary for   */
                /* this cycle) we reset the CycleInProgress, because no sends underway  */
                /* and we are ready. If sends are underwqy the CycleInProgress flag     */
                /* will be reset after all frames were sent (see TxReadyCyclic-function)*/

                if ( ! pPCtrl->ActTxInProgress )
                {
                    pPCtrl->CycleInProgress = LSA_FALSE;
                }
                Finished = LSA_TRUE;
            }
        }
    }

    if ( ! Found ) pProv = LSA_NULL;

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetToSend(), pProv: 0x%X",
                           pProv);

    return(pProv);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderTxQueryCyclicXRT             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ... *pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA           No pending tx-requests       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will look for pending tx-requests for this  +*/
/*+               device and returns a pointer to the tx-buffer if one is   +*/
/*+               present. (Cyclic SRT-Requests)                            +*/
/*+                                                                         +*/
/*+   Calculation of KnotTableIndex:                                        +*/
/*+                                                                         +*/
/*+   Interval     = 1;                                                     +*/
/*+   KnotIndex    = 0;                                                     +*/
/*+                                                                         +*/
/*+   while (Interval < MaxInterval)                                        +*/
/*+   {                                                                     +*/
/*+      HandleKnotSendlist(KnotIndex)                                      +*/
/*+      Interval = Interval * 2                                            +*/
/*+      Mask     = Interval -1                                             +*/
/*+      KnotIndex = (Interval-1) + ( CycleCounter & Mask)                  +*/
/*+   }                                                                     +*/
/*+                                                                         +*/
/*+   IMPORTANT:  Must be synchronized with remove-provider!! this function +*/
/*+               must not be interrupted by a remove-provider request.     +*/
/*+                                                                         +*/
/*+               ProviderRemove adjusts counter on remove!                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 out param - pDDB checked where called
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxQueryCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength)

{
    LSA_RESULT                           Status;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE     pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR          pProv;
    LSA_UINT32                           i;
    LSA_UINT16                           Dummy;
    LSA_UINT8                            DataStatus;
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl;
    LSA_UINT32                           TraceIdx;
    EDD_UPPER_MEM_U8_PTR_TYPE            pBufferDataStatus;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderTxQueryCyclicXRT(pDDB: 0x%X)",
                           pDDB);

    Status             = EDD_STS_OK_NO_DATA;
    pCSRTComp          = pDDB->pSRT;
    pPCtrl             = &pCSRTComp->Prov.ProviderCycle.Xrt;
    pBufferDataStatus = LSA_NULL;

    if ( pPCtrl->CycleDoSend)
    {
        LSA_UINT16                     EDDS;
        EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;

        pProv = EDDS_RTProviderGetToSend(pCSRTComp,pPCtrl);

        /* --------------------------------------------------------------------*/
        /* If a provider is found to send. we determine the send buffer        */
        /* pProv points to the provider.                                       */
        /* --------------------------------------------------------------------*/

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pProv,LSA_NULL))
        {
            EDDS_EXCHANGE_TYPE Temp;

            // NOTE:
            // Information like index and new flag is always stored in one byte in
            // the EXCHANGE variables at its lowest addresses. This applies to EDDS and
            // IOBuffer when accessing these variables. Access to this byte by byte
            // pointer does not need any consideration of endianess.
            //
            // example:
            // Value stored in memory for Xchange with reference to buffer 1 and
            // new flag is 0x8100. Index and new flag can be retrieved by accessing
            // Xchange with its address casted to a byte pointer independently from
            // host endianess.
            //
            // index = (*((LSA_UINT8*)&Xchange)) & ~MASK

            pCr = &pCSRTComp->Prov.pProvCrTbl[pProv->CrNumber - 1];   // 1 .. ProviderCnt
            EDDS = *((LSA_UINT8*)&pCr->EDDS);

            /* The sendbuffer cannot be setup on provider add, because it ist */
            /* possible, that this buffer is currently sended. (very unlikely */
            /* because someone have to remove a provider and add a new one    */
            /* while the buffer is within the controller for send. So we setup*/
            /* the sendbuffer here, because we know it is not sended currently*/
            /* This setup is only done once after adding the provider.        */

            if (pProv->DoSetupBuffer )  /* must we setup the send-buffer ? */
            {
                EDDS_RTProviderSetupFrameXRT(pCr->pBuffer[EDDS],
                                             pProv->DataLen,
                                             pProv->DstMAC.MacAdr,
                                             pProv->FrameID,
                                             pProv->VLANID);

                pProv->DoSetupBuffer = LSA_FALSE;
            }

            Temp = pCr->XChange;

            if ( (*((LSA_UINT8*)&Temp)) & EDDS_IOBUFFER_MASK_NEW_FLAG )   /* new frame from user present ?   */
            {
                // NOTE: the content of Xchange may changed in the meantime, but not the new flag
                LSA_UINT8* pValue;

                Temp = pCr->EDDS;
                Temp = EDDS_IOBUFFER_EXCHANGE(&pCr->XChange, Temp);

                // reset new flag in Temp and set EDDS entry
                pValue    = ((LSA_UINT8*)&Temp);
                *pValue  &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
                pCr->EDDS = Temp;

                EDDS = *((LSA_UINT8*)&pCr->EDDS);
            }

            /* Now fill in DataStatus (Transferstatus is already set to 0)        */
            /* DataStatus consist of a global Provider State and a provider State */
            /* (will be filled even if no new buffer, because it may be changed)  */

            i = EDDS_CSRT_FRAME_IO_DATA_PROLOG_SIZE+pProv->DataLen;

            /* Insert Cycle-Counter. Based on 31.25us  */
            /* CycleCounter in Network-Byte order      */

            Dummy = ((LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter) << pCSRTComp->CycleStampAdjustShift));

            /* Tow accesses because of possible missalignment */
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET]   = (LSA_UINT8) (Dummy >> 8);   /* CycleCounter   */  //lint !e835 JB 18/11/2014 def-based
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET+1] = (LSA_UINT8) (Dummy & 0xFF); /* CycleCounter  */  //lint !e835 JB 18/11/2014 def-based

            /* For calculation the proper setting for the data status we use  */
            /* the following formula.                                         */
            /*                                                                */
            /* DataStatus = ((A|B) & WinMask ) | (A&B))                       */
            /*                                                                */
            /* A: Provider specific state (set per provider)         8 Bit    */
            /* B: Provider global state   (set per provider group)   8 Bit    */
            /* WinMask : decides which value (0 or 1) wins           8 Bit    */
            /*           0: 0 wins                                            */
            /*           1: 1 wins                                            */

            DataStatus = ((( pProv->DataStatus | pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus ) & EDD_CSRT_DSTAT_WIN_MASK ) |
                          ( pProv->DataStatus & pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus));
            
            pBufferDataStatus = ((EDD_UPPER_MEM_U8_PTR_TYPE )pCr->pBuffer[EDDS]) + (i + EDDS_CSRT_FRAME_DATA_STATUS_OFFSET);
            
#ifdef EDDS_CFG_SYSRED_API_SUPPORT            
            if( pCr->IsSystemRedundant == EDD_PROVADD_TYPE_SYSRED )
            {
                // take only the values STATE und REDUNDANCY Bits from EDDS_IOBuffer to DataStatus
                                        // reset only the STATE und REDUNDANCY Bits                 set STATE und REDUNDANCY Bits                  mask irrelevant Bits
                DataStatus = ( DataStatus & ~( EDD_CSRT_DSTAT_BIT_STATE | EDD_CSRT_DSTAT_BIT_REDUNDANCY )) | ( (*pBufferDataStatus) & ( EDD_CSRT_DSTAT_BIT_STATE | EDD_CSRT_DSTAT_BIT_REDUNDANCY ));                

            }
#endif
            /* DataStatus is located behind CycleCounter */
            *pBufferDataStatus = DataStatus; 

            *pBuffer = pCr->pBuffer[EDDS];
            *pLength = i+EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;


            EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                  "EDDS_RTProviderTxQueryCyclicXRT: Provider to send: FrameID: 0x%X, Buffer: 0x%X, Len: %d",
                                  pProv->FrameID,
                                  *pBuffer,
                                  *pLength);

            #ifdef EDDS_CSRT_PROVIDER_INCLUDE_SEND_COUNTER
            pProv->SendCounter++;
            #endif

            pPCtrl->ActTxInProgress++;
            Status = EDD_STS_OK;


        }
    }

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderTxQueryCyclicXRT(*pBuffer: 0x%X, Length: %d), Status: 0x%X",
                           *pBuffer,
                           *pLength,
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderTxReadyCyclicXRT             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_RESULT                     TxStatus     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  TxStatus   : tx-status of "inprogress" tx. put into RQB                +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after a provider was sent (XRT)                    +*/
/*+                                                                         +*/
/*+               We check if this was the last provider for this cycle.    +*/
/*+               if so we set CycleInProgress to FALSE.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as a null ptr!
//@fixme am2219 NEA6.0 function refactoring
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxReadyCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_RESULT              TxStatus)
{
    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    LSA_UINT32                      TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderTxReadyCyclicXRT(pDDB: 0x%X, TxStatus: %d)",
                           pDDB,
                           TxStatus);

    Status = EDD_STS_OK;
    pCSRTComp = pDDB->pSRT;

    /* If Cycle in progress and no more sends pending and the */
    /* Tx-Counter reaches 0 we are ready for this cycle.      */

    if  ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress  )
    {
        pCSRTComp->Prov.ProviderCycle.Xrt.ActTxInProgress--;

        if (( ! pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend       ) &&
            ( ! pCSRTComp->Prov.ProviderCycle.Xrt.ActTxInProgress   ))
        {

            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "EDDS_RTProviderTxReadyCyclicXRT: xRT Provider Cycle ready");

            pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress = LSA_FALSE;
        }
    }

    LSA_UNUSED_ARG(TxStatus);


    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderTxReadyCyclicXRT(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}



#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderTxQueryCyclicUDP             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ... *pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA           No pending tx-requests       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will look for pending tx-requests for this  +*/
/*+               device and returns a pointer to the tx-buffer if one is   +*/
/*+               present. (Cyclic SRT-Requests)                            +*/
/*+                                                                         +*/
/*+   Calculation of KnotTableIndex:                                        +*/
/*+                                                                         +*/
/*+   Interval     = 1;                                                     +*/
/*+   KnotIndex    = 0;                                                     +*/
/*+                                                                         +*/
/*+   while (Interval < MaxInterval)                                        +*/
/*+   {                                                                     +*/
/*+      HandleKnotSendlist(KnotIndex)                                      +*/
/*+      Interval = Interval * 2                                            +*/
/*+      Mask     = Interval -1                                             +*/
/*+      KnotIndex = (Interval-1) + ( CycleCounter & Mask)                  +*/
/*+   }                                                                     +*/
/*+                                                                         +*/
/*+   IMPORTANT:  Must be synchronized with remove-provider!! this function +*/
/*+               must not be interrupted by a remove-provider request.     +*/
/*+                                                                         +*/
/*+               ProviderRemove adjusts counter on remove!                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxQueryCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength)

{
    LSA_RESULT                           Status;
    EDDS_DDB_COMP_CSRT_TYPE_PTR          pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR          pProv;
    LSA_UINT32                           i;
    LSA_UINT16                           Dummy;
    EDD_UPPER_MEM_PTR_TYPE               pTemp;
    LSA_UINT8                            DataStatus;
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl;
    LSA_UINT32                           TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderTxQueryCyclicUDP(pDDB: 0x%X)",
                           pDDB);


    Status    = EDD_STS_OK_NO_DATA;
    pCSRTComp = &pDDB->pSRT->CSRT;
    pPCtrl    = &pCSRTComp->Prov.ProviderCycle.Udp;

    if ( pPCtrl->CycleDoSend)
    {
        LSA_UINT16                     EDDS;
        EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;

        pProv = EDDS_RTProviderGetToSend(pCSRTComp,pPCtrl);

        /* --------------------------------------------------------------------*/
        /* If a provider is found to send. we determine the send buffer        */
        /* pProv points to the provider.                                       */
        /* --------------------------------------------------------------------*/

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pProv,LSA_NULL))
        {
            EDDS_EXCHANGE_TYPE Temp;

            pCr = &pCSRTComp->Prov.pProvCrTbl[pProv->CrNumber - 1];   // 1 .. ProviderCnt
            EDDS = *((LSA_UINT8*)&pCr->EDDS);

            /* The sendbuffer cannot be setup on provider add, because it is  */
            /* possible, that this buffer is currently sended. (very unlikely */
            /* because someone have to remove a provider and add a new one    */
            /* while the buffer is within the controller for send. So we setup*/
            /* the sendbuffer here, because we know it is not sended currently*/
            /* This setup is only done once after adding the provider.        */

            if (pProv->DoSetupBuffer )  /* must we setup the send-buffer ? */
            {
                EDDS_RTProviderSetupFrameUDP(pCSRTComp,
                                             pCr->pBuffer[EDDS],
                                             pProv);

                pProv->DoSetupBuffer = LSA_FALSE;
            }

            Temp = pCr->XChange;

            if ( (*((LSA_UINT8*)&Temp)) & EDDS_IOBUFFER_MASK_NEW_FLAG )   /* new frame from user present ?   */
            {
                LSA_UINT8* pValue;

                Temp = pCr->EDDS;
                Temp = EDDS_IOBUFFER_EXCHANGE(&pCr->XChange, Temp);

                // reset new flag in Temp and set EDDS entry
                pValue    = ((LSA_UINT8*)&Temp);
                *pValue  &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
                pCr->EDDS = Temp;

                EDDS = *((LSA_UINT8*)&pCr->EDDS);
            }

            /* Now fill in DataStatus (Transferstatus is already set to 0)        */
            /* DataStatus consist of a global Provider State and a provider State */
            /* (will be filled even if no new buffer, because it may be changed)  */

            i = EDDS_FRAME_HEADER_SIZE_WITH_VLAN +
                sizeof(EDDS_IP_HEADER_TYPE)  +
                sizeof(EDDS_UDP_HEADER_TYPE) +
                sizeof(EDDS_FRAME_ID_TYPE)   +
                pProv->DataLen;  /* start of APDU-Status within frame */

            /* Insert Cycle-Counter. Based on 1ms      */
            /* CycleCounter in Network-Byte order      */

            Dummy = ((LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter) << (pCSRTComp->CycleStampAdjustShift - 5) ));

            /* Tow accesses because of possible missalignment */
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET]   = (LSA_UINT8) (Dummy >> 8);   /* CycleCounter   */
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET+1] = (LSA_UINT8) (Dummy & 0xFF); /* CycleCounter  */

            /* For calculation the proper setting for the data status we use  */
            /* the following formula.                                         */
            /*                                                                */
            /* DataStatus = ((A|B) & WinMask ) | (A&B))                       */
            /*                                                                */
            /* A: Provider specific state (set per provider)         8 Bit    */
            /* B: Provider global state   (set per provider group)   8 Bit    */
            /* WinMask : decides which value (0 or 1) wins           8 Bit    */
            /*           0: 0 wins                                            */
            /*           1: 1 wins                                            */

            DataStatus = ((( pProv->DataStatus | pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus ) & EDD_CSRT_DSTAT_WIN_MASK ) |
                          ( pProv->DataStatus & pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus));

            /* DataStatus is located behind CycleCounter */

            ((EDD_UPPER_MEM_U8_PTR_TYPE )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_DATA_STATUS_OFFSET] = DataStatus;

            *pBuffer = pCr->pBuffer[EDDS];
            *pLength = i+EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;

            EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                                  "EDDS_RTProviderTxQueryCyclicUDP: Provider to send: FrameID: 0x%X, Buffer: 0x%X, Len: %d",
                                  pProv->FrameID,
                                  *pBuffer,
                                  *pLength);

            #ifdef EDDS_CSRT_PROVIDER_INCLUDE_SEND_COUNTER
            pProv->SendCounter++;
            #endif

            pPCtrl->ActTxInProgress++;
            Status = EDD_STS_OK;


        }
    }


    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderTxQueryCyclicUDP(*pBuffer: 0x%X, Length: %d), Status: 0x%X",
                           *pBuffer,
                           *pLength,
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderTxReadyCyclicUDP             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_RESULT                     TxStatus     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  TxStatus   : tx-status of "inprogress" tx. put into RQB                +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after a provider was sent (xRToverUDP)             +*/
/*+                                                                         +*/
/*+               We check if this was the last provider for this cycle.    +*/
/*+               if so we set CycleInProgress to FALSE.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderTxReadyCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_RESULT              TxStatus)
{
    LSA_RESULT                      Status;
    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRTComp;
    LSA_UINT32                      TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderTxReadyCyclicUDP(pDDB: 0x%X, TxStatus: %d)",
                           pDDB,
                           TxStatus);


    Status = EDD_STS_OK;

    pCSRTComp = &pDDB->pSRT->CSRT;

    /* If Cycle in progress and no more sends pending and the */
    /* Tx-Counter reaches 0 we are ready for this cycle.      */

    if  ( pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress  )
    {
        pCSRTComp->Prov.ProviderCycle.Udp.ActTxInProgress--;

        if (( ! pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend       ) &&
            ( ! pCSRTComp->Prov.ProviderCycle.Udp.ActTxInProgress   ))
        {
            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "EDDS_RTProviderTxReadyCyclicUDP: UDP Provider Cycle ready");

            pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress = LSA_FALSE;

        }
    }

    LSA_UNUSED_ARG(TxStatus);

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderTxReadyCyclicUDP(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}
#endif


/** \brief Indicate provider cycle overload.
 *
 * This function cancels the actual cycle, sets internal statistic counter and indicates the cause of
 * provider cycle overload in trace.
 *
 * \author	am2219
 * \date	2015-06-22
 *
 * \param	pDDB		reference to read only Device Description Block for trace index.
 * \param	pCSRTComp	reference to read only CSRT component data.
 *
 */
static LSA_VOID EDDS_RTProviderIndicateCycleOverload(EDDS_LOCAL_DDB_PTR_TYPE const     pDDB, EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE const     pCSRTComp)
{
    LSA_UINT32 TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    pCSRTComp->Stats.ProvCycleOverloadCnt++; /* statistics */

    /* indicate only if providers active */
    if ( pCSRTComp->Prov.ProviderActCnt)
    {
        EDDS_CRT_TRACE_04(TraceIdx,EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL,
                              "EDDS_RTProviderCycleRequest: Overload %d, cycle %d is not ready while trying to send provider (%d/%d TX in progress).",
                              pCSRTComp->Stats.ProvCycleOverloadCnt,
                              pCSRTComp->Prov.ProviderCycle.CycleCounter,
                              pDDB->pGlob->TxInProgCnt,
                              pDDB->pGlob->TxBuffCnt);
        /* additional cycles to drop */
        pCSRTComp->Prov.ProviderCycle.OverloadDropCycles = pCSRTComp->Cfg.ProviderCycleDropCnt;
    }else
    {
        EDDS_CRT_TRACE_04(TraceIdx,EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL,
                              "EDDS_RTProviderCycleRequest: Overload %d, cycle %d not ready (%d/%d TX in progress).",
                              pCSRTComp->Stats.ProvCycleOverloadCnt,
                              pCSRTComp->Prov.ProviderCycle.CycleCounter,
                              pDDB->pGlob->TxInProgCnt,
                              pDDB->pGlob->TxBuffCnt);
    }

    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderCycleRequest                 +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Provider Cycle-Timer function.                            +*/
/*+               This function is called after a provider-timer-request    +*/
/*+               occured                                                   +*/
/*+                                                                         +*/
/*+               This request will start a new Provider send cycle.        +*/
/*+               We check if we are still in cycle-handling. if so         +*/
/*+               we have an overload an stop the actual cycle and set      +*/
/*+               and overload indication marker.                           +*/
/*+                                                                         +*/
/*+               On Overload we check if we should drop cycles and do it.  +*/
/*+                                                                         +*/
/*+               if no overload (and no drop sequence in progress) we will +*/
/*+               start a new provider cycle by setting a flag and trigger  +*/
/*+               transmission.                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    LSA_UINT32 TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderCycleRequest(pDDB: 0x%X)",
                           pDDB);

    pCSRTComp = pDDB->pSRT;

    pCSRTComp->Prov.ProviderCycle.CycleCounter++;

    /*---------------------------------------------------------------------------*/
    /* if we are curently in OverloadDrop we only increment the cycleCounter and */
    /* continue..                                                                */
    /*---------------------------------------------------------------------------*/

    if ( pCSRTComp->Prov.ProviderCycle.OverloadDropCycles )
    {
        pCSRTComp->Prov.ProviderCycle.OverloadDropCycles--;

        EDDS_CRT_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDS_RTProviderCycleRequest: Dropping cycle %d (and the next %d) because of overload! (%d/%d TX in progress)",
                              pCSRTComp->Prov.ProviderCycle.CycleCounter,
                              pCSRTComp->Prov.ProviderCycle.OverloadDropCycles,
                              pDDB->pGlob->TxInProgCnt,
                              pDDB->pGlob->TxBuffCnt);
    }
    else
    {
        /* ----------------------------------------------------------------------*/
        /* We check if Cycle is in progress. if so we have an overload situation */
        /* We cancel the actual cycle and set the overload flag for indication   */
        /* ----------------------------------------------------------------------*/

        if (   ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress )
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
               || ( pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress )
            #endif
           )
        {
            EDDS_RTProviderIndicateCycleOverload(pDDB,pCSRTComp);
        }
        else /* no overload and provider cycle not running. good situation */
        {

            EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "EDDS_RTProviderCycleRequest: Starting provider cycle %d",
                                  pCSRTComp->Prov.ProviderCycle.CycleCounter);


            EDDS_RTProviderSetupCycleStart(pDDB,&pCSRTComp->Prov.ProviderCycle.Xrt);
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            EDDS_RTProviderSetupCycleStart(pDDB,&pCSRTComp->Prov.ProviderCycle.Udp);
            #endif
            EDDS_TransmitTrigger(pDDB,LSA_FALSE); /* Trigger a transmission for this dev (including cyclic RT) */
            /* This cause a query for this all     */
            /* components. even for this provider. */

            /* Do some error checks with Cycle based send mode...            */

            if (   ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress )
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                   || ( pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress )
                #endif
               )
            {

                /* if we cant start the cycle, because tx still running (this      */
                /* may occur, if we have no provider active but long NRT-Frames    */
                /* still in hardware) we have a overload situation because we cant */
                /* start sending. But only with cycle based send-mode,             */
                /* because we only start sending at cycle start (and cant here)    */
                /* with other modes this does not matter, because the cycle was    */
                /* still ready and send is triggered within NRT. This situation    */
                /* may occur in 10Mbit Mode if we send 1500Byte NRT-Frames. This   */
                /* lasts about 1.5ms. With this we will delay till next cycle...   */

                /* Note. XRT is send first, so we only need to check for XRT       */
                if ( ! pCSRTComp->Prov.ProviderCycle.Xrt.CycleSendStarted )
                {

                    /* this never should occur !*/

                    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                          "EDDS_RTProviderCycleRequest: Overload in cycle %d, provider TX still in work (%d/%d TX in progress).",
                                          pCSRTComp->Prov.ProviderCycle.CycleCounter,
                                          pDDB->pGlob->TxInProgCnt,
                                          pDDB->pGlob->TxBuffCnt);

                    /* stop this cycle, because we couldnt start it */
                    pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress = LSA_FALSE;
                    pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend     = LSA_FALSE;

                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    /* stop this cycle, because we couldnt start it */
                    pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress = LSA_FALSE;
                    pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend     = LSA_FALSE;
                    #endif

                    EDDS_RTProviderIndicateCycleOverload(pDDB,pCSRTComp);
                }
                else
                {
                    /* if we started sending, but dont be able to send all       */
                    /* cyclic frames at once we dont have enough tx-resources in */
                    /* lower layer. This is a TX resource overload situation (not enough TX descriptors for this cycle). */

                    if (    ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend )
                        #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                            || ( pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend )
                        #endif
                       )
                    {
                        ++pCSRTComp->Stats.ProvCycleOutDiscards; /* increment out discard statistics */

                        EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderCycleRequest: "
                                "not all providers could be send in cycle %d, "
                                "because there are not enough TX buffers available (%d/%d TX in progress).",
                                pCSRTComp->Prov.ProviderCycle.CycleCounter,
                                pDDB->pGlob->TxInProgCnt,
                                pDDB->pGlob->TxBuffCnt);
                    }
                }
            }
        }
    }

    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderCycleRequest()");

    LSA_UNUSED_ARG(TraceIdx);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :  EDDS_RTProviderGetKnotAndPhase                        +*/
/*+  Input/Output  :  EDDS_DDB_COMP_CSRT_TYPE_PTR            pCSRTComp      +*/
/*+                   EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE   pRqbParam      +*/
/*+                   EDDS_CSRT_KNOT_TYPE_PTR LOCAL_MEM_ATTR *pKnot         +*/
/*+                   EDDS_LOCAL_MEM_U32_PTR_TYPE            pKnotTableIdx  +*/
/*+                   EDDS_LOCAL_MEM_U16_PTR_TYPE            pCyclePhase    +*/
/*+                                                                         +*/
/*+  Result        :  LSA_RESULT                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to CSRT-Managment                        +*/
/*+  pRqbParam           : Pointer to RQB-Parameter for provider add        +*/
/*+  pKnot               : on return: pointer to address of knot-table entry+*/
/*+  pKnotTableIdx       : on retrun: pointer to address of knot-table idx  +*/
/*+  pCyclePhase         : on return: pointer to address of CyclePhase      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_LIMIT_REACHED                                 +*/
/*+               EDD_STS_ERR_PHASE_MASKED                                  +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Depending on the CyclePhase and ReductionRatio            +*/
/*+               we will return a valid knot-table-index and the CyclePhase+*/
/*+               for this provider to use                                  +*/
/*+                                                                         +*/
/*+               Searches for best phase if the user wants to.             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB) - pKnotElement out param
//WARNING: be careful when using this function, make sure not to use pCSRTComp or pProv as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetKnotAndPhase(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE             pCSRTComp,
    EDDS_CSRT_PROVIDER_TYPE_PTR                  pProv,
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR EDDS_LOCAL_MEM_ATTR *pKnotElement)
{


    LSA_UINT32             Interval;
//    LSA_UINT32             ProvBytesFree;
//    LSA_UINT32             ProvCntFree;
    LSA_UINT16             CyclePhase;
    LSA_RESULT             Status;

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetKnotAndPhase(pCSRTComp: 0x%X, pProv: 0x%X)",
                           pCSRTComp,
                           pProv);


    Status = EDD_STS_OK;

    /* Interval == ReductionRatio */
    Interval   = pProv->CycleReductionRatio;
    CyclePhase = pProv->CyclePhase;

    pProv->CyclePhase   = CyclePhase;

    if ( CyclePhase > Interval ) /* out of range ? */
    {
        Status = EDD_STS_ERR_PARAM;

        EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderGetKnotAndPhase: Wrong CyclePhase (%d). Max: %d",
                            CyclePhase,
                            Interval);

    }
    else
    {
        /* Get free provider bytes and cnt for this ReductionRatio and Phase. If  */
        /* no limit the values will be set to 0xFFFFFFFF which will most likely   */
        /* not be reached. So we dont have to do a special check later            */
        //ProvBytesFree = 0xFFFFFFFF;
        //ProvCntFree   = 0xFFFFFFFF;

        /* calculate Knot-Table index                           */
        /* (Interval-1) + (CyclePhase-1)                        */
        /* example for Interval = 4 and CyclePhase =2 : Idx = 4 */

        pProv->KnotTableIdx  = Interval+CyclePhase-2;
        pProv->CyclePhase    = CyclePhase;


        /* Get Knot with filled in KnotElement */
        Status = EDDS_RTKnotGet(pCSRTComp,pProv->KnotTableIdx);
        if(EDD_STS_OK == Status)
        {
            *pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[pProv->KnotTableIdx].KnotElementIdx];
        }
    }

    EDDS_CRT_TRACE_03(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetKnotAndPhase(KTIdx: %d, CyPhase: %d), Status: 0x%X",
                           pProv->KnotTableIdx,
                           pProv->CyclePhase,
                           Status);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetMinMaxRatioUsedXRT        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to start CSRT-management                 +*/
/*+                                                                         +*/
/*+  Result     : void                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs KnotTable for the CycleReductionRatio for the     +*/
/*+               maximum ProviderByteCnt and ProviderCnt and sets the value+*/
/*+               within provider  cycle control                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetMinMaxRatioUsedXRT(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp)
{

    LSA_UINT32              Max;
    LSA_UINT32              Min;
    LSA_UINT32              i;

    Min = EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX-1;
    Max = EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1;  //lint !e778 JB 18/11/2014 #def-based - should be 0

    for (i=EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1; i<EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX; i++)  //lint !e778 JB 18/11/2014 #def-based - should be 0
    {
        /* something present ? */
        if ( pCSRTComp->Prov.ProvRatioInfo[i].CntXRT )
        {
            if ( i < Min ) Min = i;
            if ( i > Max ) Max = i;
        }
    }

    if ( Min > Max ) /* the case if nothing present at all. set Min = Max */
    {
        Min = Max;
    }

    /* Note: We may be interrupted by cyclic send routine here. So it may   */
    /*       be possible that Max is updated, but Min not. This dont realy  */
    /*       care, because its only the border till we check for providers  */
    /*       and it dont matters if we check more or less. So we dont       */
    /*       protect this sequence.                                         */

    pCSRTComp->Prov.ProviderCycle.Xrt.MaxInterval = (LSA_UINT16) EDDSGet2Potenz(Max);
    pCSRTComp->Prov.ProviderCycle.Xrt.MinInterval = (LSA_UINT16) EDDSGet2Potenz(Min);

}


#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetMinMaxRatioUsedUDP        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to start CSRT-management                 +*/
/*+                                                                         +*/
/*+  Result     : void                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs KnotTable for the CycleReductionRatio for the     +*/
/*+               maximum ProviderByteCnt and ProviderCnt and sets the value+*/
/*+               within provider  cycle control                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetMinMaxRatioUsedUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp)
{

    LSA_UINT32              Max;
    LSA_UINT32              Min;
    LSA_UINT32              i;

    Min = EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MAX-1;
    Max = EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1;

    for (i=EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1; i<EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MAX; i++)
    {
        /* something present ? */
        if ( pCSRTComp->Prov.ProvRatioInfo[i].CntUDP )
        {
            if ( i < Min ) Min = i;
            if ( i > Max ) Max = i;
        }
    }

    if ( Min > Max ) /* the case if nothing present at all. set Min = Max */
    {
        Min = Max;
    }

    /* Note: We may be interrupted by cyclic send routine here. So it may   */
    /*       be possible that Max is updated, but Min not. This dont realy  */
    /*       care, because its only the border till we check for providers  */
    /*       and it dont matters if we check more or less. So we dont       */
    /*       protect this sequence.                                         */

    pCSRTComp->Prov.ProviderCycle.Udp.MaxInterval = (LSA_UINT16) EDDSGet2Potenz(Max);
    pCSRTComp->Prov.ProviderCycle.Udp.MinInterval = (LSA_UINT16) EDDSGet2Potenz(Min);

}
#endif


//JB 06/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderInsert(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE        pCSRTComp,
        LSA_UINT16                              Idx)
{
    LSA_RESULT                              Status;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement = LSA_NULL;
    LSA_UINT16                              ReductionFactor; /* 1..x */

    pProv           = &pCSRTComp->Prov.pProvTbl[Idx];

    ReductionFactor = pProv->CycleReductionNr +1;

    if(EDD_CYCLE_PHASE_UNDEFINED == ReductionFactor) {
        ReductionFactor = EDDS_RTGetAndCheckReductionFactor(
                pProv->CycleReductionRatio,
                (LSA_BOOL) (pProv->Type ==  EDDS_CSRT_PROVIDER_TYPE_UDP)
        );
    }

    if(0 == ReductionFactor)
    {
        Status = EDD_STS_ERR_PARAM;
    }else
    {
        pProv->CycleReductionNr     =    ReductionFactor-1;
        /*----------------------------------------------------------------- */
        /* Find and/or check for valid Phase, set it in Provider and get    */
        /* and get Knot-table Element                                       */
        /*----------------------------------------------------------------- */

        Status = EDDS_RTProviderSetKnotAndPhase(pCSRTComp,
                                                pProv,
                                                &pKnotElement
                                                );

        if(EDD_STS_OK == Status) {

            /*------------------------------------------------------------- */
            /* Put provider to send-list in knot management(at end of list) */
            /* sequence in list currently not supported!                    */
            /*------------------------------------------------------------- */

            if ( EDDS_CSRT_PROVIDER_TYPE_UDP != pProv->Type )
            {
                EDDS_RTProviderAddToSendList(pCSRTComp,&pKnotElement->ProviderSendListXRT,pProv,Idx);
            }
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            else
            {
                EDDS_RTProviderAddToSendList(pCSRTComp,&pKnotElement->ProviderSendListUDP,pProv,Idx);
            }
            #endif
            pKnotElement->ProviderByteCnt += pProv->DataLen; /* sum of all datalen  */

            pKnotElement->ProviderCnt++;

            if ( EDDS_CSRT_PROVIDER_TYPE_UDP == pProv->Type)
            {
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].CntUDP++;
                EDDS_RTProviderSetMinMaxRatioUsedUDP(pCSRTComp);
                #endif
            }
            else
            {
                pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].CntXRT++;
                EDDS_RTProviderSetMinMaxRatioUsedXRT(pCSRTComp);
            }
        }
        else {
            EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderADD: EDDS_RTProviderGetKnotAndPhase() failed (0x%X)",
                                Status);
        }
    }
    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderADD                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_ADD_TYPE                    +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_ADD_TYPE                                         +*/
/*+                                                                         +*/
/*+  FrameID                 : FrameID for Provider.                        +*/
/*+  DstMAC                  : Destination MAC address                      +*/
/*+  DataLen                 : DataLen of I/O-Data in frame.                +*/
/*+  CycleReductionRatio     : spezifies CycleTime based on CycleBaseFactor +*/
/*+                            (1,2,4,8,16,32,64,128,256,512)               +*/
/*+  CyclePhase              : spezifies Phase within cycle.                +*/
/*+  CyclePhaseSequence      : spezifies Sequence in Phase (not supported)  +*/
/*+  VLANID                  : VLAN-ID put into frame.                      +*/
/*+  UserID                  : UserID for Event-indications.                +*/
/*+  GroupID                 : GroupID for ProviderSetState                 +*/
/*+  ProviderID              : returned ProviderID.                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_LIMIT_REACHED                                 +*/
/*+               EDD_STS_ERR_PHASE_MASKED                                  +*/
/*+               EDD_STS_ERR_NO_LOCAL_IP                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to add a new provider (xRT and xRTOverUDP)        +*/
/*+                                                                         +*/
/*+               First we check if free providers present. if not we       +*/
/*+               return EDD_STS_ERR_RESOURCE. Then we check for several    +*/
/*+               valid parameters.                                         +*/
/*+                                                                         +*/
/*+               Next we queue the provider within "used-chain" and put    +*/
/*+               the provider to the desired Knot-Send-Chain. (depends     +*/
/*+               on CycleReductionRation and CyclePhase).                  +*/
/*+                                                                         +*/
/*+               We also preinitialize all three send-frames with the      +*/
/*+               given values (MACs, FrameID, VLAN-ID etc.) which will not +*/
/*+               change.                                                   +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderADD(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_UINT16                              Idx;
    LSA_RESULT                              Status;
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE    pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE        pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    LSA_UINT16                              ReductionFactor = 0; /* 1..x */
    LSA_UINT16                              RtClassProp;
    LSA_BOOL                                Udp;
    EDDS_LOCAL_DDB_PTR_TYPE     			pDDB;
    LSA_UINT32                              TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderADD(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    Udp             = LSA_FALSE;

    pRqbParam   = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE) pRQB->pParam;
    pCSRTComp   = pDDB->pSRT;
    RtClassProp = pRqbParam->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK;

    /*---------------------------------------------------------------------------*/
    /* get a free provider from the free list                                    */
    /*---------------------------------------------------------------------------*/

    Idx = pCSRTComp->Prov.ProviderFree.BottomIdx;

    /* Check if we have a free provider entry                   */
    /* this is true if the bottom-idx in the free list is valid */

    if ( Idx == EDDS_SRT_CHAIN_IDX_END )
    {

        EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderADD: No more free provider!");

        Status = EDD_STS_ERR_RESOURCE;
    }

    if(EDD_STS_OK == Status)
    {
        /* ---------------------------------------------------- */
        /* check Properties for RT classes                      */
        /* ---------------------------------------------------- */
        if( RtClassProp != EDD_CSRT_PROV_PROP_RTCLASS_1 &&
            RtClassProp != EDD_CSRT_PROV_PROP_RTCLASS_2 )
        {
            Status = EDD_STS_ERR_PARAM;
        } /* check Properties for RT classes */

        /* ---------------------------------------------------- */
        /* check Properties flags                               */
        /* ---------------------------------------------------- */
#ifndef EDDS_CFG_SYSRED_API_SUPPORT
        if (pRqbParam->Properties & EDD_CSRT_PROV_PROP_SYSRED)
        {
            Status = EDD_STS_ERR_PARAM;
            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderADD: System redundancy is not supported!");
        }
#endif        
        if ((pRqbParam->Properties & EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP) ||
            (pRqbParam->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
        {
            Status = EDD_STS_ERR_PARAM;
            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderADD: PDU Format DFP or RTC3 Mode Legacy are not supported!");
        } /* check Properties flags */
    }


    if(EDD_STS_OK == Status)
    {
        /*-----------------------------------------------------------------------*/
        /* check for xRToverUdp or XRT Provider ADD and for DataLen              */
        /*-----------------------------------------------------------------------*/

        if ( (EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SRT_PROVIDER_ADD) && (EDD_CSRT_PROV_PROP_RTCLASS_UDP == RtClassProp) )
        {
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            if (( pRqbParam->DataLen  < EDD_CSRT_UDP_DATALEN_MIN ) ||
                ( pRqbParam->DataLen  > EDD_CSRT_UDP_DATALEN_MAX ))
            {

                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderADD: Invalid UDP DataLen (%d)",
                                    pRqbParam->DataLen);

                Status = EDD_STS_ERR_PARAM;
            }
            else
            {
                if (pDDB->pGlob->HWParams.IPAddress.dw == 0 ) /* no IP present ? */
                {
                    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderADD: No local IP set!");

                    Status = EDD_STS_ERR_NO_LOCAL_IP;

                }
            }

            Udp = LSA_TRUE;
            #else
            Status = EDD_STS_ERR_SERVICE;   /* should not occure because already checked outside */
            #endif
        }
        else
        {
            if (( pRqbParam->DataLen  < EDD_CSRT_DATALEN_MIN ) ||
                ( pRqbParam->DataLen  > EDD_CSRT_DATALEN_MAX ))
            {
                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderADD: Invalid XRT DataLen (%d)",
                                    pRqbParam->DataLen);

                Status = EDD_STS_ERR_PARAM;
            }
            Udp = LSA_FALSE;
        }

        /*---------------------------------------------------------------------------*/
        /* get ReductionFactor(1..x) from ReductionRatio. 0 signals invalid Ratio    */
        /*---------------------------------------------------------------------------*/
        if(EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pRqbParam->CycleReductionRatio)
        {
            ReductionFactor = EDDS_RTGetAndCheckReductionFactor(pRqbParam->CycleReductionRatio,Udp);

            if(0 == ReductionFactor)
            {
                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderADD: wrong CycleReductionRatio");
                Status = EDD_STS_ERR_PARAM;
            }
        } /* check CycleReductionRatio */

        /* ---------------------------------------------------- */
        /* check CyclePhase                                     */
        /* ---------------------------------------------------- */
        if ( (EDD_CYCLE_PHASE_UNDEFINED != pRqbParam->CyclePhase) &&
             (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pRqbParam->CycleReductionRatio) )
        {
            if (pRqbParam->CyclePhase > pRqbParam->CycleReductionRatio)
            {
                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderADD: wrong CyclePhase");
                Status = EDD_STS_ERR_PARAM;
            }
        } /* check CyclePhase */

        /* ---------------------------------------------------- */
        /* check GroupID                                        */
        /* ---------------------------------------------------- */
        if ( EDD_CFG_CSRT_MAX_PROVIDER_GROUP <= pRqbParam->GroupID )
        {
            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderADD: Invalid GroupID!");

            Status = EDD_STS_ERR_PARAM;
        } /* check GroupID */

        /*-----------------------------------------------------------------------*/
        /* check FrameID                                                         */
        /*-----------------------------------------------------------------------*/
        if( EDD_FRAME_ID_UNDEFINED != pRqbParam->FrameID)
        {
            /* ---------------------------------------------------- */
            /* check FrameID                                        */
            /* --> FrameID must match RTClass                       */
            /* --> check FrameID area (RTClass boundary/configured  */
            /* ---------------------------------------------------- */
            switch (RtClassProp)
            {
                case EDD_CSRT_PROV_PROP_RTCLASS_1:
                {
                    if ((EDD_SRT_FRAMEID_CSRT_START2 > pRqbParam->FrameID) ||
                        (EDD_SRT_FRAMEID_CSRT_STOP2  < pRqbParam->FrameID))
                    {
                        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                            "[H:--] EDDS_RTProviderADD(): FrameID (0x%X) does not match RTClass",
                            pRqbParam->FrameID);
                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                break;

                case EDD_CSRT_PROV_PROP_RTCLASS_2:
                {
                    if ((EDD_SRT_FRAMEID_CSRT_START1 > pRqbParam->FrameID) ||
                        (EDD_SRT_FRAMEID_CSRT_STOP1  < pRqbParam->FrameID))
                    {
                        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                            "[H:--] EDDS_RTProviderADD(): FrameID (0x%X) does not match RTClass",
                            pRqbParam->FrameID);
                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                break;

                default:
                {
                    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        "[H:--] EDDS_RTProviderADD(): invalid RTClassProperty (0x%X)",
                        RtClassProp);
                    Status = EDD_STS_ERR_PARAM;
                }
                break;
            } /* check FrameID */
        }

        /* ---------------------------------------------------- */
        /* check border values of crnumber                      */
        /* ---------------------------------------------------- */
        if( EDD_STS_OK == Status )
        {
            if(    (EDD_CRNUMBER_NOT_USED  == pRqbParam->IOParams.CRNumber)
                || (   (pCSRTComp->Cfg.ProviderCnt <  pRqbParam->IOParams.CRNumber)
                    && (EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber) ) )
            {
                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderADD: Invalid cr number!");

                Status = EDD_STS_ERR_PARAM;
            }
        }/* check border values of crnumber */

        /* ---------------------------------------------------- */
        /* check that each crnumber is used max one time        */
        /* ---------------------------------------------------- */
        if( EDD_STS_OK == Status )
        {
            LSA_UINT16 CrTableIterator;

            if(EDD_CRNUMBER_UNDEFINED == pRqbParam->IOParams.CRNumber)
            {
                /* CrNumber won't be saved */
            }
            else
            {
                for( CrTableIterator = 0 ; CrTableIterator < pCSRTComp->Cfg.ProviderCnt ; CrTableIterator++ )
                {
                    //check that crnumber is not in use
                    if(    (pRqbParam->IOParams.CRNumber == pCSRTComp->Prov.pProvTbl[CrTableIterator].CrNumber)
                             && ( EDD_CRNUMBER_NOT_USED !=  pCSRTComp->Prov.pProvTbl[CrTableIterator].CrNumber) )
                    {
                        EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderADD: crnumber is already in use!");

                        Status = EDD_STS_ERR_PARAM;
                        break;
                    }
                }
            }
        }/* check that each crnumber is used max one time */

        if ( Status == EDD_STS_OK )
        {
            EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                "EDDS_RTProviderADD: PID: %d, Ratio: %d",
                                Idx,
                                pRqbParam->CycleReductionRatio);

            pProv        = &pCSRTComp->Prov.pProvTbl[Idx];

            /*------------------------------------------------------------- */
            /* copy parameter to management structure                       */
            /*------------------------------------------------------------- */
            pProv->KnotTableIdx			=    0xFFFFFFFF;		/* not enqueued yet. */
            pProv->CyclePhase           =    pRqbParam->CyclePhase;		///am2219
            pProv->CyclePhaseSequence   =    pRqbParam->CyclePhaseSequence;
            pProv->CycleReductionRatio  =    pRqbParam->CycleReductionRatio;
            pProv->CycleReductionNr     =    ReductionFactor-1;
            pProv->DataLen              =    pRqbParam->DataLen;
            pProv->FrameID              =    pRqbParam->FrameID;
            pProv->UserID               =    pRqbParam->UserID;
            pProv->GroupID              =    pRqbParam->GroupID;

            pRqbParam->ProviderID = Idx;	/* Provider ID is the index within */
                                            /* provider table                  */

            pProv->Type                 =   Udp ? EDDS_CSRT_PROVIDER_TYPE_UDP:EDDS_CSRT_PROVIDER_TYPE_XRT;
            pProv->Properties           =   pRqbParam->Properties;
            pProv->Active               =   LSA_FALSE;

            
#ifdef EDDS_CFG_SYSRED_API_SUPPORT
            if (pRqbParam->Properties & EDD_CSRT_PROV_PROP_SYSRED)
            {
                pProv->DataStatus = EDD_CSRT_SRD_DSTAT_LOCAL_INIT_VALUE;
            }
            else
#endif
            {
                pProv->DataStatus = EDD_CSRT_DSTAT_LOCAL_INIT_VALUE;
            }
             
            pProv->AutoStopConsumerID   =   EDD_CONSUMER_ID_REMOVE_AS; /* none */

            if(EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber)
            {
                pProv->CrNumber             =   pRqbParam->IOParams.CRNumber;
            }

            pCSRTComp->Prov.ProviderUsedCnt++;

            /* setup framebuffer in EDDS_RTProviderControl */

            /*------------------------------------------------------------- */
            /* Remove Provider at bottom from free-chain list               */
            /*------------------------------------------------------------- */

            /* if this was the last free provider. set Idx to END-tag       */
            if ( pProv->UsedFreeLink.Next == EDDS_SRT_CHAIN_IDX_END )
            {
                pCSRTComp->Prov.ProviderFree.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pCSRTComp->Prov.ProviderFree.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* not the last provider */
                pCSRTComp->Prov.ProviderFree.BottomIdx = pProv->UsedFreeLink.Next;
                pCSRTComp->Prov.pProvTbl[pProv->UsedFreeLink.Next].UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
            }

            /*------------------------------------------------------------- */
            /* Put Provider to used-chain list (end)                        */
            /*------------------------------------------------------------- */

            /* nothing in queue ? */
            if ( pCSRTComp->Prov.ProviderUsed.TopIdx == EDDS_SRT_CHAIN_IDX_END )
            {
                pCSRTComp->Prov.ProviderUsed.BottomIdx   = Idx;
                pCSRTComp->Prov.ProviderUsed.TopIdx      = Idx;
                pProv->UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
                pProv->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                pProv->UsedFreeLink.Prev = pCSRTComp->Prov.ProviderUsed.TopIdx;
                pProv->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
                pCSRTComp->Prov.pProvTbl[pCSRTComp->Prov.ProviderUsed.TopIdx].UsedFreeLink.Next = Idx;
                pCSRTComp->Prov.ProviderUsed.TopIdx = Idx;
            }

            if( !( EDD_CYCLE_PHASE_UNDEFINED == pProv->CyclePhase ||
                   EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == pProv->CyclePhaseSequence ||
                   EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pProv->CycleReductionRatio
                  ))
            {
                Status = EDDS_RTProviderInsert(pCSRTComp,Idx);
            }

        }
    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderADD, Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderChangeSendListXRT            +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProv           +*/
/*+                             LSA_UINT32                  KnotTableIdxNew +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT           : Pointer to CSRT-Management                           +*/
/*+  ProviderID      : ID of provider to change phase                       +*/
/*+  pProv           : Pointer to provider magm to change phase             +*/
/*+  KnotTableIdxNew : New KnottableIndex for this provider                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes provider from on knot-table send-list and puts    +*/
/*+               him to another. this is done with protection against      +*/
/*+               high prio context of edds.                                +*/
/*+                                                                         +*/
/*+               Note: For both phases a knot element must be present.     +*/
/*+                     After change knot we try to free the old knot elem. +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT or pProv as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderChangeSendListXRT(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
    LSA_UINT16                  ProviderID,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv,
    LSA_UINT32                  KnotTableIdxNew)
{


    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_TYPE_PTR                 pKnotNew;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElementNew;
    LSA_UINT32                              KnotTableIdx;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;

    KnotTableIdx    = pProv->KnotTableIdx;
    pKnot           = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
    EDDS_ASSERT(pKnot->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
    pKnotElement    = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];
    pKnotNew        = &pCSRT->Knot.pKnotTbl[KnotTableIdxNew];
    EDDS_ASSERT(pKnotNew->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
    pKnotElementNew = &pCSRT->Knot.pKnotElementTbl[pKnotNew->KnotElementIdx];

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    /* Because this sequence runs in RQB context and must not */
    /* be interrupted by scheduler we have to use EDDS_ENTER */

    EDDS_ENTER(pDDB->hSysDev);

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */
                pKnotElement->ProviderSendListXRT.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pKnotElement->ProviderSendListXRT.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
                pKnotElement->ProviderSendListXRT.BottomIdx = Next;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
            pKnotElement->ProviderSendListXRT.TopIdx = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
        pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* correct ActProvIdx for sender.. may only be neccessary                    */
    /* if sending NOT in cyclic blockmode, where sending is                      */
    /* done not in one part.                                                     */
    /*---------------------------------------------------------------------------*/

    if ( pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx  = Next;
    }

    /* protecting area end here */
    EDDS_EXIT(pDDB->hSysDev);

    /*---------------------------------------------------------------------------*/
    /* Now we put the provider to the new knot list                              */
    /*---------------------------------------------------------------------------*/

    pProv->KnotTableIdx     =  KnotTableIdxNew;

    EDDS_RTProviderAddToSendList(pCSRT,&pKnotElementNew->ProviderSendListXRT,pProv,ProviderID);

    /*---------------------------------------------------------------------------*/
    /* Actualize the limits                                                      */
    /*---------------------------------------------------------------------------*/

    pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
    pKnotElement->ProviderCnt--;

    pKnotElementNew->ProviderByteCnt += pProv->DataLen; /* sum of all datalen  */
    pKnotElementNew->ProviderCnt++;

    /* free old Knot Element if not used anymore */
    EDDS_RTKnotFree(pCSRT,KnotTableIdx);

}


#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderChangeSendListUDP            +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProv           +*/
/*+                             LSA_UINT32                  KnotTableIdxNew +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT           : Pointer to CSRT-Management                           +*/
/*+  ProviderID      : ID of provider to change phase                       +*/
/*+  pProv           : Pointer to provider magm to change phase             +*/
/*+  KnotTableIdxNew : New KnottableIndex for this provider                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes provider from on knot-table send-list and puts    +*/
/*+               him to another. this is done with protection against      +*/
/*+               high prio context of edds.                                +*/
/*+                                                                         +*/
/*+               Note: For both phases a knot element must be present.     +*/
/*+                     After change knot we try to free the old knot elem. +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderChangeSendListUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    LSA_UINT16                  ProviderID,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv,
    LSA_UINT32                  KnotTableIdxNew)
{


    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_TYPE_PTR                 pKnotNew;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElementNew;
    LSA_UINT32                              KnotTableIdx;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;

    KnotTableIdx    = pProv->KnotTableIdx;
    pKnot           = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
    EDDS_ASSERT(pKnot->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
    pKnotElement    = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];
    pKnotNew        = &pCSRT->Knot.pKnotTbl[KnotTableIdxNew];
    EDDS_ASSERT(pKnotNew->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
    pKnotElementNew = &pCSRT->Knot.pKnotElementTbl[pKnotNew->KnotElementIdx];

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    /* Because this sequence runs in RQB context and must not */
    /* be interrupted by scheduler we have to use EDDS_ENTER */

    EDDS_ENTER(pDDB->hSysDev);

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */
                pKnotElement->ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pKnotElement->ProviderSendListUDP.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.pProvTbl[Next].SendLink.Prev    = Prev;
                pKnotElement->ProviderSendListUDP.BottomIdx = Next;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next    = Next;
            pKnotElement->ProviderSendListUDP.TopIdx    = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
        pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
    }

    if ( pCSRT->Prov.ProviderCycle.Udp.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Udp.ActProvIdx  = Next;
    }

    /*---------------------------------------------------------------------------*/
    /* Now we put the provider to the new knot list                              */
    /*---------------------------------------------------------------------------*/

    pProv->KnotTableIdx     =  KnotTableIdxNew;

    EDDS_RTProviderAddToSendList(pCSRT,&pKnotElementNew->ProviderSendListUDP,pProv,ProviderID);

    EDDS_EXIT(pDDB->hSysDev);

    /*---------------------------------------------------------------------------*/
    /* Actualize the limits                                                      */
    /*---------------------------------------------------------------------------*/

    pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
    pKnotElement->ProviderCnt--;

    pKnotElementNew->ProviderByteCnt += pProv->DataLen; /* sum of all datalen  */
    pKnotElementNew->ProviderCnt++;

    /* free old Knot Element if not used anymore */
    EDDS_RTKnotFree(pCSRT,KnotTableIdx);

}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderChangePhase                  +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_ADD_TYPE                    +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE                                +*/
/*+                                                                         +*/
/*+  ProviderID              : ProviderID                                   +*/
/*+  CyclePhase              : spezifies Phase within cycle. (1..x)         +*/
/*+  CyclePhaseSequence      : spezifies Sequence in Phase (not supported)  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_LIMIT_REACHED                                 +*/
/*+               EDD_STS_ERR_PHASE_MASKED                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to change phase of a provider.                    +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB - pProv set null -> init
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderChangePhase(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                              Status;
    EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE        pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    LSA_UINT32                              KnotTableIdx;
    LSA_UINT16                              OldCyclePhase;
    LSA_UINT32                              Interval;
    LSA_UINT32                              TraceIdx;

    EDDS_LOCAL_DDB_PTR_TYPE                 pDDB;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderChangePhase(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pProv           = LSA_NULL; /* to prevent compiler bug */
    KnotTableIdx    = 0;        /* to prevent compiler bug */
    Interval        = 0;        /* to prevent compiler bug */

    pRqbParam   = (EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE) pRQB->pParam;
    pCSRTComp   = pDDB->pSRT;

    /*---------------------------------------------------------------------------*/
    /* check parameter                                                           */
    /*---------------------------------------------------------------------------*/

    if ( pRqbParam->ProviderID >=pCSRTComp->Cfg.ProviderCnt )
    {
        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderChangePhase: Invalid Provider ID (%d)!",
                            pRqbParam->ProviderID);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        pProv = &pCSRTComp->Prov.pProvTbl[pRqbParam->ProviderID];

        if ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED == pProv->Type )
        {
            EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderChangePhase: Provider not in use (%d)!",
                                pRqbParam->ProviderID);

            Status = EDD_STS_ERR_SEQUENCE;
        }
        else
        {

            Interval    = pProv->CycleReductionRatio; /* 1,2,4,8...512 */

            /* Check if Phase is valid */
            if (( pRqbParam->CyclePhase == 0       )  ||
                ( pRqbParam->CyclePhase > Interval ))   /* invalid Phase  */
            {
                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderChangePhase: Invalid Phase (%d)!",
                                    pRqbParam->CyclePhase);

                Status = EDD_STS_ERR_PARAM;
            }
        }
    }

    /*---------------------------------------------------------------------------*/
    /* if parameters ok check if we stay within limits..                         */
    /*---------------------------------------------------------------------------*/

    if ( Status == EDD_STS_OK )
    {
        /* is this a new phase? if not nothing is to be done..*/

        EDDS_IS_VALID_PTR(pProv);

        if ( pProv->CyclePhase != pRqbParam->CyclePhase )
        {
            /* calculate Knot-Table index                           */
            /* (Interval-1) + (CyclePhase-1)                        */
            /* example for Interval = 4 and CyclePhase =2 : Idx = 4 */

            KnotTableIdx    = Interval+pRqbParam->CyclePhase-2;

            /* save CyclePhase of old KnotTable Entry               */
            OldCyclePhase               =    pProv->CyclePhase;

            pProv->CyclePhase           =    pRqbParam->CyclePhase;
            pProv->CyclePhaseSequence   =    pRqbParam->CyclePhaseSequence;

            if ( EDDS_RTKnotGet(pCSRTComp,KnotTableIdx) == EDD_STS_OK )
            {
                EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderChangePhase: PID: %d, NewPhase: %d, OldPhase: %d",
                                    pRqbParam->ProviderID,
                                    pProv->CyclePhase,
                                    OldCyclePhase);

                /* change the providers send list (phase) atomar */
                if ( pProv->Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
                {
                    EDDS_RTProviderChangeSendListXRT(pDDB,
                                                 pCSRTComp,
                                                 pRqbParam->ProviderID,
                                                 pProv,
                                                 KnotTableIdx);
                }
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                else
                {
                    EDDS_RTProviderChangeSendListUDP(pCSRTComp,
                                                 pRqbParam->ProviderID,
                                                 pProv,
                                                 KnotTableIdx);
                }
                #endif
            }
            else
            {
                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                    "EDDS_RTProviderChangePhase: EDDS_RTKnotGet() failed");
            }
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderChangePhase(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderRemoveXRT                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to device handle                                +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  ProviderID   : ID of provider to remove                                +*/
/*+  KnotTableIdx : Index in Knottable where provider is queued in sendlist +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a provider from Knot-Sendlist and set it to       +*/
/*+               "not" InUse.                                              +*/
/*+                                                                         +*/
/*+               Removement is syncronized with sending in progress in     +*/
/*+               higher prio state (task). We adjust the sender management +*/
/*+               if we remove a provider in the sendlist.                  +*/
/*+                                                                         +*/
/*+               Ids and Idx must be valid ! not checked.                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when calling this function, make sure not to use pCSRT as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderRemoveXRT(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
    LSA_UINT16                  ProviderID,
    LSA_UINT32                  KnotTableIdx)
{
    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;
    LSA_UINT32                              TraceIdx;

    LSA_UNUSED_ARG(pDDB);
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderRemoveXRT(pCSRT: 0x%X, PID: %d, KTIdx: %d)",
                           pCSRT,
                           ProviderID,
                           KnotTableIdx);

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    /* with priority based protection:                                          */
    /* Use short protection because of "ActProvIdx" access                      */
    /* in addition to chain elements, so we must asure consistense.             */
    EDDS_ENTER(pDDB->hSysDev);

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */
    if(0xFFFFFFFF != KnotTableIdx)
    {
        pKnot        = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
        EDDS_ASSERT(pKnot->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
        pKnotElement = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

        if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
            ( Next == EDDS_SRT_CHAIN_IDX_END ))
        {
            if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
            {
                if ( Next == EDDS_SRT_CHAIN_IDX_END )
                {
                    /* only one in list */
                    pKnotElement->ProviderSendListXRT.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                    pKnotElement->ProviderSendListXRT.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
                }
                else
                {
                    /* first one in list, but not only one */
                    pCSRT->Prov.pProvTbl[Next].SendLink.Prev    = Prev;
                    pKnotElement->ProviderSendListXRT.BottomIdx = Next;
                }
            }
            else
            {
                /* last one in list but not only one */
                pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
                pKnotElement->ProviderSendListXRT.TopIdx = Prev;
            }
        }
        else
        {
            /* in middle of list */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
            pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
        }

        pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
        pKnotElement->ProviderCnt--;

        EDDS_RTKnotFree(pCSRT,KnotTableIdx); /* remove KnotElement if unused yet */
    }

    /*---------------------------------------------------------------------------*/
    /* correct ActProvIdx for sender.. may only be neccessary                    */
    /* if sending NOT in cyclic blockmode, where sending is                      */
    /* done not in one part.                                                     */
    /*---------------------------------------------------------------------------*/

    if ( pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx  = Next;
    }

    /* we have to protect knot management and ActProvIdx access */
    /* so we could leave here */
    EDDS_EXIT(pDDB->hSysDev);

    /*---------------------------------------------------------------------------*/
    /* Now we can remove  the provider from the InUse-chain.                     */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */

                pCSRT->Prov.ProviderUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pCSRT->Prov.ProviderUsed.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.ProviderUsed.BottomIdx = Next;
                pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next = Next;
            pCSRT->Prov.ProviderUsed.TopIdx              = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next  = Next;
        pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* Put this provider to the end of the free-chain..                          */
    /*---------------------------------------------------------------------------*/

    pCSRT->Prov.pProvTbl[ProviderID].Type     = EDDS_CSRT_PROVIDER_TYPE_NOT_USED; /* this provider is no more in use */
    pCSRT->Prov.pProvTbl[ProviderID].CrNumber = EDD_CRNUMBER_NOT_USED;
    pCSRT->Prov.ProviderUsedCnt--;

    if ( pCSRT->Prov.ProviderFree.TopIdx == EDDS_SRT_CHAIN_IDX_END ) /* nothing in chain ? */
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.ProviderFree.BottomIdx   = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx      = ProviderID;
    }
    else
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = pCSRT->Prov.ProviderFree.TopIdx;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[pCSRT->Prov.ProviderFree.TopIdx].UsedFreeLink.Next = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx = ProviderID;
    }

    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderRemoveXRT()");

    LSA_UNUSED_ARG(TraceIdx);
}


#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderRemoveUDP                    +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  ProviderID   : ID of provider to remove                                +*/
/*+  KnotTableIdx : Index in Knottable where provider is queued in sendlist +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a provider from Knot-Sendlist and set it to       +*/
/*+               "not" InUse.                                              +*/
/*+                                                                         +*/
/*+               Removement is syncronized with sending in progress in     +*/
/*+               higher prio state (task). We adjust the sender management +*/
/*+               if we remove a provider in the sendlist.                  +*/
/*+                                                                         +*/
/*+               Ids and Idx must be valid ! not checked.                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderRemoveUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    LSA_UINT16                  ProviderID,
    LSA_UINT32                  KnotTableIdx)
{

    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;

    EDDS_CRT_TRACE_03(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderRemoveUDP(pCSRT: 0x%X, PID: %d, KTIdx: %d)",
                           pCSRT,
                           ProviderID,
                           KnotTableIdx);

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    if(0xFFFFFFFF != KnotTableIdx)
    {
        pKnot        = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
        EDDS_ASSERT(pKnot->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
        pKnotElement = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

        if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
            ( Next == EDDS_SRT_CHAIN_IDX_END ))
        {
            if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
            {
                if ( Next == EDDS_SRT_CHAIN_IDX_END )
                {
                    /* only one in list */
                    pKnotElement->ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                    pKnotElement->ProviderSendListUDP.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
                }
                else
                {
                    /* first one in list, but not only one */
                    pCSRT->Prov.pProvTbl[Next].SendLink.Prev    = Prev;
                    pKnotElement->ProviderSendListUDP.BottomIdx = Next;
                }
            }
            else
            {
                /* last one in list but not only one */
                pCSRT->Prov.pProvTbl[Prev].SendLink.Next    = Next;
                pKnotElement->ProviderSendListUDP.TopIdx    = Prev;
            }
        }
        else
        {
            /* in middle of list */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
            pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
        }

        pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
        pKnotElement->ProviderCnt--;

        EDDS_RTKnotFree(pCSRT,KnotTableIdx); /* remove KnotElement if unused yet */
    }

    /*---------------------------------------------------------------------------*/
    /* correct ActProvIdx for sender.. may only be neccessary                    */
    /* if sending NOT in cyclic blockmode, where sending is                      */
    /* done not in one part.                                                     */
    /*---------------------------------------------------------------------------*/

    if ( pCSRT->Prov.ProviderCycle.Udp.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Udp.ActProvIdx  = Next;
    }

    /*---------------------------------------------------------------------------*/
    /* Now we can remove  the provider from the InUse-chain.                     */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */

                pCSRT->Prov.ProviderUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pCSRT->Prov.ProviderUsed.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.ProviderUsed.BottomIdx = Next;
                pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next = Next;
            pCSRT->Prov.ProviderUsed.TopIdx              = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next  = Next;
        pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* Put this provider to the end of the free-chain..                          */
    /*---------------------------------------------------------------------------*/

    pCSRT->Prov.pProvTbl[ProviderID].Type = EDDS_CSRT_PROVIDER_TYPE_NOT_USED; /* this provider is no more in use */
    pCSRT->Prov.ProviderUsedCnt--;

    if ( pCSRT->Prov.ProviderFree.TopIdx == EDDS_SRT_CHAIN_IDX_END ) /* nothing in chain ? */
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.ProviderFree.BottomIdx   = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx      = ProviderID;
    }
    else
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = pCSRT->Prov.ProviderFree.TopIdx;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[pCSRT->Prov.ProviderFree.TopIdx].UsedFreeLink.Next = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx = ProviderID;
    }

    EDDS_CRT_TRACE_00(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderRemoveUDP()");
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderRemove                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE                 +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE                                      +*/
/*+                                                                         +*/
/*+  ProviderID : Valid ProviderID                                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to remove a  provider.                            +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+               A provider  has to be in use and no buffers               +*/
/*+               locked (EDD_STS_ERR_SEQUENCE if not)                      +*/
/*+                                                                         +*/
/*+               Providers may be active on remove. will be deactivated.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderRemove(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                              Status;
    EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE        pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    LSA_UINT16                              CycleReductionNr;
    LSA_UINT8                               Type;
    LSA_UINT32                              TraceIdx;

    EDDS_LOCAL_DDB_PTR_TYPE     			pDDB;

    pDDB      = pHDB->pDDB;
    TraceIdx  = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderRemove(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;
    pRqbParam = (EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE) pRQB->pParam;

    /* check for valid ID-range. ProviderID is idx within table */

    if ( pRqbParam->ProviderID >= pDDB->pSRT->Cfg.ProviderCnt )
    {
        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderRemove: Invalid Provider ID (%d)!",
                            pRqbParam->ProviderID);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /*-----------------------------------------------------------------------*/
        /* check if provider is in use, not active and no buffer locked.         */
        /* for ALL we first check if we can remove all provider                  */
        /*-----------------------------------------------------------------------*/

        pCSRTComp = pDDB->pSRT;

        /*-----------------------------------------------------------------------*/
        /* Only one provider                                                     */
        /*-----------------------------------------------------------------------*/
        {
            pProv = &pCSRTComp->Prov.pProvTbl[pRqbParam->ProviderID];

            /* provider has to be in use, not active and not locked to remove ! */

            if  ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED == pProv->Type )
            {
                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderRemove: Provider (%d) not in use!",
                                    pRqbParam->ProviderID);

                Status = EDD_STS_ERR_PARAM;
            }

            if ( Status == EDD_STS_OK )
            {
                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderRemove: PID: %d",
                                    pRqbParam->ProviderID);

                CycleReductionNr    = pProv->CycleReductionNr;
                Type                = pProv->Type;

                /* we deactivate the provider (may already be stopped) */
                EDDS_RTProviderStop(pCSRTComp,
                                    pProv);

                /* if a Autostop-Consumer is referenced, decrement Counter */
                if ( pProv->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
                {
                    EDDS_ASSERT(pProv->AutoStopConsumerID < pCSRTComp->Cons.ConsumerCnt);

                    /* decrement number of Providers using this consumer with Autostop */
                    pCSRTComp->Cons.pConsTbl[pProv->AutoStopConsumerID].ProviderAutoStopCnt--;
                }

                if ( Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
                {
                    EDDS_RTProviderRemoveXRT(pDDB,
                                         pCSRTComp,
                                         pRqbParam->ProviderID,
                                         pProv->KnotTableIdx);

                    /* actualize min/max ratio used by XRT */
                    if( 0xFFFF != CycleReductionNr )
                    {
                        pCSRTComp->Prov.ProvRatioInfo[CycleReductionNr].CntXRT--;
                    }

                    EDDS_RTProviderSetMinMaxRatioUsedXRT(pCSRTComp);
                }
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                else
                {
                    EDDS_RTProviderRemoveUDP(pCSRTComp,
                                         pRqbParam->ProviderID,
                                         pProv->KnotTableIdx);

                    /* actualize min/max ratio used by UDP */
                    if( 0xFFFF != CycleReductionNr )
                    {
                        pCSRTComp->Prov.ProvRatioInfo[CycleReductionNr].CntUDP--;
                    }

                    EDDS_RTProviderSetMinMaxRatioUsedUDP(pCSRTComp);
                }
                #endif
            }
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderRemove(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderStart                        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRT       +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR     pProv       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  pProv      : a provider in use                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Start a Provider                                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when calling this function, make sure not to use pCSRT or pProv as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderStart(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE     pCSRT,
    EDDS_CSRT_PROVIDER_TYPE_PTR     pProv)
{
    /* only if not already active */
    if ( ! pProv->Active)
    {
        pCSRT->Prov.ProviderActCnt++;
        /* sequence is important, because this can be interrupted */
        /* by sender.                                             */
        pProv->Active       = LSA_TRUE;
    }

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderStop                         +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRT       +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR     pProv       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  pProv      : a provider in use                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops a Provider                                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when calling this function, make sure not to use pCSRT or pProv as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderStop(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE     pCSRT,
    EDDS_CSRT_PROVIDER_TYPE_PTR     pProv)
{
    if ( pProv->Active)
    {
        pCSRT->Prov.ProviderActCnt--;
        /* sequence is important, because this can be interrupted */
        /* by sender.                                             */
        pProv->Active       = LSA_FALSE;
    }
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetDebugInfo                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Helper function.                                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets DebugInfo in EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 previously taken from RQB
//WARNING: be careful when using this function, make sure not to use pRqbParam as null ptr!
static void EDDS_RTProviderSetDebugInfo(
        EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE pRqbParam,
        LSA_RESULT Status,
        LSA_UINT16 line
    )
{
    pRqbParam->DebugInfo[0] = EDD_DEBUGINFO_CONTROL_IDENT_EDDS;
    pRqbParam->DebugInfo[1] = 0x0;
    Status = EDD_STS_ERR_PARAM;
    *(&pRqbParam->DebugInfo[2]) = (LSA_UINT8) (Status);
    *(&pRqbParam->DebugInfo[3]) = (LSA_UINT8) (Status >> 8);
    *(&pRqbParam->DebugInfo[4]) = (LSA_UINT8) (EDDS_MODULE_ID);
    *(&pRqbParam->DebugInfo[5]) = (LSA_UINT8) (EDDS_MODULE_ID >> 8); //lint !e572 !e778 defined based
    *(&pRqbParam->DebugInfo[6]) = (LSA_UINT8) (line);
    *(&pRqbParam->DebugInfo[7]) = (LSA_UINT8) (line >> 8);    
    pRqbParam->DebugInfo[8] = 0x0;
    pRqbParam->DebugInfo[9] = 0x0;
    pRqbParam->DebugInfo[10] = 0x0;
    pRqbParam->DebugInfo[11] = 0x0;
    pRqbParam->DebugInfo[12] = 0x0;
    pRqbParam->DebugInfo[13] = 0x0;
    pRqbParam->DebugInfo[14] = 0x0;
    pRqbParam->DebugInfo[15] = 0x0;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderControl                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE                +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE                                     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control a  provider.                           +*/
/*+                                                                         +*/
/*+               We activate or deactivate a provider and store the        +*/
/*+               DataValid-Bit for sending.                                +*/
/*+                                                                         +*/
/*+               If only one provider is spezified we test for the provider+*/
/*+               is InUse. If not we set EDD_STS_ERR_PARAM.                +*/
/*+                                                                         +*/
/*+               If neccessary we start the cycle-timer or stop it if no   +*/
/*+               more provider and consumer are active.                    +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB - pCR set null -> init
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderControl(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                                  Status;
    EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE    pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE            pCSRT;
    EDDS_CSRT_PROVIDER_TYPE_PTR                 pProv;
    LSA_UINT8                                   Mask;
    LSA_UINT8                                   Stat;
    LSA_UINT32                                  TraceIdx;

    EDDS_LOCAL_DDB_PTR_TYPE     				pDDB;

    pDDB      = pHDB->pDDB;
    TraceIdx  = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderControl(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRqbParam   = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE) pRQB->pParam;
    pCSRT       = pDDB->pSRT;

    if (( pRqbParam->ProviderID >= pCSRT->Cfg.ProviderCnt     ) ||
        ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED == pCSRT->Prov.pProvTbl[pRqbParam->ProviderID].Type ))
    {
        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderControl: Invalid Provider ID (%d)!",
                            pRqbParam->ProviderID);

        Status = EDD_STS_ERR_PARAM;
        EDDS_RTProviderSetDebugInfo(pRqbParam, Status, (LSA_UINT16)__LINE__);
    }
    else
    {
        pProv = &pCSRT->Prov.pProvTbl[pRqbParam->ProviderID];

        /* ------------------------------------------------------------------*/
        /* Activate/Deactivate check                                         */
        /* ------------------------------------------------------------------*/
        if (( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE ) &&
            ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_PASSIVATE ))
        {
            EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderControl: Invalid Mode: (0x%X))!",
                                pRqbParam->Mode);

            Status = EDD_STS_ERR_PARAM;

            EDDS_RTProviderSetDebugInfo(pRqbParam, Status, (LSA_UINT16)__LINE__);
        }
        else if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE )
        {
            if(!pDDB->pGlob->IO_Configured)
            {
                EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        ">>> Request: EDD_SRV_SRT_PROVIDER_CONTROLL ACTIVATE invalid with IO_Configured=NO");

                Status = EDD_STS_ERR_SERVICE;
            }
            else
            {

                LSA_BOOL                       doInsert           = LSA_FALSE;
                EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr                = LSA_NULL;
                LSA_UINT16                     CalculatedCrNumber = EDD_CRNUMBER_NOT_USED;

                if( EDD_CRNUMBER_NOT_USED  == pRqbParam->IOParams.CRNumber ||
                    (    (pCSRT->Cfg.ProviderCnt <  pRqbParam->IOParams.CRNumber)
                      && (EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber) ) )
                {
                    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: Invalid cr number!");

                    Status = EDD_STS_ERR_PARAM;
                }

                if( EDD_STS_OK == Status )
                {
                    LSA_UINT16 CrTableIterator;

                    if( EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber )
                    {
                        // check that each crnumber is used max one time
                        for( CrTableIterator = 0 ; CrTableIterator < pCSRT->Cfg.ProviderCnt ; CrTableIterator++ )
                        {
                            if(    (pRqbParam->IOParams.CRNumber == pCSRT->Prov.pProvTbl[CrTableIterator].CrNumber)
                                && ( EDD_CRNUMBER_NOT_USED !=  pCSRT->Prov.pProvTbl[CrTableIterator].CrNumber)
                                && ( pRqbParam->ProviderID != CrTableIterator))
                            {
                                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: crnumber is already in use!");

                                Status = EDD_STS_ERR_PARAM;
                                break;
                            }

                            // check that crnumber cant not be changed
                            if(   (0 != pCSRT->Prov.pProvTbl[CrTableIterator].CrNumber)
                               && (pRqbParam->ProviderID == CrTableIterator)
                               && (pCSRT->Prov.pProvTbl[CrTableIterator].CrNumber != pRqbParam->IOParams.CRNumber))
                            {
                                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: crnumber can only set one time!");

                                Status = EDD_STS_ERR_PARAM;
                                break;
                            }
                        }

                        if(EDD_STS_OK == Status)
                        {

                            CalculatedCrNumber = pRqbParam->IOParams.CRNumber;
                        }
                    }
                    else
                    {

                        if(EDD_CRNUMBER_NOT_USED == pCSRT->Prov.pProvTbl[pRqbParam->ProviderID].CrNumber)
                        {
                            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: crnumber was not set at _RT_ProviderADD() and is here EDD_CRNUMBER_UNDEFINED!");

                            Status = EDD_STS_ERR_PARAM;
                        }
                        else
                        {
                            //CrNumber was already set in EDDS_RTProviderADD()
                            CalculatedCrNumber = pCSRT->Prov.pProvTbl[pRqbParam->ProviderID].CrNumber;
                        }
                    }
                }

                if( EDD_CRNUMBER_NOT_USED == CalculatedCrNumber)
                {
                    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: CalculatedCrNumber was not calculated correctly! pRqbParam->ProviderID: 0x%X",
                                      pRqbParam->ProviderID);

                    Status = EDD_STS_ERR_PARAM;
                }

                if( Status == EDD_STS_OK )
                {
                    LSA_UINT16 DataLen;

                    pCr = &pCSRT->Prov.pProvCrTbl[CalculatedCrNumber - 1];   // 1 .. ProviderCnt

                    DataLen = LE_TO_H_S(pCr->DataLen);   // ensure correct endianess!

                    if(  DataLen                        != pProv->DataLen ||   // data length of io data must match
                         pCr->Type                      != pProv->Type    ||   // matching provider type --> from allocate at IOBuffer
                         EDDS_CSRT_PROVIDER_CR_UNUSED   == pCr->CrUsed )       // Cr must be allocated previously
                    {
                        EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: Invalid input parameter!");

                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                
                if(EDD_STS_OK == Status)
                {
                    pCr = &pCSRT->Prov.pProvCrTbl[CalculatedCrNumber - 1];
                    
                    switch (pCr->IsSystemRedundant)
                    {
                        case EDD_PROVADD_TYPE_DEFAULT:
                             if( pProv->Properties & EDD_CSRT_PROV_PROP_SYSRED )
                             {
                                Status = EDD_STS_ERR_PARAM;
                             }
                             break;
                        case EDD_PROVADD_TYPE_SYSRED:
                             if(!( pProv->Properties & EDD_CSRT_PROV_PROP_SYSRED ))
                             {
                                 Status = EDD_STS_ERR_PARAM;
                             }
                             break;

                        default:
                             Status = EDD_STS_ERR_PARAM;  
                             break;
                    }
                    if( Status == EDD_STS_ERR_PARAM )
                    {
                        //check parameter IsSystemRedundant of EDDS_IOBuffer_ProviderAlloc()
                        //and Properties.EDD_CSRT_PROV_PROP_SYSRED in the PROVIDER_ADD service
                         EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                               "EDDS_RTProviderControl: for pProv->CrNumber: 0x%X Provider System redundancy property is"
                                               " not equal IsSystemRedundant of EDDS_IOBuffer_ProviderAlloc()",
                                               pProv->CrNumber);
                          
                    }
                  
                }

                if( Status == EDD_STS_OK )
                {
                    LSA_UINT16 User;
                    LSA_UINT16 XChange;
                    LSA_UINT16 EDDS;

                    // enter CrNumber
                    pProv->CrNumber = CalculatedCrNumber;

                    EDDS_IS_VALID_PTR(pCr);

                    User    = *((LSA_UINT8*)&pCr->User)    & ~EDDS_IOBUFFER_MASK_NEW_FLAG;
                    XChange = *((LSA_UINT8*)&pCr->XChange) & ~EDDS_IOBUFFER_MASK_NEW_FLAG;
                    EDDS    = *((LSA_UINT8*)&pCr->EDDS)    & ~EDDS_IOBUFFER_MASK_NEW_FLAG;

                    if( EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == pProv->CyclePhaseSequence ) {
                        pProv->CyclePhaseSequence   =    pRqbParam->CyclePhaseSequence;
                        doInsert = LSA_TRUE;
                    }

                    /* check CycleReductionRatio */
                    if( (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pProv->CycleReductionRatio) )
                    {
                        LSA_BOOL IsUdp = (pProv->Type & EDDS_CSRT_PROVIDER_TYPE_XRT) != EDDS_CSRT_PROVIDER_TYPE_XRT;

                        /* CycleReductionRatio still undefined on activation */
                        if (0 == EDDS_RTGetAndCheckReductionFactor(pRqbParam->CycleReductionRatio,IsUdp))
                        {
                            Status = EDD_STS_ERR_PARAM;
                        }
                        else
                        {
                            pProv->CycleReductionRatio = pRqbParam->CycleReductionRatio;
                            doInsert = LSA_TRUE;
                        }
                    }

                    /* check CyclePhase */
                    if( (EDD_STS_OK == Status) &&  (EDD_CYCLE_PHASE_UNDEFINED == pProv->CyclePhase))
                    {
                        LSA_UINT16 CyclePhase;
                        CyclePhase = pRqbParam->CyclePhase;

                        /* CyclePhase still undefined on activation or out of boundary*/
                        if ( (EDD_CYCLE_PHASE_UNDEFINED == CyclePhase) ||
                             (CyclePhase > pProv->CycleReductionRatio) )
                        {
                            Status = EDD_STS_ERR_PARAM;
                        }
                        else
                        {
                            pProv->CyclePhase = CyclePhase;
                            doInsert = LSA_TRUE;
                        }
                    }

                    // For FrameID: it is set every time from passivate to activate state
                    if(EDD_STS_OK == Status)
                    {
                        if(EDD_FRAME_ID_UNDEFINED == pRqbParam->FrameID)
                        {
                            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RTProviderControl: FrameID still undefined on activation!");
                            Status = EDD_STS_ERR_PARAM;
                        }
                        else
                        {
                            LSA_UINT16 RtClassProp = pProv->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK;

                            /* ---------------------------------------------------- */
                            /* check FrameID                                        */
                            /* --> FrameID must match RTClass                       */
                            /* NOTE: RT class property already checked on ADD       */
                            /* ---------------------------------------------------- */
                            if(EDD_CSRT_CONS_PROP_RTCLASS_1 == RtClassProp)
                            {
                                if ((EDD_SRT_FRAMEID_CSRT_START2 > pRqbParam->FrameID) ||
                                    (EDD_SRT_FRAMEID_CSRT_STOP2  < pRqbParam->FrameID))
                                {
                                    Status = EDD_STS_ERR_PARAM;
                                }
                            }
                            else if (EDD_CSRT_CONS_PROP_RTCLASS_2 == RtClassProp)
                            {
                                if ((EDD_SRT_FRAMEID_CSRT_START1 > pRqbParam->FrameID) ||
                                    (EDD_SRT_FRAMEID_CSRT_STOP1  < pRqbParam->FrameID))
                                {
                                    Status = EDD_STS_ERR_PARAM;
                                }
                            } /* check FrameID */

                            if(EDD_STS_OK == Status)
                            {
                                if(pProv->Active)
                                {
                                    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_WARN,
                                                "EDDS_RTProviderControl: FrameID still not updated! Provider already active!");
                                }
                                else
                                {
                                    pProv->FrameID = pRqbParam->FrameID;
                                }
                            }
                        }
                    }

                    if(EDD_STS_OK == Status)
                    {
                        /*------------------------------------------------------------- */
                        /* Because it is possible that the sendbuffer is still in send  */
                        /* (from previous use of this provider) we can not fill the     */
                        /* send buffer here! So we must fill the send framebuffer       */
                        /* later when we first send this frame. So we set the flag      */
                        /* ProviderDoSetupBuffer for the sender to do it later.         */
                        /*------------------------------------------------------------- */

                        pProv->DstMAC	= pRqbParam->DstMAC;
                        #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                        pProv->DstIP	= pRqbParam->DstIP;
                        ///note remove pProv->SrcIP ?
                        #endif

                        /* setup Sendbuffer Src MAC */

                        EDDS_RTProviderSetupFrameSrcMAC( pCr->pBuffer[EDDS],
                                                         pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr);

                        /* the rest will be setup later ! */

                        pProv->DoSetupBuffer = LSA_TRUE;

                        /* setup Xchangebuffer */

                        EDDS_RTProviderSetupFrameSrcMAC( pCr->pBuffer[XChange],
                                                         pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr);

            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                        if ( EDDS_CSRT_PROVIDER_TYPE_UDP == pProv->Type )
                        {
                            EDDS_RTProviderSetupFrameUDP(pCSRT,
                                                         pCr->pBuffer[XChange],
                                                         pProv);
                        }
                        else
            #endif
                        {
                            EDDS_RTProviderSetupFrameXRT(pCr->pBuffer[XChange],
                                                         pProv->DataLen,
                                                         pRqbParam->DstMAC.MacAdr,
                                                         pProv->FrameID,
                                                         pProv->VLANID);
                        }

                        /* setup Userbuffer */

                        EDDS_RTProviderSetupFrameSrcMAC( pCr->pBuffer[User],
                                                         pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr);

            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                        if ( EDDS_CSRT_PROVIDER_TYPE_UDP == pProv->Type )
                        {
                            EDDS_RTProviderSetupFrameUDP(pCSRT,
                                                         pCr->pBuffer[User],
                                                         pProv);
                        }
                        else
            #endif
                        {
                            EDDS_RTProviderSetupFrameXRT(pCr->pBuffer[User],
                                                         pProv->DataLen,
                                                         pProv->DstMAC.MacAdr,
                                                         pProv->FrameID,
                                                         pProv->VLANID);
                        }




                        if(doInsert) {
                            if ( EDD_STS_OK != (Status = EDDS_RTProviderInsert(pCSRT,pRqbParam->ProviderID)))
                            {
                                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RTProviderControl: EDDS_RTProviderInsert with ProviderID 0x%X failed!",
                                                    pRqbParam->ProviderID);
                                EDDS_RTProviderSetDebugInfo(pRqbParam, Status, (LSA_UINT16)__LINE__);
                            }
                        }
                    }
                }
            }
        }
        /* ------------------------------------------------------------------*/
        /* Autostop parameter check                                          */
        /* ------------------------------------------------------------------*/
        if ( Status == EDD_STS_OK )
        if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_AUTOSTOP )
        {
            if ( pRqbParam->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
            {
                if ( pRqbParam->AutoStopConsumerID >= pCSRT->Cons.ConsumerCnt )
                {
                    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderControl: Invalid AutoStopConsumerID: (0x%X))!",
                                        pRqbParam->AutoStopConsumerID);

                    Status = EDD_STS_ERR_PARAM;

                    EDDS_RTProviderSetDebugInfo(pRqbParam, Status, (LSA_UINT16)__LINE__);
                }
                else
                {
                    if ( ! pCSRT->Cons.pConsTbl[pRqbParam->AutoStopConsumerID].Type )
                    {
                        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                            "EDDS_RTProviderControl: Consumer not in use for AutoStopConsumerID: (0x%X))!",
                                            pRqbParam->AutoStopConsumerID);

                        Status = EDD_STS_ERR_PARAM;

                        EDDS_RTProviderSetDebugInfo(pRqbParam, Status, (LSA_UINT16)__LINE__);
                    }
                }
            }
        }
        /* ------------------------------------------------------------------*/
        /* Control Mode Status parameter check                               */
        /* ------------------------------------------------------------------*/
        if( (EDD_STS_OK == Status) && (pRqbParam->Mode & EDD_PROV_CONTROL_MODE_STATUS) )
        {
            /* Mode(MODE_STATUS) is only for BIT_STATION_FAILURE permitted */
            if ( (pRqbParam->Mask & ~EDD_CSRT_DSTAT_LOCAL_MASK_ALL) ||
                 (pRqbParam->Status & ~EDD_CSRT_DSTAT_LOCAL_MASK_ALL) )
            {
                EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl(): MODE_STATUS - Mask(0x%X) or Status(0x%X) are containing forbidden bits!",
                    pRqbParam->Mask, pRqbParam->Status);
                Status = EDD_STS_ERR_PARAM;
            }
        }       

        /* ------------------------------------------------------------------*/
        /* All parameters ok.                                                */
        /* ------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {

            EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                        "EDDS_RTProviderControl:  ProvID: %d, Mode: 0x%X",
                        pRqbParam->ProviderID,
                        pRqbParam->Mode);

            /* if we should stop, do it first */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_PASSIVATE )
            {
                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderControl: Deactivate Provider %d",
                                    pRqbParam-> ProviderID);

                EDDS_RTProviderStop(pCSRT,pProv);
            }

            /* Set AutoStop */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_AUTOSTOP )
            {
                /* currently Autostop used ? */
                if ( pProv->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
                {

                    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                        "EDDS_RTProviderControl: Autostop disabled ProvId: %d",
                                        pRqbParam-> ProviderID);


                    /* decrement number of Providers using this consumer with Autostop */
                    pCSRT->Cons.pConsTbl[pProv->AutoStopConsumerID].ProviderAutoStopCnt--;
                }

                /* new Autostop set ? */
                if ( pRqbParam->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS)
                {
                    EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                        "EDDS_RTProviderControl: Autostop Set ProvId: %d, ConsID: %d",
                                        pRqbParam-> ProviderID,
                                        pRqbParam-> AutoStopConsumerID);

                    /* increment number of Providers using this consumer with Autostop */
                    pCSRT->Cons.pConsTbl[pRqbParam->AutoStopConsumerID].ProviderAutoStopCnt++;
                }

                /* save new Autostop setting */
                pProv->AutoStopConsumerID = pRqbParam->AutoStopConsumerID;
            }

            /* Set DataStatus */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_STATUS )
            {

                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                     "EDDS_RTProviderControl: Set DataStatus for ProvId: %d",
                                     pRqbParam-> ProviderID);

                Mask = pRqbParam->Mask;
                Stat = pRqbParam->Status;

                Mask &= EDD_CSRT_DSTAT_LOCAL_MASK_ALL;
                Stat &= EDD_CSRT_DSTAT_LOCAL_MASK_ALL;

                pProv->DataStatus = ( pProv->DataStatus & ~Mask) | (Stat & Mask);

            }

            /* if we should start, do it after all is setup */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE )
            {
                EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderControl: Activate Provider %d.",
                                    pRqbParam->ProviderID);

                EDDS_RTProviderStart(pCSRT,pProv);
            }

        }
    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderControl(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetState                     +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE              +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE                                   +*/
/*+                                                                         +*/
/*+  GroupID    : Providergroup to set status for                           +*/
/*+  Status     : Status with bitvalues                                     +*/
/*+               EDD_CSRT_DSTAT_BIT_STOP_RUN                               +*/
/*+               EDD_CSRT_DSTAT_BIT_STATION_FAILURE                        +*/
/*+  Mask       : Mask for Status (spezifies bits to change)                +*/
/*+               EDD_CSRT_DSTAT_BIT_STOP_RUN                               +*/
/*+               EDD_CSRT_DSTAT_BIT_STATION_FAILURE                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to set gobal provider state .                     +*/
/*+                                                                         +*/
/*+               This will set Bits of DataStatus for all providers        +*/
/*+               within the same providergroup.                            +*/
/*+                                                                         +*/
/*+               NewState = (AktState & NOT Mask) OR ( Status & Mask)      +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!s
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_RESULT                                  Status;
    EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE  pRqbParam;
    LSA_UINT8                                   n;
    LSA_UINT8                                   Stat;
    LSA_UINT8                                   Mask;
    LSA_UINT32                                  TraceIdx;

    EDDS_LOCAL_DDB_PTR_TYPE     				pDDB;

    pDDB      = pHDB->pDDB;
    TraceIdx  = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderSetState(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRqbParam = (EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE) pRQB->pParam;

    if ( pRqbParam->GroupID >=  EDD_CFG_CSRT_MAX_PROVIDER_GROUP )
    {

        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderSetState: Invalid GroupID (%d)!",
                            pRqbParam->GroupID);

        Status = EDD_STS_ERR_PARAM;
    }
    else if ( (pRqbParam->Mask   & (~EDD_CSRT_DSTAT_GLOBAL_MASK_ALL)) ||
              (pRqbParam->Status & (~EDD_CSRT_DSTAT_GLOBAL_MASK_ALL)) )
    {
        EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderSetState: Invalid bit(s) set either in Mask (0x%X) or Status (0x%X)!",
                            pRqbParam->Mask, pRqbParam->Status);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /* mask to valid bits */

        Mask = pRqbParam->Mask   & EDD_CSRT_DSTAT_GLOBAL_MASK_ALL;
        Stat = pRqbParam->Status & EDD_CSRT_DSTAT_GLOBAL_MASK_ALL;

        n = pDDB->pSRT->Prov.pProvGroupTbl[pRqbParam->GroupID].ProviderDataStatus;

        /* this will not conflict with current sender, because one access only */

        pDDB->pSRT->Prov.pProvGroupTbl[pRqbParam->GroupID].ProviderDataStatus = ( n & ~Mask) | (Stat & Mask);

        EDDS_CRT_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                            "EDDS_RTProviderSetState: ProviderGroup %d. DataStatus: 0x%X",
                            pRqbParam->GroupID,
                            pDDB->pSRT->Prov.pProvGroupTbl[pRqbParam->GroupID].ProviderDataStatus);

    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderSetState(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderGetCycleCounter              +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  pCycleCounter: Pointer to address for CycleCounter                     +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:  Gets actual provider CycleCounter of cyclic SRT.         +*/
/*+                                                                         +*/
/*+                The CycleCounter is based on 31.25us and wraps around    +*/
/*+                if 32Bit is reached. The function can be called anytime  +*/
/*+                except for startup/shutdown of srt-component             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderGetCycleCounter(
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
    EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter)
{
    LSA_RESULT Status;

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetCycleCounter(pCSRT: 0x%X, pCycleCounter: 0x%X)",
                           pCSRT,
                           pCycleCounter);

    Status = EDD_STS_OK;
    if ( ! LSA_HOST_PTR_ARE_EQUAL(pCycleCounter,LSA_NULL))
    {
        pCycleCounter->Low  = ((pCSRT->Prov.ProviderCycle.CycleCounter) << pCSRT->CycleStampAdjustShift);
        pCycleCounter->High = 0;
    }
    else
    {
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetCycleCounter(), Status: 0x%X",
                           Status);

    return(Status);

}


/*****************************************************************************/
/*  end of file SRT_PROV.C                                                   */
/*****************************************************************************/



