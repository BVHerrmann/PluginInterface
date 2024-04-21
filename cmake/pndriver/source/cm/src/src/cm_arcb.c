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
/*  F i l e               &F: cm_arcb.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  arcb things common for client and multicast                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID	11
#define CM_MODULE_ID		11

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_arcb_device_add_set_iocr_cnf (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	CM_UPPER_ARCB_ADD_CONST_PTR_TYPE  cb_ar
	)
{
	CM_UPPER_ARCB_IOCR_PTR_TYPE  cb_iocr;

	for (CmListForeach (cb_iocr, & cb_ar->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
	{
		if ( cm_is_not_null(cb_iocr) ) {
			CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr = cm_ar_graph_find_io_cr(argr, cb_iocr->iocr_ref);

			if ( cm_is_not_null(iocr) ) {
				cb_iocr->edd_apdustatus_offset = iocr->cm.edd_as_offset;
				cb_iocr->edd_cw_offset         = iocr->cm.edd_cw_offset;
				cb_iocr->edd_datastatus_offset = iocr->cm.edd_ds_offset;
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
#endif /* CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS */


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
