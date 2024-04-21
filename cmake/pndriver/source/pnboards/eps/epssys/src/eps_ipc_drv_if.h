#ifndef EPS_IPC_DRV_IF_H_
#define EPS_IPC_DRV_IF_H_

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
/*  F i l e               &F: eps_ipc_drv_if.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS IPC Driver Interface                                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_IPC_RET_ERR 0
#define EPS_IPC_RET_OK  1

#define EPS_IPC_HERA_SCRB_PHY_ADDR           ((LSA_UINT32)0xC0004000) // Phsysical address of scrb on hera
#define EPS_IPC_HERA_SCRB_SET_IRQ_REG_OFFSET ((LSA_UINT32)0x00000090) // SCRB Reg INT_SW_SET_REG0
#define EPS_IPC_HERA_SCRB_CLR_IRQ_REG_OFFSET ((LSA_UINT32)0x00000094) // SCRB Reg INT_SW_CLEAR_REG0
#define EPS_IPC_HERA_BOARD_LOCAL_IRQ_VAL     ((LSA_UINT32)1<<4)       // INT_SW_4 trigger value
#define EPS_IPC_HERA_BOARD_REMOTE_IRQ_VAL    ((LSA_UINT32)1<<14)      // INT_SW_14 trigger value @see ePNDEV_INT_SRC_HERA__SCRB_SW_INT14
#define EPS_IPC_HERA_BOARD_LOCAL_IRQ_NR                   217         // INT_SW_4

#define EPS_IPC_SOC_GICU_PHY_ADDR               ((LSA_UINT32)0x1E200000)    // Phsysical address of icu on soc
#define EPS_IPC_SOC_GICU_OFFSET_IRQ_SWIRREG4    ((LSA_UINT32)0x0000008C)    // GICU Reg SWIRREG4 => (144-159)/32 = 4 for setting SW_INT_0 (144) - SW_INT_15 (159)
#define EPS_IPC_SOC_PICU_PHY_ADDR               ((LSA_UINT32)0x1e300000)    // Phsysical address of pci icu on soc
#define EPS_IPC_SOC_PICU_OFFSET_IRQ_SWIRREG_A   ((LSA_UINT32)0x0000002c)    // PCI icu Reg SWIRREG_A
#define EPS_IPC_SOC_BOARD_LOCAL_IRQ_NR          145                         // SW_INT_1 of 16 SW Interrupts triggered with SWIRREG4
#define EPS_IPC_SOC_BOARD_LOCAL_IRQ_VAL         ((LSA_UINT32)1<<(EPS_IPC_SOC_BOARD_LOCAL_IRQ_NR % 32))  // SW_INT_1 trigger value
#define EPS_IPC_SOC_BOARD_REMOTE_IRQ_VAL        ((LSA_UINT32)1<<14)         // @see ePNDEV_INT_SRC_SOC__SW_IRQ_14 trigger value, triggered with SWIRREG_A

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef enum
{
    eEPS_IPC_DRV_SUPPORT_GENERIC      = 0,
    eEPS_IPC_DRV_SUPPORT_HERA         = (1<<0),  // FPGA1-Hera board
    eEPS_IPC_DRV_SUPPORT_CP1625       = (1<<1)   // CP1625 board
} EPS_IPC_DRV_SUPPORT_TYPE;
    
typedef LSA_VOID (*EPS_IPC_THREAD_PROC_TYPE)(LSA_UINT32 uParam, LSA_VOID* pArgs);

typedef LSA_VOID(*EPS_IPC_DRV_UNINSTALL_FCT)   (LSA_VOID);

typedef LSA_VOID(*EPS_IPC_DRV_OPEN_FCT)        (LSA_VOID);
typedef LSA_VOID(*EPS_IPC_DRV_CLOSE_FCT)       (LSA_VOID);

typedef LSA_UINT16(*EPS_IPC_DRV_RX_ALLOC)		 (LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal, LSA_UINT16 hd_id, LSA_UINT32 uType);
typedef LSA_VOID(*EPS_IPC_DRV_RX_FREE)			 (LSA_UINT32 hRxHandle); 
typedef LSA_VOID(*EPS_IPC_DRV_RX_ENABLE_FCT)	 (LSA_UINT32 hRxHandle, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs);
typedef LSA_VOID(*EPS_IPC_DRV_RX_DISABLE_FCT)	 (LSA_UINT32 hRxHandle);
typedef LSA_VOID(*EPS_IPC_DRV_RX_SET_SHM)	     (LSA_UINT32  hRxHandle, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize);

typedef LSA_UINT16(*EPS_IPC_DRV_TX_ALLOC)		(LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal, LSA_UINT16 hd_id, LSA_UINT32 uType);
typedef LSA_UINT16(*EPS_IPC_DRV_TX_ALLOC_FROM_LOCAL_SHM)(LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize, LSA_UINT16 hd_id, LSA_UINT32 uType);
typedef LSA_VOID(*EPS_IPC_DRV_TX_FREE)			(LSA_UINT32  hTxHandle);
typedef LSA_VOID(*EPS_IPC_DRV_TX_SET_SHM)       (LSA_UINT32  hTxHandle, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize);
typedef LSA_VOID(*EPS_IPC_DRV_TX_SEND)			(LSA_UINT32  hTxHandle);

typedef struct
{
	EPS_IPC_DRV_UNINSTALL_FCT		uninstall;

	EPS_IPC_DRV_OPEN_FCT			open;
	EPS_IPC_DRV_CLOSE_FCT			close;

	EPS_IPC_DRV_RX_ALLOC			rxAlloc;
	EPS_IPC_DRV_RX_FREE				rxFree;
	EPS_IPC_DRV_RX_ENABLE_FCT		rxEnable;
	EPS_IPC_DRV_RX_DISABLE_FCT		rxDisable;
	EPS_IPC_DRV_RX_SET_SHM	        rxSetShm;
	
	EPS_IPC_DRV_TX_ALLOC			txAlloc;
	EPS_IPC_DRV_TX_ALLOC_FROM_LOCAL_SHM txAllocFromLocalShm;
	EPS_IPC_DRV_TX_FREE				txFree;
	EPS_IPC_DRV_TX_SET_SHM	        txSetShm;
	EPS_IPC_DRV_TX_SEND				txSend;
	
	LSA_UINT32                      uType;
}  EPS_IPC_DRV_IF_TYPE, *EPS_IPC_DRV_IF_PTR_TYPE;
typedef EPS_IPC_DRV_IF_TYPE const* EPS_IPC_DRV_IF_CONST_PTR_TYPE;


#define EPS_IPC_MAX_INSTANCES 10
#define EPS_IPC_MAGIC_NUMBER  0x12123434

typedef struct 
{
	LSA_UINT32 uMagic; /* EPS_IPC_MAGIC_NUMBER */
	volatile LSA_UINT32 uTxCnt; /* Transmit Cnt         */
	volatile LSA_UINT32 uRxCnt; /* Receive Cnt          */
	LSA_UINT32 uTriggerBaseAddr;
	LSA_UINT32 uTriggerMsg;
} EPS_IPC_SHM_TYPE, *EPS_IPC_SHM_PTR_TYPE;

typedef struct
{
    LSA_UINT32 hIntHandle;
    LSA_UINT32 uAddress;
    LSA_UINT32 uMmapAddress;
    LSA_UINT32 uPhyAddress;
    LSA_UINT32 uValue;
} EPS_IPC_DRV_NOTIFY_INFO_TYPE, *EPS_IPC_DRV_NOTIFY_INFO_PTR_TYPE;

typedef struct
{
    LSA_UINT32  hIntHandle;
    LSA_UINT32  hThread;
    LSA_INT     lEventfd;
} EPS_IPC_DRV_INT_INFO_TYPE, *EPS_IPC_DRV_INT_INFO_PTR_TYPE;

typedef struct
{
	LSA_BOOL   bUsed;
	LSA_BOOL   bIsrEnabled;
	LSA_UINT16 hd_id;
	LSA_UINT32 uType;
	LSA_UINT32 uCntRcvTx;
	LSA_UINT32 uCntRcvIsrs;
	LSA_UINT32 uCntDataTransmitDelay;
	LSA_UINT32 hOwnHandle;
	EPS_IPC_SHM_PTR_TYPE pRemoteShm;
	EPS_IPC_SHM_PTR_TYPE pLocalShm;
	EPS_IPC_DRV_NOTIFY_INFO_TYPE sLocalNotifyInfo;
	EPS_IPC_DRV_INT_INFO_TYPE sLocalInterruptInfo;
	struct
	{
		EPS_IPC_THREAD_PROC_TYPE pCbf;
		LSA_UINT32 uParam;
		LSA_VOID* pArgs;
	} Cbf;
} EPS_IPC_RX_INST_TYPE, *EPS_IPC_RX_INST_PTR_TYPE;
typedef EPS_IPC_RX_INST_TYPE const* EPS_IPC_RX_INST_CONST_PTR_TYPE;

typedef struct
{
	LSA_BOOL   bUsed;
	LSA_UINT16 hd_id;
	LSA_UINT32 uType;
	LSA_UINT32 uCntIsrTrigger;
	LSA_UINT32 uCntDataReceiveDelay;
	LSA_UINT32 hOwnHandle;
	EPS_IPC_SHM_PTR_TYPE pRemoteShm;
	EPS_IPC_SHM_PTR_TYPE pLocalShm;
	EPS_IPC_DRV_NOTIFY_INFO_TYPE sRemoteNotifyInfo;
} EPS_IPC_TX_INST_TYPE, *EPS_IPC_TX_INST_PTR_TYPE;

typedef struct
{
	LSA_UINT16 hEnterExit;
	LSA_BOOL   bInit;
	int fdIpcDriver;
	EPS_IPC_RX_INST_TYPE InstRx[EPS_IPC_MAX_INSTANCES];
	EPS_IPC_TX_INST_TYPE InstTx[EPS_IPC_MAX_INSTANCES];
} EPS_IPC_DRV_TYPE, *EPS_IPC_DRV_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

/* Basic initialisation */
LSA_VOID   eps_ipc_drv_if_init			        (LSA_VOID);
LSA_VOID   eps_ipc_drv_if_undo_init		        (LSA_VOID);

LSA_VOID   eps_ipc_drv_if_open                  (LSA_UINT16 hd_nr, LSA_UINT32 uType);
LSA_VOID   eps_ipc_drv_if_close_all             (LSA_VOID);


LSA_VOID   eps_ipc_drv_if_register				(EPS_IPC_DRV_IF_CONST_PTR_TYPE pIpcDrvIf);

LSA_UINT32 eps_ipc_drv_if_sizeof_shm_data       (LSA_VOID);

/* Local interrupt */
LSA_UINT16 eps_ipc_drv_if_rx_alloc		        (LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal, LSA_UINT16 hd_id);
LSA_VOID   eps_ipc_drv_if_rx_free               (LSA_UINT32 hRxHandle, LSA_UINT16 hd_id);
LSA_VOID   eps_ipc_drv_if_rx_enable		        (LSA_UINT32 hRxHandle, LSA_UINT16 hd_id, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs);
LSA_VOID   eps_ipc_drv_if_rx_disable	        (LSA_UINT32 hRxHandle, LSA_UINT16 hd_id);
LSA_VOID   eps_ipc_drv_if_rx_set_shm	        (LSA_UINT32  hRxHandle, LSA_UINT16 hd_id, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize);


/* Remote interrupt */
LSA_UINT16 eps_ipc_drv_if_tx_alloc			    (LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal, LSA_UINT16 hd_id);
LSA_UINT16 eps_ipc_drv_if_tx_alloc_from_local_shm(LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize, LSA_UINT16 hd_id);
LSA_VOID   eps_ipc_drv_if_tx_send               (LSA_UINT32  hTxHandle, LSA_UINT16 hd_id);
LSA_VOID   eps_ipc_drv_if_tx_free			    (LSA_UINT32  hTxHandle, LSA_UINT16 hd_id);
LSA_VOID   eps_ipc_drv_if_tx_set_shm	        (LSA_UINT32  hTxHandle, LSA_UINT16 hd_id, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_IPC_DRV_IF_H_ */
