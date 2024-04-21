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
/*  F i l e               &F: eddi_sync_check.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
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

#include "eddi_sync_check.h"
#include "eddi_sync_ir.h"

#define EDDI_MODULE_ID     M_ID_SYNC_CHECK
#define LTRC_ACT_MODUL_ID  130

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncIrCheckOverlapIrtIsrt()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SyncIrCheckOverlapIrtIsrt( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                               EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pIrtRecordSet,
                                                               LSA_UINT32                    const  ReservedIntervalBeginNs )
{
    LSA_UINT32  IrtMaxTimeNs;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncIrCheckOverlapIrtIsrt->");

    IrtMaxTimeNs = EDDI_SyncIrGetTxMaxTimeOfAllPorts(pDDB, pIrtRecordSet);

    if (IrtMaxTimeNs > ReservedIntervalBeginNs)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                           "EDDI_SyncIrCheckOverlapIrtIsrt, IrtMaxTime[ns]:0x%X ReservedIntervalBegin[ns]:0x%X",
                           IrtMaxTimeNs, ReservedIntervalBeginNs);
        return LSA_FALSE;
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_sync_check.c                                            */
/*****************************************************************************/

