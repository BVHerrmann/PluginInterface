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
/*  F i l e               &F: psi_ld.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI LD instance.                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   4
#define PSI_MODULE_ID       4 /* PSI_MODULE_ID_PSI_LD */

#include "psi_int.h"
#include "psi_ld.h"

#if (PSI_CFG_USE_LD_COMP == 1)

#if (PSI_CFG_USE_IOH)
#if defined(PSI_CFG_EDDP_CFG_HW_ERTEC200P_SUPPORT)
#include "perif_cfg.h"                                  /* creating perif for ertec200p */
#endif
#endif

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#define PSI_LD_IF_MAX_REQ   ( (PSI_MAX_PNIO_IF_NR+1) * PSI_SYS_PATH_MAX )

#define PSI_LD_ERROR_CNT_EXIT_CHECK     0xFFFF // for "error_cnt" in psi_ld_check functions to return the function without further checks

/*---------------------------------------------------------------------------*/
/*  Types                                                                    */
/*---------------------------------------------------------------------------*/
typedef enum psi_ld_state_enum
{
	PSI_LD_INIT_STATE = 0,  /* Reserved for invalid */
	PSI_LD_INIT_DONE,       /* INIT sequence done */
	PSI_LD_OPENING,         /* LD Open sequence in progress (startup) */
	PSI_LD_OPEN_DONE,       /* LD Open finished (ready for requests) */
	PSI_LD_CLOSING          /* LD Close sequence in progress (shutdown) */
} PSI_LD_STATE_TYPE;

// Open/Close function adaption for SYS channel open/Close
typedef void (*PSI_LD_CHN_OPEN_FCT)(LSA_SYS_PATH_TYPE);
typedef void (*PSI_LD_CHN_CLOSE_FCT)(LSA_SYS_PATH_TYPE, LSA_HANDLE_TYPE);

typedef struct psi_channel_fct_tag
{
	PSI_LD_CHN_OPEN_FCT     fct_open;
	PSI_LD_CHN_CLOSE_FCT    fct_close;
} PSI_LD_CHANNEL_FCT_TYPE, *PSI_LD_CHANNEL_FCT_PTR_TYPE;

/*---------------------------------------------------------------------------*/

// PSI LD request entry for one HD
typedef struct psi_ld_hd_req_entry_tag
{
	LSA_UINT16          hd_nr;                  // HD number
	PSI_HD_ID_TYPE      hd_location;            // For HIF HD Upper open
    LSA_UINT8           hd_runs_on_level_ld;    // For HIF HD Lower open - 1: HD is on LD level / 0: HD is on separate level from LD
} PSI_LD_HD_REQ_ENTRY_TYPE;

// PSI LD request list for all HD requests
typedef struct psi_ld_hd_req_tag
{
	LSA_UINT16                  nr_of_hd;                   // count of HDs in list
	LSA_INT                     act_req_idx;                // actual handled HD - index in hd_req[0..3]
	PSI_LD_HD_REQ_ENTRY_TYPE    hd_req[PSI_CFG_MAX_IF_CNT]; // array of HD entries
} PSI_LD_HD_REQ_STORE_TYPE, *PSI_LD_HD_REQ_STORE_PTR_TYPE;

/*---------------------------------------------------------------------------*/

// PSI LD IF request entry for one HD IF
typedef struct psi_ld_if_req_entry_tag
{
	LSA_UINT16      hd_nr;          // HD number
	LSA_UINT16      pnio_if_nr;     // PNIO IF number for this HD-IF: [0..16] (0=global, 1-16=PNIO IF) (set by TIA)
	LSA_UINT16      sys_path;       // Sys-Path
} PSI_LD_IF_REQ_ENTRY_TYPE;

// PSI LD request list for all HD requests
typedef struct psi_ld_if_req_tag
{
	LSA_UINT16                  nr_of_req;                  // count of Req. in list
	LSA_INT                     act_req_idx;                // actual handled Req
	PSI_LD_IF_REQ_ENTRY_TYPE    req[PSI_LD_IF_MAX_REQ];     // IF entries
} PSI_LD_IF_REQ_STORE_TYPE, *PSI_LD_IF_REQ_STORE_PTR_TYPE;

/*---------------------------------------------------------------------------*/

// PSI HD routing information
typedef struct psi_ld_hd_store_tag
{
	LSA_UINT16          hd_nr;                  // HD number
    LSA_UINT8           hd_runs_on_level_ld;    // For HIF HD Upper open - 1: HD is on LD level / 0: HD is on separate level from LD
} PSI_LD_HD_ENTRY_TYPE;

// PSI LD store for HD routing information
typedef struct psi_ld_store_hd_tag
{
    PSI_LD_HD_ENTRY_TYPE    comp[PSI_MAX_COMPONENTS][PSI_MAX_CHANNELS_PER_COMPONENT];
} PSI_LD_STORE_HD_TYPE;

// PSI LD instance store
typedef struct psi_ld_store_tag
{
    LSA_HANDLE_TYPE             global_lower_handle[PSI_SYS_PATH_MAX];  // lower handle for global sys channels
    PSI_LD_CHANNEL_FCT_TYPE     global_fct[PSI_SYS_PATH_MAX];           // fct list for open/close the global sys channels 
    PSI_LD_CHANNEL_FCT_TYPE     if_fct[PSI_SYS_PATH_MAX];               // fct list for open/close the IF sys channels 
    PSI_LD_HD_REQ_STORE_TYPE    hd_req;                                 // HD request storage
    PSI_LD_IF_REQ_STORE_TYPE    if_req;                                 // HD IF request storage
    PSI_LD_HD_TYPE              hd[PSI_CFG_MAX_IF_CNT];                 // all HD instances
} PSI_LD_STORE_TYPE, *PSI_LD_STORE_PTR_TYPE;

// PSI LD instance
typedef struct psi_ld_instance_tag
{
	PSI_LD_STATE_TYPE       ld_state;           // actual PSI LD state (see PSI_LD_STATE_TYPE)
	PSI_LD_STORE_PTR_TYPE   ld_store_ptr;       // allocated instance data
	PSI_RQB_PTR_TYPE        ld_upper_rqb;       // stored upper RQB from system request
	PSI_SOCK_INPUT_TYPE     ld_sock_details;    // stored sock details from system request
    PSI_LD_RUNS_ON_TYPE     ld_runs_on;
    PSI_LD_STORE_HD_TYPE    hd_runs_on;
} PSI_LD_INSTANCE_TYPE;

/*---------------------------------------------------------------------------*/
/*  Global data                                                              */
/*---------------------------------------------------------------------------*/
//lint -save -esym(785, psi_ld_inst)
// we need only the first 3 parameters for initialization
static PSI_LD_INSTANCE_TYPE psi_ld_inst = { PSI_LD_INIT_STATE, LSA_NULL, LSA_NULL, };

/* Helper macro to check if a parameter is either PSI_FEATURE_ENABLE or PSI_FEATURE_DISABLE
 *  Usage: if( PSI_CHECK_PARAM_ENABLE_DISABLE (myParam) ) {...} 
 */
#define PSI_CHECK_PARAM_ENABLE_DISABLE(_param) (( PSI_FEATURE_ENABLE != _param ) && ( PSI_FEATURE_DISABLE != _param ))

/* Helper macro to check if a parameter is either PSI_USE_SETTING_YES or PSI_USE_SETTING_NO 
 *  Usage: if( PSI_CHECK_PARAM_USE_SETTINGS_YES_NO (myParam) ) {...} 
 */
#define PSI_CHECK_PARAM_USE_SETTINGS_YES_NO(_param) (( PSI_USE_SETTING_YES != _param ) && ( PSI_USE_SETTING_NO != _param ))

/**
 * Checks the parameters for FeatureSupport
 * Note: This structure is common for all EDD
 * 
 * @param [in] FeatureSupport - Feature support structure
 * @return 0    - All parameters are valid
 * @return > 0  - At least one parameter is invalid. Check the trace.
 */
static LSA_UINT16 psi_ld_check_hd_feature_support_params(
    PSI_HD_FEATURE_INPUT_TYPE const FeatureSupport,
    LSA_UINT16                const hd_id )
{
    LSA_UINT16 error_cnt = 0;

    // parameter "FeatureSupport.fragmentationtype_supported"
    if (   (PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_NO      != FeatureSupport.fragmentationtype_supported)
        && (PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_STATIC  != FeatureSupport.fragmentationtype_supported)
        && (PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_DYNAMIC != FeatureSupport.fragmentationtype_supported)
       )
    {
        error_cnt++;
        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_feature_support_params(): hd_nr(%d): Parameter FeatureSupport.fragmentationtype_supported is invalid (%d)", 
            hd_id, FeatureSupport.fragmentationtype_supported );
    }
    // parameter "FeatureSupport.irt_forwarding_mode_supported"
    if (    (PSI_HD_FEATURE_IRT_FORWARDING_MODE_ABSOLUTE  != FeatureSupport.irt_forwarding_mode_supported) // only relative
         && (PSI_HD_FEATURE_IRT_FORWARDING_MODE_RELATIVE  != FeatureSupport.irt_forwarding_mode_supported) // only absolute
         && ((PSI_HD_FEATURE_IRT_FORWARDING_MODE_ABSOLUTE | PSI_HD_FEATURE_IRT_FORWARDING_MODE_RELATIVE)  != FeatureSupport.irt_forwarding_mode_supported) // both
       )
    {
        error_cnt++;
        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_feature_support_params(): hd_nr(%d): Parameter FeatureSupport.irt_forwarding_mode_supported is invalid (%d)", 
            hd_id, FeatureSupport.irt_forwarding_mode_supported );
    }
    // parameter "FeatureSupport.mrpd_supported"
    if (PSI_CHECK_PARAM_ENABLE_DISABLE(FeatureSupport.mrpd_supported))
    {
        error_cnt++;
        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_feature_support_params(): hd_nr(%d): Parameter FeatureSupport.mrpd_supported is invalid (%d)", 
            hd_id, FeatureSupport.mrpd_supported );
    }
    // parameter "FeatureSupport.short_preamble_supported"
    if (PSI_CHECK_PARAM_ENABLE_DISABLE(FeatureSupport.short_preamble_supported))
    {
        error_cnt++;
        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_feature_support_params(): hd_nr(%d): Parameter FeatureSupport.short_preamble_supported is invalid (%d)", 
            hd_id, FeatureSupport.short_preamble_supported );
    }

    // no checks available for FeatureSupport.max_dfp_frames

    // parameter "FeatureSupport.mrp_interconn_fwd_rules_supported"
    if (PSI_CHECK_PARAM_ENABLE_DISABLE(FeatureSupport.mrp_interconn_fwd_rules_supported))
    {
        error_cnt++;
        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_feature_support_params(): hd_nr(%u): Parameter FeatureSupport.mrp_interconn_fwd_rules_supported is invalid (%u)",
            hd_id, FeatureSupport.mrp_interconn_fwd_rules_supported );
    }
    // parameter "FeatureSupport.mrp_interconn_originator_supported"
    if (PSI_CHECK_PARAM_ENABLE_DISABLE(FeatureSupport.mrp_interconn_originator_supported))
    {
        error_cnt++;
        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_feature_support_params(): hd_nr(%u): Parameter FeatureSupport.mrp_interconn_originator_supported is invalid (%u)",
            hd_id, FeatureSupport.mrp_interconn_originator_supported );
    }

    return error_cnt;
}

/**
 * Checks the parameters for rqb_ptr->args.ld_open.hd_args[] for all configured hds (rqb_ptr->args.ld_open.hd_count)
 * 
 * @param [in] rqb_ptr - pointer to the original RQB
 * @return 0    - All parameters are valid
 * @return > 0  - At least one parameter is invalid. Check the trace.
 */
static LSA_UINT16 psi_ld_check_hd_open_params(
    PSI_RQB_PTR_TYPE const rqb_ptr )
{
    LSA_UINT16              idx;
    LSA_UINT16              idx_port;
    PSI_HD_INPUT_PTR_TYPE   p_hd_arg;
    LSA_UINT16              error_cnt = 0;

    // for all hds
    for (idx = 0; idx < rqb_ptr->args.ld_open.hd_count; idx++)
    {
        PSI_ASSERT(idx < PSI_CFG_MAX_IF_CNT );
        p_hd_arg = &rqb_ptr->args.ld_open.hd_args[idx];

        if ((0 == p_hd_arg->hd_id) || (p_hd_arg->hd_id > PSI_CFG_MAX_IF_CNT))
        {
            error_cnt = PSI_LD_ERROR_CNT_EXIT_CHECK;
            PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): Parameter hd_id is invalid (%u) -> breaking off further cheks", 
                p_hd_arg->hd_id );
            return error_cnt;
        }

        if (   (PSI_HD_RUNS_ON_LEVEL_LD_YES != p_hd_arg->hd_runs_on_level_ld)
            && (PSI_HD_RUNS_ON_LEVEL_LD_NO  != p_hd_arg->hd_runs_on_level_ld))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter hd_runs_on_level_ld is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->hd_runs_on_level_ld );
        }
        // check parameter "additional_forwarding_rules_supported"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->additional_forwarding_rules_supported))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter additional_forwarding_rules_supported is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->additional_forwarding_rules_supported );
        }
        // check parameter "allow_none_pnio_mautypes"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->allow_none_pnio_mautypes))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter allow_none_pnio_mautypes is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->allow_none_pnio_mautypes );
        }
        // check parameter "allow_overlapping_subnet"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->allow_overlapping_subnet))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter allow_overlapping_subnet is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->allow_overlapping_subnet );
        }
        // check parameter "application_exist"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->application_exist))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter application_exist is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->application_exist );
        }
        // check parameter "cluster_IP_support"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->cluster_IP_support))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter cluster_IP_support is invalid (%d)",
                p_hd_arg->hd_id, p_hd_arg->cluster_IP_support);
        }
        // check parameter "fill_active"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->fill_active))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter fill_active is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->fill_active );
        }
        // check parameter "io_configured"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->io_configured))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter io_configured is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->io_configured );
        }
        // if "io_configured" enable, RT (provider or consumer) must be present
        if (PSI_FEATURE_ENABLE == p_hd_arg->io_configured)
        {
            if ((PSI_FEATURE_DISABLE == p_hd_arg->pnio_if.ioc.use_setting) && (PSI_FEATURE_DISABLE == p_hd_arg->pnio_if.iod.use_setting))
            {
                error_cnt++;
                PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of io_configured(%d) and [ioc.use_setting(%d) and iod.use_setting(%d)] is not allowed", 
                    p_hd_arg->hd_id, p_hd_arg->io_configured, p_hd_arg->pnio_if.ioc.use_setting, p_hd_arg->pnio_if.iod.use_setting );
            }
        }
        // if "io_configured" disable, RT (provider and consumer) is not allowed
        if (PSI_FEATURE_DISABLE == p_hd_arg->io_configured)
        {
            if ((PSI_FEATURE_ENABLE == p_hd_arg->pnio_if.ioc.use_setting) || (PSI_FEATURE_ENABLE == p_hd_arg->pnio_if.iod.use_setting))
            {
                error_cnt++;
                PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of io_configured(%d) and [ioc.use_setting(%d) or iod.use_setting(%d)] is not allowed", 
                    p_hd_arg->hd_id, p_hd_arg->io_configured, p_hd_arg->pnio_if.ioc.use_setting, p_hd_arg->pnio_if.iod.use_setting );
            }
        }
        // check parameter "mra_enable_legacy_mode"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->mra_enable_legacy_mode))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter mra_enable_legacy_mode is invalid (%d)", 
                p_hd_arg->hd_id, p_hd_arg->mra_enable_legacy_mode );
        }

        // edd specific parameters
        switch (p_hd_arg->edd_type)
        {
            case LSA_COMP_ID_EDDI:
            {
                if (PSI_CHECK_PARAM_USE_SETTINGS_YES_NO(p_hd_arg->irte.use_setting))
                {
                    error_cnt++;
                    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter irte.use_setting is invalid (%d)", 
                        p_hd_arg->hd_id, p_hd_arg->irte.use_setting );
                }
                if (PSI_USE_SETTING_YES == p_hd_arg->irte.use_setting)
                {
                    // check parameter "irte.disable_max_port_delay_check"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->irte.disable_max_port_delay_check))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter irte.disable_max_port_delay_check is invalid (%d)", 
                            p_hd_arg->hd_id, p_hd_arg->irte.disable_max_port_delay_check );
                    }
                    // check parameter "irte.irt_supported"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->irte.irt_supported))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter irte.irt_supported is invalid (%d)", 
                            p_hd_arg->hd_id, p_hd_arg->irte.irt_supported);
                    }
                    // check parameter "irte.support_irt_flex"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->irte.support_irt_flex))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter irte.support_irt_flex is invalid (%d)", 
                            p_hd_arg->hd_id, p_hd_arg->irte.support_irt_flex );
                    }
                    // no checks available for p_hd_arg->irte.nr_irt_forwarder
                    
                    // parameter "irte.buffer_capacity_use_case"
                    if (   ( PSI_IRTE_USE_CASE_DEFAULT      != p_hd_arg->irte.buffer_capacity_use_case)
                        && ( PSI_IRTE_USE_CASE_IOC_SOC1_2P  != p_hd_arg->irte.buffer_capacity_use_case)
                        && ( PSI_IRTE_USE_CASE_IOC_SOC1_3P  != p_hd_arg->irte.buffer_capacity_use_case)
                        && ( PSI_IRTE_USE_CASE_IOD_ERTEC400 != p_hd_arg->irte.buffer_capacity_use_case))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter irte.buffer_capacity_use_case is invalid (%d)", 
                            p_hd_arg->hd_id, p_hd_arg->irte.buffer_capacity_use_case );
                    }
                    // parameter "irte.FeatureSupport.use_settings"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->irte.FeatureSupport.use_settings))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter irte.FeatureSupport.use_settings is invalid (%d)", 
                            p_hd_arg->hd_id, p_hd_arg->irte.FeatureSupport.use_settings );
                    }
                    if (PSI_FEATURE_ENABLE == p_hd_arg->irte.FeatureSupport.use_settings)
                    {
                        // check all parameters in FeatureSupport
                        error_cnt = error_cnt + psi_ld_check_hd_feature_support_params(p_hd_arg->irte.FeatureSupport, p_hd_arg->hd_id);
                    }
                }
                break;
            }

            case LSA_COMP_ID_EDDS:
            {
                // parameter "stdmac.use_setting"
                if (PSI_CHECK_PARAM_USE_SETTINGS_YES_NO(p_hd_arg->stdmac.use_setting))
                {
                    error_cnt++;
                    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter stdmac.use_setting is invalid (%d)", 
                        p_hd_arg->hd_id, p_hd_arg->stdmac.use_setting );
                }
                if (PSI_USE_SETTING_YES == p_hd_arg->stdmac.use_setting)
                {
                    // parameter "stdmac.nrt_copy_interface"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->stdmac.nrt_copy_interface))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter stdmac.nrt_copy_interface is invalid (%d)",
                            p_hd_arg->hd_id, p_hd_arg->stdmac.nrt_copy_interface );
                    }
                    // parameter "stdmac.gigabit_support"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->stdmac.gigabit_support))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter stdmac.gigabit_support is invalid (%d)",
                            p_hd_arg->hd_id, p_hd_arg->stdmac.gigabit_support );
                    }
                    // if "stdmac.gigabit_support" enable, RT (IO/provider/consumer) is not allowed
                    if (PSI_FEATURE_ENABLE == p_hd_arg->stdmac.gigabit_support)
                    {
                        if (PSI_FEATURE_ENABLE == p_hd_arg->io_configured)
                        {
                            error_cnt++;
                            PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of stdmac.gigabit_support(%d) and io_configured(%d) is not allowed", 
                                p_hd_arg->hd_id, p_hd_arg->stdmac.gigabit_support, p_hd_arg->io_configured );
                        }
                        if (PSI_FEATURE_ENABLE == p_hd_arg->pnio_if.ioc.use_setting)
                        {
                            error_cnt++;
                            PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of stdmac.gigabit_support(%d) and ioc.use_setting(%d) is not allowed", 
                                p_hd_arg->hd_id, p_hd_arg->stdmac.gigabit_support, p_hd_arg->pnio_if.ioc.use_setting );
                        }
                        if (PSI_FEATURE_ENABLE == p_hd_arg->pnio_if.iod.use_setting)
                        {
                            error_cnt++;
                            PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of stdmac.gigabit_support(%d) and iod.use_setting(%d) is not allowed", 
                                p_hd_arg->hd_id, p_hd_arg->stdmac.gigabit_support, p_hd_arg->pnio_if.iod.use_setting );
                        }
                    }
                    // parameter "stdmac.FeatureSupport.use_settings"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->stdmac.FeatureSupport.use_settings))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter stdmac.FeatureSupport.use_settings is invalid (%d)", 
                            p_hd_arg->hd_id, p_hd_arg->stdmac.FeatureSupport.use_settings );
                    }  
                    if (PSI_FEATURE_ENABLE == p_hd_arg->stdmac.FeatureSupport.use_settings)
                    {
                        // check all parameters in FeatureSupport
                        error_cnt = error_cnt + psi_ld_check_hd_feature_support_params(p_hd_arg->stdmac.FeatureSupport, p_hd_arg->hd_id);
                    }
                }

                #if (PSI_CFG_USE_IOH == 1)
                // EDDS is not allow to run with separeted LD and HD: IOH (LD) and EDDS (HD) would create access violation on crt buffers, due to not working locking mechanism
                if (PSI_HD_RUNS_ON_LEVEL_LD_NO == p_hd_arg->hd_runs_on_level_ld)
                {
                    error_cnt++;
                    PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): EDDS is only allowed to run with hd_runs_on_level_ld (%d) == PSI_HD_RUNS_ON_LEVEL_LD_YES",
                        p_hd_arg->hd_id, p_hd_arg->hd_runs_on_level_ld);
                }
                #endif

                break;
            }

            case LSA_COMP_ID_EDDP:
            {
                // parameter "pnip.use_setting"
                if (PSI_CHECK_PARAM_USE_SETTINGS_YES_NO(p_hd_arg->pnip.use_setting))
                {
                    error_cnt++;
                    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter pnip.use_setting is invalid (%d)", 
                        p_hd_arg->hd_id, p_hd_arg->pnip.use_setting );
                }
                if (PSI_USE_SETTING_YES == p_hd_arg->pnip.use_setting)
                {
                    // parameter "pnip.k32fw_trace_level"
                    if (p_hd_arg->pnip.k32fw_trace_level > PNTRC_TRACE_LEVEL_CHAT)
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter pnip.k32fw_trace_level is invalid (%d)",
                            p_hd_arg->hd_id, p_hd_arg->pnip.k32fw_trace_level );
                    }
                    // parameter "pnip.gigabit_support"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->pnip.gigabit_support))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter pnip.FeatureSupport.use_settings is invalid (%d)",
                            p_hd_arg->hd_id, p_hd_arg->pnip.FeatureSupport.use_settings );
                    }
                    // if "pnip.gigabit_support" enable, RT (IO/provider/consumer) is not allowed
                    if (PSI_FEATURE_ENABLE == p_hd_arg->pnip.gigabit_support)
                    {
                        if (PSI_FEATURE_ENABLE == p_hd_arg->io_configured)
                        {
                            error_cnt++;
                            PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of pnip.gigabit_support(%d) and io_configured(%d) is not allowed", 
                                p_hd_arg->hd_id, p_hd_arg->pnip.gigabit_support, p_hd_arg->io_configured );
                        }
                        if (PSI_FEATURE_ENABLE == p_hd_arg->pnio_if.ioc.use_setting)
                        {
                            error_cnt++;
                            PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of pnip.gigabit_support(%d) and ioc.use_setting(%d) is not allowed", 
                                p_hd_arg->hd_id, p_hd_arg->pnip.gigabit_support, p_hd_arg->pnio_if.ioc.use_setting );
                        }
                        if (PSI_FEATURE_ENABLE == p_hd_arg->pnio_if.iod.use_setting)
                        {
                            error_cnt++;
                            PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): combination of pnip.gigabit_support(%d) and iod.use_setting(%d) is not allowed", 
                                p_hd_arg->hd_id, p_hd_arg->pnip.gigabit_support, p_hd_arg->pnio_if.iod.use_setting );
                        }
                    }
                    // parameter "pnip.FeatureSupport.use_settings"
                    if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_hd_arg->pnip.FeatureSupport.use_settings))
                    {
                        error_cnt++;
                        PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter pnip.FeatureSupport.use_settings is invalid (%d)",
                            p_hd_arg->hd_id, p_hd_arg->pnip.FeatureSupport.use_settings );
                    }  
                    if (PSI_FEATURE_ENABLE == p_hd_arg->pnip.FeatureSupport.use_settings)
                    {
                        // check all parameters in FeatureSupport
                        error_cnt = error_cnt + psi_ld_check_hd_feature_support_params(p_hd_arg->pnip.FeatureSupport, p_hd_arg->hd_id);
                    }
                }
                break;
            }

            default: // edd_type unknown
            {
                error_cnt++;
                PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter edd_type is invalid (%d)",
                    p_hd_arg->hd_id, p_hd_arg->edd_type );
            }
        }

        // parameter "hsync_role"
        if (   ( PSI_HSYNC_ROLE_NONE         != p_hd_arg->hsync_role)
            && ( PSI_HSYNC_ROLE_FORWARDER    != p_hd_arg->hsync_role)
            && ( PSI_HSYNC_ROLE_APPL_SUPPORT != p_hd_arg->hsync_role))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter hsync_role is invalid (%d)",
                p_hd_arg->hd_id, p_hd_arg->hsync_role );
        }

        // Startup SendClockFactor (=CycleBaseFactor) invalid? valid = 32,64,128,256,512,1024 (=1ms,2ms,4ms,8ms,16ms,32ms Cylce-Time)
        if (   ((LSA_UINT16)32   != p_hd_arg->send_clock_factor)
            && ((LSA_UINT16)64   != p_hd_arg->send_clock_factor)
            && ((LSA_UINT16)128  != p_hd_arg->send_clock_factor)
            && ((LSA_UINT16)256  != p_hd_arg->send_clock_factor)
            && ((LSA_UINT16)512  != p_hd_arg->send_clock_factor)
            && ((LSA_UINT16)1024 != p_hd_arg->send_clock_factor))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter send_clock_factor is invalid (%d)",
                p_hd_arg->hd_id, p_hd_arg->send_clock_factor );
        }

        // check params in p_hd_arg->hw_port
        for (idx_port = 0; idx_port < PSI_CFG_MAX_PORT_CNT; idx_port++)
        {
            // No checks available for p_hd_arg->hw_port[idx_port].user_nr
            if (   (PSI_NO_RING_PORT      != p_hd_arg->hw_port[idx_port].mrp_ringport_type)
                && (PSI_RING_PORT         != p_hd_arg->hw_port[idx_port].mrp_ringport_type)
                && (PSI_RING_PORT_DEFAULT != p_hd_arg->hw_port[idx_port].mrp_ringport_type))
            {
                error_cnt++;
                PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter hw_port[%d].mrp_ringport_type is invalid (%d)",
                    p_hd_arg->hd_id, idx_port, p_hd_arg->hw_port[idx_port].mrp_ringport_type );
            }
        }
        
        // No checks available for p_hd_arg->port_mac[0]
        
        // parameter "p_hd_arg->hd_location.hd_selection"
        if (   (PSI_HD_SELECT_WITH_LOCATION != p_hd_arg->hd_location.hd_selection)
            && (PSI_HD_SELECT_WITH_MAC      != p_hd_arg->hd_location.hd_selection)
			&& (PSI_HD_SELECT_EMBEDDED		!= p_hd_arg->hd_location.hd_selection))
        {
            error_cnt++;
            PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter hd_location.hd_selection is invalid (%d)",
                p_hd_arg->hd_id, p_hd_arg->hd_location.hd_selection );
        }
        
        switch (p_hd_arg->asic_type)
        {
            case PSI_ASIC_TYPE_ERTEC200:
            case PSI_ASIC_TYPE_ERTEC400:
            case PSI_ASIC_TYPE_SOC1:
            {
                if (LSA_COMP_ID_EDDI != p_hd_arg->edd_type)
                {
                    error_cnt++;
                    PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) does not match with edd_type(%d)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type, p_hd_arg->edd_type );
                }

                #if (PSI_CFG_USE_IOH == 1)
                // Check if the HD environment is set to another EDDI config.
                // - Because IOH (in LD) is using eddx API for buffer handling or SRD and they must than be compiled like HD is.
                #ifdef PSI_CFG_EDDI_CFG_SOC
                if (   (p_hd_arg->asic_type == PSI_ASIC_TYPE_ERTEC400)
                    || (p_hd_arg->asic_type == PSI_ASIC_TYPE_ERTEC200))
                {
                    // has to be SOC1
                    error_cnt++;
                    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) does not match with compiled eddi config (PSI_CFG_EDDI_CFG_SOC)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type );
                }
                #endif

                #ifdef PSI_CFG_EDDI_CFG_ERTEC_400
                if (   (p_hd_arg->asic_type == PSI_ASIC_TYPE_SOC1)
                    || (p_hd_arg->asic_type == PSI_ASIC_TYPE_ERTEC200))
                {
                    // has to be ERTEC400
                    error_cnt++;
                    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) does not match with compiled eddi config (PSI_CFG_EDDI_CFG_ERTEC_400)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type );
                }
                #endif 

                #ifdef PSI_CFG_EDDI_CFG_ERTEC_200
                if (   (p_hd_arg->asic_type == PSI_ASIC_TYPE_SOC1)
                    || (p_hd_arg->asic_type == PSI_ASIC_TYPE_ERTEC400))
                {
                    // has to be ERTEC200
                    error_cnt++;
                    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) does not match with compiled eddi config (PSI_CFG_EDDI_CFG_ERTEC_200)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type );
                }
                #endif
                #endif

                break;
            }

            case PSI_ASIC_TYPE_FPGA_ERTEC200P:
            case PSI_ASIC_TYPE_ERTEC200P:
            case PSI_ASIC_TYPE_HERA:
            {
                if (LSA_COMP_ID_EDDP != p_hd_arg->edd_type)
                {
                    error_cnt++;
                    PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) does not match with edd_type(%d)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type, p_hd_arg->edd_type );
                }
                // "pnip.gigabit_support" enable is feature of HERA
                if ((PSI_FEATURE_ENABLE == p_hd_arg->pnip.gigabit_support) && (p_hd_arg->asic_type != PSI_ASIC_TYPE_HERA))
                {
                    error_cnt++;
                    PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%d): Parameter pnip.gigabit_support(%d) is for asic_type(%d) not allowed, only for PNIP-HERA", 
                        p_hd_arg->hd_id, p_hd_arg->pnip.gigabit_support, p_hd_arg->asic_type );
                }
                break;
            }

            case PSI_ASIC_TYPE_INTEL_SPRINGVILLE:
            case PSI_ASIC_TYPE_INTEL_HARTWELL:
            case PSI_ASIC_TYPE_KSZ88XX:
            case PSI_ASIC_TYPE_TI_AM5728:
            {
                if (LSA_COMP_ID_EDDS != p_hd_arg->edd_type)
                {
                    error_cnt++;
                    PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) does not match with edd_type(%d)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type, p_hd_arg->edd_type );
                }
                // TI-GMAC (TI + EDDS) is not supported
                if ((p_hd_arg->asic_type == PSI_ASIC_TYPE_TI_AM5728) && (p_hd_arg->edd_type == LSA_COMP_ID_EDDS))
                {
                    error_cnt++;
                    PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): TI-GMAC is not supported -> asic_type(%d) and edd_type(%d)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type, p_hd_arg->edd_type );
                }
                break;
            }

            case PSI_ASIC_TYPE_UNKNOWN: /* ASIC type may be unknown in Packet32 variants --> But then edd_type must be EDDS*/
            {
                if (LSA_COMP_ID_EDDS != p_hd_arg->edd_type)
                {
                    error_cnt++;
                    PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Unknown asic_type(%d) is only valid for EDDS, edd_type (%d)",
                        p_hd_arg->hd_id, p_hd_arg->asic_type, p_hd_arg->edd_type );
                }
                break;
            }

            default:
            {
                error_cnt++;
                PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_hd_open_params(): hd_nr(%u): Parameter asic_type(%d) is invalid", 
                    p_hd_arg->hd_id, p_hd_arg->asic_type );
            }
        }
        // no checks available for p_hd_arg->rev_nr
    }

    return error_cnt;
}

static LSA_UINT16 psi_ld_check_ld_open_sock_params(
    PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE sock_ch_details[],
    LSA_UINT16                        channel_count )
{
    LSA_UINT16  error_cnt = 0;
    LSA_UINT16  idx_channel;

    // No checks available for parameter p_socket_channel_details.sock_detail.linger_time
    // No checks available for parameter p_socket_channel_details.sock_detail.rec_buffer_max_len
    // No checks available for parameter p_socket_channel_details.sock_detail.send_buffer_max_len

    for (idx_channel = 0; idx_channel < channel_count; idx_channel++)
    {
        PSI_SOCK_APP_CH_DETAIL_INPUT_PTR_TYPE const p_socket_channel_details = &sock_ch_details[idx_channel];

        // check parameter "sock_close_at_once"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.sock_close_at_once))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d) Parameter sock_close_at_once is invalid (%d)",
                idx_channel, ((LSA_UINT16)(LSA_INT16)p_socket_channel_details->sock_detail.sock_close_at_once)); /* lint warning -e571: Suspicious Cast */
        }
        // check parameter "SO_BROADCAST_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_BROADCAST_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_BROADCAST_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_BROADCAST_);
        }
        // check parameter "SO_DONTROUTE_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_DONTROUTE_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_DONTROUTE_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_DONTROUTE_);
        }
        // check parameter "SO_LINGER_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_LINGER_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_LINGER_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_LINGER_);
        }
        // check parameter "SO_RCVBUF_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_RCVBUF_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_RCVBUF_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_RCVBUF_);
        }
        // check parameter "SO_REUSEADDR_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_REUSEADDR_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_REUSEADDR_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_REUSEADDR_);
        }
        // check parameter "SO_SNDBUF_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_SNDBUF_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_SNDBUF_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_SNDBUF_);
        }
        // check parameter "SO_TCPNOACKDELAY_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_TCPNOACKDELAY_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_TCPNOACKDELAY_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_TCPNOACKDELAY_);
        }
        // check parameter "SO_TCPNODELAY_"
        if (PSI_CHECK_PARAM_ENABLE_DISABLE(p_socket_channel_details->sock_detail.socket_option.SO_TCPNODELAY_))
        {
            error_cnt++;
            PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_sock_params(): socket channel(%d): Parameter SO_TCPNODELAY_ is invalid (%d)",
                idx_channel, p_socket_channel_details->sock_detail.socket_option.SO_TCPNODELAY_);
        }
    }

    return error_cnt;
}

/**
 * Checks the parameters for rqb_ptr->args.ld_open for the LD and all configured HDs.
 * If at least one error is in the input parameters, the response code for the RQB 
 * 
 * @param [in] rqb_ptr - pointer to the original RQB
 * @return 0    - All parameters are valid
 * @return > 0  - At least one parameter is invalid. Check the trace.
 */
static LSA_UINT16 psi_ld_check_ld_open_params(
    PSI_RQB_PTR_TYPE const rqb_ptr )
{
    PSI_UPPER_LD_OPEN_PTR_TYPE const p_open = &rqb_ptr->args.ld_open;
    LSA_UINT16                       error_cnt = 0, error_cnt_2;

    // check parameter "ld_runs_on"
    if (   (PSI_LD_RUNS_ON_LIGHT    != p_open->ld_in.ld_runs_on)
        && (PSI_LD_RUNS_ON_ADVANCED != p_open->ld_in.ld_runs_on)
        && (PSI_LD_RUNS_ON_BASIC    != p_open->ld_in.ld_runs_on))
    {
        error_cnt++;
        PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_params(): Parameter ld_runs_on is invalid (%d) ", p_open->ld_in.ld_runs_on );
    }

    // check parameter "hd_count"
    if ((0 == p_open->hd_count) || (p_open->hd_count > PSI_CFG_MAX_IF_CNT))
    {
        error_cnt = PSI_LD_ERROR_CNT_EXIT_CHECK;
        PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_check_ld_open_params(): Parameter hd_count is invalid (%d) -> breaking off further cheks", 
            p_open->hd_count );
        return error_cnt;
    }

    // No checks available for parameters in structure "p_open->im_args"

    // check parameters in structure sock_args
    error_cnt_2 = psi_ld_check_ld_open_sock_params(p_open->sock_args.sock_app_ch_details, PSI_CFG_MAX_SOCK_APP_CHANNELS);
    error_cnt += error_cnt_2;
    error_cnt_2 = psi_ld_check_ld_open_sock_params(p_open->sock_args.sock_sockapp_ch_details, PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS);
    error_cnt += error_cnt_2;

    error_cnt_2 = psi_ld_check_hd_open_params(rqb_ptr);

    if (error_cnt_2 == PSI_LD_ERROR_CNT_EXIT_CHECK)
        error_cnt = error_cnt_2;
    else
        error_cnt += error_cnt_2;

    return error_cnt;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_ld_init_channel_fct(
    PSI_LD_CHANNEL_FCT_PTR_TYPE pFunc )
{
	LSA_UINT16 idx;

	for (idx = 0; idx < PSI_SYS_PATH_MAX; idx++)
	{
		pFunc[idx].fct_open  = PSI_FCT_PTR_NULL;
		pFunc[idx].fct_close = PSI_FCT_PTR_NULL;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_ld_init_global_channel_fct(
    PSI_LD_CHANNEL_FCT_PTR_TYPE pFunc )
{
	PSI_ASSERT(pFunc != 0);

	PSI_LD_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_global_channel_fct(): Setup global PSI LD system channel list" );

	// Initialize first
	psi_ld_init_channel_fct(pFunc);

	// Prepare the global SYS channels based on compile keys
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_global_channel_fct(): set CLRPC entry(PSI_PATH_GLO_SYS_CLRPC_SOCK), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_CLRPC_SOCK );

	pFunc[PSI_PATH_GLO_SYS_CLRPC_SOCK].fct_open  = psi_clrpc_open_channel;
	pFunc[PSI_PATH_GLO_SYS_CLRPC_SOCK].fct_close = psi_clrpc_close_channel;
	/*----------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_global_channel_fct(): set OHA entry(PSI_PATH_GLO_SYS_OHA_SOCK), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_OHA_SOCK );

	pFunc[PSI_PATH_GLO_SYS_OHA_SOCK].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_GLO_SYS_OHA_SOCK].fct_close = psi_oha_close_channel;
	/*----------------------------------------------------------------------------*/

    #if (PSI_CFG_USE_DNS == 1)
    /*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_global_channel_fct(): set DNS entry(PSI_PATH_GLO_SYS_OHA_DNS), SYS-PATH(%u)",
		PSI_PATH_GLO_SYS_OHA_DNS );

    pFunc[PSI_PATH_GLO_SYS_OHA_DNS].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_GLO_SYS_OHA_DNS].fct_close = psi_oha_close_channel;
    #endif  // DNS global

	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_global_channel_fct(): set SNMPX entry(PSI_PATH_GLO_SYS_SNMPX_SOCK), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_SNMPX_SOCK );

	pFunc[PSI_PATH_GLO_SYS_SNMPX_SOCK].fct_open  = psi_snmpx_open_channel;
	pFunc[PSI_PATH_GLO_SYS_SNMPX_SOCK].fct_close = psi_snmpx_close_channel;

    #if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "set SNMPX entry(PSI_PATH_GLO_SYS_SNMPX_OHA), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_SNMPX_OHA );

	pFunc[PSI_PATH_GLO_SYS_SNMPX_OHA].fct_open  = psi_snmpx_open_channel;
	pFunc[PSI_PATH_GLO_SYS_SNMPX_OHA].fct_close = psi_snmpx_close_channel;
    #endif  // (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
    /*----------------------------------------------------------------------------*/

    #if (PSI_CFG_USE_SOCKAPP == 1)
    /*----------------------------------------------------------------------------*/
    PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set SOCKAPP entry(PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01), SYS-PATH(%u)",
        PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01);

    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01].fct_open = psi_sockapp_open_channel;
    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01].fct_close = psi_sockapp_close_channel;

    /*----------------------------------------------------------------------------*/
    PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set SOCKAPP entry(PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02), SYS-PATH(%u)",
        PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02);

    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02].fct_open = psi_sockapp_open_channel;
    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02].fct_close = psi_sockapp_close_channel;

    /*----------------------------------------------------------------------------*/
    PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set SOCKAPP entry(PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03), SYS-PATH(%u)",
        PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03);

    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03].fct_open = psi_sockapp_open_channel;
    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03].fct_close = psi_sockapp_close_channel;

    /*----------------------------------------------------------------------------*/
    PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set SOCKAPP entry(PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04), SYS-PATH(%u)",
        PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04);

    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04].fct_open = psi_sockapp_open_channel;
    pFunc[PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04].fct_close = psi_sockapp_close_channel;
    /*----------------------------------------------------------------------------*/
    #endif //SOCKAPP
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_ld_init_if_channel_fct(
    PSI_LD_CHANNEL_FCT_PTR_TYPE pFunc )
{
	PSI_ASSERT(pFunc != 0);

	// Initialize first
	psi_ld_init_channel_fct(pFunc);

	// Prepare the IF specific SYS channels based on compile Keys
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set ACP entry(PSI_PATH_IF_SYS_ACP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_ACP_EDD );

	pFunc[PSI_PATH_IF_SYS_ACP_EDD].fct_open   = psi_acp_open_channel;
	pFunc[PSI_PATH_IF_SYS_ACP_EDD].fct_close  = psi_acp_close_channel;
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_EDD );

	pFunc[PSI_PATH_IF_SYS_CM_EDD].fct_open   = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_EDD].fct_close  = psi_cm_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_CLRPC), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_CLRPC );

	pFunc[PSI_PATH_IF_SYS_CM_CLRPC].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_CLRPC].fct_close = psi_cm_close_channel;

    #if (PSI_CFG_USE_MRP == 1)
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_MRP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_MRP );

	pFunc[PSI_PATH_IF_SYS_CM_MRP].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_MRP].fct_close = psi_cm_close_channel;
    #endif

    #if (PSI_CFG_USE_POF == 1)
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_POF_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_POF_EDD );

	pFunc[PSI_PATH_IF_SYS_CM_POF_EDD].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_POF_EDD].fct_close = psi_cm_close_channel;
    #endif

    #if (PSI_CFG_USE_GSY == 1)
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_GSY), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_GSY );

	pFunc[PSI_PATH_IF_SYS_CM_GSY].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_GSY].fct_close = psi_cm_close_channel;
    #endif

    PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_NARE), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_NARE );

	pFunc[PSI_PATH_IF_SYS_CM_NARE].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_NARE].fct_close = psi_cm_close_channel;
    
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_OHA), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_OHA );

	pFunc[PSI_PATH_IF_SYS_CM_OHA].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_OHA].fct_close = psi_cm_close_channel;

	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set DCP entry(PSI_PATH_IF_SYS_DCP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_DCP_EDD );

	pFunc[PSI_PATH_IF_SYS_DCP_EDD].fct_open  = psi_dcp_open_channel;
	pFunc[PSI_PATH_IF_SYS_DCP_EDD].fct_close = psi_dcp_close_channel;
	/*----------------------------------------------------------------------------*/

    #if (PSI_CFG_USE_GSY == 1)
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set GSY entry(PSI_PATH_IF_SYS_GSY_EDD_SYNC), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_GSY_EDD_SYNC );

	pFunc[PSI_PATH_IF_SYS_GSY_EDD_SYNC].fct_open   = psi_gsy_open_channel;
	pFunc[PSI_PATH_IF_SYS_GSY_EDD_SYNC].fct_close  = psi_gsy_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set GSY entry(PSI_PATH_IF_SYS_GSY_EDD_ANNO), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_GSY_EDD_ANNO );

	pFunc[PSI_PATH_IF_SYS_GSY_EDD_ANNO].fct_open   = psi_gsy_open_channel;
	pFunc[PSI_PATH_IF_SYS_GSY_EDD_ANNO].fct_close  = psi_gsy_close_channel;
	/*----------------------------------------------------------------------------*/
    #endif // GSY

    #if (PSI_CFG_USE_MRP == 1)
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set MRP entry(PSI_PATH_IF_SYS_MRP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_MRP_EDD );

	pFunc[PSI_PATH_IF_SYS_MRP_EDD].fct_open   = psi_mrp_open_channel;
	pFunc[PSI_PATH_IF_SYS_MRP_EDD].fct_close  = psi_mrp_close_channel;
	/*----------------------------------------------------------------------------*/
    #endif // MRP

	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set NARE entry(PSI_PATH_IF_SYS_NARE_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_NARE_EDD );

	pFunc[PSI_PATH_IF_SYS_NARE_EDD].fct_open   = psi_nare_open_channel;
	pFunc[PSI_PATH_IF_SYS_NARE_EDD].fct_close  = psi_nare_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set NARE entry(PSI_PATH_IF_SYS_NARE_DCP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_NARE_DCP_EDD );

	pFunc[PSI_PATH_IF_SYS_NARE_DCP_EDD].fct_open   = psi_nare_open_channel;
	pFunc[PSI_PATH_IF_SYS_NARE_DCP_EDD].fct_close  = psi_nare_close_channel;

	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_EDD );

	pFunc[PSI_PATH_IF_SYS_OHA_EDD].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_EDD].fct_close = psi_oha_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_LLDP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_LLDP_EDD );

	pFunc[PSI_PATH_IF_SYS_OHA_LLDP_EDD].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_LLDP_EDD].fct_close = psi_oha_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_DCP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_DCP_EDD );

	pFunc[PSI_PATH_IF_SYS_OHA_DCP_EDD].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_DCP_EDD].fct_close = psi_oha_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_NARE), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_NARE );

	pFunc[PSI_PATH_IF_SYS_OHA_NARE].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_NARE].fct_close = psi_oha_close_channel;

    #if (PSI_CFG_USE_MRP == 1)
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_MRP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_MRP );

	pFunc[PSI_PATH_IF_SYS_OHA_MRP].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_MRP].fct_close = psi_oha_close_channel;
    #endif
	/*----------------------------------------------------------------------------*/

    #if (PSI_CFG_USE_TCIP == 1)
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_ARP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_ARP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ARP].fct_open   = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ARP].fct_close  = psi_tcip_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_ICMP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_ICMP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ICMP].fct_open  = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ICMP].fct_close = psi_tcip_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_UDP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_UDP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_UDP].fct_open   = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_UDP].fct_close  = psi_tcip_close_channel;

	#ifdef PSI_CFG_TCIP_CFG_SUPPORT_PATH_EDD_TCP
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_TCP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_TCP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_TCP].fct_open   = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_TCP].fct_close  = psi_tcip_close_channel;
	#endif
	/*----------------------------------------------------------------------------*/
    #endif // TCIP

	#if (PSI_CFG_USE_HSA == 1)
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "psi_ld_init_if_channel_fct(): set HSA entry(PSI_PATH_IF_SYS_HSA_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_HSA_EDD);

	pFunc[PSI_PATH_IF_SYS_HSA_EDD].fct_open = psi_hsa_open_channel;

	pFunc[PSI_PATH_IF_SYS_HSA_EDD].fct_close = psi_hsa_close_channel;
	/*----------------------------------------------------------------------------*/
    #endif // HSA
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_set_sys_handle(
    LSA_SYS_PATH_TYPE sys_path, 
    LSA_HANDLE_TYPE   handle )
{
	// Register sys channel handle for SYS PATH
	LSA_UINT16          hd_nr       = PSI_SYSPATH_GET_HD(sys_path);
	LSA_SYS_PATH_TYPE   path        = PSI_SYSPATH_GET_PATH(sys_path);
    LSA_UINT16          pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(psi_ld_inst.ld_state == PSI_LD_OPENING);
	PSI_ASSERT(path < PSI_SYS_PATH_MAX);

	// Global channel ?
    if (pnio_if_nr == PSI_GLOBAL_PNIO_IF_NR)
	{
		psi_ld_inst.ld_store_ptr->global_lower_handle[path] = handle;
	}
	else
	{
		PSI_LD_HD_PTR_TYPE hd_ptr;

		PSI_ASSERT((hd_nr >= 1) && (hd_nr <= PSI_CFG_MAX_IF_CNT));

		hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
        hd_ptr->lower_handle[path] = handle;
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_reset_sys_handle(
    LSA_SYS_PATH_TYPE const sys_path )
{
	// Register sys channel handle for SYS PATH
	LSA_UINT16        const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_SYS_PATH_TYPE const path       = PSI_SYSPATH_GET_PATH(sys_path);
    LSA_UINT16        const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(psi_ld_inst.ld_state == PSI_LD_CLOSING);
	PSI_ASSERT(path < PSI_SYS_PATH_MAX);

	// Global channel ?
    if (pnio_if_nr == PSI_GLOBAL_PNIO_IF_NR)
	{
		psi_ld_inst.ld_store_ptr->global_lower_handle[path] = PSI_INVALID_HANDLE;
	}
	else
	{
		PSI_LD_HD_PTR_TYPE      hd_ptr;

		PSI_ASSERT((hd_nr >= 1) && (hd_nr <= PSI_CFG_MAX_IF_CNT));

		hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
        hd_ptr->lower_handle[path] = PSI_INVALID_HANDLE;
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_prepare_req_list(
    PSI_UPPER_LD_OPEN_PTR_TYPE const p_open )
{
	LSA_UINT16                      hd_idx;
	LSA_UINT16                      path;
	LSA_UINT16                      add_idx = 0;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;

	PSI_ASSERT(p_open != LSA_NULL);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	// First clean the list
	PSI_MEMSET( p_hd_req, 0, (sizeof(PSI_LD_HD_REQ_STORE_TYPE)) );
	PSI_MEMSET( p_if_req, 0, (sizeof(PSI_LD_IF_REQ_STORE_TYPE)) );

	// First add global sys channel to IF req list
	for (path = PSI_SYS_PATH_INVALID; path < PSI_SYS_PATH_MAX; path++)
	{
		if (psi_ld_inst.ld_store_ptr->global_fct[path].fct_open != PSI_FCT_PTR_NULL)
		{
			p_if_req->req[add_idx].hd_nr        = 0;
			p_if_req->req[add_idx].pnio_if_nr   = PSI_GLOBAL_PNIO_IF_NR;
			p_if_req->req[add_idx].sys_path     = path;
			add_idx++;
		}
	}

	// Initialize hd req lists
	p_hd_req->nr_of_hd      = (LSA_UINT16)p_open->hd_count;
	p_hd_req->act_req_idx   = 0;

	for (hd_idx = 0; hd_idx < p_open->hd_count; hd_idx++)
	{
		PSI_HD_INPUT_PTR_TYPE const p_hd_in = &p_open->hd_args[hd_idx];

		/* add HDs to HD request list (for open/close HDs) */
		p_hd_req->hd_req[hd_idx].hd_nr                  = p_hd_in->hd_id;
		p_hd_req->hd_req[hd_idx].hd_location            = p_hd_in->hd_location;
		p_hd_req->hd_req[hd_idx].hd_runs_on_level_ld    = p_hd_in->hd_runs_on_level_ld;

		/* add HD-IF sys channels to request list */
		for (path = PSI_SYS_PATH_INVALID; path < PSI_SYS_PATH_MAX; path++)
		{
			if (psi_ld_inst.ld_store_ptr->if_fct[path].fct_open != PSI_FCT_PTR_NULL)
			{
				p_if_req->req[add_idx].hd_nr        = p_hd_in->hd_id;
                p_if_req->req[add_idx].pnio_if_nr   = p_hd_in->pnio_if.edd_if_id;
				p_if_req->req[add_idx].sys_path     = path;
				add_idx++;
			}
		}

		// Add the detail store for this PNIO IF
		// Note: if a duplicate PNIO IF ID is configured, add fails with FATAL
		psi_add_detail_store(p_hd_in->pnio_if.edd_if_id, p_hd_in);
	}

	// complete LD IF list
	p_if_req->nr_of_req     = add_idx;
	p_if_req->act_req_idx   = 0;
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_clean_req_list( LSA_VOID )
{
	LSA_UINT16                      idx;
	LSA_UINT16                      max_if;
	LSA_UINT16                      last_pnio_if_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	// Unregister all PNIO-IF in Detailstore
	max_if = p_if_req->nr_of_req;

	last_pnio_if_nr = 0xFF;

	for (idx = 0; idx < max_if; idx++)
	{
		LSA_UINT16 const pnio_if_nr = p_if_req->req[idx].pnio_if_nr;

		if (pnio_if_nr != last_pnio_if_nr) // already removed ?
		{
			if (pnio_if_nr != PSI_GLOBAL_PNIO_IF_NR)
			{
                /* Undo init all pnio ifs from oha database / opposit to psi_oha_init_db_if() */
				psi_oha_undo_init_db_if(pnio_if_nr);
			}

			// remove entry for PNIO IF
			psi_remove_detail_store(pnio_if_nr);

			last_pnio_if_nr = pnio_if_nr;
		}
	}

	// clean the lists
	PSI_MEMSET(p_hd_req, 0, sizeof(PSI_LD_HD_REQ_STORE_TYPE));
	PSI_MEMSET(p_if_req, 0, sizeof(PSI_LD_IF_REQ_STORE_TYPE));
}

/*---------------------------------------------------------------------------*/
PSI_LD_RUNS_ON_TYPE psi_get_ld_runs_on( LSA_VOID )
{
    #if defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    return PSI_LD_RUNS_ON_BASIC;
    #else
    PSI_ASSERT((psi_ld_inst.ld_runs_on != PSI_LD_RUNS_ON_UNKNOWN) && (psi_ld_inst.ld_runs_on < PSI_LD_RUNS_ON_MAX));
    return psi_ld_inst.ld_runs_on;
    #endif
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_get_lldp_chassis_id(
    LSA_UINT8 * const p_data )
{
    LSA_INT                RetVal_Printf;
	PSI_RQB_PTR_TYPE const p_rqb = psi_ld_inst.ld_upper_rqb;

	PSI_ASSERT(p_rqb != LSA_NULL);

	// prepare the LLDP chassis ID for OHA from LD I&M data and register it

    RetVal_Printf = PSI_SPRINTF( p_data, "%-25.25s %-20.20s %-16.16s %5u %c%3u%3u%3u",
		                         p_rqb->args.ld_open.im_args.device_type,
		                         p_rqb->args.ld_open.im_args.order_id,
                                 p_rqb->args.ld_open.im_args.serial_nr,
		                         p_rqb->args.ld_open.im_args.hw_revision,
		                         p_rqb->args.ld_open.im_args.sw_version.revision_prefix,
		                         p_rqb->args.ld_open.im_args.sw_version.functional_enhancement,
		                         p_rqb->args.ld_open.im_args.sw_version.bug_fix,
		                         p_rqb->args.ld_open.im_args.sw_version.internal_change );
    PSI_ASSERT(RetVal_Printf >= 0);
}

/*---------------------------------------------------------------------------*/
PSI_SOCK_INPUT_PTR_TYPE psi_ld_get_sock_details( LSA_VOID )
{
	return (&psi_ld_inst.ld_sock_details);
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_ld_get_real_pi_size(
    LSA_UINT16 const hd_nr )
{
    LSA_UINT32 pi_size;

    PSI_ASSERT((hd_nr >= 1) && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    pi_size = psi_ld_inst.ld_store_ptr->hd[hd_nr-1].io_mem_size;

    return pi_size;
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_close_next_hd(
    PSI_LD_HD_REQ_STORE_PTR_TYPE const p_hd_req )
{
	PSI_ASSERT(p_hd_req != LSA_NULL);

	// Check if all HIF HD are closed
	p_hd_req->act_req_idx -= 1;

	if (p_hd_req->act_req_idx >= 0) // Next HD ?
	{
		// Close next PSI HD (shutdown) FW system
		psi_ld_close_hd(p_hd_req->act_req_idx);
	}
	else
	{
		// All HDs closed, finish close Request
		PSI_UPPER_CALLBACK_FCT_PTR_TYPE       cbf_fct;
		PSI_RQB_PTR_TYPE                const pRQB = psi_ld_inst.ld_upper_rqb;

        #if (PSI_CFG_USE_HIF_LD == 1)
        PSI_LD_RUNS_ON_TYPE             const ld_runs_on = psi_get_ld_runs_on();
        #endif

        PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_next_hd(): LD Close finished, hd_count(%u)", 
            p_hd_req->nr_of_hd );

        // Reset the request list and the detail storage
		psi_ld_clean_req_list();

        // PN-Stack-Undo-Init: LD components (HD components if they are active in this FW)
        psi_pn_stack_undo_init();
        // EDD-Undo-Init of all EDDs in this FW (if they are active in this FW)
        psi_edd_undo_init();

        // Close fínished --> response to Upper
		psi_ld_inst.ld_upper_rqb = LSA_NULL;
        psi_ld_inst.ld_runs_on   = PSI_LD_RUNS_ON_UNKNOWN;

		cbf_fct = pRQB->args.ld_close.psi_request_upper_done_ptr;

		PSI_RQB_SET_RESPONSE(pRQB, PSI_OK);
		psi_ld_inst.ld_state = PSI_LD_INIT_DONE;  // set state to not open

        #if (PSI_CFG_USE_HIF_LD == 1)
        if ((ld_runs_on == PSI_LD_RUNS_ON_ADVANCED) || (ld_runs_on == PSI_LD_RUNS_ON_BASIC))
        {
		    //  Finish Request using HIF LD
		    psi_system_request_done(PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf_fct, pRQB);
        }
        else if (ld_runs_on != PSI_LD_RUNS_ON_LIGHT )
        {
            PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_close_next_hd(): Invalid ld_runs_on(%u) value!", ld_runs_on );
            PSI_FATAL(0);
        }
        else
        #endif
        {
            // Finish Request direct to PSI
            psi_system_request_done(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)cbf_fct, pRQB);
        }
	}
}

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_init( LSA_VOID )
{
	LSA_UINT16 path;
	LSA_UINT16 hd_idx;

    /* Init the PSI LD instance vars */
	PSI_ASSERT(psi_ld_inst.ld_state == PSI_LD_INIT_STATE);
    PSI_MEMSET(&psi_ld_inst, 0, sizeof(psi_ld_inst));
    psi_ld_inst.ld_state = PSI_LD_INIT_STATE;

	psi_ld_inst.ld_store_ptr = (PSI_LD_STORE_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_LD_STORE_TYPE));
	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);

	psi_ld_inst.ld_upper_rqb = LSA_NULL;

	/* setup the fct list for global and if specific sys channels */
	psi_ld_init_global_channel_fct(psi_ld_inst.ld_store_ptr->global_fct);
	psi_ld_init_if_channel_fct(psi_ld_inst.ld_store_ptr->if_fct);

	/* Init global IF vars */
	for (path = 0; path < PSI_SYS_PATH_MAX; path++)
	{
		psi_ld_inst.ld_store_ptr->global_lower_handle[path] = PSI_INVALID_HANDLE;
	}

	/* Init HD IF vars */
	for (hd_idx = 0; hd_idx < PSI_CFG_MAX_IF_CNT; hd_idx++)
	{
		PSI_LD_HD_PTR_TYPE const hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_idx];

		hd_ptr->hif_lower_handle                = PSI_INVALID_HANDLE;
		hd_ptr->hif_upper_handle                = PSI_INVALID_HANDLE;
		hd_ptr->sys_lower_handle.shortcut_mode  = LSA_FALSE;
		hd_ptr->sys_lower_handle.is_upper       = LSA_FALSE;

		for (path = 0; path < PSI_SYS_PATH_MAX; path++)
		{
            hd_ptr->lower_handle[path] = PSI_INVALID_HANDLE;
		}
	}

	/* Init req. list */
	PSI_MEMSET(&psi_ld_inst.ld_store_ptr->hd_req, 0, sizeof(PSI_LD_HD_REQ_STORE_TYPE));
	PSI_MEMSET(&psi_ld_inst.ld_store_ptr->if_req, 0, sizeof(PSI_LD_IF_REQ_STORE_TYPE));

	psi_ld_inst.ld_state = PSI_LD_INIT_DONE;
}

/*---------------------------------------------------------------------------*/
/* Denitialization                                                           */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_undo_init( LSA_VOID )
{
    #if PSI_DEBUG /*needed for preventing Warnings in release Builds, only Asserts are used here*/
	LSA_UINT16 path;
	LSA_UINT16 hd_idx;

	/* Undo INIT all PSI LD instance vars */
	PSI_ASSERT(psi_ld_inst.ld_state == PSI_LD_INIT_DONE);

	for (path = 0; path < PSI_SYS_PATH_MAX; path++) /* check for closed all globals channels */
	{
		LSA_HANDLE_TYPE const lower_handle = psi_ld_inst.ld_store_ptr->global_lower_handle[path];
		PSI_ASSERT(lower_handle == PSI_INVALID_HANDLE);
	}

	/* cleanup HD IF vars */
	for (hd_idx = 0; hd_idx < PSI_CFG_MAX_IF_CNT; hd_idx++)
	{
		PSI_LD_HD_PTR_TYPE const hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_idx];

		PSI_ASSERT(hd_ptr->hif_lower_handle == PSI_INVALID_HANDLE);
		PSI_ASSERT(hd_ptr->hif_upper_handle == PSI_INVALID_HANDLE);

		for (path = 0; path < PSI_SYS_PATH_MAX; path++)
		{
			PSI_ASSERT(hd_ptr->lower_handle[path] == PSI_INVALID_HANDLE);
		}
	}
    #endif

	/* init the fct list for global and if specific sys channels */
	psi_ld_init_channel_fct(psi_ld_inst.ld_store_ptr->global_fct);
	psi_ld_init_channel_fct(psi_ld_inst.ld_store_ptr->if_fct);

	psi_free_local_mem(psi_ld_inst.ld_store_ptr);

	psi_ld_inst.ld_store_ptr    = LSA_NULL;
	psi_ld_inst.ld_upper_rqb    = LSA_NULL;
	psi_ld_inst.ld_state        = PSI_LD_INIT_STATE;
}

/*---------------------------------------------------------------------------*/
/*  Request interface                                                        */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_device(
    PSI_RQB_PTR_TYPE rqb_ptr )
{
	LSA_INT                          first_idx;
	LSA_UINT16                       error_cnt;
	PSI_UPPER_LD_OPEN_PTR_TYPE const p_open = &(rqb_ptr->args.ld_open);

	PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_device(): LD Open start, ld_runs_on(%u) hd_count(%u)", p_open->ld_in.ld_runs_on, p_open->hd_count );

    error_cnt = psi_ld_check_ld_open_params(rqb_ptr);

    // Check input parameters before processing the RQB.
	if (error_cnt > 0)
	{
        // If there is at least one error in the parameters, the RQB is returned to the user with PSI_ERR_PARAM
	    PSI_UPPER_CALLBACK_FCT_PTR_TYPE     cbf_fct;
	    
	    // There is an error in the input values. 
	    PSI_LD_TRACE_00( 0, LSA_TRACE_LEVEL_ERROR, "psi_ld_open_device(): Input parameters are invalid. Check the trace to see which parameter is invalid. LD open device CANCELLED.");
	    
	    // return the RQB back to the user with the error code	    
	    
	    PSI_RQB_SET_RESPONSE(rqb_ptr, PSI_ERR_PARAM);
	    
        cbf_fct = rqb_ptr->args.ld_open.psi_request_upper_done_ptr;

        if (psi_function_is_null(p_open->psi_request_upper_done_ptr))
        {
            // no callback possible
            PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_open_device(): no CBF defined for LD Open, rqb_ptr(0x%08x)", rqb_ptr );
            PSI_FATAL(0);
        }
        else
        {
            // Give the RQB back to application. Note: Can't use psi_get_ld_runs_on() since the internal parameters are not set if the input values are incorrect.
            #if (PSI_CFG_USE_HIF_LD == 1)
            if ((p_open->ld_in.ld_runs_on == PSI_LD_RUNS_ON_ADVANCED) || (p_open->ld_in.ld_runs_on == PSI_LD_RUNS_ON_BASIC))
            {
                psi_system_request_done(PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf_fct, rqb_ptr);
            }
            else if (p_open->ld_in.ld_runs_on != PSI_LD_RUNS_ON_LIGHT)
            {
                PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_open_device(): Invalid ld_runs_on(%u) value!", psi_ld_inst.ld_runs_on );
                PSI_FATAL(0);
            }
            else
            #endif
            {
                psi_system_request_done(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)cbf_fct, rqb_ptr);
            }
        }
	}
    else
    {
        // no error
        PSI_LD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "psi_ld_open_device(): Input parameters are valid. Proceed to startup the PN Stack.");

        // EDD-Init of all EDDs in this FW (if they are active in this FW)
        psi_edd_init();
        // PN-Stack-Init: LD components (HD components if they are active in this FW)
        psi_pn_stack_init();

        // Start opening a LD device
        psi_ld_inst.ld_upper_rqb = rqb_ptr;
        psi_ld_inst.ld_state = PSI_LD_OPENING;
        psi_ld_inst.ld_runs_on = p_open->ld_in.ld_runs_on;

        // Store Sock details if available
        psi_ld_inst.ld_sock_details = rqb_ptr->args.ld_open.sock_args;

        // delete resource calculation values
        psi_res_calc_delete();

        // Prepare the global IFs (before open all HDs)
        // Note: the HD sets specifics like (hSysDev,...) are not possible on LD side
	    psi_add_detail_store(PSI_GLOBAL_PNIO_IF_NR, 0);

        // Prepare HD request list for HD open
        // Note: the request list is used for HIF HD and PSI HD open/close
	    psi_ld_prepare_req_list(p_open);

        first_idx = (LSA_UINT16)psi_ld_inst.ld_store_ptr->hd_req.act_req_idx;
        PSI_ASSERT(first_idx == 0);

        #if (PSI_CFG_USE_HIF_HD == 1)
        if (psi_get_hd_runs_on_ld(psi_ld_inst.ld_store_ptr->hd_req.hd_req[first_idx].hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
        {
            // First start creating HIF HD lower connection before open the devices
            // Note: check for HIF HD lower open (shortcut mode)
	        psi_ld_open_hif_hd_lower(first_idx);
        }
        else
        #endif
        {
            // Open all HD devices without HIF HD (normal order)
	        psi_ld_open_hd(first_idx);
        }
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_device(
    PSI_RQB_PTR_TYPE rqb_ptr )
{
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;
    PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	PSI_ASSERT(rqb_ptr);

    p_if_req = &psi_ld_inst.ld_store_ptr->if_req;
    p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

    PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_device(): LD Close start, ld_runs_on(%u), hd_count(%u)", psi_ld_inst.ld_runs_on, p_hd_req->nr_of_hd );

	// Start closing a LD device
	psi_ld_inst.ld_upper_rqb = rqb_ptr;
	psi_ld_inst.ld_state     = PSI_LD_CLOSING;

	// start with closing all PSI LD system channels (reverse order)
	p_if_req->act_req_idx = p_if_req->nr_of_req - 1;

	psi_ld_close_channels(p_if_req->act_req_idx);
}

#if (PSI_CFG_USE_HIF_HD == 1)
/*---------------------------------------------------------------------------*/
/* HIF HD open/close request                                                 */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_lower(
    LSA_INT hd_req_idx )
{
	PSI_HD_INPUT_PTR_TYPE           p_hd;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;

	PSI_ASSERT(psi_ld_inst.ld_upper_rqb != LSA_NULL);

	p_hd = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[hd_req_idx];

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT(hd_req_idx == p_hd_req->act_req_idx);

    // HD runs on LD level ?
    if (psi_get_hd_runs_on_ld(p_hd->hd_id) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
	{
        #ifdef HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
        // this HD runs on same systems with LD

        HIF_RQB_PTR_TYPE    pRQB;
		PSI_LD_HD_PTR_TYPE  hd_ptr;

		LSA_UINT16 const hd_nr = p_hd_req->hd_req[hd_req_idx].hd_nr;
		hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

		// Prepare the SysPtr
		// Note: in shortcut mode all location fields are don't care
		PSI_MEMSET( &hd_ptr->sys_lower_handle, 0, (sizeof(PSI_SYS_PTR_TYPE)) );

		hd_ptr->sys_lower_handle.hd_nr                  = hd_nr;
		hd_ptr->sys_lower_handle.hd_sys_id.edd_type     = p_hd->edd_type;
		hd_ptr->sys_lower_handle.hd_sys_id.rev_nr       = p_hd->rev_nr;
		hd_ptr->sys_lower_handle.hd_sys_id.asic_type    = p_hd->asic_type;
		hd_ptr->sys_lower_handle.hd_sys_id.hd_location  = p_hd->hd_location;
		hd_ptr->sys_lower_handle.shortcut_mode          = LSA_TRUE;
		hd_ptr->sys_lower_handle.is_upper               = LSA_FALSE;

		// Open HIF HD lower in shortcut mode
		HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, (sizeof(*pRQB)), LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB );
		PSI_ASSERT(pRQB != LSA_NULL);

		HIF_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
		HIF_RQB_SET_OPCODE(pRQB, HIF_OPC_HD_LOWER_OPEN);
		HIF_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_HIF);

		pRQB->args.dev_lower_open.hSysDev = &hd_ptr->sys_lower_handle;
		pRQB->args.dev_lower_open.hH      = PSI_INVALID_HANDLE; // out from HIF

		pRQB->args.dev_lower_open.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE) psi_ld_open_hif_hd_lower_done;

		PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_hif_hd_lower(): pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
			pRQB, hd_req_idx, p_hd_req->nr_of_hd, p_hd_req->hd_req[hd_req_idx].hd_nr );

		/* post the message to HIF HD */
		psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB);
        #else
		// this HD runs on different systems from LD
		// HIF HD lower open is part of FW system, continue with Open HIF HD Upper
		// Note: In this case the driver connection is done during HIF_HD_U_GET_PARAMS
		psi_ld_open_hif_hd_upper(hd_req_idx);
		#endif
	}
    else
    {
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_open_hif_hd_lower(): hd_runs_on_level_ld(%u) is invalid",
            p_hd->hd_runs_on_level_ld);
		PSI_FATAL( 0 );
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_lower_done(
    LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                         rc;
	LSA_UINT16                         hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE       p_hd_req;
	PSI_LD_HD_PTR_TYPE                 hd_ptr;
	HIF_RQB_PTR_TYPE             const pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT(pRQB != 0 );
	PSI_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
	PSI_ASSERT(HIF_RQB_GET_OPCODE(pRQB) == HIF_OPC_HD_LOWER_OPEN);

	p_hd_req    = &psi_ld_inst.ld_store_ptr->hd_req;
	hd_nr       = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr      = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_hif_hd_lower_done(): rsp(%u/%#x) pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		(PSI_RQB_GET_RESPONSE(pRQB)),
		(PSI_RQB_GET_RESPONSE(pRQB)),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

    hd_ptr->hif_lower_handle = pRQB->args.dev_lower_open.hH;

	HIF_FREE_LOCAL_MEM(&rc, pRQB, LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB);
	PSI_ASSERT(rc == LSA_RET_OK);

	// Continue with HIF HD Upper Open
	psi_ld_open_hif_hd_upper(p_hd_req->act_req_idx);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_upper(
    LSA_INT hd_req_idx )
{
	LSA_UINT16                      hd_nr;
	HIF_RQB_PTR_TYPE                pRQB;
	PSI_HD_INPUT_PTR_TYPE           p_hd;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;
	PSI_LD_HD_PTR_TYPE              hd_ptr;

	/* Create the HIF HD RQB and copy the HD section from LD RQB */
	HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, (sizeof(*pRQB)), LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB );
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_ASSERT(psi_ld_inst.ld_upper_rqb != LSA_NULL);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT(hd_req_idx == p_hd_req->act_req_idx);

	HIF_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
	HIF_RQB_SET_OPCODE(pRQB, HIF_OPC_HD_UPPER_OPEN);
	HIF_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_HIF);

	hd_nr   = p_hd_req->hd_req[hd_req_idx].hd_nr;
	hd_ptr  = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
	p_hd    = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[hd_nr-1];

	// Prepare the SysPtr
	// Note: the location information for HD is important in none shortcut mode
	PSI_MEMSET( &hd_ptr->sys_upper_handle, 0, (sizeof(PSI_SYS_PTR_TYPE)) );

	hd_ptr->sys_upper_handle.hd_nr                  = hd_nr;
	hd_ptr->sys_upper_handle.hd_sys_id.edd_type     = p_hd->edd_type;
	hd_ptr->sys_upper_handle.hd_sys_id.rev_nr       = p_hd->rev_nr;
	hd_ptr->sys_upper_handle.hd_sys_id.asic_type    = p_hd->asic_type;
	hd_ptr->sys_upper_handle.hd_sys_id.hd_location  = p_hd->hd_location;
	hd_ptr->sys_upper_handle.is_upper               = LSA_TRUE;

    // HD runs on LD level ?
    if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_YES)
	{
        // this HD runs on same systems with LD
		// open HIF HD upper in shortcut mode
		// Note: all location params are don't care 
		hd_ptr->sys_upper_handle.shortcut_mode      = LSA_TRUE;
		hd_ptr->sys_upper_handle.hif_lower_handle   = hd_ptr->hif_lower_handle;
	}
	else if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
	{
        // this HD runs on different systems from LD
		// open HIF HD upper
		// Note: the location params are used for real open the HD
		hd_ptr->sys_upper_handle.shortcut_mode      = LSA_FALSE;
		hd_ptr->sys_upper_handle.hif_lower_handle   = PSI_INVALID_HANDLE;
	}
    else
    {
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_open_hif_hd_upper(): hd_runs_on_level_ld(%u) is invalid", 
            p_hd->hd_runs_on_level_ld);
		PSI_FATAL( 0 );
    }

	// Setup HIF args for open HD upper
	pRQB->args.dev_hd_open.hSysDev          = &hd_ptr->sys_upper_handle;
	pRQB->args.dev_hd_open.hH               = PSI_INVALID_HANDLE; // out from HIF
	pRQB->args.dev_hd_open.Cbf              = psi_ld_open_hif_hd_upper_done;
	pRQB->args.dev_hd_open.hd_args.hd_id    = p_hd->hd_id;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_hif_hd_upper(): pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		pRQB, hd_req_idx, p_hd_req->nr_of_hd, p_hd_req->hd_req[hd_req_idx].hd_nr );

	/* post the message to HIF HD */
	psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_upper_done(
    LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                      rc;
	LSA_UINT16                      hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;
	PSI_LD_HD_PTR_TYPE              hd_ptr;

	HIF_RQB_PTR_TYPE const pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);
    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT((HIF_RQB_GET_OPCODE(pRQB)) == HIF_OPC_HD_UPPER_OPEN);

	p_hd_req    = &psi_ld_inst.ld_store_ptr->hd_req;
	hd_nr       = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr      = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_hif_hd_upper_done(): rsp(%u/%#x) pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		(PSI_RQB_GET_RESPONSE(pRQB)),
		(PSI_RQB_GET_RESPONSE(pRQB)),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if (PSI_RQB_GET_RESPONSE(pRQB) != PSI_OK)
	{
		PSI_FATAL(0);
	}

	hd_ptr->hif_upper_handle = pRQB->args.dev_hd_open.hH;

	// Free the RQB
	HIF_FREE_LOCAL_MEM(&rc, pRQB, LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB);
	PSI_ASSERT(rc == LSA_RET_OK);

	// Continue with PSI HD Open (setup the HD system)
	psi_ld_open_hd(p_hd_req->act_req_idx);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_upper(
    LSA_INT hd_req_idx )
{
	LSA_UINT16                      hd_nr;
	HIF_RQB_PTR_TYPE                pRQB;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;
	PSI_LD_HD_PTR_TYPE              hd_ptr;

	/* Create the HIF HD RQB */
	HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, (sizeof(*pRQB)), LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB );
	PSI_ASSERT(pRQB != LSA_NULL);

	HIF_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
	HIF_RQB_SET_OPCODE(pRQB, HIF_OPC_HD_UPPER_CLOSE);
	HIF_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_HIF);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT(p_hd_req->act_req_idx == hd_req_idx);

	hd_nr   = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr  = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	pRQB->args.dev_close.hH     = hd_ptr->hif_upper_handle;
	pRQB->args.dev_close.Cbf    = psi_ld_close_hif_hd_upper_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_hif_hd_upper(): pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		pRQB, p_hd_req->act_req_idx, p_hd_req->nr_of_hd, p_hd_req->hd_req[hd_req_idx].hd_nr );

	/* post the message to HIF HD */
	psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_upper_done(
    LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                         rc;
	LSA_UINT16                         hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE       p_hd_req;
	PSI_LD_HD_PTR_TYPE                 hd_ptr;
	HIF_RQB_PTR_TYPE             const pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);
	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(HIF_RQB_GET_OPCODE(pRQB) == HIF_OPC_HD_UPPER_CLOSE);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_hif_hd_upper_done(): rsp(%u/%#x) pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		(PSI_RQB_GET_RESPONSE(pRQB)),
		(PSI_RQB_GET_RESPONSE(pRQB)),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if (PSI_RQB_GET_RESPONSE(pRQB) != PSI_OK)
	{
		PSI_FATAL(0);
	}

	hd_nr                       = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr                      = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
	hd_ptr->hif_upper_handle    = PSI_INVALID_HANDLE;

	// Free the RQB
	HIF_FREE_LOCAL_MEM(&rc, pRQB, LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB);
	PSI_ASSERT(rc == LSA_RET_OK);

    // HD runs on LD level ?
    if (p_hd_req->hd_req[p_hd_req->act_req_idx].hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_YES)
	{
        // this HD runs on same systems with LD
		// Close lower HIF HD
		psi_ld_close_hif_hd_lower(p_hd_req->act_req_idx);
	}
	else if (p_hd_req->hd_req[p_hd_req->act_req_idx].hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
	{
        // this HD runs on different systems from LD
		PSI_LD_CLOSED_HD(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr);

		// Close (shutdown) next HD or finish the request
		psi_ld_close_next_hd(p_hd_req);
	}
    else
    {
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_close_hif_hd_upper_done(): hd_runs_on_level_ld(%u) is invalid", 
            p_hd_req->hd_req[p_hd_req->act_req_idx].hd_runs_on_level_ld);
        PSI_FATAL( 0 );
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_lower(
    LSA_INT hd_req_idx )
{
	LSA_UINT16                      hd_nr;
	HIF_RQB_PTR_TYPE                pRQB;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;
	PSI_LD_HD_PTR_TYPE              hd_ptr;

	/* Create the HIF HD RQB */
	HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, (sizeof(*pRQB)), LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB );
	PSI_ASSERT(pRQB != LSA_NULL);

	HIF_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
	HIF_RQB_SET_OPCODE(pRQB, HIF_OPC_HD_LOWER_CLOSE);
	HIF_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_HIF);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT(p_hd_req->act_req_idx == hd_req_idx);

	hd_nr   = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr  = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	pRQB->args.dev_close.hH     = hd_ptr->hif_lower_handle;
	pRQB->args.dev_close.Cbf    = psi_ld_close_hif_hd_lower_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_hif_hd_lower(): pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		pRQB, p_hd_req->act_req_idx, p_hd_req->nr_of_hd, p_hd_req->hd_req[hd_req_idx].hd_nr );

	/* post the message to HIF HD */
	psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_lower_done(
    LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                         rc;
	LSA_UINT16                         hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE       p_hd_req;
	PSI_LD_HD_PTR_TYPE                 hd_ptr;
	HIF_RQB_PTR_TYPE             const pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT( pRQB != LSA_NULL );
	PSI_ASSERT( HIF_RQB_GET_OPCODE(pRQB) == HIF_OPC_HD_LOWER_CLOSE );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_hif_hd_lower_done(): rsp(%u/%#x) pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		(PSI_RQB_GET_RESPONSE(pRQB)),
		(PSI_RQB_GET_RESPONSE(pRQB)),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if (PSI_RQB_GET_RESPONSE(pRQB) != PSI_OK)
	{
		PSI_FATAL(0);
	}

	hd_nr                       = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr                      = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
	hd_ptr->hif_lower_handle    = PSI_INVALID_HANDLE;

	// Free the RQB
	HIF_FREE_LOCAL_MEM(&rc, pRQB, LSA_COMP_ID_PSI, HIF_MEM_TYPE_RQB);
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_LD_CLOSED_HD(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr);

	// Close (shutdown) next HD or finish the request
	psi_ld_close_next_hd(p_hd_req);
}
#endif

#if (PSI_CFG_USE_LD_COMP == 1)
/*---------------------------------------------------------------------------*/
/* PSI LD store/get rqb channel handle to post msg to correct task           */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_store_rqb_channel_handle(
    struct psi_header * const pRQB,
    PSI_SYS_PTR_TYPE    const pSys )
{
    LSA_UINT16       const hd_nr  = pSys->hd_nr;
    LSA_HANDLE_TYPE        handle = (LSA_HANDLE_TYPE)PSI_MAX_CHANNELS_PER_COMPONENT; /* Arrayindex + 1 -> Invalid */
	LSA_COMP_ID_TYPE       comp_id;

    PSI_ASSERT(pRQB != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(pRQB);

    switch (comp_id)
    {
        #if (PSI_CFG_USE_HD_COMP == 1)
        case LSA_COMP_ID_ACP:
            PSI_ASSERT((((ACP_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args) != LSA_NULL);
            handle = ((ACP_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args->channel.open.handle;
            break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
        case LSA_COMP_ID_CM:
            PSI_ASSERT(((CM_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.open != LSA_NULL);
            handle = ((CM_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.open->handle;
            break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
        case LSA_COMP_ID_DCP:
            handle = ((DCP_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.handle;
            break;
        #endif

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
        case LSA_COMP_ID_EDD:
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        case LSA_COMP_ID_EDDS:
        case LSA_COMP_ID_EDDT:
        {
	        EDD_HANDLE_LOWER_TYPE EDD_Handle_Lower;

            PSI_ASSERT((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((EDD_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->pParam != LSA_NULL);
            EDD_Handle_Lower = ((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((EDD_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->pParam)->HandleLower;

            if ((LSA_UINT32)EDD_Handle_Lower >= PSI_MAX_CHANNELS_PER_COMPONENT)
            {
                return;
            }
            else
            {
                handle = *((LSA_HANDLE_TYPE*)&EDD_Handle_Lower);
            }

            break;
        }
        #endif

        #if ((PSI_CFG_USE_GSY == 1) && (PSI_CFG_USE_HD_COMP == 1))
        case LSA_COMP_ID_GSY:
            handle = ((GSY_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.handle;
            break;
        #endif

        #if (PSI_CFG_USE_IOH == 1)
        case LSA_COMP_ID_IOH:
            PSI_ASSERT(((IOH_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.u.channel != LSA_NULL);
            handle = ((IOH_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.u.channel->handle;
            break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
        case LSA_COMP_ID_LLDP:
            handle = ((LLDP_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.handle;
            break;
        #endif

        #if ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_HD_COMP == 1))
        case LSA_COMP_ID_MRP:
            handle = ((MRP_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.open.handle;
            break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
        case LSA_COMP_ID_NARE:
            handle = ((NARE_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.handle;
            break;
        #endif

        #if ((PSI_CFG_USE_POF == 1) && (PSI_CFG_USE_HD_COMP == 1))
        case LSA_COMP_ID_POF:
            handle = ((POF_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.handle;
            break;
        #endif

        #if (PSI_CFG_USE_EPS_RQBS == 1)
        case LSA_COMP_ID_PNBOARDS:
            PSI_ASSERT(((EPS_RQB_TYPE2*)(LSA_VOID*)pRQB)->args.channel != LSA_NULL);
            handle = ((EPS_RQB_TYPE2*)(LSA_VOID*)pRQB)->args.channel->handle;
            break;
        #endif

	    #if (PSI_CFG_USE_HSA == 1)
	    case LSA_COMP_ID_HSA:
		    handle = ((HSA_UPPER_RQB_PTR_TYPE)(LSA_VOID*)pRQB)->args.channel.handle;
		    break;
	    #endif

        /* LD Components: nothing to store here */

        case LSA_COMP_ID_TCIP:
        case LSA_COMP_ID_OHA:
        case LSA_COMP_ID_CLRPC:
            return;

        default:
        {
            /* Component not known to be running in HD */
            PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_store_rqb_channel_handle(): CompID(%u/%#x) in pRQB(0x%08x) not known to open channel to HD FW",
                comp_id, comp_id, pRQB);
            PSI_FATAL(0);
        }
    }

    PSI_ASSERT(comp_id < PSI_MAX_COMPONENTS);
    PSI_ASSERT(handle < PSI_MAX_CHANNELS_PER_COMPONENT);
    psi_ld_inst.hd_runs_on.comp[comp_id][handle].hd_nr               = hd_nr;
    psi_ld_inst.hd_runs_on.comp[comp_id][handle].hd_runs_on_level_ld = pSys->hd_runs_on_level_ld;

	PSI_LD_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_ld_store_rqb_channel_handle(): CompID(%u/%#x) in pRQB(0x%08x) handle(%u) hd_nr(%u) hd_runs_on_level_ld(%u)",
        comp_id, comp_id, pRQB, handle, hd_nr, pSys->hd_runs_on_level_ld);

    PSI_ASSERT(psi_ld_inst.hd_runs_on.comp[comp_id][handle].hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX); //Must be PSI_HD_RUNS_ON_LEVEL_LD_NO (0) or _YES (1)
    //lint --e(818) Pointer parameter 'pSys' could be declared as pointing to const - we are unable to change the API
}
#endif

/*---------------------------------------------------------------------------*/
LSA_UINT8 psi_ld_get_hd_runs_on_ld(
    LSA_COMP_ID_TYPE const comp_id,
    LSA_HANDLE_TYPE  const handle )
{
    /* check dimensions of array */
    PSI_ASSERT(comp_id < PSI_MAX_COMPONENTS);
    PSI_ASSERT(handle < PSI_MAX_CHANNELS_PER_COMPONENT);

    /* plausible array entry */
    PSI_ASSERT(psi_ld_inst.hd_runs_on.comp[comp_id][handle].hd_nr);

    return (psi_ld_inst.hd_runs_on.comp[comp_id][handle].hd_runs_on_level_ld);
}

/*---------------------------------------------------------------------------*/
/* PSI HD open/close request                                                 */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hd(
    LSA_INT hd_req_idx )
{
	LSA_UINT16                      hdNr;
	LSA_UINT16                      hd_edd_nr_of_all_ports       = 0;   // parameter necessary for TCP
	LSA_UINT16                      hd_edd_nr_of_all_interfaces  = 0;   // parameter necessary for TCP
	LSA_UINT16                      idx;
	PSI_UPPER_HD_OPEN_PTR_TYPE      p_open;
	PSI_HD_INPUT_PTR_TYPE           p_hd;
	PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;

	/* Create the PSI HD RQB and copy the HD section for LD RQB */
	PSI_RQB_PTR_TYPE const pRQB = (PSI_RQB_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_RQB_TYPE));
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_ASSERT(psi_ld_inst.ld_upper_rqb != LSA_NULL);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT(hd_req_idx == p_hd_req->act_req_idx);

	PSI_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
	PSI_RQB_SET_OPCODE(pRQB, PSI_OPC_HD_OPEN_DEVICE);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_PSI);

	hdNr    = p_hd_req->hd_req[hd_req_idx].hd_nr;
	p_hd    = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[hd_req_idx];
	p_open  = &pRQB->args.hd_open;
	
	PSI_MEMCPY( &p_open->hd_args, p_hd, (sizeof(PSI_HD_INPUT_TYPE)) );

	// patch entries for nr_of_all_ports and nur_of_all_interfaces
	for ( idx = 0; idx < psi_ld_inst.ld_upper_rqb->args.ld_open.hd_count; idx++ )
	{
        PSI_ASSERT(psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[idx].nr_of_ports <= PSI_CFG_MAX_PORT_CNT);
        PSI_ASSERT(psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[idx].nr_of_ports > 0);
	  
        hd_edd_nr_of_all_ports += psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[idx].nr_of_ports;
        hd_edd_nr_of_all_interfaces++;
	}
	
	p_open->hd_args.nr_of_all_ports = hd_edd_nr_of_all_ports;
	p_open->hd_args.nr_of_all_if    = hd_edd_nr_of_all_interfaces; 
		  
    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hdNr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
        p_open->hH = psi_ld_get_hif_hd_upper_handle(hdNr);
    }
    #endif

    p_open->psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE) psi_ld_open_hd_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_hd(): HD Open start, pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		pRQB, hd_req_idx, p_hd_req->nr_of_hd, p_hd_req->hd_req[hd_req_idx].hd_nr );

    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hdNr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    /* post the message to HIF HD */
	    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB);
    }
    else
    #endif
    {
	    /* post the message to PSI HD */
	    psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_system, pRQB);
    }
    
    #if (PSI_CFG_USE_HIF_HD == 0)
    LSA_UNUSED_ARG(hdNr); // suppress compiler warnings
    #endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hd_done(
    LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                         port_idx;
    LSA_UINT16                          pnio_if_nr;
	PSI_HD_IF_OHA_INPUT_PTR_TYPE       p_oha_inp;
	PSI_LD_HD_REQ_STORE_PTR_TYPE       p_hd_req;
	PSI_HD_INPUT_PTR_TYPE              p_hd_in;
	PSI_HD_OUTPUT_PTR_TYPE             p_hd_out;
	LSA_UINT8                          chassis_name[PSI_MAX_STATION_NAME_LEN + 1];
	PSI_RQB_PTR_TYPE             const pRQB = (PSI_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);
	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(PSI_RQB_GET_OPCODE(pRQB) == PSI_OPC_HD_OPEN_DEVICE);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_hd_done(): HD Open finished, rsp(%u/%#x) pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		(PSI_RQB_GET_RESPONSE(pRQB)),
		(PSI_RQB_GET_RESPONSE(pRQB)),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if (PSI_RQB_GET_RESPONSE(pRQB) != PSI_OK)
	{
		PSI_FATAL(0);
	}

	// Add the calculated HD ouptuts to LD upper RQB
	p_hd_out = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_out[p_hd_req->act_req_idx];
	PSI_MEMCPY( p_hd_out, &pRQB->args.hd_open.hd_out, (sizeof(PSI_HD_OUTPUT_TYPE)) );

	// Add the inout HD params to LD upper RQB
	p_hd_in = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[p_hd_req->act_req_idx];

    PSI_MEMCPY( p_hd_in->if_mac, pRQB->args.hd_open.hd_args.if_mac, sizeof(PSI_MAC_TYPE) );
	
	for ( port_idx = 0; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++ )
	{
		PSI_MEMCPY( p_hd_in->port_mac[port_idx], pRQB->args.hd_open.hd_args.port_mac[port_idx], sizeof( PSI_MAC_TYPE) );
	}

	// Free the RQB
	psi_free_local_mem(pRQB);

	// Prepare LLDP chassis ID (using for all PNIO IF)
	PSI_MEMSET( chassis_name, 0, (sizeof(chassis_name)) );
	psi_ld_get_lldp_chassis_id(chassis_name);

    // Prepare the Interfaces for this HD (before open the Sys-Channels)
	pnio_if_nr = p_hd_in->pnio_if.edd_if_id;

    // HD runs on LD level ?
    if (p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
	{
        // this HD runs on different systems from LD
		// Calculate the details for this interface (inputs)
		// Note: the HD sets specifics like (hSysDev,...) are not possible on LD side
        psi_res_calc_set_if_details(0, 0, 0, 0, p_hd_in);
	}

	// register OHA interface (before opening all IF channels
	p_oha_inp = &p_hd_in->pnio_if.oha;
	psi_oha_init_db_if(pnio_if_nr, p_oha_inp, chassis_name);

    //store reduced io_mem_size for ertec200, ertec400 in IOH
    psi_ld_inst.ld_store_ptr->hd[p_hd_in->hd_id - 1 /*index starts with 0*/].io_mem_size = p_hd_out->edd.eddi.io_mem_size;

    //continue with next hd
    p_hd_req->act_req_idx += 1;

    // nex HD Open ?
	if (p_hd_req->act_req_idx < p_hd_req->nr_of_hd)
	{
        #if (PSI_CFG_USE_HIF_HD == 1)
		// get next hd_in parameters		
		p_hd_in = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[p_hd_req->act_req_idx];
        if (p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
        {
		    // Open next HD using HIF HD
		    psi_ld_open_hif_hd_lower(p_hd_req->act_req_idx);
        }
        else
        #endif
        {
		    // Open next HD without HIF HID
		    psi_ld_open_hd(p_hd_req->act_req_idx);
        }
	}
	else
	{
		// all HD open
		PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;

		// continue with open the PSI-LD sys channels (global and IF)
		p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

		p_if_req->act_req_idx = 0;

		psi_ld_open_channels(p_if_req->act_req_idx);
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hd(
    LSA_INT hd_req_idx )
{
	LSA_UINT16                   hdNr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	/* Create the PSI HD RQB and copy the HD section for LD RQB */
	PSI_RQB_PTR_TYPE const pRQB = (PSI_RQB_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_RQB_TYPE));
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
	PSI_RQB_SET_OPCODE(pRQB, PSI_OPC_HD_CLOSE_DEVICE);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_PSI);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT(p_hd_req->act_req_idx == hd_req_idx);

	hdNr                        = p_hd_req->hd_req[hd_req_idx].hd_nr;
	pRQB->args.hd_close.hd_id   = hdNr;
    if (psi_get_hd_runs_on_ld(hdNr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    pRQB->args.hd_close.hH  = psi_ld_get_hif_hd_upper_handle(hdNr);
    }
    pRQB->args.hd_close.psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE) psi_ld_close_hd_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_hd(): HD Close start, pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		pRQB, p_hd_req->act_req_idx, p_hd_req->nr_of_hd, p_hd_req->hd_req[hd_req_idx].hd_nr );

    // go to PSI-HD
    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hdNr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    /* post the message to HIF HD */
	    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB);
    }
    else
    #endif
    {
	    /* post the message to PSI HD */
	    psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_system, pRQB);
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hd_done(
    LSA_VOID_PTR_TYPE rb_ptr )
{
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	PSI_RQB_PTR_TYPE const pRQB = (PSI_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);
	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(PSI_RQB_GET_OPCODE(pRQB) == PSI_OPC_HD_CLOSE_DEVICE);

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_hd_done(): HD Close finished, rsp(%u/%#x) pRQB(0x%08x) req_idx(%u) hd_count(%u) hd_nr(%u)",
		(PSI_RQB_GET_RESPONSE(pRQB)),
		(PSI_RQB_GET_RESPONSE(pRQB)),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if (PSI_RQB_GET_RESPONSE(pRQB) != PSI_OK)
	{
		PSI_FATAL(0);
	}

	// Free the RQB
	psi_free_local_mem(pRQB);

    #if (PSI_CFG_USE_HIF_HD == 1)
	// Close HIF HD upper
    if (psi_get_hd_runs_on_ld(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    psi_ld_close_hif_hd_upper(p_hd_req->act_req_idx);
    }
    else
    #endif
    {
	    PSI_LD_CLOSED_HD(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr);
	    // Close (shutdown) next HD or finish the request
	    psi_ld_close_next_hd(p_hd_req);
    }
}

/*---------------------------------------------------------------------------*/
/*  Channel open/close interface                                             */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_channels(
    LSA_INT if_req_idx )
{
	LSA_UINT16                      hd_nr;
	LSA_UINT16                      path;
    LSA_UINT16                      pnio_if_nr;
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;
	PSI_LD_CHN_OPEN_FCT             fct_open;
    LSA_SYS_PATH_TYPE               sys_path = 0;

	p_if_req    = &psi_ld_inst.ld_store_ptr->if_req;
	hd_nr       = p_if_req->req[if_req_idx].hd_nr;
	path        = p_if_req->req[if_req_idx].sys_path;
    pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(path < PSI_SYS_PATH_MAX);

	// Prepare the sys_path value
    PSI_SYSPATH_SET_HD( sys_path, (hd_nr & 0x000F) ); // mask the hd_nr to prevent the lint warning: --e(734): Loss of precision (assignment) (28 bits to 16 bits)
    PSI_SYSPATH_SET_PATH( sys_path, path );

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channels(): pnio_if_nr(%u) hd_nr(%u) path(%#x/%u) sys_path(%#x) req_idx(%u)",
        pnio_if_nr, hd_nr, path, path, sys_path, if_req_idx );

    // init "fct_open"
    if (pnio_if_nr == PSI_GLOBAL_PNIO_IF_NR)
	{
        // global channels
		fct_open = psi_ld_inst.ld_store_ptr->global_fct[path].fct_open;
	}
	else
	{
        // if channels
		fct_open = psi_ld_inst.ld_store_ptr->if_fct[path].fct_open;
	}

	// Open registered system channels from LD IF req list
	PSI_ASSERT(fct_open != PSI_FCT_PTR_NULL);

	// Start opening the system channel
	(fct_open)(sys_path);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_channels_done(
    LSA_SYS_PATH_TYPE sys_path, 
    LSA_HANDLE_TYPE   handle )
{
	LSA_UINT16                      hd_nr;
	LSA_UINT16                      path;
    LSA_UINT16                      pnio_if_nr;
	LSA_INT                         act_idx;
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;

	// Register sys channel handle for SYS PATH
	psi_ld_set_sys_handle (sys_path, handle);

    p_if_req    = &psi_ld_inst.ld_store_ptr->if_req;
	hd_nr       = PSI_SYSPATH_GET_HD(sys_path);
	path        = PSI_SYSPATH_GET_PATH(sys_path);
    pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

	PSI_LD_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channels_done(): pnio_if_nr(%u) hd_nr(%u) path(%#x/%u) sys_path(%#x) handle(%#x) req_idx(%u)",
        pnio_if_nr, hd_nr, path, path, sys_path, handle, p_if_req->act_req_idx );

    act_idx = p_if_req->act_req_idx + 1;

    // Next sys channel ?
	if (act_idx < p_if_req->nr_of_req)
	{
		/* open next sys channels from req list */
		p_if_req->act_req_idx = act_idx;

		psi_ld_open_channels(act_idx);
	}
	else
	{
		/* all channels done --> respond the system request */
		PSI_UPPER_CALLBACK_FCT_PTR_TYPE     cbf_fct;

		PSI_RQB_PTR_TYPE const pRQB = psi_ld_inst.ld_upper_rqb;

        // Open finished --> response to Upper
		psi_ld_inst.ld_upper_rqb = LSA_NULL;

		cbf_fct = pRQB->args.ld_open.psi_request_upper_done_ptr;

        #if (PSI_CFG_USE_DNS == 1)
        pRQB->args.ld_open.ld_out.supported_comps[PSI_DNS_COMP_IDX].lsa_comp_id   = LSA_COMP_ID_DNS;
        pRQB->args.ld_open.ld_out.supported_comps[PSI_DNS_COMP_IDX].use_comp      = LSA_TRUE;
        #else
        pRQB->args.ld_open.ld_out.supported_comps[PSI_DNS_COMP_IDX].lsa_comp_id  = LSA_COMP_ID_DNS;
        pRQB->args.ld_open.ld_out.supported_comps[PSI_DNS_COMP_IDX].use_comp     = LSA_FALSE;
        #endif

		PSI_RQB_SET_RESPONSE(pRQB, PSI_OK);
		psi_ld_inst.ld_state = PSI_LD_OPEN_DONE;

	    PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channels_done(): LD Open finished, hd_count(%u)", 
            pRQB->args.ld_open.hd_count );

        // go back to application
        #if (PSI_CFG_USE_HIF_LD == 1)
        if ((psi_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (psi_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
        {
		    psi_system_request_done(PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf_fct, pRQB);
        }
        else if (psi_get_ld_runs_on() != PSI_LD_RUNS_ON_LIGHT)
        {
            PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_ld_open_channels_done(): Invalid ld_runs_on(%u) value!", psi_ld_inst.ld_runs_on );
            PSI_FATAL(0);
        }
        else
        #endif
        {
		    psi_system_request_done(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)cbf_fct, pRQB);
        }
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_channels(
    LSA_INT if_req_idx )
{
	LSA_UINT8                       handle;
	LSA_UINT16                      hd_nr;
	LSA_UINT16                      path;
    LSA_UINT16                      pnio_if_nr;
	LSA_SYS_PATH_TYPE               sys_path = 0;
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;
	PSI_LD_CHN_CLOSE_FCT            fct_close;

	p_if_req    = &psi_ld_inst.ld_store_ptr->if_req;
	hd_nr       = p_if_req->req[if_req_idx].hd_nr;
	path        = p_if_req->req[if_req_idx].sys_path;
    pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

    // init "fct_close"
	if (pnio_if_nr == PSI_GLOBAL_PNIO_IF_NR)
	{
        // global channels
		fct_close   = psi_ld_inst.ld_store_ptr->global_fct[path].fct_close;
		handle      = psi_ld_inst.ld_store_ptr->global_lower_handle[path];
	}
	else
	{
        // if channels
		fct_close   = psi_ld_inst.ld_store_ptr->if_fct[path].fct_close;
		handle      = psi_ld_inst.ld_store_ptr->hd[hd_nr-1].lower_handle[path];
	}

	// Close registered system channels from LD IF req list
	PSI_ASSERT(fct_close != PSI_FCT_PTR_NULL);

	// Prepare the sys_path value
	PSI_SYSPATH_SET_HD( sys_path, (hd_nr & 0x000F)); // mask the hd_nr to prevent the lint warning: --e(734): Loss of precision (assignment) (28 bits to 16 bits)
	PSI_SYSPATH_SET_PATH( sys_path, path );

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_channels(): pnio_if_nr(%u) hd_nr(%u) path(%#x/%u) sys_path(%#x) req_idx(%u)",
        pnio_if_nr, hd_nr, path, path, sys_path, if_req_idx );

    // Start closing the system channel
	(fct_close)(sys_path, handle);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_channels_done(
    LSA_SYS_PATH_TYPE sys_path )
{
	LSA_UINT16                      hd_nr;
	LSA_UINT16                      path;
    LSA_UINT16                      pnio_if_nr;
	LSA_INT                         act_idx;
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;

	// Unregister sys channel handle for SYS PATH
	psi_ld_reset_sys_handle(sys_path);

	p_if_req    = &psi_ld_inst.ld_store_ptr->if_req;
	hd_nr       = PSI_SYSPATH_GET_HD(sys_path);
	path        = PSI_SYSPATH_GET_PATH(sys_path);
    pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_channels_done(): pnio_if_nr(%u) hd_nr(%u) path(%#x/%u) sys_path(%#x) req_idx(%u)",
        pnio_if_nr, hd_nr, path, path, sys_path, p_if_req->act_req_idx );

    act_idx = p_if_req->act_req_idx - 1;

    // Next channel ?
	if (act_idx >= 0)
	{
		/* close next sys channels from req list */
		p_if_req->act_req_idx = act_idx;

		psi_ld_close_channels(act_idx);
	}
	else
	{
		/* Start closing all HD */
		PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

		p_hd_req->act_req_idx = p_hd_req->nr_of_hd - 1;

		// close all HD devices
		psi_ld_close_hd(p_hd_req->act_req_idx);
	}
}

/*---------------------------------------------------------------------------*/
LSA_UINT16 psi_ld_get_hif_hd_upper_handle(
    LSA_UINT16 hd_nr )
{
	LSA_UINT16         hif_handle;
	PSI_LD_HD_PTR_TYPE hd_ptr;

	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);
	PSI_ASSERT((hd_nr >= 1) && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	hif_handle = hd_ptr->hif_upper_handle;
    #if (PSI_CFG_USE_HIF_HD == 1 )
    if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    PSI_ASSERT(hif_handle != PSI_INVALID_HANDLE);
    }
    #endif

	return hif_handle;
}

LSA_UINT16 psi_ld_get_hif_hd_lower_handle(
    LSA_UINT16 hd_nr )
{
	LSA_UINT16         hif_handle;
	PSI_LD_HD_PTR_TYPE hd_ptr;

	PSI_ASSERT(psi_ld_inst.ld_store_ptr != LSA_NULL);
	PSI_ASSERT((hd_nr >= 1) && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	if (hd_ptr->sys_lower_handle.shortcut_mode) // UPPER/LOWER shortcut mode ?
	{
		hif_handle = hd_ptr->hif_lower_handle;
		PSI_ASSERT(hif_handle != PSI_INVALID_HANDLE);
	}
	else
	{
		hif_handle = PSI_INVALID_HANDLE;

        #if (PSI_CFG_USE_HIF_HD == 1)
        if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
        {
		    // lower HIF HD is opened by FW system --> try to get the handle
		    // Note: On a HD FW system hd_nr is don't care
		    PSI_HIF_GET_HD_LOWER_HANDLE(&hif_handle, hd_nr);
        }
        else
        #endif
        {
		    PSI_FATAL(0);
        }
	}

	return hif_handle;
}

// this function is only valid in the LD (or LD+HD) firmware
LSA_UINT16 psi_ld_get_nr_of_hd( LSA_VOID )
{
    return (psi_ld_inst.ld_store_ptr->hd_req.nr_of_hd);
}

#endif //(PSI_CFG_USE_LD_COMP == 1)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
