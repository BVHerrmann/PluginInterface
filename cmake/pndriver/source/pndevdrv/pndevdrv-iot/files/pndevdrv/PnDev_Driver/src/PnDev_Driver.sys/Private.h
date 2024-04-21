/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License version 2     */
/* as published by the Free Software Foundation; or, when distributed        */
/* separately from the Linux kernel or incorporated into other               */
/* software packages, subject to the following license:                      */
/*                                                                           */
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: Private.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Private declarations of SYS
//
//****************************************************************************/

#ifndef __Private_h__
#define __Private_h__

	#ifdef __cplusplus		// if C++ - compiler: Use C linkage
	extern "C"
	{
	#endif

	//########################################################################
	//  Defines
	//########################################################################

	#define	PNDEV_DEBUG_CTR_THRESHOLD	100
	#define MAXUINT32					((UINT32)~((UINT32)0))					// BaseTsd.h

	//------------------------------------------------------------------------
	#define	PNDEV_PCI_CONFIG_SPACE__SIZE			            64
    #define	PNDEV_PCI_CONFIG_SPACE__EXPRESS_CAPABILITY_SIZE		44
	#define	PNDEV_PCI_CONFIG_SPACE__COMMAND_OFFSET	            4
	#define	PNDEV_PCI_CONFIG_SPACE__COMMAND_SIZE	            2

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_COPY_ACTION
	{
		ePNDEV_COPY_ACTION_INVALID	= 0,
		ePNDEV_COPY_ACTION_READ		= 1,
		ePNDEV_COPY_ACTION_WRITE	= 2
	}
	ePNDEV_COPY_ACTION;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_RESET_ACTION
	{
		ePNDEV_RESET_ACTION_INVALID		= 0,
		ePNDEV_RESET_ACTION_ALLOC_HW	= 1,
		ePNDEV_RESET_ACTION_RELEASE_HW	= 2,
		ePNDEV_RESET_ACTION_REINIT_HW	= 3,
		ePNDEV_RESET_ACTION_USE_HW		= 4
	}
	ePNDEV_RESET_ACTION;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_SPIN_LOCK
	{
		ePNDEV_SPIN_LOCK_INVALID	= 0,
		ePNDEV_SPIN_LOCK_DPC		= 1
	}
	ePNDEV_SPIN_LOCK;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_CRIT_SEC_SYS
	{
		ePNDEV_CRIT_SEC_SYS_INVALID		= 0,
		ePNDEV_CRIT_SEC_SYS_IOCTL		= 1,
		ePNDEV_CRIT_SEC_SYS_PAGE		= 2,
		ePNDEV_CRIT_SEC_SYS_APP_LIST	= 3
	}
	ePNDEV_CRIT_SEC_SYS;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_SBL
	{
		ePNDEV_SBL_INVALID	= 0,
		ePNDEV_SBL_S7P_V1	= 1,
		ePNDEV_SBL_S7P_V2	= 2
	}
	ePNDEV_SBL;
	
	//------------------------------------------------------------------------
	typedef enum _ePNDEV_VAL_HOST_MEM
	{
		ePNDEV_VAL_HOST_MEM_INVALID	= 0,
		ePNDEV_VAL_HOST_MEM_FIRST	= 1,		// g_uMemSys.uHostMem[0], only 0 valid
		ePNDEV_VAL_HOST_MEM_ALL		= 2			// g_uMemSys.uHostMem[i]
	}
	ePNDEV_VAL_HOST_MEM;
	
	//------------------------------------------------------------------------
	typedef enum _ePNDEV_INTERRUPT_MODE
	{
		ePNDEV_INTERRUPT_MODE_INVALID	= 0,
		ePNDEV_INTERRUPT_MODE_LEG		= 1,
		ePNDEV_INTERRUPT_MODE_MSI		= 2,
		ePNDEV_INTERRUPT_MODE_MSIX		= 3
	}
	ePNDEV_INTERRUPT_MODE;

	//------------------------------------------------------------------------
	typedef void (*FN_BOARD_XX)(void);

	//------------------------------------------------------------------------
	//	ASIC-SDRAM
	//------------------------------------------------------------------------
	#define PNDEV_ASIC_SDRAM__OFFSET_ALIVE		(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x00)
	#define PNDEV_ASIC_SDRAM__OFFSET_IDENT		(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x04)
	#define PNDEV_ASIC_SDRAM__OFFSET_VERSION	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x08)
	#define PNDEV_ASIC_SDRAM__OFFSET_REBOOT_CTR	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x0c)

	#define PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x10)
	#define PNDEV_ASIC_SDRAM__OFFSET_ELF_BASE	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x14)
	#define PNDEV_ASIC_SDRAM__OFFSET_ELF_STATUS	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x18)
	#define PNDEV_ASIC_SDRAM__OFFSET_ELF_ENTRY	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x1c)

	#define PNDEV_ASIC_SDRAM__OFFSET_BIN_CMD	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x20)
	#define PNDEV_ASIC_SDRAM__OFFSET_BIN_FW		(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x24)
	#define PNDEV_ASIC_SDRAM__OFFSET_BIN_STATUS	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x28)

	#define PNDEV_ASIC_SDRAM__OFFSET_CP1625_CMD	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x2c)

	#define PNDEV_ASIC_SDRAM__OFFSET_RESERVED2	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x30)
	#define PNDEV_ASIC_SDRAM__OFFSET_RESERVED3	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x34)
	#define PNDEV_ASIC_SDRAM__OFFSET_RESERVED4	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x38)
	#define PNDEV_ASIC_SDRAM__OFFSET_RESERVED5	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0x3c)

	//------------------------------------------------------------------------
	#define PNDEV_ASIC_SDRAM_ELF_CMD__LOAD						0x00000001
	#define PNDEV_ASIC_SDRAM_ELF_CMD__EXECUTE					0x00000002
	
	#define PNDEV_ASIC_SDRAM_BIN_CMD__EXECUTE					0x00000002

	#define PNDEV_ASIC_SDRAM_CP1625_CMD__NAND_ON				0x00000001
	#define PNDEV_ASIC_SDRAM_CP1625_CMD__NAND_OFF				0x00000002

	#define PNDEV_ASIC_SDRAM_ELF_STATUS__SUCCESS				0x00000000
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_NO_ELF			0xffffffff
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_BIT_WIDTH		0xfffffffe
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_ENDIANESS		0xfffffffd
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_NO_EXECUTABLE	0xfffffffc
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_LOAD				0xfffffffb
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_BASE_KSEG0_1		0x00000001
	#define PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_ENTRY_KSEG0_1	0x00000002

	#define PNDEV_ASIC_SDRAM_BIN_STATUS__SUCCESS				0x00000000
	#define PNDEV_ASIC_SDRAM_BIN_STATUS__ERROR_KSEG0_1			0x00000001

	//########################################################################
	//  Structures
	//########################################################################

	typedef	struct _uPNDEV_PCI_LOC
	{
		_TCHAR sString[PNDEV_SIZE_STRING_BUF_SHORT];

		struct
		{
			UINT32	lBus;
			UINT32	lDevice;
			UINT32	lFunction;
		}
		uNo;
	}
	uPNDEV_PCI_LOC;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_BLOCK_DESC
	{
		UINT32	lOffsetPhyStart;
		UINT32	lSize;
	}
	uPNDEV_BLOCK_DESC;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES_SINGLE_KM
	{
		PHYSICAL_ADDRESS	uPhyAdr;
		UINT32              lSize;
		volatile UINT8*		pVirtualAdr;
	}
	uPNDEV_HW_RES_SINGLE_KM;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_SDRAM_KM
	{
		UINT32				lSizePhy;
		UINT32				lSizeDriver;
		uPNDEV_BLOCK_DESC	uDirect;
		uPNDEV_BLOCK_DESC	uIndirect[2];
	}
	uPNDEV_HW_DESC_SDRAM_KM;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_KM
	{
		volatile UINT8*	pPtr;
		UINT32			lIndexBar;
		UINT32			lOffsetBar;
		UINT32			lOffsetPhyStart;
		UINT32			lSize;
		UINT32			lAdrAhb;
	}
	uPNDEV_HW_DESC_KM;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES_BOARD
	{
		union
		{



            struct
            {
                uPNDEV_HW_DESC_KM	uBar0;
            }
            uIX1000;
		}
		as;
	}
	uPNDEV_HW_RES_BOARD;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES_KM											// all HwRes at KernelMode
	{
		uPNDEV_HW_RES_SINGLE_KM	uBar[6];
		//						uInt;
		uPNDEV_HW_RES_BOARD		uBoard;
	}
	uPNDEV_HW_RES_KM;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_HW_RES_UM											// HwRes mapped to UserMode of current application
	{
		uPNDEV_HW_RES_SINGLE_UM	uBar[6];
		uPNDEV_HW_RES_SINGLE_UM	uAsicSdramDirect;
	}
	uPNDEV_HW_RES_UM;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_APP
	{
		// Note:
		//	- ChainPtr must be first element of structure!
		void*				pNext;												// ChainPtr for putting object to AppList
		void*				pPrev;												// ChainPtr for putting object to AppList

		HANDLE				hEventUinfo;
		HANDLE				hEventUinfoUm;
		HANDLE				hEventUisr;
		HANDLE				hThreadDll;
		uPNDEV_HW_RES_UM	uHwResUm;
		BOOLEAN				bAllocPageXhif;
		BOOLEAN				bStartedUserFw;
        BOOLEAN             bUsesHostMem;
        UINT32              lUsesHostMemIndex;
        BOOLEAN             bUsesSharedHostMem;
        UINT32              lUsesSharedHostMemIndex;
		uPNDEV_EVENT		uEvent;
	}
	uPNDEV_APP;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_KTICK_RECEIVER
	{
		// Note:
		//	- ChainPtr must be first element of structure!
		void*				pNext;												// ChainPtr for putting object to ReceiverList
		void*				pPrev;												// ChainPtr for putting object to ReceiverList

		HANDLE				hEventUtmrTick;
		HANDLE				hEventUtmrTickSynch;

		UINT32              lSyncTickEvtCount;
	}
	uPNDEV_KTICK_RECEIVER;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_INT_STATISTIC
	{
		UINT32	lCtrSharedInt;
		UINT32	lCtrIsr;
		UINT32	lCtrDpc;
		BOOLEAN	bIsrEntered;
		BOOLEAN	bDpcEntered;
		UINT32	lIsrRuntimeMin_Usec;
		UINT32	lIsrRuntimeMax_Usec;
		UINT32	lIsrDelayAsicMin_Nsec;
		UINT32	lIsrDelayAsicMax_Nsec;

		UINT32	lCtrIsrDelay_LT10_Usec;
		UINT32	lCtrIsrDelay_LT50_Usec;
		UINT32	lCtrIsrDelay_LT100_Usec;
		UINT32	lCtrIsrDelay_LT200_Usec;
		UINT32	lCtrIsrDelay_LT300_Usec;
		UINT32	lCtrIsrDelay_LT400_Usec;
		UINT32	lCtrIsrDelay_LT500_Usec;
		UINT32	lCtrIsrDelay_LT1000_Usec;
		UINT32	lCtrIsrDelay_GE1000_Usec;

		UINT32	lCtrIsrRuntime_LT10_Usec;
		UINT32	lCtrIsrRuntime_LT20_Usec;
		UINT32	lCtrIsrRuntime_LT30_Usec;
		UINT32	lCtrIsrRuntime_LT40_Usec;
		UINT32	lCtrIsrRuntime_LT50_Usec;
		UINT32	lCtrIsrRuntime_GE50_Usec;

		UINT32	lCtrIsrByPnIp;
		UINT32	lCtrIsrByPnIpSpecialIrq;
	}
	uPNDEV_INT_STATISTIC;

    //------------------------------------------------------------------------
	typedef	struct _uPNDEV_HW_DESC_CONT_MEM_HOST
	{
		UINT32				        lPhyAdr;        // physical RAM address fit to size alignment
        UINT32                      lSize;          // size of the contiguous memory
        volatile UINT8*             pAdrVirt;       // virtual address in System Space fit to size alignment
        volatile UINT8*             pAdrVirtStart;  // virtual address of first memory in System Space to be freed
        dma_addr_t      			lDmaAdr;     	// !=0 if memory was allocated with dma_alloc_coherent()
        BOOLEAN				        bInUse;         // indicates if a device uses this memory
        UINT32                      lUseCounter;    // counter for devices which uses this memory (only valid for SharedHostMem)
        uPNDEV_HW_RES_SINGLE_UM     uHwResUserSpace;
	}
	uPNDEV_HW_DESC_CONT_MEM_HOST;

    typedef struct _uPNDEV_MEM_SYS
	{
		uPNDEV_HW_DESC_CONT_MEM_HOST uHostMem[4];           // Memory is allocated at DriverEntry and released at driver unload. Mapping to UserSpace with OpenDevice, Unmapping with CloseDevice		
        uPNDEV_HW_DESC_CONT_MEM_HOST uSharedHostMem[1];     // Memory is allocated at DriverEntry and released at driver unload. Mapping to UserSpace at DriverEntry, at driver unload
	}
	uPNDEV_MEM_SYS;

    extern uPNDEV_MEM_SYS g_uMemSys;

	//------------------------------------------------------------------------
		typedef struct _uPNDEV_VAR_SYS
		{
			dev_t 					uDev;
			struct class* 			pDrvClass;
			struct device* 			pDrvDevice;
			struct cdev 			uDrvCdev;
			struct file_operations 	uFctOps;

			struct mutex			uCritSecIoctlDriver;

			UINT32			lCtrDevice;
			uLIST_HEADER	uListDevice;

		}
		uPNDEV_VAR_SYS;
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// device extension for device object
	typedef struct _DEVICE_EXTENSION
	{
			// Note:
			//	- ChainPtr must be first element of structure!
			void*			        pNext;											// ChainPtr for putting object to DeviceList
			void*			        pPrev;											// ChainPtr for putting object to DeviceList

			struct mutex	        uCritSecIoctl;
			struct mutex	        uCritSecPage;
			struct mutex	        uCritSecAppList;
			spinlock_t              uSpinlockIrq[PNDEV_MSIX_VECTOR_COUNT_MAX];

			struct pci_dev*			pPciDev;
			struct msix_entry		uMsixEntry[PNDEV_MSIX_VECTOR_COUNT_MAX];
			struct irqaction		uIrqAaction[PNDEV_MSIX_VECTOR_COUNT_MAX];

			struct timer_list       uSysThreadTimer;                                //Timer for periodically called thread
			BOOLEAN					bSysThreadTimerStop;							// don't restart periodically timer

		HANDLE					hDeviceOs;										// handle to DeviceObject
		HANDLE					hThreadSys;

		uPNDEV_HW_RES_KM		uHwResKm;
		BOOLEAN					bPtrHwResKmOk;

		ePNDEV_BOARD			eBoard;
		UINT32					lBoardRevision;
		ePNDEV_BOARD_DETAIL		eBoardDetail;
		BOOLEAN					bPcBoard;
		UINT32					lBitStreamId;
		UINT32					lBitStreamVersion;
		UINT32					lBitStreamPatchLabel;
		ePNDEV_ASIC				eAsic;
		ePNDEV_ASIC_DETAIL		eAsicDetail;
		BOOLEAN					bAsicCoreExist;
		volatile BOOLEAN		bResetRunning;
		BOOLEAN					bBootFwRunning;
		BOOLEAN					bUserFwRunning;
		BOOLEAN					bFlashSupport;
		ePNDEV_VAL_HOST_MEM		eValidHostMemPool;
		BOOLEAN					bCopyDataSupport;
		BOOLEAN					bFlrSupport;
		BOOLEAN					bDllReadyFirstOpenDevice;
		UINT32					lTimeCycleThreadSys_10msec;
		UINT32					lTimeScaleFactor;
		ePNDEV_SBL				eSblSoc;
		UINT32					lCtrThreadSys;
		BOOLEAN					bIntConnected;
		ePNDEV_INTERRUPT_MODE	eIntMode;
		UINT32					lIntLine;
		UINT32					lIntVector;
		UINT32					lIntCount;
		UINT32					lMsixTableOffset;
		volatile BOOLEAN		bMsixIntOccured[PNDEV_MSIX_VECTOR_COUNT_MAX];

		uPNDEV_HW_DESC_SDRAM_KM	uAsicSdram;										// AsicSdram without changes of Fw (necessary because Fw can be loaded several times)
		volatile UINT8*			pIrte;
		volatile UINT8*			pPnIp;
		volatile UINT8*			pAdminReg;
		volatile UINT8*			pArmIcu;
		volatile UINT8*			pPerIfIcu;
		volatile UINT8*			pIntelIcu;
		volatile UINT8*			pMsixIcu;
		ePNDEV_ICU				eIcu;
		UINT32					lValueIrteIcuReg_IntMode;
		UINT32					lValuePnIpIcuReg_ControlLow;

		uLIST_HEADER			uListApp;										// list of all AppObjects of this device
		uPNDEV_APP*				pMasterApp;										// MasterApp
		BOOLEAN					bMultiAppLocked;
		UINT32					lCtrApp;

		BOOLEAN					bOsDpcSupport;
		ePNDEV_INT_ACTION		eIntAction;
		BOOLEAN					bLegIsrRunning;
		volatile BOOLEAN		bMsixIsrRunning[PNDEV_MSIX_VECTOR_COUNT_MAX];
		UINT32					lCtrDpcQueued;
		BOOLEAN					bIoctlRunning;

		uPNDEV_CLOCK			uClockStart;
		UINT32					lTimeIrqEdge_Nsec;
		UINT32					lTimeIsrStart_Nsec;
		uPNDEV_INT_STATISTIC	uIntStatistic;

		struct
		{
			ePNDEV_XHIF_PAGE	ePage;
			UINT32				lSegmentAhb;
			UINT32				lCtrRef;
		}
		uUserPageXhif;

		uPNDEV_PCI_LOC	uPciLoc;
		_TCHAR			sPciLocShort	 [PNDEV_SIZE_STRING_BUF_SHORT];
		char			sPciLocShortAscii[PNDEV_SIZE_STRING_BUF_SHORT];

		uPNDEV_SBL_INFO			uSblInfo;
		uPNDEV_EVENT			uEvent;
		uPNCORE_REMANENT_DATA	uRemanentData;
        BOOLEAN                 bNetProfi;

		// correct prototype definition cannot be used because they depend on DEVICE_EXTENSION
		FN_BOARD_XX	pFnBoard_Reset;
		FN_BOARD_XX	pFnBoard_ChangePageXhif;
	}
	DEVICE_EXTENSION;

	//------------------------------------------------------------------------
	typedef	struct _uPNDEV_INT_DETAIL
	{
		struct
		{
			BOOLEAN	bAccessTimeoutInt;
			BOOLEAN	bGpioInt;
			BOOLEAN	bDmaInt;
			BOOLEAN	bDmaErrorInt;
			BOOLEAN	bBoardDemoInt;
			BOOLEAN	bAsicInt;
		}
		uCommon;

		union
		{
			struct
			{
				UINT32	lIrIrt;
				UINT32	lIrNrt;
			}
			uIrteIcu;

			struct
			{
				UINT32	lIrLow;
				UINT32	lIrMid;
				UINT32	lIrHigh;
			}
			uPnipIcu2;

			struct
			{
				UINT32 lIrVec;
			}
			uPciIcu;

			struct
			{
				UINT32 lIrVec;
			}
			uArmIcu;

			struct
			{
				UINT32	lIrLow;
				UINT32	lIrHigh;
			}
			uPerifIcu;

			struct
			{
				UINT32	lIcr;
			}
			uIntel;
			
			struct
			{
				UINT32	lIntst;
			}
			uMicrel;
		}
		as;
	}
	uPNDEV_INT_DETAIL;

	//------------------------------------------------------------------------

	//########################################################################
	//  Function prototypes
	//########################################################################

	typedef BOOLEAN	(*FN_BOARD_RESET)(				DEVICE_EXTENSION*			pDevExtIn,
													const ePNDEV_RESET_ACTION	eActionIn,
													const UINT32				lDimErrorIn,
													_TCHAR*						sErrorOut);
	typedef BOOLEAN	(*FN_BOARD_CHANGE_PAGE_XHIF)(	DEVICE_EXTENSION*			pDevExtIn,
													uPNDEV_APP*					pAppIn,
													const ePNDEV_XHIF_PAGE		ePageIn,
													const UINT32				lSegmentAhbIn,
													const UINT32				lDimErrorIn,
													uPNDEV_HW_DESC*				pHwDescUmOut,
													UINT32*						pCtrRefOut,
													_TCHAR*						sErrorOut);


	//------------------------------------------------------------------------
	// Board.c
	//------------------------------------------------------------------------
	BOOLEAN			fnBoard_InitVar(					DEVICE_EXTENSION*				pDevExtIn);
	BOOLEAN			fnBoard_GetHwInfo(					DEVICE_EXTENSION*				pDevExtIn,
														const UINT32					lDimErrorIn,
														uPNDEV_HW_INFO*					pHwInfoOut,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_GetRemanentData(			DEVICE_EXTENSION*				pDevExtIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	void			fnBoard_ConvertMacAdr(				const UINT64					lMacAdr64In,
														volatile UINT8*					pArrayMacAdr8Out);


	void			fnBoard_StartSdramPage(				DEVICE_EXTENSION*				pDevExtIn,
														const UINT32					lSegmentAhbIn,
														BOOLEAN*						pPagedAccessOut,
														volatile UINT8**				pPtrCurSegmentOut);
	void			fnBoard_StopSdramPage(				DEVICE_EXTENSION*				pDevExtIn);
	void			fnBoard_CopyDataDirect(				uPNDEV_COPY_DATA_IN*			pIn,
														volatile UINT8*					pHostDataIn,
														volatile UINT8*					pAsicDestIn);
	void			fnBoard_CopyDataPaged_Sdram(		DEVICE_EXTENSION*				pDevExtIn,
														uPNDEV_COPY_DATA_IN*			pIn,
														volatile UINT8*					pHostDataIn);
	void			fnBoard_CopyBlockPaged_Sdram(		DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pHostDataIn,
														const UINT32					lOffsetAsicSdramIn,
														const UINT32					lSizeIn,
														const ePNDEV_COPY_ACTION		eActionIn);
	BOOLEAN			fnBoard_CheckVersionBootFw(			DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_CheckSignOfLifeBootFw(		DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					bTimeoutMsecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_DoElfCmdLoadBootFw(			DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lOffsetFwBufIn,
														const UINT32					lIndexFwIn,
														const UINT32					lIndexFwMaxIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_DoElfCmdExecuteBootFw(		DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const BOOLEAN					bDebugModeIn,
														const UINT32					lIndexFwIn,
														const UINT32					lIndexFwMaxIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lTimeStartupSignOfLife_10msecIn,
														uPNCORE_STARTUP_PAR_DRIVER*		pStartupParDriverIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_DoBinCmdExecuteBootFw(		DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_WaitForElfCmdDoneBootFw(	DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lCmdIn,
														_TCHAR*							sActionIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_WaitForCp1625CmdDoneBootFw(	DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														const UINT32					lCmdIn,
														_TCHAR*							sActionIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_WaitForBinCmdDoneBootFw(	DEVICE_EXTENSION*				pDevExtIn,
														volatile UINT8*					pAsicSdramSeg0In,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lCmdIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_CheckElfStatusBootFw(		DEVICE_EXTENSION*				pDevExtIn,
														const UINT32					lStatusIn,
														_TCHAR*							sActionIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_CheckBinStatusBootFw(		DEVICE_EXTENSION*				pDevExtIn,
														const UINT32					lStatusIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_WaitForStartupParUserFw(	DEVICE_EXTENSION*				pDevExtIn,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_WaitForReadyUserFw(			DEVICE_EXTENSION*				pDevExtIn,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN			fnBoard_WaitForAckDebugger(			DEVICE_EXTENSION*				pDevExtIn,
														uPNCORE_CONFIG_PAR*				pConfigParIn,
														const UINT32					lTimeout_10msecIn,
														const UINT32					lWaitTime_10msecIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	void			fnBoard_InitLed(					DEVICE_EXTENSION*				pDevExtIn);
	void			fnBoard_SetLed(						DEVICE_EXTENSION*				pDevExtIn,
														BOOLEAN							bGreenOnIn,
														BOOLEAN							bRedOnIn);
	BOOLEAN 		fnBoard_ConfigPciMaster(			DEVICE_EXTENSION*				pDevExtIn,
														const UINT32					lRegionIdx,
														const UINT32					lPhyAdrIn,
														const UINT32					lSizeIn,
														const UINT32					lDimErrorIn,
														_TCHAR*							sErrorOut);
	BOOLEAN 		fnBoard_SaveRegisterHwReset(		DEVICE_EXTENSION*				pDevExtIn,
														UINT32              			lArrayRegister[],
														const UINT32        			lDimErrorIn,
														_TCHAR*             			sErrorOut);
	BOOLEAN 		fnBoard_RestoreRegisterHwReset(		DEVICE_EXTENSION*				pDevExtIn,
														UINT32           				lArrayRegister[],
														const UINT32     				lDimErrorIn,
														_TCHAR*          				sErrorOut);


	//------------------------------------------------------------------------
	// Board_IsrDpc.c
	//------------------------------------------------------------------------
	void			fnBoard_ChangeIntMaskGlobal(	DEVICE_EXTENSION*	pDevExtIn,
													const BOOLEAN		bEnableIn);
	BOOLEAN			fnBoard_CheckInt(				DEVICE_EXTENSION*	pDevExtIn,
													uPNDEV_INT_DETAIL*	pIntDetailOut);
	void			fnBoard_IsrSetEoi(				DEVICE_EXTENSION*	pDevExtIn);
	void			fnBoard_IsrStandard(			DEVICE_EXTENSION*	pDevExtIn,
													uPNDEV_INT_DETAIL*	pIntDetailOut,
													BOOLEAN*			pReqDpcOut,
													UINT32				lMsgIdMsiIn);
	void			fnBoard_DpcStandard(			DEVICE_EXTENSION*	pDevExtIn,
													BOOLEAN*			pReqUisrOut);
	void			fnBoard_IsrIntTest(				DEVICE_EXTENSION*	pDevExtIn,
													uPNDEV_INT_DETAIL*	pIntDetailOut,
													BOOLEAN*			pReqDpcOut);
	void			fnBoard_DpcIntTest(				DEVICE_EXTENSION*	pDevExtIn,
													BOOLEAN*			pReqUisrOut);
	BOOLEAN			fnBoard_StartIntTest(			DEVICE_EXTENSION*	pDevExtIn,
													const UINT32		lDimErrorIn,
													_TCHAR*				sErrorOut);
	BOOLEAN			fnBoard_SetAsicDemoInt(			DEVICE_EXTENSION*	pDevExtIn,
													const UINT32		lDimErrorIn,
													_TCHAR*				sErrorOut);
	void			fnBoard_MaskAsicDemoInt(		DEVICE_EXTENSION*	pDevExtIn);
	void			fnBoard_UnmaskAsicDemoInt(		DEVICE_EXTENSION*	pDevExtIn);
	BOOLEAN			fnBoard_CheckAsicDemoInt(		DEVICE_EXTENSION*	pDevExtIn,
													uPNDEV_INT_DETAIL*	pIntDetailIn,
													BOOLEAN				bHwCheckIn);

	//------------------------------------------------------------------------
	// Board_Res.c
	//------------------------------------------------------------------------
	BOOLEAN 		fnBoard_GetHwResKm(					DEVICE_EXTENSION*					pDevExtIn,
														HANDLE		        				hOsTransParIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnBoard_VerifyBarConfig(			DEVICE_EXTENSION*					pDevExtIn,
														volatile UINT32*					pArrayBarSizeIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnBoard_SetPtrHwResKm(				DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN 		fnBoard_SetPtrMemMapUM(				uPNDEV_APP*							pAppIn,
														DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalInOut,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnBoard_SetPtrHwResUm(				DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
														uPNDEV_HW_RES*						pHwResUmOpenOut,
														volatile UINT8**					pIcuOut,
														uPNDEV_APP*							pAppOut,
														_TCHAR*								sErrorOut);
	void			fnBoard_SetPtrHwResUm_Single(		uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC*						pHwDescUmOut);
	BOOLEAN			fnBoard_SetPtrHwResUm_Sdram(		DEVICE_EXTENSION*					pDevExtIn,
														uPNDEV_APP*							pAppIn,
														uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														const UINT32						lDimErrorIn,
														uPNDEV_HW_DESC_SDRAM*				pHwDescUmOut,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnBoard_UpdatePtrHwResUm_Sdram(		uPNCORE_STARTUP_PAR_FW*				pStartupParFwIn,
														const UINT32						lDimErrorIn,
														uPNDEV_HW_DESC_SDRAM*				pHwDescUmInOut,
														_TCHAR*								sErrorOut);
	void			fnBoard_SetPtrHwResUm_ApbPer400(	uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC_APB_PER_400*			pHwDescUmOut);
	void			fnBoard_SetPtrHwResUm_ApbPer200(	uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC_APB_PER_200*			pHwDescUmOut);
	void			fnBoard_SetPtrHwResUm_ApbPerSoc1(	uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC_APB_PER_SOC1*		pHwDescUmOut);
	void			fnBoard_SetPtrHwResUm_ApbPer200P(	uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC_APB_PER_200P*		pHwDescUmOut);
	void 			fnBoard_SetPtrHwResUm_ApbPerHera(	UINT32								lBitStreamIdIn,
														uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC_APB_PER_HERA*		pHwDescUmOut);
	void 			fnBoard_SetPtrHwResUm_ApbPerAM5728(	uPNDEV_APP*							pAppIn,
														uPNDEV_HW_DESC_KM*					pHwDescKmIn,
														uPNDEV_HW_DESC_APB_PER_AM5728*		pHwDescUmOut);

	//------------------------------------------------------------------------
	// Device.c
	//------------------------------------------------------------------------
	BOOLEAN			fnAddDevice(				DEVICE_EXTENSION*		    		pDevExtIn,
												HANDLE					    		hOsParIn,
												const BOOLEAN			    		bOsDpcSupportIn,
												const ePNDEV_BOARD		    		eBoardIn,
												const uPNDEV_PCI_LOC*	   	 		pPciLocIn,
												const UINT32			    		lDimErrorIn,
												_TCHAR*					    		sErrorOut);
	BOOLEAN 		fnPrepareHw(				DEVICE_EXTENSION*	        		pDevExtIn,
												HANDLE				        		hOsTransParIn,
												const ePNDEV_RESET_ACTION			eResetActionIn,
												const UINT32		        		lDimErrorIn,
												_TCHAR*				        		sErrorOut);
	BOOLEAN			fnReleaseHw(				DEVICE_EXTENSION*		    		pDevExtIn,
												const UINT32			    		lDimErrorIn,
												_TCHAR*					    		sErrorOut);
	void			fnShutdown(					DEVICE_EXTENSION*		    		pDevExtIn);
	void			fnGetPciLocShort(			DEVICE_EXTENSION*		    		pDevExtIn);
	BOOLEAN			fnCheckSharedHostMemUM(		uPNDEV_APP*							pAppIn,
												DEVICE_EXTENSION*           		pDevExtIn,
		   	   	   	   	   	   	   	   	   	   	BOOLEAN                     		bReqHostMem,
		   	   	   	   	   	   	   	   	   	   	BOOLEAN                             bReqSharedHostMem,
		   	   	   	   	   	   	   	   	   	   	BOOLEAN                     		bMasterApp,
		   	   	   	   	   	   	   	   	   	   	const UINT32		        		lDimErrorIn,
		   	   	   	   	   	   	   	   	   	   	_TCHAR*				        		sErrorOut );
	BOOLEAN			fnMapSharedHostMemUM(		uPNDEV_APP*							pAppIn,
												DEVICE_EXTENSION*					pDevExtIn,
												const UINT32						lDimErrorIn,
												uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
												uPNDEV_SHARED_HOST_SDRAM*			pSharedHostSdram,
												_TCHAR*								sErrorOut);
	void 			fnReleaseSharedHostMemUM(	DEVICE_EXTENSION*					pDevExtIn,
												uPNDEV_APP*							pAppIn,
												const UINT32						lDimErrorIn,
												_TCHAR*								sErrorOut);


	//------------------------------------------------------------------------
	// General.c
	//------------------------------------------------------------------------
	void	fnCheckEventList(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_APP*			pAppExcludedIn,
								BOOLEAN				bThreadSysIn);
	void	fnDoThreadSys(		DEVICE_EXTENSION*	pDevExtIn,
								UINT32				lTimeCycle_10msecIn);
	BOOLEAN	fnTimeLoop(			DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lTimeUsecIn,
								const BOOLEAN		bShowDetailsIn);
	void	fnSetTraceFctError(	DEVICE_EXTENSION*	pDevExtIn,
								_TCHAR*				sErrorIn);

	//------------------------------------------------------------------------
	// IoCtl.c
	//------------------------------------------------------------------------
	ePNDEV_IOCTL	fnDoIoctl(				DEVICE_EXTENSION*				pDevExtIn,
											HANDLE							hOsParIn,
											const UINT32					lIoctlIn,
											const UINT32					lSizeInputBufIn,
											const UINT32					lSizeOutputBufIn,
											void*							pInputBufIn,
											void*							pOutputBufIn);
	BOOLEAN			fnIoctlOpenDevice(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_OPEN_DEVICE_IN*			pIn,
											uPNDEV_OPEN_DEVICE_OUT*			pOut);
	BOOLEAN			fnIoctlCloseDevice(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_CLOSE_DEVICE_IN*			pIn,
											uPNDEV_CLOSE_DEVICE_OUT*		pOut);
	BOOLEAN			fnIoctlResetDevice(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_RESET_DEVICE_IN*			pIn,
											uPNDEV_RESET_DEVICE_OUT*		pOut);
	BOOLEAN			fnIoctlGetEventList(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_GET_EVENT_LIST_IN*		pIn,
											uPNDEV_GET_EVENT_LIST_OUT*		pOut);
	BOOLEAN			fnIoctlSetEventList(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_SET_EVENT_LIST_IN*		pIn,
											uPNDEV_SET_EVENT_LIST_OUT*		pOut);
	BOOLEAN			fnIoctlChangeInt(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_CHANGE_INT_IN*			pIn,
											uPNDEV_CHANGE_INT_OUT*			pOut);
	BOOLEAN			fnIoctlGetDeviceState(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_GET_DEVICE_STATE_IN*		pIn,
											uPNDEV_GET_DEVICE_STATE_OUT*	pOut);

	

	BOOLEAN			fnIoctlChangeXhifPage(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_CHANGE_XHIF_PAGE_IN*		pIn,
											uPNDEV_CHANGE_XHIF_PAGE_OUT*	pOut);
	BOOLEAN			fnIoctlCopyData(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_COPY_DATA_IN*			pIn,
											uPNDEV_COPY_DATA_OUT*			pOut);
	BOOLEAN			fnIoctlStartBootFw(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_START_BOOT_FW_IN*		pIn,
											uPNDEV_START_BOOT_FW_OUT*		pOut);
	BOOLEAN			fnIoctlStartUserFw(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_START_USER_FW_IN*		pIn,
											uPNDEV_START_USER_FW_OUT*		pOut);
	BOOLEAN			fnIoctlStartUserFwLinux(DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_START_USER_FW_LINUX_IN*	pIn,
											uPNDEV_START_USER_FW_LINUX_OUT*	pOut);
	BOOLEAN			fnIoctlIsUserFwReady(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_IS_USER_FW_READY_IN*		pIn,
											uPNDEV_IS_USER_FW_READY_OUT*	pOut);
	BOOLEAN			fnIoctlChangeAppLock(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_CHANGE_APP_LOCK_IN*		pIn,
											uPNDEV_CHANGE_APP_LOCK_OUT*		pOut);
	BOOLEAN			fnIoctlNotifyDllState(	DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_NOTIFY_DLL_STATE_IN*		pIn,
											uPNDEV_NOTIFY_DLL_STATE_OUT*	pOut);
	ePNDEV_IOCTL	fnCheckIoctlPar(		const UINT32					lIoctlIn,
											const UINT32					lSizeInputBufIn,
											const UINT32					lSizeOutputBufIn);
    BOOLEAN         fnIoctlDoNetProfi(	    DEVICE_EXTENSION*               pDevExtIn,
                                            uPNDEV_NP_DO_NETPROFI_REQ_IN*   pIn,
					                        uPNDEV_NP_DO_NETPROFI_REQ_OUT*  pOut);
    BOOLEAN			fnIoctlMapMemory(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_MAP_MEMORY_IN*			pIn,
											uPNDEV_MAP_MEMORY_OUT*			pOut);
    BOOLEAN			fnIoctlSetHwResUm(		DEVICE_EXTENSION*				pDevExtIn,
    										uPNDEV_OPEN_DEVICE_OUT*			pIn,
    										uPNDEV_OPEN_DEVICE_OUT*			pOut);
    BOOLEAN 		fnIoctlGetIntState(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_GET_INT_STATE_IN*		pIn,
											uPNDEV_GET_INT_STATE_OUT*		pOut);
    BOOLEAN	        fnNetProfiIntHandler(   DEVICE_EXTENSION*               pDevExtIn);
	BOOLEAN			fnFileOpenForRead(		DEVICE_EXTENSION*				pDevExtIn,
											HANDLE*							pFileHandle,
											_TCHAR*							sPath,
											UINT32*							lFileSize);
	BOOLEAN			fnFileClose(			DEVICE_EXTENSION*				pDevExtIn,
											HANDLE							hFile);
	BOOLEAN			fnFileReadBytes(		DEVICE_EXTENSION*				pDevExtIn,
											HANDLE							hFile,
											UINT32							lFileOffset,
											UINT32							lBufferSize,
											UINT8*							pDataBuffer);
	BOOLEAN			fnCloseDevice(			DEVICE_EXTENSION*				pDevExtIn,
											PFD_MEM_ATTR uPNDEV_APP*		pAppIn,
											const UINT32					lDimErrorIn,
											_TCHAR*							sErrorOut);
	BOOLEAN			fnIoctlOpenTimer(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_OPEN_TIMER_IN*			pIn,
											uPNDEV_OPEN_TIMER_OUT*			pOut);
	BOOLEAN			fnIoctlCloseTimer(		DEVICE_EXTENSION*				pDevExtIn,
											uPNDEV_CLOSE_TIMER_IN*			pIn,
											uPNDEV_CLOSE_TIMER_OUT*			pOut);

	//------------------------------------------------------------------------
	// IsrDpc.c
	//------------------------------------------------------------------------
	BOOLEAN	fnIsrLeg(				DEVICE_EXTENSION*	pDevExtIn,
									HANDLE				hOsParIn,
									UINT32				lMsgIdMsiIn);
	BOOLEAN	fnIsrMsix(				DEVICE_EXTENSION*	pDevExtIn,
									HANDLE				hOsParIn,
									UINT32				lMsgIdMsiIn);
	void 	fnRequestDpc(			DEVICE_EXTENSION* 	pDevExtIn,
									HANDLE            	hOsParIn);
	void	fnDpc(					DEVICE_EXTENSION*	pDevExtIn);
	void	fnEvtIsrSharedInt(		DEVICE_EXTENSION*	pDevExtIn);
	void	fnUpdateStatisticIsr(	DEVICE_EXTENSION*	pDevExtIn);
	void	fnUpdateStatisticDpc(	DEVICE_EXTENSION*	pDevExtIn);
	void	fnStartRuntimeIsr(		DEVICE_EXTENSION*	pDevExtIn);
	void	fnStopRuntimeIsr(		DEVICE_EXTENSION*	pDevExtIn);

	//------------------------------------------------------------------------
	// os.c
	//------------------------------------------------------------------------
	// Note:
	//	- prototypes cannot be placed at os.h
	//	- otherwise os.h needs private.h (e.g. DEVICE_EXTENSION) and reverse (e.g. PHYSICAL_ADDRESS)


			BOOLEAN		fnEvtDeviceAdd(			DEVICE_EXTENSION*	    pDevExtIn,
												ePNDEV_BOARD		    eBoardIn,
												UINT32				    lNoBusIn,
												UINT32				    lNoDeviceIn,
												UINT32				    lNoFunctionIn);
			BOOLEAN		fnEvtDevicePrepareHw(	DEVICE_EXTENSION*	    pDevExtIn);
			BOOLEAN		fnEvtDeviceReleaseHw(	DEVICE_EXTENSION*	    pDevExtIn);
			irqreturn_t fnEvtIsr(               int                     lMsgIdMsiIn,
			                                    void*                   pFctArgIn);
			int 		fnOpenDriver(			struct inode*		    inode,
												struct file*		    file);
			int 		fnCloseDriver(			struct inode*		    inode,
												struct file *		    file);
			BOOLEAN 	RetrieveIoctlBuffer(	unsigned int 		    lCmdIn,
												unsigned long 		    lArgIn,
												uPNDEV_IOCTL_ARG*	    pIoctlArgIn,
												uPNDEV64_PTR_VOID*	    pInBufferIn,
												uPNDEV64_PTR_VOID*	    pOutBufferIn);
			BOOLEAN 	ReturnIoctlBuffer(		uPNDEV_IOCTL_ARG*	    pIoctlArgIn,
												uPNDEV64_PTR_VOID*	    pInBufferIn,
												uPNDEV64_PTR_VOID*	    pOutBufferIn);
			long 		fnEvtIoctl(				struct file* 		    pFdIn,
												unsigned int 		    lCmdIn,
												unsigned long 		    lArgIn);
			int 		fnEvtMmap(				struct file* 		    pFdIn,
												struct vm_area_struct*  pVmaIn);
			BOOLEAN 	fnCheckPciLocIn(		PNDEV_CHAR*			    sPciLocIn[],
												const UINT32		    lDimArrayIn,
												UINT32				    pBusIn,
												UINT32				    pDevIn,
												UINT32				    pFuncIn);
			//void	fnThreadSys(			    union sigval		    uThreadParIn);
			int			fnEvtDeviceProbe(		struct pci_dev*			pPciDevIn,
										const	struct pci_device_id*	pPciDevIdIn);
			void		fnEvtDeviceRemove(		struct pci_dev*			pPciDevIn);

	//------------------------------------------------------------------------
	BOOLEAN			fnStartUpLockRes(					DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnShutDownLockRes(					DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	void 			fnAcquireSpinLockIrq(				DEVICE_EXTENSION*					pDevExtIn,
														UINT32								lIndexIn);
	void			fnReleaseSpinLockIrq(				DEVICE_EXTENSION*					pDevExtIn,
														UINT32								lIndexIn);
	void			fnAcquireSpinLock(					DEVICE_EXTENSION*					pDevExtIn,
														ePNDEV_SPIN_LOCK					eSpinLockIn);
	void			fnReleaseSpinLock(					DEVICE_EXTENSION*					pDevExtIn,
														ePNDEV_SPIN_LOCK					eSpinLockIn);
	void			fnEnterCritSec(						DEVICE_EXTENSION*					pDevExtIn,
														ePNDEV_CRIT_SEC_SYS					eCritSecIn);
	void			fnLeaveCritSec(						DEVICE_EXTENSION*					pDevExtIn,
														ePNDEV_CRIT_SEC_SYS					eCritSecIn);
	BOOLEAN			fnStartUpThreadResSys(				DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lPrioThreadIn,
									                    const UINT32		                lStackSizeThreadIn,
									                    const _TCHAR*                       sNameThreadIn,
														const UINT32						lDimErrorIn,
														UINT64*								pThreadIdOut,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnShutDownThreadResSys(				DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnStartUpTickEvtTimer(				uPNDEV_KTICK_RECEIVER*				pReceiverIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnShutDownTickEvtTimer(				uPNDEV_KTICK_RECEIVER*				pReceiverIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnStartUpThreadResDll(				DEVICE_EXTENSION*					pDevExtIn,
														uPNDEV_APP*							pAppIn,
														uPNDEV_THREAD_IOCTL*				pThreadIoctlIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnShutDownThreadResDll(				DEVICE_EXTENSION*					pDevExtIn,
														uPNDEV_APP*							pAppIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	void			fnSetEventThreadDll(				DEVICE_EXTENSION*					pDevExtIn,
														uPNDEV_APP*							pAppIn,
														ePNDEV_EVENT_THREAD_DLL				eEventIn,
														BOOLEAN								bThreadSysIn);
	BOOLEAN			fnStartUpTimerRes(					uPNDEV_KTICK_RECEIVER*				pReceiverIn,
														uPNDEV_KTIMER_IOCTL*				pTimerIoctlIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnShutDownTimerRes(					uPNDEV_KTICK_RECEIVER*				pReceiverIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	ePNDEV_IOCTL	fnGetIoctlBuf(						HANDLE								hOsParIn,
														const UINT32						lSizeInputBufIn,
														const UINT32						lSizeOutputBufIn,
														void**								pPtrInputBufOut,
														void**								pPtrOutputBufOut);
	BOOLEAN			fnIoctlGetDeviceInfoSys(			uPNDEV_GET_DEVICE_INFO_SYS_IN*		pIn,
														uPNDEV_GET_DEVICE_INFO_SYS_OUT*		pOut);
	BOOLEAN			fnIoctlGetDeviceHandleSys(			uPNDEV_GET_DEVICE_HANDLE_SYS_IN*	pIn,
														uPNDEV_GET_DEVICE_HANDLE_SYS_OUT*	pOut);
	BOOLEAN			fnIoctlAllocEventId(				uPNDEV_ALLOC_EVENT_ID_IN*			pIn,
														uPNDEV_ALLOC_EVENT_ID_OUT*			pOut);
	BOOLEAN			fnIoctlFreeEventId(					uPNDEV_FREE_EVENT_ID_IN*			pIn,
														uPNDEV_FREE_EVENT_ID_OUT*			pOut);
	BOOLEAN 		fnConnectInt(   					DEVICE_EXTENSION*       			pDevExtIn,
														HANDLE				   	 			hOsRawParIn,
														HANDLE				    			hOsTransParIn,
														const UINT32            			lDimErrorIn,
														_TCHAR*                 			sErrorOut);
	BOOLEAN			fnDisconnectInt(					DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnRegisterIsr(						DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lIntVectorIn,
														const UINT32        				lMsgIndexIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnQueueDpc(							DEVICE_EXTENSION*					pDevExtIn,
														HANDLE								hOsParIn);
	void*			fnAllocMemNonPaged(					size_t								lSizeIn,
														ULONG								lTagIn);
	void			fnFreeMemNonPaged(					void*								pMemIn,
														ULONG								lTagIn);
	BOOLEAN			fnMapMemPhyToVirtual(				DEVICE_EXTENSION*					pDevExtIn,
														uPNDEV_HW_RES_SINGLE_KM*			pMemKmInOut,
														const UINT32						lBarIndexIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	void			fnReleaseMemVirtual(				DEVICE_EXTENSION*					pDevExtIn,
														uPNDEV_HW_RES_SINGLE_KM*			pMemKmInOut);
	BOOLEAN			fnMapMemVirtualToUser(				DEVICE_EXTENSION*					pDevExtIn,
														const void*							pVirtualAdrIn,
														const UINT32						lSizeMemIn,
														const UINT32						lDimErrorIn,
														uPNDEV_HW_RES_SINGLE_UM*			pMemUmOut,
														_TCHAR*								sErrorOut);
	void			fnReleaseMemUser(					DEVICE_EXTENSION*					pDevExtIn,
														const uPNDEV_HW_RES_SINGLE_UM*		pMemUmIn);
	BOOLEAN			fnDelayThread(						DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lTimeMsecIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	void			fnGetClock(							uPNDEV_CLOCK*						pClockOut);
	UINT32			fnComputeRunTimeUsec(				uPNDEV_CLOCK*						pClockStartIn);
	void			fnSetTrace(							const ePNDEV_TRACE_LEVEL			eLevelIn,
														const ePNDEV_TRACE_CONTEXT			eContextIn,
														const BOOLEAN						bLogIn,
														char*								sTraceIn);
	BOOLEAN			fnCheckIrql(						const KIRQL							lIrqlMaxIn,
														KIRQL*								pIrqlCurOut);
	BOOLEAN 		fnGetResListConfigSpace(			DEVICE_EXTENSION*   				pDevExtIn,
														HANDLE		        				hOsTransParIn,          // hResTranslatedIn
														const UINT32        				lFirstBarIn,
														const UINT32        				lDimErrorIn,
														_TCHAR*             				sErrorOut);
	BOOLEAN			fnGetInfoConfigSpace(				DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														uPNDEV_CONFIG_SPACE*				pConfigSpaceOut,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnGetBarConfigSpace(				DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lBarIndexIn,
														const BOOLEAN						bGetSizeIn,
														const UINT32						lDimErrorIn,
							                            UINT32*             				pPhyAdrOut,
							                            UINT32*             				pSizeOut,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnGetExtendedConfigSpace(			DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lOffsetCapabilityIn,
														const UINT32						lDimErrorIn,
														uPNDEV_CONFIG_SPACE*				pConfigSpaceOut,
														_TCHAR*								sErrorOut);
    BOOLEAN fnCheckConfigSpaceExpressCapabilityExists(  DEVICE_EXTENSION*                   pDevExtIn,
														const UINT8							lCapabilityIn,
                                                        const UINT32                        lDimErrorIn,
                                                        UINT32*                             lOffsetOut,
                                                        _TCHAR*                             sErrorOut);
	BOOLEAN			fnPrepareConfigSpaceHwReset(		DEVICE_EXTENSION*					pDevExtIn,
														const UINT32						lDimErrorIn,
														UINT32								lArrayConfigSpaceOut[],                                                        
                                                        UINT32								lArrayExpressCapabilityOut[],
                                                        UINT32*                             lOffsetExpressCapabilityOut,
														_TCHAR*								sErrorOut);
	BOOLEAN			fnRestoreConfigSpaceHwReset(		DEVICE_EXTENSION*					pDevExtIn,
														UINT32								lArrayConfigSpaceIn[],
                                                        UINT32								lArrayExpressCapabilityIn[],  
                                                        UINT32                              lOffsetExpressCapabilityIn,
														const UINT32						lDimErrorIn,
														_TCHAR*								sErrorOut);
	BOOLEAN 		fnEnableBusMasterConfigSpace(		DEVICE_EXTENSION*   				pDevExtIn,
	                                        			const UINT32        				lDimErrorIn,
	                                        			_TCHAR*             				sErrorOut);
	BOOLEAN 		fnDisableBusMasterConfigSpace(  	DEVICE_EXTENSION*   				pDevExtIn,
	                                        			const UINT32        				lDimErrorIn,
	                                        			_TCHAR*             				sErrorOut);
	UINT32 			fnReadConfigSpaceBitStreamVer(		DEVICE_EXTENSION*  					pDevExtIn,
														UINT32								lOffsetIn,
	                                					const UINT32        				lDimErrorIn,
	                                					_TCHAR*             				sErrorOut);
	BOOLEAN 		fnCheckTransactionPending(			DEVICE_EXTENSION*					pDevExtIn,
	                                					const UINT32        				lDimErrorIn,
	                                					_TCHAR*             				sErrorOut);
	BOOLEAN 		fnSetFlrAtConfigSpace(				DEVICE_EXTENSION*					pDevExtIn,
	                                					const UINT32        				lDimErrorIn,
	                                					_TCHAR*             				sErrorOut);
	BOOLEAN 		fnGetVendorDeviceIdAtConfigSpace(	DEVICE_EXTENSION*					pDevExtIn,
														UINT16*								pVendorIdOut,
														UINT16*								pDeviceIdOut,
														const UINT32        				lDimErrorIn,
														_TCHAR*             				sErrorOut);
	BOOLEAN			fnEnableMsixAtConfigSpace(			DEVICE_EXTENSION*					pDevExtIn,
														const UINT32        				lDimErrorIn,
														_TCHAR*             				sErrorOut);
	BOOLEAN			fnScanDevice(						const UINT32						lVendorIdIn,
														const UINT32						lDeviceIdIn,
														const UINT32						lSubsystemIdIn,
														const ePNDEV_BOARD					eBoardIn);
	ePNDEV_BOARD	fnGetBoardType(						const _TCHAR*						sHardwareIdIn);


   void             fnMmAllocateContiguousMemory(	UINT32	                            lSizeIn,
                                                    const UINT32                        lAdrMinIn,
                                                    const UINT32                        lAdrMaxIn,
                                                    uPNDEV_HW_DESC_CONT_MEM_HOST*       pContMemOut);
   void             fnMmFreeContiguousMemory(	    uPNDEV_HW_DESC_CONT_MEM_HOST*       pContMemIn);

	//------------------------------------------------------------------------
	// String.c
	//------------------------------------------------------------------------
	UINT32 fnBuildString(	const _TCHAR*	sSrc1In,
							const _TCHAR*	sSrc2In,
							const _TCHAR*	sSrc3In,
							const UINT32	lDimResultIn,
							_TCHAR*			sResultOut);

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ - compiler: End of C linkage
	}
	#endif

#endif