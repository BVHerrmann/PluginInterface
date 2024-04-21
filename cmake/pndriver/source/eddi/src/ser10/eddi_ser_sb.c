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
/*  F i l e               &F: eddi_ser_sb.c                             :F&  */
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
/*  22.03.04    ZA    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_ser_ext.h"

#define EDDI_MODULE_ID     M_ID_SER_SB
#define LTRC_ACT_MODUL_ID  303

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* Timer-Scoreboard-Functions                                                 */
/*---------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  EDDI_SERScoreBoardStart()
 *
 * function:       Starts the Remote-Provider_Surveillance of the selected
 *                 Communication-Class
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERScoreBoardStart( LSA_BOOL                 const  ComClass_1_2,
                                                        LSA_BOOL                 const  ComClass3,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  RemProdCtrl;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERScoreBoardStart");

    RemProdCtrl = 0;

    EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnIRT, ComClass3 ? 1 : 0);
    EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnSRT, ComClass_1_2 ? 1 : 0);
    EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__StartPSAging, 0);   // 0: The ControlUnit "PS-Aging" is started at each NewCycle.
    //    This is essential for asynchronous applications.
    // 1: The ControlUnit "PS-Aging" is started at each TrsnsferEnd_Int.
    //    This can be essential for synchronous applications.
    EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnSCOREBChange, 1);

    IO_x32(PROD_CTRL) = RemProdCtrl;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  EDDI_SERScoreBoardStop()
 *
 * function:       Stops the Remote-Provider_Surveillance of the selected
 *                 Communication-Class
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERScoreBoardStop( LSA_BOOL                 const  ComClass_1_2,
                                                       LSA_BOOL                 const  ComClass3,
                                                       EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  RemProdCtrl;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERScoreBoardStop");

    RemProdCtrl = IO_x32(PROD_CTRL);

    if (ComClass3)
    {
        EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnIRT, 0);
    }
    if (ComClass_1_2)
    {
        EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnSRT, 0);
    }

    if (ComClass_1_2 && ComClass3)
    {
        EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnSCOREBChange, 0);
    }

    IO_x32(PROD_CTRL) = RemProdCtrl;

    if (ComClass_1_2 && ComClass3)
    {
        //Clear all SB entries. Otherwise old SB entries will cause a SBChangedInt when EnSCOREBChange is set to 1 again.
        LSA_UINT32 * pConsSB = (LSA_UINT32 *)(LSA_VOID *)pDDB->CRT.Rps.SBList.pDevSBChangeList[0];
        LSA_UINT32   Size    = pDDB->CRT.Rps.SBList.DevSBChangeListSizeInUint32;

        do
        {
            *pConsSB++ = 0;
            Size--;
        }
        while (Size);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name:  EDDI_SERScoreBoardEnableChange()
 *
 * function:       Enables Double-Buffer-Change of TimerScoreboard
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERScoreBoardEnableChange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  RemProdCtrl;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SERScoreBoardEnableChange");

    RemProdCtrl = IO_x32(PROD_CTRL);
    //enable change SBProv;
    EDDI_SetBitField32(&RemProdCtrl, EDDI_SER_PROD_CTRL_BIT__EnSCOREBChange, 1);

    IO_x32(PROD_CTRL) = RemProdCtrl;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ser_sb.c                                                */
/*****************************************************************************/
