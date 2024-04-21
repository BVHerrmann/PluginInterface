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
/*  F i l e               &F: eddi_irt_class2_stm.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
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
//#include "eddi_crt_phase_ext.h"

#include "eddi_ser_ext.h"
//#include "eddi_sync_ir.h"
#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_IRT_CLASS2_STM
#define LTRC_ACT_MODUL_ID  121

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass2StmsTrace( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_BOOL                 const  OldRtClass2_OutputState,
                                                                LSA_BOOL                 const  NewRtClass2_OutputState );


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass2StmsInit()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass2StmsInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex )
{
    EDDI_LOCAL_DDB_RT_CLASS2_MACHINE_PTR_TYPE  const  pRtClass2_Machine = &(pDDB->SYNC.PortMachines.RtClass2_Machine[HwPortIndex]);

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IrtClass2StmsInit");

    //init rt-class2-output-state
    pRtClass2_Machine->RtClass2_Prepared    = LSA_FALSE;
    pRtClass2_Machine->RtClass2_OutputState = LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass2StmsTrigger()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass2StmsTrigger( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex )
{
    LSA_BOOL                                          NewRtClass2_OutputState;
    EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE     const  pPortInputData    = &(pDDB->SYNC.PortMachines.Input.PortData[HwPortIndex]);
    EDDI_LOCAL_DDB_RT_CLASS2_MACHINE_PTR_TYPE  const  pRtClass2_Machine = &(pDDB->SYNC.PortMachines.RtClass2_Machine[HwPortIndex]);
    EDDI_PRM_RECORD_IRT_PTR_TYPE               const  pRecordSet        = pDDB->PRM.PDIRData.pRecordSet_A;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IrtClass2StmsTrigger");

    //Handle Preparation for RTClass2-ReservedInterval during PRM-COMMIT
    if (!pRtClass2_Machine->RtClass2_Prepared)
    {
        if (pDDB->SYNC.PortMachines.Input.RtClass2_isReservedIntervalPresent)
        {
            //++++++++++++++++++++++++++++++++++++
            //++   Prepare all iSRT - Elements
            //++++++++++++++++++++++++++++++++++++
            LSA_UINT32  IsrtBegTime10Ns;
            LSA_UINT32  IsrtEndTime10Ns;

            if (pDDB->PRM.PDIRData.State_A != EDDI_PRM_VALID) // no IRT in PRM
            {
                //Time for RootAcw = Reserved Interval Begin
                {
                    LSA_UINT32  const  CorrectionBegTimeNs = EDDI_PREFRAME_SEND_DELAY_100MBIT_NS + EDDI_ROOT_ACW_LEAD_TIME_NS;
                    LSA_UINT32  const  IsrtBegTimeNs       = pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin;

                    if (IsrtBegTimeNs > CorrectionBegTimeNs)
                    {
                        IsrtBegTime10Ns = (IsrtBegTimeNs - CorrectionBegTimeNs) / 10;
                    }
                    else
                    {
                        IsrtBegTime10Ns = 0;
                    }
                    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, Preparing RootAcw (using corrected ReservedInterval.Begin), IsrtBegTime10Ns:0x%X", IsrtBegTime10Ns);
                }

                //Time for Acw-EOL = Reserved Interval End
                {
                    LSA_UINT32  const  CorrectionEndTimeNs = EDDI_PREFRAME_SEND_DELAY_100MBIT_NS;
                    LSA_UINT32  const  IsrtEndTimeNs       = pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd;

                    if (IsrtEndTimeNs > CorrectionEndTimeNs)
                    {
                        IsrtEndTime10Ns = (IsrtEndTimeNs - CorrectionEndTimeNs) / 10;
                    }
                    else
                    {
                        IsrtEndTime10Ns = 0;
                    }
                    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, Preparing Acw-EOL (using corrected ReservedInterval.End), IsrtEndTime10Ns:0x%X", IsrtEndTime10Ns);
                }
            }
            else //IRT is in PRM
            {
                //Time for RootAcw from IRBeginEndDataRecord
                IsrtBegTime10Ns = pRecordSet->MetaData.GlobalTxAcwStartTimeMac10Ns;
                EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, Preparing RootAcw (using corrected IRBeginEndDataRecord) IsrtBegTime10Ns:0x%X", IsrtBegTime10Ns);

                //Time for Acw-EOL from IRBeginEndDataRecord
                IsrtEndTime10Ns = pRecordSet->MetaData.GlobalTxAcwEndTimeMac10Ns;
                EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, Preparing Acw-EOL (using corrected IRBeginEndDataRecord) IsrtEndTime10Ns:0x%X", IsrtEndTime10Ns);
            }

            // 1) Setting RootAcw
            EDDI_SERUpdateRootAcw(pDDB->CRT.RTClass2Interval.pRootAcw, IsrtBegTime10Ns, pDDB);

            // 2) Setting Acw-EOL
            EDDI_SERUpdateEOL(pDDB->pLocal_CRT->RTClass2Interval.pEOL, IsrtEndTime10Ns);

            pRtClass2_Machine->RtClass2_Prepared = LSA_TRUE;
        }
    }
    else
    {
        if (!pDDB->SYNC.PortMachines.Input.RtClass2_isReservedIntervalPresent)
        {
            //++++++++++++++++++++++++++++++++++++
            //++   Neutralize all iSRT - Elements
            //++++++++++++++++++++++++++++++++++++

            LSA_UINT32  const  IsrtBegTime10Ns = 0;
            LSA_UINT32  const  IsrtEndTime10Ns = 0;

            //Time for RootAcw = 0
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, Resetting RootAcw IsrtBegTime10Ns:0x%X", IsrtBegTime10Ns);
            EDDI_SERUpdateRootAcw(pDDB->CRT.RTClass2Interval.pRootAcw, IsrtBegTime10Ns, pDDB);

            //Time for ACW-EOL = 0
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, Resetting Acw-EOL IsrtEndTime10Ns:0x%X", IsrtEndTime10Ns);
            EDDI_SERUpdateEOL(pDDB->pLocal_CRT->RTClass2Interval.pEOL, IsrtEndTime10Ns);

            pRtClass2_Machine->RtClass2_Prepared = LSA_FALSE;
        }
    }

    //process new rt-class2-output-state according state-machine
    if (   (pRtClass2_Machine->RtClass2_Prepared)
        && (pDDB->SYNC.PortMachines.Input.RtClass2_isReservedIntervalPresent)
        && (pPortInputData->isPortStateOk)
        && (pPortInputData->isMauTypeOk)
        && (pDDB->SYNC.PortMachines.Input.isLocalSyncOk)
        && (pPortInputData->RtClass2_TopoState == EDD_SET_REMOTE_PORT_STATE_TOPO_OK))
    {
        NewRtClass2_OutputState = LSA_TRUE;
    }
    else
    {
        NewRtClass2_OutputState = LSA_FALSE;
    }

    if //rt-class2-output-state changed
       (NewRtClass2_OutputState != pRtClass2_Machine->RtClass2_OutputState)
    {
        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrigger, HWPORT:0x%X NewRtClass2_OutputState:0x%X",
                           HwPortIndex, NewRtClass2_OutputState);

        //action on OFF->ON-transition: switch on  rt-class2-TX on this port
        //action on ON->OFF-transition: switch off rt-class2-TX on this port
        // handle iSRT-Bits
        EDDI_SwiPortSetiSRTMode(HwPortIndex,
                                NewRtClass2_OutputState,
                                pDDB);

        EDDI_IrtClass2StmsTrace(pDDB, pRtClass2_Machine->RtClass2_OutputState,
                                NewRtClass2_OutputState);

        //update rt-class2-output-state
        pRtClass2_Machine->RtClass2_OutputState = NewRtClass2_OutputState;

        EDDI_GenSetRTClass2PortStatus(LSA_FALSE,
                                      HwPortIndex,
                                      (LSA_UINT8)((NewRtClass2_OutputState) ? EDD_RTCLASS2_ACTIVE : EDD_RTCLASS2_INACTIVE),
                                      pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         EDDI_IrtClass2StmsTrace()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e t u r n   V a l u e: LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IrtClass2StmsTrace( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_BOOL                 const  OldRtClass2_OutputState,
                                                                LSA_BOOL                 const  NewRtClass2_OutputState )
{
    if (OldRtClass2_OutputState)
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrace, ####  from   ON       ####");
    }
    else
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrace, ####  from   OFF      ####");
    }

    if (NewRtClass2_OutputState)
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrace, ####  to     ON       ####");
    }
    else
    {
        EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_IrtClass2StmsTrace, ####  to     OFF      ####");
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_irt_class2_stm.c                                        */
/*****************************************************************************/
