#ifndef EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_H //reinclude-protection
#define EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_H

#ifdef __cplusplus                           //If C++ - compiler: Use C linkage
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
/*  F i l e               &F: eddi_prm_record_pdport_data_adjust.h      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Version of prefix                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  06.02.15    TH    refactor AdjustPortState to AdjustLinkState            */
/*                                                                           */
/*****************************************************************************/


LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDPortDataAdjust( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const pPrmWrite,
                                                                EDDI_LOCAL_DDB_PTR_TYPE             const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustLinkState( const EDDI_PRM_RECORD_ADJUST_LINK_STATE_TYPE * const pAdjustLinkState,
                                                                  LSA_UINT16                               const UsrPortIndex,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                  const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustMAUType( const EDDI_PRM_RECORD_ADJUST_MAU_TYPE_TYPE * const pAdjustMAUType,
                                                                LSA_UINT16                             const UsrPortIndex,
                                                                LSA_UINT8                              const Local,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustMulticastBoundary( const EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_TYPE * const pAdjustMAUType,
                                                                          LSA_UINT16                                       const UsrPortIndex,
                                                                          EDDI_LOCAL_DDB_PTR_TYPE                          const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustDomainBoundary( const EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_TYPE *   const pAdjustMAUType,
                                                                       LSA_UINT16                                      const UsrPortIndex,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE                         const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckAdjustDCPBoundary( const EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_TYPE * const pAdjustMAUType,
                                                                    LSA_UINT16                                 const UsrPortIndex,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE                    const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPreambleLength( const EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_TYPE  *  const  pAdjustPreambleLength,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE                         const  pDDB );
  
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SetLinkStatusForFastForwarding( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_H


/*****************************************************************************/
/*  end of file eddi_prm_record_pdport_data_adjust.h                         */
/*****************************************************************************/
