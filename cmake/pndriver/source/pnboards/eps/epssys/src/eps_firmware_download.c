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
/*  F i l e               &F: eps_firmware_download.c                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS System Startup + Error Handling                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20066
#define EPS_MODULE_ID      20066

#include "eps_sys.h"
#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"
#include "eps_trc.h"
#include "eps_firmware_download.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/**
 * Download a firmware for the given configuration. 
 * This function is synchronous. This function is not thread safe (do not call in from multiple threads).
 * 
 * @see EPS_FIRMWARE_DOWNLOAD_CONST_PTR_TYPE - input parameter description
 * 
 * @param [in]  pFwDownloadConfig	 ptr to firmware download configuration
 * @return 	EPS_RET_ERR        an error occured. Please check the traces
 * @return  EPS_RET_OK         firmware downloaded successfully
 */
LSA_UINT16 eps_firmware_download_and_start(EPS_FIRMWARE_DOWNLOAD_PTR_TYPE pFwDownloadConfig)
{
    EPS_PNDEV_HW_PTR_TYPE 	   pBoard;
	EPS_PNDEV_LOCATION_TYPE    Location;
	LSA_UINT16                 retVal;

    /// Check the input parameter
    if(pFwDownloadConfig->hdId > PSI_CFG_MAX_IF_CNT)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_download_and_start hd id invalid! hdId = %d", pFwDownloadConfig->hdId);
        return EPS_RET_ERR;
    }

    if(pFwDownloadConfig->uLocation.eLocationSelector != EPS_PNDEV_LOCATION_PCI)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_download_and_start: LocationSelector not supported");
        return EPS_RET_ERR;
    }

    // Firmware download is always true
    if(!pFwDownloadConfig->uOption.bLoadFw)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_download_and_start: bLoadFw == LSA_FALSE. Didn't you want to download the user firmware?");
        return EPS_RET_ERR;
    }

    if(!pFwDownloadConfig->uOption.bUseEpsAppToGetFile)
    {
        if(     (pFwDownloadConfig->uOption.pFwBufData      == LSA_NULL)
            ||  (pFwDownloadConfig->uOption.pBootFwBufData  == LSA_NULL)
            ||  (pFwDownloadConfig->uOption.uFwBufSize      == 0)
            ||  (pFwDownloadConfig->uOption.uBootFwBufSize  == 0)
            )
        {
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_download_and_start: Invalid firmware / boot fw config.");
            return EPS_RET_ERR;
        }
    }
    else
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_firmware_download_and_start: Getting firmware buffer with EPS APP API later.");
    }

    /// Check if the board is already opened
    retVal = eps_pndev_if_get(pFwDownloadConfig->hdId, &pBoard);
    if (retVal == EPS_PNDEV_RET_OK)
    {
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_download_and_start: Error - already connected");
		return EPS_RET_ERR;
    }
	
    Location.eLocationSelector  = pFwDownloadConfig->uLocation.eLocationSelector;
    Location.uBusNr             = pFwDownloadConfig->uLocation.uBusNr;
    Location.uDeviceNr          = pFwDownloadConfig->uLocation.uDeviceNr;
	Location.uFctNr             = pFwDownloadConfig->uLocation.uFctNr;

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_firmware_download_and_start: PCI location = (%d,%d,%d)", Location.uBusNr, Location.uDeviceNr, Location.uFctNr);
    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_firmware_download_and_start: Debug options = bDebug=%d, bShmDebug=%d, bShmHaltOnStartup=%d", pFwDownloadConfig->uOption.bDebug, pFwDownloadConfig->uOption.bShmDebug, pFwDownloadConfig->uOption.bShmHaltOnStartup);

    /// Download the firmware
	retVal = eps_pndev_if_open(&Location, &(pFwDownloadConfig->uOption), &pBoard, pFwDownloadConfig->hdId);
    if (EPS_PNDEV_RET_OK == retVal)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_firmware_download_and_start - eps_pndev_open returned with EPS_PNDEV_RET_OK");
    }
    else
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_download_and_start - FW download failed. Please check the log");
        return EPS_RET_ERR;
    }

    return EPS_RET_OK;
}

/**
 * Stops the firmware, collects all traces using EPS mechanism.
 * This function is synchronous. This function is not thread safe (do not call in from multiple threads).
 * 
 * @see EPS_FIRMWARE_DOWNLOAD_CONST_PTR_TYPE - input parameter description
 * 
 * @param [in]  pFwDownloadConfig	 ptr to firmware download configuration
 * @return 	EPS_RET_ERR        an error occured. Please check the traces
 * @return  EPS_RET_OK         firmware downloaded successfully
 */
LSA_UINT16 eps_firmware_stop(EPS_FIRMWARE_DOWNLOAD_CONST_PTR_TYPE pFwDownloadConfig)
{
    EPS_PNDEV_HW_PTR_TYPE pBoard;
    
    if(EPS_PNDEV_RET_OK !=eps_pndev_if_get(pFwDownloadConfig->hdId, &pBoard))
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_stop: Error - Board is not in use!");
        return EPS_RET_ERR;
    }

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_firmware_stop: Stopping firmware for for hd=%d", pFwDownloadConfig->hdId);

    if(EPS_PNDEV_RET_OK != eps_pndev_if_close(pFwDownloadConfig->hdId))
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_firmware_stop: Stopping firmware failed. Please check the log");
        return EPS_RET_ERR;
    }

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_firmware_stop: Firmware stopped for hd=%d", pFwDownloadConfig->hdId);
    return EPS_RET_OK;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
