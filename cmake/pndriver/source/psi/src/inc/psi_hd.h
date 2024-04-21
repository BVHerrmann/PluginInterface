#ifndef PSI_HD_H                /* ----- reinclude-protection ----- */
#define PSI_HD_H

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: psi_hd.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile for PSI HD instance                                  */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

// Note: the adaption HW info is used for the EDDx setup and adaption functions

// HW information (from system) for one HD
typedef struct psi_hd_hw_tag
{
	PSI_MAC_TYPE            if_mac;
	PSI_MAC_TYPE            port_mac[PSI_CFG_MAX_PORT_CNT];
	PSI_HD_PORT_MAP_TYPE    port_map[PSI_CFG_MAX_PORT_CNT+1];
    LSA_BOOL                b_edd_nrt_copy_if_on;
	LSA_UINT16              edd_type;
	PSI_HD_MEM_TYPE         hif_mem;
	PSI_HD_MEM_TYPE         pi_mem;
	PSI_HD_MEM_TYPE         dev_mem;
	PSI_HD_MEM_TYPE         nrt_tx_mem;
	PSI_HD_MEM_TYPE         nrt_rx_mem;
	PSI_HD_MEM_TYPE         crt_slow_mem;
	PSI_HD_EDD_TYPE         edd;
} PSI_HD_HW_TYPE, *PSI_HD_HW_PTR_TYPE;
/* if_mac                       : IF mac from system */
/* port_mac                     : port MACs from system */
/* port_map                     : User to HW port mapping (port index 0 is reserved) for IF */
/* b_edd_nrt_copy_if_on         : EDDx NRT Copy Interface activation (with active EDDx NRT Copy Interface applications can use local memory instead of NRT memory) */
/* edd_type                     : EDDx type */
/* hif_mem                      : HIF memory */
/* pi_mem                       : PI memory used for CRT (SDRam, KRAM, IOCC) */
/* dev_mem                      : DEV memory */
/* nrt_tx_mem                   : NRT TX memory used for NRT Buffer (SDRam, SHM) */
/* nrt_rx_mem                   : NRT RX memory used for NRT Buffer (SDRam, SHM) */
/* crt_slow_mem                 : CRT slow memory */
/* edd                          : EDDx HD information */

// Storage type for a HD IF 
typedef struct psi_hd_if_tag
{
	LSA_BOOL            is_used;        // HD IF used yes/no
	PSI_SYS_HANDLE      sys_handle;     // hSysDev from System for this dev
	LSA_UINT16          pnio_if_nr;     // PNIO IF number for this HD-IF: [0..16] (0=global, 1-16=PNIO IF) (set by TIA)
	LSA_BOOL            is_arp_on;      // ARP sniffing is turned on yes/no
} PSI_HD_IF_TYPE, *PSI_HD_IF_PTR_TYPE;

// EDDI states for startup/shutdown
typedef enum psi_hd_eddi_state
{
    PSI_HD_EDDI_STATE_INIT = 0,
    PSI_HD_EDDI_STATE_DEV_OPEN_START,
    PSI_HD_EDDI_STATE_DEV_OPEN_DONE,
    PSI_HD_EDDI_STATE_DEV_COMP_INI_START,
    PSI_HD_EDDI_STATE_DEV_COMP_INI_DONE,
    PSI_HD_EDDI_STATE_DEV_SETUP_START,
    PSI_HD_EDDI_STATE_DEV_SETUP_DONE,
    PSI_HD_EDDI_STATE_DEV_CLOSE_START,
    PSI_HD_EDDI_STATE_DEV_CLOSE_DONE
} PSI_HD_EDDI_STATE_TYPE;

// EDDP states for startup/shutdown
typedef enum psi_hd_eddp_state
{
    PSI_HD_EDDP_STATE_INIT = 0,
    PSI_HD_EDDP_STATE_DEV_OPEN_START,
    PSI_HD_EDDP_STATE_DEV_OPEN_DONE,
    PSI_HD_EDDP_STATE_DEV_SETUP_START,
    PSI_HD_EDDP_STATE_DEV_SETUP_DONE,
    PSI_HD_EDDP_STATE_DEV_SHUTDOWN_START,
    PSI_HD_EDDP_STATE_DEV_SHUTDOWN_DONE,
    PSI_HD_EDDP_STATE_DEV_CLOSE_START,
    PSI_HD_EDDP_STATE_DEV_CLOSE_DONE
} PSI_HD_EDDP_STATE_TYPE;

// EDDS states for startup/shutdown
typedef enum psi_hd_edds_state
{
    PSI_HD_EDDS_STATE_INIT = 0,
    PSI_HD_EDDS_STATE_DEV_OPEN_START,
    PSI_HD_EDDS_STATE_DEV_OPEN_DONE,
    PSI_HD_EDDS_STATE_DEV_SETUP_START,
    PSI_HD_EDDS_STATE_DEV_SETUP_DONE,
    PSI_HD_EDDS_STATE_DEV_SHUTDOWN_START,
    PSI_HD_EDDS_STATE_DEV_SHUTDOWN_DONE,
    PSI_HD_EDDS_STATE_DEV_CLOSE_START,
    PSI_HD_EDDS_STATE_DEV_CLOSE_DONE
} PSI_HD_EDDS_STATE_TYPE;

// EDDT states for startup/shutdown
typedef enum psi_hd_eddt_state
{
    PSI_HD_EDDT_STATE_INIT = 0,
    PSI_HD_EDDT_STATE_DEV_OPEN_START,
    PSI_HD_EDDT_STATE_DEV_OPEN_DONE,
    PSI_HD_EDDT_STATE_DEV_SETUP_START,
    PSI_HD_EDDT_STATE_DEV_SETUP_DONE,
    PSI_HD_EDDT_STATE_DEV_SHUTDOWN_START,
    PSI_HD_EDDT_STATE_DEV_SHUTDOWN_DONE,
    PSI_HD_EDDT_STATE_DEV_CLOSE_START,
    PSI_HD_EDDT_STATE_DEV_CLOSE_DONE
} PSI_HD_EDDT_STATE_TYPE;

// Storage type for a HD instance
typedef struct psi_hd_store_tag
{
	LSA_BOOL                is_used;                            // HD used yes/no
    LSA_UINT32              nr_of_ports;                        // nr of ports (within one HD)
    LSA_UINT16              nr_of_all_if;                       // nr of all IFs, PSI LD sets this value
    LSA_UINT16              nr_of_all_ports;                    // nr of ports over all HDs (all IFs), PSI LD sets this value
    LSA_UINT8               hd_runs_on_level_ld;                // 1: HD is on LD level / 0: HD is on separate level from LD
	PSI_SYS_HANDLE          hd_sys_handle;                      // hSysDev from System for this HD
	PSI_EDD_HDDB            edd_hDDB;                           // EDD HDDB from Device OPEN
	LSA_INT                 dev_pool_handle;                    // DEV pool handle
	LSA_INT                 nrt_tx_pool_handle;                 // NRT TX pool handle
	LSA_INT                 nrt_rx_pool_handle;                 // NRT RX pool handle
    LSA_INT                 nrt_dmacw_default_pool_handle;      // NRT pool handle (DMACW pool, EDDI)
    LSA_INT                 nrt_tx_low_frag_pool_handle;        // NRT pool handle (TX_LOW fragments, EDDI)
    LSA_INT                 nrt_tx_mid_frag_pool_handle;        // NRT pool handle (TX_MID fragments, EDDI)
    LSA_INT                 nrt_tx_high_frag_pool_handle;       // NRT pool handle (TX_HIGH fragments, EDDI)
    LSA_INT                 nrt_tx_mgmtlow_frag_pool_handle;    // NRT pool handle (TX_MGMTLOW fragments, EDDI)
    LSA_INT                 nrt_tx_mgmthigh_frag_pool_handle;   // NRT pool handle (TX_MGMTHIGH fragments, EDDI)
	PSI_HD_HW_TYPE          hd_hw;                              // HW information from system
	PSI_HD_IF_TYPE          hd_if;                              // IF information from system

    PSI_HD_EDDI_STATE_TYPE  eddi_state;                         // EDDI states for startup/shutdown
    PSI_HD_EDDP_STATE_TYPE  eddp_state;                         // EDDP states for startup/shutdown
    PSI_HD_EDDS_STATE_TYPE  edds_state;                         // EDDS states for startup/shutdown
    PSI_HD_EDDT_STATE_TYPE  eddt_state;                         // EDDT states for startup/shutdown
} PSI_HD_STORE_TYPE, *PSI_HD_STORE_PTR_TYPE;

/*===========================================================================*/
/*                              prototypes                                   */
/*===========================================================================*/

LSA_VOID psi_hd_init( LSA_VOID );
LSA_VOID psi_hd_undo_init( LSA_VOID );

LSA_VOID psi_hd_open_device( PSI_RQB_PTR_TYPE const rqb_ptr );
LSA_VOID psi_hd_close_device( PSI_RQB_PTR_TYPE const rqb_ptr );

PSI_EDD_HDDB psi_hd_get_hDDB( LSA_UINT16 const hd_nr );

PSI_HD_STORE_PTR_TYPE psi_hd_get_hd_store( LSA_UINT16 const hd_nr );

PSI_RQB_PTR_TYPE psi_hd_get_hd_open_device_rqb_ptr( LSA_VOID );

LSA_VOID psi_hd_set_arp_on_state( LSA_UINT16 const pnio_if_nr, LSA_BOOL const state );
LSA_BOOL psi_hd_is_arp_on_state( LSA_UINT16 const pnio_if_nr );

LSA_VOID psi_hd_edd_open_done( LSA_UINT16 const hd_nr, LSA_VOID_PTR_TYPE const p_data );
LSA_VOID psi_hd_edd_close_done( LSA_UINT16 const hd_nr, LSA_VOID_PTR_TYPE const p_data );
LSA_VOID psi_hd_setup_edd_undo_init( PSI_RQB_EDD_UNDO_INIT_PTR_TYPE const pRQB );
LSA_VOID psi_hd_setup_edd_undo_init_done( PSI_RQB_EDD_UNDO_INIT_PTR_TYPE const pRQB );

LSA_VOID psi_hd_get_if_mac( LSA_UINT16 hd_nr, LSA_UINT32 * if_ptr, PSI_MAC_TYPE * mac_ptr );

LSA_UINT8 psi_hd_get_runs_on_level_ld( LSA_VOID );

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif /* PSI_HD_H */
