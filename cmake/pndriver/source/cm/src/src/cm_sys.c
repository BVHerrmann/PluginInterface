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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_sys.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the system LSA interface                                      */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  1
#define CM_MODULE_ID       1

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/

/*lint -e(708) Info 708: union initialization */
const LSA_USER_ID_TYPE cm_null_user_id = { 0 }; /* handy to have */
/*
  see ansi-c:
	A brace-enclosed initializer for a union object initializes the
	member that appears first in the declaration list of the union type.
  ad ms-vs2010:
	c99 feature "designator" doesn't work:
	const LSA_USER_ID_TYPE cm_null_user_id = { .uvar32 = 0 }; // compile error
*/


/*------------------------------------------------------------------------------
//	func: cm_init
//	work: initialises the module CM, must be called on startup
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_init (
	CM_INIT_CONST_PTR_TYPE init
) {
	CM_UPPER_TRACE_02 (0, LSA_TRACE_LEVEL_NOTE
		, "oneshot_lsa_time_base(%u) oneshot_lsa_time_factor(%u)"
		, init->oneshot_lsa_time_base, init->oneshot_lsa_time_factor
		);

	/***/

	{
	LSA_INT must;

	/*
	 * LSA_FALSE must be 0 or many things will go wrong...
	 * and LSA_TRUE must not be LSA_FALSE...
	 */

#if 0 /* show #define of LSA_TRUE / LSA_FALSE */
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("LSA_FALSE is defined as '" CM_STRINGIFY(LSA_FALSE) "'")
#pragma CM_MESSAGE("LSA_TRUE is defined as '" CM_STRINGIFY(LSA_TRUE) "'")
#endif /* CM_MESSAGE */
#endif

	must = ( LSA_FALSE == 0 && LSA_TRUE != LSA_FALSE );

	if( ! must ) {
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL
			, "strange TRUE/FALSE"
			);
		CM_FATAL();
	}
	}

	/*
	 * but: LSA_NULL needs not to be 0. some may set it to -1 or some other special value!
	 *
	 * don't: if( LSA_NULL == 0 ) ...
	 */

#if 0 /* show #define of LSA_NULL */
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("LSA_NULL is defined as '" CM_STRINGIFY(LSA_NULL) "'")
#endif /* CM_MESSAGE */
#endif

	/*
	 * the RQB must start with the two pointers or many things will go wrong...
	 */

	{/*lint -save -esym(550,u) Symbol 'u' not accessed*/
		union {
			CM_RQB_TYPE x;
			CM_LIST_ENTRY_TYPE y;
		} u;

		if ((void*)&CM_RQB_GET_NEXT_RQB_PTR(&u.x) != (void*)&u.y.Flink )
		{
			CM_FATAL();
		}

		if ((void*)&CM_RQB_GET_PREV_RQB_PTR(&u.x) != (void*)&u.y.Blink )
		{
			CM_FATAL();
		}
	}
	/*lint -restore */


	/*
	 * init all global vars
	 */

	cm_oneshot_sys_init(init->oneshot_lsa_time_base, init->oneshot_lsa_time_factor);

	/***/

	cm_channels_sys_init();

	/***/

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
//	func: cm_undo_init
//	work: de-initialises the module CM, must be call on shutdown
//----------------------------------------------------------------------------*/
LSA_UINT16
cm_undo_init(
	LSA_VOID
) {

	cm_channels_sys_undo_init();

	/***/

	cm_oneshot_sys_undo_init();

	/***/

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
//	func: cm_fatal
//----------------------------------------------------------------------------*/

static  LSA_FATAL_ERROR_TYPE cm_fatal_var; /* for some debuggers that have problems with the stack-trace */

LSA_VOID
cm_fatal(
	LSA_UINT16 cm_module_id,
	LSA_INT line
) {
	cm_fatal1(cm_module_id, line, 0);
}


LSA_VOID
cm_fatal1(
	LSA_UINT16 cm_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
) {
	CM_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "module_id(%u) line(%u) ec_0(0x%x)"
		, cm_module_id, line, ec_0
		);

	cm_fatal_var.lsa_component_id = LSA_COMP_ID_CM;
	cm_fatal_var.module_id        = cm_module_id;
	cm_fatal_var.line             = (LSA_UINT16)line;

	cm_fatal_var.error_code[0]    = ec_0;
	cm_fatal_var.error_code[1]    = 0;
	cm_fatal_var.error_code[2]    = 0;
	cm_fatal_var.error_code[3]    = 0;

	cm_fatal_var.error_data_length = 0;
	cm_fatal_var.error_data_ptr    = LSA_NULL;

	CM_FATAL_ERROR(sizeof(cm_fatal_var), &cm_fatal_var);
	/* not reached */
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
