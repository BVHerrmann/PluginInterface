#ifndef EDDI_INC_H           //reinclude-protection
#define EDDI_INC_H

#ifdef __cplusplus           //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_inc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Include-File:                                                            */
/*  Includes of Edd and of the different systems, compilers or               */
/*  operating system.                                                        */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some includes depend on the different system, compiler or                */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*                                                                           */
/*****************************************************************************/

/* the following LSA-headers or headers including these headers have to      */
/* be included here.                                                         */

#include "lsa_cfg.h"
#include "lsa_usr.h"
#include "lsa_sys.h"

/*- include PSI Alloc header --------------------------------------------------*/
#include "psi_sys_mem.h"
#include "psi_inc_mem.h"

/*---------------------------------------------------------------------------*/

#include "edd_cfg.h"
#include "eddi_cfg.h"
#include "edd_usr.h"
#include "eddi_usr.h"

#if ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"     /* only if LTRC is used for trace */
#endif

#include "eddi_pls.h"

#include "eddi_sys.h"
#include "eddi_irte.h"

#include "eddi_trc.h"

/*-include your output-macro-header here ------------------------------------*/

#include "eddi_out.h"


#include "eddi_err.h"

/*---------------------------------------------------------------------------*/

#if defined __cplusplus
}
#endif

#endif //EDDI_INC_H


/*****************************************************************************/
/*  end of file eddi_inc.h/txt                                                */
/*****************************************************************************/
