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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the system LSA interface                                      */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	2
#define HIF_MODULE_ID		LTRC_ACT_MODUL_ID

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

#if (HIF_CFG_USE_HIF == 1)

static LSA_FATAL_ERROR_TYPE  hif_glob_fatal_error; /**< global fatal-error structure */

/* description: see header */
LSA_UINT16 hif_init(LSA_VOID)
{
    LSA_UINT16 res = LSA_RET_OK;
	LSA_UINT16 i,e,retVal;

	HIF_LD_ENTER();
	HIF_HD_ENTER();

	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_init->");    

    /* check if HIF uses the same size for a BOOL like defined LSA_BOOL (in lsa_cfg.h) */
    //lint --e(506) Constant value boolean. This check is intended to be done while initialization.
    //lint --e(774) Boolean with if always evaluates to False.
    if (sizeof(LSA_BOOL) != HIF_LSA_BOOL_SIZE)
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_init(): LSA_BOOL-size is not equal HIF_LSA_BOOL_SIZE declared in HIF");
		HIF_FATAL(0);
    }

	/* init internal structures */
	HIF_ALLOC_LOCAL_MEM((void**) &g_pHifData, sizeof(*g_pHifData), LSA_COMP_ID_HIF, HIF_MEM_TYPE_OTHER );

	if(HIF_IS_NULL(g_pHifData))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_init(-) can not alloc g_pHifData");

        HIF_HD_EXIT();
        HIF_LD_EXIT();

        return LSA_RET_ERR_RESOURCE;
	}
	else
	{
		HIF_MEMSET(g_pHifData, 0, sizeof(*g_pHifData));
		
		/* apply init values */   
		for(i=0; i<HIF_MAX_INSTANCES; i++)
		{
			g_pHifData->Instances[i].hH    = (HIF_HANDLE)i;
			g_pHifData->Instances[i].State = HIF_INST_STATE_FREE;
	
			#if (HIF_CFG_MAX_LD_INSTANCES > 0)
			if(i<HIF_CFG_MAX_LD_INSTANCES) /* LD Device */
			{
				g_pHifData->Instances[i].bLDDevice = LSA_TRUE;
				g_pHifData->Instances[i].bHDDevice = LSA_FALSE;
				
				g_pHifData->Instances[i].func.DO_INTERRUPT_REQUEST = HIF_LD_DO_INTERRUPT_REQUEST;
				g_pHifData->Instances[i].func.DO_REQUEST_LOCAL = HIF_LD_DO_REQUEST_LOCAL;
				g_pHifData->Instances[i].func.SEND_IRQ = HIF_LD_SEND_IRQ;
				g_pHifData->Instances[i].func.ACK_IRQ  = HIF_LD_ACK_IRQ;
				g_pHifData->Instances[i].func.REQUEST_LOWER = HIF_LD_REQUEST_LOWER;
				g_pHifData->Instances[i].func.CLOSE_CHANNEL_LOWER = HIF_LD_CLOSE_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.OPEN_CHANNEL_LOWER = HIF_LD_OPEN_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.CLOSE_DEVICE_LOWER = HIF_LD_CLOSE_DEVICE_LOWER;
			}
			#endif
			#if (HIF_CFG_MAX_LD_INSTANCES > 0) && (HIF_CFG_MAX_HD_INSTANCES > 0)
			else /* HD Device */
			#endif
			#if (HIF_CFG_MAX_HD_INSTANCES > 0)
			{
				g_pHifData->Instances[i].bLDDevice = LSA_FALSE;
				g_pHifData->Instances[i].bHDDevice = LSA_TRUE;
				
				g_pHifData->Instances[i].func.DO_INTERRUPT_REQUEST = HIF_HD_DO_INTERRUPT_REQUEST;
				g_pHifData->Instances[i].func.DO_REQUEST_LOCAL = HIF_HD_DO_REQUEST_LOCAL;
				g_pHifData->Instances[i].func.SEND_IRQ = HIF_HD_SEND_IRQ;
				g_pHifData->Instances[i].func.ACK_IRQ  = HIF_HD_ACK_IRQ;
				g_pHifData->Instances[i].func.REQUEST_LOWER = HIF_HD_REQUEST_LOWER;
				g_pHifData->Instances[i].func.CLOSE_CHANNEL_LOWER = HIF_HD_CLOSE_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.OPEN_CHANNEL_LOWER = HIF_HD_OPEN_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.CLOSE_DEVICE_LOWER = HIF_HD_CLOSE_DEVICE_LOWER;
			}
			#endif

			hif_setup_single_pipe_params_by_index(&g_pHifData->Instances[i], HIF_GLOB_ADMIN, LSA_COMP_ID_HIF, 0, HIF_GLOB_ADMIN);

			for(e=0; e<HIF_MAX_PIPES_INST; e++)
			{
				g_pHifData->Instances[i].Pipes[e].PipeIndex       = e;
				g_pHifData->Instances[i].Pipes[e].State           = HIF_PIPE_STATE_CLOSED;
				g_pHifData->Instances[i].Pipes[e].pInstance       = &(g_pHifData->Instances[i]);				
			}
		}

		for(i=0; i<HIF_MAX_CH_HANDLES; i++)
		{
			g_pHifData->ChHandles[i].bUsed         = LSA_FALSE;
			g_pHifData->ChHandles[i].bValid        = LSA_FALSE;
			g_pHifData->ChHandles[i].hHifCompCh    = (LSA_HANDLE_TYPE)i;
		}

		for(i=0; i<HIF_MAX_COMP_ID; i++)
		{
			g_pHifData->CompStore.CompStd[i].bUsed = LSA_FALSE;
			g_pHifData->CompStore.CompSys[i].bUsed = LSA_FALSE;
		}

		/* reserve channel handle used by HIF internally */
		g_pHifData->ChHandles[HIF_INTERNAL_REQ_RESERVED_CH_HANDLE].bUsed = LSA_TRUE;

        #if (HIF_CFG_USE_DMA == 1)
        hif_buffer_admin_init();
        #endif
	}

    #if !defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    #if (HIF_CFG_MAX_LD_INSTANCES > 0)
	/* alloc ld poll timer */
    HIF_ALLOC_TIMER(&res, &(g_pHifData->LD.PollTimer.hPollTimer), LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_100MS);

	if(res != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "hif_init(-) can not alloc ld poll timer");        
		res = LSA_RET_ERR_RESOURCE;
	}
    #endif
    #endif

    #if !defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
    #if (HIF_CFG_MAX_HD_INSTANCES > 0)
	if(res == LSA_RET_OK)
	{
		/* alloc hd poll timer */
        HIF_ALLOC_TIMER(&res, &(g_pHifData->HD.PollTimer.hPollTimer), LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_100MS);

		if(res != LSA_RET_OK)
		{
			HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "hif_init(-) can not alloc hd poll timer");        
			res = LSA_RET_ERR_RESOURCE;
		}
	}
    #endif
    #endif

    #if (HIF_CFG_MAX_LD_INSTANCES > 0)
	if(res == LSA_RET_OK)
	{
		/* alloc ld timeout poll rqb */
		res = hif_alloc_local_internal_rqb(&(g_pHifData->LD.PollTimer.pRQBPoll), HIF_OPC_TIMEOUT_POLL);
	}
    if (res == LSA_RET_OK)
    {
        /* alloc ld poll rqb */
        res = hif_alloc_local_internal_rqb(&(g_pHifData->LD.pRQBPoll), HIF_OPC_POLL);
    }
    #endif

    #if (HIF_CFG_MAX_HD_INSTANCES > 0)
	if (res == LSA_RET_OK)
	{
		/* alloc hd timeout poll rqb */
		res = hif_alloc_local_internal_rqb(&(g_pHifData->HD.PollTimer.pRQBPoll), HIF_OPC_TIMEOUT_POLL);
	}
    if (res == LSA_RET_OK)
    {
        /* alloc hd poll rqb */
        res = hif_alloc_local_internal_rqb(&(g_pHifData->HD.pRQBPoll), HIF_OPC_POLL);
    }
    #endif

    if ((res == LSA_RET_OK))
    {
        /* alloc internal poll rqb */
        res = hif_alloc_local_internal_rqb(&(g_pHifData->internal.pInternalPoll), HIF_OPC_INTERNAL_POLL);
        g_pHifData->internal.internalPollCnt = 0;
        g_pHifData->internal.bUseInternalPoll = LSA_FALSE; // don't use it by default
    }

	if(res == LSA_RET_OK)
	{
		/* alloc isr rqb */
		for(i=0; (i<HIF_MAX_INSTANCES) && (res==LSA_RET_OK); i++)
		{
			res = hif_alloc_local_internal_rqb(&(g_pHifData->Instances[i].pRQBIsr), HIF_OPC_ISR);
			g_pHifData->Instances[i].pRQBIsr->args.internal_req.receive.hHDest = g_pHifData->Instances[i].hH;
		}
		
		/* alloc short rqb */
		for(i=0; (i<HIF_MAX_INSTANCES) && (res==LSA_RET_OK); i++)
		{
			res = hif_alloc_local_internal_rqb(&(g_pHifData->Instances[i].pRQBShort), HIF_OPC_RECEIVE);
			g_pHifData->Instances[i].pRQBShort->args.internal_req.receive.hHDest = g_pHifData->Instances[i].hH;
		}
	}

	if(res == LSA_RET_OK)
	{
		hif_install_integrated_comp_access_funcs();
	}
	
	if(res != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "hif_init(-)");
		
		/* cleanup & abort */    
		if(HIF_IS_NOT_NULL(g_pHifData))
		{
			/* dealloc ld timer poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->LD.PollTimer.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->LD.PollTimer.pRQBPoll);
			}
			/* dealloc hd timer poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->HD.PollTimer.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->HD.PollTimer.pRQBPoll);
			}
			/* dealloc ld poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->LD.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->LD.pRQBPoll);
			}
			/* dealloc hd poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->HD.pRQBPoll);
			}

            /* dealloc internal trigger poll rqb */
            if (HIF_IS_NOT_NULL(g_pHifData->internal.pInternalPoll))
            {
                hif_free_local_internal_rqb(g_pHifData->internal.pInternalPoll);
            }

			/* dealloc isr/short rqbs */      
			for(i=0; i<HIF_MAX_INSTANCES; i++)
			{
				if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBIsr))
				{
					hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBIsr);
				}
				if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBShort))
				{
					hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBShort);
				}
			}
			
			/* dealloc global hif data */
			HIF_FREE_LOCAL_MEM(&retVal, g_pHifData, LSA_COMP_ID_HIF, HIF_MEM_TYPE_OTHER );
			HIF_ASSERT(retVal == LSA_RET_OK);
			
			g_pHifData = (HIF_DATA_TYPE*)LSA_NULL;
		}
	}
	else
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_init(+)");
	}

	HIF_HD_EXIT();
	HIF_LD_EXIT();
	
	return res;
}

/* description: see header */
LSA_UINT16 hif_undo_init(LSA_VOID)
{
	LSA_UINT16 i;
	LSA_UINT16 retVal;
	
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_undo_init->");
	
	if(HIF_IS_NULL(g_pHifData))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called when g_pHifData == 0!");
		return LSA_RET_ERR_SEQUENCE;
	}

	/* check if there is any open hif device */
	if(hif_check_for_open_instances() == LSA_TRUE)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called => not all hif devices closed!");
		return LSA_RET_ERR_SEQUENCE;
	}

	/* check for open poll users - should not be possible */
	if(g_pHifData->LD.PollTimer.PollUserCnt != 0)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called => ld poll users still registered!");
		return LSA_RET_ERR_SEQUENCE;
	}
	if(g_pHifData->HD.PollTimer.PollUserCnt != 0)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called => hd poll users still registered!");
		return LSA_RET_ERR_SEQUENCE;
	} 
	
    #if (HIF_CFG_MAX_LD_INSTANCES > 0)
    #if !defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
	/* free poll timer */
	HIF_FREE_TIMER(&retVal, g_pHifData->LD.PollTimer.hPollTimer);
	HIF_ASSERT(retVal == LSA_RET_OK);
    #endif
	/* free timeout poll rqb */
	hif_free_local_internal_rqb(g_pHifData->LD.PollTimer.pRQBPoll);
	/* free poll rqb */
	hif_free_local_internal_rqb(g_pHifData->LD.pRQBPoll);
    #endif

    #if (HIF_CFG_MAX_HD_INSTANCES > 0)
    #if !defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
	/* free poll timer */
	HIF_FREE_TIMER(&retVal, g_pHifData->HD.PollTimer.hPollTimer);
	HIF_ASSERT(retVal == LSA_RET_OK);
    #endif
	/* free timeout poll rqb */
	hif_free_local_internal_rqb(g_pHifData->HD.PollTimer.pRQBPoll);
	/* free poll rqb */
	hif_free_local_internal_rqb(g_pHifData->HD.pRQBPoll);
    #endif

    /* free internal trigger rqb */
    HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData->internal.pInternalPoll));
    hif_free_local_internal_rqb(g_pHifData->internal.pInternalPoll);

	/* dealloc isr/short rqbs */
	for(i=0; i<HIF_MAX_INSTANCES; i++)
	{
		if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBIsr))
		{
			hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBIsr);
		}
		if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBShort))
		{
			hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBShort);
		}
	}

    #if (HIF_CFG_USE_DMA == 1)
    hif_buffer_admin_undo_init();
    #endif

	/* free internal structures */
	HIF_FREE_LOCAL_MEM(&retVal, g_pHifData, LSA_COMP_ID_HIF, HIF_MEM_TYPE_OTHER);
	HIF_ASSERT(retVal == LSA_RET_OK);

	g_pHifData = (HIF_DATA_TYPE*)LSA_NULL;

	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_undo_init(+)");

	return LSA_RET_OK;
}

/*------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------*/
/* description: see header */
LSA_UINT16 hif_register_standard_component(LSA_COMP_ID_TYPE const CompId, HIF_COMPONENT_SUPPORT_STD_PTR_TYPE const pCompFuncs)
{
    HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));
    HIF_ASSERT(CompId < HIF_MAX_COMP_ID);

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_register_standard_component(+) - CompId(0x%x), pCompFuncs(0x%x)", CompId, pCompFuncs);

	if(pCompFuncs == LSA_NULL)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%u/0x%x) pCompFuncs(0x%x)", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

	if( (pCompFuncs->IsCloseChannelRqb             == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->ReplaceOpenChannelLowerHandle == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->ReplaceOpenChannelCbf         == HIF_FCT_PTR_NULL) ||
	    (pCompFuncs->GetOpenChannelCompInfo        == HIF_FCT_PTR_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) a required function ptr is missing", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

	if(pCompFuncs->GetOpenChannelLowerHandle == HIF_FCT_PTR_NULL)
	{
        #if (HIF_CFG_USE_EDD == 1)
        if( (CompId == LSA_COMP_ID_EDD)  ||
            (CompId == LSA_COMP_ID_EDDI) ||
            (CompId == LSA_COMP_ID_EDDP) ||
            (CompId == LSA_COMP_ID_EDDS) ||
            (CompId == LSA_COMP_ID_EDDT) )
        {
            //no error!
        }
        else
        #endif
        {
		    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) a required function ptr is missing", CompId, CompId, pCompFuncs);
		    return LSA_RET_ERR_PARAM;
        }
	}

    #if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	if( (pCompFuncs->DeserializeLower == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->DeserializeUpper == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->SerializeLower   == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->SerializeUpper   == HIF_FCT_PTR_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) a required function ptr for serialization is missing", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}
    #endif

	if(g_pHifData->CompStore.CompStd[CompId].bUsed == LSA_TRUE)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) component already registered", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_SEQUENCE;
	}

	g_pHifData->CompStore.CompStd[CompId].Comp  = *pCompFuncs;
	g_pHifData->CompStore.CompStd[CompId].bUsed = LSA_TRUE;

	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_register_standard_component(+) CompId(%u/0x%x) pCompFuncs(0x%x)", CompId, CompId, pCompFuncs);

	return LSA_RET_OK;
}

/* description: see header */
LSA_UINT16 hif_register_sys_component(LSA_COMP_ID_TYPE const CompId, HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE const pCompFuncs)
{
    HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_register_sys_component(+) - CompId(0x%x), pCompFuncs(0x%x)", CompId, pCompFuncs);

	if(pCompFuncs == LSA_NULL)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%u/0x%x) pCompFuncs(0x%x)", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

	if( (pCompFuncs->GetSysRequesthH      == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->GetSysRequestCbf     == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->ReplaceSysRequestCbf == HIF_FCT_PTR_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) a required function ptr is missing", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	if( (pCompFuncs->DeserializeLower == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->DeserializeUpper == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->SerializeLower   == HIF_FCT_PTR_NULL) ||
		(pCompFuncs->SerializeUpper   == HIF_FCT_PTR_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) a required function ptr for serialization is missing", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}
#endif

	if(g_pHifData->CompStore.CompSys[CompId].bUsed == LSA_TRUE)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%u/0x%x) pCompFuncs(0x%x) component already registered", CompId, CompId, pCompFuncs);
		return LSA_RET_ERR_SEQUENCE;
	}

	g_pHifData->CompStore.CompSys[CompId].Comp  = *pCompFuncs;
	g_pHifData->CompStore.CompSys[CompId].bUsed = LSA_TRUE;

	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_register_sys_component(+) CompId(%u/0x%x) pCompFuncs(0x%x)", CompId, CompId, pCompFuncs);

	return LSA_RET_OK;
}

/* description: see header */
LSA_UINT16 hif_ld_system(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_HANDLE hH;
	HIF_INST_PTR_TYPE pHifInst;
	LSA_UINT16 retVal = LSA_RET_ERR_PARAM;
	LSA_COMP_ID_TYPE comp_id;
	
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_system(+) -  pRQB(0x%x)", pRQB);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(pRQB))
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_ld_system(): pRQB is null");
		HIF_FATAL(0);
	}
	
	HIF_LD_ENTER();

	comp_id = HIF_RQB_GET_COMP_ID(pRQB);

	if(comp_id != LSA_COMP_ID_HIF)
	{
		HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
        HIF_ASSERT(HIF_FCT_IS_NOT_NULL(g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH));

		g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH(&hH, pRQB);

		hif_get_instance(&pHifInst, hH);
		if(HIF_IS_NULL(pHifInst))
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_ld_system(): invalid handle, pHifInst is null");
			HIF_FATAL(0); 
		}

		HIF_ASSERT(hif_is_upper_device(pHifInst) == LSA_TRUE);
		HIF_ASSERT(hif_is_ld_device(pHifInst) == LSA_TRUE);

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_system(+) external sys req => pRQB(0x%x) hH(%d)", pRQB, hH);

		hif_send_admin_request(pHifInst, pRQB);
	}
	else
	{
		switch (HIF_RQB_GET_OPCODE(pRQB))
		{
			case HIF_OPC_LD_UPPER_OPEN:
			{
				retVal = hif_upper_ld_device_open(pRQB);
				break;
			}
			case HIF_OPC_LD_LOWER_OPEN:
			{
				retVal = hif_lower_ld_device_open(pRQB);
				break;
			}
			case HIF_OPC_LD_UPPER_CLOSE:
			{
				retVal = hif_upper_device_close(pRQB);
				break;
			}
			case HIF_OPC_LD_LOWER_CLOSE:
			{
				retVal = hif_lower_device_close(pRQB);
				break;
			}
			default:
			{
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_ld_system(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
				retVal = LSA_RET_ERR_PARAM;
			}
		}

		if(retVal != LSA_RET_OK_ASYNC)
		{
			hif_system_callback(pRQB, retVal);
		}
	}

	HIF_LD_EXIT();

	return retVal;
}

/* description: see header */
LSA_UINT16 hif_hd_system(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_HANDLE hH;
	HIF_INST_PTR_TYPE pHifInst;
	LSA_UINT16 retVal = LSA_RET_ERR_PARAM;
	LSA_COMP_ID_TYPE comp_id;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_system(+) -  pRQB(0x%x)", pRQB);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(pRQB))
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_hd_system(): pRQB is null");
		HIF_FATAL(0);
	}

	HIF_HD_ENTER();

	comp_id = HIF_RQB_GET_COMP_ID(pRQB);

	if(comp_id != LSA_COMP_ID_HIF)
	{
		HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
        HIF_ASSERT(HIF_FCT_IS_NOT_NULL(g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH));

		g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH(&hH, pRQB);

		hif_get_instance(&pHifInst, hH);
		if(HIF_IS_NULL(pHifInst))
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_hd_system(): invalid handle, pHifInst is null");
			HIF_FATAL(0); 
		}

		HIF_ASSERT(hif_is_upper_device(pHifInst) == LSA_TRUE);
		HIF_ASSERT(hif_is_ld_device(pHifInst) == LSA_FALSE);

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_system(+) external sys req => pRQB(0x%x) hH(%d)", pRQB, hH);

		hif_send_admin_request(pHifInst, pRQB);
	}
	else
	{
		switch (HIF_RQB_GET_OPCODE(pRQB))
		{
			case HIF_OPC_HD_UPPER_OPEN:
			{
				retVal = hif_upper_hd_device_open(pRQB);
				break;
			}
			case HIF_OPC_HD_LOWER_OPEN:
			{
				retVal = hif_lower_hd_device_open(pRQB);
				break;
			}
			case HIF_OPC_HD_UPPER_CLOSE:
			{
				retVal = hif_upper_device_close(pRQB);
				break;
			}
			case HIF_OPC_HD_LOWER_CLOSE:
			{
				retVal = hif_lower_device_close(pRQB);
				break;
			}
			default:
			{
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_hd_system(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
				retVal = LSA_RET_ERR_PARAM;
			}
		}

		if(retVal != LSA_RET_OK_ASYNC)
		{
			hif_system_callback(pRQB, retVal);
		}
	}

	HIF_HD_EXIT();

	return retVal;
}

/* description: see header */
LSA_VOID hif_open_device_lower_done(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_HANDLE hH = (HIF_HANDLE)0xFF;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_open_device_lower_done(+) -  pRQB(0x%x)", pRQB);

	switch(HIF_RQB_GET_OPCODE(pRQB))
	{
		case HIF_OPC_LD_UPPER_OPEN:
			hH = pRQB->args.dev_ld_open.hH;
			break;
		case HIF_OPC_HD_UPPER_OPEN:
			hH = pRQB->args.dev_hd_open.hH;
			break;
		default:
            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_open_device_lower_done(): RQB (0x%08x) OPC not valid (0x%x)", pRQB, HIF_RQB_GET_OPCODE(pRQB));
			HIF_FATAL(0);
	}

	hif_get_instance(&pHifInst, hH);

	if(HIF_IS_NULL(pHifInst))
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_device_lower_done(): handle invalid, pHifInst is null");
		HIF_FATAL(0);
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_device_lower_done pRQB(0x%x) hH(%d)",pRQB,hH);

	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		HIF_LD_ENTER();

		/* restore original system_done Cbf (only relevant for short mode) */
		pRQB->args.dev_ld_open.Cbf = pHifInst->backup.SystemDoneCbf;
		pRQB->args.dev_ld_open.hH  = pHifInst->backup.hH;

		/* proceed device open process */
		hif_lower_ld_device_open_finish(pHifInst, pRQB);

		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_ENTER();

		/* restore original system_done Cbf (only relevant for short mode) */
		pRQB->args.dev_hd_open.Cbf = pHifInst->backup.SystemDoneCbf;
		pRQB->args.dev_hd_open.hH  = pHifInst->backup.hH;

		/* proceed device open process */
		hif_lower_hd_device_open_finish(pHifInst, pRQB);

		HIF_HD_EXIT();
	}
}

/* description: see header */
LSA_VOID hif_close_device_lower_done(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_HANDLE hH = pRQB->args.dev_close.hH;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_close_device_lower_done(+) -  pRQB(0x%x)", pRQB);

	hif_get_instance(&pHifInst, hH);

	if(HIF_IS_NULL(pHifInst))
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_close_device_lower_done(): handle invalid, pHifInst is null");
		HIF_FATAL(0);
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_close_device_lower_done pRQB(0x%x) hH(%d)",pRQB,hH);

	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		/* LD Device */
		HIF_LD_ENTER();
	}
	else /* HD Device */
	{
		HIF_HD_ENTER();
	}

	/* restore original system_done Cbf (only relevant for short mode) */
	pRQB->args.dev_close.Cbf = pHifInst->backup.SystemDoneCbf;
	pRQB->args.dev_close.hH  = pHifInst->backup.hH;

	/* proceed with device close process */
	hif_lower_device_close_finish(pHifInst, pRQB);

	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		/* LD Device */
		HIF_LD_EXIT();
	}
	else /* HD Device */
	{
		HIF_HD_EXIT();
	}
}

/**
 * \brief finish a Common system request
 * 
 * Real lower_sys_request_done handling is done in hif_lower_sys_request_done2()
 * hif_lower_sys_request_done2() has to be called in the correct hif context (HD/LD)! 
 * This function gives the system adaption the possibility to change contexts
 * 
 * @param [in] rqb - The RQB that is returned to the originator
 */
LSA_VOID hif_lower_sys_request_done(LSA_VOID * rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request_done(+) -  rqb(0x%x)", rqb);

    #if (HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1)
    {
	    HIF_RQB_PTR_TYPE  const pRQB = (HIF_RQB_PTR_TYPE)rqb;
	    HIF_PIPE_PTR_TYPE       pPipe;

	    /* Sys requests returned from lower */

	    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request_done() - pRQB(0x%x)",pRQB);

	    /* Restore Cbf */
	    hif_get_return_path_of_sys_rqb(&pPipe, pRQB);

	    /* (LaM) in principle accessing pPipe structure is enter/exit critical!                 */
	    /* But for performance reasons we don't use enter/exit here expecting that there is no	*/
	    /* HIF user closing the channel/device while there are open requests in the system.		*/

	    //if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	    //{
	    //	HIF_LD_ENTER();
	    //}
	    //else /* HD Device */
	    //{
	    //	HIF_HD_ENTER();
	    //}

	    HIF_SYS_REQUEST_LOWER_DONE(hif_lower_sys_request_done2, pRQB, pPipe->pInstance->hSysDev);

	    //if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	    //{
	    //	HIF_LD_EXIT();
	    //}
	    //else /* HD Device */
	    //{
	    //	HIF_HD_EXIT();
	    //}
    }
    #else
	hif_lower_sys_request_done2(rqb);
    #endif
}

/**
 * \brief Common lower request finish function.
 * 
 * (1) Restore cbf
 * (2) Returns the RQB by calling hif_send.
 * 
 * @param rqb
 */
LSA_VOID hif_lower_sys_request_done2(LSA_VOID * rqb)
{
	HIF_RQB_PTR_TYPE  pRQB = (HIF_RQB_PTR_TYPE)rqb;
	HIF_PIPE_PTR_TYPE pPipe;

	/* Sys requests returned from lower */

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request_done2() - pRQB(0x%x)",pRQB);

	/* (1) Restore Cbf */
	hif_get_return_path_of_sys_rqb(&pPipe, pRQB);

	hif_restore_user_fields_of_sys_rqb_and_free_return_path(pPipe, pRQB);

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_ENTER();
	}
	else /* HD Device */
	{
		HIF_HD_ENTER();
	}

	hif_send(pPipe, pRQB); /* (2) Send RQB to HIF Upper */

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else /* HD Device */
	{
		HIF_HD_EXIT();
	}
}

/* description: see header */
LSA_VOID hif_timeout(LSA_UINT16 timer_id, LSA_USER_ID_TYPE user_id)
{
	HIF_RQB_PTR_TYPE pRQBPoll;
	LSA_UINT32       i;

    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_timeout(+)");

	LSA_UNUSED_ARG(timer_id);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(user_id.uvar32 == HIF_LD_TIMER_ID) /* LD Poll Timer timeout */
	{
		HIF_ASSERT(g_pHifData->LD.PollTimer.PollUserCnt != 0);

		#if HIF_CFG_MAX_LD_INSTANCES > 0
		/* Poll resource available? */
		if(HIF_IS_NOT_NULL(g_pHifData->LD.PollTimer.pRQBPoll))
		{
			for(i=0; i<HIF_CFG_MAX_LD_INSTANCES; i++)
			{
				/* send IRQ poll if there is something to receive or there is something in the send queue*/
				if( (g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) && (g_pHifData->Instances[i].if_pars.TmOptions.bTimerPoll == LSA_TRUE)) 
				{
					pRQBPoll = g_pHifData->LD.PollTimer.pRQBPoll;
					g_pHifData->LD.PollTimer.pRQBPoll = LSA_NULL;
					HIF_LD_DO_TIMEOUT_REQUEST(pRQBPoll);
					break;
				}
			}
		}
		#else
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_timeout(): HIF_CFG_MAX_LD_INSTANCES == 0");
		HIF_FATAL(0); /* Couldn't be */
		#endif

	}
	else if(user_id.uvar32 == HIF_HD_TIMER_ID) /* HD Poll Timer timeout */
	{
		HIF_ASSERT(g_pHifData->HD.PollTimer.PollUserCnt != 0);

		#if (HIF_CFG_MAX_HD_INSTANCES > 0)
		/* Poll resource available? */
		if(HIF_IS_NOT_NULL(g_pHifData->HD.PollTimer.pRQBPoll))
		{
			for(i=HIF_CFG_MAX_LD_INSTANCES; i<HIF_MAX_INSTANCES; i++)
			{
				/* send IRQ poll if there is something to receive */
				if((g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) && (g_pHifData->Instances[i].if_pars.TmOptions.bTimerPoll == LSA_TRUE))
				{
					pRQBPoll = g_pHifData->HD.PollTimer.pRQBPoll;
					g_pHifData->HD.PollTimer.pRQBPoll = LSA_NULL;
					HIF_HD_DO_TIMEOUT_REQUEST(pRQBPoll);
					break;
				}
			}
		}
		#else
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_timeout(): HIF_CFG_MAX_HD_INSTANCES == 0");
		HIF_FATAL(0); /* Couldn't be */
		#endif
	}
	else
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_timeout(): Unknown Timer Id");
		HIF_FATAL(0);
	}
}

/* description: see header */
LSA_VOID hif_internal_poll(HIF_RQB_PTR_TYPE ptr_upper_rqb)
{
    /* Why would you call it if it's not configured?*/
    HIF_ASSERT(g_pHifData->internal.bUseInternalPoll == LSA_TRUE); 

    /* g_pHifData->internal.pInternalPoll is set to LSA_NULL before the RQB is passed to the low context*/
    HIF_ASSERT(LSA_NULL == g_pHifData->internal.pInternalPoll);

    /* restore pointer */
    g_pHifData->internal.pInternalPoll = ptr_upper_rqb;

    /* Trigger hif_poll */
    hif_poll();
}

/* description: see header */
LSA_VOID hif_poll(LSA_VOID)
{
	LSA_UINT32 i;
	LSA_BOOL   bInstTriggered = LSA_FALSE;

    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_poll(+)");

	#if (HIF_CFG_MAX_LD_INSTANCES > 0)
	for(i=0; i<HIF_CFG_MAX_LD_INSTANCES; i++) /* go through LD Devices */
	{
		if( HIF_IS_NOT_NULL(g_pHifData->LD.pRQBPoll) &&	(g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) )
		{
			bInstTriggered = hif_poll_instance(&g_pHifData->Instances[i]);
			if(bInstTriggered == LSA_TRUE)
			{
				break; /* Only 1 Poll RQB available for all LD Devices, so break Loop if already sent */
			}
		}
	}

	bInstTriggered = LSA_FALSE;
	#endif

	#if (HIF_CFG_MAX_HD_INSTANCES > 0)
	for(i=HIF_CFG_MAX_LD_INSTANCES; i<HIF_MAX_INSTANCES; i++) /* go through HD Devices */
	{
		if( HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll) &&	(g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) )
		{
			bInstTriggered = hif_poll_instance(&g_pHifData->Instances[i]);
			if(bInstTriggered == LSA_TRUE)
			{
				break; /* Only 1 Poll RQB available for all HD Devices, so break Loop if already sent */
			}
		}
	}
	#endif
}

/**
 * \brief HIF Poll Instance
 * 
 * This function is called by hif_poll. It sends an internal poll RQB to the HIF task to trigger send / receive.
 * @see HIF_LD_DO_POLL_REQUEST 
 * @see HIF_HD_DO_POLL_REQUEST
 * 
 * @param [in] pHifInst - current HIF instance
 * @return LSA_TRUE  - the internal poll RQB was sent
 * @return LSA_FALSE - the internal poll RQB is still on the way. No new RQB was sent. 
 */
LSA_BOOL hif_poll_instance(HIF_INST_PTR_TYPE pHifInst)
{
	HIF_RQB_PTR_TYPE pRQBPoll;
	LSA_BOOL         bIsLdDevice;
	LSA_BOOL         bRQBSent = LSA_FALSE;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_poll_instance(+) - pHifInst(0x%x)", pHifInst);

	HIF_ASSERT(HIF_IS_NOT_NULL(pHifInst));

    // In HSM-variant there is a race condition, because the function's hif_send() and hif_receive() 
    // trigger asynchronously this fucntion. In normal Polling-Mode there is no race condition
    #if defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    HIF_ENTER();
    #endif

	bIsLdDevice = hif_is_ld_device(pHifInst);

	if(bIsLdDevice) /* LD Poll */
	{
		/* send IRQ poll if there is something to receive or there is something in the send queue*/
		if((pHifInst->bRcvEnabled == LSA_TRUE) && (HIF_IS_NOT_NULL(g_pHifData->LD.pRQBPoll)) && (pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE ))
		{
			pRQBPoll = g_pHifData->LD.pRQBPoll;
			g_pHifData->LD.pRQBPoll = LSA_NULL;
			HIF_LD_DO_POLL_REQUEST(pRQBPoll);

			bRQBSent = LSA_TRUE;
		}
	}
	else /* HD Poll */
	{
		/* Poll resource available? */
		if(HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll))
		{
			/* send IRQ poll if there is something to receive */
			if((pHifInst->bRcvEnabled == LSA_TRUE) && (HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll)) && ( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE ))
			{
				pRQBPoll = g_pHifData->HD.pRQBPoll;
				g_pHifData->HD.pRQBPoll = LSA_NULL;
				HIF_HD_DO_POLL_REQUEST(pRQBPoll);

				bRQBSent = LSA_TRUE;
			}
		}
	}
    #if defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    HIF_EXIT();
    #endif
	return bRQBSent;
}

/* description: see header */
LSA_VOID hif_interrupt(HIF_HANDLE hH)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_RQB_PTR_TYPE  pRQBIsr;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_interrupt(+) - hH(0x%x)", hH);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	hif_get_instance(&pHifInst, hH);

	if(HIF_IS_NOT_NULL(pHifInst))
	{
		/* ISR resource available? */
		if(HIF_IS_NOT_NULL(pHifInst->pRQBIsr))
		{
			pRQBIsr = pHifInst->pRQBIsr;
			pHifInst->pRQBIsr = LSA_NULL;
			pHifInst->func.DO_INTERRUPT_REQUEST(pRQBIsr);
		}
	}
}

/**
 * \brief finish an HIF system request
 * 
 * @param [in] pRQB         - pointer to the RQB
 * @param [in] response     - This response is set in the RQB
 * @return
 */
LSA_VOID hif_system_callback(HIF_RQB_PTR_TYPE pRQB, LSA_UINT16 response)
{
	LSA_UINT16 rqbResponse;
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf = HIF_FCT_PTR_NULL;
	
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_system_callback(+) - pRQB(0x%x), response(0x%x)", pRQB, response);

	switch (HIF_RQB_GET_OPCODE(pRQB))
	{
		case HIF_OPC_LD_UPPER_OPEN:
		{
			pCbf = pRQB->args.dev_ld_open.Cbf;
			break;
		}
		case HIF_OPC_HD_UPPER_OPEN:
		{
			pCbf = pRQB->args.dev_hd_open.Cbf;
			break;
		}
		case HIF_OPC_LD_LOWER_OPEN:
		case HIF_OPC_HD_LOWER_OPEN:
		{	    
			pCbf = pRQB->args.dev_lower_open.Cbf;
			break;
		}
		case HIF_OPC_LD_UPPER_CLOSE:
		case HIF_OPC_LD_LOWER_CLOSE:
		case HIF_OPC_HD_UPPER_CLOSE:
		case HIF_OPC_HD_LOWER_CLOSE:
		{
			pCbf = pRQB->args.dev_close.Cbf;
			break;
		}      
		default:
		{
			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_system_callback(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
			HIF_FATAL(0);
		}
	}	
	
	rqbResponse = hif_map_lsa_to_rqb_response(response);
	
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_system_callback pRQB(0x%x) response(0x%x)",pRQB,rqbResponse);
	
	HIF_RQB_SET_RESPONSE(pRQB, rqbResponse);
	
	if(HIF_FCT_IS_NOT_NULL(pCbf))
	{
		switch (HIF_RQB_GET_OPCODE(pRQB))
		{
			case HIF_OPC_LD_UPPER_OPEN:
			case HIF_OPC_LD_UPPER_CLOSE:
			{
				HIF_LD_U_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}
			case HIF_OPC_HD_UPPER_OPEN:
			case HIF_OPC_HD_UPPER_CLOSE:
			{
				HIF_HD_U_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}
			case HIF_OPC_LD_LOWER_OPEN:
			case HIF_OPC_LD_LOWER_CLOSE:
			{	    
				HIF_LD_L_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}
			case HIF_OPC_HD_LOWER_OPEN:
			case HIF_OPC_HD_LOWER_CLOSE: 
			{	    
				HIF_HD_L_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}  
			default:
			{
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_system_callback(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
				HIF_FATAL(0);
			}
		}
	}
}

/* description: see header */
LSA_VOID hif_fatal_error(
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32	       error_code_0,
	LSA_UINT32	       error_code_1,
	LSA_UINT32	       error_code_2,
	LSA_UINT32	       error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data)
{
	hif_glob_fatal_error.lsa_component_id	= LSA_COMP_ID_HIF;
	hif_glob_fatal_error.module_id			= module_id;
	hif_glob_fatal_error.line				= line;

	hif_glob_fatal_error.error_code[0]		= error_code_0;
	hif_glob_fatal_error.error_code[1]		= error_code_1;
	hif_glob_fatal_error.error_code[2]		= error_code_2;
	hif_glob_fatal_error.error_code[3]		= error_code_3;

	hif_glob_fatal_error.error_data_length	= error_data_len;
	hif_glob_fatal_error.error_data_ptr		= error_data;

	HIF_FATAL_ERROR ((LSA_UINT16)(sizeof(hif_glob_fatal_error)), &hif_glob_fatal_error);
}

#if (HIF_CFG_USE_DMA == 1)
/* description: see header */
LSA_VOID hif_buffer_admin_init(LSA_VOID)
{
    LSA_UINT32 i;

    for (i = 0; i < HIF_BUFFER_ADMIN_LIST_ENTRIES; i++)
    {
        g_pHifData->BufferAdministration.buffer_list[i].pBuffer       = LSA_NULL;
        g_pHifData->BufferAdministration.buffer_list[i].pReadyForFree = LSA_NULL;
        g_pHifData->BufferAdministration.buffer_list[i].compId        = 0;
    }
    g_pHifData->BufferAdministration.buffer_list_next_entry = 0;
    g_pHifData->BufferAdministration.bIsEmpty               = LSA_TRUE;
}

/* description: see header */
LSA_VOID hif_buffer_admin_add(LSA_VOID* pBuffer, LSA_UINT16 compId, LSA_UINT16 memType, HIF_HD_ID_TYPE hd_id, LSA_UINT32 expectedFinsihedFlag, LSA_UINT32 *pReadyForFree)
{
    LSA_UINT16 retVal;
    
    // Check if adress is valid where magic value will be written
    HIF_ASSERT(pReadyForFree);
    // Check if the actual entry is free
    if(g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pReadyForFree != LSA_NULL)
    {
        // Acutal entry is used, free it if possible.
        if (*(g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pReadyForFree) == 
              g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].expectedMagicValue)
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_buffer_admin_add() - list is full, free first pBuffer: 0x%08x", g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pBuffer);
            HIF_FREE_BUFFER_IMMEDIATE(&retVal, g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pBuffer, 
                                               g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].compId, 
                                               g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].memType, 
                                               g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].hd_id);
            HIF_ASSERT(retVal == HIF_OK);

            // reset magic value
            *(g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pReadyForFree) = 0x0; // mark as free for dma
            g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pReadyForFree = LSA_NULL;
            g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pBuffer = LSA_NULL;
        }
        else
        {
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_buffer_admin_add() list is full and first entry can't be freed!");
            HIF_FATAL(0);
        }
    }

    g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pBuffer            = pBuffer;
    g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].compId             = compId;
    g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].memType            = memType;
    g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].hd_id              = hd_id;
    g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].pReadyForFree      = pReadyForFree;
    g_pHifData->BufferAdministration.buffer_list[g_pHifData->BufferAdministration.buffer_list_next_entry].expectedMagicValue = expectedFinsihedFlag;

    g_pHifData->BufferAdministration.buffer_list_next_entry++;
    g_pHifData->BufferAdministration.buffer_list_next_entry %= HIF_BUFFER_ADMIN_LIST_ENTRIES;
    g_pHifData->BufferAdministration.bIsEmpty = LSA_FALSE;
}

/* description: see header */
LSA_VOID hif_buffer_admin_free(LSA_VOID)
{
    LSA_UINT32 i;
    LSA_UINT16 retVal;

    if (g_pHifData->BufferAdministration.bIsEmpty == LSA_FALSE)
    {
        g_pHifData->BufferAdministration.bIsEmpty = LSA_TRUE;

        for (i = 0; i < HIF_BUFFER_ADMIN_LIST_ENTRIES; i++)
        {
            if (g_pHifData->BufferAdministration.buffer_list[i].pReadyForFree != LSA_NULL)
            {
                if (*(g_pHifData->BufferAdministration.buffer_list[i].pReadyForFree) == g_pHifData->BufferAdministration.buffer_list[i].expectedMagicValue)
                {
                    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_buffer_admin_free() - free pBuffer: 0x%08x", g_pHifData->BufferAdministration.buffer_list[i].pBuffer);
                    HIF_FREE_BUFFER_IMMEDIATE(&retVal, g_pHifData->BufferAdministration.buffer_list[i].pBuffer, g_pHifData->BufferAdministration.buffer_list[i].compId, g_pHifData->BufferAdministration.buffer_list[i].memType, g_pHifData->BufferAdministration.buffer_list[i].hd_id);
                    HIF_ASSERT(retVal == HIF_OK);
    
                    // reset magic value
                    *(g_pHifData->BufferAdministration.buffer_list[i].pReadyForFree) = 0x0; // mark as free for dma
                    g_pHifData->BufferAdministration.buffer_list[i].pReadyForFree = LSA_NULL;
                    g_pHifData->BufferAdministration.buffer_list[i].pBuffer = LSA_NULL;
                }
                else
                {
                    g_pHifData->BufferAdministration.bIsEmpty = LSA_FALSE;
                }
            }
        }
    }
}

/* description: see header */
LSA_VOID hif_buffer_admin_undo_init(LSA_VOID)
{
    hif_buffer_admin_free();
}

#endif // (HIF_CFG_USE_DMA == 1)

#endif // (HIF_CFG_USE_HIF == 1)

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
