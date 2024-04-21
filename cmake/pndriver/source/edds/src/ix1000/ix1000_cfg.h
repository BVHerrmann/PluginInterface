#ifndef _IX1000_CFG_H                  /* ----- reinclude-protection ----- */
#define _IX1000_CFG_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: ix1000_cfg.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n: Global config file for IX1000 Low Layer.          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  10.10.2016  JB    Initial version.                                       */
#endif
/*****************************************************************************/
#define IX1000_PHY_INIT                         0x3900


#define IX1000_MIB2_SUPPORTED_COUNTERS_PORT ( EDD_MIB_SUPPORT_INOCTETS            \
                                            | EDD_MIB_SUPPORT_INUCASTPKTS         \
                                            | EDD_MIB_SUPPORT_INNUCASTPKTS        \
                                            | EDD_MIB_SUPPORT_INDISCARDS          \
                                            | EDD_MIB_SUPPORT_INERRORS            \
                                            | EDD_MIB_SUPPORT_OUTOCTETS           \
                                            | EDD_MIB_SUPPORT_OUTUCASTPKTS        \
                                            | EDD_MIB_SUPPORT_OUTNUCASTPKTS       \
                                            | EDD_MIB_SUPPORT_OUTDISCARDS         \
                                            | EDD_MIB_SUPPORT_OUTERRORS           \
                                            | EDD_MIB_SUPPORT_INMULTICASTPKTS     \
                                            | EDD_MIB_SUPPORT_INBROADCASTPKTS     \
                                            | EDD_MIB_SUPPORT_OUTMULTICASTPKTS    \
                                            | EDD_MIB_SUPPORT_OUTBROADCASTPKTS    \
                                            | EDD_MIB_SUPPORT_INHCOCTETS          \
                                            | EDD_MIB_SUPPORT_INHCUCASTPKTS       \
                                            | EDD_MIB_SUPPORT_INHCMULTICASTPKTS   \
                                            | EDD_MIB_SUPPORT_INHCBROADCASTPKTS   \
                                            | EDD_MIB_SUPPORT_OUTHCOCTETS         \
                                            | EDD_MIB_SUPPORT_OUTHCUCASTPKTS      \
                                            | EDD_MIB_SUPPORT_OUTHCMULTICASTPKTS  \
                                            | EDD_MIB_SUPPORT_OUTHCBROADCASTPKTS   )

#define IX1000_MIB2_SUPPORTED_COUNTERS_IF   ( EDD_MIB_SUPPORT_INOCTETS            \
                                            | EDD_MIB_SUPPORT_INUCASTPKTS         \
                                            | EDD_MIB_SUPPORT_INNUCASTPKTS        \
                                            | EDD_MIB_SUPPORT_OUTOCTETS           \
                                            | EDD_MIB_SUPPORT_OUTUCASTPKTS        \
                                            | EDD_MIB_SUPPORT_OUTNUCASTPKTS       \
                                            | EDD_MIB_SUPPORT_INMULTICASTPKTS     \
                                            | EDD_MIB_SUPPORT_INBROADCASTPKTS     \
                                            | EDD_MIB_SUPPORT_OUTMULTICASTPKTS    \
                                            | EDD_MIB_SUPPORT_OUTBROADCASTPKTS    \
                                            | EDD_MIB_SUPPORT_INHCOCTETS          \
                                            | EDD_MIB_SUPPORT_INHCUCASTPKTS       \
                                            | EDD_MIB_SUPPORT_INHCMULTICASTPKTS   \
                                            | EDD_MIB_SUPPORT_INHCBROADCASTPKTS   \
                                            | EDD_MIB_SUPPORT_OUTHCOCTETS         \
                                            | EDD_MIB_SUPPORT_OUTHCUCASTPKTS      \
                                            | EDD_MIB_SUPPORT_OUTHCMULTICASTPKTS  \
                                            | EDD_MIB_SUPPORT_OUTHCBROADCASTPKTS   )
                                            
#endif  /* _IX1000_CFG_H */
