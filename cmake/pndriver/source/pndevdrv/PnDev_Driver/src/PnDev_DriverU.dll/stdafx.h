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
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: stdafx.h                                  :F&  */
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

// stdafx.h:
//	include file for standard system include files
//	(or project specific include files that are used frequently, but are changed infrequently)

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	#pragma once																// specifies that this file will be included (opened) only once by the compiler when compiling a source code file

	#ifndef UNICODE
		#define	UNICODE
		#define	_UNICODE
	#endif

	#define WIN32_LEAN_AND_MEAN													// exclude rarely-used stuff from Windows headers

	#include "targetver.h"
	#include <windows.h>
	#include <stdlib.h>															// _lto()
	#include <stdio.h>															// wprintf()
	#include <tchar.h>															// _TCHAR, _tcslen, ...
	#include <setupapi.h>														// all functions SetupXX()
	#include <yvals.h>															// __FUNCTION__, __FUNCTIONW__, __STR2WSTR
	#include <regstr.h>															// REGSTR_KEY_PCIENUM
	#include <BaseTsd.h>														// MAXUINT32
//	#include <ShlObj.h>															// IShellLink, IPersistFile

	#pragma warning(disable: 4201)												// FILE_DEVICE_UNKNOWN,..., warning C4201: nonstandard extension used : nameless struct/union
		#include <winioctl.h>
	#pragma warning(default: 4201)												// set default

	#include <initguid.h>														// required for GUID definitions (GUID must be initialized, otherwise LNK2001 error)

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	#include <Adonis_Inc.h>

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	#include <stdint.h>
	#include <stddef.h>
	#include <fcntl.h>			//open() flags
	#include <unistd.h>			//close() fkt
	#include <pthread.h>		//POSIX mutex
	#include <sys/ioctl.h>		//ioclt fkt
	#include <sys/eventfd.h>	//kernel-user events
	#include <sys/ipc.h>		//System V message queue
	#include <sys/msg.h>		//System V message queue
	#include <sys/wait.h>		//System V message queue
	#include <sys/errno.h>		//System V message queue
	#include <sys/mman.h>		//mmap and munmap
	#include <stdio.h>			//file operations
	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif
