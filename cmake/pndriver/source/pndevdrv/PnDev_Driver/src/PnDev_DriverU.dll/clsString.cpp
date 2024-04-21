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
/*  F i l e               &F: clsString.cpp                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code of class "clsString" of DLL
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//	Variables
//########################################################################
//------------------------------------------------------------------------

//########################################################################
//	PUBLIC
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	constructor
//************************************************************************

clsString::clsString(void)
{
	// preset
	memset(	this,
			0,
			sizeof(clsString));
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	destructor
//************************************************************************

clsString::~clsString(void)
{
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	build string
//************************************************************************

size_t clsString::fnBuildString(const _TCHAR*	sSrc1In,
								const _TCHAR*	sSrc2In,
								const _TCHAR*	sSrc3In,
								const UINT32	lDimResultIn,
								_TCHAR*			sResultOut)
{
size_t lResult = 0;

	// preset OutputParameter
	_tcscpy_s(	sResultOut,
				lDimResultIn,
				_TEXT(""));

	// append strings to OutputString
	if (sSrc1In != NULL)	{_tcsncat_s(sResultOut, lDimResultIn, sSrc1In, _TRUNCATE);	}
	if (sSrc2In != NULL)	{_tcsncat_s(sResultOut, lDimResultIn, sSrc2In, _TRUNCATE);	}
	if (sSrc3In != NULL)	{_tcsncat_s(sResultOut, lDimResultIn, sSrc3In, _TRUNCATE);	}

	// return
	lResult = _tcslen(sResultOut);

	return(lResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	search SubString at String
//************************************************************************

BOOLEAN clsString::fnSearchSubString(	const _TCHAR*	sSubIn,
										const _TCHAR*	sBigIn,
										const BOOLEAN	bCaseInsensitiveIn)
{
BOOLEAN			bResult		= FALSE;
const _TCHAR*	sFirstOccur	= NULL;

	if	(!bCaseInsensitiveIn)
		// not case insensitive
	{
		// try to find the SubString at String
		sFirstOccur = _tcsstr(	sBigIn,
								sSubIn);
	}
	else
	{
	_TCHAR	sSubTmp[PNDEV_SIZE_STRING_BUF]	= {0};
	_TCHAR	sBigTmp[PNDEV_SIZE_STRING_BUF]	= {0};

		// copy strings
		_tcscpy_s (sSubTmp, _countof(sSubTmp), sSubIn);
		_tcscpy_s (sBigTmp, _countof(sBigTmp), sBigIn);

		// convert strings to LowerCase to make them case insensitive
		_tcslwr_s(sSubTmp, _countof(sSubTmp));
		_tcslwr_s(sBigTmp, _countof(sBigTmp));

		// try to find the SubString at String
		sFirstOccur = _tcsstr(	sBigTmp,
								sSubTmp);
	}

	if	(sFirstOccur != NULL)
		// SubString occurred at String
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	search SubString at start of String
//************************************************************************

BOOLEAN clsString::fnSearchSubStringAtStart(const _TCHAR*	sSubIn,
											const _TCHAR*	sBigIn)
{
BOOLEAN			bResult		= FALSE;
const _TCHAR*	sFirstOccur	= NULL;
UINT32			lPosition	= 0;

	// try to find the SubString at String
	sFirstOccur = _tcsstr(	sBigIn,
							sSubIn);

	if	(sFirstOccur != NULL)
		// SubString occurred at String
	{
		// get position of SubString (subtraction of 2 ptr: sFirstOccur always >= sBigIn)
		lPosition = (UINT32) (sFirstOccur - sBigIn + (UINT32) 1);

		if	(lPosition == 1)
			// SubString occurred at first position of String
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//########################################################################
//	PRIVATE
//########################################################################
