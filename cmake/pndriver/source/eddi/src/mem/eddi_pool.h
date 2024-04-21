#ifndef EDDI_POOL_H             //reinclude-protection
#define EDDI_POOL_H

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
/*  F i l e               &F: eddi_pool.h                               :F&  */
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
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  15.11.07    JS    added EDDI_MEMCheckPoolFreeBuffer                      */
/*****************************************************************************/

#define EDDI_POOL_CHECK_PAT            0xF7F7F7F7UL
#define EDDI_POOL_CHECK_PAT_DISABLED   0xFEDCBA98UL


typedef LSA_VOID  ( EDDI_LOCAL_FCT_ATTR *EDDI_ALLOC_LOCAL_MEM_FCT )
(
    void                             **     p,
    LSA_UINT32                              length
);

typedef LSA_VOID  ( EDDI_LOCAL_FCT_ATTR *EDDI_FREE_LOCAL_MEM_FCT )
(
    LSA_UINT16                       *      ret_val_ptr,
    void                             *      p
);

typedef enum EDDI_POOL_TYPE_S
{
    EDDI_POOL_TYPE_KRAM_8BA  = 4,  //KRAM-buffer with 8-Byte-Alignment
    EDDI_POOL_TYPE_KRAM_4BA,       //KRAM-buffer with 4-Byte-Alignment
    EDDI_POOL_TYPE_LOCAL_MEM

} EDDI_POOL_TYPE_T;

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_MemIniPool( const  LSA_UINT32                    MaxBuffer,
                                                const  LSA_UINT32                    BufferSize,
                                                const  EDDI_POOL_TYPE_T              PoolType,
                                                EDDI_ALLOC_LOCAL_MEM_FCT       const alloc_local_mem_fct,
                                                EDDI_FREE_LOCAL_MEM_FCT        const free_local_mem_fct,
                                                const LSA_BOOL                       do_preset_buffer,
                                                const LSA_UINT8                      preset_value,
                                                EDDI_LOCAL_DDB_PTR_TYPE        const pDDB,
                                                LSA_UINT32                   * const pPoolHandle ); // OUT

void  EDDI_LOCAL_FCT_ATTR  EDDI_MemClosePool( LSA_UINT32       const                  PoolHandle ); //IN

void EDDI_LOCAL_FCT_ATTR  EDDI_MemGetPoolBuffer( const LSA_UINT32    PoolHandle, //IN
                                                 void                   **p    );  //OUT

void EDDI_LOCAL_FCT_ATTR  EDDI_MemFreePoolBuffer( const LSA_UINT32    PoolHandle,   //IN
                                                  void               * const p         ); //IN

LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_MemGetPoolBuffersize( const LSA_UINT32    PoolHandle); //IN

LSA_INT32 EDDI_LOCAL_FCT_ATTR  EDDI_MemPoolGetFreeBuffer( const LSA_UINT32    PoolHandle); //IN

void  EDDI_LOCAL_FCT_ATTR  EDDI_MemCheckPoolBuffer( const LSA_UINT32    PoolHandle,   //IN
                                                    void                * const p         );  //IN

LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_MemCheckPoolFreeBuffer( const  LSA_UINT32  PoolHandle); //IN

#if defined (EDDI_CFG_REV7)
//////////////////////////////////////////////////////////////////////////
/// Switch for @see EDDI_MemGetApduBuffer, to choose the usage of
/// the requested buffer.
//////////////////////////////////////////////////////////////////////////
typedef enum _EDDI_MEM_APDU_BUFFER_TYPE
{
    EDDI_MEM_APDU_PROVIDER_BUFFER,
    EDDI_MEM_APDU_CONSUMER_BUFFER

} EDDI_MEM_APDU_BUFFER_TYPE;

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_MemGetApduBuffer( LSA_UINT32                    const ProvConsId,
                                                      EDDI_LOCAL_DDB_PTR_TYPE       const pDDB,
                                                      EDDI_CRT_PAEA_APDU_STATUS * * const ppAPDU_Buffer,
                                                      EDDI_MEM_APDU_BUFFER_TYPE     const Mode );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_MemFreeApduBufferCons( LSA_UINT32              const ProvConsId,
                                                       EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_MemCalcApduBuffer( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_MemCloseApduBuffer( EDDI_LOCAL_DDB_PTR_TYPE          const pDDB,
                                                        EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE const pCRTComp );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_MemIniApduConsumerIndirectionLayer( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );
#endif //EDDI_CFG_REV7

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_POOL_H


/****************************************************************************/
/*  end of file eddi_pool.h                                                 */
/****************************************************************************/
