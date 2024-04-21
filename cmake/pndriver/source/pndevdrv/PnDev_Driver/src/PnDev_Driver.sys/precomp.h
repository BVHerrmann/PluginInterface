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
/*  F i l e               &F: precomp.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Precompiled header file
//
//****************************************************************************/

#ifndef __precomp_h__
#define __precomp_h__

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	#ifndef UNICODE
		#define	UNICODE
		#define	_UNICODE
	#endif

	#define PFD_MEM_ATTR	__in __drv_aliasesMem								// because of warning of PREfast for drivers (PFD)
																				// (indicates that the called function saves the value of the parameter and frees it later)
	#define WIN9X_COMPAT_SPINLOCK

	#include <ntddk.h>

	#pragma warning(disable:4201)												// nameless struct/union warning
		#include <stdarg.h>
		#include <wdf.h>
	#pragma warning(default:4201)

	#include <initguid.h>														// required for GUID definitions
	#include <wdmguid.h>														// required for WMILIB_CONTEXT
	#include <tchar.h>															// _TCHAR, _tcslen, ...
	#include <Ntstrsafe.h>														// safe CRT functions

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	#define PFD_MEM_ATTR

	#include <Adonis_Inc.h>
	#include <acpi_drv.h>
	#include <pci.h>
	#include <mman.h>
	#include <memory_management.h>
	#include <time.h>

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	#define PFD_MEM_ATTR														// required for Basic.h

	#if defined PNDEV_DRIVER_SYS
		#include <linux/module.h>
		#include <linux/init.h>
		#include <linux/kernel.h>
		#include <linux/vmalloc.h>
		#include <linux/cdev.h>
		#include <linux/device.h>
		#include <linux/fs.h>
		#include <linux/slab.h>
		#include <linux/pci.h>
		#include <linux/msi.h>
		#include <linux/io.h>
		#include <linux/mutex.h>
		#include <linux/ioctl.h>
		#include <linux/uaccess.h>
		#include <linux/delay.h>        
		#include <linux/time.h>                 /* for time in systhread */
		#include <linux/timer.h>                /* Timer for systhread */
		#include <linux/interrupt.h>            /* pci interrupts */ 
		#include <linux/eventfd.h>				/* eventfd  */
		#include <linux/sched.h>				/* current->pid for sending events with eventfd in kernel */
		#include <linux/pid.h>					/* pid for sending events with eventfd in kernel */
		#include <linux/fdtable.h>				/* for sending events with eventfd in kernel */
		#include <linux/rcupdate.h>				/* locking when looking up user eventfd */
		#include <linux/compat.h>				/* 32-Bit compatibility */
		#include <linux/spinlock.h>             /* for using spinlocks */
		#include <linux/irq.h>					/* for using interrupts */
	#endif

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

#endif
