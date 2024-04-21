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
/*  F i l e               &F: eddi_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-system input-functions                      */
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
#include "eddi_check.h"

#include "eddi_ext.h"
#include "eddi_req.h"
#include "eddi_lock.h"

#if defined (EDDI_CFG_ERTEC_200)
#include "eddi_ser_e200.h"
#endif

#include "eddi_swi_ext.h"

#if defined (EDDI_RED_PHASE_SHIFT_ON)
#include "eddi_crt_phase_ext.h"
#endif

#include "eddi_ser_ext.h"
#include "eddi_ser.h"
#include "eddi_crt_check.h"
//#include "eddi_io_provctrl.h"

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/
#define EDDI_MODULE_ID     M_ID_EDDI_SYS
#define LTRC_ACT_MODUL_ID  8

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

EDDI_INFO_TYPE  *  g_pEDDI_Info ;

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SetDMACtrlIntern( EDDI_SET_DMA_CTRL_TYPE  const * const pSetDMACtrl,
                                                             EDDI_LOCAL_DDB_PTR_TYPE         const pDDB );

EDDI_INI_PARAMS_FIXED_TYPE  const  EDDIConstValues[1]                                       = {EDDI_CONST_VALUES_INIT};
LSA_UINT16                  const  EDDICrtMaxConsSmallSCF[EDDI_CRT_CYCLE_BASE_FACTOR_CRIT]  = EDDI_CRT_MAX_CONS_SMALL_SCF_INIT;


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    eddi_init()                                 +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the EDDI.                                     +*/
/*+               The number of handles the edd can manage is defined by    +*/
/*+               EDDI_CFG_HANDLE_CNT. The memory for handle-management is  +*/
/*+               static or will be allocated depending on                  +*/
/*+               EDDI_LOCAL_MEM_STATIC.                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_init( LSA_VOID )
{
    LSA_HANDLE_TYPE     Ctr;
    LSA_UINT32          EDDInfoAllocSize;
    
    EDDI_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eddi_init->");

    EDDInfoAllocSize = (LSA_UINT32)sizeof(EDDI_INFO_TYPE);
    //round up size for a performant MEMSET
    EDDInfoAllocSize = (EDDInfoAllocSize + 7UL) & 0xFFFFFFF8UL;
    EDDI_AllocLocalMem((void * *)&g_pEDDI_Info , EDDInfoAllocSize);
    if (g_pEDDI_Info  == (EDDI_INFO_TYPE *)0)
    {
        return EDD_STS_ERR_RESOURCE;
    }

    EDDI_MemSet(g_pEDDI_Info , (LSA_UINT8)0, EDDInfoAllocSize);

    EDDI_EnterIntern();

    //Init all DDBs
    for (Ctr = 0; Ctr < EDDI_CFG_MAX_DEVICES; Ctr++)
    {
        EDDI_DDB_TYPE  *  const  pDDB = &g_pEDDI_Info ->DDB[Ctr];

        pDDB->bUsed    = EDDI_DDB_NOT_USED;
        pDDB->DeviceNr = Ctr;

        EDDI_LOWER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIISetDummyIsr->set device granular interrupt-function-pointer to dummy-interrupt-function");
        EDDI_SIISetDummyIsr(pDDB);
    }

    //Init all HDBs
    //HDBs are kept in an array, with the LowerHandle as index.
    //Free HDBs are additionally kept in a chained queue in g_pEDDI_Info ,
    //Used HDBs are kept in a chained queue in the corresponding DDB
    g_pEDDI_Info ->FreeHDBQueue.Count  = 0;
    g_pEDDI_Info ->FreeHDBQueue.pFirst = EDDI_NULL_PTR;
    g_pEDDI_Info ->FreeHDBQueue.pLast  = EDDI_NULL_PTR;
    for (Ctr = 0; Ctr < EDDI_CFG_MAX_CHANNELS; Ctr++)
    {
        EDDI_HDB_TYPE  *  const  pHDB = &g_pEDDI_Info ->HDB[Ctr];

        pHDB->InUse  = 0;
        pHDB->Handle = Ctr;

        EDDI_QueueAddToEnd(&g_pEDDI_Info ->FreeHDBQueue, &pHDB->QueueLink);    //add to free queue
    }

    (LSA_VOID)EDDI_Check();

    g_pEDDI_Info ->Excp.Error = EDDI_FATAL_NO_ERROR;
    EDDI_IniRequestFctTable();

    eddi_ApplSync_Init();

    EDDI_ExitIntern();

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    eddi_undo_init()                            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes EDDI.                                       +*/
/*+               This is only possible if no device is open                +*/
/*+               EDD_STS_ERR_SEQUENCE if device is open.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_undo_init( LSA_VOID )
{
    LSA_UINT32  Ctr;
    LSA_UINT16  RetVal;

    if (g_pEDDI_Info  == (EDDI_INFO_TYPE *)0)
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_EnterIntern();

    for (Ctr = 0; Ctr < EDDI_CFG_MAX_DEVICES; Ctr++)
    {
        EDDI_DDB_TYPE  *  const  pDDB = &g_pEDDI_Info ->DDB[Ctr];

        if (pDDB->bUsed == EDDI_DDB_USED)
        {
            EDDI_ExitIntern();
            return EDD_STS_ERR_SEQUENCE;
        }
    }

    EDDI_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eddi_undo_init");

    EDDI_ExitIntern();
    EDDI_FREE_LOCAL_MEM(&RetVal, g_pEDDI_Info );

    if (RetVal != LSA_RET_OK)
    {
        EDDI_Excp("eddi_undo_init, EDDI_FREE_LOCAL_MEM RetVal", RetVal, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    g_pEDDI_Info  = (EDDI_INFO_TYPE *)0;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  eddi_SRTProviderGetCycleCounter()
 *
 * function:       Get SRT-provider CycleCounter (31.25us ticks)
 *
 * parameters:     
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SRTProviderGetCycleCounter( EDDI_HANDLE                  hDDB,
                                                                      EDDI_CYCLE_COUNTER_PTR_TYPE  pCycleCounter )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_SYNC_GET_SET_TYPE          SyncGet;

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_SRTProviderGetCycleCounter->");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_SRTProviderGetCycleCounter", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pCycleCounter, LSA_NULL))
    {
        EDDI_Excp("eddi_SRTProviderGetCycleCounter, pCycleCounter is NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    if (!pDDB->Glob.HWIsSetup)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SRTProviderGetCycleCounter, pDDB->Glob.HWIsSetup invalid");
        return EDD_STS_ERR_SEQUENCE;
    }

    (void)EDDI_CycUpdateSoftwareCounter(pDDB, &SyncGet, EDDI_NULL_PTR); //aktualisieren !

    pCycleCounter->High = SyncGet.CycleCounterHigh;
    pCycleCounter->Low  = SyncGet.CycleCounterLow;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_GetCycleCounterLow()
 *
 * function:
 *
 * parameters:
 *
 * return value:  CycleCounterLow
 *
 *===========================================================================*/
LSA_UINT32  EDDI_SYSTEM_IN_FCT_ATTR  eddi_GetCycleCounterLow( EDDI_HANDLE  const  hDDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    LSA_UINT32                      CurrentCycleCounterLow;

    //No Traces here! (Otherwise Recursion bec. function will be used as Timestamp-Source for Trace-Entries)

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        //No Exception here (Otherwise Recursion bec. function will be used as Timestamp-Source for Trace-Entries)!
        //EDDI_Excp("eddi_GetCycleCounterLow", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }

    CurrentCycleCounterLow = IO_R32(CYCL_COUNT_VALUE);

    return CurrentCycleCounterLow;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  eddi_SyncGet()
 *
 * function:       Get the consistent values of:
 *                 - timestamp
 *                 - CycleCounter
 *                 - ExtPllIn-Timestamp (only if no NIL-ptr!)
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_SyncGet( EDDI_HANDLE                const  hDDB,
                                                 EDDI_SYNC_GET_SET_TYPE  *  const  pSyncGet,
                                                 LSA_UINT32              *  const  pExtPLLTimeStampInTicks )
{
    EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_CYC_COUNTER_STATE_TYPE         State;
    LSA_UINT64                          NanoSeconds;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_SyncGet->");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_SyncGet, hDDB not valid", EDDI_FATAL_ERR_EXCP, pDDB->hDDB, hDDB);
        return EDD_STS_ERR_EXCP;
    }

    if (!pDDB->Glob.HWIsSetup)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_SyncGet->HW is not setup");
        return EDD_STS_ERR_SEQUENCE;
    }

    State = EDDI_CycUpdateSoftwareCounter(pDDB, pSyncGet, pExtPLLTimeStampInTicks);

    /********************************/
    /* calc PTCP time from CycleCtr */
    NanoSeconds = (LSA_UINT64)pSyncGet->CycleCounterHigh & 0x0000FFFFUL;
	NanoSeconds <<= 32;
	NanoSeconds |= (LSA_UINT64)pSyncGet->CycleCounterLow;
	NanoSeconds *= EDDI_CRT_CYCLE_LENGTH_GRANULARITY_NS;
    
    pSyncGet->PTCPSeconds     = (LSA_UINT32)(NanoSeconds/1000000000);
    pSyncGet->PTCPNanoSeconds = (LSA_UINT32)(NanoSeconds%1000000000);

    if (State != EDDI_CYC_COUNTER_STATE_RUNNING)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_SyncGet->wrong state of EDDI_CycUpdateSoftwareCounter: %d",
            State);
        return EDD_STS_ERR_SEQUENCE;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  eddi_GetTimeStamp()
 *
 * function:       Get the current Timestamp-Value.
 *                 This function does not call the EDDI_ENTER_COM / EDDI_EXIT_COM Macros.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT32            *     pTimeStamp  ResultValue
 *                                             For ERTEC400/200 this Value is counted in 10ns entities.
 *                                             For SOC1/SOC2 this Value is counted in 1ns entities.
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_GetTimeStamp( EDDI_HANDLE     const  hDDB,
                                                      LSA_UINT32   *  const  pTimeStamp )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_GetTimeStamp", EDDI_FATAL_ERR_EXCP, g_pEDDI_Info , pDDB);
        return EDD_STS_ERR_EXCP;
    }

    *pTimeStamp = IO_R32(CLK_COUNT_VALUE); //returns 10ns-timestamp or 1ns-timestamp

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SetExternalException()
 *
 * function:
 *
 * parameters:
 *
 * return value:  LSA_RESULT
 *
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_SetExternalException( EDDI_HANDLE  const  hDDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_SetExternalException->");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_SetExternalException, hDDB not valid", EDDI_FATAL_ERR_EXCP, pDDB->hDDB, hDDB);
        return EDD_STS_ERR_EXCP;
    }

    if (!pDDB->Glob.HWIsSetup)
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    pDDB->Glob.HWIsSetup = LSA_FALSE;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIISetDummyIsr->set device granular interrupt-function-pointer to dummy-interrupt-function");
    EDDI_SIISetDummyIsr(pDDB);

    EDDI_StopUser();

    pDDB->pProfKRamInfo->Excp.acw_base_adr = IO_x32(ACW_BASE_ADR);
    IO_W32(ACW_BASE_ADR, SER10_NULL_PTR);

    IO_W32(IRT_CTRL, 0x0UL);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SyncAdjust()
 *
 * function:
 *
 * parameters:
 *
 * return value:  EDD_STS_OK
 *                EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_SyncAdjust( EDDI_HANDLE  const  hDDB,
                                                    LSA_INT32    const  AdjustIntervalNs )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "eddi_SyncAdjust->AdjustIntervalNs:0x%X", AdjustIntervalNs);

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_SyncAdjust", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    if (!pDDB->Glob.HWIsSetup)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_SyncAdjust->HW is not setup");
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_ENTER_COM_S();
    
    EDDI_CycSetAdjustInterval(pDDB, &pDDB->CycCount, AdjustIntervalNs);
    
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_SyncAdjust<-");
    
    EDDI_EXIT_COM_S();

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SyncGetRealOffsetNs()
 *
 * function:
 *
 * parameters:
 *
 * return value:  EDD_STS_OK
 *                EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_SyncGetRealOffsetNs( EDDI_HANDLE     const  hDDB,
                                                             LSA_INT32    *  const  pRealOffsetNs )
{
    LSA_INT32                       RealOffsetTicks;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "eddi_SyncGetRealOffsetNs->");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_SyncGetRealOffsetNs", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_MODE;
    }

    if (!pDDB->Glob.HWIsSetup)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_SyncGetRealOffsetNs->HW is not setup");
        return EDD_STS_ERR_SEQUENCE;
    }

    if (!pDDB->PRM.PDControlPLL.PDControlPllActive)
    {
        return EDD_STS_ERR_HW;
    }

    //Get External Offset
    RealOffsetTicks = EDDI_CycGetRealPhaseOffsetTicks(pDDB); //Show ExtOffset in Ns

    if (RealOffsetTicks == EDDI_CYC_NO_EXT_PLL_IN_SIGNAL)
    {
        *pRealOffsetNs = 0;
        return EDD_STS_ERR_HW;
    }
    else
    {
        *pRealOffsetNs = RealOffsetTicks * 10;
        return EDD_STS_OK;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_SetDMACtrl()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SetDMACtrl( EDDI_SET_DMA_CTRL_TYPE  *  const  pSetDMACtrl )
{
    EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)pSetDMACtrl->hDDB;
    LSA_UINT32                       ImageMode;
    LSA_RESULT                       Status;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_SetDMACtrl->");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != pSetDMACtrl->hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_SetDMACtrl, pDDB->hDDB != pSetDMACtrl->hDDB", EDDI_FATAL_ERR_EXCP, pDDB->hDDB, pSetDMACtrl->hDDB);
        return EDD_STS_ERR_EXCP;
    }

    ImageMode = IO_x32(IMAGE_MODE);

    if (EDDI_GetBitField32(ImageMode, EDDI_SER_IMAGE_MODE_BIT__BufMode) != SER_SYNC_IMAGE_BUF_MODE)
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "eddi_SetDMACtrl, ERROR: Use function only in Single-Buffered Mode; BufMode, IMAGE_BUF_MODE, BufMode:0x%X SER_SYNC_IMAGE_BUF_MODE:0x%X",
                              EDDI_GetBitField32(ImageMode, EDDI_SER_IMAGE_MODE_BIT__BufMode), SER_SYNC_IMAGE_BUF_MODE);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_IMAGE_MODE);
        return EDD_STS_ERR_PARAM;
    }

    if (pSetDMACtrl->UserBufferSize < pDDB->ProcessImage.offset_ProcessImageEnd)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_DMA_UserBufferSize);
        return EDD_STS_ERR_PARAM;
    }

    //Check for 8-Byte-Granlularity of DMA-Adresses and -Length
    if (   ((LSA_UINT32)pSetDMACtrl->pUserRam    % 8)
        || (pSetDMACtrl->UserBufferSize          % 8)
        || (pSetDMACtrl->InDataCopy_Size         % 8)
        || (pSetDMACtrl->InData_CopyStartOffset  % 8)
        || (pSetDMACtrl->OutData_CopyStartOffset % 8)
        || (pSetDMACtrl->OutDataCopy_Size        % 8))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SetDMACtrl, ERROR: 8-Byte-Granlularity :0x%X", (LSA_UINT32)pSetDMACtrl->pUserRam);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_ALIGN);
        return EDD_STS_ERR_PARAM;
    }

    if ((pSetDMACtrl->MemModeDMA != MEMORY_SDRAM_ERTEC) && (pSetDMACtrl->MemModeDMA != MEMORY_SHARED_MEM))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SetDMACtrl, ERROR: MemModeDMA:0x%X", (LSA_UINT32)pSetDMACtrl->MemModeDMA);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_MEMMODE);
        return EDD_STS_ERR_PARAM;
    }

    EDDI_ENTER_COM_S();

    Status = EDDI_SetDMACtrlIntern(pSetDMACtrl, pDDB);

    EDDI_EXIT_COM_S();

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetDMACtrlIntern()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SetDMACtrlIntern( EDDI_SET_DMA_CTRL_TYPE   const *  const  pSetDMACtrl,
                                                                EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB )
{
    LSA_UINT32                               ctr, start_in, end_in;
    LSA_UINT32                               start_out, end_out;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp = pDDB->pLocal_CRT;
    EDDI_PROCESS_IMAGE_TYPE                  ProcessImage;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SetDMACtrlIntern->");

    if ((pCRTComp->ConsumerList.UsedEntriesRTC123) || (pCRTComp->ProviderList.UsedEntries))
    {
        if (   ((LSA_UINT32)pSetDMACtrl->pUserRam != (LSA_UINT32)pCRTComp->DMACtrl.pUserRam)
            || (pSetDMACtrl->UserBufferSize       != pCRTComp->DMACtrl.UserBufferSize))
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_SetDMACtrlIntern, ERROR: pUserRam | UserBufferSize, :0x%X :0x%X",
                                  pSetDMACtrl->UserBufferSize, pCRTComp->DMACtrl.UserBufferSize);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_Cons_Prov_Entries);
            return EDD_STS_ERR_SEQUENCE;
        }
    }

    ProcessImage = pDDB->ProcessImage;

    //OUT
    if (pSetDMACtrl->OutDataCopy_Size == 0)
    {
        ProcessImage.pUserDMA = pDDB->pKRam;
        start_out             = 0;
        end_out               = 0;
    }
    else
    {
        start_out = pSetDMACtrl->OutData_CopyStartOffset;
        end_out   = start_out + pSetDMACtrl->OutDataCopy_Size;

        if (end_out <= ProcessImage.offset_ProcessImageEnd)
        {
            //found OUT Sync
        }
        else
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SetDMACtrlIntern, ERROR: end_out<=ProcessImage.offset_ProcessImageEnd :0x%X :0x%X",
                                  end_out, ProcessImage.offset_ProcessImageEnd);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_OUT_DATA);
            return EDD_STS_ERR_PARAM;
        }

        ProcessImage.pUserDMA = pSetDMACtrl->pUserRam;
    }

    //IN
    if ((pSetDMACtrl->InDataCopy_Size == 0) && (pSetDMACtrl->OutDataCopy_Size == 0))
    {
        ProcessImage.pUserDMA = pDDB->pKRam;
        start_in              = 0;
        end_in                = 0;
    }
    else
    {
        start_in = pSetDMACtrl->InData_CopyStartOffset;
        end_in   = start_in + pSetDMACtrl->InDataCopy_Size;

        if (end_in <= ProcessImage.offset_ProcessImageEnd)
        {
            //found IN Sync
        }
        else
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_SetDMACtrlIntern, ERROR: end_in<=ProcessImage.offset_ProcessImageEnd :0x%X :0x%X",
                                  end_in, ProcessImage.offset_ProcessImageEnd);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_IN_DATA);
            return EDD_STS_ERR_PARAM;
        }

        ProcessImage.pUserDMA = pSetDMACtrl->pUserRam;
    }

    //check for overlaps
    if ((pSetDMACtrl->InDataCopy_Size == 0) || (pSetDMACtrl->OutDataCopy_Size == 0))
    {
        //checked already!
    }
    else
    {
        if (start_in == start_out)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_SetDMACtrlIntern, ERROR: start_in == start_out :0x%X :0x%X",
                                  start_in, start_out);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_IN_DATA);
            return EDD_STS_ERR_PARAM;
        }

        if (start_in > start_out)
        {
            if (end_out > start_in)
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_SetDMACtrlIntern, ERROR: end_out > start_in :0x%X :0x%X",
                                      end_out, start_in);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_IN_DATA);
                return EDD_STS_ERR_PARAM;
            }
        }
        else
        {
            if (end_in > start_out)
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_SetDMACtrlIntern, ERROR: end_in > start_out :0x%X :0x%X",
                                      end_in, start_out);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_IN_DATA);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    if ((start_in > start_out) && (end_in < end_out))
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_SetDMACtrlIntern, ERROR: (start_in > start_out) && (end_in < end_out) :0x%X :0x%X",
                              end_in, end_out);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DMA_IN_DATA);
        return EDD_STS_ERR_PARAM;
    }

    IO_W32(DMA_OUT_LENGTH, 0);
    IO_W32(DMA_IN_LENGTH,  0);

    if (pCRTComp->DMACtrl.UserBufferSize == 0) //First Time
    {
        //set limits for searching in FCW-class
        LSA_UINT32  const  IdBeginSearchProv = pCRTComp->MetaInfo.RTC3IDStartProv;
        LSA_UINT32  const  IdEndSearchProv   = pCRTComp->MetaInfo.RTC3IDEndProv;

        //check all available FCW-Provider
        for (ctr = IdBeginSearchProv; ctr < IdEndSearchProv; ctr++)
        {
            EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider = &pCRTComp->ProviderList.pEntry[ctr];

            //Check ProviderStatus
            if (   (pProvider->Status == EDDI_CRT_PROV_STS_INACTIVE)
                || (pProvider->Status == EDDI_CRT_PROV_STS_ACTIVE))
            {
                EDDI_CRT_DATA_APDU_STATUS       *  pAPDU_KRAM;
                EDDI_CRT_DATA_APDU_STATUS       *  pAPDU_UserRam;
                EDD_UPPER_MEM_U8_PTR_TYPE   const  pUserDataBuffer      = pSetDMACtrl->pUserRam + pProvider->LowerParams.DataOffset;

                //update pUserDataBuffer of this Provider
                pProvider->pUserDataBuffer = pUserDataBuffer;

                if //this Provider has an associated Provider
                   (pProvider->IRTtopCtrl.pAscProvider)  //only possible at RTC3-AUX-Provider and RTC3-Provider!
                {
                    #if defined (EDDI_CHECK_RTC3_AUX_PROVIDER_HANDLING)
                    if //associated Provider has no reference to this Provider
                       (pProvider->IRTtopCtrl.pAscProvider->IRTtopCtrl.pAscProvider != pProvider)
                    {
                        EDDI_Excp("EDDI_SetDMACtrlIntern, ERROR in associated Provider, wrong AssociatedProviderId",
                                  EDDI_FATAL_ERR_EXCP, pProvider, pProvider->IRTtopCtrl.pAscProvider->IRTtopCtrl.pAscProvider);
                        return EDD_STS_ERR_EXCP;
                    }
                    #endif

                    //update pUserDataBuffer of associated Provider too!
                    pProvider->IRTtopCtrl.pAscProvider->pUserDataBuffer = pUserDataBuffer;
                }

                pAPDU_UserRam = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pUserDataBuffer;
                pAPDU_KRAM    = (EDDI_CRT_DATA_APDU_STATUS *)(void *)pProvider->LowerParams.pKRAMDataBuffer;

                //Update Provider APDU Status in the User RAM
                pAPDU_UserRam->Block = pAPDU_KRAM->Block;
            }
        }

        //check all available FCW-Consumer
        if (pCRTComp->ConsumerList.UsedEntriesRTC123)     
        {
            //set limits for searching in FCW-class
            LSA_UINT32  const  IdBeginSearchCons = pCRTComp->MetaInfo.RTC123IDStart;
            LSA_UINT32  const  IdEndSearchCons   = pCRTComp->ConsumerList.LastIndexRTC123 + 1;

            for (ctr = IdBeginSearchCons; ctr < IdEndSearchCons; ctr++)
            {
                EDDI_CRT_CONSUMER_TYPE  *  const  pConsumer = &pCRTComp->ConsumerList.pEntry[ctr];
                EDDI_CRT_CONS_SB_TYPE   *  const  pSBEntry  = pConsumer->pSB;

                //Check ConsumerStatus
                if (   (   (pSBEntry->Status == EDDI_CRT_CONS_STS_INACTIVE)
                        || (pSBEntry->Status == EDDI_CRT_CONS_STS_ACTIVE) )
                        && (EDDI_LIST_TYPE_FCW == pConsumer->LowerParams.ListType) )
                {
                    //update pUserDataBuffer of this Consumer
                    pConsumer->pUserDataBuffer = pSetDMACtrl->pUserRam + pConsumer->LowerParams.DataOffset;
                }
            }
        }
    }

    IO_W32(DMA_IN_SRC_ADR,   DEV_kram_adr_to_asic_register(pDDB->pKRam + start_in,  pDDB));
    IO_W32(DMA_OUT_DEST_ADR, DEV_kram_adr_to_asic_register(pDDB->pKRam + start_out, pDDB));

    switch (pSetDMACtrl->MemModeDMA)
    {
        case MEMORY_SHARED_MEM:
        {
            IO_W32(DMA_IN_DEST_ADR0, pDDB->shared_mem_adr_to_asic(pDDB->hSysDev, pSetDMACtrl->pUserRam + start_in, pDDB->ERTEC_Version.Location));
            IO_W32(DMA_OUT_SRC_ADR0, pDDB->shared_mem_adr_to_asic(pDDB->hSysDev, pSetDMACtrl->pUserRam + start_out, pDDB->ERTEC_Version.Location));
        }
        break;

        case MEMORY_SDRAM_ERTEC:
        {
            IO_W32(DMA_IN_DEST_ADR0, pDDB->sdram_mem_adr_to_asic(pDDB->hSysDev, pSetDMACtrl->pUserRam + start_in, pDDB->ERTEC_Version.Location));
            IO_W32(DMA_OUT_SRC_ADR0, pDDB->sdram_mem_adr_to_asic(pDDB->hSysDev, pSetDMACtrl->pUserRam + start_out, pDDB->ERTEC_Version.Location));
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_SetDMACtrlIntern", EDDI_FATAL_ERR_EXCP, pSetDMACtrl->MemModeDMA, 0);
            return EDD_STS_ERR_PARAM;
        }
    }

    IO_W32(DMA_IN_DEST_ADR1, 0); // will not be used in single-Buffered Mode anyway!!
    IO_W32(DMA_OUT_SRC_ADR1, 0); // will not be used in single-Buffered Mode anyway!!

    IO_W32(DMA_OUT_LENGTH, pSetDMACtrl->OutDataCopy_Size);
    IO_W32(DMA_IN_LENGTH,  pSetDMACtrl->InDataCopy_Size);

    pCRTComp->DMACtrl = *pSetDMACtrl;

    pDDB->ProcessImage = ProcessImage;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SWAP_32_FCT()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_BIG_ENDIAN)
LSA_UINT32  EDDI_SWAP_32_FCT( LSA_UINT32  const  var )
{
    LSA_UINT32  tmp;

    tmp = ( (var & 0xFF000000UL) >> 24 )  +
          ( (var & 0x00FF0000UL) >>  8 )  +
          ( (var & 0x0000FF00UL) <<  8 )  +
          ( (var & 0x000000FFUL) << 24 );

    return tmp;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_XPLLSetPLLMode()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_XPLLSetPLLMode( LSA_UINT16               const  PllMode,
                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT  Result;

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_XPLLSetPLLMode->PllMode:0x%X", PllMode);

    EDDI_ENTER_COM_S();

    EDDI_LL_XPLL_SETPLLMODE_OUT (&Result, pDDB->hSysDev, pDDB->IRTE_SWI_BaseAdr, pDDB->ERTEC_Version.Location, PllMode);
    if (EDD_STS_OK == Result)
    {
        pDDB->PRM.PDControlPLL.RecordSet_A.PLL_Mode = EDDI_HTONS(PllMode);
    }

    EDDI_EXIT_COM_S();

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  eddi_ResetIRTStarttime()
 *
 * function:       Reset all IRTStartTimeRegisters
 *
 * parameters:
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_ResetIRTStarttime( EDDI_HANDLE  const  hDDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_ResetIRTStarttime->");

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    if ((pDDB->Glob.HWIsSetup) && (pDDB->pLocal_CRT))
    {
        EDDI_CrtPhaseReset(pDDB);
    }
    #endif

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  eddi_ll_PhyWrite()
 *
 * function:       This function writes a value to a PHY register.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT32                  HwPortIndex
 *                 LSA_UINT32                  Register
 *                 LSA_UINT32                 *pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_ll_PhyWrite( EDDI_SYS_HANDLE        const hSysDev,
                                           const EDDI_HANDLE            const hDDB,
                                                 LSA_UINT32             const HwPortIndex,
                                                 LSA_UINT16                   Register,
                                                 LSA_UINT32     const * const pValue )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    LSA_UNUSED_ARG(hSysDev); //satisfy lint!

    Status = EDDI_GetDDB(hDDB, &pDDB);

    if (Status == EDD_STS_OK)
    {
        EDDI_SwiPhyWriteMDCA(Register, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, pValue, pDDB);
    }
    else
    {
        EDDI_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "eddi_ll_PhyWrite, pDDB not found, HwPortIndex:0x%X Register:0x%X Status:0x%X", HwPortIndex, Register, Status);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
/*=============================================================================
 * function name:  eddi_ll_PhyWriteRaw()
 *
 * function:       Writes any PHY register at any PHY address.
 *
 * parameters:     EDDI_HANDLE                   hDDB
 *                 LSA_UINT16                    PhyAddr
 *                 LSA_UINT16	                Register
 *                 LSA_UINT32 *           const  pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR   eddi_ll_PhyWriteRaw(EDDI_HANDLE         hDDB,
                                                    LSA_UINT16          PhyAddr,
                                                    LSA_UINT16	        Register,
                                                    LSA_UINT32 *  const pValue )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    Status = EDDI_GetDDB(hDDB, &pDDB);

    if (EDD_STS_OK == Status)
    {
        EDDI_SwiPhyWriteMDCA(Register, PhyAddr, pValue, pDDB);
    }
    else
    {
        EDDI_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR, "eddi_ll_PhyWriteRaw, pDDB not found for hDDB(0x%X), PhyAddr:0x%X Register:0x%X Status:0x%X", hDDB, PhyAddr, Register, Status);
    }
}
#endif


/*=============================================================================
 * function name:  eddi_ll_PhyRead()
 *
 * function:       This function reads a value from a PHY register.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT32                  HwPortIndex
 *                 LSA_UINT32                  Register
 *                 LSA_UINT32                  *pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_ll_PhyRead( EDDI_SYS_HANDLE  const  hSysDev,
                                          const EDDI_HANDLE      const  hDDB,
                                                LSA_UINT32       const  HwPortIndex,
                                                LSA_UINT16              Register,
                                                LSA_UINT32           *  pValue )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    LSA_UNUSED_ARG(hSysDev); //satisfy lint!

    Status = EDDI_GetDDB(hDDB, &pDDB);

    if (Status == EDD_STS_OK)
    {
        EDDI_SwiPhyReadMDCA(Register, pDDB->Glob.PortParams[HwPortIndex].PhyAdr, pValue, pDDB);
    }
    else
    {
        EDDI_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "eddi_ll_PhyRead, pDDB not found, HwPortIndex:0x%X Register:0x%X Status:0x%X", HwPortIndex, Register, Status);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
/*=============================================================================
 * function name:  eddi_ll_PhyReadRaw()
 *
 * function:       Reads any PHY register at any PHY address.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT16                  PhyAddr
 *                 LSA_UINT16	               Register
 *                 LSA_UINT32 *                pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR   eddi_ll_PhyReadRaw( EDDI_HANDLE         hDDB,
                                                    LSA_UINT16		    PhyAddr,
                                                    LSA_UINT16		    Register,
                                                    LSA_UINT32 *        pValue )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    Status = EDDI_GetDDB(hDDB, &pDDB);

    if (Status == EDD_STS_OK)
    {
        EDDI_SwiPhyReadMDCA(Register, PhyAddr, pValue, pDDB);
    }
    else
    {
        EDDI_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR, "eddi_ll_PhyReadRaw,  pDDB not found for hDDB(0x%X), PhyAddr:0x%X Register:0x%X Status:0x%X", hDDB, PhyAddr, Register, Status);
    }
}
#endif


/*=============================================================================
 * function name:  eddi_ll_GetPhyType()
 *
 * function:       This function gets the PHY type.
 *
 * parameters:     EDDI_SYS_HANDLE               const  hSysDev
 *                 EDDI_HANDLE                   const  hDDB
 *                 LSA_UINT32                    const  HwPortIndex
 *                 EDDI_PHY_TRANSCEIVER_TYPE  *  const  pPhyType
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_ll_GetPhyType( EDDI_SYS_HANDLE               const  hSysDev,
                                             const EDDI_HANDLE                   const  hDDB,
                                                   LSA_UINT32                    const  HwPortIndex,
                                                   EDDI_PHY_TRANSCEIVER_TYPE  *  const  pPhyType )
{
    LSA_RESULT               Status;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    LSA_UNUSED_ARG(hSysDev); //satisfy lint!

    Status = EDDI_GetDDB(hDDB, &pDDB);

    if (Status == EDD_STS_OK)
    {
        if (HwPortIndex < EDD_CFG_MAX_PORT_CNT)
        {
            *pPhyType = pDDB->Glob.PortParams[HwPortIndex].PhyTransceiver;
        }
        else
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_ll_GetPhyType, HwPortIndex invalid, HwPortIndex:0x%X", HwPortIndex);
            *pPhyType = EDDI_PHY_TRANSCEIVER_NOT_DEFINED;
        }
    }
    else
    {
        EDDI_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_ll_GetPhyType, pDDB not found, HwPortIndex:0x%X Status:0x%X", HwPortIndex, Status);
        *pPhyType = EDDI_PHY_TRANSCEIVER_NOT_DEFINED;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SysChangeOwnMACAddr()
 *
 * function:      Change own MAC address
 *
 * parameters:    hDDB    Device handle
 *                pMAC    Pointer to new MAC Adr
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysChangeOwnMACAddr( EDDI_HANDLE                 hDDB,
                                                               EDD_MAC_ADR_TYPE  *  const  pMAC )
{
    LSA_RESULT                                 Status;
    EDDI_RQB_SWI_REMOVE_FDB_ENTRY_TYPE         RemoveUsrLocalMAC;
    EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr: hDDB wrong. 0x%X, 0:0x%X", 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    //check if HW is initialized
    if (!pDDB->Glob.HWIsSetup)
    {
        EDDI_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr: pDDB->Glob.HWIsSetup 0:0x%X, 0:0x%X", 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    // check for open channels
    if (pDDB->Glob.OpenCount)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr: Still %d channels open. 0x%X", pDDB->Glob.OpenCount, 0);
        return EDD_STS_ERR_SEQUENCE;
    }
    
    //only a UC address is allowed
    if (pMAC->MacAdr[0] & 0x1)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr: No UC address. 0x%X:0x%X", pMAC->MacAdr[0], pMAC->MacAdr[1]);
        return EDD_STS_ERR_PARAM;
    }
    
    if (pDDB->pLocal_CRT == EDDI_NULL_PTR)
    {
      EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr: No valid CRT IF. 0x%X. 0x%X", 0, 0);
      return EDD_STS_ERR_PARAM;
    }

    //set IRT-Source-Adress
    EDDI_SERSetIRTSAdress(pMAC, pDDB);

    //remove current local mac adr
    RemoveUsrLocalMAC.MACAddress    = pDDB->Glob.LLHandle.xRT.MACAddressSrc;
    RemoveUsrLocalMAC.FDBEntryExist = EDDI_SWI_FDB_ENTRY_NOT_EXIST;

    Status = EDDI_SERRemoveFDBEntry(&pDDB->Glob.LLHandle, &RemoveUsrLocalMAC, pDDB);
    if (EDD_STS_OK != Status)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr RemoveUsrLocalMAC-> Status:0x%X 0:0x%X",
                               Status, RemoveUsrLocalMAC.MACAddress.MacAdr[5]);
        return Status;
    }

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "eddi_SysChangeOwnMACAdr RemoveUsrLocalMAC-> FDBEntryExist:0x%X 0:0x%X",
                           RemoveUsrLocalMAC.FDBEntryExist, 0);

    Status = EDDI_SERSetOwnMACAdr(pMAC, SER_SWI_RUN_MODE, pDDB);

    if (EDD_STS_OK != Status)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysChangeOwnMACAdr RemoveUsrLocalMAC-> Status:0x%X 0:0x%X",
          Status, pMAC->MacAdr[5]);
        return Status;
    }

    pDDB->Glob.LLHandle.xRT.MACAddressSrc = *pMAC;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if !defined (EDDI_CFG_3BIF_2PROC)
/*=============================================================================
 * function name: eddi_ProviderGetPrimaryAR()
 *
 * function:      Get the primary AR
 *
 * parameters:    hDDB	        Device handle (see EDDI_SRV_DEV_OPEN service).
 *                ARSetID	    ID of the ARSet from which the current AR in 
 *                              state PRIMARY shall be determined.
 *                pPrimaryARID	Pointer to where the returnvalue shall be written to.
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_ProviderGetPrimaryAR( EDDI_HANDLE            hDDB,
                                                                LSA_UINT16      const  ARSetID,
                                                                LSA_UINT16   *  const  pPrimaryARID )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    if ((g_pEDDI_Info  == (EDDI_INFO_TYPE *)0) || (pDDB->hDDB != hDDB) || (pDDB->bUsed == EDDI_DDB_NOT_USED))
    {
        EDDI_Excp("eddi_ProviderGetPrimaryAR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_ProviderGetPrimaryAR->");

    if ((ARSetID > EDD_CFG_MAX_NR_ARSETS)  ||  (ARSetID < 1))
    {
        EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_ProviderGetPrimaryAR, ERROR: ARSetID:0x%X is 0 or bigger then EDD_CFG_MAX_NR_ARSETS:0x%X", ARSetID, EDD_CFG_MAX_NR_ARSETS);
        return EDD_STS_ERR_PARAM;
    }
    
    *pPrimaryARID = pDDB->pGSharedRAM->ARSet[ARSetID];
    
    EDDI_CRT_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "eddi_ProviderGetPrimaryAR, PrimaryARID[0x%X]=0x%X", ARSetID, *pPrimaryARID);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //!defined (EDDI_CFG_3BIF_2PROC)


/*****************************************************************************/
/*  end of file eddi_sys.c                                                   */
/*****************************************************************************/

