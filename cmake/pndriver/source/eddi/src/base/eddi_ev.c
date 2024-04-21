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
/*  F i l e               &F: eddi_ev.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-Timer functions                              */
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

#define EDDI_MODULE_ID     M_ID_EDDI_EV
#define LTRC_ACT_MODUL_ID  9

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"

#include "eddi_lock.h"
#include "eddi_ev.h"
#include "eddi_ext.h"


/***************************************************************************/
/* F u n c t i o n:       EDDI_EvTimerREST()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_EvTimerREST( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_SHED_OBJ_TYPE  *  const  pShedObj = (EDDI_SHED_OBJ_TYPE *)pRQB->internal_context;
    EDDI_TIMER_TYPE     *  const  pTimer   = (EDDI_TIMER_TYPE *)pShedObj->Para;
    EDDI_DDB_TYPE       *  const  pDDB     = (EDDI_DDB_TYPE *)pRQB->internal_context_1;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_EvTimerREST->");

    if (pTimer->used == 0)
    {
        //can occur due to racecondition
        return;
    }

    //check if device is closing and waits for internal RQBs to stop
    if (pDDB->Glob.pCloseDevicePendingRQB)
    {
        LSA_RESULT                          Status;
        EDDI_UPPER_DDB_REL_PTR_TYPE  const  pRqbDDBRel = (EDDI_UPPER_DDB_REL_PTR_TYPE)pDDB->Glob.pCloseDevicePendingRQB->pParam;

        //device is closing ==> take timer off duty
        EDDI_STOP_TIMER(&Status, pTimer->SysTimerID);   //Stop this timer to prevent it´s being triggered again
        pShedObj->bUsed = LSA_FALSE;                    //Mark attached static RQB as "unused"

        EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_EvTimerREST->Closing...");

        if (!(0 == pRqbDDBRel->Cbf))
        {
            EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); //try to close device now. if successful, the cbf is called
        }
    }
    else
    {
        //"normal" timer reaction
        EDDI_ENTER_REST_S();
        pShedObj->bUsed = LSA_FALSE;
        pTimer->cbf(pTimer->context);
        EDDI_EXIT_REST_S();
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ev.c                                                    */
/*****************************************************************************/
