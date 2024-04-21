#ifndef EDDS_IOBUFFER_USR_H___           /* ----- reinclude-protection ----- */
#define EDDS_IOBUFFER_USR_H___

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for StdMac)    :C&  */
/*                                                                           */
/*  F i l e               &F: edds_iobuf_usr.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IO Buffer - API                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  01.07.12    TP    initial version.                                       */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

  
/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef LSA_VOID_PTR_TYPE   EDDS_IOBUFFER_HANDLE_TYPE;


/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/

/*===========================================================================*/
/*                            function prototypes                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_IOBuffer_Init
 *
 * function:       Initialize io buffer module.
 *
 * parameters:     pIOBufferManagement   management info block from srt init
 *                 MngmId                management id for IOBuffer instance
 *                 pConsumerCRCnt        amount of consumer
 *                 pProviderCRCnt        amount of provider
 *
 * return value:   EDDS_IOBUFFER_HANDLE_TYPE as handle for initialized 
 *                 io buffer module.
 *===========================================================================*/
EDDS_IOBUFFER_HANDLE_TYPE EDDS_IOBuffer_Init ( 
    EDD_COMMON_MEM_PTR_TYPE      hIOBufferManagement,   // [IN]
    LSA_UINT16                   MngmId,                // [IN]
    EDD_COMMON_MEM_U16_PTR_TYPE  pConsumerCRCnt,        // [OUT]
    EDD_COMMON_MEM_U16_PTR_TYPE  pProviderCRCnt         // [OUT]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_Deinit
 *
 * function:       Deinitialize IO buffer module.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_Deinit ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer   // [IN]
);

/*========================================================================================================================
 * function name:  EDDS_IOBuffer_ProviderAlloc
 *
 * function:       Allocates an io buffer provider cr.
 *
 * parameters:     hIOBuffer            user io buffer handle from EDDS_IOBuffer_Init
 *                 DataLen              length of IO data
 *                 ProviderType         type of provider: xRT or UDP 
 *                                         EDDS_CSRT_PROVIDER_TYPE_XRT for XRT provider
 *                                         EDDS_CSRT_PROVIDER_TYPE_UDP for UDP provider
 *                                         
 *                 IsSystemRedundant    determine, if the provider is system redundant                                
 *                                         EDD_PROVADD_TYPE_DEFAULT for no system redundancy provider  
 *                                         EDD_PROVADD_TYPE_SYSRED  for sytem redundancy provider         
 *                                       
 * return value:   cr number as reference to provider cr
 *========================================================================================================================*/
LSA_UINT16 EDDS_IOBuffer_ProviderAlloc ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,             // [IN]
    LSA_UINT16                 DataLen,               // [IN]
    LSA_UINT16                 ProviderType,          // [IN]
    LSA_UINT8                  IsSystemRedundant      // [IN] 
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderFree
 *
 * function:       Frees an io buffer provider cr.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    cr number as reference to cr to be freed
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ProviderFree ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);


/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderLockCurrent
 *
 * function:       Lock the current provider buffer for use by user.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    provider cr number
 *
 * return value:   EDD_UPPER_MEM_U8_PTR_TYPE as pointer to I/O buffer
 *===========================================================================*/
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ProviderLockCurrent ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderLockNew
 *
 * function:       Lock a provider buffer for use by user. Content undefined.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    Provider CRNumber
 *
 * return value:   EDD_UPPER_MEM_U8_PTR_TYPE as pointer to I/O buffer
 *===========================================================================*/
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ProviderLockNew ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderUnlock
 *
 * function:       Unlock a previous locked provider buffer.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    provider cr number
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ProviderUnlock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=======================================================================================
 * function name:  EDDS_IOBuffer_ConsumerAlloc
 *
 * function:       Allocates a io buffer consumer cr.
 *
 * parameters:     hIOBuffer           user io buffer handle from EDDS_IOBuffer_Init
 *                 DataLen             length of IO data
 *                 ConsumerType        type of consumer: xRT or UDP
 *                 pDataStatusOffset  A pointer where the Offset of the DataStatus 
 *                                     will be stored
 *
 * return value:   cr number as reference to consumer cr
 *=======================================================================================*/
LSA_UINT16 EDDS_IOBuffer_ConsumerAlloc ( 
    EDDS_IOBUFFER_HANDLE_TYPE          hIOBuffer,          // [IN]
    LSA_UINT16                         DataLen,            // [IN]
    LSA_UINT16                         ConsumerType,       // [IN]
    EDD_UPPER_MEM_U16_PTR_TYPE const   pDataStatusOffset  // [IN/OUT]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ConsumerFree
 *
 * function:       Frees a io buffer consumer cr.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    cr number as reference to cr to be freed
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ConsumerFree ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=======================================================================================
 * function name:  EDDS_IOBuffer_ConsumerLock
 *
 * function:       Lock the current consumer buffer for use by user.
 *
 * parameters:     hIOBuffer             user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber              consumer cr number
 *                                       
 * return value:   EDD_UPPER_MEM_U8_PTR_TYPE as pointer to I/O buffer
 *=======================================================================================*/
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ConsumerLock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,          // [IN]
    LSA_UINT16                 CRNumber           // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ConsumerUnlock
 *
 * function:       Unlock a previous locked consumer buffer.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    consumer cr number
 *
 * return value:   LSA_VOID
 *=============================================================================*/
LSA_VOID EDDS_IOBuffer_ConsumerUnlock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

#ifdef EDDS_CFG_SYSRED_API_SUPPORT
/*=======================================================================================================================================
 * function name:  EDDS_IOBuffer_ProviderDataStatus_Set
 *
 * function:       Change System Redundancy DataStatus Bits: State and Redundancy.
 *
 * parameters:     hIOBuffer               user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber                provider cr number
 *                 Status                  Data status value of STATE und REDUNDANCY Bits.
 *      
 *                                           EDD_CSRT_DSTAT_BIT_STATE  (Bit 0)
 *                                           EDD_CSRT_DSTAT_BIT_REDUNDANCY (Bit 1)  ((both defines can be Ored).
 *                                           
 *                                           This specifies the new value of a bit. 
 *                                           Only the bits selected by the mask are set to a new value.
 *                                           If the define is used, the bit is set "1", otherwise to "0".
 *                                           only the bits STATE and REDUNDANCY can be changed by this function.
 *                                           Meaning of the bit value:
 *                                                                     REDUNDANCY: 0: Not redundant  1: redundant
 *                                                                     STATE:    0: BACKUP     1: PRIMARY
 *                  Mask                   Mask for status bits that must be changed (can be Ored).
 *
 *                                            EDD_CSRT_DSTAT_BIT_STATE  (Bit 0)
 *                                            EDD_CSRT_DSTAT_BIT_REDUNDANCY (Bit 1)
 *
 *                                           This defines a mask that specifies which of the 8 bits in data status will be changed.
 *                                           If the define is used, the bit is changed according to the value in                               
 *                    
 *  return value:    LSA_RESULT:
 *                                EDD_STS_OK
 *                                EDD_STS_ERR_PARAM 
 *                                EDD_STS_ERR_NOT_ALLOWED
 *                                EDD_STS_ERR_SEQUENCE
 *                                                                                                                                            
 *=======================================================================================================================================*/
LSA_RESULT EDDS_IOBuffer_ProviderDataStatus_Set ( 
    EDDS_IOBUFFER_HANDLE_TYPE  const hIOBuffer,                       // [IN]
    LSA_UINT16                 const CRNumber,                        // [IN]
    LSA_UINT8                  const Status,                          // [IN]
    LSA_UINT8                  const Mask                             // [IN]

);

/*============================================================================================================
 * function name:  EDDS_IOBuffer_PrimaryAR_Set
 *
 * function:       Set the primary AR
 *
 * parameters:     hIOBuffer      user io buffer handle from EDDS_IOBuffer_Init
 *                 ARSetID        ID of the ARSet from which the current AR in state PRIMARY (= PrimaryARID)
 *                                shall be set. Parameter only used with system redundancy within an IO Device.
 *                                Range:
                                      1... EDD_CFG_MAX_NR_ARSETS
 *
 *                 PrimaryARID    ARID of the ARSet (referenced by ARSetID) being the current primary AR.
 *                 

 * return value:   LSA_RESULT
 *=============================================================================================================*/
LSA_RESULT EDDS_IOBuffer_PrimaryAR_Set(
    EDDS_IOBUFFER_HANDLE_TYPE  const    hIOBuffer, 
    LSA_UINT16                 const    ARSetID,
    LSA_UINT16                 const    PrimaryARID
);

#endif

/*****************************************************************************/
/*  end of file edds_iobuf_usr.h                                             */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_IOBUFFER_USR_H___ */
