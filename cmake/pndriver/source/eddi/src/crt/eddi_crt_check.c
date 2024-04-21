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
/*  F i l e               &F: eddi_crt_check.c                          :F&  */
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
#include "eddi_dev.h"
#include "eddi_crt_ext.h"
#include "eddi_ser_ext.h"
#include "eddi_crt_check.h"
#include "eddi_crt_com.h"
#include "eddi_pool.h"
#include "eddi_crt_dfp.h"

#define EDDI_MODULE_ID     M_ID_CRT_CHECK
#define LTRC_ACT_MODUL_ID  101

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
//#define EDDI_ERR_INV_DataHoldStatus                    0x00050321UL
#endif

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTGetListType()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_UINT8  EDDI_CRTGetListType( LSA_UINT16  const  Properties, 
                                LSA_UINT16  const  FrameID )
{
    LSA_UINT8          ListType;
    LSA_UINT16  const  Prop = Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK; 

    if //RT-CLASS1/2-FrameID-range
       (   (EDD_CSRT_CONS_PROP_RTCLASS_1 == Prop)   || (EDD_CSRT_PROV_PROP_RTCLASS_1 == Prop)
        || (EDD_CSRT_CONS_PROP_RTCLASS_UDP == Prop) || (EDD_CSRT_PROV_PROP_RTCLASS_UDP == Prop)
        || (EDD_CSRT_CONS_PROP_RTCLASS_2 == Prop)   || (EDD_CSRT_PROV_PROP_RTCLASS_2 == Prop) )
    {
        /* FrameID can be UNDEFINED! */
        ListType = EDDI_LIST_TYPE_ACW;
    }
    else if //RT-CLASS3-FrameID-range
            ((FrameID >= EDDI_SRT_FRAMEID_RT_CLASS3_START) && (FrameID <= EDDI_SRT_FRAMEID_RT_CLASS3_STOP))
    {
        if ((EDD_CSRT_CONS_PROP_RTCLASS_3 == Prop) || (EDD_CSRT_PROV_PROP_RTCLASS_3 == Prop))
        {
            ListType = EDDI_LIST_TYPE_FCW;
        }
        else
        {
            EDDI_Excp("EDDI_CRTGetListType - Invalid FrameID or Properties", EDDI_FATAL_ERR_EXCP, FrameID, Properties);
            ListType = 0;
        }
    }
    else
    {
        EDDI_Excp("EDDI_CRTGetListType - Invalid FrameID or Properties", EDDI_FATAL_ERR_EXCP, FrameID, Properties);
        ListType = 0;
    }

    return ListType;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTGetProviderType()                        */
/*                                                                         */
/* D e s c r i p t i o n: inclusive FrameID-check                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          Provider-Type                                    */
/*                                                                         */
/***************************************************************************/
LSA_UINT8  EDDI_CRTGetProviderType( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                    EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                    LSA_UINT8                const  ListType )
{
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam      = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;
    EDDI_CRT_INI_PTR_TYPE                 const  pCrtCfgPara     = &pDDB->CRT.CfgPara;
    LSA_UINT16                            const  ProviderFrameID = pProvParam->FrameID;
    LSA_UINT16                            const  Prop            = pProvParam->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK;
    
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTGetProviderType");


    if (EDD_CSRT_CONS_PROP_RTCLASS_UDP == Prop) 
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetProviderType - UDP Provider is not allowed ->:0x%X :0x%X",
                          Prop, ListType);
                               
        return EDDI_RT_PROVIDER_TYPE_INVALID;
    }

    if //RT-CLASS1-FrameID-range
       ((EDD_CSRT_PROV_PROP_RTCLASS_1 == Prop) || (EDD_CSRT_PROV_PROP_RTCLASS_UDP == Prop) )
    {
    
        if (   (pCrtCfgPara->SRT.ProviderCnt)
            && (ListType == EDDI_LIST_TYPE_ACW))
        {
            if (pProvParam->DataLen)
            {
                return EDDI_RTC1_PROVIDER;
            }          
            else
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetProviderType - Class1 - Invalid DataLen = 0 ->:0x%X :0x%X",
                                  ProviderFrameID, ListType);

                return EDDI_RT_PROVIDER_TYPE_INVALID;
            }
        }
    }
    else if //RT-CLASS2-FrameID-range
            (EDD_CSRT_PROV_PROP_RTCLASS_2 == Prop)
    {
        if (   (pCrtCfgPara->SRT.ProviderCnt)
            && (ListType == EDDI_LIST_TYPE_ACW))
        {
            if (pProvParam->DataLen)
            {
                return EDDI_RTC2_PROVIDER;
            }
            else
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetProviderType - Class2 - Invalid DataLen = 0 ->:0x%X :0x%X",
                                  ProviderFrameID, ListType);

                return EDDI_RT_PROVIDER_TYPE_INVALID;
            }
        }
    }
    else if //RT-CLASS3-FrameID-range
            ((ProviderFrameID >= EDDI_SRT_FRAMEID_RT_CLASS3_START) && (ProviderFrameID <= EDDI_SRT_FRAMEID_RT_CLASS3_STOP))
    {               
        if (ListType == EDDI_LIST_TYPE_ACW)
        {
            if (pCrtCfgPara->SRT.ProviderCnt)
            {
                return EDDI_RTC3_AUX_PROVIDER;
            }
        }
        else //ListType == EDDI_LIST_TYPE_FCW
        {
            if (pCrtCfgPara->IRT.ProviderCnt)
            {
                return EDDI_RTC3_PROVIDER;
            }
        }
    } 

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetProviderType, Invalid ProviderFrameID ->:0x%X :0x%X", ProviderFrameID, ListType);

    return EDDI_RT_PROVIDER_TYPE_INVALID;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTGetConsumerType()                        */
/*                                                                         */
/* D e s c r i p t i o n: inclusive FrameID-check                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          Consumer-Type                                    */
/*                                                                         */
/***************************************************************************/
LSA_UINT8  EDDI_CRTGetConsumerType( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                    EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                    LSA_UINT8                const  ListType )
{
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam      = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;
    EDDI_CRT_INI_PTR_TYPE                 const  pCrtCfgPara     = &pDDB->CRT.CfgPara;
    LSA_UINT16                            const  ConsumerFrameID = pConsParam->FrameID;
    LSA_UINT16                            const  Properties      = pConsParam->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTGetConsumerType");


    if (EDD_CSRT_CONS_PROP_RTCLASS_UDP == Properties) 
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetConsumerType - UDP Consumer is not allowed ->:0x%X :0x%X",
                          Properties, ListType);
                               
        return EDDI_RT_CONSUMER_TYPE_INVALID;
    }

    if //RT-CLASS1-FrameID-range
       ((ConsumerFrameID >= EDDI_SRT_FRAMEID_RT_CLASS1_START) && (ConsumerFrameID <= EDDI_SRT_FRAMEID_RT_CLASS1_STOP))
    {
        LSA_UINT16  const  ConsumerCntClass12 = pCrtCfgPara->SRT.ConsumerCntClass12;

        if (   (ConsumerCntClass12)
            && (ListType == EDDI_LIST_TYPE_ACW)
            && (ConsumerFrameID >= pCrtCfgPara->SRT.ConsumerFrameIDBaseClass1)
            && (ConsumerFrameID < (LSA_UINT16)(pCrtCfgPara->SRT.ConsumerFrameIDBaseClass1 + ConsumerCntClass12)))
        {
            if (pConsParam->DataLen)
            {
                if (EDD_CSRT_CONS_PROP_RTCLASS_1 == Properties) 
                {
                    return EDDI_RTC1_CONSUMER;
                }
                else
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetConsumerType - Class1 - Invalid Consumertype ->:0x%X :0x%X",
                                      Properties, ListType);
                    return EDDI_RT_CONSUMER_TYPE_INVALID;
                }
            }
            else
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetConsumerType - Class1 - Invalid DataLen = 0 ->:0x%X :0x%X",
                                  ConsumerFrameID, ListType);

                return EDDI_RT_CONSUMER_TYPE_INVALID;
            }
        }
    }
    else if //RT-CLASS2-FrameID-range
            (  ((ConsumerFrameID >= EDDI_SRT_FRAMEID_RT_CLASS2_START) && (ConsumerFrameID <= EDDI_SRT_FRAMEID_RT_CLASS2_STOP)) )
    {
        LSA_UINT16  const  ConsumerCntClass12 = pCrtCfgPara->SRT.ConsumerCntClass12;

        if (   (ConsumerCntClass12)
            && (ListType == EDDI_LIST_TYPE_ACW)
            && (ConsumerFrameID >= pCrtCfgPara->SRT.ConsumerFrameIDBaseClass2)
            && (ConsumerFrameID < (LSA_UINT16)(pCrtCfgPara->SRT.ConsumerFrameIDBaseClass2 + ConsumerCntClass12)))
        {
            if (   (pConsParam->DataLen)
                && (EDD_CSRT_CONS_PROP_RTCLASS_2 == Properties) ) 
            {
                return EDDI_RTC2_CONSUMER;
            }
            else
            {
                EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetConsumerType - Class2 - Invalid parameters->DataLen:0x%X Properties:0x%X FrameID:0x%X ListType:0x%X",
                                  pConsParam->DataLen, Properties, ConsumerFrameID, ListType);
                return EDDI_RT_CONSUMER_TYPE_INVALID;
            }
        }
    }
    else if //RT-CLASS3-FrameID-range
            ((ConsumerFrameID >= EDDI_SRT_FRAMEID_RT_CLASS3_START) && (ConsumerFrameID <= EDDI_SRT_FRAMEID_RT_CLASS3_STOP))
    {       
        if (pCrtCfgPara->IRT.ConsumerCntClass3)
        {
            return EDDI_RTC3_CONSUMER;
        }
    } 

    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTGetConsumerType, Invalid ConsumerFrameID ->:0x%X :0x%X", ConsumerFrameID, ListType);

    return EDDI_RT_CONSUMER_TYPE_INVALID;
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckGetWDDHReloadValue( LSA_UINT8      const  ConsumerType,
                                                                  LSA_UINT16     const  TimerFactor,
                                                                  LSA_UINT16     const  CycleReductionRatio,
                                                                  LSA_UINT32  *  const  pNewCRR )
{
    LSA_UINT32  TimerReloadValue;

    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
    // Revision 5 and 6 are downcounting Watchdogtimer (and Dataholdtimer REV6) in each(!) Cycle.
    TimerReloadValue = (TimerFactor + 1UL) * CycleReductionRatio;
    *pNewCRR = CycleReductionRatio;

    if (   (TimerReloadValue == (EDDI_MAX_RELOAD_TIME_VALUE + 1UL))
        || (    (EDDI_RTC3_AUX_CONSUMER == ConsumerType)
             && (TimerReloadValue > EDDI_MAX_RELOAD_TIME_VALUE) ) )
    {
        // Special case: Projected WDReloadValue is exactly 1 above the max value:
        //               Example: DataHoldFactor: 15, CycleReductionRatio: 512
        //                          -> WDReloadValue  = (15 + 1 ) * 512 = 8192
        //                          -> WDReloadValue is decremented by 1 to fit into
        //                             the HW register.
        TimerReloadValue = EDDI_MAX_RELOAD_TIME_VALUE;
    }

    #elif defined (EDDI_CFG_REV7)
    {
        LSA_UINT32 NewCRR = CycleReductionRatio;
        
        LSA_UNUSED_ARG(ConsumerType); //satisfy lint!

        TimerReloadValue = TimerFactor;
        
        //if ReloadValue exceeds the max value for the WDF/DHF, a new SCRR for the TSB-entry has to be calculated
        while ((TimerReloadValue + 1) > EDDI_MAX_RELOAD_TIME_VALUE)
        {
            NewCRR *= 2; 
            if (NewCRR > EDDI_MAX_TSB_SCRR_VALUE)
            {
                break;  //too big, will lead to -ACK in calling function            
            }    
        
            if ((TimerReloadValue % 2) != 0)
            { 
                TimerReloadValue = TimerReloadValue/2 + 1;
            }
            else
            { 
                TimerReloadValue = TimerReloadValue/2;
            }
        }

        *pNewCRR = NewCRR;
        TimerReloadValue += 1;
    }
    #else
    #error EDDI_CFG_ERROR: Revision not defined !
    #endif

    return TimerReloadValue;
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT8                const  ListType,
                                                              LSA_UINT8                const  ConsumerType )
{
    EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam      = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;
    EDDI_CRT_CONSUMER_PTR_TYPE                   pFindConsumer;
    LSA_UINT16                                   KramDatalen;
    LSA_UINT32                                   NewCRR;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerAddCheckRQB");

    //if (!pDDB->NRT.bIO_Configured)   BV
    //{
    //    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB: adding Consumers when IO-Configuration is NO, is not allowed");
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IO_Configured_PDNRTFeedInLoadLimitation);
    //    return EDD_STS_ERR_SERVICE;
    //}

    //if (!pDDB->NRT.bFeedInLoadLimitationActive)
    //{
    //    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB: adding Consumers when LoadLimitationActive is FALSE, is not allowed");
    //    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_LoadLimitationActive_PDNRTFeedInLoadLimitation);
    //    return EDD_STS_ERR_SERVICE;
    //}


    #if defined (EDDI_CFG_REV7)
    if (     (EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC == (pConsParam->IOParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK))
         &&  (EDD_CSRT_CONS_PROP_RTCLASS_3 == (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK))
         #if defined (EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
         &&  (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY)
         #endif //(EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
       )
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB: Sync Image and Legacy Mode and RTC3 is on SOC1/2 not allowed");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Consumer_Properties);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    //Check DataOffset
    if( EDD_DATAOFFSET_INVALID == pConsParam->IOParams.DataOffset )
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB: EDD_DATAOFFSET_INVALID.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }
    
    //Check BufferProperties
    if (   (EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID == (pConsParam->IOParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK))
        || (!EDDI_CRTCheckBufferProperties(pDDB, ListType, pConsParam->IOParams.BufferProperties, LSA_FALSE /*bIsProvider*/, LSA_TRUE /*bAllowUndefined*/)) )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB: illegal Bufferproperties:0x%X",
            pConsParam->IOParams.BufferProperties);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRT_XRTProperties);
        return EDD_STS_ERR_PARAM;
    }
         
    //Check forwarding mode
    if(   (EDD_FORWARDMODE_SF        != pConsParam->IOParams.ForwardMode)
       && (EDD_FORWARDMODE_UNDEFINED != pConsParam->IOParams.ForwardMode))
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB: illegal forwarding mode.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ForwardingMode);
        return EDD_STS_ERR_PARAM;
    }

    if (EDDI_IS_DFP_CONS(pConsParam))
    {
        LSA_RESULT  const Result = EDDI_DFPConsAddCheckRQB(pRQB, pDDB, ListType, ConsumerType);
        if (EDD_STS_OK != Result)
        {
            //ErrorTrace is contained in fct above!
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
            return Result;
        }
    }
    else
    {
        if (pConsParam->DataHoldFactor < EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataHoldFactor);
            return EDD_STS_ERR_PARAM;
        }

        if (pConsParam->DataHoldFactor > EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MAX)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataHoldFactor);
            return EDD_STS_ERR_PARAM;
        }

        //check if WatchDogReloadtime will be exceeded
        if (EDDI_CRTCheckGetWDDHReloadValue(ConsumerType, pConsParam->DataHoldFactor, pConsParam->CycleReductionRatio, &NewCRR) > EDDI_MAX_RELOAD_TIME_VALUE)
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerAddCheckRQB, DataHoldFactor*CycleReductionRatio > EDDI_MAX_RELOAD_TIME_VALUE, ->:0x%X :0x%X",
                              pConsParam->DataHoldFactor * pConsParam->CycleReductionRatio, EDDI_MAX_RELOAD_TIME_VALUE);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataHoldFactor2);
            return EDD_STS_ERR_PARAM;
        }
    }  

    //Check ClearOnMiss
    if (   (EDD_CONS_CLEAR_ON_MISS_DISABLE != pConsParam->IOParams.ClearOnMISS)
        && (EDD_CONS_CLEAR_ON_MISS_ENABLE != pConsParam->IOParams.ClearOnMISS) )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTConsumerAddCheckRQB: ClearOnMISS illegal value (%d)", pConsParam->IOParams.ClearOnMISS);
        EDDI_SET_DETAIL_ERR(pDDB, 0);
        return EDD_STS_ERR_PARAM;
    }


    //Frame-ID is already checked!

    //check List-Type-Dependencies
    switch (ListType)
    {
        case EDDI_LIST_TYPE_ACW:
        {
            #ifndef EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1
            if (pConsParam->Properties & EDD_CSRT_CONS_PROP_SYSRED)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerAddCheckRQB, SysRed is not allowed for RTC3 consumers; ConsumerFrameID:0x%X", pConsParam->FrameID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
                return EDD_STS_ERR_PARAM;
            }
            #endif //EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1

            if (ConsumerType != EDDI_RTC3_AUX_CONSUMER)
            {
                #if defined (EDDI_CFG_REV5)
                if (pConsParam->Partial_DataOffset)
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataOffset not supported. ->:0x%X :0x%X",
                                          pConsParam->Partial_DataOffset, 0);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataOffset);
                    return EDD_STS_ERR_PARAM;
                }

                if (pConsParam->Partial_DataLen)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen not supported. ->:0x%X :0x%X",
                                      pConsParam->Partial_DataLen, EDD_CSRT_DATALEN_MIN);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                    return EDD_STS_ERR_PARAM;
                }
                #else
                if (pConsParam->Partial_DataLen)
                {
                    if (pConsParam->Partial_DataLen > pConsParam->DataLen)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen > pConsParam->DataLen. ->:0x%X :0x%X",
                                          pConsParam->DataLen, pConsParam->Partial_DataLen);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                        return EDD_STS_ERR_PARAM;
                    }

                    if  (  (EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_LO <= (pConsParam->Partial_DataOffset + pConsParam->Partial_DataLen))
                        && (EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_HI >= (pConsParam->Partial_DataOffset + pConsParam->Partial_DataLen))  )
                    {
                        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen (36..39). ->:%d",
                                          pConsParam->Partial_DataLen);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                        return EDD_STS_ERR_PARAM;
                    }

                    if (pConsParam->Partial_DataOffset >= pConsParam->DataLen)
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataOffset >= pConsParam->DataLen. ->:0x%X :0x%X",
                                          pConsParam->Partial_DataOffset, EDD_CSRT_DATALEN_MIN);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataOffset);
                        return EDD_STS_ERR_PARAM;
                    }

                    if (pConsParam->Partial_DataLen > (pConsParam->DataLen - pConsParam->Partial_DataOffset))
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                          "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen >= (pConsParam->DataLen - pConsParam->IOParams.Partial_DataOffset). ->:0x%X :0x%X",
                                          pConsParam->Partial_DataOffset, EDD_CSRT_DATALEN_MIN);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                #endif

                //always check against DATALEN_MIN. Short frames are no longer supported!
                if (pConsParam->DataLen < ((EDDI_UDP_CONSUMER == ConsumerType)?EDD_CSRT_UDP_DATALEN_MIN:EDD_CSRT_DATALEN_MIN))
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, Parameter pConsParam->DataLen too small. ->:0x%X :0x%X",
                                      pConsParam->DataLen, EDD_CSRT_DATALEN_MIN);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
                    return EDD_STS_ERR_PARAM;
                }
            }

            if (pConsParam->DataLen > ((EDDI_UDP_CONSUMER == ConsumerType)?EDD_CSRT_UDP_DATALEN_MAX:EDD_CSRT_DATALEN_MAX))
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerAddCheckRQB, Parameter pConsParam->DataLen too big. ->:0x%X :0x%X",
                                  pConsParam->DataLen, EDD_CSRT_DATALEN_MAX);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
                return EDD_STS_ERR_PARAM;
            }

            /* check CycleReductionRatio */
            /* NOTE: when EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12 is defined, RR > 1 is allowed */
            #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12)
            if (EDD_STS_OK != EDDI_CRTCheckClass12Reduction(pDDB, pDDB->pLocal_CRT->SrtRedTree.MaxReduction, pConsParam->CycleReductionRatio, pConsParam->CyclePhase, LSA_TRUE /*bUndefAllowed*/))
            #else
            if (EDD_STS_OK != EDDI_CRTCheckClass12Reduction(pDDB, (pDDB->CycCount.bCBFIsBinary?pDDB->pLocal_CRT->SrtRedTree.MaxReduction:1), pConsParam->CycleReductionRatio, pConsParam->CyclePhase, LSA_TRUE /*bUndefAllowed*/))
            #endif
            {
                //detail has already been set
                return EDD_STS_ERR_PARAM;
            }

            //CfgPara.SRT.ConsumerCntClass12 is already checked!
            {
                LSA_INT32  free_buffer;

                //Check wether there is still a resource free
                free_buffer = EDDI_MemPoolGetFreeBuffer(pDDB->CRT.hPool.AcwConsumer);
                if (free_buffer < 0)
                {
                    return EDD_STS_ERR_RESOURCE;
                }
            }

            break;
        }

        case EDDI_LIST_TYPE_FCW:
        {
            // SYSRED providers are not allowed here!
            if (pConsParam->Properties & EDD_CSRT_CONS_PROP_SYSRED)
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerAddCheckRQB, SysRed is not allowed for RTC3 consumers; ConsumerFrameID:0x%X", pConsParam->FrameID);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
                return EDD_STS_ERR_PARAM;
            }
            
            #if defined (EDDI_CFG_REV5)
            if (pConsParam->Partial_DataLen)
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen not supported. ->:0x%X :0x%X",
                                      pConsParam->Partial_DataLen, EDD_CSRT_DATALEN_MIN);

                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                return EDD_STS_ERR_PARAM;
            }

            if (pConsParam->Partial_DataOffset)
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataOffset not supported. ->:0x%X :0x%X",
                                      pConsParam->Partial_DataOffset, 0);

                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataOffset);
                return EDD_STS_ERR_PARAM;
            }
            #else
            if (pConsParam->Partial_DataLen)
            {
                if (pConsParam->Partial_DataLen > pConsParam->DataLen)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen > pConsParam->DataLen. ->:0x%X :0x%X",
                                      pConsParam->DataLen, pConsParam->Partial_DataLen);

                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                    return EDD_STS_ERR_PARAM;
                }

                if  (  (EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_LO <= (pConsParam->Partial_DataOffset + pConsParam->Partial_DataLen))
                    && (EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_HI >= (pConsParam->Partial_DataOffset + pConsParam->Partial_DataLen))  )
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataLen (36..39). ->:%d",
                                      pConsParam->Partial_DataLen);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                    return EDD_STS_ERR_PARAM;
                }

                if (pConsParam->Partial_DataOffset >= pConsParam->DataLen)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->IOParams.Partial_DataOffset >= pConsParam->DataLen. ->:0x%X :0x%X",
                                      pConsParam->Partial_DataOffset, EDD_CSRT_DATALEN_MIN);

                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataOffset);
                    return EDD_STS_ERR_PARAM;
                }

                if (pConsParam->Partial_DataLen > (pConsParam->DataLen - pConsParam->Partial_DataOffset))
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTConsumerAddCheckRQB, pConsParam->Partial_DataLen >= (pConsParam->DataLen - pConsParam->IOParams.Partial_DataOffset). ->:0x%X :0x%X",
                                      pConsParam->Partial_DataOffset, EDD_CSRT_DATALEN_MIN);

                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_PartialDataLen);
                    return EDD_STS_ERR_PARAM;
                }
            }
            #endif

            if (pConsParam->DataLen > EDDI_IRT_DATALEN_MAX)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTConsumerAddCheckRQB, Parameter pConsParam->DataLen too big. pConsParam->DataLen, EDDI_IRT_DATALEN_MAX, ->:0x%X :0x%X",
                                  pConsParam->DataLen, EDDI_IRT_DATALEN_MAX );
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
                return EDD_STS_ERR_PARAM;
            }
        
            /* check CycleReductionRatio*/
            if (pConsParam->CycleReductionRatio != EDD_CYCLE_REDUCTION_RATIO_UNDEFINED)
            {
                if (EDD_STS_OK != EDDI_CRTCheckClass3Reduction(pDDB, pConsParam->CycleReductionRatio, pConsParam->CyclePhase, LSA_FALSE))
                {
                    //detail has already been set
                    return EDD_STS_ERR_PARAM;
                }
            }

            #if !defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
            if (!pDDB->CycCount.bCBFIsBinary)
            {
                if (   (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pConsParam->CycleReductionRatio)
                    && (1 != pConsParam->CycleReductionRatio) )
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTConsumerAddCheckRQB, CycleReductionRatio 0x%X != 1 for CBF %i",
                        pConsParam->CycleReductionRatio, pDDB->CycCount.Entity);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CycleReductionRatio_To_Long);
                    return EDD_STS_ERR_PARAM;
                }
            }
            #endif

            //No RTC3_legacy consumers allowed for small SCFs
            if (    (pDDB->CRT.CycleBaseFactor < EDDI_CRT_CYCLE_BASE_FACTOR_CRIT)
                &&  (pConsParam->Properties & EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY) )
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDI_CRTConsumerAddCheckRQB, ERROR: No RTC3_Legacy consumers allowed for CBF %d",
                                  pDDB->CRT.CycleBaseFactor);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
                return EDD_STS_ERR_PARAM;
            }
        
            break;
        }

        default:
        {
            EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerAddCheckRQB, Invalid List-Type. ->:0x%X",
                              ListType);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
            return EDD_STS_ERR_PARAM;
        }
    }
                     
    // Check, if FrameId is already in use with the same ListType
    EDDI_CRTConsumerListFindByFrameId(pDDB, &pDDB->CRT.ConsumerList, &pDDB->CRT, pConsParam, ListType, &pFindConsumer, EDDI_IS_DFP_CONS(pConsParam)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);
    if (!(pFindConsumer == EDDI_NULL_PTR))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerAddCheckRQB, FrameID already used; ConsumerFrameID, pFindConsumer->ConsumerId, ->:0x%X :0x%X",
                          pConsParam->FrameID, pFindConsumer->ConsumerId);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_FrameID_used);
        return EDD_STS_ERR_PARAM;
    }

    if (ConsumerType == EDDI_RTC3_AUX_CONSUMER)
    {
        //the RTC3-AUX-Consumer uses it's own data-buffer (4 bytes)!
        return EDD_STS_OK;
    }
    else //ConsumerType != EDDI_RTC3_AUX_CONSUMER
    {
        LSA_RESULT  Status;

        if (   (EDD_DATAOFFSET_UNDEFINED != pConsParam->IOParams.DataOffset)
            && !EDDI_IS_DFP_CONS(pConsParam))
        {
            //DFP-overlaps are only checked when adding/activating the packframe
            KramDatalen = (pConsParam->Partial_DataLen)?pConsParam->Partial_DataLen:pConsParam->DataLen;
            Status = EDDI_CRTCheckOverlaps(pDDB, pConsParam->IOParams.DataOffset, KramDatalen, ListType, LSA_FALSE, ((LSA_UINT32)EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC == (pConsParam->IOParams.BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK)));
            if (Status != EDD_STS_OK)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_OverLap);
            }
            
            return Status;
        }

        return EDD_STS_OK;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTProviderAddCheckRQB()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT8                const  ListType,
                                                              LSA_UINT8                const  ProviderType )
{
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "CRTProviderADD_check_RQB");

    #if defined (EDDI_CFG_REV7)
    if (     (EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (pProvParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
         &&  (EDD_CSRT_PROV_PROP_RTCLASS_3 == (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK))
         #if defined (EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
         &&  (pProvParam->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY)
         #endif //(EDDI_INT_CFG_ENABLE_SYNC_IMG_RTC3)
       )
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderAddCheckRQB: Sync Image and Legacy Mode and RTC3 is on SOC1/2 not allowed");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Provider_Properties);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    //Check DataOffset
    if( EDD_DATAOFFSET_INVALID == pProvParam->IOParams.DataOffset )
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderAddCheckRQB: EDD_DATAOFFSET_INVALID");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }
    
    if (   (EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID == (pProvParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK))
        || (!EDDI_CRTCheckBufferProperties(pDDB, ListType, pProvParam->IOParams.BufferProperties, LSA_TRUE /*bIsProvider*/, LSA_TRUE /*bAllowUndefined*/)) )
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderAddCheckRQB: illegal Bufferproperties:0x%X",
            pProvParam->IOParams.BufferProperties);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_IRT_XRTProperties);
        return EDD_STS_ERR_PARAM;
    }

    //Check GroupID
    if (pProvParam->GroupID >= EDD_CFG_CSRT_MAX_PROVIDER_GROUP)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "CRTProviderADD_check_RQB, GroupID too big; pParam->GroupID, CfgPara.ProviderGroupCnt , ->:0x%X :0x%X",
                          pProvParam->GroupID, EDD_CFG_CSRT_MAX_PROVIDER_GROUP);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_GroupID);
        return EDD_STS_ERR_PARAM;
    }

    if (EDDI_IS_DFP_PROV(pProvParam))
    {
        LSA_RESULT  const Result = EDDI_DFPProvAddCheckRQB(pRQB, pDDB, ListType, ProviderType);
        if (EDD_STS_OK != Result)
        {
            //ErrorTrace is contained in fct above!
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DFPParams);
            return (Result);
        }
    }
    else
    {
        //check List-Type-Dependencies
        switch (ListType)
        {
            case EDDI_LIST_TYPE_ACW:
            {
                LSA_UINT32  const  errorcode = EDDI_CRTProviderCheckDataLenClass12(pDDB, pProvParam->DataLen, pProvParam->CycleReductionRatio, pProvParam->CyclePhase, ProviderType);
            
                if (0 != errorcode)
                {
                    EDDI_SET_DETAIL_ERR(pDDB, errorcode);
                    return EDD_STS_ERR_PARAM;
                }

                // Check for SYSRED providers
                #ifndef EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1
                if (pProvParam->Properties & EDD_CSRT_PROV_PROP_SYSRED)
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTProviderAddCheckRQB, SysRed is not allowed for RTC1 providers; ProviderFrameID:0x%X", pProvParam->FrameID);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
                    return EDD_STS_ERR_PARAM;
                }
                #endif //(EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1)
                
                #if defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12)
                if (EDD_STS_OK != EDDI_CRTCheckClass12Reduction(pDDB, pDDB->pLocal_CRT->SrtRedTree.MaxReduction, pProvParam->CycleReductionRatio, pProvParam->CyclePhase, LSA_TRUE /*bUndefAllowed*/))
                #else
                if (EDD_STS_OK != EDDI_CRTCheckClass12Reduction(pDDB, (pDDB->CycCount.bCBFIsBinary?pDDB->pLocal_CRT->SrtRedTree.MaxReduction:1), pProvParam->CycleReductionRatio, pProvParam->CyclePhase, LSA_TRUE /*bUndefAllowed*/))
                #endif
                {
                    //detail has already been set
                    return EDD_STS_ERR_PARAM;
                }

                break;
            }

            case EDDI_LIST_TYPE_FCW:
            {
                EDDI_CRT_PROVIDER_PTR_TYPE  pFindProvider;

                // SYSRED providers are not allowed here!
                if (pProvParam->Properties & EDD_CSRT_PROV_PROP_SYSRED)
                {
                    EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTProviderAddCheckRQB, SysRed is not allowed for RTC3 providers; ProviderFrameID:0x%X", pProvParam->FrameID);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_EDDProperties);
                    return EDD_STS_ERR_PARAM;
                }
                
                // Check, if FrameId is already in use with the same ListType
                EDDI_CRTProviderListFindByFrameId(pDDB, &pDDB->CRT.ProviderList, &pDDB->CRT, pProvParam, ListType, &pFindProvider, EDDI_IS_DFP_PROV(pProvParam)?LSA_TRUE:LSA_FALSE /*bIsDFP*/);
                if (!(pFindProvider == EDDI_NULL_PTR))
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTProviderAddCheckRQB, FrameID already used; ProviderFrameID, pFindProvider->ProviderId, ->:0x%X :0x%X",
                                      pProvParam->FrameID, pFindProvider->ProviderId);
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_FrameID_used);
                    return EDD_STS_ERR_PARAM;
                }

                if (pProvParam->DataLen > EDDI_IRT_DATALEN_MAX)
                {
                    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_CRTProviderAddCheckRQB, Parameter pProvParam->DataLen too big; DataLen, EDDI_IRT_DATALEN_MAX, ->:0x%X :0x%X",
                                      pProvParam->DataLen, EDDI_IRT_DATALEN_MAX );
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataLen);
                    return(EDD_STS_ERR_PARAM);
                }
                
                /* check CycleReductionRatio*/
                if (pProvParam->CycleReductionRatio != EDD_CYCLE_REDUCTION_RATIO_UNDEFINED)
                {
                    if (EDD_STS_OK != EDDI_CRTCheckClass3Reduction(pDDB, pProvParam->CycleReductionRatio, pProvParam->CyclePhase, LSA_FALSE))
                    {
                        //detail has already been set
                        return EDD_STS_ERR_PARAM;
                    }
                }

                #if !defined (EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3)
                if (!pDDB->CycCount.bCBFIsBinary)
                {
                    if (   (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pProvParam->CycleReductionRatio)
                        && (1 != pProvParam->CycleReductionRatio) )
                    {
                        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_CRTProviderAddCheckRQB, CycleReductionRatio 0x%X != 1 for CBF %i",
                            pProvParam->CycleReductionRatio, pDDB->CycCount.Entity);
                        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CycleReductionRatio_To_Long);
                        return EDD_STS_ERR_PARAM;
                    }
                }
                #endif
                break;
            }

            default:
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTProviderAddCheckRQB, Invalid List-Type. ->:0x%X",
                                  ListType);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ListType);
                return EDD_STS_ERR_PARAM;
            }
        }

        if (ProviderType != EDDI_RTC3_AUX_PROVIDER)
        {
            if (EDD_DATAOFFSET_UNDEFINED != pProvParam->IOParams.DataOffset)
            {
                LSA_RESULT  Status;

                Status = EDDI_CRTCheckOverlaps(pDDB, pProvParam->IOParams.DataOffset, pProvParam->DataLen, ListType, LSA_TRUE, ((LSA_UINT32)EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC == (pProvParam->IOParams.BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK)));
                if (Status != EDD_STS_OK)
                {
                    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_OverLap);
                }
        
                return Status;
            }        
        }
    }
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderXchangeCheckRQB( EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE  const  pParam,
                                                                  EDDI_CONST_CRT_PROVIDER_PTR_TYPE              const  pProvider,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                       const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderXchangeCheckRQB");

    if (!pProvider->bBufferParamsValid)
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderXchangeCheckRQB, Consistency-Check failed; no valid buffer params, ProvId:0x%X, BufferOffset:0x%X, BufferProperties:0x%X",
                          pProvider->ProviderId, pProvider->LowerParams.DataOffset, pProvider->LowerParams.BufferProperties);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    switch (pProvider->LowerParams.ProviderType)
    {
        case EDDI_RTC1_PROVIDER:
        case EDDI_RTC2_PROVIDER:
        {
            break;
        }
        default:
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTProviderXchangeCheckRQB, invalid ProviderType; ProviderID, ProviderType, ->:0x%X :0x%X",
                              pParam->ProviderID, pProvider->LowerParams.ProviderType);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ProviderType);
            return EDD_STS_ERR_PARAM;
        }
    }

    #if defined (EDDI_CFG_ERTEC_400)
    // Check, if Provider is already changing Buffer
    if (pProvider->IsXChangingBuffer)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderXchangeCheckRQB, Provider is already xchanging Buffer; Param->ProviderID, -, ->:0x%X",
                          pParam->ProviderID );
        return EDD_STS_ERR_SEQUENCE;
    }

    if (   (pProvider->LowerParams.DataOffset != pParam->OldOffset)
        || (pProvider->LowerParams.DataLen    != pParam->DataLen))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderXchangeCheckRQB, Consistency-Check failed; pParam->OldOffset, pParam->DataLen, ->:0x%X :0x%X",
                          pParam->OldOffset, pParam->DataLen );
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    // Check Overlaps with other Providersbuffers
    if (EDD_STS_OK != EDDI_CRTCheckOverlaps(pDDB, pParam->NewOffset, (LSA_UINT16)pParam->DataLen, pProvider->LowerParams.ListType, LSA_TRUE, LSA_FALSE /*irrelevant here*/))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderXchangeCheckRQB, New Buffer has overlaps with other ProviderBuffers; pParam->NewOffset, pParam->DataLen, ->:0x%X :0x%X",
                          pParam->NewOffset, pParam->DataLen );
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_OverLap);
        return EDD_STS_ERR_PARAM;
    }
    #else //EDDI_CFG_REV7
    if (pProvider->LowerParams.DataOffset != pParam->OldOffset)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderXchangeCheckRQB, Consistency-Check failed; pParam->OldOffset ->:0x%X",
                          pParam->OldOffset);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    if (0 == pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderXchangeCheckRQB, Only useable with PAEA/IOC enabled.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_ONLY_SUPPORTED_BY_PAEA);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    return EDD_STS_OK;
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerXchangeCheckRQB( EDD_UPPER_CSRT_CONSUMER_XCHANGE_BUF_PTR_TYPE  const  pParam,
                                                                  EDDI_CONST_CRT_CONSUMER_PTR_TYPE              const  pConsumer,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                       const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerXchangeCheckRQB");

    switch (pConsumer->LowerParams.ConsumerType)
    {
        case EDDI_RTC1_CONSUMER:
        case EDDI_RTC2_CONSUMER:
        {
            break;
        }
        default:
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerXchangeCheckRQB, invalid ConsumerType; ConsumerID, ConsumerType, ->:0x%X :0x%X",
                              pParam->ConsumerID, pConsumer->LowerParams.ConsumerType);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_ConsumerType);
            return EDD_STS_ERR_PARAM;
        }
    }

    #if defined (EDDI_CFG_REV7)
    if (!pConsumer->usingPAEA_Ram)
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerXchangeCheckRQB: Only implemented in combination with PAEA HW-support.");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_CRT_ONLY_SUPPORTED_BY_PAEA);
        return EDD_STS_ERR_PARAM;
    }

    //No overlap check due to the fact that we only have APDU in KRAM

    LSA_UNUSED_ARG(pParam); //satisfy lint!
    #else
    if (   (pConsumer->LowerParams.DataOffset != pParam->OldOffset)
        || (pConsumer->LowerParams.DataLen    != pParam->DataLen))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerXchangeCheckRQB, Consistency-Check failed; pParam->OldOffset, pParam->DataLen, ->:0x%X :0x%X",
                          pParam->OldOffset, pParam->DataLen);
        return EDD_STS_ERR_PARAM;
    }

    {
    LSA_UINT16  const KramDatalen = (pConsumer->LowerParams.Partial_DataLen)?pConsumer->LowerParams.Partial_DataLen:pConsumer->LowerParams.DataLen;
    
        // Check Overlaps with other Consumer-buffers
        if (EDD_STS_OK != EDDI_CRTCheckOverlaps(pDDB, pParam->NewOffset, KramDatalen, pConsumer->LowerParams.ListType, LSA_FALSE, LSA_FALSE /*irrelevant here*/))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTConsumerXchangeCheckRQB, Buffer has overlaps with other ConsumerBuffers; NewOffset, DataLen ->:0x%X :0x%X",
                              pParam->NewOffset, pParam->DataLen);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_OverLap);
            return EDD_STS_ERR_PARAM;
        }
    }
    #endif

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


#if !defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)
/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTCheckOverlaps()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckOverlaps( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  DataOffset,
                                                        LSA_UINT16               const  DataLen,
                                                        LSA_UINT8                const  ListType,
                                                        LSA_BOOL                 const  bProvider,
                                                        LSA_BOOL                 const  bSync)
{
    LSA_UINT32         i, len;
    LSA_UINT8       *  p;
    LSA_UINT8   const  Id = (LSA_UINT8)CRT_MIRROR_REMOVE;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCheckOverlaps");

    //check Data-Offset-Alignment
    //do nothing if Dataoffset is (still) undefined
    if (EDD_DATAOFFSET_UNDEFINED == DataOffset)
    {
        return EDD_STS_OK;
    }
    
    #if defined (EDDI_CFG_REV7)
    if (   pDDB->CRT.PAEA_Para.PAEA_BaseAdr
        && !bSync)
    {
        // don't care! When using PAEA-RAM and no SYNC image is used for the frame, the user is responsible for
        // checking boundaries and overlaps!
        // --> No additional bookkeeping with KRAMMirror
        return EDD_STS_OK;
    }
    #else
    LSA_UNUSED_ARG(bSync);
    #endif

    if (DataOffset & 0x3)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckOverlaps, check 4 Byte Alignment, ->:0x%X :0x%X", DataLen, DataOffset);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Alignment);
        return EDD_STS_ERR_PARAM;
    }

    len = DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    len = (len + 0x03UL) & (~0x03UL);

    #if !defined (EDDI_INTCFG_PROV_BUFFER_IF_SINGLE)
    if (ListType == EDDI_LIST_TYPE_ACW)
    {
        if (bProvider)
        {
            //SW-3-Buffer-Interface !!
            len *= 3;
        }
    }
    #else
    LSA_UNUSED_ARG(ListType); //satisfy lint!
    LSA_UNUSED_ARG(bProvider); //satisfy lint!
    #endif

    //check Process-Image-Overflow
    if ((DataOffset + len) > pDDB->ProcessImage.offset_ProcessImageEnd)
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTCheckOverlaps, DataOffset + len > offset_ProcessImageEnd, needed len:0x%X DataOffset:0x%X", len, DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    len = len / 4;

    p = pDDB->ProcessImage.pKRAMMirror + (DataOffset / 4);

    for (i = 0; i < len; i++)
    {
        if (*p != Id)
        {
            EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTCheckOverlaps, Overlap found  , -> DataLen   :0x%X  needed len:0x%X DataOffset:0x%X  OverlapOffset:0x%X", DataLen, len * 4UL, DataOffset, DataOffset + (i * 4UL));
            return EDD_STS_ERR_PARAM;
        }

        p++;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTCheckAddRemoveMirror()                   */
/*                                                                         */
/* D e s c r i p t i o n: This function contains no check!                 */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckAddRemoveMirror( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT16               const  DataLen,
                                                             LSA_UINT32               const  DataOffset,
                                                             LSA_UINT8                const  ListType,
                                                             LSA_BOOL                 const  bProvider,
                                                             CRT_MIRROR_TYPE          const  Action )
{
    LSA_UINT32         i, len;
    LSA_UINT8       *  p;
    LSA_UINT8   const  Id = (LSA_UINT8)Action;

    EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTCheckAddRemoveMirror, > Action:0x%X for ListType 0x%X, DataOffset 0x%X, DataLen 0x%X", 
                      Action, ListType, DataOffset, DataLen);

    //Data-Offset-Alignment is already checked in EDDI_CRTCheckOverlaps()!
    
    // Do nothing if Dataoffset is undefind
    if (EDD_DATAOFFSET_UNDEFINED == DataOffset)
    {
        return;
    }

    len = DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    len = (len + 0x03UL) & (~0x03UL);

    #if !defined (EDDI_INTCFG_PROV_BUFFER_IF_SINGLE)
    if (ListType == EDDI_LIST_TYPE_ACW)
    {
        if (bProvider)
        {
            //SW-3-Buffer-Interface !!
            len *= 3;
        }
    }
    #else
    LSA_UNUSED_ARG(ListType); //satisfy lint!
    LSA_UNUSED_ARG(bProvider); //satisfy lint!
    #endif

    //check Process-Image-Overflow again (already checked in EDDI_CRTCheckOverlaps())!
    if ((DataOffset + len) > pDDB->ProcessImage.offset_ProcessImageEnd)
    {
        EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CRTCheckAddRemoveMirror, DataOffset(0x%X)+len(0x%X) > offset_ProcessImageEnd(0x%X)",
                          DataOffset, len, pDDB->ProcessImage.offset_ProcessImageEnd);
        EDDI_Excp("EDDI_CRTCheckAddRemoveMirror, DataOffset + len > offset_ProcessImageEnd", EDDI_FATAL_ERR_EXCP, len, DataOffset);
        return;
    }

    len = len / 4;

    p = pDDB->ProcessImage.pKRAMMirror + (DataOffset / 4);

    for (i = 0; i < len; i++)
    {
        *p = Id;
        p++;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#elif defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckOverlaps( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  DataOffset,
                                                                LSA_UINT16               const  DataLen,
                                                                LSA_UINT8                const  ListType,
                                                                LSA_BOOL                 const  bProvider,
                                                                LSA_BOOL                 const  bSync)
{
    if (pDDB || DataOffset || DataLen || ListType || bProvider || bSync)
    {
    }

    return EDD_STS_OK;
}

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckAddRemoveMirror( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT16               const  DataLen,
                                                             LSA_UINT32               const  DataOffset,
                                                             LSA_UINT8                const  ListType,
                                                             LSA_BOOL                 const  bProvider,
                                                             CRT_MIRROR_TYPE          const  Action )
{
    if (pDDB || DataLen || DataOffset || ListType || bProvider || Action)
    {
    }
}
#endif //EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK


/******************************************************************************
 *  Function:    EDDI_CRTCheckClass12Reduction()  //HelperFunction only
 *
 *  Description: Checks CycleReductionRatio and CyclePhase are within their ranges.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if Data has no Overlapping
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckClass12Reduction( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  MaxAllowedReduction,
                                                                LSA_UINT16               const  CycleReductionRatio,
                                                                LSA_UINT16               const  CyclePhase,
                                                                LSA_BOOL                 const  bUndefAllowed)
{
    //Check Params
    if (   !bUndefAllowed 
        || (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != CycleReductionRatio) )
    {
        if (!EDDI_RedIsBinaryValue((LSA_UINT32)CycleReductionRatio))
        {
            EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTCheckClass12Reduction, EDDI_RedIsBinaryValue, ->:0x%X :0x%X ",
                              CycleReductionRatio,  CyclePhase);
            EDDI_SET_DETAIL_ERR( pDDB, EDDI_ERR_CycleReductionRatio_Binary);
            return EDD_STS_ERR_PARAM;
        }

        if (CycleReductionRatio > MaxAllowedReduction)
        {
            EDDI_CRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_CRTCheckClass12Reduction, CycleReductionRatio (0x%X) > MaxAllowedReduction (0x%X), CyclePhase: 0x%X ",
                              CycleReductionRatio, MaxAllowedReduction, CyclePhase);
            EDDI_SET_DETAIL_ERR( pDDB, EDDI_ERR_CycleReductionRatio_To_Long);
            return EDD_STS_ERR_PARAM;
        }

        if (   !bUndefAllowed 
            || (EDD_CYCLE_PHASE_UNDEFINED != CyclePhase) )
        {
            if (CyclePhase > CycleReductionRatio)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTCheckClass12Reduction, CyclePhase > CycleReductionRatio, ->:0x%X :0x%X ",
                                  CycleReductionRatio,  CyclePhase);
                EDDI_SET_DETAIL_ERR( pDDB, EDDI_ERR_CyclePhase_To_Long);
                return EDD_STS_ERR_PARAM;
            }

            if (0 == CyclePhase)
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_CRTCheckClass12Reduction, 0 == CyclePhase, ->:0x%X :0x%X ",
                                  CycleReductionRatio,  CyclePhase);
                EDDI_SET_DETAIL_ERR( pDDB, EDDI_ERR_CyclePhase_NULL);
                return EDD_STS_ERR_PARAM;
            }
        }
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTCheckClass12Bandwidth()  //HelperFunction only
 *
 *  Description: Checks if there is enough Bandwidth in the specified CyclePhase
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if Data has no Overlapping
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckClass12Bandwidth( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT,
                                                                LSA_UINT16                        const  CycleReductionRatio,
                                                                LSA_UINT16                        const  CyclePhase,
                                                                LSA_UINT32                        const  ProvDataLen )
{
    LSA_UINT32         resMaxByteCounter = 0;
    LSA_UINT32         resMaxProvCounter = 0;
    LSA_UINT32         MaxAllowedByteCnt;
    LSA_UINT32         MaxAllowedProvCnt;
    LSA_RESULT         const Result = EDDI_GetMaxProviderParams(pDDB, &MaxAllowedProvCnt, &MaxAllowedByteCnt);

    if (Result != EDD_STS_OK)
    {
        return (Result);  //errortrace was made in called function!
    }

    if (   ((MaxAllowedByteCnt == 0) && (MaxAllowedProvCnt == 0))
        || (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == CycleReductionRatio)
        || (EDD_CYCLE_PHASE_UNDEFINED == CyclePhase) )
    {
        //Check is not necessary (yet)
        return EDD_STS_OK;
    }

    EDDI_RedTreeGetMaxPathLoad(&pCRT->SrtRedTree,
                               CycleReductionRatio,
                               CyclePhase,
                               &resMaxByteCounter,
                               &resMaxProvCounter);

    // Only do check, if MaxAllowedByteCnt != 0
    if ((MaxAllowedByteCnt) && ((resMaxByteCounter + ProvDataLen) > MaxAllowedByteCnt))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTCheckClass12Bandwidth, Check Bandwidth failed, too many Bytes; resMaxByteCounter, MaxAllowedByteCnt, ->:0x%X :0x%X",
                          resMaxByteCounter, MaxAllowedByteCnt);
        return EDD_STS_ERR_PARAM;
    }

    // Only do check, if MaxAllowedProvCnt != 0
    if ((MaxAllowedProvCnt) && ((resMaxProvCounter + 1UL) > MaxAllowedProvCnt))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTCheckClass12Bandwidth, Check Bandwidth failed, too many Providers; resMaxProvCounter, MaxAllowedProvCnt, ->:0x%X :0x%X",
                          resMaxProvCounter, MaxAllowedProvCnt);
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;

}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:   EDDI_CRTCheckClass3Reduction()  //HelperFunction only
 *
 *  Description:Checks CycleReductionRatio and CyclePhase are within their ranges.
 *
 *  Arguments:  pCRT (THIS)          :
 *              CycleReductionRatio  :
 *              CyclePhase           :
 *
 *  Return:     EDD_STS_OK if Data has no Overlapping
 *              EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckClass3Reduction( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT16               const  CycleReductionRatio,
                                                               LSA_UINT16               const  CyclePhase,
                                                               LSA_BOOL                 const  bCheckPhase )
{
    LSA_UINT32  const  IndexFirstTree = EDDI_PmGetHwPortIndex(pDDB, 0);
    LSA_UINT32  const  MaxReduction   = pDDB->CRT.IRTRedAx.IrtRedTreeTx[IndexFirstTree].MaxReduction;

    /* MaxReduction is set on EDDI_RedTreeInit (Tx/Rx) in EDDI_SYNCInitIRT */
    if (   (CycleReductionRatio > MaxReduction)
        || (   (CyclePhase      > CycleReductionRatio)
            && (bCheckPhase)))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_Phase);
        return EDD_STS_ERR_PARAM;
    }

    if (!EDDI_RedIsBinaryValue((LSA_UINT32)CycleReductionRatio))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTCheckClass3Reduction, EDDI_RedIsBinaryValue, ->:0x%X :0x%X ",
                          CycleReductionRatio,  CyclePhase);
        EDDI_SET_DETAIL_ERR( pDDB, EDDI_ERR_CycleReductionRatio_Binary);
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;
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
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CRTCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                              LSA_UINT8                 const  ListType,
                                                              LSA_UINT32                const  BufferProperties,
                                                              LSA_BOOL                  const  bIsProvider,
                                                              LSA_BOOL                  const  bAllowUndefined)
{
    LSA_UINT32  const  BufferProp   = (bIsProvider)?(BufferProperties & EDD_PROV_BUFFER_PROP_IRTE_MASK):(BufferProperties & EDD_CONS_BUFFER_PROP_IRTE_MASK); //no more properties to check until now
    LSA_UINT32  const  PropUndef    = (bIsProvider)?EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED:EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED;
    LSA_UINT32  const  PropAsync    = (bIsProvider)?EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC:EDD_CONS_BUFFER_PROP_IRTE_IMG_ASYNC;
    LSA_UINT32  const  PropSync     = (bIsProvider)?EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC:EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC;

    if (   bAllowUndefined
        && (PropUndef == BufferProp))
    {
        return LSA_TRUE;
    }

    switch (ListType)
    {
        case EDDI_LIST_TYPE_ACW:
        {
            //only asynchronous image supported
            if (PropAsync != BufferProp)
            {
                return LSA_FALSE;
            }
        }
        break;

        case EDDI_LIST_TYPE_FCW:
        {
            //both imagetypes supported
            if (   (PropAsync == BufferProp)
                || (   (PropSync == BufferProp)
                    && (pDDB->ProcessImage.offset_ProcessImageEnd != 0) ) )
            {
            }
            else
            {
                EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTCheckEddProperties, illegal bufferproperties(0x%X) or SYNC and offset_ProcessImageEnd(0x%X)=0",
                    BufferProp, pDDB->ProcessImage.offset_ProcessImageEnd);
                return LSA_FALSE;
            }
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_CRTCheckEddProperties - Invalid ListType; EDDProperties ", EDDI_FATAL_ERR_EXCP, ListType, BufferProperties);
            return LSA_FALSE;
        }
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_crt_check.c                                             */
/*****************************************************************************/
