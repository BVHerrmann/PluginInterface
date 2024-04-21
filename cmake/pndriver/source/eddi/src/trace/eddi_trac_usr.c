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
/*  F i l e               &F: eddi_trac_usr.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Trace for EDDI.                                  */
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
#include "eddi_trac_ext.h"
#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_TRAC_USR
#define LTRC_ACT_MODUL_ID  28

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                local Variables                            */
/*===========================================================================*/

#define   TRACE_DIAG_IRTRcvData           0x00000001
#define   TRACE_DIAG_IRTRcvTime           0x00000002
#define   TRACE_DIAG_IRTRcvSubstitute     0x00000004
#define   TRACE_DIAG_SRTRcvErrLate        0x00000008
#define   TRACE_DIAG_IRTRcvErrNRT         0x00000010
#define   TRACE_DIAG_IRTRcvErrMissing     0x00000020
#define   TRACE_DIAG_IRTRcvErraSRT        0x00000040
#define   TRACE_DIAG_IRTRcvErrUnexpected  0x00000080
#define   TRACE_DIAG_IRTRcvErrCRC         0x00000100
#define   TRACE_DIAG_IRTRcvErrDataLost    0x00000200
#define   TRACE_DIAG_IRTRcvErrLength      0x00000400
#define   TRACE_DIAG_IRTRcvErrNoACW       0x00000800
#define   TRACE_DIAG_RRTErrNoTableEntry   0x00001000
#define   TRACE_DIAG_IRTRcvErrBufOvfl     0x00002000
#define   TRACE_DIAG_SRTRcvErrNoACW       0x00004000
#define   TRACE_DIAG_IRTSndErrNoData      0x00002000
#define   TRACE_DIAG_IRTSndErrLate        0x00004000

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                local constants                            */
/*===========================================================================*/
static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_TRACEWriteTCW          ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_TRACEPRMIndicationToUsr( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_TRACECloseChRes        ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_TraceInitComponent()                   +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE    Service: EDDI_SRV_COMP_SRT_INI                         +*/
/*+   LSA_RESULT    Status:     Return status                               +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_CMP_SRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE       Service: EDDI_SRV_COMP_SRT_INI                      +*/
/*+   LSA_RESULT        Status:  EDD_STS_OK                                 +*/
/*+                              EDD_STS_ERR_SEQUENCE                       +*/
/*+                              EDDI_STS_ERR_RESOURCES                     +*/
/*+                              EDD_STS_ERR_PARAM                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the SRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TraceInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                                 UsrPortIndex;
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;
    LSA_UINT32                                 TrcDiagnosEnable;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TraceInitComponent->");

    //EDDI_MemSet(pTRACEComp, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_COMP_TRACE_TYPE));

    //build diagnostic table in KRAM, 64-Bit aligned base address

    pTRACEComp->NrOfDiagEntries = MAX_TRACE_DIAG_ENTRIES; /* only 64 entries allowed */
    pTRACEComp->KramDiagSize    = (pTRACEComp->NrOfDiagEntries * sizeof(EDDI_TRACE_DIAG_ENTRY_TYPE));

    pTRACEComp->pDev_TrcEntries  = (EDD_UPPER_MEM_U8_PTR_TYPE)(void *)&pDDB->pKramFixMem->TraceDiagEntry[0];
    pDDB->KramRes.Trace          = pTRACEComp->KramDiagSize;

    //check 64 bit alignment
    if ((LSA_UINT32)(void *)pTRACEComp->pDev_TrcEntries & 0x07UL)
    {
        EDDI_Excp("EDDI_TraceInitComponent, wrong alignment!", EDDI_FATAL_ERR_EXCP, pTRACEComp->pDev_TrcEntries, 0);
        return;
    }

    //schon 0  pTRACEComp->UsrHandleCnt           = 0;
    //schon 0  pTRACEComp->pHDB                   = EDDI_NULL_PTR;
    pTRACEComp->TraceUsrRAMMemMode                = pDDB->pLocal_NRT->IF[EDDI_NRT_CHA_IF_0].MemModeBuffer;
    pTRACEComp->TraceUsrMemID                     = pDDB->pLocal_NRT->IF[EDDI_NRT_CHA_IF_0].UserMemIDRXDefault;
    pTRACEComp->TraceTransferLimit                = (LSA_UINT16)0x0010;

    //schon 0  pTRACEComp->Pending.pRQB           = EDDI_NULL_PTR;
    //schon 0  pTRACEComp->IndiQueueReq.pBottom   = EDDI_NULL_PTR;
    //schon 0  pTRACEComp->IndiQueueReq.pTop      = EDDI_NULL_PTR;
    pTRACEComp->IndiQueueReq.Cnt                  = 0;
    pTRACEComp->RcvDataTimeSubEnable.IRTConsumer  = LSA_TRUE; // Record normal events for IRT-Consumer   
    pTRACEComp->RcvDataTimeSubEnable.IRTForwarder = LSA_TRUE; // Record normal events for IRT-Forwarder  
    pTRACEComp->RcvDataTimeSubEnable.IRTSyncSlave = LSA_TRUE; // Record normal events for IRT-Sync-Slave 
    pTRACEComp->RcvDataTimeSubEnable.SRTConsumer  = LSA_TRUE; // Record normal events for SRT-Consumer   
    pTRACEComp->RcvDataTimeSubEnable.SRTSyncSlave = LSA_TRUE; // Record normal events for SRT-Sync-Slave 
    pTRACEComp->TraceDiagState                    = TRACE_DIAG_STATE_PRM_NO_RQB;
    //schon 0  pTRACEComp->pTracePRMDataBuffer    = EDDI_NULL_PTR;

    switch (pTRACEComp->TraceUsrRAMMemMode)
    {
        case MEMORY_SHARED_MEM:
        {
            pTRACEComp->LimitDown_BufferAddr = pDDB->NRTMEM_LowerLimit;
            pTRACEComp->LimitUp_BufferAddr   = pDDB->NRTMEM_UpperLimit;
            pTRACEComp->adr_to_asic          = pDDB->shared_mem_adr_to_asic;
        }
        break;

        case MEMORY_SDRAM_ERTEC:
        {
            pTRACEComp->LimitDown_BufferAddr = pDDB->NRTMEM_LowerLimit;
            pTRACEComp->LimitUp_BufferAddr   = pDDB->NRTMEM_UpperLimit;
            pTRACEComp->adr_to_asic          = pDDB->sdram_mem_adr_to_asic;
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_TraceInitComponent", EDDI_FATAL_ERR_EXCP, pTRACEComp->TraceUsrRAMMemMode, 0);
            return;
        }
    }

    EDDI_DEVAllocMode(pDDB, pTRACEComp->TraceUsrRAMMemMode, &pTRACEComp->pTracePRMDataBuffer, pTRACEComp->KramDiagSize, pTRACEComp->TraceUsrMemID);

    //build in KRAM

    pTRACEComp->Pending.pTCW = &pDDB->pKramFixMem->TCW;
    pDDB->KramRes.TraceTCW   = sizeof(EDDI_SER10_TCW_TYPE);

    //Trace_Base_Adress, //0x00018000
    IO_W32(TRC_BASE_ADR, (SER10_NULL_PTR & DEV_kram_adr_to_asic_register((void *)pTRACEComp->pDev_TrcEntries, pDDB)));

    //Trace_Buffer_Length, //0x00018004
    IO_W32(TRC_LENGTH, (0xFFFF & pTRACEComp->NrOfDiagEntries));

    //Trace_Transfer_Limit, //0x00018008
    IO_W32(TRC_TRANSFER_LIMIT, (0xFFFF & pTRACEComp->TraceTransferLimit));

    //IRT_Diagnose_Enable_P0/P1/.., //0x00008
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        TrcDiagnosEnable = 0;

        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                              "EDDI_TraceInitComponent, UsrPortIndex:0x%X TrcDiagnosEnable:0x%X",
                              UsrPortIndex, TrcDiagnosEnable);

        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_IRT_DIAG, TrcDiagnosEnable, pDDB);
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TraceInitComponent<-");
}
/*===========================================================================*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_TraceRelComponent()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TraceRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TraceRelComponent->");

    if (pTRACEComp->NrOfDiagEntries == 0)
    {
        return; //Switch Modus
    }

    EDDI_DEVFreeMode(pDDB, pTRACEComp->TraceUsrRAMMemMode, pTRACEComp->pTracePRMDataBuffer, pTRACEComp->TraceUsrMemID);

    //free pending.TCW     -> KRAM does not have to be unlocked!
    //free pDev_TrcEntries -> KRAM does not have to be unlocked!

    pTRACEComp->pDev_TrcEntries = EDDI_NULL_PTR;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TraceRelComponent<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*---------------------------------------------------------------------------*/
/* Open/Close-TRAEChannel                                                    */
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_TRACEOpenChannel()                     +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                        :    EDDI_LOCAL_HDB_PTR_TYPE       pHDB          +*/
/*+                             LSA_HANDLE_TYPE               Handle        +*/
/*+                             EDDI_UPPER_CDB_ASRT_PTR_TYPE  pCDB          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel (<= EDDI_CFG_MAX_CHANNELS)        +*/
/*+  pCDB       : Pointer to filled ChannelDescriptionBlock parameters      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_CHANNEL_USE aSRT already in used by a channel +*/
/*+               EDDI_STS_ERR_RESOURCES   Alloc failed                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Opens Channel for acyclic SRT. The Handle will specify    +*/
/*+               the channel and pDDB the Device. pCDB has all SRT         +*/
/*+               parameters to use for this channel                        +*/
/*+                                                                         +*/
/*+               It is not checked if the handle is already open!          +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Only on Channel can use acyclic SRT!                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACEOpenChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEOpenChannel->");

    /*---------------------------------------------------------------------------*/
    /* we support only one TRACE-Channel for each device !                       */
    /*---------------------------------------------------------------------------*/
    if (pTRACEComp->UsrHandleCnt > 1)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_TRACEOpenChannel, EDD_STS_ERR_CHANNEL_USE, pTRACEComp->UsrHandleCnt:0x%X",
                              pTRACEComp->UsrHandleCnt);
        return EDD_STS_ERR_CHANNEL_USE;
    }

    pTRACEComp->UsrHandleCnt++;  //cnt of currently open handles

    pTRACEComp->pHDB = pHDB;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                          "EDDI_TRACEOpenChannel, pTRACEComp->UsrHandleCnt:0x%X",
                          pTRACEComp->UsrHandleCnt);

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEOpenChannel<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_TRACECloseChannel()                         +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                        :    EDDI_LOCAL_HDB_PTR_TYPE       pHDB          +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel to be closed                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE         Channel has pending requests +*/
/*+               EDD_STS_ERR_RESOURCE         Error on freeing memory      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an acyclic SRT-Channel. if there are any pending   +*/
/*+               requests the channel can not be closed.                   +*/
/*+                                                                         +*/
/*+               This function is only called if the handle uses aSRT!     +*/
/*+               (not checked!)                                            +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Note:  If EDD_STS_ERR_RESOURCE is returned the channel is +*/
/*+                      closed but we had an error while freeing memory.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACECloseChannel( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACECloseChannel->");

    if (   (pTRACEComp->pHDB == EDDI_NULL_PTR)
        || (pTRACEComp->UsrHandleCnt != 1))
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_TRACECloseChannel, Trace-Channel was not open.");
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_TRACECloseChRes(pDDB);

    pTRACEComp->UsrHandleCnt--;

    pTRACEComp->pHDB = EDDI_NULL_PTR;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                          "EDDI_TRACECloseChannel, pTRACEComp->UsrHandleCnt:0x%X",
                          pTRACEComp->UsrHandleCnt);

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACECloseChannel<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_TRACECloseChRes()
*
* function:
*
* parameters:     
*
* return value:  LSA_VOID
*                                       
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRACECloseChRes( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACECloseChRes->");

    //sendback all RQBs in queue
    for (;;)
    {
        EDD_UPPER_RQB_PTR_TYPE  const  pRQB = EDDI_RemoveFromQueue(pDDB, &pTRACEComp->IndiQueueReq);

        if (!(pRQB == EDDI_NULL_PTR))
        {
            EDDI_RequestFinish(pTRACEComp->pHDB, pRQB, EDD_STS_OK_CANCEL);
            pTRACEComp->IndiQueueReq.Cnt--;
        }
        else
        {
            break; //leave for-loop
        }
    }

    //sendback pending RQB
    if (!(pTRACEComp->Pending.pRQB == EDDI_NULL_PTR))
    {
        EDDI_RequestFinish(pTRACEComp->pHDB, pTRACEComp->Pending.pRQB, EDD_STS_OK_CANCEL);
    }

    pTRACEComp->Pending.pRQB         = EDDI_NULL_PTR;
    pTRACEComp->IndiQueueReq.pBottom = EDDI_NULL_PTR;
    pTRACEComp->IndiQueueReq.pTop    = EDDI_NULL_PTR;
    pTRACEComp->IndiQueueReq.Cnt     = 0;
    pTRACEComp->TraceDiagState       = TRACE_DIAG_STATE_PRM_NO_RQB;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACECloseChRes<-");
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRACEPRMSetFilterTraceunit( LSA_UINT32               const  HwPortIndex,
                                                                LSA_UINT32               const  DiagnosticEnable,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  TrcDiagnosEnable;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEPRMSetFilterTraceunit->");

    TrcDiagnosEnable = 0;

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvData,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_DATA)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvTime          ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_TIME)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvSubstitute    ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_SUBSTITUTE)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__SRTRcvErrLate       ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_LATE)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrNRT        ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NRT)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrMissing    ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_MISSING)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErraSRT       ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_SRT)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrUnexpected ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_UNEXPECTED)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrCRC        ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_CRC)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrDataLost   ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_DATA_LOST)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrLength     ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_LENGTH)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrNoACW      ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_ACW)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__RRTErrNoTableEntry  ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_TABLE_ENTRY)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrBufOvfl    ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_BUFFER_OVERFLOW)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__SRTRcvErrNoACW      ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_NO_ACW)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTSndErrNoData     ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_NO_DATA)) ? 1:0);  // 1-Bit

    EDDI_SetBitField32(&TrcDiagnosEnable, EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTSndErrLate       ,
                        (0 != (DiagnosticEnable & EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_LATE)) ? 1:0);  // 1-Bit

    EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_IRT_DIAG, TrcDiagnosEnable, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_TRACEPRMIndicationFromUsr()
*
* function:
*
* parameters:     EDDI_LOCAL_DDB_PTR_TYPE   pDDB
*
* return value:  LSA_RESULT    EDD_STS_OK
*                              EDDI_STS_ERR_HANDLE
*                              EDD_STS_ERR_RESOURCE
*
*     All other RQB-parameters depend on kind of request.

*===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACEPRMIndicationFromUsr( EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_BOOL                     *  bIndicate )
{
    LSA_RESULT                                 Status;
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEPRMIndicationFromUsr->");

    *bIndicate = LSA_TRUE;

    if (pTRACEComp->pTracePRMDataBuffer == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_TRACEPRMIndicationFromUsr", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_RESOURCE;
    }

    //Enqueu indication to queue "IndiQueueReq"
    EDDI_AddToQueueEnd(pDDB, &pTRACEComp->IndiQueueReq, pRQB);

    pTRACEComp->IndiQueueReq.Cnt++;

    //Output the trace contents immediately
    Status = EDDI_TRACETrigger(pDDB, TRACE_DIAG_EVENT_PRM_IND_FROM_USR);

    //Everything ok, do not indicate
    *bIndicate = LSA_FALSE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEPRMIndicationFromUsr<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_TRACETrigger()
*
* function:
*
* parameters:    
*
* return value:  LSA_RESULT     EDD_STS_OK
*                               EDDI_STS_ERR_HANDLE
*                               EDD_STS_ERR_RESOURCE
*
*===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACETrigger( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    TRACE_DIAG_EVENT_TYPE    const  Event )
{
    LSA_RESULT                                 Status     = LSA_RET_ERR_RESOURCE;
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT,
                           "EDDI_TRACETrigger->pTRACEComp->TraceDiagState:0x%X Event:0x%X",
                           pTRACEComp->TraceDiagState, Event);

    switch (pTRACEComp->TraceDiagState)
    {
        case TRACE_DIAG_STATE_PRM_NO_RQB: //No RQB for TCW available
        {
            if (Event == TRACE_DIAG_EVENT_PRM_IND_FROM_USR) //New RQB indication sent
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                      "EDDI_TRACETrigger, EVENT_IND_PROVIDE_FROM_USR, pTRACEComp->IndiQueueReq.Cnt:0x%X",
                                      pTRACEComp->IndiQueueReq.Cnt);

                Status = EDDI_TRACEWriteTCW(pDDB);
                if (EDD_STS_OK == Status)
                {
                    //Output diagnostic entries immediately
                    IO_x32(TRC_MODE) = TRC_EXECUTE;
                    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACETrigger, TRACE_DIAG_STATE_OPERATE bDoTrigger");

                    pTRACEComp->TraceDiagState = TRACE_DIAG_STATE_PRM_WAIT_FOR_DIAG_ENTRY;
                }
            }
            else
            {
                EDDI_Excp("EDDI_TRACETrigger 3", EDDI_FATAL_ERR_EXCP, pTRACEComp->TraceDiagState, Event);
                return EDD_STS_ERR_EXCP;
            }
        }
        break;

        case TRACE_DIAG_STATE_PRM_WAIT_FOR_DIAG_ENTRY: //RQB for TCW available
        {
            if (TRACE_DIAG_EVENT_IRQ == Event)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACETrigger, EVENT_IRQ, IndiQueueReq.Cnt:0x%X",
                                      pTRACEComp->IndiQueueReq.Cnt);

                //Indicate to user
                Status = EDDI_TRACEPRMIndicationToUsr(pDDB);

                //RQB indications still available, only one rqb allowed
                if (pTRACEComp->IndiQueueReq.Cnt >= 1)
                {
                    EDDI_Excp("EDDI_TRACETrigger 4", EDDI_FATAL_ERR_EXCP, pTRACEComp->TraceDiagState, pTRACEComp->IndiQueueReq.Cnt);
                    return EDD_STS_ERR_EXCP;
                }

                pTRACEComp->TraceDiagState = TRACE_DIAG_STATE_PRM_NO_RQB;
            }
            else
            {
                EDDI_Excp("EDDI_TRACETrigger 5", EDDI_FATAL_ERR_EXCP, pTRACEComp->TraceDiagState, Event);
                return EDD_STS_ERR_EXCP;
            }
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_TRACETrigger 6", EDDI_FATAL_ERR_EXCP, pTRACEComp->TraceDiagState, Event);
            return EDD_STS_ERR_EXCP;
        }
    }

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT,
                           "EDDI_TRACETrigger<-pTRACEComp->TraceDiagState:0x%X Event:0x%X",
                           pTRACEComp->TraceDiagState, Event);

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_TRACEWriteTCW()
*
* function:
*
* parameters:    
*
* return value:  LSA_RESULT    EDD_STS_OK
*                              EDDI_STS_ERR_HANDLE
*                              EDD_STS_ERR_RESOURCE
*
*===========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACEWriteTCW( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                 Status;
    EDDI_LOCAL_HDB_PTR_TYPE                    pHDB;
    EDD_UPPER_RQB_PTR_TYPE                     pRQB;
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE  const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEWriteTCW->");

    //already one TCW executed
    if (!(pTRACEComp->Pending.pRQB == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_TRACEWriteTCW 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return LSA_RET_ERR_RESOURCE;
    }

    //take indication-RQB from queue 
    pRQB = EDDI_RemoveFromQueue(pDDB, &pTRACEComp->IndiQueueReq);
    if (pRQB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_TRACEWriteTCW 1", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_RESOURCE;
    }

    pTRACEComp->IndiQueueReq.Cnt--;

    //pass RQB over
    pTRACEComp->Pending.pRQB = pRQB;

    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_Excp("EDDI_TRACEWriteTCW 2", EDDI_FATAL_ERR_EXCP, Status, 0);
        return Status;
    }

    //execute TCW 
    pTRACEComp->Pending.pTCW->L0.Value = 0;
    EDDI_SetBitField32(&pTRACEComp->Pending.pTCW->L0.Value, EDDI_SER10_L0_TCW_BIT__Reserved1, 0);
    EDDI_SetBitField32(&pTRACEComp->Pending.pTCW->L0.Value, EDDI_SER10_L0_TCW_BIT__Tcw_F_BIT, 0);
    EDDI_SetBitField32(&pTRACEComp->Pending.pTCW->L0.Value, EDDI_SER10_L0_TCW_BIT__Tcw_D_BIT, 0);
    EDDI_SetBitField32(&pTRACEComp->Pending.pTCW->L0.Value, EDDI_SER10_L0_TCW_BIT__NrOfDiag,
                        pTRACEComp->NrOfDiagEntries); //How many entries shall be transfered?

    EDDI_SetBitField32(&pTRACEComp->Pending.pTCW->L0.Value, EDDI_SER10_L0_TCW_BIT__Reserved2, 0);

    if (pTRACEComp->pTracePRMDataBuffer == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_TRACEWriteTCW 3", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_RESOURCE;
    }

    pTRACEComp->Pending.pTCW->L1.DestAdr = EDDI_SWAP_32_FCT(pTRACEComp->adr_to_asic(pDDB->hSysDev, pTRACEComp->pTracePRMDataBuffer, pDDB->ERTEC_Version.Location));

    IO_W32(TRC_COMMAND, pDDB->kram_adr_loc_to_asic_dma(pDDB->hSysDev, pTRACEComp->Pending.pTCW, pDDB->ERTEC_Version.Location));

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACEWriteTCW, L0.Value:0x%X L1.DestAdr:0x%X",
                          pTRACEComp->Pending.pTCW->L0.Value, pTRACEComp->Pending.pTCW->L1.DestAdr);

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEWriteTCW<-Status:0x%X", Status);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_TRACEPRMIndicationToUsr()
*
* function:
*
* parameters:    
*
* return value:  LSA_RESULT     EDD_STS_OK
*                               EDDI_STS_ERR_HANDLE
*                               EDD_STS_ERR_RESOURCE
*
*===========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACEPRMIndicationToUsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16                                         Temp16, RecvDiagEntryCnt;
    LSA_UINT32                                         Temp32, DiagnosticState, DiagnosticStateTemp, DataBufferIndex, TCWL0Value;
    EDDI_PRM_RECORD_PDTRACE_UNIT_DATA_TYPE             TraceUnitDataLocal;
    EDD_UPPER_PRM_READ_PTR_TYPE                        pPrmRead;
    EDDI_TRACE_DIAG_ENTRY_PTR_TYPE                     pDiagEnty;
    EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE          const  pTRACEComp = &pDDB->PRM.TRACE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEPRMIndicationToUsr->");

    //TraceIndication is called from INT -> higher parameter-check (hardware error)

    //No TCW executed yet
    if (pTRACEComp->Pending.pRQB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_TRACEPRMIndicationToUsr 1", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pTRACEComp->Pending.pRQB->pParam, LSA_NULL))
    {
        EDDI_Excp("EDDI_TRACEPRMIndicationToUsr 2", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_SEQUENCE;
    }

    //read pending TCW
    TCWL0Value = pTRACEComp->Pending.pTCW->L0.Value;

    if (0 == (EDDI_GetBitField32(TCWL0Value, EDDI_SER10_L0_TCW_BIT__Tcw_D_BIT)))
    {
        EDDI_Excp("EDDI_TRACEPRMIndicationToUsr 3, D-Bit == 0, Command still active", EDDI_FATAL_ERR_EXCP, TCWL0Value, 0);
        return EDD_STS_ERR_HW;
    }

    if (1 == (EDDI_GetBitField32(TCWL0Value, EDDI_SER10_L0_TCW_BIT__Tcw_F_BIT)))
    {
        EDDI_Excp("EDDI_TRACEPRMIndicationToUsr 4, F-Bit == 1, At the processing of TCW a fault has appeared", EDDI_FATAL_ERR_EXCP, TCWL0Value, 0);
        return EDD_STS_ERR_HW;
    }

    pPrmRead       = (EDD_UPPER_PRM_READ_PTR_TYPE)pTRACEComp->Pending.pRQB->pParam;

    switch (pDDB->ERTEC_Version.Location)
    {
        case EDDI_LOC_FPGA_XC2_V8000:
            Temp16 = EDDI_LOC_FPGA_XC2_V8000;
            break;
        case EDDI_LOC_ERTEC400:
            Temp16 = EDDI_LOC_ERTEC400;
            break;
        case EDDI_LOC_ERTEC200:
            Temp16 = EDDI_LOC_ERTEC200;
            break;
        case EDDI_LOC_SOC1:
            Temp16 = EDDI_LOC_SOC1;
            break;
        case EDDI_LOC_SOC2:
            Temp16 = EDDI_LOC_SOC2;
            break;
        default:
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_TRACEPRMIndicationToUsr, pDDB->ERTEC_Version.Location:0x%X",
                                   pDDB->ERTEC_Version.Location);
            EDDI_Excp("EDDI_TRACEPRMIndicationToUsr, pDDB->ERTEC_Version.Location", EDDI_FATAL_ERR_EXCP, pDDB->ERTEC_Version.Location, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    TraceUnitDataLocal.HardwareType                 = EDDI_HTONS(Temp16);

    Temp16                                       = (LSA_UINT16)pTRACEComp->NrOfDiagEntries;
    TraceUnitDataLocal.HardwareBufferCnt         = EDDI_HTONS(Temp16);

    Temp16                                       = (LSA_UINT16)IO_R32(TRC_COUNT_FAULT);
    TraceUnitDataLocal.LostDiagEntryCnt          = EDDI_HTONS(Temp16);

    RecvDiagEntryCnt                             = (LSA_UINT16)EDDI_GetBitField32(TCWL0Value, EDDI_SER10_L0_TCW_BIT__NrOfDiag);

    TraceUnitDataLocal.RecvDiagEntryCnt          = EDDI_HTONS(RecvDiagEntryCnt);

    pPrmRead->record_data_length                 = EDDI_PRM_ENTRY_PD_TRACE_UNIT_DATA_EMPTY + (RecvDiagEntryCnt * EDDI_PRM_LENGTH_PD_TRACE_UNIT_DATA_ENTRY);

    TraceUnitDataLocal.BlockHeader.BlockType     = EDDI_PRM_BLOCKTYPE_PD_TRACE_UNIT_DATA;

    Temp16                                       = (LSA_UINT16)(pPrmRead->record_data_length - 4UL);
    TraceUnitDataLocal.BlockHeader.BlockLength      = EDDI_HTONS(Temp16);

    TraceUnitDataLocal.BlockHeader.BlockVersionHigh = EDDI_PRM_BLOCK_VERSION_HIGH;
    TraceUnitDataLocal.BlockHeader.BlockVersionLow  = EDDI_PRM_BLOCK_VERSION_LOW;
    TraceUnitDataLocal.BlockHeader.Padding1_1       = 0;
    TraceUnitDataLocal.BlockHeader.Padding1_2       = 0;

    //compare access with LE value
    pDiagEnty = (EDDI_TRACE_DIAG_ENTRY_PTR_TYPE)pTRACEComp->pTracePRMDataBuffer;

    for (DataBufferIndex = 0; DataBufferIndex < RecvDiagEntryCnt; DataBufferIndex++)
    {
        EDDI_TRACE_DIAG_ENTRY_PTR_TYPE            const  pActDiagEntry = &pDiagEnty[DataBufferIndex];
        EDDI_PRM_RECORD_DIAGNOSTIC_ENTRY_TYPE  *  const  pPrmDiagEntry = &TraceUnitDataLocal.DiagEntry[DataBufferIndex];

        if (DataBufferIndex >= EDDI_PRM_MAX_ENTRY_PDTRACE_UNIT_DATA)
        {
          EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_TRACEPRMIndicationToUsr ERROR: DataBufferIndex(0x%X) >= EDDI_PRM_MAX_ENTRY_PDTRACE_UNIT_DATA(0x%X)",
            DataBufferIndex, EDDI_PRM_MAX_ENTRY_PDTRACE_UNIT_DATA);
          break;
        }

        DiagnosticState     = EDDI_GetBitField32(pActDiagEntry->Hw0.Value2_32.U32_0, EDDI_SER10_L0_TDE_BIT__DiagnoseStatus);

        DiagnosticStateTemp = 0;

        if (!EDDI_GetBitField32(pActDiagEntry->Hw0.Value2_32.U32_0, EDDI_SER10_L0_TDE_BIT__DQ))
        {
            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvData) )        ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_DATA : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvTime) )        ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_TIME : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvSubstitute) )  ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_SUBSTITUTE : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_SRTRcvErrLate) )     ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_LATE : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrNRT) )      ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NRT : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrMissing) )  ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_MISSING : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErraSRT) )     ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_SRT : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrUnexpected))? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_UNEXPECTED : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrCRC) )      ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_CRC : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrDataLost) ) ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_DATA_LOST : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrLength) )   ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_LENGTH : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrNoACW) )    ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_ACW : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_RRTErrNoTableEntry) )? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_TABLE_ENTRY : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTRcvErrBufOvfl) )  ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_BUFFER_OVERFLOW : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_SRTRcvErrNoACW) )    ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_NO_ACW : 0);
        }
        else
        {
            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTSndErrNoData) )   ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_NO_DATA : 0);

            DiagnosticStateTemp  |=
                ( (0 != (DiagnosticState & TRACE_DIAG_IRTSndErrLate) )     ? EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_LATE : 0);
        }

        pPrmDiagEntry->DiagnosticState      = EDDI_HTONL(DiagnosticStateTemp);

        Temp16                              = (LSA_UINT16)EDDI_GetBitField32(pActDiagEntry->Hw0.Value2_32.U32_0, EDDI_SER10_L0_TDE_BIT__FrameID);
        pPrmDiagEntry->FrameID              = EDDI_HTONS(Temp16);

        Temp32                              = pDiagEnty[DataBufferIndex].Hw0.StructField.CycleTime * 10;
        pPrmDiagEntry->TimeNS_SrcIPAddr     = EDDI_HTONL(Temp32);

        pPrmDiagEntry->SrcMACAddr[0]        = pActDiagEntry->Hw1.StructField.SourceMacAdr[0];
        pPrmDiagEntry->SrcMACAddr[1]        = pActDiagEntry->Hw1.StructField.SourceMacAdr[1];
        pPrmDiagEntry->SrcMACAddr[2]        = pActDiagEntry->Hw1.StructField.SourceMacAdr[2];
        pPrmDiagEntry->SrcMACAddr[3]        = pActDiagEntry->Hw1.StructField.SourceMacAdr[3];
        pPrmDiagEntry->SrcMACAddr[4]        = pActDiagEntry->Hw1.StructField.SourceMacAdr[4];
        pPrmDiagEntry->SrcMACAddr[5]        = pActDiagEntry->Hw1.StructField.SourceMacAdr[5];

        pPrmDiagEntry->PortID               = (LSA_UINT8)  (EDDI_GetBitField32(pActDiagEntry->Hw1.Value_2_32.U32_1,
                                                                                EDDI_SER10_L3_TDE_BIT__Port) + 1);

        Temp16                              = (LSA_UINT16) (EDDI_GetBitField32(pActDiagEntry->Hw1.Value_2_32.U32_1,
                                                                                EDDI_SER10_L3_TDE_BIT__CycleNumber));
        pPrmDiagEntry->CycleNumber          = EDDI_HTONS(Temp16);

        pPrmDiagEntry->Reserved_1           = 0;
        pPrmDiagEntry->Reserved_2           = 0UL;
    } //end of for-loop

    EDDI_MemCopy(pPrmRead->record_data, &TraceUnitDataLocal, pPrmRead->record_data_length);

    //indicate to user
    EDDI_RequestFinish(pTRACEComp->pHDB, pTRACEComp->Pending.pRQB, EDD_STS_OK);

    pTRACEComp->Pending.pRQB = EDDI_NULL_PTR;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TRACEPRMIndicationToUsr<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_trac_usr.c                                              */
/*****************************************************************************/



