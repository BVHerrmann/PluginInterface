
#ifndef EDDS_DEV_H                    /* ----- reinclude-protection ----- */
#define EDDS_DEV_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_dev.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for EDDS                */
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

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

/** recurring task reduction in ms*/
#define EDDS_RECURRING_REDUCTION_MS			25

/** total time of show locaton blink in seconds */
#define EDDS_TOTAL_BLINK_DURATION_S         3


/*---------------------------------------------------------------------------*/
/* setting this define will push the frame filtering for                     */
/* ARP DCP and DCPHELLO Frames back into the RQB-Context                     */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_FILTER_FRAME_QUEUE_NRT_RCV_ENABLE

/*---------------------------------------------------------------------------*/
/* Because of the zero-copy - interface we must take care of alignment and   */
/* buffersize requirements from the ethernetcontroller within application.   */
/*---------------------------------------------------------------------------*/
/* The EDDS needs the bufferrequirements mentioned below.                    */
/* For Rx-Frames and SRT - Tx-Frames it needs DWORD-Aligned buffers because  */
/* it needs to access the framecontent with WORD and DWORD - accesses. To    */
/* prevent misaligned accesses the buffers have to be proper aligned.        */
/* For NRT - Tx- Buffers the EDDS does not access the buffer, so the edds has */
/* no special requirements.                                                  */
/*---------------------------------------------------------------------------*/

#define EDDS_RX_FRAME_BUFFER_ALIGN      0x03 /* 4 -Byte-aligned frame buffers*/
#define EDDS_TX_NRT_FRAME_BUFFER_ALIGN  0x00 /* no alignment needed          */
#define EDDS_TX_SRT_FRAME_BUFFER_ALIGN  0x03 /* 4 -Byte-aligned frame buffers*/

/*---------------------------------------------------------------------------*/
/* type/len-field types                                                      */
/*---------------------------------------------------------------------------*/

#ifndef EDDS_CFG_BIG_ENDIAN
#define EDDS_VLAN_TAG                           0x0081
#define EDDS_xRT_TAG                            0x9288
#define EDDS_LLDP_TAG                           0xCC88
#define EDDS_IP_TAG                             0x0008
#define EDDS_ARP_TAG                            0x0608
#define EDDS_RARP_TAG                           0x3580    /* reversed ARP */
#define EDDS_MRP_TAG                            0xE388
#define EDDS_HSYNC_TAG                          0x0A80
#else
#define EDDS_xRT_TAG                            0x8892
#define EDDS_LLDP_TAG                           0x88CC
#define EDDS_IP_TAG                             0x0800
#define EDDS_ARP_TAG                            0x0806
#define EDDS_RARP_TAG                           0x8035    /* reversed ARP */
#define EDDS_VLAN_TAG                           0x8100
#define EDDS_MRP_TAG                            0x88E3
#define EDDS_HSYNC_TAG                          0x800A
#endif

/*---------------------------------------------------------------------------*/
/* xRToverUDP UDP-Port                                                       */
/*---------------------------------------------------------------------------*/

#ifndef EDDS_CFG_BIG_ENDIAN
#define EDDS_xRT_UDP_PORT                       0x9288
#else
#define EDDS_xRT_UDP_PORT                       0x8892
#endif


/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* General Ethernet Header:                                                  */
/*                                                                           */
/*+   6   6   2     2     2                                                 +*/
/*+ +---+---+----+-----+-----+                                              +*/
/*+ |DA |SA |VLAN|VLAN |Type |                                              +*/
/*+ |   |   |TYPE| ID  |LEN  |                                              +*/
/*+ |   |   |    |     |     |                                              +*/
/*+ +---+---+----+-----+-----+                                              +*/
/*+          \--------/                                                     +*/
/*+           optional                                                      +*/
/*---------------------------------------------------------------------------*/

/* for optimized access to framebuffer we use this defines       */
/* The framebuffer has to be DWORD-aligned                       */

#define EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN           14
#define EDDS_FRAME_HEADER_SIZE_WITH_VLAN              18

#define EDDS_FRAME_LEN_TYPE_WORD_OFFSET                6
#define EDDS_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET      8

/* if VLAN Tag is present */
#define EDDS_FRAME_VLAN_TYPE_WORD_OFFSET              EDDS_FRAME_LEN_TYPE_WORD_OFFSET
#define EDDS_FRAME_VLAN_ID_WORD_OFFSET                 7

/* Macro to get LEN_TYPE-Field of Ethernetframe. This may be the VLAN-Type!  */
/* pBuffer has to be at least 16Bit aligned and points to start of frame     */

#define EDDS_FRAME_GET_LEN_TYPE(pBuffer)(((EDD_UPPER_MEM_U16_PTR_TYPE )pBuffer)[EDDS_FRAME_LEN_TYPE_WORD_OFFSET])

/*---------------------------------------------------------------------------*/
/* Head and Tail of Ethernet Header:                                         */
/*                                                                           */
/*     7      1   6   6   2     2     2    4   12                            */
/* +--------+---+---+---+----+-----+-----+---+....+                          */
/* |Preamble|SFD|DA |SA |VLAN|VLAN |Type |FCS|IPG |                          */
/* |        |   |   |   |TYPE| ID  |LEN  |   |    |                          */
/* |        |   |   |   |    |     |     |   |    |                          */
/* +--------+---+---+---+----+-----+-----+---+....+                          */
/*  \----------/        \---------/                                          */
/*      Head             optional                                            */
/*---------------------------------------------------------------------------*/

#define EDDS_ETHERNET_FRAME_PREAMBLE_LENGTH     7  /* Preamble allows devices on the network to easily detect a new incoming frame */
                                                   /* NOTE: EDDS does only support long preamble! */
#define EDDS_ETHERNET_FRAME_SFD_LENGTH          1  /* Start Frame Delimiter is an 8-bit (1-byte) value marking the end of the preamble */
#define EDDS_ETHERNET_FRAME_FCS_LENGTH          4  /* Frame Check Sequence 32-bit CRC */
#define EDDS_ETHERNET_FRAME_IPG_LENGTH          12 /* Interpacket gap is the idle time between packets */
#define EDDS_ETHERNET_FRAME_HEAD_LENGTH         (EDDS_ETHERNET_FRAME_PREAMBLE_LENGTH + \
                                                 EDDS_ETHERNET_FRAME_SFD_LENGTH)

/* ethernet frame wrapper (wraps ethernet payload) */
#define EDDS_ETHERNET_FRAME_HEADER_TRAILER_LENGTH (EDDS_ETHERNET_FRAME_HEAD_LENGTH + \
                                                   EDDS_ETHERNET_FRAME_IPG_LENGTH  + \
                                                   EDDS_ETHERNET_FRAME_FCS_LENGTH) /* SFD + Preamble + inter packet gap + frame check sequence */

#define EDDS_ETHERNET_FRAME_PAYLOAD_LENGTH_MAX  1518 /* maximum payload for a ethernet frame including a VLAN tag */
#define EDDS_ETHERNET_FRAME_LENGTH_MAX          (EDDS_ETHERNET_FRAME_HEAD_LENGTH        + \
                                                 EDDS_ETHERNET_FRAME_PAYLOAD_LENGTH_MAX + \
                                                 EDDS_ETHERNET_FRAME_FCS_LENGTH)

#define EDDS_ETHERNET_FRAME_ON_WIRE_LENGTH_MAX  (EDDS_ETHERNET_FRAME_LENGTH_MAX + \
                                                 EDDS_ETHERNET_FRAME_IPG_LENGTH)

/* NOTE: with 10MBit/s the maximum byte count would be 1250, assure at least one maximum frame */
#define EDDS_RXTX_MAX_BYTE_COUNT_10MBIT_1MS     1542    /* maximum byte count for 1ms cycle at 10MBit */
#define EDDS_RXTX_MAX_BYTE_COUNT_100MBIT_1MS    12500   /* maximum byte count for 1ms cycle at 100MBit */
#define EDDS_RXTX_MAX_BYTE_COUNT_1000MBIT_1MS   125000  /* maximum byte count for 1ms cycle at 1000MBit */
#define EDDS_RXTX_MAX_BYTE_COUNT_10000MBIT_1MS  1250000 /* maximum byte count for 1ms cycle at 10000MBit */

/* NOTE: Prio queue 0 always gets 100% Tx bandwith, the actual bandwith    */
/*       for prio queue 0 is then restricted through the total Tx bandwith */
/*                                                                         */
/*       Hsync Prio queue 3 and 4 always gets 100% Tx bandwith, the actual */
/*       bandwith for both queues is then restricted through the total Tx  */
/*       bandwith                                                          */
/*                                                                         */
/*  highest  |     EDDS_TX_BANDWITH_PERCENTAGE_PRIO5 (ORG)                 */
/*           |     100% without FILL, 0% with FILL   (HSYNC_HIGH)          */
/*           |     100% without FILL, 0% with FILL   (HSYNC_LOW)           */
/*           |     EDDS_TX_BANDWITH_PERCENTAGE_PRIO2 (Alarm High)          */
/*           |     EDDS_TX_BANDWITH_PERCENTAGE_PRIO1 (Alarm Low)           */
/*  lowest   |     EDDS_TX_BANDWITH_PERCENTAGE_PRIO0 (IP, etc.)            */
/*           V                                                             */
#define EDDS_TX_BANDWITH_PERCENTAGE_PRIO1     10  /* send bandwith for EDDS_NRT_TX_PRIO_1 (Alarm High) */
#define EDDS_TX_BANDWITH_PERCENTAGE_PRIO2     10  /* send bandwith for EDDS_NRT_TX_PRIO_2 (Alarm Low) */
#define EDDS_TX_BANDWITH_PERCENTAGE_PRIO5     10  /* send bandwith for EDDS_NRT_TX_PRIO_3 (ORG) */

/* minimal frame (ieee) */
#define EDDS_IEEE_MIN_FRAME_LENGTH_WITHOUT_PADDING  14
#define EDDS_IEEE_MIN_FRAME_LENGTH                  60

/* NOTE: with 10MBit/s the actual Tx bandwith is smaller than FILL */
#define EDDS_TX_FILL_MAX_BYTE_COUNT           3084 /* maximum bytes to be sent with FILL activated */

/*---------------------------------------------------------------------------*/
/* Frameformat for xRT over Ethernet                                         */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ mandatory on send      */
/* 12/16 | 2 |TypeLenxRT                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 | 2 |FrameID                              |                         */
/* 16/20 |   |Data                                 |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define EDDS_FRAME_XRT_FRAME_ID_WORD_OFFSET            7     /* without VLAN */
#define EDDS_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET  9

/*---------------------------------------------------------------------------*/
/* Frameformat for xRT over UDP                                              */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ not inserted on send   */
/* 12/16 | 2 |IP-Type                              |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 |20 |IPHeader                             |  we only accept 20 Byte */
/* ------+---+-------------------------------------+-------------------------*/
/* 34/38 | 8 |UDP-Header                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 42/46 | 2 |FrameID                              |                         */
/* 44/48 |   |Data                                 |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* for IP/UPD-Header see edds_pck.txt/h */

#ifndef EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_100
#define EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_100 0xFFFFFFFFl
#endif

#ifndef EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_10
#define EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_10  0xFFFFFFFFl
#endif


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/* Handle-management-structures                                              */
/* (this structure is asigned to every open channel. Each component (NRT,SRT */
/* IRT) can have individual parameters within management structure only used */
/* in component.                                                             */
/*===========================================================================*/

typedef struct _EDDS_HDB_TYPE     EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_HDB_PTR_TYPE;
typedef struct _EDDS_DDB_TYPE     EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DDB_PTR_TYPE;

/* foreward declaration of LLIF table structure pointer (@see edds_llif.h) */
typedef struct _EDDS_LL_TABLE_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_EDDS_LL_TABLE_PTR_TYPE;

/* A EDDS_TRANSFER_BUFFER is an array of EDD_FRAME_BUFFER_LENGTH byte */
typedef LSA_UINT8 EDDS_TRANSFER_BUFFER[EDD_FRAME_BUFFER_LENGTH];
typedef EDDS_TRANSFER_BUFFER      EDDS_LOCAL_MEM_ATTR *     EDDS_TRANSFER_BUFFER_ARRAY;

typedef struct _EDDS_RQB_LIST_TYPE
{
        EDD_UPPER_RQB_PTR_TYPE          pBottom;
        EDD_UPPER_RQB_PTR_TYPE          pTop;
} EDDS_RQB_LIST_TYPE;

typedef struct _EDDS_RQB_LIST_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_RQB_LIST_TYPE_PTR;

typedef struct _EDDS_HDB_TYPE
{
    LSA_BOOL                            InUse;
    LSA_UINT32                          UsedComp;
    LSA_SYS_PATH_TYPE                   SysPath;
    LSA_SYS_PTR_TYPE                    pSys;
    EDDS_DETAIL_PTR_TYPE                pDetail;
    LSA_HANDLE_TYPE                     UpperHandle;
    LSA_UINT32                          HDBIndex;
    LSA_VOID                            LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);

    LSA_UINT8                           InsertSrcMAC; /* EDDS inserts station MAC */
    /* address in send frame   */
    /* before sending.         */
    /* Link-Indication Requests*/
    EDDS_RQB_LIST_TYPE                  LinkIndReq[EDDS_MAX_PORT_CNT+1];  /* index 0 is AUTO */

    /* ResetValue for Statistics    */
    /* because Statistics can be    */
    /* Reset per channel and Port   */
    /* we must calculate the actual */
    /* depending on this values     */
    EDDS_STATISTICS_TYPE                StatsResetValues[EDDS_MAX_PORT_CNT+1];  /* index 0 is AUTO */
    EDDS_STATISTICS_TYPE                StatsValues[EDDS_MAX_PORT_CNT+1]; /* index 0 is AUTO */


    LSA_UINT16                          prmStatsPortID;


    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;         /* link to devicemanagement*/
    EDDS_LOCAL_HDB_COMP_NRT_PTR_TYPE    pNRT;
} EDDS_HDB_TYPE;


/* defines for UsedComp (can be ored) */

#define EDDS_COMP_NRT   0x01
#define EDDS_COMP_CSRT  0x04    /* cyclic  SRT */
#define EDDS_COMP_IRT   0x08
#define EDDS_COMP_SWI   0x10
#define EDDS_COMP_PRM   0x20

typedef struct _EDDS_DDB_HDB_MGMT_TYPE
{
    LSA_UINT32               MaxHandleCnt;
    LSA_UINT32               UsedHandleCnt;
    EDDS_HDB_TYPE            HDBTable[EDDS_CFG_MAX_CHANNELS];
} EDDS_DDB_HDB_MGMT_TYPE;

typedef struct _EDDS_DDB_HDB_MGMT_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_DDB_HDB_MGMT_PTR_TYPE;

/*===========================================================================*/
/* Device-description block (DDB)                                            */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* general devicemanagement (component independend)                          */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_TX_INFO_TYPE
{
    LSA_UINT32      InUse;                      /* Transmit in progress      */
    LSA_UINT32      TxCompID;                   /* Component ID for this Tx  */
} EDDS_TX_INFO_TYPE;

typedef struct _EDDS_TX_INFO_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_TX_INFO_PTR_TYPE;

/* defines for TxCompID */

#define EDDS_TXCOMPID_NRT_PRIO_0            0   /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_NRT_PRIO_1            1   /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_NRT_PRIO_2            2   /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_NRT_PRIO_3_HSYNC_LOW  3   /*  */
#define EDDS_TXCOMPID_NRT_PRIO_4_HSYNC_HIGH 4   /*  */
#define EDDS_TXCOMPID_NRT_PRIO_5            5   /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_SRT_Z_XRT             6   /* this tx-entry is from cyclic xRT     */
#define EDDS_TXCOMPID_SRT_Z_UDP             7   /* this tx-entry is from cyclic xRToUDP */

/* Multicast MAC-Management */

typedef enum _EDDS_MC_MAC_FILTER_STATE_TYPE
{
    EDDS_MC_MAC_FILTER_STATE_NONE = 0,
    EDDS_MC_MAC_FILTER_STATE_PAUSED,
    EDDS_MC_MAC_FILTER_STATE_ACTIVE
}EDDS_MC_MAC_FILTER_STATE_TYPE;

typedef enum _EDDS_MC_MAC_FILTER_TRIGGER_TYPE
{
    EDDS_MC_MAC_FILTER_TRIGGER_DISABLE = 0,
    EDDS_MC_MAC_FILTER_TRIGGER_ENABLE,
    EDDS_MC_MAC_FILTER_TRIGGER_START,
    EDDS_MC_MAC_FILTER_TRIGGER_STOP
}EDDS_MC_MAC_FILTER_TRIGGER_TYPE;

typedef struct _EDDS_MC_MAC_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_MC_MAC_PTR_TYPE;

typedef struct _EDDS_MC_MAC_TYPE
{
    LSA_UINT32              Cnt;                   /* MC-MAC usage counter      */
    EDD_MAC_ADR_TYPE        MAC;                   /* MC-MAC if used            */
} EDDS_MC_MAC_TYPE;

typedef struct _EDDS_MC_MAC_FILTER_TYPE
{
    /* TODO: fill filter structure */
    LSA_UINT32  filterDummy;
}EDDS_MC_MAC_FILTER_TYPE;

typedef struct _EDDS_MC_MAC_FILTER_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_MC_MAC_FILTER_PTR_TYPE;

/* the number of supported MC depends on LLIF, so the LLIF can configure   */
/* the number of MC MACs managed by EDDS. If the EDDS resources filled up, */
/* the EDDS set a RESOURCE-Error. In addition the LLIF can set a RESOURCE  */
/* error too. So it dont cares if the the EDDS can manage more MC-MACs then */
/* the LLIF supports. Both can signal a RESOURCE error. But it makes no    */
/* sense to let the EDDS manage more MC then the LLIF supports. On the other*/
/* hand the EDDS may manage less MC then the LLIF supports (to save memory) */

#if defined(EDDS_CFG_MAX_MC_MAC_CNT)
#define EDDS_MAX_MC_MAC_CNT EDDS_CFG_MAX_MC_MAC_CNT
#else /* defined(EDDS_CFG_MAX_MC_MAC_CNT) */
#define EDDS_MAX_MC_MAC_CNT 15  /* default if not defined by cfg switch */
#endif /* defined(EDDS_CFG_MAX_MC_MAC_CNT) */

typedef struct _EDDS_MC_MAC_INFO_TYPE
{
    EDDS_MC_MAC_FILTER_STATE_TYPE   McMacFilterState;       /* current state of the sw mc filtering */
    LSA_UINT32                      cntEnabledMcMac;        /* number of enabled mc mac addresses */
    EDDS_MC_MAC_FILTER_TYPE         mcSWFilter;             /* sw filter */
    EDDS_MC_MAC_TYPE McMac[EDDS_MAX_MC_MAC_CNT];            /* enabled mc mac address table */
} EDDS_MC_MAC_INFO_TYPE;

typedef struct _EDDS_MC_MAC_INFO_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_MC_MAC_INFO_PTR_TYPE;

#define EDDS_MC_NO_IDX 0xFFFFFFFF;


typedef struct _EDDS_HW_PARAM_STRUCT
{
    LSA_UINT32        InterfaceID;    /* Interface ID from DPB                   */
    EDDS_LL_CAPS_TYPE Caps;           /* Capabilities */
    EDDS_LL_HW_PARAM_TYPE   hardwareParams;   /* LL hardware Parameters */
    EDD_IP_ADR_TYPE   IPAddress;      /* Local IP address                        */
} EDDS_HW_PARAM_TYPE;

typedef EDDS_HW_PARAM_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_HW_PARAM_PTR_TYPE;

typedef struct _EDDS_STAT_TX_STRUCT
{
    LSA_UINT32    TxTimeout;
} EDDS_STAT_TX_TYPE;

typedef EDDS_STAT_TX_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_STAT_TX_TYPE_PTR;

typedef struct _EDDS_STAT_RX_STRUCT
{
    LSA_UINT32    RxXRToUDPIPWrongChecksum;
    LSA_UINT32    RxXRToUDPWrongHeader;
    LSA_UINT32    RxOverload;             /* rx-overload (rx-flooding) */
} EDDS_STAT_RX_TYPE;

typedef EDDS_STAT_RX_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_STAT_RX_TYPE_PTR;

typedef volatile struct _EDDS_TIMER_STRUCT
{
    LSA_BOOL       InUse;                /* LSA_TRUE: Timer in USE  */
    LSA_BOOL       InProgress;           /* LSA_TRUE: Timer running */
    LSA_TIMER_ID_TYPE TimerID;           /* LSA Timer ID            */
    LSA_UINT16     EDDSTimerID;         /* Timer ID within EDDS     */
} EDDS_TIMER_TYPE;

typedef volatile struct _EDDS_TIMER_STRUCT EDDS_LOCAL_MEM_ATTR *     EDDS_TIMER_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* This structure is used for internal errors detected for debugging         */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_INTERNAL_ERROR_STRUCT
{
    LSA_UINT32    TxResource;           /* Tx-Resources error            */
    LSA_UINT32    RxOverloadCnt;        /* Number of RxOverloads occured */
} EDDS_INTERNAL_ERROR_TYPE;

typedef struct _EDDS_INTERNAL_ERROR_STRUCT EDDS_LOCAL_MEM_ATTR *     EDDS_INTERNAL_ERROR_TYPE_PTR;

#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
typedef struct _EDDS_INTERNAL_STATISTIC_JITTER_STRUCT {
    LSA_UINT64 time;
    LSA_INT64  jitter;
    LSA_UINT64 jittersumP;
    LSA_UINT64 jittersumN;
    LSA_UINT32 sumcountP;
    LSA_UINT32 sumcountN;
    LSA_UINT32 bouncing;
    LSA_UINT32 skipping;
    LSA_INT32  min;
    LSA_INT32  max;
} EDDS_INTERNAL_STATISTIC_JITTER_TYPE;

#define EDDS_INTERNAL_STATISTICS_JITTER_INIT(statistics) \
{ \
    statistics.time = 0; \
    statistics.jitter = 0; \
    statistics.jittersumP = 0; \
    statistics.jittersumN = 0; \
    statistics.sumcountP = 0; \
    statistics.sumcountN = 0; \
    statistics.bouncing = 0; \
    statistics.skipping = 0; \
    statistics.min = 0x7FFFFFFF; statistics.max = 0x80000000; \
}

#define EDDS_INTERNAL_STATISTICS_JITTER_DEINIT(statistics) \
{ \
    statistics.time = statistics.max - statistics.min; \
    if(statistics.sumcountP || statistics.sumcountN) \
    { \
        statistics.jitter = (statistics.jittersumP - statistics.jittersumN ) / (statistics.sumcountP + statistics.sumcountN); \
    } \
}

typedef struct _EDDS_INTERNAL_STATISTIC_STRUCT {
    LSA_UINT64 time;
    LSA_UINT64 timesum;
    LSA_UINT64 sumcount;
    LSA_UINT64 min;
    LSA_UINT64 max;
} EDDS_INTERNAL_STATISTIC_TYPE;
typedef struct _EDDS_INTERNAL_STATISTIC_STRUCT EDDS_LOCAL_MEM_ATTR *     EDDS_INTERNAL_STATISTIC_TYPE_PTR;

#else  /* defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT) */

typedef LSA_UINT64  EDDS_INTERNAL_STATISTIC_TYPE;
typedef EDDS_INTERNAL_STATISTIC_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_INTERNAL_STATISTIC_TYPE_PTR;

#endif /* defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT) */


typedef struct _EDDS_INTERNAL_PERFORMANCE_STATS_STRUCT
{
    EDDS_INTERNAL_STATISTIC_TYPE   callsAll;             /* statistics where scheduler was called (even if HW was not initialized) */
    EDDS_INTERNAL_STATISTIC_TYPE   callsCyclic;          /* statistics where scheduler executed cyclic call */
    EDDS_INTERNAL_STATISTIC_TYPE   callsIntermediate;    /* statistics where scheduler executed intermediate call  */
    EDDS_INTERNAL_STATISTIC_TYPE   callsRecurringTask;   /* statistics where scheduler executed EDDS_LL_RECURRING_TASK */
    EDDS_INTERNAL_STATISTIC_TYPE   callsPhyEvent;        /* statistics where scheduler executed EDDS_HandlePHYEvent */
    EDDS_INTERNAL_STATISTIC_TYPE   callsHandleRequest;   /* statistics where scheduler executed general requests (async EDDS_LL_* functions) */
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
    EDDS_INTERNAL_STATISTIC_JITTER_TYPE   cylceJitter;  /* jitter statistics of cycle calls */
#endif
} EDDS_INTERNAL_PERFORMANCE_STATS_TYPE;
typedef struct _EDDS_INTERNAL_PERFORMANCE_STATS_STRUCT EDDS_LOCAL_MEM_ATTR *     EDDS_INTERNAL_PERFORMANCE_STATS_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* This structure is used for internal scheduler measurement                 */
/* and performance monitoring                                                */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_INTERNAL_SCHED_STRUCT
{
    LSA_UINT64 currentBeginNS;                           /* system time in nanoseconds of actual scheduler call */
    LSA_UINT64 currentCycleBeginNS;                      /* system time in nanoseconds of cycle begin */
    LSA_UINT16 CycleTimeMS;                              /* actual time between cyclic scheduler calling in milliseconds */
    LSA_UINT8  RecurringTaskExecuteCountDownReloadValue; /* reload value of RecurringTaskExecuteCountDown counter */
    LSA_UINT8  RecurringTaskExecuteCountDown;            /* reduction of executing EDDS_LL_RECURRING_TASK is controlled with this counter */
} EDDS_INTERNAL_SCHED_TYPE;

typedef struct _EDDS_INTERNAL_SCHED_STRUCT EDDS_LOCAL_MEM_ATTR *     EDDS_INTERNAL_SCHED_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* actual LinkStatus/Speed and Mode of PHY                                   */
/*---------------------------------------------------------------------------*/
typedef struct _EDDS_LINK_CHANGED_STRUCT
{
    LSA_UINT8   LinkStateChanged;   /* indicates a pending change of link state */
    LSA_UINT8   PortStateChanged;   /* indicates a pending change of port state */
}EDDS_LINK_CHANGE_TYPE;

#define EDDS_LINK_STATE_NO_CHANGE       0 /* no pending change */
#define EDDS_LINK_STATE_CHANGE_PENDING  1 /* a pending change */

typedef struct _EDDS_LINK_STAT_STRUCT
{
    LSA_UINT8               Status;
    LSA_UINT8               Speed;
    LSA_UINT8               Mode;
    LSA_UINT8               Autoneg;        /* Autoneg configuration                   */
    /* EDD_AUTONEG_ON                          */
    /* EDD_AUTONEG_OFF                         */
    LSA_UINT16              MAUType;        /* MAU Type                                */
    LSA_UINT8               MediaType;      /* Media Type                              */
    LSA_UINT8               IsPOF;          /* is POF                                  */
    LSA_UINT8               PortStatus;     /* Status of Port                          */
    LSA_UINT8               PhyStatus;      /* PHYStatus of Port                       */
    LSA_UINT8               IsPulled;       /* ChangeMediaTypeinRun value              */
    LSA_UINT16              PortState;      /* State  of Port                          */
    LSA_UINT32              LineDelay;      /* LineDelay set by EDD_SRV_SET_LINE_DELAY */
    LSA_UINT32              CableDelayNs;   /* CableDel. set by EDD_SRV_SET_LINE_DELAY */
    LSA_UINT32              AutonegCapAdvertised;      /* autoneg advertised capabil.  */
    LSA_UINT8               AddCheckEnabled;
    LSA_UINT8               ChangeReason;
    EDDS_LINK_CHANGE_TYPE   LinkChange;     /* sub structure of pending changes in states */
} EDDS_LINK_STAT_TYPE;

typedef struct _EDDS_LINK_STAT_STRUCT EDDS_LOCAL_MEM_ATTR *         EDDS_LINK_STAT_PTR_TYPE;
typedef struct _EDDS_LINK_STAT_STRUCT EDDS_LOCAL_MEM_ATTR const *   EDDS_LINK_STAT_PTR_TO_CONST_TYPE;

/* defines for Status       see EDD_SRV_GET_LINK_STATUS*/
/* defines for Speed        see EDD_SRV_GET_LINK_STATUS*/
/* defines for Mode         see EDD_SRV_GET_LINK_STATUS*/
/* defines for PortState    see edd_usr.h              */
/* defines for PHYStatus    see edd_usr.h              */
/* defines for LinkSpeedModeCapability  see edd_usr.h  */

/*---------------------------------------------------------------------------*/
/* actual LED blink status                                                   */
/*---------------------------------------------------------------------------*/
typedef struct _EDDS_LED_BLINK_TYPE
{
    LSA_UINT8           Status;        /* Blink running?                      */
    LSA_BOOL            LEDOn;         /* LED on/off?                         */
    LSA_UINT32          RemainTotal;   /* Time till LED blink stopped (ticks) */
} EDDS_LED_BLINK_TYPE;

typedef struct _EDDS_LED_BLINK_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_LED_BLINK_PTR_TYPE;

/* Status */
#define EDDS_LED_BLINK_INACTIVE    0
#define EDDS_LED_BLINK_ACTIVE      1
/*---------------------------------------------------------------------------*/
/* PRM/Diagnosis (configured by PRM-services)                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* PDU-Format:                                                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*   BlockHeader                                                             */
/*---------------------------------------------------------------------------*/
/*    LSA_UINT16          BlockType                                     2    */
/*    LSA_UINT16          BlockLength        (without Type/Length)      2    */
/*    LSA_UINT8           BlockVersionHigh                              1    */
/*    LSA_UINT8           BlockVersionLow                               1    */
/*                                                                     ---   */
/*                                                                      6    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_HEADER_SIZE 6
#define EDDS_PRM_BLOCK_VERSION_HIGH                 0x01
#define EDDS_PRM_BLOCK_VERSION_LOW                  0x00
#define EDDS_PRM_STATISTIC_BLOCK_VERSION_LOW        0x01

#define EDDS_PRM_BLOCK_VERSION_LOW_MC_BOUNDARY      0x00
#define EDDS_PRM_BLOCK_VERSION_LOW_MAUTYPE          0x00
#define EDDS_PRM_BLOCK_VERSION_LOW_PORTSTATE        0x00
#define EDDS_PRM_BLOCK_VERSION_D_BOUNDARY           0x01
#define EDDS_PRM_BLOCK_VERSION_P2P_BOUNDARY         0x00
#define EDDS_PRM_BLOCK_VERSION_DCP_BOUNDARY         0x00
#define EDDS_PRM_BLOCK_VERSION_PREAMBLE_LENGTH      0x00


/*---------------------------------------------------------------------------*/
/*   PDNCDataCheck (= Frame dropped - no resource)                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          MaintenanceRequiredPowerBudget;               4    */
/*    LSA_UINT32          MaintenanceDemandedPowerBudget;               4    */
/*    LSA_UINT32          ErrorPowerBudget;                             4    */
/*                                                                    -----  */
/*                                                                     20    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDNC_DATA_CHECK        0x8070

#define EDDS_PRM_PDNC_DATA_CHECK_BLOCK_TYPE          0x0230
#define EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE         (EDDS_PRM_RECORD_HEADER_SIZE+14)

#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK    0x7FFFFFFF

#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_MSK    0x80000000
#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON     0x80000000
#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_OFF    0x00000000

#define EDDS_PRM_PDNC_DROP_BUDGET_MIN                0x1
#define EDDS_PRM_PDNC_DROP_BUDGET_MAX                0x3E7

/*---------------------------------------------------------------------------*/
/* Record for ExtChannelErrorType "Frame dropped - no resource"              */
/* Note: will be set to 0 in startup -> default record                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_BUDGET_TYPE
{
    LSA_BOOL      Enabled;
    LSA_UINT32    Value;
} EDDS_PRM_BUDGET_TYPE;

#define EDDS_FRAME_DROP_BUDGET_CNT            3

typedef struct _EDDS_NC_DATA_CHECK_RECORD_SET_TYPE
{
    LSA_BOOL               Present;  /* record set present ? */
    EDDS_PRM_BUDGET_TYPE  Budget[EDDS_FRAME_DROP_BUDGET_CNT];
} EDDS_NC_DATA_CHECK_RECORD_SET_TYPE;

typedef struct _EDDS_NC_DATA_CHECK_RECORD_SET_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_NC_DATA_CHECK_RECORD_SET_PTR_TYPE;

/* do not change order ! */
#define EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX  0
#define EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX  1
#define EDDS_ERROR_BUDGET_IDX                 2

typedef struct _EDDS_PRM_NC_DATA_TYPE
{
    LSA_UINT32          DropCnt;                      /* drop count last interval */
    LSA_BOOL            CheckEnable;                  /* Check Enable             */
    LSA_UINT8           ErrorStatus;                  /* Error Status             */
    LSA_UINT8           ErrorStatusIndicated;         /* Error Status indicated   */
    EDDS_NC_DATA_CHECK_RECORD_SET_TYPE   RecordSet_A; /* Current RecordSet        */
    EDDS_NC_DATA_CHECK_RECORD_SET_TYPE   RecordSet_B; /* temporary RecordSet      */
} EDDS_PRM_NC_DATA_TYPE;

typedef struct _EDDS_PRM_NC_DATA_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_NC_DATA_PTR_TYPE;

/* CheckEnable (if record set A is present and something to check) */
/* ErrorStatus          (actual Error Status)                      */
/* ErrorStatusIndicated (actual Error Status indicated to user)    */
#define EDDS_DIAG_FRAME_DROP_OK                         0    /* dont change */
#define EDDS_DIAG_FRAME_DROP_ERROR                      1
#define EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED       2
#define EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED       3

/*---------------------------------------------------------------------------*/
/*   PDPortDataAdjust                                                        */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT16          SlotNumber                                    2    */
/*    LSA_UINT16          SubslotNumber                                 2    */
/*    [AdjustDomainBoundary]                             skipped   by EDDS   */
/*    [AdjustMulticastBoundary]                          evaluated by EDDS   */
/*    [AdjustMauType ^ AdjustPortState]                  evaluated by EDDS   */
/*                                                                           */
/*    AdjustDomainBoundary:                                                  */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          DomainBoundary                                4    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*                                                                    -----  */
/*                                                                     16    */
/*    AdjustMulticastBoundary:                                               */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          MulticastBoundary                             4    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*                                                                    -----  */
/*                                                                     16    */
/*    AdjustMauType                                                          */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT16          MAUType                                       2    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*                                                                    -----  */
/*                                                                     12    */
/*                                                                           */
/*    AdjustPortState                                                        */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT16          PortState                                     2    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*                                                                    -----  */
/*                                                                     12    */
/*    AdjustPeerToPeerBoundary                                               */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          PeerToPeerBoundary                            4    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*                                                                    -----  */
/*                                                                     16    */
/*                                                                           */
/*    AdjustDCPBoundary                                                      */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          DCPBoundary                                   4    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*                                                                    -----  */
/*                                                                     16    */
/*    AdjustPreambleLength                                                   */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT16          PreambleLength                                2    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*                                                                    -----  */
/*                                                                     12    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST             0x802F

#define EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_TYPE               0x0202
#define EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE          (EDDS_PRM_RECORD_HEADER_SIZE+6)

/* AdjustDomainBoundary  */
#define EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_TYPE   0x0209
#define EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_SIZE  (EDDS_PRM_RECORD_HEADER_SIZE+10)

/* AdjustMulticastBoundary */
#define EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_TYPE  0x0210
#define EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE (EDDS_PRM_RECORD_HEADER_SIZE+10)

#define EDDS_PRM_PDPORT_DATA_MC_BOUNDARY_BLOCK               1
#define EDDS_PRM_PDPORT_DATA_MC_BOUNDARY_NOT_BLOCK           0

#define EDDS_PRM_PDPORT_DATA_MC_ADJUST_PROPERTIES            0

/* AdjustMauType */
#define EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_TYPE      0x020E
#define EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE     (EDDS_PRM_RECORD_HEADER_SIZE+6)

#define EDDS_PRM_PDPORT_DATA_MAUTYPE_ADJUST_PROPERTIES       0

/* AdjustPortState */
#define EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_TYPE    0x021B
#define EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE   (EDDS_PRM_RECORD_HEADER_SIZE+6)

#define EDDS_PRM_PDPORT_DATA_PORTSTATE_UP                    1
#define EDDS_PRM_PDPORT_DATA_PORTSTATE_DOWN                  2  /* only valid with adjust */
#define EDDS_PRM_PDPORT_DATA_PORTSTATE_TESTING               3
#define EDDS_PRM_PDPORT_DATA_PORTSTATE_UNKNOWN               4

#define EDDS_PRM_PDPORT_DATA_PORTSTATE_ADJUST_PROPERTIES     0

/* AdjustPeerToPeerBoundary */
#define EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_TYPE  0x0224
#define EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_SIZE (EDDS_PRM_RECORD_HEADER_SIZE+10)

/* AdjustDCPBoundary */
#define EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_TYPE  0x0225
#define EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_SIZE (EDDS_PRM_RECORD_HEADER_SIZE+10)

#define EDDS_PRM_PDPORT_DATA_DCP_BOUNDARY_BLOCK               1
#define EDDS_PRM_PDPORT_DATA_DCP_BOUNDARY_NOT_BLOCK           0

#define EDDS_PRM_PDPORT_DATA_DCP_ADJUST_PROPERTIES            0

/* AdjustPreambleLength */
#define EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_TYPE   0x0226
#define EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_SIZE   (EDDS_PRM_RECORD_HEADER_SIZE+6)

#define EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_SHORT                1   // One octet Preamble shall be used
#define EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_LONG                 0   // Seven octets Preamble shall be used
#define EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_PROPERTIES           0

/* MAX Record Size */
/* Note: we add all block but MAUType and PORTState will not occure both! */

#define EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MAX_SIZE          ((EDDS_PRM_RECORD_HEADER_SIZE+6) + \
EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_SIZE    + \
EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE   + \
EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE       + \
EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE     + \
EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_SIZE  + \
EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_SIZE  + \
EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_SIZE)

typedef struct _EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE
{
    LSA_BOOL               Present;                   /* record set present at all ? */
    /* (but maybe no subblock!)    */
    LSA_BOOL               MulticastBoundaryPresent;
    LSA_BOOL               MAUTypePresent;
    LSA_BOOL               PortStatePresent;
    LSA_BOOL               DCPBoundaryPresent;
    LSA_BOOL               PreambleLengthPresent;
    LSA_UINT8              LinkStat;                  /* SpeedMode form MAUType if present */
    LSA_UINT16             SlotNumber;
    LSA_UINT16             SubSlotNumber;
    LSA_UINT32             MulticastBoundary;
    LSA_UINT16             MulticastBoundaryAP;
    LSA_UINT16             MAUType;
    LSA_UINT16             MAUTypeAP;
    LSA_UINT16             PortState;
    LSA_UINT16             PortStateAP;
    LSA_UINT32             DCPBoundary;
    LSA_UINT16             DCPBoundaryAP;
    LSA_UINT16             PreambleLength;
    LSA_UINT16             PreambleLengthAP;
    LSA_UINT32             RecordLength;                                      /* record length if "Present" */
    LSA_UINT8              Record[EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MAX_SIZE]; /* record if "Present" */
} EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE;

typedef struct _EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE;

typedef struct _EDDS_PRM_PORT_DATA_TYPE
{
    LSA_BOOL                                LesserCheckQuality;              /* Flag form CDB.           */
    EDD_UPPER_PORTID_MODE_PTR_TYPE          pPortIDModeArray;                /* allocated on startup     */
    /* (one entry only!)        */
    EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE  RecordSet_A[EDDS_MAX_PORT_CNT];   /* Current RecordSet        */
    EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE  RecordSet_B[EDDS_MAX_PORT_CNT];   /* temporary RecordSet      */
} EDDS_PRM_PORT_DATA_TYPE;

typedef struct _EDDS_PRM_PORT_DATA_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_PORT_DATA_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/*   PDSCFDataCheck (= SendClock factor check)                               */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT16          SendClockFactor                               2    */
/*    LSA_UINT8           Data[x]                                       x    */
/*                                                                    -----  */
/*                                                                      8+x  */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK        0x10000

#define EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_TYPE          0xF000
#define EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MIN_SIZE     (EDDS_PRM_RECORD_HEADER_SIZE+2)
#define EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MAX_SIZE	  (EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MIN_SIZE+4) /* x == 4 */

typedef struct _EDDS_PRM_PDSCF_DATA_CHECK_RECORD_TYPE
{
    LSA_BOOL               Present;              								/* record set present ? */
    LSA_UINT32             RecordLength;                                        /* record length if "Present" */
    LSA_UINT8              Record[EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MAX_SIZE];   /* record if "Present" */
    LSA_UINT16			   SCF;
} EDDS_PRM_PDSCF_DATA_CHECK_RECORD_TYPE;

typedef struct _EDDS_PRM_PDSCF_DATA_CHECK_TYPE
{
    EDDS_PRM_PDSCF_DATA_CHECK_RECORD_TYPE 	RecordSet_A;
    EDDS_PRM_PDSCF_DATA_CHECK_RECORD_TYPE 	RecordSet_B;
} EDDS_PRM_PDSCF_DATA_CHECK_TYPE;

typedef struct _EDDS_PRM_PDSCF_DATA_CHECK_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_PDSCF_DATA_CHECK_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDPortMrpDataAdjust                                                     */
/*   Note: We only support WRITE ! and only check for Record to be written   */
/*   (no validation!)                                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_MRP_DATA_TYPE
{
    LSA_BOOL                             Present_A[EDDS_MAX_PORT_CNT];
    LSA_BOOL                             Present_B[EDDS_MAX_PORT_CNT];
    LSA_BOOL                             AtLeastTwoRecordsPresentB;      /* TRUE if at least 2 records present within */
    /* Record set B on END. helper var  */
} EDDS_PRM_MRP_PORT_DATA_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDPortMrpIcDataAdjust                                                   */
/*   Note: We only support WRITE ! and only check for Record to be written   */
/*   (no validation!)                                                        */
/*---------------------------------------------------------------------------*/
typedef struct _EDDS_PRM_MRP_IN_DATA_TYPE
{
    LSA_BOOL                             Present_A[EDDS_MAX_PORT_CNT];
    LSA_BOOL                             Present_B[EDDS_MAX_PORT_CNT];
    LSA_BOOL                             AtLeastOneRecordPresentB;
} EDDS_PRM_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_DATA_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDInterfaceMrpDataAdjust                                                */
/*   Note: We only support WRITE ! and only check for Record to be written   */
/*   (no validation!)                                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_MRP_INTERFACE_DATA_TYPE
{
    LSA_BOOL                             Present_A;
    LSA_BOOL                             Present_B;
} EDDS_PRM_MRP_INTERFACE_DATA_TYPE;


/*---------------------------------------------------------------------------*/
/*   PDPortStatistic                                                         */
/*---------------------------------------------------------------------------*/
#define EDDS_PRM_PDPORT_STATISTIC_RECORD_INDEX                      0x8072
#define EDDS_PRM_PDPORT_STATISTIC_RECORD_DATA_LENGTH                0x0020
#define EDDS_PRM_PDPORT_STATISTIC_BLOCKTYPE                         0x0251
#define EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONHIGH                  (EDDS_PRM_BLOCK_VERSION_HIGH)
#define EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONLOW                   (EDDS_PRM_STATISTIC_BLOCK_VERSION_LOW)
#define EDDS_PRM_PDPORT_STATISTIC_SIZE								(EDDS_PRM_RECORD_HEADER_SIZE+26)


/*---------------------------------------------------------------------------*/
/*   PDSetDefaultPortStates                                                  */
/*---------------------------------------------------------------------------*/
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    (                                                                      */
/*      LSA_UINT8           PortId                                      1    */
/*      LSA_UINT8           PortState                                   1    */
/*      LSA_UINT8           Padding                                     1    */
/*      LSA_UINT8           Padding                                     1    */
/*    )*                                                                     */
/*                                                                           */
/* PortID: 1..255 (a valid PortId)                                           */
/* each PortId must be present Port and only appears once within record!     */
/* Multiple PortId settings may be present within record.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDSET_DEF_PORTSTATES    0x10001

#define EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_TYPE          0xF001
#define EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_MIN_SIZE     (EDDS_PRM_RECORD_HEADER_SIZE+2)

typedef struct _EDDS_PRM_RECORD_SET_PORT_STATES_TYPE
{
    LSA_BOOL                             PortStatePresent;  /* is PortState present? */
    LSA_UINT8                            PortState;
} EDDS_PRM_RECORD_SET_PORT_STATES_TYPE;

/* PortState : */
#define EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_DISABLE  0x01
#define EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_BLOCKING 0x02

typedef struct _EDDS_PRM_SET_DEF_PORTSTATES_TYPE
{
    LSA_BOOL                             Present_A;
    LSA_BOOL                             Present_B;
    EDDS_PRM_RECORD_SET_PORT_STATES_TYPE RecordSet_A[EDDS_MAX_PORT_CNT];
    EDDS_PRM_RECORD_SET_PORT_STATES_TYPE RecordSet_B[EDDS_MAX_PORT_CNT];
} EDDS_PRM_SET_DEF_PORTSTATES_TYPE;

typedef struct _EDDS_PRM_SET_DEF_PORTSTATES_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_SET_DEF_PORTSTATES_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDNRTFeedInLoadLimitation (= Feed-In Load Limitation)                   */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT16          Reserved                                      2    */
/*    LSA_UINT8           LoadLimitationActive                          1    */
/*    LSA_UINT8           IO_Configured                                 1    */
/*    LSA_UINT16          Reserved                                      2    */
/*                                                                    -----  */
/*                                                                     12    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDNRT_FEED_IN_LOAD_LIMITATION        0x10003

#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_TYPE          0xF004
#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_SIZE         (EDDS_PRM_RECORD_HEADER_SIZE+6)
#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_LENGTH       (EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_BLOCK_SIZE-4)

#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_INACTIVE            0x00
#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_ACTIVE              0x01

#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_NOT_CONFIGURED   0x00
#define EDDS_PRM_PDNRT_FEED_IN_LOAD_LIMITATION_IO_CONFIGURED       0x01

typedef struct _EDDS_PDNRT_FILL_RECORD_SET_TYPE
{
    LSA_BOOL             Present;              /* record set present ? */
    LSA_UINT8            LoadLimitationActive; /* FILL active: 0: INACTIVE, 1: ACTIVE */
    LSA_UINT8            IO_Configured;        /* IO_Configured: 0: NO, 1: YES */
} EDDS_PDNRT_FILL_RECORD_SET_TYPE;

typedef struct _EDDS_PDNRT_FILL_RECORD_SET_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PDNRT_FILL_RECORD_SET_PTR_TYPE;

typedef struct _EDDS_PRM_PDNRT_FILL_DATA_TYPE
{
    EDDS_PDNRT_FILL_RECORD_SET_TYPE   RecordSet_A; /* Current RecordSet        */
    EDDS_PDNRT_FILL_RECORD_SET_TYPE   RecordSet_B; /* temporary RecordSet      */
} EDDS_PRM_PDNRT_FILL_DATA_TYPE;

typedef struct _EDDS_PRM_PDNRT_FILL_DATA_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_PDNRT_FILL_DATA_PTR_TYPE;


/** macro to set the ErrorOffset
 *
 * @param   pDetailErr      pointer to struct EDDS_PRM_DETAIL_ERR_TYPE
 * @param   _ErrOffset      LSA_UINT32 value for ErrOffset
 */
#define EDDS_PRM_SET_ERR_OFFSET(pDetailErr, _ErrOffset) \
{ \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)(pDetailErr))->ErrOffset = (LSA_UINT32)(_ErrOffset); \
}

/** macro to set all values for PrmDetailErr
*
* @param[in]   pDetailErr      pointer to struct EDDS_PRM_DETAIL_ERR_TYPE
* @param[in]   _ErrPortID      LSA_UINT16 value for ErrPortID
* @param[in]   _ErrIndex       LSA_UINT32 value for ErrIndex
* @param[in]   _ErrOffset      LSA_UINT32 value for ErrOffset
* @param[in]   _ErrFault       LSA_UINT8 value for ErrFault
*/
#define EDDS_PRM_SET_DETAIL_ERR(pDetailErr, _ErrPortID, _ErrIndex, _ErrOffset, _ErrFault)     \
{                                                                                             \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrPortID   = (LSA_UINT16)(_ErrPortID);       \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrIndex    = (LSA_UINT32)(_ErrIndex);        \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrOffset   = (LSA_UINT32)(_ErrOffset);       \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrFault    =  (LSA_UINT8)(_ErrFault);        \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrValid    =  LSA_TRUE;                      \
}

/** macro to set values for EDD_RQB_PRM_END_TYPE
 *
 * @param   pPrmEnd         pointer to struct EDD_RQB_PRM_END_TYPE
 * @param   _err_port_id    LSA_UINT16 value for err_port_id
 * @param   _err_index      LSA_UINT32 value for err_index
 * @param   _err_offset     LSA_UINT32 value for err_offset
 * @param   _err_fault      LSA_BOOL value for err_fault
 */
#define EDDS_RQB_PRM_END_SET(pPrmEnd, _err_port_id, _err_index, _err_offset, _err_fault) \
{                                                                                       \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_port_id  = (_err_port_id);       \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_index    = (_err_index);         \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_offset   = (_err_offset);        \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_fault    = (_err_fault);         \
}


/** macro to first init PrmDetailErr with default values
 *
 * @param   pDetailErr      pointer to struct EDDS_PRM_DETAIL_ERR_TYPE
 */
#define EDDS_PRM_INIT_DETAIL_ERR(pDetailErr) \
{ \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)(pDetailErr))->ErrPortID = EDD_PRM_ERR_PORT_ID_DEFAULT; \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)(pDetailErr))->ErrIndex  = EDD_PRM_ERR_INDEX_DEFAULT; \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)(pDetailErr))->ErrOffset = EDD_PRM_ERR_OFFSET_DEFAULT; \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)(pDetailErr))->ErrFault  = EDD_PRM_ERR_FAULT_DEFAULT; \
    ((EDDS_PRM_DETAIL_ERR_PTR_TYPE)(pDetailErr))->ErrValid  = EDDS_PRM_ERR_VALID_DEFAULT; \
}

typedef struct _EDDS_PRM_DETAIL_ERR_TYPE
{
    LSA_BOOL        ErrValid;
    LSA_UINT8       ErrFault;
    LSA_UINT16      ErrPortID;
    LSA_UINT32      ErrIndex;
    LSA_UINT32      ErrOffset;

    /* offsets that are set in prmwrite but error appears in prmend */
    LSA_UINT32      RecordOffset_MauType;

    /* special info for Change Media Type */
    //LSA_UINT8       PortparamsNotApplicable[EDD_CFG_MAX_PORT_CNT];
} EDDS_PRM_DETAIL_ERR_TYPE;

typedef struct _EDDS_PRM_DETAIL_ERR_TYPE         EDDS_LOCAL_MEM_ATTR *    EDDS_PRM_DETAIL_ERR_PTR_TYPE;

/* cycles of PrmCommit state machine */
typedef enum _EDDS_PRM_COMMIT_STATE
{
    EDDS_PRM_COMMIT_STATE_SYNCHRONOUS = 0,
    EDDS_PRM_COMMIT_STATE_MRP,
    EDDS_PRM_COMMIT_STATE_PDPORT,
    EDDS_PRM_COMMIT_STATE_SETPORT
}EDDS_PRM_COMMIT_STATE;

/* cycles of PrmCommit state machine of PDPort */
typedef enum _EDDS_PRM_COMMIT_PDPORT_STATE
{
    EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY = 0,
    EDDS_PRM_COMMIT_PDPORT_STATE_MC_BOUNDARY_LL,
    EDDS_PRM_COMMIT_PDPORT_STATE_DCP_BOUNDARY,
    EDDS_PRM_COMMIT_PDPORT_STATE_DCP_BOUNDARY_LL,
    EDDS_PRM_COMMIT_PDPORT_STATE_NO_MAUTYPE,
    EDDS_PRM_COMMIT_PDPORT_STATE_MAUTYPE_PHY_OFF,
    EDDS_PRM_COMMIT_PDPORT_STATE_MAUTYPE_PHY_ON,
    EDDS_PRM_COMMIT_PDPORT_STATE_FINISH_PORT
}EDDS_PRM_COMMIT_PDPORT_STATE;

/* cycles of PrmCommit state machine of PDPort */
typedef enum _EDDS_PRM_COMMIT_MRP_STATE
{
    EDDS_PRM_COMMIT_MRP_INIT_STATE = 0,
    EDDS_PRM_COMMIT_MRP_1_FWD_STATE_LL,
    EDDS_PRM_COMMIT_MRP_2_FWD_STATE_LL,
    EDDS_PRM_COMMIT_MRP_IN_1_FWD_STATE_LL,
    EDDS_PRM_COMMIT_MRP_IN_2_FWD_STATE_LL,
    EDDS_PRM_COMMIT_MRP_SAVE_PORT_STATES_STATE
}EDDS_PRM_COMMIT_MRP_STATE;

/*---------------------------------------------------------------------------*/

/* PrmState */
typedef enum _EDDS_PRM_STATE
{
    EDDS_PRM_STATE_PREPARED,	/* PREPARED for new param sequence  */
    EDDS_PRM_STATE_WRITE,		/* at least one WRITE after PREPARED*/
    EDDS_PRM_STATE_END			/* END arrived (await commit)       */
}EDDS_PRM_STATE;

typedef enum _EDDS_PRMCHANGEPORT_STATE
{
	EDDS_PRMCHANGEPORT_STATE_ISPULLED,
    EDDS_PRMCHANGEPORT_STATE_GETLINKSTATE,
    EDDS_PRMCHANGEPORT_STATE_PLUGGING,
    EDDS_PRMCHANGEPORT_STATE_PLUGINPROGRESS,
    EDDS_PRMCHANGEPORT_STATE_PHYPOWERUP,
    EDDS_PRMCHANGEPORT_STATE_ISPLUGGED,
    EDDS_PRMCHANGEPORT_STATE_PULLINPROGRESS,
    EDDS_PRMCHANGEPORT_STATE_PHYPOWERDOWN
}EDDS_PRMCHANGEPORT_STATE;

typedef struct _EDDS_PRM_TYPE
{
    LSA_UINT32                HandleCnt;           /* Number ob Handles(only 1)*/
    EDD_HANDLE_LOWER_TYPE     LowerHandle;         /* lower Channel handle           */
    EDDS_PRM_STATE            PrmState;            /* Actual State             */
    EDDS_PRMCHANGEPORT_STATE  PrmChangePortState[EDDS_MAX_PORT_CNT];  /* Actual State of ChangePort-StateMachine */
    EDDS_TIMER_TYPE           Timer;               /* cylic One second timer   */
    EDDS_RQB_LIST_TYPE        PrmIndReq[EDDS_MAX_PORT_CNT+1];  /* Indication queues. 0 = Interface   */
    LSA_BOOL                  DoIndicate[EDDS_MAX_PORT_CNT+1]; /* Indicate for port. 0 = Interface  */
    LSA_BOOL                  PRMUsed;             /* PRM  was at least once   */
    /* written.                 */
    LSA_BOOL                  FirstCommitExecuted; /* TRUE after first COMMIT  */
    /* after startup was execut.*/
    EDDS_PRM_NC_DATA_TYPE     FrameDrop;           /* PDNC Record              */
    EDDS_PRM_PORT_DATA_TYPE   PortData;            /* PDPort Record            */
    EDDS_PRM_MRP_PORT_DATA_TYPE      MRPPort;      /* PDPortMrpDataAdjust      */
    EDDS_PRM_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_DATA_TYPE SupportsMRPInterconnPortConfig;    /* PDPortMrpIcDataAdjust    */
    EDDS_PRM_MRP_INTERFACE_DATA_TYPE MRPInterface;  /* PDInterfaceMrpDataAdjust */
    EDDS_PRM_SET_DEF_PORTSTATES_TYPE DefPortStates; /* PDSetDefaultPortStates   */
    EDDS_PRM_PDSCF_DATA_CHECK_TYPE   PDSCFDataCheck;/* PDSCFDataCheck           */
    EDDS_PRM_PDNRT_FILL_DATA_TYPE    PDNRTFill;     /* PNNRT Fill Record        */

    EDDS_PRM_DETAIL_ERR_TYPE  PrmDetailErr;

    /* backup for values used in several states of PrmCommit state machine */
    volatile EDDS_PRM_COMMIT_STATE      currentState;
    EDDS_PRM_COMMIT_PDPORT_STATE        currentStatePrmCommitPDPort;
    EDDS_PRM_COMMIT_MRP_STATE           currentStatePrmCommitMrp;

    LSA_BOOL                            PortStateSet[EDDS_MAX_PORT_CNT];
    LSA_BOOL                            fireExtLinkChange[EDDS_MAX_PORT_CNT];
    EDD_RQB_SWI_SET_PORT_STATE_TYPE     PortStates;
    EDD_RQB_PORTID_MODE_TYPE            PortIDMode[EDDS_MAX_PORT_CNT];
    LSA_BOOL                            MrpDisabled;
    LSA_UINT16                          currentPort;
    LSA_BOOL                            firstPartDone;
    LSA_UINT32                          MACAddrLow;

} EDDS_PRM_TYPE;

typedef struct _EDDS_PRM_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_PTR_TYPE;

typedef enum _EDDS_PRM_TRIGGER_TYPE
{
	EDDS_PRM_TRIGGER_TYPE_SRV_PRM_PREPARE,
	EDDS_PRM_TRIGGER_TYPE_SRV_PRM_WRITE,
	EDDS_PRM_TRIGGER_TYPE_SRV_PRM_END,
	EDDS_PRM_TRIGGER_TYPE_SRV_PRM_COMMIT,
	EDDS_PRM_TRIGGER_TYPE_PRM_COMMIT_FINISH,
	EDDS_PRM_TRIGGER_TYPE_SRV_PRM_CHANGE_PORT,
    EDDS_PRM_TRIGGER_TYPE_ERROR                                     /* this state is default value for error */
}EDDS_PRM_TRIGGER_TYPE;

typedef enum _EDDS_PRMCHANGEPORT_TRIGGER_TYPE
{
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE_UPDATE,
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PULL,
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_PREPARE,
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_CHANGE_PORT_PLUG_COMMIT,
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_END,
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE_SRV_PRM_COMMIT,
    EDDS_PRMCHANGEPORT_TRIGGER_TYPE_ERROR                           /* this state is default value for error */
}EDDS_PRMCHANGEPORT_TRIGGER_TYPE;

typedef struct _EDDS_PRMCHANGEPORT_TYPE
{
    /* values for PlugPrepare ---> */
	LSA_UINT8						PortparamsNotApplicable; /// return value for PlugPrepare
	LSA_UINT8 						LinkSpeedModeApplicable; /// saved during PlugPrepare, used during PlugCommit
	/* <--- values for PlugPrepare */

    /* values for PlugCommit ---> */
	LSA_UINT8 						ApplyDefaultPortparams; /// input value from PlugCommit
	/* <--- values for PlugCommit */

}EDDS_PRMCHANGEPORT_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDInterfaceMrpDataAdjust , PDPortMrpDataAdjust                          */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDINTERFACE_MRP_DATA_ADJUST             0x8052
#define EDDS_PRM_RECORD_INDEX_PDPORT_MRP_DATA_ADJUST                  0x8053

/*---------------------------------------------------------------------------*/
/*                              PDPortMrpIcDataAdjust                        */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDPORT_MRP_IC_DATA_ADJUST               0x8055

/* ---------------------------------------------------------------------------- */
/*   User specific records                                                      */
/*   The following records are not used by EDDS but they must be known          */
/* ---------------------------------------------------------------------------- */
#define EDDS_PRM_PDCONTROL_PLL_RECORD_INDEX                     0xB050
#define EDDS_PRM_PDTRACE_UNIT_CONTROL_RECORD_INDEX              0xB060
#define EDDS_PRM_PDTRACE_UNIT_DATA_RECORD_INDEX                 0xB061

/*****************************************************************************/
/*  Default error values                                                     */
/*****************************************************************************/

/** Set to LSA_FALSE because there is no error yet */
#define EDDS_PRM_ERR_VALID_DEFAULT      LSA_FALSE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DEVICE_GLOB_TYPE
{
	EDDS_INTERNAL_SCHED_TYPE        scheduler;	    /* internal scheduler data */
    /* --- Setup --------------- */
    EDDS_DSB_TYPE                   DSBParams;  /* DSB save.                 */
    /* -- Low-Level-Ethernet---- */
    /* depends on hardware       */
    EDDS_LOCAL_EDDS_LL_TABLE_PTR_TYPE	pLLFunctionTable;   /* Low-Level-function table     */
    EDDS_HANDLE							pLLManagementData;   /* Low-Level-Management data     */
    LSA_UINT32						TraceIdx;
    LSA_BOOL                        HWIsSetup;  /* LSA_TRUE if device_setup  */
    /* was successful.           */
    EDDS_INTERNAL_ERROR_TYPE        Errors;     /* Internal Errors for       */
    /* Debugging                 */

    EDDS_HW_PARAM_TYPE              HWParams;   /* HW-Parameters             */

    /* actual Link-Status        */
    EDDS_LINK_STAT_TYPE             LinkStatus[EDDS_MAX_PORT_CNT+1];  /* 0 = Auto */
    EDDS_PRMCHANGEPORT_TYPE         PrmChangePort[EDDS_MAX_PORT_CNT];

    EDDS_LED_BLINK_TYPE             LEDBLink;   /* LED blink Status          */
    EDDS_PRM_TYPE                   Prm;        /* Prm/Diagnosis             */

    /* queue for pending RQBs in scheduler */
    EDDS_RQB_LIST_TYPE              GeneralRequestQueue;   /* General Request Queue  */
    EDDS_RQB_LIST_TYPE              HighPriorGeneralRequestQueue;   /* General Request Queue for high prior RQBs */

    /* RQB queue for filtering ARP Frames in RQB-Context */
    EDDS_RQB_LIST_TYPE              FilterARPFrameQueue;   /* ARP Request Queue  */
    EDDS_INTERNAL_REQUEST_TYPE      FilterARPFrameTrigger;   /* trigger RQB context request */
    LSA_BOOL                        FilterARPFrameTriggerPending;/* flag for sending GeneralRequest_ARPFILTER_Trigger */

    /* RQB queue for filtering DCP Frames in RQB-Context */
    EDDS_RQB_LIST_TYPE              FilterDCPFrameQueue;   /* DCP Request Queue  */
    EDDS_INTERNAL_REQUEST_TYPE      FilterDCPFrameTrigger;   /* trigger RQB context request */
    LSA_BOOL                        FilterDCPFrameTriggerPending;/* flag for sending GeneralRequest_DCPFILTER_Trigger */

    /* RQB queue for filtering DCPHELLO Frames in RQB-Context */
    EDDS_RQB_LIST_TYPE              FilterDCPHELLOFrameQueue;   /* DCP Request Queue  */
    EDDS_INTERNAL_REQUEST_TYPE      FilterDCPHELLOFrameTrigger;   /* trigger RQB context request */
    LSA_BOOL                        FilterDCPHELLOFrameTriggerPending;/* flag for sending GeneralRequest_DCPFILTER_Trigger */

    /* RQB queue for finishing RQBs in RQB-Context */
    EDDS_RQB_LIST_TYPE              GeneralRequestFinishedQueue;   /* Finished Request Queue  */
    EDDS_INTERNAL_REQUEST_TYPE      GeneralRequestFinishedTrigger;   /* trigger RQB context request */
    LSA_BOOL                        GeneralRequestFinishedTriggerPending;/* flag for sending GeneralRequestFinishedTrigger */

    LSA_BOOL                        triggerSchedulerPending;    /* scheduler needs a trigger for finishing some pending actions */

    EDDS_INTERNAL_REQUEST_TYPE      triggerSchedulerRequest;   /* trigger scheduler from scheduler/RQB context */
    LSA_BOOL                        triggerSchedulerRequestPending; /* flag for sending triggerSchedulerRequest from scheduler context */

    LSA_BOOL                        triggerSchedulerHighPriorRequestPending; /* scheduler needs a trigger for finishing a pending high priority request */

    EDDS_INTERNAL_REQUEST_TYPE      SchedulerEventsDone;   /* trigger RQB context (scheduler events done) */
    LSA_UINT32                      SchedulerEventsDonePending; /* flags for sending triggerSchedulerEventsDone */

    /* --- Rx-Buffer Info ------ */
    LSA_UINT32                      RxMaxByteCntPerCycle;    /* maximum byte count to receive per cycle */
                                                             /* count depending on scheduler cycle */
    LSA_UINT32                      RxRemainingByteCntPerCycle;   /* current byte count of already received frames */

    /* buffer            */
    /* (user-parameter   */
    /*  from DPB !)      */
    /* --- Tx handling --------- */
    LSA_UINT32                      TxBuffCnt;  /* Number of Tx-Resouces get */
    /* from low-level functions  */
    LSA_UINT32                      TxFreeIdx;  /* Idx to pTxInfo for next   */
    /* frame to be send.         */
    LSA_UINT32                      TxCheckIdx; /* Idx to pTxInfo for next   */
    /* frame finished sending    */
    LSA_UINT32                      TxInProgCnt;/* Number of Tx in progress. */
    /* in progress.         */
    LSA_UINT32                      TxMaxByteCntPerCycle;/* max number of bytes in cycle, that could be transfered. */
    LSA_UINT32                      actTxBytesCycle; /* counter of actual send bytes in actual cycle */

    /* per Cycle for actual speed*/
    /* for NRT/aSRT              */
    EDDS_TX_INFO_PTR_TYPE           pTxInfo;    /* Infos about current tx in */
    /* progress. Array with TxCnt*/
    /* entrys                    */
    EDDS_MC_MAC_INFO_PTR_TYPE       pMCInfo;    /* Multicast MAC management  */
    /* --hw-independend statistic*/
    EDDS_STAT_TX_TYPE               TxStats;    /* statistics for Tranmit    */
    EDDS_STAT_RX_TYPE               RxStats;    /* statistics for Receive    */
    EDDS_STATISTICS_TYPE            DropStats;  /* temporary storage for Drop Counter / PDNC PRM  */
    EDDS_STATISTICS_TYPE            DropStats_Recent; /* temporary storage for recent Drop Counter */

    /* statistics when read      */

    /* MaxPortCntOfAllEDD :    Number of all interfaces of all EDDs              */
    /*                         max: EDD_CFG_MAX_INTERFACE_CNT                    */
    LSA_UINT16                          MaxPortCntOfAllEDD;

    /* MaxInterfaceCntOfAllEDD Number of all ports of all EDDs                   */
    /*                         max: EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE           */
    LSA_UINT16                          MaxInterfaceCntOfAllEDD;

    /* pointer to tx transfer memory (only valid if ZeroCopyInterface is FALSE */
    EDDS_TRANSFER_BUFFER_ARRAY  pTxTransferMem;

    /* traditional zero copy interface is used in EDDS, if this property is set TRUE */
    LSA_BOOL ZeroCopyInterface;

    /* time between cyclic scheduler calling (IO and NRT mode)
     * Actual used scheduler cycle is set in scheduler.CycleTimeMS
     */
    LSA_UINT16 schedulerCycleIO;  /* IO mode; this RO parameter is set on DevOpen. */
    LSA_UINT16 schedulerCycleNRT; /* NRT mode; this RO parameter is set on DevOpen. */
    LSA_UINT8  schedulerRecurringTaskMS; /* aimed recurring task execution time in milliseconds */

    /* if ProviderCnt = 0 and ConsumerCnt = 0   */
    /* then IsCrtUsed is set to FALSE           */
    LSA_BOOL                        IsSrtUsed;  /* is SRT used or not */

    LSA_BOOL    IO_Configured;                     /* is IO setup for this EDDS instance;    */
                                                   /* initially set on DEV_OPEN depending on */
                                                   /* whether SRT is supported or not,       */
                                                   /* can be changed by FILL record          */
    LSA_BOOL    IO_ConfiguredDefault;              /* default from DEV_OPEN                  */
    LSA_BOOL    FeedInLoadLimitationActive;        /* current activation status of FILL      */
    LSA_BOOL    FeedInLoadLimitationActiveDefault; /* FILL default behaviour from DEV_OPEN   */
    LSA_UINT32  actTxPacketsFinishCycle;           /* counter of processed finished send packets in actual cycle */
    LSA_UINT32  actRxPacketsCycle;                 /* counter of processed received packets in actual cycle */
    LSA_UINT32  RxPacketsQuota;                    /* quota of processed received packets for each edds_scheduler call (set on IO/NRT-mode change) */
    LSA_UINT32  RxPacketsQuota_IO;                 /* quota of processed received packets for each edds_scheduler call in IO mode (set from DPB) */
    LSA_UINT32  RxPacketsQuota_NRT;                /* quota of processed received packets for each edds_scheduler call in NRT mode (set from DPB) */

    LSA_BOOL    NRT_UseNotifySchedulerCall;        /* in IO/NRT mode, no direct call of edds_scheduler should be done (parameter is only used in RQB context) */

    LSA_UINT64  lastResetTxBandwithTime;           /* remember time for resetting Tx bandwidth */
} EDDS_DEVICE_GLOB_TYPE;

typedef struct _EDDS_DEVICE_GLOB_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE;

typedef EDDS_DPB_SWI_TYPE EDDS_DDB_SWI_TYPE;

typedef enum
{
    EDDS_TIMER_LED_BLINK    = 0,
    EDDS_TIMER_PRM          = 1,
    EDDS_TIMER_MAX          = 2
} EDDS_TIMER_ENUM;

/*===========================================================================*/
/* Global Timer management                                                   */
/* This is used to asign a LSA-Timer ID to an static RQB                     */
/* This management is deviceindependend and used for all devices.            */
/*===========================================================================*/

/* Max EDDS Timers needed.                                                   */
/* 1. Tx-Timeout                                                             */
/* 2. Consumer cycle Timer                                                   */
/* 3. Provider cycle Timer                                                   */
/* 5. LED-Blink Timer          (if configured)                               */
/* 6. Diag Timer               (if configured)                               */

typedef struct EDDS_TIMER_MGMT
{
    // compare value to current system time
    // if 0, then disabled
    LSA_UINT64 nsExpireTime;

    // which value to add to nsEcpireTime for next cycle (Nanoseconds)
    // 0 if One Shot Timer
    LSA_UINT32 nsCycleTimeBase;

    // call this method when SystemTime >= ExpireTime
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, TimeoutMethod)(EDDS_LOCAL_DDB_PTR_TYPE pDDB);
} EDDS_TIMER_MGMT_TYPE;

/*---------------------------------------------------------------------------*/
/* main devicemanagement                                                     */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DDB_TYPE
{
    EDDS_LOCAL_DDB_PTR_TYPE         pNext;      /* Pointer to next device    */
    EDDS_LOCAL_DDB_PTR_TYPE         pPrev;      /* Pointer to previous device*/
    EDDS_HANDLE                     hDDB;       /* Device-handle (currently  */
    EDDS_SYS_HANDLE                 hSysDev;    /* system handle of device   */
    EDDS_TIMER_MGMT_TYPE            timerMgmt[EDDS_TIMER_MAX];
    /* pointer) to EDDS_DEVICE_TYP*/
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;      /* Global Device management  */

    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRT;      /* NRT-specific management   */

    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pSRT;      /* SRT-specific management   */

    EDDS_DDB_SWI_TYPE				 SWI;		/* switch specific management */

    EDDS_DDB_HDB_MGMT_TYPE           HDBMgmt;   /* HDB management */

    EDDS_DPB_FEATURE_TYPE            FeatureSupport; /* currently only used for HSYNC */

    EDDS_INTERNAL_PERFORMANCE_STATS_TYPE PerformanceStatistics;
} EDDS_DDB_TYPE;


/*****************************************************************************/
/*  end of file edds_dev.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_DEV_H */
