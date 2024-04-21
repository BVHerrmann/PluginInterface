
#ifndef _PACKET32_USR_H                 /* ----- reinclude-protection ----- */
#define _PACKET32_USR_H

#ifdef __cplusplus                      /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: packet32_usr.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level user header (PCAP)                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  23.06.10    AM    initial version.                                       */
#endif
/*****************************************************************************/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_OPEN(LSA_VOID_PTR_TYPE pLLManagement,
	EDDS_HANDLE				            hDDB,
	EDDS_UPPER_DPB_PTR_TO_CONST_TYPE    pDPB,
	LSA_UINT32				            TraceIdx,
	EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE    pCaps);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SETUP(	LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_DSB_PTR_TYPE pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SHUTDOWN(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV(	LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV_PROVIDE(
		LSA_VOID_PTR_TYPE pLLManagement, EDD_UPPER_MEM_PTR_TYPE pBufferAddr);

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV_TRIGGER(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE pBufferAddr, LSA_UINT32 Length,
		LSA_UINT32 PortID);

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_TRIGGER(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_STS(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_STATS(
        LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, EDDS_LOCAL_STATISTICS_PTR_TYPE pStats);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
		EDDS_LOCAL_MEM_U8_PTR_TYPE  pMediaType,
		EDDS_LOCAL_MEM_U8_PTR_TYPE  pIsPOF,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE  pLinkSpeedModeConfigured);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT8 * const pLinkStat,
		LSA_UINT8 * const pPHYPower);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_ENABLE(
        LSA_VOID_PTR_TYPE pLLManagement, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_DISABLE(
        LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL DisableAll, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_UINT32 EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECURRING_TASK(
        LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL hwTimeSlicing);

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_BACKUP_MODE(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_RESTORE_MODE(
	    LSA_VOID_PTR_TYPE pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_SET_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_CHANGE_PORT(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT16        PortID,
        LSA_UINT8         isPulled);


/*---------------------------------------------------------------------------*/
/* Define the PACKET32_LL_SHADOWQUEUE_SEND for internal use by packet32 LL   */
/*---------------------------------------------------------------------------*/

typedef struct _PACKET32_LL_SHADOWQUEUE_SEND_ELEMENT
{
    LSA_UINT8* pBuffer;
    LSA_UINT32 length;
} PACKET32_LL_SHADOWQUEUE_SEND_ELEMENT;

typedef struct _PACKET32_LL_SHADOWQUEUE_SEND
{
    LSA_UINT32 size;
    LSA_UINT32 index_start;
    LSA_UINT32 index_next;
    LPPACKET   lpPacket;
    PACKET32_LL_SHADOWQUEUE_SEND_ELEMENT* pElement;

    
} PACKET32_LL_SHADOWQUEUE_SEND;


typedef struct _PACKET32_THREAD_DATA_TYPE
{
  HANDLE    hThread;
  HANDLE    hEvent;
  HANDLE    hMutex;
  DWORD     dwThreadId;
  LSA_BOOL  alive;
} PACKET32_THREAD_DATA_TYPE;


/* only 1 port is supported by packet32 */
#define EDDS_PACKET32_SUPPORTED_PORTS 1

typedef struct _PACKET32_LL_STATS_TYPE
{
    LSA_UINT32      InOctets;
    LSA_UINT32      InUcastPkts;
    LSA_UINT32      InNUcastPkts;
    LSA_UINT32      OutOctets;
    LSA_UINT32      OutUcastPkts;
    LSA_UINT32      OutNUcastPkts;
    LSA_UINT32      InMulticastPkts;
    LSA_UINT32      InBroadcastPkts;
    LSA_UINT32      OutMulticastPkts;
    LSA_UINT32      OutBroadcastPkts;
    LSA_UINT64      InHCOctets;
    LSA_UINT64      InHCUcastPkts;
    LSA_UINT64      InHCMulticastPkts;
    LSA_UINT64      InHCBroadcastPkts;
    LSA_UINT64      OutHCOctets;
    LSA_UINT64      OutHCUcastPkts;
    LSA_UINT64      OutHCMulticastPkts;
    LSA_UINT64      OutHCBroadcastPkts;
}PACKET32_LL_STATS_TYPE;

/*---------------------------------------------------------------------------*/
/* Define the PACKET32_LL_HANDLE_TYPE for use by EDDS                        */
/*---------------------------------------------------------------------------*/
typedef struct _PACKET32_LL_HANDLE_TYPE
{
    LPADAPTER 			pAdapter;
    LSA_UINT32			TraceIdx;
    EDDS_SYS_HANDLE     hSysDev;            /* system handle of device */
    LSA_VOID_PTR_TYPE	pSendBuffer;		/* 1x EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM()*/
    LSA_VOID_PTR_TYPE	pRecvBuffer;		/* 1x EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM()*/
    LPPACKET            pSendPacketStruct;	/* pointer to packet32 user send buffer */
    LPPACKET            pRecvPacketStruct;	/* pointer to packet32 user send buffer */
    LSA_UINT32	        sendPackets;
    struct bpf_hdr*     pRecvPacket;	    /* pointer to actual recv bpf_hdr */
    NDIS_MEDIA_STATE    linkState;		    /* saved link state */
    LSA_UINT32          RxDropCnt;          /* counts received but dropped frames, e.g. oversized frames */
    EDD_MAC_ADR_TYPE    MACAddress;
    EDD_MAC_ADR_TYPE    MACAddressPort[EDDS_PACKET32_SUPPORTED_PORTS]; /* port mac addresses */
    PACKET32_LL_SHADOWQUEUE_SEND sendQueue; /* queues back all buffer from LL_SEND, will be send on LL_SEND_TRIGGER */
    PACKET32_THREAD_DATA_TYPE      thread_data;
    LSA_BOOL            isLinkStateChanged;
    LSA_UINT8           newPHYPower;
    LSA_UINT8           PortStatus[EDDS_PACKET32_SUPPORTED_PORTS];
    PACKET32_LL_STATS_TYPE  LL_Stats;
} PACKET32_LL_HANDLE_TYPE;

/*---------------------------------------------------------------------------*/
/* Packet32 LL specific defines                                              */
/*---------------------------------------------------------------------------*/
#define LLIF_CFG_KERNEL_BUFFER_SIZE		(512*1024)
#define	LLIF_CFG_SEND_BUFFER_SIZE		(128*1024)
#define	LLIF_CFG_RECV_BUFFER_SIZE		(128*1024)

/* MIB2 Support */
#define PACKET32_MIB2_SUPPORTED_COUNTERS  (                                     \
                                            EDD_MIB_SUPPORT_INOCTETS            \
                                          | EDD_MIB_SUPPORT_INUCASTPKTS         \
                                          | EDD_MIB_SUPPORT_INNUCASTPKTS        \
                                          | EDD_MIB_SUPPORT_OUTOCTETS           \
                                          | EDD_MIB_SUPPORT_OUTUCASTPKTS        \
                                          | EDD_MIB_SUPPORT_OUTNUCASTPKTS       \
                                          | EDD_MIB_SUPPORT_INMULTICASTPKTS     \
                                          | EDD_MIB_SUPPORT_INBROADCASTPKTS     \
                                          | EDD_MIB_SUPPORT_OUTMULTICASTPKTS    \
                                          | EDD_MIB_SUPPORT_OUTBROADCASTPKTS    \
                                          | EDD_MIB_SUPPORT_INHCOCTETS          \
                                          | EDD_MIB_SUPPORT_INHCUCASTPKTS       \
                                          | EDD_MIB_SUPPORT_INHCMULTICASTPKTS   \
                                          | EDD_MIB_SUPPORT_INHCBROADCASTPKTS   \
                                          | EDD_MIB_SUPPORT_OUTHCOCTETS         \
                                          | EDD_MIB_SUPPORT_OUTHCUCASTPKTS      \
                                          | EDD_MIB_SUPPORT_OUTHCMULTICASTPKTS  \
                                          | EDD_MIB_SUPPORT_OUTHCBROADCASTPKTS  \
                                          )

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* _PACKET32_USR_H */
