#ifndef EPS_C66X_SHM_H_
#define EPS_C66X_SHM_H_

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
/*  F i l e               &F: eps_c66x_shm.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS C66x Shared Memory                                                   */
/*                                                                           */
/*****************************************************************************/

    
/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_c66x_shm_init();

LSA_VOID   eps_c66x_shm_write_c66xReleaseCmd();
LSA_VOID   eps_c66x_shm_wait_c66xReadyCmd();

LSA_VOID   eps_c66x_shm_write(LSA_UINT32 offset, LSA_UINT32 val);
LSA_UINT32 eps_c66x_shm_read(LSA_UINT32 offset);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* EPS_C66X_SHM_H_ */
