#ifndef SRC_EPS_STATISTICS_STACK_H_
#define SRC_EPS_STATISTICS_STACK_H_

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_statistics_stack.h                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS internal Statistics API for stack usage.                             */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
#if ( EPS_CFG_USE_STATISTICS == 1 )
LSA_VOID   eps_statistics_stack_init                       ( LSA_VOID );
LSA_VOID   eps_statistics_stack_undo_init                  ( LSA_VOID );

LSA_UINT32 eps_statistics_stack_register                   ( const LSA_CHAR * pName, LSA_UINT32 stackSize, LSA_UINT8* startAddress );
LSA_UINT32 eps_statistics_stack_undo_register              ( LSA_CHAR * pName, LSA_UINT8* startAddress );

LSA_VOID   eps_statistics_stack_cyclic_start               ( LSA_UINT32 cycle_time_ms );
LSA_VOID   eps_statistics_stack_cyclic_stop                ( LSA_VOID );

// Exported functions, surpress lint warnings for "not referenced", "could be declared static", "could be removed from header to module"

//lint -esym(714, eps_statistics_stack_init)
//lint -esym(759, eps_statistics_stack_init)
//lint -esym(765, eps_statistics_stack_init)

//lint -esym(714, eps_statistics_stack_undo_init)
//lint -esym(759, eps_statistics_stack_undo_init)
//lint -esym(765, eps_statistics_stack_undo_init)

//lint -esym(714, eps_statistics_stack_register)
//lint -esym(759, eps_statistics_stack_register)
//lint -esym(765, eps_statistics_stack_register)

//lint -esym(714, eps_statistics_stack_undo_register)
//lint -esym(759, eps_statistics_stack_undo_register)
//lint -esym(765, eps_statistics_stack_undo_register)

//lint -esym(714, eps_statistics_stack_cyclic_start)
//lint -esym(759, eps_statistics_stack_cyclic_start)
//lint -esym(765, eps_statistics_stack_cyclic_start)

//lint -esym(714, eps_statistics_stack_cyclic_stop)
//lint -esym(759, eps_statistics_stack_cyclic_stop)
//lint -esym(765, eps_statistics_stack_cyclic_stop)
#endif
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* SRC_EPS_STATISTICS_STACK_H_ */
