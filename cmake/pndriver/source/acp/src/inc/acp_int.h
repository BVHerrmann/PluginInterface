#ifndef ACP_INT_H
#define ACP_INT_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#include "acp_inc.h"
#include "acp_trc.h"


/*------------------------------------------------------------------------------
// stringify
//----------------------------------------------------------------------------*/

#define ACP_STRINGIFY_(n)  # n
#define ACP_STRINGIFY(n)   ACP_STRINGIFY_(n) /* make it a string */

/*------------------------------------------------------------------------------
// lower macros
//----------------------------------------------------------------------------*/

#define ACP_LL_SET_HANDLES(pRQB, value) {\
	EDD_RQB_SET_HANDLE (pRQB, 0);\
	EDD_RQB_SET_HANDLE_LOWER (pRQB, value);\
	}

/*------------------------------------------------------------------------------
// logging
//----------------------------------------------------------------------------*/

#ifdef ACP_LOG_FCT
# error "ACP_LOG_FCT is obsolete (TIA 1649473, V6.1i1.4). please remove the define from acp_cfg.h"
#endif

/*------------------------------------------------------------------------------
// Offset Macro check RQB
//----------------------------------------------------------------------------*/

#define ACP_OFFS(type, field)  (/*lint -e(413) likely use of null pointer */ (LSA_INT)( &((type*)0)->field ))

/*------------------------------------------------------------------------------
// double-linked-list, copy from CM
//----------------------------------------------------------------------------*/

#define ACP_LIST_TYPED_ENTRY_TYPE(ptr_type_)	\
	struct ptr_type_ ## _entry_tag { \
		ptr_type_  Flink; \
		ptr_type_  Blink; \
	}

typedef struct ACP_LIST_ENTRY_PTR_TYPE_entry_tag * ACP_LIST_ENTRY_PTR_TYPE;

typedef ACP_LIST_TYPED_ENTRY_TYPE(ACP_LIST_ENTRY_PTR_TYPE)  ACP_LIST_ENTRY_TYPE;


#define AcpListInitialize(ListHead_) {\
	(ListHead_)->Flink = (ListHead_)->Blink = (ListHead_);\
	}

#define AcpListIsEmpty(ListHead_) (\
	LSA_HOST_PTR_ARE_EQUAL((ListHead_)->Flink, (ListHead_))\
	)

#define AcpListIterFirst(ListHead_, Type_) (\
	(Type_)(AcpListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Flink)\
	)

#define AcpListIterNext(ListHead_, Entry_, Type_) (\
	(Type_)(LSA_HOST_PTR_ARE_EQUAL((Entry_)->Flink, (ListHead_)) ? LSA_NULL : (Entry_)->Flink)\
	)

#define AcpListPeekTail(ListHead_, Type_) (\
	(Type_)(AcpListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Blink)\
	)

#define AcpListInsertTail(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_);\
	(Entry_)->Blink = (ListHead_)->Blink;\
	(ListHead_)->Blink->Flink = (Entry_);\
	(ListHead_)->Blink = (Entry_);\
	}

#define AcpListRemoveEntry(Entry_) {\
	ACP_LIST_ENTRY_PTR_TYPE EX_Entry_;\
	EX_Entry_ = (Entry_);\
	EX_Entry_->Blink->Flink = EX_Entry_->Flink;\
	EX_Entry_->Flink->Blink = EX_Entry_->Blink;\
	}

#define AcpListRemoveHead(ListHead_, Entry_, Type_) {\
	(Entry_) = (Type_)((ListHead_)->Flink);\
	AcpListRemoveEntry((ListHead_)->Flink);\
	}

#define AcpListForeach(Entry_, ListHead_, Type_) \
	Entry_ = AcpListIterFirst (ListHead_, Type_); \
	acp_is_not_null (Entry_); \
	Entry_ = AcpListIterNext (ListHead_, (ACP_LIST_ENTRY_PTR_TYPE)(Entry_), Type_)

/*------------------------------------------------------------------------------
// double-linked-list generic, copy from CM
//----------------------------------------------------------------------------*/
#define AcpListInsertTailT(ListHead_, Entry_) {\
	AcpListInsertTail(/*TypedList*/(ACP_LIST_ENTRY_PTR_TYPE)(ListHead_), (ACP_LIST_ENTRY_PTR_TYPE)(Entry_)); \
}

/*------------------------------------------------------------------------------
// oneshot-timer -- implemented in file acp_tim.c, copy from CM
//----------------------------------------------------------------------------*/

typedef struct acp_oneshot_tag  * ACP_ONESHOT_PTR_TYPE;
typedef struct acp_oneshot_tag  const* ACP_ONESHOT_CONST_PTR_TYPE;

typedef LSA_VOID LSA_FCT_PTR(/*ATTR*/,
	ACP_ONESHOT_CALLBACK)(
		ACP_ONESHOT_PTR_TYPE oneshot,
		LSA_VOID_PTR_TYPE user_ptr
	);

typedef struct acp_oneshot_tag {
	ACP_LIST_ENTRY_TYPE dlink; /* must be first in struct */
	ACP_ONESHOT_CALLBACK callback;
	LSA_VOID_PTR_TYPE user_ptr;
	LSA_UINT32 ticks;
	LSA_UINT16 running;
} ACP_ONESHOT_TYPE;

LSA_VOID
acp_oneshot_sys_init (
	LSA_UINT16  time_base,
	LSA_UINT16  time_factor
	);

LSA_VOID
acp_oneshot_sys_undo_init (
	LSA_VOID
	);

LSA_VOID
acp_oneshot_init(
	ACP_ONESHOT_PTR_TYPE oneshot,
	ACP_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
);

LSA_VOID
acp_oneshot_trigger(
	ACP_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
);

LSA_UINT16
acp_oneshot_stop(
	ACP_ONESHOT_PTR_TYPE oneshot
);

LSA_UINT16
acp_oneshot_is_running(
	ACP_ONESHOT_CONST_PTR_TYPE oneshot
);

LSA_VOID
acp_timeout_synchronized (
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

typedef struct acp_channel_tag     * ACP_CHANNEL_PTR;

typedef struct acp_channel_tag       const* ACP_CHANNEL_CONST_PTR;

typedef struct acp_local_alarm_tag * ACP_RTA_PTR;

/* typedef ACP_LIST_TYPED_ENTRY_TYPE(ACP_RTA_SM_PTR)  ACP_RTA_SM_ENTRY_TYPE; not yet */



typedef struct acp_lower_tag {
	LSA_UINT16				nic_id;
	LSA_UINT16				count_of_open_channel;
	EDD_HANDLE_LOWER_TYPE	handle_lower;	/* TIA 2709127 */
	LSA_UINT16				trace_idx;		/* needed for indexed trace */

	LSA_SYS_PTR_TYPE		sysptr;			/* need for alloc/free lower_layer */

		/* queue or send, resources for rta */
	struct acp_rta_qos_tag {
	ACP_LIST_ENTRY_TYPE	nrt_send_list;	/* available sending resources */
	ACP_LIST_ENTRY_TYPE	data_list;		/* alarms waiting for data-sending resource */
	ACP_LIST_ENTRY_TYPE	ack_list;		/* alarms waiting for ack-sending resource */
	} q_or_send;

} ACP_LOWER_TYPE;

typedef ACP_LOWER_TYPE * ACP_LOWER_PTR;
typedef ACP_LOWER_TYPE const* ACP_LOWER_CONST_PTR;

/*------------------------------------------------------------------------------
// local data for one alarm
//----------------------------------------------------------------------------*/

/* ALARM-State */
#define ACP_ALARM_ST_FREE				0x00
#define ACP_ALARM_ST_WF_OPEN			0x01
#define ACP_ALARM_ST_OPEN				0x02
#define ACP_ALARM_ST_WF_FREE			0x04
#define ACP_ALARM_ST_ERROR				0x08

/* ALARM-Reference */
#define ACP_ALARM_REF_NOT_USED			0xFFFF


/* handles RTA-protocol within ACP */

/* RTA-SM-State */
#define ACP_RTA_ST_FREE			0
#define ACP_RTA_ST_OPEN			1

#define ACP_RTA_ST_ERROR_Q		10
#define ACP_RTA_ST_ERROR_CNF	11
#define ACP_RTA_ST_DATA_Q		20
#define ACP_RTA_ST_DATA_CNF		21
#define ACP_RTA_ST_ACK_Q		30
#define ACP_RTA_ST_ACK_CNF		31
#define ACP_RTA_ST_NACK_Q		40
#define ACP_RTA_ST_NACK_CNF		41

#if ACP_FRAG_MODE
#define ACP_RTA_ST_WAIT_ACK		22
#define ACP_RTA_ST_WAIT_DATA	32
#endif

/* start sequencen numbers */
#define ACP_RTA_SQN_START_SND	0xFFFF
#define ACP_RTA_SQN_START_ACK	0xFFFE

#define ACP_RTA_INCR_SEQ_NUM(SeqNum, SeqNumO)	{\
		SeqNumO = (SeqNum);\
		SeqNum  = ACP_CAST_U16 (((SeqNum) + 1) & 0x7FFF);\
	}

	/*	ErrorCode	0xCF	RTA error
	//	ErrorDecode	0x81	PNIO
	//	ErrorCode1	0xFD	Used by RTA for protocol error
	//	ErrorCode2	0x..	see ACP_ALARM_CLOSE
	*/
#define ACP_PNIO_STATUS_REASON_TEMPLATE	0xCF81FD00

#if ACP_FRAG_MODE
#define ACP_FRAG_RECV_IN_WORK		0x00
#define ACP_FRAG_RECV_COMPLETED		0x01
#endif

typedef struct acp_rta_sm_tag {

	ACP_LIST_ENTRY_TYPE  link;		/* for sendQ */
	ACP_RTA_PTR		rta;			/* backlink */
	LSA_UINT16		state;			/* State of Connection */

	LSA_UINT8		prio;
	LSA_UINT8		pnio_status_ec2;/* pnio_status to send / only part ErrorCode2 */

	LSA_UINT8		wf_ack;			/* send pending, wf_ack.ind */
	LSA_UINT8		wf_timeout;		/* acp_rta_sm_state_enter() must handle timeout */

	LSA_UINT16		tx_seq_num;		/* TxSeqNum : Actual SendSeqNumber      */
	LSA_UINT16		tx_seq_numO;	/* TxSeqNumO: Old    SendSeqNumber      */

	LSA_UINT16		rx_seq_num;		/* RxSeqNum : Next Rx SendSeqNum awaited*/
	LSA_UINT16		rx_seq_numO;	/* RxSeqNumO: Old  Rx SendSeqNum (ACK)  */

	ACP_UPPER_RQB_PTR_TYPE	upper;	/* keep a upper-acp_alarm.ind */
	ACP_LIST_ENTRY_TYPE	lower_list;	/* keep lower-nrt-recv.ind */

	ACP_ONESHOT_TYPE		oneshot_timer;	/* need by re-send, acp_rta_send() */
	LSA_UINT16				retry;			/* retry counter, see rta->retry_count */

#if ACP_FRAG_MODE
	LSA_UINT32				frag_offset;			/* Byte Offset for refactoring: Deactivated: 0xFFFF */
	LSA_UINT32				data_length;			/* Absoulte data length of alarm_data */
	LSA_UINT32				payload_sent;			/* Length of payload sended */
	LSA_UINT8				last_recv_frag 			/* checks if it is the last frag of receive */;
	LSA_UINT32				upper_buffer_len;		/* len of upper buffer */
#endif

} ACP_RTA_SM_TYPE;

typedef struct acp_rta_sm_tag * ACP_RTA_SM_PTR;
typedef struct acp_rta_sm_tag const* ACP_RTA_SM_CONST_PTR;


typedef struct acp_local_alarm_tag {
	/* ACP-ALARM-Part */
	ACP_CHANNEL_PTR			channel;
	LSA_UINT32				usr_id;
	LSA_UINT16				alarm_ref;
	LSA_UINT8				state;
	LSA_BOOL				is_udp;

#if ACP_FRAG_MODE
	ACP_LIST_ENTRY_TYPE		send_list[3];	/* low, high, and frag prio */
	ACP_LIST_ENTRY_TYPE		frag_provide_list;	/* upper frag-queue */
#else
	ACP_LIST_ENTRY_TYPE		send_list[2];	/* low & high prio */
#endif

	/* ACP-RTA-Part */
/*	LSA_UINT8				state;			used from ACP-ALARM-Part */
/*	LSA_BOOL				is_udp;			used from ACP-ALARM-Part */
/*	LSA_UINT16				alarm_ref;		used from ACP-ALARM-Part */
	LSA_UINT16				dst_alarm_ref;
	ACP_MAC_ADDR_TYPE		dst_mac;
	ACP_IP_ADDR_TYPE		dst_ip;
	ACP_IP_ADDR_TYPE		src_ip;
	LSA_UINT8				on_closing;
	LSA_UINT8				close_reason;
	LSA_UINT16				vlan_id;
	LSA_UINT16				timeout_ms;
	LSA_UINT16				retry_count;

#if ACP_FRAG_MODE
	ACP_RTA_SM_TYPE			rta_sm[3]; /* low, high, and frag */
#else
	ACP_RTA_SM_TYPE			low; /* low prio */
	ACP_RTA_SM_TYPE			high; /* high prio */
#endif

} ACP_LOCAL_ALARM_TYPE;


typedef ACP_LOCAL_ALARM_TYPE * ACP_LOCAL_ALARM_PTR;
typedef ACP_LOCAL_ALARM_TYPE   const* ACP_LOCAL_ALARM_CONST_PTR;

#define acp_rta_from_alarm(alarm) \
	((ACP_RTA_PTR)(alarm))

#define acp_alarm_from_rta(rta) \
	((ACP_LOCAL_ALARM_PTR)(rta))

#if ACP_FRAG_MODE
#define acp_rta_sm_from_rta(rta, prio) \
	(&(rta)->rta_sm[prio])
#else
#define acp_rta_sm_from_rta(rta, prio) \
	(((prio) == ACP_ALARM_PRIORITY_LOW) ? (& (rta)->low) : (& (rta)->high))
#endif

#define acp_rta_sm_from_alarm(alarm, prio) \
	acp_rta_sm_from_rta(acp_rta_from_alarm(alarm), (prio))


/* see PNIO 61158-6 */

#define ACP_ETHERTYPE_VLAN				0x8100
#define ACP_ETHERTYPE_IP				0x0800
#define ACP_ETHERTYPE_RT				0x8892	/* EDD-Version does swap on EDD_CFG_BIG_ENDIAN */

#define ACP_RTA_MIN_SEND_LEN			32

#define ACP_RTA_VLAN_ID_MSK				0x0FFF

#define ACP_RTA_VLAN_PRIO_MSK			0xE000
#define ACP_RTA_VLAN_PRIO_HIGH			(6 << 13)
#define ACP_RTA_VLAN_PRIO_LOW			(5 << 13)

#define ACP_RTA_PDU_VERS_MASK			0xF0
#define ACP_RTA_PDU_VERS_1				0x10

#define ACP_RTA_PDU_TYPE_MASK			0x0F
#define ACP_RTA_PDU_TYPE_DATA			0x01
#define ACP_RTA_PDU_TYPE_NACK			0x02
#define ACP_RTA_PDU_TYPE_ACK			0x03
#define ACP_RTA_PDU_TYPE_ERROR			0x04

#if ACP_FRAG_MODE
#define ACP_RTA_PDU_TYPE_FRAG			0x05
#endif

#define ACP_RTA_ADD_FLAGS_WINDOWS_SIZE	0x01
#define ACP_RTA_ADD_FLAGS_TACK			0x10

#define ACP_RTA_ADD_FLAGS_DATA			(ACP_RTA_ADD_FLAGS_WINDOWS_SIZE | ACP_RTA_ADD_FLAGS_TACK)
#define ACP_RTA_ADD_FLAGS_ELSE			(ACP_RTA_ADD_FLAGS_WINDOWS_SIZE)

#if ACP_FRAG_MODE
#define ACP_RTA_ADD_FLAGS_MORE_FRAG		0x20
#endif


/*
EtherHeader		::=
	DA, SA, [VLAN], LT

IPHeader		::=
	IP_VersionIHL(=0x45), IP_TypeOfService(=0), IP_TotalLength, IP_Identification,
	IP_Flags_FragOffset, IP_TTL, IP_Protocol, IP_HeaderChecksum,
	IP_SrcIPAddress, IP_DstIPAddress, [IP_Options]

UDPHeader		::=
	UDP_SrcPort(=0x8892), UDP_DstPort(=0x8892), UDP_DataLength, UDP_Checksum

*/
#define ACP_PDU_ETHERNET_HEADER_LENGTH	(6+6+2+2+2)
										/* dst.mac + src.mac + vlan.type + vlan.id + LT */

#define ACP_PDU_IP_UDP_HEADER_LENGTH	(1+1+2+2+2+1+1+2+4+4 + 2+2+2+2)
										/* VersionIHL .. DstIPAddress, SrcPort .. UdpChecksum */

/*
Data-RTA-PDU	::=
	[VLAN], LT(=0x8892),				// not contained in this define
	FrameID, Reference, PDUType (=1),
	AddFlags, SendSeqNum, AckSeqNum, VarPartLen (1..1432),
	RTA-SDU, RTAPadding (0..31)
*/
#define ACP_PDU_RTA_HEADER_LENGTH		(2+2+2+1+1+2+2+2)

#if ACP_FRAG_MODE
#define ACP_PDU_MAX_VAR_PART_LEN		1432
#endif

#if ACP_FRAG_MODE
/*
FRAG-HEADER ::=
	data_length (complete data to transfer), fragment offset
*/
#define ACP_PDU_FRAG_HEADER_LENGTH		(4+4)
#endif

typedef struct acp_rta_header_tag {
	ACP_MAC_ADDR_TYPE	src_mac;
	LSA_UINT16			prio; /* <=> frame_id */
	LSA_UINT16			dst_alarm_ref;
	LSA_UINT16			src_alarm_ref;
	LSA_UINT8			version;
	LSA_UINT8			type;
	LSA_UINT8			add_flags;
	LSA_UINT16			send_seq_nr;
	LSA_UINT16			ack_seq_nr;
	LSA_UINT16			var_part_len;
	LSA_UINT32			pnio_status;
} ACP_RTA_HEADER_TYPE;

typedef ACP_RTA_HEADER_TYPE  * ACP_RTA_HEADER_PTR;
typedef ACP_RTA_HEADER_TYPE  const* ACP_RTA_HEADER_CONST_PTR;


typedef struct acp_ip_udp_header_tag {
	/* IP */
	LSA_UINT16			ip_length;
	LSA_UINT8			ip_protocol;
	ACP_IP_ADDR_TYPE	ip_addr_src;
	ACP_IP_ADDR_TYPE	ip_addr_dst;
	/* UDP */
	LSA_UINT16			udp_port_src;
	LSA_UINT16			udp_port_dst;
	LSA_UINT16			udp_length;
} ACP_IP_UDP_HEADER_TYPE;

typedef ACP_IP_UDP_HEADER_TYPE * ACP_IP_UDP_HEADER_PTR;


/*------------------------------------------------------------------------------
//	read data from PDU / write data to PDU
//----------------------------------------------------------------------------*/

#define acp_dp_set_u8(dp_, u8_)		\
		{ACP_PUT8_HTON ((dp_), 0, (u8_)); (dp_) += 1;}

#define acp_dp_get_u8(u8_, dp_)		\
		{u8_ = ACP_GET8_NTOH ((dp_), 0); (dp_) += 1;}

#define acp_dp_set_u16(dp_, u16_)		\
		{ACP_PUT16_HTON ((dp_), 0, (u16_)); (dp_) += 2;}

#define acp_dp_get_u16(u16_, dp_)		\
		{u16_ = ACP_GET16_NTOH ((dp_), 0); (dp_) += 2;}

#define acp_dp_set_u32(dp_, u32_)		\
		{ACP_PUT32_HTON (dp_, 0, u32_); (dp_) += 4;}

#define acp_dp_get_u32(u32_, dp_)		\
		{u32_ = ACP_GET32_NTOH ((dp_), 0); (dp_) += 4;}

#define acp_dp_set_mac(dp_, mac_) \
		{ACP_MEMCPY (dp_, mac_, ACP_MAC_ADDR_SIZE); (dp_) += ACP_MAC_ADDR_SIZE;}

#define acp_dp_get_mac(mac_, dp_) \
		{ACP_MEMCPY (mac_, dp_, ACP_MAC_ADDR_SIZE); (dp_) += ACP_MAC_ADDR_SIZE;}

#define acp_dp_set_ip(dp_, ip_) \
		{ACP_MEMCPY (dp_, ip_, ACP_IP_ADDR_SIZE); (dp_) += ACP_IP_ADDR_SIZE;}

#define acp_dp_get_ip(ip_, dp_) \
		{ACP_MEMCPY (ip_, dp_, ACP_IP_ADDR_SIZE); (dp_) += ACP_IP_ADDR_SIZE;}


/*------------------------------------------------------------------------------
// channel table
//----------------------------------------------------------------------------*/

#define ACP_CHANNEL_STATE_FREE		0
#define ACP_CHANNEL_STATE_RESERVED	1
#define ACP_CHANNEL_STATE_READY		2

typedef struct acp_channel_tag {
	LSA_HANDLE_TYPE			my_handle;		/* my one handle <=> index in table + 1 */
	LSA_UINT16				state;			/* internal state */

	LSA_SYS_PTR_TYPE		sysptr;			/* current not used, lsa specifically, must match with lower->sysptr */
	ACP_DETAIL_PTR_TYPE		detailptr;		/* pointer to channel-details */

	LSA_UINT16				trace_idx;		/* needed for indexed trace */

	ACP_MAC_ADDR_TYPE		mac_addr;		/* local mac-address */

	ACP_UPPER_CALLBACK_FCT_PTR_TYPE
							callback;		/* the callback-function to the user */
	LSA_HANDLE_TYPE			handle_upper;	/* the handle of the user */
	ACP_LOWER_PTR			lower;			/* pointer to lower-structur */

	ACP_UPPER_RQB_PTR_TYPE	upper;			/* current working upper request block */
	ACP_LIST_ENTRY_TYPE		upper_list;		/* a queue with upper request blocks, waiting for serve */
	LSA_BOOL				upper_is_busy;	/* request-loop is running */

	LSA_UINT16				max_alarms;		/* alarms[0 .. detailptr->alarm_nr_of_ref-1] */
	ACP_LOCAL_ALARM_PTR		alarms;			/* an array of alarm used by this channel */
	ACP_LIST_ENTRY_TYPE	alarm_provide[2];
											/* upper alarm-queue, prio low & high */
	ACP_LIST_ENTRY_TYPE	alarm_lower[2];	/* lower alarm-queue, prio low & high */

} ACP_CHANNEL;


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

extern ACP_CHANNEL			acp_channels[ACP_CFG_MAX_CHANNELS];
extern ACP_LOWER_TYPE		acp_lowers[ACP_CFG_MAX_CHANNELS];
extern LSA_USER_ID_TYPE		acp_null_usr_id;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define acp_is_null(ptr)			(LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define acp_is_not_null(ptr)		(! LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define acp_are_equal(ptr1,ptr2)	(LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

#define acp_is_null_ip(acp_ip_addr_) (\
		(acp_ip_addr_)[0] == 0 && (acp_ip_addr_)[1] == 0 \
	&&	(acp_ip_addr_)[2] == 0 && (acp_ip_addr_)[3] == 0 \
	)

/*lint -function(exit, acp_fatal_error) */
#define ACP_FATAL(error_code_0)		acp_fatal_error (ACP_MODULE_ID, (LSA_UINT16)__LINE__,(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL) /*lint -unreachable */

/*------------------------------------------------------------------------------
// helper for casting and warning-level
//----------------------------------------------------------------------------*/

#if ACP_CFG_CAST_WARN

#	define ACP_CAST_U8(val_)				(val_)
#	define ACP_CAST_U16(val_)				(val_)

#	define ACP_CAST_UPPER_RQB_PTR_PTR(val_)	(val_)
#	define ACP_CAST_UPPER_MEM_PTR_PTR(val_)	(val_)

#	define ACP_CAST_LOCAL_MEM_PTR_PTR(val_)	(val_)

#	define ACP_CAST_LOWER_RQB_PTR_PTR(val_)	(val_)
#	define ACP_CAST_LOWER_MEM_PTR_PTR(val_)	(val_)

#else

#	define ACP_CAST_U8(val_)				((LSA_UINT8)(val_))
#	define ACP_CAST_U16(val_)				((LSA_UINT16)(val_))

#	define ACP_CAST_UPPER_RQB_PTR_PTR(val_)	((ACP_UPPER_RQB_PTR_TYPE *)(val_))
#	define ACP_CAST_UPPER_MEM_PTR_PTR(val_)	((ACP_UPPER_MEM_PTR_TYPE *)(val_))

#	define ACP_CAST_LOCAL_MEM_PTR_PTR(val_)	((ACP_LOCAL_MEM_PTR_TYPE *)(val_))

#	define ACP_CAST_LOWER_RQB_PTR_PTR(val_)	((ACP_LOWER_RQB_PTR_TYPE *)(val_))
#	define ACP_CAST_LOWER_MEM_PTR_PTR(val_)	((ACP_LOWER_MEM_PTR_TYPE *)(val_))

#endif


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*-acp_al.c-------------------------------------------------------------------*/

LSA_UINT16
acp_alarm_build_data (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_ALARM_CONST_PTR  alarm,
	LSA_UINT8  prio,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  dp_size
	);
LSA_UINT16
acp_alarm_indication_set_data (
	ACP_LOCAL_ALARM_CONST_PTR  alarm,
	ACP_UPPER_RQB_PTR_TYPE  upper,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  dp_len,
	LSA_UINT16  prio
	);

ACP_LOCAL_ALARM_PTR
acp_alarm_find (
	ACP_CHANNEL_CONST_PTR  channel,
	LSA_UINT16  alarm_ref,
	LSA_BOOL  do_elog
	);
LSA_UINT16
acp_alarm_count_open (
	ACP_CHANNEL_CONST_PTR  channel
	);

LSA_VOID
acp_op_alarm_open (
	ACP_CHANNEL_PTR  channel,
	LSA_BOOL  is_udp_alarm
	);

LSA_VOID
acp_op_alarm_close (
	ACP_CHANNEL_PTR  channel
	);

LSA_VOID
acp_op_alarm_send (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

LSA_VOID
acp_op_alarm_ack_send (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

LSA_VOID
acp_op_alarm_provide (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

#if ACP_FRAG_MODE

LSA_UINT16
acp_frag_build (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_ALARM_CONST_PTR  alarm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp
	);

LSA_VOID
acp_op_frag_send (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

LSA_VOID
acp_op_frag_recv (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

#endif

/*-acp_rta.c-----------------------------------------------------------------*/

/* rta */
LSA_VOID
acp_rta_open (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  alarm_ref,
	LSA_UINT16  dst_alarm_ref,
	ACP_MAC_ADDR_TYPE  dst_mac,
	ACP_IP_ADDR_TYPE  dst_ip,
	ACP_IP_ADDR_TYPE  src_ip,
	LSA_UINT16  vlan_id,
	LSA_UINT16  timeout10ms,
	LSA_UINT16  retry_count
	);
LSA_VOID
acp_rta_open_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  rsp
	);

LSA_VOID
acp_rta_close (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta
	);
LSA_VOID
acp_rta_close_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  rsp
	);

ACP_RTA_PTR
acp_rta_find_from_hdr (
	ACP_LOWER_CONST_PTR  channel_lower,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);

LSA_VOID
acp_rta_send (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio
	);
LSA_VOID
acp_rta_send_done (
	ACP_CHANNEL_CONST_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio,
	LSA_UINT16  rsp,
	LSA_UINT32  pnio_status
	);

#if ACP_FRAG_MODE
LSA_UINT16
acp_frag_indication_check_data (
	ACP_LOCAL_ALARM_CONST_PTR  alarm,
	ACP_UPPER_RQB_PTR_TYPE  upper,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	ACP_RTA_HEADER_CONST_PTR  hdr,
	ACP_RTA_SM_PTR  rtasm
	);
#endif

LSA_VOID
acp_rta_indication (
	ACP_CHANNEL_CONST_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio,
	LSA_UINT16  rsp,
	ACP_UPPER_RQB_PTR_TYPE * upper
	);

LSA_VOID
acp_rta_header_build (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_CONST_PTR  rtasm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  type,
	LSA_UINT16  var_part_len
	);
LSA_BOOL
acp_rta_header_parse (
	ACP_LOWER_CONST_PTR  channel_lower,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  offset,
	LSA_UINT16  left,
	ACP_RTA_HEADER_PTR  hdr
	);

LSA_BOOL
acp_ip_udp_header_parse (
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  left,
	ACP_IP_UDP_HEADER_PTR  hdr
	);

/* rta_sm_q_or_send */
LSA_VOID
acp_rta_sm_q_or_send_provide (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID
acp_rta_sm_q_or_send (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	);
LSA_VOID
acp_rta_sm_q_or_send_wakeup (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);

/* nrt */
ACP_LOWER_RQB_PTR_TYPE
acp_nrt_recv_alloc (
	ACP_LOWER_CONST_PTR  channel_lower
	);
LSA_VOID
acp_nrt_recv_free (
	ACP_LOWER_CONST_PTR  channel_lower,
	ACP_LOWER_RQB_CONST_PTR_TYPE * lower
	);
ACP_LOWER_RQB_PTR_TYPE
acp_nrt_send_alloc (
	ACP_LOWER_CONST_PTR  channel_lower,
	LSA_UINT32  len
	);
LSA_VOID
acp_nrt_send_free (
	ACP_LOWER_CONST_PTR  channel_lower,
	ACP_LOWER_RQB_CONST_PTR_TYPE * lower
	);
LSA_VOID
acp_nrt_send (
	ACP_CHANNEL_CONST_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	LSA_VOID_PTR_TYPE  user_id
	);
LSA_VOID
acp_nrt_send_done (
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	LSA_VOID_PTR_TYPE  user_id
	);
LSA_VOID
acp_nrt_recv_provide (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID
acp_nrt_recv_indication (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);


/* rta_sm */
LSA_VOID
acp_rta_sm_state_enter (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	);

LSA_VOID
acp_rta_sm_recv_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);

#if ACP_FRAG_MODE
/* rta frag _functions */
LSA_VOID
acp_frag_recv_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE  nrt,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);

LSA_VOID
acp_frag_recv_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE  nrt,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);

LSA_VOID
acp_frag_send_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE  nrt
	);
#endif

LSA_VOID
acp_rta_sm_recv_nack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);
LSA_VOID
acp_rta_sm_recv_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);
LSA_VOID
acp_rta_sm_recv_error (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_CONST_PTR  hdr
	);

LSA_VOID
acp_rta_sm_send_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID
acp_rta_sm_send_data_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);
LSA_VOID
acp_rta_sm_send_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID
acp_rta_sm_send_ack_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);
LSA_VOID
acp_rta_sm_send_nack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID
acp_rta_sm_send_nack_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);
LSA_VOID
acp_rta_sm_send_error (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID
acp_rta_sm_send_error_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);

LSA_VOID
acp_rta_sm_timeout (
	ACP_ONESHOT_PTR_TYPE  oneshot,
	LSA_VOID_PTR_TYPE  user_ptr
	);


/*-acp_low.c------------------------------------------------------------------*/

ACP_LOWER_RQB_PTR_TYPE
acp_ll_alloc_rqb (
	ACP_LOWER_CONST_PTR		channel_lower
	);
LSA_VOID
acp_ll_free_rqb (
	ACP_LOWER_CONST_PTR		channel_lower,
	ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
	);
LSA_VOID
acp_ll_request (
	ACP_LOWER_CONST_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE  edd,
	EDD_SERVICE  service,
	LSA_VOID_PTR_TYPE  user_id
	);


/*-acp_opc.c------------------------------------------------------------------*/

LSA_VOID
acp_free_resource (
	ACP_CHANNEL_CONST_PTR channel
	);

LSA_UINT16
acp_rsp_from_edd_rsp (
	LSA_UINT16  edd_rsp
	);

LSA_VOID
acp_op_channel_info (
	ACP_CHANNEL_PTR  channel
	);
LSA_VOID
acp_op_channel_info_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);

/*-acp_sys.c------------------------------------------------------------------*/

/*-acp_tim.c------------------------------------------------------------------*/

	/* see above */


/*-acp_usr.c------------------------------------------------------------------*/

LSA_VOID
acp_callback (
	ACP_CHANNEL_PTR channel,
	LSA_UINT32 result
	);
LSA_VOID
acp_callback_async (
	ACP_CHANNEL_CONST_PTR channel,
	LSA_UINT32 result,
	ACP_UPPER_RQB_PTR_TYPE * rb
	);
LSA_VOID
acp_callback_and_free_channel (
	ACP_CHANNEL_PTR  channel,
	LSA_UINT32  result,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

ACP_CHANNEL_PTR
acp_channel_from_handle (
	LSA_HANDLE_TYPE  channel_handle
	);

LSA_VOID
acp_run_request (
	ACP_CHANNEL_PTR channel
	);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_INT_H */
