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
/*  F i l e               &F: eddi_irt_tree.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IRT (isochron real time) for EDDI.               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_irt_tree.h"
#include "eddi_ser_ext.h"

#if defined (EDDI_CFG_REV5)
#include "eddi_sync_ini.h"
#endif

#define EDDI_MODULE_ID     M_ID_IRT_TREE
#define LTRC_ACT_MODUL_ID  119

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_IRTTreeInitAx()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTTreeInitAx( EDDI_DDB_TYPE  *  const  pDDB,
                                                   LSA_UINT16        const  CycleBaseFactor )
{
    EDDI_IRT_REDUCTION_AX_TYPE  *  const  pIrtRed      = &pDDB->CRT.IRTRedAx;
    LSA_UINT32                     const  UsrPortCount = pDDB->PM.PortMap.PortCnt;
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                            ret;
    LSA_BOOL                              bNoopUsed[EDDI_MAX_IRTE_PORT_CNT * 2];
    EDDI_LOCAL_NOOP_PTR_TYPE              pNoopTable[EDDI_MAX_IRTE_PORT_CNT * 2];

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTTreeInitAx->");

    for (UsrPortIndex = 0; UsrPortIndex<EDDI_MAX_IRTE_PORT_CNT * 2; UsrPortIndex++)
    {
        bNoopUsed[UsrPortIndex] = LSA_FALSE;
        pNoopTable[UsrPortIndex] = (EDDI_LOCAL_NOOP_PTR_TYPE)0;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // EOL is always needed for IRT-SendeQueue
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //Allocate Buffer in KRAM for EOL-Entries one for each IRT-TX-Tree
    //AND      Buffer in KRAM for Dummy-EOL-Entries one for each IRT-RX-Tree
    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pIrtRed->pEOLMemHeader, pIrtRed->KramSizeEOL);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeInitAx, Not enough free KRAM!", EDDI_FATAL_ERR_EXCP, ret, pIrtRed->KramSizeEOL);
        return;
    }

    pIrtRed->pEOL = (EDDI_LOCAL_EOL_PTR_TYPE)(void *)pIrtRed->pEOLMemHeader->pKRam;

    pDDB->KramRes.pool.irt_eol = pIrtRed->KramSizeEOL;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // NOOP is needed for Root of each IRT-Tree
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //Allocate Buffer in KRAM for NOOP-Entries one for each IRT-TX and RX-Tree
    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pIrtRed->pNOOPMemHeader, pIrtRed->KramSizeNOOP);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeInitAx, Not enough free KRAM!", EDDI_FATAL_ERR_EXCP, ret, pIrtRed->KramSizeNOOP);
        return;
    }

    pIrtRed->pNOOPBase = (EDDI_LOCAL_NOOP_PTR_TYPE)(void *)pIrtRed->pNOOPMemHeader->pKRam;

    pDDB->KramRes.pool.irt_noop = pIrtRed->KramSizeNOOP;

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // IrtPortStartTimeTxBlock is needed for shifting TX-Start of each IRT-Tree
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //Allocate Buffer in KRAM for each IRT-TX-Tree
    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pIrtRed->pIrtPortStartTimeTxBlockMemHeader, pIrtRed->KramSizeIrtPortStartTimeTxBlock);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeInitAx, Not enough free KRAM!", EDDI_FATAL_ERR_EXCP, ret, pIrtRed->KramSizeIrtPortStartTimeTxBlock);
        return;
    }

    pIrtRed->pIrtPortStartTimeTxBlock = (EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE)(void *)pIrtRed->pIrtPortStartTimeTxBlockMemHeader->pKRam;

    pDDB->KramRes.pool.irt_port_start_time_tx_block = pIrtRed->KramSizeIrtPortStartTimeTxBlock;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // IrtPortStartTimeRxBlock is needed for shifting RX-Start of each IRT-Tree
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //Allocate Buffer in KRAM for each IRT-RX-Tree
    ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pIrtRed->pIrtPortStartTimeRxBlockMemHeader, pIrtRed->KramSizeIrtPortStartTimeRxBlock);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeInitAx, Not enough free KRAM!", EDDI_FATAL_ERR_EXCP, ret, pIrtRed->KramSizeIrtPortStartTimeRxBlock);
        return;
    }

    pIrtRed->pIrtPortStartTimeRxBlock = (EDDI_SER_IRT_PORT_START_TIME_RX_BLOCK_PTR_TYPE)(void *)pIrtRed->pIrtPortStartTimeRxBlockMemHeader->pKRam;

    pDDB->KramRes.pool.irt_port_start_time_rx_block = pIrtRed->KramSizeIrtPortStartTimeRxBlock;
    #else
    pDDB->KramRes.pool.irt_port_start_time_tx_block = 0;
    pDDB->KramRes.pool.irt_port_start_time_rx_block = 0;
    #endif

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Init IRT-Trees
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    for (UsrPortIndex = 0; UsrPortIndex < (UsrPortCount * 2); UsrPortIndex++)
    {
        bNoopUsed[UsrPortIndex]  = LSA_FALSE;
        pNoopTable[UsrPortIndex] = (EDDI_LOCAL_NOOP_PTR_TYPE)0;
    }

    //allocate NOOP-space for RX first
    for (UsrPortIndex = 0; UsrPortIndex < UsrPortCount; UsrPortIndex++)
    {
        LSA_UINT8  NoopIdx;

        for (NoopIdx=0; NoopIdx < (UsrPortCount * 2); NoopIdx++)
        {
            //search free NOOP-entry
            if (!bNoopUsed[NoopIdx]) //temporarily_disabled_lint !e771
            {
                pNoopTable[UsrPortCount + UsrPortIndex] = &pIrtRed->pNOOPBase[NoopIdx];

                //due to an asic-bug (AP00694034), b9..4 are not allowed to be all 1
                if  //NOOP has no illegal address
                (0x3F0 != ((LSA_UINT32)pNoopTable[UsrPortCount + UsrPortIndex] & 0x3F0UL))
                {
                    bNoopUsed[NoopIdx] = LSA_TRUE;
                    break;  //found!
                }
            }
        }

        if (NoopIdx >= (UsrPortCount * 2))
        {
            EDDI_Excp("EDDI_IRTTreeInitAx, Too many NOOP on illegal addresses!", EDDI_FATAL_ERR_EXCP, ret, 0);
            return;
        }
    }

    //allocate NOOP-space for TX
    for (UsrPortIndex = 0; UsrPortIndex < UsrPortCount; UsrPortIndex++)
    {
        LSA_UINT8  NoopIdx;

        for (NoopIdx=0; NoopIdx < (UsrPortCount * 2); NoopIdx++)
        {
            //search free NOOP-entry
            if (!bNoopUsed[NoopIdx])
            {
                pNoopTable[UsrPortIndex] = &pIrtRed->pNOOPBase[NoopIdx];
                bNoopUsed[NoopIdx]       = LSA_TRUE;
                break;  //found!
            }
        }

        if (NoopIdx >= (UsrPortCount * 2))
        {
            EDDI_Excp("EDDI_IRTTreeInitAx, Too many NOOP on illegal addresses!", EDDI_FATAL_ERR_EXCP, ret, 0);
            return;
        }
    }

    for (UsrPortIndex = 0; UsrPortIndex < UsrPortCount; UsrPortIndex++)
    {
        EDDI_LOCAL_NOOP_PTR_TYPE         pNOOP;
        LSA_UINT32                       EOLCount;
        EDDI_LOCAL_EOL_PTR_TYPE          pEOLn[EDDI_FCW_TX_EOL_CNT];
        LSA_UINT32                       EOLMemIndex = UsrPortIndex;
        LSA_UINT32                const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        EDDI_TREE_TYPE                *  pTree;

        //PREPARE TX-TREE WITH NOOP AND EOL
        pNOOP = pNoopTable[UsrPortIndex];
        EDDI_SERIniNOOP(pDDB, pNOOP);

        for (EOLCount = 0; EOLCount < EDDI_FCW_TX_EOL_CNT; EOLCount++)
        {
            pEOLn[EOLCount] = &pIrtRed->pEOL[EOLMemIndex];
            EOLMemIndex += UsrPortCount;
            EDDI_SERIniEOL(pDDB, pEOLn[EOLCount]);
        }

        pTree = &pIrtRed->IrtRedTreeTx[HwPortIndex];

        EDDI_RedTreeBuild(pDDB,
                          pTree,
                          CycleBaseFactor,
                          (EDDI_SER_CCW_PTR_TYPE)(void *)pNOOP,
                          &pEOLn[0]);

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        {
            EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE  const  pIrtPortStartTimeTxBlock = &pIrtRed->pIrtPortStartTimeTxBlock[UsrPortIndex];

            EDDI_SERIniIrtPortStartTimeTxBlock(pDDB,
                                               pTree,
                                               HwPortIndex,
                                               pIrtPortStartTimeTxBlock);
        }
        #endif

        //PREPARE RX-TREE WITH NOOP AND DUMMY-EOL
        pNOOP    = pNoopTable[UsrPortCount + UsrPortIndex];

        pEOLn[0] = &pIrtRed->pEOL[EOLMemIndex]; //Dummy-EOL in Receive-Trees

        EDDI_SERIniNOOP(pDDB,pNOOP);
        EDDI_SERIniNOOP(pDDB,(EDDI_LOCAL_NOOP_PTR_TYPE)(void *)pEOLn[0]); //Initialize Dummy-EOL as NOOP!

        pTree = &pIrtRed->IrtRedTreeRx[HwPortIndex];

        EDDI_RedTreeBuild(pDDB,
                          pTree,
                          CycleBaseFactor,
                          (EDDI_SER_CCW_PTR_TYPE)(void *)pNOOP,
                          &pEOLn[0]);

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        {
            EDDI_SER_IRT_PORT_START_TIME_RX_BLOCK_PTR_TYPE  const  pIrtPortStartTimeRxBlock = &pIrtRed->pIrtPortStartTimeRxBlock[UsrPortIndex];

            EDDI_SERIniIrtPortStartTimeRxBlock(pDDB,
                                               pTree,
                                               HwPortIndex,
                                               pIrtPortStartTimeRxBlock);
        }
        #endif
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IRTTreeCloseAx()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTTreeCloseAx( EDDI_DDB_TYPE  *  const  pDDB )
{
    EDDI_IRT_REDUCTION_AX_TYPE  *  const  pIrtRed = &pDDB->CRT.IRTRedAx;
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                            ret;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTTreeCloseAx->");

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        EDDI_RedTreeClose(pDDB, &pIrtRed->IrtRedTreeRx[HwPortIndex]);
        EDDI_RedTreeClose(pDDB, &pIrtRed->IrtRedTreeTx[HwPortIndex]);
    }

    ret = EDDI_MEMFreeBuffer(pIrtRed->pEOLMemHeader);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeCloseAx, pIrtRed->pEOLMemHeader", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }

    ret = EDDI_MEMFreeBuffer(pIrtRed->pNOOPMemHeader);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeCloseAx, pIrtRed->pNOOPMemHeader", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    ret = EDDI_MEMFreeBuffer(pIrtRed->pIrtPortStartTimeTxBlockMemHeader);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeCloseAx, pIrtRed->pIrtPortStartTimeTxBlockMemHeader", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }

    ret = EDDI_MEMFreeBuffer(pIrtRed->pIrtPortStartTimeRxBlockMemHeader);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_IRTTreeCloseAx, pIrtRed->pIrtPortStartTimeRxBlockMemHeader", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IRTTreeSendClockChange()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTTreeSendClockChange( EDDI_DDB_TYPE  *  const  pDDB,
                                                            LSA_UINT16        const  CycleBaseFactor,
                                                            LSA_UINT16        const  OldCycleBaseFactor )
{
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                     const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    EDDI_IRT_REDUCTION_AX_TYPE  *  const  pIrtRed    = &pDDB->CRT.IRTRedAx;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTTreeSendClockChange->");

    if (OldCycleBaseFactor == CycleBaseFactor)
    {
        return;
    }

    #if defined (EDDI_CFG_REV5)
    {
        LSA_UINT8 const CBFIsBin    = EDDI_RedIsBinaryValue(CycleBaseFactor)?1:0;
        LSA_UINT8 const OldCBFIsBin = EDDI_RedIsBinaryValue(OldCycleBaseFactor)?1:0;
        //CycleBaseFactor changed from 2^n-value to a non-2^n-value or vice versa
        //--> Trees must totally be rebuild
        if (CBFIsBin != OldCBFIsBin)
        {
            EDDI_IRTTreeCloseAx(pDDB);

            EDDI_SYNCInitIRT(pDDB);

            EDDI_IRTTreeInitAx(pDDB, CycleBaseFactor);

            return;
        }
    }
    #endif

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32      const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        EDDI_TREE_TYPE      *  pTree;

        pTree = &pIrtRed->IrtRedTreeRx[HwPortIndex];

        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        EDDI_RedTreeUpdateRCWHeads(pDDB, pTree, CycleBaseFactor);
        #else
        EDDI_RedTreeUpdateBCWMods(pDDB, pTree, 1, 1, CycleBaseFactor);
        #endif

        pTree = &pIrtRed->IrtRedTreeTx[HwPortIndex];

        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        EDDI_RedTreeUpdateRCWHeads(pDDB, pTree, CycleBaseFactor);
        #else
        EDDI_RedTreeUpdateBCWMods(pDDB, pTree, 1, 1, CycleBaseFactor);
        #endif
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_irt_tree.c                                              */
/*****************************************************************************/
