#ifndef EPS_ADONIS_IPC_DRV_H_
#define EPS_ADONIS_IPC_DRV_H_

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
/*  F i l e               &F: eps_adonis_ipc_drv.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Adonis Inter Processor Communication driver                          */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/

#define EPS_ADONIS_IPC_DRV_NAME     "/dev/eps_ipc" /* Name of driver */

#define EPS_ADONIS_IPC_DRV_ALLOC_LOCAL_NOTIFY_INFO    1
#define EPS_ADONIS_IPC_DRV_FREE_LOCAL_NOTIFY_INFO     2
#define EPS_ADONIS_IPC_DRV_TRANSLATE_NOTIFY_INFO      3
#define EPS_ADONIS_IPC_DRV_LOCAL_INT_ENABLE           4
#define EPS_ADONIS_IPC_DRV_LOCAL_INT_DISABLE          5  
#define EPS_ADONIS_IPC_DRV_FREE_REMOTE_NOTIFY_INFO    6

/*---------------------------------------------------------------------------*/
/* Prototypes                                                                */
/*---------------------------------------------------------------------------*/

LSA_INT32 eps_adonis_ipc_drv_install(void);
LSA_INT32 eps_adonis_ipc_drv_uninstall(void);

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
} // extern "C" 
#endif

/*****************************************************************************/
#endif  /* of EPS_ADONIS_IPC_DRV_H_ */
