#ifndef EDDI_CRT_PHASE_EXT_H    //reinclude-protection
#define EDDI_CRT_PHASE_EXT_H

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
/*  F i l e               &F: eddi_crt_phase_ext.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  IRT- Externals                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseClass2Init( EDDI_RTCLASS2_TYPE        * const pClass2,
                                                      EDDI_DDB_TYPE             * const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseTxInit( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                  EDDI_CRT_PHASE_TX_MACHINE   * const  pMachine,
                                                  LSA_UINT32                    const  HwPortIndex );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseTxTrigger ( EDDI_CRT_PHASE_TX_MACHINE * const pMachine,
                                                      EDDI_CRT_PHASE_TX_EVENT     const Event,
                                                      EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseTxTriggerFinish( EDDI_CRT_PHASE_TX_MACHINE * const pMachine,
                                                           EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );
#endif

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseRxInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                  EDDI_CRT_PHASE_RX_MACHINE * const  pMachine,
                                                  LSA_UINT32                  const  HwPortIndex );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseRxTrigger ( EDDI_CRT_PHASE_RX_MACHINE * const pMachine,
                                                      EDDI_CRT_PHASE_RX_EVENT     const Event,
                                                      EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseRxTriggerFinish( EDDI_CRT_PHASE_RX_MACHINE * const pMachine,
                                                           EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );
#endif

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CrtPhaseReset     ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseAreSolsCompleted        ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseAreDisablingFCWsRunning ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseAreTransitionsRunning   ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_CrtPhaseIsTransitionRunning     ( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                      LSA_UINT32              const HwPortIndex );
#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_PHASE_EXT_H


/*****************************************************************************/
/*  end of file eddi_crt_phase_ext.h                                         */
/*****************************************************************************/
