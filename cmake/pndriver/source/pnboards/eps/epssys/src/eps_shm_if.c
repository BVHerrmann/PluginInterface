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
/*  F i l e               &F: eps_shm_if.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Shared Memory Driver Interface                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20026
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>    /* Types / Prototypes / Fucns    */
#include <eps_trc.h>    /* Tracing                       */
#include <eps_shm_if.h> /* PN-Driver Interface     */

//----------------------------------------------------------------------------------------------------

/* Global definition */
static EPS_SHM_IF_TYPE g_EpsShmIf;
static EPS_SHM_IF_PTR_TYPE g_pEpsShmIf = LSA_NULL;

//----------------------------------------------------------------------------------------------------

/**
 * Registers the EPS Shared Memory driver implementation to the EPS.
 * 
 * @param [in] pShmIf     - pointer to the structure that contains the function pointers of the implementation
 * @return LSA_VOID
 */
LSA_VOID eps_shm_if_register(EPS_SHM_IF_CONST_PTR_TYPE pShmIf)
{
	EPS_ASSERT(pShmIf != LSA_NULL);

	EPS_ASSERT(pShmIf->close != LSA_NULL);
	EPS_ASSERT(pShmIf->open != LSA_NULL);
	EPS_ASSERT(pShmIf->uninstall != LSA_NULL);

	g_pEpsShmIf = &g_EpsShmIf;  /* Init Global Instance     */
	*g_pEpsShmIf = *pShmIf;     /* Install Pn Device Driver */
}

//----------------------------------------------------------------------------------------------------
LSA_UINT16 eps_shm_update_device_list(LSA_VOID)
{
	LSA_UINT16 retVal;

	EPS_ASSERT(g_pEpsShmIf != LSA_NULL);  /* No Interface installed */
	EPS_ASSERT(g_pEpsShmIf->update_device_list != LSA_NULL);
	
	retVal = g_pEpsShmIf->update_device_list();

	return retVal;
}

//----------------------------------------------------------------------------------------------------
/**
 * Opens the EPS SHM instance.
 * 
 * This instance is used for the LD Lower interface: 
 *    HIF LD Lower (if LD + HD firmware - Basic, Advanced, Light (between EPS and APP))
 *    HIF HD Lower (if HD firmware - Basic)
 *    PNTRC Lower  (Light, Advanced, Basic)
 *    
 * This function calls the open function registred at eps_shm_if_register.
 * The EPS SHM implemetation will install a EPS HIF Driver that provides memory and 
 * signaling mechanism for the HD / LD Lower interface. 
 * 
 * @param [in] hSys           - ???
 * @param [in] pOption        - HD Number + Watchdogs settings
 * @param [out] ppShmInstOut  - handle to the SHM instance
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_shm_if_open (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppShmInstOut)
{
	LSA_UINT16 retVal;

	EPS_ASSERT(ppShmInstOut != LSA_NULL);

	EPS_ASSERT(g_pEpsShmIf != LSA_NULL);        /* No Interface installed           */
	EPS_ASSERT(g_pEpsShmIf->open != LSA_NULL); /* Is a Pn Device Driver installed? */

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_shm_if_open()");
	retVal = g_pEpsShmIf->open(hSys, pOption, ppShmInstOut);

	EPS_ASSERT(*ppShmInstOut != LSA_NULL);

    g_pEpsData->bEpsShmIfInitialized = LSA_TRUE;

    return retVal;
}

/**
 * Closes the EPS SHM instance by calling the close function of the EPS SHM implementation.
 * 
 * @param [in] pShmInstIn  - handle to instance
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_shm_if_close (EPS_SHM_HW_CONST_PTR_TYPE pShmInstIn)
{
	LSA_UINT16 retVal;

	EPS_ASSERT(pShmInstIn != LSA_NULL);
	retVal = pShmInstIn->Close(pShmInstIn);
	EPS_ASSERT(retVal == EPS_SHM_RET_OK);

	return retVal;
}

/**
 * Calls the uninstall function of the EPS SHM implementation. Deletes the pointer to the SHM implementation.
 * 
 * @return LSA_VOID
 */
LSA_VOID eps_shm_if_undo_init()
{
	EPS_ASSERT(g_pEpsShmIf != LSA_NULL);        /* No Interface installed           */
	EPS_ASSERT(g_pEpsShmIf->uninstall != LSA_NULL); /* Is a Pn Device Driver installed? */

	g_pEpsShmIf->uninstall();

	g_pEpsShmIf = LSA_NULL;

    g_pEpsData->bEpsShmIfInitialized = LSA_FALSE;
}

//----------------------------------------------------------------------------------------------------
