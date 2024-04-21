#ifndef EDDI_GLB_H              //reinclude-protection
#define EDDI_GLB_H

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
/*  F i l e               &F: eddi_glb.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines, internal constants, types, data, macros and prototyping for     */
/*  EDDI.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RequestFinish( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                 EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                 LSA_RESULT               const  Status );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_FatalError( LSA_UINT32     const  Line,
                                              LSA_UINT8   *  const  sFile,
                                              LSA_UINT32     const  ModuleID,
                                              LSA_UINT8   *  const  sErr,
                                              LSA_UINT32     const  Error,
                                              LSA_UINT32     const  DW_0,
                                              LSA_UINT32     const  DW_1 );

//#define EDDI_CFG_NO_FATAL_FILE_INFO

#if defined (EDDI_CFG_NO_FATAL_FILE_INFO)
#define EDDI_Excp(sErr, Error, DW_0, DW_1) \
    /*lint --e(961) */   \
    EDDI_FatalError((LSA_UINT32)__LINE__, (LSA_UINT8 *)(void *)EDDI_NULL_PTR, EDDI_MODULE_ID, \
    (LSA_UINT8 *)(void *)(sErr), (LSA_UINT32)(Error), (LSA_UINT32)(DW_0), (LSA_UINT32)(DW_1))
#else
#define EDDI_Excp(sErr, Error, DW_0, DW_1) \
    /*lint --e(961) */   \
    EDDI_FatalError((LSA_UINT32)__LINE__, (LSA_UINT8 *)(void *)__FILE__, EDDI_MODULE_ID, \
    (LSA_UINT8 *)(void *)(sErr), (LSA_UINT32)(Error), (LSA_UINT32)(DW_0), (LSA_UINT32)(DW_1))
#endif

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_QueueAddToEnd( EDDI_QUEUE_PTR_TYPE         const pQueue,
                                                  EDDI_QUEUE_ELEMENT_PTR_TYPE const pNewElement);

#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV7)
LSA_VOID EDDI_LOCAL_FCT_ATTR EDDIQueueAddToBegin( EDDI_QUEUE_PTR_TYPE         const pQueue,
                                                  EDDI_QUEUE_ELEMENT_PTR_TYPE const pNewElement );
#endif

EDDI_QUEUE_ELEMENT_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_QueueRemoveFromEnd( EDDI_QUEUE_PTR_TYPE        const pQueue );

EDDI_QUEUE_ELEMENT_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_QueueRemoveFromBegin( EDDI_QUEUE_PTR_TYPE      const pQueue );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_QueueRemove( EDDI_QUEUE_PTR_TYPE         const  pQueue,
                                                EDDI_QUEUE_ELEMENT_PTR_TYPE const  pElement);

EDDI_QUEUE_ELEMENT_PTR_TYPE EDDI_LOCAL_FCT_ATTR  EDDI_QueueGetNext( EDDI_QUEUE_PTR_TYPE         const  pQueue,
                                                                    EDDI_QUEUE_ELEMENT_PTR_TYPE const  pElement);

LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_AddToQueueEnd( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                   EDDI_RQB_QUEUE_TYPE      * const  pQueue,
                                                   EDD_UPPER_RQB_PTR_TYPE     const  pRQB );

EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                                   EDDI_RQB_QUEUE_TYPE      * const pQueue );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_InitQueue( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                              EDDI_RQB_QUEUE_TYPE      * const  pQueue );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_GLB_H


/*****************************************************************************/
/*  end of file eddi_glb.h                                                   */
/*****************************************************************************/
