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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg_pof.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI POF HW system adaption                             */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID     32004
#define LTRC_ACT_MODUL_ID 32004

#include "eps_sys.h"

#if (( PSI_CFG_USE_POF == 1 ) && (PSI_CFG_USE_HD_COMP == 1))
/*----------------------------------------------------------------------------*/

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifndef PSI_POF_FO_LED
LSA_VOID PSI_POF_FO_LED(
	LSA_UINT16        const PortID,
	LSA_UINT8         const ON,
	PSI_SYS_HANDLE    hSysDev )
{
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	LSA_UNUSED_ARG( PortID );  
	LSA_UNUSED_ARG( ON );

    #if ( PSI_DEBUG == 0)
    // surppress compiler warnings
    LSA_UNUSED_ARG(pSys);
    #endif

}
#else
# error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#endif /* PSI_CFG_USE_POF && PSI_CFG_USE_HD_COMP  */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
