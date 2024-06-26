#ifndef EPS_CP_MEM_H_               /* ----- reinclude-protection ----- */
#define EPS_CP_MEM_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_cp_mem.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/* Memory pool handling (used for NRT, PI, HIF pools which are Dev specific) */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef LSA_VOID * (* EPS_MEMSET_FCT)( LSA_VOID_PTR_TYPE, LSA_INT, size_t ); // according to MEM3 type pMemSet_ft (see mem3if.h) CAUTION: size_t type (stddef.h) is required for EPS_MEMSET_FCT

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

/* Init / Undo init */
LSA_VOID    eps_cp_mem_init( LSA_VOID );
LSA_VOID    eps_cp_mem_undo_init( LSA_VOID );

/* Create / Destroy device specific DEV memory pools */
LSA_INT     eps_cp_mem_create_dev_pool( LSA_UINT16 board_nr, LSA_UINT8* base_ptr, LSA_UINT32 size );
LSA_VOID    eps_cp_mem_delete_dev_pool( LSA_UINT16 board_nr, LSA_INT pool_handle );

/* DEV memory pools */
LSA_UINT8*  eps_cp_mem_dev_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_dev_free( LSA_UINT8 * ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );

/* Create / Destroy device specific NRT memory pools */
LSA_INT     eps_cp_mem_create_nrt_pool( LSA_UINT16 board_nr, LSA_UINT8* base_ptr, LSA_UINT32 size, LSA_UINT8 cp_mem_nrt_type );
LSA_VOID    eps_cp_mem_delete_nrt_pool( LSA_UINT16 board_nr, LSA_INT pool_handle, LSA_UINT8 cp_mem_nrt_type );

/* Selective NRT memory alloc / frees */
LSA_UINT8*  eps_cp_mem_nrt_tx_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_nrt_tx_free( LSA_UINT8* ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_nrt_rx_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_nrt_rx_free( LSA_UINT8* ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
#if ( PSI_CFG_USE_EDDI == 1 )
LSA_UINT8*  eps_cp_mem_eddi_nrt_dmacw_default_alloc( LSA_UINT32 size,  LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_dmacw_default_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_rx_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_rx_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_tx_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_tx_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_tx_low_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_tx_low_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_tx_mid_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_tx_mid_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_tx_high_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_tx_high_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_tx_mgmtlow_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_tx_mgmtlow_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_tx_mgmthigh_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_tx_mgmthigh_frag_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_UINT8*  eps_cp_mem_eddi_nrt_fatal_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_eddi_nrt_fatal_free( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
#endif // PSI_CFG_USE_EDDI

/* PI memory pools */
LSA_INT     eps_cp_mem_create_pi_pool( LSA_UINT16 board_nr,	LSA_UINT8* base_ptr, LSA_UINT32 size, LSA_UINT32 gran_size, EPS_MEMSET_FCT p_memset_fct );
LSA_VOID    eps_cp_mem_delete_pi_pool( LSA_UINT16 board_nr, LSA_INT pool_handle );
LSA_UINT8*  eps_cp_mem_pi_alloc( LSA_UINT32 size, LSA_BOOL is_provider, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_pi_free( LSA_UINT8* p_buffer, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID*   eps_cp_mem_hd1_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len );
LSA_VOID*   eps_cp_mem_hd2_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len );
LSA_VOID*   eps_cp_mem_hd3_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len );
LSA_VOID*   eps_cp_mem_hd4_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len );

/* Create / Destroy device specific CRT slow memory pools */
LSA_INT     eps_cp_mem_create_crt_slow_pool( LSA_UINT16 board_nr, LSA_UINT8* base_ptr, LSA_UINT32 size, LSA_UINT8 cp_mem_nrt_type );
LSA_VOID    eps_cp_mem_delete_crt_slow_pool( LSA_UINT16 board_nr, LSA_INT pool_handle, LSA_UINT8 cp_mem_nrt_type );

/* CRT slow memory pools */
LSA_UINT8*  eps_cp_mem_crt_slow_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );
LSA_VOID    eps_cp_mem_crt_slow_free( LSA_UINT8* ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id );

/* MEM3 Pool Functions for TCIP memory pool */
#if ( PSI_CFG_USE_TCIP == 1 )
LSA_VOID    eps_cp_mem_create_tcip_pool( LSA_UINT8 * const base_ptr, LSA_UINT32 const size );
LSA_VOID    eps_cp_mem_delete_tcip_pool( LSA_VOID );
LSA_UINT8*  eps_cp_mem_tcip_alloc( LSA_UINT32 size );
LSA_VOID    eps_cp_mem_tcip_free( LSA_UINT8 * p_buffer );
#endif // PSI_CFG_USE_TCIP

/* MEM3 Pool Functions for OpenBSD Page memory pool */
#if ( PSI_CFG_TCIP_STACK_OPEN_BSD == 1 )
LSA_VOID    eps_cp_mem_create_obsd_pool( LSA_UINT8 * const base_ptr, LSA_UINT32 const size );
LSA_VOID    eps_cp_mem_delete_obsd_pool( LSA_VOID );
LSA_UINT8*  eps_cp_mem_obsd_alloc( LSA_UINT32 size );
LSA_VOID    eps_cp_mem_obsd_free( LSA_UINT8 * p_buffer );
#endif // PSI_CFG_TCIP_STACK_OPEN_BSD

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_CP_MEM_H_ */
