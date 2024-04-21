#ifndef EPS_PN_VDD_DRV_H_             /* ----- reinclude-protection ----- */
#define EPS_PN_VDD_DRV_H_

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
/*  F i l e               &F: eps_pn_vdd_drv.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
//#include "eps_rtos.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <precomp.h>
#include "vdd_Env.h"
#include "vddU.h"
#endif

#include <eps_plf.h>   
    
//lint --e(537) Repeated include file. Without the header we get compile errors. 
#include "eps_sys.h"
//lint --e(537) Repeated include file. Without the header we get compile errors. 
#include "eps_shm_if_config.h"
#include "eps_shm_file.h"

#include <pntrc_cfg.h>
#include <pntrc_usr.h>
#include "pntrc_sys.h"

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_CFG_PN_VDD_MAX_BOARDS        5

#define EPS_PN_VDD_WD_ALIVE_POLL_TIME_MS              1000    // time in ms
#define EPS_PN_VDD_WD_MAX_TIMEOUTCOUNTER              30      // EPS_CFG_PN_VDD_WD_ALIVE_POLL_TIME_MS * EPS_CFG_PN_VDD_WD_MAX_TIMEOUTCOUNTER = TIMEOUT (e.g. 1000 ms * 30 = 30 sec) 
#define EPS_PN_VDD_ADN_REBOOT_MAX_TIMEOUTCOUNTER      40      // wait max. 40 trys for the reboot of adonis (each try waits 500 ms -> 40 * 500 ms = 20 sec)
    
#define EPS_PN_VDD_SELF_STATE_NINIT     0x00000000ul
#define EPS_PN_VDD_SELF_STATE_READY     0x12345678ul
#define EPS_PN_VDD_REMO_STATE_READY     0x5512AA34ul
#define EPS_PN_VDD_REMO_STATE_CLOSED    0xcafecafeul

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef struct _uVDD_OPEN_DEVICE_INFO  // device parama after open
{
	LSA_UINT16 uEddType;
	LSA_UINT16 uPciBusNr;
	LSA_UINT16 uPciDeviceNr;
	LSA_UINT16 uPciFuncNr;
}uVDD_OPEN_DEVICE_INFO;

typedef struct eps_vdd_driver_open_info_tag 
	{
	LSA_BOOL               is_open;    // Board opened yes/no
	LSA_UINT16             board_nr;   // logical board number (from INI)
	LSA_UINT16             hif_handle; // HIF handle for HIF ISR
    uVDD_OPEN_DEVICE_INFO  openInfo;   // Device params after open
} EPS_VDD_DRIVER_OPEN_INFO_TYPE;

typedef struct eps_pn_vdd_wd_tag
{
    LSA_UINT32          hWdThread;
    LSA_UINT32          bWdEnabled;
    LSA_TIMER_ID_TYPE   hWdTimer;
    LSA_UINT32          bAlive;
    LSA_UINT32          uTimeOutCounter;
}EPS_PN_VDD_WD_TYPE;
//lint -esym(768, eps_pn_vdd_wd_tag::hWdTimer) Element not used, but cant be removed since its part of the shared memory interface.

typedef struct eps_pn_vdd_check_remote_state_tag
{
    LSA_UINT32          bCheckRemoteStateEnabled;
    LSA_TIMER_ID_TYPE   hCheckRemoteStateTimer;
}EPS_PN_VDD_CHECK_REMOTE_STATE_TYPE;
//lint -esym(768, eps_pn_vdd_check_remote_state_tag::bCheckRemoteStateEnabled) Element not used, but cant be removed since its part of the shared memory interface.
//lint -esym(768, eps_pn_vdd_check_remote_state_tag::hCheckRemoteStateTimer) Element not used, but cant be removed since its part of the shared memory interface.

typedef struct {  // structure of Shared MEM( must match mit EPS, see also EPS)

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

	EPS_SHM_FILE_TRANSFER_TYPE          HostFileTransfer;
    EPS_PN_VDD_WD_TYPE                  Watchdog;
	EPS_PN_VDD_CHECK_REMOTE_STATE_TYPE     CheckRemoteState;

}*EPS_VDD_SHM_MEM_HEADER_PTR_TYPE;
//lint -esym(768, uSizeSharedMemBlock) Element not used, but cant be removed since its part of the shared memory interface.
//lint -esym(768, uSharedMemBlockAddr) Element not used, but cant be removed since its part of the shared memory interface.
//lint -esym(768, uOsIdent) Element not used, but cant be removed since its part of the shared memory interface.
//lint -esym(768, CheckRemoteState) Element not used, but cant be removed since its part of the shared memory interface.
typedef struct eps_pn_vdd_board_tag
{
    LSA_BOOL                        bUsed;
	EPS_PNDEV_HW_TYPE               sHw;
	EPS_SYS_TYPE                    sysDev;
	struct eps_pn_vdd_store_tag*       pBackRef;
    LSA_UINT16                      hd_id;

	struct 
	{
		LSA_BOOL bOpened;
		PNTRC_CPU_HANDLE_PTR_TYPE hPntrcCPU;
	} Pntrc;

} EPS_PN_VDD_BOARD_TYPE, *EPS_PN_VDD_BOARD_PTR_TYPE;
typedef EPS_PN_VDD_BOARD_TYPE const* EPS_PN_VDD_BOARD_CONST_PTR_TYPE;

typedef struct eps_vdd_poll_tag
{
	LSA_BOOL                        bRunning;
	LSA_UINT32                      uCntUsers;
	LSA_UINT32                      hThread;
	EPS_PNDEV_INTERRUPT_DESC_TYPE   uIntSrc[EPS_CFG_PN_VDD_MAX_BOARDS];
	EPS_PN_VDD_BOARD_PTR_TYPE       pBoard[EPS_CFG_PN_VDD_MAX_BOARDS];

} EPS_VDD_POLL_TYPE, *EPS_VDD_POLL_PTR_TYPE;


typedef struct eps_pn_vdd_store_tag
{
	LSA_UINT16              hEnterExit;
	EPS_PN_VDD_BOARD_TYPE   pBoard[EPS_CFG_PN_VDD_MAX_BOARDS];
	EPS_SHM_HW_TYPE         sHw;
	EPS_HIF_DRV_HW_PTR_TYPE pHifDrvHwInstOut;
	struct
	{
		EPS_VDD_POLL_TYPE   PnGatheredIsr;
	} poll;

    uVDD_DESC               *pDrvHandle;
    LSA_BOOL                bFileAllocated;
    EPS_APP_FILE_TAG_TYPE   file_tag;
} EPS_PN_VDD_STORE_TYPE, *EPS_PN_VDD_STORE_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_pn_vdd_install             (LSA_VOID);

// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_pn_vdd_install)
//lint -esym(759, eps_pn_vdd_install)
//lint -esym(765, eps_pn_vdd_install)

typedef struct eps_vdd_global_vals 
{
	LSA_UINT32  uThreadFileTransferUserCnt;
	LSA_BOOL    bVddStarted;
	LSA_BOOL    bFWDebugActivated;

    EPS_VDD_DRIVER_OPEN_INFO_TYPE     DeviceOpenList[PSI_CFG_MAX_IF_CNT+1];      // Device list
	EPS_VDD_SHM_MEM_HEADER_PTR_TYPE   pShmHeaderShared;

} EPS_VDD_GLOBAL_VALS_TYPE;

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PN_VDD_DRV_H_ */
