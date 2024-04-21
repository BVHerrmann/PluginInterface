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
/*  F i l e               &F: snmpx_mngr.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX-system functions                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  32
#define SNMPX_MODULE_ID      LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_MNGR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles */
#include "snmpx_int.h"            /* internal header   */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

#ifdef SNMPX_CFG_SNMP_MANAGER

#include "snmpx_asn1.h"
#include "snmpx_agnt.h"           /* SNMPX-Agent header  */

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

static U_CHAR   mngr_sidbuf[SNMPX_MAX_COMMUNITY_SIZE  +  1];   /* Community string buffer */
static unsigned mngr_sidlen;

extern U_CHAR * snmpx_packet_end; /* end of built packet data */

/*===========================================================================*/
/*                           (external) functions                            */
/*===========================================================================*/

/* FUNCTION: snmpx_get_value()
 * Print the value into a buffer and return the buffer.
 *
 * Buffer is static to this function.
 *
 * PARAM1: u_char * var_val          - ASN encoded value
 * PARAM1: u_char  var_val_type      - type of value
 *
 * RETURNS:
 */

static int snmpx_get_value(
    U_CHAR * var_val,		/* IN  */ /* ASN.1 encoded value of variable */
    U_CHAR var_val_type, 	/* IN  */ /* ASN.1 type of var_val */
	U_CHAR * out_data,		/* OUT */ /* value of variable     */
    unsigned *out_length)
{
    /* set an abritrarily large buffer parm for asn1 parsing.
     * We don't use the returned value for this anyway.
     */
    unsigned   asn_buf_size = 1000;     /*  */
	unsigned   set_var_size;
    long tmpvalue=0;
	int ret_val = SNMPX_SNMP_ERR_NOERROR;

    char datatmp[SNMPX_SNMP_MAX_LEN];
    SNMPX_OID_TYPE dataoid[SNMPX_MAX_OID_LEN];

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_get_value(out_data: 0x%X, out_length: 0x%X, var_val: 0x%X, var_val_type: 0x%X)",
                            out_data, out_length, var_val, var_val_type);

    if (!out_data)
        return SNMPX_SNMP_ERR_GENERR;

    if (!var_val)
        return SNMPX_SNMP_ERR_GENERR;

    switch (var_val_type)
    {
        case SNMPX_ASN_INTEGER:
        case SNMPX_ASN_COUNTER:
        case SNMPX_ASN_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case SNMPX_ASN_TIMETICKS:
            set_var_size = sizeof(long);

			/* GET-Response (AP00966895): Allow the coding of a value sequence up to 64 bit. */
            if (snmpx_asn_parse_int(var_val, &asn_buf_size, &var_val_type,
                                     &tmpvalue, sizeof(long)))
			{
	            SNMPX_MEMCPY(out_data, &tmpvalue, sizeof(long));
			}
			else
			{
				SNMPX_COUNTER64 tmpcounter;

				tmpcounter.ll.l = 0L;
				tmpcounter.ll.h = 0L;

				if (snmpx_asn_parse_counter64(var_val, &asn_buf_size, &var_val_type,
											   &tmpcounter, sizeof(tmpcounter)))
				{
					if (tmpcounter.ll.h != 0)	/* higher 32bit value <> 0 ? */
					{
						SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"snmpx_get_value(): Received ASN-Type (0x%X) too big",var_val_type);
						ret_val = SNMPX_SNMP_ERR_BADVALUE;
					}
					else
					{
						SNMPX_MEMCPY(out_data, &tmpcounter.ll.l, sizeof(long));
					}
				}
				else
				{
					ret_val = SNMPX_SNMP_ERR_BADVALUE;
				}
			}
            break;

        case SNMPX_ASN_BITS:
			set_var_size = SNMPX_SNMP_MAX_LEN;  /* bytes in field to set */
            if (!snmpx_asn_parse_bits(var_val, &asn_buf_size, &var_val_type,
                                      (unsigned char *)datatmp, &set_var_size))
            {
				ret_val = SNMPX_SNMP_ERR_BADVALUE;
                break;
            }
            SNMPX_MEMCPY(out_data,datatmp,set_var_size);
            break;

        case SNMPX_ASN_STRING:
        case SNMPX_ASN_IPADDRESS:
        case SNMPX_ASN_OPAQUE:
			set_var_size = SNMPX_SNMP_MAX_LEN;  /* bytes in field to set */
            if (!snmpx_asn_parse_string(var_val, &asn_buf_size, &var_val_type,
                                        (unsigned char *)datatmp, &set_var_size))
            {
				ret_val = SNMPX_SNMP_ERR_BADVALUE;
                break;
            }
            SNMPX_MEMCPY(out_data,datatmp,set_var_size);
            break;

        case SNMPX_ASN_OBJID:
			set_var_size = SNMPX_MAX_OID_LEN;  /* oids in field to set */
            if (!snmpx_asn_parse_objid(var_val, &asn_buf_size, &var_val_type,
                                       dataoid, &set_var_size))
            {
				ret_val = SNMPX_SNMP_ERR_BADVALUE;
                break;
            }
			SNMPX_OIDCPY(out_data, dataoid, set_var_size);
            break;
		case SNMPX_ASN_NULLOBJ:
			set_var_size = 0;
		break;
		default:
			set_var_size = 0;
	        SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"snmpx_get_value(): Unknown ASN Type received (0x%X)",var_val_type);
			ret_val = SNMPX_SNMP_ERR_BADVALUE;
			break;
    }

    if (out_length)
	{
		*out_length = set_var_size;
	}

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_get_value(datastr: 0x%X)", ret_val);
    return ret_val;
}

/* FUNCTION: SNMPX_GetCommunityString()
 *
 * SNMPX_GetCommunityString() - retrieves the community (string and length)
 * of the corresponding session
 *
 * PARAM1: SNMPX_HDB_USER_PTR_TYPE pHDB
 * PARAM2: unsigned * length
 *
 * RETURNS: LSA_UINT8 * community
 */

SNMPX_UPPER_COMMUNITY_PTR_TYPE SNMPX_GetCommunityString(SNMPX_UPPER_RQB_PTR_TYPE pRQB,
									 SNMPX_HDB_USER_PTR_TYPE pHDB)
{
	static SNMPX_COMMUNITY_TYPE ReadCommunity  = {6, {'p','u','b','l','i','c','\0','\0',       \
													  '\0','\0','\0','\0','\0','\0','\0','\0', \
													  '\0','\0','\0','\0','\0','\0','\0','\0', \
													  '\0','\0','\0','\0','\0','\0','\0','\0', \
													  '\0'}};
	static SNMPX_COMMUNITY_TYPE WriteCommunity = {7, {'p','r','i','v','a','t','e','\0',        \
													  '\0','\0','\0','\0','\0','\0','\0','\0', \
													  '\0','\0','\0','\0','\0','\0','\0','\0', \
													  '\0','\0','\0','\0','\0','\0','\0','\0', \
													  '\0'}};

    SNMPX_UPPER_COMMUNITY_PTR_TYPE	pCommunity = LSA_NULL;
    LSA_UINT16	SessionIndex;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetCommunityString(pRQB: 0x%X, pHDB: 0x%X",
                            pRQB, pHDB);

	SNMPX_ASSERT_NULL_PTR(pRQB);
	SNMPX_ASSERT_NULL_PTR(pHDB);

	SNMPX_ASSERT(pRQB->args.Object.SessionID);
    SessionIndex = (LSA_UINT16)(pRQB->args.Object.SessionID - 1); /* Index := SessionID - 1 */
	SNMPX_ASSERT(SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS);

	/* the community (read or write) is depending on the message type (Get, Set,..) */
	switch(pRQB->args.Object.MessageType)
	{
        case SNMPX_GET:
        case SNMPX_GETNEXT:
        case SNMPX_GETBULK:
			if (snmpx_is_not_null(pHDB->MngrSession[SessionIndex].pReadCommunity))
			{
				pCommunity = pHDB->MngrSession[SessionIndex].pReadCommunity;
			}
			else
			{
				pCommunity = &ReadCommunity;
			}
            break;
        case SNMPX_SET:
			if (snmpx_is_not_null(pHDB->MngrSession[SessionIndex].pWriteCommunity))
			{
				pCommunity = pHDB->MngrSession[SessionIndex].pWriteCommunity;
			}
			else
			{
				pCommunity = &WriteCommunity;
			}
            break;
        default:
		    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: Unknown message type (0x%X)", pRQB->args.Object.MessageType);
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetCommunityString(pCommunity: 0x%X", pCommunity);
			return(pCommunity);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetCommunityString(pCommunity: 0x%X", pCommunity);
    return(pCommunity);
}


/* FUNCTION: SNMPX_GetVarOpList()
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
 *
 * RETURNS:
 * Returns one of SNMP_ERR codes.
 * If there is no error, it returns SNMP_ERR_NOERROR
 * Some of the other error codes returned are SNMP_ERR_NOSUCHNAME,
 * SNMP_ERR_BADVALUE, and SNMP_ERR_TOOBIG.
 */

static int SNMPX_GetVarOpList(
    /* U_CHAR *     data,  */
    /* unsigned    length, */
	SNMPX_OID_TYPE * var_name,
	unsigned var_name_len,	/* OID length */
	U_CHAR     var_val_type,  /* type of variable (int or octet string) (one byte) */
	unsigned   var_val_len,   /* length of variable     */
	U_CHAR *   var_val,       /* pointer to ASN1 encoded value of variable */
    U_CHAR *    out_data,
    unsigned    out_length,
    U_CHAR      msgtype,
    long *      index)
{
    /* U_CHAR   type; */
    U_CHAR * headerP, *  var_list_start;
    int ret_val;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetVarOpList(var_name: 0x%X, var_name_len: 0x%X, out_data: 0x%X, out_length: 0x%X, msgtype: 0x%X)",
                            var_name, var_name_len, out_data, out_length, msgtype);

    headerP = out_data;
    out_data = snmpx_asn_build_header(out_data, &out_length, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    if (out_data == LSA_NULL)
    {
        /* SNMPX_MIB_CTR(snmpOutTooBigs); */
        SNMPX_SNMPERROR("SNMPX_GetVarOpList(): not enough space in output packet");
        return BUILD_ERROR;
    }

    var_list_start = out_data;
    *index = 1;

    /* while ((int)length > 0) */   /* only one object in the RQB */
    {
        /* U_CHAR   var_val_type, * var_val *, statType; */
        /* unsigned var_val_len;						 */

		/*  SNMPX_UPPER_OBJECT_PTR_TYPE pObject;		 */

        /* check the name, value pair (oid)				 */
        /* data = snmpx_snmp_parse_var_op(data, var_name, &var_name_len, &var_val_type, */
        /*                               &var_val_len, &var_val, &length); */

        if (var_name == LSA_NULL || var_name_len == 0)
        {
            return SNMPX_SNMP_ERR_NOSUCHNAME;
        }
        if (var_val == LSA_NULL || var_val_len == 0)
        {
            return SNMPX_SNMP_ERR_NOSUCHNAME;
        }

        if (msgtype == SET_REQ_MSG)
        {
            /* see if the type and value is consistent with this
                entry's variable. Do range checking */
            if (snmpx_snmp_goodValue(var_val, var_val_type, (int)var_val_len,
                                     var_val_type, (int)var_val_len) /*!= TRUE*/)
            {
                /* SNMPX_LOGOID(snmpx_sidbuf, var_name, var_name_len,			*/
                /*              SNMPX_SNMP_ERR_BADVALUE, var_val, var_val_type);*/
                return SNMPX_SNMP_ERR_BADVALUE;
            }
        }
        else     /* not a set */
        {
			var_val_type = ASN_NULL;
			var_val_len  = 0;
        }

        /* retrieve the value of the variable and place it into the outgoing packet */
        out_data = snmpx_snmp_build_var_op(out_data, var_name, var_name_len,
											var_val_type, var_val_len, var_val,
											&out_length);

        if (out_data == LSA_NULL)
            return SNMPX_SNMP_ERR_TOOBIG;

        (*index)++;
    }

    /* finally build the outgoing packet */
    ret_val = snmpx_snmp_build_var_op_list(headerP, var_list_start, out_data, out_length, index);

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetVarOpList() - 0x%X", ret_val);
    return ret_val;
}

/* FUNCTION: snmpx_mngr_build_snmp()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned length
 * PARAM3: U_CHAR * out_data
 * PARAM4: unsigned out_length
 *
 * snmpx_mngr_build_snmp() - This is the main entry point for outgoing snmp
 * packets directed to the master's UDP connection. As an embedded
 * master we will get these packets by direct upcalls from the UDP
 * layer. We send authentication, community, message type, and build
 * a request packet.
 *
 * RETURNS:
 * Returns 0 if no request packet was left in
 * out_data, else returns size of request packet;
 *
 */

LSA_INT snmpx_mngr_build_snmp (
	SNMPX_OID_TYPE * var_name, /* IN - object id of variable */
	unsigned   var_name_len,  /* IN - size of var_name, in oids */
	LSA_UINT8  var_val_type,  /* IN - type of variable (int or octet string) (one byte) */
	unsigned   var_val_len,   /* IN - length of variable     */
	LSA_UINT8 * var_val,      /* IN - pointer to ASN1 encoded value of variable */
    LSA_UINT8 * out_data,/* IN/OUT buffer for request packet */
    LSA_UINT out_length, /* IN/SCRTACH size of output buffer */
	long     version,	 /* IN snmp version */
	LSA_UINT8 * snmpx_sidbuf, /* IN Community string buffer */
	unsigned snmpx_sidlen,  /* IN Community string length */
	LSA_UINT8 msg_type,  /* IN snmp type (Get, GetNext, Set,...) */
	long     reqid)		 /* IN request id */
{
    U_CHAR   type;
    long     zero  =  0L;
    long     errstat, errindex;
    U_CHAR * out_auth;   /* this also happens to be start of out buffer */
    U_CHAR * out_header, *  out_reqid;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_mngr_build_snmp(var_name: 0x%X, var_name_len: 0x%X, out_data: 0x%X, out_length: 0x%X)",
                            var_name, var_name_len, out_data, out_length);

    /* authenticates community and check length if valid */
    if (snmpx_sidbuf == LSA_NULL || snmpx_sidlen == 0 || snmpx_sidlen > SNMPX_MAX_COMMUNITY_SIZE)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpInASNParseErrs): bad authentication");
        return 0;
    }

    /* only SNMPv1 is supported. */
    if (version != SNMPX_SNMP_VERSION_1)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpInBadVersions): wrong version");
        return 0;
    }

    /* check message type */
    switch (msg_type)
    {
        case GETNEXT_REQ_MSG:
            break;
        case GET_REQ_MSG:
            break;
        case SET_REQ_MSG:
            SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpInSetRequests): Unsupported message type");
            return 0;
        case GETBULK_REQ_MSG:
        default:
            SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpInASNParseErrs): Unsupported message type");
            return 0;   /* check for traps, responses, later */
    }

	errstat = SNMPX_SNMP_ERR_NOERROR;
	errindex = 0;

    /*
     * Now start cobbling together what is known about the output packet.
     * The final lengths are not known now, so they will have to be recomputed
     * later.
     */
    out_auth = out_data;
    out_header = snmpx_snmp_auth_build(out_auth, &out_length, snmpx_sidbuf, &snmpx_sidlen, &version, 0);
    if (out_header == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): snmp_auth_build failed");
        return 0;
    }
	out_reqid = snmpx_asn_build_header(out_header, &out_length, msg_type, 0);
    if (out_reqid == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpOutGenErrs): build opcode failed");
        return 0;
    }

    type = (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    /* return identical request id */
    out_data = snmpx_asn_build_int(out_reqid, &out_length, type, &reqid, sizeof(reqid));
    if (out_data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): build reqid failed");
        return 0;
    }

    /* assume that error status will be zero */
    out_data = snmpx_asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
    if (out_data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): build errstat failed");
        return 0;
    }

    /* assume that error index will be zero */
    out_data = snmpx_asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
    if (out_data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): build errindex failed");
        return 0;
    }

    if (errstat == SNMPX_SNMP_ERR_NOERROR )
    {
		/* see snmpx_snmp_parse_var_op_list() */
        errstat = SNMPX_GetVarOpList(var_name, var_name_len,
									 var_val_type, var_val_len, var_val,
									 out_data, out_length,
									 msg_type, &errindex);
    }

    if (errstat < 0)
	{
        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): build errindex failed, usually AUTH_ERROR");
		return 0;    /* usually AUTH_ERROR */
	}

	{
		int      header_shift,  auth_shift;

		switch ((short)errstat)
		{
			case SNMPX_SNMP_ERR_NOERROR:
				/*
				 * Because of the assumption above that header lengths would be encoded
				 * in one byte, things need to be fixed, now that the actual lengths are known.
				 */
				header_shift = 0;
				out_length = (LSA_UINT)(snmpx_packet_end - out_reqid);
				if (out_length >= 0x80)
				{
					header_shift++;
					if (out_length > 0xFF)
						header_shift++;
				}
				auth_shift = 0;
				out_length = (LSA_UINT)((snmpx_packet_end - out_auth) - 2 + header_shift);
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
				out_length = (LSA_UINT)(snmpx_packet_end - out_reqid);
				out_data = snmpx_asn_build_header(out_data, &out_length, msg_type, out_length);
				if (out_data != out_reqid)
				{
					SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): internal error, header");
					return 0;
				}
				out_data = out_auth;
				out_length = (LSA_UINT)(snmpx_packet_end - out_auth);
				out_data = snmpx_snmp_auth_build(out_data, &out_length, snmpx_sidbuf, &snmpx_sidlen,
												 &version, (LSA_UINT)(snmpx_packet_end - out_header));
				if (out_data != out_header)
				{
					SNMPX_SNMPERROR("snmpx_mngr_build_snmp(): internal error");
					return 0;
				}
				break;

			case SNMPX_SNMP_ERR_NOSUCHNAME:
		       SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpOutNoSuchNames): build opcode failed");
				return 0;
			case SNMPX_SNMP_ERR_TOOBIG:
		       SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpOutTooBigs): build opcode failed");
				return 0;
			case SNMPX_SNMP_ERR_BADVALUE:
	 	        SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpOutBadValues): build opcode failed");
				return 0;
			case SNMPX_SNMP_ERR_READONLY:
		       SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpInBadCommunityUses): build opcode failed");
				return 0;
			case SNMPX_SNMP_ASN_EOM: /* endOfMib */
			case SNMPX_SNMP_ERR_GENERR:
		       SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpOutGenErrs): build opcode failed");
			   return 0;
			default:
		       SNMPX_SNMPERROR("snmpx_mngr_build_snmp(snmpSilentDrops): build opcode failed");
			   return 0;
		}
	}

    /* Assume this will result in an SNMP send & update MIB-II stats */
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_mngr_build_snmp(RetLength: 0x%X)", (snmpx_packet_end - out_auth));
    return(snmpx_packet_end - out_auth);
}


/* FUNCTION: snmpx_mngr_parse_var_op()
 *
 * snmpx_mngr_parse_var_op() - parse next variable from a sequence of
 * received variables. This extracts the object ID and type of
 * variable for the caller. A pointer to the start of the
 *
 * PARAM1: U_CHAR * data
 * PARAM2: oid *       var_name
 * PARAM3: unsigned *    var_name_len
 * PARAM4: U_CHAR * var_val_type
 * PARAM5: unsigned *    var_val_len
 * PARAM6: U_CHAR ** var_val
 * PARAM7: unsigned * listlength
 *
 * RETURNS:
 */

static U_CHAR *
snmpx_mngr_parse_var_op(U_CHAR * data, /* IN - pointer to the start of object */
                        SNMPX_OID_TYPE *       var_name,   /* OUT - object id of variable */
                        unsigned *     var_name_len,  /* IN/OUT - size of var_name, in oids */
                        U_CHAR *    var_val_type,     /* OUT - type of variable (int or octet string) (one byte) */
                        unsigned *     var_val_len,   /* OUT - length of variable */
                        U_CHAR **      var_val,        /* OUT - pointer to ASN1 encoded value of variable */
                        unsigned *     listlength)    /* IN/OUT - number of valid bytes left in var_op_list */
{
    U_CHAR   var_op_type = 0;
    unsigned var_op_len = *listlength;
    U_CHAR * var_op_start   =  data;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_mngr_parse_var_op(var_name: 0x%X, var_name_len: 0x%X)",
                            var_name, var_name_len);
    /* pull off leading SEQUENCE header */
    data = snmpx_asn_parse_header(data, &var_op_len, &var_op_type);
    if (data == LSA_NULL)
        return LSA_NULL;

    if (var_op_type != (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
        return LSA_NULL;
    /* pull off field with variable's object Id: */
    data = snmpx_asn_parse_objid(data, &var_op_len, &var_op_type, var_name, var_name_len);
    if (data == LSA_NULL)
        return LSA_NULL;

    if (var_op_type != (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID))
        return LSA_NULL;

    *var_val = data;     /* save pointer to this object */

    /* find out what type of object this is */
    data = snmpx_asn_parse_header(data, &var_op_len, var_val_type);
    if (data == LSA_NULL)
        return LSA_NULL;

    /*  *var_val = data;  */   /* save pointer to this object */

    *var_val_len = var_op_len;
    data += var_op_len;
    *listlength -= (unsigned)(data - var_op_start);
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_mngr_parse_var_op(data: 0x%X)", data);
    return data;   /* return */
}


/* FUNCTION: snmpx_mngr_parse_var_op_list()
 *
 * Parse_var_op_list goes through the list of variables and retrieves
 * each one, placing it's value in the output packet. If doSet is
 * non-zero, the variable is set with the value in the packet. If any
 * error occurs, an error code is returned.
 *
 * PARAM1: U_CHAR *   data
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

static int snmpx_mngr_parse_var_op_list(
    U_CHAR * data,
    unsigned    length,

	SNMPX_UPPER_OID_PTR_TYPE var_name,/* OUT - object id of variable */
	unsigned  * var_name_len,  /* OUT - size of var_name, in oids */

	LSA_UINT8 * var_val_type,  /* OUT - type of variable (int or octet string) (one byte) */
	unsigned  * var_val_len,   /* OUT - length of variable     */
	SNMPX_COMMON_MEM_U8_PTR_TYPE var_val, /* OUT - pointer to ASN1 encoded value of variable */

    U_CHAR      msgtype,
    long *      index,
    long        errstat,
    int         version)
{
    U_CHAR   type = 0;

    U_CHAR   statType = 0;
    unsigned statLen = 0;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                          "IN : snmpx_mngr_parse_var_op_list(data: 0x%X, length: 0x%X, msgtype: 0x%X, version: 0x%X)",
                          data, length, msgtype, version);

	SNMPX_ASSERT_NULL_PTR(var_name);
	SNMPX_ASSERT_NULL_PTR(var_name_len);
	SNMPX_ASSERT_NULL_PTR(var_val_type);
	SNMPX_ASSERT_NULL_PTR(var_val_len);
	SNMPX_ASSERT_NULL_PTR(var_val);
	SNMPX_ASSERT(version == SNMPX_SNMP_VERSION_1);

    data = snmpx_asn_parse_header(data, &length, &type);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_var_op_list(): not enough space for varlist");
        return PARSE_ERROR;
    }
    if (type != (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_var_op_list(): wrong type");
        return PARSE_ERROR;
    }

    *index = 1;

    while ((int)length > 0)
    {
		U_CHAR * var_val_tmp = LSA_NULL;
        int   err;

        /* parse the name, value pair */
        *var_name_len = SNMPX_MAX_OID_LEN;
        data = snmpx_mngr_parse_var_op(data, var_name, var_name_len, var_val_type,
										var_val_len, &var_val_tmp, &length);

        if (data == LSA_NULL)
		{
            return PARSE_ERROR;
		}
        if (msgtype == SET_REQ_MSG)
        {
            /* see if the type and value is consistent with this
            entry's variable. Do range checking */
            if (snmpx_snmp_goodValue(var_val_tmp, *var_val_type, (int)(*var_val_len),
                                   statType, (int)statLen) /*!= TRUE*/)
            {
                SNMPX_SNMPERROR("snmpx_mngr_parse_var_op_list(): type or value failed (InASNParseErrs)");
                SNMPX_LOGOID(mngr_sidbuf, var_name, *var_name_len,
                             SNMPX_SNMP_ERR_BADVALUE, var_val_tmp, *var_val_type);
                return SNMPX_SNMP_ERR_BADVALUE;
            }

            /* actually do the set if necessary */
			/* generic set of variable */
            if (msgtype == SET_REQ_MSG)
            {
				/* err = snmpx_mngr_setVariable(var_val, var_val_type, statP, statLen); */
				err = SNMPX_SNMP_ERR_GENERR; /* not supported */
				SNMPX_LOGOID(mngr_sidbuf, var_name, *var_name_len, err, var_val_tmp, *var_val_type);
                if (err)
                {
                    if (err > SNMPX_SNMP_ERR_GENERR)
                        return SNMPX_SNMP_ERR_BADVALUE;
                    else
                        return err; /* return any errors */
                }
            }
        }

        /* retrieve the value of the variable and place it into the outgoing buffer */
		err = snmpx_get_value(var_val_tmp, *var_val_type, var_val, var_val_len);
		if (err != SNMPX_SNMP_ERR_NOERROR)
		{
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE, "OUT: snmpx_mngr_parse_var_op_list(err=0x%X) - SNMPX_SNMP_ERR_BADVALUE", err);
			return err;
		}

		if (length > 0)
		{
		    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: snmpx_mngr_parse_var_op_list() - more than one object found!");
			break;
		}

        (*index)++;
    }

    LSA_UNUSED_ARG(errstat);
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_mngr_parse_var_op_list() - SNMPX_SNMP_ERR_NOERROR");
    return SNMPX_SNMP_ERR_NOERROR;
}

/* FUNCTION: snmpx_mngr_parse_snmp()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned length
 * PARAM3: U_CHAR * out_data
 * PARAM4: unsigned out_length
 *
 * snmpx_mngr_parse_snmp() - This is the main entry point for incoming snmp
 * packets directed to the manager's UDP connection. As an embedded
 * manager we will get these packets by direct upcalls from the UDP
 * layer. We check authentication, community, message type, and return
 * the mib object.
 *
 * RETURNS:
 * Returns 0 if no mib object was left in
 * out_data, else returns size of mib object;
 *
 */

static LSA_INT snmpx_mngr_parse_snmp (
    LSA_UINT8 * data,		  /* IN pointer to packet data (past UDP header) */
    LSA_UINT    length,       /* IN/SCRTACH length of packet data */

	LSA_UINT8 * var_val_type,  /* OUT - type of variable (int or octet string) (one byte) */
	unsigned  * var_val_len,   /* OUT - length of variable     */
	SNMPX_COMMON_MEM_U8_PTR_TYPE var_val, /* OUT - pointer to ASN1 encoded value of variable */

	SNMPX_UPPER_OID_PTR_TYPE var_name,/* OUT - object id of variable */
	unsigned  * var_name_len,  /* OUT - size of var_name, in oids */

	LSA_UINT8 * msg_type,     /* OUT snmp type (Get, GetNext, Set,...) */
	long     * reqid,         /* OUT request id */
    LSA_UINT32 * SnmpError)	  /* OUT snmp error */
{
    U_CHAR   type;
    long     outerrstat;
    long     errstat  = 0;
    long     errindex = 0;
    long     version  = 0;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_mngr_parse_snmp(data: 0x%X, length: 0x%X, var_val_type: 0x%X, var_val_len: 0x%X, SnmpError: 0x%X)",
                            data, length, var_val_type, var_val_len, SnmpError);

	SNMPX_ASSERT_NULL_PTR(data);
	SNMPX_ASSERT_NULL_PTR(var_val_type);
	SNMPX_ASSERT_NULL_PTR(var_val_len);
	SNMPX_ASSERT_NULL_PTR(var_val);
	SNMPX_ASSERT_NULL_PTR(var_name);
	SNMPX_ASSERT_NULL_PTR(var_name_len);
	SNMPX_ASSERT_NULL_PTR(msg_type);
	SNMPX_ASSERT_NULL_PTR(reqid);
	SNMPX_ASSERT_NULL_PTR(SnmpError);

    if (length > SOCK_SNMP_BUFFER_SIZE)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): incoming pkt too big");

        /* truncate incoming packet & try to build error reply */
        length = SOCK_SNMP_BUFFER_SIZE;
        outerrstat = SNMPX_SNMP_ERR_TOOBIG;
    }
    else
    {
        outerrstat = SNMPX_SNMP_ERR_NOERROR;
    }

	*var_val_type = 0;
	*var_val_len  = 0;
	*var_name_len = 0;
	*msg_type     = 0;
	*reqid        = 0;
	*SnmpError    = SNMPX_SNMP_ERR_GENERR;

	/* check the SnmpVersion (Snmp-V1) */

    mngr_sidlen = SNMPX_MAX_COMMUNITY_SIZE;
    /* authenticates message and returns length if valid */
    data = snmpx_snmp_auth_parse(data, &length, mngr_sidbuf, &mngr_sidlen, &version);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): bad authentication");
        return 0;
    }
    if (version != SNMPX_SNMP_VERSION_1)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): wrong version");
        return 0;
    }

	/* check the community ? */

    data = snmpx_asn_parse_header(data, &length, msg_type);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): bad header");
        return 0;
    }

    /* check message type */
    switch (*msg_type)
    {
        case GET_RSP_MSG:
            break;
        default:
            SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): Unsupported message type");
        return 0;   /* check for traps, responses, later */
    }

    data = snmpx_asn_parse_int(data, &length, &type, reqid, sizeof(reqid));
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): bad parse of reqid");
        return 0;
    }
    data = snmpx_asn_parse_int(data, &length, &type, (long *)&errstat, sizeof(errstat));
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): bad parse of errstat");
        return 0;
    }

	*SnmpError = (LSA_UINT32)errstat;

    data = snmpx_asn_parse_int(data, &length, &type, (long *)&errindex, sizeof(errindex));
    if (data == LSA_NULL)
    {
		*SnmpError = SNMPX_SNMP_ERR_GENERR;
        SNMPX_SNMPERROR("snmpx_mngr_parse_snmp(): bad parse of errindex");
        return 0;
    }

    if (outerrstat == SNMPX_SNMP_ERR_NOERROR)
    {
		outerrstat = snmpx_mngr_parse_var_op_list(data, length, var_name, var_name_len,
												var_val_type, var_val_len, var_val,
												*msg_type, &errindex, errstat, version);
    }

    if (outerrstat < 0)
	{
		*SnmpError = SNMPX_SNMP_ERR_GENERR;
		return 0;    /* usually AUTH_ERROR */
	}

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_mngr_parse_snmp(length: 0x%X)", length);
    return(LSA_INT)length;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_ParseManager                          +*/
/*+  Input/Output          :    SNMPX_SOCK_LOWER_RQB_PTR_TYPE       pRQB    +*/
/*+                        :    SNMPX_HDB_SOCK_PTR_TYPE             pHDB    +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pRQB                 : Pointer to Receive RQB                          +*/
/*+                                                                         +*/
/*+  Result               : SNMPX_OK                                        +*/
/*+                         SNMPX_ERR_RESOURCE                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: the manager parses an incoming SNMP frame                 +*/
/*+               (SOCK_OPC_UDP_RECEIVE)                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_ParseManager(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_UPPER_RQB_PTR_TYPE       pRQBUser,
	LSA_INT32				       RequestID)
{
	LSA_UINT16					   Response = SNMPX_OK;
    LSA_INT                        Len;
    SNMPX_SOCK_UDP_DATA_PTR_TYPE   udp_recv;

    LSA_UINT8 var_val_buffer[SNMPX_SNMP_MAX_LEN];
	SNMPX_COMMON_MEM_U8_PTR_TYPE var_val = var_val_buffer; /* variable (int or octet string) */
	LSA_UINT8 var_val_type = 0;
	unsigned  var_val_len  = 0;

    SNMPX_OID_TYPE  var_name_buffer[SNMPX_MAX_OID_LEN];
	SNMPX_UPPER_OID_PTR_TYPE var_name = var_name_buffer; /* OID of variable */
	unsigned  var_name_len = 0;

	LSA_UINT8 msg_type = 0xff;     /* snmp type (Get, GetNext, Set,...) */
	long      reqid    = 0;        /* request id */

	LSA_UINT32						SnmpError = SNMPX_SNMP_ERR_NOERROR;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_ParseManager(pRQB: 0x%X,pRQBUser: 0x%X, RequestID: 0x%X)",
                            pRQB,pRQBUser,RequestID);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pRQBUser);

    udp_recv = &pRQB->args.data;

    SNMPX_ASSERT_NULL_PTR(udp_recv);
    SNMPX_ASSERT_NULL_PTR(udp_recv->buffer_ptr); /* received data */

	/* get the mib object */
	/* Main entry point for incoming snmp packets directed to the masters's UDP connection. */
	Len = snmpx_mngr_parse_snmp((LSA_UINT8 *)udp_recv->buffer_ptr, udp_recv->data_length,
								&var_val_type, &var_val_len, var_val,
								var_name, &var_name_len,
								&msg_type, &reqid, &SnmpError);

	/* check the mib object */
	if (Len != 0)
	{
		/* check message type and request id */
		if (!SNMPX_CheckAsnMessageType(msg_type))
		{
			Response = SNMPX_ERR_DATA_INVALID;
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE, "Received MessageType failed (msg_type=0x%X)!", msg_type);
		}
		if (RequestID != (LSA_INT32)reqid)
		{
			Response = SNMPX_ERR_DATA_INVALID;
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE, "RequestID of the session is not the received one (reqid=0x%X)!", reqid);
		}
	}
	else
	{
		Response = SNMPX_ERR_FAILED;
	}

	/* write the mib object (OID and variable) to the user rqb */
	if (Response == SNMPX_OK)
	{
		SNMPX_ASSERT(var_name_len <= SNMPX_MAX_OID_LEN);
		pRQBUser->args.Object.OidLen = var_name_len;
		if (var_name_len != 0)
		{
	        SNMPX_OIDCPY(pRQBUser->args.Object.pOid, var_name, var_name_len);
		}

		if(var_val_len > SNMPX_SNMP_MAX_LEN)
		{
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Received ObjectLen failed (var_val_len=0x%X)!", var_val_len);
			Response = SNMPX_ERR_DATA_INVALID;
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_ParseManager(Response: 0x%X)",Response);
			return(Response);
		}

		if(var_val_len != 0) SNMPX_ASSERT(snmpx_is_not_null(var_val));
		pRQBUser->args.Object.VarType   = var_val_type;
		pRQBUser->args.Object.VarLength = var_val_len;

		if (var_val_type == SNMPX_ASN_OBJID)	/* OID in SNMPX_OID_TYPE */
		{
			SNMPX_OIDCPY(pRQBUser->args.Object.pVarValue, var_val, var_val_len);
		}
		else
		{
	        SNMPX_MEMCPY(pRQBUser->args.Object.pVarValue, var_val, var_val_len);
		}
	}

	pRQBUser->args.Object.SnmpError = SnmpError;
	SNMPX_RQB_SET_RESPONSE(pRQBUser, Response);

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_ParseManager(Response: 0x%X)",Response);
    return(Response);
}

#endif    /* SNMPX_CFG_SNMP_MANAGER */

/*****************************************************************************/
/*  end of file SNMPX_MSTR.C                                                 */
/*****************************************************************************/

