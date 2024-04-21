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
/*  F i l e               &F: eddi_pm.c                                 :F&  */
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
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#define EDDI_MODULE_ID     M_ID_EDDI_PM
#define LTRC_ACT_MODUL_ID  16

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PmCheck( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                             EDDI_UPPER_DPB_PTR_TYPE  const  pDPB )
{
    LSA_UINT32  PortCnt, i, j;
    LSA_UINT32  HWPortID;
    LSA_UINT32  NumberOfHits;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmCheck->");

    //UsrPortID: range is from 1..4  -> 0 is an invalid value !
    pDDB->PM.UsrPortID_to_HWPort_0_3[0] = 0xFFFFFFFFUL;

    for (i = 0; i < (EDD_CFG_MAX_PORT_CNT + 1); i++)
    {
        pDDB->PM.UsrPortID_to_HWPort_0_3[i] = EDDI_PORT_NOT_CONNECTED;
    }

    //Port Mapping
    for (PortCnt = 0; PortCnt < EDD_CFG_MAX_PORT_CNT; PortCnt++)
    {
        NumberOfHits = 0;
        HWPortID     = pDPB->PortMap.UsrPortID_x_to_HWPort_y[PortCnt];

        if (HWPortID == 0)
        {
            EDDI_Excp("EDDI_PmCheck - pDPB->PortMap.UsrPortID_x_to_HWPort_y[PortCnt] == 0",
                      EDDI_FATAL_ERR_EXCP, PortCnt, HWPortID);
            return;
        }

        if (HWPortID == EDDI_PORT_NOT_CONNECTED)
        {
            break;
        }

        if (HWPortID > EDDI_MAX_IRTE_PORT_CNT)
        {
            EDDI_Excp("EDDI_PmCheck - pDPB->PortMap.UsrPortID_x_to_HWPort_y[PortCnt] > EDDI_MAX_IRTE_PORT_CNT",
                      EDDI_FATAL_ERR_EXCP, PortCnt, HWPortID);
            return;
        }

        for (j = 0; j < EDD_CFG_MAX_PORT_CNT; j++)
        {
            if (HWPortID == pDPB->PortMap.UsrPortID_x_to_HWPort_y[j])
            {
                NumberOfHits++;
            }
        }

        if (NumberOfHits > 1)
        {
            EDDI_Excp("EDDI_PmCheck - Double HWPortID", EDDI_FATAL_ERR_EXCP, 0, HWPortID);
            return;

        }

        switch (HWPortID)
        {
            case 1:
            {
                pDDB->PM.PortMask_01_08[PortCnt] = 0x01;
                pDDB->PM.PortMask_02_10[PortCnt] = 0x02;
                pDDB->PM.PortMask_04_20[PortCnt] = 0x04;
                break;
            }
            case 2:
            {
                pDDB->PM.PortMask_01_08[PortCnt] = 0x02;
                pDDB->PM.PortMask_02_10[PortCnt] = 0x04;
                pDDB->PM.PortMask_04_20[PortCnt] = 0x08;
                break;
            }
            case 3:
            {
                pDDB->PM.PortMask_01_08[PortCnt] = 0x04;
                pDDB->PM.PortMask_02_10[PortCnt] = 0x08;
                pDDB->PM.PortMask_04_20[PortCnt] = 0x10;
                break;
            }
            case 4:
            {
                pDDB->PM.PortMask_01_08[PortCnt] = 0x08;
                pDDB->PM.PortMask_02_10[PortCnt] = 0x10;
                pDDB->PM.PortMask_04_20[PortCnt] = 0x20;
                break;
            }
            default:
            {
                EDDI_Excp("EDDI_PmCheck - HWPortID", EDDI_FATAL_ERR_EXCP, 0, HWPortID);
                return;
            }
        }

        pDDB->PM.UsrPortID_to_HWPort_0_3[PortCnt + 1] = HWPortID - 1;
    }

    for (i = 0; i < EDD_CFG_MAX_PORT_CNT; i++)
    {
        LSA_UINT32  const  UsrPortID   = i + 1;
        LSA_UINT32  const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortID];

        if (HwPortIndex >= EDDI_MAX_IRTE_PORT_CNT)
        {
            continue;
        }
        pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex] = UsrPortID;
    }

    for (i = PortCnt; i < EDD_CFG_MAX_PORT_CNT; i++)
    {
        if (pDPB->PortMap.UsrPortID_x_to_HWPort_y[i] != EDDI_PORT_NOT_CONNECTED)
        {
            EDDI_Excp("EDDI_PmCheck - Gap !!", EDDI_FATAL_ERR_EXCP, i, 0);
            return;
        }
    }

    pDPB->PortMap.PortCnt = PortCnt;

    pDDB->PM.PortMap = pDPB->PortMap;

    if (PortCnt == 0)
    {
        EDDI_Excp("EDDI_PmCheck - PortCnt == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    #if defined (EDDI_CFG_ERTEC_400)
    pDDB->PM.HwTypeMaxPortCnt = 4;
    #elif defined (EDDI_CFG_ERTEC_200)
    pDDB->PM.HwTypeMaxPortCnt = 2;
    #elif defined (EDDI_CFG_SOC)
    switch (pDDB->ERTEC_Version.Location)
    {
        case EDDI_LOC_SOC1:
        {
            pDDB->PM.HwTypeMaxPortCnt = 3;
            break;
        }
        case EDDI_LOC_SOC2:
        {
            pDDB->PM.HwTypeMaxPortCnt = 2;
            break;
        }
        case EDDI_LOC_FPGA_XC2_V8000:
        {
            pDDB->PM.HwTypeMaxPortCnt = 1;
            break;
        }
        default:
        {
            EDDI_Excp("EDDI_PmCheck - wrong location", EDDI_FATAL_ERR_EXCP, PortCnt, pDDB->PM.HwTypeMaxPortCnt);
            return;
        }
    }
    #else
    #error EDDI_CFG_ERROR: Hardware not defined!
    #endif

    if (PortCnt > pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_PmCheck - PortCnt > pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP, PortCnt, pDDB->PM.HwTypeMaxPortCnt);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PmGetHwPortIndex()                          */
/*                                                                         */
/* D e s c r i p t i o n: UserPortIndex-range is: 0..3!                    */
/*                        HwPortIndex-range is:   0..3!                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PmGetHwPortIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  UsrPortIndex )
{
    LSA_UINT32  HwPortIndex;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmGetHwPortIndex->");

    if (UsrPortIndex >= pDDB->PM.PortMap.PortCnt)
    {
        EDDI_Excp("EDDI_PmGetHwPortIndex, UsrPortIndex >= pDDB->PM.PortMap.PortCnt", EDDI_FATAL_ERR_EXCP, UsrPortIndex, pDDB->PM.PortMap.PortCnt);
        return 0;
    }

    HwPortIndex = pDDB->PM.PortMap.UsrPortID_x_to_HWPort_y[UsrPortIndex] - 1;

    if (HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_PmGetHwPortIndex, HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP, HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return 0;
    }

    return HwPortIndex;
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
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PmIsValidHwPortIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex )
{
    LSA_UINT32  UsrPortIndex;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmIsValidHwPortIndex->");

    if (HwPortIndex >= EDDI_MAX_IRTE_PORT_CNT)
    {
        EDDI_Excp("EDDI_PmIsValidHwPortIndex, HwPortIndex >= EDDI_MAX_IRTE_PORT_CNT", EDDI_FATAL_ERR_EXCP, HwPortIndex, pDDB->PM.PortMap.PortCnt);
        return LSA_FALSE;
    }

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        if ((pDDB->PM.PortMap.UsrPortID_x_to_HWPort_y[UsrPortIndex] - 1) == HwPortIndex)
        {
            return LSA_TRUE;
        }
    }

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n: UserPortID-interval is: 1..4 !                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PmUsrPortIDToHwPort03( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  UsrPortId )
{
    LSA_UINT32  HwPort0_3;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmUsrPortIDToHwPort03->");

    if (UsrPortId == 0)
    {
        EDDI_Excp("EDDI_PmUsrPortIDToHwPort03, UsrPortId == 0",
                  EDDI_FATAL_ERR_EXCP, UsrPortId, pDDB->PM.PortMap.PortCnt);
        return LSA_FALSE;
    }

    if (UsrPortId > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_Excp("EDDI_PmUsrPortIDToHwPort03, UsrPortId > pDDB->PM.PortMap.PortCnt", EDDI_FATAL_ERR_EXCP, UsrPortId, pDDB->PM.PortMap.PortCnt);
        return LSA_FALSE;
    }

    HwPort0_3 = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];

    return HwPort0_3;
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PmGetMask0420( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  UsrPortIndex )
{
    LSA_UINT32  mask;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmGetMask0420->");

    if (UsrPortIndex >= pDDB->PM.PortMap.PortCnt)
    {
        EDDI_Excp("EDDI_PmGetMask0420, UsrPortIndex >= pDDB->PM.PortMap.PortCnt", EDDI_FATAL_ERR_EXCP, UsrPortIndex, pDDB->PM.PortMap.PortCnt);
        return 0;
    }

    mask = pDDB->PM.PortMask_04_20[UsrPortIndex];

    return mask;
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PmGetMask0108( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  UsrPortIndex )
{
    LSA_UINT32  mask;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmGetMask0108->");

    if (UsrPortIndex >= pDDB->PM.PortMap.PortCnt)
    {
        EDDI_Excp("EDDI_PmGetMask0108, UsrPortIndex >= pDDB->PM.PortMap.PortCnt", EDDI_FATAL_ERR_EXCP, UsrPortIndex, pDDB->PM.PortMap.PortCnt);
        return 0;
    }

    mask = pDDB->PM.PortMask_01_08[UsrPortIndex];

    return mask;
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PmGetMask0210( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  UsrPortIndex )
{
    LSA_UINT32  mask;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PmGetMask0210->");

    if (UsrPortIndex >= pDDB->PM.PortMap.PortCnt)
    {
        EDDI_Excp("EDDI_PmGetMask0210, UsrPortIndex >= pDDB->PM.PortMap.PortCnt", EDDI_FATAL_ERR_EXCP, UsrPortIndex, pDDB->PM.PortMap.PortCnt);
        return 0;
    }

    mask = pDDB->PM.PortMask_02_10[UsrPortIndex];

    return mask;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_pm.c                                                    */
/*****************************************************************************/

