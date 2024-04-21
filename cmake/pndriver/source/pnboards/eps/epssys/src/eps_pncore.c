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
/*  F i l e               &F: eps_pncore.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PnCore Shared Mem Interface Adaption                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20030
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Defines -------------------------------------------------------------------------------------- */

/* - Includes ------------------------------------------------------------------------------------- */

#include <psi_inc.h>

#include <eps_sys.h>                /* Types / Prototypes / Fucns        */
#include <eps_locks.h>              /* EPS Locks                         */
#include <eps_mem.h>                /* EPS Local Memory                  */
#include <eps_trc.h>                /* Tracing                           */
#include <eps_shm_if.h>             /* Shared Mem Interface              */
#include <eps_rtos.h>               /* OS interface                      */
#include <eps_pncore.h>             /* Own Include                       */
#include <eps_timer.h>              /* Timer System Interface            */
#include <eps_pndrvif.h>            /* Driver Interface for HIF Isr      */
#include <eps_trc.h>                /* Tracing                           */
#include <eps_hif_drv_if.h>         /* HIF-Driver Interface              */
#include <eps_shm_if_config.h>
#include <eps_ipc_drv_if.h>
#include <eps_plf.h>
#include <eps_shm_map.h>
#include <eps_shm_file.h>

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#define EPS_CFG_USE_PNCORE

#ifdef EPS_CFG_USE_PNCORE

#include <PnCore_Inc.h>             /* PnCore Interface */

#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
#include <sys/eventfd.h>    //kernel-user events
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#endif

/* - Typedefs ------------------------------------------------------------------------------------- */

typedef struct eps_pncore_store_tag
{
    LSA_BOOL bInit;
    LSA_BOOL bOpen;
    LSA_BOOL bFatal;
    LSA_BOOL bWdEnabled;
    LSA_UINT16 hEnterExit;
    EPS_SHM_HW_TYPE sHw;
    EPS_SHM_INSTALL_ARGS_TYPE   sInstallArgs;
    uPNCORE_STARTUP_PAR_DRIVER  StartupParDriverOut;
    uPNCORE_STARTUP_PAR_FW      StartupParFwIn;
    LSA_TIMER_ID_TYPE           hWdTimer;
    EPS_HIF_DRV_HW_PTR_TYPE     pHifDrvHwInstOut;
} EPS_PNCORE_STORE_TYPE, *EPS_PNCORE_STORE_PTR_TYPE;


/* - Defines from Linker Files -------------------------------------------------------------------- */
extern LSA_UINT32 __PNCORE_SHM_START__;
extern LSA_UINT32 __HOST_MEM0_START__;


/* - Prototypes ----------------------------------------------------------------------------------- */

#if ( PSI_CFG_USE_PNTRC == 1 )
static LSA_VOID    eps_pncore_trace_buffer_full        (LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength);
#endif

static LSA_VOID   *eps_pncore_wd_service               (LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id);
static LSA_UINT16  eps_pncore_app_ready                (EPS_SHM_HW_PTR_TYPE pHwInstIn, LSA_VOID_PTR_TYPE pHifHandle);
static LSA_UINT16  eps_pncore_app_shutdown             (EPS_SHM_HW_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pncore_sig_fatal                (EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal);
static LSA_UINT16  eps_pncore_open                     (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppHwInstOut);
static LSA_UINT16  eps_pncore_close                    (EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn);

/* - Global Data ---------------------------------------------------------------------------------- */

EPS_PNCORE_STORE_TYPE g_EpsPnCoreDrv;
EPS_PNCORE_STORE_PTR_TYPE g_pEpsPnCoreDrv;

/* - Source --------------------------------------------------------------------------------------- */
/**
 * undo init critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_undo_init_critical_section( LSA_VOID )
{
    eps_free_critical_section(g_pEpsPnCoreDrv->hEnterExit);
}

/**
 * init critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_init_critical_section( LSA_VOID )
{
    eps_alloc_critical_section(&g_pEpsPnCoreDrv->hEnterExit, LSA_FALSE);
}

/**
 * enter critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_enter( LSA_VOID )
{
    eps_enter_critical_section(g_pEpsPnCoreDrv->hEnterExit);
}

/**
 * exit criticial section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_exit( LSA_VOID )
{
    eps_exit_critical_section(g_pEpsPnCoreDrv->hEnterExit);
}

/**
 * undo init WatchdogService
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_undo_init_wd_service( LSA_VOID )
{
    LSA_UINT16 ret_val;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncore_undo_init_wd_service()");

    if(g_pEpsPnCoreDrv->bWdEnabled == LSA_TRUE)
    {
        ret_val = eps_timer_stop(g_pEpsPnCoreDrv->hWdTimer);
        EPS_ASSERT(ret_val == LSA_RET_OK);
        ret_val = eps_timer_free(g_pEpsPnCoreDrv->hWdTimer);
        EPS_ASSERT(ret_val == LSA_RET_OK);

        g_pEpsPnCoreDrv->bWdEnabled = LSA_FALSE;
    }
}

/**
 * init WatchdogService
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_init_wd_service( LSA_VOID )
{
    LSA_USER_ID_TYPE eps_sys_user_id;
    LSA_UINT16 		 ret_val;
    LSA_UINT16 		 uCycle      = 1;
    LSA_UINT16 		 uRes        = 100; /* LSA_TIME_BASE_100MS */
    LSA_UINT16 		 uIntervalMs = uCycle * uRes;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncore_init_wd_service()");

    g_pEpsPnCoreDrv->bWdEnabled = LSA_TRUE;

    ret_val = eps_timer_allocate_tgroup0(&g_pEpsPnCoreDrv->hWdTimer, LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_100MS, (EPS_TIMER_CBF)eps_pncore_wd_service);
    EPS_ASSERT(ret_val == LSA_RET_OK);

    eps_sys_user_id.uvar16 = uIntervalMs;

    ret_val = eps_timer_start(g_pEpsPnCoreDrv->hWdTimer, eps_sys_user_id, 1);
    EPS_ASSERT(ret_val == LSA_RET_OK);
}

/**
 * Opens a HIF driver for the EPS PNCore driver
 *
 * The HIF capabilities are transfered by the ExchangeStartupPar mechanism of the PNDevDriver.
 *
 * - set HifConfigurationParameter in SharedHostMemoryConfiguration-Strukture
 * - set Parameter for Hif-MemoryPools and IpcInterrupts
 *
 * Hint: Possible EPS HIF drivers are:
 *    eps_hif_pncorestd_drv - HIF PNCore Driver
 *    eps_hif_universal_drv - HIF Universal Driver
 *    TODO: eps_hif_siedmadrv ????
 *
 * @see eps_hif_drv_if_open         - This function is used to find a matching HIF driver.
 * @see eps_hif_pncorestd_drv_open  - The HIF PNCore Driver may be called by eps_hif_drv_if_open.
 * @see eps_hif_universal_drv_open  - The HIF Universal Driver may be called by eps_hif_drv_if_open.
 *
 * @see eps_ipc_tx_alloc            - The EPS IPC may be used to send interrupts
 *
 * @param [in] pHwInstOut           - pointer to structure containing function pointers and memory pointers. Also contains SharedHostMemoryConfiguration
 * @return LSA_VOID
 */
static LSA_VOID eps_pncore_open_hif_driver( EPS_SHM_HW_PTR_TYPE pHwInstOut )
{
    LSA_UINT16 retVal;
    EPS_HIF_DRV_OPENPAR_TYPE InPars;
    EPS_HIF_DRV_CFG_TYPE sHifCfg;
    EPS_SHM_UPPER_CONFIG_PTR_TYPE pCfg;
    #if (EPS_PLF == EPS_PLF_SOC_MIPS || EPS_PLF == EPS_PLF_LINUX_SOC1)
    LSA_BOOL bMapResult;
    #endif
	#if ((EPS_PLF == EPS_PLF_SOC_MIPS || EPS_PLF == EPS_PLF_LINUX_SOC1) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9) || (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_ARM_CORTEX_A15))
    LSA_UINT32 LocalAddress;
    LSA_UINT8* pMapAddress;
    #endif

    // get ShareMemConfiguration
    pCfg = (EPS_SHM_UPPER_CONFIG_PTR_TYPE)g_pEpsPnCoreDrv->StartupParDriverOut.lArrayUserData;

    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);

    eps_memset(&InPars, 0, sizeof(InPars));
    eps_memset(&sHifCfg, 0, sizeof(sHifCfg));

    /* Fill in standard values for Legacy support */
    InPars.Mode = EPS_HIF_DRV_HIF_TM_SHARED_MEM;
    InPars.if_pars.SendIrqMode = HIF_SEND_IRQ_AFTER_NO_WAIT; // delivering interrupt faster than data is not as important as pncore performance -> waiting before sending irq back would lower performance on boards
    InPars.if_pars.SharedMem.Remote.Base = pHwInstOut->sHif.pBase;
    InPars.if_pars.SharedMem.Remote.Size = pHwInstOut->sHif.uSize/2;
    InPars.if_pars.SharedMem.Local.Base  = pHwInstOut->sHif.pBase + (pHwInstOut->sHif.uSize/2);
    InPars.if_pars.SharedMem.Local.Size  = pHwInstOut->sHif.uSize/2;

    if( (pCfg->bValid) && (pCfg->HIF.bValid) )
    {
        EPS_ASSERT(pCfg->uCfgLength == sizeof(EPS_SHM_UPPER_CONFIG_TYPE));

        /* Copy HIF settings */
        eps_memcpy(&sHifCfg, &pCfg->HIF.HifCfg, sizeof(EPS_HIF_DRV_CFG_TYPE));

        if(sHifCfg.bUseRemoteMem)
        {
            /* Remote Mem */
#if (EPS_PLF == EPS_PLF_SOC_MIPS)
            LocalAddress = (LSA_UINT32)EPS_PLF_PCI_TRANSLATE_PCI_TO_LOCAL_ADDR((LSA_UINT8*)g_pEpsPnCoreDrv->StartupParDriverOut.uSharedHostSdram.lPhyAdr);
            bMapResult = EPS_PLF_MAP_MEMORY(&pMapAddress, LocalAddress, pCfg->HIF.HostRam.uSize);
            EPS_ASSERT(bMapResult == LSA_TRUE);
#elif (EPS_PLF == EPS_PLF_LINUX_SOC1)
            LocalAddress = (g_pEpsPnCoreDrv->StartupParDriverOut.uSharedHostSdram.lPhyAdr + 0xC0000000); // No pci memory translation on linux soc1, pci master window begins at 0xC0000000
            bMapResult = EPS_PLF_MAP_MEMORY(&pMapAddress, LocalAddress, pCfg->HIF.HostRam.uSize);
            EPS_ASSERT(bMapResult == LSA_TRUE);
#elif (EPS_PLF == EPS_PLF_ERTEC400_ARM9)
            LocalAddress = (g_pEpsPnCoreDrv->StartupParDriverOut.uSharedHostSdram.lPhyAdr + 0xC0000000); // There is no memory translation available on ertec400, pci master window begins at 0xC0000000
            pMapAddress  = (LSA_UINT8*)LocalAddress;
#elif (EPS_PLF == EPS_PLF_PNIP_A53)
            LocalAddress = (LSA_UINT32)__HOST_MEM0_START__; // use pci master window 0
            pMapAddress  = (LSA_UINT8*)LocalAddress;
#elif (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
            LocalAddress = (LSA_UINT32)__HOST_MEM0_START__; // use outbound region 0
            pMapAddress  = (LSA_UINT8*)LocalAddress;
#endif
#if ((EPS_PLF == EPS_PLF_SOC_MIPS || EPS_PLF == EPS_PLF_LINUX_SOC1) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9) || (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_ARM_CORTEX_A15))
            InPars.if_pars.SharedMem.Remote.Base = pMapAddress;

            if(sHifCfg.bUseDma)
            {
                InPars.drv_type_specific.pars.siegdma.bUseDma = LSA_TRUE;
                InPars.drv_type_specific.pars.siegdma.RemoteShmPhysBaseAhb = LocalAddress; /* Physical Address is local AHB Address! */
            }

            InPars.if_pars.SharedMem.Remote.Size = pCfg->HIF.HostRam.uSize;
            InPars.if_pars.SharedMem.Local.Base  = pHwInstOut->sHif.pBase;
            InPars.if_pars.SharedMem.Local.Size  = pHwInstOut->sHif.uSize;
#else
			EPS_FATAL("Shared memory is only supported for SOC, ERTEC400, HERA and TI yet"); /* not supported now */
#endif
        }

        if(sHifCfg.bUseIpcInterrupts)
        {
            // Open ipc interface
            eps_ipc_drv_if_open(pHwInstOut->uHdNr, sHifCfg.uType);
            
            InPars.drv_type_specific.pars.ipc.bUsehIpcTx = LSA_TRUE;
			retVal = eps_ipc_drv_if_tx_alloc(&InPars.drv_type_specific.pars.ipc.hIpcTx, pCfg->HIF.IPC.uPhysAddress, pCfg->HIF.IPC.uMsgValue, pHwInstOut->uHdNr);
            EPS_ASSERT(retVal == EPS_IPC_RET_OK);
        }
    }
    else
    {
        /* Legacy => Shared Mem, No Remote Mem, PnCore Interrupts */

        sHifCfg.bUsePnCoreInterrupts = LSA_FALSE;
        sHifCfg.bUseSharedMemMode    = LSA_TRUE;
    }

    retVal = eps_hif_drv_if_open(pHwInstOut->uHdNr, LSA_FALSE /*bUpper*/, &sHifCfg, &InPars, &g_pEpsPnCoreDrv->pHifDrvHwInstOut);

    EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
}

/**
 * Closes the used HIF driver using the EPS HIF Drv IF.
 *
 * @see eps_hif_drv_if_close          - used by this function
 * @see eps_hif_universal_drv_close   - The HIF Universal Driver may be called by eps_hif_drv_if_close.
 *
 * @param [in] pHwInstOut             - pointer to structure containing function pointers and memory pointers and SharedHostMemoryConfiguration
 * @return LSA_VOID
 */
static LSA_VOID eps_pncore_close_hif_driver( EPS_SHM_HW_CONST_PTR_TYPE pHwInstOut )
{
    LSA_UINT16 retVal;

    retVal = eps_hif_drv_if_close(pHwInstOut->uHdNr, LSA_FALSE /*bUpper*/);

    EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
}

/**
 * uninstall DriverInterface for pnCore
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pncore_uninstall( LSA_VOID )
{
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncore_uninstall()");

    EPS_ASSERT(g_pEpsPnCoreDrv->bInit == LSA_TRUE);
    EPS_ASSERT(g_pEpsPnCoreDrv->bOpen == LSA_FALSE);

    g_pEpsPnCoreDrv->bInit = LSA_FALSE;

    eps_pncore_undo_init_critical_section();
}

/**
 * update device list
 *
 * @param LSA_VOID
 * @return
 */
LSA_UINT16 eps_pncore_update_device_list( LSA_VOID )
{
    //Do nothing
    return EPS_SHM_RET_OK;
}

/**
 * Registers the EPS PNCore Drv into the EPS SHM IF.
 *
 * The EPS PNCore Drv is a EPS SHM implementation for the LD Lower IF as well as HD Lower IF.
 *
 * UseCases:
 *     EPS Basic Firmware with LD + HD (LD Lower IF)
 *     EPS Basic Firmware with HD (HD Lower IF)
 *
 * Restrictions:
 *     Only for Basic Firmware that runs on a PNBoard with the Adonis OS.
 *
 * Concept:
 *    The PNDevDriver downloads the Basic Firmware to the PNBoard. The PNDevDriver also provides the PNCore API that is used
 *    in this driver. Also, StartupParameter are passed to the Basic FW. These Parameters are used to setup HIF.
 *
 *    Each PNBoard has a local RAM that is used as shared memory. The host has access to this shared memory via PCI by calling
 *    PNDevDriver API functions. This local RAM is write PNTRC data and HIF data. Also, this memory is used by HIF to exchange
 *    RQBs with the LD/HD Lower IF.
 *
 * HIF Integration:
 *    The EPS NoShmDrv Drv uses the EPS Short Driver as HIF Driver. This driver will set HIF to HIF_TM_SHORT.
 *
 * PNTRC Integration:
 *    The trace is written into the local memory.
 *
 * Watchdog:
 *    The PNCore API has a SignOfLife concept that is used here. If a fatal occurs in the Basic Firmware, the SignOfLife is no longer
 *    set. The Upper IF will recognize this and extract FATAL error information stored in the shared memory.
 *
 * @see eps_shm_if_register           - this function is used to register the EPS SHM implementation into the EPS SHM IF.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pncoredrv_install(EPS_SHM_INSTALL_ARGS_PTR_TYPE pInstallArgs)
{
    EPS_SHM_IF_TYPE sPnCoreDrvIf;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncoredrv_install()");

    g_pEpsPnCoreDrv = &g_EpsPnCoreDrv;

    eps_memset(g_pEpsPnCoreDrv, 0, sizeof(*g_pEpsPnCoreDrv));

    eps_pncore_init_critical_section();

    g_pEpsPnCoreDrv->sInstallArgs            = *pInstallArgs;

    g_pEpsPnCoreDrv->sHw.AppReady            = eps_pncore_app_ready;
    g_pEpsPnCoreDrv->sHw.AppShutdown         = eps_pncore_app_shutdown;
    g_pEpsPnCoreDrv->sHw.FatalError          = eps_pncore_sig_fatal;
    g_pEpsPnCoreDrv->sHw.Close               = eps_pncore_close;

#if ( PSI_CFG_USE_PNTRC == 1 )
    g_pEpsPnCoreDrv->sHw.TraceBufferFull     = eps_pncore_trace_buffer_full;
    g_pEpsPnCoreDrv->sHw.sPntrcParam         = pInstallArgs->sPntrcParam;
#endif

    g_pEpsPnCoreDrv->bInit = LSA_TRUE;

    sPnCoreDrvIf.open      = eps_pncore_open;
    sPnCoreDrvIf.close     = eps_pncore_close;
    sPnCoreDrvIf.uninstall = eps_pncore_uninstall;
    sPnCoreDrvIf.update_device_list = eps_pncore_update_device_list;

    eps_shm_if_register(&sPnCoreDrvIf);
}

/**
 * WatchdogService, will be called periodicly
 *
 * @param [in] timer_id	- TimerId
 * @param [in] user_id	- UserId
 * @return LSA_VOID
 */
static LSA_VOID * eps_pncore_wd_service( LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id )
{
    LSA_BOOL   bDriverUp   = LSA_FALSE;
    LSA_UINT16 uIntervalMs = user_id.uvar16;

    LSA_UNUSED_ARG(timer_id);

    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);

    fnPnCore_SetSignOfLife(g_pEpsPnCoreDrv->sHw.pBase, LSA_TRUE);

    bDriverUp = fnPnCore_CheckSignOfLife(g_pEpsPnCoreDrv->sHw.pBase, uIntervalMs / 10, LSA_TRUE);

    if (!bDriverUp)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_WARN,"eps_pncore_wd_service() - Driver Sign of Life Timeout!");
    }

    return NULL;
}

/**
 * open pnCore Interface
 *
 * @see eps_shm_if_open     - calls this function to open the LD Lower IF.
 *
 * @param [in] hSys         - SystemHandle
 * @param [in] pOption      - pointer to SharedMemoryOptions
 * @param [in] ppHwInstOut  - pointer to structure with SharedHostMemoryConfiguration
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_pncore_open( LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppHwInstOut )
{
    LSA_UINT16 retVal = EPS_SHM_RET_OK;

    EPS_SHM_ADR_MAP_PTR_TYPE pShmAdrMap;

    LSA_UNUSED_ARG(hSys);

    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);
    EPS_ASSERT(ppHwInstOut != LSA_NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncore_open()");

    eps_pncore_enter();

    /* PNCore SDRAM base address from lk files */
#if defined (__ghs__)
    g_pEpsPnCoreDrv->sHw.pBase = (LSA_UINT8*)((LSA_UINT32)&__PNCORE_SHM_START__);
#elif (EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&g_pEpsPnCoreDrv->sHw.pBase, 0xA0000000, 0x500000);
#else
    g_pEpsPnCoreDrv->sHw.pBase = (LSA_UINT8*)((LSA_UINT32)__PNCORE_SHM_START__);
#endif

    if(retVal == EPS_SHM_RET_OK)
    {
        g_pEpsPnCoreDrv->bWdEnabled = LSA_FALSE;

        if(pOption != LSA_NULL)
        {
            g_pEpsPnCoreDrv->sHw.uHdNr = pOption->uHdNr;

            if(pOption->bEnableWatchdog)
            {
                eps_pncore_init_wd_service();
            }
        }

        /* Check if Debug mode active */
        g_pEpsPnCoreDrv->sHw.bDebug = fnPnCore_CheckDebugMode(g_pEpsPnCoreDrv->sHw.pBase);

        g_pEpsPnCoreDrv->StartupParFwIn.lSizeIndirect1               = 0;
        g_pEpsPnCoreDrv->StartupParFwIn.lTimeUpdateSignOfLife_10msec = 20; /* WD service is done every 100ms */
        g_pEpsPnCoreDrv->StartupParFwIn.uHif.lOffset      = 0x00200000;
        
        // the size in uHif.lSize given to pndevdriver shows the size of the direct mapped memory. This is not only
        // hif but also shm memory map ...
        g_pEpsPnCoreDrv->StartupParFwIn.uHif.lSize        = 0x001FE000;//reduce HIF size by 8kB to make space for shared memory debugging structure
        g_pEpsPnCoreDrv->StartupParFwIn.uFatal.lOffset    = 0x00400000;
        g_pEpsPnCoreDrv->StartupParFwIn.uFatal.lSize      = 0x00100000;
        g_pEpsPnCoreDrv->StartupParFwIn.uTrace[0].lOffset = 0x00001000;
        g_pEpsPnCoreDrv->StartupParFwIn.uTrace[0].lSize   = 0x000FF000;
        g_pEpsPnCoreDrv->StartupParFwIn.uTrace[1].lOffset = 0x00100000;
        g_pEpsPnCoreDrv->StartupParFwIn.uTrace[1].lSize   = 0x00100000;

        /* Exchange startup parameters */
        fnPnCore_ExchangeStartupPar(g_pEpsPnCoreDrv->sHw.pBase, &g_pEpsPnCoreDrv->StartupParFwIn, &g_pEpsPnCoreDrv->StartupParDriverOut);

        // Create shared memory map
        pShmAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)(g_pEpsPnCoreDrv->sHw.pBase + g_pEpsPnCoreDrv->StartupParFwIn.uHif.lOffset);

        /* Write Shm Adress Map, offsets in relation to g_pEpsPnCoreDrv->StartupParFwIn.uHif.lOffset*/
        eps_shm_map_write(pShmAdrMap, EPS_SHM_MAP_KEY, 	            0, 														                                                                    EPS_SHM_MAP_SIZE);              // SHM Map
        eps_shm_map_write(pShmAdrMap, EPS_SHM_MAP_KEY_HIF, 		    (pShmAdrMap->adr_map[EPS_SHM_MAP_KEY].offset +                  pShmAdrMap->adr_map[EPS_SHM_MAP_KEY].size),                 EPS_SHM_MAP_SIZE_HIF);	        // HIF
        eps_shm_map_write(pShmAdrMap, EPS_SHM_MAP_KEY_SRD_API_MEM,  (pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset +              pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size),             EPS_SHM_MAP_SIZE_SRD_API_MEM);  // SRD_API_MEM
        eps_shm_map_write(pShmAdrMap, EPS_SHM_MAP_KEY_GTEST,		(pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset +      pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size),     EPS_SHM_MAP_SIZE_GTEST);	    // GTest
        eps_shm_map_write(pShmAdrMap, EPS_SHM_MAP_KEY_FILE,     	(pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_GTEST].offset +            pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_GTEST].size),           EPS_SHM_MAP_SIZE_FILE);	        // FW file transfer
        eps_shm_map_write(pShmAdrMap, EPS_SHM_MAP_KEY_RSMD, 		(pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset +             pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].size),            EPS_SHM_MAP_SIZE_RSMD);	        // RSMD

        g_pEpsPnCoreDrv->sHw.bShmHalt		 = (g_pEpsPnCoreDrv->StartupParDriverOut.bShmHaltOnStartup == LSA_TRUE);

        g_pEpsPnCoreDrv->sHw.sHif.pBase      = g_pEpsPnCoreDrv->sHw.pBase + g_pEpsPnCoreDrv->StartupParFwIn.uHif.lOffset + pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset; //HIF Offset moved up by 100kB
        g_pEpsPnCoreDrv->sHw.sHif.uOffset    = g_pEpsPnCoreDrv->StartupParFwIn.uHif.lOffset + pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
        g_pEpsPnCoreDrv->sHw.sHif.uSize      = pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;
        g_pEpsPnCoreDrv->sHw.sSrdApi.pBase   = g_pEpsPnCoreDrv->sHw.sHif.pBase + g_pEpsPnCoreDrv->sHw.sHif.uSize;
        g_pEpsPnCoreDrv->sHw.sSrdApi.uOffset = g_pEpsPnCoreDrv->sHw.sHif.uOffset + g_pEpsPnCoreDrv->sHw.sHif.uSize;
        g_pEpsPnCoreDrv->sHw.sSrdApi.uSize   = pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size;
        g_pEpsPnCoreDrv->sHw.sFatal.pBase    = g_pEpsPnCoreDrv->sHw.pBase + g_pEpsPnCoreDrv->StartupParFwIn.uFatal.lOffset;
        g_pEpsPnCoreDrv->sHw.sFatal.uOffset  = g_pEpsPnCoreDrv->StartupParFwIn.uFatal.lOffset;
        g_pEpsPnCoreDrv->sHw.sFatal.uSize    = g_pEpsPnCoreDrv->StartupParFwIn.uFatal.lSize;
        g_pEpsPnCoreDrv->sHw.sPntrc1.pBase   = g_pEpsPnCoreDrv->sHw.pBase + g_pEpsPnCoreDrv->StartupParFwIn.uTrace[0].lOffset;
        g_pEpsPnCoreDrv->sHw.sPntrc1.uOffset = g_pEpsPnCoreDrv->StartupParFwIn.uTrace[0].lOffset;
        g_pEpsPnCoreDrv->sHw.sPntrc1.uSize   = g_pEpsPnCoreDrv->StartupParFwIn.uTrace[0].lSize;
        g_pEpsPnCoreDrv->sHw.sPntrc2.pBase   = g_pEpsPnCoreDrv->sHw.pBase + g_pEpsPnCoreDrv->StartupParFwIn.uTrace[1].lOffset;;
        g_pEpsPnCoreDrv->sHw.sPntrc2.uOffset = g_pEpsPnCoreDrv->StartupParFwIn.uTrace[1].lOffset;;
        g_pEpsPnCoreDrv->sHw.sPntrc2.uSize   = g_pEpsPnCoreDrv->StartupParFwIn.uTrace[1].lSize;;

        g_pEpsPnCoreDrv->bOpen = LSA_TRUE;

#if ((EPS_PLF == EPS_PLF_PNIP_A53) ||  (EPS_PLF == EPS_PLF_ARM_CORTEX_A15) || (EPS_PLF == EPS_PLF_PNIP_ARM9))
        eps_shm_file_init(g_pEpsPnCoreDrv->sHw.pBase + g_pEpsPnCoreDrv->StartupParFwIn.uHif.lOffset + pShmAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset);
#endif

        eps_pncore_open_hif_driver(&g_pEpsPnCoreDrv->sHw);

        *ppHwInstOut = &g_pEpsPnCoreDrv->sHw;
    }

    eps_pncore_exit();

    return retVal;
}

/**
 * close pnCore Interface
 *
 * @param [in] pHwInstIn    - pointer to structure with SharedHostMemoryConfiguration
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_pncore_close( EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn )
{
    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncore_close()");

    eps_pncore_enter();

    eps_pncore_undo_init_wd_service();

    EPS_ASSERT(g_pEpsPnCoreDrv->bOpen == LSA_TRUE);

#if ((EPS_PLF == EPS_PLF_PNIP_A53) ||  (EPS_PLF == EPS_PLF_ARM_CORTEX_A15) || (EPS_PLF == EPS_PLF_PNIP_ARM9))
    eps_shm_file_undo_init();
#endif
    
    eps_pncore_close_hif_driver(pHwInstIn);

    g_pEpsPnCoreDrv->bOpen = LSA_FALSE;

    eps_pncore_exit();

    return EPS_SHM_RET_OK;
}

#if ( PSI_CFG_USE_PNTRC == 1 )
/**
 * The SHM API requires the implementation of this function.
 *
 * Since this is an EPS Basic implementation, the tracebuffer must never be emptied, since the Basic FW is never the topmost.
 *
 * @see PNTRC_BUFFER_FULL          - calls tis function
 * @param [in] hTrace              - Trace handle ID from PNTRC
 * @param [in] uBufferId           - Buffer ID from PNTRC
 * @param [in] pBuffer             - Buffer to be emptied (SRC)
 * @param [in] uLength             - Length of the Buffer to be emptied (SRC)
 * @return LSA_VOID
 */
static LSA_VOID eps_pncore_trace_buffer_full( LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 uBufferId, LSA_UINT8 * pBuffer, LSA_UINT32 uLength )
{
    LSA_UNUSED_ARG(hTrace);
    LSA_UNUSED_ARG(uBufferId);
    LSA_UNUSED_ARG(uBufferId);
    LSA_UNUSED_ARG(uBufferId);
    LSA_UNUSED_ARG(pBuffer);
    LSA_UNUSED_ARG(uLength);

    /* EPS_FATAL(0); */ /* Should never happen, because we are not TopMost */
}
#endif

/**
 * signals Application ready
 *
 * @see eps_startup_done - calls this function
 *
 * @param [in] pHwInstIn	 - pointer to structure with SharedHostMemoryConfiguration
 * @param [in] pHifHandle	 - unused
 * @return EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_pncore_app_ready( EPS_SHM_HW_PTR_TYPE pHwInstIn, LSA_VOID_PTR_TYPE pHifHandle )
{
    LSA_BOOL bReady = LSA_FALSE;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "eps_pncore_app_ready()");

    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    LSA_UNUSED_ARG(pHifHandle);

    eps_pncore_enter();

    EPS_ASSERT(g_pEpsPnCoreDrv->bOpen == LSA_TRUE);

    while (!bReady)
    {
        bReady = fnPnCore_ExchangeStartupState(g_pEpsPnCoreDrv->sHw.pBase);
    }

    eps_pncore_exit();

    return EPS_SHM_RET_OK;
}

/**
 * shut down application (not used right now)
 *
 * @see eps_undo_init     - calls this function
 *
 * @param [in] pHwInstIn	- pointer to structure with SharedHostMemoryConfiguration
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_pncore_app_shutdown( EPS_SHM_HW_PTR_TYPE pHwInstIn )
{	// no functionality at the moment (4/2013)

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"eps_pncore_app_shutdown()");

    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    return EPS_SHM_RET_OK;
}

/**
 * signal Fatal of Firmware to PnDevDriver
 *
 * @see eps_fatal_with_reason   - calls this function to signal a FATAL caused by PSI or EPS.
 *
 * @param [in] pHwInstIn        - pointer to structure with SharedHostMemoryConfiguration
 * @param [in] sFatal	        - structur with FatalInformation
 * @return #EPS_SHM_RET_OK
 */
static LSA_UINT16 eps_pncore_sig_fatal( EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal )
{
    EPS_ASSERT(g_pEpsPnCoreDrv != LSA_NULL);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    g_pEpsPnCoreDrv->bFatal = LSA_TRUE;
    g_pEpsPnCoreDrv->bOpen  = LSA_FALSE;

    fnPnCore_SetFatal(g_pEpsPnCoreDrv->sHw.pBase, sFatal.exit_code_org);

    return EPS_SHM_RET_OK;
}

/**
 * get all MacAddr
 *
 * @param [out] pMacAddr	- pointer structure, where CtrMacAdr and MacAdr[] will be filled up
 * @return
 */
LSA_VOID eps_pncore_get_mac_addr( EPS_PNCORE_MAC_ADDR_PTR_TYPE pMacAddr )
{
    LSA_UINT32 i;

    eps_pncore_enter();

    EPS_ASSERT(g_pEpsPnCoreDrv->bOpen == LSA_TRUE); /* Access is only valid if startup is done */
    EPS_ASSERT(pMacAddr != LSA_NULL);
    EPS_ASSERT(g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lCtrMacAdr <= EPS_PNCORE_MAC_ADR_MAX);

    pMacAddr->lCtrMacAdr = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lCtrMacAdr;

    for (i=0; i<pMacAddr->lCtrMacAdr; i++)
    {
        pMacAddr->lArrayMacAdr[i][0] = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lArrayMacAdr[i][0];
        pMacAddr->lArrayMacAdr[i][1] = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lArrayMacAdr[i][1];
        pMacAddr->lArrayMacAdr[i][2] = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lArrayMacAdr[i][2];
        pMacAddr->lArrayMacAdr[i][3] = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lArrayMacAdr[i][3];
        pMacAddr->lArrayMacAdr[i][4] = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lArrayMacAdr[i][4];
        pMacAddr->lArrayMacAdr[i][5] = g_pEpsPnCoreDrv->StartupParDriverOut.uRemanentData.lArrayMacAdr[i][5];
    }

    eps_pncore_exit();
}

#else /* EPS_CFC_USE_PNCORE not defined */

LSA_VOID eps_pncoredrv_install( EPS_SHM_INSTALL_ARGS_PTR_TYPE pInstallArgs )
{
    /* do nothing if pncore interface is not supported */
}

#endif /* EPS_CFC_USE_PNCORE */
