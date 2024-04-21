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
/*  F i l e               &F: eddi_oc.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.02.04    ZR    add icc modul                                          */
/*  30.04.04    ZA    EDD_OPC_REQUEST_SRT_BUFFER hinzugefügt                 */
/*  16.07.07    JS    parametercheck. SYNC channel needs NRT-Channel with    */
/*                    channel B IF 0.                                        */
/*  18.11.08    AH    clear pHDB->StatisticShadow in EDDI_OpenChIntern(..)   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_ext.h"
#include "eddi_nrt_oc.h"
#include "eddi_swi_ext.h"
#include "eddi_crt_ext.h"
//#include "eddi_profile.h"
#include "eddi_sync_usr.h"
#include "eddi_time.h"
#include "eddi_prm_req.h"
#include "eddi_lock.h"

#if defined (EDDI_CFG_REV5)
#include "eddi_nrt_ts.h"
#endif

#define EDDI_MODULE_ID     M_ID_EDDI_OC
#define LTRC_ACT_MODUL_ID  21

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_OpenChIntern( EDD_UPPER_RQB_PTR_TYPE            const pRQB,
                                                         EDD_UPPER_OPEN_CHANNEL_PTR_TYPE * const ppRQBChannel,
                                                         EDDI_LOCAL_HDB_PTR_TYPE         * const ppHDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CloseChannelFinishOk( EDD_UPPER_RQB_PTR_TYPE const pRQB );


/***************************************************************************/
/* F u n c t i o n:       eddi_open_channel()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_open_channel( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    LSA_RESULT                       Status;
    EDD_UPPER_OPEN_CHANNEL_PTR_TYPE  pRQBChannel = 0;
    EDDI_LOCAL_HDB_PTR_TYPE          pHDB        = (EDDI_LOCAL_HDB_PTR_TYPE)LSA_NULL;

    EDDI_ENTER_COM_S();

    Status = EDDI_OpenChIntern(pRQB, &pRQBChannel, &pHDB);

    if (Status != EDD_STS_OK)
    {
        EDDI_RQB_ERROR(pRQB);
    }

    if (pHDB)
    {
        EDDI_UPPER_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_open_channel->");
    }

    EDD_RQB_SET_RESPONSE(pRQB, Status);

    if (pRQBChannel)
    {
        EDD_RQB_SET_HANDLE(pRQB, pRQBChannel->HandleUpper);
    }

    if (pRQBChannel && (!(0 == pRQBChannel->Cbf)))
    {
        if (   (EDD_STS_OK == Status)
            && (pHDB))
        {
            EDDI_REQUEST_UPPER_DONE(pRQBChannel->Cbf /*pHDB->Cbf*/, pRQB, pHDB->pSys); //pRQBChannel->Cbf(pRQB);
        }
        else
        {
            LSA_UINT32           Ctr;
            LSA_SYS_PTR_TYPE     LSASysPtrLocal;
            LSA_UINT8         *  p = (LSA_UINT8 *)(LSA_VOID *)&LSASysPtrLocal;

            //preset local SysPtr
            for (Ctr=0; Ctr<sizeof(LSA_SYS_PTR_TYPE); Ctr++)
            {
                *p++=0;
            }
            EDDI_REQUEST_UPPER_DONE(pRQBChannel->Cbf /*pHDB->Cbf*/, pRQB, LSASysPtrLocal); //pRQBChannel->Cbf(pRQB);
        }
    }
    else
    {
        EDDI_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_open_channel (pRQB=0x%X), NO channel (0x%X) OR NO CBF given!!",
                              pRQB, pRQBChannel);
    }

    EDDI_EXIT_COM_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_OpenChIntern()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_OpenChIntern( EDD_UPPER_RQB_PTR_TYPE              const  pRQB,
                                                            EDD_UPPER_OPEN_CHANNEL_PTR_TYPE  *  const  ppRQBChannel,
                                                            EDDI_LOCAL_HDB_PTR_TYPE          *  const  ppHDB )
{
    LSA_RESULT               Status, Status2;
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB;
    LSA_SYS_PTR_TYPE         pSys;
    EDDI_DETAIL_PTR_TYPE     pDetail;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;
    LSA_RESULT               PathStat;
    LSA_HANDLE_TYPE          Handle;
    LSA_UINT                 UsrPortIndex;
    LSA_UINT32               CheckAdr;

    *ppRQBChannel = (EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)pRQB->pParam;

    if (EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_OPEN_CHANNEL)
    {
        return EDD_STS_ERR_OPCODE;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        return EDD_STS_ERR_PARAM;
    }

    if ( 0 == (*ppRQBChannel)->Cbf )
    {
        return EDD_STS_ERR_PARAM;
    }

    /*-----------------------------------------------------------------------*/
    /* Call LSA Output-macro..                                               */
    /*-----------------------------------------------------------------------*/
    EDDI_GET_PATH_INFO(&PathStat, &pSys, &pDetail, (*ppRQBChannel)->SysPath);

    if (PathStat != LSA_RET_OK)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_OpenChIntern, EDDI_GET_PATH_INFO() failed, RetVal:0x%X", PathStat);
        EDDI_Excp("EDDI_OpenChIntern, EDDI_GET_PATH_INFO() failed, RetVal:", EDDI_FATAL_ERR_EXCP, PathStat, 0);
        return EDD_STS_ERR_SYS_PATH;
    }

    /*-----------------------------------------------------------------------*/
    /* Do some parameter checks.                                             */
    /*-----------------------------------------------------------------------*/
    if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
    {
        return EDD_STS_ERR_PARAM;
    }

    if (   (   (EDD_CDB_INSERT_SRC_MAC_DISABLED != pDetail->InsertSrcMAC)
            && (EDD_CDB_INSERT_SRC_MAC_ENABLED != pDetail->InsertSrcMAC) )
        || (   (EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD != pDetail->Prm_PortDataAdjustLesserCheckQuality)
            && (EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY != pDetail->Prm_PortDataAdjustLesserCheckQuality) )
        || (   (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UseNRT)
            && (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UseNRT) )
        || (   (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UsePRM)
            && (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UsePRM) )
        || (   (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UseSWITCH)
            && (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UseSWITCH) )
        || (   (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UseSYNC)
            && (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UseSYNC) )
        || (   (EDD_CDB_CHANNEL_USE_OFF != pDetail->UsedComp.UseXRT)
            && (EDD_CDB_CHANNEL_USE_ON != pDetail->UsedComp.UseXRT) )
        )
    {
        EDDI_PROGRAM_TRACE_07(0, LSA_TRACE_LEVEL_FATAL, "EDDI_OpenChIntern, Errors in DetailPtr: InsertSrcMAC (%d), Prm_PortDataAdjustLesserCheckQuality (%d), UseNRT (%d), UsePRM (%d), UseSWITCH (%d), UseSYNC (%d), UseXRT (%d),", 
            pDetail->InsertSrcMAC, pDetail->Prm_PortDataAdjustLesserCheckQuality, pDetail->UsedComp.UseNRT, pDetail->UsedComp.UsePRM, pDetail->UsedComp.UseSWITCH, pDetail->UsedComp.UseSYNC, pDetail->UsedComp.UseXRT);
        EDDI_Excp("EDDI_OpenChIntern, EDDI_GET_PATH_INFO() failed, RetVal:", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_PARAM;
    }
        
    if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseNRT)
    {
        if (!pDetail->pNRT)
        {
            EDDI_Excp("EDDI_OpenChIntern, pDetail->pNRT is NULL", EDDI_FATAL_ERR_EXCP, 0, 0);
            return EDD_STS_ERR_PARAM;
        }
        else
        {
            if (   (EDD_FEATURE_DISABLE != pDetail->pNRT->UseFrameTriggerUnit)
                && (EDD_FEATURE_ENABLE != pDetail->pNRT->UseFrameTriggerUnit)
                )
            {
                EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_OpenChIntern, Errors in DetailPtr: UseFrameTriggerUnit (%d)",
                    pDetail->pNRT->UseFrameTriggerUnit);
                EDDI_Excp("EDDI_OpenChIntern, EDDI_GET_PATH_INFO() failed", EDDI_FATAL_ERR_EXCP, 0, 0);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Get pointer to DDB.                                                   */
    /*-----------------------------------------------------------------------*/
    Status = EDDI_GetDDB(pDetail->hDDB, &pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "EDDI_OpenChIntern, GetDDB failed, Status:0x%X", Status);
        EDDI_Excp("EDDI_OpenChIntern", EDDI_FATAL_ERR_EXCP, Status, 0);
        return EDD_STS_ERR_PARAM;
    }

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_OpenChIntern->");

    /* check if HW is initialized */
    if (!pDDB->Glob.HWIsSetup)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_OpenChIntern, pDDB->Glob.HWIsSetup");
        EDDI_Excp("EDDI_OpenChIntern, pDDB->Glob.HWIsSetup", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    /* Check for proper IRTE-ptr */
    CheckAdr = EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(pDDB->hSysDev, 0, pDDB->ERTEC_Version.Location);
    if (pDDB->IRTE_SWI_BaseAdr != CheckAdr)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_OpenChIntern, IRTE_SWI_BaseAdr(0x%X) <> KRAM_ADR_ASIC_TO_LOCAL(0x%X)", 
                              pDDB->IRTE_SWI_BaseAdr, CheckAdr);
        EDDI_Excp("EDDI_OpenChIntern, IRTE_SWI_BaseAdr", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    /*-----------------------------------------------------------------------*/
    /* Get a Handle. On error abort.                                         */
    /*-----------------------------------------------------------------------*/
    Status = EDDI_HandleNew(&Handle, &pHDB, pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_RELEASE_PATH_INFO(&Status2, pSys, pDetail);
        return Status;
    }

    *ppHDB = pHDB;
    pHDB->pDetail = pDetail;

    //init HDB (Handle-Description-Block)
    //pHDB->handle is initialized during eddi_init and will never be changed!
    pHDB->InUse       = 1;
    pHDB->UsedComp    = 0;
    pHDB->SysPath     = (*ppRQBChannel)->SysPath;
    pHDB->UpperHandle = (*ppRQBChannel)->HandleUpper;
    pHDB->Cbf         = (*ppRQBChannel)->Cbf;
    pHDB->pDDB        = EDDI_NULL_PTR;
    pHDB->pSys        = pSys;

    /* default */
    pHDB->pIF = EDDI_NULL_PTR;
    EDDI_InitQueue(pDDB, &pHDB->AutoLinkIndReq);
    
    for (UsrPortIndex = 0; UsrPortIndex < EDD_CFG_MAX_PORT_CNT; UsrPortIndex++)
    {
        pHDB->LinkIndProvideType[UsrPortIndex] = LinkIndProvide_NoExist;
    }

    EDDI_InitQueue(pDDB, &pHDB->LowWaterIndReq);
    pHDB->LowWaterLostMark = 0;

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        EDDI_InitQueue(pDDB, &pHDB->PortIDLinkIndReq[HwPortIndex]);
    }

    EDDI_InitQueue(pDDB, &pHDB->SyncDiag.IndQueue);

    EDDI_InitQueue(pDDB, &pDDB->ClearFDBQueue);

    EDDI_InitQueue(pDDB, &pHDB->intTxRqbQueue);
    EDDI_InitQueue(pDDB, &pHDB->intRxRqbQueue);

    pHDB->SyncDiag.LostEntryCnt = 0;
    pHDB->SyncDiag.pCurrentRqb  = EDDI_NULL_PTR;

    pDDB->ErrDetail.bSet        = 0;

    //reset all shadow-counters for this channel
    EDDI_MEMSET(pHDB->sStatisticShadow, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_STATISTICS_SHADOW_CTRS));

    for (;;)
    {
        pHDB->pDDB         = pDDB;
        pHDB->InsertSrcMAC = pDetail->InsertSrcMAC;

        /*-----------------------------------------------------------------------*/
        /* Special check for EDDI_NRT_CHANNEL_B_IF_0 and SYNC                    */
        /*-----------------------------------------------------------------------*/

        /* SYNC channel must also use NRT with EDDI_NRT_CHANEL_B_IF_0 too */
        if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseSYNC)
        {
            if (   (EDD_CDB_CHANNEL_USE_OFF == pDetail->UsedComp.UseNRT)                       
                || (LSA_HOST_PTR_ARE_EQUAL(pDetail->pNRT, LSA_NULL))
                || (pDetail->pNRT->Channel != EDDI_NRT_CHANEL_B_IF_0))
            {
                EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_OpenChIntern, SYNC-Channel must also use NRT with EDDI_NRT_CHANEL_B_IF_0");
                Status = EDD_STS_ERR_CHANNEL_USE;
                break;
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init   PRM Component.                                                 */
        /*-----------------------------------------------------------------------*/
        if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UsePRM)
        {
            /* only one channel supported per device */
            if (pDDB->PRM.HandleCnt == 0)
            {
                /* open channel for trace unit */
                Status = EDDI_PrmOpenChannel(pHDB);
                if (Status != EDD_STS_OK)
                {
                    break;
                }

                /* check if component is initialized */
                pHDB->UsedComp              |= EDDI_COMP_PRM;
                pDDB->PRM.HandleCnt          = 1;
                pDDB->PRM.LesserCheckQuality = (EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY == pDetail->Prm_PortDataAdjustLesserCheckQuality)?LSA_TRUE:LSA_FALSE;
            }
            else
            {
                Status = EDD_STS_ERR_CHANNEL_USE;
                break;
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init   NRT Component.                                                 */
        /*-----------------------------------------------------------------------*/
        if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseNRT)
        {
            /* check if component is initialized */
            if (!pDDB->pLocal_NRT)
            {
                Status = EDD_STS_ERR_PARAM;
                break;
            }

            /* open channel for NRT */
            Status = EDDI_NRTOpenChannel(pDDB, pHDB, pDetail->pNRT);
            if (Status != EDD_STS_OK)
            {
                break;
            }

            pHDB->UsedComp |= EDDI_COMP_NRT;
        }

        /*-----------------------------------------------------------------------*/
        /* Init   XRT Component.                                                 */
        /*-----------------------------------------------------------------------*/
        if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseXRT)
        {
            /* check if component is initialized */
            if (!pDDB->pLocal_CRT)
            {
                Status = EDD_STS_ERR_PARAM;
                break;
            }

            /* open channel for NRT */
            Status = EDDI_CRTOpenChannel(pDDB, pHDB);
            if (Status != EDD_STS_OK)
            {
                break;
            }

            pHDB->UsedComp |= EDDI_COMP_XRT;
        }

        /*-----------------------------------------------------------------------*/
        /* Init   SYNC Component.                                                */
        /*-----------------------------------------------------------------------*/
        if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseSYNC)
        {
            /* check if component is initialized */
            if (!pDDB->pLocal_SYNC)
            {
                Status = EDD_STS_ERR_PARAM;
                break;
            }

            Status = EDDI_SYNCOpenChannel(pDDB, pHDB);
            if (Status != EDD_STS_OK)
            {
                break;
            }

            pHDB->UsedComp |= EDDI_COMP_SYNC;
        }

        /*-----------------------------------------------------------------------*/
        /* Init   SWI Component (not supported)                                  */
        /*-----------------------------------------------------------------------*/
        if (EDD_CDB_CHANNEL_USE_ON == pDetail->UsedComp.UseSWITCH)
        {
            if (!pDDB->pLocal_SWITCH)
            {
                Status = EDD_STS_ERR_PARAM;
                break;
            }

            Status = EDDI_SwiUsrOpenChannel(pDDB);
            if (Status != EDD_STS_OK)
            {
                break;
            }

            pHDB->UsedComp |= EDDI_COMP_SWI;
        }

        //nothing selected -> general channel
        if (pHDB->UsedComp == 0)
        {
            pHDB->UsedComp = EDDI_COMP_GENERAL;
            break;
        }

        break;
    } //end of for-loop

    if (Status == EDD_STS_OK)
    {
        //is now returned in para_block
        EDD_RQB_SET_HANDLE(pRQB, 0xFF); //code sanity
//BV?        EDD_RQB_SET_HANDLE(pRQB, Handle);
//BV?        (*ppRQBChannel)->Handle = Handle;
        (*ppRQBChannel)->HandleLower = pHDB;  /* set EDD Lower Handle */
        return EDD_STS_OK;
    }

    /*-----------------------------------------------------------------------*/
    /* On error we have to undo several things..                             */
    /* (without statuscheck, because we can't do anything.)                  */
    /*-----------------------------------------------------------------------*/
    if (pHDB->UsedComp & EDDI_COMP_PRM)
    {
        Status2             = EDDI_PrmCloseChannel(pHDB);
        pDDB->PRM.HandleCnt = 0;
        pHDB->UsedComp     &= ~EDDI_COMP_PRM;
    }

    if (pHDB->UsedComp & EDDI_COMP_NRT)
    {
        Status2         = EDDI_NRTCloseChannel(pDDB, pHDB);
        pHDB->UsedComp &= ~EDDI_COMP_NRT;
    }

    if (pHDB->UsedComp & EDDI_COMP_XRT)
    {
        Status2         = EDDI_CRTCloseChannel(pDDB);
        pHDB->UsedComp &= ~EDDI_COMP_XRT;
    }

    if (pHDB->UsedComp & EDDI_COMP_SWI)
    {
        Status2         = EDDI_SwiUsrCloseChannel(pHDB);
        pHDB->UsedComp &= ~EDDI_COMP_SWI;
    }

    EDDI_RELEASE_PATH_INFO(&Status2, pSys, pDetail);

    (void)EDDI_HandleRel(pHDB /*Handle*/); //free handle

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_close_channel()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_close_channel( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB = 0;
    LSA_HANDLE_TYPE          UpperHandle;
    LSA_SYS_PTR_TYPE         pSys;
    EDD_UPPER_RQB_PTR_TYPE   pRQBDummy;
    LSA_VOID      LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB);
    LSA_UINT                 UsrPortIndex;
    LSA_BOOL                 bScheduleCloseReq = LSA_FALSE;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
    if (EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_CLOSE_CHANNEL)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_OPCODE);
        Status = EDD_STS_ERR_OPCODE;
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);

    if (Status != EDD_STS_OK)
    {
        EDDI_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eddi_close_channel, invalid HDB!");
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        Status = EDD_STS_ERR_PARAM;
        EDDI_RQB_ERROR(pRQB);
        return;
    }

    pDDB = pHDB->pDDB;

    EDDI_UPPER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_close_channel-> UsedComp:0x%X", pHDB->UsedComp);

    UpperHandle = pHDB->UpperHandle; //save this because we will free HDB
    Cbf         = pHDB->Cbf; //save this because we will free the HDB later on
    pSys        = pHDB->pSys; 

    if (0 == Cbf)
    {
        EDDI_UPPER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_close_channel, no Cbf!");
        EDDI_Excp("eddi_close_channel, no Cbf!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_LockClose(pDDB);

    /*-----------------------------------------------------------------------*/
    /* Close Channel for General                                             */
    /*-----------------------------------------------------------------------*/
    if (pHDB->UsedComp == EDDI_COMP_GENERAL)
    {
        pHDB->UsedComp &= ~EDDI_COMP_GENERAL;
    }

    /*-----------------------------------------------------------------------*/
    /* Close Channel for NRT                                                 */
    /*-----------------------------------------------------------------------*/
    if (pHDB->UsedComp & EDDI_COMP_NRT)
    {
        Status = EDDI_NRTCloseChannel(pDDB, pHDB);

        if (Status == EDD_STS_OK)
        {
            bScheduleCloseReq = LSA_TRUE;
            pHDB->UsedComp &= ~EDDI_COMP_NRT;
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Special check  SYNC-Channel                                           */
    /* Must be checked *BEFORE* NRT_CHB_IF0 will be closed !                 */
    /*-----------------------------------------------------------------------*/
    if (Status == EDD_STS_OK)
    {
        if (pHDB->UsedComp & EDDI_COMP_SYNC)
        {
            if (   (pDDB->SYNC.IrtActivity)
                || (pDDB->SYNC.SyncActivity))
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "SYNCCanBeClosed, SYNC-Channel can not be closed now. IrtActivity:0x%X SyncActivity:0x%X",
                                      pDDB->SYNC.IrtActivity, pDDB->SYNC.SyncActivity);
                Status = EDD_STS_ERR_SEQUENCE;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Close Channel for XRT                                                 */
    /*-----------------------------------------------------------------------*/
    if (Status == EDD_STS_OK)
    {
        if (pHDB->UsedComp & EDDI_COMP_XRT)
        {
            Status = EDDI_CRTCloseChannel(pDDB);

            if (Status == EDD_STS_OK)
            {
                pHDB->UsedComp &= ~EDDI_COMP_XRT;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Close Channel for SYNC                                                */
    /*-----------------------------------------------------------------------*/
    if (Status == EDD_STS_OK)
    {
        if (pHDB->UsedComp & EDDI_COMP_SYNC)
        {
            Status = EDDI_SYNCCloseChannel(pDDB,pHDB);

            if (Status == EDD_STS_OK)
            {
                pHDB->UsedComp &= ~EDDI_COMP_SYNC;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Close Channel for SWI                                                 */
    /*-----------------------------------------------------------------------*/
    if (Status == EDD_STS_OK)
    {
        if (pHDB->UsedComp & EDDI_COMP_SWI)
        {
            Status = EDDI_SwiUsrCloseChannel(pHDB);

            if (Status == EDD_STS_OK)
            {
                pHDB->UsedComp &= ~EDDI_COMP_SWI;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Close Channel for PRM                                                 */
    /*-----------------------------------------------------------------------*/
    if (Status == EDD_STS_OK)
    {
        if (pHDB->UsedComp & EDDI_COMP_PRM)
        {
            Status = EDDI_PrmCloseChannel(pHDB);

            if (Status == EDD_STS_OK)
            {
                pDDB->PRM.HandleCnt = 0;
                pHDB->UsedComp &= ~EDDI_COMP_PRM;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    /* Release all Link-Indication RQBs with CANCEL.                         */
    /*-----------------------------------------------------------------------*/
    if (Status == EDD_STS_OK)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_close_channel, releasing link ind resources");

        if (pHDB->RxOverLoad.cIndication)
        {
            if (pHDB->RxOverLoad.cIndication > 1)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_close_channel, RxOverLoad.cIndication > 1:0x%X", pHDB->RxOverLoad.cIndication);
                EDDI_Excp("eddi_close_channel, RxOverLoad.cIndication:", EDDI_FATAL_ERR_EXCP, pHDB->RxOverLoad.cIndication, 0);
                EDDI_UnlockClose(pDDB);
                return;
            }

            EDDI_RequestFinish(pHDB, pHDB->RxOverLoad.pRQB, EDD_STS_OK_CANCEL);
        }

        do
        {
            pRQBDummy = EDDI_RemoveFromQueue(pDDB, &pHDB->AutoLinkIndReq);

            if (!LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, EDDI_NULL_PTR))
            {
                EDDI_RequestFinish(pHDB, pRQBDummy, EDD_STS_OK_CANCEL);
            }
        }
        while (!(pRQBDummy == EDDI_NULL_PTR));

        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            do
            {
                pRQBDummy = EDDI_RemoveFromQueue(pDDB, &pHDB->PortIDLinkIndReq[HwPortIndex]);

                if (!(pRQBDummy == EDDI_NULL_PTR))
                {
                    EDDI_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK_CANCEL);
                }
            }
            while (!(pRQBDummy == EDDI_NULL_PTR));
        }

        for (UsrPortIndex=0; UsrPortIndex<EDD_CFG_MAX_PORT_CNT; UsrPortIndex++)
        {
            pHDB->LinkIndProvideType[UsrPortIndex] = LinkIndProvide_NoExist;
        }

        //clear SyncDiag-Queue (dont forget pCurrentRqb)
        if (!(pHDB->SyncDiag.pCurrentRqb == EDDI_NULL_PTR))
        {
            EDDI_RequestFinish(pHDB, pHDB->SyncDiag.pCurrentRqb, EDD_STS_OK_CANCEL);
            pHDB->SyncDiag.pCurrentRqb = EDDI_NULL_PTR;
        }

        do
        {
            pRQBDummy = EDDI_RemoveFromQueue(pDDB, &pHDB->SyncDiag.IndQueue);

            if (!(pRQBDummy == EDDI_NULL_PTR))
            {
                EDDI_RequestFinish(pHDB, pRQBDummy, EDD_STS_OK_CANCEL);
            }
        }
        while (!(pRQBDummy == EDDI_NULL_PTR));
    }

    //hdb is freed in next step

    if (Status == EDD_STS_OK)
    {
        if (!bScheduleCloseReq)
        {
            EDDI_CloseChannelFinishOk(pRQB);
        }
        else
        {
            if (pDDB->CloseReq.pTop == EDDI_NULL_PTR)
            {
                Status = EDDI_StartTimer(pDDB, pDDB->CloseReqTimerID,
                                         (LSA_UINT16)((pDDB->NRT.TimeStamp.Timeout_ticks_in10ms * 2) + 1)); // 110ms
                if (Status != EDD_STS_OK)
                {
                    EDDI_UPPER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_close_channel, EDDI_StartTimer Status:0x%X, CloseReqTimerID:0x%X", 
                        Status, pDDB->CloseReqTimerID);
                    EDDI_Excp("eddi_close_channel, CloseReqTimerID", EDDI_FATAL_ERR_EXCP, Status, 0);
                    EDDI_UnlockClose(pDDB);
                    return;
                }
            }

            EDDI_UPPER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_close_channel, finishing asynchronously!");
            EDDI_AddToQueueEnd(pDDB, &pDDB->CloseReq, pRQB);
        }
    }
    else
    {
        EDD_RQB_SET_RESPONSE(pRQB, Status);
        EDD_RQB_SET_HANDLE(pRQB, UpperHandle);
        EDDI_REQUEST_UPPER_DONE(Cbf, pRQB, pSys);
    }

    EDDI_UnlockClose(pDDB);
    EDDI_UPPER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "<- eddi_close_channel, bScheduleCloseReq=0x%x", bScheduleCloseReq);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CloseChannelFinishOk()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CloseChannelFinishOk( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT               Status;
    LSA_RESULT               Status2;
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB = LSA_NULL;
    LSA_SYS_PTR_TYPE         pSys;
    EDDI_DETAIL_PTR_TYPE     pDetail;
    LSA_HANDLE_TYPE          UpperHandle;
    LSA_VOID                 LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE pRQB);

	Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);

	if (Status != EDD_STS_OK)
	{
		EDDI_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "EDDI_CloseChannelFinishOk, Invalid HDB!");
		EDDI_Excp("EDDI_CloseChannelFinishOk, Invalid HDB!", EDDI_FATAL_ERR_EXCP, Status, 0);
		return;
	}

    EDDI_UPPER_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CloseChannelFinishOk->");

    #if defined (EDDI_CFG_REV5)
    if (pHDB->pDDB->NRT.TimeStamp.State == EDDI_TS_STATE_CLOSING)
    {
        EDDI_NRTReloadTimeStamp(pHDB->pDDB, EDDI_TS_CALLER_CLOSE);
    }
    #endif

    /*-----------------------------------------------------------------------*/
    /* If all channels successfully closed, we release the handle and call   */
    /* the LSA release path info-macro.                                      */
    /*-----------------------------------------------------------------------*/
    //save the following HDB-parameters because HDB is then freed
    UpperHandle = pHDB->UpperHandle;
    Cbf         = pHDB->Cbf;
    pSys        = pHDB->pSys;
    pDetail     = pHDB->pDetail;

    Status  = EDDI_HandleRel(pHDB /*Handle*/); /* free HDB. don't use HDB and pHDB anymore! */

    /*-----------------------------------------------------------------------*/
    /* Finish the request. Note: dont use EDDI_RequestFinish() here, because */
    /* pHDB is already released and invalid!                                 */
    /*-----------------------------------------------------------------------*/

    EDD_RQB_SET_RESPONSE(pRQB, Status);
    EDD_RQB_SET_HANDLE(pRQB, UpperHandle);
    EDDI_REQUEST_UPPER_DONE(Cbf, pRQB, pSys);

    if (Status == EDD_STS_OK)
    {
        /* Call LSA-Release-Path-Info. This is done only on success.  */
        /* NOTE: If this fails, we have already closed all things and */
        /*       can't do anything so we ignore this error or fatal?  */

        EDDI_RELEASE_PATH_INFO(&Status2, pSys, pDetail);

        if (Status2 != LSA_RET_OK)
        {
            EDDI_UPPER_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_CloseChannelFinishOk, EDDI_RELEASE_PATH_INFO() generates invalid Status:0x%X", Status2);
            EDDI_Excp("EDDI_CloseChannelFinishOk, EDDI_RELEASE_PATH_INFO() generates invalid Status:", EDDI_FATAL_ERR_EXCP, Status2, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_DeferredCloseChannelReq()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DeferredCloseChannelReq( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;
    EDD_UPPER_RQB_PTR_TYPE          pRQB;
    LSA_RESULT                      Status;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_DeferredCloseChannelReq->");

    pRQB = EDDI_RemoveFromQueue(pDDB, &pDDB->CloseReq);

    if (!pRQB)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DeferredCloseChannelReq, pRQB == 0");
        EDDI_Excp("EDDI_DeferredCloseChannelReq, pRQB == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_CloseChannelFinishOk(pRQB);

    if (pDDB->CloseReq.pTop)
    {
        Status = EDDI_StartTimer(pDDB, pDDB->CloseReqTimerID, (LSA_UINT16)1); //100ms
        if (Status != EDD_STS_OK)
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DeferredCloseChannelReq, EDDI_StartTimer() generates invalid Status:0x%X", Status);
            EDDI_Excp("EDDI_DeferredCloseChannelReq, EDDI_StartTimer() generates invalid Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_oc.c                                                    */
/*****************************************************************************/
