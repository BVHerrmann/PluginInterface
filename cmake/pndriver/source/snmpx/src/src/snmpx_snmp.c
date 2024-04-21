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
/*  F i l e               &F: snmpx_snmp.c                      :F&          */
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

#define LTRC_ACT_MODUL_ID  6
#define SNMPX_MODULE_ID     LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_SNMP */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles  */
#include "snmpx_int.h"            /* internal header    */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

#include "snmpx_asn1.h"
#include "snmpx_agnt.h"           /* SNMPX-Agent header */
#include "snmpx_snmp.h"           /* SNMP header        */
#ifdef SNMPX_CFG_SNMP_AGENT
# include "snmpx_mibs.h"          /* SNMP group of MIB2  */
#endif

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/
/* int snmp_compare(oid* name1, unsigned len1, oid* name2, unsigned len2);   */


/* set parms is used by  SNMP agents to SET value of MIB objects */
struct SetParms   snmpx_set_parms;

#ifdef SNMPX_CFG_SNMP_AGENT

/* FUNCTION: snmpx_snmp_get_oid_element()
 *
 * When a OID is found,
 * that is lexicographicly preceded by the input string, the function
 * for that entry is called to find the method of access of the
 * instance of the named variable. If that variable is not found,
 * NULL is returned. If it is found, the function
 * returns a character pointer and a length.
 *
 * Checks the version field. If the version is SNMPv1, then
 * restrict the use of 64-bit datatypes
 * - return NULL on a GET for 64-bit datatype
 * - skip 64-bit objects when checking for GETNEXT
 *
 * PARAM1: oid * name
 * PARAM2: unsigned * namelen
 * PARAM3: U_CHAR *   stat_type
 * PARAM4: unsigned * stat_len
 * PARAM5: U_SHORT *  access_control
 * PARAM6: int msg_type
 * PARAM7: int version
 *
 * RETURNS:
 */

LSA_UINT8 * snmpx_snmp_get_oid_element(
    SNMPX_UPPER_OID_PTR_TYPE     var_name,/* IN - name of var, OUT - name matched */
    LSA_UINT * var_name_len, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    SNMPX_COMMON_MEM_U8_PTR_TYPE   stat_type,    /* OUT - type of matched variable */
    /* or exception code 0x80XX (ASN_EOM, ASN_NSO, ASN_NSI) if version is SNMP-V2 */
    LSA_UINT *  stat_len,     /* OUT - length of matched variable */
    SNMPX_COMMON_MEM_U16_PTR_TYPE  access_control, /* OUT - access control */
    LSA_UINT8     msg_type, /* IN - message type */
    SNMPX_COMMON_MEM_U32_PTR_TYPE  err_code,/* OUT - error code,if match is not found */
    LSA_INT       version)  /* IN - SNMP version */
{
    LSA_INT   operation = 0;
    LSA_UINT8 * element = 0;

    *err_code = SNMPX_SNMP_ERR_NOERROR;
    *stat_len = 0;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_get_oid_element(var_name: 0x%X, var_name_len: 0x%X)",
                            var_name, var_name_len);
    if (!(var_name && var_name_len && access_control))
    {
        *err_code = SNMPX_SNMP_ERR_BADVALUE;
        SNMPX_SNMPERROR("snmpx_snmp_get_oid_element(): Parameter failed");
    }
    else
    {
        /* check message type, support mib counters: */
        /* set exact based on message type  */
        switch (msg_type)
        {
            case SNMPX_GETNEXT:
            case SNMPX_GETBULK:
                operation = SNMPX_NEXT_OP;
                break;
            case SNMPX_GET:
                operation = SNMPX_GET_OP;
                break;
            case SNMPX_COMMITSET:
                operation = SNMPX_SET_OP;
                break;
            default:
                *err_code = SNMPX_SNMP_ERR_BADVALUE;
                SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "snmpx_snmp_get_oid_element(): Unsupported message type (0x%X)", msg_type);
                break;
        }

        if (*err_code == SNMPX_SNMP_ERR_NOERROR)
        {
            LSA_INT32  temp_err_code = SNMPX_SNMP_ERR_NOERROR;

            /* now attempt to retrieve the variable on the local entity */
            element = snmpx_snmp_getStatPtr(var_name, var_name_len, stat_type, stat_len, access_control,
                                            operation, &temp_err_code, version);
            switch (version)
            {
                case SNMPX_SNMP_VERSION_1:      /* map the err_code */
                    switch (temp_err_code)
                    {
                        case SNMPX_SNMP_ASN_EOM:    /* endOfMib */
                        case SNMPX_SNMP_ASN_NSO:    /* noSuchObject */
                        case SNMPX_SNMP_ASN_NSI:    /* noSuchInstance */
                            *err_code = SNMPX_SNMP_ERR_NOSUCHNAME;
                            break;
                        default:
                            break;
                    }
                    break;
                case SNMPX_SNMP_VERSION_2:      /* map the exception code */
                case SNMPX_SNMP_VERSION_3: /* *err_code = SNMPX_SNMP_ERR_NOERROR */
                    switch (temp_err_code)
                    {
                        case SNMPX_SNMP_ASN_EOM:   /* endOfMib */
                        case SNMPX_SNMP_ASN_NSO:    /* noSuchObject */
                        case SNMPX_SNMP_ASN_NSI:    /* noSuchInstance */
                            *stat_type = (LSA_UINT8)temp_err_code;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_get_oid_element(element: 0x%X, *err_code: 0x%X)", element, *err_code);
    return(element);
}

#endif

/* FUNCTION: SNMPX_CheckAsnMessageType()
 *
 * SNMPX_CheckAsnMessageType() - checks the asn message typ
 *
 * PARAM1: LSA_UINT8 AsnMsgType
 *
 * RETURNS: LSA_BOOL
 */

LSA_BOOL SNMPX_CheckAsnMessageType(LSA_UINT8 AsnMsgType)
{
    LSA_BOOL MsgType = LSA_TRUE;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CheckAsnMessageType(AsnMsgType: 0x%X",
                            AsnMsgType);

    /* check asn message type, set user message type */
    switch (AsnMsgType)
    {
        case GETNEXT_REQ_MSG:
        case GETBULK_REQ_MSG:
        case SET_REQ_MSG:
        case GET_REQ_MSG:
		case GET_RSP_MSG:
            break;
        default:
			MsgType = LSA_FALSE;
		    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: Unknown asn message type (0x%X)", AsnMsgType);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckAsnMessageType(AsnMsgType: 0x%X", AsnMsgType);
    return(MsgType);
}


/* FUNCTION: SNMPX_GetAsnMessageType()
 *
 * SNMPX_GetAsnMessageType() - converts the user message typ
 * to the corresponding asn message type
 *
 * PARAM1: LSA_UINT8 UserMsgType
 *
 * RETURNS: LSA_UINT8 AsnMsgType
 */

LSA_UINT8 SNMPX_GetAsnMessageType(LSA_UINT8 UserMsgType)
{
    LSA_UINT8 AsnMsgType = 0;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetAsnMessageType(AsnMsgType: 0x%X",
                            AsnMsgType);

    /* check asn message type, set user message type */
    switch (UserMsgType)
    {
        case SNMPX_GET:
            AsnMsgType =  GET_REQ_MSG;
            break;
        case SNMPX_GETNEXT:
            AsnMsgType =  GETNEXT_REQ_MSG;
            break;
        case SNMPX_SET:
            AsnMsgType =  SET_REQ_MSG;
            break;
        case SNMPX_GETBULK:
            AsnMsgType = GETBULK_REQ_MSG;
            break;
        default:
		    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: Unknown user message type (0x%X)", UserMsgType);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetAsnMessageType(AsnMsgType: 0x%X", AsnMsgType);
    return(AsnMsgType);
}

/* FUNCTION: SNMPX_GetUserMessageType()
 *
 * SNMPX_GetUserMessageType() - converts the asn message typ
 * to the corresponding user message type
 *
 * PARAM1: LSA_UINT8 AsnMsgType
 *
 * RETURNS: LSA_UINT8 UserMsgType
 */

LSA_UINT8 SNMPX_GetUserMessageType(LSA_UINT8 AsnMsgType)
{
    LSA_UINT8 UserMsgType = 0xff;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetUserMessageType(AsnMsgType: 0x%X",
                            AsnMsgType);

    /* check asn message type, set user message type */
    switch (AsnMsgType)
    {
        case GETNEXT_REQ_MSG:
            UserMsgType = SNMPX_GETNEXT;
            break;
        case GETBULK_REQ_MSG:
            UserMsgType =  SNMPX_GETBULK;
            break;
        case SET_REQ_MSG:
            UserMsgType =  SNMPX_COMMITSET;
            break;
        case GET_REQ_MSG:
            UserMsgType =  SNMPX_GET;
            break;
        default:
		    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: Unknown asn message type (0x%X)", AsnMsgType);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetUserMessageType(UserMsgType: 0x%X", UserMsgType);
    return(UserMsgType);
}


/* FUNCTION: snmp_parse_var_op()
 *
 * snmp_parse_var_op() - parse next variable from a sequence of
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

U_CHAR *
snmpx_snmp_parse_var_op(U_CHAR * data, /* IN - pointer to the start of object */
                        SNMPX_OID_TYPE *       var_name,   /* OUT - object id of variable */
                        unsigned *     var_name_len,  /* IN/OUT - size of var_name, in oids */
                        U_CHAR *    var_val_type,     /* OUT - type of variable (int or octet string) (one byte) */
                        unsigned *     var_val_len,   /* OUT - length of variable */
                        U_CHAR **      var_val,       /* OUT - pointer to ASN1 encoded value of variable */
                        unsigned *     listlength)    /* IN/OUT - number of valid bytes left in var_op_list */
{
    U_CHAR   var_op_type = 0;
    unsigned var_op_len = *listlength;
    U_CHAR * var_op_start   =  data;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_parse_var_op(var_name: 0x%X, var_name_len: 0x%X)",
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

    *var_val_len = var_op_len;
    data += var_op_len;
    *listlength -= (unsigned)(data - var_op_start);
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_parse_var_op(data: 0x%X)", data);
    return data;   /* return */
}



/* FUNCTION: snmp_build_var_op()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: oid *    var_name
 * PARAM3: unsigned var_name_len
 * PARAM4: U_CHAR   var_val_type
 * PARAM5: unsigned var_val_len
 * PARAM6: U_CHAR * var_val
 * PARAM7: unsigned * listlength
 *
 * RETURNS:
 */

U_CHAR * snmpx_snmp_build_var_op(
    U_CHAR * data,    /* IN - pointer to the beginning of the output buffer */
    SNMPX_OID_TYPE *    var_name,      /* IN - object id of variable */
    unsigned var_name_len,  /* IN - length of object id */
    U_CHAR   var_val_type,  /* IN - type of variable */
    unsigned var_val_len,   /* IN - length of variable */
    U_CHAR * var_val,       /* IN - value of variable */
    unsigned *  listlength) /* IN/OUT - number of valid bytes left in output buffer */
{
    unsigned    dummyLen, headerLen;
    U_CHAR  *   dataPtr;
    unsigned    adjustment;    /* number of bytes to move date when re-alaigning */

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_build_var_op(var_name: 0x%X, var_name_len: 0x%X)",
                            var_name, var_name_len);
    dummyLen = *listlength;
    dataPtr = data;
    /* initially build header for maximum size variable ( >255 ) */
    data = snmpx_asn_build_header(data, &dummyLen,
                                  (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 256);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_build_var_op(): asn build var");
        SNMPX_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }
    headerLen = (unsigned)(data - dataPtr);
    *listlength -= headerLen;
    data = snmpx_asn_build_objid(data, listlength,
                                 (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID),
                                 var_name, var_name_len);
    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_build_var_op(): asn build obj");
        SNMPX_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }
    switch (var_val_type)
    {
        case SNMPX_ASN_INTEGER:
        case SNMPX_ASN_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case SNMPX_ASN_COUNTER:
        case SNMPX_ASN_TIMETICKS:
            data = snmpx_asn_build_int(data, listlength, var_val_type,
                                       (long *)var_val, var_val_len);
            break;
        case ASN_BIT_STR:
            data = snmpx_asn_build_bits(data, listlength, var_val_type,
                                        var_val, var_val_len);
            break;
        case ASN_OCTET_STR:
        case SNMPX_ASN_IPADDRESS:
        case SNMPX_ASN_OPAQUE:
            data = snmpx_asn_build_string(data, listlength, var_val_type,
                                          var_val, var_val_len);
            break;
        case ASN_OBJECT_ID:
            data = snmpx_asn_build_objid(data, listlength, var_val_type,
                                         (SNMPX_OID_TYPE *)var_val, var_val_len / sizeof(SNMPX_OID_TYPE));
            break;
        case ASN_NULL:
        case SNMPX_SNMP_ASN_NSO:
        case SNMPX_SNMP_ASN_NSI:
        case SNMPX_SNMP_ASN_EOM:
            data = snmpx_asn_build_null(data, listlength, var_val_type);
            break;

#ifdef SNMP_COUNTER64
        case SNMPX_ASN_COUNTER64:
        case SNMPX_ASN_INTEGER64:
        case SNMPX_ASN_UNSIGNED64:
            data = snmpx_asn_build_counter64(data, listlength, var_val_type,
                                             (SNMPX_COUNTER64 *)var_val, var_val_len);
            break;
#endif

        default:
            SNMPX_SNMPERROR("snmpx_snmp_build_var_op(): bad type");
            SNMPX_MIB_CTR(snmpOutTooBigs);
            return LSA_NULL;
    }

    if (data == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_build_var_op(): data");
        SNMPX_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }

    /* now rebuild the header with the actual size */
    dummyLen = (unsigned)(data - dataPtr) - headerLen;     /* size to put in header */
    if (dummyLen > 255)  /* how many bytes to move data in buffer? */
        adjustment = 0;   /* since we assumed >255, no adjustment needed */
    else if (dummyLen > 127) /* header will shrink 1 byte ? */
        adjustment = 1;   /* byte data left 1 byte */
    else  /* else length encodes in 1 byte, shrink header by 2 bytes */
        adjustment = 2;
    if (adjustment)   /* left shift data right behind header area */
        SNMPX_MEMMOVE(dataPtr, dataPtr+adjustment, dummyLen+ headerLen - adjustment);
    *listlength += adjustment;    /* fixed return value for bytes left in buf */
    data -= adjustment;  /* fix return pointer to next free space */

    /* rebuild header with correct length */
    if (snmpx_asn_build_header(dataPtr, &dummyLen,
                               (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == LSA_NULL)
    {
        SNMPX_SNMPERROR("snmpx_snmp_build_var_op(): bad hdr");
        SNMPX_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_build_var_op(data: 0x%X)", data);
    return data;
}

#ifdef SNMPX_CFG_SNMP_AGENT

struct snmpx_variable *  snmpx_get_variables_arr(void);



/* FUNCTION: snmp_getStatPtr()
 *
 * Each variable name has been placed in the variable table generated
 * by the MIB compiler, without the terminating substring that
 * determines the instance of the variable. When a string is found
 * that is lexicographicly preceded by the input string, the function
 * for that entry is called to find the method of access of the
 * instance of the named variable. If that variable is not found,
 * NULL is returned, and the search through the table continues (it
 * should stop at the next entry). If it is found, the function
 * returns a character pointer and a length or a function pointer.
 * The former is the address of the operand, the latter is an access
 * routine for the variable. If an exact match for the variable name
 * exists, it is returned. If not, (and exact is zero), the next
 * variable lexicographically after the requested one is returned.
 * Set the "p_ranges" member of global struct set_params. That way
 * the SET routines can do validation of input values. If no
 * appropriate variable can be found, NULL is returned.
 *
 * Check the version field. If the version is SNMPv1, then
 * restrict the use of 64-bit datatypes
 * - return NULL on a GET for 64-bit datatype
 * - skip 64-bit objects when checking for GETNEXT
 *
 * PARAM1: oid * name
 * PARAM2: unsigned * namelen
 * PARAM3: U_CHAR *   type
 * PARAM4: unsigned * len
 * PARAM5: U_SHORT *  acl
 * PARAM6: int exact
 * PARAM7: int version
 *
 * RETURNS:
 */

U_CHAR  *  snmpx_snmp_getStatPtr(
    SNMPX_OID_TYPE * name,       /* IN - name of var, OUT - name matched */
    unsigned *  namelen, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    U_CHAR *    type,    /* OUT - type of matched variable */
    unsigned *  len,     /* OUT - length of matched variable */
    U_SHORT *   acl,     /* OUT - access control list */
    int         exact,   /* IN - nonzero if exact match wanted */
    long *      err_code,/* OUT - error code,if match is not found */
    int         version)
{
    struct snmpx_variable * vp;
    int      x;
    U_CHAR * access   =  LSA_NULL;
    int      result;
    int      num_variables=snmpx_snmp_get_num_variables();

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_getStatPtr(name: 0x%X, name_len: 0x%X)",
                            name, namelen);
    /* search variables table for match */
    for (x = 0, vp = snmpx_get_variables_arr(); x < num_variables; vp++, x++)
    {
        /* newer MIB compilers skip the .0 when doing multiple-varbind
         * GET request. Hence doing a smarter compare when
         * namelen < vp->namelen.
         */
        if ( *namelen >= vp->namelen )
            result = snmpx_snmp_compare(name, (int)vp->namelen-1,vp->name,(int)vp->namelen-1);
        else
        {
            /* if it is a GET/SET operation and the SNMP Mgr has skipped ".0"
             * (for a simple, non-table object), then treat it as a valid match */
            if (exact && (*namelen == (unsigned)(vp->namelen-1)) && !(vp->name[vp->namelen-1]))
                result = snmpx_snmp_compare(name, (*namelen), vp->name, (*namelen));
            else
                result = snmpx_snmp_compare(name, (*namelen), vp->name, (int)vp->namelen);
        }

        /* correction: wrong response, if a getnext of a non existing object  */
        /* (not a table object) is made                                       */
        /* snmpegtnext -v1 -c public 140.80.00.03 1.0.8802.1.1.2.1.1.1.4294   */
        /* Response: 1.0.8802.1.1.2.1.1.1.0, but not 1.0.8802.1.1.2.1.1.2.0   */
        /* if ( (result < 0) || (exact && (result == 0)) ||                   */
        /*    ((exact == 0) && (*namelen > vp->namelen) && (result == 0)) ||  */
        /*    ((exact == 0) && (*namelen == vp->namelen) && (result == 0) &&  */
        /*     (name[vp->namelen-1] > 0 && vp->name[vp->namelen-1] == 0xFF)) )*/
        if ((result < 0) || (exact && (result == 0)) ||
            ((exact == 0) && (*namelen >= vp->namelen) && (result == 0) &&
              ((vp->name[vp->namelen-1] > name[vp->namelen-1])||(vp->name[vp->namelen-1]== 0xFF) ) ))
        {
            /* 64-bit datatypes not supported in SNMPv1 */
            if ((version == SNMPX_SNMP_VERSION_1) &&
                ((vp->type == SNMPX_ASN_COUNTER64) || (vp->type == SNMPX_ASN_INTEGER64) || (vp->type == SNMPX_ASN_UNSIGNED64)))
            {
                if (exact) /* GET/SET */
                    return LSA_NULL;
                else /* GETNEXT */
                    continue;
            }
            access = (*(vp->findVar))(vp, name, (int *)namelen, exact, (int *)len);
            if (access != LSA_NULL)
                break;
            if ( exact )
                break;
        }
    }

    if (x == num_variables)
    {
        *err_code = SNMPX_SNMP_ASN_EOM;    /* endOfMib */
        return LSA_NULL;
    }

    if (access == LSA_NULL)
    {
        if (vp->name[vp->namelen-1] == 0)  /* Leaf node, no index */
            *err_code = SNMPX_SNMP_ASN_NSO; /* noSuchObject */
        else
            *err_code = SNMPX_SNMP_ASN_NSI; /* noSuchInstance */
    }
    /* vp now points to the approprate struct */
    *type = vp->type;
    *acl = vp->acl;

#ifdef MIB_VALIDATION
    snmpx_set_parms.p_ranges=&(vp->ranges);
#endif

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_getStatPtr(access: 0x%X, *err_code: 0x%X)", access, *err_code);
    return access;
}

#endif

/* FUNCTION: snmp_compare()
 *
 * PARAM1: oid *  name1
 * PARAM2: unsigned   len1
 * PARAM3: oid *      name2
 * PARAM4: unsigned   len2
 *
 * RETURNS:
 */

int snmpx_snmp_compare(
    SNMPX_OID_TYPE *  name1,
    unsigned    len1,
    SNMPX_OID_TYPE *       name2,
    unsigned    len2)
{
    unsigned len;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_compare(name1: 0x%X, len1: 0x%X, name2: 0x%X, len2: 0x%X)",
                            name1, len1, name2, len2);
    /* len = minimum of len1 and len2 */
    if (len1 < len2)
        len = len1;
    else
        len = len2;
    /* find first non-matching byte */
    while (len-- > 0)
    {
        if (*name1 < *name2)
            return -1;
        if (*name2++ < *name1++)
            return 1;
    }
    /* bytes match up to length of shorter string */
    if (len1 < len2)
        return -1;  /* name1 shorter, so it is "less" */
    if (len2 < len1)
        return 1;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_compare(): both strings are equal");
    return 0;   /* both strings are equal */
}



/* FUNCTION: snmp_compare()
 *
 * PARAM1: oid *  name1
 * PARAM2: unsigned   len1
 * PARAM3: oid *      name2
 * PARAM4: unsigned   len2
 *
 * RETURNS:
 */

int snmpx_oid_compare(
    SNMPX_OID_TYPE *  name1,
    unsigned    len1,
    SNMPX_OID_TYPE *       name2,
    unsigned    len2)
{
    unsigned len;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_oid_compare(name1: 0x%X, len1: 0x%X, name2: 0x%X, len2: 0x%X)",
                            name1, len1, name2, len2);
    /* len = minimum of len1 and len2 */
    if (len1 < len2)
        len = len1;
    else
        len = len2;
    /* find first non-matching byte */
    while (len-- > 0)
    {
        if (*name2++ != *name1++)
            return -2;
    }
    /* bytes match up to length of shorter string */
    if (len1 < len2)
        return -1;  /* name1 shorter, so it is "less" */
    if (len2 < len1)
        return 1;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_oid_compare(): both strings are equal");
    return 0;   /* both strings are equal */
}

/* FUNCTION: snmp_sizeof_int()
 *
 * Return the number of bytes needed to store "value". "value" represents
 * a length. It can be length of PDU, header, pkt , etc.
 *
 * PARAM1: long value
 *
 * RETURNS:
 */

int  snmpx_snmp_sizeof_int(long value)
{
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN/OUT : snmpx_snmp_sizeof_int(value: 0x%X)", value);
    if ( value < 0x80 )
        return 2;
    else if ( value <= 0xFF )
        return 3;
    else  /* if ( value <= 0xFFFF ) */
        return 4;
}

/* FUNCTION: snmp_build_adjust_len()
 *
 * Encode the header for the data. Finally, in the pkt,
 * we would need HEADER (length of data) followed by DATA.
 * When we start building data, we don't know its length.
 * Hence the following sequence happens
 * 1. Build header with dummy length.
 * 2. Build the data and find its length
 * 3. call snmp_build_adjust_len() to put the right length
 * in the header field.
 * The gotchha is that the header field's size can vary
 * depending on the length to encoded. Here is how it goes
 * 80   > Length > 0  :  30 <len>
 * ff   > Length > 80 :  30 81 <len>
 * ffff > length > ff :  30 82 <len> <len>
 * Hence,
 * 1. Check the len of encoding of orig (dummy_len) header.
 * 2. Then check the len of encoding for new header (correct len).
 * 3. If they are different, then move the DATA, so that
 * there is proper space for length encoding.
 * 4. Encode the new length.
 *
 * PARAM1: U_CHAR *start_hdr
 * PARAM2: U_CHAR *start_data
 * PARAM3: unsigned len_data
 * PARAM4: int *diff_hdr
 * PARAM5: unsigned *len_final
 *
 * RETURNS:
 */

/* int */
LSA_BOOL snmpx_snmp_build_adjust_len(
    U_CHAR * start_hdr, U_CHAR * start_data, unsigned len_data,
    int * diff_hdr,   unsigned *  len_final)
{
    int   len_pkt_encoding  =  start_data  -  start_hdr   ;
    U_CHAR   hdr_type       = *   start_hdr   ;           /* Retain the hdr_type */
    unsigned dummy_pktlen;        /* as a parameter to asn_build_header */
    int   diff;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_build_adjust_len(start_hdr: 0x%X, start_data: 0x%X, len_data: 0x%X, len_final: 0x%X)",
                            start_hdr, start_data, len_data, len_final);
    diff = len_pkt_encoding - snmpx_snmp_sizeof_int((long)len_data);

    if ( diff == 0 )
    {
        /* Length field encodings required the same number of bytes.
         * Hence MEMMOVE won't be required
         */
    }
    else
    {
        /* if (diff > 0 ), then To encode the correct length, lesser
         * bytes are needed. if (diff < 0 ), then To encode the correct
         * length, more bytes are needed. In both cases, the following
         * MEMMOVE should do the shifting.
         */
        SNMPX_MEMMOVE(start_data-diff, start_data, len_data );
        start_data -= diff ;       /* Adjust start_data accordingly */
    }

    dummy_pktlen = len_data ;

    start_hdr = snmpx_asn_build_header(start_hdr, &dummy_pktlen, hdr_type, len_data);
    if ( start_hdr == LSA_NULL )
        return LSA_FALSE;

    if ( start_hdr != start_data )
        return LSA_FALSE ;

    if ( diff_hdr != LSA_NULL )       /* Return the diff in encodings */
        *diff_hdr = diff ;

    if ( len_final != LSA_NULL )      /* Return the final len starting from header */
        *len_final = len_data + (unsigned)snmpx_snmp_sizeof_int((long)len_data);

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_build_adjust_len(): LSA_TRUE");
    return LSA_TRUE;
}



#ifdef MIB_VALIDATION
/* FUNCTION : validateValue()
 *
 * Validate a new value with specifications from MIB
 *
 * PARAM1 : value - value to be validated
 * PARAM2 : p_ranges - structure having validation information
 *
 * Returns : LSA_TRUE if the value is good, else LSA_FALSE
 */

/* int */
LSA_BOOL  snmpx_validateValue(long value, SIZE_INFO p_ranges)
{
    unsigned i;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_build_adjust_len(value: 0x%X, p_ranges: 0x%X)", value, p_ranges);
    /* Check if the value is consistent with MIB specs */
    if ( p_ranges->rtype == MCT_VALUES )
    {
        /* The new value has to be one of those specified in MIB */
        for (i=0 ; i < p_ranges->count ; i++)
            if ( value == p_ranges->values[i] )
                break;
        if (i == p_ranges->count )
            return FALSE;
    }
    else  /* p_ranges.rtype == MCT_RANGES */
    {
        /* The new value has to be in one of the  ranges */
        for (i=0 ; i < p_ranges->count ; i++)
            if ((value >= p_ranges->values[i*2]) &&
                (value <= p_ranges->values[i*2+1]))
            {
                break;
            }
        if (i == p_ranges->count )
            return LSA_FALSE;
    }
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_validateValue(LSA_TRUE)");
    return LSA_TRUE;
}
#endif   /* MIB_VALIDATION */


/* FUNCTION: is_us_ascii()
 * Verify whether all characters in the input string are US ASCII or not.
 *
 * The return code is logic is such that reading the code makes sense.
 *
 * PARAM1: char * str    - string to be validated
 * PARAM2: int   len     - len of string
 *
 * RETURNS:
 * If any character is not US ASCII, then return 0
 * Else return 1
 */

static int snmpx_is_us_ascii(const char * str,int len)
{
    int match;
    int i;
    for (i=0; i < len; i++)
    {
        match=0;
        switch (str[i])
        {
            case 0:    /* NUL - null            */
            case 7:    /* BEL - bell            */
            case 8:    /* BS  - back space      */
            case 9:    /* HT  - horizontal tab  */
            case 10:   /* LF  - line feed       */
            case 11:   /* VT  - vertical tab    */
            case 12:   /* FF  - form feed       */
                match=1;
                break;
            case 13:   /* CR  - carriage return */
                /* next char must be NUL OR LF */
                if ((str[i+1] == 0) || (str[i+1] == 10))
                    match=1;
                break;
            default:
                if ((str[i] >= 32) && (str[i] <= 126))
                    match=1;
                break;
        }
        if (!match)
            return 0;  /* input char is non US ASCII */
    }

    return 1;
}


/* FUNCTION: snmp_goodValue()
 *
 * PARAM1: U_CHAR *value
 * PARAM2: U_CHAR inType
 * PARAM3: int      inLen
 * PARAM4: U_CHAR   actualType
 * PARAM5: int      actualLen
 *
 * RETURNS: TRUE if the value is good. Otherwise it returns an error
 * code which informs about the bad value. Error codes being
 * V3_VB_WRONGTYPE, V3_VB_WRONGLENGTH, V3_VB_WRONGVALUE.
 */

int  snmpx_snmp_goodValue(
    U_CHAR * value,
    U_CHAR   inType,
    int      inLen,
    U_CHAR   actualType,
    int      actualLen)
{
    int   type;
    long  int_value = 0;
    unsigned datalen=6;
    U_CHAR   val_type;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_goodValue(value: 0x%X, inType: 0x%X, inLen: 0x%X)", value, inType, inLen);

    /* convert intype to internal representation */
    type = inType == ASN_INTEGER ? ASN_INTEGER : inType == ASN_OCTET_STR ? ASN_OCTET_STR : inType == ASN_OBJECT_ID ? ASN_OBJECT_ID : ASN_NULL;
    if ( !((inType == actualType) || (type == actualType)) )
    {
        return V3_VB_WRONGTYPE;
    }

    switch (actualType)
    {
        case SNMPX_ASN_INTEGER:
        case SNMPX_ASN_COUNTER:
        case SNMPX_ASN_IPADDRESS:
        case SNMPX_ASN_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case SNMPX_ASN_TIMETICKS:
            if (actualLen != 4)  /* check that length is for 32 bit number */
            {
                SNMPX_SNMPERROR("snmpx_snmp_goodValue(): INT size not 4 bytes");
                SNMPX_MIB_CTR(snmpInBadValues);
                return V3_VB_WRONGLENGTH;
            }
            /* get the value from varbind */
            if (LSA_NULL==snmpx_asn_parse_int(value, &datalen, &val_type, &int_value,sizeof(long) ))
                return V3_VB_WRONGLENGTH;
            if (actualType == SNMPX_ASN_IPADDRESS)  /* No range checking required for IP Address */
                break;

            /* Validate with the user specified range of values */
            if (snmpx_set_parms.do_range)
            {
                if (int_value > snmpx_set_parms.hi_range)
                    return V3_VB_WRONGVALUE;
                if (int_value < snmpx_set_parms.lo_range)
                    return V3_VB_WRONGVALUE;
            }
            /* Validation of popular data types (e.g. TimeInterval,
             * StorageType, SecurityLevel) is supported via SF_* flags.
             * In the var_*() function, the appropriate SF_* flag is set.
             * Accordingly, the received value is validated here.
             * For example, for a StorageType object SnmpTargetAddrStorageType,
             * the SF_STORAGETYPE flag is set in var_snmpTargetAddrEntry().
             * Hence the recieved value would be validated here.
             */
            if (snmpx_set_parms.vbflags)
            {
                /* Some user defined datatypes like TimeInterval
                 * accept only positive values. If the SF_UNSIGNEDVALUE flag is
                 * set, then verify that the value is positive.
                 */
                if (snmpx_set_parms.vbflags & SF_UNSIGNEDVALUE)
                    if (int_value < 0)
                        return V3_VB_WRONGVALUE;
                if (snmpx_set_parms.vbflags & SF_STORAGETYPE)
                    if ((int_value < 1) || (int_value > 5))  /* 1..5 */
                        return V3_VB_WRONGVALUE;
                if (snmpx_set_parms.vbflags & SF_SECURITYLEVEL)
                    if ((int_value < 1) || (int_value > 3))  /* 1..3 */
                        return V3_VB_WRONGVALUE;
            }
#ifdef MIB_VALIDATION
            if ( (!snmpx_set_parms.access_method) && (snmpx_set_parms.p_ranges) &&
                 snmpx_set_parms.p_ranges->count)
            {
                if ( snmpx_validateValue(int_value,snmpx_set_parms.p_ranges) )
                    break;
                else
                    return V3_VB_WRONGVALUE;
            }
#endif   /* MIB_VALIDATION */
            break;
        case ASN_BIT_STR:
            break; /* assume all BIT patterns are good values */
        case SNMPX_ASN_STRING:
        case SNMPX_ASN_OBJID:
        case SNMPX_ASN_OPAQUE:
            if (snmpx_set_parms.do_range) /* check user's range */
            {
                if (inLen > snmpx_set_parms.hi_range)
                    return V3_VB_WRONGLENGTH;
                if (inLen < snmpx_set_parms.lo_range)
                    return V3_VB_WRONGLENGTH;
            }
            else  /* Ranges not specified in stub function. Use those from MIB */
            {
                if (snmpx_set_parms.access_method)
                {
                    /* access_method is set. Hence validation will be
                     * done in access_method.
                     * Thus user can provide access_method to override
                     * default range checking.
                     */
                }
                else
                {
#ifdef MIB_VALIDATION
                    if (set_parms.p_ranges && set_parms.p_ranges->count)
                    {
                        if ( validateValue(inLen,set_parms.p_ranges) == TRUE )
                            break;
                        else
                            return V3_VB_WRONGLENGTH;
                    }
                    else
#endif   /* MIB_VALIDATION */
                    {
                        /* Range not specified in MIB. */
                        if (inLen > actualLen)
                            return V3_VB_WRONGLENGTH;
                    }
                }
            }

            /* for a string, validate all chars to be part of US ASCII */
            if ((actualType == ASN_OCTET_STR) && (!(snmpx_set_parms.vbflags & SF_NOASCIICHK)))
            {
                char buf[SNMPX_MAX_OID_LEN*2];
                int bufsize=SNMPX_MAX_OID_LEN*2;
                datalen=SNMPX_MAX_OID_LEN*2;

                /* read the input string into buf */
                if (!snmpx_asn_parse_string(value, &datalen, &val_type,
                                            (U_CHAR *)buf, (unsigned *)&bufsize))
                {
                    return V3_VB_WRONGVALUE;  /* input string might be too big */
                }

                /* validate all chars */
                if (!snmpx_is_us_ascii(buf,bufsize))
                    return V3_VB_WRONGVALUE;  /* input char is non US ASCII */
            }
            break;
        case SNMPX_ASN_COUNTER64:
        case SNMPX_ASN_INTEGER64:
        case SNMPX_ASN_UNSIGNED64:
            /* we don't have to check for the validity of Counter64 value
             * because as per spec it can only be ReadOnly or
             *"accessible-for-notify". Hence this value would never be modified
             * and hence this function would never be called for it */

        default:
            SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_goodValue(): V3_VB_NOERROR");
            return V3_VB_WRONGTYPE; /* bad type, should this be assert later? */
    }

    /* return TRUE; */
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_goodValue(): V3_VB_NOERROR");
    return V3_VB_NOERROR;
}



/* FUNCTION: snmp_setVariable()
 *
 * PARAM1: U_CHAR * var_val
 * PARAM2: U_CHAR   var_val_type
 * PARAM3: U_CHAR * statP
 * PARAM4: int      statLen
 *
 * RETURNS: 0 on success, else error code
 */

int snmpx_snmp_setVariable(
    U_CHAR * var_val, /* ASN.1 encoded value of variable to set */
    U_CHAR   var_val_type,  /* ASN.1 type of var_val */
    U_CHAR * statP,      /* pointer returned by var_ routine */
    int      statLen)    /* var_len set by var_ routine */
{
    /* set an abritrarily large buffer parm for asn1 parsing. We don't
    use the returned value for this anyway. -JB- */
    unsigned   asn_buf_size = 1000;     /*  */
    unsigned   set_var_size = (unsigned)statLen;  /* bytes in field to set */
    long       tmpvalue     = 0;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_setVariable(var_val: 0x%X, var_val_type: 0x%X, statLen: 0x%X)", var_val, var_val_type, statLen);

    SNMPX_ASSERT_NULL_PTR(statP);

    switch (var_val_type)
    {
        case ASN_INTEGER:
        case SNMPX_ASN_COUNTER:
        case SNMPX_ASN_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case SNMPX_ASN_TIMETICKS:
            if (!snmpx_asn_parse_int(var_val, &asn_buf_size, &var_val_type,
                                     &tmpvalue, set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            if ((snmpx_set_parms.vbflags & SF_STORAGETYPE) &&
                ((long)(*statP) != tmpvalue))
            {
                if ((long)(*statP) == SNMPX_ST_PERMANENT)
                    return V3_VB_WRONGVALUE;  /* can't change this value */
                if ((long)(*statP) == SNMPX_ST_READONLY)
                    return V3_VB_WRONGVALUE;  /* can't change this value */
            }
            if (snmpx_set_parms.vbflags & SF_SECURITYLEVEL)
                *(long *)(statP) = SNMPX_SL2F(tmpvalue); /* map rcvd SecurityLevel to flags */
            else
                *(long *)(statP) = tmpvalue; /* assign the read value */
            break;
        case ASN_BIT_STR:
            if (!snmpx_asn_parse_bits(var_val, &asn_buf_size, &var_val_type,
                                      statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            break;
        case ASN_OCTET_STR:
        case SNMPX_ASN_IPADDRESS:
        case SNMPX_ASN_OPAQUE:
            /* allow buffer sizes up limit set in var_ routine */
            if (snmpx_set_parms.do_range) /* if var_ routine set limits */
                set_var_size = (unsigned)snmpx_set_parms.hi_range;
#ifdef MIB_VALIDATION
            else if (snmpx_set_parms.p_ranges && (snmpx_set_parms.p_ranges->count >0))
            {
                if ( snmpx_set_parms.p_ranges->rtype == MCT_VALUES )
                {
                    /* Set the highest value. Assuming last value is the highest */
                    set_var_size = (unsigned)
                                   snmpx_set_parms.p_ranges->values[snmpx_set_parms.p_ranges->count-1];
                }
                else  /* snmpx_set_parms.p_ranges.rtype == MCT_RANGES */
                {
                    /* Set the highest value. Assuming last pair is the highest */
                    set_var_size = (unsigned)
                                   snmpx_set_parms.p_ranges->values[(snmpx_set_parms.p_ranges->count-1)*2+1];
                }
            }
#endif   /* MIB_VALIDATION */
            if (!snmpx_asn_parse_string(var_val, &asn_buf_size, &var_val_type,
                                        statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            /* null terminate the strings */
            if ( var_val_type == ASN_OCTET_STR )
                *(statP + set_var_size) = '\0';
            break;
        case ASN_OBJECT_ID:
            if (!snmpx_asn_parse_objid(var_val, &asn_buf_size, &var_val_type,
                                       (SNMPX_OID_TYPE*)statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            break;

#ifdef SNMP_COUNTER64
        case SNMPX_ASN_COUNTER64:
        case SNMPX_ASN_INTEGER64:
        case SNMPX_ASN_UNSIGNED64:
            (void)snmpx_asn_parse_counter64(var_val, &asn_buf_size, &var_val_type,
                                            (SNMPX_COUNTER64  *)statP, set_var_size);
            break;
#endif
		default:
            SNMPX_SNMPERROR("snmpx_snmp_setVariable(): bad type");
            return V3_VB_WRONGTYPE;
    }
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_setVariable()");
    return V3_VB_NOERROR;
}


/* FUNCTION: snmp_shift_array()
 *
 * PARAM1: U_CHAR * begin
 * PARAM2: int      length
 * PARAM3: int      shift_amount
 *
 * RETURNS:
 */

void snmpx_snmp_shift_array(
    U_CHAR * begin,
    int      length,
    int      shift_amount)
{
    U_CHAR * oldp, *  newp;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_shift_array(begin: 0x%X, length: 0x%X, shift_amount: 0x%X)", begin, length, shift_amount);
    if (shift_amount >= 0)
    {
        oldp = begin + length - 1;
        newp = oldp + shift_amount;

        while (length--)
            *newp-- = *oldp--;
    }
    else
    {
        oldp = begin;
        newp = begin + shift_amount;

        while (length--)
            *newp++ = *oldp++;
    }
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_shift_array()");
}

/* move oid values into a byte array. Assumes all values are < 256! */


/* FUNCTION: oid2bytes()
 *
 * PARAM1: char * bytebuf
 * PARAM2: oid * oidbuf
 * PARAM3: int count
 *
 * RETURNS:
 */

void snmpx_snmp_oid2bytes(char * bytebuf, SNMPX_OID_TYPE * oidbuf, int count)
{
    int   i;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_snmp_oid2bytes(bytebuf: 0x%X, oidbuf: 0x%X, count: 0x%X)", bytebuf, oidbuf, count);
    for (i = 0; i < count; i++)
        *bytebuf++ = *(char *)oidbuf++;
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_snmp_oid2bytes()");
}


/* end of file snmpx_snmp.c */

