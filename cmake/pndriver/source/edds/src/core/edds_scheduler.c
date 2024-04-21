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
/*  F i l e               &F: edds_scheduler.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS Event Scheduler                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who      What                                                */
#ifdef EDDS_MESSAGE
/*  2014-08-13  am2219   initial version.                                    */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID 12
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_ISR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#include "edds_int.h"            /* internal header */

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DropDisabledMCFrames                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE  pDDB               +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE  pFrame           +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                  : reference to device description block          +*/
/*+  pFrame                : Framedata                                      +*/
/*+                                                                         +*/
/*+ Return Value:          LSA_TRUE:  drop MC Frame                         +*/
/*+                        LSA_FALSE: pass to User, do not filter           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function filters the MC frames based on              +*/
/*+               enabled mc mac addresses.                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_BOOL EDDS_LOCAL_FCT_ATTR
EDDS_DropDisabledMCFrames (EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
const EDD_UPPER_MEM_U8_PTR_TYPE const pFrame)
{
    LSA_BOOL dropFrame = LSA_FALSE;

    /* only filtering mc mac addresses (broadcast is not multicast) */
    if( (EDDS_IS_MC_MAC(pFrame)) &&
    (!(EDDS_IS_BC_MAC(pFrame))) )
    {
        /* sw filtering is enabled? check frames  */
        if(EDDS_McSWFilter_IsEnabled(pDDB))
        {
            if(EDDS_McSWFilter_IsActive(pDDB))
            {
                /* not enabled mc mac addresses are dropped */
                if(!EDDS_McSWFilter_IsEnabledMcMac(pDDB,pFrame))
                {
                    dropFrame = LSA_TRUE;
                }
                else
                {
                    dropFrame = LSA_FALSE;
                }
            }
            else
            {
                /* drop all frame if sw filter is enabled but not active */
                dropFrame = LSA_TRUE;
            }
        }
        /* sw filtering not enabled ? default behavior: handle all frames */
    }

    return dropFrame;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DropWithEqualSAandIFAdress             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE  pDDB               +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE  pFrame           +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                  : reference to device description block          +*/
/*+  pFrame                : Framedata                                      +*/
/*+                                                                         +*/
/*+ Return Value:          LSA_TRUE:  drop Frame                            +*/
/*+                        LSA_FALSE: pass to User, do not filter           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function filters the frames which has the            +*/
/*+               same address as SA as the receiving interface.            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_BOOL EDDS_LOCAL_FCT_ATTR
EDDS_DropWithEqualSAandIFAdress (EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
const EDD_UPPER_MEM_U8_PTR_TYPE const pFrame)
{
    LSA_BOOL dropFrame = LSA_FALSE;

    if ((pFrame[6] == pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr[0]) &&
    (pFrame[7] == pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr[1]) &&
    (pFrame[8] == pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr[2]) &&
    (pFrame[9] == pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr[3]) &&
    (pFrame[10] == pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr[4]) &&
    (pFrame[11] == pDDB->pGlob->HWParams.hardwareParams.MACAddress.MacAdr[5]) )
    {
        dropFrame = LSA_TRUE;
    }

    return dropFrame;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DropUnusedUDPFrames                    +*/
/*+  Input/Output          :    LSA_UINT32  MACTyp                          +*/
/*+                             LSA_BOOL  RxFilterUDP_Broadcast             +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE  pFrame           +*/
/*+                             LSA_UINT32  UserDataOffset                  +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  MACTyp                : MAC Typ                                        +*/
/*+  pFrame                : Framedata                                      +*/
/*+  UserDataOffset        : Offset to IP Header                            +*/
/*+                                                                         +*/
/*+ Return Value:          LSA_TRUE:  filter UDP Frame                      +*/
/*+                        LSA_FALSE: pass to User, do not filter           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function filters the UDP frames based on UDP         +*/
/*+               Whitelist.                                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//JB 11/11/2014 pFrame can not be a null ptr - pDDB is checked within EDDS_GetDDB
//WARNING: be careful when calling this function, make sure not to use any null ptr!
static LSA_BOOL EDDS_LOCAL_FCT_ATTR
EDDS_DropUnusedUDPFrames (EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
EDD_UPPER_MEM_U8_PTR_TYPE const pFrame,
LSA_UINT32 const UserDataOffset )
{
    LSA_BOOL dropFrame;

    EDDS_IP_HEADER_TYPE * pIPHeader;
    EDDS_UDP_HEADER_TYPE * pUDPHeader;
    LSA_UINT32 DstPort;
    LSA_UINT8 IHL;

    pIPHeader = (EDDS_IP_HEADER_TYPE *)(void *)&pFrame[UserDataOffset];

    EDDS_SCHED_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_DropUnusedUDPFrames->");

    IHL = (LSA_UINT8)((pIPHeader->VersionIHL) & 0xF);

    /* first, all frames should be dropped */
    dropFrame = LSA_TRUE;

    if (IHL < 0x5) //IP Header Length is min. 5
    {
        //Drop Frame, because wrong IP Header Length
    }
    else
    {
        // check for fragmentation
        if ((EDDS_NTOHS(pIPHeader->FragmentOffset) & EDDS_IP_OFFSET) > 0)
        {
            dropFrame = LSA_FALSE;
        }
        else
        {
            pUDPHeader = (EDDS_UDP_HEADER_TYPE *)(void *)&pFrame[UserDataOffset + (sizeof(LSA_UINT32) * IHL)];
            DstPort = (LSA_UINT32)EDDS_NTOHS(pUDPHeader->DstPort);

            if (!EDDS_IS_MC_MAC(pFrame) && (pDDB->pNRT->RxFilterUDP_Unicast)) /* Unicast Frame & should be filtered */
            {
                //Check UDP Whitelist's
                if ( (EDDS_UDP_FILTER_DST_PORT_DHCP_CLIENT == DstPort)
                || (EDDS_UDP_FILTER_DST_PORT_NTP_SNTP == DstPort)
                || (EDDS_UDP_FILTER_DST_PORT_SNMP == DstPort)
                || (EDDS_UDP_FILTER_DST_PORT_SNMPTRAP == DstPort)
                || (EDDS_UDP_FILTER_DST_PORT_PNIO_EPM == DstPort)
                || ( (DstPort >= EDDS_UDP_FILTER_DST_PORT_IANA_FREE_PORT_BEGIN)
                        && (DstPort <= EDDS_UDP_FILTER_DST_PORT_IANA_FREE_PORT_END) )
                )
                {
                    //Pass to User
                    dropFrame = LSA_FALSE;
                }
            }
            else if (EDDS_IS_BC_MAC(pFrame) && (pDDB->pNRT->RxFilterUDP_Broadcast)) /* Broadcast Frame & should be filtered */
            {
                if ((EDDS_UDP_FILTER_DST_PORT_DHCP_CLIENT == DstPort) || (EDDS_UDP_FILTER_DST_PORT_NTP_SNTP == DstPort))
                {
                    //Pass to User
                    dropFrame = LSA_FALSE;
                }
            }
            else /* Multicast */
            {
                /* no filtering on multicast */
                dropFrame = LSA_FALSE;
            }
        }
    }

    return dropFrame;
}


#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_IPChecksumAndGetHeader                 +*/
/*+  Input                      EDD_UPPER_MEM_U16_PTR_TYPE    pBuf          +*/
/*+                             LSA_UINT16                   *pIPHeader     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuf       : Pointer to start of 20 Bytes IP-Header                    +*/
/*+  pIPHeader  : Pointer to 20 Byte filled with IP-Header data (output)    +*/
/*+  Result     : Checksum                                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates IP-Header checksum.                            +*/
/*+               if result is 0 the checksum of the IP-Header is valid     +*/
/*+               Note: pBuffer points to a WORD aligned address            +*/
/*+               Note: There is a "normal" checksum function within edds   +*/
/*+                     (EDDS_IPChecksum()) used to fill a IP-Header!       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_UINT16 EDDS_IPChecksumAndGetHeader (
EDD_UPPER_MEM_U16_PTR_TYPE pBuf,
LSA_UINT16 *pIPHeader)
{
    LSA_UINT32 Sum;
    LSA_UINT32 i;

#undef EDDS_CFG_IP_CHECKSUM_OPTIMIZE
#ifdef EDDS_CFG_IP_CHECKSUM_OPTIMIZE
    LSA_UINT32 Help;
    EDD_UPPER_MEM_U32_PTR_TYPE pHelp;
#endif

    Sum = 0;

#ifdef EDDS_CFG_IP_CHECKSUM_OPTIMIZE
    /*------------------------------------------------------- */
    /* optimized checksum calc (reduced amount of framebuffer */
    /* accesses. pBuf[1] must be DWORD aligned! (is the case  */
    /* within an Ethernetframe). We assume 20 Byte IP Header  */
    /*------------------------------------------------------- */
    Sum = *pIPHeader++ = pBuf[0];

    pHelp = (EDD_UPPER_MEM_U32_PTR_TYPE) pBuf[1];

    for ( i=0; i< 4; i++ ) /* (4*4 Byte = 16 Byte) */
    {
        Help = *pHelp++;
#ifdef EDDS_CFG_BIG_ENDIAN
        Sum += *pIPHeader++ = (LSA_UINT16) (Help >> 16);
        Sum += *pIPHeader++ = (LSA_UINT16) Help;
#else
        Sum += *pIPHeader++ = (LSA_UINT16) Help;
        Sum += *pIPHeader++ = (LSA_UINT16) (Help >> 16);
#endif
    }

    Sum = *pIPHeader++ = pBuf[9];
    /*------------------------------------------------------- */
#else
    for ( i=0; i < (EDDS_IP_HEADER_SIZE/2); i++ )
    Sum += *pIPHeader++ = *pBuf++;
#endif

    /* add carrys so we get ones-complement */
    Sum = (Sum >> 16) + (Sum & 0xffff);
    Sum += (Sum >> 16);

    return((LSA_UINT16)~Sum);
}

#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleRxFrame                          +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ..    *ppRxBuffer    +*/
/*+                             LSA_UINT32                    RxLen         +*/
/*+                             LSA_UINT32                    PortID        +*/
/*+  Output                     EDD_UPPER_MEM_U8_PTR_TYPE     pRxBuffer     +*/
/*+  Result                :    ---                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+  pRxBuffer  : address of pointer to Receive Buffer (in/out)             +*/
/*+  RxLen      : Length of Frame                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for Frametype and calls handler for frame          +*/
/*+               As result the new framebuffer is returned (which may      +*/
/*+               be the same as pFrame)                                    +*/
/*+                                                                         +*/
/*+               First we check for VLAN-Tag. If so we must read the type  +*/
/*+               len field 4 bytes ahead. then wie analyse the type field  +*/
/*+               for the different framestypes wie have to filter.         +*/
/*+               For SRT we also have to check the FrameID-Field which is  +*/
/*+               located behind the Type/len-field.                        +*/
/*+                                                                         +*/
/*+               For xRToverUDP we have to check several more fields.      +*/
/*+                                                                         +*/
/*+               Note: This function have to be very speed optimized and   +*/
/*+                     should do as less as possible access to the frame   +*/
/*+                     buffer (uncached-memory!!)                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//JB 18/11/2014 checked where called (LLIF) - can not be a null pointer
//WARNING: be careful when using this function, make sure not to use ppRxBuffer as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_HandleRxFrame (EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *ppRxBuffer,
LSA_UINT32 RxLen,
LSA_UINT32 PortID)
{

    EDDS_TYPE_LEN_FRAME_ID_TYPE FrameData;
    LSA_UINT16 Dummy;
    EDD_UPPER_MEM_U8_PTR_TYPE pFrame;
    LSA_BOOL VLANTagPresent;
    LSA_BOOL DropFrame;

    /* initialize local variable */
    pFrame = (EDD_UPPER_MEM_U8_PTR_TYPE) *ppRxBuffer;

    EDDS_SCHED_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "IN :EDDS_HandleRxFrame(pDDB: 0x%X,*ppRxBuffer: 0x%X,RxLen: %d, PortID: %d)",
    pDDB,
    *ppRxBuffer,
    RxLen,
    PortID);

    DropFrame = EDDS_DropWithEqualSAandIFAdress(pDDB,pFrame);

    if (!DropFrame)
    {
        /* ---------------------------------------------------------------------------*/
        /* SW filtering of multicast                                                  */
        /* ---------------------------------------------------------------------------*/
        DropFrame = EDDS_DropDisabledMCFrames(pDDB,pFrame);
    }

    if(!DropFrame)
    {
        /* ---------------------------------------------------------------------------*/
        /* get bytes 12..15 from Frame                                                */
        /* Note that this is in inet-format (big endian)                              */
        /* ---------------------------------------------------------------------------*/

        FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)pFrame)[3]; //lint !e826 JB 11/11/2014 correct access to pFrame

        /* ---------------------------------------------------------------------------*/
        /* check for VLAN-Tag. if so we must read the real Type/len 4 Bytes ahead     */
        /* ---------------------------------------------------------------------------*/

        if ( FrameData.w.TypeLen == EDDS_VLAN_TAG )
        {
            FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)pFrame)[4]; //lint !e826 JB 11/11/2014 correct access to pFrame
            VLANTagPresent = LSA_TRUE;
        }
        else VLANTagPresent = LSA_FALSE;

        /* ---------------------------------------------------------------------------*/
        /* Now check for the Frametype                                                */
        /* Note: we check for SRT outside the switch because we want to be sure       */
        /*       we get the fastest branch to SRT-case because SRT needs more         */
        /*       filtering.                                                           */
        /* ---------------------------------------------------------------------------*/

        /* ---------------------------------------------------------------------------*/
        /* xRT-Ethernet frame                                                         */
        /* ---------------------------------------------------------------------------*/
        if ( FrameData.w.TypeLen == EDDS_xRT_TAG )
        {

            /* here a check for cyclic and acyclic SRT                     */

            /* Frame-ID ranges:                                            */
            /*                                                             */
            /* 0000...00FF:  Timesynchronisation                           */
            /* 0100...7FFF:  IRT-Frames                                    */
            /* 8000...FBFF:  cyclic SRT                                    */
            /* FC00...FCFF:  acyclic SRT high prio                         */
            /* FC01          PN-IO Alarm high                              */
            /* FD00...FDFF:  reserved                                      */
            /* FE00...FEFC:  acyclic SRT low  prio                         */
            /* FE01          PN-IO Alarm low                               */
            /* FEFE          DCP via xRT                                   */
            /* FEFF          DCP via xRT                                   */
            /* FF00...FFFF:  reserved                                      */
            /*                                                             */

            /* first we check for cyclic SRT, then for acyclic. all other  */
            /* frames will be dropped.                                     */

#ifndef EDDS_CFG_BIG_ENDIAN
            Dummy = (FrameData.b.FrameID_H << 8 ) + FrameData.b.FrameID_L;
#else
            Dummy = FrameData.w.FrameID;
#endif

            if (( Dummy >= EDD_SRT_FRAMEID_CSRT_START ) && ( Dummy <= EDD_SRT_FRAMEID_CSRT_STOP ))
            {
                if( pDDB->pGlob->IsSrtUsed )
                {
                    EDDS_RTConsumerRxIndicateCyclicXRT( pDDB,
                    ppRxBuffer,
                    RxLen,
                    Dummy,
                    VLANTagPresent);
                }
            }
            else
            {
                /*------------------------------------------------------ */
                /* a aSRT  Frame was received. We indicate the           */
                /* new frame. (the functions gives us a new              */
                /* rxbuffer which will be given to the                   */
                /* controller below.                                     */
                /* IMPORTANT: This function must be very fast            */
                /*------------------------------------------------------ */

                /* ALARM region ? */
                if ((( Dummy >= EDD_SRT_FRAMEID_ASRT_START_LOW ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_LOW )) ||
                (( Dummy >= EDD_SRT_FRAMEID_ASRT_START_HIGH ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_HIGH )))
                {
                    EDDS_NRTRxIndicate( pDDB,
                    ppRxBuffer,
                    RxLen,
                    EDDS_NRT_BUFFER_ASRT_ALARM_IDX,
                    PortID,
                    EDDS_NRT_DSCR_STATE_IN_USE_XRT);
                }
                else /* no ALARM */
                {
                    EDDS_NRTRxIndicate( pDDB,
                    ppRxBuffer,
                    RxLen,
                    EDDS_NRT_BUFFER_ASRT_OTHER_IDX,
                    PortID,
                    EDDS_NRT_DSCR_STATE_IN_USE_XRT);
                }
            }
        }
        /* ---------------------------------------------------------------------------*/
        /* no xRT-Ethernet frame                                                      */
        /* ---------------------------------------------------------------------------*/
        else
        {
            switch (FrameData.w.TypeLen)
            {
                case EDDS_IP_TAG: /* IP */
                /*------------------------------------------------------ */
                /* Drop unused UDP Frames                                */
                /*------------------------------------------------------ */
                if ( (pDDB->pNRT->RxFilterUDP_Broadcast)
                || (pDDB->pNRT->RxFilterUDP_Unicast)
                )
                {
                    LSA_UINT32 DLen;
                    EDDS_IP_HEADER_TYPE * pIPHeader;

                    (VLANTagPresent)?(DLen = EDDS_FRAME_HEADER_SIZE_WITH_VLAN):(DLen = EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN);

                    pIPHeader = (EDDS_IP_HEADER_TYPE *) &pFrame[DLen]; //lint !e826 JB 11/11/2014 correct access to pFrame

                    if (EDDS_IP_UDP == pIPHeader->Protocol)
                    {
                        if (EDDS_DropUnusedUDPFrames(pDDB,pFrame, DLen))
                        {
                            /* UDP frame should be dropped */
                            break;
                        }
                        else
                        {
                            /* do nothing, frame should be processed */
                        }
                    }
                }
                /* no break, fall through */

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
                /*------------------------------------------------------ */
                /* check for xRT over UDP-Frames.                        */
                /*------------------------------------------------------ */
                EDDS_UDP_XRT_FRAME_PTR_TYPE pHeader;
                LSA_UINT32 DLen;
                EDDS_IP_HEADER_TYPE IPHeader; /* IPHeader cache */

                if ( VLANTagPresent )
                {
                    pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pFrame[EDDS_FRAME_HEADER_SIZE_WITH_VLAN];
                    DLen = RxLen-EDDS_FRAME_HEADER_SIZE_WITH_VLAN;
                }
                else
                {
                    pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pFrame[EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN];
                    DLen = RxLen-EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN;
                }

                /* check if this is a xRToverUDP - IP/UDP-Header */

                if (( FrameData.bIP.IPVersIHL == EDDS_IP_XRT_VER_IHL ) &&
                ( pHeader->Ip.Protocol == EDDS_IP_UDP ) &&
                ( pHeader->Udp.DstPort == EDDS_xRT_UDP_PORT ))
                {

                    /* check the IP-Checksum and copy header to IPHeader */
                    if ( EDDS_IPChecksumAndGetHeader((LSA_UINT16 *) pHeader,(LSA_UINT16 *)&IPHeader) == 0 )
                    {

                        LSA_UINT32 IPLen;
                        LSA_UINT32 UDPLen;

                        IPLen = (LSA_UINT32) EDDS_NTOHS(IPHeader.TotalLength);
                        UDPLen = (LSA_UINT32) EDDS_NTOHS(pHeader->Udp.Length);

                        /* verify some more header settings..        */
                        /* Note: We dont check the UDP-Checksum      */
                        /*       must be 0, else discarded!          */
                        /* check: FragmentOffset (== 0)              */
                        /* check: UDP-Src Port                       */
                        /* check: Local IP Address (== DstIP) or MC  */
                        /* check: No UDP-Checksum (==0)              */
                        /* check: Verify length consistenz           */

                        if (((IPHeader.FragmentOffset & EDDS_IP_FLAG_OFFSET_MSK) == EDDS_IP_FLAG_OFFSET_VAL )
                        && ( pHeader->Udp.SrcPort == EDDS_xRT_UDP_PORT )
                        &&
                        (
                                ( IPHeader.DestIP.dw == pDDB->pGlob->HWParams.IPAddress.dw ) ||
                                ( (IPHeader.DestIP.b.IP[0] & 0xF0) == 0xE0 ) /* IP-Multicast 224..239 */
                        )
                        && ( IPLen <= DLen )
                        && ( IPLen >= EDDS_UDP_XRT_HEADER_SIZE )
                        && ( UDPLen == IPLen-EDDS_IP_HEADER_SIZE ))
                        {
                            /* we have a valid xRToverUDP-Frame */
                            Dummy = EDDS_NTOHS(pHeader->FrameID); /* get FrameID */

                            if (( Dummy >= EDD_SRT_FRAMEID_CSRT_START ) && ( Dummy <= EDD_SRT_FRAMEID_CSRT_STOP ))
                            {
                                if(pDDB->pGlob->IsSrtUsed )
                                {
                                    EDDS_RTConsumerRxIndicateCyclicUDP( pDDB,
                                    ppRxBuffer,
                                    &pHeader->FrameID, /* pointer to UDP-Data start */
                                    &IPHeader,
                                    UDPLen-EDDS_UDP_HEADER_SIZE,
                                    Dummy);/* FrameID */
                                }
                            }
                            else
                            {
                                /* we have a asynchroneous xRToverUDP frame.      */

                                /* ALARM region ? */
                                if ((( Dummy >= EDD_SRT_FRAMEID_ASRT_START_LOW ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_LOW )) ||
                                (( Dummy >= EDD_SRT_FRAMEID_ASRT_START_HIGH ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_HIGH )))
                                {
                                    EDDS_NRTRxIndicate(
                                    pDDB,
                                    ppRxBuffer,
                                    RxLen,
                                    EDDS_NRT_BUFFER_ASRT_ALARM_IDX,
                                    PortID,
                                    EDDS_NRT_DSCR_STATE_IN_USE_UDP);
                                }
                                else /* no ALARM */
                                {

                                    EDDS_NRTRxIndicate(
                                    pDDB,
                                    ppRxBuffer,
                                    RxLen,
                                    EDDS_NRT_BUFFER_ASRT_OTHER_IDX,
                                    PortID,
                                    EDDS_NRT_DSCR_STATE_IN_USE_UDP);
                                }
                            }
                            break; /* if it is xrt over UDP we break */
                        }
                        else
                        {

                            EDDS_SCHED_TRACE__00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP, "EDDS_HandleRxFrame(): xRToverUDP invalid IP header or wrong IP");

                            pDDB->pGlob->RxStats.RxXRToUDPWrongHeader++;
                            break; /* invalid Header or IP setting of xRToUDP-Frame. discard */
                        }
                    }
                    else
                    {
                        EDDS_SCHED_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP, "EDDS_HandleRxFrame(): xRToverUDP frame with wrong IP checksum");

                        pDDB->pGlob->RxStats.RxXRToUDPIPWrongChecksum++;
                        break; /* invalid checksum. discard */
                    }

                } /* not a xRToUDP Frame. let it handle by standard IP */
#endif

                /* no break, fall through */
                //lint -fallthrough
                //JB 12/11/2014 on purpose, see comment above
                case EDDS_ARP_TAG: /* ARP          */
                case EDDS_RARP_TAG: /* reversed ARP */

                /*------------------------------------------------------ */
                /* a NRT-ARPIP Frame was received. We indicate the       */
                /* new frame. (the functions gives us a new              */
                /* rxbuffer which will be given to the                   */
                /* controller below.                                     */
                /*------------------------------------------------------ */

                EDDS_NRTRxIndicate( pDDB,
                ppRxBuffer,
                RxLen,
                EDDS_NRT_BUFFER_IP_IDX,
                PortID,
                EDDS_NRT_DSCR_STATE_IN_USE);
                break;

                default:

                /* -------------------------------------------------------------*/
                /* convert type/len to host-format                              */
                /* -------------------------------------------------------------*/

#ifndef EDDS_CFG_BIG_ENDIAN
                Dummy = (FrameData.b.TypeLen_H << 8 ) + FrameData.b.TypeLen_L;
#else
                Dummy = FrameData.w.TypeLen;
#endif

                if ( Dummy <= 1500 )
                {

                    /*------------------------------------------------------ */
                    /* a NRT-LEN Frame was received. We indicate the         */
                    /* new frame. (the functions gives us a new              */
                    /* rxbuffer which will be given to the                   */
                    /* controller below.                                     */
                    /*------------------------------------------------------ */

                    EDDS_NRTRxIndicate( pDDB,
                    ppRxBuffer,
                    RxLen,
                    EDDS_NRT_BUFFER_OTHER_IDX,
                    PortID,
                    EDDS_NRT_DSCR_STATE_IN_USE_OTHER_LEN);
                }
                else /* NRT-Other*/
                {

                    /*------------------------------------------------------ */
                    /* a NRT-OTHER Frame was received. We indicate the       */
                    /* new frame. (the functions gives us a new              */
                    /* rxbuffer which will be given to the                   */
                    /* controller below.                                     */
                    /*------------------------------------------------------ */

                    EDDS_NRTRxIndicate( pDDB,
                    ppRxBuffer,
                    RxLen,
                    EDDS_NRT_BUFFER_OTHER_IDX,
                    PortID,
                    EDDS_NRT_DSCR_STATE_IN_USE_OTHER_TYPE);
                }
                break; /* default */

            } /* switch */
        } /* else */
    }

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_HandleRxFrame(*ppRxBuffer: 0x%X)", *ppRxBuffer);

}

//lint -esym(550,framesFinished)

/** \brief      EDDS_TransmitLastCycleFinish.
 *
 *  \details    This function checks for sent frames. If there a are no more frames this function returns.
 *              If a frame was sent the corresponding send RQB (from send prio queue) is determined and then acknowledged.
 *
 *  \param      pDDB            Pointer to Device Description Block.
 *
 *  \author     am2219
 *  \date       2015-02-24
 *
 */
static LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_TransmitLastCycleFinish (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_UINT framesFinished = 0;

    /* always stop timeout - timeout is not used anymore! */
    while ( EDD_STS_OK == EDDS_TransmitSts (pDDB))
    {
        ++pDDB->pGlob->actTxPacketsFinishCycle;  /* for internal statistics: count processed finished send frame for actual cycle */
        ++framesFinished;                       /* for local statistics: count processed finished send frame for for actual scheduler call */
    }
}
//lint +esym(550,framesFinished)


/** \brief      EDDS_ReceiveLastCycleFinish.
 *
 *  \details    This functions checks if a frame was received. If there a are no more frames or the reception bandwith is exceeded this function returns.
 *              If a frame was received the frame type is determined. The frame buffer is then exchanged with one buffer from a corresponding pre queue.
 *              A frame may be dropped if there is no free pre queue entry. The exchanged buffer is than reprovided to the LL as new receive buffer.\n\n
 *
 *  \param      pDDB            Pointer to Device Description Block.
 *
 *  \note       The receive bandwith depends on the current link speed. It is determined on every link change.
 *
 *  \author     am2219
 *  \date       2015-02-24
 */
static LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_ReceiveLastCycleFinish (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_RESULT Result;
    EDD_UPPER_MEM_PTR_TYPE pRxBuffer;
    LSA_UINT32 RxLen;
    LSA_UINT32 PortID;
    LSA_UINT32 actRxPackets;

    /*-------------------------------------------------------------------*/
    /* check for received frames                                         */
    /*-------------------------------------------------------------------*/

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE, "==> EDDS_LL_RECV(pDDB: 0x%X)",
                        pDDB);

    actRxPackets = 0; /* count received packets per edds scheduler call */

    Result = EDD_STS_OK;

    /* if receive packets quota is not reached,
     * another (maximum) frame could be received/processed in this cycle,
     * and another frame is available,
     * process/handle this received frame.
     */
    while (((!pDDB->pGlob->RxPacketsQuota)
            || (pDDB->pGlob->RxPacketsQuota
                    > actRxPackets))
            && ((EDDS_ETHERNET_FRAME_LENGTH_MAX + EDDS_ETHERNET_FRAME_IPG_LENGTH)
                    <= pDDB->pGlob->RxRemainingByteCntPerCycle)
            && (EDD_STS_OK_NO_DATA
                    != (Result = EDDS_LL_RECV(pDDB, &pRxBuffer, &RxLen, &PortID))))
    {
        EDDS_LOWER_TRACE_04(
                0,
                LSA_TRACE_LEVEL_NOTE,
                "<== EDDS_LL_RECV(pRXBuffer: 0x%X, RxLen: 0x%X, PortID: %d), Result: 0x%X",
                pRxBuffer, RxLen, PortID, Result);

        /* note: it's possible, that one frame will be processed, that is from "next" cycle.
         *       the theoretically maximum of more bytes processed in cycle is : MaxFrameSize - (MinimumFrameSize -1)
         *       ==> 100MBit: 1457 Byte (With 1 VLAN Tag: 1415 byte data; w/o VLAN tag 1419 byte data).
         *
         */
        pDDB->pGlob->RxRemainingByteCntPerCycle -=
                (EDDS_ETHERNET_FRAME_HEAD_LENGTH +
                EDDS_ETHERNET_FRAME_FCS_LENGTH +
                EDDS_ETHERNET_FRAME_IPG_LENGTH); /* preamble + SFD + FCS + IPG */
        pDDB->pGlob->RxRemainingByteCntPerCycle -= RxLen; /* frame length from EDDS_LL_RECV */

        ++pDDB->pGlob->actRxPacketsCycle; /* for internal statistics: count processed received frame for actual cycle */
        ++actRxPackets;                   /* for local statistics: count processed received frames for actual scheduler call */

        /*---------------------------------------------------------------*/
        /* if data was present we only continue if the data is           */
        /* errorfree. data with error will be ignored.                   */
        /*---------------------------------------------------------------*/

        if (Result == EDD_STS_OK)
        {
            EDDS_HandleRxFrame (pDDB, &pRxBuffer, RxLen, PortID);

        } /* if (Result == EDD_STS_OK)..*/

        /*-------------------------------------------------------------- */
        /* provide a new buffer to the controller                        */
        /*-------------------------------------------------------------- */

        EDDS_LOWER_TRACE_02(
                pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                "==> EDDS_LL_RECV_PROVIDE(pDDB: 0x%X,pRXBuffer: 0x%X)", pDDB,
                pRxBuffer);

        Result = EDDS_LL_RECV_PROVIDE(pDDB,pRxBuffer);

        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                            "<== EDDS_LL_RECV_PROVIDE(), Result: Result: 0x%X",Result);

        if (EDD_STS_OK != Result)
        {
            EDDS_FatalError ( EDDS_FATAL_ERR_ISR,
            EDDS_MODULE_ID,
                             __LINE__);
        }

        EDDS_LOWER_TRACE_03(
                pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                "==> EDDS_LL_RECV(pDDB: 0x%X,&pRXBuffer: 0x%X, &RxLen: 0x%X)",
                pDDB, &pRxBuffer, &RxLen);
    }

    /* there are some frames left; scheduler should get a trigger again... */
    if(EDD_STS_OK_NO_DATA != Result)
    {
        /*...but only, if bandwidth limit not exceeded */
        if(((EDDS_ETHERNET_FRAME_LENGTH_MAX + EDDS_ETHERNET_FRAME_IPG_LENGTH)
                    <= pDDB->pGlob->RxRemainingByteCntPerCycle))
        {
            pDDB->pGlob->triggerSchedulerPending = LSA_TRUE;
        }
    }

    /* only re-trigger hw if necessary (reduces register accesses) */
    if(actRxPackets)
    {
        EDDS_LL_RECV_TRIGGER(pDDB);
    }

    EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<== EDDS_LL_RECV()");
}


/** \brief      Handle PRM Diag Timer.
 *
 *  \details    This function is called after PRM Diag-Timer elapsed. It reads the MIB statistics for the
 *              interface port.
 *
 *  \author     am2219
 *  \date       2015-08-27
 */
LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_PRMDiagCycleCBF (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_RESULT Status;

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "IN :EDDS_LEDBlinkCycleRequest(pDDB: 0x%X)", pDDB);

    Status = EDDS_LL_GET_STATS(pDDB, 0, &pDDB->pGlob->DropStats_Recent); /* @todo jb_check IF port only ? */

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                        "<== EDDS_LL_GET_STATS(). Status: 0x%X", Status);

    if (Status == EDD_STS_OK)
    {
        /* Note: Overflow will not be handled!                                 */
        pDDB->pGlob->DropStats.InDiscards =
                pDDB->pGlob->DropStats_Recent.InDiscards
                        - pDDB->pGlob->DropStats.InDiscards;
        pDDB->pGlob->DropStats.OutDiscards =
                pDDB->pGlob->DropStats_Recent.OutDiscards
                        - pDDB->pGlob->DropStats.OutDiscards;
    }
    pDDB->pGlob->SchedulerEventsDonePending |=
            EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__PRM_DIAG_CYCLE;
}


/** \brief      Show Location LED timer callback function..
 *
 *  \details    This function is called after a LED cyclic timer-request
 *              occurred.
 *
 *  \author     am2219
 *  \date       2015-08-06
 */
static LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_LEDBlinkCycleCBF (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{

    EDDS_LED_BLINK_PTR_TYPE pLED;
    LSA_BOOL LEDOn;
    //LSA_BOOL                                    SetLEDMode;
    LSA_BOOL  Restore;

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "IN :EDDS_LEDBlinkCycleRequest(pDDB: 0x%X)", pDDB);

    pLED = &pDDB->pGlob->LEDBLink;
    LEDOn = LSA_FALSE;

    if ((pLED->Status == EDDS_LED_BLINK_ACTIVE))
    {
        //SetLEDMode = LSA_FALSE;
        Restore = LSA_FALSE;

        if (pLED->RemainTotal)
        {
            --pLED->RemainTotal;

            /* toggle LED */

            if (pLED->LEDOn)
            {
                pLED->LEDOn = LSA_FALSE;
                LEDOn = LSA_FALSE;
            }
            else
            {
                pLED->LEDOn = LSA_TRUE;
                LEDOn = LSA_TRUE;
            }

            //SetLEDMode = LSA_TRUE;
        }
        else /* we stop if RemainTotal is 0 */
        {

            pLED->Status = EDDS_LED_BLINK_INACTIVE;
            //SetLEDMode   = LSA_TRUE;
            Restore = LSA_TRUE;

            EDDS_StopTimer (pDDB, EDDS_TIMER_LED_BLINK);
        }
        /* Set LEDs on all ports */
        //if ( SetLEDMode )
        {

            if (Restore)
            {
                EDDS_LL_LED_RESTORE_MODE(pDDB);
            }
            else
            {
                EDDS_LL_LED_SET_MODE(pDDB, LEDOn);
            }
        }
    }
    else /* pLED->Status == EDDS_LED_BLINK_INACTIVE */
    {
        /* we may modify this, if no BLINK is active */
        pLED->Status = EDDS_LED_BLINK_ACTIVE;
        pLED->RemainTotal = (EDDS_TOTAL_BLINK_DURATION_S * 2) - 1; /* in 500ms ticks */
        pLED->LEDOn = LSA_TRUE; /* start with LED on */

        /* turn on "show location" LED(s) */

        {
            LEDOn = LSA_TRUE;

            EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "==> EDDS_LL_LED_BACKUP_MODE(pDDB: 0x%X)",
                                pDDB);

            EDDS_LL_LED_BACKUP_MODE(pDDB);

            EDDS_SCHED_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_LED_BACKUP_MODE()");

            EDDS_SCHED_TRACE_02(
                    pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                    "==> EDDS_LL_LED_SET_MODE(pDDB: 0x%X, LED = %d)", pDDB,
                    LEDOn);

            EDDS_LL_LED_SET_MODE(pDDB, LEDOn);

            EDDS_SCHED_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_LED_SET_MODE()");

        }
    }

    EDDS_SCHED_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "OUT:EDDS_LEDBlinkCycleRequest()");

}


/** \brief        EDDS_LEDShowLocation.
 *
 *  \details      We will start or retrigger LED Blink.
 *                if we already running, we only set the new time.
 *
 *  \author       am2219
 *  \date         2015-08-06
 */
static LSA_VOID
EDDS_LEDShowLocation (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    EDDS_LED_BLINK_PTR_TYPE pLED;

    pLED = &pDDB->pGlob->LEDBLink;

    if (pLED->Status == EDDS_LED_BLINK_INACTIVE)
    {

        /*---------------------------------------------------------------*/
        /* start LED Blink Timer                                         */
        /*---------------------------------------------------------------*/

        EDDS_StartTimer (
                pDDB, EDDS_TIMER_LED_BLINK,
                pDDB->pGlob->scheduler.currentCycleBeginNS
                + ((LSA_UINT64)pDDB->pGlob->scheduler.CycleTimeMS) * 1000000, // next cyclic call ( in nanoseconds )
                500 * 1000000,               // 500 ms ( 1.000.000 ns = 1ms)
                EDDS_LEDBlinkCycleCBF);
    }
    else /* active */
    {
        pLED->RemainTotal = (EDDS_TOTAL_BLINK_DURATION_S * 2) - 1;/* in 500ms ticks */
    }
}


/** \brief      EDDS_HandleHighPriorRequests
 *
 *  \details    proccess RQBs that have a high priority
 *              and need to be finished asap.
 *
 *  \param      pDDB                Pointer to DeviceDescriptionBlock
 *
 * \note        called from scheduler context.
 *
 * \author      th
 * \date        2015-09-xx
 */
static LSA_VOID
EDDS_HandleHighPriorRequests (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    EDD_UPPER_RQB_PTR_TYPE pRQB;
    LSA_RESULT Status;
    EDDS_RQB_LIST_TYPE_PTR refHighPriorGeneralRequestQueue;
    EDDS_RQB_LIST_TYPE_PTR refFinishedGeneralRequestQueue;

    Status = EDD_STS_OK;
    refHighPriorGeneralRequestQueue =
            &pDDB->pGlob->HighPriorGeneralRequestQueue;
    refFinishedGeneralRequestQueue = &pDDB->pGlob->GeneralRequestFinishedQueue;

    do
    {
        EDDS_RQB_REM_BLOCK_BOTTOM(refHighPriorGeneralRequestQueue->pBottom,
                                  refHighPriorGeneralRequestQueue->pTop, pRQB);

        if (pRQB)
        {

            /* handle the rqb */
            switch (EDD_RQB_GET_SERVICE(pRQB))
            {
                case EDD_SRV_MULTICAST_FWD_CTRL :
                {
                    EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE pMCParam =
                            (EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE) pRQB->pParam;
                    Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(
                            pDDB, pMCParam->pPortIDModeArray,
                            pMCParam->PortIDModeCnt, pMCParam->MACAddrPrio,
                            pMCParam->MACAddrGroup, pMCParam->MACAddrLow);
                }
                    break;

                case EDD_SRV_MULTICAST :
                {
                    EDD_UPPER_MULTICAST_PTR_TYPE pMCParam =
                            (EDD_UPPER_MULTICAST_PTR_TYPE) pRQB->pParam;
                    switch (pMCParam->Mode)
                    {
                        case EDD_MULTICAST_ENABLE:
                        {
                            Status = EDDS_LL_MC_ENABLE(pDDB,
                                                       &pMCParam->MACAddr);
                        }
                            break;

                        case EDD_MULTICAST_DISABLE:
                        {
                            Status = EDDS_LL_MC_DISABLE(pDDB, LSA_FALSE,
                                                        &pMCParam->MACAddr);
                        }
                            break;

                        case EDD_MULTICAST_DISABLE_ALL:
                        {
                            Status = EDDS_LL_MC_DISABLE(pDDB, LSA_TRUE,
                                                        LSA_NULL);
                        }
                            break;

                        default:
                        {
                            Status = EDD_STS_ERR_NOT_SUPPORTED;
                        }
                    }

                }
                    break;

                case EDD_SRV_SWITCH_SET_PORT_STATE :
                {
                    EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE pPortStates =
                            (EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE) pRQB->pParam;
                    Status = EDDS_LL_SWITCH_SET_PORT_STATE(pDDB, pPortStates);
                }
                    break;

                case EDD_SRV_SWITCH_FLUSH_FILTERING_DB :
                {
                    EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE pFlushFilterDB =
                            (EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE) pRQB->pParam;
                    Status = EDDS_LL_SWITCH_FLUSH_FILTERING_DB(pDDB,
                                                               pFlushFilterDB);
                }
                    break;

                    /* PRM commit has to be a high prio service,
                     * because it uses high prio EDDS_LL_* functions in its state machine!
                     */
                case EDD_SRV_PRM_COMMIT :
                {
                    /* returns PENDING until it is ready */
                    Status = EDDS_PrmCopySetB2A (pDDB);
                }
                    break;

                case EDD_SRV_PRM_CHANGE_PORT:
                {
                    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
                    Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB); /* get handle */

                    if( EDD_STS_OK == Status )
                    {
                        Status = EDDS_SchedulerPrmChangePort(pHDB,pRQB);
                    }
                }
                break;

                default:
                {
                    Status = EDD_STS_ERR_NOT_SUPPORTED;
                }
                    break;

            }

            if (EDD_STS_OK_PENDING == Status)
            {
                /* push this RQB back (to bottom!) because it is pending */
                EDDS_RQB_PUT_BLOCK_BOTTOM(
                        refHighPriorGeneralRequestQueue->pBottom,
                        refHighPriorGeneralRequestQueue->pTop, pRQB);
                pDDB->pGlob->triggerSchedulerHighPriorRequestPending = LSA_TRUE;
                break;
            }
            else
            {
                EDD_RQB_SET_RESPONSE(pRQB, Status);
                EDDS_RQB_PUT_BLOCK_TOP(refFinishedGeneralRequestQueue->pBottom,
                                       refFinishedGeneralRequestQueue->pTop,
                                       pRQB);
                pDDB->pGlob->GeneralRequestFinishedTriggerPending = LSA_TRUE;
            }
        }
    }
    while ((pRQB));
}


/** \brief        EDDS_getStatisticToRecord.
 *
 *  \details      This function read the PRMRecord (EDDS_PRM_PDPORT_STATISTIC_RECORD_INDEX).
 *
 *  \author       z00306yf
 *  \date         2015-09-09
 */
static LSA_RESULT
EDDS_GetStatisticToRecord (EDDS_LOCAL_HDB_PTR_TYPE pHDB,
                           EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
                           EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    EDD_UPPER_PRM_READ_PTR_TYPE pPrmReadParam;
    EDD_UPPER_MEM_U8_PTR_TYPE pRecordData;
    LSA_UINT32 InDiscards, InErrors, InOctets, OutDiscards, OutErrors,
            OutOctets;
    LSA_UINT16 PortID;
    EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset;
    EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal;
    EDDS_STATISTICS_TYPE StatsLocal;
    LSA_RESULT Status;
    LSA_UINT16 CounterStatus=0;

    pPrmReadParam = (EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam;
    PortID = pPrmReadParam->edd_port_id;
    pStatsReset = &pHDB->StatsResetValues[PortID];
    pStatsLocal = &StatsLocal;

    EDDS_MEMSET_LOCAL(pStatsLocal, 0, sizeof(StatsLocal));
    Status = EDDS_LL_GET_STATS(pDDB, PortID, &StatsLocal);

    if ( EDD_STS_OK == Status)
    {
        /* calculate the statistics depending on reset-values for this Port/IF */
        /* Note: Overflow will not be handled!                                 */
        InDiscards = pStatsLocal->InDiscards - pStatsReset->InDiscards;
        InErrors = pStatsLocal->InErrors - pStatsReset->InErrors;
        InOctets = pStatsLocal->InOctets - pStatsReset->InOctets;
        OutDiscards = pStatsLocal->OutDiscards - pStatsReset->OutDiscards;
        OutErrors = pStatsLocal->OutErrors - pStatsReset->OutErrors;
        OutOctets = pStatsLocal->OutOctets - pStatsReset->OutOctets;

        if ((pStatsLocal->SupportedCounters & EDD_MIB_SUPPORT_INOCTETS) != 0)
            CounterStatus |= EDD_RECORD_SUPPORT_INOCTETS;
        if ((pStatsLocal->SupportedCounters & EDD_MIB_SUPPORT_OUTOCTETS) != 0)
            CounterStatus |= EDD_RECORD_SUPPORT_OUTOCTETS;
        if ((pStatsLocal->SupportedCounters & EDD_MIB_SUPPORT_INDISCARDS) != 0)
            CounterStatus |= EDD_RECORD_SUPPORT_INDISCARDS;
        if ((pStatsLocal->SupportedCounters & EDD_MIB_SUPPORT_OUTDISCARDS) != 0)
            CounterStatus |= EDD_RECORD_SUPPORT_OUTDISCARDS;
        if ((pStatsLocal->SupportedCounters & EDD_MIB_SUPPORT_INERRORS) != 0)
            CounterStatus |= EDD_RECORD_SUPPORT_INERRORS;
        if ((pStatsLocal->SupportedCounters & EDD_MIB_SUPPORT_OUTERRORS) != 0)
            CounterStatus |= EDD_RECORD_SUPPORT_OUTERRORS;

        //"CounterStatus" is coded in negative logic (0=supported)
        CounterStatus = (~CounterStatus) & EDD_RECORD_SUPPORT_MASK;

        //fill prm record
        pRecordData = pPrmReadParam->record_data;

        // BlockType
        EDDS_SET_U16_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_BLOCKTYPE);

        // BlockLength
        pPrmReadParam->record_data_length = EDDS_PRM_PDPORT_STATISTIC_SIZE;
        // BlockLength + BlockType (4byte) not counted in BlockLength
        EDDS_SET_U16_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_SIZE-4); //lint !e572 !e778 correct shift operation

        // BlockVersionHigh
        EDDS_SET_U8_INCR(pRecordData,
                         EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONHIGH);
        // BlockVersionLow
        EDDS_SET_U8_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONLOW);

        // CounterStatus
        EDDS_SET_U16_INCR(pRecordData, CounterStatus);

        /* ------------------------------------------------------------------------ */
        /* >>> StatisticData                                                        */
        /* ------------------------------------------------------------------------ */
        // ifInOctets
        EDDS_SET_U32_INCR(pRecordData, InOctets);
        // ifOutOctets
        EDDS_SET_U32_INCR(pRecordData, OutOctets);
        // ifInDiscards
        EDDS_SET_U32_INCR(pRecordData, InDiscards);
        // ifOutDiscards
        EDDS_SET_U32_INCR(pRecordData, OutDiscards);
        // ifInErrors
        EDDS_SET_U32_INCR(pRecordData, InErrors);
        // ifOutErrors
        EDDS_SET_U32_INCR(pRecordData, OutErrors);

        // Padding: only for alignment (the block must be UINT32 aligned)
#if 0
        EDDS_SET_U8_INCR(pRecordData, 0x00);
#endif
    }

    return Status;
}


/** \brief        EDDS_getStatisticsToMIB.
 *
 *  \details      This function transfers the results of EDDS_LL_GET_STATS to the MIB counter structure.
 *                One port.
 *
 *  \author       z00306yf
 *  \date         2015-09-09
 */
static LSA_RESULT
EDDS_GetStatisticsToMIB (EDDS_LOCAL_HDB_PTR_TYPE pHDB,
                         EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
                         EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_RESULT Status;
    EDD_UPPER_GET_STATISTICS_PTR_TYPE pStats;

    pStats = (EDD_UPPER_GET_STATISTICS_PTR_TYPE) pRQB->pParam;

    EDDS_MEMSET_LOCAL(&pHDB->StatsValues[pStats->PortID], 0, sizeof(pHDB->StatsValues[0]));
    Status = EDDS_LL_GET_STATS(pDDB, pStats->PortID,
                               &pHDB->StatsValues[pStats->PortID]);

    if (Status == EDD_STS_OK)
    {
        EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset =
                &pHDB->StatsResetValues[pStats->PortID];
        EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal =
                &pHDB->StatsValues[pStats->PortID];

        /* calculate the statistics depending on reset-values for this Port/IF */
        /* Note: Overflow will not be handled!                                 */
        pStats->sMIB.SupportedCounters = pStatsLocal->SupportedCounters;
        pStats->sMIB.InDiscards = pStatsLocal->InDiscards
                - pStatsReset->InDiscards;
        pStats->sMIB.InErrors = pStatsLocal->InErrors - pStatsReset->InErrors;
        pStats->sMIB.InNUcastPkts = pStatsLocal->InNUcastPkts
                - pStatsReset->InNUcastPkts;
        pStats->sMIB.InOctets = pStatsLocal->InOctets - pStatsReset->InOctets;
        pStats->sMIB.InUcastPkts = pStatsLocal->InUcastPkts
                - pStatsReset->InUcastPkts;
        pStats->sMIB.InUnknownProtos = pStatsLocal->InUnknownProtos
                - pStatsReset->InUnknownProtos;
        pStats->sMIB.OutDiscards = pStatsLocal->OutDiscards
                - pStatsReset->OutDiscards;
        pStats->sMIB.OutErrors = pStatsLocal->OutErrors
                - pStatsReset->OutErrors;
        pStats->sMIB.OutNUcastPkts = pStatsLocal->OutNUcastPkts
                - pStatsReset->OutNUcastPkts;
        pStats->sMIB.OutOctets = pStatsLocal->OutOctets
                - pStatsReset->OutOctets;
        pStats->sMIB.OutQLen = pStatsLocal->OutQLen - pStatsReset->OutQLen;
        pStats->sMIB.OutUcastPkts = pStatsLocal->OutUcastPkts
                - pStatsReset->OutUcastPkts;
		/* RFC2863_COUNTER */
        pStats->sMIB.InMulticastPkts = pStatsLocal->InMulticastPkts
            - pStatsReset->InMulticastPkts;
        pStats->sMIB.InBroadcastPkts = pStatsLocal->InBroadcastPkts
            - pStatsReset->InBroadcastPkts;
        pStats->sMIB.OutMulticastPkts = pStatsLocal->OutMulticastPkts
            - pStatsReset->OutMulticastPkts;
        pStats->sMIB.OutBroadcastPkts = pStatsLocal->OutBroadcastPkts
            - pStatsReset->OutBroadcastPkts;
        pStats->sMIB.InHCOctets = pStatsLocal->InHCOctets
            - pStatsReset->InHCOctets;
        pStats->sMIB.InHCUcastPkts = pStatsLocal->InHCUcastPkts
            - pStatsReset->InHCUcastPkts;
        pStats->sMIB.InHCMulticastPkts = pStatsLocal->InHCMulticastPkts
            - pStatsReset->InHCMulticastPkts;
        pStats->sMIB.InHCBroadcastPkts = pStatsLocal->InHCBroadcastPkts
            - pStatsReset->InHCBroadcastPkts;
        pStats->sMIB.OutHCOctets = pStatsLocal->OutHCOctets
            - pStatsReset->OutHCOctets;
        pStats->sMIB.OutHCUcastPkts = pStatsLocal->OutHCUcastPkts
            - pStatsReset->OutHCUcastPkts;
        pStats->sMIB.OutHCMulticastPkts = pStatsLocal->OutHCMulticastPkts
            - pStatsReset->OutHCMulticastPkts;
        pStats->sMIB.OutHCBroadcastPkts = pStatsLocal->OutHCBroadcastPkts
            - pStatsReset->OutHCBroadcastPkts;
		/* RFC2863_COUNTER - End */

        /* if we should reset we store the actual values as Reset-Values */
        if (pStats->Reset)
        {
            pStatsReset->InDiscards = pStatsLocal->InDiscards;
            pStatsReset->InErrors = pStatsLocal->InErrors;
            pStatsReset->InNUcastPkts = pStatsLocal->InNUcastPkts;
            pStatsReset->InOctets = pStatsLocal->InOctets;
            pStatsReset->InUcastPkts = pStatsLocal->InUcastPkts;
            pStatsReset->InUnknownProtos = pStatsLocal->InUnknownProtos;
            pStatsReset->OutDiscards = pStatsLocal->OutDiscards;
            pStatsReset->OutErrors = pStatsLocal->OutErrors;
            pStatsReset->OutNUcastPkts = pStatsLocal->OutNUcastPkts;
            pStatsReset->OutOctets = pStatsLocal->OutOctets;
            pStatsReset->OutQLen = pStatsLocal->OutQLen;
            pStatsReset->OutUcastPkts = pStatsLocal->OutUcastPkts;
			/* RFC2863_COUNTER */
			pStatsReset->InMulticastPkts = pStatsLocal->InMulticastPkts;
            pStatsReset->InBroadcastPkts = pStatsLocal->InBroadcastPkts;
            pStatsReset->OutMulticastPkts = pStatsLocal->OutMulticastPkts;
            pStatsReset->OutBroadcastPkts = pStatsLocal->OutBroadcastPkts;
            pStatsReset->InHCOctets = pStatsLocal->InHCOctets;
            pStatsReset->InHCUcastPkts = pStatsLocal->InHCUcastPkts;
            pStatsReset->InHCMulticastPkts = pStatsLocal->InHCMulticastPkts;
            pStatsReset->InHCBroadcastPkts = pStatsLocal->InHCBroadcastPkts;
            pStatsReset->OutHCOctets = pStatsLocal->OutHCOctets;
            pStatsReset->OutHCUcastPkts = pStatsLocal->OutHCUcastPkts;
            pStatsReset->OutHCMulticastPkts = pStatsLocal->OutHCMulticastPkts;
            pStatsReset->OutHCBroadcastPkts = pStatsLocal->OutHCBroadcastPkts;
			/* RFC2863_COUNTER - End */
		}
    }

    return Status;
}


/** \brief        EDDS_getStatisticsAllToMIB.
 *
 *  \details      This function transfers the results of EDDS_LL_GET_STATS to the MIB counter structure.
 *                All ports.
 *
 *  \author       z00306yf
 *  \date         2015-09-09
 */
static LSA_RESULT
EDDS_GetStatisticsAllToMIB (EDDS_LOCAL_HDB_PTR_TYPE pHDB,
                            EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
                            EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16 PortID;
    LSA_RESULT Status = EDD_STS_OK;
    EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE pStats;

    pStats = (EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE) pRQB->pParam;

    for (PortID = 0; PortID <= pDDB->pGlob->HWParams.Caps.PortCnt; ++PortID)
    {
        EDDS_MEMSET_LOCAL(&pHDB->StatsValues[PortID], 0,
                          sizeof(pHDB->StatsValues[0]));

        /* Note: PortID 0 = interface */
        Status = EDDS_LL_GET_STATS(pDDB, PortID, &pHDB->StatsValues[PortID]);

        if (Status == EDD_STS_OK)
        {
            EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset =
                    &pHDB->StatsResetValues[PortID];
            EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal =
                    &pHDB->StatsValues[PortID];

            /* calculate the statistics depending on reset-values for this Port/IF */
            /* Note: Overflow will not be handled!                                 */
            pStats->sMIB[PortID].SupportedCounters =
                    pStatsLocal->SupportedCounters;
            pStats->sMIB[PortID].InDiscards = pStatsLocal->InDiscards
                    - pStatsReset->InDiscards;
            pStats->sMIB[PortID].InErrors = pStatsLocal->InErrors
                    - pStatsReset->InErrors;
            pStats->sMIB[PortID].InNUcastPkts = pStatsLocal->InNUcastPkts
                    - pStatsReset->InNUcastPkts;
            pStats->sMIB[PortID].InOctets = pStatsLocal->InOctets
                    - pStatsReset->InOctets;
            pStats->sMIB[PortID].InUcastPkts = pStatsLocal->InUcastPkts
                    - pStatsReset->InUcastPkts;
            pStats->sMIB[PortID].InUnknownProtos = pStatsLocal->InUnknownProtos
                    - pStatsReset->InUnknownProtos;
            pStats->sMIB[PortID].OutDiscards = pStatsLocal->OutDiscards
                    - pStatsReset->OutDiscards;
            pStats->sMIB[PortID].OutErrors = pStatsLocal->OutErrors
                    - pStatsReset->OutErrors;
            pStats->sMIB[PortID].OutNUcastPkts = pStatsLocal->OutNUcastPkts
                    - pStatsReset->OutNUcastPkts;
            pStats->sMIB[PortID].OutOctets = pStatsLocal->OutOctets
                    - pStatsReset->OutOctets;
            pStats->sMIB[PortID].OutQLen = pStatsLocal->OutQLen
                    - pStatsReset->OutQLen;
            pStats->sMIB[PortID].OutUcastPkts = pStatsLocal->OutUcastPkts
                    - pStatsReset->OutUcastPkts;
			/* RFC2863_COUNTER */
			pStats->sMIB[PortID].InMulticastPkts = pStatsLocal->InMulticastPkts
                - pStatsReset->InMulticastPkts;
            pStats->sMIB[PortID].InBroadcastPkts = pStatsLocal->InBroadcastPkts
                - pStatsReset->InBroadcastPkts;
            pStats->sMIB[PortID].OutMulticastPkts = pStatsLocal->OutMulticastPkts
                - pStatsReset->OutMulticastPkts;
            pStats->sMIB[PortID].OutBroadcastPkts = pStatsLocal->OutBroadcastPkts
                - pStatsReset->OutBroadcastPkts;
            pStats->sMIB[PortID].InHCOctets = pStatsLocal->InHCOctets
                - pStatsReset->InHCOctets;
            pStats->sMIB[PortID].InHCUcastPkts = pStatsLocal->InHCUcastPkts
                - pStatsReset->InHCUcastPkts;
            pStats->sMIB[PortID].InHCMulticastPkts = pStatsLocal->InHCMulticastPkts
                - pStatsReset->InHCMulticastPkts;
            pStats->sMIB[PortID].InHCBroadcastPkts = pStatsLocal->InHCBroadcastPkts
                - pStatsReset->InHCBroadcastPkts;
            pStats->sMIB[PortID].OutHCOctets = pStatsLocal->OutHCOctets
                - pStatsReset->OutHCOctets;
            pStats->sMIB[PortID].OutHCUcastPkts = pStatsLocal->OutHCUcastPkts
                - pStatsReset->OutHCUcastPkts;
            pStats->sMIB[PortID].OutHCMulticastPkts = pStatsLocal->OutHCMulticastPkts
                - pStatsReset->OutHCMulticastPkts;
            pStats->sMIB[PortID].OutHCBroadcastPkts = pStatsLocal->OutHCBroadcastPkts
                - pStatsReset->OutHCBroadcastPkts;
			/* RFC2863_COUNTER - End */

            /* if we should reset we store the actual values as Reset-Values */
            if (pStats->Reset)
            {
                pStatsReset->InDiscards = pStatsLocal->InDiscards;
                pStatsReset->InErrors = pStatsLocal->InErrors;
                pStatsReset->InNUcastPkts = pStatsLocal->InNUcastPkts;
                pStatsReset->InOctets = pStatsLocal->InOctets;
                pStatsReset->InUcastPkts = pStatsLocal->InUcastPkts;
                pStatsReset->InUnknownProtos = pStatsLocal->InUnknownProtos;
                pStatsReset->OutDiscards = pStatsLocal->OutDiscards;
                pStatsReset->OutErrors = pStatsLocal->OutErrors;
                pStatsReset->OutNUcastPkts = pStatsLocal->OutNUcastPkts;
                pStatsReset->OutOctets = pStatsLocal->OutOctets;
                pStatsReset->OutQLen = pStatsLocal->OutQLen;
                pStatsReset->OutUcastPkts = pStatsLocal->OutUcastPkts;
				/* RFC2863_COUNTER */
				pStatsReset->InMulticastPkts = pStatsLocal->InMulticastPkts;
                pStatsReset->InBroadcastPkts = pStatsLocal->InBroadcastPkts;
                pStatsReset->OutMulticastPkts = pStatsLocal->OutMulticastPkts;
                pStatsReset->OutBroadcastPkts = pStatsLocal->OutBroadcastPkts;
                pStatsReset->InHCOctets = pStatsLocal->InHCOctets;
                pStatsReset->InHCUcastPkts = pStatsLocal->InHCUcastPkts;
                pStatsReset->InHCMulticastPkts = pStatsLocal->InHCMulticastPkts;
                pStatsReset->InHCBroadcastPkts = pStatsLocal->InHCBroadcastPkts;
                pStatsReset->OutHCOctets = pStatsLocal->OutHCOctets;
                pStatsReset->OutHCUcastPkts = pStatsLocal->OutHCUcastPkts;
                pStatsReset->OutHCMulticastPkts = pStatsLocal->OutHCMulticastPkts;
                pStatsReset->OutHCBroadcastPkts = pStatsLocal->OutHCBroadcastPkts;
				/* RFC2863_COUNTER - End */
			}
        }
    }

    return Status;
}


/** \brief        EDDS_resetStatistics.
 *
 *  \details      This function resets the statistic counters.
 *
 *  \author       z00306yf
 *  \date         2015-09-09
 */
static LSA_RESULT
EDDS_ResetStatistics (EDDS_LOCAL_HDB_PTR_TYPE pHDB,
                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB,
                      EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT32 i = 0;
    EDD_UPPER_RESET_STATISTICS_PTR_TYPE pReset;
    LSA_RESULT Status = EDD_STS_OK;

    pReset = (EDD_UPPER_RESET_STATISTICS_PTR_TYPE) pRQB->pParam;

    /* loop over all ports and IF */
    while ((i <= pDDB->pGlob->HWParams.Caps.PortCnt) && (Status == EDD_STS_OK ))
    {
        /* should we reset this port/IF ? */
        if ((i == pReset->PortID)
                || ( EDD_STATISTICS_RESET_ALL == pReset->PortID))
        {
            EDDS_MEMSET_LOCAL(&pHDB->StatsValues[i], 0,
                              sizeof(pHDB->StatsValues[0]));
            /* Note: PortID 0 = interface */
            Status = EDDS_LL_GET_STATS(pDDB, i, &pHDB->StatsValues[i]);

            if (Status == EDD_STS_OK)
            {
                EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset =
                        &pHDB->StatsResetValues[i];
                EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal =
                        &pHDB->StatsValues[i];

                pStatsReset->InDiscards = pStatsLocal->InDiscards;
                pStatsReset->InErrors = pStatsLocal->InErrors;
                pStatsReset->InNUcastPkts = pStatsLocal->InNUcastPkts;
                pStatsReset->InOctets = pStatsLocal->InOctets;
                pStatsReset->InUcastPkts = pStatsLocal->InUcastPkts;
                pStatsReset->InUnknownProtos = pStatsLocal->InUnknownProtos;
                pStatsReset->OutDiscards = pStatsLocal->OutDiscards;
                pStatsReset->OutErrors = pStatsLocal->OutErrors;
                pStatsReset->OutNUcastPkts = pStatsLocal->OutNUcastPkts;
                pStatsReset->OutOctets = pStatsLocal->OutOctets;
                pStatsReset->OutQLen = pStatsLocal->OutQLen;
                pStatsReset->OutUcastPkts = pStatsLocal->OutUcastPkts;
				/* RFC2863_COUNTER */
				pStatsReset->InMulticastPkts = pStatsLocal->InMulticastPkts;
                pStatsReset->InBroadcastPkts = pStatsLocal->InBroadcastPkts;
                pStatsReset->OutMulticastPkts = pStatsLocal->OutMulticastPkts;
                pStatsReset->OutBroadcastPkts = pStatsLocal->OutBroadcastPkts;
                pStatsReset->InHCOctets = pStatsLocal->InHCOctets;
                pStatsReset->InHCUcastPkts = pStatsLocal->InHCUcastPkts;
                pStatsReset->InHCMulticastPkts = pStatsLocal->InHCMulticastPkts;
                pStatsReset->InHCBroadcastPkts = pStatsLocal->InHCBroadcastPkts;
                pStatsReset->OutHCOctets = pStatsLocal->OutHCOctets;
                pStatsReset->OutHCUcastPkts = pStatsLocal->OutHCUcastPkts;
                pStatsReset->OutHCMulticastPkts = pStatsLocal->OutHCMulticastPkts;
                pStatsReset->OutHCBroadcastPkts = pStatsLocal->OutHCBroadcastPkts;
				/* RFC2863_COUNTER - End */
			}
        }
        i++;
    }

    return Status;
}


/** \brief        EDDS_HandleRequests.
 *
 *  \details      This function handles pending requests from RQB context.
 *
 *  \author       am2219
 *  \date         2015-08-06
 */
static LSA_VOID
EDDS_HandleRequests (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    EDD_UPPER_RQB_PTR_TYPE pRQB;
    EDDS_RQB_LIST_TYPE_PTR refGeneralRequestQueue;
    EDDS_RQB_LIST_TYPE_PTR refFinishedGeneralRequestQueue;

    refGeneralRequestQueue = &pDDB->pGlob->GeneralRequestQueue;
    refFinishedGeneralRequestQueue = &pDDB->pGlob->GeneralRequestFinishedQueue;

    EDDS_INTERNAL_STATISTIC_ENTER(pDDB->hSysDev,
                                  pDDB->PerformanceStatistics.callsHandleRequest);

    do
    {
        EDDS_RQB_REM_BLOCK_BOTTOM(refGeneralRequestQueue->pBottom,
                                  refGeneralRequestQueue->pTop, pRQB);

        if (pRQB)
        {
            LSA_RESULT Status = EDD_STS_ERR_SERVICE; /* for clean code in switch statement */

            switch (EDD_RQB_GET_SERVICE(pRQB))
            {
                case EDD_SRV_LED_BLINK :
                {
                    EDDS_LEDShowLocation (pDDB);
                    Status = EDD_STS_OK;
                }
                    break;

                case EDD_SRV_PRM_READ :
                {
                    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
                    Status = EDDS_HandleGetHDB (EDD_RQB_GET_HANDLE_LOWER(pRQB),
                                                &pHDB); /* get handle */

                    switch (((EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam)->record_index)
                    {
                        case EDDS_PRM_PDPORT_STATISTIC_RECORD_INDEX:
                        {
                            if (Status == EDD_STS_OK)
                            {
                                Status = EDDS_GetStatisticToRecord (pHDB, pDDB,
                                                                    pRQB);
                            }
                            break;
                        }
                        default:
                            break; // should not get here
                    }
                }
                    break;

                case EDD_SRV_GET_STATISTICS :
                {
                    EDDS_LOCAL_HDB_PTR_TYPE pHDB;

                    Status = EDDS_HandleGetHDB (EDD_RQB_GET_HANDLE_LOWER(pRQB),
                                                &pHDB); /* get handle */

                    if ( EDD_STS_OK == Status)
                    {
                        Status = EDDS_GetStatisticsToMIB (pHDB, pDDB, pRQB);
                    }

                }
                    break;

                case EDD_SRV_RESET_STATISTICS :
                {
                    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
                    Status = EDDS_HandleGetHDB (EDD_RQB_GET_HANDLE_LOWER(pRQB),
                                                &pHDB); /* get handle */

                    if ( EDD_STS_OK == Status)
                    {
                        Status = EDDS_ResetStatistics (pHDB, pDDB, pRQB);
                    }
                }
                    break;

                case EDD_SRV_GET_STATISTICS_ALL :
                {
                    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
                    Status = EDDS_HandleGetHDB (EDD_RQB_GET_HANDLE_LOWER(pRQB),
                                                &pHDB); /* get handle */

                    if ( EDD_STS_OK == Status)
                    {
                        Status = EDDS_GetStatisticsAllToMIB (pHDB, pDDB, pRQB);
                    }

                    break;
                }

#ifdef LLIF_CFG_USE_LL_ARP_FILTER
                    case EDD_SRV_NRT_SET_ARP_FILTER:
                    {
                        EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE pRqbParam =
                        (EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE) pRQB->pParam;
                        EDDS_NRT_ARP_FILTER_MGM_PTR pARPFilter = &pDDB->pNRT->ARPFilter;

                        Status = EDD_STS_OK;

                        switch(pRqbParam->Mode)
                        {
                            case EDD_ARP_FILTER_OFF:
                            {
                                /* reset filter at LL when function pointer is properly set */
                                if( EDDS_LL_AVAILABLE(pDDB,setArpFilter) )
                                {
                                    Status = EDDS_LL_SET_ARP_FILTER(pDDB, pRqbParam->Mode, pRqbParam->UserIndex, pRqbParam->IPAddr);
                                }
                                break;
                            }
                            case EDD_ARP_FILTER_ON:
                            {
                                /* set filter at LL when function pointer is properly set */
                                if( EDDS_LL_AVAILABLE(pDDB,setArpFilter) )
                                {
                                    Status = EDDS_LL_SET_ARP_FILTER(pDDB, pRqbParam->Mode, pRqbParam->UserIndex, pRqbParam->IPAddr); /* EDDS filter is disabled */
                                }
                                else
                                {
                                    pARPFilter->InUse = LSA_TRUE; /* EDDS filter is enabled */
                                }
                                break;
                            }
                            default:
                            {
                                Status = EDD_STS_ERR_PARAM;
                                break;
                            }
                        }
                        break;
                    }
#endif

                default:
                {
                    Status = EDD_STS_ERR_NOT_SUPPORTED;
                }
                    break;
            }

            if (EDD_STS_OK_PENDING == Status)
            {
                /* push this RQB back (to bottom!), because it is pending */
                EDDS_RQB_PUT_BLOCK_BOTTOM(refGeneralRequestQueue->pBottom,
                                          refGeneralRequestQueue->pTop, pRQB);

                /* if in NRT-mode a scheduler trigger is send to rqb context
                 */
                if (!pDDB->pGlob->IO_Configured)
                {
                    pDDB->pGlob->triggerSchedulerRequestPending = LSA_TRUE;

                    /* leave this handling-loop! */
                    break;
                }
            }
            else
            {

                EDD_RQB_SET_RESPONSE(pRQB, Status);
                EDDS_RQB_PUT_BLOCK_TOP(refFinishedGeneralRequestQueue->pBottom,
                                       refFinishedGeneralRequestQueue->pTop,
                                       pRQB);
                pDDB->pGlob->GeneralRequestFinishedTriggerPending = LSA_TRUE;
            }
        }

    }
    while ((pRQB) && (!pDDB->pGlob->IO_Configured)); /* handle more RQBs, if in NRT mode */

    EDDS_INTERNAL_STATISTIC_EXIT(pDDB->hSysDev,
                                 pDDB->PerformanceStatistics.callsHandleRequest);
}


/** \brief      EDDS_HandleRecurringTask.
 *
 *  \details    This function calls the LL specific LL_RECURRING_TASK function and checks for link changes.
 *
 *  \param[IN]  pDDB                    Pointer to Device Description Block
 *
 *  \note       On the LL specific LL_RECURRING_TASK statistic counters and the link status are updated. LL specific bug workarounds can
 *              be implemented here, e.g. link down state machine.
 *
 *  \author     am2219
 *  \date       2015-02-24
 */
static LSA_VOID
EDDS_HandleRecurringTask (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    if (!(pDDB->pGlob->scheduler.RecurringTaskExecuteCountDown))
    {
        LSA_UINT32 resultBitmap;
        pDDB->pGlob->scheduler.RecurringTaskExecuteCountDown =
                pDDB->pGlob->scheduler.RecurringTaskExecuteCountDownReloadValue;

        EDDS_INTERNAL_STATISTIC_ENTER(
                pDDB->hSysDev, pDDB->PerformanceStatistics.callsRecurringTask);
        resultBitmap =
                EDDS_LL_RECURRING_TASK(
                        pDDB,
                        pDDB->pGlob->scheduler.RecurringTaskExecuteCountDownReloadValue ? LSA_TRUE : LSA_FALSE);
        EDDS_INTERNAL_STATISTIC_EXIT(pDDB->hSysDev,
                                     pDDB->PerformanceStatistics.callsRecurringTask);

        /* handle a link state change, even if chip is in reset! */
        if (EDDS_LL_RECURRING_TASK_LINK_CHANGED & resultBitmap)
        {
            EDDS_INTERNAL_STATISTIC_ENTER(pDDB->hSysDev,
                                          pDDB->PerformanceStatistics.callsPhyEvent);
            EDDS_HandlePHYEvent (pDDB);
            EDDS_INTERNAL_STATISTIC_EXIT(pDDB->hSysDev,
                                         pDDB->PerformanceStatistics.callsPhyEvent);
        }

        /* a reset is pending, zero RecurringTaskExecuteCountDown to
         * force EDDS_LL_RECURRING_TASK execution next time
         * other async LL functions should not be executed
         */
        if (EDDS_LL_RECURRING_TASK_CHIP_RESET & resultBitmap)
        {
            /* check EDDS_LL_RECURRING_TASK the next time, do nothing else yet.  */
            pDDB->pGlob->scheduler.RecurringTaskExecuteCountDown = 0;
        }
        else
        {
            /* other actions may be implemented, if they are needed. */

            /* is RecurringTaskExecuteCountDownReloadValue zero ?
             * --> there is no reduction of EDDS_LL_RECURRING_TASK execution
             * --> handle General Request, even if EDDS_LL_RECURRING_TASK was executed
             */
            if (!(pDDB->pGlob->scheduler.RecurringTaskExecuteCountDownReloadValue))
            {
                EDDS_HandleRequests (pDDB);
            }
        }
    }
    else
    {
        EDDS_HandleRequests (pDDB);

        /* EDDS_HandleRequest can reset the execute count down aswell */
        if ((pDDB->pGlob->scheduler.RecurringTaskExecuteCountDown))
            --pDDB->pGlob->scheduler.RecurringTaskExecuteCountDown;
    }
}


/** \brief      EDDS_SchedulerUpdateCycleTime.
 *
 *  \details    This function updates the internal scheduler cycle time. This depends on IO-Configured parameter.
 *
 *  \param      pDDB            Pointer to Device Description Block
 *
 *  \author     am2219
 *  \date       2015-08-12
 */
LSA_VOID
EDDS_SchedulerUpdateCycleTime (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    /* set actual scheduler cycle time in milliseconds,
     * set actual receive packet count limitation per scheduler cycle */
    if (pDDB->pGlob->IO_Configured) /* IO-Mode */
    {
        pDDB->pGlob->scheduler.CycleTimeMS = pDDB->pGlob->schedulerCycleIO;
        pDDB->pGlob->RxPacketsQuota = pDDB->pGlob->RxPacketsQuota_IO;
    }
    else /* NRT-Mode */
    {
        pDDB->pGlob->scheduler.CycleTimeMS = pDDB->pGlob->schedulerCycleNRT;
        pDDB->pGlob->RxPacketsQuota = pDDB->pGlob->RxPacketsQuota_NRT;
    }

    /* reinit jitter statistics */
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
    EDDS_INTERNAL_STATISTICS_JITTER_DEINIT(pDDB->PerformanceStatistics.cylceJitter); //lint !e713 !e732 !e776 TH remove warning for debug statistics (not relevant)
    EDDS_INTERNAL_STATISTICS_JITTER_INIT(pDDB->PerformanceStatistics.cylceJitter); //lint !e569 TH remove warning for debug statistics (not relevant)
#endif

    /* set reduction of EDDS_LL_RECURRING_TASK */
    pDDB->pGlob->scheduler.RecurringTaskExecuteCountDown = 0;
    pDDB->pGlob->scheduler.RecurringTaskExecuteCountDownReloadValue = (LSA_UINT8)(
            ((LSA_UINT16)pDDB->pGlob->schedulerRecurringTaskMS) / pDDB->pGlob->scheduler.CycleTimeMS);
    if (0 != pDDB->pGlob->scheduler.RecurringTaskExecuteCountDownReloadValue)
        --pDDB->pGlob->scheduler.RecurringTaskExecuteCountDownReloadValue;

    /* calculate Tx packets quota, if FILL is on */
    if (pDDB->pGlob->FeedInLoadLimitationActive)
    {
        pDDB->pNRT->TxPacketsCycleQuota = pDDB->pGlob->scheduler.CycleTimeMS
                * pDDB->pNRT->TxPacketsQuotaMS;
    }
    else
    {
        pDDB->pNRT->TxPacketsCycleQuota = 0;
    }
}


/** \brief      EDDS_CheckInternalTimer.
 *
 *  \details    Check for expired software timers. These timers are only used by EDDS.
 *              When a timer is expired its callback function is called.
 *
 *  \param      pDDB            Pointer to Device Description Block
 *
 *  \note       The scheduler is called cyclically by system adaption with a fixed cycle (with including possibly jitters). The cycle is specified on OPEN.
 *              Additionally the scheduler can be called in between regular scheduler calls with a so called intermediate call. Thereby the timer
 *              implementation can not assume a certain fixed cycle. Therefore a output macro is used to get the current system time in ns.
 *
 *  \author     dp
 *  \date       2015-02-24
 *  \author     am2219
 *  \date       2016-05-09
 */
static LSA_VOID
EDDS_CheckInternalTimer (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_UINT16 i;
    LSA_UINT64 currentTime;

    currentTime = EDDS_GET_SYSTEM_TIME_NS (pDDB->hSysDev);

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "IN :EDDS_CheckInternalTimer(pDDB: 0x%X)", pDDB);

    for (i = 0; i < EDDS_TIMER_MAX; i++)
    {
        if (pDDB->timerMgmt[i].nsExpireTime != 0)
        {
            if (currentTime >= pDDB->timerMgmt[i].nsExpireTime)
            {
                // call cbf
                EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                    "EDDS_CheckInternalTimer cbf (ms: %d)",
                                    (LSA_UINT32) (currentTime / (1000 * 1000)));

                /* calculate next expire time, before calling TimeoutMethod:
                 * TimeoutMethod may call EDDS_StopTimer !
                 */
                if (pDDB->timerMgmt[i].nsCycleTimeBase != 0)
                {
                    pDDB->timerMgmt[i].nsExpireTime += pDDB->timerMgmt[i].nsCycleTimeBase;

                    /* timer handling "overload" */
                    if(currentTime >= pDDB->timerMgmt[i].nsExpireTime)
                    {
                        EDDS_SCHED_TRACE_03(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                                "    :EDDS_CheckInternalTimer(pDDB: 0x%X) cyclic timer(%d) overload (%d reload value)",
                                                pDDB,i,pDDB->timerMgmt[i].nsCycleTimeBase);
                        pDDB->timerMgmt[i].nsExpireTime = currentTime + pDDB->timerMgmt[i].nsCycleTimeBase;
                    }
                }

                pDDB->timerMgmt[i].TimeoutMethod (pDDB);
            }
        }
    }

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                        "OUT :EDDS_CheckInternalTimer(pDDB: 0x%X)", pDDB);
}


/**
 *  \brief      EDDS_ResetTxBandwithLimit.
 *
 *  \details    Reset Tx bandwith limitation for all prio queues on a cyclic scheduler call.
 *              Every Tx prio queue has a bandwith restriction. With every sent frame the respective current byte count
 *              is raised by frame length (on wire length). When the maximum is reached no more frames are sent for
 *              this queue.
 *              By resetting the Tx bandwith limitation with only every cyclic scheduler call all subsequent intermediate
 *              calls share the rest bandwith of the current cycle.
 *
 *  \param      pDDB            Pointer to Device Description Block
 *
 *  \author     adpath2
 *  \date       2015-07-10
 */
static LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_ResetTxBandwithLimit (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_UINT16 Prio;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;

    pNRTComp = pDDB->pNRT;

    for (Prio = 0; Prio < EDDS_NRT_TX_PRIO_CNT; Prio++)
    {
        pNRTComp->Tx[Prio].TxByteCntCurrent = 0;
    }
    pDDB->pNRT->actTxPacketsCycle = 0; /* new cycle, reset send NRT Tx Packets from (last) cycle. */
}


static LSA_UINT32 EDDS_LOCAL_FCT_ATTR
EDDS_GetTaskBitField (EDDS_LOCAL_DDB_PTR_TYPE const pDDB, LSA_UINT8 const intermediateCall)
{
    LSA_UINT32 tasksBitField;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    pGlob = pDDB->pGlob;

    if (EDDS_SCHEDULER_INTERMEDIATE_CALL != intermediateCall)
    {
        /* cyclic call */
        pGlob->scheduler.currentCycleBeginNS = pGlob->scheduler.currentBeginNS;
        pGlob->actTxPacketsFinishCycle = 0;  /* new cycle, reset send Tx Packets Finish from (last) cycle. */

        tasksBitField = ( EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART1
                | EDDS_SCHEDULER_BITFIELD_NRT_RCV
                | EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS
                | EDDS_SCHEDULER_BITFIELD_RESET_FILL);
        if (pGlob->IO_Configured && pGlob->IsSrtUsed)
        {
            tasksBitField |= EDDS_SCHEDULER_BITFIELD_SRT;
        }
        else
        {
            tasksBitField |= EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART2;
        }
    }
    else
    {
        /* intermediate call */
        tasksBitField = ( EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART1
                | EDDS_SCHEDULER_BITFIELD_NRT_RCV
                | EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART2);
        if (pGlob->FeedInLoadLimitationActive)
        {
            LSA_UINT64 currentTimeMicroSeconds;
            LSA_UINT64 lastResetTxBandwithTimeMicroSeconds;

            /* check-granularity is 1us */
            currentTimeMicroSeconds = pGlob->scheduler.currentCycleBeginNS / 1000;
            lastResetTxBandwithTimeMicroSeconds = pGlob->lastResetTxBandwithTime / 1000;

            if ((lastResetTxBandwithTimeMicroSeconds != 0) // ignore initial case with default value 0
                    && (pGlob->scheduler.CycleTimeMS > 1) // schedulerCycle must be larger than 1ms
                    && ((currentTimeMicroSeconds
                            - lastResetTxBandwithTimeMicroSeconds) > 1000) // last call must be at least 1ms in the past
                    && (((currentTimeMicroSeconds
                            - lastResetTxBandwithTimeMicroSeconds) + 1000)
                            < ((LSA_UINT64)1000 * pGlob->scheduler.CycleTimeMS)) // is the next cyclic at least 1ms in the future
                    && (currentTimeMicroSeconds > lastResetTxBandwithTimeMicroSeconds) // currentTime must be larger than previous time
                    )
            {
                tasksBitField |= EDDS_SCHEDULER_BITFIELD_RESET_FILL;
            }
        }
    }

    /* this should be checked in any mode */
    if (!EDDS_RQB_QUEUE_TEST_EMPTY(
            &pDDB->pGlob->HighPriorGeneralRequestQueue))
    {
    /* High priority hardware programming is running */
        tasksBitField |= EDDS_SCHEDULER_BITFIELD_HIGHPRIOR_RQB;
    }

    return tasksBitField;
}


static LSA_VOID EDDS_LOCAL_FCT_ATTR
EDDS_DoGeneralRequests (EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_BOOL doNotifyRetriggerScheduler;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    pGlob = pDDB->pGlob;

    doNotifyRetriggerScheduler = LSA_FALSE;

    /* all finished RQBs should be send to RQB context
     */
    if (pGlob->GeneralRequestFinishedTriggerPending)
    {
        if (!pGlob->GeneralRequestFinishedTrigger.InUse)
        {
            pGlob->GeneralRequestFinishedTriggerPending = LSA_FALSE;
            pGlob->GeneralRequestFinishedTrigger.InUse = LSA_TRUE;
            EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                    pDDB->hSysDev,
                    pGlob->GeneralRequestFinishedTrigger.pRQB);
        }
    }

    /* confirm consumer indications within rqb context
     */
    if (pGlob->IsSrtUsed)
    {
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pSRT;
        pSRT = pDDB->pSRT;
        if (pSRT->Indication.IndicationTriggerPending)
        {
            if (!pSRT->Indication.ConsIndTriggerRQB.InUse)
            {
                pSRT->Indication.IndicationTriggerPending = LSA_FALSE;
                pSRT->Indication.ConsIndTriggerRQB.InUse = LSA_TRUE;
                EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                        pDDB->hSysDev,
                        pSRT->Indication.ConsIndTriggerRQB.pRQB);
            }
        }
    }

    /* send all ARP Frames for filtering to to RQB context
     */
    if (pGlob->FilterARPFrameTriggerPending)
    {
        if (!pGlob->FilterARPFrameTrigger.InUse)
        {
            pGlob->FilterARPFrameTriggerPending = LSA_FALSE;
            pGlob->FilterARPFrameTrigger.InUse = LSA_TRUE;
            EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                    pDDB->hSysDev,
                    pGlob->FilterARPFrameTrigger.pRQB);
        }
    }

    /* send all DCP Frames for filtering to to RQB context
     */
    if (pGlob->FilterDCPFrameTriggerPending)
    {
        if (!pGlob->FilterDCPFrameTrigger.InUse)
        {
            pGlob->FilterDCPFrameTriggerPending = LSA_FALSE;
            pGlob->FilterDCPFrameTrigger.InUse = LSA_TRUE;
            EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                    pDDB->hSysDev,
                    pGlob->FilterDCPFrameTrigger.pRQB);
        }
    }

    /* send all DCP_HELLO Frames for filtering to to RQB context
     */
    if (pGlob->FilterDCPHELLOFrameTriggerPending)
    {
        if (!pGlob->FilterDCPHELLOFrameTrigger.InUse)
        {
            pGlob->FilterDCPHELLOFrameTriggerPending = LSA_FALSE;
            pGlob->FilterDCPHELLOFrameTrigger.InUse = LSA_TRUE;
            EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                    pDDB->hSysDev,
                    pGlob->FilterDCPHELLOFrameTrigger.pRQB);
        }
    }

    /* fire pending events
     */
    if (pGlob->SchedulerEventsDonePending)
    {
        if (!pGlob->SchedulerEventsDone.InUse)
        {
            pGlob->SchedulerEventsDone.InUse = LSA_TRUE;

            /* set actual pending events in internal RQB */
            ((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pGlob->SchedulerEventsDone.pRQB->pParam)->Param =
                    pGlob->SchedulerEventsDonePending;

            /* clear actual pending events */
            EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                    pDDB->hSysDev,
                    pGlob->SchedulerEventsDone.pRQB);
            pGlob->SchedulerEventsDonePending &=
                    ~(((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pGlob->SchedulerEventsDone.pRQB->pParam)->Param);

        }
    }

    /* this should be done as last send trigger, to give RQB context the chance to finish the
     * executed RQBs from GeneralRequestFinishedTrigger;
     * triggerSchedulerRequestPending could only be true, if we are in NRT-mode (@see EDDS_HandleRequests)
     * or if there is a high prior RQB pending that has to be finished asap
     */
    if (pGlob->triggerSchedulerRequestPending || pGlob->triggerSchedulerPending || pGlob->triggerSchedulerHighPriorRequestPending)
    {
        if(pGlob->IO_Configured || pGlob->NRT_UseNotifySchedulerCall)
        {
            /* edds_scheduler should get a trigger with EDDS_DO_RETRIGGER_SCHEDULER() when we are in
             * - IO mode
             * - NRT mode with set NRT_UseNotifySchedulerCall
             */

            pGlob->triggerSchedulerPending = LSA_FALSE;
            pGlob->triggerSchedulerRequestPending = LSA_FALSE;
            pGlob->triggerSchedulerHighPriorRequestPending = LSA_FALSE;
            doNotifyRetriggerScheduler = LSA_TRUE;
        }
        else
        {
            /* NRT-Mode and NRT_UseNotifySchedulerCall is not set: */

            /* Locking between Edds scheduler context (cyclic calls in NRT-Mode are executed in this mode):
             * EDDS_ENTER(pDDB->hSysDev) / EDDS_EXIT(pDDB->hSysDev) is done in edds_scheduler!
             */
            if (!pGlob->triggerSchedulerRequest.InUse)
            {
                pGlob->triggerSchedulerPending = LSA_FALSE;
                pGlob->triggerSchedulerRequestPending = LSA_FALSE;
                pGlob->triggerSchedulerHighPriorRequestPending = LSA_FALSE;
                pGlob->triggerSchedulerRequest.InUse = LSA_TRUE;
                EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
                        pDDB->hSysDev,
                        pGlob->triggerSchedulerRequest.pRQB);
            }
        }
    }

    /* Indirect Call of scheduler through macro */
    if(doNotifyRetriggerScheduler)
    {
        EDDS_DO_RETRIGGER_SCHEDULER(pDDB->hSysDev);
    }
}

#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
static void EDDS_INTERNAL_STATISTICS_JITTER_GET(EDDS_INTERNAL_STATISTIC_JITTER_TYPE * const jitterStatistics, LSA_UINT64 _time, LSA_UINT32 interval)
{
    if( jitterStatistics->time )
    {
        LSA_BOOL getstatistics = LSA_TRUE;
        jitterStatistics->jitter =
                (LSA_INT64)(_time - jitterStatistics->time) - interval;

        /* first cycle may cause one "cycleSkip" */

        if(jitterStatistics->jitter >= (LSA_INT32)(interval>>2))
        {
            ++jitterStatistics->skipping;
            getstatistics = LSA_FALSE;
        }
        else if(-jitterStatistics->jitter >= (LSA_INT32)(interval>>2))
        {
            ++jitterStatistics->bouncing;
            getstatistics = LSA_FALSE;
        }

        if(getstatistics)
        {
            /* add jitter */
            if(0 > jitterStatistics->jitter)
            {
                jitterStatistics->jittersumN -= jitterStatistics->jitter; //lint !e737 TH if case prevents the loss
                ++jitterStatistics->sumcountN;
            }
            else
            {
                jitterStatistics->jittersumP += jitterStatistics->jitter; //lint !e737 TH if case prevents the loss
                ++jitterStatistics->sumcountP;
            }

            if((LSA_INT64)jitterStatistics->min > jitterStatistics->jitter) jitterStatistics->min = (LSA_INT32)jitterStatistics->jitter;
            if((LSA_INT64)jitterStatistics->max < jitterStatistics->jitter) jitterStatistics->max = (LSA_INT32)jitterStatistics->jitter;
        }
    }
    jitterStatistics->time = _time;
}
#endif

static void EDDS_BeginStatistics(EDDS_LOCAL_DDB_PTR_TYPE const pDDB,LSA_UINT8 const intermediateCall)
{
    EDDS_SYS_HANDLE const hSysDev = pDDB->hSysDev;
    EDDS_INTERNAL_PERFORMANCE_STATS_TYPE * const pPerformanceStatistics = &pDDB->PerformanceStatistics;
    EDDS_INTERNAL_SCHED_TYPE * const pScheduler = &pDDB->pGlob->scheduler;

    pScheduler->currentBeginNS = EDDS_GET_SYSTEM_TIME_NS(hSysDev);

    if (EDDS_SCHEDULER_INTERMEDIATE_CALL != intermediateCall)
    {
        /* begin statistics of cyclic call */
        EDDS_INTERNAL_STATISTIC_ENTER(hSysDev, pPerformanceStatistics->callsCyclic);
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
        EDDS_INTERNAL_STATISTICS_JITTER_GET(&pPerformanceStatistics->cylceJitter, pScheduler->currentBeginNS,(((LSA_UINT32)pScheduler->CycleTimeMS)*1000*1000));
#endif
    }
    else
    {
        /* begin statistics of intermediate call */
        EDDS_INTERNAL_STATISTIC_ENTER(hSysDev,
                                      pPerformanceStatistics->callsIntermediate);
    }
}


static void EDDS_EndStatistics(EDDS_LOCAL_DDB_PTR_TYPE const pDDB, LSA_UINT8 const intermediateCall)
{
    EDDS_INTERNAL_PERFORMANCE_STATS_TYPE * const pPerformanceStatistics = &pDDB->PerformanceStatistics;

    if(EDDS_SCHEDULER_INTERMEDIATE_CALL != intermediateCall )
    {
        /* end statistics of cyclic call */
        EDDS_INTERNAL_STATISTIC_EXIT(pDDB->hSysDev,pPerformanceStatistics->callsCyclic);
    }
    else
    {
        /* end statistics of intermediate call */
        EDDS_INTERNAL_STATISTIC_EXIT(pDDB->hSysDev,pPerformanceStatistics->callsIntermediate);
    }
}


/** \brief      Main function of EDDS scheduler context.
 *
 *  \details    For each EDDS instance there is one scheduler context. The scheduler context is thereby prior to the RQB context.
 *              Each EDDS instance is called with its own Device Description Block (hDDB). Typically the scheduler is called
 *              cyclically by system adaption for applications with IO and NRT.
 *              For throughput optimization for NRT the scheduler context can be called in between regular scheduler calls.
 *              This is done through a so called intermediate call. This is only allowed for an EDDS instance that is NOT using IO.
 *
 *  \param      hDDB                Device Description Block
 *  \param      intermediateCall    When not set, this is a regular scheduler call, otherwise a intermediate call.
 *
 *  \note       The EDDS scheduler as a cyclic call (intermediateCall = LSA_FALSE) should be called only once in a cycle, with the
 *              promised frequency from pDPB->schedulerCycleIO (IO mode), or pDPB->schedulerCycleNRT (NRT mode).
 *
 *  \author     am2219, adpath2
 *  \date       2015-12-02
 */
LSA_VOID EDD_UPPER_IN_FCT_ATTR
edds_scheduler (EDDS_HANDLE const hDDB, LSA_UINT8 const intermediateCall)
{
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    if(EDD_STS_OK != EDDS_GetDDB(hDDB, &pDDB))
    {
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
        EDDS_MODULE_ID,
        __LINE__);
    }

    EDDS_ENTER(pDDB->hSysDev);

    pGlob = pDDB->pGlob;

    EDDS_INTERNAL_STATISTIC_ENTER(pDDB->hSysDev, pDDB->PerformanceStatistics.callsAll);

    EDDS_SCHED_TRACE_02(pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN :edds_scheduler(pDDB: 0x%X, intermediateCall: 0x%X)", pDDB, intermediateCall);

    if(pGlob->HWIsSetup)
    {
        LSA_UINT32 tasksBitField;

        EDDS_BeginStatistics(pDDB,intermediateCall);

        /* if cyclic, then figure out required tasks and store in "tasksBitField" for
         * separated execution below.
         */

        /*================================================================*/
        /*=== get the tasks for this scheduler call ======================*/
        /*================================================================*/
        tasksBitField = EDDS_GetTaskBitField(pDDB,intermediateCall);

        /*================================================================*/
        /*=== start execute tasks ========================================*/
        /*================================================================*/

        /* --------------------------------------------------------------------------*/
        /* Determine the maximum amount of bytes to be sent in this cycle.           */
        /* NOTE: Depends on whether FILL is active or not.                           */
        /* NOTE: Tx bandwith includes ethernet frame trailer and header              */
        /* --------------------------------------------------------------------------*/
        if( EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS & tasksBitField )
        {
            pGlob->actTxBytesCycle = 0;

        }

        /* reset Tx bandwith limit */
        if( EDDS_SCHEDULER_BITFIELD_RESET_FILL & tasksBitField)
        {
            EDDS_ResetTxBandwithLimit(pDDB);
            pDDB->pGlob->lastResetTxBandwithTime = pGlob->scheduler.currentBeginNS;
        }

        /* currently CBF for all former sent frames will be called in this section */
        if( EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART1 & tasksBitField )
        {
            EDDS_TransmitLastCycleFinish(pDDB);
        }

        /* frame pre filters will be applied to received frames in this section */
        if( EDDS_SCHEDULER_BITFIELD_NRT_RCV & tasksBitField )
        {
            /* reload remaining Rx bandwith for a cylic call */
            /* NOTE: EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS will only be set in a cyclic call */
            if( EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS & tasksBitField )
            {
                pGlob->RxRemainingByteCntPerCycle = pGlob->RxMaxByteCntPerCycle;
                pGlob->actRxPacketsCycle = 0;  /* new cycle, reset send Rx Packets from (last) cycle. */
            }
            EDDS_ReceiveLastCycleFinish(pDDB);
        }

        /* EDDS_SCHEDULER_BITFIELD_SRT will only be set in a cyclic call */
        if( EDDS_SCHEDULER_BITFIELD_SRT & tasksBitField )
        {
            EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pSRT;
            pSRT  = pDDB->pSRT;
            if (0 == (++pSRT->schedulerCounter % pSRT->schedulerModuloFactorIO))
            {
                    EDDS_RTProviderCycleRequest(pDDB); /* trigger provider cycle */
                    EDDS_RTConsumerCycleRequest(pDDB); /* process consumer scoreboard */
            }
        }

        /* frames will be queued in LL and send is triggered in this section */
        if( EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART2 & tasksBitField )
        {
            EDDS_TransmitTrigger(pDDB, LSA_TRUE); /* trigger transmission for NRT only */
        }

        /* process RX pre queues and acknowledge RQB's */
        /* NOTE: all received frames (see call to EDDS_ReceiveLastCycleFinish) are handled here */
        /* frames filters will be applied and CBF will be called in this section */
        if( EDDS_SCHEDULER_BITFIELD_NRT_RCV & tasksBitField )
        {
            EDDS_NRTRxHandleFrames(pDDB);
        }

        /* EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS will only be set in a cyclic call */
        if( EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS & tasksBitField )
        {
            /* call recurring task on LL, e.g. for statistic counter handling
             * if high priority hardware programming is running, no normal pending RQBs should be executed!
             */
            EDDS_HandleRecurringTask(pDDB);
            EDDS_CheckInternalTimer(pDDB);  /* check for expired timers */
        }
        /* direct scheduler call in NRT mode:
         * handle general requests to guarantee low latency RQB processing experience.
         */
        else if(!((pDDB->pGlob->IO_Configured) || (pDDB->pGlob->NRT_UseNotifySchedulerCall)))
        {
            EDDS_HandleRequests(pDDB);
        }

        /* High priority hardware programming */
        if( EDDS_SCHEDULER_BITFIELD_HIGHPRIOR_RQB & tasksBitField)
        {
            EDDS_HandleHighPriorRequests(pDDB);
        }

        /*================================================================*/
        /*=== end execute tasks ==========================================*/
        /*================================================================*/

        /*================================================================*/
        /*=== proccess pending tasks =====================================*/
        /*================================================================*/
        EDDS_DoGeneralRequests(pDDB);

        /* complete statistics of cyclic or intermediate call*/
        EDDS_EndStatistics(pDDB,intermediateCall);

    }
    else
    {
      EDDS_SCHED_TRACE_01(pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"OUT :edds_scheduler(pDDB: 0x%X): waiting for setup finished.",pDDB);
    }

    EDDS_SCHED_TRACE_01(pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT :edds_scheduler(pDDB: 0x%X)",pDDB);

    /* complete statistic of all calls */
    EDDS_INTERNAL_STATISTIC_EXIT(pDDB->hSysDev, pDDB->PerformanceStatistics.callsAll);
    EDDS_EXIT(pDDB->hSysDev);
}
