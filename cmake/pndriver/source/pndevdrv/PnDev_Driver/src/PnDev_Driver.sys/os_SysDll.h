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
/*  F i l e               &F: os_SysDll.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	OS specific declarations common for SYS/DLL
//
//****************************************************************************/

#ifndef __os_SysDll_h__
#define __os_SysDll_h__

	#ifdef __cplusplus		// if C++ compiler: Use C linkage
	extern "C"
	{
	#endif

	//########################################################################
	//  Defines
	//########################################################################

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	// {70EF9C99-5112-45a3-901C-C69AD2C63F59}
	DEFINE_GUID(GUID_PNDEV_IF,
				0x70ef9c99, 0x5112, 0x45a3, 0x90, 0x1c, 0xc6, 0x9a, 0xd2, 0xc6, 0x3f, 0x59);

	//------------------------------------------------------------------------
	// VendorId & DeviceId & SUBSYS_SubSystemId/Subsystem VendorId
	#define	PNDEV_HW_ID_CP1616				_TEXT("PCI\\VEN_110a&DEV_4036")
	#define	PNDEV_HW_ID_DB_EB400_PCIE		_TEXT("PCI\\VEN_110a&DEV_408c")
	#define	PNDEV_HW_ID_EB200				_TEXT("PCI\\VEN_110a&DEV_4039")
	#define	PNDEV_HW_ID_DB_EB200_PCIE		_TEXT("PCI\\VEN_110a&DEV_409A")
	#define	PNDEV_HW_ID_DB_SOC1_PCI			_TEXT("PCI\\VEN_110a&DEV_403e")
	#define	PNDEV_HW_ID_DB_SOC1_PCIE		_TEXT("PCI\\VEN_110a&DEV_408a")

	#define	PNDEV_HW_ID_FPGA1_ERTEC200P		_TEXT("PCI\\VEN_110a&DEV_406B&SUBSYS_0001110a")		// SubSystemId=1, Subsystem VendorId=110a

	#define	PNDEV_HW_ID_EB200P				_TEXT("PCI\\VEN_110a&DEV_4073")
	#define	PNDEV_HW_ID_I210				_TEXT("PCI\\VEN_8086&DEV_1533")
	#define	PNDEV_HW_ID_I210SFP				_TEXT("PCI\\VEN_8086&DEV_1536")
	#define	PNDEV_HW_ID_I82574				_TEXT("PCI\\VEN_8086&DEV_10D3")
	
	#define	PNDEV_HW_ID_FPGA1_HERA			_TEXT("PCI\\VEN_110A&DEV_4090&SUBSYS_4090110A")
	
	#define	PNDEV_HW_ID_MICREL_KSZ8841		_TEXT("PCI\\VEN_16C6&DEV_8841")
	#define	PNDEV_HW_ID_MICREL_KSZ8842		_TEXT("PCI\\VEN_16C6&DEV_8842")
	
	#define	PNDEV_HW_ID_TI_AM5728			_TEXT("PCI\\VEN_104C&DEV_8888")

	#define	PNDEV_HW_ID_CP1625				_TEXT("PCI\\VEN_110a&DEV_4060")

	//------------------------------------------------------------------------
	// Oney S.422: Operations that will transfer more than a page of data should use one of the direct methods
	//	- ntddk.h: #define PAGE_SIZE 0x1000 (=4kB)

	#define IOCTL_OPEN_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_CLOSE_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_RESET_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_GET_EVENT_LIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_SET_EVENT_LIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_CHANGE_INT			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_GET_DEVICE_STATE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_PROGRAM_FLASH			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_CHANGE_XHIF_PAGE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_COPY_DATA				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_DIRECT_FROM_HARDWARE,	FILE_ANY_ACCESS)	// =METHOD_OUT_DIRECT
	#define IOCTL_START_BOOT_FW			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80a, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_START_USER_FW			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80b, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_CHANGE_APP_LOCK		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80c, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_NOTIFY_DLL_STATE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80d, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_GET_DEVICE_INFO_SYS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80e, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_GET_DEVICE_HANDLE_SYS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80f, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_ALLOC_EVENT_ID		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_FREE_EVENT_ID			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED,				FILE_ANY_ACCESS)
    #define IOCTL_DO_NETPROFI 		    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_MAP_MEMORY 		    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_SET_HW_RES_UM			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_GET_INT_STATE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_IS_USER_FW_READY		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_OPEN_TIMER			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_CLOSE_TIMER			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x819, METHOD_BUFFERED,				FILE_ANY_ACCESS)
	#define IOCTL_START_USER_FW_LINUX	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81a, METHOD_BUFFERED,				FILE_ANY_ACCESS)


	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------
	#define	PNDEV_VENDOR_ID_SIEMENS			0x110a
	#define	PNDEV_VENDOR_ID_INTEL			0x8086
	#define	PNDEV_VENDOR_ID_MICREL			0x16C6
	#define	PNDEV_VENDOR_ID_TI				0x104C

	#define	PNDEV_DEVICE_ID_CP1616			0x4036
	#define	PNDEV_DEVICE_ID_DB_EB400_PCIE	0x408c
	#define	PNDEV_DEVICE_ID_EB200			0x4039
	#define	PNDEV_DEVICE_ID_DB_EB200_PCIE	0x409A
	#define	PNDEV_DEVICE_ID_DB_SOC1_PCI		0x403e
	#define	PNDEV_DEVICE_ID_DB_SOC1_PCIE	0x408a
	#define	PNDEV_DEVICE_ID_FPGA1			0x406B
	#define	PNDEV_DEVICE_ID_EB200P			0x4073
	#define	PNDEV_DEVICE_ID_I210			0x1533
	#define	PNDEV_DEVICE_ID_I210SFP			0x1536
	#define	PNDEV_DEVICE_ID_I82574			0x10D3
	#define PNDEV_DEVICE_ID_IX1000			0x0937
	
	#define	PNDEV_DEVICE_ID_FPGA1_HERA		0x4090
	
	#define	PNDEV_DEVICE_ID_MICREL_KSZ8841	0x8841
	#define	PNDEV_DEVICE_ID_MICREL_KSZ8842	0x8842
	
	#define	PNDEV_DEVICE_ID_TI_AM5728		0x8888

	#define	PNDEV_SUBSYSTEM_ID_FPGA1_ERTEC200P		1
	#define	PNDEV_SUBSYSTEM_ID_FPGA1_HERA			0x4090110A
    
    #define	PNDEV_DEVICE_ID_CP1625			0x4060
	
	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
		#define PNDEV_ANY_ID                PCI_ANY_ID
	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------
		#define PNDEV_ANY_ID                0
	#endif
	
	#define PNDEV_SCAN_DEVICES 	\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_CP1616,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_CP1616},			/*CP1616 board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_EB200,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_EB200},			/*EB200 board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_DB_SOC1_PCI,	PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_DB_SOC1_PCI},	/*DB-Soc1-PCI board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_DB_SOC1_PCIE,	PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_DB_SOC1_PCIE},	/*DB-Soc1-PCIe board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_FPGA1,			PNDEV_VENDOR_ID_SIEMENS,	PNDEV_SUBSYSTEM_ID_FPGA1_ERTEC200P,	0, 0, ePNDEV_BOARD_FPGA1_ERTEC200P},/*FPGA1-Ertec200P board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_EB200P,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_EB200P},			/*EB200P board*/\
			{PNDEV_VENDOR_ID_INTEL,		PNDEV_DEVICE_ID_I210,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_I210},			/*I210 board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_DB_EB400_PCIE,	PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_DB_EB400_PCIE},	/*DB-EB400-PCIe board*/\
			{PNDEV_VENDOR_ID_INTEL,		PNDEV_DEVICE_ID_I82574,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_I82574},			/*IntelI82574 board*/\
			{PNDEV_VENDOR_ID_MICREL,	PNDEV_DEVICE_ID_MICREL_KSZ8841,	PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_MICREL_KSZ8841},	/*MicrelKSZ8841 board*/\
			{PNDEV_VENDOR_ID_MICREL,	PNDEV_DEVICE_ID_MICREL_KSZ8842,	PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_MICREL_KSZ8842},	/*MicrelKSZ8842 board*/\
			{PNDEV_VENDOR_ID_TI,		PNDEV_DEVICE_ID_TI_AM5728,		PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_TI_AM5728},		/*TIAM5728 board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_FPGA1_HERA,		PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_FPGA1_HERA},		/*FPGA1-HERA board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_DB_EB200_PCIE,	PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_DB_EB200_PCIE},	/*DB-EB200-PCIe board*/\
			{PNDEV_VENDOR_ID_INTEL,		PNDEV_DEVICE_ID_I210SFP,		PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_I210SFP},		/*I210SFP board*/\
			{PNDEV_VENDOR_ID_INTEL,		PNDEV_DEVICE_ID_IX1000,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_IX1000},			/*IntelQuarkX1000 (on Galileo) board*/\
			{PNDEV_VENDOR_ID_SIEMENS,	PNDEV_DEVICE_ID_CP1625,			PNDEV_ANY_ID,				PNDEV_ANY_ID,						0, 0, ePNDEV_BOARD_CP1625}			/*CP1625 board*/


		//------------------------------------------------------------------------
		#if defined (PNDEV_OS_ADONIS)
		//------------------------------------------------------------------------
			#define IOCTL_OPEN_DEVICE			1
			#define IOCTL_CLOSE_DEVICE			2
			#define IOCTL_RESET_DEVICE			3
			#define IOCTL_GET_EVENT_LIST		4
			#define IOCTL_SET_EVENT_LIST		5
			#define IOCTL_CHANGE_INT			6
			#define IOCTL_GET_DEVICE_STATE		7
			#define IOCTL_PROGRAM_FLASH			8
			#define IOCTL_CHANGE_XHIF_PAGE		9
			#define IOCTL_COPY_DATA				10
			#define IOCTL_START_BOOT_FW			11
			#define IOCTL_START_USER_FW			12
			#define IOCTL_CHANGE_APP_LOCK		13
			#define IOCTL_NOTIFY_DLL_STATE		14
			#define IOCTL_GET_DEVICE_INFO_SYS	15
			#define IOCTL_GET_DEVICE_HANDLE_SYS	16
			#define IOCTL_ALLOC_EVENT_ID		17
			#define IOCTL_FREE_EVENT_ID			18
			#define IOCTL_DO_NETPROFI 		    19
			#define IOCTL_MAP_MEMORY 		    20
			#define IOCTL_SET_HW_RES_UM			21
			#define IOCTL_GET_INT_STATE			22
			#define IOCTL_IS_USER_FW_READY		23
			#define IOCTL_OPEN_TIMER			24
			#define IOCTL_CLOSE_TIMER			25
            #define IOCTL_GET_SHARED_HOST_MEM_ADDR      26

		//------------------------------------------------------------------------
		#elif defined (PNDEV_OS_LINUX)
		//------------------------------------------------------------------------
			#define PNDEV_DRV_NAME				"PnDevDrv/pndevdrv"
			#define PNDEV_PCI_DRV_NAME			"pndevdrv"
			#define PNDEV_DRV_NAME_UM			"/dev/PnDevDrv/pndevdrv"
			#define PNDEV_IOC_MAGIC				'p'+'n'										// MagicNumber ProfiNet

			#define IOCTL_OPEN_DEVICE			_IOWR(PNDEV_IOC_MAGIC, 1, uPNDEV_IOCTL_ARG)
			#define IOCTL_CLOSE_DEVICE			_IOWR(PNDEV_IOC_MAGIC, 2, uPNDEV_IOCTL_ARG)
			#define IOCTL_RESET_DEVICE			_IOWR(PNDEV_IOC_MAGIC, 3, uPNDEV_IOCTL_ARG)
			#define IOCTL_GET_EVENT_LIST		_IOWR(PNDEV_IOC_MAGIC, 4, uPNDEV_IOCTL_ARG)
			#define IOCTL_SET_EVENT_LIST		_IOWR(PNDEV_IOC_MAGIC, 5, uPNDEV_IOCTL_ARG)
			#define IOCTL_CHANGE_INT			_IOWR(PNDEV_IOC_MAGIC, 6, uPNDEV_IOCTL_ARG)
			#define IOCTL_GET_DEVICE_STATE		_IOWR(PNDEV_IOC_MAGIC, 7, uPNDEV_IOCTL_ARG)
			#define IOCTL_PROGRAM_FLASH			_IOWR(PNDEV_IOC_MAGIC, 8, uPNDEV_IOCTL_ARG)
			#define IOCTL_CHANGE_XHIF_PAGE		_IOWR(PNDEV_IOC_MAGIC, 9, uPNDEV_IOCTL_ARG)
			#define IOCTL_COPY_DATA				_IOWR(PNDEV_IOC_MAGIC, 10, uPNDEV_IOCTL_ARG)
			#define IOCTL_START_BOOT_FW			_IOWR(PNDEV_IOC_MAGIC, 11, uPNDEV_IOCTL_ARG)
			#define IOCTL_START_USER_FW			_IOWR(PNDEV_IOC_MAGIC, 12, uPNDEV_IOCTL_ARG)
			#define IOCTL_CHANGE_APP_LOCK		_IOWR(PNDEV_IOC_MAGIC, 13, uPNDEV_IOCTL_ARG)
			#define IOCTL_NOTIFY_DLL_STATE		_IOWR(PNDEV_IOC_MAGIC, 14, uPNDEV_IOCTL_ARG)
			#define IOCTL_GET_DEVICE_INFO_SYS	_IOWR(PNDEV_IOC_MAGIC, 15, uPNDEV_IOCTL_ARG)
			#define IOCTL_GET_DEVICE_HANDLE_SYS	_IOWR(PNDEV_IOC_MAGIC, 16, uPNDEV_IOCTL_ARG)
			#define IOCTL_ALLOC_EVENT_ID		_IOWR(PNDEV_IOC_MAGIC, 17, uPNDEV_IOCTL_ARG)
			#define IOCTL_FREE_EVENT_ID			_IOWR(PNDEV_IOC_MAGIC, 18, uPNDEV_IOCTL_ARG)
			#define IOCTL_DO_NETPROFI 		    _IOWR(PNDEV_IOC_MAGIC, 19, uPNDEV_IOCTL_ARG)
			#define IOCTL_MAP_MEMORY 		    _IOWR(PNDEV_IOC_MAGIC, 20, uPNDEV_IOCTL_ARG)
			#define IOCTL_SET_HW_RES_UM			_IOWR(PNDEV_IOC_MAGIC, 21, uPNDEV_IOCTL_ARG)
			#define IOCTL_GET_INT_STATE			_IOWR(PNDEV_IOC_MAGIC, 22, uPNDEV_IOCTL_ARG)
			#define IOCTL_IS_USER_FW_READY		_IOWR(PNDEV_IOC_MAGIC, 23, uPNDEV_IOCTL_ARG)
			#define IOCTL_OPEN_TIMER			_IOWR(PNDEV_IOC_MAGIC, 24, uPNDEV_IOCTL_ARG)
			#define IOCTL_CLOSE_TIMER			_IOWR(PNDEV_IOC_MAGIC, 25, uPNDEV_IOCTL_ARG)
			#define IOCTL_START_USER_FW_LINUX	_IOWR(PNDEV_IOC_MAGIC, 26, uPNDEV_IOCTL_ARG)

		#endif

	#endif


	//########################################################################
	//  Structures
	//########################################################################

	// Note:
	//	- ensure that driver and DLL use the same alignment!
//	#pragma pack(show)
	#pragma pack(push)															// save current packing alignment
    //------------------------------------------------------------------------
    #if defined PNDEV_OS_WINDOWS || defined PNDEV_OS_ADONIS
    //------------------------------------------------------------------------
    #pragma pack(8)                                                           // set packing alignment 8 Byte
    //------------------------------------------------------------------------
    #elif defined PNDEV_OS_LINUX
    //------------------------------------------------------------------------
    #pragma pack(4)                                                           // set packing alignment 4 Byte
    //------------------------------------------------------------------------
    #else 
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    #endif
//	#pragma pack(show)

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif (defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX))
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_IOCTL_ARG
	{
		uPNDEV64_HANDLE		uDeviceSys;
		UINT32				lSizeInputBuf;
		UINT32				lSizeOutputBuf;
		uPNDEV64_PTR_VOID	uIn;
		uPNDEV64_PTR_VOID	uOut;
	}
	uPNDEV_IOCTL_ARG;

	typedef struct _uPNDEV_SCAN_DEV_ARG
	{
		const UINT32        lVendorId;
		const UINT32        lDeviceId;
		const UINT32		lSubsysVendorId;
		const UINT32        lSubsystemId;
		const UINT32		lPciClass;
		const UINT32		lPciClassMask;
		const ePNDEV_BOARD  eBoard;
	}
	uPNDEV_SCAN_DEV_ARG;

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

	//------------------------------------------------------------------------
	#pragma pack(pop)															// restore packing alignment
//	#pragma pack(show)

	//########################################################################
	//  Function prototypes
	//########################################################################

	//------------------------------------------------------------------------
	// os.c
	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)

	#elif defined (PNDEV_OS_ADONIS)
		int32_t	DriverEntry(		adn_drv_info_t*	uInfoInOut,
									int				lFlagIn);
		int32_t	fnEvtDriverUnload(	adn_drv_info_t*	uInfoIn);
		int32_t	fnEvtIoctl(			adn_drv_info_t*	uInfoIn,
									int				lCmdIn,
									void*			pArgIn);

	#elif defined (PNDEV_OS_LINUX)
    #if defined (PNDEV_DRIVER_SYS)
		int 	DriverEntry(		void);
		void 	fnEvtDriverUnload(	void);
		long 	fnEvtIoctl (		struct file *filp,
		                 	 	 	unsigned int cmd,
		                 	 	 	unsigned long arg);
    #endif //PNDEV_DRIVER_SYS
	#elif defined (PNDEV_OS_UNKNOWN)
	#endif

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ compiler: End of C linkage
	}
	#endif

#endif
