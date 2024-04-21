
#ifndef OHA_INC_H                        /* ----- reinclude-protection ----- */
#define OHA_INC_H

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
/*  F i l e               &F: oha_inc.h                                 :F&  */
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
/*  Includes of OHA and of the different systems, compilers or               */
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
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/


/*-inlcude your global systsem header here ----------------------------------*/


/* the following LSA-headers or headers including these headers have to      */
/* be included here.                                                         */

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

/*---------------------------------------------------------------------------*/
#include "psi_cfg.h"
#include "oha_cfg.h"

/* interface to EDD -------------------------------------------------------- */
#include "edd_inc.h"

/*- include PSI Alloc header --------------------------------------------------*/
#include "psi_sys_mem.h"
#include "psi_inc_mem.h"

/* interface to NARE ------------------------------------------------------- */
#ifndef OHA_CFG_NO_NARE
#include "nare_inc.h"
#endif

/* interface to MRP -------------------------------------------------------- */
#ifndef OHA_CFG_NO_MRP
#include "mrp_inc.h"
#endif

/* interface to SOCK ------------------------------------------------------- */
#include "sock_inc.h"

/* interface to DNS -------------------------------------------------------- */
#ifdef OHA_CFG_USE_DNS
#include "dns_inc.h"
#endif

/* interface to DCP -------------------------------------------------------- */
#include "dcp_inc.h"

/* interface to LLDP ------------------------------------------------------- */
#include "lldp_inc.h"

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#include "oha_usr.h"

#include "oha_sys.h"
#include "oha_trc.h"

#if ((OHA_CFG_TRACE_MODE == 1) || (OHA_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"
#endif

#include "oha_low.h"


/*****************************************************************************/
/*  end of file OHA_INC.H                                                    */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/

#endif  /* of OHA_INC_H */

