#ifndef TCIP_INICHE_CFG_H                       /* ----- reinclude-protection ----- */
#define TCIP_INICHE_CFG_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: pcIOX (PNIO Controler, Device and more)   :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_iniche_cfg.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration module                                                     */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*=== pcIOX configuration (do not copy/paste it) ============================*/
/*===========================================================================*/

/* maximum mumber of ARP table entries, at least 32 */
#define TCIP_CFG_MAXARPS        LSA_MAX(((PSI_CFG_MAX_DIAG_TOOLS) + (((PSI_CFG_MAX_CL_DEVICES) + ((PSI_CFG_MAX_SV_DEVICES) * (PSI_CFG_MAX_SV_IOC_ARS))) * (PSI_CFG_MAX_IF_CNT))),32)

#define TCIP_CFG_SYS_SERVICES   (PSI_CFG_TCIP_CFG_SYS_SERVICES)
#define TCIP_CFG_SOMAXCONN      (PSI_CFG_TCIP_CFG_SOMAXCONN)

#define TCIP_CFG_NUMBIGBUFS     250     /* number of packets of size BIGBUFSIZE */
#define TCIP_CFG_NUMLILBUFS     150     /* number of packets of size LILBUFSIZE */

#define TCIP_CFG_COMPRESS_RCV_MBUFS     0   /* off, see sockbuf define SB_MBCOMP */

#define TCIP_CFG_NPDEBUG        0       /* off */

void  tcp_sleep(void * timeout);
void  tcp_wakeup(void * wake);

void LOCK_NET_RESOURCE(int res);
void UNLOCK_NET_RESOURCE(int res);

//lint -esym(714, tcp_sleep) not referenced                     - it is used by TCIP_INTERNICHE
//lint -esym(714, tcp_wakeup) not referenced                    - it is used by TCIP_INTERNICHE
//lint -esym(714, LOCK_NET_RESOURCE) not referenced             - it is used by TCIP_INTERNICHE
//lint -esym(714, UNLOCK_NET_RESOURCE) not referenced           - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_COMPRESS_RCV_MBUFS) not referenced   - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_MAXARPS) not referenced              - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_NPDEBUG) not referenced              - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_NUMBIGBUFS) not referenced           - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_NUMLILBUFS) not referenced           - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_SYS_SERVICES) not referenced         - it is used by TCIP_INTERNICHE
//lint -esym(755, TCIP_CFG_SOMAXCONN) not referenced            - it is used by TCIP_INTERNICHE

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_CFG_H (reinclude protection) */
