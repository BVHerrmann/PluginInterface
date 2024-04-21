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
/*  F i l e               &F: eps_vdd.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS VDD Shared Mem Interface Adaption                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20025
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>         /* Types / Prototypes / Fucns        */
#include <eps_locks.h>       /* EPS Locks                         */
#include <eps_cp_hw.h>       /* EPS CP PSI adaption               */
#include <eps_shm_if.h>      /* Shared Mem Interface              */
#include <eps_vdd.h>         /* Own Header                        */
#include <eps_tasks.h>       /* EPS Task Interface                */
#include <eps_plf.h>         /* PCI Interface funcs               */
#include <eps_pn_drv_if.h>   /* EPS PN Device Interface           */
#include <eps_trc.h>         /* Tracing                           */
#include <eps_hif_drv_if.h>  /* HIF-Driver Interface              */
#include <eps_shm_file.h>    /* EPS Shared Mem File handling      */
#include <eps_timer.h>       /* Timer System Interface            */

#include <vdd_Env.h>         /* VDD Interface                       */
#include <vddU.h>            /* VDD Interface                       */
#include <eps_vdd_helper.h>  /* VDD Helper                          */

#include "eps_register.h"    /*EPS_CAST_TO_VOID_PTR*/
/* - Local defines -------------------------------------------------------------------------------- */

#ifndef EPS_SUPPRESS_PRINTF
#include "eps_lib.h"         /* printf */
#define EPS_VDD_PRINTF
#endif

#define EPS_VDD_SELF_STATE_NINIT  0x00000000ul
#define EPS_VDD_SELF_STATE_READY  0x12345678ul
#define EPS_VDD_SELF_STATE_FATAL  0xaffeaffeul
#define EPS_VDD_REMO_STATE_NINIT  0x00000000ul
#define EPS_VDD_REMO_STATE_READY  0x5512AA34ul
#define EPS_VDD_REMO_STATE_CLOSED 0xcafecafeul

#define EPS_VDD_OS_IDENT_INVALID 0
#define EPS_VDD_OS_IDENT_ADONIS  2
//lint --esym(750, EPS_VDD_OS_IDENT_ADONIS) only used by adonis variants
/* - Typedefs ------------------------------------------------------------------------------------- */

typedef struct eps_vdd_wd_tag
{
    LSA_UINT32          hWdThread;
    LSA_UINT32          bWdEnabled;
    LSA_TIMER_ID_TYPE   hWdTimer;
    LSA_UINT32          bAlive;
    LSA_UINT32          uTimeOutCounter;
}EPS_VDD_WD_TYPE;

typedef struct eps_vdd_check_remote_state_tag
{
    LSA_UINT32          bCheckRemoteStateEnabled;
    LSA_TIMER_ID_TYPE   hCheckRemoteStateTimer;
}EPS_VDD_CHECK_REMOTE_STATE_TYPE;

typedef struct {
	LSA_UINT32 uSyncSelf;
	LSA_UINT32 uSyncRemote;
	LSA_UINT32 uSizeSharedMemBlock;
	LSA_UINT32 uSharedMemBlockAddr;
	LSA_UINT32 uHifOffset;
	LSA_UINT32 uHifSize;
	LSA_UINT32 uFatalOffset;
	LSA_UINT32 uFatalSize;
	LSA_UINT32 uTrace1Offset;
	LSA_UINT32 uTrace1Size;
	LSA_UINT32 uTrace2Offset;
	LSA_UINT32 uTrace2Size;
	LSA_UINT32 bDebugOn;
	LSA_UINT32 bFatal;
	LSA_UINT32 uOsIdent;
	struct
	{
		LSA_UINT32 uEddType;
		LSA_UINT32 uPciBusNr;
		LSA_UINT32 uPciDeviceNr;
		LSA_UINT32 uPciFuncNr;
	} BoardInfoStruct[10];
	
    EPS_SHM_FILE_TRANSFER_TYPE FileTransfer;
    EPS_VDD_WD_TYPE Watchdog;
    EPS_VDD_CHECK_REMOTE_STATE_TYPE CheckRemoteState;
    
} EPS_VDD_SHM_HEADER_TYPE, *EPS_VDD_SHM_HEADER_PTR_TYPE;

typedef struct eps_vdd_store_tag
{
	LSA_BOOL bInit;
	LSA_BOOL bOpen;
	LSA_BOOL bFatal;
	LSA_UINT16 hEnterExit;
	EPS_SHM_HW_TYPE sHw;
	EPS_SHM_INSTALL_ARGS_TYPE   sInstallArgs;
	EPS_VDD_SHM_HEADER_PTR_TYPE pShmHeader;
	EPS_HIF_DRV_HW_PTR_TYPE     pHifDrvHwInstOut;
	
	uVDD_DESC               *pDrvHandle;
} EPS_VDD_STORE_TYPE, *EPS_VDD_STORE_PTR_TYPE;

/* - Prototypes ----------------------------------------------------------------------------------- */

#if ( PSI_CFG_USE_PNTRC == 1 )
static LSA_VOID   eps_vdd_trace_buffer_full      (LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength);
#endif

static LSA_VOID   *eps_vdd_wd_service                (LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id);
static LSA_VOID   *eps_vdd_check_remote_state_service(LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id);

static LSA_UINT16 eps_vdd_app_ready              (EPS_SHM_HW_PTR_TYPE pHwInstIn, LSA_VOID* pHifHandle);
static LSA_UINT16 eps_vdd_app_shutdown           (EPS_SHM_HW_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_vdd_sig_fatal              (EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal);

static LSA_BOOL eps_vdd_startup( LSA_VOID );

/* - Global Data ---------------------------------------------------------------------------------- */

static EPS_VDD_STORE_TYPE g_EpsVdd;
static volatile EPS_VDD_STORE_PTR_TYPE g_pEpsVdd;

/* - Source --------------------------------------------------------------------------------------- */

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_undo_init_critical_section(LSA_VOID)
{
	LSA_RESPONSE_TYPE retVal;
	retVal = eps_free_critical_section(g_pEpsVdd->hEnterExit);
	EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_init_critical_section(LSA_VOID)
{
    LSA_UINT16 retVal;
    retVal = eps_alloc_critical_section(&g_pEpsVdd->hEnterExit, LSA_FALSE);
    EPS_ASSERT(retVal == LSA_RET_OK);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsVdd->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsVdd->hEnterExit);
}

/**
 * initialize the check_remote_state_service
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_vdd_init_check_remote_state_service(LSA_VOID)
{
    LSA_USER_ID_TYPE  eps_sys_user_id;
    LSA_UINT16 ret_val;
    LSA_UINT16 uCycle      = 1;
    LSA_UINT16 uRes        = 100; /* LSA_TIME_BASE_100MS */
    LSA_UINT16 uIntervalMs = (LSA_UINT16)(uCycle * uRes);
    
    EPS_ASSERT(g_pEpsVdd->pShmHeader->CheckRemoteState.hCheckRemoteStateTimer == 0);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EPS VDD-CheckRemoteState (lower): eps_vdd_init_check_remote_state_service()");    

    ret_val = eps_timer_allocate_tgroup0(&g_pEpsVdd->pShmHeader->CheckRemoteState.hCheckRemoteStateTimer, LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_100MS, (EPS_TIMER_CBF)eps_vdd_check_remote_state_service);
    EPS_ASSERT(ret_val == LSA_RET_OK);

    eps_sys_user_id.uvar16 = uIntervalMs;

    ret_val = eps_timer_start(g_pEpsVdd->pShmHeader->CheckRemoteState.hCheckRemoteStateTimer, eps_sys_user_id, 1);
    EPS_ASSERT(ret_val == LSA_RET_OK);
    
    g_pEpsVdd->pShmHeader->CheckRemoteState.bCheckRemoteStateEnabled = LSA_TRUE;
}

/**
 * undo init check_remote_state_service
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_vdd_undo_init_check_remote_state_service(LSA_VOID)
{
    LSA_UINT16 ret_val;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EPS VDD-CheckRemoteState (lower): eps_vdd_undo_init_check_remote_state_service()");

    if(g_pEpsVdd->pShmHeader->CheckRemoteState.bCheckRemoteStateEnabled == LSA_TRUE)
    {
        ret_val = eps_timer_stop(g_pEpsVdd->pShmHeader->CheckRemoteState.hCheckRemoteStateTimer);
        EPS_ASSERT(ret_val == LSA_RET_OK);
        ret_val = eps_timer_free(g_pEpsVdd->pShmHeader->CheckRemoteState.hCheckRemoteStateTimer);
        EPS_ASSERT(ret_val == LSA_RET_OK);

        g_pEpsVdd->pShmHeader->CheckRemoteState.hCheckRemoteStateTimer = 0;
        
        g_pEpsVdd->pShmHeader->CheckRemoteState.bCheckRemoteStateEnabled = LSA_FALSE;
    }
}

/**
 * init WatchdogService
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_vdd_init_wd_service(LSA_VOID)
{
    LSA_USER_ID_TYPE  eps_sys_user_id;
    LSA_UINT16 ret_val;
    LSA_UINT16 uCycle      = 1;
    LSA_UINT16 uRes        = 100; /* LSA_TIME_BASE_100MS */
    LSA_UINT16 uIntervalMs = (LSA_UINT16)(uCycle * uRes);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EPS VDD-Watchdog (lower): eps_vdd_init_wd_service()");    

    ret_val = eps_timer_allocate_tgroup0(&g_pEpsVdd->pShmHeader->Watchdog.hWdTimer, LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_100MS, (EPS_TIMER_CBF)eps_vdd_wd_service);
    EPS_ASSERT(ret_val == LSA_RET_OK);

    eps_sys_user_id.uvar16 = uIntervalMs;

    ret_val = eps_timer_start(g_pEpsVdd->pShmHeader->Watchdog.hWdTimer, eps_sys_user_id, 1);
    EPS_ASSERT(ret_val == LSA_RET_OK);
}

/**
 * undo init WatchdogService
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_vdd_undo_init_wd_service(LSA_VOID)
{
    LSA_UINT16 ret_val;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EPS VDD-Watchdog (lower): eps_vdd_undo_init_wd_service()");

    if(g_pEpsVdd->pShmHeader->Watchdog.bWdEnabled == LSA_TRUE)
    {
        ret_val = eps_timer_stop(g_pEpsVdd->pShmHeader->Watchdog.hWdTimer);
        EPS_ASSERT(ret_val == LSA_RET_OK);
        ret_val = eps_timer_free(g_pEpsVdd->pShmHeader->Watchdog.hWdTimer);
        EPS_ASSERT(ret_val == LSA_RET_OK);

        g_pEpsVdd->pShmHeader->Watchdog.bWdEnabled = LSA_FALSE;
    }
}

/**
 * Initialize the shared memory header.
 * Note that this setting must match the settings that are done by the VDD LD Upper implementation.
 *
 * @param [in] uSize size that may be used by the VDD driver
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_vdd_update_device_list( LSA_VOID )
{
	EPS_VDD_SHM_HEADER_PTR_TYPE     pShmHeaderShared = g_pEpsVdd->pShmHeader;
    EPS_PNDEV_IF_DEV_INFO_TYPE      sPnBoard[10]; // Count must be same as BoardInfoStruct[]
    LSA_UINT16                      i,e,f;

	eps_pndev_if_get_registered_devices(sPnBoard, 10, &e); // Max Count must be same as BoardInfoStruct[]

	f=0;

	for(i=0; i<e; i++)
	{
		if(sPnBoard[i].uPciAddrValid == 1)
		{
			pShmHeaderShared->BoardInfoStruct[f].uEddType     = sPnBoard[i].uEddType;
			pShmHeaderShared->BoardInfoStruct[f].uPciBusNr    = sPnBoard[i].uPciBusNr;
			pShmHeaderShared->BoardInfoStruct[f].uPciDeviceNr = sPnBoard[i].uPciDeviceNr;
			pShmHeaderShared->BoardInfoStruct[f].uPciFuncNr   = sPnBoard[i].uPciFuncNr;
			f++;
		}
	}

    if (f>0)
    {
        //found Devices
        return EPS_SHM_RET_OK;
    }
    else
    {
        return EPS_SHM_RET_ERR;
    }
}

static LSA_UINT16 eps_vdd_init_shm_header( LSA_UINT32 uSize )
{
	LSA_UINT16 i;

  	/* calculate real shared mem base ptr */
  	EPS_VDD_SHM_HEADER_PTR_TYPE pShmHeaderShared = g_pEpsVdd->pShmHeader;
  	LSA_UINT32 uHifOffset    = 0x00000000;
  	LSA_UINT32 uHifSize      = 0x00200000;
  	LSA_UINT32 uFatalOffset  = uHifOffset + uHifSize;
  	LSA_UINT32 uFatalSize    = 0x00010000;
  	LSA_UINT32 uPntrc1Offset = uFatalOffset + uFatalSize;
  	LSA_UINT32 uPntrc1Size   = 0x00100000;
  	LSA_UINT32 uPntrc2Offset = uPntrc1Offset + uPntrc1Size;
  	LSA_UINT32 uPntrc2Size   = 0x00100000;
  	LSA_UINT32 uReqSize      = sizeof(EPS_VDD_SHM_HEADER_TYPE) + uHifSize + uFatalSize + uPntrc1Size + uPntrc2Size;

  	if(uSize < uReqSize)
  	{
    	return EPS_SHM_RET_ERR;
  	}

  	pShmHeaderShared->uSyncSelf            = EPS_VDD_SELF_STATE_NINIT;
  	pShmHeaderShared->uSharedMemBlockAddr  = (LSA_UINT32)((LSA_UINT8*)pShmHeaderShared + sizeof(EPS_VDD_SHM_HEADER_TYPE));
  	pShmHeaderShared->uSizeSharedMemBlock  = uSize - sizeof(EPS_VDD_SHM_HEADER_TYPE);
  	pShmHeaderShared->bFatal               = 0;

  	#if defined EPS_USE_RTOS_ADONIS
  	pShmHeaderShared->uOsIdent = EPS_VDD_OS_IDENT_ADONIS;
  	#else
  	pShmHeaderShared->uOsIdent = EPS_VDD_OS_IDENT_INVALID;
  	#endif

  	pShmHeaderShared->uHifOffset    = uHifOffset    + sizeof(EPS_VDD_SHM_HEADER_TYPE);
  	pShmHeaderShared->uHifSize      = uHifSize;
  	pShmHeaderShared->uFatalOffset  = uFatalOffset  + sizeof(EPS_VDD_SHM_HEADER_TYPE);
  	pShmHeaderShared->uFatalSize    = uFatalSize;
  	pShmHeaderShared->uTrace1Offset = uPntrc1Offset + sizeof(EPS_VDD_SHM_HEADER_TYPE);
  	pShmHeaderShared->uTrace1Size   = uPntrc1Size;
  	pShmHeaderShared->uTrace2Offset = uPntrc2Offset + sizeof(EPS_VDD_SHM_HEADER_TYPE);
  	pShmHeaderShared->uTrace2Size   = uPntrc2Size;

    // init device list
  	for(i=0; i<10; i++)
  	{
    	pShmHeaderShared->BoardInfoStruct[i].uEddType     = 0;
    	pShmHeaderShared->BoardInfoStruct[i].uPciBusNr    = 0;
    	pShmHeaderShared->BoardInfoStruct[i].uPciDeviceNr = 0;
    	pShmHeaderShared->BoardInfoStruct[i].uPciFuncNr   = 0;
  	}

  	eps_shm_file_init((LSA_UINT8*)&pShmHeaderShared->FileTransfer);

	pShmHeaderShared->Watchdog.hWdThread          = 0;
	pShmHeaderShared->Watchdog.bAlive             = LSA_FALSE;
	pShmHeaderShared->Watchdog.bWdEnabled         = LSA_FALSE;
	pShmHeaderShared->Watchdog.hWdTimer           = 0;
	pShmHeaderShared->Watchdog.uTimeOutCounter    = 0;
	
	pShmHeaderShared->CheckRemoteState.bCheckRemoteStateEnabled   = LSA_FALSE;
	pShmHeaderShared->CheckRemoteState.hCheckRemoteStateTimer     = 0;

  	return EPS_SHM_RET_OK;
}

/**
 * Opens a HIF driver for the VDD LD Upper IF using the EPS HIF Drv IF.
 *
 * The following capabilies are requested:
 *    bUseIpcInterrupts     := FALSE
 *    bUsePnCoreInterrupts  := FALSE
 *    bUseRemoteMem         := FALSE
 *    bUseDma               := FALSE
 *    bUseShortMode         := FALSE
 *    bUseSharedMemMode     := TRUE
 *
 * Hint: The HIF Universal offers such capabilities.
 *
 * @see eps_hif_drv_if_open         - This function is used to find a matching HIF driver.
 * @see eps_hif_universal_drv_open  - The HIF Universal Driver may be called by eps_hif_drv_if_open.
 *
 * @param [in] pHwInstOut           - pointer to structure containing function pointers and memory pointers.
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_open_hif_driver( EPS_SHM_HW_PTR_TYPE pHwInstOut )
{
	LSA_UINT16 retVal;
	EPS_HIF_DRV_OPENPAR_TYPE InPars;
	EPS_HIF_DRV_CFG_TYPE sHifCfg;

	EPS_ASSERT(g_pEpsVdd != LSA_NULL);

	eps_memset(&InPars, 0, sizeof(InPars));

	InPars.Mode = EPS_HIF_DRV_HIF_TM_SHARED_MEM;
	InPars.if_pars.SendIrqMode = HIF_SEND_IRQ_AFTER_NO_WAIT; // don't care (polling)
	InPars.if_pars.SharedMem.Remote.Base = pHwInstOut->sHif.pBase;
	InPars.if_pars.SharedMem.Remote.Size = pHwInstOut->sHif.uSize/2;
	InPars.if_pars.SharedMem.Local.Base  = pHwInstOut->sHif.pBase + (pHwInstOut->sHif.uSize/2);
	InPars.if_pars.SharedMem.Local.Size  = pHwInstOut->sHif.uSize/2;

	eps_memset(&sHifCfg, 0, sizeof(sHifCfg));
	sHifCfg.bUseSharedMemMode    = LSA_TRUE;
	sHifCfg.bUseRemoteMem        = LSA_FALSE;
	sHifCfg.bUseDma              = LSA_FALSE;
	sHifCfg.bUseIpcInterrupts    = LSA_FALSE;
	sHifCfg.bUsePnCoreInterrupts = LSA_FALSE;

	retVal = eps_hif_drv_if_open ((LSA_UINT16)pHwInstOut->uHdNr, LSA_FALSE /*bUpper*/, &sHifCfg, &InPars, &g_pEpsVdd->pHifDrvHwInstOut);

	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
	//lint --e(818) Pointer parameter 'pHwInstOut' could be declared as pointing to const. Required by API to be not const. 
}

/**
 * Closes the used HIF driver using the EPS HIF Drv IF.
 *
 * @see eps_hif_drv_if_close          - used by this function
 * @see eps_hif_universal_drv_close   - The HIF Universal Driver may be called by eps_hif_drv_if_close.
 *
 * @param [in] pHwInstOut             - pointer to structure containing function pointers and memory pointers.
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_close_hif_driver( EPS_SHM_HW_CONST_PTR_TYPE pHwInstOut )
{
	LSA_UINT16 retVal;

	retVal = eps_hif_drv_if_close((LSA_UINT16)pHwInstOut->uHdNr, LSA_FALSE /*bUpper*/);

	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
}

/**
 * Uninstalls the EPS SHM implementation by setting bOpen to FALSE and bInit to TRUE.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_uninstall( LSA_VOID )
{
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_vdd_uninstall()");

	EPS_ASSERT(g_pEpsVdd != LSA_NULL);
	EPS_ASSERT(g_pEpsVdd->bOpen == LSA_FALSE);
	EPS_ASSERT(g_pEpsVdd->bInit == LSA_TRUE);

	eps_shm_file_undo_init();

	g_pEpsVdd->bInit = LSA_FALSE;

	eps_vdd_undo_init_critical_section();
}

/**
 * Registers the EPS IMCEA Drv into the EPS SHM IF.
 * 
 * The EPS IMCEA Drv is a EPS SHM implementation for the LD Lower IF. 
 * 
 * UseCases: 
 *     EPS Advanced 2T and 3T. 
 *     
 * Restrictions:
 *     Only usable with SIMATIC VMM Hypervisor.
 *     Currently only usable with Adonis OS.
 *  
 * Concept:
 *    In a hypervisor environment, the hypervisor generates a virtual PCI device. This virtual PCI device has a memory space and is 
 *    accessible by both Windows OS as well as the Adonis OS. This implementation seaches for this PCI Devices and uses the memory 
 *    of the virtual PCI device as shared memory to write PNTRC data and HIF data.
 *    Also, this memory is used by HIF to exchange RQBs with the LD Lower IF. 
 * 
 * Memory Mapping:
 *    In order to access the memory of the PCI Device, some memory mapping functions are required. These are implemented by calling EPS_PLF OUT Macros.
 * 
 * PN Dev IF Usage:
 *    In the Shared Memory, information about the available PNBoards are stored into the shared memory (@see eps_imcea_init_shm_header, 
 *    @see eps_pndev_if_get_registered_devices). The LD Lower uses this information to get the available devices.
 * 
 * HIF Integration:
 *    The EPS IMCEA Drv uses the EPS Universal Driver as HIF Driver. This driver works on the SHM provided by EPS IMCEA.
 *    
 * PNTRC Integration:
 *    The Trace data is written into the shared memory provided by the virtual PCI device (PNTRC Lower).
 * 
 * Watchdog:
 *    Currently not implemented.
 *    
 * @see eps_shm_if_register           - this function is used to register the EPS SHM implementation into the EPS SHM IF.
 * @see eps_hif_universal_drv_install - used HIF driver
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_vdd_install (EPS_SHM_INSTALL_ARGS_CONST_PTR_TYPE pInstallArgs)
{
	EPS_SHM_IF_TYPE     sVddDrvIf;
	LSA_BOOL            bIfLoadOk;

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_vdd_install()");

	g_pEpsVdd = &g_EpsVdd;

	eps_memset(g_pEpsVdd, 0, sizeof(*g_pEpsVdd));
	
    bIfLoadOk = eps_vdd_startup();
    EPS_ASSERT(g_pEpsVdd->pDrvHandle != LSA_NULL);
    
    if(!bIfLoadOk)
    {
        #ifdef EPS_VDD_PRINTF_DRIVER_INFO
        printf("\r\n");
        printf("EPS VDD install failed!\r\n");
        printf("    => VDD Dll available? (Windows only)\r\n");
        #endif
    
        return;
    }

	eps_vdd_init_critical_section();
	
	g_pEpsVdd->sInstallArgs            = *pInstallArgs;
	
	g_pEpsVdd->sHw.AppReady            = eps_vdd_app_ready;
	g_pEpsVdd->sHw.AppShutdown         = eps_vdd_app_shutdown;
	g_pEpsVdd->sHw.FatalError          = eps_vdd_sig_fatal;
	g_pEpsVdd->sHw.Close               = eps_vdd_close;
	
#if ( PSI_CFG_USE_PNTRC == 1 )
	g_pEpsVdd->sHw.TraceBufferFull     = eps_vdd_trace_buffer_full;
	g_pEpsVdd->sHw.sPntrcParam         = pInstallArgs->sPntrcParam;
#endif

	g_pEpsVdd->bInit = LSA_TRUE;

	sVddDrvIf.open  = eps_vdd_open;
	sVddDrvIf.close = eps_vdd_close;
	sVddDrvIf.uninstall = eps_vdd_uninstall;
	sVddDrvIf.update_device_list = eps_vdd_update_device_list;

	eps_shm_if_register(&sVddDrvIf);
}

/**
 * Opens the LD lower IF. The PCI Tree is searched for virtual PCI devices.
 * The SHM Header is initialized, then the HIF driver is opened.
 * 
 * @see eps_vdd_open_hif_driver   - used by this function to open the HIF driver.
 * @see eps_vdd_init_shm_header   - initializes the SHM header. After the initialization, the LD upper may write data into the SHM.
 * @see eps_shm_if_open           - calls this function to open the LD Lower IF.
 * 
 * @param [in] hSys               - sys handle
 * @param [in] pOption            - EPS SHM decriptor (hd_nr: 0 = LD, 1...4 = HD, Debug options). 1...4 NOT SUPPORTED!!! This is a LD Lower Driver.
 * @param [out] ppHwInstOut       - pointer to structure containing function pointers and memory pointers.
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_vdd_open (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppHwInstOut)
{
	LSA_UINT16 retVal;
	uVDD_DESC               *pDrvHandle;
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eps_vdd_open()");

	EPS_ASSERT(g_pEpsVdd != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	LSA_UNUSED_ARG(hSys);
	
    // *****************************************************************************
    // new connect using VDD
    uVDD_GET_DEVICE_INFO_IN     uGetDeviceInfoIn;
    uVDD_OPEN_DEVICE_IN         uOpenDeviceIn;
    eVDD_RESULT                 drvResult;
    _TCHAR                      sError[VDD_SIZE_STRING_BUF] = { 0 };
    UINT32                      lCtrDevice = 0;
    uVDD_GET_DEVICE_INFO_OUT    uArrayDeviceInfo[1];
    uVDD_OPEN_DEVICE_OUT        uOpenDeviceOut;

    eps_memset(&uOpenDeviceIn, 0, sizeof(uVDD_OPEN_DEVICE_IN));
    pDrvHandle = g_pEpsVdd->pDrvHandle;
    // get DeviceInfo of all devices
    drvResult = (   *g_pEpsVdd->pDrvHandle->uIf.pFnGetDeviceInfo)(  pDrvHandle,
                                                                    sizeof(uVDD_GET_DEVICE_INFO_IN),
                                                                    sizeof(uVDD_GET_DEVICE_INFO_OUT),
                                                                    1,
                                                                    _countof(sError),
                                                                    &uGetDeviceInfoIn,
                                                                    uArrayDeviceInfo,
                                                                    &lCtrDevice,
                                                                    sError);
    EPS_ASSERT(drvResult == eVDD_OK);

    uOpenDeviceIn.uCbfIsr.pPtr          = LSA_NULL;

    uOpenDeviceIn.uCbfEvent.pPtr        = eps_vdd_event_cbf;
    uOpenDeviceIn.uCbfParUser.pPtr      = (void *)&uOpenDeviceOut;

    uOpenDeviceIn.uThreadDll.lPrio      = VDD_THREAD_PRIO_HIGH;
    uOpenDeviceIn.uThreadDll.lStackSize = 0;        /* Use Default 8kB according to documentation */
    uOpenDeviceIn.uThreadSys.lPrio      = VDD_THREAD_PRIO_HIGH;
    uOpenDeviceIn.uThreadSys.lStackSize = 0;        /* Use Default 8kB according to documentation */
    uOpenDeviceIn.uThreadSys.lTimeCycle_10msec = 0; /* Use Default 100ms according to documentation */

    eps_memcpy(uOpenDeviceIn.sPciLoc, uArrayDeviceInfo[0].sPciLoc, VDD_SIZE_STRING_BUF_SHORT);

    drvResult = g_pEpsVdd->pDrvHandle->uIf.pFnOpenDevice(pDrvHandle, sizeof(uOpenDeviceIn), sizeof(uOpenDeviceOut), &uOpenDeviceIn, &uOpenDeviceOut);

    if (drvResult != eVDD_OK)
    {
        LSA_CHAR sErrConv[VDD_SIZE_STRING_BUF] = { 0 };
        eps_convert_vdd_err_string(uOpenDeviceOut.sError, sErrConv);
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "eps_vdd_open() %s", sErrConv);
    }
    // *****************************************************************************

	eps_vdd_enter();

	if(drvResult == eVDD_OK)
	{
		g_pEpsVdd->sHw.pBase = (UINT8*)uOpenDeviceOut.uHwRes.as.uVirtPCIDev.uSharedMem.uBase.pPtr;
		g_pEpsVdd->sHw.uSize = uOpenDeviceOut.uHwRes.as.uVirtPCIDev.uSharedMem.lSize;

		g_pEpsVdd->pShmHeader = (EPS_VDD_SHM_HEADER_PTR_TYPE)EPS_CAST_TO_VOID_PTR(uOpenDeviceOut.uHwRes.as.uVirtPCIDev.uSharedMem.uBase.pPtr);
		eps_memset(g_pEpsVdd->pShmHeader, 0, sizeof(*g_pEpsVdd->pShmHeader));

    	retVal = eps_vdd_init_shm_header(uOpenDeviceOut.uHwRes.as.uVirtPCIDev.uSharedMem.lSize);
  	}
	else
	{
	    retVal = EPS_SHM_RET_ERR;
	}

	if(retVal == EPS_SHM_RET_OK)
	{
        LSA_UINT32 tmpOffset; // Note: g_pEpsVdd is volatile. Surpress lint error 'variable 'g_pEpsVdd' depends on order of evaluation' by using tmpOffset here
        g_pEpsVdd->sHw.bDebug          = LSA_FALSE;
        tmpOffset = g_pEpsVdd->pShmHeader->uHifOffset;
        g_pEpsVdd->sHw.sHif.pBase      = g_pEpsVdd->sHw.pBase + tmpOffset /* plus g_pEpsVdd->pShmHeader->uHifOffset */;
        g_pEpsVdd->sHw.sHif.uOffset    = g_pEpsVdd->pShmHeader->uHifOffset;
        g_pEpsVdd->sHw.sHif.uSize      = g_pEpsVdd->pShmHeader->uHifSize;
        tmpOffset = g_pEpsVdd->pShmHeader->uFatalOffset;
        g_pEpsVdd->sHw.sFatal.pBase    = g_pEpsVdd->sHw.pBase + tmpOffset /* plus g_pEpsVdd->pShmHeader->uFatalOffset*/;
        g_pEpsVdd->sHw.sFatal.uOffset  = g_pEpsVdd->pShmHeader->uFatalOffset;
        g_pEpsVdd->sHw.sFatal.uSize    = g_pEpsVdd->pShmHeader->uFatalSize;
        tmpOffset = g_pEpsVdd->pShmHeader->uTrace1Offset;
        g_pEpsVdd->sHw.sPntrc1.pBase   = g_pEpsVdd->sHw.pBase + tmpOffset /* plus g_pEpsVdd->pShmHeader->uTrace1Offset*/;
        g_pEpsVdd->sHw.sPntrc1.uOffset = g_pEpsVdd->pShmHeader->uTrace1Offset;
        g_pEpsVdd->sHw.sPntrc1.uSize   = g_pEpsVdd->pShmHeader->uTrace1Size;
        tmpOffset = g_pEpsVdd->pShmHeader->uTrace2Offset;
        g_pEpsVdd->sHw.sPntrc2.pBase   = g_pEpsVdd->sHw.pBase + tmpOffset /* plus g_pEpsVdd->pShmHeader->uTrace2Offset*/;
        g_pEpsVdd->sHw.sPntrc2.uOffset = g_pEpsVdd->pShmHeader->uTrace2Offset;
        g_pEpsVdd->sHw.sPntrc2.uSize   = g_pEpsVdd->pShmHeader->uTrace2Size;
        
		if(pOption != LSA_NULL)
		{
			g_pEpsVdd->sHw.uHdNr = pOption->uHdNr;
      
			if(pOption->bEnableWatchdog)
			{
			  g_pEpsVdd->pShmHeader->Watchdog.bWdEnabled = LSA_TRUE;
			}
		}

		g_pEpsVdd->bOpen = LSA_TRUE;

		eps_vdd_open_hif_driver(&g_pEpsVdd->sHw);

		*ppHwInstOut = &g_pEpsVdd->sHw;
	}

	eps_vdd_exit();

	return retVal;
	//lint --e(818) Pointer parameter 'pOption' could be declared as pointing to const. Required by API to be not const. 
}


/**
 * Closes the EPS SHM implementation
 * 
 * @see eps_shm_if_close            - calls this function
 * @see eps_vdd_close_hif_driver    - used by this function to close the HIF driver instance
 * @see eps_hif_universal_drv_close - may be used by this function
 * 
 * @param [in] pHwInstIn            - pointer to structure containing function pointers and memory pointers.
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_vdd_close (EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn)
{   
	EPS_ASSERT(g_pEpsVdd != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eps_vdd_close()");

	eps_vdd_enter();

	EPS_ASSERT(g_pEpsVdd->bOpen == LSA_TRUE);

	eps_vdd_close_hif_driver(pHwInstIn);

	eps_vdd_undo_init_wd_service();
	eps_vdd_undo_init_check_remote_state_service();

	g_pEpsVdd->pShmHeader->uSyncSelf = EPS_VDD_SELF_STATE_NINIT;
	g_pEpsVdd->bOpen                 = LSA_FALSE;

    // *****************************************************************************
    // Shutdown the VDD

    if (g_pEpsVdd->pDrvHandle != 0)
    {
        eVDD_RESULT eResult;
        VDD_CHAR    drvError[VDD_SIZE_STRING_BUF];

        eps_memset(drvError, 0, sizeof(drvError));

        // ShutDownDriver (disconnect driver connection)
        // Note: DLL cleanup (unload, cleanup Fct list) is done by the driver service
        //       Tracing not done, possible already disabled
        eResult = fnVDD_ShutDownDriver( g_pEpsVdd->pDrvHandle,
                                        VDD_SIZE_STRING_BUF,
                                        drvError);

        if (eResult != eVDD_OK)
        {
            LSA_CHAR sErrConv[VDD_SIZE_STRING_BUF] = { 0 };
            eps_convert_vdd_err_string(drvError, sErrConv);
            EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS VDD %s", sErrConv);
        }

        EPS_ASSERT(eResult == eVDD_OK);
    }
    // *****************************************************************************

    g_pEpsVdd->pDrvHandle = 0;

	#ifdef EPS_VDD_PRINTF
  	printf("\nEPS VDD Shutdown");
	#endif

	eps_vdd_exit();

	return EPS_SHM_RET_OK;
}

#if ( PSI_CFG_USE_PNTRC == 1 )
/**
 * The SHM API requires the implementation of this function.
 * 
 * Since this is an EPS Advanced implementation, the tracebuffer must never be emptied, since the Advanced FW is never the topmost.
 * 
 * @see PNTRC_BUFFER_FULL          - calls tis function
 * @param [in] hTrace              - Trace handle ID from PNTRC
 * @param [in] uBufferId           - Buffer ID from PNTRC
 * @param [in] pBuffer             - Buffer to be emptied (SRC)
 * @param [in] uLength             - Length of the Buffer to be emptied (SRC)
 * @return LSA_VOID
 */
static LSA_VOID eps_vdd_trace_buffer_full(LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength)
{
    LSA_UNUSED_ARG(hTrace);
    LSA_UNUSED_ARG(uBufferId);
    LSA_UNUSED_ARG(pBuffer);
    LSA_UNUSED_ARG(uLength);

    EPS_FATAL("Tracebuffer full! (Should never happen, because we are not TopMost)");
}
#endif

/**
 * Starts the synchronization with the LD Upper IF by doing a handshake algorithm over the shared memory.
 * 
 * Extracts info from the SHM 
 *    - Debug Mode
 * 
 * @see eps_startup_done - calls this function
 * 
 * @param [in] pHwInstIn            - pointer to structure containing function pointers and memory pointers.
 * @param [in] pHifHandle           - handle for HIF
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_vdd_app_ready (EPS_SHM_HW_PTR_TYPE pHwInstIn, LSA_VOID* pHifHandle)
{
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eps_vdd_app_ready()");

	/* Init ISR polling specific structure */

	EPS_ASSERT(g_pEpsVdd != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	LSA_UNUSED_ARG(pHifHandle);

	eps_vdd_enter();

	EPS_ASSERT(g_pEpsVdd->bOpen == LSA_TRUE);

	g_pEpsVdd->pShmHeader->uSyncSelf = EPS_VDD_SELF_STATE_READY;

	#ifdef EPS_VDD_PRINTF
	printf("\r\nEPS VDD Shared Mem - Wait for Upper Client\r\n");
	#endif

	while (g_pEpsVdd->pShmHeader->uSyncRemote != EPS_VDD_REMO_STATE_READY)
	{
		eps_tasks_sleep(500);
	}

	if(g_pEpsVdd->pShmHeader->Watchdog.bWdEnabled == LSA_TRUE)
    {
        eps_vdd_init_wd_service();
    }

	if(g_pEpsVdd->pShmHeader->bDebugOn == 1)
	{
		pHwInstIn->bDebug = LSA_TRUE;
	}
	else
	{
		pHwInstIn->bDebug = LSA_FALSE;
    
		eps_vdd_init_check_remote_state_service();
	}

	eps_vdd_exit();

	return EPS_SHM_RET_OK;
}

/**
 * The SHM API requires the implementation of this function.
 * 
 * Currently, there is no functionality.
 * 
 * @see eps_undo_init     - calls this function
 * 
 * @param [in] pHwInstIn  - pointer to structure containing function pointers and memory pointers.
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_vdd_app_shutdown( EPS_SHM_HW_PTR_TYPE pHwInstIn )
{	// no functionality at the moment (4/2013)
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eps_vdd_app_shutdown()");

	EPS_ASSERT(g_pEpsVdd != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	return EPS_SHM_RET_OK;
	//lint --e(818) Pointer parameter 'pHwInstIn' could be declared as pointing to const. Required to be not const by API.
}

/**
 * 
 * FATAL Handling implementation of the EPS VDD driver.
 * This function will set SHM values to FATAL so that the LD upper recognizes the fatal.
 * 
 * Some error info is printed on the COM-Port. This may take some time. 
 * 
 * @see eps_fatal_with_reason   - calls this function to signal a FATAL caused by PSI or EPS.
 * 
 * @param [in] pHwInstIn        - pointer to structure containing function pointers and memory pointers.
 * @param [in] sFatal           - structure with FATAL info
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_vdd_sig_fatal( EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal )
{
    EPS_ASSERT(g_pEpsVdd != LSA_NULL);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    g_pEpsVdd->bFatal = LSA_TRUE;
    g_pEpsVdd->bOpen  = LSA_FALSE;

    #ifdef EPS_VDD_PRINTF
    /*
     * Blocking function calls, like printf or sleep are not permitted while holding a spinlock!
     * Check before printing error information
     */
    if (g_pEpsData->bBlockingPermitted == LSA_TRUE)
    {
        printf("\r\n\r\n");

        switch (sFatal.exit_code_org)
        {
            case EPS_EXIT_CODE_LSA_FATAL: //caller = PSI_FATAL_ERROR()
            {
                printf("!!!EPS LSA FATAL!!! => Comp(%s) CompId(%d) ModuleId(%d) Line(%d) ErrCode([0]=%d / 0x%lx,[1]= %d / 0x%lx,[2]= %d / 0x%lx,[3]= %d / 0x%lx)", sFatal.pComp, (int)sFatal.pLsaFatal->lsa_component_id, (int)sFatal.pLsaFatal->module_id, (int)sFatal.pLsaFatal->line, (int)sFatal.pLsaFatal->error_code[0], sFatal.pLsaFatal->error_code[0], (int)sFatal.pLsaFatal->error_code[1], sFatal.pLsaFatal->error_code[1], (int)sFatal.pLsaFatal->error_code[2], sFatal.pLsaFatal->error_code[2], (int)sFatal.pLsaFatal->error_code[3], sFatal.pLsaFatal->error_code[3]);
                break;
            }
            default: //all other exit_codes/callers
            {
                printf("!!!EPS FATAL!!! => File(%s) Function(%s) Line(%lu) Msg(%s)", sFatal.pFile, sFatal.pFunc, sFatal.uLine, sFatal.pMsg);
            } 
        }

        printf("\r\n\r\n");
        eps_tasks_sleep(20); //spend some time to print out
    }
    #else
    LSA_UNUSED_ARG(sFatal);
    #endif

    g_pEpsVdd->pShmHeader->bFatal    = 1;
    g_pEpsVdd->pShmHeader->uSyncSelf = EPS_VDD_SELF_STATE_FATAL;

    return EPS_SHM_RET_OK;
}


/**
 * check_remote_state_service, will be called periodically to check the remote state
 * 
 * @param [in] timer_id - TimerId
 * @param [in] user_id  - UserId
 * @return LSA_VOID
 */
static LSA_VOID * eps_vdd_check_remote_state_service( LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id )
{
    LSA_UNUSED_ARG(timer_id);
    
    LSA_UNUSED_ARG(user_id);

    EPS_ASSERT(g_pEpsVdd != LSA_NULL);
    
    eps_vdd_enter();
    
    // check if reboot already triggered
    if(!eps_advanced_reboot_is_active())
    {    
        switch (g_pEpsVdd->pShmHeader->uSyncRemote)
        {
            case EPS_VDD_REMO_STATE_NINIT:
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-CheckRemoteState (lower): eps_vdd_check_remote_state_service() - Not initialized");
                
                break;
            }
            case EPS_VDD_REMO_STATE_READY:
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-CheckRemoteState (lower): eps_vdd_check_remote_state_service() - Ready");
                
                break;
            }
            case EPS_VDD_REMO_STATE_CLOSED:
            {
                // we read the closed flag, reset it
                g_pEpsVdd->pShmHeader->uSyncRemote = EPS_VDD_REMO_STATE_NINIT;
                
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-CheckRemoteState (lower): eps_vdd_check_remote_state_service() - Closed");
    
                // Upper Instance is closed, we are going to reboot the EPS Advanced FW      
                // NOTE:    we can not call the reboot directly from here
                //          we have to start a new thread which triggers the reboot
                //          because a direct call from here will cause a thread stack overflow of the TIMER_GROUP0 thread            
                    
                eps_advanced_init_reboot();   
             
                break;
            }
            default:
            {
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"EPS VDD-CheckRemoteState (lower): eps_vdd_check_remote_state_service() - Unknown State: 0x%08x", g_pEpsVdd->pShmHeader->uSyncRemote);
                EPS_FATAL("eps_vdd_check_remote_state_service() - Unknown State");
    
                break;
            }
        }
    }
    
    eps_vdd_exit();

    return NULL;
}

/**
 * WatchdogService, will be called periodically to set the Alive-Flag
 * 
 * @param [in] timer_id - TimerId
 * @param [in] user_id  - UserId
 * @return LSA_VOID
 */
static LSA_VOID * eps_vdd_wd_service( LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id )
{
    LSA_UNUSED_ARG(timer_id);
    
    LSA_UNUSED_ARG(user_id);

    EPS_ASSERT(g_pEpsVdd != LSA_NULL);
    
    g_pEpsVdd->pShmHeader->Watchdog.bAlive = LSA_TRUE;
    
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-Watchdog (lower): eps_vdd_wd_service() - Set Alive-Flag done");

    return NULL;
}

/**
 * startup VDD (open DLL)
 * 
 * @param LSA_VOID
 * @return  ePNDEV_OK
 */
static LSA_BOOL eps_vdd_startup( LSA_VOID )
{
    VDD_CHAR                drvError[VDD_SIZE_STRING_BUF];
    eVDD_RESULT             eResult  = eVDD_FATAL;
    LSA_BOOL                bSuccess = LSA_FALSE;
    EPS_APP_DRIVER_PATH_ARR uPathArr;
    LSA_UINT32              lLoopCount;   

    eps_memset(&uPathArr, 0, sizeof(uPathArr));
    
    EPS_APP_GET_DRIVER_PATH(&uPathArr, EPS_APP_DRIVER_VDD);

    // Try to startup PnDevDrv DLL from the given paths
    // note: Function pointers and version check is internal done by with driver startup service
    for (lLoopCount = 0; lLoopCount < EPS_APP_DRIVER_PATH_MAX_ARR_SIZE; lLoopCount++)
    {       
        VDD_CHAR drvPath[EPS_APP_PATH_MAX_LENGTH];

        eps_convert_vdd_path(uPathArr.Path[lLoopCount].sPath, drvPath);

        eResult = fnVDD_StartUpDriver(  drvPath,
                                        VDD_SIZE_STRING_BUF,
                                        &g_pEpsVdd->pDrvHandle,
                                        drvError);

        if (eResult == eVDD_OK)
            // Startup successfull
        {
            break;
        }
    }

    if (eResult == eVDD_OK)
    {
        bSuccess = LSA_TRUE;
    }
    else
    {
        LSA_CHAR sErrConv[VDD_SIZE_STRING_BUF] = {0};
        eps_convert_vdd_err_string(drvError, sErrConv);
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS VDD %s", sErrConv);

        // For Adonis no fatal is called
        #if (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_WINDOWS_X86)
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_DRIVER_NOT_FOUND, EPS_EXIT_CODE_DRIVER_NOT_FOUND, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_vdd_startup() - Couldn't load VDD-DLL");
        #endif
    }

    return bSuccess;
}
