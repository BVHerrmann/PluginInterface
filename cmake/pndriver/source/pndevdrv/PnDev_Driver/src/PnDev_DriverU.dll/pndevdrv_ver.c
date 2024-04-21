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
/*  F i l e               &F: pndevdrv_ver.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Version of PnDev_Driver
//
//****************************************************************************/

// Note:
//	A "C" module doesn't support precompiled header
//	-> turn it off at ProjectSettings >> C/C++ >> Precompiled Headers of  t h i s  file

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//************************************************************************
//  D e s c r i p t i o n :
//
//  get version
//************************************************************************

PNDEV_DRIVER_API LSA_UINT16 fnPnDev_GetVersion(LSA_UINT16 length, LSA_VERSION_TYPE* ptr_version)
{
LSA_UINT32 i;
LSA_UINT8  *pDest;
LSA_UINT8  *pSrc;

	LSA_VERSION_TYPE v = {PNDEVDRV_LSA_COMPONENT_ID, PNDEVDRV_LSA_PREFIX,
	                      PNDEVDRV_KIND,             PNDEVDRV_VERSION,
	                      PNDEVDRV_DISTRIBUTION,     PNDEVDRV_FIX,
	                      PNDEVDRV_HOTFIX,           PNDEVDRV_PROJECT_NUMBER,
	                      PNDEVDRV_INCREMENT,        PNDEVDRV_INTEGRATION_COUNTER,
	                      PNDEVDRV_GEN_COUNTER };

	if (length > sizeof(v))
	{
		length = sizeof(v);
	}
		
	i     = length;
	pDest = (LSA_UINT8*)ptr_version;
	pSrc  = (LSA_UINT8*)&v;
	
	while (i--) *pDest++ = *pSrc++;

	return(LSA_UINT16)(sizeof(v) - length);
}
