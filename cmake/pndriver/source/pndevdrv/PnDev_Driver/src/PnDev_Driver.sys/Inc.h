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
/*  F i l e               &F: Inc.h                                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common include file of SYS
//
//****************************************************************************/

#ifndef __Inc_h__
#define __Inc_h__

	#include <PnDev_Driver_Inc.h>
	//------------------------------------------------------------------------
	#include "Basic.h"															// must be before os.h / os_SysDll.h
	#include "os.h"
	#include "os_SysDll.h"
	#include "SysDll.h"															// compiler switches -> must be before Asic_xx.h/Board_xx.h    

    #ifdef PNDEV_SUPPORT_OTHERS
    #define PNDEV_CFG_USE_FLASH
    #endif

    #ifdef PNDEV_CFG_USE_FLASH
    #include "Flash.h"															// must be before Private.h
    #include "Flash_SPI.h"														// must be before Private.h		
    #endif

    #ifdef PNDEV_SUPPORT_OTHERS
    #include "eddi_irte.h"    
	#include "Asic_Irte.h"														// must be before Private.h
	#include "Asic_Pnip.h"														// must be before Private.h
	#include "Board_Irte.h"														// must be before Private.h
	#include "Board_IrtePnip.h"													// must be before Private.h
	#include "Board_Pnip.h"														// must be before Private.h	
	#include "Board_Micrel.h"													// must be before Private.h
	#include "Board_AM5728.h"													// must be before Private.h    
    #endif    

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------                                       
	                  
    #include "ErtecReg.h"                                                       // must be before Online.h     
    #include "netProfiKernelStructs.h"

	#endif

	#include "Private.h"

    #include "Board_Intel.h"													// must be before Private.h    
    
    #ifdef PNDEV_SUPPORT_I210
    #include "Board_I210.h"													    // must be before Private.h
    #endif    
    #ifdef PNDEV_SUPPORT_I210SFP
    #include "Board_I210SFP.h"													// must be before Private.h
    #endif
    #ifdef PNDEV_SUPPORT_I82574
    #include "Board_I82574.h"													// must be before Private.h
    #endif
    #ifdef PNDEV_SUPPORT_IX1000
    #include "Board_IX1000.h"													// must be before Private.h
    #endif

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------                                       
    
	#include "netProfiKernel.h" 
	#include "trace.h"     

	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------
	#undef PNDEV_TRACE

	#endif

#endif
