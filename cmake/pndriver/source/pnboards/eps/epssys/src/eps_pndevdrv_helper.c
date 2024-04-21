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
/*  F i l e               &F: eps_pndevdrv_helper.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Implementation - module for helper functions             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20067
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>           /* Types / Prototypes / Fucns */
#include <eps_rtos.h> 

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
/*-------------------------------------------------------------------------*/

#include <precomp.h>
#include <PnDev_Driver_Inc.h>       /* PnDevDrv Interface                       */

#include <eps_locks.h>              /* EPS Locks                                */
#include <eps_cp_hw.h>              /* EPS CP PSI adaption                      */
#include <eps_trc.h>                /* Tracing                                  */
#include <eps_pn_drv_if.h>          /* PN Device Driver Interface               */
#include <eps_pndevdrv.h>           /* PNDEV-Driver Interface implementation    */
#include <eps_shm_file.h>           /* EPS Shared Memory File Interface         */

#include "eps_pndevdrv_helper.h"


typedef struct eps_pndevdrv_helper_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit; /// Lock handle
    LSA_UINT32 uCntFoundDevices;
    uPNDEV_GET_DEVICE_INFO_OUT foundDevices[EPS_CFG_PNDEVDRV_MAX_BOARDS];
    uPNDEV_DRIVER_DESC *pDrvHandle;
} EPS_PNDEVDRV_HELPER_STORE_TYPE, *EPS_PNDEVDRV_HELPER_STORE_PTR_TYPE;

static EPS_PNDEVDRV_HELPER_STORE_TYPE g_EpsPnDevDrvHelper;
static EPS_PNDEVDRV_HELPER_STORE_PTR_TYPE g_pEpsPnDevDrvHelper = LSA_NULL;

/**
 * Initializes the global structure g_EpsPnDevDrvHelper and assigns the pointer g_pEpsPnDevDrvHelper.
 * Note: Do not access g_EpsPnDevDrvHelper directly, only access g_pEpsPnDevDrvHelper or use the functions to access members of the structure.
 *
 * @see eps_pndevdrv_install - Calls this function
*/
LSA_VOID eps_pndevdrv_helper_init( uPNDEV_DRIVER_DESC *pDrvHandle )
{
    eps_memset(&g_EpsPnDevDrvHelper, 0, sizeof(g_EpsPnDevDrvHelper));

	g_pEpsPnDevDrvHelper = &g_EpsPnDevDrvHelper;
    g_pEpsPnDevDrvHelper->bInit = LSA_TRUE;
    g_pEpsPnDevDrvHelper->pDrvHandle = pDrvHandle;
}

/**
 * Deletes the reference of g_pEpsPnDevDrvHelper to g_EpsPnDevDrvHelper
 * @see eps_pndevdrv_uninstall - Calls this function
*/
LSA_VOID eps_pndevdrv_helper_undo_init( LSA_VOID )
{
	g_pEpsPnDevDrvHelper->bInit = LSA_FALSE;

    g_pEpsPnDevDrvHelper->pDrvHandle = LSA_NULL;

	g_pEpsPnDevDrvHelper = LSA_NULL;
}

/**
 * undo init critical section
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndevdrv_undo_init_critical_section( LSA_VOID )
{
    LSA_RESPONSE_TYPE retVal;
	retVal = eps_free_critical_section(g_pEpsPnDevDrvHelper->hEnterExit);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * init critical section
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndevdrv_init_critical_section( LSA_VOID )
{
    LSA_RESPONSE_TYPE retVal;
    retVal = eps_alloc_critical_section(&g_pEpsPnDevDrvHelper->hEnterExit, LSA_FALSE);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * enter critical section
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndevdrv_enter( LSA_VOID )
{
	eps_enter_critical_section(g_pEpsPnDevDrvHelper->hEnterExit);
}

/**
 * leave critical section
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndevdrv_exit( LSA_VOID )
{
	eps_exit_critical_section(g_pEpsPnDevDrvHelper->hEnterExit);
}

/**
 * convert string to integer
 * 
 * @param [in]    pStr			ptr to first character of string
 * @param [inout] pNextChar		ptr to next character of string
 * @param [in]    pbWCharStr	Width of every single string element	
 * @return converted integer
 */
static LSA_INT32 eps_pndevdrv_str_to_int( const LSA_CHAR* pStr, LSA_UINT32 *pNextChar, LSA_BOOL bWChar )
{
	LSA_INT32 i = 0, e, mul = 1, f = 1;
	LSA_INT32 nr = 0;

    if (bWChar)
    {
        f = 2;
    }

    if (pStr == LSA_NULL)
    {
        return -1; /* No String */
    }
	while ( (pStr[i] != 0) && ( (pStr[i] < '0') || (pStr[i] > '9') ) ) /* Find Start of Number*/
	{
		i+=f;
	}

    if (pStr[i] == 0)
    {
        return -1; /* End of String => No Number */
    }
	e = i;

	while ( (pStr[e] >= '0') && (pStr[e] <= '9') ) /* Find End of Number */
	{
		e+=f;
	}

    if (pNextChar != 0)
    {
        *pNextChar = (LSA_UINT32)e;
    }
	e-=f;

	while (e >= i) /* Convert String to int */
	{
		nr += (pStr[e] - 48) * mul;
		mul *= 10;
		e-=f;
	}

	return nr;
}

/**
 * parse pci location, which are included in input string. Returns the three values bus, device and function number.
 * 
 * @param [in]  pStr		ptr to first charackter of string
 * @param [out] pBusNr		ptr containing computed BusNr
 * @param [out] pDeviceNr	ptr containing computed DeviceNr	
 * @param [out] pFunctionNr	ptr containing computed FunctionNr
 * @return 
 */
static LSA_VOID eps_pndevdrv_parse_location_string( const LSA_CHAR* pStr, LSA_INT *pBusNr, LSA_INT *pDeviceNr, LSA_INT *pFunctionNr )
{
	LSA_UINT32 e = 0, ealt;
	LSA_BOOL bWChar = LSA_FALSE;

	EPS_ASSERT(pStr != LSA_NULL);
	EPS_ASSERT(pBusNr != LSA_NULL);
	EPS_ASSERT(pDeviceNr != LSA_NULL);
	EPS_ASSERT(pFunctionNr != LSA_NULL);

    if (pStr[1] == 0)
    {
        bWChar = LSA_TRUE;
    }

	/* Get Bus Nr */
	*pBusNr = eps_pndevdrv_str_to_int(pStr, &e, bWChar);
	EPS_ASSERT(*pBusNr >= 0);
	EPS_ASSERT(*pBusNr < 256);
	EPS_ASSERT(pStr[e] == ',');
	ealt = e;

	/* Get Device Nr */
	*pDeviceNr = eps_pndevdrv_str_to_int(&pStr[ealt], &e, bWChar);
	ealt += e;
	EPS_ASSERT(*pDeviceNr >= 0);
	EPS_ASSERT(pStr[ealt] == ',');

	/* Get Function Nr */
	*pFunctionNr = eps_pndevdrv_str_to_int(&pStr[ealt], &e, bWChar);
	ealt += e;
	EPS_ASSERT(*pFunctionNr >= 0);
	EPS_ASSERT(pStr[ealt] == 0);
}

#ifdef EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
/**
 * print a list of boards, found by the PnDevDriver
 * 
 * @param LSA_VOID
 * @return 
 */
LSA_VOID eps_pndevdrv_printf_found_boards( LSA_VOID )
{
	LSA_UINT32 i;
	LSA_INT uBusNr, uDeviceNr, uFctNr;
	uPNDEV_GET_DEVICE_INFO_OUT* pDevice;

	printf("\r\nEPS PnDevDrv Found PN Boards:\r\n");
	printf("----------------------------------------");

	for (i=0; i<g_pEpsPnDevDrvHelper->uCntFoundDevices; i++)
	{
		pDevice = &g_pEpsPnDevDrvHelper->foundDevices[i];

		//printf("\r\n%s  -  ",(LSA_CHAR*)pDevice->sPciLoc); /* Commented because of wchar problem */
		eps_pndevdrv_parse_location_string((LSA_CHAR*)pDevice->sPciLoc, &uBusNr, &uDeviceNr, &uFctNr);
		printf("\r\nBus %2d, Dev %2d, Fct %2d  -  ", uBusNr, uDeviceNr, uFctNr);

		switch (pDevice->eBoard)
		{
			case ePNDEV_BOARD_INVALID:
				printf("Invalid Board              ");
				break;
			case ePNDEV_BOARD_CP1616:					// CP1616 board
				printf("CP1616 Ertec400            ");
				break;
			case ePNDEV_BOARD_DB_EB400_PCIE:			// DB-EB400-PCIe board
				printf("EB400_PCIe Ertec400        ");
				break;
			case ePNDEV_BOARD_EB200:					// EB200 board
			case ePNDEV_BOARD_DB_EB200_PCIE:            // EB200-PCIe board
				printf("EB200 Ertec200             ");
				break;
			case ePNDEV_BOARD_DB_SOC1_PCI:    			// Soc1 PCI Dev board
				printf("Soc1 PCI Dev Board Rev7    ");
				break;
			case ePNDEV_BOARD_DB_SOC1_PCIE:    			// Soc1 PCIe Dev board
				printf("Soc1 PCIe Dev Board Rev7   ");
				break;
			case ePNDEV_BOARD_CP1625:    				// CP1625 Soc1 PCIe Dev board
				printf("CP1625 - Soc1 PCIe Dev Board Rev7   ");
				break;
			case ePNDEV_BOARD_FPGA1_ERTEC200P:		    // FPGA1-Ertec200P board
				printf("FPGA Ertec200P Rev1        ");
				break;
			case ePNDEV_BOARD_EB200P:		            // EB200P board
				printf("EB200P Ertec200P Rev1      ");
				break;
			case ePNDEV_BOARD_I210:		          		// I210 board
				printf("SPVL I210				   ");
				break;
			case ePNDEV_BOARD_I210SFP:                  // I210 SFP board
				printf("SPVL I210 SFP              ");
				break;
            case ePNDEV_BOARD_I82574:                   // I82574 board
                printf("Hartwell I82574            ");
                break;
            case ePNDEV_BOARD_MICREL_KSZ8841:           // Micrel KSZ8841 board
                printf("Micrel KSZ8841             ");
                break;                
            case ePNDEV_BOARD_MICREL_KSZ8842:           // Micrel KSZ8842 board
                printf("Micrel KSZ8842             ");
                break;
			case ePNDEV_BOARD_TI_AM5728:		        // TI AM5728 board
				printf("TI AM5728				   ");
				break;
			case ePNDEV_BOARD_FPGA1_HERA:		        // FPGA-Hera board
				printf("FPGA Hera                  ");	
				break;
            case ePNDEV_BOARD_IX1000:           		// IOT2000 board
                printf("Intel Quark IOT2000		   ");
                break;
			default:
				printf("EPS Unknown Board          ");
				break;
		}
	}
	printf("\r\n----------------------------------------\r\n");
}
#endif

#ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
/**
 * print information, once a board has been closed or opened
 * 
 * @param bOpen		True, if it was opened, FALSE if it was closed
 * @param pBoard	ptr to current board structure
 * @return 
 */
LSA_VOID eps_pndevdrv_printf_openclose_board( LSA_BOOL bOpen, EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard )
{
	if (bOpen)
	{
		//printf("\r\nEPS PnDevDrv Board Opened %s:\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc); /* Commented because of wchar problem */
		printf("\r\nEPS PnDevDrv Board Opened PCI Bus %d, Dev %d, Fct %d (using FW (%d), SBL %d.%d.%d, Cnt Mac: %d)\r\n", (int)pBoard->Location.uBusNr, (int)pBoard->Location.uDeviceNr, (int)pBoard->Location.uFctNr, (int)pBoard->bBoardFwUsed, (int)pBoard->OpenDevice.uSblInfo.uVersion.lMajor, (int)pBoard->OpenDevice.uSblInfo.uVersion.lMinor, (int)pBoard->OpenDevice.uSblInfo.uVersion.lBuild,(int)pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lCtrMacAdr);
	}
	else
	{
		//printf("\r\nEPS PnDevDrv Board Closed %s:\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc); /* Commented because of wchar problem */
		printf("\r\nEPS PnDevDrv Board Closed PCI Bus %d, Dev %d, Fct %d\r\n", (int)pBoard->Location.uBusNr, (int)pBoard->Location.uDeviceNr, (int)pBoard->Location.uFctNr);
	}
}
#endif

/**
 * converts an error string of the pndevdrv
 *
 * PnDevDrv returns unicode strings including '\n'
 * We have to convert from unicode and delete the \n to be able to trace the string correctly
 * 
 * @param [in]  pIn	    pointer to the original string
 * @param [out] pOut	pointer to the converted string
 * @return
 */
LSA_VOID eps_convert_pndev_err_string( const PNDEV_CHAR* pIn, LSA_CHAR* pOut )
{
    UINT32  lLoop;
    UINT32  lNewPos	= 0;
    UINT32  lSizeIn;

    #if defined (PNDEV_UNICODE)
    fnConvertStringWcharToChar(pIn, pOut);
    #else
    eps_strcpy(pOut, pIn);
    #endif

    lSizeIn	= eps_strlen(pOut);

	for (lLoop = 0; lLoop <= lSizeIn; lLoop++)
		// every char in input string
	{
		if (pOut[lLoop] == '\n')
		   // char is new line
		{ 
			// do nothing because we want to remove it
		}
		else if (pOut[lLoop] == '\0')
			    // char is \0
		{
			// write the \0 one position forward, to overwrite the new line
			pOut[lNewPos] = pOut[lLoop];
		}
		else
			// every other char
		{
			// copy to actual position and increment pos
			pOut[lNewPos] = pOut[lLoop];
			lNewPos++;
		}
	}
}

/**
 * converts a path string for the pndevdrv to wchar (UNICODE)
 *
 * 
 * @param [in]  pIn   pointer to the original string
 * @param [out] pOut  pointer to the converted string
 * @return
 */
LSA_VOID eps_convert_pndev_path( const LSA_CHAR * pIn, PNDEV_CHAR * pOut )
{
    #if defined (PNDEV_UNICODE)
    fnConvertStringCharToWchar(pIn, pOut);
    #else
    eps_strcpy(pOut, pIn);
    #endif
}

/**
 * Allocates a local buffer to store a fatal board from a pn board.
 * This function calls the output macro EPS_APP_SAVE_DUMP_FILE to pass the buffer to the application.
 *
 * @param 	[in] pBoard						pointer to current board structure
 * @param 	[in] pPnDevDrvPciParamsArray	pointer to AhbPciBridgeConfiguration
 * @param 	[in] uCntBars					counter of bars
 * @param 	[in] pLocalPhyAddress			pointer with local address
*/
LSA_VOID eps_pndevdrv_alloc_dump_buffer( EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo, const LSA_UINT8* pHWPtr )
{
    LSA_UINT16 retValFree;
    pDumpStoreInfo->lBoardType      = pBoard->OpenDevice.uHwInfo.eBoard;
    pDumpStoreInfo->lAsicType       = pBoard->OpenDevice.uHwInfo.eAsic;
    pDumpStoreInfo->lAsicTypeInfo   = pBoard->OpenDevice.uHwInfo.eAsicDetail;
    pDumpStoreInfo->pDumpBuf        = (LSA_UINT8*)eps_mem_alloc(pDumpStoreInfo->lDumpSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);

    if (pDumpStoreInfo->pDumpBuf == 0)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EPS PnDevDrv: eps_pndevdrv_alloc_dump_buffer() gets 0-ptr at DumpBuf-allocation!!!, Allocated Size(%u)", 
                            pDumpStoreInfo->lDumpSize);
        EPS_FATAL("No memory for dump-buffer in eps_pndevdrv_alloc_dump_buffer()!!!");
    }

    eps_memcpy(pDumpStoreInfo->pDumpBuf, pHWPtr, pDumpStoreInfo->lDumpSize);
    EPS_APP_SAVE_DUMP_FILE(pDumpStoreInfo);
    retValFree = eps_mem_free(pDumpStoreInfo->pDumpBuf, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
    EPS_ASSERT(LSA_RET_OK == retValFree);
}

#if (PSI_CFG_USE_EDDI == 1)
/**
 * Translate local address to PciHostAdress (only for AhbPciBridge for Soc1 and Ertec400)
 * 
 * @param 	[in] pBoard						pointer to current board structure
 * @param 	[in] pPnDevDrvPciParamsArray	pointer to AhbPciBridgeConfiguration
 * @param 	[in] uCntBars					counter of bars
 * @param 	[in] pLocalPhyAddress			pointer with local address
 * @return 	[out] pTargetAddress			pointer with PciHostAddress
 */
LSA_UINT8* eps_pndevdrv_calc_phy_pci_host_address(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, uPNCORE_PCI_HOST_PAR* pPnDevDrvPciParamsArray, LSA_UINT32 uCntBars, const LSA_UINT8* pLocalPhyAddress)
{
	// Get the SOC/ERTEC400 AHB Address for an local physical pci address

	LSA_UINT32 i;
	uPNCORE_PCI_HOST_PAR *pHostBar;
	LSA_UINT8* pPciAddress;
	LSA_UINT8* pAhbAddress;
	LSA_UINT8* pTargetAddress = LSA_NULL;

	LSA_UNUSED_ARG(pBoard);

    if (uCntBars != 0)
	{
		EPS_ASSERT(pLocalPhyAddress < (LSA_UINT8*)0xFFFFFFFF); // (LaM) Soc/Ertec400 can only reach the first 4 GB

		for (i=0; i<uCntBars; i++)
		{
			pHostBar = &pPnDevDrvPciParamsArray[i];
			pPciAddress = (LSA_UINT8*)pHostBar->lBatPciHost;
			pAhbAddress = (LSA_UINT8*)pHostBar->lBarHost;
			if ( (pLocalPhyAddress > pPciAddress) && (pLocalPhyAddress <= ((pPciAddress - 1) + pHostBar->lSizePciHost) ) )
			{
				// found Fitting Window
				pTargetAddress = (pLocalPhyAddress - pPciAddress) + pAhbAddress;
				break;
			}
		}
	}

	return pTargetAddress;
}
#endif

/**
 * This function calls the service GetDeviceInfo of the PnDevDriver and store Information in global DeviceStructure (g_pEpsPnDevDrvHelper->foundDevices).
 * Throws a fatal if the return value is incorrect.
 * 
 * @param LSA_VOID
 * @return 
 */
LSA_VOID eps_pndevdrv_get_device_infos( LSA_VOID )
{
	ePNDEV_RESULT retVal;
	uPNDEV_GET_DEVICE_INFO_IN uGetDeviceInfoIn;

	PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

	eps_memset(&uGetDeviceInfoIn, 0, sizeof(uPNDEV_GET_DEVICE_INFO_IN));
	eps_memset(g_pEpsPnDevDrvHelper->foundDevices, 0, sizeof(g_pEpsPnDevDrvHelper->foundDevices));

	// Get DeviceInfo of all devices
	retVal = g_pEpsPnDevDrvHelper->pDrvHandle->uIf.pFnGetDeviceInfo(g_pEpsPnDevDrvHelper->pDrvHandle,
	                               sizeof(uPNDEV_GET_DEVICE_INFO_IN),
	                              (sizeof(uPNDEV_GET_DEVICE_INFO_OUT) * EPS_CFG_PNDEVDRV_MAX_BOARDS),
	                               EPS_CFG_PNDEVDRV_MAX_BOARDS,
	                               _countof(sError),
	                               &uGetDeviceInfoIn,
	                               g_pEpsPnDevDrvHelper->foundDevices,
								   (UINT32*)&g_pEpsPnDevDrvHelper->uCntFoundDevices,
	                               sError);

	#ifdef EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
	eps_pndevdrv_printf_found_boards();
	#endif

	/* OK or no devices found! */
	if ( (retVal != ePNDEV_OK) &&
	     (retVal != ePNDEV_ERROR_NOT_FOUND) )
	{
        LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
        eps_convert_pndev_err_string(sError, sErrConv);
		EPS_SYSTEM_TRACE_STRING( 0, LSA_TRACE_LEVEL_ERROR, "EPS PnDevDrv %s", sErrConv);

		#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
		printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__, (LSA_CHAR*)sError);
		#endif

		EPS_FATAL("Error getting device information via pndevdrv.");
	}
}

/**
 * Get the device as a handle (ppDevice) as well as an Index (return value) by passung a PCI Location (pLocation). 
 *
 * This function uses the PnDevDrv helper function ComparePciLoc.
 * 
 * @param [in]  pLocation 	    ptr including PciLocation
 * @param [out] ppDecive		ptr including ptr to corresponding element in DeviceStructure
 * @return 				        Index of the device within the management structure g_pEpsPnDevDrvHelper->foundDevices[].
 */
LSA_UINT32 eps_pndevdrv_get_device( EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, uPNDEV_GET_DEVICE_INFO_OUT** ppDevice )
{
	LSA_UINT32 i;
	ePNDEV_RESULT drvResult = ePNDEV_ERROR_NOT_FOUND;
	uPNDEV_COMPARE_PCI_LOC_IN  inComp;
	uPNDEV_COMPARE_PCI_LOC_OUT outComp;
	uPNDEV_GET_DEVICE_INFO_OUT *pDevice;

    #if (EPS_PLF != EPS_PLF_WINDOWS_X86)
	LSA_INT uBusNr, uDeviceNr, uFuncNr;
    #endif

	eps_memset( &inComp, 0,  sizeof( inComp ));
	eps_memset( &outComp, 0, sizeof( outComp ));

	inComp.lPciBusNo      = pLocation->uBusNr;
	inComp.lPciDeviceNo   = pLocation->uDeviceNr;
	inComp.lPciFunctionNo = pLocation->uFctNr;

	for (i=0; i < g_pEpsPnDevDrvHelper->uCntFoundDevices; i++)
	{
		pDevice = &g_pEpsPnDevDrvHelper->foundDevices[i];

        #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
		// driver strings have w_char size
		eps_memcpy( inComp.sPciLoc, g_pEpsPnDevDrvHelper->foundDevices[i].sPciLoc, sizeof(inComp.sPciLoc));
        #else
        eps_memcpy(inComp.sPciLoc,g_pEpsPnDevDrvHelper->foundDevices[i].sPciLoc, PNDEV_SIZE_STRING_BUF_SHORT);

		eps_pndevdrv_parse_location_string((LSA_CHAR*)pDevice->sPciLoc, &uBusNr, &uDeviceNr, &uFuncNr); /* (LaM) Workaround for uIf.pFnComparePciLoc Bug */
		if ( (pLocation->uBusNr == (LSA_UINT32)uBusNr) &&
		     (pLocation->uDeviceNr == (LSA_UINT32)uDeviceNr) &&
			 (pLocation->uFctNr == (LSA_UINT32) uFuncNr) )
		{
			*ppDevice = pDevice;
			break;
		}
        #endif

		drvResult = g_pEpsPnDevDrvHelper->pDrvHandle->uIf.pFnComparePciLoc( g_pEpsPnDevDrvHelper->pDrvHandle, sizeof(inComp), sizeof(outComp), &inComp, &outComp );

		if ( drvResult == ePNDEV_OK ) // Location matches
		{
			*ppDevice = pDevice;
			break;
		}
	}

	return i;
}

/**
 * register device infos 
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pndevdrv_register_device_infos( LSA_VOID )
{
	LSA_UINT16  i;
	LSA_UINT8   bRegisterBoardType;
	EPS_PNDEV_IF_DEV_INFO_TYPE sPnDevInfo;
	uPNDEV_GET_DEVICE_INFO_OUT* pDevice;
	LSA_INT uBusNr, uDeviceNr, uFuncNr;

	for (i=0; i<g_pEpsPnDevDrvHelper->uCntFoundDevices; i++)
	{
		pDevice = &g_pEpsPnDevDrvHelper->foundDevices[i];
	    bRegisterBoardType = LSA_TRUE;

		eps_pndevdrv_parse_location_string((LSA_CHAR*)pDevice->sPciLoc, &uBusNr, &uDeviceNr, &uFuncNr);

		sPnDevInfo.uPciAddrValid = 1;
		sPnDevInfo.uPciBusNr    = (LSA_UINT16)uBusNr;
		sPnDevInfo.uPciDeviceNr = (LSA_UINT16)uDeviceNr;
		sPnDevInfo.uPciFuncNr   = (LSA_UINT16)uFuncNr;

		sPnDevInfo.uMacAddrValid = 0;

		switch (pDevice->eBoard)
		{
			case ePNDEV_BOARD_CP1616:                   // CP1616 board
			case ePNDEV_BOARD_DB_EB400_PCIE:           	// DB-EB400-PCIe board
			case ePNDEV_BOARD_EB200:                    // EB200 board
			case ePNDEV_BOARD_DB_EB200_PCIE:            // EB200-PCIe board
			case ePNDEV_BOARD_DB_SOC1_PCI:              // Soc1-Dev board
			case ePNDEV_BOARD_DB_SOC1_PCIE:             // Soc1-Dev board
			case ePNDEV_BOARD_CP1625:             // Soc1-Dev board
				sPnDevInfo.uEddType = LSA_COMP_ID_EDDI;
				break;
			case ePNDEV_BOARD_FPGA1_ERTEC200P:          // FPGA1-Ertec200P board
			case ePNDEV_BOARD_EB200P:                   // EB200P board
			case ePNDEV_BOARD_FPGA1_HERA:               // FPGA-Hera board
				sPnDevInfo.uEddType = LSA_COMP_ID_EDDP;
				break;
			case ePNDEV_BOARD_I210:                   	// I210 board
			case ePNDEV_BOARD_I210SFP:					// I210 board with SPF module
            case ePNDEV_BOARD_I82574:                   // I82574 board
			case ePNDEV_BOARD_MICREL_KSZ8841:          	// Micrel KSZ8841 board
            case ePNDEV_BOARD_MICREL_KSZ8842:           // Micrel KSZ8842 board
			case ePNDEV_BOARD_TI_AM5728:		        // TI AM5728 board
            case ePNDEV_BOARD_IX1000:                   // IOT2000 board
                sPnDevInfo.uEddType = LSA_COMP_ID_EDDS;
                break;
			case ePNDEV_BOARD_INVALID:
				EPS_FATAL("Can't register a not supported board.");
				break;
			default:
			    bRegisterBoardType = LSA_FALSE;
				EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_WARN, "eps_pndevdrv_register_device_infos() - Board type (%u) on the PCI location (%u.%u.%u) is not supported in this firmware version", pDevice->eBoard, sPnDevInfo.uPciBusNr, sPnDevInfo.uPciDeviceNr, sPnDevInfo.uPciFuncNr);
				break;
		}
		
		if(bRegisterBoardType)
		{
		    eps_pndev_if_register_device(&sPnDevInfo);
		}
	}
}

EPS_PNDEV_BOARD_TYPE eps_pndevdrv_get_eps_pndev_board_type(ePNDEV_BOARD board)
{
    switch (board)
    {
        case ePNDEV_BOARD_CP1616:                   // CP1616 board
            return EPS_PNDEV_BOARD_CP1616;
        case ePNDEV_BOARD_DB_EB400_PCIE:            // DB-EB400-PCIe board
            return EPS_PNDEV_BOARD_EB400_PCIE;
        case ePNDEV_BOARD_EB200:                    // EB200 board
            return EPS_PNDEV_BOARD_EB200;
        case ePNDEV_BOARD_DB_EB200_PCIE:            // EB200-PCIe board
            return EPS_PNDEV_BOARD_EB200_PCIE;
        case ePNDEV_BOARD_DB_SOC1_PCI:              // Soc1-Dev board
            return EPS_PNDEV_BOARD_SOC1_PCI;
        case ePNDEV_BOARD_DB_SOC1_PCIE:             // Soc1-Dev board
            return EPS_PNDEV_BOARD_SOC1_PCIE;
		case ePNDEV_BOARD_CP1625:					// CP1625 Soc1-Dev board
			return EPS_PNDEV_BOARD_CP1625;
        case ePNDEV_BOARD_FPGA1_ERTEC200P:          // FPGA1-Ertec200P board
            return EPS_PNDEV_BOARD_FPGA_ERTEC200P;
        case ePNDEV_BOARD_EB200P:                   // EB200P board
            return EPS_PNDEV_BOARD_EB200P;
        case ePNDEV_BOARD_FPGA1_HERA:               // FPGA-Hera board
            return EPS_PNDEV_BOARD_FPGA_HERA;
        case ePNDEV_BOARD_I210:                     // I210 board
            return EPS_PNDEV_BOARD_INTEL_XX;
        case ePNDEV_BOARD_I210SFP:                  // I210 board with SPF module
            return EPS_PNDEV_BOARD_INTEL_XX;
        case ePNDEV_BOARD_I82574:                   // I82574 board
            return EPS_PNDEV_BOARD_INTEL_XX;
        case ePNDEV_BOARD_MICREL_KSZ8841:           // Micrel KSZ8841 board
            return EPS_PNDEV_BOARD_KSZ88XX;
        case ePNDEV_BOARD_MICREL_KSZ8842:           // Micrel KSZ8842 board
            return EPS_PNDEV_BOARD_KSZ88XX;
        case ePNDEV_BOARD_TI_AM5728:                // TI AM5728 board
            return EPS_PNDEV_BOARD_TI_XX;
        case ePNDEV_BOARD_IX1000:                   // IOT2000 board
            return EPS_PNDEV_BOARD_IX1000;
        case ePNDEV_BOARD_INVALID:
        default:
            EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_get_eps_pndev_board_type() - Board type unknown");
            EPS_FATAL(0);
    }
	//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_BOARD_INVALID;
}

#endif //((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
