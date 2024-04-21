#ifndef EPS_PNDEVDRV_TYPES_H_
#define EPS_PNDEVDRV_TYPES_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_pndevdrv_types.h                      :F&  */
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

#include "eps_hif_drv_if.h"
#include "eps_shm_if.h"
#include "eps_shm_if_config.h"
#include "pntrc_inc.h"
#include "eps_hw_edds.h"

//lint --e(537) Repeated include file. Necessary to have the types here.
#include <PnDev_Driver_Inc.h>       /* PnDevDrv Interface                       */

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_CFG_PNDEVDRV_MAX_BOARDS         10
/*****************************************************************************/
/*  Structure definitions                                                    */
/*****************************************************************************/
typedef struct eps_pndevdrv_tmr_par
{
	uPNDEV_OPEN_TIMER_OUT           OpenTmr;            // open tmr out parameter
	EPS_PNDEV_TMR_TICK_PAR_TYPE     TmrPar;             // tmr tick parameters

	LSA_UINT32                      uTicksToSyncCount;  // Check sync tick occurrence
	LSA_UINT32                      uSyncCount;			// count received sync ticks
	LSA_UINT32                      uSyncErrCount;		// count sync errors
} EPS_PNDEVDRV_TMR_PAR_TYPE, *EPS_PNDEVDRV_TMR_PAR_PTR_TYPE;

typedef struct eps_pndevdrv_board_tag
{
	LSA_BOOL                        bUsed;
	EPS_PNDEV_HW_TYPE               sHw;
	LSA_UINT32                      uCountIsrEnabled;
	EPS_PNDEV_CALLBACK_TYPE         sIsrIsochronous;
	EPS_PNDEV_CALLBACK_TYPE         sIsrPnGathered;
	EPS_PNDEV_CALLBACK_TYPE         sIsrPnNrt;
    EPS_PNDEV_CALLBACK_TYPE         sIsrInterrupt;
    EPS_PNDEV_CALLBACK_TYPE         sIsrIpcRx;
    EPS_PNDEV_INTERRUPT_DESC_TYPE   sIsrDesc;
	uPNDEV_GET_DEVICE_INFO_OUT*     pDeviceInfo;
	uPNDEV_OPEN_DEVICE_OUT          OpenDevice;
	EPS_PNDEVDRV_TMR_PAR_TYPE       TmrParPnGathered;
	EPS_PNDEVDRV_TMR_PAR_TYPE       TmrParPnNrt;
	EPS_PNDEV_LOCATION_TYPE         Location;
	EPS_SYS_TYPE                    sysDev;
	LSA_BOOL                        bBoardFwUsed;
	EPS_HIF_DRV_HW_PTR_TYPE         pHifDrvHwInstOut;
	EPS_SHM_UPPER_CONFIG_TYPE       SharedMemConfig;

    LSA_UINT32                      fileTransferPollThreadHandle;

    struct
	{
		struct
		{
			struct
			{
				LSA_UINT32  uRxHandle;
			} Rx;
		} Ipc;
		struct
		{
			LSA_UINT8*  pLocalAddress; /* Local Host Mem Address */
		} HostMem;
	} Hif;

    struct eps_pndevdrv_store_tag* pBackRef;

    struct
	{
		EPS_PNDEV_CALLBACK_TYPE     sTimerCbf;
		LSA_UINT32                  hThread;
		LSA_BOOL                    bRunning;
	} TimerCtrl;

    struct
	{
		LSA_BOOL                    bOpened;
		PNTRC_CPU_HANDLE_PTR_TYPE   hPntrcCPU;

		volatile LSA_UINT8* pTraceTimerUnitBase; // pointer to trace timer accessible by the HOST
        LSA_UINT32 lowOffset;			         // offset from the trace timer unit to the low value of the timestamp
        LSA_UINT32 highOffset;			         // offset from the trace timer unit to the high value of the timestamp
	} Pntrc;

    #if (PSI_CFG_USE_EDDS == 1)
    #if defined (PSI_EDDS_CFG_HW_INTEL)
	EPS_EDDS_INTEL_LL_TYPE      uEddsIntel;     // Intel specific (Springville, Hartwell)
    #endif

    #if defined (PSI_EDDS_CFG_HW_KSZ88XX)
	EPS_EDDS_KSZ88XX_LL_TYPE    uEddsKSZ88XX;   // Micrel specific
    #endif

    #if defined (PSI_EDDS_CFG_HW_TI)
    EPS_EDDS_TI_LL_TYPE         uEddsTI;        // TI specific
    #endif

    #if defined (PSI_EDDS_CFG_HW_IOT2000)
	EPS_EDDS_IOT2000_LL_TYPE    uEddsIOT2000;   // IOT2000 specific
    #endif
    #endif // #if (PSI_CFG_USE_EDDS == 1)
} EPS_PNDEVDRV_BOARD_TYPE, *EPS_PNDEVDRV_BOARD_PTR_TYPE;
typedef EPS_PNDEVDRV_BOARD_TYPE const* EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE;

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDEVDRV_TYPES_H_ */
