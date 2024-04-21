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
/*  F i l e               &F: snmpx_lib.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX-global functions                           */
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

#define LTRC_ACT_MODUL_ID  2
#define SNMPX_MODULE_ID     LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_LIB */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles */
#include "snmpx_int.h"            /* internal header   */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_FatalError                            +*/
/*+  Input/Output               SNMPX_FATAL_ERROR_TYPE Error                +*/
/*+  Input/Output          :    LSA_UINT16            ModuleID              +*/
/*+                             LSA_UINT16            Line                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : SNMPX-fatal-errorcode                           +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via SNMPX_FATAL_ERROR macro.          +*/
/*+               This function does not return!                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_FatalError(
    SNMPX_FATAL_ERROR_TYPE   Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line)

{
    LSA_FATAL_ERROR_TYPE    LSAError;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "IN : SNMPX_FatalError()");
    LSAError.lsa_component_id  = LSA_COMP_ID_SNMPX;
    LSAError.module_id         = ModuleID;
    LSAError.line              = (LSA_UINT16) Line;
    LSAError.error_code[0]     = (LSA_UINT32) Error;
    LSAError.error_code[1]     = 0;
    LSAError.error_code[2]     = 0;
    LSAError.error_code[3]     = 0;
    LSAError.error_data_length = 0;
    LSAError.error_data_ptr    = LSA_NULL; /* currently no message */

    SNMPX_PROGRAM_TRACE_03(LSA_TRACE_LEVEL_FATAL,"OUT: SNMPX_FatalError. ModID: %d, Line: %d, Error: %Xh",ModuleID,Line,Error);

    SNMPX_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_CallCbf                               +*/
/*+  Input/Output               LSA_VOID LSA_FCT_PTR..      Cbf             +*/
/*+                             SNMPX_UPPER_RQB_PTR_TYPE      pRQB          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Cbf                  : Pointer to Callbackfunction                     +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calls Cbf with RQB.                                       +*/
/*+               if Cbf is LSA_NULL Cbf is not called.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_CallCbf(
    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE Cbf,
    SNMPX_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE              pSys)
{
    SNMPX_ASSERT_NULL_PTR(pRQB);

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CallCbf(Cbf: 0x%X,pRQB: 0x%X, RQB-Status: 0x%X)",
                            Cbf,pRQB,(SNMPX_RQB_GET_RESPONSE(pRQB)));

#if SNMPX_CFG_TRACE_RQB_ERRORS
    switch ( SNMPX_RQB_GET_RESPONSE(pRQB))
    {
        case SNMPX_OK:
        case SNMPX_OK_CANCEL:
            break;
        default:
            SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE_HIGH,"RQB-Response error [opcode: 0x%X, Handle: 0x%X, Resp: 0x%X]",SNMPX_RQB_GET_OPCODE(pRQB),SNMPX_RQB_GET_HANDLE(pRQB),SNMPX_RQB_GET_RESPONSE(pRQB));
            break;
    }
#endif

    if (Cbf)
    {
        SNMPX_REQUEST_UPPER_DONE(Cbf, pRQB, pSys);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CallCbf()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RQBSetResponse                        +*/
/*+  Input                      SNMPX_UPPER_RQB_PTR_TYPE       pRQB         +*/
/*+                             LSA_UINT16                   Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets Response                                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_RQBSetResponse(
    SNMPX_UPPER_RQB_PTR_TYPE    pRQB,
    LSA_UINT16                Response)
{
    SNMPX_ASSERT_NULL_PTR(pRQB);

    SNMPX_RQB_SET_RESPONSE(pRQB, Response);

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RQBSetResponse(pRQB: 0x%X, Response: 0x%X)",
                            pRQB,Response);
    if (( Response != SNMPX_OK ) &&
        ( Response != SNMPX_OK_CANCEL) )
    {
        SNMPX_PROGRAM_TRACE_02(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_RQBSetResponse: An error occured (Opcode:0x%X, Response:0x%X)",SNMPX_RQB_GET_OPCODE(pRQB),Response);
    }
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RQBSetResponse()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_UserRequestFinish                     +*/
/*+  Input/Output               SNMPX_HDB_USER-PTR_TYPE       pHDB          +*/
/*+                             SNMPX_UPPER_RQB_PTR_TYPE      pRQB          +*/
/*+                             LSA_UINT16                  Response        +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+  Response             : Response to set in RQB                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishs Request by calling call-back-function located in  +*/
/*+               HDB. if Cbf is LSA_NULL noting is done.                   +*/
/*+                                                                         +*/
/*+               Fill the RQB-local-Err-Structure (err) with local SNMPX   +*/
/*+               error if lsa_componente_id is LSA_COMP_ID_UNUSED.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_UserRequestFinish(
    SNMPX_HDB_USER_PTR_TYPE      pHDB,
    SNMPX_UPPER_RQB_PTR_TYPE     pRQB,
    LSA_UINT16                 Response)
{
    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserRequestFinish(pHDB: 0x%X,pRQB: 0x%X,Response: 0x%X)",
                            pHDB,pRQB,Response);

    SNMPX_ASSERT_NULL_PTR(pHDB);
    SNMPX_ASSERT_NULL_PTR(pRQB);

    SNMPX_RQBSetResponse(pRQB,Response);
    SNMPX_RQB_SET_HANDLE(pRQB, pHDB->Params.HandleUpper );

    SNMPX_UPPER_TRACE_04(LSA_TRACE_LEVEL_NOTE,"<<<: Request finished. Opcode: 0x%X, Handle: 0x%Xh,UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),pHDB->ThisHandle,SNMPX_RQB_GET_HANDLE(pRQB),Response);

    SNMPX_CallCbf(pHDB->Params.Cbf,pRQB,pHDB->Params.pSys);

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserRequestFinish()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_GetUserHDBFromHandle                  +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    SNMPX_HDB_USER_PTR_TYPE                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB for.     +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to valid User-HDB for handle.                +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_HDB_USER_PTR_TYPE SNMPX_GetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    SNMPX_HDB_USER_PTR_TYPE pHDB;
    LSA_UINT16        Idx;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetUserHDBFromHandle(Handle: 0x%X)",
                            Handle);

    /* Idx = (LSA_UINT16) (Handle);  */ /* Get User-Idx */

    /* if ( Idx < SNMPX_Data.NDB.UserCnt )  */
    if ( SNMPX_HANDLE_IS_USER(Handle))
    {
        Idx = (LSA_UINT16) SNMPX_HANDLE_IDX(Handle);   /* Get User-Idx */

        if ( Idx < SNMPX_CFG_MAX_CHANNELS /*SNMPX_Data.LowerCnt*/ )
        {
            pHDB = SNMPX_GET_HUSER_PTR(Idx);
        }
        else
        {
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Invalid USER handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }

        /* Handle must be in use (<> free) */
        if (snmpx_is_not_null(pHDB) && pHDB->UserState == SNMPX_HANDLE_STATE_FREE )
        {
            pHDB = LSA_NULL;
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Invalid user handle (0x%X). Out of range.",Handle);
        pHDB = LSA_NULL;
    }
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_GetUserHDBFromHandle(pHDB: 0x%X)",
                            pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPXGetSockHDBFromHandle                   +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    SNMPX_HDB_SOCK_PTR_TYPE                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX SOCK-channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to SOCK-HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_HDB_SOCK_PTR_TYPE SNMPXGetSockHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    SNMPX_HDB_SOCK_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPXGetSockHDBFromHandle(Handle: 0x%X)",
                            Handle);
    if ( SNMPX_HANDLE_IS_SOCK(Handle))
    {
        Idx = (LSA_UINT16) SNMPX_HANDLE_IDX(Handle);   /* Get SOCK-Idx */

        if ( Idx == 0 ) /* one SOCK channel only */
        {
            pHDB = SNMPX_GET_HSOCK_PTR();
        }
        else
        {
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Invalid SOCK handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Invalid SOCK handle (0x%X). No SOCK handle.",Handle);
        pHDB = LSA_NULL;
    }
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPXGetSockHDBFromHandle(pHDB: 0x%X)",
                            pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname        :    SNMPX_SOCKGetFreeHDB                          +*/
/*+  Input/Output        :    SNMPX_HDB_SOCK_PTR_TYPE SNMPX_LOCAL..pHDBSock +*/
/*+  Result              :    LSA_UINT16                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX SOCK  -channel handle to get pointer to HDB for.    +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free SOCK-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_SOCKGetFreeHDB(
    SNMPX_HDB_SOCK_PTR_TYPE *pHDBSock)
{
    LSA_UINT16                Status = SNMPX_OK;
    LSA_BOOL                  Found;
    SNMPX_HDB_SOCK_PTR_TYPE   pHDB  = SNMPX_GET_HSOCK_PTR();

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKGetFreeHDB(pHDBSock: 0x%X)",
                            pHDBSock);

    SNMPX_ASSERT_NULL_PTR(pHDBSock);

    /* ----------------------------------------------------------------------*/
    /* Search for free handle-management                                     */
    /* ----------------------------------------------------------------------*/
    if ( pHDB->SockState == SNMPX_HANDLE_STATE_FREE )
    {
        Found = LSA_TRUE;
    }
    else
    {
        Found = LSA_FALSE;
    }

    /* ----------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..              */
    /* ----------------------------------------------------------------------*/
    if ( Found )
    {
        pHDB->SockState = SNMPX_HANDLE_STATE_CLOSED;
		pHDB->Params.Cbf = 0;
        *pHDBSock = pHDB;
    }
    else
    {
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SNMPX_SOCKGetFreeHDB(). No more free handles.");
    }

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                            Status,*pHDBSock);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKReleaseHDB                        +*/
/*+  Input/Output          :    SNMPX_HDB_SOCK_PTR_TYPE pHDB                +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBSock    : Pointer to User Handle to release                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release SOCK -HDB                                         +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to SNMPX_HANDLE_STATE_FREE              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKReleaseHDB(
    SNMPX_HDB_SOCK_PTR_TYPE pHDB)
{
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKReleaseHDB(pHDB: 0x%X)",
                            pHDB);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    pHDB->SockState = SNMPX_HANDLE_STATE_FREE;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKReleaseHDB()");
}

#if SNMPX_CFG_SNMP_OPEN_BSD
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname        :    SNMPX_OHAGetFreeHDB                          +*/
/*+  Input/Output        :    SNMPX_HDB_SOCK_PTR_TYPE SNMPX_LOCAL..pHDBSock +*/
/*+  Result              :    LSA_UINT16                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX SOCK  -channel handle to get pointer to HDB for.    +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free SOCK-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16		/* mh2290: LINT 508 extern used with definition */
SNMPX_OHAGetFreeHDB (
	SNMPX_HDB_OHA_PTR_TYPE *pHDBOHA
) {
	LSA_UINT16 Status = SNMPX_OK;
	LSA_BOOL Found;
	SNMPX_HDB_OHA_PTR_TYPE pHDB = SNMPX_GET_HOHA_PTR();

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHAGetFreeHDB(pHDBOHA: 0x%X)"
		, pHDBOHA
		);

	if ( pHDB->State == SNMPX_HANDLE_STATE_FREE )
	{
		Found = LSA_TRUE;
	}
	else
	{
		Found = LSA_FALSE;
	}

	if ( Found )
	{
		pHDB->State = SNMPX_HANDLE_STATE_CLOSED;
		*pHDBOHA = pHDB;
	}
	else
	{
		Status = SNMPX_ERR_RESOURCE;

		SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP
			, "SNMPX_OHAGetFreeHDB(). No more free handles."
			);
	}

	SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
		"OUT: SNMPX_OHAGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
		Status,*pHDBOHA);

	return(Status);
}

LSA_VOID		/* mh2290: LINT 508 extern used with definition */
SNMPX_OHAReleaseHDB (
	SNMPX_HDB_OHA_PTR_TYPE pHDB
) {
	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHAReleaseHDB(pHDB: 0x%X)"
		, pHDB
		);

	SNMPX_ASSERT_NULL_PTR(pHDB);

	pHDB->State = SNMPX_HANDLE_STATE_FREE;

	SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
		"OUT: SNMPX_OHAReleaseHDB()"
		);
}

SNMPX_HDB_OHA_PTR_TYPE
SNMPXGetOHAHDBFromHandle (
	LSA_HANDLE_TYPE Handle
) {
	SNMPX_HDB_OHA_PTR_TYPE pHDB;
	LSA_UINT16 Idx;

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPXGetOHAHDBFromHandle(Handle: 0x%X)"
		, Handle
		);

	if ( SNMPX_HANDLE_IS_OHA(Handle))
	{
		Idx = (LSA_UINT16) SNMPX_HANDLE_IDX(Handle);   /* Get OHA-Idx */

		if ( Idx == 0 ) /* one OHA channel only */
		{
			pHDB = SNMPX_GET_HOHA_PTR();
		}
		else
		{
			SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP
				, "Invalid OHA handle (0x%X). Out of range."
				, Handle
				);

			pHDB = LSA_NULL;
		}
	}
	else
	{
		SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP
			, "Invalid OHA handle (0x%X). No OHA handle."
			, Handle
			);
		pHDB = LSA_NULL;
	}

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "OUT: SNMPXGetOHAHDBFromHandle(pHDB: 0x%X)"
		, pHDB
		);

	return(pHDB);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_DoRecvProvide                         +*/
/*+  Input/Output          :    SNMPX_HDB_PTR_TYPE           pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : SNMPX_OK                                        +*/
/*+                         SNMPX_ERR_RESOURCE                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends Indication-Resource to SOCK                         +*/
/*+                            (SOCK_OPC_SNMP_RECEIVE_REQ)                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_DoRecvProvide(
    SNMPX_HDB_SOCK_PTR_TYPE         pHDB,
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE   pRQB
)
{
    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_DoRecvProvide(pHDB: 0x%X, pRQB: 0x%X)",
                            pHDB, pRQB);
    SNMPX_ASSERT(pHDB);
    SNMPX_ASSERT(pRQB);
    SNMPX_ASSERT(pRQB->args.data.buffer_ptr);

    SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_UDP_RECEIVE (SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pRQB);

    pRQB->args.data.flags = 0;  /* wait for all data with SOCK_READ_ALL ? */
    pRQB->args.data.offset = 0; /* ? */
    /* pRQB->args.data.timer_val = 0; */ /* AP01388668: timer references (SOCK) removed */

    pRQB->args.data.sock_fd = pHDB->SockFD;
    pRQB->args.data.data_length = SOCK_SNMP_BUFFER_SIZE;
    pRQB->args.data.buffer_length = SOCK_SNMP_BUFFER_SIZE;

    pHDB->SockRQBCnt++;  /* number of RQBs pending within SOCK */
    pHDB->RxPendingCnt++; /* number of Recv-Request pending within SOCK */

    SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_UDP_RECEIVE (SOCKHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pRQB);
    SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_UDP_RECEIVE);
    SNMPX_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle); /* set handle */
    SNMPX_SOCK_REQUEST_LOWER(pRQB,pHDB->Params.pSys);

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_DoRecvProvide()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_FreeAllRegisterEntries                +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees registered MIB objects if allocated.                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_FreeAllRegisterEntries(SNMPX_HDB_USER_PTR_TYPE pHDBUser)
{
    LSA_UINT16   i;
    LSA_UINT16   RetVal = SNMPX_OK;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_FreeAllRegisterEntries(pHDBUser: 0x%X)", pHDBUser);

    /* free allocated MIB objects */
    SNMPX_ASSERT_NULL_PTR(pHDBUser);

    for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
    {
        if (snmpx_is_not_null(pHDBUser->RegisteredOids[i].pOid))
        {
            /* delete the mib tree in the User-HDB */
            SNMPX_FREE_UPPER_MEM(&RetVal, pHDBUser->RegisteredOids[i].pOid, pHDBUser->Params.pSys);
            SNMPX_ASSERT(RetVal == SNMPX_OK);

            pHDBUser->RegisteredOids[i].Priority = 0;
            pHDBUser->RegisteredOids[i].OidLen   = 0;
            pHDBUser->RegisteredOids[i].pOid     = LSA_NULL;
        }
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_FreeAllRegisterEntries()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_FreeAllSessionEntries                 +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees session entries if allocated.                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_FreeAllSessionEntries(SNMPX_HDB_USER_PTR_TYPE pHDBUser)
{
    LSA_UINT16   i;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_FreeAllSessionEntries(pHDBUser: 0x%X)", pHDBUser);

    /* free allocated community objects */

    for (i=0; i < SNMPX_CFG_MAX_MANAGER_SESSIONS; i++)
    {
        (LSA_UINT16)SNMPX_UserDeleteSessionEntry(pHDBUser, i);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_FreeAllSessionEntries()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_CheckMibTree                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks the entry of the mib tree                          +*/
/*+                                                                         +*/
/*+  LSA_TRUE:  o.k.                                                        +*/
/*+  LSA_FALSE: the mib tree is already registered                          +*/
/*+             or belongs to the MIB-II SNMP-group                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL SNMPX_CheckMibTree(
    SNMPX_UPPER_OID_PTR_TYPE    pOid,
    LSA_UINT16                  OidLen)
{
    LSA_BOOL        RetVal = LSA_TRUE;
    LSA_BOOL        Found;
    LSA_UINT16      Channel;
    SNMPX_HDB_USER_PTR_TYPE   pHDB = SNMPX_Data.HUser;
    SNMPX_OID_TYPE  SnmpGroupOid[] = {SNMPX_SNMP_GROUP_TREE};

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CheckMibTree(pOid: 0x%X)", pOid);

    if (snmpx_is_null(pOid) && OidLen == 0)
    {
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_CheckMibTree()");
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckMibTree(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* does it belong to the MIB-II SNMP-group ? */
    if (OidLen >= SNMPX_SNMP_GROUP_TREE_LEN)
    {
        Found = SNMPX_OIDCMP(pOid, SnmpGroupOid, SNMPX_SNMP_GROUP_TREE_LEN);
        if (Found)
        {
            SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: the mib tree belongs to the MIB-II SNMP-group, which is handled by SNMPX itself");
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckMibTree(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
    }

    /* get the entries of the mib tree of all users (subagents) */
    for (Channel=0; Channel<SNMPX_CFG_MAX_CHANNELS; Channel++, pHDB++)
    {
        LSA_UINT16  i;

        for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
        {
            SNMPX_UPPER_OID_PTR_TYPE    pRegisterOid    = pHDB->RegisteredOids[i].pOid;
            LSA_UINT16                  RegisterOidLen  = pHDB->RegisteredOids[i].OidLen;

            if (snmpx_is_not_null(pRegisterOid) && (RegisterOidLen == OidLen))
            {
                /* is the mib tree is already registered by any user ? */
                Found = SNMPX_OIDCMP(pRegisterOid, pOid, OidLen);
                if (Found)
                {
                    RetVal = LSA_FALSE;
                    break;
                }
            }
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckMibTree(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_InitMibTrees                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: inits the entry of all mib trees                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_InitMibTrees(LSA_VOID)
{
    LSA_UINT16      Channel;
    SNMPX_HDB_USER_PTR_TYPE   pHDB = SNMPX_Data.HUser;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_InitMibTrees()");

    /* init the entries of the mib tree of all users (subagents) */
    for (Channel=0; Channel<SNMPX_CFG_MAX_CHANNELS; Channel++, pHDB++)
    {
        LSA_UINT16      Trees;
        for (Trees=0; Trees < SNMPX_CFG_MAX_OIDS; Trees++)
        {
            pHDB->SubTreePassed[Trees] = LSA_FALSE;
        }
    }

    /* ... and the master agent himself */
    SNMPX_GET_GLOB_PTR()->AgtTreePassed = LSA_FALSE;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_InitMibTrees()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_sock_alloc_rsp_rqb                    +*/
/*+  Input/Output          :    SNMPX_HDB_PTR_TYPE             pHDB         +*/
/*+                                                                         +*/
/*+  Result                :    SNMPX_SOCK_LOWER_RQB_PTR_TYPE               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pRQBSnmp             : Recv-RQB for filling the Send-RQB               +*/
/*+                                                                         +*/
/*+  Result               : <> NULL   allocated SockRQB                     +*/
/*+                         NULL   could not allocate (ErrResource          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: allocate a Response-RQB for SOCK                          +*/
/*+               (SOCK_OPC_UDP_SEND)                                       +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe UDP-Request pending      +*/
/*+                     within SOCK!                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_SOCK_LOWER_RQB_PTR_TYPE snmpx_sock_alloc_rsp_rqb(
    SNMPX_HDB_SOCK_PTR_TYPE     pHDB,
    LSA_UINT16                  Orginator,
    LSA_UINT16                  SessionIndex
)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB = 0;
    SNMPX_SOCK_LOWER_MEM_PTR_TYPE  MemPtr = LSA_NULL; /* Pointer to SNMP-Frame memory. */
    LSA_SYS_PTR_TYPE            pSys;
    LSA_USER_ID_TYPE            UserId;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_sock_alloc_rsp_rqb(pHDB: 0x%X, Orginator: 0x%X)",
                            pHDB, Orginator);

    SNMPX_ASSERT_NULL_PTR(pHDB);
    pSys   =  pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* alloc the buffer and attach them to the RQB */
    SNMPX_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, SOCK_SNMP_BUFFER_SIZE, pHDB->Params.pSys);

    if (LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
    {
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                                "OUT: snmpx_sock_alloc_rsp_rqb(pRQB: 0x%X)",pRQB);
        return (pRQB);
    }

    /* alloc. response-RQB...   */
    SNMPX_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);
    if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        SNMPX_SOCK_UDP_DATA_PTR_TYPE   udp_send;

        /* ...set opcode to SOCK_OPC_UDP_SEND,... */
        SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_UDP_SEND);
        SNMPX_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        SNMPX_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);
        SNMPX_SOCK_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(pRQB,SessionIndex);

        /* alloc the buffer and attach them to the RQB */
        /* SNMPX_SOCK_ALLOC_LOWER_MEM(&MemPtr, UserId, SNMPX_UDP_BUFFER_SIZE, pSys); */

        udp_send = &pRQB->args.data;
        SNMPX_ASSERT (udp_send);

        udp_send->data_length   = SOCK_SNMP_BUFFER_SIZE; /* sock need this !! */
        udp_send->buffer_length = SOCK_SNMP_BUFFER_SIZE; /* sock need this !! */

        udp_send->flags     = 0;        /* wait for all data with SOCK_READ_ALL ? */
        udp_send->offset    = 0;        /* ? */
        /* udp_send->timer_val = 0;  */ /* AP01388668: timer references (SOCK) removed */

        udp_send->buffer_ptr = MemPtr;
    }
    else
    {
        LSA_UINT16 RetVal = LSA_RET_OK;

        SNMPX_SOCK_FREE_LOWER_MEM(&RetVal,MemPtr,pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: snmpx_sock_alloc_rsp_rqb(pRQB: 0x%X)",pRQB);
    return (pRQB);
}

/*****************************************************************************/
/* Internal function: snmpx_sock_free_rsp_rqb()                              */
/* free a lower Response-RQB for SOCK                                        */
/*****************************************************************************/
LSA_VOID snmpx_sock_free_rsp_rqb(SNMPX_HDB_SOCK_PTR_TYPE pHDB,
                                 SNMPX_SOCK_LOWER_RQB_PTR_TYPE pRQB)
{
    SNMPX_SOCK_LOWER_MEM_PTR_TYPE  MemPtr;
    LSA_UINT16                   RetVal = LSA_RET_OK;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_sock_free_rsp_rqb(pHDB: 0x%X, pRQB: 0x%X)",
                            pHDB, pRQB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

    /* free allocated buffer */
    MemPtr = pRQB->args.data.buffer_ptr;
    SNMPX_ASSERT (MemPtr);
    if (!LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL))
    {
        SNMPX_SOCK_FREE_LOWER_MEM(&RetVal, MemPtr, pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }
    SNMPX_SOCK_FREE_LOWER_RQB(&RetVal, pRQB, pHDB->Params.pSys);
    SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: snmpx_sock_free_rsp_rqb(RetVal: 0x%X)",RetVal);
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_UserGetFreeRegisterEntry                 +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+  Result                : LSA_UINT16                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free register entry in a User-HDB       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_UserGetFreeRegisterEntry(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser,
    SNMPX_UPPER_REGISTER_PTR_TYPE *pSnmpxRegister)
{
    LSA_UINT16                Status  = SNMPX_OK;
    LSA_BOOL                  Found   = LSA_FALSE;
    SNMPX_UPPER_REGISTER_PTR_TYPE   pRegister;
    LSA_UINT32                i       =   0;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserGetFreeRegisterEntry(pHDBUser: %Xh)",
                            pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);
    SNMPX_ASSERT_NULL_PTR(pSnmpxRegister);

    /* ----------------------------------------------------------------------*/
    /* Search for free handle-management                                     */
    /* ----------------------------------------------------------------------*/

    pRegister  = pHDBUser->RegisteredOids;
    while ( (! Found)  &&  ( i < SNMPX_CFG_MAX_OIDS))
    {
        if ( snmpx_is_null(pRegister->pOid) )
        {
            Found = LSA_TRUE;
        }
        else
        {
            i++;
            pRegister++;
        }
    }

    /* ----------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..              */
    /* ----------------------------------------------------------------------*/

    if ( Found )
    {
        /* register-entry of the MaxOidsCnt */
        *pSnmpxRegister = pRegister;
    }
    else
    {
        *pSnmpxRegister = LSA_NULL;
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_UserGetFreeRegisterEntry(). No more free entries.");
    }

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserGetFreeRegisterEntry(Status: 0x%X, pSnmpxRegister: 0x%X )",
                            Status, pSnmpxRegister);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_IsOidRegistered                       +*/
/*+                             SNMPX_HDB_USER_PTR_TYPE      pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Look for the registered mib tree by one user              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
SNMPX_UPPER_REGISTER_PTR_TYPE SNMPX_GetRegisteredOid(
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser,
    SNMPX_UPPER_OID_PTR_TYPE    pOid,       /* name of var   */
    LSA_UINT                    OidLen      /* number of sub-ids in name */
)
{
    LSA_UINT16                  i;
    SNMPX_UPPER_REGISTER_PTR_TYPE pRegisteredOid = LSA_NULL;
    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetRegisteredOid(pHDBUser: 0x%X, pOid: 0x%X, OidLen: 0x%X)", pHDBUser, pOid, OidLen);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);
    SNMPX_ASSERT_NULL_PTR(pOid);

    for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
    {
        /* is there an oid registered */
        if (pHDBUser->RegisteredOids[i].OidLen != 0 &&
            snmpx_is_not_null (pHDBUser->RegisteredOids[i].pOid))
        {
            /* error, if a smaller object is delivered as registered */
            /* or any other object deliverd as registered */
            if (!(OidLen < pHDBUser->RegisteredOids[i].OidLen ||
                  OidLen > SNMPX_MAX_OID_LEN ||
                  (!SNMPX_OIDCMP(pHDBUser->RegisteredOids[i].pOid, pOid, pHDBUser->RegisteredOids[i].OidLen))))
            {
                /* such oid is registered by this user (subagent) */
                pRegisteredOid = &pHDBUser->RegisteredOids[i];
                SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetRegisteredOid(index: 0x%X)", i);
                break;
            }
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetRegisteredOid(pRegisteredOid: 0x%X)", pRegisteredOid);
    return (pRegisteredOid);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_GetOidsRegistered                     +*/
/*+                             SNMPX_HDB_USER_PTR_TYPE      pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser             : Pointer to UserManagement                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Look for the registered mib tree by one user              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_GetCountOfRegisteredOids(
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser
)
{
    LSA_UINT16      i;
    LSA_UINT16      CountOfRegOids = 0;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_GetCountOfRegisteredOids(pHDBUser: 0x%X)", pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);

    for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
    {
        /* is there an oid registered */
        if (pHDBUser->RegisteredOids[i].OidLen != 0 &&
            snmpx_is_not_null (pHDBUser->RegisteredOids[i].pOid))
        {
            CountOfRegOids++;
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_GetCountOfRegisteredOids(CountOfRegOids: 0x%X)", CountOfRegOids);
    return (CountOfRegOids);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_UserGetActiveSessions                    +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL.pHDBUser   +*/
/*+  Result                : LSA_UINT16                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets number of active sessions in a User-HDB              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_UserGetActiveSessions(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16   i, Sessions;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserGetActiveSessions(pHDBUser: %Xh)",
                            pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);

	for (i=0, Sessions = 0; i < SNMPX_CFG_MAX_MANAGER_SESSIONS; i++) /* for all sessions */
	{
		switch (pHDBUser->MngrSession[i].PortState)
		{
			case SNMPX_HANDLE_STATE_INIT:
			case SNMPX_SNMP_STATE_CLOSE:
			break;
			default:
				Sessions++;
			break;
        }
	}

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserGetActiveSessions(Sessions: 0x%X )",
                            Sessions);
    return(Sessions);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_UserGetFreeSessionEntry                  +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL.pHDBUser   +*/
/*+  Result                : LSA_UINT16                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free session entry in a User-HDB        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_UserGetFreeSessionEntry(
    SNMPX_HDB_USER_PTR_TYPE    pHDBUser,
    SNMPX_COMMON_MEM_U16_PTR_TYPE pSessionIndex)
{
    LSA_UINT16                Status  = SNMPX_OK;
    LSA_BOOL                  Found   = LSA_FALSE;
    SNMPX_HDB_SESSION_PTR_TYPE pSessionEntry;
    LSA_UINT16                i       =   0;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserGetFreeSessionEntry(pHDBUser: %Xh)",
                            pHDBUser);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);
    SNMPX_ASSERT_NULL_PTR(pSessionIndex);

    /* ----------------------------------------------------------------------*/
    /* Search for free handle-management                                     */
    /* ----------------------------------------------------------------------*/

    pSessionEntry = pHDBUser->MngrSession;
    while ( (! Found)  &&  ( i < SNMPX_CFG_MAX_MANAGER_SESSIONS))
    {
        if ( pSessionEntry->PortState == 0 &&
             pSessionEntry->SessionID == 0)
        {
            /* all params must be zero */
            SNMPX_ASSERT(SNMPX_MEMCMP(pSessionEntry->IPAddress, (SNMPX_COMMON_MEM_PTR_TYPE)SNMPX_ZERO_IP, SNMPX_IP_ADDR_SIZE));
            SNMPX_ASSERT(snmpx_is_null(pSessionEntry->pReadCommunity));
            SNMPX_ASSERT(snmpx_is_null(pSessionEntry->pWriteCommunity));
            Found = LSA_TRUE;
        }
        else
        {
            i++;
            pSessionEntry++;
        }
    }

    /* ----------------------------------------------------------------------*/
    /* If a free session management was found we initialize it.              */
    /* ----------------------------------------------------------------------*/

    if ( Found )
    {
        *pSessionIndex = i;
    }
    else
    {
        *pSessionIndex = i;
        *pSessionIndex = 0;
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_UserGetFreeSessionEntry(). No more free entries.");
    }

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserGetFreeSessionEntry(Status: 0x%X, SessionID: 0x%X )",
                            Status, i);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_UserDeleteSessionEntry                   +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL.pHDBUser   +*/
/*+  Result                : LSA_UINT16                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reset session entry in a User-HDB                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_UserDeleteSessionEntry(
    SNMPX_HDB_USER_PTR_TYPE    pHDB,
    LSA_UINT16                 SessionIndex)
{
    LSA_UINT16         Status  = SNMPX_OK;
    LSA_UINT16         RetVal  = LSA_RET_OK;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserDeleteSessionEntry(pHDB: %Xh)",
                            pHDB);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    if (SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS)
    {
        SNMPX_HDB_SESSION_PTR_TYPE pSessionEntry = &pHDB->MngrSession[SessionIndex];

        if (snmpx_is_not_null(pHDB->MngrSession[SessionIndex].pReadCommunity))
        {
            SNMPX_FREE_LOCAL_MEM(&RetVal,pHDB->MngrSession[SessionIndex].pReadCommunity);
            SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
        }

        if (snmpx_is_not_null(pHDB->MngrSession[SessionIndex].pWriteCommunity))
        {
            SNMPX_FREE_LOCAL_MEM(&RetVal,pHDB->MngrSession[SessionIndex].pWriteCommunity);
            SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
        }

        SNMPX_MEMSET(pSessionEntry, 0, sizeof(*pSessionEntry) );
    }
    else
    {
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_UNEXP, "ParamError: SNMPX_UserDeleteSessionEntry(index %u)", SessionIndex);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserDeleteSessionEntry(Status: 0x%X)",
                            Status);
    return(Status);
}

/*****************************************************************************/
/*  end of file SNMPX_LIB.C                                                  */
/*****************************************************************************/

