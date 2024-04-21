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
/*  F i l e               &F: psi_hd.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI HD instance                                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   5
#define PSI_MODULE_ID       5 /* PSI_MODULE_ID_PSI_HD */

#include "psi_int.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#if (PSI_CFG_USE_HD_COMP == 1)

/*---------------------------------------------------------------------------*/
/*  Types                                                                    */
/*---------------------------------------------------------------------------*/
typedef enum psi_hd_state_enum
{
	PSI_HD_INIT_STATE = 0,  // Reserved for invalid
	PSI_HD_INIT_DONE,       // INIT sequence done
	PSI_HD_OPENING,         // HD Open sequence in progress (startup)
	PSI_HD_OPEN_DONE,       // HD Open finished (ready for requests)
	PSI_HD_CLOSING          // HD Close sequence in progress (shutdown)
} PSI_HD_STATE_TYPE;

// PSI HD instance
typedef struct psi_hd_instance_tag
{
	PSI_HD_STATE_TYPE       hd_state;                           // current PSI HD state (see PSI_HD_STATE_TYPE)
	PSI_RQB_PTR_TYPE        hd_open_device_rqb_ptr;             // stored upper RQB from system request HD Open Device
	PSI_RQB_PTR_TYPE        hd_close_device_rqb_ptr;            // stored upper RQB from system request HD Close Device
	PSI_HD_STORE_PTR_TYPE   hd_store_ptr[PSI_CFG_MAX_IF_CNT];   // pointer to allocated instance data
    LSA_UINT8               hd_runs_on_level_ld;
} PSI_HD_INSTANCE_TYPE;

/*---------------------------------------------------------------------------*/
/*  Global data                                                              */
/*---------------------------------------------------------------------------*/
static PSI_HD_INSTANCE_TYPE psi_hd_inst = { PSI_HD_INIT_STATE, LSA_NULL, LSA_NULL, {LSA_NULL,}, PSI_HD_RUNS_ON_LEVEL_LD_YES };

#if (PSI_CFG_USE_EDDI == 1)
// reserve static memory for gshared SRD RAM used in EDDI open device and IOH 
// Note: need allocation of shared mem from upper side for LD/HD splitted
EDDI_GSHAREDMEM_TYPE psi_eddi_srd_shmem[PSI_CFG_MAX_IF_CNT];
#endif

#if (PSI_CFG_USE_EDDP == 1)
// reserve static memory for gshared SRD RAM used in EDDP open device and IOH 
// Note: need allocation of shared mem from upper side for LD/HD splitted
EDDP_GSHAREDMEM_TYPE psi_eddp_srd_shmem[PSI_CFG_MAX_IF_CNT];
#endif

// Number of opened HDs within one FW
static LSA_UINT32 psi_hd_open_cnt = 0;
#if (PSI_CFG_USE_EDDS == 1)
static LSA_UINT32 psi_hd_edds_rqb_low_cnt  = 0; // counter of used EDDS RQB low tasks
static LSA_UINT32 psi_hd_edds_rqb_high_cnt = 0; // counter of used EDDS RQB high tasks
#endif

/*---------------------------------------------------------------------------*/
/*  Prototypes                                                               */
/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_store_delete_and_response_to_ld(
    LSA_UINT16 const hd_nr );

/*---------------------------------------------------------------------------*/
static PSI_HD_IF_PTR_TYPE psi_hd_get_if_for_pnio_if_nr(
    LSA_UINT16 const pnio_if_nr )
{
	LSA_UINT16         hd_idx;
	PSI_HD_IF_PTR_TYPE p_if = LSA_NULL;

    // lookup to get the IF matching with PNIO IF nr
	// Note: necessary and used for NARE, TCIP adaption coding (ARP-Sniffer)
	// Init the PSI HD instance vars
	PSI_ASSERT(psi_hd_inst.hd_state != PSI_HD_INIT_STATE);
    PSI_ASSERT((pnio_if_nr > PSI_GLOBAL_PNIO_IF_NR) && (pnio_if_nr <= PSI_MAX_PNIO_IF_NR));

	for (hd_idx = 0; hd_idx < PSI_CFG_MAX_IF_CNT; hd_idx++)
	{
		PSI_HD_STORE_PTR_TYPE const p_hd = psi_hd_inst.hd_store_ptr[hd_idx];
	    PSI_ASSERT(p_hd != LSA_NULL);

        if ((p_hd->hd_if.is_used) && (p_hd->hd_if.pnio_if_nr == pnio_if_nr))
        {
			p_if = &(p_hd->hd_if);
            break; // one entry found - exit LOOP
        }
	}

	return p_if;
}

/*---------------------------------------------------------------------------*/
PSI_HD_STORE_PTR_TYPE psi_hd_get_hd_store(
    LSA_UINT16 const hd_nr )
{
	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	return (psi_hd_inst.hd_store_ptr[hd_nr-1]);
}

/*---------------------------------------------------------------------------*/
PSI_RQB_PTR_TYPE psi_hd_get_hd_open_device_rqb_ptr( LSA_VOID )
{
	PSI_ASSERT(psi_hd_inst.hd_open_device_rqb_ptr != LSA_NULL);
	PSI_ASSERT(psi_hd_inst.hd_state == PSI_HD_OPENING);

	return (psi_hd_inst.hd_open_device_rqb_ptr);
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_connect(
    PSI_HD_INPUT_PTR_TYPE const p_hd_in,
    PSI_HD_STORE_PTR_TYPE const p_hd )
{
	LSA_UINT16            ret;
    LSA_UINT32            port_idx;
	PSI_HD_PARAM_TYPE     hw_param;
	PSI_HD_SYS_ID_TYPE    hw_sys_id;
    PSI_HD_IF_PTR_TYPE    p_if;
    LSA_UINT16            hd_nr;

	PSI_ASSERT(p_hd_in != LSA_NULL);
	PSI_ASSERT(p_hd != LSA_NULL);

    hd_nr = p_hd_in->hd_id;
    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	PSI_ASSERT(!p_hd->is_used);

    PSI_MEMSET(&hw_param,  0, sizeof(hw_param));
	PSI_MEMSET(&hw_sys_id, 0, sizeof(hw_sys_id));

	// set the SYS address information
	hw_sys_id.hd_location   = p_hd_in->hd_location;
	hw_sys_id.asic_type     = p_hd_in->asic_type;
	hw_sys_id.rev_nr        = p_hd_in->rev_nr;
	hw_sys_id.edd_type      = p_hd_in->edd_type;

    PSI_HD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_connect(): -> PSI_GET_HD_PARAM()");
    /* try connection HD and getting the HW params */
	PSI_GET_HD_PARAM(&ret, p_hd_in, &hw_sys_id, &hw_param);
    PSI_HD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_connect(): <- PSI_GET_HD_PARAM()");

	if (ret != PSI_OK)
	{
		PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_connect(): getting HD params failed, hd_nr(%u) ret(%u/%#x)",
            hd_nr, ret, ret);
		PSI_FATAL(0);
	}

	PSI_ASSERT(hw_param.edd_type == p_hd_in->edd_type);

	// Store the HD information for this HD
	p_hd->hd_sys_handle   	 = hw_param.hd_sys_handle;
	p_hd->hd_hw.edd_type  	 = hw_param.edd_type;
	p_hd->hd_hw.dev_mem   	 = hw_param.dev_mem;
	p_hd->hd_hw.nrt_tx_mem   = hw_param.nrt_tx_mem;
	p_hd->hd_hw.nrt_rx_mem   = hw_param.nrt_rx_mem;
	p_hd->hd_hw.crt_slow_mem = hw_param.crt_slow_mem;
	p_hd->hd_hw.pi_mem       = hw_param.pi_mem;
	p_hd->hd_hw.hif_mem   	 = hw_param.hif_mem;
	p_hd->hd_hw.edd       	 = hw_param.edd;

	// EDDI information
    #if (PSI_CFG_USE_EDDI == 1)
    if (hw_param.edd_type == LSA_COMP_ID_EDDI)
    {
        if (p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_YES)
        {
		    // store shared mem in HD store
            p_hd->hd_hw.edd.eddi.g_shared_ram_base_ptr = &(psi_eddi_srd_shmem[hd_nr-1]);
            PSI_ASSERT(LSA_NULL != p_hd->hd_hw.edd.eddi.g_shared_ram_base_ptr);
        }
    }
    #endif //PSI_CFG_USE_EDDI

	// EDDP information
    #if (PSI_CFG_USE_EDDP == 1)
	if (hw_param.edd_type == LSA_COMP_ID_EDDP)
    {
        if (p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_YES)
	    {
		    // store shared mem in HD store
		    p_hd->hd_hw.edd.eddp.g_shared_ram_base_ptr = &(psi_eddp_srd_shmem[hd_nr-1]);
            PSI_ASSERT(LSA_NULL != p_hd->hd_hw.edd.eddp.g_shared_ram_base_ptr);
        }
    }
    #endif //PSI_CFG_USE_EDDP

	// EDDS information
    #if (PSI_CFG_USE_EDDS == 1)
    if (hw_param.edd_type == LSA_COMP_ID_EDDS)
    {
	    if (   (PSI_FEATURE_ENABLE == p_hd_in->additional_forwarding_rules_supported)
	        && (PSI_FEATURE_ENABLE == p_hd_in->application_exist))
	    {
            // bHsyncAppl == LSA_TRUE
            p_hd->hd_hw.edd.edds.bHsyncModeActive = LSA_TRUE;
            switch (psi_hd_edds_rqb_high_cnt)
            {
                case 0:
                {
                    p_hd->hd_hw.edd.edds.mbx_id_edds_rqb = PSI_MBX_ID_EDDS_RQB_H1;
                }
                break;
                case 1:
                {
                    p_hd->hd_hw.edd.edds.mbx_id_edds_rqb = PSI_MBX_ID_EDDS_RQB_H2;
                }
                break;
                default:
                {
		            PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_connect(): invalid psi_hd_edds_rqb_high_cnt(%u)", psi_hd_edds_rqb_high_cnt);
		            PSI_FATAL(0);
                }
            }
            psi_hd_edds_rqb_high_cnt++;
	    }
	    else
	    {
            // bHsyncAppl == LSA_FALSE
            p_hd->hd_hw.edd.edds.bHsyncModeActive = LSA_FALSE;
            switch (psi_hd_edds_rqb_low_cnt)
            {
                case 0:
                {
                    p_hd->hd_hw.edd.edds.mbx_id_edds_rqb = PSI_MBX_ID_EDDS_RQB_L1;
                }
                break;
                case 1:
                {
                    p_hd->hd_hw.edd.edds.mbx_id_edds_rqb = PSI_MBX_ID_EDDS_RQB_L2;
                }
                break;
                case 2:
                {
                    p_hd->hd_hw.edd.edds.mbx_id_edds_rqb = PSI_MBX_ID_EDDS_RQB_L3;
                }
                break;
                case 3:
                {
                    p_hd->hd_hw.edd.edds.mbx_id_edds_rqb = PSI_MBX_ID_EDDS_RQB_L4;
                }
                break;
                default:
                {
		            PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_connect(): invalid psi_hd_edds_rqb_low_cnt(%u)", psi_hd_edds_rqb_low_cnt);
		            PSI_FATAL(0);
                }
            }
            psi_hd_edds_rqb_low_cnt++;
	    }

        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_hd_connect(): psi_hd_edds_rqb_high_cnt(%u) psi_hd_edds_rqb_low_cnt(%u)", psi_hd_edds_rqb_high_cnt, psi_hd_edds_rqb_low_cnt);
    }
    #endif //PSI_CFG_USE_EDDS

    // Preset EDD-NRT-Copy-Interface
	p_hd->hd_hw.b_edd_nrt_copy_if_on = LSA_FALSE;

	// Init port map
	PSI_MEMSET(p_hd->hd_hw.port_map, 0, sizeof(p_hd->hd_hw.port_map));

    p_if                = &(p_hd->hd_if);
    p_if->is_used       = LSA_TRUE;
	p_if->pnio_if_nr    = p_hd_in->pnio_if.edd_if_id;
	p_if->sys_handle    = hw_param.hd_sys_handle;

	// Copy IF MAC addresses from HD System adaption
	PSI_MEMCPY(p_hd->hd_hw.if_mac, hw_param.if_mac, sizeof(PSI_MAC_TYPE));
	PSI_MEMCPY(p_hd_in->if_mac, hw_param.if_mac, sizeof(PSI_MAC_TYPE));

	for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
	{
        LSA_UINT32 HwPortId = hw_param.port_map[port_idx].hw_port_id;

        // Port invalid?
        if ((port_idx == 0) || (port_idx > p_hd_in->nr_of_ports))
        {
            HwPortId = 0; //HwPortId = 0 = reserved
        }

	    PSI_ASSERT(HwPortId <= PSI_CFG_MAX_PORT_CNT);

		// copy Port mapping for IF
		p_hd->hd_hw.port_map[port_idx] = hw_param.port_map[port_idx];
	}

    for (port_idx = 0; port_idx < p_hd_in->nr_of_ports; port_idx++)
	{
	    // Copy Port MAC addresses from HD system adaption
	    PSI_MEMCPY(p_hd->hd_hw.port_mac[port_idx], hw_param.port_mac[port_idx], sizeof(PSI_MAC_TYPE));
	    PSI_MEMCPY(p_hd_in->port_mac[port_idx], hw_param.port_mac[port_idx], sizeof(PSI_MAC_TYPE));
	}

	p_hd->nr_of_all_ports     = p_hd_in->nr_of_all_ports;
	p_hd->nr_of_all_if        = p_hd_in->nr_of_all_if;
    p_hd->hd_runs_on_level_ld = p_hd_in->hd_runs_on_level_ld;
    p_hd->nr_of_ports         = p_hd_in->nr_of_ports;

	p_hd->is_used = LSA_TRUE; // is deleted in psi_hd_store_delete_and_response_to_ld()
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_pool_create(
    LSA_UINT16 const hd_nr )
{
	// Note: NRT pool is only created for EDDI, EDDP HD
	PSI_HD_STORE_PTR_TYPE p_hd;
	LSA_INT               nrt_pool_handle;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    PSI_HD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_pool_create()");

    p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
	PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

    #if (EPS_PLF != EPS_PLF_SOC_MIPS)
    if // DEV memory pool requested?
       (p_hd->hd_hw.dev_mem.size)
    {
        // create DEV memory pool

	    LSA_INT dev_pool_handle = PSI_DEV_POOL_HANDLE_INVALID;

        PSI_HD_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "psi_hd_pool_create(): creating DEV pool at (base)0x08%x, (phys)0x08%x, size 0x%x, hd_nr(%u)",
            p_hd->hd_hw.dev_mem.base_ptr, p_hd->hd_hw.dev_mem.phy_addr, p_hd->hd_hw.dev_mem.size, hd_nr);

		PSI_CREATE_DEV_MEM_POOL(
			p_hd->hd_hw.dev_mem.base_ptr,
			p_hd->hd_hw.dev_mem.size,
			p_hd->hd_sys_handle,
			&dev_pool_handle);

		if (PSI_DEV_POOL_HANDLE_INVALID == dev_pool_handle)
		{
			PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating DEV pool failed, hd_nr(%u)", hd_nr);
			PSI_FATAL(0);
		}

		p_hd->dev_pool_handle = dev_pool_handle;
    }
    else // no DEV memory pool requested
    #endif //(EPS_PLF != EPS_PLF_SOC_MIPS)
    {
		p_hd->dev_pool_handle = PSI_DEV_POOL_HANDLE_INVALID;
    }

    // create standard NRT memory pools for TX and RX
    {
        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

        PSI_HD_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "psi_hd_pool_create(): creating NRT TX pool at base(0x08%x) phys(0x08%x) size(0x%x) hd_nr(%u)", 
            p_hd->hd_hw.nrt_tx_mem.base_ptr, p_hd->hd_hw.nrt_tx_mem.phy_addr, p_hd->hd_hw.nrt_tx_mem.size, hd_nr);

        PSI_CREATE_NRT_MEM_POOL(
		    p_hd->hd_hw.nrt_tx_mem.base_ptr,
		    p_hd->hd_hw.nrt_tx_mem.size,
		    p_hd->hd_sys_handle,
		    &nrt_pool_handle,
		    PSI_HD_NRT_TX_MEM );

	    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
	    {
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT TX pool failed, hd_nr(%u)", hd_nr);
		    PSI_FATAL(0);
	    }

	    p_hd->nrt_tx_pool_handle = nrt_pool_handle;

        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

        PSI_HD_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "psi_hd_pool_create(): creating NRT RX pool at base(0x08%x) phys(0x08%x) size(0x%x) hd_nr(%u)",
            p_hd->hd_hw.nrt_rx_mem.base_ptr, p_hd->hd_hw.nrt_rx_mem.phy_addr, p_hd->hd_hw.nrt_rx_mem.size, hd_nr);

        PSI_CREATE_NRT_MEM_POOL(
		    p_hd->hd_hw.nrt_rx_mem.base_ptr,
		    p_hd->hd_hw.nrt_rx_mem.size,
		    p_hd->hd_sys_handle,
		    &nrt_pool_handle,
		    PSI_HD_NRT_RX_MEM );

	    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
	    {
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT RX pool failed, hd_nr(%u)", hd_nr);
		    PSI_FATAL(0);
	    }

	    p_hd->nrt_rx_pool_handle = nrt_pool_handle;
    }

    #if (PSI_CFG_USE_EDDI == 1)
    if (p_hd->hd_hw.edd_type == LSA_COMP_ID_EDDI)
    {
        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
	    PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_dmacw_default_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_dmacw_default_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_EDDI_DMACW_DEFAULT);

	    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
	    {
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_EDDI_DMACW_DEFAULT failed, hd_nr(%u)", hd_nr);
		    PSI_FATAL(0);
	    }

	    p_hd->nrt_dmacw_default_pool_handle = nrt_pool_handle;

	    // EDDI needs some more pools for fragmentation
	    if (p_hd->hd_hw.edd.eddi.fragmentation_supported)
	    {
            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		    PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_low_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_low_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_EDDI_TX_LOW_FRAG);

		    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_EDDI_TX_LOW_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_low_frag_pool_handle = nrt_pool_handle;

            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		    PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_mid_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_mid_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_EDDI_TX_MID_FRAG);

		    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_EDDI_TX_MID_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

            p_hd->nrt_tx_mid_frag_pool_handle = nrt_pool_handle;

            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		    PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_high_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_high_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_EDDI_TX_HIGH_FRAG);

		    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_EDDI_TX_HIGH_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_high_frag_pool_handle = nrt_pool_handle;

            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		    PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_mgmtlow_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_mgmtlow_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG);

		    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_mgmtlow_frag_pool_handle = nrt_pool_handle;

            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		    PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_mgmthigh_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_mgmthigh_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG);

		    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_mgmthigh_frag_pool_handle = nrt_pool_handle;
	    }
    }
    #endif //PSI_CFG_USE_EDDI
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_pool_delete(
    LSA_UINT16 const hd_nr )
{
    // Note: NRT pool is only created for EDDI, EDDP HD
	PSI_HD_STORE_PTR_TYPE p_hd;
	LSA_UINT16            ret;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
    PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

    #if (EPS_PLF != EPS_PLF_SOC_MIPS)
    if // DEV memory pool exists?
       (p_hd->dev_pool_handle != PSI_DEV_POOL_HANDLE_INVALID)
    {
        // delete DEV memory pool

        ret = LSA_RET_ERR_PARAM;

		PSI_DELETE_DEV_MEM_POOL(
			&ret,
			p_hd->hd_sys_handle,
			p_hd->dev_pool_handle);

		if (ret != LSA_RET_OK)
		{
			PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting DEV pool failed, hd_nr(%u)", hd_nr);
			PSI_FATAL(0);
		}

		p_hd->dev_pool_handle = PSI_DEV_POOL_HANDLE_INVALID;
    }
    #endif //(EPS_PLF != EPS_PLF_SOC_MIPS)
    
    // delete standard NRT memory pools for TX and RX
    {
        ret = LSA_RET_ERR_PARAM;

        PSI_DELETE_NRT_MEM_POOL(
		    &ret,
		    p_hd->hd_sys_handle,
		    p_hd->nrt_tx_pool_handle,
		    PSI_HD_NRT_TX_MEM);

	    if (ret != LSA_RET_OK)
	    {
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT TX pool failed, hd_nr(%u)", hd_nr);
		    PSI_FATAL(0);
	    }

	    p_hd->nrt_tx_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

        ret = LSA_RET_ERR_PARAM;

        PSI_DELETE_NRT_MEM_POOL(
		    &ret,
		    p_hd->hd_sys_handle,
		    p_hd->nrt_rx_pool_handle,
		    PSI_HD_NRT_RX_MEM);

	    if (ret != LSA_RET_OK)
	    {
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT RX pool failed, hd_nr(%u)", hd_nr);
		    PSI_FATAL(0);
	    }

	    p_hd->nrt_rx_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
    }

    #if (PSI_CFG_USE_EDDI == 1)
	if (p_hd->hd_hw.edd_type == LSA_COMP_ID_EDDI)
    {
        ret = LSA_RET_ERR_PARAM;

	    PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_dmacw_default_pool_handle, PSI_HD_NRT_EDDI_DMACW_DEFAULT );

	    if (ret != LSA_RET_OK) 
	    {
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_EDDI_DMACW_DEFAULT failed, hd_nr(%u)", hd_nr);
		    PSI_FATAL(0);
	    }

	    p_hd->nrt_dmacw_default_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

	    // EDDI needs some more pools for fragmentation
	    if (p_hd->hd_hw.edd.eddi.fragmentation_supported)
	    {
            ret = LSA_RET_ERR_PARAM;

		    PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_tx_low_frag_pool_handle, PSI_HD_NRT_EDDI_TX_LOW_FRAG );

		    if (ret != LSA_RET_OK)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_EDDI_TX_LOW_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_low_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

            ret = LSA_RET_ERR_PARAM;

		    PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_tx_mid_frag_pool_handle, PSI_HD_NRT_EDDI_TX_MID_FRAG );

		    if (ret != LSA_RET_OK)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_EDDI_TX_MID_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_mid_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

            ret = LSA_RET_ERR_PARAM;

		    PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_tx_high_frag_pool_handle, PSI_HD_NRT_EDDI_TX_HIGH_FRAG );

		    if (ret != LSA_RET_OK)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_EDDI_TX_HIGH_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_high_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

            ret = LSA_RET_ERR_PARAM;

		    PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_tx_mgmtlow_frag_pool_handle, PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG );

		    if (ret != LSA_RET_OK)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_mgmtlow_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

            ret = LSA_RET_ERR_PARAM;

		    PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_tx_mgmthigh_frag_pool_handle, PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG );

		    if (ret != LSA_RET_OK)
		    {
			    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG failed, hd_nr(%u)", hd_nr);
			    PSI_FATAL(0);
		    }

		    p_hd->nrt_tx_mgmthigh_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
	    }
    }
    #endif //PSI_CFG_USE_EDDI
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_open(
	LSA_UINT16            hd_nr,
	PSI_HD_INPUT_PTR_TYPE p_hd_in,
	PSI_HD_STORE_PTR_TYPE p_hd )
{
	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
    PSI_ASSERT(p_hd_in != LSA_NULL);
	PSI_ASSERT(p_hd    != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

	PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_open(): EDD Open start, hd_nr(%u) edd_type(%u/%#x)", 
        hd_nr, p_hd_in->edd_type, p_hd_in->edd_type );

	switch (p_hd_in->edd_type)
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
		{
			PSI_EDDI_SETUP_PTR_TYPE const p_data = (PSI_EDDI_SETUP_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDI_SETUP_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDI_SETUP_TYPE));

			// Init and prepare the setup data (using EDDI conv. functions)
			// Note: the calculated PI size is set in HD store
			psi_eddi_prepare_setup_data(p_data, p_hd_in, p_hd);

			// set the open session data
			p_data->session.hd_nr           = hd_nr;
			p_data->session.hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->session.hd_sys_handle   = p_hd->hd_sys_handle;

			// start open EDDI device
			// Note: finish is done after setup sequence (see psi_eddi.c)
			psi_eddi_device_open(p_data);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
	    case LSA_COMP_ID_EDDP:
		{
			PSI_EDDP_SETUP_PTR_TYPE const p_data = (PSI_EDDP_SETUP_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDP_SETUP_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDP_SETUP_TYPE));

			// Init and prepare the setup data (using EDDP init functions)
			psi_eddp_prepare_setup_data(p_data, p_hd_in, p_hd); 

			// set the open session data (needed for async open)
			p_data->session.hd_nr           = hd_nr;
			p_data->session.act_if_idx      = 0;
			p_data->session.hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->session.hd_sys_handle   = p_hd->hd_sys_handle;

			// start open EDDP device
			// Note: finish is done after setup sequence (see psi_eddp.c)
			psi_eddp_device_open(&(p_hd->hd_hw.edd.eddp), p_data);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
	    case LSA_COMP_ID_EDDS:
		{
			PSI_EDDS_SETUP_PTR_TYPE const p_data = (PSI_EDDS_SETUP_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDS_SETUP_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDS_SETUP_TYPE));

			// Init and prepare the setup data (using EDDI conv. functions)
			// Note: the calculated PI size is set in HD store
			psi_edds_prepare_setup_data(p_data, p_hd_in, p_hd);

			// set the open session data
			p_data->session.hd_nr           = hd_nr;
			p_data->session.hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->session.hd_sys_handle   = p_hd->hd_sys_handle;

			// open EDDS device
			// Note: finish is done after setup sequence (see psi_edds.c)
			psi_edds_device_open(p_data);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
	    case LSA_COMP_ID_EDDT:
		{
			PSI_EDDT_SETUP_PTR_TYPE const p_data = (PSI_EDDT_SETUP_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDT_SETUP_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDT_SETUP_TYPE));

			// Init and prepare the setup data (using EDDT init functions)
			psi_eddt_prepare_setup_data(p_data, p_hd_in, p_hd); 

			// set the open session data (needed for async open)
			p_data->session.hd_nr           = hd_nr;
			p_data->session.act_if_idx      = 0;
			p_data->session.hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->session.hd_sys_handle   = p_hd->hd_sys_handle;

			// start open EDDT device
			// Note: finish is done after setup sequence (see psi_eddt.c)
			psi_eddt_device_open(&(p_hd->hd_hw.edd.eddt), p_data);
		}
		break;
        #endif

	    default:
		{
			PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_open(): invalid edd_type(%u/%#x)", p_hd_in->edd_type, p_hd_in->edd_type );
			PSI_FATAL(0);
		}
	}
    //lint -esym(765, psi_hd_edd_open)
    //to prevent the lint warning: "could be declared static"
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_open_done(
    LSA_UINT16        const hd_nr, 
    LSA_VOID_PTR_TYPE const p_data )
{
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE       cbf_fct;
	PSI_RQB_PTR_TYPE                const pRQB = psi_hd_get_hd_open_device_rqb_ptr();
	PSI_HD_STORE_PTR_TYPE                 p_hd;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
    PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

    psi_hd_open_cnt++;

    PSI_HD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_open_done(): EDD Open finished, hd_nr(%u) psi_hd_open_cnt(%u) edd_type(%u/%#x)",
        hd_nr, psi_hd_open_cnt, p_hd->hd_hw.edd_type, p_hd->hd_hw.edd_type );

    // Free the setup data
	psi_free_local_mem(p_data);

	// HD Open Device finished --> response to Upper (go back to PSI-LD)
	psi_hd_inst.hd_open_device_rqb_ptr = LSA_NULL;

	cbf_fct = pRQB->args.hd_open.psi_request_upper_done_ptr;

	PSI_RQB_SET_RESPONSE(pRQB, PSI_OK);
	psi_hd_inst.hd_state = PSI_HD_OPEN_DONE;

    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_hd_get_runs_on_level_ld() == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    psi_system_request_done(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)cbf_fct, pRQB);
    }
    else
    #endif
    {
	    psi_system_request_done(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)cbf_fct, pRQB);
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_close(
	LSA_UINT16            hd_nr,
	PSI_HD_STORE_PTR_TYPE p_hd )
{
	LSA_UINT16 edd_type;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

	edd_type = p_hd->hd_hw.edd_type;

	PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close(): EDD Close start, hd_nr(%u) edd_type(%u/%#x)",
	    hd_nr, edd_type, edd_type);

	switch (edd_type)
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
		{
			PSI_EDDI_SESSION_PTR_TYPE const p_data = (PSI_EDDI_SESSION_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDI_SESSION_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDI_SESSION_TYPE));

			// set the close session data
			p_data->hd_nr           = hd_nr;
			p_data->hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->hd_sys_handle   = p_hd->hd_sys_handle;

			// close EDDI device
			// Note: finish is done after close sequence (see psi_eddi.c)
			psi_eddi_device_close(p_data);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
    	case LSA_COMP_ID_EDDP:
		{
			PSI_EDDP_SESSION_PTR_TYPE const p_data = (PSI_EDDP_SESSION_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDP_SESSION_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDP_SESSION_TYPE));

			// set the close session data (needed for async close)
			p_data->hd_nr           = hd_nr;
			p_data->act_if_idx      = 0;
			p_data->hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->hd_sys_handle   = p_hd->hd_sys_handle;

			// close EDDP device, starting with closing the EDDP interfaces 
			// Note: finish is done after close sequence (see psi_eddp.c)
            psi_eddp_device_shutdown(p_data);
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDS == 1 )
	    case LSA_COMP_ID_EDDS:
		{
			PSI_EDDS_SESSION_PTR_TYPE const p_data = (PSI_EDDS_SESSION_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDS_SESSION_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDS_SESSION_TYPE));

			// set the close session data
			p_data->hd_nr           = hd_nr;
			p_data->hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->hd_sys_handle   = p_hd->hd_sys_handle;

			// close EDDS device
			// Note: finish is done after close sequence (see psi_edds.c)
			psi_edds_device_shutdown(p_data);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
    	case LSA_COMP_ID_EDDT:
		{
			PSI_EDDT_SESSION_PTR_TYPE const p_data = (PSI_EDDT_SESSION_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_EDDT_SESSION_TYPE));
			PSI_ASSERT(p_data);

			PSI_MEMSET(p_data, 0, sizeof(PSI_EDDT_SESSION_TYPE));

			// set the close session data (needed for async close)
			p_data->hd_nr           = hd_nr;
			p_data->act_if_idx      = 0;
			p_data->hDDB_ptr        = &(p_hd->edd_hDDB);
			p_data->hd_sys_handle   = p_hd->hd_sys_handle;

			// close EDDT device, starting with closing the EDDT interfaces 
			// Note: finish is done after close sequence (see psi_eddt.c)
            psi_eddt_device_shutdown(p_data);
		}
		break;
        #endif

	    default:
		{
			PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_close(): invalid edd_type(%u/0x%X)", edd_type, edd_type);
			PSI_FATAL(0);
		}
	}
    //lint -esym(765, psi_hd_edd_close)
    //to prevent the lint warning: "could be declared static"
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_close_done(
    LSA_UINT16        const hd_nr,
    LSA_VOID_PTR_TYPE const p_data )
{
	PSI_HD_STORE_PTR_TYPE p_hd;
	LSA_UINT16            ret    = PSI_ERR_PARAM;
    LSA_UINT16            mbx_id = PSI_MBX_ID_MAX;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(p_data);

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
	PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

    PSI_ASSERT(psi_hd_open_cnt);
    psi_hd_open_cnt--;

    #if (PSI_CFG_USE_EDDS == 1)
    if (p_hd->hd_hw.edd_type == LSA_COMP_ID_EDDS)
    {
        switch (p_hd->hd_hw.edd.edds.mbx_id_edds_rqb)
        {
            case PSI_MBX_ID_EDDS_RQB_H1:
            case PSI_MBX_ID_EDDS_RQB_H2:
            {
                PSI_ASSERT(psi_hd_edds_rqb_high_cnt);
                psi_hd_edds_rqb_high_cnt--;
            }
            break;
            case PSI_MBX_ID_EDDS_RQB_L1:
            case PSI_MBX_ID_EDDS_RQB_L2:
            case PSI_MBX_ID_EDDS_RQB_L3:
            case PSI_MBX_ID_EDDS_RQB_L4:
            {
                PSI_ASSERT(psi_hd_edds_rqb_low_cnt);
                psi_hd_edds_rqb_low_cnt--;
            }
            break;
            default:
            {
		        PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_close_done(): invalid mbx_id_edds_rqb(%u)", p_hd->hd_hw.edd.edds.mbx_id_edds_rqb);
		        PSI_FATAL(0);
            }
        }

        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_hd_edd_close_done(): psi_hd_edds_rqb_high_cnt(%u) psi_hd_edds_rqb_low_cnt(%u)", psi_hd_edds_rqb_high_cnt, psi_hd_edds_rqb_low_cnt);
    }
    #endif

    // free session data allocated for shutdown
    psi_free_local_mem(p_data);

	PSI_HD_TRACE_05(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close_done(): EDD Close finished, hd_nr(%u) hd_runs_on_level_ld(%u) psi_hd_open_cnt(%u) edd_type(%u/%#x)",
        hd_nr, p_hd->hd_runs_on_level_ld, psi_hd_open_cnt, p_hd->hd_hw.edd_type, p_hd->hd_hw.edd_type);

    // HD runs on LD level ?
	if (p_hd->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
	{
        // this HD runs on different systems from LD
		// Deactivate all active detail entries for PNIO IF
		// Note: removing is only done if HD runs not on same level as LD
		if (p_hd->hd_if.is_used) // Activated?
		{
			psi_remove_detail_store(p_hd->hd_if.pnio_if_nr);
		}
	}

	// Free the NRT pool
	psi_hd_pool_delete(hd_nr);

    PSI_HD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close_done(): -> PSI_FREE_HD_PARAM()");
    // disconnect HD
	PSI_FREE_HD_PARAM(&ret, hd_nr);
    PSI_HD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close_done(): <- PSI_FREE_HD_PARAM()");

	if (ret != PSI_OK)
	{
		PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_close_done(): free HD params failed, hd_nr(%u) ret(%u/%#x)", hd_nr, ret, ret );
		PSI_FATAL(0);
	}

    if ((p_hd->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO) && (psi_hd_open_cnt == 0))
    {
        // this HD runs on different systems from LD and this is last HD Close in this FW
        // PN-Stack-Undo-Init: HD components
        psi_pn_stack_undo_init();
    }

    if (psi_hd_open_cnt == 0)
    {
        // this is last HD Close in this FW
        // setup the EDD-Undo-Init of all EDDs in this FW (in EDD thread)

        // allocate the dummy RQB
        PSI_RQB_EDD_UNDO_INIT_PTR_TYPE const pRQBUndoInit = (PSI_RQB_EDD_UNDO_INIT_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_RQB_EDD_UNDO_INIT_TYPE));
	    PSI_ASSERT(pRQBUndoInit != LSA_NULL);

        // fill this dummy RQB
        PSI_MEMSET(pRQBUndoInit, 0, sizeof(PSI_RQB_EDD_UNDO_INIT_TYPE));
        PSI_RQB_SET_COMP_ID( pRQBUndoInit, LSA_COMP_ID_PSI );
        pRQBUndoInit->args.hd_nr = hd_nr;

	    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close_done(): setup edd_undo_init in EDD thread" );

        // init mbx_id for calling of the EDD-Undo-Init in EDD thread of this current EDD (last EDD was closed)
	    switch (p_hd->hd_hw.edd_type)
	    {
            #if (PSI_CFG_USE_EDDI == 1)
	        case LSA_COMP_ID_EDDI:
                mbx_id = PSI_MBX_ID_EDDI_REST;
		    break;
            #endif

            #if (PSI_CFG_USE_EDDP == 1)
	        case LSA_COMP_ID_EDDP:
                mbx_id = PSI_MBX_ID_EDDP_LOW;
		    break;
            #endif

            #if (PSI_CFG_USE_EDDS == 1)
	        case LSA_COMP_ID_EDDS:
                mbx_id = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
		    break;
            #endif

            #if (PSI_CFG_USE_EDDT == 1)
	        case LSA_COMP_ID_EDDT:
                mbx_id = PSI_MBX_ID_EDDT_LOW;
		    break;
            #endif

	        default:
		    {
			    PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_close_done(): invalid edd_type(%u/%#x)", 
                    p_hd->hd_hw.edd_type, p_hd->hd_hw.edd_type);
			    PSI_FATAL(0);
		    }
	    }

        psi_request_local(mbx_id, (PSI_REQUEST_FCT)psi_hd_setup_edd_undo_init, pRQBUndoInit);
    }
    else
    {
        // not all HDs are closed in this FW, so go back to PSI-LD
        psi_hd_store_delete_and_response_to_ld(hd_nr);
    }
}

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_init( LSA_VOID )
{
	LSA_UINT16 hd_idx;

	// init the PSI HD instance vars
	PSI_ASSERT(psi_hd_inst.hd_state == PSI_HD_INIT_STATE);

	for (hd_idx = 0; hd_idx < PSI_CFG_MAX_IF_CNT; hd_idx++)
	{
		PSI_HD_STORE_PTR_TYPE const p_hd = (PSI_HD_STORE_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_HD_STORE_TYPE));

		PSI_ASSERT(p_hd != LSA_NULL);

        // init the HD storage
	    PSI_MEMSET(p_hd, 0, sizeof(PSI_HD_STORE_TYPE));

        p_hd->is_used		     = LSA_FALSE;
		p_hd->dev_pool_handle	 = PSI_DEV_POOL_HANDLE_INVALID;
		p_hd->nrt_tx_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		p_hd->nrt_rx_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

		p_hd->hd_if.is_used   = LSA_FALSE;
		p_hd->hd_if.is_arp_on = LSA_FALSE;

        p_hd->eddi_state = PSI_HD_EDDI_STATE_INIT;
        p_hd->eddp_state = PSI_HD_EDDP_STATE_INIT;
        p_hd->edds_state = PSI_HD_EDDS_STATE_INIT;

        psi_hd_inst.hd_store_ptr[hd_idx] = p_hd;
	}

    psi_hd_open_cnt                     = 0;
    #if (PSI_CFG_USE_EDDS == 1)
    psi_hd_edds_rqb_low_cnt             = 0;
    psi_hd_edds_rqb_high_cnt            = 0;
    #endif
    psi_hd_inst.hd_open_device_rqb_ptr  = LSA_NULL;
	psi_hd_inst.hd_close_device_rqb_ptr = LSA_NULL;
	psi_hd_inst.hd_state                = PSI_HD_INIT_DONE;
}

/*---------------------------------------------------------------------------*/
/* Deinitialization                                                          */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_undo_init( LSA_VOID )
{
	LSA_UINT16 hd_idx;

	// undo INIT all PSI HD instance vars
	PSI_ASSERT(psi_hd_inst.hd_state == PSI_HD_INIT_DONE);

	// cleanup HD IF vars
	for (hd_idx = 0; hd_idx < PSI_CFG_MAX_IF_CNT; hd_idx++)
	{
		PSI_HD_STORE_PTR_TYPE const p_hd = psi_hd_inst.hd_store_ptr[hd_idx];

	    PSI_ASSERT(p_hd != LSA_NULL);
		PSI_ASSERT(!p_hd->is_used);
		PSI_ASSERT(!p_hd->hd_if.is_used);

		psi_free_local_mem(p_hd);

        psi_hd_inst.hd_store_ptr[hd_idx] = LSA_NULL;
	}

    psi_hd_open_cnt                     = 0;
    #if (PSI_CFG_USE_EDDS == 1)
    psi_hd_edds_rqb_low_cnt             = 0;
    psi_hd_edds_rqb_high_cnt            = 0;
    #endif
    psi_hd_inst.hd_open_device_rqb_ptr  = LSA_NULL;
	psi_hd_inst.hd_close_device_rqb_ptr = LSA_NULL;
	psi_hd_inst.hd_state                = PSI_HD_INIT_STATE;
}

/*---------------------------------------------------------------------------*/
/*  Request interface                                                        */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_open_device(
    PSI_RQB_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                       pnio_if_nr;
    PSI_UPPER_HD_OPEN_PTR_TYPE const p_open = &(rqb_ptr->args.hd_open);
	PSI_HD_INPUT_PTR_TYPE            p_hd_in;
	PSI_HD_OUTPUT_PTR_TYPE           p_hd_out;
	PSI_HD_STORE_PTR_TYPE            p_hd;

	// check params

	if ((p_open->hd_args.hd_id == 0) || (p_open->hd_args.hd_id > PSI_CFG_MAX_IF_CNT))
	{
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_hd_open_device(): invalid hd_nr(%u) rqb_ptr(0x%08x)",
			p_open->hd_args.hd_id, rqb_ptr );
		PSI_FATAL(0);
	}

    if (psi_hd_open_cnt >= PSI_CFG_MAX_IF_CNT)
    {
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_hd_open_device(): psi_hd_open_cnt(%u) maximum reached - no more HD Open, rqb_ptr(0x%08x)",
			psi_hd_open_cnt, rqb_ptr );
		PSI_FATAL(0);
    }

    // trace input parameter
    PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_device(): hd_nr(%u) hd_runs_on_level_ld(%u) psi_hd_open_cnt(%u)",
		p_open->hd_args.hd_id, p_open->hd_args.hd_runs_on_level_ld, psi_hd_open_cnt );
    PSI_HD_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_device(): nr_of_ports(%u) nr_of_all_ports(%u) nr_of_all_if(%u) edd_type(%#x) asic_type(0x%X) rev_nr(%u) send_clock_factor(%u)",
        p_open->hd_args.nr_of_ports,
        p_open->hd_args.nr_of_all_ports,
        p_open->hd_args.nr_of_all_if,
        p_open->hd_args.edd_type,
        p_open->hd_args.asic_type,
        p_open->hd_args.rev_nr,
        p_open->hd_args.send_clock_factor );
    PSI_HD_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_device(): fill_active(%u) io_configured(%u) additional_forwarding_rules_supported(%u) application_exist(%u) cluster_IP_support(%u) mra_enable_legacy_mode(%u) irte.buffer_capacity_use_case(0x%X)",
        p_open->hd_args.fill_active, 
        p_open->hd_args.io_configured,
		p_open->hd_args.additional_forwarding_rules_supported,
		p_open->hd_args.application_exist,
        p_open->hd_args.cluster_IP_support,
        p_open->hd_args.mra_enable_legacy_mode,
        p_open->hd_args.irte.buffer_capacity_use_case);
    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_device(): EDDI-FeatureSupport.use_settings(%u) - short_preamble_supported(%u) mrpd_supported(%u) fragmentationtype_supported(%u) irt_forwarding_mode_supported(%u) max_dfp_frames(%u)", 
        p_open->hd_args.irte.FeatureSupport.use_settings, 
        p_open->hd_args.irte.FeatureSupport.short_preamble_supported, 
        p_open->hd_args.irte.FeatureSupport.mrpd_supported, 
        p_open->hd_args.irte.FeatureSupport.fragmentationtype_supported, 
        p_open->hd_args.irte.FeatureSupport.irt_forwarding_mode_supported, 
        p_open->hd_args.irte.FeatureSupport.max_dfp_frames );
    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_device(): EDDP-FeatureSupport.use_settings(%u) - short_preamble_supported(%u) mrpd_supported(%u) fragmentationtype_supported(%u) irt_forwarding_mode_supported(%u) max_dfp_frames(%u)", 
        p_open->hd_args.pnip.FeatureSupport.use_settings, 
        p_open->hd_args.pnip.FeatureSupport.short_preamble_supported, 
        p_open->hd_args.pnip.FeatureSupport.mrpd_supported, 
        p_open->hd_args.pnip.FeatureSupport.fragmentationtype_supported, 
        p_open->hd_args.pnip.FeatureSupport.irt_forwarding_mode_supported, 
        p_open->hd_args.pnip.FeatureSupport.max_dfp_frames );

    // Start HD Open Device
	psi_hd_inst.hd_open_device_rqb_ptr  = rqb_ptr;
	psi_hd_inst.hd_state                = PSI_HD_OPENING;
    psi_hd_inst.hd_runs_on_level_ld     = p_open->hd_args.hd_runs_on_level_ld;

	p_hd_in  = &(p_open->hd_args);
	p_hd_out = &(p_open->hd_out);
	p_hd     = psi_hd_inst.hd_store_ptr[p_hd_in->hd_id-1];
    PSI_ASSERT(p_hd != LSA_NULL);

    if ((p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO) && (psi_hd_open_cnt == 0))
    {
        // This HD runs on different systems from LD and this is first HD Open in this FW.
        // EDD-Init of all EDDs in this FW.
        psi_edd_init();
        // PN-Stack-Init: HD components
        psi_pn_stack_init();
    }

	// Get the HD HW params
	psi_hd_connect(p_hd_in, p_hd);

	// Create the NRT pool for this HD
	psi_hd_pool_create(p_hd_in->hd_id);

	// init HD_OUTPUT_TYPE with initial values
	psi_res_calc_init_hd_outputs(p_hd_in, p_hd, p_hd_out);

	// Calculate the channel details for the HD-IF from the inputs
	// Note: this information is needed for XXX_GET_PATH_INFO during open_channel_xx
	pnio_if_nr = p_hd_in->pnio_if.edd_if_id;

    // HD runs on LD level ?
    if (p_hd->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
	{
        // this HD runs on different systems from LD
		// Activate entry for PNIO IF
		// Note: adding is only done if HD runs not on same level as LD
		psi_add_detail_store(pnio_if_nr, p_hd_in);
	}

	// Calculate the details for this interface
	psi_res_calc_set_if_details(
        p_hd->hd_if.sys_handle,
		p_hd->dev_pool_handle,
		p_hd->nrt_tx_pool_handle,
		p_hd->nrt_rx_pool_handle,
		p_hd_in);

	// Start open EDDx Device
	psi_hd_edd_open(p_hd_in->hd_id, p_hd_in, p_hd);

    // Calculate the output details for this interface
    // psi_eddi_prepare_setup_data may change pi_mem size, so hd_out args have to be copied after psi_hd_edd_open function.
    psi_res_calc_set_if_outputs(p_hd_in, p_hd_out);

    #if !defined(PSI_CFG_EDDI_CFG_SOC)
    //Copy IO-parameters from store to hd out, so application gets valid adresses: only for EDDI ERTEC200 and ERTEC400
    p_hd_out->edd.eddi.io_mem_size = p_hd->hd_hw.pi_mem.size;
    #endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_close_device(
    PSI_RQB_PTR_TYPE const rqb_ptr )
{
	PSI_HD_STORE_PTR_TYPE             p_hd;
	PSI_UPPER_HD_CLOSE_PTR_TYPE const p_close = &(rqb_ptr->args.hd_close);

	// check the params

	if ((p_close->hd_id == 0) || (p_close->hd_id > PSI_CFG_MAX_IF_CNT))
	{
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_hd_close_device(): invalid hd_nr(%u) rqb_ptr(0x%08x)",
			p_close->hd_id, rqb_ptr );
		PSI_FATAL(0);
	}

    // Start HD Close Device
	psi_hd_inst.hd_close_device_rqb_ptr = rqb_ptr;
	psi_hd_inst.hd_state                = PSI_HD_CLOSING;

	p_hd = psi_hd_inst.hd_store_ptr[p_close->hd_id-1];
    PSI_ASSERT(p_hd != LSA_NULL);

	PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_close_device(): hd_nr(%u) hd_runs_on_level_ld(%u) psi_hd_open_cnt(%u)",
        p_close->hd_id, p_hd->hd_runs_on_level_ld, psi_hd_open_cnt);

    // Close EDD device first
	psi_hd_edd_close((LSA_UINT16)p_close->hd_id, p_hd);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_set_arp_on_state(
    LSA_UINT16 const pnio_if_nr,
    LSA_BOOL   const state )
{
	PSI_HD_IF_PTR_TYPE p_if;

	PSI_ASSERT((pnio_if_nr > PSI_GLOBAL_PNIO_IF_NR) && (pnio_if_nr <= PSI_MAX_PNIO_IF_NR));

	p_if = psi_hd_get_if_for_pnio_if_nr(pnio_if_nr);

	if (p_if != LSA_NULL)
	{
		p_if->is_arp_on = state;
	}
}

/*---------------------------------------------------------------------------*/
LSA_BOOL psi_hd_is_arp_on_state(
    LSA_UINT16 const pnio_if_nr )
{
	PSI_HD_IF_PTR_TYPE p_if;
	LSA_BOOL           b_result;

	PSI_ASSERT((pnio_if_nr > PSI_GLOBAL_PNIO_IF_NR) && (pnio_if_nr <= PSI_MAX_PNIO_IF_NR));

	p_if = psi_hd_get_if_for_pnio_if_nr(pnio_if_nr);

	b_result = (p_if != LSA_NULL) ? p_if->is_arp_on : LSA_FALSE;

	return b_result;
}

/*---------------------------------------------------------------------------*/
LSA_INT psi_hd_get_nrt_tx_pool_handle(
    LSA_UINT16 const hd_nr )
{
	LSA_INT nrt_tx_pool_handle;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL);

	// Return HD NRT Pool Handle (needed for HIF_NRT_ALLOC_xxx(), HIF_NRT_FREE_xxx())
	nrt_tx_pool_handle = psi_hd_inst.hd_store_ptr[hd_nr-1]->nrt_tx_pool_handle;

	return nrt_tx_pool_handle;
}

/*---------------------------------------------------------------------------*/
LSA_INT psi_hd_get_nrt_rx_pool_handle(
    LSA_UINT16 const hd_nr )
{
	LSA_INT nrt_rx_pool_handle;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL);

	// Return HD NRT Pool Handle (needed for HIF_NRT_ALLOC_xxx(), HIF_NRT_FREE_xxx())
	nrt_rx_pool_handle = psi_hd_inst.hd_store_ptr[hd_nr-1]->nrt_rx_pool_handle;

	return nrt_rx_pool_handle;
}

/*---------------------------------------------------------------------------*/
LSA_BOOL psi_hd_is_edd_nrt_copy_if_on(
    LSA_UINT16 const hd_nr )
{
	LSA_BOOL b_edd_nrt_copy_if_on;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL);

	// Return boolean with HD EDD NRT Copy Interface activation (needed for HIF_NRT_ALLOC_xxx(), HIF_NRT_FREE_xxx(), ...)
    // (with active EDD NRT Copy Interface applications can use local memory instead of NRT memory)
    b_edd_nrt_copy_if_on = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_hw.b_edd_nrt_copy_if_on;

	return b_edd_nrt_copy_if_on;
}

/*---------------------------------------------------------------------------*/
LSA_UINT8 psi_hd_get_runs_on_level_ld( LSA_VOID )
{
    PSI_ASSERT(psi_hd_inst.hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX);
    #if defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
    return PSI_HD_RUNS_ON_LEVEL_LD_NO;
    #else
    return psi_hd_inst.hd_runs_on_level_ld;
    #endif
}

/*---------------------------------------------------------------------------*/
/*  Access to EDDx                                                           */
/*---------------------------------------------------------------------------*/

PSI_EDD_HDDB psi_hd_get_hDDB(
    LSA_UINT16 const hd_nr )
{
	PSI_EDD_HDDB edd_handle;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL);

	edd_handle = psi_hd_inst.hd_store_ptr[hd_nr-1]->edd_hDDB;

	return edd_handle;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_get_if_mac(
	LSA_UINT16     hd_nr,
	LSA_UINT32   * if_ptr,
	PSI_MAC_TYPE * mac_ptr )
{
	PSI_HD_STORE_PTR_TYPE p_hd;
	PSI_HD_IF_PTR_TYPE    p_if;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
    PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

	p_if = &(p_hd->hd_if);
	PSI_ASSERT(p_if->is_used);

	PSI_MEMCPY(mac_ptr, p_hd->hd_hw.if_mac, sizeof(PSI_MAC_TYPE));

	*if_ptr = p_if->pnio_if_nr;
}

/*---------------------------------------------------------------------------*/
/*  Access to EDDx IR-Handling                                               */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_interrupt(
    LSA_UINT16 const hd_nr,
    LSA_UINT32 const int_src )
{
	PSI_HD_STORE_PTR_TYPE p_hd;

	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
    PSI_ASSERT(p_hd != LSA_NULL);

    PSI_HD_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_interrupt(): hd_nr(%u) int_src(%#x) edd_type(%u/%#x) edd_hDDB(0x%X) eddi_state(%u) eddp_state(%u) edds_state(%u)",
        hd_nr, int_src, p_hd->hd_hw.edd_type, p_hd->hd_hw.edd_type, p_hd->edd_hDDB, p_hd->eddi_state, p_hd->eddp_state, p_hd->edds_state );

    if (p_hd->edd_hDDB != 0)
	{
		switch (p_hd->hd_hw.edd_type)
		{
            #if (PSI_CFG_USE_EDDI == 1)
		    case LSA_COMP_ID_EDDI:
			{
                /* only allowed between DEV_OPEN_DONE and DEV_SETUP_DONE */
                if ((p_hd->eddi_state >= PSI_HD_EDDI_STATE_DEV_OPEN_DONE) && (p_hd->eddi_state <= PSI_HD_EDDI_STATE_DEV_SETUP_DONE))
                {
				    eddi_interrupt(p_hd->edd_hDDB);
                    #if ( defined(PSI_CFG_EDDI_CFG_SOC) || defined(EDDI_CFG_SII_USE_IRQ_SP) ) && !defined(PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI)
				    eddi_interrupt_set_eoi(p_hd->edd_hDDB, 0x0F); // external ICU
                    #endif
                }
			}
			break;
            #endif

            #if (PSI_CFG_USE_EDDP == 1)
		    case LSA_COMP_ID_EDDP:
			{
                /* only allowed between DEV_OPEN_DONE and DEV_SHUTDOWN_DONE */
                if ((p_hd->eddp_state >= PSI_HD_EDDP_STATE_DEV_OPEN_DONE) && (p_hd->eddp_state <= PSI_HD_EDDP_STATE_DEV_SHUTDOWN_DONE))
                {
				    eddp_interrupt(p_hd->edd_hDDB, int_src);
                }
			}
			break;
            #endif

            #if (PSI_CFG_USE_EDDS == 1)
		    case LSA_COMP_ID_EDDS:
			{
                /* only allowed between DEV_OPEN_DONE and DEV_SHUTDOWN_DONE */
                if ((p_hd->edds_state >= PSI_HD_EDDS_STATE_DEV_OPEN_DONE) && (p_hd->edds_state <= PSI_HD_EDDS_STATE_DEV_SHUTDOWN_DONE))
                {
                    #if defined(PSI_CFG_MEASURE_EDDS_SCHEDULER)
			        LSA_UINT64 const uStartTime = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;
			        LSA_UINT64       uEndTime;
			        LSA_UINT64       uDiffTime;
                    #endif

                    edds_scheduler(p_hd->edd_hDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);

                    #if defined(PSI_CFG_MEASURE_EDDS_SCHEDULER)
                    uEndTime  = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;
                    uDiffTime = uEndTime - uStartTime;

                    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_hd_interrupt(): edds_scheduler() - call by interrupt - %u ns runtime", uDiffTime);
                    #endif
                }
			}
			break;
            #endif

            #if (PSI_CFG_USE_EDDT == 1)
		    case LSA_COMP_ID_EDDT:
			{
                /* only allowed between DEV_OPEN_DONE and DEV_SHUTDOWN_DONE */
                if ((p_hd->eddt_state >= PSI_HD_EDDT_STATE_DEV_OPEN_DONE) && (p_hd->eddt_state <= PSI_HD_EDDT_STATE_DEV_SHUTDOWN_DONE))
                {
                    #if defined(PSI_CFG_MEASURE_EDDT_INTERRUPT)
			        LSA_UINT64 const uStartTime = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;
			        LSA_UINT64       uEndTime;
			        LSA_UINT64       uDiffTime;
                    #endif

				    eddt_interrupt(p_hd->edd_hDDB);

                    #if defined(PSI_CFG_MEASURE_EDDT_INTERRUPT)
                    uEndTime  = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;
                    uDiffTime = uEndTime - uStartTime;

                    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_hd_interrupt(): eddt_interrupt() - call by interrupt - %u ns runtime", uDiffTime);
                    #endif
                }
			}
			break;
            #endif

            default:
			{
				LSA_UNUSED_ARG(int_src);
			}
		}
	}
}

/*-------------------------------------------------------------------------------------*/
/* int from external timer occured (replaces NEWCYCLE-Int) */
LSA_VOID psi_hd_eddi_exttimer_interrupt(
    LSA_UINT16 const hd_nr )
{
	PSI_HD_STORE_PTR_TYPE p_hd;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
	PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

    PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW,
        "psi_hd_eddi_exttimer_interrupt(): hd_nr(%u) edd_hDDB(0x%X) eddi_state(%u)",
        hd_nr, p_hd->edd_hDDB, p_hd->eddi_state);

	if (p_hd->edd_hDDB != 0)
	{
		switch (p_hd->hd_hw.edd_type)
		{
            #if (PSI_CFG_USE_EDDI == 1)
			case LSA_COMP_ID_EDDI:
			{
                /* only allowed between DEV_OPEN_DONE and DEV_SETUP_DONE */
                if ((p_hd->eddi_state >= PSI_HD_EDDI_STATE_DEV_OPEN_DONE) && (p_hd->eddi_state <= PSI_HD_EDDI_STATE_DEV_SETUP_DONE))
                {
                    eddi_ExtTimerInterrupt(p_hd->edd_hDDB);
                    // EOI must not be set for polling mode or special EOI mode
                    #if !(defined(PSI_CFG_EDDI_CFG_SII_POLLING_MODE) || defined(PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI))
				    eddi_interrupt_set_eoi(p_hd->edd_hDDB, 0x0F);  // external ICU
                    #endif
                }
			}
			break;
            #endif

            default: break;
		}
	}
}

#if (PSI_CFG_USE_EDDS == 1)
/*-------------------------------------------------------------------------------------*/
/* polling EDDS interrupts (using LL adaption)                                         */
/*-------------------------------------------------------------------------------------*/
LSA_BOOL psi_hd_isr_poll(
    LSA_UINT16 const hd_nr )
{
    PSI_HD_STORE_PTR_TYPE p_hd;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
	PSI_ASSERT(p_hd != LSA_NULL);
	PSI_ASSERT(p_hd->is_used);

    PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_isr_poll(): hd_nr(%u) edd_hDDB(0x%X) edds_state(%u)",
        hd_nr, p_hd->edd_hDDB, p_hd->edds_state );

    if (p_hd->edd_hDDB != 0)
    {
        /* only allowed between DEV_OPEN_DONE and DEV_SHUTDOWN_DONE */
        if ((p_hd->edds_state >= PSI_HD_EDDS_STATE_DEV_OPEN_DONE) && (p_hd->edds_state <= PSI_HD_EDDS_STATE_DEV_SHUTDOWN_DONE))
        {
            #if defined(PSI_CFG_MEASURE_EDDS_SCHEDULER)
			LSA_UINT64 const uStartTime = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;
			LSA_UINT64       uEndTime;
			LSA_UINT64       uDiffTime;
            #endif

            edds_scheduler(p_hd->edd_hDDB, EDDS_SCHEDULER_CYCLIC_CALL);

            #if defined(PSI_CFG_MEASURE_EDDS_SCHEDULER)
            uEndTime  = PSI_GET_TICKS_100NS() * (LSA_UINT64)100;
            uDiffTime = uEndTime - uStartTime;

            PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_hd_isr_poll(): edds_scheduler() - call by poll thread - %u ns runtime", uDiffTime);
            #endif
        }
    }

    return LSA_FALSE;
}

/*-------------------------------------------------------------------------------------*/
/* access ot EDDS PI memory managment (see edds setup)                                 */
/*-------------------------------------------------------------------------------------*/
LSA_VOID_PTR_TYPE psi_hd_get_edd_io_handle(
    LSA_UINT16 const hd_nr )
{
	LSA_VOID_PTR_TYPE pIO;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));
	PSI_ASSERT(psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL);

	pIO = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_hw.edd.edds.io_buffer_handle;

	return pIO;
}
#endif //PSI_CFG_USE_EDDS

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_setup_edd_undo_init(
    PSI_RQB_EDD_UNDO_INIT_PTR_TYPE const pRQB )
{
    PSI_ASSERT(pRQB != LSA_NULL);

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_setup_edd_undo_init(): hd_nr(%u)", pRQB->args.hd_nr);

    // EDD-Undo-Init (here within EDD thread)
    psi_edd_undo_init();

    // and go back to PSI thread
    psi_request_local(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_setup_edd_undo_init_done, pRQB);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_setup_edd_undo_init_done(
    PSI_RQB_EDD_UNDO_INIT_PTR_TYPE const pRQB )
{
    LSA_UINT16 hd_nr;

    PSI_ASSERT(pRQB != LSA_NULL);

    hd_nr = pRQB->args.hd_nr;

    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_setup_edd_undo_init_done(): hd_nr(%u)", hd_nr );

    // Free the dummy RQB of EDD-Undo-Init
    psi_free_local_mem(pRQB);

    // delete the HD storage and go back to PSI-LD
    psi_hd_store_delete_and_response_to_ld(hd_nr);
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_store_delete_and_response_to_ld(
    LSA_UINT16 const hd_nr )
{
	PSI_HD_STORE_PTR_TYPE           p_hd;
    PSI_RQB_PTR_TYPE                pRQB;
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE cbf_fct;
    LSA_UINT32                      hd_runs_on_level_ld;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_store_delete_and_response_to_ld(): hd_nr(%u)", hd_nr);

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];
    PSI_ASSERT(p_hd != LSA_NULL);
    PSI_ASSERT(p_hd->is_used);

    // store hd_runs_on_level_ld for response
    hd_runs_on_level_ld = p_hd->hd_runs_on_level_ld;

    // delete the HD storage
	PSI_MEMSET(p_hd, 0, sizeof(PSI_HD_STORE_TYPE));
    p_hd->is_used = LSA_FALSE;

    // HD Close Device finished --> response to Upper (go back to PSI-LD)
	pRQB                                = psi_hd_inst.hd_close_device_rqb_ptr;
	psi_hd_inst.hd_close_device_rqb_ptr = LSA_NULL;

	cbf_fct = pRQB->args.hd_close.psi_request_upper_done_ptr;

	PSI_RQB_SET_RESPONSE(pRQB, PSI_OK);
	psi_hd_inst.hd_state = PSI_HD_INIT_DONE;

    // go back to PSI-LD
    #if (PSI_CFG_USE_HIF_HD == 1)
    if (hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    psi_system_request_done(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)cbf_fct, pRQB);
    }
    else
    #endif
    {
        LSA_UNUSED_ARG(hd_runs_on_level_ld);
        psi_system_request_done(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)cbf_fct, pRQB);
    }

    #if (PSI_CFG_USE_HIF_HD == 0)
    LSA_UNUSED_ARG(hd_runs_on_level_ld); // suppress compiler warning
    #endif
}

#endif //PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
