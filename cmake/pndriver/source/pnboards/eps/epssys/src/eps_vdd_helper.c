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
/*  F i l e               &F: eps_vdd_helper.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS VDD Implementation - module for helper functions                     */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  20072
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include <eps_sys.h>           /* Types / Prototypes / Fucns			   */
#include <eps_trc.h>           /* Tracing                                  */
    
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
//lint -save -e157 -e114 -e18
//#include <tchar.h>
#include <windows.h>
//lint -restore
#endif
#include <precomp.h>
#include "vdd_Env.h"
#include "vddU.h"
#include <eps_vdd_helper.h>


/**
* Callbackfunction for event handling of VDD
*
* @param [in] hDeviceIn         Handle of Device
* @param [in] eEventClassIn     Enume with different DeviceClasses
* @param [in] sEventIn          string with cause of event
* @param [in] pCbfParUserIn     pointer to user defined parameter
* @return
*/
void __stdcall eps_vdd_event_cbf(const HANDLE hDeviceIn, const eVDD_EVENT_CLASS eEventClassIn, VDD_CHAR * sEventIn, const void * pCbfParUserIn)
{
    uVDD_OPEN_DEVICE_OUT* puOpenDeviceOut = (uVDD_OPEN_DEVICE_OUT*)pCbfParUserIn;

    LSA_UNUSED_ARG(hDeviceIn);
    LSA_UNUSED_ARG(sEventIn);

    if (eEventClassIn == eVDD_EVENT_CLASS_FATAL_FW)
    {
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_vdd_event_cbf() - FATAL ERROR in VDD (%s), please check the logfile!", puOpenDeviceOut->sPciLocShort);   
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_ADVANCED_FATAL, EPS_EXIT_CODE_ADVANCED_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "EPS VDD eps_vdd_event_cbf()");
    }
}

/**
* converts an error string of the VDD
*
* VDD returns unicode strings including '\n'
* We have to convert from unicode and delete the \n to be able to trace the string correctly
*
* @param [in]  pIn      pointer to the original string
* @param [out] pOut     pointer to the converted string
* @return
*/
LSA_VOID eps_convert_vdd_err_string(VDD_CHAR const* pIn, LSA_CHAR* pOut)
{
    UINT32  lLoop;
    UINT32  lNewPos = 0;
    UINT32  lSizeIn;

    #if defined (VDD_UNICODE)
    fnVDDConvertStringWcharToChar(pIn, pOut);
    #else
    eps_strcpy(pOut, pIn);
    #endif

    lSizeIn = eps_strlen(pOut);

    for (lLoop = 0; lLoop <= lSizeIn; lLoop++)
        // every char in input string
    {
        if (pOut[lLoop] == '\n')
            // char is new line
        {
            // do nothing because we want to remove it
        }
        else if (pOut[lLoop] == '\0')
            // char is \0
        {
            // write the \0 one position forward, to overwrite the new line
            pOut[lNewPos] = pOut[lLoop];
        }
        else
            // every other char
        {
            // copy to actual position and increment pos
            pOut[lNewPos] = pOut[lLoop];
            lNewPos++;
        }
    }
}

/**
* converts a path string for the VDD to wchar (UNICODE)
*
*
* @param [in]  pIn   pointer to the original string
* @param [out] pOut  pointer to the converted string
* @return
*/
LSA_VOID eps_convert_vdd_path(LSA_CHAR const* pIn, VDD_CHAR * pOut)
{
    #if defined (VDD_UNICODE)
    fnVDDConvertStringCharToWchar(pIn, pOut);
    #else
    eps_strcpy(pOut, pIn);
    #endif
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
