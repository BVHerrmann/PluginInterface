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
/*  F i l e               &F: eddi_ser_cmd.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDI.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  10.10.07    JS    use EDDI_FATAL_ERR_CMD_IF_TIMEOUT on timeout           */
/*                    use special timeout with FCODE_REMOVE_ACW_RX to        */
/*                    be tolerant against possible MAC-hang situation        */
/*                    (ERTEC-Bug) causing a long delay.                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_ser_ext.h"
//#include "eddi_ext.h"
#include "eddi_ser_cmd.h"
#include "eddi_swi_ext.h"
#include "eddi_crt_ext.h"
#include "eddi_lock.h"
#include "eddi_req.h"

#define EDDI_MODULE_ID     M_ID_SER_CMD
#define LTRC_ACT_MODUL_ID  300

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define EDDI_SERCMD_SW_ID_DEFAULT    0x0UL
#define EDDI_SERCMD_SW_ID_SYNC       0x1UL
#define EDDI_SERCMD_SW_ID_ASYNC      0x2UL

/*---------------------------------------------------------------------------*/
/* COMMAND-Functions                                                         */
/*---------------------------------------------------------------------------*/

#define SER_TIMEOUT_10_US     0x00000000AUL           // generall CMD-timeout
#if defined(EDDI_CFG_REV7)
#define SER_TIMEOUT_MOV_RCV   (3*SER_TIMEOUT_10_US)   // Timeout for Command MOVE_RCV_ACW_DB
#endif
#define SER_TIMEOUT_RESET_TX_QUEUE     (SER_TIMEOUT_10_US  * 2000UL /* 200UL*/) // 20ms

/* Special Timeout with command REMOVE_ACW_RX to handle possible MAC-hang    */
/* situation (ERTEC-Bug). It is possible that this command is delayed for    */
/* a longer time because the EDD-ERTEC just hangs. This hang may be temporary*/
/* and we continue if it is within this timeout.                             */

#ifndef EDDI_CFG_SER_TIMEOUT_REMOVE_ACW_RX_US
#define SER_TIMEOUT_REMOVE_ACW_RX      (SER_TIMEOUT_10_US  * 200UL) // 2000us 
#else
#define SER_TIMEOUT_REMOVE_ACW_RX      (EDDI_CFG_SER_TIMEOUT_REMOVE_ACW_RX_US)
#endif

static EDD_UPPER_RQB_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_SERRemoveSheduledRequest( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_SERCheckAsyncConf( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetPrimCmd( LSA_UINT16              const FCode,
                                                        LSA_UINT32              const PrimPara,
                                                        EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                        LSA_UINT32              const UsrId );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetSecCmd( LSA_UINT32              const SecPara,
                                                       EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERWaitForConf( EDDI_LOCAL_MEM_U32_PTR_TYPE const pResult,
                                                         EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                         LSA_UINT16                  const FCode,
                                                         LSA_UINT32                  const TimeOutUs );

#if defined (EDDI_SER_CMD_STAT)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SerCmdStatistic( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                          LSA_UINT16              const FCode,
                                                          LSA_UINT32              const DiffIn10Ns,
                                                          LSA_BOOL                const isAsync );
#endif


/*=============================================================================
* function name: EDDI_SERCommand()
*
* function:      Command
*
* parameters:    LSA_UINT16        FCode,
*                LSA_UINT32        PrimPara,
*                LSA_UINT32        SecPara,
*                EDDI_LOCAL_MEM_U32_PTR_TYPE    pResult
*
* return value:  void
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERCommand( LSA_UINT16                   const  FCode,
                                                LSA_UINT32                   const  PrimPara,
                                                LSA_UINT32                   const  SecPara,
                                                EDDI_LOCAL_MEM_U32_PTR_TYPE  const  pResult,
                                                EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_UINT32  timeout_us;
    LSA_UINT32  PrimCommand;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERCommand, > ->FCode:0x%X PrimPara:0x%X", FCode, PrimPara);

    EDDI_ENTER_COM_S();

    switch (pDDB->CmdIF.State)
    {
        case EDDI_CMD_IF_FREE:
            break;

        case EDDI_CMD_IF_LOCKED:
        {
            PrimCommand = IO_x32(PRIM_COMMAND);
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SERCommand, CMD_IF_LOCKED > ->PrimCommand:0x%X", PrimCommand);

            switch (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode))
            {
                case FCODE_CLEAR_FDB:
                case FCODE_RESET_TX_QUEUE:
                case FCODE_REMOVE_ACW_TX:
                case FCODE_AGE_FDB:
                    break;

                default:
                {
                    EDDI_Excp("EDDI_SERCommand, FCode !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
                    EDDI_EXIT_COM_S();
                    return;
                }
            }

            EDDI_SERWaitForConf(pResult, pDDB, FCode, SER_TIMEOUT_10_US * 100 * 10); //10ms !!
            pDDB->CmdIF.State = EDDI_CMD_IF_LOCKED_CONF_DONE;
            break;
        }

        case EDDI_CMD_IF_LOCKED_CONF_DONE:
            break;

        default:
        {
            EDDI_Excp("EDDI_SERCommand, FCode -- State", EDDI_FATAL_ERR_EXCP, FCode, pDDB->CmdIF.State);
            EDDI_EXIT_COM_S();
            return;
        }
    }


    if (SecPara == 0)
    {
        EDDI_Excp("EDDI_SERCommand SecPara == 0 !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
        EDDI_EXIT_COM_S();
        return;
    }

    switch (FCode)
    {
            //case FCODE_INSERT_FCW_TX:
            //case FCODE_INSERT_FCW_RX:
            #if defined(EDDI_CFG_REV7)
        case FCODE_MOVE_RCV_ACW_DB:
        {
            timeout_us = SER_TIMEOUT_MOV_RCV;
            break;
        }
        #endif
        case FCODE_REMOVE_ACW_TX:
        case FCODE_INSERT_ACW_TX:
        case FCODE_INSERT_ACW_RX:
        {
            timeout_us = SER_TIMEOUT_10_US;
            break;
        }


        default:
        {
            EDDI_Excp("EDDI_SERCommand FCode 4 !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
            EDDI_EXIT_COM_S();
            return;
        }
    }

    #if defined (EDDI_SER_CMD_STAT)
    pDDB->CmdIF.StartClkCountValue = IO_GET_CLK_COUNT_VALUE_10NS;
    #endif

    EDDI_SERSetPrimCmd(FCode, PrimPara, pDDB, EDDI_SERCMD_SW_ID_SYNC);

    EDDI_SERSetSecCmd(SecPara, pDDB);

    EDDI_SERWaitForConf(pResult, pDDB, FCode, timeout_us);

    EDDI_EXIT_COM_S();
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERWaitForConf( EDDI_LOCAL_MEM_U32_PTR_TYPE  const  pResult,
                                                            EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                            LSA_UINT16                  const  FCode,
                                                            LSA_UINT32                  const  TimeOutUs )
{
    volatile  LSA_UINT32                  ConfCommand;
    volatile  LSA_UINT32                  PrimCommand;
    LSA_UINT32                            i;
    LSA_UINT32                            TestTimeOut;
    #if defined (EDDI_SER_CMD_STAT)
    LSA_UINT32                            StopClkCountValue = 0;
    LSA_UINT32                            DiffIn10Ns;
    #endif

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERWaitForConf > ->FCode:0x%X TimeOut:0x%X", FCode, TimeOutUs);

    ConfCommand = 0;
    for (i = 0; i < TimeOutUs; i++)
    {

        //   PrimCommand = IO_x32(PRIM_COMMAND);  //0x16400
        ConfCommand = IO_x32(CONF_COMMAND);  //0x16408

        if ((EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Conf) != 0) ||
            (EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Error) != 0))
        {
            #if defined (EDDI_SER_CMD_STAT)
            StopClkCountValue = IO_GET_CLK_COUNT_VALUE_10NS;
            #endif

            if (EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Error))
            {
                //LSA_UINT32 const prim = IO_x32(PRIM_COMMAND);
                //LSA_UINT32 const sec  = IO_x32(SEC_COMMAND);
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, 
                                      "EDDI_SERWaitForConf, ConfCommand:0x%X PRIMCommand:0x%X SECCommand:0x%X", 
                                      ConfCommand, 
                                      IO_x32(PRIM_COMMAND), 
                                      IO_x32(SEC_COMMAND));
                                      
                EDDI_Excp("EDDI_SERWaitForConf Command with Error - FCode ", EDDI_FATAL_ERR_EXCP, FCode, i);
                return;
            }

            if (!(pResult == EDDI_NULL_PTR))
            {
                *pResult = EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Result);
            }

            //Command finished with expected timeout
            #if defined (EDDI_SER_CMD_STAT)
            DiffIn10Ns = StopClkCountValue - pDDB->CmdIF.StartClkCountValue;

            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERWaitForConf, < ->FCode:0x%X Diff:0x%X", FCode, DiffIn10Ns);
            EDDI_SerCmdStatistic(pDDB, FCode, DiffIn10Ns, LSA_FALSE);
            #endif

            return;
        }

        EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL); // wait 1us
    }

    // Now we check how long the command will REALLY take to finish:
    {

        PrimCommand = IO_x32(PRIM_COMMAND);  //0x16400
        if (PrimCommand)
        {
        }
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EXCP EDDI_SERCommand, > ->PrimCommand:0x%X ConfCommand:0x%X",
                              PrimCommand, ConfCommand);

        IO_x32(PRIM_COMMAND) = PrimCommand;  //0x16400
    }

    TestTimeOut = 100000; //wait 100ms additionally
    for (i = 0; i < TestTimeOut; i++)
    {
        ConfCommand = IO_x32(CONF_COMMAND);  // 0x 16408

        if ((EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Conf)  != 0) ||
            (EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Error) != 0))
        {
            #if defined (EDDI_SER_CMD_STAT)
            StopClkCountValue = IO_GET_CLK_COUNT_VALUE_10NS;
            #endif
            break;
        }

        EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);
    }

    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERWaitForConf Expected   Timeout overrun ->FCode:0x%X TimeOut:0x%X, PrimCommand:0x%X ConfCommand:0x%X",
                          FCode, i, PrimCommand, ConfCommand);

    if (EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Error))
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERWaitForConf Additional Test failed due to Error ->FCode:0x%X AdditionalTimeUs:0x%X",
                              FCode, i);
    }
    else
    {
        if (i < TestTimeOut)
        {
            //Command finished with prolonged timeout
            #if defined (EDDI_SER_CMD_STAT)
            //no error --> log statistics only
            DiffIn10Ns = StopClkCountValue - pDDB->CmdIF.StartClkCountValue;

            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERWaitForConf, < ->FCode:0x%X Diff:0x%X", FCode, DiffIn10Ns);
            EDDI_SerCmdStatistic(pDDB, FCode, DiffIn10Ns, LSA_FALSE);
            #else
            //error
            EDDI_Excp("EDDI_SERWaitForConf Timeout ConfCommand; FCode - i ", EDDI_FATAL_ERR_CMD_IF_TIMEOUT, FCode, i);
            #endif
        }
        else
        {
            //Command did not finish even with prolonged timeout
            EDDI_Excp("EDDI_SERWaitForConf Timeout ConfCommand; FCode - Original TimeOutUs", EDDI_FATAL_ERR_CMD_IF_TIMEOUT, FCode, TimeOutUs);
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
#if defined (EDDI_SER_CMD_STAT)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerCmdStatistic( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT16              const  FCode,
                                                             LSA_UINT32              const  DiffIn10Ns,
                                                             LSA_BOOL                const  isAsync )
{
    PROF_EDDI_SERCMD_INFO  *  pInfo;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SerCmdStatistic > ->FCode:0x%X ;Diff10ns:%X", FCode, DiffIn10Ns);

    pInfo = &pDDB->pProfKRamInfo->SerCmdInfo;

    if (isAsync)
    {
        if (pInfo->FCodeMaxDelayAsync[FCode] < DiffIn10Ns)
        {
            pInfo->FCodeMaxDelayAsync[FCode] = DiffIn10Ns;
        }

        pInfo->FCodeCurDelayAsync[FCode] = DiffIn10Ns;
    }
    else
    {
        if (pInfo->FCodeMaxDelay[FCode] < DiffIn10Ns)
        {
            pInfo->FCodeMaxDelay[FCode] = DiffIn10Ns;
        }

        pInfo->FCodeCurDelay[FCode] = DiffIn10Ns;
    }
}
#endif //defined (EDDI_SER_CMD_STAT)
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSingleCommand( LSA_UINT16                  const  FCode,
                                                      LSA_UINT32                  const  PrimPara,
                                                      EDDI_LOCAL_MEM_U32_PTR_TYPE  const  pResult,
                                                      EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_UINT32  TimeOut = 7 * SER_TIMEOUT_10_US; // Worstcase-Duration for  ACW-RX-Remove: 70 us
    LSA_UINT32  PrimCommand;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERSingleCommand, > ->:0x%X :0x%X", FCode, PrimPara);

    EDDI_ENTER_COM_S();

    switch (pDDB->CmdIF.State)
    {
        case EDDI_CMD_IF_FREE:
            break;

        case EDDI_CMD_IF_LOCKED:
        {
            PrimCommand = IO_x32(PRIM_COMMAND);
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SERSingleCommand, CMD_IF_LOCKED > ->PrimCommand:0x%X", PrimCommand);

            switch (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode))
            {
                case FCODE_CLEAR_FDB:
                case FCODE_RESET_TX_QUEUE:
                case FCODE_REMOVE_ACW_TX:
                case FCODE_AGE_FDB:
                    break;

                default:
                {
                    EDDI_Excp("EDDI_SERSingleCommand, FCode !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
                    EDDI_EXIT_COM_S();
                    return;
                }
            }

            EDDI_SERWaitForConf(pResult, pDDB, FCode, SER_TIMEOUT_10_US * 100 * 10); //10ms !!
            pDDB->CmdIF.State = EDDI_CMD_IF_LOCKED_CONF_DONE;
            break;
        }

        case EDDI_CMD_IF_LOCKED_CONF_DONE:
            break;

        default:
        {
            EDDI_Excp("EDDI_SERSingleCommand, FCode -- State", EDDI_FATAL_ERR_EXCP, FCode, pDDB->CmdIF.State);
            EDDI_EXIT_COM_S();
            return;
        }
    }

    switch (FCode)
    {
        case FCODE_REMOVE_ACW_RX:
            TimeOut = SER_TIMEOUT_REMOVE_ACW_RX;
            break;
        case FCODE_RESET_TX_QUEUE:
            TimeOut = SER_TIMEOUT_RESET_TX_QUEUE;
            break;
            //case FCODE_REMOVE_FCW_TX:
            //case FCODE_REMOVE_FCW_RX:
        case FCODE_ENABLE_ACW_RX_LIST:
        case FCODE_ENABLE_ACW_TX_LIST:
        case FCODE_DISABLE_ACW_TX_LIST:
        case FCODE_INSERT_ACW_TX:
        case FCODE_REMOVE_ACW_TX:
        case FCODE_RESERVE_ADR_ENTRY:
        case FCODE_CLEAR_FDB_ENTRY:
        case FCODE_ENABLE_DATA_IMAGE:
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
            //case FCODE_CLEAR_REDUNDANCE_STATE:
            #endif
        case FCODE_DISABLE_DATA_IMAGE:
            break;
        default:
        {
            EDDI_Excp("EDDI_SERSingleCommand 3 FCode !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
            EDDI_EXIT_COM_S();
            return;
        }
    }

    #if defined (EDDI_SER_CMD_STAT)
    pDDB->CmdIF.StartClkCountValue = IO_GET_CLK_COUNT_VALUE_10NS;
    #endif

    EDDI_SERSetPrimCmd(FCode, PrimPara, pDDB, EDDI_SERCMD_SW_ID_SYNC);

    EDDI_SERWaitForConf(pResult, pDDB, FCode, TimeOut);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSingleCommand");

    EDDI_EXIT_COM_S();
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetPrimCmd( LSA_UINT16              const  FCode,
                                                           LSA_UINT32              const  PrimPara,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32              const  UsrId )
{
    volatile  LSA_UINT32  PrimCommand;
    LSA_UINT32            TmpCommand;
    LSA_UINT32            TimeOut;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetPrimCmd > ->FCode:0x%X  PrimPara:0x%X", FCode, PrimPara);

    PrimCommand = IO_x32(PRIM_COMMAND);

    if (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__Active))
    {
        EDDI_Excp("EDDI_SERSetPrimCmd IF active FCode - old FCode!", EDDI_FATAL_ERR_EXCP, FCode,
                  EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode));
        return;
    }

    if (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode))
    {
        EDDI_Excp("EDDI_SERSetPrimCmd 1 IF active FCode - old FCode!", EDDI_FATAL_ERR_EXCP, FCode,
                  EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode));
        return;
    }

    // build prim command
    TmpCommand = 0x0L;
    EDDI_SetBitField32(&TmpCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode,  FCode);
    EDDI_SetBitField32(&TmpCommand, EDDI_SER_PRIM_COMMAND_BIT__UsrId,  UsrId);
    EDDI_SetBitField32(&TmpCommand, EDDI_SER_PRIM_COMMAND_BIT__Para,   PrimPara);
    EDDI_SetBitField32(&TmpCommand, EDDI_SER_PRIM_COMMAND_BIT__Active, COMMAND_ACTIVATE);

    PrimCommand = TmpCommand;

    for (TimeOut = 0; TimeOut < SER_TIMEOUT_10_US; TimeOut++)
    {
        IO_x32(PRIM_COMMAND) = PrimCommand;

        if (PrimCommand == IO_x32(PRIM_COMMAND))
        {
            return;
        }

        EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);
    }

    EDDI_Excp("EDDI_SERSetPrimCmd - Timeout!", EDDI_FATAL_ERR_EXCP, FCode, PrimCommand );
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERSetSecCmd()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetSecCmd( LSA_UINT32              const  SecPara,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    volatile  LSA_UINT32  SecCommand;
    LSA_UINT32            TmpCommand;
    LSA_UINT32            TimeOutUs;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetSecCmd, > SecPara:0x%X", SecPara);

    //build sec. command
    TmpCommand = 0x0L;
    EDDI_SetBitField32(&TmpCommand, EDDI_SER_SEC_COMMAND_BIT__Para, SecPara);
    EDDI_SetBitField32(&TmpCommand, EDDI_SER_SEC_COMMAND_BIT__Active, COMMAND_ACTIVATE);

    SecCommand = TmpCommand;
    for (TimeOutUs = 0; TimeOutUs < SER_TIMEOUT_10_US; TimeOutUs++)
    {
        IO_x32(SEC_COMMAND) = SecCommand;

        if (SecCommand == IO_x32(SEC_COMMAND))
        {
            return;
        }
        EDDI_WAIT_10_NS(pDDB->hSysDev, 100UL);
    }

    EDDI_Excp("EDDI_SERSetSecCmd, SecCommand, CycleTimeOnError", EDDI_FATAL_ERR_EXCP, SecCommand, 0);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERSingleDirectCmd()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     PrimPara: Attention: HOST-Endianess!             */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
/* has to be executed directly -> synchronous lock functions at ERTEC200!! */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSingleDirectCmd( LSA_UINT16                  const  FCode,
                                                        LSA_UINT32                  const  PrimPara,
                                                        EDDI_LOCAL_MEM_U32_PTR_TYPE  const  pResult,
                                                        EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_UINT32  PrimCommand;
    LSA_UINT32  TimeOut;

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERSingleDirectCmd, FCode:0x%X PrimPara:0x%X", FCode, PrimPara);

    EDDI_ENTER_COM_S();

    switch (pDDB->CmdIF.State)
    {
        case EDDI_CMD_IF_FREE:
            break;

        case EDDI_CMD_IF_LOCKED:
        {
            PrimCommand = IO_x32(PRIM_COMMAND);

            switch (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode))
            {
                case FCODE_CLEAR_FDB:
                case FCODE_RESET_TX_QUEUE:
                case FCODE_REMOVE_ACW_TX:
                case FCODE_AGE_FDB:
                    break;

                default:
                {
                    EDDI_Excp("EDDI_SERSingleDirectCmd, FCode !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
                    EDDI_EXIT_COM_S();
                    return;
                }
            }

            EDDI_SERWaitForConf(pResult, pDDB, FCode, SER_TIMEOUT_10_US * 100 * 5); //5ms !!
            pDDB->CmdIF.State = EDDI_CMD_IF_LOCKED_CONF_DONE;
            break;
        }

        case EDDI_CMD_IF_LOCKED_CONF_DONE:
            break;

        default:
        {
            EDDI_Excp("EDDI_SERSingleDirectCmd, FCode -- State", EDDI_FATAL_ERR_EXCP, FCode, pDDB->CmdIF.State);
            EDDI_EXIT_COM_S();
            return;
        }
    }

    switch (FCode)
    {
        case FCODE_DISABLE_ACW_TX_LIST:
        case FCODE_ENABLE_ACW_TX_LIST:
        case FCODE_ENABLE_ACW_RX_LIST:
            TimeOut = SER_TIMEOUT_10_US;
            break;

        case FCODE_RESET_TX_QUEUE:
            TimeOut = SER_TIMEOUT_RESET_TX_QUEUE;
            break;
        case FCODE_ENABLE_DATA_IMAGE:
            TimeOut = SER_TIMEOUT_10_US;
            break;
        case FCODE_DISABLE_DATA_IMAGE:
            TimeOut = SER_TIMEOUT_10_US;
            break;

            #if defined (EDDI_CFG_REV7) || defined (EDDI_CFG_REV6)
        case FCODE_RELOAD_SB_ENTRY:
            TimeOut = SER_TIMEOUT_10_US;
            break;
            #endif

        default:
        {
            EDDI_Excp("EDDI_SERSingleDirectCmd, 2 !", EDDI_FATAL_ERR_EXCP, FCode, 0);
            EDDI_EXIT_COM_S();
            return;
        }
    }

    #if defined (EDDI_SER_CMD_STAT)
    pDDB->CmdIF.StartClkCountValue = IO_GET_CLK_COUNT_VALUE_10NS;
    #endif

    EDDI_SERSetPrimCmd(FCode, PrimPara, pDDB, EDDI_SERCMD_SW_ID_SYNC);

    EDDI_SERWaitForConf(pResult, pDDB, FCode, TimeOut);

    EDDI_EXIT_COM_S();

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSingleDirectCmd");
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERAsyncCmd( LSA_UINT16               const  FCode,
                                                 LSA_UINT32               const  PrimPara,
                                                 LSA_UINT32               const  SecPara,
                                                 EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                 EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                 LSA_BOOL                 const  bLock )
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERAsyncCmd,  FCode:0x%X PrimPara:0x%X", FCode, PrimPara);

    if (bLock)
    {
        EDDI_ENTER_COM_S();
    }
    if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
    {
        EDDI_Excp("EDDI_SERAsyncCmd pDDB->CmdIF.State != EDDI_CMD_IF_FREE !", EDDI_FATAL_ERR_EXCP, pDDB->CmdIF.State, 0);
        if (bLock)
        {
            EDDI_EXIT_COM_S();
        }
        return;
    }

    pDDB->CmdIF.State = EDDI_CMD_IF_LOCKED;

    #if defined (EDDI_SER_CMD_STAT)
    pDDB->CmdIF.StartClkCountValueAsync = IO_GET_CLK_COUNT_VALUE_10NS;
    #endif

    switch (FCode)
    {
        case FCODE_CLEAR_FDB:
        case FCODE_RESET_TX_QUEUE:
        case FCODE_REMOVE_ACW_TX:
        case FCODE_AGE_FDB:
            break;

        default:
        {
            EDDI_Excp("EDDI_SERAsyncCmd 2 FCode !", EDDI_FATAL_ERR_EXCP, FCode, PrimPara);
            if (bLock)
            {
                EDDI_EXIT_COM_S();
            }
            return;
        }
    }

    EDDI_SERSetPrimCmd(FCode, PrimPara, pDDB, EDDI_SERCMD_SW_ID_ASYNC);

    // Some Commands have 2 Params
    switch (FCode)
    {
        case FCODE_REMOVE_ACW_TX:
        {
            EDDI_SERSetSecCmd(SecPara, pDDB);
            break;
        }

        default:
            break;
    }

    if (pDDB->CmdIF.pActAsyncRQB != 0)
    {
        EDDI_Excp("EDDI_SERAsyncCmd pDDB->CmdIF.pActAsyncRQB != 0 !", EDDI_FATAL_ERR_EXCP, pDDB->CmdIF.State, 0);
        if (bLock)
        {
            EDDI_EXIT_COM_S();
        }
        return;
    }

    pDDB->CmdIF.pActAsyncRQB = pRQB;
    pDDB->CmdIF.ActAsyncService = pRQB->Service;

    if (bLock)
    {
        EDDI_EXIT_COM_S();
    }
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERAsyncCmd");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERAsyncConf()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERAsyncConf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_RQB_PTR_TYPE                pRQB;
    EDD_SERVICE                           ActAsyncService;
    LSA_BOOL                              bRet;
    #if defined (EDDI_SER_CMD_STAT)
    LSA_UINT32                            StopClkCountValue; // = IO_GET_CLK_COUNT_VALUE_10NS;
    LSA_UINT32                            DiffIn10Ns;
    LSA_UINT16                            FCode;
    LSA_UINT32                            PrimCommand;

    StopClkCountValue = IO_GET_CLK_COUNT_VALUE_10NS;
    #endif

    // Cmd.State == FREE
    // --> Ignore old Zombie-Interupt !

    // Cmd.State == LOCKED
    // --> Asynchronous service had been started last
    // --> Verify with EDDI_SERCheckAsyncConf
    //     --> If command is ready;
    //         --> call finishing functions
    //     --> If command is not ready yet: Zombie-Interupt !
    //         --> return and wait for next interrupt.

    // Cmd.State == LOCKED_CFG_DONE
    // --> synchronous service had been called while a Async-Cmd was still running.

    // --> Search and execute requests in the queue until it is empty or 
    //      CMD_IF is used again

    // runs under REST-lock -> triggered by interrupt -> EDDint_INT_Instruction_Done
    // with that the command-if is protected by EDD_ENTER_REST_S geschützt -- EXCEPT !!!
    // if at ERTEC200 functions access the process image directly 
    // - e.g. edd_ConsumerBufferRequest -> SERSingleDirectCmdLock
    // NOTE: if one of these functions releases EDD_ENTER_COM_S lock, it can (this function) interrupted
    // by access to the process image at ERTEC200 !!!!!!!
  
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERAsyncConf, > ->pDDB->CmdIF.State:0x%X", pDDB->CmdIF.State);

    EDDI_ENTER_COM_S();

    switch (pDDB->CmdIF.State)
    {
        case EDDI_CMD_IF_FREE:
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERAsyncConf, -> Ignore this Zombie-IRQ, No Cmd Running. pDDB->CmdIF.State :0x%X", pDDB->CmdIF.State);
            // Zombie - Interrupt of a Synchronous Command
            // No more Requests in the Cmd Queue
            // --> ignore it
            EDDI_EXIT_COM_S();
            return;
        }

        case EDDI_CMD_IF_LOCKED:
        {
            // Interrupt after Asynchronous Command, Check Result
            bRet = EDDI_SERCheckAsyncConf(pDDB);
            if (!bRet)
            {
                // Cmd is still running
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERAsyncConf,-> Ignore this Zombie-IRQ, Async Cmd Still Running. pDDB->CmdIF.State:0x%X", pDDB->CmdIF.State);
                EDDI_EXIT_COM_S();
                return;
            }
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERAsyncConf,-> Async Done. pDDB->CmdIF.State:0x%X", pDDB->CmdIF.State);
            break;
        }

        case EDDI_CMD_IF_LOCKED_CONF_DONE:
        {
            // Interrupt after a Synchronous Command
            // --> This Interrupt is needed to trigger the finishing-Code below
            // (ConfBit was already checked in WaitForConf-Function)
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_SERAsyncConf, State !", EDDI_FATAL_ERR_EXCP, pDDB->CmdIF.State, 0);
            EDDI_EXIT_COM_S();
            return;
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Get current RQB .
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++

    pRQB = pDDB->CmdIF.pActAsyncRQB;
    ActAsyncService = pDDB->CmdIF.ActAsyncService;

    if (!pRQB)
    {
        // Asynchronous command -> pRQB is not allowed to be missing !
        EDDI_Excp("EDDI_SERAsyncConf, Missing RQB !", EDDI_FATAL_ERR_EXCP, pDDB->CmdIF.State, 0);
        EDDI_EXIT_COM_S();
        return;
    }

    pDDB->CmdIF.pActAsyncRQB = 0;
    pDDB->CmdIF.ActAsyncService = 0;
    pDDB->CmdIF.State        = EDDI_CMD_IF_FREE;

    EDDI_EXIT_COM_S();

    // -> COM lock is open - at ERTEC200 this can be interrupted by an access to the process image
    // in order to make the rest of the function work, all state checks and changes 
    // to  <pDDB->CmdIF.State> have to be executed under COM lock

    #if defined (EDDI_SER_CMD_STAT)
    PrimCommand = IO_x32(PRIM_COMMAND);
    FCode       = (LSA_UINT16)EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode);
    DiffIn10Ns  = StopClkCountValue - pDDB->CmdIF.StartClkCountValueAsync;

    EDDI_SerCmdStatistic(pDDB, FCode, DiffIn10Ns, LSA_TRUE);
    #endif

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Do refinishing operations of Current RQB
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //do not access RQB for the service here, as it could be invalid (close_channel interrupted the command flow)
    switch (ActAsyncService)
    {
        case EDDI_SRV_AGE_DUMMY:
        {
            if (EDDI_OPC_AGE_DUMMY_IN_USE != EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB))
            {
                EDDI_Excp("EDDI_SERAsyncConf, StateError AGE_DUMMY_RQB !", EDDI_FATAL_ERR_EXCP, EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB), 0);
                return;
            }

            EDD_RQB_SET_OPCODE(&pDDB->CmdIF.AgeDummyRQB, EDDI_OPC_AGE_DUMMY_UNUSED);
            break;
        }

        case EDD_SRV_SWITCH_FLUSH_FILTERING_DB:
        {
            if (SWI_FLUSH_FILTERING_DB_TX_FCODE_RESET_TX_QUEUE == pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
            {
                EDDI_SwiPortResetSQTimeoutConf(pRQB, (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context);
            }
            else if (SWI_FLUSH_FILTERING_DB_TX_FCODE_CLEAR_FDB == pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERAsyncConf, SWI_FLUSH_FILTERING_DB_TX_FCODE_CLEAR_FDB finished pDDB->CmdIF.State:0x%X", pDDB->CmdIF.State);

                if (pDDB->pLocal_SWITCH->ResetSQ_Reestablish_SpanningTreeState)
                {
                    LSA_UINT32  UsrPortIndex;
        
                    //restore spanning tree state
                    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
                    {
                        LSA_UINT32  HwPortIndex;

                        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                        /* change the PortState only if not Linkdown. If LinkDown we only store the */
                        /* state for a later LinkUp. While Link Down the DISABLED-State shall not   */
                        /* be changed!                                                              */
                        if (   (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus != EDD_LINK_DOWN)
                            && (!pDDB->Glob.PhyPowerOff[HwPortIndex]))
                        {
                            EDDI_SwiPortSetSpanningTree(HwPortIndex, pDDB->pLocal_SWITCH->ResetSQ_SpanningTreeState[HwPortIndex], pDDB);
                        }
                        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState = pDDB->pLocal_SWITCH->ResetSQ_SpanningTreeState[HwPortIndex];
                    }
                    pDDB->pLocal_SWITCH->ResetSQ_Reestablish_SpanningTreeState = LSA_FALSE;
                }

                //finish service
                EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
                pDDB->CmdIF.cRequest_CLEAR_DYN_FDB--;
                pDDB->pLocal_SWITCH->ResetSQ_Cmd       = 0;
                pDDB->pLocal_SWITCH->ResetSQ_pRQB      = EDDI_NULL_PTR;
                pDDB->pLocal_SWITCH->ServiceCanceled   = 0;
                pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode = SWI_FCODE_NOT_DEFINIED;

                EDDI_SwiUsrClearFDBGetNextRequestAndExecute(pDDB);
            }
            else
            {
                if (   (0 == pDDB->pLocal_SWITCH->ResetSQ_Cmd)
                    && (SWI_FCODE_NOT_DEFINIED == pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode)
                    && !pDDB->pLocal_SWITCH->ResetSQ_Reestablish_SpanningTreeState )
                {
                    //channel has been closed in the meantime
                    EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SERAsyncConf, ResetSQ_Cmd is 0, LSA channel has been closed, pRQB:0x%X, Current Service:0x%X, Canceled Service:0x%X", 
                        pDDB->pLocal_SWITCH->ResetSQ_pRQB, ActAsyncService, pDDB->pLocal_SWITCH->ServiceCanceled);
                }
                else
                {
                    EDDI_Excp("EDDI_SERAsyncConf, EDD_SRV_SWITCH_FLUSH_FILTERING_DB, Service: ResetSQ_Cmd_Fcode:", EDDI_FATAL_ERR_EXCP,
                              pRQB->Service, pDDB->pLocal_SWITCH->ResetSQ_Cmd_Fcode);
                }
                return;
            }
            break;
        }

        case EDDI_SRV_SWITCH_CLEAR_DYN_FDB:
        {
            EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);

            pDDB->CmdIF.cRequest_CLEAR_DYN_FDB--;

            EDDI_SwiUsrClearFDBGetNextRequestAndExecute(pDDB);
            break;
        }

        case EDD_SRV_SRT_PROVIDER_CONTROL:
        {
            EDDI_CRTProviderRemoveEventFinish(pRQB, pDDB, LSA_TRUE /*bPassivate*/);
            break;
        }

        case EDD_SRV_SRT_PROVIDER_REMOVE:
        {
            EDDI_CRTProviderRemoveEventFinish(pRQB, pDDB, LSA_FALSE /*bPassivate*/);
            break;
        }

        #if !defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
		case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE:
        {
            EDDI_RedTreeSrtProviderChPhasePart2(pRQB, pDDB);
            break;
        }
		#endif //!defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

        default:
        {
            EDDI_Excp("EDDI_SERAsyncConf, 5 Service !", EDDI_FATAL_ERR_EXCP, pRQB->Service, 0);
            return;
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Execute next RQB from Queue
    // until it is empty or new AsyncCmd was activated
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for (;;)
    {
        EDDI_ENTER_COM_S();

        // -> can also be EDD_CMD_IF_LOCKED_CONF_DONE if this function has been interrupted through an access
        // to the process image at ERTEC200
        if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
        {
            // A New AsyncCmd was started --> return and Wait for Async Confirmation
            EDDI_EXIT_COM_S();
            return;
        }

        pRQB = EDDI_SERRemoveSheduledRequest(pDDB);

        if (!pRQB)
        {
            // No more Requests in Cmd-Queue
            EDDI_EXIT_COM_S();
            return;
        }

        EDDI_EXIT_COM_S();

        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERAsyncConf, > Dequeue and call Service:0x%X", pRQB->Service);

        switch (pRQB->Service)
        {
            case EDDI_SRV_AGE_DUMMY:
            {
                // AgeTimeout is triggered by timer and so has no pRQB.
                // Special call needed.
                if (EDDI_OPC_AGE_DUMMY_QUEUED != EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB))
                {
                    EDDI_Excp("EDDI_SERAsyncConf, StateError AGE_DUMMY_RQB !", EDDI_FATAL_ERR_EXCP, EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB), 0);
                    return;
                }

                EDD_RQB_SET_OPCODE(&pDDB->CmdIF.AgeDummyRQB, EDDI_OPC_AGE_DUMMY_DEQUEUED);
                EDDI_SWIAgeTimeout(pDDB);
                return;
            }

            default:
            {
                if (pRQB->internal_context_1 == 0)
                {
                    EDDI_Excp("EDDI_SERAsyncConf, 6 Service !", EDDI_FATAL_ERR_EXCP, pRQB->Service, 0);
                    return;
                }

                // Execute next Request
                // CmdIF.State will automatically be set to EDDI_CMD_IF_LOCKED if an AsyncCmd was executed
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERAsyncConf, Dequeue and call Service:0x%X", pRQB->Service);


                if (pRQB->Service == EDD_SRV_SWITCH_FLUSH_FILTERING_DB)
                {
                    //CmdIf had been busy when service had to be executed -> try again!
                    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERAsyncConf, continue CMD 0xA/0xB:0x%X", pRQB->Service);
                    EDDI_SwiPortResetSQTimeout(pDDB);
                }
                else
                {
                    ((EDDI_HDB_REQ_FCT)pRQB->internal_context_1)(pRQB, (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context);
                }
                break;
            }
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
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SERCheckAsyncConf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  PrimCommand;
    LSA_UINT32  ConfCommand;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERCheckAsyncConf > ->pDDB->CmdIF.State:0x%X", pDDB->CmdIF.State);

    PrimCommand = IO_x32(PRIM_COMMAND);
    if (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__UsrId) == EDDI_SERCMD_SW_ID_DEFAULT)
    {
        // ConfCommand has already been read (by whom ?  Synchronously confirmed command ?)
        EDDI_Excp("EDDI_SERCheckAsyncConf Default !", EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode), 0);
        return LSA_FALSE;
    }

    if (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__UsrId) != EDDI_SERCMD_SW_ID_ASYNC)
    {
        EDDI_Excp("EDDI_SERCheckAsyncConf PrimCommand.---.UsrId != EDDI_SERCMD_SW_ID_ASYNC!",
                  EDDI_FATAL_ERR_EXCP,
                  EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode),
                  EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__UsrId));
        return LSA_FALSE;
    }

    ConfCommand = IO_x32(CONF_COMMAND);  // 0x 16408

    switch (EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode))
    {
        case FCODE_CLEAR_FDB:
        case FCODE_RESET_TX_QUEUE:
        case FCODE_REMOVE_ACW_TX:
        case FCODE_AGE_FDB:
        {
            if (EDDI_GetBitField32(ConfCommand, EDDI_SER_CONF_COMMAND_BIT__Conf) == 0)
            {
                // CmdIF is in use! -> delayed interrupt of an synchronous command
                //  -> the interrupt of the currently executed asynchronous command will follow !
                return LSA_FALSE;
            }

            return LSA_TRUE;
        }

        default:
        {
            EDDI_Excp("EDDI_SERCheckAsyncConf Default!", EDDI_FATAL_ERR_EXCP,
                      EDDI_GetBitField32(PrimCommand, EDDI_SER_PRIM_COMMAND_BIT__FCode), 0);
            return LSA_TRUE;
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
static  EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SERRemoveSheduledRequest( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_RQB_PTR_TYPE  const  pRQB = pDDB->CmdIF.pTop;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERRemoveSheduledRequest->");

    if (pRQB == EDDI_NULL_PTR)
    {
        return pRQB;
    }

    //dequeue RQB
    pDDB->CmdIF.pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pDDB->CmdIF.pTop);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERRemoveSheduledRequest<-");

    return pRQB;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERSheduledRequest()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSheduledRequest( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                        LSA_UINT32               const  eddi_req_fct )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSheduledRequest->");

    if (eddi_req_fct == 0)
    {
        EDDI_Excp("EDDI_SERSheduledRequest, eddi_req_fct == 0 !", EDDI_FATAL_ERR_EXCP, pRQB->Service, 0);
        return;
    }

    /* append request (RQB) to the end of the request queue */
    EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

    pRQB->internal_context_1 = eddi_req_fct;

    if (pDDB->CmdIF.pTop == EDDI_NULL_PTR)
    {
        pDDB->CmdIF.pTop = pRQB;
    }
    else
    {
        EDD_RQB_SET_NEXT_RQB_PTR(pDDB->CmdIF.pBottom, pRQB);
    }

    pDDB->CmdIF.pBottom = pRQB;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSheduledRequest<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ser_cmd.c                                               */
/*****************************************************************************/
