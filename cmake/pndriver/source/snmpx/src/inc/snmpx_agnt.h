
#ifndef SNMPX_AGNT_H                        /* ----- reinclude-protection ----- */
#define SNMPX_AGNT_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/** Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved. */
/** Copyright 1990-96 by NetPort Software. All rights reserved.              */
/** Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved. */
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
/*  P r o j e c t         &P: Layer Stack Architecture (LSA)    :P&          */
/*                                                                           */
/*  P a c k a g e         &W: SNMPX                             :W&          */
/*                                                                           */
/*  C o m p o n e n t     &C: SNMPX                             :C&          */
/*                                                                           */
/*  F i l e               &F: snmpx_agnt.h                      :F&          */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15 :V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SNMP-Library of SNMPX-Agent                                              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

#ifndef INCLUDE_SNMPV1
#define INCLUDE_SNMPV1        1   /* SNMPv1 library, agent & hook */
#endif
/* #define INCLUDE_SNMPV2C       1  */ /* Include SNMPv2c over SNMPv1 */
/* #define INCLUDE_SNMPV3        1  */ /* Include SNMPv3 code in build */

/* If traps are not enabled, then snmp_trap() does nothing */
#ifndef snmpx_snmp_trap
#define  snmpx_snmp_trap(_v, _w, _x, _y, _z)
#endif

/* per-port local limitations */
#define  SNMPX_MAX_COMMUNITY_SIZE   32 /* max chars in a community string */

#define  SNMPX_SYS_STRING_MAX    128         /* max length of sys group strings */

#define  GET_REQ_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x0)
#define  GETNEXT_REQ_MSG   (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x1)
#define  GET_RSP_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x2)
#define  SET_REQ_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x3)
#define  TRP_REQ_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x4)
#define  GETBULK_REQ_MSG   (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x5)
#define  V3_INFORM_PDU     (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x6)
#define  V3_TRAP_PDU       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x7)
#define  V3_REPORT_PDU     (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x8)

/* values of various error codes as defined in RFC1905 */
#define  V3_VB_NOERROR              0
#define  V3_VB_TOOBIG               1
#define  V3_VB_NOSUCHNAME           2
#define  V3_VB_BADVALUE             3
#define  V3_VB_READONLY             4
#define  V3_VB_GENERR               5
#define  V3_VB_NOACCESS             6
#define  V3_VB_WRONGTYPE            7
#define  V3_VB_WRONGLENGTH          8
#define  V3_VB_WRONGENCODING        9
#define  V3_VB_WRONGVALUE           10
#define  V3_VB_NOCREATION           11
#define  V3_VB_INCONSISTENTVALUE    12
#define  V3_VB_RESOURCEUNAVAILABLE  13
#define  V3_VB_COMMITFAILED         14
#define  V3_VB_UNDOFAILED           15
#define  V3_VB_AUTHORIZATIONERROR   16
#define  V3_VB_NOTWRITABLE          17
#define  V3_VB_INCONSISTENTNAME     18

/* RowStatus values */
#define  SNMPX_RS_INVALID            0
#define  SNMPX_RS_ACTIVE             1
#define  SNMPX_RS_NOTINSERVICE       2
#define  SNMPX_RS_NOTREADY           3
#define  SNMPX_RS_CREATEANDGO        4
#define  SNMPX_RS_CREATEANDWAIT      5
#define  SNMPX_RS_DESTROY            6

/* StorageType values */
#define  SNMPX_ST_OTHER              1
#define  SNMPX_ST_VOLATILE           2
#define  SNMPX_ST_NONVOLATILE        3
#define  SNMPX_ST_PERMANENT          4
#define  SNMPX_ST_READONLY           5

/*  Error codes:  */
/*  These must not clash with SNMP error codes (all positive).  */
#define  PARSE_ERROR -1
#define  BUILD_ERROR -2
#define  AUTH_ERROR  -3

#define  SEQUENCE (ASN_SEQUENCE  |  ASN_CONSTRUCTOR)


struct communityId
{
    char  name[SNMPX_MAX_COMMUNITY_SIZE];
    int   access;  /* either RONLY or RWRITE */
};

#ifndef NUM_COMMUNITIES
#define NUM_COMMUNITIES   2
#endif

extern   struct communityId   communities[NUM_COMMUNITIES];

/* if READ and WRITE are not defined in the development system or
snmpport.h, define them here: */
#ifndef SNMPX_READ
#define SNMPX_READ  0
#endif
#ifndef SNMPX_WRITE
#define SNMPX_WRITE 1
#endif

/* map Berkeley-ish lib calls into standard C lib:             */
/* #ifndef bcopy                                               */
/* #define bcopy(src,  dest, len)  MEMCPY(dest,   src,  len)   */
/* #endif                                                      */

/* values for "oper", passed to var_ routines.    */
#define  SNMPX_GET_OP   1  /* oper is GET operation     */
#define  SNMPX_NEXT_OP  0  /* oper is GETNEXT operation */
#define  SNMPX_SET_OP   -1 /* oper is SET operation     */

/* generate flags from SecurityLevel
 * Output one of these 3 values.
 * noAuthnoPriv  - 0   - bit 0 is disabled, bit 1 is disabled
 * AuthNoPriv    - 1   - bit 0 is enabled,  bit 1 is disabled
 * AuthPriv      - 3   - bit 0 is enabled,  bit 1 is enabled
 */
#define SNMPX_SL2F(sl)  ((sl == 1) ? 0 : ((sl ==2) ? 1 : 3))

#ifndef SNMPX_LOGOID
# define SNMPX_LOGOID(_a,_b,_c,_d,_e, _f)
#endif
#define SNMPX_LOGTRAP(_a,_b)

#ifdef SNMPX_CFG_SNMP_AGENT
 extern   struct snmpx_snmp_mib   SnmpxSnmpMib;

# define  SNMPX_MIB_CTR(varname);  SnmpxSnmpMib.varname++;    /* increment */
# define  SNMPX_MIB_DEC(varname);  SnmpxSnmpMib.varname--;    /* decrement */
#else
# define  SNMPX_MIB_CTR(varname);
# define  SNMPX_MIB_DEC(varname);
#endif

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

LSA_EXTERN void snmpx_snmp_shift_array(U_CHAR*, int, int);
LSA_EXTERN int  snmpx_snmp_goodValue(U_CHAR *, U_CHAR, int, U_CHAR, int);
LSA_EXTERN int  snmpx_snmp_setVariable(U_CHAR*, U_CHAR, U_CHAR*, int);

LSA_EXTERN U_CHAR * snmpx_snmp_parse_var_op(U_CHAR*, SNMPX_OID_TYPE*, unsigned*, U_CHAR*,
                                            unsigned*, U_CHAR**, unsigned*);
LSA_EXTERN U_CHAR * snmpx_snmp_build_var_op(U_CHAR*, SNMPX_OID_TYPE*, unsigned, U_CHAR,
                                            unsigned, U_CHAR*, unsigned*);
LSA_EXTERN LSA_BOOL /* int */ snmpx_snmp_build_adjust_len(U_CHAR * start_hdr, U_CHAR * start_data,
                                                          unsigned len_data, int *diff_hdr, unsigned *len_final);
LSA_EXTERN int snmpx_snmp_build_var_op_list(U_CHAR * headerP, U_CHAR * var_list_start,
											U_CHAR * out_data, unsigned out_length, long *index);
int snmpx_snmp_create_identical(U_CHAR * snmp_in, U_CHAR * snmp_out,
                                unsigned snmp_length, long errstat, long errindex);
int snmpx_snmp_create_identical_no_var_bind(U_CHAR * snmp_in, U_CHAR * snmp_out,
                                            unsigned snmp_length, long errstat, long errindex);

/* Set the PDU parsing routine.
 *   For SNMPv1, use snmp_parse_var_op_list() to parse SNMPv1 varbinds
 *   For SNMPv2c, use snmp_parse_var_op_v2list() to parse SNMPv1, SNMPv2c varbinds
 */
#ifdef INCLUDE_SNMPV2C
#define SNMP_PARSE_VARBIND   snmpx_snmp_parse_var_op_v2list
#else
#define SNMP_PARSE_VARBIND   snmpx_snmp_parse_var_op_list
#endif

int snmpx_snmp_parse_var_op_list(U_CHAR*, unsigned, U_CHAR*, unsigned, U_CHAR, long*,
                                 long, int version);
int snmpx_snmp_parse_var_op_v2list(U_CHAR*, unsigned, U_CHAR*, unsigned, U_CHAR, long*,
                                   long, int version);
U_CHAR * snmpx_snmp_auth_parse(U_CHAR*, unsigned*, U_CHAR*, unsigned*, long*);
U_CHAR * snmpx_snmp_auth_build(U_CHAR*, unsigned*, U_CHAR*, unsigned*, long*,
                               unsigned);

LSA_EXTERN int      snmpx_snmp_sizeof_int(long value);

LSA_EXTERN U_CHAR * snmpx_snmp_getStatPtr(SNMPX_OID_TYPE* name, unsigned* namelen, U_CHAR* type, unsigned* len,
                                          U_SHORT* acl, int exact, long* err_code, int version);

LSA_EXTERN void snmpx_snmp_oid2bytes(char* octetbuf, SNMPX_OID_TYPE* oidbuf, int count); /* convert octets */

LSA_EXTERN int snmpx_snmp_get_num_variables(void);

LSA_EXTERN char * snmpx_print_value(U_CHAR * var_val, U_CHAR var_val_type);

/*****************************************************************************/
/*  end of file snmpx_agnt.h                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* end of SNMPX_AGNT_H */
