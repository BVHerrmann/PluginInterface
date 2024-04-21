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
/*  F i l e               &F: eps_pn_drv_if.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN Device Driver Interface                                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20024
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>       /* Types / Prototypes / Fucns */
#include <eps_trc.h>       /* Tracing                    */
#include <eps_cp_hw.h>     /* EPS PSI HW adaption layer  */
#include <eps_pn_drv_if.h> /* PN-Driver Interface        */

//----------------------------------------------------------------------------------------------------

#define EPS_CFG_PN_DRV_IF_MAX               2   // eg. windows and vdd
#define EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO    20  // max boards in one PC

typedef struct eps_pn_drv_if_store_tag
{
	struct
	{
		LSA_BOOL            bUsed;
		EPS_PNDEV_IF_TYPE   g_EpsPnDevIf;
	} sInterface[EPS_CFG_PN_DRV_IF_MAX];
	struct
	{
		EPS_PNDEV_HW_PTR_TYPE	pHw;
		EPS_PNDEV_IF_PTR_TYPE	pIf;
	} sHw[PSI_CFG_MAX_IF_CNT+1];
	struct
	{
		LSA_BOOL                    bUsed;
		EPS_PNDEV_IF_DEV_INFO_TYPE  sDevInfo;
	} sRegisteredDevInfoStore[EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO];
} EPS_PN_DRV_IF_STORE_TYPE, *EPS_PN_DRV_IF_STORE_PTR_TYPE;

//----------------------------------------------------------------------------------------------------

/* Global definition */
static EPS_PN_DRV_IF_STORE_TYPE     g_EpsPnDevIfStore;
static EPS_PN_DRV_IF_STORE_PTR_TYPE g_pEpsPnDevIfStore = LSA_NULL;

//----------------------------------------------------------------------------------------------------

/**
 * Initializes the PN Dev IF store by setting all values to 0.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pndev_if_init(LSA_VOID)
{
	eps_memset(&g_EpsPnDevIfStore, 0, sizeof(g_EpsPnDevIfStore));

	g_pEpsPnDevIfStore = &g_EpsPnDevIfStore;

    g_pEpsData->bEpsDevIfInitialized = LSA_TRUE;
}

/**
 * revert the initialization of pndevdrv interfaces
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndev_if_undo_init(LSA_VOID)
{
	LSA_UINT16  i;

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);

	for ( i=0; i < (PSI_CFG_MAX_IF_CNT+1); i++ )
	{
		if( (g_pEpsPnDevIfStore->sHw[i].pHw != LSA_NULL) ||
			(g_pEpsPnDevIfStore->sHw[i].pIf != LSA_NULL) )
		{
			EPS_FATAL("All boards have to be closed by undo initialization of pndevdrv"); /* not all boards closed */
		}
	}

    eps_memset(&g_EpsPnDevIfStore, 0, sizeof(g_EpsPnDevIfStore));
	g_pEpsPnDevIfStore = LSA_NULL;

    g_pEpsData->bEpsDevIfInitialized = LSA_FALSE;
}

/**
* Uninstall pndevdrv interfaces from application    -> call the pndevdrv-uninstall
*
* @param	LSA_VOID
* @return 	LSA_VOID
*/
LSA_VOID eps_pndev_if_uninstall_from_app(LSA_VOID)
{
    LSA_UINT16  i;

    EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);

    for ( i = 0; i < EPS_CFG_PN_DRV_IF_MAX; i++ )
    {
        if (g_pEpsPnDevIfStore->sInterface[i].bUsed)
        {
            EPS_ASSERT(g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf.uninstall != LSA_NULL);

            g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf.uninstall();
            g_pEpsPnDevIfStore->sInterface[i].bUsed = LSA_FALSE;
        }
    }

    // Unregister all PNBoards from the devices storage which were registred with eps_pndev_if_register_device()
    for ( i = 0; i < EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO; i++ )
    {
        if (g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed)
        {
            eps_memset( &g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo, 0, sizeof(EPS_PNDEV_IF_DEV_INFO_TYPE) );
            g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed = LSA_FALSE;
        }
    }
}

/**
 * This function registers a PN Dev implementation into the EPS PN Dev IF.
 * 
 * Each PN Dev implementation calls this function in the install function.
 * Checks if all the function pointer are linked to actual functions by checking for NULL pointers.
 * 
 * @param [in] pPnDevIf    - structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @return LSA_VOID
 */
LSA_VOID eps_pndev_if_register(EPS_PNDEV_IF_CONST_PTR_TYPE pPnDevIf)
{
	LSA_UINT32 i;

	EPS_ASSERT(pPnDevIf != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);

	EPS_ASSERT(pPnDevIf->close != LSA_NULL);
	EPS_ASSERT(pPnDevIf->open != LSA_NULL);
	EPS_ASSERT(pPnDevIf->uninstall != LSA_NULL);

	for(i=0; i<EPS_CFG_PN_DRV_IF_MAX; i++)
	{
		if(!g_pEpsPnDevIfStore->sInterface[i].bUsed)
		{
			g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf = *pPnDevIf;
			g_pEpsPnDevIfStore->sInterface[i].bUsed = LSA_TRUE;
			return;
		}
	}

	EPS_FATAL("Not enough free interfaces to register a new one"); /* If Overflow */
}

/**
 * Registers a PNBoard into the registred devices storage.
 * 
 * @see eps_pndev_if_get_registered_devices - returns all the registred devices.
 * 
 * @param [in] pDev                         - info about the PCI Location / MAC address 
 * @return LSA_VOID
 */
LSA_VOID eps_pndev_if_register_device(EPS_PNDEV_IF_DEV_INFO_CONST_PTR_TYPE pDev)
{
	LSA_UINT32 i;
	LSA_BOOL bSuccess = LSA_FALSE;

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);	

	for(i=0; i<EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO; i++)
	{
		if(!g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed)
		{
			g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo = *pDev;

			g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed = LSA_TRUE;
			bSuccess = LSA_TRUE;
			break;
		}
	}

	EPS_ASSERT(bSuccess == LSA_TRUE);
}

/**
 * Returns a structure will all supported PNBoards. 
 * 
 * Currently eps_imcea only. T
 * 
 * UseCase: The LD Lower at Advanced uses this function to store all supported PNBoards into the shared memory.
 * @see eps_pndev_if_get_registered_devices - calls this function
 * @param [out] pDev                        - out structure with infos about the supported PN Boards
 * @param [in] uMaxDevices                  - max. number of devices that the caller expects.
 * @param [out] pCntDevices                 - actual number of devices. Will be smaller or equal to uMaxDevices
 * @return
 */
LSA_VOID eps_pndev_if_get_registered_devices(EPS_PNDEV_IF_DEV_INFO_PTR_TYPE pDev, LSA_UINT16 uMaxDevices, LSA_UINT16* pCntDevices)
{
	LSA_UINT16 i=0,e = 0;

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);
	EPS_ASSERT(pDev != LSA_NULL);
	EPS_ASSERT(pCntDevices != LSA_NULL);

	if(uMaxDevices  > EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO)
	{
		e = EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO;
	}
	else
	{
		e = uMaxDevices;
	}

	while(i<e)
	{
		if(g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed)
		{
			pDev[i].uPciAddrValid = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciAddrValid;
			pDev[i].uPciBusNr     = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciBusNr;
			pDev[i].uPciDeviceNr  = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciDeviceNr;
			pDev[i].uPciFuncNr    = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciFuncNr;

			pDev[i].uMacAddrValid = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddrValid;
			pDev[i].uMacAddr[0]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[0];
			pDev[i].uMacAddr[1]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[1];
			pDev[i].uMacAddr[2]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[2];
			pDev[i].uMacAddr[3]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[3];
			pDev[i].uMacAddr[4]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[4];
			pDev[i].uMacAddr[5]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[5];

			pDev[i].uEddType     = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uEddType;

			i++;
		}
		else
		{
			break;
		}
	}

	*pCntDevices = i;
}

/**
 * This function searches in all installed PN Dev Instances for a PN Dev implementation that supports the given PNBoard.
 * This function is used
 *    - by the EPS LD upper that wants to download the LD+HD firmware
 *    - by the EPS HD upper that wants to download the HD firmware
 *    - by the HD to get connection to the read hardware (Basic, Advanced 2T)
 *    
 * Note that the first driver that finds a board on the given location will be used. This may be the case if two drivers support the board (eps_pndevdrv, eps_stdmacdrv).
 *    
 * The information about the PNBoard are stored in ppHwInstOut. This structure also contains function pointers to access the board.
 * 
 * @see eps_hw_connect                  - calls this function 
 * @see PSI_GET_HD_PARAM                - calls eps_hw_connect. UseCase: Light, Basic FW (LD+HD, HD part), Advanced 2T (LD+HD, HD part)
 * @see PSI_HIF_HD_U_GET_PARAMS         - calls eps_hw_connect. UseCase: Light + HD FW (HD Upper). Calls it with hd_nr = 1...4
 * @see PSI_HIF_LD_U_GET_PARAMS         - calls eps_hw_connect. UseCase: Basic and Advanced (LD Upper, at application level). Calls it with hd_nr = 0 
 * 
 * @param [in] pLocation                - location descriptor (PCI Location or MAC Address)
 * @param [in] pOption                  - options (debug, download of firmware). Note that not all PN Dev implementations support this.
 * @param [in] ppHwInstOut              - out structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @param [in] hd_id                    - hd number. 0 = LD, 1...4 = HD instance.
 * @return #EPS_PNDEV_RET_DEV_NOT_FOUND - board was not found by any installed PN Dev Implementation
 * @return #EPS_PNDEV_RET_OK            - board was successfully openend
 */
LSA_UINT16 eps_pndev_if_open( EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id )
{
	LSA_UINT32            i;
	LSA_UINT16            retVal = EPS_PNDEV_RET_DEV_NOT_FOUND;
	EPS_PNDEV_IF_PTR_TYPE pIf    = LSA_NULL;

	EPS_ASSERT(pLocation != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);  /* Uninit? */
	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pHw == LSA_NULL);
	EPS_ASSERT(hd_id <= PSI_CFG_MAX_IF_CNT);  /* hd_id out of range */

	for (i=0; i<EPS_CFG_PN_DRV_IF_MAX; i++)
	{
	    // call open at all PN Dev implementations
		if (g_pEpsPnDevIfStore->sInterface[i].bUsed)
		{
			pIf = &g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf;

			EPS_ASSERT(pIf->open != LSA_NULL);

			retVal = pIf->open(pLocation, pOption, ppHwInstOut, hd_id);

			if (retVal == EPS_PNDEV_RET_OK)
			{
				g_pEpsPnDevIfStore->sHw[hd_id].pHw = *ppHwInstOut;
				g_pEpsPnDevIfStore->sHw[hd_id].pIf = pIf;
				// PN Dev implementation supports board => success. Even if other implementations support that board, they are not used.
				break;
			}
		}
	}

    if (retVal != EPS_PNDEV_RET_OK)
    {
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_DEVICE_NOT_FOUND, EPS_EXIT_CODE_DEVICE_NOT_FOUND, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndev_if_open() - no driver has found a board with the requested location.");
    }

	return retVal;
}

/**
 * Returns the descriptior of the PNBoard on the given hd_id.
 * Note that at eps_pndev_if_open, a hd_id and location descriptor is given to EPS PN Dev. 
 * EPS PN Dev stores the ppHwInstOut for the PNBoard with the hd_id provided there. If the same hd_id is provided here, the same 
 * ppHwInstOut will be returned.
 * 
 * @see eps_pndev_if_open       - opens the PNBoard. Also returns EPS_PNDEV_HW_PTR_PTR_TYPE. The usecases are also described there.
 * @see eps_hw_connect          - calls this function to see if the board was already openend. 
 * 
 * @see eps_eddi_exttimer_start   - calls this function (EDDI only)
 * @see eps_eddi_exttimer_stop    - calls this function (EDDI only)
 * @see eps_enable_pnio_event     - calls this function (interrupt mechanism)
 * @see eps_disable_pnio_event    - calls this function (interrupt mechanism)
 * @see eps_enable_iso_interrupt  - calls this function (isochronous mode, interrupt mechanism)
 * @see eps_disable_iso_interrupt - calls this function (isochronous mode, interrupt mechanism)
 * @param [in] hd_id              - 0 = LD , 1...4 = HD. 
 * @param [out] ppHwInstOut       - out structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @return
 */
LSA_UINT16 eps_pndev_if_get( LSA_UINT16 hd_id, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut )
{
	LSA_UINT16 retVal = EPS_PNDEV_RET_OK;

	EPS_ASSERT(ppHwInstOut != LSA_NULL);
	EPS_ASSERT(hd_id <= PSI_CFG_MAX_IF_CNT);     /* hd_id out of range */

	*ppHwInstOut = g_pEpsPnDevIfStore->sHw[hd_id].pHw;

	if (g_pEpsPnDevIfStore->sHw[hd_id].pHw == LSA_NULL)
	{
		retVal = EPS_PNDEV_RET_DEV_NOT_FOUND;
	}

	EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "<< eps_pndev_if_get(hd_id=%d): retVal=%d", hd_id, retVal );

    return retVal;
}

/**
 * Closes the PN Dev instance by callind the close function of the implementation.
 * @param hd_id               - 0 = LD, 1...4 = HD.
 * @return #EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndev_if_close( LSA_UINT16 hd_id )
{
	LSA_UINT16 retVal;

	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">> eps_pndev_if_close(hd_id=%d)", hd_id);

    EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pHw != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pIf != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pIf->close != LSA_NULL);

    // call eps_pn_imceadrv_close()
	retVal = g_pEpsPnDevIfStore->sHw[hd_id].pIf->close(g_pEpsPnDevIfStore->sHw[hd_id].pHw);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);

	g_pEpsPnDevIfStore->sHw[hd_id].pHw = LSA_NULL;
	g_pEpsPnDevIfStore->sHw[hd_id].pIf = LSA_NULL;

	EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< eps_pndev_if_close(hd_id=%d): retVal=%d", hd_id, retVal);

    return retVal;
}

/**
 * Closes the PN Dev instance in a fatal. Does not check for return code EPS_PNDEV_RET_OK.
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndev_if_fatal_close( LSA_VOID )
{
	LSA_UINT32 i;

	for( i = 0; i < (PSI_CFG_MAX_IF_CNT+1); i++ )
	{
		if(g_pEpsPnDevIfStore->sHw[i].pHw != LSA_NULL)
		{
			g_pEpsPnDevIfStore->sHw[i].pIf->close(g_pEpsPnDevIfStore->sHw[i].pHw);
			g_pEpsPnDevIfStore->sHw[i].pHw = LSA_NULL;
			g_pEpsPnDevIfStore->sHw[i].pIf = LSA_NULL;
		}
	}
}

/**
 * Called upon receiving a cyclic timer tick
 * - checks received time and traces overruns and underruns
 * - executes Callbackfunction when tick wasn't received too early
 * 
 * @param [in] pTmrPar          pointer to timer parameter
 * @return
 */
LSA_VOID eps_pndev_if_timer_tick (EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE pTmrPar)
{
	EPS_PNDEV_CALLBACK_PTR_TYPE     pCbf = pTmrPar->pTickCbf;
	EPS_ASSERT(pCbf != LSA_NULL);
	EPS_ASSERT(pCbf->pCbf != LSA_NULL);

	// count timer ticks
	pTmrPar->uTickCount++;

	{
	    LSA_UINT64  uStartTimeNs;
	    LSA_UINT64  uStopTimeNs;
	    LSA_UINT64  uDiffTimeNs;
	    
        // read and store current StartTime
        uStartTimeNs = eps_get_ticks_100ns();
		uDiffTimeNs = (uStartTimeNs - pTmrPar->uTickCbfLastCallTimeNs) * 100;

		if	(uDiffTimeNs > pTmrPar->uTickUnderrunMarkNs)
			// tick is not too early.
		{
			// call Cbf
			pCbf->pCbf(pCbf->uParam, pCbf->pArgs);
			// read and store current StopTime
			uStopTimeNs = eps_get_ticks_100ns();

			if	(uDiffTimeNs > pTmrPar->uTickOverrunMarkNs)
				// timer tick cycle overrun occured?
			{
				pTmrPar->uTickOverrunCount++;
				EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_WARN, "eps_pndev_if_timer_tick(pTickCbf=0x%x): Tick Overrun %u (evt %u) Cbf was called after 0%u ns", 
                    pTmrPar->pTickCbf, pTmrPar->uTickOverrunCount, pTmrPar->uTickCount, (LSA_UINT32)uDiffTimeNs);

                // store current counts to check below if this will be a follow-up call of last overrun
                pTmrPar->uTickCountOnLastOverrun        = pTmrPar->uTickCount;
                pTmrPar->uTickOverrunCountOnLastOverrun = pTmrPar->uTickOverrunCount;
            }

			// calculate duration of Cbf
			uDiffTimeNs = (uStopTimeNs - uStartTimeNs) * 100;

			if	(uDiffTimeNs >= pTmrPar->uTickCbfOverrunMarkNs)
				// Cbf lasted too much time?
			{
				pTmrPar->uTickCbfOverrunCount++;
				EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_WARN, "eps_pndev_if_timer_tick(pTickCbf=0x%x): Cbf Overrun %u (evt %u) Cbf lasted 0%u ns ", 
                    pTmrPar->pTickCbf, pTmrPar->uTickCbfOverrunCount, pTmrPar->uTickCount, (LSA_UINT32)uDiffTimeNs);
			}

			// save last time calling Cbf in timer pars
			pTmrPar->uTickCbfLastCallTimeNs = uStartTimeNs;
		}
		else
		{
			// tick is too ealry don't call Cbf!
			pTmrPar->uTickUnderrunCount++;

            // trace underrun only this is a really underrun and does NOT belong to last overrun
            {
                // detect the follow-up call of last overrun
                LSA_UINT32  uTickCountOnLastOverrun_Next = pTmrPar->uTickCountOnLastOverrun + 1;

                if ((pTmrPar->uTickOverrunCountOnLastOverrun == pTmrPar->uTickOverrunCount) && (uTickCountOnLastOverrun_Next == pTmrPar->uTickCount))
                {
                    // this underrun is a follow-up call of last overrun
                    // increment for the possible next follow-up call
                    pTmrPar->uTickCountOnLastOverrun++;
                }
                else
                {
                    // this is a really underrun
                    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_WARN, "eps_pndev_if_timer_tick(pTickCbf=0x%x): Tick Underrun %u (evt %u) To early to call Cbf 0%u ns", 
                        pTmrPar->pTickCbf, pTmrPar->uTickUnderrunCount, pTmrPar->uTickCount, (LSA_UINT32)uDiffTimeNs);

                    // reset for next overrun
                    pTmrPar->uTickCountOnLastOverrun        = 0;
                    pTmrPar->uTickOverrunCountOnLastOverrun = 0;
                }
            }
		}
	}
}

//----------------------------------------------------------------------------------------------------
