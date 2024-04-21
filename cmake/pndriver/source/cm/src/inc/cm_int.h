#ifndef CM_INT_H
#define CM_INT_H

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
/*  F i l e               &F: cm_int.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#ifndef CM_MODULE_ID
# error "CM_MODULE_ID not defined!"
#endif

/*----------------------------------------------------------------------------*/

#include "cm_inc.h"

#include "cm_pdu.h" /* PDUs are internal */

#include "cm_md5.h" /* md5 functions */

/*------------------------------------------------------------------------------
// stringify
//----------------------------------------------------------------------------*/

#define CM_STRINGIFY_(n)  # n
#define CM_STRINGIFY(n)   CM_STRINGIFY_(n) /* make it a string */

/*------------------------------------------------------------------------------
// pragma message
// intended usage: #pragma CM_MESSAGE(...)
//----------------------------------------------------------------------------*/

#ifdef _MSC_VER /* Microsoft, pragma-args are expanded */

# ifdef CM_MESSAGE
#  error "who defined CM_MESSAGE?"
# else
#  ifdef CM_CFG_MESSAGE /* for developers only */
#    define CM_MESSAGE(str) message( __FILE__ "(" CM_STRINGIFY(__LINE__) ") : " str)
#  endif
# endif /* CM_MESSAGE */

#endif

/*------------------------------------------------------------------------------
// logging
//----------------------------------------------------------------------------*/

#ifdef CM_LOG_FCT
# error "CM_LOG_FCT is obsolete (TIA 1649473, V6.1i1.4). please remove the define from cm_cfg.h"
#endif

/*------------------------------------------------------------------------------
// assertions
//----------------------------------------------------------------------------*/

/*
 * #ifndef ... moved to cm_pls.h
 */

/*------------------------------------------------------------------------------
// pointer compare macros
//----------------------------------------------------------------------------*/

#	define cm_is_null(ptr_)			(LSA_HOST_PTR_ARE_EQUAL(CM_TEST_POINTER(ptr_), LSA_NULL))
#	define cm_is_not_null(ptr_)		(! LSA_HOST_PTR_ARE_EQUAL(CM_TEST_POINTER(ptr_), LSA_NULL))

/*------------------------------------------------------------------------------
// helper for casting and warning-level
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_CASTS

#	define CM_CAST_ARGR_IOCR_PTR(val_)		((CM_AR_GRAPH_IO_CR_PTR_TYPE)(val_))
#	define CM_CAST_ARGR_ALARMCR_PTR(val_)	((CM_AR_GRAPH_ALARM_CR_PTR_TYPE)(val_))

#	define CM_CAST_UPPER_MEM_PTR_PTR(val_)	((CM_UPPER_MEM_PTR_TYPE *)(val_))
#	define CM_CAST_COMMON_MEM_U8_PTR(val_)	((CM_COMMON_MEM_U8_PTR_TYPE                )(val_))

#	define CM_CAST_LOCAL_MEM_PTR(val_)		((CM_LOCAL_MEM_PTR_TYPE)(val_))
#	define CM_CAST_LOCAL_MEM_PTR_PTR(val_)	((CM_LOCAL_MEM_PTR_TYPE *)(val_))

#	define CM_CAST_EDD_MEM_U8_PTR(val_)		((EDD_UPPER_MEM_U8_PTR_TYPE                    )(val_))

#	define CM_CAST_GSY_MEM_U8_PTR(val_)		((GSY_UPPER_BYTE_PTR_TYPE                      )(val_))

#	define CM_CAST_OHA_MEM_U8_PTR(val_)		((OHA_UPPER_MEM_U8_PTR_TYPE                    )(val_))

#	define CM_CAST_MRP_MEM_U8_PTR(val_)		((MRP_UPPER_MEM_UINT8_PTR_TYPE                 )(val_))

#	define CM_CAST_POF_MEM_U8_PTR(val_)		((POF_UPPER_MEM_U8_PTR_TYPE                    )(val_))

#else

#	define CM_CAST_ARGR_IOCR_PTR(val_)		(val_)
#	define CM_CAST_ARGR_ALARMCR_PTR(val_)	(val_)

#	define CM_CAST_UPPER_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_COMMON_MEM_U8_PTR(val_)	(val_)

#	define CM_CAST_LOCAL_MEM_PTR(val_)		(val_)
#	define CM_CAST_LOCAL_MEM_PTR_PTR(val_)	(val_)

#	define CM_CAST_EDD_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_GSY_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_OHA_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_MRP_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_POF_MEM_U8_PTR(val_)		(val_)

#endif

#if CM_CFG_USE_CASTS || CM_DEBUG  || defined(_lint)

#	define CM_CAST_U8(val_)			((LSA_UINT8)(val_))
#	define CM_CAST_U16(val_)		((LSA_UINT16)(val_))
#	define CM_CAST_U32(val_)		((LSA_UINT32)(val_))
#	define CM_CAST_INT(val_)		((LSA_INT)(val_))
#	define CM_CAST_INT32(val_)		((LSA_INT32)(val_))

#else

#	define CM_CAST_U8(val_)			(val_)
#	define CM_CAST_U16(val_)		(val_)
#	define CM_CAST_U32(val_)		(val_)
#	define CM_CAST_INT(val_)		(val_)
#	define CM_CAST_INT32(val_)		(val_)

#endif


/*------------------------------------------------------------------------------
// get the containing struct
// (analog to window's CONTAINING_RECORD)
//----------------------------------------------------------------------------*/

#define CM_CONTAINING_STRUCT(field_ptr, struct_ptr_type, field) \
	((struct_ptr_type) \
		/*lint -e(413) */ \
		((CM_COMMON_MEM_U8_PTR_TYPE)(field_ptr) - (LSA_INT)( &((struct_ptr_type)0)->field )))

/*------------------------------------------------------------------------------
// acpid valid check
//----------------------------------------------------------------------------*/

#define CM_ACPID_ISVALID(iocr) ((iocr->cm.is_consumer && iocr->cm.acp_id != EDD_CONS_ID_INVALID) || iocr->cm.acp_id != EDD_PROV_ID_INVALID)
#define CM_ACPID_SETINVALID(iocr) (iocr->cm.acp_id = iocr->cm.is_consumer ? EDD_CONS_ID_INVALID : EDD_PROV_ID_INVALID);

/*------------------------------------------------------------------------------
// edd frame id
//----------------------------------------------------------------------------*/

#define CM_EDD_INVALID_FRAME_ID				0xFFFF /* invalid frame_id */

/*------------------------------------------------------------------------------
// edd handle lower
//----------------------------------------------------------------------------*/

#define CM_EDD_SET_HANDLES(pRQB, value) {\
	EDD_RQB_SET_HANDLE (pRQB, 0);\
	EDD_RQB_SET_HANDLE_LOWER (pRQB, value);\
	}

/*------------------------------------------------------------------------------
// count of state indications, see EDD_SRV_SRT_IND_PROVIDE
//----------------------------------------------------------------------------*/

#define CM_EDD_STATE_IND_RESOURCE_COUNT			16 /* derived from max allowed reduction ratios for rtc3 */

/*------------------------------------------------------------------------------
// Station_Failure <-> Station_Ok
//----------------------------------------------------------------------------*/

/* note: using undefined "mode" bits, see EDD_PROV_CONTROL_MODE_STATUS */
#define CM_ARCQ_PROVIDER_SET_STATION_FAILURE	0x0100
#define CM_ARCQ_PROVIDER_SET_STATION_OK			0x0200
#define CM_ARCQ_PROVIDER_SET_STATION_MASK \
	( CM_ARCQ_PROVIDER_SET_STATION_FAILURE	| CM_ARCQ_PROVIDER_SET_STATION_OK )

/*------------------------------------------------------------------------------
// groupid definitions
//----------------------------------------------------------------------------*/

#if EDD_CFG_CSRT_MAX_PROVIDER_GROUP < 3
# error "check EDD_CFG_CSRT_MAX_PROVIDER_GROUP"
#endif

#define CM_PROVIDER_GROUP_IOC 0 /* 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP */
#define CM_PROVIDER_GROUP_IOD 1 /* 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP */
#define CM_PROVIDER_GROUP_IOM 2 /* 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP */

/*------------------------------------------------------------------------------
// "Remote_Application_Timeout" See IEC 61784-2
//----------------------------------------------------------------------------*/

#define CM_REMOTE_APPLICATION_TIMEOUT_1SEC 300

/*------------------------------------------------------------------------------
// oneshot timer -- implemented in file cm_tim.c
// (copy/paste from CLRPC)
//----------------------------------------------------------------------------*/

LSA_VOID
cm_timeout_synchronized(
	CM_UPPER_RQB_PTR_TYPE  rb
);

typedef struct cm_oneshot_tag * CM_ONESHOT_PTR_TYPE;

typedef struct cm_oneshot_tag const * CM_ONESHOT_CONST_PTR_TYPE;

typedef LSA_VOID LSA_FCT_PTR(/*ATTR*/,
	CM_ONESHOT_CALLBACK)(
		CM_ONESHOT_PTR_TYPE oneshot,
		LSA_VOID_PTR_TYPE user_ptr
	);

typedef struct cm_oneshot_tag {
	CM_LIST_ENTRY_TYPE dlink; /* must be first in struct */
	CM_ONESHOT_CALLBACK callback;
	LSA_VOID_PTR_TYPE user_ptr;
	LSA_UINT32 ticks;
	LSA_UINT16 running;
} CM_ONESHOT_TYPE;

LSA_VOID
cm_oneshot_sys_init(
	LSA_UINT16 time_base,
	LSA_UINT16 time_factor
);

LSA_VOID cm_oneshot_sys_undo_init(
	LSA_VOID
);

LSA_VOID
cm_oneshot_init(
	CM_ONESHOT_PTR_TYPE oneshot,
	CM_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
);

LSA_VOID
cm_oneshot_trigger(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
);

LSA_UINT16
cm_oneshot_stop(
	CM_ONESHOT_PTR_TYPE oneshot
);

LSA_UINT16
cm_oneshot_is_running(
	CM_ONESHOT_CONST_PTR_TYPE oneshot
);


/*------------------------------------------------------------------------------
// Range, see cm_cmn.c
//----------------------------------------------------------------------------*/

enum cm_range_id_type {
	CM_RANGE_ID_ALARM_REF = 0,
	CM_RANGE_ID_CLASS_1_UNICAST,
	CM_RANGE_ID_CLASS_1_MULTICAST,
	CM_RANGE_ID_CLASS_2_UNICAST,
	CM_RANGE_ID_CLASS_2_MULTICAST,
	CM_RANGE_ID_MAX
};

typedef struct cm_range_tag {
	LSA_UINT16				base;
	LSA_UINT16				count;
	CM_COMMON_MEM_U8_PTR_TYPE arr; /* allocated bit-array */
} CM_RANGE_TYPE;

typedef CM_RANGE_TYPE * CM_RANGE_PTR_TYPE;

typedef CM_RANGE_TYPE const * CM_RANGE_CONST_PTR_TYPE;

LSA_VOID
cm_range_init (
	CM_RANGE_PTR_TYPE range
);

LSA_VOID
cm_range_undo_init (
	CM_RANGE_CONST_PTR_TYPE range
);

LSA_BOOL
cm_range_alloc (
	CM_RANGE_PTR_TYPE range,
	LSA_UINT16	base,
	LSA_UINT16	count
);

LSA_BOOL
cm_range_alloc_id  (
	CM_RANGE_CONST_PTR_TYPE	range,
	LSA_UINT16 * id_ptr
);

LSA_BOOL
cm_range_reserve_id  (
	CM_RANGE_CONST_PTR_TYPE	range,
	LSA_UINT16  id
);

LSA_VOID
cm_range_free_id  (
	CM_RANGE_CONST_PTR_TYPE	range,
	LSA_UINT16			id
);


/*------------------------------------------------------------------------------
// helper function: convert a format_0 sub_state to an format_1 sub_state
//----------------------------------------------------------------------------*/

LSA_UINT16
cm_sub_state_to_format_1 (
	LSA_UINT16  sub_state	/* in: sub_state in format_0(return converted) or format_1(simple return) */
	);


/*------------------------------------------------------------------------------
// null user_id and NIL UUID
//----------------------------------------------------------------------------*/

extern const LSA_USER_ID_TYPE  cm_null_user_id; /* defined in cm_sys.c */


/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

typedef struct cm_channel_tag * CM_CHANNEL_PTR_TYPE;

typedef struct cm_channel_tag const * CM_CHANNEL_CONST_PTR_TYPE;

/*------------------------------------------------------------------------------
// helper function: alloc upper rqb for internal use
//----------------------------------------------------------------------------*/

CM_UPPER_RQB_PTR_TYPE
cm_upper_alloc_rqb (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

LSA_VOID
cm_upper_free_rqb (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  upper
);


/*------------------------------------------------------------------------------
// helper function: record index handling
//----------------------------------------------------------------------------*/

#define CM_VENDOR_ID_DONTCARE 0x0000 /* see where used (pdev iterators) */
#define CM_VENDOR_ID_SIEMENS  0x002A /* see AP00579173 */

enum cm_record_index_classify_enum {

	CM_REC_IDX_CMPD_MASK      = 0x0000000F, /* see also cm_sub_comp_id_enum */
	CM_REC_IDX_CMPD_NONE      = 0x00000000,
	CM_REC_IDX_CMPD_EDD       = 0x00000001,
#if CM_CFG_USE_MRP
	CM_REC_IDX_CMPD_MRP       = 0x00000002,
#endif
#if CM_CFG_USE_GSY
	CM_REC_IDX_CMPD_GSY       = 0x00000003,
#endif
	CM_REC_IDX_CMPD_0x04      = 0x00000004, /* defunct, was GSY_TIME */
#if CM_CFG_USE_POF
	CM_REC_IDX_CMPD_POF       = 0x00000005,
#endif
	CM_REC_IDX_CMPD_OHA       = 0x00000006,
	CM_REC_IDX_CMPD_BBUF      = 0x00000007, /* usage see CM_OPC_SV_AR_PDEVPRM_READ/WRITE (TIA 1193013) */
	CM_REC_IDX_CMPD_PD        = 0x00000008,

	CM_REC_IDX_INTERFACE = 0x00000010, /* the index addresses an interface submodule */
	CM_REC_IDX_PORT      = 0x00000020, /* the index addresses a port submodule */

	CM_REC_IDX_MASK    = 0x00003F00,
	CM_REC_IDX_0000    = 0x00000100,
	CM_REC_IDX_8000    = 0x00000200,
	CM_REC_IDX_C000    = 0x00000400,
	CM_REC_IDX_E000    = 0x00000800,
	CM_REC_IDX_F000    = 0x00001000,
	CM_REC_IDX_F800    = 0x00002000,

	CM_REC_IDX_READ        = 0x00010000, /* note: reserved := neither readable nor writeable */
	CM_REC_IDX_WRITE       = 0x00020000, /* note: reserved := neither readable nor writeable */
	CM_REC_IDX_READ_LOCAL  = 0x00040000, /* only the pdev-user is allowed to read */
	CM_REC_IDX_WRITE_LOCAL = 0x00080000, /* only the pdev-user is allowed to write */

	CM_REC_IDX_USER     = 0x00100000, /* handled by cm-user */
	CM_REC_IDX_PDEV     = 0x00200000, /* handled by cm-pd */
	CM_REC_IDX_NORM     = 0x00400000, /* handled by cm-sv or cm-pd (diagnosis) */
	CM_REC_IDX_DIAG     = 0x00800000, /* diagnosis-part of normative */
	CM_REC_IDX_SIMATIC  = 0x01000000, /* simatic-indices (0xBxxx) */
	CM_REC_IDX_NONPRM   = 0x02000000, /* these indices are "out of band" and must not be stored remanent */
	CM_REC_IDX_MKREMA   = 0x04000000, /* index is NONPRM but "must" stored remanent */
	CM_REC_IDX_IM1234   = 0x08000000, /* I&M1-4 */
	CM_REC_IDX_MIXIN    = 0x10000000  /* mixin records: locally written records kept for merge with remote parameterization */
};

LSA_UINT32
cm_record_index_classify (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT32 api,
	LSA_UINT32 index32,
	LSA_UINT16 vendor_id
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_record_marshal_diag_yes(
	LSA_UINT16 index,
	LSA_BOOL is_generic, /* false: channel or ext-channel lists, true: generic-list */
	LSA_UINT16 prop
);


/*------------------------------------------------------------------------------
// helper functions: miscellaneous
//----------------------------------------------------------------------------*/

LSA_BOOL
cm_is_power_of_2 (
	LSA_UINT16  var
);

/*------------------------------------------------------------------------------
// physical device things
//----------------------------------------------------------------------------*/

/* === derived typedef (AP01034411) ==== */

#if !defined(EDD_CFG_MAX_PORT_CNT)
#error "EDD_CFG_MAX_PORT_CNT not defined"

#elif EDD_CFG_MAX_PORT_CNT < 1 || EDD_CFG_MAX_PORT_CNT > 255
#error "EDD_CFG_MAX_PORT_CNT not within 1..255"

#elif EDD_CFG_MAX_PORT_CNT <= 31
typedef LSA_UINT32  CM_PORT_UINT; /* 32 bits = 1 interface and up to 31 ports */

#elif  EDD_CFG_MAX_PORT_CNT <= 63
typedef LSA_UINT64  CM_PORT_UINT; /* 64 bits = 1 interface and up to 63 ports */

#else
#error "EDD_CFG_MAX_PORT_CNT >= 64 not supported"
#endif


#include "cm_ipd.h"

#include "cm_prm.h" /* the prm-interface of the subcomponents */


/*------------------------------------------------------------------------------
// AR-Graph things
//----------------------------------------------------------------------------*/

#include "cm_argr.h"


/*------------------------------------------------------------------------------
// rpc things
//----------------------------------------------------------------------------*/

#include "cm_rpc.h"


/*------------------------------------------------------------------------------
// client things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS
#include "cm_icl.h"
#endif


/*------------------------------------------------------------------------------
// server things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS
#include "cm_isv.h"
#include "cm_isv_sr.h"
#endif


/*------------------------------------------------------------------------------
// multicast things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_MULTICAST_CHANNELS
#include "cm_imc.h"
#endif


/*------------------------------------------------------------------------------
// channel states, -type and -table
//----------------------------------------------------------------------------*/

enum cm_channel_state_enum { /* state machine CM-CH */
	CM_CH_FREE = 0,
	CM_CH_ALLOCATED,

	/*
	 * the state machine CM-CH-<ANY>
	 */

	CM_CH_OPEN,
	CM_CH_INFO,
	CM_CH_READY, /* ready for create/delete or close channel */

	/*
	 * the "in-lined" state machine CM-PD
	 */

	CM_PD_STARTING, /* RPC:EPM_START, OHA:SET_IDENT */
	CM_PD_READY,
	CM_PD_REMAPPING,
	CM_PD_DELETING, /* RPC:EPM_STOP */

	/*
	 * the "in-lined" state machine CM-CL
	 */

#if CM_CFG_MAX_CLIENTS
	CM_CL_GLOBAL_CTRL,
	CM_CL_REG_IFC,
	CM_CL_READY,
	CM_CL_UNREGISTER,
#endif

	/*
	 * the "in-lined" state machine CM-SV
	 */

#if CM_CFG_MAX_SERVERS
	CM_SV_CREATING,
	CM_SV_CREATED,
	CM_SV_DELETING,
#endif

	/*
	 * the "in-lined" state machine CM-MC
	 */

#if CM_CFG_MAX_MULTICAST_CHANNELS
	CM_MC_GLOBAL_CTRL,
	CM_MC_CREATED,
	CM_MC_DELETING,
#endif

	/*
	 * the state machine CM-CH-<ANY>
	 */

	CM_CH_CLOSE
};

struct cm_edd_port_status_tag {
	LSA_UINT8  status;
	LSA_UINT8  speed;
	LSA_UINT8  mode;
	LSA_UINT8  irt_port_status;
	LSA_UINT8  autoneg;
};

typedef struct cm_edd_port_status_tag * CM_EDD_PORT_STATUS_PTR_TYPE;

struct cm_edd_channel_params_tag { /* only a part of EDD_RQB_GET_PARAMS_TYPE */
    LSA_UINT16                           InterfaceID;
    LSA_UINT32                           HardwareType;
    EDD_MAC_ADR_TYPE                     MACAddr;
    LSA_UINT16                           MaxPortCnt;
    LSA_UINT16                           CycleBaseFactor;
	LSA_UINT8                            MRPSupportedRole;
    LSA_UINT16                           ConsumerCntClass3; /* note: ConsumerFrameIDBaseClass3 is const */
    LSA_UINT16                           ConsumerFrameIDBaseClass2;
    LSA_UINT16                           ConsumerCntClass2;
    LSA_UINT16                           ConsumerFrameIDBaseClass1;
    LSA_UINT16                           ConsumerCntClass1;
    LSA_BOOL                             ConsumerCntClass12Combined;
    LSA_UINT16                           ProviderCnt;
    LSA_UINT16                           ProviderCntClass3;
};

typedef struct cm_edd_channel_params_tag CM_EDD_CHANNEL_PARAMS_TYPE, * CM_EDD_CHANNEL_PARAMS_PTR_TYPE;

typedef struct cm_channel_tag {

	LSA_UINT16			state;			/* see cm_channel_state_enum */
	LSA_HANDLE_TYPE		my_handle;		/* my own handle */
	LSA_HANDLE_TYPE		handle_upper;	/* the handle of the user */
	CM_UPPER_CALLBACK_FCT_PTR_TYPE callback; /* the callback-function to the user */
	LSA_SYS_PTR_TYPE	sysptr;			/* the 'hint' for the system adaption */
	CM_DETAIL_PTR_TYPE	detailptr;		/* pointer to channel-details */
	LSA_UINT8			path_type;		/* copy of channel->detailptr->path_type (optimization, TIA 1678036) */
	LSA_UINT32			trace_idx;		/* for "LSA index trace" */

	/***/

	union cm_channel_usr_tag {

		struct {
			CM_UPPER_RQB_PTR_TYPE curr; /* current working upper request block for open / close channel */
		} ch;

		struct {
			CM_EDD_PORT_STATUS_PTR_TYPE  ports; /* array[ 0: auto-port / port_nr ] */
		} edd; /* edd things */

#if CM_CFG_USE_GSY
		struct {
			LSA_BOOL do_sync_clock;
			LSA_BOOL subdomain;
			LSA_BOOL is_sync;
			LSA_BOOL is_rate_valid;
			LSA_UINT8 rcv_sync_prio; /* MasterPriority1 from SyncFrame (0 at active master) */
			LSA_UINT8 local_prio;    /* MasterPriority1 from SyncRecord (0 at slave) with ActiveBit set at active Master after Startup */
			LSA_UINT8 pll_state;    /* see cm_pd_ppl_state_enum */
			GSY_MAC_ADR_TYPE master_mac_addr;
		} gsy; /* gsy things */
#endif

		CM_PD_CHANNEL_TYPE pd; /* physical device things */

#if CM_CFG_MAX_CLIENTS
		CM_CL_CHANNEL_TYPE cl; /* client things */
#endif

#if CM_CFG_MAX_SERVERS
		CM_SV_CHANNEL_TYPE sv; /* server things */
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
		CM_MC_CHANNEL_TYPE mc; /* multicast things */
#endif

		struct {
			LSA_UINT8  name_of_station[OHA_MAX_STATION_NAME_LEN];
			LSA_UINT16 name_of_station_length;

			OHA_IP_SUITE_TYPE	IPSuite;                /* current IP suite of the stack */
			LSA_BOOL			is_IPAddr_null;         /* a shortcut: LSA_TRUE if IPSuite.IpAddr == 0.0.0.0 (NetMask and Gateway are not considered) */
			LSA_BOOL			is_IPSuite_emergency;   /* LSA_TRUE if the current IP suite is the "emergency" IP suite */
			LSA_BOOL			is_IPSuite_conflicting; /* LSA_TRUE if the current IP suite is conflicting with another station */
		} oha;

	} usr;

	/***/

	struct cm_channel_res_tag {

		LSA_UINT16 provider_cnt[2]; /* 0=class1_2, 1=class3 */

		CM_RANGE_TYPE id_range[CM_RANGE_ID_MAX]; /* id range for frame_id(class 1..3) and alarm_ref */

		LSA_UINT16 consumer_cnt[2]; /* 0=class1_2, 1=class3, see acp.info.is_combined */

	} res;

	/***/

	struct cm_channel_acp_tag {

		LSA_UINT16 max_alarm_data_length;

	} acp;

	/***/

	struct { /* each channel has 0 or 1 lower-layer */

		LSA_HANDLE_TYPE handle;
		EDD_HANDLE_LOWER_TYPE edd_handle;

		union /* see open/close/prm */
		{
			CM_EDD_LOWER_RQB_PTR_TYPE  edd;
			CM_OHA_LOWER_RQB_PTR_TYPE  oha;
#if CM_CFG_USE_GSY
			CM_GSY_LOWER_RQB_PTR_TYPE  gsy;
#endif
#if CM_CFG_USE_MRP
			CM_MRP_LOWER_RQB_PTR_TYPE  mrp;
#endif
#if CM_CFG_USE_POF
			CM_POF_LOWER_RQB_PTR_TYPE  pof;
#endif
		} prealloc;

		union {

			CM_EDD_CHANNEL_PARAMS_TYPE edd; /* getter: cm_edd_channel_params() */

			struct { /* only a part of ACP_CHANNEL_INFO_TYPE */
				LSA_UINT16	alarm_ref_base;
				LSA_UINT16	alarm_nr_of_ref;
			} acp;

		} info;

		/* each channel has >= 0 associated lower-layers (system channels) */

		CM_CHANNEL_PTR_TYPE channel[CM_PATH_TYPE_ANY_MAX]; /* not: CM_PATH_TYPE_SYS_MAX */

	} lower;

} CM_CHANNEL_TYPE;

/* the channel table */

extern CM_CHANNEL_TYPE  cm_channels[CM_MAX_CHANNELS]; /* defined in cm_usr.c */

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

LSA_VOID
cm_channels_sys_init(
	LSA_VOID
);

LSA_VOID
cm_channels_sys_undo_init(
	LSA_VOID
);

CM_CHANNEL_PTR_TYPE
cm_channel_from_handle(
	LSA_HANDLE_TYPE handle
);

LSA_VOID
cm_callback(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 response,
	CM_UPPER_RQB_PTR_TYPE *upper_ptr
);

LSA_VOID
cm_channel_done(
	CM_CHANNEL_PTR_TYPE channel, LSA_UINT32 result
);

/*----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_channel_find_path_type(
	LSA_UINT8  nic_id,
	LSA_UINT8  path_type
);

/*----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_peer_channel_get(
	CM_CHANNEL_PTR_TYPE channel, /* PD, CL, MC or SV */
	LSA_UINT8 peer_path_type
		/* PD: CM_PATH_TYPE_CLIENT, CM_PATH_TYPE_MULTICAST or CM_PATH_TYPE_SERVER
		 * CL: CM_PATH_TYPE_PD
		 * MC: CM_PATH_TYPE_PD
		 * SV: CM_PATH_TYPE_PD
		 */
);

/*------------------------------------------------------------------------------
// Resources, see cm_res.c
//----------------------------------------------------------------------------*/

LSA_VOID
cm_res_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID
cm_res_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
);

LSA_BOOL
cm_res_frame_alloc (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 * frame_id_ptr /* consumer only */
);

LSA_VOID
cm_res_frame_free (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 frame_id /* consumer only */
);

/*===========================================================================*/
/*===========================================================================*/

CM_ACP_LOWER_RQB_PTR_TYPE
cm_acp_alloc_rqb  (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

LSA_VOID
cm_acp_free_rqb (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE lower_rqb_ptr
);

LSA_VOID
cm_acp_channel_open(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID
cm_acp_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID
cm_acp_channel_close(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID
cm_acp_request_lower(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_VOID_PTR_TYPE  user_id,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
);

/*===========================================================================*/

LSA_VOID
cm_acp_frame_indication(
	EDD_UPPER_CSRT_IND_DATA_PTR_TYPE  data,
	LSA_UINT8 * debug_info
);

/*===========================================================================*/

LSA_BOOL
cm_link_status_is_up(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL
cm_link_status_is_100Mbps_FDX_or_more(
	CM_CHANNEL_PTR_TYPE channel
);

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_acp_frame_add_done (
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_acp_frame_control(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT16  acp_mode,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_acp_frame_control_done(
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_acp_frame_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_acp_frame_remove_done (
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_UINT16
cm_acp_frame_get_apdu_status (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_VOID_PTR_TYPE  user_id
);

LSA_VOID
cm_acp_frame_get_apdu_status_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

/*===========================================================================*/

LSA_VOID
cm_acp_alarm_add (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_add_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_remove (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID
cm_ar_com_ready (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID
cm_ar_com_down_debug_data_set (
	CM_AR_GRAPH_PTR_TYPE  argr,
	ACP_UPPER_ERROR_INFO_DATA_PTR_TYPE  debug_data
);

LSA_VOID
cm_ar_com_down_debug_data_fmt (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  module_id,
	LSA_UINT32  line_nr,
	LSA_UINT8  reason,
	LSA_UINT32  data1
);

LSA_VOID
cm_ar_com_down2(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  reason,
	LSA_UINT16  cycle_counter
);

LSA_VOID
cm_ar_com_down(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  reason
);

LSA_VOID
cm_ar_com_idle (
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*===========================================================================*/

enum cm_arcq_opcode_enum {
	CM_ARCQ_OPC_IDLE			=  0,

	CM_ARCQ_OPC_FRAMES_ADD			=  1, /* add provider + consumer / unicast and multicast */
	CM_ARCQ_OPC_FRAMES_REMOVE		=  2, /* remove provider + consumer / unicast and multicast */
	CM_ARCQ_OPC_FRAMES_PASSIVATE	=  3, /* passivate provider + consumer / unicast and multicast */

	CM_ARCQ_OPC_FRAMES_RTF			=  4, /* only CMCL and CMMC, prepare added frames for reset-to-factory */

	CM_ARCQ_OPC_PROV_ACTIVATE			= 11, /* + config-autostop */
	CM_ARCQ_OPC_PROV_ACTIVATE_AUX		= 12,
	CM_ARCQ_OPC_PROV_PASSIVATE_AUX		= 13,
	CM_ARCQ_OPC_PROV_DATASTATUS_SET		= 14,
	CM_ARCQ_OPC_PROV_IOPARAMS_SET		= 15,

	CM_ARCQ_OPC_CONS_ACTIVATE			= 21,
	CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK	= 22, /* + set-to-unknown */
	CM_ARCQ_OPC_CONS_SET2UNK			= 23,
	CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS		= 24, /* + enable-provider-autostop */

	CM_ARCQ_OPC_ALARM_ADD			= 31,
	CM_ARCQ_OPC_ALARM_REMOVE		= 32,

	CM_ARCQ_OPC_CALLBACK			= 91, /* internal: does a callback */
	CM_ARCQ_OPC_FREE_AR_GRAPH		= 92  /* internal for cm_arcq_free_ar_graph() */
};

/*callback*/  LSA_BOOL
cm_arcq_init (
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*callback*/  LSA_VOID
cm_arcq_undo_init (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
);

LSA_VOID
cm_arcq_free_ar_graph (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID
cm_arcq_request (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode, /* see cm_arcq_opcode_enum */
	LSA_UINT16  para1
);

LSA_VOID
cm_arcq_step_next (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_VOID_PTR_TYPE  cr,	/* io_cr or alarm_cr */
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);	/* callback to: cm_cl_arcq_request_done(), cm_mc_arcq_request_done() or cm_sv_arcq_request_done() */

/*===========================================================================*/

LSA_VOID
cm_ar_com_cons_apdu_status_changed (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT32 state_event
);

LSA_VOID
cm_ar_com_cons_apdu_status_check_unicast (
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*===========================================================================*/

LSA_VOID
cm_ar_com_check(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL
cm_ar_com_is_up(
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
);

LSA_BOOL
cm_ar_com_is_rir(
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
);

LSA_BOOL
cm_ar_com_is_down(
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
);

/*===========================================================================*/
/*===========================================================================*/

LSA_BOOL
cm_alarm_type_is_internal ( /* user is not allowed to send this alarm-type */
	LSA_UINT16  alarm_type
);

/*===========================================================================*/

LSA_VOID
cm_acp_alarm_send (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_ALARM_CONST_PTR_TYPE  cm_al,
	LSA_UINT16  upper_alarm_tag,
	LSA_UINT16  upper_alarm_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  upper_alarm_data,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_send_done (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_ack_send (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_ack_send_done (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/

LSA_VOID
cm_acp_alarm_indication (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID
cm_acp_alarm_copy_ind (
	CM_UPPER_ALARM_PTR_TYPE  cm_al, /* destination */
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp, /* source (ACP alarm indication) */
	LSA_UINT32  maintenance_status
);

LSA_VOID
cm_acp_alarm_copy_ack(
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp,        /* destination (alarm-ack request) */
	ACP_UPPER_ALARM_DATA_PTR_TYPE  acp_al, /* source (ACP alarm indication) */
	CM_UPPER_ALARM_CONST_PTR_TYPE  cm_al,        /* source (CM alarm response) */
	LSA_UINT32 cm_pnio_err
);

LSA_VOID
cm_acp_alarm_ack_indication (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_BOOL
cm_acp_alarm_ack_matches (
	CM_UPPER_ALARM_CONST_PTR_TYPE  cm_al,
	CM_ACP_LOWER_RQB_CONST_PTR_TYPE  acp,
	LSA_UINT32 * cm_pnio_err /* result from "acp" */
);

/*------------------------------------------------------------------------------
// EDD
//----------------------------------------------------------------------------*/

LSA_VOID
cm_edd_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_edd_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE
cm_edd_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

CM_EDD_CHANNEL_PARAMS_PTR_TYPE
cm_edd_channel_params(
	CM_CHANNEL_PTR_TYPE  channel
);

CM_EDD_LOWER_RQB_PTR_TYPE
cm_edd_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE  channel,
	EDD_SERVICE  service /* use 0 for open- and close-channel */
);

LSA_VOID
cm_edd_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

CM_EDD_LOWER_RQB_PTR_TYPE
cm_edd_get_rqb (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_edd_release_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_VOID
cm_edd_request_lower(
	CM_CHANNEL_PTR_TYPE  channel,
	EDD_SERVICE  service,
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

LSA_BOOL
cm_edd_can_set_send_clock(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_edd_default_sendclock_prop(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL
cm_edd_can_partial_data(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 rt_class
);

LSA_BOOL
cm_edd_bug_consumer_partial_data_length_allowed ( /* 1415794 */
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_CONST_PTR_TYPE iocr
);

LSA_BOOL
cm_edd_can_autopadding(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 rt_class
);

LSA_UINT16
cm_edd_get_apdu_status_length(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL
cm_edd_can_time_sync_slave(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL
cm_edd_is_HERA(
	CM_CHANNEL_PTR_TYPE channel
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_edd_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_edd_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID
cm_edd_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL
cm_edd_send_clock_range_ok_hw (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock
);

LSA_BOOL
cm_edd_set_send_clock(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock
);

LSA_VOID
cm_edd_link_init (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_edd_link_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_edd_link_data_changed (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  port_id,
	LSA_UINT8  status,
	LSA_UINT8  speed,
	LSA_UINT8  mode,
	LSA_UINT8  irt_port_status,
	LSA_UINT8  autoneg
);

LSA_VOID
cm_edd_link_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

#if CM_CFG_MAX_CLIENTS
LSA_UINT16
cm_edd_set_dcp_hello_filter (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8  mode,
	LSA_UINT32  user_id,
	EDD_UPPER_MEM_U8_PTR_TYPE  name_of_station,
	LSA_UINT32 name_of_station_length
);
#endif

LSA_BOOL
cm_edd_get_link_status (
	CM_CHANNEL_PTR_TYPE  channel
);

/*------------------------------------------------------------------------------
// MRP
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_MRP

LSA_VOID
cm_mrp_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_mrp_channel_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE
cm_mrp_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_mrp_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mrp_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_mrp_channel_close (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

#endif


/*------------------------------------------------------------------------------
// GSY
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY

LSA_VOID
cm_gsy_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_gsy_channel_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE
cm_gsy_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_gsy_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_gsy_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_gsy_channel_close (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

LSA_UINT16
cm_gsy_ptcp_master_control (
	CM_CHANNEL_PTR_TYPE		channel,
	CM_UPPER_PD_PTCP_MASTER_CONTROL_CONST_PTR_TYPE ptcp_master_control
);

#endif

/*------------------------------------------------------------------------------
// POF
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_POF

LSA_VOID
cm_pof_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_pof_channel_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE
cm_pof_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_pof_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_pof_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_pof_channel_close (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

#endif


/*------------------------------------------------------------------------------
// OHA
//----------------------------------------------------------------------------*/

LSA_VOID
cm_oha_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16
cm_oha_channel_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE
cm_oha_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

CM_OHA_LOWER_RQB_PTR_TYPE
cm_oha_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel
);

LSA_VOID
cm_oha_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_oha_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_oha_channel_close (
	CM_CHANNEL_CONST_PTR_TYPE channel
);

#if CM_CFG_MAX_CLIENTS

LSA_VOID
cm_oha_register_client (
	CM_CHANNEL_PTR_TYPE  oha_channel,
	CM_CHANNEL_PTR_TYPE  cl_channel /* LSA_NULL..unregister */
);

#endif

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_oha_set_ident (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_OEM_DATA_CONST_PTR_TYPE  oem_data
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_oha_control_ip_suite(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT8 oha_mode, /* OHA_LOCK_SET or OHA_UNLOCK_SET */
	CM_OHA_LOWER_RQB_PTR_TYPE oha
);

LSA_VOID
cm_oha_write_ptcp_data(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL do_sync_clock,
	LSA_BOOL is_sync,
	OHA_MAC_TYPE *master_mac
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_oha_ip_suite_is_notnull_nonconflicting_and_expected (
	CM_CHANNEL_PTR_TYPE  user_channel
);

LSA_BOOL
cm_oha_get_ip_suite (
	CM_CHANNEL_PTR_TYPE  user_channel,
	LSA_UINT32  * ip_addr,
	LSA_UINT32  * subnet_mask,
	LSA_UINT32  * gateway_ip
);

LSA_BOOL
cm_oha_name_of_station_is_valid (
	CM_CHANNEL_PTR_TYPE  user_channel
);

LSA_BOOL
cm_oha_get_name_of_station (
	CM_CHANNEL_PTR_TYPE  user_channel,
	LSA_UINT16 * nos_length,
	CM_COMMON_MEM_U8_PTR_TYPE * nos
);


/*------------------------------------------------------------------------------
// NARE
//----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE
cm_nare_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

CM_NARE_LOWER_RQB_PTR_TYPE
cm_nare_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel
);

LSA_VOID
cm_nare_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

LSA_VOID
cm_nare_free_rqb_alias(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_nare_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID
cm_nare_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

#if CM_CFG_MAX_CLIENTS

LSA_VOID
cm_nare_register_client (
	CM_CHANNEL_PTR_TYPE  nare_channel,
	CM_CHANNEL_PTR_TYPE  cl_channel /* LSA_NULL..unregister */
);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_BOOL
cm_nare_ident (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 name_length,
	CM_COMMON_MEM_U8_PTR_TYPE name_ptr,
	LSA_UINT16 ident_timeout,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_UINT32 notify_id,
	LSA_VOID_PTR_TYPE cancel_request_id,
	CM_LIST_ENTRY_CONST_PTR_TYPE alias_list,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
);

LSA_BOOL
cm_nare_cancel (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_VOID_PTR_TYPE cancel_request_id,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
);

#endif

#if CM_CFG_MAX_CLIENTS

LSA_BOOL
cm_nare_set(				/* called by CM-CL-AR */
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  mode,
	LSA_UINT8  name_length,
	CM_COMMON_MEM_U8_PTR_TYPE  name_ptr,
	LSA_UINT32  ip_addr,
	LSA_UINT32  subnet_mask,
	LSA_UINT32  def_router,
	CM_MAC_TYPE  mac_addr,
	LSA_BOOL  set_ip_remanent,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr,
	LSA_VOID_PTR_TYPE  cancel_request_id
);

LSA_BOOL
cm_nare_test_ip (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  local_ip_addr,
	LSA_UINT32  ip_addr,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr,
	LSA_VOID_PTR_TYPE  cancel_request_id
);

#endif


/*------------------------------------------------------------------------------
// CM unit test helper functions
//----------------------------------------------------------------------------*/

#ifdef CM_UNIT_TEST
#include "CmUnitTestHelper.h"
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_INT_H */
