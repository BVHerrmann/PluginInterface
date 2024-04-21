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
//	Common include file of DLL
//
//****************************************************************************/

#ifndef __Inc_h__
#define __Inc_h__

	#define PFD_MEM_ATTR														// required for Basic.h
	
	#include "PnDev_Driver_Inc.h"
	//------------------------------------------------------------------------
	#include "../PnDev_Driver.sys/Basic.h"										// must be before os_SysDll.h
	#include "../PnDev_Driver.sys/os_SysDll.h"									// GUID_PNDEV_IF,...
	#include "../PnDev_Driver.sys/SysDll.h"										// compiler switches -> must be before Asic_xx.h/Board_xx.h
	#include "../PnDev_Driver.sys/Asic_Pnip.h"									// defines of Asics with PNIP
	#include "../PnDev_Driver.sys/Asic_Irte.h"									// defines of Asics with IRTE
	#include "../PnDev_Driver.sys/Board_Intel.h"								// defines of Asics with Intel
    #include "../PnDev_Driver.sys/Board_I210.h"								    // defines of I210
    #include "../PnDev_Driver.sys/Board_I210SFP.h"								// defines of I210SFP
    #include "../PnDev_Driver.sys/Board_I82574.h"								// defines of I82574
    #include "../PnDev_Driver.sys/Board_IX1000.h"								// defines of IX1000
	#include "../PnDev_Driver.sys/Board_Micrel.h"								// defines of Asics with Micrel
	#include "../PnDev_Driver.sys/Board_AM5728.h"								// defines of Asics with AM5728
	//------------------------------------------------------------------------
	#include "../../common/PnDev_Util.h"
	//------------------------------------------------------------------------
	#ifdef __cplusplus															// if C++ compiler
		#include "clsDataSet.h"
		#include "clsFile.h"
		#include "clsString.h"
		#include "clsDmm.h"
		#include "clsUsedDevice.h"
		#include "clsUsedTimer.h"
#endif

#endif
