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
/*  F i l e               &F: clsFile.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations of class "clsFile"
//
//****************************************************************************/

#ifndef __clsFile_h__
#define __clsFile_h__

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
	
	class clsFile
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

	clsFile(	void);
	~clsFile(	void);

	//------------------------------------------------------------------------
	static BOOLEAN	fnOpenFileForWrite(		const _TCHAR*	sNameFileIn,
											const BOOLEAN	bAppendIn,
											const UINT32	lDimErrorIn,
											HANDLE*			hHandleOut,
											_TCHAR*			sErrorOut);
	static BOOLEAN	fnOpenFileForRead(		const _TCHAR*	sNameFileIn,
											const UINT32	lDimErrorIn,
											HANDLE*			hHandleOut,
											_TCHAR*			sErrorOut);
	static void		fnCloseFile(			const HANDLE	hHandleIn);
	static BOOLEAN	fnGetFileSize(			const HANDLE	hHandleIn,
											const UINT32	lDimErrorIn,
											UINT32*			pSizeFileOut,
											_TCHAR*			sErrorOut);
	static BOOLEAN	fnReadFile(				const HANDLE	hHandleIn,
											void*			pBufDestIn,
											const UINT32	lSizeIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut);
	static BOOLEAN	fnWriteFile(			const HANDLE	hHandleIn,
											const _TCHAR*	sDataIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut);
	static BOOLEAN	fnCreateFileFixedSize(	const _TCHAR*	sNameFileIn,
											const UINT32	lSizeBytesIn,
											const UINT32	lSizeFileInfoIn,
											const UINT32	lDimErrorIn,
											HANDLE*			hHandleOut,
											_TCHAR*			sErrorOut);
	static BOOLEAN	fnGetFileVersion(		_TCHAR*			sFileIn,
											UINT32*			pVersionMajorOut,
											UINT32*			pVersionMinorOut,
											UINT32*			pVersionBuildOut,
											UINT32*			pVersionPrivateOut,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut);

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
