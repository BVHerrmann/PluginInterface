#ifndef EDDI_MEM_H              //reinclude-protection
#define EDDI_MEM_H

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
/*  F i l e               &F: eddi_mem.h                                :F&  */
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
/*                                                                           */
/*****************************************************************************/

#define EDDI_MEM_RET_OK                         (LSA_UINT32)0x01
#define EDDI_MEM_RET_INVALID_POINTER_ALIGN      (LSA_UINT32)0x02
#define EDDI_MEM_RET_ERR_NULL_PTR               (LSA_UINT32)0x03
#define EDDI_MEM_RET_INVALID_FIRST_BUF          (LSA_UINT32)0x06
#define EDDI_MEM_RET_NO_FREE_MEMORY             (LSA_UINT32)0x07
//#define EDDI_MEM_RET_SIZE_TOO_SMALL           (LSA_UINT32)0x08   // > 100
//#define EDDI_MEM_RET_DESC_CORRUPT             (LSA_UINT32)0x09
//#define EDDI_MEM_RET_BUF_CORRUPT              (LSA_UINT32)0x0a
//#define EDDI_MEM_RET_MEM_READ_ONLY            (LSA_UINT32)0x0b
#define EDDI_MEM_RET_MEM_LEN_NULL               (LSA_UINT32)0x0c
#define EDDI_MEM_RET_REENTER                    (LSA_UINT32)0x0d
#define EDDI_MEM_RET_MEM_CORRUPT_PREV_OVERWRITE (LSA_UINT32)0x0e
#define EDDI_MEM_RET_MEM_CORRUPT                (LSA_UINT32)0x0f
//#define EDDI_MEM_RET_ERR_FREE                 (LSA_UINT32)0x10
//#define EDDI_MEM_RET_ERR_BUFFER_SIZE          (LSA_UINT32)0x11
#define EDDI_MEM_RET_MEM_CORRUPT_PREV_POINTER   (LSA_UINT32)0x12
#define EDDI_MEM_RET_MEM_CORRUPT_NEXT_POINTER   (LSA_UINT32)0x13
//#define EDDI_MEM_RET_BUF_ALREADY_FREED          (LSA_UINT32)0x14
//#define EDDI_MEM_RET_INVALID_FIRST_BUF_CHECKSUM (LSA_UINT32)0x15
#define EDDI_MEM_RET_INVALID_HANDLE             (LSA_UINT32)0x16
#define EDDI_MEM_RET_INVALID_CHECK_PAT          (LSA_UINT32)0x17

#define EDDI_MEM_DO_PRESET       LSA_TRUE
#define EDDI_MEM_DO_NO_PRESET    LSA_FALSE

LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_MEMIni(const  LSA_UINT32                   MaxBuffer,
                                           const  LSA_UINT32                   Alignment,
                                           const  LSA_UINT32                   KramSize,
                                           LSA_UINT8                   * const pKRAM,
                                           const  LSA_BOOL                     do_preset_buffer,
                                           EDDI_LOCAL_DDB_PTR_TYPE        const pDDB,
                                           LSA_UINT32                        * MemHandle ); // OUT

void  EDDI_LOCAL_FCT_ATTR  EDDI_MEMClose( LSA_UINT32  const  MemHandle ); //IN

typedef struct _EDDI_MEM_BUF_EL_H
{
    LSA_UINT8   *  pKRam;
    LSA_UINT32     MemHandle;

} EDDI_MEM_BUF_EL_H;

LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMGetBuffer( const LSA_UINT32      MemHandle, //IN
                                                    EDDI_MEM_BUF_EL_H    **ppHeader,  //OUT
                                                    const LSA_UINT32      len );

LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMFreeBuffer( EDDI_MEM_BUF_EL_H    * const pHeader      ); //IN

//LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEM_check_buffer( EDDI_MEM_BUF_EL_H    * const pHeader   ); //IN

LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMTestBuffer( const LSA_UINT32      MemHandle, //IN
                                                     const LSA_UINT32      len );

typedef struct EDDI_MEM_INFO_S
{
    LSA_UINT32           MemHandle; // IN

    LSA_UINT32           free_mem;
    LSA_UINT32           used_mem;
    LSA_UINT32           count_allocated_buffer;
    LSA_UINT32           max_free_buffer_size;

} EDDI_MEM_INFO_T;

LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_MEMInfo( EDDI_MEM_INFO_T * const info);

typedef struct EDDI_MEM_VERW_H_S
{
    LSA_UINT32                     CheckPattern;
    LSA_BOOL                       do_preset_buffer;
    LSA_UINT32                     KramSize;
    LSA_UINT32                     MaxBuffer;
    LSA_UINT32                     hPool;
    LSA_UINT8                   *  pKRAM;
    LSA_UINT32                     check_reenter;
    LSA_UINT32                     Alignment;
    LSA_UINT32                     AligMask;

    struct _EDDI_MEM_BUFFER_EL  *  pFirst_el; //fix
    struct _EDDI_MEM_BUFFER_EL  *  pAct_el;

    LSA_UINT32                     used_mem;
    LSA_UINT32                     count_allocated_buffer;

    LSA_UINT32                     MemHandle;

    LSA_UINT8                      preset_value;

} EDDI_MEM_VERW_H_T;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_MEM_H


/****************************************************************************/
/*  end of file eddi_mem.h                                                  */
/****************************************************************************/
