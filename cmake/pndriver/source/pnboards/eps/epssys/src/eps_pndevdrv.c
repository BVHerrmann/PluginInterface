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
/*  F i l e               &F: eps_pndevdrv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Implementation - Main module.                            */
/*  Implementation of the EPS PN Dev Drv interface.                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20031
#define EPS_MODULE_ID      20031

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>           /* Types / Prototypes / Fucns */
#include <eps_rtos.h>

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
#include <eps_tasks.h>
#endif

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
/*-------------------------------------------------------------------------*/

#include <precomp.h>
#include <PnDev_Driver_Inc.h>           /* PnDevDrv Interface                               */

#include <eps_cp_hw.h>                  /* EPS CP PSI adaption                              */
#include <eps_trc.h>                    /* Tracing                                          */
#include <eps_pn_drv_if.h>              /* PN Device Driver Interface                       */
#include <eps_hw_edds.h>		        /* EDDS LL					                        */
#include <eps_pndevdrv.h>               /* PNDEV-Driver Interface implementation            */

#if ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_SOC)))
#include <eps_hw_soc.h>                 /* Soc1 hw adaption                                 */
#endif
#if ( ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_200))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
#include <eps_hw_ertec200.h>            /* Ertec200 hw adaption                             */
#endif
#if ( ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
#include <eps_hw_ertec400.h>            /* Ertec400 hw adaption                             */
#endif
#include <eps_shm_file.h>               /* EPS Shared Memory File Interface                 */
#include <eps_ipc_drv_if.h>             /* EPS Interprocessor communication                 */
#include <eps_register.h>               /* Register Access Macros                           */

#include "eps_pndevdrv_helper.h"        /* EPS PNDevDrv Helper functions                    */
#include "eps_pndevdrv_boards.h"        /* EPS PNDevDrv board specific functions            */
#include "eps_pndevdrv_timer_threads.h" /* EPS PNDevDrv timers and threads                  */
#include "eps_pndevdrv_events_isr.h"    /* EPS PNDevDrv callback functions and event impl   */

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
#include "eps_tasks.h"
#endif

#ifdef _TEST
#include "testapp.h"
#endif

#if ((EPS_PLF == EPS_PLF_ADONIS_X86) && (PSI_CFG_TCIP_STACK_OPEN_BSD == 1))
#include <eps_lib.h>
#endif

#ifdef EPS_CFG_USE_PNDEVDRV

/* - Local Defines -------------------------------------------------------------------------------- */

// TTU Timer on SOC1, offset to time value. See TTU-spec
#define PNDEVDRV_SOC1_OFFSET_TTU_LOW    0x20
#define PNDEVDRV_SOC1_OFFSET_TTU_HIGH   0x28

// TTU Timer on EB200P, offset to time value. See TTU-spec
#define PNDEVDRV_EB200P_OFFSET_TTU_LOW  0x8
#define PNDEVDRV_EB200P_OFFSET_TTU_HIGH 0xC

// Local timer on ERTEC400, offset to time value. See ERTEC spec
#define PNDEVDRV_ERTEC400_OFFSET_TTU_LOW 0x1C

// Local timer on ERTEC400, offset to time value. See ERTEC spec
#define PNDEVDRV_ERTEC200_OFFSET_TTU_LOW 0x1C

// TTU Timer on TI AM5728 board, offset to time value. See AM5728-Evalboard_Spec
#define PNDEVDRV_AM5728_OFFSET_TTU_LOW 0x3C

/* - Function Forward Declaration ----------------------------------------------------------------- */

static LSA_VOID    eps_pndevdrv_uninstall         (LSA_VOID);

static LSA_UINT16  eps_pndevdrv_read_trace_data        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_pndevdrv_write_trace_data       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const*  ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_pndevdrv_write_sync_time_lower  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);
static LSA_UINT16  eps_pndevdrv_save_dump              (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);

/* - Typedefs ------------------------------------------------------------------------------------- */
typedef struct eps_pndevdrv_store_tag
{
	LSA_BOOL bInit;

	EPS_PNDEVDRV_BOARD_TYPE board[EPS_CFG_PNDEVDRV_MAX_BOARDS];

	uPNDEV_DRIVER_DESC *pDrvHandle;
} EPS_PNDEVDRV_STORE_TYPE, *EPS_PNDEVDRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_PNDEVDRV_STORE_TYPE g_EpsPnDevDrv;
static EPS_PNDEVDRV_STORE_PTR_TYPE g_pEpsPnDevDrv = LSA_NULL;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86) && (PSI_CFG_USE_EDDT == 1)
// Currently only required as a hack to download the C66 FW.
/* - Functions ------------------------------------------------------------------------------------ */
static LSA_BOOL eps_file_exist( LSA_CHAR * const filename )
{
	FILE     * filep;
	LSA_BOOL   bExist = LSA_FALSE; // preset negative

	filep = fopen(filename, "r");
	if (filep)
	{
		// file exists
		bExist = LSA_TRUE;
		fclose(filep);
	}

	return bExist;
}
#endif

/**
 * @param [in] pHwInstIn        pointer to structure with hardware configuration
 * @return #EPS_PNDEV_RET_OK    Dump was saved
*/
static LSA_UINT16 eps_pndevdrv_save_dump( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    LSA_UINT16                    result         = EPS_PNDEV_RET_OK;
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
    EPS_FATAL_STORE_DUMP_PTR_TYPE pStoreDumpInfo;
    EPS_FATAL_STORE_DUMP_TYPE     StoreDumpInfo;

	eps_pndevdrv_enter();

	pBoard         = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
    pStoreDumpInfo = &StoreDumpInfo;

    switch (pBoard->OpenDevice.uHwInfo.eAsic)
    {
        case ePNDEV_ASIC_ERTEC400:
        case ePNDEV_ASIC_ERTEC200:
        case ePNDEV_ASIC_SOC1:
            //copy dump of eddi irte to local buffer
            StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddi.irte.size;
            StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_IRTE;
            eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr);
            break;

        case ePNDEV_ASIC_ERTEC200P:
		case ePNDEV_ASIC_HERA:
            //copy dump of eddp pnip to local buffer
            StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.pnip.size;
            StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_PNIP;
            eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr);

            StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size + pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size;
            StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_SDRAM;
            eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr);

			if	(pBoard->OpenDevice.uHwInfo.eAsic == ePNDEV_ASIC_ERTEC200P)
				// Ertec200P
			{
			    StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.perif_ram.size;
			    StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_PERIF_RAM;
			    eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.perif_ram.base_ptr);

                StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.size;
                StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_K32_TCM;
                eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.base_ptr);
            }

            if	(pBoard->OpenDevice.uHwInfo.eAsic == ePNDEV_ASIC_HERA)
				// HERA
			{
                StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.size;
				StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_HERA_K32_A;
                eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.base_ptr);

                StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.size;
				StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_HERA_K32_B;
                eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.base_ptr);

                StoreDumpInfo.lDumpSize     = pBoard->sHw.EpsBoardInfo.eddp.k32_ddr3.size;
				StoreDumpInfo.lMemRamType   = EPS_RAM_MEM_HERA_DDR3;
                eps_pndevdrv_alloc_dump_buffer(pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.k32_ddr3.base_ptr);
			}

            break;

        case ePNDEV_ASIC_I210:
        case ePNDEV_ASIC_I210IS:
        case ePNDEV_ASIC_I82574:
        case ePNDEV_ASIC_KSZ8841:
        case ePNDEV_ASIC_KSZ8842:
        case ePNDEV_ASIC_AM5728:
        case ePNDEV_ASIC_IX1000:
        case ePNDEV_ASIC_INVALID:
            break;

        default:
            break;
    }

	eps_pndevdrv_exit();

    return result;
}

/**
 * show fatal dump in AsicSdram (FirmwareFatal of PnCoreInterface)
 *
 * Calls EPS_FATAL_WITH_REASON if the lower instance reported a FATAL error.
 *
 * @param [in] pBoard    pointer to current board structure
 * @return
 */
LSA_VOID eps_pndevdrv_show_fatal_dump( EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE const pBoard )
{
	ePNDEV_RESULT                drvResult;
	EPS_SHM_FATAL_STORE_PTR_TYPE pFatalStore;
	LSA_UINT8                    BufOut[sizeof(EPS_SHM_FATAL_STORE_TYPE) + sizeof(uPNDEV_COPY_DATA_OUT)];
	uPNDEV_COPY_DATA_OUT *       pCopyDataOut;
	uPNDEV_COPY_DATA_IN          copyDataIn;

	EPS_ASSERT(pBoard->bUsed);

	pCopyDataOut = (uPNDEV_COPY_DATA_OUT*)BufOut;

	eps_memset( &copyDataIn, 0, sizeof(copyDataIn) );

	switch( pBoard->OpenDevice.uHwInfo.eAsic )
	{
		case ePNDEV_ASIC_ERTEC400:
			copyDataIn.uArrayBlockRead[0].lOffsetAsic = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwFatal.lOffset;
			break;
		case ePNDEV_ASIC_ERTEC200:
			copyDataIn.uArrayBlockRead[0].lOffsetAsic = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwFatal.lOffset;
			break;
		case ePNDEV_ASIC_SOC1:
			copyDataIn.uArrayBlockRead[0].lOffsetAsic = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwFatal.lOffset;
			break;
		case ePNDEV_ASIC_ERTEC200P:
			copyDataIn.uArrayBlockRead[0].lOffsetAsic = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwFatal.lOffset;
			break;
		case ePNDEV_ASIC_HERA:
			copyDataIn.uArrayBlockRead[0].lOffsetAsic = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwFatal.lOffset;
			break;
        case ePNDEV_ASIC_AM5728:
            copyDataIn.uArrayBlockRead[0].lOffsetAsic = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwFatal.lOffset;
            break;
        case ePNDEV_ASIC_I210:
        case ePNDEV_ASIC_I210IS:
        case ePNDEV_ASIC_I82574:
        case ePNDEV_ASIC_IX1000:
        case ePNDEV_ASIC_KSZ8841:
        case ePNDEV_ASIC_KSZ8842:
        case ePNDEV_ASIC_INVALID:
        default:
			EPS_FATAL("Not supported ASIC found.");
	}

	copyDataIn.eHwRes          = ePNDEV_COPY_HW_RES_SDRAM;
	copyDataIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;

	copyDataIn.uArrayBlockRead[0].lOffsetHostDataBuf = 0;
	copyDataIn.uArrayBlockRead[0].lSize              = sizeof( EPS_SHM_FATAL_STORE_TYPE );

	drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCopyData(g_pEpsPnDevDrv->pDrvHandle,
		                                                    sizeof(copyDataIn),
		                                                    sizeof(BufOut),
		                                                    &copyDataIn,
		                                                    pCopyDataOut);

	EPS_ASSERT( drvResult == ePNDEV_OK );

	pFatalStore = (EPS_SHM_FATAL_STORE_PTR_TYPE)(BufOut+sizeof(uPNDEV_COPY_DATA_OUT));

	if (pFatalStore->bUsed)
	{
        //read fatal dump from subordinate instance

		EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EPS PnDevDrv: eps_pndevdrv_show_fatal_dump(): EPS FATAL ON FW !!! hd_id(%u)", pBoard->sysDev.hd_nr);

        #ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
		printf("\r\n\r\n");

		printf("EPS PnDevDrv: eps_pndevdrv_show_fatal_dump(): EPS FATAL ON FW !!! hd_id(%u)", pBoard->sysDev.hd_nr);

        switch (pFatalStore->exit_code_org)
        {
            case EPS_EXIT_CODE_LSA_FATAL: //caller = PSI_FATAL_ERROR()
            {
			    printf(" => Comp(%s) CompId(%d) ModuleId(%d) Line(%d) ErrCode([0]=%d / 0x%lx,[1]= %d / 0x%lx,[2]= %ld / 0x%lx,[3]= %d / 0x%lx)", pFatalStore->sComp, (int)pFatalStore->lsa_fatal.lsa_component_id, (int)pFatalStore->lsa_fatal.module_id, (int)pFatalStore->lsa_fatal.line, (int)pFatalStore->lsa_fatal.error_code[0], pFatalStore->lsa_fatal.error_code[0], (int)pFatalStore->lsa_fatal.error_code[1], pFatalStore->lsa_fatal.error_code[1], pFatalStore->lsa_fatal.error_code[2], pFatalStore->lsa_fatal.error_code[2], (int)pFatalStore->lsa_fatal.error_code[3], pFatalStore->lsa_fatal.error_code[3]);
			    eps_tasks_sleep(10); //spend some time to print out
                break; 
            }
            default: //all other exit_codes/callers
            {
			    printf(" => File(%s) Line(%lu) Msg(%s)", pFatalStore->sFile, pFatalStore->uLine, pFatalStore->sMsg);
			    eps_tasks_sleep(10); //spend some time to print out
            }
        }

		printf("\r\n\r\n");
	    eps_tasks_sleep(20); //spend some time to print out
        #endif

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
		            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_show_fatal_dump(): EPS lsa_fatal_error_data buffer not sufficient! SHM_error_data_length(%u) EPS_ERROR_DATA_BUF_LENGTH(%u)", pFatalStore->lsa_fatal.error_data_length, EPS_ERROR_DATA_BUF_LENGTH);
                }

                EPS_FATAL_WITH_REASON(pFatalStore->exit_code_org, pFatalStore->exit_code_org, comp, length, lsa_fatal_ptr, 0, 0, 0, 0);
                break;
            }
            default: //all other exit_codes/callers
            {
			    LSA_CHAR * const file = &(pFatalStore->sFile[0]);
			    LSA_UINT32 const line = pFatalStore->uLine;
			    LSA_CHAR * const func = &(pFatalStore->sFunc[0]);
			    LSA_CHAR * const str  = &(pFatalStore->sMsg[0]);

                EPS_FATAL_WITH_REASON(pFatalStore->exit_code_org, pFatalStore->exit_code_org, 0, 0, 0, file, (int)line, func, str);
            }
        }
    }
    else
    {
        EPS_FATAL("EPS FATAL ON FW, no information available in FatalStore!!!");
    }
}

/**
 * duplicate board entry within the internal management structure.
 * - initialize board structure with function pointers and other parameters
 *
 * @param pBoardOld            already allocated board with valid board data
 * @param pBoardNew            new free board entry
 * @param InterfaceSelectorNew new Interface Selector
 * @return [out] -
 */
static LSA_VOID eps_pndevdrv_duplicate_board_data( EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoardOld, EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoardNew, EPS_PNDEV_INTERFACE_SELECTOR_TYPE const InterfaceSelectorNew )
{
    //copy board data from already allocated board (old board)
    EPS_ASSERT(pBoardOld != LSA_NULL);
    EPS_ASSERT(pBoardNew != LSA_NULL);
	eps_memcpy(pBoardNew, pBoardOld, sizeof(EPS_PNDEVDRV_BOARD_TYPE));

    //modify board data
    pBoardNew->Location.eInterfaceSelector = InterfaceSelectorNew;
    pBoardNew->sHw.hDevice                 = (LSA_VOID*)pBoardNew;
}

/**
 * allocate board entry in the internal management structure.
 * - create new board
 * - initialize board structure with function pointers and other parameters
 *
 * @param pLocation
 * @param pHD_Open_Device_Ctr
 * @return [out] pBoard     Pointer to board structure
 * @return [out] LSA_NULL   if all accessible entries are already in use
 */
static EPS_PNDEVDRV_BOARD_PTR_TYPE eps_pndevdrv_alloc_board( EPS_PNDEV_LOCATION_CONST_PTR_TYPE const pLocation, LSA_UINT32 * const pHD_Open_Device_Ctr )
{
	LSA_UINT32                  Ctr;
    LSA_UINT32                  HD_Open_Device_Ctr = 0; //number of HD open devices at current board
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoardFound        = LSA_NULL;
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoardFree         = LSA_NULL;

    //check all allocated board entries in the internal management structure
	for (Ctr=EPS_CFG_PNDEVDRV_MAX_BOARDS; Ctr; )
    {
	    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;

        Ctr--;

        pBoard = &g_pEpsPnDevDrv->board[Ctr];

		if //used board entry?
		   (pBoard->bUsed)
        {
		    if //same board (same PCI location) is already opened?
		       (   (pBoard->Location.eLocationSelector == pLocation->eLocationSelector)
                && (pBoard->Location.eLocationSelector == EPS_PNDEV_LOCATION_PCI)
                && (pBoard->Location.uBusNr            == pLocation->uBusNr)
                && (pBoard->Location.uDeviceNr         == pLocation->uDeviceNr)
                && (pBoard->Location.uFctNr            == pLocation->uFctNr))
            {
                //same board (same PCI location) is already opened

                switch (pBoard->OpenDevice.uHwInfo.eAsic) //ASIC type of the stored board
                {
                    case ePNDEV_ASIC_HERA:   //HERA board:      2 board entries are allowed because of the 2 PNIPs
                    case ePNDEV_ASIC_AM5728: //TI-AM5728 board: 2 board entries are allowed because of GMAC and ICSS
                    {
                        if //board already found twice?
                           (HD_Open_Device_Ctr)
                        {
                            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_alloc_board: Error: board already opened twice");
                            EPS_FATAL("eps_pndevdrv_alloc_board: Error: board already opened twice");
                        }
                        if //Interface Selector is identical?
                           (pBoard->Location.eInterfaceSelector == pLocation->eInterfaceSelector)
                        {
                            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_alloc_board: Error: InterfaceSelector is identical");
                            EPS_FATAL("eps_pndevdrv_alloc_board: Error: InterfaceSelector is identical");
                        }
                        break;
                    }
                    default: //another board
	                {
                        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_alloc_board: Error: same board already opened");
                        EPS_FATAL("eps_pndevdrv_alloc_board: Error: same board already opened");
                    }
	                //lint --e(788) enum constant not used within defaulted switch - some of the cases are only visible when compiler switches are set.
                }

                pBoardFound = pBoard;
                HD_Open_Device_Ctr++;
            }
        }
        else //free board entry
        {
            //store free board entry
            pBoardFree = pBoard;
        }
    } //for-loop

	if //free board entry available?
	   (pBoardFree != LSA_NULL)
	{
        if //same board (same PCI location) is already opened?
           (HD_Open_Device_Ctr)
        {
            //copy board data from 1st board
            eps_pndevdrv_duplicate_board_data(pBoardFound, pBoardFree, pLocation->eInterfaceSelector);
        }
        else //new board
        {
			pBoardFree->sHw.hDevice        		= (LSA_VOID*)pBoardFree;
			pBoardFree->sHw.EnableIsr      		= eps_pndevdrv_enable_interrupt;
			pBoardFree->sHw.DisableIsr     		= eps_pndevdrv_disable_interrupt;
            pBoardFree->sHw.EnableHwIr     		= eps_pndevdrv_enable_hw_interrupt;
			pBoardFree->sHw.DisableHwIr    		= eps_pndevdrv_disable_hw_interrupt;
			pBoardFree->sHw.ReadHwIr       		= eps_pndevdrv_read_hw_interrupt;
			pBoardFree->sHw.SetGpio        		= eps_pndevdrv_set_gpio;
			pBoardFree->sHw.ClearGpio      		= eps_pndevdrv_clear_gpio;
			pBoardFree->sHw.TimerCtrlStart 		= eps_pndevdrv_timer_ctrl_start;
			pBoardFree->sHw.TimerCtrlStop  		= eps_pndevdrv_timer_ctrl_stop;
			pBoardFree->sHw.ReadTraceData  		= eps_pndevdrv_read_trace_data;
			pBoardFree->sHw.WriteTraceData 		= eps_pndevdrv_write_trace_data;
			pBoardFree->sHw.WriteSyncTimeLower	= eps_pndevdrv_write_sync_time_lower;
            pBoardFree->sHw.SaveDump       		= eps_pndevdrv_save_dump;
			pBoardFree->pBackRef           		= g_pEpsPnDevDrv;
 		    pBoardFree->Location                = *pLocation;
			pBoardFree->bUsed              		= LSA_TRUE;
        }

        *pHD_Open_Device_Ctr = HD_Open_Device_Ctr + 1UL;
		return pBoardFree;
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_alloc_board: no free board entry available");

	return LSA_NULL;
}

/**
 * free board entry
 *
 * @param pBoard	ptr to current board structure
 * @return LSA_VOID
 */
static LSA_VOID eps_pndevdrv_free_board( EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard )
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);

	// UT5507 - Memset to Zero of pBoard-Data
	eps_memset(pBoard, 0, sizeof(EPS_PNDEVDRV_BOARD_TYPE));

	pBoard->bUsed = LSA_FALSE;
}

/**
 * This helper function sets up the config record for the HIF configuration.
 *
 * Since the eps_pndevdriver is used in the three variants Linux, Adonis and Windows, different HIF configurations are possible:
 *
 * - EPS Light Variant (Windows and Linux) : (HIF is not active)
 * - EPS Basic Variant (Windows and Linux) : - Uses EPS HIF Universal Driver or EPS HIF PNCoreDriver (@see eps_hif_universal_drv_install, @see eps_hif_pncorestd_drv_install)
 *                                           - This will cause HIF to run with polling mode or interrupt mode (PN Core interrupts).
 *                                           - HERA and SOC1 are using DMA for data transfer in Basic 1 IF and Basic NIF.
 * - EPS Advanced Variant (Adonis x86)     : Use EPS HIF IPC SieDMA Driver (@see eps_hif_ipcsiedma_drv_install) and
 *                                           - This will cause HIF to run with interrupt mode (interprocessor interrupts, see @eps_ipc_install).
 *
 * Currently, we set the configuration by checking the precompile switches EPS_PLF_WINDOWS_X86, EPS_PLF_LINUX_X86 and EPS_PLF_ADONIS_X86.
 *
 * Note: You can activate HIF polling mode for advanced by setting pBoard->SharedMemConfig.bValid = 0. This will cause the EPS to use EPS HIF Universal driver for the advanced configuration.
 *
 * @param [in]  pLocation 	ptr including PciLocation
 * @param [out] ppDecive	ptr including ptr to corresponding element in DeviceStructure
 */
static LSA_VOID eps_pndevdrv_open_device_setup_cfg_record( EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard, LSA_BOOL const bSetupEmpty, LSA_UINT16 const hd_id )
{
	#if (EPS_PLF != EPS_PLF_LINUX_IOT2000)
	eps_memset(&pBoard->SharedMemConfig, 0, sizeof(pBoard->SharedMemConfig));

    pBoard->SharedMemConfig.bValid = 0;

    // No valid HIF setup required (e.g. I210)
    if(bSetupEmpty)
    {
        return;
    }

	switch(pBoard->pDeviceInfo->eBoard)
	{
		case ePNDEV_BOARD_CP1616:
		case ePNDEV_BOARD_DB_EB400_PCIE:
        {
            #if (EPS_PLF != EPS_PLF_WINDOWS_X86)
			pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode    = LSA_TRUE;
			pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem        = LSA_TRUE;
			pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts	= LSA_FALSE;
			pBoard->SharedMemConfig.uCfgLength                      = sizeof(pBoard->SharedMemConfig);
			pBoard->SharedMemConfig.HIF.bValid                      = 1;
			pBoard->SharedMemConfig.bValid                          = 1;
            #endif
			break;
        }
		case ePNDEV_BOARD_EB200:
		case ePNDEV_BOARD_DB_EB200_PCIE:
        {
		    pBoard->SharedMemConfig.bValid = 0; /* (LaM) not supported now */
			break;
        }
		case ePNDEV_BOARD_DB_SOC1_PCI:
		case ePNDEV_BOARD_DB_SOC1_PCIE:
        case ePNDEV_BOARD_CP1625:
        {
			pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode    = LSA_TRUE;
            #if (EPS_PLF != EPS_PLF_WINDOWS_X86)
            // don't use remote mem in Windows
			pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem        = LSA_TRUE;
            #endif
            #if (EPS_PLF == EPS_PLF_ADONIS_X86)
			// generic IPC driver eps_ipc_generic_drv only supported in adonis, it is used in for x86 and soc1 here
			pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts	= LSA_TRUE;
			pBoard->SharedMemConfig.HIF.HifCfg.uType				= eEPS_IPC_DRV_SUPPORT_GENERIC;
            #else
            // use eps_ipc_pndevdrv for x86 and eps_ipc_linux_soc_drv/eps_ipc_generic_drv for soc basic fw
		    pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts    = LSA_TRUE;
		    pBoard->SharedMemConfig.HIF.HifCfg.uType                = eEPS_IPC_DRV_SUPPORT_CP1625;
            #endif
			pBoard->SharedMemConfig.uCfgLength                      = sizeof(pBoard->SharedMemConfig);
			pBoard->SharedMemConfig.HIF.bValid                      = 1;
			pBoard->SharedMemConfig.bValid                          = 1;
			break;
        }
		case ePNDEV_BOARD_FPGA1_ERTEC200P:
        {
		    pBoard->SharedMemConfig.bValid = 0; /* (LaM) not supported now */
			break;
        }
		case ePNDEV_BOARD_EB200P:
        {
		    pBoard->SharedMemConfig.bValid = 0; /* (LaM) not supported now */
			break;
        }
		case ePNDEV_BOARD_FPGA1_HERA:
        {
            // HERA enable shm and ipc and xgdma for HIF.
            pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode    = LSA_TRUE;
            pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem        = LSA_TRUE;
            pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts    = LSA_TRUE;
			pBoard->SharedMemConfig.HIF.HifCfg.uType				= eEPS_IPC_DRV_SUPPORT_HERA;
            pBoard->SharedMemConfig.HIF.HifCfg.bUseDma              = LSA_TRUE;
            pBoard->SharedMemConfig.uCfgLength                      = sizeof(pBoard->SharedMemConfig);
            pBoard->SharedMemConfig.HIF.bValid                      = 1;
            pBoard->SharedMemConfig.bValid                          = 1;
            break;
        }
        case ePNDEV_BOARD_TI_AM5728:
        {
            pBoard->SharedMemConfig.bValid = 0;
#if (EPS_PLF != EPS_PLF_WINDOWS_X86)
            //pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode    = LSA_TRUE;
            //pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem        = LSA_TRUE;
            //pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts    = LSA_FALSE;
            //pBoard->SharedMemConfig.uCfgLength                      = sizeof(pBoard->SharedMemConfig);
            //pBoard->SharedMemConfig.HIF.bValid                      = 1;
            //pBoard->SharedMemConfig.bValid                          = 1;
#endif
            break;
        }
        case ePNDEV_BOARD_I210:
        case ePNDEV_BOARD_I210SFP:
        case ePNDEV_BOARD_I82574:
        case ePNDEV_BOARD_IX1000:
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        case ePNDEV_BOARD_INVALID:
		default:
        {
			EPS_FATAL(0); /* Board not supported */
			break;
		}
    }

	/* if shared mem config is not used, HIF parameters get set up by eps_pndevdrv_open_hif_driver() */

	if(pBoard->SharedMemConfig.bValid == 1)
	{
		if(pBoard->SharedMemConfig.HIF.bValid == 1)
		{
			if(pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts == LSA_TRUE)
			{
				LSA_UINT16 retVal;

				// Open ipc interface
				eps_ipc_drv_if_open(hd_id, pBoard->SharedMemConfig.HIF.HifCfg.uType);

			    retVal = eps_ipc_drv_if_rx_alloc(&pBoard->Hif.Ipc.Rx.uRxHandle, &pBoard->SharedMemConfig.HIF.IPC.uPhysAddress, &pBoard->SharedMemConfig.HIF.IPC.uMsgValue, hd_id);
				EPS_ASSERT(retVal == EPS_IPC_RET_OK);
			}
			if (pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode)
			{
				if (pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem)
				{
					LSA_UINT32 uPciHostMemHifSize = 0x00200000;

					pBoard->SharedMemConfig.HIF.HostRam.uSize = uPciHostMemHifSize;
				}
			}
		}
	}
    #else
	LSA_UNUSED_ARG( hd_id );
	LSA_UNUSED_ARG( bSetupEmpty );
	LSA_UNUSED_ARG( pBoard );
    #endif
}

/**
 * Open the device for a given location. This calls the PnDevDriver function OpenDevice.
 *
 * @param [in]  pBoard 			    ptr to corresponding board structure
 * @param [in]  pOption			    ptr to options, if FirmwareDownload and Debugging will be executed
 * @param [out] puOpenDeviceOut	    ptr to OpenDeviceOut parameter
 * @param [in]  hd_id				index of hd
 * @return 	ePNDEV_OK if            successfull
 */
static ePNDEV_RESULT eps_pndevdrv_open_device( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, uPNDEV_OPEN_DEVICE_OUT * const puOpenDeviceOut, LSA_UINT16 const hd_id )
{
    EPS_APP_FILE_TAG_TYPE file_tag_fw;
    EPS_APP_FILE_TAG_TYPE file_tag_bootloader;
	uPNDEV_OPEN_DEVICE_IN uOpenDeviceIn;
	ePNDEV_RESULT         drvResult;
    LSA_BOOL              bMaster = LSA_FALSE; //preset
    LSA_BOOL              bSetupEmpty;
    
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    if (pBoard->pDeviceInfo->eBoard != ePNDEV_BOARD_TI_AM5728)
    {
	    bMaster = LSA_TRUE;  // Using on windows level the Master APP access is always set.
    }
#elif (EPS_PLF == EPS_PLF_ADONIS_X86) // For Adonis we are activating the bMaster, because we are interrupt driven.
    if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I210)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I210SFP)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I82574)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA))
    {
        bMaster = LSA_TRUE; // TODO-MSI: = LSA_TRUE;  // If EDDS is driven in interrupt mode, it has to be opened as MasterApp.
    }
#elif (EPS_PLF == EPS_PLF_LINUX_X86)
    if (pOption->bLoadFw)   // we are running a basic variant
    {
        if (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625)
        {
            bMaster = LSA_TRUE;
        }
    }
#endif

    eps_memset(&uOpenDeviceIn, 0, sizeof(uOpenDeviceIn));

	uOpenDeviceIn.bMasterApp = (bMaster == LSA_TRUE) ? LSA_TRUE : LSA_FALSE;

    if (bMaster)
	{
		uOpenDeviceIn.uCbfIsr.pPtr = eps_pndevdrv_isr_cbf;
	}
	else
	{
		uOpenDeviceIn.uCbfIsr.pPtr = LSA_NULL;
	}

    // bReqHostMem = TRUE:  NRT Mem is in host
    // bReqHostMem = FALSE: NRT Mem is at PNBoard
    // Preset HostMem not required
	uOpenDeviceIn.bReqHostMem       = FALSE;
    uOpenDeviceIn.bReqSharedHostMem = FALSE;

    if (   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I210)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I210SFP)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I82574)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_IX1000)
        /* || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_TI_AM5728) GMAC on TI can't write to hostmem, hostmem can be used for HIF */
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA))
        // board needs HostMem on Adonis/Linux/Windows
    {
        // get HostMemory
        uOpenDeviceIn.bReqHostMem = TRUE;
    }

    #if ((EPS_PLF != EPS_PLF_WINDOWS_X86) || (defined EPS_CFG_USE_IO_HOSTMEM))
	if (   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625))
		// additional boards on Adonis/Linux
	{
		// get HostMemory
		uOpenDeviceIn.bReqHostMem = TRUE;
	}
    #endif

	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_device(): bReqHostMem=%u", uOpenDeviceIn.bReqHostMem);

	uOpenDeviceIn.bInitPhy           = TRUE; /* inits the PHY of an Soc1 Board  */
	uOpenDeviceIn.bVerifyEnvironment = TRUE; /* Interrupt Test / Reset of Board */
	uOpenDeviceIn.uCbfEvent.pPtr     = eps_pndevdrv_event_cbf;
	uOpenDeviceIn.uCbfParUser.pPtr	 = (void*)pBoard;

	uOpenDeviceIn.uThreadDll.lPrio              = PNDEV_THREAD_PRIO_HIGH;
	uOpenDeviceIn.uThreadDll.lStackSize         = 0;        /* Use Default 8kB according to documentation */
	uOpenDeviceIn.uThreadSys.lPrio              = PNDEV_THREAD_PRIO_HIGH;
	uOpenDeviceIn.uThreadSys.lStackSize         = 0;        /* Use Default 8kB according to documentation */
	uOpenDeviceIn.uThreadSys.lTimeCycle_10msec  = 0; /* Use Default 100ms according to documentation */
	EPS_ASSERT(pBoard->pDeviceInfo != LSA_NULL);

    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
	// driver strings have w_char size
	eps_memcpy(uOpenDeviceIn.sPciLoc, pBoard->pDeviceInfo->sPciLoc, sizeof(uOpenDeviceIn.sPciLoc));
    #else
    eps_memcpy(uOpenDeviceIn.sPciLoc, pBoard->pDeviceInfo->sPciLoc, PNDEV_SIZE_STRING_BUF_SHORT);
    #endif

    pBoard->bBoardFwUsed = pOption->bLoadFw;

	file_tag_fw.FileType = EPS_APP_FILE_FW_ADONIS; // this is a preset, application can override this value (e.g. use linux firmware instead)	

    if (pOption->bLoadFw)
    {
        // download FW
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_pndevdrv_open_device(): download FW");

        file_tag_fw.isHD     					= (LSA_BOOL)hd_id;
        file_tag_fw.pndevBoard					= eps_pndevdrv_get_eps_pndev_board_type(pBoard->pDeviceInfo->eBoard);
        
		if (!pOption->bUseEpsAppToGetFile)
        {   
			// use the firmware provided in the option pointer
			EPS_ASSERT(pOption->pFwBufData != LSA_NULL);
			EPS_ASSERT(pOption->uFwBufSize > 0);
            
			uOpenDeviceIn.uUserFw.uFw[0].eSrc				= ePNDEV_FW_SRC_BUF;
			uOpenDeviceIn.uUserFw.uFw[0].eFwType			= ePNDEV_FW_TYPE_ADONIS;
        
			uOpenDeviceIn.uUserFw.uFw[0].as.uBuf.uBase.pPtr = pOption->pFwBufData;
			uOpenDeviceIn.uUserFw.uFw[0].as.uBuf.lSize		= pOption->uFwBufSize;
        }
        else
		{
			// use EPS APP to get the firmware
			EPS_APP_ALLOC_FILE(&file_tag_fw);

			if (file_tag_fw.FileType == EPS_APP_FILE_FW_ADONIS)
				// we use the adonis variant 
			{
				uOpenDeviceIn.uUserFw.uFw[0].eSrc       = ePNDEV_FW_SRC_BUF;
				uOpenDeviceIn.uUserFw.uFw[0].eFwType    = ePNDEV_FW_TYPE_ADONIS;
    
                uOpenDeviceIn.uUserFw.uFw[0].as.uBuf.uBase.pPtr = file_tag_fw.pFileData;
                uOpenDeviceIn.uUserFw.uFw[0].as.uBuf.lSize      = file_tag_fw.uFileSize;
            }
			else if (file_tag_fw.FileType == EPS_APP_FILE_FW_LINUX)
				// we use the linux variant 
			{
				if (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625)
					// supported boards for linux
				{
					uOpenDeviceIn.uUserFw.uFw[0].eSrc		= ePNDEV_FW_SRC_FILE;
					uOpenDeviceIn.uUserFw.uFw[0].eFwType	= ePNDEV_FW_TYPE_LINUX;

					// use path to linux image instead of buffer (pndevdrv opens the file)
					eps_convert_pndev_path(file_tag_fw.Path.sPath, uOpenDeviceIn.uUserFw.uFw[0].as.uFile.sPath);
				}
				else
				{
					EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_open_device(): Board does not support Linux firmware");
					EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_EPS_FATAL, EPS_EXIT_CODE_EPS_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndevdrv_open_device(): Board does not support Linux firmware");
				}
			}
			else
			{
				EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_open_device(): Unsupported EPS_APP_FILE_TAG_ENUM used");
				EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_EPS_FATAL, EPS_EXIT_CODE_EPS_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndevdrv_open_device(): Unsupported EPS_APP_FILE_TAG_ENUM used");
            }
        }

        uOpenDeviceIn.uUserFw.lCtrFw            = 1;
        uOpenDeviceIn.uUserFw.bDebugMode        = (pOption->bDebug == LSA_TRUE);
		uOpenDeviceIn.uUserFw.bShmDebug			= (pOption->bShmDebug == LSA_TRUE);
		uOpenDeviceIn.uUserFw.bShmHaltOnStartup = (pOption->bShmHaltOnStartup == LSA_TRUE);
    }
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    #if (PSI_CFG_USE_EDDT == 1)
    else // bLoadFw = LSA_FALSE
    {
        // force firmware download at TI AM5728 ICSS in light variant
        if (   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_TI_AM5728)
            && (pBoard->Location.eInterfaceSelector == EPS_PNDEV_INTERFACE_2)) // TI ICSS?
        {
	        LSA_CHAR file_name[256];

            pOption->bLoadFw                  		= LSA_TRUE;
            uOpenDeviceIn.uUserFw.uFw[0].eSrc 		= ePNDEV_FW_SRC_FILE;
			uOpenDeviceIn.uUserFw.uFw[0].eFwType	= ePNDEV_FW_TYPE_ADONIS;

            #if ( PSI_CFG_USE_TCIP_OBSD == 1 )
            eps_strcpy(file_name, "ld_openbsd_am5728.elf");
            #else
            eps_strcpy(file_name, "ld_am5728.elf");
            #endif

	        if (eps_file_exist(file_name))
	        {
		        // file exists in current-directory, use it
		        fnConvertStringCharToTchar((LSA_CHAR * const)file_name, uOpenDeviceIn.uUserFw.uFw[0].as.uFile.sPath);
	        }
            else // file not exists
            {
		        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_open_device(): FW-File not existing");
                EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_APP_FW_NOT_AVAIL, EPS_EXIT_CODE_APP_FW_NOT_AVAIL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndevdrv_open_device(): FW-File not existing");
            }
        } 
    }
    #endif // (PSI_CFG_USE_EDDT == 1)
    #endif // (EPS_PLF == EPS_PLF_WINDOWS_X86)

    // This is to prevent the download of a bootloader for Boards which do not need a BootFW (e.g. I210)
    if (   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB200_PCIE)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200P)
        || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA))
    {
		if (file_tag_fw.FileType == EPS_APP_FILE_FW_LINUX)
			// the main firmware is linux, we do not need a bootloader
		{
			// set BootFW Source to Invalid, no need to set further params
			uOpenDeviceIn.uBootFw.eSrc		= ePNDEV_FW_SRC_INVALID;
			uOpenDeviceIn.uBootFw.eFwType	= ePNDEV_FW_TYPE_INVALID;
		}
		else if (file_tag_fw.FileType == EPS_APP_FILE_FW_ADONIS)
		{
    		file_tag_bootloader.FileType   = EPS_APP_FILE_BOOTLOADER;
    		file_tag_bootloader.isHD       = (LSA_BOOL)hd_id;
    		file_tag_bootloader.pndevBoard = eps_pndevdrv_get_eps_pndev_board_type(pBoard->pDeviceInfo->eBoard);
			uOpenDeviceIn.uBootFw.eSrc     = ePNDEV_FW_SRC_BUF;
			uOpenDeviceIn.uBootFw.eFwType  = ePNDEV_FW_TYPE_BOOT;

			if (pOption->bUseEpsAppToGetFile)
			{
				// use EPS APP to get the bootloader
				EPS_APP_ALLOC_FILE(&file_tag_bootloader);

				uOpenDeviceIn.uBootFw.as.uBuf.uBase.pPtr    = file_tag_bootloader.pFileData;
				uOpenDeviceIn.uBootFw.as.uBuf.lSize         = file_tag_bootloader.uFileSize;
			}
			else
			{
				// use the bootloader provided in the option structure
				EPS_ASSERT(pOption->pBootFwBufData != LSA_NULL);
				EPS_ASSERT(pOption->uBootFwBufSize > 0);

				uOpenDeviceIn.uBootFw.as.uBuf.uBase.pPtr    = pOption->pBootFwBufData;
				uOpenDeviceIn.uBootFw.as.uBuf.lSize         = pOption->uBootFwBufSize;
			}
		}
		else
		{
			EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_open_device(): Unsupported EPS_APP_FILE_TAG_ENUM used");
			EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_EPS_FATAL, EPS_EXIT_CODE_EPS_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndevdrv_open_device(): Unsupported EPS_APP_FILE_TAG_ENUM used");
		}
    }
    else
    {
        // set BootFW Source to Invalid, no need to set further params
        uOpenDeviceIn.uBootFw.eSrc                      = ePNDEV_FW_SRC_INVALID;
		uOpenDeviceIn.uBootFw.eFwType					= ePNDEV_FW_TYPE_INVALID;
    }

	if (pOption->bDebug || pOption->bShmDebug)
	{
		#ifdef _TEST
		uOpenDeviceIn.uUserFw.lTimeoutFwLoad_10msec			= READ_INI_FILE("Timeouts","FwLoad",0);
		uOpenDeviceIn.uUserFw.lTimeoutFwExe_10msec			= READ_INI_FILE("Timeouts","FwExe",0);
		uOpenDeviceIn.uUserFw.lTimeoutFwStartupStd_10msec	= READ_INI_FILE("Timeouts","FwStartupStd",0);
		uOpenDeviceIn.uUserFw.lTimeoutFwStartupDebug_10msec	= READ_INI_FILE("Timeouts","FwStartupDebug",0);
		uOpenDeviceIn.uUserFw.lTimeoutDebugger_10msec		= READ_INI_FILE("Timeouts","Debugger",0);
		uOpenDeviceIn.uUserFw.lTimeWaitDebugger_10msec		= READ_INI_FILE("Timeouts","WaitDebugger",0);
		uOpenDeviceIn.uUserFw.lTimeStartupSignOfLife_10msec	= READ_INI_FILE("Timeouts","StartupSignOfLife",0);
		uOpenDeviceIn.uUserFw.lTimeUpdateSignOfLife_10msec	= READ_INI_FILE("Timeouts","UpdateSignOfLife",0);
		#else
		// use default values of timeouts
		uOpenDeviceIn.uUserFw.lTimeoutFwLoad_10msec			= 10000;
		uOpenDeviceIn.uUserFw.lTimeoutFwExe_10msec			= 10000;
		uOpenDeviceIn.uUserFw.lTimeoutFwStartupStd_10msec	= 10000;
		uOpenDeviceIn.uUserFw.lTimeoutFwStartupDebug_10msec	= 10000;
		uOpenDeviceIn.uUserFw.lTimeoutDebugger_10msec		= 10000;
		uOpenDeviceIn.uUserFw.lTimeWaitDebugger_10msec		= 10000;
		uOpenDeviceIn.uUserFw.lTimeStartupSignOfLife_10msec	= 10000;
		uOpenDeviceIn.uUserFw.lTimeUpdateSignOfLife_10msec	= 10000;
		#endif
	}
    {
        bSetupEmpty = (pOption->bLoadFw) ? LSA_FALSE : LSA_TRUE;
        eps_pndevdrv_open_device_setup_cfg_record(pBoard, bSetupEmpty, hd_id);
    }
    //lint --e(506) --e(774) Constant value Boolean; Boolean within 'if' always evaluates to False - dynamic check to see if EPS_SHM_UPPER_CONFIG_TYPE fits into the ArrayUserData
	EPS_ASSERT(sizeof(EPS_SHM_UPPER_CONFIG_TYPE)<=sizeof(uOpenDeviceIn.uUserFw.lArrayUserData));
	eps_memcpy(uOpenDeviceIn.uUserFw.lArrayUserData, &pBoard->SharedMemConfig, sizeof(pBoard->SharedMemConfig));
	
    #if (EPS_PLF == EPS_PLF_ADONIS_X86)
	uOpenDeviceIn.uUserFw.lBatHostAddress = 0xF0000000; // reconfigure (soc1/cp1616) pci host bridge to make local apic registers available (used for IPC messages)
    #endif

    // call PNDEVDRV
	drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnOpenDevice(g_pEpsPnDevDrv->pDrvHandle, sizeof(uOpenDeviceIn), sizeof(*puOpenDeviceOut), &uOpenDeviceIn, puOpenDeviceOut);

	if (drvResult != ePNDEV_OK)
	{
        LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
        LSA_CHAR sMasterAppAlreadyExist[PNDEV_SIZE_STRING_BUF] = "fnPnDev_OpenDevice(): Master App already exist!";
        eps_convert_pndev_err_string(puOpenDeviceOut->sError, sErrConv);
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_open_device() %s", sErrConv);

        // Attention: Here the error "Master App already exist" is currently detected by string comparison! This can be made better in the future!
        if (eps_strcmp(sErrConv, sMasterAppAlreadyExist) == 0) // strings identical?
        {
            EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_MASTER_APP_ALREADY_EXIST, EPS_EXIT_CODE_MASTER_APP_ALREADY_EXIST, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndevdrv_open_device(): Master App already exist");
        }

		#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
		printf("\r\nEPS PnDevDrv Board Open failed! PCI Bus %d, Dev %d, Fct %d - (using FW (%d), SBL %d.%d.%d, Cnt Mac: %d)", (int)pBoard->Location.uBusNr, (int)pBoard->Location.uDeviceNr, (int)pBoard->Location.uFctNr, (int)pBoard->bBoardFwUsed, (int)puOpenDeviceOut->uSblInfo.uVersion.lMajor, (int)puOpenDeviceOut->uSblInfo.uVersion.lMinor, (int)puOpenDeviceOut->uSblInfo.uVersion.lBuild, (int)puOpenDeviceOut->uHwRes.uCommon.uRemanentData.lCtrMacAdr);
		printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__, (LSA_CHAR*)puOpenDeviceOut->sError);
	    #endif
	}
	else
	{
        LSA_UINT8* pSharedHostSdramBase = LSA_NULL;
        LSA_UINT32 lSharedHostSdramPhyAdr = 0x0;

        if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616)
            || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE))
            // Ertec400
        {
            pSharedHostSdramBase = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr;
            lSharedHostSdramPhyAdr = pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lPhyAdr;
        }
        else if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
            || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
			|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625))
            // Soc1
        {
            pSharedHostSdramBase = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr;
            lSharedHostSdramPhyAdr = pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lPhyAdr;
        }
        else if (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA)
            // HERA
        {
            pSharedHostSdramBase = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uHera.uSharedHostSdram.uBase.pPtr;
            lSharedHostSdramPhyAdr = pBoard->OpenDevice.uHwRes.as.uHera.uSharedHostSdram.lPhyAdr;
        }
        else if (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_TI_AM5728)
            // TI
        {
            pSharedHostSdramBase = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uSharedHostSdram.uBase.pPtr;
            lSharedHostSdramPhyAdr = pBoard->OpenDevice.uHwRes.as.uAM5728.uSharedHostSdram.lPhyAdr;
        }

		EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_device(): bUseSharedMemMode=%u, bUseRemoteMem=%u, uSharedHostSdram.pPtr=0x%x, uSharedHostSdram.lPhyAdr=0x%x", 
							pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode,
							pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem,
                            pSharedHostSdramBase,
                            lSharedHostSdramPhyAdr);

		if (pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode)
		{
			if (pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem)
			{
				// Use HIF Host Memory => use the first 2MB of uPciHostMemOffsetDevice for HIF
				EPS_ASSERT(pSharedHostSdramBase != LSA_NULL);

				pBoard->Hif.HostMem.pLocalAddress                 = pSharedHostSdramBase;
				pBoard->SharedMemConfig.HIF.HostRam.uPhyAddress   = lSharedHostSdramPhyAdr;
			}
		}
	}

    //Free buffers allocated by EPS_APP_ALLOC_FILE
    if (pOption->bUseEpsAppToGetFile)
    {
        //Free the user firmware buffer by calling EPS_APP
        if (pOption->bLoadFw)
        {
            EPS_APP_FREE_FILE(&file_tag_fw);
        }

        // free the boot firmware buffer by calling EPS_APP
        // This is to prevent the FREE_FILE of a BootFW for Boards which do not need a BootFW (e.g. I210)
        if (    (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616)
            ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE)
            ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
            ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
    		||	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625)
            ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200)
    		||	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB200_PCIE)
            ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200P)
            ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
    		||	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA))
        {
			if (file_tag_fw.FileType != EPS_APP_FILE_FW_LINUX)
				// for linux we do not have to call it, because it was not allocated
			{
                EPS_APP_FREE_FILE(&file_tag_bootloader);
            }
        }
    }

    // set base address of the lower trace timer as well as the offsets required by eps_pndevdrv_write_lower_time
    if (    (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
        ||  (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
		||	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1625)
        )
    {
		pBoard->Pntrc.pTraceTimerUnitBase = pBoard->OpenDevice.uHwRes.as.uSoc1.uTtu.uBase.pPtr;
        pBoard->Pntrc.lowOffset           = PNDEVDRV_SOC1_OFFSET_TTU_LOW;
        pBoard->Pntrc.highOffset          = PNDEVDRV_SOC1_OFFSET_TTU_HIGH;
    }
    else if (
                (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200P)
             || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
            )
    {
        pBoard->Pntrc.pTraceTimerUnitBase = pBoard->OpenDevice.uHwRes.as.uErtec200P.uTtu.uBase.pPtr;
        pBoard->Pntrc.lowOffset           = PNDEVDRV_EB200P_OFFSET_TTU_LOW;
        pBoard->Pntrc.highOffset          = PNDEVDRV_EB200P_OFFSET_TTU_HIGH;
    }
	else if(
		       (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616)
		    || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE)
		   )
	{
		pBoard->Pntrc.pTraceTimerUnitBase = pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uTimer0_1.uBase.pPtr;
		pBoard->Pntrc.lowOffset           = PNDEVDRV_ERTEC400_OFFSET_TTU_LOW;
        pBoard->Pntrc.highOffset          = 0; // INVALID, there is no high value
	}
	else if (
        		(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200)
			 || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB200_PCIE)
	        )
	{
		pBoard->Pntrc.pTraceTimerUnitBase = pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uTimer0_2.uBase.pPtr;
		pBoard->Pntrc.lowOffset           = PNDEVDRV_ERTEC200_OFFSET_TTU_LOW;
        pBoard->Pntrc.highOffset          = 0; // INVALID, there is no high value
	}
	else if (
			 (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA)
            )
    {
		pBoard->Pntrc.pTraceTimerUnitBase = pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uTimerA_0_5.uBase.pPtr;
        pBoard->Pntrc.lowOffset           = 0;
        pBoard->Pntrc.highOffset          = 0;
    }
    else if (
             (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_TI_AM5728)
            )
    {
        pBoard->Pntrc.pTraceTimerUnitBase = pBoard->OpenDevice.uHwRes.as.uAM5728.uTimer.uBase.pPtr;
        pBoard->Pntrc.lowOffset           = PNDEVDRV_AM5728_OFFSET_TTU_LOW;
        pBoard->Pntrc.highOffset          = 0; // invalid
    }
	else
	{
        // Not supported
		pBoard->Pntrc.pTraceTimerUnitBase = LSA_NULL;
	    pBoard->Pntrc.lowOffset           = 0;
        pBoard->Pntrc.highOffset          = 0;
	}

	return drvResult;
}

/**
 * initialize and set parameters for hif communication and open hif driver
 *
 * @param [in] pBoard           pointer to corresponding board structue
 * @return 	EPS_HIF_DRV_RET_OK
 */
static LSA_VOID eps_pndevdrv_open_hif_driver( EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard )
{
	LSA_UINT16               retVal;
	EPS_HIF_DRV_OPENPAR_TYPE InPars;

	EPS_ASSERT(g_pEpsPnDevDrv != LSA_NULL);

	if (pBoard->bBoardFwUsed) /* HIF Driver is only installed if we are working with an Firmware equipped Board */
	{
		eps_memset(&InPars, 0, sizeof(InPars));

		/* this function is related to eps_pndevdrv_open_device_setup_cfg_record() */

		InPars.Mode                = EPS_HIF_DRV_HIF_TM_SHARED_MEM;
	    InPars.if_pars.SendIrqMode = HIF_SEND_IRQ_AFTER_READ_BACK; // read back before sending irqs to pnboards, to prevent from unecassary hif_requests

		if( (pBoard->SharedMemConfig.bValid == 1) &&
			(pBoard->SharedMemConfig.HIF.bValid == LSA_TRUE) &&
			(pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem == 1) )
		{
			/* Host Memory used => fill in addresses */
			InPars.if_pars.SharedMem.Local.Base   = pBoard->Hif.HostMem.pLocalAddress;
			InPars.if_pars.SharedMem.Local.Size   = pBoard->SharedMemConfig.HIF.HostRam.uSize;
			InPars.if_pars.SharedMem.Remote.Base  = pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr;
			InPars.if_pars.SharedMem.Remote.Size  = pBoard->sHw.EpsBoardInfo.hif_mem.size;
		}
		else
		{
			/* Legacy = No Interrupts / No Host Memory (splitted shared memory) / Shared Memory */
			pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode = LSA_TRUE; /* That's needed for selecting a valid HIF Driver */
		    InPars.if_pars.SharedMem.Local.Base                  = pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr;
		    InPars.if_pars.SharedMem.Local.Size                  = pBoard->sHw.EpsBoardInfo.hif_mem.size/2;
		    InPars.if_pars.SharedMem.Remote.Base                 = pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr + (pBoard->sHw.EpsBoardInfo.hif_mem.size/2);
		    InPars.if_pars.SharedMem.Remote.Size                 = pBoard->sHw.EpsBoardInfo.hif_mem.size/2;
		}

		if( (pBoard->SharedMemConfig.bValid == 1) &&
			(pBoard->SharedMemConfig.HIF.bValid == LSA_TRUE) &&
			(pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts == 1) )
		{
			/* IPC interrupts => fill in already allocated handles */
			InPars.drv_type_specific.pars.ipc.bUsehIpcRx = LSA_TRUE;
			InPars.drv_type_specific.pars.ipc.hIpcRx     = pBoard->Hif.Ipc.Rx.uRxHandle;
		}

		retVal = eps_hif_drv_if_open(pBoard->sysDev.hd_nr, LSA_TRUE /*bUpper*/, &pBoard->SharedMemConfig.HIF.HifCfg, &InPars, &pBoard->pHifDrvHwInstOut);

		EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
	}
}

/**
 * close hif driver
 *
 * @param [in] pBoard               pointer to corresponding board structue
 */
static LSA_VOID eps_pndevdrv_close_hif_driver( EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE const pBoard )
{
	LSA_UINT16 retVal;

	if (pBoard->bBoardFwUsed)
	{
		retVal = eps_hif_drv_if_close(pBoard->sysDev.hd_nr, LSA_TRUE /*bUpper*/);

		EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
	}
}

/**
 * startup PnDevDriver (open DLL)
 *
 * @param LSA_VOID
 * @return 	ePNDEV_OK
 */
static LSA_BOOL eps_pndevdrv_startup( LSA_VOID )
{
	PNDEV_CHAR              drvError[PNDEV_SIZE_STRING_BUF];
	ePNDEV_RESULT           eResult    = ePNDEV_FATAL;
	LSA_BOOL                bSuccess   = LSA_FALSE;
	EPS_APP_DRIVER_PATH_ARR uPathArr;
	LSA_UINT32  lLoopCount;

	eps_memset(&uPathArr, 0, sizeof(uPathArr));

	EPS_APP_GET_DRIVER_PATH(&uPathArr, EPS_APP_DRIVER_PNDEVDRV);

    // Try to startup PnDevDrv DLL from the given paths
	// note: Function pointers and version check is internal done by with driver startup service
	for (lLoopCount = 0; lLoopCount < EPS_APP_DRIVER_PATH_MAX_ARR_SIZE; lLoopCount++)
	{
	    PNDEV_CHAR drvPath[EPS_APP_PATH_MAX_LENGTH];

	    eps_convert_pndev_path(uPathArr.Path[lLoopCount].sPath, drvPath);

	    eResult = fnPnDev_StartUpDriver(drvPath,
			                            PNDEV_SIZE_STRING_BUF,
			                            &g_pEpsPnDevDrv->pDrvHandle,
			                            drvError);

	    if (eResult == ePNDEV_OK)
	        // Startup successfull
		{
	        break;
	    }
	}

	if (eResult == ePNDEV_OK)
	{
		bSuccess = LSA_TRUE;
	}
	else
	{
        LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
        eps_convert_pndev_err_string(drvError, sErrConv);
		EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS PnDevDrv %s", sErrConv);

		#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
		printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__, (LSA_CHAR*)drvError);
		#endif

		// For Adonis no fatal is called
        #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000))
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_DRIVER_NOT_FOUND, EPS_EXIT_CODE_DRIVER_NOT_FOUND, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_pndevdrv_startup() - Couldn't load PnDevDrv-DLL");
        #endif
	}

	return bSuccess;
}

/**
 * uninstall PnDevDriver
 *
 * @param	LSA_VOID
 * @return 	LSA_VOID
 */
static LSA_VOID eps_pndevdrv_uninstall(LSA_VOID)
{
	PNDEV_CHAR drvError[PNDEV_SIZE_STRING_BUF];

    EPS_ASSERT(g_pEpsPnDevDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsPnDevDrv->bInit == LSA_TRUE);

	if ( g_pEpsPnDevDrv->pDrvHandle != 0 )
	{
		ePNDEV_RESULT eResult;

		eps_memset( drvError, 0, sizeof( drvError ));

		// ShutDownDriver (disconnect driver connection)
		// Note: DLL cleanup (unload, cleanup Fct list) is done by the driver service
		//       Tracing not done, possible already disabled
		eResult = fnPnDev_ShutDownDriver(
			g_pEpsPnDevDrv->pDrvHandle,
			PNDEV_SIZE_STRING_BUF,
			drvError );

		if (eResult != ePNDEV_OK)
		{
            LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
            eps_convert_pndev_err_string(drvError, sErrConv);
			EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS PnDevDrv %s", sErrConv);

			#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
			printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__, (LSA_CHAR*)drvError);
		    #endif
		}

		EPS_ASSERT( eResult == ePNDEV_OK );
	}

	g_pEpsPnDevDrv->pDrvHandle = 0;

	eps_pndevdrv_undo_init_critical_section();

    eps_pndevdrv_timer_threads_undo_init();
    eps_pndevdrv_event_isr_undo_init();
    eps_pndevdrv_helper_undo_init();

	g_pEpsPnDevDrv->bInit = LSA_FALSE;

	g_pEpsPnDevDrv = LSA_NULL;
}

/**
 * Installing the EPS PnDevDriver as a EPS PN Driver IF implementation.
 *
 * The EPS EPS PnDevDriver is a EPS PN Dev Driver implementation (@see eps_pndev_if_open). This driver requires the kernel mode part PnDevDriver to be installed in the system (see PnDevDriver docu).
 *
 * UseCases:
 *     EPS Light    (LD / HD part)
 *     EPS Basic    (App part or App/LD part)
 *     EPS Advanced (LD Part or LD/HD part)
 *
 * HIF Integration:
 *    @see eps_pndevdrv_open_device_setup_cfg_record
 *
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_pndevdrv_open
 *  eps_pndev_if_close     -> eps_pndevdrv_close
 *  eps_pndev_if_uninstall -> eps_pndevdrv_uninstall
 *
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pndevdrv_install( LSA_VOID )
{
	EPS_PNDEV_IF_TYPE sPnDevDrvIf;
	LSA_BOOL          bIfLoadOk;

	// Init Internals
	eps_memset(&g_EpsPnDevDrv, 0, sizeof(g_EpsPnDevDrv));
	g_pEpsPnDevDrv = &g_EpsPnDevDrv;

	bIfLoadOk = eps_pndevdrv_startup();

    EPS_ASSERT(g_pEpsPnDevDrv->pDrvHandle != LSA_NULL);

    // Must be called after eps_pndevdrv_startup()
	eps_pndevdrv_timer_threads_init(g_pEpsPnDevDrv->pDrvHandle);
    eps_pndevdrv_helper_init(g_pEpsPnDevDrv->pDrvHandle);
    eps_pndevdrv_event_isr_init(g_pEpsPnDevDrv->pDrvHandle);

	if (!bIfLoadOk)
	{
		#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
		printf("\r\n");
		printf("EPS PnDevDrv install failed!\r\n");
		printf("    => PnDevDrv Dll available? (Windows only)\r\n");
		printf("    => PnDevDrv Boards are in (startup) reset state?\r\n");
		printf("    => correct Bootloader version installed on your PnDevDrv Boards?\r\n");
		#endif

		return;
	}

    #ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
	printf("\r\n");
	printf("EPS PnDevDrv (%c%02d.%02d.%02d.%02d_%02d.%02d.%02d.%02d) install done\r\n",
		g_pEpsPnDevDrv->pDrvHandle->uVersion.kind,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.version,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.distribution,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.fix,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.hotfix,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.project_number,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.increment,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.integration_counter,
		g_pEpsPnDevDrv->pDrvHandle->uVersion.gen_counter);
    #endif

	eps_pndevdrv_init_critical_section();
	eps_pndevdrv_get_device_infos(); // Find all devices
	eps_pndevdrv_register_device_infos();

	// Init Interface
	sPnDevDrvIf.open      = eps_pndevdrv_open;
	sPnDevDrvIf.close     = eps_pndevdrv_close;
	sPnDevDrvIf.uninstall = eps_pndevdrv_uninstall;

	g_pEpsPnDevDrv->bInit = LSA_TRUE;

	eps_pndev_if_register(&sPnDevDrvIf);
}

/**
 * open pntrc lower cpu
 *
 * @param [in] pBoard	pointer to current board structur
 * @param [in] pOption	ptr to options, if FirmwareDownload and Debugging will be executed
 * @return 	LSA_VOID
 */
static LSA_VOID eps_pndevdrv_open_pntrc_lower_cpu( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption )
{
	if (pOption != LSA_NULL)
	{
		if (pOption->bLoadFw)
		{
			pBoard->Pntrc.hPntrcCPU = pntrc_open_lower_cpu((PNTRC_SYS_HANDLE_LOWER_PTR_TYPE)&pBoard->sHw);
			pBoard->Pntrc.bOpened   = LSA_TRUE;
		}
	}
}

/**
 * close pntrc lower cpu
 *
 * @param [in] pBoard	    pointer to current board structur
 * @return 	LSA_VOID
 */
static LSA_VOID eps_pndevdrv_close_pntrc_lower_cpu( EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard )
{
	if (pBoard->Pntrc.bOpened)
	{
		pntrc_close_lower_cpu(pBoard->Pntrc.hPntrcCPU);
		pBoard->Pntrc.bOpened = LSA_FALSE;
	}
}

/**
 * Open a PN board supported by the PnDevDriver for the given PCI Location.
 *
 * This function implements the eps_pndev_if function eps_pndev_if_open.
 * The function may only be called once since there is only one HD.
 *
 * @see eps_pndev_if_open             - calls this function
 *
 * @param	[in] pLocation 		            ptr including PciLocation
 * @param	[in] pOption			        ptr to options, if FirmwareDownload and Debugging will be executed
 * @param	[out] ppHwInstOut		        pointer to ptr to structure with hardware configuration
 * @param 	[in] hd_id			            index of hd
 * @return 	EPS_PNDEV_RET_OK                everything went better then expected.
 * @return  EPS_PNDEV_RET_DEV_NOT_FOUND     device was not found (wrong PCI location?)
 * @return  EPS_PNDEV_RET_UNSUPPORTED       device is supported by PnDevDriver, but not by EPS yet.
 */
LSA_UINT16 eps_pndevdrv_open( EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id )
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT32                  uStoreIndex;
	ePNDEV_RESULT               drvResult;
    LSA_UINT32                  HD_Open_Device_Ctr = 0; //number of HD open devices at current board
	LSA_UINT16                  result             = EPS_PNDEV_RET_OK;
    LSA_UINT32                  retValPntrc;
	EPS_ASSERT(g_pEpsPnDevDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

    retValPntrc = PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    EPS_ASSERT(0 == retValPntrc);
	eps_pndevdrv_enter();

	pBoard = eps_pndevdrv_alloc_board(pLocation, &HD_Open_Device_Ctr);
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */
	EPS_ASSERT(HD_Open_Device_Ctr);

    if //the first device of a board is just opening?
       (HD_Open_Device_Ctr == 1UL)
	{
	    pBoard->pDeviceInfo = LSA_NULL;

	    if (pLocation->eLocationSelector == EPS_PNDEV_LOCATION_PCI)
	    {
		    uStoreIndex = eps_pndevdrv_get_device(pLocation, &pBoard->pDeviceInfo); // check board already known
            LSA_UNUSED_ARG(uStoreIndex); // pBoard->pDeviceInfo is checked instead
	    }

        if (pBoard->pDeviceInfo == LSA_NULL)
        {
		    result = EPS_PNDEV_RET_DEV_NOT_FOUND;
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_open(): Error: EPS_PNDEV_RET_DEV_NOT_FOUND");
        }

	    if (result == EPS_PNDEV_RET_OK)
	    {
		    drvResult = eps_pndevdrv_open_device(pBoard, pOption, &pBoard->OpenDevice, hd_id); // uses pBoard->pDeviceInfo!
		    if (ePNDEV_OK != drvResult)
		    {
			    result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_open(): Error: EPS_PNDEV_RET_DEV_OPEN_FAILED");
		    }
        }
    }

	#ifdef _TEST
	pOption->pBaseAdr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.uBase.pPtr;
	#endif //_TEST

	if (result == EPS_PNDEV_RET_OK)
	{
		switch (pBoard->OpenDevice.uHwInfo.eAsic)
		{
            #if ( PSI_CFG_USE_EDDI == 1 )
			case ePNDEV_ASIC_ERTEC400:
				result = eps_pndevdrv_open_ertec400(pBoard, pOption, hd_id);
				break;
			case ePNDEV_ASIC_ERTEC200:
				result = eps_pndevdrv_open_ertec200(pBoard, pOption, hd_id);
				break;
			case ePNDEV_ASIC_SOC1:
				result = eps_pndevdrv_open_soc1(pBoard, pOption, hd_id);
				break;
            #endif //PSI_CFG_USE_EDDI
            #if ( PSI_CFG_USE_EDDP == 1 )
			case ePNDEV_ASIC_ERTEC200P:
				result = eps_pndevdrv_open_eb200p(pBoard, pOption, hd_id);
				break;
			case ePNDEV_ASIC_HERA:
				result = eps_pndevdrv_open_hera(pBoard, pOption, hd_id);
				break;
            #endif //PSI_CFG_USE_EDDP
			#if (PSI_CFG_USE_EDDS == 1)
            #if defined(PSI_EDDS_CFG_HW_INTEL)
			case ePNDEV_ASIC_I210:
				result = eps_pndevdrv_open_i210(pBoard, hd_id);
				break;
            case ePNDEV_ASIC_I210IS:
				result = eps_pndevdrv_open_i210IS(pBoard, hd_id);
				break;
            case ePNDEV_ASIC_I82574:
                result = eps_pndevdrv_open_i82574(pBoard, hd_id);
                break;
			#endif
			#if defined(PSI_EDDS_CFG_HW_KSZ88XX)
            case ePNDEV_ASIC_KSZ8841:
                result = eps_pndevdrv_open_ksz8841(pBoard, hd_id);
                break;
            case ePNDEV_ASIC_KSZ8842:
                result = eps_pndevdrv_open_ksz8842(pBoard, hd_id);
                break;
            #endif
            #if defined (PSI_EDDS_CFG_HW_IOT2000)
            case ePNDEV_ASIC_IX1000:
                result = eps_pndevdrv_open_iot2000(pBoard, hd_id);
                break;
            #endif
            #endif //PSI_CFG_USE_EDDS
            //special case TI AM5728: GMAC or ICSS
			#if (((PSI_CFG_USE_EDDS == 1) && defined(PSI_EDDS_CFG_HW_TI)) || (PSI_CFG_USE_EDDT == 1))
            case ePNDEV_ASIC_AM5728:
			    #if ((PSI_CFG_USE_EDDS == 1) && defined(PSI_EDDS_CFG_HW_TI))
                if (pLocation->eInterfaceSelector == EPS_PNDEV_INTERFACE_1)  // TI GMAC?
                {
                    result = eps_pndevdrv_open_am5728_gmac(pBoard, pOption, hd_id);
                    break;
                }
                #endif
			    #if (PSI_CFG_USE_EDDT == 1)
                if (pLocation->eInterfaceSelector == EPS_PNDEV_INTERFACE_2) // TI ICSS?
                {
                    result = eps_pndevdrv_open_am5728_icss(pBoard, pOption, hd_id);
                    break;
                }
                #endif
				result = EPS_PNDEV_RET_UNSUPPORTED;
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_open(): Error at ePNDEV_ASIC_AM5728: EPS_PNDEV_RET_UNSUPPORTED, invalid eInterfaceSelector(%u)", pLocation->eInterfaceSelector);
                break;
            #endif
			default:
				result = EPS_PNDEV_RET_UNSUPPORTED;
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_open(): Error: EPS_PNDEV_RET_UNSUPPORTED, invalid eAsic(%u)", pBoard->OpenDevice.uHwInfo.eAsic);
            //lint --e(788) enum constant not used within defaulted switch - some of the cases are only visible when compiler switches are set.
		}
	}

	if (result == EPS_PNDEV_RET_OK)
	{
        LSA_UINT16 retValFileTransfer;
        if //the first device of a board is just opening?
           (HD_Open_Device_Ctr == 1UL)
        {
            // load shm file transfer memory and service thread
            switch (pBoard->OpenDevice.uHwInfo.eAsic)
            {
                #if (PSI_CFG_USE_EDDP == 1)
                case ePNDEV_ASIC_ERTEC200P:
                case ePNDEV_ASIC_HERA:
                {
                    if (pOption->bLoadFw)
                    {
                    
                        // start service thread
                        retValFileTransfer = eps_pndevdrv_create_file_transfer_poll_thread(pBoard);
                        EPS_ASSERT(EPS_PNDEV_RET_OK == retValFileTransfer);
                    }
                    break;
                }
                #endif
                #if (((PSI_CFG_USE_EDDS == 1) && defined(PSI_EDDS_CFG_HW_TI)) || (PSI_CFG_USE_EDDT == 1))
                case ePNDEV_ASIC_AM5728:
                {
                    if (pOption->bLoadFw)
                    {
                        // start service thread
                        retValFileTransfer = eps_pndevdrv_create_file_transfer_poll_thread(pBoard);
                        EPS_ASSERT(EPS_PNDEV_RET_OK == retValFileTransfer);
                    }
                    break;
                }
                #endif
                default: 
                    LSA_UNUSED_ARG(retValFileTransfer);
                    break;
                    //lint --e(788) enum constant not used within defaulted switch - some of the cases are only visible when compiler switches are set.
            }

		    eps_pndevdrv_open_pntrc_lower_cpu(pBoard, pOption);

            eps_pndevdrv_open_hif_driver(pBoard);
        }

		#ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
		eps_pndevdrv_printf_openclose_board(LSA_TRUE, pBoard);
		#endif

		*ppHwInstOut = &pBoard->sHw;
	}
	else
	{
		eps_pndevdrv_free_board(pBoard);
	}

	eps_pndevdrv_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
	return result;
	//lint --e(818) Pointer parameter 'pOption' could be declared as pointing to const. It contains an output value that is set when _TEST is active. Required by API
}

/**
 * close device with PnDevDriver
 *
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 *
 * @see eps_pndev_if_close          - calls this function
 *
 * @param	[in] pHwInstIn		    pointer to structure with hardware configuration
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_close( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
	ePNDEV_RESULT                 drvResult;
	uPNDEV_CLOSE_DEVICE_IN        uCloseDeviceIn;
	uPNDEV_CLOSE_DEVICE_OUT       uCloseDeviceOut;
	EPS_PNDEVDRV_BOARD_PTR_TYPE   pBoard;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
    LSA_UINT32                    retValPntrc;
    LSA_UINT16                    retValDisableIsr;
    #if ((PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
    LSA_UINT32                    HD_Open_Device_Ctr; //number of HD open devices at current board
    #endif

	EPS_ASSERT(pHwInstIn != LSA_NULL);

    retValPntrc = PNTRC_ENTER_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129
    EPS_ASSERT(0 == retValPntrc);
	eps_pndevdrv_enter();

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pBoard->bUsed);

    #if ((PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
    switch (pBoard->OpenDevice.uHwInfo.eAsic) //ASIC type of the closing board
    {
        case ePNDEV_ASIC_HERA:   //HERA board:      2 board entries are allowed because of the 2 PNIPs
        case ePNDEV_ASIC_AM5728: //TI-AM5728 board: 2 board entries are allowed because of GMAC and ICSS
        {
	        LSA_UINT32 Ctr;

            HD_Open_Device_Ctr = 0;

            //check all allocated board entries in the internal management structure
	        for (Ctr=0; Ctr<EPS_CFG_PNDEVDRV_MAX_BOARDS; Ctr++)
	        {
	            EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard2 = &g_pEpsPnDevDrv->board[Ctr];

		        if //same board (same PCI location) found twice?
                   (   (pBoard2 == pBoard)
                    || (   (pBoard2->bUsed)
                        && (pBoard2->Location.eLocationSelector == pBoard->Location.eLocationSelector)
                        && (pBoard2->Location.eLocationSelector == EPS_PNDEV_LOCATION_PCI)
                        && (pBoard2->Location.uBusNr            == pBoard->Location.uBusNr)
                        && (pBoard2->Location.uDeviceNr         == pBoard->Location.uDeviceNr)
                        && (pBoard2->Location.uFctNr            == pBoard->Location.uFctNr)))
                {
                    HD_Open_Device_Ctr++;
                    if (HD_Open_Device_Ctr >= 2UL)
                    {
                        break; //leave loop - runtime-optimization
                    }
                }
            }
	        EPS_ASSERT(HD_Open_Device_Ctr);
            break;
        }
        default: //another board
        {
            HD_Open_Device_Ctr = 1UL;
        }
        //lint --e(788) enum constant not used within defaulted switch - some of the cases are only visible when compiler switches are set.
    }
    #endif

    #if ((PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
    if //the last device of a board is just closing?
       (HD_Open_Device_Ctr == 1UL)
    #endif
    {
	    eps_pndevdrv_close_pntrc_lower_cpu(pBoard);

	    eps_pndevdrv_close_hif_driver(pBoard);
    }

	if (pBoard->uCountIsrEnabled != 0)
	{
		if (pBoard->sIsrIsochronous.pCbf != LSA_NULL)
		{
			IsrDesc = EPS_PNDEV_ISR_ISOCHRONOUS;
			retValDisableIsr = eps_pndevdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
            EPS_ASSERT(EPS_PNDEV_RET_OK == retValDisableIsr);
		}

		if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
		{
			IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
            retValDisableIsr = eps_pndevdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
            EPS_ASSERT(EPS_PNDEV_RET_OK == retValDisableIsr);
		}

		if (pBoard->sIsrPnNrt.pCbf != LSA_NULL)
		{
			IsrDesc = EPS_PNDEV_ISR_PN_NRT;
            retValDisableIsr = eps_pndevdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
            EPS_ASSERT(EPS_PNDEV_RET_OK == retValDisableIsr);
		}
	}

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

	if (pBoard->TimerCtrl.bRunning)
	{
        LSA_UINT16 retValTimerStop;
        retValTimerStop = eps_pndevdrv_timer_ctrl_stop_intern(pHwInstIn);
        EPS_ASSERT(EPS_PNDEV_RET_OK == retValTimerStop);
	}

	EPS_ASSERT(!pBoard->TimerCtrl.bRunning);

	switch (pBoard->OpenDevice.uHwInfo.eAsic)
	{
        // "PSI_CFG_EDDI_CFG_ERTEC_400" is only defined by "EPS_PLF_ERTEC400_ARM9", so the (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
        #if (((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86))
		case ePNDEV_ASIC_ERTEC400:
			eps_undo_init_hw_ertec400(pBoard->sysDev.hd_nr);
			break;
        #endif
        // "PSI_CFG_EDDI_CFG_ERTEC_200" is only defined by "EPS_PLF_ERTEC200_ARM9", so the (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
        #if (((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_200))) || (EPS_PLF == EPS_PLF_ADONIS_X86))
		case ePNDEV_ASIC_ERTEC200:
			eps_undo_init_hw_ertec200(pBoard->sysDev.hd_nr);
			break;
        #endif
        #if ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_SOC)))
		case ePNDEV_ASIC_SOC1:
			eps_undo_init_hw_SOC(pBoard->sysDev.hd_nr);
			break;
        #endif
		default: /* nothing to do */
            break;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	eps_memset(&uCloseDeviceIn,  0, sizeof(uCloseDeviceIn));
	eps_memset(&uCloseDeviceOut, 0, sizeof(uCloseDeviceOut));

	uCloseDeviceIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
	uCloseDeviceIn.bDoAnyway       = LSA_FALSE;

    #if ((PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
    if //the last device of a board is just closing?
       (HD_Open_Device_Ctr == 1UL)
    #endif
    {
        // stop file transfer thread and undo init shm
        switch (pBoard->OpenDevice.uHwInfo.eAsic)
        {
            #if (PSI_CFG_USE_EDDP == 1)
            case ePNDEV_ASIC_ERTEC200P:
            case ePNDEV_ASIC_HERA:
            {
                if (pBoard->bBoardFwUsed)
                {
                    eps_pndevdrv_kill_file_transfer_poll_thread(pBoard);
                }
                break;
            }
            #endif
            #if (((PSI_CFG_USE_EDDS == 1) && defined(PSI_EDDS_CFG_HW_TI)) || (PSI_CFG_USE_EDDT == 1))
            case ePNDEV_ASIC_AM5728:
            {
                if (pBoard->bBoardFwUsed)
                {
                    eps_pndevdrv_kill_file_transfer_poll_thread(pBoard);
                }
                break;
            }
            #endif
            default: 
            break;
            //lint --e(788) enum constant not used within defaulted switch - some of the cases are only visible when compiler switches are set.
        }

        //call PNDEVDRV
	    drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCloseDevice(g_pEpsPnDevDrv->pDrvHandle, sizeof(uCloseDeviceIn), sizeof(uCloseDeviceOut), &uCloseDeviceIn, &uCloseDeviceOut);
	if(drvResult != ePNDEV_OK)
	    {
	    uPNDEV_CLOSE_DEVICE_OUT* pOut = &uCloseDeviceOut;
	        LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
            eps_convert_pndev_err_string(pOut->sError, sErrConv);
            EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_close() %s", sErrConv);
	        EPS_FATAL("Error at eps_pndevdrv_close - drvResult != ePNDEV_OK");
	    }
	}

    #ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
    eps_pndevdrv_printf_openclose_board(LSA_FALSE, pBoard);
    #endif

	eps_pndevdrv_free_board(pBoard);

	eps_pndevdrv_exit();
    PNTRC_EXIT_SERVICE(); // Hack -> Prevents Deadlock with PNTRC -> will be fixed in RQ 1071129

	return EPS_PNDEV_RET_OK;
}

/**
 * read trace data
 *
 * @param [in]  pHwInstIn 			pointer to structure with hardware configuration
 * @param [in]  offset				offset where should be read from
 * @param [in]  ptr					destination pointer for the data
 * @param [in]  size				size of data
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_read_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{
	ePNDEV_RESULT drvResult;
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
    LSA_UINT16 retValFree;
	LSA_UINT32 lreadHostBufSize = size;
	LSA_UINT32 lSizeBufOut      = sizeof( uPNDEV_COPY_DATA_OUT ) + lreadHostBufSize;
	LSA_UINT8* pBufOut;
	uPNDEV_COPY_DATA_OUT* pCopyDataOut;
	LSA_UINT8* pHostBuf;
	uPNDEV_COPY_DATA_IN copyDataIn;
	LSA_UINT32 PntrcAsicOffset = 0;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(ptr != LSA_NULL);

	eps_pndevdrv_enter();

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard->bUsed);

	// allocate output buffer
	pBufOut = (LSA_UINT8*)eps_mem_alloc( lSizeBufOut, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT( pBufOut != 0 );

	pCopyDataOut = (uPNDEV_COPY_DATA_OUT*) EPS_CAST_TO_VOID_PTR(pBufOut);
	pHostBuf = pBufOut + sizeof( uPNDEV_COPY_DATA_OUT );

	eps_memset( &copyDataIn, 0, sizeof( copyDataIn ));

	switch( pBoard->OpenDevice.uHwInfo.eAsic )
	{
	    case ePNDEV_ASIC_ERTEC400:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_ERTEC200:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_SOC1:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_ERTEC200P:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_HERA:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwTrace[0].lOffset;
		    break;
        case ePNDEV_ASIC_AM5728:
            PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwTrace[0].lOffset;
            break;
    case ePNDEV_ASIC_I210:
    case ePNDEV_ASIC_I210IS:
    case ePNDEV_ASIC_I82574:
    case ePNDEV_ASIC_IX1000:
    case ePNDEV_ASIC_KSZ8841:
    case ePNDEV_ASIC_KSZ8842:
    case ePNDEV_ASIC_INVALID:
        default:
		EPS_FATAL("Board not supported");
	}

	copyDataIn.eHwRes  = ePNDEV_COPY_HW_RES_SDRAM;
	copyDataIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;

	copyDataIn.uArrayBlockRead[0].lOffsetAsic        = PntrcAsicOffset + offset;
	copyDataIn.uArrayBlockRead[0].lOffsetHostDataBuf = 0;
	copyDataIn.uArrayBlockRead[0].lSize              = lreadHostBufSize;

	drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCopyData(
		g_pEpsPnDevDrv->pDrvHandle,
		sizeof( copyDataIn ),
		lSizeBufOut,
		&copyDataIn,
		pCopyDataOut );

	EPS_ASSERT( drvResult == ePNDEV_OK );

	eps_memcpy( ptr, pHostBuf, size );

    retValFree = eps_mem_free( pBufOut, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
    EPS_ASSERT(LSA_RET_OK == retValFree);
	eps_pndevdrv_exit();

	return result;
}

/**
 * read trace data
 *
 * @param [in]  pHwInstIn 			pointer to structure with hardware configuration
 * @param [in]  ptr					source pointer for the data
 * @param [out] offset				offset where should be write to
 * @param [in]  size				size of data
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_write_trace_data( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const*  ptr, LSA_UINT32 offset, LSA_UINT32 size )
{
	ePNDEV_RESULT drvResult;
	uPNDEV_COPY_DATA_IN copyDataIn;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
    LSA_UINT16 retValFree;
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT32 lWriteHostBufSize             = size;
	LSA_UINT32 lWriteBlock0OffsetHostDataBuf = 0;
	LSA_UINT32 lSizeBufOut = sizeof( uPNDEV_COPY_DATA_OUT ) + lWriteHostBufSize;
	LSA_UINT8* pBufOut;
	uPNDEV_COPY_DATA_OUT *pCopyDataOut;
	LSA_UINT8* pHostBuf;
	LSA_UINT32 PntrcAsicOffset = 0;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(ptr != LSA_NULL);

	eps_pndevdrv_enter();

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard->bUsed);

	// allocate output buffer
	pBufOut = (LSA_UINT8*)eps_mem_alloc( lSizeBufOut, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT( pBufOut != 0 );

	pCopyDataOut = (uPNDEV_COPY_DATA_OUT*) EPS_CAST_TO_VOID_PTR(pBufOut);
	pHostBuf = pBufOut + sizeof( uPNDEV_COPY_DATA_OUT );

	eps_memcpy( pHostBuf, ptr, size );
	eps_memset( &copyDataIn, 0, sizeof( copyDataIn ) );

	switch( pBoard->OpenDevice.uHwInfo.eAsic )
	{
	    case ePNDEV_ASIC_ERTEC400:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_ERTEC200:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_SOC1:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_ERTEC200P:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwTrace[0].lOffset;
		    break;
	    case ePNDEV_ASIC_HERA:
		    PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwTrace[0].lOffset;
		    break;
        case ePNDEV_ASIC_AM5728:
            PntrcAsicOffset = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwTrace[0].lOffset;
            break;
    case ePNDEV_ASIC_I210:
    case ePNDEV_ASIC_I210IS:
    case ePNDEV_ASIC_I82574:
    case ePNDEV_ASIC_IX1000:
    case ePNDEV_ASIC_KSZ8841:
    case ePNDEV_ASIC_KSZ8842:
    case ePNDEV_ASIC_INVALID:
	default:
        EPS_FATAL("Board not supported");
	}

	copyDataIn.eHwRes          = ePNDEV_COPY_HW_RES_SDRAM;
	copyDataIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;

	copyDataIn.uArrayBlockWrite[0].lOffsetAsic        = PntrcAsicOffset + offset;
	copyDataIn.uArrayBlockWrite[0].lOffsetHostDataBuf = lWriteBlock0OffsetHostDataBuf;
	copyDataIn.uArrayBlockWrite[0].lSize              = lWriteHostBufSize;

	drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCopyData(
		g_pEpsPnDevDrv->pDrvHandle,
		sizeof( copyDataIn ),
		lSizeBufOut,
		&copyDataIn,
		pCopyDataOut );
	EPS_ASSERT( drvResult == ePNDEV_OK );

    retValFree = eps_mem_free( pBufOut, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
    EPS_ASSERT(LSA_RET_OK == retValFree);
	eps_pndevdrv_exit();

	return result;
}

/**
 * This function the value of the time stamp counters from the pn boards. The offsets as well as time timers are different for different boards.
 *
 * - SOC1:          High and Low part accessible
 * - ERTEC200P      High and Low part accessible
 * - ERTEC200       Low part accessible, High part done by software counter. Note: This counter counts downwards, not upwards
 * - ERTEC200       Low part accessible, High part done by software counter. Note: This counter counts downwards, not upwards
 * - TI Board       No basic variant, Function should not be called
 * - Intel Boards   No basic variant, function should not be called
 * - Micrel Boards  No basic variant, function should not be called
 *
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId )
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
	LSA_UINT32 lower_timestamp_low, lower_timestamp_high;

	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	EPS_ASSERT(pBoard->Pntrc.pTraceTimerUnitBase != 0); // has to be set at device open

	if (   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200)
		|| (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB200_PCIE)
        )
	{
		lower_timestamp_low = (0xffffffff - *((uint32_t*) EPS_CAST_TO_VOID_PTR(pBoard->Pntrc.pTraceTimerUnitBase + pBoard->Pntrc.lowOffset))); // this counter does not increment starting from 0, but a timer that decrements from 0xffffffff
		lower_timestamp_high = 0xffffffff; // we can not read it here. Invalid value
	}
    else if (   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_TI_AM5728)
             || (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_HERA))
    {
        lower_timestamp_low  = EPS_REG32_READ_OFFSET(pBoard->Pntrc.pTraceTimerUnitBase, pBoard->Pntrc.lowOffset); // this counter does increment starting from 0
        lower_timestamp_high = 0x00000000; // we can not read it here. Invalid value
    }
	else
	{
		lower_timestamp_low =  *((uint32_t*) EPS_CAST_TO_VOID_PTR(pBoard->Pntrc.pTraceTimerUnitBase + pBoard->Pntrc.lowOffset)); // This counter starts from 0 and increments
		lower_timestamp_high = *((uint32_t*) EPS_CAST_TO_VOID_PTR(pBoard->Pntrc.pTraceTimerUnitBase + pBoard->Pntrc.highOffset));
	}

    EPS_SYSTEM_TRACE_SYNC_TIME_LOWER(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOWER lower_cpu_id=%ld lower_time=%ld %ld", lowerCpuId, lower_timestamp_low, lower_timestamp_high);

	return result;
}

#else /* EPS_CFG_USE_PNDEVDRV not configured */

LSA_VOID eps_pndevdrv_install( LSA_VOID )
{
	/* Do nothing if PnDevDrv not supported */
}

#endif /* EPS_CFG_USE_PNDEVDRV */

#endif /* ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1)) */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
