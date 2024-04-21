#ifndef EDDI_SYS_SHOWLOCATION_H //reinclude-protection
#define EDDI_SYS_SHOWLOCATION_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_sys_showlocation.h                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* prototypes for "Show Location" functions                                  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  25.08.08    AB    created                                                */
/*                                                                           */
/*****************************************************************************/ 

#if defined (EDDI_CFG_ERTEC_200)

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SetPortModeEB200( LSA_UINT32  const  HwPortIndex,
                                                     LSA_BOOL    const  bSwitchToStandardFunction );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_SetLEDOnOffEB200( LSA_UINT32  const  HwPortIndex,
                                                     LSA_UINT16  const  LEDMode );

#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYS_SHOWLOCATION_H


/*****************************************************************************/
/*  end of file eddi_sys_showlocation.h                                      */
/*****************************************************************************/
