#ifndef EDDI_PRM_RECORD_PDIR_DATA_H //reinclude-protection
#define EDDI_PRM_RECORD_PDIR_DATA_H

#ifdef __cplusplus                  //If C++ - compiler: Use C linkage
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
/*  F i l e               &F: eddi_prm_record_pdir_data.h               :F&  */
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

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDIRData( EDD_CONST_UPPER_PRM_WRITE_PTR_TYPE  const pPrmWrite,
                                                        EDDI_LOCAL_DDB_PTR_TYPE             const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckPDIRDataMaxPortDelays( EDDI_LOCAL_DDB_PTR_TYPE                            const  pDDB,
                                                                     EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE const *  const  pPDIRDataGlobalExt,
                                                                     LSA_BOOL                                           const  bGlobalDataExtended,
                                                                     LSA_UINT32                                         const  UsrPortIndex,
                                                                     LSA_UINT32                                      *  const  pErrOffset);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRFrameDataCheckElement( EDDI_LOCAL_DDB_PTR_TYPE                        pDDB,
                                                                 EDDI_PRM_RECORD_FRAME_DATA_TYPE  const * const pFrmData);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRFrameDataBlockHeader( EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  const *  const  pPDIRFrameData,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE                        const  pDDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRGlobalDataBlockHeader( EDDI_PRM_RECORD_HEADER_TYPE  const * const pBlockHeader,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE              const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRDataBlockHeader( EDDI_PRM_RECORD_HEADER_TYPE     const * const pBlockHeader,
                                                                EDDI_LOCAL_DDB_PTR_TYPE                 const pDDB,
                                                                LSA_UINT32                              const RecordDataLength);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckPDIRBeginEndBlockHeader( EDDI_PRM_RECORD_HEADER_TYPE const * const pBlockHeader,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE             const pDDB);

EDDI_PRM_RECORD_FRAME_DATA_TYPE  * EDDI_LOCAL_FCT_ATTR  EDDI_CheckGetNextFrameDataElement ( EDDI_PRM_RECORD_FRAME_DATA_TYPE  * const pFrmData);

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_PDIRFrameCheckAndSubstituteSecondaryRTSync(EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE const pFrmData);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRBeginEndParse( EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE  const *  const pBlockBeginEnd,
                                                         EDDI_LOCAL_MEM_U8_PTR_TYPE                               pRecord,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                            const pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRBeginEndValidate( EDDI_LOCAL_DDB_PTR_TYPE                               const  pDDB, 
                                                            EDDI_IRT_BEGIN_END_DATA_TYPE  EDDI_LOCAL_MEM_ATTR  *  const  pBeginEndData );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PDIRBeginEndValidateOrangeEnds( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                                      EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE    const  pRecordSet );        

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirData( EDDI_LOCAL_MEM_U8_PTR_TYPE                * pRecord,
                                                         EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE        * pPDIRData,
                                                         EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE        * pPDIRDataNetworkFormat,
                                                         EDDI_LOCAL_DDB_PTR_TYPE            const    pDDB  );               

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirGlobalDataExt( EDDI_LOCAL_MEM_U8_PTR_TYPE                        * pRecord,
                                                                  EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE         * pPDIRGlobalDataExt,
                                                                  EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE         * pPDIRGlobalDataExtNetworkFormat,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB  );              
                                                                                 
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirFrameData( EDDI_LOCAL_MEM_U8_PTR_TYPE                  * pRecord,
                                                              EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE        * pFrameData,
                                                              EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE        * pFrameDataNetworkFormat,
                                                              EDDI_LOCAL_DDB_PTR_TYPE               const   pDDB  );              

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirBeginEndData( EDDI_LOCAL_MEM_U8_PTR_TYPE                       * pRecord,
                                                                 EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE         * pPDIRBeginEndData,
                                                                 EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE         * pPDIRBeginEndDataNetworkFormat,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE                const    pDDB );             
                  
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctGetPdirFrameDataArray( EDDI_LOCAL_MEM_U8_PTR_TYPE              * pRecord,
                                                                   EDDI_PRM_RECORD_FRAME_DATA_TYPE         * pFrameData,
                                                                   EDDI_PRM_RECORD_FRAME_DATA_TYPE         * pFrameDataNetworkFormat,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE          const    pDDB );   


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopyPDIRData( EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE     * pPDIRDataDst,
                                                        EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE     * pPDIRDataSrc,
                                                        LSA_BOOL                           const bGlobalDataExtended );
                                                        
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopyPDIRFrameData( EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  * pPDIRFrameDataDst,
                                                             EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE  * pPDIRFrameDataSrc );
                                                             
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PrmFctCopyGlobalDataExtended( EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE  * pGlobalDataExtendedDst,
                                                                  EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE  * pGlobalDataExtendedSrc );
                                                                  
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_PrmCheckReductionRatioIsPowerOf2( LSA_UINT32 const ReductionRatio);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_PrmPDIRCheckFrameTimesAddFrameToList( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                            LSA_UINT32                           const  UsrPortIdx,
                                                                            LSA_UINT32                           const  NodeIndex,
                                                                            EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData,
                                                                            LSA_BOOL                             const  bIsRx );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CheckFrameDataProperties( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
                                                                 
//FrameDataProperties                                                                  
#define EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MODE                                        EDDI_BIT_MASK_PARA (0, 0)
#define EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FORWARDING_MULTICAST_MAC_ADD                           EDDI_BIT_MASK_PARA (2, 1)   
#define EDDI_PRM_BITMASK_PDIRDATA_FRAME_DATA_FRAGMENTATION_MODE                                     EDDI_BIT_MASK_PARA (4, 3) 

#define  EDDI_PRM_PDIR_FRAMEDATA_FORWARDING_MODE_ABSOLUTE_MODE                                      0x00              /* Not allowed in EDDI */
#define  EDDI_PRM_PDIR_FRAMEDATA_FORWARDING_MODE_RELATIVE_MODE                                      0x01                                                                           

#define  EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_LEGACY                                               0x00
#define  EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_RT_CLASS_3                                           0x01
#define  EDDI_PRM_PDIR_FRAMEDATA_FF_MC_MAC_ADD_FF                                                   0x02

#define  EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_OFF                                                  0x00
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_128B                                              0x01
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_ON_256B                                              0x02
#define  EDDI_PRM_FRAGMENTATION_YELLOWTIME_FOR_FRAGMETATION_OFF                                     125000UL 

#define  EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_SHORT                                                128
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAGMENTATION_LONG                                                 256

//FrameDetails
#define EDDI_PRM_RECORD_FRAME_DETAILS_BIT_SYNC_FRAME                                                EDDI_BIT_MASK_8BIT(1,0)
#define EDDI_PRM_RECORD_FRAME_DETAILS_BIT_MEANING_FRAME_SEND_OFFSET                                 EDDI_BIT_MASK_8BIT(3,2)
#define EDDI_PRM_RECORD_FRAME_DETAILS_BIT_RESERVED                                                  EDDI_BIT_MASK_8BIT(6,4)
#define EDDI_PRM_RECORD_FRAME_DETAILS_MEDIA_REDUNDANCY_WATCHDOG                                     EDDI_BIT_MASK_8BIT(7,7)
   
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_SYNCFRAME_NO_SYNC_FRAME                               0x00
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_SYNCFRAME_PRIMARY_SYNC_FRAME                          0x01
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_SYNCFRAME_SECONDARY_SYNC_FRAME                        0x02

#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEANING_FRAMESENDOFFSET_SPECIFIES_TIME                0x00
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEANING_FRAMESENDOFFSET_BEGINNING_PHASE_RT_CLASS_3    0x01
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEANING_FRAMESENDOFFSET_ENDING_PHASE_RT_CLASS_3       0x02

#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEDIA_REDUNDANCY_WATCHDOG_DISABLE                     0x00
#define  EDDI_PRM_PDIR_FRAMEDATA_FRAMEDETAILS_MEDIA_REDUNDANCY_WATCHDOG_ENABLE                      0x01

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_RECORD_PDIR_DATA_H


/*****************************************************************************/
/*  end of file eddi_prm_record_pdir_data.h                                  */
/*****************************************************************************/
