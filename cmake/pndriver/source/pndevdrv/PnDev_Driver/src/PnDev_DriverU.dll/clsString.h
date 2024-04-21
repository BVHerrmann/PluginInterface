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
/*  F i l e               &F: clsString.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations of class "clsString" of DLL
//
//****************************************************************************/

#ifndef __clsString_h__
#define __clsString_h__

	//########################################################################
	//  Defines
	//########################################################################
	//------------------------------------------------------------------------

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

	//########################################################################
	//  Class
	//########################################################################
	
	class clsString
	{
	public:

	//========================================================================
	//  PUBLIC-attributes
	//========================================================================

	//========================================================================
	//  PUBLIC-inline-methods
	//========================================================================

	//========================================================================
	//  PUBLIC-methods
	//========================================================================

	clsString(	void);
	~clsString(	void);

	//------------------------------------------------------------------------
	static size_t	fnBuildString(				const _TCHAR*	sSrc1In,
												const _TCHAR*	sSrc2In,
												const _TCHAR*	sSrc3In,
												const UINT32	lDimResultIn,
												_TCHAR*			sResultOut);
	static BOOLEAN	fnSearchSubString(			const _TCHAR*	sSubIn,
												const _TCHAR*	sBigIn,
												const BOOLEAN	bCaseInsensitiveIn);
	static BOOLEAN	fnSearchSubStringAtStart(	const _TCHAR*	sSubIn,
												const _TCHAR*	sBigIn);

	private:

	//========================================================================
	//  PRIVATE-attributes
	//========================================================================

	//========================================================================
	//  PRIVATE-inline-methods
	//========================================================================

	//========================================================================
	//  PRIVATE-methods
	//========================================================================
	};
	
#endif
