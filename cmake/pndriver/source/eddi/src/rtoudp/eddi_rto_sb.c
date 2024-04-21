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
/*  F i l e               &F: eddi_rto_sb.c                             :F&  */
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

#define EDDI_MODULE_ID     M_ID_RTO_SB
#define LTRC_ACT_MODUL_ID  127

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_RTO_SB) //satisfy lint!
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_dev.h"
#include "eddi_rto_sb.h"
#include "eddi_crt_ext.h"

static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSACTNOTRCV  ( const EDDI_SB_RTO_EV_TYPE            Event,
                                                                EDDI_CRT_CONSUMER_TYPE      * const  pConsumer ,
                                                                EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB);

static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSACTUK       ( const EDDI_SB_RTO_EV_TYPE            Event,
                                                                 EDDI_CRT_CONSUMER_TYPE      * const  pConsumer,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB);

static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSetEvent  ( EDDI_CRT_CONSUMER_TYPE      * const  pConsumer,
                                                              LSA_UINT8                     const  Event,  
                                                              EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB);

#define UDP_DATA_STATUS_USED_BITS ((LSA_UINT8)(EDD_CSRT_DSTAT_BIT_STATE             | \
                                               EDD_CSRT_DSTAT_BIT_STOP_RUN          | \
                                               EDD_CSRT_DSTAT_BIT_STATION_FAILURE       ))


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
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSetEvent  ( EDDI_CRT_CONSUMER_TYPE   *  const  pConsumer,
                                                              LSA_UINT8                   const  DeltaStatus,  
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDDI_SER_DATA_STATUS_TYPE  DataStatus;
    LSA_UINT16                 CycleCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsSetEvent->");

    DataStatus = pConsumer->Udp.pAPDU->Detail.DataStatus;
    CycleCnt         = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

    if (EDDI_DS_BIT__State_backup0_primary1(DeltaStatus))
    {
        // EDD_CSRT_DSTAT_BIT_STATE changed
        if (EDDI_DS_BIT__State_backup0_primary1(DataStatus))
        {
            pConsumer->PrimaryEventStartTime_ticks = IO_R32(CLK_COUNT_VALUE);   //first record event start time
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_PRIMARY, CycleCnt, 0, pDDB);
        }
        else
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_BACKUP, CycleCnt, 0, pDDB);
        }
    }

    if (EDDI_DS_BIT__Stop0_Run1(DeltaStatus))
    {
        // EDD_CSRT_DSTAT_BIT_STOP_RUN changed
        if (EDDI_DS_BIT__Stop0_Run1(DataStatus))
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_RUN, CycleCnt, 0, pDDB);
        }
        else
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STOP, CycleCnt, 0, pDDB);
        }
    }

    if (EDDI_DS_BIT__StationFailure(DeltaStatus))
    {
        // EDD_CSRT_DSTAT_BIT_STATION_FAILURE   changed
        if (EDDI_DS_BIT__StationFailure(DataStatus))
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STATION_OK, CycleCnt, 0, pDDB);
        }
        else
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_STATION_FAILURE, CycleCnt, 0, pDDB);
        }
    }

    if (EDDI_DS_BIT__Redundancy(DeltaStatus))
    {
        // EDD_CSRT_DSTAT_BIT_STATE changed
        if (EDDI_DS_BIT__Redundancy(DataStatus))
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS, CycleCnt, 0, pDDB);
        }
        else
        {
            EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS, CycleCnt, 0, pDDB);
        }
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSINACTIVE ( const EDDI_SB_RTO_EV_TYPE            Event,
                                                       EDDI_CRT_CONSUMER_TYPE      * const  pConsumer,
                                                       EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB)
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsSINACTIVE->");

    switch (Event)
    {
        case EDDI_SB_RTO_EV_FRAME:
        case EDDI_SB_RTO_EV_TIMEOUT:
        case EDDI_SB_RTO_EV_CTRL_DEACT:
            return;

        case EDDI_SB_RTO_EV_CTRL_ACT:
        {
            pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;
            break;
        }

        case EDDI_SB_RTO_EV_CTRL_ACT_UK:
        {
            pConsumer->Udp.StateFct = EDDI_RtoConsSACTUK;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_RtoConsSINACTIVE", EDDI_FATAL_ERR_EXCP, Event, 0);
            return;
        }
    }

    pConsumer->Udp.FramesMissed  = 0;

    EDDI_CRTConsumerSetPendingEvent(pConsumer, EDDI_CSRT_CONS_EVENT_RESET_EVENT_HANDLING, (LSA_UINT16)0, 0, pDDB);
    pConsumer->Udp.PrevDataStatus = ((pConsumer->Properties & EDD_CSRT_CONS_PROP_SYSRED)?EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP:EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS);
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
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSACTNOTRCV  ( const EDDI_SB_RTO_EV_TYPE            Event,
                                                                EDDI_CRT_CONSUMER_TYPE      * const  pConsumer,  
                                                                EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB)
{
    EDDI_SER_DATA_STATUS_TYPE  DeltaStatus, DataStatus;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsSACTNOTRCV->");
    
    switch (Event)
    {
        case EDDI_SB_RTO_EV_TIMEOUT    :
        case EDDI_SB_RTO_EV_CTRL_ACT   :
            return;

        case EDDI_SB_RTO_EV_CTRL_DEACT :
        {
            pConsumer->Udp.StateFct = EDDI_RtoConsSINACTIVE;
            return;
        }

        case EDDI_SB_RTO_EV_CTRL_ACT_UK:
        {
            pConsumer->Udp.StateFct      = EDDI_RtoConsSACTUK;
            pConsumer->Udp.FramesMissed  = 0;
            return;
        }

        case EDDI_SB_RTO_EV_FRAME:
        {
            DataStatus = pConsumer->Udp.pAPDU->Detail.DataStatus;

            if (EDDI_DS_BIT__DataValid(DataStatus))
            {
                LSA_UINT16  const  CycleCnt = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

                pConsumer->Udp.StateFct = EDDI_RtoConsSACTRCV;

                DeltaStatus =
                    (LSA_UINT8)((pConsumer->Udp.PrevDataStatus ^ DataStatus) & UDP_DATA_STATUS_USED_BITS);

                pConsumer->Udp.FramesMissed = 0;

                pConsumer->Udp.PrevDataStatus = pConsumer->Udp.pAPDU->Detail.DataStatus;
                pConsumer->Udp.pDDB->RTOUDP.PendingEventsCount++;
                EDDI_CRTConsumerSetPendingEvent( pConsumer, EDD_CSRT_CONS_EVENT_AGAIN, CycleCnt, 0, pDDB);

                if (DeltaStatus)
                {
                    EDDI_RtoConsSetEvent(pConsumer, DeltaStatus, pDDB);
                }

                return;
            }

            // no valid frame
            return;
        }

        default                       :
        {
            EDDI_Excp("EDDI_RtoConsSACTNOTRCV", EDDI_FATAL_ERR_EXCP, Event, 0);
            return;
        }
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSACTUK ( const  EDDI_SB_RTO_EV_TYPE            Event,
                                                            EDDI_CRT_CONSUMER_TYPE      *  const  pConsumer, 
                                                            EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB)
{
    EDDI_SER_DATA_STATUS_TYPE  DeltaStatus, DataStatus;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsSACTUK->");

    switch (Event)
    {
        case EDDI_SB_RTO_EV_CTRL_DEACT :
        {
            pConsumer->Udp.StateFct = EDDI_RtoConsSINACTIVE;
            return;
        }

        case EDDI_SB_RTO_EV_CTRL_ACT   :
        {
            pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;
            return;
        }

        case EDDI_SB_RTO_EV_CTRL_ACT_UK:
            break;

        case EDDI_SB_RTO_EV_FRAME      :
        {
            LSA_UINT16  const  CycleCnt = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

            DataStatus = pConsumer->Udp.pAPDU->Detail.DataStatus;

            // Get all changed Bits
            DeltaStatus  =
                (LSA_UINT8)((pConsumer->Udp.PrevDataStatus ^ DataStatus) & UDP_DATA_STATUS_USED_BITS);

            pConsumer->Udp.pDDB->RTOUDP.PendingEventsCount++;

            if (DeltaStatus)
            {
                EDDI_RtoConsSetEvent(pConsumer, DeltaStatus, pDDB);
            }

            if (EDDI_DS_BIT__DataValid(DataStatus))
            {
                pConsumer->Udp.StateFct = EDDI_RtoConsSACTRCV;

                EDDI_CRTConsumerSetPendingEvent( pConsumer, EDD_CSRT_CONS_EVENT_AGAIN, CycleCnt, 0, pDDB);

                pConsumer->Udp.PrevDataStatus = DataStatus;

                return;
            }

            EDDI_CRTConsumerSetPendingEvent( pConsumer, EDD_CSRT_CONS_EVENT_MISS, CycleCnt, 0, pDDB);
            pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;
            return;

        }

        case EDDI_SB_RTO_EV_TIMEOUT:
        {
            pConsumer->Udp.FramesMissed++;
            if (pConsumer->Udp.FramesMissed > pConsumer->LowerParams.DataHoldFactor)
            {
                pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;

                // Indication
                pConsumer->Udp.pDDB->RTOUDP.PendingEventsCount++;

                if (pConsumer->Udp.pAPDU)
                {
                    LSA_UINT16  const  CycleCnt = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

                    EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS, CycleCnt, 0, pDDB);
                }
                else
                {
                    EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS, 0, 0, pDDB);
                }

                return;
            }

            return;
        }

        default:
        {
            EDDI_Excp("EDDI_RtoConsSACTNOTRCV", EDDI_FATAL_ERR_EXCP, Event, 0);
            return;
        }
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsSACTRCV( const  EDDI_SB_RTO_EV_TYPE            Event,
                                                    EDDI_CRT_CONSUMER_TYPE      *  const  pConsumer, 
                                                    EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB)
{
    EDDI_SER_DATA_STATUS_TYPE  DeltaStatus, DataStatus;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoConsSACTRCV->");

    switch (Event)
    {
        case EDDI_SB_RTO_EV_CTRL_DEACT:
        {
            pConsumer->Udp.StateFct = EDDI_RtoConsSINACTIVE;
            return;
        }

        case EDDI_SB_RTO_EV_CTRL_ACT:
            return;

        case EDDI_SB_RTO_EV_CTRL_ACT_UK:
        {
            pConsumer->Udp.StateFct      = EDDI_RtoConsSACTUK;
            pConsumer->Udp.FramesMissed  = 0;
            return;
        }

        case EDDI_SB_RTO_EV_FRAME:
        {
            LSA_UINT16  const  CycleCnt = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

            DataStatus = pConsumer->Udp.pAPDU->Detail.DataStatus;

            // Get all changed Bits execpt DataValid
            DeltaStatus  =
                (LSA_UINT8)((pConsumer->Udp.PrevDataStatus ^ DataStatus) & UDP_DATA_STATUS_USED_BITS);

            pConsumer->Udp.FramesMissed         = 0;
            pConsumer->Udp.PrevDataStatus = DataStatus;

            if (DeltaStatus)
            {
                pConsumer->Udp.pDDB->RTOUDP.PendingEventsCount++;
                EDDI_RtoConsSetEvent(pConsumer, DeltaStatus, pDDB);

                if (!EDDI_DS_BIT__DataValid(DataStatus))
                {
                    pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;
                    EDDI_CRTConsumerSetPendingEvent( pConsumer, EDD_CSRT_CONS_EVENT_MISS, CycleCnt, 0, pDDB);
                }
                return;
            }

            if (!EDDI_DS_BIT__DataValid(DataStatus))
            {
                pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;
                pConsumer->Udp.pDDB->RTOUDP.PendingEventsCount++;
                EDDI_CRTConsumerSetPendingEvent(pConsumer, EDD_CSRT_CONS_EVENT_MISS, CycleCnt, 0, pDDB);
            }

            return;
        }

        case EDDI_SB_RTO_EV_TIMEOUT:
        {
            pConsumer->Udp.FramesMissed++;

            if (pConsumer->Udp.FramesMissed > pConsumer->LowerParams.DataHoldFactor)
            {
                LSA_UINT16  const  CycleCnt = EDDI_NTOHS(pConsumer->Udp.pAPDU->Detail.CycleCnt);

                pConsumer->Udp.StateFct = EDDI_RtoConsSACTNOTRCV;
                pConsumer->Udp.pDDB->RTOUDP.PendingEventsCount++;
                EDDI_CRTConsumerSetPendingEvent( pConsumer, EDD_CSRT_CONS_EVENT_MISS, CycleCnt, 0, pDDB);
                return;
            }

            return;
        }

        default:
        {
            EDDI_Excp("EDDI_RtoConsSACTRCV", EDDI_FATAL_ERR_EXCP, Event, 0);
            return;
        }
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoSBInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_DDB_COMP_RTOUDP_TYPE  *  pRTOUDP;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoSBInit->");

    pRTOUDP = &pDDB->RTOUDP;

    EDDI_TreeIni(EDDI_UDP_MAX_BINARY_REDUCTION, &pRTOUDP->pTreeSB);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RtoSBCheck( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                     Status;
    LSA_UINT32                     i, k, CntEntries, LastIndex;
    EDDI_CRT_CONSUMER_PTR_TYPE     pConsumer;
    E_TREE                      *  pSBTree;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RtoSBCheck->");

    pSBTree = pDDB->RTOUDP.pTreeSB;
    EDDI_TreeGetPathList(pSBTree);

    LastIndex = pSBTree->CntActPath;

    for (i = 0; i < LastIndex; i++)
    {
        pConsumer = (EDDI_CRT_CONSUMER_PTR_TYPE )pSBTree->ActPath[i].pList;

        CntEntries = pSBTree->ActPath[i].CntEntries;

        for (k = 0; k < CntEntries; k++)
        {
            if (!pConsumer)
            {
                EDDI_Excp("EDDI_RtoSBCheck", EDDI_FATAL_ERR_EXCP, i, k);
                return;
            }

            pConsumer->Udp.StateFct(EDDI_SB_RTO_EV_TIMEOUT, pConsumer, pDDB);

            pConsumer = pConsumer->Link.pNext;

        } // end for
    }  // end LastIndex

    if (pDDB->RTOUDP.PendingEventsCount)
    {
        pDDB->RTOUDP.PendingEventsCount = 0;

        Status = EDDI_CRTRpsFillAndSendIndication(pDDB, pDDB->CRT.ConsumerList.LastIndexRTC123, 0xFFFFFFFFUL /*DFP cons are not needed here*/, LSA_TRUE /*bDirectIndication*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRTRpsInformStateMachine(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_XRT_OVER_UDP_SOFTWARE


/*****************************************************************************/
/*  end of file eddi_rto_sb.c                                                */
/*****************************************************************************/
