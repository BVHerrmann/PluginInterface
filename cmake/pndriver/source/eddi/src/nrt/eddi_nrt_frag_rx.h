#ifndef EDDI_NRT_FRAG_RX_H      //reinclude-protection
#define EDDI_NRT_FRAG_RX_H

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
/*  F i l e               &F: eddi_nrt_frag_rx.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NRT- Externals                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#if defined (EDDI_CFG_DEFRAG_ON)

#define EDDI_NRT_FRAG_DA_MAC_SIZE                       6
#define EDDI_NRT_FRAG_SA_MAC_SIZE                       6
#define EDDI_NRT_FRAG_DA_SA_MAC_SIZE                    EDDI_NRT_FRAG_DA_MAC_SIZE + EDDI_NRT_FRAG_SA_MAC_SIZE
#define EDDI_NRT_FRAG_VLAN_SIZE                         4 
//#define EDDI_NRT_FRAG_MAX_FRAME_SIZE                    EDD_FRAME_BUFFER_LENGTH 
#define EDDI_NRT_FRAG_ETHERTYPE_SIZE                    2
#define EDDI_NRT_FRAG_FRAME_ID_SIZE                     2
#define EDDI_NRT_FRAG_DATA_LENGTH_SIZE                  1
#define EDDI_NRT_FRAG_DATA_STATUS_SIZE                  1
//#define EDDI_NRT_FRAG_FCS_SIZE                          4

#define EDDI_NRT_ONLY_FRAG_HEADER                       EDDI_NRT_FRAG_ETHERTYPE_SIZE + EDDI_NRT_FRAG_FRAME_ID_SIZE + EDDI_NRT_FRAG_DATA_LENGTH_SIZE + EDDI_NRT_FRAG_DATA_STATUS_SIZE

#define EDDI_NRT_FRAG_FIRST_HEADER_WITHOUT_VLAN         EDDI_NRT_FRAG_DA_SA_MAC_SIZE + EDDI_NRT_ONLY_FRAG_HEADER + EDDI_NRT_FRAG_ETHERTYPE_SIZE 
//#define EDDI_NRT_FRAG_HEADER_WITHOUT_VLAN               EDDI_NRT_FRAG_DA_SA_MAC_SIZE + EDDI_NRT_ONLY_FRAG_HEADER 

#define EDDI_NRT_HEADER_WITHOUT_VLAN                    EDDI_NRT_FRAG_DA_MAC_SIZE + EDDI_NRT_FRAG_SA_MAC_SIZE + EDDI_NRT_FRAG_ETHERTYPE_SIZE

/*****************************************************************************/
//Return State Values

typedef enum _EDDI_NRT_FRAGMENT_STATE
{
    EDDI_NRT_RECEIVED_FRAGMENTED_FRAMES,
    EDDI_NRT_RECEIVED_FRAGMENTED_FRAME,         
    EDDI_NRT_RECEIVED_NON_FRAGMENTED_FRAMES,    
    EDDI_NRT_DISCARDED_RX_FRAMES,           
    EDDI_NRT_DISCARDED_RX_FRAGMENTS 
         
} EDDI_NRT_FRAGMENT_STATE;

EDDI_NRT_FRAGMENT_STATE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxFragBuildFrame( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                                        EDD_UPPER_MEM_U8_PTR_TYPE     const  pDataBuffer,
                                                                        EDDI_NRT_CHX_SS_IF_TYPE    *  const  pIF,
                                                                        LSA_UINT32                 *  const  pUserDataOffset, //OUT
                                                                        LSA_BOOL                   *  const  pbSyncID1 );

LSA_VOID  EDDI_NRTInitRxFrag( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTRxFragSetStatitics( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             EDDI_NRT_FRAGMENT_STATE  const  StateFrame );
#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_NRT_FRAG_RX_H


/*****************************************************************************/
/*  end of file eddi_nrt_frag_rx.h                                           */
/*****************************************************************************/
