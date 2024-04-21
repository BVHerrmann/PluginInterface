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
/*  F i l e               &F: eddi_io_kram.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* SRT (soft real time) for EDDI.                                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_IO_KRAM
#define LTRC_ACT_MODUL_ID  509

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_IO_KRAM) //satisfy lint!
#endif

#include "eddi_io_provctrl.h"

/*===========================================================================*/
/*                              Vars                                         */
/*===========================================================================*/
extern EDDI_IO_CB_TYPE * eddi_IOInstanceList[EDD_INTERFACE_ID_MAX+1];
#if defined (EDDI_CFG_REV5)
extern LSA_INT32         EDDI_Lock_Sema_KRAM[EDD_INTERFACE_ID_MAX+1];
#endif

/*===========================================================================*/
/*                              Local Macros                                 */
/*===========================================================================*/
#if defined (EDDI_CFG_REV5)
#define EDDI_SER_CONS_BLOCK_REQ_BIT__BlockStart                EDDI_BIT_MASK_PARA(19, 0)
#define EDDI_SER_CONS_BLOCK_REQ_BIT__BlockLen                  EDDI_BIT_MASK_PARA(27,20)
#define EDDI_SER_CONS_BLOCK_REQ_BIT__BlockCmd                  EDDI_BIT_MASK_PARA(29,28)
#define EDDI_SER_CONS_BLOCK_REQ_BIT__Reserved                  EDDI_BIT_MASK_PARA(31,30)
#define EDDI_SER_CONS_BLOCK_ACK_BIT__BlockStart                EDDI_BIT_MASK_PARA(19, 0)
#define EDDI_SER_CONS_BLOCK_ACK_BIT__BlockLen                  EDDI_BIT_MASK_PARA(27,20)
#define EDDI_SER_CONS_BLOCK_ACK_BIT__BlockUsed                 EDDI_BIT_MASK_PARA(28,28)
#define EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk                   EDDI_BIT_MASK_PARA(29,29)
#define EDDI_SER_CONS_BLOCK_ACK_BIT__BlockError                EDDI_BIT_MASK_PARA(30,30)
#define EDDI_SER_CONS_BLOCK_ACK_BIT__Reserved                  EDDI_BIT_MASK_PARA(31,31)

#define EDDI_SER_CONS_BLOCK_CMD_REQUEST                         0x2
#define EDDI_SER_CONS_BLOCK_CMD_FREE                            0x1
#define EDDI_SER_CONS_BLOCK_USED_NO_TRANSITION                  0x0
#define EDDI_SER_CONS_BLOCK_USED_TRANSITION                     0x1
#define EDDI_SER_CONS_BLOCK_OK_NOT_READY                        0x0
#define EDDI_SER_CONS_BLOCK_OK_READY                            0x1
#define EDDI_SER_CONS_BLOCK_ERROR_NOT_OK                        0x1

#define EDDI_SER_CONS_BLOCK_REQ_RD                              0x0000A000
#define EDDI_SER_CONS_BLOCK_ACK_RD                              0x0000A004
#define EDDI_SER_CONS_BLOCK_REQ_WR                              0x0000A008
#define EDDI_SER_CONS_BLOCK_ACK_WR                              0x0000A00C
#define EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN                      255            // max. length of a consistent block
#endif //(EDDI_CFG_REV5)

#define EDDI_SER_SYNC_MODE                                      0x00013400

//avoid swapping during r/w
#if defined (EDDI_CFG_IO_LITTLE_ENDIAN)
#define EDDI_SER_SYNC_MODE_IN_DONE   0x00000001   // 1-Bit, Input-Data operating ready
#define EDDI_SER_SYNC_MODE_OUT_DONE  0x00000002   // 1-Bit, Output-Data operating ready
#else
#define EDDI_SER_SYNC_MODE_IN_DONE   0x01000000   // 1-Bit, Input-Data operating ready
#define EDDI_SER_SYNC_MODE_OUT_DONE  0x02000000   // 1-Bit, Output-Data operating ready
#endif

/*===========================================================================*/
/*                              macros or inline fcts                        */
/*===========================================================================*/
#if defined (EDDI_CFG_REV5)
#if defined (EDDI_INLINE_SUFFIX)
#define EDDI_IO_INLINE_SUFFIX_ EDDI_INLINE_SUFFIX
#else
#define EDDI_IO_INLINE_SUFFIX_
#endif
#if defined (EDDI_INLINE)
#define EDDI_IO_INLINE_ EDDI_INLINE
#else
#define EDDI_IO_INLINE_ static
#endif

EDDI_IO_INLINE_ LSA_VOID   EDDI_IO_SetBitField32(LSA_UINT32 * const pReg,
    LSA_UINT32   const BitHigh,
    LSA_UINT32   const BitLow,
    LSA_UINT32   const Value) EDDI_IO_INLINE_SUFFIX_;

EDDI_IO_INLINE_ LSA_UINT32  EDDI_IO_GetBitField32(LSA_UINT32 const Reg,
    LSA_UINT32 const BitHigh,
    LSA_UINT32 const BitLow) EDDI_IO_INLINE_SUFFIX_;

EDDI_IO_INLINE_ LSA_VOID   EDDI_IO_SetBitField32(LSA_UINT32 * const pReg,
    LSA_UINT32   const BitHigh,
    LSA_UINT32   const BitLow,
    LSA_UINT32   const Value)
{
    LSA_UINT32 const MaskLocal = ((0xFFFFFFFFUL << (31 - BitHigh)) >> (((31 - BitHigh) + BitLow))) << BitLow;
    LSA_UINT32 const RegLocal = (EDDI_IO_HOST2IRTE32(*pReg) & (~MaskLocal)) | ((Value << BitLow) & MaskLocal);
    *pReg = EDDI_IO_HOST2IRTE32(RegLocal);
}

EDDI_IO_INLINE_ LSA_UINT32  EDDI_IO_GetBitField32(LSA_UINT32 const Reg,
    LSA_UINT32 const BitHigh,
    LSA_UINT32 const BitLow)
{
    LSA_UINT32 const MaskLocal = 0xFFFFFFFFUL >> ((31 - BitHigh) + BitLow);
    return ((EDDI_IO_HOST2IRTE32(Reg) >> BitLow) & MaskLocal);
}
#undef EDDI_INLINE_SUFFIX_ 
#undef EDDI_INLINE_
#endif //(EDDI_CFG_REV5)

/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/
static LSA_RESULT EDDI_IO_GET_IOCB_KRAM(EDDI_IO_CB_TYPE **ppIOCB, LSA_UINT32 InstanceHandle)
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
//TODO FATALTRACE missing
		EDDI_Excp("eddi_BufferRequest_xxx ERROR:illegal InstanceHandle ", EDDI_FATAL_ERR_EXCP, __LINE__, (InstanceHandle));
		return (EDD_STS_ERR_PARAM);
	}

	*ppIOCB = pIOCB;
	return (EDD_STS_OK);
}

/*===========================================================================*/
/*                              global functions                             */
/*===========================================================================*/


#if defined (EDDI_CFG_REV5)
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
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CnsReset( LSA_UINT32  const  InstanceHandle )
{
    EDDI_IO_CB_TYPE       *  pIOCB;
    volatile  LSA_UINT32     ConsBlockReq;
    volatile  LSA_UINT32     ConsBlockAck;
	LSA_RESULT			     Result;

	Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_KRAM(&pIOCB, InstanceHandle);

	if( (EDD_STS_OK != Result) )
	{
		EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
		return Result;
	}

    ConsBlockReq = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_RD);
    ConsBlockAck = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_RD);

    if (ConsBlockReq){} //satisfy lint!

    if (EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk))
    {
        (void)eddi_KRAMReadUnLock(InstanceHandle);
    }

    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_RD) = 0x00000000;
    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_RD) = 0x00000000;

    //////////////////////////////////////////////////////////////////////////
    // Check the WRITE-BLOCK:
    //////////////////////////////////////////////////////////////////////////
    ConsBlockReq = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_WR);
    ConsBlockAck = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_WR);

    if (EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk))
    { // Block is still in use -> free it !
        (void)eddi_KRAMWriteUnLock(InstanceHandle);
    }

    // ConsBlock-Regs must be reset manually
    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_WR) = 0x00000000; // Strange: these operations have no effect on registers.
    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_WR) = 0x00000000;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_KRAMWriteLock()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMWriteLock( LSA_UINT32 const InstanceHandle,
                                                         LSA_UINT32 const Offset,
                                                         LSA_UINT32 const Length )
{
    EDDI_IO_CB_TYPE     *pIOCB;
    LSA_UINT32          ConsBlockReq;
    LSA_UINT32          ConsBlockAck;
    LSA_UINT32          CurrTime_10ns;
    LSA_RESULT          Result = EDD_STS_OK;

	Result = EDDI_IO_GET_IOCB_KRAM(&pIOCB, InstanceHandle);

	if( (EDD_STS_OK != Result) )
	{
		EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
		return Result;
	}

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    EDDI_CRT_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_KRAMWriteLock -> InstanceHandle:0x%X Offset:0x%X Length:0x%X",
                      InstanceHandle, Offset, Length);

    //Check if Datalen > 255 Bytes !!
    if (Length > EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock BlockLen too large. ->Length:0x%X EDDI_MAX_CONSISTENCY_BLOCK_LEN:0x%X", 
                          Length, EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN);
        return EDD_STS_ERR_PARAM;
    }

    if (Length == 0)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock BlockLen == 0.");
        return EDD_STS_ERR_PARAM;
    }

    //Check whether offset is out of range!!
    if ((Offset + Length) > pIOCB->offset_ProcessImageEnd)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock ->Length:0x%X EDDI_MAX_CONSISTENCY_BLOCK_LEN:0x%X",
                          Length, EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN);
        return EDD_STS_ERR_PARAM;
    }
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    EDDI_ENTER_IO_KRAM_S(InstanceHandle);

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    if (EDDI_IO_CONSST_STS_INUSE == pIOCB->ConsistencyWrite.Status)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR,"eddi_KRAMWriteLock OutputData already locked. You must unlock first.");
        EDDI_EXIT_IO_KRAM_S(InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    /*** START ***/
    ConsBlockReq = 0;
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockStart, (Offset | EDDI_IO_KRAM_TO_REGS_OFFSET));
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockLen, Length);
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockCmd, EDDI_SER_CONS_BLOCK_CMD_REQUEST);

    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_WR) = ConsBlockReq;
    ConsBlockAck = 0;

    CurrTime_10ns = EDDI_IO_R32(CLK_COUNT_VALUE);
    for (;;) 
    {
        ConsBlockAck = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_WR);

        if (EDDI_SER_CONS_BLOCK_ERROR_NOT_OK == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockError))
        {
            // Sequence-Error -> return at once !!
            EDDI_CRT_TRACE_04(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock BLOCKERROR -> InstanceHandle:0x%X Offset:0x%X Length:0x%X, Ack:0x%X",
                              InstanceHandle, Offset, Length, ConsBlockAck);
            Result = EDD_STS_ERR_HW;
            break;
        }

        if (   (EDDI_SER_CONS_BLOCK_USED_TRANSITION == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockUsed))
            && (EDDI_SER_CONS_BLOCK_OK_READY        == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk)))
        {
            // Command acknowledged -> return OK
            //Change status of consistency-control
            pIOCB->ConsistencyWrite.Status = EDDI_IO_CONSST_STS_INUSE;
            break;
        }

        if ((EDDI_IO_R32(CLK_COUNT_VALUE) - CurrTime_10ns) >= pIOCB->PollTimeOut_10ns)
        {
            EDDI_CRT_TRACE_03(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock TIMEOUT -> InstanceHandle:0x%X Offset:0x%X Length:0x%X",
                InstanceHandle, Offset, Length);
            Result = EDD_STS_ERR_TIMEOUT;
            break;
        }
    } 
    /*** END ***/

    EDDI_EXIT_IO_KRAM_S(InstanceHandle);

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_KRAMWriteUnLock()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMWriteUnLock( LSA_UINT32  const  InstanceHandle )
{
    EDDI_IO_CB_TYPE  *  pIOCB;
    LSA_UINT32          ConsBlockReq;
    LSA_UINT32          ConsBlockAck;
    LSA_UINT32          CurrTime_10ns;
    LSA_RESULT          Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_KRAM(&pIOCB, InstanceHandle);

	if( (EDD_STS_OK != Result) )
	{
		EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
		return Result;
	}

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_KRAMWriteUnLock -> InstanceHandle:0x%X",
                      InstanceHandle);
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    EDDI_ENTER_IO_KRAM_S(InstanceHandle);

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    if (EDDI_IO_CONSST_STS_INUSE != pIOCB->ConsistencyWrite.Status)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR,"eddi_KRAMWriteLock No OutputData locked!");
        EDDI_EXIT_IO_KRAM_S(InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    /*** START ***/
    ConsBlockReq = 0;
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockCmd, EDDI_SER_CONS_BLOCK_CMD_FREE);

    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_WR) = ConsBlockReq;
    ConsBlockAck = 0;

    CurrTime_10ns = EDDI_IO_R32(CLK_COUNT_VALUE);
    for (;;)
    {
        ConsBlockAck = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_WR);

        if (EDDI_SER_CONS_BLOCK_ERROR_NOT_OK == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockError))
        {
            // Sequence-Error -> return at once !!
            EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteUnLock BLOCKERROR -> InstanceHandle:0x%X, Ack:0x%X",
                InstanceHandle, ConsBlockAck);
            Result = EDD_STS_ERR_HW;
            break;
        }

        if (   (EDDI_SER_CONS_BLOCK_USED_NO_TRANSITION == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockUsed))
            && (EDDI_SER_CONS_BLOCK_OK_NOT_READY       == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk)))
        {
            // Command acknowledged -> return OK
            //Change status of consistency-control
            pIOCB->ConsistencyWrite.Status = EDDI_IO_CONSST_STS_FREE;
            break;
        }

        if ((EDDI_IO_R32(CLK_COUNT_VALUE) - CurrTime_10ns) >= pIOCB->PollTimeOut_10ns)
        {
            EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock TIMEOUT -> InstanceHandle:0x%X",
                InstanceHandle);
            Result = EDD_STS_ERR_TIMEOUT;
            break;
        }
    }
    /*** END ***/

    EDDI_EXIT_IO_KRAM_S(InstanceHandle);

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_KRAMReadLock()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMReadLock( LSA_UINT32  const  InstanceHandle,
                                                        LSA_UINT32  const  Offset,
                                                        LSA_UINT32  const  Length )
{
    EDDI_IO_CB_TYPE  *  pIOCB;
    LSA_UINT32          ConsBlockReq;
    LSA_UINT32          ConsBlockAck;
    LSA_UINT32          CurrTime_10ns;
    LSA_RESULT          Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_KRAM(&pIOCB, InstanceHandle);

	if( (EDD_STS_OK != Result) )
	{
		EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
		return Result;
	}

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    EDDI_CRT_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_KRAMReadLock -> InstanceHandle:0x%X Offset:0x%X Length:0x%X",
        InstanceHandle, Offset, Length);

    //Check if Datalen > 255 Bytes !!
    if (Length > EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMReadLock BlockLen too large. ->Length:0x%X EDDI_MAX_CONSISTENCY_BLOCK_LEN:0x%X", 
                          Length, EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN);
        return EDD_STS_ERR_PARAM;
    }

    if (Length == 0)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMReadLock BlockLen == 0.");
        return EDD_STS_ERR_PARAM;
    }

    //Check whether offset is out of range!!
    if ((Offset + Length) > pIOCB->offset_ProcessImageEnd)
    {
        EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMReadLock ->Length:0x%X EDDI_MAX_CONSISTENCY_BLOCK_LEN:0x%X",
                          Length, EDDI_CNS_MAX_CONSISTENCY_BLOCK_LEN);
        return EDD_STS_ERR_PARAM;
    }
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    EDDI_ENTER_IO_KRAM_S(InstanceHandle);

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    if (EDDI_IO_CONSST_STS_INUSE == pIOCB->ConsistencyRead.Status)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR,"eddi_KRAMReadLock InputData already locked. You must unlock first.");
        EDDI_EXIT_IO_KRAM_S(InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    /*** START ***/
    ConsBlockReq = 0;
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockStart,(Offset | EDDI_IO_KRAM_TO_REGS_OFFSET));
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockLen, Length);
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockCmd, EDDI_SER_CONS_BLOCK_CMD_REQUEST);

    /* the bitfield has already been initialized correctly */
    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_RD) = ConsBlockReq;
    ConsBlockAck = 0;

    CurrTime_10ns = EDDI_IO_R32(CLK_COUNT_VALUE);
    for (;;) 
    {
        ConsBlockAck = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_RD);

        if (EDDI_SER_CONS_BLOCK_ERROR_NOT_OK == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockError))
        {
            // Sequence-Error -> return at once !!
            EDDI_CRT_TRACE_04(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMReadLock BLOCKERROR -> InstanceHandle:0x%X Offset:0x%X Length:0x%X, Ack:0x%X",
                              InstanceHandle, Offset, Length, ConsBlockAck);
            Result = EDD_STS_ERR_HW;
            break;
        }

        if (   (EDDI_SER_CONS_BLOCK_USED_TRANSITION == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockUsed))
            && (EDDI_SER_CONS_BLOCK_OK_READY        == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk)))
        {
            // Command acknowledged -> return OK
            //Change status of consistency-control
            pIOCB->ConsistencyRead.Status = EDDI_IO_CONSST_STS_INUSE;
            break;
        }

        if ((EDDI_IO_R32(CLK_COUNT_VALUE) - CurrTime_10ns) >= pIOCB->PollTimeOut_10ns)
        {
            EDDI_CRT_TRACE_03(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMWriteLock TIMEOUT -> InstanceHandle:0x%X Offset:0x%X Length:0x%X",
                              InstanceHandle, Offset, Length);
            Result = EDD_STS_ERR_TIMEOUT;
            break;
        }
    } 
    /*** END ***/

    EDDI_EXIT_IO_KRAM_S(InstanceHandle);

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_KRAMReadUnLock()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMReadUnLock( LSA_UINT32 const InstanceHandle )
{
    EDDI_IO_CB_TYPE  *  pIOCB;
    LSA_UINT32          ConsBlockReq;
    LSA_UINT32          ConsBlockAck;
    LSA_UINT32          CurrTime_10ns;
    LSA_RESULT          Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_KRAM(&pIOCB, InstanceHandle);

	if( (EDD_STS_OK != Result) )
	{
		EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
                Result, InstanceHandle);
		return Result;
	}

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_KRAMReadUnLock -> InstanceHandle:0x%X",
                      InstanceHandle);
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    EDDI_ENTER_IO_KRAM_S(InstanceHandle);

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    if (EDDI_IO_CONSST_STS_INUSE != pIOCB->ConsistencyRead.Status)
    {
        EDDI_CRT_TRACE_00(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR,"eddi_KRAMReadUnLock No InputData locked!");
        EDDI_EXIT_IO_KRAM_S(InstanceHandle);
        return EDD_STS_ERR_SEQUENCE;
    }
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)

    /*** START ***/
    ConsBlockReq = 0;
    EDDI_IO_SetBitField32(&ConsBlockReq, EDDI_SER_CONS_BLOCK_REQ_BIT__BlockCmd, EDDI_SER_CONS_BLOCK_CMD_FREE);

    EDDI_IO_x32(EDDI_SER_CONS_BLOCK_REQ_RD) = ConsBlockReq;
    ConsBlockAck = 0;

    CurrTime_10ns = EDDI_IO_R32(CLK_COUNT_VALUE);
    for (;;)
    {
        ConsBlockAck = EDDI_IO_x32(EDDI_SER_CONS_BLOCK_ACK_RD);

        if (EDDI_SER_CONS_BLOCK_ERROR_NOT_OK == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockError))
        {
            //Sequence-Error -> return at once !!
            EDDI_CRT_TRACE_02(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMReadUnLock BLOCKERROR -> InstanceHandle:0x%X, Ack:0x%X",
                              InstanceHandle, ConsBlockAck);
            Result = EDD_STS_ERR_HW;
            break;
        }

        if (   (EDDI_SER_CONS_BLOCK_USED_NO_TRANSITION == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockUsed))
            && (EDDI_SER_CONS_BLOCK_OK_NOT_READY       == EDDI_IO_GetBitField32(ConsBlockAck, EDDI_SER_CONS_BLOCK_ACK_BIT__BlockOk)))
        {
            //Command acknowledged -> return OK
            //Change status of consistency-control
            pIOCB->ConsistencyRead.Status = EDDI_IO_CONSST_STS_FREE;
            break;
        }

        if ((EDDI_IO_R32(CLK_COUNT_VALUE) - CurrTime_10ns) >= pIOCB->PollTimeOut_10ns)
        {
            EDDI_CRT_TRACE_01(pIOCB->TraceIdx, LSA_TRACE_LEVEL_ERROR, "eddi_KRAMReadUnLock TIMEOUT -> InstanceHandle:0x%X",
                              InstanceHandle);
            Result = EDD_STS_ERR_TIMEOUT;
            break;
        }
    }
    /*** END ***/

    EDDI_EXIT_IO_KRAM_S(InstanceHandle);

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/
#else //(EDDI_CFG_REV5)
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMWriteLock( LSA_UINT32 const InstanceHandle,
                                                         LSA_UINT32 const Offset,
                                                         LSA_UINT32 const Length )
{
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(Offset);
    LSA_UNUSED_ARG(Length);
    return (EDD_STS_ERR_RESOURCE);
}

LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMWriteUnLock( LSA_UINT32  const  InstanceHandle )
{
    LSA_UNUSED_ARG(InstanceHandle);
    return (EDD_STS_ERR_RESOURCE);
}

LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMReadLock( LSA_UINT32  const  InstanceHandle,
                                                        LSA_UINT32  const  Offset,
                                                        LSA_UINT32  const  Length )
{
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(Offset);
    LSA_UNUSED_ARG(Length);
    return (EDD_STS_ERR_RESOURCE);
}

LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMReadUnLock( LSA_UINT32 const InstanceHandle )
{
    LSA_UNUSED_ARG(InstanceHandle);
    return (EDD_STS_ERR_RESOURCE);
}
#endif //(EDDI_CFG_REV5)


/***************************************************************************/
/* F u n c t i o n:       eddi_KRAMInOutDone()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMInOutDone( LSA_UINT32  const  InstanceHandle )
{
    EDDI_IO_CB_TYPE  *  pIOCB = (EDDI_IO_CB_TYPE*)0;
    LSA_UINT32          SyncMode;
    LSA_RESULT          Result = EDD_STS_OK;

    Result = EDDI_IO_GET_IOCB_KRAM(&pIOCB, InstanceHandle);

    if( (EDD_STS_OK != Result) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_BufferRequest_xxx ERROR in EDDI_IO_GET_IOCB Status 0x%X - InstanceHandle 0x%X",
            Result, InstanceHandle);
        return Result;
    }

    #if !defined (EDDI_CFG_CNS400_SPEED_OPT_ON)
    EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_KRAMInOutDone -> InstanceHandle:0x%X",
                      InstanceHandle);
    #endif //(EDDI_CFG_CNS400_SPEED_OPT_ON)
    
    //get current SyncMode-Register-Value
    SyncMode = EDDI_IO_x32(EDDI_SER_SYNC_MODE);

    //activate IN_DONE and OUT_DONE-bit
    SyncMode = SyncMode | (EDDI_SER_SYNC_MODE_IN_DONE | EDDI_SER_SYNC_MODE_OUT_DONE);

    //write back the new Value
    EDDI_IO_x32(EDDI_SER_SYNC_MODE) = SyncMode;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_io_kram.c                                               */
/*****************************************************************************/

