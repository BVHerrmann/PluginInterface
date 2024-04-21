#ifndef EDDI_NRT_ARP_H          //reinclude-protection
#define EDDI_NRT_ARP_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_nrt_arp.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

LSA_VOID    EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitARPFilter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#if !defined (EDDI_CFG_DISABLE_ARP_FILTER)
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetARPFilter( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                       EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetARPFilterONOFF( EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE  const  pRqbParam,
                                                            EDDI_LOCAL_HDB_PTR_TYPE                const  pHDB,
                                                            LSA_BOOL                               const  bON);

#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCheckARPFilter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                         NRT_ARP_FILTER_PTR_TYPE  const  pARPFilter,
                                                         EDDI_ARP_FRAME_PTR_TYPE  const  pARPFrame );
#endif //EDDI_CFG_REV5 || EDDI_CFG_DEFRAG_ON
#endif //EDDI_CFG_DISABLE_ARP_FILTER

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_NRT_ARP_H


/*****************************************************************************/
/*  end of file eddi_nrt_arp.h                                               */
/*****************************************************************************/
