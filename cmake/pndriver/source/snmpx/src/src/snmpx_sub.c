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
/*  F i l e               &F: snmpx_sub.c                               :F&  */
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

#define LTRC_ACT_MODUL_ID  17

#define SNMPX_MODULE_ID      LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_SUB */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles */
#include "snmpx_int.h"            /* internal header   */

#if SNMPX_MODULE_ID_SNMPX_SUB != SNMPX_MODULE_ID /* to avoid Lint warning 766 (header file not used) -- note: agent not yet implemented */
#error
#endif
#ifdef SNMPX_CFG_SNMP_AGENT

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

unsigned char * getStatPtr(SNMPX_UPPER_OID_PTR_TYPE name, unsigned* namelen, unsigned char* type, unsigned* len,
                           unsigned short* acl, int exact, long* err_code, int version);

LSA_UINT8 *
mib2_snmp_get_oid_element(
    SNMPX_UPPER_OID_PTR_TYPE     var_name,/* IN - name of var, OUT - name matched */
    LSA_UINT * var_name_len, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    SNMPX_COMMON_MEM_U8_PTR_TYPE   stat_type,    /* OUT - type of matched variable */
    /* or exception code 0x80XX (ASN_EOM, ASN_NSO, ASN_NSI) if version is SNMP-V2 */
    LSA_UINT *  stat_len,     /* OUT - length of matched variable */
    SNMPX_COMMON_MEM_U16_PTR_TYPE  access_control, /* OUT - access control */
    LSA_UINT8     msg_type, /* IN - message type */
    SNMPX_COMMON_MEM_U32_PTR_TYPE  err_code,/* OUT - error code,if match is not found */
    LSA_INT       version)
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
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "Parameter failed");
    }
    else
    {
        /* check message type, support mib counters: */
        /* set exact based on message type  */
        switch (msg_type)
        {
            case SNMPX_GETNEXT:
            case SNMPX_GETBULK:
                operation = 0;
                break;
            case SNMPX_GET:
                operation = 1;
                break;
                /* case SNMPX_SET: */
                /*    operation = -1; */
                /*    break;              */
            default:
                *err_code = SNMPX_SNMP_ERR_BADVALUE;
                SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,"Unsupported message type");
                break;
        }

        if (*err_code == SNMPX_SNMP_ERR_NOERROR)
        {
            LSA_INT32  temp_err_code = SNMPX_SNMP_ERR_NOERROR;

            /* now attempt to retrieve the variable on the local entity */
            element = getStatPtr(var_name, var_name_len, stat_type, stat_len, access_control,
                                 operation,&temp_err_code,version);
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

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestOidProvide                     +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE     pRQB           +*/
/*+                        :    SNMPX_HDB_PTR_TYPE           pHDB           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_PROVIDE_AGENT        +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Object                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide Indication Resources for the MIB obj.  +*/
/*+                                                                         +*/
/*+               Stores the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if one indication       +*/
/*+               pends to be indicated. If so we indicate this, if not     +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_RequestOidProvide(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestOidProvide(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    /* Check if we have a SOCK-Channel */
    pHDBSock = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "No Sock channel: SNMPX_RequestOidProvide()");
        return;
    }

    /* check the params */
    if (pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_AGENT)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not an agent channel: SNMPX_RequestOidProvide()");
        return;
    }
    if (pRQB->args.Object.OidLen < SNMPX_MAX_OID_LEN || pRQB->args.Object.VarLength < SNMPX_SNMP_MAX_LEN ||
        snmpx_is_null(pRQB->args.Object.pOid) || snmpx_is_null(pRQB->args.Object.pVarValue))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestOidProvide()");
        return;
    }

    /* init the object data */
    pRQB->args.Object.TransactionID = 0;
    pRQB->args.Object.MessageType   = 0;
    pRQB->args.Object.VarType       = 0;
    pRQB->args.Object.AccessControl = 0;
    pRQB->args.Object.SnmpError     = 0;
    pRQB->args.Object.SnmpVersion   = 0;

    /* queue the request */
    SNMPX_RQB_PUT_BLOCK_TOP(pHDB->UserIndReq.pBottom,
                            pHDB->UserIndReq.pTop,
                            pRQB);

    pHDB->UserIndReqPending++;

    /* if this is the first Indication Resource, we check if      */
    /* an indications is pending and the master agent is started  */
    if (pHDBSock->AgntPortState == SNMPX_SNMP_STATE_OPEN)
    {
        LSA_UINT16      i;

        /* Provide.Ind - look for a stored Object to indicate */
        for (i = 0; i < SNMPX_CFG_MAX_CHANNELS; i++)
        {
            /* is this SubAgents to indicate ? */
            //          if (snmpx_is_not_null(pHDB->pSubAgentInd[i]))
            //          {
            //              (LSA_VOID)SNMPX_IndicateOid(pHDB->pSubAgentInd[i], pHDB);
            //          }
#ifdef _MSC_VER
#pragma message("Provide.Ind - look for a stored Object to indicate!")
#endif
        }
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestOidProvide()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestOidReceive                     +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE     pRQB           +*/
/*+                        :    SNMPX_HDB_PTR_TYPE           pHDB           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_PROVIDE_AGENT        +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Object                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Response from a subagent (MIB object)                     +*/
/*+                                                                         +*/
/*+               we check if if the master agent is started                +*/
/*+               If so we indicate this, if not we queue the request.      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_RequestOidReceive(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser)
{
#if 1
    LSA_EXTERN LSA_UINT8 * oha_snmp_get_oid_element(
        SNMPX_UPPER_OID_PTR_TYPE var_name,/* IN - name of var, OUT - name matched */
        LSA_UINT * var_name_len, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
        SNMPX_COMMON_MEM_U8_PTR_TYPE   stat_type,    /* OUT - type of matched variable */
        LSA_UINT *  stat_len,     /* OUT - length of matched variable */
        SNMPX_COMMON_MEM_U16_PTR_TYPE  access_control, /* OUT - access control */
        LSA_UINT8 msg_type, /* IN - message type */
        SNMPX_COMMON_MEM_U32_PTR_TYPE  err_code,/* OUT - error code,if match is not found */
        LSA_INT version);
#endif

    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;

    SNMPX_OID_TYPE              LldpOid[] = {SNMPX_SNMP_LLDP_TREE};
    SNMPX_OID_TYPE              SnmpOid[] = {SNMPX_SNMP_MIB2_TREE};

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestOidReceive(pRQB: 0x%X, pHDBUser: 0x%X)",
                            pRQB, pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    /* Check if we have a SOCK-Channel */
    pHDBSock = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDBUser,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "No Sock channel: SNMPX_RequestOidReceive()");
        return;
    }

    /* check the params */
    if (pHDBUser->Params.pDetail->PathType != SNMPX_PATH_TYPE_AGENT)
    {
        SNMPX_UserRequestFinish(pHDBUser,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not an agent channel: SNMPX_RequestOidReceive()");
        return;
    }

    /* THE HACK FOR THE RESPONSE */
    if (SNMPX_OIDCMP(pHDBUser->RegisteredOids[0].pOid, LldpOid, SNMPX_SNMP_LLDP_TREE_LEN))
    {
        SNMPX_UPPER_OBJECT_PTR_TYPE pObject = &pRQB->args.Object;
        SNMPX_UPPER_MEM_U8_PTR_TYPE pStat;

        SNMPX_ASSERT_NULL_PTR(pObject->pVarValue);

        pStat = oha_snmp_get_oid_element(pObject->pOid, &pObject->OidLen,
                                         &pObject->VarType, &pObject->VarLength,
                                         &pObject->AccessControl,
                                         pObject->MessageType,
                                         &pObject->SnmpError,pObject->SnmpVersion);
        SNMPX_ASSERT(pObject->VarLength < SNMPX_SNMP_MAX_LEN);
        if (snmpx_is_not_null(pStat))
        {
            SNMPX_MEMCPY(pObject->pVarValue, pStat, pObject->VarLength);
        }
        //TESTTESTTEST
        //pObject->SnmpError = SNMPX_SNMP_ERR_NOSUCHNAME;
    }
    else if (SNMPX_OIDCMP(pHDBUser->RegisteredOids[0].pOid, SnmpOid, SNMPX_SNMP_MIB2_TREE_LEN)) /* the ip stack */
    {
        SNMPX_UPPER_OBJECT_PTR_TYPE pObject = &pRQB->args.Object;
        SNMPX_UPPER_MEM_U8_PTR_TYPE pStat;

        SNMPX_ASSERT_NULL_PTR(pObject->pVarValue);

        pStat = mib2_snmp_get_oid_element(pObject->pOid, &pObject->OidLen,
                                          &pObject->VarType, &pObject->VarLength,
                                          &pObject->AccessControl,
                                          pObject->MessageType,
                                          &pObject->SnmpError,pObject->SnmpVersion);
        SNMPX_ASSERT(pObject->VarLength < SNMPX_SNMP_MAX_LEN);
        if (snmpx_is_not_null(pStat))
        {
            SNMPX_MEMCPY(pObject->pVarValue, pStat, pObject->VarLength);
        }
    }
    else
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    /* check the params */
    if (pRQB->args.Object.OidLen > SNMPX_MAX_OID_LEN || pRQB->args.Object.VarLength > SNMPX_SNMP_MAX_LEN ||
        snmpx_is_null(pRQB->args.Object.pOid) || snmpx_is_null(pRQB->args.Object.pVarValue))
    {
        SNMPX_UserRequestFinish(pHDBUser,pRQB,SNMPX_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestOidProvide()");
        return;
    }

    if (SNMPX_CheckOidResponse(pHDBUser, pRQB) != SNMPX_OK)
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_USER);
    }

    /* get the mib object */
    if (snmpx_is_null(pHDBUser->pCurrentObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    /* store the mib object */
    SNMPX_CopyObject(pHDBUser->pCurrentObject, &pRQB->args.Object);

    /* reprovide the resource */
    SNMPX_RQB_SET_OPCODE(pRQB, SNMPX_OPC_PROVIDE_AGENT);
    SNMPX_RQB_SET_HANDLE(pRQB, pHDBUser->ThisHandle);

    pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;

    /* queue the request */
    SNMPX_RQB_PUT_BLOCK_TOP(pHDBUser->UserIndReq.pBottom,
                            pHDBUser->UserIndReq.pTop,
                            pRQB);

    pHDBUser->UserIndReqPending++;

    /* we check if the master agent is started  */
    if (pHDBSock->AgntPortState == SNMPX_SNMP_STATE_OPEN)
    {
#ifdef _MSC_VER
#pragma message("Provide.Res - look for a responded Object!")
#endif
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestOidReceive()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_IndicateOidReceive                    +*/
/*+                             SNMPX_HDB_USER_PTR_TYPE      pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pThisInd            : Pointer to TOPO-Indication                       +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a DIAG-Event to one user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_IndicateOidReceive(
    SNMPX_HDB_USER_PTR_TYPE          pHDBUser,
    SNMPX_UPPER_OBJECT_PTR_TYPE      pObject
)
{
    SNMPX_UPPER_RQB_PTR_TYPE  pRQB;

    LSA_UINT16                  RetVal = SNMPX_OK;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_IndicateOidReceive(pHDBUser: 0x%X, pObject: 0x%X)", pHDBUser, pObject);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);

    if (snmpx_is_null(pObject) || snmpx_is_null(pObject->pOid))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }
    if (pObject->OidLen > SNMPX_MAX_OID_LEN)
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    /* set the IN parameters (pOid, OidLen, MessageType, Version) for the request */
    pHDBUser->pCurrentObject = pObject;

    /* get an indication resource */
    SNMPX_RQB_REM_BLOCK_BOTTOM(pHDBUser->UserIndReq.pBottom,
                               pHDBUser->UserIndReq.pTop,
                               pRQB);

    if (snmpx_is_null(pRQB))    /* no (more) resource provided */
    {
        RetVal = SNMPX_RSP_ERR_RESOURCE;
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_IndicateOidReceive(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    pHDBUser->UserIndReqPending--;

    SNMPX_RQB_SET_OPCODE(pRQB, SNMPX_OPC_RECEIVE_AGENT);
    SNMPX_RQB_SET_HANDLE(pRQB, pHDBUser->ThisHandle);

    pRQB->err.lsa_component_id   = LSA_COMP_ID_UNUSED;

    if (SNMPX_CopyObject(&pRQB->args.Object, pObject) != SNMPX_OK)
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    //  SNMPX_RequestOidReceive(pRQB, pHDBUser);
    pHDBUser->pReceiveReq = pRQB;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_IndicateOidReceive(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_GetSubAgent                              +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+  Result                : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : <> NULL            closest match                          +*/
/*+               NULL               no match anyway                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free register entry in a User-HDB       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_HDB_USER_PTR_TYPE SNMPX_GetSubAgent(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,    /* for internal object */
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version     /* SNMPX version */
)
{
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser = SNMPX_Data.HUser;
    LSA_UINT16                  Channel;
    SNMPX_HDB_USER_PTR_TYPE     pUserHandle = LSA_NULL;

    SNMPX_OID_TYPE              lowest[SNMPX_MAX_OID_LEN];        /* "best fit" object Id */
    LSA_UINT16                  lowest_len = 0;
    LSA_UINT16                  MibTree = 0xffff;

    SNMPX_OID_TYPE              SnmpGroupOid[] = {SNMPX_SNMP_GROUP_TREE};

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetSubAgent(pObject: %Xh, pOid: %Xh, OidLen: %Xh, MessageType: %Xh)", pObject, pOid, OidLen, MessageType);

    if (snmpx_is_null(pOid) || OidLen == 0 || snmpx_is_null(pObject))
    {
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetSubAgent(pUserHandle: 0x%X)", pUserHandle);
        return(pUserHandle);
    }

    /* check asn message type, set user message type */
    switch (MessageType)
    {
        case SNMPX_GETNEXT:
        case SNMPX_GETBULK:
            break;
        case SNMPX_TESTSET: /* must be a plain GET or SET */
        case SNMPX_GET:
        case SNMPX_COMMITSET:
        case SNMPX_UNDOSET:
        case SNMPX_CLEANUPSET:
        case SNMPX_TRAP:
            break;
        default:
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    /* scan at register tables for closest match - first the internal oids */
    /* is the master agent already passed ? */
    if (!(SNMPX_GET_GLOB_PTR()->AgtTreePassed))
    {
        if (snmpx_oid_compare(pOid, OidLen, SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN) >= 0)
        {
            SNMPX_OIDCPY((char *)lowest, (char *)SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN);
            lowest_len = SNMPX_SNMP_GROUP_TREE_LEN;
        }
    }

    /* get the entries of the registered mib trees of all users (subagents) */
    for (Channel=0; Channel<SNMPX_CFG_MAX_CHANNELS; Channel++, pHDBUser++)
    {
        LSA_UINT16  i;

        for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
        {
            SNMPX_UPPER_OID_PTR_TYPE pRegisterOid   = pHDBUser->RegisteredOids[i].pOid;
            LSA_UINT16               RegisterOidLen = pHDBUser->RegisteredOids[i].OidLen;

            /* is this sub agent already passed ? */
            if (pHDBUser->SubTreePassed[i])
            {
                continue;
            }

            if (snmpx_is_not_null(pRegisterOid) && (RegisterOidLen != 0))
            {
                if (snmpx_oid_compare(pOid, OidLen, pRegisterOid, RegisterOidLen) >= 0)
                {
                    /* is there already a better ("longer") fit ? */
                    if (snmpx_oid_compare(lowest, lowest_len, pRegisterOid, RegisterOidLen) < 0)
                    {
                        SNMPX_OIDCPY((char *)lowest, pRegisterOid, RegisterOidLen);
                        lowest_len = RegisterOidLen;
                        pUserHandle = pHDBUser;
                        MibTree = i;
                    }
                }
            }
        }
    }

    /* no SubAgent found for GET_NEXT, e.g. Oid {0,0} ? */
    if (lowest_len == 0 && snmpx_is_null(pUserHandle))
    {
        if (MessageType == SNMPX_GETNEXT || MessageType == SNMPX_GETBULK)
        {
            SNMPX_HDB_USER_PTR_TYPE pNextUserHandle;

            /* try to find for GET_NEXT */
            pNextUserHandle = SNMPX_GetNextSubAgent(pObject, pOid, OidLen, MessageType, Version);
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetSubAgent(pNextUserHandle: 0x%X)", pNextUserHandle);
            return(pNextUserHandle);
        }
    }

    /* read object from SNMP-group (it's the internal agent) */
    if (lowest_len == SNMPX_SNMP_GROUP_TREE_LEN && snmpx_is_null(pUserHandle))
    {
        SNMPX_COMMON_MEM_U8_PTR_TYPE    pValue;

        /* mark the master agent as passed one */
        SNMPX_GET_GLOB_PTR()->AgtTreePassed = LSA_TRUE;

        /* try to get the mib-object */
        pValue = snmpx_snmp_get_oid_element(pObject->pOid,
                                            &pObject->OidLen,
                                            &pObject->VarType,
                                            &pObject->VarLength,
                                            &pObject->AccessControl,
                                            pObject->MessageType,
                                            &pObject->SnmpError,
                                            pObject->SnmpVersion);
        if (snmpx_is_not_null(pValue))
        {
            SNMPX_MEMCPY(pObject->pVarValue, pValue, pObject->VarLength);
        }
        /* nothing found in the local agent (e.g. last OID) */
        if (snmpx_is_null(pValue) &&
            (pObject->MessageType == SNMPX_GETNEXT || pObject->MessageType == SNMPX_GETBULK))
        {
            /* static SNMPX_OID_TYPE NullOid = {0}; */

            SNMPX_HDB_USER_PTR_TYPE pNewUserHandle;

            /* supply the IN params of current MIB object */
            (LSA_VOID)SNMPX_SetObject(pObject, pOid, OidLen, MessageType, Version);

            pNewUserHandle = SNMPX_GetSubAgent(pObject, pOid, OidLen,  /* pOid, OidLen */
                                               MessageType, Version);

            /* if a SNMPX_GetNextSubAgent() is to do, it'll be called in SNMPX_GetSubAgent()! */

            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetSubAgent(pNewUserHandle: 0x%X)", pNewUserHandle);
            return(pNewUserHandle);
        }
    }

    /* mark the sub agent's tree as passed one */
    if (snmpx_is_not_null(pUserHandle))
    {
        SNMPX_ASSERT(MibTree!=0xffff);
        pUserHandle->SubTreePassed[MibTree] = LSA_TRUE;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetSubAgent(pUserHandle: 0x%X)", pUserHandle);
    return(pUserHandle);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_GetNextSubAgent                          +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+  Result                : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : <> NULL            closest match                          +*/
/*+               NULL               no match anyway                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free register entry in a User-HDB       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_HDB_USER_PTR_TYPE SNMPX_GetNextSubAgent(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE pOid,       /* name of var   */
    LSA_UINT                 OidLen,     /* number of sub-ids in name */
    LSA_UINT8                MessageType,/* message type  */
    LSA_INT                  Version     /* SNMPX version */
)
{
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser = SNMPX_Data.HUser;
    LSA_UINT16                  Channel;
    SNMPX_HDB_USER_PTR_TYPE     pUserHandle = LSA_NULL;

    SNMPX_OID_TYPE              next[SNMPX_MAX_OID_LEN];        /* "best next fit" object Id */
    LSA_UINT16                  next_len = 0;
    LSA_UINT16                  MibTree = 0xffff;

    SNMPX_OID_TYPE              SnmpGroupOid[] = {SNMPX_SNMP_GROUP_TREE};

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetNextSubAgent(pObject: %Xh, pOid: %Xh, OidLen: %Xh, MessageType: %Xh, Version: %Xh)",
                            pObject, pOid, OidLen, MessageType, Version);

    if (snmpx_is_null(pOid) || OidLen == 0 || snmpx_is_null(pObject))
    {
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetNextSubAgent(pUserHandle: 0x%X)", pUserHandle);
        return(pUserHandle);
    }

    /* check asn message type, set user message type */
    switch (MessageType)
    {
        case SNMPX_GETNEXT: /* only NEXT's are allowed */
        case SNMPX_GETBULK:
            break;
        default:
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    /* is the master agent already passed ? */
    if (!(SNMPX_GET_GLOB_PTR()->AgtTreePassed))
    {
        /* if new one is greater than input and closer to input than
         * previous lowest, save this one as the "next" one. */
        if (snmpx_snmp_compare(SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN, pOid, OidLen) > 0)
        {
            SNMPX_OIDCPY((char *)next, (char *)SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN);
            next_len = SNMPX_SNMP_GROUP_TREE_LEN;
        }
    }

    /* get the entries of the registered mib trees of all users (subagents) */
    for (Channel=0; Channel<SNMPX_CFG_MAX_CHANNELS; Channel++, pHDBUser++)
    {
        LSA_UINT16  i;

        for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
        {
            SNMPX_UPPER_OID_PTR_TYPE pRegisterOid   = pHDBUser->RegisteredOids[i].pOid;
            LSA_UINT16               RegisterOidLen = pHDBUser->RegisteredOids[i].OidLen;

            /* is this sub agent already passed ? */
            if (pHDBUser->SubTreePassed[i])
            {
                continue;
            }

            if (snmpx_is_not_null(pRegisterOid) && (RegisterOidLen != 0))
            {
                /* if new one is greater than input and closer to input than
                 * previous lowest, save this one as the "next" one. */
                if (snmpx_snmp_compare(pRegisterOid, RegisterOidLen, pOid, OidLen) > 0)
                {
                    if (next_len == 0 ||
                        snmpx_snmp_compare(pRegisterOid, RegisterOidLen, next, next_len) < 0)
                    {
                        SNMPX_OIDCPY((char *)next, (char *)pRegisterOid, RegisterOidLen);
                        next_len = RegisterOidLen;
                        pUserHandle = pHDBUser;
                        MibTree = i;
                    }
                }
            }
        }
    }

    /* read object from SNMP-group (it's the internal agent) */
    if (next_len == SNMPX_SNMP_GROUP_TREE_LEN && snmpx_is_null(pUserHandle))
    {
        SNMPX_COMMON_MEM_U8_PTR_TYPE    pValue;

        /* mark the master agent as passed one */
        SNMPX_GET_GLOB_PTR()->AgtTreePassed = LSA_TRUE;

        /* try to get the mib-object */
        pValue = snmpx_snmp_get_oid_element(pObject->pOid,
                                            &pObject->OidLen,
                                            &pObject->VarType,
                                            &pObject->VarLength,
                                            &pObject->AccessControl,
                                            pObject->MessageType,
                                            &pObject->SnmpError,
                                            pObject->SnmpVersion);
        if (snmpx_is_not_null(pValue))
        {
            SNMPX_MEMCPY(pObject->pVarValue, pValue, pObject->VarLength);
        }

        /* nothing found in the local agent (e.g. last OID) */
        if (snmpx_is_null(pValue))
        {
            SNMPX_HDB_USER_PTR_TYPE pNextUserHandle;

            /* supply current MIB object */
            (LSA_VOID)SNMPX_SetObject(pObject, pOid, OidLen, MessageType, Version);

            /* try to find for another "next" subagent */
            pNextUserHandle = SNMPX_GetNextSubAgent(pObject, pOid, OidLen, MessageType, Version);
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetNextSubAgent(pNextUserHandle: 0x%X)", pNextUserHandle);
            return(pNextUserHandle);
        }
    }

    /* mark the sub agent's tree as passed one */
    if (snmpx_is_not_null(pUserHandle))
    {
        SNMPX_ASSERT(MibTree!=0xffff);
        pUserHandle->SubTreePassed[MibTree] = LSA_TRUE;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetNextSubAgent(pUserHandle: 0x%X)", pUserHandle);
    return(pUserHandle);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_GetCloserSubAgent                        +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+  Result                : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : <> NULL            closest match                          +*/
/*+               NULL               no match anyway                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get the user handle if a closer subagent exists           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_HDB_USER_PTR_TYPE SNMPX_GetCloserSubAgent(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version     /* SNMPX version */
)
{
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser = SNMPX_Data.HUser;
    LSA_UINT16                  Channel;
    SNMPX_HDB_USER_PTR_TYPE     pUserHandle = LSA_NULL;

    LSA_UINT16                  prior_len = 0;
    LSA_UINT16                  MibTree = 0xffff;
    SNMPX_OID_TYPE              prior[SNMPX_MAX_OID_LEN];  /* "closest fit" object Id */

    SNMPX_OID_TYPE              SnmpGroupOid[] = {SNMPX_SNMP_GROUP_TREE};

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetCloserSubAgent(pObject: %Xh, pOid: %Xh, OidLen: %Xh, MessageType: %Xh)", pObject, pOid, OidLen, MessageType);

    if (snmpx_is_null(pOid) || OidLen == 0 || snmpx_is_null(pObject))
    {
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetCloserSubAgent(pUserHandle: 0x%X)", pUserHandle);
        return(pUserHandle);
    }

    /* check asn message type, set user message type */
    switch (MessageType)
    {
        case SNMPX_GETNEXT: /* only NEXT's are allowed */
        case SNMPX_GETBULK:
            break;
        default:
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    if (!SNMPX_GET_GLOB_PTR()->AgtTreePassed)
    {
        /* is the current OID smaller than the registered one ? */
        if (snmpx_snmp_compare(SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN, pOid, OidLen) > 0)
        {
            /* is the returned OID bigger than the registered one ? */
            if (snmpx_snmp_compare(SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN, pObject->pOid, pObject->OidLen) < 0)
            {
                SNMPX_OIDCPY((char *)prior, (char *)SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN);
                prior_len = SNMPX_SNMP_GROUP_TREE_LEN;
            }
        }
    }

    /* get the entries of the registered mib trees of all users (subagents) */
    for (Channel=0; Channel<SNMPX_CFG_MAX_CHANNELS; Channel++, pHDBUser++)
    {
        LSA_UINT16  i;

        for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
        {
            SNMPX_UPPER_OID_PTR_TYPE pRegisterOid   = pHDBUser->RegisteredOids[i].pOid;
            LSA_UINT16               RegisterOidLen = pHDBUser->RegisteredOids[i].OidLen;

            /* is this sub agent already passed ? */
            if (pHDBUser->SubTreePassed[i])
            {
                continue;
            }

            if (snmpx_is_not_null(pRegisterOid) && (RegisterOidLen != 0))
            {
                /* is the current OID smaller than the registered one ? */
                if (snmpx_snmp_compare(pRegisterOid, RegisterOidLen, pOid, OidLen) > 0)
                {
                    /* is the returned OID bigger than the registered one ? */
                    if (snmpx_snmp_compare(pRegisterOid, RegisterOidLen, pObject->pOid, pObject->OidLen) < 0)
                    {
                        if (prior_len == 0 ||
                            snmpx_snmp_compare(pRegisterOid, RegisterOidLen, prior, prior_len) < 0)
                        {
                            SNMPX_OIDCPY((char *)prior, (char *)pRegisterOid, RegisterOidLen);
                            prior_len = RegisterOidLen;
                            pUserHandle = pHDBUser;
                            MibTree = i;    /* store the found mib tree */
                        }
                    }
                }
            }
        }
    }

    /* read object from SNMP-group (it's the internal agent) */
    /* if the local agent is the closest, there's no other closer one ! */
    if (prior_len != 0 && snmpx_is_null(pUserHandle))
    {
        SNMPX_COMMON_MEM_U8_PTR_TYPE    pValue;

        /* mark the master agent as passed one */
        SNMPX_GET_GLOB_PTR()->AgtTreePassed = LSA_TRUE;

        /* supply current MIB object */
        SNMPX_SetObject(pObject, pOid, OidLen, MessageType, Version);

        /* get the mib-object */
        pValue = snmpx_snmp_get_oid_element(pObject->pOid,
                                            &pObject->OidLen,
                                            &pObject->VarType,
                                            &pObject->VarLength,
                                            &pObject->AccessControl,
                                            pObject->MessageType,
                                            &pObject->SnmpError,
                                            pObject->SnmpVersion);
        if (snmpx_is_not_null(pValue))
        {
            SNMPX_MEMCPY(pObject->pVarValue, pValue, pObject->VarLength);
        }

        /* always find a mib object in the local agent, at minimum one (first) oid must be there! */
        if (snmpx_is_null(pValue))
        {
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }
    }

    /* mark the sub agent's tree as passed one */
    if (snmpx_is_not_null(pUserHandle))
    {
        SNMPX_ASSERT(MibTree!=0xffff);
        pUserHandle->SubTreePassed[MibTree] = LSA_TRUE;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetCloserSubAgent(pUserHandle: 0x%X)", pUserHandle);
    return(pUserHandle);
}

/*****************************************************************************/
/* Internal function: SNMPX_SetObject()                                      */
/* set the current MIB object                                                */
/*****************************************************************************/
LSA_UINT16 SNMPX_SetObject(
    SNMPX_UPPER_OBJECT_PTR_TYPE    pObject,
    SNMPX_UPPER_OID_PTR_TYPE       pOid,       /* name of var   */
    LSA_UINT                       OidLen,     /* number of sub-ids in name */
    LSA_UINT8                      MsgType,    /* message type  */
    LSA_INT                        Version)    /* SNMPX version */
{
    LSA_UINT16                  RetVal = SNMPX_OK;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SetObject(pObject: 0x%X, pOid: 0x%X, OidLen: 0x%X, MsgType: 0x%X, Version: 0x%X)",
                            pObject, pOid, OidLen, MsgType, Version);

    if (snmpx_is_null(pObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    if (OidLen > SNMPX_MAX_OID_LEN)
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    if ((snmpx_is_null(pOid) && OidLen != 0) ||     /* both values not NULL */
        (snmpx_is_not_null(pOid) && OidLen == 0))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    if (snmpx_is_null(pObject->pOid))
    {
        SNMPX_ALLOC_LOCAL_MEM(&pObject->pOid, SNMPX_MAX_OID_LEN * sizeof(SNMPX_OID_TYPE));

        if (snmpx_is_null(pObject->pOid))
        {
            RetVal = SNMPX_ERR_RESOURCE;
            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_SetObject(). Allocating MIB Object failed.");
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SetObject(RetVal: 0x%X)",RetVal);
            return (RetVal);
        }
    }

    if (snmpx_is_not_null(pOid) && OidLen != 0)
    {
        SNMPX_OIDCPY(pObject->pOid, pOid, OidLen);
    }

    if (snmpx_is_null(pObject->pVarValue))
    {
        SNMPX_ALLOC_LOCAL_MEM(&pObject->pVarValue, SNMPX_SNMP_MAX_LEN);

        if (snmpx_is_null(pObject->pVarValue))
        {
            RetVal = SNMPX_ERR_RESOURCE;
            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_SetObject(). Allocating MIB Object failed.");
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SetObject(RetVal: 0x%X)",RetVal);
            return (RetVal);
        }
    }

    pObject->OidLen        = OidLen;
    pObject->MessageType   = MsgType;
    pObject->SnmpVersion   = Version;
    pObject->SnmpError     = SNMPX_SNMP_ERR_NOERROR;
    pObject->VarType       = 0;
    pObject->AccessControl = SNMPX_NO_ACCESS;
    pObject->VarLength     = 0;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SetObject(RetVal: 0x%X)",RetVal);
    return (RetVal);
}

/*****************************************************************************/
/* Internal function: SNMPX_CopyObject()                                     */
/* set the current MIB object                                                */
/*****************************************************************************/
LSA_UINT16 SNMPX_CopyObject(
    SNMPX_UPPER_OBJECT_PTR_TYPE    pDestObject,
    SNMPX_UPPER_OBJECT_PTR_TYPE    pSrcObject)
{
    LSA_UINT16                  RetVal = SNMPX_OK;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CopyObject(pDestObject: 0x%X, pSrcObject: 0x%X)",
                            pDestObject, pSrcObject);

    if (snmpx_is_null(pDestObject) || snmpx_is_null(pSrcObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    if (pSrcObject->OidLen > SNMPX_MAX_OID_LEN || pSrcObject->VarLength > SNMPX_SNMP_MAX_LEN)
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    if (snmpx_is_null(pSrcObject->pOid) || snmpx_is_null(pDestObject->pOid))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    if (snmpx_is_null(pSrcObject->pVarValue) || snmpx_is_null(pDestObject->pVarValue))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        return (SNMPX_ERR_PARAM);
    }

    SNMPX_OIDCPY(pDestObject->pOid, pSrcObject->pOid, pSrcObject->OidLen);
    SNMPX_OIDCPY(pDestObject->pVarValue, pSrcObject->pVarValue, pSrcObject->VarLength);

    pDestObject->TransactionID = pSrcObject->TransactionID;
    pDestObject->OidLen        = pSrcObject->OidLen;
    pDestObject->MessageType   = pSrcObject->MessageType;
    pDestObject->SnmpVersion   = pSrcObject->SnmpVersion;
    pDestObject->SnmpError     = pSrcObject->SnmpError;
    pDestObject->VarType       = pSrcObject->VarType;
    pDestObject->AccessControl = pSrcObject->AccessControl;
    pDestObject->VarLength     = pSrcObject->VarLength;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_CopyObject(RetVal: 0x%X)",RetVal);
    return (RetVal);
}

/*****************************************************************************/
/* Internal function: SNMPX_DeleteObjects()                                  */
/* free the local MIB-Objects "prior" and "current"                          */
/*****************************************************************************/
LSA_VOID SNMPX_DeleteObjects(LSA_VOID)
{
    LSA_UINT16                  RetVal = LSA_RET_OK;
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_DeleteObjects()");

    /* the "current" object */
    pObject = SNMPX_GetCurrentObject();
    if (snmpx_is_null(pObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    if (snmpx_is_not_null(pObject->pOid))
    {
        SNMPX_FREE_LOCAL_MEM(&RetVal,pObject->pOid);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }

    if (snmpx_is_not_null(pObject->pVarValue))
    {
        SNMPX_FREE_LOCAL_MEM(&RetVal,pObject->pVarValue);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }

    SNMPX_MEMSET(pObject, 0, sizeof(SNMPX_OBJECT_TYPE));

    /* the "prior" object */
    pObject = SNMPX_GetCloserObject();
    if (snmpx_is_null(pObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    if (snmpx_is_not_null(pObject->pOid))
    {
        SNMPX_FREE_LOCAL_MEM(&RetVal,pObject->pOid);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

        pObject->pOid = LSA_NULL;
    }

    if (snmpx_is_not_null(pObject->pVarValue))
    {
        SNMPX_FREE_LOCAL_MEM(&RetVal,pObject->pVarValue);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }

    SNMPX_MEMSET(pObject, 0, sizeof(SNMPX_OBJECT_TYPE));

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_DeleteObjects(RetVal: 0x%X)", RetVal);
    return;
}

/*****************************************************************************/
/* Internal function: SNMPX_GetCurrentObject()                               */
/* init the current MIB object                                               */
/*****************************************************************************/
SNMPX_UPPER_OBJECT_PTR_TYPE SNMPX_GetCurrentObject(LSA_VOID)
{
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetObject()");

    pObject = &SNMPX_GET_GLOB_PTR()->ActiveObject;

    if (snmpx_is_null(pObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_GetCurrentObject(pObject: 0x%X)",pObject);

    return(pObject);
}

/*****************************************************************************/
/* Internal function: SNMPX_GetCloserObject()                                */
/* init the current MIB object                                               */
/*****************************************************************************/
SNMPX_UPPER_OBJECT_PTR_TYPE SNMPX_GetCloserObject(LSA_VOID)
{
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetCloserObject()");

    pObject = &SNMPX_GET_GLOB_PTR()->CloserObject;

    if (snmpx_is_null(pObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_GetCloserObject(pObject: 0x%X)",pObject);

    return(pObject);
}


/*****************************************************************************/
/* Internal function: SNMPX_OID_SwitchCloser()                               */
/* set the closer MIB object if existing                                     */
/*****************************************************************************/
SNMPX_HDB_USER_PTR_TYPE SNMPX_OID_SwitchCloser(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version)    /* SNMPX version */
{
    SNMPX_UPPER_OBJECT_PTR_TYPE pCloserObject;
    SNMPX_HDB_USER_PTR_TYPE     pUserHandle = LSA_NULL;
    LSA_UINT16                  RetVal;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_OID_SwitchCloser(pObject: 0x%X, pOid: 0x%X, OidLen: 0x%X, MessageType: 0x%X, Version: 0x%X)",
                            pObject, pOid, OidLen, MessageType, Version);

    pCloserObject = SNMPX_GetCloserObject();

    if (snmpx_is_null(pCloserObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    while (1)
    {
        /* supply the IN params of the current MIB object */
        (LSA_VOID)SNMPX_SetObject(pCloserObject, pOid, OidLen, MessageType, Version);

        /* we check the responded oid, if there's already a closer subagent between */
        /* request and response, we delete the object and repeat the request        */
        pUserHandle = SNMPX_GetCloserSubAgent(pObject, pOid, OidLen,     /* Oid, OidLen */
                                              MessageType, Version);

        /* found no match of any user (subagent) */
        if (snmpx_is_null(pUserHandle))/* end of MIB or internal subagent, no more subagents */
        {
            break;
        }

        /************************************************************/
        /*      send indication to (closest) user (subagent)        */
        /************************************************************/

        /* mark the sub agent already as passed one */
        /* pUserHandle->SubTreePassed[] = LSA_TRUE; */
        /* already done in SNMPX_GetCloserSubAgent() */

        /* send request to the found agent */
        RetVal = SNMPX_IndicateOidReceive(pUserHandle, pCloserObject);

        if (RetVal == SNMPX_ERR_RESOURCE)   /* no indication resources provided */
        {
            continue;   /* look for an other subagent */
        }

        if (RetVal != SNMPX_OK)
        {
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }

        /************************************************************/
        /*      response from (closest) user (subagent)             */
        /************************************************************/
        {
            SNMPX_UPPER_RQB_PTR_TYPE  pRQB = pUserHandle->pReceiveReq;

            /* queue is empty */
            if (snmpx_is_null(pRQB))
            {
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            }

            SNMPX_RequestOidReceive(pRQB, pUserHandle);

            /* mark the sub agent as passed one */
            /* pUserHandle->SubTreePassed[] = LSA_TRUE; */
            /* already done in SNMPX_GetCloserSubAgent() */

            pCloserObject = pUserHandle->pCurrentObject;
            if (snmpx_is_null(pCloserObject))
            {
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            }

            /* we just look for a closer subagent */
            if (snmpx_is_not_null(pCloserObject->pVarValue))
            {
                /* Copy the prior Object to the current object - look for another subagent once again */
                (LSA_VOID)SNMPX_CopyObject(pObject, pCloserObject);
            }
            else    /* an "empty" subagent - look for another subagent once again */
            {
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"A subagent is found, but has no obejcts returned!");
            }
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_OID_SwitchCloser(pUserHandle: 0x%X)", pUserHandle);
    return(pUserHandle);
}

/*****************************************************************************/
/* Internal function: SNMPX_OID_Switcher()                                   */
/* set the current MIB object                                                */
/*****************************************************************************/
SNMPX_HDB_USER_PTR_TYPE SNMPX_OID_Switcher(
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen,     /* number of sub-ids in name */
    LSA_UINT8                   MessageType,/* message type  */
    LSA_INT                     Version)    /* SNMPX version */
{
    SNMPX_HDB_USER_PTR_TYPE     pUserHandle = LSA_NULL;
    LSA_UINT16                  RetVal;

    SNMPX_FUNCTION_TRACE_05(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_OID_Switcher(pObject: 0x%X, pOid: 0x%X, OidLen: 0x%X, MessageType: 0x%X, Version: 0x%X)",
                            pObject, pOid, OidLen, MessageType, Version);

    if (snmpx_is_null(pObject))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    /* find the (next) match of a user (subagent) */
    while (1)
    {
        /* supply the IN params of current MIB object */
        (LSA_VOID)SNMPX_SetObject(pObject, pOid, OidLen, MessageType, Version);

        /* find the closest match of a user (subagent) */
        pUserHandle = SNMPX_GetSubAgent(pObject, pOid, OidLen,  /* pOid, OidLen */
                                        MessageType, Version);

        /* found no match of any user (subagent) */
        if (snmpx_is_null(pUserHandle))/* end of MIB or internal subagent, no more subagents */
        {
            break;
        }

        /* is the match of this user (subagent) o.k. */
        if (0 == SNMPX_GetCountOfRegisteredOids(pUserHandle))
        {
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }

        /********************************************************************/
        /*          send indication to (closest) user (subagent)            */
        /********************************************************************/

        /* mark the sub agent already as passed one */
        /* pUserHandle->MibTreePassed[] = LSA_TRUE; */
        /* alerady marked in SNMPX_GetSubAgent()    */

        /* send request to the found agent */
        RetVal = SNMPX_IndicateOidReceive(pUserHandle, pObject);

        if (RetVal == SNMPX_ERR_RESOURCE)   /* no indication resources provided */
        {
            continue;   /* look for an other subagent */
        }

        if (RetVal != SNMPX_OK)
        {
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }

        /********************************************************************/
        /*          response from (closest) user (subagent)                 */
        /********************************************************************/
        {
            SNMPX_UPPER_RQB_PTR_TYPE  pRQB = pUserHandle->pReceiveReq;

            /* queue is empty */
            if (snmpx_is_null(pRQB))
            {
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            }

            SNMPX_RequestOidReceive(pRQB, pUserHandle);

            pObject = pUserHandle->pCurrentObject;
            if (snmpx_is_null(pObject))
            {
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            }

            /* end of a GET,... */
            if (MessageType != SNMPX_GETNEXT && MessageType != SNMPX_GETBULK)
            {
                SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                                        "OUT: SNMPX_OID_Switcher(pObject: 0x%X)", pObject);
                return(pUserHandle);    /* ========>>>  */
            }

            /* end of a GET_NEXT,... */
            if (pObject->SnmpError == SNMPX_SNMP_ERR_NOERROR)       /* object found */
            {
                SNMPX_HDB_USER_PTR_TYPE     pCloserUserHandle;

                /* we check the responded oid, if there's already a closer subagent */
                /* "between" request and response                                   */
                pCloserUserHandle = SNMPX_OID_SwitchCloser(pObject, pOid, OidLen, MessageType, Version);

                /* the same agent - not a closer agent is found */
                if (LSA_HOST_PTR_ARE_EQUAL(pCloserUserHandle, pUserHandle))
                {
                    /* the origin user must have been already marked as passed in GetCloserSubagent() */
                    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                }

                /* o.k., finally we have found the oid */
                SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                                        "OUT: SNMPX_OID_Switcher(pUserHandle: 0x%X)", pUserHandle);
                return(pUserHandle);    /* ========>>>  */
            }
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_OID_Switcher(pUserHandle: 0x%X)", pUserHandle);
    return(pUserHandle);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_CheckOidResponse                      +*/
/*+                             SNMPX_HDB_USER_PTR_TYPE      pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a Response to one user (SubAgent)               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_CheckOidResponse(
    SNMPX_HDB_USER_PTR_TYPE       pHDBUser,
    SNMPX_UPPER_RQB_PTR_TYPE      pRQB
)
{
    LSA_UINT16                  RetVal = SNMPX_OK;
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject;
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CheckOidResponse(pHDBUser: 0x%X)", pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);
    SNMPX_ASSERT_NULL_PTR(pRQB);

    if (snmpx_is_null(pHDBUser->pCurrentObject) || snmpx_is_null(pHDBUser->pCurrentObject->pOid))
    {
        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
    }

    pObject = &pRQB->args.Object;

    /* check the "GetSubAgent"-response */
    if (pObject->SnmpError == SNMPX_SNMP_ERR_NOERROR)       /* object found */
    {
        SNMPX_UPPER_REGISTER_PTR_TYPE pRegisteredOid;

        /* subagent not registered */
        pRegisteredOid = SNMPX_GetRegisteredOid(pHDBUser, pObject->pOid, pObject->OidLen);

        if (snmpx_is_null(pRegisteredOid))
        {
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
        }

        /* error, if a smaller object is delivered as registered */
        /* or any other object deliverd as registered */
        if (pObject->OidLen < pRegisteredOid->OidLen ||
            pObject->OidLen > SNMPX_MAX_OID_LEN ||
            (!SNMPX_OIDCMP(pRegisteredOid->pOid, pObject->pOid, pRegisteredOid->OidLen)))
        {
            /* no such oid registered by this user (subagent) */
            RetVal = SNMPX_ERR_PARAM;
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckOidResponse(RetVal: 0x%X)", RetVal);
            return (RetVal);
        }

        /* GET_NEXT: delievered obejct must be greater */
        if (pObject->MessageType == SNMPX_GETNEXT || pObject->MessageType == SNMPX_GETBULK)
        {
            if (snmpx_snmp_compare(pRegisteredOid->pOid,
                                   pRegisteredOid->OidLen,
                                   pObject->pOid, pObject->OidLen) > 0)
            {
                /* new one must is smaller than input oid */
                RetVal = SNMPX_ERR_PARAM;
                SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckOidResponse(RetVal: 0x%X)", RetVal);
                return (RetVal);
            }
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckOidResponse(RetVal: 0x%X)", RetVal);
    return (RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_ParseAgent                            +*/
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
/*+  Description: parses an incoming SNMP frame                             +*/
/*+               (SOCK_OPC_UDP_RECEIVE)                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_ParseAgent(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pResRQB;
    LSA_UINT16                     Response;
    SNMPX_SOCK_UDP_DATA_PTR_TYPE   udp_recv;
    LSA_USER_ID_TYPE               UserId;
    LSA_INT                        len = 0;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_ParseAgent(pRQB: 0x%X,pHDB: 0x%X)",
                            pRQB,pHDB);

    UserId.uvar32 = 0;
    udp_recv = &pRQB->args.data;
    Response  = SNMPX_OK;

    SNMPX_ASSERT_NULL_PTR(udp_recv);
    SNMPX_ASSERT_NULL_PTR(udp_recv->buffer_ptr); /* received data */

    /* free the Ind-Rqb  */
    if (!(pHDB->SockState == SNMPX_HANDLE_STATE_OPENED && pHDB->AgntPortState == SNMPX_SNMP_STATE_OPEN))
    {
        LSA_UINT16 RetVal;

        /* free the Ind-Rqb  */
        SNMPX_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.data.buffer_ptr,pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

        SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

        Response = SNMPX_OK_CANCEL;
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_ParseAgent(Response: 0x%X)",Response);
        return(Response);
    }

    /* ---------------------------------------------*/
    /* Successfully done the request.               */
    /* ---------------------------------------------*/
    /* parse framedata for LLDP-MIB */

    /* alloc Response-RQB */
    pResRQB = snmpx_sock_alloc_rsp_rqb(pHDB, SNMPX_HANDLE_TYPE_SOCK, 0xFFFF); /* RQB for SOCK_OPC_UDP_RECEIVE with buffer for packet building */

    if ( LSA_HOST_PTR_ARE_EQUAL(pResRQB, LSA_NULL) )
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower response RQB-memory failed!");
        Response = SNMPX_RSP_ERR_RESOURCE;
    }
    else
    {
        SNMPX_ASSERT_NULL_PTR(pResRQB->args.data.buffer_ptr);

        /* Main entry point for incoming snmp packets directed to the agent's UDP connection. */
        len = snmpx_agt_parse_snmp(udp_recv->buffer_ptr, udp_recv->data_length,
                                   pResRQB->args.data.buffer_ptr, pResRQB->args.data.buffer_length, &Response);

        if (Response != SNMPX_OK_ACTIVE)
        {
            /* if it's no OID for any subagent, snmpx_snmp_agt_parse() has made a dummy-response in snmp_recv->recv_buffer */
            /* send this packet for responding the remote SNMP manager */
            /* parse worked, send response packet */

            if (len == 0) /* parsing failed, free the response RQB */
            {
                snmpx_sock_free_rsp_rqb(pHDB, pRQB);
            }
            else    /* SNMPX can respond the SNMP-manager */
            {
                /*  SNMPX_ASSERT (pResRQB->args.data.buffer_ptr);        */
                /*  len = snmpx_snmp_agt_parse(udp_recv->buffer_ptr, udp_recv->data_length, pResRQB->args.data.buffer_ptr, SNMPX_SNMP_SIZE); */
                /*  if(len)   */   /* parse worked, send response packet */
                /*  {                                                    */
                /*      pResRQB->args.data.sock_fd = pHDB->SockFD;       */

                pResRQB->args.data.sock_fd = udp_recv->sock_fd;    /* Socketdescriptor  */

                /* SOCKADDR-structure with the remote Adress */
                /*      pResRQB->args.data.rem_addr.sin_family = SOCK_AF_INET; */
                /*      pResRQB->args.data.rem_addr.sin_addr = udp_recv->rem_addr.sin_addr; */ /* remote IP-Adress  */
                /*      pResRQB->args.data.rem_addr.sin_port = udp_recv->rem_addr.sin_port; */ /* remote Portnumber */

                SNMPX_MEMCPY(&pResRQB->args.data.rem_addr, &udp_recv->rem_addr, sizeof(SOCK_SOCKADDR_TYPE));  /* for UDP */

                /* swap the bytes (SOCK_HTONS) ? */
                pResRQB->args.data.rem_addr.sin_port = /*SNMPX_SWAP16*/(pResRQB->args.data.rem_addr.sin_port);

                /* entry of the send buffers */
                pResRQB->args.data.data_length = (LSA_UINT16)len;

                SNMPX_SOCK_REQUEST_LOWER(pResRQB,pHDB->Params.pSys);
                pHDB->RxPendingCnt++; /* number of Rx-Request pending within SOCK */
            }
        }
    }

    /* reprovide the Ind-Rqb  */
    SNMPX_DoRecvProvide(pHDB, pRQB);

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_ParseAgent(Response: 0x%X)",Response);
    return(Response);
}

#endif    /* SNMPX_CFG_SNMP_AGENT */

/*****************************************************************************/
/*  end of file SNMPX_MSTR.C                                                 */
/*****************************************************************************/

