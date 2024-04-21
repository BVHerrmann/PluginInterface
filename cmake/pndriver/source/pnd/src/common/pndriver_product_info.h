#ifndef PNDRIVER_PRODUCT_INFO_H
#define PNDRIVER_PRODUCT_INFO_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pndriver_product_info.h                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  This header file holds PN Driver's product information.                  */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*===========================================================================*/
//Product specific defines
/*===========================================================================*/

#define PND_VENDOR                              "Siemens AG"
#define PND_PRODUCT_FAMILY                      "PN DRIVER"
#define PND_IM_DEVICE_TYPE                      "PN DRIVER"
#define PND_MLFB                                "6ES7195-3AA00-0YA0"

#define PND_VERSION_REVISION_PREFIX             'V'
#define PND_VERSION_FUNCTIONAL_ENHANCEMENT      2
#define PND_VERSION_BUG_FIX                     1
#define PND_VERSION_INTERNAL_CHANGE             0

#define PND_HW_REVISION                         0
#define PND_PRODUCT_SERIAL_NUMBER              "S0000000"

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PNDRIVER_PRODUCT_INFO_H */