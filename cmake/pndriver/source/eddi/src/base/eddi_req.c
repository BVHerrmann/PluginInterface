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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_req.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-Device and handle-management functions      */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ext.h"
#include "eddi_swi_ext.h"
#include "eddi_crt_ext.h"

#include "eddi_sync_usr.h"

#include "eddi_prm_req.h"
//#include "eddi_crt_brq.h"

#include "eddi_req.h"
#include "eddi_lock.h"

#include "eddi_nrt_usr.h"
//#include "eddi_nrt_arp.h"
#include "eddi_nrt_oc.h"

#if defined (EDDI_CFG_FRAG_ON)
//#include "eddi_nrt_q.h"
#endif

#include "eddi_ev.h"

#define EDDI_MODULE_ID     M_ID_EDDI_REQ
#define LTRC_ACT_MODUL_ID  14

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_NRTReq( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_SYNCReq( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_RESTReq( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_ReqErr( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                  EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_ErrService( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_NrtReqErr( EDD_UPPER_RQB_PTR_TYPE  const  pRQB,
                                                     LSA_RESULT              const  Status );

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_RequestError( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                        EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                        LSA_RESULT               const  Status );

#define MD_EDDI_REQ_SIZE   0xF

typedef struct _EDDI_REQ_REST
{
    EDDI_HDB_REQ_FCT  fct;
    LSA_UINT32        UsedComp;

} EDDI_REQ_REST;

static  EDDI_REQ_REST  const  MD_EDDI_RESTReq[] =
{
    /* -0-- */ { (EDDI_HDB_REQ_FCT)EDDI_CRTRequest,     EDDI_COMP_XRT },        //service-group  //EDDI_HDB_REQ_FCT has 2 arguments!
    /* -1-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -2-- */ { (EDDI_HDB_REQ_FCT)EDDI_SwiUsrRequest,  EDDI_COMP_SWI },        //service-group
    /* -3-- */ { (EDDI_HDB_REQ_FCT)EDDI_GenRequest,     EDDI_COMP_GENERAL },    //service-group
    /* -4-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -5-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -6-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -7-- */ { (EDDI_HDB_REQ_FCT)EDDI_PrmRequest,     EDDI_COMP_PRM },        //service-group
    /* -8-- */ { (EDDI_HDB_REQ_FCT)EDDI_NRTSetFilters,  EDDI_COMP_NRT },        //service-group
    /* -9-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -a-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -b-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -c-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -d-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -e-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL },
    /* -f-- */ { (EDDI_HDB_REQ_FCT)EDDI_ReqErr,         EDDI_COMP_GENERAL }
};

static  EDDI_REQ  const  MD_EDDI_NRTReq[] =
{
    /* ---0 */ { (EDDI_REQ_FCT)EDDI_NRTRecv },       //EDDI_REQ_FCT has 1 argument!
    /* ---1 */ { (EDDI_REQ_FCT)EDDI_NRTSendUsr },
    /* ---2 */ { (EDDI_REQ_FCT)EDDI_NRTCancel },
    /* ---3 */ { (EDDI_REQ_FCT)EDDI_ReqErr },
    /* ---4 */ { (EDDI_REQ_FCT)EDDI_ReqErr },
    /* ---5 */ { (EDDI_REQ_FCT)EDDI_ReqErr },
    /* ---6 */ { (EDDI_REQ_FCT)EDDI_NRTSendTimestamp },
    /* ---7 */ { (EDDI_REQ_FCT)EDDI_ReqErr },
    /* ---8 */ { (EDDI_REQ_FCT)EDDI_ReqErr },
    /* ---9 */ { (EDDI_REQ_FCT)EDDI_ReqErr },
    /* ---a */ { (EDDI_REQ_FCT)EDDI_ErrService },
    /* ---b */ { (EDDI_REQ_FCT)EDDI_ErrService },
    /* ---c */ { (EDDI_REQ_FCT)EDDI_ErrService },
    /* ---d */ { (EDDI_REQ_FCT)EDDI_ErrService },
    /* ---e */ { (EDDI_REQ_FCT)EDDI_ErrService },
    /* ---f */ { (EDDI_REQ_FCT)EDDI_ErrService }
};


/***************************************************************************/
/* F u n c t i o n:       EDDI_IniRequestFctTable()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IniRequestFctTable( LSA_VOID )
{
    LSA_UINT32  Ctr, Number;

    static  const  EDDI_REQ  eddi_req[] =
    {
        /* 0--- */ { (EDDI_REQ_FCT)EDDI_ErrService },           //EDDI_REQ_FCT has 1 argument!
        /* 1--- */ { (EDDI_REQ_FCT)EDDI_NRTReq },
        /* 2--- */ { (EDDI_REQ_FCT)EDDI_RESTReq },
        /* 3--- */ { (EDDI_REQ_FCT)EDDI_EvTimerREST },
        /* 4--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        /* 5--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        /* 6--- */ { (EDDI_REQ_FCT)EDDI_SYNCReq },
        /* 7--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
        /* 8--- */ { (EDDI_REQ_FCT)EDDI_SIIEvPrio1Aux },        //do not change without changing service-define!
        #else
        /* 8--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        #endif
        /* 9--- */ { (EDDI_REQ_FCT)EDDI_SIIEvPrio2Org },        //do not change without changing service-define!
        /* a--- */ { (EDDI_REQ_FCT)EDDI_SIIEvPrio3Rest },       //do not change without changing service-define!
        /* b--- */ { (EDDI_REQ_FCT)EDDI_SIIEvPrio4NRTLow },     //do not change without changing service-define!
        #if defined (EDDI_CFG_SYSRED_2PROC)
        /* c--- */ { (EDDI_REQ_FCT)EDDI_SysRedPoll },           //do not change without changing service-define!
        #else
        /* c--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        #endif
        /* d--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        /* e--- */ { (EDDI_REQ_FCT)EDDI_ErrService },
        /* f--- */ { (EDDI_REQ_FCT)EDDI_ErrService }
    };

    Number = sizeof(eddi_req) / sizeof(EDDI_REQ);
    for (Ctr = 0; Ctr < Number; Ctr++)
    {
        g_pEDDI_Info ->eddi_req[Ctr].fct = eddi_req[Ctr].fct;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_request()                                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_request( EDD_UPPER_RQB_PTR_TYPE  pRQB  )
{
    LSA_UINT32  const  Index = EDD_RQB_GET_SERVICE(pRQB) >> 12;

    EDDI_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "eddi_request->EDD_RQB_GET_SERVICE(pRQB):0x%X", EDD_RQB_GET_SERVICE(pRQB));

    if (Index > MD_EDDI_REQ_SIZE)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eddi_request, Index:0x%X", Index);
        EDDI_Excp("eddi_request, Servicenumber is too big!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    g_pEDDI_Info ->eddi_req[Index].fct(pRQB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RESTReq( EDD_UPPER_RQB_PTR_TYPE  const   pRQB)
{
    LSA_RESULT                      Status;
    EDDI_LOCAL_HDB_PTR_TYPE         pHDB;
    LSA_UINT32               const  Index  = (EDD_RQB_GET_SERVICE(pRQB) >> 8) & 0xF;
    
    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    if (EDD_STS_OK == Status)
    {
        EDDI_UPPER_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RESTReq->EDD_RQB_GET_SERVICE(pRQB):0x%X", EDD_RQB_GET_SERVICE(pRQB));
    }

    //map the Bits31..12 of Service Number to Index (--> Component-Id)

    if (Index > MD_EDDI_REQ_SIZE)
    {
        EDDI_PROGRAM_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RESTReq, Index:0x%X", Index);
        EDDI_Excp("EDDI_RESTReq, Servicenumber is too big!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    EDDI_ENTER_REST_S();

    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        Status = EDD_STS_ERR_PARAM;
        EDDI_RQB_ERROR(pRQB);
        EDDI_EXIT_REST_S();
        return;
    }

    if (EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_REQUEST)
    {
        EDDI_RequestError(pRQB, pHDB, EDD_STS_ERR_OPCODE);
        EDDI_EXIT_REST_S();
        return;
    }

    pRQB->internal_context   = pHDB; //store pHDB for later use
    pRQB->internal_context_1 = 0;

    //clear Detail Error
    pHDB->pDDB->ErrDetail.bSet = 0;

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        EDDI_RequestError(pRQB, pHDB, EDD_STS_ERR_PARAM);
        EDDI_EXIT_REST_S();
        return;
    }

    //map the Bits31..12 of Service Number to Index (--> Component-Id)

    if (!(pHDB->UsedComp & MD_EDDI_RESTReq[Index].UsedComp))
    {
        EDDI_RequestError(pRQB, pHDB, EDD_STS_ERR_CHANNEL_USE);
        EDDI_EXIT_REST_S();
        return;
    }

    MD_EDDI_RESTReq[Index].fct(pRQB, pHDB);

    EDDI_EXIT_REST_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SYNCReq( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB;
    
    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    
    //Index = (EDD_RQB_GET_SERVICE(pRQB) >> 8) & 0xF;

    if (EDD_STS_OK == Status)
    {
        EDDI_UPPER_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SYNCReq->EDD_RQB_GET_SERVICE(pRQB):0x%X", EDD_RQB_GET_SERVICE(pRQB));
    }

    //map the Bits31..12 of Service Number to Index (--> Component-Id)

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    EDDI_ENTER_SYNC_S();

    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        Status = EDD_STS_ERR_PARAM;
        EDDI_RQB_ERROR(pRQB);
        EDDI_EXIT_SYNC_S();
        return;
    }

    if ((EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_REQUEST))
    {
        EDDI_RequestError(pRQB, pHDB, EDD_STS_ERR_OPCODE);
        EDDI_EXIT_SYNC_S();
        return;
    }

    pRQB->internal_context   = pHDB; //store pHDB for later use
    pRQB->internal_context_1 = 0;

    //Clear Detail Error
    pHDB->pDDB->ErrDetail.bSet = 0;

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        EDDI_RequestError(pRQB, pHDB, EDD_STS_ERR_PARAM);
        EDDI_EXIT_SYNC_S();
        return;
    }

    //map the Bits31..12 of Service Number to Index (--> Component-Id)
    if (!(pHDB->UsedComp & EDDI_COMP_SYNC) || !(pHDB->UsedComp & EDDI_COMP_NRT))
    {
        EDDI_RequestError(pRQB, pHDB, EDD_STS_ERR_CHANNEL_USE);
        EDDI_EXIT_SYNC_S();
        return;
    }

    EDDI_SYNCRequest(pRQB, pHDB);

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTReq()                                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTReq( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_UINT32               const  Index  = EDD_RQB_GET_SERVICE(pRQB) & 0xF;
    LSA_RESULT                      Status;
    EDDI_LOCAL_HDB_PTR_TYPE         pHDB;

    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    if (Status != EDD_STS_OK)
    {
        return;
    }

    EDDI_UPPER_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTReq->EDD_RQB_GET_SERVICE(pRQB):0x%X", EDD_RQB_GET_SERVICE(pRQB));

    //map the Bits31..12 of Service Number to Index (--> Component-Id)

    if (Index > MD_EDDI_REQ_SIZE)
    {
        EDDI_PROGRAM_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTReq, Index:0x%X", Index);
        EDDI_Excp("EDDI_NRTReq, Servicenumber is too big!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    if (EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_REQUEST)
    {
        EDDI_NrtReqErr(pRQB, EDD_STS_ERR_OPCODE);
        return;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        EDDI_NrtReqErr(pRQB, EDD_STS_ERR_PARAM);
        return;
    }

    //map the Bits31..12 of Service Number to Index (--> Component-Id)

    MD_EDDI_NRTReq[Index].fct(pRQB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtReqErr( EDD_UPPER_RQB_PTR_TYPE  const  pRQB,
                                                       LSA_RESULT              const  Status )
{
    LSA_RESULT               LocStatus;
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB;

    LSA_UNUSED_ARG(Status);

    LocStatus = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    if (LocStatus != EDD_STS_OK)
    {
        return;
    }

    EDDI_PROGRAM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NrtReqErr->");

    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
    EDDI_RQB_ERROR(pRQB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RequestError( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                          LSA_RESULT               const  Status )
{
    EDDI_PROGRAM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RequestError->");

    if (EDD_RQB_GET_OPCODE(pRQB) == EDD_OPC_REQUEST_SRT_BUFFER)
    {
        EDD_RQB_SET_RESPONSE(pRQB, Status);
        EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);
    }
    else
    {
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_ErrService( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB;

    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    if (Status != EDD_STS_OK)
    {
        return;
    }

    EDDI_PROGRAM_TRACE_02(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_ErrService->Opc:0x%X, Service:0x%X",
                          EDD_RQB_GET_OPCODE(pRQB), EDD_RQB_GET_SERVICE(pRQB));

    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_SERVICE);
    EDDI_RQB_ERROR(pRQB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_ReqErr( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                    EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_PROGRAM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_ReqErr->");

    EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_SERVICE);
    EDDI_RQB_ERROR(pRQB);

    EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_SERVICE);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_StopUser( LSA_VOID )
{
    LSA_UINT32         Ctr;
    LSA_UINT32  const  LastIndex = sizeof(g_pEDDI_Info ->eddi_req) / sizeof(EDDI_REQ);

    for (Ctr = 0; Ctr < LastIndex; Ctr++)
    {
        g_pEDDI_Info ->eddi_req[Ctr].fct = EDDI_ErrService;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_req.c                                                   */
/*****************************************************************************/

