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
/*  F i l e               &F: eps_adonis_cache_drv.h                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Adonis Soc1 Cache driver                                             */
/*                                                                           */
/*****************************************************************************/


#ifndef EPS_ADONIS_CACHE_DRV_H_
#define EPS_ADONIS_CACHE_DRV_H_

/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/
#define EPS_ADONIS_CACHE_DRV_NAME     "/dev/eps_cache" /* Name of driver */

#define EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_INVALIDATE     1
#define EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_WRITEBACK      2
#define EPS_ADONIS_CACHE_IOCTL_MEM_CACHE_WB_INV         3
#define EPS_ADONIS_CACHE_IOCTL_MEM_CMP_UNCACHED         4

/*---------------------------------------------------------------------------*/
/* Prototypes                                                                */
/*---------------------------------------------------------------------------*/
LSA_INT32 eps_adonis_cache_sync(LSA_VOID * address, LSA_UINT32 len);
LSA_INT32 eps_adonis_cache_drv_install(LSA_VOID);
LSA_INT32 eps_adonis_cache_drv_uninstall(LSA_VOID);


#endif /*EPS_ADONIS_CACHE_DRV_H_*/
