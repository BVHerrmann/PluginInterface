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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_psi.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*  base adaption for PSI                                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 3603
#define PND_MODULE_ID   3603

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_pnstack.h"
#include "psi_int.h"
#include "psi_hd.h"
#include "pnd_trc.h"



/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

PNIO_UINT32 pnd_psi_get_path_info(LSA_SYS_PTR_TYPE * ppSys, PNIO_VOID_PTR_TYPE * ppDetail, PNIO_UINT16 hdNr, PNIO_UINT16 ifNr)
{
    PSI_SYS_PTR_TYPE pSys;
    PNIO_UINT16 rspMbxId;
    PNIO_UINT16 compIdLower;
    PSI_HD_STORE_PTR_TYPE pPsiHdStore;
    PSI_DETAIL_STORE_PTR_TYPE pDetailsIF;
    PND_SYSTEM_DEVICE_INFO_PTR_TYPE pDetail;
    PNIO_UINT32 result = PNIO_OK;
    
    PND_ASSERT(ppSys != PNIO_NULL);
    PND_ASSERT(ppDetail != PNIO_NULL);

    pDetailsIF = psi_get_detail_store(hdNr);
    pPsiHdStore = psi_hd_get_hd_store(hdNr);

    PND_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, 
        "pnd_psi_get_path_info: pnio_if_nr(%u) hd_nr(%u) if_nr(%u)",
        pDetailsIF->pnio_if_nr, 
        hdNr,
        ifNr);

    if((pPsiHdStore->hd_hw.edd_type == LSA_COMP_ID_EDDI) || (pPsiHdStore->hd_hw.edd.eddi.device_type == EDD_HW_TYPE_USED_SOC))
    {
        pDetail = static_cast<PND_SYSTEM_DEVICE_INFO_PTR_TYPE>(pnd_mem_alloc(sizeof(PND_SYSTEM_DEVICE_INFO_TYPE)));
        PND_ASSERT(pDetail);

        pSys = (PSI_SYS_PTR_TYPE)pnd_mem_alloc(sizeof(PSI_SYS_TYPE));
        PND_ASSERT(pSys);

        pDetail->if_nr = ifNr;
        pDetail->hd_nr = hdNr;
        pDetail->hw.soc.iocc_virtual_addr = (PNIO_UINT32)pPsiHdStore->hd_hw.edd.eddi.iocc.base_ptr;
        pDetail->hw.soc.iocc_ahb_addr = (PNIO_UINT32)pPsiHdStore->hd_hw.edd.eddi.iocc.phy_addr;
        pDetail->hw.soc.kram_ahb_addr = (PNIO_UINT32)pPsiHdStore->hd_hw.edd.eddi.kram.phy_addr;
        pDetail->hw.soc.iocc_enable = PNIO_TRUE;
        pDetail->hw.soc.sdram.base_ptr = pPsiHdStore->hd_hw.edd.eddi.sdram.base_ptr;
        pDetail->hw.soc.sdram.phy_addr = pPsiHdStore->hd_hw.edd.eddi.sdram.phy_addr;
        pDetail->hw.soc.sdram.size = pPsiHdStore->hd_hw.edd.eddi.sdram.size;
        pDetail->hw.soc.shared_mem.base_ptr = pPsiHdStore->hd_hw.edd.eddi.shared_mem.base_ptr;
        pDetail->hw.soc.shared_mem.phy_addr = pPsiHdStore->hd_hw.edd.eddi.shared_mem.phy_addr;
        pDetail->hw.soc.shared_mem.size = pPsiHdStore->hd_hw.edd.eddi.shared_mem.size;

        #if (PSI_CFG_USE_HIF_HD == 1)
        rspMbxId = PSI_MBX_ID_HIF_HD;
        #else
        rspMbxId = PSI_MBX_ID_USER;
        #endif
        compIdLower = LSA_COMP_ID_PND;


        // now we setup the Sysptr based on information stored for this PNIO IF
        pSys->hd_nr = hdNr;
        pSys->pnio_if_nr = pDetailsIF->pnio_if_nr;
        pSys->comp_id = LSA_COMP_ID_PND;
        pSys->comp_id_lower = compIdLower;
        pSys->comp_id_edd = pDetailsIF->edd_comp_id;
        pSys->mbx_id_rsp = rspMbxId;           // MBX for responses
        pSys->hPoolNrtRx = pDetailsIF->hPoolNrtRx; // NRT RX pool handle for NRT mem
        pSys->hPoolNrtTx = pDetailsIF->hPoolNrtTx; // NRT RX pool handle for NRT mem
        pSys->check_arp = PNIO_FALSE;
        pSys->hd_runs_on_level_ld = pDetailsIF->hd_runs_on_level_ld;
        pSys->trace_idx = pDetailsIF->trace_idx;

        *ppDetail = pDetail;
        *ppSys = pSys;
    }
    else
    {
        PND_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,
            "pnd_psi_get_path_info: invalid edd and device type, edd_type(%#x) device_type(%#x)",
            pPsiHdStore->hd_hw.edd_type,
            pPsiHdStore->hd_hw.edd.eddi.device_type);

        *ppDetail = PNIO_NULL;
        *ppSys = PNIO_NULL;

        result = PNIO_ERR_PRM_INVALIDARG;
    }


    return result;
}

PNIO_UINT32 pnd_psi_release_path_info(LSA_SYS_PTR_TYPE pSys, PNIO_VOID_PTR_TYPE pDetail)
{
    PND_ASSERT(pSys);
    PND_ASSERT(pDetail);

    pnd_mem_free(pSys);
    pnd_mem_free(pDetail);

    return PNIO_OK;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
