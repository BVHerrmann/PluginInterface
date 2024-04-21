#ifndef HIF_INC_H
#define HIF_INC_H
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
/*  F i l e               &F: hif_inc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main include file for HIF                                            */
/*                                                                           */
/*  Includes of hif and of the different systems, compilers or               */
/*  operating system.                                                        */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some includes depend on the different system, compiler or                */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/

/*- include the global system header ----------------------------------------*/

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

#include "psi_inc.h" /* PSI is always used with HIF, when THIS hif_inc.h is used */

/*- include PSI Alloc header --------------------------------------------------*/
#include "psi_sys_mem.h"
#include "psi_inc_mem.h"

/* For the EDDx Alloc Macros  */
#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
#include "edd_inc.h"
#if (PSI_CFG_USE_EDDT == 1)
#include "eddt_sys.h"
#else
#define EDDT_DATABUFFER_RXHEADER_SIZE 0     // Needed in AllocMacro
#endif
#endif

#include "hif_cfg.h"

/*- include the lower-layer header ------------------------------------------*/

#include "pnio_version.h"

#if (HIF_CFG_USE_EDD == 1)
#include "edd_inc.h"
#endif

#if (HIF_CFG_USE_OHA == 1)
#include "oha_inc.h"
#endif

#if (HIF_CFG_USE_DNS == 1)
#include "dns_inc.h"
#endif

#if (HIF_CFG_USE_CLRPC == 1)
#include "clrpc_inc.h"
#endif

#if (HIF_CFG_USE_CM == 1)
#include "cm_inc.h"
#endif

#if (HIF_CFG_USE_SOCK == 1)
#include "sock_inc.h"
#endif

#if (HIF_CFG_USE_SNMPX == 1)
#include "snmpx_inc.h"
#endif

#if (HIF_CFG_USE_LLDP == 1)
#include "lldp_inc.h"
#endif

#if (HIF_CFG_USE_MRP == 1)
#include "mrp_inc.h"
#endif

#if (HIF_CFG_USE_DCP == 1)
#include "dcp_inc.h"
#endif

#if (HIF_CFG_USE_NARE == 1)
#include "nare_inc.h"
#endif

#if (HIF_CFG_USE_GSY == 1)
#include "gsy_inc.h"
#endif

#if (HIF_CFG_USE_ACP == 1)
#include "acp_inc.h"
#endif

#if (HIF_CFG_USE_IOH == 1)
#include "ioh_inc.h"
#endif

#if (HIF_CFG_USE_HSA == 1)
#include "hsa_inc.h"
#endif

/*- include the HIF header --------------------------------------------------*/



#include "hif_usr.h"
#include "hif_sys.h"
#include "hif_pls.h"


/*- include the trace header ------------------------------------------------*/

#if HIF_CFG_TRACE_MODE != 0
#include "ltrc_if.h"
#endif


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of HIF_INC_H */
