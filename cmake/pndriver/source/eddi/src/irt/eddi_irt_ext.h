#ifndef EDDI_IRT_EXT_H          //reinclude-protection
#define EDDI_IRT_EXT_H

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
/*  F i l e               &F: eddi_irt_ext.h                            :F&  */
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

/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
// Macro to convert FrameSendOffsetNs in IR_RECORD to FCWTimeNs
// --> Depends on Revision
/*===========================================================================*/

//Correct Propagation-Delay between Transfer-Unit and Tx-Timestamp-Unit
#define EDDI_IRT_CONVERT_FRAME_SEND_OFFSET(_FrameSendOffsetNs)  ((_FrameSendOffsetNs) - EDDI_PREFRAME_SEND_DELAY_100MBIT_NS)

/*---------------------------------------------------------------------------*/
/* External functions                                                        */
/*---------------------------------------------------------------------------*/
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTInitFcwLists   ( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTProviderAdd    ( EDDI_LOCAL_DDB_PTR_TYPE                const pDDB,
                                                        EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE    const pFrmHandler,
                                                        EDD_MAC_ADR_TYPE                        const DstMacAdr,
                                                        EDDI_DEV_MEM_U8_PTR_TYPE                const pFrameBuffer,
                                                        LSA_UINT8                               const ImageMode,
                                                        LSA_UINT32                            * const ppFCW,
                                                        LSA_BOOL                                const bXCW_DBInitialized);

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTProviderRemove ( EDDI_LOCAL_DDB_PTR_TYPE                const pDDB,
                                                        EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE    const pFrmHandler );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTConsumerAdd    ( EDDI_LOCAL_DDB_PTR_TYPE                const pDDB,
                                                        EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE    const pFrmHandler,
                                                        EDDI_CRT_CONSUMER_PARAM_PTR_TYPE       const pLowerParams,
                                                        EDD_UPPER_MEM_U8_PTR_TYPE              const pKramDataBufferRed );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTConsumerRemove ( EDDI_LOCAL_DDB_PTR_TYPE              const pDDB,
                                                        EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const pFrmHandler );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTFcwAdd         ( EDDI_LOCAL_DDB_PTR_TYPE        const pDDB,
                                                        EDDI_IRT_FRM_HANDLER_PTR_TYPE  const pFrmHandler );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_IRTFcwRemove      ( EDDI_LOCAL_DDB_PTR_TYPE        const pDDB,
                                                        EDDI_IRT_FRM_HANDLER_PTR_TYPE  const pFrmHandler );

LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_IRTIsTxPortSet    ( EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pIrFrameDataElem,
                                                        LSA_UINT32                          const PortCnt,
                                                        LSA_UINT32                          const UsrPortIndex );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IRT_EXT_H


/*****************************************************************************/
/*  end of file eddi_irt_ext.h                                               */
/*****************************************************************************/
