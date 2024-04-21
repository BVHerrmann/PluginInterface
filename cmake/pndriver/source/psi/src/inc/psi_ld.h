#ifndef PSI_LD_H                /* ----- reinclude-protection ----- */
#define PSI_LD_H

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
/*  F i l e               &F: psi_ld.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile for PSI LD instance                                  */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

// PNIO IF storage (details, channels) for one HD
typedef struct psi_ld_hd_tag
{
    LSA_UINT16          hif_lower_handle;               // HIF HD lower handle
    PSI_HIF_SYS_TYPE    sys_lower_handle;               // PSI hSysDev for HIF lower
    LSA_UINT16          hif_upper_handle;               // HIF HD upper handle
    PSI_HIF_SYS_TYPE    sys_upper_handle;               // PSI hSysDev for HIF upper
    LSA_HANDLE_TYPE     lower_handle[PSI_SYS_PATH_MAX]; // lower handle
    LSA_UINT32          io_mem_size;                    // reduced size used for ertec200, ertec400 in IOH
} PSI_LD_HD_TYPE, *PSI_LD_HD_PTR_TYPE;

/*===========================================================================*/
/*                              prototypes                                   */
/*===========================================================================*/

/* user interface */
LSA_VOID psi_ld_init( LSA_VOID );
LSA_VOID psi_ld_undo_init( LSA_VOID );

LSA_VOID psi_ld_open_device( PSI_RQB_PTR_TYPE rqb_ptr );
LSA_VOID psi_ld_close_device( PSI_RQB_PTR_TYPE rqb_ptr  );

/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_HIF_HD == 1 )
/* HIF HD open/close interface for upper and lower */
LSA_VOID psi_ld_open_hif_hd_lower( LSA_INT hd_req_idx );
LSA_VOID psi_ld_open_hif_hd_lower_done( LSA_VOID_PTR_TYPE rqb_ptr );

LSA_VOID psi_ld_open_hif_hd_upper( LSA_INT hd_req_idx );
LSA_VOID psi_ld_open_hif_hd_upper_done( LSA_VOID_PTR_TYPE rqb_ptr );

LSA_VOID psi_ld_close_hif_hd_lower( LSA_INT hd_req_idx );
LSA_VOID psi_ld_close_hif_hd_lower_done( LSA_VOID_PTR_TYPE rqb_ptr );

LSA_VOID psi_ld_close_hif_hd_upper( LSA_INT hd_req_idx );
LSA_VOID psi_ld_close_hif_hd_upper_done( LSA_VOID_PTR_TYPE rqb_ptr );
#endif

/* HD open/close interface */
LSA_VOID psi_ld_open_hd( LSA_INT hd_req_idx );
LSA_VOID psi_ld_open_hd_done( LSA_VOID_PTR_TYPE rqb_ptr );

LSA_VOID psi_ld_close_hd( LSA_INT hd_req_idx );
LSA_VOID psi_ld_close_hd_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*---------------------------------------------------------------------------*/
/* Channel open/close interface */
LSA_VOID    psi_ld_open_channels( LSA_INT hd_req_idx );
LSA_VOID    psi_ld_open_channels_done( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );

LSA_VOID    psi_ld_close_channels( LSA_INT hd_req_idx );
LSA_VOID    psi_ld_close_channels_done( LSA_SYS_PATH_TYPE sys_path );

LSA_UINT16  psi_ld_get_hif_hd_upper_handle( LSA_UINT16 hd_nr );
LSA_UINT16  psi_ld_get_hif_hd_lower_handle( LSA_UINT16 hd_nr );

/* Store HD info in LD */
#if (PSI_CFG_USE_LD_COMP == 1)
LSA_VOID    psi_ld_store_rqb_channel_handle( struct psi_header * const pRQB, PSI_SYS_PTR_TYPE const pSys );
#endif
LSA_UINT8   psi_ld_get_hd_runs_on_ld( LSA_COMP_ID_TYPE const comp_id, LSA_HANDLE_TYPE const handle );
LSA_UINT32  psi_ld_get_real_pi_size( LSA_UINT16 const hd_nr );

/*---------------------------------------------------------------------------*/
LSA_UINT16  psi_ld_get_nr_of_hd(LSA_VOID);

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PSI_LD_H */
