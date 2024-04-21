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
/*  F i l e               &F: eddi_rto_check.c                          :F&  */
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
/*                                                            H i s t o r y :*/
/*   ________________________________________________________________________*/
/*                                                                           */
/*                                                     Date        Who   What*/
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_RTO_CHECK
#define LTRC_ACT_MODUL_ID  129

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_RTO_CHECK) //satisfy lint!
#endif

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_dev.h"

#include "eddi_crt_check.h"
#include "eddi_rto_check.h"


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoConsumerAddCheckRQB()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoConsumerAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT8                const  ListType )
{
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;

    //check CycleReductionRatio
    if (pConsParam->CycleReductionRatio < EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoConsumerAddCheckRQB, CycleReductionRatio < EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN");

        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
        return EDD_STS_ERR_PARAM;
    }

    if (pConsParam->CycleReductionRatio > EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoConsumerAddCheckRQB, CycleReductionRatio > EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX");

        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
        return EDD_STS_ERR_PARAM;
    }

    #if !defined(EDDI_CFG_REV7)
    if (pConsParam->Partial_DataLen)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoConsumerAddCheckRQB, pConsParam->Partial_DataLen not supported. :0x%X :0x%X",
                              pConsParam->Partial_DataLen, EDD_CSRT_DATALEN_MIN);

        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    if (pConsParam->DataLen < EDD_CSRT_UDP_DATALEN_MIN)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoConsumerAddCheckRQB, Parameter pConsParam->DataLen too small. :0x%X :0x%X",
                              pConsParam->DataLen, EDD_CSRT_UDP_DATALEN_MIN);

        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
        return EDD_STS_ERR_PARAM;
    }

    if (pConsParam->DataLen > EDD_CSRT_UDP_DATALEN_MAX)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoConsumerAddCheckRQB, Parameter pConsParam->DataLen too big. :0x%X :0x%X",
                              pConsParam->DataLen, EDD_CSRT_UDP_DATALEN_MAX);

        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
        return EDD_STS_ERR_PARAM;
    }

    if (pDDB->Glob.LocalIP.dw == 0)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoConsumerAddCheckRQB, No IP Address set.");

        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NO_LOCAL_IP_SET);
        return EDD_STS_ERR_NO_LOCAL_IP;
    }

    switch (ListType)
    {
        case EDDI_LIST_TYPE_ACW:
            break;

        default:
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_RtoConsumerAddCheckRQB, invalid ListType. :0x%X ListType:0x%X",
                                  pConsParam->FrameID, ListType);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
            return EDD_STS_ERR_PARAM;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RtoProviderAddCheckRQB()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RtoProviderAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT8                const  ListType )
{
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;

    //check CycleReductionRatio
    if (pProvParam->CycleReductionRatio < EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoProviderAddCheckRQB, CycleReductionRatio < EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
        return EDD_STS_ERR_PARAM;
    }

    if (pProvParam->CycleReductionRatio > EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoProviderAddCheckRQB, CycleReductionRatio > EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
        return EDD_STS_ERR_PARAM;
    }

    if (!pDDB->NRT.PaddingEnabled)
    {
        if (pProvParam->DataLen < EDD_CSRT_UDP_DATALEN_MIN)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_RtoProviderAddCheckRQB, Parameter pProvParam->DataLen too small. DataLen:0x%X EDD_CSRT_UDP_DATALEN_MIN:0x%X",
                                  pProvParam->DataLen, EDD_CSRT_UDP_DATALEN_MIN);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
            return EDD_STS_ERR_PARAM;
        }
    }

    if (pProvParam->DataLen > EDD_CSRT_UDP_DATALEN_MAX)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_RtoProviderAddCheckRQB, Parameter pProvParam->DataLen too big. DataLen:0x%X EDD_CSRT_UDP_DATALEN_MAX:0x%X",
                              pProvParam->DataLen, EDD_CSRT_UDP_DATALEN_MAX);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
        return EDD_STS_ERR_PARAM;
    }

    if (pDDB->Glob.LocalIP.dw == 0)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RtoProviderAddCheckRQB, No IP Address set.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_NO_LOCAL_IP_SET);
        return EDD_STS_ERR_NO_LOCAL_IP;
    }

    switch (ListType)
    {
        case EDDI_LIST_TYPE_ACW:
            break;

        default:
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_RtoProviderAddCheckRQB, invalid ListType. FrameID:0x%X ListType:0x%X",
                                  pProvParam->FrameID, ListType);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
            return EDD_STS_ERR_PARAM;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*****************************************************************************/
/*  end of file eddi_rto_check.c                                             */
/*****************************************************************************/
