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
/*  F i l e               &F: pniobase.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Data types and function declarations for IO-Base user interface          */
/*                                                                           */
/*****************************************************************************/

#ifndef PNIOBASE_H
#define PNIOBASE_H

/**
 * \file pniobase.h
 * \brief Data types and function declarations for IO-Base user interface   
 */
//#define PNIO_ALARM_OLD_STRUC


#if defined(WIN32)
    typedef unsigned __int8  _uint8_t;
    typedef unsigned __int16 _uint16_t;
    typedef unsigned __int32 _uint32_t;

    typedef signed __int8   _int8_t;
    typedef signed __int16  _int16_t;
    typedef signed __int32  _int32_t;
    
#elif defined(WIN64)
    typedef unsigned __int8  _uint8_t;
    typedef unsigned __int16 _uint16_t;
    typedef unsigned __int32 _uint32_t;

    typedef signed __int8       _int8_t;
    typedef signed __int16      _int16_t;
    typedef signed __int32      _int32_t;
    typedef signed __int64      _int64_t;

#elif defined(TOOL_CHAIN_GNU)

    typedef  unsigned char          _uint8_t;
    typedef  unsigned short int     _uint16_t;
    typedef  unsigned long int      _uint32_t;
    typedef  unsigned long long int _uint64_t;

    typedef  signed char            _int8_t;
    typedef  signed short int       _int16_t;
    typedef  signed long int        _int32_t;
    typedef  signed long long int   _int64_t;

#else
#error "unknown compiler!"
#endif

/* base data type specification */
typedef _int8_t   PNIO_INT8;
typedef _int16_t  PNIO_INT16;
typedef _int32_t  PNIO_INT32;
typedef _uint8_t  PNIO_UINT8;
typedef _uint16_t PNIO_UINT16;
typedef _uint32_t PNIO_UINT32;
typedef _uint32_t PNIO_REF;

/* other types */
#define PNIO_BOOL           PNIO_UINT8
#define PNIO_FALSE          ((PNIO_BOOL) 0)
#define PNIO_TRUE           ((PNIO_BOOL)(!PNIO_FALSE))
#define PNIO_RET_OK         1
#define PNIO_VOID           void 
#define PNIO_VOID_PTR_TYPE  void *
#ifdef __cplusplus
# define PNIO_NULL          (0)
#else
# define PNIO_NULL          ((void*)0)
#endif

#include "pnioerrx.h"

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
 #error please adapt pniobase.h header for your compiler
#endif

#undef PNIO_CODE_ATTR
 
#if defined PASCAL && !defined PNIO_PNDRIVER
    #define PNIO_CODE_ATTR __stdcall
#else
    #define PNIO_CODE_ATTR
#endif

//#define PNIO_CODE_ATTR


/* deprecated definition; don't use this define */
/*
#ifdef __cplusplus
    #define PNIO_EXTRN extern "C"
#else
    #define PNIO_EXTRN extern
#endif*/

#define PNIO_API_VERSION 300

#define PNIO_MAX_ALARM_DATA_LEN  1472

/**
    \brief
    This enumeration holds address types.
    \param PNIO_ADDR_LOG Logical address type 
    \param PNIO_ADDR_GEO Geographical address type
 */
typedef enum {
    PNIO_ADDR_LOG = 0,
    PNIO_ADDR_GEO = 1
} PNIO_ADDR_TYPE;

/**
    \brief
    Identifier for the data direction of an address.
    \param PNIO_IO_IN Identifier for the input data direction
    \param PNIO_IO_OUT Identifier for the output data direction
    \param PNIO_IO_IN_OUT Identifier for bi-directional data direction
    \param PNIO_IO_UNKNOWN Identifier for the unknown data direction
 */
typedef enum {
    PNIO_IO_IN      = 0, ///< Identifier for the input data direction
    PNIO_IO_OUT     = 1, ///< Identifier for the output data direction
    PNIO_IO_IN_OUT  = 2, ///< Identifier for the input/output data direction
    PNIO_IO_UNKNOWN = 3  ///< Identifier for the unknown data direction
} PNIO_IO_TYPE;

/* geographical addressing (device only)*/
typedef struct {
    PNIO_ADDR_TYPE  AddrType;
    PNIO_IO_TYPE    IODataType;
    union {
        PNIO_UINT32 Addr;
        struct {
            PNIO_UINT32 reserved1[2];
            PNIO_UINT32 Slot;
            PNIO_UINT32 Subslot;
            PNIO_UINT32 reserved2;
        } Geo;                   /* geographical address */
    } u;
} ATTR_PACKED PNIO_DEV_ADDR;
#ifndef PNIO_SOFTNET /**< not supported for PNIO SOFTNET */

/**
    \brief
    This enumeration type specifies the types of data of a submodule. 
    It is used in the PNIO_CTRL_DIAG_CONFIG_SUBMODULE structure.
    \param PNIO_DATA_RT RT data
    \param PNIO_DATA_IRT IRT data
*/
typedef enum {
    PNIO_DATA_RT  = 0,  
    PNIO_DATA_IRT = 1
} PNIO_DATA_TYPE;

/**
    \brief
    This enumeration type specifies the communication type of a submodule. 
    It is used in the PNIO_CTRL_DIAG_CONFIG_SUBMODULE structure.
    \param PNIO_COM_UNICAST Transfer between IO device and IO controller
    \param PNIO_COM_DIRECT_DATA_EX Direct data exchange transfer
*/
typedef enum {
    PNIO_COM_UNICAST        = 0, /**< controller-device */
    PNIO_COM_DIRECT_DATA_EX = 1  /**< direct data exchange */
} PNIO_COM_TYPE; /**< communication type */

/**
    \brief
    This enumeration type lists the possible types of access.
    The values are supplied as parameters with the PNIO_initiate_data_read_ext( ) 
    and PNIO_initiate_data_write_ext( ) functions.  They restrict the submodules 
    and the type of access for which the PNIO_DATA_READ_CBF( ) and PNIO_DATA_WRITE_CBF( )
    callbacks are called.
    \param PNIO_ACCESS_ALL_WITHOUT_LOCK Both RT and IRT data will be accessed.
        Consistency of the RT data is not guaranteed.
        Consistency of the IRT data only if access is isochronous real-time access.
    \param PNIO_ACCESS_RT_WITH_LOCK Only RT data will be accessed.
        Consistency of the data is guaranteed.
    \param PNIO_ACCESS_RT_WITHOUT_LOCK Only RT data will be accessed.
        Consistency of the data is not guaranteed.
    \param PNIO_ACCESS_IRT_WITHOUT_LOCK Only IRT data will be accessed.
        Consistency of the IRT data is only guaranteed if access is isochronous real-time access.
 */

typedef enum {
    PNIO_ACCESS_ALL_WITH_LOCK = 0,    /**< not supported */
    PNIO_ACCESS_ALL_WITHOUT_LOCK = 1, /**< Both RT and IRT data will be accessed. */

    PNIO_ACCESS_RT_WITH_LOCK    = 2,  /**< Only RT data will be accessed.Consistency of the data is guaranteed.*/
    PNIO_ACCESS_RT_WITHOUT_LOCK = 3,  /**< Only RT data will be accessed. Consistency of the data is not guaranteed. */

    PNIO_ACCESS_IRT_WITH_LOCK = 4,    /**< not supported */
    PNIO_ACCESS_IRT_WITHOUT_LOCK = 5  /**< Only IRT data will be accessed.Consistency of the IRT data is only guaranteed if access is isochronous real-time access.*/
} PNIO_ACCESS_ENUM; 
#endif /* not supported for PNIO SOFTNET */

/**
    \brief
    Identifiers to describe the status of the IO data.
    \param PNIO_S_GOOD IO data is valid
    \param PNIO_S_BAD IO data is invalid
 */
typedef enum {
    PNIO_S_GOOD = 0,
    PNIO_S_BAD  = 1
} PNIO_IOXS;

/**
 * \struct PNIO_ERR_STAT:
 * a) The first four parameters represent the PNIO Status.
 *    For details refer to IEC 61158-6 chapter "Coding of the field PNIOStatus" \n
 * b) The last two parameters correspond to AdditionalValue1 and AdditionalValue2, \n
 *    see IEC 61158-6 chapter "Coding of the field AdditionalValue1 and AdditionalValue2":
 *    The values shall contain additional user information within negative responses.
 *    The value zero indicates no further information.
 *    For positive read responses, the value 1 of the field AdditionalValue1 indicates
 *    that the Record Data Object contains more data than have been read. \n
 * c) For the two callback events PNIO_CBE_REC_READ_CONF and
 *    PNIO_CBE_REC_WRITE_CONF, the following special cases apply to 
 *    the "Err" parameter: \n
 *    - Case 1 
 *      ErrorCode=0xC0 and
 *      ErrorDecode=0x80 and
 *      ErrCode1=0xA3 and
 *      ErrCode2=0x0
 *      Cause: A connection to the target device cannot be established
 *             or was interrupted. \n
 *    - Case 2 
 *      ErrorCode=0xDE/0xDF and
 *      ErrorDecode=0x80 and
 *      ErrCode1=0xC3 and
 *      ErrCode2=0x0
 *      Cause: The target device is currently busy with other jobs.
 *             Repeat the job. \n
 *    - Case 3 
 *      For all other situations, refer to the error codes of the
 *      PROFINET IO standard IEC 61158-6, Section "Coding of the field PNIOStatus".
 */
typedef struct {
    PNIO_UINT8     ErrCode;   /**< ErrorCode: Most significant word, most significant byte of PNIO Status */
    PNIO_UINT8     ErrDecode; /**< ErrorDecode: Most significant word, least significant byte of PNIO Status */
    PNIO_UINT8     ErrCode1;  /**< ErrorDecode: Least significant word, most significant byte of PNIO Status */
    PNIO_UINT8     ErrCode2;  /**< ErrorCode2: Least significant word, least significant byte of PNIO Status */
    PNIO_UINT16    AddValue1;
    PNIO_UINT16    AddValue2;
} /*! \cond HideIt */ ATTR_PACKED /*! \endcond */ PNIO_ERR_STAT;

typedef enum {
    PNIO_ALARM_DIAGNOSTIC                   = 0x01,
    PNIO_ALARM_PROCESS                      = 0x02,
    PNIO_ALARM_PULL                         = 0x03,
    PNIO_ALARM_PLUG                         = 0x04,
    PNIO_ALARM_STATUS                       = 0x05,
    PNIO_ALARM_UPDATE                       = 0x06,
    PNIO_ALARM_REDUNDANCY                   = 0x07,
    PNIO_ALARM_CONTROLLED_BY_SUPERVISOR     = 0x08,
    PNIO_ALARM_RELEASED_BY_SUPERVISOR       = 0x09,
    PNIO_ALARM_PLUG_WRONG                   = 0x0A,
    PNIO_ALARM_RETURN_OF_SUBMODULE          = 0x0B,
    PNIO_ALARM_DIAGNOSTIC_DISAPPEARS        = 0x0C,
    PNIO_ALARM_MCR_MISMATCH                 = 0x0D,
    PNIO_ALARM_PORT_DATA_CHANGED            = 0x0E,
    PNIO_ALARM_SYNC_DATA_CHANGED            = 0x0F,
    PNIO_ALARM_ISOCHRONE_MODE_PROBLEM       = 0x10,
    PNIO_ALARM_NETWORK_COMPONENT_PROBLEM    = 0x11,
    PNIO_ALARM_TIME_DATA_CHANGED            = 0x12,
    PNIO_ALARM_UPLOAD_AND_STORAGE           = 0x1E,
    PNIO_ALARM_PULL_MODULE                  = 0x1F,
    PNIO_ALARM_DEV_FAILURE                  = 0x00010000,
    PNIO_ALARM_DEV_RETURN                   = 0x00010001
} PNIO_ALARM_TYPE;

typedef enum {
    PNIO_APRIO_LOW  = 0,
    PNIO_APRIO_HIGH = 1
} PNIO_APRIO_TYPE;

/**\brief diagnosis Alarm */
typedef struct {
    PNIO_UINT16 ChannelNumber ; /**< 0x0000 ... 0x7fff manufacturer specific */
                                /**< 0x8000 Submodule */
                                /**< 0x8001 .. 0xffff reserved */
    PNIO_UINT16 ChannelProperties; /**< see FDIS 61158-6-10 chapter 6.2.7.4 for coding Table 520 - 524 */
    PNIO_UINT16 ChannelErrorType; /**< see FDIS 61158-6-10 chapter 6.2.7.2 for coding Table 518 */
} PNIO_ALARM_DATA_DIAGNOSIS2 ;	

/**
    \brief
    The PNIO_BLOCK_HEADER structure is part of the I&M data record structures described below:
        PNIO_IM0_Type
        PNIO_IM1_TYPE
        PNIO_IM2_TYPE
        PNIO_IM3_TYPE
        PNIO_IM4_TYPE
    The values of the elements of PNIO_BLOCK_HEADER depend on the I&M data record structures. 
    You will find these values in the descriptions of the I&M data records below (PNIO_IM0 to PNIO_IM4).
*/

typedef struct {
    PNIO_UINT16 BlockType;
    PNIO_UINT16 BlockLength;
    PNIO_UINT8  BlockVersionHigh;
    PNIO_UINT8  BlockVersionLow;
} PNIO_BLOCK_HEADER;

typedef struct {
    PNIO_BLOCK_HEADER BlockHeader;  /**< 6 Byte don't care */
    PNIO_UINT8 padd;                /**< don't care */
    PNIO_UINT8 padd1;               /**< don't care */
    PNIO_UINT32 MaintenanceStatus;  /**< bit 0 = maintance required if 1 */
                                    /**< bit 1 = maintance demanded if 1 */
} PNIO_MAINTENANCE ;

/**\brief diagnosis Alarm */
typedef struct {
    PNIO_UINT16 ChannelNumber ; /**< 0x0000 ... 0x7fff manufacturer specific */
                                /**< 0x8000 Submodule */
                                /**< 0x8001 .. 0xffff reserved */
    PNIO_UINT16 ChannelProperties; /**< see FDIS 61158-6-10 chapter 6.2.7.4 for coding Table 520 - 524 */
    PNIO_UINT16 ChannelErrorType; /**< see FDIS 61158-6-10 chapter 6.2.7.2 for coding Table 518 */
} PNIO_ALARM_DATA_DIAGNOSIS ;	

/**\brief maintenance diagnosis Alarm */
typedef struct {
    PNIO_MAINTENANCE maintInfo;
    PNIO_UINT16 UserStrucIdent;      /**< user structure identifier for user alarm data         
                                       * - 0x8000 :  diagnosis alarm                             
                                       * - 0x8002 :  extended diagnosis alarm                    
                                       * - 0x0000  ... 0x7fff :  Manufacturer specific diagnosis or process alarm    */

    PNIO_UINT16 ChannelNumber ; /**< - 0x0000 ... 0x7fff manufacturer specific 
                                  *   - 0x8000 Submodule 
                                  *   - 0x8001 .. 0xffff reserved */
    PNIO_UINT16 ChannelProperties; /**< see FDIS 61158-6-10 chapter 6.2.7.4 for coding Table 520 - 524 */
    PNIO_UINT16 ChannelErrorType; /**< see FDIS 61158-6-10 chapter 6.2.7.2 for coding Table 518 */
} PNIO_ALARM_DATA_MAINTENANCE_DIAGNOSIS ;	

/**\brief extended diagnosis Alarm */
typedef struct {
    PNIO_UINT16 ChannelNumber ;
    PNIO_UINT16 ChannelProperties;
    PNIO_UINT16 ChannelErrorType;
    PNIO_UINT16 ExtChannelErrorType; /**< see FDIS 61158-6-10 chapter 6.2.7.5 for coding Table 525 - 537 */
    PNIO_UINT32 ExtChannelAddValue;  /**< see FDIS 61158-6-10 chapter 6.2.7.6 for coding  Table 538 -541 */
} PNIO_ALARM_DATA_EXT_DIAGNOSIS ;	

/**\brief extended diagnosis Alarm */
typedef struct {
    PNIO_MAINTENANCE maintInfo;
    PNIO_UINT16 UserStrucIdent;
    PNIO_UINT16 ChannelNumber ;
    PNIO_UINT16 ChannelProperties;
    PNIO_UINT16 ChannelErrorType;
    PNIO_UINT16 ExtChannelErrorType; /**< see FDIS 61158-6-10 chapter 6.2.7.5 for coding Table 525 - 537 */
    PNIO_UINT32 ExtChannelAddValue;  /**< see FDIS 61158-6-10 chapter 6.2.7.6 for coding  Table 538 -541 */
} PNIO_ALARM_DATA_MAINTENANCE_EXT_DIAGNOSIS ;	

typedef struct {
    PNIO_UINT16    BlockType;           /**< block type from alarm data block */
    PNIO_UINT16    BlockVersion;        /**< version from alarm data block */
    PNIO_UINT32    Api;                 /**< for PNIO: Application Process Identifier from database           */
    PNIO_UINT16    AlarmSpecifier;      /**< alarm-specifier from alarm data block
                                         *        - Bit 14-15: reserved
                                         *        - Bit 13:    Submodul-Diagnosis State
                                         *        - Bit 12:    Generic Diagnosis
                                         *        - Bit 11:    Channel Diagnosis
                                         *        - Bit  0-10: sequence number                               */

    PNIO_UINT32    ModIdent;            /**< modul-identifikation from alarm-data-block                       */
    PNIO_UINT32    SubIdent;            /**< submodul-identification from alarm-data-block                    */

    PNIO_UINT16    UserStrucIdent;      /**< user structure identifier for user alarm data         */
                                        /* - 0x8000 :  diagnosis alarm                             */
                                        /* - 0x8002 :  extended diagnosis alarm                    */
                                        /* - 0x8100 :  maintenance diagnosis alarm                 */
                                        /* - 0x0000  ... 0x7fff :  Manufacturer specific diagnosis or process alarm    */

    PNIO_UINT16    UserAlarmDataLen;                        /**< length of the user alarm data     */
    union {
            PNIO_ALARM_DATA_MAINTENANCE_DIAGNOSIS m_diag;	/**< maintenance Alarm */
            PNIO_ALARM_DATA_MAINTENANCE_EXT_DIAGNOSIS mext_diag;	/**< extended Maintenance Alarm */
            PNIO_ALARM_DATA_DIAGNOSIS diag;	/**< diagnosis Alarm */
            PNIO_ALARM_DATA_EXT_DIAGNOSIS ext_diag; /**< extended diagnosis Alarm */
            PNIO_UINT8     UserAlarmData[PNIO_MAX_ALARM_DATA_LEN];  /**< Manufacturer specific diagnosis or process alarm */
    } UAData;


} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_ALARM_INFO;

typedef struct {
    PNIO_UINT16    CompatDevGeoaddr;    /**< bit 0-10 = device_no,                                */
                                        /**< bit 11-14 = io-subsys-nr,                            */
                                        /**< bit 15 = 1 (pnio identifier)                         */
                                        /**< bit 11-14 = io-subsys-no,                            */
                                        /**< bit 15 = 1 (pnio identifier)                         */
    PNIO_UINT8     ProfileType;         /**< 0x08 => bit 0-3 for PNIO, bit 4-7 for DP             */
    PNIO_UINT8     AinfoType;           /**< bit 0-3 alinfotyp = 0x00 (transparent)               */
    PNIO_UINT8     ControllerFlags;     /**< bit 0 = 1 (ext. dp-interface), bit 1-7 reserved      */
    PNIO_UINT8     DeviceFlag;          /**< bit 0 for PNIO: apdu-stat-failure, bit 1-7 reserved  */
    PNIO_UINT16    PnioVendorIdent;     /**< for PNIO: vendor-identification                      */
    PNIO_UINT16    PnioDevIdent;        /**< for PNIO: device-identification                      */
    PNIO_UINT16    PnioDevInstance;     /**< for PNIO: instance of device                         */

} /**\cond HideIt */ ATTR_PACKED /**\endcond*/ PNIO_ALARM_TINFO;

/** constants specification for PNIO_controller_open, PNIO_device_open ExtPar */
#define PNIO_CEP_MODE_CTRL                 0x00000002      /**< allow use of PNIO_set_mode */

#ifndef PNIO_SOFTNET /* not supported for PNIO SOFTNET */
#define PNIO_CEP_DEV_MAXAR_BY_TYPE         0x00000004      /**< allow specificate maximal AR nummber by AR-Type, only for PNIO_device_open */
#define PNIO_CEP_SLICE_ACCESS              0x00000008      /**< allow module slice read/write */
#define PNIO_CEP_DEV_MAXAR_BY_TYPE_ADV     0x00000010      /**< allow specification of maximum AR number by AR-Type */
                                                           /**< (RT, DA), only for PNIO_device_open()               */
#define PNIO_CEP_PE_MODE_ENABLE            0x00000020      /**< allow Host-PC shutdown for PROFIEnergy  */
#define PNIO_CEP_SET_MRP_ROLE_OFF          0x00000040      /**< sets MRP role to state OFF  */

/**
    This callback is called by the IO-Base interface as soon as user program watchdog has responded. 
    After calling this callback, all user programs that have called PNIO_controller_open( ) must call 
    PNIO_controller_close( ) and all user programs that have called PNIO_device_open( ) must send PNIO_device_close( ).
    \param CpIndex CP index for which the user program watchdog was registered.
 */
typedef void (*PNIO_CBF_APPL_WATCHDOG)
       (PNIO_UINT32 CpIndex);

/**
 * \enum PNIO_CP_CBE_TYPE The IO-Base user programming interface recognizes the following callback event types:
 *       These callback events can only be registered by the IO controller or the IO device.
 *       If you want to implement an IO controller and IO device at the same time, 
 *       you will need to address both devices in a common IO-Base user program.
 */
typedef enum {
    PNIO_CP_CBE_UNKNOWN         = 0,
    PNIO_CP_CBE_STARTOP_IND     = 1, /**< Start of isochronous real-time data processing */
    PNIO_CP_CBE_APPL_START_IND  = PNIO_CP_CBE_STARTOP_IND,
    PNIO_CP_CBE_OPFAULT_IND     = 2, /**< Violation of isochronous real-time mode */
    PNIO_CP_CBE_APPL_FAULT_IND  = PNIO_CP_CBE_OPFAULT_IND,
    PNIO_CP_CBE_NEWCYCLE_IND    = 3, /**< Start of a new bus cycle */
    PNIO_CP_CBE_BUSCYCLE_IND    = PNIO_CP_CBE_NEWCYCLE_IND
} PNIO_CP_CBE_TYPE;

/**
    The structure contains information on the current cycle and allows the following to be identified:
    Highly accurate counter ("ClockCount”) with a resolution of 10 ns
    Loss of interrupts ("CycleCount" does not change by the same value in every cycle)
    "CountSinceCycleStart" returns the time between the start of the cycle and the call for the function 
    that returns the PNIO_CYCLE_INFO structure.
    Time between the PNIO_CP_CBE_STARTOP_IND callback event and the PNIO_CP_set_opdone( ) function call 
    (difference between the input value "CountSinceCycleStart” of the PNIO_CP_CBE_STARTOP_IND callback event 
    and the return value of the PNIO_CP_set_opdone( ) function)
    It is used as the input parameter for the callback events:
        PNIO_CP_CBE_STARTOP_IND
        PNIO_CP_CBE_OPFAULT_IND
        PNIO_CP_CBE_NEWCYCLE_IND
    The structure serves as a return value for the function PNIO_CP_set_opdone( ).
    \param ClockCount Free-running hardware counter on the CP with a resolution of 10 ns (32-bits wide).
    \param CycleCount The "CycleCount" value is incremented in each cycle by the value corresponding to 
    the cycle duration based on 31.25 &mu;s. The value is 16 bits wide.
    Example
    With a cycle of 1 ms, the value is incremented each time by 32.
    \param CountSinceCycleStart The "CountSinceCycleStart" value specifies the time since the start of the last cycle 
    based on 10 ns. The value is 32 bits wide.
 */
typedef struct {
    PNIO_UINT32       CycleCount;	/**< Free-running hardware counter on the CP with a resolution of 10 ns (32-bits wide).*/
    PNIO_UINT32       ClockCount; /**< The "CycleCount" value is incremented in each cycle by the value corre-sponding to 
                                    *	the cycle duration based on 31.25 us.
                                    *	The value is 16 bits wide.
                                    *	Example
                                    *	With a cycle of 1 ms, the value is incremented each time by 32. 
                                    */
    PNIO_UINT32       CountSinceCycleStart; /**< The "CountSinceCycleStart" value specifies the time since 
                                              *  the start of the last cycle based on 10 ns. The value is 32 bits wide.
                                              */
} PNIO_CYCLE_INFO;

/**
    \brief
    The PNIO_CP_CBE_STARTOP_IND callback event informs your IO-Base user program of the end of the 
    IRT data transfer phase and the transfer of the IRT input data from the process image to the host memory by DMA.
    From this point onwards, all IRT input data is in the host memory. This allows your user program to 
    access consistent IRT data.
    The following syntax shows the specific parameters for this event as part of the 
    "union" from the PNIO_CP_CBE_PRM structure; see the section "PNIO_CP_CBE_PRM (callback event parameter)
    The PNIO_CP_CBE_STARTOP_IND callback event informs your IO-Base user program of the end of the 
    IRT data transfer phase and the transfer of the IRT output data (from the perspective of the IO controller) 
    from the process image to the host memory using DMA. 
    As of this point in time, all IRT output data (from the perspective of the IO controller) is in the host memory. 
    This allows your user program to access consistent IRT data.
    The following syntax shows the specific parameters for this event as part of the 
    "union" from the PNIO_CP_CBE_PRM structure; see the section "PNIO_CP_CBE_PRM (callback event parameter) 
    \note The PNIO_CP_CBE_STARTOP_IND callback event is called at the end of each IRT data transfer phase. This occurs even if there is no connection established to the IO controller and there is therefore no IRT data for the IO device.
    If functions for IO data access are called due to the PNIO_CP_CBE_STARTOP_IND callback event, although there is no IRT data available, this causes the warning return value PNIO_WARN_NO_SUBMODULES that can be ignored in this case.
    \param AppHandle Handle from PNIO_controller_open( ) or PNIO_device_open( )
    \param CycleInfo Information on the current cycle
    \note During the PNIO_CP_CBE_STARTOP_IND event, you must not call any functions that might endanger 
        the real-time capability of your IO-Base user program. 
        This means functions that take longer to process such as file operations or screen displays. 
        Refer to the description of your operating system to find out which functions might be involved in this situation.
        As a general recommendation, we advise you to restrict yourself to use of the functions for 
        accessing the IRT data of the IO-Base user interface.
    \param AppHandle Handle from PNIO_controller_open( ) or PNIO_device_open( )
    \param CycleInfo Information on the current cycle
    \note During the PNIO_CP_CBE_STARTOP_IND event, you must not call any functions that might endanger 
        the real-time capability of your user program. 
        This means functions that take longer to process such as file operations or screen displays. 
        Refer to the description of your operating system to find out which functions might be involved in your situation.
        As a general recommendation, we advise you to restrict yourself to use of the functions for 
        accessing the IRT data of the IO-Base user interface.
**/
typedef struct {
    PNIO_UINT32       AppHandle;             //in
    PNIO_CYCLE_INFO   CycleInfo;            //in
} PNIO_CP_CBE_PRM_STARTOP_IND;
 
/**
    \brief
    The PNIO_CP_CBE_OPFAULT_IND callback event signals a violation of the isochronous real-time mode to 
    your IO-Base user program. This means that your user program called PNIO_CP_set_opdone( ) too late after 
    receiving the PNIO_CP_CBE_STARTOP_IND callback event.
    The following syntax shows the specific parameters for this event 
    as part of the "union" from the PNIO_CP_CBE_PRM structure; 
    see the section "PNIO_CP_CBE_PRM (callback event parameter) 
    The PNIO_CP_CBE_OPFAULT_IND callback event signals a violation of the isochronous real-time mode to 
    your IO-Base user program. This means that your user program called PNIO_CP_set_opdone( ) too late after 
    receiving the PNIO_CP_CBE_STARTOP_IND callback event.
    The following syntax shows the specific parameters for this event 
    as part of the "union" from the PNIO_CP_CBE_PRM structure; 
    see the section "PNIO_CP_CBE_PRM (callback event parameter) 
    \param AppHandle Handle from PNIO_controller_open( ) or PNIO_device_open( )
    \param CycleInfo Information on the current cycle
 */
typedef struct {
    PNIO_UINT32       AppHandle;             //in
    PNIO_CYCLE_INFO   CycleInfo;            //in
} PNIO_CP_CBE_PRM_OPFAULT_IND;

 /**
    \brief
    The PNIO_CP_CBE_NEWCYCLE_IND callback event signals the start of a new bus cycle to the IO-Base user program. 
    The event is only signaled when it was registered using PNIO_CP_register_cbf( ).
    The following syntax shows the specific parameters for this event 
    as part of the "union" from the PNIO_CP_CBE_PRM structure; 
    see the section "PNIO_CP_CBE_PRM (callback event parameter) 
    The PNIO_CP_CBE_NEWCYCLE_IND callback event signals the start of a new bus cycle to your IO-Base user program. 
    The event is only signaled when it was registered using PNIO_CP_register_cbf( ).
    The following syntax shows the specific parameters for this event 
    as part of the "union" from the PNIO_CP_CBE_PRM structure; 
    see the section "PNIO_CP_CBE_PRM (callback event parameter) 
    \param AppHandle Handle from PNIO_controller_open( ) or PNIO_device_open( )
    \param CycleInfo Information on the current cycle
 */
typedef struct {
    PNIO_UINT32       AppHandle;             //in
    PNIO_CYCLE_INFO   CycleInfo;            //in
} PNIO_CP_CBE_PRM_NEWCYCLE_IND;

/**
    \brief
    The various callback events have the same data type PNIO_CP_CBE_PRM that 
    groups the various parameters of the individual callback events using a "union".
    \param CbeType Callback event
    \param CpIndex CpIndex from PNIO_controller_open( ) or from PNIO_device_open( )
    \param CbeType Callback event type
    \param Handle Handle from PNIO_controller_open( ) or from PNIO_device_open( )
*/
typedef struct {
    PNIO_CP_CBE_TYPE  CbeType; 
    PNIO_UINT32       CpIndex; 
    union {
        PNIO_CP_CBE_PRM_STARTOP_IND StartOp;
        PNIO_CP_CBE_PRM_OPFAULT_IND OpFault;
        PNIO_CP_CBE_PRM_NEWCYCLE_IND NewCycle;

        /* to be extended */
    } u;
} PNIO_CP_CBE_PRM;

/**
    \brief
    Using the callback event parameter PNIO_CP_CBE_PRM, 
    a general PNIO callback function of the type PNIO_CP_CBF is declared.
 */
typedef void (*PNIO_CP_CBF) (PNIO_CP_CBE_PRM *pCbfPrm);

#ifdef __cplusplus
extern "C" {
#endif

/**
    \brief
    With this function, the user program registers for time monitoring (watchdog). 
    The user program specifies the timeout interval in "wdTimeOutInMs". 
    After activating the watchdog, the user program must call the PNIO_CP_trigger_watchdog( ) function at least once 
    during the monitoring interval to signal to the IO-Base interface that the user program is functioning. 
    The monitoring is activated only after PNIO_CP_trigger_watchdog() is called the first time. 
    If the user program does not call the PNIO_CP_trigger_watchdog( ) in good time, monitoring stops, 
    the registered callback is called and the CP does not send any more IO data. 
    Following this, no other function calls are permitted except PNIO_contoller_close() and PNIO_device_close(). 
    To allow the CP to send IO data again, the user program must first call PNIO_controller_close() or 
    PNIO_device_close() and then call PNIO_contoller_open() or PNIO_controller_open() again.
    \note Only one user program can ever register for the user program timeout monitoring.
    \note The watchdog is only started when the PNIO_CP_trigger_watchdog function has been called at least once.
    \note If the timeout interval of a user program is exceeded, the PROFINET IO communication of other user programs 
    over the same CP is also stopped.
    \note To restart the watchdog when the timeout interval has elapsed, 
    the previous watchdog must be deregistered and then registered again.
    \note If the watchdog is registered and unregistered repeatedly, a wait time should be maintained between 
    the individual function calls. This must be at least 20 ms.
    \param CpIndex : Module index - Used to uniquely identify the communications module. 
        Refer to the configuration for this parameter ("module index" of the CP).
    \param wdTimeOutInMs : Timeout interval in ms. To deregister the user program watchdog, the value must be set to 0.
    \param pnio_appl_wd_cbf : Callback to be called in the timeout interval is exceeded. To deregister the user program
        watchdog, the value must be set to 0.
    \return If successful, "PNIO_OK" is returned. If an error occurs, 
        the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_ALLREADY_DONE
        - PNIO_ERR_CREATE_INSTANCE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_MAX_REACHED
        - PNIO_ERR_PRM_CP_ID
 */ 
PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_set_appl_watchdog(
    PNIO_UINT32              CpIndex,
    PNIO_UINT32              wdTimeOutInMs,
    PNIO_CBF_APPL_WATCHDOG   pnio_appl_wd_cbf
);

/**
    \brief
    This function is used to retrigger the user program watchdog.
    \param CpIndex CP index for which the user program watchdog will be retriggered.
    \return If successful, "PNIO_OK" is returned. If an error occurs, 
        the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_NO_FW_COMMUNICATION
        - PNIO_ERR_PRM_CP_ID
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_trigger_watchdog(
    PNIO_UINT32              CpIndex
);




/**
    \brief
    This function registers a callback function.
    \note Callback functions can only be registered by the IO controller user program in OFFLINE mode.
    \note The PNIO_CP_CBE_OPFAULT_IND callback event type must be registered before 
    the PNIO_CP_CBE_STARTOP_IND callback event type.
    This function registers a callback function.
    \note Callback functions can only be registered by the IO device user program prior to PNIO_device_start( ).
    \note The PNIO_CP_CBE_OPFAULT_IND callback event type must be registered before 
        the PNIO_CP_CBE_STARTOP_IND callback event type.
    \param AppHandle Handle from PNIO_controller_open( ) or PNIO_device_open( )
    \param CbeType Callback event type for which the callback function "Cbf" will be registered; 
        see the section "PNIO_CP_CBE_TYPE (callback event type)    
    \param Cbf Callback function to be started after arrival of the callback event "CbeType".
        Note 
        The function pointer must not be NULL.
    \param AppHandle IO device handle from PNIO_device_open( )
    \param CbeType Callback event type for which the callback function "Cbf" will be registered; 
        see the section "PNIO_CP_CBE_TYPE (callback event type)    
    \param Cbf Callback function to be started after arrival of the callback event "CbeType".
        Note 
        The function pointer must not be NULL.
    \return If successful, "PNIO_OK" is returned. If an error occurs, 
        the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_ALLREADY_DONE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_INVALID_CONFIG
        - PNIO_ERR_PRM_CALLBACK
        - PNIO_ERR_PRM_TYPE
        - PNIO_ERR_SEQUENCE
        - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_register_cbf(
    PNIO_UINT32              AppHandle,
    PNIO_CP_CBE_TYPE         CbeType,
    PNIO_CP_CBF              Cbf
);

/**
    \brief
    By calling this function, the IO-Base user program signals the IO-Base interface
    that it has completed isochronous real-time processing of the IRT IO data. The IO-Base
    interface than initiates transmission of the IRT output data from the host memory to the 
    process image by DMA.
    By calling this function, the IO-Base user program signals the IO-Base interface 
    that it has completed isochronous real-time processing of the IRT IO data. The IO-Base 
    interface then initiates the transfer of the IRT input data (from the perspective of the controller) 
    from the host memory to the process image using DMA.
    \param AppHandle Handle from PNIO_controller_open( ) or PNIO_device_open( )
    \param pCycleInfo Information on the current cycle in which this call was executed. 
        If the pointer was 0, nothing is returned.
   \return If successful, "PNIO_OK" is returned. If an error occurs, 
        the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
       - PNIO_ERR_INTERNAL
       - PNIO_ERR_NO_FW_COMMUNICATION
       - PNIO_ERR_WRONG_HND
 */
PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_set_opdone(
    PNIO_UINT32              AppHandle,          //in
    PNIO_CYCLE_INFO        * pCycleInfo         //out
);
#ifdef __cplusplus
}
#endif


/** Typedefs and Functions for development purposes.
 *  Not needed for normal operation. */
#define PNIO_CI_FAULT_TIME_LEN 20

typedef enum {
    PNIO_CP_CI_UNKNOWN = 0,
    PNIO_CP_CI_STARTOP = 1,
    PNIO_CP_CI_OPFAULT = 2,
    PNIO_CP_CI_NEWCYCLE = 3,
    PNIO_CP_CI_OPDONE = 4
} PNIO_CP_CI_TYPE;

typedef struct {
    PNIO_BOOL        Valid; /**< true: Min/Max valid */
    PNIO_UINT32      Min;
    PNIO_UINT32      Max;
} PNIO_LIMITS;

typedef struct {
    PNIO_CP_CI_TYPE  Type;
    PNIO_CYCLE_INFO  CycleInfo;
} PNIO_CI_ENTRY;

typedef struct {
    PNIO_BOOL        FaultOccurred; /**< true: all counters stopped */
    char             FaultTime[PNIO_CI_FAULT_TIME_LEN];
    PNIO_LIMITS      StartOp;
    PNIO_LIMITS      OpDone;
    PNIO_LIMITS      ApplDelay;
} PNIO_CYCLE_STAT;

#ifdef __cplusplus
extern "C" {
#endif

PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_cycle_stat(
    PNIO_UINT32              AppHandle,
    int                      MeasureNr,
    PNIO_CYCLE_STAT        * pCycleStat
);

PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_cycle_info(
    PNIO_UINT32              AppHandle,
    PNIO_CI_ENTRY          * pCycleInfoEntry,
    int                      MeasureNr,
    PNIO_UINT32              Offset
);

#ifdef __cplusplus
}
#endif

#endif /* not supported for PNIO SOFTNET */

#if defined(_MSC_VER)
 #pragma pack( pop, safe_old_packing )
#elif defined(BYTE_ATTR_PACKING)
 #include "unpack.h"
#endif

#endif /* PNIOBASE_H */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
