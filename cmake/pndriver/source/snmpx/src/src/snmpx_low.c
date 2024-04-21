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
/*  F i l e               &F: snmpx_low.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX SOCK-LowerLayer interface functions        */
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

#define LTRC_ACT_MODUL_ID  4
#define SNMPX_MODULE_ID    LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_LOW */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles */
#include "snmpx_int.h"            /* internal header */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_sock_request_lower_done               +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for SOCK-requests.                       +*/
/*+               Fills Error-Structure in NDB with SOCK-Response           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID snmpx_sock_request_lower_done(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDB = SNMPX_GET_HSOCK_PTR();

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN :snmpx_sock_request_lower_done(pRQB: 0x%X)",
                            pRQB);
    SNMPX_ENTER();

    SNMPX_ASSERT_NULL_PTR(pRQB);

    /* SOCK-request always done from a SOCK-Channel. !           */

	SNMPX_ASSERT_NULL_PTR(pHDB);

    switch (SNMPX_SOCK_RQB_GET_OPCODE(pRQB) )
    {
        case SOCK_OPC_OPEN_CHANNEL:
            SNMPX_SOCKOpenChannelDone(pRQB,pHDB);
            break;
        case SOCK_OPC_CLOSE_CHANNEL:
            SNMPX_SOCKCloseChannelDone(pRQB,pHDB);
            break;
        case SOCK_OPC_UDP_OPEN:
            SNMPX_SOCKOpenPortDone(pRQB,pHDB);
            break;
        case SOCK_OPC_CLOSE:
            SNMPX_SOCKClosePortDone(pRQB,pHDB);
            break;
        case SOCK_OPC_UDP_RECEIVE:
            SNMPX_SOCKRecvDone(pRQB,pHDB);
            break;
        case SOCK_OPC_UDP_SEND:
            SNMPX_SOCKSendDone(pRQB,pHDB);
            break;
        default:
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    SNMPX_EXIT();

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: snmpx_sock_request_lower_done()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKOpenChannel                       +*/
/*+  Input                      LSA_HANDLE_TYPE       SNMPXHandle           +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXHandle           : SNMPX SOCK-Channel handle                      +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : SNMPX_OK                                        +*/
/*+                         SNMPX_ERR_RESOURCE                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to SOCK. The Request will be  +*/
/*+               finished by calling the SOCK-Callbackfunction.            +*/
/*+               snmpx_sock_request_lower_done() if SNMPX_OK.              +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_SOCKOpenChannel(
    LSA_HANDLE_TYPE       SNMPXHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKOpenChannel(SNMPXHandle: 0x%X, SysPath: 0x%X)",
                            SNMPXHandle, SysPath);
    Result = SNMPX_OK;

    UserId.uvar32 = 0;
    SNMPX_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_OPEN_CHANNEL);

        pRQB->args.channel.handle_upper  = SNMPXHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.sock_request_upper_done_ptr = snmpx_sock_request_lower_done;

        SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",SNMPXHandle,pRQB);
        SNMPX_SOCK_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory failed!");
        Result = SNMPX_ERR_RESOURCE;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKOpenChannel. Result: 0x%X",
                            Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKOpenChannelDone                   +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE   pRQB        +*/
/*+                             SNMPX_HDB_SOCK_PTR_TYPE         pHDB        +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Open Channel done handling.                               +*/
/*+                                                                         +*/
/*+               - save SOCK-Channel                                       +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKOpenChannelDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response = SNMPX_OK;
    LSA_UINT16               RetVal = LSA_RET_OK;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB,pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

    SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));

    if ( SNMPX_SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK )
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Open Channel successful");
        /* save SOCK-Channel in HDB */
        pHDB->SockHandle = pRQB->args.channel.handle;
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"SOCK-Open Channel failed.[0x%X]",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
        Response = SNMPX_ERR_LL;
    }

    SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

    SNMPX_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKOpenChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKCloseChannel                      +*/
/*+  Input                      LSA_HANDLE_TYPE       SOCKHandle            +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             SNMPX_ERR_RESOURCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SOCKHandle           : SOCK handle to close                            +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to SOCK. The Request will be +*/
/*+               finished by calling the SOCK-Callbackfunction.            +*/
/*+               snmpx_sock_request_lower_done()                           +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_SOCKCloseChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PTR_TYPE      pSys)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKCloseChannel(SOCKHandle: 0x%X)",
                            SOCKHandle);
    Result = SNMPX_OK;

    UserId.uvar32 = 0;
    SNMPX_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_CLOSE_CHANNEL);
        SNMPX_SOCK_RQB_SET_HANDLE(pRQB,SOCKHandle);

        SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_CLOSE_CHANNEL (SOCKHandle: 0x%X,pRQB: 0x%X)",SOCKHandle,pRQB);
        SNMPX_SOCK_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating RQB memory failed!");
        Result = SNMPX_ERR_RESOURCE;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKCloseChannel(Result: 0x%X)", Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKCloseChannelDone                  +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                             SNMPX_HDB_SOCK_PTR_TYPE        pHDB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Close Channel done handling.                              +*/
/*+                                                                         +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKCloseChannelDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response = SNMPX_OK;
    LSA_UINT16               RetVal = LSA_RET_OK;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKCloseChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB,pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

    SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));

    if ( SNMPX_SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK )
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Close Channel successful");
    }
    else
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"SOCK-Close Channel failed.[0x%X]",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
        Response = SNMPX_ERR_LL;
    }

    /* is there a stored receive-rqb */
    while (pHDB->SockRQBs.pBottom)
    {
        SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQBHelp;

        SNMPX_RQB_REM_BLOCK_BOTTOM(pHDB->SockRQBs.pBottom,
                                   pHDB->SockRQBs.pTop,
                                   pRQBHelp);

        /* free the receive RQB */
        SNMPX_SOCK_FREE_LOWER_MEM(&RetVal,pRQBHelp->args.data.buffer_ptr,pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

        SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQBHelp,pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }

    /* are receive resources pending ? */
    SNMPX_ASSERT(pHDB->RxPendingCnt == 0);

    SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

#ifdef SNMPX_CFG_SNMP_AGENT
    SNMPX_DeleteObjects();
#endif

    SNMPX_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKCloseChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKChannelDone                       +*/
/*+  Input                      SNMPX_HDB_SOCK_PTR_TYPE         pHDB        +*/
/*+                             LSA_UINT16                   Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  Response   : Responsecode of actual action taken (depends on state)    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handler for Open and Close-Channel requests to SNMPX for  +*/
/*+               SOCK-Channels. This function is called after a lower      +*/
/*+               layer called back the SNMPX (e.g. after SOCK finished his +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKChannelDone(
    SNMPX_HDB_SOCK_PTR_TYPE       pHDB,
    LSA_UINT16                 Response)
{

    SNMPX_UPPER_RQB_PTR_TYPE     pRQB;
    LSA_UINT16                   PathStat = LSA_RET_OK;
    LSA_BOOL                     Finish   = LSA_FALSE; /* dont finish request by default */
    LSA_BOOL                     Release  = LSA_FALSE; /* dont release PATH-Info by default */
    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE             pSys;
    SNMPX_DETAIL_PTR_TYPE        pDetail;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKChannelDone(pHDB: 0x%X, Response: 0x%X)",
                            pHDB,Response);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    pRQB    = pHDB->pOCReq;
    Cbf     = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
	pSys    = pHDB->Params.pSys;
	pDetail = pHDB->Params.pDetail;

    SNMPX_ASSERT_NULL_PTR(pRQB);

    switch (pHDB->SockState)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Open Channel request.                        */
            /* Provide Recv requests.                                            */
            /*-------------------------------------------------------------------*/
        case SNMPX_HANDLE_STATE_OPEN_SOCK_RUNNING:
            if ( Response == SNMPX_OK)
            {
                /* SNMPX_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail; */

#ifdef SNMPX_CFG_SNMP_AGENT
                /* Open the Port 161 for SNMP                       */
                pHDB->SockState  = SNMPX_HANDLE_STATE_OPEN_PORT_RUNNING;

                Response = SNMPX_SOCKOpenPort(pHDB, SNMPX_HANDLE_TYPE_SOCK, pHDB->IPAddress, 0xFFFF);

                if ( Response != SNMPX_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/

                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing SockOpenPort-Request failed.");

                    pHDB->SockState  = SNMPX_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING;

                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Closing Channel because of error.");

                    if ( SNMPX_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys) != SNMPX_OK)
                    {
                        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        SNMPX_FATAL(SNMPX_FATAL_ERR_SOCK);
                    }
                }
#else
                /* only snmp manager: no more actions needed. Channel is open now. */
                pHDB->SockState  = SNMPX_HANDLE_STATE_OPENED;
                pHDB->AgntPortState  = SNMPX_SNMP_STATE_CLOSE;
                pHDB->RxPendingCnt = 0; /* no Rx pending within SOCK */
                Finish = LSA_TRUE;
#endif
            }
            else
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a OPEN-PORT SOCK-request.                           */
            /*-------------------------------------------------------------------*/
        case SNMPX_HANDLE_STATE_OPEN_PORT_RUNNING:
            if ( Response == SNMPX_OK)
            {
                /* NOTE: Providing Recv-Indication Request must be          */
                /*       the last action before going into State "open".    */
                /*       So only Indciations while in Open-state must be    */
                /*       handled and reprovided!                            */

                /* provide SOCK-Recv-Requests */
                Response = SNMPX_SOCKRecvProvide(pHDB, pDetail->Params.Sock.NumOfRecv);

                if ( Response == SNMPX_OK )
                {
                    /* no more actions needed. Channel is open now. */
                    pHDB->SockState  = SNMPX_HANDLE_STATE_OPENED;
                    pHDB->AgntPortState  = SNMPX_SNMP_STATE_OPEN; /* snmp port is open */
                    Finish = LSA_TRUE;
                }
                else
                {
                    /* Any Recv-Request failed. Cancel Recv-Requests (by CloseChannel) */
                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Receive Provide request failed.");

                    pHDB->OpenResponse = Response; /* save for later */

                    pHDB->SockState = SNMPX_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING;

                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Closing Channel because of error");

                    if ( SNMPX_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys) != SNMPX_OK)
                    {
                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel().");
                        SNMPX_FATAL(SNMPX_FATAL_ERR_SOCK);
                    }
                }
            }
            else
            {
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK OpenPort request failed.");

                pHDB->OpenResponse = Response; /* save for later */

                pHDB->SockState  = SNMPX_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING;

                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-Closing Channel because of error");

                if ( SNMPX_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys) != SNMPX_OK)
                {
                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel().");
                    SNMPX_FATAL(SNMPX_FATAL_ERR_SOCK);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Close-Channel because of error within Open   */
            /*-------------------------------------------------------------------*/
        case SNMPX_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING:
            if ( Response == SNMPX_OK)
            {
                Response = pHDB->OpenResponse;  /* get Error-Response  */

                /* release PATH-Info ! */
				Release = LSA_TRUE;
            }
            else
            {
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel().");
                SNMPX_FATAL(SNMPX_FATAL_ERR_SOCK);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a SOCK-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case SNMPX_HANDLE_STATE_CLOSE_SOCK_RUNNING:

            if ( Response == SNMPX_OK)
            {
                /* release PATH-Info !                          */
				Release = LSA_TRUE;
            }
            else
            {
                /* is this a fatal error ?                      */
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCK Channel().");
            }
            Finish = LSA_TRUE;
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a CLOSE-PORT SOCK-request.                          */
            /*-------------------------------------------------------------------*/
        case SNMPX_HANDLE_STATE_CLOSE_PORT_RUNNING:

            if ( Response == SNMPX_OK)
            {
                Response = SNMPX_SOCKCloseChannel(pHDB->SockHandle,pHDB->Params.pSys);
                if ( Response == SNMPX_OK )
                {
                    /* Request is underway now.. */
                    pHDB->SockState  = SNMPX_HANDLE_STATE_CLOSE_SOCK_RUNNING;
                }
            }
            else
            {
                /* is this a fatal error ?                      */
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCKET().");
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/
        default:
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */

    if ( Release )  /* release the PATH-Info ? */
    {
        /* we release the HDB  */
        SNMPX_SOCKReleaseHDB(pHDB);
	}

    if ( Finish )  /* finish the request ? */
    {
        pHDB->pOCReq = LSA_NULL;  /* to signal no more request pending.      */

        SNMPX_RQBSetResponse(pRQB,Response);
        /* upper - handler already set */
        SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),SNMPX_RQB_GET_HANDLE(pRQB),Response);
        SNMPX_CallCbf(Cbf,pRQB,pSys);
    }

    if ( Release )  /* release the PATH-Info ? */
    {
        SNMPX_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

        if (PathStat != LSA_RET_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_RELEASE_PATH_INFO);
	}

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKChannelDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKRecvProvide                       +*/
/*+  Input/Output          :    SNMPX_HDB_PTR_TYPE           pHDB           +*/
/*+                             LSA_UINT32                   Cnt            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  Cnt                  : Number of Recv-Request to send to SOCK          +*/
/*+                                                                         +*/
/*+  Result               : SNMPX_OK                                        +*/
/*+                         SNMPX_ERR_RESOURCE                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends Indication-Resources to SOCK                        +*/
/*+               (SOCK_OPC_SNMP_RECEIVE_REQ)                               +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Requests                      +*/
/*+                                                                         +*/
/*+               Note: On error there still maybe RECV-Request pending     +*/
/*+                     within SOCK!                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_SOCKRecvProvide(
    SNMPX_HDB_SOCK_PTR_TYPE    pHDB,
    LSA_UINT32               Cnt)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE               pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                            pSys;
    LSA_UINT16                                  Status;
    LSA_USER_ID_TYPE                            UserId;
    LSA_UINT16                                  RetVal = LSA_RET_OK;
    SNMPX_SOCK_LOWER_MEM_PTR_TYPE               pRcv = LSA_NULL; /* Pointer for Frame memory. */
    SNMPX_SOCK_UDP_DATA_PTR_TYPE                udp_recv;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKRecvProvide(pHDB: 0x%X, Cnt: 0x%X)",
                            pHDB,Cnt);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    Status = SNMPX_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /*---------------------------------------------------------------------------*/
    /* allocate and send Receive-Request to SOCK                                 */
    /*---------------------------------------------------------------------------*/

    pHDB->SockRQBCnt = 0; /* no Rx pending within SOCK */

    while ((Status == SNMPX_OK) && (Cnt))
    {
        SNMPX_SOCK_ALLOC_LOWER_MEM(&pRcv,
                                   UserId,
                                   SOCK_SNMP_BUFFER_SIZE,
                                   pSys);

        if ( LSA_HOST_PTR_ARE_EQUAL(pRcv, LSA_NULL) )
        {
            Status = SNMPX_ERR_RESOURCE;
            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SNMPX_SOCKRecvProvide(). Allocation receive memory failed.");
        }
        else
        {
            /* allocate RQB */
            SNMPX_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
            {
                /* provide recv resources */
                SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_UDP_RECEIVE);
                SNMPX_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);

                SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_SNMP_RECEIVE_REQ (Handle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pRQB);

                udp_recv = &pRQB->args.data;
                SNMPX_ASSERT (udp_recv);

                udp_recv->buffer_ptr = pRcv;

                SNMPX_DoRecvProvide(pHDB, pRQB);
            }
            else
            {
                Status = SNMPX_ERR_RESOURCE;
                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
                SNMPX_SOCK_FREE_LOWER_MEM(&RetVal,pRcv,pSys);
                SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
            }
        }
        Cnt--;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKRecvProvide(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKRecvDone                          +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                             SNMPX_SOCK_HDB_PTR_TYPE        pHDB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: SOCK Recv-Request done (Indication received)              +*/
/*+                                                                         +*/
/*+               Copy received frame infos and content to the DataBase     +*/
/*+               within global management (Frame) for further handling.    +*/
/*+                                                                         +*/
/*+               Calls further handling                                    +*/
/*+                                                                         +*/
/*+               Reprovides Receveive-Request to SOCK                      +*/
/*+                                                                         +*/
/*+               If Recv-Request was canceled we free the RQB/Frame and    +*/
/*+               dont reprovide.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKRecvDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
#ifdef SNMPX_CFG_SNMP_AGENT
    LSA_BOOL                    Reprovide = LSA_TRUE;
#endif
    LSA_UINT16                  Orginator;
    LSA_UINT16                  SessionIndex;
    LSA_UINT16                  Response;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCK_RecvDone(pRQB: 0x%X,pHDB: 0x%X)",
                            pRQB,pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

    Orginator    = SNMPX_SOCK_RQB_GET_USERID_UVAR16(pRQB);
    SessionIndex = SNMPX_SOCK_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(pRQB);

    switch (SNMPX_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response = SNMPX_OK;
            break;
        case SOCK_RSP_OK_WITHDRAW:
		case SOCK_RSP_ERR_LOC_ABORT:
            Response = SNMPX_RSP_ERR_LOC_ABORT;
            break;
        case SOCK_RSP_ERR_TIMEOUT:
            Response = SNMPX_ERR_TIMEOUT;
            break;
		case SOCK_RSP_ERR_REM_ABORT:  /* (0xc3) remote connection abort */
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE,"SOCK-ERROR: SOCK_OPC_UDP_RECEIVE failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
            Response  = SNMPX_ERR_REM_ABORT;
            break;
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SOCK-ERROR: SOCK_OPC_UDP_RECEIVE failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = SNMPX_ERR_LL;
            break;
    }

    switch (SNMPX_HANDLE_GET_TYPE(Orginator))
    {
#ifdef SNMPX_CFG_SNMP_MANAGER
        case SNMPX_HANDLE_TYPE_USER:
        {
            SNMPX_HDB_USER_PTR_TYPE pHDBUser = SNMPX_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);
            SNMPX_UPPER_RQB_PTR_TYPE pRQBUser;

            /* get the corresponding user HDB and store the socket */
            SNMPX_ASSERT_NULL_PTR(pHDBUser);
            SNMPX_ASSERT(SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS);

            pRQBUser = pHDBUser->MngrSession[SessionIndex].pUserRQB;

            SNMPX_ASSERT(pHDBUser->MngrSession[SessionIndex].RxPendingCnt == 1); /* exactly one resource for each request */
            pHDBUser->MngrSession[SessionIndex].RxPendingCnt--; /* number of Rx-Request pending within SOCK */

			/* check the incoming ip address, we have a bind to '0.0.0.0' (pSession->IPAddress) */
			if (snmpx_is_not_null(pRQBUser) && Response == SNMPX_OK)
			{
				SNMPX_ASSERT(pRQB->args.data.sock_fd == pHDBUser->MngrSession[SessionIndex].SockFD);

				if (!SNMPX_MEMCMP(pHDBUser->MngrSession[SessionIndex].IPAddress,
								  &pRQB->args.data.rem_addr.sin_addr.S_un.S_un_b,
								  SNMPX_IP_ADDR_SIZE))
				{
					Response = SNMPX_ERR_FAILED;
					SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_UNEXP, "IP address (0x%X) of the session is not the received one: SNMPX_SOCKRecvDone()", pRQB->args.data.rem_addr.sin_addr.s_addr);
				}
			}

			if (snmpx_is_not_null(pRQBUser) && Response == SNMPX_OK)
			{
				if (SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_REQUEST_MANAGER) /* could be also a CloseSession */
				{
					Response = SNMPX_ParseManager(pRQB, pRQBUser, (LSA_INT32)pHDBUser->MngrSession[SessionIndex].RequestID);
				}
			}

			snmpx_sock_free_rsp_rqb(pHDB, pRQB);

			if (snmpx_is_null(pRQBUser)) /* no snmp request active -> ignore */
			{
				SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_NOTE, "SNMPX_SOCKRecvDone(Index=%u): SOCK_UDP_RECV.CNF ignored (Response: 0x%X, PortState: 0x%X)!", SessionIndex, Response, pHDBUser->MngrSession[SessionIndex].PortState);
				SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKRecvDone()");
                return;
			}

            /* SNMPX_SOCKManagerRequestHandler(pHDB, LSA_NULL, Response); */ /* further actions taken in request handler */
            {
                switch (pHDBUser->MngrSession[SessionIndex].PortState)
                {
                    case SNMPX_SNMP_STATE_OPEN:	/* send.cnf is done */
                        /**************** SNMPX_OPC_CLOSE_MANAGER_SESSION ****************/
                        if (SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_CLOSE_MANAGER_SESSION) /* close the port now */
                        {
                            pHDBUser->MngrSession[SessionIndex].RetryCnt = 0;   /* stop the retries */

                            Response = SNMPX_SOCKClosePort(pHDB, pHDBUser->ThisHandle, pHDBUser->MngrSession[SessionIndex].SockFD, SessionIndex);
                            if (Response != SNMPX_OK)
                            {
                                pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
                                SNMPX_UserRequestFinish(pHDBUser,pRQBUser,SNMPX_ERR_FAILED);
                                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing Manager close port failed.");
                            }
                            else
                            {
                                pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_WF_CLOSE;
                            }
                            break;
                        }

                        /**************** SNMPX_OPC_REQUEST_MANAGER ****************/
                        SNMPX_ASSERT(SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_REQUEST_MANAGER);
                        if (Response == SNMPX_OK || pHDBUser->MngrSession[SessionIndex].RetryCnt == 0)  /* confirm the user */
                        {
                            pHDBUser->MngrSession[SessionIndex].RetryCnt = 0;
                            pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
                            SNMPX_UserRequestFinish(pHDBUser,pRQBUser,Response);
                        }
                        break;
                    case SNMPX_SNMP_STATE_WF_CNF:   /* send request is pending */
                        /* store the response in the user RQB and wait for send.cnf */
                        if (Response == SNMPX_OK || pHDBUser->MngrSession[SessionIndex].RetryCnt == 0)
                        {
                            pHDBUser->MngrSession[SessionIndex].RetryCnt = 0;
                            SNMPX_RQBSetResponse(pRQBUser,Response);
                        }
						SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_NOTE, "SNMPX_SOCKRecvDone(Index=%u): SOCK_UDP_SEND.CNF is outstanding (Response: 0x%X)!", SessionIndex, Response);
						SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKRecvDone()");
                        return;
                    case SNMPX_SNMP_STATE_WF_CLOSE:
                    case SNMPX_SNMP_STATE_CLOSE:
						SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_NOTE, "SNMPX_SOCKRecvDone(Index=%u): SOCK_UDP_RECV.CNF ignored (Response: 0x%X)!", SessionIndex, Response);
						SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKRecvDone()");
                        return;
                    case SNMPX_HANDLE_STATE_INIT:
                    case SNMPX_SNMP_STATE_WF_OPEN:
                    default:
                        SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SOCK_UDP_SEND.CNF failed (PortState: 0x%X).",pHDBUser->MngrSession[SessionIndex].PortState);
                        SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                        break;
                }

                if (pHDBUser->MngrSession[SessionIndex].RetryCnt != 0)
                {
                    Response = SNMPX_DoRequestManager(pRQBUser, pHDBUser);
                    if (Response != SNMPX_OK)   /* stop the retries */
                    {
                         pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
                         SNMPX_UserRequestFinish(pHDBUser,pRQBUser,Response);
                    }
                }
            }
        }
        break;
#endif
#ifdef SNMPX_CFG_SNMP_AGENT
        case SNMPX_HANDLE_TYPE_SOCK:    /* SNMP agent */
            pHDB->SockRQBCnt--;  /* number of RQBs pending within SOCK */
            pHDB->RxPendingCnt--; /* number of Recv-Request pending within SOCK */
            break;
#endif
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Unknown Orginator (0x%X)!", Orginator);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }


#ifdef SNMPX_CFG_SNMP_AGENT
    /* if we are in OPEN-State we handle the Request. If not we are not open and */
    /* we discard the frame and dont reprovide it. This is the case if we are    */
    /* just closing the handle because of error while opening, or regular closing*/
    /* Note: It is possible that we get a valid frame while closing, so CANCEL   */
    /*       may not be enough to catch all rqbs. We have to check the State too */
    /*       to be sure we dont reprovide while closing!                         */
    /* Note: It is important, that the Recv-Request will be provided in the last */
    /*       State while opening the channel.                                    */

    if ( pHDB->SockState == SNMPX_HANDLE_STATE_OPENED && pHDB->AgntPortState == SNMPX_SNMP_STATE_OPEN)
    {
        SNMPX_SOCK_LOWER_RQB_PTR_TYPE pBottom = pHDB->SockRQBs.pBottom;

        switch (SNMPX_SOCK_RQB_GET_RESPONSE(pRQB))
        {
            case SOCK_RSP_OK:
                /* we queue the request in the SOCK-HDB */
                /* if the queue is empty, we execute the "request" */
                SNMPX_RQB_PUT_BLOCK_TOP(pHDB->SockRQBs.pBottom, pHDB->SockRQBs.pTop, pRQB);

                /* queue is empty */
                if (snmpx_is_null(pBottom))
                {
                    /* further actions taken in request handler */
                    SNMPX_SOCKRequestHandler(pHDB, LSA_NULL, SNMPX_OK);
                }

                SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKRecvDone()");
                return;

            case SOCK_RSP_OK_WITHDRAW:
                /* we canceled the request (e.g. on channel close ) */
                /* Note: typically we dont get here, because the    */
                /*       channel state is <> SNMPX_HANDLE_STATE_OPENED  */
                /*       while closing!                             */

                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE,"SOCK Receive canceled (0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
                Reprovide = LSA_FALSE;
                break;

            default:
                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SOCK_OPC_UDP_RECEIVE failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
                break;
        }
    }
    else
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE,"SOCK Receive while not in open-state. discard and dont reprovide().");
        Reprovide = LSA_FALSE;
    }

    if (Reprovide)
    {
        SNMPX_DoRecvProvide(pHDB, pRQB);    /* provide receive resource */
    }
    else
    {
        LSA_UINT16 RetVal;

        /* free the receive RQB */
        SNMPX_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.data.buffer_ptr,pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

        SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
        SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
    }
#endif

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKRecvDone()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKUserCheckPending                  +*/
/*+  Input/Output          :    SNMPX_HDB_SOCK_PTR_TYPE    pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to SOCK-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_AGENT
LSA_VOID SNMPX_SOCKUserCheckPending(
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock,
    LSA_UINT16              Response)
{
    SNMPX_HDB_USER_PTR_TYPE     pHDBUser;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKUserCheckPending(pHDBSock: 0x%X, Response: 0x%X)",
                            pHDBSock, Response);

    SNMPX_ASSERT_NULL_PTR(pHDBSock);

    if ( pHDBSock->UserRQBCnt ) /* User-RCTRL-Blocks pending ? */
    {
        SNMPX_UPPER_RQB_PTR_TYPE  pRQBUser;
        LSA_HANDLE_TYPE         Handle;

        SNMPX_RQB_REM_BLOCK_BOTTOM(pHDBSock->UserRQBs.pBottom,
                                   pHDBSock->UserRQBs.pTop,
                                   pRQBUser);

        /* because PendRCtrlCnt was >0 we dont expect LSA_NULL here */
        SNMPX_ASSERT_NULL_PTR(pRQBUser);
        pHDBSock->UserRQBCnt--;

        /* control = &pRQBUser->args.SockControl; */
        Handle = SNMPX_RQB_GET_HANDLE(pRQBUser);
        pHDBUser = SNMPX_GetUserHDBFromHandle(Handle);
        SNMPX_UserRequestFinish(pHDBUser,pRQBUser,Response);
    }
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKUserCheckPending()");
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKRequestHandler                    +*/
/*+                        :    SNMPX_HDB_SOCK_PTR_TYPE    pHDB             +*/
/*+                        :    LSA_UINT16                 Response         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQBUser   : Pointer to User-RCTRL-Block                               +*/
/*+  Response   : Response of last action. if <> SNMPX_OK the Request       +*/
/*+               will be finished with this error. Set to SNMPX_OK if      +*/
/*+               not error present (e.g. on first call)                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Statemaching for SOCK-User-Requests (ControlMasterAgent). +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_AGENT
LSA_VOID SNMPX_SOCKRequestHandler(
    SNMPX_HDB_SOCK_PTR_TYPE         pHDBSock,
    SNMPX_UPPER_RQB_PTR_TYPE        pRQBUser,
    LSA_UINT16                      Response)
{
    LSA_BOOL                  Ready = LSA_FALSE;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SockRequestHandler(pHDBSock: 0x%X, pRQBUser: 0x%X, Response: 0x%X)",
                            pHDBSock, pRQBUser, Response);

    /* -----------------------------------------------------------------------*/
    /* forward in statemachine till an asynchroneous request is underway which*/
    /* we have to wait for.                                                   */
    /* -----------------------------------------------------------------------*/
    while (! Ready)
    {
        Ready = LSA_TRUE;      /* Default: break loop because request underway */

        switch (pHDBSock->AgntPortState)
        {
            case SNMPX_SNMP_STATE_CLOSE:   /* initial, after OpenChannel: Agent is in Deact */
                SNMPX_PROGRAM_TRACE_02(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_STOP (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                /* comes from the user ? */
                SNMPX_ASSERT_NULL_PTR(pRQBUser);
                {
                    /* Activate the SNMP-Agent   */
                    LSA_UINT16 RetVal = SNMPX_SOCKOpenPort(pHDBSock, SNMPX_HANDLE_TYPE_SOCK, pHDBSock->IPAddress, 0xFFFF);
                    if ( RetVal != SNMPX_OK)
                    {
                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing SockAgentActivate-Request failed.");
                        /* check for pending user-requests and confirm  */
                        SNMPX_SOCKUserCheckPending(pHDBSock, RetVal);
                    }
                    else
                    {
                        pHDBSock->AgntPortState = SNMPX_SNMP_STATE_WF_OPEN;
                    }
                    break;
                }

                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;

            case SNMPX_SNMP_STATE_WF_OPEN:   /* ControlStart.Req is active    */
                SNMPX_PROGRAM_TRACE_02(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_WF_START (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                if (snmpx_is_null(pRQBUser))
                {
                    /* is there an error during Control-Start (UdpOpen) ? */
                    if (Response != SNMPX_OK)
                    {
                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK SockAgentActivate request failed.");

                        /* on error, we copy the LL-Error to the RQB    */
                        if (pRQBUser) SNMPXRQBCopyLLErrorRQB(pRQBUser);

                        pHDBSock->AgntPortState = SNMPX_SNMP_STATE_CLOSE;
                    }
                    else
                    {
                        /* NOTE: Providing Recv-Indication Request must be          */
                        /*       the last action before going into State "open".    */
                        /*       So only Indciations while in Open-state must be    */
                        /*       handled and reprovided!                            */

                        SNMPX_DETAIL_PTR_TYPE pDetail = pHDBSock->Params.pDetail;

                        if (!pHDBSock->RxPendingCnt) /* no Recv-Requests pending within SOCK (first Activate-Call ?) */
                        {
                            /* provide SOCK-Recv-Requests */
                            Response = SNMPX_SOCKRecvProvide(pHDBSock, pDetail->Params.Sock.NumOfRecv);

                            if ( Response != SNMPX_OK )
                            {
                                /* Any Recv-Request failed. Cancel Recv-Requests (by DeactAgent) */
                                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"Any SOCK Receive Provide request failed.");

                                /* on error, we copy the LL-Error to the RQB    */
                                if (pRQBUser) SNMPXRQBCopyLLErrorRQB(pRQBUser);
                            }
                        }

                        /* no more actions needed. Agent is started now. */
                        pHDBSock->AgntPortState = SNMPX_SNMP_STATE_OPEN; /* initialize */
                    }

                    /* confirm to the user, if exist */
                    /* check for pending user-requests   */
                    SNMPX_SOCKUserCheckPending(pHDBSock, Response);
                    break;
                }

                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;

            case SNMPX_SNMP_STATE_WF_CLOSE:     /* close port is active   */
                SNMPX_PROGRAM_TRACE_02(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_WF_STOP (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                if (snmpx_is_null(pRQBUser))
                {
                    /* is there an error during DEACTIVATE_AGENT ? */
                    if (Response != SNMPX_OK)
                    {
                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK SockAgentDeactivate request failed.");
                        /* on error, we copy the LL-Error to the RQB    */
                        if (pRQBUser) SNMPXRQBCopyLLErrorRQB(pRQBUser);

                        pHDBSock->AgntPortState = SNMPX_SNMP_STATE_OPEN;
                    }
                    else
                    {
                        /* is there a stored receive-rqb */
                        while (pHDBSock->SockRQBs.pBottom)
                        {
                            SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB;
                            LSA_UINT16 RetVal;

                            SNMPX_RQB_REM_BLOCK_BOTTOM(pHDBSock->SockRQBs.pBottom,
                                                       pHDBSock->SockRQBs.pTop,
                                                       pRQB);

                            /* free the receive RQB */
                            SNMPX_SOCK_FREE_LOWER_MEM(&RetVal,pRQB->args.data.buffer_ptr,pHDBSock->Params.pSys);
                            SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

                            SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDBSock->Params.pSys);
                            SNMPX_MEM_FREE_FAIL_CHECK(RetVal);
                        }

                        pHDBSock->AgntPortState = SNMPX_SNMP_STATE_CLOSE;
                    }
                    /* confirm to the user, if exist */
                    /* check for pending user-requests   */
                    SNMPX_SOCKUserCheckPending(pHDBSock, Response);
                    break;
                }

                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;

            case SNMPX_SNMP_STATE_OPEN:      /* SNMP-agent is active     */
                SNMPX_PROGRAM_TRACE_02(LSA_TRACE_LEVEL_NOTE_LOW,"SOCK-SM [%X]: STATE_START (Resp: %Xh)",pHDBSock->ThisHandle,Response);

                if (snmpx_is_not_null(pRQBUser))
                {
                    LSA_UINT16 RetVal;
                    LSA_HANDLE_TYPE         Handle   = SNMPX_RQB_GET_HANDLE(pRQBUser);
                    SNMPX_HDB_USER_PTR_TYPE pHDBUser = SNMPX_GetUserHDBFromHandle(Handle);

                    SNMPX_ASSERT_NULL_PTR(pHDBUser);

                    RetVal = SNMPX_SOCKClosePort(pHDBSock, pHDBUser->ThisHandle, pHDBSock->SockFD, 0xFFFF);
                    if (RetVal != SNMPX_OK)
                    {                           /* confirm to the user, if exist */
                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing SockAgentDeactivate-Request failed.");
                        /* check for pending user-requests   */
                        SNMPX_SOCKUserCheckPending(pHDBSock, Response);
                    }
                    else
                    {
                        pHDBSock->AgntPortState = SNMPX_SNMP_STATE_WF_CLOSE;
                    }
                    break;
                }
                else    /* call from the oid-switcher */
                {
                    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB;

                    SNMPX_RQB_REM_BLOCK_BOTTOM(pHDBSock->SockRQBs.pBottom,
                                               pHDBSock->SockRQBs.pTop,
                                               pRQB);
                    /* queue is not empty */
                    if (snmpx_is_not_null(pRQB))
                    {
                        LSA_UINT16 RetVal = SNMPX_ParseAgent(pRQB, pHDBSock);  /* call the parser */
                        if (RetVal != SNMPX_OK)             /* the oid switcher works */
                        {
                            Ready = LSA_FALSE; /* could not send, look for next request */
                        }
                        break;
                    }
                    else
                    {
                        break;
                    }
                }

                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;

            default:
                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_SockRequestHandler(): Invalid RState (%Xh)",pHDBSock->AgntPortState);
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SockRequestHandler(), state %Xh", pHDBSock->AgntPortState);
}
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKOpenPort                          +*/
/*+  Input/Output          :    SNMPX_HDB_PTR_TYPE             pHDB         +*/
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
/*+  Description: Open port 161 for SNMP                                    +*/
/*+               (SOCK_OPC_SNMP_AGENT_ACTIVATE)                            +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Requests                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_SOCKOpenPort(
    SNMPX_HDB_SOCK_PTR_TYPE    pHDB,
    LSA_UINT16                 Orginator,
    SNMPX_IP_TYPE              IPAddress,
    LSA_UINT16                 SessionIndex)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE           pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;
    SNMPX_SOCK_UDP_OPEN_PTR_TYPE            sock_open;

    SNMPX_FUNCTION_TRACE_03(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKOpenPort(pHDB: 0x%X, Orginator: 0x%X, SessionIndex: 0x%X, ",
                            pHDB, Orginator, SessionIndex);

    SNMPX_ASSERT_NULL_PTR(IPAddress);

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "                        IP-Address:%u.%u.%u.%u)",
                            IPAddress[0], IPAddress[1], IPAddress[2], IPAddress[3]);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    Status = SNMPX_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    SNMPX_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_UDP_OPEN);
        SNMPX_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        SNMPX_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);
        SNMPX_SOCK_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(pRQB,SessionIndex);

        SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_UDP_OPEN (Handle: 0x%X,SOCKHandle: 0x%X)",pHDB->ThisHandle,pRQB);

        sock_open = &pRQB->args.udp_open;
        sock_open->loc_addr.sin_family = SOCK_AF_INET;
        /* sock_open->loc_addr.sin_addr.S_un.S_addr = 0; */   /* agent */
        SNMPX_MEMCPY(&sock_open->loc_addr.sin_addr.S_un.S_un_b, IPAddress, SNMPX_IP_ADDR_SIZE);

		/* HANDLE_TYPE_SOCK means snmp-agent */
        if (SNMPX_HANDLE_IS_SOCK(Orginator))
        {
            LSA_UINT16 SwapValue = SNMPX_SNMP_PORT;
            sock_open->loc_addr.sin_port = SNMPX_SWAP16(SwapValue);
        }
        else
        {
            sock_open->loc_addr.sin_port = 0;   /* the UDP stack assigns the port number */
        }
	    /* sock_open->timer_on = LSA_FALSE; */ /* AP01388668: timer references (SOCK) removed */

        pHDB->SockRQBCnt++;
        SNMPX_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKOpenPort(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKOpenPortDone                      +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                             SNMPX_HDB_PTR_TYPE             pHDB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Open Port 161 done handling                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKOpenPortDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16                      Response;
    LSA_UINT16                      RetVal = LSA_RET_OK;
    SNMPX_SOCK_UDP_OPEN_PTR_TYPE    sock_open;
    LSA_UINT16                      Orginator;
    LSA_INT32                       SockFD = -1;         /* lower-layer send sock    */
    LSA_UINT16                      SessionIndex;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKOpenPortDone(pRQB: 0x%X,pHDB: 0x%X)",
                            pRQB,pHDB);

	SNMPX_ASSERT_NULL_PTR(pHDB);
	SNMPX_ASSERT_NULL_PTR(pRQB);

    SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_UDP_OPEN done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));

    Orginator  = SNMPX_SOCK_RQB_GET_USERID_UVAR16(pRQB);

    switch (SNMPX_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            sock_open = &pRQB->args.udp_open;
            SNMPX_ASSERT_NULL_PTR (sock_open);
            Response  = SNMPX_OK;

            SockFD = sock_open->sock_fd;       /* lower-layer send sock  */
            /* pHDB->LocalPort = sock_open->loc_addr.sin_port; */  /* the used port */
            break;

        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_UDP_OPEN failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = SNMPX_ERR_LL;
            break;
    }

    SessionIndex = SNMPX_SOCK_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(pRQB);

    SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

    switch (SNMPX_HANDLE_GET_TYPE(Orginator))
    {
        case SNMPX_HANDLE_TYPE_USER:
        {
            SNMPX_HDB_USER_PTR_TYPE pHDBUser  = SNMPX_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);
            SNMPX_UPPER_RQB_PTR_TYPE pRQBUser;

            /* get the corresponding user HDB and store the socket */
            SNMPX_ASSERT_NULL_PTR(pHDBUser);
            SNMPX_ASSERT(SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS);

            SNMPX_ASSERT_NULL_PTR(pHDBUser->MngrSession[SessionIndex].pUserRQB);
            pRQBUser = pHDBUser->MngrSession[SessionIndex].pUserRQB;
            pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;

            if (Response == SNMPX_OK)
            {
                pHDBUser->MngrSession[SessionIndex].SockFD = SockFD;   /* lower-layer send sock  */
                pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_OPEN;

				/* if this is the first user and the first session, we start the timer */
				if ( SNMPX_GET_GLOB_PTR()->CyclicTimerUserCnt == 1 )
				{
					if (SNMPX_UserGetActiveSessions(pHDBUser) == 1)
					{
						SNMPX_TimerStart(SNMPX_GET_GLOB_PTR()->CyclicTimerID, 1); /* 100ms */
					}
				}
            }
            else    /* could not open */
            {
                pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_CLOSE;
                (LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDBUser, SessionIndex);
            }

            SNMPX_UserRequestFinish(pHDBUser,pRQBUser,Response);
            /* SNMPX_SOCKManagerRequestHandler(pHDB, LSA_NULL, Response); */ /* further actions taken in request handler */
        }
        break;
        case SNMPX_HANDLE_TYPE_SOCK:
            pHDB->SockFD = SockFD;       /* lower-layer send sock  */
            /* pHDB->LocalPort = sock_open->loc_addr.sin_port; */  /* the used port */
#ifdef SNMPX_CFG_SNMP_AGENT
            SNMPX_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */
            /* SNMPX_SOCKRequestHandler(pHDB, LSA_NULL, Response); */ /* further actions taken in request handler */
#endif
            break;
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Unknown Orginator (0x%X)!", Orginator);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKOpenPortDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKClosePort                         +*/
/*+  Input/Output          :    SNMPX_HDB_PTR_TYPE             pHDB         +*/
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
/*+  Description: Close port 161 for SNMP                                   +*/
/*+               (SOCK_OPC_CLOSE)                                          +*/
/*+                                                                         +*/
/*+               Allocates RQB and does SOCK-Close                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_SOCKClosePort(
    SNMPX_HDB_SOCK_PTR_TYPE    pHDB,
    LSA_UINT16                 Orginator,
    LSA_INT32                  SockFD,
    LSA_UINT16                 SessionIndex)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE           pRQB = LSA_NULL;
    LSA_SYS_PTR_TYPE                        pSys;
    LSA_UINT16                              Status;
    LSA_USER_ID_TYPE                        UserId;

    SNMPX_FUNCTION_TRACE_04(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKClosePort(pHDB: 0x%X, SessionIndex: 0x%X, Orginator: 0x%X, SockFD: 0x%X)",
                            pHDB, SessionIndex, Orginator, SockFD);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    Status = SNMPX_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    SNMPX_SOCK_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(SOCK_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        SNMPX_SOCK_RQB_SET_OPCODE(pRQB,SOCK_OPC_CLOSE);
        SNMPX_SOCK_RQB_SET_HANDLE(pRQB,pHDB->SockHandle);
        SNMPX_SOCK_RQB_SET_USERID_UVAR16(pRQB,Orginator);
        SNMPX_SOCK_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(pRQB,SessionIndex);

        SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,">>>: SOCK-Request: SOCK_OPC_CLOSE (Handle: 0x%X, pRQB: 0x%X)",pHDB->ThisHandle,pRQB);

        pRQB->args.close.sock_fd = SockFD;

        SNMPX_SOCK_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower RQB-memory failed!");
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKClosePort(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKClosePortDone                     +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                             SNMPX_HDB_PTR_TYPE             pHDB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Close Port 161 done handling                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKClosePortDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16                  Response;
    LSA_UINT16                  RetVal = LSA_RET_OK;
    LSA_UINT16                  Orginator;
    LSA_UINT16                  SessionIndex;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKClosePortDone(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB,pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

    SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE,"<<<: SOCK_OPC_CLOSE done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));

    Orginator  = SNMPX_SOCK_RQB_GET_USERID_UVAR16(pRQB);

    Response         = SNMPX_OK;

    switch (SNMPX_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            break;

        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: SOCK_OPC_CLOSE failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = SNMPX_ERR_LL;
            break;
    }

    SessionIndex = SNMPX_SOCK_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(pRQB);

    SNMPX_SOCK_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

    switch (SNMPX_HANDLE_GET_TYPE(Orginator))
    {
        case SNMPX_HANDLE_TYPE_USER:
        {
            SNMPX_HDB_USER_PTR_TYPE pHDBUser = SNMPX_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);

            /* get the corresponding user HDB and store the socket */
            SNMPX_ASSERT_NULL_PTR(pHDBUser);

            switch (pHDBUser->Params.pDetail->PathType)
            {
                case SNMPX_PATH_TYPE_AGENT:
#ifdef SNMPX_CFG_SNMP_AGENT
                    SNMPX_SOCKRequestHandler(pHDB, LSA_NULL, Response); /* further actions taken in request handler */
#endif
                    break;
                case SNMPX_PATH_TYPE_MANAGER:
                {
                    SNMPX_UPPER_RQB_PTR_TYPE pRQBUser;

                    SNMPX_ASSERT(SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS);

                    pRQBUser = pHDBUser->MngrSession[SessionIndex].pUserRQB;

                    SNMPX_ASSERT(pHDBUser->MngrSession[SessionIndex].PortState == SNMPX_SNMP_STATE_WF_CLOSE);

                    /* it's a SNMPX_OPC_CLOSE_MANAGER_SESSION */
                    if (snmpx_is_not_null(pRQBUser))
                    {
                        SNMPX_ASSERT(SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_CLOSE_MANAGER_SESSION);
                        pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;

						if (Response == SNMPX_OK)   /* only if o.k. */
						{
							pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_CLOSE;
							(LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDBUser, SessionIndex);

							/* if this is the last user and the last session, we stop the timer */
							if ( SNMPX_GET_GLOB_PTR()->CyclicTimerUserCnt == 1 )
							{
								if(SNMPX_UserGetActiveSessions(pHDBUser) == 0)
								{
									SNMPX_TimerStop(SNMPX_GET_GLOB_PTR()->CyclicTimerID);
								}
							}
						}

                        SNMPX_UserRequestFinish(pHDBUser,pRQBUser,Response);
                    }

                    if (snmpx_is_not_null(pHDBUser->pOCReq)) /* it's a SNMPX_OPC_CLOSE_CHANNEL */
                    {
                        LSA_BOOL Finished = LSA_FALSE;

                        pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_CLOSE;
                        (LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDBUser, SessionIndex);

						/* if this is the last user and the last session, we stop the timer */
						if ( SNMPX_GET_GLOB_PTR()->CyclicTimerUserCnt == 1 )
						{
							if(SNMPX_UserGetActiveSessions(pHDBUser) == 0)
							{
								SNMPX_TimerStop(SNMPX_GET_GLOB_PTR()->CyclicTimerID);
							}
						}

                        pRQBUser = pHDBUser->pOCReq;
                        SNMPX_ASSERT(SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_CLOSE_CHANNEL);

                        (LSA_VOID) SNMPX_CheckUserCloseChannel(pHDBUser, &Finished);

                        if (Finished)
                        {
							SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE Cbf = pHDBUser->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
							LSA_SYS_PTR_TYPE             pSys     = pHDBUser->Params.pSys;
							SNMPX_DETAIL_PTR_TYPE        pDetail  = pHDBUser->Params.pDetail;

                            /* first Cancel the ReceiveRQB's, then close the channel ! */
                            RetVal = SNMPX_UserCancelReceive(pHDBUser);
                            if (RetVal != SNMPX_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);

						    SNMPX_RQB_SET_HANDLE(pRQBUser, pHDBUser->Params.HandleUpper );

                            /* we release the HDB. Handle is closed now    */
                            SNMPX_UserReleaseHDB(pHDBUser);

							SNMPX_RQBSetResponse(pRQBUser,Response);
							SNMPX_UPPER_TRACE_04(LSA_TRACE_LEVEL_NOTE,"<<<: Request finished. Opcode: 0x%X, Handle: 0x%Xh,UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),pHDB->ThisHandle,SNMPX_RQB_GET_HANDLE(pRQB),Response);
							SNMPX_CallCbf(Cbf,pRQBUser,pSys);

                            /* release PATH-Info ! */
                            SNMPX_RELEASE_PATH_INFO(&RetVal,pSys,pDetail);
                            if (RetVal != LSA_RET_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_RELEASE_PATH_INFO);
                        }
                        /* further actions taken in request handler */
                        /* SNMPX_SOCKManagerRequestHandler(pHDB, LSA_NULL, Response); */
                    }
                }
                break;
                default:
                    SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Unknown PathType (0x%X)!", pHDBUser->Params.pDetail->PathType);
                    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
        break;
        case SNMPX_HANDLE_TYPE_SOCK:
            /* now close the channel */
            SNMPX_SOCKChannelDone(pHDB,Response); /* further actions taken in channel handler */
            break;
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Unknown Orginator (0x%X)!", Orginator);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_SOCKClosePortDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_SOCKSendDone                          +*/
/*+  Input                      SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                             SNMPX_HDB_SOCK_PTR_TYPE        pHDB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Send-Response done handling.                              +*/
/*+                                                                         +*/
/*+              - frees RQB memory                                         +*/
/*+              - call global state handling for further actions           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_SOCKSendDone(
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    SNMPX_HDB_SOCK_PTR_TYPE        pHDB)
{
    LSA_UINT16  Response;
    LSA_UINT16  Orginator;
    LSA_UINT16  SessionIndex;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_SOCKSendDone(pRQB: 0x%X,pHDB: 0x%X)",
                            pRQB,pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    Orginator  = SNMPX_SOCK_RQB_GET_USERID_UVAR16(pRQB);

    switch (SNMPX_SOCK_RQB_GET_RESPONSE(pRQB))
    {
        case SOCK_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            Response  = SNMPX_OK;
            break;

			case SOCK_RSP_ERR_LOC_ABORT:
            Response = SNMPX_RSP_ERR_LOC_ABORT;
            break;

		case SOCK_RSP_ERR_REM_ABORT:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE,"SOCK-ERROR: SOCK_OPC_UDP_SEND failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
            Response  = SNMPX_ERR_REM_ABORT;
            break;
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SOCK-ERROR: SOCK_OPC_UDP_SEND failed.(0x%X).",SNMPX_SOCK_RQB_GET_RESPONSE(pRQB));
            Response = SNMPX_ERR_LL;
            break;
    }

    SessionIndex = SNMPX_SOCK_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(pRQB);

    snmpx_sock_free_rsp_rqb(pHDB, pRQB);

    switch (SNMPX_HANDLE_GET_TYPE(Orginator))
    {
#ifdef SNMPX_CFG_SNMP_MANAGER
        case SNMPX_HANDLE_TYPE_USER:
        {
            SNMPX_HDB_USER_PTR_TYPE pHDBUser  = SNMPX_GetUserHDBFromHandle((LSA_HANDLE_TYPE)Orginator);
            SNMPX_UPPER_RQB_PTR_TYPE pRQBUser;

            /* get the corresponding user HDB and store the socket */
            SNMPX_ASSERT_NULL_PTR(pHDBUser);
            SNMPX_ASSERT(SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS);

            pRQBUser = pHDBUser->MngrSession[SessionIndex].pUserRQB;
            SNMPX_ASSERT_NULL_PTR(pRQBUser);

            /* SNMPX_SOCKManagerRequestHandler(pHDB, LSA_NULL, Response); */ /* further actions taken in request handler */
            switch (pHDBUser->MngrSession[SessionIndex].PortState)
            {
                case SNMPX_SNMP_STATE_WF_CNF:
                    pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_OPEN;
                    if (pHDBUser->MngrSession[SessionIndex].RxPendingCnt == 0)  /* receive resource is no more pending ? */
                    {
                        /**************** SNMPX_OPC_CLOSE_MANAGER_SESSION ****************/
                        if (SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_CLOSE_MANAGER_SESSION) /* close the port now */
                        {
                            pHDBUser->MngrSession[SessionIndex].RetryCnt = 0;   /* stop the retries */

                            Response = SNMPX_SOCKClosePort(pHDB, pHDBUser->ThisHandle, pHDBUser->MngrSession[SessionIndex].SockFD, SessionIndex);
                            if (Response != SNMPX_OK)
                            {
                                pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
                                SNMPX_UserRequestFinish(pHDBUser,pRQBUser,SNMPX_ERR_FAILED);
                                SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing Manager close port failed.");
                            }
                            else
                            {
                                pHDBUser->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_WF_CLOSE;
                            }
                            break;
                        }

                        /**************** SNMPX_OPC_REQUEST_MANAGER ****************/
                        SNMPX_ASSERT(SNMPX_RQB_GET_OPCODE(pRQBUser) == SNMPX_OPC_REQUEST_MANAGER);
                        if (pHDBUser->MngrSession[SessionIndex].RetryCnt == 0)  /* confirm the user */
                        {
                            /* the response is alerady written in recv.done */
                            pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
                            SNMPX_UserRequestFinish(pHDBUser,pRQBUser,SNMPX_RQB_GET_RESPONSE(pRQBUser));
                        }
                        else
                        {
                            Response = SNMPX_DoRequestManager(pRQBUser, pHDBUser);
                            if (Response != SNMPX_OK)   /* stop the retries */
                            {
                                pHDBUser->MngrSession[SessionIndex].pUserRQB = LSA_NULL;
                                SNMPX_UserRequestFinish(pHDBUser,pRQBUser,Response);
                            }
                        }
                        break;
                    }
                    break;
                case SNMPX_SNMP_STATE_OPEN:
                case SNMPX_HANDLE_STATE_INIT:
                case SNMPX_SNMP_STATE_CLOSE:
                case SNMPX_SNMP_STATE_WF_OPEN:
                case SNMPX_SNMP_STATE_WF_CLOSE:
                default:
                    SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_OPC_REQUEST_MANAGER failed (PortState: 0x%X).",pHDBUser->MngrSession[SessionIndex].PortState);
                    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
        break;
#endif
#ifdef SNMPX_CFG_SNMP_AGENT
        case SNMPX_HANDLE_TYPE_SOCK:    /* SNMP agent */
            SNMPX_ASSERT (SOCK_RSP_OK == Response);
            pHDB->RxPendingCnt--; /* number of Rx-Request pending within SOCK */
            break;
#endif
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Unknown Orginator (0x%X)!", Orginator);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_SOCKSendDone(Response: 0x%X)",Response);
}

#if SNMPX_CFG_SNMP_OPEN_BSD
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_oha_request_lower_done                +*/
/*+  Input                      SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for OHA-requests.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID snmpx_oha_request_lower_done(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB
) {
	SNMPX_HDB_OHA_PTR_TYPE pHDB;

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN :snmpx_oha_request_lower_done(pRQB: 0x%X)"
		, pRQB
		);

	SNMPX_ENTER();

	SNMPX_ASSERT_NULL_PTR(pRQB);

	if (SNMPX_OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_OPEN_CHANNEL)
	{
		pHDB = SNMPXGetOHAHDBFromHandle(pRQB->args.channel.handle_upper);
	}
	else
	{
		pHDB = SNMPXGetOHAHDBFromHandle(SNMPX_OHA_RQB_GET_HANDLE(pRQB));
	}

	switch (SNMPX_OHA_RQB_GET_OPCODE(pRQB) )
	{
		case OHA_OPC_OPEN_CHANNEL:
			SNMPX_OHAOpenChannelDone(pRQB, pHDB);
			break;

		case OHA_OPC_CLOSE_CHANNEL:
			SNMPX_OHACloseChannelDone(pRQB, pHDB);
			break;

		case OHA_OPC_LLDP_CONTROL_OID:
		case OHA_OPC_MRP_CONTROL_OID:
			SNMPX_OHA_Control_Oid_Done(pRQB, pHDB);
			break;

		case OHA_OPC_SNMP_INDICATION:
			SNMPX_OHA_Snmp_Indication_Done(pRQB, pHDB);
			break;

		default:
			SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
			break;
	}

	SNMPX_EXIT();

	SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT
		, "OUT: snmpx_oha_request_lower_done()"
		);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_OHAOpenChannel                       +*/
/*+  Input                      LSA_HANDLE_TYPE       SNMPXHandle           +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  SNMPXHandle           : SNMPX OHA-Channel handle                      +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : SNMPX_OK                                        +*/
/*+                         SNMPX_ERR_RESOURCE                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to OHA. The Request will be  +*/
/*+               finished by calling the OHA-Callbackfunction.            +*/
/*+               snmpx_oha_request_lower_done() if SNMPX_OK.              +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16
SNMPX_OHAOpenChannel(
	LSA_HANDLE_TYPE SNMPXHandle,
	LSA_SYS_PATH_TYPE SysPath,
	LSA_SYS_PTR_TYPE pSys
) {
	SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
	LSA_UINT16 Result;
	LSA_USER_ID_TYPE UserId;

	SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHAOpenChannel(SNMPXHandle: 0x%X, SysPath: 0x%X)"
		, SNMPXHandle
		, SysPath
		);

	UserId.uvar32 = 0;

	SNMPX_OHA_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(OHA_RQB_TYPE),pSys);

	if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
	{
		SNMPX_OHA_RQB_SET_OPCODE(pRQB, OHA_OPC_OPEN_CHANNEL);

		pRQB->args.channel.handle_upper = SNMPXHandle;
		pRQB->args.channel.sys_path = SysPath;
		pRQB->args.channel.oha_request_upper_done_ptr = snmpx_oha_request_lower_done;

		SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE
			, ">>>: OHA-Request: OHA_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)"
			, SNMPXHandle
			, pRQB
			);

		SNMPX_OHA_OPEN_CHANNEL_LOWER(pRQB, pSys);

		Result = SNMPX_OK;
	}
	else
	{
		SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP
			, "OHA-Allocating RQB memory failed!"
			);

		Result = SNMPX_ERR_RESOURCE;
	}

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "OUT: SNMPX_OHAOpenChannel. Result: 0x%X"
		, Result
		);

	return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_OHAOpenChannelDone                   +*/
/*+  Input                      SNMPX_OHA_LOWER_RQB_PTR_TYPE   pRQB        +*/
/*+                             SNMPX_HDB_OHA_PTR_TYPE         pHDB        +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Open Channel done handling.                               +*/
/*+                                                                         +*/
/*+               - save OHA-Channel                                       +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID
SNMPX_OHAOpenChannelDone(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB,
	SNMPX_HDB_OHA_PTR_TYPE pHDB
) {
	LSA_UINT16 Response;
	LSA_UINT16 RetVal = LSA_RET_OK;

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHAOpenChannelDone(pRQB: 0x%X)"
		, pRQB
		);

	SNMPX_LOWER_TRACE_01(LSA_TRACE_LEVEL_NOTE
		, "<<<: OHA_OPC_OPEN_CHANNEL done Rsp(0x%X)"
		, SNMPX_OHA_RQB_GET_RESPONSE(pRQB)
		);

	if (SNMPX_OHA_RQB_GET_RESPONSE(pRQB) == OHA_OK)
	{
		SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW, "OHA-Open Channel successful");

		pHDB->pOCReq->args.Channel.handle = pRQB->args.channel.handle_upper;
		pHDB->OhaHandle = pRQB->args.channel.handle;

		Response = SNMPX_OK;

		SNMPX_OHA_Snmp_Indication(); /* provide indication resource  */
	}
	else
	{
		SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP
			, "OHA-Open Channel failed.[0x%X]"
			, SNMPX_OHA_RQB_GET_RESPONSE(pRQB)
			);

		Response = SNMPX_ERR_LL;
	}

	SNMPX_RQBSetResponse(pHDB->pOCReq, Response);

	SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE
		, "<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X"
		, SNMPX_RQB_GET_OPCODE(pRQB)
		, SNMPX_RQB_GET_HANDLE(pRQB)
		, Response
		);

	SNMPX_CallCbf( pHDB->Params.Cbf
		, pHDB->pOCReq
		, pHDB->Params.pSys
		);

	pHDB->pOCReq = LSA_NULL; /* sanity */

	SNMPX_OHA_FREE_LOWER_RQB(&RetVal, pRQB, pHDB->Params.pSys);

	SNMPX_MEM_FREE_FAIL_CHECK(RetVal);

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "OUT: SNMPX_OHAOpenChannelDone(Response: 0x%X)"
		, Response
		);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_OHACloseChannel                      +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle            +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             SNMPX_ERR_RESOURCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA handle to close                            +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to OHA. The Request will be +*/
/*+               finished by calling the OHA-Callbackfunction.            +*/
/*+               snmpx_oha_request_lower_done()                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16
SNMPX_OHACloseChannel(
	LSA_HANDLE_TYPE OHAHandle,
	LSA_SYS_PTR_TYPE pSys
) {
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB = LSA_NULL;
	LSA_UINT16 Result;
	LSA_USER_ID_TYPE UserId;

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHACloseChannel(OHAHandle: 0x%X)"
		, OHAHandle
		);

	Result = SNMPX_OK;

	UserId.uvar32 = 0;

	SNMPX_OHA_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(OHA_RQB_TYPE), pSys);

	if ( !LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
	{
		SNMPX_OHA_RQB_SET_OPCODE(pRQB, OHA_OPC_CLOSE_CHANNEL);
		SNMPX_OHA_RQB_SET_HANDLE(pRQB, OHAHandle);

		SNMPX_LOWER_TRACE_02(LSA_TRACE_LEVEL_NOTE
			, ">>>: OHA-Request: OHA_OPC_CLOSE_CHANNEL (OHAHandle: 0x%X,pRQB: 0x%X)"
			, OHAHandle
			, pRQB
			);

		SNMPX_OHA_CLOSE_CHANNEL_LOWER(pRQB, pSys);
	}
	else
	{
		SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"OHA-Allocating RQB memory failed!");
		Result = SNMPX_ERR_RESOURCE;
	}

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "OUT: SNMPX_OHACloseChannel(Result: 0x%X)"
		, Result
		);
	return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_OHACloseChannelDone                  +*/
/*+  Input                      SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to channel context                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Close Channel done handling.                              +*/
/*+               - frees RQB memory                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID
SNMPX_OHACloseChannelDone(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	SNMPX_HDB_OHA_PTR_TYPE pHDB
) {
	LSA_UINT16 Response;
	LSA_UINT16 ret_val;
	LSA_RESPONSE_TYPE rsp = SNMPX_RQB_GET_RESPONSE(pRQB);

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHACloseChannelDone(pRQB: 0x%X)"
		, pRQB
		);

	SNMPX_OHA_FREE_LOWER_RQB(&ret_val, pRQB, pHDB->Params.pSys);
	SNMPX_MEM_FREE_FAIL_CHECK(ret_val);

	if ( rsp!= OHA_OK)
	{
		Response = SNMPX_ERR_LL;

		SNMPX_RQBSetResponse(pHDB->pOCReq, Response);

		SNMPX_CallCbf(pHDB->Params.Cbf, pHDB->pOCReq, pHDB->Params.pSys);

		pHDB->pOCReq = LSA_NULL; /* sanity */
	}
	else
	{

		SNMPX_UPPER_RQB_PTR_TYPE pOCReq = pHDB->pOCReq;

		Response = SNMPX_OK;

		SNMPX_RQBSetResponse(pOCReq, Response);

		{
		LSA_SYS_PTR_TYPE pSys = pHDB->Params.pSys;
		SNMPX_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail;
		SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE req_done_fct = pHDB->Params.Cbf;

		SNMPX_OHAReleaseHDB(pHDB);
		pHDB->pOCReq = LSA_NULL;

		SNMPX_CallCbf(req_done_fct, pOCReq, pSys);

		SNMPX_RELEASE_PATH_INFO(&ret_val, pSys, pDetail );
		SNMPX_ASSERT( ret_val == LSA_RET_OK );
		}
	}

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "OUT: SNMPX_OHACloseChannelDone(Response: 0x%X)"
		, Response
		);
}


LSA_VOID
SNMPX_OHA_Snmp_Indication(
	LSA_VOID
) {
	SNMPX_HDB_OHA_PTR_TYPE pOHA = SNMPX_GET_HOHA_PTR(); /* (!)works only for one lower */
	SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
	LSA_USER_ID_TYPE UserId;

	UserId.uvar32 = 0;
	SNMPX_OHA_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(OHA_RQB_TYPE), pOHA->Params.pSys);
	SNMPX_ASSERT(!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))

	SNMPX_OHA_RQB_SET_OPCODE(pRQB, OHA_OPC_SNMP_INDICATION);
	SNMPX_OHA_RQB_SET_HANDLE(pRQB, pOHA->OhaHandle);

	pRQB->args.SnmpControl.Mode = OHA_CTRL_SNMP_ACTIVATE;

	SNMPX_OHA_REQUEST_LOWER(pRQB, pOHA->Params.pSys);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_OHA_Snmp_Indication_Done              +*/
/*+  Input                      SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: treat indication                                          +*/
/*+   if OHA_OK, re-provide indication resource                             +*/
/*+   if OHA_OK_CANCEL, free indication resource                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID
SNMPX_OHA_Snmp_Indication_Done(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	const SNMPX_OHA_TYPE * const pHDB
) {
	SNMPX_ASSERT(!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL));

	if (SNMPX_OHA_RQB_GET_RESPONSE(pRQB) == OHA_OK) /*  */
	{
		int retval = 0; /* avoid C4701: potentially uninitialized local variable 'retval' used */

		if (pRQB->args.SnmpControl.Mode == OHA_CTRL_SNMP_ACTIVATE)
		{
			retval = obsd_snmpd_snmp_on(); /* open snmp socket */
		}
		else if (pRQB->args.SnmpControl.Mode == OHA_CTRL_SNMP_DEACTIVATE)
		{
			retval = obsd_snmpd_snmp_off(); /* close snmp socket */
		}
		else
		{
			SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
		}

		if (retval == 0)
		{
			SNMPX_OHA_RQB_SET_RESPONSE(pRQB, OHA_OK);
		}
		else
		{
			SNMPX_OHA_RQB_SET_RESPONSE(pRQB, OHA_ERR_FAILED);
		}

		SNMPX_OHA_RQB_SET_HANDLE(pRQB, pHDB->OhaHandle);
		SNMPX_OHA_REQUEST_LOWER(pRQB, pHDB->Params.pSys);
	}
	else
	{
		LSA_RESPONSE_TYPE response = SNMPX_OHA_RQB_GET_RESPONSE(pRQB);

		if (response != OHA_OK_CANCEL)
		{
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_FATAL
			, "OUT: SNMPX_OHA_Snmp_Indication_Done(Response: 0x%X)"
			, response
			);

			SNMPX_FATAL(SNMPX_FATAL_ERR_USER);
		}
		else
		{
			SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
			, "OUT: SNMPX_OHA_Snmp_Indication_Done(Response: 0x%X)"
			, response
			);
		}

		{
		LSA_UINT16 ret_val;
		SNMPX_OHA_FREE_LOWER_RQB(&ret_val, pRQB, pHDB->Params.pSys);
		SNMPX_MEM_FREE_FAIL_CHECK(ret_val);
		}
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_oha_get_lldp_var                      +*/
/*+  Input                 :    OHA_OID_PTR_TYPE	oid                     +*/
/*+                             int                 oid_length              +*/
/*+                             void                *buffer                 +*/
/*+                             int                 buffer_length           +*/
/*+                             void                *context_ptr            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: rets a variable addressed with oid                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
snmpx_oha_get_lldp_var(
	unsigned int *oid,
	int oid_length,
	void *buffer,
	int buffer_length,
	void *context_ptr
) {
	return SNMPX_OHA_Control_Oid((OHA_OID_PTR_TYPE)oid
		, oid_length
		, buffer
		, buffer_length
		, 0 /* var_type is ignored by get request */
		, 0 /* VarLength is ignored by get request */
		, context_ptr
		, OHA_OPC_LLDP_CONTROL_OID
		, OHA_SNMP_GET_REQ
		);
}

int
snmpx_oha_get_next_lldp_var(
	unsigned int *oid,
	int oid_length,
	void *buffer,
	int buffer_length,
	void *context_ptr
) {
	return SNMPX_OHA_Control_Oid((OHA_OID_PTR_TYPE)oid
		, oid_length
		, buffer
		, buffer_length
		, 0 /* var_type is ignored by get request */
		, 0 /* VarLength is ignored by get request */
		, context_ptr
		, OHA_OPC_LLDP_CONTROL_OID
		, OHA_SNMP_GETNEXT_REQ
		);
}

int
snmpx_oha_set_lldp_var(
	unsigned int *oid,
	int oid_length,
	void *buffer,
	unsigned char var_type,
	int var_length,
	void *context_ptr
) {
	return SNMPX_OHA_Control_Oid((OHA_OID_PTR_TYPE)oid
		, oid_length
		, buffer
		, 0 /* buffer length not needed for set request */
		, var_type
		, (LSA_UINT32)var_length
		, context_ptr
		, OHA_OPC_LLDP_CONTROL_OID
		, OHA_SNMP_SET_REQ
		);
}

int
snmpx_oha_get_mrp_var(
	unsigned int *oid,
	int oid_length,
	void *buffer,
	int buffer_length,
	void *context_ptr
) {
	return SNMPX_OHA_Control_Oid((OHA_OID_PTR_TYPE)oid
		, oid_length
		, buffer
		, buffer_length
		, 0 /* var_type is ignored by get request */
		, 0 /* VarLength is ignored by get request */
		, context_ptr
		, OHA_OPC_MRP_CONTROL_OID
		, OHA_SNMP_GET_REQ
		);
}

int
snmpx_oha_get_next_mrp_var(
	unsigned int *oid,
	int oid_length,
	void *buffer,
	int buffer_length,
	void *context_ptr
) {
	return SNMPX_OHA_Control_Oid((OHA_OID_PTR_TYPE)oid
		, oid_length
		, buffer
		, buffer_length
		, 0 /* var_type is ignored by get request */
		, 0 /* VarLength is ignored by get request */
		, context_ptr
		, OHA_OPC_MRP_CONTROL_OID
		, OHA_SNMP_GETNEXT_REQ
		);
}

int
snmpx_oha_set_mrp_var(
	unsigned int *oid,
	int oid_length,
	void *buffer,
	unsigned char var_type,
	int var_length,
	void *context_ptr
) {
	return SNMPX_OHA_Control_Oid((OHA_OID_PTR_TYPE)oid
		, oid_length
		, buffer
		, 0 /* buffer length not needed for set request */
		, var_type
		, (LSA_UINT32)var_length
		, context_ptr
		, OHA_OPC_MRP_CONTROL_OID
		, OHA_SNMP_SET_REQ
		);
}

LSA_INT SNMPX_OHA_Control_Oid(
	OHA_OID_PTR_TYPE oid,
	int oid_length,
	void *buffer,
	int buffer_length,
	LSA_UINT8 VarType,
	LSA_UINT32 VarLength,
	void *context_ptr,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT8 MessageType
) {
	SNMPX_HDB_OHA_PTR_TYPE pOHA = SNMPX_GET_HOHA_PTR(); /* (!)works only for one lower */
	SNMPX_OHA_LOWER_RQB_PTR_TYPE  pRQB = LSA_NULL;
	LSA_USER_ID_TYPE UserId;
	LSA_INT return_val;

	UserId.uvar32 = 0;

	if (MessageType == OHA_SNMP_GET_REQ
		&& buffer_length < OHA_MAX_STATION_NAME_LEN
		)
	{
		return_val = -1;
	}
	else
	{
		SNMPX_OHA_ALLOC_LOWER_RQB(&pRQB, UserId, sizeof(OHA_RQB_TYPE), pOHA->Params.pSys);
		if (LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
		{
			return_val = -1;
		}
		else
		{
			SNMPX_OHA_RQB_SET_OPCODE(pRQB, opcode);
			SNMPX_OHA_RQB_SET_HANDLE(pRQB, pOHA->OhaHandle);
			SNMPX_OHA_RQB_SET_USER_ID_PTR(pRQB, context_ptr);

			pRQB->args.SnmpOid.MessageType = MessageType;
			pRQB->args.SnmpOid.SnmpVersion = OHA_SNMP_VERSION_2;

			pRQB->args.SnmpOid.pOidName    = oid;
			pRQB->args.SnmpOid.OidNameLen  = (LSA_UINT32)oid_length;

			pRQB->args.SnmpOid.VarType     = VarType;
			if (MessageType == OHA_SNMP_SET_REQ)
			{
				pRQB->args.SnmpOid.VarLength = VarLength; /* needed for SET */
			}
			else
			{
				pRQB->args.SnmpOid.VarLength = (LSA_UINT32)buffer_length; /* GET/GETNEXT: will receive length of returned data */
			}
			pRQB->args.SnmpOid.pVarValue   = (LSA_UINT8*)buffer;

			pRQB->args.SnmpOid.SnmpError   = OHA_SNMP_ERR_GENERR; /* unchanged if rsp != OHA_OK */

			SNMPX_OHA_REQUEST_LOWER(pRQB, pOHA->Params.pSys);

			return_val = 0;
		}
	}

	return return_val;
}

LSA_VOID SNMPX_OHA_Control_Oid_Done(
	SNMPX_OHA_LOWER_RQB_PTR_TYPE pRQB,
	const SNMPX_OHA_TYPE * const pHDB
) {
	OHA_UPPER_GET_OID_PTR_TYPE pSnmp = &pRQB->args.SnmpOid;

	SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT
		, "IN : SNMPX_OHACloseChannelDone(pRQB: 0x%X)"
		, pRQB
		);

	switch (SNMPX_OHA_RQB_GET_OPCODE(pRQB))
	{
	case OHA_OPC_LLDP_CONTROL_OID:
		switch (pSnmp->MessageType)
		{
		case OHA_SNMP_GET_REQ:
			snmpx_oha_get_lldp_var_done((unsigned int*)pSnmp->pOidName
					, (int)pSnmp->OidNameLen
					, (int)pSnmp->SnmpError
					, pSnmp->pVarValue
					, pSnmp->VarType
					, (int)pSnmp->VarLength
					, SNMPX_OHA_RQB_GET_USER_ID_PTR(pRQB)
					);
			break;

		case OHA_SNMP_GETNEXT_REQ:
			snmpx_oha_get_next_lldp_var_done((unsigned int*)pSnmp->pOidName
					, (int)pSnmp->OidNameLen
					, (int)pSnmp->SnmpError
					, pSnmp->pVarValue
					, pSnmp->VarType
					, (int)pSnmp->VarLength
					, SNMPX_OHA_RQB_GET_USER_ID_PTR(pRQB)
					);
			break;

		case OHA_SNMP_SET_REQ:
			snmpx_oha_set_lldp_var_done((unsigned int*)pSnmp->pOidName
					, (int)pSnmp->OidNameLen
					, (int)pSnmp->SnmpError
					, pSnmp->pVarValue
					, pSnmp->VarType
					, (int)pSnmp->VarLength
					, SNMPX_OHA_RQB_GET_USER_ID_PTR(pRQB)
					);
			break;

		default:
			SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ); /* sanity */
			break;
		}
		break;

	case OHA_OPC_MRP_CONTROL_OID:
		switch (pSnmp->MessageType)
		{
		case OHA_SNMP_GET_REQ:
			snmpx_oha_get_mrp_var_done((unsigned int*)pSnmp->pOidName
					, (int)pSnmp->OidNameLen
					, (int)pSnmp->SnmpError
					, pSnmp->pVarValue
					, pSnmp->VarType
					, (int)pSnmp->VarLength
					, SNMPX_OHA_RQB_GET_USER_ID_PTR(pRQB)
					);
			break;

		case OHA_SNMP_GETNEXT_REQ:
			snmpx_oha_get_next_mrp_var_done((unsigned int*)pSnmp->pOidName
					, (int)pSnmp->OidNameLen
					, (int)pSnmp->SnmpError
					, pSnmp->pVarValue
					, pSnmp->VarType
					, (int)pSnmp->VarLength
					, SNMPX_OHA_RQB_GET_USER_ID_PTR(pRQB)
					);
			break;

		case OHA_SNMP_SET_REQ:
			snmpx_oha_set_mrp_var_done((unsigned int*)pSnmp->pOidName
					, (int)pSnmp->OidNameLen
					, (int)pSnmp->SnmpError
					, pSnmp->pVarValue
					, pSnmp->VarType
					, (int)pSnmp->VarLength
					, SNMPX_OHA_RQB_GET_USER_ID_PTR(pRQB)
					);
			break;

		default:
			SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ); /* sanity */
			break;
		}
		break;

	default:
		SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ); /* sanity */
		break;
	}

	{
	LSA_UINT16 ret_val;
	SNMPX_OHA_FREE_LOWER_RQB(&ret_val, pRQB, pHDB->Params.pSys);
	SNMPX_MEM_FREE_FAIL_CHECK(ret_val);
	}
}
#endif

/*****************************************************************************/
/*  end of file SNMPX_LOW.C                                                  */
/*****************************************************************************/

