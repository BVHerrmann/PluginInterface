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
/*  F i l e               &F: os_Env.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for adaption to used environment (OS, compiler, C/C++ library)
//
//****************************************************************************/

// Note if using VisualStudio:
//	A "C" module doesn't support precompiled header
//	-> turn it off at ProjectSettings >> C/C++ >> Precompiled Headers of  t h i s  file

//------------------------------------------------------------------------
//------------------------------------------------------------------------
#include "precomp.h"
#include "PnDev_Driver_Inc.h"												// all IncludeFiles of PnDev_Driver
//------------------------------------------------------------------------


#ifdef PNDEV_TRACE															// required for Driver ToolChain
	#include "os_Env.tmh"
#endif


//************************************************************************
//  D e s c r i p t i o n :
//
//	set ThreadPrio
//************************************************************************

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//	get ThreadPrio
//************************************************************************

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//	get current ThreadHandle
//************************************************************************

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//	create mutex
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN	fnCreateMutex(	const UINT32	lDimErrorIn,
						void*			pMutexOut,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN				bResult = FALSE;
	// linux kernel mutexes are initialized without mutex attributes


	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	// linux kernel mutexes are initialized without mutex attributes
	// PriorityInheritance is implemented in all in-kernel semaphores in real time patched linux (CONFIG_PREEMPT_RT)
	mutex_init((struct mutex*) pMutexOut);
	// mutex_init has no return value
	bResult = TRUE;
	return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//	delete mutex
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN	fnDeleteMutex(	void*			pMutexIn,
						const UINT32	lDimErrorIn,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	// destroy linux kernel mutex
	mutex_destroy((struct mutex*) pMutexIn);
	// mutex_destroy has no return value
	bResult = TRUE;

	return(bResult);
}

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//	set ThreadPar
//************************************************************************

//------------------------------------------------------------------------

//########################################################################
//	INTERNAL
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert a string to LowerCase
//************************************************************************

//------------------------------------------------------------------------
#if defined (TOLOWER)
//------------------------------------------------------------------------
errno_t _tcslwr_s(	PNDEV_CHAR*	sIn,
					size_t		lCountBufIn)
{
errno_t	lResult	= -1;
size_t	lNoChar	= 0;

	// get real number of characters
	lNoChar = _tcslen(sIn);

	if	(lNoChar < lCountBufIn)
		// valid number of real characters (terminal NULL must be regarded)
	{
	UINT32 i = 0;

		// success
		lResult = 0;

		for	(i = 0; i < lNoChar; i++)
			// all characters
		{
			// convert to LowerCase
			sIn[i] = tolower(sIn[i]);
		}
	}

	return(lResult);
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert a string to UpperCase
//************************************************************************

//------------------------------------------------------------------------
#if defined (TOUPPER)
//------------------------------------------------------------------------
errno_t _tcsupr_s(	PNDEV_CHAR*	sIn,
					size_t		lCountBufIn)
{
errno_t	lResult	= -1;
size_t	lNoChar	= 0;

	// get real number of characters
	lNoChar = _tcslen(sIn);

	if	(lNoChar < lCountBufIn)
		// valid number of real characters (terminal NULL must be regarded)
	{
	UINT32 i = 0;

		// success
		lResult = 0;

		for	(i = 0; i < lNoChar; i++)
			// all characters
		{
			// convert to UpperCase
			sIn[i] = toupper(sIn[i]);
		}
	}

	return(lResult);
}
#endif


//************************************************************************
//  D e s c r i p t i o n :
//
//	convert _TCHAR to ASCII string
//************************************************************************

void fnConvertStringTcharToChar(const PNDEV_CHAR*	sIn,
								char*				sResultOut)
{
size_t i = 0;

	// copy _TCHAR to char inclusive terminal NULL
	for (i = 0; i <= _tcslen(sIn); i++)
	{
		sResultOut[i] = (char) sIn[i];
	}
}
