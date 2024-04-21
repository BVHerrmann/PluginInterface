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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pniousrx.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Data types and function declarations for IO-Base controller              */
/*  user interface                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef PNIOUSRX_H
#define PNIOUSRX_H

#include "pniobase.h"

#undef ATTR_PACKED
#if defined(_MSC_VER)
 #pragma pack( push, safe_old_packing, 4 )
 #define ATTR_PACKED
#elif defined(__GNUC__)
 #define ATTR_PACKED  __attribute__ ((aligned (4)))
#elif defined(BYTE_ATTR_PACKING)
 #include "pack.h"
 #define ATTR_PACKED PPC_BYTE_PACKED
#else
 #error please adapt pniousrx.h header for your compiler
#endif


#undef PNIO_CODE_ATTR
#if defined PASCAL && !defined PNIO_PNDRIVER
    #define PNIO_CODE_ATTR __stdcall
#else
    #define PNIO_CODE_ATTR
#endif


//#define PNIO_CODE_ATTR
/* constants specificaton */

#define PNIO_MAX_IO_LEN         1024
#define PNIO_MAX_REC_LEN       32768

/* data types specification */

/**
    \brief
    The PNIO_MODE_TYPE data type contains the IDs for the supported modes.
    \param PNIO_MODE_OFFLINE OFFLINE mode ("Stop")
    \param PNIO_MODE_CLEAR   CLEAR mode
    \param PNIO_MODE_OPERATE Operate mode
 */
typedef enum {
    PNIO_MODE_OFFLINE = 0, /**< OFFLINE mode ("Stop") */
    PNIO_MODE_CLEAR   = 1, /**< CLEAR mode  */
    PNIO_MODE_OPERATE = 2  /**< OPERATE mode */
} PNIO_MODE_TYPE;

/**
    \brief
    Identifiers for deactivating and activating an IO device.
    \param PNIO_DA_FALSE Disable
    \param PNIO_DA_TRUE  Activate
 */
typedef enum {
    PNIO_DA_FALSE = 0, 
    PNIO_DA_TRUE  = 1  
} PNIO_DEV_ACT_TYPE;

/**
    \brief
    The PNIO_ADDR address structure is used for addressing with all functions described here.
    \param AddrType Must always have the value PNIO_ADDR_LOG for IO controllers.
    \param IODataType Input or output
    \param Addr Address
    \param Reserved Reserved for future expansion
*/
 typedef struct {
    PNIO_ADDR_TYPE  AddrType; 
    PNIO_IO_TYPE IODataType;  
    union {
        PNIO_UINT32 Addr;  
        PNIO_UINT32  Reserved [5]; 
    } u;
} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_ADDR;

typedef PNIO_ALARM_TINFO PNIO_CTRL_RALRM_TINFO;
typedef PNIO_ALARM_INFO PNIO_CTRL_ALARM_INFO;

/**
\brief
The PNIO_ALARM_MODD_DIFF_BLOCK structure is used for storing module diff block info.
\param mod_diff_block_length is the number of bytes of module diff block.
\param mod_diff_block is the buffer where module diff block data is stored, if any.
*/
typedef struct {
	PNIO_UINT16 ModDiffBlockLength;
	PNIO_VOID_PTR_TYPE pModDiffBlock;
} PNIO_ALARM_MOD_DIFF_BLOCK;

typedef struct {
    PNIO_ALARM_TYPE  AlarmType;         /**< original alarm-identifier from alarm-data-block                  */
    PNIO_APRIO_TYPE  AlarmPriority;     /**< alarm priority for net, high or low -> see cm defines            */
    PNIO_UINT16      DeviceNum;         /**< station number                                                   */
    PNIO_UINT16      SlotNum;           /**< slot-no form alarm-data-block or 0                               */
    PNIO_UINT16      SubslotNum;        /**< subslot-no from alarm-data-block or 0                            */

    PNIO_UINT16  CpuAlarmPriority;      /**< only relevant with process alarm, ob number                      */
    PNIO_UINT32  PnioCompatModtype;     /**< only relevant for plug/pull alarms: real == expected => 0x8101   */


    PNIO_CTRL_RALRM_TINFO  AlarmTinfo;   /**< see struct definition*/

    PNIO_UINT8       DiagDs[4];         /**< only relevant with diagnostic alarm, all 0x0 otherwise         */
    PNIO_UINT8       PrAlarmInfo[4];    /**< only relevant with process alarm, all 0x0 otherwise            */

    PNIO_CTRL_ALARM_INFO   AlarmAinfo;   /**< see struct definition, only relevant if not device failure/return alarm  */

	PNIO_ALARM_MOD_DIFF_BLOCK ModDiffBlock; /**< only relevant with PNIO_ALARM_DEV_RETURN, PNIO_ALARM_PLUG, PNIO_ALARM_RELEASED_BY_SUPERVISOR */
} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_ALARM_DATA;

#if !defined PNIO_SOFTNET /* not supported for PNIO SOFTNET */
/* PNIO_UINT8 type of the 'AddressInvalid'  field in the struct PNIO_CTRL_DIAG_CONFIG_SUBMODULE
 *                                            The AddressInvalid flag describes content of the field 'Address' in the struct.
 *   PNIO_DIAG_CONF_EA_ADDR_NOT_VALID = 0   : content of the 'Address' element is invalid!!! Use HwIdentifier for addressing !!!
 *   PNIO_DIAG_CONF_EA_ADDR_VALID     = 1   : content of the 'Address' element is valid. I.e.: Logical address is present
 *  
 * PNIO_DIAG_CONF_IS_ADDR_VALID: Access macro to test the validity of the log-address ('Address') value
 */ 

#define PNIO_DIAG_CONF_EA_ADDR_NOT_VALID    0
#define PNIO_DIAG_CONF_EA_ADDR_VALID        1

#define PNIO_DIAG_CONF_IS_ADDR_VALID(pCtrlDiagConf_Resp) (bool)(pCtrlDiagConf_Resp->AddressValid == PNIO_DIAG_CONF_EA_ADDR_VALID )

/* PNIO_CTRL_DIAG_CONFIG_SUBMODULE
*/

/**
    \brief
    The structure provides information about a configured submodule. 
    It is returned for the diagnostics request PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST.
    \param Address Logical address of the submodule
    \param DataLength Data length of the submodule
        Note 
        Logical addresses of submodules with the input length 0 are diagnostics addresses. 
        For the meaning of the diagnostics addresses, refer to the STEP 7 documentation.
    \param DataType Specifies the type of IO data.
    \param ComType Specifies whether or not direct data exchange is used for the submodule.
    \param Api Specifies the Api to which the submodule belongs.
    \param ReductionFactor The "ReductionFactor" specifies how often data is sent. 
        With, for example, a ReductionFactor of 2, data is sent every 2 * CycleTime.
    \param Phase If there is a "ReductionFactor" > 1, this specifies the send time within a cycle; 
        Range of values: 0 to ReductionFactor - 1
    \param CycleTime Send clock in multiples of 31.25 &mu;s
    \param HwIdentifier HW-Identifier
    \param AddressValid I/O Address (Log-Addr) is valid
    \param StartNo Number of the station in which the submodule is inserted.
    \param Slot Module number from the perspective of the device
    \param Subslot Submodule number from the perspective of the device
    \param Reserved1 Reserved for future expansions
    \param Reserved2 Reserved for future expansions
*/
typedef struct {
    PNIO_ADDR    Address;   ///< IO address of the submodule

    PNIO_UINT32     DataLength; /**< data length of IO-Data input, output */
    PNIO_DATA_TYPE  DataType;   /**< data type of IO-Data IRT, RT */
    PNIO_COM_TYPE   ComType;    /**< communication type: controller-device, direct data exchange */

    PNIO_UINT32   Api;          ///< Specifies the Api to which the submodule belongs.
    PNIO_UINT32   ReductionFactor; ///< The "ReductionFactor" specifies how often data is sent. With, for example, a ReductionFactor of 2, data is sent every 2 * CycleTime.
    PNIO_UINT32   Phase; /**< If there is a "ReductionFactor" > 1, this specifies the send time with-in a cycle; Range of values: 0 to ReductionFactor – 1 */
    PNIO_UINT32   CycleTime;      /**< Send clock in multiples of 31.25 us */

    PNIO_UINT32   HwIdentifier;    /**< HW-Identifier */
    PNIO_UINT8    AddressValid;    /**< E/A Address (Log-Addr) is valid  */
    PNIO_UINT8    Reserved1[3]; 
	PNIO_UINT32   StatNo;
    PNIO_UINT32   Slot;
    PNIO_UINT32   Subslot;
    PNIO_UINT32   Reserved2[2];
} ATTR_PACKED PNIO_CTRL_DIAG_CONFIG_SUBMODULE;

/**
    \brief
    If output bit slices are assigned to an external IO controller due to IO routing, 
    the local IO controller user program can no longer write to these output bit slices.
    The PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE_LIST diagnostics query provides information 
    about the output bits that the local IO controller user program can write to. 
    It is an extension of the PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT diagnostics request 
    described in the section "PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT 
    (diagnostics request about IO routers) 
    The notification comes from the callback function with callback event type 
    PNIO_CBE_TYPE = PNIO_CBE_CTRL_DIAG_CONF registered in PNIO_register_cbf( ) 
    (see also PNIO_CTRL_DIAG and PNIO_CTRL_DIAG_ENUM).
    The output bit slices available for the local IO controller user program are reported 
    in the form of the PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE data structure described below. 
    The data structures of several output bit slices follow one after the other.
    The parameters "DiagDataBuffer" and "DiagDataBufferLen" in the callback event structure PNIO_CBE_CTRL_DIAG_CONF 
    (see the section "Callback event PNIO_CBE_CTRL_DIAG_CONF (signal result of the diagnostics request)      
    return the result of the request.
    \note In contrast to the output bits that can only be assigned to a single IO controller, 
        input bits can always be read by both IO controllers. There is therefore no need for an information function. 
    \param Address Logical address of the submodule
    \param BitOffset Specified as a number of bits
    \param BitLength Specified as a number of bits
 */
typedef struct {
    PNIO_ADDR     Address; ///< Logical address of the submodule
    PNIO_UINT16   BitOffset; ///< Specified as a number of bits
    PNIO_UINT16   BitLength; ///< Specified as a number of bits
} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE;

#define  PNIO_CTRL_DEV_DIAG_OFFLINE            0x00000001 /**< device isn't switched on (or) 
                                                                    isn't attainable over the net (or) 
                                                                    has wrong device name */
#define  PNIO_CTRL_DEV_DIAG_ADDR_INFO_OK       0x00000010 /**< name resolution succeded (device name -> IP) */
#define  PNIO_CTRL_DEV_DIAG_AR_CONNECTING      0x00000100 /**< intermediate state */
#define  PNIO_CTRL_DEV_DIAG_AR_IN_DATA         0x00000200 /**<  
                                                                - SRT: connection was built up (or)
                                                                - RT: intermediate state (or)
                                                                - RT: device is attached by wrong port (or)
                                                                - RT: network configuration is not equal to STEP7 project */
#define  PNIO_CTRL_DEV_DIAG_AR_OFFLINE         0x00000400 /**<  SRT: see PNIO_CTRL_DIAG_DEVICE::Reason (or) 
                                                                RT: intermediate state */
#define  PNIO_CTRL_DEV_DIAG_AR_RTC3_FOLLOWS    0x00001000 /**< RT: intermediate state (or) 
                                                              RT: device don't send APPLICATION_READY confirmation */


#define  PNIO_CTRL_REASON_NONE                             0x0000
#define  PNIO_CTRL_REASON_NARE_ERR_MULTIPLE_IP_USE         0x0101
#define  PNIO_CTRL_REASON_NARE_ERR_IP_IN_USE               0x0102
#define  PNIO_CTRL_REASON_NARE_ERR_DCP_MULTIPLE_STATIONS   0x0103
#define  PNIO_CTRL_REASON_NARE_ERR_DCP_SET_FAILED          0x0104
#define  PNIO_CTRL_REASON_NARE_ERR_DCP_SET_TIMEOUT         0x0105
#define  PNIO_CTRL_REASON_NARE_ERR_DCP_STATION_NOT_FOUND   0x0106
#define  PNIO_CTRL_REASON_NARE_ERR_OPCODE                  0x0107
#define  PNIO_CTRL_REASON_NARE_ERR_CHANNEL_USE             0x0108
#define  PNIO_CTRL_REASON_NARE_ERR_DNS_FAILED              0x0109
#define  PNIO_CTRL_REASON_NARE_ERR_LL                      0x010a
#define  PNIO_CTRL_REASON_NARE_ERR_NO_OWN_IP               0x010b
#define  PNIO_CTRL_REASON_NARE_ERR_DCP_IDENT_TLV_ERROR     0x010c
#define  PNIO_CTRL_REASON_NARE_ERR_IP_RESOLVE_NO_ANSWER    0x010d
#define  PNIO_CTRL_REASON_CLRPC_ERR_FAULTED                0x0201
#define  PNIO_CTRL_REASON_CLRPC_ERR_REJECTED               0x0202
#define  PNIO_CTRL_REASON_CLRPC_ERR_IN_ARGS                0x0203
#define  PNIO_CTRL_REASON_CLRPC_ERR_OUT_ARGS               0x0204
#define  PNIO_CTRL_REASON_CLRPC_ERR_DECODE                 0x0205
#define  PNIO_CTRL_REASON_CLRPC_ERR_PNIO_OUT_ARGS          0x0206
#define  PNIO_CTRL_REASON_CLRPC_ERR_TIMEOUT                0x0207
#define  PNIO_CTRL_REASON_CM_ADDR_INFO_CLASS_LINK          0x0301
#define  PNIO_CTRL_REASON_CM_ADDR_INFO_CLASS_MISS          0x0401
#define  PNIO_CTRL_REASON_CM_ADDR_INFO_CLASS_COMPANION     0x0501
#define  PNIO_CTRL_REASON_CM_ADDR_INFO_CLASS_OTHER         0x0601

/**
    \brief
    The "PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC" structure returns diagnostics information and the status of an IO device.
        To execute the request, the logical address of an IO device submodule must be specified in the 
            "u.Addr" parameter with the function call of the function PNIO_ctrl_diag_req().
        The result of the request is returned when the callback event "PNIO_CBE_CTRL_DIAG_CONF" is called. 
            The parameter "DiagDataBuffer" contains the structure "PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC". 
            The length is specified with the "DiagDataBufferLen".
    \param Mode The current status of the IO device PNIO_DEV_ACT_TYPE; 
        see the section "PNIO_DEV_ACT_TYPE (type for activating all deactivating an IO device) 
    \param ErrorCause detailed IO device diagnostics
    \param ReasonCode detailed IO device diagnostics
    \param AdditionalInfo[10] detailed IO device diagnostics
*/
typedef struct {
    PNIO_DEV_ACT_TYPE Mode; ///< The current status of the IO device PNIO_DEV_ACT_TYPE; see the section
    PNIO_UINT16       ErrorCause;          /**< detailed IO device diagnostics. big-endian(!) */
    PNIO_UINT8        ReasonCode;      ///< detailed IO device diagnostics
    PNIO_UINT8        AdditionalInfo[10];  /**< detailed IO device diagnostics. big-endian(!) */
} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA;

 /**
    \brief
    The PNIO_CTRL_DIAG_DEVICE_STATE structure provides information on the status of an IO device.
    To start the request, the logical address of an IO device submodule must be transferred with 
    the PNIO_ctrl_diag_req( ) function call in the "u.Addr" element of 
    the PNIO_CTRL_DIAG structure to which "pDiagReq" points.
    The "DiagDataBuffer" and "DiagDataBufferLen" parameters in the PNIO_CBE_CTRL_DIAG_CONF callback event 
    (see section "Callback event PNIO_CBE_CTRL_DIAG_CONF (signal result of the diagnostics request) 
    return the result of the request - the "PNIO_CTRL_DIAG_DEVICE" structure.
    \param Mode Current status of the IO device "PNIO_DEV_ACT_TYPE"; 
        see the section "PNIO_DEV_ACT_TYPE (type for activating all deactivating an IO device) 
    \param DiagState Current status of the connection establishment to this IO device - 
        The possible values are as follows:
        - PNIO_CTRL_DEV_DIAG_OFFLINE
        - PNIO_CTRL_DEV_DIAG_ADDR_INFO_OK
        - PNIO_CTRL_DEV_DIAG_AR_CONNECTING
        - PNIO_CTRL_DEV_DIAG_AR_IN_DATA
        You will find the meaning of the values in the header file "pniousrx.h".
    \param Reason If there are errors in connection establishment, the cause of the error is entered here. 
        You will find the possible values in the "PNIO_CTRL_REASON_..." constant in the "pniousrx.h" header file.
    \param Reserved1[12] Reserved 
*/
typedef struct {
    PNIO_DEV_ACT_TYPE  Mode; ///< Current status of the IO device "PNIO_DEV_ACT_TYPE";
    PNIO_UINT32        DiagState; /**< BitField, see PNIO_CTRL_DEV_DIAG_XXX */
    PNIO_UINT32        Reason;    /**< see PNIO_CTRL_REASON_XXX */
    PNIO_UINT32        Reserved1[12];
} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_DIAG_DEVICE;

/**
    \brief
    This structure contains the PROFINET network parameters of the controller such as device name, 
    device type and the address parameters (IP address, mask and default router). These parameters 
    are transferred to the controller by configuring with STEP 7.
    \param name[256] PROFINET station name of the module
    \param TypeOfStation[256] PROFINET device type
    \param ip_addr IP address of the module
    \param ip_mask IP subnet mask of the module
    \param default_router IP default gateway of the module
 */
 #ifndef PNIO_SOFTNET /* not supported for PNIO SOFTNET */
typedef struct {
    PNIO_UINT8    name[256];           /**< PROFINET station name of the module. ctrl 'device name' (Geraetename) StationName  */
    PNIO_UINT8    TypeOfStation[256];  /**< PROFINET device type. ctrl 'device type' (Geraetetyp ) TypeOfStation*/
    PNIO_UINT32   ip_addr;  ///< IP address of the module
    PNIO_UINT32   ip_mask;  ///< IP subnet mask of the module
    PNIO_UINT32   default_router; ///< IP default gateway of the module
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA;
#endif /* PNIO_SOFTNET */
/**
    \brief
    These structures return information on the quality of the data transfer 
    in the form of statistical data of the Ethernet interface.
    \param SndNoError Frames sent free of error
    \param SndCollision Failed attempt to send (collision)
    \param SndOtherError Failed attempts to send
    \param RcvNoError Frames received free of error
    \param RcvResError Incorrectly received frames
    \param RcvRejected Rejected frames
    \param Port[4] Statistical data of ports 1/-4
    \param NumberOfPorts Number of Ethernet interfaces (can be 3 (industrial PC) or 4 (CP 1616 / CP 1604))
*/
typedef struct {
    PNIO_UINT32     SndNoError;     ///< Frames sent free of error
    PNIO_UINT32     SndCollision;   ///< Failed attempt to send (collision)
    PNIO_UINT32     SndOtherError;  ///< Failed attempts to send
    PNIO_UINT32     RcvNoError;     ///< Frames received free of error
    PNIO_UINT32     RcvResError;    ///< Incorrectly received frames
    PNIO_UINT32     RcvRejected;    ///< Rejected frames
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_COMM_PORT_COUNTER_DATA;

typedef struct {
    PNIO_CTRL_COMM_PORT_COUNTER_DATA    Port[4]; ///< Statistical data of ports 1/-4
    PNIO_UINT16                 NumberOfPorts;  ///< Number of Ethernet interfaces (can be 3 (industrial PC) or 4 (CP 1616 / CP 1604))
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_COMM_COUNTER_DATA;

/**
    \brief
    This enumeration type lists the diagnostics services. The supported services are described.
    \param PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST Supplies a list of all configured submodules - 
        The diagnostics reply buffer "pDataBuffer" in the PNIO_CBE_CTRL_DIAG_CONF callback event 
        contains elements of the type PNIO_CTRL_DIAG_CONFIG_SUBMODULE; 
        see the section "PNIO_CTRL_DIAG_CONFIG_SUBMODULE (submodule configuration information) 
    \param PNIO_CTRL_DIAG_DEVICE_STATE The diagnostics query provides information on the status of an IO device. 
        The diagnostics reply buffer "pDataBuffer" in the PNIO_CBE_CTRL_DIAG_CONF callback event 
        contains an element of the type PNIO_CTRL_DIAG_DEVICE; 
        see the section "PNIO_CTRL_DIAG_DEVICE_STATE 
        Is not supported by the PN Driver
    \param PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT The diagnostics request provides information 
        on whether there are output bit slices reserved for IO routing; 
        see the section "PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT (diagnostics request about IO routers) 
        Is not supported by the PN Driver
    \param PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE_LIST The diagnostics request provides information 
        on the size of the output bit slices remaining for IO routing; 
        see the section "PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE_LIST (diagnostics query about IO router) 
        Is not supported by the PN Driver
    \param PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO The diagnostic query returns the configured device name, 
        device type and the IP address parameters (IP address, mask and default router). 
        The results are returned in the structure "PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA"; 
        see the section "PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA 
    \param PNIO_CTRL_DIAG_GET_COMM_COUNTER_DATA This diagnostics query returns information 
        on the quality of the data transfer in the form of statistical data of the Ethernet interface. 
        Refer to the explanations of the structure 
        "PNIO_CTRL_COMM_PORT_COUNTER_DATA and PNIO_CTRL_COMM_COUNTER_DATA 
        Is not supported by the PN Driver
    \param PNIO_CTRL_DIAG_RESET_COMM_COUNTERS This diagnostics query returns information
        on the quality of the data transfer in the form of statistical data of the Ethernet interface. 
        Refer to the explanations of the structure 
        "PNIO_CTRL_COMM_PORT_COUNTER_DATA and PNIO_CTRL_COMM_COUNTER_DATA 
        In addition, the counters for the statistical data are reset to 0 after reading out the data.
        Is not supported by the PN Driver
    \param PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC The diagnostics request returns detailed information for an AR abort. 
        The result is returned with the structure "PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC"; 
        see section "PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC 
*/
typedef enum {
    PNIO_CTRL_DIAG_RESERVED                         = 0, /**< Supplies a list of all configured submodules : The diagnos-tics reply buffer "pDataBuffer"
                                                          * in the PNIO_CBE_CTRL_DIAG_CONF callback event contains elements of the type PNIO_CTRL_DIAG_CONFIG_ SUBMODULE;*/
    PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST            = 1, /**< The diagnostics query provides information on the status of an IO device. The diagnostics reply buffer 
                                                           * "pDataBuffer" in the PNIO_CBE_CTRL_DIAG_CONF callback event contains an element of the type PNIO_CTRL_DIAG_DEVICE; */
    PNIO_CTRL_DIAG_DEVICE_STATE                     = 2, /**< The diagnostics request provides information on whether there are output bit slices reserved for IO routing; 
                                                          *   see the section PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT.
                                                          *   Is not supported by the PN Driver*/
    PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT          = 3, /**< The diagnostics request provides information on whether there are output bit slices reserved for IO routing;
                                                          *  Is not supported by the PN Driver */
    PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE_LIST         = 4, /**< The diagnostics request provides information on the size of the output bit slices remaining for IO routing; 
                                                          *  Is not supported by the PN Driver*/
#ifndef PNIO_SOFTNET /* not supported for PNIO SOFTNET */
    PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO            = 5, /**< The diagnostic query returns the configured device name, device type and the IP address parameters 
                                                           * (IP address, mask and default router). 
                                                           * The results are returned in the structure*/
                                                /* FW V2.5.2.0 and greater */ 
    PNIO_CTRL_DIAG_GET_COMM_COUNTER_DATA            = 6, /**< This diagnostics query returns information on the quality of the data transfer 
                                                          * in the form of statistical data of the Ethernet interface. Is not supported by the PN Driver */
    PNIO_CTRL_DIAG_GET_AND_RESET_COMM_COUNTER_DATA  = 7, /**< This diagnostics query returns information on the quality of the data transfer 
                                                          * in the form of statistical data of the Ethernet interface. Refer to the explanations of the structure 
                                                          * PNIO_CTRL_COMM_PORT_COUNTER_DATA and PNIO_CTRL_COMM_COUNTER_DATA. In addition, the counters for the statistical data are 
                                                          * reset to 0 after reading out the data.Is not supported by the PN Driver */	  
                                                 /* FW V2.5.2.0 and greater */
#endif /* PNIO_SOFTNET */												 
    PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC                = 8 /**< The diagnostics request returns detailed information for an AR abort.
                                                         * The result is returned with the structure "PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC"; */
} PNIO_CTRL_DIAG_ENUM;

typedef enum {
    PNIO_MOD_STATE_NO_MODULE = 0,
    PNIO_MOD_STATE_WRONG_MODULE = 1,
    PNIO_MOD_STATE_PROPER_MODULE = 2,
    PNIO_MOD_STATE_SUBSTITUTED_MODULE = 3,
}PNIO_MOD_STATE_ENUM;

typedef enum {
    PNIO_SUB_STATE_ADD_INFO_MASK = 0x0007, /* Bit 0 - 2: SubmoduleState.AddInfo */
    PNIO_SUB_STATE_ADD_INFO_NONE = 0x0000,
    PNIO_SUB_STATE_ADD_INFO_TAKEOVER_NOT_ALLOWED = 0x0001,

    PNIO_SUB_STATE_ADVICE_MASK = 0x0008, /* Bit 3: SubmoduleState.Advice */
    PNIO_SUB_STATE_ADVICE_NOT_AVAILABLE = 0x0000,
    PNIO_SUB_STATE_ADVICE_AVAILABLE = 0x0008,

    PNIO_SUB_STATE_MAINTENANCE_REQUIRED_MASK = 0x0010, /* Bit 4: SubmoduleState.MaintenanceRequired */
    PNIO_SUB_STATE_MAINTENANCE_NOT_REQUIRED = 0x0000,
    PNIO_SUB_STATE_MAINTENANCE_REQUIRED = 0x0010,

    PNIO_SUB_STATE_MAINTENANCE_DEMANDED_MASK = 0x0020, /* Bit 5: SubmoduleState.MaintenanceDemanded */
    PNIO_SUB_STATE_MAINTENANCE_NOT_DEMANDED = 0x0000,
    PNIO_SUB_STATE_MAINTENANCE_DEMANDED = 0x0020,

    PNIO_SUB_STATE_DIAG_INFO_MASK = 0x0040, /* Bit 6: SubmoduleState.DiagInfo */
    PNIO_SUB_STATE_DIAG_INFO_NOT_AVAILABLE = 0x0000,
    PNIO_SUB_STATE_DIAG_INFO_AVAILABLE = 0x0040,

    PNIO_SUB_STATE_AR_INFO_MASK = 0x0780, /* Bit 7 - 10: SubmoduleState.ARInfo */
    PNIO_SUB_STATE_AR_INFO_OWNED = 0x0000,
    PNIO_SUB_STATE_AR_INFO_APPL_READY_PENDING = 0x0080,
    PNIO_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED = 0x0100,
    PNIO_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER = 0x0180,
    PNIO_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR = 0x0200,

    PNIO_SUB_STATE_IDENT_MASK = 0x7800, /* Bit 11 - 14: SubmoduleState.IdentInfo */
    PNIO_SUB_STATE_IDENT_OK = 0x0000,
    PNIO_SUB_STATE_IDENT_SUBSTITUTE = 0x0800,
    PNIO_SUB_STATE_IDENT_WRONG = 0x1000,
    PNIO_SUB_STATE_IDENT_NO_SUBMODULE = 0x1800,

    PNIO_SUB_STATE_FORMAT_MASK = 0x8000, /* Bit 15: SubmoduleState.IdentInfo */
    PNIO_SUB_STATE_FORMAT_0 = 0x0000,
    PNIO_SUB_STATE_FORMAT_1 = 0x8000
}PNIO_SUB_STATE_ENUM;


/**
    \brief
    This structure is used to set up a diagnostics request. 
    It is used with the PNIO_ctrl_diag_req( ) function and with the PNIO_CBE_CTRL_DIAG_CONF callback event.
    \param DiagService Interpret the diagnostics reply - 
        Diagnostics service according to the PNIO_CTRL_DIAG_ENUM diagnostics service; 
        see the section "PNIO_CTRL_DIAG_ENUM (diagnostics service) 
    \param Reserved1[8] Reserved for future expansion
    \param Addr Address of a submodule for the diagnostics query of the status of an IO device; 
        applies to DiagService = PNIO_CTRL_DIAG_DEVICE_STATE.
    \param ReqRef Unique reference supplied by the user for this diagnostics request.
    \param Reserved2 Reserved for future expansion
 */
typedef struct {
    PNIO_CTRL_DIAG_ENUM DiagService;
    union {
       PNIO_UINT32      Reserved1[8]; /**< Interpret the diagnostics reply : Diagnostics service according to the PNIO_CTRL_DIAG_ENUM 
                                        *  diagnostics service; see the section PNIO_CTRL_DIAG_ENUM */
       PNIO_ADDR        Addr;        /**< Address of a submodule for the diagnostics query of the status of an IO device; applies to DiagService = PNIO_CTRL_DIAG_DEVICE_STATE. */
                                     /* for PNIO_CTRL_DIAG_DEVICE_STATE */
    }u;
    PNIO_REF            ReqRef; /**< Unique reference supplied by the user for this diagnostics request. */
    PNIO_UINT32         Reserved2; /**< Reserved for future expansion */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CTRL_DIAG;
#endif /* not supported for PNIO SOFTNET */

/**
    \brief
    The data type "PNIO_SET_IP_NOS_MODE_TYPE" lists the possible variants for the parameter "Mode" of 
    the function "PNIO_interface_set_ip_and_nos().
    \param PNIO_SET_IP_MODE The "IP Suite" can be changed.
    \param PNIO_SET_NOS_MODE The station name can be changed
 */
typedef enum {
    PNIO_SET_IP_MODE         = (1 << 0), /**< 0001b */
    PNIO_SET_NOS_MODE        = (1 << 1)  /**< 0010b */
}PNIO_SET_IP_NOS_MODE_TYPE;

/**
    \brief
    The data type "PNIO_IPv4" sets the "IP Suite" of the function "PNIO_interface_set_ip_and_nos".
    \param IpAddress[4] IP address
    \param NetMask[4] Subnet mask
    \param Gateway[4] Gateway
    \param Remanent If the parameter "PNIO_TRUE" is set, you can use the callback function to store the parameters retentively.
 */
typedef struct {
    PNIO_UINT8  IpAddress[4];
    PNIO_UINT8  NetMask[4];
    PNIO_UINT8  Gateway[4];
    PNIO_BOOL   Remanent;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_IPv4;

/**
    \brief
    The data type "PNIO_NOS" sets the station name of the function "PNIO_interface_set_ip_and_nos".
    \param Nos[256] Station name
    \param Length Length of the station name
    \param Remanent If the parameter "PNIO_TRUE" is set, 
        you can use the callback function to store the parameters retentively.
 */
typedef struct  
{
    PNIO_UINT8  Nos[256]; ///< Station name
    PNIO_UINT16 Length; ///< Length of the station name
    PNIO_BOOL   Remanent; ///< If the parameter "PNIO_TRUE" is set, you can use the callback function to store the parameters retentively.
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_NOS;

/**
    \enum
    The function "PNIO_IOS_RECONFIG_ MODE()" lists the possible 
        variant for the "Mode" parameter of the function "PNIO_interface_reconfig()".
    \param PNIO_IOS_RECONFIG_MODE_DEACT Deactivates all IO devices.
    \param PNIO_IOS_RECONFIG_MODE_TAILOR Starts the reconfiguration of the IO system with the configuration set 
        by the user and then activates the IO devices according to the configuration.
 */
typedef enum {
    PNIO_IOS_RECONFIG_MODE_DEACT   = 1, ///< Deactivates all IO devices.
    PNIO_IOS_RECONFIG_MODE_TAILOR  = 2 /**< Starts the reconfiguration of the IO system with the configuration 
                                        * set by the user and then activates the IO devices according to the configuration.
                                         */
}PNIO_IOS_RECONFIG_MODE;

typedef struct {
    PNIO_UINT32            DeviceCount;
    PNIO_ADDR *            DeviceList;
    PNIO_UINT32            PortInterconnectionCnt;
    PNIO_ADDR *            PortInterconnectionList;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_IOS_RECONFIG_TYPE, * PNIO_IOS_RECONFIG_PTR_TYPE;

/* structure types for the callback events */

typedef enum {
    PNIO_CBE_MODE_IND       = 1,
    PNIO_CBE_ALARM_IND      = 2,
    PNIO_CBE_REC_READ_CONF  = 3,
    PNIO_CBE_REC_WRITE_CONF = 4,
    PNIO_CBE_DEV_ACT_CONF   = 5,
    PNIO_CBE_CP_STOP_REQ    = 6
#if !defined PNIO_SOFTNET && !defined PNIO_PNDRIVER /* not supported for PNIO SOFTNET and PNIO PNDRIVER*/
    ,
    PNIO_CBE_START_LED_FLASH = 7,
    PNIO_CBE_STOP_LED_FLASH = 8
#endif /* not supported for PNIO SOFTNET and PNIO PNDRIVER */

#if !defined PNIO_SOFTNET /* not supported for PNIO SOFTNET */
    ,
    PNIO_CBE_CTRL_DIAG_CONF = 9
#endif /* not supported for PNIO SOFTNET */
    ,
    PNIO_CBE_IFC_SET_ADDR_CONF  = 10,
    PNIO_CBE_REMA_READ_CONF     = 11,
    PNIO_CBE_IOSYSTEM_RECONFIG  = 12,
    PNIO_CBE_IFC_REC_READ_CONF  = 13,
    PNIO_CBE_IFC_ALARM_IND      = 14

} PNIO_CBE_TYPE;

/**
    \brief
    The PNIO_CBE_MODE_IND callback event reports a change in the mode of the IO controller; 
    see the section "PNIO_CBE_TYPE (callback event type) 
    The callback event must already have been registered with the PNIO_register_cbf( ) function.
    \note This callback event must be registered before the PNIO_set_mode call.
    \note On completion of PNIO_controller_open( ), the IO controller is OFFLINE.
    This mode change cannot, however, be signaled because at this time the callback function cannot have been registered!
    \param Mode: New mode; see the section "PNIO_MODE_TYPE (operating mode type) 
    and header file PNIOUSRX.H.
 */
typedef struct {
    PNIO_MODE_TYPE      Mode; ///< New mode; see the section PNIO_MODE_TYPE  (operating mode type)
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_MODE_IND;

/**
    \brief
    The PNIO_CBE_REC_READ_CONF callback event reports the result of a read data record job sent previously 
        with a PNIO_rec_read_req call; see the section "PNIO_CBF (PNIO callback function) 
    The callback event must already have been registered with the PNIO_controller_open( ) function.
    This callback event signals that a previous read data record job was completed.
    \param Length: Length of the transferred data record (in bytes) to which "pBuffer" points.
    \param pBuffer: Pointer to the data buffer that, if successful, contains the requested data record.
        Note 
            The data buffer is valid only within the callback function 
            and becomes invalid once the function completes!
        Note 
            The data buffer must not be changed during processing of the callback function
    \param pAddr: Address of the module of the IO device that replied to the read data record job.
    \param RecordIndex: Data record number
    \param ReqRef: Reference assigned by the IO-Base controller user program. 
        Note 
        Here, the reference specified with the PNIO_rec_read_req( ) call is returned.
        This allows the IO-Base controller user program to distinguish different jobs.
    \param Err: Error code on the execution of the job
    \param Length Length of the data record in bytes to which the "pBuffer" parameter points.
    \param pBuffer Pointer to the data record containing the requested data record.
        The data buffer is only valid while the callback function is executing, 
        and it becomes invalid as soon as the function is completed.
    \param pAddr Address of a module of the local Ethernet interface, that responded to the read data record job.
    \param RecordIndex Data record index
    \param ReqRef Reference specified by you. 
        This reference allows you to distinguish between several jobs. 
        You transferred this parameter with by calling "PNIO_interface_rec_read_req()".
    \param Err Error code on the execution of the job.
    \returns No return values.
 */
typedef struct {
    PNIO_ADDR *         pAddr;
    PNIO_UINT32         RecordIndex;
    PNIO_REF            ReqRef;
    PNIO_ERR_STAT       Err;
    PNIO_UINT32         Length;
    const PNIO_UINT8 *  pBuffer;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_REC_READ_CONF;

/**
    \brief
    The PNIO_CBE_REC_WRITE_CONF callback event reports the result of a write data record job sent previously 
        with a PNIO_rec_write_req call; see the section "PNIO_CBF (PNIO callback function) 
        The callback event must already have been registered with the PNIO_controller_open( ) function.
    \param pAddr Address of the module of the IO device for which the read data record job is intended.
    \param RecordIndex Data record number
    \param ReqRef Reference assigned by the IO-Base controller user program. 
        Note 
        The "ReqRef" reference specified with PNIO_rec_write_req( ) is returned.
        This allows the IO-Base controller user program to distinguish different jobs.
    \param Err Error code on the execution of the job. 
        Note 
        "Err" is the PROFINET IO error code transferred by the IO device for the error that occurred 
        when this read data record job executed on the IO device.
 */
typedef struct {
    PNIO_ADDR *         pAddr; ///< Address of the module of the IO device for which the read data record job is intended.
    PNIO_UINT32         RecordIndex; ///< Data record number
    PNIO_REF            ReqRef; /**< Reference assigned by the IO-Base controller user program. 
                                 * \Note The "ReqRef" reference specified with PNIO_rec_write_req( ) is returned. 
                                 *       This allows the IO-Base controller user program to distinguish different jobs.
                                 */
    PNIO_ERR_STAT       Err; /**< Error code on the execution of the job. 
                              * \Note "Err" is the PROFINET IO error code transferred by the IO device for the error that occurred 
                              *       when this read data record job executed on the IO device.*/
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_REC_WRITE_CONF;

/**
    \brief
    The PNIO_CBE_ALARM_IND callback event reports an incoming alarm from an IO device to the IO controller; 
    see the section "PNIO_CBF (PNIO callback function) 
    If you want to use this callback event in the IO-Base controller user program, 
    it must be registered with the PNIO_controller_open( ) function.
    \note Registration of this callback event with PNIO_controller_open( ) is optional.
        If this callback event is not registered, an alarm is confirmed internally shortly after it arrives.
    \note Alarms are confirmed to the IO device internally after this callback function is exited.
    \note Until the callback function belonging to this callback event is exited, 
    no further alarms are signaled to the IO-Base controller user program for this IO controller. 
    They are signaled later.
    \note After the device return alarm (PNIO_ALARM_DEV_RETURN) or the plug alarm (PNIO_ALARM_PLUG) arrives, 
    invalid IO data with the BAD status is transferred to the IO device. 
    For this reason, the IO-Base controller user program must write all output data 
    with valid values and the status GOOD after the return or plug alarm. 
    This corresponds to an initialization of the output data.
    With this callback event, alarms of the local Ethernet interface are signaled.
    \note As long as this callback event has not been acknowledged, no further alarms are signaled.
    \param pAddr Address of the device from which the alarm originates.
        For example, the following adresses are transferred:
        Meaning of "pAddr" in case of an IO device failure (stations failure alarm): Base address of the IO device (diagnostics address)
        Meaning of "pAddr" in case of a module failure (pull alarm): Module address
    \param IndRef Internally assigned reference (reserved)
    \param pAlarmData Structure containing information on the alarms
        Note
        "pAlarmData" points to data in the PNIO_CTRL_ALARM_DATA structure. 
        This is valid only within the callback function and becomes invalid once the function completes! 
        The data must not be changed during processing of the callback function.
        Note
        The PNIO_CTRL_ALARM_DATA structure is defined in the pniousrx.h header file and contains, 
        among other things, the parameters "Alarm type" and "Alarm specifier".
        For more detailed information on alarms, refer to the following documentation:
             STEP 7 documentation, for example in the STEP 7 online help on SFB 54.
             Excerpt from the PROFINET IO standard in the SIMATIC NET document "Alarm ASE.pdf" on the "SIMATIC NET, PC Software" CD.
             Documentation of the IO device
        \param pAddr Address of the module from which the alarm originates.
        \param IndRef Reference specified by you. This reference allows you to distinguish between several jobs. 
        You transferred this parameter with by calling "PNIO_interface_rec_read_req()".
        \param pAlarmData Structure that contains the alarm information.
            "pAlarmData" points to data of the type "PNIO_CTRL_ALARM_DATA". 
            The data buffer is only valid while the callback function is executing, 
            and it becomes invalid as soon as the function is completed.
            The PNIO_CTRL_ALARM_DATA structure is defined in the pniousrx.h header file and contains, 
            among other things, the parameters "Alarm type" and "Alarm specifier".
                STEP 7 documentation, for example in the STEP 7 online help on SFB 54.
                Documentation of the IO device
    \return No return values.
 */
typedef struct {
    PNIO_ADDR *                  pAddr; ///< Address of the device from which the alarm originates.
    PNIO_REF                     IndRef; ///< Internally assigned reference (reserved)
    const PNIO_CTRL_ALARM_DATA * pAlarmData; /**< Structure containing information on the alarms. 
                                              * \Note "pAlarmData" points to data in the PNIO_CTRL_ALARM_DATA structure. 
                                              * This is valid only within the callback function and becomes invalid once the function completes! 
                                              * The data must not be changed during processing of the callback function.
                                              * \Note The PNIO_CTRL_ALARM_DATA structure is defined in the pniousrx.h header file and contains, 
                                              *       among other things, the parameters "Alarm type" and "Alarm specifier".
                                             */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_ALARM_IND;




 /**
   \brief
    The callback event PNIO_CBE_DEV_ACT_CONF signals the status of the connection to the IO device 
    (activated or deactivated) to your IO-Base controller user program.
    The callback event must already have been registered with the PNIO_register_cbf( ) function.
    \param pAddr: Pointer to the address of the IO device to which the job was sent.
    \param Mode: Current status of the connection to the device
    \param Result: (error code) The "Result" parameter indicates that activation or deactivation 
        was successful with PNIO_OK.
 */
typedef struct {
    PNIO_ADDR *         pAddr;
    PNIO_DEV_ACT_TYPE   Mode;  
    PNIO_UINT32         Result;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_DEV_ACT_CONF;


#if !defined PNIO_SOFTNET && !defined PNIO_PNDRIVER /* not supported for PNIO SOFTNET and PNIO PNDRIVER*/
/**
    \brief
    The PNIO_CBE_START_LED_FLASH_IND callback event signals the IO-Base controller user program 
        that a request for identification has been received.
    The IO-Base controller user program can then take suitable measures to attract the attention of the operator, 
        for example by lighting up a diode. 
        The callback event must already have been registered with the PNIO_register_cbf( ) function.
    The syntax below shows the specific parameters for this event as part of the "union" 
        from the PNIO_CBE_PRM structure; see the section "PNIO_CBE_PRM (callback event parameter) 
    \note The PNIO_CBE_START_LED_FLASH_IND and PNIO_CBE_STOP_LED_FLASH_IND callbacks occur alternately 
        every three seconds as long as the module is expected to make itself noticeable. 
        When PNIO_CBE_START_LED_FLASH_IND( ) arrives, 
        the LED should flash at the frequency specified for the "Frequency" parameter.
    \param Frequency Specified frequency for signaling in hertz
 */
typedef struct {
    PNIO_UINT32         Frequency;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_LED_FLASH;
#endif /* not supported for PNIO SOFTNET and PNIO PNDRIVER */

#if !defined PNIO_SOFTNET /* not supported for PNIO SOFTNET*/

/**
    \brief 
    The PNIO_CBE_CTRL_DIAG_CONF callback event reports the result of the diagnostics request to the IO controller; 
        see also the section "PNIO_ctrl_diag_req( ) (trigger diagnostics request) 
    If you want to use this callback event in the IO-Base user program, 
        it must be registered with the PNIO_register_cbf( ) function.
    \param pDiagData Pointer to the diagnostics request - points to the diagnostics request transferred to the 
        PNIO_ctrl_diag_req( ) function; for the structure of the diagnostics request, 
        refer to the section "PNIO_CTRL_DIAG (diagnostics request) 
    \param DiagDataBufferLen Length of the diagnostics reply in "pDiagDataBuffer"
    \param pDiagDataBuffer Pointer to the buffer with the diagnostics response - Interpret the diagnostics 
        response according to the diagnostics service PNIO_CTRL_DIAG_ENUM; 
        see the section "PNIO_CTRL_DIAG_ENUM (diagnostics service) 
        Note
        This buffer is valid only within the callback.
        The structure of this buffer depends on the diagnostics request.
    \param ErrorCode Error code that occurred when executing the diagnostics service.
    \return No return values.
 */
typedef struct {
    PNIO_CTRL_DIAG *    pDiagData;
    PNIO_UINT32         DiagDataBufferLen;
    PNIO_UINT8 *        pDiagDataBuffer;
    PNIO_UINT32         ErrorCode;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_CTRL_DIAG_CONF;
#endif /* not supported for PNIO SOFTNET */

typedef struct {
     PNIO_SET_IP_NOS_MODE_TYPE Mode;
     void*                     pStationName;
     PNIO_UINT16               StationNameLen;
     PNIO_UINT8                LocalIPAddress[4];
     PNIO_UINT8                LocalSubnetMask[4];
     PNIO_UINT8                DefaultRouterAddr[4];
     PNIO_UINT32               Options;
     PNIO_UINT32               Err;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_SET_ADDR_CONF;

typedef struct {
     PNIO_UINT8 *              RemaXMLBuffer;
     PNIO_UINT32               RemaXMLBufferLength;
     PNIO_UINT32               Err;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_REMA_READ_IND;

typedef struct {
     PNIO_ADDR *        pAddr;
     PNIO_DEV_ACT_TYPE  Mode;
     PNIO_UINT32        Status;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM_IOSYS_CONF;

/**
    \brief
    The various callback events have the same data type PNIO_CBE_PRM 
        that groups the various parameters of the individual callback events using a "union".
    \param CbeType Callback event
    \param  Handle Handle from PNIO_controller_open( ) or from PNIO_device_open( ).
 */
typedef struct {
    PNIO_CBE_TYPE       CbeType;
    PNIO_UINT32         Handle;
    union {
        PNIO_CBE_PRM_MODE_IND       ModeInd;
        PNIO_CBE_PRM_REC_READ_CONF  RecReadConf;
        PNIO_CBE_PRM_REC_WRITE_CONF RecWriteConf;
        PNIO_CBE_PRM_ALARM_IND      AlarmInd;
        PNIO_CBE_PRM_DEV_ACT_CONF   DevActConf;
        #if !defined PNIO_SOFTNET && !defined PNIO_PNDRIVER 
        /* not supported for PNIO SOFTNET and PNIO PNDRIVER*/
        PNIO_CBE_PRM_LED_FLASH      LedFlash;
        #endif 
        /* not supported for PNIO SOFTNET and PNDRIVER */
        #if !defined PNIO_SOFTNET 
        /* not supported for PNIO SOFTNET */
        PNIO_CBE_PRM_CTRL_DIAG_CONF CtrlDiagConf;
        #endif 
        /* not supported for PNIO SOFTNET */
        PNIO_CBE_PRM_SET_ADDR_CONF  SetAddrConf;
        PNIO_CBE_PRM_REMA_READ_IND  RemaReadConf;
        PNIO_CBE_PRM_IOSYS_CONF     IoSystemReconf;
    } u;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_CBE_PRM;

/**
    \brief
    Using the callback event parameter PNIO_CBE_PRM, a general PNIO callback function of the type PNIO_CBF is declared.
*/
typedef void (*PNIO_CBF) (PNIO_CBE_PRM * pCbfPrm);

/* controller function specification */

#ifdef __cplusplus
extern "C" {
#endif

/**
    \brief
    Using this function, the IO-Base controller user program registers an IO controller with the IO-Base functions.
        Callback functions that process alarm events and read/write data record events can also be registered or 
        partial access to submodules (cyclic data) can be enabled with the PNIO_CEP_SLICE_ACCESS ExtPar flag.
        The following applies only to SOFTNET PN IO:
        After starting the PC, the PNIO_controller_open( ) function is permitted only on completion of the 
        SIMATIC NET configuration phase.
        If this phase is not completed, the error code PNIO_ERR_INTERNAL is returned and the call must be repeated.
        Instead, you can also evaluate the "SimaticNetPcStationUpEvent" event. For a detailed description, 
        refer to the "Commissioning PC Stations" manual. 
    \param CpIndex Module index: Used to uniquely identify the communications module. Refer to the configuration for 
        this parameter ("module index" of the CP).
    \param ExtPar  Each of the 32 bits of this parameter can be used for a parameter assignment. 
        For the significance of the individual bits, refer to the section "ExtPar (extended parameter) 
        Undefined bits are "reserved" and must not be set.
    \param cbf_rec_read_conf Address of the callback function that is started after the arrival of the read data record 
        job with the callback event type PNIO_CBE_REC_READ_CONF.
        Note 
        The function pointer must not be NULL.
    \param cbf_rec_write_conf  Address of the callback function that is started after the arrival of 
        the write data record job with the callback event type PNIO_CBE_REC_WRITE_CONF.
        Note 
        The function pointer must not be NULL.
    \param cbf_alarm_ind  Address of the callback function that is started after the arrival of an alarm with 
        the callback event type PNIO_CBE_ALARM_IND.
        Note 
        The function pointer can be NULL.
    \param Handle  Handle assigned to the registered IO controller that is returned to the IO-Base controller 
        user program if successful. 
        This must be included in all further function calls.
    \return If successful, "PNIO_OK" is returned. If an error occurs, 
        the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_CONFIG_IN_UPDATE
        - PNIO_ERR_CREATE_INSTANCE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_MAX_REACHED
        - PNIO_ERR_NO_CONFIG
        - PNIO_ERR_NO_FW_COMMUNICATION
        - PNIO_ERR_NO_LIC_SERVER (SOFTNET PNIO only)
        - PNIO_ERR_NO_RESOURCE
        - PNIO_ERR_PRM_CALLBACK
        - PNIO_ERR_PRM_CP_ID
        - PNIO_ERR_PRM_EXT_PAR
        - PNIO_ERR_PRM_HND
 */
PNIO_UINT32 /** \htmlonly */ PNIO_CODE_ATTR /** \endhtmlonly*/ PNIO_controller_open(
    PNIO_UINT32    CpIndex,            //in
    PNIO_UINT32    ExtPar,             //in
    PNIO_CBF       cbf_rec_read_conf,  //in
    PNIO_CBF       cbf_rec_write_conf, //in
    PNIO_CBF       cbf_alarm_ind,      //in
    PNIO_UINT32 *  Handle              //out
);

/**
   \brief
    With this function you can receive alarms, read data records and change the "IP Suite" and 
    also assign the station name of the local Ethernet interface.
    The function "PNIO_interface_open()" must have been called to use functions 
    (setting the IP or "NameOfStation", reading data records, receiving alarms ...) of the local Ethernet interface.
    \param CpIndex  Module index
    \param cbf_RecReadConf Address of the callback function that is called as soon as 
        the read data record job is completed. 
        The callback event type is "PNIO_CBE_IFC_REC_READ_CONF". "ZERO" can also be transferred.
    \param cbf_AlarmInd  Address of the callback function that is called as soon as an alarm is signaled. 
        The callback event type is "PNIO_CBE_IFC_ALARM_IND". "ZERO" can also be transferred.
    \param Handle Handle that is returned to the user if successful and references a local Ethernet interface. 
        This handle must be used with all following calls of the local Ethernet interface.
    \return If successful, "PNIO_OK" is returned. If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_NO_RESOURCE
        - PNIO_ERR_PRM_CALLBACK
        - PNIO_ERR_PRM_CP_ID
        - PNIO_ERR_PRM_HND
        - PNIO_ERR_SEQUENCE
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_open(
    PNIO_UINT32 CpIndex,
    PNIO_CBF cbf_RecReadConf,              //in
    PNIO_CBF cbf_AlarmInd,                 //in
    PNIO_UINT32 * Handle                   //in
);

/**
    \brief
    There is a connection to an activated IO device and there is no connection to a deactivated IO device.
    After the operating mode of the IO controller changes to the "OPERATE" mode, the configured I/O devices 
    that can be reached are signaled as activated by a station return alarm if they were activated previously.
    Description of the function
    With this function, an IO device is activated or deactivated by the IO controller.
    The function returns immediately. If the function has returned "PNIO_OK", the "PNIO_CBE_DEV_ACT_CONF"
    event is used to signal whether or not the IO device could be informed of the activation or deactivation.
    In addition to this, the actual activation or deactivation of an IO device is also signaled to the IO controller
    with a station return alarm (PNIO_ALARM_DEV_RETURN) or station failure alarm (PNIO_ALARM_DEV_FAILURE).
    The sequence of the events "PNIO_CBE_DEV_ACT_CONF" and the station failure or return alarms over time is not predictable.
    \note Parallel jobs with PN Driver
        Up to eight activation or deactivation requests in total could be handled in parallel. 
        The callback function is called for each request after the handling is completed.
    \note Note that a deactivated IO device may only be disconnected or turned off after arrival of the callback 
        for "PNIO_ALARM_DEV_FAILURE" and the "PNIO_CBE_DEV_ACT_CONF" callback.
    \param Handle  Handle from "PNIO_controller_open()"
    \param pAddr  Any address of the IO device to which the job is sent.
        Explanation
        Each IO device can have several modules and therefore also have several addresses.
        It is adequate to address one of these IO device addresses to activate or deactivate the entire IO device.
        Note 
            With the CP 1626 it is not the address of the module that is used but the hardware identifier of 
            the STEP 7 configuration.
    \param DeviceMode  The "DeviceMode" parameter can adopt two states:
        - PNIO_DA_FALSE deactivates the IO device.
        - PNIO_DA_TRUE activates the IO device.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_DEV_ACT_NOT_ALLOWED
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_MODE_VALUE
    - PNIO_ERR_NO_FW_COMMUNICATION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_DEV_STATE
    - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_device_activate(
    PNIO_UINT32    Handle,           //in
    PNIO_ADDR *    pAddr,            //in
    PNIO_DEV_ACT_TYPE DeviceMode     //in
);

/**
    \brief
    With this function you can change to a new mode.
    The function returns immediately.
    The actual status change is signaled with the PNIO_CBE_MODE_IND event if the function returned PNIO_OK. 
    The user may only call PNIO_set_mode( ) again when a previous PNIO_set_mode( ) job was confirmed 
    with PNIO_CBE_MODE_IND( ).
    \param Handle  Handle from PNIO_controller_open( )
    \param Mode New mode to be set; see section "PNIO_MODE_TYPE (operating mode type) 
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_MODE_VALUE
    - PNIO_ERR_NO_FW_COMMUNICATION
    - PNIO_ERR_SET_MODE_NOT_ALLOWED
    - PNIO_ERR_WRONG_HND
    - PNIO_ERR_SEQUENCE
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_set_mode(
    PNIO_UINT32    Handle,           //in
    PNIO_MODE_TYPE Mode              //in
);

/**
    \brief
    With this function, the IO controller sends a read data record job.
        The result of the job is signaled by the "PNIO_CBE_REC_READ_CONF" callback 
        even if the function returned "PNIO_OK".
        With this function, you can also read diagnostics 
        data of an IO device stored in data records. Please read the description of the relevant IO devices.
        \note Parallel jobs with PN Driver
        Up to 16 requests in total could be handled in parallel.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr Address of the module of the IO device for which the read data record job is intended.
        Notice 
        With the CP 1626 it is not the address of the module that is used 
        but the hardware identifier of the STEP 7 configuration.
        Note 
        If the address (pAddr) is a diagnostics address, the parameter "IO-DataType" must be set to Input.
        Note 
        The "IODataType" parameter is described in the section "PNIO_ADDR (address structure) 
    \param RecordIndex  Data record number
    \param ReqRef  Reference assigned by the IO-Base controller user program.
        Note 
        The "ReqRef" reference allows the IO-Base controller user program to distinguish different jobs.
        This parameter can be assigned with any value by the IO-Base controller user program 
        and is returned with the callback event "PNIO_CBE_REC_READ_CONF".
    \param Length  Maximum number of bytes to be read with this request. Maximum value is 32768. The pointer to the data record and
        its real length are returned by the callback event "PNIO_CBE_REC_READ_CONF".
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_NO_FW_COMMUNICATION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_REC_INDEX 
    - PNIO_ERR_VALUE_LEN
    - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_rec_read_req(
    PNIO_UINT32    Handle,            //in
    PNIO_ADDR *    pAddr,             //in
    PNIO_UINT32    RecordIndex,       //in
    PNIO_REF       ReqRef,            //in
    PNIO_UINT32    Length             //in
);

/**
    \brief
    With this function, the IO controller sends a write data record job. 
    The result of the job is signaled by the PNIO_CBE_REC_WRITE_CONF callback even if the function returned PNIO_OK.
    \note Parallel jobs with PN Driver
        Up to 16 requests in total could be handled in parallel.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr   Address of the module of the IO device for which the read data record job is intended. 
        Note 
        If the address (pAddr) is a diagnostics address, the parameter "IODataType" must be set to Input.
        Note 
        The "IODataType" parameter is described in the section "PNIO_ADDR (address structure) 
    \param RecordIndex Data record number
    \param ReqRef Reference assigned by the IO-Base controller user program.
        Note 
        The "ReqRef" reference allows the IO-Base controller user program to distinguish different jobs. 
        This parameter can be assigned with any value by the IO-Base controller user program 
        and is returned in the callback event "PNIO_CBE_REC_WRITE_CONF".
    \param Length Length of the data buffer (in bytes). 
        The maximum length depends on the version and the product used. You can find it in the "readme.txt" file.
    \param pBuffer Data buffer containing the data record.
        Note 
        The caller of the "PNIO_rec_write_req()" function must make 
        the "pBuffer" data buffer available that contains the data record to be written.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_NO_FW_COMMUNICATION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_REC_INDEX
    - PNIO_ERR_VALUE_LEN
    - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_rec_write_req(
    PNIO_UINT32    Handle,            //in
    PNIO_ADDR *    pAddr,             //in
    PNIO_UINT32    RecordIndex,       //in
    PNIO_REF       ReqRef,            //in
    PNIO_UINT32    Length,            //in
    PNIO_UINT8 *   pBuffer            //in
);

#if !defined PNIO_SOFTNET /* not supported for PNIO SOFTNET */
/**
    \brief
    This function triggers a diagnostics request on the IO controller. 
    The result of the job is signaled by the PNIO_CBE_CTRL_DIAG_CONF callback event if the function returned PNIO_OK.
    If the IO-Base controller user program wants to use this function, 
    it must first register the PNIO_CBE_CTRL_DIAG_CONF callback event with the PNIO_register_cbf( ) function.
    \param Handle  Handle from PNIO_controller_open( )
    \param pDiagReq  Diagnostics request - Pointer to a filled PNIO_CTRL_DIAG structure containing 
        the diagnostics request job; for the structure of the diagnostics request, 
        refer to the section "PNIO_CTRL_DIAG (diagnostics request) 
        Note
        The memory to which "pDiagReq" points remains occupied by the IO-Base interface until the confirmation 
        is received. This is signaled by the PNIO_CBE_CTRL_DIAG_CONF( ) callback event.
        As soon as the diagnostics request result is signaled with the PNIO_CBE_CTRL_DIAG_CONF callback event, 
        the memory is returned to the IO-Base controller user program.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_NO_FW_COMMUNICATION
    - PNIO_ERR_WRONG_HND
    - PNIO_ERR_PRM_POINTER
    - PNIO_ERR_PRM_INVALIDARG
    - PNIO_ERR_PRM_ADD
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_ctrl_diag_req(
    PNIO_UINT32    Handle,            //in
    PNIO_CTRL_DIAG * pDiagReq         //in
);
#endif /* not supported for PNIO SOFTNET */

/**
    \brief
    This function makes it possible to reconfigure a PROFINET IO System during runtime using the feature "Configuration Control".
        The product CD contains an example of an implementation of this function.
    \param Handle  Handle from PNIO_controller_open()
    \param Mode  Specifies the mode to be executed.
        - PNIO_IOS_RECONFIG_MODE_DEACT: All IO devices are deactivated to avoid diagnostics messages
        - PNIO_IOS_RECONFIG_MODE_TAILOR: Starts "tailoring" with the parameters specified in the "DeviceList" and
            "PortInterconnectionList" and then activates all IO devices that are absolutely necessary or optional 
            and also included in the "DeviceList".
    \param DeviceCnt  Number of entries contained in the "DeviceList"
    \param DeviceList  List of logical addresses of the optional IO devices that exist in the real setup.
    \param PortInterconnectionCnt  Number of entries contained in the"PortInterconnectionList"
    \param PortInterconnectionList  List of logical addresses of the port interconnections.
        Each port interconnection is made up or a pair of logical addresses
        (logical address device1/port2, logical address device2/port1)
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_MODE_VALUE
    - PNIO_ERR_TAILOR_INV_STATION
    - PNIO_ERR_TAILOR_INV_PORT
    - PNIO_ERR_NOT_POSSIBLE
    - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_iosystem_reconfig(
    PNIO_UINT32               Handle,                     //in
    PNIO_IOS_RECONFIG_MODE    Mode,                       //in
    PNIO_UINT32               DeviceCnt,                  //in
    PNIO_ADDR*                DeviceList,                 //in
    PNIO_UINT32               PortInterconnectionCnt,     //in
    PNIO_ADDR*                PortInterconnectionList     //in
);

#ifdef PNIO_SOFTNET
/**
    * \fn PNIO_controller_close
    \brief 
    Using this function, the IO-Base controller user program deregisters an IO controller.
        All registered callback functions are deregistered (including the callback functions registered with PNIO_register_cbf( )).
        On completion of the PNIO_controller_close( ) function, no further callback functions will be called for the deregistered IO controller.
    \note While the PNIO_controller_close( ) function executes, callback functions can also be executed.
    
    \note After successfully deregistering, the handle on the CP is no longer valid and 
        can no longer be used by the IO-Base controller user program.
    
    \note Before exiting the IO-Base controller user program, the PNIO_controller_close( ) function must be called.
    
    \note If the CP is operated as an IO controller and IO device at the same time in the same user program, 
        before deregistering the IO controller/IO device, 
        the writing and reading of IO data of the IO device/IO controller must also be stopped.
    \param Handle  Handle from PNIO_controller_open( )
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_NO_FW_COMMUNICATION
        - PNIO_ERR_WRONG_HND
 */
#define PNIO_controller_close PNIO_close
PNIO_UINT32 PNIO_CODE_ATTR PNIO_close(
    PNIO_UINT32 Handle  //in
);
#else
#define PNIO_close PNIO_controller_close
PNIO_UINT32 PNIO_CODE_ATTR PNIO_controller_close(
    PNIO_UINT32 Handle          //in
);

/**
    \brief
    With this function you close the local Ethernet interface. 
    No registered callback function will be called afterwards.
    \note Until execution of "PNIO_interface_close()" is completed, callback functions can still be called.
    \note After closing the local Ethernet interface, 
    the handle supplied with "PNIO_interface_open()" becomes invalid and can no longer be used.
    \param Handle  Handle from "PNIO_interface_open()"  
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_close(
    PNIO_UINT32 Handle
);
#endif

/**
    \brief
    This function reads input data from the process image. The function then returns immediately. 
        The process image is kept up to date by the IO controller cyclically regardless of this read job.
        The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    \note The PNIO_data_read( ) function reads the data of a single submodule. 
        As a result, data consistency beyond the limits of the submodule 
        can only be achieved for IRT data accessed in isochronous real-time mode.
    \note This function is not reentrant. 
        Make sure that access is only from a thread context or that a locking strategy is used.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr  Logical address of the submodule. With partial access, this can be any address of the submodule.
        Example: 
        4 bytes input. Address 100 to 103 Valid addresses: 100, 101, 102, 103
        Note  On the IO controller, "pAddr" is the address of the remote submodule from which data will be read.
    \param BufLen  Length of the data buffer made available (in bytes). 
        This parameter specifies how many bytes will be read.
    Less bytes than available can be read (in other words, the number of bytes from the specified address to the end of the module).
    \param pDataLen  Length of the read data buffer (in bytes)
        If the length of the IO device data is less than or equal to "BufLen", 
        "*pDataLen" contains the length of the IO data that was read.
        If the length of the IO device data is greater than the data buffer available, 
        as much data as possible is read into the data buffer.
        "*pDataLen", however, contains the length of the data buffer that would have been necessary to read all IO data.
    \param pBuffer  Pointer to the data buffer of the read IO data.
    \param IOlocState  Local status of the read IO data (GOOD or BAD).
        Note  The IO-Base controller user program specifies the local status using "IOlocState".
        "IOlocState" is sent by the underlying IO-Base functions to the remote device in the next cycle.
        The BAD status informs the communications partner that the IO-Base controller user program could not 
        process the received IO data.
        Partial access: 
        With each partial access, the transferred status ("IOlocState") applies to the entire submodule.
    \param pIOremState  Remote status of the read IO data (GOOD or BAD). 
        Indicates the quality/validity of the read data.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_NO_CONNECTION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_IO_TYPE
    - PNIO_ERR_PRM_LEN
    - PNIO_ERR_PRM_LOC_STATE
    - PNIO_ERR_PRM_RSTATE
    - PNIO_ERR_UNKNOWN_ADDR
    - PNIO_ERR_WRONG_HND
    - PNIO_WARN_IRT_INCONSISTENT
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_read(
    PNIO_UINT32   Handle,            //in
    PNIO_ADDR *   pAddr,             //in
    PNIO_UINT32   BufLen,            //in
    PNIO_UINT32 * pDataLen,          //out
    PNIO_UINT8 *  pBuffer,           //out
    PNIO_IOXS     IOlocState,        //in
    PNIO_IOXS *   pIOremState        //out
);

#if !defined PNIO_SOFTNET && !defined PNIO_PNDRIVER /* not supported for PNIO SOFTNET and PNIO PNDRIVER*/
/**
    \brief Brief
    This function reads output data from the process image. The function then returns immediately. 
        The process image is kept up to date by the IO controller cyclically regardless of this read job.
        The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    \note The PNIO_output_data_read( ) function reads the data of a single submodule. As a result, 
         data consistency beyond the limits of the submodule can only be achieved for IRT data accessed in isochronous real-time mode.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr  Logical address of the submodule. With partial access, this can be any address of the submodule.
         Example: 
        4 bytes input. Address 100 to 103 Valid addresses: 100, 101, 102, 103
        Note  On the IO controller, "pAddr" is the address of the remote submodule from which data will be read.
    \param BufLen  Length of the data buffer made available (in bytes).
        This parameter specifies how many bytes will be read. 
        Less bytes than available can be read (in other words, the number of bytes from the specified address to the end of the module).
    \param pDataLen  Length of the read data buffer (in bytes)
        If the length of the IO device data is less than or equal to "BufLen", 
        "*pDataLen" contains the length of the IO data that was read.
        If the length of the IO device data is greater than the data buffer available, 
        as much data as possible is read into the data buffer.
        "*pDataLen", however, contains the length of the data buffer that would have been necessary to read all IO data.
    \param pBuffer  Pointer to the data buffer of the read IO data.
    \param pIOlocState  Local status of the read IO data (GOOD or BAD).
    Note  The IO-Base controller user program specifies the local status using "IOlocState".
        "IOlocState" is sent by the underlying IO-Base functions to the remote device in the next cycle.
        The BAD status informs the communications partner that the IO-Base controller user program could not 
        process the received IO data.
        Partial access:
        With each partial access, the transferred status ("IOlocState") applies to the entire submodule.
    \param pIOremState  Remote status of the read IO data (GOOD or BAD). 
        Indicates the quality/validity of the read data.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_NO_CONNECTION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_IO_TYPE
    - PNIO_ERR_PRM_LEN
    - PNIO_ERR_PRM_LOC_STATE
    - PNIO_ERR_PRM_RSTATE
    - PNIO_ERR_UNKNOWN_ADDR
    - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_output_data_read(
    PNIO_UINT32   Handle,            //in
    PNIO_ADDR *   pAddr,             //in
    PNIO_UINT32   BufLen,            //in
    PNIO_UINT32 * pDataLen,          //out
    PNIO_UINT8 *  pBuffer,           //out
    PNIO_IOXS *   pIOlocState,       //out
    PNIO_IOXS *   pIOremState        //out
);

/**
    \brief
    With this function, your IO-Base controller user program initiates the transfer of the 
        following data from the process image on the CP to the read cache:
        - RT IO input data and its remote status
        - The remote status of the RT IO output data
    The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    This function does not transfer any IRT IO data to the read cache; 
        see also the section "Isochronous real-time and non-isochronous access to cyclic IO data (IRT)
    \param Handle  Handle from PNIO_controller_open( )
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_read_cache_refresh(
        PNIO_UINT32 Handle               //in
);

/**
    \brief
    This function reads RT input data from the read cache and writes the local status of the RT output data to the write cache.
        The function then returns immediately. The process image is kept up to date by the IO controller cyclically regardless of this read job.
    The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    This function does not transfer any IRT IO data to the read cache; see also the section "Isochronous real-time and non-isochronous access to cyclic IO data (IRT)
    \note The PNIO_data_read_cache( ) reads the data of a single submodule.
        As a result, data consistency beyond the limits of the submodule can only be achieved for IRT data accessed in isochronous real-time mode.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr  Logical address of the submodule. With partial access, this can be any address of the submodule.
        Example: 
        4 bytes input. Address 100 to 103 Valid addresses: 100, 101, 102, 103
        Note On the IO controller, "pAddr" is the address of the remote submodule from which data will be read.
    \param BufLen  Length of the data buffer made available (in bytes).
        This parameter specifies how many bytes will be read. 
        Less bytes than available can be read (in other words, the number of bytes from 
        the specified address to the end of the module).
    \param pDataLen  Length of the read data buffer (in bytes)
        If the length of the IO device data is less than or equal to "BufLen", 
        "*pDataLen" contains the length of the IO data that was read.
        If the length of the IO device data is greater than the data buffer available, 
        as much data as possible is read into the data buffer.
        "*pDataLen", however, contains the length of the data buffer that would have been necessary to read all IO data.
    \param pBuffer  Pointer to the data buffer of the read IO data.
    \param IOlocState  Local status of the read IO data (GOOD or BAD).
        Note The IO-Base controller user program specifies the local status using "IOlocState".
        "IOlocState" is sent by the underlying IO-Base functions to the remote device in the next cycle.
        The BAD status informs the communications partner that the IO-Base controller user program 
        could not process the received IO data.
        Partial access:
        With each partial access, the transferred status ("IOlocState") applies to the entire submodule.
    \param pIOremState  Remote status of the read IO data (GOOD or BAD). 
        Indicates the quality/validity of the read data.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_INTERNAL
    - PNIO_ERR_NO_CONNECTION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_IO_TYPE
    - PNIO_ERR_PRM_LEN
    - PNIO_ERR_PRM_LOC_STATE
    - PNIO_ERR_PRM_RSTATE
    - PNIO_ERR_UNKNOWN_ADDR
    - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_read_cache(
    PNIO_UINT32 Handle,              //in
    PNIO_ADDR * pAddr,               //in
    PNIO_UINT32 BufLen,              //in
    PNIO_UINT32 * pDataLen,          //out
    PNIO_UINT8 * pBuffer,            //out
    PNIO_IOXS   IOlocState,          //in
    PNIO_IOXS * pIOremState          //out
);
#endif /* not supported for PNIO SOFTNET and PNIO PNDRIVER */




/**
    \brief
    With this function, your IO-Base controller user program writes IO data to the process image. 
        The function then returns immediately. 
        The IO controller transfers the IO data to the IO device in the next cycle.
    The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    \note The PNIO_data_write( ) function writes the data of a single submodule. 
         As a result, data consistency beyond the limits of the submodule can only 
         be achieved for IRT data accessed in isochronous real-time mode.
    \note This function is not reentrant. 
        Make sure that access is only from a thread context or that a locking strategy is used.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr  Logical address of the submodule.
        Note On the IO controller, "pAddr" is the address of the remote submodule to 
        which data will be written.
        With partial access, this can be any logical address of the submodule. 
        The data length (BufLen) must be adapted accordingly. 
        Maximum up to end of module (see section "ExtPar (extended parameter) 
    \param BufLen  Length of the data buffer made available (in bytes). 
        This parameter specifies how many bytes will be written. 
        Less bytes can be written than are available from the specified address up to the end of the module.
        Note 
        Partial access not enabled:
        "BufLen" must match the length of the output data of the addressed module exactly, 
        otherwise an error is reported in the return code.
        Partial access enabled:
        "BufLen" can be less than or equal to the length of the output data of the addressed module.
        The maximum data length is the number of bytes available from the specified address to the end of the module.
   \param pBuffer Pointer to the data buffer of the IO data to be written.
   \param IOlocState Local status of the IO data to be written (GOOD or BAD).
        Partial access enabled:
        The last transferred status ("IOIocState") applies to the entire submodule. 
        In other words, if a partial status has the BAD status, all data of the module have the BAD status.
    \param pIOremState Remote status, the returned status of the written IO data (GOOD or BAD).
    Note 
        This status relates to IO data written earlier and not to the write job that has just been sent.
    Note 
        With BAD, the remote device can signal to the IO-Base controller user program that 
        it could not process the previously received IO data.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
       (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_NO_CONNECTION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_IO_TYPE
    - PNIO_ERR_PRM_LEN
    - PNIO_ERR_PRM_LOC_STATE
    - PNIO_ERR_PRM_RSTATE
    - PNIO_ERR_UNKNOWN_ADDR
    - PNIO_ERR_VALUE_LEN
    - PNIO_ERR_WRONG_HND
    - PNIO_WARN_IRT_INCONSISTENT
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_write(
    PNIO_UINT32   Handle,            //in
    PNIO_ADDR *   pAddr,             //in
    PNIO_UINT32   BufLen,            //in
    PNIO_UINT8 *  pBuffer,           //in
    PNIO_IOXS     IOlocState,        //in
    PNIO_IOXS *   pIOremState        //out
);

#if !defined PNIO_SOFTNET && !defined PNIO_PNDRIVER /* not supported for PNIO SOFTNET and PNIO PNDRIVER*/
/**
    \brief
    With this function, your IO-Base controller user program initiates the 
        transfer of the following data from the write cache to the process image on the CP:
        - RT IO output data and its local status.
        - The local status of the RT IO input data.
    The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    This function does not transfer any IRT IO data from the write cache; 
    see also the section "Isochronous real-time and non-isochronous access to cyclic IO data (IRT) 
    \param Handle  Handle from PNIO_controller_open( )
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_write_cache_flush(
    PNIO_UINT32   Handle              //in
);

/**
    \brief
    With this function, your IO-Base controller user program writes RT output data to the write cache and 
        reads the remote status of the RT input data from the read cache. The data remains in the write cache 
        until the next PNIO_data_write_cache_flush( ) function call.
    The basic mechanisms for this are described in the section "Non-isochronous access to cyclic IO data (RT)
    This function is transparent for IRT IO data; see also the section "Isochronous real-time and non-isochronous access to cyclic IO data (IRT) 
    \note The PNIO_data_write_cache( ) functions always relate to the data of one submodule. As a result, 
        data consistency beyond the limits of the submodule can only be achieved for IRT data accessed in isochronous real-time mode.
    \param Handle  Handle from PNIO_controller_open( )
    \param pAddr  Logical address of the submodule.
        Note On the IO controller, "pAddr" is the address of the remote submodule to 
        which data will be written.
        With partial access, this can be any logical address of the submodule. 
        The data length (BufLen) must be adapted accordingly. 
        Maximum up to end of module (see section "ExtPar (extended parameter) 
    \param BufLen  Length of the data buffer (in bytes).
        This parameter specifies how many bytes will be written. 
        Less bytes can be written than are available from the specified address up to the end of the module.
        Note 
        Partial access not enabled: 
        "BufLen" must match the length of the output data of the addressed module exactly, 
        otherwise an error is reported in the return code.
        Partial access enabled: 
        "BufLen" can be less than or equal to the length of the output data of the addressed module.
        The maximum data length is the number of bytes available from the specified address to the end of the module.
    \param pBuffer  Pointer to the data buffer of the IO data to be written.
    \param IOlocState Local status of the IO data to be written (GOOD or BAD).
        Partial access enabled: 
        The last transferred status ("IOlocState") applies to the entire submodule.
        In other words, if a partial status has the BAD status, all data of the module has the BAD status.
    \param pIOremState Remote status, the returned status of the written IO data (GOOD or BAD).
        Note 
        This status relates to IO data written earlier and not to the write job that has just been sent.
        Note 
        With BAD, the remote device can signal to the IO-Base controller user program that 
        it could not process the previously received IO data.
        Note 
        This status is transferred only after the PNIO_data_read_cache_refresh( ) call.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
       (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_NO_CONNECTION
    - PNIO_ERR_PRM_ADD
    - PNIO_ERR_PRM_BUF
    - PNIO_ERR_PRM_IO_TYPE
    - PNIO_ERR_PRM_LEN
    - PNIO_ERR_PRM_LOC_STATE
    - PNIO_ERR_PRM_RSTATE
    - PNIO_ERR_UNKNOWN_ADDR
    - PNIO_ERR_WRONG_HND
    - PNIO_ERR_VALUE_LEN
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_write_cache(
    PNIO_UINT32   Handle,            //in
    PNIO_ADDR*    pAddr,             //in
    PNIO_UINT32   BufLen,            //in
    PNIO_UINT8*   pBuffer,           //in
    PNIO_IOXS     IOlocState,        //in
    PNIO_IOXS*    pIOremState        //out
);
PNIO_UINT32 PNIO_CODE_ATTR PNIO_data_cache_exchange(
    PNIO_UINT32 Handle
);
#endif /* not supported for PNIO SOFTNET and PNIO PNDRIVER */

/**
    \brief
    This function registers a callback function.
    \note Callback functions can only be registered in offline mode. 
    \param Handle  Handle from PNIO_controller_open( )
    \param CbeType  Callback event type for which the callback function "Cbf" will be registered; 
        see the section PNIO_CBE_TYPE (callback event type).
    \param Cbf  Address of the callback function to be started after arrival of the callback event "CbeType".
        Note 
        - The function pointer must not be NULL.
        - If a callback function is already registered for a callback event type, a further callback
            function can only be registered after PNIO_controller_close( ).
    \return If successful, "PNIO_OK" is returned. If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_ALLREADY_DONE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_MODE_VALUE
        - PNIO_ERR_PRM_CALLBACK
        - PNIO_ERR_PRM_TYPE
        - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_register_cbf(
    PNIO_UINT32   Handle,            //in
    PNIO_CBE_TYPE CbeType,           //in
    PNIO_CBF      Cbf                //in
);




/**
    \brief
    With this function, you register callback functions with the Ethernet
        interface after you have called the "PNIO_interface_open()" function.
    \param Handle   Handle from "PNIO_interface_open()"
    \param CbeType  Callback event type for which the callback function will be registered.
    \param Cbf   Address of the callback function that is to be called. 
        Note 
        The function pointer "Cbf" must not be "NULL".
        If a function pointer is already registered, no further one can be registered
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_ALLREADY_DONE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_PRM_CALLBACK
        - PNIO_ERR_PRM_TYPE
        - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_register_cbf(
    PNIO_UINT32 Handle,         //in
    PNIO_CBE_TYPE CbeType,      //in
    PNIO_CBF Cbf                //in
);

/**
   \brief
    With this function you can read data records from the local Ethernet interface.
    \param Handle  Handle from "PNIO_interface_open()"
    \param pAddr  Address of a submodule of the local Ethernet interface on which the read data record job will be executed.
    \param RecordIndex  Data record index
    \param ReqRef  Reference specified by you. This reference allows you to distinguish between several jobs. 
        You can set any value for this parameter and it is returned with the callback event "PNIO_CBE_IFC_REC_READ_CONF".
    \param Length  Specifies the maximum record length of 4096 bytes. 
        The pointer to the data and the real length are returned by the callback event "PNIO_CBE_IFC_REC_READ_CONF"
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_PRM_ADD
        - PNIO_ERR_PRM_REC_INDEX
        - PNIO_ERR_VALUE_LEN
        - PNIO_ERR_WRONG_HND
        - PNIO_ERR_NO_RESOURCE
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_rec_read_req(
    PNIO_UINT32 Handle,                                 //in
    PNIO_ADDR * pAddr,                                  //in
    PNIO_UINT32 RecordIndex,                            //in
    PNIO_REF ReqRef,                                    //in
    PNIO_UINT32 Length                                  //in
);

/**
    \brief
    With this function, you can change the "IP Suite" and / or the station name of the local Ethernet interface.
    \note The requirement for using this function is that the hardware configuration has been specified, 
         that the IP address or the station name may be adapted locally.
    \param Handle  Handle from "PNIO_interface_open()"
    \param Mode 
        - PNIO_SET_IP_MODE (0x0001): The IpV4 is to be changed.
        - PNIO_SET_NOS_MODE (0x0002): The station name is to be changed.
    \param IPv4  "IP Suite" to be changed.
    \param NoS  Station name to be changed.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_WRONG_HND
        - PNIO_ERR_MODE_VALUE
        - PNIO_ERR_NOT_REENTERABLE
        - PNIO_ERR_PRM_NOS_LEN
        - PNIO_ERR_SET_IP_NOS_NOT_ALLOWED
        - PNIO_ERR_PRM_IP PNIO_ERR_PRM_NOS
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_set_ip_and_nos(
    PNIO_UINT32               Handle,               //in
    PNIO_SET_IP_NOS_MODE_TYPE Mode,                 //in
    PNIO_IPv4                 IPv4,                 //in
    PNIO_NOS                  NoS                   //in
);
#ifdef __cplusplus
}
#endif

/** PROFI_ENERGY: This macro must be defined. Do not undefine it.
 *   PROFIenerg functionality is a part of PNIO Controller Ifc
 *   in Version V2.5.2 and higher.
 */
#ifndef PROFI_ENERGY
#define PROFI_ENERGY
#endif

#ifdef PROFI_ENERGY
/** PE (PROFIenergy) Extensions: PE data types and functions
 */

/** Config Data Record:
 * PE Parameter Record  to configure PROFIenergy Power Moduls.
 * Standard PNIO_rec_write_req() is used to configure a PE device:
 * Siemens IM 151-3BA23 Version 01 parameters
 */
#define PNIO_PE_CFG_DEV_NUM_SLOTS_MAX     8
#define PNIO_PE_CFG_SLOT_EMPTY            0x00
#define PNIO_PE_CFG_SLOT_MODE_POWER_ON    0x00
#define PNIO_PE_CFG_SLOT_MODE_POWER_OFF   0x01

#if defined(_MSC_VER)
 #pragma pack( push, safe_usrx_prev_packing, 2 )
 //#pragma pack(show)
 #define ATTR_PACKED
#elif defined(__GNUC__)
 #undef ATTR_PACKED
 #define ATTR_PACKED  __attribute__ ((aligned(2))) __attribute__ ((packed))
#endif

#if 0
// ----------  Block header is not required - is set by the firmware
    PNIO_UINT16         block_type;    /* 0x0800 RecordDataWrite, 0x0801 RecordDataRead  */
    PNIO_UINT16         block_length;  /* Num bytes without counting the fields BlockType and BlockLength   */
    PNIO_UINT8          version_high;  /* 0x01 */
    PNIO_UINT8          version_low;   /* 0x00 */
#endif


/**
    \brief
    The structure of the PE configuration data record is vendor-specific.
    Before using the PE functions, the IO device needs to be configured appropriately. 
    The configuration is based on a PE configuration data record and 
    the "PNIO_rec_write_req( )" request (write record job).
    \note The power modules (PM) of an ET200S PE device are configured with the data record described below.
    The configuration parameters are transferred to the ET200S PE with 
    the write record function "PNIO_rec_write_req( )" with "RecordIndex" = 3.
    \param slot_num Slot of the power module to be turned off 1 to 62
        0: An entry in this field is not relevant
    \param power_mode 0: Continue working (do not turn off module)
        1: Turn off
    \param reserved1 Not used
    \param num_cfg_slots Number of Slot-Config blocks
    \param slot[num_cfg_slots] Field of "Slot-Config blocks" ((PNIO_PE_CFG_SLOT)) with length "num_cfg_slots".
        Max: PNIO_PE_CFG_DEV_NUM_SLOTS_MAX 8
*/
typedef struct {
    PNIO_UINT8          slot_num;     /**< 0=empty  1...62= valid slot numbers */
    PNIO_UINT8          power_mode;   /**< 0x00=power on, 0x01=power off during power save mode */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_CFG_SLOT;



typedef struct {
    PNIO_UINT8          reserved1;                            /**< reserved for internal use - version num */
    PNIO_UINT8          num_cfg_slots;                        /**< current number of slots in the array    */
    PNIO_PE_CFG_SLOT    slot[PNIO_PE_CFG_DEV_NUM_SLOTS_MAX];  /**< array of slot config blocks             */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_CONFIG_DEVICE_REQ;

/* Alignment test: sizeof PNIO_PE_PRM_CONFIG_DEVICE_REQ must be == 18
*/
typedef char _size_tst_config_dev_rec_ [ ((sizeof(PNIO_PE_PRM_CONFIG_DEVICE_REQ) == 18) ? 1 : -1) ];

#if defined(_MSC_VER)
 #pragma pack( pop, safe_usrx_prev_packing )
#elif defined(__GNUC__)
 #undef ATTR_PACKED
 #define ATTR_PACKED  __attribute__ ((aligned(4)))
#endif

/* PE (PROFIenergy service related parameters: */

/* PE_SAP_DR_INDEX : subslot specific PROFIenergy Service Access Point 'Data Record Index'
 * PE_DR_INDEX_MASK: 0x80A1..0x80AF: subslot specific record data reserved for further use
 */
#define PE_SAP_DR_INDEX   0x80A0
#define PE_DR_INDEX_MASK  0xFFF0    /* PL: 19.09.2014: Fix: PR 1113532: 0x80A0 ---> 0xFFF0 */

/* PNIO_PE_SERVICE_REQ_LIFETIME_DEFAULT: Time in sec. to wait for Service Response
*/
#define PNIO_PE_SERVICE_REQ_LIFETIME_DEFAULT     10   /* [sec] */

/**
    \brief
    This enumeration type lists the PE jobs. This means both requests and confirmations.
    \param PNIO_PE_CMD_START_PAUSE Start energy saving mode
    \param PNIO_PE_CMD_END_PAUSE End energy saving mode
    \param PNIO_PE_CMD_Q_MODES Returns information about the energy saving mode depending on the CmdModifier
    \param PNIO_PE_CMD_PEM_STATUS Returns the current energy saving mode
    \param PNIO_PE_CMD_PE_IDENTIFY Returns a list of all supported PE jobs
    \param PNIO_PE_CMD_Q_MSRMT Depending on the CmdModifier, returns information about the energy consumption
    \param PNIO_PE_CMD_RESERVED
*/
/* PE Service Request IDs ==Command IDs */
typedef enum {
    PNIO_PE_CMD_RESERVED            = 0,
    PNIO_PE_CMD_START_PAUSE         = 1,
    PNIO_PE_CMD_END_PAUSE           = 2,
    PNIO_PE_CMD_Q_MODES             = 3,
    PNIO_PE_CMD_PEM_STATUS          = 4,
    PNIO_PE_CMD_PE_IDENTIFY         = 5,
    PNIO_PE_CMD_Q_MSRMT             = 0x10,
} PNIO_PE_CMD_ENUM;

/**
    \brief
    This enumeration type lists the PE command modifiers required to define the precise job type with "group jobs". 
    PE command modifiers are used with requests. PE job modifier is an element of the PNIO_PE_REQ_PRM structure. 
    See section "PNIO_PE_REQ_PRM 
    With confirmations, this function is handled by the data structure identifier ( struc_id ). 
    See section "PNIO_PE_CBE_PRM (callback event parameter) 
    \note Group jobs are the PNIO_PE_CMD_Q_MODES (query modes) and PNIO_PE_CMD_Q_MSRMT (query measurement) jobs.
    \param PE_CMD_MODIF_NOT_USED For all unique jobs. These do not require a modifier.
    \param PE_CMD_MODIF_Q_MODE_LIST_ALL With PNIO_PE_CMD_Q_MODES job. Returns a list of all available energy saving modes.
    \param PE_CMD_MODIF_Q_MODE_GET_MODE With PNIO_PE_CMD_Q_MODES job. Returns data of an energy saving mode.
    \param PE_CMD_MODIF_Q_MSRMT_LIST_ALL With PNIO_PE_CMD_Q_MSRMT job. Returns a list of all available measured values 
        (list of measured value IDs)
    \param PE_CMD_MODIF_Q_MSRMT_GET_VAL With PNIO_PE_CMD_Q_MSRMT job. Returns data of a measured value.
 */
/* PE Command Modifiers */
typedef enum {
    PE_CMD_MODIF_NOT_USED           = 0,
    PE_CMD_MODIF_Q_MODE_LIST_ALL    = 1,
    PE_CMD_MODIF_Q_MODE_GET_MODE    = 2,
    PE_CMD_MODIF_Q_MSRMT_LIST_ALL   = 1,
    PE_CMD_MODIF_Q_MSRMT_GET_VAL    = 2,
} PNIO_PE_CMD_MODIFIER_ENUM;

/*--- PE Service Request Parameters ---*/

/**
    \brief
    Parameter for the PNIO_PE_CMD_START_PAUSE PE job that triggers the start of a break in operation.
    \param time_ms Specifies the expected duration of the break in milliseconds. 
        If this time is too short for an IO device, this IO device does not pause operation.
*/
typedef struct {
    PNIO_UINT32         time_ms;     /**< requested pause time in ms */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_START_PAUSE_REQ;

/**
    \brief
    Parameter of the callback event that confirms the start of the break in operation.
    \param pe_mode_id Mode reached
        0x00: Energy saving mode
        0x01...0xFE: Vendor-specific
        0xFF: Ready to function
*/
typedef struct {
    PNIO_UINT32         pe_mode_id;  /**< energy-saving mode selected by the device */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_START_PAUSE_CONF;

/**
    \brief
    Parameter of the callback event that confirms the end of the break in operation.
    \param time_ms Time required in milliseconds to reach the ready to operate status.
*/

typedef struct {
    PNIO_UINT32         time_ms;     /**< time to leave energy-saving mode */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_END_PAUSE_CONF;

 /**
    \brief
    Parameter of the callback event that returns a list of all supported PE services.
    \param numServices Number of valid parameters in the following serviceId field
    \param serviceId[254] Field of supported PE job IDs
*/
typedef struct {
    PNIO_UINT8          numServices;    /**< number of supported services */
    PNIO_UINT8          serviceId[254]; /**< array of service ids PNIO_PE_CMD_xxx  */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_PE_IDENTIFY_CONF;

/**
    \brief
    Parameter of the callback event that describes the current energy saving mode.
    \param PE_Mode_ID_Source Original energy saving mode
        0x00: Energy saving mode (power off)
        0x01...0xFE: Vendor-specific
        0xFF: ready for operation
    \param PE_Mode_ID_Destination Energy saving mode to be reached
        0x00: Energy saving mode
        0x01...0xFE: Vendor-specific
        0xFF: ready for operation
    \param Time_to_operate Maximum time before the ready to operate status is reached
    \param Remaining_time_to_destination Time remaining until reaching the target status
    \param Mode_Power_Consumption Energy consumption in the current energy saving mode in [kW]
        If unknown: = 0.0 (Float 32 )
    \param Energy_Consumption_to_Destination Energy consumption of the current status change in [kWh]
    \param Energy_Consumption_to_operate Energy consumption from the current energy saving status 
        until reaching the ready to operate status in [kWh]
*/
typedef struct {
    PNIO_UINT8          PE_Mode_ID_Source;
    PNIO_UINT8          PE_Mode_ID_Destination;
    PNIO_UINT32         Time_to_operate;
    PNIO_UINT32         Remaining_time_to_destination;
    float               Mode_Power_Consumption;
    float               Energy_Consumption_to_Destination;
    float               Energy_Consumption_to_operate;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_PEM_STATUS_CONF;

/**
    \brief Brief
    Parameter of the callback event that returns a list of all supported energy saving modes.
    \param numModeIds Number of valid parameters in the following peModeId[254] field
    \param peModeId[254] Field with IDs of all supported energy saving modes
        Energy saving mode IDs:
        0: Energy saving mode(power off)
        1-0xFE: Vendor-specific
        0xFF: ready for operation
 */
typedef struct {
    PNIO_UINT8          numModeIds; 
    PNIO_UINT8          peModeId[254]; 

}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_Q_MODE_LIST_ALL_CONF;


/**
    \brief Brief
    Parameter for the request to signal all details of a certain energy saving mode.
    \param peModeId Energy saving mode ID
    \param reserved reserved
 */
typedef struct {
    PNIO_UINT8          peModeId; ///< Energy saving mode ID
    PNIO_UINT8          reserved; ///< reserved
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_Q_MODE_GET_MODE_REQ;
 
 
/* CONF: */
/**
    \brief
    Parameter of the callback event that returns details of a specific energy saving mode.
    \param peModeId Energy saving status ID
    \param PE_Mode_Attributes Bit 0; Possible values:
        0 = time and performance/energy information is static
        1 = time and performance/energy information is dynamic
        Bit 1 to 7: reserved
    \param Time_min_Pause Minimum duration of break in operation. 
    Is the sum in milliseconds of the three following parameters:
        - Time_to_Pause
        - Time_to_operate
        - Time_min_length_of_stay
    \param Time_to_Pause Expected duration before the energy saving mode is reached. Can only be static.
    \param Time_to_operate Expected duration before reaching the ready to operate status. 
        Can be either static or dynamic.
    \param Time_min_length_of_stay Minimum duration of energy saving mode.
    \param Time_max_length_of_stay Maximum duration of energy saving mode.
    \param Mode_Power_Consumption Energy consumption in the current energy saving mode in [kW]
        If undefined: = 0.0 (Float 32 )
    \param Energy_Consumption_to_pause Energy consumption of the changeover to the energy saving mode in [kWh]
    \param Energy_Consumption_to_operate Energy consumption from the current energy saving status until 
        reaching the ready to operate status in [kWh]
 */
typedef struct {
    PNIO_UINT8          peModeId; 
    PNIO_UINT8          PE_Mode_Attributes;
    PNIO_UINT32         Time_min_Pause; 
    PNIO_UINT32         Time_to_Pause; 
    PNIO_UINT32         Time_to_operate;
    PNIO_UINT32         Time_min_length_of_stay; 
    PNIO_UINT32         Time_max_length_of_stay; 
    float               Mode_Power_Consumption; 
    float               Energy_Consumption_to_pause; 
    float               Energy_Consumption_to_operate; 
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_Q_MODE_GET_MODE_CONF;

/* PE command Query_Measurement:  Q_MSRMT_GET_LIST     Service_Modifier= 0x01 == Get_Measurement_List
 */

#define PE_MSRMT_IDS_MAX   254

/* REQ: NULL */
/* CONF: */
typedef struct {
    PNIO_UINT16         Measurement_ID;
    PNIO_UINT8          Accuracy_Domain;
    PNIO_UINT8          Accuracy_Class;
    float               Range;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_MSRMT_ID_ELEM;

typedef struct {
    PNIO_UINT8          numMeasurementIds;
    PNIO_UINT8          reserved;
    PNIO_PE_MSRMT_ID_ELEM elem[PE_MSRMT_IDS_MAX];
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_Q_MSRMT_GET_LIST_CONF;


/* PE command Query_Measurement:  Q_MSRMT_GET_VAL      Service_Modifier= 0x02 == Get_Measurement_Values
 */
/* REQ: */
typedef struct {
    PNIO_UINT8          numMeasurementIds;
    PNIO_UINT8          reserved;
    PNIO_UINT16         Measurement_ID[PE_MSRMT_IDS_MAX];
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_Q_MSRMT_GET_VAL_REQ;

/* CONF: */
typedef struct {
    PNIO_UINT16         Measurement_ID;
    PNIO_UINT16         Status_of_Measurement_Value;
    float               Transmission_Data_Type;
    PNIO_UINT32         End_of_demand;
    PNIO_UINT32         reserved;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_MSRMT_VAL_ELEM;

typedef struct {
    PNIO_UINT8          numMeasurementValues;
    PNIO_UINT8          reserved;
    PNIO_PE_MSRMT_VAL_ELEM value[PE_MSRMT_IDS_MAX];
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_Q_MSRMT_GET_VAL_CONF;


typedef struct {
    PNIO_UINT8    err_code;
    PNIO_UINT8    reserved;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_PRM_GENERIC_CONF_NEG;

/* PNIO_PE_CBE_PRM:  PE confirmation parameter (callback param)
 */

/* PE_CONF_HDR: PE Service Response (confirmation) Header */
/**
    \brief
    This structure describes parameters of the "cbf_pe_cmd_conf" callback function 
    that are shared by all PE callback events.
    \param CmdId PE job IDs (PNIO_PE_CMD_ENUM 
    \param Handle "Handle", transferred with PNIO_pe_cmd_req()
    \param Addr Address of the head module from the job
    \param Ref Returned ReqRef of the PE job
    \param Err Error code on the execution of the job
        Note
        "Err" is the PROFINET IO error code transferred by the IO device for the error that occurred 
        when this read data record job executed on the IO device.
    \param state Job result:
        1= ok (job successful)
        2=err (job could not be executed)
    \param struct_id Structure ID of the job result:
        Depending on the CmdId, the struct_id precisely defines the type of the response parameters. 
        This distinction is necessary with "group jobs", for example PNIO_PE_CMD_Q_MODES. 
        With these jobs, the CmdId is not unique.
        Possible combinations are described in the section PNIO_PE_CBE_PRM; 
        PNIO_PE_CBE_PRM (callback event parameter) 
    \param length Length in bytes of the following parameters
 */
typedef struct {
    PNIO_PE_CMD_ENUM    CmdId;   /**< PE Service id (CommandId) 'int'   */
    PNIO_UINT32         Handle;  /**< Hndl returned by PNIO_ctrl_open() */
    PNIO_ADDR           Addr;
    PNIO_REF            Ref;
    PNIO_ERR_STAT       Err;
    PNIO_UINT16         state;         /**< Response state:1= ok, 2=err */
    PNIO_UINT16         struct_id;     /**< param. struct id,  0xff=err */
    PNIO_UINT32         length;  /**< num bytes of the following params */
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PE_CBE_HDR;

/* PE Confirmation Struct Ids */
#define PE_CNF_STRUCT_ID_RESERVED        0
#define PE_CNF_STRUCT_ID_ERROR           0xFF
#define PE_CNF_STRUCT_ID_UNIQUE          1
#define PE_CNF_STRUCT_ID_QMODE_LIST_ALL  1
#define PE_CNF_STRUCT_ID_QMODE_GET_MODE  2
#define PE_CNF_STRUCT_ID_QMSRMT_LIST_ALL 1
#define PE_CNF_STRUCT_ID_QMSRMT_GET_VAL  2

/**
   \brief
    The various PE callback events have the same data type PNIO_CBE_PRM 
    that groups the various response parameters of the individual PE jobs using a "union".
    \param pe_hdr The pe_hdr structure contains parameters that are shared by all PE callback events.
        Structure as in section "PE_CBE_HDR (PROFIenergy callback event header)         
    \param pe_prm "union" of the job-specific parameter structures.
        The selection of the correct parameter structure is based on the elements CmdId and struct_id from the pe_hdr structure.
        The following table shows the dependency of the parameter structures of CmdId and struct_id:
 */
typedef struct {
    PE_CBE_HDR                              pe_hdr; /**< The pe_hdr structure contains parameters that are shared by all PE callback events.
                                                      *  Structure as in section PE_CBE_HDR */
    /**
     * "union" of the job-specific parameter structures.The selection of the correct parameter structure is based 
     * on the elements CmdId and struct_id from the pe_hdr structure.
     */
    union {
        PNIO_PE_PRM_GENERIC_CONF_NEG        RespNegative;
        PNIO_PE_PRM_START_PAUSE_CONF        StartPause;
        PNIO_PE_PRM_END_PAUSE_CONF          EndPause;
        PNIO_PE_PRM_PE_IDENTIFY_CONF        PeIdentify;
        PNIO_PE_PRM_PEM_STATUS_CONF         PemStatus;
        PNIO_PE_PRM_Q_MODE_LIST_ALL_CONF    ModeList;
        PNIO_PE_PRM_Q_MODE_GET_MODE_CONF    ModeGet;
        PNIO_PE_PRM_Q_MSRMT_GET_LIST_CONF   MeasurementList;
        PNIO_PE_PRM_Q_MSRMT_GET_VAL_CONF    MeasurementValue;
    } pe_prm;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_CBE_PRM;


/* PNIO_PE_REQ_PRM: PE command request parameter
*/
/**
    * All these parameter structures are grouped together in a "union" in the PNIO_PE_REQ_PRM structure. 
    * The individual structures are described in detail in the following sections.
   \brief
    The PE job parameters for the individual PE jobs are transferred in suitable parameter structures. All these parameter structures are grouped together in a "union" in the PNIO_PE_REQ_PRM structure. The individual structures are described in detail in the following sections.
    \param CmdId PE job ID
    \param CmdModifier PE job modifier
    \param rq "union" of all PE job parameter structures
 */
typedef struct {
    PNIO_PE_CMD_ENUM          CmdId; ///< PE job ID
    PNIO_PE_CMD_MODIFIER_ENUM CmdModifier; ///< PE job modifier
    
    //"union" of the various PE job parameter structures 
                    //(details in the relevant sections)
    union {
        PNIO_PE_PRM_START_PAUSE_REQ         StartPause;
        PNIO_PE_PRM_Q_MODE_GET_MODE_REQ     ModeGet;
        PNIO_PE_PRM_Q_MSRMT_GET_VAL_REQ     MeasurementValue;
    } rq;
}  /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_PE_REQ_PRM;

/* PE Command Request Funnction
 */
/**
    \brief
    This function is used to send PE jobs to an IO device. 
    It is a group function for all PE requests of a controller application. 
    The results (confirmations) of the PE jobs are signaled by the PE group callback function cbf_pe_cmd_conf.
    If the IO-Base controller user program wants to use this function 
    (in other words the PROFIenergy functionality), 
    it must already have registered the callback function with the PNIO_register_pe_cbf() call 
    (PNIO_register_pe_cbf() 
    You will find an overview of all possible PE jobs in Table 4-1 List of all possible PE jobs 
    \param Handle Handle from PNIO_controller_open( )
    \param pAddr Logical address of the head module of the IO device for which the job is intended.
    \param ReqRef Job reference assigned by the IO-Base controller user program.
        Note 
        The "ReqRef" reference allows the IO-Base controller user program to distinguish different PE jobs.
        This parameter can be assigned any value by the IO-Base controller user program and is returned in
        the callback event.
    \param pPeReqPrm Pointer to the parameter structure "PNIO_PE_REQ_PRM" with 
        the corresponding "PNIO_PE_PRM_xxx_REQ" parameter structures. 
        With jobs that do not require any parameters, only the CmdId and CmdModifier are evaluated.
        Refer to the description of the "PNIO_PE_REQ_PRM" structure (PE job parameter); PNIO_PE_REQ_PRM 
        Note 
        The content of the memory to which "pPeReqPrm" points is copied, 
        in other words on the return from the function, the memory can be used again.
   \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
       (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_NO_FW_COMMUNICATION
        - PNIO_ERR_WRONG_HND
        - PNIO_ERR_SEQUENCE
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_pe_cmd_req(
    PNIO_UINT32    Handle,            //in
    PNIO_ADDR     *pAddr,             //in
    PNIO_REF       ReqRef,            //in
    PNIO_PE_REQ_PRM *pPeReqPrm        //in
);

/**
    \brief
    The PNIO_PE_CBF callback event reports the result of a PE job sent previously with a PNIO_pe_cmd_req call. 
    The callback function must be implemented by the user and registered by the PNIO_register_pe_cbf() call. 
    See section 5.13.2.3
    The callback function can be given any name by the user. 
    In this manual, for example, the name "cbf_pe_cmd_conf" is used.
    \param pPeCbfPrm Pointer to PE confirmation parameter
        The various callback events have the same data type PNIO_PE_CBE_PRM 
        (see PNIO_PE_CBE_PRM (callback event parameter) 
        that groups the various parameters of the individual PE callback events using a "union".
        Note
        After exiting this callback function, the "pPeCbfPrm" parameters are invalid. 
        If necessary, these must be saved prior to this.
    \return No return values.
 */
typedef void (*PNIO_PE_CBF) (PNIO_PE_CBE_PRM * pCbfPrm);

/**
    \brief
    This function registers a PE confirmation callback function.
    \note This callback function can be registered in any IO controller operating mode. 
        The registration of the callback function must, however, take place after the
         PNIO_controller_open( ) and before the first PNIO_pe_cmd_req() function call. 
        If the registration does not take place, the PNIO_pe_cmd_req() function returns 
         the error: PNIO_ERR_SEQUENCE
    \param Handle Handle from PNIO_controller_open( )
    \param cbf_pe_cmd_conf Address of the callback function to be called after arrival of the PE confirmation from the PNIOLib.
        Note 
        The function pointer must not be NULL.
    \return If successful, "PNIO_OK" is returned.
        If an error occurs, the following values are possible 
       (for the meaning, refer to the comments in the header file "pnioerrx.h"):
    - PNIO_ERR_WRONG_HND
    - PNIO_ERR_PRM_CALLBACK
    - PNIO_ERR_ALREADY_DONE
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_register_pe_cbf(
    PNIO_UINT32    Handle,            //in
    PNIO_PE_CBF    cbf_pe_cmd_conf    //in
);

#endif /* PROFI_ENERGY */
/* PE (PROFIenergy) Extensions END */


#if defined(_MSC_VER)
 #pragma pack( pop, safe_old_packing )
#elif defined(BYTE_ATTR_PACKING)
 #include "unpack.h"
#endif

#endif /* PNIOUSRX_H */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
