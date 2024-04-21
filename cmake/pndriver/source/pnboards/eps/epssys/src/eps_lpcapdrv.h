#ifndef EPS_LPCAPDRV_H_
#define EPS_LPCAPDRV_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2017 Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: eps_mem.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.01.00.09         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2017-12-19                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Memory                                                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_lpcapdrv_uninstall(LSA_VOID);
LSA_VOID eps_lpcapdrv_install(LSA_VOID);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif
#endif  /* of EPS_LPCAPDRV_H_ */
