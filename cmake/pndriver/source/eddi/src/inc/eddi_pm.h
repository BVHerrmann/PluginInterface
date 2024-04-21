#ifndef EDDI_PM_H               //reinclude-protection
#define EDDI_PM_H

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
/*  F i l e               &F: eddi_pm.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

LSA_VOID    EDDI_LOCAL_FCT_ATTR EDDI_PmCheck( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                              EDDI_UPPER_DPB_PTR_TYPE  const  pDPB );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_PmGetHwPortIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  UsrPortIndex );

LSA_BOOL EDDI_LOCAL_FCT_ATTR   EDDI_PmIsValidHwPortIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_PmGetMask0420( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                   LSA_UINT32               const  UsrPortIndex );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_PmGetMask0108( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                   LSA_UINT32               const  UsrPortIndex );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_PmGetMask0210( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                   LSA_UINT32               const  UsrPortIndex );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_PmUsrPortIDToHwPort03( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  UsrPortId );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PM_H


/*****************************************************************************/
/*  end of file eddi_pm.h                                                    */
/*****************************************************************************/
