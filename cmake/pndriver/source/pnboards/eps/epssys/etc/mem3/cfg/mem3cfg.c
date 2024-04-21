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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: mem3cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System adaption of MEM3                                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20042
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include <stddef.h> 		// OBSD_ITGR -- shifted from pcpnio_sys.h, otherwise collision with OpenBSD types
#include "mem3if.h"        	// mem3 headerfiles
#include "eps_mem.h"      	// cp_mem_lock(), cp_mem_unlock()
#include "eps_sys.h"

/*----------------------------------------------------------------------------*/
/*  System adaption for MEM3                                                  */
/*----------------------------------------------------------------------------*/

LSA_VOID mem3_assert_fatal( const LSA_CHAR * file, LSA_INT line, const LSA_CHAR * func )
{
    EPS_FATAL_WITH_REASON( EPS_EXIT_CODE_EPS_FATAL, EPS_EXIT_CODE_EPS_FATAL, 0, 0, 0, (LSA_CHAR *)file, line, func, "MEM3 assert" );
}

/*----------------------------------------------------------------------------*/
LSA_VOID * mem3_memset( LSA_VOID * pMem, LSA_INT Value, LSA_UINT Length )
{ 
	MEM3_ASSERT( pMem != 0 );
	eps_memset( pMem, Value, Length );
	return pMem;
}

/*----------------------------------------------------------------------------*/
LSA_VOID * mem3_memcpy( LSA_VOID * pDst, LSA_VOID * pSrc, LSA_UINT Length )
{
	MEM3_ASSERT( pDst != 0 );
	MEM3_ASSERT( pSrc != 0 );
	eps_memcpy( pDst, pSrc, Length );
	return pDst;
	//lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API.
}

/*----------------------------------------------------------------------------*/
LSA_VOID MEM3_ENTER( LSA_VOID )
{
	eps_mem_lock();
}

/*----------------------------------------------------------------------------*/
LSA_VOID MEM3_EXIT( LSA_VOID )
{
	eps_mem_unlock();
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
