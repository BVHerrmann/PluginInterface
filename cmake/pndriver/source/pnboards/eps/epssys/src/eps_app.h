#ifndef EPS_APP_H_                  /* ----- reinclude-protection ----- */
#define EPS_APP_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_app.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS APP adapttion interface                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

//lint --e(537) Repeated include file - Defines are necessary, but other C files directly include psi_inc.h
#include "psi_inc.h" /*1*/
#include "eps_rtos.h"
#include <eps_cp_hw.h>          /* EPS CP PSI adaption                      */

#define EPS_APP_FILE_CHUNK_SIZE   			(1024*128) /*128 KB*/
#define EPS_APP_PATH_MAX_LENGTH             255
#define EPS_APP_DRIVER_PATH_MAX_ARR_SIZE    3
#define EPS_APP_MAC_ADDR_MAX              	32

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef enum eps_app_file_type_enum { 
	EPS_APP_FILE_FW_ADONIS   = 0x00,  /**< Adonis Firmware Image         */
	EPS_APP_FILE_FW_LINUX    = 0x01,  /**< Linux Firmware Image         */
	EPS_APP_FILE_BOOTLOADER  = 0x02,  /**< Bootloader Image       */
	EPS_APP_FILE_INTERNAL_FW = 0x03,  /**< Internal Firmware Image (for co-processor on Hera/TI/EB200P)   */
	EPS_APP_FILE_INVALID     = 0x04   /**< Invalid File           */
} EPS_APP_FILE_TAG_ENUM;

typedef enum eps_app_driver_type_enum {
	EPS_APP_DRIVER_PNDEVDRV		= 0x00,  /**< PnDevDrv                      */
	EPS_APP_DRIVER_VDD			= 0x01,  /**< VDD (Virtual Device Driver)   */
	EPS_APP_DRIVER_INVALID		= 0x02   /**< Invalid Driver                */
} EPS_APP_DRIVER_TYPE_ENUM;

typedef struct eps_app_path
{
    LSA_CHAR    sPath[EPS_APP_PATH_MAX_LENGTH];
    LSA_UINT32  uPathLength;
} EPS_APP_PATH, *EPS_APP_PATH_PTR_TYPE;

typedef struct eps_app_file_tag
{
    LSA_UINT32            nHandle;            /**< [out] Handle                  */ 
	LSA_UINT32            uFileSize;          /**< [out] File size               */ 
	LSA_UINT8*            pFileData;          /**< [out] Pointer to file in mem  */
    EPS_APP_PATH          Path;               /**< [out] Path to file            */
    LSA_BOOL              isHD;               /**< [in] 0 = LD, 1 = HD           */
    EPS_PNDEV_BOARD_TYPE  pndevBoard;         /**< [in] Board type @see EPS_PNDEV_BOARD_TYPE          */
	EPS_EDDP_BOARD_TYPE   pndevBoardDetail;   /**< [in] Detail Board type @see EPS_EDDP_BOARD_TYPE    */
    EPS_APP_FILE_TAG_ENUM		FileType;           /**< [in/out] File description @see EPS_APP_FILE_TAG_ENUM   */ 
} EPS_APP_FILE_TAG_TYPE, *EPS_APP_FILE_TAG_PTR_TYPE;

typedef struct eps_app_driver_path_arr
{
    EPS_APP_PATH     Path[EPS_APP_DRIVER_PATH_MAX_ARR_SIZE];
} EPS_APP_DRIVER_PATH_ARR, *EPS_APP_DRIVER_PATH_ARR_PTR_TYPE;

typedef struct eps_app_mac_adr_arr
{
    LSA_UINT8   lArrayMacAdr [EPS_APP_MAC_ADDR_MAX][6];
} EPS_APP_MAC_ADDR_ARR, *EPS_APP_MAC_ADDR_ARR_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID EPS_APP_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw);
LSA_VOID EPS_APP_UNDO_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw); 

//File (FW & Bootloader)
LSA_VOID EPS_APP_ALLOC_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag); //0 = LD, 1 = HD
LSA_VOID EPS_APP_FREE_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag);
LSA_VOID EPS_APP_INSTALL_DRV_OPEN(PSI_LD_RUNS_ON_TYPE ldRunsOnType);
LSA_VOID EPS_APP_UNINSTALL_DRV_CLOSE(LSA_VOID_PTR_TYPE hSys);
LSA_VOID EPS_APP_SAVE_DUMP_FILE(EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo);
LSA_VOID EPS_APP_GET_DRIVER_PATH(EPS_APP_DRIVER_PATH_ARR_PTR_TYPE pPathArr, EPS_APP_DRIVER_TYPE_ENUM driverType);
LSA_VOID EPS_APP_GET_MAC_ADDR(EPS_APP_MAC_ADDR_ARR_PTR_TYPE pMacArr, LSA_UINT16 hd_id, LSA_UINT16 countMacAddrRequested);

//extern LED Functionality (optional - EDDS)
#if (defined PSI_EDDS_CFG_HW_INTEL || defined PSI_EDDS_CFG_HW_KSZ88XX || defined PSI_EDDS_CFG_HW_TI || defined PSI_EDDS_CFG_HW_IOT2000)
LSA_VOID EPS_APP_LL_LED_BACKUP_MODE(EPS_BOARD_INFO_PTR_TYPE pBoard, LSA_VOID_PTR_TYPE pLLManagement);
LSA_VOID EPS_APP_LL_LED_RESTORE_MODE(EPS_BOARD_INFO_PTR_TYPE pBoard, LSA_VOID_PTR_TYPE pLLManagement);
LSA_VOID EPS_APP_LL_LED_SET_MODE(EPS_BOARD_INFO_PTR_TYPE pBoard, LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn);
#endif // (PSI_EDDS_CFG_HW_INTEL || PSI_EDDS_CFG_HW_KSZ88XX || PSI_EDDS_CFG_HW_TI || PSI_EDDS_CFG_HW_IOT2000)

//extern LED Functionality (optional - EDDI - SOC1)
#if (PSI_CFG_USE_EDDI == 1 && defined EDDI_CFG_SOC)
LSA_VOID EPS_APP_LL_LED_BLINK_BEGIN(EPS_SYS_PTR_TYPE hSysDev, LSA_UINT32 HwPortIndex, LSA_UINT32 PhyTransceiver);
LSA_VOID EPS_APP_LL_LED_BLINK_END(EPS_SYS_PTR_TYPE hSysDev, LSA_UINT32 HwPortIndex, LSA_UINT32 PhyTransceiver);
LSA_VOID EPS_APP_LL_LED_BLINK_SET_MODE(EPS_SYS_PTR_TYPE hSysDev, LSA_UINT32 HwPortIndex, LSA_UINT32 PhyTransceiver, LSA_UINT16 mode);
#endif

LSA_VOID EPS_APP_FATAL_HOOK( EPS_TYPE_EXIT_CODE eps_exit_code );
LSA_VOID EPS_APP_KILL_EPS( EPS_TYPE_EXIT_CODE eps_exit_code );
LSA_VOID EPS_APP_HALT( LSA_VOID );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_APP_H_ */
