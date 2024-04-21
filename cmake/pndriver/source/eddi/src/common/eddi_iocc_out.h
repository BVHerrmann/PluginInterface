#ifndef EDDI_IOCC_OUT_H              //reinclude-protection
#define EDDI_IOCC_OUT_H

#if defined (__cplusplus)              //If C++ - compiler: Use C linkage
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
/*  F i l e               &F: eddi_iocc_out.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  EDDI IOCC output macros framework. Have to be filled by systemadaption.  */
/*  This file declares prototypes with no function if macro is not defined.  */
/*                                                                           */
/*****************************************************************************/
/** @file eddi_iocc_out.h "EDDI IOCC output macros framework"  **/
/**
@defgroup eddi_iocc_out EDDI IOCC output macros
@ingroup eddi-iocc-group
@{ 
**/

/*=============================================================================*/
/*        LOCK-FUNCTIONS                                                       */
/*=============================================================================*/
/**************************************************************************//**
@brief Entry into a short protected area.
@details This call identifies the start of a protected area. It is used by eddi_IOCC in the context of register access to IOCC channel 1.
@param [in]  UpperHandle Upper handle provided by the application during @ref eddi_IOCC_Setup
@return -
******************************************************************************/

#ifndef EDDI_ENTER_IOCC_CH1
LSA_VOID EDDI_ENTER_IOCC_CH1( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle);
#endif

/**************************************************************************//**
@brief Entry into a short protected area.
@details This call identifies the start of a protected area. It is used by eddi_IOCC in the context of register access to IOCC channel 2.
@param [in]  UpperHandle Upper handle provided by the application during @ref eddi_IOCC_Setup
@return -
******************************************************************************/
#ifndef EDDI_ENTER_IOCC_CH2
LSA_VOID EDDI_ENTER_IOCC_CH2( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle);
#endif

/**************************************************************************//**
@brief Exit from short protected area.
@details After calling this function, the protected area started with EDDI_ENTER_IOCC_CH1 is exited. This means that program sections can be executed simultaneously again.
@param [in]  UpperHandle Upper handle provided by the application during @ref eddi_IOCC_Setup
@return -
******************************************************************************/
#ifndef EDDI_EXIT_IOCC_CH1
LSA_VOID EDDI_EXIT_IOCC_CH1( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle);
#endif
    
/**************************************************************************//**
@brief Exit from short protected area.
@details After calling this function, the protected area started with EDDI_ENTER_IOCC_CH2 is exited. This means that program sections can be executed simultaneously again.
@param [in]  UpperHandle Upper handle provided by the application during @ref eddi_IOCC_Setup
@return -
******************************************************************************/
#ifndef EDDI_EXIT_IOCC_CH2
LSA_VOID EDDI_EXIT_IOCC_CH2( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle);
#endif

/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/
/**************************************************************************//**
@brief Allocates local application memory.
@details The function allocates "Length" bytes of memory and returns a pointer to this area. The memory area must be correctly aligned to the natural alignment of the processor and compiler used (E.g. for a 32bit compile with possible 64bit access a 64bit alignment is necessary!). \n
If a NULL pointer is returned, no memory could be allocated. With this function, the eddi_IOCC allocates internal management structures.
@param [inout]  ppLocalMem  Pointer to address for pointer to memory area. If NULL is returned, no memory could be allocated.
@param [in]     Length      Length of the memory area to be allocated in bytes.
@return -
******************************************************************************/
#ifndef EDDI_IOCC_ALLOC_LOCAL_MEM
LSA_VOID EDDI_IOCC_ALLOC_LOCAL_MEM( LSA_VOID                * * ppLocalMem,
                                    LSA_UINT32                  Length);
#endif

/**************************************************************************//**
@brief Frees local application memory.
@details The function releases memory allocated earlier with EDDI_IOCC_ALLOC_LOCAL_MEM. pLocalMem is the pointer returned with EDDI_IOCC_ALLOC_LOCAL_MEM.
@param [inout]  pRetVal     Return value.
@param [in]     pLocalMem   Pointer to the memory area to be released.
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM on failure 
******************************************************************************/
#ifndef EDDI_IOCC_FREE_LOCAL_MEM
LSA_VOID EDDI_IOCC_FREE_LOCAL_MEM( LSA_UINT16                * pRetVal,
                                   LSA_VOID                  * pLocalMem);
#endif

/**************************************************************************//**
@brief Allocates LinkList memory.
@details The function allocates "Length" bytes of linklist memory and returns a pointer to this area. **The memory area must be 4 Byte aligned** . \n
If a NULL pointer is returned, no memory could be allocated. With this function, the eddi_IOCC allocates LinkLists.
@param [in]     UpperHandle  Upper handle provided by the application during @ref eddi_IOCC_Setup
@param [inout]  ppLocalMem   Pointer to address for pointer to memory area. If NULL is returned, no memory could be allocated.
@param [in]     Length       Length of the memory area to be allocated in bytes.
@param [in]     LinkListType Memory type of LinkList:
|                                 |                               |
|---------------------------------|--------------------------------|
|EDDI_IOCC_LINKLIST_TYPE_INTERNAL | Memory is requested from IOCC internal LinkList memory. 
|EDDI_IOCC_LINKLIST_TYPE_EXTERNAL | Memory is requested from external memory (SOC1 DDR2 or host memory).
@return -
******************************************************************************/
#ifndef EDDI_IOCC_ALLOC_LINKLIST_MEM
LSA_VOID EDDI_IOCC_ALLOC_LINKLIST_MEM( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle,
                                       LSA_VOID                * * ppLocalMem,
                                       LSA_UINT32                  Length,
                                       LSA_UINT8                   LinkListType);
#endif

/**************************************************************************//**
@brief Frees LinkList memory.
@details The function releases memory allocated earlier with EDDI_IOCC_ALLOC_LINKLIST_MEM. pLocalMem is the pointer returned with EDDI_IOCC_ALLOC_LINKLIST_MEM.
@param [in]     UpperHandle Upper handle provided by the application during @ref eddi_IOCC_Setup
@param [inout]  pRetVal     Return value.
@param [in]     pLocalMem   Pointer to the memory area to be released.
@param [in]     LinkListType Memory type of LinkList:
|                                 |                               |
|---------------------------------|--------------------------------|
|EDDI_IOCC_LINKLIST_TYPE_INTERNAL | Memory is requested from IOCC internal LinkList memory. 
|EDDI_IOCC_LINKLIST_TYPE_EXTERNAL | Memory is requested from external memory (SOC1 DDR2 or host memory).
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM on failure 
******************************************************************************/
#ifndef EDDI_IOCC_FREE_LINKLIST_MEM
LSA_VOID EDDI_IOCC_FREE_LINKLIST_MEM( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle,
                                      LSA_UINT16                * pRetVal,
                                      LSA_VOID                  * pLocalMem,
                                      LSA_UINT8                   LinkListType);
#endif

/**************************************************************************//**
@brief Notify a fatal-error.
@param [in]     Length   Number of bytes.
@param [in]     pError   Pointer to LSA_FATAL_ERROR.
@return - 
******************************************************************************/
#ifndef EDDI_IOCC_FATAL_ERROR
LSA_VOID  EDDI_IOCC_FATAL_ERROR( LSA_UINT16                     Length,
                                 EDDI_IOCC_FATAL_ERROR_PTR_TYPE pError );
#endif    
    
/** @} **/ //end of group eddi_iocc_out 

#if defined(__cplusplus) //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IOCC_OUT_H


/*****************************************************************************/
/*  end of file eddi_iocc_out.h                                              */
/*****************************************************************************/
