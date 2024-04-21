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
/*  F i l e               &F: eddi_kram.c                               :F&  */
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
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_kram.h"

#define EDDI_MODULE_ID     M_ID_EDDI_KRAM
#define LTRC_ACT_MODUL_ID  23

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_KramCheckFreeBuffer()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_KramCheckFreeBuffer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_MEM_INFO_T  info;
    LSA_UINT32       ret;

    info.MemHandle = pDDB->KRamMemHandle;
    ret = EDDI_MEMInfo(&info);
    if (ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_KramCheckFreeBuffer", EDDI_FATAL_ERR_EXCP, ret, 0);
        return;
    }

    pDDB->free_KRAM_buffer = info.free_mem;
}
/*---------------------- end [subroutine] ---------------------------------*/


//////////////////////////////////////////////////////////////////////////
/// Copies data from DDB to mirror struct in KRAM
/// The reason therefore is the different struct sizes on different
/// EDD-platforms.
/// The Offsets for the checks can be found in @see PROF_KRAM_INFO.
//////////////////////////////////////////////////////////////////////////
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_KramSyncDpbContext( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDDI_UPPER_DPB_PTR_TYPE  const  pDPB )
{
    PROF_KRAM_INFO  *  const  pKramInfo = pDDB->pProfKRamInfo;
    LSA_UINT32                size      = 0;
    LSA_UINT32                offset    = 0;
    LSA_UINT32                head      = 0;

    /* Check 4-Byte alignment.
       Even if it is checking a constant, it should trigger problems with different
       Compiler settings.                                                           */
    size = sizeof(PROF_KRAM_INFO);

    if ((PROF_KRAM_SIZE + PROF_KRAM_SIZE_SERCMD) != size) //temporarily_disabled_lint !e774
    {
        /* Offset Test */
        head = (LSA_UINT32)pKramInfo->mode;

        offset = (LSA_UINT32) &(pKramInfo->DPB) - head;
        if (offset != PROF_KRAM_OFFSET_PROF_EDDI_DPB_INFO)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error PROF_EDDI_DPB_INFO!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }

        offset = (LSA_UINT32) &(pKramInfo->Excp) - head;
        if (offset != PROF_KRAM_OFFSET_PROF_EDDI_EXCP_INFO)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error PROF_EDDI_EXCP_INFO!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }

        offset = (LSA_UINT32) &(pKramInfo->info) - head;
        if (offset != PROF_KRAM_OFFSET_PROF_EDDI_PARA_INFO)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error PROF_EDDI_PARA_INFO!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }

        offset = (LSA_UINT32) &(pKramInfo->intStats) - head;
        if (offset != PROF_KRAM_OFFSET_PROF_EDDI_INT_STATS_INFO)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error PROF_EDDI_INT_STATS_INFO!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }

        offset = (LSA_UINT32) &(pKramInfo->eddi_lsa_vers) - head;
        if (offset != PROF_KRAM_OFFSET_PROF_EDDI_LSA_VERS)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error PROF_EDDI_LSA_VERS!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }

        offset = (LSA_UINT32) &(pKramInfo->Swi_AgePollTime) - head;
        if (offset != PROF_KRAM_OFFSET_SWI_AGEPOLL_TIME)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error Swi_AgePollTime!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }

        offset = (LSA_UINT32) &(pKramInfo->Ticks_till_PhyReset) - head;
        if (offset != PROF_KRAM_OFFSET_TICKS_TILL_PHYRESET)
        {
            EDDI_Excp("EDDI_KramSyncDpbContext, Offset-Error Ticks_till_PhyReset!", EDDI_FATAL_ERR_EXCP, offset, 0);
            return;
        }
    }

    pKramInfo->DPB.EnableReset                  = (LSA_UINT32)pDPB->EnableReset;
    pKramInfo->DPB.BOARD_SDRAM_BaseAddr         = pDPB->BOARD_SDRAM_BaseAddr;
    pKramInfo->DPB.ERTECx00_SCRB_BaseAddr       = pDPB->ERTECx00_SCRB_BaseAddr;
    pKramInfo->DPB.InterfaceID                  = pDPB->InterfaceID;
    pKramInfo->DPB.IRTE_SWI_BaseAdr             = pDPB->IRTE_SWI_BaseAdr;
    pKramInfo->DPB.IRTE_SWI_BaseAdr_LBU_16Bit   = pDPB->IRTE_SWI_BaseAdr_LBU_16Bit;
    pKramInfo->DPB.PAEA_BaseAdr                 = pDPB->PAEA_BaseAdr;
    pKramInfo->Ticks_till_PhyReset              = pDDB->pProfKRamInfo->Ticks_till_PhyReset;

    pKramInfo->DPB.KRam_offset_ProcessImageEnd  = pDPB->KRam.offset_ProcessImageEnd;
    pKramInfo->DPB.KRam_size_reserved_for_eddi  = pDPB->KRam.size_reserved_for_eddi;
}
/*---------------------- end [subroutine] ---------------------------------*/


/****************************************************************************/
/*  end of file eddi_kram.c                                                 */
/****************************************************************************/

