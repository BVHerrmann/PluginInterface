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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_agent.cpp                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver agent - startup /config /shutdown                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID         1110
#define PND_MODULE_ID             1110
#define EDDI_DEFAULT_SEND_CLOCK   32

#include "pndriver_version.h"

#include "pnd_int.h"
#include "pnd_sys.h"

#include "pniobase.h"
#include "servusrx.h"
#include "pnioerrx.h"

/* StdLib++ */
#include <vector>

using namespace std;

#include "pnd_IOBaseCoreCommon.h"
#include "pnd_pnio_user_core.h"
#include "pnd_ParamStore.h"  // component params

#include "pntrc_inc.h"
#include "pnd_trc.h"

#if defined (EPS_CFG_USE_PNDEVDRV) && !defined (PSI_EDDS_CFG_HW_LPCAP)
#include "eps_pndevdrv.h"
#include "eps_pndevdrv_helper.h"
#elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
#define HAVE_REMOTE
#include "pcap.h"
#include <packet32_inc.h>      /* EDDS Lower Layer Intel Driver            */
#include "packet32.h"
#endif
#ifdef PSI_EDDS_CFG_HW_LPCAP
#include "pcap.h"
#include "libpcap_inc.h"
#include <string.h>
#include <errno.h>
#endif

PND_FILE_SYSTEM_EXTENSION(PNDRIVER_MODULE_ID)


/*---------------------------------------------------------------------------*/
/* PnDriver instance                                                         */
/*---------------------------------------------------------------------------*/

// TODO hier müssen die globalen Instance daten des Agents ergänzt werden

// have to be a global structure instead of malloc/free because memory pools are not available at the time when used
static PNIO_DEBUG_SETTINGS_TYPE g_pnio_debug_settings;
/*---------------------------------------------------------------------------*/
/* types                                                                     */
/*---------------------------------------------------------------------------*/

enum pnd_agent_status
{ 
    PND_AGENT_STATUS_STOP = 42,
    PND_AGENT_STATUS_RUN
};


typedef struct pnd_instance_tag {
    PNIO_BOOL                 init_done;            // Init sequence done ?
    PNIO_BOOL                 serv_cp_init_done;    // SERV CP Init sequence done ?
    PNIO_UINT16               wait_event;           // Wait event

    EPS_DATA_PTR_TYPE        eps_sys_data_ptr;     // EPS data IF for core (read access)

    PND_PARAM_STARTUP_TYPE   pnd_startup_param;
    pnd_agent_status         status;
} PNDRIVER_INSTANCE_TYPE;


#if (EPS_PLF == EPS_PLF_WINDOWS_X86) && !defined(EPS_CFG_USE_PNDEVDRV)

#define PND_CFG_WPCAPDRV_MAX_BOARDS 50

typedef struct eps_stdmacdrv_device_info_tag
{
	LSA_CHAR*  pName;
	LSA_CHAR   Name[300];
	PNIO_UINT8  uMac[6];
	LSA_CHAR   description[300];
} PND_WPCAPDRV_DEVICE_INFO_TYPE, *PND_WPCAPDRV_DEVICE_INFO_PTR_TYPE;


typedef struct eps_wpcapdrv_store_tag
{
	PND_WPCAPDRV_DEVICE_INFO_TYPE foundDevices[PND_CFG_WPCAPDRV_MAX_BOARDS];
	PNIO_UINT32 uCntFoundDevices;

} EPS_WPCAPDRV_STORE_TYPE, *EPS_WPCAPDRV_STORE_PTR_TYPE;

#endif

/*---------------------------------------------------------------------------*/
/* instance data                                                             */
/*---------------------------------------------------------------------------*/

static PNDRIVER_INSTANCE_TYPE pnd_instance = { PNIO_FALSE };

/*---------------------------------------------------------------------------*/
/* prototypes                                                                */
/*---------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_iob_core_startup( PND_PARAM_STARTUP_CONST_PTR_TYPE pnd_startup_param );
static PNIO_VOID   pnd_agent_iob_core_startup_done(PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_iob_core_shutdown(PNIO_VOID);
static PNIO_VOID   pnd_agent_iob_core_shutdown_done(PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_pnio_user_core_startup( PND_PARAM_STARTUP_CONST_PTR_TYPE pnd_startup_param );
static PNIO_VOID   pnd_agent_pnio_user_core_startup_done(PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_pnio_user_core_shutdown(PNIO_VOID);
static PNIO_VOID   pnd_agent_pnio_user_core_shutdown_done(PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_shutdown(PNIO_VOID);
//PNIO_VOID pnd_sys_init_done(PNIO_VOID); /* commented out to avoid the warning: */
                                          /* Unused declaration of function 'pnd_sys_init_done' */

/*---------------------------------------------------------------------------*/
static PNIO_VOID pnd_sys_undo_init(PNIO_VOID)
{
    PNIO_UINT32 result = PNIO_OK;

    result = pnd_free_event(pnd_instance.wait_event);
	PND_ASSERT( result == PNIO_RET_OK );

    pnd_mbx_sys_undo_init();

    pnd_threads_undo_init();

    eps_undo_init();
}

/*---------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_startup(
    PNIO_CP_ID_PTR_TYPE           CpList,
    PNIO_UINT32                   NrOfCp,
    PNIO_UINT8                  * pConfigData,
    PNIO_UINT32                   ConfigDataLen,
	PNIO_UINT8                  * pRemaData,
    PNIO_UINT32                   RemaDataLen,
    PNIO_SYSTEM_DESCR           * pSysDescr)
{
    PNIO_UINT32 result = PNIO_OK;

    pnd_pnio_user_core_init();

    pnd_iobase_core_init();

    // Create the Param storage, used for LD startup
    CParamStore *paramObj = CParamStore::GetInstance();

    if ( paramObj != 0 ) 
    {
        result = paramObj->Create( NrOfCp, CpList, pConfigData, ConfigDataLen, pRemaData, RemaDataLen, pSysDescr );

        if ( result == PNIO_OK ) // param config correct ?
        {
            EPS_OPEN_TYPE uOpen;
            PNIO_UINT16 hdNr = 0;
            PND_PARAM_PSI_LD_TYPE psiParams; 
            PND_PARAM_HIF_LD_TYPE hifParam;            
            
            #if defined(PND_DEBUG)
            LSA_UINT16 timeoutSec = 0;
            #else
            LSA_UINT16 timeoutSec = 30;
            #endif
 
            pnd_instance.eps_sys_data_ptr = g_pEpsData;  // see eps_sys.h

            //Get HIF Params
            paramObj->GetParamHifLD( &hifParam );

            //Get PSI Params
            paramObj->GetParamPsiLD( &psiParams );
           
            PSI_MEMSET(&uOpen, 0, sizeof(EPS_OPEN_TYPE));
    
            uOpen.hd_count      = psiParams.hd_count;
            for (hdNr=0; hdNr<PSI_CFG_MAX_HD_CNT; hdNr++)
            {
                uOpen.hd_args[hdNr]   = psiParams.hd_param[hdNr];
                #if (PSI_CFG_USE_EDDI == 1)
                uOpen.hd_args[hdNr].send_clock_factor = EDDI_DEFAULT_SEND_CLOCK;
                #endif
            }
           
            #if defined(PND_CFG_BASIC_VARIANT)
            uOpen.fw_debug_activated = LSA_FALSE;
            uOpen.shm_debug_activated = LSA_FALSE;
            uOpen.shm_halt_activated = LSA_FALSE;
            uOpen.ldRunsOnType = PSI_LD_RUNS_ON_BASIC;
            #else
            uOpen.ldRunsOnType = PSI_LD_RUNS_ON_LIGHT;
            #endif

            uOpen.imInput      = psiParams.im_param;
            uOpen.pUsrHandle   = PNIO_NULL;

             /* startup EPS (open LD), wait for complete */
            eps_open(&uOpen, PNIO_NULL/*PNIO_NULL = without Cbf -> wait*/, timeoutSec);

            for (hdNr=0; hdNr < psiParams.hd_count; hdNr++)
            {
                // update out params from PSI
                paramObj->SetParamPsiHD( hdNr+1, &uOpen.hd_out[hdNr] );  // Interface specific ???
				paramObj->SetParamIrteBase(hdNr, &uOpen.io_param_out[hdNr]);
            }           

            result = paramObj->GetParamStartup( &pnd_instance.pnd_startup_param );
            if (result != PNIO_OK)
            {
                pnd_agent_shutdown();
                return result;
            }            
            
            for (hdNr = 0; hdNr < psiParams.hd_count; hdNr++)
            {
                pnd_instance.pnd_startup_param.ioAddress[hdNr] = uOpen.io_param_out[hdNr];
            }

            result = pnd_agent_iob_core_startup(&pnd_instance.pnd_startup_param);
            if (result != PNIO_OK)
            {
                pnd_agent_shutdown();
                return result;
            }

            // prepare startup request pnio_user_core
            result = pnd_agent_pnio_user_core_startup(&pnd_instance.pnd_startup_param);
            if (result != PNIO_OK)
            {
                pnd_agent_shutdown();
                return result;
            }
        }
        else
        {
           pnd_instance.serv_cp_init_done = PNIO_FALSE;
            
           pnd_iobase_core_undo_init();

           CParamStore:: DeleteInstance();

           pnd_pnio_user_core_undo_init();

           pnd_sys_undo_init();
        }

    }
    else
    {
        pnd_instance.serv_cp_init_done = PNIO_FALSE;

        pnd_iobase_core_undo_init();

        pnd_pnio_user_core_undo_init();

        pnd_sys_undo_init();

        result = PNIO_ERR_CREATE_INSTANCE;
    }
    
    return ( result );
}

/*---------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_shutdown(PNIO_VOID)
{
    PNIO_UINT32 result;
    EPS_CLOSE_TYPE uClose;  

    #if defined(PND_DEBUG)
    LSA_UINT16 timeoutSec = 0;
    #else
    LSA_UINT16 timeoutSec = 15;
    #endif

    result = pnd_agent_iob_core_shutdown();
    if (result != PNIO_OK)
    {
        return result;
    }

    pnd_agent_pnio_user_core_shutdown();

	 result = pnd_pnio_user_core_undo_init();
	 if (result != PNIO_OK)
	 {
		 return result;
	 }

    pnd_iobase_core_undo_init();
    
    CParamStore::DeleteInstance();
    
    eps_close(&uClose, PNIO_NULL, timeoutSec);

    return PNIO_OK;
}

// Example for HIF, PSI system posting 
/*---------------------------------------------------------------------------*/
/* PnDriver request function to EPS system  (using HIF or PSI)               */
/*---------------------------------------------------------------------------*/
PNIO_VOID pndriver_core_system_request_lower( PNIO_VOID_PTR_TYPE rqb_ptr )
{
    /* PN system request to a lower PNIO comp (i.E.: CM) over HIF LD */   
	PND_ASSERT( rqb_ptr != 0 );

    psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system, rqb_ptr );
}

/*----------------------------------------------------------------------------*/
PNIO_VOID pndriver_core_open_channel_lower( PNIO_VOID_PTR_TYPE rqb_ptr )
{
    /* PN open channel request to a lower PNIO comp (i.E.: CM) over HIF LD */
	PND_ASSERT( rqb_ptr != 0 );

    psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_open_channel, rqb_ptr );
}

/*----------------------------------------------------------------------------*/
/*lint -e{818} Pointer parameter 'p_rqb' could be declared as pointing to const*/
static PNIO_VOID pnd_agent_iob_core_startup_done(PNIO_VOID_PTR_TYPE p_rqb)
{
    // runs in context of iobase core
	PND_ASSERT( p_rqb != 0 );
    pnd_set_event( pnd_instance.wait_event );
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_iob_core_startup( PND_PARAM_STARTUP_CONST_PTR_TYPE pnd_startup_param )
{
    PNIO_UINT32 result = PNIO_OK;

    PND_RQB_PTR_TYPE              pRQB;
    PND_STARTUP_PTR_TYPE          pStart;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
	PND_ASSERT( pRQB );
	
    pStart = /*lint -e(613) */ &pRQB->args.startup;

    pStart->startup_param   = *pnd_startup_param;
    pStart->cbf = pnd_agent_iob_core_startup_done;
    pStart->pnio_err = PNIO_OK;
	/*lint -e(613) */
    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_STARTUP );

    // Wait on callback, after request 
    pnd_reset_event( pnd_instance.wait_event );

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pnd_instance.wait_event );

    result = pStart->pnio_err;

    pnd_mem_free(pRQB);

    return result;
}

/*----------------------------------------------------------------------------*/
/*lint -e{818} Pointer parameter 'p_rqb' could be declared as pointing to const */
static PNIO_VOID pnd_agent_pnio_user_core_startup_done(PNIO_VOID_PTR_TYPE p_rqb)
{
    // runs in context of iobase core
	PND_ASSERT( p_rqb != 0 );
    pnd_set_event( pnd_instance.wait_event );
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_pnio_user_core_startup( PND_PARAM_STARTUP_CONST_PTR_TYPE pnd_startup_param )
{
    PNIO_UINT32 result = PNIO_OK;

    PND_RQB_PTR_TYPE              pRQB;
    PND_STARTUP_PTR_TYPE          pStart;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
	PND_ASSERT( pRQB );

    pStart = /*lint -e(613) */ &pRQB->args.startup;

    pStart->startup_param   = *pnd_startup_param;
    pStart->cbf = pnd_agent_pnio_user_core_startup_done;
    pStart->pnio_err = PNIO_OK;
	/*lint -e(613) */
    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_STARTUP );

    // Wait on callback, after request 
    pnd_reset_event( pnd_instance.wait_event );

    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pnd_instance.wait_event );

    result = pStart->pnio_err;

    pnd_mem_free(pRQB);

    return result;
}

/*----------------------------------------------------------------------------*/
/*lint -e{818} Pointer parameter 'p_rqb' could be declared as pointing to const */
static PNIO_VOID pnd_agent_iob_core_shutdown_done(PNIO_VOID_PTR_TYPE p_rqb)
{
    // runs in context of iobase core
	PND_ASSERT( p_rqb != 0 );
    pnd_set_event( pnd_instance.wait_event );
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_iob_core_shutdown( PNIO_VOID )
{
    PNIO_UINT32 result = PNIO_OK;

    PND_RQB_PTR_TYPE              pRQB;
    PND_SHUTDOWN_PTR_TYPE         pStop;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
	PND_ASSERT( pRQB );

    pStop = /*lint -e(613) */ &pRQB->args.shutdown;

    pStop->cbf = pnd_agent_iob_core_shutdown_done;
    pStop->pnio_err = PNIO_OK;
	/*lint -e(613) */
    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_SHUTDOWN );

    // Wait on callback, after request 
    pnd_reset_event( pnd_instance.wait_event );

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pnd_instance.wait_event );

    result = pStop->pnio_err;

    pnd_mem_free(pRQB);

    return result;
}

/*----------------------------------------------------------------------------*/
/*lint -e{818} Pointer parameter 'p_rqb' could be declared as pointing to const */
static PNIO_VOID pnd_agent_pnio_user_core_shutdown_done(PNIO_VOID_PTR_TYPE p_rqb)
{
    // runs in context of iobase core 
	PND_ASSERT( p_rqb != 0 );
    pnd_set_event( pnd_instance.wait_event );
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_pnio_user_core_shutdown(PNIO_VOID )
{
    PNIO_UINT32 result = PNIO_OK;

    PND_RQB_PTR_TYPE              pRQB;
    PND_SHUTDOWN_PTR_TYPE         pStop;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
	PND_ASSERT( pRQB );

    pStop = /*lint -e(613) */ &pRQB->args.shutdown;

    pStop->cbf = pnd_agent_pnio_user_core_shutdown_done;
    pStop->pnio_err = PNIO_OK;
	/*lint -e(613) */
    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_SHUTDOWN );

    // Wait on callback, after request 
    pnd_reset_event( pnd_instance.wait_event );

    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pnd_instance.wait_event );

    result = pStop->pnio_err;

    pnd_mem_free(pRQB);

    return result;
}

/*---------------------------------------------------------------------------*/
/* Implementation of IOBASE Interface                                        */
/*---------------------------------------------------------------------------*/

PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_startup (  
    PNIO_CP_ID_PTR_TYPE           CpList,
    PNIO_UINT32                   NrOfCp,
    PNIO_UINT8                  * pConfigData,
    PNIO_UINT32                   ConfigDataLen,
	PNIO_UINT8                  * pRemaData,
    PNIO_UINT32                   RemaDataLen,
    PNIO_SYSTEM_DESCR           * pSysDescr)
{
    PNIO_UINT32 result = PNIO_OK;

    if ((pnd_instance.init_done == PNIO_TRUE) || (pnd_instance.status == PND_AGENT_STATUS_RUN) ||
        (pnd_instance.serv_cp_init_done == PNIO_FALSE))
        return PNIO_ERR_SEQUENCE;

    pnd_instance.status = PND_AGENT_STATUS_STOP;

    if (( NrOfCp > PSI_CFG_MAX_HD_CNT ) || ( ConfigDataLen == 0 ))  // invalid value args ?
    {
        result = PNIO_ERR_PRM_INVALIDARG;
    }
    else
    {
        if ( ( CpList == 0 ) || ( pConfigData == 0 )) // invalid pointer args  ?
        {
            result = PNIO_ERR_PRM_POINTER;
        }
        else
        {
            if ( pnd_instance.status == PND_AGENT_STATUS_STOP )  // invalid state ?
            {
                // startup the iobase system 
                result = pnd_agent_startup( CpList, NrOfCp, pConfigData, ConfigDataLen, pRemaData, RemaDataLen, pSysDescr );

                if ( result == PNIO_OK ) // startup ok ?
                {
                    // change to state "startup done"

                    pnd_instance.init_done = PNIO_TRUE;
                    pnd_instance.status    = PND_AGENT_STATUS_RUN;
                }
                else
                {
                    pnd_instance.init_done = PNIO_FALSE;
                    pnd_instance.status    = PND_AGENT_STATUS_STOP;
                    return( result);
                }

            }
            else
            {
                // invalid state
                result = PNIO_ERR_SEQUENCE;
            }
        }
    }

    return result;
}
/*lint -e{818} Pointer parameter 'DebugSetting' could be declared as pointing to const*/
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_init(PNIO_DEBUG_SETTINGS_PTR_TYPE  DebugSetting)
{
    PNIO_UINT32 result = PNIO_OK;

	if ( pnd_instance.serv_cp_init_done == PNIO_TRUE )
	{
		return PNIO_ERR_SEQUENCE;
	}
	else
	{
		pnd_instance.serv_cp_init_done = PNIO_TRUE;
	}

    //check if DebugSetting == 0 -> disable trace
    if(DebugSetting == 0)
    {
        g_pnio_debug_settings.CbfPntrcBufferFull = PNIO_NULL;
        g_pnio_debug_settings.CbfPntrcWriteBuffer = PNIO_NULL;

        for( PNIO_UINT32 i=0; i < PNIO_TRACE_COMP_NUM; i++ )
        {
            g_pnio_debug_settings.TraceLevels[i] = PNIO_TRACE_LEVEL_OFF;
        }
    }
    else
    {
        g_pnio_debug_settings = *DebugSetting;
    }

    eps_init( ( PNIO_VOID_PTR_TYPE )& g_pnio_debug_settings );
    
    //This is not a "real" warning. We want to make PN Driver version visible in a trace file which is received from 
    //the field for the investigation of a run-time problem.
    PND_AGENT_TRACE_08(0, LSA_TRACE_LEVEL_WARN, "SERV_CP_init(): PN Driver version: %02d.%02d.%02d.%02d_%02d.%02d.%02d.%02d",
        PND_VERSION, PND_DISTRIBUTION, PND_FIX, PND_HOTFIX, PND_PROJECT_NUMBER, PND_INCREMENT, PND_INTEGRATION_COUNTER,
        PND_GEN_COUNTER);

    pnd_mbx_sys_init();

    pnd_threads_init();

    pnd_start_threads();

    result = pnd_alloc_event( &pnd_instance.wait_event );
	PND_ASSERT( result == PNIO_RET_OK );

    PND_STATIC_ASSERT (sizeof(PNIO_INT8) == sizeof(LSA_INT8), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_INT16) == sizeof(LSA_INT16), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_INT32) == sizeof(LSA_INT32), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_UINT8) == sizeof(LSA_UINT8), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_UINT8) == sizeof(LSA_UINT8), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_UINT16) == sizeof(LSA_UINT16), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_UINT32) == sizeof(LSA_UINT32), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_BOOL) == sizeof(LSA_BOOL), "must have same size");
    PND_STATIC_ASSERT (sizeof(PNIO_VOID_PTR_TYPE) == sizeof(LSA_VOID_PTR_TYPE), "must have same size");

    return PNIO_OK;
}

PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_shutdown ()
{
    PNIO_UINT32 result = 0;    

     if ((pnd_instance.init_done == PNIO_FALSE) || (pnd_instance.status == PND_AGENT_STATUS_STOP) || 
        (pnd_instance.serv_cp_init_done == PNIO_FALSE))
        return PNIO_ERR_SEQUENCE;

    result = pnd_agent_shutdown();
    if (result != PNIO_OK)
    {
        return result;
    }
    
    pnd_instance.init_done = PNIO_FALSE;
    pnd_instance.status    = PND_AGENT_STATUS_STOP;

    return result;
}

PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_undo_init ()
{
    if(pnd_instance.serv_cp_init_done == PNIO_FALSE || pnd_instance.status == PND_AGENT_STATUS_RUN)
        return PNIO_ERR_SEQUENCE;

    pnd_instance.serv_cp_init_done = PNIO_FALSE;

    pnd_sys_undo_init();

    return PNIO_OK;
}

static void pntrc_rqb_done(PNIO_VOID* pRQB0)
{
    PNIO_UINT16 rc;
    PNTRC_RQB_PTR_TYPE pRQB;
    PNIO_PNTRC_SET_TRACE_LEVEL_DONE user_cbf;

	PND_ASSERT( pRQB0 != 0 );

    pRQB=(PNTRC_RQB_PTR_TYPE)pRQB0;

    user_cbf = /*lint -e(613) */ (PNIO_PNTRC_SET_TRACE_LEVEL_DONE)pRQB->_user_func;

    PNTRC_FREE_LOCAL_MEM( &rc, pRQB );

	PND_ASSERT( rc == PNIO_RET_OK );

    //call user CBF if there is any
    if(user_cbf != PNIO_NULL)
    {
        user_cbf();
    }
}



PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_set_trace_level ( 
    PNIO_UINT32                       Component,            
    PNIO_UINT32                       TraceLevel,           
    PNIO_PNTRC_SET_TRACE_LEVEL_DONE   CbfSetTraceLevelDone)
{
    PNIO_UINT32 result = PNIO_OK;
    PNTRC_RQB_PTR_TYPE pRQB=0;

    //check state
    if( pnd_instance.init_done == PNIO_FALSE)
    {
        result = PNIO_ERR_SEQUENCE;
    }
    else
    {
        if( (Component < PNIO_TRACE_COMP_NUM) && (TraceLevel < PNIO_TRACE_LEVEL_NUM) )//parameter check
        {
			/*lint -e{1773, 1776} Converting a string literal to char* is not const safe */
            PNTRC_ALLOC_LOCAL_MEM( (void**)&pRQB, sizeof(PNTRC_RQB_TYPE));

            PNTRC_RQB_SET_OPCODE(pRQB,PNTRC_OPC_SET_TRACE_LEVELS_COMP);

            pRQB->args.set_trace_levels_comp.comp = (LTRC_COMP_TYPE)Component;

            pRQB->args.set_trace_levels_comp.level = (PNTRC_LEVEL_TYPE)TraceLevel;

            pRQB->_user_func = (void(*)(void*, void*))CbfSetTraceLevelDone;

            pRQB->Cbf=pntrc_rqb_done;

            pntrc_request(pRQB);
        }
        else
        {
            result = PNIO_ERR_PRM_INVALIDARG;
        }
    }
    return result;
  }


#if defined PSI_EDDS_CFG_HW_LPCAP

// Can't use here eps_pndev_if_get_registered_devices - not yet filled
static PNIO_VOID pnd_lpcapdrv_find_boards(PNIO_CP_ID_PTR_TYPE pCpList, PNIO_UINT8* No_of_CPList)
{
     LSA_INT32                   result;
     pcap_if_t                   *alldevs;
     pcap_if_t                   *dev;
     const LSA_UINT8             *mac;
     char                        errbuf[PCAP_ERRBUF_SIZE];
     LSA_BOOL                    noRealDevice = LSA_FALSE;   /* Set when a non real device is found by pcap e.g. "any" */
     struct ifreq  ifr;
     int sockfd;
     LSA_UINT8                   found_Devices = 0;


     /* Get socket for ioctl-operation */
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     if(sockfd < 0)
     {
         EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "pnd_lpcapdrv_find_boards: Can't get a socket");
         EPS_FATAL("pnd_lpcapdrv_find_boards - couldn't get a socket!");
     }

     /* pcap function to find all network devices */
     result = pcap_findalldevs(&alldevs, errbuf);
     if(result < 0)
     {
         EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "pnd_lpcapdrv_find_boards: pcap_findalldevs failed with \"%s\"!", errbuf);
         EPS_FATAL("pnd_lpcapdrv_find_boards: pcap_findalldevs failed with error-text:" errbuf);
     }

     /* Get all network devices */
     for(dev = alldevs; dev != LSA_NULL; dev = dev->next)
     {
         noRealDevice = LSA_FALSE;

         /* Name of the network-card we want to take a look at */
         strcpy(ifr.ifr_name, dev->name);
         ifr.ifr_addr.sa_family = AF_INET;

         /* Get hardware-address (MAC) of network card */
         result = ioctl(sockfd, SIOCGIFHWADDR, &ifr);
         if(result < 0)
         {
             switch(errno)
             {
             case ENODEV: /* happens for devices like "any" or "usbmon" */
                 noRealDevice = LSA_TRUE;
                 break;
             default:
                 EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "pnd_lpcapdrv_find_boards: ioctl failed with errno %d!", errno);
                 EPS_FATAL("pnd_lpcapdrv_find_boards: ioctl failed with errno " errno);
                 break;
             }
         }

         /* If it isn't a real device skip setting it in registered devices */
         if(noRealDevice == LSA_TRUE || !strcmp(dev->name, "lo"))
         {
             continue;
         }

         mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;

         pCpList[found_Devices].CpMacAddr[0] = mac[0];
         pCpList[found_Devices].CpMacAddr[1] = mac[1];
         pCpList[found_Devices].CpMacAddr[2] = mac[2];
         pCpList[found_Devices].CpMacAddr[3] = mac[3];
         pCpList[found_Devices].CpMacAddr[4] = mac[4];
         pCpList[found_Devices].CpMacAddr[5] = mac[5];

         eps_strcpy(pCpList[found_Devices].Description, dev->name);
         pCpList[found_Devices].CpSelection = PNIO_CP_SELECT_WITH_MAC_ADDRESS;

         found_Devices++;
    }

    *No_of_CPList = (PNIO_UINT8)found_Devices;

    close(sockfd);
    pcap_freealldevs(alldevs);
}
#endif


#if (EPS_PLF == EPS_PLF_WINDOWS_X86 && !defined(EPS_CFG_USE_PNDEVDRV))

static PNIO_VOID pnd_wpcapdrv_find_boards(EPS_WPCAPDRV_STORE_PTR_TYPE pPndWpcapDrv)
{
    PNIO_INT32 result;
    pcap_if_t *alldevs;
    pcap_if_t *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    LPADAPTER lpAdapter = 0;
    char dev_name[PCAP_BUF_SIZE] = {0};
	
	struct
	{
		PACKET_OID_DATA pkt;
		char more[6];
	} buf;
	
	PND_ASSERT( pPndWpcapDrv != PNIO_NULL );

    pnd_memset(pPndWpcapDrv, 0, sizeof(EPS_WPCAPDRV_STORE_TYPE));

	/*lint -e{1776} Converting a string literal to char* is not const safe*/
	result = pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf);
    
    if(result != -1)
    {
        for(dev = alldevs; (dev != PNIO_NULL) && (pPndWpcapDrv->uCntFoundDevices < PND_CFG_WPCAPDRV_MAX_BOARDS); dev = dev->next)
        {
			/*lint -e{737} Loss of sign in promotion from int to unsigned int*/
            pnd_memcpy(dev_name, dev->name + pnd_strlen(PCAP_SRC_IF_STRING), static_cast<unsigned int>(pnd_strlen(dev->name) - pnd_strlen(PCAP_SRC_IF_STRING)));

            // query the mac-address
            lpAdapter = PacketOpenAdapter(dev_name);

			PND_ASSERT( 0 != lpAdapter );

			buf.pkt.Oid    = OID_802_3_CURRENT_ADDRESS;
			buf.pkt.Length = 6;

            if( PacketRequest(lpAdapter, PNIO_FALSE, &buf.pkt) && buf.pkt.Length == 6 )
            {
                pnd_memcpy(pPndWpcapDrv->foundDevices[pPndWpcapDrv->uCntFoundDevices].description, dev->description, pnd_strlen(dev->description));
				for (unsigned int i = 0; i < static_cast<unsigned int> (buf.pkt.Length); i++)
				{
					/*lint -e{737} Loss of sign in promotion from int to unsigned int*/
					pPndWpcapDrv->foundDevices[pPndWpcapDrv->uCntFoundDevices].uMac[i] = buf.pkt.Data[i];
				}
                pPndWpcapDrv->uCntFoundDevices++;
            }
            PacketCloseAdapter(lpAdapter);
        }

    }

    pcap_freealldevs(alldevs);
}

static PNIO_UINT32 pnd_get_mac_wpcapdrv( PNIO_CP_ID_PTR_TYPE cp_list, PNIO_UINT8 *NumberOfCps)
{
    PNIO_CP_ID_PTR_TYPE pCpList = cp_list;
    PND_WPCAPDRV_DEVICE_INFO_PTR_TYPE pDevice;
    PNIO_UINT8* subchar_array;
    EPS_WPCAPDRV_STORE_TYPE pndWpcapDrv;

    pnd_wpcapdrv_find_boards(&pndWpcapDrv);

    if(pndWpcapDrv.uCntFoundDevices == 0)
    {
        return PNIO_ERR_NO_ADAPTER_FOUND;
    }

	PND_ASSERT(pCpList != PNIO_NULL);
	
    for(PNIO_UINT8 i = 0; i < pndWpcapDrv.uCntFoundDevices; i++)
    {
        pDevice = &pndWpcapDrv.foundDevices[i];

		PND_ASSERT(pDevice != 0);

		/*lint -esym(613, pCpList, pDevice) */
		pCpList[i].CpSelection = PNIO_CP_SELECT_WITH_MAC_ADDRESS;
		pCpList[i].CpMacAddr[0] = pDevice->uMac[0];
		pCpList[i].CpMacAddr[1] = pDevice->uMac[1];
		pCpList[i].CpMacAddr[2] = pDevice->uMac[2];
		pCpList[i].CpMacAddr[3] = pDevice->uMac[3];
		pCpList[i].CpMacAddr[4] = pDevice->uMac[4];
		pCpList[i].CpMacAddr[5] = pDevice->uMac[5];
		
        subchar_array = (PNIO_UINT8*) strtok(pDevice->description, "'");
        
        if(subchar_array != PNIO_NULL)
        {
            subchar_array = (PNIO_UINT8*) strtok(PNIO_NULL, "'");
			PND_ASSERT( 0 != pCpList[i].Description );
			PND_ASSERT( 0 != subchar_array );
            pnd_memcpy( pCpList[i].Description, subchar_array, pnd_strlen((LSA_CHAR*)subchar_array) );
        } 
    }
	/*lint -restore */
    *NumberOfCps = (PNIO_UINT8) pndWpcapDrv.uCntFoundDevices;

    return PNIO_OK;
}

#endif

#if defined (EPS_CFG_USE_PNDEVDRV) && !defined (PSI_EDDS_CFG_HW_LPCAP)
/*lint -e{818} Pointer parameter 'pPciLocation' could be declared as pointing to const*/
static ePNDEV_BOARD pnd_get_pci_board_type( PNIO_PCI_LOCATION_PTR_TYPE pPciLocation)
{
	/*lint -e{530} Symbol EPS_PNDEV_LOCATION_TYPE is not initialized*/
	EPS_PNDEV_LOCATION_TYPE loc;
	uPNDEV_GET_DEVICE_INFO_OUT* pDevice = PNIO_NULL;

	loc.eLocationSelector = EPS_PNDEV_LOCATION_PCI;
	loc.uBusNr = pPciLocation->BusNr;
	loc.uDeviceNr = pPciLocation->DeviceNr;
	loc.uFctNr = pPciLocation->FunctionNr;

	eps_pndevdrv_get_device(&loc, &pDevice);
	PND_ASSERT(pDevice != PNIO_NULL);

	return pDevice->eBoard;
}

static PNIO_UINT32 pnd_get_pci_pndevdrv( PNIO_CP_ID_PTR_TYPE cp_list, PNIO_UINT8 *NumberOfCps )
{
    PNIO_CP_ID_PTR_TYPE pCpList = cp_list;
    EPS_PNDEV_IF_DEV_INFO_TYPE  sPnBoard[10]; // Count must be same as BoardInfoStruct[]
    PNIO_UINT16 deviceCount = 0;
    PNIO_UINT8 validDeviceCount = 0;

    eps_pndev_if_get_registered_devices(sPnBoard, 10, &deviceCount);

    for(PNIO_UINT16 i=0; i<deviceCount; i++)
    {
        if(sPnBoard[i].uPciAddrValid == 1)
        {
            PNIO_UINT8 board_name[64] = {0};
            pCpList[i].CpSelection              = PNIO_CP_SELECT_WITH_PCI_LOCATION;
            pCpList[i].CpPciLocation.BusNr      = sPnBoard[i].uPciBusNr;
            pCpList[i].CpPciLocation.DeviceNr   = sPnBoard[i].uPciDeviceNr;
            pCpList[i].CpPciLocation.FunctionNr = sPnBoard[i].uPciFuncNr;
            
            #if defined (EPS_CFG_USE_PNDEVDRV) && !defined (PSI_EDDS_CFG_HW_LPCAP)
            ePNDEV_BOARD eBoard = pnd_get_pci_board_type(&pCpList[i].CpPciLocation);
            switch(eBoard)
		    {
			    case ePNDEV_BOARD_CP1616:                   // CP1616 board
                    eps_strcpy( board_name , "CP1616 board");
                    break;
			    case ePNDEV_BOARD_DB_EB400_PCIE:           	// DB-EB400-PCIe board
                    eps_strcpy( board_name , "DB-EB400-PCIe board");
                    break;
			    case ePNDEV_BOARD_EB200:                    // EB200 board
                    eps_strcpy( board_name , "EB200 board");
                    break;
			    case ePNDEV_BOARD_DB_SOC1_PCI:              // Soc1-Dev board
                    eps_strcpy( board_name , "Soc1-Dev board - PCI");
                    break;
			    case ePNDEV_BOARD_DB_SOC1_PCIE:             // Soc1-Dev board
                    eps_strcpy( board_name , "Soc1-Dev board - PCIE");
                    break;
			    case ePNDEV_BOARD_FPGA1_ERTEC200P:          // FPGA1-Ertec200P board
                    eps_strcpy( board_name , "FPGA1-Ertec200P board");
                    break;
			    case ePNDEV_BOARD_EB200P:                   // EB200P board
                    eps_strcpy( board_name , "EB200P board");
                    break;
			    case ePNDEV_BOARD_I210:                   	// I210 board
                    eps_strcpy( board_name , "I210 board");
                    break;
			    case ePNDEV_BOARD_I82574:                   //I82574 board
			    	eps_strcpy( board_name , "I82574 board");
			    	break;
			    case ePNDEV_BOARD_IX1000:                   //IOT2000 board
                    eps_strcpy( board_name , "IOT2000 board");
                    break;
				case ePNDEV_BOARD_CP1625:                   //CP1625 board
					eps_strcpy(board_name, "CP1625 board"); 
					break;
			    default:
				    PND_FATAL("Can't register a not supported board.");
				    break;
		    }
            #endif

            pnd_memcpy(pCpList[i].Description, board_name, sizeof(board_name));

            validDeviceCount++;
        }
    }

    *NumberOfCps = validDeviceCount;

    if (validDeviceCount == 0)
    {
        return PNIO_ERR_NO_ADAPTER_FOUND;
    }
    
    return PNIO_OK;
}
#endif
/*lint -e{818} Pointer parameter 'NumberOfCps' could be declared as pointing to const */
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_get_network_adapters( PNIO_CP_ID_PTR_TYPE CpList,   // output
							              PNIO_UINT8 *NumberOfCps )     // output
{
    PNIO_UINT32 result = PNIO_OK;

	PND_ASSERT( CpList != PNIO_NULL );
    PND_ASSERT( NumberOfCps != PNIO_NULL );

    #if (EPS_PLF == EPS_PLF_WINDOWS_X86) && !defined(PND_CFG_BASIC_VARIANT)
        result = pnd_get_mac_wpcapdrv(CpList, NumberOfCps);
    #elif (PSI_EDDS_CFG_HW_LPCAP)
        pnd_lpcapdrv_find_boards(CpList, NumberOfCps);
        result = PNIO_OK;
    #elif ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000))
        result = pnd_get_pci_pndevdrv(CpList, NumberOfCps);
    #else
        *NumberOfCps = 0;
    #endif

    return result;				
}



/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
