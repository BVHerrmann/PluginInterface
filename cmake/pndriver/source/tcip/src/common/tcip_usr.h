#ifndef TCIP_USR_H
#define TCIP_USR_H

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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_usr.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/

#if TCIP_INT_CFG_TRACE_ON

#define CONTROLLER_MAX_MEASURES 1000
#define CONTROLLER_MAX_SEND_MEASURES 10000

#define CONTROLLER_MIN(min_, act_) if ((act_)<(min_)) {(min_) = (act_);}
#define CONTROLLER_MAX(max_, act_) if ((act_)>(max_)) {(max_) = (act_);}

typedef struct _measure_tag
{
	LSA_UINT64 m[CONTROLLER_MAX_MEASURES];
	int count;
} controller_measure_struct;

typedef struct _send_measure_tag
{
	LSA_UINT64 m[CONTROLLER_MAX_SEND_MEASURES];
	int count;
} controller_send_measure_struct;

/* record number of drops when socket buffer is full */
typedef struct _dropped_tag
{
	int dropped[CONTROLLER_MAX_MEASURES];
	LSA_UINT32 drop_count;
} controller_dropped_struct;

/* record number of successful reads */
typedef struct _recv_success_tag
{
	int success[CONTROLLER_MAX_MEASURES];
} controller_recv_success_struct;

void controller_add_recv_measure_1(void); /* TCIP gets packet from EDD */
void controller_add_recv_measure_2(void); /* TCIP invokes ether_input() */
void controller_add_recv_measure_3(void); /* point in netintr() */
void controller_add_recv_measure_4(void); /* before copy into socket buffer */
void controller_add_recv_timestamp_4(LSA_UINT64 timestamp);
void controller_add_recv_measure_5(int dropped); /* after copy; SOCK is notified data is present in socket buffer */

/*  */
void controller_add_user_send_measure_1(LSA_VOID_PTR_TYPE);
void controller_add_user_send_measure_2(LSA_VOID_PTR_TYPE);

void controller_add_tcip_send_measure_1(LSA_UINT64 timestamp); /* OBSD calls TCIP send function */
void controller_add_tcip_send_measure_2(void); /* TCIP gives send RQB to EDD */
void controller_init_tcip_send_measures();

/* "upper" measures in SOCK */
void controller_add_recv_measure_101(LSA_UINT64 timestamp); /* before recv system call */
void controller_add_recv_measure_102(void); /* after recv system call */

void controller_init_recv_measures();


LSA_UINT64 controller_get_time(LSA_VOID);

extern int event_list_size_max; /* maximum size of event list */
extern int recv_successful; /* set to one if read was successful */

void controller_init_send_measures();


#endif

/*------------------------------------------------------------------------------
//	OPCODES
//	TCIP_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

#define	TCIP_OPC_OPEN_CHANNEL   1
#define TCIP_OPC_CLOSE_CHANNEL  2
#define	TCIP_OPC_TIMER			3


/*------------------------------------------------------------------------------
//	ERROR_CODES
//	TCIP_RQB_TYPE::response
//----------------------------------------------------------------------------*/

#define TCIP_OK                   LSA_RSP_OK /* (usually 0x01 but configurable) */
#define TCIP_OK_PENDING           (LSA_RSP_OK_OFFSET + 0) /* (0x40) internally used only! */

#define TCIP_ERR_RESOURCE         LSA_RSP_ERR_RESOURCE /* (0x84) */
#define TCIP_ERR_PARAM            LSA_RSP_ERR_PARAM    /* (0x86) */
#define TCIP_ERR_SEQUENCE         LSA_RSP_ERR_SEQUENCE /* (0x87) */
#define TCIP_ERR_SYS_PATH         LSA_RSP_ERR_SYS_PATH /* (0x91) */

#define TCIP_ERR_LOWER_LAYER      (LSA_RSP_ERR_OFFSET + 0) /* (0xc0) */


/*-----------------------------------------------------------------------------
//---------------------------------------------------------------------------*/

typedef struct tcip_rqb_tag * TCIP_UPPER_RQB_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: TCIP_OPC_OPEN_CHANNEL
//---------------------------------------------------------------------------*/

#define TCIP_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */

typedef LSA_VOID  LSA_FCT_PTR(/*ATTR*/, TCIP_UPPER_CALLBACK_FCT_PTR_TYPE)(TCIP_UPPER_RQB_PTR_TYPE rb);

typedef struct tcip_open_channel_tag {
	LSA_HANDLE_TYPE		handle;						/* cnf: channel-handle of TCIP */
	LSA_HANDLE_TYPE		handle_upper;				/* req: channel-handle of user, every confirmation of an request set this handle to rqb.handle */
	LSA_SYS_PATH_TYPE	sys_path;					/* req: sys_path of channel */
	TCIP_UPPER_CALLBACK_FCT_PTR_TYPE
						tcip_request_upper_done_ptr;	/* req: pointer to the callback-function */
} TCIP_OPEN_CHANNEL_TYPE;

typedef TCIP_OPEN_CHANNEL_TYPE * TCIP_UPPER_OPEN_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: TCIP_OPC_CLOSE_CHANNEL
//---------------------------------------------------------------------------*/

#if 0
typedef struct tcip_close_channel_tag {
	no parameters
} TCIP_CLOSE_CHANNEL_TYPE;
#endif


/*-----------------------------------------------------------------------------
// OPCODE: TCIP_OPC_TIMER
//---------------------------------------------------------------------------*/

typedef struct tcip_timer_tag {

	volatile long in_use; /* note: native long, see TCIP_EXCHANGE_LONG() */

} TCIP_TIMER_TYPE;

typedef TCIP_TIMER_TYPE * TCIP_UPPER_TIMER_PTR_TYPE;


/*------------------------------------------------------------------------------
// TCIP_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

typedef struct tcip_rqb_tag {
	TCIP_RQB_HEADER  /* first in struct, see tcip_pnio_cfg.h */

	union {
		TCIP_OPEN_CHANNEL_TYPE   open;
		/* TCIP_CLOSE_CHANNEL_TYPE  close; -- no parameters, no type */
		TCIP_TIMER_TYPE          timer;
	} args;

	TCIP_RQB_TRAILER /* last in struct, see tcip_pnio_cfg.h */
} TCIP_RQB_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  tcip_open_channel
 *
 * function:       open a communication channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID
tcip_open_channel(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  tcip_close_channel
 *
 * function:       close a communication channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID
tcip_close_channel(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  tcip_request
 *
 * function:       send a request to a communication channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID
tcip_request(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  tcip_set_ip_suite
 *
 * function:       sets the ip address for a network interface
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#if TCIP_CFG_INTERNICHE || TCIP_CFG_OPEN_BSD
LSA_UINT8
tcip_set_ip_suite( /* return value: TCIP_OK or TCIP_ERR_PARAM */
	LSA_UINT32  interface_id,
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask
);

LSA_UINT8
tcip_set_route( /* return value: TCIP_OK or TCIP_ERR_PARAM */
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask,
	LSA_UINT32	gateway
);

#endif

/*=============================================================================
 * function name:  tcip_get_any_interface_mac_addr
 *
 * function:       returns the mac address for an existing network interface
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT8
tcip_get_any_interface_mac_addr(
    LSA_UINT32* p_interface_id,
	LSA_UINT8*	p_mac_address,
	LSA_UINT16	mac_address_length
);

/*=============================================================================
 * function name:  tcip_get_timeparams
 *
 * function:       returns the TODO
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID
tcip_get_timeparams(
	LSA_UINT32 *p_keepalivetime,
	LSA_UINT32 *p_connectiontimeout,
	LSA_UINT32 *p_retransmittimeout,
	LSA_UINT32 *p_retransmitcount
);

/*=============================================================================
 * function name:  tcip_set_timeparams
 *
 * function:       returns the TODO
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_UINT16
tcip_set_timeparams(
	LSA_UINT32 keepalivetime,
	LSA_UINT32 connectiontimeout,
	LSA_UINT32 retransmittimeout,
	LSA_UINT32 retransmitcount,
	LSA_UINT32 no_change_value
);

/*=============================================================================
 * function name:  tcip_get_mib2_ifindex
 *
 * function:       returns interface index
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT8
tcip_get_mib2_ifindex(
	LSA_UINT32  interface_id,
	LSA_UINT16  port_id,
	LSA_UINT32* IfIndex
);


/*=============================================================================
 * function name:  tcip_set_mib2_ifgroup_data
 *
 * function:       sets interface description string for one interface
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT8
tcip_set_mib2_ifgroup_data(
	LSA_UINT32 interface_id,
	LSA_UINT16 port_id,
	LSA_UINT8* p_ifDescr,
	LSA_UINT16 ifDescr_length
);

/*=============================================================================
 * function name:  tcip_set_mib2_sysgroup_data
 *
 * function:       sets parameters of mib2-system-group
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_UINT8
tcip_set_mib2_sysgroup_data(
	LSA_UINT8 const* p_sysName,
	LSA_UINT16 sysName_length,
	LSA_UINT8 const* p_sysDescr,
	LSA_UINT16 sysDescr_length,
	LSA_UINT8 const* p_sysContact,
	LSA_UINT16 sysContact_length,
	LSA_UINT8 const* p_sysLocation,
	LSA_UINT16 sysLocation_length
);

/*=============================================================================
 * function name:  tcip_mib2_statistics_update
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

#define TCIP_PENDING_NO  0
#define TCIP_PENDING_YES 1

LSA_UINT8
tcip_mib2_statistics_update( /* return value: TCIP_PENDING_NO or TCIP_PENDING_YES */
	LSA_VOID
);

/*====  out functions  =====*/

/*=============================================================================
 * function name:  TCIP_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper rqb for usage in tcip
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ALLOC_UPPER_RQB_LOCAL
LSA_VOID TCIP_ALLOC_UPPER_RQB_LOCAL(
    TCIP_UPPER_RQB_PTR_TYPE * upper_rqb_ptr_ptr,
    LSA_UINT16				  length
);
#endif

/*=============================================================================
 * function name:  TCIP_FREE_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper rqb for usage in tcip
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FREE_UPPER_RQB_LOCAL
LSA_VOID TCIP_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16              * ret_val_ptr,
    TCIP_UPPER_RQB_PTR_TYPE   upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper-RQB
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ALLOC_UPPER_RQB
LSA_VOID
TCIP_ALLOC_UPPER_RQB(
    TCIP_UPPER_RQB_PTR_TYPE * upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE          user_id,
    LSA_UINT16                length,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_FREE_UPPER_RQB
 *
 * function:       free an upper-RQB
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FREE_UPPER_RQB
LSA_VOID
TCIP_FREE_UPPER_RQB(
    LSA_UINT16              * ret_val_ptr,
    TCIP_UPPER_RQB_PTR_TYPE   upper_rqb_ptr,
    LSA_SYS_PTR_TYPE          sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_REQUEST_LOCAL
 *
 * function:       send a request to "self"
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 * maybe this should go into tcip_sys.h, but there is no upper-rqb-type...
 *
 *===========================================================================*/

#ifndef TCIP_REQUEST_LOCAL
LSA_VOID
TCIP_REQUEST_LOCAL(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_TRIGGER_SELECT
 *
 * function:       usage see SOCK
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID
TCIP_TRIGGER_SELECT(
	LSA_VOID
);


/*=============================================================================
 * function name:  TCIP_MIB2_SYSGROUP_DATA_WRITTEN
 *
 * function:       usage see SOCK
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID
TCIP_MIB2_SYSGROUP_DATA_WRITTEN(
	LSA_CHAR * ptr_sysName,
	LSA_CHAR * ptr_sysContact,
	LSA_CHAR * ptr_sysLocation
);


/*=============================================================================
 * function name:  TCIP_MIB2_STATISTICS_UPDATE_DONE
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID
TCIP_MIB2_STATISTICS_UPDATE_DONE(
	LSA_VOID
);


/*=============================================================================
 * function name:  tcip_srv_multicast, TCIP_SRV_MULTICAST_DONE
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#if TCIP_CFG_ENABLE_MULTICAST

#define TCIP_SRV_MULTICAST_ENABLE  1
#define TCIP_SRV_MULTICAST_DISABLE 2

LSA_RESPONSE_TYPE
tcip_srv_multicast(
	LSA_INT32 dev_handle,
	LSA_UINT32 interface_id,
	LSA_UINT32 mc_address, /* (!) network byte order */
	LSA_UINT8  mode
);

LSA_VOID
TCIP_SRV_MULTICAST_DONE(
	LSA_RESPONSE_TYPE result,
	LSA_INT32 dev_handle,
	LSA_UINT8 mode
);

#if TCIP_CFG_OPEN_BSD

#define TCIP_ARP_FILTER_ENABLE 1
#define TCIP_ARP_FILTER_DISABLE 2

#define TCIP_CLUSTER_IP_STATE_ACTIVE  1
#define TCIP_CLUSTER_IP_STATE_INACTIVE 0

#define TCIP_CARP_DEV_HANDLE -1

LSA_RESPONSE_TYPE tcip_srv_cluster_ip_config_set(
	LSA_UINT32 InterfaceID,
	LSA_UINT8  ClusterID,
	LSA_UINT32 ClusterIP,
	LSA_UINT32 ClusterSubnetMask
);

LSA_RESPONSE_TYPE tcip_srv_cluster_ip_state_set(
	LSA_UINT32 InterfaceID,
	LSA_UINT8 ClusterIPState
);

LSA_UINT8 tcip_srv_cluster_ip_state_get(
	LSA_UINT32 InterfaceID
);

//void TCIP_SRV_CLUSTER_IP_CONFIG_SET_DONE(unsigned int interface_id, int result)
//{
//	LSA_UNUSED_ARG(interface_id);
//	LSA_UNUSED_ARG(result);
//}

#endif /* TCIP_CFG_OPEN_BSD */

#endif


#define TCIP_READ_FLAG 0x1
#define TCIP_WRITE_FLAG 0x2
#define TCIP_CONNECT_FLAG 0x4

typedef struct select_entry_tag
{
	void* sock_so;
	LSA_UINT8 flags;
} TCIP_SOCKET_EVENT_TYPE;

LSA_VOID
TCIP_ADD_SOCKET_EVENT(
	void* sock_so, /* SOCK socket structure pointer */
	LSA_UINT8 flags /* see TCIP_ defines above */
);

LSA_VOID
TCIP_DO_SOCKET_EVENTS(
	LSA_VOID
);


#ifdef  SOCK_CFG_WAIT_FOR_NRT_SEND /* (!) for pnTest usage only */
LSA_VOID
TCIP_SRV_NRT_SEND_DONE(
    LSA_VOID
);
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_USR_H */
