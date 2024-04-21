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
/*  F i l e               &F: eddi_crt_dfpcons.c                        :F&  */
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

#include "eddi_int.h"

#include "eddi_dev.h"

//#include "eddi_csrt_ext.h"
//#include "eddi_irt_ext.h"
#include "eddi_crt_ext.h"
//#include "eddi_ser_ext.h"

#include "eddi_crt_check.h"
//#include "eddi_ser_cmd.h"
//#include "eddi_crt_sb_sw_tree.h"
#include "eddi_crt_com.h"

//#include "eddi_prm_record_pdir_data.h"
//#include "eddi_prm_record_common.h"
//#include "eddi_prm_record_pdir_subframe_data.h"

#include "eddi_time.h"

#include "eddi_crt_dfp.h"

#define EDDI_MODULE_ID     M_ID_CRT_DFPCONS
#define LTRC_ACT_MODUL_ID  114

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_DFP_ON)
/*--------------------------------------------------------------------------*
 * Local protos                                                             *
 *--------------------------------------------------------------------------*/
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsGetPF(EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                        EDDI_CRT_CONSUMER_PTR_TYPE      * const ppPFConsumer,
                                                        EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const pConsumer );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPRemoveFromSBList(EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                                EDDI_CONST_CRT_CONSUMER_PTR_TYPE    const  pConsumer);

static LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPAddToSBList(EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer);

static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPSWSBHandler( LSA_VOID  *  const  context );

static LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerCheckDGData(EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                                     EDDI_CRT_CONSUMER_PTR_TYPE       const pConsumer,
                                                                     EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE const pSubFrmHandler);

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsActPFFrame(EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const pConsumer,
                                                             EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE   const pSubFrmHandler);

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsAddPFHandler(EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE            const pConsumer,
                                                               EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const pConsParam );


/*--------------------------------------------------------------------------*
 * Consumerhandling                                                         *
 *--------------------------------------------------------------------------*/
/*****************************************************************************
 *  Function:    EDDI_DFPConsAddCheckRQB()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
******************************************************************************/
//Checks if all conditions for a DFP frame are met
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT8                const  ListType,
                                                          LSA_UINT8                const  ConsumerType )
{
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;
    
    LSA_UNUSED_ARG(pDDB);

    if (   (EDDI_LIST_TYPE_FCW != ListType)
        || (EDDI_RTC3_CONSUMER != ConsumerType)
        || (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY) )
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPConsAddCheckRQB, No DFP Consumer: FrameID:0x%X, ListType:%d, ConsumerType:%d, Properties:0x%X",
                          pConsParam->FrameID, ListType, ConsumerType, pConsParam->Properties);
        return (EDD_STS_ERR_PARAM);
    }
    
    if (   (pConsParam->DataLen < EDD_CSRT_DFP_DATALEN_MIN)
        || (pConsParam->DataLen > EDD_CSRT_DFP_DATALEN_MAX) )
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPConsAddCheckRQB, invalid DataLen: FrameID:0x%X, DataLen:0x%X",
                          pConsParam->FrameID, pConsParam->DataLen);
        return (EDD_STS_ERR_PARAM);
    }

    /* check DataHoldFactor */
    if (   (pConsParam->DataHoldFactor < EDD_SRT_DFP_CONSUMER_DATAHOLD_FACTOR_MIN)
        || (pConsParam->DataHoldFactor > EDD_SRT_DFP_CONSUMER_DATAHOLD_FACTOR_MAX) )
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPConsAddCheckRQB, invalid DataHoldFactor: FrameID:0x%X, DataHoldFactor:0x%X",
                          pConsParam->FrameID, pConsParam->DataHoldFactor);
        return (EDD_STS_ERR_PARAM);
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsAddPFHandler()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
******************************************************************************/
//searches if a pConsumer for a packframe with this FrameID has been set yet, if not, it allocates it
static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsAddPFHandler(EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE            const pConsumer,
                                                               EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const pConsParam )
{
    EDDI_CRT_CONSUMER_PTR_TYPE      pPFConsumer;
    LSA_RESULT                      Result;

    //check if a PF has to be added
    //1. find PF by FrameID
    EDDI_CRTConsumerListFindByFrameId(pDDB, &pDDB->CRT.ConsumerList, &pDDB->CRT, pConsParam, EDDI_LIST_TYPE_FCW, &pPFConsumer, LSA_FALSE /*bIsDFP*/);

    //2. Add PF if needed
    if (pPFConsumer == EDDI_NULL_PTR)
    {
        EDD_RQB_TYPE    RQBLocal;
        EDD_RQB_CSRT_CONSUMER_ADD_TYPE  ConsParamLocal;

        //first DG consumer ==> add PF
        ConsParamLocal.FrameID              = pConsParam->FrameID;
        ConsParamLocal.Properties           = EDD_CSRT_CONS_PROP_RTCLASS_3;
        ConsParamLocal.DataLen              = EDD_CSRT_DATALEN_MIN;   //will be changed before activate!
        ConsParamLocal.CycleReductionRatio  = pConsParam->CycleReductionRatio;
        ConsParamLocal.CyclePhase           = EDD_CYCLE_PHASE_UNDEFINED;
        ConsParamLocal.DataHoldFactor       = pConsParam->DataHoldFactor;
        ConsParamLocal.UserID               = 0xFFFFFFFF;
        ConsParamLocal.ConsumerID           = 0xCCCC;
        ConsParamLocal.IOParams.BufferProperties   = pConsParam->IOParams.BufferProperties;        
        ConsParamLocal.IOParams.ClearOnMISS        = pConsParam->IOParams.ClearOnMISS;        
        ConsParamLocal.IOParams.ForwardMode        = EDD_FORWARDMODE_SF;             
        ConsParamLocal.IOParams.DataOffset         = EDD_DATAOFFSET_UNDEFINED;
        ConsParamLocal.Partial_DataLen      = 0;         
        ConsParamLocal.Partial_DataOffset   = 0;      
        RQBLocal.pParam = (EDD_UPPER_MEM_PTR_TYPE)&ConsParamLocal;
        RQBLocal.Service = EDD_SRV_SRT_CONSUMER_ADD;
        EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);
        
        Result = EDDI_CRTConsumerListAddEntry(&RQBLocal, pDDB);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPConsAddPFHandler, ERROR: Adding PF failed. FrameID:0x%X, ConsumerID:0x%X",
                              pConsumer->LowerParams.FrameId, pConsumer->ConsumerId);
            return EDD_STS_ERR_PARAM;
        }
        else
        {
            EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_DFPConsAddPFHandler, PF Added. SFPos:%d, FrameID:0x%X, ConsumerID:0x%X, PFConsumerId:0x%X",
                              pConsumer->LowerParams.SFPosition, pConsumer->LowerParams.FrameId, pConsumer->ConsumerId, ConsParamLocal.ConsumerID);
            
            
            //Get PFConsumer by ConsumerID
            if (EDD_STS_OK != EDDI_CRTConsumerListGetEntry(pDDB, &pPFConsumer, &pDDB->pLocal_CRT->ConsumerList, ConsParamLocal.ConsumerID))
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                                  "EDDI_DFPConsAddPFHandler, ERROR: Could not get pPFConsumer, ConsumerID:0x%X pFConsumerID:0x%X",
                                  pConsumer->ConsumerId, ConsParamLocal.ConsumerID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
                EDDI_Excp("EDDI_DFPConsAddPFHandler, ERROR: Could not get pPFConsumer", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, ConsParamLocal.ConsumerID);
                return EDD_STS_ERR_PARAM;
            }

            //Init PFConsumer
            pPFConsumer->bIsPFConsumer = LSA_TRUE;  
            pPFConsumer->DGActiveCtr  = 0;                            
            pPFConsumer->DGPassiveCtr = 0;                            
        }
    }
    else
    {
        //PF exists, set links
        if (!pPFConsumer->bIsPFConsumer)
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                              "EDDI_DFPConsAddPFHandler, ERROR: Cons is no PF. FrameID:0x%X, ConsumerID:0x%X pFConsumerID:0x%X",
                              pConsumer->LowerParams.FrameId, pConsumer->ConsumerId, pConsumer->PFConsumerId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
            EDDI_Excp("EDDI_DFPConsAddPFHandler, ERROR: Cons is no PF", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->PFConsumerId);
            return EDD_STS_ERR_PARAM;
        }

        EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"EDDI_DFPConsAddPFHandler, Adding DG, PF exists. SFPos:%d, FrameID:0x%X, ConsumerID:0x%X, PFConsumerId:0x%X, PassDGs:%d, ActDGs:%d",
                          pConsumer->LowerParams.SFPosition, pConsumer->LowerParams.FrameId, pConsumer->ConsumerId, pPFConsumer->ConsumerId, pPFConsumer->DGPassiveCtr, pPFConsumer->DGActiveCtr);

    }

    //check for underrun/overrun
    if (0xFF == pPFConsumer->DGPassiveCtr)
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPConsPassPFFrame, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFConsumerID:0x%X, FrameID:0x%X", 
                          pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr, pPFConsumer->ConsumerId, pPFConsumer->LowerParams.FrameId);
        EDDI_Excp("EDDI_DFPConsPassPFFrame, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr);
    }
    pPFConsumer->DGPassiveCtr++;

    pConsumer->PFConsumerId = pPFConsumer->ConsumerId;
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
                                                      
/******************************************************************************
 *  Function:    EDDI_DFPConsActPFFrame()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks if the conditions for activating a packframe are met
static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsActPFFrame(EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                                             EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const pConsumer,
                                                             EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE   const pSubFrmHandler)
{
    EDDI_CRT_CONSUMER_PTR_TYPE  pPFConsumer;
    LSA_RESULT                  Result;          

    EDDI_DFPConsGetPF(pDDB, &pPFConsumer, pConsumer);
    
    //if the PF has active DGs, then it had been activated before 
    if (!pPFConsumer->DGActiveCtr)
    {
        EDD_RQB_TYPE                        RQBLocal;
        EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE  ParamLocal;
        LSA_UINT8                           Ctr;

        //get PF-handler via SubframeHandler
        EDDI_PRM_PF_DSCR_TYPE * const pPFHandler = pSubFrmHandler->pPF;
        
        //complete PF handler
        pPFHandler->ConsProvID = pPFConsumer->ConsumerId;
        
        //activate PF
        ParamLocal.ConsumerID           = pPFConsumer->ConsumerId;
        ParamLocal.Mode                 = EDD_CONS_CONTROL_MODE_ACTIVATE;
               
        //change MAC-Adress from PF-Consumer to Broadcast
        for (Ctr = 0; Ctr < EDD_MAC_ADDR_SIZE; Ctr++)
        {
            ParamLocal.SrcMAC.MacAdr[Ctr] = 0;
        }     
           
        ParamLocal.CycleReductionRatio  = pConsumer->LowerParams.CycleReductionRatio;
        ParamLocal.CyclePhase           = pConsumer->LowerParams.CyclePhase;
        ParamLocal.IOParams.BufferProperties    = pConsumer->LowerParams.BufferProperties;
        ParamLocal.IOParams.ForwardMode         = EDD_FORWARDMODE_SF;
        ParamLocal.IOParams.DataOffset          = pConsumer->LowerParams.DataOffset - pConsumer->LowerParams.SFOffset;
        pPFConsumer->LowerParams.ImageMode      = pConsumer->LowerParams.ImageMode;

        #if defined (EDDI_CFG_REV7)
        pPFConsumer->LowerParams.Partial_DataOffset = 0;
        if (pPFHandler->FrameLen < EDDI_IRT_DATALEN_MIN)
        {
            //consumer gets min IRT datalen
            pPFConsumer->LowerParams.DataLen = EDDI_IRT_DATALEN_MIN;
            pPFConsumer->LowerParams.Partial_DataLen = pPFHandler->FrameLen;
        }
        else
        {
            //consumer gets real IRT datalen
            pPFConsumer->LowerParams.DataLen = pPFHandler->FrameLen;
        }
        #else
        #error "No IRT partialdata available!"
        #endif

        RQBLocal.pParam = (EDD_UPPER_MEM_PTR_TYPE)&ParamLocal;
        RQBLocal.Service = EDD_SRV_SRT_CONSUMER_CONTROL;
        EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);
        
        Result = EDDI_CRTConsumerControlEvent(pPFConsumer, &RQBLocal, pDDB, pPFConsumer->LowerParams.ConsumerType);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPConsActPFFrame, ERROR: EDDI_CRTConsumerControlEvent failed. PFConsumerId:0x%X Status:0x%X",
                              pPFConsumer->ConsumerId, Result);
            //Details have already been set
            return Result;
        }
        else
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_DFPConsActPFFrame, PF activated. PFConsumerId:0x%X Status:0x%X",
                              pPFConsumer->ConsumerId, Result);
        }
        
        pPFConsumer->pSubFrmHandler = (EDDI_PRM_SF_DSCR_TYPE *)(void *)pPFHandler;
    }
    else
    {
        //check the DG-dataoffset against the dataoffset of the PF
        if (pPFConsumer->LowerParams.DataOffset != (pConsumer->LowerParams.DataOffset - pConsumer->LowerParams.SFOffset))
        {
            EDDI_CRT_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPConsActPFFrame, ERROR: DataOffset does not match. ConsumerId:0x%X, FrameID:0x%X, PFDataOffset:0x%X, DGDataOffset:0x%X, DGSFOffset:0x%X, DGSFPosition:0x%X",
                              pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, pPFConsumer->LowerParams.DataOffset, pConsumer->LowerParams.DataOffset, pConsumer->LowerParams.SFOffset, pConsumer->LowerParams.SFPosition);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
            return EDD_STS_ERR_PARAM;
        }
    }

    //check for underrun/overrun
    if (   (0xFF == pPFConsumer->DGActiveCtr)
        || (0 == pPFConsumer->DGPassiveCtr))
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
            "EDDI_DFPConsPassPFFrame, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFConsumerID:0x%X, FrameID:0x%X", 
                          pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr, pPFConsumer->ConsumerId, pPFConsumer->LowerParams.FrameId);
        EDDI_Excp("EDDI_DFPConsPassPFFrame, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr);
    }
    pPFConsumer->DGActiveCtr++;
    pPFConsumer->DGPassiveCtr--;
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsPassPFFrame()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks if the conditions for passivating a packframe are met, and passivate it
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsPassPFFrame(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                       EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer)
{
    EDDI_CRT_CONSUMER_PTR_TYPE  pPFConsumer;
    LSA_RESULT                  Result;          

    EDDI_DFPConsGetPF(pDDB, &pPFConsumer, pConsumer);

    //check for underrun/overrun
    if (   (0 == pPFConsumer->DGActiveCtr)
        || (0xFF == pPFConsumer->DGPassiveCtr))
    {
        EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPConsPassPFFrame, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFConsumerID:0x%X, FrameID:0x%X", 
                          pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr, pPFConsumer->ConsumerId, pPFConsumer->LowerParams.FrameId);
        EDDI_Excp("EDDI_DFPConsPassPFFrame, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr);
    }
    pPFConsumer->DGActiveCtr--;
    pPFConsumer->DGPassiveCtr++;

    //if the PF now has passive DGs only , then passivate it 
    if (!pPFConsumer->DGActiveCtr)
    {
        EDD_RQB_TYPE                        RQBLocal;
        EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE  ParamLocal;
    
        //Passivate PF
        ParamLocal.ConsumerID           = pPFConsumer->ConsumerId;
        ParamLocal.Mode                 = EDD_CONS_CONTROL_MODE_PASSIVATE;

        RQBLocal.pParam = (EDD_UPPER_MEM_PTR_TYPE)&ParamLocal;
        RQBLocal.Service = EDD_SRV_SRT_CONSUMER_CONTROL;
        EDD_RQB_SET_OPCODE(&RQBLocal, EDD_OPC_REQUEST);
        
        Result = EDDI_CRTConsumerControlEvent(pPFConsumer, &RQBLocal, pDDB, pPFConsumer->LowerParams.ConsumerType);
        if (EDD_STS_OK != Result)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPConsPassPFFrame, ERROR: EDDI_CRTConsumerControlEvent failed. PFConsumerId:0x%X Status:0x%X",
                              pPFConsumer->ConsumerId, Result);
            return Result;
        }
        else
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDI_DFPConsPassPFFrame, PF passivated. PFConsumerId:0x%X Status:0x%X",
                              pPFConsumer->ConsumerId, Result);
        }
    
        //Reset all parameters, they get inherited from 1st DG being activated
        pPFConsumer->pSubFrmHandler                   = EDDI_NULL_PTR;
        pPFConsumer->LowerParams.BufferProperties     = EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED;
        pPFConsumer->LowerParams.CyclePhase           = EDD_CYCLE_PHASE_UNDEFINED;
        pPFConsumer->LowerParams.CycleReductionRatio  = EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
        pPFConsumer->LowerParams.DataOffset           = EDD_DATAOFFSET_UNDEFINED;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsRemovePFHandler()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks if the conditions for removing a packframe are met
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsRemovePFHandler( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                              EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        EDDI_CRT_CONSUMER_PTR_TYPE  pPFConsumer;
        LSA_RESULT                  Status;          
    
        //Get PF Consumer
        EDDI_DFPConsGetPF(pDDB, &pPFConsumer, pConsumer);

        //check for underrun/overrun
        if (   (0 == pPFConsumer->DGPassiveCtr)
            || (pConsumer->pSB->Status != EDDI_CRT_CONS_STS_INACTIVE) )
        {
            EDDI_CRT_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                "EDDI_DFPConsPassPFFrame, illegal CtrValues: DGActiveCtr:0x%X, DGPassiveCtr:0x%X, PFConsumerID:0x%X, FrameID:0x%X, SBStatus:%d", 
                              pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr, pPFConsumer->ConsumerId, pPFConsumer->LowerParams.FrameId, pConsumer->pSB->Status);
            EDDI_Excp("EDDI_DFPConsPassPFFrame, illegal CtrValues", EDDI_FATAL_ERR_EXCP, pPFConsumer->DGActiveCtr, pPFConsumer->DGPassiveCtr);
        }
        pPFConsumer->DGPassiveCtr--;   //DG gets removed from PF

        if (    (0 == pPFConsumer->DGActiveCtr)
            &&  (0 == pPFConsumer->DGPassiveCtr) )
        {
            //remove PF
            Status = EDDI_CRTConsumerRemoveEvent(pPFConsumer, pDDB);
            if (Status != EDD_STS_OK)
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_DFPConsRemovePFHandler, ERROR removing PF Consumer, Status:0x%X, ConsumerID:0x%X pFConsumerID:0x%X", 
                                  Status, pConsumer->ConsumerId, pConsumer->PFConsumerId);
                return Status;
            }

            EDDI_CRTConsumerListUnReserveEntry(pPFConsumer, pDDB->pLocal_CRT, LSA_FALSE /*bIsDFP*/);
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerInit()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//initializes a DFP-consumer
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerInit( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                       EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                       EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam )
{                                                               
    LSA_RESULT  Result;
    
    pConsumer->pSubFrmHandler                    = EDDI_NULL_PTR;  
    pConsumer->LowerParams.SFPosition            = pConsParam->IOParams.SFPosition;
    pConsumer->LowerParams.SFOffset              = pConsParam->IOParams.SFOffset;
    pConsumer->DGActiveCtr                       = EDDI_DFP_INVALID_DG_CTR;
    pConsumer->DGPassiveCtr                      = EDDI_DFP_INVALID_DG_CTR;
    pConsumer->PFConsumerId                      = EDD_CONS_ID_INVALID;
    pConsumer->bIsPFConsumer                     = LSA_FALSE;
    
    pConsumer->LowerParams.DataOffset         = pConsParam->IOParams.DataOffset;
    pConsumer->LowerParams.Partial_DataLen    = pConsParam->Partial_DataLen;
    pConsumer->LowerParams.Partial_DataOffset = pConsParam->Partial_DataOffset;

    if (   (0 != pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
        && (EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC != (pConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) ) )
    {
        pConsumer->usingPAEA_Ram = LSA_TRUE;
    }

    //check if a PF has to be added, or just enter the PF-ConsID
    Result = EDDI_DFPConsAddPFHandler(pDDB, pConsumer, pConsParam);
    if (EDD_STS_OK != Result)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPConsumerInit, EDDI_DFPConsAddPFHandler failed. ConsID:0x%X, FrameID:0x%X",
                          pConsumer->ConsumerId,pConsParam->FrameID);
        return Result;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerCheckDGData()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//checks a subframe against the PDIRSubframeData prior to activation
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerCheckDGData(EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                                                     EDDI_CRT_CONSUMER_PTR_TYPE        const pConsumer,
                                                                     EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  const pSubFrmHandler)
{
    EDDI_PRM_PF_DSCR_TYPE * const pPFHandler = pSubFrmHandler->pPF;
    LSA_UINT32                    iDataElem;
    LSA_UINT32                    iDataElemCount = 0;
    
    if (pPFHandler->bIsProv)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPConsumerCheckDGData, no consumer. ConsumerID:0x%X, FrameID:0x%X",
                          pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_HandlerType);                           
        return EDD_STS_ERR_PARAM;
    }

    if (pSubFrmHandler->UsedByUpper)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPConsumerCheckDGData, LSA_FALSE |= UsedByUpper. ConsumerID:0x%X, FrameID:0x%X",
                          pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_UsedByUpper);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!(EDDI_NULL_PTR == pConsumer->pSubFrmHandler))
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPConsumerCheckDGData, no EDDI_NULL_PTR, ConsumerID:0x%X, FrameID:0x%X, pSubFrmHandler 0x%X", 
                          pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, pConsumer->pSubFrmHandler);
        EDDI_Excp("EDDI_CRTConsumerCheckData, pConsumer->pFrmHandler", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->LowerParams.FrameId);
        return EDD_STS_ERR_EXCP;
    }

    if (pSubFrmHandler->Len != pConsumer->LowerParams.DataLen)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPConsumerCheckDGData, DataLength -> pConsumer->LowerParams.DataLen:0x%X pFrmHandler->pIrFrameDataElem[iElem]->DataLength:0x%X",
                          pConsumer->LowerParams.DataLen, pSubFrmHandler->Len);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameDataLen);
        return EDD_STS_ERR_PARAM;
    }

    for (iDataElem = 0; iDataElem < pDDB->PM.PortMap.PortCnt; iDataElem++)
    {        
        if (EDDI_NULL_PTR == pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem])
        {
            //do nothing
            continue;                         
        }
        
        if (pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->FrameID == pConsumer->LowerParams.FrameId)
        {
            if (pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->ReductionRatio != pConsumer->LowerParams.CycleReductionRatio)
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_DFPConsumerCheckDGData, ReductionRatio -> pConsumer->LowerParams.CycleReductionRatio:0x%X pFrmHandler->pIrFrameDataElem[0x%X]->ReductionRatio:0x%X",
                                  pConsumer->LowerParams.CycleReductionRatio, iDataElem, pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->ReductionRatio);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ReductionRatio);
                return EDD_STS_ERR_PARAM;
            }

            //non-legacy consumers take their phase directly from the PDIRData
            pConsumer->LowerParams.CyclePhase = pPFHandler->pFrameHandler->pIrFrameDataElem[iDataElem]->Phase;

            if (pPFHandler->FrameID != pConsumer->LowerParams.FrameId)
            {
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_DFPConsumerCheckDGData, FrameId -> pConsumer->LowerParams.FrameId:0x%X pFrmHandler->pIrFrameDataElem[0x%X]->FrameID:0x%X",
                                  pConsumer->LowerParams.FrameId, iDataElem, pPFHandler->FrameID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataElem_FrameID);
                return EDD_STS_ERR_PARAM;
            }
            
            iDataElemCount++;
        }  
    }
    
    if (iDataElemCount == 0)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_DFPConsumerCheckDGData, No pFrmHandler->pIrFrameDataElem with same FrameID was found, pConsumer->LowerParams.FrameId:0x%X pFrmHandler->pIrFrameDataElem[iElem]->FrameID:0x%X",
                          pConsumer->LowerParams.FrameId, pPFHandler->FrameID);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataElem_FrameID);
        return EDD_STS_ERR_PARAM;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerPassivate()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//Passivate a DG
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerPassivate(EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                            LSA_BOOL                    const  bClearEvents)
{
    if (bClearEvents)
    {
        //clear all pending events
        EDDI_CRTConsumerSetPendingEvent(pConsumer, 0xFFFFFFFFUL, (LSA_UINT16)0, 0, pDDB);
    }

    //deactivate Consumer
    pConsumer->pSB->Status = EDDI_CRT_CONS_STS_INACTIVE;
    pConsumer->pSB->OldDataStatus = EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;
    
    //trigger Scoreboard-SM
    if (EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE)) {} //Lint
    
    //disable WDT
    pConsumer->pSB->UkStatus    = EDDI_CRT_CONS_UK_OFF;
    if (pConsumer->pSB->SBEntryDFP.bInWDTList)
    {
        EDDI_DFPRemoveFromSBList(pDDB, pConsumer);
    }

    EDDI_CRT_CONSTRACE_TRACE(pConsumer, (LSA_UINT32)(*(pConsumer->LowerParams.pKRAMDataBuffer + (EDDI_SFAPDU_OFFSET_SFCycleCtr * (pConsumer->usingPAEA_Ram ? 4 : 1)))), 0, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_DEACT);

    //Deactivate associated providers ("Provider AutoStop")
    if (    (pConsumer->bEnableAutoStop)
        && (
                ((pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_RTC3_MAIN)    == EDDI_CONS_ACTIVE_RTC3_MAIN)
             || ((pConsumer->IRTtopCtrl.ConsActiveMask & EDDI_CONS_ACTIVE_CLASS12_MAIN) == EDDI_CONS_ACTIVE_CLASS12_MAIN)
           )
       ) //STATE: ACTIVE
    {
        LSA_RESULT const Status = EDDI_CrtASCtrlAllProviderList(pDDB, pConsumer, LSA_FALSE /*RemoveASCoupling*/, LSA_TRUE /*DeactivateProvider*/ );
        if (Status != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_CrtASCtrlAllProviderList, Error", EDDI_FATAL_ERR_EXCP, Status, pConsumer);
        }
    }
    
    //remove bindings to subframehandler
    if (!pConsumer->pSubFrmHandler)
    {
        //UsedByUpper does not have to be checked (is always paired with pFrmHandler)
        //no framehandler present.
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_DFPConsumerPassivate, EDDI_NULL_PTR, pConsumer->pSubFrmHandler, ConsID:0x%X", 
                          pConsumer->ConsumerId);
        return EDD_STS_ERR_PARAM;
    }
    
    pConsumer->pSubFrmHandler->ConsProvID  = EDDI_CONSUMER_ID_INVALID;
    pConsumer->pSubFrmHandler->UsedByUpper = LSA_FALSE;
    pConsumer->pSubFrmHandler              = EDDI_NULL_PTR;
    
    EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, 
                      "EDDI_DFPConsumerPassivate, SubFrameHandler FREED: ConsId:0x%X DG/FrameId:%d/0x%X pSubFrmHandler:0x%X", 
                      pConsumer->ConsumerId, pConsumer->LowerParams.SFPosition, pConsumer->LowerParams.FrameId, (LSA_UINT32)pConsumer->pSubFrmHandler);

    EDDI_CRTConsumerIRTtopSM(pDDB, pConsumer, EDDI_CONS_SM_TRIGGER_PASSIVATE);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPCheckBufferProperties()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//check if the buffer properties of DGConsumer and PFConsumer match
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pConsumer)
{
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        EDDI_CRT_CONSUMER_PTR_TYPE  pPFConsumer;
    
        EDDI_DFPConsGetPF(pDDB, &pPFConsumer, pConsumer);

        //if the PFConsumer has properties already, they have to match the properties of the DGConsumer. 
        if  (   (   ((pPFConsumer->LowerParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK) != EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED)
            && (pPFConsumer->LowerParams.BufferProperties != pConsumer->LowerParams.BufferProperties) )
             || (   (pPFConsumer->LowerParams.CycleReductionRatio != EDD_CYCLE_REDUCTION_RATIO_UNDEFINED)
                 && (pPFConsumer->LowerParams.CycleReductionRatio != pConsumer->LowerParams.CycleReductionRatio) )
             || (   (pPFConsumer->LowerParams.CyclePhase          != EDD_CYCLE_PHASE_UNDEFINED)
                 && (pPFConsumer->LowerParams.CyclePhase          != pConsumer->LowerParams.CyclePhase) ) )
        {
            EDDI_CRT_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_DFPCheckBufferProperties, ERROR: Properties/CRR/CyclePhase do not match, ConsumerID:0x%X, Prop:0x%X/CRR:0x%X/CyclePhase:0x%X / pFConsumerID:0x%X, Prop:0x%X/CRR:0x%X/CyclePhase:0x%X",
                              pConsumer->ConsumerId,     pConsumer->LowerParams.BufferProperties,   pConsumer->LowerParams.CycleReductionRatio,   pConsumer->LowerParams.CyclePhase,
                              pConsumer->PFConsumerId, pPFConsumer->LowerParams.BufferProperties, pPFConsumer->LowerParams.CycleReductionRatio, pPFConsumer->LowerParams.CyclePhase);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
            return EDD_STS_ERR_PARAM;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerActivate()
 *
 *  Arguments:
 *
 *  Return:      LSA_TRUE if consumer is a PF,
 *               LSA_FALSE else.
 ******************************************************************************/
//activates a DG consumer
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerActivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                           EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{                                                         
    LSA_RESULT                               Status;
    EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  const  pSubFrmHandler = EDDI_SyncIrFindSubFrameHandler(pDDB, (LSA_VOID *)pConsumer, LSA_FALSE /*bIsProv*/);

    if (EDDI_NULL_PTR == pSubFrmHandler)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPConsumerActivate, SubFrameHandler not found. ConsumerID:0x%X", pConsumer->ConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
        return EDD_STS_ERR_SEQUENCE;
    }
    else
    {
        //SFIRDATA found and valid!
        Status = EDDI_DFPConsumerCheckDGData(pDDB, pConsumer, pSubFrmHandler);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPConsumerActivate, SFHandler does not match., ConsumerID:0x%X", pConsumer->ConsumerId);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRDATA_do_not_match);
            return EDD_STS_ERR_PARAM;
        }
        else
        {                    
            //set buffer-data for DFP-Consumer
            EDDI_CRTSetImageMode(pDDB, pConsumer, LSA_FALSE /*bIsProv*/);

            //if this is the first DG of a PF being activated, activate the PF also 
            Status =  EDDI_DFPConsActPFFrame(pDDB, pConsumer, pSubFrmHandler);
            if(EDD_STS_OK != Status)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_DFPConsumerActivate, illegal EDDI_DFPConsActPFFrame, ConsumerID:0x%X", pConsumer->ConsumerId);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
                return Status;
            }
            
            pConsumer->pSubFrmHandler   = pSubFrmHandler;
            pSubFrmHandler->UsedByUpper = LSA_TRUE;  //mark framehandler as used and initialized 
            pSubFrmHandler->ConsProvID  = pConsumer->ConsumerId;

            #if defined (EDDI_CFG_REV7)
            if (pConsumer->usingPAEA_Ram)
            {
                //PAEARAM used
                pConsumer->LowerParams.pKRAMDataBuffer      = (EDD_UPPER_MEM_U8_PTR_TYPE)(pDDB->CRT.PAEA_Para.PAEA_BaseAdr + (4 * (pConsumer->LowerParams.DataOffset - sizeof(EDDI_DFP_APDU_TYPE))));
                pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;

                //complete WDT initialization
                pConsumer->pSB->SBEntryDFP.pSFDataStatus  = pConsumer->LowerParams.pKRAMDataBuffer + (EDDI_SFAPDU_OFFSET_SFDataStatus * 4);
                pConsumer->pSB->SBEntryDFP.pSFCycleCtr    = pConsumer->LowerParams.pKRAMDataBuffer + (EDDI_SFAPDU_OFFSET_SFCycleCtr * 4);
            }
            else
            {
                //no PAEARAM or SYNC consumer
                pConsumer->LowerParams.pKRAMDataBuffer      = (EDD_UPPER_MEM_U8_PTR_TYPE)((LSA_UINT32)pDDB->pKRam + pConsumer->LowerParams.DataOffset - sizeof(EDDI_DFP_APDU_TYPE));
                pConsumer->LowerParams.bKRAMDataBufferValid = LSA_TRUE;

                //complete WDT initialization
                pConsumer->pSB->SBEntryDFP.pSFDataStatus  = pConsumer->LowerParams.pKRAMDataBuffer + EDDI_SFAPDU_OFFSET_SFDataStatus;
                pConsumer->pSB->SBEntryDFP.pSFCycleCtr    = pConsumer->LowerParams.pKRAMDataBuffer + EDDI_SFAPDU_OFFSET_SFCycleCtr;
            }
            #else
                #error "not allowed here"
            #endif

            //complete WDT initialization
            pConsumer->pSB->SBEntryDFP.WDTReloadValue = (LSA_UINT8) (EDD_CRT_DFP_WD_RELOAD_VALUE + 
                (pConsumer->LowerParams.DataHoldFactor * pDDB->CycCount.CycleLength_us/1000)/100); /* correct the reload value if DG_DHF * Sendclock exceeds the default DG DHT */

            EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_DFPConsumerActivate, FrameHandler USED, ConsId:0x%X DG/FrameId:%d/0x%X pFrmHandler:0x%X", 
                pConsumer->ConsumerId, pConsumer->LowerParams.SFPosition, pConsumer->LowerParams.FrameId, (LSA_UINT32)pSubFrmHandler);

            EDDI_CRTConsumerIRTtopSM(pDDB, pConsumer, EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA);
        }
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerSetEvents()
 *
 *  Arguments:
 *
 *  Return:      LSA_TRUE if consumer is a PF,
 *               LSA_FALSE else.
 ******************************************************************************/
//checks if event occured in a packframe, return LSA_FALSE if not
LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerSetEvents( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                         EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pPFConsumer,
                                                         EDDI_CRT_EVENT_TYPE                const  SetEvent)
{
    LSA_UINT32    PendingEvent;

    if (!pPFConsumer->bIsPFConsumer)
    {
        return LSA_FALSE;
    }
    else
    {
        if ((EDD_CSRT_CONS_EVENT_MISS + EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED + EDD_CSRT_CONS_EVENT_AGAIN) & SetEvent)
        {            
            EDDI_CRT_CONSUMER_PTR_TYPE          pConsumer;
            EDDI_PRM_SF_DSCR_TYPE             * pSFDscr = ((EDDI_PRM_PF_DSCR_TYPE *)(void *)(pPFConsumer->pSubFrmHandler))->pSF;  //first SF in chain
            EDDI_CRT_CONSUMER_LIST_TYPE       * const pConsList = &pDDB->pLocal_CRT->ConsumerList;
            LSA_UINT8                           SFCtr = ((EDDI_PRM_PF_DSCR_TYPE *)(void *)(pPFConsumer->pSubFrmHandler))->NrOfSubFrames;

            //handle all DGs in this PF by recursively calling EDDI_CRTConsumerSetPendingEvent for each DG
            do
            {
                if (0 == SFCtr)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_DFPConsumerSetEvents, ERROR: SFCtr underrun, NrOfSubFrames:0x%X pFConsumerID:0x%X",
                                      ((EDDI_PRM_PF_DSCR_TYPE *)(void *)(pPFConsumer->pSubFrmHandler))->NrOfSubFrames, pPFConsumer->PFConsumerId);
                    EDDI_Excp("EDDI_DFPConsumerSetEvents, ERROR: SFCtr underrun", EDDI_FATAL_ERR_EXCP, ((EDDI_PRM_PF_DSCR_TYPE *)(void *)(pPFConsumer->pSubFrmHandler))->NrOfSubFrames, pPFConsumer->PFConsumerId);
                }
            
                //get DG-consumer
                if (pSFDscr->UsedByUpper)
                {
                    if (EDD_STS_OK != EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer, pConsList, pSFDscr->ConsProvID))
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_DFPConsumerSetEvents, ERROR: Could not get pConsumer, ConsumerID:0x%X pFConsumerID:0x%X",
                                          pSFDscr->ConsProvID, pPFConsumer->PFConsumerId);
                        EDDI_Excp("EDDI_DFPConsumerSetEvents, ERROR: Could not get pConsumer", EDDI_FATAL_ERR_EXCP, pSFDscr->ConsProvID, pPFConsumer->PFConsumerId);
                    }
            
                    //only active DGs 
                    if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
                    {
                        EDDI_DFP_SB_APDU_TYPE       DGAPDU;
                        #if (defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2))) || (defined (EDDI_CFG_CONSTRACE_DEPTH)) //LSA-Trace
                        LSA_UINT32                  DGAPDUAsDWord;
                        #endif              
                                                
                        if (pConsumer->usingPAEA_Ram)
                        {
                            //PAEARAM allows reading of an uint32 at any offset
                            DGAPDU.AsDword = *(LSA_UINT32 *)((LSA_VOID *)(pConsumer->LowerParams.pKRAMDataBuffer));
                        }
                        else
                        {
                            DGAPDU.SFAPDU.SFPosition   = *(pConsumer->LowerParams.pKRAMDataBuffer + EDDI_SFAPDU_OFFSET_SFPosition);
                            DGAPDU.SFAPDU.SFLen        = *(pConsumer->LowerParams.pKRAMDataBuffer + EDDI_SFAPDU_OFFSET_SFLen);
                            DGAPDU.SFAPDU.SFCycleCtr   = *(pConsumer->LowerParams.pKRAMDataBuffer + EDDI_SFAPDU_OFFSET_SFCycleCtr);
                            DGAPDU.SFAPDU.SFDataStatus = *(pConsumer->LowerParams.pKRAMDataBuffer + EDDI_SFAPDU_OFFSET_SFDataStatus);
                        }                        
                        #if (defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2))) || (defined (EDDI_CFG_CONSTRACE_DEPTH)) //LSA-Trace
                        DGAPDUAsDWord = DGAPDU.AsDword; // Needed to be passed to trace functions. Compiler does not guarantee members of a packed union to be on 4B boundaries. 
                        #endif      
                        switch (SetEvent & (EDD_CSRT_CONS_EVENT_MISS + EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED + EDD_CSRT_CONS_EVENT_AGAIN))
                        {
                            case EDD_CSRT_CONS_EVENT_AGAIN:
                            {
                                //handle only DGs waiting for PF-AGAIN
                                if (EDDI_CRT_CONS_UK_PENDING == pConsumer->pSB->UkStatus)
                                {
                                    if  /* valid DG received ((DG.DS.Valid == 1) && (DG.DS.Surrogat == 0))*/
                                        (EDDI_DS_BIT2_DATA_VALID == (DGAPDU.SFAPDU.SFDataStatus & (EDDI_DS_BIT2_DATA_VALID + EDDI_DS_BIT7_SURROGAT)))
                                    {

                                        // OK: UKCounting can be switched off again in EDDI_CRTRpsCalculateIndEntry
                                        pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_COUNTING;

                                        //Signal AGAIN
                                        pConsumer->pSB->SBEntryDFP.bPSO             = LSA_TRUE;
                                        pConsumer->pSB->SBEntryDFP.LastDS           = DGAPDU.SFAPDU.SFDataStatus;
                                        pConsumer->pSB->SBEntryDFP.SBEntryValue = (EDDI_DFP_PS_ENTRY_ProdStateCh + EDDI_DFP_PS_ENTRY_ProdState + EDDI_DFP_PS_ENTRY_DataStateCh);

                                        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                                          "EDDI_DFPConsumerSetEvents.. UKState EVENT_AGAIN; ConsID:0x%X FrameID:0x%X APDUStatus:0x%X",
                                                          pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, DGAPDUAsDWord);

                                        if (EDDI_CRTRpsCalculateIndEntry(pConsumer, pConsumer->pSB->SBEntryDFP.SBEntryValue, pDDB))
                                        {
                                            // OK we have a new Event for this Consumer
                                            if (   (pDDB->CRT.Rps.MaxPendingDGCons < (LSA_INT32)pConsumer->ConsumerId)
                                                || (pDDB->CRT.Rps.MaxPendingDGCons > 0xFFFFUL ) ) 
                                            {
                                                pDDB->CRT.Rps.MaxPendingDGCons = (LSA_INT32)pConsumer->ConsumerId;
                                            }
                                        }
                                        pConsumer->pSB->SBEntryDFP.SBEntryValue = EDDI_DFP_PS_ENTRY_ProdState;
                                        break;
                                    }
                                    else
                                    { 
                                        //no break! Signal MISS
                                    }
                                }
                                else
                                {
                                  break;
                                }                                        
                            }
                            //no break;
                            //lint -fallthrough
                            case EDD_CSRT_CONS_EVENT_MISS:
                            case EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED:
                            {
                                EDDI_CRT_CONSTRACE_TRACE(pConsumer, DGAPDUAsDWord, pConsumer->pSB->UkStatus, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_UKTO_Miss);

                                //clear WDT
                                pConsumer->pSB->SBEntryDFP.bPSO = 0;
                                pConsumer->pSB->SBEntryDFP.WDTValue = 0;
                                pConsumer->pSB->SBEntryDFP.SBEntryValue = 0;
                                pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_OFF;
                                
                                //signal MISS, the SM will generate either MISS or MISS_NOT_STOPPED
                                PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                                EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, DGAPDU.SFAPDU.SFCycleCtr, 0, pDDB);

                                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN,
                                                  "EDDI_DFPConsumerSetEvents.. UKState EVENT_MISS, EVENT_MISS; ConsID:0x%X FrameID:0x%X PendingEvent:0x%X APDUStatus:0x%X",
                                                  pConsumer->ConsumerId, pConsumer->LowerParams.FrameId, PendingEvent, DGAPDUAsDWord);

                                if (PendingEvent)
                                {
                                    // OK we have a new Event for this Consumer
                                    if (   (pDDB->CRT.Rps.MaxPendingDGCons < (LSA_INT32)pConsumer->ConsumerId)
                                        || (pDDB->CRT.Rps.MaxPendingDGCons > 0xFFFFUL ) ) 
                                    {
                                        pDDB->CRT.Rps.MaxPendingDGCons = (LSA_INT32)pConsumer->ConsumerId;
                                    }
                                }
                                break;
                            }
                            default:
                            {
                                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                                                  "EDDI_DFPConsumerSetEvents, ERROR: Unexpected event 0x%X, ConsumerID:0x%X pFConsumerID:0x%X",
                                                  SetEvent, pSFDscr->ConsProvID, pPFConsumer->PFConsumerId);
                                EDDI_Excp("EDDI_DFPConsumerSetEvents, ERROR: Unexpected event", EDDI_FATAL_ERR_EXCP, pSFDscr->ConsProvID, SetEvent);
                            }
                        }
                    }
                }
            
                //get next DG
                pSFDscr = pSFDscr->pNext;
                SFCtr--;
            }    
            while (pSFDscr);
        } //end if
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************
 *  Function:    EDDI_DFPConsumerIsPF()
 *
 *  Arguments:
 *
 *  Return:      LSA_TRUE if consumer is a PF,
 *               LSA_FALSE else.
 ******************************************************************************/
//checks if a consumer is a packframe
LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerIsPF( EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer)
{
    return (pConsumer->bIsPFConsumer);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPCheckBufferProperties()
 *
 *  Arguments:
 *
 *  Return:
 ******************************************************************************/
//Get the PFCons for a DGCons
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsGetPF( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                          EDDI_CRT_CONSUMER_PTR_TYPE      *  const  ppPFConsumer,
                                                          EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pConsumer )
{
    EDDI_CRT_CONSUMER_PTR_TYPE  pPFConsumer = (EDDI_CRT_CONSUMER_PTR_TYPE)0;

    //Get PF Consumer
    if (EDD_STS_OK != EDDI_CRTConsumerListGetEntry(pDDB, &pPFConsumer, &pDDB->pLocal_CRT->ConsumerList, pConsumer->PFConsumerId))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPConsGetPF, ERROR: Could not get pConsumer, ConsumerID:0x%X pFConsumerID:0x%X",
                          pConsumer->ConsumerId, pConsumer->PFConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
        EDDI_Excp("EDDI_DFPConsGetPF, ERROR: Could not get pConsumer", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->PFConsumerId);
    }

    if (!pPFConsumer->bIsPFConsumer)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPConsGetPF, ERROR: Cons is no PF. ConsumerID:0x%X pFConsumerID:0x%X",
                          pConsumer->ConsumerId, pConsumer->PFConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerID);
        EDDI_Excp("EDDI_DFPConsGetPF, ERROR: Cons is no PF", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->PFConsumerId);
    }
    
    *ppPFConsumer = pPFConsumer;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*--------------------------------------------------------------------------*
 * Scoreboardhandling                                                       *
 *--------------------------------------------------------------------------*/
/******************************************************************************
 *  Function:    EDDI_DFPWatchDogTimerAlloc()
 *
 *  Arguments:
 *
 *  Return:      
 ******************************************************************************/
//Allocates the datagramm-watchdog-timer
//cannot be included in EDDI_DFPWatchDogStart, as te timers have to be allocated at DEV_OPEN
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogTimerAlloc(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT const Status = EDDI_AllocTimer(pDDB,
                                              &pDDB->CRT.Rps.DFPSWSBTimerID,
                                             (void *)pDDB /*context*/,
                                             (EDDI_TIMEOUT_CBF)EDDI_DFPSWSBHandler,
                                             EDDI_TIMER_TYPE_CYCLIC,
                                             EDDI_TIME_BASE_100MS,
                                             EDDI_TIMER_REST);

    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DFPWatchDogTimerAlloc, EDDI_DFPSWSBHandler, EDDI_AllocTimer FAILED.Status:0x%X, TimerID:0x%X", 
                         Status, pDDB->CRT.Rps.DFPSWSBTimerID);
        EDDI_Excp("EDDI_DFPWatchDogTimerAlloc, EDDI_DFPSWSBHandler, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->CRT.Rps.DFPSWSBTimerID);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPWatchDogStart()
 *
 *  Arguments:
 *
 *  Return:      
 ******************************************************************************/
//starts the datagramm-watchdog-handling
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogStart(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT Status;
    
    //init DGSWSB
    pDDB->CRT.Rps.DFPSB.pNext = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)&(pDDB->CRT.Rps.DFPSB.pNext); //temporarily_disabled_lint !e826 Warning 826: Suspicious pointer-to-pointer conversion (area too small)   
    pDDB->CRT.Rps.DFPSB.pPrev = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)&(pDDB->CRT.Rps.DFPSB.pNext); //temporarily_disabled_lint !e826 Warning 826: Suspicious pointer-to-pointer conversion (area too small)  
    
    //Start wd-timer
    Status = EDDI_StartTimer(pDDB, pDDB->CRT.Rps.DFPSWSBTimerID, 1);
    if (Status != EDD_STS_OK)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DFPWatchDogStart, EDDI_DFPSWSBHandler, EDDI_StartTimer FAILED.Status:0x%X, TimerID:0x%X", 
                         Status, pDDB->CRT.Rps.DFPSWSBTimerID);
        EDDI_Excp("EDDI_DFPWatchDogStart, EDDI_DFPSWSBHandler, EDDI_StartTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->CRT.Rps.DFPSWSBTimerID);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPWatchDogStop()
 *
 *  Arguments:
 *
 *  Return:      
 ******************************************************************************/
//stops the datagramm-watchdog-handling
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogStop(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_RESULT const Status = EDDI_StopTimer(pDDB, pDDB->CRT.Rps.DFPSWSBTimerID);

    switch (Status)
    {
        case EDDI_STS_OK_TIMER_NOT_RUNNING:
            break;
        case EDD_STS_OK:
            //Check DGSWSB
            {
                //temporarily_disabled_lint --e{826} Warning 826: Suspicious pointer-to-pointer conversion (area too small)
                if (   (pDDB->CRT.Rps.DFPSB.pNext != (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)(&pDDB->CRT.Rps.DFPSB.pNext))
                    || (pDDB->CRT.Rps.DFPSB.pPrev != (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)(&pDDB->CRT.Rps.DFPSB.pNext)) ) 
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DFPWatchDogStop, SB-List NOT EMPTY. pNext:0x%X, pPrev:0x%X", 
                                      (LSA_UINT32)pDDB->CRT.Rps.DFPSB.pNext, (LSA_UINT32)pDDB->CRT.Rps.DFPSB.pPrev);
                    EDDI_Excp("EDDI_DFPWatchDogStop, SB-List NOT EMPTY", EDDI_FATAL_ERR_EXCP, (LSA_UINT32)pDDB->CRT.Rps.DFPSB.pNext, (LSA_UINT32)pDDB->CRT.Rps.DFPSB.pPrev);
                    return;
                }
            }
            break;
        default:
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DFPWatchDogStop, EDDI_DFPSWSBHandler, EDDI_StopTimer FAILED.Status:0x%X, TimerID:0x%X", 
                             Status, pDDB->CRT.Rps.DFPSWSBTimerID);
            EDDI_Excp("EDDI_DFPWatchDogStart, EDDI_DFPSWSBHandler, EDDI_AllocTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->CRT.Rps.DFPSWSBTimerID);
            return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPSWSBHandler()
 *
 *  Arguments:
 *
 *  Return:      
 ******************************************************************************/
//central DFP SW-SB routine
//is called every 100ms
static LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPSWSBHandler( LSA_VOID  *  const  context )
{
    LSA_RESULT                      Status;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    EDDI_DFP_SB_ENTRY_PTR_TYPE      pSBEntry = pDDB->CRT.Rps.DFPSB.pNext;
    EDDI_DFP_SB_ENTRY_PTR_TYPE      pSBEntryNext;
    LSA_UINT32               const  EndP = (LSA_UINT32)&pDDB->CRT.Rps.DFPSB.pNext;       
    LSA_UINT8                       DGDataStatus;
    LSA_UINT32                      SBEntryValue;    
    LSA_INT32                       PendingEventMaxConsIDDFP = -1;
    EDDI_CRT_CONSUMER_PTR_TYPE      pConsumer; 
    
    if (EDDI_NULL_PTR == context)
    {
        EDDI_Excp("EDDI_DFPSWSBHandler, EDDI_NULL_PTR == context", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //Process WD list
    while /* List not empty */
          ((LSA_UINT32)pSBEntry != EndP)
    {
        LSA_BOOL bWDTRetriggered = LSA_FALSE;
        
        /* Get DataStatus */
        DGDataStatus = *pSBEntry->pSFDataStatus;
        SBEntryValue = pSBEntry->SBEntryValue;    
        pConsumer = pSBEntry->pConsumer;
        pSBEntryNext = pSBEntry->Link.pNext;    //store next ptr, as this consumer could be removed from list later on       

        if  /* DG does not wait for PF-AGAIN */
            (pConsumer->pSB->UkStatus != EDDI_CRT_CONS_UK_PENDING)
        {
            if  /* DG is neither invalid nor a surrogat */
                (EDDI_DS_BIT2_DATA_VALID == (DGDataStatus & (EDDI_DS_BIT2_DATA_VALID + EDDI_DS_BIT7_SURROGAT)))
            {
                //retrigger WDT
                pSBEntry->WDTValue = pSBEntry->WDTReloadValue;
                SBEntryValue |= EDDI_DFP_PS_ENTRY_ProdState;
                bWDTRetriggered = LSA_TRUE;
                
                if  /* Consumer had not been received before */
                    (!pSBEntry->bPSO)
                {
                    pSBEntry->bPSO = LSA_TRUE;
                    SBEntryValue |= EDDI_DFP_PS_ENTRY_ProdStateCh;
                }
            }
        } 
        
        //WDT decrement only if it had not been retriggered before
        if (!bWDTRetriggered)
        {
            //if WDT is 0, then do not check
            if (0 != pSBEntry->WDTValue)
            {
                pSBEntry->WDTValue--;
                if (0 == pSBEntry->WDTValue)
                {
                    //WDT expired ==> MISS
                    SBEntryValue = (SBEntryValue & ~EDDI_DFP_PS_ENTRY_ProdState) | EDDI_DFP_PS_ENTRY_ProdStateCh;
                }
            }
        }
        else
        {
            //Check change of datastatus
            if ((DGDataStatus ^ pSBEntry->LastDS) /*& (EDD_CSRT_DSTAT_BIT_STATE + EDD_CSRT_DSTAT_BIT_1 + EDD_CSRT_DSTAT_BIT_DATA_VALID + EDD_CSRT_DSTAT_BIT_STOP_RUN + EDD_CSRT_DSTAT_BIT_STATION_FAILURE + EDD_CSRT_DSTAT_BIT_6)*/)
            {
                SBEntryValue |= EDDI_DFP_PS_ENTRY_DataStateCh;
                pSBEntry->LastDS = DGDataStatus;
            }        
        }
                
        //check if an event has to be generated
        if (EDDI_PS_BIT__ProdDataStateCh(SBEntryValue))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                              "EDDI_DFPSWSBHandler INFO: Checking Scoreboardentry; ConsumerID, SBEntryValue -> ConsumerID:0x%X SBEntryValue:0x%X",
                              pConsumer->ConsumerId, SBEntryValue);

            if (EDDI_CRTRpsCalculateIndEntry(pConsumer, SBEntryValue, pDDB))
            {
                // OK we have a new Event for this Consumer
                if (PendingEventMaxConsIDDFP < (LSA_INT32)pConsumer->ConsumerId)
                {
                    PendingEventMaxConsIDDFP = (LSA_INT32)pConsumer->ConsumerId;
                }
            }
        }

        //update TimerScoreboard value
        pSBEntry->SBEntryValue = SBEntryValue & ~(EDDI_DFP_PS_ENTRY_DataStateCh + EDDI_DFP_PS_ENTRY_ProdStateCh);
        
        //get next DG
        pSBEntry = pSBEntryNext;        
    } //end while         

    //send events, if any
    if (PendingEventMaxConsIDDFP > 0)
    {
        Status = EDDI_CRTRpsFillAndSendIndication(pDDB, 0xFFFFFFFF, (LSA_UINT32)PendingEventMaxConsIDDFP, LSA_TRUE /*bDirectIndication*/);
        if (Status != EDD_STS_OK)
        {
            EDDI_CRTRpsInformStateMachine(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPWatchDogInit()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//initializes a sw-sb-entry for a DG
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogInit( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                       EDDI_CRT_CONSUMER_PTR_TYPE const  pConsumer)
{
    LSA_UNUSED_ARG(pDDB);

    pConsumer->PendingIndEvent = 0;
    pConsumer->PendingCycleCnt = 0;
    pConsumer->EventStatus     = 0;

    pConsumer->pSB->OldDataStatus = EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;
    
    //SBEntry is only initialized here, WD gets started in EDDI_DFPConsumerSetToUnknown!
    pConsumer->pSB->SBEntryDFP.bPSO           = LSA_FALSE;
    pConsumer->pSB->SBEntryDFP.LastDS         = EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS;
    pConsumer->pSB->SBEntryDFP.pConsumer      = pConsumer;
    //pSFDataStatus, pSFCycleCtr and WDTReloadValue get initialized in EDDI_DFPConsumerActivate
    pConsumer->pSB->SBEntryDFP.WDTValue       = 0;
    pConsumer->pSB->SBEntryDFP.SBEntryValue = 0;
    pConsumer->pSB->SBEntryDFP.bInWDTList    = LSA_FALSE;
        
    return EDD_STS_OK;
}                                              
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerSetToUnknown()
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 ******************************************************************************/
//Set a DG-Consumer to UNKNOWN state
LSA_BOOL  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerSetToUnknown(EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                             EDDI_CRT_CONSUMER_PTR_TYPE const  pConsumer,
                                                             EDDI_CRT_CONS_HWWD         const  eHwWatchdog,
                                                             LSA_BOOL                   const  bTrigConsSBSM )
{
    LSA_BOOL                    bPendingEvent = LSA_FALSE;
    EDDI_CRT_CONSUMER_PTR_TYPE  pPFConsumer;
    LSA_UINT32                  PendingEvent;
    
    EDDI_DFPConsGetPF(pDDB, &pPFConsumer, pConsumer);

    if /* set2unknown-command has been given */
       (bTrigConsSBSM)
    {
        if( EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN)){} //Lint
    }

    //check state of PF and DG
    if (   (EDDI_CONS_SCOREBOARD_STATE_MISS    == pPFConsumer->ScoreBoardStatus)
        || (EDDI_CONS_SCOREBOARD_STATE_PASSIVE == pPFConsumer->ScoreBoardStatus) 
        || (   (EDDI_CRT_CONS_STS_ACTIVE != pConsumer->pSB->Status)
            && (EDDI_CRT_CONS_HWWD_LOAD != eHwWatchdog)))
    {
        //PF does not have an active state ==> DG produces MISS
        PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT);
        EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, 0, 0, pDDB);
        if (PendingEvent)
        {
            bPendingEvent = LSA_TRUE;
        }
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                          "EDDI_DFPConsumerSetToUnknown, PF or DG is/are not active. PFConsumerID:0x%X, Event 0x%X generated for ConsumerID:0x%X",
                          pPFConsumer->ConsumerId, PendingEvent, pConsumer->ConsumerId);
    }
    else 
    {
        //PF is UNKNOWN or AGAIN
        pConsumer->pSB->SBEntryDFP.bPSO = LSA_FALSE;  //clear PSO
        EDDI_CRTConsumerSetPendingEvent(pConsumer, (EDDI_CSRT_CONS_EVENT_NEGATION + EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED + EDD_CSRT_CONS_EVENT_MISS + EDD_CSRT_CONS_EVENT_AGAIN), (LSA_UINT16)0, 0, pDDB);

        if (EDDI_CRT_CONS_HWWD_LOAD == eHwWatchdog)
        {
            //Start WDT
            pConsumer->pSB->SBEntryDFP.WDTValue = pConsumer->pSB->SBEntryDFP.WDTReloadValue;
            pConsumer->pSB->UkStatus = (EDDI_CONS_SCOREBOARD_STATE_AGAIN == pPFConsumer->ScoreBoardStatus)?EDDI_CRT_CONS_UK_COUNTING:EDDI_CRT_CONS_UK_PENDING;
            EDDI_DFPAddToSBList(pDDB, pConsumer);
        }
        else
        {
            //WDT is kept untouched, just set UkStatus
            if (EDDI_CONS_SCOREBOARD_STATE_AGAIN != pPFConsumer->ScoreBoardStatus)
            {
                //only set to PENDING, if the PF is in an unknown state
                pConsumer->pSB->UkStatus = EDDI_CRT_CONS_UK_PENDING;
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_DFPConsumerSetToUnknown, PFConsumerID 0x%X is in unknown state %d. ConsumerID:0x%X set to UK_PENDING",
                                  pPFConsumer->ConsumerId, pPFConsumer->ScoreBoardStatus, pConsumer->ConsumerId);
            }
        }

        if (EDDI_CONS_SCOREBOARD_STATE_AGAIN == pPFConsumer->ScoreBoardStatus)
        {
            LSA_UINT8  const  SFDataStatus = *pConsumer->pSB->SBEntryDFP.pSFDataStatus;

            if /* DG is active, but invalid or a surrogat */
               (EDDI_DS_BIT2_DATA_VALID != (SFDataStatus  & (EDDI_DS_BIT2_DATA_VALID + EDDI_DS_BIT7_SURROGAT)))
            {
                PendingEvent = EDDI_CRTConsumerScoreBoardSM(pDDB, pConsumer, EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT);
                EDDI_CRTConsumerSetPendingEvent(pConsumer, PendingEvent, 0, 0, pDDB);
                if (PendingEvent)
                {
                    bPendingEvent = LSA_TRUE;
                }
                EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  "EDDI_DFPConsumerSetToUnknown, DG is invalid. DG.DataStatus:0x%X, Event 0x%X generated for ConsumerID:0x%X",
                                  SFDataStatus, PendingEvent, pConsumer->ConsumerId);
            }
        }
    }

    EDDI_CRT_CONSTRACE_TRACE(pConsumer, 0, 0, pConsumer->ScoreBoardStatus, EDDI_CONSTRACE_STATE_SETTOUK);

    return bPendingEvent;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPConsumerGetDS()
 *
 *  Arguments:
 *
 *  Return:      
 ******************************************************************************/
//Get the DS and CC from a DG
LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerGetDS(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                      EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                      LSA_UINT8                       * const  pSFDataStatus,       
                                                      LSA_UINT16                      * const  pSFCycleCtr)
{
    LSA_UNUSED_ARG(pDDB);

    *pSFDataStatus = *pConsumer->pSB->SBEntryDFP.pSFDataStatus; 
    *pSFCycleCtr   = (LSA_UINT16)(*pConsumer->pSB->SBEntryDFP.pSFCycleCtr);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPRemoveFromSB()
 *
 *  Arguments:
 *
 *  Return:      -
 ******************************************************************************/
//Remove a SB-Entry from list
static LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPRemoveFromSBList(EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer)
{
    EDDI_DFP_SB_LINK_TYPE  *   const pAnchor  = &pDDB->CRT.Rps.DFPSB;

    if (   ((LSA_UINT32)pAnchor->pNext == (LSA_UINT32)&pAnchor->pNext)
        || ((LSA_UINT32)pAnchor->pPrev == (LSA_UINT32)&pAnchor->pPrev) 
        || !pConsumer->pSB->SBEntryDFP.bInWDTList)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPRemoveFromSBList, ERROR: Remove from empty or corrupted list, ConsumerID:0x%X, bInWDTList:%X",
                          pConsumer->ConsumerId, pConsumer->pSB->SBEntryDFP.bInWDTList);
        EDDI_Excp("EDDI_DFPRemoveFromSBList, ERROR: Remove from empty or corrupted list", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->pSB->SBEntryDFP.bInWDTList);
    }
    
    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_DFPAddRemoveFromSBList REMOVE, ConsumerID:0x%X", pConsumer->ConsumerId);

    //Next.pPrev = Curr.pPrev
    pConsumer->pSB->SBEntryDFP.Link.pNext->Link.pPrev = pConsumer->pSB->SBEntryDFP.Link.pPrev;
    //Prev.pNext = Curr.pNext
    pConsumer->pSB->SBEntryDFP.Link.pPrev->Link.pNext = pConsumer->pSB->SBEntryDFP.Link.pNext;

    pConsumer->pSB->SBEntryDFP.Link.pNext = 
    pConsumer->pSB->SBEntryDFP.Link.pPrev = (struct _EDDI_DFP_SB_ENTRY_TYPE  *)0xFFFFFFFF;

    pConsumer->pSB->SBEntryDFP.bInWDTList = LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_DFPAddToSB()
 *
 *  Arguments:
 *
 *  Return:      -
 ******************************************************************************/
//Add a SB-Entry to last of list
static LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPAddToSBList(EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer)
{
    EDDI_DFP_SB_LINK_TYPE  *   const pAnchor  = &pDDB->CRT.Rps.DFPSB;
    EDDI_DFP_SB_ENTRY_PTR_TYPE const pSB      = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)&pConsumer->pSB->SBEntryDFP.Link.pNext; /*lint !e826 Warning 826: Suspicious pointer-to-pointer conversion (area too small)*/
    EDDI_DFP_SB_ENTRY_TYPE *   const pSBStore = pAnchor->pPrev->Link.pPrev->Link.pNext;

    if (pConsumer->pSB->SBEntryDFP.bInWDTList)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL,
                          "EDDI_DFPAddFromSBList, ERROR: Consumer already in WDT list, ConsumerID:0x%X, bInWDTList:%X",
                          pConsumer->ConsumerId, pConsumer->pSB->SBEntryDFP.bInWDTList);
        EDDI_Excp("EDDI_DFPAddFromSBList, ERROR: Consumer already in WDT list", EDDI_FATAL_ERR_EXCP, pConsumer->ConsumerId, pConsumer->pSB->SBEntryDFP.bInWDTList);
    }

    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,"EDDI_DFPAddRemoveFromSBList ADD, ConsumerID:0x%X", pConsumer->ConsumerId);
       
    //Last SB-Entry pNext = Curr  
    pAnchor->pPrev->Link.pNext = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)pSB;
    //pAnchor->pPrev = Curr
    pAnchor->pPrev = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)pSB;
    //Curr->pNext = pAnchor
    pSB->Link.pNext = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)pAnchor; /*lint !e826 Warning 826: Suspicious pointer-to-pointer conversion (area too small)*/
    //Curr->Prev = pSBStore (Last SB-Entry pPrev)   
    pSB->Link.pPrev = (EDDI_DFP_SB_ENTRY_PTR_TYPE)(void *)pSBStore;   

    pConsumer->pSB->SBEntryDFP.bInWDTList = LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


#else //if defined (EDDI_CFG_DFP_ON)
/* #########################################################################################################*/
/* Dummy-Functions                                                                                          */
/* #########################################################################################################*/
//Checks if all conditions for a DFP frame are met
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT8                const  ListType,
                                                          LSA_UINT8                const  ConsumerType )
{
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;
    
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(ListType);
    LSA_UNUSED_ARG(ConsumerType);

    if (EDDI_IS_DFP_CONS(pConsParam))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_DFPConsAddCheckRQB, DFP not supported. FrameID:0x%X, Properties:0x%X",
                          pConsParam->FrameID, pConsParam->Properties);
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//checks if the conditions for passivating a packframe are met, and passivate it
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsPassPFFrame(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                       EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer)
{
    LSA_UNUSED_ARG(pDDB);

    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        return EDD_STS_ERR_PARAM;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//checks if the conditions for removing a packframe are met
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsRemovePFHandler(EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer)
{
    LSA_UNUSED_ARG(pDDB);
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        return (EDD_STS_ERR_PARAM);
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//initializes a DFP-consumer
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerInit( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                       EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                       EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam)
{                                                               
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pConsumer);
    LSA_UNUSED_ARG(pConsParam);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/
    
//Passivate a DG
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerPassivate(EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                            LSA_BOOL                    const  bClearEvents)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pConsumer);
    LSA_UNUSED_ARG(bClearEvents);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

//check if the buffer properties of DGConsumer and PFConsumer match
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pConsumer)
{
    LSA_UNUSED_ARG(pDDB);
    if (EDDI_IS_DFP_CONS(pConsumer))
    {
        return EDD_STS_ERR_PARAM;
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//activates a DG consumer
LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerActivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                          EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer)
{                                                         
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pConsumer);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

//checks if event occured in a packframe, return LSA_FALSE if not
LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerSetEvents( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                         EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pPFConsumer,
                                                         EDDI_CRT_EVENT_TYPE                const  SetEvent)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pPFConsumer);
    LSA_UNUSED_ARG(SetEvent);

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/
    
//Allocates the datagramm-watchdog-timer
//cannot be included in EDDI_DFPWatchDogStart, as te timers have to be allocated at DEV_OPEN
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogTimerAlloc(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

//starts the datagramm-watchdog-handling
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogStart(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

//stops the datagramm-watchdog-handling
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogStop(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB)
{
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

//initializes a sw-sb-entry for a DG
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pConsumer);

    return EDD_STS_ERR_PARAM;
}                                              
/*---------------------- end [subroutine] ---------------------------------*/

//Set a DG-Consumer to UNKNOWN state
LSA_BOOL    EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerSetToUnknown(EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE const  pConsSet2UK,
                                                               EDDI_CRT_CONS_HWWD         const  eHwWatchdog,
                                                               LSA_BOOL                   const  bTrigConsSBSM )
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pConsSet2UK);
    LSA_UNUSED_ARG(eHwWatchdog);
    LSA_UNUSED_ARG(bTrigConsSBSM);

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

#if !defined (EDDI_CFG_USE_SW_RPS)
//Get the DS and CC from a DG
LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerGetDS(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                      EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                      LSA_UINT8                       * const  pSFDataStatus,       
                                                      LSA_UINT16                      * const  pSFCycleCtr)
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pConsumer);
    LSA_UNUSED_ARG(pSFDataStatus);
    LSA_UNUSED_ARG(pSFCycleCtr);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //!defined (EDDI_CFG_USE_SW_RPS)

//checks if a consumer is a packframe
LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerIsPF( EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer)
{
    LSA_UNUSED_ARG(pConsumer);
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //defined (EDDI_DFP_ON)


/*****************************************************************************/
/*  end of file eddi_crt_dfpcons.c                                           */
/*****************************************************************************/

