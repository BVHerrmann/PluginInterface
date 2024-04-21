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
/*  F i l e               &F: String.cpp                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for string functions of SYS
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "String.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBuildString)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	build string
//************************************************************************

UINT32 fnBuildString(	const _TCHAR*	sSrc1In,
						const _TCHAR*	sSrc2In,
						const _TCHAR*	sSrc3In,
						const UINT32	lDimResultIn,
						_TCHAR*			sResultOut)
{
UINT32 lResult = 0;

	// preset OutputString
	_RtlStringCchCopy(	sResultOut,
						lDimResultIn,
						_TEXT(""));

	// append strings to OutputString
	if (sSrc1In != NULL)	{_RtlStringCchCatN(	sResultOut, lDimResultIn, sSrc1In, (lDimResultIn - _tcslen(sResultOut) - 1));}
	if (sSrc2In != NULL)	{_RtlStringCchCatN(	sResultOut, lDimResultIn, sSrc2In, (lDimResultIn - _tcslen(sResultOut) - 1));}
	if (sSrc3In != NULL)	{_RtlStringCchCatN(	sResultOut, lDimResultIn, sSrc3In, (lDimResultIn - _tcslen(sResultOut) - 1));}

/*
	_RtlStringCchCopy(	sResultOut, lDimResultIn, _TEXT(""));
	_RtlStringCchCatN(	sResultOut, lDimResultIn, _TEXT("0123456789012345678901234567890123456789aaaaaa50aa"), (lDimResultIn - _tcslen(sResultOut) - 1));
	_RtlStringCchCatN(	sResultOut, lDimResultIn, _TEXT("0123456789012345678901234567890123456789aaaaa100aa"), (lDimResultIn - _tcslen(sResultOut) - 1));
	_RtlStringCchCatN(	sResultOut, lDimResultIn, _TEXT("0123456789012345678901234567890123456789aaaaa150aa"), (lDimResultIn - _tcslen(sResultOut) - 1));
	_RtlStringCchCatN(	sResultOut, lDimResultIn, _TEXT("0123456789012345678901234567890123456789aaaaa200aa"), (lDimResultIn - _tcslen(sResultOut) - 1));
	_RtlStringCchCatN(	sResultOut, lDimResultIn, _TEXT("0123456789012345678901234567890123456789aaaaa250aa"), (lDimResultIn - _tcslen(sResultOut) - 1));
	_RtlStringCchCatN(	sResultOut, lDimResultIn, _TEXT("0123456789012345678901234567890123456789aaaaa300aa"), (lDimResultIn - _tcslen(sResultOut) - 1));
*/

	// return
	lResult = _tcslen(sResultOut);

	return(lResult);
}
