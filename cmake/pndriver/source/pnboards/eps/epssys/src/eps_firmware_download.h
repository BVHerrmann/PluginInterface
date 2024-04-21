#ifndef EPS_FIRMWARE_DOWNLOAD_H                   /* ----- reinclude-protection ----- */
#define EPS_FIRMWARE_DOWNLOAD_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_firmware_download.h                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Firmware downloader for the user                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/
//lint --e(537) Repeated include file - The header is necessary here, but other C-Files may have already included eps_usr.h
#include "eps_usr.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef struct eps_firmware_download_tag
{
    LSA_UINT16                  hdId;                                           /// IN 0 - LD Firmware, 1...4 HD Firmware
    EPS_PNDEV_LOCATION_TYPE     uLocation;                                      /// IN location specifier. Selects if MAC Address or PCI-Location should be used. Currently, only PCI Location is valid for firmware download, @see EPS_PNDEV_LOCATION_TYPE
	EPS_PNDEV_OPEN_OPTION_TYPE  uOption;                                        /// IN option specifier, for firmware download we only use PCI location. @see EPS_PNDEV_OPEN_OPTION_TYPE 
}EPS_FIRMWARE_DOWNLOAD_TYPE, *EPS_FIRMWARE_DOWNLOAD_PTR_TYPE;
typedef EPS_FIRMWARE_DOWNLOAD_TYPE const* EPS_FIRMWARE_DOWNLOAD_CONST_PTR_TYPE;


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_UINT16 eps_firmware_download_and_start  (EPS_FIRMWARE_DOWNLOAD_PTR_TYPE pFwDownloadConfig);
LSA_UINT16 eps_firmware_stop                (EPS_FIRMWARE_DOWNLOAD_CONST_PTR_TYPE pFwDownloadConfig);

// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"
//lint -esym(714, eps_firmware_download_and_start)
//lint -esym(759, eps_firmware_download_and_start)
//lint -esym(765, eps_firmware_download_and_start)
//lint -esym(714, eps_firmware_stop)
//lint -esym(759, eps_firmware_stop)
//lint -esym(765, eps_firmware_stop)
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_FIRMWARE_DOWNLOAD_H */
