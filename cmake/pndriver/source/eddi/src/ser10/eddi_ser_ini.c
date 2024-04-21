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
/*  F i l e               &F: eddi_ser_ini.c                            :F&  */
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
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  02.08.07    JS    ChannelB mapping changed to free configuration except  */
/*                    for Prio 7 and 8 (iSRT and ORG) which must use if 0!   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_ser_ext.h"
#include "eddi_ser_cmd.h"
#include "eddi_nrt_inc.h"

#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_SER_INI
#define LTRC_ACT_MODUL_ID  301

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define SER_POLL_TIMEOUT    0x40

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetupxRT( EDDI_UPPER_DSB_PTR_TYPE const pDSB,
                                                      EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetupGlobMACModul( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetupSWI( SER_HANDLE_PTR          const pHandle,
                                                      EDDI_UPPER_DSB_PTR_TYPE const pDSB,
                                                      EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetupNRT( SER_HANDLE_PTR          const pHandle,
                                                      EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSetupGlobMAC( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERStartErtec( EDDI_UPPER_DSB_PTR_TYPE const pDSB,
                                                        EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SerSetMinBetriebsmodus( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                 LSA_UINT32              const DeviceSWIStatus );


/*=============================================================================
* function name: EDDI_SERReset()
*
* function:      
*
* parameters:    
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERReset( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, LSA_BOOL bCalledForOpen)
{
    LSA_UINT32            HwPortIndex;
    LSA_INT               TimeOutIn10MS;
    volatile  LSA_UINT32  TempSWIStatus;
    LSA_UINT32            DeviceSWISollStatus;
    LSA_UINT32            ActionSWIStatus;
    LSA_INT               i;
    LSA_UINT32            SWICtrl;
    LSA_UINT32            NumberOfPorts;
    LSA_BOOL              bHWPortIndexNotStopped[EDDI_MAX_IRTE_PORT_CNT];             

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERReset->");

    SWICtrl       = IO_x32(SWI_CTRL);
    TempSWIStatus = IO_x32(SWI_STATUS);

    DeviceSWISollStatus = 0;

    switch (pDDB->ERTEC_Version.Variante)
    {
        /* 4 ports */
        case 0x00:
        {
            if ((SWICtrl == 0) && (TempSWIStatus == 0xFFFF))
            {
                return;
            }
            EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port1Stop, 1);
            EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port2Stop, 1);
            EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port3Stop, 1);
            NumberOfPorts = 4;
            break;
        }

        /* 3 ports */
        case 0x01:
        case 0x03:
        case 0x07: //SOC1
        case 0x09:
        {
            if ((SWICtrl == 0) && (TempSWIStatus == 0x7FFF))
            {
                return;
            }
            EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port1Stop, 1);
            EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port2Stop, 1);
            NumberOfPorts = 3;
            break;
        }

        /* 2 ports */
        case 0x04:
        case 0x06:
        case 0x08:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x02:
        case 0x0F: //SOC2
        {
            if ((SWICtrl == 0) && (TempSWIStatus == 0x3FFF))
            {
                return;
            }
            EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port1Stop, 1);
            NumberOfPorts = 2;
            break;
        }

        /* 1 port */
        case 0x0D:  //theoretically 2 ports - practically only one port in the FPGA
        case 0x05:
        {
            if ((SWICtrl == 0) && (TempSWIStatus == 0x1FFF))
            {
                return;
            }
            NumberOfPorts = 1;
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_SERReset, pDDB->ERTEC_Version.Variante", EDDI_FATAL_ERR_EXCP, pDDB->ERTEC_Version.Variante, 0);
            return;
        }
    }

    //close all ports
    for (HwPortIndex = 0; HwPortIndex < NumberOfPorts; HwPortIndex++)
    {
        //no port mapping necessary
        if (!bCalledForOpen)
        {
                EDDI_SwiMiscSetDisable(HwPortIndex, LSA_TRUE, pDDB);
        }
        bHWPortIndexNotStopped[HwPortIndex] = LSA_FALSE;
    }

    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__SWIStop,     1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__KonsStop,    1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__PHY_SMIStop, 1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__StatiStop,   1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__TRCStop,     1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__KRAMStop,    1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__SRTStop,     1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__IRTStop,     1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__NRTStop,     1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__TRIGGERStop, 1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__IRQStop,     1);
    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__CYCStop,     1);

    EDDI_SetBitField32(&DeviceSWISollStatus, EDDI_SER_SWI_STATUS_BIT__Port0Stop,   1);

    IO_x32(IRT_CTRL) = 0; /* IRT_Control: all ports disabled */
    EDDI_WAIT_10_NS(pDDB->hSysDev, 2000000UL); /* 20 ms */

    SWICtrl = IO_x32(SWI_CTRL);

    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__IRTCtrl, 0); /* IRT-Control Modul disabled */
    EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL); /* 10 ms */

    TempSWIStatus = IO_x32(SWI_STATUS);
    if (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__PHY_SMIStop))
    {
        if (!bCalledForOpen)
        {
            LSA_UINT32 UsrPortIndex;
            LSA_RESULT Result;

            //Shutdown: set all transceivers to PowerDown
            for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
            {
                HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
                Result = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_OFF, LSA_TRUE /*bRaw*/); 
                if (EDD_STS_OK != Result)
                {
                    EDDI_Excp("EDDI_SWIStartPhys, EDDI_TRASetPowerDown, HwPortIndex: Result:", EDDI_FATAL_ERR_LL, HwPortIndex, Result);
                    return;
                }
            }
            EDDI_WAIT_10_NS(pDDB->hSysDev, 2000000UL); /* 20 ms */
        }
    }
    else
    {
        if (!bCalledForOpen)
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SERReset, should shutdown transceivers, but SMI is not running!");
            EDDI_Excp("EDDI_SERReset, should shutdown transceivers, but SMI is not running! ", EDDI_FATAL_ERR_LL, 0, 0);
            return;
        }
    }

    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__Port0, 0); //switch off port 0
    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__Port1, 0); //switch off port 1
    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__Port2, 0); //switch off port 2
    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__Port3, 0); //switch off port 3

    IO_x32(SWI_CTRL) = SWICtrl;
    EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL); /* 10 ms */
    
    TempSWIStatus = IO_x32(SWI_STATUS);

    switch (NumberOfPorts)
    {
        case 4:
        {
            if (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port3Stop))
            {
                bHWPortIndexNotStopped[3] = LSA_TRUE;
                EDDI_WAIT_10_NS(pDDB->hSysDev, 10000UL);
            }
        }
        //lint -fallthrough
        case 3:
        {
            if (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port2Stop))
            {
                bHWPortIndexNotStopped[2] = LSA_TRUE;
                EDDI_WAIT_10_NS(pDDB->hSysDev, 10000UL);
            }
        }
        //lint -fallthrough
        case 2:
        {
            if (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port1Stop))
            {
                bHWPortIndexNotStopped[1] = LSA_TRUE;
                EDDI_WAIT_10_NS(pDDB->hSysDev, 10000UL);
            }
        }
        //lint -fallthrough
        case 1:
        {
            if (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port0Stop))
            {
                bHWPortIndexNotStopped[0] = LSA_TRUE;
                EDDI_WAIT_10_NS(pDDB->hSysDev, 10000UL);
            }
            break;
        }
        default:
        {
            EDDI_Excp("EDDI_SERReset, invalid NumberOfPorts:", EDDI_FATAL_ERR_EXCP, NumberOfPorts, 0);
            return;
        }
    }

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERReset, SWI_STATUS:0x%X", TempSWIStatus);

    if (!bCalledForOpen)
    {
        /* check for not stopped ports */
        for (HwPortIndex=0; HwPortIndex<NumberOfPorts; HwPortIndex++)
        {
            if (bHWPortIndexNotStopped[HwPortIndex])
            {
                /* Workaround for ports that will not stop (TFS2660127) */
                /* bSMIRunning has been checked before */
                LSA_RESULT Result;
                volatile LSA_UINT32 RegLocal;
            
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SERReset, HWPortIndex %d could not be stopped, trying workaround.", HwPortIndex);

                /* Step 1: Enable FCW lists for this port and wait for the longest cycle. This will inject an EOL.*/
                RegLocal = IO_x32(IRT_CTRL);
                IO_x32(IRT_CTRL) = RegLocal | (0x3UL << (HwPortIndex*2));
                EDDI_WAIT_10_NS(pDDB->hSysDev, 4UL*1000UL*100UL); /* 4ms (longest cycle)*/

                /* Step 2: Set the PHY transceiver to PowerUp, for 256µs, see EDD_USR.DOC Ch. 9.2.2 (SW induced waiting times (the same for all supported PHY transceivers) */
                Result = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_ON, LSA_TRUE /*bRaw*/); 
                if (EDD_STS_OK != Result)
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SERReset, transceiver at HWPortIndex %d could not be powered up, Status:0x%X", HwPortIndex, Result);
                    EDDI_Excp("EDDI_SERReset, transceiver could not be powered up", EDDI_FATAL_ERR_EXCP, HwPortIndex, Result);
                    return;
                }
                EDDI_WAIT_10_NS(pDDB->hSysDev, 256UL*100UL); /* 256µs, only some clock pulses are needed */

                /* Step 3: Disable FCW lists again.*/
                IO_x32(IRT_CTRL) = 0;

                /* Step 4: Set the PHY transceiver to PowerDown again */
                Result = pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRASetPowerDown(pDDB, HwPortIndex, EDDI_PHY_POWER_OFF, LSA_TRUE /*bRaw*/); 
                if (EDD_STS_OK != Result)
                {
                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SERReset, transceiver at HWPortIndex %d could not be powered down, Status:0x%X", HwPortIndex, Result);
                    EDDI_Excp("EDDI_SERReset, transceiver could not be powered down", EDDI_FATAL_ERR_EXCP, HwPortIndex, Result);
                    return;
                }
                EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL);

                /* Step 5: Check port again */
                TempSWIStatus = IO_x32(SWI_STATUS);
                if (   ((0 == HwPortIndex) && (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port0Stop)))
                    || ((1 == HwPortIndex) && (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port1Stop)))
                    || ((2 == HwPortIndex) && (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port2Stop)))
                    || ((3 == HwPortIndex) && (!EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__Port3Stop))) )
                {
                    EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SERReset, HWPortIndex %d could not be stopped, TempSWIStatus:0x%X, SWICtrl:0x%X", 
                        HwPortIndex, TempSWIStatus, SWICtrl);
                    EDDI_Excp("EDDI_SERReset, TempSWIStatus, SWICtrl", EDDI_FATAL_ERR_EXCP, TempSWIStatus, SWICtrl);
                    return;
                }
            }
        }
    }   //end of workaround
    
    /* card set in run-mode                      */
    /* card will be in config-mode after poweron */
    if (EDDI_GetBitField32(SWICtrl, EDDI_SER_SWI_CTRL_BIT__SwitchEnable) == 0)
    {
        EDDI_SerSetMinBetriebsmodus(pDDB, DeviceSWISollStatus);
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    //Check if IRTStartTimeRegister were all set back to 0
    {
        LSA_UINT32  ErrorPortMaskSnd = 0;
        LSA_UINT32  ErrorPortMaskRcv = 0;

        for (HwPortIndex = 0; HwPortIndex < pDDB->PM.HwTypeMaxPortCnt; HwPortIndex++)
        {
            LSA_UINT32  SndTime = 0;
            LSA_UINT32  RcvTime = 0;

            switch (HwPortIndex)
            {
                case 0:
                    SndTime = IO_R32(IRT_START_TIME_SND_P0);
                    RcvTime = IO_R32(IRT_START_TIME_RCV_P0);
                    break;
                case 1:
                    SndTime = IO_R32(IRT_START_TIME_SND_P1);
                    RcvTime = IO_R32(IRT_START_TIME_RCV_P1);
                    break;
                case 2:
                    SndTime = IO_R32(IRT_START_TIME_SND_P2);
                    RcvTime = IO_R32(IRT_START_TIME_RCV_P2);
                    break;
                case 3:
                    SndTime = IO_R32(IRT_START_TIME_SND_P3);
                    RcvTime = IO_R32(IRT_START_TIME_RCV_P3);
                    break;
                default:
                {
                    EDDI_Excp("EDDI_SERReset, wrong HwPortIndex:", EDDI_FATAL_ERR_EXCP, HwPortIndex, 0);
                    return;
                }
            }

            if (SndTime | RcvTime)
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERReset, IRTStartTimeRegister Error SndTime:0x%X, RcvTime:0x%X", SndTime, RcvTime);
                ErrorPortMaskSnd = ErrorPortMaskSnd | ((SndTime?1UL:0) << (HwPortIndex));
                ErrorPortMaskRcv = ErrorPortMaskRcv | ((RcvTime?1UL:0) << (HwPortIndex));
            }
        }

        if (ErrorPortMaskSnd | ErrorPortMaskRcv)
        {
            EDDI_Excp("EDDI_SERReset, IRTStartTimeRegister, ErrorPortMaskSnd: ErrorPortMaskRcv:", EDDI_FATAL_ERR_EXCP, ErrorPortMaskSnd, ErrorPortMaskRcv);
            return;
        }
    }
    #endif

    #if defined (EDDI_CFG_ERTEC_400)
    //Workaround HW Bug
    if (IO_R32(TRC_COMMAND) > 0)
    {
        IO_x32(TRC_MODE) = TRC_EXECUTE;
    }

    TempSWIStatus = IO_x32(SWI_STATUS);
    if (EDDI_GetBitField32(TempSWIStatus, EDDI_SER_SWI_STATUS_BIT__KonsStop) == 0)
    {
        EDDI_Excp("EDDI_SERReset, Consistency-Reset failed", EDDI_FATAL_ERR_EXCP, TempSWIStatus, 0);
        return;
    }

    EDDI_SerSetMinBetriebsmodus(pDDB, DeviceSWISollStatus);
    //SerPortsPower(pDDB, EDDI_PHY_POWER_OFF);
    #else
    EDDI_SerSetMinBetriebsmodus(pDDB, DeviceSWISollStatus);
    #endif

    /* card set in config-mode                                                  */
    /* soft-reset will be executed during changing from run-mode to config-mode */
    IO_W32(SWI_CTRL, 2UL);

    ActionSWIStatus = DeviceSWISollStatus;

    TimeOutIn10MS = 100;

    //check all Switch-Ctlr stopped
    for (i = 0; i < TimeOutIn10MS; i++)
    {
        TempSWIStatus = IO_x32(SWI_STATUS);

        if (TempSWIStatus == ActionSWIStatus)
        {
            //close all ports
            for (HwPortIndex = 0; HwPortIndex < pDDB->PM.HwTypeMaxPortCnt; HwPortIndex++)
            {
                //no mapping necessary
                EDDI_SwiMiscSetDisable(HwPortIndex, LSA_TRUE, pDDB);
            }

            EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL); /* 10 ms */
            return;
        }

        EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL); /* 10 ms */
    }

    EDDI_Excp("EDDI_SERReset, TempSWIStatus != ActionSWIStatus", EDDI_FATAL_ERR_EXCP, TempSWIStatus, ActionSWIStatus);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SerSetMinBetriebsmodus()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SerSetMinBetriebsmodus( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  DeviceSWIStatus )
{
    volatile  LSA_UINT32  TempSWIIstStatus;
    LSA_UINT32            ActionSWISollStatus;
    LSA_INT               i;
    LSA_UINT32            SWICtrl;
    LSA_INT               TimeOutIn10MS;

    SWICtrl = 0;
    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__SwitchEnable, 1);    //Working mode
    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__PHY_SMICtrl,  1);    //One module has been active in order to execute SoftReset

    IO_x32(SWI_CTRL) = SWICtrl;

    //check all Switch-Ctlr stopped except PHY
    ActionSWISollStatus = DeviceSWIStatus;
    EDDI_SetBitField32(&ActionSWISollStatus, EDDI_SER_SWI_STATUS_BIT__PHY_SMIStop, 0);
    EDDI_SetBitField32(&ActionSWISollStatus, EDDI_SER_SWI_STATUS_BIT__SWIStop,     0);

    TimeOutIn10MS        = 100;
    TempSWIIstStatus = 0;

    for (i = 0; i < TimeOutIn10MS; i++)
    {
        TempSWIIstStatus = IO_x32(SWI_STATUS);

        if (TempSWIIstStatus == ActionSWISollStatus)
        {
            EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL); /* 10ms */
            break;
        }
        EDDI_WAIT_10_NS(pDDB->hSysDev, 1000000UL); /* 10ms */
    }

    if (i == TimeOutIn10MS)
    {
        EDDI_Excp("EDDI_SerSetMinBetriebsmodus, TempSWIIstStatus != ActionSWISollStatus", EDDI_FATAL_ERR_EXCP, TempSWIIstStatus, ActionSWISollStatus);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetupxRT()
*
* function:      
*
* parameters:    
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetupxRT( EDDI_UPPER_DSB_PTR_TYPE  const  pDSB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_INT           Index;
    LSA_UINT32        DADR_31__0;
    LSA_UINT16        DADR_47_32;
    EDD_MAC_ADR_TYPE  MACAdr;
    LSA_UINT16        tmp, i, EtherType;
    LSA_UINT16        xSwapRTFrameType[0x4];

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetupxRT");

    // Check if we can use Short DataBuffers for Class1_2-Communication
    // The function must be called after Switch-Setup.
    #if defined (EDDI_CFG_ENABLE_PADDING_CLASS12)
    pDDB->NRT.PaddingEnabled    = LSA_TRUE;
    #else
    pDDB->NRT.PaddingEnabled    = LSA_FALSE;
    #endif

    DADR_31__0 = 0;
    DADR_47_32 = 0;

    //================== Configuration of xRT-Frame-Type =====================
    // swap again ! */
    EtherType = EDDI_xRT_TAG;   //invers like Compiler Little or Big
    for (i = 0; i < 4; i++)
    {
        xSwapRTFrameType[i] =  (LSA_UINT16)(EtherType << 8); /* LowByte to HighByte */
        tmp                 =  (LSA_UINT16)(EtherType >> 8);
        xSwapRTFrameType[i] =  EDDI_HOST2IRTE16((LSA_UINT16)(xSwapRTFrameType[i] + tmp));
    }

    // Hardware always needs Little
    IO_W32(ETH_TYPE_0 , xSwapRTFrameType[0]);
    IO_W32(ETH_TYPE_1 , xSwapRTFrameType[1]);
    IO_W32(ETH_TYPE_2 , xSwapRTFrameType[2]);
    IO_W32(ETH_TYPE_3 , xSwapRTFrameType[3]);

    //============== Configuration of xRT-SRC/DST-MAC-Adress =================
    //IRT-Destination-Adress

    MACAdr = EDDI_SwiPNGetRT3INVALIDDA(pDDB);
    //pMACAdr = &pDSB->GlobPara.IRTMACAddressDst;
    for (Index = (LSA_INT)(EDD_MAC_ADDR_SIZE - 1); Index >= 0; Index--)
    {
        if (Index < (LSA_INT)(sizeof(LSA_UINT32)))
        {
            DADR_31__0 <<= 8;
            DADR_31__0 |= MACAdr.MacAdr[Index];
        }
        else
        {
            DADR_47_32 <<= 8;
            DADR_47_32 |= MACAdr.MacAdr[Index];
        }
    }

    //write in Switch-IRT-DA_31__0/IRTDA_47_32-Register
    IO_W32(IRT_DA_31__0, DADR_31__0);
    IO_W32(IRT_DA_47_32, DADR_47_32);

    //IRT-Source-Adress
    EDDI_SERSetIRTSAdress(&pDSB->GlobPara.xRT.MACAddressSrc, pDDB);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIRTSAdress( EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pMACAdress,
                                                      EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_INT           Index;
    LSA_UINT32        SADR_31__0;
    LSA_UINT16        SADR_47_32;
    EDD_MAC_ADR_TYPE  MACAdr;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetIRTSAdress");

    /* set IRT-Source-Adress */
    SADR_31__0 = 0;
    SADR_47_32 = 0;
    MACAdr     = *pMACAdress;

    for (Index = (LSA_INT)(EDD_MAC_ADDR_SIZE - 1); Index >= 0; Index--)
    {
        if (Index < (LSA_INT)(sizeof(LSA_UINT32)))
        {
            SADR_31__0 <<= 8;
            SADR_31__0 |= MACAdr.MacAdr[Index];
        }
        else
        {
            SADR_47_32 <<= 8;
            SADR_47_32 |= MACAdr.MacAdr[Index];
        }
    }

    /* write in Switch-IRT-DA_31__0/IRTDA_47_32-Register */
    IO_W32(SA_31__0, SADR_31__0);
    IO_W32(SA_47_32, SADR_47_32);

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetIRTSAdress");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetupGlobMACModul()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetupGlobMACModul( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortIndex;
    LSA_UINT32  MacCtrl;
    LSA_UINT32  ArcCtrl;
    LSA_UINT32  TraCtrl;
    LSA_UINT32  RcvCtrl;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetupGlobMACModul->");

    /*================== Configuration of MAC ===============================*/
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        //======== Mac-Control =========
        MacCtrl = 0;
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__HaltSndRcv , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__HaltImmed  , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__SoftReset  , 0);
        EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__FullDuplex   , 1);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__MacLoop    , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__ConMode    , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__Loop_10    , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__LinkChanged, 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__MissedRoll , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__EnableMiss , 0);
        //EDDI_SetBitField32(&MacCtrl, EDDI_SER_MAC_CTRL_BIT__LinkStat_10, 0);

        //======== Arc-Control =========
        ArcCtrl = 0;
        //EDDI_SetBitField32(&ArcCtrl, EDDI_SER_ARC_CTRL_BIT__StationAcpt, 0);
        //EDDI_SetBitField32(&ArcCtrl, EDDI_SER_ARC_CTRL_BIT__GroupAcpt  , 0);
        //EDDI_SetBitField32(&ArcCtrl, EDDI_SER_ARC_CTRL_BIT__BroadAcpt  , 0);
        //NegativARC must be 0 => Pause-telegram
        //EDDI_SetBitField32(&ArcCtrl, EDDI_SER_ARC_CTRL_BIT__NegativARC , 0);
        //EDDI_SetBitField32(&ArcCtrl, EDDI_SER_ARC_CTRL_BIT__CompEnable , 0);

        //======== Transmit-Control =========
        TraCtrl = 0;
        EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__TransEn      , 1);
        //EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__TransHalt  , 0);
        EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__SupprPadd    , 1); //Signal von HW auf generelles Unterdruecken geklemmt, don't care
        //EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__SupprCRC   , 0);
        //EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__FastBackOff, 0);
        //EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__NoExcessive, 0);
        //EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__SendPause  , 0);
        //EDDI_SetBitField32(&TraCtrl, EDDI_SER_TRANS_CTRL_BIT__MII_10_Mode, 0);

        //======== Receive-Control =========
        RcvCtrl = 0;
        EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__RcvEnable, 1);
        //EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__RcvHalt       , 0);
        EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__LongEnable    , 1);
        EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__ShortEnable   , 1);
        EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__CRCStrip      , 1);
        //EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__PassCtrl      , 0);
        //EDDI_SetBitField32(&RcvCtrl, EDDI_SER_RCV_CTRL_BIT__IngnoreCRCVal , 0);

        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_MAC_CONTROL, MacCtrl, pDDB);
        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_ARC_CTRL,    ArcCtrl, pDDB);
        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_TRANS_CTRL,  TraCtrl, pDDB);
        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_RCV_CTRL,    RcvCtrl, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetupGlobMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetupGlobMAC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortIndex;
    LSA_UINT32  MinPream;
    LSA_UINT32  NRTCtrl;
    LSA_UINT32  VlanTag;
    LSA_UINT32  MinIdleTime;
    LSA_UINT32  MacEmptyCnt;
    LSA_UINT32  SafetyMargin;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetupGlobMAC->");

    /*================== Configuration of Global-Mac-Parameter ==============*/
    MinIdleTime  = 0;
    MacEmptyCnt  = 0;
    SafetyMargin = 0;

    EDDI_SetBitField32(&MinIdleTime, EDDI_SER_MIN_IDLE_TIME_BIT__MinIdleTime10MBit, pDDB->pConstValues->MinIdleTime10Mbit);
    EDDI_SetBitField32(&MinIdleTime, EDDI_SER_MIN_IDLE_TIME_BIT__MinIdleTime100MBit, pDDB->pConstValues->MinIdleTime100Mbit);

    EDDI_SetBitField32(&MacEmptyCnt, EDDI_SER_MAC_EMPTY_COUNT_BIT__MacEmpty10MBit, pDDB->pConstValues->MacEmptyCnt10Mbit);
    EDDI_SetBitField32(&MacEmptyCnt, EDDI_SER_MAC_EMPTY_COUNT_BIT__MacEmpty100MBit, pDDB->pConstValues->MacEmptyCnt100Mbit);

    EDDI_SetBitField32(&SafetyMargin, EDDI_SER_NRT_SAFETY_MARGIN_BIT__Bd10, pDDB->pConstValues->NRTSafetyMargin10Mbit);
    EDDI_SetBitField32(&SafetyMargin, EDDI_SER_NRT_SAFETY_MARGIN_BIT__Bd100,  pDDB->pConstValues->NRTSafetyMargin100Mbit);

    IO_x32(MIN_IDLE_TIME)     = MinIdleTime;
    IO_x32(MAC_EMPTY_COUNT)   = MacEmptyCnt;
    IO_x32(NRT_SAFETY_MARGIN) = SafetyMargin;

    pDDB->pLocal_SWITCH->DisableLinkChangePHYSMI = pDDB->pConstValues->DisableLinkChangePHYSMI;

    /*================== Configuration of Port ==============*/
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        NRTCtrl  = 0;
        VlanTag  = 0;
        MinPream = 0;

        //take speed and mode
        switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
        {
	        case EDD_MEDIATYPE_COPPER_CABLE:
	        {
		        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config = pDDB->pConstValues->LinkSpeedMode_Copper;
		        break;
	        }
	        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
	        {
		        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config = pDDB->pConstValues->LinkSpeedMode_FO;
		        break;
	        }
	        default:
	        {
		        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config = pDDB->pConstValues->LinkSpeedMode_Other;
	        }
        }

        //startup of Phy
        pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].PhyPowerOff = pDDB->pConstValues->PhyPowerOff;

        //======== NRT-Port-Control =========
        //EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__Baudrate, 0); //Adjusted by LinkSpeedMode_Config
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__FullDuplex,       1); //Adjusted by LinkSpeedMode_Config

        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnCutThrough,
                            (LSA_FALSE != pDDB->pConstValues->EnCutThrough  ? 1:0));

        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnSRTPriority,
                            (LSA_FALSE != pDDB->pConstValues->EnSRTPriority    ? 1:0));

        //EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnLearning,      0); //Adjusted by SpanningTreeZustand;
        
        //MC-learning is switched off here, if the port has no MRPD-configuration it is switched on later
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__MCLearningEn, 0);

        #if defined (EDDI_CFG_ENABLE_PADDING_CLASS12)
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnPadding, 1);
        #else
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnPadding, 0);
        #endif

        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__IngressFilter,
                            (LSA_FALSE !=pDDB->pConstValues->IngressFilter    ? 1:0));

        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnCheckSA,
                            (LSA_FALSE !=pDDB->pConstValues->EnCheckSA        ? 1:0));

        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnISRT,
                            (LSA_FALSE !=pDDB->pConstValues->EnISRT           ? 1:0));

        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__EnShortFrame,  0); //short frames are no longer supported

        #if defined(EDDI_CFG_REV6) || defined(EDDI_CFG_REV7)
        EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__Add_Line_Delay         , 1); // Add Line_Delay during reception
        //EDDI_SetBitField32(&NRTCtrl, EDDI_SER_NRT_CTRL_BIT__Sync_Redundance_Enable,  0);
        #endif

        //======== VLAN-Tag =========
        EDDI_SetBitField32(&VlanTag, EDDI_SER_DEF_VLAN_TAG_BIT__VLANAdr,     pDDB->pConstValues->DefVLANAdress);
        EDDI_SetBitField32(&VlanTag, EDDI_SER_DEF_VLAN_TAG_BIT__DefVLANPrio, pDDB->pConstValues->DefVLANPrio);

        //======== Min-Preamble-Count =======
        EDDI_SetBitField32(&MinPream, EDDI_SER_MIN_PREAMBLE_BIT__MinPreamble, MIN_PREAMBLE_100MBIT);
        pDDB->pLocal_SWITCH->MinPreamble100Mbit = MIN_PREAMBLE_100MBIT;

        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_MIN_PREAMBLE,     MinPream, pDDB);
        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_NRT_CONTROL,      NRTCtrl,  pDDB);
        EDDI_SwiMiscSetRegisterNoSwap(HwPortIndex, REG_DEFAULT_VLAN_TAG, VlanTag,  pDDB);

        //======== SpanningTreeState =========
        //Set all ports to DISABLE. The Learning-Bit has to be switched off !!
        EDDI_SwiPortSetSpanningTree(HwPortIndex, (LSA_UINT16)EDD_PORT_STATE_DISABLE, pDDB);

        pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex] = 0; //clear accumulated drop-events during port in CLOSED-state
        pDDB->LocalIFStats.bOpen[HwPortIndex]          = LSA_TRUE;
        
        /* If this is a MRPRedundantPort we always set BLOCKING! */
        if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].bMRPRedundantPort)
        {
            //Set Spanning-Tree-State after the 1st LinkUp.
            pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState = EDD_PORT_STATE_BLOCKING;
        }
        else
        {
            //Set Spanning-Tree-State after the 1st LinkUp.
            pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState = pDDB->pConstValues->SpanningTreeState;
        }

        //======== LineDelay =========
        //Let IRTE know about the current LineDelay
        EDDI_SwiMiscSetRegister(HwPortIndex, REG_LINE_DELAY,
                                pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LineDelayInTicksHardware, pDDB);
    }

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetupGlobMAC<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetupSWI()
*
* function:      
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetupSWI( SER_HANDLE_PTR           const  pHandle,
                                                         EDDI_UPPER_DSB_PTR_TYPE  const  pDSB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT    Index;
    LSA_UINT32  Map;
    LSA_UINT32  CHxMap;
    LSA_UINT32  DefPort;
    LSA_UINT32  Mask;
    LSA_UINT32  NRTTransCtrl;
    LSA_UINT32  SWITCHMode;
    LSA_UINT32  adr;
    LSA_UINT32  UsrPortIndex;
    LSA_UINT32  Port;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetupSWI->");

    //default SWITCH-SETUP
    SWITCHMode = 0;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__NRT_API_Access, 0); //0: overlapping access from NRT-API
    #else
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__NRT_API_Access, 1); //1: no overlapping access from NRT-API
    #endif
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__MACChargMode , 1); //0: Half-Full is used
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__IRTDMAAcces  , 1); //1: IRT-DMA at IDA-Bus
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__NRTChxTxMode , 1); //1: Discard if LowWaterMark is reached

    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__NRTChxRxMode , 1); //1: use NRT-DMA even if buffer is too small

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__NRTDischargMode      , 1); //0: Receive buffer is freed during NRT transfer already
    //EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__SRTAPIChargeMode   , 0); //no SRT_FCW_Count optimization
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__Confirmation_SndTime , 1); //TimeStemp of PT Frame in DMACW !

    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__Max_TxStat_Valuation    , 1);
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__Ignore_Substitute_Frame , 1);
    #endif

    #if defined (EDDI_CFG_REV7)
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__RToverUDP_Enable        , 1);
    //EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__NRT_API_DELAY_Enable  , 0); //PCI ??
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__Confirmation_WR_Done    , 1); //Read back to prevent an interrupt from overtaking posted writes.
    //only relevant for IRT-DMA and Trace-Memory
    EDDI_SetBitField32(&SWITCHMode, EDDI_SER_SWITCH_SETUP_BIT__Extended_Delay          , 1); //1ns

    IO_W32(IO_CONTROL_DELAY, 0x2C);
    #endif

    IO_x32(SWITCH_SETUP_MODE) = SWITCHMode;

    //default NRT-SS-QUEUE disable
    IO_W32(SS_QUEUE_DISABLE, (NRT_API_CTRL_RESET
                              | NRT_API_CTRL_CHA0
                              | NRT_API_CTRL_CHA1
                              | NRT_API_CTRL_CHB0
                              | NRT_API_CTRL_CHB1));

    //default NRT-CHXx-Control disable
    IO_x32(NRT_ENABLE_CHA0) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_TX_ENABLE | NRT_RX_ENABLE); //disable TX/RX
    IO_x32(NRT_ENABLE_CHA1) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_TX_ENABLE | NRT_RX_ENABLE); //disable TX/RX
    IO_x32(NRT_ENABLE_CHB0) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_TX_ENABLE | NRT_RX_ENABLE); //disable TX/RX
    IO_x32(NRT_ENABLE_CHB1) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_TX_ENABLE | NRT_RX_ENABLE); //disable TX/RX

    //====== NRT-Transfer-Ctrl ========
    NRTTransCtrl = 0;

    pDDB->NRT.MaxSndLen = EDD_NRT_MAX_SND_LEN;

    EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__TagMode, 0); //no tagged mode

    EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__NRT_CT_Delay, 0x04);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_ARP_Filter, 0);
    //EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_DCP_Filter, 0);

    EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__b_2Port_CT_Mode,
                       (pDDB->pConstValues->TwoPortCTMode ? 1:0));
    #endif

    IO_x32(NRT_TRANS_CTRL) = NRTTransCtrl;

    //Ini NRT-Ctrl
    IO_W32(NRT_CTRL_BASE_ADR, DEV_kram_adr_to_asic_register(pHandle->pCTRLDevBaseAddr, pDDB));
    IO_W32(NRT_FCW_COUNT, pDSB->NRTPara.NRT_FCW_Count);
    //Ini FREE-Db
    IO_W32(FREE_CTRL_BASE_ADR, DEV_kram_adr_to_asic_register(pHandle->pCCWDevBaseAddr, pDDB));
    IO_W32(NRT_DB_COUNT, pDSB->NRTPara.NRT_DB_Count);

    //NRT-CHA-Control
    CHxMap = EDDI_NRT_RCV_CHA_MAP_A0_ALONE;
    IO_W32(NRT_RCV_MAP_CHA, CHxMap);

    //NRT-CHB-Control
    CHxMap = EDDI_NRT_RCV_CHA_MAP_B0_ALONE;
    IO_W32(NRT_RCV_MAP_CHB, CHxMap);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //Ini Switch-Parameter
    IO_W32(NRT_FCW_LIMIT,       pHandle->NRT_FCW_Limit_Up);
    IO_W32(NRT_FCW_LIMIT_DOWN,  pHandle->NRT_FCW_Limit_Down);
    IO_W32(NRT_DB_LIMIT,        pHandle->NRT_DB_Limit_Up);
    IO_W32(NRT_DB_LIMIT_DOWN,   pHandle->NRT_DB_Limit_Down);
    IO_W32(HOL_LIMIT_CH,        pHandle->HOL_Limit_CH_Up);
    IO_W32(HOL_LIMIT_CH_DOWN,   pHandle->HOL_Limit_CH_Down);
    IO_W32(HOL_LIMIT_PORT,      pHandle->HOL_Limit_Port_Up);
    IO_W32(HOL_LIMIT_PORT_DOWN, pHandle->HOL_Limit_Port_Down);
    #else
    //Ini Switch-Parameter
    IO_W32(NRT_FCW_LIMIT,       pHandle->NRT_FCW_Limit_Down);
    IO_W32(NRT_DB_LIMIT,        pHandle->NRT_DB_Limit_Down);
    IO_W32(HOL_LIMIT_PORT,      pHandle->HOL_Limit_Port_Up);
    IO_W32(HOL_LIMIT_CH,        pHandle->HOL_Limit_CH_Up);
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    IO_W32(UCMC_LFSR_CTRL,      pHandle->UCMC_LFSR_Mask);

    //Ini UC-Table
    IO_W32(UC_TABLE_BASE,       DEV_kram_adr_to_asic_register(pHandle->pDev_UCMC_Table_Base, pDDB));
    IO_W32(UC_TABLE_LENGTH,     pHandle->UCMC_Table_Length);
    IO_W32(UC_TABLE_RANGE,      pHandle->UCMC_Table_Range);

    //Ini MC-Table
    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pHandle->bMCTableEnabled)
    {
        IO_W32(MC_TABLE_BASE,    DEV_kram_adr_to_asic_register(pHandle->pDev_MC_Table_Base, pDDB));
        IO_W32(MC_TABLE_LENGTH,  (pDDB->pConstValues->MC_MaxSubTable - 1)); 
    }
    else
    #endif
    {
        IO_W32(MC_TABLE_BASE,   SER10_NULL_PTR);
        IO_W32(MC_TABLE_LENGTH, 0);
    }
    #else
    //Ini UCMC-Table
    IO_W32(UCMC_TABLE_BASE,     DEV_kram_adr_to_asic_register(pHandle->pDev_UCMC_Table_Base, pDDB));
    IO_W32(UCMC_TABLE_LENGTH,   pHandle->UCMC_Table_Length);
    IO_W32(UCMC_LFSR_CTRL,      pHandle->UCMC_LFSR_Mask);
    IO_W32(UCMC_TABLE_RANGE,    pHandle->UCMC_Table_Range);
    #endif

    //Ini UC-Default-Port
    DefPort = 0;
    if (pDDB->pConstValues->UCDefaultControlCHA)
    {
        DefPort |= NRT_DEF_CTRL_CHA;
    }

    if (pDDB->pConstValues->UCDefaultControlCHB)
    {
        DefPort |= NRT_DEF_CTRL_CHB;
    }

    //Port0 = 0x04, Port1= 0x08, Port2 = 0x10; Port3 = 0x20
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        if (pDDB->pConstValues->UCDefaultControlDestPort)
        {
            Port = EDDI_PmGetMask0420(pDDB, UsrPortIndex);

            DefPort |= Port;
        }
    }
    IO_W32(UC_DEFAULT_CTRL, DefPort);

    //Ini MC-Default-Port
    DefPort = 0;
    if (pDDB->pConstValues->MCDefaultControlCHA)
    {
        DefPort |= NRT_DEF_CTRL_CHA;
    }

    if (pDDB->pConstValues->MCDefaultControlCHB)
    {
        DefPort |= NRT_DEF_CTRL_CHB;
    }

    //Port0 = 0x04, Port1= 0x08, Port2 = 0x10; Port3 = 0x20
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        if (pDDB->pConstValues->MCDefaultControlDestPort)
        {
            Port = EDDI_PmGetMask0420(pDDB, UsrPortIndex);
            DefPort |= Port;
        }
    }

    IO_W32(MC_DEFAULT_CTRL, DefPort);

    //Ini VLAN-Table
    IO_W32(VLAN_TABLE_BASE,   SER10_NULL_PTR);  //no vlan-table
    IO_W32(VLAN_TABLE_LENGTH, 0);

    IO_W32(VLAN_LFSR_MASK, pHandle->vlan.LFSR_Mask);

    //Buffer for Statistic Counters
    adr = (LSA_UINT32)pHandle->pDev_StatisticBaseAddr % SER_STATISTIC_BASE_OFFSET;
    adr = (LSA_UINT32)pHandle->pDev_StatisticBaseAddr + (SER_STATISTIC_BASE_OFFSET - adr);
    IO_W32(STAT_CTRL_BASE_ADR, DEV_kram_adr_to_asic_register((void *)adr, pDDB));

    //Ini LOW-WATER-MARK
    Mask = 0;
    Map  = 1UL;
    for (Index = 0; Index < EDDI_NRT_MAX_PRIO; Index++)
    {
        if (pDDB->pConstValues->FCMaskPrio[Index])
        {
            Mask |= Map;
        }
        Map = Map << 1;
    }
    IO_W32(FC_MASK, Mask);

    //Ini HOL-MASK
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        Mask = 0;
        Map  = 1UL;

        for (Index = 0; Index < EDDI_NRT_MAX_PRIO; Index++)
        {
            if (pDDB->pConstValues->HOLMaskPrio[Index])
            {
                Mask |= Map;
            }
            Map = Map << 1;
        }

        EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_HOL_MASK, Mask, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetupNRT()
*
* function:      create structs für NRT-Communication
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetupNRT( SER_HANDLE_PTR           const  pHandle,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32            Ctr;
    LSA_UINT32            QueueIdx;
    volatile  LSA_UINT32  CHx;
    LSA_UINT32            CHxCtrl;
    LSA_UINT32            nrt_enable_CHx;
    LSA_UINT32            nrt_snd_descr_CHx;
    LSA_UINT32            nrt_rcv_descr_CHx;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetupNRT->");

    CHx = 0;
    //CardType = IO_x32(SWI_VERSION);

    //if(!((EDDI_GetBitField32(CardType, EDDI_SER_VERSION_BIT__Variante) == 0x8) ||
    //     (EDDI_GetBitField32(CardType, EDDI_SER_VERSION_BIT__Variante) == 0x9))) //only for IP-Test
    if (!((pDDB->ERTEC_Version.Variante == 0x8) || (pDDB->ERTEC_Version.Variante == 0x9))) //only for IP-Test
    {
        //disable all CHx-Interfaces
        for (QueueIdx = EDDI_NRT_CHA_IF_0; QueueIdx < EDDI_NRT_IF_CNT; QueueIdx++) //A0, B0
        {
            switch (QueueIdx)
            {
                case EDDI_NRT_CHA_IF_0:
                    nrt_enable_CHx = NRT_ENABLE_CHA0;
                    break;
                case EDDI_NRT_CHB_IF_0:
                    nrt_enable_CHx = NRT_ENABLE_CHB0;
                    break;
                default:
                {
                    EDDI_Excp("EDDI_SERSetupNRT, invalid QueueIdx:", EDDI_FATAL_ERR_EXCP, QueueIdx, 0);
                    return;
                }
            }

            IO_x32(nrt_enable_CHx) = (NRT_TX_ENABLE | NRT_RX_ENABLE); //reset TX/RX
            for (Ctr = 0; Ctr < SER_POLL_TIMEOUT; Ctr++)
            {
                CHx = IO_x32(nrt_enable_CHx);
                if (CHx & NRT_TX_ENABLE)
                {
                    continue;
                }

                break;
            }

            if (Ctr == SER_POLL_TIMEOUT)
            {
                EDDI_Excp("EDDI_SERSetupNRT, SER_POLL_TIMEOUT", EDDI_FATAL_ERR_EXCP, Ctr, 0);
                return;
            }
        }

        //Attention; do not merge loops -> will lead to error interrupt
        //init NRT-Descriptor
        for (QueueIdx = EDDI_NRT_CHA_IF_0; QueueIdx < EDDI_NRT_IF_CNT; QueueIdx++) //A0, B0
        {
            SER_TOP_DMACW_ADR_TYPE  *  const  pTopAdrDMACW = &pHandle->TopAdrDMACW[QueueIdx];
            LSA_UINT32                        tmp;

            switch (QueueIdx) //WorkAround <-> NRT-SS-QUEUE disable
            {
                case EDDI_NRT_CHA_IF_0:
                {
                    nrt_snd_descr_CHx = NRT_SND_DESCRIPTOR_CHA0;
                    nrt_rcv_descr_CHx = NRT_RCV_DESCRIPTOR_CHA0;
                    break;
                }
                case EDDI_NRT_CHB_IF_0:
                {
                    nrt_snd_descr_CHx = NRT_SND_DESCRIPTOR_CHB0;
                    nrt_rcv_descr_CHx = NRT_RCV_DESCRIPTOR_CHB0;
                    break;
                }
                default:
                {
                    EDDI_Excp("EDDI_SERSetupNRT, invalid QueueIdx:", EDDI_FATAL_ERR_EXCP, QueueIdx, 0);
                    return;
                }
            }

            //init NRT-Descriptor-TX
            IO_x32(nrt_snd_descr_CHx) = pTopAdrDMACW->Tx;
            tmp = IO_x32(nrt_snd_descr_CHx);
            if (tmp != pTopAdrDMACW->Tx)
            {
                EDDI_Excp("EDDI_SERSetupNRT, invalid pTopAdrDMACW->Tx", EDDI_FATAL_ERR_EXCP, pTopAdrDMACW->Tx, tmp);
                return;
            }

            //init NRT-Descriptor-RX
            IO_x32(nrt_rcv_descr_CHx) = pTopAdrDMACW->Rx;
            tmp = IO_x32(nrt_rcv_descr_CHx);
            if (tmp != pTopAdrDMACW->Rx)
            {
                EDDI_Excp("EDDI_SERSetupNRT, invalid pTopAdrDMACW->Rx", EDDI_FATAL_ERR_EXCP, pTopAdrDMACW->Rx, tmp);
                return;
            }
        } //end of for-loop
    }

    //disable all NRT channels
    CHxCtrl = 0;
    EDDI_SetBitField32(&CHxCtrl, EDDI_SER_NRT_API_CTRL_BIT__S_R,  1);  /* set disable bit */
    EDDI_SetBitField32(&CHxCtrl, EDDI_SER_NRT_API_CTRL_BIT__CHA0, 1);  /* select NRT channel A0 */
    EDDI_SetBitField32(&CHxCtrl, EDDI_SER_NRT_API_CTRL_BIT__CHA1, 1);  /* select NRT channel A1 */
    EDDI_SetBitField32(&CHxCtrl, EDDI_SER_NRT_API_CTRL_BIT__CHB0, 1);  /* select NRT channel B0 */
    EDDI_SetBitField32(&CHxCtrl, EDDI_SER_NRT_API_CTRL_BIT__CHB1, 1);  /* select NRT channel B1 */

    IO_x32(SS_QUEUE_DISABLE) = CHxCtrl;

    //insert Receives
    for (QueueIdx = EDDI_NRT_CHA_IF_0; QueueIdx < EDDI_NRT_IF_CNT; QueueIdx++) //A0, B0
    {
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF        = &pDDB->pLocal_NRT->IF[QueueIdx];
        NRT_DSCR_PTR_TYPE                  pReqRxDscr = pIF->Rx.Dscr.pReq;

        //disable nrt-channel for receiving
        IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_DISABLE_BIT | NRT_RX_ENABLE);

        for (Ctr = pIF->Rx.Dscr.Cnt; Ctr; Ctr--)
        {
            //ATTENTION: do not change!
            #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
            //invalidate nrt-cache
            EDDI_SYNC_CACHE_NRT_RECEIVE(pReqRxDscr->pRcvDataBuffer, EDDI_FRAME_BUFFER_LENGTH);
            #endif

            //set DMACW-owner to IRTE
            EDDI_SET_SINGLE_BIT(pReqRxDscr->pDMACW->L0.Reg, BM_S_DMACW_OwnerHW_29);

            pReqRxDscr = pReqRxDscr->next_dscr_ptr;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERAcwSetup()
*
* function:      
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERAcwSetup( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERAcwSetup");

    IO_W32(ACW_BASE_ADR, DEV_kram_adr_to_asic_register(pDDB->SRT.pACWSerBase, pDDB));

    /************************************************************************/
    /* Here the ACW-Recv-Lists are connected !                              */
    /* They are needed for Consumers and SyncSlaves !                       */
    /************************************************************************/
    IO_W32(ACW_MASK, pDDB->SRT.ACWRxMask);
    //activate ACW-RX-List

    EDDI_SERSingleDirectCmd(FCODE_ENABLE_ACW_RX_LIST, 0UL, 0, pDDB);

    // Finally connect SerAcwHeader to first Entry in KRAM-Tree;
    /************************************************************************/
    /* Here the KRAM-Send-Tree is connected to header !                     */
    /************************************************************************/

    EDDI_SERConnectCwToHeader(pDDB,
                              (EDDI_LOCAL_LIST_HEAD_PTR_TYPE)(LSA_VOID *)pDDB->SRT.pACWTxHeadCtrl->pKramListHead,
                              (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pDDB->CRT.RTClass2Interval.pRootAcw); //temporarily_disabled_lint !e826 Warning 826: Suspicious pointer-to-pointer conversion (area too small)

}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SERSetup()                             +*/
/*+  Input                 :    ...                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  description of input/output                                            +*/
/*+                                                                         +*/
/*+  pHandle    :  Pointer to initialized Handle.                           +*/
/*+  pDSB       :  Pointer to Setup-parameters                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:  Setup       Ethernetcontroller with spezified parameters +*/
/*+                                                                         +*/
/*+                - PHY                                                    +*/
/*+                - MAC                                                    +*/
/*+                - Receive/Transmit-Bufferunits                           +*/
/*+                - Receive/Transmit-DMA-Units                             +*/
/*+                                                                         +*/
/*+                Setup descriptor rings for transmit and receive.         +*/
/*+                Setup Interrupt-masks (enables all needed sources)       +*/
/*+                                                                         +*/
/*+                EIB_ERR_PARAM may occur if the number of rx/tx-descr.    +*/
/*+                exceeds EIB_MAX_RX_DSCR or EIB_MAX_TX_DSCR               +*/
/*+                                                                         +*/
/*+                EIB_ERR_TIMEOUT may occur if access to Ethernetcontroller+*/
/*+                times out.                                               +*/
/*+                                                                         +*/
/*+                EibSetup() should be called after EibInit().             +*/
/*+                                                                         +*/
/*+                After this function rx/tx can be started via EibStart(). +*/
/*+                                                                         +*/
/*+                Interrupthandlers have to be installed and interrupts    +*/
/*+                have to be enabled when calling (needed for Commands to  +*/
/*+                units)                                                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetup( SER_HANDLE_PTR           const  pHandle,
                                              EDDI_UPPER_DSB_PTR_TYPE  const  pDSB,
                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetup->");

    /*-------------------------------------------------------------------------*/
    /* Setup Port, Global Parameter                                            */
    /*-------------------------------------------------------------------------*/
    EDDI_SERSetupGlobMAC(pDDB);

    /*-------------------------------------------------------------------------*/
    /* Setup MAC-Modul                                                         */
    /*-------------------------------------------------------------------------*/
    EDDI_SERSetupGlobMACModul(pDDB);

    /*-------------------------------------------------------------------------*/
    /* SWI-Setup                                                               */
    /*-------------------------------------------------------------------------*/
    EDDI_SERSetupSWI(pHandle, pDSB, pDDB);

    /*-------------------------------------------------------------------------*/
    /* Special module-Setup                                                    */
    /*-------------------------------------------------------------------------*/
    EDDI_SERSetupxRT(pDSB, pDDB);

    /*-------------------------------------------------------------------------*/
    /* set Switch-macro to Betriebsmode                                        */
    /*-------------------------------------------------------------------------*/
    EDDI_SERStartErtec(pDSB, pDDB);  //counter is running

    /*-------------------------------------------------------------------------*/
    /* NRT-Setup                                                               */
    /*-------------------------------------------------------------------------*/
    EDDI_SERSetupNRT(pHandle, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERImageDMASetup()
*
* function:      Sets up thew DMA-Setting for ProcessImage
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERImageDMASetup( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                const EDDI_CRT_SYNC_IMAGE_TYPE  *  const  pSyncImage )
{
    LSA_UINT32  ImageMode;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERImageDMASetup->");

    //Preserve the SyncAPIStart mode
    ImageMode = IO_x32(IMAGE_MODE);

    #if defined(EDDI_CFG_REV7)
    if (0 != pDDB->CRT.PAEA_Para.PAEA_BaseAdr)
    {
        EDDI_SetBitField32(&ImageMode, EDDI_SER_IMAGE_MODE_BIT__BufMode, SER_SYNC_IMAGE_UNBUF_MODE);
    }
    else
    {
        EDDI_SetBitField32(&ImageMode, EDDI_SER_IMAGE_MODE_BIT__BufMode, SER_SYNC_IMAGE_BUF_MODE);
    }
    #else
    EDDI_SetBitField32(&ImageMode, EDDI_SER_IMAGE_MODE_BIT__BufMode, SER_SYNC_IMAGE_BUF_MODE);
    #endif

    /*  switch (pSyncImage->ImageExpert.BufferMode)
      {
        case EDDI_SYNC_IMAGE_BUF_MODE:           ImageMode.---.BufMode = SER_SYNC_IMAGE_BUF_MODE;   break;
        case EDDI_SYNC_IMAGE_UNBUF_MODE:         ImageMode.---.BufMode = SER_SYNC_IMAGE_UNBUF_MODE; break;
        case EDDI_SYNC_IMAGE_CHBUF_MODE:         ImageMode.---.BufMode = SER_SYNC_IMAGE_CHBUF_MODE; break;
        case EDDI_SYNC_IMAGE_OFF:                ImageMode.---.BufMode = SER_SYNC_IMAGE_NO_MODE;    break;
        default:
        {
          EDDI_Excp("EDDI_SERImageDMASetup BufferMode", EDDI_FATAL_ERR_EXCP, pSyncImage->ImageExpert.BufferMode, 0);
          return;
        }
      } */

    switch (pSyncImage->ImageExpert.TriggerMode)
    {
        case EDDI_SYNC_TRIGMODE_IRT_END:
        {
            break;
        }
        case EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC:
        {
            if (EDD_FEATURE_ENABLE == pDDB->CRT.CfgPara.bUseTransferEnd)
            {
                break;
            }
        }
        //no break;
        //lint -fallthrough
        case EDDI_SYNC_TRIGMODE_ISRT_END:
        case EDDI_SYNC_TRIGMODE_DEADLINE:
        default:
        {
            EDDI_Excp("EDDI_SERImageDMASetup TriggerMode", EDDI_FATAL_ERR_EXCP, pSyncImage->ImageExpert.TriggerMode, 0);
            return;
        }
    }

    EDDI_SetBitField32(&ImageMode, EDDI_SER_IMAGE_MODE_BIT__SyncAPI, pSyncImage->ImageExpert.TriggerMode);

    EDDI_SetBitField32(&ImageMode, EDDI_SER_IMAGE_MODE_BIT__EnAsyncDMA, 0);

    /*
    if (pCRTIni->ProcessImage.AsyncDMA)
    {
      ImageMode.---.EnAsyncDMA = IMAGE_ENABLE_ASYNC_DMA;
    }
    else
    {
      ImageMode.---.EnAsyncDMA = 0;
    }
    */

    IO_x32(IMAGE_MODE) = ImageMode;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetIrtPortStartTimeTx()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIrtPortStartTimeTx( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                              EDDI_CRT_PHASE_TX_MACHINE  *  const  pMachine,
                                                              EDDI_CRT_PHASE_TX_EVENT       const  Event,
                                                              LSA_UINT32                    const  StartTime10Ns )
{
    if (StartTime10Ns >= pDDB->CycCount.CycleLength_10ns)
    {
        EDDI_Excp("EDDI_SERSetIrtPortStartTimeTx 1, StartTime10Ns too Big", EDDI_FATAL_ERR_LL, StartTime10Ns, (pDDB->CycCount.CycleLength_10ns));
        return;
    }

    if (pMachine->HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_SERSetIrtPortStartTimeTx 2, HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_LL, pMachine->HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return;
    }

    //store current StartTime
    pMachine->StartTime10Ns = StartTime10Ns;
    //insert StartTime into SOL
    EDDI_SERUpdateSOL(&pMachine->pIrtPortStartTimeTxBlock->Sol, StartTime10Ns);

    switch (Event)
    {
        case EDDI_CRT_PHASE_TX_EVT_SHIFT:
        {
            volatile  LSA_UINT32  const  StartTime10NsAsic = EDDI_HOST2IRTE32(StartTime10Ns);

            //insert StartTime into Dummy-FCW
            pMachine->pIrtPortStartTimeTxBlock->FcwSnd.Hw1.Value.U32_1 = StartTime10NsAsic;

            //connect LMW with FCW
            EDDI_SERConnectCwToNext(pDDB,
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeTxBlock->Lmw,      
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeTxBlock->FcwSnd);  //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

            //connect SOL with EOLmax
            EDDI_SERConnectCwToNext(pDDB,
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeTxBlock->Sol,      
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pMachine->pEOLMax);                            //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

            //insert Shift-List into List-Header
            *pMachine->pFcwListHead = pMachine->pShiftListAsic; //regular: EDDI_SERConnectCwToHeader()
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT:
        {
            //connect SOL with Productive-List
            EDDI_SERConnectCwToNext(pDDB,
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeTxBlock->Sol,      
                                    pMachine->pRootCw);                                                               //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

            //insert SOL into List-Header
            *pMachine->pFcwListHead = pMachine->pShiftSolAsic; //regular: EDDI_SERConnectCwToHeader()
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT:
        {
            //connect SOL with NULL-Pointer
            EDDI_SERConnectCwToNext(pDDB,
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeTxBlock->Sol,      
                                    (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)EDDI_NULL_PTR);                                //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

            //insert SOL into List-Header
            *pMachine->pFcwListHead = pMachine->pShiftSolAsic; //regular: EDDI_SERConnectCwToHeader()
        }
        break;

        case EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE:
        case EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH:
        case EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH:
        case EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH:
        default:
        {
            EDDI_Excp("EDDI_SERSetIrtPortStartTimeTx 3, ERROR invalid Event", EDDI_FATAL_ERR_LL, Event, 0);
            return;
        }
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetIrtPortStartTimeRx()
*
* function:
*
* parameters:    StartTime10Ns: 0:  unshifting StartTime
*                               >0: shifting StartTime
*
* return value:  LSA_VOID
*
*===========================================================================*/
#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIrtPortStartTimeRx( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                              EDDI_CRT_PHASE_RX_MACHINE  *  const  pMachine,
                                                              LSA_UINT32                    const  StartTime10Ns )
{
    if (StartTime10Ns >= pDDB->CycCount.CycleLength_10ns)
    {
        EDDI_Excp("EDDI_SERSetIrtPortStartTimeRx 1, StartTime10Ns too big", EDDI_FATAL_ERR_LL, StartTime10Ns, pDDB->CycCount.CycleLength_10ns);
        return;
    }

    if (pMachine->HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_SERSetIrtPortStartTimeRx 2, Wrong HwPortIndex", EDDI_FATAL_ERR_LL, pMachine->HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return;
    }

    //store current StartTime
    pMachine->StartTime10Ns = StartTime10Ns;
    //insert StartTime into SOL
    EDDI_SERUpdateSOL(&pMachine->pIrtPortStartTimeRxBlock->Sol, StartTime10Ns);

    if (StartTime10Ns)  //shifting StartTime
    {
        volatile  LSA_UINT32  const  StartTime10NsAsic = EDDI_HOST2IRTE32(StartTime10Ns);

        //insert StartTime into Dummy-FCW
        pMachine->pIrtPortStartTimeRxBlock->FcwRcv.Hw1.Value.U32_1 = StartTime10NsAsic;

        //connect LMW with FCW (LMW=switcher)
        EDDI_SERConnectCwToNext(pDDB,
                                (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeRxBlock->Lmw,      //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)
                                (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)&pMachine->pIrtPortStartTimeRxBlock->FcwRcv);  //temporarily_disabled_lint !e826 //Suspicious pointer-to-pointer conversion (area too small)

        //insert Shift-List into List-Header
        *pMachine->pFcwListHead = pMachine->pShiftListAsic; //regular: EDDI_SERConnectCwToHeader()
    }
    else //unshifting StartTime
    {
        //insert SOL into List-Header
        *pMachine->pFcwListHead = pMachine->pShiftSolAsic; //regular: EDDI_SERConnectCwToHeader()
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetIrtPortActivityTx()
*
* function:      Function must be protected when calling from different locations!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIrtPortActivityTx( LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bTxActivity,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         LocalRegIrtCtrl;
    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    LSA_UINT32         const  BitMask = 0x00000001UL << (2 * HwPortIndex);
    #else
    LSA_UINT32         TwoBitMask;
    LSA_UINT32         TwoBitValue;
    #endif

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERSetIrtPortActivityTx -> HwPortIndex:0x%X bTxActivity:0x%X", HwPortIndex, bTxActivity);

    //read register IRT_Control
    LocalRegIrtCtrl = IO_R32(IRT_CTRL);

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    if (bTxActivity)
    {
        LocalRegIrtCtrl |= BitMask; //set bit
    }
    else
    {
        LocalRegIrtCtrl &= (~BitMask); //clear bit
    }
    #else
    TwoBitMask = 0x00000001UL;
    TwoBitMask = TwoBitMask << (2 * HwPortIndex);

    TwoBitValue = (bTxActivity ? 1:0);
    TwoBitValue = TwoBitValue << (2 * HwPortIndex);

    LocalRegIrtCtrl &= ~TwoBitMask;  //delete old Values
    LocalRegIrtCtrl |=  TwoBitValue; //write new Values
    #endif

    //update register IRT_Control
    IO_W32(IRT_CTRL, LocalRegIrtCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetIrtPortActivityRx()
*
* function:      Function must be protected when calling from different locations!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIrtPortActivityRx( LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bRxActivity,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         LocalRegIrtCtrl;
    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    LSA_UINT32  const  BitMask = 0x00000002UL << (2 * HwPortIndex);
    #else
    LSA_UINT32         TwoBitMask;
    LSA_UINT32         TwoBitValue;
    #endif

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERSetIrtPortActivityRx -> HwPortIndex:0x%X bRxActivity:0x%X", HwPortIndex, bRxActivity);

    //read register IRT_Control
    LocalRegIrtCtrl = IO_R32(IRT_CTRL);

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    if (bRxActivity)
    {
        LocalRegIrtCtrl |= BitMask;  //set bit
    }
    else
    {
        LocalRegIrtCtrl &= (~BitMask);  //clear bit
    }
    #else
    TwoBitMask = 0x00000002UL;
    TwoBitMask = TwoBitMask << (2 * HwPortIndex);

    TwoBitValue = (bRxActivity ? 2:0);
    TwoBitValue = TwoBitValue << (2 * HwPortIndex);

    LocalRegIrtCtrl &= ~TwoBitMask;  // delete old Values
    LocalRegIrtCtrl |=  TwoBitValue; // write  new Values
    #endif

    //update register IRT_Control
    IO_W32(IRT_CTRL, LocalRegIrtCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetIrtPortActivityRxTx()
*
* function:      Function must be protected when calling from different locations!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
#if !defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIrtPortActivityRxTx( LSA_UINT32               const  HwPortIndex,
                                                               LSA_BOOL                 const  bRxTxActivity,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SER_IRT_CTRL_TYPE         LocalRegIrtCtrl;
    LSA_UINT32              const  BitMask = 0x00000003UL << (2 * HwPortIndex);

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SERSetIrtPortActivityRxTx -> HwPortIndex:0x%X bRxTxActivity:0x%X", HwPortIndex, bRxTxActivity);

    //read register IRT_Control
    LocalRegIrtCtrl = IO_R32(IRT_CTRL);

    if (bRxTxActivity)
    {
        LocalRegIrtCtrl |= BitMask;  //set bits
    }
    else
    {
        LocalRegIrtCtrl &= (~BitMask);  //clear bits
    }

    //update register IRT_Control
    IO_W32(IRT_CTRL, LocalRegIrtCtrl);
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetIrtGlobalActivity()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetIrtGlobalActivity( LSA_BOOL                 const  bActivate,
                                                             EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  SWICtrl;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERSetIrtGlobalActivity->");

    SWICtrl = IO_R32(SWI_CTRL);

    if (bActivate)
    {
        SWICtrl |= PARA_IRT_CTRL_ENABLE;
    }
    else
    {
        SWICtrl &= ~PARA_IRT_CTRL_ENABLE;
    }

    IO_W32(SWI_CTRL, SWICtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: SERStopIrtGlobalAndPorts()
*
* function:      Stop the IRT-module of the switch macro
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERStopIrtGlobalAndIrtPorts( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  SWICtrl;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SERStopIrtGlobalAndPorts");

    SWICtrl = IO_R32(SWI_CTRL) & ~PARA_IRT_CTRL_ENABLE;

    IO_W32(SWI_CTRL, SWICtrl);

    IO_W32(IRT_CTRL, 0);   // Disable Send/Receive-Lists of all ports
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERStartErtec()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERStartErtec( EDDI_UPPER_DSB_PTR_TYPE const pDSB,
                                                           EDDI_LOCAL_DDB_PTR_TYPE const pDDB )
{
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32         SWICtrl;
    LSA_UINT32         PortMask;
    LSA_UINT32  const  PortMaskArray[] = {PARA_PORT0_ENABLE, PARA_PORT1_ENABLE, PARA_PORT2_ENABLE, PARA_PORT3_ENABLE};

    #if !defined (EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    if (pDSB)
    {
    }
    #endif
    
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERStartErtec");

    PortMask = 0;

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        PortMask |= PortMaskArray[HwPortIndex];
    }

    //switch PHY polling off before switching to on
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        EDDI_SwiPhyEnableLinkIRQ(HwPortIndex, LSA_FALSE, pDDB);
    }

    #if defined (EDDI_CFG_ERTEC_200)
    EDDI_SwiPhyE200ConfigPHY(pDDB);
    #endif

    //SWICtrl = IO_R32(SWI_CTRL);
    #if defined(EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    pDDB->SWITCH.bResetIRTCtrl = (EDD_FEATURE_ENABLE == pDSB->SWIPara.bResetIRTCtrl)?LSA_TRUE:LSA_FALSE;
    if (EDD_FEATURE_ENABLE != pDSB->SWIPara.bResetIRTCtrl)
    {
        SWICtrl = IO_R32(SWI_CTRL) & PARA_IRT_CTRL_ENABLE;  //do not affect IRTCtrl
    }
    else
    #endif
    {
        SWICtrl = 0;
    }
    //PARA_STATI_CTRL_ENABLE will be set later in EDDI_SERIniStatistic()
    SWICtrl |= PARA_SWI_ENABLE
                   | PARA_IRQ_CTRL_ENABLE
                   | PARA_PHY_SMI_CTRL_ENABLE
                   | PARA_KRAM_CTRL_ENABLE
                   | PARA_NRT_CTRL_ENABLE
                   | PARA_CYC_CTRL_ENABLE
                   | PortMask;

    //PHY-/SMI-Control will be enabled by separate service EDDI_SRV_SWITCH_ENABLE_LINK_CHANGE
    if (pDDB->pLocal_SWITCH->DisableLinkChangePHYSMI)
    {
        EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__PHY_SMICtrl,  0);
    }
    
    SWICtrl |= (PARA_SRT_CTRL_ENABLE | PARA_TRACE_CTRL_ENABLE);
    
    //Switch ERTEC in working mode
    IO_W32(SWI_CTRL, SWICtrl);

    //pDDB->pLocal_SWITCH->StartErtecTicks = IO_GET_CLK_COUNT_VALUE_10NS;
    pDDB->pLocal_SWITCH->StartErtecTicks = 0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: SERSrtStop()
*
* function:      Stop the SRT-module of the switch macro
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
/*
LSA_VOID  EDDI_LOCAL_FCT_ATTR  SERSrtStop( EDDI_LOCAL_DDB_PTR_TYPE  pDDB )
{
  EDDI_SER_SWI_CTRL_TYPE  SWICtrl;

  EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "SERSrtStop->");

  SWICtrl =  IO_SWI_CTRL & ~PARA_SRT_CTRL_ENABLE;

  IO_SWI_CTRL = SWICtrl;

  EDDI_SERSingleCommand(FCODE_DISALBE_ACW_TX_LIST, 0, 0, pDDB);
} */
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ser_ini.c                                               */
/*****************************************************************************/
