#ifndef EPS_HIF_DRV_IF_H_
#define EPS_HIF_DRV_IF_H_

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
/*  F i l e               &F: eps_hif_drv_if.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HIF Hardware Manager Interface                                       */
/*                                                                           */
/*****************************************************************************/

#if ( EPS_CFG_USE_HIF == 1 )

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_HIF_DRV_RET_ERR             0
#define EPS_HIF_DRV_RET_OK              1

#define EPS_HIF_DRV_HIF_TM_SHARED_MEM        1
#define EPS_HIF_DRV_HIF_TM_SHORT             2
#define EPS_HIF_DRV_HIF_TM_MIXED             3
#define EPS_HIF_DRV_HIF_TM_SHARED_MEM_NO_SER 4

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

struct eps_hif_drv_hw_tag;
struct eps_hif_drv_openpar_tag;

typedef LSA_VOID   (*EPS_HIF_DRV_UNINSTALL_FCT)     (LSA_VOID);
typedef LSA_UINT16 (*EPS_HIF_DRV_OPEN_FCT)          (struct eps_hif_drv_openpar_tag const* pInPars, struct eps_hif_drv_hw_tag** ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper);
typedef LSA_UINT16 (*EPS_HIF_DRV_CLOSE_FCT)         (struct eps_hif_drv_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_ENABLE_ISR_FCT)    (struct eps_hif_drv_hw_tag const* pHwInstIn, LSA_UINT16 hHif);
typedef LSA_UINT16 (*EPS_HIF_DRV_DISABLE_ISR_FCT)   (struct eps_hif_drv_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_SEND_HIF_IR)       (struct eps_hif_drv_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_ACK_HIF_IR)        (struct eps_hif_drv_hw_tag const* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_REENABLE_HIF_IR)   (struct eps_hif_drv_hw_tag const* pHwInstIn);
typedef LSA_VOID   (*EPS_HIF_DRV_DMA_CPY_FCT)       (struct eps_hif_drv_hw_tag const* pHwInstIn, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength);
typedef LSA_BOOL   (*EPS_HIF_DRV_DMA_PEND_FREE_FCT) (struct eps_hif_drv_hw_tag const* pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree);
typedef LSA_VOID   (*EPS_HIF_DRV_GET_PARAMS)        (struct eps_hif_drv_hw_tag const* pHwInstIn, LSA_VOID* pPar);

typedef struct eps_hif_drv_hw_tag
{	
	LSA_UINT32 hHandle;
	LSA_UINT16 hd_id;
	LSA_BOOL   bUpper;
}  EPS_HIF_DRV_HW_TYPE, *EPS_HIF_DRV_HW_PTR_TYPE, **EPS_HIF_DRV_HW_PTR_PTR_TYPE;
typedef EPS_HIF_DRV_HW_TYPE const* EPS_HIF_DRV_HW_CONST_PTR_TYPE;

typedef struct
{
	EPS_HIF_DRV_OPEN_FCT            open;
	EPS_HIF_DRV_CLOSE_FCT           close;
	EPS_HIF_DRV_UNINSTALL_FCT       uninstall;	
	EPS_HIF_DRV_DMA_CPY_FCT         DmaCpy;
	EPS_HIF_DRV_DMA_PEND_FREE_FCT   DmaPendFree;
	EPS_HIF_DRV_ENABLE_ISR_FCT      EnableIsr;
	EPS_HIF_DRV_DISABLE_ISR_FCT     DisableIsr;
	EPS_HIF_DRV_SEND_HIF_IR         SendIsr;
	EPS_HIF_DRV_ACK_HIF_IR          AckIsr;
	EPS_HIF_DRV_REENABLE_HIF_IR     ReenableIsr;
	EPS_HIF_DRV_GET_PARAMS          GetParams;
}  EPS_HIF_DRV_IF_TYPE, *EPS_HIF_DRV_IF_PTR_TYPE;
typedef EPS_HIF_DRV_IF_TYPE const* EPS_HIF_DRV_IF_CONST_PTR_TYPE;

/**
 * Configuration to select proper HIF driver. The given configuration is compared to the supported configuration of all 
 * installed HIF driver. The HIF driver which matches all values is used.
 */
typedef struct eps_hif_drv_cfg_tag
{
	LSA_BOOL bUseIpcInterrupts;         /**< Use ipc interrupts for communication over HIF (used together with @see uType to select proper
	                                         ipc interrupt driver */
	LSA_BOOL bUsePnCoreInterrupts;      /**< Select interrupt driven HIF driver that uses the PNCore API provided by the PNDevDriver  */
	LSA_BOOL bUseRemoteMem;             /**< Use a shared memory section on the remote device for HIF data transfers       */
	LSA_BOOL bUseDma;                   /**< Use dma controller for data transfer in HIF      */
	LSA_BOOL bUseShortMode;             /**< No inter processor data transfer over shared memory, HIF is used in short mode  */
	LSA_BOOL bUseSharedMemMode;         /**< Use short mode driver for HIF. (Inverse of @see bUseShortMode !redundant!)     */ 
	LSA_UINT32 uType;                   /**< Select specific/generic IPC driver for ipc communication @see EPS_IPC_DRV_SUPPORT_TYPE, value is evaluated together with @see bUseIpcInterrupts */ 
	LSA_BOOL bReserved2;                /**< padding */
} EPS_HIF_DRV_CFG_TYPE, *EPS_HIF_DRV_CFG_PTR_TYPE;
typedef EPS_HIF_DRV_CFG_TYPE const* EPS_HIF_DRV_CFG_CONST_PTR_TYPE;

typedef struct
{
	LSA_UINT32 bValid; /* If not valid, default values will be used => Poll On, No Isr, Shared Mem (Buffer Split) */
	EPS_HIF_DRV_CFG_TYPE HifCfg;
	struct
	{
		LSA_UINT32 uMsgValue;             /* writing this value        */			
		LSA_UINT32 uPhysAddress;          /* to this pci address       */
	} IPC;                                /* IPC configuration for HIF */
	struct
	{
		LSA_UINT32 uPhyAddress;           /* PCI Address                           */
		LSA_UINT32 uSize;                 /* Size in Bytes                         */
	} HostRam;                            /* Host Memory Support => HIF Split Pool */
} EPS_HIF_DRV_SHM_UPPER_CONFIG_TYPE, *EPS_HIF_DRV_SHM_UPPER_CONFIG_PTR_TYPE;

typedef struct eps_hif_drv_openpar_tag
{
    LSA_UINT16 Mode;
    struct
    {
        struct
        {
			struct
			{
				LSA_UINT8* Base;
				LSA_UINT32 Size;
			} Local;
			struct
			{
				LSA_UINT8* Base;
				LSA_UINT32 Size;
			} Remote;
        } SharedMem;
        HIF_HANDLE hif_lower_device;
        HIF_SEND_IRQ_MODE_TYPE SendIrqMode;
        LSA_UINT64 uNsWaitBeforeSendIrq;
  
    } if_pars;
	struct
	{
		struct
		{
			struct
			{
				LSA_UINT8* pBase;
				LSA_UINT32 PhysBaseAhb;
				LSA_UINT32 RemoteShmPhysBaseAhb;
				LSA_BOOL   bUseDma;
			} siegdma;
			struct
			{
				LSA_BOOL   bUsehIpcRx;
				LSA_UINT32 hIpcRx;
				LSA_BOOL   bUsehIpcTx;
				LSA_UINT32 hIpcTx;
			} ipc;
		} pars;
	} drv_type_specific;
} EPS_HIF_DRV_OPENPAR_TYPE, *EPS_HIF_DRV_OPENPAR_PTR_TYPE;
typedef EPS_HIF_DRV_OPENPAR_TYPE const* EPS_HIF_DRV_OPENPAR_CONST_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_hif_drv_if_init              (LSA_VOID);
LSA_VOID   eps_hif_drv_if_undo_init         (LSA_VOID);
LSA_VOID   eps_hif_drv_init_config          (EPS_HIF_DRV_CFG_PTR_TYPE pCfg);
LSA_VOID   eps_hif_drv_if_register          (EPS_HIF_DRV_IF_CONST_PTR_TYPE pHifDevIf, EPS_HIF_DRV_CFG_CONST_PTR_TYPE pConfig);
LSA_UINT16 eps_hif_drv_if_open              (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_CFG_CONST_PTR_TYPE pCfg, EPS_HIF_DRV_OPENPAR_CONST_PTR_TYPE pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut);
LSA_UINT16 eps_hif_drv_if_close             (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_get               (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut);
LSA_VOID   eps_hif_drv_if_dma_copy          (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength);
LSA_BOOL   eps_hif_drv_if_dma_pend_free     (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree);
LSA_UINT16 eps_hif_drv_if_enable_isr        (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_UINT16 hHif);
LSA_UINT16 eps_hif_drv_if_disable_isr       (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_send_isr          (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_ack_isr           (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_reenable_isr      (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_VOID   eps_hif_drv_if_get_params        (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID* pPar);
LSA_VOID   eps_hif_drv_if_check_reenable_isr(LSA_VOID);

#endif // ( EPS_CFG_USE_HIF == 1 )

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_HIF_DRV_IF_H_ */
