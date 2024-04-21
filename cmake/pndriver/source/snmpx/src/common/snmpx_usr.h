
#ifndef SNMPX_USR_H                     /* ----- reinclude-protection ----- */
#define SNMPX_USR_H

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
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)             :C&  */
/*                                                                           */
/*  F i l e               &F: snmpx_usr.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for SNMPX               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

/*-----------------------------------------------------------------------------
   library
  ---------------------------------------------------------------------------*/

#include "snmpx_lib.h"

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcodes within RQB                                                        */
/*---------------------------------------------------------------------------*/

#define SNMPX_OPC_OPEN_CHANNEL                (LSA_OPCODE_TYPE)   0x01
#define SNMPX_OPC_CLOSE_CHANNEL               (LSA_OPCODE_TYPE)   0x02

/* Agent functionality */
#define SNMPX_OPC_REGISTER_AGENT              (LSA_OPCODE_TYPE)   0x04
#define SNMPX_OPC_UNREGISTER_AGENT            (LSA_OPCODE_TYPE)   0x05
#define SNMPX_OPC_PROVIDE_AGENT               (LSA_OPCODE_TYPE)   0x06
#define SNMPX_OPC_RECEIVE_AGENT               (LSA_OPCODE_TYPE)   0x07

/* Manager functionality */
#define SNMPX_OPC_OPEN_MANAGER_SESSION        (LSA_OPCODE_TYPE)   0x10
#define SNMPX_OPC_CLOSE_MANAGER_SESSION       (LSA_OPCODE_TYPE)   0x11
#define SNMPX_OPC_REQUEST_MANAGER             (LSA_OPCODE_TYPE)   0x12

/* Timer functionality (only internal usage) */
#define SNMPX_OPC_TIMER                       (LSA_OPCODE_TYPE)   0x20

/*---------------------------------------------------------------------------*/
/*  Response within RQB (Response)                                           */
/*---------------------------------------------------------------------------*/

/* SNMPX OK-Code (MaxOffset = 0x3F)  */
/* LSA_RET_OK_OFFSET = 0x40          */

#define SNMPX_OK                          LSA_OK
#define SNMPX_OK_TIMER_RESTARTED          LSA_RET_OK_TIMER_RESTARTED   /* timer has been restarted */
#define SNMPX_OK_TIMER_NOT_RUNNING        LSA_RET_OK_TIMER_NOT_RUNNING /* timer was not running    */

#define SNMPX_OK_CANCEL                   (LSA_RET_OK_OFFSET + 0x01)
#define SNMPX_OK_ACTIVE                   (LSA_RET_OK_OFFSET + 0x02)

#define SNMPX_ERR_RESOURCE                LSA_RET_ERR_RESOURCE
#define SNMPX_ERR_PARAM                   LSA_RET_ERR_PARAM
#define SNMPX_ERR_SEQUENCE                LSA_RET_ERR_SEQUENCE
#define SNMPX_ERR_SYS_PATH                LSA_RET_ERR_SYS_PATH


#define SNMPX_RSP_OK                      SNMPX_OK
#define SNMPX_RSP_ERR_LOC_ABORT           SNMPX_OK_CANCEL
#define SNMPX_RSP_ERR_RESOURCE            SNMPX_ERR_RESOURCE
#define SNMPX_RSP_ERR_PARAM               SNMPX_ERR_PARAM
#define SNMPX_RSP_ERR_SEQUENCE            SNMPX_ERR_SEQUENCE
#define SNMPX_RSP_ERR_SYS_PATH            SNMPX_ERR_SYS_PATH


/* SNMPX ERR-Codes (MaxOffset = 0x3E) */
/* LSA_RET_ERR_OFFSET = 0xC0          */

#define SNMPX_ERR_CHANNEL_USE             (LSA_RET_ERR_OFFSET + 0x01)
#define SNMPX_ERR_SYNTAX                  (LSA_RET_ERR_OFFSET + 0x02)
#define SNMPX_ERR_REF                     (LSA_RET_ERR_OFFSET + 0x03)
#define SNMPX_ERR_LL                      (LSA_RET_ERR_OFFSET + 0x04)
#define SNMPX_ERR_PROTOCOL                (LSA_RET_ERR_OFFSET + 0x05)
#define SNMPX_ERR_NOT_IMPLEMENTED         (LSA_RET_ERR_OFFSET + 0x06)
#define SNMPX_ERR_FAILED                  (LSA_RET_ERR_OFFSET + 0x07)
#define SNMPX_ERR_BUFFER_OVERFLOW         (LSA_RET_ERR_OFFSET + 0x08)
#define SNMPX_ERR_USER_LOCK               (LSA_RET_ERR_OFFSET + 0x09)
#define SNMPX_ERR_DATA_INVALID            (LSA_RET_ERR_OFFSET + 0x0A)
#define SNMPX_ERR_TIMEOUT                 (LSA_RET_ERR_OFFSET + 0x0B)
#define SNMPX_ERR_REM_ABORT               (LSA_RET_ERR_OFFSET + 0x0C)

/*===========================================================================*/
/*                              basic types for SNMPX                        */
/*===========================================================================*/

#define SNMPX_UPPER_MEM_PTR_TYPE         LSA_VOID          *
#define SNMPX_UPPER_MEM_U8_PTR_TYPE      LSA_UINT8         *
#define SNMPX_UPPER_MEM_U16_PTR_TYPE     LSA_UINT16        *
#define SNMPX_UPPER_MEM_U32_PTR_TYPE     LSA_UINT32        *
#define SNMPX_UPPER_MEM_U64_PTR_TYPE     LSA_UINT64        *

#define SNMPX_SOCK_LOWER_MEM_PTR_TYPE     LSA_VOID         *
#define SNMPX_SOCK_LOWER_MEM_U8_PTR_TYPE  LSA_UINT8        *

#define SNMPX_LOCAL_MEM_PTR_TYPE          LSA_VOID         *
#define SNMPX_LOCAL_MEM_U8_PTR_TYPE       LSA_UINT8        *
#define SNMPX_LOCAL_MEM_U16_PTR_TYPE      LSA_UINT16       *
#define SNMPX_LOCAL_MEM_U32_PTR_TYPE      LSA_UINT32       *

#define SNMPX_LOCAL_MEM_U8_CONST_PTR_TYPE const LSA_UINT8  *
#define SNMPX_LOCAL_MEM_BOOL_PTR_TYPE     LSA_BOOL         *

#define SNMPX_UPPER_RQB_MEM_PTR_TYPE      LSA_VOID         *

#define SNMPX_COMMON_MEM_PTR_TYPE         LSA_VOID         *
#define SNMPX_COMMON_MEM_U8_PTR_TYPE      LSA_UINT8        *
#define SNMPX_COMMON_MEM_U16_PTR_TYPE     LSA_UINT16       *
#define SNMPX_COMMON_MEM_U32_PTR_TYPE     LSA_UINT32       *
#define SNMPX_COMMON_MEM_BOOL_PTR_TYPE    LSA_BOOL         *


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct snmpx_rqb_tag * SNMPX_UPPER_RQB_PTR_TYPE;

#define SNMPX_IP_ADDR_SIZE   4

typedef LSA_UINT8 SNMPX_IP_TYPE[SNMPX_IP_ADDR_SIZE];

/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/

/* the SNMP oid of the MIB-II */

#define SNMPX_SNMP_MIB2_TREE            1,3,6,1,2,1
#define SNMPX_SNMP_MIB2_TREE_LEN        6

/* the SNMP oid of the LLDP-MIB */

#define SNMPX_SNMP_LLDP_TREE            1,0,8802,1,1,2
#define SNMPX_SNMP_LLDP_TREE_LEN        6

/*===========================================================================*/
/*                                 SNMP                                      */
/*===========================================================================*/

#define SNMPX_OID_TYPE              LSA_UINT32
#define SNMPX_UPPER_OID_PTR_TYPE    SNMPX_OID_TYPE *

//#define  SNMPX_SNMP_SIZE            1400  /* MAX size of an snmp packet */

#ifndef SNMPX_SNMP_PORT
#define  SNMPX_SNMP_PORT        161  /* reseved UDP port for SNMP  */
#endif
#ifndef SNMPX_SNMP_TRAP_PORT
#define  SNMPX_SNMP_TRAP_PORT   162
#endif

#define  SNMPX_SNMP_MAX_LEN         484  /* max length in bytes of a snmp object */

#define  SNMPX_MAX_OID_LEN          64   /* max number of subid's in a objid */


#define  SNMPX_MAX_COMMUNITY_SIZE   32 /* max chars in a community string */

typedef struct snmpx_community_tag     /* either read or write community */
{
    LSA_UINT16    name_len; /* length of the community string */
    LSA_UINT8     name[SNMPX_MAX_COMMUNITY_SIZE+1]; /* "private" or "public" */
} SNMPX_COMMUNITY_TYPE;

typedef struct snmpx_community_tag * SNMPX_UPPER_COMMUNITY_PTR_TYPE;

/*===========================================================================*/
/*                       Args for RQB (Request Block)                        */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* Opcode: SNMPX_OPC_OPEN_CHANNEL                                            */
/*---------------------------------------------------------------------------*/

#define SNMPX_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */


typedef LSA_VOID  LSA_FCT_PTR(/*ATTR*/,SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE)(SNMPX_UPPER_RQB_PTR_TYPE rb);

typedef struct snmpx_open_channel_tag
{
    LSA_HANDLE_TYPE                 handle;
    /* req: -                                */
    /* cnf: channel-handle of SNMPX          */
    /* At all later RQBs user will write it  */
    /* into handle of SNMPX_RQB_HEADER       */
    LSA_HANDLE_TYPE                 handle_upper;
    /* req: channel-handle of user           */
    /* cnf: -                                */
    /* At all confirmations SNMPX writes it  */
    /* into handle of SNMPX_RQB_HEADER.      */
    LSA_SYS_PATH_TYPE               sys_path;
    /* req: system-path of channel           */
    /* cnf: -                                */
    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE snmpx_request_upper_done_ptr;
    /* req: pointer to callback-function     */
    /* cnf: -                                */
} SNMPX_CHANNEL_TYPE;

typedef struct snmpx_open_channel_tag * SNMPX_UPPER_OPEN_CHANNEL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: SNMPX_OPC_CLOSE_CHANNEL                                           */
/*---------------------------------------------------------------------------*/

/* no args */


/*---------------------------------------------------------------------------*/
/* Opcode: SNMPX_OPC_REGISTER_AGENT, SNMPX_OPC_UNREGISTER_AGENT              */
/*---------------------------------------------------------------------------*/

typedef struct snmpx_register_tag
{
    LSA_UINT16                  Priority;
    LSA_UINT16                  OidLen;
    SNMPX_UPPER_OID_PTR_TYPE    pOid;
} SNMPX_REGISTER_TYPE;

typedef struct snmpx_register_tag * SNMPX_UPPER_REGISTER_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: SNMPX_OPC_PROVIDE_AGENT, SNMPX_OPC_RECEIVE_AGENT,                 */
/*         SNMPX_OPC_REQUEST_MANAGER                                         */
/*---------------------------------------------------------------------------*/

typedef struct snmpx_object_tag
{
    LSA_UINT32                  TransactionID;  /* only for snmp agent   */
    LSA_UINT16                  SessionID;      /* only for snmp manager */
    LSA_UINT8                   MessageType;
    LSA_UINT8                   VarType;
    LSA_UINT16                  AccessControl;
    SNMPX_UPPER_OID_PTR_TYPE    pOid;
    SNMPX_UPPER_MEM_U8_PTR_TYPE pVarValue;
    LSA_UINT32                  OidLen;
    LSA_UINT32                  VarLength;
    LSA_UINT32                  SnmpError;
    LSA_INT32                   SnmpVersion;
} SNMPX_OBJECT_TYPE;

typedef struct snmpx_object_tag * SNMPX_UPPER_OBJECT_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: SNMPX_OPC_OPEN_MANAGER_SESSION, SNMPX_OPC_CLOSE_MANAGER_SESSION   */
/*---------------------------------------------------------------------------*/

typedef struct snmpx_session_tag
{                                                   /* OPEN / CLOSE */
    LSA_UINT16                      SessionID;      /* OUT: OpenSession  */
    /* IN:  CloseSession */
    SNMPX_IP_TYPE                   IPAddress;      /* IN:  OpenSession  */
    SNMPX_UPPER_COMMUNITY_PTR_TYPE  pReadCommunity; /* IN:  OpenSession  */
    SNMPX_UPPER_COMMUNITY_PTR_TYPE  pWriteCommunity;/* IN:  OpenSession  */
} SNMPX_SESSION_TYPE;

typedef struct snmpx_session_tag * SNMPX_UPPER_SESSION_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: SNMPX_OPC_TIMER  (for SNMPX internal use only!)                   */
/*---------------------------------------------------------------------------*/

typedef struct snmpx_timer_tag
{
    LSA_UINT16              TimerID;
    LSA_UINT32              UserID;
} SNMPX_TIMER_TYPE;

typedef struct snmpx_timer_tag * SNMPX_UPPER_TIMER_PTR_TYPE;

#define SNMPX_TIMER_USER_ID_CYLIC    0   /* Snmp-Receive base timer */


/* snmp message types */

#define  SNMPX_GET              (0x0)    /* Get        */
#define  SNMPX_GETNEXT          (0x1)    /* GetNext    */
#define  SNMPX_SET              (0x3)    /* Set        */
#define  SNMPX_TESTSET          (0x10)   /* TestSet    */
#define  SNMPX_COMMITSET        (0x11)   /* CommitSet  */
#define  SNMPX_UNDOSET          (0x12)   /* UndoSet    */
#define  SNMPX_CLEANUPSET       (0x13)   /* CleanupSet */
#define  SNMPX_TRAP             (0x4)    /* Trap       */
#define  SNMPX_GETBULK          (0x5)    /* GetBulk    */


/* types of snmp objects */

/* ASN.1 primitive types */
#define  SNMPX_ASN_INTEGER      (0x02)
#define  SNMPX_ASN_INTEGER32    (0x02)
#define  SNMPX_ASN_BITS         (0x03)
#define  SNMPX_ASN_STRING       (0x04)
#define  SNMPX_ASN_NULLOBJ      (0x05)
#define  SNMPX_ASN_OBJID        (0x06)
/* defined types (from the SMI, RFC 1065) */
#define  SNMPX_ASN_APPLICATION  (0x40)
#define  SNMPX_ASN_IPADDRESS    (SNMPX_ASN_APPLICATION |  0)
#define  SNMPX_ASN_COUNTER      (SNMPX_ASN_APPLICATION |  1)
#define  SNMPX_ASN_COUNTER32    (SNMPX_ASN_APPLICATION |  1)
#define  SNMPX_ASN_GAUGE        (SNMPX_ASN_APPLICATION |  2)
#define  SNMPX_ASN_GAUGE32      (SNMPX_ASN_APPLICATION |  2)
#define  SNMPX_ASN_UNSIGNED32   (SNMPX_ASN_APPLICATION |  2)
#define  SNMPX_ASN_TIMETICKS    (SNMPX_ASN_APPLICATION |  3)
#define  SNMPX_ASN_OPAQUE       (SNMPX_ASN_APPLICATION |  4)
/*       SNMPX_ASN_NSAP         (SNMPX_ASN_APPLICATION |  5) historic */
#define  SNMPX_ASN_COUNTER64    (SNMPX_ASN_APPLICATION |  6)
/*       SNMPX_ASN_UINTEGER     (SNMPX_ASN_APPLICATION |  7) historic */
#define  SNMPX_ASN_FLOAT        (SNMPX_ASN_APPLICATION |  8)
#define  SNMPX_ASN_DOUBLE       (SNMPX_ASN_APPLICATION |  9)
#define  SNMPX_ASN_INTEGER64    (SNMPX_ASN_APPLICATION | 10)
#define  SNMPX_ASN_UNSIGNED64   (SNMPX_ASN_APPLICATION | 11)

/* snmp version */

#define  SNMPX_SNMP_VERSION_1   0
#define  SNMPX_SNMP_VERSION_2   1
#define  SNMPX_SNMP_VERSION_3   3

/* snmp error status */

#define  SNMPX_SNMP_ERR_NOERROR     (0x0)
#define  SNMPX_SNMP_ERR_TOOBIG      (0x1)
#define  SNMPX_SNMP_ERR_NOSUCHNAME  (0x2)
#define  SNMPX_SNMP_ERR_BADVALUE    (0x3)
#define  SNMPX_SNMP_ERR_READONLY    (0x4)
#define  SNMPX_SNMP_ERR_GENERR      (0x5)

/* for SnmpVersion SNMPX_SNMP_VERSION_2 und SNMPX_SNMP_VERSION_3 */

#define  SNMPX_SNMP_ASN_NSO     (0x80)  /* RFC1905:noSuchObject   */
#define  SNMPX_SNMP_ASN_NSI     (0x81)  /* RFC1905:noSuchInstance */
#define  SNMPX_SNMP_ASN_EOM     (0x82)  /* RFC1905:endOfMib       */

/* access-controls for snmp objects */

#define SNMPX_READ_ONLY         0xAAAA  /* read access for everyone */
#define SNMPX_READ_WRITE        0xAABA  /* add write access for community private */
#define SNMPX_NO_ACCESS         0x0000  /* no access for anybody */
#define SNMPX_READ_CREATE       0xBBBB  /* read,write,create access for everyone*/
#define SNMPX_WRITE_ONLY        0xCCCC  /* writeonly access for everyone */
#define SNMPX_NOTIFY_ACCESS     0xDDDD  /* notify access for everyone*/


/*---------------------------------------------------------------------------*/
/*                           RQB- Header for Services                        */
/*---------------------------------------------------------------------------*/

typedef union snmpx_rqb_args_tag
{
    SNMPX_CHANNEL_TYPE          Channel;        /* open/close channel        */
    SNMPX_REGISTER_TYPE         Register;       /* un/register sub-agent     */
    SNMPX_SESSION_TYPE          Session;        /* open/close session        */
    SNMPX_OBJECT_TYPE           Object;         /* snmp object (receive)     */
	SNMPX_TIMER_TYPE            Timer;          /* TimerEvent                */
} SNMPX_RQB_ARGS_TYPE;


typedef struct snmpx_rqb_tag
{
    SNMPX_RQB_HEADER
    SNMPX_RQB_ARGS_TYPE         args;       /* IN/OUT: Args of Request     */
    SNMPX_RQB_TRAILER
} SNMPX_RQB_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/


/*=============================================================================
 * function name:  snmpx_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     SNMPX_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         SNMPX_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_UINT16              Response:       -
 *      SNMPX_RQB_ERROR_TYPE    Error:          -
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(ATTR, Cbf)
 *                                         (SNMPX_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of nare
 *      LSA_UINT16              Response:       SNMPX_OK
 *                                              SNMPX_ERR_PARAM
 *                                              SNMPX_ERR_SEQUENCE
 *                                              or others
 *      SNMPX_RQB_ERROR_TYPE    Error:          Additional errorcodes from
 *                                              lower layer
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID snmpx_open_channel(SNMPX_UPPER_RQB_PTR_TYPE pRQB);

/*=============================================================================
 * function name:  snmpx_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     SNMPX_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         SNMPX_OPC_CLOSE_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_UINT16              Response:       -
 *      SNMPX_RQB_ERROR_TYPE    Error:          -
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      LSA_UINT16              Response:       SNMPX_OK
 *                                              SNMPX_ERR_PARAM
 *                                              SNMPX_ERR_SEQUENCE
 *                                              or others
 *      SNMPX_RQB_ERROR_TYPE    Error:          Additional errorcodes from
 *                                              lower layer
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID snmpx_close_channel(SNMPX_UPPER_RQB_PTR_TYPE pRQB);


/*=============================================================================
 * function name:  snmpx_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     SNMPX_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     SNMPX_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of nare
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_UINT16              Response:   -
 *      SNMPX_RQB_ERROR_TYPE    Error:      -
 *      SNMPX_ARGS_TYPE         Args:       Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     SNMPX_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_UINT16              Response:   SNMPX_OK
 *                                          SNMPX_ERR_PARAM
 *                                          SNMPX_ERR_SEQUENCE
 *                                          or others
 *      SNMPX_RQB_ERROR_TYPE    Error:      Additional errorcodes from lower
 *                                          layer
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID snmpx_request(SNMPX_UPPER_RQB_PTR_TYPE  upper_rqb_ptr);


/*----------------------------------------------------------------------------*/
/* oid to ascii() */
/* Return Value   */
/* snmpx_oid2a returns the number of characters written, not including the terminating null character, */
/* or a negative value if an output error occurs. A negative value is also returned if oidlen or more  */
/* wide characters are requested to be written. */
/*----------------------------------------------------------------------------*/
LSA_INT snmpx_oid2a (const SNMPX_UPPER_OID_PTR_TYPE oid, LSA_UINT16 oidlen,
                     SNMPX_COMMON_MEM_U8_PTR_TYPE s_oid, LSA_UINT16 s_oidlen);

/* FUNCTION: snmpx_print_oid()
 *
 * format an oid string into a buffer for display.
 * Buffer is static, and so must be used or copied before
 * the next call is made.
 *
 * PARAM1:  oid * oidp - Pointer to OID array (IN)
 * PARAM2: int len - Length of array (IN)
 *
 * REMARKS: This function has been picked from InstantStation code
 * No changes done.
 *
 * RETURNS: A pointer to the display buffer.
 */
char * snmpx_print_oid(
    const SNMPX_OID_TYPE * oidp,  /* pointer to an array of type oid */
    unsigned   oid_len,       /* number of oids in array */
    unsigned * buf_len);  /* number of bytes written */

/*------------------------------------------------------------------------------------*/
/*  snmpx_ultoa converts an unsigned long value to a null-terminated string using     */
/*  the specified base and stores the result in the array given by str parameter.     */
/*  str should be an array long enough to contain any possible value:                 */
/*  (sizeof(long)*8+1) for radix=2, i.e. 33 bytes in 32-bits platforms.               */
/*                                                                                    */
/*  Parameters:                                                                       */
/*  value   Value (only positive) to be converted to a string.                        */
/*  str     Array in memory where to store the resulting null-terminated string.      */
/*  base    Numerical base used to represent the value as a string, between 2 and 16, */
/*          where 10 means decimal base, 16 hexadecimal, 8 octal, and 2 binary.       */
/*  numbers Fills up with leading zeros if numbers <> 0 is given.                     */
/*          If the buffer is too small, you risk a buffer overflow.                   */
/*                                                                                    */
/* returns the number of characters written, not including the terminating            */
/* null character or 0 if an output error occurs.                                     */
/*------------------------------------------------------------------------------------*/
LSA_INT snmpx_ultoa(
	LSA_UINT32 value,
	SNMPX_COMMON_MEM_U8_PTR_TYPE str,
	LSA_UINT base,
	LSA_INT numbers);

/* FUNCTION: snmpx_handle_snmp_request()
 *
 * called by SOCK to deliver a SNMP packet
 *
 * PARAM1: LSA_VOID* p_snmp_data: buffer containing SNMP message
 * PARAM2: LSA_INT snmp_data_length: length of data in buffer p_snmp_data
 *
 * REMARKS:async function; on completion, SNMPX calls SNMPX_HANDLE_SNMP_REQUEST_DONE
 */
LSA_VOID
snmpx_handle_snmp_request(
	LSA_VOID* p_snmp_data,
	LSA_INT snmp_data_length
);

/* FUNCTION: SNMPX_HANDLE_SNMP_REQUEST_DONE
 *
 * called by SNMPX to deliver result of a SNMP request
 *
 * PARAM1: LSA_VOID* p_snmp_data: buffer containing SNMP message
 * PARAM2: LSA_INT snmp_data_length: length of data in buffer p_snmp_data
 *
 * REMARKS: this function is called by SNMPX to deliver a SNMP response to SOCK
 */
LSA_INT
SNMPX_HANDLE_SNMP_REQUEST_DONE (
	LSA_VOID* p_snmp_data,
	LSA_INT snmp_data_length
);

/*====  out functions  =====*/


/*****************************************************************************/
/*  end of file snmpx_usr.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_USR_H */
