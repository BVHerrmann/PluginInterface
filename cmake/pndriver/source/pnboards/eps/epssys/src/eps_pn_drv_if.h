#ifndef EPS_PN_DRV_IF_H_            /* ----- reinclude-protection ----- */
#define EPS_PN_DRV_IF_H_

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
/*  F i l e               &F: eps_pn_drv_if.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN Hardware Manager Interface                                        */
/*                                                                           */
/*****************************************************************************/

//lint --e(537) eps_enums may also be included in eps_cp_hw.h
#include "eps_enums.h"    
    
/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_PNDEV_RET_ERR             0
#define EPS_PNDEV_RET_OK              1
#define EPS_PNDEV_RET_DEV_NOT_FOUND   2
#define EPS_PNDEV_RET_DEV_OPEN_FAILED 3
#define EPS_PNDEV_RET_UNSUPPORTED     4

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

struct eps_pndev_hw_tag;
struct eps_pndev_callback_tag;
struct eps_pndev_location_tag;
struct eps_pndev_open_option_tag;

typedef enum
{
	EPS_PNDEV_ISR_UNDEFINED     = 0,
	EPS_PNDEV_ISR_ISOCHRONOUS   = 1,
	EPS_PNDEV_ISR_PN_GATHERED   = 2,
	EPS_PNDEV_ISR_PN_NRT        = 3,
    EPS_PNDEV_ISR_INTERRUPT     = 4,
    EPS_PNDEV_ISR_POLLINTERRUPT = 5,
    EPS_PNDEV_ISR_CPU           = 6,
    EPS_PNDEV_ISR_IPC_RX        = 7
} EPS_PNDEV_INTERRUPT_DESC_TYPE, *EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE;
typedef EPS_PNDEV_INTERRUPT_DESC_TYPE const* EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE;

typedef enum
{
    EPS_PNDEV_IR_MODE_UNDEFINED     = 0,
    EPS_PNDEV_IR_MODE_POLL          = 1,
    EPS_PNDEV_IR_MODE_INTERRUPT     = 2,
    EPS_PNDEV_IR_MODE_POLLINTERRUPT = 3,
    EPS_PNDEV_IR_MODE_CPU           = 4
}EPS_PNDEV_IR_MODE_TYPE, *EPS_PNDEV_IR_MODE_PTR_TYPE;

// typedef enum { ... } EPS_PNDEV_ASIC_TYPE, *EPS_PNDEV_ASIC_PTR_TYPE;  moved to "eps_enums.h"

typedef enum
{
	EPS_PNDEV_LOCATION_INVALID  = 0,
	EPS_PNDEV_LOCATION_PCI      = 1,
	EPS_PNDEV_LOCATION_MAC      = 2
} EPS_PNDEV_LOCATION_SELECTOR_TYPE, *EPS_PNDEV_LOCATION_SELECTOR_PTR_TYPE;

typedef enum
{
    EPS_PNDEV_INTERFACE_INVALID = 0,
    EPS_PNDEV_INTERFACE_1       = 1,    // Interface 1: HERA: PNIP A; TI-AM5728: GMAC
    EPS_PNDEV_INTERFACE_2       = 2     // Interface 2: HERA: PNIP B; TI-AM5728: ICSS
} EPS_PNDEV_INTERFACE_SELECTOR_TYPE, *EPS_PNDEV_INTERFACE_SELECTOR_PTR_TYPE;

typedef enum
{
	EPS_PNDEV_DEBUG_GPIO_1 = 1,
	EPS_PNDEV_DEBUG_GPIO_2 = 2
} EPS_PNDEV_GPIO_DESC_TYPE;

typedef LSA_VOID (*EPS_PNDEV_ISR_CBF)(LSA_UINT32 uParam, LSA_VOID* pArgs);

typedef struct eps_pndev_callback_tag //this structure must be identical to EPS_PN_DRV_CALLBACK_TYPE!
{
	EPS_PNDEV_ISR_CBF                   pCbf;
	LSA_UINT32                          uParam;
	LSA_VOID*                           pArgs;
	LSA_UINT32 			                lSII_ExtTimerInterval_us;
	EPS_PNDEV_INTERFACE_SELECTOR_TYPE   eInterfaceSelector; /* Interface Selector */
} EPS_PNDEV_CALLBACK_TYPE, *EPS_PNDEV_CALLBACK_PTR_TYPE;
typedef EPS_PNDEV_CALLBACK_TYPE const* EPS_PNDEV_CALLBACK_CONST_PTR_TYPE;

typedef LSA_UINT8 EPS_PNDEV_MAC_TYPE[6];  /* MAC Type */

typedef struct eps_pndev_location_tag
{
	EPS_PNDEV_LOCATION_SELECTOR_TYPE    eLocationSelector;
    EPS_PNDEV_INTERFACE_SELECTOR_TYPE   eInterfaceSelector;
	LSA_UINT32                          uBusNr;
	LSA_UINT32                          uDeviceNr;
	LSA_UINT32                          uFctNr;
	EPS_PNDEV_MAC_TYPE                  Mac;
} EPS_PNDEV_LOCATION_TYPE, *EPS_PNDEV_LOCATION_PTR_TYPE;
typedef EPS_PNDEV_LOCATION_TYPE const* EPS_PNDEV_LOCATION_CONST_PTR_TYPE;

typedef struct eps_pndev_open_option_tag
{
	LSA_BOOL    bLoadFw;                /// IN Firmware download yes/no 
    // These parameters are only valid, if bLoadFw = LSA_TRUE, otherwise they are ignored
	LSA_BOOL    bDebug;                 /// IN Hardware debug yes/no
	LSA_BOOL    bShmDebug;              /// IN Shared memory debug yes/no
	LSA_BOOL    bShmHaltOnStartup;      /// IN Shared memory halt in eps_init() yes / no
    LSA_BOOL    bUseEpsAppToGetFile;    /// IN LSA_TRUE: EPS_APP_ALLOC_FILE and EPS_APP_FREE_FILE are used to get the user firmware and boot firmware. LSA_FALSE: pFwBufData and pBootFwBufData are used

    LSA_UINT32  uFwBufSize;             /// IN size of Firmware
	LSA_UINT8*  pFwBufData;             /// IN pointer to buffer with Firmware
    LSA_UINT32  uBootFwBufSize;         /// IN size of boot firmware
	LSA_UINT8*  pBootFwBufData;         /// IN pointer to boot firmware
	LSA_UINT8*  pBaseAdr;				/// OUT pointer to BaseAdress (HIF-SHM)
} EPS_PNDEV_OPEN_OPTION_TYPE, *EPS_PNDEV_OPEN_OPTION_PTR_TYPE;
typedef EPS_PNDEV_OPEN_OPTION_TYPE const* EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE;

typedef struct eps_pndev_timer_ctrl_tag
{
	LSA_UINT32  uTimeoutUs;
} EPS_PNDEV_TIMER_CTRL_TYPE, *EPS_PNDEV_TIMER_CTRL_PTR_TYPE;

typedef LSA_VOID   (*EPS_PNDEV_UNINSTALL_FCT)   (LSA_VOID);
typedef LSA_UINT16 (*EPS_PNDEV_OPEN_FCT)        (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, struct eps_pndev_hw_tag** ppHwInstOut, LSA_UINT16 hd_id);
typedef LSA_UINT16 (*EPS_PNDEV_CLOSE_FCT)       (struct eps_pndev_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_ENABLE_ISR_FCT)  (struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
typedef LSA_UINT16 (*EPS_PNDEV_DISABLE_ISR_FCT) (struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
typedef LSA_UINT16 (*EPS_PNDEV_SET_GPIO)        (struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
typedef LSA_UINT16 (*EPS_PNDEV_CLEAR_GPIO)      (struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
typedef LSA_UINT16 (*EPS_PNDEV_TIMER_CTRL_START)(struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
typedef LSA_UINT16 (*EPS_PNDEV_TIMER_CTRL_STOP) (struct eps_pndev_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_READ_TRACE_DATA) (struct eps_pndev_hw_tag const* pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
typedef LSA_UINT16 (*EPS_PNDEV_WRITE_TRACE_DATA)(struct eps_pndev_hw_tag const* pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
typedef LSA_UINT16 (*EPS_PNDEV_SAVE_DUMP)       (struct eps_pndev_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_ENABLE_HW_IR)    (struct eps_pndev_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_DISABLE_HW_IR)   (struct eps_pndev_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_PNDEV_READ_HW_IR)      (struct eps_pndev_hw_tag const* pHwInstIn, LSA_UINT32* interrupts);
typedef LSA_UINT16 (*EPS_PNDEV_WRITE_SYNC_TIME) (struct eps_pndev_hw_tag const* pHwInstIn, LSA_UINT16 lowerCpuId);

typedef struct eps_pndev_hw_tag
{	
	LSA_VOID*                   hDevice;	
	EPS_BOARD_INFO_TYPE         EpsBoardInfo;
	EPS_PNDEV_ENABLE_ISR_FCT    EnableIsr;
	EPS_PNDEV_DISABLE_ISR_FCT   DisableIsr;
	EPS_PNDEV_ENABLE_HW_IR      EnableHwIr;
	EPS_PNDEV_DISABLE_HW_IR     DisableHwIr;
	EPS_PNDEV_READ_HW_IR        ReadHwIr;
    EPS_PNDEV_IR_MODE_TYPE      IrMode;
	EPS_PNDEV_SET_GPIO          SetGpio;
	EPS_PNDEV_CLEAR_GPIO        ClearGpio;
	EPS_PNDEV_TIMER_CTRL_START  TimerCtrlStart;
	EPS_PNDEV_TIMER_CTRL_STOP   TimerCtrlStop;
	EPS_PNDEV_READ_TRACE_DATA   ReadTraceData;   // PNTRC specific
	EPS_PNDEV_WRITE_TRACE_DATA  WriteTraceData; // PNTRC specific
	EPS_PNDEV_WRITE_SYNC_TIME   WriteSyncTimeLower;  // PNTRC specific
    EPS_PNDEV_SAVE_DUMP         SaveDump;
	EPS_PNDEV_ASIC_TYPE         asic_type;
} EPS_PNDEV_HW_TYPE, *EPS_PNDEV_HW_PTR_TYPE, **EPS_PNDEV_HW_PTR_PTR_TYPE;
typedef EPS_PNDEV_HW_TYPE const* EPS_PNDEV_HW_CONST_PTR_TYPE;

typedef struct eps_pndev_tmr_tick_par
{
	EPS_PNDEV_CALLBACK_PTR_TYPE     pTickCbf;               // Cbf executed, when tick is not received too early
	LSA_UINT32                      uTickCount;             // counts timer ticks
	LSA_UINT64                      uTickUnderrunMarkNs;    // tick is received too early, when time of received tick < (last execution time of Cbf + uTickUnderrunMarkNs) 
	LSA_UINT32                      uTickUnderrunCount;		// counts early ticks
	LSA_UINT64                      uTickOverrunMarkNs;		// tick is received too late, when time of received tick > (last execution time of Cbf + uTickOverrunMarkNs) 
	LSA_UINT32                      uTickOverrunCount;		// count late ticks
	LSA_UINT64                      uTickCbfLastCallTimeNs; // last start time of Cbf execution
	LSA_UINT64                      uTickCbfOverrunMarkNs;	// Cbf execution takes too long, when end time of Cbf execution > (start time of Cbf execution + uTickCbfOverrunMarkNs)
	LSA_UINT32                      uTickCbfOverrunCount;	// count long Cbf executions
    /* for detection of follow-up call of last overrun */
    LSA_UINT32                      uTickCountOnLastOverrun;        // value of uTickCount on last overrun
    LSA_UINT32                      uTickOverrunCountOnLastOverrun; // value of uTickOverrunCount on last overrun
} EPS_PNDEV_TMR_TICK_PAR_TYPE, *EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE;

typedef struct
{
	EPS_PNDEV_OPEN_FCT          open;
	EPS_PNDEV_CLOSE_FCT         close;
	EPS_PNDEV_UNINSTALL_FCT     uninstall;
}  EPS_PNDEV_IF_TYPE, *EPS_PNDEV_IF_PTR_TYPE;
typedef EPS_PNDEV_IF_TYPE const* EPS_PNDEV_IF_CONST_PTR_TYPE;

typedef struct
{
	LSA_UINT16      uPciAddrValid;
	LSA_UINT16      uMacAddrValid;
	LSA_UINT16      uPciBusNr;
	LSA_UINT16      uPciDeviceNr;
	LSA_UINT16      uPciFuncNr;
	LSA_UINT16      uEddType;
	LSA_UINT8       uMacAddr[6];

} EPS_PNDEV_IF_DEV_INFO_TYPE, *EPS_PNDEV_IF_DEV_INFO_PTR_TYPE;
typedef EPS_PNDEV_IF_DEV_INFO_TYPE const* EPS_PNDEV_IF_DEV_INFO_CONST_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_pndev_if_init(LSA_VOID);
LSA_VOID   eps_pndev_if_undo_init(LSA_VOID);
LSA_VOID   eps_pndev_if_uninstall_from_app(LSA_VOID);
LSA_VOID   eps_pndev_if_fatal_close(LSA_VOID);
LSA_VOID   eps_pndev_if_register(EPS_PNDEV_IF_CONST_PTR_TYPE pPnDevIf);
LSA_UINT16 eps_pndev_if_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndev_if_close(LSA_UINT16 hd_id);
LSA_UINT16 eps_pndev_if_get(LSA_UINT16 hd_id, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut);

LSA_VOID   eps_pndev_if_register_device(EPS_PNDEV_IF_DEV_INFO_CONST_PTR_TYPE pDev);
LSA_VOID   eps_pndev_if_get_registered_devices(EPS_PNDEV_IF_DEV_INFO_PTR_TYPE pDev, LSA_UINT16 uMaxDevices, LSA_UINT16* pCntDevices);

LSA_VOID   eps_pndev_if_timer_tick(EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE pTmrPar);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PN_DRV_IF_H_ */
