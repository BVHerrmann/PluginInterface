#ifndef EDDI_PRM_REQ_H          //reinclude-protection
#define EDDI_PRM_REQ_H

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
/*  F i l e               &F: eddi_prm_req.h                            :F&  */
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
/*                                                                           */
/*****************************************************************************/



/*===========================================================================*/
/*                              Types                                        */
/*===========================================================================*/
/*===========================================================================*/
/*                              Macros                                       */
/*===========================================================================*/
/** macro to set values for EDD_RQB_PRM_END_TYPE
 *
 * @param   pPrmEnd         pointer to struct EDD_RQB_PRM_END_TYPE
 * @param   _err_port_id    LSA_UINT16 value for err_port_id
 * @param   _err_index      LSA_UINT32 value for err_index
 * @param   _err_offset     LSA_UINT32 value for err_offset
 * @param   _err_fault      LSA_BOOL value for err_fault
 */
#define EDDI_RQB_PRM_END_SET(pPrmEnd, _err_port_id, _err_index, _err_offset, _err_fault) \
{                                                                                       \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_port_id  = (_err_port_id);     \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_index    = (_err_index);       \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_offset   = (_err_offset);      \
    ((EDD_UPPER_PRM_END_PTR_TYPE)(pPrmEnd))->err_fault    = ((_err_fault) ? EDD_PRM_ERR_FAULT_WRONG_OR_TOO_MANY_PARAMS : EDD_PRM_ERR_FAULT_NO_OR_TOO_LESS_PARAMS);   \
}

/*===========================================================================*/
/*                              Protos                                       */
/*===========================================================================*/
LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_PrmRequest( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                  EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmOpenChannel( EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCloseChannel( EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

#if defined (EDDI_RED_PHASE_SHIFT_ON)
LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_PrmTransitionDoneCbf( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif

LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_PrmMoveBToAPart2( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesAddElement( EDDI_LOCAL_DDB_PTR_TYPE             const pDDB,
                                                                        EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pFrmData );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesCheckBranches( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesSetup( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR    EDDI_PrmPDIRCheckFrameTimesInit( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR    EDDI_PrmPDIRCheckFrameTimesExit( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetBlockTypeLength(LSA_UINT8 *  const pBlockHeader,
                                                            LSA_UINT16 * const pBlockTypeNetwork,
                                                            LSA_UINT16 * const pBlockLength );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_PrmCalculateTriggerDeadLine( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                               LSA_UINT32              const CycleBaseFactor,
                                                               LSA_UINT32                  * pTriggerDeadLine10ns,
                                                               LSA_UINT16                  * pTriggerMode );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_REQ_H


/*****************************************************************************/
/*  end of file eddi_prm_req.h                                               */
/*****************************************************************************/
