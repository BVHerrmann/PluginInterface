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
/*  F i l e               &F: eddi_nrt_filter.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDI. Receive            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.07.07    JS    added support for PTCP_SYNC and PTCP_DELAY             */
/*  07.12.07    JS    with EDDI_NRT_FILTER_INDEX_PTCP_SYNC we store the      */
/*                    FrameID within FrameFilterInfo for later use           */
/*  29.01.08    JS    added EDDI_FRAME_STDBY_TAG. Support for STDBY frame    */
/*                    filter.                                                */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_nrt_inc.h"
#include "eddi_nrt_filter.h"

//#include "eddi_sync_usr.h"

#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)
#include "eddi_nrt_arp.h"
#endif

#if defined (EDDI_CFG_LITTLE_ENDIAN)

#define EDDI_FRAME_DSAP_SSAP             0xAAAA
#define EDDI_FRAME_DSAP_SSAP_DEFAULT     0xFDFD
#define EDDI_FRAME_CTRL_OUI1             0x03
#define EDDI_FRAME_OUI_SIEMENS           0x06000800
#define EDDI_FRAME_OUI_HIRSCHMANN        0x63800000

#define EDDI_FRAME_DCP_TAG               0xFD01
#define EDDI_FRAME_STDBY_TAG             0xFB01
#define EDDI_FRAME_TSYNC_TAG             0x0001
#define EDDI_FRAME_HSR_TAG               0x0008
#define EDDI_FRAME_SINEC_FWL_TAG         0x1001

#else //EDDI_CFG_BIG_ENDIAN

#define EDDI_FRAME_DSAP_SSAP             0xAAAA
#define EDDI_FRAME_DSAP_SSAP_FWL         0xFDFD
#define EDDI_FRAME_CTRL_OUI1             0x03
#define EDDI_FRAME_OUI_SIEMENS           0x080006
#define EDDI_FRAME_OUI_HIRSCHMANN        0x008063

#define EDDI_FRAME_DCP_TAG               0x01FD
#define EDDI_FRAME_STDBY_TAG             0x01FB
#define EDDI_FRAME_TSYNC_TAG             0x0100
#define EDDI_FRAME_HSR_TAG               0x0800
#define EDDI_FRAME_SINEC_FWL_TAG         0x0110

#endif

#define EDDI_MODULE_ID     M_ID_NRT_FILTER
#define LTRC_ACT_MODUL_ID  201

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_udp.h"

static LSA_UINT16 EDDI_RtoIPChecksumAndGetHeader( EDD_UPPER_MEM_U16_PTR_TYPE   pBuf,
                                                  LSA_UINT16                 * pIPHeader );

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_RtoFilterXRTCyclic( EDD_UPPER_MEM_U8_PTR_TYPE   const pFrame,
                                                               EDDI_UDP_XRT_FRAME_PTR_TYPE const pHeader,
                                                               LSA_UINT8                   const IPVersIHL,
                                                               LSA_UINT32                  const DLen,
                                                               EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );
#endif

#if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
static LSA_UINT32  EDDI_LOCAL_FCT_ATTR EDDI_NrtRxFilterDcp( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                            EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE const pNRTComp,
                                                            EDD_UPPER_MEM_U8_PTR_TYPE        const pFrame,
                                                            LSA_UINT32                       const Adjust,
                                                            LSA_UINT32                       const FrameLen );
#endif

#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_NrtRxFilterDcpHello( EDDI_LOCAL_DDB_PTR_TYPE            const pDDB, 
                                                                EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE   const pNRTComp,
                                                                EDD_UPPER_MEM_U8_PTR_TYPE          const pFrame,
                                                                EDDI_NRT_CHX_SS_IF_TYPE          * const pIF,
                                                                LSA_UINT32                         const Adjust,
                                                                LSA_UINT32                         const FrameLen );

static LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_NRTFrameMemcmp( EDD_UPPER_MEM_U8_PTR_TYPE   pFrame,
                                                         EDDI_LOCAL_MEM_U8_PTR_TYPE  pLocal,
                                                         LSA_UINT32                  Length );
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTRxCheckFrame()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          FilterIndex                                      */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxCheckFrame( EDD_UPPER_MEM_U8_PTR_TYPE     const  pFrame,
                                                       EDDI_NRT_CHX_SS_IF_TYPE    *  const  pIF,
                                                       LSA_UINT32                    const  FrameLen,
                                                       LSA_UINT32                 *  const  pUserDataOffset, //OUT
                                                       EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                       LSA_BOOL                   *  const  pbSyncID1 ) //OUT
{
    LSA_UINT32                      DLen;
    EDDI_TYPE_LEN_FRAME_ID_TYPE     FrameData;
    EDDI_IP_HEADER_TYPE          *  pIPHeader;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTRxCheckFrame->");

    *pbSyncID1 = LSA_FALSE;

    FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)(void *)pFrame)[3]; //Dest and Src start at offset 12

    if (pIF->RxFilterIFMACAddr)
    {
        if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)pFrame)[3] == ((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)pDDB->Glob.LLHandle.xRT.MACAddressSrc.MacAdr)[0]) &&
             (((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)pFrame)[4] == ((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)pDDB->Glob.LLHandle.xRT.MACAddressSrc.MacAdr)[1]) &&
             (((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)pFrame)[5] == ((EDD_UPPER_MEM_U16_PTR_TYPE)(void *)pDDB->Glob.LLHandle.xRT.MACAddressSrc.MacAdr)[2])
           )
        {
            return EDDI_NRT_FILTER_INDEX_GARBAGE;
        }
    }

    /* -----------------------------------------------------------------------*/
    /* get bytes 12..15 from frame                                            */
    /* Note that this is in inet-format (big endian)                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /* check for VLAN-Tag. if so we must read the real Type/len 4 Bytes ahead */
    /* -----------------------------------------------------------------------*/
    if (FrameData.w.TypeLen == EDDI_VLAN_TAG)
    {
        FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)(void *)pFrame)[4];
        *pUserDataOffset    = EDDI_FRAME_HEADER_SIZE_WITH_VLAN;
        DLen                = FrameLen - EDDI_FRAME_HEADER_SIZE_WITH_VLAN;
        pIPHeader           = (EDDI_IP_HEADER_TYPE *)(void *)&pFrame[EDDI_FRAME_HEADER_SIZE_WITH_VLAN];
    }
    else
    {
        *pUserDataOffset = EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN;
        DLen             = FrameLen - EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN;
        pIPHeader        = (EDDI_IP_HEADER_TYPE *)(void *)&pFrame[EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN];
    }

    /* -----------------------------------------------------------------------*/
    /* Now check for the Frametype                                            */
    /* -----------------------------------------------------------------------*/

    switch (FrameData.w.TypeLen)
    {
        case EDDI_xRT_TAG:
        {
            LSA_UINT16  FrameID;
            /* here a check for cyclic and acyclic SRT                     */
            /* Frame-ID ranges:                                            */
            /*                                                             */
            /* 0000...00FF:  Timesynchronisation                           */
            /* 0100...7FFF:  IRT-frames -> kommt hier nicht macht HW       */
            /* 8000...FBFF:  cyclic SRT  -> kommt hier nicht macht HW      */
            /* FC00...FCFF:  acyclic SRT high prio                         */
            /* FC01          PN-IO Alarm high                              */
            /* FD00...FDFF:  reserved                                      */
            /* FE00...FEFC:  acyclic SRT low  prio                         */
            /* FE01          PN-IO Alarm low                               */
            /* FEFE          DCP via xRT -> ??                             */
            /* FEFF          DCP via xRT -> ??                             */
            /* FF00...FFFF:  reserved                                      */
            /*                                                             */

            /* first we check for cyclic SRT, then for acyclic. all other  */
            /* frames will be dropped.                                     */

            FrameID = EDDI_NTOHS(FrameData.w.FrameID);

            if (((FrameID <= EDD_SRT_FRAMEID_PTCP_STOP_RANGE_0)) ||
                ((FrameID >= EDD_SRT_FRAMEID_PTCP_START_RANGE_1) && (FrameID <= EDD_SRT_FRAMEID_PTCP_STOP_RANGE_1)))
            {
                /*------------------------------------------------------ */
                /* a PTCP frame was received.                            */
                /*------------------------------------------------------ */
                if ( (FrameID >= EDD_SRT_FRAMEID_PTCP_DELAY_START) && (FrameID <= EDD_SRT_FRAMEID_PTCP_DELAY_END) )
                {
                    /* DELAY frame */
                    return EDDI_NRT_FILTER_INDEX_PTCP_DELAY;
                }
                else if( (FrameID >= EDD_SRT_FRAMEID_PTCP_ANNO_START) && (FrameID <= EDD_SRT_FRAMEID_PTCP_ANNO_END) )
                {
                    /* ANNOUNCE frame */
                    *pbSyncID1 = (0x1 == (FrameID & 0x1))?LSA_TRUE:LSA_FALSE;
                    return EDDI_NRT_FILTER_INDEX_PTCP_ANNO;
                }
                else
                {
                    /* all other frames */
                    pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_PTCP_SYNC].FrameFilterInfo = FrameID; /* save FrameID for later use */
                    *pbSyncID1 = (0x1 == (FrameID & 0x1))?LSA_TRUE:LSA_FALSE;
                    return EDDI_NRT_FILTER_INDEX_PTCP_SYNC;
                }
            }

            if (((FrameID >= EDD_SRT_FRAMEID_ASRT_START_LOW)  && (FrameID <= EDD_SRT_FRAMEID_ASRT_STOP_LOW)) ||
                ((FrameID >= EDD_SRT_FRAMEID_ASRT_START_HIGH) && (FrameID <= EDD_SRT_FRAMEID_ASRT_STOP_HIGH)))
            {
                /*------------------------------------------------------ */
                /* an aSRTP frame was received.                          */
                /*------------------------------------------------------ */
                return EDDI_NRT_FILTER_INDEX_ASRT;
            }

            if ((FrameID >= EDD_SRT_FRAMEID_CSRT_START) && (FrameID <= EDD_SRT_FRAMEID_CSRT_STOP))
            {
                /*------------------------------------------------------ */
                /* a cyclic RT frame was received.                       */
                /*------------------------------------------------------ */
                return EDDI_NRT_FILTER_INDEX_CSRT;
            }

            if ((FrameID >= EDDI_NRT_FRAMEID_FRAG_START) && (FrameID <= EDDI_NRT_FRAMEID_FRAG_END))
            {
                /*------------------------------------------------------ */
                /* a fragmented frame was received.                      */
                /*------------------------------------------------------ */
                return EDDI_NRT_FILTER_INDEX_NRT_FRAG;
            }

            /* acyclic SRT */
            /* check for DCP via xRT ( for speed we check for >= ) */
            /*------------------------------------------------------ */
            /* a NRT-OTHER DCP-frame was received.                   */
            /*------------------------------------------------------ */
            switch (FrameID)
            {
                case EDD_SRT_FRAMEID_ASRT_DCP_UNICAST:
                case EDD_SRT_FRAMEID_ASRT_DCP_MC_RSP:
                {
                    //no filtering!
                    return EDDI_NRT_FILTER_INDEX_DCP;
                }

                case EDD_SRT_FRAMEID_ASRT_DCP_MC_REQ:
                {
                    #if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
                    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = pDDB->pLocal_NRT;

                    if //SW-DCP-Filter active?
                       (pNRTComp->DCPFilterSwCount)
                    {
                        LSA_UINT32  Adjust = 0;

                        if (*pUserDataOffset == EDDI_FRAME_HEADER_SIZE_WITH_VLAN)
                        {
                            Adjust = EDD_VLAN_TAG_SIZE;
                        }

                        //filtering or no filtering!
                        return EDDI_NrtRxFilterDcp(pDDB, pNRTComp, pFrame, Adjust, FrameLen);
                    }
                    #endif

                    //no filtering!
                    return EDDI_NRT_FILTER_INDEX_DCP;
                }

                case EDD_SRT_FRAMEID_ASRT_DCP_HELLO:
                {
                    #if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
                    LSA_UINT32                               Adjust   = 0;
                    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp = pDDB->pLocal_NRT;

                    if (*pUserDataOffset == EDDI_FRAME_HEADER_SIZE_WITH_VLAN)
                    {
                        Adjust = EDD_VLAN_TAG_SIZE;
                    }

                    //filtering or no filtering!
                    return EDDI_NrtRxFilterDcpHello(pDDB, pNRTComp, pFrame, pIF, Adjust, FrameLen);
                    #else
                    //no filtering!
                    return EDDI_NRT_FILTER_INDEX_DCP_HELLO;
                    #endif
                }

                default:
                    //filtering!
                    return EDDI_NRT_FILTER_INDEX_GARBAGE;
            }
        }

        case EDDI_IP_TAG:
        {
            LSA_UNUSED_ARG(pDDB->Glob.LocalIP.dw);
            LSA_UNUSED_ARG(DLen);

            //access "protocol" field within frame
            switch (pIPHeader->Protocol)
            {
                case EDDI_IP_ICMP:
                    return EDDI_NRT_FILTER_INDEX_IP_ICMP;
                case EDDI_IP_IGMP:
                    return EDDI_NRT_FILTER_INDEX_IP_IGMP;
                case EDDI_IP_TCP:
                    return EDDI_NRT_FILTER_INDEX_IP_TCP;
                case EDDI_IP_VRRP:
                    return EDDI_NRT_FILTER_INDEX_IP_VRRP;
                case EDDI_IP_UDP:
                {
                    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
                    EDDI_UDP_XRT_FRAME_PTR_TYPE  const  pHeader = (EDDI_UDP_XRT_FRAME_PTR_TYPE)(void *)&pFrame[*pUserDataOffset];
                    if (pIF == pDDB->RTOUDP.pIFNrtUDP)
                    {
                        return EDDI_RtoFilterXRTCyclic(pFrame, pHeader, FrameData.bIP.IPVersIHL, DLen, pDDB);
                    }
                    #endif
                    return EDDI_NRT_FILTER_INDEX_IP_UDP;
                }

                default:
                    return EDDI_NRT_FILTER_INDEX_IP_OTHER;
            }
        }

        case EDDI_ARP_TAG:
        {
            #if !defined (EDDI_CFG_DISABLE_ARP_FILTER)
            #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)
            {
                NRT_ARP_FILTER_PTR_TYPE  const  pARPFilter = &pDDB->pLocal_NRT->ARPFilter;

                if //ARP-Filter active?
                   (   (pARPFilter->FilterCnt)
                    #if defined (EDDI_CFG_REV5_IF_IDX_EXCLUDED_FROM_ARP_FILTER)
                    && (pIF->QueueIdx != EDDI_CFG_REV5_IF_IDX_EXCLUDED_FROM_ARP_FILTER)
                    #endif
                   )
                {
                    //filtering or no filtering!
                    return EDDI_NRTCheckARPFilter(pDDB, pARPFilter, (EDDI_ARP_FRAME_PTR_TYPE)(void *)(pFrame + (*pUserDataOffset)));
                }
            }
            #endif
            #endif

            //no filtering!
            return EDDI_NRT_FILTER_INDEX_ARP;
        }

        case EDDI_RARP_TAG:
            return EDDI_NRT_FILTER_INDEX_RARP;

        case EDDI_LLDP_TAG:
            return EDDI_NRT_FILTER_INDEX_LLDP;

        case EDDI_MRP_TAG:
            return EDDI_NRT_FILTER_INDEX_MRP;

        case EDDI_HSYNC_TAG:
            return EDDI_NRT_FILTER_INDEX_HSYNC;

        default:
        {
            LSA_UINT16  const  TypeLenHost = EDDI_NTOHS(FrameData.w.TypeLen);

            if (TypeLenHost <= (LSA_UINT16)1500) /* 1500 is a decimal value, not hex!!! */
            {
                /*------------------------------------------------------ */
                /* a NRT-LEN frame was received.                         */
                /*------------------------------------------------------ */

                LSA_UINT16  SnapID = 0xFFFF; /* Mark SNAP-ID invalid */

                /* Check if at least a SNAP header and a SNAP-ID was reveived (i.e. 8 Bytes) */
                if (DLen >= 8)
                {
                    LSA_UINT16  DsapSsap;
                    LSA_UINT8   Ctrl;
                    LSA_UINT32  Oui;
                    /*                        DSAP   CTRL    OUI                        
                      Check for SNAP header: |AA AA| |03| |08 00 06| (hex) */
                    
                    #if defined (EDDI_CFG_LITTLE_ENDIAN)
                    DsapSsap = *(LSA_UINT16 *)(void *)&pFrame[*pUserDataOffset];
                    Ctrl = *(LSA_UINT8 *)(void *)&pFrame[*pUserDataOffset + 2];
                    Oui = *(LSA_UINT32 *)(void *)&pFrame[*pUserDataOffset + 2];
                    Oui &= 0xFFFFFF00;

                    if ((Ctrl == EDDI_FRAME_CTRL_OUI1) &&
                        ((Oui == EDDI_FRAME_OUI_SIEMENS) || 
                         (Oui == EDDI_FRAME_OUI_HIRSCHMANN)))
                    {
                        if (DsapSsap == EDDI_FRAME_DSAP_SSAP)
                        {
                            /* If SNAP header is ok, then the next 2 bytes are SNAP-ID: */
                            SnapID = *(LSA_UINT16 *)(void *)&pFrame[*pUserDataOffset + 6];
                        }
                        else if (DsapSsap == EDDI_FRAME_DSAP_SSAP_DEFAULT)
                        {
                            LSA_UINT16  const SnapIDLocal = *(LSA_UINT16 *)(void *)&pFrame[*pUserDataOffset + 6];
                            if (EDDI_FRAME_SINEC_FWL_TAG == SnapIDLocal)
                            {
                                //with DSAP_SSAP_DEFAULT only this snaptype is allowed!
                                SnapID = SnapIDLocal;
                            }
                        }
                    }           
                    #else //EDDI_CFG_BIG_ENDIAN
                    /*                        DSAP   CTRL    OUI                        
                      Check for SNAP header: |AA AA| |03| |08 00 06| (hex) */
                    DsapSsap  = *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 0] << 0x08;   
                    DsapSsap |= *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 1] << 0x00;   
                    Ctrl =      *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 2] << 0x00;   
                    Oui  =      *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 3] << 0x10;   
                    Oui |=      *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 4] << 0x08;   
                    Oui |=      *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 5] << 0x00;   

                    if ((DsapSsap == EDDI_FRAME_DSAP_SSAP) &&
                        (Ctrl == EDDI_FRAME_CTRL_OUI1) &&
                        ((Oui == EDDI_FRAME_OUI_SIEMENS) || 
                         (Oui == EDDI_FRAME_OUI_HIRSCHMANN)))
                    {
                        /* If SNAP header is ok, then the next 2 bytes are SNAP-ID: */
                        SnapID  = *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 6] << 0x08;
                        SnapID |= *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 7] << 0x00;
                    } 
                    else if ((DsapSsap == EDDI_FRAME_DSAP_SSAP_FWL) &&
                             (Ctrl == EDDI_FRAME_CTRL_OUI1) &&
                             (Oui == EDDI_FRAME_OUI_SIEMENS))
                    {
                        /* If SNAP header is ok, then the next 2 bytes are SNAP-ID: */
                        SnapID  = *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 6] << 0x08;
                        SnapID |= *(LSA_UINT8 *)(LSA_VOID *)&pFrame[*pUserDataOffset + 7] << 0x00;
                    }
                    #endif 
                }

                /* Now analyze SNAP-ID */
                switch (SnapID)
                {
                    case EDDI_FRAME_DCP_TAG:
                        return EDDI_NRT_FILTER_INDEX_LEN_DCP;

                    case EDDI_FRAME_TSYNC_TAG:
                        return EDDI_NRT_FILTER_INDEX_LEN_TSYNC;

                    case EDDI_FRAME_STDBY_TAG:
                        return EDDI_NRT_FILTER_INDEX_LEN_STDBY;

                    case EDDI_FRAME_HSR_TAG:
                        return EDDI_NRT_FILTER_INDEX_LEN_HSR;

                    case EDDI_FRAME_SINEC_FWL_TAG:
                        return EDDI_NRT_FILTER_INDEX_LEN_SINEC_FWL;

                    default:
                        return EDDI_NRT_FILTER_INDEX_LEN_OTHER;
                }
            }

            /*------------------------------------------------------ */
            /* a NRT-OTHER frame was received.                       */
            /*------------------------------------------------------ */
            return EDDI_NRT_FILTER_INDEX_OTHER;
        }
    } //end switch
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtRxFilterDcp()                            */
/*                                                                         */
/* D e s c r i p t i o n: executes DCP-Identify-Filter in SW               */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          NRT-FILTER-INDEX: EDDI_NRT_FILTER_INDEX_DCP      */
/*                                          EDDI_NRT_FILTER_INDEX_GARBAGE  */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_DCP_FILTER)
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_NrtRxFilterDcp( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                              EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE  const  pNRTComp,
                                                              EDD_UPPER_MEM_U8_PTR_TYPE         const  pFrame,
                                                              LSA_UINT32                        const  Adjust,
                                                              LSA_UINT32                        const  FrameLen )
{
    EDD_UPPER_MEM_U8_PTR_TYPE  const  pTLVFrame = &pFrame[EDDI_FRAME_DCP_TLV_OFFSET + Adjust]; //-> first TLV
    LSA_UINT16                        FrameDCPLength;
    LSA_UINT32                        DcpFilterRefCnt1;
    LSA_UINT32                        DcpFilterRefCnt2;
    LSA_UINT32                        Ctr1, Ctr2;
    LSA_UINT16                 const  FrameFilterType = *(EDD_UPPER_MEM_U16_PTR_TYPE)((LSA_VOID *)&pTLVFrame[EDDI_DCP_TLV_TYPE_OFFSET]);
    LSA_BOOL                          bFilterTypeMatch;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtRxFilterDcp->");

    /* ---------------------------------------------------------------*/
    /* Filtering is done as follows:                                  */
    /* We have to check the TLV-Filter if..                           */
    /* - it is a IDENTIFY MC-Request (FrameID check)                  */
    /* - AND a TLV is present (length > 0)                            */
    /* - AND the T-field is equal to the filter T-Field.              */
    /* If all is true, we check if the LV matches the Filter.         */
    /* If so, we forward the frame. If not, we drop it.               */
    /* Note: The buffer has a size of at least 60 bytes.              */
    /* ---------------------------------------------------------------*/

    //ATTENTION: Only the first TLV (filter entry) in the DCP-Identify-Request-frame must be considered by definition!

    //FrameID is already checked!
    //Note: Start of Length-Field has to be WORD-aligned!!!
    FrameDCPLength = EDDI_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)((LSA_VOID *)&pFrame[EDDI_FRAME_DCP_LEN_OFFSET + Adjust]));

    /* ----------------------------------------------------------*/
    /* Validate FrameDCPlength against frame-length              */
    /* ----------------------------------------------------------*/
    if ((FrameDCPLength + EDDI_FRAME_DCP_MC_HEADER_SIZE + Adjust) > FrameLen)
    {
        //invalid frame-length -> filtering!
        pNRTComp->DCPFilterInvalidFrameCount++;
        EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NrtRxFilterDcp, invalid DCP-Identify-frame (length) filtered, FrameFilterType:0x%X DCPFilterInvalidFrameCount:%u",
                          (LSA_UINT32)FrameFilterType, pNRTComp->DCPFilterInvalidFrameCount);
        return EDDI_NRT_FILTER_INDEX_GARBAGE;
    }

    //frame ok. Now check, if we have a TLV length big enough to hold a TLV-Field.
    if (FrameDCPLength < EDDI_DCP_TLV_TLSIZE)
    {
        //no filtering!
        return EDDI_NRT_FILTER_INDEX_DCP;
    }

    /* Check all filters. If at least one filter type matches, the T-field of the  */
    /* first TLV at least one filter must match. If not, the frame gets discarded. */
    /* If no filter-type matches the T-field at all, no filter will be applied.    */

    /* Check if Type of TLV-field matches Filter-TLV. Note that first TLV is */
    /* WORD-aligned so we can access with 16-Bit.                            */

    //ALL filter handling
    if (FrameFilterType == EDDI_DCP_FILTERTYPE_ALL) //optimized! (no EDDI_NTOHS() used because EDDI_DCP_FILTERTYPE_ALL = 0xFFFF)
    {
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtRxFilterDcp, frame with filter-type ALL received and not filtered.");

        //no filtering!
        return EDDI_NRT_FILTER_INDEX_DCP;
    }

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    #if !defined (EDDI_CFG_DEFRAG_ON)
    //NOS filter is realized in HW
    if (FrameFilterType == EDDI_DCP_FILTERTYPE_NOS) //optimized! (no EDDI_NTOHS() used because EDDI_DCP_FILTERTYPE_NOS = 0x0202)
    {
        //ignore SW-DCP-Filter => no filtering!
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtRxFilterDcp, REV6/7: frame with filter-type NOS received and not filtered.");

        //no filtering!
        return EDDI_NRT_FILTER_INDEX_DCP;
    }
    #endif //EDDI_CFG_DEFRAG_ON
    #endif //EDDI_CFG_REV6 || EDDI_CFG_REV7

    //consistency-check by RefCnt is necessary because this code is not completely protected by ENTER-macros!

    //RefCnt == even: SW-DCP-Filter-Update currently not in progress!
    //RefCnt == odd:  SW-DCP-Filter-Update currently in progress!

    DcpFilterRefCnt1 = pNRTComp->DCPFilterRefCnt;

    if //RefCnt1 odd (SW-DCP-Filter-Update currently in progress)?
       (DcpFilterRefCnt1 & 1UL)
    {
        //ignore SW-DCP-Filter => no filtering!
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtRxFilterDcp, SW-DCP-Filter-Update currently in progress, frame not filtered.");

        //no filtering!
        return EDDI_NRT_FILTER_INDEX_DCP;
    }
    else //SW-DCP-Filter-Update currently not in progress
    {
        bFilterTypeMatch = LSA_FALSE;

        for (Ctr1 = 0; Ctr1 < pNRTComp->DCPFilterSwCount; Ctr1++)
        {
            NRT_DCP_FILTER_TYPE         *  const  pDCPFilter = &pNRTComp->DCPFilter[Ctr1];
            EDDI_LOCAL_MEM_U8_PTR_TYPE            pTLVFilter = pDCPFilter->pDCP_TLVFilter;

            //gaps in SW-DCP-Filter are not allowed!
            if (pTLVFilter == EDDI_NULL_PTR)
            {
                EDDI_Excp("EDDI_NrtRxFilterDcp, gaps in SW-DCP-Filter are not allowed.", EDDI_FATAL_ERR_EXCP, pDDB, 0);
                return EDDI_NRT_FILTER_INDEX_GARBAGE;
            }

            //compare type-field
            if (EDDI_NTOHS(FrameFilterType) == pDCPFilter->Type)
            {
                LSA_UINT16  const  FilterValueLength = pDCPFilter->ValueLength;

                bFilterTypeMatch = LSA_TRUE;

                //compare length-field
                if (EDDI_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)((LSA_VOID *)&pTLVFrame[EDDI_DCP_TLV_LENGTH_OFFSET])) == FilterValueLength)
                {
                    EDD_UPPER_MEM_U8_PTR_TYPE  const  pTLVFrameValue = pTLVFrame + EDDI_DCP_TLV_VALUE_OFFSET; //point to Value-Field

                    pTLVFilter += EDDI_DCP_TLV_VALUE_OFFSET; //point to Value-Field

                    //now check Value-Field
                    for (Ctr2 = 0; Ctr2 < (LSA_UINT32)FilterValueLength; Ctr2++)
                    {
                        if (pTLVFrameValue[Ctr2] != pTLVFilter[Ctr2])
                        {
                            //leave for-loop
                            break;
                        }
                    }

                    if (Ctr2 != (LSA_UINT32)FilterValueLength)
                    {
                        //Value-Field doesn't match -> check next filter TLV
                        continue;
                    }

                    //We found a complete matching filter TLV
                    //no filtering!
                    EDDI_NRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NrtRxFilterDcp, DCP-Identify-frame not filtered, complete filter-TLV matches, FrameFilterType:0x%X", (LSA_UINT32)FrameFilterType);
                    return EDDI_NRT_FILTER_INDEX_DCP;
                }
            }
        }
    }

    //TLV not found in SW-DCP-Filter-Entry:
    //  - SW-DCP-Filter-consistency must still be checked!

    //check SW-DCP-Filter-consistency
    DcpFilterRefCnt2 = pNRTComp->DCPFilterRefCnt;

    if //SW-DCP-Filter inconsistent?
       (DcpFilterRefCnt2 != DcpFilterRefCnt1)
    {
        //ignore SW-DCP-Filter => no filtering!
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NrtRxFilterDcp, SW-DCP-Filter inconsistent, frame not filtered.");

        //no filtering!
        return EDDI_NRT_FILTER_INDEX_DCP;
    }
    else //SW-DCP-Filter consistent
    {
        if //at least 1 matching filter-type is found (but the corresponding complete filter TLV doesn't match)?
           (bFilterTypeMatch)
        {
            //filtering!
            pNRTComp->DCPFilterDropCount++;
            EDDI_NRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_NrtRxFilterDcp, DCP-Identify-frame filtered, filter-type matches but complete filter-TLV doesn't, FrameFilterType:0x%X DCPFilterDropCount:%u",
                              (LSA_UINT32)FrameFilterType, pNRTComp->DCPFilterDropCount);
            return EDDI_NRT_FILTER_INDEX_GARBAGE;
        }
        else
        {
            //no filtering!
            return EDDI_NRT_FILTER_INDEX_DCP;
        }
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NrtRxFilterDcpHello()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          NRT-FILTER-INDEX: EDDI_NRT_FILTER_INDEX_DCP_HELLO*/
/*                                          EDDI_NRT_FILTER_INDEX_GARBAGE  */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_NrtRxFilterDcpHello( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                   EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE     const  pNRTComp,
                                                                   EDD_UPPER_MEM_U8_PTR_TYPE            const  pFrame,
                                                                   EDDI_NRT_CHX_SS_IF_TYPE           *  const  pIF,
                                                                   LSA_UINT32                           const  Adjust,
                                                                   LSA_UINT32                           const  FrameLen )
{
    EDD_UPPER_MEM_U8_PTR_TYPE  pTLVFrame;
    LSA_UINT16                 FrameDCPLength;
    LSA_UINT16                 FilterValueLength;
    LSA_UINT32                 Ctr;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NrtRxFilterDcpHello->");

    /* Note: Start of Length-Field has to be WORD-aligned!!! */
    FrameDCPLength = EDDI_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)((LSA_VOID *)&pFrame[EDDI_FRAME_DCP_LEN_OFFSET + Adjust]));

    /* ----------------------------------------------------------*/
    /* Validate FrameDCPlength against frame-length              */
    /* ----------------------------------------------------------*/
    if ((FrameDCPLength + EDDI_FRAME_DCP_UC_HEADER_SIZE + Adjust) > FrameLen)
    {
        //invalid frame -> filtering!
        pNRTComp->DCPHelloFilterInvalidFrameCount++;
        return EDDI_NRT_FILTER_INDEX_GARBAGE;
    }

    /* frame ok. now check if we have a TLV   */
    /* length big enough to hold a TLV-Field? */
    if (FrameDCPLength < EDDI_DCP_TLV_TLSIZE)
    {
        //filtering!
        return EDDI_NRT_FILTER_INDEX_GARBAGE;
    }

    pTLVFrame = &pFrame[EDDI_FRAME_DCP_TLV_OFFSET + Adjust]; //-> first TLV

    /* check for NameOfStation TLV */
    if (   (pTLVFrame[EDDI_DCP_TLV_TYPE_OFFSET]     == EDDI_DCP_DEVICE_PROPERTIES_OPTION)
        && (pTLVFrame[EDDI_DCP_TLV_TYPE_OFFSET + 1] == EDDI_DCP_SUBOPTION_NAME_OF_STATION))
    {
        /* get length */
        FilterValueLength = EDDI_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)((LSA_VOID *)&pTLVFrame[EDDI_DCP_TLV_LENGTH_OFFSET]));

        /* length ok? (Value contains Blockinfo) */
        if (FilterValueLength >= EDDI_DCP_TLV_RES_BLOCKINFO_SIZE)
        {
            FilterValueLength -= EDDI_DCP_TLV_RES_BLOCKINFO_SIZE;                             /* Length of NameOfStation  */
            pTLVFrame         += EDDI_DCP_TLV_VALUE_OFFSET + EDDI_DCP_TLV_RES_BLOCKINFO_SIZE; /* Pointer to NameOfStation */

            /* Now search filter table for a matching entry */

            /* loop till matching filter found or end of filtertable */
            for (Ctr = 0; Ctr < EDD_DCP_MAX_DCP_HELLO_FILTER; Ctr++)
            {
                NRT_DCP_HELLO_FILTER_TYPE  *  const  pFilter = &pNRTComp->DCPHelloFilter[Ctr];

                if (   (pFilter->bInUse)
                    && (pFilter->StationNameLen == FilterValueLength))
                {
                    /* compare nameofstation */
                    if (EDDI_NRTFrameMemcmp(pTLVFrame, pFilter->StationName, FilterValueLength))
                    {
                        pIF->Rx.Filter[EDDI_NRT_FILTER_INDEX_DCP_HELLO].FrameFilterInfo = pFilter->UserID;
                        return EDDI_NRT_FILTER_INDEX_DCP_HELLO;
                    }
                }
            }
        }
    }

    /* Value don't match -> drop frame */
    pNRTComp->DCPHelloFilterDropCount++;
    LSA_UNUSED_ARG(pDDB);
    return EDDI_NRT_FILTER_INDEX_GARBAGE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTFrameMemcmp()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFrameMemcmp( EDD_UPPER_MEM_U8_PTR_TYPE   pFrame,
                                                            EDDI_LOCAL_MEM_U8_PTR_TYPE  pLocal,
                                                            LSA_UINT32                  Length )
{
    for (; Length; Length--)
    {
        if (*pFrame != *pLocal)
        {
            return LSA_FALSE;
        }
        pFrame++;
        pLocal++;
    }

    return LSA_TRUE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoFilterXRTCyclic()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_RtoFilterXRTCyclic( EDD_UPPER_MEM_U8_PTR_TYPE    const  pFrame,
                                                                  EDDI_UDP_XRT_FRAME_PTR_TYPE  const  pHeader,
                                                                  LSA_UINT8                    const  IPVersIHL,
                                                                  LSA_UINT32                   const  DLen,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    EDDI_IP_HEADER_TYPE  IPHeader; /* IPHeader cache */
    LSA_UINT32           IPLen;
    LSA_UINT32           UDPLen;
    LSA_UINT32           FrameID;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_RtoFilterXRTCyclic->");

    if (pHeader->Udp.DstPort != EDDI_xRT_UDP_PORT)
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP;
    }

    if (IPVersIHL != EDDI_IP_XRT_VER_IHL)
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP;
    }

    /* check the IP-Checksum and copy header to IPHeader */
    if (EDDI_RtoIPChecksumAndGetHeader((LSA_UINT16 *)(void *)pHeader, (LSA_UINT16 *)(void *)&IPHeader) != 0)
    {
        //pIF->stats.RxXRToUDPIPWrongChecksum++;
        return EDDI_NRT_FILTER_INDEX_GARBAGE;
    }

    IPLen  = (LSA_UINT32) EDDI_NTOHS(IPHeader.TotalLength);
    UDPLen = (LSA_UINT32) EDDI_NTOHS(pHeader->Udp.Length);

    /* verify some more header settings..        */
    /* Note: We dont check the UDP-Checksum      */
    /*       must be 0, else discarded!          */
    /* check: FragmentOffset (== 0)              */
    /* check: UDP-Src Port                       */
    /* check: Local IP Address (== DstIP) or MC  */
    /* check: No UDP-Checksum (==0)              */
    /* check: Verify length consistenz           */

    if ((IPHeader.FragmentOffset & EDDI_IP_FLAG_OFFSET_MSK) != EDDI_IP_FLAG_OFFSET_VAL)
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP; //normal IP frame
    }

    if ( ( IPHeader.DestIP.dw                != pDDB->Glob.LocalIP.dw ) &&
         ( (IPHeader.DestIP.b.IP[0] & 0xF0)  != 0xE0                  ) /* IP-Multicast 224..239 */  )
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP; //normal IP frame
    }

    if (IPLen > DLen)
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP; //normal IP frame
    }

    if (IPLen < EDDI_UDP_XRT_HEADER_SIZE)
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP; //normal IP frame
    }

    if (UDPLen != (IPLen - EDDI_IP_HEADER_SIZE))
    {
        return EDDI_NRT_FILTER_INDEX_IP_UDP; //normal IP frame
    }

    /* we have a valid xRToverUDP-frame */
    FrameID = EDDI_NTOHS(pHeader->FrameID); //get FrameID

    if ((FrameID >= EDD_SRT_FRAMEID_CSRT_START) && (FrameID <= EDD_SRT_FRAMEID_CSRT_STOP))
    {
        // cyclic xRToverUDP-frame -> will not be sent to user!
        EDDI_RtoProcessXRTCyclic(pFrame, FrameID, pHeader, pDDB);
        return EDDI_NRT_FILTER_INDEX_GARBAGE;
    }

    /* we have an asynchroneous xRToverUDP-frame */
    /* ALARM region ? */
    if (((FrameID >= EDD_SRT_FRAMEID_ASRT_START_LOW ) && (FrameID <= EDD_SRT_FRAMEID_ASRT_STOP_LOW)) ||
        ((FrameID >= EDD_SRT_FRAMEID_ASRT_START_HIGH) && (FrameID <= EDD_SRT_FRAMEID_ASRT_STOP_HIGH)))
    {
        return EDDI_NRT_FILTER_INDEX_ASRT;
    }

    /* no ALARM */
    return EDDI_NRT_FILTER_INDEX_IP_UDP; //normal IP Frame
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_RtoIPChecksumAndGetHeader()            +*/
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
/*+               Note: There is a "normal" checksum function within edd    +*/
/*+                     (EDDI_PChecksum()) used to fill a IP-Header!         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_UINT16  EDDI_RtoIPChecksumAndGetHeader( EDD_UPPER_MEM_U16_PTR_TYPE     pBuf,
                                                    LSA_UINT16                  *  pIPHeader )
{
    LSA_UINT32  Sum;
    LSA_UINT32  i;

    //#undef EDDI_CFG_IP_CHECKSUM_OPTIMIZE
    #if defined (EDDI_CFG_IP_CHECKSUM_OPTIMIZE)
    LSA_UINT32                  Help;
    EDD_UPPER_MEM_U32_PTR_TYPE  pHelp;
    #endif

    Sum = 0;

    #if defined (EDDI_CFG_IP_CHECKSUM_OPTIMIZE)
    /*------------------------------------------------------- */
    /* optimized checksum calc (reduced amount of framebuffer */
    /* accesses. pBuf[1] must be DWORD aligned! (is the case  */
    /* within an Ethernetframe). We assume 20 Byte IP Header  */
    /*------------------------------------------------------- */
    Sum = *pIPHeader++ = pBuf[0];

    pHelp = (EDD_UPPER_MEM_U32_PTR_TYPE)pBuf[1];

    for (i=0; i< 4; i++) /* (4*4 Byte = 16 Byte) */
    {
        Help  = *pHelp++;
        #if defined (EDDI_CFG_BIG_ENDIAN)
        Sum  += *pIPHeader++ = (LSA_UINT16) (Help >> 16);
        Sum  += *pIPHeader++ = (LSA_UINT16) Help;
        #else //EDDI_CFG_LITTLE_ENDIAN
        Sum  += *pIPHeader++ = (LSA_UINT16) Help;
        Sum  += *pIPHeader++ = (LSA_UINT16) (Help >> 16);
        #endif
    }

    Sum  = *pIPHeader++ = pBuf[9];
    /*------------------------------------------------------- */
    #else
    for (i=0; i < (EDDI_IP_HEADER_SIZE/2); i++)
    {
        Sum += *pIPHeader++ = *pBuf++;
    }
    #endif

    /* add carrys so we get ones-complement */
    Sum  = (Sum >> 16) + (Sum & 0xffff);
    Sum += (Sum >> 16);

    return ((LSA_UINT16)~Sum);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_IsUDPFrameToDrop()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_TRUE:  filter UDP Frame                      */ 
/*                        LSA_FALSE: pass to User, do not filter           */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_IsUDPFrameToDrop ( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                       LSA_UINT32                 const  MACTyp,
                                                       EDD_UPPER_MEM_U8_PTR_TYPE  const  pFrame,
                                                       LSA_UINT32                 const  UserDataOffset )
{
    EDDI_IP_HEADER_TYPE  *  const  pIPHeader = (EDDI_IP_HEADER_TYPE *)(void *)&pFrame[UserDataOffset];    
    EDDI_UDP_HEADER_TYPE *         pUDPHeader;    
    LSA_UINT32                     DstPort;
    LSA_UINT8                      IHL;
    LSA_BOOL                       ReturnValue=LSA_TRUE; // Drop frame

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_IsUDPFrameToDrop->");

    IHL = (LSA_UINT8)((pIPHeader->VersionIHL) & 0xF);
        
    if (IHL > 0x4) //IP Header Length is min. 5 
    {
        if (   (EDDI_NTOHS(pIPHeader->FragmentOffset) & EDDI_IP_OFFSET) != 0 ) /* last fragment */
        {
            // is IP-Fragmentation fragment
            ReturnValue = LSA_FALSE;
        }
        else
        {
            pUDPHeader = (EDDI_UDP_HEADER_TYPE *)(void *)&pFrame[UserDataOffset + (sizeof(LSA_UINT32) * IHL)];
            DstPort = (LSA_UINT32)EDDI_NTOHS(pUDPHeader->DstPort);

            switch (MACTyp)
            {
                case EDDI_UDP_FILTER_MAC_BROADCAST:
                {
                    if (   (EDDI_UDP_FILTER_DST_PORT_NTP_SNTP == DstPort) //NTP/SNTP
                        || (EDDI_UDP_FILTER_DST_PORT_DHCP_CLIENT==DstPort) //DHCP client
                        )
                    {
                        //Pass to User
                        ReturnValue=LSA_FALSE;
                    }
                    break;
                }

                case EDDI_UDP_FILTER_MAC_UNICAST:
                {
                    //Check UDP Whitelist's
                    if (   (EDDI_UDP_FILTER_DST_PORT_NTP_SNTP == DstPort)  //NTP/SNTP
                        || (EDDI_UDP_FILTER_DST_PORT_SNMP_1 == DstPort)    //SNMP
                        || (EDDI_UDP_FILTER_DST_PORT_SNMP_2 == DstPort)    //SNMP
                        || (EDDI_UDP_FILTER_DST_PORT_PNIO_EPM == DstPort)  //PNIO: EPM
                        || (EDDI_UDP_FILTER_DST_PORT_DHCP_CLIENT==DstPort) //DHCP client
                        || (    (DstPort >= EDDI_UDP_FILTER_DST_PORT_IANA_FREE_PORT_BEGIN)	   //IANA dynamic or private
                             && (DstPort <= EDDI_UDP_FILTER_DST_PORT_IANA_FREE_PORT_END)       //ports
                           )  
                       )
                    {
                        //Pass to User
                        ReturnValue=LSA_FALSE;
                    }
                    break;
                }
                case EDDI_UDP_FILTER_MAC_MULTICAST:
                    break;
                default:
                {
                    LSA_UNUSED_ARG(pDDB);
                    break;
                }
            }
        }
    }
    return ReturnValue;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_filter.c                                            */
/*****************************************************************************/



