#ifndef PSI_INC_H               /* ----- reinclude-protection ----- */
#define PSI_INC_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_inc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main include file for PSI                                            */
/*                                                                           */
/*  Includes of psi and of the different systems, compilers or               */
/*  operating system.                                                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/* user specific include files */

#include "psi_cfg.h"      // Basic configuration of PSI framework

/*--- include the global system header ----------------------------------------*/

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

/*--- include pn-stack header ------------------------------------------*/

#include "pnio_version.h"  // PNIO

#include "acp_inc.h"

#include "clrpc_inc.h"

#include "cm_inc.h"

#include "dcp_inc.h"

#if (( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 ) || ( PSI_CFG_USE_EDDT == 1 ) || ( PSI_CFG_USE_PCSRT == 1 ))
#include "edd_inc.h"
#endif

#if ( PSI_CFG_USE_GSY == 1 )
#include "gsy_inc.h"
#endif

#if ( PSI_CFG_USE_HIF == 1 )
#include "hif_inc.h"
#endif

#if ( PSI_CFG_USE_IOH == 1 )
#include "ioh_inc.h"
#endif

#include "lldp_inc.h"

#if ( PSI_CFG_USE_MRP == 1 )
#include "mrp_inc.h"
#endif

#include "nare_inc.h"

#include "oha_inc.h"

#if ( PSI_CFG_USE_DNS == 1 )
#include "dns_inc.h"
#endif

#if ( PSI_CFG_USE_POF == 1 )
#include "pof_inc.h"
#endif

#include "snmpx_inc.h"

#include "sock_inc.h"

#if ( PSI_CFG_USE_TCIP == 1 )
#include "tcip_inc.h"
#endif

#if ( PSI_CFG_USE_HSA == 1 )
#include "hsa_inc.h"
#endif

#if ( PSI_CFG_USE_SOCKAPP == 1 )
#include "sockapp_inc.h"
#endif

#if ( PSI_CFG_USE_EPS_RQBS == 1 )
#include "eps_usr.h"
#endif

/*- include the PSI header --------------------------------------------------*/
#include "psi_usr.h"
#include "psi_sys.h"
#include "psi_pls.h"

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif   /* of PSI_INC_H */
