#ifndef CM_ARCB_H
#define CM_ARCB_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_arcb.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  ARCB, the application relation control block                             */
/*                                                                           */
/*****************************************************************************/

/*
 * included by cm_usr.h
 */

#ifndef CM_CFG_EVERYTHING_VISIBLE
# error "something wrong with the includes"
#endif

/*------------------------------------------------------------------------------
// ARCB .. the Application Relation Control Block
//----------------------------------------------------------------------------*/

/*
//	SPH AS_mech204_V7.22i12 Draft :: Address mode flags
//
//	Bit 0: IP issue
//			0: The IO controller issues IP Suite
//			1: The IO controller uses the found IP Suite
//  Bit 1: Search protocol
//			0: The IO controller searches with DCP
//			1: Reserved
//	Bit 2: don't care
//	Bit 3: Reserved (CM-upper interface only)
//
//	Bit 4: set IP Suite remanent
//			0: set IP Suite temporary
//			1: set IP Suite remanent
*/

/**************************************************************************//**
@defgroup CM_ARCB_TYPE CM_ARCB_TYPE (AR Control Block)
@ingroup cm-rqbs-group
@see @ref cm-rqbs-cm-arcb-type
******************************************************************************/

/*****************************************************************************/
/* CM_ARCB_NARE_TYPE                                                         */
/**************************************************************************//**
@defgroup CM_ARCB_NARE_TYPE CM_ARCB_NARE_TYPE
@ingroup CM_ARCB_TYPE
@{
@brief Part of CM_CL_DEVICE_ADD_TYPE
******************************************************************************/
#define CM_NARE_MODE_RESERVED_MASK		0xFFFFFFEA
#define CM_NARE_MODE_DONTCARE_MASK		0x00000004

#define CM_NARE_MODE_MASK				0x00000001
#define CM_NARE_MODE_DCP_CONFIG_IP		0x00000000
#define CM_NARE_MODE_DCP_RESOLVE_IP		0x00000001
/*      CM_NARE_MODE_DNS_CONFIG_IP		0x00000002 reserved, see 740653 */
/*      CM_NARE_MODE_DNS_RESOLVE_IP		0x00000003 reserved, see 740653 */
/*      CM_NARE_MODE_USE_IP				0x00000008 reserved, see TIA 1723405 */

#define CM_NARE_MODE_SETIP_MASK			0x00000010
#define CM_NARE_MODE_SETIP_TEMPORARY	0x00000000
#define CM_NARE_MODE_SETIP_REMANENT		0x00000010


typedef struct cm_arcb_nare_alias_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_nare_alias_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_NARE_ALIAS_TYPE
	 */

	LSA_UINT16 name_length;
	/**< ## REQ
	 * Length of name
	 */

	CM_UPPER_MEM_U8_PTR_TYPE name;
	/**< ## REQ
	 * Not null-terminated alias-name of IO-Device
	 */

} CM_ARCB_NARE_ALIAS_TYPE;

typedef CM_ARCB_NARE_ALIAS_TYPE * CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE;

typedef CM_ARCB_NARE_ALIAS_TYPE const * CM_UPPER_ARCB_NARE_ALIAS_CONST_PTR_TYPE;

typedef struct cm_arcb_nare_tag {
	LSA_UINT32 properties;
	/**< ## REQ
	 * Type of address resolution, see @ref CM_NARE_MODE_RESERVED_MASK, ...
	 */

	LSA_UINT16 device_name_length;
	/**< ## REQ
	 * Length of device name
	 */

	CM_UPPER_MEM_U8_PTR_TYPE device_name;
	/**< ## REQ
	 * Pointer to name of device, not \0 terminated
	 */

	LSA_UINT32 device_ip;
	/**< ## REQ
	 * Only for CM_NARE_MODE_DCP_CONFIG_IP: IP address for naming
	 */

	LSA_UINT32 device_subnet_mask;
	/**< ## REQ
	 * Only for CM_CL_DEVICE_MODE_USE_DEF_ROUTER_YES for naming
	 */

	LSA_UINT32 device_gateway_ip;
	/**< ## REQ
	 * Only for CM_CL_DEVICE_MODE_USE_DEF_ROUTER_YES for naming
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_nare_alias_tag) device_alias_list;
	/**< ## REQ
	 * List with alias names (CM_ARCB_NARE_ALIAS_TYPE)
	 */

} CM_ARCB_NARE_TYPE; /**< The type used by CM_CL_DEVICE_ADD_TYPE */

typedef CM_ARCB_NARE_TYPE * CM_UPPER_ARCB_NARE_PTR_TYPE; /**< The pointer type used by CM_CL_DEVICE_ADD_TYPE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_PRM_TYPE                                                          */
/**************************************************************************//**
@defgroup CM_ARCB_PRM_TYPE CM_ARCB_PRM_TYPE
@ingroup CM_ARCB_TYPE
@{
@brief Part of CM_ARCB_ADD_TYPE (pointer)
******************************************************************************/
#define CM_PRM_PROP_RESERVED_MASK  0xFFFFFFFF /**< reserved (set to zero) */

typedef struct cm_arcb_prm_tag {
	LSA_UINT32 prm_properties;
	/**< ## REQ
	 * See @ref CM_PRM_PROP_RESERVED_MASK, ...
	 */

	LSA_UINT16 prm_server_name_length;
	/**< ## REQ
	 * Length of prm_server_name
	 */

	CM_UPPER_MEM_U8_PTR_TYPE prm_server_name;
	/**< ## REQ
	 * Not null-terminated name of prm-server
	 */

	LSA_UINT16 prm_vendor_id;
	/**< ## REQ
	 * Identification of the PRM-server (object_uuid)
	 */

	LSA_UINT16 prm_device_id;
	/**< ## REQ
	 * Identification of the PRM-server (object_uuid)
	 */

	LSA_UINT16 prm_instance;
	/**< ## REQ
	 * Identification of the PRM-server (object_uuid)
	 */

} CM_ARCB_PRM_TYPE; /**< The type used by CM_ARCB_ADD_TYPE (pointer) */

typedef CM_ARCB_PRM_TYPE * CM_UPPER_ARCB_PRM_PTR_TYPE; /**< The pointer type used by CM_ARCB_ADD_TYPE (pointer) */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_ALARM_CR_TYPE                                                     */
/**************************************************************************//**
@defgroup CM_ARCB_ALARM_CR_TYPE CM_ARCB_ALARM_CR_TYPE
@ingroup CM_ARCB_TYPE
@{
@brief Part of CM_ARCB_ADD_TYPE (pointer)
******************************************************************************/
#define CM_ALCR_TYPE_RESERVED_0  0x0000
#define CM_ALCR_TYPE_ALARM       0x0001


#define CM_ALCR_PROP_PRIO_MASK       0x00000001 /**< Bit 0: AlarmCRProperties.Priority */
#define CM_ALCR_PROP_PRIO_USER       0x00000000 /**< priority given by the user */
#define CM_ALCR_PROP_PRIO_ONLY_LOW   0x00000001 /**< use only low priority */

#define CM_ALCR_PROP_TRANSPORT_MASK  0x00000002 /**< Bit 1: AlarmCRProperties.Transport */
#define CM_ALCR_PROP_TRANSPORT_RTA   0x00000000 /**< Alarm CR uses Data-RTA-PDU */
#define CM_ALCR_PROP_TRANSPORT_UDP   0x00000002 /**< Alarm CR uses UDP-RTA-PDU */

#define CM_ALCR_PROP_UNCHECKED_2_23  0x00FFFFFC /**< Bit 2 - 23: reserved_1, set to zero but do not check */

#define CM_ALCR_PROP_RESERVED_24_31  0xFF000000 /**< Bit 24 - 31: reserved_2, set to zero and test for zero */


typedef struct cm_arcb_alarm_cr_tag {
	LSA_UINT32		alcr_properties;		/**< see defines @ref CM_ALCR_PROP_PRIO_MASK, ... */
	LSA_UINT16		timeout100ms;			/**< >= 1 */
	LSA_UINT16		retries;				/**< 3..15 */

	/* LSA_UINT16	alcr_tag_header_high ... removed with V6.0i1 (TIA 946498) */
	/* LSA_UINT16	alcr_tag_header_low	... removed with V6.0i1 (TIA 946498) */
} CM_ARCB_ALARM_CR_TYPE; /**< The type used by CM_ARCB_ADD_TYPE */

typedef CM_ARCB_ALARM_CR_TYPE * CM_UPPER_ARCB_ALARM_CR_PTR_TYPE; /**< The pointer type used by CM_ARCB_ADD_TYPE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_MCR_TYPE                                                          */
/**************************************************************************//**
@defgroup CM_ARCB_MCR_TYPE CM_ARCB_MCR_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
#define CM_MCR_PROP_PROTOCOL_MASK     0x00000007UL
#define CM_MCR_PROP_PROTOCOL_DNS      0x00000001UL
#define CM_MCR_PROP_PROTOCOL_DCP      0x00000002UL

#define CM_MCR_PROP_UNCHECKED_3_7     0x000000F8UL /**< Bit 3 -  7: reserved_1, set to zero but do not check */
#define CM_MCR_PROP_RESERVED_8_15     0x0000FF00UL /**< Bit 8 - 15: reserved_2, set to zero and test for zero */

#define CM_MCR_PROP_FACTOR_1SEC_MASK  0xFFFF0000UL /**< 0..reserved, 0x01-0x64 mandatory, >= 0x65 optional */

#define CM_MCR_PROP_GET_FACTOR_1SEC(mcr_prop)  ((LSA_UINT16)((mcr_prop) >> 16))

#define CM_MCR_PROPERTIES_BUILD(protocol, factor) (\
	((protocol) & CM_MCR_PROP_PROTOCOL_MASK) \
	| (((LSA_UINT32)(LSA_UINT16)(factor)) << 16) \
	)


typedef struct cm_arcb_mcr_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_mcr_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h
	 * Entry type: CM_ARCB_MCR_TYPE
	 */

	LSA_UINT16 iocr_ref;
	/**< ## REQ
	 * For reference to iocr
	 */

	LSA_UINT32 mcr_properties;
	/**< ## REQ
	 * AddressResolutionProperties, see defines @ref CM_MCR_PROP_PROTOCOL_MASK, ...
	 */

	LSA_UINT16 mci_timeout_100ms;
	/**< ## REQ
	 * 0 <= MCITimeoutFactor <= 100 \n
	 * Base: 100ms
	 */

	LSA_UINT16 provider_name_length;
	/**< ## REQ
	 * Length of ProviderStationName
	 */

	CM_UPPER_MEM_U8_PTR_TYPE provider_name;
	/**< ## REQ
	 * ProviderStationName (not null-terminated)
	 */

} CM_ARCB_MCR_TYPE;

typedef CM_ARCB_MCR_TYPE * CM_UPPER_ARCB_MCR_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_RPC_TYPE                                                          */
/**************************************************************************//**
@defgroup CM_ARCB_RPC_TYPE CM_ARCB_RPC_TYPE
@ingroup CM_ARCB_TYPE
@{
@brief Part of CM_ARCB_ADD_TYPE
******************************************************************************/
typedef struct cm_arcb_rpc_tag {
	LSA_UINT16 vendor_id;
	/**< ## REQ
	 * Vendor ID of the device Object UUID
	 */

	LSA_UINT16 device_id;
	/**< ## REQ
	 * Device ID of the device Object UUID
	 */

	LSA_UINT16 instance;
	/**< ## REQ
	 * Instance number of the device Object UUID
	 */

/* AP00784533 */
	LSA_UINT32 max_supported_record_length;
	/**< ## REQ
	 * Gives the largest CM_OPC_CL_RECORD_WRITE::data_length \n
	 * 0 means the default value "4152" (CM_RECORD_OFFSET + 4068)
	 * ## CNF
	 * Returns the default value, if 0 is given.
	 */
} CM_ARCB_RPC_TYPE; /**< The type used by CM_ARCB_ADD_TYPE */

typedef CM_ARCB_RPC_TYPE * CM_UPPER_ARCB_RPC_PTR_TYPE; /**< The pointer type used by CM_ARCB_ADD_TYPE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_IR_INFO_TYPE                                                      */
/**************************************************************************//**
@defgroup CM_ARCB_IR_INFO_TYPE CM_ARCB_IR_INFO_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
typedef struct cm_arcb_ir_info_sub_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ir_info_sub_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_IR_INFO_SUB_TYPE
	 */

	LSA_UINT16 iocr_ref;
	/**< ## REQ
	 * The IOCR reference
	 */

	LSA_UINT16 subframe_offset;
	/**< ## REQ
	 * The SubframeOffset, see [/11/](@ref cm-ref-no-11)
	 */

	LSA_UINT32 subframe_data;
	/**< ## REQ
	 * The SubframeData, see [/11/](@ref cm-ref-no-11)
	 */

} CM_ARCB_IR_INFO_SUB_TYPE;

typedef CM_ARCB_IR_INFO_SUB_TYPE * CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE;


typedef struct cm_arcb_ir_info_tag {
	CLRPC_UUID_TYPE ir_data_uuid;
	/**<  ## REQ
	 * The IRDataUUID
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ir_info_sub_tag) ir_info_sub_list;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h \n
	 * List of: CM_ARCB_IR_INFO_SUB_TYPE
	 */

} CM_ARCB_IR_INFO_TYPE;

typedef CM_ARCB_IR_INFO_TYPE * CM_UPPER_ARCB_IR_INFO_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_SR_INFO_TYPE                                                      */
/**************************************************************************//**
@defgroup CM_ARCB_SR_INFO_TYPE CM_ARCB_SR_INFO_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
#define CM_SR_PROP_INP_VALID_ON_BACKUP_AR  0x00000001
#define CM_SR_PROP_RESERVED_1_MASK         0x00000002
#define CM_SR_PROP_MODE                    0x00000004
#define CM_SR_PROP_RESERVED_2_MASK         0x0000FFF8
#define CM_SR_PROP_RESERVED_3_MASK         0xFFFF0000

typedef struct cm_arcb_sr_info_tag {
	LSA_UINT16 rdh_factor;
	/**< ## REQ
	 * RedundancyDataHoldFactor
	 */

	LSA_UINT32 sr_properties;
	/**< ## REQ
	 * See CM_SR_PROP_INP_VALID_ON_BACKUP_AR, ...
	 */

} CM_ARCB_SR_INFO_TYPE;

typedef CM_ARCB_SR_INFO_TYPE * CM_UPPER_ARCB_SR_INFO_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_ARVENDOR_TYPE                                                     */
/**************************************************************************//**
@defgroup CM_ARCB_ARVENDOR_TYPE CM_ARCB_ARVENDOR_TYPE
@ingroup CM_ARCB_TYPE
@{
@note The feature ARVendorBlock is currently not supported, arvendor_data_len must be 0.
******************************************************************************/
typedef struct cm_arcb_arvendor_tag {
	LSA_UINT16 arvendor_data_len;
	/**< ## REQ
	 * 0 or length of arvendor_data
	 */

	CM_UPPER_MEM_U8_PTR_TYPE arvendor_data;
	/**< ## REQ
	 * Vendor specific data \n
	 * LSA_NULL or ARVendorBlockReq 1..n if .arvendor_data_len > 0, see [/11/](@ref cm-ref-no-11).
	 */

} CM_ARCB_ARVENDOR_TYPE;

typedef CM_ARCB_ARVENDOR_TYPE * CM_UPPER_ARCB_ARVENDOR_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_SUBMODULE_PROPERTIES_TYPE                                         */
/**************************************************************************//**
@defgroup CM_ARCB_SUBMODULE_PROPERTIES_TYPE CM_ARCB_SUBMODULE_PROPERTIES_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
#define CM_SUB_COC_GROUPID_MASK     0x00FF /**< 0..submodule does not belong to a coc, 1-n..submodule belong to this coc group */
#define CM_SUB_COC_IS_ACCESS_POINT  0x0100 /**< 1..submodule is the access point for the coc group */
#define CM_SUB_COC_RESERVED         0xFE00 /**< reserved */

typedef struct cm_arcb_submodule_properties_tag {
	LSA_UINT16 submodule_properties_data_len;
	/**< ## REQ
	 * 0 or length of submodule_properties_data
	 */

	CM_UPPER_MEM_U8_PTR_TYPE submodule_properties_data;
	/**< ## REQ
	 * LSA_NULL or SubmoduleProperties \n
	 * Additional properties for IO device submodules, see [/23/](@ref cm-ref-no-23) and [/24/](@ref cm-ref-no-24)
	 */

} CM_ARCB_SUBMODULE_PROPERTIES_TYPE;

typedef CM_ARCB_SUBMODULE_PROPERTIES_TYPE * CM_UPPER_SUBMODULE_PROPERTIES_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_RS_INFO_TYPE                                                      */
/**************************************************************************//**
@defgroup CM_ARCB_RS_INFO_TYPE CM_ARCB_RS_INFO_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
typedef struct cm_arcb_rs_info_tag {
	LSA_UINT16 rs_info_data_len;
	/**< ## REQ
	 * 0 or length of rs_info_data
	 */

	CM_UPPER_MEM_U8_PTR_TYPE rs_info_data;
	/**< ## REQ
	 * LSA_NULL or RSInfoBlock \n
	 * Properties for "Reporting System", see /xxx/
	 */

} CM_ARCB_RS_INFO_TYPE;

typedef CM_ARCB_RS_INFO_TYPE * CM_UPPER_ARCB_RS_INFO_PTR_TYPE;
/** @} ***********************************************************************/


/*----------------------------------------------------------------------------*/
/*	IODConnectReq ::=
//    ARBlockReq,
//      { [IOCRBlockReq*],  [AlarmCRBlockReq], [ExpectedSubmoduleBlockReq*]
//      , [PrmServerBlock], [MCRBlockReq*],    [ARRPCBlockReq]
//      , [IRInfoBlock],    [SRInfoBlock],     [ARVendorBlockReq*]
//      , [ARFSUBlock],     [RSInfoBlock]
//      }
*/

/*****************************************************************************/
/* AR type and AR properties                                                */
/**************************************************************************//**
@defgroup cm-arcb-ar-type-and-properties AR type and AR properties
@ingroup CM_ARCB_TYPE
@{
@brief Parameters of CM_CL_DEVICE_ADD_TYPE
******************************************************************************/
#define CM_AR_TYPE_SINGLE         0x0001 /**< IOCARSingle */
#define CM_AR_TYPE_SUPERVISOR     0x0006 /**< IOSAR, The supervisor AR is a special form of the IOCARSingle */
#define CM_AR_TYPE_SINGLE_RTC3    0x0010 /**< IOCARSingle using RT_CLASS_3 */
#define CM_AR_TYPE_SINGLE_SYSRED  0x0020 /**< IOCARSR, The SR AR is a special form of the IOCARSingle indicating system redundancy or configure in run usage */


#define CM_AR_PROP_STATE_MASK                   0x00000007UL /**< Bit 0 - 2: ARProperties.State */
#define CM_AR_PROP_STATE_ACTIVE                 0x00000001UL

#define CM_AR_PROP_SUPERVISOR_TAKEOVER_ALLOWED  0x00000008UL /**< Bit 3: ARProperties.SupervisorTakeoverAllowed */

#define CM_AR_PROP_PRM_SERVER_CMINITIATOR       0x00000010UL /**< Bit 4: ARProperties.ParametrizationServer (1 = CMInitiator, 0 = external) */

#define CM_AR_PROP_UNCHECKED_5_7                0x000000E0UL /**< Bit 5 - 7: reserved_1, set to zero but do not check */

#define CM_AR_PROP_DEVICE_ACCESS                0x00000100UL /**< Bit 8: ARProperties.DeviceAccess (1 = yes, 0 = no) */

#define CM_AR_PROP_COMPANION_AR_MASK            0x00000600UL /**< Bit 9-10: ARProperties.CompanionAR */
#define CM_AR_PROP_COMPANION_AR_SINGLE          0x00000000UL /**< single AR */

#define CM_AR_PROP_ACKNOWLEDGE_COMPANION_AR     0x00000800UL /**< Bit 11: Companion AR with acknowledge (1 = with, 0 = without) */

#define CM_AR_PROP_UNCHECKED_12_23              0x00FFF000UL /**< Bit 12 - 23: reserved_2, set to zero but do not check */

#define CM_AR_PROP_RESERVED_24_28               0x1F000000UL /**< Bit 24 - 28: reserved_3, set to zero and test for zero */

#define CM_AR_PROP_COMBINED_OBJECT_CONTAINER    0x20000000UL /**< Bit 29: ARProperties.CombinedObjectContainer (1 = with COC, 0 = without) */
#define CM_AR_PROP_STARTUP_MODE_ADVANCED        0x40000000UL /**< Bit 30: ARProperties.StartupMode (1 = advanced, 0 = legacy) */

#define CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED    0x80000000UL /**< Bit 31: The AlarmType(=Pull module) shall signal pulling of module. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_ADD_TYPE                                                          */
/**************************************************************************//**
@defgroup CM_ARCB_ADD_TYPE CM_ARCB_ADD_TYPE
@ingroup CM_ARCB_TYPE
@{
@brief Parameters of CM_OPC_CL_DEVICE_ADD. Header of an ARCB
@details
With "ar_prm_record_list" a list of prm-records can be handed over. These records will only be transmitted in the connect phase.
These records shall be transmitted before any submodule related record. The index of these records should be AR-related.
******************************************************************************/
typedef struct cm_arcb_add_tag {
		/* ARBlockReq */
	LSA_UINT16 ar_nr;
	/**< ## REQ
	 * Local identification of this AR; 1..255
	 */

	LSA_UINT16 ar_type;
	/**< ## REQ
	 * See defines @ref CM_AR_TYPE_SINGLE, ...
	 */

	CLRPC_UUID_TYPE ar_uuid;
	/**< ## REQ
	 * AR UUID reference
	 * ## CNF
	 * Modified AR UUID reference if CM_AR_TYPE_SINGLE_SYSRED
	 */

	LSA_UINT32 ar_properties;
	/**< ## REQ
	 * See defines @ref CM_AR_PROP_STATE_MASK, ...
	 */

		/* IOCRBlockReq */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_tag) iocr_list;
	/**< ## REQ
	 * List of CM_ARCB_IOCR_TYPE
	 * ## CNF
	 * Filled frame_id
	 */

		/* IOCR local sendclock-adaption */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_local_tag) iocr_local_list;
	/**< Optional list of CM_ARCB_IOCR_LOCAL_TYP
	 */

		/* AlarmCRBlockReq */
	CM_UPPER_ARCB_ALARM_CR_PTR_TYPE alarm_cr;
	/**< ## REQ
	 * max_alarm_data_length, see Create-Client
	 */

		/* ExpectedSubmoduleBlockReq: expected api/modules/submodules/prm_records */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ap_tag) ap_list;
	/**< ## REQ
	 * List of CM_ARCB_AP_TYPE
	 */

		/* PrmServerBlock */
	CM_UPPER_ARCB_PRM_PTR_TYPE prm;
	/**< ## REQ
	 * LSA_NULL or an PrmServerBlock
	 */

		/* MCRBlockReq */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_mcr_tag) mcr_list;
	/**< ## REQ
	 * List of CM_ARCB_MCR_TYPE
	 */

		/* IRInfoBlock */
	CM_UPPER_ARCB_IR_INFO_PTR_TYPE ir_info;
	/**< ## REQ
	 * LSA_NULL or an IRInfoBlock
	 */

		/* SRInfoBlock */
	CM_UPPER_ARCB_SR_INFO_PTR_TYPE sr_info;
	/**< ## REQ
	 * LSA_NULL or an SRInfoBlock, for CM_AR_TYPE_SINGLE_SYSRED
	 */

		/* ARVendorBlockReq */
	CM_ARCB_ARVENDOR_TYPE arvendor_blob;
	/**< ## REQ
	 * ARVendorBlockReq 1..n
	 */

		/* SubmoduleProperties */
	CM_ARCB_SUBMODULE_PROPERTIES_TYPE submodule_properties_blob;
	/**< ## REQ
	 * SubmoduleProperties
	 */

		/* RSInfoBlock */
	CM_ARCB_RS_INFO_TYPE rs_info;
	/**< ## REQ
	 * RSInfoBlock
	 */

		/* ar-related prm_records */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag) ar_prm_record_list;
	/**< ## REQ
	 * List of CM_ARCB_RECORD_TYPE \n
	 * This records will only prm-ed in connect-phase, index should ar-related
	 */

} CM_ARCB_ADD_TYPE;

typedef CM_ARCB_ADD_TYPE * CM_UPPER_ARCB_ADD_PTR_TYPE; /**< The pointer type used for parameters of CM_OPC_CL_DEVICE_ADD */
typedef CM_ARCB_ADD_TYPE const * CM_UPPER_ARCB_ADD_CONST_PTR_TYPE; /**< The cons pointer type used for parameters of CM_OPC_CL_DEVICE_ADD */
/** @} ***********************************************************************/

/*------------------------------------------------------------------------------
// IOCRBlockReq (iodata / iocs / ioap / iocr)
//----------------------------------------------------------------------------*/

/*****************************************************************************/
/* CM_ARCB_IODATA_OBJECT_TYPE                                                */
/**************************************************************************//**
@defgroup CM_ARCB_IODATA_OBJECT_TYPE CM_ARCB_IODATA_OBJECT_TYPE
@ingroup CM_ARCB_TYPE
@{
@details
With the parameter application_offset the cm-user is able to determine the memory area of the IO-data object by adding it to the image offset of the corresponding IOCR. \n
Therefore the parameter frame_offset is obsolete for the CM-user and only needed by CM.
******************************************************************************/
typedef struct cm_arcb_iodata_object_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iodata_object_tag) link;
	/**< Link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_IODATA_OBJECT_TYPE
	 */

	LSA_UINT16 slot_nr;
	/**< Related info
	 */

	LSA_UINT16 subslot_nr;
	/**< Subslot Number
	 */

	LSA_UINT16 frame_offset;
	/**< Frame offset
	 */

	LSA_UINT16 application_offset;
	/**< Application offset
	 */

} CM_ARCB_IODATA_OBJECT_TYPE;

typedef CM_ARCB_IODATA_OBJECT_TYPE * CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_IOCS_TYPE                                                         */
/**************************************************************************//**
@defgroup CM_ARCB_IOCS_TYPE CM_ARCB_IOCS_TYPE
@ingroup CM_ARCB_TYPE
@{
@details
With the parameter application_offset the cm-user is able to determine the memory area of the iocs by adding it to the image offset of the corresponding IOCR. \n
Therefore the parameter frame_offset is obsolete for the CM-user and only needed by CM.
******************************************************************************/
typedef struct cm_arcb_iocs_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocs_tag) link;
	/**< Link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_IOCS_TYPE
	 */

	LSA_UINT16 slot_nr;
	/**< Related info
	 */

	LSA_UINT16		subslot_nr;
	/**< Subslot Number
	 */

	LSA_UINT16		frame_offset;
	/**< Frame offset
	 */

	LSA_UINT16		application_offset;
	/**< Application offset
	 */

} CM_ARCB_IOCS_TYPE;

typedef CM_ARCB_IOCS_TYPE * CM_UPPER_ARCB_IOCS_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_IOAPI_TYPE                                                        */
/**************************************************************************//**
@defgroup CM_ARCB_IOAPI_TYPE CM_ARCB_IOAPI_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
typedef struct cm_arcb_ioapi_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ioapi_tag) link;
	/**< Link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_IOAPI_TYPE
	 */

	LSA_UINT32 api;
	/**< application process identifier
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iodata_object_tag) related_iodata_objects_list;
	/**< List of: CM_ARCB_IODATA_OBJECT_TYPE
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocs_tag) related_iocs_list;
	/**< List of: CM_ARCB_IOCS_TYPE
	 */

} CM_ARCB_IOAPI_TYPE;

typedef CM_ARCB_IOAPI_TYPE * CM_UPPER_ARCB_IOAPI_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_IOCR_TYPE                                                         */
/**************************************************************************//**
@defgroup CM_ARCB_IOCR_TYPE CM_ARCB_IOCR_TYPE
@ingroup CM_ARCB_TYPE
@{
@brief Part of CM_ARCB_ADD_TYPE (pointer)
@details
@see Further details about parameters for RT Frames: [/11/](@ref cm-ref-no-11)

Notes for EDDx:

## CMCL and CMMC

With the help of the function "m_arcb_calc_iocr_buffer_size()" the CM-user can calculate the need size of the KRAM.

The fields subframe_data and subframe_offset gives the CM-user information about the option DFP frame (see overview below).

As a side effect, this helper-function calculate the related_xxx_application_offset on base of partial_length, subframe_data.length and releated_xxx_frame_offset (see offset calculation below).

After allocation of the required KRAM the CM-user must fill the remaining fields required by EDDx.

apdu_status_length and partial_length gives the allocated size of the KRAM.


## Example for the single buffer model

KRAMSize = partial_length + apdu_status_length.

ioXXXOffset  = data_offset + apdu_status_length + related_XXX_application_offset.

## Overview for RTC123

![Overview for RTC123](cm-arcb-overview-for-rtc123.svg)

## Overview for DFP

![Overview for DFP](cm-arcb-overview-for-dfp.svg)
******************************************************************************/
#define CM_IOCR_TYPE_RESERVED_0          0x0000
#define CM_IOCR_TYPE_INPUT               0x0001
#define CM_IOCR_TYPE_OUTPUT              0x0002
#define CM_IOCR_TYPE_MULTICAST_PROVIDER  0x0003
#define CM_IOCR_TYPE_MULTICAST_CONSUMER  0x0004


#define CM_IOCR_PROP_RT_CLASS_MASK    0x0000000FUL /**< Bit 0 - 3: RTClass */
#define CM_IOCR_PROP_RT_CLASS_1       0x00000001UL /**< RT_CLASS_1   / Shall be supported for legacy IO devices by IO controller and IO supervisor. It should not be generated by an IO device. */
#define CM_IOCR_PROP_RT_CLASS_2       0x00000002UL /**< RT_CLASS_2   / Data-RTC-PDU */
#define CM_IOCR_PROP_RT_CLASS_3       0x00000003UL /**< RT_CLASS_3   / Data-RTC-PDU */
#define CM_IOCR_PROP_RT_CLASS_1_UDP   0x00000004UL /**< RT_CLASS_UDP / UDP-RTC-PDU */

#define CM_IOCR_PROP_RESERVED_4_12    0x00001FF0UL /**< Bit  4 - 12: reserved_1, set to zero and test for zero */
#define CM_IOCR_PROP_UNCHECKED_13_23  0x00FFE000UL /**< Bit 13 - 23: reserved_2, set to zero but do not check */
#define CM_IOCR_PROP_RESERVED_24_31   0xFF000000UL /**< Bit 24 - 31: reserved_3, set to zero and test for zero */


#define CM_PNIO_C_SDU_MIN_LENGTH_UDP  12
#define CM_PNIO_C_SDU_MIN_LENGTH_RT   40
#define CM_PNIO_C_SDU_MAX_LENGTH      1440


typedef struct cm_arcb_iocr_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry. Refer to list-macros in cm_list.h
	 * Entry type: CM_ARCB_IOCR_TYPE
	 */

	LSA_UINT16 iocr_type;
	/**< ## REQ
	 * IO CR Type, see defines @ref CM_IOCR_TYPE_RESERVED_0, ...
	 */

	LSA_UINT16 iocr_ref;
	/**< ## REQ
	 * for reference
	 */

	LSA_UINT32 iocr_properties;
	/**< ## REQ
	 * see defines @ref CM_IOCR_PROP_RT_CLASS_MASK, ...
	 */

	LSA_UINT16 frame_id;
	/**< ## REQ
	 * Class 1 & 2 Unicast: don't care \n
	 * Class 3 and Multicasts: The FrameID to be used
	 * ## CNF
	 * class 1 & 2: assigned frame_id, valid for consumer only
	 */

	LSA_UINT16 data_length;
	/**< ## REQ
	 * see CM_PNIO_C_SDU_MIN_LENGTH_RT, etc.
	 * ### From part 6, C_SDU
	 * @par Table "RTC APDU syntax", note a)
	 * In case of RTC-PDU the number of padding octets shall be in the range of 0..40 depending on the DataItem size. \n
	 * In case of UDP-RTC-PDU the number of padding octets shall be in the range of 0..12 depending on the DataItem size.
	 *
	 * @par Table "RTC APDU syntax", note b)
	 * In case of RT_CLASS_3 transportation the number of padding octets is given by the engineering system. \n
	 *
	 * @par Table "RTC substitutions", note a)
	 * The maximum size of a C_SDU or CSF_SDU shall be not larger than 1440 octets.
	 *
	 * ### CM V4.1i1
	 * RT class 3 frames shall not be "short frames" \n
	 * CM_PNIO_C_SDU_MIN_LENGTH_IRT removed (value was 0)
	 *
	 * ### CM_PNIO_C_SDU_MIN_LENGTH_RT/UDP
	 * An ethernet frame should have a minimum length of 64 bytes (legacy of 10MB ethernet). \n
	 * If a switch removes the VLAN tag, the frame should still be at least 64 bytes long.
	@verbatim
	        DST + SRC + VLAN + LT + IP + UDP + FRAMEID + C_SDU + APDUSTATUS + FCS
	RTC3:    6  +  6         +  2            +    2    +   40  +    2+1+1   +  4  = 64
	RTC12:   6  +  6  +   4  +  2            +    2    +   40  +    2+1+1   +  4  = 68
	RToUDP:  6  +  6  +   4  +  2 + 20 +  8  +    2    +   12  +    2+1+1   +  4  = 68
	@endverbatim
	 */

	LSA_UINT16 send_clock;
	/**< ## REQ
	 * The time base is 31,25 microseconds. The value range is from 1 to 128. It is mandatory to support the value 32.
	 */

	LSA_UINT16 reduction_ratio;
	/**< ## REQ
	 * reduction ratio \n
	 * mandatory:1,2,4,8,16,32,128 \n
	 * mandatory-udp:256,512,1024,2048,8196,16384 \n
	 * optional:others < 16385
	 */

	LSA_UINT16 phase;
	/**< REQ
	 * 1 <= phase <= reduction_ratio
	 */

	/* LSA_UINT16		sequence ... removed with V5.3i1 (AP01104502); note: had no effect since AP00542516 and AP00619993 (stable sequence of provider frames) */
	LSA_UINT32 frame_send_offset;
	/**< ## REQ
	 * time base:1 nanosec \n
	 * mandatory:0xFFFFFFFF(best effort) \n
	 * optional:0..0x003D08FF
	 */

	/* LSA_UINT16		watchdog_factor ... removed with V5.3i1 (AP01304706, PNIO-Spec V2.3, Part 6, Rev 12); note: value had to be the same as data_hold_factor */
	LSA_UINT16 data_hold_factor;
	/**< ## REQ
	 * 0x0001 \n
	 * 0x0002 optional \n
	 * 0x0003-0x00FF mandatory \n
	 * 0x0100-0x1E00 optional \n
	 * rest reserved
	 */

	/* LSA_UINT16		iocr_tag_header ... removed with V6.0i1 (TIA 946498) */
	CM_MAC_TYPE iocr_multicast_mac;
	/**< ## REQ
	 * Multicast MAC address
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ioapi_tag) related_ioapi_list;
	/**< ## REQ
	 * List of: CM_ARCB_IOAPI_TYPE
	 */

		/* information for buffer access (req) */

	LSA_UINT32 edd_properties;
	/**< ## REQ
	 * Data for EDDI
	 */

	LSA_UINT32 edd_data_offset;
	/**< ## REQ
	 * Data for EDDx
	 */

	LSA_UINT16 edd_cr_number;
	/**< ## REQ
	 * Data for EDDx, see @ref cm-ref-no-16, see @ref cm-ref-no-8 \n
	 * or Data for EDDS, (see EDDS_IOBuffer_ProviderAlloc() and EDDS_IOBuffer_ConsumerAlloc(), V5.3i1 AP01403254)
	 */

	LSA_UINT8 edd_forward_mode;
	/**< ## REQ
	 * Data for EDDP, see @ref cm-ref-no-16
	 */

	LSA_UINT32 edd_ct_outdatastart;
	/**< ## REQ
	 * Data for EDDP, see @ref cm-ref-no-16
	 */

	LSA_UINT16 apdu_status_length;
	/**< ## REQ
	 * see cm_arcb_calc_iocr_buffer_size()
	 */

	LSA_UINT16 partial_length;
	/**< ## REQ
	 * see cm_arcb_calc_iocr_buffer_size()
	 */

	LSA_UINT16 partial_offset;
	/**< ## REQ
	 * see cm_arcb_calc_iocr_buffer_size()
	 */

	LSA_UINT16 subframe_data;
	/**< ## REQ
	 * see cm_arcb_calc_iocr_buffer_size()
	 */

	LSA_UINT16 subframe_offset;
	/**< ## REQ
	 * see cm_arcb_calc_iocr_buffer_size()
	 */

		/* information for buffer access (cnf) */

	LSA_UINT32 edd_apdustatus_offset;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * The EDD::ApduStatusOffset, valid if consumer \n
	 * For usage by the Consumer-IO-Update Modul (EDD/ERTEC200)
	 */

	LSA_UINT32 edd_cw_offset;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * The EDD::CWOffset, valid if provider \n
	 * For usage by the Provider-IO-Update Modul (EDD/ERTEC200)
	 */

	LSA_UINT32 edd_datastatus_offset;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * The EDD::DataStatusOffset, valid if provider \n
	 * For usage by the Provider-IO-Update Modul (EDD/ERTEC200)
	 */

} CM_ARCB_IOCR_TYPE;

typedef CM_ARCB_IOCR_TYPE * CM_UPPER_ARCB_IOCR_PTR_TYPE;

typedef CM_ARCB_IOCR_TYPE const * CM_UPPER_ARCB_IOCR_CONST_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_IOCR_LOCAL_TYPE                                                   */
/**************************************************************************//**
@defgroup CM_ARCB_IOCR_LOCAL_TYPE CM_ARCB_IOCR_LOCAL_TYPE
@ingroup CM_ARCB_TYPE
@{
@details
There can be an optional IOCR LOCAL entry corresponding to each IOCR. \n
An entry must exist corresponding to each IOCR if an IOCR LOCAL entry exists.

Cross-referencing takes place through the iocr_ref.

The values recorded in the IOCR LOCAL are set locally in EDD (CM-CL and CM-MC).

The values recorded in IOCR are transferred to Connect.req.

The IOCR LOCAL values must correspond to the rules of IOCR. In addition, the following must be applicable:

The Send Clock must be set to the value of the local send_clock before AddDevice.

With AP01267944 CM following constraints has must hold: \n
local.send_clock * local.reduction_ratio <= iocr.send_clock * iocr.reduction_ratio

and additionaly for consumer: \n
local.send_clock * local.reduction_ratio * local.data_hold_factor >= iocr.send_clock * iocr.reduction_ratio * iocr.data_hold_factor
******************************************************************************/
typedef struct cm_arcb_iocr_local_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_local_tag) link;
	/**< Link to next entry, see list-macros in cm_list.h
	 * Entry type CM_ARCB_IOCR_LOCAL_TYPE
	 */

	LSA_UINT16 iocr_ref;
	/**< For reference to iocr
	*/

	LSA_UINT16 send_clock;
	/**< The time base is 31,25 microseconds.
	 * The value range is from 1 to 128.
	 * It is mandatory to support the value 32.
	 */

	LSA_UINT16 reduction_ratio;
	/**< Reduction ratio \n
	 * mandatory:1,2,4,8,16,32,128 \n
	 * mandatory-udp:256,512,1024,2048,8196,16384 \n
	 * optional:others < 16385
	 */

	LSA_UINT16 phase;
	/**< 1 <= phase <= reduction_ratio
	 */

	/* LSA_UINT16		sequence ... removed with V5.3i1, see CM_ARCB_IOCR_TYPE */
	/* LSA_UINT16		watchdog_factor ... removed with V5.3i1, see CM_ARCB_IOCR_TYPE */

	LSA_UINT16 data_hold_factor;
	/**< 0..use data_hold_factor from refered iocr \n
	 * else..use as defined
	 */

	LSA_UINT32 frame_send_offset;
	/**< time base:1 nanosec \n
	 * mandatory:0xFFFFFFFF(best effort) \n
	 * optional:0..0x003D08FF
	 */

} CM_ARCB_IOCR_LOCAL_TYPE;

typedef CM_ARCB_IOCR_LOCAL_TYPE * CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE;
/** @} ***********************************************************************/


/*------------------------------------------------------------------------------
// Record / Submodule / Module / AP
//----------------------------------------------------------------------------*/

/*****************************************************************************/
/* CM_ARCB_RECORD_TYPE                                                       */
/**************************************************************************//**
@defgroup CM_ARCB_RECORD_TYPE CM_ARCB_RECORD_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
typedef struct cm_arcb_record_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag) link;
	/**< ## REQ
	 * Chaining, ink to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_RECORD_TYPE
	 */

	LSA_UINT32 record_index;
	/**< ## REQ
	 * Record index
	 */

	LSA_UINT16 transfer_sequence;
	/**< ## REQ
	 * Transfer sequence for user specific records. \n
	 * Valid for manufacture specific index only (index < 0x8000); \n
	 * 0 .. undefined seqence \n
	 * >0 .. use this sequence
	 */

	LSA_UINT16 record_data_len;
	/**< ## REQ
	 * Length of record_data
	 */

	CM_UPPER_MEM_U8_PTR_TYPE record_data;
	/**< ## REQ
	 * Pointer to a buffer with the record data.\n
	 * CM creates a copy of this data.
	 */

} CM_ARCB_RECORD_TYPE;

typedef CM_ARCB_RECORD_TYPE * CM_UPPER_ARCB_RECORD_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_SUBMODULE_TYPE                                                    */
/**************************************************************************//**
@defgroup CM_ARCB_SUBMODULE_TYPE CM_ARCB_SUBMODULE_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
#define CM_SUB_PROP_TYPE_MASK            0x0003 /**< Bit 0 - 1: SubmoduleProperties.Type */
#define CM_SUB_PROP_TYPE_NO_DATA         0x0000
#define CM_SUB_PROP_TYPE_INPUT           0x0001
#define CM_SUB_PROP_TYPE_OUTPUT          0x0002
#define CM_SUB_PROP_TYPE_INPUT_OUTPUT    0x0003

#define CM_SUB_PROP_SHARED_INP_MASK      0x0004 /**< Bit 2: SubmoduleProperties.SharedInput */
#define CM_SUB_PROP_SHARED_INP_NO        0x0000
#define CM_SUB_PROP_SHARED_INP_YES       0x0004

#define CM_SUB_PROP_REDUCE_INP_LEN_MASK  0x0008 /**< Bit 3: SubmoduleProperties.ReduceInputSubmoduleDataLength */
#define CM_SUB_PROP_REDUCE_INP_LEN_NO    0x0000 /**< Use expected input SubmoduleDataLength for I-CR */
#define CM_SUB_PROP_REDUCE_INP_LEN_YES   0x0008 /**< Reduce input SubmoduleDataLength to zero for I-CR */

#define CM_SUB_PROP_REDUCE_OUT_LEN_MASK  0x0010 /**< Bit 4: SubmoduleProperties.ReduceOutputSubmoduleDataLength */
#define CM_SUB_PROP_REDUCE_OUT_LEN_NO    0x0000 /**< Use expected output SubmoduleDatalength for O-CR */
#define CM_SUB_PROP_REDUCE_OUT_LEN_YES   0x0010 /**< Reduce output SubmoduleDatalength to zero for O-CR */

#define CM_SUB_PROP_DISCARD_IOXS_MASK    0x0020 /**< Bit 5: SubmoduleProperties.DiscardIOXS */
#define CM_SUB_PROP_DISCARD_IOXS_NO      0x0000 /**< Use IOXS-length */
#define CM_SUB_PROP_DISCARD_IOXS_YES     0x0020 /**< Treat IOXS-Length as 0 */

#define CM_SUB_PROP_UNCHECKED_6_7        0x00C0 /**< Bit 6 - 7: reserved_1, set to zero but do not check */
#define CM_SUB_PROP_RESERVED_8_15        0xFF00 /**< Bit 8 - 15: reserved_2, set to zero and test for zero */


#define CM_SUB_DATA_DESCR_TYPE_MASK        0x0003 /**< Bit 0 - 1: DataDescription.Type */
#define CM_SUB_DATA_DESCR_TYPE_RESERVED_0  0x0000
#define CM_SUB_DATA_DESCR_TYPE_INPUT       0x0001
#define CM_SUB_DATA_DESCR_TYPE_OUTPUT      0x0002
#define CM_SUB_DATA_DESCR_TYPE_RESERVED_3  0x0003

#define CM_SUB_DATA_DESCR_RESERVED_MASK    0xFFFC /* Bit 2-15: DataDescription.reserved (set to zero) */


#define CM_SUB_DATA_LENGTH_MAX  (CM_PNIO_C_SDU_MAX_LENGTH - 1) /**< The range shall be between 0 and 1439 */

typedef struct cm_arcb_submodule_descr_tag {
	LSA_UINT16 data_description;
	/**< See defines @ref CM_SUB_DATA_DESCR_TYPE_MASK
	 */

	LSA_UINT16 data_length;
	/**< SubmoduleDataLength, see CM_SUB_DATA_LENGTH_MAX
	 */

	LSA_UINT8 iops_length;
	/**< Length of IOPS 1..n
	 */

	LSA_UINT8 iocs_length;
	/**< Length of IOCS 1..m
	 */

} CM_ARCB_SUBMODULE_DESCR_TYPE;


typedef struct cm_arcb_submodule_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_submodule_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_SUBMODULE_TYPE
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * Subslot Number, ascending order
	 */

	LSA_UINT16 sub_properties;
	/**< ## REQ
	 * See defines @ref CM_SUB_PROP_TYPE_MASK, ...
	 * Refer to [/11/](@ref cm-ref-no-11).
	 */

	LSA_UINT32 sub_ident;
	/**< ## REQ
	 * Submodule ident number
	 */

	/* submodule data description */
	CM_ARCB_SUBMODULE_DESCR_TYPE descr[2];
	/**< ## REQ
	 * Submodule data description \n
	 * Index: 0..data description for "input types" \n
	 * Index: 1..data description for "output types"
	 */

	union {
		struct {
			LSA_UINT32 nothing_so_far; /**< do not evaluate, AP00565264 */
		} pd;
		struct {
			CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag) prm_record_list;
			/**< ## REQ
			 * List of PRM-records (CM_ARCB_RECORD_TYPE) for this submodule \n
			 * The records are transferred in the same sequence to IO device.
			 */

		} cl; /**< cm-client only */
	} u;
} CM_ARCB_SUBMODULE_TYPE;

typedef CM_ARCB_SUBMODULE_TYPE * CM_UPPER_ARCB_SUBMODULE_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_MODULE_TYPE                                                       */
/**************************************************************************//**
@defgroup CM_ARCB_MODULE_TYPE CM_ARCB_MODULE_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
#define CM_MOD_PROP_UNCHECKED_0_7  0x00FF /**< Bit 0 -  7: reserved_1, set to zero but do not check */
#define CM_MOD_PROP_RESERVED_8_15  0xFF00 /**< Bit 8 - 15: reserved_2, set to zero and test for zero */

typedef struct cm_arcb_module_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_module_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_MODULE_TYPE
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * Slot Number
	 */

	LSA_UINT16 mod_properties;
	/**< ## REQ
	 * Module properties
	 */

	LSA_UINT32 mod_ident;
	/**< ## REQ
	 * Module Ident Number
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_submodule_tag) submodule_list;
	/**< ## REQ
	 * List of submodules of type CM_ARCB_SUBMODULE_TYPE
	 */

} CM_ARCB_MODULE_TYPE;

typedef CM_ARCB_MODULE_TYPE * CM_UPPER_ARCB_MODULE_PTR_TYPE;

typedef CM_ARCB_MODULE_TYPE const * CM_UPPER_ARCB_MODULE_CONST_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_ARCB_AP_TYPE                                                           */
/**************************************************************************//**
@defgroup CM_ARCB_AP_TYPE CM_ARCB_AP_TYPE
@ingroup CM_ARCB_TYPE
@{
******************************************************************************/
typedef struct cm_arcb_ap_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ap_tag) link;
	/**< ## REQ
	 * Chaining, link to next entry, see list-macros in cm_list.h \n
	 * Entry type: CM_ARCB_AP_TYPE
	 */

	LSA_UINT32 api;
	/**< ## REQ
	 * Application process identifier
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_module_tag) module_list;
	/**< ## REQ
	 * List of modules of an AP, type CM_ARCB_MODULE_TYPE
	 */

} CM_ARCB_AP_TYPE;

typedef CM_ARCB_AP_TYPE * CM_UPPER_ARCB_AP_PTR_TYPE;

typedef CM_ARCB_AP_TYPE const * CM_UPPER_ARCB_AP_CONST_PTR_TYPE;
/** @} ***********************************************************************/


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ARCB_H */
