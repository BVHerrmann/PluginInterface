#ifndef EPS_NOSHMDRV_H_
#define EPS_NOSHMDRV_H_

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
/*  F i l e               &F: eps_noshmdrv.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS No Shared Memory - Shared Memory Driver                              */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_noshmdrv_install  (EPS_SHM_INSTALL_ARGS_CONST_PTR_TYPE pInstallArgs);
LSA_UINT16 eps_noshmdrv_open     (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppHwInstOut);
LSA_UINT16 eps_noshmdrv_close    (EPS_SHM_HW_CONST_PTR_TYPE pHwInstIn);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_NOSHMDRV_H_ */