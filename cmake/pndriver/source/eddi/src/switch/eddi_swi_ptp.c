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
/*  F i l e               &F: eddi_swi_ptp.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDI.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  21.09.07    JS    added DCPBoundary handling                             */
/*  26.11.2007  JS    added Support for changing of PRIO of PTCP Sync Frame  */
/*                    without FollowUp if BLOCKING is enabled to ORG Prio    */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_swi_ext.h"
#include "eddi_ext.h"
//#include "eddi_swi_ucmc.h"
#include "eddi_swi_ptp.h"

#define EDDI_MODULE_ID     M_ID_SWI_PTP
#define LTRC_ACT_MODUL_ID  314

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_UINT16 EDDI_LOCAL_FCT_ATTR EDDI_SwiPNDetermineSyncMACPrio( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNSetFWCtrlDAll( EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE     const  pMCFWDCtrlParam,
                                                                  EDD_MAC_ADR_TYPE                const  *  const  pMACAdr,
                                                                  LSA_UINT32                                const  MACLen,
                                                                  LSA_BOOL                                  const  checkBLAllowed,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                  LSA_UINT8                                 const  blTableIndex,
                                                                  LSA_UINT16                                const  MACAdrBandIndex );

static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNCompareMAC( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                             EDD_MAC_ADR_TYPE  const  *  const  pMCMACToCompare,
                                                             EDD_MAC_ADR_TYPE  const  *  const  pMCMACPtp,
                                                             LSA_UINT32                  const  MCPtpLen );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNtoFDB( LSA_BOOL                           const bFWDPortOn,
                                                       EDD_MAC_ADR_TYPE           const * const pMACAdr,
                                                       LSA_UINT32                         const MACLen,
                                                       LSA_UINT16                         const Prio,
                                                       EDDI_LOCAL_DDB_PTR_TYPE            const pDDB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiSetBCPNtoFDB( LSA_BOOL                           const  bFWDPortOn,
                                                             EDD_MAC_ADR_TYPE         const  *  const  pMACAdr,
                                                             LSA_UINT32                         const  MACLen,
                                                             LSA_UINT16                         const  Prio,
                                                             EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB );

//static LSA_RESULT EDDI_LOCAL_FCT_ATTR SwiPNIsMACExist( EDDI_LOCAL_MAC_ADR_PTR_TYPE   const pMACAdr,
//                                                       EDDI_LOCAL_DDB_PTR_TYPE       const pDDB );

//static LSA_BOOL EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsSTBY1MAC (EDDI_LOCAL_MAC_ADR_PTR_TYPE const pMCMAC );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsSTBY2MAC( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsHSRMAC( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                          EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE const  pMCMAC );

static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsPTPMAC( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                           EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE    const  pMCMAC );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsPNMAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                         EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsIEEEMAC( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                           EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE    const  pMCMAC,
                                                           LSA_BOOL                             const  bSetInRun );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsLLDPMAC( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                           EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE    const  pMCMAC );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsMRPMAC( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB, 
                                                          EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE     const  pMCMAC );

static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsSYNCMAC( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                           EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE    const  pMCMAC );


#if defined (EDDI_CFG_REV7)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiSetBLTableforGSY_OHAUsr( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                                     LSA_BOOL                   const  IsPTCPMacAdrBandIndex,
                                                                     LSA_UINT32                 const  MacTableIdx,
                                                                     LSA_UINT8                  const  blTableIndex,
                                                                     LSA_UINT32                 const  UsrPortIndex,
                                                                     LSA_BOOL                   const  bFWDPortOn,
                                                                     LSA_UINT16                 const  PortMode);

static LSA_UINT8 EDDI_LOCAL_FCT_ATTR EDDI_SwiCalculateBLBit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                             LSA_BOOL                    const  IsPTCPMacAdrBandIndex,
                                                             LSA_UINT32                  const  MacTableIdx,
                                                             LSA_UINT8                   const  blTableIndex,
                                                             LSA_BOOL                    const  bFWDPortOn,
                                                             LSA_BOOL                    const  checkBLAllowed,
                                                             LSA_BOOL                    const  ActuelBorderline,
                                                             LSA_UINT16                  const  PortMode);
#endif

/*===========================================================================*/
/*                                local variables                            */
/*===========================================================================*/
const EDD_MAC_ADR_TYPE    MAC_BROADCAST               = { EDDI_MAC_BROADCAST };

const EDD_MAC_ADR_TYPE    MAC_PROFINET                = { EDDI_MAC_PROFINET };
/* IEC - PNIO */
const EDD_MAC_ADR_TYPE    MAC_RT_3_DA_BEGIN           = { EDDI_MAC_RT_3_DA_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_RT_3_INVALID_DA_BEGIN   = { EDDI_MAC_RT_3_INVALID_DA_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_RT_2_QUER_BEGIN         = { EDDI_MAC_RT_2_QUER_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_PTP_ANNOUNCE_BEGIN      = { EDDI_MAC_PTP_ANNOUNCE_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_TIME_ANNOUNCE_BEGIN     = { EDDI_MAC_TIME_ANNOUNCE_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_PTP_SYNC_WITH_FU_BEGIN  = { EDDI_MAC_PTP_SYNC_WITH_FU_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_TIME_SYNC_WITH_FU_BEGIN = { EDDI_MAC_TIME_SYNC_WITH_FU_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_PTP_FOLLOW_UP_BEGIN     = { EDDI_MAC_PTP_FOLLOW_UP_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_TIME_FOLLOW_UP_BEGIN    = { EDDI_MAC_TIME_FOLLOW_UP_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_PTP_SYNC_BEGIN          = { EDDI_MAC_PTP_SYNC_BEGIN };
const EDD_MAC_ADR_TYPE    MAC_TIME_SYNC_BEGIN         = { EDDI_MAC_TIME_SYNC_BEGIN };
/* IEC - MRP 1 */
const EDD_MAC_ADR_TYPE    MAC_MRP_1_BEGIN             = { EDDI_MAC_MRP_1_BEGIN };      //when changing check all locations with MAC_MRP_1_LEN!
/* IEC - MRP 2 */
const EDD_MAC_ADR_TYPE    MAC_MRP_2_BEGIN             = { EDDI_MAC_MRP_2_BEGIN };      //when changing check all locations with MAC_MRP_2_LEN!
/* IEC - MRP Interconnect 1 */
const EDD_MAC_ADR_TYPE    MAC_MRP_IC_1_BEGIN          = { EDDI_MAC_MRP_IC_1_BEGIN };   //when changing check all locations with MAC_MRP_IC_1_LEN!
/* IEC - MRP Interconnect 2 */
const EDD_MAC_ADR_TYPE    MAC_MRP_IC_2_BEGIN          = { EDDI_MAC_MRP_IC_2_BEGIN };   //when changing check all locations with MAC_MRP_IC_2_LEN!
/* IEC - HSYNC */
const EDD_MAC_ADR_TYPE    MAC_HSYNC_BEGIN             = { EDDI_MAC_HSYNC_BEGIN };      //when changing check all locations with MAC_HSYNC_LEN!
/* IEEE Reserved addresses - 1 00-00-00 - 00-00-0D */
const EDD_MAC_ADR_TYPE    MAC_IEEE_RESERVED_BEGIN_1   = { EDDI_MAC_IEEE_RESERVED_BEGIN_1 };
/* IEEE Reserved addresses - 1 00-00-01 - 00-00-0D */
const EDD_MAC_ADR_TYPE    MAC_IEEE_RESERVED_BEGIN_1_1 = { EDDI_MAC_IEEE_RESERVED_BEGIN_1_1 };
/* PTP (Delay) and LLDP        00-00-0E            */
const EDD_MAC_ADR_TYPE    MAC_LLDP_PTP_DELAY_BEGIN    = { EDDI_MAC_LLDP_PTP_DELAY_BEGIN };
/* IEEE Reserved addresses - 2 00-00-0F - 00-00-10 */
const EDD_MAC_ADR_TYPE    MAC_IEEE_RESERVED_BEGIN_2   = { EDDI_MAC_IEEE_RESERVED_BEGIN_2 };
/* HSR 1 */
const EDD_MAC_ADR_TYPE    MAC_HSR_1_BEGIN             = { EDDI_MAC_HSR_1_BEGIN };      //when changing check all locations with MAC_HSR_1_LEN!
/* HSR 2 */
const EDD_MAC_ADR_TYPE    MAC_HSR_2_BEGIN             = { EDDI_MAC_HSR_2_BEGIN };      //when changing check all locations with MAC_HSR_2_LEN!
/* HSR STBY 1 */
const EDD_MAC_ADR_TYPE    MAC_STBY_1_BEGIN            = { EDDI_MAC_STBY_1_BEGIN };
/* HSR STBY 2 */
const EDD_MAC_ADR_TYPE    MAC_STBY_2_BEGIN            = { EDDI_MAC_STBY_2_BEGIN };
/* DCP addresses 01-0E-CF-00-00-00 - 01-0E-CF-00-00-01 */
const EDD_MAC_ADR_TYPE    MAC_DCP_BEGIN               = { EDDI_MAC_DCP_BEGIN };
/* VRRP */
const EDD_MAC_ADR_TYPE    MAC_CARP_MC_BEGIN           = { EDDI_MAC_CARP_MC_BEGIN };

#if defined (EDDI_CFG_REV7)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPNClearAllBL()                           */
/*                                                                         */
/* D e s c r i p t i o n: clears all relevant BL-bits in FDB-Table         */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e s u l t:           LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNClearAllBL( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT  result;

    //clear BL-Bits from Multicast-Boundaries
    result = EDDI_SwiPNSetBL(pDDB, 0 /* clear all BL */, &MAC_RT_2_QUER_BEGIN, MAC_RT_3_QUER_LEN);
    if (EDD_STS_OK != result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_RT_2_QUER), Result:0x%X", result);
        EDDI_Excp("EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_RT_2_QUER)", EDDI_FATAL_ERR_EXCP, result, 0);
    }

    //clear BL-Bits from Domain-Boundaries
    result = EDDI_SwiPNSetBL(pDDB, 0 /* clear all BL */, &MAC_PTP_ANNOUNCE_BEGIN, MAC_PTP_ANNOUNCE_LEN);
    if (EDD_STS_OK != result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_ANNOUNCE), Result:0x%X", result);
        EDDI_Excp("EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_ANNOUNCE)", EDDI_FATAL_ERR_EXCP, result, 0);
    }
    result = EDDI_SwiPNSetBL(pDDB, 0 /* clear all BL */, &MAC_PTP_SYNC_WITH_FU_BEGIN, MAC_PTP_SYNC_WITH_FU_LEN);
    if (EDD_STS_OK != result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_SYNC_WITH_FU), Result:0x%X", result);
        EDDI_Excp("EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_SYNC_WITH_FU)", EDDI_FATAL_ERR_EXCP, result, 0);
    }
    result = EDDI_SwiPNSetBL(pDDB, 0 /* clear all BL */, &MAC_PTP_FOLLOW_UP_BEGIN, MAC_PTP_FOLLOW_UP_LEN);
    if (EDD_STS_OK != result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_FOLLOW_UP), Result:0x%X", result);
        EDDI_Excp("EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_FOLLOW_UP)", EDDI_FATAL_ERR_EXCP, result, 0);
    }
    result = EDDI_SwiPNSetBL(pDDB, 0 /* clear all BL */, &MAC_PTP_SYNC_BEGIN, MAC_PTP_SYNC_LEN);
    if (EDD_STS_OK != result)
    {
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_SYNC), Result:0x%X", result);
        EDDI_Excp("EDDI_SwiPNClearAllBL, EDDI_SwiPNSetBL(MAC_PTP_SYNC)", EDDI_FATAL_ERR_EXCP, result, 0);
    }

}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPNSetSyncMACPrio()                       */
/*                                                                         */
/* D e s c r i p t i o n: Sets the priority of the Sync-MC-MAC addresses   */
/*                        of Sync-Frames without Followup to the prio      */
/*                        The MC-MAC must exist within FDB table!          */
/*                        The actual prio will be saved within variable    */
/*                        pDDB->pLocal_SWITCH->SyncMACPrio                 */
/*                                                                         */
/* A r g u m e n t s:     Prio: EDDI_SWI_FDB_PRIO_DEFAULT                  */
/*                              EDDI_SWI_FDB_PRIO_ORG                      */
/*                                                                         */
/* R e s u l t:           LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNSetSyncMACPrio( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT16               const  Prio )
{
    LSA_RESULT                       Status;
    LSA_BOOL                         bValid;
    LSA_BOOL                         bDestSameMCDef;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE  UsrSetEntryFDB;
    EDD_MAC_ADR_TYPE                 MACAdr;
    LSA_UINT32                       MACIndex;

    /* only if Prio changed..*/
    if (pDDB->pLocal_SWITCH->SyncMACPrio != Prio)
    {
        MACAdr = MAC_PTP_SYNC_BEGIN;

        /* All MAC addresses must be changed..(SyncID 0..31) */
        for (MACIndex = 0; MACIndex < MAC_PTP_SYNC_LEN; MACIndex++)
        {
            Status = EDDI_GenGetDestPortMCDef((EDDI_LOCAL_MAC_ADR_PTR_TYPE)&MACAdr, &bDestSameMCDef, &bValid, &UsrSetEntryFDB, pDDB);

            if (EDD_STS_OK != Status)
            {
                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SwiPNSetSyncMACPrio, EDDI_GenGetDestPortMCDef failed, Status:0x%X", Status);
                EDDI_Excp("SEDDI_SwiPNSetSyncMACPrio, failed!", EDDI_FATAL_ERR_EXCP, Status, 0);
                return;
            }
            else
            {
                if (bValid)
                {
                    UsrSetEntryFDB.Prio = Prio;

                    Status = EDDI_SERSetFDBEntryRun(&pDDB->Glob.LLHandle, &UsrSetEntryFDB, LSA_FALSE, pDDB); /* no PNIO check! */
                    if (EDD_STS_OK != Status)
                    {
                        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SwiPNSetSyncMACPrio, EDDI_SERSetFDBEntryRun failed, Status:0x%X", Status);
                        EDDI_Excp("EDDI_SwiPNSetSyncMACPrio, EDDI_SERSetFDBEntryRun failed!", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }

                    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SwiPNSetSyncMACPrio, SYNC-MAC-Prio set to 0x%x (%d)", Prio, Prio);
                }
                else
                {
                    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SwiPNSetSyncMACPrio, MC-MAC entry is not valid.");
                    EDDI_Excp("EDDI_SwiPNSetSyncMACPrio, MC-MAC entry is not valid!", EDDI_FATAL_ERR_EXCP, 0, 0);
                    return;
                }
            }

            MACAdr.MacAdr[EDD_MAC_ADDR_SIZE - 1] += 1; /* Next address. increment last byte */
        } /* for */

        pDDB->pLocal_SWITCH->SyncMACPrio = Prio;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPNDetermineSyncMACPrio()                 */
/*                                                                         */
/* D e s c r i p t i o n: Determines the actual SyncMACPrio needed         */
/*                        depending on the actual Port state of ports      */
/*                        If no Port is CLOSED the prio will be set to     */
/*                        EDDI_SWI_FDB_PRIO_DEFAULT. If at least one is    */
/*                        CLOSED we set EDDI_SWI_FDB_PRIO_ORG              */
/*                                                                         */
/*                        Note: EDDI_SWI_FDB_PRIO_DEFAULT means ISRT Prio  */
/*                              with RT-Frames!                            */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e s u l t:           Prio: EDDI_SWI_FDB_PRIO_DEFAULT                  */
/*                              EDDI_SWI_FDB_PRIO_ORG                      */
/*                                                                         */
/***************************************************************************/
static  LSA_UINT16  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNDetermineSyncMACPrio( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortIndex;
    
    #if defined (EDDI_CFG_FRAG_ON)
    if (pDDB->NRT.TxFragData.bTxFragmentationOn)
    {
        return EDDI_SWI_FDB_PRIO_ORG;
    }
    #endif

    /* check all ports for a BLOCKING state */
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        /* A Port is CLOSED if PortState is BLOCKING, LISTENING or LEARNING */
        if ((EDD_PORT_STATE_BLOCKING  == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState) ||
            (EDD_PORT_STATE_LISTENING == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState) ||
            (EDD_PORT_STATE_LEARNING  == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].SpanningTreeState))
        {
            return EDDI_SWI_FDB_PRIO_ORG;
        }
    }

    return EDDI_SWI_FDB_PRIO_DEFAULT;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPNUpdateSyncMACPrio()                    */
/*                                                                         */
/* D e s c r i p t i o n: Updates the priority of the Sync-MC-MAC address  */
/*                        of Sync-Frames without Followup depending on     */
/*                        the actual CLOSED state of a port.               */
/*                        If no Port is CLOSED the prio will be set to     */
/*                        EDDI_SWI_FDB_PRIO_DEFAULT. If at least one is    */
/*                        blocking we set EDDI_SWI_FDB_PRIO_ORG            */
/*                                                                         */
/*                        Note: EDDI_SWI_FDB_PRIO_DEFAULT means ISRT Prio  */
/*                              with RT-Frames!                            */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* R e s u l t:           LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNUpdateSyncMACPrio( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT16  Prio;

    Prio = EDDI_SwiPNDetermineSyncMACPrio(pDDB);  /* get actual prio to set */
    EDDI_SwiPNSetSyncMACPrio(pDDB,Prio);          /* set the prio           */
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNPNToFDB()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNPNToFDB( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                  EDDI_UPPER_DSB_PTR_TYPE  const  pDSB )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNPNToFDB->");

    //Write FDB-entries at startup

    /* IEC - PNIO */
    EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_RT_2_QUER_BEGIN,          MAC_RT_3_QUER_LEN,          EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_RT_3_DA_BEGIN,            MAC_RT_3_DA_LEN,            EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    EDDI_SwiSetBCPNtoFDB(LSA_TRUE, &MAC_BROADCAST,            MAC_BROADCAST_LEN,          EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_RT_3_INVALID_DA_BEGIN,    MAC_RT_3_INVALID_DA_LEN,    EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_PTP_ANNOUNCE_BEGIN,       MAC_PTP_ANNOUNCE_LEN,       EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_PTP_SYNC_WITH_FU_BEGIN,   MAC_PTP_SYNC_WITH_FU_LEN,   EDDI_SWI_FDB_PRIO_ORG,     pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_PTP_FOLLOW_UP_BEGIN,      MAC_PTP_FOLLOW_UP_LEN,      EDDI_SWI_FDB_PRIO_ORG,     pDDB);

    /* The Prio for SYNC-Frames without FollowUp depends on PortState. If one Port is CLOSED we */
    /* use EDDI_SWI_FDB_PRIO_ORG. If no Port is BLOCKING we use EDDI_SWI_FDB_PRIO_DEFAULT       */
    pDDB->pLocal_SWITCH->SyncMACPrio = EDDI_SwiPNDetermineSyncMACPrio(pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_PTP_SYNC_BEGIN,           MAC_PTP_SYNC_LEN,           pDDB->pLocal_SWITCH->SyncMACPrio, pDDB);

    /* PTCP TimeSync addresses are only reserved in the FDB. No forwarding, no reception unless enabled by GSY */
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_TIME_ANNOUNCE_BEGIN, MAC_TIME_ANNOUNCE_LEN, EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_TIME_SYNC_WITH_FU_BEGIN, MAC_TIME_SYNC_WITH_FU_LEN, EDDI_SWI_FDB_PRIO_ORG, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_TIME_FOLLOW_UP_BEGIN, MAC_TIME_FOLLOW_UP_LEN, EDDI_SWI_FDB_PRIO_ORG, pDDB);
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_TIME_SYNC_BEGIN, MAC_TIME_SYNC_LEN, EDDI_SWI_FDB_PRIO_ORG, pDDB);

    if (EDD_FEATURE_ENABLE == pDSB->SWIPara.Multicast_Bridge_IEEE802RSTP_Forward)
    {
        /* Forward -> 0x01, 0x80, 0xC2, 0x00, 0x00, 0x00 -> RSTP */
        EDDI_SwiPNtoFDB(LSA_TRUE, &MAC_IEEE_RESERVED_BEGIN_1,   1,                          EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);

        /* IEEE Reserved */
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_IEEE_RESERVED_BEGIN_1_1, MAC_IEEE_RESERVED_LEN_1_1, EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    }
    else
    {
        /* IEEE Reserved */
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_IEEE_RESERVED_BEGIN_1,MAC_IEEE_RESERVED_LEN_1,    EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    }

    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_IEEE_RESERVED_BEGIN_2,    MAC_IEEE_RESERVED_LEN_2,    EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);

    /* PTP (Delay) and LLDP */
    EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_LLDP_PTP_DELAY_BEGIN,     MAC_LLDP_PTP_DELAY_LEN,     EDDI_SWI_FDB_PRIO_ORG,     pDDB);

    /* IEC - MRP */
    if (!pDDB->SWITCH.bMRPRedundantPortsUsed)
    {
        /* if MRP is NOT used we shall forward MRP-MC-Addresses ! */
        EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_MRP_1_BEGIN,          MAC_MRP_1_LEN,              EDDI_SWI_FDB_PRIO_ORG,     pDDB);
        EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_MRP_2_BEGIN,          MAC_MRP_2_LEN,              EDDI_SWI_FDB_PRIO_ORG,     pDDB);
    }
    else
    {
        /* if MRP is used we shall NOT forward MRP-MC-Addresses ! */
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_MRP_1_BEGIN,          MAC_MRP_1_LEN,              EDDI_SWI_FDB_PRIO_ORG,     pDDB);
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_MRP_2_BEGIN,          MAC_MRP_2_LEN,              EDDI_SWI_FDB_PRIO_ORG,     pDDB);
    }

    /* IEC - MRP IC */
    if (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported)
    {
        /* if MRP IC is used, the MC addr´s are reserved without forwarding */
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_MRP_IC_1_BEGIN,       MAC_MRP_IC_1_LEN,           EDDI_SWI_FDB_PRIO_ORG,     pDDB);
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_MRP_IC_2_BEGIN,       MAC_MRP_IC_2_LEN,           EDDI_SWI_FDB_PRIO_ORG,     pDDB);
        /* if MRP IC is NOT used, the MC addr´s will take the default MC behaviour */
    }

    /* IEC - HSYNC */
    if (EDDI_HSYNC_ROLE_NONE != pDDB->HSYNCRole)
    {
        EDDI_SwiPNtoFDB(LSA_FALSE, &MAC_HSYNC_BEGIN,          MAC_HSYNC_LEN,              EDDI_SWI_FDB_PRIO_ORG,     pDDB);
    }

    /* HSR 1 */
    EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_HSR_1_BEGIN,              MAC_HSR_1_LEN,              EDDI_SWI_FDB_PRIO_3,       pDDB);

    /* HSR 2 */
    EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_HSR_2_BEGIN,              MAC_HSR_2_LEN,              EDDI_SWI_FDB_PRIO_3,       pDDB);

    /* STBY 1 */
    EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_STBY_1_BEGIN,             MAC_STBY_1_LEN,             EDDI_SWI_FDB_PRIO_3,       pDDB);

    /* STBY 2 */
    EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_STBY_2_BEGIN,             MAC_STBY_2_LEN,             EDDI_SWI_FDB_PRIO_3,       pDDB);

    /* DCP */
    EDDI_SwiPNtoFDB(LSA_TRUE,  &MAC_DCP_BEGIN,                MAC_DCP_LEN,                EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);

    /* VRRP */
    if (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ClusterIPSupport)
    {
        EDDI_SwiPNtoFDB(LSA_TRUE, &MAC_CARP_MC_BEGIN,         MAC_CARP_MC_LEN,               EDDI_SWI_FDB_PRIO_DEFAULT, pDDB);
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNPNToFDB<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_ENABLE_MC_FDB)
/*=============================================================================
* function name: EDDI_SwiPNPNToFDBGetSize()
*
* function:
*
* parameters:
*
* return value:  LSA_UINT32
*
* comment:
*==========================================================================*/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNPNToFDBGetSize( LSA_VOID )
{
    return (EDDI_MAC_MAX_MCMAC_ADR);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*=============================================================================
* function name: EDDI_SwiPNtoFDB()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNtoFDB( LSA_BOOL                           const  bFWDPortOn,
                                                        EDD_MAC_ADR_TYPE         const  *  const  pMACAdr,
                                                        LSA_UINT32                         const  MACLen,
                                                        LSA_UINT16                         const  Prio,
                                                        EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    LSA_UINT8                        MACIndex;
    LSA_UINT32                       UsrPortIndex;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE  PTPMAC;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPNtoFDB->MACLen:0x%X", MACLen);

    //EDDI_MemSet(&PTPMAC, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE));

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        //Okay User structure
        PTPMAC.PortID[UsrPortIndex] = (bFWDPortOn) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
    }

    PTPMAC.CHA        = EDD_FEATURE_DISABLE;
    PTPMAC.CHB        = EDD_FEATURE_DISABLE;
    PTPMAC.Prio       = Prio;
    PTPMAC.Pause      = EDD_FEATURE_DISABLE;
    PTPMAC.Filter     = EDD_FEATURE_DISABLE;
    PTPMAC.Borderline = EDD_FEATURE_DISABLE;
    PTPMAC.MACAddress = *pMACAdr;

    EDDI_SERSetFDBEntryConfig(&pDDB->Glob.LLHandle, &PTPMAC, pDDB);

    //Insert addressband from MC-MAC
    for (MACIndex = 1; MACIndex < MACLen; MACIndex++)
    {
        PTPMAC.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE - 1] = (LSA_UINT8)(MACIndex + pMACAdr->MacAdr[EDD_MAC_ADDR_SIZE - 1]);

        EDDI_SERSetFDBEntryConfig(&pDDB->Glob.LLHandle, &PTPMAC, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiSetBCPNtoFDB()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiSetBCPNtoFDB( LSA_BOOL                           const  bFWDPortOn,
                                                             EDD_MAC_ADR_TYPE         const  *  const  pMACAdr,
                                                             LSA_UINT32                         const  MACLen,
                                                             LSA_UINT16                         const  Prio,
                                                             EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    LSA_UINT8                        MACIndex;
    LSA_UINT32                       UsrPortIndex;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE  PTPMAC;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPNtoFDB->MACLen:0x%X", MACLen);

    //EDDI_MemSet(&PTPMAC, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE));

    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        //Okay User structure
        PTPMAC.PortID[UsrPortIndex] = (bFWDPortOn) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
    }

    PTPMAC.CHA        = EDD_FEATURE_ENABLE;
    PTPMAC.CHB        = EDD_FEATURE_ENABLE;
    PTPMAC.Prio       = Prio;
    PTPMAC.Pause      = EDD_FEATURE_DISABLE;
    PTPMAC.Filter     = EDD_FEATURE_DISABLE;
    PTPMAC.Borderline = EDD_FEATURE_DISABLE;
    PTPMAC.MACAddress = *pMACAdr;

    EDDI_SERSetFDBEntryConfig(&pDDB->Glob.LLHandle, &PTPMAC, pDDB);

    //Insert addressband from MC-MAC
    for (MACIndex = 1; MACIndex < MACLen; MACIndex++)
    {
        PTPMAC.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE - 1] = (LSA_UINT8)(MACIndex + pMACAdr->MacAdr[EDD_MAC_ADDR_SIZE - 1]);

        EDDI_SERSetFDBEntryConfig(&pDDB->Glob.LLHandle, &PTPMAC, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNGetPrio()
*
* function:
*
* parameters:
*
* return value:  LSA_UINT16
*
* comment:
*==========================================================================*/
LSA_UINT16  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNGetPrio( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                    EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNGetPrio->");

    if ((EDDI_SwiPNIsLLDPMAC(pDDB, pMCMAC))  ||
        (EDDI_SwiPNIsHSRMAC(pDDB, pMCMAC))   ||
        (EDDI_SwiPNIsSTBY1MAC(pDDB, pMCMAC)) ||
        (EDDI_SwiPNIsPTPMAC(pDDB, pMCMAC))   ||
        (EDDI_SwiPNIsMRPMAC(pDDB, pMCMAC)))
    {
        return EDDI_SWI_FDB_PRIO_ORG;
    }

    if //SYNC without FollowUp prio is dynamic?
       (EDDI_SwiPNIsSYNCMAC(pDDB, pMCMAC))
    {
        return (pDDB->pLocal_SWITCH->SyncMACPrio);
    }

    if (EDDI_SwiPNIsSTBY2MAC(pDDB, pMCMAC))
    {
        return EDDI_SWI_FDB_PRIO_3;
    }

    //IEEE Reserved-Frame and rest of PNIO are EDDI_SWI_FDB_PRIO_DEFAULT
    return EDDI_SWI_FDB_PRIO_DEFAULT;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNMCFwdCtrl()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNMCFwdCtrl( EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE   const  pMCFWDCtrlParam,
                                                      EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB, 
                                                      LSA_UINT8                               const  blTableIndex )
{
    LSA_RESULT                          Status;
    EDD_RQB_MULTICAST_FWD_CTRL_TYPE     MCFWDCtrlParam;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiPNMCFwdCtrl->MACAddrGroup:0x%X MACAddrLow:0x%X", pMCFWDCtrlParam->MACAddrGroup,pMCFWDCtrlParam->MACAddrLow);

    switch ( pMCFWDCtrlParam->MACAddrGroup)
    {
        case EDDI_MAC_ADDR_GROUP_PTCP_ANNOUNCE:
        {
            //frames for syncid 0 and 1 shall be forwarded by HW
            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_ANNOUNCE_BEGIN, 1UL,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_ANNOUNCE_INDEX);
        }
        break;

        case EDDI_MAC_ADDR_GROUP_PTCP_SYNC_WITH_FU:
        {
            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_SYNC_WITH_FU_BEGIN, 1UL,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_SYNC_WITH_FU_INDEX);
        }
        break;

        case EDDI_MAC_ADDR_GROUP_PTCP_FOLLOW_UP:
        {
            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_FOLLOW_UP_BEGIN, 1UL,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_FOLLOW_UP_INDEX);
        }
        break;

        case EDDI_MAC_ADDR_GROUP_PTCP_SYNC:
        {
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_PTP_SYNC_BEGIN, 1UL /*MAC_PTP_SYNC_LEN*/,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTCP_SYNC_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_PTCP_ALL:
        {
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;

            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_PTP_ANNOUNCE_BEGIN, MAC_PTP_ANNOUNCE_LEN,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_ANNOUNCE_INDEX);
            if (EDD_STS_OK != Status)
            {
                break;
            }
            
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_PTP_SYNC_WITH_FU_BEGIN, MAC_PTP_SYNC_WITH_FU_LEN,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_SYNC_WITH_FU_INDEX);
            if (EDD_STS_OK != Status)
            {
                break;
            }

            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_PTP_FOLLOW_UP_BEGIN, MAC_PTP_FOLLOW_UP_LEN,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_FOLLOW_UP_INDEX);
            if (EDD_STS_OK != Status)
            {
                break;
            }

            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_PTP_SYNC_BEGIN, MAC_PTP_SYNC_LEN,
                                             LSA_TRUE, pDDB, blTableIndex,EDDI_BL_MAC_ADR_BAND_PTCP_SYNC_INDEX);

        }
        break;

        case EDD_MAC_ADDR_GROUP_PTCP:
        {
            //frames for syncid 0 and 1 shall be forwarded by HW
            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_ANNOUNCE_BEGIN, 1UL /*2UL*/,
                                              LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_ANNOUNCE_INDEX);
            if (EDD_STS_OK != Status)
            {
                break;
            }

            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_SYNC_WITH_FU_BEGIN, 1UL,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_SYNC_WITH_FU_INDEX);
            if (EDD_STS_OK != Status)
            {
                break;
            }

            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_FOLLOW_UP_BEGIN, 1UL,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTP_FOLLOW_UP_INDEX);
            if (EDD_STS_OK != Status)
            {
                break;
            }

            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_PTP_SYNC_BEGIN, 1UL,
                                             LSA_TRUE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_PTCP_SYNC_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_RT_2_QUER:
        {
            //MACAddrLow -> BitMask !!!!!!!!!!!!!!
            //Will have EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX. but BL_USR is NONE ==> no effect on BL table
            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam, &MAC_RT_2_QUER_BEGIN, MAC_RT_3_QUER_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_DCP:
        {
            //MACAddrLow -> BitMask !!!!!!!!!!!!!!
            //only bit 0 and 1 used!
            //Will have EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX. but BL_USR is NONE ==> no effect on BL table
            Status = EDDI_SwiPNSetFWCtrlDAll(pMCFWDCtrlParam,&MAC_DCP_BEGIN, MAC_DCP_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_MRP_1:
        {
            //MACAddrLow -> no importance
            //Will have EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX. but BL_USR is NONE ==> no effect on BL table
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_MRP_1_BEGIN, MAC_MRP_1_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_MRP_2:
        {
            //MACAddrLow -> no importance
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_MRP_2_BEGIN, MAC_MRP_2_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_MRP_IN_1:
        {
            //MACAddrLow -> no importance
            //Will have EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX. but BL_USR is NONE ==> no effect on BL table
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_MRP_IC_1_BEGIN, MAC_MRP_IC_1_LEN,
                LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_MRP_IN_2:
        {
            //MACAddrLow -> no importance
            //Will have EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX. but BL_USR is NONE ==> no effect on BL table
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_MRP_IC_2_BEGIN, MAC_MRP_IC_2_LEN,
                LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_HSYNC:
        {
            //MACAddrLow -> no importance
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_HSYNC_BEGIN, MAC_HSYNC_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_HSR_1:
        {
            //MACAddrLow -> no importance
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_HSR_1_BEGIN, MAC_HSR_1_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_HSR_2:
        {
            //MACAddrLow -> no importance
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_HSR_2_BEGIN, MAC_HSR_2_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_STBY_1:
        {
            //MACAddrLow -> no importance
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_STBY_1_BEGIN, MAC_STBY_1_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        case EDD_MAC_ADDR_GROUP_STBY_2:
        {
            //MACAddrLow -> no importance
            MCFWDCtrlParam=*pMCFWDCtrlParam;
            MCFWDCtrlParam.MACAddrLow=0;
            Status = EDDI_SwiPNSetFWCtrlDAll(&MCFWDCtrlParam, &MAC_STBY_2_BEGIN, MAC_STBY_2_LEN,
                                             LSA_FALSE, pDDB, blTableIndex, EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX);
        }
        break;

        default:
        {
            Status = EDD_STS_ERR_PARAM;
            EDDI_Excp("EDDI_SwiPNMCFwdCtrl", EDDI_FATAL_ERR_EXCP, pMCFWDCtrlParam->MACAddrGroup, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNMCFwdCtrl<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsPTPMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:       all mc-adresses with org-prio must return LSA_TRUE!
*==========================================================================*/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsPTPMAC( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                           EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE    const  pMCMAC )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsPTPMAC->");

    //Compare Sync-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_ANNOUNCE_BEGIN, MAC_PTP_ANNOUNCE_LEN))
    {
        return LSA_FALSE;
    }

    //Compare FollowUp-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_SYNC_WITH_FU_BEGIN, MAC_PTP_TIME_SYNC_WITH_FU_LEN))
    {
        return LSA_TRUE;
    }

    //Compare PTCP-FollowUp-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_FOLLOW_UP_BEGIN, MAC_PTP_TIME_FOLLOW_UP_LEN))
    {
        return LSA_TRUE;
    }

    //Compare Delay-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_LLDP_PTP_DELAY_BEGIN, MAC_LLDP_PTP_DELAY_LEN))
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsPTPMAC<-");

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsCHBMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:       all mc-adresses for nrt-channel B must return LSA_TRUE!
*==========================================================================*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsCHBMAC( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB, 
                                                   EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE    const  pMCMAC )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsCHBMAC->");

    //Compare PTCP-RTSync-with-FollowUp-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_SYNC_WITH_FU_BEGIN, MAC_PTP_TIME_SYNC_WITH_FU_LEN))
    {
        return LSA_TRUE;
    }

    //Compare PTCP-FollowUp-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_FOLLOW_UP_BEGIN, MAC_PTP_TIME_FOLLOW_UP_LEN))
    {
        return LSA_TRUE;
    }

    //Compare PTCP-RTSync-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_SYNC_BEGIN, MAC_PTP_TIME_SYNC_LEN))
    {
        return LSA_TRUE;
    }

    //Compare Delay/LLDP-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_LLDP_PTP_DELAY_BEGIN, MAC_LLDP_PTP_DELAY_LEN))
    {
        return LSA_TRUE;
    }

    //Compare MRP-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_MRP_1_BEGIN, (MAC_MRP_1_LEN+MAC_MRP_2_LEN+MAC_MRP_IC_1_LEN+MAC_MRP_IC_2_LEN)))    //optimization!
    {
        return LSA_TRUE;
    }

    //Compare HSR-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_HSR_1_BEGIN, (MAC_HSR_1_LEN+MAC_HSR_2_LEN)))    //optimization!
    {
        return LSA_TRUE;
    }

    //Compare HSR-STBY1-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_STBY_1_BEGIN, MAC_STBY_1_LEN))
    {
        return LSA_TRUE;
    }

    //Compare HSR-STBY2-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_STBY_2_BEGIN, MAC_STBY_2_LEN))
    {
        return LSA_TRUE;
    }

    //Compare HSYNC-telegram
    if ((EDD_MRP_ROLE_NONE != pDDB->SWITCH.MRPSupportedRole)  &&  EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_HSYNC_BEGIN, MAC_HSYNC_LEN))
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsCHBMAC<-");

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsHSRMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsHSRMAC   (EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const pMCMAC)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsHSRMAC->");

    //Compare HSR1-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_HSR_1_BEGIN, MAC_HSR_1_LEN))
    {
        return LSA_TRUE;
    }

    //Compare HSR2-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_HSR_2_BEGIN, MAC_HSR_2_LEN))
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsHSRMAC<-");

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsDCPMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsDCPMAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                   EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsDCPMAC->");

    //Compare STBY1-telegram
    return EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_DCP_BEGIN, MAC_DCP_LEN);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
* function name: EDDI_SwiPNIsSTBY1MAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsSTBY1MAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
    EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsSTBY1MAC->");

    //Compare STBY1-telegram
    return EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_STBY_1_BEGIN, MAC_STBY_1_LEN);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
* function name: EDDI_SwiPNIsCARPMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsCARPMAC( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                    EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC )
{
    const EDD_MAC_ADR_TYPE    MAC_CARP_BEGIN = { EDDI_MAC_CARP_BEGIN };
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsCARPMAC->");

    //Compare CARP-telegram
    return EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_CARP_BEGIN, MAC_CARP_LEN);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsSTBY2MAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsSTBY2MAC (EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE const  pMCMAC)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsSTBY2MAC->");

    //Compare STBY2-telegram
    return EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_STBY_2_BEGIN, MAC_STBY_2_LEN);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsLLDPMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsLLDPMAC  (EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE const pMCMAC)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsLLDPMAC->");

    //Compare LLDP-telegram
    return EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_LLDP_PTP_DELAY_BEGIN, MAC_LLDP_PTP_DELAY_LEN);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsMRPMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsMRPMAC   (EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsMRPMAC->");

    //Compare MRP-telegram
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_MRP_1_BEGIN, MAC_MRP_1_LEN))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_MRP_2_BEGIN, MAC_MRP_2_LEN))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_MRP_IC_1_BEGIN, MAC_MRP_IC_1_LEN))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_MRP_IC_2_BEGIN, MAC_MRP_IC_2_LEN))
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsMRPMAC<-");

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsSYNCMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsSYNCMAC  (EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsSYNCMAC->");

    //Compare SYNC-telegram
    return EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_PTP_SYNC_BEGIN, MAC_PTP_SYNC_LEN);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsIEEEMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static LSA_BOOL   EDDI_LOCAL_FCT_ATTR EDDI_SwiPNIsIEEEMAC  (EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB, 
                                                            EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE   const  pMCMAC,
                                                            LSA_BOOL                            const  bSetInRun )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsIEEEMAC->");

    //The first address of the rapid tree spanning protocol address-range can be modified by the user (forward/block)
    //Compare IEEE-Reserved+1 -> 1
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, (bSetInRun?&MAC_IEEE_RESERVED_BEGIN_1_1:&MAC_IEEE_RESERVED_BEGIN_1_1), (bSetInRun?MAC_IEEE_RESERVED_LEN_1_1:MAC_IEEE_RESERVED_LEN_1)))
    {
        return LSA_TRUE;
    }

    //Compare IEEE-Reserved - 1
    if (EDDI_SwiPNCompareMAC(pDDB, pMCMAC, &MAC_IEEE_RESERVED_BEGIN_2, MAC_IEEE_RESERVED_LEN_2))
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsIEEEMAC<-");

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsPNMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsPNMAC( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                          EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC )
{
    LSA_UINT32  Index;
    LSA_INT32   retVal;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsPNMAC->");

    retVal = 0L;
    for (Index = 0; Index < MAC_PROFINET_LEN; Index++)
    {
        if (pMCMAC->MacAdr[Index] == MAC_PROFINET.MacAdr[Index])
        {
            continue;
        }

        if (pMCMAC->MacAdr[Index] < MAC_PROFINET.MacAdr[Index])
        {
            retVal = -1L;
        }
        else
        {
            retVal = 1L;
        }
        break;
    }

    if (0L == retVal) //Identisch
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsPNMAC<-");

    LSA_UNUSED_ARG(pDDB);
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNIsMACInFDB()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNIsMACInFDB( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB, 
                                                     EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE  const  pMCMAC,    
                                                     LSA_BOOL                           const  bSetInRun )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsMACInFDB->");

    //The MAC address is entered during EDDI startup

    if (EDDI_SwiPNIsPNMAC(pDDB, pMCMAC))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNIsIEEEMAC(pDDB, pMCMAC, bSetInRun))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNIsMRPMAC(pDDB, pMCMAC))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNIsLLDPMAC(pDDB, pMCMAC))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNIsHSRMAC(pDDB, pMCMAC))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNIsSTBY1MAC(pDDB, pMCMAC))
    {
        return LSA_TRUE;
    }

    if (EDDI_SwiPNIsSTBY2MAC(pDDB, pMCMAC))
    {
        return LSA_TRUE;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNIsMACInFDB<-");

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNSetFWCtrlDAll()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNSetFWCtrlDAll( EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE     const  pMCFWDCtrlParam,
                                                                  EDD_MAC_ADR_TYPE                const  *  const  pMACAdr,
                                                                  LSA_UINT32                                const  MACLen,
                                                                  LSA_BOOL                                  const  checkBLAllowed,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                  LSA_UINT8                                 const  blTableIndex,
                                                                  LSA_UINT16                                const  MACAdrBandIndex )
{
    LSA_UINT32                           UsrPortIndex,PortArrayIdx,Index;
    LSA_RESULT                           Status;
    LSA_UINT8                            MACIndex;
    EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE  GetEntryFDB;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE      UsrSetEntryFDB;
    LSA_BOOL                             bFWDPortOn = LSA_TRUE;

#if defined (EDDI_CFG_REV7)
    LSA_UINT32                           MacTableIdx = 0;
    LSA_BOOL                             IsPTCPMacAdrBandIndex=LSA_FALSE;
#else
    LSA_UNUSED_ARG(blTableIndex); //satisfy lint!
    LSA_UNUSED_ARG(MACAdrBandIndex); //satisfy lint!
    LSA_UNUSED_ARG(checkBLAllowed); //satisfy lint!
#endif


    for (MACIndex = 0; MACIndex < MACLen; MACIndex++)
    {
        EDDI_MemSet(&GetEntryFDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE));
        
        GetEntryFDB.Valid      = EDDI_SWI_FDB_ENTRY_INVALID;
        GetEntryFDB.MACAddress = *pMACAdr;

        //All pPortIDModeArray elements have to have EDD_MULTICAST_FWD_BITMASK set
        if (   (EDD_MULTICAST_FWD_BITMASK         == pMCFWDCtrlParam->pPortIDModeArray[0].Mode)
            || (EDD_MULTICAST_FWD_BITMASK_PLUS_BL == pMCFWDCtrlParam->pPortIDModeArray[0].Mode) )
        {
            GetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE - 1] =
                (LSA_UINT8) (MACIndex + pMACAdr->MacAdr[EDD_MAC_ADDR_SIZE - 1]);
        }
        else
        {
            GetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE - 1] =
                (LSA_UINT8) (pMCFWDCtrlParam->MACAddrLow + MACIndex + pMACAdr->MacAdr[EDD_MAC_ADDR_SIZE - 1]);
        }

        Status = EDDI_SERGetFDBEntryMAC(pDDB, &pDDB->Glob.LLHandle, &GetEntryFDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNSetFWCtrlDAll, EDD_STS_OK <> Status, MACIndex:0x%X MACLen:0x%X MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X",
                MACIndex, MACLen, GetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]);
            return EDD_STS_ERR_PARAM;
        }

        if (EDDI_SWI_FDB_ENTRY_INVALID == GetEntryFDB.Valid)
        {
            EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNSetFWCtrlDAll, LSA_FALSE == GetEntryFDB.Valid, MACIndex:0x%X MACLen:0x%X MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X",
                MACIndex, MACLen, GetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]);
            return EDD_STS_ERR_PARAM;
        }

        if (EDDI_SWI_FDB_ENTRY_DYNAMIC == GetEntryFDB.Type)
        {
            EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNSetFWCtrlDAll, EDDI_SWI_FDB_ENTRY_DYNAMIC == GetEntryFDB.Type, MACIndex:0x%X MACLen:0x%X MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X",
                MACIndex, MACLen, GetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]);
            return EDD_STS_ERR_PARAM;
        }

        EDDI_MemSet(&UsrSetEntryFDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE));

        for ( Index = 0; Index < pDDB->PM.PortMap.PortCnt; Index++)
        {
            UsrSetEntryFDB.PortID[Index] = GetEntryFDB.PortID[Index];
        }

        //set prio for MAC Address
        switch (pMCFWDCtrlParam->MACAddrPrio)
        {
            case EDD_MULTICAST_FWD_PRIO_UNCHANGED:
            {
                UsrSetEntryFDB.Prio = GetEntryFDB.Prio;
            }
            break;
            case EDD_MULTICAST_FWD_PRIO_NORMAL:
            {
                UsrSetEntryFDB.Prio = EDDI_SWI_FDB_PRIO_DEFAULT;
            }
            break;
            case EDD_MULTICAST_FWD_PRIO_HIGH:
            {
                UsrSetEntryFDB.Prio = EDDI_SWI_FDB_PRIO_3;
            }
            break;
            case EDD_MULTICAST_FWD_PRIO_ORG:
            {
                UsrSetEntryFDB.Prio = EDDI_SWI_FDB_PRIO_ORG;
            }
            break;
            default:
            {
                EDDI_Excp("EDDI_SwiPNSetFWCtrlDAll, MACAddrPrio", EDDI_FATAL_ERR_EXCP, pMCFWDCtrlParam->MACAddrPrio, 0);
                return EDD_STS_ERR_EXCP;
            }
        }
        UsrSetEntryFDB.CHA        = GetEntryFDB.CHA;
        UsrSetEntryFDB.CHB        = GetEntryFDB.CHB;
        UsrSetEntryFDB.MACAddress = GetEntryFDB.MACAddress;
        UsrSetEntryFDB.Pause      = GetEntryFDB.Pause;
        UsrSetEntryFDB.Filter     = GetEntryFDB.Filter;
        UsrSetEntryFDB.Borderline = GetEntryFDB.Borderline;

        //evaluate all ports mode to determine if the port should be forwarding or not
        for (PortArrayIdx = 0; PortArrayIdx < pMCFWDCtrlParam->PortIDModeCnt; PortArrayIdx++)
        {
            if (   (EDD_MULTICAST_FWD_BITMASK         == pMCFWDCtrlParam->pPortIDModeArray[PortArrayIdx].Mode)
                || (EDD_MULTICAST_FWD_BITMASK_PLUS_BL == pMCFWDCtrlParam->pPortIDModeArray[PortArrayIdx].Mode))
            {
                //MACAddrLow -> BitMaske
                if (((pMCFWDCtrlParam->MACAddrLow >> MACIndex) & 1UL) == 1UL)
                {
                    bFWDPortOn = LSA_TRUE;
                }
                else
                {
                    bFWDPortOn = LSA_FALSE;
                }
            }
            else
            {
                bFWDPortOn = (LSA_BOOL)(EDD_MULTICAST_FWD_ENABLE == pMCFWDCtrlParam->pPortIDModeArray[PortArrayIdx].Mode ? LSA_TRUE : LSA_FALSE);
            }

            UsrPortIndex = pMCFWDCtrlParam->pPortIDModeArray[PortArrayIdx].DstPortID-1 ;
            //Change forwarding port
            UsrSetEntryFDB.PortID[UsrPortIndex] = (bFWDPortOn) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;

            #if defined (EDDI_CFG_REV7)
            IsPTCPMacAdrBandIndex =   (EDDI_BL_MAC_ADR_BAND_PTCP_SYNC_INDEX == MACAdrBandIndex)
                                    ||(EDDI_BL_MAC_ADR_BAND_PTP_ANNOUNCE_INDEX == MACAdrBandIndex)
                                    ||(EDDI_BL_MAC_ADR_BAND_PTP_SYNC_WITH_FU_INDEX == MACAdrBandIndex)
                                    ||(EDDI_BL_MAC_ADR_BAND_PTP_FOLLOW_UP_INDEX == MACAdrBandIndex);
             MacTableIdx = MACAdrBandIndex*EDDI_MAX_MACADR_BAND_SIZE + MACIndex;

            //map the port mode to the port state and request this port state for only the GSY_OHA user in the borderline table
            //this is just for PTCP MAC entries
            EDDI_SwiSetBLTableforGSY_OHAUsr(pDDB,IsPTCPMacAdrBandIndex,MacTableIdx,
                                            blTableIndex,UsrPortIndex,bFWDPortOn,
                                            pMCFWDCtrlParam->pPortIDModeArray[PortArrayIdx].Mode);
            #endif // (EDDI_CFG_REV7)

        } // end for

    #if defined (EDDI_CFG_REV7)

        UsrSetEntryFDB.Borderline=EDDI_SwiCalculateBLBit( pDDB,IsPTCPMacAdrBandIndex,MacTableIdx,
                                                          blTableIndex,bFWDPortOn,checkBLAllowed,
                                                          GetEntryFDB.Borderline,pMCFWDCtrlParam->pPortIDModeArray[0].Mode);
    #endif // (EDDI_CFG_REV7)



        Status = EDDI_SERSetFDBEntryRun(&pDDB->Glob.LLHandle, &UsrSetEntryFDB, LSA_FALSE, pDDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiPNSetFWCtrlDAll, EDDI_SERSetFDBEntryRun, MACIndex:0x%X MACLen:0x%X MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X Status:0x%X",
                MACIndex, MACLen, GetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1], Status);
            return EDD_STS_ERR_PARAM;
        }
    }

    return EDD_STS_OK;
}

/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiSetBLTableforGSY_OHAUsr()
*
* function: map the port mode to the port state and request this port state for only the GSY_OHA user in the borderline table,
            this is done just if MAC address belongs to to PTCP_SYNC or ANNOUNCE or SYNC_WITH_FU or FOLLOW_UP MAC bands.
*
* parameters:
*   pDDB                    : DDB-Device-Description-Block-structures.
*   IsPTCPMacAdrBandIndex   : if LSA_TRUE the MAC address belongs to PTCP_SYNC or ANNOUNCE or SYNC_WITH_FU or FOLLOW_UP MAC bands
*   MacTableIdx             : index of MAC table entry in the MAC table
*   blTableIndex            : index of the user in the BL table, 
                              possible value: EDDI_BL_USR_GSY_OHA
                                              EDDI_BL_USR_PRM
                                              EDDI_BL_USR_NONE
*   UsrPortIndex            : 
                              possible value: 0
                                              1
                                              2
                                              3
*   bFWDPortOn              : forwarding state for one port
*   PortMode                : 
                              possible value: EDD_MULTICAST_FWD_ENABLE
                                              EDD_MULTICAST_FWD_DISABLE
                                              EDD_MULTICAST_FWD_BITMASK
                                              EDD_MULTICAST_FWD_BITMASK_PLUS_BL
                                              EDD_MULTICAST_FWD_DISABLE_PLUS_BL
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
#if defined (EDDI_CFG_REV7)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiSetBLTableforGSY_OHAUsr( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                                     LSA_BOOL                   const  IsPTCPMacAdrBandIndex,
                                                                     LSA_UINT32                 const  MacTableIdx,
                                                                     LSA_UINT8                  const  blTableIndex,
                                                                     LSA_UINT32                 const  UsrPortIndex,
                                                                     LSA_BOOL                   const  bFWDPortOn,
                                                                     LSA_UINT16                 const  PortMode)
{

    if (   (EDDI_BL_USR_GSY_OHA == blTableIndex)
         && IsPTCPMacAdrBandIndex 
       )
    {
        if    // MCMac shall be forwarded on this port
            (bFWDPortOn)
        {
            pDDB->pLocal_SWITCH->BLTable[EDDI_BL_USR_GSY_OHA][MacTableIdx].PortState[UsrPortIndex] =EDDI_BL_FW;
        }
        else
        {
            if    //FW disabled AND BL shall be set => set borderline-bit
                (    (EDD_MULTICAST_FWD_DISABLE_PLUS_BL == PortMode) 
                  || (EDD_MULTICAST_FWD_BITMASK_PLUS_BL == PortMode)
                 )
            {
                pDDB->pLocal_SWITCH->BLTable[EDDI_BL_USR_GSY_OHA][MacTableIdx].PortState[UsrPortIndex] = EDDI_BL_IGRESS_EGRESS;
            }
            else
            {
                pDDB->pLocal_SWITCH->BLTable[EDDI_BL_USR_GSY_OHA][MacTableIdx].PortState[UsrPortIndex] = EDDI_BL_EGRESS;
            }
        }
    }

}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
* function name: EDDI_SwiCalculateBLBit()
*
* function: calculate Borderline bit value for Multicast MAC entry.
            for PTCP_SYNC, ANNOUNCE, SYNC_WITH_FU and FOLLOW_UP MAC the border line bit depends on the Border line table.
            for other MACs the border line depends on checkBLAllowed, DomainBoundaryBLNotAllowed, PortMode, ActuelBorderline and bFWDPortOn
            The Borderline bit is only for SOC1 HW available
            Start                   P0  P1  P2  P3
                    GSY_OHA         e   e   e   x      (e: EDDI_BL_EGRESS) (x: port is not used)
                    PRM             f   f   f   x      (f: EDDI_BL_FW)
                    PortStateMerged e   e   e   x       BL=0
-------------------------------------------------------------------------------------
                    GSY_OHA         e   e   e   x      
                    PRM             f  ie   f   x      (ie: EDDI_BL_IGRESS_EGRESS)
                    PortStateMerged e  ie   e   x       BL=0
-------------------------------------------------------------------------------------
                    GSY_OHA         f   e   e   x      
                    PRM             f  ie   f   x      
                    PortStateMerged f  ie   e   x       BL=0
-------------------------------------------------------------------------------------

                    GSY_OHA         f   e   f   x      
                    PRM             f  ie   f   x      
                    PortStateMerged f  ie   f   x       BL=1
-------------------------------------------------------------------------------------


* parameters:
*   pDDB                    : DDB-Device-Description-Block-structures.
*   IsPTCPMacAdrBandIndex   : if LSA_TRUE the MAC address belongs to PTCP_SYNC or ANNOUNCE or SYNC_WITH_FU or FOLLOW_UP MAC bands
*   MacTableIdx             : index of MAC table entry in the MAC table
*   blTableIndex            : index of the user in the BL table, 
                              possible value: EDDI_BL_USR_GSY_OHA
                                              EDDI_BL_USR_PRM
                                              EDDI_BL_USR_NONE
*   bFWDPortOn              : forwarding state for one port
*   checkBLAllowed          : 
*   ActuelBorderline        : last borderline bit value in the MAC entry
*   PortMode                : 
                              possible value: EDD_MULTICAST_FWD_ENABLE
                                              EDD_MULTICAST_FWD_DISABLE
                                              EDD_MULTICAST_FWD_BITMASK
                                              EDD_MULTICAST_FWD_BITMASK_PLUS_BL
                                              EDD_MULTICAST_FWD_DISABLE_PLUS_BL

* return value: the value of the border line bit
*
* comment:
*==========================================================================*/
#if defined (EDDI_CFG_REV7)
static LSA_UINT8 EDDI_LOCAL_FCT_ATTR EDDI_SwiCalculateBLBit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                             LSA_BOOL                    const  IsPTCPMacAdrBandIndex,
                                                             LSA_UINT32                  const  MacTableIdx,
                                                             LSA_UINT8                   const  blTableIndex,
                                                             LSA_BOOL                    const  bFWDPortOn,
                                                             LSA_BOOL                    const  checkBLAllowed,
                                                             LSA_BOOL                    const  ActuelBorderline,
                                                             LSA_UINT16                  const  PortMode)
{
    LSA_UINT8       ReturnValue=EDD_FEATURE_DISABLE;
    LSA_UINT8       UsrIndex=0;
    LSA_UINT32      UsrPortIndex=0;


    if (   (EDDI_BL_USR_NONE != blTableIndex)
        && (IsPTCPMacAdrBandIndex)
       )
    {
        //Set TableBorderline
        //BL only set, if no entry with ingress=0 and egress=1 at same time. You can see that with 
        //Borderline=0 AND not all used PortID's with 1.

        for (UsrPortIndex = 0 ; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT8  PortStateMerged = EDDI_BL_FW;

            //merge for each port the entries of all users. FWD < EGRESS < IGRESS_EGRESS
            for (UsrIndex = 0; UsrIndex < EDDI_BL_USR_SIZE; UsrIndex++)
            {
                if (PortStateMerged < pDDB->pLocal_SWITCH->BLTable[UsrIndex][MacTableIdx].PortState[UsrPortIndex])
                {
                    PortStateMerged = pDDB->pLocal_SWITCH->BLTable[UsrIndex][MacTableIdx].PortState[UsrPortIndex];
                }
            } 
            if (EDDI_BL_EGRESS == PortStateMerged)
            {
                ReturnValue = EDD_FEATURE_DISABLE;
                break;
            }
            else if (EDDI_BL_IGRESS_EGRESS == PortStateMerged)
            {
                ReturnValue = EDD_FEATURE_ENABLE;
            }
        }
    }
    else
    {
        //determine BL for all other MACAdresses (i.e. RTC2-MC-Boundaries) without taking the history in account
        if //no BL allowed => clear borderline-bit
            (   (checkBLAllowed)
             && (pDDB->PM.DomainBoundaryBLNotAllowed))
        {
            ReturnValue = EDD_FEATURE_DISABLE;
        } 
        else if //FW disabled AND BL shall be set => set borderline-bit
            (   (EDD_MULTICAST_FWD_DISABLE_PLUS_BL == PortMode)
             || (  (EDD_MULTICAST_FWD_BITMASK_PLUS_BL == PortMode)
                 && (!bFWDPortOn)
                )
            )
        {
            ReturnValue = EDD_FEATURE_ENABLE;
        }
        else if //FW disabled (other reasons) AND BL already set => clear borderline-bit because no history can be taken into account
            (!bFWDPortOn && ActuelBorderline)
        {
            ReturnValue = EDD_FEATURE_DISABLE;
        }
        else
        {
            //(FW disabled) AND (BL already set (only possible during PRM)) ==> keep BL
            //FW enabled AND borderline-bit already set => keep borderline-bit
        }
    }

    return ReturnValue;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/
/*=============================================================================
* function name: EDDI_SwiPNCompareMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_BOOL
*
* comment:
*==========================================================================*/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNCompareMAC( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                             EDD_MAC_ADR_TYPE   const *  const  pMCMACToCompare,
                                                             EDD_MAC_ADR_TYPE   const *  const  pMCMACPtp,
                                                             LSA_UINT32                  const  MCPtpLen )
{
    LSA_UINT8         Index;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNCompareMAC->");

    //compare B0..B4 of macaddr
    for (Index = 0;  Index < (EDD_MAC_ADDR_SIZE-1); Index++)
    {
        if (pMCMACToCompare->MacAdr[Index] != pMCMACPtp->MacAdr[Index])
        {
            //failed
            EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNCompareMAC<- compare failed");
            return (LSA_FALSE);
        }
    }

    //compare B5 of macaddr
    {
        LSA_UINT32        const MCMACToCompareB5 = (LSA_UINT32)pMCMACToCompare->MacAdr[EDD_MAC_ADDR_SIZE-1];
        LSA_UINT32        const MCMACPtpB5       = (LSA_UINT32)pMCMACPtp->MacAdr[EDD_MAC_ADDR_SIZE-1];

        if (   (MCMACToCompareB5 >= MCMACPtpB5)
            && (MCMACToCompareB5 < (MCMACPtpB5 + MCPtpLen)) )
        {
            //succeeded
            return (LSA_TRUE);
        }
    }
    
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNCompareMAC<- compare failed");

    LSA_UNUSED_ARG(pDDB);
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNGetRT3DA()
*
* function:
*
* parameters:
*
* return value:  EDD_MAC_ADR_TYPE
*
* comment:
*==========================================================================*/
EDD_MAC_ADR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNGetRT3DA( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNGetRT3DA->");
    LSA_UNUSED_ARG(pDDB);
    return MAC_RT_3_DA_BEGIN;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNGetRT3INVALIDDA()
*
* function:
*
* parameters:
*
* return value:  EDD_MAC_ADR_TYPE
*
* comment:
*==========================================================================*/
EDD_MAC_ADR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNGetRT3INVALIDDA( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNGetRT3INVALIDDA->");
    LSA_UNUSED_ARG(pDDB);
    return MAC_RT_3_INVALID_DA_BEGIN;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiPNGetPTPSYNCDA()
*
* function:
*
* parameters:
*
* return value:  EDD_MAC_ADR_TYPE
*
* comment:
*==========================================================================*/
EDD_MAC_ADR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNGetPTPSYNCDA( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiPNGetPTPSYNCDA->");
    LSA_UNUSED_ARG(pDDB);
    return MAC_PTP_SYNC_BEGIN;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*****************************************************************************/
/*  end of file eddi_swi_ptp.c                                               */
/*****************************************************************************/
