#ifndef EPS_PNDEVDRV_HELPER_H_          /* ----- reinclude-protection ----- */
#define EPS_PNDEVDRV_HELPER_H_

#ifdef __cplusplus                      /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: eps_pndevdrv_helper.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PnDevDrv Driver                                                      */
/*                                                                           */
/*****************************************************************************/

//lint --e(537) Repeated include file - Defines are necessary, but other C files directly include psi_inc.h
#include "psi_inc.h" /*8*/
/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#ifndef EPS_SUPPRESS_PRINTF
#define EPS_PNDEVDRV_PRINTF_DRIVER_INFO
#define EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
#define EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
//#define EPS_PNDEVDRV_PRINTF_EVENTS
//#define EPS_PNDEVDRV_PRINTF_ISR
#endif
/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_pndevdrv_helper_init(uPNDEV_DRIVER_DESC *pDrvHandle);
LSA_VOID eps_pndevdrv_helper_undo_init(LSA_VOID);
LSA_VOID eps_pndevdrv_undo_init_critical_section(LSA_VOID);
LSA_VOID eps_pndevdrv_init_critical_section(LSA_VOID);
LSA_VOID eps_pndevdrv_enter(LSA_VOID);
LSA_VOID eps_pndevdrv_exit(LSA_VOID);

LSA_VOID eps_convert_pndev_err_string (const PNDEV_CHAR* pIn, LSA_CHAR* pOut);
LSA_VOID eps_convert_pndev_path (const LSA_CHAR* pIn, PNDEV_CHAR* pOut);
LSA_VOID eps_pndevdrv_alloc_dump_buffer (EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo, const LSA_UINT8* pHWPtr);

LSA_VOID eps_pndevdrv_register_device_infos(LSA_VOID);
LSA_UINT32 eps_pndevdrv_get_device(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, uPNDEV_GET_DEVICE_INFO_OUT** ppDevice);
LSA_VOID eps_pndevdrv_get_device_infos(LSA_VOID);

EPS_PNDEV_BOARD_TYPE eps_pndevdrv_get_eps_pndev_board_type(ePNDEV_BOARD board);

#if ( PSI_CFG_USE_EDDI == 1 ) 
LSA_UINT8* eps_pndevdrv_calc_phy_pci_host_address(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, uPNCORE_PCI_HOST_PAR* pPnDevDrvPciParamsArray, LSA_UINT32 uCntBars, const LSA_UINT8* pLocalPhyAddress);
// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_pndevdrv_calc_phy_pci_host_address)
//lint -esym(759, eps_pndevdrv_calc_phy_pci_host_address)
//lint -esym(765, eps_pndevdrv_calc_phy_pci_host_address)

#endif

#ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
LSA_VOID eps_pndevdrv_printf_openclose_board(LSA_BOOL bOpen, EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard);
#endif

#ifdef EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
LSA_VOID eps_pndevdrv_printf_found_boards(LSA_VOID);
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDEVDRV_H_ */
