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
/*  F i l e               &F: eddi_g_r6.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD General requests                             */
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

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_G_R6
#define LTRC_ACT_MODUL_ID  3

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_EDDI_G_R6) //satisfy lint!
#endif

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

//#include "eddi_swi_ext.h"

//#include "eddi_ser_cmd.h"
//#include "eddi_nrt_arp.h"


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenR6Ini()                                  */
/*                                                                         */
/* D e s c r i p t i o n: initialize HW-extensions in revision 6 and 7     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenR6Ini( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_R6_NRT_ARP_DCP_FILTER_TYPE  *  const  pR6_arp_dcp = &pDDB->pLocal_NRT->R6_arp_dcp;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenR6Ini->");

    //Init ARP/DCP-Rcv-List

    pR6_arp_dcp->pCCW_Entry = &pDDB->pKramFixMem->DCP_ARP_CCW_Entry;

    pDDB->KramRes.ArpDcpCCWLen = sizeof(EDDI_SER_NRT_CCW_ENTRY_TYPE);

    pR6_arp_dcp->pCCW_Entry->p.Top    = SER10_NULL_PTR_SWAP;
    pR6_arp_dcp->pCCW_Entry->p.Bottom = SER10_NULL_PTR_SWAP;

    IO_W32(FILTER_CTRL_BASE, DEV_kram_adr_to_asic_register(pR6_arp_dcp->pCCW_Entry, pDDB));

    //Init Time Master List (for Generic Sync)
    IO_x32(TM_LIST_BASE_ADR) = SER10_NULL_PTR_SWAP; //deactivate list
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_REV6 || EDDI_CFG_REV7


/*****************************************************************************/
/*  end of file eddi_g_r6.c                                                  */
/*****************************************************************************/

