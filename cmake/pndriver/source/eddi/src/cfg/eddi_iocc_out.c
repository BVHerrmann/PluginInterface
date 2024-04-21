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
/*  C o m p o n e n t     &C: EDDI IOCC                                 :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_iocc_out.c                           :F&  */
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
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/ 

#include "eddi_inc.h"

#if defined (EDDI_CFG_SOC)
#include "eddi_iocc_cfg.h"
#include "eddi_iocc.h"

#if defined __cplusplus     //If C++ - compiler: Use C linkage
extern "C"
{
#endif

#define UNREFERENCED_PARAMETER(P)	{(void)(P);}

/*=============================================================================*/
/*        LOCK-FUNCTIONS                                                       */
/*=============================================================================*/
/**************************************************************************/
/* Entry into a short protected area.                                     */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_ENTER_IOCC_CH1
LSA_VOID EDDI_ENTER_IOCC_CH1( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    UNREFERENCED_PARAMETER(UpperHandle);
}
#endif

/**************************************************************************/
/* Entry into a short protected area.                                     */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_ENTER_IOCC_CH2
LSA_VOID EDDI_ENTER_IOCC_CH2( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    UNREFERENCED_PARAMETER(UpperHandle);
}
#endif

/**************************************************************************/
/* Exit from short protected area.                                        */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_EXIT_IOCC_CH1
LSA_VOID EDDI_EXIT_IOCC_CH1( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    UNREFERENCED_PARAMETER(UpperHandle);
}
#endif
    
/**************************************************************************/
/* Exit from short protected area.                                        */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_EXIT_IOCC_CH2
LSA_VOID EDDI_EXIT_IOCC_CH2( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    UNREFERENCED_PARAMETER(UpperHandle);
}
#endif

/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/

/**************************************************************************/
/* Allocates local application memory.                                    */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_IOCC_ALLOC_LOCAL_MEM
LSA_VOID EDDI_IOCC_ALLOC_LOCAL_MEM( LSA_VOID                * * ppLocalMem,
                                    LSA_UINT32                  Length)
{
    UNREFERENCED_PARAMETER(ppLocalMem);
    UNREFERENCED_PARAMETER(Length);
}
#endif

/**************************************************************************/
/* Frees local application memory.                                        */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_IOCC_FREE_LOCAL_MEM
LSA_VOID EDDI_IOCC_FREE_LOCAL_MEM( LSA_UINT16                * pRetVal,
                                   LSA_VOID                  * pLocalMem)
{
    UNREFERENCED_PARAMETER(pRetVal);
    UNREFERENCED_PARAMETER(pLocalMem);
}
#endif

/**************************************************************************/
/* Allocates LinkList memory.                                             */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_IOCC_ALLOC_LINKLIST_MEM
LSA_VOID EDDI_IOCC_ALLOC_LINKLIST_MEM( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle,
                                       LSA_VOID                * * ppLocalMem,
                                       LSA_UINT32                  Length,
                                       LSA_UINT8                   LinkListType)
{
    UNREFERENCED_PARAMETER(UpperHandle);
    UNREFERENCED_PARAMETER(ppLocalMem);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LinkListType);
}
#endif

/**************************************************************************/
/* Frees LinkList memory.                                                 */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_IOCC_FREE_LINKLIST_MEM
LSA_VOID EDDI_IOCC_FREE_LINKLIST_MEM( EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle,
                                      LSA_UINT16                * pRetVal,
                                      LSA_VOID                  * pLocalMem,
                                      LSA_UINT8                   LinkListType)
{
    UNREFERENCED_PARAMETER(UpperHandle);
    UNREFERENCED_PARAMETER(pRetVal);
    UNREFERENCED_PARAMETER(pLocalMem);
    UNREFERENCED_PARAMETER(LinkListType);
}
#endif

/**************************************************************************/
/* Notify a fatal-error.                                                  */
/* For description see eddi_iocc_out.h                                    */
/**************************************************************************/
#ifndef EDDI_IOCC_FATAL_ERROR
LSA_VOID  EDDI_IOCC_FATAL_ERROR( LSA_UINT16                     Length,
                                 EDDI_IOCC_FATAL_ERROR_PTR_TYPE pError )
{
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(pError);
}
#endif    


#if defined __cplusplus
}
#endif

#endif //(EDDI_CFG_SOC)

/*****************************************************************************/
/*  end of file eddi_out.c                                                   */
/*****************************************************************************/

