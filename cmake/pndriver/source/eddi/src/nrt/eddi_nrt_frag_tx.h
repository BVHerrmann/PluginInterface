#ifndef EDDI_NRT_FRAG_TX_H      //reinclude-protection
#define EDDI_NRT_FRAG_TX_H

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
/*  F i l e               &F: eddi_nrt_frag_tx.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NRT- Externals                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#if defined (EDDI_CFG_FRAG_ON)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFragInitStartup( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFragCheckPrmCommitPart1( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTFragCheckPrmCommitPart2( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCalcFragments( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                      EDD_UPPER_RQB_PTR_TYPE             const  pRQB,
                                                      EDD_UPPER_NRT_SEND_PTR_TYPE        const  pRQBSnd,
                                                      LSA_UINT32                         const  SendLength,
                                                      EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  *  const  pNrtTxFragPreData );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTBuildFragments( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                       EDD_UPPER_RQB_PTR_TYPE                   const  pRQB,
                                                       EDD_UPPER_NRT_SEND_PTR_TYPE              const  pRQBSnd,
                                                       EDDI_NRT_TX_FRAG_PRE_DATA_TYPE  const *  const  pNrtTxFragPreData );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NRTTxFragSemaphore( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                        LSA_UINT32                  const  SemaphoreCommand,
                                                        LSA_UINT32                  const  InterfaceNr,
                                                        LSA_UINT32               *  const  pNextFragInterfaceNr );

#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_NRT_FRAG_TX_H


/*****************************************************************************/
/*  end of file eddi_nrt_frag_tx.h                                           */
/*****************************************************************************/
