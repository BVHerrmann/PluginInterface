
#ifndef GSY_INC_H                        /* ----- reinclude-protection ----- */
#define GSY_INC_H

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
/*  F i l e               &F: gsy_inc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                      Frame for file "gsy_inc.h".                          */
/*                      ==============================                       */
/*                                                                           */
/*  Include-File:                                                            */
/*  Includes of prefix and of the different systems, compilers or            */
/*  operating system.                                                        */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some includes depend on the different system, compiler or                */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-11-19  P04.01.00.00_00.02.02.02 lrg: k32_cfg.h                      */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2009-04-23  P04.01.00.00_00.03.04.02 lrg: eddp_k32_cfg.h                 */
/*  2010-07-20  P05.00.00.00_00.01.02.02 lrg: AP00999110 GSY_CFG_TRACE_MODE  */
/*              Always include gsy_trc.h                                     */
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01069493 GSY_REQUEST_UPPER_DONE   */
/*              moved from gsy_usr.h to gsy_sys.h                            */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */
 
/* general system definitions */

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

/*- lower layer header --------------------------------------------------------*/

#include "edd_inc.h"

/*- include PSI Alloc header --------------------------------------------------*/
#include "psi_sys_mem.h"
#include "psi_inc_mem.h"

#include "gsy_cfg.h"

#if ((GSY_CFG_TRACE_MODE == 1) || (GSY_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"     /* only if LTRC is used for trace */
#endif

#include "gsy_usr.h"
#include "gsy_sys.h"
#include "gsy_trc.h"
#include "gsy_low.h"
#include "gsy_pls.h"

/*****************************************************************************/
/*  end of file GSY_INC.H                                                    */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_INC_H */

