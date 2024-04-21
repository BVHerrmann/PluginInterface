#ifndef ACP_USR_H
#define ACP_USR_H

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
/*  F i l e               &F: acp_usr.h                                 :F&  */
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


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*------------------------------------------------------------------------------
// experimental feature RTA-FRAG, do not turn on
//----------------------------------------------------------------------------*/

#ifndef ACP_FRAG_MODE
/* see edd_usr.h: #define EDD_SRT_FRAMEID_ASRT_PNIO_FRAG */
#define ACP_FRAG_MODE 0
#endif


/*****************************************************************************/
/* OPCODES                                                                   */
/* ACP_RQB_TYPE::opcode                                                      */
/**************************************************************************//**
@defgroup acp-opcode-overview Opcode overview
@ingroup acp-opcodes
@{
@brief Overview of all ACP opcodes.
******************************************************************************/
/** @cond INTERNAL */
#define ACP_OPC_INTERNAL_TIMER			0 /**< only internal usage */
/** @endcond */

#define ACP_OPC_OPEN_CHANNEL			1 /**< see @ref ACP_OPC_OPEN_CHANNEL */
#define ACP_OPC_CLOSE_CHANNEL			2 /**< see @ref ACP_OPC_CLOSE_CHANNEL */
#define ACP_OPC_CHANNEL_INFO			3 /**< see @ref ACP_OPC_CHANNEL_INFO */

#define ACP_OPC_ALARM_OPEN				18 /**< see @ref ACP_OPC_ALARM_OPEN */
#define ACP_OPC_UDP_ALARM_OPEN			19 /**< see @ref ACP_OPC_UDP_ALARM_OPEN */
#define ACP_OPC_ALARM_CLOSE				20 /**< see @ref ACP_OPC_ALARM_CLOSE */
#define ACP_OPC_ALARM_SEND				21 /**< see @ref ACP_OPC_ALARM_SEND */
#define ACP_OPC_ALARM_ACK_SEND			22 /**< see @ref ACP_OPC_ALARM_ACK_SEND */
#define ACP_OPC_ALARM_IND_RES_PROVIDE	23 /**< see @ref ACP_OPC_ALARM_IND_RES_PROVIDE */
#define ACP_OPC_ALARM_INDICATION		24 /**< see @ref ACP_OPC_ALARM_INDICATION */
#define ACP_OPC_ALARM_ACK_INDICATION	25 /**< see @ref ACP_OPC_ALARM_ACK_INDICATION */

#if ACP_FRAG_MODE
#define ACP_OPC_FRAG_SEND				30 /**< see @ref ACP_OPC_FRAG_SEND */
#define ACP_OPC_FRAG_RECV				31 /**< see @ref ACP_OPC_FRAG_RECV */
#endif

#define ACP_OPCODE_MAX					33 /**< Maximum of ACP Opcodes */
/** @} ***********************************************************************/


/**************************************************************************//**
@defgroup acp-response-codes Response Codes
@ingroup acp-group
@{
@brief RQB response codes for ACP
@details The following codes are used as response in ACP_RQB_TYPE::response.
******************************************************************************/

#define ACP_OK						LSA_RSP_OK               /**< (0x01)       The request was processed successfully */
#define ACP_OK_CANCEL				(LSA_RSP_OK_OFFSET + 1)  /**< (0x41 /  65) The request was cancelled. The user can release the request block. */

#define ACP_ERR_RESOURCE			LSA_RSP_ERR_RESOURCE     /**< (0x84 / 132) General error in resources (free handles, ...) */
#define ACP_ERR_PARAM				LSA_RSP_ERR_PARAM        /**< (0x86 / 134) An error in the provided arguments. e.g., RQB contains wrong parameters, the required data is missing; ...  */
#define ACP_ERR_SEQUENCE			LSA_RSP_ERR_SEQUENCE     /**< (0x87 / 135) The required processing sequence was not observed. */
#define ACP_ERR_SYS_PATH			LSA_RSP_ERR_SYS_PATH     /**< (0x91 / 145) */

#define ACP_ERR_LOWER_LAYER			(LSA_RSP_ERR_OFFSET + 0) /**< (0xc0 / 192) The error search lies in the lower layer. */
#define ACP_ERR_ALLOC_LOCAL_MEM		(LSA_RSP_ERR_OFFSET + 1) /**< (0xc1 / 193) No (more) local memory is available. */
#define ACP_ERR_ALLOC_LOWER_RQB		(LSA_RSP_ERR_OFFSET + 2) /**< (0xc2 / 194) No request block available for the lower layer. */
#define ACP_ERR_UNSUPPORTED			(LSA_RSP_ERR_OFFSET + 3) /**< (0xc3 / 195) */
#define ACP_ERR_ASRT				(LSA_RSP_ERR_OFFSET + 4) /**< (0xc4 / 196) Error case (timeout / protocol) in the lower layer. RA Submodule. No more alarms can be sent or received. */
#define ACP_ERR_ALLOC_LOWER_MEM		(LSA_RSP_ERR_OFFSET + 5) /**< (0xc5 / 197) No transmission or reception memory available for the lower layer. */
#define ACP_ERR_EDD_PARAM			(LSA_RSP_ERR_OFFSET + 6) /**< (0xc6 / 198) A parameter error was delivered by the lower layer. */
#define ACP_ERR_EDD_RESOURCE		(LSA_RSP_ERR_OFFSET + 7) /**< (0xc7 / 199) The lower layer delivered a resource error. */
/**@}*************************************************************************/

/*-----------------------------------------------------------------------------
// forward declarations / common types
//---------------------------------------------------------------------------*/

typedef struct acp_rqb_tag * ACP_UPPER_RQB_PTR_TYPE;

typedef LSA_VOID * ACP_UPPER_MEM_PTR_TYPE;

typedef LSA_UINT8 * ACP_UPPER_MEM_U8_PTR_TYPE;

#define ACP_MAC_ADDR_SIZE	6

typedef LSA_UINT8  ACP_MAC_ADDR_TYPE[ACP_MAC_ADDR_SIZE];

#define ACP_IP_ADDR_SIZE	4

typedef LSA_UINT8  ACP_IP_ADDR_TYPE[ACP_IP_ADDR_SIZE]; /* IPv4, [0]..MSB  / network-format */

typedef LSA_UINT8 * ACP_LOWER_MEM_U8_PTR_TYPE;

/**************************************************************************//**
@addtogroup ACP_OPC_ALARM_CLOSE
@{*/
#define ACP_ERROR_INFO_DATA_SIZE	EDD_DEBUG_INFO_SIZE

typedef LSA_UINT8  ACP_ERROR_INFO_DATA_TYPE[ACP_ERROR_INFO_DATA_SIZE];

typedef ACP_ERROR_INFO_DATA_TYPE * ACP_UPPER_ERROR_INFO_DATA_PTR_TYPE;
/**@}*************************************************************************/


/*****************************************************************************/
/* 01 ACP_OPC_OPEN_CHANNEL                                                   */
/**************************************************************************//**
@defgroup ACP_OPC_OPEN_CHANNEL 01 ACP_OPC_OPEN_CHANNEL
@ingroup acp-opcodes
@{
@brief Creates and initializes a new ACP instance.
@details

The instance is identified by a handle after the successful opening.

This request requires a detail_ptr with the help of the @ref ACP_GET_PATH_INFO output macro.
A network interface (nic_id) to be used and the type of the channel (channel_type) must then be specified.

@see Chapter @ref acp-functions-instances-system-user-channel.

The callback function will never be called directly. ACP uses the macro @ref ACP_REQUEST_UPPER_DONE
to delegate this action to the system adaption.

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
| @ref ACP_ERR_RESOURCE        |
| @ref ACP_ERR_SEQUENCE        |
| @ref ACP_ERR_LOWER_LAYER     |
| @ref ACP_ERR_PARAM           |
| @ref ACP_ERR_SYS_PATH        |
| @ref ACP_ERR_ALLOC_LOWER_RQB |
| @ref ACP_ERR_ALLOC_LOCAL_MEM |
******************************************************************************/
#define ACP_INVALID_HANDLE	0x00

typedef  LSA_VOID  LSA_FCT_PTR(/*ATTR*/, ACP_UPPER_CALLBACK_FCT_PTR_TYPE)(
	ACP_UPPER_RQB_PTR_TYPE  rb
);

struct acp_open_channel_tag
{
	LSA_HANDLE_TYPE handle;
	/**< ## CNF
	 * Handle of the created ACP instance. In future calls this handle must also be specified in the request block with the help of the RQB.handle.
	 */

	LSA_HANDLE_TYPE handle_upper;
	/**< ## REQ
	 * The upper layer must specify its own handle here, with which the upper layer wants to be addressed in the callback function in future.
	 */

	LSA_SYS_PATH_TYPE sys_path;
	/**< ## REQ
	 * System path according to LSA. This is transferred by the upper layer to ACP and forwarded to the lower layer.
	 */

	ACP_UPPER_CALLBACK_FCT_PTR_TYPE acp_request_upper_done_ptr;
	/**< ## REQ
	 * A pointer on the callback function that is called as the answer to a request.
	 * This function is also called during an indication.
	 * This callback function can be called by ACP at any convenient time as long as the communication channel is open.
	 * The user must therefore mark critical code sections in which he should not be interrupted by ACP as 'not interruptible'.
	 */
};

typedef struct acp_open_channel_tag ACP_OPEN_CHANNEL_TYPE; /**< The type used for ACP_OPC_OPEN_CHANNEL. */

typedef ACP_OPEN_CHANNEL_TYPE * ACP_UPPER_OPEN_CHANNEL_PTR_TYPE; /**< The pointer type used for ACP_OPC_OPEN_CHANNEL. */
/**@}*************************************************************************/


/*****************************************************************************/
/* 02 ACP_OPC_CLOSE_CHANNEL                                                  */
/**************************************************************************//**
@defgroup ACP_OPC_CLOSE_CHANNEL 02 ACP_OPC_CLOSE_CHANNEL
@ingroup acp-opcodes

@brief Closes an (opened) communication channel.
@details Closes a communication channel opened with ACP_OPC_OPEN_CHANNEL and closes this ACP instance.
All alarms must have been removed before closing the communication channel.

All request blocks provided for indications are closed with @ref ACP_OK_CANCEL before the final release of the handle.

@note No type used.

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
| @ref ACP_ERR_SEQUENCE        |
******************************************************************************/


/*****************************************************************************/
/* 03 ACP_OPC_CHANNEL_INFO                                                   */
/**************************************************************************//**
@defgroup ACP_OPC_CHANNEL_INFO 03 ACP_OPC_CHANNEL_INFO
@ingroup acp-opcodes
@{
@brief Delivers different information about the network interface
@details
For this information from the lower layer (refer to [/3/](@ref acp-ref-no-3) EDD_SRV_SRT_GET_PARAMS)
as well as data from the detail pointer is evaluated (refer to @ref acp-interfaces-detailptr).

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
| @ref ACP_ERR_LOWER_LAYER     |
| @ref ACP_ERR_PARAM           |
| @ref ACP_ERR_ALLOC_LOWER_RQB |
******************************************************************************/
struct acp_channel_info_tag
{
	LSA_UINT16 eth_interface_id;
	/**< ## CNF
	 *The 'LABEL' of the assigned network interface. Corresponds to the numbering of the housing.
	 */

	LSA_UINT16 alarm_ref_base;
	/**< ## CNF
	 *Specifies the first available alarm instance reference.
	 */

	LSA_UINT16 alarm_nr_of_ref;
	/**< ## CNF
	 * Specifies the number of assigned references.\n
	 * The used alarm instance references must correspond to the following condition: \n
	 * alarm_ref_base <= used_alarm_ref < (alarm_ref_base + nr_of_alarm_ref)
	 */
};

typedef struct acp_channel_info_tag ACP_CHANNEL_INFO_TYPE; /**< The type used for ACP_OPC_CHANNEL_INFO. */

typedef ACP_CHANNEL_INFO_TYPE * ACP_CHANNEL_UPPER_INFO_PTR_TYPE; /**< The pointer type used for ACP_OPC_CHANNEL_INFO. */
/**@}*************************************************************************/


/*****************************************************************************/
/* 18 ACP_OPC_ALARM_OPEN                                                     */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_OPEN 18 ACP_OPC_ALARM_OPEN
@ingroup acp-opcodes
@{
@brief Sets up a new alarm reference.
@details
Instances the client/server of this alarm reference. Refer to [/2/](@ref acp-ref-no-2).

The request exists in the RT (ACP_OPC_UDP_ALARM_OPEN) and RT-over-UDP (ACP_OPC_UDP_ALARM_OPEN) versions.
The type of the alarm reference is marked in the ACP locally, hence no individual opcodes are required  for AlarmSend, AlarmAckSend and AlarmClose.

@attention Caution: An @ref ACP_ERR_UNSUPPORTED is returned in the current version for ACP_OPC_UDP_ALARM_OPEN.

## Result of the request

| Return Code           |
|-----------------------|
| @ref ACP_OK           |
| @ref ACP_ERR_PARAM    |
| @ref ACP_ERR_SEQUENCE |
******************************************************************************/
#define ACP_INVALID_ALARM_REF	0xFFFF /**< alarm::alarm_ref, alarm::dst_alarm_ref */

#define ACP_AL_PROP_USE_GATEWAY_MASK	0x0001 /**< alarm::properties */
#define ACP_AL_PROP_USE_GATEWAY_NO		0x0000 /**< alarm::properties */
#define ACP_AL_PROP_USE_GATEWAY_YES		0x0001 /**< alarm::properties */

#define ACP_AL_PROP_RESERVED			0xFFFE /**< alarm::properties */

struct acp_alarm_open_tag
{
	LSA_UINT16 alarm_ref;
	/**< ## REQ
	 * Identifies the new alarm reference to be opened.
	 * Must be a local RTA alarm reference that was allotted to the channel through the detail pointer.
	 * Refer to @ref acp-interfaces-detailptr
	 */

	LSA_UINT16 dst_alarm_ref;
	/**< ## REQ
	 * The RTA alarm reference of the counter point. Is specified by Context Management (user).
	 */

	LSA_UINT16 properties;
	/**< ## REQ
	 * Relevant only to RT-over-UDP. See alarm::properties
	 *
	 * @note Not used at present
	 */

	ACP_MAC_ADDR_TYPE dst_mac;
	/**< ## REQ
	 * Ethernet address of the associated alarm sink (alarm-destination).
	 * Can be for RT-over-UDP on the Ethernet address of a gateway.
	 */

	ACP_IP_ADDR_TYPE dst_ip;
	/**< ## REQ
	 * Relevant only to RTA-over-UDP. The IP address of the associated alarm sink (alarm-destination), must not be equal to 0.
	 */

	ACP_IP_ADDR_TYPE src_ip;
	/**< ## REQ
	 * Relevant only to RTA-over-UDP. The own IP address (alarm-source), must not be equal to 0.
	 */

	LSA_UINT32 usr_id;
	/**< ## REQ
	 * The user can specify an ID which is also specified additionally in an alarm indication.
	 */

	LSA_UINT16 vlan_id;
	/**< ## REQ
	 * The VLAN-ID that is recorded in each RTA Ethernet frame. Value range: 0..4095
	 */

	LSA_UINT8 timeout100ms;
	/**< ## REQ
	 * Timeout for the needed transport-ack.\n
	 * Required by ACP_OPC_ALARM_SEND and ACP_OPC_ALARM_ACK_SEND.\n
	 * Multiple of 100ms, jitter: 100ms.\n
	 * Value range: 0x01..0x64h mandatory, 0x0064 ..0xffff optional
	 *
	 * The alarm package is re-transmitted 'retry_count' times, if no transport acknowledgement is received within this timeout.
	 * The Send order is acknowledged with the @ref ACP_ERR_ASRT error if a transport acknowledgement is never received.
	 */

	LSA_UINT8 retry_count;
	/**< ## REQ
	 * Send repetitions if the transport acknowledgement is not received. Value range: 3..15
	 */
};

typedef struct acp_alarm_open_tag ACP_ALARM_OPEN_TYPE; /**< The type used for ACP_OPC_ALARM_OPEN. */

typedef ACP_ALARM_OPEN_TYPE * ACP_UPPER_ALARM_OPEN_PTR_TYPE; /**< The pointer type used for ACP_OPC_ALARM_OPEN. */

/* alarm_ref: (alarm_ref_base <= alarm_ref < alarm_ref_base + alarm_nr_of_ref) */
/* dst_mac: maybe the gateway-addr with RT-over-UDP */
/**@}*************************************************************************/


/*****************************************************************************/
/* 19 ACP_OPC_UDP_ALARM_OPEN                                                 */
/**************************************************************************//**
@defgroup ACP_OPC_UDP_ALARM_OPEN 19 ACP_OPC_UDP_ALARM_OPEN
@ingroup acp-opcodes
@brief Sets up a new alarm reference over UDP.
@details
Please refer to @ref ACP_OPC_ALARM_OPEN.
******************************************************************************/


/*****************************************************************************/
/* 20 ACP_OPC_ALARM_CLOSE                                                    */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_CLOSE 20 ACP_OPC_ALARM_CLOSE
@ingroup acp-opcodes
@{
@brief Closes an existing alarm reference.
@details
One more RTA-ERROR package is transmitted with ACP_ALARM_PRIORITY_LOW 'priority' before the alarm reference is closed.
This package contains the PNIO status (0xCF81FDrr). (rr) is to be replaced by the transferred pnio_status_reason.
'pnio_status_reason' specifies the reason of the alarm removal, refer to [/2/](@ref acp-ref-no-2).

@verbatim
normative:
	VendorDeviceErrorInfo           ::= VendorID, DeviceID, Data*

our define:
	Vendor(Siemens)DeviceErrorInfo  ::= VendorID_u16(0x002a), DeviceID_u16, PNIOVersion_u32, DebugIdent_u8, DebugData_u8 x 15

sizeof(VendorDeviceErrorInfo)             <= 24
sizeof(Vendor(Siemens)DeviceErrorInfo)    == 24
@endverbatim

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
| @ref ACP_ERR_PARAM           |
| @ref ACP_ERR_SEQUENCE        |
******************************************************************************/
#if ACP_ERROR_INFO_DATA_SIZE != 16
# error "ACP != EDD"
#endif

typedef struct acp_error_info_tag
{
	LSA_UINT16 vendor_id;
	/**< the VendorID */


	LSA_UINT16 device_id;
	/**< the DeviceID */

	LSA_UINT32 pnio_version;
	/**< the version of the PNIO stack, used by VendorDeviceErrorInfo\n
	 * must be 0 if VendorDeviceErrorInfo shall not be used within ERR-RTA-PDU
	 */

	ACP_ERROR_INFO_DATA_TYPE debug_data;
	/**< format: DebugIdent, DebugData
	 * must be filled by caller
	 */

} ACP_ERROR_INFO_TYPE;


#define ACP_PNIO_STATUS_REASON_MIN	2

typedef struct acp_alarm_close_tag
{
	LSA_UINT16 alarm_ref;
	/**< ## REQ
	 * The handle to the alarm-relation.\n
	 * Identifies an existing alarm reference.
	 */

	LSA_UINT8 pnio_status_reason;
	/**< ## REQ
	 * Specifies the standard reason according to [/2/](@ref acp-ref-no-2) for closing the alarm reference.
	 * The 'pnio_status_reason' must be more than equal to ACP_PNIO_STATUS_REASON_MIN.
	*/

	ACP_ERROR_INFO_TYPE error_info;
	/**< ## REQ
	 * If error_info.pnio_version is != 0, then the optional Block VendorDeviceErrorInfo is send inside the RTA-ERR-PDU, instead of the needed padding.
	 *
	 * If given, then the pnio_version should be given in the form:
	 @verbatim
	 pnio_version =
		( ((PNIO_VERSION		& 0xF) << 28)
		| ((PNIO_DISTRIBUTION	& 0xF) << 24)
		| ((PNIO_PROJECT_NUMBER	& 0xF) << 20)
		| ((PNIO_INCREMENT		& 0xF) << 16)
		| (((PNIO_INTEGRATION_COUNTER * 100 + PNIO_GEN_COUNTER) & 0xFFFF) << 0)
		);
	 @endverbatim
	 * For example the PNIO-Version P06.01.00.00_00.02.03.17 is shown as 0x6102013d.
	 *
	 * Vendor_id, device_id and pnio_version is sending in network byte order. The remaining debug_data is send as given.
	 */

} ACP_ALARM_CLOSE_TYPE;

typedef ACP_ALARM_CLOSE_TYPE *	ACP_UPPER_ALARM_CLOSE_PTR_TYPE; /**< The pointer type used for ACP_OPC_ALARM_CLOSE. */
/**@}*************************************************************************/


/*****************************************************************************/
/* 21 ACP_OPC_ALARM_SEND                                                     */
/* 22 ACP_OPC_ALARM_ACK_SEND                                                 */
/* 23 ACP_OPC_ALARM_IND_RES_PROVIDE                                          */
/* 24 ACP_OPC_ALARM_INDICATION                                               */
/* 25 ACP_OPC_ALARM_ACK_INDICATION                                           */
/**************************************************************************//**
@defgroup acp-opcodes-alarm 21-25 ACP_OPC_ALARM_*
@ingroup acp-opcodes
@{
@brief ACP alarm data specific opcodes
@details
The struct ACP_ALARM_DATA_TYPE is used for the following opcodes:
- @ref ACP_OPC_ALARM_SEND
- @ref ACP_OPC_ALARM_ACK_SEND
- @ref ACP_OPC_ALARM_IND_RES_PROVIDE
- @ref ACP_OPC_ALARM_INDICATION
- @ref ACP_OPC_ALARM_ACK_INDICATION

informative: existing alarm_type within IEC 61158-6

Value               | Meaning
--------------------|----------------------------------
0x0000              | reserved
0x0001              | Diagnosis appears
0x0002              | Process
0x0003              | Pull
0x0004              | Plug
0x0005              | Status
0x0006              | Update
0x0007              | Redundancy
0x0008              | Controlled by supervisor
0x0009              | Released
0x000A              | Plug Wrong Submodule
0x000B              | Return of Submodule
0x000C              | Diagnosis disappears
0x000D              | Multicast Communication Mismatch
0x000E              | Port Data Change Notification
0x000F              | Sync Data Changed Notification
0x0010              | Isochronous Mode Problem Notification
0x0011              | Network component problem notification
0x0012              | Time data changed notification
0x0013              | Dynamic Frame Packing problem notification
0x0014              | MRPD problem notification
0x0015 - 0x001D     | Reserved
0x001E              | Upload and retrieval notification
0x001F              | Pull module b
0x0020 - 0x007F     | Manufacturer specific
0x0080 - 0x00FF     | Reserved for profiles
0x0100 - 0xFFFF     | Reserved
******************************************************************************/
#define ACP_ALARM_PRIORITY_LOW			0
#define ACP_ALARM_PRIORITY_HIGH			1

#define ACP_SET_ALARM_SPECIFIER(\
	alarm_specifier_ptr, sequence_nr, diag_channel, diag_generic, diag_submod, ar_diag_state\
) {\
	*(alarm_specifier_ptr) = (LSA_UINT16)( \
			((((ar_diag_state) ? 0x1 : 0x0)  &   0x01) << 15) | \
			((((diag_submod)   ? 0x1 : 0x0)  &   0x01) << 13) | \
			((((diag_generic)  ? 0x1 : 0x0)  &   0x01) << 12) | \
			((((diag_channel)  ? 0x1 : 0x0)  &   0x01) << 11) | \
			(((sequence_nr)                  & 0x07FF) <<  0)   \
		);\
}

#define ACP_GET_ALARM_SPECIFIER(\
	sequence_nr_ptr, diag_channel_ptr, diag_generic_ptr, diag_submod_ptr, ar_diag_state_ptr, alarm_specifier\
) {\
	*(ar_diag_state_ptr) =   ((((alarm_specifier) >> 15) &   0x01) ? 1 : 0); \
	*(diag_submod_ptr)   =   ((((alarm_specifier) >> 13) &   0x01) ? 1 : 0); \
	*(diag_generic_ptr)  =   ((((alarm_specifier) >> 12) &   0x01) ? 1 : 0); \
	*(diag_channel_ptr)  =   ((((alarm_specifier) >> 11) &   0x01) ? 1 : 0); \
	*(sequence_nr_ptr)   = (LSA_UINT16)( ((alarm_specifier) >>  0) & 0x07FF); \
}

#define ACP_PNIO_STATUS_OK				0x0

#define ACP_ALARM_USER_DATA_OFFSET		28	/**< see 61158-6: AlarmNotification-PDU.AlarmItem.Data */

/** @todo ACP refactor: create separate alarm_data_tag for each opcode */

struct acp_alarm_data_tag
{
	LSA_UINT16 alarm_ref;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Identifies an existing alarm reference.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * Identifies an existing alarm reference.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * Identifies an existing alarm reference.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * Identifies an existing alarm reference.
	 */

	LSA_UINT16 priority;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * ACP_ALARM_PRIORITY_HIGH or ACP_ALARM_PRIORITY_LOW
	 *
	 * ## ACP_OPC_ALARM_IND_RES_PROVIDE: REQ
	 * The ACP_ALARM_PRORITY_HIGH and ACP_ALARM_PRORITY_LOW values define the alarm priority to which this resource is assigned.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * Must be reflected from the alarm to be acknowledged.
	 */

	LSA_UINT32 usr_id;
	/**<
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * The user 'usr_id', that must also be specified while opening the alarm reference.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * The user 'usr_id' that was also entered while opening the alarm reference.
	 */

	LSA_UINT32 api;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Application process identifier, refer to [/2/](@ref acp-ref-no-2).
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * Is transferred during ACP_OPC_ALARM_SEND.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * Must be reflected from the alarm to be acknowledged.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_ACK_SEND.
	 */

	LSA_UINT16 alarm_type;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Possible coding of field according to [/2/](@ref acp-ref-no-2):
	 * Value               | Meaning
	 * --------------------|--------------------------------
	 * 0x00                | reserved
	 * 0x01                | Diagnostic alarm (DIAL)
	 * 0x02                | Process alarm (PRAL) - high
	 * 0x03                | Pull alarm (PUAL)
	 * 0x04                | Plug alarm (PLAL)
	 * 0x05                | Status alarm (STAL)
	 * 0x06                | Update alarm (UPAL)
	 * 0x07                | Redundancy alarm (REAL)
	 * 0x08                | Controlled_by_Supervisor Alarm (CSAL)
	 * 0x09                | Released_by_Supervisor Alarm (RSAL)
	 * 0x0A                | Plug Wrong Submodule Alarm (PWAL)
	 * 0x0B                | Return of Submodule
	 * 0x0C                | Diagnostic alarm (disappearing)
	 * 0x0D                | Multicast Provider Communication Stopped
	 * 0x0E                | Multicast Provider Communication Running
	 * 0x0F                | Port Data Change Notification
	 * 0x10                | Sync Data Changed Notification
	 * 0x11                | Isochronous Mode Problem Notification
	 * 0x12                | Time data changed notification
	 * 0x13                | Dynamic Frame Packing problem notification
	 * 0x14                | MRPD problem notification
	 * 0x15                | Reserved
	 * 0x16                | Multiple interface mismatch notification
	 * 0x0017 - 0x001F     | Reserved
	 * 0x0020 - 0x007F     | Manufacturer-specific alarms (MAAL)
	 * 0x0080 - 0x00FF     | Reserved for profiles
	 * 0x0100 - 0xFFFF     | Reserved
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * Must be reflected from the alarm to be acknowledged.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_ACK_SEND.
	 */

	LSA_UINT16 slot_nr;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Slot Number. 0 - 65535 permissible
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 *Must be reflected from the alarm to be acknowledged.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_ACK_SEND.
	 */

	LSA_UINT16 subslot_nr;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Subslot Number.  0 - 65535 permissible
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * Must be reflected from the alarm to be acknowledged.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_ACK_SEND.
	 */

	LSA_UINT16 alarm_specifier;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Variable, contains the AlarmSpecifier, SequenceNumber, GenericDiagnosis and SubmoduleDiagnosis fields.
	 *
	 * This field is to be filled up or queried with the ACP_SET_ALARM_SPECIFIER() and ACP_GET_ALARM_SPECIFIER() ACP macros to maintain the internal structure.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND.
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * Must be reflected from the alarm to be acknowledged.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_ACK_SEND.
	 */

		/* for ALARM_ACK */
	LSA_UINT32 pnio_status;
	/**<
	 * ## ACP_OPC_ALARM_SEND: CNF
	 * ACP_PNIO_STATUS_xxx can be recorded in ACP_ERR_ASRT.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * refer to ACP_PNIO_STATUS_xxx
	 *
	 * ## ACP_OPC_ALARM_ACK_SEND: REQ
	 * An additional PNIO error status must also be specified here. \n
	 * An ACP_PNIO_STATUS_OK can be entered here in case of OK. \n
	 * OUT: An ACP_PNIO_STATUS_xxx can be recorded here in case of ACP_ERR_ASRT.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_ACK_SEND.
	 */

		/* for ALARM */
	LSA_UINT32 module_ident;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Module identification in which alarm is recorded.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND.
	 */

	LSA_UINT32 submodule_ident;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Submodule identification in which alarm is recorded.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND.
	 */

	LSA_UINT16 user_tag;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Format ID for the structure of the data lying in the 'user_data' block.\n
	 * 0 - 32767  manufacturer-defined structure\n
	 * 32768 - 65535  reserved
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_SEND, refer also to 'user_data'
	 */

	LSA_UINT16 user_max_length;
	/**< must be >= ACP_ALARM_USER_DATA_OFFSET + user_length
	 *
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Specifies the size of the buffer in which the user_data is pointing to.\n
	 * The following must be valid if user_length > 0:
	 * 1. user_max_length >=user_length
	 * 2. user_data should not be LSA_NULL
	 *
	 * ## ACP_OPC_ALARM_IND_RES_PROVIDE: REQ
	 * Specifies the size of the buffer to which user_data refers.\n
	 * 'user_max_length' must be less than 0xFFFF.\n
	 * 'user_max_length' must be more than equal to ACP_ALARM_USER_DATA_OFFSET.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_IND_RES_PROVIDE.
	 *
	 * ## ACP_OPC_ALARM_ACK_INDICATION: IND
	 * As transferred during ACP_OPC_ALARM_IND_RES_PROVIDE.
	 */

	LSA_UINT16 user_length;
	/**<
	 * for req: 0..no user_tag, >0 user_tag and user_data will send within this alarm-pdu\
	 * for ind: 0..no user_tag, 0xFFFF..more user_data than user_max_length, >0..user_tag and user_data are valid
	 *
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * The number of bytes to be transferred to the 'user_data'.
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * refer to 'user_data'.
	 */

	ACP_UPPER_MEM_U8_PTR_TYPE user_data;
	/**<
	 * ## ACP_OPC_ALARM_SEND: REQ
	 * Pointer on data buffer with manufacturer-specific additional alarm information. Can be LSA_NULL if user_length == 0. \n
	 * The additional alarm information must be written to the data buffer starting with Index 0.
	 *
	 * ## ACP_OPC_ALARM_IND_RES_PROVIDE: REQ
	 * Pointer on buffer of the 'user_max_length' size
	 *
	 * ## ACP_OPC_ALARM_INDICATION: IND
	 * The 'user_length' is set to 0xFFFF and the 'user_data' buffer is not filled up if the data received is more than the 'user_max_length'.\n
	 * Otherwise, the length of the additional data is stored in user_length, and 'user_data' points to the "raw" alarm block,
	 * refer to @ref ACP_OPC_ALARM_SEND.
	 *
	 * If the 'user_length' >= ACP_ALARM_USER_DATA_OFFSET, then user_tag is valid and the additional alarm information from the
	 * ACP_ALARM_USER_DATA_OFFSET index is to be found, as transferred in ACP_OPC_ALARM_SEND.\n
	 * The RTA alarm PDU can be read started with Index 0. This index corresponds to Offset 0: The alarm block as it is described
	 * in @ref ACP_OPC_ALARM_SEND. Caution: This data is always saved in the network byte order (MSB First) format.
	 */
};

typedef struct acp_alarm_data_tag ACP_ALARM_DATA_TYPE;
/**< The type used for alarm data based opcodes.
 * @note If an opcode is not quoted in the description of a data field, the opcode does not use this data field.
 */

typedef ACP_ALARM_DATA_TYPE * ACP_UPPER_ALARM_DATA_PTR_TYPE; /**< The pointer type used for alarm data based opcodes. */
/**@}*************************************************************************/


/*****************************************************************************/
/* 21 ACP_OPC_ALARM_SEND                                                     */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_SEND 21 ACP_OPC_ALARM_SEND
@ingroup acp-opcodes-alarm
@brief Sends an alarm through an existing alarm reference.
@details
@see Type used for this opcode: @ref acp_alarm_data_tag.

The data of the PROFINET IO alarm is packaged in an RTA order and forwarded to the lower layer.

The alarm block transferred through the RTA as byte stream has the following structure (refer also to [/2/](@ref acp-ref-no-2)):
@verbatim
Offset   0: UINT16  alarm_block
Offset   2: UINT16  alarm_length
Offset   4: UINT16  alarm_version
Offset   6: UINT16  alarm_type
Offset   8: UINT32  api
Offset  12: UINT16  slot_nr
Offset  14: UINT16  subslot_nr
Offset  16: UINT32  modul_ident
Offset  20: UINT32  submodul_ident
Offset  24: UINT16  alarm_specifier
Offset  26: UINT16  user_tag                    (optional)
Offset  28: UINT8   user_data[0]                (optional)
Offset  29: UINT8   user_data[1]                (optional)
...
Offset  xx: UINT8    user_data[user_length - 1]  (optional)
@endverbatim

## alarm_block:
The following values are defined:
@verbatim
ACP_ALARM_BLOCK_REQUEST_HIGH    0x0001
ACP_ALARM_BLOCK_REQUEST_LOW     0x0002
ACP_ALARM_BLOCK_ACK_HIGH        0x8001
ACP_ALARM_BLOCK_ACK_LOW         0x8002
@endverbatim

## alarm_version:
The constant 0x0100 is transferred as the version.

If the request is returned with the @ref ACP_ERR_ASRT error code, the lower layer has detected an error in the RTA protocol
that does not need to be corrected any more. The alarm reference must be closed by the user and created again.

If an alarm reference is closed, then all the still pending requests are returned with the @ref ACP_OK_CANCEL error code.

The alarm data is transferred to ACP in the host format. ACP converts this data into the required structure and the
specified network format ("network byte order format"). For this, it uses the ACP_PUT(16|32)_HTON and ACP_GET(16|32)_NTOH output macros.
These must be provided by System Adjustment (refer to @ref acp-interfaces-cfg).

The user data lying in 'user_data' is transferred without any conversion.

## Result of the request

| Return Code           |
|-----------------------|
| @ref ACP_OK           |
| @ref ACP_OK_CANCEL    |
| @ref ACP_ERR_PARAM    |
| @ref ACP_ERR_SEQUENCE |
| @ref ACP_ERR_ASRT     |
******************************************************************************/


/*****************************************************************************/
/* 22 ACP_OPC_ALARM_ACK_SEND                                                 */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_ACK_SEND 22 ACP_OPC_ALARM_ACK_SEND
@ingroup acp-opcodes-alarm
@brief Transmits an alarm acknowledge.
@details
@see Type used for this opcode: @ref acp_alarm_data_tag.

The alarm block has the following structure:
@verbatim
Offset   0: UINT16  alarm_block
Offset   2: UINT16  alarm_length
Offset   4: UINT16  alarm_version
Offset   6: UINT16  alarm_type
Offset   8: UINT32  api
Offset  12: UINT16  slot_nr
Offset  14: UINT16  subslot_nr
Offset  16: UINT16  alarm_specifier
Offset  18: UINT8   pnio_status.error_code
Offset  19: UINT8   pnio_status.error_decode
Offset  20: UINT8   pnio_status.error_code1
Offset  21: UINT8   pnio_status.error_code2
@endverbatim

ACP_ALARM_BLOCK_ACK_HIGH or ACP_ALARM_BLOCK_ACK_LOW is used as alarm block.

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
| @ref ACP_OK_CANCEL           |
| @ref ACP_ERR_PARAM           |
| @ref ACP_ERR_SEQUENCE        |
| ACP_ERR_LOWER_LAYER          |
| @ref ACP_ERR_ASRT            |
| @ref ACP_ERR_ALLOC_LOWER_RQB |
| @ref ACP_ERR_ALLOC_LOWER_MEM |
******************************************************************************/


/*****************************************************************************/
/* 23 ACP_OPC_ALARM_IND_RES_PROVIDE                                          */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_IND_RES_PROVIDE 23 ACP_OPC_ALARM_IND_RES_PROVIDE
@ingroup acp-opcodes-alarm
@brief Provides an RQB to the alarm reference for the transmission of indications.
@details
@see Type used for this opcode: @ref acp_alarm_data_tag.

Provides a request block for the receipt of an alarm.
No confirmation is returned for this request, only the @ref ACP_OPC_ALARM_INDICATION or @ref ACP_OPC_ALARM_ACK_INDICATION indication.

This request block is returned with @ref ACP_OK_CANCEL if this channel is closed.

The user of ACP must provide at least two resources. One for 'high'-priority alarms and one for 'low'-priority alarms.

ACP provides several reception resources to the lower layer. The number is read from the 'detail_pointer' of the system channel,
refer to @ref acp-interfaces-detailptr

Received alarms are queued inside ACP if no UPPER resources are available.

## Result of the request

| Return Code           |
|-----------------------|
| @ref ACP_OK_CANCEL    |
| @ref ACP_ERR_PARAM    |
******************************************************************************/


/*****************************************************************************/
/* 24 ACP_OPC_ALARM_INDICATION                                               */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_INDICATION 24 ACP_OPC_ALARM_INDICATION
@ingroup acp-opcodes-alarm
@brief An alarm was received.
@details
@see Type used for this opcode: @ref acp_alarm_data_tag.

If an alarm block containing an 'alarm_block' of ACP_ALARM_BLOCK_REQUEST_HIGH or ACP_ALARM_BLOCK_REQUEST_LOW arrives,
a free alarm indication resource is filled with the arrived data. This resource is returned as ACP_ALARM_INDICATION.

The alarm data is converted by ACP to the host format. To do this, ACP uses the @ref ACP_GET16_NTOH and @ref ACP_GET32_NTOH output macros.

The request block is provided again typically after the processing with the help of @ref ACP_OPC_ALARM_IND_RES_PROVIDE.

Special case: @ref ACP_ERR_ASRT\n
In case of a response by @ref ACP_ERR_ASRT, only the 'alarm_ref', 'usr_id', 'priority' and 'pnio_status' can be scanned.
The user must close the alarm reference after this indication. The alarm is in the 'Error' state
(refer to @ref acp-overview-alarm-provider and @ref acp-overview-alarm-consumer).

## Result of the request

| Return Code           |
|-----------------------|
| @ref ACP_OK           |
| @ref ACP_ERR_ASRT     |
******************************************************************************/


/*****************************************************************************/
/* 25 ACP_OPC_ALARM_ACK_INDICATION                                           */
/**************************************************************************//**
@defgroup ACP_OPC_ALARM_ACK_INDICATION 25 ACP_OPC_ALARM_ACK_INDICATION
@ingroup acp-opcodes-alarm
@brief An alarm confirmation was received.
@details
@see Type used for this opcode: @ref acp_alarm_data_tag.

A free alarm indication resource is filled with the arrived data if an alarm block containing an 'alarm_block'
of ACP_ALARM_BLOCK_ACK_HIGH or ACP_ALARM_BLOCK_ACK_LOW arrives. This resource is returned as ACP_ALARM_ACK_INDICATION.

The user must provide a request block again with the help of @ref ACP_OPC_ALARM_IND_RES_PROVIDE to reestablish the resource balance.

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
******************************************************************************/

#if ACP_FRAG_MODE
/*****************************************************************************/
/* 30 ACP_OPC_FRAG_SEND                                                      */
/**************************************************************************//**
@defgroup ACP_OPC_FRAG_SEND 30 ACP_OPC_FRAG_SEND
@ingroup acp-opcodes
@{
@brief Send a large data block through an existing alarm reference.
@details

The large data block is divided into individual fragments and send to the receiver.

If all of this fragments are confirmed from the receiver side, this opcode is confirmed with ACP_OK.

If the range-check fails (e.g. 'alarm_ref' not found), the error ACP_ERR_PARAM is confirmed.

If the fragments could not be transferred from any cause of error, the error ACP_ERR_ASRT is confirmed.

## More details

The large data block is divided into individual fragments with an var-part-size of 9999 bytes.
Only the last fragment may have an smaller size, the size belongs to 'data_length'.

Each of these fragments is transmitted with the PDU-type RTA-FRAG.

Each of this fragment, except the last one, have set the AddFlag.MoreFrags.

The priority ACP_ALARM_PRIORITY_FRAG is used. This means, the frame_id 0xFFFF is watched on the net.

The first fragment starts with a window-size of 1. Each RTA-ACK may adjust this window-size in the range from 1 to 7.

This means, the first fragment has set the flag AddFlags.TACK and expect an RTA-ACK from the receiver side.

This RTA-ACK acknowledge the fragment sequence number and may set the window size, e.g. to 3.

Following this, two fragments without TACK followed by a fragment with TACK can be seen on the net.

## Result of the request

| Return Code                  |
|------------------------------|
| @ref ACP_OK                  |
| @ref ACP_ERR_PARAM           |
| @ref ACP_ERR_ASRT            |
*******************************************************************************/
#define ACP_ASRT_PRIORITY_FRAG 2 /**< Priority belongs to ACP_ALARM_PRIORITY_LOW and ACP_ALARM_PRIORITY_HIGH */

struct acp_frag_send_tag
{
	LSA_UINT16                  alarm_ref;      /* req: see alarm_open */
	LSA_UINT32                  data_length;    /* req: length of data */
	ACP_UPPER_MEM_U8_PTR_TYPE   data;           /* req: pointer to data to send */
};

typedef struct acp_frag_send_tag   ACP_FRAG_SEND_TYPE;

typedef ACP_FRAG_SEND_TYPE *       ACP_FRAG_SEND_PTR_TYPE;
/**@}*************************************************************************/


/*****************************************************************************/
/* 31 ACP_OPC_FRAG_RECV                                                      */
/**************************************************************************//**
@defgroup ACP_OPC_FRAG_RECV 31 ACP_OPC_FRAG_RECV
@ingroup acp-opcodes
@{
@brief Receives a large data block through an existing alarm reference.
@details
*******************************************************************************/
struct acp_frag_recv_tag
{
	LSA_UINT16                  alarm_ref;        /* req: see alarm_open */
	LSA_UINT32                  data_max_length;  /* req: length of buffer */
	LSA_UINT32                  data_length;      /* ind: length of received data */
	ACP_UPPER_MEM_U8_PTR_TYPE   data;             /* req: pointer to data to send */
	LSA_UINT8                   reprovide;        /* req: 1 if reprovide, otherwise 0 */
};

typedef struct acp_frag_recv_tag   ACP_FRAG_RECV_TYPE;

typedef ACP_FRAG_RECV_TYPE *       ACP_FRAG_RECV_PTR_TYPE;
/**@}*************************************************************************/

#endif


/**************************************************************************//**
@defgroup acp-group-rqb ACP RQBs
@ingroup acp-group
@{
For the \#defines ACP_RQB_HEADER and ACP_RQB_TRAILER of the RQB type, refer @ref acp-interfaces-cfg-header-trailer.

Each UNION in the RQB structure corresponds to at least one opcode.

Example access for Channel Open:

@snippet "code examples/acp_opc_open_channel_example.c" acp-opc-open-channel-snippet

Example access for Provider Add:

@snippet "code examples/acp_opc_provider_add_example.c" acp-opc-provider-add-snippet
******************************************************************************/

/*------------------------------------------------------------------------------
// UNION of all types
//----------------------------------------------------------------------------*/

union acp_args_tag
{
	union {
		ACP_OPEN_CHANNEL_TYPE  open;
		ACP_CHANNEL_INFO_TYPE  info;
	} channel;

	union {
		ACP_ALARM_OPEN_TYPE  open;
		ACP_ALARM_CLOSE_TYPE  close;
		ACP_ALARM_DATA_TYPE  data;

#if ACP_FRAG_MODE
		ACP_FRAG_SEND_TYPE frag_send;
		ACP_FRAG_RECV_TYPE frag_recv;
#endif

	} alarm;
};

typedef union acp_args_tag					ACP_ARGS_TYPE;

typedef ACP_ARGS_TYPE  *	ACP_UPPER_ARGS_PTR_TYPE;


/*------------------------------------------------------------------------------
// ACP_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

struct acp_rqb_tag
{
	ACP_RQB_HEADER /**< refer to @ref acp-interfaces-cfg-header-trailer */

	/* ACP needed data */
	ACP_UPPER_ARGS_PTR_TYPE	 args; /**< arguments corresponding to the opcode, see @ref acp-opcodes */

	ACP_RQB_TRAILER /**< refer to @ref acp-interfaces-cfg-header-trailer */
};

typedef struct acp_rqb_tag ACP_RQB_TYPE;
/**@}*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define ACP_RQB_SET_ARGS(rb,v)			{ (rb)->args = (v); }
#define ACP_RQB_GET_ARGS(rb)			((rb)->args)


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*------------------------------------------------------------------------------
// access to consumer-provider-buffer
//	AP01403259: removed because of new EDDS buffer interface
//----------------------------------------------------------------------------*/


/*====  in functions  =====*/

/**************************************************************************//**
@defgroup acp-group-input-functions Input Functions
@ingroup acp-group
@brief The configuration options and the input functions are listed on this page.

## Unused input functions

### acp_upper_rqb(LSA_USER_ID_TYPE user_id, LSA_UINT16 length, ACP_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
Asynchronous allocation of an upper-RQB.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_rqb_ptr pointer to upper RQB

### acp_upper_mem(LSA_USER_ID_TYPE user_id, LSA_UINT16 length, ACP_UPPER_MEM_PTR_TYPE upper_mem_ptr)
Asynchronous allocation of upper-memory.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_mem_ptr pointer to upper memory.
******************************************************************************/

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Opening a communication channel
@details
LSA function, asynchronous

Creates and initializes an ACP instance. Opens a communication channel with LOWER_LAYER or uses an already open communication channel or another ACP instance.

A callback function must also be specified in the request block. All asynchronous functions are answered through this callback function.

For more details on Detail pointer, please refer to Chapter @ref acp-interfaces-detailptr.

For more details on the request block, please refer to Chapter @ref acp-interfaces-structure-rqb.

The LSA output function FATAL_ERROR is called in case of missing or faulty data.

Request: @ref ACP_OPC_OPEN_CHANNEL

@param [in] upper_rqb_ptr Pointer on request block
******************************************************************************/
LSA_VOID acp_open_channel(
	ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Closing a communication channel
@details
LSA function, asynchronously

Releases the local memory requested in acp_open_channel() and closes the communication channel with the LOWER LAYER if necessary.\n
Before closing the communication channel, all the request blocks that were provided for indications with the help of ACP_OPC_ALARM_IND_RES_PROVIDE
are reported back with cancel.\n

Request: @ref ACP_OPC_CLOSE_CHANNEL

@param [in] upper_rqb_ptr Pointer on request block
******************************************************************************/
LSA_VOID acp_close_channel(
	ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Processing a request
@details
LSA function, asynchronous

Accepts a request block from an upper level for processing.\n
The results are entered in the request block and answered with the callback function disclosed asynchronously with the acp_open_channel().\n
The functionality to be executed is determined by the opcode in the request block. The user must provide the required parts of the request block,
depending on the entered opcode. The value entered in the opcode is also called request subsequently.
For a more precise description of the requests and the required parameters, please refer to the description of the @ref acp-opcodes.

Request: All requests as specified in the interface description, except the ACP_OPEN_CHANNEL and ACP_CLOSE_CHANNEL requests.

@param [in] upper_rqb_ptr Pointer on request block
******************************************************************************/
LSA_VOID acp_request(
	ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

#if 0 /* LSA-function, not used */
/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Asynchronous allocation of an upper-RQB.
@details Not used.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_rqb_ptr pointer to upper RQB
******************************************************************************/
LSA_VOID acp_upper_rqb(
	LSA_USER_ID_TYPE           user_id,
	LSA_UINT16                 length,
	ACP_UPPER_RQB_PTR_TYPE     upper_rqb_ptr
);
#endif


#if 0 /* LSA-function, not used */
/**************************************************************************//**
@ingroup acp-group-input-functions
@brief Asynchronous allocation of an upper-memory.
@details Not used.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_mem_ptr pointer to upper memory.
******************************************************************************/
LSA_VOID acp_upper_mem(
	LSA_USER_ID_TYPE           user_id,
	LSA_UINT16                 length,
	ACP_UPPER_MEM_PTR_TYPE     upper_mem_ptr
);
#endif

/*====  out functions  =====*/


/**************************************************************************//**
@defgroup acp-group-output-macros Output Macros
@ingroup acp-group
@brief These are the output functions for ACP.
******************************************************************************/


/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocate an upper-RQB.
@details

@param [out] *upper_rqb_ptr_ptr  return value: pointer to upper-RQB-pointer or LSA_NULL: The allocation will be asynchronous, see acp_upper_rqb().
@param [in]  user_id             id of acp
@param [in]  length              length of RQB
@param [in]  sys_ptr             system-pointer
@return LSA_VOID
******************************************************************************/
#ifndef ACP_ALLOC_UPPER_RQB
LSA_VOID ACP_ALLOC_UPPER_RQB(
	ACP_UPPER_RQB_PTR_TYPE  *  upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE           user_id,
	LSA_UINT32                 length,
	LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Free an upper-RQB.
@details

@param [out] *ret_val_ptr   return value: LSA_RET_OK: ok; LSA_RET_ERR_PARAM: no deallocation because of wrong pointer to upper-RQB or wrong system-pointer.
@param [in]  upper_rqb_ptr  pointer to upper-RQB
@param [in]  sys_ptr        system-pointer
@return LSA_VOID
******************************************************************************/
#ifndef ACP_FREE_UPPER_RQB
LSA_VOID ACP_FREE_UPPER_RQB(
	LSA_UINT16               *  ret_val_ptr,
	ACP_UPPER_RQB_PTR_TYPE      upper_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Allocate upper-memory.
@details

@param [out] *upper_mem_ptr_ptr  return value: pointer to upper-memory-pointer or LSA_NULL: The allocation will be asynchronous, see acp_upper_mem().
@param [in]  user_id pointer     id of acp
@param [in]  length              length of memory
@param [in]  sys_ptr             system-pointer
@return LSA_VOID
******************************************************************************/
#ifndef ACP_ALLOC_UPPER_MEM
LSA_VOID ACP_ALLOC_UPPER_MEM(
	ACP_UPPER_MEM_PTR_TYPE  *  upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE           user_id,
	LSA_UINT32                 length,
	LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Free upper-memory.
@details

@param [out] *ret_val_ptr   return value: LSA_RET_OK: ok; LSA_RET_ERR_PARAM: no deallocation because of wrong pointer to upper-memory or wrong system-pointer.
@param [in]  upper_mem_ptr  pointer to upper-memory
@param [in]  sys_ptr        system-pointer
@return LSA_VOID
******************************************************************************/
#ifndef ACP_FREE_UPPER_MEM
LSA_VOID ACP_FREE_UPPER_MEM(
	LSA_UINT16                 *  ret_val_ptr,
	ACP_UPPER_MEM_PTR_TYPE        upper_mem_ptr,
	LSA_SYS_PTR_TYPE              sys_ptr
);
#endif

/**************************************************************************//**
@ingroup acp-group-output-macros
@brief Send a request to "self"
@details
Serves the purpose of transferring an upper RQB to the ACP task.

Used in the @ref acp_timeout function

@param [in] upper_rqb_ptr  pointer to upper-RQB
******************************************************************************/
#ifndef ACP_REQUEST_LOCAL
LSA_VOID
ACP_REQUEST_LOCAL (
	ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_USR_H */
