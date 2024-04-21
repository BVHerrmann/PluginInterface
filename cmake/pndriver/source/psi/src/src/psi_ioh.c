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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_ioh.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for IOH                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   110
#define PSI_MODULE_ID       110 //PSI_MODULE_ID_PSI_IOH

#include "psi_int.h"

#if ((PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== IOH====================================================================*/
/*===========================================================================*/

LSA_RESULT psi_ioh_get_path_info(
    LSA_SYS_PTR_TYPE  * const sys_ptr_ptr,
	LSA_VOID_PTR_TYPE   const detail_ptr,
    LSA_SYS_PATH_TYPE   const sys_path)
{
    IOH_DETAIL_PTR_TYPE             const pDetail       = (IOH_DETAIL_PTR_TYPE)detail_ptr;
    IOH_SYSTEM_DEVICE_INFO_PTR_TYPE       pDevice_info  = LSA_NULL;
    PSI_SYS_PTR_TYPE                      pSys;
    PSI_SYS_PTR_TYPE                      pSysHD;
    PSI_HD_PARAM_TYPE                     hd_param;
    LSA_USER_ID_TYPE                      user_id;
    LSA_UINT16                            mbx_id_rsp    = PSI_MBX_ID_MAX;
    LSA_UINT16                            comp_id_lower = LSA_COMP_ID_UNUSED;
    LSA_UINT16                            result        = LSA_RET_OK;
    LSA_UINT16                      const hd_nr_ioh     = PSI_SYSPATH_GET_HD(sys_path);
    LSA_UINT16                            hd_nr;
    LSA_UINT16                      const path          = PSI_SYSPATH_GET_PATH(sys_path);
    PSI_DETAIL_STORE_PTR_TYPE       const pDetailsIOH   = psi_get_detail_store(hd_nr_ioh);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
    PSI_ASSERT(detail_ptr != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "psi_ioh_get_path_info(sys_path=%#x): path(%#x/%u)", 
        sys_path, path, path);

    PSI_ALLOC_LOCAL_MEM(((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
    PSI_ASSERT(pSys != LSA_NULL);

    switch (path)
    {
        case PSI_PATH_GLO_APP_IOH: //physical device
        {
            //get all information for all HDs
            mbx_id_rsp    = PSI_MBX_ID_USER; //IOH user
            comp_id_lower = LSA_COMP_ID_UNUSED;

            for (hd_nr = 1; hd_nr <= pDetail->hd_count; hd_nr++)
            {
                PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

                if (pDetailsIF->is_created == LSA_FALSE)
                {
                    //no hd created, but expected --> FATAL;
                    PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_ioh_get_path_info(): hd (%u) is not created, but used", hd_nr);
                    PSI_FATAL(0);
                }
                else
                {
                    //allocate struct for this HD
                    PSI_ALLOC_LOCAL_MEM(((LSA_VOID_PTR_TYPE*)&pDetail->hd[hd_nr].device_info), user_id, sizeof(*pDetail->hd[hd_nr].device_info), 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
                    PSI_ASSERT(pDetail->hd[hd_nr].device_info != LSA_NULL);
                    PSI_MEMSET(pDetail->hd[hd_nr].device_info, 0, sizeof(*pDetail->hd[hd_nr].device_info));

                    //set device_info to current device
                    pDevice_info = pDetail->hd[hd_nr].device_info;
                }

                PSI_ASSERT(pDevice_info != LSA_NULL);

                PSI_ALLOC_LOCAL_MEM(((LSA_VOID_PTR_TYPE*)&pSysHD), user_id, sizeof(*pSysHD), 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
                PSI_ASSERT(pSysHD != LSA_NULL);

                //get the HD parameters for this HD
                PSI_MEMSET(&hd_param, 0, sizeof(hd_param));
                PSI_LD_GET_HD_PARAM(&result, hd_nr, &hd_param);
                PSI_ASSERT(result == PSI_OK);

                switch (hd_param.edd_type)
                {
#if ((IOH_CFG_USE_PERIF == 1) || (IOH_CFG_USE_HERA == 1))
                    case LSA_COMP_ID_EDDP:
                    {
                        switch (hd_param.edd.eddp.hw_type)
                        {
#if (IOH_CFG_USE_PERIF == 1)
                            case EDDP_HW_ERTEC200P:
                            case EDDP_HW_ERTEC200P_FPGA:
                            {
                                pDevice_info->device_type                       = IOH_DEVICE_TYPE_PNIP;
                                pDevice_info->hw.ertec200p.perif_enable         = LSA_TRUE;
                                pDevice_info->hw.ertec200p.perif_hw_base_ptr    = (LSA_UINT32)hd_param.edd.eddp.apb_periph_perif.base_ptr;
                                pDevice_info->hw.ertec200p.perif_ram_base_ptr   = (LSA_UINT32)hd_param.edd.eddp.perif_ram.base_ptr;
                                pDevice_info->hw.ertec200p.pnip_virtual_addr    = (LSA_UINT32)hd_param.edd.eddp.pnip.base_ptr;
                                pDevice_info->g_shared_ram_addr                 = (LSA_UINT32)pDetailsIF->shared_ram_base_ptr;
                            }
                            break;
#endif // IOH_CFG_USE_PERIF == 1

#if (IOH_CFG_USE_HERA == 1)
                            case EDDP_HW_HERA:
                            case EDDP_HW_HERA_FPGA:
                            {
                                pDevice_info->device_type               = IOH_DEVICE_TYPE_HERA;
                                pDevice_info->hw.hera.bhera_enable      = LSA_TRUE;
                                pDevice_info->hw.hera.uBaseAdrFastMem   = (LSA_UINT32)0; //FastMem is not going to be use in 1st step
                                //lint --e(506) Constant value Boolean - the "HERA_IO_CFG_DEFAULT_CONSUMERS_PROVIDERS" can be changed by system adaption
                                pDevice_info->hw.hera.uNrMaxProvCons    = HERA_IO_CFG_DEFAULT_CONSUMERS_PROVIDERS;
                                pDevice_info->hw.hera.pnip_virtual_addr = (LSA_UINT32)hd_param.edd.eddp.pnip.base_ptr;
                                pDevice_info->crt_slow_mem.base_ptr     = hd_param.edd.eddp.sdram_CRT.base_ptr;
                                pDevice_info->crt_slow_mem.size         = hd_param.edd.eddp.sdram_CRT.size;
                                pDevice_info->crt_slow_mem.phy_addr     = hd_param.edd.eddp.sdram_CRT.phy_addr;
                                pDevice_info->g_shared_ram_addr         = (LSA_UINT32)pDetailsIF->shared_ram_base_ptr;
                            }
                            break;
#endif // IOH_CFG_USE_HERA == 1

                            default:
                            {
                                PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_ioh_get_path_info(): invalid eddp.hw_type(%u) in HD store",
                                    hd_param.edd.eddp.hw_type);
                                PSI_FATAL(0);
                            }
                        }
                    }
                    break;
#endif // (IOH_CFG_USE_PERIF == 1) || (IOH_CFG_USE_HERA == 1)

#if (IOH_CFG_USE_IRTE == 1)
                    case LSA_COMP_ID_EDDI:
                    {
                        switch (hd_param.edd.eddi.device_type)
                        {
                            case EDD_HW_TYPE_USED_ERTEC_400:
                            {
                                pDevice_info->device_type               = IOH_DEVICE_TYPE_ERTEC400;
                                pDevice_info->hw.ertec400.kram_enable   = LSA_TRUE;
                                pDevice_info->hw.ertec400.kram.base_ptr = hd_param.edd.eddi.kram.base_ptr;
                                pDevice_info->hw.ertec400.kram.phy_addr = hd_param.edd.eddi.kram.phy_addr;
                                pDevice_info->hw.ertec400.kram.size     = hd_param.edd.eddi.kram.size;
                                pDevice_info->pi_mem.size               = IOH_GET_REAL_PI_SIZE(hd_nr);     // IN: contains the real size of usable kram
                                pDevice_info->g_shared_ram_addr         = (LSA_UINT32)pDetailsIF->shared_ram_base_ptr;
                            }
                            break;

                            case EDD_HW_TYPE_USED_ERTEC_200:
                            {
                                pDevice_info->device_type                   = IOH_DEVICE_TYPE_ERTEC200;
                                pDevice_info->hw.ertec200.kram_virtual_addr = (LSA_UINT32)hd_param.edd.eddi.kram.base_ptr;
                                pDevice_info->hw.ertec200.kram_ahb_addr     = (LSA_UINT32)hd_param.edd.eddi.kram.phy_addr;
                                pDevice_info->hw.ertec200.kram_enable       = LSA_TRUE;
                                pDevice_info->pi_mem.size                   = IOH_GET_REAL_PI_SIZE(hd_nr);     // IN: contains the real size of usable kram
                                pDevice_info->g_shared_ram_addr             = (LSA_UINT32)pDetailsIF->shared_ram_base_ptr;
                            }
                            break;

                            case EDD_HW_TYPE_USED_SOC:
                            {
                                pDevice_info->device_type                       = IOH_DEVICE_TYPE_SOC1;
                                pDevice_info->hw.soc.iocc_data.iocc_base_addr   = (LSA_UINT32)hd_param.edd.eddi.iocc.base_ptr;
                                pDevice_info->hw.soc.iocc_data.iocc_ahb_addr    = hd_param.edd.eddi.iocc.phy_addr;
                                pDevice_info->hw.soc.iocc_enable                = LSA_TRUE;
                                pDevice_info->hw.soc.kram_ahb_addr              = hd_param.edd.eddi.kram.phy_addr;
                                pDevice_info->pi_mem.base_ptr                   = hd_param.pi_mem.base_ptr;
                                pDevice_info->pi_mem.phy_addr                   = hd_param.pi_mem.phy_addr;
                                pDevice_info->pi_mem.size                       = hd_param.pi_mem.size;
                                pDevice_info->crt_slow_mem.base_ptr             = hd_param.crt_slow_mem.base_ptr;
                                pDevice_info->crt_slow_mem.phy_addr             = hd_param.crt_slow_mem.phy_addr;
                                pDevice_info->crt_slow_mem.size                 = hd_param.crt_slow_mem.size;
                                pDevice_info->g_shared_ram_addr                 = (LSA_UINT32)pDetailsIF->shared_ram_base_ptr;
                            }
                            break;

                            default:
                            {
                                PSI_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_ioh_get_path_info(): invalid eddi.device_type(%u) in HD store",
                                    hd_param.edd.eddi.device_type);
                                PSI_FATAL(0);
                            }
                        }
                    }
                    break;
#endif // IOH_CFG_USE_IRTE == 1

#if (IOH_CFG_USE_EDDS_IOBUFFER == 1)
                    case LSA_COMP_ID_EDDS:
                        pDevice_info->device_type = IOH_DEVICE_TYPE_STD;
                        break;
#endif // IOH_CFG_USE_EDDS_IOBUFFER == 1

                    default:
                    {
                        PSI_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_ioh_get_path_info(): invalid edd_type(%u/%#x) in HD store",
                            hd_param.edd_type, hd_param.edd_type);
                        PSI_FATAL(0);
                    }
                }

                pDevice_info->pnio_if_nr            = pDetailsIF->pnio_if_nr;
                pDevice_info->hd_runs_on_level_ld   = pDetailsIF->hd_runs_on_level_ld;
                pDevice_info->hd_nr                 = hd_nr;
                pDevice_info->traceidx              = pDetailsIF->trace_idx;
                pDevice_info->nr_of_ports           = pDetailsIF->nr_of_ports;
                pDevice_info->crt_slow_pool_handle  = IOH_CRT_SLOW_POOL_HANDLE_INVALID;

                // now we setup the Sysptr based on information stored for this PNIO IF (HD)
                pSysHD->hd_nr               = hd_nr;
                pSysHD->pnio_if_nr          = pDetailsIF->pnio_if_nr;
                pSysHD->comp_id             = LSA_COMP_ID_IOH;
                pSysHD->comp_id_lower       = comp_id_lower;
                pSysHD->comp_id_edd         = pDetailsIF->edd_comp_id;
                pSysHD->mbx_id_rsp          = mbx_id_rsp;               //MBX for responses
                pSysHD->hSysDev             = pDetailsIF->hSysDev;      //Sys handle for HW out functions
                pSysHD->hPoolDev            = pDetailsIF->hPoolDev;     //DEV pool handle
                pSysHD->hPoolNrtTx          = pDetailsIF->hPoolNrtTx;   //NRT pool handle for NRT TX mem
                pSysHD->hPoolNrtRx          = pDetailsIF->hPoolNrtRx;   //NRT pool handle for NRT RX mem
                pSysHD->check_arp           = LSA_FALSE;
                pSysHD->hd_runs_on_level_ld = pDetailsIF->hd_runs_on_level_ld;
                pSysHD->trace_idx           = pDetailsIF->trace_idx;
                pSysHD->psi_path            = path;

                pDevice_info->sysptr = pSysHD;
            }
        }
        break;

        default:
        {
            result = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05(0, LSA_TRACE_LEVEL_ERROR, "psi_ioh_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr_ioh, result);
        }
    }

    if (result != LSA_RET_OK)
    {
        LSA_UINT16 rc;

        for (hd_nr = 1; hd_nr <= IOH_CFG_MAX_DEVICES; hd_nr++)
        {
            if (pDetail->hd[hd_nr].device_info != LSA_NULL)
            {
                PSI_FREE_LOCAL_MEM(&rc, ((LSA_VOID_PTR_TYPE*)pDetail->hd[hd_nr].device_info), 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
                PSI_ASSERT(rc == LSA_RET_OK);
                pDetail->hd[hd_nr].device_info = LSA_NULL;
            }
        }

        PSI_FREE_LOCAL_MEM(&rc, pSys, 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
        PSI_ASSERT(rc == LSA_RET_OK);

        *sys_ptr_ptr = LSA_NULL;
    }
    else
    {
        // now we setup the Sysptr based on information stored for this PNIO IF
        pSys->hd_nr                 = 0;                            // IOH is on LD
        pSys->pnio_if_nr            = 0;
        pSys->comp_id               = LSA_COMP_ID_IOH;
        pSys->comp_id_lower         = comp_id_lower;
        pSys->comp_id_edd           = 0;                            // no specific eddi used
        pSys->mbx_id_rsp            = mbx_id_rsp;                   // MBX for responses
        pSys->hSysDev               = 0;                            // not valid and not used
        pSys->hPoolDev              = PSI_DEV_POOL_HANDLE_INVALID;  // not valid and not used
        pSys->hPoolNrtTx            = PSI_NRT_POOL_HANDLE_INVALID;  // not valid and not used
        pSys->hPoolNrtRx            = PSI_NRT_POOL_HANDLE_INVALID;  // not valid and not used
        pSys->check_arp             = LSA_FALSE;
        pSys->hd_runs_on_level_ld   = pDetailsIOH->hd_runs_on_level_ld;
        pSys->trace_idx             = pDetailsIOH->trace_idx;
        pSys->psi_path              = path;

        *sys_ptr_ptr = pSys;
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_ioh_release_path_info(
    LSA_SYS_PTR_TYPE  const sys_ptr,
    LSA_VOID_PTR_TYPE const detail_ptr)
{
    LSA_UINT8                 hd_nr;
    LSA_UINT16                rc;
    PSI_SYS_PTR_TYPE    const pSys    = (PSI_SYS_PTR_TYPE)sys_ptr;
    IOH_DETAIL_PTR_TYPE const pDetail = (IOH_DETAIL_PTR_TYPE)detail_ptr;

    PSI_ASSERT(sys_ptr != LSA_NULL);
    PSI_ASSERT(detail_ptr != LSA_NULL);

    PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_IOH);

    PSI_HD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "psi_ioh_release_path_info()");

    for (hd_nr = 1; hd_nr <= pDetail->hd_count; hd_nr++)
    {
        if (pDetail->hd[hd_nr].device_info != LSA_NULL)
        {
            PSI_FREE_LOCAL_MEM(&rc, pDetail->hd[hd_nr].device_info->sysptr, 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
            PSI_ASSERT(rc == LSA_RET_OK);

            PSI_FREE_LOCAL_MEM(&rc, ((LSA_VOID_PTR_TYPE*)pDetail->hd[hd_nr].device_info), 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
            PSI_ASSERT(rc == LSA_RET_OK);
        }
    }

    PSI_FREE_LOCAL_MEM(&rc, sys_ptr, 0, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM);
    PSI_ASSERT(rc == LSA_RET_OK);

    return LSA_RET_OK;
}

#endif //  ((PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
