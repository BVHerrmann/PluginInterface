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
/*  F i l e               &F: eddi_swi_statistic.c                      :F&  */
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
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_swi_ext.h"

#define EDDI_MODULE_ID     M_ID_SWI_STATISTIC
#define LTRC_ACT_MODUL_ID  315

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SERIniStatistic()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERIniStatistic( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                   SWICtrl;
    LSA_UINT32                   SWIStat;
    EDDI_SER_STATS_PTR_TYPE      pStatisticBaseAddr;
    LSA_UINT32                   adr, length, ret;
    EDDI_MEM_BUF_EL_H         *  pMemHeader;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERIniStatistic->");

    SWIStat = IO_x32(SWI_STATUS);
    if (0 == EDDI_GetBitField32(SWIStat, EDDI_SER_SWI_STATUS_BIT__StatiStop) )
    {
        //active -> cannot be!
        EDDI_Excp ("EDDI_SERIniStatistic", EDDI_FATAL_ERR_LL, SWIStat, 0);
        return;
    }

    //alloc memory for statistic counters
    length = (SER_STATISTIC_PORT_SIZE * pDDB->PM.HwTypeMaxPortCnt) + SER_STATISTIC_BASE_OFFSET;
    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pMemHeader, length );

    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_SERIniStatistic, pStatisticBaseAddr, EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }

    pStatisticBaseAddr = (EDDI_SER_STATS_PTR_TYPE)(void *)pMemHeader->pKRam;

    pDDB->KramRes.Statistic = length;

    EDDI_MemSet((void *)pStatisticBaseAddr, (LSA_UINT8)0x0, length);

    pDDB->Glob.LLHandle.pDev_StatisticBaseAddr = pStatisticBaseAddr;

    //buffer for statistic counters
    adr = (LSA_UINT32)pStatisticBaseAddr % SER_STATISTIC_BASE_OFFSET;
    adr = (LSA_UINT32)pStatisticBaseAddr + (SER_STATISTIC_BASE_OFFSET - adr);
    IO_W32(STAT_CTRL_BASE_ADR, DEV_kram_adr_to_asic_register((void *)adr, pDDB));

    //enable statistic control
    SWICtrl = IO_x32(SWI_CTRL);

    EDDI_SetBitField32(&SWICtrl, EDDI_SER_SWI_CTRL_BIT__StatiCtrl, 1);

    IO_x32(SWI_CTRL) = SWICtrl;

    //init local interface counters
    pDDB->LocalIFStats.RxGoodCtr = 0;
    pDDB->LocalIFStats.TxGoodCtr = 0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_swi_statistic.c                                         */
/*****************************************************************************/
