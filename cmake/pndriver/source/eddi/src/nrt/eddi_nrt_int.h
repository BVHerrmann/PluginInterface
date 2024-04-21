#ifndef EDDI_NRT_INT_H          //reinclude-protection
#define EDDI_NRT_INT_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_nrt_int.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NRT-                                                                     */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.07.07    JS    added EDDI_NRT_FILTER_INDEX_PTCP_SYNC                  */
/*  24.07.07    JS    added EDDI_FRAME_BUFFER_ALIGN                          */
/*  29.01.08    JS    added EDDI_NRT_FILTER_INDEX_LEN_STDBY                  */
/*  28.09.09    UL    removed EDDI_FRAME_BUFFER_ALIGN                        */
/*  11.11.14    TH    removed StoredRqbStatus from _EDDI_NRT_TX_FRAG_DDB_TYPE*/
/*****************************************************************************/

#include "pnio_pck4_on.h"

#define EDDI_NRT_DB_LEN          120        //netto size

/*===========================================================================*/
/*========================= EDDI_SER_NRT_DB_TYPE ============================*/
/*===========================================================================*/
#define EDDI_SER_NRT_LL0_DB_BIT__CntPort     EDDI_BIT_MASK_PARA( 3, 0)
#define EDDI_SER_NRT_LL0_DB_BIT__Reserved    EDDI_BIT_MASK_PARA( 7, 4)
#define EDDI_SER_NRT_LL0_DB_BIT__pNext       EDDI_BIT_MASK_PARA(28, 8)
#define EDDI_SER_NRT_LL0_DB_BIT__Res1        EDDI_BIT_MASK_PARA(31,29)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_LL0_DB_TYPE
{
    EDDI_SER10_UINT64_TYPE   Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_LL0_DB_TYPE EDDI_SER_NRT_LL0_DB_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_NRT_DB_TYPE
{
    EDDI_SER_NRT_LL0_DB_TYPE  Hw0;
    EDDI_SER10_UINT64_TYPE    Hw[(SER10_NRT_DB_SIZE / sizeof(EDDI_SER10_UINT64_TYPE))];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_NRT_DB_TYPE EDDI_SER_NRT_DB_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_NRT_DB_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_NRT_DB_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*===================== EDDI_SER_NRT_TAG_TYPE ===============================*/
/*===========================================================================*/
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_PRIO_HEAD_TYPE
{
    LSA_UINT32     Top;
    LSA_UINT32     Bottom;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_PRIO_HEAD_TYPE EDDI_SER_NRT_PRIO_HEAD_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_NRT_PRIO_ENTRY_TYPE
{
    EDDI_SER10_UINT64_TYPE       Value;
    EDDI_SER_NRT_PRIO_HEAD_TYPE  p;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_NRT_PRIO_ENTRY_TYPE EDDI_SER_NRT_PRIO_ENTRY_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_PRIO_TYPE
{
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  Tag_1_2; // Prio0
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  Tag_0_3; // Prio1
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  Tag_4_5; // Prio2
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  Tag_6_7; // Prio3
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  aSRTLow; // Prio4
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  cSRT;    // Prio5
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  aSRTHigh;// Prio6
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  iSRT;    // Prio7
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  ORG;     // Prio8
    EDDI_SER_NRT_PRIO_ENTRY_TYPE  Fluss;   // Prio9

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_PRIO_TYPE EDDI_SER_NRT_PRIO_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_NRT_PRIO_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_NRT_PRIO_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*===================== EDDI_SER_NRT_HEAD_TYPE ==============================*/
/*===========================================================================*/
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_CH_SND_TYPE
{
    LSA_UINT32     A;
    LSA_UINT32     B;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_CH_SND_TYPE EDDI_SER_NRT_CH_SND_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_NRT_CH_TYPE
{
    EDDI_SER10_UINT64_TYPE    Value;
    EDDI_SER_NRT_CH_SND_TYPE  CH;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_NRT_CH_TYPE EDDI_SER_NRT_CH_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_P0_P1_SND_TYPE
{
    LSA_UINT32     N0;
    LSA_UINT32     N1;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_P0_P1_SND_TYPE EDDI_SER_NRT_P0_P1_SND_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_NRT_P0_P1_TYPE
{
    EDDI_SER10_UINT64_TYPE       Value;
    EDDI_SER_NRT_P0_P1_SND_TYPE  P;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_NRT_P0_P1_TYPE EDDI_SER_NRT_P0_P1_TYPE;

#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_SOC)
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_P2_P3_SND_TYPE
{
    LSA_UINT32     N2;
    LSA_UINT32     N3;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_P2_P3_SND_TYPE EDDI_SER_NRT_P2_P3_SND_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_NRT_P2_P3_TYPE
{
    EDDI_SER10_UINT64_TYPE        Value;
    EDDI_SER_NRT_P2_P3_SND_TYPE   P;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_NRT_P2_P3_TYPE EDDI_SER_NRT_P2_P3_TYPE;

//used for passing arguments to 2nd processor 
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_ARGPASS_TYPE
{
    LSA_UINT32                    CycleCtrHigh;
    LSA_UINT32                    res0;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_ARGPASS_TYPE EDDI_SER_NRT_ARGPASS_TYPE;

#endif

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_HEAD_TYPE
{
    EDDI_SER_NRT_CH_TYPE        Hw0;
    EDDI_SER_NRT_P0_P1_TYPE     Hw1;
    #if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_SOC)
    EDDI_SER_NRT_P2_P3_TYPE     Hw2;
    #endif
    #if defined (EDDI_CFG_REV5)
    EDDI_SER_NRT_ARGPASS_TYPE   Arg;
    #endif
    #if defined (EDDI_CFG_ENABLE_FAST_SB_TIMING)
    LSA_UINT32                  TimeMin;
    LSA_UINT32                  Time;
    LSA_UINT32                  TimeMax;
    LSA_UINT32                  res0;
    #endif

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_HEAD_TYPE EDDI_SER_NRT_HEAD_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_NRT_HEAD_TYPE  EDDI_LOWER_MEM_ATTR  *   EDDI_SER_NRT_HEAD_PTR_TYPE;
/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================== EDDI_SER_DMACW_TYPE ==============================*/
/*===========================================================================*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_DMACW_TYPE
{
    PNIO_PACKED_ATTRIBUTE_PRE struct _L0
    {
        LSA_UINT32                Reg;
    } PNIO_PACKED_ATTRIBUTE_POST L0;

    LSA_UINT32            pNext;
    LSA_UINT32            Time;
    LSA_UINT32            pDataBuffer;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_DMACW_TYPE EDDI_SER_DMACW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_DMACW_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_DMACW_PTR_TYPE;

#include "pnio_pck_off.h" 
/*===========================================================================*/

//typedef struct _NRT_LINK_BASE_TYPE EDDI_LOCAL_MEM_ATTR * NRT_LINK_BASE_TYPE_PTR;
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* NRT-DSCR-Structure                                                        */
/*---------------------------------------------------------------------------*/
typedef struct _NRT_DSCR_TYPE EDDI_LOWER_MEM_ATTR * NRT_DSCR_PTR_TYPE;

typedef struct _NRT_DSCR_TYPE
{
    NRT_DSCR_PTR_TYPE           next_dscr_ptr;  /* pointer to next DSCR     */
    NRT_DSCR_PTR_TYPE           prev_dscr_ptr;  /* pointer to previous DSCR */

    EDD_UPPER_RQB_PTR_TYPE      pSndRQB;

    EDDI_SER_DMACW_PTR_TYPE     pDMACW;         /* pointer to DMACW */
    LSA_UINT32                  pAsicDMACW;     /* pointer to DMACW */
    EDDI_DEV_MEM_U8_PTR_TYPE    pRcvDataBuffer;

} NRT_DSCR_TYPE;

/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* NRT-CHX-SSX-Structure                                                     */
/*---------------------------------------------------------------------------*/
#define NRT_MAX_PRIO_QUEUE              9

//Attention: if changing numbers also adapt HSYNCQueueSwapper[]!
#define NRT_MGMT_HIGH_PRIO_INDEX        0
#define NRT_HSYNC1_HIGH_PRIO_INDEX      1
#define NRT_HSYNC2_HIGH_PRIO_INDEX      2
#define NRT_MGMT_LOW_PRIO_INDEX         3
#define NRT_HIGH_PRIO_INDEX             4
#define NRT_MEDIUM_PRIO_INDEX           5
#define NRT_HSYNC1_LOW_PRIO_INDEX       6
#define NRT_HSYNC2_LOW_PRIO_INDEX       7
#define NRT_LOW_PRIO_INDEX              8

typedef struct _NRT_CHX_SSX_TYPE
{
    NRT_DSCR_PTR_TYPE         pTop;     /* queue of DSCRs  */
    LSA_UINT32                Cnt;      /* number of DSCRs */

    NRT_DSCR_PTR_TYPE         pReq;     /* pointer to new Req-DSCRs  -> initialized with Dscr.pTop */
    NRT_DSCR_PTR_TYPE         pReqDone; /* pointer to Req-Done-DSCRs -> initialized with Dscr.pTop */
    NRT_DSCR_PTR_TYPE         pNRTDscrArray;
    EDDI_SER_DMACW_PTR_TYPE   pDMACWArray;

} NRT_CHX_SSX_TYPE;

typedef struct _NRT_CHX_SSX_TYPE EDDI_LOWER_MEM_ATTR * NRT_CHX_SSX_PTR_TYPE;
/*===========================================================================*/

typedef LSA_UINT32 ( EDDI_SYSTEM_OUT_FCT_ATTR  *DEV_NRT_ADR_CONVERT_FCT) ( EDDI_SYS_HANDLE                    hSysDev,
                                                                           LSA_VOID                  * const  p,
                                                                           LSA_UINT32                  const  location );

/*---------------------------------------------------------------------------*/
/* NRT-CHX-RX-TX-Structure                                                   */
/*---------------------------------------------------------------------------*/

#define EDDI_MAX_DCP_FILTER_STRING_BUFFER_LEN   256                                         //do not change! EDDI-limitation (Rev6/7: for KRAM-saving)! limit for all DCP filter strings!
                                                                                            //length in bytes! must be a multiple of 8!
#define EDDI_MAX_DCP_FILTER_TLV_LEN             (EDDI_MAX_DCP_FILTER_STRING_BUFFER_LEN + 4) //do not change! length in bytes!

typedef struct _EDDI_NRT_RX_USER_TYPE
{
    EDD_RQB_TYPE                                *pFirstRQB;
    EDD_RQB_TYPE                                *pLastRQB;
    struct _EDDI_HDB_TYPE EDDI_LOCAL_MEM_ATTR   *pHDB;          /* HDB-Pointer */
    LSA_UINT32                                   RxQueueReqCnt; /* number of Rx-Requests */

} EDDI_NRT_RX_USER_TYPE;

typedef struct _EDDI_NRT_RX_FILTER_TYPE
{
    EDDI_NRT_RX_USER_TYPE      *pRxUser;
    LSA_UINT32                  Filter;
    LSA_UINT32                  FrameFilterInfo;

} EDDI_NRT_RX_FILTER_TYPE;

/* for DCP-Filtering of xRT-DCP-Frames */
typedef struct _NRT_DCP_FILTER_TYPE
{
    EDDI_LOCAL_MEM_U8_PTR_TYPE  pDCP_TLVFilter;                             //if <> EDDI_NULL_PTR: Filter for xRT-DCP-Frames (for EDD_NRT_FRAME_DCP)
    LSA_UINT16                  Type;                                       //Type of TLV-Field, network-format!
    LSA_UINT16                  ValueLength;                                //Length of Value-Field, network-format!
    LSA_UINT8                   DCP_TLVBuffer[EDDI_MAX_DCP_FILTER_TLV_LEN]; //Filter-buffer for DCP-Filter for xRT-DCP-Frames (for EDD_NRT_FRAME_DCP)

} NRT_DCP_FILTER_TYPE;

/* -------------------------------------------------------------------------*/
/* for DCP-Filtering of xRT-DCP-Frames (Hello)                              */
/* -------------------------------------------------------------------------*/
typedef struct _NRT_DCP_HELLO_FILTER_TYPE
{
    volatile  LSA_BOOL          bInUse;         /* Filter in Use?           */
    LSA_UINT32                  UserID;         /* UserID if in use         */
    LSA_UINT8                   StationName[EDD_DCP_MAX_DCP_HELLO_NAME_OF_STATION];
    LSA_UINT32                  StationNameLen; /* Length of StationName    */
    EDDI_LOCAL_HDB_PTR_TYPE     pHDB;

} NRT_DCP_HELLO_FILTER_TYPE;

/* -------------------------------------------------------------------------*/
/* Normal User-Filters                                                      */
/* -------------------------------------------------------------------------*/

// Note: The following filters must not be changed in sequence and must
//       apply to the bit-sequence within edd_usr.h framefilters!
//       see eddi_nrt_oc.c. eg. ARP must be #1 etc.
     
#define EDDI_NRT_FILTER_INDEX_ARP              0
#define EDDI_NRT_FILTER_INDEX_LEN_DCP          1
#define EDDI_NRT_FILTER_INDEX_LEN_TSYNC        2
#define EDDI_NRT_FILTER_INDEX_LEN_OTHER        3
#define EDDI_NRT_FILTER_INDEX_OTHER            4
#define EDDI_NRT_FILTER_INDEX_DCP              5
#define EDDI_NRT_FILTER_INDEX_LLDP             6
#define EDDI_NRT_FILTER_INDEX_ASRT             7
//#define EDDI_NRT_FILTER_INDEX_UDP_ASRT       8
//#define EDDI_NRT_FILTER_INDEX_UDP_DCP        9
#define EDDI_NRT_FILTER_INDEX_PTCP_SYNC        10
#define EDDI_NRT_FILTER_INDEX_MRP              11
#define EDDI_NRT_FILTER_INDEX_IP_ICMP          12
#define EDDI_NRT_FILTER_INDEX_IP_IGMP          13
#define EDDI_NRT_FILTER_INDEX_IP_TCP           14
#define EDDI_NRT_FILTER_INDEX_IP_UDP           15
#define EDDI_NRT_FILTER_INDEX_IP_VRRP		   16
#define EDDI_NRT_FILTER_INDEX_IP_OTHER         17
#define EDDI_NRT_FILTER_INDEX_RARP             18
#define EDDI_NRT_FILTER_INDEX_DCP_HELLO        19
//#define EDDI_NRT_FILTER_INDEX_UDP_DCP_HELLO  20
#define EDDI_NRT_FILTER_INDEX_LEN_STDBY        21
#define EDDI_NRT_FILTER_INDEX_LEN_HSR          22
#define EDDI_NRT_FILTER_INDEX_PTCP_ANNO        23
#define EDDI_NRT_FILTER_INDEX_PTCP_DELAY       24
#define EDDI_NRT_FILTER_INDEX_LEN_SINEC_FWL    25
#define EDDI_NRT_FILTER_INDEX_HSYNC			   26
#define EDDI_NRT_FILTER_INDEX_NRT_FRAG         27

/* new user filters must be added here! */    
#define EDDI_NRT_FILTER_INDEX_USER_LAST        EDDI_NRT_FILTER_INDEX_NRT_FRAG

/* Special Filters.. */

#define EDDI_NRT_FILTER_INDEX_CSRT             28

/* EDDI_NRT_FILTER_INDEX_PTCP_SYNC is an internal filter */
/* used with EDD_SRV_SYNC_RECV !                         */

#define EDDI_NRT_FILTER_INDEX_GARBAGE   (EDDI_NRT_FILTER_INDEX_CSRT + 1)
#define EDDI_NRT_FILTER_INDEX_LAST      (EDDI_NRT_FILTER_INDEX_GARBAGE)

/* -------------------------------------------------------------------------*/

typedef struct _EDDI_NRT_CHX_IF_STATS_TYPE
{
    LSA_UINT32    RxXRToUDPIPWrongChecksum;

} EDDI_NRT_CHX_IF_STATS_TYPE;

#define EDDI_NRT_MAX_DCP_FILTER_CNT     (EDDI_MAX_IRTE_PORT_CNT + 1)

typedef enum _EDDI_NRT_STATE_DEF_ALLOC
{
    DEF_ALLOC_DONT_CARE = 2,
    DEF_ALLOC_CLOSE,
    DEF_ALLOC_OPEN

} EDDI_NRT_STATE_DEF_ALLOC;

typedef struct _EDDI_NRT_SND_LIMIT_CNT_TYPE
{
    LSA_UINT32                        BytesPerCycle;
    LSA_UINT32                        BytesPerCycleLMH;

    LSA_UINT32                        BytesPerPrio[NRT_MAX_PRIO_QUEUE];

} EDDI_NRT_SND_LIMIT_CNT_TYPE;

typedef struct _EDDI_NRT_SND_LIMIT_TYPE
{
    LSA_UINT32                        MaxFrameLen[NRT_MAX_PRIO_QUEUE];

    LSA_UINT32                        BytesPerCycle;
    LSA_UINT32                        BytesPerCycleLMH;

    LSA_UINT32                        Prio_x_BytesPerMs[NRT_MAX_PRIO_QUEUE];

    LSA_UINT32                        SllReduction;     //reduction for resetting SLL (send load limitation)
    LSA_UINT32                        ActRedCnt;

    EDDI_NRT_SND_LIMIT_CNT_TYPE       ActCnt;

} EDDI_NRT_SND_LIMIT_TYPE;

#if defined (EDDI_CFG_FRAG_ON)
//maximum number of TX Fragments
#define MAX_TX_FRAG_NUMBER          (16UL)

typedef struct _EDDI_NRT_TX_FRAG_SEND_DATA_TYPE
{
    LSA_UINT32                      __FramelengthInDBs;
    LSA_UINT8                    *  pNRTFragBuffer;
    LSA_UINT32                      TmpNRTPrivFragBuffer;

} EDDI_NRT_TX_FRAG_SEND_DATA_TYPE;

typedef struct _EDDI_NRT_TX_FRAG_PRE_DATA_TYPE
{
    //Outputs of EDDI_NRTCalcFragments
    LSA_UINT32                      PDUHeaderSize;                              //header size of original PDU (excludes Ethertyp)
    LSA_UINT32                      FragCnt;                                    //number of required fragments for the current NRT send service
    LSA_UINT32                      FragBufNettoSize;                           //maximal netto size of the fragment frame buffers without FCS (4-byte-aligned)
    LSA_UINT32                      BigFragBufTotalSize;                        //total size of the big fragmentation buffer for the current NRT send service
    LSA_UINT8                       FragPayloadLenArray[MAX_TX_FRAG_NUMBER];    //Frag Payload Length in bytes for each fragment

    //Inputs for EDDI_NRTBuildFragments
    LSA_UINT8                    *  pDstNRTFragBuffer;                          //pointer to big fragment buffer used for 1 NRT send service

} EDDI_NRT_TX_FRAG_PRE_DATA_TYPE;
#endif

struct _EDDI_NRT_CHX_SS_IF_TYPE;

typedef  LSA_VOID  ( EDDI_LOCAL_FCT_ATTR  *  NRT_SEND_FCT )   ( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                struct _EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );

typedef  LSA_VOID  ( EDDI_LOCAL_FCT_ATTR  *  EDDI_LOCK_FCT )  ( struct _EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );

typedef struct _EDDI_NRT_SND_IF_TYPE
{
    NRT_CHX_SSX_TYPE                Dscr;                                   //NRT-IF-Snd

    EDDI_RQB_QUEUE_TYPE             SndReq[NRT_MAX_PRIO_QUEUE];             //queue for commands, requestqueue
    LSA_UINT32                      QueuedSndReq;                           //counts queued RQBs in SendQueue of this NRT interface (e.g. A0) (caused by SLL)
    LSA_UINT32                      PrioQueueArrayStartIndex;               //dynamic start index for SendQueue check (= SndReq[]). End is always MaxPrioQueueIndex 
    LSA_UINT8                       PrioQueueArrayIndex[NRT_MAX_PRIO_QUEUE];//contains prioqueue numbers for this IF (A:NRT_HIGH/MEDIUM/LOW_PRIO_INDEX, 
                                                                            //B: NRT_MGMT_HIGH/HSYNC1_HIGH/MGMT_LOW/HSYNC1_LOW_PRIO_INDEX)
    LSA_UINT8                       RevPrioQueueArrayIndex[NRT_MAX_PRIO_QUEUE];//contains PrioQueueIndex in reverse for this IF 
    LSA_UINT8                       MaxPrioQueueArrayIndex;                 //Max index for PrioQueueIndex for this IF
    LSA_UINT8                       NextHSYNCPrioQueueHigh;                 //changes between NRT_HSYNC1_HIGH_PRIO_INDEX and NRT_HSYNC2_HIGH_PRIO_INDEX
    LSA_UINT8                       NextHSYNCPrioQueueLow ;                 //changes between NRT_HSYNC1_LOW_PRIO_INDEX and NRT_HSYNC2_LOW_PRIO_INDEX
    #if defined (EDDI_CFG_FRAG_ON)
    EDDI_NRT_TX_FRAG_SEND_DATA_TYPE SndReqFragData[NRT_MAX_PRIO_QUEUE];     //send data necessary for TX fragmentation
    LSA_UINT32                      QueuedPreSndReq;                        //counts queued RQBs in PreSendQueue of this NRT interface (e.g. A0) (caused by FRAG)
    EDDI_RQB_QUEUE_TYPE             PreSndReq[NRT_MAX_PRIO_QUEUE];          //pre send queue necessary for TX fragmentation
    EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  PreSndReqFragData[NRT_MAX_PRIO_QUEUE];  //pre send data necessary for TX fragmentation
    EDDI_RQB_QUEUE_TYPE             FragDeallocQueue;                       //contains fragmentation buffers for later deallocation
    #endif

    LSA_UINT32                      BalanceSndReq;                          //counts currently occupied TX-DMACW-entries

    EDDI_LOCK_FCT                   LockFct;
    EDDI_LOCK_FCT                   UnLockFct;

    EDDI_NRT_SND_LIMIT_TYPE         SndLimit;
} EDDI_NRT_SND_IF_TYPE;

typedef  LSA_VOID  ( EDDI_LOCAL_FCT_ATTR  *  EDDI_NRT_RCV_FCT )  ( struct _EDDI_DDB_TYPE            *  const  pDDB,
                                                                   struct _EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );

typedef struct _EDDI_NRT_RCV_IF_TYPE
{
    NRT_CHX_SSX_TYPE           Dscr;    /* NRT-IF-Rcv */

    LSA_UINT32                 Queued_RxCount;
    LSA_UINT32                 UsedFilter;
    EDDI_NRT_RX_FILTER_TYPE    Filter[EDDI_NRT_FILTER_INDEX_LAST];
    EDDI_NRT_RX_USER_TYPE      User[EDDI_NRT_FILTER_INDEX_LAST];

    EDDI_NRT_RCV_FCT           DoneFct;
    EDDI_LOCK_FCT              LockFct;
    EDDI_LOCK_FCT              UnLockFct;

    LSA_UINT32                 MaxRcvFrame_SendToUser;
    LSA_UINT32                 MaxRcvFrame_ToLookUp;

    #if defined (EDDI_CFG_DEFRAG_ON)   
    LSA_UINT32                 MaxRcvFragments_ToCopy;
    #endif
    
} EDDI_NRT_RCV_IF_TYPE;

typedef struct _EDDI_NRT_CHX_SS_IF_TYPE
{
    LSA_INT32                        Lock_Sema_SND;
    LSA_INT32                        Lock_Sema_RCV;

    EDDI_NRT_SND_IF_TYPE             Tx;
    EDDI_NRT_RCV_IF_TYPE             Rx;

    LSA_UINT32                       HOLCount_CH;

    EDDI_MEMORY_MODE_TYPE            MemModeBuffer;   /* Memory-Mode: KRAM, MPRAM, PC-Shared-Memory   */
    EDDI_MEMORY_MODE_TYPE            MemModeDMACW;    /* Memory-Mode: KRAM, MPRAM, PC-Shared-Memory   */
    EDDI_NRT_STATE_DEF_ALLOC         StateDeferredAlloc;

    EDDI_USERMEMID_TYPE              UserMemIDDMACWDefault;
    EDDI_USERMEMID_TYPE              UserMemIDTXDefault;
    EDDI_USERMEMID_TYPE              UserMemIDRXDefault;
    EDDI_USERMEMID_TYPE              UserMemIDTXFragArray[NRT_MAX_PRIO_QUEUE];

    LSA_UINT32                       CntOpenChannel;

    LSA_UINT32                       LimitDown_BufferAddr;
    LSA_UINT32                       LimitUp_BufferAddr;

    LSA_UINT32                       Value_for_IO_NRT_ENABLE;
    LSA_UINT32                       Value_for_IO_HOL_COUNT_CH;
    LSA_UINT32                       Value_for_IO_NRT_SND_DESCRIPTOR;
    LSA_UINT32                       Value_for_NRT_Queue_ENABLE;         //bitfield for enabling NRT channel in IRTE-register SS_QUEUE_DISABLE
    LSA_UINT32                       Value_for_NRT_Queue_DISABLE;        //bitfield for disabling NRT channel in IRTE-register SS_QUEUE_DISABLE
    LSA_UINT32                       Value_for_IO_NRT_TX_Interrupt_IRTE; //selector for nrt-tx-done-interrupt-bit in IRTE format (= little endian)
    LSA_UINT32                       Value_for_IO_NRT_RX_Interrupt_IRTE; //selector for nrt-rx-done-interrupt-bit in IRTE format (= little endian)

    #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) || defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
    volatile  LSA_UINT32             NewCycleReductionFactorForNRTCheckRxLimit; //factor for NRT-RX-timeslice-emulation for this NRT-Channel, 0 = not selected
    volatile  LSA_UINT32  *          pNewCycleReductionCtrForNRTCheckRxLimit;   //pointer to counter for NRT-RX-timeslice-emulation for this NRT-Channel
    volatile  LSA_UINT32  *          pNRT_RX_Limit_MaskIRTE;                    //pointer to dynamic NRT-RX-interrupt-mask for this NRT-Channel
    #endif

    DEV_NRT_ADR_CONVERT_FCT          adr_to_asic;
    DEV_NRT_ADR_CONVERT_FCT          adr_to_asic_dmacw;

    EDDI_NRT_CHX_IF_STATS_TYPE       stats;

    LSA_UINT32                       QueueIdx;
    LSA_UINT32                       NRTChannel;

    LSA_BOOL                         RxFilterUDP_Unicast;
    LSA_BOOL                         RxFilterUDP_Broadcast;
    LSA_BOOL                         RxFilterIFMACAddr;

} EDDI_NRT_CHX_SS_IF_TYPE;

//typedef struct _EDDI_NRT_CHX_SS_IF_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_NRT_CHX_SS_IF_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* NRT handle (channel) management-structures                                */
/*---------------------------------------------------------------------------*/

//typedef struct _EDDI_HDB_COMP_NRT_TYPE    EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_HDB_COMP_NRT_PTR_TYPE;
/*===========================================================================*/

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

#if (EDDI_MAX_ARP_FILTER <= 2)
#define MAX_ARP_IP_FILTER   2
#elif (EDDI_MAX_ARP_FILTER  <= 4)
#define MAX_ARP_IP_FILTER   4
#elif (EDDI_MAX_ARP_FILTER  <= 6)
#define MAX_ARP_IP_FILTER   6
#elif (EDDI_MAX_ARP_FILTER  <= 8)
#define MAX_ARP_IP_FILTER   8
#endif

//temporarily_disabled_lint -esym(768, _NRT_ARP_FILTER_TABLE_TYPE::entry)
typedef struct _NRT_ARP_FILTER_TABLE_TYPE   //used for KRAM-allocation!
{
    EDD_IP_ADR_TYPE         entry[MAX_ARP_IP_FILTER];

} NRT_ARP_FILTER_TABLE_TYPE;

#define EDDI_MAX_IRTE_DCP_FILTER_TABLE_ENTRIES  8   //do not change! IRTE-caused limitation!

#define EDDI_R6_DCP_FILTER_TABLE_BIT__FilterStringPtr     EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_R6_DCP_FILTER_TABLE_BIT__FilterStringLength  EDDI_BIT_MASK_PARA(31,21)

typedef struct _EDDI_R6_DCP_FILTER_STRING
{
    LSA_UINT32                             Val32;

} EDDI_R6_DCP_FILTER_STRING;

typedef struct _EDDI_R6_DCP_FILTER_TABLE_TYPE //KRAM-dcp-filter-table is defined by IRTE
{
    LSA_UINT16                             FilterType[EDDI_MAX_IRTE_DCP_FILTER_TABLE_ENTRIES];

    EDDI_R6_DCP_FILTER_STRING              f[EDDI_MAX_IRTE_DCP_FILTER_TABLE_ENTRIES];

} EDDI_R6_DCP_FILTER_TABLE_TYPE;

typedef struct _EDDI_R6_DCP_FILTER_STRING_BUF_TYPE //KRAM-dcp-filter-string-buffer is defined by IRTE
{
    LSA_UINT8                              Byte[EDDI_MAX_DCP_FILTER_STRING_BUFFER_LEN];

} EDDI_R6_DCP_FILTER_STRING_BUF_TYPE;

typedef struct _EDDI_R6_NRT_ARP_DCP_FILTER_TYPE //for HW-Filters
{
    EDDI_R6_DCP_FILTER_TABLE_TYPE       *  pDev_DCP_Filter_Table;         //pointer to DCP-Filter-table in KRAM

    EDDI_R6_DCP_FILTER_STRING_BUF_TYPE  *  pDev_DCP_Filter_String_Buf;    //pointer to DCP-Filter-String-Buffer in KRAM

    LSA_UINT32                             DCPFilterHwCount;              //possible: 0 or 1

    EDDI_SER_NRT_CCW_ENTRY_TYPE         *  pCCW_Entry;

} EDDI_R6_NRT_ARP_DCP_FILTER_TYPE;

#endif //EDDI_CFG_REV6 || EDDI_CFG_REV7

//Defines for ARP-Filter
#define EDDI_ARP_FILTER_INVALID_HANDLE 0xFF
typedef struct _NRT_ARP_FILTER_TYPE
{
    LSA_UINT32                    FilterCnt;      //0 = deactive ARP-Filter

    LSA_HANDLE_TYPE               Handle[EDDI_MAX_ARP_FILTER];          //channel that activated the ARP-Filter.  
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
    LSA_UINT32                    IPAddrAsU32HW[EDDI_MAX_ARP_FILTER];
    #endif
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON) 
    LSA_UINT32                    IPAddrAsU32SW[EDDI_MAX_ARP_FILTER];
    #endif
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)
    EDDI_IP_TYPE                  IPAddr[EDDI_MAX_ARP_FILTER];
    volatile  LSA_UINT32          RefCnt;         //reference counter for SW-ARP-Filter
    LSA_UINT32                    ARPPassCtr;     //frames passed ARP-Filter-Entries
    LSA_UINT32                    ARPDropCtr;     //frames dropped by ARP-Filter-Entries
    #endif
    
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)   //!EDDI_CFG_REV5
    NRT_ARP_FILTER_TABLE_TYPE  *  pDev_ARP_Filter_Table;
    #endif

} NRT_ARP_FILTER_TYPE;

typedef NRT_ARP_FILTER_TYPE  EDDI_LOWER_MEM_ATTR  *  NRT_ARP_FILTER_PTR_TYPE;

//Defines for DCP-Filter
#define EDDI_DCP_FILTERTYPE_ALL                 EDDI_PASTE_16(FF,FF)    //do not change! filter type for ALL-filter in host format, defined by NORM!
#define EDDI_DCP_FILTERTYPE_NOS                 EDDI_PASTE_16(02,02)    //do not change! filter type for NOS-filter (Name of Station) in host format, defined by NORM!
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_DCP_FILTERTYPE_INVALID             EDDI_PASTE_16(7F,7F)    //do not change! filter type for inactive filter used for IRTE
#endif

/* -------------------------------------------------------------------------*/
/* HOL hysteresis                                                           */
/* -------------------------------------------------------------------------*/
typedef enum _EDDI_NRT_HYST
{
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_NRT_HYST_STS_UP_NORMAL = 2
    #else
    EDDI_NRT_HYST_STS_UP_NORMAL = 2
    //EDDI_NRT_HYST_STS_DOWN,
    //EDDI_NRT_HYST_STS_UP
    #endif

} EDDI_NRT_HYST;

/*---------------------------------------------------------------------------*/
/* NRT-Timestamp-Trace                                                       */
/*---------------------------------------------------------------------------*/
#if defined (EDDI_CFG_REV5)

#if defined (EDDI_CFG_NRT_TS_TRACE_DEPTH)

//Defines for used File
#define EDDI_FILE_INDEX_EDD_ISR_NRT_C    0x10000000UL
#define EDDI_FILE_INDEX_NRT_OC_C         0x20000000UL
#define EDDI_FILE_INDEX_NRT_TS_C         0x30000000UL
#define EDDI_FILE_INDEX_NRT_USR_C        0x40000000UL

typedef struct _EDDI_NRT_TS_TRACE_ENTRY_TYPE
{
    LSA_UINT32                    ClockCntVal;    //10ns-time-tick
    LSA_UINT32                    Location;       //combination of file-index and line-number of source-code
    LSA_UINT32                    Caller;         //Caller
    LSA_UINT32                    TimeStampState; //Time-Stamp-State
    LSA_UINT32                    Detail;         //Detail

} EDDI_NRT_TS_TRACE_ENTRY_TYPE;

typedef struct _EDDI_NRT_TS_TRACE_TYPE
{
    LSA_UINT32                    NextIndex;      //reference next entry, 0...(EDDI_CFG_NRT_TS_TRACE_DEPTH-1)
    EDDI_NRT_TS_TRACE_ENTRY_TYPE  Trace[EDDI_CFG_NRT_TS_TRACE_DEPTH];

} EDDI_NRT_TS_TRACE_TYPE;

#define EDDI_NRT_TS_TRACE_INIT(pDDB_)           \
{                                               \
(pDDB_)->NRT.TimeStampTrace.NextIndex = 0;      \
}

#define EDDI_NRT_TS_TRACE_ENTRY(pDDB_, FileIndex_, Caller_, TimeStampState_)                                \
{                                                                                                           \
    LSA_UINT32                              NextIndex   = (pDDB_)->NRT.TimeStampTrace.NextIndex;            \
    EDDI_NRT_TS_TRACE_ENTRY_TYPE  *  const  pTraceEntry = &((pDDB_)->NRT.TimeStampTrace.Trace[NextIndex]);  \
                                                                                                            \
    pTraceEntry->ClockCntVal    = IO_R32(CLK_COUNT_VALUE);                                                  \
    pTraceEntry->Location       = (LSA_UINT32)((FileIndex_) | __LINE__);                                    \
    pTraceEntry->Caller         = (Caller_);                                                                \
    pTraceEntry->TimeStampState = (LSA_UINT32)(TimeStampState_);                                            \
    pTraceEntry->Detail         = IO_R32(CLK_COUNT_VALUE_SND);                                              \
                                                                                                            \
    NextIndex++;                                                                                            \
    if (NextIndex >= (LSA_UINT32)EDDI_CFG_NRT_TS_TRACE_DEPTH)                                               \
    {                                                                                                       \
        NextIndex = 0;                                                                                      \
    }                                                                                                       \
    (pDDB_)->NRT.TimeStampTrace.NextIndex = NextIndex;                                                      \
}

#define EDDI_NRT_TS_COPY_TS_TRACE()  EDDI_CopyTimestampTraceToLSATrace(pDDB)

#else //!EDDI_CFG_NRT_TS_TRACE_DEPTH

#define EDDI_NRT_TS_TRACE_INIT(pDDB_)

#define EDDI_NRT_TS_TRACE_ENTRY(pDDB_, FileIndex_, Caller_, TimeStampState_)

#define EDDI_NRT_TS_COPY_TS_TRACE()

#endif //EDDI_CFG_NRT_TS_TRACE_DEPTH

#endif //EDDI_CFG_REV5

/*---------------------------------------------------------------------------*/
/* NRT-fragmentation                                                         */
/*---------------------------------------------------------------------------*/

//NRT Fragmention FrameID
#define  EDDI_NRT_FRAMEID_FRAG_START  0xFF80
#define  EDDI_NRT_FRAMEID_FRAG_END    0xFF8F

#if defined (EDDI_CFG_FRAG_ON)

//internal compiler-switches
#define EDDI_TX_FRAG_DEBUG_MODE         //for testing!
//#define EDDI_CFG_FRAG_RELOAD_MODE1    //optional: try to reload all PreQueues (5) of an interface (A0) at every NRT send on this interface and timer triggered, only if any fragmentation buffer has just been deallocated from the Frag Dealloc Queue of this interface before
#define EDDI_CFG_FRAG_RELOAD_MODE2      //optional: try to reload all PreQueues (5) of an interface (A0) at every NRT send on this interface and timer triggered unconditionally

//maximum TX Fragment Netto Sizes (without FCS, IFG, long Preamble and SFD). FragSize = 124/244 bytes
#define MAX_TX_FRAG_NETTO_SIZE_LOW                  (124UL - 4UL) //-FCS
#define MAX_TX_FRAG_NETTO_SIZE_HIGH                 (244UL - 4UL) //-FCS
//Header Size of Fragment Header
#define TX_FRAG_HEADER_SIZE                         (6UL)
//Alignment of Fragment Buffer netto Size
#define TX_FRAG_BUF_NETTO_SIZE_ALIGN                (3UL) //4-Byte-aligned
//defines for private control data in TX Fragment Buffer
#define TX_FRAG_BUF_PRIV_SIZE                       (4UL) //must be 4-byte-aligned!
#define TX_FRAG_BUF_PRIV_OFFS_SEND_LEN_IN_DBS       (0UL) //1 byte; is inserted in all fragments
#define TX_FRAG_BUF_PRIV_OFFS_SEND_LEN              (1UL) //1 byte; is inserted in all fragments
#define TX_FRAG_BUF_PRIV_OFFS_FRAMEID_LOW_OFFSET    (2UL) //1 byte; is inserted in all fragments (only 1st is used)

typedef struct _EDDI_NRT_TX_FRAG_STATISTIC_TYPE
{
    LSA_UINT32                      SentFragmentedFrames;
    LSA_UINT32                      SentNonFragmentedFrames;
    LSA_UINT32                      DiscardedTXFrames;

} EDDI_NRT_TX_FRAG_STATISTIC_TYPE;

typedef struct _EDDI_NRT_TX_FRAG_DDB_TYPE
{
    //Global data
    LSA_BOOL                        bTxFragmentationOn;                         //control data for TX Fragmentation
    volatile  LSA_BOOL              bFragCleanupActive;                         //control data for TX Fragmentation
    LSA_UINT32                      SemaphoreState;                             //control data for TX Frag Semaphore
    LSA_UINT32                      SemaphoreCurrentInterfaceNr;                //control data for TX Frag Semaphore
    LSA_BOOL                        bNewClaimRequest[EDDI_NRT_IF_CNT];          //control data for TX Frag Semaphore round-robin scheduler
    LSA_UINT32                      SemaphoreClaimRefCnt;                       //control data for TX Frag Semaphore round-robin scheduler
    LSA_UINT32                      LastSemaphoreClaimRefCnt;                   //control data for TX Frag Semaphore round-robin scheduler
    LSA_UINT8                       FragmentFrameIdLow;
    LSA_UINT32                      ActiveUsrPortIndex;                         //UsrPortIndex of the one active FRAG port
    LSA_UINT32                      ActiveHwPortIndex;                          //HwPortIndex of the one active FRAG port
    LSA_UINT32                      SendLengthMaxNoFrag;                        //maximal send length without necessary TX fragmentation (several conditions are coded into this parameter)
    LSA_UINT32                      ActivePortCnt;                              //number of active ports - always valid!
    LSA_UINT32                      NRTFragHeaderBuffer;                        //global buffer with constant NRT Fragment Header data

    //following data only valid during a semaphore lock
    EDD_UPPER_MEM_U8_PTR_TYPE       pSndBufferCurrent;                          //pointer to send buffer of current fragment
    LSA_UINT32                      SndLengthCurrent;                           //send length of send buffer of current fragment
    LSA_UINT32                      PDUFragmentFrameIdLowOffset;                //offset of FrameIdLow in all fragments of a NRT send service
    LSA_UINT32                      RemainingFragmentDmacwExitCnt;              //counter of remaining fragments of the current NRT send service, that must still exit the DMACW-ring
    LSA_UINT32                      RemainingFragmentDmacwExitCntAtStopping;    //counter of remaining fragments of the current NRT send service, that must still exit the DMACW-ring at stopping
    LSA_BOOL                        bLastFragmentDmacwEntry;                    //boolean that determines whether the last fragment of the current NRT send service enters the DMACW-ring
    LSA_RESULT                      StoredTxStatus;                             //stored TxStatus of the 1st negative confirmed NRT TX fragment

    EDDI_NRT_TX_FRAG_STATISTIC_TYPE Statistic[EDDI_MAX_IRTE_PORT_CNT+1];

} EDDI_NRT_TX_FRAG_DDB_TYPE;

//TX Frag semaphore commands
#define EDDI_TX_FRAG_SEMA_CLAIM         0UL
#define EDDI_TX_FRAG_SEMA_RELEASE       1UL
#define EDDI_TX_FRAG_SEMA_RESET         2UL
#define EDDI_TX_FRAG_SEMA_CLEAN         3UL
//TX Frag semaphore states
#define EDDI_TX_FRAG_SEMA_FREE          0UL
#define EDDI_TX_FRAG_SEMA_LOCKED        1UL

//TX Frag RQB defines
#define EDDI_TX_FRAG_RQB_MARK           0x80000000UL //mark for RQB.internal_context_1
#define EDDI_TX_FRAG_SEND_DATA_STORED   0x40000000UL //mark for RQB.internal_context_1

//internal plausibilizations
#if defined (EDDI_CFG_FRAG_RELOAD_MODE1) && defined (EDDI_CFG_FRAG_RELOAD_MODE2)
#error "NRT TX fragmentation: EDDI_CFG_FRAG_RELOAD_MODE1 and EDDI_CFG_FRAG_RELOAD_MODE2 defined together!"
#endif

#endif //EDDI_CFG_FRAG_ON

#if defined (EDDI_CFG_DEFRAG_ON)
#define EDD_MAC_ADDR_DA_SA_SIZE   EDD_MAC_ADDR_SIZE + EDD_MAC_ADDR_SIZE  

//NRT FRAG RX
typedef struct _EDDI_NRT_RX_FRAG_STATISTIC_TYPE
{
    LSA_UINT32  ReceivedFragmentedFrames;   
    LSA_UINT32  ReceivedNonFragmentedFrames;
    LSA_UINT32  DiscardedRXFrames;          
    LSA_UINT32  DiscardedRXFragments;     
   
} EDDI_NRT_RX_FRAG_STATISTIC_TYPE;

typedef struct _EDDI_NRT_RX_FRAG_TYPE
{
    EDD_UPPER_MEM_U8_PTR_TYPE        pNRTDeFragFrame[EDDI_NRT_IF_CNT]; //buffer to fragmented data
    LSA_UINT8                        NextExpectedFragmentNumber[EDDI_NRT_IF_CNT];
    LSA_UINT32                       LastOffset[EDDI_NRT_IF_CNT];                
    LSA_UINT16                       LastFrameID[EDDI_NRT_IF_CNT];              
    LSA_UINT32                       LastFilterIndex[EDDI_NRT_IF_CNT];  
    LSA_UINT8                        LastMacDASA[EDDI_NRT_IF_CNT][EDD_MAC_ADDR_DA_SA_SIZE];
    LSA_UINT32                       LastVLAN[EDDI_NRT_IF_CNT];    
    LSA_UINT32                       DstPayLoadDataOffset[EDDI_NRT_IF_CNT];      
    LSA_UINT32                       SrcPayLoadDataOffset[EDDI_NRT_IF_CNT];      
    LSA_UINT32                       OriginalFilterIndex[EDDI_NRT_IF_CNT];       
    LSA_UINT32                       ClaimedInterfaceNr[EDDI_NRT_IF_CNT];  

    LSA_UINT32                       FrameLen[EDDI_NRT_IF_CNT];
    LSA_BOOL                         bLastFrame[EDDI_NRT_IF_CNT];
    
    EDDI_NRT_RX_FRAG_STATISTIC_TYPE  Statistic[EDDI_MAX_IRTE_PORT_CNT+1];
    
    LSA_BOOL                         bDefragOn;

} EDDI_NRT_RX_FRAG_TYPE;
#endif //EDDI_CFG_DEFRAG_ON

/*---------------------------------------------------------------------------*/
/* NRT-COMP-Structure                                                        */
/*---------------------------------------------------------------------------*/
#define EDDI_TS_CALLER_TS_INT            0x00000004UL    //= NRT-Timestamp-Interrupt

#if defined (EDDI_CFG_REV5)
#if defined (EDDI_CFG_NRT_TS_TRACE_DEPTH)
#define EDDI_TS_CALLER_NRT_SEND_TS_REQ   0x00000100UL
#define EDDI_TS_CALLER_NRT_CANCEL_TX     0x00000200UL
#endif

#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
#define EDDI_TS_CALLER_MIRROR_MODE       0x00000300UL
#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

#define EDDI_TS_CALLER_TIMEOUT           0x00000005UL
#define EDDI_TS_CALLER_TX_DONE           0xabcdef98UL    //= NRT-TX-Done-Interrupt for TS-Service
#define EDDI_TS_CALLER_CLOSE             0x00000006UL
#define EDDI_TS_CALLER_LINK_DOWN         0x00000007UL
#endif //EDDI_CFG_REV5

#if defined (EDDI_CFG_REV5)
typedef enum _EDDI_DDB_COMP_NRT_TS_STATE_TYPE
{
    EDDI_TS_STATE_NO_REQ,
    EDDI_TS_STATE_REQ_STARTED_AWAIT_INTS,
    EDDI_TS_STATE_TS_INT_AWAIT_TX_DONE,
    EDDI_TS_STATE_RECOVER,
    EDDI_TS_STATE_CHECK_LIST_STATUS,
    EDDI_TS_STATE_TX_DONE_AWAIT_TS_INT_OR_TIMEOUT,
    EDDI_TS_STATE_CLOSING

} EDDI_DDB_COMP_NRT_TS_STATE_TYPE;
#endif

typedef struct _EDDI_DDB_COMP_NRT_TIME_STAMP_TYPE
{
    #if defined (EDDI_CFG_REV5)
    EDDI_RQB_QUEUE_TYPE                ReqQueue;
    EDD_UPPER_RQB_PTR_TYPE             pActReq;
    EDDI_DDB_COMP_NRT_TS_STATE_TYPE    State;
    LSA_BOOL                           bLastTxTimeStored;
    LSA_UINT32                         LastTxTime;
    #endif

    LSA_TIMER_ID_TYPE                  TimerID;
    LSA_UINT32                         Timeout_ticks_in10ms;
    LSA_UINT32                         Timeout;

    #if defined (EDDI_CFG_REV5)
    LSA_UINT32                         cTimeStamp; //Counter for Timestamp-Timeout
    LSA_UINT32                         HwPortIndex;
    #endif

} EDDI_DDB_COMP_NRT_TIME_STAMP_TYPE;

typedef struct _EDDI_DDB_COMP_NRT_TYPE
{
    LSA_UINT32                         HandleCnt; //Number of handles using NRT

    EDDI_NRT_CHX_SS_IF_TYPE            IF[EDDI_NRT_IF_CNT];

    EDDI_NRT_HYST                      HOL_CH_Limit;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_R6_NRT_ARP_DCP_FILTER_TYPE    R6_arp_dcp;
    #endif

    NRT_ARP_FILTER_TYPE                ARPFilter;

    LSA_UINT32                         DCPFilterTotalCount;             //total-counter for active DCP-Filter-entries (SW+HW). not protected by reference counter!
    LSA_UINT32                         DCPFilterSwCount;                //counter for active DCP-Filter-entries processed by SW
    volatile  LSA_UINT32               DCPFilterRefCnt;                 //reference counter for SW-DCP-Filter
    LSA_HANDLE_TYPE                    DCPFilterHandle;                 //channel that activates the DCP-Filter at last. Parameter is only valid when DCP-Filter is active!
    LSA_UINT32                         DCPFilterInvalidFrameCount;      //statistic counter for invalid frames
    LSA_UINT32                         DCPFilterDropCount;              //statistic counter for dropped frames
    NRT_DCP_FILTER_TYPE                DCPFilter[EDDI_NRT_MAX_DCP_FILTER_CNT];

    volatile  LSA_UINT32               DCPHelloFilterRefCnt;            //reference counter for DCP-Hello-Filter
    LSA_UINT32                         DCPHelloFilterInvalidFrameCount; //statistic counter for invalid frames
    LSA_UINT32                         DCPHelloFilterDropCount;         //statistic counter for dropped frames
    NRT_DCP_HELLO_FILTER_TYPE          DCPHelloFilter[EDD_DCP_MAX_DCP_HELLO_FILTER];

    EDDI_NRT_CHX_SS_IF_TYPE          * pTS_IF;

    EDDI_DDB_COMP_NRT_TIME_STAMP_TYPE  TimeStamp;
    #if defined (EDDI_CFG_NRT_TS_TRACE_DEPTH)
    EDDI_NRT_TS_TRACE_TYPE             TimeStampTrace;
    #endif

    LSA_UINT32                         NRT_DB_CHx;

    LSA_UINT32                         MaxSndLen;
    LSA_BOOL                           bFeedInLoadLimitationActive;
    LSA_BOOL                           bFeedInLoadLimitationActiveDefault;
    LSA_BOOL                           bIO_Configured;
    LSA_BOOL                           bIO_ConfiguredDefault;

    LSA_BOOL                           PaddingEnabled;  //True, if all Ports are enabled for Padding
                                                        //Allows to use DataLen < EDDI_CSRT_MIN_DATALEN
                                                        //for SRT-Consumer and -Provider
                                                        //Will only be set once during CrtConfig

    #if defined (EDDI_CFG_FRAG_ON)
    EDDI_NRT_TX_FRAG_DDB_TYPE          TxFragData;
    LSA_UINT16                         NRTPreQueuesTimerID;
    LSA_BOOL                           bNRTPreQueuesTimerStart;
    #endif

    #if defined (EDDI_CFG_DEFRAG_ON)
    EDDI_NRT_RX_FRAG_TYPE              RxFragData;
    #endif

} EDDI_DDB_COMP_NRT_TYPE;

typedef struct _EDDI_DDB_COMP_NRT_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_NRT_INT_H


/*****************************************************************************/
/*  end of file eddi_nrt_int.h                                               */
/*****************************************************************************/
