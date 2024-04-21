#ifndef EDDI_CRT_DFP_H          //reinclude-protection
#define EDDI_CRT_DFP_H

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
/*  F i l e               &F: eddi_crt_dfp.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
    


/*===========================================================================*/
/*                                Defines                                    */
/*===========================================================================*/
#define EDDI_IS_DFP_CONS(pCons_)                        ((pCons_)->Properties & EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP)
#define EDDI_IS_DFP_PROV(pProv_)                        ((pProv_)->Properties & EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP)

#if defined (EDDI_CFG_DFP_ON)
#define EDDI_DFP_INVALID_DG_CTR                         0xFF

#define EDDI_DS_BIT__Surrogat(Reg)                      (Reg & 0x80)
#define EDDI_SET_DS_BIT__Surrogat(pReg, value)          \
/*lint --e(941) */  (*pReg  = (LSA_UINT8)((*pReg & ~0x80) | ( ((value) << 7) & 0x80 )))

#define EDDI_DS_BIT7_SURROGAT                           0x80

#define EDDI_DFP_DG_HEADER_LENGTH                       4
#define EDDI_DFP_DG_CRC_LENGTH                          2
#define EDDI_DFP_PACKFRM_HEADER_CHECKSUM_LENGTH         2
#define EDDI_DFP_PACKFRM_SF_END_DELIMITER_LENGTH        2

#define EDDI_DFP_PS_ENTRY_ProdState                     0x01UL
#define EDDI_DFP_PS_ENTRY_ProdStateCh                   0x02UL
#define EDDI_DFP_PS_ENTRY_DataStateCh                   0x04UL

#define EDDI_DFP_PNIO_HEADER_SIZE                       (2 * EDD_MAC_ADDR_SIZE /* DA and SA MAC Addr*/ + sizeof(LSA_UINT16) /*ethertype*/ + sizeof(LSA_UINT16) /*frameid*/) 


#define EDDI_CRT_MIN_DFP_SFPOSITION             1
#define EDDI_CRT_MAX_DFP_SFPOSITION             64

#define EDD_CRT_DFP_WD_RELOAD_VALUE                2
                     
                     // check macro: is SFPosition in valid range ?
#define EDDI_CRT_IS_DFP_CONS_SFPOSITION_IN_VALID_RANGE(pConsProv_)                             \
            (   EDDI_IS_DFP_CONS(pConsProv_)                                                   \
             && (   (EDD_SFPOSITION_UNDEFINED != pConsProv_->LowerParams.SFPosition)           \
                 || (EDD_SFPOSITION_INVALID   != pConsProv_->LowerParams.SFPosition)           \
                 || (pConsProv_->LowerParams.SFPosition >= EDDI_CRT_MIN_DFP_SFPOSITION)        \
                 || (pConsProv_->LowerParams.SFPosition <= EDDI_CRT_MAX_DFP_SFPOSITION) )  )   
            
#define EDDI_CRT_IS_DFP_PROV_SFPOSITION_IN_VALID_RANGE(pConsProv_)                             \
            (   EDDI_IS_DFP_PROV(pConsProv_)                                                   \
             && (   (EDD_SFPOSITION_UNDEFINED != pConsProv_->LowerParams.SFPosition)           \
                 || (EDD_SFPOSITION_INVALID   != pConsProv_->LowerParams.SFPosition)           \
                 || (pConsProv_->LowerParams.SFPosition >= EDDI_CRT_MIN_DFP_SFPOSITION)        \
                 || (pConsProv_->LowerParams.SFPosition <= EDDI_CRT_MAX_DFP_SFPOSITION) )  )

#if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
#define EDDI_CRT_GET_SFPOS(pConsProv_) (pConsProv_->LowerParams.SFPosition)
#endif

#else //defined (EDDI_CFG_DFP_ON)

#define EDDI_CRT_IS_DFP_CONS_SFPOSITION_IN_VALID_RANGE(pConsProv_) (!(EDDI_IS_DFP_CONS(pConsProv_)))
#define EDDI_CRT_IS_DFP_PROV_SFPOSITION_IN_VALID_RANGE(pConsProv_) (!(EDDI_IS_DFP_PROV(pConsProv_)))

#if !defined (EDDI_CFG_USE_SW_RPS)
#if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
#define EDDI_CRT_GET_SFPOS(pConsProv_) 0
#endif
#endif

#endif

/*===========================================================================*/
/*                                Structs                                    */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* APDU-Status                                                               */
/*---------------------------------------------------------------------------*/
#if defined (EDDI_CFG_DFP_ON)
#define EDDI_SFAPDU_OFFSET_SFPosition   0
#define EDDI_SFAPDU_OFFSET_SFLen        1
#define EDDI_SFAPDU_OFFSET_SFCycleCtr   2
#define EDDI_SFAPDU_OFFSET_SFDataStatus 3

#include "pnio_pck1_on.h"
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_DFP_APDU_TYPE
{
    LSA_UINT8   SFPosition;
    LSA_UINT8   SFLen;
    LSA_UINT8   SFCycleCtr;
    LSA_UINT8   SFDataStatus;
    
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_DFP_APDU_TYPE EDDI_DFP_APDU_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_DFP_SB_APDU_TYPE
{
    EDDI_DFP_APDU_TYPE  SFAPDU;
    LSA_UINT32          AsDword;
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_DFP_SB_APDU_TYPE EDDI_DFP_SB_APDU_TYPE;

#include "pnio_pck_off.h"

//typedef struct _EDDI_DFP_APDU_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_DFP_APDU_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* PRM                                                                       */
/*---------------------------------------------------------------------------*/
//description of a subframe ("DG")
typedef struct _EDDI_PRM_SF_DSCR_TYPE
{
    struct _EDDI_PRM_SF_DSCR_TYPE *   pNext;          //ptr to next subframe descriptor, 0 for last element
    struct _EDDI_PRM_PF_DSCR_TYPE *   pPF;            //ptr to corresponding PF
        
    LSA_UINT8                         SFPosition;     //SFPosition
    LSA_UINT8                         Len;            //C_SDU Length
    LSA_BOOL                          UsedByUpper;    //LSA_TRUE: associated to DG (Cons or Prov)
    LSA_UINT16                        ConsProvID;     //ID of the associated provider or consumer
} EDDI_PRM_SF_DSCR_TYPE;

typedef struct _EDDI_PRM_SF_DSCR_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE;

//description of a packframe ("PF")
typedef struct _EDDI_PRM_PF_DSCR_TYPE
{
    struct _EDDI_PRM_PF_DSCR_TYPE *   pNext;          //ptr to next packframe descriptor, 0 for last element
    struct _EDDI_PRM_SF_DSCR_TYPE *   pSF;            //ptr to first corresponding Subframe
        
    EDDI_IRT_FRM_HANDLER_PTR_TYPE     pFrameHandler;  //ptr to framehandler
    LSA_UINT32                        SFProperties;   //SFProperties;
    LSA_UINT16                        FrameID;        //FrameID 
    LSA_UINT16                        ConsProvID;     //ID of the associated provider or consumer
    LSA_UINT8                         NrOfSubFrames;  //Nr of associated subframes
    LSA_UINT16                        FrameLen;       //Lenght of the complete frame WITHOUT APDU
    LSA_BOOL                          bIsProv;        //LSA_TRUE: Provider, LSA_FALSE: Consumer
    
} EDDI_PRM_PF_DSCR_TYPE;

typedef struct _EDDI_PRM_PF_DSCR_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Scoreboard                                                                */
/*---------------------------------------------------------------------------*/


/*===========================================================================*/
/*                                Protos                                     */
/*===========================================================================*/

//Common
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDFPDataInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPRelComponent(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  EDDI_LOCAL_FCT_ATTR   EDDI_DFPFindPackFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                       LSA_UINT16               const  FrameId);


LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPBuildAllPackFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                   EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE  const  pRecordSet );


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DFPPackFrameHandlerCleanUp( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE  const  pRecordSet );
             
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPCheckPackFrameAgainstIrtFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                               EDDI_DFP_PACKFRM_HANDLER_PTR_TYPE  const  pPackFrame,
                                                                               EDDI_IRT_FRM_HANDLER_PTR_TYPE      const  pFrmHandler );                                                                

EDDI_DFP_SUBFRM_HANDLER_PTR_TYPE  EDDI_LOCAL_FCT_ATTR EDDI_SyncIrFindSubFrameHandler( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                                      LSA_VOID           const *  const  pConsProv,
                                                                                      LSA_BOOL                    const  bIsProv);
                                                                                                                                                                                      
                                                                                                                                 
#endif //defined (EDDI_CFG_DFP_ON)
//Consumer

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT8                const  ListType,
                                                          LSA_UINT8                const  ConsumerType );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsPassPFFrame(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                       EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPConsRemovePFHandler(EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_CONST_CRT_CONSUMER_PTR_TYPE const  pConsumer);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerInit( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                       EDDI_CRT_CONSUMER_PTR_TYPE            const  pConsumer,
                                                       EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  const  pConsParam);

LSA_BOOL    EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerSetToUnknown(EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                               EDDI_CRT_CONSUMER_PTR_TYPE const  pConsSet2UK,
                                                               EDDI_CRT_CONS_HWWD         const  eHwWatchdog,
                                                               LSA_BOOL                   const  bTrigConsSBSM );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerPassivate(EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                            LSA_BOOL                    const  bClearEvents);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_CONST_CRT_CONSUMER_PTR_TYPE   const  pConsumer);

LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerActivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                          EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer);

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerSetEvents( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                         EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pPFConsumer,
                                                         EDDI_CRT_EVENT_TYPE               const  SetEvent);

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPConsumerIsPF( EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer);

LSA_BOOL EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderIsPF( EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider);

LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogTimerAlloc(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB);

LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogStart(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB);

LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogStop(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPWatchDogInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer);

#if !defined (EDDI_CFG_USE_SW_RPS)
LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPConsumerGetDS(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                      EDDI_CONST_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                      LSA_UINT8                       * const  pSFDataStatus,       
                                                      LSA_UINT16                      * const  pSFCycleCtr);
#endif //!defined (EDDI_CFG_USE_SW_RPS)

//Provider
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProvAddCheckRQB( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT8                const  ListType,
                                                          LSA_UINT8                const  ProviderType );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvPassPFFrame(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                       EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider);

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_DFPProvRemovePFHandler( EDD_UPPER_RQB_PTR_TYPE              const  pRQB,
                                                            EDDI_CONST_CRT_PROVIDER_PTR_TYPE    const  pProvider,
                                                            EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                            LSA_BOOL                         *  const  pIndicate );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderInit( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                       EDDI_CRT_PROVIDER_PTR_TYPE            const  pProvider,
                                                       EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE  const  pProvParam);
    
LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderPassivate(EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderCheckBufferProperties( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                                         EDDI_CONST_CRT_PROVIDER_PTR_TYPE     const  pProvider,
                                                                         EDDI_CRT_PROVIDER_PARAM_TYPE const * const  pLowerParamsLocal);
                                                                         
LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_DFPProviderActivate( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                          EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider);

LSA_VOID EDDI_DFPProviderSetActivity( EDDI_LOCAL_DDB_PTR_TYPE           const pDDB,
                                      EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const pProvider,
                                      LSA_BOOL                          const bActivate ); 

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTDFPProviderSetDataStatus( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                                 EDDI_CONST_CRT_PROVIDER_PTR_TYPE   const  pProvider,
                                                                 LSA_UINT8                          const  DGDataStatus );

LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_DFPProviderGetDS(EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                      EDDI_CONST_CRT_PROVIDER_PTR_TYPE  const  pProvider,
                                                      LSA_UINT8                       * const  pSFDataStatus);

LSA_VOID EDDI_DFPCheckFrameHandlerPF( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
                                      LSA_UINT16                     const  ProviderId,
                                      EDDI_IRT_FRM_HANDLER_PTR_TYPE      *  pFrmHandler);
                                          
#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_DFP_H


/*****************************************************************************/
/*  end of file eddi_crt_dfp.h                                               */
/*****************************************************************************/
