#ifndef EDDI_NRT_Q_H            //reinclude-protection
#define EDDI_NRT_Q_H

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
/*  F i l e               &F: eddi_nrt_q.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NrtGetFromPrioQueueWithLimit( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                                                EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );

#if defined (EDDI_CFG_FRAG_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtFragQueuesTimerCBF( LSA_VOID  *  const  context );
#endif //EDDI_CFG_FRAG_ON

#if defined (EDDI_CFG_FRAG_ON)
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_NrtAddToPrioQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                       EDD_UPPER_RQB_PTR_TYPE      const  pRQB );
#else
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NrtAddToPrioQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF,
                                                       EDD_UPPER_RQB_PTR_TYPE      const  pRQB );
#endif

EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCheckAndRemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                                              EDDI_RQB_QUEUE_TYPE     * const pQueue,
                                                                              EDDI_LOCAL_HDB_PTR_TYPE   const pHDB,
                                                                              LSA_BOOL                  const bSync );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_NRT_Q_H


/*****************************************************************************/
/*  end of file eddi_nrt_q.h                                                 */
/*****************************************************************************/
