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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE user-interface functions with RQBs          */
/*                                                                           */
/*                          - nare_open_channel()                            */
/*                          - nare_request()                                 */
/*                          - nare_close_channel()                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  9
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_TIM */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* NARE headerfiles */
#include "nare_int.h"            /* internal header */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* State-sequences RState within User-RCtrl-Block points to depending on mode*/
/*---------------------------------------------------------------------------*/

/* states for Opcode: NARE_OPC_IDENT                                          */
static const LSA_UINT8 NareStatesModeIdent[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_DCP_IDENT,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_SET Mode                                       */
static const LSA_UINT8 NareStatesModeSet[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_DCP_SET,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_TEST_IP         */
static const LSA_UINT8 NareStatesTestIP[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP,
    NARE_RCTRL_RSTATE_READY,
};

/* states for Opcode: NARE_OPC_RESOLVE_IP      */
static const LSA_UINT8 NareStatesResolveIP[] =
{
    NARE_RCTRL_RSTATE_START,
    NARE_RCTRL_RSTATE_EDD_RES_IP_ARP,
    NARE_RCTRL_RSTATE_READY,
};

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#ifndef NARE_CFG_DONT_VALIDATE_IP_SUITE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREValidateIPSuite                         +*/
/*+  Input/Output          :    NARE_IP_TYPE   IPAddr                       +*/
/*+                             NARE_IP_TYPE   SubMask                      +*/
/*+                             NARE_IP_TYPE   DefRouter                    +*/
/*+  Result                :    LSA_TRUE: OK                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if IP-Suite is valid (as defined by PNIO norm)     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL NAREValidateIPSuite(
    NARE_IP_TYPE const IPAddr,
    NARE_IP_TYPE const SubMask,
    NARE_IP_TYPE const DefRouter)
{
    LSA_UINT32                Msk;
    LSA_UINT32                AktMsk1;
    LSA_UINT32                AktMsk2;
    LSA_BOOL                  Status;

    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"IN : NAREValidateIPSuite()");
    Status = LSA_TRUE;
    /* --------------------------------------------------------------------------*/
    /* check IPAddr                                                              */
    /* --------------------------------------------------------------------------*/
    /* Not allowed: 127.0.0.0/8 e.g. 7F.xx.xx.xx */
    /* Not allowed: 224.0.0.0/4 e.g. Ex.xx.xx.xx */
    if ( (IPAddr[0] == 0x7F) || ((IPAddr[0] & 0xE0) == 0xE0))
    {
        NARE_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"Invalid IPAddress: %d.%d.%d.%d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
        Status = LSA_FALSE;
    }
    /* --------------------------------------------------------------------------*/
    /* check DefRouter                                                           */
    /* --------------------------------------------------------------------------*/
    /* Not allowed: 127.0.0.0/8 e.g. 7F.xx.xx.xx */
    /* Not allowed: 224.0.0.0/4 e.g. Ex.xx.xx.xx */
    if ( (DefRouter[0] == 0x7F) || ((DefRouter[0] & 0xE0) == 0xE0))
    {
        NARE_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"Invalid DefRouter Address: %d.%d.%d.%d",DefRouter[0],DefRouter[1],DefRouter[2],DefRouter[3]);
        Status = LSA_FALSE;
    }
    /* --------------------------------------------------------------------------*/
    /* subnet mask                                                               */
    /* --------------------------------------------------------------------------*/
    /* a subnetmask is valid, if a bitfield pair '1,0' exists which has on the   */
    /* "left" only bits '1' and on the "right" only bits '0'                     */
    Msk   = ((LSA_UINT32)SubMask[0] << 24) +
            ((LSA_UINT32)SubMask[1] << 16) +
            ((LSA_UINT32)SubMask[2] << 8 ) +
             (LSA_UINT32)SubMask[3];
    AktMsk1 = 0x8000000l;
    AktMsk2 = 0xFFFFFFFl;
    /* search first transition from 1 to 0 */
    while ((AktMsk1) && (Msk & AktMsk1 ))
    {
        AktMsk1 >>= 1;
        AktMsk2 >>= 1;
    }
    /* no more 1 allowed behind first transition */
    if ( AktMsk2 & Msk )
    {
        NARE_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"Invalid Subnetmask: %d.%d.%d.%d",SubMask[0],SubMask[1],SubMask[2],SubMask[3]);
        Status = LSA_FALSE;
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NAREValidateIPSuite()");
    return(Status);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserReady                              +*/
/*+  Input                      NARE_HDB_USER_PTR_TYPE       pHDB           +*/
/*+                             NARE_USER_RCTRL_PTR_TYPE     pRCtrlUser     +*/
/*+                             NARE_RSP                     Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  RCtrlUser  : Pointer to User-RCTRL-Block                               +*/
/*+  Response   : Responsecode to finish request                             */
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees the RCtrlUser and finished the User-Request with    +*/
/*+               Response by calling the Channel-Call-Back.                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_UserReady(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_USER_RCTRL_PTR_TYPE    pRCtrlUser,
    NARE_RSP                    Response)
{
    NARE_UPPER_RQB_PTR_TYPE  pRQB;

    pRQB = pRCtrlUser->pRQB;
    /* release the user-RCtrl-block      */
    NARE_UserReleaseRCTRL(pHDB,pRCtrlUser);
    /* finish the request */
    NAREUserRequestFinish(pHDB,pRQB,Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserStateMachine                       +*/
/*+  Input/Output          :    NARE_USER_RCTRL_PTR_TYPE       pRCtrlUser   +*/
/*+                             NARE_RSP                       Response     +*/
/*+                             LSA_BOOL                       CheckPending +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRCtrlUser : Pointer to User-RCTRL-Block                               +*/
/*+  Response   : Response of last action. if <> NARE_RSP_OK the Request    +*/
/*+               will be finished with this error. Set to NARE_RSP_OK if   +*/
/*+               not error present (e.g. on first call)                    +*/
/*+  CheckPending:LSA_TRUE if NARE_UserCheckPending(pHDB) should be called  +*/
/*+                        after a request was finised.                     +*/
/*+               LSA_FALSE if not.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Statemaching for User-Requests.                           +*/
/*+               The RCTRL-Block contains all information about the request+*/
/*+               pRState holds the actual state.                           +*/
/*+                                                                         +*/
/*+               CheckPending should be set to LSA_TRUE if called within   +*/
/*+               NARE_UserCheckPending() to avoid multiple reentrant and   +*/
/*+               therefore increasing of stack. it is not neccessary to    +*/
/*+               call NARE_UserCheckPending() if we are called from it.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_UserStateMachine(
    NARE_USER_RCTRL_PTR_TYPE   pRCtrlUser,
    NARE_RSP                   Response,
    LSA_BOOL                   CheckPending)
{
    NARE_HDB_USER_PTR_TYPE     pHDB;
    LSA_UINT8                  ActState;
    LSA_BOOL                   StartPending;
    NARE_UPPER_RQB_PTR_TYPE    pRQB;
    NARE_UPPER_RQB_PTR_TYPE    pRQBCancel;
    LSA_BOOL                   Ready;
    LSA_BOOL                   Cancel;

    StartPending = LSA_FALSE;
    pRQB         = pRCtrlUser->pRQB;
    NARE_ASSERT_NULL_PTR(pRQB);
    pHDB = NAREGetUserHDBFromHandle(NARE_RQB_GET_HANDLE(pRQB));
    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_UserStateMachine(pRCtrlUser: 0x%X, Resp: 0x%X)",pRCtrlUser,Response);
    ActState = *pRCtrlUser->pRState;
    /*---------------------------------------------------------------------------*/
    /* finish handling before advance to next state                              */
    /*---------------------------------------------------------------------------*/
    switch (ActState)
    {
        case NARE_RCTRL_RSTATE_START:
            NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Start",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
            break;

        case NARE_RCTRL_RSTATE_DCP_IDENT:
            /* Note: This state only for NARE_OPC_IDENT */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "SM [%X:%d]: DCP-Ident ready (Resp: %Xh)", pHDB->ThisHandle, pRCtrlUser->ThisIdx, Response);
            /* after finishing a DCP-Request we check and start further   */
            /* possible pending DCP-Request already in DCP-queue.         */
            /* This may result in finishing user-request if starting DCP  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */
            NARE_DCPCheckAndStartPending(pHDB->pHDBDcp);
            StartPending = LSA_TRUE;
            break;

        case NARE_RCTRL_RSTATE_DCP_SET:
            /* Note: This state only for NARE_OPC_SET */

            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Set ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);

            /* after finishing a DCP-Request we check and start further   */
            /* possible pending DCP-Request already in DCP-queue.         */
            /* This may result in finishing user-request if starting DCP  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */

            NARE_DCPCheckAndStartPending(pHDB->pHDBDcp);
            StartPending = LSA_TRUE;
            break;

        case NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP:
            /* Note: This state only for NARE_OPC_TEST_IP */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP for test-IP ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            /* After ARP-Request is ready (including the sniffing) we     */
            /* check and start further possible pending EDD-Requests      */
            /* already in EDD-queue.                                      */
            /* This may result in finishing user-request if starting EDD  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */

            NARE_EDDCheckAndStartPending(pHDB->pHDBEdd);
            StartPending = LSA_TRUE;
            break;

        case NARE_RCTRL_RSTATE_EDD_RES_IP_ARP:
            /* Note: This state only for NARE_OPC_RESOLVE_IP */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP for Resolve-IP ready (Resp: %Xh)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            /* After ARP-Request is ready (including the sniffing) we     */
            /* check and start further possible pending EDD-Requests      */
            /* already in EDD-queue.                                      */
            /* This may result in finishing user-request if starting EDD  */
            /* request fails. so we may get free user-RCTRL-Blocks        */
            /* which can be used by pending user-requests. need to be     */
            /* checked later!                                             */
            NARE_EDDCheckAndStartPending(pHDB->pHDBEdd);
            StartPending = LSA_TRUE;
            break;

        default:
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid RState (%Xh)",*pRCtrlUser->pRState);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;
    }
    /*---------------------------------------------------------------------------*/
    /* On error we dont advance to next state, instead we finish the request.    */
    /* On cancel we abort too.                                                   */
    /*---------------------------------------------------------------------------*/
    if (( Response != NARE_RSP_OK ) || ( pRCtrlUser->DoCancel))
    {
        Cancel       = LSA_FALSE;
        pRQBCancel   = LSA_NULL;
        if ( pRCtrlUser->DoCancel  )
        {
            pRQBCancel = pRCtrlUser->pCancelRQB; /* save the CancelRQB */
            Response   = NARE_RSP_OK_CANCEL;
            switch ( NARE_RQB_GET_OPCODE(pRQB) )
            {
                    /* RequestID temporary used as Cancel-Pending Counter. if 0 this was */
                    /* the last Reqeust to be canceled, so the Cancel is ready.          */
                case NARE_OPC_IDENT:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "SM [%X:%d]: NARE_OPC_CANCEL Canceling", pHDB->ThisHandle, pRCtrlUser->ThisIdx);
                    if (!--pRQBCancel->args.Cancel.RequestID)
                    {
                        Cancel = LSA_TRUE;
                        /* restore RequestID */
                        pRQBCancel->args.Cancel.RequestID = pRQB->args.Ident.RequestID;
                    }
                    break;

                case NARE_OPC_SET:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "SM [%X:%d]: NARE_OPC_CANCEL Canceling", pHDB->ThisHandle, pRCtrlUser->ThisIdx);
                    if (!--pRQBCancel->args.Cancel.RequestID)
                    {
                        Cancel = LSA_TRUE;
                        /* restore RequestID */
                        pRQBCancel->args.Cancel.RequestID = pRQB->args.Set.RequestID;
                    }
                    break;

                case NARE_OPC_TEST_IP:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "SM [%X:%d]: NARE_OPC_CANCEL Canceling", pHDB->ThisHandle, pRCtrlUser->ThisIdx);
                    if (!--pRQBCancel->args.Cancel.RequestID)
                    {
                        Cancel = LSA_TRUE;
                        /* restore RequestID */
                        pRQBCancel->args.Cancel.RequestID = pRQB->args.TestIP.RequestID;
                    }
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid opcode on cancel (%d)",NARE_RQB_GET_OPCODE(pRQB));
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
        else
        {
            if (Response == NARE_RSP_OK_CANCEL)
            {
                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Request Canceling (Opcode: 0x%X)",pHDB->ThisHandle,pRCtrlUser->ThisIdx, NARE_RQB_GET_OPCODE(pRQB));
            }
            else
            {
                NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Response failure (0x%X)",pHDB->ThisHandle,pRCtrlUser->ThisIdx,Response);
            }
        }
        /* release RCtrl-Block and finish the request */
        NARE_UserReady(pHDB,pRCtrlUser,Response);
        /* Finish Cancel-request */
        if ( Cancel )
        {
            NARE_ASSERT_NULL_PTR(pRQBCancel);
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Finishing Cancel request (RequestID: 0x%X)", pRQBCancel->args.Cancel.RequestID);
            NAREUserRequestFinish(pHDB,pRQBCancel,NARE_RSP_OK);
        }
        StartPending = LSA_TRUE;
    }
    else
    {
        Ready = LSA_FALSE;
        /* -----------------------------------------------------------------------*/
        /* forward in statemachine till an asynchroneous request is underway which*/
        /* we have to wait for.                                                   */
        /* -----------------------------------------------------------------------*/
        while (! Ready)
        {
            pRCtrlUser->pRState++; /* switch to next state */
            NARE_PROGRAM_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: Next State: %d",pHDB->ThisHandle,pRCtrlUser->ThisIdx,*pRCtrlUser->pRState);
            Ready = LSA_TRUE;      /* Default: break loop because request underway*/
            /*--------------------------------------------------------------------*/
            /* do handling for this state                                         */
            /*--------------------------------------------------------------------*/
            switch (*pRCtrlUser->pRState )
            {
                case NARE_RCTRL_RSTATE_DCP_IDENT:
                    /* Note: This state only for NARE_OPC_IDENT */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: DCP-Ident request",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a DCP-request. This may cause a immidiate     */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    NARE_DCPApplyRCtrl(pHDB->pHDBDcp,pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP:
                    /* Note: This state only for NARE_OPC_TEST_IP */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP request for testing IP",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a EDD-ARP-request. This may cause a immediate */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    /* Note that cfg.IPAddr must contain the address to ARP for   */
                    NARE_EDDApplyRCtrl(pHDB->pHDBEdd,pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_EDD_RES_IP_ARP:
                    /* Note: This state only for NARE_OPC_RESOLVE_IP */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: EDD-ARP request for resolving IP",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* Try to start a EDD-ARP-request. This may cause a immediate */
                    /* finishing off the request because of no resources. so      */
                    /* it is possible, that a user-RCTRL-Block gets free. So we   */
                    /* have to check for queued user-request later..              */
                    /* Note that cfg.IPAddr must contain the address to ARP for   */
                    NARE_EDDApplyRCtrl(pHDB->pHDBEdd,pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_DCP_SET:
                    /* Note: This state only for NARE_OPC_SET                     */
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "SM [%X:%d]: DCP-Set request", pHDB->ThisHandle, pRCtrlUser->ThisIdx);
                    pRCtrlUser->DCPSetMode = NARE_RCTRL_DCP_SET_NONE;
                    /* With NARE_MODE_CONFIG_IP_FIXED we set an IP-Address        */
                    if (pRCtrlUser->pRQB->args.Set.Mode & NARE_MODE_CONFIG_IP)
                    {
                        pRCtrlUser->DCPSetMode |= NARE_RCTRL_DCP_SET_IP;
                    }

                    if (pRCtrlUser->pRQB->args.Set.Mode & NARE_MODE_CONFIG_NOS)
                    {
                        pRCtrlUser->DCPSetMode |= NARE_RCTRL_DCP_SET_STATIONNAME;
                    }

                    /* Try to start a DCP-request. This may cause an immediate*/
                    /* finishing of  the request because of no resources. So  */
                    /* it is possible  that a user-RCTRL-Block gets free.     */
                    /* So we have to check for queued user-request later..    */
                    NARE_DCPApplyRCtrl(pHDB->pHDBDcp, pRCtrlUser);
                    StartPending = LSA_TRUE;
                    break;

                case NARE_RCTRL_RSTATE_READY:
                    NARE_PROGRAM_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"SM [%X:%d]: User-Request successfully finished",pHDB->ThisHandle,pRCtrlUser->ThisIdx);
                    /* The request is ready. we finish it and check for further   */
                    /* pending user-requests.                                     */
                    NARE_UserReady(pHDB,pRCtrlUser,Response);
                    StartPending = LSA_TRUE;
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"NARE_UserStateMachine(): Invalid RState (%Xh)",*pRCtrlUser->pRState);
                    NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                    break;
            } /* switch */
        } /* while */
    }
    /*---------------------------------------------------------------------------*/
    /* if a user-request was finished we have to check for already pending       */
    /* user-request which can be startet (queued in user-handle). Within         */
    /* NARE_UserCheckPending() we call NARE_UserStateMachine() again             */
    /* (reentrance) but with "CheckPending = FALSE" because we already try       */
    /* to start pending requests.                                                */
    /*---------------------------------------------------------------------------*/
    if ( StartPending && CheckPending )
    {
        /* check for pending user-requests   */
        NARE_UserCheckPending(pHDB);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_UserStateMachine()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARE_UserCheckPending                       +*/
/*+  Input/Output          :    NARE_HDB_USER_PTR_TYPE             pHDB     +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to DCP -HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-RQB pending.                               +*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_UserCheckPending(
    NARE_HDB_USER_PTR_TYPE    pHDB)
{
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser;
    LSA_BOOL                  Ready;
    NARE_UPPER_RQB_PTR_TYPE   pRQB;

    NARE_ASSERT_NULL_PTR(pHDB);
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARE_UserCheckPending(pHDB: 0x%X)",pHDB);
    Ready = LSA_FALSE;
    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-Checking for pending USER-requests (pHDB: 0x%X)",pHDB);
    while (! Ready )
    {
        if ( pHDB->UserReqPendCnt ) /* User-RQB pending ? */
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-requests pending (pHDB: 0x%X)",pHDB);
            if (pHDB->UserRCtrl.Free) /* Free USER-RCTRL-Blocks ? */
            {
                NARE_RQB_REM_BLOCK_BOTTOM(pHDB->UserReq.pBottom,
                                          pHDB->UserReq.pTop,
                                          pRQB);
                /* because UserReqPendCnt was >0 we dont expect LSA_NULL here */
                NARE_ASSERT_NULL_PTR(pRQB);
                pHDB->UserReqPendCnt--;
                pRCtrlUser= NARE_UserGetFreeRCTRL(pHDB);
                /* because Free was >0 we dont expect LSA_NULL here         */
                NARE_ASSERT_NULL_PTR(pRCtrlUser);
                /* Setup RCTRL-Block with init-values for state-maching      */
                pRCtrlUser->pRQB   = pRQB;
                /* Setup States to start of Statesequence depending on opcode*/
                switch ( NARE_RQB_GET_OPCODE(pRQB))
                {
                    case NARE_OPC_IDENT:
                        pRCtrlUser->pRState = &NareStatesModeIdent[0];
                        break;

                    case NARE_OPC_SET:
                        pRCtrlUser->pRState = &NareStatesModeSet[0];
                        break;

                    case NARE_OPC_TEST_IP:
                        pRCtrlUser->pRState = &NareStatesTestIP[0];
                        break;

                    case NARE_OPC_RESOLVE_IP:
                        pRCtrlUser->pRState = &NareStatesResolveIP[0];
                        break;

                    default:
                        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_FATAL,"Unexpected RQB-Opcode (0x%X)",NARE_RQB_GET_OPCODE(pRQB));
                        NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                        break;
                }
                /* Call with LSA_FALSE, because we already in a loop */
                /* handling all user-request, so we dont need to call*/
                /* us again within NARE_UserStateMachine().          */
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-Free User RCTRL-Found [Idx: %d]. Starting-Statemachine.",pRCtrlUser->ThisIdx);
                NARE_UserStateMachine(pRCtrlUser,NARE_RSP_OK,LSA_FALSE);
            }
            else
            {
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-No free USER-RCTRL (pHDB: 0x%X)",pHDB);
                Ready = LSA_TRUE;
            }
        }
        else
        {
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"USER-No requests pending (pHDB: 0x%X)",pHDB);
            Ready = LSA_TRUE;
        }
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARE_UserCheckPending()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestTimer                            +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     NARE_OPC_TIMER                  +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response    Response                        +*/
/*+     NARE_RQB_ERROR_TYPE     Error       Addition errorcodes from LowerL.+*/
/*+     NARE_RQB_ARGS_TYPE      args.Timer: NARE_RQB_TIMER_TYPE             +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : NARE-Timer ID                                            +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from NARE within nare_timeout().        +*/
/*+               Handles Timer-events within NARE main context.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID NARERequestTimer(
    NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestTimer(pRQB: 0x%X)",pRQB);
    switch (pRQB->args.Timer.UserID )
    {
        case NARE_TIMER_USER_ID_ARP:
            /* NARE_DBG_ACT("NARERequestTimer RQB received.\r\n",0,0,0); */
            NARETimerRQBFree(pRQB->args.Timer.TimerID);
            NARE_EDDTimerEventARP();
            break;

        default:
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"NARERequestTimer. Invalid UserID (0x%X)",pRQB->args.Timer.UserID);
            NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
            break;

    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestTimer()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestIdentCancel                      +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_CANCEL                +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+                                                                         +*/
/*+     RequestID         : Request ID specifying Request to cancel         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Cancel Request from user.                         +*/
/*+                                                                         +*/
/*+               We search for all requests with the spezified RequestID.  +*/
/*+               We cancel all of these requests. If the requests already  +*/
/*+               has a cancel pending, we skip it, because it already will +*/
/*+               be canceled.                                              +*/
/*+                                                                         +*/
/*+               * Checks User-Request queue.                              +*/
/*+               * Checks DCP/EDD-Pending queues.                          +*/
/*+               * Checks if cancel have to be delayed because of action   +*/
/*+                 currently in progress (e.g. lowerlayer request running) +*/
/*+                                                                         +*/
/*+               If the RCTRL-block is in use and running we set a flag    +*/
/*+               to signal a cancel is pending. the cancel is done later.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID NARERequestCancel(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32                i;
    NARE_RSP                  Response;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp2;
    LSA_UINT32                RequestID;
    NARE_USER_RCTRL_PTR_TYPE  pRCtrl;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : NARERequestCancel(pRQB: 0x%X)", pRQB);
    Response = NARE_RSP_OK;
    RequestID = pRQB->args.Cancel.RequestID;
    /*---------------------------------------------------------------------------*/
    /* we use the RequestID within RQB as counter of pending request to be       */
    /* canceled. will be restored to RequestID on confirmation.                  */
    /*---------------------------------------------------------------------------*/
    pRQB->args.Cancel.RequestID = 0; /* 0 = no queued cancel */
    /* --------------------------------------------------------------------------*/
    /* First we check if the request is pending with USER-handle mgm             */
    /* This request can canceled at once.                                        */
    /* --------------------------------------------------------------------------*/
    pRQBHelp = pHDB->UserReq.pBottom;
    while (!LSA_HOST_PTR_ARE_EQUAL(pRQBHelp, LSA_NULL)) /* search all */
    {
        if (((NARE_RQB_GET_OPCODE(pRQBHelp) == NARE_OPC_IDENT) && (pRQBHelp->args.Ident.RequestID == RequestID)) ||
            ((NARE_RQB_GET_OPCODE(pRQBHelp) == NARE_OPC_SET) && (pRQBHelp->args.Set.RequestID == RequestID)) ||
            ((NARE_RQB_GET_OPCODE(pRQBHelp) == NARE_OPC_TEST_IP) && (pRQBHelp->args.TestIP.RequestID == RequestID)))
        {
            NARE_RQB_REM_BLOCK(pHDB->UserReq.pBottom,
                pHDB->UserReq.pTop,
                pRQBHelp,
                pRQBHelp2);
            NARE_ASSERT_NULL_PTR(pRQBHelp);
            NARE_ASSERT_NULL_PTR(pRQBHelp2); /* Help2 has to be the same value as Help */
            NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Canceling User-Request1 (RequestID: 0x%X)", RequestID);
            pHDB->UserReqPendCnt--;
            pRQBHelp = NARE_RQB_GET_NEXT_RQB_PTR(pRQBHelp);
            NAREUserRequestFinish(pHDB, pRQBHelp2, NARE_RSP_OK_CANCEL);
        }
        else
        {
            pRQBHelp = NARE_RQB_GET_NEXT_RQB_PTR(pRQBHelp);
        }
    }
    /* --------------------------------------------------------------------------*/
    /* Now search within Usr RCTRL-Block for Request                             */
    /* --------------------------------------------------------------------------*/
    i = 0;
    pRCtrl = &(*pHDB->UserRCtrl.pTable)[0];

    while (i < pHDB->UserRCtrl.Cnt)
    {
        if ((pRCtrl->BState == NARE_RCTRL_BSTATE_USED) &&
            (!LSA_HOST_PTR_ARE_EQUAL(pRCtrl->pRQB, LSA_NULL)) &&    /* should be!*/
            (((NARE_RQB_GET_OPCODE(pRCtrl->pRQB) == NARE_OPC_IDENT) && (pRCtrl->pRQB->args.Ident.RequestID == RequestID)) ||
            ((NARE_RQB_GET_OPCODE(pRCtrl->pRQB) == NARE_OPC_SET) && (pRCtrl->pRQB->args.Set.RequestID == RequestID)) ||
            ((NARE_RQB_GET_OPCODE(pRCtrl->pRQB) == NARE_OPC_TEST_IP) && (pRCtrl->pRQB->args.TestIP.RequestID == RequestID))) &&
            (!pRCtrl->DoCancel))  /* not already a Cancel running ? */
        {
            /* -------------------------------------------------------------*/
            /* if found check if the request is currently pending. if so we */
            /* can cancel it                                                */
            /* -------------------------------------------------------------*/
            switch (pRCtrl->QueueType)
            {
                /* if the request is pending we can cancel it here */
            case NARE_RCTRL_QTYPE_DCP_PENDING:
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Canceling User-Request2 (RequestID: 0x%X)", RequestID);
                NARE_DCPRemoveRCtrl(pHDB->pHDBDcp, pRCtrl); /* remove from DCP-queue */
                NARE_UserReady(pHDB, pRCtrl, NARE_RSP_OK_CANCEL);
                break;

            case NARE_RCTRL_QTYPE_EDD_PENDING:
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Canceling User-Request3 (RequestID: 0x%X)", RequestID);
                NARE_EDDRemoveRCtrl(pHDB->pHDBEdd, pRCtrl); /* remove from EDD-queue */
                NARE_UserReady(pHDB, pRCtrl, NARE_RSP_OK_CANCEL);
                break;

                /* if the request is running we set a flag for     */
                /* later cancel. This flag will be checked in      */
                /* statemachine                                    */

            case NARE_RCTRL_QTYPE_DCP_RUNNING:
            case NARE_RCTRL_QTYPE_EDD_RUNNING:
                /* Note that we may cancel ARP-Sniffing here.  */
                /* This is currently not implemented.          */
                NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Cancel not possible yet. Queuing it (RequestID: 0x%X)", RequestID);
                pRCtrl->pCancelRQB = pRQB;
                pRCtrl->DoCancel = LSA_TRUE;
                pRQB->args.Cancel.RequestID++;  /* counter for pending requests */
                break;

            case NARE_RCTRL_QTYPE_NONE:
            default:
                NARE_FATAL(NARE_FATAL_ERR_INCONSISTENZ);
                break;
            } /* switch */
        }
        i++;
        pRCtrl++;
    } /* while ()  */
    /* --------------------------------------------------------------------------*/
    /* Now search within EDD-RCTRL-Blocks. Because Sniffing maybe running.       */
    /* --------------------------------------------------------------------------*/
    NARE_EDDAbortSniffing(pHDB->pHDBEdd);
    /*----------------------------------------------------------------------------*/
    /* If no Cancel is queued we can finish the Cancel-Request here.              */
    /*----------------------------------------------------------------------------*/
    if (!pRQB->args.Cancel.RequestID) /* if no queued cancel. */
    {
        pRQB->args.Cancel.RequestID = RequestID; /* restore RequestID */
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Finishing Cancel-request (RequestID: 0x%X)", pRQB->args.Cancel.RequestID);
        NAREUserRequestFinish(pHDB, pRQB, Response);
    }
    else
    {   /* a DCP request is "on the way" so cancel it */
        NARE_DCPCancel(pHDB->pHDBDcp, RequestID);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT: NARERequestCancel()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestIdentNotify                      +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_IDENT_NOTIFY          +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.IdentNotify   NARE_OPC_IDENT_NOTIFY    +*/
/*+                                                                         +*/
/*+     args.IdentNotify                                                    +*/
/*+                                                                         +*/
/*+     NotifyID          : NotifID from corresponding IDENT req.           +*/
/*+     Res               : Resolve-parameters                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles IdentNotify Request from user.                    +*/
/*+                                                                         +*/
/*+               The request is queued within NARE and used for indication +*/
/*+               if a identify response comes in.                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestIdentNotify(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : NARERequestIdentNotify(pRQB: 0x%X)", pRQB);
    /* queue the request */
    NARE_RQB_PUT_BLOCK_TOP(pHDB->NotifyReq.pBottom,
        pHDB->NotifyReq.pTop,
        pRQB);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT: NARERequestIdentNotify()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestIdent                            +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_IDENT                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.Ident NARE_RQB_IDENT_TYPE              +*/
/*+                                                                         +*/
/*+     args.Ident:                                                         +*/
/*+                                                                         +*/
/*+     StationName       : StationName to use search for                   +*/
/*+     StationNameLen    : Length of StationName in bytes                  +*/
/*+     RequestID         : Request ID. used for cancel.                    +*/
/*+     NARE_IP_TYPE      : IPAddr;               0.0.0.0 if no IP          +*/
/*+     NARE_IP_TYPE      : SubMask;              0.0.0.0 if no SubMask     +*/
/*+     NARE_IP_TYPE      : DefRouter;            0.0.0.0 if no DefRout.    +*/
/*+     NARE_MAC_TYPE     : MACAddr;              0.0.0.0.0.0 if no MAC     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Ident-Request from user                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestIdent(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_RSP                  Response;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : NARERequestIdent(pRQB: 0x%X)", pRQB);
    pRQB->args.Ident.AliasUsed = 0; /* Initialize return value */
    Response = NARE_RSP_OK;
    /* check parameters of request */
    if ((pRQB->args.Ident.StationNameLen == 0)
        || (LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Ident.pStationName, LSA_NULL)))
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "Ident-Request. No StationName in RQB");
        Response = NARE_RSP_ERR_PARAM;
    }
    else if (pRQB->args.Ident.Notify != NARE_NOTIFY_DCP_RESPONSE_NO && pRQB->args.Ident.Notify != NARE_NOTIFY_DCP_RESPONSE_YES)
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "Ident-Request. Invalid Notify parameter!");
        Response = NARE_RSP_ERR_PARAM;
    }
    else
    {
#ifndef NARE_CFG_NO_ALIAS_SUPPORT
        /* validate ALIAS-name parameters */
        if (pRQB->args.Ident.AliasCnt)
        {
            if (LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Ident.pStationNameAlias, LSA_NULL))
            {
                NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "Ident-Request. Alias mismatch in RQB");
                Response = NARE_RSP_ERR_PARAM;
            }
            else
            {
                LSA_UINT32 j;
                for (j = 0; j< pRQB->args.Ident.AliasCnt; j++)
                {
                    if ((pRQB->args.Ident.pStationNameAlias[j].NameLen == 0)
                        || (LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Ident.pStationNameAlias[j].pName, LSA_NULL)))
                    {
                        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "Ident-Request. Alias mismatch in RQB");
                        Response = NARE_RSP_ERR_PARAM;
                    }
                }
            }
        }
#endif
    }

    if (Response == NARE_RSP_OK)
    {
        LSA_UINT32  i;
        /* init out-params */
        for (i = 0; i<NARE_IP_ADDR_SIZE; i++)
        {
            pRQB->args.Ident.IPAddr[i] = 0;
            pRQB->args.Ident.DefRouter[i] = 0;
            pRQB->args.Ident.SubMask[i] = 0;
        }
        for (i = 0; i<NARE_MAC_ADDR_SIZE; i++)
        {
            pRQB->args.Ident.MACAddr[i] = 0;
        }
        pRQB->args.Ident.DeviceID = 0;
        pRQB->args.Ident.VendorID = 0;
        pRQB->args.Ident.InstanceID = 0;
        pRQB->args.Ident.InstanceIDValid = 0;
        pRQB->args.Ident.IdentifiedStationNameLen = 0;
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Ident-Request. Queuing block (pRQB: 0x%X)", pRQB);
        /* queue the request */
        NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
            pHDB->UserReq.pTop,
            pRQB);
        pHDB->UserReqPendCnt++;
        /* check for pending requests and try to execute it */
        NARE_UserCheckPending(pHDB);
    }
    else
    {
        NAREUserRequestFinish(pHDB, pRQB, Response);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT: NARERequestIdent()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestSet                              +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_SET                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.Set NARE_RQB_SET_TYPE                  +*/
/*+                                                                         +*/
/*+     args.Set:                                                           +*/
/*+                                                                         +*/
/*+     StationName       : StationName to use search for                   +*/
/*+     StationNameLen    : Length of StationName in bytes                  +*/
/*+     RequestID         : Request ID. used for cancel.                    +*/
/*+     NARE_IP_TYPE      : IPAddr;                                         +*/
/*+     NARE_IP_TYPE      : SubMask;                                        +*/
/*+     NARE_IP_TYPE      : DefRouter;                                      +*/
/*+     NARE_MAC_TYPE     : MACAddr;                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Set-Request from user                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestSet(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_RSP                  Response;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN : NARERequestSet(pRQB: 0x%X)", pRQB);

    Response = NARE_RSP_OK;

    if ((pRQB->args.Set.Mode & ~NARE_MODE_CONFIG_MASK) != 0 || (pRQB->args.Set.Mode & NARE_MODE_CONFIG_MASK) == 0)
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "Set-Request. Wrong Mode (0x%Xh)", pRQB->args.Set.Mode);
        Response = NARE_RSP_ERR_PARAM;
    }
    else
    {
        if ((pRQB->args.Set.Mode & NARE_MODE_CONFIG_NOS) != 0)
        {
            if ((pRQB->args.Set.StationNameLen == 0)
                || (LSA_HOST_PTR_ARE_EQUAL (pRQB->args.Set.pStationName, LSA_NULL)))
            {
                NARE_PROGRAM_TRACE_00 (pHDB->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "Set-Request. No StationName in RQB");
                Response = NARE_RSP_ERR_PARAM;
            }
        }

#ifndef NARE_CFG_DONT_VALIDATE_IP_SUITE
        if ((pRQB->args.Set.Mode & NARE_MODE_CONFIG_IP) != 0)
        {
            if (!NAREValidateIPSuite (pRQB->args.Set.IPAddr,
                pRQB->args.Set.SubMask,
                pRQB->args.Set.DefRouter))
            {
                NARE_PROGRAM_TRACE_00 (pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "Set-Request. Invalid IP-Suite parameters!");
                Response = NARE_RSP_ERR_PARAM;
            }
        }
#endif
    }

    if (pRQB->args.Set.SetIPRemanent != NARE_SET_IP_REMANENT_NO && pRQB->args.Set.SetIPRemanent != NARE_SET_IP_REMANENT_YES)
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "Set-Request. Invalid SetIPRemanent parameter!");
        Response = NARE_RSP_ERR_PARAM;
    }

    if (Response == NARE_RSP_OK)
    {
        NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "Set-Request. Queuing block (pRQB: 0x%X)", pRQB);
        /* queue the request */
        NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
            pHDB->UserReq.pTop,
            pRQB);
        pHDB->UserReqPendCnt++;
        /* check for pending requests and try to execute it */
        NARE_UserCheckPending(pHDB);
    }
    else
    {
        NAREUserRequestFinish(pHDB, pRQB, Response);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT: NARERequestSet()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestTestIP                           +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_TEST_IP               +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.TestIP                                 +*/
/*+                                                                         +*/
/*+     args.TestIP                                                         +*/
/*+                                                                         +*/
/*+     IPAddr            : IP-Address to test                              +*/
/*+     MACAddr           : Found MAC if IP present                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles TestIP- Request from user.                        +*/
/*+                                                                         +*/
/*+               Responses:  NARE_RSP_OK                                   +*/
/*+                           NARE_RSP_ERR_IP_IN_USE                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestTestIP(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32                i;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestTestIP(pRQB: 0x%X)",pRQB);

    if (pRQB->args.TestIP.UseDelay != NARE_USE_DELAY_NO && pRQB->args.TestIP.UseDelay != NARE_USE_DELAY_YES)
    {
        NARE_PROGRAM_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "NARERequestTestIP. Invalid UseDelay parameter!");
        NAREUserRequestFinish(pHDB, pRQB, NARE_RSP_ERR_PARAM);
        NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestTestIP()");
        return;
    }

    /* init out-params */
    for ( i=0; i<NARE_MAC_ADDR_SIZE; i++)
    {
        pRQB->args.TestIP.MACAddr[i] = 0;
    }
    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"TestIP-Request. Queuing block (pRQB: 0x%X).",pRQB);
    /* queue the request */
    NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
                           pHDB->UserReq.pTop,
                           pRQB);
    pHDB->UserReqPendCnt++;
    /* check for pending requests and try to execute it */
    NARE_UserCheckPending(pHDB);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestTestIP()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestResolveIP                        +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_RESOLVE_IP            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.ResolveIP                              +*/
/*+                                                                         +*/
/*+     args.ResolveIP                                                      +*/
/*+                                                                         +*/
/*+     IPAddr            : IP-Address to test                              +*/
/*+     MACAddr           : Resolved MAC if IP present                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles ResolveIP-Request from user.                      +*/
/*+                                                                         +*/
/*+               Responses:  NARE_RSP_OK                                   +*/
/*+                           NARE_RSP_ERR_MULTIPLE_IP_USE                  +*/
/*+                           NARE_RSP_ERR_IP_RESOLVE_NO_ANSWER             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestResolveIP(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    LSA_UINT32              i;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestResolveIP(pRQB: 0x%X)",pRQB);
    /* init out-params */
    for ( i=0; i<NARE_MAC_ADDR_SIZE; i++)
    {
        pRQB->args.ResolveIP.MACAddr[i] = 0;
    }
    NARE_PROGRAM_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"ResolveIP-Request. Queuing block (pRQB: 0x%X).",pRQB);
    /* queue the request */
    NARE_RQB_PUT_BLOCK_TOP(pHDB->UserReq.pBottom,
                           pHDB->UserReq.pTop,
                           pRQB);
    pHDB->UserReqPendCnt++;
    /* check for pending requests and try to execute it */
    NARE_UserCheckPending(pHDB);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestResolveIP()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestHello                            +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_HELLO                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.Hello                                  +*/
/*+                                                                         +*/
/*+     args.Hello                                                          +*/
/*+                                                                         +*/
/*+     SendInterval            : send interval in 10ms ticks               +*/
/*+     SendCnt                 : number of sends                           +*/
/*+     pTLV                    : pointer to HELLO TLV chain                +*/
/*+     TLVLen                  : length of TLV chain                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles HELLO request from user.                          +*/
/*+                                                                         +*/
/*+               Note: Only one user per DCP channel can use HELLO requests+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestHello(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_RSP                  Status = NARE_RSP_OK;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NARERequestHello(pRQB: 0x%X)",pRQB);
    /* Parametercheck */
    if (( LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Hello.pTLV,LSA_NULL )) ||
        ( pRQB->args.Hello.SendCnt == 0 ) ||
        ( pRQB->args.Hello.OptCnt  == 0 ))
    {
        Status = NARE_RSP_ERR_PARAM;
    }
    else
    {   /* Do a DCP Hello (note: only if not already a hello is running. */
        /* checked inside!)                                              */
        Status = NARE_DCPHello(pHDB->pHDBDcp,
                               pHDB,
                               pRQB->args.Hello.SendInterval,
                               pRQB->args.Hello.SendCnt,
                               pRQB->args.Hello.Delay,
                               pRQB->args.Hello.pTLV,
                               pRQB->args.Hello.OptCnt);
        /* If OK the Request to DCP is underway */
        if ( Status == NARE_RSP_OK )
        {
            pHDB->Hello.pRQB   = pRQB;
            pHDB->Hello.Status = NARE_USR_HELLO_IN_PROGRESS;
        }
    }
    if ( Status != NARE_RSP_OK )
    {
        NAREUserRequestFinish(pHDB,pRQB,Status);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestHello()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERequestHelloStop                        +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User-HDB pointer                                          +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      NARE_OPC_HELLO_STOP            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     NARE_RSP                Response     Response                       +*/
/*+     NARE_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     NARE_RQB_ARGS_TYPE      args.HelloStop                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles HELLO_STOP request from user.                     +*/
/*+                                                                         +*/
/*+               Note: Only one user per DCP channel can use HELLO requests+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID NARERequestHelloStop(
    NARE_UPPER_RQB_PTR_TYPE pRQB,
    NARE_HDB_USER_PTR_TYPE  pHDB)
{
    NARE_RSP                  Status;

    NARE_FUNCTION_TRACE_01(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"IN : NARERequestHelloStop(pRQB: 0x%X)",pRQB);
    Status = NARE_DCPHelloStop(pHDB->pHDBDcp,pHDB);
    /* If OK the Request to DCP is underway */
    if ( Status == NARE_RSP_OK )
    {
        pHDB->Hello.pRQB   = pRQB;
        pHDB->Hello.Status = NARE_USR_HELLO_IN_PROGRESS;
    }
    else
    {
        NAREUserRequestFinish(pHDB,pRQB,Status);
    }
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NARERequestHelloStop()");
}

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_open_channel                           +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       NARE_OPC_OPEN_CHANNEL         +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     NARE_RSP                Response      -                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error         -                             +*/
/*+     NARE_RQB_ARGS_TYPE      Args.Channel: NARE_RQB_OPEN_CHANNEL_TYPE    +*/
/*+                                                                         +*/
/*+     Args.Channel:                                                       +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         nandle:       channel-handle of NARE (ret)  +*/
/*+     LSA_HANDLE_TYPE         handle_upper  channel-handle of user        +*/
/*+     LSA_SYS_PATH_TYPE       sys_path      system-path of channel        +*/
/*+     LSA_VOID                LSA_FCT_PTR(ATTR,                           +*/
/*+                                         nare_request_upper_done_ptr)    +*/
/*+                                         (NARE_UPPER_RQB_PTR_TYPE pRQB)  +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user. This  +*/
/*+                                         is HandleUpper from Args.Channel+*/
/*+     NARE_RSP                Response    NARE_RSP_OK                     +*/
/*+                                         NARE_RSP_ERR_RESOURCE           +*/
/*+                                         NARE_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel                                 +*/
/*+                                                                         +*/
/*+               Within this request the output - macro NARE_GET_PATH_INFO +*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+                                                                         +*/
/*+               After a successful open a channel-handle is returned      +*/
/*+               in Args.Channel.Handle for use with further requests.     +*/
/*+                                                                         +*/
/*+               HandleUpper:                                              +*/
/*+                                                                         +*/
/*+               This handle will be stored within NARE and return as      +*/
/*+               handle on request-confirmation (in RQB-header).           +*/
/*+                                                                         +*/
/*+               cbf:                                                      +*/
/*+                                                                         +*/
/*+               The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-funktion will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+               SysPath:                                                  +*/
/*+                                                                         +*/
/*+               The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via NARE_GET_PATH_INFO.                    +*/
/*+                                                                         +*/
/*+               This function is most likely done asynchroneous because   +*/
/*+               of calling lower layer open channel (DCP)                 +*/
/*+               depending on channel type spezified in CDP (detail-ptr)   +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro NARE_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               NARE_RSP_ERR_OPCODE                                       +*/
/*+               NARE_RSP_ERR_PARAM                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID nare_open_channel(NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    NARE_RSP                  Status;
    LSA_UINT32                Finish;
    LSA_SYS_PTR_TYPE          pSys;
    NARE_DETAIL_PTR_TYPE      pDetail;
    LSA_UINT16                PathStat;
    NARE_HDB_USER_PTR_TYPE    pHDBUser;
    NARE_HDB_DCP_PTR_TYPE     pHDBDcp;
    NARE_HDB_EDD_PTR_TYPE     pHDBEdd;
    LSA_BOOL                  PathOpen;

    NARE_ENTER();
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_open_channel(pRQB: 0x%X)",pRQB);
    Status   = NARE_RSP_OK;
    Finish   = NOT_FINISHED;
    PathOpen = LSA_FALSE;
    {
        LSA_SYS_PTR_TYPE tmp = {0};
        pSys  = tmp; /* pSys may not be a pointer ! */
    }
    pDetail  = LSA_NULL;
    NARE_ASSERT_NULL_PTR(pRQB);
    /*----------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                   */
    /*----------------------------------------------------------------------------*/
    NARE_RQB_SET_HANDLE(pRQB,pRQB->args.channel.handle_upper); /* set upper handle */
    /*----------------------------------------------------------------------------*/
    /* Checks some parameters                                                     */
    /*----------------------------------------------------------------------------*/
    if ( NARE_RQB_GET_OPCODE(pRQB) != NARE_OPC_OPEN_CHANNEL )
    {
        NARE_RQB_SET_RESPONSE(pRQB, NARE_RSP_ERR_OPCODE);
        Status = NARE_RSP_ERR_OPCODE;
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,">>>: nare_open_channel(): Invalid RQB-Opcode (0x%X)",NARE_RQB_GET_OPCODE(pRQB));
        NARE_RQB_ERROR(pRQB);
    }
    else
    {
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_OPEN_CHANNEL (Handle: 0x%X)",NARE_RQB_GET_HANDLE(pRQB));
        if ( pRQB->args.channel.nare_request_upper_done_ptr == 0 )
        { /* AP00999037: throw fatal if NULL callback is given */
            NARE_FATAL(NARE_FATAL_ERR_NULL_PTR);
        }
    }
    /*----------------------------------------------------------------------------*/
    /* if all ok handle request                                                   */
    /*----------------------------------------------------------------------------*/
    if (Status == NARE_RSP_OK)
    {
        Finish = FINISHED_OK;
        /*------------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                                */
        /*------------------------------------------------------------------------*/
        NARE_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling NARE_GET_PATH_INFO.");
        NARE_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQB->args.channel.sys_path);
        if ( PathStat != LSA_RET_OK)
        {
            NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"<--: NARE_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
            NARE_FATAL(NARE_FATAL_ERR_PATHINFO);
        }
        else
        {
            NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"<--: NARE_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);
            if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
            {
                Status = NARE_RSP_ERR_PARAM;
                NARE_FATAL(NARE_FATAL_ERR_PATHINFO);
            }
            else
            {
                PathOpen = LSA_TRUE;
            }
        }
        /*------------------------------------------------------------------------*/
        /* Get Infos from Detail-Ptr (CDB) and start handling..                   */
        /*------------------------------------------------------------------------*/
        if ( Status == NARE_RSP_OK )
        {
            switch (pDetail->PathType)
            {       /* -----------------------------------------------------------*/
                    /* User-Channel.                                              */
                    /* -----------------------------------------------------------*/
                case NARE_PATH_TYPE_USER:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening USER-Channel");
                    /* check the detail ptr for correct parameter AP01513629*/
                    if (pDetail->Params.User.MaxRUser == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }
                    /* Check if we have DCP-Channel for NIC-ID. */
                    pHDBDcp  = NARE_DCPGetHandleByNIC(pDetail->Params.User.NICId);
                    pHDBEdd  = NARE_EDDGetHandleByNIC(pDetail->Params.User.NICId);
                    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBDcp,  LSA_NULL) ||
                         LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL))
                    {
                        Status = NARE_RSP_ERR_SEQUENCE;
                        /* Error on getting handle. Abort. */
                        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE USER-channel failed. No DCP/EDD-Channel present (NIC-ID: 0x%X)",pDetail->Params.User.NICId);
                    }
                    else
                    {   /* Get a free, initialized HDB */
                        Status = NARE_UserGetFreeHDB(pDetail->Params.User.MaxRUser,&pHDBUser);
                        if ( Status == NARE_RSP_OK )
                        {
                            NARE_ASSERT_NULL_PTR(pHDBUser);
                            pHDBUser->Params.pSys          = pSys;
                            pHDBUser->Params.pDetail       = pDetail;
                            pHDBUser->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                            pHDBUser->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBUser->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                            /* set our handle in RQB */
                            pRQB->args.channel.handle      = pHDBUser->ThisHandle;
                            pHDBUser->pHDBDcp              = pHDBDcp;
                            pHDBDcp->UserCnt++;
                            pHDBUser->pHDBEdd              = pHDBEdd;
                            pHDBEdd->UserCnt++;
                            pHDBUser->Hello.Status         = NARE_USR_HELLO_IDLE;
                            pHDBUser->Hello.pRQB           = LSA_NULL;
                            pHDBUser->TraceIdx             = pHDBEdd->TraceIdx;
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open NARE USER-channel failed. Cannot get USER handle! (Rsp: 0x%X).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* DCP -Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case NARE_PATH_TYPE_DCP:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening DCP- SystemChannel");
                    /* check the detail ptr for correct parameter AP01513629*/
                    if (pDetail->Params.Dcp.MaxRDCP == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }
                    /* Check if NIC-ID already used by a DCP-Channel. If so abort */
                    if ( NARE_DCPCheckNICUsed(pDetail->Params.Dcp.NICId))
                    {
                        Status = NARE_RSP_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE DCP-channel failed. NIC-ID already used (ID: 0x%X)",pDetail->Params.Dcp.NICId);
                    }
                    else
                    {   /* Get a free, initialized HDB */
                        Status = NARE_DCPGetFreeHDB(pDetail->Params.Dcp.MaxRDCP,&pHDBDcp,pSys);
                        if ( Status == NARE_RSP_OK )
                        {
                            NARE_ASSERT_NULL_PTR(pHDBDcp);
                            pHDBDcp->Params.pSys          = pSys;
                            pHDBDcp->Params.pDetail       = pDetail;
                            pHDBDcp->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                            pHDBDcp->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBDcp->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                            pHDBDcp->NoOfRCtrlCnt         = pDetail->Params.Dcp.MaxRDCP;
                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBDcp->ThisHandle;
                            /* Do a DCP- open channel request */
                            Status = NARE_DCPOpenChannel(pHDBDcp->ThisHandle,pHDBDcp->Params.SysPath,pHDBDcp->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {   /* Request is underway now.. */
                                pHDBDcp->State  = NARE_HANDLE_STATE_OPEN_DCP_RUNNING;
                                pHDBDcp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = NOT_FINISHED;
                            }
                            else
                            {
                                NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE-DCP channel failed. (Rsp: %Xh)",Status);
                                NARE_DCPReleaseHDB(pHDBDcp);
                            }
                        }
                        else
                        {
                            /* Error on getting handle. Abort. */
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open NARE-DCP channel failed. Cannot get DCP handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* EDD -Channel.                                                 */
                    /* --------------------------------------------------------------*/
                case NARE_PATH_TYPE_EDD:
                    NARE_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Opening EDD- SystemChannel");
                    if (pDetail->Params.Edd.MaxREDD == 0)
                    {
                        Status = NARE_RSP_ERR_PARAM;
                        break;
                    }
                    /* Check if NIC-ID already used by a EDD-Channel. If so abort */
                    if ( NARE_EDDCheckNICUsed(pDetail->Params.Edd.NICId))
                    {
                        Status = NARE_RSP_ERR_CHANNEL_USE;
                        /* Error on getting handle. wrong NICId. abort */
                        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE EDD-channel failed. NIC-ID already used (ID: %Xh)",pDetail->Params.Edd.NICId);
                    }
                    else
                    {   /* Get a free, initialized HDB */
                        Status = NARE_EDDGetFreeHDB(pDetail->Params.Edd.MaxREDD, &pHDBEdd,pSys);
                        if ( Status == NARE_RSP_OK )
                        {
                            NARE_ASSERT_NULL_PTR(pHDBEdd);
                            pHDBEdd->Params.pSys          = pSys;
                            pHDBEdd->Params.pDetail       = pDetail;
                            pHDBEdd->Params.Cbf           = pRQB->args.channel.nare_request_upper_done_ptr;
                            pHDBEdd->Params.SysPath       = pRQB->args.channel.sys_path;
                            pHDBEdd->Params.HandleUpper   = pRQB->args.channel.handle_upper;
                            pHDBEdd->SizeOfEDDRCtrlQue    = pDetail->Params.Edd.MaxREDD;
                            /* set our handle in RQB */
                            pRQB->args.channel.handle     = pHDBEdd->ThisHandle;
                            /* Do a EDD- open channel request */
                            Status = NARE_EDDOpenChannel(pHDBEdd->ThisHandle,pHDBEdd->Params.SysPath,pHDBEdd->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBEdd->State  = NARE_HANDLE_STATE_OPEN_EDD_RUNNING;
                                pHDBEdd->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish          = NOT_FINISHED;
                            }
                            else
                            {
                                NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Open NARE-EDD channel failed. (Rsp: %Xh)",Status);
                                NARE_EDDReleaseHDB(pHDBEdd);
                            }
                        }
                        else
                        {   /* Error on getting handle. Abort. */
                            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Open NARE-EDD channel failed. Cannot get EDD handle! (Rsp: %Xh).",Status);
                        }
                    }
                    break;

                default:
                    NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Unknown PathType in CDB (Type: %Xh)",pDetail->PathType);
                    Status = NARE_RSP_ERR_PARAM;
                    break;
            }/* switch */
        }
    }
    /*----------------------------------------------------------------------------*/
    /* Should we finish the request ?                                             */
    /*----------------------------------------------------------------------------*/
    if ( Finish )  /* finish the request ? */
    {
        /* upper - handle already set */
        NARE_RQB_SET_RESPONSE(pRQB,Status);
        NARE_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Status);
        NARECallCbf(pRQB->args.channel.nare_request_upper_done_ptr,pRQB,pSys);
        /*----------------------------------------------------------------------------*/
        /* On error, if Syspath was opened we close the path                          */
        /*----------------------------------------------------------------------------*/
        if ( ( Status != NARE_RSP_OK ) &&  ( PathOpen)  )
        {
            NARE_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"-->: Calling NARE_RELEASE_PATH_INFO.");
            NARE_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);
            if ( PathStat != LSA_RET_OK)
            {
                NARE_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"<--: NARE_RELEASE_PATH_INFO failed (Rsp: 0x%X).",PathStat);
                NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
            }
        }
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_open_channel()");
    NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_close_channel                          +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       NARE_OPC_CLOSE_CHANNEL        +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     NARE_RSP                Response      -                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error         -                             +*/
/*+     NARE_RQB_ARGS_TYPE      Args          -                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user.       +*/
/*+     NARE_RSP                Response    NARE_RSP_OK                     +*/
/*+                                         NARE_RSP_ERR_RESOURCE           +*/
/*+                                         NARE_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes a channel.                                         +*/
/*+               Close lower-layer channel if neccessary. (depends on      +*/
/*+               channel type). Releases Path-info. Note that this         +*/
/*+               function is done asynchron because of calling lower layer +*/
/*+               functions with callback. On error we may finish synchron. +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro NARE_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               NARE_RSP_ERR_OPCODE                                       +*/
/*+               NARE_RSP_ERR_PARAM                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID nare_close_channel(NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    NARE_RSP                  Status;
    LSA_HANDLE_TYPE           Handle;
    LSA_UINT32                Finish;
    LSA_HANDLE_TYPE           HandleUpper;
    NARE_HDB_USER_PTR_TYPE    pHDBUser = LSA_NULL;
    NARE_HDB_DCP_PTR_TYPE     pHDBDcp;
    NARE_HDB_EDD_PTR_TYPE     pHDBEdd;
    LSA_UINT16                PathStat;
    LSA_SYS_PTR_TYPE          pSys;
    NARE_UPPER_RQB_PTR_TYPE   pRQBHelp;
    LSA_VOID                  LSA_FCT_PTR(/*ATTR*/, Cbf) (NARE_UPPER_RQB_PTR_TYPE pRQB);

    NARE_ENTER();
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_close_channel(pRQB: 0x%X)",pRQB);
    Status      = NARE_RSP_OK;
    Finish      = NOT_FINISHED;
    HandleUpper = 0;
    Handle      = 0;
    Cbf         = 0;
    NARE_ASSERT_NULL_PTR(pRQB);
    /*---------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                  */
    /*---------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if ( NARE_RQB_GET_OPCODE(pRQB) != NARE_OPC_CLOSE_CHANNEL )
    {
        NARE_RQB_SET_RESPONSE(pRQB, NARE_RSP_ERR_OPCODE);
        Status = NARE_RSP_ERR_OPCODE;
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,">>>: nare_close_channel(): Invalid RQB-Opcode (0x%X)",NARE_RQB_GET_OPCODE(pRQB));
        NARE_RQB_ERROR(pRQB);
    }
    else
    {
        NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_CLOSE_CHANNEL (Handle: 0x%X)",NARE_RQB_GET_HANDLE(pRQB));
        /*-----------------------------------------------------------------------*/
        /* Check if it is allowed to close this channel.                         */
        /*-----------------------------------------------------------------------*/
        Handle = NARE_RQB_GET_HANDLE(pRQB);
        switch ( NARE_HANDLE_GET_TYPE(Handle))
        {
                /* ------------------------------------------------------------------*/
                /* USER Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_USER:
                pHDBUser = NAREGetUserHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser, LSA_NULL) )
                {
                    Finish      = FINISHED_REL_SYSPTR;  /* normal close */
                    Cbf         = pHDBUser->Params.Cbf;
                    pSys        = pHDBUser->Params.pSys;
                    HandleUpper = pHDBUser->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    /* abort all NOTIFY-requests */
                    while (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->NotifyReq.pBottom, LSA_NULL) )
                    {
                        NARE_RQB_REM_BLOCK_BOTTOM(pHDBUser->NotifyReq.pBottom,
                                                  pHDBUser->NotifyReq.pTop,
                                                  pRQBHelp);
                        /* finish the request */
                        NAREUserRequestFinish(pHDBUser,pRQBHelp,NARE_RSP_OK_CANCEL);
                    }
                    if (( pHDBUser->UserRCtrl.Used ) ||
                        ( pHDBUser->UserReqPendCnt ) ||
                        ( pHDBUser->Hello.Status != NARE_USR_HELLO_IDLE))
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. User requests pending");
                        Status = NARE_RSP_ERR_SEQUENCE;
                        Finish = FINISHED_OK;
                    }
                    else
                    {
                        pHDBUser->pHDBDcp->UserCnt--;
                        pHDBUser->pHDBEdd->UserCnt--;
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-User Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* DCP -Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_DCP:
                pHDBDcp = NAREGetDcpHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBDcp, LSA_NULL) )
                {
                    Finish      = FINISHED_OK;
                    Cbf         = pHDBDcp->Params.Cbf;
                    pSys        = pHDBDcp->Params.pSys;
                    HandleUpper = pHDBDcp->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    if (( pHDBDcp->UserCnt ) ||                          /* some user-channel uses this DCP-channel ?    */
                        ( pHDBDcp->Hello.Status != NARE_DCP_HELLO_IDLE)) /* should not occur if all user channel closed! */
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Channel still in use by user-channels");
                        Status = NARE_RSP_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBDcp->pOCReq, LSA_NULL))
                        {
                            NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Close already running.");
                            Status = NARE_RSP_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = NARE_DCPCloseChannel(pHDBDcp->DCPHandle,pHDBDcp->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBDcp->State  = NARE_HANDLE_STATE_CLOSE_DCP_RUNNING;
                                pHDBDcp->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = NOT_FINISHED;
                            }
                        }
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-DCP Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
                break;

                /* ------------------------------------------------------------------*/
                /* EDD -Channel.                                                     */
                /* ------------------------------------------------------------------*/
            case NARE_HANDLE_TYPE_EDD:
                pHDBEdd = NAREGetEddHDBFromHandle(Handle);
                if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd, LSA_NULL) )
                {
                    Finish      = FINISHED_OK;
                    Cbf         = pHDBEdd->Params.Cbf;
                    pSys        = pHDBEdd->Params.pSys;
                    HandleUpper = pHDBEdd->Params.HandleUpper;
                    NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
                    if ( pHDBEdd->UserCnt ) /* some user-channel uses this EDD-channel ?*/
                    {
                        NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Channel still in use by user-channels");
                        Status = NARE_RSP_ERR_SEQUENCE;
                    }
                    else
                    {
                        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd->pOCReq, LSA_NULL))
                        {
                            NARE_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Close channel failed. Close already running");
                            Status = NARE_RSP_ERR_SEQUENCE;
                        }
                        else
                        {
                            Status = NARE_EDDCloseChannel(pHDBEdd->EDDHandle,pHDBEdd->Params.pSys);
                            if ( Status == NARE_RSP_OK )
                            {
                                /* Request is underway now.. */
                                pHDBEdd->State  = NARE_HANDLE_STATE_CLOSE_EDD_RUNNING;
                                pHDBEdd->pOCReq = pRQB; /* save pointer to request for later use */
                                Finish = NOT_FINISHED;
                            }
                        }
                    }
                }
                else
                {
                    NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid NARE-EDD Handle! (0x%X)",Handle);
                    Status = NARE_RSP_ERR_PARAM;
                    NARE_RQB_SET_RESPONSE(pRQB, Status);
                    NARE_RQB_ERROR(pRQB);
                }
                break;

            default:
                NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"nare_close_channel(): Invalid Handle (0x%X)",Handle);
                Status = NARE_RSP_ERR_PARAM;
                NARE_RQB_SET_RESPONSE(pRQB, Status);
                NARE_RQB_ERROR(pRQB);
                break;
        }
    }
    /*---------------------------------------------------------------------------*/
    /* Finish the request.                                                       */
    /*---------------------------------------------------------------------------*/
    if ( Finish != NOT_FINISHED)
    {
        NARE_DETAIL_PTR_TYPE    pDetail;

        NARE_RQB_SET_RESPONSE(pRQB,Status);
        NARE_RQB_SET_HANDLE(pRQB, HandleUpper );
        NARE_ASSERT_NULL_PTR(pHDBUser);
        NARE_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),Status);
        /* take the values out of the HDB */
        pSys    = pHDBUser->Params.pSys;
        pDetail = pHDBUser->Params.pDetail;
        if (Finish == FINISHED_REL_SYSPTR)
        {   /* we release the HDB. Handle closed now        */
            NARE_UserReleaseHDB(pHDBUser);
        }
        NARECallCbf(Cbf,pRQB,pSys);
        if (Finish == FINISHED_REL_SYSPTR)
        {   /* release PATH-Info !                          */
            NARE_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);
            if (PathStat != LSA_RET_OK )
            {
                NARE_FATAL(NARE_FATAL_ERR_RELEASE_PATH_INFO);
            }
        }
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_close_channel()");
    NARE_EXIT();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    nare_request                                +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       varies:                       +*/
/*+                                           NARE_OPC_IDENT                +*/
/*+                                           NARE_OPC_SET                  +*/
/*+                                           NARE_OPC_TIMER                +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     NARE_RSP                Response      -                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error         -                             +*/
/*+     NARE_RQB_ARGS_TYPE      Args          varies depending on opcode    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     upper-handle from open_channel  +*/
/*+     NARE_RSP                Response    NARE_RSP_OK                     +*/
/*+                                         NARE_RSP_ERR_RESOURCE           +*/
/*+                                         NARE_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     NARE_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
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
/*+               case we call the output-macro NARE_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               NARE_RSP_ERR_OPCODE                                       +*/
/*+               NARE_RSP_ERR_PARAM                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID nare_request(NARE_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_HANDLE_TYPE           Handle;
    NARE_HDB_USER_PTR_TYPE    pHDB;

    NARE_ENTER();
    NARE_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"IN : nare_request(pRQB: 0x%X)",pRQB);
    NARE_ASSERT_NULL_PTR(pRQB);
    Handle   = NARE_RQB_GET_HANDLE(pRQB);
    /* TIMER-Request needs special handling  */
    if (  NARE_RQB_GET_OPCODE(pRQB) == NARE_OPC_TIMER )
    {
        NARERequestTimer(pRQB);
    }
    else
    {
        pHDB = NAREGetUserHDBFromHandle(Handle);
        if (! LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
        {
            switch ( NARE_RQB_GET_OPCODE(pRQB))
            {
                case NARE_OPC_IDENT:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE, ">>>: Request: NARE_OPC_IDENT (pRQB: 0x%X, Handle: 0x%X)", pRQB, NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestIdent(pRQB, pHDB);
                    break;

                case NARE_OPC_CANCEL:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE, ">>>: Request: NARE_OPC_CANCEL (pRQB: 0x%X, Handle: 0x%X)", pRQB, NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestCancel(pRQB, pHDB);
                    break;

                case NARE_OPC_SET:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE, ">>>: Request: NARE_OPC_SET (pRQB: 0x%X, Handle: 0x%X)", pRQB, NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestSet(pRQB, pHDB);
                    break;

                case NARE_OPC_IDENT_NOTIFY:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE, ">>>: Request: NARE_OPC_IDENT_NOTIFY (pRQB: 0x%X, Handle: 0x%X)", pRQB, NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestIdentNotify(pRQB, pHDB);
                    break;

                case NARE_OPC_TEST_IP:
                    NARE_UPPER_TRACE_03(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_TEST_IP (pRQB: 0x%X, Handle: 0x%X Delay: 0x%x)",pRQB,NARE_RQB_GET_HANDLE(pRQB), pRQB->args.TestIP.UseDelay);
                    NARERequestTestIP(pRQB,pHDB);
                    break;

                case NARE_OPC_RESOLVE_IP:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_RESOLVE_IP (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestResolveIP(pRQB,pHDB);
                    break;

                case NARE_OPC_HELLO:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_HELLO (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestHello(pRQB,pHDB);
                    break;

                case NARE_OPC_HELLO_STOP:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,">>>: Request: NARE_OPC_HELLO_STOP (pRQB: 0x%X, Handle: 0x%X)",pRQB,NARE_RQB_GET_HANDLE(pRQB));
                    NARERequestHelloStop(pRQB,pHDB);
                    break;

                default:
                    NARE_UPPER_TRACE_02(pHDB->TraceIdx, LSA_TRACE_LEVEL_ERROR,">>>: nare_request(): Invalid RQB-Opcode (pRQB: 0x%X, Opcode: 0x%X)",pRQB,NARE_RQB_GET_OPCODE(pRQB));
                    NAREUserRequestFinish(pHDB,pRQB,NARE_RSP_ERR_OPCODE);
                    break;
            } /* switch "opcode" */
        } /* if valid pHDB */
        else
        {   /* wrong handle */
            NARE_RQB_SET_RESPONSE(pRQB, NARE_RSP_ERR_PARAM);
            NARE_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,">>>: nare_request(): Invalid Handle (0x%X)",Handle);
            NARE_RQB_ERROR(pRQB);
        }
    } /* else */
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: nare_request()");
    NARE_EXIT();
}

/*****************************************************************************/
/*  end of file NARE_USR.C                                                    */
/*****************************************************************************/

