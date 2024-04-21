
#ifndef LLDP_INC_H                     /* ----- reinclude-protection ----- */
#define LLDP_INC_H

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
/*  F i l e               &F: lldp_inc.h                                :F&  */
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
/*  Includes of LLDP and of the different systems, compilers or              */
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
#ifdef LLDP_MESSAGE
/*  05.04.04    JS    initial version.                                       */
#endif
/*****************************************************************************/


/*-inlcude your global systsem header here ----------------------------------*/

/* the following LSA-headers or headers including these headers have to      */
/* be included here.                                                         */

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

/*---------------------------------------------------------------------------*/

/* interface to EDD -------------------------------------------------------- */
#include "edd_inc.h"

/*- include PSI Alloc header --------------------------------------------------*/
#include "psi_sys_mem.h"
#include "psi_inc_mem.h"

/*---------------------------------------------------------------------------*/

#include "lldp_cfg.h"
#include "lldp_usr.h"
#include "lldp_sys.h"

#if ((LLDP_CFG_TRACE_MODE == 1) || (LLDP_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"
#endif

#include "lldp_low.h"


/*****************************************************************************/
/*  end of file LLDP_INC.H                                                    */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/

#endif  /* of LLDP_INC_H */
