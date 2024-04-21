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
/*  F i l e               &F: eddi_io_provctrl.c                        :F&  */
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
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date        Who   What                                                    */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_IO_PROV_CTRL
#define LTRC_ACT_MODUL_ID  500

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_IO_PROV_CTRL) //satisfy lint!
#endif

#include "eddi_dev.h"
//#include "eddi_crt_ext.h"
#include "eddi_io_provctrl.h"
//#include "eddi_ser_cmd.h"

/*===========================================================================*/
/*                              Vars                                         */
/*===========================================================================*/
EDDI_IO_CB_TYPE   eddi_IOCB[EDDI_CFG_MAX_DEVICES];
EDDI_IO_CB_TYPE * eddi_IOInstanceList[EDD_INTERFACE_ID_MAX+1];
LSA_INT32         EDDI_Lock_Sema_IO[EDD_INTERFACE_ID_MAX+1];
#if defined (EDDI_CFG_REV5)
LSA_INT32         EDDI_Lock_Sema_KRAM[EDD_INTERFACE_ID_MAX+1];
#endif

/*===========================================================================*/
/*                              Local Macros                                 */
/*===========================================================================*/
#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

//#define EDDI_IO_CLOSE2SPEC
#if defined (EDDI_IO_CLOSE2SPEC)
#define EDDI_IO_CHANGE_BUFFER_STATES_PART1 {                                                \
pProvider->offset_F_buffer = pProvider->offset_D2F_buffer;  /* D2F->F  F=D2F */             \
pProvider->offset_D_buffer = pProvider->offset_U2D_buffer;  /* U2D->D  D=U2D */             \
pProvider->offset_D2F_buffer = EDD_DATAOFFSET_UNDEFINED;    /* D2F=0         */             \
pProvider->offset_U2D_buffer = EDD_DATAOFFSET_UNDEFINED;    /* U2D=0         */             \
}

//Do not change sequence!
#define EDDI_IO_CHANGE_BUFFER_STATES_PART4 {                                              \
pProvider->offset_D2F_buffer = pProvider->offset_D_buffer;  /* D->D2F  D2F=D */           \
pProvider->offset_U2D_buffer = pProvider->offset_U_buffer;  /* U->U2D  U2D=U */           \
pProvider->offset_U_buffer   = pProvider->offset_F_buffer;    /* F->U    U=F   */         \
pProvider->offset_F_buffer     = EDD_DATAOFFSET_UNDEFINED;    /* 0->F    F=0   */         \
pProvider->offset_D_buffer     = EDD_DATAOFFSET_UNDEFINED;    /* 0->D    D=0   */         \
}

#else
#define EDDI_IO_CHANGE_BUFFER_STATES_PART1

//Do not change sequence!
#define EDDI_IO_CHANGE_BUFFER_STATES_PART4 {                                              \
LSA_UINT32 const temp_buffer = pProvider->offset_D2F_buffer;                              \
pProvider->offset_D2F_buffer = pProvider->offset_U2D_buffer;  /* U2D->D2F  D2F=D=U2D */   \
pProvider->offset_U2D_buffer = pProvider->offset_U_buffer;    /* U->U2D  U2D=U       */   \
pProvider->offset_U_buffer   = temp_buffer;                   /* D2F->U    U=F=D2F   */   \
}

#endif
#endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/
static LSA_RESULT EDDI_IO_GET_IOCB_PROVCTRL(EDDI_IO_CB_TYPE **ppIOCB, LSA_UINT32 InstanceHandle)
{
    EDDI_IO_CB_TYPE * pIOCB = (EDDI_IO_CB_TYPE*)0;
    if( (LSA_HOST_PTR_ARE_EQUAL(LSA_NULL,ppIOCB)) )
    {
        return (EDD_STS_ERR_PARAM);
    }
    if ((InstanceHandle) > EDD_INTERFACE_ID_MAX)
    {
        (ppIOCB) = LSA_NULL;
        return (EDD_STS_ERR_PARAM);
    }

    pIOCB = eddi_IOInstanceList[InstanceHandle];
    if (   LSA_HOST_PTR_ARE_EQUAL(LSA_NULL,pIOCB)
        || (EDD_DATAOFFSET_INVALID == (LSA_UINT32) pIOCB))
    {
        return (EDD_STS_ERR_SEQUENCE);
    }
    
    if (pIOCB->InstanceHandle != InstanceHandle)
    {
        EDDI_Excp("eddi_BufferRequest_xxx ERROR:illegal InstanceHandle ", EDDI_FATAL_ERR_EXCP, __LINE__, (InstanceHandle));
        return (EDD_STS_ERR_PARAM);
    }

    *ppIOCB = pIOCB;
    return (EDD_STS_OK);
}

#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_BufferXchangeDummy( EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  const  pProvider,
                                                                   LSA_UINT32                        *  const  pOffsetDB,
                                                                   EDDI_IO_CB_TYPE                   *  const  pIOCB,
                                                                   LSA_UINT32                           const  InstanceHandle)
{
    LSA_UNUSED_ARG(pProvider);
    LSA_UNUSED_ARG(pOffsetDB);
    LSA_UNUSED_ARG(pIOCB);
    LSA_UNUSED_ARG(InstanceHandle);
    
    return EDD_STS_ERR_PARAM;
}

/* Buffer exchange for Class1/2-Providers */
static  LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_BufferXchangeClass12( EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  const  pProvider,
                                                                        LSA_UINT32                        *  const  pOffsetDB,
                                                                        EDDI_IO_CB_TYPE                   *  const  pIOCB,
                                                                        LSA_UINT32                           const  InstanceHandle )
{
    LSA_UINT32  cur_clk_cnt_begin;

    EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_BufferXchangeClass12 -> ProvID:0x%X pOffsetDB:0x%X",
                      (LSA_UINT32)pProvider->provider_id, (LSA_UINT32)*pOffsetDB);

    EDDI_ENTER_IO_S(InstanceHandle);

    if /* wrong parameter or provider has been removed */
       (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_EXIT_IO_S(InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }

    if /* wrong buffer */
       (pProvider->offset_U_buffer != *pOffsetDB)
    {
        EDDI_EXIT_IO_S(InstanceHandle);
        return EDD_STS_ERR_PARAM;
    }

    cur_clk_cnt_begin = (LSA_UINT32)EDDI_IO_R32(CLK_COUNT_BEGIN_VALUE);

    if /* at least 1 complete cycle has passed since last xchange  */
       (cur_clk_cnt_begin != pProvider->clk_cnt_begin)
    {
        /* 1. change states */
        #if defined (EDDI_IO_CLOSE2SPEC)
        EDDI_IO_CHANGE_BUFFER_STATES_PART1;

        /* 2. copy APDU-Status from old Data-Buffer to new Data-Buffer */
        //*((LSA_UINT32 *)(pIOCB->pKRAM + *pOffsetDB)) = *((LSA_UINT32 *)(pIOCB->pKRAM + pProvider->offset_D_buffer));
        #else
        EDDI_IO_CHANGE_BUFFER_STATES_PART1;

        /* 2. copy APDU-Status from old Data-Buffer to new Data-Buffer */
        //*((LSA_UINT32 *)(pIOCB->pKRAM + *pOffsetDB)) = *((LSA_UINT32 *)(pIOCB->pKRAM + pProvider->offset_U2D_buffer));
        #endif


        /* 3. insert user-buffer into ACW */
        EDDI_SetBitField32(&(pProvider->pACWSnd->Hw1.Value.U32_0), EDDI_SER10_LL1_ACW_SND_BIT__pDB0,
                            (*pOffsetDB + EDDI_IO_KRAM_TO_REGS_OFFSET)); /* 21 Bit */

        //record moment of exchange - could have changed from check before!
        pProvider->clk_cnt_begin = (LSA_UINT32)EDDI_IO_R32(CLK_COUNT_BEGIN_VALUE);

        /* 4. Change buffer-states */
        EDDI_IO_CHANGE_BUFFER_STATES_PART4;

        /* 5. return new buffer */
        *pOffsetDB = pProvider->offset_U_buffer;

        EDDI_EXIT_IO_S(InstanceHandle);
    }
    else /* called too early */
    {
        EDDI_EXIT_IO_S(InstanceHandle);

        //buffer-ptr is not changed - user gets the same returned
        EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EDDI_BufferXchangeClass12 ERROR: Called too early. No free buffer");
        return EDD_STS_OK_PENDING;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/



/* Buffer exchange for Class3-Providers */
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_BufferXchangeClass3( EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  const  pProvider,
                                                                    LSA_UINT32                        *  const  pOffsetDB,
                                                                    EDDI_IO_CB_TYPE                   *  const  pIOCB,
                                                                    LSA_UINT32                           const  InstanceHandle)
{
    LSA_UINT32  cur_clk_cnt_begin;

    EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_BufferXchangeClass3 -> ProvID:0x%X pOffsetDB:0x%X",
                      (LSA_UINT32)pProvider->provider_id, (LSA_UINT32)*pOffsetDB);

    EDDI_ENTER_IO_S(InstanceHandle);

    if /* wrong parameter or provider has been removed */
       (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_EXIT_IO_S(InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }

    if /* wrong buffer */
       (pProvider->offset_U_buffer != *pOffsetDB)
    {
        EDDI_EXIT_IO_S(InstanceHandle);
        return EDD_STS_ERR_PARAM;
    }

    cur_clk_cnt_begin = (LSA_UINT32)EDDI_IO_R32(CLK_COUNT_BEGIN_VALUE);

    if /* at least 1 complete cycle has passed since last xchange  */
       (cur_clk_cnt_begin != pProvider->clk_cnt_begin)
    {
        /* 1. change states */
        #if defined (EDDI_IO_CLOSE2SPEC)
        EDDI_IO_CHANGE_BUFFER_STATES_PART1;

        /* 2. copy APDU-Status from old Data-Buffer to new Data-Buffer */
        //*((LSA_UINT32 *)(pIOCB->pKRAM + *pOffsetDB)) = *((LSA_UINT32 *)(pIOCB->pKRAM + pProvider->offset_D_buffer));
        #else
        /* 2. copy APDU-Status from old Data-Buffer to new Data-Buffer */
        //*((LSA_UINT32 *)(pIOCB->pKRAM + *pOffsetDB)) = *((LSA_UINT32 *)(pIOCB->pKRAM + pProvider->offset_U2D_buffer));
        #endif

        /* 3. insert user-buffer into ACW */
        EDDI_SetBitField32(&(pProvider->pFCWSnd->Hw1.Value.U32_0), EDDI_SER10_LL1_FCW_SND_BIT__pDB0,
                            (*pOffsetDB + EDDI_IO_KRAM_TO_REGS_OFFSET)); /* 21 Bit */

        //record moment of exchange - could have changed from check before!
        pProvider->clk_cnt_begin = (LSA_UINT32)EDDI_IO_R32(CLK_COUNT_BEGIN_VALUE);

        /* 4. Change buffer-states */
        EDDI_IO_CHANGE_BUFFER_STATES_PART4;

        /* 5. return new buffer */
        *pOffsetDB = pProvider->offset_U_buffer;

        EDDI_EXIT_IO_S(InstanceHandle);
    }
    else  /* called too early */
    {
        EDDI_EXIT_IO_S(InstanceHandle);

        //buffer-ptr is not changed - user gets the same returned
        EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EDDI_BufferXchangeClass3 ERROR: Called too early. No free buffer.");
        return EDD_STS_OK_PENDING;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/* Buffer exchange for Class3-Providers during connect */
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_BufferXchangeCheck( EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  const  pProvider,
                                                                   LSA_UINT32                        *  const  pOffsetDB,
                                                                   EDDI_IO_CB_TYPE                   *  const  pIOCB,
                                                                   LSA_UINT32                           const  InstanceHandle)
{
    LSA_RESULT result;
    LSA_UINT32 xcw_offset = *pProvider->pTransport;

    //check for appearance of xcw-offset
    if (EDDI_IO_XCW_OFFSET_FOLLOWS == xcw_offset)
    {
        result = EDD_STS_OK_PENDING;                                //continue waiting for appearance of fcw-ptt
    }
    else if (xcw_offset & EDDI_IO_OFFSET_IS_ACW)
    {
        //get acw-ptr
        xcw_offset = xcw_offset & (~EDDI_IO_OFFSET_IS_ACW);

        if /*valid alignment */
           (   (0 == (xcw_offset & 0x7UL)) 
            && (xcw_offset < EDDI_IO_KRAM_SIZE))
        {
            pProvider->pACWSnd = (EDDI_SER10_ACW_SND_TYPE *)(pIOCB->pKRAM + xcw_offset);

            pProvider->io_xchange_fct = EDDI_BufferXchangeClass12;       
            result = EDDI_BufferXchangeClass12(pProvider, pOffsetDB, pIOCB, InstanceHandle);   
        }
        else
        {
            result = EDD_STS_ERR_PARAM;
        }
    }
    else if (xcw_offset & EDDI_IO_OFFSET_IS_FCW)
    {
        //get acw-ptr
        xcw_offset = xcw_offset & (~EDDI_IO_OFFSET_IS_FCW);

        if /*valid alignment */
           (   (0 == (xcw_offset & 0x7UL)) 
            && (xcw_offset < EDDI_IO_KRAM_SIZE))
        {
            pProvider->pFCWSnd = (EDDI_SER10_FCW_SND_TYPE *)(pIOCB->pKRAM + xcw_offset);

            pProvider->io_xchange_fct = EDDI_BufferXchangeClass3;       
            result = EDDI_BufferXchangeClass3(pProvider, pOffsetDB, pIOCB, InstanceHandle);    
        }
        else
        {
            result = EDD_STS_ERR_PARAM;
        }
    }
    else
    {
        result = EDD_STS_ERR_PARAM;
    }

    return result;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)


/*===========================================================================*/
/*                              global functions                             */
/*===========================================================================*/
/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_Init()
 *
 * function:      Initialisation of the system for controling the provider 
 *                DataStatus and for ERTEC200 Provider-buffer-exchange
 *
 * parameters:    -
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_Init( LSA_VOID)
{
    LSA_UINT8  i;
  
    for (i=0;i<=EDD_INTERFACE_ID_MAX; i++)
    {
        eddi_IOInstanceList[i] = (EDDI_IO_CB_TYPE *)EDD_DATAOFFSET_INVALID;
        EDDI_Lock_Sema_IO[i] = 0;
        #if defined (EDDI_CFG_REV5)
        EDDI_Lock_Sema_KRAM[i] = 0;
        #endif
    }

    for (i=0;i<EDDI_CFG_MAX_DEVICES; i++)
    {
        eddi_IOCB[i].InstanceHandle = EDDI_IO_UNDEFINED_INSTANCE_HANDLE;

        #if !defined EDDI_CFG_MAX_NR_PROVIDERS
        eddi_IOCB[i].provider_list = LSA_NULL;
        #endif
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_Deinit()
 *
 * function:      Deinitialisation of the system for controling the provider 
 *                DataStatus and for ERTEC200 Provider-buffer-exchange
 *
 * parameters:    -
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_Deinit( LSA_VOID)
{
    return;  // nothing to do
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_Setup()
 *
 * function:      Initialisation of the interface for controling the provider 
 *                DataStatus and for ERTEC200 Provider-buffer-exchange for one 
 *                instance
 *
 * parameters:    InstanceHandle  An instance handle unambiguously identifying 
 *                                the used device/interface throughout the whole 
 *                                system. It is advised to use the InterfaceID.
 *                TraceIdx        Trace index
 *                pKRAM              Pointer to the start of KRAM. This is a virtual 
 *                                address (as the user sees the KRAM).
 *                pIRTE              Pointer to the start of the IRTE-registers. 
 *                                This is a virtual address (as the user sees 
 *                                the IRTE-registers).
 *                pGSharedRAM      Don´t care. Reserved for later versions.
 *                pPAEARAM          Don´t care. Reserved for later versions.
 *                MaxNrProviders  EDDI_IO_CFG_MAX_NR_PROVIDERS is set in eddi_cfg.h:
 *                                Don´t care. Internal control structures get allocated statically.
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_Setup  ( LSA_UINT32    const   InstanceHandle,
                                                                      LSA_UINT32    const   TraceIdx,
                                                                      LSA_VOID    * const   pKRAM, 
                                                                      LSA_VOID    * const   pIRTE, 
                                                                      LSA_VOID    * const   pGSharedRAM, 
                                                                      LSA_VOID    * const   pPAEARAM,
                                                                      LSA_UINT32    const   offset_ProcessImageEnd,
                                                                      LSA_UINT32    const   PollTimeOut_ns,
                                                                      LSA_UINT16    const   MaxNrProviders)
{
    LSA_UINT16        id;
    EDDI_IO_CB_TYPE * pIOCB;
    LSA_RESULT        Result;

    Result = EDD_STS_OK;
    
    LSA_UNUSED_ARG(pPAEARAM);
    #if !defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW) && !defined (EDDI_CFG_REV5)
    LSA_UNUSED_ARG(pIRTE);
    #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    #if defined (EDDI_CFG_3BIF_2PROC)
    LSA_UNUSED_ARG(pGSharedRAM);
    #endif
    #if !defined (EDDI_CFG_REV5)
    LSA_UNUSED_ARG(offset_ProcessImageEnd);
    LSA_UNUSED_ARG(PollTimeOut_ns);
    #endif

    EDDI_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_SysRed_BufferRequest_Setup  -> pKRAM:0x%X pIRTE:0x%X",
                      (LSA_UINT32)pKRAM, (LSA_UINT32)pIRTE);

    if (InstanceHandle > EDD_INTERFACE_ID_MAX)
    { 
        EDDI_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_BufferRequest_Setup illegal InstanceHandle 0x%X",
                          InstanceHandle);
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_CFG_REV5)
    if (   (0 == offset_ProcessImageEnd) || (offset_ProcessImageEnd >= EDDI_KRAM_SIZE_ERTEC400)
        || (0 == PollTimeOut_ns) || (PollTimeOut_ns < 20UL) )
    {
        EDDI_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_BufferRequest_Setup illegal offset_ProcessImageEnd(0x%X) or PollTimeOut_ns(0x%X)",
                          offset_ProcessImageEnd, PollTimeOut_ns);
        return EDD_STS_ERR_PARAM;
    }
    #endif //(EDDI_CFG_REV5)

    //find next free instance
    for (id=0;id<EDDI_CFG_MAX_DEVICES; id++)
    {
        if (InstanceHandle == eddi_IOCB[id].InstanceHandle)
        {
            EDDI_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_BufferRequest_Setup:ERROR InstanceHandle 0x%X exists",
                              InstanceHandle);
            return EDD_STS_ERR_PARAM;
        }
        else if (EDDI_IO_UNDEFINED_INSTANCE_HANDLE == eddi_IOCB[id].InstanceHandle)
        {
            eddi_IOInstanceList[InstanceHandle] = &eddi_IOCB[id];
            eddi_IOCB[id].InstanceHandle = InstanceHandle;
            break;
        }
    }

    if (id>=EDDI_CFG_MAX_DEVICES)
    {
        EDDI_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_BufferRequest_Setup:ERROR no more instances, InstanceHandle:0x%X",
                          InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    #if defined EDDI_CFG_MAX_NR_PROVIDERS
    pIOCB->MaxNrProviders = EDDI_CFG_MAX_NR_PROVIDERS;

    LSA_UNUSED_ARG(MaxNrProviders);
    #else

    if ((MaxNrProviders == 0) || (MaxNrProviders > EDDI_INT_MAX_NR_PROVIDERS))
    {
        EDDI_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_BufferRequest_Setup:ERROR Invalid MaxNrProviders:0x%X", MaxNrProviders);
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_IOAllocLocalMem((void * *)&pIOCB->provider_list, MaxNrProviders * sizeof(EDDI_IO_PROVIDER_LIST_ENTRY_TYPE));
    
    if (EDDI_NULL_PTR == pIOCB->provider_list)
    {
        EDDI_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_BufferRequest_Setup:ERROR IpIOCB->provider_list is not allocated");
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_MemSet(pIOCB->provider_list ,(LSA_UINT8)0, MaxNrProviders * sizeof(EDDI_IO_PROVIDER_LIST_ENTRY_TYPE));

    pIOCB->MaxNrProviders = MaxNrProviders;
    #endif
    //reset provider-data
    for (id=0; id<pIOCB->MaxNrProviders; id++)
    {
        pIOCB->provider_list[id].provider_id       = EDDI_IO_UNKNOWN_PROV_ID;

        #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        pIOCB->provider_list[id].offset_U_buffer   = EDD_DATAOFFSET_UNDEFINED;
        pIOCB->provider_list[id].offset_D_buffer   = EDD_DATAOFFSET_UNDEFINED;
        pIOCB->provider_list[id].offset_F_buffer   = EDD_DATAOFFSET_UNDEFINED;
        pIOCB->provider_list[id].offset_U2D_buffer = EDD_DATAOFFSET_UNDEFINED;
        pIOCB->provider_list[id].offset_D2F_buffer = EDD_DATAOFFSET_UNDEFINED;
        pIOCB->provider_list[id].pFCWSnd           = (EDDI_SER10_FCW_SND_TYPE *)0xFFFFFFFF;
        pIOCB->provider_list[id].io_xchange_fct    = EDDI_BufferXchangeDummy;
        #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        #if defined (EDDI_CFG_SYSRED_2PROC)
        pIOCB->provider_list[id].OrderID           = 0;
        #endif
    }

    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW) || defined (EDDI_CFG_REV5)
    pIOCB->pIRTE              = (LSA_UINT32)pIRTE;
    #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    pIOCB->pKRAM              = (LSA_UINT32)pKRAM;
    pIOCB->TraceIdx           = TraceIdx;
    #if !defined (EDDI_CFG_3BIF_2PROC)
    pIOCB->pGSharedRAM        = (EDDI_GSHAREDMEM_TYPE *)pGSharedRAM;

    #if defined (EDDI_CFG_SYSRED_2PROC)
        EDDI_MemSet(pIOCB->pGSharedRAM, (LSA_UINT8) 0, sizeof(EDDI_GSHAREDMEM_TYPE));
    #endif

    for (id=0; id<=EDD_CFG_MAX_NR_ARSETS; id++)
    {
        pIOCB->pGSharedRAM->ARSet[id] = 0;
    }
    #endif //defined (EDDI_CFG_3BIF_2PROC)

    #if defined (EDDI_CFG_REV5)
    pIOCB->offset_ProcessImageEnd  = offset_ProcessImageEnd;
    pIOCB->PollTimeOut_10ns        = PollTimeOut_ns/10UL;
    pIOCB->ConsistencyRead.Status  = EDDI_IO_CONSST_STS_FREE;
    pIOCB->ConsistencyWrite.Status = EDDI_IO_CONSST_STS_FREE;

    (void)EDDI_CnsReset(InstanceHandle);
    #endif //(EDDI_CFG_REV5)

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_UndoSetup()
 *
 * function:      Free instance resources
 *
 * parameters:    InstanceHandle  The instance handle.
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_UndoSetup ( LSA_UINT32    const   InstanceHandle)
{
    EDDI_IO_CB_TYPE  *  pIOCB;
    LSA_RESULT          Result;

    Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    #if !defined EDDI_CFG_MAX_NR_PROVIDERS
    if (pIOCB->MaxNrProviders > 0)
    {
        LSA_UINT16          ret16;

        EDDI_IOFreeLocalMem(InstanceHandle, &ret16, (EDDI_DEV_MEM_PTR_TYPE)(pIOCB->provider_list));

        if (ret16 != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_MemCloseApduBuffer - EDDI_FREELOCALMEM see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, (LSA_UINT32)ret16, 0);
            return EDD_STS_ERR_RESOURCE;
        }

        pIOCB->provider_list = LSA_NULL;
    }
    #endif

    pIOCB->InstanceHandle = EDDI_IO_UNDEFINED_INSTANCE_HANDLE;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SysRed_Provider_Add()
 *
 * function:      Add a provider
 *
 * parameters:    InstanceHandle    The instance handle.
 *                pApplProviderID    Ptr to ApplProviderID (return value). 
 *                CWOffset            Offset of a controlword from the beginning of 
 *                                  KRAM. (ERTEC200 with 3BIF in SW only.)
 *                DataOffset        Offset to the beginning of KRAM of 3 adjacent
 *                                  buffers. (ERTEC200 with 3BIF in SW only.)
 *                DataStatusOffset    Offset of the DataStatus from the beginning 
 *                                  of KRAM. (!=ERTEC200 with 3BIF in SW only)
 *                GroupID            Don´t care. Reserved for later versions.
 *                ProvType            EDD_PROVADD_TYPE_... (see edd_usr.h)
 *                DataLen            Data length of 1 ProviderBuffer.
 *                                  (ERTEC200 with 3BIF in SW only) 
 *
 * return value:   EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_Provider_Add ( LSA_UINT32      const   InstanceHandle,
                                                              LSA_UINT16    * const   pApplProviderID,
                                                              LSA_UINT32      const   CWOffset,
                                                              LSA_UINT32      const   DataOffset,
                                                              LSA_UINT32      const   DataStatusOffset,
                                                              LSA_UINT16      const   GroupID,
                                                              LSA_UINT8       const   ProvType,
                                                              LSA_UINT16      const   DataLen)
{
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  pProvider;
    LSA_UINT16                           ProviderID;
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result;
    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT32                           length = DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    LSA_UNUSED_ARG(DataStatusOffset);
    #else
    LSA_UNUSED_ARG(CWOffset);
    LSA_UNUSED_ARG(DataOffset);
    LSA_UNUSED_ARG(DataLen);
    #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UNUSED_ARG(GroupID);

    Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_SysRed_Provider_Add -> InstanceHandle:0x%X DataOffset:0x%X",
                      InstanceHandle, (LSA_UINT32)DataOffset);

    //***************************
    // check validity
    //***************************
    if (   (EDD_PROVADD_TYPE_DEFAULT != ProvType)
        && (EDD_PROVADD_TYPE_SYSRED  != ProvType))
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_Provider_Add ERROR: illegal ProvType:0x%X", ProvType);
        return (EDD_STS_ERR_PARAM);
    }

    if ((pIOCB->MaxNrProviders == 0) && (pIOCB->MaxNrProviders > EDDI_INT_MAX_NR_PROVIDERS))  
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_Provider_Add ERROR: Invalid MaxNrProviders:0x%X", pIOCB->MaxNrProviders);
        return (EDD_STS_ERR_PARAM);
    }

    //***************************
    // Find free provider
    //***************************
    *pApplProviderID = EDDI_IO_UNKNOWN_PROV_ID;
    pProvider = &pIOCB->provider_list[0];
    for (ProviderID=0; ProviderID < pIOCB->MaxNrProviders; ProviderID++)
    {
        if (EDDI_IO_UNKNOWN_PROV_ID == pProvider->provider_id)
        {
            //found!
            *pApplProviderID        = ProviderID;
            pProvider->provider_id  = ProviderID;
            break;
        }
        pProvider++;
    }

    if (EDDI_IO_UNKNOWN_PROV_ID == *pApplProviderID)
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_Provider_Add ERROR: No more providers in Instance 0x%X", InstanceHandle);
        return EDD_STS_ERR_PARAM;
    }
    
    //***************************
    // Setup provider
    //***************************
    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    // Preserve 4 Byte-Alignment
    length = (length + 0x03) & ~0x00000003;

    if (CWOffset > EDDI_IO_KRAM_SIZE)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_Provider_Add ERROR: CWOffset:0x%X exceeds KRAM Size:0x%X", CWOffset, EDDI_IO_KRAM_SIZE);
        //invalidate storage space
        *pApplProviderID        = EDDI_IO_UNKNOWN_PROV_ID;
        pProvider->provider_id  = EDDI_IO_UNKNOWN_PROV_ID;
        return EDD_STS_ERR_PARAM;
    }
    
    if ((DataOffset+3*length) > EDDI_IO_KRAM_SIZE)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_Provider_Add ERROR: (DataOffset+3*length):0x%X exceeds KRAM Size:0x%X", (DataOffset+3*length), EDDI_IO_KRAM_SIZE);
        *pApplProviderID        = EDDI_IO_UNKNOWN_PROV_ID;
        pProvider->provider_id  = EDDI_IO_UNKNOWN_PROV_ID;
        return EDD_STS_ERR_PARAM;
    }

    pProvider->offset_U_buffer    = DataOffset;                  //first buffer is the USER-Buffer
    pProvider->offset_D_buffer    = DataOffset + length;         //second buffer is the DATA-Buffer
    pProvider->offset_F_buffer    = pProvider->offset_D_buffer + length;  //third buffer is the FREE-Buffer
    pProvider->offset_U2D_buffer  = pProvider->offset_D_buffer;
    pProvider->offset_D2F_buffer  = pProvider->offset_F_buffer;
    pProvider->clk_cnt_begin      = (LSA_UINT32)EDDI_IO_R32(CLK_COUNT_BEGIN_VALUE);
    pProvider->pDataStatus        = (LSA_UINT8 * )(pIOCB->pKRAM + DataOffset + sizeof(LSA_UINT16));  //points to DS of 1st buffer
    pProvider->pDataStatus1       = pProvider->pDataStatus + length;                                                    //points to DS of 2nd buffer
    pProvider->pDataStatus2       = pProvider->pDataStatus1 + length;                                                   //points to DS of 3rd buffer

    //store ptr to transport-buffer (4B) in KRAM
    pProvider->pTransport = (LSA_UINT32 * )(pIOCB->pKRAM + CWOffset);

    //check for appearance of fcw-offset during xchange
    pProvider->io_xchange_fct = EDDI_BufferXchangeCheck;
    #else
        #if defined (EDDI_CFG_SYSRED_2PROC)
            //check DS-Offset
            pProvider->pDataStatus = (LSA_UINT8 * )&(pIOCB->pGSharedRAM->DataStatusShadow[DataStatusOffset]); 
        #else
            if //DS-Offset too high OR wrong alignment?
               (   (DataStatusOffset >= EDDI_IO_KRAM_SIZE)
                || ((DataStatusOffset + sizeof(LSA_UINT16)) & 0x3UL))
            {
                #if defined (EDDI_CFG_ACCEPT_INVALID_ADDS)
                if (EDD_DATAOFFSET_INVALID == DataStatusOffset)
                {
                    EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_SysRed_Provider_Add: invalid DataStatusOffset:0x%X for ApplProvID:0x%X ignored", 
                                      DataStatusOffset, ProviderID);
                    pProvider->pDataStatus = (LSA_UINT8 *)EDD_DATAOFFSET_INVALID;
                }
                else
                #endif
                {
                    EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_Provider_Add ERROR: DataStatusOffset:0x%X exceeds KRAM Size:0x%X, or is misaligned", 
                                      DataStatusOffset, EDDI_IO_KRAM_SIZE);
                    *pApplProviderID        = EDDI_IO_UNKNOWN_PROV_ID;
                    pProvider->provider_id  = EDDI_IO_UNKNOWN_PROV_ID;
                    return EDD_STS_ERR_PARAM;
                }
            }
            else
            {
                pProvider->pDataStatus        = (LSA_UINT8 * )(pIOCB->pKRAM + DataStatusOffset);
            }
        #endif // (EDDI_CFG_SYSRED_2PROC)
    #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

    pProvider->ProvType           = ProvType;
    //pProvider->OrderID            = 0xFFFF;

    //***************************
    // Init provider
    //***************************
    #if !defined (EDDI_CFG_3BIF_2PROC)
    //*pProvider->pARSet = PrimaryARID;

    if (EDD_PROVADD_TYPE_SYSRED == ProvType)
    {
        #if defined (EDDI_CFG_SYSRED_2PROC)
            eddi_SysRed_ProviderDataStatus_Set(InstanceHandle, (LSA_UINT16)*pApplProviderID, EDD_CSRT_DSTAT_BIT_STATE_BACKUP, EDD_CSRT_DSTAT_BITNR_REDUNDANCY, NULL /* pOrderID */);
            // initialize SharedMem to init=Value /as not written once
        #else
            EDDI_ENTER_IO_S(InstanceHandle);
            //set DataStatus.State to BACKUP
            EDDI_CSRT_SET_DATASTATUS_STATE(*pProvider->pDataStatus, EDD_CSRT_DSTAT_BIT_STATE_BACKUP);
            EDDI_EXIT_IO_S(InstanceHandle);
        #endif
    }
    #endif //!defined (EDDI_CFG_3BIF_2PROC)

    EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_SysRed_Provider_Add <- ApplProviderID:0x%X",
                      (LSA_UINT32)*pApplProviderID);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_SysRed_Provider_Remove()
 *
 * function:      Remove a provider
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID  The provider-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_Provider_Remove( LSA_UINT32    const   InstanceHandle,
                                                                LSA_UINT16    const   ApplProviderID)
{
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  pProvider;
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result;

    Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_SysRed_Provider_Remove -> ProvID:0x%X", (LSA_UINT32)ApplProviderID);

    //***************************
    // check validity
    //***************************
    if (ApplProviderID >= pIOCB->MaxNrProviders)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_Provider_Remove ERROR: ApplProvID:0x%X bigger MaxNrProviders:0x%X", 
            ApplProviderID, pIOCB->MaxNrProviders);
        return EDD_STS_ERR_PARAM;
    }

    pProvider = &pIOCB->provider_list[ApplProviderID];

    if (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_Provider_Remove ERROR: Unknown ProvID:0x%X", pProvider->provider_id); 
        return EDD_STS_ERR_PARAM;
    }

    EDDI_ENTER_IO_S(InstanceHandle);
    //***************************
    // Init provider
    //***************************
    pProvider->provider_id        = EDDI_IO_UNKNOWN_PROV_ID;

    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    pProvider->offset_U_buffer    = EDD_DATAOFFSET_UNDEFINED;
    pProvider->offset_D_buffer    = EDD_DATAOFFSET_UNDEFINED;
    pProvider->offset_F_buffer    = EDD_DATAOFFSET_UNDEFINED;
    pProvider->offset_U2D_buffer  = EDD_DATAOFFSET_UNDEFINED;
    pProvider->offset_D2F_buffer  = EDD_DATAOFFSET_UNDEFINED;
    pProvider->pACWSnd            = (EDDI_SER10_ACW_SND_TYPE *)EDD_DATAOFFSET_UNDEFINED;
    pProvider->pFCWSnd            = (EDDI_SER10_FCW_SND_TYPE *)EDD_DATAOFFSET_UNDEFINED;
    pProvider->io_xchange_fct     = EDDI_BufferXchangeDummy;
    #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

    #if !defined (EDDI_CFG_3BIF_2PROC)
    //pProvider->pARSet  = (LSA_UINT16 *)EDD_DATAOFFSET_UNDEFINED;
    #endif
    EDDI_EXIT_IO_S(InstanceHandle);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if !defined (EDDI_CFG_3BIF_2PROC)
/*=============================================================================
 * function name: eddi_SysRed_ProviderDataStatus_Set()
 *
 * function:      Set the provider specific DataStatus 
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID    The provider-ID
 *                Status            Data status value
 *                Mask              Mask for status bits that must be changed
 *                pOrderID          Pointer to an address to return the Order-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_ProviderDataStatus_Set ( LSA_UINT32      const   InstanceHandle,
                                                                        LSA_UINT16      const   ApplProviderID,
                                                                        LSA_UINT8       const   Status,
                                                                        LSA_UINT8       const   Mask,
                                                                        LSA_UINT16    * const   pOrderID )
{
    LSA_UINT16                           OrderID = 0;
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  pProvider;
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result;

    Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_SysRed_ProviderDataStatus_Set -> ProvID:0x%X", (LSA_UINT32)ApplProviderID);

    //***************************
    // check validity
    //***************************
    if (ApplProviderID >= pIOCB->MaxNrProviders)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Set ERROR: ApplProvID:0x%X bigger then MaxNrProviders:0x%X", 
                          ApplProviderID, pIOCB->MaxNrProviders);
        return EDD_STS_ERR_PARAM;
    }

    pProvider = &pIOCB->provider_list[ApplProviderID];

    if (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Set ERROR: Unknown ProvID:0x%X", pProvider->provider_id); 
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_CFG_ACCEPT_INVALID_ADDS)
    if (EDD_DATAOFFSET_INVALID == (LSA_UINT32)pProvider->pDataStatus)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_ProviderDataStatus_Set ERROR: invalid pDataStatus:0x%X for ApplProvID:0x%X ignored", 
                          pProvider->pDataStatus, pProvider->provider_id); 
        return EDD_STS_OK;
    }
    #endif //defined (EDDI_CFG_ACCEPT_INVALID_ADDS)

    //only STATE and REDUNDANCY are currently allowed to be set
    if (   (Status & (~(EDD_CSRT_DSTAT_BIT_STATE + EDD_CSRT_DSTAT_BIT_REDUNDANCY)))
        || (Mask   & (~(EDD_CSRT_DSTAT_BIT_STATE + EDD_CSRT_DSTAT_BIT_REDUNDANCY))))
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Set ERROR: Status(0x%X) or Mask(0x%X) wrong.", Status, Mask); 
        return EDD_STS_ERR_PARAM;
    }

    //***************************
    // Set DS
    //***************************
    #if defined (EDDI_CFG_SYSRED_2PROC)     //BV!!!!
    OrderID   = pProvider->OrderID;
    if ((EDD_PROVADD_TYPE_SYSRED == pProvider->ProvType) && (Mask))
    #else
    if (Mask)
    #endif
    {
        //LSA_UINT8   DataStatus;

        EDDI_ENTER_IO_S(InstanceHandle);
        #if defined (EDDI_CFG_SYSRED_2PROC)     //BV!!!!
        //DataStatus = (*pProvider->pDataStatus & (~Mask)) | (Status & Mask);
        pProvider->OrderID++;
        OrderID   = pProvider->OrderID;
        {
            EDDI_SET_DS_ORDER   DS_Order              = { pProvider->OrderID, Status, Mask };
            EDDI_H2LE_L(DS_Order.SetDS_OrderCombined);

            //DataStatus = (*pProvider->pDataStatus & (~Mask)) | (Status & Mask);
            *((LSA_UINT32*) pProvider->pDataStatus) = DS_Order.SetDS_OrderCombined;
            pIOCB->pGSharedRAM->ChangeCount++;
        }
        #else
        {
            LSA_UINT8   DataStatus = (*pProvider->pDataStatus & (~Mask)) | (Status & Mask);

            *pProvider->pDataStatus = DataStatus;
            #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
            *pProvider->pDataStatus1 = DataStatus;
            *pProvider->pDataStatus2 = DataStatus;
            #endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
        }
        #endif
        EDDI_EXIT_IO_S(InstanceHandle);
    }

    if (LSA_NULL != pOrderID)
    {
        *pOrderID = OrderID;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

#if defined (EDDI_CFG_SYSRED_2PROC)
/*=============================================================================
 * function name: eddi_SysRed_ProviderDataStatus_Get()
 *
 * function:      Get the provider specific DataStatus and OrderID
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID    The provider-ID
 *                pStatus           Constant pointer to an address that will receive the value of the 
 *                                  Status parameter that has been written to the APDU_Status.DataStatus
 *                pOrderID          Pointer to an address to return the Order-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM, EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_ProviderDataStatus_Get(LSA_UINT32      const  InstanceHandle,
                                                                        LSA_UINT16      const  ApplProviderID,
                                                                        LSA_UINT8     * const  pStatus,
                                                                        LSA_UINT16    * const  pOrderID)
{
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  pProvider;
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_SysRed_ProviderDataStatus_get -> ProvID:0x%X", (LSA_UINT32)ApplProviderID);

    //***************************
    // check validity
    //***************************
    if (LSA_NULL == pStatus)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Get ERROR: pStatus equals NULL");
        return EDD_STS_ERR_PARAM;
    }

    if (LSA_NULL == pOrderID)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Get ERROR: pOrderID equals NULL");
        return EDD_STS_ERR_PARAM;
    }

    if (ApplProviderID >= pIOCB->MaxNrProviders)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Get ERROR: ApplProvID:0x%X bigger then MaxNrProviders:0x%X", 
                          ApplProviderID, pIOCB->MaxNrProviders);
        return EDD_STS_ERR_PARAM;
    }

    pProvider = &pIOCB->provider_list[ApplProviderID];

    if (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Get ERROR: Unknown ProvID:0x%X", pProvider->provider_id); 
        return EDD_STS_ERR_PARAM;
    }

    if (EDD_PROVADD_TYPE_SYSRED != pProvider->ProvType)
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_SysRed_ProviderDataStatus_Get ERROR: invalid ProviderType: 0x%X", pProvider->ProvType); 
        return EDD_STS_ERR_PARAM;
    }

    //***************************
    // Get DS
    //***************************
    {
        EDDI_SET_DS_ORDER   DS_Order;
        LSA_UINT8        *  pDataStatusAcknowledged = pProvider->pDataStatus + ((LSA_UINT8 *) pIOCB->pGSharedRAM->DataStatusAcknowledge - (LSA_UINT8 *) pIOCB->pGSharedRAM->DataStatusShadow);

        DS_Order.SetDS_OrderCombined = *((LSA_UINT32 *) pDataStatusAcknowledged); //pIOCB->pGSharedRAM->DataStatusAcknowledge[pProvider->provider_id];
        EDDI_LE2H_L(DS_Order);
        
        *pStatus  = DS_Order.SetDS_Order.Status;
        *pOrderID = DS_Order.SetDS_Order.OrderID;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif

/*=============================================================================
* function name: eddi_SysRed_PrimaryAR_Set()
*
* function:      Set the provider specific DataStatus 
*
* parameters:    InstanceHandle	    The instance handle.
*                ARSetID            The AR-index
*                PrimaryARID	    ARID of the AR being the current primary AR. 
*
* return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
*===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_PrimaryAR_Set ( LSA_UINT32    const   InstanceHandle,
                                                               LSA_UINT16    const   ARSetID, 
                                                               LSA_UINT16    const   PrimaryARID)
{
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
            Result, InstanceHandle);
        return Result;
    }

    //#if !defined (EDDI_CFG_3BIF_2PROC)
    if ((ARSetID > EDD_CFG_MAX_NR_ARSETS)  ||  (ARSetID < 1))
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_SysRed_PrimaryAR_Set ERROR: ARSetID:0x%X is 0 or bigger then EDD_CFG_MAX_NR_ARSETS:0x%X", ARSetID, EDD_CFG_MAX_NR_ARSETS);
        return (EDD_STS_ERR_PARAM);
    }

    pIOCB->pGSharedRAM->ARSet[ARSetID] = PrimaryARID;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#else //(EDDI_CFG_3BIF_2PROC)
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_ProviderDataStatus_Set(LSA_UINT32    const   InstanceHandle,
                                                                      LSA_UINT16    const   ApplProviderID,
                                                                      LSA_UINT8     const   Status,
                                                                      LSA_UINT8     const   Mask,
                                                                      LSA_UINT16  * const   pOrderID )
{
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(ApplProviderID);
    LSA_UNUSED_ARG(Status);
    LSA_UNUSED_ARG(Mask);
    LSA_UNUSED_ARG(pOrderID);

    return (EDD_STS_ERR_RESOURCE);
}


LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_PrimaryAR_Set ( LSA_UINT32    const   InstanceHandle,
                                                               LSA_UINT16    const   ARSetID, 
                                                               LSA_UINT16    const   PrimaryARID)
{
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(ARSetID);
    LSA_UNUSED_ARG(PrimaryARID);
    return (EDD_STS_ERR_RESOURCE);
}
#endif //!defined (EDDI_CFG_3BIF_2PROC)
                                                                      

#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
/*=============================================================================
 * function name: eddi_ProviderBufferRequest_Xchange()
 *
 * function:      Exchange a provider-buffer
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID  The provider-ID
 *                pOffsetDB       ptr to the offset of the new buffer
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *                *pOffsetDB   offset of the old buffer
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_ProviderBufferRequest_Xchange( LSA_UINT32    const   InstanceHandle,
                                                                       LSA_UINT16    const   ApplProviderID,
                                                                       LSA_UINT32     *      pOffsetDB)
{
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  pProvider;
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result;

    Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_ProviderBufferRequest_XChange -> ProvID:0x%X pOffsetDB:0x%X",
                      (LSA_UINT32)ApplProviderID, (LSA_UINT32)*pOffsetDB);


    //***************************
    // check validity
    //***************************
    if (ApplProviderID >= pIOCB->MaxNrProviders)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_ProviderBufferRequest_Xchange ERROR: ProvID:0x%X bigger then MaxNrProviders:0x%X", ApplProviderID, pIOCB->MaxNrProviders);
        return EDD_STS_ERR_PARAM;
    }

    pProvider = &pIOCB->provider_list[ApplProviderID];

    if (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_ProviderBufferRequest_Xchange ERROR: Unknown ProvID:0x%X", pProvider->provider_id); 
        return EDD_STS_ERR_PARAM;
    }

    //***************************
    // xchange buffer
    //***************************
    return (pProvider->io_xchange_fct(pProvider, pOffsetDB, pIOCB, InstanceHandle));
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: eddi_ProviderBufferRequest_GetUserBuffer()
 *
 * function:      Get the offset of the current USER-buffer
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID  The provider-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM, EDD_STS_ERR_SEQUENCE
 *                *pOffsetDB   offset of the existent user-buffer
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_ProviderBufferRequest_GetUserBuffer( LSA_UINT32    const   InstanceHandle,
                                                                             LSA_UINT16    const   ApplProviderID,
                                                                             LSA_UINT32     *      pOffsetDB)
{
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE  *  pProvider;
    EDDI_IO_CB_TYPE                   *  pIOCB;
    LSA_RESULT                           Result;

    Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_PROVCTRL(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
        return Result;
    }

    EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_ProviderBufferRequest_GetUserBuffer -> ProvID:0x%X",
                      (LSA_UINT32)ApplProviderID);

    //***************************
    // check validity
    //***************************
    if (ApplProviderID >= pIOCB->MaxNrProviders)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_ProviderBufferRequest_GetUserBuffer ERROR: ProvID:0x%X bigger then MaxNrProviders:0x%X", ApplProviderID, pIOCB->MaxNrProviders);
        return EDD_STS_ERR_PARAM;
    }

    pProvider = &pIOCB->provider_list[ApplProviderID];

    EDDI_ENTER_IO_S(InstanceHandle);
    if (pProvider->provider_id == EDDI_IO_UNKNOWN_PROV_ID)
    {
        EDDI_EXIT_IO_S(InstanceHandle);
        EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_ProviderBufferRequest_Xchange ERROR: Unknown ProvID:0x%X", pProvider->provider_id); 
        return EDD_STS_ERR_PARAM;
    }

    *pOffsetDB = pProvider->offset_U_buffer;

    EDDI_EXIT_IO_S(InstanceHandle);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#else //(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_ProviderBufferRequest_Xchange( LSA_UINT32    const   InstanceHandle,
                                                                       LSA_UINT16    const   ApplProviderID,
                                                                       LSA_UINT32     *      pOffsetDB)
{
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(ApplProviderID);
    LSA_UNUSED_ARG(pOffsetDB);
    return (EDD_STS_ERR_RESOURCE);
}

LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_ProviderBufferRequest_GetUserBuffer( LSA_UINT32    const   InstanceHandle,
                                                                             LSA_UINT16    const   ApplProviderID,
                                                                             LSA_UINT32     *      pOffsetDB)
{
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(ApplProviderID);
    LSA_UNUSED_ARG(pOffsetDB);
    return (EDD_STS_ERR_RESOURCE);
}
#endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)


/*****************************************************************************/
/*  end of file eddi_io_provctrl.c                                           */
/*****************************************************************************/
