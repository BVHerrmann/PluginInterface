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
/*  F i l e               &F: eps_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Impelements the LSA Componenent Interface.                               */
/*	Even though eps is not an LSA component we want to use its interface to  */ 
/*  communicate with other instances of eps.                                 */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID   20065

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "eps_sys.h"
#include "eps_trc.h"
#include "eps_locks.h"
//lint --e(537) repeated include file. Intended.
#include "eps_usr.h"
/*===========================================================================*/
/*                                 variables                                 */
/*===========================================================================*/

///Lock-ID for the reentrace protection
static LSA_UINT16 eps_rqb_enter_exit_id = PSI_LOCK_ID_INVALID;

///Variable to store the information necessary for channel management 
EPS_RQB_INTERFACE_DATA_TYPE eps_rqb_interface_data;

///Data-structure of the memory statistics
extern EPS_MEM_STATISTIC_TYPE g_EpsMemStat[EPS_LSA_COMP_ID_SUM+1][EPS_LSA_COUNT_MEM_TYPES + 1];

/*===========================================================================*/
/*                                 helper functions                          */
/*===========================================================================*/

// pointer compare macros
#define IS_NULL(ptr_)			(LSA_HOST_PTR_ARE_EQUAL(EPS_TEST_POINTER(ptr_), LSA_NULL))
#define IS_FUNCTION_NULL(fct_ptr_) (fct_ptr_ == EPS_FCT_PTR_NULL)                                     
#define IS_NOT_NULL(ptr_)		(! LSA_HOST_PTR_ARE_EQUAL(EPS_TEST_POINTER(ptr_), LSA_NULL))

/**
 * @brief Resets a channel in the eps_rqb_interface_data.channels array.
 *        This function sets the channel to a "free" state so that it may be used again.
 * 
 * @param channel_index The index of the channel in the eps_rqb_interface_data.channels array
 */
static LSA_VOID reset_channel(LSA_HANDLE_TYPE channel_index)
{
	EPS_CHANNEL_PTR_TYPE channel;
	EPS_ASSERT(channel_index < EPS_CFG_MAX_CHANNELS);

	//Reset the channel
	channel = &eps_rqb_interface_data.channels[channel_index];
	channel->callback     = LSA_NULL;
	channel->detailptr    = LSA_NULL;
	channel->handle_upper = 0;
	channel->state        = EPS_CHANNEL_STATE_FREE;
}

#ifdef EPS_USE_MEM_STATISTIC
/**
* @brief collects the summarized memory statistics from eps_mem.c.
* This function may only be used if EPS_USE_MEM_STATISTIC is set.
* @param pStats                         - pointer to wrapper structure
* @param pStats->sum_alloc_count        - sum of all allocs overwall                (= how much element were allocated overall)
* @param pStats->sum_act_alloc_count    - sum of all current allocs in all pools    (= how much element are in use)
* @param pStats->max_alloc_size         - maximum element                           (= biggest element)
* @param pStats->sum_act_alloc_size     - sum of the sizes of all allocated blocks  (= usage)
*/
static LSA_VOID eps_mem_get_collected_memstats(EPS_MEM_STAT_PTR_TYPE pStats, LSA_UINT16 comp_id)
{        
    pStats->sum_alloc_count         = g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uAllocCount;
    pStats->sum_act_alloc_count     = g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount;
    pStats->max_alloc_size          = g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize;
    pStats->sum_act_alloc_size      = g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize;
}
#endif
/**
 * @brief This function is called by the user the to tell eps to process a RQB.
 *        It is mandatory that the channel identified by rqb->_handle is opened.
 *         
 *        At first the RQB is checked for obvious parameterization errors.
 *        After that the action described through the RQB is performed.
 * 
 * @param pRQB Pointer to a RQB which specifies the action that shall be performed.
 *			  ->_handle Identifies the channel which this RQB belongs to.
 *            ->_opcode Identifies the action/operation that shall be performed.
 *            ->_args   This union provides a pointer to the arguments. The field of the union that is valid is identified by the _opcode.
 */
LSA_VOID eps_request(EPS_RQB_PTR_TYPE2 pRQB)
{
	EPS_CHANNEL_PTR_TYPE channel;
	LSA_HANDLE_TYPE channel_index;
  
	EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,">>> eps_request(): rqb(0x%08x) h(%u) opc(%u)", pRQB, EPS_UPPER_RQB_GET_HANDLE (pRQB), EPS_UPPER_RQB_GET_OPCODE (pRQB));
	
	// apply checks to the RQB  
	if (IS_NULL (pRQB)) 
	{
		//No RQB means that a serious error in the system occured. 
		//Signal this by calling EPS_RQB_ERROR.
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_request(): no rqb");
		EPS_RQB_ERROR(pRQB);
		//EPS_RQB_ERROR never returns control. Therefore this line should be unreachable.
	}

	channel_index = EPS_UPPER_RQB_GET_HANDLE(pRQB);
	
	if (channel_index >= EPS_CFG_MAX_CHANNELS) 
	{
		//Wrong channel handle(index). This is most likely an user error.
		//We cannot signal that error to the user because we cannot pass the RQB back.
		//As the channel handle is invalid we do not know to which channel this RQB belongs to. 
		//Therefore we also don't know which callback-function should be used to pass the RQB back, since that information is stored in the channel.
		//All we can do is call EPS_RQB_ERROR to signal a critical error.
		EPS_SYSTEM_TRACE_02 (0, LSA_TRACE_LEVEL_ERROR, "eps_request(): handle/channel_index(%u) higher than EPS_CFG_MAX_CHANNEL(%u)", channel_index, EPS_CFG_MAX_CHANNELS);
		EPS_RQB_ERROR(pRQB);
		//EPS_RQB_ERROR never returns control. Therefore this line should be unreachable.
	}

	//Reentrance lock, needed if code of this component is being run by more than one task or interrupt routine (on the same processor)
	eps_enter_critical_section(eps_rqb_enter_exit_id);

	if (eps_rqb_interface_data.channels[channel_index].state == EPS_CHANNEL_STATE_FREE) 
	{
		//Wrong channel handle(index). This is most likely an user error.
		//We cannot signal that error to the user because we cannot pass the RQB back.
		//As the channel handle is invalid we do not know to which channel this RQB belongs. 
		//Therefore we also don't know which callback-function should be used to pass the RQB back, since that information is stored in the channel.
		//All we can do is call EPS_RQB_ERROR to signal a critical error.
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_request(): invalid handle/channel_index or channel closed");
		EPS_RQB_ERROR(pRQB);
		//EPS_RQB_ERROR never returns control. Therefore this line should be unreachable.
	}        

	channel = &eps_rqb_interface_data.channels[EPS_UPPER_RQB_GET_HANDLE (pRQB)];
  
	//Main switch for all requests 
	switch (EPS_UPPER_RQB_GET_OPCODE (pRQB)) 
	{
		case EPS_OPC_GET_STATISTICS:
			{
                if(pRQB->args.mem_statistics->comp_id >= EPS_LSA_COMP_ID_SUM)
                {
                    EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_request(): invalid comp_id");
                    eps_callback (channel, EPS_ERR_PARAM, & pRQB);	
                }
                // Only supported if precompile switches are set to support memory statistics -> eps_mem.h #define EPS_USE_MEM_STATISTIC
                #ifdef EPS_USE_MEM_STATISTIC
                {
                    //Point to mem_stats_array.
			        //If there is no HIF active this is the user can simply access the array.
			        //Otherwise HIF will copy the array in by serializing it.
                    eps_mem_get_collected_memstats(&pRQB->args.mem_statistics->mem_stats_for_component, pRQB->args.mem_statistics->comp_id);
                    eps_callback (channel, EPS_OK, & pRQB);	
                }
                #else
                {
                    //Return RQB with error: Precompile switches have to be set in order to use this function
                    EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_WARN, "eps_request(): error - mem statistics not configured");
                    pRQB->args.mem_statistics->mem_stats_for_component.sum_act_alloc_count     = 0xC0FFEE;
                    pRQB->args.mem_statistics->mem_stats_for_component.sum_act_alloc_size      = 0xDEADBEEF;
                    pRQB->args.mem_statistics->mem_stats_for_component.sum_alloc_count         = 0x13371337;
                    pRQB->args.mem_statistics->mem_stats_for_component.max_alloc_size          = 0x12345678;
			        eps_callback (channel, EPS_OK, & pRQB);	
                }
            #endif
			}
			break;
		default:
			{
			EPS_SYSTEM_TRACE_01 (0, LSA_TRACE_LEVEL_ERROR, "eps_request(): unsupported opcode(%u)", EPS_UPPER_RQB_GET_OPCODE(pRQB));
			eps_callback (channel, EPS_ERR_OPCODE, & pRQB);
			}
			break;
	}

	eps_exit_critical_section(eps_rqb_enter_exit_id);
	EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<< eps_request(): pRQB=%X Handle/ChannelIndex=%X", pRQB, channel_index);
	return;
}


/**
 * @brief Called to open a channel specified by the RQB.
 *        At first check the RQB for parameterization errors.
 *        After that find an unused channel in the eps_rqb_interface_data.channels array.
 *        Then get detailpointer and systempointer through the sys_path which is part of the pRQB->args.channel.
 *        Finally store the information about this channel in the eps_rqb_interface_data.channels array.
 *        
 * 
 * @param pRQB Pointer to the RQB which specifies the 
 *        The pRQB->_opcode must be EPS_OPC_OPEN_CHANNEL.
 *        That means the pRQB->args.channel must point to a valid struct of type EPS_RQB_ARGS_CHANNEL_TYPE.
 */
LSA_VOID eps_open_channel(EPS_RQB_PTR_TYPE2 pRQB)
{
	LSA_HANDLE_TYPE                   used_channel_index;
	EPS_CHANNEL_PTR_TYPE              channel;
	EPS_RQB_ARGS_CHANNEL_PTR_TYPE     open_channel_args;
	EPS_DETAIL_PTR_TYPE               detailptr	= LSA_NULL;
	LSA_SYS_PTR_TYPE                  sysptr    = LSA_NULL;
	LSA_UINT16                        retval;

	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>> eps_open_channel(): pRQB=%X", pRQB);
  
	// apply checks to the RQB
	if (IS_NULL (pRQB)) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_open_channel(): no rqb");
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}  
	open_channel_args = pRQB->args.channel;
	if (IS_FUNCTION_NULL(open_channel_args->eps_request_done_callback_fct_ptr))
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_open_channel(): no callback function");
		EPS_UPPER_RQB_SET_RESPONSE(pRQB, EPS_ERR_PARAM);
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}
	if (EPS_UPPER_RQB_GET_OPCODE (pRQB) != EPS_OPC_OPEN_CHANNEL) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_open_channel(): wrong opcode");
		EPS_UPPER_RQB_SET_RESPONSE(pRQB, EPS_ERR_OPCODE);
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}

	//get details and sys-pointer
	retval = eps_get_path_info (&sysptr, &detailptr, pRQB->args.channel->sys_path);
	if ( (retval != LSA_RET_OK) || IS_NULL(detailptr))
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_open_channel(): could not get detailpointer and syspointer");		
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}

	
	eps_enter_critical_section(eps_rqb_enter_exit_id);
	//find the first free channel in the eps_rqb_interface_data.channels array an use it
	for (used_channel_index = 0; used_channel_index < EPS_CFG_MAX_CHANNELS; used_channel_index++) 
	{
		if (eps_rqb_interface_data.channels[used_channel_index].state == EPS_CHANNEL_STATE_FREE) 
		{
			break;
		}
	}
  
	if (used_channel_index >= EPS_CFG_MAX_CHANNELS) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_open_channel(): no resource");
		EPS_UPPER_RQB_SET_RESPONSE (pRQB, EPS_ERR_RESOURCE);
		EPS_UPPER_RQB_SET_HANDLE (pRQB, open_channel_args->handle_upper);
		EPS_REQUEST_DONE(open_channel_args->eps_request_done_callback_fct_ptr, pRQB, sysptr );
		eps_exit_critical_section(eps_rqb_enter_exit_id);
		return;
	}
	else
	{
		reset_channel(used_channel_index);
		eps_rqb_interface_data.channels[used_channel_index].state = EPS_CHANNEL_STATE_IN_USE; 
		channel = &eps_rqb_interface_data.channels[used_channel_index];
		open_channel_args->handle   = used_channel_index;
		channel->handle_upper       = open_channel_args->handle_upper;
		channel->callback           = open_channel_args->eps_request_done_callback_fct_ptr;
		channel->sysptr             = sysptr;
		channel->detailptr          = detailptr;
	}

	EPS_UPPER_RQB_SET_RESPONSE (pRQB, EPS_OK);
	eps_exit_critical_section(eps_rqb_enter_exit_id);
	eps_callback(channel,EPS_UPPER_RQB_GET_RESPONSE(pRQB),&pRQB);

	EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<< eps_open_channel(): pRQB=%X Handle=%X", pRQB, open_channel_args->handle_upper);
	return;
}

/**
 * @brief Function to close a channel specified by the RQB.
 *        At first check the RQB for parameterization errors.
 *		  Then identify the channel which shall be closed by the RQB->_handle.
 *        Finally free the resources of the channel and reset the channel.
 * 
 * 
 * @param pRQB Pointer to the RQB which specifies the 
 *        The pRQB->_opcode must be EPS_OPC_CLOSE_CHANNEL.
 *        The pRQB->_handle indicates which channel shall be closed.
 */
LSA_VOID eps_close_channel(EPS_RQB_PTR_TYPE2 pRQB)
{    
	LSA_HANDLE_TYPE         channel_index;
	LSA_UINT16              retval = LSA_RET_OK;

	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,">>> eps_close_channel(): pRQB=%X", pRQB);

	// apply checks to the RQB  
	if (IS_NULL (pRQB)) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_close_channel(): no rqb");
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}
	if (EPS_UPPER_RQB_GET_OPCODE(pRQB) != EPS_OPC_CLOSE_CHANNEL) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_close_channel(): wrong opcode");
		EPS_UPPER_RQB_SET_RESPONSE(pRQB, EPS_ERR_OPCODE);
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}
	if (EPS_UPPER_RQB_GET_HANDLE(pRQB) >= EPS_CFG_MAX_CHANNELS) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_close_channel(): invalid handle");
		EPS_UPPER_RQB_SET_RESPONSE (pRQB, EPS_ERR_PARAM);
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}
	eps_enter_critical_section(eps_rqb_enter_exit_id);
	if (eps_rqb_interface_data.channels[EPS_UPPER_RQB_GET_HANDLE(pRQB)].state == EPS_CHANNEL_STATE_FREE)
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_ERROR, "eps_close_channel(): invalid handle (state is IN_USE)");
		EPS_UPPER_RQB_SET_RESPONSE (pRQB, EPS_ERR_PARAM);
		EPS_RQB_ERROR(pRQB);
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
		return;
	}

	channel_index = EPS_UPPER_RQB_GET_HANDLE(pRQB);	
	
	//callback for close_channel, needs to be done before we "forget" the channel information
	eps_callback(&(eps_rqb_interface_data.channels[channel_index]), retval, &pRQB);

	//free resources and reset channel
	retval = eps_release_path_info(eps_rqb_interface_data.channels[channel_index].sysptr, eps_rqb_interface_data.channels[channel_index].detailptr);
	if(retval != LSA_RET_OK) 
	{
		EPS_SYSTEM_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL, "eps_close_channel(): could not release path info");
		EPS_FATAL(EPS_FATAL_ERR_RELEASE_PATH);
	}
	reset_channel(channel_index);

	eps_exit_critical_section(eps_rqb_enter_exit_id);
	EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"<<< eps_close_channel(): pRQB=%X Handle/ChannelIndex=%X", pRQB, channel_index);    
	return;
}

/**
 * @brief Do the initialization needed to process RQBs.
 *        Allocate critical sections and initialize the channel management structure.
 * 
 */
LSA_RESPONSE_TYPE eps_init_rqb_interface()
{
	LSA_HANDLE_TYPE channel_index;
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,">>> eps_init_rqb_interface");    

	//allocate critical section for reentrance lock
	if(!eps_alloc_critical_section(&eps_rqb_enter_exit_id, LSA_FALSE))
	{
		EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"eps_init_rqb_interface(): could not allocate critical section");
		return EPS_ERR_RESOURCE;
	}

	//initialize channels (this is necessary if you use open_eps repeatedly, which means this function gets called after there were values writen into the channel management structure)
	for(channel_index = 0; channel_index < EPS_CFG_MAX_CHANNELS; channel_index++)
	{
		reset_channel(channel_index);
	}

    return EPS_OK;
}

/**
 * @brief Undo the initialization for RQB handling in EPS.
 *        Free the critical section.
 * 
 */
LSA_RESPONSE_TYPE eps_undo_init_rqb_interface()
{
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"eps_undo_init_rqb_interface()");
	if(!eps_free_critical_section(eps_rqb_enter_exit_id))
	{
		EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"eps_undo_init_rqb_interface(): could not free critical section");
		return EPS_ERR_RESOURCE;
	}
	return EPS_OK ;
}

/**
 * @brief Use the information provided through sys_path to fill in sys_ptr_ptr amd detail_ptr_ptr with pathing information about the channel.
 *        This function allocates memory for the detailpointer and the systempointer. It stores their locations in the output parameters detail_ptr_ptr and sys_ptr_ptr respectivley.
 * 
 * @param sys_ptr_ptr       Out  Pointer to the systempointer which is used by systemadaption to store pathing information, currently this is getting filled, but is not used 
 * @param detail_ptr_ptr    Out  Pointer to the detailpointer which describes details of the channel which are used for the routing of the EPS RQBs.
 * @param sys_path          In   System path used to describe the kind of path. 
 */
LSA_RESULT eps_get_path_info(
	LSA_SYS_PTR_TYPE*        sys_ptr_ptr,
	EPS_DETAIL_PTR_TYPE*     detail_ptr_ptr,
	LSA_SYS_PATH_TYPE        sys_path )
{
	EPS_DETAIL_PTR_TYPE  pDetail;
	PSI_SYS_PTR_TYPE     pSys;

	LSA_UINT16 rsp_mbx_id    = PSI_MBX_ID_MAX;
	LSA_UINT16 comp_id_lower = LSA_COMP_ID_UNUSED;

	LSA_UINT16 const hd_nr  = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const path   = PSI_SYSPATH_GET_PATH(sys_path);

	LSA_USER_ID_TYPE user_id;
	LSA_UINT16 result        = LSA_RET_OK;

	user_id.uvar32 = 0;
	
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "eps_get_path_info(sys_path=%#x): hd_nr(%u) path(%#x/%u)", sys_path, hd_nr, path, path);

    EPS_ASSERT(sys_ptr_ptr != LSA_NULL);
	EPS_ASSERT(detail_ptr_ptr != LSA_NULL);

	//allocate memory for systempointer
	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM );
	EPS_ASSERT(pSys);

	//allocate memory for detailpointer
	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM );
	EPS_ASSERT(pDetail);

	//fill detailpointer and systempointer based on the system path
	switch (path)
	{
	    case PSI_PATH_GLO_APP_EPS_LD:
		{
            #if (PSI_CFG_USE_LD_COMP == 1) 
            if(eps_get_ld_runs_on() != PSI_LD_RUNS_ON_LIGHT)
            {
			    rsp_mbx_id = PSI_MBX_ID_HIF_LD;
            }
            else
            #endif
            {
			    rsp_mbx_id = PSI_MBX_ID_EPS;
            }
			comp_id_lower = LSA_COMP_ID_PNBOARDS;

            //Save detailed path information
			pDetail->hd_nr      = hd_nr;
			pDetail->is_ld_eps  = LSA_TRUE;
		}
		break;

	    case PSI_PATH_IF_APP_EPS_HD:
		{
            #if (PSI_CFG_USE_HD_COMP == 1)  // Using PSI_CFG_USE_HD_COMP
            if((pDetail->is_ld_eps == 0) && (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO))
            {
			    rsp_mbx_id = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    rsp_mbx_id = PSI_MBX_ID_EPS;
            }
			comp_id_lower = LSA_COMP_ID_PNBOARDS;

            //Save detailed path information
			pDetail->hd_nr      = hd_nr;
			pDetail->is_ld_eps  = LSA_FALSE;
		}
		break;

	    default:
		{
			//invalid path
			LSA_UINT16 ret_val;

			//free allocated memory for detailpointer
			PSI_FREE_LOCAL_MEM( &ret_val, pDetail, 0, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
			EPS_ASSERT( ret_val == LSA_RET_OK );

			//free allocated memory for systempointer
			PSI_FREE_LOCAL_MEM( &ret_val, pSys, 0, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
			EPS_ASSERT( ret_val == LSA_RET_OK );

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;
		}
		break;
	}

	if (result == LSA_RET_OK)
	{
		//fill the systempointer
		pSys->hd_nr                 = hd_nr;
		pSys->comp_id               = LSA_COMP_ID_PNBOARDS;
		pSys->comp_id_lower         = comp_id_lower;
		pSys->mbx_id_rsp            = rsp_mbx_id;   
		pSys->check_arp             = LSA_FALSE;
		pSys->hd_runs_on_level_ld   = (pDetail->is_ld_eps == LSA_TRUE ? LSA_TRUE : LSA_FALSE);
		
		*sys_ptr_ptr    = pSys;
		*detail_ptr_ptr = pDetail;
	}

	return (result);
}

/**
 * @brief This function frees the two passed pointers. 
 *        Use it to free the pointers obtained by eps_get_path_info.
 * 
 * @param sys_ptr     Systempointer acquired through eps_get_path_info.
 * @param detail_ptr  Detailpointer acquired through eps_get_path_info.
 */
LSA_RESULT eps_release_path_info(
	LSA_SYS_PTR_TYPE      sys_ptr,
	EPS_DETAIL_PTR_TYPE   detail_ptr )
{
	LSA_UINT16          ret_val;

	EPS_ASSERT(sys_ptr);
	EPS_ASSERT(detail_ptr);

	EPS_ASSERT(((PSI_SYS_PTR_TYPE)sys_ptr)->comp_id == LSA_COMP_ID_PNBOARDS);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_release_path_info()");

    PSI_FREE_LOCAL_MEM(&ret_val, detail_ptr, 0, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
	EPS_ASSERT(ret_val == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM(&ret_val, sys_ptr, 0, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
	EPS_ASSERT(ret_val == LSA_RET_OK);

	return (LSA_RET_OK);
}

/**
 * @brief Set the RQBs repsonse to the response passed to this function.
 *        Then use the callback-function saved for the channel to pass the RQB back to the caller.
 * 
 * 
 * 
 * @param channel   The channel which the RQB belongs to.
 * @param response  The response which the RQB signal to the caller.
 * @param ppRQB     Pointer to the pointer of the RQB.
 */
LSA_VOID eps_callback(EPS_CHANNEL_CONST_PTR_TYPE channel, LSA_UINT16 response, EPS_RQB_PTR_TYPE2* ppRQB) 
{
	EPS_RQB_PTR_TYPE2 pRQB;
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"eps_callback()");

	//check for valid ppRQB
	EPS_ASSERT (IS_NOT_NULL (ppRQB));
	EPS_ASSERT (IS_NOT_NULL (*ppRQB));

	//save the pointer to the RQB for internal use and set ppRQB to NULL
	pRQB = *ppRQB;
	*ppRQB = LSA_NULL;

	EPS_UPPER_RQB_SET_HANDLE (pRQB, channel->handle_upper);
	EPS_UPPER_RQB_SET_RESPONSE (pRQB, response);
	EPS_SYSTEM_TRACE_05 (0, LSA_TRACE_LEVEL_NOTE, "<<< eps_callback(): eps rqb(0x%08x) h(%u) opc(%u) rsp(%u)  h-to(%u)", ppRQB, channel->handle_upper, EPS_UPPER_RQB_GET_OPCODE (pRQB), response, channel->handle_upper);

	//pass RQB back to the caller
	EPS_REQUEST_DONE(channel->callback, pRQB, channel->sysptr);
}

/**
 * @brief Use PSI_RQB_ERROR function to singal an error.
 * 
 * @param pRQB Pointer to the RQB whose parameters caused the error.
 */
LSA_VOID  EPS_RQB_ERROR(
    EPS_RQB_PTR_TYPE2 pRQB)
{
  PSI_RQB_ERROR( LSA_COMP_ID_PNBOARDS, 0, pRQB );
}

/**
 * @brief Send a processed RQB back to the caller. 
 * 
 * @param eps_request_done_callback_fct_ptr Pointer to the callbackfunction specified through the arguments of eps_open_channel. 
 *        If HIF is used this is not the original callbackfunction but rather a function of HIF, 
 *        which handles the restoration of the original callbackfunction as well as the way through the shared memory. 
 * @param pRQB Pointer to the RQB which shall be passed back to the caller.
 * @param sys_ptr System pointer which contains information needed for passing the RQB back. Especially the mailbox of the caller is required.
 */
LSA_VOID EPS_REQUEST_DONE(
	EPS_CALLBACK_FCT_PTR_TYPE2 eps_request_done_callback_fct_ptr,
	EPS_RQB_PTR_TYPE2                   pRQB,
	LSA_SYS_PTR_TYPE                 sys_ptr)
{
	//use the psi_request_done function to pass the RQB to the correct mailbox and invoke the callbackfunction there to process the RQB.
	psi_request_done( (PSI_REQUEST_FCT)eps_request_done_callback_fct_ptr, (struct psi_header*)pRQB, sys_ptr );
}

/*****************************************************************************/
/*  end of file eps_usr.c                                                    */
/*****************************************************************************/
