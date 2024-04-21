#ifndef EDDI_TRAC_INT_H         //reinclude-protection
#define EDDI_TRAC_INT_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_trac_int.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  TRACE-                                                                   */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  03.07.02    JS    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDD functions uses the following IDs                      */
/*                                                                           */
/*        EDD:   0x00 .. 0x0F                                                */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*                                                                           */
/*        Each component uses the followin module IDs (defined in component) */
/*                                                                           */
/*        NRT:   0x80 .. 0x8F                                                */
/*        SRT:   0x90 .. 0x9F                                                */
/*        IRT:   0xA0 .. 0xAF                                                */
/*        SWI:   0xB0 .. 0xBF                                                */
/*                                                                           */
/* ========================================================================= */

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/
/*********************************************************************************/
/* The diagnostic data from the IRTTraceUnit are copied from IRTE do user RAM by */
/* DMA. These data are stored in little-endian format.                           */
/* If EDDI_CFG_BIG_ENDIAN is selected, consider the following:                   */
/*   - no bitfield-access                                                        */
/*   - swap 16- and 32bit values                                                 */
/* 1 trace entry consists of 128bit (HW0 + HW1)                                  */
/*********************************************************************************/

/****** HW0  *****/
typedef struct _EDDI_RQB_TRACE_DIAG_UINT64_TYPE /* 64 Bit */
{
    LSA_UINT32   U32_0;
    LSA_UINT32   U32_1;

} EDDI_RQB_TRACE_DIAG_UINT64_TYPE;

typedef struct _EDDI_RQB_TRACE_DIAG_LL0_STRUCT /* 64 Bit */
{
    LSA_UINT16   TRACE_DIAG_DQ;  /*16 Bit */
    LSA_UINT16   FrameId;        /*16 Bit */
    LSA_UINT32   CycleTime;      /*32 Bit */

} EDDI_RQB_TRACE_DIAG_LL0_STRUCT;

typedef union _EDDI_RQB_TRACE_DIAG_LL0_TYPE
{
    EDDI_RQB_TRACE_DIAG_UINT64_TYPE   Value2_32;     //Acccess via 2*32bit value
    EDDI_RQB_TRACE_DIAG_LL0_STRUCT    StructField;   //Access by structure elements

} EDDI_RQB_TRACE_DIAG_LL0_TYPE;

/****** HW1  *****/
typedef struct _EDDI_RQB_TRACE_DIAG_LL1_STRUCT    /* 64 Bit */
{
    LSA_UINT8   SourceMacAdr[EDD_MAC_ADDR_SIZE]; /* 48 Bit */

    LSA_UINT16  LostEntry_Port_CycleNumber;      /* 16 Bit */

} EDDI_RQB_TRACE_DIAG_LL1_STRUCT;

typedef union _EDDI_RQB_TRACE_DIAG_LL1_TYPE        /* 64 Bit */
{
    EDDI_RQB_TRACE_DIAG_UINT64_TYPE   Value_2_32;     //Acccess via 2*32bit value
    EDDI_RQB_TRACE_DIAG_LL1_STRUCT    StructField;    //Access by structure elements

} EDDI_RQB_TRACE_DIAG_LL1_TYPE;

/****** HW0 + HW1  *****/
#define MAX_TRACE_DIAG_ENTRIES   0x40
typedef struct _EDDI_TRACE_DIAG_ENTRY_TYPE     /* 128 Bit */
{
    EDDI_RQB_TRACE_DIAG_LL0_TYPE  Hw0;               /* 64 Bit */
    EDDI_RQB_TRACE_DIAG_LL1_TYPE  Hw1;               /* 64 Bit */

} EDDI_TRACE_DIAG_ENTRY_TYPE;

typedef EDDI_TRACE_DIAG_ENTRY_TYPE   EDDI_LOWER_MEM_ATTR * EDDI_TRACE_DIAG_ENTRY_PTR_TYPE;


/**** traceunit state machine ******/
typedef struct _TRACE_DSCR_TYPE
{
    EDD_UPPER_RQB_PTR_TYPE      pRQB;
    EDDI_LOCAL_TCW_PTR_TYPE     pTCW;

} TRACE_DSCR_TYPE;

typedef enum _TRACE_DIAG_STATE_TYPE
{
    TRACE_DIAG_STATE_PRM_NO_RQB,
    TRACE_DIAG_STATE_PRM_WAIT_FOR_DIAG_ENTRY

} TRACE_DIAG_STATE_TYPE;

typedef enum _TRACE_DIAG_EVENT_TYPE
{
    TRACE_DIAG_EVENT_IRQ,
    TRACE_DIAG_EVENT_PRM_IND_FROM_USR

} TRACE_DIAG_EVENT_TYPE;

/*===========================================================================*/
/* TRACE handle (channel) management-structures                              */
/*===========================================================================*/

//typedef EDDI_LOCAL_MEM_PTR_TYPE  EDDI_LOCAL_HDB_COMP_TRACE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Trace-parameters                                                          */
/*---------------------------------------------------------------------------*/
/* On order to trace the normal receive events "IRTRcvData", "IRTRcvTime" and "IRTRcvSubstitute"
   the Enable-Bit has to be set !! additionally!! in the respective FCW-table. The adequate setting
   of the Enable-Bit is done by an entry in "RcvDataTimeSubEnable". Furthermore it is possible to 
   assigen a role (Consumer, Forwarder, etc.), for which the normal events have to be recorded.      */
typedef struct _EDDI_TRACE_DIAG_IRT_SRT_ENABLE_TYPE
{
    LSA_BOOL       IRTConsumer;   // Record normal events for IRT-Consumer   
    LSA_BOOL       IRTForwarder;  // Record normal events for IRT-Forwarder  
    LSA_BOOL       IRTSyncSlave;  // Record normal events for IRT-Sync-Slave 
    LSA_BOOL       SRTConsumer;   // Record normal events for SRT-Consumer   
    LSA_BOOL       SRTSyncSlave;  // Record normal events for SRT-Sync-Slave 

} EDDI_TRACE_DIAG_IRT_SRT_ENABLE_TYPE;

/*---------------------------------------------------------------------------*/
/* DDB-TRACE                                                                 */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_DDB_COMP_TRACE_TYPE
{
    LSA_UINT32                           UsrHandleCnt;          /* Number of handles using */
    EDDI_LOCAL_HDB_PTR_TYPE              pHDB;                  /* HDB-Pointer             */

    EDDI_MEMORY_MODE_TYPE                TraceUsrRAMMemMode;    /* Memory-Mode: SDRAM, PC-Shared-Memory for Usr-Buffer    */
    EDDI_USERMEMID_TYPE                  TraceUsrMemID;

    LSA_UINT16                           TraceTransferLimit;    /* ThresHold for Interrupt                                */
    EDD_UPPER_MEM_U8_PTR_TYPE            pDev_TrcEntries;       /* NOT 64-Bit-Aligned */
    TRACE_DSCR_TYPE                      Pending;               /* Current RQB from indication */
    EDDI_RQB_QUEUE_TYPE                  IndiQueueReq;          /* All RQB's from indication */
    EDDI_TRACE_DIAG_IRT_SRT_ENABLE_TYPE  RcvDataTimeSubEnable;  /* Enables FCWs / ACW-Types for Trace-Diagnose-Generation */
    TRACE_DIAG_STATE_TYPE                TraceDiagState;        /* State of TraceDiag                                     */
    LSA_VOID  EDDI_LOWER_MEM_ATTR     *  pTracePRMDataBuffer;   /* Buffer for diagnostic data if PRM is running           */
    DEV_NRT_ADR_CONVERT_FCT               adr_to_asic;
    LSA_UINT32                            LimitDown_BufferAddr;
    LSA_UINT32                            LimitUp_BufferAddr;
                                      
    LSA_UINT32                            NrOfDiagEntries;
    LSA_UINT32                            KramDiagSize;

} EDDI_DDB_COMP_TRACE_TYPE;

typedef EDDI_DDB_COMP_TRACE_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_LOCAL_DDB_COMP_TRACE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_TRAC_INT_H


/*****************************************************************************/
/*  end of file eddi_trac_int.h                                              */
/*****************************************************************************/
