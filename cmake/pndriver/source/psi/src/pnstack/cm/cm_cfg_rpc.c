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
/*  F i l e               &F: cm_cfg_rpc.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CM using PSI.                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1209
#define PSI_MODULE_ID       1209 /* PSI_MODULE_ID_CM_CFG_RPC */

#include "psi_int.h"

#if (PSI_CFG_USE_HD_COMP == 1)

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*------------------------------------------------------------------------------
//	Lower Layer
//----------------------------------------------------------------------------*/

#ifndef CM_RPC_OPEN_CHANNEL_LOWER
LSA_VOID CM_RPC_OPEN_CHANNEL_LOWER(
	CM_RPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_RPC_OPEN_CHANNEL_LOWER" );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_CLRPC );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( (PSI_CFG_USE_HIF_HD == 1) && (PSI_CFG_USE_LD_COMP == 1) )
    if(pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    // post the message to HIF HD LOWER (CLRPC is located on LD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_open_channel, lower_rqb_ptr );
    }
    else
    {
	    // post the message to CLRPC (without HIF CLRPC is located in same system)
	    psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, lower_rqb_ptr );
    }
    #elif ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD LOWER (CLRPC is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_open_channel, lower_rqb_ptr );
    #elif ( PSI_CFG_USE_LD_COMP == 1 )
	// post the message to CLRPC (without HIF CLRPC is located in same system)
	psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, lower_rqb_ptr );
    #else
    #error "No possibility to open channel to clrpc"
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_CLOSE_CHANNEL_LOWER
LSA_VOID CM_RPC_CLOSE_CHANNEL_LOWER(
	CM_RPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_RPC_CLOSE_CHANNEL_LOWER" );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_CLRPC );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( (PSI_CFG_USE_HIF_HD == 1) && (PSI_CFG_USE_LD_COMP == 1) )
    if(pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    // post the message to HIF HD LOWER (CLRPC is located on LD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_close_channel, lower_rqb_ptr );
    }
    else
    {
	    // post the message to CLRPC (without HIF CLRPC is located in same system)
	    psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, lower_rqb_ptr );
    }
    #elif ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD LOWER (CLRPC is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_close_channel, lower_rqb_ptr );
    #elif ( PSI_CFG_USE_LD_COMP == 1 )
	// post the message to CLRPC (without HIF CLRPC is located in same system)
	psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, lower_rqb_ptr );
    #else
    #error "No posibility to close channel to clrpc."
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_REQUEST_LOWER
LSA_VOID CM_RPC_REQUEST_LOWER(
	CM_RPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"CM_RPC_REQUEST_LOWER(): rqb(0x%08x) h(%d) opc(%d)",
		lower_rqb_ptr,
		CM_RPC_GET_HANDLE(lower_rqb_ptr),
		CM_RPC_GET_OPCODE(lower_rqb_ptr) );

	PSI_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_CLRPC );
    PSI_ASSERT( pSys->hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( (PSI_CFG_USE_HIF_HD == 1) && (PSI_CFG_USE_LD_COMP == 1) )
    if(pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    // post the message to HIF HD LOWER (CLPRC is located on LD side)
	    psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_request, lower_rqb_ptr );
    }
    else
    {
	    // post the message to CLRPC (without HIF CLRPC is located in same system)
	    psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, lower_rqb_ptr );
    }
    #elif ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD LOWER (CLPRC is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_request, lower_rqb_ptr );
    #elif ( PSI_CFG_USE_LD_COMP == 1 )
	// post the message to CLRPC (without HIF CLRPC is located in same system)
	psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, lower_rqb_ptr );
    #else
    #error "No posibility to post messages to clrpc."
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_ALLOC_LOWER_RQB
LSA_VOID CM_RPC_ALLOC_LOWER_RQB(
	CM_RPC_LOWER_RQB_PTR_TYPE * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE            user_id,
	LSA_UINT16                  length,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

    if (length != sizeof(CM_RPC_LOWER_RQB_TYPE))
    {
		PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "CM_RPC_ALLOC_LOWER_RQB(): invalid length(%u) != sizeof(CM_RPC_LOWER_RQB_TYPE)", length );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_RQB_CLRPC );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_RPC_ALLOC_LOWER_RQB(): ptr(0x%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_FREE_LOWER_RQB
LSA_VOID CM_RPC_FREE_LOWER_RQB(
	LSA_UINT16                * ret_val_ptr,
	CM_RPC_LOWER_RQB_PTR_TYPE   lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_RPC_FREE_LOWER_RQB(): ptr(0x%08x)", lower_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_RQB_CLRPC );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_ALLOC_LOWER_MEM
LSA_VOID CM_RPC_ALLOC_LOWER_MEM(
	CM_RPC_LOWER_MEM_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE            user_id,
	LSA_UINT16                  length,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_MEM_CLRPC );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_RPC_ALLOC_LOWER_MEM(): ptr(0x%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RPC_FREE_LOWER_MEM
LSA_VOID CM_RPC_FREE_LOWER_MEM(
	LSA_UINT16                * ret_val_ptr,
	CM_RPC_LOWER_MEM_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_RPC_FREE_LOWER_MEM(): ptr(0x%08x)", lower_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_MEM_CLRPC );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#endif // PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
