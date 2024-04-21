
#ifndef GSY_PLS_H                     /* ----- reinclude-protection ----- */
#define GSY_PLS_H

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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2006-06-22  P03.12.00.00_00.02.01.02 lrg  from LSA_GLOB P2.1.0.0_0.7.1.1 */
/*              LSA_HOST_ENDIANESS_BIG und LSA_HOST_ENDIANESS_LITTLE neu.    */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2010-03-23  P05.00.00.00_00.01.04.02 lrg: Arts-RQ AP00933526             */
/*              Support only SyncId CLOCK and TIME                           */
/*  2015-06-09  V06.00, Inc07, Feature 1227823, Task 1261424                 */
/*              GSY und CLP Konfigurationsschalter bereinigen                */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined LSA_HOST_ENDIANESS_BIG && !defined LSA_HOST_ENDIANESS_LITTLE
#error "LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE is missing in file lsa_cfg.h!"
#endif

#if defined LSA_HOST_ENDIANESS_BIG && defined LSA_HOST_ENDIANESS_LITTLE
#error "Define LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE in file lsa_cfg.h!"
#endif

#if !defined GSY_UPPER_IN_FCT_ATTR
#error "GSY_UPPER_IN_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_UPPER_OUT_FCT_ATTR
#error "GSY_UPPER_OUT_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_LOWER_IN_FCT_ATTR
#error "GSY_LOWER_IN_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_LOWER_OUT_FCT_ATTR
#error "GSY_LOWER_OUT_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_SYSTEM_IN_FCT_ATTR
#error "GSY_SYSTEM_IN_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_SYSTEM_OUT_FCT_ATTR
#error "GSY_SYSTEM_OUT_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_LOCAL_FCT_ATTR
#error "GSY_LOCAL_FCT_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_UPPER_RQB_ATTR
#error "GSY_UPPER_RQB_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_UPPER_MEM_ATTR
#error "GSY_UPPER_MEM_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_LOWER_RQB_ATTR
#error "GSY_LOWER_RQB_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_LOWER_MEM_ATTR
#error "GSY_LOWER_MEM_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_SYSTEM_MEM_ATTR
#error "GSY_SYSTEM_MEM_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined GSY_LOCAL_MEM_ATTR
#error "GSY_LOCAL_MEM_ATTR not defined in file gsy_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined GSY_LOCAL_MEM_STATIC && !defined GSY_LOCAL_MEM_DYNAMIC
#error "GSY_LOCAL_MEM_STATIC or GSY_LOCAL_MEM_DYNAMIC is missing in file gsy_cfg.h!"
#endif

#if defined GSY_LOCAL_MEM_STATIC && defined GSY_LOCAL_MEM_DYNAMIC
#error "Define GSY_LOCAL_MEM_STATIC or GSY_LOCAL_MEM_DYNAMIC in file gsy_cfg.h!"
#endif


#if !defined GSY_SYNC_ID_SUPPORTED
#error "GSY_SYNC_ID_SUPPORTED not defined in file gsy_cfg.h"
#else
#if (GSY_SYNC_ID_SUPPORTED == 0x00000000)
#error "GSY_SYNC_ID_SUPPORTED defined = 0x00000000 in file gsy_cfg.h"
#endif
#if (GSY_SYNC_ID_SUPPORTED > 0x00000003)
#error "GSY_SYNC_ID_SUPPORTED defined > 0x00000003 in file gsy_cfg.h"
#endif
#endif



/*****************************************************************************/
/*  end of file GSY_PLS.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_PLS_H */
