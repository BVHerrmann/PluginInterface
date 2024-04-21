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
/*  F i l e               &F: snmpx_usr.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX user-interface functions with RQBs         */
/*                                                                           */
/*                          - snmpx_open_channel()                           */
/*                            Release SOCK HDB if error on open channel.     */
/*                            If wrong handle use                            */
/*                                         SNMPX_FATAL(SNMPX_FATAL_ERR_RQB)  */
/*                          - snmpx_request()                                */
/*                          - snmpx_close_channel()                          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  1
#define SNMPX_MODULE_ID      LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_USR */

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

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_open_channel                          +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE      pRQB          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       SNMPX_OPC_OPEN_CHANNEL        +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LSA_UINT16              Response      -                             +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error         -                             +*/
/*+     SNMPX_RQB_ARGS_TYPE     Args.Channel: SNMPX_RQB_OPEN_CHANNEL_TYPE   +*/
/*+                                                                         +*/
/*+     Args.Channel:                                                       +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:       channel-handle of SNMPX (ret) +*/
/*+     LSA_HANDLE_TYPE         handle_upper  channel-handle of user        +*/
/*+     LSA_SYS_PATH_TYPE       sys_path      system-path of channel        +*/
/*+     LSA_VOID                LSA_FCT_PTR (ATTR,                          +*/
/*+                                          snmpx_request_upper_done_ptr)  +*/
/*+                                         (SNMPX_UPPER_RQB_PTR_TYPE pRQB) +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user. This  +*/
/*+                                         is HandleUpper from Args.Channel+*/
/*+     LSA_UINT16              Response    SNMPX_OK                        +*/
/*+                                         SNMPX_ERR_RESOURCE              +*/
/*+                                         SNMPX_ERR_SYS_PATH              +*/
/*+                                         SNMPX_ERR_PARAM                 +*/
/*+                                         ...                             +*/
/*+     SNMPX_RQB_ERROR_TYPE     Error       additional errorcodes from LL  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel                                 +*/
/*+                                                                         +*/
/*+               Within this request the output - macro SNMPX_GET_PATH_INFO+*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+                                                                         +*/
/*+               After a successful open a channel-handle is returned      +*/
/*+               in Args.Channel.Handle for use with further requests.     +*/
/*+                                                                         +*/
/*+  HandleUpper: This handle will be stored within SNMPX and return as     +*/
/*+               handle on request-confirmation (in RQB-header).           +*/
/*+                                                                         +*/
/*+  cbf:         The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-function will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+ SysPath:      The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via SNMPX_GET_PATH_INFO.                   +*/
/*+                                                                         +*/
/*+               This function is most likely done asynchroneous because   +*/
/*+               of calling lower layer open channel (DCP or SOCK)         +*/
/*+               depending on channel type spezified in CDP (detail-ptr)   +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call SNMPX_FATAL(SNMPX_FATAL_ERR_RQB)             +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               SNMPX_ERR_PARAM                                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID snmpx_open_channel(SNMPX_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16                Status    = SNMPX_OK;
    LSA_BOOL                  Finish;
    SNMPX_DETAIL_PTR_TYPE     pDetail   = LSA_NULL;
    LSA_UINT16                PathStat  = LSA_RET_OK;
    /* SNMPX_HDB_USER_PTR_TYPE  pHDBUser; */
    /* SNMPX_HDB_SOCK_PTR_TYPE  pHDBSock; */
    LSA_BOOL                  PathOpen  = LSA_FALSE;
    LSA_SYS_PTR_TYPE          pSys;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_open_channel(pRQB: 0x%X)",
                            pRQB);
    SNMPX_ENTER();

    SNMPX_ASSERT_NULL_PTR(pRQB);

    /*----------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                   */
    /*----------------------------------------------------------------------------*/

    SNMPX_RQB_SET_HANDLE(pRQB,pRQB->args.Channel.handle_upper); /* set upper handle */

    /*----------------------------------------------------------------------------*/
    /* Checks some parameters                                                     */
    /*----------------------------------------------------------------------------*/

    if ( SNMPX_RQB_GET_OPCODE(pRQB) != SNMPX_OPC_OPEN_CHANNEL )
    {
        SNMPX_RQB_SET_RESPONSE(pRQB, SNMPX_ERR_PARAM);
        Status = SNMPX_ERR_PARAM;
        SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_UNEXP,">>>: snmpx_open_channel(): Invalid RQB-Opcode (0x%X)",SNMPX_RQB_GET_OPCODE(pRQB));
        SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
    }
    else
    {
        SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_OPEN_CHANNEL (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));

        if (pRQB->args.Channel.snmpx_request_upper_done_ptr == 0)
        {
            SNMPX_RQB_SET_RESPONSE(pRQB, SNMPX_ERR_PARAM);
            Status = SNMPX_ERR_PARAM;
            SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"snmpx_open_channel(): Cbf is NULL");
			SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
        }
    }

    /*----------------------------------------------------------------------------*/
    /* if all ok handle request                                                   */
    /*----------------------------------------------------------------------------*/
    if (Status == SNMPX_OK)
    {
        Finish = LSA_TRUE;

        /*------------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                                */
        /*------------------------------------------------------------------------*/

        SNMPX_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQB->args.Channel.sys_path);

        SNMPX_SYSTEM_TRACE_00(LSA_TRACE_LEVEL_NOTE,"-->: Calling SNMPX_GET_PATH_INFO");

        if ( PathStat != LSA_RET_OK)
        {
            SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
            Status = SNMPX_ERR_SYS_PATH;
        }
        else
        {
            SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_NOTE,"<--: SNMPX_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);

            if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
            {
                Status = SNMPX_ERR_PARAM;
            }
            else
            {
                PathOpen = LSA_TRUE;
            }
        }

        /*------------------------------------------------------------------------*/
        /* Get Infos from Detail-Ptr (CDB) and start handling..                   */
        /*------------------------------------------------------------------------*/

        if ( Status == SNMPX_OK )
        {
            SNMPX_ASSERT_NULL_PTR(pDetail);
            switch (pDetail->PathType)
            {
                    /* ---------------------------------------------------------------*/
                    /* User-Channel.                                                  */
                    /* ---------------------------------------------------------------*/
#ifdef SNMPX_CFG_SNMP_MANAGER
                case SNMPX_PATH_TYPE_MANAGER:
#endif
#ifdef SNMPX_CFG_SNMP_AGENT
                case SNMPX_PATH_TYPE_AGENT:
#endif
                    {
                        SNMPX_HDB_SOCK_PTR_TYPE  pHDBSock = SNMPX_GET_HSOCK_PTR();   /* a sock channel must be opened */

                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"Opening USER-Channel");

                        if (!(snmpx_is_not_null(pHDBSock) && (pHDBSock->SockState != SNMPX_HANDLE_STATE_FREE) && snmpx_is_not_null(pHDBSock->Params.pDetail)))
                        {
                            Status = SNMPX_ERR_SEQUENCE;
                            /* Error on getting SOCK-Channel. Abort. */
                            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"Open SNMPX USER-channel failed. No SOCK-Channel present");
                        }
                        else if (pDetail->PathType == SNMPX_PATH_TYPE_MANAGER && pDetail->Params.Manager.TimeoutInterval == 0)
                        {
                            Status = SNMPX_ERR_PARAM;
                            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"Open SNMPX USER-channel failed. TimeoutInterval 0 is invalid.");
                        }
                        else
                        {
                            SNMPX_HDB_USER_PTR_TYPE pHDBUser = LSA_NULL;
                            /* Get a free, initialized HDB */
                            Status = SNMPX_UserGetFreeHDB(&pHDBUser);

                            if ( Status == SNMPX_OK )
                            {
                                SNMPX_ASSERT_NULL_PTR(pHDBUser);

                                pHDBUser->Params.pSys          = pSys;
                                pHDBUser->Params.pDetail       = pDetail;
                                pHDBUser->Params.Cbf           = pRQB->args.Channel.snmpx_request_upper_done_ptr;
                                pHDBUser->Params.SysPath       = pRQB->args.Channel.sys_path;
                                pHDBUser->Params.HandleUpper   = pRQB->args.Channel.handle_upper;

                                /* set our handle in RQB */
                                pRQB->args.Channel.handle      = pHDBUser->ThisHandle;

                                /* pHDBUser->pOCReq = pRQB; */ /* save pointer to request for later use */
                                pHDBUser->UserState            = SNMPX_HANDLE_STATE_OPENED;
                                pHDBUser->UserReqPendCnt       = 0; /* Number of pend. User-requests */

								if (pDetail->PathType == SNMPX_PATH_TYPE_MANAGER)
								{
									/* Alloc the cyclic 100ms Timer */
									if ( SNMPX_GET_GLOB_PTR()->CyclicTimerID == SNMPX_TIMER_ID_INVALID )
									{
										Status = SNMPX_TimerAlloc(LSA_TIMER_TYPE_CYCLIC,
																  LSA_TIME_BASE_100MS,
																  SNMPX_TIMER_USER_ID_CYLIC,
																  SNMPX_TimerCB,
																  &SNMPX_GET_GLOB_PTR()->CyclicTimerID);

										if ( Status != SNMPX_OK )
										{
											Status = SNMPX_ERR_RESOURCE;
											/* Error on getting timer. Abort. */
											SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_ERROR,"Open SNMPX USER-channel failed. Alloc Timer failed! (RetVal: 0x%X).",Status);
											break;
										}
									}
									SNMPX_GET_GLOB_PTR()->CyclicTimerUserCnt++;
								}
                            }
                            else
                            {
                                /* Error on getting handle. Abort. */
                                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_ERROR,"Open SNMPX USER-channel failed. Cannot get USER handle! (Rsp: 0x%X).",Status);
                            }
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* SOCK-Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case SNMPX_PATH_TYPE_SOCK:
                    SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"Opening SOCK- SystemChannel");

                    /* Check if NIC-ID already used by a SOCK-Channel. If so abort */
                    {
                        SNMPX_HDB_SOCK_PTR_TYPE pHDBSock = LSA_NULL;
                        /* Get a free, initialized HDB */
                        Status = SNMPX_SOCKGetFreeHDB(&pHDBSock);

                        if ( Status == SNMPX_OK )
                        {
                            SNMPX_ASSERT_NULL_PTR(pHDBSock);

                            pHDBSock->Params.pSys          = pSys;
                            pHDBSock->Params.pDetail       = pDetail;
                            pHDBSock->Params.Cbf           = pRQB->args.Channel.snmpx_request_upper_done_ptr;
                            pHDBSock->Params.SysPath       = pRQB->args.Channel.sys_path;
                            pHDBSock->Params.HandleUpper   = pRQB->args.Channel.handle_upper;

                            /* set our handle in RQB */
                            pRQB->args.Channel.handle     = pHDBSock->ThisHandle;

                            /* Do a SOCK- open channel request */
                            Status = SNMPX_SOCKOpenChannel(pHDBSock->ThisHandle,pHDBSock->Params.SysPath,pHDBSock->Params.pSys);

                            if ( Status == SNMPX_OK )
                            {
                                /* Request is underway now.. */
                                pHDBSock->SockState  = SNMPX_HANDLE_STATE_OPEN_SOCK_RUNNING;
                                pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish           = LSA_FALSE;
                            }
                            else
                            {
                                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Open SNMPX-SOCK channel failed. (Rsp: %Xh)",Status);
                                SNMPX_SOCKReleaseHDB(pHDBSock);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_ERROR,"Open SNMPX-SOCK channel failed. Cannot get SOCK handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

#if SNMPX_CFG_SNMP_OPEN_BSD
				/* -------------
				 * OHA-Channel
				 * ------------- */
				case SNMPX_PATH_TYPE_OHA:

					SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_LOW,"Opening OHA- SystemChannel");

					{
					SNMPX_HDB_OHA_PTR_TYPE pHDBOha = LSA_NULL;

					Status = SNMPX_OHAGetFreeHDB(&pHDBOha);

					if ( Status == SNMPX_OK )
					{
						pHDBOha->Params.pSys = pSys;
						pHDBOha->Params.pDetail = pDetail;
						pHDBOha->Params.Cbf = pRQB->args.Channel.snmpx_request_upper_done_ptr;
						pHDBOha->Params.SysPath = pRQB->args.Channel.sys_path;
						pHDBOha->Params.HandleUpper = pRQB->args.Channel.handle_upper;

						Status = SNMPX_OHAOpenChannel(pHDBOha->ThisHandle
													 , pHDBOha->Params.SysPath
													 , pHDBOha->Params.pSys
													 );

						if ( Status == SNMPX_OK )
						{
							pHDBOha->pOCReq = pRQB;
							Finish = LSA_FALSE;
						}
						else
						{
							SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Open SNMPX-OHA channel failed. (Rsp: %Xh)",Status);
						}
					}
					else
					{
						SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_ERROR,"Open SNMPX-OHA channel failed. Handle allocation failed (Rsp: %Xh).", Status);
					}
					}
					break;
#endif
                default:
                    SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"Unknown PathType in CDB (Type: %Xh)",pDetail->PathType);
                    Status = SNMPX_ERR_PARAM;
                    break;
            }/* switch */
        }

		/*----------------------------------------------------------------------------*/
		/* Should we finish the request ?                                             */
		/*----------------------------------------------------------------------------*/
		if ( Finish )  /* finish the request ? */
		{
			/* upper - handler already set */
			SNMPX_RQB_SET_RESPONSE(pRQB,Status);
			SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),SNMPX_RQB_GET_HANDLE(pRQB),Status);
			SNMPX_CallCbf(pRQB->args.Channel.snmpx_request_upper_done_ptr,pRQB,pSys);
		}

        /*----------------------------------------------------------------------------*/
        /* On error, if Syspath was opened we close the path                          */
        /*----------------------------------------------------------------------------*/
        if ( ( Status != SNMPX_OK ) &&  ( PathOpen)  )
        {
	        SNMPX_SYSTEM_TRACE_00(LSA_TRACE_LEVEL_NOTE,"-->: Calling SNMPX_RELEASE_PATH_INFO");

	        SNMPX_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);

			if ( PathStat != LSA_RET_OK)
			{
				SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_RELEASE_PATH_INFO failed (Rsp: 0x%X).",PathStat);
				SNMPX_FATAL(SNMPX_FATAL_ERR_RELEASE_PATH_INFO);
			}
        }
    }

    SNMPX_EXIT();

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_open_channel()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_close_channel                         +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE      pRQB          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       SNMPX_OPC_CLOSE_CHANNEL       +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LSA_UINT16              Response      -                             +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error         -                             +*/
/*+     SNMPX_RQB_ARGS_TYPE     Args          -                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user.       +*/
/*+     LSA_UINT16              Response    SNMPX_OK                        +*/
/*+                                         SNMPX_ERR_RESOURCE              +*/
/*+                                         SNMPX_ERR_PARAM                 +*/
/*+                                         ...                             +*/
/*+     SNMPX_RQB_ERROR_TYPE     Error       additional errorcodes from LL  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes a channel.                                         +*/
/*+               Close lower-layer channel if neccessary. (depends on      +*/
/*+               channel type). Releases Path-info. Note that this         +*/
/*+               function is done asynchron because of calling lower layer +*/
/*+               functions with callback. On error we may finish synchron. +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call SNMPX_FATAL(SNMPX_FATAL_ERR_RQB)             +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               SNMPX_ERR_PARAM                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID snmpx_close_channel(SNMPX_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16                Status;
    LSA_HANDLE_TYPE           Handle;
    LSA_BOOL                  Finish;
    LSA_HANDLE_TYPE           HandleUpper;
    SNMPX_HDB_USER_PTR_TYPE   pHDBUser;
    LSA_UINT16                PathStat  = LSA_RET_OK;
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;

    SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE Cbf;
	LSA_SYS_PTR_TYPE          pSys;
	SNMPX_DETAIL_PTR_TYPE     pDetail;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_close_channel(pRQB: 0x%X)",
                            pRQB);
    SNMPX_ENTER();

    SNMPX_ASSERT_NULL_PTR(pRQB);


    /*---------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                  */
    /*---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    if ( SNMPX_RQB_GET_OPCODE(pRQB) != SNMPX_OPC_CLOSE_CHANNEL )
    {
        SNMPX_RQB_SET_RESPONSE(pRQB, SNMPX_ERR_PARAM);
        Status = SNMPX_ERR_PARAM;
        SNMPX_UPPER_TRACE_02(LSA_TRACE_LEVEL_UNEXP,">>>: snmpx_close_channel(): Invalid RQB-Opcode (0x%X), Response (0x%X)",SNMPX_RQB_GET_OPCODE(pRQB),Status);
		SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
    }
    else
    {
        SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_CLOSE_CHANNEL (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));

        /*-----------------------------------------------------------------------*/
        /* Check if it is allowed to close this channel.                         */
        /*-----------------------------------------------------------------------*/

        Handle = SNMPX_RQB_GET_HANDLE(pRQB);

        switch ( SNMPX_HANDLE_GET_TYPE(Handle))
        {
                /* ------------------------------------------------------------------*/
                /* USER Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case SNMPX_HANDLE_TYPE_USER:
                pHDBUser = SNMPX_GetUserHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser, LSA_NULL) )
                {
					LSA_BOOL Release = LSA_FALSE;	/* AP01206813 */

                    Finish      = LSA_TRUE;
                    Cbf         = pHDBUser->Params.Cbf;
                    HandleUpper = pHDBUser->Params.HandleUpper;
					pSys        = pHDBUser->Params.pSys;
                    pDetail     = pHDBUser->Params.pDetail;

                    SNMPX_RQB_SET_HANDLE(pRQB, HandleUpper );

                    /* only if no open/close request in progress */
                    if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pOCReq, LSA_NULL))
                    {
                        SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Close channel failed. Open or Close already running");
                        Status = SNMPX_ERR_SEQUENCE;
                    }
                    else if (pHDBUser->UserReqPendCnt != 0)
                    {
                        SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Close channel failed. User requests pending");
                        Status = SNMPX_ERR_SEQUENCE;
                    }
                    else
                    {
                        Status = SNMPX_DoUserCloseChannel(pHDBUser,&Finish);
                        if ( ! Finish )
                        {
                            pHDBUser->UserState = SNMPX_HANDLE_STATE_CLOSE_USER_RUNNING;
                            pHDBUser->pOCReq = pRQB; /* save RQB for later use */
                            break;
                        }

                        if (Status == SNMPX_OK)
                        {
							/* stop the cyclic timer if there's the CheckInterval in use */
							if (pDetail->PathType == SNMPX_PATH_TYPE_MANAGER &&
								SNMPX_GET_GLOB_PTR()->CyclicTimerID != SNMPX_TIMER_ID_INVALID) /* timer started */
							{
								/* cancel cyclic timer if active */
								(LSA_VOID)SNMPX_CancelCyclicTimer(pHDBUser, &Finish);
							}

							if ( ! Finish )
							{
								pHDBUser->UserState = SNMPX_HANDLE_STATE_CLOSE_WAIT_TIMER;
								pHDBUser->pOCReq = pRQB; /* save RQB for later use */
								break;
							}
						}

                        if (Status == SNMPX_OK)
                        {
                            /* first Cancel the ReceiveRQB's, then close the channel ! */
                            Status = SNMPX_UserCancelReceive(pHDBUser);
                            if (Status != SNMPX_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);

                            /* we release the HDB. Handle close now        */
                            SNMPX_UserReleaseHDB(pHDBUser);

							Release = LSA_TRUE;	/* AP01206813 */
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						SNMPX_RQB_SET_RESPONSE(pRQB,Status);
						SNMPX_RQB_SET_HANDLE(pRQB, HandleUpper );
						SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),SNMPX_RQB_GET_HANDLE(pRQB),Status);
						SNMPX_CallCbf(Cbf,pRQB,pSys);
					}
					if ( Release )		/* AP01206813 */
					{
                        /* release PATH-Info ! */
						SNMPX_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);
						if (PathStat != LSA_RET_OK ) SNMPX_FATAL(SNMPX_FATAL_ERR_RELEASE_PATH_INFO);
					}
                }
                else
                {
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Invalid SNMPX-User Handle! (0x%X)",Handle);
                    Status = SNMPX_ERR_PARAM;
                    SNMPX_RQB_SET_RESPONSE(pRQB, Status);
					SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
                }

                break;

                /* ------------------------------------------------------------------*/
                /* SOCK-Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case SNMPX_HANDLE_TYPE_SOCK: /* SOCK not supported yet */

                pHDBSock = SNMPXGetSockHDBFromHandle(Handle);

                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSock, LSA_NULL) )
                {
                    Finish      = LSA_TRUE;
                    Cbf         = pHDBSock->Params.Cbf;
                    HandleUpper = pHDBSock->Params.HandleUpper;
					pSys        = pHDBSock->Params.pSys;

                    SNMPX_RQB_SET_HANDLE(pRQB, HandleUpper );

                    if ( SNMPX_Data.MMgm.UsedUserCnt ) /* some user-channel opened ? */
                    {
                        SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Close channel failed. Channel still in use by user-channels");
                        Status = SNMPX_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBSock->pOCReq, LSA_NULL))
                        {
                            SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Close channel failed. Close already running.");
                            Status = SNMPX_ERR_SEQUENCE;
                        }
                        else
                        {
                            /* a Stop Agent is done - close the channel */
                            if (pHDBSock->AgntPortState == SNMPX_SNMP_STATE_CLOSE)
                            {
                                Status = SNMPX_SOCKCloseChannel(pHDBSock->SockHandle,pHDBSock->Params.pSys);
                                if ( Status == SNMPX_OK )
                                {
                                    /* Request is underway now.. */
                                    pHDBSock->SockState  = SNMPX_HANDLE_STATE_CLOSE_SOCK_RUNNING;
                                    pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                                    /* pHDBSock->State  = SNMPX_HANDLE_STATE_CLOSE_PORT_RUNNING; */
                                    Finish = LSA_FALSE;
                                }
                                else
                                {
                                    /* is this a fatal error ?                      */
                                    SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Cannot Close Channel(Status:0x%X).", Status);
                                }
                            }
                            else /* if there's no Stop Agent done close the UDP port */
                            {
                                Status = SNMPX_SOCKClosePort(pHDBSock, SNMPX_HANDLE_TYPE_SOCK, pHDBSock->SockFD, 0xFFFF);
                                if ( Status == SNMPX_OK )
                                {
                                    /* Request is underway now.. */
                                    pHDBSock->SockState  = SNMPX_HANDLE_STATE_CLOSE_PORT_RUNNING;
                                    pHDBSock->pOCReq = pRQB; /* save pointer to request for later use */
                                    Finish = LSA_FALSE;
                                }
                                else
                                {
                                    /* is this a fatal error ?                      */
                                    SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"Cannot Close SOCKET(Status:0x%X).", Status);
                                }
                            }
                        }
                    }

					/*---------------------------------------------------------------*/
					/* Finish the request.                                           */
					/*---------------------------------------------------------------*/
					if ( Finish )
					{
						SNMPX_RQBSetResponse(pRQB,Status);
						SNMPX_RQB_SET_HANDLE(pRQB, HandleUpper );
						SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),SNMPX_RQB_GET_HANDLE(pRQB),Status);
						SNMPX_CallCbf(Cbf,pRQB,pSys);
					}
                }
                else
                {
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Invalid SNMPX-SOCK Handle! (0x%X)",Handle);
                    Status = SNMPX_ERR_PARAM;
                    SNMPX_RQB_SET_RESPONSE(pRQB, Status);
					SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
                }
                break;

#if SNMPX_CFG_SNMP_OPEN_BSD
			/* -------------
			 * OHA-Channel
			 * ------------- */
			case SNMPX_HANDLE_TYPE_OHA:
				{
				SNMPX_HDB_OHA_PTR_TYPE pOha = SNMPXGetOHAHDBFromHandle(Handle);

				if (! LSA_HOST_PTR_ARE_EQUAL(pOha, LSA_NULL) )
				{
					Finish      = LSA_TRUE;
					Cbf = pOha->Params.Cbf;
					HandleUpper = pOha->Params.HandleUpper;
					pSys = pOha->Params.pSys;

					if (! LSA_HOST_PTR_ARE_EQUAL(pOha->pOCReq, LSA_NULL))
					{
						SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Close channel failed. Close already running.");
						Status = SNMPX_ERR_SEQUENCE;
					}
					else
					{
						Status = SNMPX_OHACloseChannel(pOha->OhaHandle, pOha->Params.pSys);

						if ( Status == SNMPX_OK )
						{
							pOha->pOCReq = pRQB;
							Finish = LSA_FALSE;
						}
					}
					if (Finish)
					{
						SNMPX_RQBSetResponse(pRQB,Status);
						SNMPX_RQB_SET_HANDLE(pRQB, HandleUpper );
						SNMPX_UPPER_TRACE_03(LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",SNMPX_RQB_GET_OPCODE(pRQB),SNMPX_RQB_GET_HANDLE(pRQB),Status);
						SNMPX_CallCbf(Cbf,pRQB,pSys);
					}
				}
				else
				{
					SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Invalid SNMPX-OHA Handle! (0x%X)", Handle);
					Status = SNMPX_ERR_PARAM;
					SNMPX_RQB_SET_RESPONSE(pRQB, Status);
					SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
				}
				}
				break;
#endif
			default:
                SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,"snmpx_close_channel(): Invalid Handle (0x%X)",Handle);
                Status = SNMPX_ERR_PARAM;
                SNMPX_RQB_SET_RESPONSE(pRQB, Status);
				SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
                break;
        }
    }

    SNMPX_EXIT();

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: snmpx_close_channel()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_request                               +*/
/*+  Input/Output          :    SNMPX_UPPER_RQB_PTR_TYPE     pRQB           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       varies:                       +*/
/*+                                           SNMPX_OPC_REGISTER_AGENT      +*/
/*+                                           SNMPX_OPC_UNREGISTER_AGENT    +*/
/*+                                           SNMPX_OPC_PROVIDE_AGENT       +*/
/*+                                           SNMPX_OPC_RECEIVE_AGENT       +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LSA_UINT16              Response      -                             +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error         -                             +*/
/*+     SNMPX_RQB_ARGS_TYPE     Args          varies depending on opcode    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     upper-handle from open_channel  +*/
/*+     LSA_UINT16              Response    SNMPX_OK                        +*/
/*+                                         SNMPX_ERR_RESOURCE              +*/
/*+                                         SNMPX_ERR_PARAM                 +*/
/*+                                         ...                             +*/
/*+     SNMPX_RQB_ERROR_TYPE     Error       additional errorcodes from LL  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given on open_channel-request.             +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call SNMPX_FATAL(SNMPX_FATAL_ERR_RQB)             +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               SNMPX_ERR_PARAM                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID snmpx_request(SNMPX_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_HANDLE_TYPE          Handle;
    SNMPX_HDB_USER_PTR_TYPE    pHDB;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : snmpx_request(pRQB: 0x%X)",
                            pRQB);
    SNMPX_ENTER();

    SNMPX_ASSERT_NULL_PTR(pRQB);

    Handle   = SNMPX_RQB_GET_HANDLE(pRQB);

    /* TIMER-Request needs special handling  */
    if ( SNMPX_RQB_GET_OPCODE(pRQB) == SNMPX_OPC_TIMER )
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_CHAT,">>>: Request: SNMPX_OPC_TIMER (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
        SNMPX_RequestTimer(pRQB);

        SNMPX_EXIT();
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_request()");
        return;
    }

    pHDB = SNMPX_GetUserHDBFromHandle(Handle);

    if (! LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        /* no open/close runnning ? and channel open ?                    */
        /* Note: If a close-channel failed within some closing state. The */
        /*       channel can not be used anymore for this user - requests */
        /*       The channel is in some inoperable state and it can only  */
        /*       be tried to do another close-request..                   */

        if ( (pHDB->UserState == SNMPX_HANDLE_STATE_OPENED) &&
             LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) )
        {
            switch ( SNMPX_RQB_GET_OPCODE(pRQB))
            {
#ifdef SNMPX_CFG_SNMP_AGENT
                case SNMPX_OPC_REGISTER_AGENT:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_REGISTER_AGENT (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestRegisterAgent(pRQB, pHDB);
                    break;
                case SNMPX_OPC_UNREGISTER_AGENT:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_UNREGISTER_AGENT (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestUnregisterAgent(pRQB, pHDB);
                    break;
                case SNMPX_OPC_PROVIDE_AGENT:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_PROVIDE_AGENT (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestOidProvide(pRQB, pHDB);
                    break;
                case SNMPX_OPC_RECEIVE_AGENT:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_RECEIVE_AGENT (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestOidReceive(pRQB, pHDB);
                    break;
#endif
#ifdef SNMPX_CFG_SNMP_MANAGER
                case SNMPX_OPC_OPEN_MANAGER_SESSION:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_OPEN_MANAGER_SESSION (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestOpenSession(pRQB, pHDB);
                    break;
                case SNMPX_OPC_CLOSE_MANAGER_SESSION:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_CLOSE_MANAGER_SESSION (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestCloseSession(pRQB, pHDB);
                    break;
                case SNMPX_OPC_REQUEST_MANAGER:
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_NOTE,">>>: Request: SNMPX_OPC_REQUEST_MANAGER (Handle: 0x%X)",SNMPX_RQB_GET_HANDLE(pRQB));
                    SNMPX_RequestManager(pRQB, pHDB);
                    break;
#endif
                default:
                    SNMPX_RQB_SET_RESPONSE(pRQB, SNMPX_ERR_PARAM);
                    SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,">>>: snmpx_request(): Invalid RQB-Opcode (0x%X)",SNMPX_RQB_GET_OPCODE(pRQB));
					SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
                    break;
            } /* switch */
        }
        else
        {
            SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,">>>: snmpx_request(): Channel not open or Open/Close running (0x%X)",SNMPX_RQB_GET_OPCODE(pRQB));
            SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        }
    }
    else
    {
        SNMPX_RQB_SET_RESPONSE(pRQB, SNMPX_ERR_PARAM);
        SNMPX_UPPER_TRACE_01(LSA_TRACE_LEVEL_ERROR,">>>: snmpx_request(): Invalid handle (0x%X)",Handle);
		SNMPX_FATAL(SNMPX_FATAL_ERR_RQB);
    }

    SNMPX_EXIT();
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: snmpx_request()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_DoUserCloseChannel                    +*/
/*+  Input/Output          :    SNMPX_HDB_USER_PTR_TYPE        pHDB         +*/
/*+                             SNMPX_LOCAL_MEM_BOOL_PTR_TYPE pFinish       +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             SNMPX_ERR_SEQUENCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                : User HDB                                         +*/
/*+  pFinish             : Output: LSA_TRUE : Finish the request            +*/
/*+                                LSA_FALSE: Waiting for any RQB           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes User-Handle.                                       +*/
/*+               If any User RQB is underway we have to wait for the RQB   +*/
/*+               arrives so we go into Wait-for-State and leave with       +*/
/*+               pFinish = LSA_FALSE.                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_DoUserCloseChannel(
    SNMPX_HDB_USER_PTR_TYPE        pHDBUser,
    SNMPX_LOCAL_MEM_BOOL_PTR_TYPE  pFinish)
{
    LSA_UINT16 Status = SNMPX_OK;
    LSA_UINT16 i;
    LSA_BOOL   Finish = LSA_TRUE;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_DoUserCloseChannel(pHDBUser: 0x%X, pFinish: 0x%X)",
                            pHDBUser, pFinish);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);

    /* are queued user requests existing ? */
    for (i = 0; i < SNMPX_CFG_MAX_MANAGER_SESSIONS; i++)
    {
        if (snmpx_is_not_null(pHDBUser->MngrSession[i].pUserRQB))
        {
            *pFinish = LSA_TRUE;
            Status = SNMPX_ERR_SEQUENCE;
            SNMPX_UPPER_TRACE_00(LSA_TRACE_LEVEL_ERROR,"SNMPX_DoUserCloseChannel(): Close channel failed. Manager user request active");
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                                    "OUT: SNMPX_DoUserCloseChannel(Status: 0x%X)", Status);
            return(Status);
        }
    }

    /* are SOCK resources pending ? */
    for (i = 0; i < SNMPX_CFG_MAX_MANAGER_SESSIONS; i++)
    {
        switch (pHDBUser->MngrSession[i].PortState)
        {
            case SNMPX_HANDLE_STATE_INIT:
            case SNMPX_SNMP_STATE_CLOSE:
                Status = SNMPX_OK;
                break;

            case SNMPX_SNMP_STATE_OPEN:  /* close port is to do         */
            {
                SNMPX_HDB_SOCK_PTR_TYPE pHDBSock = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

                SNMPX_ASSERT_NULL_PTR(pHDBSock);
                Status = SNMPX_SOCKClosePort(pHDBSock, pHDBUser->ThisHandle, pHDBUser->MngrSession[i].SockFD, i);
                if (Status != SNMPX_OK)
                {
                    SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing Manager close port at session (%u) failed.", i);
                }
                else
                {
                    Finish = LSA_FALSE;
                    pHDBUser->MngrSession[i].PortState = SNMPX_SNMP_STATE_WF_CLOSE;
                }
            }
            break;

            case SNMPX_SNMP_STATE_WF_CLOSE: /* CLOSE port is active  */
            case SNMPX_SNMP_STATE_WF_OPEN:  /* OPEN  port is active  */
            case SNMPX_SNMP_STATE_WF_CNF:   /* snmp request is pending */
            default:
                Status = SNMPX_ERR_SEQUENCE;
                SNMPX_UPPER_TRACE_02(LSA_TRACE_LEVEL_ERROR,"SNMPX_DoUserCloseChannel(): Close channel failed. Wrong PortState (0x%X) at index (0x%X)", pHDBUser->MngrSession[i].PortState, i);
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    *pFinish = Finish;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_DoUserCloseChannel(Status: 0x%X)", Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_CheckUserCloseChannel                 +*/
/*+  Input/Output          :    SNMPX_HDB_USER_PTR_TYPE        pHDB         +*/
/*+                             SNMPX_LOCAL_MEM_BOOL_PTR_TYPE pFinish       +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             SNMPX_ERR_SEQUENCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                : User HDB                                         +*/
/*+  pFinish             : Output: LSA_TRUE : Finish the request            +*/
/*+                                LSA_FALSE: Waiting for any RQB           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes User-Handle.                                       +*/
/*+               If any User RQB is underway we have to wait for the RQB   +*/
/*+               arrives so we go into Wait-for-State and leave with       +*/
/*+               pFinish = LSA_FALSE.                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_CheckUserCloseChannel(
    SNMPX_HDB_USER_PTR_TYPE        pHDBUser,
    SNMPX_LOCAL_MEM_BOOL_PTR_TYPE  pFinish)
{
    LSA_UINT16 Status = SNMPX_OK;
    LSA_UINT16 i;
    LSA_BOOL   Finish = LSA_TRUE;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CheckUserCloseChannel(pHDBUser: 0x%X, pFinish: 0x%X)",
                            pHDBUser, pFinish);

    SNMPX_ASSERT_NULL_PTR(pHDBUser);

    /* are queued user requests existing ? */
    for (i = 0; i < SNMPX_CFG_MAX_MANAGER_SESSIONS; i++)
    {
        SNMPX_ASSERT(snmpx_is_null(pHDBUser->MngrSession[i].pUserRQB));
    }

    /* are SOCK resources pending ? */
    for (i = 0; Finish && (i < SNMPX_CFG_MAX_MANAGER_SESSIONS); i++)
    {
        switch (pHDBUser->MngrSession[i].PortState)
        {
            case SNMPX_HANDLE_STATE_INIT:
            case SNMPX_SNMP_STATE_CLOSE:
                Status = SNMPX_OK;
                break;

            case SNMPX_SNMP_STATE_WF_CLOSE: /* (further) CLOSE port is active  */
                Finish = LSA_FALSE;
                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_CHAT,"SOCK Manager close port at session (%u) is active.", i);
                break;

            case SNMPX_SNMP_STATE_OPEN:     /* we are closing, why ? */
            case SNMPX_SNMP_STATE_WF_OPEN:
            default:
                Status = SNMPX_ERR_SEQUENCE;
                SNMPX_UPPER_TRACE_02(LSA_TRACE_LEVEL_ERROR,"SNMPX_CheckUserCloseChannel(): Close channel state failed. Wrong PortState (0x%X) at index (0x%X)", pHDBUser->MngrSession[i].PortState, i);
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                break;
        }
    }

    *pFinish = Finish;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_CheckUserCloseChannel(Status: 0x%X)", Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestRegisterAgent                  +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_REGISTER_AGENT       +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Register                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to register a mib or a mib tree of a subagent     +*/
/*+                                                                         +*/
/*+               Stores the RQB wthin User-Handlemanagement.               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_AGENT
LSA_VOID SNMPX_RequestRegisterAgent(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;
    LSA_UINT16                RetVal;
    SNMPX_UPPER_REGISTER_PTR_TYPE pRegisterEntry = LSA_NULL;
    LSA_USER_ID_TYPE          UserId;
    LSA_SYS_PTR_TYPE          pSys;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestRegisterAgent(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;

    /* Check if we have a SOCK-Channel */
    pHDBSock  = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "No Sock channel: SNMPX_RequestRegisterAgent()");
        return;
    }

    /* check the params */
    if (pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_AGENT)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not an agent channel: SNMPX_RequestRegisterAgent()");
        return;
    }

    if (pRQB->args.Register.OidLen == 0 || pRQB->args.Register.OidLen > SNMPX_MAX_OID_LEN ||
        snmpx_is_null(pRQB->args.Register.pOid))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestRegisterAgent()");
        return;
    }

    /* check the entry of the mib tree of all users (subagents) */
    if (!SNMPX_CheckMibTree(pRQB->args.Register.pOid, pRQB->args.Register.OidLen))
    {
        /* the mib tree is already registered or belongs to the SNMp-group */
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_OPC_REGISTER_AGENT: The mib tree is already registered or belongs to the MIB-II SNMP-group.");
        SNMPX_UserRequestFinish(pHDB,pRQB,LSA_RET_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestRegisterAgent()");
        return;
    }

    /* get a free enty for the mib tree */
    RetVal = SNMPX_UserGetFreeRegisterEntry(pHDB, &pRegisterEntry);
    if (RetVal != SNMPX_OK || snmpx_is_null(pRegisterEntry))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_RESOURCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestRegisterAgent()");
        return;
    }

    /* store the mib tree in the User-HDB */
    pRegisterEntry->Priority = pRQB->args.Register.Priority;
    pRegisterEntry->OidLen   = pRQB->args.Register.OidLen;

    SNMPX_ALLOC_UPPER_MEM((SNMPX_UPPER_MEM_PTR_TYPE)(&pRegisterEntry->pOid), UserId,
                          (LSA_UINT16)(pRegisterEntry->OidLen * sizeof(SNMPX_OID_TYPE)), pSys);
    if (snmpx_is_not_null(pRegisterEntry->pOid))
    {
        SNMPX_OIDCPY(pRegisterEntry->pOid, pRQB->args.Register.pOid, pRegisterEntry->OidLen);
    }
    else
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_RESOURCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ResourceError: SNMPX_RequestRegisterAgent()");
        return;
    }

    SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_OK);
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestRegisterAgent()");
    return;
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestUnregisterAgent                +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_UNREGISTER_AGENT     +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Register                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to unregister a mib or a mib tree of a subagent   +*/
/*+                                                                         +*/
/*+        Stores the RQB wthin User-Handlemanagement                       +*/
/*+        If the last mib or mib tree is unregistered we send the          +*/
/*+        indication ressources (SNMPX_OPC_PROVIDE_AGENT) back to the user.+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_AGENT
LSA_VOID SNMPX_RequestUnregisterAgent(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;
    LSA_UINT16                i, RetVal;
    LSA_USER_ID_TYPE          UserId;
    LSA_SYS_PTR_TYPE          pSys;
    LSA_BOOL                  Found;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestUnregisterAgent(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    pSys   = pHDB->Params.pSys;
    UserId.uvar32 = 0;
    Found = LSA_FALSE;

    /* Check if we have a SOCK-Channel */
    pHDBSock  = SNMPX_GET_HSOCK_PTR();

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "No Sock channel: SNMPX_RequestUnregisterAgent()");
        return;
    }

    /* check the params */
    if (pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_AGENT)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not an agent channel: SNMPX_RequestUnregisterAgent()");
        return;
    }
    if (pRQB->args.Register.OidLen == 0 || pRQB->args.Register.OidLen > SNMPX_MAX_OID_LEN ||
        snmpx_is_null(pRQB->args.Register.pOid))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestUnregisterAgent()");
        return;
    }

    /* get the entry of the mib tree */
    for (i=0; i < SNMPX_CFG_MAX_OIDS; i++)
    {
        if (snmpx_is_not_null(pHDB->RegisteredOids[i].pOid) && (pHDB->RegisteredOids[i].OidLen == pRQB->args.Register.OidLen))
        {
            Found = SNMPX_OIDCMP(pRQB->args.Register.pOid, pHDB->RegisteredOids[i].pOid,
                                 pHDB->RegisteredOids[i].OidLen);
            if (Found)
            {
                break;
            }
        }
    }

    if (Found)
    {
        /* delete the mib tree in the User-HDB */
        SNMPX_FREE_UPPER_MEM(&RetVal, pHDB->RegisteredOids[i].pOid, pSys);
        SNMPX_ASSERT(RetVal == SNMPX_OK);

        pRQB->args.Register.Priority = pHDB->RegisteredOids[i].Priority;
        pHDB->RegisteredOids[i].Priority = 0;
        pHDB->RegisteredOids[i].OidLen   = 0;
        pHDB->RegisteredOids[i].pOid     = LSA_NULL;
    }
    else
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestUnregisterAgent()");
        return;
    }

    SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_OK);
    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestUnregisterAgent()");
    return;
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_UserCancelReceive                     +*/
/*+  Input                      LSA_HANDLE_TYPE       DCPHandle             +*/
/*+                             LSA_SYS_PTR_TYPE      pSys                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                             SNMPX_ERR_RESOURCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : User HDB                                        +*/
/*+  pRQB                 : User RQB (CloseChannel)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Issues Cancel Receive Request to User. The Request will be+*/
/*+               finished by calling the DCP-Callbackfunction.             +*/
/*+               snmpx_dcp_request_lower_done()                            +*/
/*+                                                                         +*/
/*+               Note: Only call if no close-channel request already runs  +*/
/*+                     We dont support multiple close request at a time.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_UserCancelReceive(
    SNMPX_HDB_USER_PTR_TYPE       pHDB)
{
    LSA_UINT16                  Result  = SNMPX_OK;
    SNMPX_UPPER_RQB_PTR_TYPE    pRQBHelp;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserCancelReceive(pHDB: 0x%X)",
                            pHDB);

    /* cancel all pending user-indication resources for SNMPX */
    while (pHDB->UserIndReq.pBottom)
    {
        SNMPX_RQB_REM_BLOCK_BOTTOM(
            pHDB->UserIndReq.pBottom,
            pHDB->UserIndReq.pTop,
            pRQBHelp);

        pHDB->UserIndReqPending--; /* not here, in the callback */

        SNMPX_UserRequestFinish(pHDB,pRQBHelp,SNMPX_OK_CANCEL);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserCancelReceive(Result: 0x%X)", Result);
    return(Result);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestOpenSession                    +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_OPEN_MANAGER_SESSION +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Register                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a manager session (IN = IP, OUT = ID)     +*/
/*+                                                                         +*/
/*+               Stores the RQB wthin User-Handlemanagement.               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_MANAGER
LSA_VOID SNMPX_RequestOpenSession(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;
    LSA_UINT16                RetVal;

    SNMPX_UPPER_SESSION_PTR_TYPE pSession;
    LSA_UINT16                SessionIndex = 0;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestOpenSession(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    pSession = &pRQB->args.Session;

    /* user channel is opened and no close is running */
    if (pHDB->UserState != SNMPX_HANDLE_STATE_OPENED)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_RSP_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Channel is not open (anymore), UserState (%u): SNMPX_RequestOpenSession()", pHDB->UserState);
        return;
    }

    /* Check if we have a SOCK-Channel */
    pHDBSock = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "No Sock channel: SNMPX_RequestOpenSession()");
        return;
    }

    /* params to check */
    if (pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_MANAGER)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not a manager channel: SNMPX_RequestOpenSession()");
        return;
    }

    /* ip 0.0.0.0 is not allowed */
    if (SNMPX_MEMCMP(pSession->IPAddress, (SNMPX_COMMON_MEM_PTR_TYPE)SNMPX_ZERO_IP, SNMPX_IP_ADDR_SIZE))   /* 0.0.0.0 is not allowed */
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_PARAM);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "IP address 0.0.0.0 is not allowed: SNMPX_RequestOpenSession()");
        return;
    }

    /* pReadCommunity  = LSA_NULL means community string = "public"  */
    /* pWriteCommunity = LSA_NULL means community string = "private" */

    /* get a free enty for the session */
    RetVal = SNMPX_UserGetFreeSessionEntry(pHDB, &SessionIndex);
    if (RetVal != SNMPX_OK)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_RESOURCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestOpenSession()");
        return;
    }

    /* store the session parameters in the User-HDB */
    pHDB->MngrSession[SessionIndex].SessionID   = (LSA_UINT16)(SessionIndex + 1); /* Session-ID := SessionIndex + 1 */
    pHDB->MngrSession[SessionIndex].PortState   = SNMPX_SNMP_STATE_WF_OPEN;
    SNMPX_MEMCPY(pHDB->MngrSession[SessionIndex].IPAddress, pSession->IPAddress, SNMPX_IP_ADDR_SIZE);

    /* the read community */
    if (snmpx_is_not_null(pSession->pReadCommunity))
    {
	    SNMPX_LOCAL_MEM_PTR_TYPE MemPtr = LSA_NULL;

        SNMPX_ALLOC_LOCAL_MEM(&MemPtr, sizeof(SNMPX_COMMUNITY_TYPE));
        if (snmpx_is_not_null(MemPtr))
        {
			pHDB->MngrSession[SessionIndex].pReadCommunity = (SNMPX_UPPER_COMMUNITY_PTR_TYPE)MemPtr;

            pHDB->MngrSession[SessionIndex].pReadCommunity->name_len = pSession->pReadCommunity->name_len;
            SNMPX_MEMCPY(pHDB->MngrSession[SessionIndex].pReadCommunity->name,
                         pSession->pReadCommunity->name, pSession->pReadCommunity->name_len);
        }
        else
        {
            (LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDB, SessionIndex);
            SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_RESOURCE);
            SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ResourceError: SNMPX_RequestOpenSession(pReadCommunity)");
            return;
        }
    }

    /* the write community */
    if (snmpx_is_not_null(pSession->pWriteCommunity))
    {
	    SNMPX_LOCAL_MEM_PTR_TYPE MemPtr = LSA_NULL;

        SNMPX_ALLOC_LOCAL_MEM(&MemPtr, sizeof(SNMPX_COMMUNITY_TYPE));
        if (snmpx_is_not_null(MemPtr))
        {
			pHDB->MngrSession[SessionIndex].pWriteCommunity = (SNMPX_UPPER_COMMUNITY_PTR_TYPE)MemPtr;

            pHDB->MngrSession[SessionIndex].pWriteCommunity->name_len = pSession->pWriteCommunity->name_len;
            SNMPX_MEMCPY(pHDB->MngrSession[SessionIndex].pWriteCommunity->name,
                         pSession->pWriteCommunity->name, pSession->pWriteCommunity->name_len);
        }
        else
        {
            (LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDB, SessionIndex);
            SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_RESOURCE);
            SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ResourceError: SNMPX_RequestOpenSession(pWriteCommunity)");
            return;
        }
    }

    /* store the user rqb */
    pSession->SessionID = (LSA_UINT16)(SessionIndex + 1);
    SNMPX_ASSERT(snmpx_is_null(pHDB->MngrSession[SessionIndex].pUserRQB));
    pHDB->MngrSession[SessionIndex].pUserRQB = pRQB;

    /* open sock port */
    /* SOCK_OPC_UDP_OPEN (sin_family = SOCK_AF_INET,
                        sin_port = 0, the UDP stack assigns the port number
                        sin_addr = IPAddress,
                        Timer_on = LSA_TRUE);
        Cnf => sock_fd */

#if 0	/* bind to IP address */
    RetVal = SNMPX_SOCKOpenPort(pHDBSock, pHDB->ThisHandle, pSession->IPAddress, SessionIndex);
#else	/* bind to 0.0.0.0 */
	{
	SNMPX_IP_TYPE  IPAddress = {0,0,0,0};
    RetVal = SNMPX_SOCKOpenPort(pHDBSock, pHDB->ThisHandle, IPAddress, SessionIndex);
	}
#endif
    if ( RetVal != SNMPX_OK)
    {
        (LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDB, SessionIndex);
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing Manager open port failed.");
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_RESOURCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "ParamError: SNMPX_RequestOpenSession()");
        return;
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestOpenSession()");
    return;
}
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestCloseSession                   +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_OPEN_MANAGER_SESSION +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Register                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to close a manager session (IN = SessionID)       +*/
/*+                                                                         +*/
/*+               Stores the RQB wthin User-Handlemanagement.               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_MANAGER
LSA_VOID SNMPX_RequestCloseSession(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;
    LSA_UINT16                SessionIndex;
    LSA_UINT16                Response;

    SNMPX_UPPER_SESSION_PTR_TYPE pSession;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestCloseSession(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    pSession = &pRQB->args.Session;

    /* Check if we have a SOCK-Channel */
    pHDBSock = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "No Sock channel: SNMPX_RequestOpenSession()");
        return;
    }

    /* params to check */
    if (pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_MANAGER)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not a manager channel: SNMPX_RequestCloseSession()");
        return;
    }

    if (pSession->SessionID == 0 || pSession->SessionID > SNMPX_CFG_MAX_MANAGER_SESSIONS)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Invalid SessionID (%u): SNMPX_RequestCloseSession()", pSession->SessionID);
        return;
    }

    SessionIndex = (LSA_UINT16)(pSession->SessionID - 1); /* Index := SessionID - 1 */
    if (snmpx_is_not_null(pHDB->MngrSession[SessionIndex].pUserRQB))
    {
        /* if a RequestManager.Req is active, we handle the CloseSession.Req */
        if (SNMPX_RQB_GET_OPCODE(pHDB->MngrSession[SessionIndex].pUserRQB) != SNMPX_OPC_REQUEST_MANAGER)
        {
            SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_RSP_ERR_SEQUENCE);
            SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_NOTE_HIGH, "User-Request is active, Opcode = (%u), SessionState (%u): SNMPX_RequestCloseSession()",
                                    SNMPX_RQB_GET_OPCODE(pHDB->MngrSession[SessionIndex].pUserRQB), pHDB->MngrSession[SessionIndex].PortState);
            SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestCloseSession()");
            return;
        }
    }

    /* user channel is opened and no close is running */
    switch (pHDB->UserState)
    {
        case SNMPX_HANDLE_STATE_INIT:
        case SNMPX_HANDLE_STATE_FREE:
        case SNMPX_HANDLE_STATE_CLOSED:
            Response = SNMPX_ERR_SEQUENCE;
            break;
        case SNMPX_HANDLE_STATE_CLOSE_USER_RUNNING:
            Response = SNMPX_RSP_ERR_SEQUENCE;
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Channel is not open (anymore), UserState (%u): SNMPX_RequestCloseSession()", pHDB->UserState);
            break;
        case SNMPX_HANDLE_STATE_OPENED: /* sock channel is opened  */
        {
            /* if a RequestManager.Req is active, first we cancel this request and queue the CloseSession.Req */
            SNMPX_UPPER_RQB_PTR_TYPE pQueuedRQB = pHDB->MngrSession[SessionIndex].pUserRQB;

            SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_NOTE_LOW, "SNMPX_RequestCloseSession(Index=%u): PortState is (%u)", SessionIndex, pHDB->MngrSession[SessionIndex].PortState);

            switch (pHDB->MngrSession[SessionIndex].PortState)
            {
                case SNMPX_HANDLE_STATE_INIT:   /* no open */
                    Response = SNMPX_ERR_SEQUENCE;
                    break;
                case SNMPX_SNMP_STATE_CLOSE:    /* not open anymore - o.k. */
                    SNMPX_ASSERT(snmpx_is_null(pQueuedRQB));
                    /* delete the session entry */
                    (LSA_VOID)SNMPX_UserDeleteSessionEntry(pHDB, SessionIndex);
                    Response = SNMPX_OK;
                    break;
                case SNMPX_SNMP_STATE_OPEN:
                    if (snmpx_is_not_null(pQueuedRQB))
                    {
                        SNMPX_UserRequestFinish(pHDB,pQueuedRQB,SNMPX_RSP_ERR_LOC_ABORT);
                        pHDB->MngrSession[SessionIndex].pUserRQB = pRQB;    /* queueing CloseSession */
                        /* Response = SNMPX_OK_ACTIVE; */ /* UDP response is underway */
                        /* break; */
                    }

                    Response = SNMPX_SOCKClosePort(pHDBSock, pHDB->ThisHandle, pHDB->MngrSession[SessionIndex].SockFD, SessionIndex);
                    if (Response != SNMPX_OK)
                    {
                        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK Issuing Manager close port failed.");
                    }
                    else
                    {
                        pHDB->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_WF_CLOSE;
                        pHDB->MngrSession[SessionIndex].pUserRQB  = pRQB;   /* queueing CloseSession */
                        Response = SNMPX_OK_ACTIVE; /* ClosePort is underway */
                    }
                    break;
                case SNMPX_SNMP_STATE_WF_CNF:   /* snmp request is pending */
                    if (snmpx_is_not_null(pQueuedRQB))
                    {
                        SNMPX_UserRequestFinish(pHDB,pQueuedRQB,SNMPX_RSP_ERR_LOC_ABORT);
                        pHDB->MngrSession[SessionIndex].pUserRQB = pRQB;    /* queueing CloseSession */
                        Response = SNMPX_OK_ACTIVE; /* UDP request is underway */
                        break;
                    }
                /* intentionally no break *//*FALLTHROUGH*/
                case SNMPX_SNMP_STATE_WF_OPEN:
                case SNMPX_SNMP_STATE_WF_CLOSE:
                default:
                    Response = SNMPX_ERR_SYNTAX;
                    SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_OPC_CLOSE_MANAGER_SESSION failed (PortState: 0x%X).",pHDB->MngrSession[SessionIndex].PortState);
                    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
        break;
        default:
            Response = SNMPX_ERR_SYNTAX;
            SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_OPC_CLOSE_MANAGER_SESSION failed (UserState: 0x%X).",pHDB->UserState);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    if (Response != SNMPX_OK_ACTIVE)/* SOCK request is'nt underway ? */
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,Response);
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestCloseSession(Response = 0x%X)", Response);
    return;
}
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_RequestManager                        +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      SNMPX_OPC_REQUEST_MANAGER      +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     SNMPX_RQB_ERROR_TYPE    Error        Addition errorcodes from LowerL+*/
/*+     SNMPX_RQB_ARGS_TYPE     args.Register                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Manager request Snmp-Get, Snmp-GetNext (IN = SessionID)   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef SNMPX_CFG_SNMP_MANAGER
LSA_VOID SNMPX_RequestManager(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_HDB_SOCK_PTR_TYPE   pHDBSock;
    LSA_UINT16                SessionIndex;
    LSA_UINT16                Response;

    SNMPX_UPPER_OBJECT_PTR_TYPE pObject;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_RequestManager(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

    SNMPX_ASSERT_NULL_PTR(pRQB);

    pObject = &pRQB->args.Object;


    /* Check if we have a SOCK-Channel */
    pHDBSock = SNMPX_GET_HSOCK_PTR();  /* SOCK for this request necessary */

    if (snmpx_is_null(pHDBSock))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "No Sock channel: SNMPX_RequestManager()");
        return;
    }

    /* params to check */
    if (pHDB->Params.pDetail->PathType != SNMPX_PATH_TYPE_MANAGER)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH, "Not a manager channel: SNMPX_RequestManager()");
        return;
    }

    if (pObject->SessionID == 0 || pObject->SessionID > SNMPX_CFG_MAX_MANAGER_SESSIONS)
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_ERR_REF);
        SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Invalid SessionID (%u): SNMPX_RequestManager()", pObject->SessionID);
        return;
    }

    SessionIndex = (LSA_UINT16)(pObject->SessionID - 1); /* Index := SessionID - 1 */
    if (snmpx_is_not_null(pHDB->MngrSession[SessionIndex].pUserRQB))
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,SNMPX_RSP_ERR_SEQUENCE);
        SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_NOTE_HIGH, "User-Request is active, Opcode = (%u), SessionState (%u): SNMPX_RequestManager()",
                                SNMPX_RQB_GET_OPCODE(pHDB->MngrSession[SessionIndex].pUserRQB), pHDB->MngrSession[SessionIndex].PortState);
        return;
    }

    /* user channel is opened and no close is running ? */
    switch (pHDB->UserState)
    {
        case SNMPX_HANDLE_STATE_INIT:
        case SNMPX_HANDLE_STATE_FREE:
        case SNMPX_HANDLE_STATE_CLOSED:
        case SNMPX_HANDLE_STATE_CLOSE_USER_RUNNING:
            Response = SNMPX_RSP_ERR_SEQUENCE;
            SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Channel is not open (anymore), UserState (%u): SNMPX_RequestManager()", pHDB->UserState);
            break;
        case SNMPX_HANDLE_STATE_OPENED: /* sock channel is opened  */
            switch (pHDB->MngrSession[SessionIndex].PortState)
            {
                case SNMPX_HANDLE_STATE_INIT:   /* not open */
                case SNMPX_SNMP_STATE_CLOSE:
                    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Channel is not open, PortState (%u): SNMPX_RequestManager()", pHDB->MngrSession[SessionIndex].PortState);
                    Response = SNMPX_ERR_SEQUENCE;
                    break;
                case SNMPX_SNMP_STATE_OPEN:
                    Response = SNMPX_OK;
                    break;
                case SNMPX_SNMP_STATE_WF_OPEN:
                case SNMPX_SNMP_STATE_WF_CLOSE:
                case SNMPX_SNMP_STATE_WF_CNF:   /* snmp request is pending */
                    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH, "Request is active, PortState (%u): SNMPX_RequestManager()", pHDB->MngrSession[SessionIndex].PortState);
                    Response = SNMPX_ERR_SEQUENCE;
                    break;
                default:
                    Response = SNMPX_ERR_FAILED;
                    SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_OPC_REQUEST_MANAGER failed (PortState: 0x%X).",pHDB->MngrSession[SessionIndex].PortState);
                    SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
                    break;
            }
            break;
        default:
            Response = SNMPX_ERR_SYNTAX;
            SNMPX_SYSTEM_TRACE_01(LSA_TRACE_LEVEL_UNEXP,"<--: SNMPX_OPC_REQUEST_MANAGER failed (UserState: 0x%X).",pHDB->UserState);
            SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            break;
    }

    if (Response == SNMPX_OK)
    {
        Response = SNMPX_CheckRequestManager(pRQB);
    }

    if (Response == SNMPX_OK)
    {
		/* build the global snmp reqiest-id */
		SNMPX_INCR_REQ_ID();
		pHDB->MngrSession[SessionIndex].RequestID = SNMPX_GET_REQ_ID();
        pHDB->MngrSession[SessionIndex].RetryCnt  = (LSA_UINT16)(pHDB->Params.pDetail->Params.Manager.NumOfRetries + 1);

        Response = SNMPX_DoRequestManager(pRQB, pHDB);
    }

    if (Response == SNMPX_OK)
    {
        pHDB->MngrSession[SessionIndex].pUserRQB = pRQB; /* store the user request */
    }
    else
    {
        SNMPX_UserRequestFinish(pHDB,pRQB,Response);
    }

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_RequestManager()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_CheckRequestManager                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check request Snmp-Get, Snmp-GetNext (IN = SessionID)     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_CheckRequestManager(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB)
{
    LSA_UINT16 RetVal = SNMPX_OK;
    SNMPX_UPPER_OBJECT_PTR_TYPE pObject;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_CheckRequestManager(pRQB: 0x%X)",
                            pRQB);

    SNMPX_ASSERT_NULL_PTR(pRQB);
    pObject = &pRQB->args.Object;

    SNMPX_ASSERT(!(pObject->SessionID == 0 ||
                   pObject->SessionID > SNMPX_CFG_MAX_MANAGER_SESSIONS));

	if (snmpx_is_null(pObject->pVarValue))	/* buffer for a snmp object */
	{
		RetVal = SNMPX_ERR_PARAM;
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: No buffer (0x%X) for SNMP object given!", pObject->pVarValue);
		SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckRequestManager(RetVal = 0x%X)", RetVal);
		return(RetVal);
	}

    switch (pObject->MessageType)
    {
        case SNMPX_GET:
        case SNMPX_GETNEXT:
			if (pObject->VarLength < SNMPX_SNMP_MAX_LEN)	/* buffer for a snmp object */
			{
	            RetVal = SNMPX_ERR_PARAM;
		        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: Invalid buffer length (0x%X) for SNMP object given!", pObject->VarLength);
				SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckRequestManager(RetVal = 0x%X)", RetVal);
				return(RetVal);
			}
			SNMPX_MEMSET(pObject->pVarValue, 0, SNMPX_SNMP_MAX_LEN);  /* null terminate */
            break;
        case SNMPX_SET:
            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: MessageType SNMPX_SET is not supported !");
            RetVal = SNMPX_ERR_NOT_IMPLEMENTED;
            break;
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: Invalid MessageType (0x%X) !", pObject->MessageType);
            RetVal = SNMPX_ERR_PARAM;
            break;
    }

    switch (pObject->SnmpVersion)
    {
        case SNMPX_SNMP_VERSION_1:
            break;
        case SNMPX_SNMP_VERSION_2:
            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: SnmpVersion SNMP-V2 is not supported !");
            RetVal = SNMPX_ERR_NOT_IMPLEMENTED;
            break;
        case SNMPX_SNMP_VERSION_3:
            SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: SnmpVersion SNMP-V3 is not supported !");
            RetVal = SNMPX_ERR_NOT_IMPLEMENTED;
            break;
        default:
            SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: Invalid SnmpVersion (0x%X) !", pObject->SnmpVersion);
            RetVal = SNMPX_ERR_PARAM;
            break;
    }

    /* OID and length of OID: The given oid buffer size must be SNMPX_MAX_OID_LEN * sizeof(SNMPX_OID_TYPE) */
    if (pObject->OidLen == 0 || pObject->OidLen > SNMPX_MAX_OID_LEN)
    {
        SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: Invalid OID length (0x%X) !", pObject->OidLen);
        RetVal = SNMPX_ERR_PARAM;
    }
    if (snmpx_is_null(pObject->pOid))
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPX_CheckRequestManager: No oid buffer given, must be SNMPX_MAX_OID_LEN * sizeof(SNMPX_OID_TYPE)!");
        RetVal = SNMPX_ERR_PARAM;
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_CheckRequestManager(RetVal = 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_DoRequestManager                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Execute request Snmp-Get, Snmp-GetNext (IN = SessionID)   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_DoRequestManager(
    SNMPX_UPPER_RQB_PTR_TYPE   pRQB,
    SNMPX_HDB_USER_PTR_TYPE    pHDB)
{
    SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pReqRQB;
    SNMPX_HDB_SOCK_PTR_TYPE        pHDBSock;
    LSA_UINT16                     SessionIndex;
	SNMPX_UPPER_COMMUNITY_PTR_TYPE pCommunity;
	LSA_UINT32                     RxTimeOut;

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_DoRequestManager(pRQB: 0x%X, pHDB: 0x%X)",
                            pRQB, pHDB);

	SNMPX_ASSERT_NULL_PTR(pRQB);
    SNMPX_ASSERT_NULL_PTR(pHDB);

    pHDBSock = SNMPX_GET_HSOCK_PTR();
    SNMPX_ASSERT_NULL_PTR(pHDBSock);

	SNMPX_ASSERT(pRQB->args.Object.SessionID);
    SessionIndex = (LSA_UINT16)(pRQB->args.Object.SessionID - 1); /* Index := SessionID - 1 */
	SNMPX_ASSERT(SessionIndex < SNMPX_CFG_MAX_MANAGER_SESSIONS);
	SNMPX_ASSERT(pHDB->MngrSession[SessionIndex].RetryCnt != 0);

	RxTimeOut = pHDB->Params.pDetail->Params.Manager.TimeoutInterval * 10; /* in 100ms */
	SNMPX_ASSERT(RxTimeOut != 0);

    /* alloc Request-RQB for Get, GetNext */
    pReqRQB = snmpx_sock_alloc_rsp_rqb(pHDBSock, pHDB->ThisHandle,
                                       SessionIndex); /* RQB for SOCK_OPC_UDP_SEND with buffer for packet building */

    if ( LSA_HOST_PTR_ARE_EQUAL(pReqRQB, LSA_NULL) )
    {
		pHDB->MngrSession[SessionIndex].RetryCnt = 0;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower send request RQB-memory failed!");
        SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_DoRequestManager()");
        return SNMPX_RSP_ERR_RESOURCE;
    }

    pReqRQB->args.data.sock_fd = pHDB->MngrSession[SessionIndex].SockFD;  /* socketdescriptor  */

    SNMPX_MEMCPY(&pReqRQB->args.data.rem_addr.sin_addr.S_un.S_un_b,
                 pHDB->MngrSession[SessionIndex].IPAddress, SNMPX_IP_ADDR_SIZE);

    /* swap the bytes (SOCK_HTONS) ? */
    {LSA_UINT16 sin_port = SNMPX_SNMP_PORT; pReqRQB->args.data.rem_addr.sin_port = SNMPX_SWAP16(sin_port);}
    pReqRQB->args.data.rem_addr.sin_family = SOCK_AF_INET;

	pCommunity = SNMPX_GetCommunityString(pRQB, pHDB);
	SNMPX_ASSERT_NULL_PTR(pCommunity);

    /* entry of the snmp send buffers */
    SNMPX_ASSERT_NULL_PTR(pReqRQB->args.data.buffer_ptr);
    {
		/* build the snmp request in buffer_ptr */
        LSA_INT Len = snmpx_mngr_build_snmp(pRQB->args.Object.pOid,
												(unsigned)(pRQB->args.Object.OidLen),
												pRQB->args.Object.VarType,
												(unsigned)(pRQB->args.Object.VarLength),
												pRQB->args.Object.pVarValue,
												(LSA_UINT8 *)pReqRQB->args.data.buffer_ptr,
												pReqRQB->args.data.buffer_length,
												(long)(pRQB->args.Object.SnmpVersion),
												pCommunity->name,
												pCommunity->name_len,
												SNMPX_GetAsnMessageType(pRQB->args.Object.MessageType),
												(long)(pHDB->MngrSession[SessionIndex].RequestID));
        if (Len)
        {
            pReqRQB->args.data.data_length = (LSA_UINT16)Len;
        }
        else /* build snmp failed, free the request RQB */
        {
            snmpx_sock_free_rsp_rqb(pHDBSock, pReqRQB);
			pHDB->MngrSession[SessionIndex].RetryCnt = 0;

            SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_DoRequestManager(SNMPX_ERR_FAILED)");
            return(SNMPX_ERR_FAILED);
        }
    }

    /* alloc Receive-RQB for Get.Res, GetNext.Res */
    /* exactly one receive resource for each request */
    if(pHDB->MngrSession[SessionIndex].RxPendingCnt == 0)
    {
        SNMPX_SOCK_LOWER_RQB_PTR_TYPE  pRecvRQB;

		/* RQB for SOCK_OPC_UDP_RECEIVE with buffer for packet building */
		pRecvRQB = snmpx_sock_alloc_rsp_rqb(pHDBSock, pHDB->ThisHandle,
											SessionIndex);

		if ( LSA_HOST_PTR_ARE_EQUAL(pRecvRQB, LSA_NULL) )
		{
			snmpx_sock_free_rsp_rqb(pHDBSock, pReqRQB);
			pHDB->MngrSession[SessionIndex].RetryCnt = 0;

			SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_UNEXP,"SOCK-Allocating lower receive request RQB-memory failed!");
			SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_DoRequestManager()");
			return SNMPX_RSP_ERR_RESOURCE;
		}

		pHDB->MngrSession[SessionIndex].RxPendingCnt++; /* number of Rx-Request pending within SOCK */

		/* provide the (1) Ind-Rqb  */
		pRecvRQB->args.data.sock_fd = pHDB->MngrSession[SessionIndex].SockFD;
		SNMPX_SOCK_RQB_SET_OPCODE(pRecvRQB,SOCK_OPC_UDP_RECEIVE);

		SNMPX_MEMCPY(&pRecvRQB->args.data.rem_addr.sin_addr.S_un.S_un_b,
					/* pHDB->MngrSession[SessionIndex].IPAddress, SNMPX_IP_ADDR_SIZE); */
					 (SNMPX_COMMON_MEM_PTR_TYPE)SNMPX_ZERO_IP, SNMPX_IP_ADDR_SIZE);

		/* swap the bytes (SOCK_HTONS) ? */
		{LSA_UINT16 sin_port = SNMPX_SNMP_PORT; pRecvRQB->args.data.rem_addr.sin_port = SNMPX_SWAP16(sin_port);}
		pRecvRQB->args.data.rem_addr.sin_family = SOCK_AF_INET;

		SNMPX_SOCK_REQUEST_LOWER(pRecvRQB,pHDBSock->Params.pSys);
    }

    /* set timeout counter for next send */
    pHDB->MngrSession[SessionIndex].NextSend = SNMPX_GET_GLOB_PTR()->CyclicTimerTick + RxTimeOut + 1;

    SNMPX_SOCK_REQUEST_LOWER(pReqRQB,pHDBSock->Params.pSys);

    pHDB->MngrSession[SessionIndex].PortState = SNMPX_SNMP_STATE_WF_CNF;
	pHDB->MngrSession[SessionIndex].RetryCnt--;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "OUT: SNMPX_DoRequestManager()");

    return(SNMPX_OK);
}

#endif
/*****************************************************************************/
/*  end of file SNMPX_USR.C                                                  */
/*****************************************************************************/

