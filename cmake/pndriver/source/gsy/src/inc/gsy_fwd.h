
#ifndef GSY_FW_H                         /* ----- reinclude-protection ----- */
#define GSY_FW_H

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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_fwd.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile of forwarding module                                 */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date       Version                  Who What                             */
/*  2005-03-29 P01.00.00.00_00.01.00.00 JS  initial version.                 */
/*  2005-04-06 P01.00.00.00_00.01.00.01 lrg 32 -> GSY_MAX_PROTOCOLS          */
/*             TLV -> gsy_int.h                                              */
/*  2005-06-02  P03.06.00.00_00.01.00.01 rg  Prototype for PT2               */
/*  2005-09-16  P03.07.00.00_00.08.01.01 js  ps (pico seconds) added.        */
/*  2005-10-24  P03.08.00.00_00.01.01.01 js  gsy_FwdLinkStatus added.        */
/*                                           SendEnabled added.              */
/*  Date        Who   What                                                   */
/*  16.12.05    JS    seperated queueing macros to avoid GCC compiler error. */
/*  14.09.06    JS    added annouce support.                                 */
/*  2007-07-12  P04.00.00.00_00.30.01.00 120707lrg001: RTSync                */
/*  2007-08-29  P04.00.00.00_00.30.04.02 290807lrg001: FwdModeIn, FwdModeOut */
/*              GSY_FWD_ENABLE => GSY_FWD_MODE_ENABLE                        */
/*              GSY_FWD_MODE_ENABLE=GSY_FWD_MODE_OUTPUT & GSY_FWD_MODE_INPUT */
/*  2007-09-24  P04.00.00.00_00.30.06.02 240907lrg001: Enable/ Disable       */
/*              Announce-Multicast-Address                                   */
/*  2007-10-04  P04.00.00.00_00.40.01.02 041007lrg003: Announce SeqId        */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-07-24  P04.01.00.00_00.01.02.03 240708lrg001: ArtsRQ AP00676859     */
/*              Check ClockTopoOk at output port                .            */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-10  P04.01.00.00_00.02.03.02 101208lrg001: Announce mit          */
/*              gsy_DataSend() versenden: ReprovideSync                      */
/*  2009-08-31  P04.02.00.00_00.04.01.02 310809lrg001: GSY_FWD_INPUT for     */
/*              Announce only to user. FwdUserMode now per channel           */  
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2013-05-23  P05.02.00.00_00.04.24.02 AP01539291 Linkerfehler bei         */
/*              Verwendung von C++ compiler (Fkt. ptr als Parameter)         */
/*  2015-06-09  V06.00, Inc07, Feature 1227823, Task 1261424                 */
/*              GSY und CLP Konfigurationsschalter bereinigen                */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                configs                                    */
/*===========================================================================*/

/* --------------------------------------------------------------------------*/  
/* GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL :                             */
/*                                                                           */  
/* If defined we only forward to ports of same EDD-Systemchannel.            */  
/* If not defined we forward to all ports of all EDD-Systemchannels.         */  
/*                                                                           */  
/* Note: Typically we can not forward to all ports because of unsynchronised */  
/*       time stamps between system channel!                                 */  
/*                                                                           */  
/* IMPORTANT: Dont undef this setting, because the implementation for closing*/  
/*            a channel is not fully implemented to support multi channel    */  
/*            Because we dont know yet if we ever need this, we dont support */  
/* --------------------------------------------------------------------------*/  

#define GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL

/* --------------------------------------------------------------------------*/  
/* GSY_CFG_MASTER_TIMEOUT_IN_S    :                                          */
/*                                                                           */  
/* Forwarding module:                                                        */  
/* Time in seconds sync frame data of a master will be stored (for further   */  
/* sync frame sequence id handling). If the time elapses, the entry will be  */  
/* freed and become available for a new master. As long as the entry is used */  
/* we can check the sequence id of the sync frame against a new one.         */  
/*                                                                           */  
/* This timeout starts after a sync was received!                            */  
/*                                                                           */  
/* Important: Must be >= GSY_CFG_MASTER_FU_TIMEOUT_IN_S !!                   */  
/*                                                                           */  
/* --------------------------------------------------------------------------*/  

#define GSY_CFG_MASTER_TIMEOUT_IN_S            5

/* --------------------------------------------------------------------------*/  
/* GSY_CFG_MASTER_FU_TIMEOUT_IN_S    :                                       */
/*                                                                           */  
/* Forwarding module:                                                        */  
/* Timeout in seconds to wait for FollowUp after Sync was received. If the   */  
/* time elapses we dont wait for the followup anymore.                       */  
/*                                                                           */  
/* This timeout starts after a sync was received!                            */  
/*                                                                           */  
/* Important: Must be <= GSY_CFG_MASTER_TIMEOUT_IN_S !!                      */  
/* --------------------------------------------------------------------------*/  

#define GSY_CFG_MASTER_FU_TIMEOUT_IN_S         3

/* --------------------------------------------------------------------------*/  
/* GSY_CFG_FWD_ERTEC_200_DRIFT_CORRECTION                                    */
/*                                                                           */  
/* If defined the forwarding module calculates the drift correction and      */  
/* inserts the value into the delay-field of the FollowUp frame.             */  
/* If not defined, the ERTEC 200 handles the drift correction by hardware.   */  
/*                                                                           */  
/* --------------------------------------------------------------------------*/  

#define GSY_CFG_FWD_ERTEC_200_DRIFT_CORRECTION	

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* number of different protocols 
*/
#define GSY_FWD_MAC_ADDR_CNT             GSY_MAX_PROTOCOLS

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

#define GSY_FRAME_GET_DST_MACID(pFrame) ((((GSY_LOWER_RXBYTE_PTR )pFrame)[5]) & 0x1F)

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

	
/* forward reference */
typedef struct _GSY_FWD_MASTER_TYPE  GSY_LOCAL_MEM_ATTR *   GSY_FWD_MASTER_PTR_TYPE;
typedef struct _GSY_FWD_PORT_TYPE    GSY_LOCAL_MEM_ATTR *   GSY_FWD_PORT_PTR_TYPE;

typedef struct _GSY_FWD_INIT_TYPE
{
	LSA_UINT32  FwdMode;  /* bitfield */
} GSY_FWD_INIT_TYPE;

typedef struct _GSY_FWD_INIT_TYPE  GSY_LOCAL_MEM_ATTR *   GSY_FWD_INIT_PTR_TYPE;

/* --------------------------------------------------------------------------*/
/* FrameInfo                                                                 */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_FRAME_INFO_TYPE
{
	GSY_LOWER_RXBYTE_PTR  pMemRx;
	GSY_LOWER_RXBYTE_PTR  pPduRx;
	LSA_UINT16            IOCount;
	LSA_UINT32            PduOffset;
	LSA_UINT16            PduLen;
	LSA_UINT32            MACAddrId;
	LSA_UINT32            TimeStamp; /* recv time stamp */
	LSA_BOOL              DoFwd;						/*FwdControl */
	LSA_BOOL              DoFwdPort[GSY_CH_MAX_PORTS];	/*FwdControl from EDD */
} GSY_FWD_FRAME_INFO_TYPE;

typedef struct _GSY_FWD_FRAME_INFO_TYPE       * GSY_FWD_FRAME_INFO_PTR_TYPE;
typedef struct _GSY_FWD_FRAME_INFO_TYPE const * GSY_FWD_FRAME_INFO_CONST_PTR_TYPE;

/* --------------------------------------------------------------------------*/
/* Channel Statistics                                                        */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_STATS_TYPE
{
	LSA_UINT32		InvalidTLVCnt;
	LSA_UINT32      SyncRestartCnt;
	LSA_UINT32      SyncOldSeqIDCnt;
	LSA_UINT32		SyncDiscardFUInProgressCnt;
	LSA_UINT32		OutOfFCBCnt;
	LSA_UINT32      FUDontMatchPortOrSeqIDCnt;
	LSA_UINT32		FUNoProperSyncFoundCnt;
	LSA_UINT32		DelayExceededLimitCnt;
	LSA_UINT32      NoFreeMasterSlotCnt;
	LSA_UINT32      AnnounceOldSeqIDCnt;
	LSA_UINT32      AnnounceDiscardFUInProgressCnt;
} GSY_FWD_STATS_TYPE;


/* --------------------------------------------------------------------------*/
/* Forward Control Block                                                     */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_FCB_TYPE  GSY_LOCAL_MEM_ATTR *   GSY_FWD_FCB_PTR_TYPE;

typedef struct _GSY_FWD_FCB_BLINK_TYPE
{
    GSY_FWD_FCB_PTR_TYPE          pBottom;
    GSY_FWD_FCB_PTR_TYPE          pTop;
} GSY_FWD_FCB_BLINK_TYPE;

typedef struct _GSY_FWD_FCB_LINK_TYPE
{
	GSY_FWD_FCB_PTR_TYPE          pNext;   /* used for queuing blocks        */
	GSY_FWD_FCB_PTR_TYPE          pPrev;   /* used for queuing blocks        */
} GSY_FWD_FCB_LINK_TYPE;

typedef struct _GSY_FWD_FCB_TYPE
{
	LSA_UINT32               Status;
	LSA_BOOL                 Sending;        /* If frame currently send  */
	GSY_CH_SYS_PTR           pChSys;         /* SystemChannel FCB belongs*/
	GSY_FWD_FCB_LINK_TYPE    UseQueue;       /* queue for FCBs used by   */
	                                         /* Master or free           */
	GSY_FWD_FCB_LINK_TYPE    SendQueue;      /* queue for output port    */
	GSY_FWD_MASTER_PTR_TYPE  pMaster;        /* master  using this fcb   */ 
	GSY_FWD_PORT_PTR_TYPE    pPort;          /* OutPort using this fcb   */ 
	LSA_UINT32               MACAddrID;      /* MAC-Multicast Idx        */
	                                         /* 0..GSY_FWD_MAC_ADDR_CNT-1*/ 
	LSA_UINT16               SequenceID;
	LSA_UINT32               MasterSeqID;    /* Sequence ID of Master    */
	LSA_UINT32               SyncSendTimeStamp;
	LSA_BOOL                 SyncSendTimeValid; /* 120707lrg001 */
	LSA_UINT32               LineDelayHWNS;  /* Current LineDelay within */
                                             /* Hardware Register in ns. */
	                                         /* (only used with ERTEC400)*/
	                                         /* set after Send confirm of*/
	                                         /* SYNC frame               */ 
} GSY_FWD_FCB_TYPE;


/* Status */

#define GSY_FWD_FCB_STS_FREE                0
#define GSY_FWD_FCB_STS_SYNC_SEND           1
#define GSY_FWD_FCB_STS_FU_WAITING          2
#define GSY_FWD_FCB_STS_FU_SEND             3
#define GSY_FWD_FCB_STS_ANNOUNCE_SEND       4

/* Note to queues:                                                           */
/* a FCB can be in two queues at once:                                       */
/*                                                                           */ 
/* queue 1 (UseQueue)                                                        */
/*    FCBFreeQueue          within Channel   GSY_FWD_FCB_STS_FREE            */
/*    FCBQueue              within Master    GSY_FWD_FCB_STS_SYNC_SEND       */
/*                                           GSY_FWD_FCB_STS_FU_WAITING      */
/*                                           GSY_FWD_FCB_STS_FU_SEND         */
/* queue 2 (SendQueue)                                                       */
/*    FCBSendQueue          within Port      GSY_FWD_FCB_STS_SYNC_SEND       */
/*    (Sending=LSA_FALSE)                    GSY_FWD_FCB_STS_FU_SEND         */
/*                                                                           */
/* If sending = TRUE, the FCB is referenced within the port: pFCBInProgress  */
/* and not queued within FCBSendQueue!                                       */


/* --------------------------------------------------------------------------*/
/* PTCP Master per protocol Management                                       */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_SYNC_FRAME_TYPE
{
	LSA_UINT8             Frame[GSY_LOWER_MEM_MAX_LEN];
	LSA_UINT16            Len;             /* actual Length */
} GSY_FWD_SYNC_FRAME_TYPE;


typedef struct _GSY_FWD_FU_FRAME_TYPE
{
	LSA_UINT8             Frame[GSY_LOWER_MEM_MAX_LEN];
	LSA_UINT16            Len;             /* actual Length */
	LSA_INT32             DelayNS;         /* NS Part (signed)                   */
	LSA_UINT32            DelayNSOffset;   /* Offset within frame of NS field    */
} GSY_FWD_FU_FRAME_TYPE;

typedef struct _GSY_FWD_ANNOUNCE_FRAME_TYPE
{
	LSA_UINT8             Frame[GSY_LOWER_MEM_MAX_LEN];
	LSA_UINT16            Len;             /* actual Length */
} GSY_FWD_ANNOUNCE_FRAME_TYPE;

typedef struct _GSY_FWD_MASTER_TYPE
{
	LSA_UINT32               Status;
	LSA_UINT32               ThisIdx;                /* This master idx      */
	LSA_UINT32               ThisProt;               /* This protocol idx    */
	GSY_FWD_FCB_BLINK_TYPE   FCBQueue;               /* Queue with FCBs      */
	LSA_UINT32               FCBCnt;                 /* Number of FCB in que.*/
	LSA_UINT32               LineDelayNS;            /* Line Delay from recv */
	GSY_CH_SYS_PTR           pChSys;                 /* if fwd in progress   */
	GSY_MAC_ADR_TYPE         SrcMACAddr;             /* if fwd in progress   */           
	LSA_UINT16               PortID;                 /* if fwd in progress   */           
	LSA_UINT16               SequenceID;             /* if fwd in progress   */    
	LSA_UINT32               MasterSeqID;            /* if fwd in progress   */
	LSA_UINT32               SyncRecvTimeStamp;      /* if fwd in progress   */    
	GSY_FWD_SYNC_FRAME_TYPE        Sync;
	GSY_FWD_FU_FRAME_TYPE          FollowUp;
	GSY_FWD_ANNOUNCE_FRAME_TYPE    Announce;
	LSA_UINT32               TimeoutCnt;             /* for FU-Timeout       */
} GSY_FWD_MASTER_TYPE;


typedef struct _GSY_FWD_MASTER_PER_PROT_TYPE
{
	GSY_FWD_MASTER_TYPE      Master[GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL];
} GSY_FWD_MASTER_PER_PROT_TYPE;

typedef GSY_FWD_MASTER_PER_PROT_TYPE GSY_LOCAL_MEM_ATTR *   GSY_FWD_MASTER_PER_PROT_PTR_TYPE;

/* Status */
#define GSY_FWD_MASTER_STS_IDLE                   0
#define GSY_FWD_MASTER_STS_FWD_SYNC_RECEIVED      1
#define GSY_FWD_MASTER_STS_FWD_FU_GENERATED       2
#define GSY_FWD_MASTER_STS_FWD_FU_RECEIVED        3
#define GSY_FWD_MASTER_STS_FWD_ANNOUNCE_RECEIVED  4
#define GSY_FWD_MASTER_STS_FWD_FINISHED           5

/* --------------------------------------------------------------------------*/
/* Portmanagement                                                            */
/* Every Systemchannel contains a variable number of Ports.                  */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_PORT_TYPE
{
	LSA_UINT32               Status;
	GSY_CH_SYS_PTR           pChSys;         /* SystemChannel PORT belongs   */
	LSA_UINT16               ThisPortID;      
	LSA_BOOL                 SendEnabled;    /* LSA_FALSE:                   */
	                                         /* If Port is disabled/down/or  */
	                                         /* half-duplex.                 */
	LSA_UINT32               LineDelayNS;    /* if set, LineDelay in ns      */
	LSA_UINT32               FwdPortMode[GSY_FWD_MAC_ADDR_CNT];
	GSY_FWD_FCB_BLINK_TYPE   FCBSendQueue;   /* Queue with FCBs              */
	GSY_FWD_FCB_PTR_TYPE     pFCBInProgress; /* Currently in progress FCB    */ 
	GSY_LOWER_QUEUE          TxRQBQueue;     /* only one TxRQB!              */
	GSY_LOWER_QUEUE          AnnoRQBQueue;   /* only one TxRQB!              */
	LSA_BOOL                 CallCbfAfterSend; /* Call Channel Cbf after send*/
	LSA_BOOL                 ClockTopoOk;    /* 240708lrg001: TopoOk from ExtLinkInd */
} GSY_FWD_PORT_TYPE;

/* Status */
#define GSY_FWD_PORT_STS_LINE_DELAY_NOT_SET    0
#define GSY_FWD_PORT_STS_LINE_DELAY_SET        1

/* FwdPortMode : Forwarding of frames with MAC to output ports */
#define GSY_FWD_MODE_DISABLE     0  /* currently no forwarding, can be activated  */
#define GSY_FWD_MODE_OUTPUT      1  /* forwarding send enabled                    */
#define GSY_FWD_MODE_INPUT       2  /* forwarding receive enabled                 */
#define GSY_FWD_MODE_ENABLE      3  /* send and receive enabled                   */


/* --------------------------------------------------------------------------*/
/* Global Management                                                         */
/*                                                                           */
/* ProtMasterMgm: Array with pointers for each protocol (=DST-MAC) supported */
/*                                                                           */
/*    +-------------+                                                        */
/*  0 |    ptr      | --------------------------------------> +-----------+  */
/*    +-------------+                                         |           |  */
/*    |    ptr      | ------------------>  +-----------+      +-----------+  */
/*    +-------------+                      |           |      |           |  */
/*    |    NULL     |                      +-----------+      +-----------+  */
/*    +-------------+                      |           |      :           :  */
/*    |    ptr      | --> +----------+     +-----------+      +-----------+  */
/*    +-------------+     |          |     :           :                     */
/*    |    NULL     |     +----------+     +-----------+                     */
/*    +-------------+     |          |                                       */
/*    :             :     +----------+                                       */
/*    +-------------+     :          :     \-----v-----/                     */
/* 31 |    NULL     |     +----------+                                       */
/*    +-------------+                        max masters per dst-MAC         */
/*                                                                           */
/*                                                                           */
/*      NULL = not used                                                      */
/*                                                                           */
/*                                                                           */
/* For each protocol the same number over different Masters are supported at */
/* a time.                                                                   */
/*                                                                           */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_PROTCOL_TYPE
{
	GSY_FWD_MASTER_PER_PROT_PTR_TYPE         pMasterTable;  /* Master Management     */
//	LSA_UINT32                               FwdUserMode;	310809lrg001: now per channel
} GSY_FWD_PROTCOL_TYPE;

typedef struct _GSY_FWD_GLOBAL_TYPE
{
	GSY_FWD_PROTCOL_TYPE      ProtMasterMgm[GSY_FWD_MAC_ADDR_CNT];
	GSY_CH_SYS_PTR            EDDSysChannel[GSY_MAX_SYS_CHANNELS];
	LSA_UINT32                EDDSysChannelCnt;
	LSA_UINT32                ProtCnt;    /* Number of used Protocols       */
	GSY_TIMER                 Timer;      /* Timer for timeout              */
	LSA_UINT32                TimerCnt;   /* TimerCounter for local         */
	                                      /* Timeouthandling                */
	GSY_CH_SYS_PTR            pCloseChSys;/* <> NULL if we have to finish   */
	                                      /* a channel close within timer   */
} GSY_FWD_GLOBAL_TYPE;


/* --------------------------------------------------------------------------*/
/* EDD System channel management                                             */
/*                                                                           */
/* pPortMgm points to an array depening on number of ports for this channel  */
/*                                                                           */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_FWD_MC_ENABLED_TYPE
{
	LSA_BOOL                       SyncEnabled;   /* Sync-MC-MAC enabled?    */
	LSA_BOOL                       SyncFuEnabled; /* SyncFu-MC-MAC enabled?  */
	LSA_BOOL                       FuEnabled;     /* Fu-MC-MAC enabled?      */
												  /* 240907lrg001:           */
	LSA_BOOL                       AnnoEnabled;   /* Announce-MC-MAC enabled?*/
	LSA_UINT32                     FwdUserMode;   /* 310809lrg001            */
} GSY_FWD_MC_ENABLED_TYPE;

/* FwdUserMode : Forwarding of frames with MAC to local user   */
/* GSY_FWD_DISABLE                 */  
/* GSY_FWD_ENABLE for all frames   */  
/* GSY_FWD_INPUT for Announce only 310809lrg001 */  

typedef struct _GSY_FWD_CHANNEL_TYPE
{
	LSA_UINT32                     Status;
	LSA_UINT32                     DelayCorrectionMode;
	GSY_FWD_PORT_PTR_TYPE          pPortMgm;    /* pointer to array          */
	LSA_UINT32                     TimeoutCnt;  /* Configured FU-Timeout in s*/
	LSA_UINT32                     PortCnt;
	LSA_UINT32                     MaxLocalDelay;              
	LSA_UINT32                     FCBCnt;      /* number of allocated FCBs  */
	LSA_UINT32                     FCBInUseCnt; /* Number of FCBs in use     */
	LSA_UINT32                     FCBPortCnt;  /* Number of FCBs used by    */
	                                            /* all ports of channel      */
	                                            /* over all Channel!!!       */
	GSY_FWD_FCB_PTR_TYPE           pFCBAlloc;   /* Allocated FCB Base addr   */
	GSY_FWD_FCB_BLINK_TYPE         FCBFreeQueue;/* Queue of free FCBs        */
	LSA_UINT32                     MCSetIdx;    /* Idx of last enabled or    */
	                                            /* disabled Multicast MAC    */
	                                            /* (0..GSY_FWD_MAC_ADDR_CNT) */
	LSA_UINT8                      MCSetSync;   /* Sync/SyncFu/Fu MC set     */
	GSY_FWD_MC_ENABLED_TYPE        MCEnabled[GSY_FWD_MAC_ADDR_CNT];            
	GSY_FWD_STATS_TYPE             Stats;       /* Statistics                */
	LSA_VOID    	LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf) 
	                           (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response);
} GSY_FWD_CHANNEL_TYPE;


/* MCSetSync */
#define GSY_FWD_SYNC_SET         0
#define GSY_FWD_SYNCFU_SET       1
#define GSY_FWD_FU_SET           2
#define GSY_FWD_ANNO_SET         3		/* 240907lrg001 */

/* Status */
#define GSY_FWD_CHANNEL_CLOSED         0
#define GSY_FWD_CHANNEL_OPENING        1
#define GSY_FWD_CHANNEL_OPEN           2
#define GSY_FWD_CHANNEL_CLOSING        3

/* --------------------------------------------------------------------------*/  
/* DelayCorrectionMode                                                       */
/*                                                                           */  
/* 0: Add Linedelay, local delay and local delay drift to FU-Delay field     */
/*    All correction done by software. No special HW-support.                */  
/* 1: Add Linedelay and local delay drift correction to FU-Delay field       */
/*    (used with ERTEC4000. ERTEC400 inserts local delay in SYNC-frame by    */
/*     hardware)                                                             */  
/*    In addition we have to correct the LineDelay from ERTEC 400 HW-Register*/
/*    which is added to SYNC-Frame but may be wrong. This correction is done */
/*    within FollowUP Frame.                                                 */
/*    NOTE: If the HW-Register content is changed between SYNC and FOLLOWUP  */
/*          The correction is wrong!                                         */
/* 2: ERTEC 200: Add local drift correction to FU-Delay field.               */
/* 3: All done by hardware nothing (local delay, line delay and drift comp.) */
/* --------------------------------------------------------------------------*/  

#define GSY_FWD_DELAY_CORRECTION_NO_HW_SUPPORT         0
#define GSY_FWD_DELAY_CORRECTION_ERTEC400              1
#define GSY_FWD_DELAY_CORRECTION_ERTEC200_DC           2
#define GSY_FWD_DELAY_CORRECTION_FULL_HW_SUPPORT       3

/* --------------------------------------------------------------------------*/
/* Macros                                                                    */
/* --------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  GSY_PUT_BLOCK_TOP_USEQUEUE
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define GSY_PUT_BLOCK_TOP_USEQUEUE(_Bottom, _Top, _Block)   \
{                                                               \
                                                                \
    _Block->UseQueue.pNext = LSA_NULL;                          \
    _Block->UseQueue.pPrev = _Top;                              \
                                                                \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
    {                                                           \
        _Top    = _Block;                                       \
        _Bottom = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Top->UseQueue.pNext       = _Block;                    \
        _Top                       = _Block;                    \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   GSY_REM_BLOCK_BOTTOM_USEQUEUE
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define GSY_REM_BLOCK_BOTTOM_USEQUEUE(_Bottom, _Top, _Block)\
{                                                               \
    _Block = _Bottom;                                           \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = _Bottom->UseQueue.pNext;                      \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            _Bottom->UseQueue.pPrev = LSA_NULL;                 \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   GSY_REM_BLOCK_TOP_USEQUEUE
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define GSY_REM_BLOCK_TOP_USEQUEUE(_Bottom, _Top, _Block)                \
{                                                               \
    _Block = _Top;                                              \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = _Top->UseQueue.pPrev;                            \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            _Top->UseQueue.pNext = LSA_NULL;                    \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   GSY_REM_BLOCK_USEQUEUE
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block, 
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define GSY_REM_BLOCK_USEQUEUE(_Bottom, _Top, _Handle, _Block)           \
{                                                               \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
        _Block = LSA_NULL;                                      \
    else                                                        \
    {                                                           \
        _Block = _Bottom;                                       \
                                                                \
        while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
              (! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
            _Block = _Block->UseQueue.pNext;                    \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                GSY_REM_BLOCK_BOTTOM_USEQUEUE(_Bottom, _Top, _Block);   \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    GSY_REM_BLOCK_TOP_USEQUEUE(_Bottom, _Top, _Block);  \
                }                                               \
                else                                            \
                {                                               \
                    _Block->UseQueue.pPrev->UseQueue.pNext = _Block->UseQueue.pNext;       \
                    _Block->UseQueue.pNext->UseQueue.pPrev = _Block->UseQueue.pPrev;       \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:  GSY_PUT_BLOCK_TOP_SENDQUEUE
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block,
 *
 * return value:
 *
 *===========================================================================*/

#define GSY_PUT_BLOCK_TOP_SENDQUEUE(_Bottom, _Top, _Block)                \
{                                                               \
                                                                \
    _Block->SendQueue.pNext = LSA_NULL;                         \
    _Block->SendQueue.pPrev = _Top;                             \
                                                                \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
    {                                                           \
        _Top    = _Block;                                       \
        _Bottom = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Top->SendQueue.pNext      = _Block;                    \
        _Top                       = _Block;                    \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   GSY_REM_BLOCK_BOTTOM_SENDQUEUE
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block,
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define GSY_REM_BLOCK_BOTTOM_SENDQUEUE(_Bottom, _Top, _Block)             \
{                                                               \
    _Block = _Bottom;                                           \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = _Bottom->SendQueue.pNext;                     \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            _Bottom->SendQueue.pPrev = LSA_NULL;                \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   GSY_REM_BLOCK_TOP_SENDQUEUE
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block,
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define GSY_REM_BLOCK_TOP_SENDQUEUE(_Bottom, _Top, _Block)                \
{                                                               \
    _Block = _Top;                                              \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = _Top->SendQueue.pPrev;                           \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            _Top->SendQueue.pNext = LSA_NULL;                   \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   GSY_REM_BLOCK_SENDQUEUE
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define GSY_REM_BLOCK_SENDQUEUE(_Bottom, _Top, _Handle, _Block)           \
{                                                               \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
        _Block = LSA_NULL;                                      \
    else                                                        \
    {                                                           \
        _Block = _Bottom;                                       \
                                                                \
        while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
              (! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
            _Block = _Block->SendQueue.pNext;                   \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                GSY_REM_BLOCK_BOTTOM_SENDQUEUE(_Bottom, _Top, _Block);   \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    GSY_REM_BLOCK_TOP_SENDQUEUE(_Bottom, _Top, _Block);  \
                }                                               \
                else                                            \
                {                                               \
                    _Block->SendQueue.pPrev->SendQueue.pNext = _Block->SendQueue.pNext;       \
                    _Block->SendQueue.pNext->SendQueue.pPrev = _Block->SendQueue.pPrev;       \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \

/* --------------------------------------------------------------------------*/
/* Functions and assotiated types                                            */
/* --------------------------------------------------------------------------*/

/* AP01539291 Linkerfehler bei Verwendung von C++ compiler (Fkt. ptr als Parameter)
*/
typedef  LSA_VOID  LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR, GSY_CBF_TYPE)(GSY_CH_SYS_PTR, LSA_UINT16);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdInit(
	LSA_UINT32       ProtocolSupport);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdExit(LSA_VOID);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdChannelInit(
	GSY_CH_SYS_PTR          pChSys,
	LSA_UINT16              PortCnt,
	GSY_FWD_INIT_PTR_TYPE   pPortInit,
	GSY_CBF_TYPE			Cbf);
//	LSA_VOID    	LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf) 
//	                           (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response));

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdChannelExit(
	GSY_CH_SYS_PTR  pChSys,
	GSY_CBF_TYPE			Cbf);
//	LSA_VOID    	LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf) 
//	                           (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response));

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdLineDelaySet(
	GSY_CH_SYS_PTR          pChSys,
	LSA_UINT16              PortID,
	LSA_UINT32              LineDelay);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdPortForwardCtrl(
	GSY_CH_SYS_PTR          pChSys,
	LSA_UINT16              PortID,
	LSA_UINT32              FwdModeIn,
	LSA_UINT32              FwdModeOut,
	GSY_CBF_TYPE			Cbf);
//	LSA_VOID    	LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf) 
//	                           (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response));


LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdUserForwardCtrl(
	GSY_CH_SYS_PTR          pChSys,
	LSA_UINT32              Mode,
	LSA_UINT32              MACAddrID);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdRcvIndication(
	GSY_CH_SYS_PTR          pChSys,
	GSY_LOWER_RQB_PTR_TYPE  pRbl,
	LSA_BOOL				ReprovideSync);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdSendConfirmation(
	GSY_CH_SYS_PTR          pChSys,
	GSY_LOWER_RQB_PTR_TYPE  pRbl);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdMCConfirmation(
	GSY_CH_SYS_PTR          pChSys,
	LSA_UINT16              Response);

LSA_EXTERN LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdTimerEvent(
	GSY_TIMER_PTR	pTimer);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdFrameUserForward(
	GSY_CH_SYS_PTR          pChSys,
	GSY_LOWER_RQB_PTR_TYPE  pRbl,
	GSY_TLV_INFO_PTR_TYPE   pTLVInfo);

LSA_EXTERN LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdLinkStatus(
 GSY_CH_SYS_PTR          pChSys,
 LSA_UINT16              PortID,
 LSA_UINT8               Status,
 LSA_UINT8               Mode,
 LSA_BOOL                TopoOk);	//240708lrg001

/*****************************************************************************/
/*  end of file GSY_FWD.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_FWD_H */
