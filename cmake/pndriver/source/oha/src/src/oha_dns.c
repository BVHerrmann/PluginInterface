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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_dns.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA DNS-LowerLayer interface functions           */
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

#define LTRC_ACT_MODUL_ID  19
#ifndef _lint
  #define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DNS */
#endif

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

#ifdef OHA_CFG_USE_DNS

#include "oha_int.h"            /* internal header */

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

static LSA_UINT16 DnsDataFailed(LSA_UINT32 err_offset)
{
    LSA_UINT16 RetVal = OHA_ERR_PRM_DATA;
    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: Length of Record failed (err_offset: 0x%X).", err_offset);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_dns_request_lower_done                  +*/
/*+  Input                      OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Callbackfunction for DNS-requests.                        +*/
/*+               Fills Error-Structure in NDB with DNS-Response            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID oha_dns_request_lower_done(
    OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB)
{
    OHA_HDB_DNS_PTR_TYPE   pHDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN :oha_dns_request_lower_done(pRQB: 0x%X)",
                          pRQB);
    OHA_ENTER();

    OHA_ASSERT_NULL_PTR(pRQB);

    /* DNS-request always done from a DNS-Channel. !        */

    pHDB = OHAGetDnsHDBFromHandle(OHA_DNS_RQB_GET_HANDLE(pRQB));

    switch (OHA_DNS_RQB_GET_OPCODE(pRQB) )
    {
        case DNS_OPC_OPEN_CHANNEL:
            OHA_DNSOpenChannelDone(pRQB,pHDB);
            break;
        case DNS_OPC_CLOSE_CHANNEL:
            OHA_DNSCloseChannelDone(pRQB,pHDB);
            break;
        case DNS_OPC_SET_SERVER:
            OHA_DNSSetDone(pRQB,pHDB);
            break;
        case DNS_OPC_GET_SERVER:
            OHA_DNSGetDone(pRQB,pHDB);
            break;
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_EXIT();

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dns_request_lower_done()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSOpenChannel                          +*/
/*+  Input                      LSA_HANDLE_TYPE       OHAHandle             +*/
/*+                             LSA_SYS_PATH_TYPE     SysPath               +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  OHAHandle           : OHA DNS-Channel handle                           +*/
/*+  SysPath              : SysPath for this handle.                        +*/
/*+  pSys                 : Sys-Ptr for open channel lower                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Open-Channel Request to DNS. The Request will be   +*/
/*+               finished by calling the DNS-Callbackfunction.             +*/
/*+               oha_dns_request_lower_done() if OHA_OK.                   +*/
/*+                                                                         +*/
/*+               Note: Only call if no open-channel request already running+*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_DNSOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSOpenChannel(OHAHandle: 0x%X, SysPath: 0x%X)",
                          OHAHandle, SysPath);
    Result = OHA_OK;

    UserId.uvar32 = 0;
    OHA_DNS_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DNS_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_DNS_RQB_SET_OPCODE(pRQB,DNS_OPC_OPEN_CHANNEL);

        pRQB->args.channel.handle_upper  = OHAHandle;
        pRQB->args.channel.sys_path      = SysPath;
        pRQB->args.channel.dns_request_upper_done_ptr = oha_dns_request_lower_done;

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: DNS-Request: DNS_OPC_OPEN_CHANNEL (Handle: 0x%X,pRQB: 0x%X)",OHAHandle,pRQB);
        OHA_DNS_OPEN_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"DNS-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSOpenChannel. Result: 0x%X",
                          Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSOpenChannelDone                      +*/
/*+  Input                      OHA_DNS_LOWER_RQB_PTR_TYPE     pRQB         +*/
/*+                             OHA_HDB_DNS_PTR_TYPE           pHDB         +*/
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
/*+               - save DNS-Channel                                        +*/
/*+               - frees RQB memory                                        +*/
/*+               - call global state handling for open channel for further +*/
/*+                 actions.                                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DNSOpenChannelDone(
    OHA_DNS_LOWER_RQB_PTR_TYPE pRQB,
    OHA_HDB_DNS_PTR_TYPE       pHDB)
{
    LSA_UINT16               Response;
    LSA_UINT16               RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSOpenChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    Response = OHA_OK;

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: DNS_OPC_OPEN_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DNS_RQB_GET_RESPONSE(pRQB));

    if ( OHA_DNS_RQB_GET_RESPONSE(pRQB) == DNS_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"DNS-Open Channel successful");
        /* save DNS-Channel in HDB */
        pHDB->DnsHandle = pRQB->args.channel.handle;

    }
    else
    {
        OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"DNS-Open Channel failed.[0x%X]",OHA_DNS_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_DNS_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_DNSChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSOpenChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSCloseChannel                         +*/
/*+  Input                      LSA_HANDLE_TYPE       DNSHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             OHA_ERR_RESOURCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  DNSHandle            : DNS handle to close                             +*/
/*+  pSys                 : Sys-Ptr for close channel lower                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Close Channel Request to DNS. The Request will be  +*/
/*+               finished by calling the DNS-Callbackfunction.             +*/
/*+               oha_dns_request_lower_done()                              +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple open request at a time.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_DNSCloseChannel(
    LSA_HANDLE_TYPE       DNSHandle,
    LSA_SYS_PTR_TYPE      pSys)
{

    OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB;
    LSA_UINT16                  Result;
    LSA_USER_ID_TYPE            UserId;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSCloseChannel(DNSHandle: 0x%X)",
                          DNSHandle);
    Result = OHA_OK;

    UserId.uvar32 = 0;
    OHA_DNS_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DNS_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        OHA_DNS_RQB_SET_OPCODE(pRQB,DNS_OPC_CLOSE_CHANNEL);
        OHA_DNS_RQB_SET_HANDLE(pRQB,DNSHandle);

        OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,">>>: DNS-Request: DNS_OPC_CLOSE_CHANNEL (DNSHandle: 0x%X,pRQB: 0x%X)",DNSHandle,pRQB);
        OHA_DNS_CLOSE_CHANNEL_LOWER(pRQB,pSys);
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"DNS-Allocating RQB memory failed!");
        Result = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSCloseChannel(Result: 0x%X)", Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSCloseChannelDone                     +*/
/*+  Input                      OHA_DNS_LOWER_RQB_PTR_TYPE    pRQB          +*/
/*+                             OHA_HDB_DNS_PTR_TYPE          pHDB          +*/
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
LSA_VOID OHA_DNSCloseChannelDone(
    OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DNS_PTR_TYPE        pHDB)
{
    LSA_UINT16               Response;
    LSA_UINT16               RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSCloseChannelDone(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    Response = OHA_OK;

    OHA_LOWER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<<: DNS_OPC_CLOSE_CHANNEL done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DNS_RQB_GET_RESPONSE(pRQB));

    if ( OHA_DNS_RQB_GET_RESPONSE(pRQB) == DNS_RSP_OK )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"DNS-Close Channel successful");
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"DNS-Close Channel failed.[0x%X]",OHA_DNS_RQB_GET_RESPONSE(pRQB));
        Response = OHA_ERR_LL;
    }

    OHA_DNS_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_DNSChannelDone(pHDB,Response); /* further actions taken in channel handler */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSCloseChannelDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSChannelDone                          +*/
/*+  Input                      OHA_HDB_DNS_PTR_TYPE         pHDB           +*/
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
/*+  Description: Handler for Open and Close-Channel requests to OHA for    +*/
/*+               DNS-Channels. This function is called after a lower       +*/
/*+               layer called back the OHA (e.g. after DNS finished his    +*/
/*+               open/close request)                                       +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DNSChannelDone(
    OHA_HDB_DNS_PTR_TYPE     pHDB,
    LSA_UINT16               Response)
{

    OHA_UPPER_RQB_PTR_TYPE       pRQB;
    LSA_UINT16                   PathStat = LSA_RET_OK;
    LSA_BOOL                     Finish   = LSA_FALSE; /* dont finish request by default */
    LSA_BOOL                     Release  = LSA_FALSE; /* dont release PATH-Info by default */
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
    LSA_SYS_PTR_TYPE             pSys;
    OHA_DETAIL_PTR_TYPE          pDetail;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSChannelDone(pHDB: 0x%X, Response: 0x%X)",
                          pHDB,Response);

    OHA_ASSERT_NULL_PTR(pHDB);

    pRQB   = pHDB->pOCReq;
    Cbf    = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe feed! */
    pSys   = pHDB->Params.pSys;
    pDetail = pHDB->Params.pDetail;

    OHA_ASSERT_NULL_PTR(pRQB);

    OHA_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"<<<: OHA_DNSChannelDone done (Handle: 0x%X,Rsp: 0x%X,State: 0x%X)",pHDB->ThisHandle,Response,pHDB->DnsState);

    switch (pHDB->DnsState)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a DNS-Open Channel request.                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_OPEN_DNS_RUNNING:
            if ( Response == OHA_OK)
            {
                /* OHA_DETAIL_PTR_TYPE pDetail = pHDB->Params.pDetail; */

                /* no more actions needed. Channel is open now. */
                pHDB->DnsState  = OHA_HANDLE_STATE_OPEN;
                pHDB->RequestState  = OHA_DNS_STATE_STOP;
                Finish = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Cannot open DNS. Response: 0x%X", Response);

                /* release PATH-Info !                          */
                Release = LSA_TRUE;

                Finish = LSA_TRUE;
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a DNS-Close-Channel because of error within Open  */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_ERROR_DNS_RUNNING:
            if ( Response == OHA_OK)
            {
                Response = pHDB->OpenResponse;  /* get Error-Response  */

                /* release PATH-Info ! */
                Release = LSA_TRUE;
            }
            else
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"Cannot Close DNS Channel().");
                OHA_FATAL(OHA_FATAL_ERR_DNS);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a DNS-Close-Channel request                         */
            /*-------------------------------------------------------------------*/
        case OHA_HANDLE_STATE_CLOSE_DNS_RUNNING:

            if ( Response == OHA_OK)
            {
                /* release PATH-Info !                          */
                Release = LSA_TRUE;
            }
            else
            {
                /* is this a fatal error ?                      */
                OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Cannot Close DNS Channel(). Response: 0x%X", Response);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/
        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */

    if ( Release )  /* release the PATH-Info ? */
    {
        /* we release the HDB  */
        OHA_DNSReleaseHDB(pHDB);
    }

    if ( Finish )  /* finish the request ? */
    {
        pHDB->pOCReq = LSA_NULL;  /* to signal no more request pending.      */

        OHA_RQBSetResponse(pRQB,Response);
        /* upper - handler already set */
        OHA_UPPER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),Response);
        OHA_CallCbf(Cbf,pRQB,pSys);
    }

    if ( Release )  /* release the PATH-Info ? */
    {
        OHA_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

        if (PathStat != LSA_RET_OK ) OHA_FATAL(OHA_FATAL_ERR_RELEASE_PATH_INFO);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSChannelDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSReleaseHDB                           +*/
/*+  Input/Output          :    OHA_HDB_DNS_PTR_TYPE pHDB                   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBDns   : Pointer to User Handle to release                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release DNS -HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DNSReleaseHDB(
    OHA_HDB_DNS_PTR_TYPE pHDB)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    pHDB->DnsState = OHA_HANDLE_STATE_FREE;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSReleaseHDB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSGetFreeHDB                           +*/
/*+  Input/Output          :    OHA_HDB_DNS_PTR_TYPE OHA_LOCAL..pHDBDns     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA DNS  -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_CHANNEL_USE                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free DNS -HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_DNSGetFreeHDB(
    OHA_HDB_DNS_PTR_TYPE *pHDBDns)
{
    LSA_UINT16                Status = OHA_OK;
    LSA_BOOL                  Found;
    OHA_HDB_DNS_PTR_TYPE      pHDB  = OHA_GET_HDNS_PTR();

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSGetFreeHDB(pHDBDns: 0x%X)",
                          pHDBDns);

    OHA_ASSERT_NULL_PTR(pHDBDns);

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/
    {
        if ( pHDB->DnsState == OHA_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            Found = LSA_FALSE;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/
    if ( Found )
    {
        pHDB->DnsState = OHA_HANDLE_STATE_CLOSED;
        *pHDBDns = pHDB;
    }
    else
    {
        Status = OHA_ERR_CHANNEL_USE;
        /* Error on getting handle. */
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP,"OHA_DNSGetFreeHDB(). Channel already used!");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSGetFreeHDB(Status: 0x%X, pHDB: 0x%X)",
                          Status,*pHDBDns);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSSet                                  +*/
/*+  Input/Output          :    OHA_HDB_PTR_TYPE             pHDB           +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+                                                                         +*/
/*+  Result               : OHA_OK                                          +*/
/*+                         OHA_ERR_RESOURCE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: register the OHA SubAgent for the LLDP-MIB                +*/
/*+               (DNS_OPC_SET_SERVER)                                      +*/
/*+                                                                         +*/
/*+               Allocates RQB and does DNS-Request                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_DNSSet(
    OHA_HDB_DNS_PTR_TYPE    pHDB,
    LSA_UINT16              Orginator,
    OHA_IP_TYPE            *pIpAddrList)
{
    OHA_DNS_LOWER_RQB_PTR_TYPE    pRQB;
    LSA_SYS_PTR_TYPE              pSys;
    LSA_UINT16                    Status;
    LSA_USER_ID_TYPE              UserId;
    DNS_SERVER_PTR_TYPE           Server;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSSet(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pIpAddrList);

    Status = OHA_OK;
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* allocate RQB */
    OHA_DNS_ALLOC_LOWER_RQB(&pRQB,UserId,sizeof(DNS_RQB_TYPE),pSys);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        LSA_UINT16 i;

        OHA_DNS_RQB_SET_OPCODE(pRQB,DNS_OPC_SET_SERVER);
        OHA_DNS_RQB_SET_HANDLE(pRQB,pHDB->DnsHandle);
        OHA_DNS_RQB_SET_USERID_UVAR16(pRQB,Orginator);

        OHA_LOWER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: DNS-Request: DNS_OPC_SET_SERVER (Handle: 0x%X,DNSHandle: 0x%X,pRQB: 0x%X)",pHDB->ThisHandle,pHDB->DnsHandle,pRQB);

        Server = &pRQB->args.server;
        Server->udp_port = DNS_SERVER_PORT;

        for(i=0; i < DNS_MAX_SERVER_IP_CNT; i++)
        {
            OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr = (OHA_COMMON_MEM_U8_PTR_TYPE)pIpAddrList;

            Server->ip_addr[i].S_un.S_un_b.s_b1 = *pIpAddr;
            Server->ip_addr[i].S_un.S_un_b.s_b2 = *(pIpAddr+1);
            Server->ip_addr[i].S_un.S_un_b.s_b3 = *(pIpAddr+2);
            Server->ip_addr[i].S_un.S_un_b.s_b4 = *(pIpAddr+3);

            pIpAddrList++;
        }

        OHA_DNS_REQUEST_LOWER(pRQB,pSys);
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"DNS-Allocating lower RQB-memory failed!");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DNSSet(Status: 0x%X)",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSSetDone                              +*/
/*+  Input                      OHA_DNS_LOWER_RQB_PTR_TYPE pRQB             +*/
/*+                             OHA_HDB_PTR_TYPE           pHDB             +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Register Agent for the LLDP-MIB done handling             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DNSSetDone(
    OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DNS_PTR_TYPE        pHDB)
{
    LSA_UINT16              Response;
    LSA_UINT16              RetVal;
    LSA_UINT16              Orginator;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSSetDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: DNS_OPC_REGISTER_AGENT done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DNS_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_DNS_RQB_GET_USERID_UVAR16(pRQB);

    switch (OHA_DNS_RQB_GET_RESPONSE(pRQB))
    {
        case DNS_RSP_OK:
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
            OHA_ASSERT (DNS_OPC_SET_SERVER == OHA_DNS_RQB_GET_OPCODE(pRQB));
            Response = OHA_OK;
            break;
        case DNS_RSP_ERR_PARAM:
            Response = OHA_ERR_PARAM;
            break;
        case DNS_RSP_ERR_SEQUENCE:
            Response = OHA_ERR_SEQUENCE;
            break;
        case DNS_RSP_ERR_RESOURCE:
            Response = OHA_ERR_RESOURCE;
            break;
        case DNS_RSP_OK_WITHDRAW:
            Response = OHA_OK_CANCEL;
            break;
        default:
            OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"DNS-ERROR: DNS_OPC_SET_SERVER failed.(0x%X).",OHA_DNS_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    OHA_DNS_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        /* from the USER (OhaOpcPrmWrite DNSParametersRecord) */
        case OHA_HANDLE_TYPE_USER:
        {
            OHA_HDB_USER_PTR_TYPE pHDBUser;
            OHA_UPPER_RQB_PTR_TYPE pRQBUser;

            /* check the handle */
            if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
            {
                OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            }

            pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(Orginator)));
            OHA_ASSERT(pHDBUser);

            OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->DnsUserReq.pBottom,
                                     pHDBUser->DnsUserReq.pTop,
                                     pRQBUser);

            /* we dont expect LSA_NULL here */
            OHA_ASSERT_NULL_PTR(pRQBUser);

            OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);    /* confirm the user */
        }
        break;
        default:
            OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"OHA_DNSSetDone: Invalid HandleType (0x%X).", OHA_HANDLE_GET_TYPE(Orginator));
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DNSSetDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSGetDone                              +*/
/*+  Input                      OHA_DNS_LOWER_RQB_PTR_TYPE pRQB             +*/
/*+                             OHA_HDB_DNS_PTR_TYPE       pHDB             +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reading of DNS server data                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DNSGetDone(
    OHA_DNS_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DNS_PTR_TYPE        pHDB)
{
    OHA_HDB_USER_PTR_TYPE   pHDBUser;
    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              RetVal;
    LSA_UINT16              Orginator;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DNSGetDone(pRQB: 0x%X,pHDB: 0x%X)",
                          pRQB,pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    OHA_LOWER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: DNS_OPC_GET_SERVER done (Handle: 0x%X,Rsp: 0x%X)",pHDB->ThisHandle,OHA_DNS_RQB_GET_RESPONSE(pRQB));

    Orginator = OHA_DNS_RQB_GET_USERID_UVAR16(pRQB);
	OHA_ASSERT(OHA_HANDLE_GET_TYPE(Orginator) == OHA_HANDLE_TYPE_USER);

    /* check the handle */
    if (OHA_HANDLE_IDX(Orginator) > OHA_CFG_MAX_CHANNELS)
    {
        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
    }

    pHDBUser = OHA_GetUserHDBFromHandle((LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(Orginator)));
    OHA_ASSERT(pHDBUser);

    OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->DnsGetReq.pBottom,
                             pHDBUser->DnsGetReq.pTop,
                             pRQBUser);
    OHA_ASSERT_NULL_PTR(pRQBUser);    /* we don't expect LSA_NULL here */

    switch (OHA_DNS_RQB_GET_RESPONSE(pRQB))
    {
        case DNS_RSP_OK:
			{
            /* ---------------------------------------------*/
            /* Successfully done the request.               */
            /* ---------------------------------------------*/
			/* write the DNSParametersRecord (BlockType = 0x3018) */
			/* BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved (u16) */
			/* LengthServerAddress1 (u32) ServerAddress1 (4 = IPVv4, 16 = IPv6), LengthServerAddress2 (u32) ServerAddress2, ... */

			OHA_UPPER_PRM_DATA_PTR_TYPE pPrmData  = &pRQBUser->args.PrmData;
			OHA_UPPER_MEM_U8_PTR_TYPE record_data = pPrmData->record_data;
			LSA_UINT16 block_length;
			LSA_UINT16 ip_count = 0;

			record_data += (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/);
			block_length = (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/);

			/* If no dns server is set, an empty record (Length=8, Data=30,18,00,04,01,00,00,00) must be set */
			while (ip_count < DNS_MAX_SERVER_IP_CNT && pRQB->args.server.ip_addr[ip_count].s_addr != 0)
			{
				block_length += sizeof(LSA_UINT32);
				OHA_PDU_SET_U32(record_data, sizeof(DNS_IPADDR_TYPE)); /* LengthServerAddress */
				if(block_length > pPrmData->record_data_length)
				{
					Response = OHA_ERR_BUFFER_OVERFLOW;
					OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA Read DnsParamsRecord: Length of Record failed (block_length: 0x%X).", block_length);
					break;
				}
				block_length += sizeof(DNS_IPADDR_TYPE);
				OHA_PDU_SET_MEM(record_data, &pRQB->args.server.ip_addr[ip_count].S_un.S_un_b, sizeof(DNS_IPADDR_TYPE)); /* ServerAddress */
				if(block_length > pPrmData->record_data_length)
				{
					Response = OHA_ERR_BUFFER_OVERFLOW;
					OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Read DnsParamsRecord: Length of Record failed (block_length: 0x%X).", block_length);
					break;
				}
				ip_count++;
			}

			if(Response == OHA_OK)
			{
				if (ip_count) /* if dns servers were set */
				{
					oha_prm_write_header(pRQBUser->args.PrmData.record_data, OHA_BLOCK_TYPE_DNS_PARAMETERS,
										block_length,
										OHA_TOPO_DATA_BLOCK_VERSION, LSA_TRUE);

					pRQBUser->args.PrmData.record_data_length = block_length;
					pPrmData->mk_rema = OHA_REMANENT_NO;
				}
				else
				{
					pRQBUser->args.PrmData.record_data_length = 0; /* if no dns servers were set, record length shall be 0 */
					pPrmData->mk_rema = OHA_REMANENT_NO;
				}
			}
			}
            break;
        case DNS_RSP_ERR_SEQUENCE:
            Response = OHA_ERR_SEQUENCE;
            break;
        default:
            OHA_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"DNS-ERROR: DNS_OPC_GET_SERVER failed.(0x%X).",OHA_DNS_RQB_GET_RESPONSE(pRQB));
            Response = OHA_ERR_LL;
            break;
    }

    if (Response != OHA_OK)
    {
        OHA_PROGRAM_TRACE_01(pHDBUser->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"SOCK-ERROR: DNS_OPC_GET_SERVER failed.(0x%X).",OHA_DNS_RQB_GET_RESPONSE(pRQB));
    }

    OHA_DNS_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);    /* confirm the user */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DNSGetDone(Response: 0x%X)", Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DNSGetHandle                            +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_DNS_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a DNS HDB. If none is found               +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_DNS_PTR_TYPE OHA_DNSGetHandle(
    LSA_VOID)
{
    OHA_HDB_DNS_PTR_TYPE  pHDB = OHA_GET_HDNS_PTR();
    LSA_BOOL              Found;

    OHA_ASSERT_NULL_PTR(pHDB);

    /* we have only one DNS channel!  */
    if ( pHDB->DnsState == OHA_HANDLE_STATE_OPEN )
    {
        Found = LSA_TRUE;
    }
    else
    {
        Found = LSA_FALSE;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DNSGetHandle(pHDB: 0x%X, DnsState: 0x%X)", pHDB, pHDB->DnsState);

    if ( ! Found ) pHDB = LSA_NULL;

    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_PrmDNSData                          +*/
/*+  Input/Output          :    DNSParametersRecord                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get server addresses from a DNSParameters-record          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_Get_PrmDNSData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_IP_TYPE                 *pIpAddrList,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength, /* length of a single IP */
    LSA_UINT32                  *err_offset)
{
    LSA_UINT32      index  = 0;
    LSA_UINT16      RetVal = OHA_OK;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_PrmDNSData(pRecData: 0x%X)", pRecData);

    OHA_ASSERT_NULL_PTR(pRecord);
    OHA_ASSERT_NULL_PTR(pIpAddrList);
    OHA_ASSERT_NULL_PTR(err_offset);

	if (OHA_IS_NOT_NULL(pLength))
	{
		*pLength = 0;
	}

    *err_offset = 0;

	if (RecordLen < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/)) /* no entry at minimum */
	{
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: RecordLen (0x%X) too small.", RecordLen);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* IPSuiteBlock: BlockType = 0x3018	BlockHeader, Reserved, LengthServerAddress1 (u32) ServerAddress1 (4 = IPVv4, 16 = IPv6) */
	/*                                                         LengthServerAddress2 (u32) ServerAddress2, ... */
	{
		LSA_UINT16 BlockType;
		LSA_UINT16 BlockLength;
		LSA_UINT16 BlockVersion;
		LSA_UINT16 Reserved;
		LSA_UINT32 ServerAddrLength;

		/* check the BlockHeader: */
		/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8), Reserved (u16) */
		index += 2;
		if (index>RecordLen) {*err_offset = index - 2; return DnsDataFailed(*err_offset);}
		OHA_PDU_GET_U16 (BlockType, pRecord);
		index += 2;
		if (index>RecordLen) {*err_offset = index - 2; return DnsDataFailed(*err_offset);}
		OHA_PDU_GET_U16 (BlockLength, pRecord);
		index += 2;
		if (index>RecordLen) {*err_offset = index - 2; return DnsDataFailed(*err_offset);}
		OHA_PDU_GET_U16 (BlockVersion, pRecord);
		index += 2;
		if (index>RecordLen) {*err_offset = index - 2; return DnsDataFailed(*err_offset);}
		OHA_PDU_GET_U16 (Reserved, pRecord);

		/* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
		if (BlockLength < (OHA_TOPO_BLOCK_HEADER_PDU_SIZE + 2/*Reserved*/ - 2*sizeof(LSA_UINT16)))
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: BlockHeader (BlockLength) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
			*err_offset = index - 2 - 2 - 2; /* BlockLength */
			return(RetVal);
		}

		if (BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: BlockHeader(BlockVersion) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
			*err_offset = index - 2 - 2; /* BlockVersion */
			return(RetVal);
		}

		if (Reserved != 0)
		{
			RetVal = OHA_ERR_PRM_DATA;
			OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: BlockHeader(Reserved) failed.");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
			*err_offset = index - 2; /* Reserved */
			return(RetVal);
		}

		switch (BlockType)
		{
			case OHA_BLOCK_TYPE_DNS_PARAMETERS:
			{
				LSA_UINT16      i;

				for(i=0; index < RecordLen; i++)
				{
					/* LengthServerAddress */
					index += 4;
					if (index>RecordLen) {*err_offset = index - 2; return DnsDataFailed(*err_offset);}
					OHA_PDU_GET_U32 (ServerAddrLength, pRecord);

					/* LengthServerAddress shall be coded as data type Unsigned32 and shall contain the length of the server address (4 = IPVv4, 16 = IPv6) */
					if (ServerAddrLength != OHA_IP_ADDR_SIZE)
					{
						RetVal = OHA_ERR_PRM_DATA;
						OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: LengthServerAddress1 (%u) not valid or not supported.", ServerAddrLength);
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
						*err_offset = index;
						return(RetVal);
					}
					if (OHA_IS_NOT_NULL(pLength))
					{
						*pLength = (LSA_UINT16)ServerAddrLength;
					}

					/* ServerAddress */
					index += OHA_IP_ADDR_SIZE;
					if (index>RecordLen) {*err_offset = index - OHA_IP_ADDR_SIZE; return DnsDataFailed(*err_offset);}

					if (i < DNS_MAX_SERVER_IP_CNT)
					{
						OHA_MEMCPY(pIpAddrList, pRecord, OHA_IP_ADDR_SIZE);
//VE					*ppIpAddr_1 = pRecord;
						pIpAddrList++;
					}

					pRecord += OHA_IP_ADDR_SIZE;
				}
#if 0
				/* LengthServerAddress2 */
				index += 4;
				if (index>RecordLen) {*err_offset = index - 2; return DnsDataFailed(*err_offset);}
				OHA_PDU_GET_U32 (ServerAddrLength, pRecord);

				/* server address (4 = IPVv4, 16 = IPv6) */
				if (ServerAddrLength != OHA_IP_ADDR_SIZE)
				{
					RetVal = OHA_ERR_PRM_DATA;
					OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: LengthServerAddress2 (%u) not valid or not supported.", ServerAddrLength);
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
					*err_offset = index;
					return(RetVal);
				}

				/* ServerAddress2 */
				index += OHA_IP_ADDR_SIZE;
				if (index>RecordLen) {*err_offset = index - OHA_IP_ADDR_SIZE; return DnsDataFailed(*err_offset);}
				*ppIpAddr_2 = pRecord;
#endif

			}
			break;

			default:
				RetVal = OHA_ERR_PRM_BLOCK;
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: BlockHeader(BlockType: 0x%X) invalid or unknown.", BlockType);
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
				*err_offset = index - 2 - 2- 2 -2; /* BlockType */
			return(RetVal);
		}
	}

    /* RecordLength should be the sum of all the existing BlockLengths */
    if (!(RecordLen == index))
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_PrmDNSData: Length of Record failed (err_offset: 0x%X).", index);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
        *err_offset = index;
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_PrmDNSData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

#endif    /* OHA_CFG_USE_DNS */

/*****************************************************************************/
/*  end of file OHA_DNS.C                                                    */
/*****************************************************************************/

