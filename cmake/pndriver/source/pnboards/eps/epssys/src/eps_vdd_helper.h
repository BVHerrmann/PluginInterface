#ifndef EPS_VDD_HELPER_H_
#define EPS_VDD_HELPER_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: eps_vdd_helper.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS VDD                                                                  */
/*                                                                           */
/*****************************************************************************/
#define EPS_VDD_PRINTF_DRIVER_INFO    
    
void __stdcall eps_vdd_event_cbf(const HANDLE hDeviceIn, const eVDD_EVENT_CLASS eEventClassIn, VDD_CHAR * sEventIn, const void * pCbfParUserIn);    
    
LSA_VOID       eps_convert_vdd_err_string             (VDD_CHAR const* pIn, LSA_CHAR* pOut);
LSA_VOID       eps_convert_vdd_path                   (LSA_CHAR const* pIn, VDD_CHAR * pOut);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_VDD_HELPER_H_ */