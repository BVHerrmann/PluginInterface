#ifndef EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_H //reinclude-protection
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_H

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
/*  F i l e               &F: eddi_prm_record_pdir_subframe_data.h      :F&  */
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

#if defined (EDDI_CFG_DFP_ON)

LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCalculateMaxSubframeLength( EDDI_LOCAL_DDB_PTR_TYPE const  pDDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCheckSubframeBlock( EDDI_LOCAL_MEM_U8_PTR_TYPE                       * pRecord,
                                                                EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE       pPDIRSubframeBlock,
                                                                EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE       pPDIRSubframeBlockNetworkFormat,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB  );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCheckPDIRSubframeData( EDDI_LOCAL_MEM_U8_PTR_TYPE                       * pRecord,
                                                                   EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE        PDIRSubframeData,
                                                                   EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE        PDIRSubframeDataNetworkFormat,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB  );
                                                                 
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckAndCopyPdirSubframeData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const  pPrmWrite,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB ); 
           
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopySubframeData( EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE  pPDIRSubframeDataDst,
                                                            EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE  pPDIRSubframeDataSrc);
                                                            
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopySubframeBlock( EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE         pPDIRSubframeBlockDataDst,
                                                             EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE         pPDIRSubframeBlockDataSrc,
                                                             LSA_UINT32                                    const  NumberOfSubframeBlocks) ;                                                       
 
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCalcSubframeCount( EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE  const  pPDIRSubframeBlockData ) ;

//SFIOCRProperties
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DISTRIBUTED_WATCH_DOG_FACTOR       EDDI_BIT_MASK_PARA(7 , 0)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_RESTART_FACTOR_FOR_DISTRIBUTED_WD  EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_MODE                           EDDI_BIT_MASK_PARA(23,16)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_RESERVED_2                         EDDI_BIT_MASK_PARA(27,24)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_RESERVED_3                         EDDI_BIT_MASK_PARA(28,28)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION                      EDDI_BIT_MASK_PARA(29,29)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_REDUNDANT_PATH_LAYOUT          EDDI_BIT_MASK_PARA(30,30)
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_SFCRC16                            EDDI_BIT_MASK_PARA(31,31)

#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION_INBOUND              0x0
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_DFP_DIRECTION_OUTBOUND             0x1

#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_MIN_DISTRIBUTED_WATCH_DOG_FACTOR   0x03
#define EDDI_PRM_RECORD_SFIOCR_PROPERTIES_MAX_DISTRIBUTED_WATCH_DOG_FACTOR   0x1F

//SubframeData
#define EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION                               EDDI_BIT_MASK_PARA(6 , 0)
#define EDDI_PRM_RECORD_SUBFRAME_DATA_RESERVED_1                             EDDI_BIT_MASK_PARA(7 , 7)
#define EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH                            EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_PRM_RECORD_SUBFRAME_DATA_RESERVED_2                             EDDI_BIT_MASK_PARA(31,16)

//Ranges
#define EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MAX               EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL
#define EDDI_PRM_RECORD_SUBFRAME_NUMBER_OF_SUBFRAME_BLOCKS_MIN               0x1

#define EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MAX                           0x7F
#define EDDI_PRM_RECORD_SUBFRAME_DATA_POSITION_MIN                           0x1

#define EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MAX                        EDD_CSRT_DFP_DATALEN_MAX
#define EDDI_PRM_RECORD_SUBFRAME_DATA_DATA_LENGTH_MIN                        EDD_CSRT_DFP_DATALEN_MIN

#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_H


/*****************************************************************************/
/*  end of file eddi_prm_record_pdir_subframe_data.h                         */
/*****************************************************************************/
