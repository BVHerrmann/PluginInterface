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
/*  C o m p o n e n t     &C: pcIOX (PNIO Controler, Device and more)   :C&  */
/*                                                                           */
/*  F i l e               &F: eps_plf_windows.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Platform Adaption                                                    */
/*                                                                           */
/*****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include "psi_inc.h"    /*6*/
#include <eps_plf.h>           /* EPS PLF prototypes                       */

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_PLF_REGISTER_SHM_IF( LSA_VOID* pShmIf )
{
	// note: not used on pcIOX NIF
	LSA_UNUSED_ARG(pShmIf);
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_PLF_ENABLE_INTERRUPTS(LSA_VOID)
{
	// note: on windows level no enable IR is possible
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_PLF_DISABLE_INTERRUPTS(LSA_VOID)
{
	// note: on windows level no disable IR is possible
}
/*----------------------------------------------------------------------------*/

long EPS_PLF_EXCHANGE_LONG( long volatile *pAddr, long lVal)
{
	return( InterlockedExchange( pAddr, lVal) );
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
