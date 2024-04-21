
#ifndef SNMPX_INT_H                        /* ----- reinclude-protection ----- */
#define SNMPX_INT_H

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
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)             :C&  */
/*                                                                           */
/*  F i l e               &F: snmpx_int.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping for      */
/*  SNMPX.                                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    JS    initial version.                                       */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*------------------------------------------------------------------------------
   derive SNMPX_CFG_SNMP_OPEN_BSD from SOCK_CFG_OPEN_BSD
  ----------------------------------------------------------------------------*/
#if SOCK_CFG_OPEN_BSD == 1
#define SNMPX_CFG_SNMP_OPEN_BSD 1
#else
#define SNMPX_CFG_SNMP_OPEN_BSD 0
#endif

#if SNMPX_CFG_SNMP_OPEN_BSD
#include "obsd_platform_mibal.h"
#include "obsd_snmpd_snmpdext.h"
#include "snmpx_oha_interface.h" /* OHA's interface for LLDP and MRP MIB requests  */
#endif

/* no subagent functionality */
#ifdef SNMPX_CFG_SNMP_AGENT
#  error "No agent functionality implemented!"
#endif

#ifdef SNMPX_CFG_LOCAL_MEM_STATIC
#  error "SNMPX_CFG_LOCAL_MEM_STATIC is defined. Please remove the define from snmpx_cfg.h"
#endif

#ifdef SNMPX_CFG_LOCAL_MEM_DYNAMIC
# error "SNMPX_CFG_LOCAL_MEM_DYNAMIC is defined. Please remove the define from snmpx_cfg.h"
#endif

#ifdef SNMPX_INVALID_NIC_ID
#  error "SNMPX_INVALID_NIC_ID is defined. Please remove the define from snmpx_cfg.h"
#endif

/*------------------------------------------------------------------------------
   stringify
  ----------------------------------------------------------------------------*/

#define SNMPX_STRINGIFY_(n)  # n
#define SNMPX_STRINGIFY(n)   SNMPX_STRINGIFY_(n) /* make it a string */

/*------------------------------------------------------------------------------
   pragma message
   intended usage: #pragma SNMPX_MESSAGE(...)
  ----------------------------------------------------------------------------*/

#ifdef _MSC_VER /* Microsoft, pragma-args are expanded */
# ifdef SNMPX_MESSAGE
#  error "who defined SNMPX_MESSAGE?"
# endif /* SNMPX_MESSAGE */
# ifdef SNMPX_MESSAGE_OFF /* when compiling the variants, we want to see the warnings */
#  undef SNMPX_MESSAGE
# else
#  define SNMPX_MESSAGE(str) message( __FILE__ "(" SNMPX_STRINGIFY(__LINE__) ") : " str)
# endif /* SNMPX_MESSAGE */
#else
# undef SNMPX_MESSAGE
#endif

/*----------------------------------------------------------------------------*/
/* pointer compare macros                                                     */
/*----------------------------------------------------------------------------*/

#define snmpx_is_null(ptr_)         (LSA_HOST_PTR_ARE_EQUAL((ptr_), LSA_NULL))
#define snmpx_is_not_null(ptr_)     (! LSA_HOST_PTR_ARE_EQUAL((ptr_), LSA_NULL))
#define snmpx_are_equal(ptr1_, ptr2_)   (LSA_HOST_PTR_ARE_EQUAL((ptr1_), (ptr2_)))


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/* ========================================================================= */

/* SNMPX module IDs used in C-Files */

#define SNMPX_MODULE_ID_SNMPX_USR              0x1
#define SNMPX_MODULE_ID_SNMPX_LIB              0x2
#define SNMPX_MODULE_ID_SNMPX_SYS              0x3
#define SNMPX_MODULE_ID_SNMPX_LOW              0x4

#define SNMPX_MODULE_ID_SNMPX_ASN1             0x5
#define SNMPX_MODULE_ID_SNMPX_SNMP             0x6

#define SNMPX_MODULE_ID_SNMPX_MSTR             0x10
#define SNMPX_MODULE_ID_SNMPX_SUB              0x11
#define SNMPX_MODULE_ID_SNMPX_MIBS             0x12
#define SNMPX_MODULE_ID_SNMPX_AGNT             0x13

#define SNMPX_MODULE_ID_SNMPX_MNGR             0x20

#define SNMPX_MODULE_ID_SNMPX_CFG              0x30

#define SNMPX_MODULE_ID_SNMPX_TIME             0x40

/*===========================================================================*/
/*                             IEEE-802.3 defines                            */
/*===========================================================================*/

/*
 * see OSF sources IPNAF.H line 90
 *
 * The max # of data bytes that can go into a UDP packet body such that
 * the resulting IP packet can fit through any of the local network
 * interfaces without inducing IP fragmentation.
 *
 * NOTE WELL:  This value is derived from
 *
 *      (1) The size of the data section of data link packets.  For the
 *          time being, the data link is assumed to be ethernet.
 *
 *      (2) The size of the LLC frame.  RFC 1042, which specifies IP
 *          over 802 networks, calls for the use of the SNAP protocol.
 *          SNAP takes up 8 bytes of the ethernet frame's data section.
 *
 *      (3) The size of the UDP and IP headers, from RFCs 768 and 791.
 *
 *      (4) The length of the IP options part of the header.  Since we
 *          do not currently use any of the IP options, this value is
 *          0.   *** This constant must be modified if we ever make use
 *          of IP options in the future. ***
 *
 *
 *  #define RPC_C_ETHER_MAX_DATA_SIZE 1500
 *  #define RPC_C_IP_LLC_SIZE            8 - LLC frame for SNAP protocol
 *  #define RPC_C_IP_HDR_SIZE           20 - Base IP header
 *  #define RPC_C_IP_OPTS_SIZE           0 - IP options length
 *  #define RPC_C_UDP_HDR_SIZE           8 - UDP header
 *
 *  #define RPC_C_IP_UDP_MAX_LOC_UNFRG_TPDU ( RPC_C_ETHER_MAX_DATA_SIZE - \
 *          (RPC_C_IP_LLC_SIZE + RPC_C_IP_HDR_SIZE + RPC_C_IP_OPTS_SIZE + RPC_C_UDP_HDR_SIZE) \
 *          )
 */

//#define SNMPX_UDP_BUFFER_SIZE    1500


/*===========================================================================*/
/*                   some internal configuration settings                    */
/*===========================================================================*/

#ifndef SNMP_32BIT_OID
#define SNMP_32BIT_OID
#endif

#ifndef SNMP_COUNTER64
#define SNMP_COUNTER64
#endif

#ifndef SNMPX_SNMP_PORT
#define  SNMPX_SNMP_PORT            161  /* reseved UDP port for SNMP  */
#endif
#ifndef SNMPX_SNMP_TRAP_PORT
#define  SNMPX_SNMP_TRAP_PORT       162
#endif

/*---------------------------------------------------------------------------*/
/* This structure is  used for internal request initiated by timeout-handler.*/
/* InUse is set if the RQB is underway (from timeout to SOCK_system(). if    */
/* it arrives SOCK_system() InUse is reset to LSA_FALSE. So only one RQB is  */
/* used for every event.                                                     */
/*---------------------------------------------------------------------------*/

typedef struct snmp_internal_request_tag
{
    LSA_BOOL                   InUse;          /* LSA_TRUE if in rqb-queue  */
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB;
} SNMPX_INTERNAL_REQUEST_TYPE;

typedef volatile SNMPX_INTERNAL_REQUEST_TYPE * SNMPX_INTERNAL_REQUEST_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Number of max. trees or sub-trees (OID's) which can be registered by one  */
/* user-channel (=SubAgent) Default for PNIO = 2 (MIB2.mib and LLDP.mib).    */
/*---------------------------------------------------------------------------*/
#ifndef SNMPX_CFG_MAX_OIDS  /* max. trees or sub-trees for agent register */
#define  SNMPX_CFG_MAX_OIDS         2
#endif

#ifndef SNMPX_CFG_SNMP_MANAGER
#ifndef SNMPX_CFG_MAX_MANAGER_SESSIONS  /* max. sessions for one user (manager) */
#define  SNMPX_CFG_MAX_MANAGER_SESSIONS     1   /* for compilation needed   */
#endif
#endif

#ifndef SNMPX_SNMPERROR
#define SNMPX_SNMPERROR(_x)     SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, _x)
#endif


#define SNMPX_MAX_SUBID           0xFFFFFFFF

#define SNMPX_ZERO_IP           "\0\0\0\0"

/* the SNMP group of the MIB-II is handled by SNMPX itself */

#define SNMPX_SNMP_GROUP_TREE           1,3,6,1,2,1,11
#define SNMPX_SNMP_GROUP_TREE_LEN       7

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef SNMPX_IP_TYPE * SNMPX_LOCAL_MEM_IP_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Overview of Handle and Requests handled by SNMPX:                         */
/*                                                                           */
/* We have four types of handles:                                            */
/*                                                                           */
/* - SOCK-Systemchannel (one for all NICs)                                   */
/* - USER-Channels (multiple, one per sub agent)                             */
/*                                                                           */
/* The Systemchannels have to be opened by Systemadaption bevore a user      */
/* can open a USER-Channel. User-Requests can only be done with USER-Channels*/
/*                                                                           */
/* All Requests are handled by the User-Channel. Each user Channel holds a   */
/* number of RCTRL-blocks (RequestControlBlocks) which hold all information  */
/* about the current state of the Request within SNMPX.                      */
/*                                                                           */
/* This User-RCTRL-Block will move through the SOCK and SNMPX channels       */
/* depending on the Actions to perform.                                      */
/*                                                                           */
/* If a User-Request needs a SOCK-Call the RCTRL-Blcok is queued within the  */
/* SOCK-Handle for performing a SOCK-Request.                                */
/*                                                                           */
/* The RCTRL-Block managed as queue of RCTRL-Blocks. So we can use the       */
/* index into the queue for reference. For DSOCK we use this index          */
/* as user_id within the lower RQB, so we find the proper RCTRL-Block on     */
/* confirmation (the requests are not first in first out! so we need to      */
/* find the proper RCTRL!)                                                   */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/* Handle-management-structures                                              */
/*===========================================================================*/

/* general handle parameters */
typedef struct snmpx_SNMPX_HANDLE_PARAMS_TYPE
{
    LSA_SYS_PATH_TYPE                   SysPath;
    LSA_SYS_PTR_TYPE                    pSys;
    SNMPX_DETAIL_PTR_TYPE               pDetail;
    LSA_HANDLE_TYPE                     HandleUpper;
    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE   Cbf;
} SNMPX_HANDLE_PARAMS_TYPE;


typedef struct snmpx_user_rqb_link_tag
{
    SNMPX_UPPER_RQB_PTR_TYPE          pBottom;
    SNMPX_UPPER_RQB_PTR_TYPE          pTop;
} SNMPX_USER_RQB_LINK_TYPE;

/* --------------------------------------------------------------------------*/
/* for SOCK-Systemchannel (only one SOCK-Channel possible)..                 */
/*                                                                           */
/* for handling all requests to SOCK. We have a queue which holds all        */
/* user-request blocks awaiting service. i.e. need a request to SOCK.        */
/* If a free SOCK-Request nlock is present pending user-requests are         */
/* removed and assotiated with a SOCK-Request block.                         */
/*                                                                           */
/* We have only a limited number of SOCK-Request blocks (configured). This   */
/* number is the number of simultaneous active SOCK-requests.                */
/*                                                                           */
/* --------------------------------------------------------------------------*/

/* LowerLayer (SOCK) Request control block  */
#ifndef SNMPX_SOCK_RQB_PTR_TYPE
#define SNMPX_SOCK_RQB_PTR_TYPE         SOCK_UPPER_RQB_PTR_TYPE
#endif

typedef struct snmpx_sock_rqb_link_tag
{
    SNMPX_SOCK_RQB_PTR_TYPE             pBottom;
    SNMPX_SOCK_RQB_PTR_TYPE             pTop;
} SNMPX_SOCK_RQB_LINK_TYPE;

typedef struct snmpx_sock_rqb_link_tag * SNMPX_SOCK_RQB_LINK_PTR_TYPE;


/* Channel-States */
#define SNMPX_HANDLE_STATE_INIT                          0
#define SNMPX_HANDLE_STATE_FREE                          1
#define SNMPX_HANDLE_STATE_CLOSE_USER_RUNNING            2
#define SNMPX_HANDLE_STATE_CLOSED                        3
#define SNMPX_HANDLE_STATE_OPENED                        4   /* sock channel is opened       */

#define SNMPX_HANDLE_STATE_OPEN_SOCK_RUNNING             40
#define SNMPX_HANDLE_STATE_CLOSE_SOCK_RUNNING            41
#define SNMPX_HANDLE_STATE_OPEN_PORT_RUNNING             42  /* opens port 161 for SNMP    */
#define SNMPX_HANDLE_STATE_CLOSE_PORT_RUNNING            43  /* close port 161 for SNMP      */
#define SNMPX_HANDLE_STATE_CANCEL_ERROR_SOCK_RUNNING     44  /* cancel because of open error */
#define SNMPX_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING      45  /* close because of open error  */

#define SNMPX_HANDLE_STATE_CLOSE_WAIT_TIMER              70  /* wait for timer while closing */

/* (Sock-)Port-States: (Master)Agent- and Manager(Session)-States: */
#define SNMPX_SNMP_STATE_CLOSE           1  /* initial, after OpenChannel */
#define SNMPX_SNMP_STATE_WF_CLOSE        2  /* CLOSE port is active       */
#define SNMPX_SNMP_STATE_OPEN            3  /* OPEN port is done          */
#define SNMPX_SNMP_STATE_WF_OPEN         4  /* OPEN port is active        */
#define SNMPX_SNMP_STATE_WF_CNF          5  /* snmp REQUEST is active     */

typedef struct snmpx_hdb_sock_tag * SNMPX_HDB_SOCK_PTR_TYPE;

typedef struct snmpx_hdb_sock_tag
{
    LSA_UINT16                  SockState;    /* State of channel handle       */
    LSA_HANDLE_TYPE             ThisHandle;   /* this handle                   */
    SNMPX_HANDLE_PARAMS_TYPE    Params;       /* general handle parameters     */
    SNMPX_UPPER_RQB_PTR_TYPE    pOCReq;       /* Open or CloseChannel Request  */
    LSA_UINT16                  OpenResponse; /* Response from Open if error   */

    /* AGENT handling */
    LSA_UINT16                  AgntPortState;/* request state (Port open/close)*/
    LSA_HANDLE_TYPE             SockHandle;   /* If InUse. the SOCK handle     */
    LSA_UINT32                  UserRQBCnt;   /* Number of pending user RQBs   */
    SNMPX_USER_RQB_LINK_TYPE    UserRQBs;     /* Queued User-RQB-Blocks        */
    LSA_UINT32                  SockRQBCnt;   /* Number of pending SOCK RQBs   */
    SNMPX_SOCK_RQB_LINK_TYPE    SockRQBs;     /* Request-Control-blocks for    */
    /* SockRequests (Queued)         */
    LSA_UINT32                  RxPendingCnt; /* Number of pending SOCK RCVs   */
    LSA_INT32                   SockFD;       /* lower-layer send sock         */
    SNMPX_IP_TYPE               IPAddress;    /* 0.0.0.0 for agent open port   */
} SNMPX_HDB_SOCK_TYPE;

#if SNMPX_CFG_SNMP_OPEN_BSD
typedef struct snmpx_oha_tag * SNMPX_HDB_OHA_PTR_TYPE;

typedef struct snmpx_oha_tag
{
	LSA_UINT16 State; /* State of channel handle */
	LSA_HANDLE_TYPE ThisHandle; /* channel handle */
	LSA_HANDLE_TYPE OhaHandle; /* lower */
	SNMPX_HANDLE_PARAMS_TYPE Params; /* general handle parameters */
	SNMPX_UPPER_RQB_PTR_TYPE pOCReq; /* Open or CloseChannel Request */
} SNMPX_OHA_TYPE;
#endif

/* --------------------------------------------------------------------------*/
/* For UserChannel                                                           */
/*                                                                           */
/* For handling all User-Requests to SNMPX. We have a queue which holds all  */
/* user-requests till resources are free for further processing (UserReq).   */
/*                                                                           */
/* Every UserChannel uses one assoziated SOCK-channel                        */
/* selected by NICId in CDB (detail-ptr). Every UserChannel uses SOCK-       */
/* Channels (currently there is only one SOCK-Channel for all User-Channels, */
/* in the future the SOCK-Channel will be NIC dependent)                     */
/* --------------------------------------------------------------------------*/

/* manager functionality */
typedef struct snmpx_hdb_session_tag * SNMPX_HDB_SESSION_PTR_TYPE;

typedef struct snmpx_hdb_session_tag
{
    LSA_UINT32          InterfaceID;

    LSA_UINT16          PortState;  /* request state (Port open/close) */
    LSA_UINT16          RetryCnt;   /* retries of a snmp request */
    LSA_UINT16          SessionID;
    LSA_UINT32          RequestID;  /* snmp request-id           */

    SNMPX_IP_TYPE       IPAddress;
    SNMPX_UPPER_COMMUNITY_PTR_TYPE pReadCommunity;
    SNMPX_UPPER_COMMUNITY_PTR_TYPE pWriteCommunity;

    LSA_UINT16          RxPendingCnt; /* Number of pending SOCK RCVs (max. 1)  */
    LSA_INT32           SockFD;       /* lower-layer send sock                 */

    LSA_UINT32          NextSend;     /* Next Send TimeOut ounter.             */

    SNMPX_UPPER_RQB_PTR_TYPE    pUserRQB;       /* current User-RQB-Block      */
} SNMPX_HDB_SESSION_TYPE;


typedef struct snmpx_hdb_user_tag * SNMPX_HDB_USER_PTR_TYPE;

typedef struct snmpx_hdb_user_tag
{
    LSA_UINT16                  UserState;      /* handle state                  */
    LSA_HANDLE_TYPE             ThisHandle;     /* this handle                   */
    SNMPX_HANDLE_PARAMS_TYPE    Params;         /* general handle parameters     */
    SNMPX_UPPER_RQB_PTR_TYPE    pOCReq;         /* Open or Close Request running */
    LSA_UINT16                  OpenResponse;   /* Response from Open if error   */
    LSA_UINT32                  UserReqPendCnt; /* Number of User-Requests       */
    /* currently pending             */
    /* AGENT handling */
    SNMPX_USER_RQB_LINK_TYPE    UserIndReq;     /* Indication Requests for USER  */
    LSA_UINT32                  UserIndReqPending; /* Number of pend. Ind-Req (USER) */
    /* registered trees or subtrees  */
    SNMPX_REGISTER_TYPE         RegisteredOids[SNMPX_CFG_MAX_OIDS];
    /* subtree passed (Get, GetNext) */
    LSA_BOOL                    SubTreePassed[SNMPX_CFG_MAX_OIDS];
    SNMPX_UPPER_OBJECT_PTR_TYPE pCurrentObject; /* the searched mib object       */
    SNMPX_UPPER_RQB_PTR_TYPE    pReceiveReq;    /* current receive resource      */

    /* MANAGER handling */      /* request state (Port open/close) */
    SNMPX_HDB_SESSION_TYPE      MngrSession[SNMPX_CFG_MAX_MANAGER_SESSIONS];
} SNMPX_HDB_USER_TYPE;


/*===========================================================================*/
/* Global Timer management                                                   */
/* This is used to asign a LSA-Timer ID to an static RQB                     */
/* This management is deviceindependend and used for all devices.            */
/*===========================================================================*/

typedef struct snmpx_timer_info_tag
{
    LSA_BOOL                       InUse;
    LSA_TIMER_ID_TYPE              TimerID;
    SNMPX_INTERNAL_REQUEST_TYPE    IntRQB;
    LSA_VOID                       LSA_FCT_PTR(/*ATTR*/, Cbf)
    (SNMPX_INTERNAL_REQUEST_PTR_TYPE   pIntRQB);
} SNMPX_TIMER_INFO_TYPE;

typedef struct snmpx_timer_info_tag * SNMPX_TIMER_INFO_PTR_TYPE;

#define SNMPX_TIMER_ID_INVALID   0xFFFF

#define SNMPX_MAX_TIMER          1

/* --------------------------------------------------------------------------*/
/* main SNMPX-management (handle independend)                                */
/* --------------------------------------------------------------------------*/

typedef struct snmpx_mmgm_tag
{
    SNMPX_TIMER_INFO_TYPE       Timer[SNMPX_MAX_TIMER]; /* Timermanagement       */
    LSA_UINT16                  CyclicTimerID;  /* Timer_ID of cylcic timer      */
	LSA_UINT32                  CyclicTimerTick;/* TimerCounter                  */
    LSA_UINT32                  CyclicTimerUserCnt;  /* number of users using    */
                                                /* the cyclic timer              */
    LSA_BOOL                    AgentActive;    /* master agent state            */

    LSA_UINT16                  UsedUserCnt;    /* currently used user-channels  */
    /* currently used Lower-channels */
    LSA_UINT16                  UsedLowerCnt;   /* one per NIC => NicCount       */

    LSA_RQB_LOCAL_ERROR_TYPE    LLError;        /* last LL-Error                 */

    LSA_UINT32                  SnmpRequestID;  /* snmp request-id (manager)     */

    SNMPX_OBJECT_TYPE           ActiveObject;   /* oid data to or from subagent  */
    SNMPX_OBJECT_TYPE           CloserObject;   /* oid data to or from subagent  */
    LSA_BOOL                    AgtTreePassed;  /* internal tree passed (Get, GetNext) */
} SNMPX_MMGM_TYPE;

typedef struct snmpx_mmgm_tag * SNMPX_MMGM_PTR_TYPE;


/* --------------------------------------------------------------------------*/
/* global SNMPX-Data                                                         */
/* --------------------------------------------------------------------------*/

typedef struct snmpx_data_tag
{
    /* main-managment of SNMPX */
    SNMPX_MMGM_TYPE         MMgm;       /* Global Mgm.                     */

    SNMPX_HDB_USER_TYPE     HUser[SNMPX_CFG_MAX_CHANNELS]; /* USER-Handle Mgm */
    /* we have only one SOCK channel!  */
    SNMPX_HDB_SOCK_TYPE     HSock;                         /* SOCK-Handle Mgm */

#if SNMPX_CFG_SNMP_OPEN_BSD
	SNMPX_OHA_TYPE          HOha; /* OHA-Handle Mgm */
#endif

} SNMPX_DATA_TYPE;

extern SNMPX_DATA_TYPE SNMPX_Data; /* global SNMPX-Structure defined in snmpx_sys.c */

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

#define SNMPX_GET_GLOB_PTR()    (&(SNMPX_Data.MMgm))
#define SNMPX_GET_HUSER_PTR(_x) (&(SNMPX_Data.HUser[_x]))
#define SNMPX_GET_HSOCK_PTR()   (&(SNMPX_Data.HSock))
#if SNMPX_CFG_SNMP_OPEN_BSD
#define SNMPX_GET_HOHA_PTR()    (&(SNMPX_Data.HOha))
#endif

#define SNMPX_GET_REQ_ID()      (SNMPX_Data.MMgm.SnmpRequestID)
#define SNMPX_INCR_REQ_ID()     (SNMPX_Data.MMgm.SnmpRequestID++)

/*===========================================================================*/
/* Handle-handling                                                           */
/*===========================================================================*/

/* Coding of Handle-Variable:                                        */
/* we have two types of handle. A SOCK and a USER-Handle.            */

#define SNMPX_HANDLE_TYPE_MSK   0xC0    /* Bit 7 + 8 select the Type of handle  */
#define SNMPX_HANDLE_VALUE_MSK  0x3F    /* Bit 1..6 the handle value itself     */

#define SNMPX_HANDLE_TYPE_USER  0x00
#define SNMPX_HANDLE_TYPE_OHA   0x40
#define SNMPX_HANDLE_TYPE_SOCK  0x80

#define SNMPX_HANDLE_GET_TYPE(Handle)  ((Handle) & SNMPX_HANDLE_TYPE_MSK)

#define SNMPX_MAX_HANDLE_IDX           128  /* 7 bits */
#define SNMPX_HANDLE_IDX(Handle)       ((Handle) & SNMPX_HANDLE_VALUE_MSK)

#define SNMPX_HANDLE_IS_USER(Handle)   ((SNMPX_HANDLE_GET_TYPE(Handle) == SNMPX_HANDLE_TYPE_USER) ? LSA_TRUE:LSA_FALSE)
#define SNMPX_HANDLE_IS_SOCK(Handle)   ((SNMPX_HANDLE_GET_TYPE(Handle) == SNMPX_HANDLE_TYPE_SOCK) ? LSA_TRUE:LSA_FALSE)
#if SNMPX_CFG_SNMP_OPEN_BSD
#define SNMPX_HANDLE_IS_OHA(Handle)    ((SNMPX_HANDLE_GET_TYPE(Handle) == SNMPX_HANDLE_TYPE_OHA) ? LSA_TRUE:LSA_FALSE)
#endif
/*===========================================================================*/
/*                           chain macros for rqb                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  SNMPX_RQB_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define SNMPX_RQB_PUT_BLOCK_TOP(_Bottom, _Top, _Block)          \
{                                                               \
    SNMPX_RQB_SET_NEXT_RQB_PTR(_Block, LSA_NULL);               \
    SNMPX_RQB_SET_PREV_RQB_PTR(_Block, _Top);                   \
                                                                \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
    {                                                           \
        _Top    = _Block;                                       \
        _Bottom = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        SNMPX_RQB_SET_NEXT_RQB_PTR(_Top, _Block);               \
        _Top        = _Block;                                   \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:  SNMPX_RQB_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define SNMPX_RQB_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)       \
{                                                               \
    SNMPX_RQB_SET_NEXT_RQB_PTR(_Block, _Bottom);                \
    SNMPX_RQB_SET_PREV_RQB_PTR(_Block, LSA_NULL);               \
                                                                \
    if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
    {                                                           \
        _Bottom = _Block;                                       \
        _Top    = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        SNMPX_RQB_SET_PREV_RQB_PTR(_Bottom, _Block);            \
        _Bottom         = _Block;                               \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   SNMPX_RQB_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block)          \
{                                                               \
    _Block = _Top;                                              \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = SNMPX_RQB_GET_PREV_RQB_PTR(_Top);                \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            SNMPX_RQB_SET_NEXT_RQB_PTR(_Top, LSA_NULL);         \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   SNMPX_RQB_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)       \
{                                                               \
    _Block = _Bottom;                                           \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = SNMPX_RQB_GET_NEXT_RQB_PTR((_Bottom));        \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            SNMPX_RQB_SET_PREV_RQB_PTR((_Bottom), LSA_NULL);    \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   SNMPX_RQB_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_RQB_REM_BLOCK(_Bottom, _Top, _Handle, _Block)     \
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
            _Block = SNMPX_RQB_GET_NEXT_RQB_PTR(_Block);        \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                SNMPX_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block); \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    SNMPX_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block); \
                }                                               \
                else                                            \
                {                                               \
                    SNMPX_RQB_SET_NEXT_RQB_PTR(SNMPX_RQB_GET_PREV_RQB_PTR(_Block),SNMPX_RQB_GET_NEXT_RQB_PTR(_Block)); \
                    SNMPX_RQB_SET_PREV_RQB_PTR(SNMPX_RQB_GET_NEXT_RQB_PTR(_Block),SNMPX_RQB_GET_PREV_RQB_PTR(_Block)); \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   SNMPX_RQB_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_RQB_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)  \
{                                                                   \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
        ((_Handle) == 0) )                                          \
        _Block = LSA_NULL;                                          \
    else                                                            \
    {                                                               \
        _Block = _Bottom;                                           \
                                                                    \
        while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
               (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
            _Block = SNMPX_RQB_GET_NEXT_RQB_PTR(_Block);            \
                                                                    \
        if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
            _Block = LSA_NULL;                                      \
    }                                                               \
}                                                                   \

/*=============================================================================
 * function name:   SNMPX_RQB_SEARCH_QUEUE_HANDLE
 *
 * function:        Search RQB-Queue for RQB with Handle.
 *
 * parameters:      SNMPX_UPPER_RQB_PTR_TYPE         pRQB
 *                  LSA_HANDLE_TYPE                Handle
 * return value:    LSA_VOID
 *
 * pRQB             points to start of RQB-queue (maybe NULL)
 * Handle           Handle to search in RQBs
 *
 * Description:     On return pRQB points to NULL if no entry was found, else
 *                  to the RQB with the spezified Handle. On entry pRQB may
 *                  already be NULL.
 *===========================================================================*/

#define SNMPX_RQB_SEARCH_QUEUE_HANDLE(pRQB,Handle)                      \
    {                                                                   \
        while   ((! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) &&          \
                ( SNMPX_RQB_GET_HANDLE(pRQB) != Handle ))               \
        {                                                               \
            pRQB = SNMPX_RQB_GET_NEXT_RQB_PTR(pRQB);                    \
        }                                                               \
    }                                                                   \


/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  SNMPX_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define SNMPX_PUT_BLOCK_TOP(_Bottom, _Top, _Block)              \
{                                                               \
                                                                \
    _Block->pNext = LSA_NULL;                                   \
    _Block->pPrev = _Top;                                       \
                                                                \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
    {                                                           \
        _Top    = _Block;                                       \
        _Bottom = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Top->pNext = _Block;                                   \
        _Top        = _Block;                                   \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:  SNMPX_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define SNMPX_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)           \
{                                                               \
    _Block->pNext = _Bottom;                                    \
    _Block->pPrev = LSA_NULL;                                   \
                                                                \
    if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
    {                                                           \
        _Bottom = _Block;                                       \
        _Top    = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Bottom->pPrev  = _Block;                               \
        _Bottom         = _Block;                               \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   SNMPX_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_REM_BLOCK_TOP(_Bottom, _Top, _Block)              \
{                                                               \
    _Block = _Top;                                              \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = _Top->pPrev;                                     \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            _Top->pNext = LSA_NULL;                             \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   SNMPX_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)           \
{                                                               \
    _Block = _Bottom;                                           \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = _Bottom->pNext;                               \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            _Bottom->pPrev = LSA_NULL;                          \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   SNMPX_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_REM_BLOCK(_Bottom, _Top, _Handle, _Block)         \
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
            _Block = _Block->pNext;                             \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                SNMPX_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);  \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    SNMPX_REM_BLOCK_TOP(_Bottom, _Top, _Block); \
                }                                               \
                else                                            \
                {                                               \
                    _Block->pPrev->pNext = _Block->pNext;       \
                    _Block->pNext->pPrev = _Block->pPrev;       \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   SNMPX_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define SNMPX_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)      \
{                                                                   \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
        ((_Handle) == 0) )                                          \
        _Block = LSA_NULL;                                          \
    else                                                            \
    {                                                               \
        _Block = _Bottom;                                           \
                                                                    \
        while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
               (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
            _Block = (_Block)->pNext;                               \
                                                                    \
        if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
            _Block = LSA_NULL;                                      \
    }                                                               \
}                                                                   \

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* SNMPX_SYS.c */

LSA_EXTERN SNMPX_DATA_TYPE        SNMPX_Data;

LSA_EXTERN LSA_UINT16 SNMPX_UserGetFreeHDB(
    SNMPX_HDB_USER_PTR_TYPE *pHDBUser);

LSA_EXTERN LSA_VOID SNMPX_UserCloseChannelFinish(
    SNMPX_HDB_USER_PTR_TYPE        pHDBUser);

LSA_EXTERN LSA_VOID SNMPX_UserReleaseHDB(
    SNMPX_HDB_USER_PTR_TYPE pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_UserGetFreeRegisterEntry(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser,
    SNMPX_UPPER_REGISTER_PTR_TYPE *pSnmpxRegister);

LSA_EXTERN LSA_UINT16 SNMPX_CheckOidResponse(
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser,
    SNMPX_UPPER_RQB_PTR_TYPE    pRQB);

LSA_EXTERN LSA_UINT16 SNMPX_UserGetFreeSessionEntry(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser,
    SNMPX_COMMON_MEM_U16_PTR_TYPE pSessionIndex);

LSA_EXTERN LSA_UINT16 SNMPX_UserDeleteSessionEntry(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser,
    LSA_UINT16                 SessionIndex);

LSA_EXTERN LSA_UINT16 SNMPX_UserGetActiveSessions(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser);

/* SNMPX_LIB.c */

#define SNMPX_MAC2A_LEN             18
#define SNMPX_IP2A_LEN              16

LSA_EXTERN LSA_UINT16 snmpx_ip2a (SNMPX_COMMON_MEM_U8_PTR_TYPE ip,
                                  SNMPX_COMMON_MEM_U8_PTR_TYPE s_ip);

LSA_EXTERN LSA_BOOL snmpx_namecmp(
    SNMPX_COMMON_MEM_U8_PTR_TYPE name1,
    SNMPX_COMMON_MEM_U8_PTR_TYPE name2,
    LSA_UINT32 length);

LSA_EXTERN LSA_VOID SNMPX_CallCbf(
    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE Cbf,
    SNMPX_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE              pSys);

LSA_EXTERN LSA_VOID SNMPX_RQBSetResponse(
    SNMPX_UPPER_RQB_PTR_TYPE    pRQB,
    LSA_UINT16                Response);

LSA_EXTERN LSA_VOID SNMPX_UserRequestFinish(
    SNMPX_HDB_USER_PTR_TYPE      pHDB,
    SNMPX_UPPER_RQB_PTR_TYPE     pRQB,
    LSA_UINT16                 Status);

LSA_EXTERN SNMPX_HDB_USER_PTR_TYPE SNMPX_GetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN SNMPX_HDB_SOCK_PTR_TYPE SNMPXGetSockHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN LSA_VOID SNMPX_FreeAllRegisterEntries(
    SNMPX_HDB_USER_PTR_TYPE pHDBUser);

LSA_EXTERN LSA_VOID SNMPX_FreeAllSessionEntries(
    SNMPX_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID SNMPX_InitMibTrees(LSA_VOID);

LSA_EXTERN LSA_BOOL SNMPX_CheckMibTree(
    SNMPX_UPPER_OID_PTR_TYPE    pOid,
    LSA_UINT16                  OidLen);

LSA_EXTERN SNMPX_HDB_USER_PTR_TYPE SNMPX_GetSubAgent(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,    /* internal obj. */
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,
    LSA_INT                     Version);   /* SNMPX version */

LSA_EXTERN SNMPX_HDB_USER_PTR_TYPE SNMPX_GetNextSubAgent(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,    /* internal obj. */
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version);    /* SNMPX version */

LSA_EXTERN SNMPX_HDB_USER_PTR_TYPE SNMPX_GetCloserSubAgent(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version);    /* SNMPX version */

LSA_EXTERN LSA_UINT16 SNMPX_SetObject(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MsgType,    /* message type  */
    LSA_INT                     Version);   /* SNMPX version */

LSA_EXTERN SNMPX_UPPER_OBJECT_PTR_TYPE SNMPX_GetCurrentObject(LSA_VOID);

LSA_EXTERN SNMPX_UPPER_OBJECT_PTR_TYPE SNMPX_GetCloserObject(LSA_VOID);

LSA_EXTERN LSA_VOID SNMPX_DeleteObjects(LSA_VOID);

LSA_UINT16 SNMPX_CopyObject(
    SNMPX_UPPER_OBJECT_PTR_TYPE    pDestObject,
    SNMPX_UPPER_OBJECT_PTR_TYPE    pSrcObject);

LSA_EXTERN SNMPX_HDB_USER_PTR_TYPE SNMPX_OID_Switcher(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version);   /* SNMPX version */

LSA_EXTERN SNMPX_UPPER_REGISTER_PTR_TYPE SNMPX_GetRegisteredOid(
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen);    /* number of sub-ids in name */

LSA_EXTERN LSA_UINT16 SNMPX_GetCountOfRegisteredOids(
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser);

LSA_EXTERN LSA_VOID SNMPX_DoRecvProvide(
    SNMPX_HDB_SOCK_PTR_TYPE         pHDB,
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE   pRQB);

/* SNMPX_USR.c */

LSA_EXTERN LSA_VOID SNMPX_UserChannelStateMachine(
    SNMPX_HDB_USER_PTR_TYPE  pHDB,
    LSA_UINT16             Response);

LSA_EXTERN LSA_VOID SNMPX_RequestRegisterAgent(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID SNMPX_RequestUnregisterAgent(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID SNMPX_RequestOidProvide(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID SNMPX_RequestOidReceive(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_IndicateOidReceive(
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser,
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject);

LSA_EXTERN LSA_UINT16 SNMPX_DoUserCloseChannel(
    SNMPX_HDB_USER_PTR_TYPE        pHDB,
    SNMPX_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

LSA_EXTERN LSA_UINT16 SNMPX_CheckUserCloseChannel(
    SNMPX_HDB_USER_PTR_TYPE        pHDB,
    SNMPX_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

LSA_EXTERN LSA_UINT16 SNMPX_UserCancelReceive(
    SNMPX_HDB_USER_PTR_TYPE       pHDB);

LSA_EXTERN LSA_VOID SNMPX_RequestOpenSession(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID SNMPX_RequestCloseSession(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID SNMPX_RequestManager(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_CheckRequestManager(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB);

LSA_EXTERN LSA_UINT16 SNMPX_DoRequestManager(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB);

/* SNMPX_LOW.c */

LSA_EXTERN LSA_UINT16 SNMPX_SOCKGetFreeHDB(
    SNMPX_HDB_SOCK_PTR_TYPE *pHDBSock);

LSA_EXTERN LSA_VOID SNMPX_SOCKReleaseHDB(
    SNMPX_HDB_SOCK_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID SNMPX_SOCKChannelDone(
    SNMPX_HDB_SOCK_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 SNMPX_SOCKOpenChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID SNMPX_SOCKOpenChannelDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_SOCKCloseChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID SNMPX_SOCKCloseChannelDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_SOCKRecvProvide(
    SNMPX_HDB_SOCK_PTR_TYPE    pHDB,
    LSA_UINT32               Cnt);

LSA_EXTERN LSA_VOID SNMPX_SOCKRecvDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID SNMPX_SOCKSendDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_SOCKOpenPort(
    SNMPX_HDB_SOCK_PTR_TYPE    pHDB,
    LSA_UINT16                 Orginator,
    SNMPX_IP_TYPE              IPAddress,
    LSA_UINT16                 SessionIndex);

LSA_EXTERN LSA_VOID SNMPX_SOCKOpenPortDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_SOCKClosePort(
    SNMPX_HDB_SOCK_PTR_TYPE    pHDB,
    LSA_UINT16                 Orginator,
    LSA_INT32                  SockFD,
    LSA_UINT16                 SessionIndex);

LSA_EXTERN LSA_VOID SNMPX_SOCKClosePortDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN  SNMPX_SOCK_LOWER_RQB_PTR_TYPE snmpx_sock_alloc_rsp_rqb(
    SNMPX_HDB_SOCK_PTR_TYPE     pHDB,
    LSA_UINT16                  Orginator,
    LSA_UINT16                  SessionIndex);

LSA_EXTERN  LSA_VOID snmpx_sock_free_rsp_rqb(
    SNMPX_HDB_SOCK_PTR_TYPE pHDB,
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_INT snmpx_agt_parse_snmp(LSA_UINT8*, LSA_UINT, LSA_UINT8*, LSA_UINT, LSA_UINT16*);

LSA_EXTERN LSA_VOID SNMPX_SOCKRequestHandler(
    SNMPX_HDB_SOCK_PTR_TYPE     pHDBSock,
    SNMPX_UPPER_RQB_PTR_TYPE    pRQBUser,
    LSA_UINT16                  Response);

LSA_EXTERN LSA_VOID SNMPX_OidSwitcher(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_ParseAgent(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 SNMPX_ParseManager(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE	pRQB,
    SNMPX_UPPER_RQB_PTR_TYPE		pRQBUser,
	LSA_INT32				       RequestID);

/* snmpx_time.c */

LSA_EXTERN LSA_UINT16 SNMPX_TimerAlloc(
    LSA_UINT16                      TimerType,
    LSA_UINT16                      TimerBase,
    LSA_UINT32                      UserID,
    LSA_VOID                        LSA_FCT_PTR(/*ATTR*/, Cbf)
    (SNMPX_INTERNAL_REQUEST_PTR_TYPE  pIntRQB),
    SNMPX_LOCAL_MEM_U16_PTR_TYPE     pSNMPXTimerID);

LSA_EXTERN LSA_UINT16 SNMPX_TimerFree(LSA_UINT16 SNMPXTimerID);

LSA_EXTERN LSA_VOID SNMPX_TimerStop(
    LSA_UINT16 SNMPXTimerID);

LSA_EXTERN LSA_VOID SNMPX_TimerStart(
    LSA_UINT16 SNMPXTimerID,
    LSA_UINT16 Time);

LSA_EXTERN LSA_VOID SNMPX_TimerRQBFree(
    LSA_UINT16 SNMPXTimerID);

LSA_EXTERN LSA_VOID SNMPX_TimerCB(
    SNMPX_INTERNAL_REQUEST_PTR_TYPE   pIntRQB);

LSA_EXTERN LSA_VOID SNMPX_RequestTimer(
    SNMPX_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID SNMPX_RequestCyclicTimer(
    SNMPX_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_UINT16 SNMPX_CancelCyclicTimer(
    SNMPX_HDB_USER_PTR_TYPE        pHDBUser,
    SNMPX_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

/* SNMPX_SNMP.C */

LSA_EXTERN LSA_UINT8 SNMPX_GetUserMessageType(LSA_UINT8 AsnMsgType);
LSA_EXTERN LSA_BOOL SNMPX_CheckAsnMessageType(LSA_UINT8 AsnMsgType);
LSA_EXTERN LSA_UINT8 SNMPX_GetAsnMessageType(LSA_UINT8 UserMsgType);

LSA_EXTERN LSA_UINT8 *
snmpx_snmp_get_oid_element(
    SNMPX_UPPER_OID_PTR_TYPE     var_name,/* IN - name of var, OUT - name matched */
    LSA_UINT * var_name_len, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    SNMPX_COMMON_MEM_U8_PTR_TYPE   stat_type,    /* OUT - type of matched variable */
    /* or exception code 0x80XX (ASN_EOM, ASN_NSO, ASN_NSI) if version is SNMP-V2 */
    LSA_UINT *  stat_len,     /* OUT - length of matched variable */
    SNMPX_COMMON_MEM_U16_PTR_TYPE  access_control, /* OUT - access control */
    LSA_UINT8     msg_type, /* IN - message type */
    SNMPX_COMMON_MEM_U32_PTR_TYPE  err_code,/* OUT - error code,if match is not found */
    LSA_INT       version);

/* SNMPX_AGNT.C */

LSA_EXTERN int  snmpx_snmp_compare(SNMPX_OID_TYPE* name1, unsigned len1, SNMPX_OID_TYPE* name2, unsigned len2);
LSA_EXTERN int  snmpx_oid_compare(SNMPX_OID_TYPE* name1, unsigned len1, SNMPX_OID_TYPE* name2, unsigned len2);
LSA_EXTERN LSA_VOID snmpx_logoid(LSA_UINT8 *community, SNMPX_OID_TYPE * var_name, unsigned var_name_len,
                                 LSA_UINT32 err, LSA_UINT8 *var_val, LSA_UINT8 valuetype);

/* SNMPX_MNGR.C */

LSA_EXTERN LSA_INT snmpx_mngr_build_snmp (
	SNMPX_OID_TYPE * var_name, /* IN - object id of variable */
	unsigned   var_name_len,  /* IN - size of var_name, in oids */
	LSA_UINT8  var_val_type,  /* IN - type of variable (int or octet string) (one byte) */
	unsigned   var_val_len,   /* IN - length of variable     */
	LSA_UINT8 * var_val,       /* IN - pointer to ASN1 encoded value of variable */
    LSA_UINT8 * out_data,/* IN/OUT buffer for request packet */
    LSA_UINT out_length, /* IN/SCRTACH size of output buffer */
	long     version,	 /* IN snmp version */
	LSA_UINT8 * snmpx_sidbuf,   /* IN Community string buffer */
	unsigned snmpx_sidlen,  /* IN Community string length */
	LSA_UINT8  msg_type,	 /* IN snmp type (Get, GetNext, Set,...) */
	long     reqid);	/* IN: request id */

LSA_EXTERN SNMPX_UPPER_COMMUNITY_PTR_TYPE SNMPX_GetCommunityString(SNMPX_UPPER_RQB_PTR_TYPE pRQB,
									 SNMPX_HDB_USER_PTR_TYPE pHDB);

#if SNMPX_CFG_SNMP_OPEN_BSD
/* OHA */
LSA_UINT16
SNMPX_OHAGetFreeHDB(
	SNMPX_HDB_OHA_PTR_TYPE *pHDBSock
	);

LSA_VOID
SNMPX_OHAReleaseHDB(
	SNMPX_HDB_OHA_PTR_TYPE pHDB
	);

SNMPX_HDB_OHA_PTR_TYPE
SNMPXGetOHAHDBFromHandle (
	LSA_HANDLE_TYPE Handle
	);

LSA_UINT16
SNMPX_OHAOpenChannel(
	LSA_HANDLE_TYPE OHAHandle,
	LSA_SYS_PATH_TYPE SysPath,
	LSA_SYS_PTR_TYPE pSys
	);

LSA_VOID
SNMPX_OHAOpenChannelDone(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	SNMPX_HDB_OHA_PTR_TYPE pHDB
	);

LSA_UINT16 SNMPX_OHACloseChannel(
	LSA_HANDLE_TYPE OHAHandle,
	LSA_SYS_PTR_TYPE pSys
	);

LSA_VOID SNMPX_OHACloseChannelDone(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	SNMPX_HDB_OHA_PTR_TYPE pHDB
	);


LSA_INT
SNMPX_OHA_Control_Oid(
	OHA_OID_PTR_TYPE oid,
	int oid_length,
	void *buffer,
	int buffer_length,
	LSA_UINT8 VarType,
	LSA_UINT32 VarLength,
	void *context_ptr,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT8 MessageType
	);


LSA_VOID
SNMPX_OHA_Control_Oid_Done(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	const SNMPX_OHA_TYPE * const pHDB
	);

LSA_VOID
SNMPX_OHA_Snmp_Indication(
	LSA_VOID
	);

LSA_VOID
SNMPX_OHA_Snmp_Indication_Done(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	const SNMPX_OHA_TYPE * const pHDB
	);

#endif

/*===========================================================================*/
/*                                   macros                                  */
/*===========================================================================*/
/*lint -function(exit, SNMPX_FatalError) */
#define SNMPX_FATAL(Error) \
		SNMPX_FatalError( Error, SNMPX_MODULE_ID, __LINE__)

#ifndef SNMPX_ALLOC_FAIL_FATAL
#define SNMPX_ALLOC_FAIL_FATAL(pPtr) \
{                                                         \
	if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
	{                                                     \
		SNMPX_FATAL(SNMPX_FATAL_ERR_NULL_PTR);            \
	}                                                     \
}
#endif

#ifdef SNMPX_CFG_FREE_ERROR_IS_FATAL
#define SNMPX_MEM_FREE_FAIL_CHECK(RetVal) \
{                                                         \
	if( RetVal != LSA_RET_OK )                            \
	{                                                     \
		SNMPX_FATAL(SNMPX_FATAL_ERR_MEM_FREE);            \
	}                                                     \
}
#else
#define SNMPX_MEM_FREE_FAIL_CHECK(pPtr) {}
#endif


/*===========================================================================*/
/*                                  prototypes                               */
/*===========================================================================*/


/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif



/*****************************************************************************/
/*  end of file SNMPX_INT.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_INT_H */
