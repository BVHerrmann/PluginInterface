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
/*  F i l e               &F: edds_srt_cons.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDS. Consumer          */
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

#define LTRC_ACT_MODUL_ID  146
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SRT_CONS */

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
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerRxIndicateCyclicXRT          +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             LSA_UINT32                     Length       +*/
/*+                             LSA_UINT16                     FrameID      +*/
/*+                             LSA_BOOL                       VLANTagPresent*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB           : Pointer to DeviceDescriptionBlock                     +*/
/*+  pBuffer        : Address for pointer to rx-buffer (input and output)   +*/
/*+  Length         : Length of received data.(input)                       +*/
/*+  FrameID        : valid cyclic FrameID                                  +*/
/*+  VLANTagPresent : LSA_TRUE if VLAN-Tag is present in frame              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called after a cyclic SRT-Frame was recv.+*/
/*+               The function is called within int-handler and has to      +*/
/*+               free the buffer and provide a new one.                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked before this function is called - pCSRTComp is taken from pDDB - pCSRTTable set LSA_NULL and initialized
//WARNING: be careful when using this function, make sure not to use pBuffer or pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRxIndicateCyclicXRT(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBuffer,
LSA_UINT32 Length,
LSA_UINT16 FrameID,
LSA_BOOL VLANTagPresent)

{
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_CSRT_CONSUMER_TYPE_PTR pCSRTTable;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pCSRTFrameTable;
    LSA_UINT32 Len;
    LSA_UINT8 DataStatus;
    EDD_UPPER_MEM_U8_PTR_TYPE pEpilog;
    LSA_UINT16 CycleCounter;
    LSA_UINT16 Delta;
    LSA_BOOL Good;
    LSA_UINT16 ConsumerId;
    LSA_UINT32 TraceIdx;

#define DATA_STATUS_GOOD_MASK     (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_GOOD_VALUE    (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_STATE_MASK    (EDD_CSRT_DSTAT_BIT_STATE)
#define DATA_STATUS_STATE_VALUE   (EDD_CSRT_DSTAT_BIT_STATE)

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "IN :EDDS_RTConsumerRxIndicateCyclicXRT(pDDB: 0x%X,*pBuffer: 0x%X, Len: %d, FId: 0x%X)",
    pDDB,
    *pBuffer,
    Length,
    FrameID);

    pCSRTComp = pDDB->pSRT;
    pCSRTFrameTable = LSA_NULL;
    pCSRTTable = LSA_NULL;

    /* check FrameID for valid range. FrameID is within valid CSRT range on entry. */

    if (( FrameID >= pCSRTComp->Cons.ConsumerFrameIDBase1) &&
    ( FrameID <= pCSRTComp->Cons.ConsumerFrameIDTop1 ))
    {
        ConsumerId = FrameID-pCSRTComp->Cons.ConsumerFrameIDBase1;
        pCSRTFrameTable = &pCSRTComp->Cons.pConsFrameTbl[ConsumerId];
        pCSRTTable = &pCSRTComp->Cons.pConsTbl[ConsumerId];
    }
    else
    {
        if (( FrameID >= pCSRTComp->Cons.ConsumerFrameIDBase2) &&
        ( FrameID <= pCSRTComp->Cons.ConsumerFrameIDTop2 ))
        {
            ConsumerId = FrameID-pCSRTComp->Cons.ConsumerFrameIDBase2;
            pCSRTFrameTable = &pCSRTComp->Cons.pConsFrameTbl2[ConsumerId];
            pCSRTTable = &pCSRTComp->Cons.pConsTbl2[ConsumerId];
        }
    }

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pCSRTFrameTable,LSA_NULL))
    {
        /* Framelength to check for. if VLAN-Tag not present adjust length.         */

        if (! VLANTagPresent) Len = pCSRTFrameTable->FrameLength - EDD_VLAN_TAG_SIZE;
        else Len = pCSRTFrameTable->FrameLength;

        /* we use optimized access to the MAC-address! The start of the MAC-address */
        /* has to be DWORD-aligned. So the Rx-Buffer has to be DWORD-aligned!       */

        if (pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_0) {
            if(( (pCSRTFrameTable->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_XRT ) &&
            ( Length == Len ) &&
            (
                    ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[0] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6/*+0*/] )
                    && ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[1] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+1] )
                    && ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[2] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+2] )
                    && ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[3] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+3] )
                    && ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[4] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+4] )
                    && ( pCSRTFrameTable->StampAddr.xrt.SrcMAC.MacAdr[5] == ((EDD_UPPER_MEM_U8_PTR_TYPE)*pBuffer)[6+5] )
            )
            )
            {

                /* check CycleCounter, DataStatus + Transfer-State                 */
                /* cannot be accessed together because of alignment !              */
                /* (maybe not neccessary for system without alignment requirements */
                /*  but we dont do different handling now and dont use MACROs to   */
                /*  be sure whats going on here..)                                 */

                /* set pEpilog to last Byte of frame (TransferStatus)              */
                pEpilog = &((EDD_UPPER_MEM_U8_PTR_TYPE)(*pBuffer))[Length-1];

                /* check TransferStatus and move pEpilog to DataStatus */
                if ( *pEpilog-- == 0 )
                {
                    LSA_BOOL IsBackup /*= LSA_FALSE*/;
                    LSA_BOOL IsDataValid /*= LSA_FALSE*/;

                    /* Read DataStatus */
                    DataStatus = *pEpilog--;

                    IsBackup = !( (DataStatus & DATA_STATUS_STATE_MASK) == DATA_STATUS_STATE_VALUE );
                    IsDataValid = ( (DataStatus & DATA_STATUS_GOOD_MASK) == DATA_STATUS_GOOD_VALUE );

                    if(   IsDataValid ||
                        (!IsDataValid && IsBackup) )  //lint !e774 JB 05/12/2014 readability

                    {
                        Good = LSA_TRUE;

                        /* pEpilog now points to last byte of CycleCounter */
                        /* read CycleCounter (Big-Endian!)                 */
                        CycleCounter = (LSA_UINT16) ((((LSA_UINT16)*(pEpilog-1)) << 8) + *pEpilog);

                        /* check CycleCounter only if this is not the */
                        /* first frame after activate.                */

                        if ( pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_3 )
                        {
                            /* The CycleCounter is older as the current one, if the value   */
                            /* is within 4096 digits behind the current one. because of the */
                            /* wraparound we have to do a little math..                     */
                            /*                                                              */
                            /*                                                              */
                            /* 0                                 0xFFFF                     */
                            /* |--------xxxxxxO...........N--------|                        */
                            /*          |older|                                             */
                            /*           4096                                               */
                            /*                                                              */
                            /* |..N-----xxxxxxO....................|                        */
                            /*          |older|                                             */
                            /*           4096                                               */
                            /* |xxxxO..............N-------------xx|                        */
                            /*  lder|                            |o                         */
                            /*                                                              */
                            /*  N: new Counter                                              */
                            /*  O: old Counter                                              */
                            /*                                                              */
                            /*  Delta = (LSA_UINT16) (N-O)                                  */
                            /*                                                              */
                            /*  Note: a CycleCounter equal (Delta == 0) is handled as old   */
                            Delta = CycleCounter - pCSRTFrameTable->CycleCounter;
                            if ( (Delta == 0) || (Delta > 0xF000) ) /* between -4095..0 */
                            {
                                Good = LSA_FALSE;
                                pCSRTComp->Stats.ConsRxCycleCounterOld++;
                            }
                            if(EDDS_SRT_CONS_STATE_ON_3 != pCSRTFrameTable->State)
                            {
                                pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame after SetToUnknown was received */
                            }
                        }
                        else
                        {
                            pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame received */
                        }

                        if ( Good )  // old/new frame ok
                        {
                            pCSRTFrameTable->CycleCounter = CycleCounter; /* save actual CycleCounter*/
                            pCSRTFrameTable->DataStatus = DataStatus; /* save actual DataStatus  */
                            pCSRTFrameTable->StampAddr.xrt.CycleCntStamp = (LSA_UINT16) pCSRTComp->Prov.ProviderCycle.CycleCounter;

                            if( IsDataValid )
                            {
                                LSA_UINT16 EDDS;
                                LSA_UINT8* pValue;
                                EDDS_EXCHANGE_TYPE Temp;
                                EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
                                EDDS_IS_VALID_PTR(pCSRTTable);
                                pCr = &pCSRTComp->Cons.pConsCrTbl[pCSRTTable->CrNumber - 1]; // 1 .. ConsumerCnt

                                // NOTE:
                                // Information like index and new flag is always stored in one byte in
                                // the EXCHANGE variables at its lowest addresses. This applies to EDDS and
                                // IOBuffer when accessing these variables. Access to this byte by byte
                                // pointer does not need any consideration of endianess.
                                // example:
                                // Value stored in memory for Xchange with reference to buffer 1 and
                                // new flag is 0x8100. Index and new flag can be retrieved by accessing
                                // Xchange with its address casted to a byte pointer independently from
                                // host endianess.
                                //
                                // index = (*((LSA_UINT8*)&Xchange)) & ~MASK

                                // set reference in EDDS to received buffer
                                EDDS = *((LSA_UINT8*)&pCr->EDDS);
                                pCr->pBuffer[EDDS] = *pBuffer;

                                Temp = pCr->EDDS;

                                // set new flag in Temp
                                pValue = ((LSA_UINT8*)&Temp);
                                *pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

                                Temp = EDDS_IOBUFFER_EXCHANGE( &pCr->XChange, Temp );

                                // reset new flag in User
                                pValue = ((LSA_UINT8*)&Temp);
                                *pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;

                                pCr->EDDS = Temp;

                                // return buffer referenced in EDDS as new receive resource
                                // NOTE: Buffer that is currently used in EDDS as receive resource must be
                                //       set to NULL!
                                EDDS = *((LSA_UINT8*)&pCr->EDDS);
                                *pBuffer = pCr->pBuffer[EDDS];
                                pCr->pBuffer[EDDS] = LSA_NULL;
                            }		// exchange buffer --> only when data valid

                        }					      // old/new frame check
                    }					      // data valid
                }					      // transfer status != 0
            }					      // length and mac check
            else
            {
                pCSRTComp->Stats.ConsRxWrongXRTLenOrMAC++;
            }
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "OUT:EDDS_RTConsumerRxIndicateCyclicXRT(*pBuffer: 0x%X)",
    *pBuffer);

    LSA_UNUSED_ARG(TraceIdx);
}


#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerRxIndicateCyclicUDP          +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...     pUDPData     +*/
/*+                             EDDS_IP_HEADER_TYPE           *pHeader      +*/
/*+                             LSA_UINT32                     UDPLen       +*/
/*+                             LSA_UINT16                     FrameID      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB           : Pointer to DeviceDescriptionBlock                     +*/
/*+  pBuffer        : Address for pointer to rx-buffer (input and output)   +*/
/*+  pUDPData       : Pointer to UDP-Data start within pBuffer              +*/
/*+  pHeader        : Pointer to IPHeader (32-Bit aligned! not in frame!!)  +*/
/*+  UDPLen         : Length of UDP-User Data                               +*/
/*+  FrameID        : valid cyclic FrameID                                  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE         No free rx-resources         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called after a cyclic xRT over UDP frame +*/
/*+               was received. IP/UDP-Header already checked.              +*/
/*+               The function is called within int-handler and has to      +*/
/*+               free the buffer and provide a new one.                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRxIndicateCyclicUDP(
EDDS_LOCAL_DDB_PTR_TYPE pDDB,
EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBuffer,
EDD_UPPER_MEM_PTR_TYPE pUDPData,
EDDS_IP_HEADER_TYPE * pHeader,
LSA_UINT32 UDPLen,
LSA_UINT16 FrameID)
{
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp;
    EDDS_CSRT_CONSUMER_TYPE_PTR pCSRTTable;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pCSRTFrameTable;
    EDD_UPPER_MEM_PTR_TYPE pTemp;
    LSA_UINT8 DataStatus;
    EDD_UPPER_MEM_U8_PTR_TYPE pEpilog;
    LSA_UINT16 CycleCounter;
    LSA_UINT16 Delta;
    LSA_BOOL Good;
    LSA_UINT32 TraceIdx;

#define DATA_STATUS_GOOD_MASK     (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_GOOD_VALUE    (EDD_CSRT_DSTAT_BIT_DATA_VALID)
#define DATA_STATUS_STATE_MASK    (EDD_CSRT_DSTAT_BIT_STATE)
#define DATA_STATUS_STATE_VALUE   (EDD_CSRT_DSTAT_BIT_STATE)

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "IN :EDDS_RTConsumerRxIndicateCyclicUDP(pDDB: 0x%X,*pBuffer: 0x%X, Len: %d, FId: 0x%X)",
    pDDB,
    *pBuffer,
    UDPLen,
    FrameID);

    pCSRTComp = &pDDB->pSRT->CSRT;
    pCSRTFrameTable = LSA_NULL;
    pCSRTTable = LSA_NULL;

    /* check FrameID for valid range   */
    /* UDP only possible within array2 */

    if (( FrameID >= pCSRTComp->Cons.ConsumerFrameIDBase2) &&
    ( FrameID <= pCSRTComp->Cons.ConsumerFrameIDTop2 ))
    {

        pCSRTFrameTable = &pCSRTComp->Cons.pConsFrameTbl2[FrameID-pCSRTComp->Cons.ConsumerFrameIDBase2];
        pCSRTTable = &pCSRTComp->Cons.pConsTbl2[FrameID-pCSRTComp->Cons.ConsumerFrameIDBase2];

        if (pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_0 ) {
            if(( (pCSRTFrameTable->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_UDP ) &&
            ( pCSRTFrameTable->FrameLength == UDPLen ) &&
            ( pCSRTFrameTable->StampAddr.udp.SrcIP.dw == pHeader->SrcIP.dw ))
            {

                /* check CycleCounter, DataStatus + Transfer-State                 */
                /* cannot be accessed together because of alignment !              */
                /* (maybe not neccessary for system without alignment requirements */
                /*  but we dont do different handling now and dont use MACROs to   */
                /*  be sure whats going on here..)                                 */

                /* set pEpilog to last Byte of frame (TransferStatus)              */
                pEpilog = &((EDD_UPPER_MEM_U8_PTR_TYPE) pUDPData)[UDPLen-1];

                /* check TransferStatus and move pEpilog to DataStatus */
                if ( *pEpilog-- == 0 )
                {
                    LSA_BOOL IsBackup = LSA_FALSE;
                    LSA_BOOL IsDataValid = LSA_FALSE;

                    /* Read DataStatus */
                    DataStatus = *pEpilog--;

                    IsBackup = !( (DataStatus & DATA_STATUS_STATE_MASK) == DATA_STATUS_STATE_VALUE );
                    IsDataValid = ( (DataStatus & DATA_STATUS_GOOD_MASK) == DATA_STATUS_GOOD_VALUE );

                    if( IsDataValid ||
                    (!IsDataValid && IsBackup) ) //lint !e774 JB 05/12/2014 readability
                    {
                        Good = LSA_TRUE;

                        /* pEpilog now points to last byte of CycleCounter */
                        /* read CycleCounter (Big-Endian!)                 */
                        CycleCounter = (((LSA_UINT16)*(pEpilog-1)) << 8) + *pEpilog;

                        /* check CycleCounter only if this is not the */
                        /* first frame after activate.                */

                        if (pCSRTFrameTable->State >= EDDS_SRT_CONS_STATE_ON_3)
                        {

                            /* The CycleCounter is older as the current one, if the value   */
                            /* is within 4096 digits behind the current one. because of the */
                            /* wraparound we have to do a little math..                     */
                            /*                                                              */
                            /*                                                              */
                            /* 0                                 0xFFFF                     */
                            /* |--------xxxxxxO...........N--------|                        */
                            /*          |older|                                             */
                            /*           4096                                               */
                            /*                                                              */
                            /* |..N-----xxxxxxO....................|                        */
                            /*          |older|                                             */
                            /*           4096                                               */
                            /* |xxxxO..............N-------------xx|                        */
                            /*  lder|                            |o                         */
                            /*                                                              */
                            /*  N: new Counter                                              */
                            /*  O: old Counter                                              */
                            /*                                                              */
                            /*  Delta = (LSA_UINT16) (N-O)                                  */
                            /*                                                              */
                            /*  Note: a CycleCounter equal (Delta == 0) is handled as old   */

                            Delta = CycleCounter - pCSRTFrameTable->CycleCounter;
                            if ( (Delta == 0) || (Delta > 0xF000) ) /* between -4095..0 */
                            {
                                Good = LSA_FALSE;
                                pCSRTComp->Stats.ConsRxCycleCounterOld++;
                            }
                            if(EDDS_SRT_CONS_STATE_ON_3 != pCSRTFrameTable->State)
                            {
                                pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame after SetToUnknown was received */
                            }
                        }
                        else
                        {
                            pCSRTFrameTable->State = EDDS_SRT_CONS_STATE_ON_AGAIN; /* first frame received */
                        }

                        if ( Good ) // old/new frame check
                        {
                            pCSRTFrameTable->CycleCounter = CycleCounter; /* save actual CycleCounter*/
                            pCSRTFrameTable->DataStatus = DataStatus; /* save actual DataStatus  */
                            pCSRTFrameTable->StampAddr.udp.CycleCntStamp = pCSRTComp->Prov.ProviderCycle.CycleCounter;

                            if( IsDataValid )
                            {
                                LSA_UINT16 EDDS;
                                LSA_UINT8* pValue;
                                EDDS_EXCHANGE_TYPE Temp;
                                EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;

                                pCr = &pCSRTComp->Cons.pConsCrTbl[pCSRTTable->CrNumber - 1]; // 1 .. ConsumerCnt

                                // set reference in EDDS to received buffer
                                EDDS = *((LSA_UINT8*)&pCr->EDDS);
                                pCr->pBuffer[EDDS] = *pBuffer;

                                Temp = pCr->EDDS;

                                // set new flag in Temp
                                pValue = ((LSA_UINT8*)&Temp);
                                *pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

                                Temp = EDDS_IOBUFFER_EXCHANGE( &pCr->XChange, Temp );

                                // reset new flag in User
                                pValue = ((LSA_UINT8*)&Temp);
                                *pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;

                                pCr->EDDS = Temp;

                                // return buffer referenced in EDDS as new receive resource
                                // NOTE: Buffer that is currently used in EDDS as receive resource must be
                                //       set to NULL!
                                EDDS = *((LSA_UINT8*)&pCr->EDDS);
                                *pBuffer = pCr->pBuffer[EDDS];
                                pCr->pBuffer[EDDS] = LSA_NULL;
                            }        // exchange buffer --> only when data valid

                        }                // old/new frame check
                    }                // data valid
                }                // transfer status != 0
            }                // length and mac check
            else
            {
                pCSRTComp->Stats.ConsRxWrongUDPLenOrIP++;
            }
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "OUT:EDDS_RTConsumerRxIndicateCyclicUDP(*pBuffer: 0x%X)",
    *pBuffer);

    LSA_UNUSED_ARG(TraceIdx);
}

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :  EDDS_RTConsumerHandleClearOnMiss              +*/
/*+  Input/Output          :  EDDS_DDB_COMP_CSRT_TYPE_PTR       pCSRTComp   +*/
/*+                           EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame  +*/
/*+                           EDDS_CSRT_CONSUMER_TYPE_PTR       pCons       +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to CSRT-Management.                               +*/
/*+  pConsFrame : Pointer to Consumer management                            +*/
/*+  pCons      : Pointer to Consumer management                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles a MISS of a consumer.               +*/
/*+                                                                         +*/
/*+               If the ClearOnMiss flag is set for this consumer a null   +*/
/*+               buffer with New flag is exchanged. Otherwise nothing is   +*/
/*+               done.                                                     +*/
/*+                                                                         +*/
/*+               The consumer should have already received valid frames.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: Be careful when using this function, make sure not to use any null ptr's!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerHandleClearOnMiss(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR_TO_CONST pConsFrame,
        EDDS_CSRT_CONSUMER_TYPE_PTR_TO_CONST pCons) {

    /* ClearOnMiss flag set? */
    if (pConsFrame->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS) {
        LSA_UINT16 EDDS;
        LSA_UINT8* pValue;
        EDDS_EXCHANGE_TYPE Temp;
        EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
        EDD_UPPER_MEM_U8_PTR_TYPE pBuffer;
        LSA_UINT16 Len = 0;

        /* get frame length without VLAN tag depending on consumer type */
        if (pConsFrame->Type & EDDS_CSRT_CONSUMER_TYPE_XRT) {
            Len = EDDS_CSRT_FRAME_IO_START_WITHOUT_VLAN + pCons->DataLen;
        }
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
        else if
        {
            Len = EDDS_CSRT_FRAME_IO_START_WITHOUT_VLAN +
            sizeof(EDDS_IP_HEADER_TYPE) + sizeof(EDDS_UDP_HEADER_TYPE) + sizeof(EDDS_FRAME_ID_TYPE) +
            pCons->DataLen; /* 4 bytes APDU status */
        }
#endif
        else {
            EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,
                    "EDDS_RTConsumerHandleClearOnMiss: Illegal consumer type %d",
                    pConsFrame->Type);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
            EDDS_MODULE_ID,
            __LINE__);
        }

        /* clear buffer */
        /* NOTE: clear + 4 bytes for APDU status --> TransferStatus = 0 */
        EDDS_MEMSET_UPPER(pCSRTComp->Cons.pClearOnMissBuffer, 0, Len + 4);

        /* setup APDU status in ClearOnMiss buffer */
        pBuffer =
                (EDD_UPPER_MEM_U8_PTR_TYPE) pCSRTComp->Cons.pClearOnMissBuffer;
        pBuffer[Len++] = (LSA_UINT8) (pConsFrame->CycleCounter >> 8);
        pBuffer[Len++] = (LSA_UINT8) (pConsFrame->CycleCounter & 0xFF);
        pBuffer[Len] = (LSA_UINT8) (pConsFrame->DataStatus);

        /* setup frame without VLAN tag */
        /* NOTE: IOBuffer determines the offset to IO data depending on present VLAN tag */
        ((EDD_UPPER_MEM_U16_PTR_TYPE ) pCSRTComp->Cons.pClearOnMissBuffer)[EDDS_FRAME_LEN_TYPE_WORD_OFFSET] =
                (LSA_UINT16) ~EDDS_VLAN_TAG;

        /* get consumer Cr */
        pCr = &pCSRTComp->Cons.pConsCrTbl[pCons->CrNumber - 1]; // 1 .. ConsumerCnt

        /* set reference in EDDS to received buffer */
        EDDS = *((LSA_UINT8*) &pCr->EDDS);
        pCr->pBuffer[EDDS] = pCSRTComp->Cons.pClearOnMissBuffer;

        /* set new flag in EDDS entry */
        Temp = pCr->EDDS;
        pValue = ((LSA_UINT8*) &Temp);
        *pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

        /* exchange EDDS with XChange entry */
        Temp = EDDS_IOBUFFER_EXCHANGE(&pCr->XChange, Temp);

        /* reset new flag in User */
        pValue = ((LSA_UINT8*) &Temp);
        *pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
        pCr->EDDS = Temp;

        /* return buffer referenced in EDDS as new ClearOnMiss buffer */
        /* NOTE: Buffer that is currently used in EDDS must be set to NULL! */
        EDDS = *((LSA_UINT8*) &pCr->EDDS);
        pCSRTComp->Cons.pClearOnMissBuffer = pCr->pBuffer[EDDS];
        pCr->pBuffer[EDDS] = LSA_NULL;
    }
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerProcessTimerScoreboard       +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR    pCSRTComp    +*/
/*+                             LSA_UINT32                     ConsumerID   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to SRT-Management.                                +*/
/*+  ConsIdx    : ConsumerID to actualize state for                         +*/
/*+                                                                         +*/
/*+  Result     : LSA_FALSE       DHT is not expired.                       +*/
/*+               LSA_TRUE        DHT is expired.                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the consumer timer scoreboard.      +*/
/*+                                                                         +*/
/*+            Calculate delta between current cycle counter and cycle      +*/
/*+            counter of last consumer reception. If delta exceeds         +*/
/*+            the maximum value for this consumer, the DHT expires.        +*/
/*+                                                                         +*/
/*+            Consumer is active. Is DHT expired ...                       +*/
/*+            ... and the consumer has never received a valid frame set    +*/
/*+                state to EDDS_SRT_CONS_STATE_ON_MNS. MISS_NOT_STOPPED    +*/
/*+                will be indicated with SET_TO_UNKNOWN set for this       +*/
/*+                consumer.                                                +*/
/*+            ... and the consumer has already received valid frames set   +*/
/*+                state to EDDS_SRT_CONS_STATE_OFF_MISS. MISS will be      +*/
/*+                indicated.                                               +*/
/*+                                                                         +*/
/*+            The consumer should be active (not checked here).            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
static LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerProcessTimerScoreboard(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT32 ConsumerID)
{
    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
    LSA_UINT32 Delta;
    LSA_BOOL isDhtExpired = LSA_TRUE;

    pCons = &pCSRTComp->Cons.pConsTbl[ConsumerID];
    pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[ConsumerID];

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE_LOW,
            "EDDS_RTConsumerProcessTimerScoreboard: ConsID %d (current state %d)",
            ConsumerID, pConsFrame->State);

    /* Check if we have to skip here. */
    if ((pConsFrame->State < EDDS_SRT_CONS_STATE_ON_0)
            || (pCSRTComp->Prov.ProviderCycle.CycleCounter
                    - pCons->SkipCycleStamp <= pCons->WatchdogCycleCnt))
    {
        //pConsFrame->State = pConsFrame->State;
    }
    else
    {
        /* Get delta between current cycle counter and cycle counter of last reception. */
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
        if( (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_UDP )
        {
            /* UDP Consumer have a 32 Bit Stamp!*/
            Delta = pCSRTComp->Prov.ProviderCycle.CycleCounter - pConsFrame->StampAddr.udp.CycleCntStamp;
        }
        else
#endif
        {
            /* xRT Consumer have a 16 Bit Stamp!*/
            Delta = (LSA_UINT16) (pCSRTComp->Prov.ProviderCycle.CycleCounter
                    - pConsFrame->StampAddr.xrt.CycleCntStamp);
        }

        /* Consumer is active... is DHT expired?                        */
        /* ... but has never received a valid frame set state to ON_MNS */
        /* .. has already received valid frames set state to OFF_MISS   */
        if (Delta > pCons->WatchdogCycleCnt)
        {
            switch (pConsFrame->State)
            {
            case EDDS_SRT_CONS_STATE_ON_0:
            case EDDS_SRT_CONS_STATE_ON_2:
            case EDDS_SRT_CONS_STATE_ON_MNS:
                /* do nothing */
                break;
            case EDDS_SRT_CONS_STATE_ON_1:
                /* set consumer state to ON_MNS */
                /* NOTE: MISS_NOT_STOPPED is only indicated with SET_TO_UNKNOWN set */
                pConsFrame->State = EDDS_SRT_CONS_STATE_ON_MNS;
                break;
            case EDDS_SRT_CONS_STATE_ON_AGAIN:
            case EDDS_SRT_CONS_STATE_ON_3:
            case EDDS_SRT_CONS_STATE_ON_4:
                /* set consumer state to OFF_MISS, MISS indication will be indicated */
                /* NOTE: with setting consumer state to OFF_MISS consumer is not anymore handled within reception handler */
                pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_MISS;
                EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_WARN,
                        "EDDS_RTConsumerProcessTimerScoreboard: Consumer MISS. State: %d, ConsID: %d",
                        pConsFrame->State, ConsumerID);
                EDDS_RTConsumerHandleClearOnMiss(pCSRTComp, pConsFrame, pCons);
                pCSRTComp->Cons.ConsumerActCnt--;   /* consumer is now passivated */
                break;
            default:
                EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,
                        "EDDS_RTConsumerProcessTimerScoreboard: Illegal state %d, ConsID: %d",
                        pConsFrame->State, ConsumerID);
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                EDDS_MODULE_ID,
                __LINE__);
                break;
            }
            EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE_LOW,
                    "EDDS_RTConsumerProcessTimerScoreboard: ConsID %d -->state %d",
                    ConsumerID, pConsFrame->State);
        }
        else
        {
            isDhtExpired = LSA_FALSE;
        }
    }
    return isDhtExpired;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerProcessScoreboard            +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR    pCSRTComp    +*/
/*+                             LSA_UINT32                     ConsumerID   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : pointer to SRT management                                 +*/
/*+  ConsumerID : ConsumerID of consumer to be processed                    +*/
/*+                                                                         +*/
/*+  Result     : LSA_FALSE : Event could not be handled due to missing     +*/
/*+                           indication resource.                          +*/
/*+               LSA_TRUE  : Event could be handled.                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handle consumer events. If an event is present an         +*/
/*+               indication resource is filled.                            +*/
/*+               Note: Consumer state shall not be processed without       +*/
/*+                     present indication resource. This is assured within +*/
/*+                     consumer cycle handler.                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
static LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerProcessScoreboard(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT32 ConsumerID)
{
    LSA_UINT8 Act, Ind, Change;
    LSA_UINT32 Event = 0;
    LSA_UINT32 EventStatus = 0;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;

    pConsFrame =&pCSRTComp->Cons.pConsFrameTbl[ConsumerID];
    Act = pCSRTComp->Cons.pConsTbl[ConsumerID].ActStatusFlags;
    Ind = pCSRTComp->Cons.pConsTbl[ConsumerID].IndStatusFlags;

    /* For every status we have on bit within DataStatus. We check if any bit has changed */
    /* since last indication. This is the case if the bit in Act and Ind is different. */
    Change = Act ^ Ind; /* bit is 1 if changed */

    /* check for MISS_NOT_STOPPED, MISS or AGAIN event */
    switch (pConsFrame->State)
    {
        /* SetToUnknown on a passive consumer (only initialized or passivated) --> indicate MNS */
        /* do not indicate a MNS without SetToUnknown */
    case EDDS_SRT_CONS_STATE_OFF_MNS:
        pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_0;
        Event = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
        break;

        /* SetToUnknown for a passivated consumer that has already seen a MISS or expired DHT for an activate consumer --> indicate a MISS */
        /* if consumer is already auto stopped by MISS, do not indicate a MISS without SetToUnknown */
    case EDDS_SRT_CONS_STATE_OFF_MISS:
        EDDS_RTConsumerStop(pCSRTComp, pConsFrame);
        Event = EDD_CSRT_CONS_EVENT_MISS;
        /* no break, fall through */
        //lint -fallthrough JB 18/11/2014
    case EDDS_SRT_CONS_STATE_OFF_1:
        EventStatus = EDD_CSRT_CONS_EVENT_MISS;
        break;

        /* SetToUnknown or expired DHT for an activated consumer that has never seen a valid frame --> indicate MNS */
        /* do not indicate a MNS without SetToUnknown */
    case EDDS_SRT_CONS_STATE_ON_MNS:
        pConsFrame->State = EDDS_SRT_CONS_STATE_ON_2;
        Event = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
        /* no break, fall through */
        //lint -fallthrough JB 18/11/2014
    case EDDS_SRT_CONS_STATE_OFF_0:
    case EDDS_SRT_CONS_STATE_ON_0: /* default value after activate */
    case EDDS_SRT_CONS_STATE_ON_1: /* default value after activate */
    case EDDS_SRT_CONS_STATE_ON_2:
        EventStatus = EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED;
        break;

        /* frame reception of either initial first frame or first frame after SetToUnknown --> indicate AGAIN */
        /* if consumer is already receiving valid frames, do not indicate a AGAIN without SetToUnknown */
    case EDDS_SRT_CONS_STATE_ON_AGAIN:
        pConsFrame->State = EDDS_SRT_CONS_STATE_ON_3;
        Event = EDD_CSRT_CONS_EVENT_AGAIN;
        /* no break, fall through */
        //lint -fallthrough JB 18/11/2014
    case EDDS_SRT_CONS_STATE_ON_3:
    case EDDS_SRT_CONS_STATE_ON_4:
        EventStatus = EDD_CSRT_CONS_EVENT_AGAIN;
        break;
    default:
        EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,
                "EDDS_RTConsumerProcessScoreboard: Illegal state %d, ConsID: %d",
                pConsFrame->State, ConsumerID);
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
        EDDS_MODULE_ID,
        __LINE__);
        break;
    }

    /* check redundancy bit 0/1 change      */
    /* 0: STOP , 1: RUN           */
    if (Change & EDD_CSRT_DSTAT_BIT_REDUNDANCY)
    {
        if (Act & EDD_CSRT_DSTAT_BIT_REDUNDANCY)
            Event |= EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS;
        else
            Event |= EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS;
    }
    /* set EventState */
    if (Act & EDD_CSRT_DSTAT_BIT_REDUNDANCY)
        EventStatus |= EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS;
    else
        EventStatus |= EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS;

    /* check PRIMARY/BACKUP change */
    /* 0: BACKUP, 1: PRIMARY       */
    if (Change & EDD_CSRT_DSTAT_BIT_STATE)
    {
        if (Act & EDD_CSRT_DSTAT_BIT_STATE)
            Event |= EDD_CSRT_CONS_EVENT_PRIMARY;
        else
            Event |= EDD_CSRT_CONS_EVENT_BACKUP;
    }
    /* set EventState */
    if (Act & EDD_CSRT_DSTAT_BIT_STATE)
        EventStatus |= EDD_CSRT_CONS_EVENT_PRIMARY;
    else
        EventStatus |= EDD_CSRT_CONS_EVENT_BACKUP;

    /* check STOP/RUN change      */
    /* 0: STOP , 1: RUN           */
    if (Change & EDD_CSRT_DSTAT_BIT_STOP_RUN)
    {
        if (Act & EDD_CSRT_DSTAT_BIT_STOP_RUN)
            Event |= EDD_CSRT_CONS_EVENT_RUN;
        else
            Event |= EDD_CSRT_CONS_EVENT_STOP;
    }
    /* set EventState */
    if (Act & EDD_CSRT_DSTAT_BIT_STOP_RUN)
        EventStatus |= EDD_CSRT_CONS_EVENT_RUN;
    else
        EventStatus |= EDD_CSRT_CONS_EVENT_STOP;

    /* check StationFailure change */
    /* 0: Failure, 1: OK           */
    if (Change & EDD_CSRT_DSTAT_BIT_STATION_FAILURE)
    {
        if (Act & EDD_CSRT_DSTAT_BIT_STATION_FAILURE)
            Event |= EDD_CSRT_CONS_EVENT_STATION_OK;
        else
            Event |= EDD_CSRT_CONS_EVENT_STATION_FAILURE;
    }
    /* set EventState */
    if (Act & EDD_CSRT_DSTAT_BIT_STATION_FAILURE)
        EventStatus |= EDD_CSRT_CONS_EVENT_STATION_OK;
    else
        EventStatus |= EDD_CSRT_CONS_EVENT_STATION_FAILURE;

    /* Event(s) present? */
    if (0 != Event)
    {
        EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
                "EDDS_RTConsumerProcessScoreboard: Event (ConsumerID: %d, Event: %Xh)",
                ConsumerID, Event);

        /* Fill event(s) into indication resource. It is assured that event(s) */
        /* of the current processed consumer can always be handled. */
        /* Note: Indication resource is not returned to the user within scheduler context. */
        if (!EDDS_RTConsumerIndicationFill(pCSRTComp,
                pCSRTComp->Cons.pConsTbl[ConsumerID].UserID, Event, EventStatus,
                pCSRTComp->Cons.pConsFrameTbl[ConsumerID].CycleCounter))
        {
            EDDS_CRT_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE_HIGH,
                    "EDDS_RTConsumerProcessScoreboard: No indication resources present!");

            /* No more indication resources present, skip further consumer scoreboard processing. */
            return LSA_FALSE;
        }
        else
        {
            /* Indication could be handled. Actualize consumer DataStatus. */
            pCSRTComp->Cons.pConsTbl[ConsumerID].IndStatusFlags =
                    pCSRTComp->Cons.pConsTbl[ConsumerID].ActStatusFlags;

            return LSA_TRUE;
        }
    }

    return LSA_TRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerIndicationFill               +*/
/*+  Input                 :    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp  +*/
/*+                        :    LSA_UINT32                   UserID         +*/
/*+                             LSA_UINT32                   Event          +*/
/*+                             LSA_UINT32                   EventStatus    +*/
/*+                             LSA_UINT16                   CycleCounter   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp    : Pointer to SRT management                               +*/
/*+  UserID       : UserID from consumer add                                +*/
/*+  Event        : consumer event                                          +*/
/*+  EventStatus  : consumer event status                                   +*/
/*+  CycleCounter : current cycle counter                                   +*/
/*+                                                                         +*/
/*+  Result     : LSA_TRUE  : Event was handled.                            +*/
/*+               LSA_FALSE : Event could not be handled due to missing     +*/
/*+                           indication resource.                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Fills event(s) in indication resource. If a resource is   +*/
/*+               full it will be moved from request queue to the           +*/
/*+               confirmation queue.                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerIndicationFill(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT32 UserID,
        LSA_UINT32 Event,
        LSA_UINT32 EventStatus,
        LSA_UINT16 CycleCounter)
{
    EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE pInd;
    LSA_BOOL Status;
    EDD_UPPER_RQB_PTR_TYPE pRQB;

    EDDS_CRT_TRACE_03(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerIndicationFill(pCSRTComp: 0x%X, UserID: 0x%X, Event: 0x%X)",
            pCSRTComp, UserID, Event);

    pRQB = pCSRTComp->Indication.IndicationRequestQueue.pBottom;

    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        Status = LSA_TRUE;

        pInd = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE) pRQB->pParam;
        pInd->Data[pInd->Count].Event = Event;
        pInd->Data[pInd->Count].EventStatus = EventStatus;
        pInd->Data[pInd->Count].UserID = UserID;
        pInd->Data[pInd->Count].CycleCnt = CycleCounter;
        pInd->Data[pInd->Count].DebugInfo[0] = 0x00;
        pInd->Data[pInd->Count].DebugInfo[1] = 0x00;
        pInd->Data[pInd->Count].DebugInfo[2] = (CycleCounter & 0xFF00) >> 8;
        pInd->Data[pInd->Count].DebugInfo[3] = (CycleCounter & 0x00FF)/*>>0*/;

        EDDS_CRT_TRACE_04(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE,
                "EDDS_RTConsumerIndicationFill(pRQB:0x%X, Count:%d): Event: 0x%X, UserID: 0x%X", pRQB, pInd->Count, Event,
                UserID);

        /* maximum number of events per indication resource reached? */
        if (++pInd->Count == EDD_CFG_CSRT_MAX_INDICATIONS)
        {
            EDDS_SRT_LINK_BASE_TYPE_PTR refConfirmationQueue;
            /* move indication resource from request queue to confirmation queue */

            EDDS_RQB_REM_BLOCK_BOTTOM(pCSRTComp->Indication.IndicationRequestQueue.pBottom,
                                pCSRTComp->Indication.IndicationRequestQueue.pTop, pRQB);

            refConfirmationQueue = &pCSRTComp->Indication.IndicationConfirmationQueue;
            EDDS_RQB_PUT_BLOCK_TOP(refConfirmationQueue->pBottom,refConfirmationQueue->pTop,pRQB);
            pCSRTComp->Indication.IndicationTriggerPending = LSA_TRUE;
        }
    }
    else
    {
        EDDS_CRT_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_UNEXP,
                "EDDS_RTConsumerIndicationFill: No SRT indication resource present");

        Status = LSA_FALSE;
    }

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTConsumerIndicationFill(), Status: 0x%X", Status);
    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerIndicationFinish             +*/
/*+  Input                 :    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to SRT management                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If an indication resource within request queue contains   +*/
/*+               consumer events it will be moved to confirmation queue.   +*/
/*+               This function is called on finishing the consumer cycle.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerIndicationFinish(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp)
{
    EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE     pInd;
    EDD_UPPER_RQB_PTR_TYPE                  pRQB;

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerIndicationFinish(pCSRTComp: 0x%X)", pCSRTComp);

    pRQB = pCSRTComp->Indication.IndicationRequestQueue.pBottom;

    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        pInd = (EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

        /* any consumer event(s) present? */
        if (pInd->Count)
        {
            EDDS_SRT_LINK_BASE_TYPE_PTR refConfirmationQueue;
            /* move indication resource from request queue to confirmation queue */

            EDDS_RQB_REM_BLOCK_BOTTOM(pCSRTComp->Indication.IndicationRequestQueue.pBottom,
                    pCSRTComp->Indication.IndicationRequestQueue.pTop, pRQB);

            refConfirmationQueue = &pCSRTComp->Indication.IndicationConfirmationQueue;
            EDDS_RQB_PUT_BLOCK_TOP(refConfirmationQueue->pBottom,refConfirmationQueue->pTop,pRQB);
            pCSRTComp->Indication.IndicationTriggerPending = LSA_TRUE;
        }
    }

    EDDS_CRT_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_CSRTIndicationFinish()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerCycleRequest                 +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Consumer Cycle Timer request.                             +*/
/*+               This function is called after a consumer-timer-request    +*/
/*+               occured (triggered by EDDS_DO_CONSUMER_TIMER_REQUEST)     +*/
/*+                                                                         +*/
/*+               This request will start a new consumer check cycle.       +*/
/*+                                                                         +*/
/*+               We caclulate all knots for this cycle to check and look   +*/
/*+               if all frames arrived. if not we signal an indication if  +*/
/*+               indication resources present.                             +*/
/*+                                                                         +*/
/*+      Note:    there is one cycle "jitter" with the test. so it is       +*/
/*+               possible, that a consumer is added and checked at once    +*/
/*+               which result in an errorneous situation, because the      +*/
/*+               cycle wasnt really done. So the first Cycle of a consumer +*/
/*+               will be ignored. This results in an first check cycle of  +*/
/*+               CheckCycle <= Cycle <= CheckCycle*2 for the consumer.     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerCycleRequest(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    LSA_UINT32 Interval, Mask;
    LSA_UINT32 KnotIndex;
    LSA_UINT16 ConsIdx;
    LSA_BOOL IndResourcePresent;
    LSA_BOOL AllDone;
    LSA_UINT32 TraceIdx;
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
    LSA_UINT8 Run;
#endif

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerCycleRequest(pDDB: 0x%X)", pDDB);

    pCSRTComp = pDDB->pSRT;

    AllDone = LSA_FALSE;
    IndResourcePresent = LSA_TRUE;

    if(LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Indication.IndicationRequestQueue.pBottom, LSA_NULL))
    {
        IndResourcePresent = LSA_FALSE;
    }

    /*----------------------------------------------------------------------------*/
    /* Optional check for jitter detection. (if the time since last check is      */
    /* much shorter then the ConsumerCheckCycle). This can occur if we were       */
    /* delayed for too long. As result we may do two successive checks within     */
    /* a very short time which may result in detection of errorneous MISS         */
    /* detection.                                                                 */
    /* So the Systemadaption can verify if enough time elapsed since last test    */
    /* and if not we skip this test.                                              */
    /*----------------------------------------------------------------------------*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK

    /* Systemadaption can check if the time since last call is long enough        */
    /* (for example: if we have a 3ms cycle it can check if at least 2ms elapsed) */

    Run = EDDS_CSRT_CYCLE_JITTER_CHECK_DONT_DO_CONS_CHECK;

    EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT(
            pCSRTComp->ConsCycleTimer.TimerID,
            pCSRTComp->ConsCycleTimer.BaseTimeNS,
            &pCSRTComp->Cons.ConsumerCycle.TimerValue,
            &Run);

    if (EDDS_CSRT_CYCLE_JITTER_CHECK_DO_CONS_CHECK == Run )
    {

        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                "EDDS_RTConsumerCycleRequest: Jitter drop (current cycle: 0x%X)",
                pCSRTComp->Cons.ConsumerCycle.CycleCounter);

        pCSRTComp->Stats.ConsCycleJitterDrop++;
        pCSRTComp->ConsTimerRQB.InUse = LSA_FALSE; /* RQB is free for further use now*/
    }
    else
#endif
    {

        pCSRTComp->Cons.ConsumerCycle.CycleCounter++;

        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerCycleRequest: Starting consumer cycle (0x%X)",
                pCSRTComp->Cons.ConsumerCycle.CycleCounter);

        Interval = pCSRTComp->Cons.ConsumerCycle.MinInterval;
        Mask = Interval - 1;
        KnotIndex = (Mask)
                + ((pCSRTComp->Cons.ConsumerCycle.CycleCounter) & Mask);

        /*----------------------------------------------------------------------------*/
        /* get ConsIdx of first Consumer. Check if KnotElement present (may not)      */
        /* If not we can advance to next Knot. (MinInterval not updated yet)          */
        /*----------------------------------------------------------------------------*/

        if (pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx
                == EDDS_SRT_IDX_NOT_USED)
        {
            ConsIdx = EDDS_SRT_CHAIN_IDX_END;
        }
        else
        {
            ConsIdx =
                    pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ConsumerCheckList.BottomIdx;
        }

        /*----------------------------------------------------------------------------*/
        /* loop until all consumers for this cycle checked.                           */
        /* one consumer can indicate more then one status!                            */
        /*----------------------------------------------------------------------------*/

        while (!AllDone)
        {
            /* check if we have to switch to next knot */

            if (ConsIdx == EDDS_SRT_CHAIN_IDX_END)
            {
                /* check if we have done all knots, if  not switch to next one   */

                if (Interval < pCSRTComp->Cons.ConsumerCycle.MaxInterval)
                {
                    Interval = Interval * 2;
                    Mask = Interval - 1;
                    KnotIndex = Mask
                            + ((pCSRTComp->Cons.ConsumerCycle.CycleCounter)
                                    & Mask);

                    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
                            "EDDS_RTConsumerCycleRequest: Switch to KnotIDX: %d, Int: %d, CyCnt: %d",
                            KnotIndex, Interval,
                            pCSRTComp->Cons.ConsumerCycle.CycleCounter);

                    /* if knot not used set to SRT_CHAIN_IDX_END */
                    if (pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx
                            == EDDS_SRT_IDX_NOT_USED)
                    {
                        ConsIdx = EDDS_SRT_CHAIN_IDX_END;
                    }
                    else
                    {
                        ConsIdx =
                                pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ConsumerCheckList.BottomIdx;
                    }

                }
                else
                {
                    AllDone = LSA_TRUE;
                }
            }
            else /* valid index */
            {
                /* only check active consumers which watchdogcounter reached 0  */
                /* NOTE: EDDS_SRT_CONS_STATE_OFF_MISS and EDDS_SRT_CONS_STATE_OFF_MNS have */
                /*       to be handled here as well because of SetToUnknown */
                if (((pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State >= EDDS_SRT_CONS_STATE_ON_0)||
                        pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State == EDDS_SRT_CONS_STATE_OFF_MISS ||
                        pCSRTComp->Cons.pConsFrameTbl[ConsIdx].State == EDDS_SRT_CONS_STATE_OFF_MNS)&&
                        ((--pCSRTComp->Cons.pConsTbl[ConsIdx].WatchdogCnt) == 0))
                {
                    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
                    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;

                    pCons = &pCSRTComp->Cons.pConsTbl[ConsIdx];
                    pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[ConsIdx];

                    /* reload counter */
                    pCons->WatchdogCnt = pCons->WatchdogFactor;

                    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
                            "EDDS_RTConsumerCycleRequest: ConsumerActualizeState (ConsIdx: %d)",
                            ConsIdx);

                    /* process the timer score board (DHT) only for active consumer */
                    if(pConsFrame->State >= EDDS_SRT_CONS_STATE_ON_0)
                    {
                        /* process consumer timer scoreboard --> DHT for MNS or MISS detection */
                        if(!EDDS_RTConsumerProcessTimerScoreboard(pCSRTComp, ConsIdx))
                        {
                            /* DHT is not expired, save current DataStatus within ActStatusFlags */
                            /* for comparing with IndStatusFlags. */
                            /* NOTE: IndStatusFlags contains the last indicated DataStatus. */
                            pCons->ActStatusFlags &= ~EDDS_CSRT_CONS_STAT_DSTAT_MSK;
                            pCons->ActStatusFlags |= (pConsFrame->DataStatus & EDDS_CSRT_CONS_STAT_DSTAT_MSK);
                        }
                    }

                    /* process the consumer scoreboard, determine events and fill indication resources */
                    if (IndResourcePresent)
                    {
                        IndResourcePresent = EDDS_RTConsumerProcessScoreboard(
                                pCSRTComp, ConsIdx);
                    }

                }

                /* proceed with next consumer in phase list */
                ConsIdx = pCSRTComp->Cons.pConsTbl[ConsIdx].CheckLink.Next;
            }
        } /* while */

        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerCycleRequest: Consumer cycle finished");

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END
        EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END(pCSRTComp->ConsCycleTimer.TimerID,
                &pCSRTComp->Cons.ConsumerCycle.TimerValue);
#endif
#endif

    } /* else */

    /* --------------------------------------------------------------------------*/
    /* If there is an indication resource containing consumer events move it     */
    /* from request queue to confirmation queue. IndicationTriggerPending is set */
    /* when at least one indication resource contains consumer events. With this */
    /* flag set the scheduler triggers the RQB context for acknowledging all     */
    /* indication resources within confirmation queue.                           */
    /* --------------------------------------------------------------------------*/
    EDDS_RTConsumerIndicationFinish(pCSRTComp);

    /* consumer cycle is ready */
    pCSRTComp->Cons.ConsumerCycle.CycleInProgress = LSA_FALSE;

    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTConsumerCycleRequest()");

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerConfirmIndication            +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Confirm pending indication resources.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerConfirmIndication(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDD_UPPER_RQB_PTR_TYPE           pLocalRQB;
    EDDS_SRT_LINK_BASE_TYPE          LockFreeLocalQueue; /* local request queue; used to reduce lock */
    EDDS_SRT_LINK_BASE_TYPE_PTR      refFinishedRequestQueue;
    EDDS_LOCAL_HDB_PTR_TYPE          pHDB;
    LSA_UINT32                       TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerConfirmIndication(pDDB: 0x%X)", pDDB);

    pCSRTComp = pDDB->pSRT;

    pLocalRQB = LSA_NULL;
    LockFreeLocalQueue.pBottom = LSA_NULL;
    LockFreeLocalQueue.pTop = LSA_NULL;
    refFinishedRequestQueue = &pCSRTComp->Indication.IndicationConfirmationQueue;

    /* get all finished general RQBs; move them to local, lock free queue */
    EDDS_ENTER(pDDB->hSysDev);
    pCSRTComp->Indication.ConsIndTriggerRQB.InUse = LSA_FALSE;
    LockFreeLocalQueue.pBottom = refFinishedRequestQueue->pBottom;
    LockFreeLocalQueue.pTop = refFinishedRequestQueue->pTop;
    refFinishedRequestQueue->pBottom = LSA_NULL;
    refFinishedRequestQueue->pTop = LSA_NULL;
    EDDS_EXIT(pDDB->hSysDev);

    do
    {
        EDDS_RQB_REM_BLOCK_BOTTOM(LockFreeLocalQueue.pBottom,LockFreeLocalQueue.pTop,pLocalRQB);

        /* now handle all RQBs in lock-free RQB context */
        if(pLocalRQB)
        {
            if (EDD_STS_OK != EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pLocalRQB), &pHDB))
            {
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                EDDS_MODULE_ID,
                __LINE__);
            }
            else
            {
                EDDS_RequestFinish(pHDB, pLocalRQB, EDD_STS_OK);
            }
        }
    } while(pLocalRQB);

    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTConsumerConfirmIndication()");

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerSetMinMaxRatioUsed           +*/
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
/*+               maximum ConsumerCnt and sets the value within             +*/
/*+               the consumer cycle control                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerSetMinMaxRatioUsed(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp) {

    LSA_UINT32 Max;
    LSA_UINT32 Min;
    LSA_UINT32 i;

    Min = EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX - 1;
    Max = EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN - 1; //lint !e778 JB 18/11/2014 define-based behaviour

    for (i = EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN - 1; //lint !e778 JB 18/11/2014 define-based behaviour
            i < EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX; i++)
            {
        /* something present ? */
        if (pCSRTComp->Cons.ConsRatioInfo[i].Cnt) {
            if (i < Min)
                Min = i;
            if (i > Max)
                Max = i;
        }
    }

    if (Min > Max) /* the case if nothing present at all. set Min = Max */
    {
        Min = Max;
    }

    /* Note: We may be interrupted by cyclic check routine here. So it may   */
    /*       be possible that Max is updated, but Min not. This dont realy   */
    /*       care, because its only the border till we check for consumers   */
    /*       and it dont matters if we check more or less. So we dont        */
    /*       protect this sequence.                                          */

    pCSRTComp->Cons.ConsumerCycle.MaxInterval = (LSA_UINT16) EDDSGet2Potenz(
            Max);
    pCSRTComp->Cons.ConsumerCycle.MinInterval = (LSA_UINT16) EDDSGet2Potenz(
            Min);

}



//JB 18/11/2014 previously taken from DDB
//WARNING: be careful when calling this function, make sure not to use pCSRTComp as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerInsert(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT16 Idx) {
    LSA_RESULT Status;
    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
    EDDS_CSRT_KNOT_TYPE_PTR pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
    LSA_UINT32 KnotTableIdx;
    LSA_UINT32 Interval; /* == reduction ratio */
    LSA_UINT16 ReductionFactor; /* 1..x */
    LSA_UINT16 n;

    pCons = &pCSRTComp->Cons.pConsTbl[Idx];
    /* interval is the reduction Ratio */

    ReductionFactor = pCons->CycleReductionNr + 1;
    Interval = pCons->CycleReductionRatio;

    if ((EDD_CYCLE_PHASE_UNDEFINED == ReductionFactor)) {
        ReductionFactor = EDDS_RTGetAndCheckReductionFactor(
                pCons->CycleReductionRatio,
                (LSA_BOOL) (pCons->Type == EDDS_CSRT_PROVIDER_TYPE_UDP));
        pCons->CycleReductionNr = ReductionFactor - 1;
    }

    pCSRTComp->Cons.ConsRatioInfo[pCons->CycleReductionNr].Cnt++;
    EDDS_RTConsumerSetMinMaxRatioUsed(pCSRTComp);

    /* check for valid CyclePhase and Frame not already in use */

    if ((pCons->CyclePhase > Interval)) {

        EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerAdd:Invalid CylcePhase (ID: %d, Phase: %d)",
                Idx, pCons->CyclePhase);

        Status = EDD_STS_ERR_PARAM;
    } else {
        /* calculate Knot-Table index          */
        /* (Interval-1) + (CyclePhase-1)       */
        /* Get Knot with filled in KnotElement */
        Status = EDDS_RTKnotGet(pCSRTComp, Interval + pCons->CyclePhase - 2);
    }

    if (EDD_STS_OK == Status) {
        /*------------------------------------------------------------- */
        /* Put consumer to checklist in knot management (at end of list)*/
        /* sequence in list currently not supported!                    */
        /*------------------------------------------------------------- */

        /* calculate Knot-Table index    */
        /* (Interval-1) + (CyclePhase-1) */
        /* example for Interval = 4 and CyclePhase =2 : Idx = 4 */

        KnotTableIdx = Interval + pCons->CyclePhase - 2;

        pKnot = &pCSRTComp->Knot.pKnotTbl[KnotTableIdx];
        pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

        pCons->KnotTableIdx = KnotTableIdx;
        pCons->CheckLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCons->CheckLink.Prev = pKnotElement->ConsumerCheckList.TopIdx;

        n = pKnotElement->ConsumerCheckList.TopIdx; /* current top idx */
        pKnotElement->ConsumerCheckList.TopIdx = Idx; /* new top idx     */

        /* Note: No protection against MIDDLE context needed because      */
        /*       MIDDLE context only accesses this parameter for chain    */
        /*       handling.                                                */

        if (n == EDDS_SRT_CHAIN_IDX_END) /* it was nothing in chain. */
        {
            pKnotElement->ConsumerCheckList.BottomIdx = Idx;
        } else {
            pCSRTComp->Cons.pConsTbl[n].CheckLink.Next = Idx;
        }

        pKnotElement->ConsumerCnt++;
    }
    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerAdd                          +*/
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
/*+               pParam: EDD_RQB_CSRT_CONSUMER_ADD_TYPE                    +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_CONSUMER_ADD_TYPE                                         +*/
/*+                                                                         +*/
/*+  FrameID                 : FrameID for Provider.                        +*/
/*+  SrcMAC                  : Source MAC address (only XRT)                +*/
/*+  SrcIP                   : Source IP (only xRTOverUDP)                  +*/
/*+  DataLen                 : DataLen of I/O-Data in frame.                +*/
/*+  CycleReductionRatio     : spezifies CycleTime based on CycleBaseFactor +*/
/*+                            (1,2,4,8,16,32,64,128..)                     +*/
/*+  CyclePhase              : spezifies Phase within cycle.                +*/
/*+  WatchdogFactor          : spezifies watchog time                       +*/
/*+  DataHoldFactor          : spezifies datahold time                      +*/
/*+  UserID                  : UserID for Event-indications.                +*/
/*+  ConsumerID              : returned ConsumerID.                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_NO_LOCAL_IP                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to add a consumer (XRT and XRTOverUDP)            +*/
/*+                                                                         +*/
/*+               First we check if free consumer is present. if not we     +*/
/*+               return EDD_STS_ERR_RESOURCE. Then we check for several    +*/
/*+               valid parameters.                                         +*/
/*+                                                                         +*/
/*+               Next we queue the consumer within "used-chain" and put    +*/
/*+               the consumer to the desired Knot-Check-Chain. (depends    +*/
/*+               on CycleReductionRation and CyclePhase).                  +*/
/*+                                                                         +*/
/*+               We do not preinitialize the receive-frames.               +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerAdd(EDD_UPPER_RQB_PTR_TYPE pRQB,
        EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_UINT16 Idx;
    LSA_RESULT Status;
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
    LSA_UINT16 Prev;
    LSA_UINT16 Next;
    LSA_UINT16 ReductionFactor = 0; /* 1..x */
    LSA_UINT16 RtClassProp;
    LSA_BOOL Udp;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerAdd(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)", pRQB,
            pDDB, pHDB);

    Status = EDD_STS_OK;
    Udp = LSA_FALSE;
    Idx = 1;

    pRqbParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE) pRQB->pParam;

    pCSRTComp = pDDB->pSRT;
    RtClassProp = pRqbParam->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK;

    /* ---------------------------------------------------- */
    /* check Properties for RT classes                      */
    /* ---------------------------------------------------- */
    if (RtClassProp != EDD_CSRT_CONS_PROP_RTCLASS_1
            && RtClassProp != EDD_CSRT_CONS_PROP_RTCLASS_2) {
        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerAdd: wrong RT class");
        Status = EDD_STS_ERR_PARAM;
    } /* check Properties for RT classes */

    /* ---------------------------------------------------- */
    /* check Properties flags                               */
    /* ---------------------------------------------------- */
#ifndef EDDS_CFG_SYSRED_API_SUPPORT
    if ( ( EDD_STS_OK == Status ) && (pRqbParam->Properties & EDD_CSRT_CONS_PROP_SYSRED) )
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerAdd: System redundancy is not supported!");
    }
#endif   
        
    if (EDD_STS_OK == Status
            && ((pRqbParam->Properties
                            & EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP)
                    || (pRqbParam->Properties
                            & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY))) {
        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerAdd: unsupported property flag");
        Status = EDD_STS_ERR_PARAM;
    } /* check Properties flags */

    /*---------------------------------------------------------------------------*/
    /* check for xRToverUdp or XRT Provider ADD.                                 */
    /*---------------------------------------------------------------------------*/
    if (EDD_STS_OK == Status) {
        if ((EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SRT_CONSUMER_ADD )
                && ( EDD_CSRT_CONS_PROP_RTCLASS_UDP == RtClassProp)) {
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
            if (( pRqbParam->DataLen < EDD_CSRT_UDP_DATALEN_MIN ) ||
                    ( pRqbParam->DataLen > EDD_CSRT_UDP_DATALEN_MAX ) ||
                    ( pRqbParam->FrameID == 0 ) ||
                    ( pRqbParam->FrameID < pCSRTComp->Cons.ConsumerFrameIDBase2) ||
                    ( pRqbParam->FrameID > pCSRTComp->Cons.ConsumerFrameIDTop2 )
            )
            {

                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "EDDS_RTConsumerAdd:Invalid RQB parameter(s)");

                Status = EDD_STS_ERR_PARAM;
            }
            else
            {
                if (pDDB->pGlob->HWParams.IPAddress.dw == 0 ) /* no IP present ? */
                {
                    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTConsumerAdd:Local IP not set yet!");

                    Status = EDD_STS_ERR_NO_LOCAL_IP;
                }
            }

            Udp = LSA_TRUE;
            Idx = 2; /* UDP always within table 2 */
#else
            Status = EDD_STS_ERR_PARAM; /* should not occure because already checked outside */
#endif
        } else /* XRT */
        {
            if ((pRqbParam->DataLen < EDD_CSRT_DATALEN_MIN)
                    || (pRqbParam->DataLen > EDD_CSRT_DATALEN_MAX)
                    || (pRqbParam->FrameID == 0)) {
                EDDS_CRT_TRACE_00(TraceIdx,
                        LSA_TRACE_LEVEL_ERROR,
                        "EDDS_RTConsumerAdd: Invalid RQB parameter(s)");
                Status = EDD_STS_ERR_PARAM;
            } else {
                /* ---------------------------------------------------- */
                /* check FrameID                                        */
                /* --> FrameID must match RTClass                       */
                /* --> check FrameID area (RTClass boundary/configured  */
                /* ---------------------------------------------------- */
                switch (RtClassProp) {
                case EDD_CSRT_CONS_PROP_RTCLASS_1: {
                    if ((EDD_SRT_FRAMEID_CSRT_START2 > pRqbParam->FrameID)
                            || (EDD_SRT_FRAMEID_CSRT_STOP2 < pRqbParam->FrameID)) {
                        Status = EDD_STS_ERR_PARAM;
                    } else {
                        if ((pRqbParam->FrameID
                                >= pCSRTComp->Cons.ConsumerFrameIDBase2)
                                && (pRqbParam->FrameID
                                        <= pCSRTComp->Cons.ConsumerFrameIDTop2)) {
                            Idx = 2; /* table 2 */
                        } else {
                            EDDS_CRT_TRACE_01(TraceIdx,
                                    LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTConsumerAdd: Invalid FrameID: 0x%X)",
                                    pRqbParam->FrameID);
                            Status = EDD_STS_ERR_PARAM;
                        }
                    }
                }
                    break;

                case EDD_CSRT_CONS_PROP_RTCLASS_2: {
                    if ((EDD_SRT_FRAMEID_CSRT_START1 > pRqbParam->FrameID)
                            || (EDD_SRT_FRAMEID_CSRT_STOP1 < pRqbParam->FrameID)) {
                        Status = EDD_STS_ERR_PARAM;
                    } else {
                        if ((pRqbParam->FrameID
                                >= pCSRTComp->Cons.ConsumerFrameIDBase1)
                                && (pRqbParam->FrameID
                                        <= pCSRTComp->Cons.ConsumerFrameIDTop1)) {
                            Idx = 1; /* table 1 */
                        } else {
                            EDDS_CRT_TRACE_01(TraceIdx,
                                    LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTConsumerAdd: Invalid FrameID: 0x%X)",
                                    pRqbParam->FrameID);
                            Status = EDD_STS_ERR_PARAM;
                        }
                    }
                }
                    break;

                default: {
                    EDDS_CRT_TRACE_01(TraceIdx,
                            LSA_TRACE_LEVEL_ERROR,
                            "[H:--] EDDS_RTConsumerAdd(): invalid RTClassProperty (0x%X)",
                            RtClassProp);
                    Status = EDD_STS_ERR_PARAM;
                }
                    break;
                } /* check FrameID */
            }

            Udp = LSA_FALSE;
        }
    }

    /* ---------------------------------------------------- */
    /* check ClearOnMISS                                    */
    /* ---------------------------------------------------- */
    if (   (EDD_CONS_CLEAR_ON_MISS_DISABLE != pRqbParam->IOParams.ClearOnMISS)
        && (EDD_CONS_CLEAR_ON_MISS_ENABLE != pRqbParam->IOParams.ClearOnMISS)  )
    {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDS_RTConsumerAdd: Invalid ClearOnMiss value (%d)!", pRqbParam->IOParams.ClearOnMISS);

        Status = EDD_STS_ERR_PARAM;
    }

    /* ---------------------------------------------------- */
    /* check border values of crnumber                      */
    /* ---------------------------------------------------- */
    if(   (EDD_CRNUMBER_NOT_USED  == pRqbParam->IOParams.CRNumber)
       || (   (pCSRTComp->Cons.ConsumerCnt <  pRqbParam->IOParams.CRNumber)
           && (EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber) ) )

    {
        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerAdd: Invalid cr number!");

        Status = EDD_STS_ERR_PARAM;
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
            for( CrTableIterator = 0 ; CrTableIterator < pCSRTComp->Cons.ConsumerCnt ; CrTableIterator++ )
            {
                //check that crnumber is not in use
                if(    (pRqbParam->IOParams.CRNumber == pCSRTComp->Cons.pConsTbl[CrTableIterator].CrNumber)
                    && ( EDD_CRNUMBER_NOT_USED       != pCSRTComp->Cons.pConsTbl[CrTableIterator].CrNumber) )
                {
                    EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTConsumerAdd: crnumber is already in use!");

                    Status = EDD_STS_ERR_PARAM;
                    break;
                }
            }
        }
    }/* check that each crnumber is used max one time */

    /*---------------------------------------------------------------------------*/
    /* get ReductionFactor(1..x) from ReductionRatio. 0 signals invalid Ratio    */
    /*---------------------------------------------------------------------------*/
    if ((EDD_STS_OK == Status)
            && (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
                    != pRqbParam->CycleReductionRatio)) {
        ReductionFactor = EDDS_RTGetAndCheckReductionFactor(
                pRqbParam->CycleReductionRatio, Udp);

        if (0 == ReductionFactor) {
            EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTConsumerAdd: wrong CycleReductionRatio");
            Status = EDD_STS_ERR_PARAM;
        }
    } /* check CycleReductionRatio */

    /* ---------------------------------------------------- */
    /* check CyclePhase                                     */
    /* ---------------------------------------------------- */
    if ((EDD_STS_OK == Status)
            && (EDD_CYCLE_PHASE_UNDEFINED != pRqbParam->CyclePhase)
            && (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
                    != pRqbParam->CycleReductionRatio)) {
        if (pRqbParam->CyclePhase > pRqbParam->CycleReductionRatio) {
            EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTConsumerAdd: wrong CyclePhase");
            Status = EDD_STS_ERR_PARAM;
        }
    } /* check CyclePhase */

    /* ---------------------------------------------------- */
    /* check DataHoldFactor                                 */
    /* ---------------------------------------------------- */
    if ((EDD_STS_OK == Status)
            && ((pRqbParam->DataHoldFactor
                    < EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
                    || (pRqbParam->DataHoldFactor
                            > EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MAX))) {
        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerAdd: wrong DataHoldFactor");
        Status = EDD_STS_ERR_PARAM;
    } /* check DataHoldFactor */

    if (EDD_STS_OK == Status) {
        /* calculate consumer table idx */
        if (Idx == 2) /* table 2 */
        {
            Idx = ( pRqbParam->FrameID - pCSRTComp->Cons.ConsumerFrameIDBase2 )
                    + pCSRTComp->Cfg.ConsumerCntClass2;
        } else /* table 1 */
        {
            Idx = pRqbParam->FrameID - pCSRTComp->Cons.ConsumerFrameIDBase1;
        }

        EDDS_ASSERT(Idx < pCSRTComp->Cons.ConsumerCnt);

        pCons = &pCSRTComp->Cons.pConsTbl[Idx];
        pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[Idx];

        if (EDDS_CSRT_CONSUMER_TYPE_NOT_USED != pCons->Type) {
            EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTConsumerAdd: ConsumerID %d (FrameID 0x%x) already in use.",
                    Idx, pRqbParam->FrameID);
            Status = EDD_STS_ERR_PARAM;
        }

        if (Status == EDD_STS_OK) {
            /*------------------------------------------------------------- */
            /* copy parameter to management structure                       */
            /*------------------------------------------------------------- */

            pConsFrame->CycleCounter = 0;
            pConsFrame->State = EDDS_SRT_CONS_STATE_INIT;

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
            if ( Udp )
            {
                EDD_MAC_ADR_TYPE _temp = { {0,0,0,0,0,0}};
                pCons->Type = EDDS_CSRT_CONSUMER_TYPE_UDP;

                /* FrameLength will be used as UDPLen for xRToverUDP */
                pConsFrame->FrameLength = pRqbParam->DataLen +
                sizeof(EDDS_FRAME_ID_TYPE) +
                EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;
                pConsFrame->StampAddr.udp.SrcIP = _temp;
            }
            else
#endif
            {
                EDD_MAC_ADR_TYPE _temp = { { 0, 0, 0, 0, 0, 0 } };
                pCons->Type = EDDS_CSRT_CONSUMER_TYPE_XRT;
                pConsFrame->FrameLength = pRqbParam->DataLen +
                EDDS_CSRT_FRAME_IO_DATA_PROLOG_SIZE +
                EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;
                pConsFrame->StampAddr.xrt.SrcMAC = _temp;
            }

            pConsFrame->Type = pCons->Type
                    | (pRqbParam->IOParams.ClearOnMISS ?
                            EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS : 0);
            pCons->KnotTableIdx = 0xFFFFFFFF; /* not enqueued yet. */

            pCons->CyclePhase = pRqbParam->CyclePhase;
            pCons->CycleReductionRatio = pRqbParam->CycleReductionRatio;
            pCons->CycleReductionNr = ReductionFactor - 1;

            /* convert to a multiple of EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MIN               */
            /* we round up by one if not a muliple of EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MIN */
            pCons->WatchdogFactor = pRqbParam->DataHoldFactor
                    / EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN;
            if (pRqbParam->DataHoldFactor % EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
                pCons->WatchdogFactor++;

            if ( EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
                    == pRqbParam->CycleReductionRatio) {
                /* save WatchdogFactor for later calculation in EDDS_RTConsumerControl() */
                pCons->WatchdogCycleCnt = pRqbParam->DataHoldFactor;
            } else {
                /* Number of Provider Cycle Counts for Consumer check */
                pCons->WatchdogCycleCnt = pRqbParam->CycleReductionRatio
                        * pRqbParam->DataHoldFactor;
            }

            /* convert to a multiple of EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN               */
            /* we round up by one if not a muliple of EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN */
            pCons->DataHoldFactor = pRqbParam->DataHoldFactor
                    / EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN;
            if (pRqbParam->DataHoldFactor % EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
                pCons->DataHoldFactor++;

            pCons->WatchdogCnt              = pCons->WatchdogFactor;
            pCons->DataHoldCnt              = pCons->DataHoldFactor;
            pCons->ProviderAutoStopCnt      = 0;
            pCons->ProviderAutoStopEnabled  = LSA_FALSE;
            pCons->DataLen                  = pRqbParam->DataLen;
            pCons->UserID                   = pRqbParam->UserID;
            
            if (pRqbParam->Properties & EDD_CSRT_CONS_PROP_SYSRED)
            {
                pCons->IsSystemRedundant = LSA_TRUE;  
            }
            else
            {
                pCons->IsSystemRedundant = LSA_FALSE;   
            }


            if(EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber)
            {
                pCons->CrNumber                 = pRqbParam->IOParams.CRNumber;
            }

            pRqbParam->ConsumerID = Idx; /* Consumer ID is the index within */
            /* consumer table                  */

            pCSRTComp->Cons.ConsumerUsedCnt++;

            /*------------------------------------------------------------- */
            /* Remove Consumer from free chain list                         */
            /*------------------------------------------------------------- */

            Prev = pCons->UsedFreeLink.Prev;
            Next = pCons->UsedFreeLink.Next;

            if ((Prev == EDDS_SRT_CHAIN_IDX_END)
                    || (Next == EDDS_SRT_CHAIN_IDX_END))

                    {
                if (Prev == EDDS_SRT_CHAIN_IDX_END) {
                    if (Next == EDDS_SRT_CHAIN_IDX_END) {
                        /* only one in list */

                        pCSRTComp->Cons.ConsumerFree.BottomIdx =
                                EDDS_SRT_CHAIN_IDX_END;
                        pCSRTComp->Cons.ConsumerFree.TopIdx =
                                EDDS_SRT_CHAIN_IDX_END;
                    } else {
                        /* first one in list, but not only one */
                        pCSRTComp->Cons.ConsumerFree.BottomIdx = Next;
                        pCSRTComp->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
                    }
                } else {
                    /* last one in list but not only one */
                    pCSRTComp->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
                    pCSRTComp->Cons.ConsumerFree.TopIdx = Prev;
                }
            } else {
                /* in middle of list */
                pCSRTComp->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
                pCSRTComp->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
            }

            /*------------------------------------------------------------- */
            /* Put Consumer to used-chain list (end)                        */
            /*------------------------------------------------------------- */

            /* nothing in queue ? */
            if (pCSRTComp->Cons.ConsumerUsed.TopIdx == EDDS_SRT_CHAIN_IDX_END) {
                pCSRTComp->Cons.ConsumerUsed.BottomIdx = Idx;
                pCSRTComp->Cons.ConsumerUsed.TopIdx = Idx;
                pCons->UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
                pCons->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
            } else {
                pCons->UsedFreeLink.Prev = pCSRTComp->Cons.ConsumerUsed.TopIdx;
                pCons->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
                pCSRTComp->Cons.pConsTbl[pCSRTComp->Cons.ConsumerUsed.TopIdx].UsedFreeLink.Next =
                        Idx;
                pCSRTComp->Cons.ConsumerUsed.TopIdx = Idx;
            }

            if (!( EDD_CYCLE_PHASE_UNDEFINED == pCons->CyclePhase ||
            EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pCons->CycleReductionRatio)) {
                Status = EDDS_RTConsumerInsert(pCSRTComp, Idx);
            }

            EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "EDDS_RTConsumerAdd: CID: %d, Ratio: %d, Phase: %d", Idx,
                    pCons->CycleReductionRatio, pCons->CyclePhase);

        } else {
            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                    "EDDS_RTConsumerAdd: EDDS_RTKnotGet failed. (0x%X)",
                    Status);
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTConsumerAdd(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerRemoveSRT                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ConsumerID      +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to device                                       +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  ConsumerID   : ID of consumer to remove                                +*/
/*+  KnotTableIdx : Index in Knottable where consumer is queued in checklist+*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a consumer from Knot-checklist and set it to      +*/
/*+               "not" InUse.                                              +*/
/*+                                                                         +*/
/*+               Ids and Idx must be valid ! not checked.                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRemoveSRT(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
        LSA_UINT16 ConsumerID,
        LSA_UINT32 KnotTableIdx) {

    EDDS_CSRT_KNOT_TYPE_PTR pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
    LSA_UINT16 Prev;
    LSA_UINT16 Next;

    if (0xFFFFFFFF != KnotTableIdx) {
        pKnot = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
        EDDS_ASSERT(pKnot->KnotElementIdx < pCSRT->Knot.KnotElementCnt);
        pKnotElement = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

        /* this area should be protected against scheduler context */
        EDDS_ENTER(pDDB->hSysDev);

        /*---------------------------------------------------------------------------*/
        /* First we remove the consumer from the Knotmanagment checklist.            */
        /*---------------------------------------------------------------------------*/

        /* four  cases:                                                              */
        /* consumer is the first in list (but not the only one)                      */
        /* consumer is the last in list (but not the only one)                       */
        /* consumer is the only one in list                                          */
        /* consumer is inside linked list                                            */

        /* check if first or last or only in list */

        Prev = pCSRT->Cons.pConsTbl[ConsumerID].CheckLink.Prev;
        Next = pCSRT->Cons.pConsTbl[ConsumerID].CheckLink.Next;

        if ((Prev == EDDS_SRT_CHAIN_IDX_END)
                || (Next == EDDS_SRT_CHAIN_IDX_END))

                {
            if (Prev == EDDS_SRT_CHAIN_IDX_END) {
                if (Next == EDDS_SRT_CHAIN_IDX_END) {
                    /* only one in list */
                    pKnotElement->ConsumerCheckList.BottomIdx =
                            EDDS_SRT_CHAIN_IDX_END;
                    pKnotElement->ConsumerCheckList.TopIdx =
                            EDDS_SRT_CHAIN_IDX_END;

                } else {
                    /* first one in list, but not only one */
                    pKnotElement->ConsumerCheckList.BottomIdx = Next;
                    pCSRT->Cons.pConsTbl[Next].CheckLink.Prev = Prev;
                }
            } else {
                /* last one in list but not only one */
                pCSRT->Cons.pConsTbl[Prev].CheckLink.Next = Next;
                pKnotElement->ConsumerCheckList.TopIdx = Prev;
            }
        } else {
            /* in middle of list */
            pCSRT->Cons.pConsTbl[Prev].CheckLink.Next = Next;
            pCSRT->Cons.pConsTbl[Next].CheckLink.Prev = Prev;
        }

        pKnotElement->ConsumerCnt--; /* Number of Consumers in KnotList */

        EDDS_RTKnotFree(pCSRT, KnotTableIdx); /* remove KnotElement if unused yet */

        /* protected area ends here */
        EDDS_EXIT(pDDB->hSysDev);
    }

    /*---------------------------------------------------------------------------*/
    /* Now we can remove  the consumer from the InUse-chain.                     */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev;
    Next = pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Next;

    if ((Prev == EDDS_SRT_CHAIN_IDX_END) || (Next == EDDS_SRT_CHAIN_IDX_END))

    {
        if (Prev == EDDS_SRT_CHAIN_IDX_END) {
            if (Next == EDDS_SRT_CHAIN_IDX_END) {
                /* only one in list */

                pCSRT->Cons.ConsumerUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pCSRT->Cons.ConsumerUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END;
            } else {
                /* first one in list, but not only one */
                pCSRT->Cons.ConsumerUsed.BottomIdx = Next;
                pCSRT->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
            }
        } else {
            /* last one in list but not only one */
            pCSRT->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
            pCSRT->Cons.ConsumerUsed.TopIdx = Prev;
        }
    } else {
        /* in middle of list */
        pCSRT->Cons.pConsTbl[Prev].UsedFreeLink.Next = Next;
        pCSRT->Cons.pConsTbl[Next].UsedFreeLink.Prev = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* Put this consumer to the end of the free-chain..                          */
    /*---------------------------------------------------------------------------*/

    pCSRT->Cons.pConsTbl[ConsumerID].Type     = EDDS_CSRT_CONSUMER_TYPE_NOT_USED; /* this consumer is no more in use */
    pCSRT->Cons.pConsTbl[ConsumerID].CrNumber = EDD_CRNUMBER_NOT_USED;
    pCSRT->Cons.ConsumerUsedCnt--;

    if (pCSRT->Cons.ConsumerFree.TopIdx == EDDS_SRT_CHAIN_IDX_END) /* nothing in chain ? */
    {
        pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev =
                EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev =
                EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Cons.ConsumerFree.BottomIdx = ConsumerID;
        pCSRT->Cons.ConsumerFree.TopIdx = ConsumerID;
    } else {
        pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Prev =
                pCSRT->Cons.ConsumerFree.TopIdx;
        pCSRT->Cons.pConsTbl[ConsumerID].UsedFreeLink.Next =
                EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Cons.pConsTbl[pCSRT->Cons.ConsumerFree.TopIdx].UsedFreeLink.Next =
                ConsumerID;
        pCSRT->Cons.ConsumerFree.TopIdx = ConsumerID;
    }

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerRemove                       +*/
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
/*+               pParam: EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE                 +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE                                      +*/
/*+                                                                         +*/
/*+  ConsumerID : Valid ConsumerID                                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to remove a  consumer.                            +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+               A consumer  has to be in use and no buffers               +*/
/*+               locked (EDD_STS_ERR_SEQUENCE if not)                      +*/
/*+                                                                         +*/
/*+               Consumers may be active on remove. will be deactivated.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerRemove(
        EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
    LSA_UINT16 CycleReductionNr;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerRemove(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
            pRQB, pDDB, pHDB);

    Status = EDD_STS_OK;
    pCons = LSA_NULL;

    pRqbParam = (EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE) pRQB->pParam;
    pCSRTComp = pDDB->pSRT;

    /* check for valid ID-range. ConsumerID is idx within table */

    if (pRqbParam->ConsumerID >= pCSRTComp->Cons.ConsumerCnt) {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerRemove:Invalid Consumer ID (0x%X)",
                pRqbParam->ConsumerID);

        Status = EDD_STS_ERR_PARAM;
    } else {
        pCons = &pCSRTComp->Cons.pConsTbl[pRqbParam->ConsumerID];

        /* Consumer must be in use */
        if ( EDDS_CSRT_CONSUMER_TYPE_NOT_USED == pCons->Type) {
            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTConsumerRemove: Consumer not inUse! (0x%X)",
                    pRqbParam->ConsumerID);

            Status = EDD_STS_ERR_PARAM;
        } else {
            /* Consumer can only be removed if it has no AutoStop Providers! */
            if (pCons->ProviderAutoStopCnt) {
                EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        "EDDS_RTConsumerRemove: Consumer already has AutoStop Providers! can not be removed! (0x%X)",
                        pRqbParam->ConsumerID);

                Status = EDD_STS_ERR_SEQUENCE;
            }

            if (Status == EDD_STS_OK) {

                EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                        "EDDS_RTConsumerRemove: CID: %d",
                        pRqbParam->ConsumerID);

                CycleReductionNr = pCons->CycleReductionNr;

                /* deactivate Consumer first */

                EDDS_RTConsumerStop(pCSRTComp,
                        &pCSRTComp->Cons.pConsFrameTbl[pRqbParam->ConsumerID]);

                EDDS_RTConsumerRemoveSRT(pDDB, pCSRTComp, pRqbParam->ConsumerID,
                        pCons->KnotTableIdx);

                if (0xFFFF != CycleReductionNr) {
                    pCSRTComp->Cons.ConsRatioInfo[CycleReductionNr].Cnt--;
                }

                EDDS_RTConsumerSetMinMaxRatioUsed(pCSRTComp);
            }
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTConsumerRemove(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerStop                         +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR       pCSRT     +*/
/*+                             EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame+*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  pConsFrame : a Consumer in use                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops a Consumer                                          +*/
/*+               Note: After calling this function                         +*/
/*                      EDDS_RTConsumerCheckTimerSwitch shall be called too +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerStop(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
        EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame)
{
    switch (pConsFrame->State) {
    case EDDS_SRT_CONS_STATE_ON_0:
    case EDDS_SRT_CONS_STATE_ON_1:
    case EDDS_SRT_CONS_STATE_ON_2:
    case EDDS_SRT_CONS_STATE_ON_3:
    case EDDS_SRT_CONS_STATE_ON_4:
    case EDDS_SRT_CONS_STATE_ON_AGAIN:
    case EDDS_SRT_CONS_STATE_ON_MNS:
        pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_0;
        pCSRT->Cons.ConsumerActCnt--;   /* consumer is now stopped and passivated */
        pConsFrame->CycleCounter = 0;
        break;
    case EDDS_SRT_CONS_STATE_OFF_MISS:
        pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_1;
        break;
    case EDDS_SRT_CONS_STATE_OFF_MNS:
    case EDDS_SRT_CONS_STATE_OFF_0:
    case EDDS_SRT_CONS_STATE_OFF_1:
        /* do nothing */
        break;
    default:
        EDDS_CRT_TRACE_03(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_FATAL,
                "EDDS_RTConsumerStop: Illegal state %d, pCSRT %p, pConsFrame %p",
                pConsFrame->State, pCSRT, pConsFrame);
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
        EDDS_MODULE_ID,
        __LINE__);
        break;
    }
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerControlLow                   +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pDDB            +*/
/*+                        :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                 ConsID           +*/
/*+                             LSA_UINT16                 Mode             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to device handle                                  +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  ConsID     : Valid ConsumerID                                          +*/
/*+  Mode       : see service EDD_SRV_SRT_CONSUMER_CONTROL                  +*/
/*+               EDD_CONS_CONTROL_MODE_ACTIVATE                            +*/
/*+               EDD_CONS_CONTROL_MODE_PASSIVATE                           +*/
/*+               EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN                      +*/
/*+               EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE                +*/
/*+               EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE               +*/
/*+                                                                         +*/
/*+  Note: The default-state after activating a not active consumer with    +*/
/*+        SetToUnknownState = FALSE is the MISS-state.                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Control a consumer. Note that Mode must be consistent     +*/
/*+               on calling (e.g. Activate/passivate shall not be set both)+*/
/*+               The consumer must be inUse!                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerControlLow(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT,
        LSA_UINT16 ConsID,
        LSA_UINT16 Mode)
{
    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
    LSA_UINT32 TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    /*---------------------------------------------------------------------------*/
    /* Now set consumer status..                                                 */
    /*---------------------------------------------------------------------------*/
    pCons = &pCSRT->Cons.pConsTbl[ConsID];
    pConsFrame = &pCSRT->Cons.pConsFrameTbl[ConsID];

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
            "EDDS_RTConsumerControlLow: ConsID: %d(state %d), Mode: 0x%X",
            ConsID, pConsFrame->State, Mode);

    /* --------------------------------------------------------------------------*/
    /* disable Provider autostop first if set.                                   */
    /* --------------------------------------------------------------------------*/

    if (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE) {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerControlLow: ProvAutoStop Disable. ConsID: %d.",
                ConsID);

        pCons->ProviderAutoStopEnabled = LSA_FALSE;
    }

    /* --------------------------------------------------------------------------*/
    /* if we should stop do it                                                   */
    /* --------------------------------------------------------------------------*/

    if (Mode & EDD_CONS_CONTROL_MODE_PASSIVATE) {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerControlLow: Deactivate Consumer %d", ConsID);

        EDDS_RTConsumerStop(pCSRT, pConsFrame);
    }

    /* --------------------------------------------------------------------------*/
    /* if we should start do it and handle SetToUnknown                          */
    /* --------------------------------------------------------------------------*/

    if (Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) {

        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerControlLow: Activate Consumer %d.", ConsID);

        /* only if not already active */

        if (pConsFrame->State < EDDS_SRT_CONS_STATE_ON_0) {
            pCSRT->Cons.ConsumerActCnt++;

            /* Setup Check-Counter with 1, because we will drop */
            /* the first check and reload the counter with the  */
            /* real watchdog-count later.                       */

            pCons->WatchdogCnt = 1;
            pCons->SkipCycleStamp = pCSRT->Prov.ProviderCycle.CycleCounter;

            /* Set default status-flags for consumer.          */
#ifdef EDDS_CFG_SYSRED_API_SUPPORT            
            if( pCons->IsSystemRedundant )
            {
                pCons->IndStatusFlags = EDDS_CSRT_SRD_CONS_STAT_DEFAULT;
                pCons->ActStatusFlags = EDDS_CSRT_SRD_CONS_STAT_DEFAULT;
            }
            else
#endif
            {
                pCons->IndStatusFlags = EDDS_CSRT_CONS_STAT_DEFAULT;
                pCons->ActStatusFlags = EDDS_CSRT_CONS_STAT_DEFAULT;
            }

            pConsFrame->State = EDDS_SRT_CONS_STATE_ON_0;
        }
    }

    if (Mode & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN)
    {
        /* protect this against scheduler context */
        /* because some changes could get lost */
        EDDS_ENTER(pDDB->hSysDev);

        switch (pConsFrame->State) {
        /* pCons->SkipCycleStamp  = pCSRT->Prov.ProviderCycle.CycleCounter; */
        case EDDS_SRT_CONS_STATE_OFF_0:
            pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_MNS;
            break;
        case EDDS_SRT_CONS_STATE_ON_0:
        case EDDS_SRT_CONS_STATE_ON_1:
            pConsFrame->State = EDDS_SRT_CONS_STATE_ON_1;
            break;
        case EDDS_SRT_CONS_STATE_ON_2:
            pConsFrame->State = EDDS_SRT_CONS_STATE_ON_MNS;
            break;
        case EDDS_SRT_CONS_STATE_ON_3:
        case EDDS_SRT_CONS_STATE_ON_4:
            pConsFrame->State = EDDS_SRT_CONS_STATE_ON_4;
            break;
        case EDDS_SRT_CONS_STATE_OFF_1:
            pConsFrame->State = EDDS_SRT_CONS_STATE_OFF_MISS;
            break;
        case EDDS_SRT_CONS_STATE_OFF_MISS:
        case EDDS_SRT_CONS_STATE_OFF_MNS:
        case EDDS_SRT_CONS_STATE_ON_AGAIN:
        case EDDS_SRT_CONS_STATE_ON_MNS:
            /* do nothing, event will be sent anyway */
            break;
        default:
            EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,
                    "EDDS_RTConsumerControlLow: SetToUnknown in illegal state %d, ConsID: %d",
                    pConsFrame->State, ConsID);
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
            EDDS_MODULE_ID,
            __LINE__);
            break;
        }

        EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerControlLow: ConsID: %d -->state %d", ConsID,
                pConsFrame->State);

        /* protected area ends */
        EDDS_EXIT(pDDB->hSysDev);
    }

    /* set Provider Autostop after activting consumer because Provider handling */
    /* can interrupt this and should not be enabled before consumer is active   */

    if (Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE) {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "EDDS_RTConsumerControlLow: ProvAutoStop Enable. ConsID: %d.",
                ConsID);

        pCons->ProviderAutoStopEnabled = LSA_TRUE;
    }

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTConsumerControl                      +*/
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
/*+               pParam: EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE                +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE                                     +*/
/*+                                                                         +*/
/*+  ConsumerID :        Valid ConsumerID                                   +*/
/*+  Activate   :        LSA_TRUE : activate provider                       +*/
/*+                      LSA_FALSE: deaktivate provider                     +*/
/*+  SetToUnknownState:  only used if Activate = LSA_TRUE                   +*/
/*+                      LSA_TRUE : set the actual consumer state to UNKNOWN+*/
/*+                                 for the MISS/AGAIN-indication.          +*/
/*+                                 So the next consumer-check-cycle will   +*/
/*+                                 cause an AGAIN or MISS indication,      +*/
/*+                                 depending on frame received or net after+*/
/*+                                 setting the state to UNKNOWN.           +*/
/*+                      LSA_FALSE: do nothing with the actual state        +*/
/*+                                                                         +*/
/*+                      Note: Does only effect the MISS/AGAIN indication!  +*/
/*+                                                                         +*/
/*+  Note: The default-state after activating a not active consumer with    +*/
/*+        SetToUnknownState = FALSE is the MISS-state.                     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control a consumer.                            +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTConsumerControl(
        EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRT;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTConsumerControl(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
            pRQB, pDDB, pHDB);

    Status = EDD_STS_OK;

    pRqbParam = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE) pRQB->pParam;
    pCSRT = pDDB->pSRT;

    if ((pRqbParam->ConsumerID >= pCSRT->Cons.ConsumerCnt)
            || ( EDDS_CSRT_CONSUMER_TYPE_NOT_USED
                    == pCSRT->Cons.pConsTbl[pRqbParam->ConsumerID].Type)) {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "EDDS_RTConsumerControl: Invalid Consumer ID (0x%X)",
                pRqbParam->ConsumerID);

        Status = EDD_STS_ERR_PARAM;
    } else {
        EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
        EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
        LSA_UINT16 CalculatedCrNumber;
        pCons = &pCSRT->Cons.pConsTbl[pRqbParam->ConsumerID];
        pConsFrame = &pCSRT->Cons.pConsFrameTbl[pRqbParam->ConsumerID];
        CalculatedCrNumber = EDD_CRNUMBER_NOT_USED;

        /* ------------------------------------------------------------------*/
        /* Activate/Deactivate check                                         */
        /* ------------------------------------------------------------------*/
        if (((pRqbParam->Mode & EDD_CONS_CONTROL_MODE_ACTIVATE)
                && (pRqbParam->Mode & EDD_CONS_CONTROL_MODE_PASSIVATE))
                || ((pRqbParam->Mode & EDD_CONS_CONTROL_MODE_PASSIVATE)
                        && (pRqbParam->Mode
                                & EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN))

                ) {
            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTConsumerControl: Invalid Activate/Deactivate Mode: (0x%X))!",
                    pRqbParam->Mode);

            Status = EDD_STS_ERR_PARAM;
        } else if (pRqbParam->Mode & EDD_CONS_CONTROL_MODE_ACTIVATE) {

            if(!pDDB->pGlob->IO_Configured)
            {
                EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        ">>> Request: EDD_SRV_SRT_CONSUMER_CONTROL ACTIVATE invalid with IO_Configured=NO");
                Status = EDD_STS_ERR_SERVICE;
            }
            else
            {

                LSA_BOOL doInsert = LSA_FALSE;
                EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr = LSA_NULL;

                if (   EDD_CRNUMBER_NOT_USED == pRqbParam->IOParams.CRNumber
                    || (   (pCSRT->Cons.ConsumerCnt < pRqbParam->IOParams.CRNumber)
                        && (EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber) ) )
                {
                    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTConsumerControl: Invalid cr number!");

                    Status = EDD_STS_ERR_PARAM;
                }

                if( EDD_STS_OK == Status )
                {
                    LSA_UINT16 CrTableIterator;

                    if( EDD_CRNUMBER_UNDEFINED != pRqbParam->IOParams.CRNumber )
                    {
                        // check that each crnumber is used max one time
                        for( CrTableIterator = 0 ; CrTableIterator < pCSRT->Cons.ConsumerCnt ; CrTableIterator++ )
                        {
                            //check crnumber is already in use
                            if(    (pRqbParam->IOParams.CRNumber == pCSRT->Cons.pConsTbl[CrTableIterator].CrNumber)
                                && ( EDD_CRNUMBER_NOT_USED !=  pCSRT->Cons.pConsTbl[CrTableIterator].CrNumber)
                                && ( pRqbParam->ConsumerID != CrTableIterator))
                            {
                                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTConsumerControl: crnumber is already in use!");

                                Status = EDD_STS_ERR_PARAM;
                                break;
                            }

                            // check that crnumber cant not be changed
                            if(   (0 != pCSRT->Cons.pConsTbl[CrTableIterator].CrNumber)
                               && (pRqbParam->ConsumerID == CrTableIterator)
                               && (pCSRT->Cons.pConsTbl[CrTableIterator].CrNumber != pRqbParam->IOParams.CRNumber))
                            {
                                EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTConsumerControl: crnumber can only set one time!");

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
                        if(EDD_CRNUMBER_NOT_USED == pCSRT->Cons.pConsTbl[pRqbParam->ConsumerID].CrNumber)
                        {
                            EDDS_CRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTConsumerControl: crnumber was not set at EDDS_RT_ConsumerADD() and is here EDD_CRNUMBER_UNDEFINED!");

                            Status = EDD_STS_ERR_PARAM;
                        }
                        else
                        {
                            //CrNumber was already set in EDDS_RTProviderADD()
                            CalculatedCrNumber = pCSRT->Cons.pConsTbl[pRqbParam->ConsumerID].CrNumber;
                        }
                    }
                }

                if( EDD_CRNUMBER_NOT_USED == CalculatedCrNumber)
                {
                    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: CalculatedCrNumber was not calculated correctly! pRqbParam->ProviderID: 0x%X",
                                      pRqbParam->ConsumerID);

                    Status = EDD_STS_ERR_PARAM;
                }

                if( EDD_STS_OK == Status )
                {
                    LSA_UINT16 DataLen;

                    pCr = &pCSRT->Cons.pConsCrTbl[CalculatedCrNumber - 1]; // 1 .. ConsumerCnt

                    DataLen = LE_TO_H_S(pCr->DataLen);  // ensure correct endianess!

                    if (DataLen                             != pCons->DataLen   || // data length of io data must match
                            pCr->Type                       != pCons->Type      || // matching consumer type --> from allocate at IOBuffer
                            EDDS_CSRT_CONSUMER_CR_UNUSED    == pCr->CrUsed)        // Cr must be allocated previously
                    {
                        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTConsumerControl: Invalid input parameter!");

                        Status = EDD_STS_ERR_PARAM;
                    }
                }

                if (Status == EDD_STS_OK) {
                    // enter CrNumber
                    pCons->CrNumber = CalculatedCrNumber;

    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    if( (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK) == EDDS_CSRT_CONSUMER_TYPE_UDP )
                    {
                        pConsFrame->StampAddr.udp.SrcIP = pRqbParam->SrcIP;
                    } else
    #endif
                    {
                        pConsFrame->StampAddr.xrt.SrcMAC = pRqbParam->SrcMAC;
                    }

                    /* check CycleReductionRatio */
                    if (/*(EDD_STS_OK == Status)
                            &&*/ (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED
                                    == pCons->CycleReductionRatio)) {
                        LSA_BOOL IsUdp = (pCons->Type & EDDS_CSRT_CONSUMER_TYPE_XRT)
                                != EDDS_CSRT_CONSUMER_TYPE_XRT;

                        /* CycleReductionRatio still undefined on activation */
                        if (0
                                == EDDS_RTGetAndCheckReductionFactor(
                                        pRqbParam->CycleReductionRatio, IsUdp)) {
                            Status = EDD_STS_ERR_PARAM;
                        } else {
                            pCons->CycleReductionRatio =
                                    pRqbParam->CycleReductionRatio;

                            /* Number of Provider Cycle Counts for Consumer check */

                            /*@note if CycleReductionRatio was EDD_CYCLE_REDUCTION_RATIO_UNDEFINED on
                             * EDDS_RTConsumerAdd, pCons->WatchdogCycleCnt holds temporary the
                             * pRqbParam->WatchdogFactor
                             */
                            pCons->WatchdogCycleCnt *=
                                    pRqbParam->CycleReductionRatio;
                            doInsert = LSA_TRUE;
                        }
                    }

                    /* check CyclePhase */
                    if ((EDD_STS_OK == Status)
                            && (EDD_CYCLE_PHASE_UNDEFINED == pCons->CyclePhase)) {
                        LSA_UINT16 CyclePhase;
                        CyclePhase = pRqbParam->CyclePhase;

                        /* CyclePhase still undefined on activation or out of boundary*/
                        if ((EDD_CYCLE_PHASE_UNDEFINED == CyclePhase)
                                || (CyclePhase > pCons->CycleReductionRatio)) {
                            Status = EDD_STS_ERR_PARAM;
                        } else {
                            pCons->CyclePhase = CyclePhase;
                            doInsert = LSA_TRUE;
                        }
                    }

                    if ((EDD_STS_OK == Status) && doInsert) {
                        Status = EDDS_RTConsumerInsert(pCSRT,
                                pRqbParam->ConsumerID);
                    }
                }
            }
        }

        /* ------------------------------------------------------------------*/
        /* Provider AUTOSTOP                                                 */
        /* ------------------------------------------------------------------*/
        if ((pRqbParam->Mode & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE)
                && (pRqbParam->Mode
                        & EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE)) {
            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTConsumerControl: Invalid Provider Autostop Mode: (0x%X))!",
                    pRqbParam->Mode);

            Status = EDD_STS_ERR_PARAM;
        }

        if (Status == EDD_STS_OK) {
            EDDS_RTConsumerControlLow(pDDB, pCSRT, pRqbParam->ConsumerID,
                    pRqbParam->Mode);
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTConsumerControl(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}

/*****************************************************************************/
/*  end of file SRT_CONS.C                                                   */
/*****************************************************************************/

