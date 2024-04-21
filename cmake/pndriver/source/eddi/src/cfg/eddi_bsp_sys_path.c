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
/*  F i l e               &F: eddi_bsp_sys_path.c                       :F&  */
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
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  17.07.07    JS    added NRT-Channel to SYNC Path                         */
/*  25.07.07    JS    added SYS_PATH_NRT_CHB_IF_0_FRAME_PTCP_SYNC            */
/*                    added SYS_PATH_NRT_CHB_IF_0_FRAME_LLDP                 */
/*                                                                           */
/*****************************************************************************/ 

#include "eddi_inc.h"
#include "eddi_bsp_edd_ini.h"

//lint -save -e952

/*============ Channel-Description-Types ============*/
typedef struct SYS_CDB_TYPE_
{
    EDDI_CDB_TYPE       CDBHead;
    EDDI_CDB_NRT_TYPE   CDBNRT;

} SYS_CDB_TYPE;

/*===========================================================================*/

typedef struct CDB_S
{
    SYS_CDB_TYPE   NRT_CHA_IF_0;
    SYS_CDB_TYPE   NRT_CHB_IF_0;
    SYS_CDB_TYPE   CRT;
    SYS_CDB_TYPE   SWITCH;
    SYS_CDB_TYPE   SYNC;
    SYS_CDB_TYPE   PRM;

} CDB_T;

static  CDB_T  MD_cdb;

static void EDDI_SysPathIniBlock(SYS_CDB_TYPE   *cdb );

#define SYS_NULL_PTR     (void *)0


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  void  EDDI_SysPathIniBlock( SYS_CDB_TYPE  *  cdb )
{
    EDDI_MEMSET(cdb, (LSA_UINT8)0, (LSA_UINT32)sizeof(SYS_CDB_TYPE));

    cdb->CDBHead.UsedComp.UseNRT    = EDD_CDB_CHANNEL_USE_OFF;
    cdb->CDBHead.UsedComp.UseXRT    = EDD_CDB_CHANNEL_USE_OFF;
    cdb->CDBHead.UsedComp.UseSWITCH = EDD_CDB_CHANNEL_USE_OFF;
    cdb->CDBHead.UsedComp.UseSYNC   = EDD_CDB_CHANNEL_USE_OFF;
    cdb->CDBHead.UsedComp.UsePRM    = EDD_CDB_CHANNEL_USE_OFF;

    cdb->CDBHead.hDDB               = g_SyshDDB;
    cdb->CDBHead.InsertSrcMAC       = EDD_CDB_INSERT_SRC_MAC_DISABLED;

    cdb->CDBHead.pNRT               = (EDDI_UPPER_CDB_NRT_PTR_TYPE)SYS_NULL_PTR;

    cdb->CDBHead.Prm_PortDataAdjustLesserCheckQuality = EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_GET_PATH_INFO( LSA_RESULT            EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                        LSA_SYS_PTR_TYPE      EDDI_LOCAL_MEM_ATTR  *  sys_ptr_ptr,
                                                        EDDI_DETAIL_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  detail_ptr_ptr,
                                                        LSA_SYS_PATH_TYPE                             path )
{
    SYS_CDB_TYPE  *  cdb;
    LSA_UINT32       frame_filter_all;

    *ret_val_ptr = LSA_RET_OK;

    frame_filter_all =   EDD_NRT_FRAME_ARP | EDD_NRT_FRAME_LEN_DCP | EDD_NRT_FRAME_LEN_TSYNC | EDD_NRT_FRAME_LEN_OTHER |EDD_NRT_FRAME_OTHER        
                       | EDD_NRT_FRAME_DCP | EDD_NRT_FRAME_LLDP | EDD_NRT_FRAME_ASRT | EDD_NRT_FRAME_UDP_ASRT | EDD_NRT_FRAME_UDP_DCP      
                       | EDD_NRT_FRAME_PTCP_SYNC | EDD_NRT_FRAME_MRP | EDD_NRT_FRAME_IP_ICMP | EDD_NRT_FRAME_IP_IGMP | EDD_NRT_FRAME_IP_TCP 
                       | EDD_NRT_FRAME_IP_UDP | EDD_NRT_FRAME_IP_VRRP | EDD_NRT_FRAME_IP_OTHER | EDD_NRT_FRAME_RARP | EDD_NRT_FRAME_DCP_HELLO | EDD_NRT_FRAME_UDP_DCP_HELLO
                       | EDD_NRT_FRAME_LEN_STDBY | EDD_NRT_FRAME_LEN_HSR | EDD_NRT_FRAME_PTCP_ANNO | EDD_NRT_FRAME_PTCP_DELAY | EDD_NRT_FRAME_LEN_SINEC_FWL
                       | EDD_NRT_FRAME_HSYNC;  //(EDDI_HSYNC_ROLE_APPL_SUPPORT == pDDB->HSYNCRole) ? EDD_NRT_FRAME_HSYNC : 0UL);


    switch (path)
    {
        case SYS_PATH_SYNC:
        {
            cdb = &MD_cdb.SYNC;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseSYNC = EDD_CDB_CHANNEL_USE_ON;

            /* SYNC must also have a NRT-Channel with B IF0 */
            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_B_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_PTCP_SYNC;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = frame_filter_all;
            break;
        }

        case SYS_PATH_NRT_CHB_IF_0:
        {
            cdb = &MD_cdb.NRT_CHB_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_B_IF_0;
            cdb->CDBNRT.FrameFilter       = frame_filter_all;
            break;
        }

        case SYS_PATH_SWI_0:
        case SYS_PATH_SWI_1:
        case SYS_PATH_SWI_2:
        case SYS_PATH_SWI_3:
        {
            cdb = &MD_cdb.SWITCH;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseSWITCH   = EDD_CDB_CHANNEL_USE_ON;
            break;
        }

        case SYS_PATH_CRT:
        {
            cdb = &MD_cdb.CRT;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseXRT      = EDD_CDB_CHANNEL_USE_ON;
            break;
        }

        /* Examples for NRT filter add-ons (Filters can also be ORed) */
        case SYS_PATH_NRT_CHA_IF_0_FRAME_ASRT:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_ASRT;
            break;
        }

        case SYS_PATH_NRT_CHB_IF_0_FRAME_LLDP:
        {
            cdb = &MD_cdb.NRT_CHB_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_B_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_LLDP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDDI_NRT_FRAME_IP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP_ICMP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_IP_ICMP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP_IGMP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_IP_IGMP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP_TCP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_IP_TCP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP_UDP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_IP_UDP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP_VRRP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT = &cdb->CDBNRT;
            cdb->CDBNRT.Channel = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter = EDD_NRT_FRAME_IP_VRRP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_IP_OTHER:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_IP_OTHER;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_ARP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_ARP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_RARP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_RARP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_DCP:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_DCP;
            break;
        }

        case SYS_PATH_NRT_CHA_IF_0_FRAME_PTCP_ANNO:
        {
            cdb = &MD_cdb.NRT_CHA_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_A_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_PTCP_ANNO;
            break;
        }

        case SYS_PATH_NRT_CHB_IF_0_FRAME_PTCP_SYNC:
        {
            cdb = &MD_cdb.NRT_CHB_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_B_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_PTCP_SYNC;
            break;
        }

        case SYS_PATH_NRT_CHB_IF_0_FRAME_MRP:
        {
            cdb = &MD_cdb.NRT_CHB_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_B_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_MRP;
            break;
        }

        case SYS_PATH_NRT_CHB_IF_0_FRAME_HSYNC:
        {
            cdb = &MD_cdb.NRT_CHB_IF_0;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UseNRT  = LSA_TRUE;
            cdb->CDBHead.pNRT             = &cdb->CDBNRT;
            cdb->CDBNRT.Channel           = EDDI_NRT_CHANEL_B_IF_0;
            cdb->CDBNRT.FrameFilter       = EDD_NRT_FRAME_HSYNC;
            break;
        }

        case SYS_PATH_PRM:
        {
            cdb = &MD_cdb.PRM;
            EDDI_SysPathIniBlock(cdb);

            cdb->CDBHead.UsedComp.UsePRM                      = EDD_CDB_CHANNEL_USE_ON;
            cdb->CDBHead.Prm_PortDataAdjustLesserCheckQuality = EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY;
            break;
        }

        default:
        {
            *ret_val_ptr = LSA_RET_ERR_SYS_PATH;
            return;
        }
    }

    *detail_ptr_ptr = &cdb->CDBHead;
    *sys_ptr_ptr    = (LSA_SYS_PTR_TYPE)0;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_RELEASE_PATH_INFO( LSA_RESULT            EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                            LSA_SYS_PTR_TYPE                              sys_ptr,
                                                            EDDI_DETAIL_PTR_TYPE                          detail_ptr )
{
    LSA_UNUSED_ARG(sys_ptr); //satisfy lint!
    LSA_UNUSED_ARG(detail_ptr); //satisfy lint!

    *ret_val_ptr = LSA_RET_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//lint -restore


/****************************************************************************/
/*  end of file eddi_bsp_sys_path.c                                         */
/****************************************************************************/
