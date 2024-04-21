#ifndef PSI_SYS_H_                  /* ----- reinclude-protection ----- */
#define PSI_SYS_H_

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define PSI_COMP_ID_LAST    (LSA_COMP_ID_LAST - 1) /* see <lsa_sys.h> */

/*----------------------------------------------------------------------------*/
/* All OPEN_CHANNEL Opcodes of used components must fit to this define, to    */
/* check correctly, if an channel is opend and store the channel handle       */
/*----------------------------------------------------------------------------*/
#define PSI_OPC_GENERAL_OPEN_CHANNEL        1
    
/*----------------------------------------------------------------------------*/

struct psi_header
{
	LSA_RQB_HEADER(struct psi_header*)
};

typedef void(*psi_postfunction)(void*);

/*==========================================================================================*/
/*  PSI Sys Ptr                                                                             */
/*  Parameters of PSI_SYS_TYPE are filled by PSI on every xxx_get_path_info().              */
/*  If a LSA component calls their output macros, they give the "sys_ptr" to the struct.    */
/*==========================================================================================*/

typedef struct psi_sys_tag
{
	LSA_UINT16      hd_nr;                  // HD number [1..N]
	LSA_UINT16      comp_id;                // LSA_COMP_ID of owner
	LSA_UINT16      comp_id_lower;          // LSA_COMP_ID of lower component (i.E.: used EDDx lower apdaption)
	LSA_UINT16      comp_id_edd;            // LSA_COMP_ID of used EDDx (i.E.: used for HW adaptiom (i.E. GSY Sync)
	LSA_UINT16      pnio_if_nr;             // PNIO IF number for this HD-IF: [0..16] (0=global, 1-16=PNIO IF) (set by TIA)
	LSA_UINT16      mbx_id_rsp;             // MBX ID for posting responses (from lower to upper)
	LSA_UINT32      trace_idx;              // Trace Index
	PSI_SYS_HANDLE  hSysDev;                // hSysDev for one HD
	LSA_INT         hPoolDev;               // DEV pool handle for one HD
	LSA_INT         hPoolNrtTx;             // NRT TX pool handle for one HD
	LSA_INT         hPoolNrtRx;             // NRT RX pool handle for one HD
	LSA_BOOL        check_arp;              // check for ARP sniffing at callback yes/no
    LSA_UINT8       hd_runs_on_level_ld;    // 1: HD is on LD level / 0: HD is on separate level from LD
    LSA_UINT16      psi_path;               // system or user channel from xxx_get_path_info()
} PSI_SYS_TYPE, * PSI_SYS_PTR_TYPE;

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
typedef struct psi_edd_sys_tag
{
	LSA_UINT16      hd_nr;                  // HD number [1..N]
	LSA_UINT16      pnio_if_nr;             // PNIO IF number for this HD-IF: [0..16] (0=global, 1-16=PNIO IF) (set by TIA)
	LSA_UINT16      edd_comp_id;            // LSA_COMP_ID of used EDDx
    #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
    LSA_UINT16      mbx_id_edds_rqb;        // MBX ID for EDDS RQB task (from upper to lower)
    #endif
} PSI_EDD_SYS_TYPE, * PSI_EDD_SYS_PTR_TYPE;
#endif

// Note: Tasks see description of PNIO tasks
typedef enum
{
	PSI_NRT_ORG_TASK_ID,
	PSI_NRT_LOW_TASK_ID,
	PSI_EDDI_SYS_REST_TASK_ID,
	PSI_EDDI_AUX_TASK_ID,
	PSI_EDDP_TASK_ID,
	PSI_EDDS_RQB_L1_TASK_ID,
	PSI_EDDS_RQB_L2_TASK_ID,
	PSI_EDDS_RQB_L3_TASK_ID,
	PSI_EDDS_RQB_L4_TASK_ID,
	PSI_EDDS_RQB_H1_TASK_ID,
	PSI_EDDS_RQB_H2_TASK_ID,
    PSI_EDDT_TASK_ID,
    PSI_IP_TASK_ID,
	PSI_PN_LSA_LD_TASK_ID,
	PSI_PN_LSA_HD_TASK_ID,
	PSI_PN_LSA_LOW_TASK_ID,
	PSI_HIF_HD_TASK_ID,
	PSI_HIF_LD_TASK_ID,
    PSI_HIF_LOW_TASK_ID,
	PSI_PN_TASK_ID,
	PSI_POF_TASK_ID,
	PSI_IOH_TASK_ID,
    PSI_SOCKAPP_TASK_ID,
    PSI_EPS_TASK_ID,
	/*-------------------------*/
	PSI_MAX_TASK_ID        /* Max TASK ID */
} PSI_TASK_ID_TYPE;

/*===========================================================================*/
/*  PSI mailbox adaption                                                     */
/*===========================================================================*/
#define PSI_MBX_ID_EDDI_AUX		    PSI_EDDI_AUX_TASK_ID
#define PSI_MBX_ID_EDDI_NRT_LOW	    PSI_NRT_LOW_TASK_ID
#define PSI_MBX_ID_EDDI_REST	    PSI_EDDI_SYS_REST_TASK_ID
#define PSI_MBX_ID_EDDP_LOW		    PSI_EDDP_TASK_ID
#define PSI_MBX_ID_EDDS_RQB_L1	    PSI_EDDS_RQB_L1_TASK_ID
#define PSI_MBX_ID_EDDS_RQB_L2	    PSI_EDDS_RQB_L2_TASK_ID
#define PSI_MBX_ID_EDDS_RQB_L3	    PSI_EDDS_RQB_L3_TASK_ID
#define PSI_MBX_ID_EDDS_RQB_L4	    PSI_EDDS_RQB_L4_TASK_ID
#define PSI_MBX_ID_EDDS_RQB_H1		PSI_EDDS_RQB_H1_TASK_ID
#define PSI_MBX_ID_EDDS_RQB_H2		PSI_EDDS_RQB_H2_TASK_ID
#define PSI_MBX_ID_EDDT_LOW		    PSI_EDDT_TASK_ID
#define PSI_MBX_ID_HIF_HD		    PSI_HIF_HD_TASK_ID
#define PSI_MBX_ID_HIF_LD	        PSI_HIF_LD_TASK_ID
#define PSI_MBX_ID_HIF_LOW	        PSI_HIF_LOW_TASK_ID
#define PSI_MBX_ID_EDDI_NRT_ORG	    PSI_NRT_ORG_TASK_ID
#define PSI_MBX_ID_GSY	            PSI_NRT_ORG_TASK_ID
#define PSI_MBX_ID_LLDP			    PSI_NRT_ORG_TASK_ID    
#define PSI_MBX_ID_MRP		        PSI_NRT_ORG_TASK_ID
#define	PSI_MBX_ID_HSA				PSI_NRT_ORG_TASK_ID
#define PSI_MBX_ID_ACP			    PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_CM	            PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_DCP	            PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_NARE			    PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_CLRPC		    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_OHA			    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_POF			    PSI_POF_TASK_ID
#define PSI_MBX_ID_DNS			    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_SNMPX		    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_SOCK			    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_TCIP			    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_IOH              PSI_IOH_TASK_ID
#define PSI_MBX_ID_SOCKAPP          PSI_SOCKAPP_TASK_ID
#define PSI_MBX_ID_PSI              PSI_PN_TASK_ID
#define PSI_MBX_ID_EPS  			PSI_EPS_TASK_ID
#define PSI_MBX_ID_USER			    PSI_MAX_TASK_ID
#define PSI_MBX_ID_MAX              (PSI_MAX_TASK_ID+1)

LSA_VOID psi_mbx_sys_init(LSA_VOID);
LSA_VOID psi_mbx_sys_undo_init(LSA_VOID);

/*===========================================================================*/
/*  PSI threading                                                            */
/*===========================================================================*/

#ifndef PSI_THREAD_READY
LSA_VOID PSI_THREAD_READY( LSA_VOID_PTR_TYPE arg );
#endif

#ifndef PSI_THREAD_STOPPED
LSA_VOID PSI_THREAD_STOPPED( LSA_VOID_PTR_TYPE arg );
#endif

/*---------------------------------------------------------------------------*/
/*  SYS PATH definition for system and user channels                         */
/*---------------------------------------------------------------------------*/

typedef enum psi_path_enum
{
	PSI_SYS_PATH_INVALID = 0, /* Reserved for invalid */

	/* group of global system channels */
	PSI_PATH_GLO_SYS_CLRPC_SOCK,
	PSI_PATH_GLO_SYS_SNMPX_SOCK,
	PSI_PATH_GLO_SYS_OHA_SOCK,
	PSI_PATH_GLO_SYS_OHA_DNS,
    PSI_PATH_GLO_SYS_SNMPX_OHA,
    PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01,
    PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02,
    PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03,
    PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04,

	/* group of IF specific system channels */
	PSI_PATH_IF_SYS_MRP_EDD,
	PSI_PATH_IF_SYS_TCIP_EDD_ARP,
	PSI_PATH_IF_SYS_TCIP_EDD_ICMP,
	PSI_PATH_IF_SYS_TCIP_EDD_UDP,
	PSI_PATH_IF_SYS_TCIP_EDD_TCP,
	PSI_PATH_IF_SYS_DCP_EDD,
	PSI_PATH_IF_SYS_NARE_EDD,
	PSI_PATH_IF_SYS_NARE_DCP_EDD,
	PSI_PATH_IF_SYS_OHA_EDD,
	PSI_PATH_IF_SYS_OHA_LLDP_EDD,
	PSI_PATH_IF_SYS_OHA_DCP_EDD,
	PSI_PATH_IF_SYS_OHA_NARE,
	PSI_PATH_IF_SYS_OHA_MRP,
	PSI_PATH_IF_SYS_GSY_EDD_SYNC,
	PSI_PATH_IF_SYS_GSY_EDD_ANNO,
	PSI_PATH_IF_SYS_ACP_EDD,
	PSI_PATH_IF_SYS_CM_EDD,
	PSI_PATH_IF_SYS_CM_CLRPC,
	PSI_PATH_IF_SYS_CM_MRP,
	PSI_PATH_IF_SYS_CM_POF_EDD,
	PSI_PATH_IF_SYS_CM_GSY,
	PSI_PATH_IF_SYS_CM_NARE,
	PSI_PATH_IF_SYS_CM_OHA,
	PSI_PATH_IF_SYS_HSA_EDD,

	/* group of global application user channels */
	PSI_PATH_GLO_APP_SOCK1,
	PSI_PATH_GLO_APP_SOCK2,
	PSI_PATH_GLO_APP_SOCK3,
	PSI_PATH_GLO_APP_SOCK4,
	PSI_PATH_GLO_APP_SOCK5,
	PSI_PATH_GLO_APP_SOCK6,
	PSI_PATH_GLO_APP_SOCK7,
	PSI_PATH_GLO_APP_SOCK8,
	PSI_PATH_GLO_APP_OHA,
	PSI_PATH_GLO_APP_DNS,
	PSI_PATH_GLO_APP_SNMPX,
	PSI_PATH_GLO_APP_IOS_CLRPC,
	PSI_PATH_GLO_APP_EPS_LD,
	PSI_PATH_GLO_APP_HSA,
    PSI_PATH_GLO_APP_SOCKAPP,
    PSI_PATH_GLO_APP_IOH,

    /* group of IF specific application user channels */
    PSI_PATH_IF_APP_CMPD,
    PSI_PATH_IF_APP_IOC_CMCL_ACP,
    PSI_PATH_IF_APP_IOM_CMMC,
    PSI_PATH_IF_APP_IOD_CMSV_ACP,
    PSI_PATH_IF_APP_OHA,
    PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC,
    PSI_PATH_IF_APP_NARE_IP_TEST,
	PSI_PATH_IF_APP_EPS_HD,

    /*---------------------------------------------------------------------------*/
    PSI_SYS_PATH_MAX
} PSI_SYS_PATH_TYPE;

/*===========================================================================*/
/*     FATAL error types                                                     */
/*===========================================================================*/

#define PSI_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
	struct lsa_fatal_error_tag*

/*===========================================================================*/
/*                                FctPtr NRTMEM                              */
/*===========================================================================*/
typedef struct _PSI_USR_NRTMEM_ID_FKT_TYPE
{
	LSA_UINT8* LSA_FCT_PTR( , Alloc)( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
	LSA_VOID   LSA_FCT_PTR( , Free)( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
} PSI_USR_NRTMEM_ID_FKT_TYPE;

typedef PSI_USR_NRTMEM_ID_FKT_TYPE const* PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE;

/*===========================================================================*/
/*                                   Macros                                  */
/*===========================================================================*/

#define PSI_FATAL( error_code_0 ) \
	psi_fatal_error(LSA_COMP_ID_PSI, PSI_MODULE_ID, (LSA_UINT16)__LINE__, (LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL)

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*  Initialization                                                           */
/*---------------------------------------------------------------------------*/

LSA_UINT16 psi_init( LSA_VOID );
LSA_UINT16 psi_undo_init( LSA_VOID );

/*---------------------------------------------------------------------------*/
/*  stdlib                                                                   */
/*---------------------------------------------------------------------------*/

#ifndef PSI_MEMSET
LSA_VOID PSI_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT           val,
	LSA_UINT          len );
#endif

#ifndef PSI_MEMCPY
LSA_VOID PSI_MEMCPY(
	LSA_VOID_PTR_TYPE       dst,
	const LSA_VOID_PTR_TYPE src,
	LSA_UINT                len );
#endif

#ifndef PSI_MEMCMP
LSA_INT PSI_MEMCMP(
	const LSA_VOID_PTR_TYPE pBuf1,
	const LSA_VOID_PTR_TYPE pBuf2,
	LSA_UINT                Length );
#endif

#ifndef PSI_MEMMOVE
LSA_VOID PSI_MEMMOVE(
	LSA_VOID_PTR_TYPE       dst,
	const LSA_VOID_PTR_TYPE src,
	LSA_UINT                len );
#endif

#ifndef PSI_STRCPY
LSA_VOID PSI_STRCPY(
	LSA_VOID_PTR_TYPE       dst,
	const LSA_VOID_PTR_TYPE src );
#endif

#ifndef PSI_STRLEN
LSA_UINT PSI_STRLEN(
	const LSA_VOID_PTR_TYPE str );
#endif

#ifndef PSI_SPRINTF
LSA_INT PSI_SPRINTF(
	LSA_UINT8  *str,
	LSA_CHAR   *fmt,
	... );
#endif

#ifndef PSI_SSCANF_UUID
LSA_INT PSI_SSCANF_UUID(
	const LSA_UINT8 *uuid_string,
	const LSA_CHAR  *fmt,
	LSA_UINT32      *time_low,
	LSA_INT         *time_mid,
	LSA_INT         *time_hi_and_version,
	LSA_INT         *clock_seq_hi_and_reserved,
	LSA_INT         *clock_seq_low,
	LSA_INT         *node0,
	LSA_INT         *node1,
	LSA_INT         *node2,
	LSA_INT         *node3,
	LSA_INT         *node4,
	LSA_INT         *node5,
	LSA_INT         *read_count );
#endif

#ifndef PSI_LOG_10
LSA_VOID PSI_LOG_10(
	LSA_UINT32  const Arg_in_ppm,
	LSA_INT32         * pResult_in_ppm );
#endif

#ifndef PSI_POW_10
LSA_VOID PSI_POW_10(
	LSA_INT16       const numerator, 
	LSA_UINT16      const denominator, 
	LSA_UINT32*     pResult );
#endif

#ifndef PSI_EXCHANGE_LONG
long PSI_EXCHANGE_LONG(
	long volatile * long_ptr,
	long            val );
#endif

/*---------------------------------------------------------------------------*/
/*  FATAL handling                                                           */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_fatal_error(
	LSA_UINT16         comp_id,
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32         error_code_0,
	LSA_UINT32         error_code_1,
	LSA_UINT32         error_code_2,
	LSA_UINT32         error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data );

#ifndef PSI_FATAL_ERROR
LSA_VOID PSI_FATAL_ERROR(
	LSA_CHAR                  *comp,
	LSA_UINT16                length,
	PSI_FATAL_ERROR_PTR_TYPE  error_ptr );
#endif

#ifndef PSI_RQB_ERROR
LSA_VOID PSI_RQB_ERROR(
	LSA_UINT16         comp_id,
	LSA_UINT16         comp_id_lower,
	LSA_VOID_PTR_TYPE  rqb_ptr );
#endif

#ifndef PSI_FATAL_ERROR_OCCURED
LSA_BOOL PSI_FATAL_ERROR_OCCURED( LSA_VOID );
#endif

/*----------------------------------------------------------------------------*/
/* Reentrance Locks                                                           */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_REENTRANCE_LOCK
LSA_VOID PSI_ALLOC_REENTRANCE_LOCK(
    LSA_UINT16 * ret_val_ptr,
    LSA_UINT16 * lock_handle_ptr );
#endif

#ifndef PSI_ALLOC_REENTRANCE_LOCK_PRIO_PROTECTED
LSA_VOID PSI_ALLOC_REENTRANCE_LOCK_PRIO_PROTECTED( 
    LSA_UINT16 * ret_val_ptr,
    LSA_UINT16 * lock_handle_ptr );
#endif

#ifndef PSI_FREE_REENTRANCE_LOCK
LSA_VOID PSI_FREE_REENTRANCE_LOCK(
    LSA_UINT16 * ret_val_ptr,
    LSA_UINT16   lock_handle );
#endif

#ifndef PSI_ENTER_REENTRANCE_LOCK
LSA_VOID PSI_ENTER_REENTRANCE_LOCK(
    LSA_UINT16 lock_handle );
#endif

#ifndef PSI_EXIT_REENTRANCE_LOCK
LSA_VOID PSI_EXIT_REENTRANCE_LOCK(
	LSA_UINT16 lock_handle );
#endif

/*---------------------------------------------------------------------------*/
/* Enter/exit handling (Reentrance locks) and Wait event                     */
/*---------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  PSI_ENTER
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef PSI_ENTER
    LSA_VOID PSI_ENTER( LSA_VOID );
#endif

/*=============================================================================
 * function name:  PSI_EXIT
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef PSI_EXIT
    LSA_VOID PSI_EXIT( LSA_VOID );
#endif

/*----------------------------------------------------------------------------*/
/* IR Locks                                                                   */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_IR_LOCK
LSA_VOID PSI_ALLOC_IR_LOCK( 
	LSA_UINT16   * ret_val_ptr, 
	LSA_UINT16   * lock_handle_ptr );
#endif

#ifndef PSI_FREE_IR_LOCK
LSA_VOID PSI_FREE_IR_LOCK(
	 LSA_UINT16   *ret_val_ptr,
	 LSA_UINT16   lock_handle );
#endif

#ifndef PSI_ENTER_IR_LOCK
LSA_VOID PSI_ENTER_IR_LOCK( 
	LSA_UINT16 lock_handle );
#endif

#ifndef PSI_EXIT_IR_LOCK
LSA_VOID PSI_EXIT_IR_LOCK( 
	LSA_UINT16 lock_handle );
#endif

/*----------------------------------------------------------------------------*/
/* Blocking Waits                                                             */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_BLOCKING_WAIT
LSA_VOID PSI_ALLOC_BLOCKING_WAIT(
	LSA_UINT16  *ret_val_ptr,
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_FREE_BLOCKING_WAIT
LSA_VOID PSI_FREE_BLOCKING_WAIT( 
	LSA_UINT16  *ret_val_ptr,
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_SET_BLOCKING_WAIT
LSA_VOID PSI_SET_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_RESET_BLOCKING_WAIT
LSA_VOID PSI_RESET_BLOCKING_WAIT( 
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_WAIT_BLOCKING_WAIT
LSA_VOID PSI_WAIT_BLOCKING_WAIT( 
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_WAIT_NS
LSA_VOID PSI_WAIT_NS(
    LSA_UINT64 uTimeNs );
#endif

/*----------------------------------------------------------------------------*/
/* Cache                                                                      */
/*----------------------------------------------------------------------------*/
#if (PSI_CFG_USE_NRT_CACHE_SYNC == 1)

#ifndef PSI_CACHE_SYNC
LSA_VOID PSI_CACHE_SYNC(
    LSA_VOID_PTR_TYPE   basePtr,
    LSA_UINT32          length );
#endif

#ifndef PSI_CACHE_INV
LSA_VOID PSI_CACHE_INV(
    LSA_VOID_PTR_TYPE   basePtr,
    LSA_UINT32          length );
#endif

#ifndef PSI_CACHE_WB
LSA_VOID PSI_CACHE_WB(
    LSA_VOID_PTR_TYPE   basePtr,
    LSA_UINT32          length );
#endif

#ifndef PSI_CACHE_WB_INV
LSA_VOID PSI_CACHE_WB_INV(
    LSA_VOID_PTR_TYPE   basePtr,
    LSA_UINT32          length );
#endif

#endif  // PSI_CFG_USE_NRT_CACHE_SYNC

/*----------------------------------------------------------------------------*/
/* Timer                                                                      */
/*----------------------------------------------------------------------------*/

typedef LSA_VOID (*PSI_TIMEOUT_CBF_PTR_TYPE)(LSA_TIMER_ID_TYPE, LSA_USER_ID_TYPE);

#ifndef PSI_ALLOC_TIMER_TGROUP0
LSA_VOID PSI_ALLOC_TIMER_TGROUP0(
	LSA_UINT16               * ret_val_ptr,
	LSA_TIMER_ID_TYPE        * timer_id_ptr,
	LSA_UINT16                 timer_type,
	LSA_UINT16                 time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout );
#endif

#ifndef PSI_ALLOC_TIMER_TGROUP1
LSA_VOID PSI_ALLOC_TIMER_TGROUP1(
	LSA_UINT16               * ret_val_ptr,
	LSA_TIMER_ID_TYPE        * timer_id_ptr,
	LSA_UINT16                 timer_type,
	LSA_UINT16                 time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout );
#endif

#ifndef PSI_START_TIMER
LSA_VOID PSI_START_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          time );
#endif

#ifndef PSI_STOP_TIMER
LSA_VOID PSI_STOP_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id );
#endif

#ifndef PSI_FREE_TIMER
LSA_VOID PSI_FREE_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id );
#endif

/*----------------------------------------------------------------------------*/
/*  Ticks count                                                               */
/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_TICKS_1MS
LSA_UINT32 PSI_GET_TICKS_1MS( LSA_VOID );
#endif

#ifndef PSI_GET_TICKS_100NS
LSA_UINT64 PSI_GET_TICKS_100NS( LSA_VOID );
#endif

/*----------------------------------------------------------------------------*/
/*  CRT SLOW MEM params                                                       */
/*----------------------------------------------------------------------------*/

#define PSI_HD_CRT_SLOW_MEM      	0
#define PSI_HD_CRT_SLOW_MEM_MAX		1

#ifndef PSI_CREATE_CRT_SLOW_MEM_POOL
LSA_VOID PSI_CREATE_CRT_SLOW_MEM_POOL( 
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length, 
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT           * pool_handle_ptr,
	LSA_UINT8           cp_mem_crt_slow_type );
#endif

#ifndef PSI_DELETE_CRT_SLOW_MEM_POOL
LSA_VOID PSI_DELETE_CRT_SLOW_MEM_POOL( 
	LSA_UINT16     * ret_val_ptr,
	PSI_SYS_HANDLE   sys_handle, 
	LSA_INT          pool_handle,
	LSA_UINT8        cp_mem_crt_slow_type );
#endif

#ifndef PSI_CRT_SLOW_ALLOC_MEM
LSA_VOID PSI_CRT_SLOW_ALLOC_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_CRT_SLOW_ALLOC_TX_MEM
LSA_VOID PSI_CRT_SLOW_ALLOC_TX_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_CRT_SLOW_ALLOC_RX_MEM
LSA_VOID PSI_CRT_SLOW_ALLOC_RX_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_CRT_SLOW_FREE_MEM
LSA_VOID PSI_CRT_SLOW_FREE_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_CRT_SLOW_FREE_TX_MEM
LSA_VOID PSI_CRT_SLOW_FREE_TX_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_CRT_SLOW_FREE_RX_MEM
LSA_VOID PSI_CRT_SLOW_FREE_RX_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

/*----------------------------------------------------------------------------*/
/*  HD HW params                                                              */
/*----------------------------------------------------------------------------*/
// enumeration for eddp board types
typedef enum psi_eddp_board_type_enum
{
	PSI_EDDP_BOARD_TYPE_UNKNOWN                 = 0x00, // unknown board
	PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1   = 0x01, // FPGA1-Ertec200P board Rev1
	PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV2   = 0x02, // FPGA1-Ertec200P board Rev2
	PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1  = 0x03, // EB200P board Rev1
	PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2  = 0x04, // EB200P board Rev2
	PSI_EDDP_BOARD_MAX
} PSI_EDDP_BOARD_TYPE;

// HW info for EDDI HDs (IRTE-IP boards)
typedef struct psi_hd_eddi_tag
{
	LSA_UINT16                  device_type;                    // EDD_HW_TYPE_USED_SOC, EDD_HW_TYPE_USED_ERTEC_400, EDD_HW_TYPE_USED_ERTEC_200
	PSI_HD_MEM_TYPE				irte;							// IRTE memory
	PSI_HD_MEM_TYPE				kram;							// KRAM memory (part of it used for PI)
	PSI_HD_MEM_TYPE				iocc;							// IOCC memory (used for PI on SOC)
	PSI_HD_MEM_TYPE				paea;							// PAEA memory (located at offset in IOCC used for PI)
	PSI_HD_MEM_TYPE				sdram;							// SDRAM memory uncached (used for NRT)
    PSI_HD_MEM_TYPE             sdram_cached;                   // SDRAM memory cached (used for NRT)
	PSI_HD_MEM_TYPE				shared_mem;						// Shared memory (used for NRT)
	PSI_HD_MEM_TYPE				apb_periph_scrb;				// APB-Periphericals SCRB
	PSI_HD_MEM_TYPE				apb_periph_timer;				// APB-Periphericals TIMER
    LSA_BOOL					has_ext_pll;					// EXT_PLL IF avaliable yes/no

    //SII configuration parameters
	LSA_UINT32					SII_IrqSelector;				// EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
	LSA_UINT32					SII_IrqNumber;					// EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
	LSA_UINT32					SII_ExtTimerInterval;			// 0, 250, 500, 1000 in us

    LSA_BOOL					fragmentation_supported;
    LSA_UINT8                   BC5221_MCModeSet;               // 0: Pin “MEDIA_CONV#” of a BC5221 is strapped to 1.
                                                                // 1: Pin “MEDIA_CONV#” of a BC5221 is strapped to 0.
	PSI_HD_MEM_TYPE             nrt_dmacw_default_mem;          // NRT memory used for NRT Buffer (DMACWs)
	PSI_HD_MEM_TYPE             nrt_tx_default_mem;             // NRT memory used for NRT Buffer (TX)
	PSI_HD_MEM_TYPE             nrt_rx_default_mem;             // NRT memory used for NRT Buffer (RX)
	PSI_HD_MEM_TYPE             nrt_tx_low_frag_mem;            // NRT memory used for NRT Buffer (TX fragmentation prio low, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_mid_frag_mem;            // NRT memory used for NRT Buffer (TX fragmentation prio mid, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_high_frag_mem;           // NRT memory used for NRT Buffer (TX fragmentation prio high, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_mgmtlow_frag_mem;        // NRT memory used for NRT Buffer (TX fragmentation prio MGMTLow, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_mgmthigh_frag_mem;       // NRT memory used for NRT Buffer (TX fragmentation prio MGMTHigh, used only by EDDI SOC)
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_dmacw_default;    // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_rx_default;       // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_default;       // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_low_frag;      // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mid_frag;      // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_high_frag;     // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mgmtlow_frag;  // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mgmthigh_frag; // NRT Alloc/Free 
	LSA_VOID_PTR_TYPE           g_shared_ram_base_ptr;          // Shared mem base ptr for SRD
} PSI_HD_EDDI_TYPE, * PSI_HD_EDDI_PTR_TYPE;

// HW info for EDDP HDs (PNIP boards)
typedef struct psi_hd_eddp_tag
{
	LSA_UINT16              icu_location;                   // ICU IF for EDDP setup
	LSA_UINT16              hw_type;                        // EDDP HW-type for EDDP setup
    LSA_UINT16              hw_interface;                   // EDDP Interface type: EDDP_HW_IF_A, EDDP_HW_IF_B
	LSA_UINT8               appl_timer_mode;                // EDDP IPB CRT Param ApplTimerMode
	LSA_UINT16              appl_timer_reduction_ratio;     // EDDP IPB CRT Param ApplTimerReductionRatio
	LSA_BOOL                is_transfer_end_correction_pos; // EDDP IPB CRT Param isTransferEndCorrectionPos
	LSA_UINT32              transfer_end_correction_value;  // EDDP IPB CRT Param TransferEndCorrectionValue
	PSI_EDDP_BOARD_TYPE     board_type;                     // type of board
	PSI_HD_MEM_TYPE         pnip;                           // PNIP memory
	PSI_HD_MEM_TYPE         sdram_NRT;                      // SDRAM memory NRT range
	PSI_HD_MEM_TYPE         sdram_CRT;                      // SDRAM memory CRT range (for IOBUFFER in SDRAM memory)
	PSI_HD_MEM_TYPE         perif_ram;                      // PERIF memory (for IO-Buffer in PERIF memory) 
	PSI_HD_MEM_TYPE         k32_tcm;                        // K32 memory
	PSI_HD_MEM_TYPE     	k32_Atcm;				        // K32 Atcm memory
	PSI_HD_MEM_TYPE     	k32_Btcm;				        // K32 Btcm memory
	PSI_HD_MEM_TYPE     	k32_ddr3;				        // K32 ddr3 memory
	PSI_HD_MEM_TYPE         apb_periph_scrb;                // APB-Periphericals SCRB
	PSI_HD_MEM_TYPE         apb_periph_perif;               // APB-Periphericals PERIF
	LSA_VOID_PTR_TYPE       g_shared_ram_base_ptr;          // Shared mem base ptr for SRD
} PSI_HD_EDDP_TYPE, * PSI_HD_EDDP_PTR_TYPE;

typedef PSI_HD_EDDP_TYPE const* PSI_HD_EDDP_CONST_PTR_TYPE;

// HW info for EDDS HDs
typedef struct psi_hd_edds_tag
{
	LSA_VOID_PTR_TYPE       ll_handle;                  // LL handle (depends on LL adaption)
	LSA_VOID_PTR_TYPE       ll_function_table;          // LL function table (depends on LL adaption)
	LSA_VOID_PTR_TYPE       ll_parameter;               // LL parameter (depends on LL adaption)
	LSA_VOID_PTR_TYPE       io_buffer_handle;           // IO buffer handle for this HD
    #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
    LSA_BOOL                bHsyncModeActive;           // Hsync Mode Active
    LSA_UINT16              mbx_id_edds_rqb;            // MBX ID for EDDS RQB task (from upper to lower)
    #endif
} PSI_HD_EDDS_TYPE, * PSI_HD_EDDS_PTR_TYPE;

// HW info for EDDT HDs (TI boards)
typedef struct psi_hd_eddt_tag
{
	PSI_HD_MEM_TYPE         sdram_NRT;                  // SDRAM memory NRT range
	PSI_HD_MEM_TYPE         sdram_CRT;                  // SDRAM memory CRT range
	PSI_HD_MEM_TYPE         pnunit_mem;                 // PNUNIT memory
} PSI_HD_EDDT_TYPE, * PSI_HD_EDDT_PTR_TYPE;

// HD information for this PN board (EDD specific)
typedef union psi_hd_edd_type
{
	PSI_HD_EDDI_TYPE        eddi;
	PSI_HD_EDDP_TYPE        eddp;
	PSI_HD_EDDS_TYPE        edds;
    PSI_HD_EDDT_TYPE        eddt;
} PSI_HD_EDD_TYPE, * PSI_HD_EDD_PTR_TYPE;

// User to HW port mapping information
typedef struct psi_hd_usr_port_map_tag
{
	LSA_UINT16              hw_port_id;                 // HW port number
	LSA_UINT16              hw_phy_nr;                  // PHY nr for HW port
} PSI_HD_PORT_MAP_TYPE, * PSI_HD_PORT_MAP_PTR_TYPE;

// Note: the adaption HW info is used for the EDDx setup and adaption functions
// HW information (from system) for one HD
typedef struct psi_get_hd_param_tag
{
	PSI_SYS_HANDLE          hd_sys_handle;                      // hSysDev for HD
	PSI_MAC_TYPE            if_mac;                             // IF MAC from system
	PSI_MAC_TYPE            port_mac[PSI_CFG_MAX_PORT_CNT];     // all port MACs from system
	PSI_HD_PORT_MAP_TYPE    port_map[PSI_CFG_MAX_PORT_CNT+1];   // User to HW port mapping (port index 0 is reserved) for this IF
	LSA_UINT16              edd_type;                           // EDD HW info belongs to EDD type (Comp-ID)
	PSI_HD_MEM_TYPE         hif_mem;                            // HIF memory
	PSI_HD_MEM_TYPE         pi_mem;                             // PI memory used for CRT (SDRam, KRAM, IOCC)
	PSI_HD_MEM_TYPE         dev_mem;                            // DEV memory
	PSI_HD_MEM_TYPE         nrt_tx_mem;                         // NRT TX memory used for NRT Buffer (SDRam, SHM)
	PSI_HD_MEM_TYPE         nrt_rx_mem;							// NRT RX memory used for NRT Buffer (SDRam, SHM)
	PSI_HD_MEM_TYPE         crt_slow_mem;                       // CRT memory on Sdram
	PSI_HD_EDD_TYPE         edd;                                // HD information for EDDx HD
} PSI_HD_PARAM_TYPE, * PSI_HD_PARAM_PTR_TYPE;

// HD information for SYS adaption 
typedef struct psi_get_hd_sys_id_tag
{
	PSI_HD_ID_TYPE          hd_location;                // HD address information (PCI, MAC)
	LSA_UINT16              asic_type;                  // Expected asic type
	LSA_UINT16              rev_nr;                     // Expected rev nr
	LSA_UINT16              edd_type;                   // Expected type of EDDx
} PSI_HD_SYS_ID_TYPE, * PSI_HD_SYS_ID_PTR_TYPE;

typedef PSI_HD_SYS_ID_TYPE const* PSI_HD_SYS_ID_CONST_PTR_TYPE;

/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_HD_PARAM
LSA_VOID PSI_GET_HD_PARAM(
	LSA_UINT16                      *ret_val_ptr,       // Getting state
    PSI_HD_INPUT_CONST_PTR_TYPE     hd_ptr,             // HD input configuration for one HD
    PSI_HD_SYS_ID_CONST_PTR_TYPE    hd_sys_id_ptr,      // HD address params for SYS adaption
	PSI_HD_PARAM_PTR_TYPE           hd_param_ptr        // HD HW params from system
);
#endif

#ifndef PSI_LD_GET_HD_PARAM
LSA_VOID PSI_LD_GET_HD_PARAM(
    LSA_UINT16             *        ret_val_ptr,        // Getting state
    LSA_UINT16                      hd_id,              // HD number [1..N]
    PSI_HD_PARAM_PTR_TYPE           hd_param_ptr        // HD HW params from system
);
#endif

#ifndef PSI_LD_UPDATE_HD_PARAM
LSA_VOID PSI_LD_UPDATE_HD_PARAM(
    LSA_UINT16             *        ret_val_ptr,        // Getting state
    LSA_UINT16                      hd_id,              // HD number [1..N]
    PSI_HD_PARAM_PTR_TYPE           hd_param_ptr        // HD HW params from system
);
#endif

#ifndef PSI_FREE_HD_PARAM
LSA_VOID PSI_FREE_HD_PARAM(
	LSA_UINT16            * ret_val_ptr,                // Getting state
	LSA_UINT16              hd_id                       // HD number [1..N]
);
#endif

#ifndef PSI_LD_CLOSED_HD
LSA_VOID PSI_LD_CLOSED_HD(
	LSA_UINT16              hd_id                       // HD number [1..N]
);
#endif

/*----------------------------------------------------------------------------*/

typedef struct psi_hif_sys_ptr_tag {
	LSA_UINT16              hd_nr;              // HD number [1..N]
	LSA_BOOL                is_upper;           // Upper/Lower Hif instance
	LSA_BOOL                shortcut_mode;      // Upper/Lower in shortcut mode
    PSI_LD_RUNS_ON_TYPE     ldRunsOnType;       // Upper/Lower LD Runs on HW
	LSA_UINT16              hif_lower_handle;   // HIF lower handle (only used for Upper OPEN)
	PSI_HD_SYS_ID_TYPE      hd_sys_id;          // HD location information (for real Open HD)
} PSI_HIF_SYS_TYPE, * PSI_HIF_SYS_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/*  HD IR adaption (PNIO IR)                                                  */
/*----------------------------------------------------------------------------*/

#ifndef PSI_HD_ENABLE_EVENT
LSA_VOID PSI_HD_ENABLE_EVENT(
	PSI_SYS_HANDLE sys_handle );
#endif

#ifndef PSI_HD_DISABLE_EVENT
LSA_VOID PSI_HD_DISABLE_EVENT(
	PSI_SYS_HANDLE sys_handle );
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
/*----------------------------------------------------------------------------*/
/*  PSI EDDx Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDD_NRT_SEND_HOOK
LSA_VOID PSI_EDD_NRT_SEND_HOOK(
	PSI_SYS_HANDLE          hSysDev,
	LSA_VOID_PTR_TYPE       ptr_upper_rqb );
#endif

#ifndef PSI_EDD_IS_PORT_PULLED
LSA_BOOL PSI_EDD_IS_PORT_PULLED(
    PSI_SYS_HANDLE     hSysDev,
    LSA_UINT32         HwPortID );
#endif

#ifndef PSI_EDD_GET_MEDIA_TYPE
LSA_VOID PSI_EDD_GET_MEDIA_TYPE(
    PSI_SYS_HANDLE    hSysDev,
    LSA_UINT32        HwPortID, 
    LSA_UINT8         isPofAutoDetection,
    LSA_UINT8       * pMediaType,
    LSA_UINT8       * pIsPOF,
    LSA_UINT8       * pPhyType,
    LSA_UINT8       * pFXTransceiverType);
#endif

#endif

#if (PSI_CFG_USE_EDDI == 1) // EDDI used 
/*----------------------------------------------------------------------------*/
/*  PSI EDDI Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_XPLL_SETPLLMODE_OUT
LSA_VOID PSI_EDDI_LL_XPLL_SETPLLMODE_OUT(
	LSA_RESULT            * result,
	PSI_SYS_HANDLE          hSysDev,
	LSA_UINT32              pIRTE,
	LSA_UINT32              location,
	LSA_UINT16              pllmode );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_BEGIN
LSA_VOID PSI_EDDI_LL_LED_BLINK_BEGIN(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT32      HwPortIndex,
	LSA_UINT32      PhyTransceiver );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_END
LSA_VOID PSI_EDDI_LL_LED_BLINK_END(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT32      HwPortIndex,
	LSA_UINT32      PhyTransceiver );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_SET_MODE
LSA_VOID PSI_EDDI_LL_LED_BLINK_SET_MODE(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT32      HwPortIndex,
	LSA_UINT32      PhyTransceiver,
	LSA_UINT16      LEDMode );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA
LSA_UINT32 PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA(
	PSI_SYS_HANDLE            hSysDev,
	LSA_VOID           const* p,
	LSA_UINT32         const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC( 
	PSI_SYS_HANDLE           hSysDev,
	LSA_VOID          const* p,
	LSA_UINT32        const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID             const* p,
	LSA_UINT32           const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_SHAREDMEM_OFFSET
LSA_UINT32 PSI_EDDI_DEV_SHAREDMEM_OFFSET( 
	PSI_SYS_HANDLE           hSysDev,
	LSA_VOID          const* p,
	LSA_UINT32        const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL
LSA_UINT32 PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(
	PSI_SYS_HANDLE           hSysDev,
	LSA_UINT32         const p,
	LSA_UINT32         const location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE( 
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          CycleBaseFactor,
	LSA_UINT8           Mode );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED
LSA_VOID PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED( 
	PSI_EDD_HDDB        hDDB );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED
LSA_VOID PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED(
	PSI_EDD_HDDB hDDB );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SCL_LOW_HIGHZ
LSA_VOID PSI_EDDI_I2C_SCL_LOW_HIGHZ(
	PSI_SYS_HANDLE       hSysDev,
	LSA_UINT8      const Level );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_LOW_HIGHZ
LSA_VOID PSI_EDDI_I2C_SDA_LOW_HIGHZ(
	PSI_SYS_HANDLE       hSysDev,
    LSA_UINT8      const Level );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_READ
LSA_VOID PSI_EDDI_I2C_SDA_READ( 
	PSI_SYS_HANDLE    hSysDev ,
	LSA_UINT8         *const  value_ptr );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SELECT
LSA_VOID PSI_EDDI_I2C_SELECT( 
    LSA_UINT8       * const  ret_val_ptr,
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      const  PortId,
	LSA_UINT16      const  I2CMuxSelect );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC
LSA_VOID PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC( 
	LSA_RESULT       * const  ret_val_ptr,
	PSI_SYS_HANDLE   hSysDev,
	LSA_UINT8        const  I2CDevAddr,
	LSA_UINT8        const  I2COffsetCnt,
	LSA_UINT8        const  I2COffset1,
	LSA_UINT8        const  I2COffset2,
	LSA_UINT32       const  Size,
	LSA_UINT8       *const  pBuf );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_I2C_READ_OFFSET_SOC
LSA_VOID PSI_EDDI_LL_I2C_READ_OFFSET_SOC( 
	LSA_RESULT       * const  ret_val_ptr,
	PSI_SYS_HANDLE   hSysDev,
	LSA_UINT8        const  I2CDevAddr,
	LSA_UINT8        const  I2COffsetCnt,
	LSA_UINT8        const  I2COffset1,
	LSA_UINT8        const  I2COffset2,
	LSA_UINT32       const  Size,
	LSA_UINT8       *const  pBuf );
#endif

/*----------------------------------------------------------------------------*/

//ExtTimer commands
#define PSI_EDDI_EXTTIMER_CMD_INIT_START    1
#define PSI_EDDI_EXTTIMER_CMD_STOP          2

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_EXT_TIMER_CMD
LSA_VOID PSI_EDDI_EXT_TIMER_CMD(
	PSI_SYS_HANDLE  const hSysDev, 
	LSA_UINT8       const Cmd,
	PSI_EDD_HDDB    const hDDB );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM
LSA_VOID PSI_EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM( 
    PSI_SYS_HANDLE                              hSysDev,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID);
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_FREE_DEV_SDRAM_ERTEC_MEM
LSA_VOID PSI_EDDI_FREE_DEV_SDRAM_ERTEC_MEM( 
    PSI_SYS_HANDLE                              hSysDev,
    LSA_RESULT *                                ret_val_ptr,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID);
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_ALLOC_DEV_SHARED_MEM
LSA_VOID PSI_EDDI_ALLOC_DEV_SHARED_MEM(
    PSI_SYS_HANDLE                              hSysDev,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID);
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_FREE_DEV_SHARED_MEM
LSA_VOID PSI_EDDI_FREE_DEV_SHARED_MEM(
    PSI_SYS_HANDLE                              hSysDev,
    LSA_RESULT *                                ret_val_ptr,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID);
#endif

#endif  // EDDI HW adaption

#if (PSI_CFG_USE_EDDP == 1) // EDDP used 
/*----------------------------------------------------------------------------*/
/*  PSI EDDP Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDP_ALLOC_UPPER_RX_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_RX_MEM(
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID_PTR_TYPE  *upper_mem_ptr_ptr,
	LSA_UINT32         length );
#endif

#ifndef PSI_EDDP_FREE_UPPER_RX_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDP_ALLOC_UPPER_TX_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_TX_MEM(
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID_PTR_TYPE  *upper_mem_ptr_ptr,
	LSA_UINT32         length );
#endif

#ifndef PSI_EDDP_FREE_UPPER_TX_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDP_ALLOC_UPPER_DEV_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_DEV_MEM(
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID_PTR_TYPE  *upper_mem_ptr_ptr,
	LSA_UINT32         length );
#endif

#ifndef PSI_EDDP_FREE_UPPER_DEV_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDP_RX_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_RX_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_TX_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_TX_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_DEV_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_DEV_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_IO_EXTRAM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_IO_EXTRAM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_IO_PERIF_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_IO_PERIF_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_TEST_WRITE_REG
LSA_VOID PSI_EDDP_TEST_WRITE_REG(
	PSI_SYS_HANDLE     hSysDev,
	LSA_UINT32         Reg,
	LSA_UINT32         Value );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_K32_RESET
LSA_VOID PSI_EDDP_K32_RESET(
	PSI_SYS_HANDLE      hSysDev,
	LSA_BOOL            on );
#endif

/*----------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NEC_MAU_TYPE_INTERNAL
LSA_VOID PSI_EDDP_PHY_NEC_GET_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT32              Speed,
    LSA_UINT32              Duplexity,
    LSA_UINT16              *pMauType );

/*---------------------------------------------------------------------------*/
LSA_VOID PSI_EDDP_PHY_NEC_CHECK_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT16              MAUType,
    LSA_UINT32              *pSpeed,
    LSA_UINT32              *pDuplexity );
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NEC_LED_BLINK_INTERNAL
LSA_VOID PSI_EDDP_PHY_NEC_LED_BlinkBegin(
	PSI_EDD_HDDB 	    hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID );

/*---------------------------------------------------------------------------*/
LSA_VOID PSI_EDDP_PHY_NEC_LED_BlinkSetMode(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode );

/*---------------------------------------------------------------------------*/
LSA_VOID PSI_EDDP_PHY_NEC_LED_BlinkEnd(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID );
#endif

/*----------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NSC_MAU_TYPE_INTERNAL
LSA_VOID PSI_EDDP_PHY_NSC_GET_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT32              Speed,
    LSA_UINT32              Duplexity,
    LSA_UINT16              *pMauType );

LSA_VOID PSI_EDDP_PHY_NSC_CHECK_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT16              MAUType,
    LSA_UINT32            * pSpeed,
    LSA_UINT32            * pDuplexity );
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NSC_LED_BLINK_INTERNAL
LSA_VOID PSI_EDDP_PHY_NSC_LED_BlinkBegin(
    PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          HwPortID );

/*---------------------------------------------------------------------------*/
LSA_VOID PSI_EDDP_PHY_NSC_LED_BlinkSetMode(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode );

/*---------------------------------------------------------------------------*/
LSA_VOID PSI_EDDP_PHY_NSC_LED_BlinkEnd(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          HwPortID );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDP_SIGNAL_SENDCLOCK_CHANGE( 
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          CycleBaseFactor,
	LSA_UINT8           Mode );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_I2C_SELECT
LSA_VOID PSI_EDDP_I2C_SELECT(
	LSA_UINT8      * const ret_val_ptr,
	PSI_SYS_HANDLE         hSysDev,
	LSA_UINT16       const PortId,
	LSA_UINT16       const I2CMuxSelect );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_K32_RELEASE_RESET_HERA
LSA_VOID PSI_EDDP_K32_RELEASE_RESET_HERA( 
	PSI_SYS_HANDLE			hSysDev );
#endif

/*----------------------------------------------------------------------------*/
#endif  // EDDP HW adaption

#if (PSI_CFG_USE_EDDS == 1) //EDDS Used
/*----------------------------------------------------------------------------*/
/*  PSI EDDS Interface                                                        */
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR_DEV_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_DEV_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR64_DEV_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_DEV_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT64        * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR_TX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_TX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR64_TX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_TX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT64        * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR_RX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_RX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR64_RX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_RX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT64        * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#else
#error "by design a function!"
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_ALLOC_DEV_BUFFER_MEM
LSA_VOID PSI_EDDS_ALLOC_DEV_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_FREE_DEV_BUFFER_MEM
LSA_VOID PSI_EDDS_FREE_DEV_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_NOTIFY_FILL_SETTINGS
LSA_BOOL PSI_EDDS_NOTIFY_FILL_SETTINGS(
    PSI_SYS_HANDLE const hSysDev,
    LSA_BOOL       const bFillActive,
    LSA_BOOL       const bIOConfigured,
    LSA_BOOL       const bHsyncModeActive );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_DO_NOTIFY_SCHEDULER
LSA_VOID PSI_EDDS_DO_NOTIFY_SCHEDULER(
    PSI_SYS_HANDLE const hSysDev );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_DO_RETRIGGER_SCHEDULER
LSA_VOID PSI_EDDS_DO_RETRIGGER_SCHEDULER(
    PSI_SYS_HANDLE const hSysDev );
#endif

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDS

#if (PSI_CFG_USE_EDDT == 1) // EDDT used 
/*----------------------------------------------------------------------------*/
/*  PSI EDDT Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDT_ALLOC_UPPER_RX_MEM
LSA_VOID PSI_EDDT_ALLOC_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif

#ifndef PSI_EDDT_FREE_UPPER_RX_MEM
LSA_VOID PSI_EDDT_FREE_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDT_ALLOC_UPPER_TX_MEM
LSA_VOID PSI_EDDT_ALLOC_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif

#ifndef PSI_EDDT_FREE_UPPER_TX_MEM
LSA_VOID PSI_EDDT_FREE_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDT_ALLOC_UPPER_DEV_MEM
LSA_VOID PSI_EDDT_ALLOC_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif

#ifndef PSI_EDDT_FREE_UPPER_DEV_MEM
LSA_VOID PSI_EDDT_FREE_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDT_ALLOC_LOWER_DEV_MEM
LSA_VOID PSI_EDDT_ALLOC_LOWER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length );
#endif

#ifndef PSI_EDDT_FREE_LOWER_DEV_MEM
LSA_VOID PSI_EDDT_FREE_LOWER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDT_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDT_SIGNAL_SENDCLOCK_CHANGE(
    PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     CycleBaseFactor,
	LSA_UINT8      Mode );
#endif

/*----------------------------------------------------------------------------*/
#endif  // EDDT HW adaption

#if (( PSI_CFG_USE_POF == 1 ) && (PSI_CFG_USE_HD_COMP == 1)) // POF used 
/*----------------------------------------------------------------------------*/
/*  PSI POF Interface                                                         */
/*----------------------------------------------------------------------------*/

#ifndef PSI_POF_FO_LED
LSA_VOID PSI_POF_FO_LED(
	LSA_UINT16        const PortID,
	LSA_UINT8         const ON,
	PSI_SYS_HANDLE    hSysDev );
#endif

/*----------------------------------------------------------------------------*/
#endif  // POF HW adaption

#if (PSI_CFG_USE_HIF == 1) // HIF used 
/*----------------------------------------------------------------------------*/
/*  PSI HIF Interface                                                         */
/*----------------------------------------------------------------------------*/

/* HIF LD */

#ifndef PSI_HIF_GET_LD_UPPER_HANDLE
LSA_VOID PSI_HIF_GET_LD_UPPER_HANDLE( 
	LSA_UINT16  *hif_handle_ptr );
#endif

#ifndef PSI_HIF_LD_U_GET_PARAMS
LSA_VOID PSI_HIF_LD_U_GET_PARAMS( 
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars );
#endif

#ifndef PSI_HIF_LD_L_GET_PARAMS
LSA_VOID PSI_HIF_LD_L_GET_PARAMS( 
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars );
#endif

#ifndef PSI_HIF_LD_ENABLE_ISR
LSA_VOID PSI_HIF_LD_ENABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_LD_DISABLE_ISR
LSA_VOID PSI_HIF_LD_DISABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_LD_SEND_IRQ
LSA_VOID PSI_HIF_LD_SEND_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef  PSI_HIF_LD_ACK_IRQ
LSA_VOID PSI_HIF_LD_ACK_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_LD_REQUEST_UPPER_DONE
LSA_VOID PSI_HIF_LD_REQUEST_UPPER_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_request_upper_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb, 
	LSA_SYS_PTR_TYPE                 sys_ptr );
#endif

#ifndef PSI_HIF_LD_L_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_LD_L_SYSTEM_REQUEST_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb );
#endif

#ifndef PSI_HIF_LD_U_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_LD_U_SYSTEM_REQUEST_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE   hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                 ptr_upper_rqb );
#endif

/* HIF HD */

#ifndef PSI_HIF_GET_HD_LOWER_HANDLE
LSA_VOID PSI_HIF_GET_HD_LOWER_HANDLE( 
	LSA_UINT16      *hif_handle_ptr,
	LSA_UINT16      hd_id );
#endif

#ifndef PSI_HIF_HD_U_GET_PARAMS
LSA_VOID PSI_HIF_HD_U_GET_PARAMS( 
	LSA_UINT16            *ret_val_ptr,
	PSI_SYS_HANDLE        hSysDev,
	LSA_VOID_PTR_TYPE     pPars );
#endif

#ifndef PSI_HIF_HD_L_GET_PARAMS
LSA_VOID PSI_HIF_HD_L_GET_PARAMS( 
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars );
#endif

#ifndef PSI_HIF_HD_ENABLE_ISR
LSA_VOID PSI_HIF_HD_ENABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_HD_DISABLE_ISR
LSA_VOID PSI_HIF_HD_DISABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_HD_SEND_IRQ
LSA_VOID PSI_HIF_HD_SEND_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef  PSI_HIF_HD_ACK_IRQ
LSA_VOID PSI_HIF_HD_ACK_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_HD_L_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_HD_L_SYSTEM_REQUEST_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_MEMCPY_BUF_DMA
LSA_VOID PSI_HIF_MEMCPY_BUF_DMA(
    LSA_VOID_PTR_TYPE dst,
    LSA_VOID const*   src,
    LSA_UINT32        size,
    PSI_SYS_HANDLE    sys_handle );
#endif

#ifndef PSI_HIF_MEMCPY_BUF
LSA_VOID PSI_HIF_MEMCPY_BUF(
    LSA_VOID_PTR_TYPE dst,
    LSA_VOID const*   src,
    LSA_UINT32        size );
#endif

#ifndef PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED
LSA_VOID PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED(
    LSA_VOID const* src );
#endif

#ifndef PSI_HIF_BUF_DMA_PEND_FREE
LSA_BOOL PSI_HIF_BUF_DMA_PEND_FREE(
    LSA_VOID_PTR_TYPE   buffer,
    LSA_VOID_PTR_TYPE * pReadyForFree,
    PSI_SYS_HANDLE      sys_handle );
#endif

#endif  // HIF adaption (Application or FW System)

typedef struct psi_sock_name_tag  /* Note see sock_int.h for SOCK Type */
{
    LSA_UINT8   *pName;
    LSA_UINT16   Length;
    LSA_BOOL     Remanent;
} PSI_SOCK_NAME_TYPE;

#if (( PSI_CFG_USE_LD_COMP == 1 ) && ( PSI_CFG_USE_TCIP == 0 )) // SOCK without TCIP used 
/*----------------------------------------------------------------------------*/
/*  PSI SOCK not TCIP Interface                                               */
/*----------------------------------------------------------------------------*/

#ifndef PSI_SOCK_SET_IPSUITE
LSA_BOOL PSI_SOCK_SET_IPSUITE(
	LSA_UINT32  IfID,
	LSA_UINT32  ip_addr,
	LSA_UINT32  net_mask,
	LSA_UINT32  gateway ); 
#endif

#ifndef PSI_SOCK_SET_MIB2_IFGROUP_DATA
LSA_BOOL PSI_SOCK_SET_MIB2_IFGROUP_DATA(
	LSA_UINT32          IfID,
	LSA_UINT16          PortId, 
	PSI_SOCK_NAME_TYPE  IfDescr );
#endif

#ifndef PSI_SOCK_SET_MIB2_SYSGROUP_DATA
LSA_BOOL PSI_SOCK_SET_MIB2_SYSGROUP_DATA( 
	PSI_SOCK_NAME_TYPE SysName,
	PSI_SOCK_NAME_TYPE SysDescr,
	PSI_SOCK_NAME_TYPE SysContact,
	PSI_SOCK_NAME_TYPE SysLocation );
#endif

#ifndef PSI_SOCK_GET_MIB2_IFINDEX
LSA_BOOL PSI_SOCK_GET_MIB2_IFINDEX(
	LSA_UINT32    IfID, 
	LSA_UINT16    PortID, 
	LSA_UINT32*   p_IfIndex );
#endif

#ifndef PSI_SOCK_TCPIP_SET_TIMEPARAMS
LSA_BOOL PSI_SOCK_TCPIP_SET_TIMEPARAMS(
	LSA_UINT32  KeepAliveTime,
	LSA_UINT32  ConnectionTimeout,
	LSA_UINT32  RetransTimeout,
	LSA_UINT32  RetransCount );
#endif

#ifndef PSI_SOCK_TCPIP_GET_TIMEPARAMS
LSA_BOOL PSI_SOCK_TCPIP_GET_TIMEPARAMS(
	LSA_UINT32  *KeepAliveTime,
	LSA_UINT32  *ConnectionTimeout,
	LSA_UINT32  *RetransTimeout,
	LSA_UINT32  *RetransCount );
#endif

#ifndef PSI_SOCK_SNMP_AGT_PARSE
LSA_UINT16 PSI_SOCK_SNMP_AGT_PARSE(
    LSA_UINT8  *precv_buffer,
    LSA_UINT    len,
    LSA_UINT8  *psnmp_buffer,
    LSA_UINT    snmp_len );
#endif

/*----------------------------------------------------------------------------*/
#endif  // SOCK no TCIP adaption (Application or FW System)

#if (( PSI_CFG_USE_IOH == 1 ) && (PSI_CFG_USE_LD_COMP == 1)) // IOH used 
/*----------------------------------------------------------------------------*/
/*  PSI IOH Interface                                                         */
/*----------------------------------------------------------------------------*/

#ifndef PSI_IOH_ALLOC_FAST_MEM
LSA_VOID PSI_IOH_ALLOC_FAST_MEM(
	LSA_VOID   ** local_mem_ptr_ptr,
	LSA_UINT32    length );
#endif

#ifndef PSI_IOH_FREE_FAST_MEM
LSA_VOID PSI_IOH_FREE_FAST_MEM(
	LSA_UINT16 * ret_val_ptr,
	LSA_VOID   * local_mem_ptr );
#endif

#ifndef PSI_IOH_ENABLE_ISOCHRONOUS
LSA_RESULT PSI_IOH_ENABLE_ISOCHRONOUS(
	PSI_SYS_HANDLE   hSysDev,
	LSA_VOID (*func_ptr)(LSA_INT params, LSA_VOID* args),
	LSA_INT32        param,
	LSA_VOID       * args );
#endif

#ifndef PSI_IOH_DISABLE_ISOCHRONOUS
LSA_RESULT PSI_IOH_DISABLE_ISOCHRONOUS(
	PSI_SYS_HANDLE hSysDev );
#endif

#ifndef PSI_IOH_DEV_MEM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_IOH_DEV_MEM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE         hSysDev,     //device info
	LSA_VOID       const*  p,           //virtual address
  	LSA_BOOL               bShared );   //LSA_TRUE: calculated for SharedMEM, LSA_FALSE: calculated for LocalMEM
#endif

#ifndef PSI_IOH_DEV_IS_IOCC_ACCESSED_LOCALLY
LSA_BOOL PSI_IOH_DEV_IS_IOCC_ACCESSED_LOCALLY(
    PSI_SYS_HANDLE hSysDev,             //device info
    LSA_UINT32     iocc_base_addr );
#endif

/*----------------------------------------------------------------------------*/
#endif  // IOH adaption (Application or FW System)

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PSI_SYS_H_ */

