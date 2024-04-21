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
/*  P a c k a g e         &W: SNMPX (SNMP eXtensible agent)     :W&          */
/*                                                                           */
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)     :C&          */
/*                                                                           */
/*  F i l e               &F: snmpx_agnt.c                      :F&          */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15 :V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX SNMP-Agent functions                       */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  19
#define SNMPX_MODULE_ID    LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_AGNT */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles */
#include "snmpx_int.h"            /* internal header   */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

#include "snmpx_asn1.h"
#include "snmpx_agnt.h"           /* SNMPX-Agent header  */
#ifdef SNMPX_CFG_SNMP_AGENT
#include "snmpx_snmp.h"           /* SNMP header         */
# include "snmpx_mibs.h"          /* SNMP group of MIB2  */
#endif

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

 #ifdef SNMPX_CFG_SNMP_AGENT
static struct communityId   snmpx_communities[NUM_COMMUNITIES]  =
{
    {"private", SNMPX_READ_WRITE},
    {"public", SNMPX_READ_ONLY},
};
#endif

#if 0
static char * snmpx_errcode[19] =
{
    "success" ,            /* V3_VB_NOERROR              */
    "tooBig" ,             /* V3_VB_TOOBIG               */
    "noSuchName" ,         /* V3_VB_NOSUCHNAME           */
    "badValue" ,           /* V3_VB_BADVALUE             */
    "readOnly" ,           /* V3_VB_READONLY             */
    "genErr" ,             /* V3_VB_GENERR               */
    "noAccess" ,           /* V3_VB_NOACCESS             */
    "wrongType" ,          /* V3_VB_WRONGTYPE            */
    "wrongLength" ,        /* V3_VB_WRONGLENGTH          */
    "wrongEncoding" ,      /* V3_VB_WRONGENCODING        */
    "wrongValue" ,         /* V3_VB_WRONGVALUE           */
    "noCreation" ,         /* V3_VB_NOCREATION           */
    "inconsistentValue" ,  /* V3_VB_INCONSISTENTVALUE    */
    "resourceUnavailable" ,/* V3_VB_RESOURCEUNAVAILABLE  */
    "commitFailed" ,       /* V3_VB_COMMITFAILED         */
    "undoFailed" ,         /* V3_VB_UNDOFAILED           */
    "authorizationError" , /* V3_VB_AUTHORIZATIONERROR   */
    "notWritable" ,        /* V3_VB_NOTWRITABLE          */
    "inconsistentName" ,   /* V3_VB_INCONSISTENTNAME     */
};
#endif

static U_CHAR   snmpx_sidbuf[SNMPX_MAX_COMMUNITY_SIZE  +  1];   /* Community string buffer */
static unsigned snmpx_sidlen = 0;

/* these can't be global in a re-entrant system */
const U_CHAR * snmpx_packet_end  =  LSA_NULL; /* end of built packet data */

#ifdef SNMPX_CFG_SNMP_AGENT
static const int snmpx_community;  /* access code for current packet's community string */

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

int snmpx_snmp_access(U_SHORT acl, unsigned community, unsigned rw);
/*  int snmp_get_community(U_CHAR*);                      */
int snmpx_snmp_get_community(U_CHAR*, unsigned);

#ifdef INCLUDE_SNMPV3
/* extern int v3_community_check(U_CHAR * rcvdcomm);      */
extern int snmpx_snmp_v3_community_check(U_CHAR * rcvdcomm, unsigned sidlen);
#define SNMP_COMMUNITY_CHECK snmpx_snmp_v3_community_check
#else
#define SNMP_COMMUNITY_CHECK snmpx_snmp_get_community
#endif


/* FUNCTION: snmp_agt_parse()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned length
 * PARAM3: U_CHAR * out_data
 * PARAM4: unsigned out_length
 *
 * snmp_agt_parse() - This is the main entry point for incoming snmp
 * packets directed to the agent's UDP connection. As an embedded
 * agent we will get these packets by direct upcalls from the UDP
 * layer. We check authentication, community, message type, and build
 * a response packet.
 *
 * RETURNS:
 * Returns 0 if no response packet was left in
 * out_data, else returns size of response packet;
 *
 */

LSA_INT snmpx_agt_parse_snmp (
    LSA_UINT8 * data,   /* IN pointer to packet data (past UDP header) */
    LSA_UINT    length,        /* IN/SCRTACH length of packet data */
    LSA_UINT8 *    out_data,      /* IN/OUT buffer for response packet */
    LSA_UINT    out_length,     /* IN/SCRTACH size of output buffer */
    LSA_UINT16 * Response)
{
    U_CHAR   msg_type,   type;
    long     zero  =  0L;
    long     reqid,   errstat, errindex, outerrstat;
    U_CHAR * out_auth;   /* this also happens to be start of out buffer */
    U_CHAR * out_header, *  out_reqid;
    U_CHAR * startData   =  data;
    int      startLength =  length;
    long     version;
    int      header_shift,  auth_shift;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_agt_parse_snmp(data: 0x%X, length: 0x%X, out_data: 0x%X, out_length: 0x%X)",
                            data, length, out_data, out_length);

    SNMPX_MIB_CTR(snmpInPkts);     /* update MIB-II stats */

    *Response = SNMPX_OK;

    if (length > SNMPX_SNMP_SIZE)
    {
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): incoming pkt too big");
        SNMPX_MIB_CTR(snmpInTooBigs);

        /* truncate incoming packet & try to build error reply */
        length = SNMPX_SNMP_SIZE;
        outerrstat = SNMPX_SNMP_ERR_TOOBIG;
    }
    else
    {
        outerrstat = SNMPX_SNMP_ERR_NOERROR;
        errindex = 0;
    }

    snmpx_sidlen = SNMPX_MAX_COMMUNITY_SIZE;
    /* authenticates message and returns length if valid */
    data = snmpx_snmp_auth_parse(data, &length, snmpx_sidbuf, &snmpx_sidlen, &version);
    if (data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpInASNParseErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): bad authentication");
        snmpx_snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, LSA_NULL, version);
        return 0;
    }
#if (defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
    /* Both SNMPv1 and SNMPv2c are defined. Discard any other version */
    if (!((version == SNMP_VERSION_1) || (version == SNMP_VERSION_2)))
    {
        SNMPX_MIB_CTR(snmpInBadVersions);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): wrong version");
        return 0;
    }
#endif
#if (defined(INCLUDE_SNMPV1) && !defined(INCLUDE_SNMPV2C))
    /* only SNMPv1 is defined. Discard SNMPv2c packets */
    if (version != SNMPX_SNMP_VERSION_1)
    {
        SNMPX_MIB_CTR(snmpInBadVersions);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): wrong version");
        return 0;
    }
#endif
#if (!defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
    /* only SNMPv2c is defined. Discard SNMPv1 packets */
    if (version != SNMPX_SNMP_VERSION_2)
    {
        SNMPX_MIB_CTR(snmpInBadVersions);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): wrong version");
        return 0;
    }
#endif

    /* snmpx_community = SNMP_COMMUNITY_CHECK(snmpx_sidbuf); */ /* set to RONLY or RWRITE */
    snmpx_community = SNMP_COMMUNITY_CHECK(snmpx_sidbuf, snmpx_sidlen); /* set to RONLY or RWRITE */
    if (snmpx_community == -1)
    {
        SNMPX_MIB_CTR(snmpInBadCommunityNames);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): bad community");
        snmpx_snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, LSA_NULL, version);
        return 0;
    }
    data = snmpx_asn_parse_header(data, &length, &msg_type);
    if (data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpInGenErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): bad header");
        return 0;
    }

    /* check message type, support mib counters: */
    switch (msg_type)
    {
        case GETNEXT_REQ_MSG:
            SNMPX_MIB_CTR(snmpInGetNexts);
            break;
        case GET_REQ_MSG:
            SNMPX_MIB_CTR(snmpInGetRequests);
            break;
        case SET_REQ_MSG:
            SNMPX_MIB_CTR(snmpInSetRequests);
            break;
        case GETBULK_REQ_MSG:
            /* no counter for GETBULK in struct snmp_mib */
            break;
        default:
            SNMPX_MIB_CTR(snmpInASNParseErrs);
            SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): Unsupported message type");
            return 0;   /* check for traps, responses, later */
    }

    data = snmpx_asn_parse_int(data, &length, &type, (long *)&reqid, sizeof(reqid));
    if (data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpInASNParseErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): bad parse of reqid");
        return 0;
    }
    data = snmpx_asn_parse_int(data, &length, &type, (long *)&errstat, sizeof(errstat));
    if (data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpInASNParseErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): bad parse of errstat");
        return 0;
    }
    data = snmpx_asn_parse_int(data, &length, &type, (long *)&errindex, sizeof(errindex));
    if (data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpInASNParseErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): bad parse of errindex");
        return 0;
    }
    /*
     * Now start cobbling together what is known about the output packet.
     * The final lengths are not known now, so they will have to be recomputed
     * later.
     */
    out_auth = out_data;
    out_header = snmpx_snmp_auth_build(out_auth, &out_length, snmpx_sidbuf, &snmpx_sidlen, &version, 0);
    if (out_header == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpOutGenErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): snmp_auth_build failed");
        return 0;
    }
    out_reqid = snmpx_asn_build_header(out_header, &out_length, (U_CHAR)GET_RSP_MSG, 0);
    if (out_reqid == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpOutGenErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): build opcode failed");
        return 0;
    }

    type = (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    /* return identical request id */
    out_data = snmpx_asn_build_int(out_reqid, &out_length, type, &reqid, sizeof(reqid));
    if (out_data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpOutGenErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): build reqid failed");
        return 0;
    }

    /* assume that error status will be zero */
    out_data = snmpx_asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
    if (out_data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpOutGenErrs);
        SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): build errstat failed");
        return 0;
    }

    /* assume that error index will be zero */
    out_data = snmpx_asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
    if (out_data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpOutGenErrs);
        SNMPX_SNMPERROR("snmpx_snmp_agt_parse(): build errindex failed");
        return 0;
    }

    if (outerrstat == SNMPX_SNMP_ERR_NOERROR )
    {
        if (msg_type != SET_REQ_MSG)
        {
            outerrstat = SNMP_PARSE_VARBIND(data, length, out_data, out_length,
                                            msg_type, &errindex, errstat, version);
        }
        else     /* just set the variable and exit */
        {
            outerrstat = SNMP_PARSE_VARBIND(data, length, out_data, out_length,
                                            msg_type, &errindex, errstat, version);

            if (outerrstat < 0)  /* Usually AUTH_ERROR - bad set community */
            {
                return 0;
            }
#if 0
            /* create the get response packet for the set operation */
            if (outerrstat == SNMPX_SNMP_ERR_NOSUCHNAME)
            {                          /* only if getStatPtr() delivers NoSuchName */
                * snmpx_oid = LSA_FALSE;   /* it's not an oid for SNMPX */
            }
#endif

            return (snmpx_snmp_create_identical(startData, out_auth, startLength,
                                                outerrstat, errindex));
        }
    }

    if (outerrstat < 0) return 0;    /* usually AUTH_ERROR */

    switch ((short)outerrstat)
    {
        case SNMPX_SNMP_ERR_NOERROR:
            /*
             * Because of the assumption above that header lengths would be encoded
             * in one byte, things need to be fixed, now that the actual lengths are known.
             */
            header_shift = 0;
            out_length = snmpx_packet_end - out_reqid;
            if (out_length >= 0x80)
            {
                header_shift++;
                if (out_length > 0xFF)
                    header_shift++;
            }
            auth_shift = 0;
            out_length = (snmpx_packet_end - out_auth) - 2 + header_shift;
            if (out_length >= 0x80)
            {
                auth_shift++;
                if (out_length > 0xFF)
                    auth_shift++;
            }
            if (auth_shift + header_shift)
            {
                /*
                 * Shift packet (from request id to end of packet) by the sum of the
                 * necessary shift counts.
                 */
                snmpx_snmp_shift_array(out_reqid, snmpx_packet_end - out_reqid, auth_shift + header_shift);
                /* Now adjust pointers into the packet */
                snmpx_packet_end += auth_shift + header_shift;
                out_reqid += auth_shift + header_shift;
                out_header += auth_shift;
            }

            /* re-encode the headers with the real lengths */
            out_data = out_header;
            out_length = snmpx_packet_end - out_reqid;
            out_data = snmpx_asn_build_header(out_data, &out_length, GET_RSP_MSG, out_length);
            if (out_data != out_reqid)
            {
                SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): internal error, header");
                return 0;
            }
            out_data = out_auth;
            out_length = snmpx_packet_end - out_auth;
            out_data = snmpx_snmp_auth_build(out_data, &out_length, snmpx_sidbuf, &snmpx_sidlen,
                                             &version, snmpx_packet_end - out_header);
            if (out_data != out_header)
            {
                SNMPX_SNMPERROR("snmpx_agt_parse_snmp(): internal error");
                return 0;
            }
            SNMPX_MIB_CTR(snmpOutGetResponses);
            break;

        case SNMPX_SNMP_ERR_NOSUCHNAME:
            SNMPX_MIB_CTR(snmpOutNoSuchNames);
            goto OutErrPkt;
        case SNMPX_SNMP_ERR_TOOBIG:
            SNMPX_MIB_CTR(snmpOutTooBigs);
            errindex = 0;  /* SNMPV1/2 expect err_index=0, and no var bind in reply */
            if (version == SNMPX_SNMP_VERSION_1) /* for SNMPV1 return all Var Bind's */
                return(snmpx_snmp_create_identical(startData, out_auth, startLength, outerrstat, errindex));
            else   /* for SNMPV2 don't return any VB (same for SNMPV3) */
                return(snmpx_snmp_create_identical_no_var_bind(startData, out_auth, startLength, outerrstat, errindex));
        case SNMPX_SNMP_ERR_BADVALUE:
            SNMPX_MIB_CTR(snmpOutBadValues);
            goto OutErrPkt;
        case SNMPX_SNMP_ERR_READONLY:
            SNMPX_MIB_CTR(snmpInBadCommunityUses);
            goto OutErrPkt;
        case SNMPX_SNMP_ASN_EOM: /* endOfMib */
        case SNMPX_SNMP_ERR_GENERR:
            SNMPX_MIB_CTR(snmpOutGenErrs);

OutErrPkt:
            return(snmpx_snmp_create_identical(startData, out_auth, startLength,
                                               outerrstat, errindex));
        default:
            /* SNMPX_MIB_CTR(snmpSilentDrops); */
            return 0;
    }

    /* Assume this will result in an SNMP send & update MIB-II stats */
    SNMPX_MIB_CTR(snmpOutPkts);
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_agt_parse_snmp(RetLength: 0x%X)", (snmpx_packet_end - out_auth));
    return(snmpx_packet_end - out_auth);
}

#endif	/* SNMPX_CFG_SNMP_AGENT */


#ifndef INCLUDE_SNMPV2C

int snmpx_snmp_build_var_op_list(
    U_CHAR * headerP,
    U_CHAR * var_list_start,
    U_CHAR * out_data,
    unsigned out_length,
    long *      index)
{
    unsigned dummyLen;
    int      header_shift;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_build_var_op_list(headerP: 0x%X, var_list_start: 0x%X, out_data: 0x%X, out_length: 0x%X, index: 0x%X)",
                            headerP, var_list_start, out_data, out_length, index);

    snmpx_packet_end = out_data;  /* save a pointer to the end of the packet */

    SNMPX_ASSERT_NULL_PTR(index);
    /*
    * Because of the assumption above that header lengths would be encoded
    * in one byte, things need to be fixed, now that the actual lengths are known.
    */
    header_shift = 0;
    out_length = (unsigned)(snmpx_packet_end - var_list_start);
    if (out_length >= 0x80)
    {
        header_shift++;
        if (out_length > 0xFF)
            header_shift++;
    }

    if (header_shift)
    {
        snmpx_snmp_shift_array(var_list_start, snmpx_packet_end - var_list_start, header_shift);
        snmpx_packet_end += header_shift;
        var_list_start += header_shift;
    }

    /* Now rebuild header with the actual lengths */
    dummyLen = (unsigned)(snmpx_packet_end - var_list_start);
    if (snmpx_asn_build_header(headerP, &dummyLen, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == LSA_NULL)
    {
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_build_var_op_list() - SNMPX_SNMP_ERR_TOOBIG");
        return SNMPX_SNMP_ERR_TOOBIG;    /* bogus error ???? */
    }
    *index = 0;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_build_var_op_list() - SNMPX_SNMP_ERR_NOERROR");
    return SNMPX_SNMP_ERR_NOERROR;
}


#ifdef SNMPX_CFG_SNMP_AGENT

/* FUNCTION: snmp_parse_var_op_list()
 *
 * Parse_var_op_list goes through the list of variables and retrieves
 * each one, placing it's value in the output packet. If doSet is
 * non-zero, the variable is set with the value in the packet. If any
 * error occurs, an error code is returned.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned   length
 * PARAM3: U_CHAR *   out_data
 * PARAM4: unsigned   out_length
 * PARAM5: U_CHAR     msgtype
 * PARAM6: long *     index
 * PARAM6: long       errstat
 * PARAM7: int        version
 *
 * RETURNS:
 * Returns one of SNMP_ERR codes.
 * If there is no error, it returns SNMP_ERR_NOERROR
 * Some of the other error codes returned are SNMP_ERR_NOSUCHNAME,
 * SNMP_ERR_BADVALUE, and SNMP_ERR_TOOBIG.
 */

int snmpx_snmp_parse_var_op_list(
    U_CHAR * data,
    unsigned    length,
    U_CHAR *    out_data,
    unsigned    out_length,
    U_CHAR      msgtype,
    long *      index,
    long        errstat,
    int         version)
{
    U_CHAR   type;
    int      rw /*, exact */;
    U_CHAR * headerP, *  var_list_start;
    int ret_val;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_parse_var_op_list(data: 0x%X, length: 0x%X, out_data: 0x%X, out_length: 0x%X, msgtype: 0x%X)",
                            data, length, out_data, out_length, msgtype);

    LSA_UNUSED_ARG(errstat);

    /* set exact & rw flag based on message type, which has already  */
    /* been checked for a valid type by caller. */
    if ((msgtype == GETNEXT_REQ_MSG) ||
        (msgtype == GETBULK_REQ_MSG))
    {
        rw = SNMPX_READ;
        /* exact = SNMPX_NEXT_OP; */
    }
    else     /* must be a plain GET or SET */
    {
        if (msgtype == SET_REQ_MSG)
        {
            rw = SNMPX_WRITE;
            /* exact = SNMPX_SET_OP; */
        }
        else
        {
            rw = SNMPX_READ;
            /* exact = SNMPX_GET_OP; */
        }
    }

    data = snmpx_asn_parse_header(data, &length, &type);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_parse_var_op_list(): not enough space for varlist");
        return PARSE_ERROR;
    }
    if (type != (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        SNMPX_SNMPERROR("snmpx_snmp_parse_var_op_list(): wrong type");
        return PARSE_ERROR;
    }
    headerP = out_data;
    out_data = snmpx_asn_build_header(out_data, &out_length, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    if (out_data == LSA_NULL)
    {
        SNMPX_MIB_CTR(snmpOutTooBigs);
        SNMPX_SNMPERROR("snmpx_snmp_parse_var_op_list(): not enough space in output packet");
        return BUILD_ERROR;
    }

    var_list_start = out_data;
    *index = 1;
    snmpx_set_parms.pduflags = 0;  /* clear the pduflags field */

    while ((int)length > 0)
    {
        U_CHAR   var_val_type, * var_val /*, statType*/;
        static SNMPX_OID_TYPE   var_name[SNMPX_MAX_OID_LEN];
        unsigned var_name_len, var_val_len;

        SNMPX_UPPER_OBJECT_PTR_TYPE pObject;

        /* clear static vars for set paramters */
        snmpx_set_parms.access_method = LSA_NULL;
        snmpx_set_parms.do_range = LSA_FALSE;
        snmpx_set_parms.vbflags = 0;

        /* parse the name, value pair */
        var_name_len = SNMPX_MAX_OID_LEN;
        data = snmpx_snmp_parse_var_op(data, var_name, &var_name_len, &var_val_type,
                                       &var_val_len, &var_val, &length);
        if (data == LSA_NULL)
        {
            return PARSE_ERROR;
        }

        /* init agents passing for their MIB-Trees */
        SNMPX_InitMibTrees();

        pObject = SNMPX_GetCurrentObject();

        /* now attempt to retrieve the variable on the corresponding entity */
        SNMPX_OID_Switcher(pObject, var_name, var_name_len,     /* Oid, OidLen */
                           SNMPX_GetUserMessageType(msgtype), version);

        if (snmpx_is_null(pObject))
        {
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }

        if (pObject->pVarValue == LSA_NULL)
        {
            return SNMPX_SNMP_ERR_NOSUCHNAME;
        }

        /* Check if this user has access rights to this variable */
        if (!snmpx_snmp_access(pObject->AccessControl, snmpx_community, rw))
        {
            SNMPX_SNMPERROR("snmpx_snmp_parse_var_op_list(): SNMP agent authentication failed\n");
            SNMPX_LOGOID(snmpx_sidbuf, var_name, var_name_len, SNMPX_SNMP_ERR_READONLY,
                         var_val, var_val_type);
            snmpx_snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, LSA_NULL, version);
            return (snmpx_community == (int)SNMPX_READ_WRITE) ? SNMPX_SNMP_ERR_NOSUCHNAME : AUTH_ERROR;
        }

        if (msgtype == SET_REQ_MSG)
        {
            /* see if the type and value is consistent with this
                entry's variable. Do range checking */
            if (snmpx_snmp_goodValue(var_val, var_val_type, var_val_len,
                                     pObject->VarType, pObject->VarLength) /*!= TRUE*/)
            {
                SNMPX_LOGOID(snmpx_sidbuf, var_name, var_name_len,
                             SNMPX_SNMP_ERR_BADVALUE, var_val, var_val_type);
                return SNMPX_SNMP_ERR_BADVALUE;
            }

            /* actually do the set if necessary */
            if (msgtype == SET_REQ_MSG)
            {
                int   err;
                /* see if access_method was set in snmp_getStatPtr() call */
                if (snmpx_set_parms.access_method)
                {
                    /* call port routine for custom set of variable */
                    err = (*snmpx_set_parms.access_method)(var_val, var_val_type,
                                                           var_val_len, pObject->pVarValue, pObject->VarLength);
                    if (err) return err; /* return any errors */
                }
                else     /* generic set of variable */
                {
                    err = snmpx_snmp_setVariable(var_val, var_val_type, pObject->pVarValue, pObject->VarLength);
                    SNMPX_LOGOID(snmpx_sidbuf, var_name, var_name_len, err,
                                 var_val, var_val_type);
                }
                if (err)
                {
                    if (err > SNMPX_SNMP_ERR_GENERR)
                        return SNMPX_SNMP_ERR_BADVALUE;
                    else
                        return err; /* return any errors */
                }
            }
            SNMPX_MIB_CTR(snmpInTotalSetVars);
        }
        else     /* not a set, just keep mib counter for total requested vars */
        {
            SNMPX_MIB_CTR(snmpInTotalReqVars);
        }

        /* retrieve the value of the variable and place it into the outgoing packet */
        out_data = snmpx_snmp_build_var_op(out_data, pObject->pOid, pObject->OidLen, pObject->VarType,
                                           pObject->VarLength, pObject->pVarValue, &out_length);

        if (out_data == LSA_NULL)
            return SNMPX_SNMP_ERR_TOOBIG;

        (*index)++;
    }

    /* finally build the outgoing packet */
    ret_val = snmpx_snmp_build_var_op_list(headerP, var_list_start, out_data, out_length, index);

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_parse_var_op_list() - 0x%X", ret_val);
    return ret_val;
}

#endif /* #ifdef SNMPX_CFG_SNMP_AGENT */

#endif  /* ifndef INCLUDE_SNMPV2C */


/* FUNCTION: snmp_create_identical()
 *
 * Create a packet identical to the input packet, except for the
 * error status and the error index which are set according to the
 * input variables. This function is called for the following cases
 * only. 1. When a GET RESPONSE is to be sent for a SET REQUEST 2.
 * When a received GET REQUEST has an error and a GET RESPONSE is to
 * be sent with the error codes. Some implementations use extraneous
 * bytes in length encoding. For example a length of 4 will be
 * encoded as 82 00 04 instead of 04. So if we use our asn_build_*()
 * routines, then our packet will be smaller than the received
 * packet. The other party might not like it. Hence if
 * SNMP_ERR_PDUHDR_ENC is enabled, then we will use our encoding
 * routine for PDUHDR. Otherwise we will be use the encoding that
 * comes with the received packet. Returns 0 if it fails, else
 * returns size of new packet in snmp_out.
 *
 * PARAM1: U_CHAR * snmp_in
 * PARAM2: U_CHAR * snmp_out
 * PARAM3: unsigned snmp_length
 * PARAM4: long     errstat
 * PARAM5: long     errindex
 *
 * RETURNS:
 * Returns 0 if it fails,
 * else returns size of new packet in snmp_out.
 */

int snmpx_snmp_create_identical(
    U_CHAR * snmp_in,     /* SNMP packet to duplicate */
    U_CHAR * snmp_out,   /* OUT - buffer to build duplicate in */
    unsigned snmp_length,   /* length of snmp packet */
    long     errstat,
    long     errindex)
{
    U_CHAR   type;
    U_LONG   dummy;
    unsigned length;
#ifdef SNMP_ERR_PDUHDR_ENC
    unsigned headerLength;
#endif
    U_CHAR * headerPtr, *   reqidPtr, * errstatPtr, *  errindexPtr, * varListPtr;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_create_identical(snmp_in: 0x%X, snmp_out: 0x%X, snmp_length: 0x%X, errstat: 0x%X, errindex: 0x%X)",
                            snmp_in, snmp_out, snmp_length, errstat, errindex);

    SNMPX_ASSERT_NULL_PTR(snmp_out);

    SNMPX_MEMCPY((char *)snmp_out, (char *)snmp_in, snmp_length);
    length = snmp_length;
    headerPtr = snmpx_snmp_auth_parse(snmp_out, &length, snmpx_sidbuf, &snmpx_sidlen, (long *)&dummy);
    if (headerPtr == LSA_NULL)
        return 0;
    reqidPtr = snmpx_asn_parse_header(headerPtr, &length, (U_CHAR *)&dummy);
    if (reqidPtr == LSA_NULL)
        return 0;
#ifdef SNMP_ERR_PDUHDR_ENC
    headerLength = length;
#endif
    errstatPtr = snmpx_asn_parse_int(reqidPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* request id */
    if (errstatPtr == LSA_NULL)
        return 0;
    errindexPtr = snmpx_asn_parse_int(errstatPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error status */
    if (errindexPtr == LSA_NULL)
        return 0;
    varListPtr = snmpx_asn_parse_int(errindexPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error index */
    if (varListPtr == LSA_NULL)
        return 0;

#ifdef INCLUDE_SNMPV2C
    /* If errstat is ASN_EOM then we need to encode the EndOfMib situation.
     * That is done by
     * 1. setting errorStatus and errorIndex as 0
     * 2. setting the corresponding varbind's value to ASN_EOM
     *
     * Step 2 was already done in v2agent.c.
     */
    if (errstat == SNMPX_SNMP_ASN_EOM)
    {
        errstat = 0;
        errindex = 0;
    }
#endif


#ifdef SNMP_MINIMAL_LEN_ENC
    {
        u_char * data;
        data = asn_build_header(headerPtr, &headerLength, GET_RSP_MSG, headerLength);
        if (data != reqidPtr)
            return 0;

        length = snmp_length;
        type = (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
        data = asn_build_int(errstatPtr, &length, type, &errstat, sizeof(errstat));
        if (data != errindexPtr)
            return 0;

        data = asn_build_int(errindexPtr, &length, type, &errindex, sizeof(errindex));
        if (data != varListPtr)
            return 0;
    }
#else /* SNMP_MINIMAL_LEN_ENC */
    /* each of the values to be changed is a "byte". Hence just
     * change it directly.
     */
    *headerPtr = GET_RSP_MSG;  /* Just change the message-type */
    *(errindexPtr-1) = (U_CHAR)errstat;
    *(varListPtr-1) = (U_CHAR)errindex;
#endif   /* SNMP_MINIMAL_LEN_ENC */
    snmpx_packet_end = snmp_out + snmp_length;
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_create_identical(RetLength: 0x%X)", snmp_length);
    return (int)snmp_length;
}

int snmpx_snmp_create_identical_no_var_bind(
    U_CHAR * snmp_in    , /* SNMP packet to duplicate           */
    U_CHAR * snmp_out   , /* OUT - buffer to build duplicate in */
    unsigned snmp_length, /* length of snmp packet              */
    long     errstat    ,
    long     errindex)
{
    U_CHAR     type        ;
    U_LONG     dummy       ;
    U_LONG     version     ; /* extract SNMP Ver from original packet */
    unsigned   length      ;
    U_LONG     reqid       ;
    unsigned   tx_length   ;
    unsigned   headerLength;
    U_CHAR   * headerPtr   ;
    U_CHAR   * reqidPtr    ;
    U_CHAR   * errstatPtr  ;
    U_CHAR   * errindexPtr ;
    U_CHAR   * varListPtr  ;
    U_CHAR   * endPtr      ;

    SNMPX_ASSERT_NULL_PTR(snmp_out);

    /* copy rec packet to out buffer ( from 30 82 <lenHigh> <LenLow>....) */
    SNMPX_MEMCPY((char *)snmp_out, (char *)snmp_in, snmp_length);
    length = snmp_length;
    /* -- Process received packet -- */
    /* extract from RX packet community string, its length, and SNMP version. return ptr to 1st byte after community string */
    headerPtr = snmpx_snmp_auth_parse(snmp_out, &length, snmpx_sidbuf, &snmpx_sidlen, (long *)&version);
    if (headerPtr == LSA_NULL)
        return 0;
    reqidPtr = snmpx_asn_parse_header(headerPtr, &length, (U_CHAR *)&dummy);  /* GET */
    if (reqidPtr == LSA_NULL)
        return 0;
    headerLength = length;
    errstatPtr = snmpx_asn_parse_int(reqidPtr   , &length, &type, (long*)&reqid, sizeof(reqid) );   /* request id */
    if (errstatPtr == LSA_NULL)
        return 0;
    errindexPtr = snmpx_asn_parse_int(errstatPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error status */
    if (errindexPtr == LSA_NULL)
        return 0;
    varListPtr = snmpx_asn_parse_int(errindexPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error index */
    if (varListPtr == LSA_NULL)
        return 0;
    headerLength = headerLength - length ; /* calc size of ASN1 RedId + ErrStat + ErrIndex */
    /* ------- Start to build reply packet based on received processed packet -------- */
    tx_length = snmp_length ; /* max theoretical pkt size memory space we have ( we don't need so much) */
    /* Header being build will place requested size+authentication size after 0x30 in the start of SNMP packet
       Add 1st 2 to size in order to take into account GET_RESPONSE ASN.1 obj
       Add 2nd 2 to size in order to take into account 2 byte end of SNMP packet  */
    headerPtr = snmpx_snmp_auth_build( snmp_out, &tx_length, snmpx_sidbuf, &snmpx_sidlen, (long *)&version, headerLength + 2 + 2 );
    if (headerPtr == LSA_NULL)
        return 0;
    /* Place GET_RESPONSE and the size of ASN.1 obj which follow it (RedId + ErrStat + ErrIndex)
       Add 2 to size in order to take into account 2 byte end of SNMP packet  */
    reqidPtr  = snmpx_asn_build_header(headerPtr, &tx_length, GET_RSP_MSG, headerLength + 2 );
    if (reqidPtr == LSA_NULL)
        return 0;
    type = (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    errstatPtr = snmpx_asn_build_int(reqidPtr, &tx_length, type, (long *)&reqid, sizeof(reqid));
    if (errstatPtr == LSA_NULL)
        return 0;
    errindexPtr  = snmpx_asn_build_int(errstatPtr, &tx_length, type, &errstat, sizeof(errstat));
    if (errstatPtr == LSA_NULL)
        return 0;
    varListPtr = snmpx_asn_build_int(errindexPtr, &tx_length, type, &errindex, sizeof(errindex));
    if (errindexPtr == LSA_NULL)
        return 0;
    endPtr = snmpx_asn_build_null(varListPtr, &tx_length , ASN_CONSTRUCTOR | ASN_SEQUENCE);
    if (endPtr == LSA_NULL)
        return 0;
    tx_length = snmp_length - tx_length  ; /* calc real SNMP size (UDP data) */
    snmpx_packet_end = snmp_out + tx_length; /* calc end memory address of pkt to be send */
    return (int)tx_length;
}

/* FUNCTION: snmp_access()
 *
 * snmp_access() - Check for a legal combination of variable access,
 * community priviledge, and operation. This gets called after
 * community has been looked up via SNMP_COMMUNITY_CHECK(), so we know the
 * community at least exists in our table... Returns 1 if this access
 * is OK, else returns 0
 *
 * PARAM1: U_SHORT    acl
 * PARAM2: unsigned   community
 * PARAM3: unsigned   rw
 *
 * RETURNS:
 * Returns 1 if this access is OK, else returns 0  */
 #ifdef SNMPX_CFG_SNMP_AGENT

static int snmpx_snmp_access(
    U_SHORT acl,   /* variable access, RONLY or RWRITE */
    unsigned    community,  /* community access, RONLY or RWRITE */
    unsigned    rw)         /* operation; READ or WRITE */
{
    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_access(acl: 0x%X, community: 0x%X, rw: 0x%X)",
                            acl, community, rw);
    /* If we got here, community was in our list, so... */
    if (rw == SNMPX_READ)
        return 1;   /* read operations are always always OK */

    /* fall to here if operation is write... */
    if ((acl == SNMPX_READ_WRITE)||(acl == SNMPX_READ_CREATE)||(acl == SNMPX_WRITE_ONLY))
    {
        /* variable is writable */
        if (community == SNMPX_READ_WRITE)   /* both var * comm are write OK */
            return 1;   /* write OK for this community */
    }

    /* else either var or community is not OK for write */
    SNMPX_MIB_CTR(snmpInBadCommunityUses);
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_access() - either var or community is not OK for write");
    return 0;
}
#endif


/* FUNCTION: snmp_get_community()
 *
 * PARAM1: U_CHAR * rcvdcomm - received community name
 *
 * SNMPv1/SNMPv2c uses snmp_get_community() to verify the received
 * community name.
 *
 * If SNMPv3 is used, then SNMPv1/SNMPv2c calls v3_community_check().
 * If SNMPv3 is not used, then SNMPv1/SNMPv2c calls its own snmp_get_community().
 *
 * RETURNS:
 * If the rcvdcomm is valid, return access permission
 * Else return -1.
 */
 #ifdef SNMPX_CFG_SNMP_AGENT

static int snmpx_snmp_get_community(U_CHAR * rcvdcomm, unsigned sidlen) /* snmp_get_community(U_CHAR * rcvdcomm)  */
{
    int   count;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_get_community(rcvdcomm: 0x%X, sidlen: 0x%X)",
                            rcvdcomm, sidlen);
    for (count = 0; count < NUM_COMMUNITIES; count++)
    {
        /* if (!strcmp(snmpx_communities[count].name, (char*)rcvdcomm)) */
        if (SNMPX_MEMCMP(snmpx_communities[count].name, (char*)rcvdcomm, sidlen))
            break;
    }
    if (count == NUM_COMMUNITIES)
        return(-1);
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_get_community(access: 0x%X)", snmpx_communities[count].access);
    return(snmpx_communities[count].access);
}
#endif

/*
 * Authentication for SNMP (RFC 1067).  This implements a null
 * authentication layer.
 */

/* FUNCTION: snmp_auth_parse()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * length
 * PARAM3: U_CHAR * sid
 * PARAM4: unsigned * slen
 * PARAM5: long *    version
 *
 * RETURNS:
 */

U_CHAR * snmpx_snmp_auth_parse(
    U_CHAR * data,
    unsigned *  length,
    U_CHAR * sid,
    unsigned *  slen,
    long *      version)
{
    U_CHAR   type = 0;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_auth_parse(data: 0x%X, length: 0x%X, sid: 0x%X, slen: 0x%X, version: 0x%X)",
                            data, length, sid, slen, version);
    data = snmpx_asn_parse_header(data, length, &type);

    SNMPX_ASSERT_NULL_PTR(slen);
    SNMPX_ASSERT_NULL_PTR(sid);

    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_parse(): bad header");
        return LSA_NULL;
    }
    if (type != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_parse(): wrong auth header type");
        return LSA_NULL;
    }
    data = snmpx_asn_parse_int(data, length, &type, version, sizeof(*version));
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_parse(): bad parse of version");
        return LSA_NULL;
    }
    if (type != ASN_INTEGER)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_parse(): bad data type of version");
        return LSA_NULL;
    }

    data = snmpx_asn_parse_string(data, length, &type, sid, slen);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_parse(): bad parse of community");
        return LSA_NULL;
    }
    sid[*slen] = '\0';
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_auth_parse(*data: 0x%X)", *data);
    return (U_CHAR *)data;
}



/* FUNCTION: snmp_auth_build()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * length
 * PARAM3: U_CHAR *   sid
 * PARAM4: unsigned * slen
 * PARAM5: long *     version
 * PARAM6: unsigned   messagelen
 *
 * RETURNS:
 */

U_CHAR * snmpx_snmp_auth_build(U_CHAR * data,
    unsigned *  length,
    U_CHAR *    sid,
    unsigned *  slen,
    long *      version,
    unsigned    messagelen)
{
    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_auth_build(data: 0x%X, length: 0x%X, sid: 0x%X, slen: 0x%X, version: 0x%X)",
                            data, length, sid, slen, version);

    SNMPX_ASSERT_NULL_PTR(slen);

    data = snmpx_asn_build_header(data, length, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), messagelen + *slen + 5);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_build(): buildheader");
        return LSA_NULL;
    }
    data = snmpx_asn_build_int(data, length,
                               (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                               (long *)version, sizeof(*version));
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_build(): buildint");
        return LSA_NULL;
    }
    data = snmpx_asn_build_string(data, length,
                                  (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR),
                                  sid, *slen);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_auth_build(): buildstring");
        return LSA_NULL;
    }
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_auth_build(*data: 0x%X)", *data);
    return (U_CHAR *)data;
}

#define  SNMPX_OID_BUF_SIZE   128   /* max length of display line */
static char snmpx_oidbuf[SNMPX_OID_BUF_SIZE];   /* display buffer */

/* FUNCTION: snmpx_put_oid()
 *
 * put the ascii decimal digits representing the value
 * of the oid into a buffer.
 *
 * PARAM1: char* buf
 * PARAM2: oid oidval
 *
 * RETURNS: A pointer to the next buffer space available after the oid.
 */

static char * snmpx_put_oid(char* buf, SNMPX_OID_TYPE oidval)
{

    SNMPX_ASSERT_NULL_PTR(buf);

	snmpx_ultoa(oidval, (SNMPX_COMMON_MEM_U8_PTR_TYPE)buf, 10, 0);    /* format decimal digits into buf */

    while (*buf != 0) buf++;            /* find end of digit string */
    return(buf);                     /* return pointer to trailing null */
}


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
    unsigned * buf_len)  /* number of bytes written */
{
    char *   cp;

    SNMPX_ASSERT_NULL_PTR(oidp);

    cp = snmpx_oidbuf;

    /* pretty up "2B"s at the front of list */
    if (*oidp == 0x2b)
    {
        SNMPX_MEMCPY(cp, (SNMPX_COMMON_MEM_PTR_TYPE)"1.3.", 4);
        cp += 4;
        oidp++;
        oid_len--;
    }

    while (oid_len--)
    {
        cp = snmpx_put_oid(cp, *oidp++);  /* format number into buffer */
        if (cp > &snmpx_oidbuf[SNMPX_OID_BUF_SIZE - 6])  /* don't overwrite past buffer */
            break;
        if (oid_len) *cp++ = '.'; /* add dots between digits */
    }
    *cp = 0;   /* NULL terminate string */
    if (buf_len) *buf_len = (unsigned)(cp - snmpx_oidbuf);
    return(snmpx_oidbuf);
}


/* FUNCTION: snmpx_print_value()
 * Print the value into a buffer and return the buffer.
 *
 * Buffer is static to this function.
 *
 * PARAM1: u_char * var_val          - ASN encoded value
 * PARAM1: u_char  var_val_type      - type of value
 *
 * RETURNS:
 */

char * snmpx_print_value(
    U_CHAR * var_val, /* ASN.1 encoded value of variable to set */
    U_CHAR var_val_type)     /* ASN.1 type of var_val */
{
    /* set an abritrarily large buffer parm for asn1 parsing.
     * We don't use the returned value for this anyway.
     */
    unsigned   asn_buf_size = 1000;     /*  */
    unsigned   set_var_size = SNMPX_MAX_OID_LEN;  /* bytes in field to set */
    long tmpvalue=0;

    static char datastr[SNMPX_MAX_OID_LEN];
    char datatmp[SNMPX_MAX_OID_LEN];
    SNMPX_OID_TYPE dataoid[SNMPX_MAX_OID_LEN];

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_print_value(var_val: 0x%X, var_val_type: 0x%X)",
                            var_val, var_val_type);

    SNMPX_MEMSET(datastr, 0, sizeof(datastr));  /* null terminate */

    if (!var_val)
        return datastr;

    switch (var_val_type)
    {
        case SNMPX_ASN_INTEGER:
        case SNMPX_ASN_COUNTER:
        case SNMPX_ASN_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case SNMPX_ASN_TIMETICKS:
            if (!snmpx_asn_parse_int(var_val, &asn_buf_size, &var_val_type,
                                     &tmpvalue, sizeof(long)))
            {
                break;
            }
			snmpx_ultoa((LSA_UINT32)tmpvalue, (SNMPX_COMMON_MEM_U8_PTR_TYPE)datastr, 10, 0);
            break;

        case SNMPX_ASN_BITS:
            if (!snmpx_asn_parse_bits(var_val, &asn_buf_size, &var_val_type,
                                      (unsigned char *)datatmp, &set_var_size))
            {
                break;
            }
            SNMPX_MEMCPY(datastr,datatmp,set_var_size);
            break;

        case SNMPX_ASN_STRING:
        case SNMPX_ASN_IPADDRESS:
        case SNMPX_ASN_OPAQUE:
            if (!snmpx_asn_parse_string(var_val, &asn_buf_size, &var_val_type,
                                        (unsigned char *)datatmp, &set_var_size))
            {
                break;
            }
            SNMPX_MEMCPY(datastr,datatmp,set_var_size);
            break;

        case SNMPX_ASN_OBJID:
            if (!snmpx_asn_parse_objid(var_val, &asn_buf_size, &var_val_type,
                                       dataoid, &set_var_size))
            {
                break;
            }
            SNMPX_MEMCPY(datastr, snmpx_print_oid(dataoid, set_var_size, &set_var_size), set_var_size);
            break;
        default:
		    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_ERROR, "OUT: snmpx_print_value(unsupported value type: 0x%X)", var_val_type);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_print_value(datastr: 0x%X)", datastr);
    return datastr;
}

/*****************************************************************************/
/*  end of file SNMPX_AGNT.C                                                 */
/*****************************************************************************/
