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
/*  F i l e               &F: eps_pn_vdd_drv.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN VDD LD Upper Interface Adaption                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20043
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/*****************************************************************************/
/*   Includes                                                                */
/*****************************************************************************/
#include <eps_sys.h>			/* Types / Prototypes / Funcs               */
#include <eps_trc.h>			/* Tracing                                  */
#include <eps_rtos.h>			/* OS for Thread Api                        */
#include <eps_tasks.h>          /* EPS TASK API                             */
#include <eps_locks.h>          /* EPS Locks                                */
#include <eps_cp_hw.h>          /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>		/* PN Device Driver Interface               */
#include <eps_shm_if.h>         /* Shared Memory Interface                  */
//#include <eps_pndrvif.h>      /* EPS Adonis Pnip Driver If                */
#include <eps_register.h>       /* EPS Register access macros               */
#include "eps_hif_drv_if.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif
#include "eps_pn_vdd_drv.h"     // VDD IF

#include "eps_vdd_helper.h"
//lint --e(537) Repeated include file. Without the header we get compile errors.
//#include "eps_shm_if_config.h" 

// required by eps_pn_vdd_write_lower_time to read the TSC value 
// processor: x86, x64
#include <stdio.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)

/*****************************************************************************/
/*   Global Data                                                             */
/*****************************************************************************/
static EPS_VDD_GLOBAL_VALS_TYPE    g_vdd_vals;
static EPS_PN_VDD_STORE_TYPE          g_EpsVdd;
static EPS_PN_VDD_STORE_PTR_TYPE      g_pEpsVdd = LSA_NULL;

//Function '' whose address was taken has an unprotected access to variable '' - intended by design
//lint --esym(459, g_vdd_vals)

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
static LSA_VOID                     eps_pn_vdd_undo_init_critical_section  (LSA_VOID);
static LSA_VOID                     eps_pn_vdd_init_critical_section       (LSA_VOID);
static LSA_VOID                     eps_pn_vdd_enter                       (LSA_VOID);
static LSA_VOID                     eps_pn_vdd_exit                        (LSA_VOID);
static EPS_PN_VDD_BOARD_PTR_TYPE    eps_pn_vdd_alloc_board                 (LSA_VOID);
static LSA_VOID                     eps_pn_vdd_free_board                  (EPS_PN_VDD_BOARD_PTR_TYPE pBoard);

//Thread functions
static LSA_UINT16   eps_pn_vdd_disable_interrupt                   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16   eps_pn_vdd_enable_interrupt                    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16   eps_pn_vdd_kill_pn_gathered_polling_thread     (EPS_PN_VDD_BOARD_CONST_PTR_TYPE pBoard);
static LSA_UINT16   eps_pn_vdd_create_pn_gathered_polling_thread   (EPS_PN_VDD_BOARD_PTR_TYPE pBoard);
static LSA_VOID     eps_pn_vdd_create_wd_thread                    (LSA_CHAR const* cName);
static LSA_VOID     eps_pn_vdd_kill_poll_thread                    (EPS_VDD_POLL_PTR_TYPE pPollIsr);
static LSA_VOID     eps_pn_vdd_kill_wd_thread                      ( LSA_VOID );
static LSA_UINT16   eps_pn_vdd_create_poll_thread                  (LSA_CHAR const* cName, EPS_VDD_POLL_PTR_TYPE pPollIsr);
static LSA_VOID     eps_pn_vdd_poll_isr_thread                     (LSA_UINT32 uParam, void *arg);
static LSA_UINT16   eps_pn_vdd_unregister_board_for_polling        (EPS_VDD_POLL_PTR_TYPE pPollIsr, EPS_PN_VDD_BOARD_CONST_PTR_TYPE pBoard, LSA_BOOL *bKillThread);
static LSA_UINT16   eps_pn_vdd_register_board_for_polling          (EPS_VDD_POLL_PTR_TYPE pPollIsr, EPS_PN_VDD_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread);
static LSA_BOOL     eps_pn_vdd_file_transfer                       ( LSA_VOID );
static LSA_UINT16   eps_pn_vdd_save_dump                           (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16   eps_pn_vdd_enable_hw_interrupt                 (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16   eps_pn_vdd_disable_hw_interrupt                (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16   eps_pn_vdd_read_hw_interrupt                   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16   eps_pn_vdd_write_sync_time_lower               (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);

static LSA_VOID     eps_pn_vdd_uninstall                            (LSA_VOID);
static LSA_UINT16   eps_pn_vdd_open                                 (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16   eps_pn_vdd_close                                (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_BOOL     eps_pn_vdd_connect_board                        (LSA_UINT16 boardNr, EPS_PN_VDD_BOARD_PTR_TYPE pBoard); 
static LSA_BOOL     eps_pn_vdd_disconnect_board                     (LSA_UINT16 boardNr);
static LSA_BOOL     eps_pn_vdd_get_board                            (LSA_UINT16 boardNr, EPS_PNDEV_LOCATION_CONST_PTR_TYPE location_id );
static LSA_BOOL     eps_pn_vdd_connect_driver                       (LSA_VOID );

/**
 * Install VDD
 * 
 * @see eps_pn_vdd_install
 * 
 * @param LSA_VOID
 * @return 
 */
LSA_VOID eps_pn_vdd_install( LSA_VOID )
{
    #if (EPS_PLF != EPS_PLF_WINDOWS_X86)
    EPS_FATAL(0);
    #endif

	EPS_PNDEV_IF_TYPE sPnVddIf;

    g_pEpsVdd = &g_EpsVdd;

    eps_memset(g_pEpsVdd, 0, sizeof(EPS_PN_VDD_STORE_TYPE));

    eps_memset(g_vdd_vals.DeviceOpenList, 0, sizeof(g_vdd_vals.DeviceOpenList));
	g_vdd_vals.bVddStarted                = LSA_FALSE;
	g_vdd_vals.bFWDebugActivated          = LSA_FALSE;
	g_vdd_vals.uThreadFileTransferUserCnt = 0;

    g_vdd_vals.bVddStarted = LSA_TRUE;

    eps_pn_vdd_init_critical_section();

	//Init Interface
    sPnVddIf.open      = eps_pn_vdd_open;
    sPnVddIf.close     = eps_pn_vdd_close;
    sPnVddIf.uninstall = eps_pn_vdd_uninstall;

	eps_pndev_if_register(&sPnVddIf);
}

/**
 * Uninstall VDD
 * 
 * @see  eps_pn_vdd_uninstall
 * 
 * @param LSA_VOID
 * @return 
 */
static LSA_VOID eps_pn_vdd_uninstall( LSA_VOID )
{
    eps_pn_vdd_undo_init_critical_section();
	// cleanup resources
}

/**
 * Connect to board and starting filetransfer thread  
 * 
 * @see eps_pn_vdd_connect_board
 * 
 * @param boardNr   
 * @param pBoard
 * @return #LSA_TRUE
 */
static LSA_BOOL eps_pn_vdd_connect_board( LSA_UINT16 boardNr, EPS_PN_VDD_BOARD_PTR_TYPE pBoard )
{
	EPS_ASSERT(boardNr <= PSI_CFG_MAX_IF_CNT);

	if (g_vdd_vals.uThreadFileTransferUserCnt == 0)
    {
		LSA_UINT16 result = eps_pn_vdd_create_pn_gathered_polling_thread(pBoard);

		if (result != EPS_PNDEV_RET_OK)
	    {
			return LSA_FALSE;
		}
	}

	g_vdd_vals.uThreadFileTransferUserCnt++;

	g_vdd_vals.DeviceOpenList[boardNr].board_nr   = boardNr;
	g_vdd_vals.DeviceOpenList[boardNr].hif_handle = 0;
	g_vdd_vals.DeviceOpenList[boardNr].is_open    = LSA_TRUE;

	return LSA_TRUE;
}

#if ( EPS_CFG_USE_HIF == 1 )
/**
 * Open the HIF driver
 * 
 * @see eps_pn_vdd_open_hif_driver
 * 
 * @param pHwInstOut
 * @return
 */
static LSA_VOID eps_pn_vdd_open_hif_driver( EPS_SHM_HW_PTR_TYPE pHwInstOut )
{
	LSA_UINT16 retVal;
	EPS_HIF_DRV_OPENPAR_TYPE InPars;
	EPS_HIF_DRV_CFG_TYPE sHifCfg;

	EPS_ASSERT(g_pEpsVdd != LSA_NULL);

	eps_memset(&InPars, 0, sizeof(InPars));

	InPars.Mode = EPS_HIF_DRV_HIF_TM_SHARED_MEM;
    InPars.if_pars.SendIrqMode = HIF_SEND_IRQ_AFTER_NO_WAIT;  // don't care (polling)
	InPars.if_pars.SharedMem.Local.Base  = pHwInstOut->sHif.pBase;
	InPars.if_pars.SharedMem.Local.Size  = pHwInstOut->sHif.uSize/2;
	InPars.if_pars.SharedMem.Remote.Base = pHwInstOut->sHif.pBase + (pHwInstOut->sHif.uSize/2);
	InPars.if_pars.SharedMem.Remote.Size = pHwInstOut->sHif.uSize/2;

	eps_memset(&sHifCfg, 0, sizeof(sHifCfg));
	sHifCfg.bUseSharedMemMode    = LSA_TRUE;
	sHifCfg.bUseRemoteMem        = LSA_TRUE;
	sHifCfg.bUseDma              = LSA_FALSE;
	sHifCfg.bUseIpcInterrupts    = LSA_FALSE;
	sHifCfg.bUsePnCoreInterrupts = LSA_FALSE;

	retVal = eps_hif_drv_if_open((LSA_UINT16)pHwInstOut->uHdNr, LSA_TRUE /*bUpper*/, &sHifCfg, &InPars, &g_pEpsVdd->pHifDrvHwInstOut);

	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
	//lint --e(818) Pointer parameter 'pHwInstOut' could be declared as pointing to const. Required by API to be not const.
}
#endif // ( EPS_CFG_USE_HIF == 1 )
/**
 * 
 * 
 * @see eps_pn_vdd_connect_driver
 * 
 * @param LSA_VOID
 * @return
*/
static LSA_BOOL eps_pn_vdd_connect_driver( LSA_VOID )
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    LSA_UINT32 idx = 0;

    // *****************************************************************************
    // new connect using VDD
    uVDD_GET_DEVICE_INFO_IN		uGetDeviceInfoIn;
    uVDD_OPEN_DEVICE_IN			uOpenDeviceIn;
    eVDD_RESULT					drvResult;
    _TCHAR						sError[VDD_SIZE_STRING_BUF] = { 0 };
    UINT32						lCtrDevice = 0;
    uVDD_GET_DEVICE_INFO_OUT	uArrayDeviceInfo[1];
    uVDD_OPEN_DEVICE_OUT		uOpenDeviceOut;

    eps_memset(&uOpenDeviceIn, 0, sizeof(uVDD_OPEN_DEVICE_IN));

    // get DeviceInfo of all devices
    drvResult = (   *g_pEpsVdd->pDrvHandle->uIf.pFnGetDeviceInfo)(	g_pEpsVdd->pDrvHandle,
																	sizeof(uVDD_GET_DEVICE_INFO_IN),
																	sizeof(uVDD_GET_DEVICE_INFO_OUT),
																	1,
																	_countof(sError),
																	&uGetDeviceInfoIn,
																	uArrayDeviceInfo,
																	&lCtrDevice,
																	sError);
    
    EPS_ASSERT(drvResult == ePNDEV_OK);

    uOpenDeviceIn.uCbfIsr.pPtr          = LSA_NULL;

    uOpenDeviceIn.uCbfEvent.pPtr        = eps_vdd_event_cbf;
    uOpenDeviceIn.uCbfParUser.pPtr      = (void *)&uOpenDeviceOut;

    uOpenDeviceIn.uThreadDll.lPrio      = VDD_THREAD_PRIO_HIGH;
    uOpenDeviceIn.uThreadDll.lStackSize = 0;        /* Use Default 8kB according to documentation */
    uOpenDeviceIn.uThreadSys.lPrio      = VDD_THREAD_PRIO_HIGH;
    uOpenDeviceIn.uThreadSys.lStackSize = 0;        /* Use Default 8kB according to documentation */
    uOpenDeviceIn.uThreadSys.lTimeCycle_10msec = 0; /* Use Default 100ms according to documentation */

    eps_memcpy(uOpenDeviceIn.sPciLoc, uArrayDeviceInfo[0].sPciLoc, sizeof(uOpenDeviceIn.sPciLoc));

    drvResult = g_pEpsVdd->pDrvHandle->uIf.pFnOpenDevice(g_pEpsVdd->pDrvHandle, sizeof(uOpenDeviceIn), sizeof(uOpenDeviceOut), &uOpenDeviceIn, &uOpenDeviceOut);

    if (drvResult != ePNDEV_OK)
    {
        LSA_CHAR sErrConv[VDD_SIZE_STRING_BUF] = { 0 };
        eps_convert_vdd_err_string(uOpenDeviceOut.sError, sErrConv);
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_vdd_connect_driver() %s", sErrConv);
    }

    // set the pointer to address given from VDD
    g_vdd_vals.pShmHeaderShared = (EPS_VDD_SHM_MEM_HEADER_PTR_TYPE)EPS_CAST_TO_VOID_PTR(uOpenDeviceOut.uHwRes.as.uVirtPCIDev.uSharedMem.uBase.pPtr);

    // *****************************************************************************

	while ( g_vdd_vals.pShmHeaderShared->uSyncSelf != EPS_PN_VDD_SELF_STATE_READY )
	{
		Sleep(1000); // wait 1000 ms
		// wait for initializing the Shared Memory

		if (idx >= 2)
		{
			return LSA_FALSE;
		}

		idx++;
	}

	g_vdd_vals.pShmHeaderShared->bDebugOn = ( g_vdd_vals.bFWDebugActivated ) ? 1 : 0; 

	g_vdd_vals.pShmHeaderShared->uSyncRemote = EPS_PN_VDD_REMO_STATE_READY; // Shared Memory is initialized

    // Start VDD-Watchdog if NOT debug-mode
    if  (g_vdd_vals.pShmHeaderShared->bDebugOn == LSA_FALSE)
        // in Debug-Mode Watch-Dog is not active
    {
        eps_pn_vdd_create_wd_thread("EPS_VDD_WD_ISR");
    }
    else
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-Watchdog (upper): not activated - bDebugOn: %d", g_vdd_vals.pShmHeaderShared->bDebugOn);
    }
    #endif

    return LSA_TRUE;
}

/**
 * Get pointer, size and offset from HW 
 * 
 * @see eps_pn_vdd_read_hw_param
 * 
 * @param boardNr
 * @param pHW
 * @return LSA_BOOL
 */
static LSA_VOID eps_pn_vdd_read_hw_param( LSA_UINT16 boardNr, EPS_SHM_HW_PTR_TYPE pHW )
{
	//EPS_ASSERT( pParam != 0 );
	EPS_ASSERT(boardNr <= PSI_CFG_MAX_IF_CNT);
	EPS_ASSERT(g_vdd_vals.DeviceOpenList[boardNr].is_open); // Connected ?

	// setup HIF buffer
	pHW->sHif.pBase   = (LSA_UINT8*)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uHifOffset;
	pHW->sHif.uOffset = (LSA_UINT32)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uHifOffset;
	pHW->sHif.uSize   = g_vdd_vals.pShmHeaderShared->uHifSize;

	// setup PNTRC buffer
	// note: the PNTRC memory blocks must be read indirect
	pHW->sPntrc1.pBase   = (LSA_UINT8*)g_vdd_vals.pShmHeaderShared->uTrace1Offset;
	pHW->sPntrc1.uOffset = (LSA_UINT32)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uTrace1Offset;
	pHW->sPntrc1.uSize   = g_vdd_vals.pShmHeaderShared->uTrace1Size;

	pHW->sPntrc2.pBase   = (LSA_UINT8*)g_vdd_vals.pShmHeaderShared->uTrace2Offset;
	pHW->sPntrc2.uOffset = (LSA_UINT32)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uTrace2Offset;
	pHW->sPntrc2.uSize   = g_vdd_vals.pShmHeaderShared->uTrace2Size;

	// setup FATAL buffer
	// note: the FATAL memory blocks must be read indirect
	pHW->sFatal.pBase   = (LSA_UINT8 *)g_vdd_vals.pShmHeaderShared->uFatalOffset;
	pHW->sFatal.uOffset = (LSA_UINT32)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uFatalOffset;
	pHW->sFatal.uSize   = g_vdd_vals.pShmHeaderShared->uFatalSize;
}

/**
 * open pntrc lower cpu
 * 
 * @param pBoard	pointer to current board structur
 * @param pOption	ptr to options, if FirmwareDownload and Debugging will be executed
 * @return 	LSA_VOID
 */
static LSA_VOID eps_pn_vdd_open_pntrc_lower_cpu( EPS_PN_VDD_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption )
{
	if(pOption != LSA_NULL)
	{
		pBoard->Pntrc.hPntrcCPU = pntrc_open_lower_cpu((PNTRC_SYS_HANDLE_LOWER_PTR_TYPE)&pBoard->sHw /*EPS_PNDEV_HW_TYPE*/);
		pBoard->Pntrc.bOpened = LSA_TRUE;
	}
	//lint --e(818) Pointer parameter 'pOption' could be declared as pointing to const. Required by API to be not const.
}

/**
 * close pntrc lower cpu
 * 
 * @param pBoard	pointer to current board structur
 * @return 	LSA_VOID
 */
static LSA_VOID eps_pn_vdd_close_pntrc_lower_cpu(EPS_PN_VDD_BOARD_PTR_TYPE pBoard)
{
	if (pBoard->Pntrc.bOpened) 
    {
		pntrc_close_lower_cpu(pBoard->Pntrc.hPntrcCPU);
		pBoard->Pntrc.bOpened = LSA_FALSE;
	}
}

/**
 *  Open VDD
 * 
 * @see eps_pn_vdd_open
 * 
 * @param pLocation
 * @param pOption
 * @param ppHwInstOut
 * @param hd_id
 * @return
 */
static LSA_UINT16 eps_pn_vdd_open( EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id )
{
    LSA_UINT16					Result  = EPS_PNDEV_RET_ERR;
    LSA_BOOL					bResult;
    EPS_PN_VDD_BOARD_PTR_TYPE   pBoard;

    LSA_UNUSED_ARG(pOption);

    (void)PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    eps_pn_vdd_enter();

    pBoard = eps_pn_vdd_alloc_board();

    if (pBoard)
    {
        // *****************************************************************************
        // install VDD
        VDD_CHAR				drvError[VDD_SIZE_STRING_BUF];
        eVDD_RESULT				eResult = eVDD_FATAL;
        EPS_APP_DRIVER_PATH_ARR uPathArr;
        LSA_UINT32				lLoopCount;

        eps_memset(&uPathArr, 0, sizeof(uPathArr));

        EPS_APP_GET_DRIVER_PATH(&uPathArr, EPS_APP_DRIVER_VDD);

        // Try to startup VDD DLL from the given paths
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

        if (eResult != eVDD_OK)
        {
            LSA_CHAR sErrConv[VDD_SIZE_STRING_BUF] = { 0 };
            eps_convert_vdd_err_string(drvError, sErrConv);
            EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS VDD %s", sErrConv);

            // For Adonis no fatal is called
            #if (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_WINDOWS_X86)
            EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_DRIVER_NOT_FOUND, EPS_EXIT_CODE_DRIVER_NOT_FOUND, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pn_vdd_open() - Couldn't load VDD-DLL");
            #endif
        }
        // *****************************************************************************
        
        // now connect with the SharedMemory
		bResult = eps_pn_vdd_connect_driver();

		if (bResult)
		{
			bResult = eps_pn_vdd_connect_board(hd_id, pBoard);

            if (bResult)
            {
			    eps_pn_vdd_read_hw_param(hd_id, &g_pEpsVdd->sHw /*EPS_SHM_HW_TYPE*/);

			    bResult = eps_pn_vdd_get_board(hd_id /*boardNr*/, pLocation);

                if (bResult)
                {
                    eps_pn_vdd_open_pntrc_lower_cpu(pBoard, pOption);
                    #if ( EPS_CFG_USE_HIF == 1 )
			        eps_pn_vdd_open_hif_driver(&g_pEpsVdd->sHw);
                    #endif
                    *ppHwInstOut         = &pBoard->sHw;
                    pBoard->hd_id        = hd_id;
	                pBoard->sysDev.hd_nr = hd_id;

			        Result = EPS_PNDEV_RET_OK;
                }
                else
                {
		            Result = EPS_PNDEV_RET_DEV_NOT_FOUND;
                    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_vdd_open(): Error: EPS_PNDEV_RET_DEV_NOT_FOUND");
                }
            }
		}
		else
        {
            EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_ADVANCED_FATAL, EPS_EXIT_CODE_ADVANCED_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pn_vdd_open(): Couldn't connect driver");
		}
    }

    eps_pn_vdd_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129

	return Result;
}

/**
 * 
 * 
 * @see eps_pn_vdd_disconnect_board
 * 
 * @param boardNr
 * @return
 */
static LSA_BOOL eps_pn_vdd_disconnect_board( LSA_UINT16 boardNr ) 
{
	EPS_ASSERT(boardNr <= PSI_CFG_MAX_IF_CNT);
	EPS_ASSERT(g_vdd_vals.DeviceOpenList[boardNr].is_open);

	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">> eps_pn_vdd_disconnect_board(): uThreadFileTransferUserCnt=%d",
        g_vdd_vals.uThreadFileTransferUserCnt);

    g_vdd_vals.uThreadFileTransferUserCnt--;

	if(g_vdd_vals.uThreadFileTransferUserCnt == 0)
	{
		LSA_UINT16 result;

		result = eps_pn_vdd_kill_pn_gathered_polling_thread(&g_pEpsVdd->pBoard[boardNr]);

		if (result != EPS_PNDEV_RET_OK)
		{
	        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< eps_pn_vdd_disconnect_board(): return = LSA_FALSE, result=%d", result);

            return LSA_FALSE;
	    }
	}

	g_vdd_vals.DeviceOpenList[boardNr].is_open    = LSA_FALSE;
	g_vdd_vals.DeviceOpenList[boardNr].board_nr   = 0;
	g_vdd_vals.DeviceOpenList[boardNr].hif_handle = 0;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< eps_pn_vdd_disconnect_board(): return = LSA_TRUE");

    return LSA_TRUE;
}

#if ( EPS_CFG_USE_HIF == 1 )
/**
 * 
 * 
 * @see eps_pn_vdd_close_hif_driver
 * 
 * @param pHwInstOut
 * @return
 */
static LSA_VOID eps_pn_vdd_close_hif_driver( EPS_SHM_HW_PTR_TYPE pHwInstOut )
{
	LSA_UINT16 retVal;

    retVal = eps_hif_drv_if_close((LSA_UINT16)pHwInstOut->uHdNr, LSA_TRUE /*bUpper*/);

	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
	//lint --e(818) Pointer parameter 'pHwInstOut' could be declared as pointing to const. Required by API to be not const.
}
#endif // ( EPS_CFG_USE_HIF == 1 )

/**
 * 
 * 
 * @see eps_pn_vdd_close
 * 
 * @param pHwInstIn
 * @return
 */
static LSA_UINT16 eps_pn_vdd_close( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    LSA_UINT16                  boardNr;
    EPS_PN_VDD_BOARD_PTR_TYPE   pBoard;
    LSA_UINT16                  adnRebootCounter = 0;
    VDD_CHAR					drvError[VDD_SIZE_STRING_BUF];
    LSA_BOOL                    bResult;
    LSA_UNUSED_ARG(pHwInstIn);   

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">> eps_pn_vdd_close()");

    //Close and init all open boards
    for (boardNr = 0; boardNr < PSI_CFG_MAX_IF_CNT; boardNr++)
    {
        if (g_vdd_vals.DeviceOpenList[boardNr].is_open)
        {
            bResult = eps_pn_vdd_disconnect_board(boardNr);
            EPS_ASSERT(bResult);
        }
    }
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_vdd_close(): disconnect_board done");

    // disable Watchdog an write Closed-State to VddShm
    eps_pn_vdd_kill_wd_thread();

    (void)PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    eps_pn_vdd_enter();

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_vdd_close(): enter_critical_section");

    g_vdd_vals.pShmHeaderShared->uSyncSelf    = EPS_PN_VDD_SELF_STATE_NINIT;  // reset the adonis state
    g_vdd_vals.pShmHeaderShared->uSyncRemote  = EPS_PN_VDD_REMO_STATE_CLOSED; // pciox is going down
    
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_vdd_close(): start waiting for ADN core reboot");
    // wait for the reboot of adonis core
    while (g_vdd_vals.pShmHeaderShared->uSyncSelf != EPS_PN_VDD_SELF_STATE_READY)
    {
        adnRebootCounter++;
        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_pn_vdd_close(): EPS VDD (upper): ADN core reboot wait loop - RebootCounter: %d (*500ms), pShmHeaderShared->uSyncSelf: 0x%08x",
            adnRebootCounter, g_vdd_vals.pShmHeaderShared->uSyncSelf);

        if (EPS_PN_VDD_ADN_REBOOT_MAX_TIMEOUTCOUNTER == adnRebootCounter)
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_pn_vdd_close(): EPS VDD (upper): ADN core reboot failed - RebootCounter: %d (*500ms)", adnRebootCounter);

            EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_ADVANCED_FATAL, EPS_EXIT_CODE_ADVANCED_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "EPS VDD (upper): Timeout occured during automized reboot of ADN core (error@ADN)");
        }

        eps_tasks_sleep(500);
    }

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_vdd_close(): EPS VDD (upper): ADN core reboot succeeded - RebootCounter: %d (*500ms)", adnRebootCounter);

    pBoard = (EPS_PN_VDD_BOARD_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pHwInstIn->hDevice);

    eps_pn_vdd_close_pntrc_lower_cpu(pBoard);
    #if ( EPS_CFG_USE_HIF == 1 )
    eps_pn_vdd_close_hif_driver(&g_pEpsVdd->sHw);
    #endif
	
    // *****************************************************************************
    // Shutdown the VDD

    if (g_pEpsVdd->pDrvHandle != 0)
    {
        eVDD_RESULT eResult;

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

    // free board is always as last action because the entry "g_pEpsVdd->pBoard[x]" will be deleted here
    eps_pn_vdd_free_board(pBoard);

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_vdd_close(): exit_critical_section");

    eps_pn_vdd_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< eps_pn_vdd_close()");

    return EPS_PNDEV_RET_OK;

    #else
    LSA_UNUSED_ARG(pHwInstIn);
    return EPS_PNDEV_RET_OK;
    #endif
}

/**
 * 
 * 
 * @see eps_pn_vdd_get_board
 * 
 * @param boardNr
 * @param location_id
 * @return
 */
static LSA_BOOL eps_pn_vdd_get_board( LSA_UINT16 boardNr, EPS_PNDEV_LOCATION_CONST_PTR_TYPE location_id ) 
{
    LSA_UINT16 i;
    LSA_BOOL   bHdFound = LSA_FALSE; //= no HD found

	//load IMCEA_DRV and search for shared memory
	if (g_vdd_vals.bVddStarted == LSA_FALSE)
	{
        EPS_FATAL(0);
	}

	EPS_ASSERT(boardNr <= PSI_CFG_MAX_IF_CNT);
	EPS_ASSERT(g_vdd_vals.pShmHeaderShared != LSA_NULL);

	//create a matching board based on driver Info
	for (i = 0; i<10; i++)
	{
        if (((LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uPciBusNr == location_id->uBusNr) &&
            ((LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uPciDeviceNr == location_id->uDeviceNr) &&
            ((LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uPciFuncNr == location_id->uFctNr))
		{
			g_vdd_vals.DeviceOpenList[boardNr].openInfo.uEddType     = (LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uEddType;
			g_vdd_vals.DeviceOpenList[boardNr].openInfo.uPciBusNr    = (LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uPciBusNr;
			g_vdd_vals.DeviceOpenList[boardNr].openInfo.uPciDeviceNr = (LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uPciDeviceNr;
			g_vdd_vals.DeviceOpenList[boardNr].openInfo.uPciFuncNr   = (LSA_UINT16)g_vdd_vals.pShmHeaderShared->BoardInfoStruct[i].uPciFuncNr;
            bHdFound = LSA_TRUE;
			break;
		}
	}

    return bHdFound;
}

/**
 * 
 * @see eps_pn_vdd_undo_init_critical_section
 * 
 * @param LSA_VOID
 * @return
*/
static LSA_VOID eps_pn_vdd_undo_init_critical_section(LSA_VOID)
{
    LSA_UINT16 retVal;
    retVal = eps_free_critical_section(g_pEpsVdd->hEnterExit);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * 
 * @see eps_pn_vdd_init_critical_section
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pn_vdd_init_critical_section(LSA_VOID)
{ 
    LSA_UINT16 retVal;
    retVal = eps_alloc_critical_section(&g_pEpsVdd->hEnterExit, LSA_FALSE);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * 
 * @see eps_pn_vdd_enter
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pn_vdd_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsVdd->hEnterExit);
}

/**
 * 
 * @see eps_pn_vdd_exit
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pn_vdd_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsVdd->hEnterExit);
}

/**
 * @see eps_pn_vdd_register_board_for_polling
 * 
 * @param pPollIsr
 * @param pBoard
 * @param uIntSrc
 * @param bCreateThread
 * @return
 */
static LSA_UINT16 eps_pn_vdd_register_board_for_polling(EPS_VDD_POLL_PTR_TYPE pPollIsr, EPS_PN_VDD_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
{
	LSA_UINT32 i;

	*bCreateThread = LSA_FALSE;

	if(pPollIsr->uCntUsers >= EPS_CFG_PN_VDD_MAX_BOARDS)
	{
		return EPS_PNDEV_RET_ERR; /* No More Resources */
	}

	for(i=0; i<EPS_CFG_PN_VDD_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			return EPS_PNDEV_RET_ERR; /* Board polling already enabled */
		}
	}

	for(i=0; i<EPS_CFG_PN_VDD_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == LSA_NULL)
		{
			pPollIsr->pBoard[i]  = pBoard;  /* Register Board for polling */
			pPollIsr->uIntSrc[i] = uIntSrc;
			break;
		}
	}

	pPollIsr->uCntUsers++;

	if(pPollIsr->bRunning == LSA_FALSE)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}

/**
 * 
 * @see eps_pn_vdd_unregister_board_for_polling
 * 
 * @param pPollIsr
 * @param pBoard
 * @param bKillThread
 * @return
 */
static LSA_UINT16 eps_pn_vdd_unregister_board_for_polling(EPS_VDD_POLL_PTR_TYPE pPollIsr, EPS_PN_VDD_BOARD_CONST_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
{
	LSA_UINT32 i;

	*bKillThread = LSA_FALSE;

	if(pPollIsr->uCntUsers == 0)
	{
		return EPS_PNDEV_RET_ERR; /* ??? Somethings wrong! */
	}

	for(i=0; i<EPS_CFG_PN_VDD_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			pPollIsr->pBoard[i] = LSA_NULL;

			pPollIsr->uCntUsers--;
			if(pPollIsr->uCntUsers == 0)
			{
				*bKillThread = LSA_TRUE;
			}

			return EPS_PNDEV_RET_OK; /* Board polling already enabled */
		}
	}

	return EPS_PNDEV_RET_ERR; /* Board not found */
}

/**
 * 
 * @see eps_pn_vdd_poll_isr_thread
 * 
 * @param uParam
 * @param arg
 * @return
 */
static LSA_VOID eps_pn_vdd_poll_isr_thread(LSA_UINT32 uParam, void *arg)
{
	/* Enter polling loop */

    LSA_BOOL bActivity = eps_pn_vdd_file_transfer();

    if (bActivity)
    {
    }

	LSA_UNUSED_ARG(uParam);
    LSA_UNUSED_ARG(arg);
}

/**
 * 
 * the thread function for watchdog functionalitiy eps_pn_vdd_wd_isr_thread
 * 
 * @param uParam
 * @param arg
 * @return
 */
static LSA_VOID eps_pn_vdd_wd_isr_thread( LSA_UINT32 uParam, void * arg )
{
    LSA_UINT32 bAlive          = LSA_FALSE;
    LSA_UINT32 uTimeOutCounter = 0;

    LSA_UNUSED_ARG(uParam);
    LSA_UNUSED_ARG(arg);

    //enter polling loop

    (void)PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    //check Alive flag
    eps_pn_vdd_enter();

	//check if memory is corrupt
	EPS_ASSERT((g_vdd_vals.pShmHeaderShared->Watchdog.bWdEnabled == LSA_TRUE) || (g_vdd_vals.pShmHeaderShared->Watchdog.bWdEnabled == LSA_FALSE));

    if (g_vdd_vals.pShmHeaderShared->Watchdog.bWdEnabled == LSA_TRUE)
    {
        bAlive = g_vdd_vals.pShmHeaderShared->Watchdog.bAlive;

        if (g_vdd_vals.pShmHeaderShared->bFatal) // check ADN if fatal occured
        {
            //read fatal dump from subordinate instance

            EPS_SHM_FATAL_STORE_PTR_TYPE const pFatalStore = (EPS_SHM_FATAL_STORE_PTR_TYPE)((LSA_UINT32)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uFatalOffset);

		    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "EPS VDD-Watchdog: eps_pn_vdd_wd_isr_thread(): FW-FATAL occured !!!");

            switch (pFatalStore->exit_code_org)
            {
                case EPS_EXIT_CODE_LSA_FATAL: //caller = PSI_FATAL_ERROR()
                {
                    LSA_CHAR             * const comp          = &(pFatalStore->sComp[0]);
                    LSA_UINT16             const length        = (LSA_UINT16)(pFatalStore->uLsaFatalLen);
                    LSA_FATAL_ERROR_TYPE * const lsa_fatal_ptr = (LSA_FATAL_ERROR_TYPE*)&(pFatalStore->lsa_fatal);

                    //set the error_data_ptr to lsa_fatal_error_data[x] array
                    lsa_fatal_ptr->error_data_ptr = &(pFatalStore->lsa_fatal_error_data[0]);

                    if //EPS lsa_fatal_error_data buffer not sufficient?
                       (pFatalStore->lsa_fatal.error_data_length > EPS_ERROR_DATA_BUF_LENGTH)
                    {
		                EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_vdd_wd_isr_thread(): EPS lsa_fatal_error_data buffer not sufficient! SHM_error_data_length(%u) EPS_ERROR_DATA_BUF_LENGTH(%u)", pFatalStore->lsa_fatal.error_data_length, EPS_ERROR_DATA_BUF_LENGTH);
                    }

                    EPS_FATAL_WITH_REASON(pFatalStore->exit_code_org, EPS_EXIT_CODE_ADVANCED_FATAL, comp, length, lsa_fatal_ptr, 0, 0, 0, 0);
                    break;
                }
                default: //all other exit_codes/callers
                {
			        LSA_CHAR * const file = &(pFatalStore->sFile[0]);
			        LSA_INT    const line = (LSA_INT)pFatalStore->uLine;
			        LSA_CHAR * const func = &(pFatalStore->sFunc[0]);
			        LSA_CHAR * const str  = &(pFatalStore->sMsg[0]);

                    EPS_FATAL_WITH_REASON(pFatalStore->exit_code_org, EPS_EXIT_CODE_ADVANCED_FATAL, 0, 0, 0, file, line, func, str);
                }
            }
        }

        if (bAlive == LSA_FALSE) // not alive?
        {
            //increase TimeOutCounter
            g_vdd_vals.pShmHeaderShared->Watchdog.uTimeOutCounter++;
        }
        else
        {
            //reset Alive-Flag and TimeOutCounter
            g_vdd_vals.pShmHeaderShared->Watchdog.bAlive          = LSA_FALSE;
            g_vdd_vals.pShmHeaderShared->Watchdog.uTimeOutCounter = 0;
        }

        uTimeOutCounter = g_vdd_vals.pShmHeaderShared->Watchdog.uTimeOutCounter;

        if (uTimeOutCounter > EPS_PN_VDD_WD_MAX_TIMEOUTCOUNTER)
        {
            EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_ADVANCED_FATAL, EPS_EXIT_CODE_ADVANCED_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "EPS VDD-Watchdog: eps_pn_vdd_wd_isr_thread(): TimeOut occured - no Alive-Signal detected");
        }

        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-Watchdog (upper): eps_pn_vdd_wd_isr_thread(): bAlive(%u) TimeOutCounter(%u)", bAlive, uTimeOutCounter);
    }

    eps_pn_vdd_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
}

/**
 * 
 * @see eps_pn_vdd_create_poll_thread
 * 
 * @param cName
 * @param pPollIsr
 * @return
 */
static LSA_UINT16 eps_pn_vdd_create_poll_thread( LSA_CHAR const* cName, EPS_VDD_POLL_PTR_TYPE pPollIsr )
{
	pPollIsr->hThread = eps_tasks_start_poll_thread(cName, EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedFifo, 100 /*ms*/, eps_pn_vdd_poll_isr_thread, 0, (LSA_VOID*)pPollIsr, eRUN_ON_1ST_CORE ); 
	EPS_ASSERT(pPollIsr->hThread != 0);

	pPollIsr->bRunning = LSA_TRUE;
    #ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_add ( pPollIsr->hThread, EPS_TGROUP_USER );
    #endif
	return EPS_PNDEV_RET_OK;
}

/**
 * 
 * creates the thread for watchdog functionality eps_pn_vdd_create_wd_thread
 * 
 * @param cName
 * @param pPollIsr
 * @return
 */
static LSA_VOID eps_pn_vdd_create_wd_thread( LSA_CHAR const* cName )
{
    g_vdd_vals.pShmHeaderShared->Watchdog.hWdThread = eps_tasks_start_poll_thread(cName, EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedFifo, EPS_PN_VDD_WD_ALIVE_POLL_TIME_MS, eps_pn_vdd_wd_isr_thread, 0, (LSA_VOID*)LSA_NULL, eRUN_ON_1ST_CORE); 
	EPS_ASSERT(g_vdd_vals.pShmHeaderShared->Watchdog.hWdThread != 0);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "EPS VDD-Watchdog (upper): eps_pn_vdd_create_wd_thread() - WD Thread created");

    #ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_add( g_vdd_vals.pShmHeaderShared->Watchdog.hWdThread, EPS_TGROUP_USER );
    #endif
}

/**
 * 
 * @see eps_pn_vdd_kill_poll_thread
 * 
 * @param pPollIsr
 * @return
 */
static LSA_VOID eps_pn_vdd_kill_poll_thread( EPS_VDD_POLL_PTR_TYPE pPollIsr )
{
	pPollIsr->bRunning = LSA_FALSE;

    #ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_remove ( pPollIsr->hThread );
    #endif

	eps_tasks_stop_poll_thread(pPollIsr->hThread);

}

/**
 * 
 * stops the watchdog thread eps_pn_vdd_kill_wd_thread
 * 
 * @return
 */
static LSA_VOID eps_pn_vdd_kill_wd_thread( LSA_VOID )
{
    #ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_remove ( g_vdd_vals.pShmHeaderShared->Watchdog.hWdThread );
    #endif

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_vdd_kill_wd_thread(): EPS VDD-Watchdog (upper) - stop WD Thread, hWdThread=%d",
        g_vdd_vals.pShmHeaderShared->Watchdog.hWdThread);

    eps_tasks_stop_poll_thread(g_vdd_vals.pShmHeaderShared->Watchdog.hWdThread);
}

/**
 * 
 * @see eps_pn_vdd_create_pn_gathered_polling_thread
 * 
 * @param pBoard
 * @return
 */
static LSA_UINT16 eps_pn_vdd_create_pn_gathered_polling_thread( EPS_PN_VDD_BOARD_PTR_TYPE pBoard )
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_VDD_POLL_PTR_TYPE pPollIsr = &g_pEpsVdd->poll.PnGatheredIsr;

	retVal = eps_pn_vdd_register_board_for_polling(pPollIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread == LSA_TRUE)
	{
		retVal = eps_pn_vdd_create_poll_thread("EPS_VDD_GP_ISR", pPollIsr);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			(void) eps_pn_vdd_unregister_board_for_polling(pPollIsr, pBoard, &bCreateThread); // Note: Don't overwrite retVal here. The creation of the thread failed, thus we want to return EPS_PNDEV_RET_ERR here regardless what eps_pn_vdd_unregister_board_for_polling returns.
		}
	}

	return retVal;
}

/**
 * 
 * @see eps_pn_vdd_kill_pn_gathered_polling_thread
 * 
 * @param pBoard
 * @return
 */
static LSA_UINT16 eps_pn_vdd_kill_pn_gathered_polling_thread(EPS_PN_VDD_BOARD_CONST_PTR_TYPE pBoard)
{
	LSA_UINT16                 retVal;
	LSA_BOOL                   bKillThread;
	EPS_VDD_POLL_PTR_TYPE pPollIsr = &g_pEpsVdd->poll.PnGatheredIsr;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">> eps_pn_vdd_kill_pn_gathered_polling_thread()");

    retVal = eps_pn_vdd_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

	if(bKillThread == LSA_TRUE)
	{
		eps_pn_vdd_kill_poll_thread(pPollIsr);
	}

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< eps_pn_vdd_kill_pn_gathered_polling_thread(): retVal=%d", retVal);

    return retVal;
}

/**
 * 
 * @see eps_pn_vdd_enable_interrupt
 * 
 * @param pHwInstIn
 * @param pInterrupt
 * @param pCbf
 * @return
 */
static LSA_UINT16 eps_pn_vdd_enable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16                 result;
    EPS_PN_VDD_BOARD_PTR_TYPE  pBoard = (EPS_PN_VDD_BOARD_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pHwInstIn->hDevice);

    LSA_UNUSED_ARG(pCbf);
    LSA_UNUSED_ARG(pInterrupt);

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pn_vdd_enter();

	result = eps_pn_vdd_create_pn_gathered_polling_thread(pBoard);

	eps_pn_vdd_exit();

	return result;
}

/**
 * 
 * @see eps_pn_vdd_disable_interrupt
 * 
 * @param pHwInstIn
 * @param pInterrupt
 * @return
 */
static LSA_UINT16 eps_pn_vdd_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	LSA_UINT16                 result;
    EPS_PN_VDD_BOARD_PTR_TYPE  pBoard = (EPS_PN_VDD_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    LSA_UNUSED_ARG(pInterrupt);

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pn_vdd_enter();

	result = eps_pn_vdd_kill_pn_gathered_polling_thread(pBoard);

	eps_pn_vdd_exit();

	return result;
}

/**
 * 
 * @see eps_pn_vdd_read_trace_data
 * 
 * @param pHwInstIn
 * @param offset
 * @param ptr
 * @param size
 * @return
 */
static LSA_UINT16 eps_pn_vdd_read_trace_data( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8 * ptr, LSA_UINT32 size )
{	
	LSA_UINT8 * pBase;
    
    LSA_UNUSED_ARG(pHwInstIn);

    (void)PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    eps_pn_vdd_enter();
    
    // note: the PNTRC memory blocks must be read indirect
	pBase = (LSA_UINT8*)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uTrace1Offset + offset;
	eps_memcpy(ptr, pBase, size);

    eps_pn_vdd_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129

	return EPS_PNDEV_RET_OK;
}

/**
 * 
 * @see eps_pn_vdd_write_trace_data
 * 
 * @param pHwInstIn
 * @param ptr
 * @param offset
 * @param size
 * @return
 */
static LSA_UINT16 eps_pn_vdd_write_trace_data( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size )
{	
	LSA_UINT8 * pBase;

    LSA_UNUSED_ARG(pHwInstIn);

    (void)PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    eps_pn_vdd_enter();

	pBase = (LSA_UINT8*)g_vdd_vals.pShmHeaderShared + g_vdd_vals.pShmHeaderShared->uTrace1Offset + offset;
	eps_memcpy(pBase, ptr, size);

    eps_pn_vdd_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129

	return EPS_PNDEV_RET_OK;
}

#define HELPER_GET_HIGH_VALUE(_val) ((_val >> 32) & 0xFFFFFFFF)
#define HELPER_GET_LOW_VALUE(_val) (_val & 0xFFFFFFFF)

/**
 * This function reads the TSC counter from the PC using an intrinsic function. 
 * The advanced firmware also uses this TSC counter to generate it's local timestamp.
 *
 * Note: 
 * This value does not present the actual time stamp value from the other PC since we only read the TSC value. This value is scaled other factor (see pntrc_cfg.c::PNTRC_WRITE_LOCAL_SYNC_TIME)
 * The PNTRC converter calculates the value by dividing the local_scaler traced by the lower PNTRC instance.
 * 
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return
 */
static LSA_UINT16 eps_pn_vdd_write_sync_time_lower( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId )
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
	LSA_UINT64 i;
	LSA_UINT32 lower_timestamp_low, lower_timestamp_high;

	LSA_UNUSED_ARG(pHwInstIn); // one PC, processor, one TSC. Drop argument

	// Read the Time Stamp Counter of the CPU.
	i = __rdtsc() ; //  read TSC value 
	
	lower_timestamp_low = HELPER_GET_LOW_VALUE(i);
	lower_timestamp_high = HELPER_GET_HIGH_VALUE(i);

	EPS_SYSTEM_TRACE_SYNC_TIME_LOWER(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOWER lower_cpu_id=%ld lower_time=%ld %ld", lowerCpuId, lower_timestamp_low, lower_timestamp_high);

	return result;
}

/**
 * 
 * @see eps_pn_vdd_save_dump
 * 
 * @param pHwInstIn
 * @return
 */
static LSA_UINT16 eps_pn_vdd_save_dump( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    LSA_UNUSED_ARG(pHwInstIn);

    eps_pn_vdd_enter();

    eps_pn_vdd_exit();

    return EPS_PNDEV_RET_OK;
}

/**
 * 
 * @see eps_pn_vdd_alloc_board
 * 
 * @param LSA_VOID
 * @return EPS_PN_VDD_BOARD_PTR_TYPE
 */
static EPS_PN_VDD_BOARD_PTR_TYPE eps_pn_vdd_alloc_board(LSA_VOID)
{
	LSA_UINT32 ctr;

	for (ctr=0; ctr<EPS_CFG_PN_VDD_MAX_BOARDS; ctr++)
	{
	    EPS_PN_VDD_BOARD_PTR_TYPE	const pBoard = &g_pEpsVdd->pBoard[ctr];

		if //free board entry available?
		   (!pBoard->bUsed)
		{
			pBoard->sHw.hDevice            = (LSA_VOID*)pBoard;
			pBoard->sHw.EnableIsr          = eps_pn_vdd_enable_interrupt;
			pBoard->sHw.DisableIsr         = eps_pn_vdd_disable_interrupt;
			pBoard->sHw.EnableHwIr         = eps_pn_vdd_enable_hw_interrupt;
            pBoard->sHw.DisableHwIr        = eps_pn_vdd_disable_hw_interrupt;
            pBoard->sHw.ReadHwIr           = eps_pn_vdd_read_hw_interrupt;
			pBoard->sHw.ReadTraceData      = eps_pn_vdd_read_trace_data;
			pBoard->sHw.WriteTraceData     = eps_pn_vdd_write_trace_data;
			pBoard->sHw.WriteSyncTimeLower = eps_pn_vdd_write_sync_time_lower;
            pBoard->sHw.SaveDump       	   = eps_pn_vdd_save_dump;
			pBoard->pBackRef               = g_pEpsVdd;
			pBoard->bUsed                  = LSA_TRUE;

			return pBoard;
		}
	}

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_vdd_alloc_board: no free board entry available");

	return LSA_NULL;
}

/**
 * 
 * @see eps_pn_vdd_free_board
 * 
 * @param EPS_PN_VDD_BOARD_PTR_TYPE pBoard
 * @return
 */
static LSA_VOID eps_pn_vdd_free_board(EPS_PN_VDD_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT32 i;

    // look for given pBoard in g_pEpsVdd->pBoard[x] and delete this entry
    for ( i=0; i < EPS_CFG_PN_VDD_MAX_BOARDS; i++ )
	{
        if (   (g_pEpsVdd->pBoard[i].bUsed)
            && (g_pEpsVdd->pBoard[i].sHw.hDevice == ((LSA_VOID*)pBoard)) )
        {
			g_pEpsVdd->pBoard[i].sHw.hDevice               = 0;
			g_pEpsVdd->pBoard[i].sHw.EnableIsr             = 0;
			g_pEpsVdd->pBoard[i].sHw.DisableIsr            = 0;
			g_pEpsVdd->pBoard[i].sHw.EnableHwIr            = 0;
            g_pEpsVdd->pBoard[i].sHw.DisableHwIr           = 0;
            g_pEpsVdd->pBoard[i].sHw.ReadHwIr              = 0;
			g_pEpsVdd->pBoard[i].sHw.ReadTraceData         = 0;
			g_pEpsVdd->pBoard[i].sHw.WriteTraceData        = 0;
			g_pEpsVdd->pBoard[i].sHw.WriteSyncTimeLower    = 0;
            g_pEpsVdd->pBoard[i].sHw.SaveDump              = 0;
			g_pEpsVdd->pBoard[i].pBackRef                  = 0;
			g_pEpsVdd->pBoard[i].bUsed                     = LSA_FALSE;

            return;
        }
    }

    EPS_FATAL("EPS VDD free board: the given pBoard not found");
    //lint --e(818) Pointer parameter 'pBoard' could be declared as pointing to const. Contents of it are changed in this function.
}

/**
 * 
 * @see eps_pn_vdd_file_transfer
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_BOOL eps_pn_vdd_file_transfer(LSA_VOID)
{
	LSA_BOOL bActivity = LSA_FALSE;
    //static LSA_BOOL bFileAllocated = LSA_FALSE;
    //static EPS_APP_FILE_TAG_TYPE file_tag = {0};
	EPS_SHM_FILE_TRANSFER_PTR_TYPE pFileTransfer;
    LSA_UINT8*     pFileData = LSA_NULL;    

	EPS_ASSERT ( g_vdd_vals.pShmHeaderShared != 0 );
	EPS_ASSERT(g_pEpsVdd != 0);
	
    (void)PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    eps_pn_vdd_enter();

    pFileTransfer = (EPS_SHM_FILE_TRANSFER_PTR_TYPE)&g_vdd_vals.pShmHeaderShared->HostFileTransfer;
    
	//lint --e(774) Boolean within 'if' always evaluates to True. SHM header may be overwritten
	if ( pFileTransfer != 0 )
    {
        bActivity = (LSA_BOOL)pFileTransfer->req.bReqStart;

        if (     (g_pEpsVdd->bFileAllocated == LSA_TRUE) 
              && (pFileTransfer->rsp.bTransferDone == 1)
            )
        {
			EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_pn_vdd_file_transfer() - Firmware download finished");

            EPS_APP_FREE_FILE((EPS_APP_FILE_TAG_TYPE*)&g_pEpsVdd->file_tag); 
            g_pEpsVdd->bFileAllocated = LSA_FALSE;
            pFileTransfer->rsp.bTransferDoneCnf = 1;
        }

		if( pFileTransfer->req.bReqStart == 1 )
		{
			pFileTransfer->rsp.uActPacket     = 0;
			pFileTransfer->rsp.bRspErr        = 0;
			pFileTransfer->rsp.uActPacketSize = 0;
			pFileTransfer->rsp.uFileSize      = 0;

            if (g_pEpsVdd->bFileAllocated == LSA_FALSE)
            {
				EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_pn_vdd_file_transfer() - Start firmware download");

				g_pEpsVdd->file_tag.pndevBoard          = (EPS_PNDEV_BOARD_TYPE) pFileTransfer->req.pndevBoard;
				g_pEpsVdd->file_tag.pndevBoardDetail    = (EPS_EDDP_BOARD_TYPE) pFileTransfer->req.pndevBoardDetail;
				g_pEpsVdd->file_tag.isHD                = (LSA_BOOL)pFileTransfer->req.isHD;
				g_pEpsVdd->file_tag.FileType            = (EPS_APP_FILE_TAG_ENUM) pFileTransfer->req.FileType;

                EPS_APP_ALLOC_FILE((EPS_APP_FILE_TAG_TYPE*)&g_pEpsVdd->file_tag); 
                g_pEpsVdd->bFileAllocated = LSA_TRUE;
            }

            if( g_pEpsVdd->file_tag.pFileData && g_pEpsVdd->file_tag.uFileSize > 0 )
			{
				LSA_UINT32 uOffset = pFileTransfer->req.uReqPacketRead * sizeof(pFileTransfer->rsp.uData);

				pFileTransfer->rsp.uFileSize = g_pEpsVdd->file_tag.uFileSize;

				if( uOffset < pFileTransfer->rsp.uFileSize )
				{
					pFileTransfer->rsp.uActPacketSize = pFileTransfer->rsp.uFileSize - uOffset; // for last packet n

					if( pFileTransfer->rsp.uActPacketSize > sizeof(pFileTransfer->rsp.uData) ) //correct for chunks 0..n-1
					{
						pFileTransfer->rsp.uActPacketSize = sizeof(pFileTransfer->rsp.uData);
					}

                    pFileData = g_pEpsVdd->file_tag.pFileData + (long)uOffset;
                    eps_memcpy(pFileTransfer->rsp.uData, pFileData, pFileTransfer->rsp.uActPacketSize);
					pFileTransfer->rsp.uActPacket = pFileTransfer->req.uReqPacketRead;
				}
				else
				{
					pFileTransfer->rsp.bRspErr = 1; // Tried to read beyond file
				}
			}
			else
			{
				pFileTransfer->rsp.bRspErr = 1;
			}

			pFileTransfer->req.bReqStart = 0;
			pFileTransfer->rsp.bRspDone  = 1;
		}
	}

    eps_pn_vdd_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129

    return bActivity;
}

/**
 * Enable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_vdd_enable_hw_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Enable HW Interrupt in vdd not supported in this driver."); 
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_vdd_disable_hw_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Disable HW Interrupt in vdd not supported in this driver."); 
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16  eps_pn_vdd_read_hw_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 * interrupts )
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("Read HW Interrupt in vdd not supported in this driver."); 
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}
