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
/*  F i l e               &F: os_clsFile.cpp                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code of class "clsFile" (OS specific)
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//  Variables
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

clsFile::clsFile(void)
{
	// preset
	memset(	this,
			0,
			sizeof(clsFile));
}
//************************************************************************
//  D e s c r i p t i o n :
//
//	destructor
//************************************************************************

clsFile::~clsFile(void)
{
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	open file for writing
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnOpenFileForWrite(const _TCHAR*	sNameFileIn,
									const BOOLEAN	bAppendIn,
									const UINT32	lDimErrorIn,
									HANDLE*			hHandleOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hTmp	= INVALID_HANDLE_VALUE;

	// preset OutputParameter
	*hHandleOut	= NULL;

	if	(_tcscmp(sNameFileIn, _TEXT("")) == 0)
		// empty FileName
	{
		// e.g. no DebugFile desired

		// success
		bResult = TRUE;
	}
	else
	{
		if	(bAppendIn)
			// appending data desired
		{
			hTmp = CreateFile(	sNameFileIn,						// Filename e.g. _TEXT("d:\\test.txt")
								GENERIC_WRITE ,						// needed for SetFilePointer()
								FILE_SHARE_WRITE | FILE_SHARE_READ,	// file can be shared
								NULL,								// handle cannot be inherited 
								OPEN_ALWAYS,						// open file if exist, otherwise create a new file
								FILE_ATTRIBUTE_NORMAL,				// not hidden, readonly,...
								NULL);								// no handle to a template file
		}
		else
		{
			hTmp = CreateFile(	sNameFileIn,						// Filename e.g. _TEXT("d:\\test.txt")
								FILE_WRITE_DATA,					// right to write data to the file
								FILE_SHARE_WRITE | FILE_SHARE_READ,	// file can be shared
								NULL,								// handle cannot be inherited 
								CREATE_ALWAYS,						// overwrite an existing file
								FILE_ATTRIBUTE_NORMAL,				// not hidden, readonly,...
								NULL);								// no handle to a template file
		}

		if	(hTmp == INVALID_HANDLE_VALUE)
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(bAppendIn)
				// appending data desired
			{
			UINT32	lResult		= 0;;
			UINT32	lDistance	= 0;

				// set FilePtr to end of file (e.g. Debug messages should be appended)
				lResult = SetFilePointer(	hTmp, 
											lDistance, 
											NULL, 
											FILE_END) ; 

				if	(lResult == INVALID_SET_FILE_POINTER)
					// error
				{ 
					// set ErrorString, append error code of GetLastError()
					clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
												_TEXT("(): "),
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;

					// return handle
					*hHandleOut = hTmp;
				}
			}
			else
			{
				// success
				bResult = TRUE;

				// return handle
				*hHandleOut = hTmp;
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnOpenFileForWrite(const _TCHAR*	sNameFileIn,
									const BOOLEAN	bAppendIn,
									const UINT32	lDimErrorIn,
									HANDLE*			hHandleOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult	= FALSE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnOpenFileForWrite(const _TCHAR*	sNameFileIn,
									const BOOLEAN	bAppendIn,
									const UINT32	lDimErrorIn,
									HANDLE*			hHandleOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hTmp	= NULL;

	// preset OutputParameter
	*hHandleOut	= NULL;

	if	(_tcscmp(sNameFileIn, _TEXT("")) == 0)
		// empty FileName
	{
		// e.g. no DebugFile desired

		// success
		bResult = TRUE;
	}
	else
	{
		if	(bAppendIn)
			// appending data desired
		{
			// open file for appending (write only)
			// file is created if it does not exist
			// write position is at end of file,
			hTmp = fopen(sNameFileIn, "a");
		}
		else
		{
			// open file for writing (write only)
			// file is created if it does not exist, otherwise truncatee to zero length
			// write position is at beginning of the file
			hTmp = fopen(sNameFileIn, "w");
		}

		if	(hTmp == NULL)
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResult = TRUE;

			// return handle
			*hHandleOut = hTmp;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	open file for reading
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsFile::fnOpenFileForRead(	const _TCHAR*	sNameFileIn,
									const UINT32	lDimErrorIn,
									HANDLE*			hHandleOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// preset OutputParameter
	*hHandleOut	= NULL;

	if	(_tcscmp(sNameFileIn, _TEXT("")) == 0)
		// empty FileName
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Empty file name!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	HANDLE	hTmp = INVALID_HANDLE_VALUE;
	_TCHAR	sPathTmp[PNDEV_SIZE_STRING_BUF] = {0};

		// for debug purpose
		GetCurrentDirectory(_countof(sPathTmp), sPathTmp);

		hTmp = CreateFile(	sNameFileIn,			// Filename e.g. _TEXT("d:\\test.txt")
							GENERIC_READ,			// right to read data from the file
							FILE_SHARE_READ,		// file can be shared
							NULL,					// handle cannot be inherited 
							OPEN_EXISTING,			// file must exist
							FILE_ATTRIBUTE_NORMAL,	// not hidden, readonly,...
							NULL);					// no handle to a template file

		if	(hTmp == INVALID_HANDLE_VALUE)
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);

			// append string
			_tcsncat_s(sErrorOut, lDimErrorIn, _TEXT("\n(CurDir="),		_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, sPathTmp,				_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, _TEXT(", FileName="),	_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, sNameFileIn,				_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, _TEXT(")"),				_TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;

			// return handle
			*hHandleOut = hTmp;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsFile::fnOpenFileForRead(	const _TCHAR*	sNameFileIn,
									const UINT32	lDimErrorIn,
									HANDLE*			hHandleOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult = FALSE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsFile::fnOpenFileForRead(	const _TCHAR*	sNameFileIn,
									const UINT32	lDimErrorIn,
									HANDLE*			hHandleOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// preset OutputParameter
	*hHandleOut	= NULL;

	if	(_tcscmp(sNameFileIn, _TEXT("")) == 0)
		// empty FileName
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Empty file name!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	FILE*	hTmp = NULL;
	_TCHAR	sPathTmp[PNDEV_SIZE_STRING_BUF] = {0};

		// for debug purpose
		getcwd(sPathTmp, sizeof(sPathTmp));

		// open file for read
		hTmp = fopen(sNameFileIn, "r");

		if	(hTmp == NULL)
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);

			// append string
			_tcsncat_s(sErrorOut, lDimErrorIn, _TEXT("\n(CurDir="),		_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, sPathTmp,				_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, _TEXT(", FileName="),	_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, sNameFileIn,				_TRUNCATE);
			_tcsncat_s(sErrorOut, lDimErrorIn, _TEXT(")"),				_TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;

			// return handle
			*hHandleOut = (HANDLE)hTmp;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	close file
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void clsFile::fnCloseFile(const HANDLE hHandleIn)
{
	// close File
	CloseHandle(hHandleIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void clsFile::fnCloseFile(const HANDLE hHandleIn)
{
	UNREFERENCED_PARAMETER(hHandleIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void clsFile::fnCloseFile(const HANDLE hHandleIn)
{
	fclose((FILE*)hHandleIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	get FileSize
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnGetFileSize(	const HANDLE	hHandleIn,
								const UINT32	lDimErrorIn,
								UINT32*			pSizeFileOut,
								_TCHAR*			sErrorOut)
{
BOOLEAN			bResult = FALSE;
LARGE_INTEGER	uSize;

	if	(!GetFileSizeEx(hHandleIn,
						&uSize))
		// error
	{
		// set ErrorString, append error code of GetLastError()
		clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;

		// return
		*pSizeFileOut = uSize.LowPart;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnGetFileSize(	const HANDLE	hHandleIn,
								const UINT32	lDimErrorIn,
								UINT32*			pSizeFileOut,
								_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnGetFileSize(	const HANDLE	hHandleIn,
								const UINT32	lDimErrorIn,
								UINT32*			pSizeFileOut,
								_TCHAR*			sErrorOut)
{
BOOLEAN	bResult 	= FALSE;
long	uFileSize	= 0;
long	uCurrentPos = 0;

	if	(hHandleIn != NULL)
		// valid handle
	{
		uCurrentPos = ftell((FILE*) hHandleIn);
	
		
		
		if	(LINUX_OK != fseek(	(FILE*) hHandleIn, 
								0L, 
								SEEK_END))
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): error at fseek to end of file"),
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			uFileSize = ftell((FILE*) hHandleIn);
			
			if	(LINUX_OK != fseek(	(FILE*) hHandleIn, 
									uCurrentPos, 
									SEEK_SET))
				// error
			{
				// set ErrorString, append error code of GetLastError()
				clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
											_TEXT("(): error at fseek to previous position of file"),
											lDimErrorIn,
											sErrorOut);
			}
			
			
			// success
			bResult = TRUE;
	
			// return
			*pSizeFileOut = (UINT32) uFileSize;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	read from file
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnReadFile(const HANDLE	hHandleIn,
							void*			pBufDestIn,
							const UINT32	lSizeIn,
							const UINT32	lDimErrorIn,
							_TCHAR*			sErrorOut)
{
BOOLEAN	bResult		= FALSE;
ULONG	lBytesRead	= 0;

	if	(!ReadFile(	hHandleIn,
					pBufDestIn,
					lSizeIn,
					&lBytesRead,
					NULL))
		// error
	{
		// set ErrorString, append error code of GetLastError()
		clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									lDimErrorIn,
									sErrorOut);
	}
	else if	(lBytesRead != lSizeIn)
			// error
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid number of bytes read!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnReadFile(const HANDLE	hHandleIn,
							void*			pBufDestIn,
							const UINT32	lSizeIn,
							const UINT32	lDimErrorIn,
							_TCHAR*			sErrorOut)
{
BOOLEAN	bResult = FALSE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnReadFile(const HANDLE	hHandleIn,
							void*			pBufDestIn,
							const UINT32	lSizeIn,
							const UINT32	lDimErrorIn,
							_TCHAR*			sErrorOut)
{
BOOLEAN	bResult		= FALSE;

	if	(hHandleIn != NULL)
		// valid handle
	{
		if	(lSizeIn != fread(	pBufDestIn,
								1, 
								lSizeIn, 
								(FILE*) hHandleIn))
				// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid number of bytes read!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	write to file
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnWriteFile(	const HANDLE	hHandleIn,
								const _TCHAR*	sDataIn,
								const UINT32	lDimErrorIn,
								_TCHAR*			sErrorOut)
{
BOOLEAN	bResult			= FALSE;
ULONG	lBytesWritten	= 0;
size_t	lLenData		= 0;

	if	(hHandleIn != NULL)
		// valid handle
	{
		// compute length of data
		lLenData = (sizeof(_TCHAR)* _tcslen(sDataIn));

		if	(!WriteFile(hHandleIn, 
						sDataIn,
						(DWORD) lLenData, 
						&lBytesWritten, 
						NULL))
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// write buffers to the file at once to ensure consistency

			if	(!FlushFileBuffers(hHandleIn))
				// error at writing buffers to the file
			{
				// set ErrorString, append error code of GetLastError()
				clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
											_TEXT("(): "),
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnWriteFile(	const HANDLE	hHandleIn,
								const _TCHAR*	sDataIn,
								const UINT32	lDimErrorIn,
								_TCHAR*			sErrorOut)
{
BOOLEAN	bResult = FALSE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnWriteFile(	const HANDLE	hHandleIn,
								const _TCHAR*	sDataIn,
								const UINT32	lDimErrorIn,
								_TCHAR*			sErrorOut)
{
BOOLEAN	bResult			= FALSE;
size_t	lLenData		= 0;

	if	(hHandleIn != NULL)
		// valid handle
	{
		// compute length of data
		lLenData = (sizeof(_TCHAR)* _tcslen(sDataIn));

		

		if	(lLenData != fwrite(	sDataIn, 
									1, 
									lLenData, 
									(FILE*) hHandleIn))
			// error
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid number of bytes written!"),
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	create file with a fixed size
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsFile::fnCreateFileFixedSize(	const _TCHAR*	sNameFileIn,
										const UINT32	lSizeBytesIn,
										const UINT32	lSizeFileInfoIn,
										const UINT32	lDimErrorIn,
										HANDLE*			hHandleOut,
										_TCHAR*			sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hTmp	= INVALID_HANDLE_VALUE;

	// preset OutputParameter
	*hHandleOut	= NULL;

	// create file
	hTmp = CreateFile(	sNameFileIn,						// Filename e.g. _TEXT("d:\\test.txt")
						GENERIC_WRITE ,						// needed for SetFilePointer()
						0,									// file cannot be shared
						NULL,								// handle cannot be inherited 
						CREATE_NEW,							// creates a new file, the function fails if the specified file already exists
						FILE_ATTRIBUTE_NORMAL,				// not hidden, readonly,...
						NULL);								// no handle to a template file

	if	(hTmp == INVALID_HANDLE_VALUE)
		// error
	{
		// set ErrorString, append error code of GetLastError()
		clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	UINT8*	pArrayFileInfo	= NULL;
	ULONG	lBytesWritten	= 0;

		pArrayFileInfo = new UINT8[lSizeFileInfoIn];

		// Note:
		//	- it does not cause an error if the file ptr is set to a position beyond the end of the file
		//	- the size of the file does not increase until the SetEndOfFile or WriteFile function is called
		//	- a write operation increases the size of the file to the file ptr position plus the size of the buffer written, leaving the intervening bytes uninitialized
		//	- experience:
		//		If bytes are written by WriteFile() at end of file then creating of a file takes a very very long time!
		//		It seems that files are filled with 0.
		//		This is independent if SetEndOfFile() is used or not used before writing bytes at end of file!
		//		-> Write FileInfo to start of file

		// Note:
		//	- file must be signed as empty: LenStruct=Version=0

		// preset memory with 0
		ZeroMemory(pArrayFileInfo, lSizeFileInfoIn);

		if	(!WriteFile(hTmp, 
						pArrayFileInfo,
						lSizeFileInfoIn, 
						&lBytesWritten, 
						NULL))
			// error at writing FileInfo
		{
			// set ErrorString, append error code of GetLastError()
			clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
		UINT32 lResult = 0;

			// free buffer
			delete[] pArrayFileInfo;
			pArrayFileInfo = NULL;

			// set FilePtr to desired size
			lResult = SetFilePointer(	hTmp, 
										lSizeBytesIn,
										NULL, 
										FILE_BEGIN) ; 

			if	(lResult == INVALID_SET_FILE_POINTER)
				// error
			{ 
				// set ErrorString, append error code of GetLastError()
				clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
											_TEXT("(): "),
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				if	(!SetEndOfFile(hTmp))
					// error at setting EOF
				{
					// set ErrorString, append error code of GetLastError()
					clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
												_TEXT("(): "),
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;

					// return handle
					*hHandleOut = hTmp;
				}
			}
		}
	}

	return(bResult);
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get FileVersion
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// MSDN: look for GetVers.exe

BOOLEAN clsFile::fnGetFileVersion(	_TCHAR*			sFileIn,
									UINT32*			pVersionMajorOut,
									UINT32*			pVersionMinorOut,
									UINT32*			pVersionBuildOut,
									UINT32*			pVersionPrivateOut,
									const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult		= FALSE;
UINT32	lInfoSize	= 0;
ULONG	lHandle		= 0;

	// preset OutputParameter
	*pVersionMajorOut	= 0;
	*pVersionMinorOut	= 0;
	*pVersionBuildOut	= 0;
	*pVersionPrivateOut	= 0;

	// get the VersionInfo size of the desired file
	lInfoSize = GetFileVersionInfoSize(	sFileIn,
										&lHandle);

	if	(lInfoSize == 0)
		// error
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT("Error at getting FileVersion. Probably file "),
									sFileIn,
									_TEXT(" was not found!"),
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	BYTE* pBuffer = new BYTE[lInfoSize];

		if	(!pBuffer)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at allocating buffer!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(GetFileVersionInfo(	sFileIn, 
										0, 
										lInfoSize, 
										pBuffer) == 0)
				// error at getting VersionInfo of the desired file
			{
				// set ErrorString, append error code of GetLastError()
				clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
											_TEXT("(): "),
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
			VS_FIXEDFILEINFO*	uInfo;
			UINT				nInfoSize;

				if	(VerQueryValue(	pBuffer, 
									TEXT("\\"),
									(void**) &uInfo,
									&nInfoSize) == 0)
					// error at getting specified VersionInfo of the desired file
					// ('\' specifies the root block)
				{
					// set ErrorString, append error code of GetLastError()
					clsDataSet::fnAddLastError(	_TEXT(__FUNCTION__),
												_TEXT("(): "),
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					if	(nInfoSize == 0)
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): No VersionInfo available!"),
													NULL,
													lDimErrorIn,
													sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;

						// return
						*pVersionMajorOut	= HIWORD(uInfo->dwFileVersionMS);
						*pVersionMinorOut	= LOWORD(uInfo->dwFileVersionMS);
						*pVersionBuildOut	= HIWORD(uInfo->dwFileVersionLS);
						*pVersionPrivateOut	= LOWORD(uInfo->dwFileVersionLS);
					}
				}
			}
		}

		// free buffer
		delete[] pBuffer;
		pBuffer = NULL;
	}

	return(bResult);
}
#endif
