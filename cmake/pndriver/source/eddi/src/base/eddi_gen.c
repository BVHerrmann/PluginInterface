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
/*  F i l e               &F: eddi_gen.c                                :F&  */
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
/*  14.07.07    JS    Added support for EDD_SRV_SET_REMOTE_PORT_STATE and    */
/*                    EDD_SRV_SET_SYNC_STATE                                 */
/*  07.11.07    JS    Added support PortOnlyHasSyncTx                        */
/*  26.11.07    JS    EDDI_GenDisableMC() static removed (now public)        */
/*  08.01.07    JS    removed EDDI_SRV_SET_MRP_STATE service                 */
/*  11.02.07    JS    added IsWireless                                       */
/*  10.04.08    AB    added IsMDIX                                           */
/*  03.12.08    AH    added new statistics EDDI_GenGetStatistics()           */
/*                                         EDDI_GenResetStatistics()         */
/*                    remove old statistics functions                        */
/*  19.10.09    UL    removed support PortOnlyHasSyncTx                      */
/*  06.02.15    TH    Added PhyStatus for EDD_SRV_GET_PORT_PARAMS            */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_ext.h"

#include "eddi_swi_ext.h"
#include "eddi_ser_cmd.h"

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_udp.h"
#endif

#include "eddi_sync_usr.h"

#include "eddi_time.h"
#include "eddi_Tra.h"
//#include "eddi_crt_ext.h"

//#include "eddi_ser_ext.h"
//#include "eddi_ser.h"
//#include "eddi_crt_com.h"
#include "eddi_sync_ir.h"
#include "eddi_lock.h"

#define EDDI_MODULE_ID     M_ID_EDDI_GEN
#define LTRC_ACT_MODUL_ID  12

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                 definition                                */
/*===========================================================================*/
typedef struct _EDDI_DYN_LIST_DATA_MAC_TYPE
{
    EDD_MAC_ADR_TYPE MACAddr;
    LSA_UINT32       RefCnt;

} EDDI_DYN_LIST_DATA_MAC_TYPE;

typedef struct _EDDI_DYN_LIST_DATA_MAC_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_DYN_LIST_DATA_MAC_PTR_TYPE;

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenLEDBlink( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                        EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenMCFWDCtrl( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenSetLineDelay( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                            EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenGetPortParams( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                             EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenGetParam ( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenGetLinkStatus ( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB );


static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenLinkIndProvide( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                              EDDI_LOCAL_HDB_PTR_TYPE const pHDB,
                                                              LSA_BOOL                    * bIndicate );

static LSA_UINT   EDDI_LOCAL_FCT_ATTR EDDI_GenGetRatingSpeedMode( EDDI_LOCAL_DDB_PTR_TYPE        const pDDB,
                                                                  SWI_LINK_STAT_TYPE    const *  const pBestLinkState );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenGetBestAutoLinkStatus( EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                                     SWI_LINK_STAT_TYPE      * const pBestLinkState );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenGetBestPortIDLinkStatus( LSA_UINT32                const HwPortIndex,
                                                                       SWI_LINK_STAT_TYPE      * const pBestLinkState,
                                                                       EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenIncRefCntMC( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                           EDDI_DYN_LIST_HEAD_PTR_TYPE       pListHeader,
                                                           EDDI_LOCAL_MAC_ADR_PTR_TYPE       pMACAdr,
                                                           LSA_UINT32                      * pRefCnt );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenDecRefCntMC( EDDI_LOCAL_DDB_PTR_TYPE      const pDDB,
                                                              EDDI_DYN_LIST_HEAD_PTR_TYPE        pListHeader,
                                                              EDDI_LOCAL_MAC_ADR_PTR_TYPE        pMACAdr,
                                                              LSA_BOOL                         * pEntryExist,
                                                              LSA_UINT32                       * pRefCnt );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenSetupPhy( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_BOOL                     *  bIndicate );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenDelAllfCntMC( EDDI_DYN_LIST_HEAD_PTR_TYPE       pListHeader,
                                                            EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenDisableMC( EDDI_LOCAL_MAC_ADR_PTR_TYPE const pMACAdr,
                                                         EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenEnableMC( EDDI_LOCAL_MAC_ADR_PTR_TYPE const pMACAdr,
                                                        EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                        LSA_BOOL                    const bLocalReceptionOnly);

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_GenSetIP( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                     EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenMC( EDD_UPPER_MULTICAST_PTR_TYPE const pMCParam,
                                                  EDDI_LOCAL_DDB_PTR_TYPE      const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenSetRemotePortState( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                                  EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenSetSyncState( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                            EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenGetStatistics( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                             EDDI_LOCAL_HDB_PTR_TYPE const pHDB );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenResetStatistics( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                  EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                                  LSA_BOOL                 const  bUseShadowRegister );
     
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenResetRAWStatistics( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                                   LSA_UINT32                const  HwPortIndex );
                                                                                                                                     

/***************************************************************************/
/* F u n c t i o n:       EDDI_GenDisableMC()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenDisableMC( EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pMACAdr,
                                                    EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_RESULT                       Status;
    LSA_BOOL                         bValid;
    LSA_BOOL                         bDestSameMCDef;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE  UsrSetEntryFDB;

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDisableMC->");

    //Compare MC_Def and Destport and read current settings from FDB
    Status = EDDI_GenGetDestPortMCDef(pMACAdr, &bDestSameMCDef, &bValid, &UsrSetEntryFDB, pDDB);
    if (EDD_STS_OK != Status)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDisableMC, Status:0x%X", Status);
        return Status;
    }

    if (!bValid) //No entry found
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDisableMC, LSA_FALSE == bValid, Status:0x%X", Status);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ENTRY_NOT_EXIST);
        return EDD_STS_ERR_PARAM;
    }

    //do not take PNIO-MAC from FDB
    if (   (bDestSameMCDef)
        && (!EDDI_SwiPNIsMACInFDB(pDDB, pMACAdr, LSA_FALSE /*bSetInRun*/)))
    {
        //Destport is equal to MC-Defaultport -> FDB-entry for MC can be deleted
        //Forwarding is not changed
        //for the PTP-MC only the local port is allowed to be blocked!!!
        EDDI_RQB_SWI_REMOVE_FDB_ENTRY_TYPE RemoveEntryFDB;

        EDDI_MemSet(&RemoveEntryFDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_REMOVE_FDB_ENTRY_TYPE));

        RemoveEntryFDB.MACAddress    = *pMACAdr;
        RemoveEntryFDB.FDBEntryExist = EDDI_SWI_FDB_ENTRY_NOT_EXIST;

        Status = EDDI_SERRemoveFDBEntry(&pDDB->Glob.LLHandle, &RemoveEntryFDB, pDDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDisableMC, EDDI_SERRemoveFDBEntry, Status:0x%X", Status);
            return Status;
        }

        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenDisableMC, RemoveEntryFDB.FDBEntryExist:0x%X", RemoveEntryFDB.FDBEntryExist);

        //MC-MAC does not exist
        if (EDDI_SWI_FDB_ENTRY_NOT_EXIST == RemoveEntryFDB.FDBEntryExist)
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_GenDisableMC, LSA_FALSE == RemoveEntryFDB.FDBEntryExist, Status:0x%X", Status);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ENTRY_NOT_EXIST);
            return EDD_STS_ERR_PARAM;
        }
    }
    else
    {
        UsrSetEntryFDB.CHA = EDD_FEATURE_DISABLE; //block local reception
        UsrSetEntryFDB.CHB = EDD_FEATURE_DISABLE; //block local reception

        //user view no port mapping

        Status = EDDI_SERSetFDBEntryRun(&pDDB->Glob.LLHandle, &UsrSetEntryFDB, LSA_FALSE, pDDB);
        if (EDD_STS_OK != Status)
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDisableMC, EDDI_SERSetFDBEntryRun, Status:0x%X", Status);
            return Status;
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDisableMC<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenEnableMC()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenEnableMC( EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pMACAdr,
                                                           EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                           LSA_BOOL                     const  bLocalReceptionOnly)
{
    LSA_UINT32                       UsrPortIndex;
    LSA_RESULT                       Status;
    LSA_BOOL                         bValid;
    LSA_BOOL                         bDestSameMCDef;
    EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE  UsrSetEntryFDB;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenEnableMC->");

    //Compare MC_Def and Destport and read current settings from FDB
    Status = EDDI_GenGetDestPortMCDef(pMACAdr, &bDestSameMCDef, &bValid, &UsrSetEntryFDB, pDDB);
    if (EDD_STS_OK != Status)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenEnableMC - EDDI_GenGetDestPortMCDef, Status:0x%X", Status);
        return Status;
    }

    if (!bValid) //No entry found
    {
        LSA_UINT32                            MCDefaultCtrl;
        LSA_UINT32                            DestPort;
        LSA_UINT32                     const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        if (bLocalReceptionOnly)
        {
            for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
            {
                UsrSetEntryFDB.PortID[UsrPortIndex] = EDD_FEATURE_DISABLE;
            }
        }
        else
        {
            //set Destport = MCDefault
            MCDefaultCtrl = IO_R32(MC_DEFAULT_CTRL);

            for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
            {
                DestPort = EDDI_PmGetMask0420(pDDB, UsrPortIndex);
            UsrSetEntryFDB.PortID[UsrPortIndex] = (0 != (DestPort & MCDefaultCtrl) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE);
            }
        }

        UsrSetEntryFDB.CHA        = EDD_FEATURE_DISABLE;
        UsrSetEntryFDB.CHB        = EDD_FEATURE_DISABLE;

        UsrSetEntryFDB.Prio       = EDDI_SwiPNGetPrio(pDDB,pMACAdr);  //get priority 
        UsrSetEntryFDB.Pause      = EDD_FEATURE_DISABLE;
        UsrSetEntryFDB.Filter     = EDD_FEATURE_DISABLE;
        UsrSetEntryFDB.MACAddress = *pMACAdr;
        UsrSetEntryFDB.Borderline = EDD_FEATURE_DISABLE;
    }

    if //MC-address for nrt-channel B
       (EDDI_SwiPNIsCHBMAC(pDDB, pMACAdr))
    {
        UsrSetEntryFDB.CHB = EDD_FEATURE_ENABLE; // enable local reception for channel B
    }
    else //another MC-address
    {
        UsrSetEntryFDB.CHA = EDD_FEATURE_ENABLE; // enable local reception for channel A
    }

    /* user view no port mapping */

    Status = EDDI_SERSetFDBEntryRun(&pDDB->Glob.LLHandle, &UsrSetEntryFDB, LSA_FALSE, pDDB);

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenEnableMC<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenGetDestPortMCDef()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetDestPortMCDef( EDDI_LOCAL_MAC_ADR_PTR_TYPE            const  pMACAdr,
                                                           LSA_BOOL                                   *  pDestSameMCDef,
                                                           LSA_BOOL                                   *  pbFDBExist,
                                                           EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE         pUsrSetEntryFDB,
                                                           EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB )
{
    LSA_RESULT                                  Status;
    LSA_UINT32                                  MCDefCnt;
    LSA_UINT32                                  DestPort;
    LSA_UINT32                                  UsrPortIndex;
    EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE         UsrGetEntryFDB;
    LSA_UINT32                                  MCDefaultCtrl;
    LSA_UINT8                                   bMCDefault;
    LSA_UINT32                           const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetDestPortMCDef->");

    *pDestSameMCDef = LSA_FALSE;
    *pbFDBExist     = LSA_FALSE;

    EDDI_MemSet(&UsrGetEntryFDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE));

    UsrGetEntryFDB.MACAddress = *pMACAdr;
    UsrGetEntryFDB.Valid      = EDDI_SWI_FDB_ENTRY_INVALID;

    Status = EDDI_SERGetFDBEntryMAC(pDDB, &pDDB->Glob.LLHandle, &UsrGetEntryFDB);
    if (EDD_STS_OK != Status)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenGetDestPortMCDef - EDDI_SERGetFDBEntryMAC, UsrGetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X",
                              UsrGetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_FROM_FUNCTION);
        return EDD_STS_ERR_PARAM;
    }
    if (EDDI_SWI_FDB_ENTRY_DYNAMIC == UsrGetEntryFDB.Type)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenGetDestPortMCDef, EDDI_SWI_FDB_ENTRY_DYNAMIC == UsrGetEntryFDB.Type, UsrGetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X",
                              UsrGetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TYPE);
        return EDD_STS_ERR_PARAM;
    }
    if (EDDI_SWI_FDB_ENTRY_INVALID == UsrGetEntryFDB.Valid)
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                              "EDDI_GenGetDestPortMCDef, LSA_FALSE == UsrGetEntryFDB.Valid, UsrGetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X",
                              UsrGetEntryFDB.MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]);
        return EDD_STS_OK;
    }

    *pbFDBExist = LSA_TRUE;

    MCDefaultCtrl = IO_R32(MC_DEFAULT_CTRL);

    MCDefCnt = 0x00;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        DestPort = EDDI_PmGetMask0420(pDDB, UsrPortIndex);

        bMCDefault = (0 != (DestPort & MCDefaultCtrl) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE);

        if (UsrGetEntryFDB.PortID[UsrPortIndex] == bMCDefault)
        {
            MCDefCnt++;
        }
    }

    if (MCDefCnt == PortMapCnt)
    {
        *pDestSameMCDef = LSA_TRUE;
    }

    EDDI_MemSet(pUsrSetEntryFDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE));

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        pUsrSetEntryFDB->PortID[UsrPortIndex] = UsrGetEntryFDB.PortID[UsrPortIndex];
    }

    pUsrSetEntryFDB->MACAddress = *pMACAdr;
    pUsrSetEntryFDB->CHA        = UsrGetEntryFDB.CHA;
    pUsrSetEntryFDB->CHB        = UsrGetEntryFDB.CHB;
    pUsrSetEntryFDB->Prio       = UsrGetEntryFDB.Prio;
    pUsrSetEntryFDB->Pause      = UsrGetEntryFDB.Pause;
    pUsrSetEntryFDB->Filter     = UsrGetEntryFDB.Filter;
    pUsrSetEntryFDB->Borderline = UsrGetEntryFDB.Borderline;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenIncRefCntMC()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_GenIncRefCntMC( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                           EDDI_DYN_LIST_HEAD_PTR_TYPE          pListHeader,
                                                           EDDI_LOCAL_MAC_ADR_PTR_TYPE          pMACAdr,
                                                           LSA_UINT32                         * pRefCnt )
{
    LSA_RESULT                       Status;
    EDDI_DYN_LIST_DATA_MAC_PTR_TYPE  pListMACEntry;
    EDDI_DYN_LIST_ENTRY_PTR_TYPE     pListTop;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenIncRefCntMC->");

    *pRefCnt = 0;

    pListTop = pListHeader->pTop;

    do
    {
        //get data-entry from list
        Status = EDDI_GetTopFromDynList(pDDB, &pListTop, (LSA_VOID_PTR_TYPE  *)&pListMACEntry);
        if (EDD_STS_OK != Status)
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenIncRefCntMC - EDDI_GetTopFromDynList");
            return Status;
        }

        if (!(pListMACEntry == EDDI_NULL_PTR))
        {
            //entry with matching MACAdr found
            if (0 == EDDI_SwiUcCompareMac(pDDB, pMACAdr, &pListMACEntry->MACAddr))
            {
                pListMACEntry->RefCnt++;

                *pRefCnt = pListMACEntry->RefCnt;

                EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                      "EDDI_GenIncRefCntMC, Not new. RefCnt:0x%X pListMACEntry:0x%X pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE-1]:0x%X pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE-2]:0x%X",
                                      pListMACEntry->RefCnt, (LSA_UINT32)pListMACEntry, pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 1], pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 2]);
                return EDD_STS_OK;
            }
        }
    }
    while (!(pListMACEntry == EDDI_NULL_PTR));

    //add new entry, no matching MACAdr found
    EDDI_AllocLocalMem((void * *)&pListMACEntry, (LSA_UINT32)sizeof(EDDI_DYN_LIST_DATA_MAC_TYPE));

    if (pListMACEntry == EDDI_NULL_PTR)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenIncRefCntMC, pListMACEntry == EDDI_NULL_PTR");
        return LSA_RET_ERR_RESOURCE;
    }

    EDDI_MemSet(pListMACEntry, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DYN_LIST_DATA_MAC_TYPE));

    pListMACEntry->RefCnt  = 1;
    pListMACEntry->MACAddr = *pMACAdr;

    *pRefCnt = pListMACEntry->RefCnt;

    Status = EDDI_InsertBottomToDynList(pDDB, pListHeader, pListMACEntry);
    if (EDD_STS_OK != Status)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenIncRefCntMC - EDDI_InsertBottomToDynList");
        return Status;
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenDecRefCntMC()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenDecRefCntMC( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                              EDDI_DYN_LIST_HEAD_PTR_TYPE         pListHeader,
                                                              EDDI_LOCAL_MAC_ADR_PTR_TYPE         pMACAdr,
                                                              LSA_BOOL                         *  pEntryExist,
                                                              LSA_UINT32                       *  pRefCnt )
{
    LSA_UINT16                       RetVal;
    LSA_RESULT                       Status;
    EDDI_DYN_LIST_DATA_MAC_PTR_TYPE  pListMACEntry;
    EDDI_DYN_LIST_ENTRY_PTR_TYPE     pListTopNext;
    EDDI_DYN_LIST_ENTRY_PTR_TYPE     pListTopOldNext;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDecRefCntMC->");

    *pRefCnt     = 0;
    *pEntryExist = LSA_FALSE;
    pListTopNext = pListHeader->pTop;

    do
    {
        pListTopOldNext = pListTopNext;

        //get data entry from list
        Status = EDDI_GetTopFromDynList(pDDB, &pListTopNext, (LSA_VOID_PTR_TYPE  *)&pListMACEntry);
        if (EDD_STS_OK != Status)
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDecRefCntMC - EDDI_GetTopFromDynList");
            return Status;
        }

        if (!(pListMACEntry == EDDI_NULL_PTR))
        {
            //entry with matching MACAdr found
            if (0 == EDDI_SwiUcCompareMac(pDDB, pMACAdr, &pListMACEntry->MACAddr))
            {
                *pEntryExist = LSA_TRUE;

                pListMACEntry->RefCnt--;

                *pRefCnt = pListMACEntry->RefCnt;

                if (pListMACEntry->RefCnt == 0)
                {
                    //get data entry from list
                    Status = EDDI_RemoveFromDynList(pDDB, pListHeader, pListTopOldNext);
                    if (EDD_STS_OK != Status)
                    {
                        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDecRefCntMC - EDDI_RemoveBottomFromDynList");
                        return Status;
                    }

                    EDDI_FREE_LOCAL_MEM(&RetVal, pListMACEntry);
                    if (EDD_STS_OK != RetVal)
                    {
                        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                              "EDDI_GenDecRefCntMC, RetVal:0x%X", RetVal);
                        return RetVal;
                    }
                }
                return EDD_STS_OK;
            }
        }
    }
    while (!(pListTopNext == EDDI_NULL_PTR));

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDecRefCntMC<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenDelAllfCntMC                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenDelAllfCntMC( EDDI_DYN_LIST_HEAD_PTR_TYPE         pListHeader,
                                                               EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB )
{
    LSA_UINT16                       RetVal;
    LSA_RESULT                       Status;
    EDDI_DYN_LIST_DATA_MAC_PTR_TYPE  pListMACEntry;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDelAllfCntMC->");

    do
    {
        //get data entry from list
        Status = EDDI_RemoveBottomFromDynList(pDDB, pListHeader, (LSA_VOID_PTR_TYPE  *)&pListMACEntry);
        if (EDD_STS_OK != Status)
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDelAllfCntMC - EDDI_RemoveBottomFromDynList");
            return Status;
        }

        if (!(pListMACEntry == EDDI_NULL_PTR))
        {
            EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                  "EDDI_GenDelAllfCntMC, pListMACEntry->RefCnt:0x%X (LSA_UINT32)pListMACEntry:0x%X, pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 1]:0x%X pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 2]:0x%X",
                                  pListMACEntry->RefCnt, (LSA_UINT32)pListMACEntry, pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 1], pListMACEntry->MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 2]);

            (void)EDDI_GenDisableMC(&pListMACEntry->MACAddr, pDDB);

            EDDI_FREE_LOCAL_MEM(&RetVal, pListMACEntry);
            if (EDD_STS_OK != RetVal)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenDelAllfCntMC, RetVal:0x%X", RetVal);
                return RetVal;
            }
        }
    }
    while (!(pListMACEntry == EDDI_NULL_PTR));

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDelAllfCntMC");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenMC()                                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenMC( EDD_UPPER_MULTICAST_PTR_TYPE  const  pMCParam,
                                                     EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB )
{
    LSA_RESULT  Status;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenMC->");

    //only allow MC-MAC if EDD_MULTICAST_DISABLE_ALL is NOT set
    if ((EDD_MULTICAST_DISABLE_ALL != pMCParam->Mode) &&
        ((pMCParam->MACAddr.MacAdr[0] & SWI_MULTICAST_ADR) == 0))
    {
        //Special case MRP: UC-Address 00:80:63:07:10:00 is handled like a MC-MACAdr
        if (EDDI_SwiPNIsSTBY1MAC(pDDB, &pMCParam->MACAddr))
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenMC, 00:80:63:07:10:00");
        }
        else if (   (EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ClusterIPSupport)
                 && (EDDI_SwiPNIsCARPMAC(pDDB, &pMCParam->MACAddr) ) )
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenMC, 00:00:5E:00:01:00");
        }
        else
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenMC, NO MC-MAC");
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TYPE);
            return EDD_STS_ERR_PARAM;
        }
    }

    switch (pMCParam->Mode)
    {
        case EDD_MULTICAST_ENABLE: //enable local reception, forwarding is not to be influenced
        {
            //inc. RefCounter by 1
            Status = EDDI_GenIncRefCntMC(pDDB, &pDDB->Glob.MCMACTabelleLocal, &pMCParam->MACAddr, &pMCParam->RefCnt);
            if (EDD_STS_OK != Status)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenMC, EDDI_GenIncRefCntMC -> Status:0x%X", Status);
                return Status;
            }

            if (pMCParam->RefCnt > 1) //"EDD_MULTICAST_ENABLE" has already been called before for this MC-MACAdr -> no action
            {
                return EDD_STS_OK;
            }

            EDDI_ENTER_SYNC_S();
            Status = EDDI_GenEnableMC(&pMCParam->MACAddr, pDDB, LSA_FALSE /*bLocalReceptionOnly*/);
            EDDI_EXIT_SYNC_S();
            if (EDD_STS_OK != Status)
            {
                LSA_BOOL  bEntryExist;
                //remove entry
                (void)EDDI_GenDecRefCntMC(pDDB, &pDDB->Glob.MCMACTabelleLocal, &pMCParam->MACAddr, &bEntryExist, &pMCParam->RefCnt);
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_GenMC, EDDI_GenEnableMC, pMCParam->RefCnt -> Status:0x%X pMCParam->RefCnt:0x%X",
                                      Status, pMCParam->RefCnt);
            }
        }
        break;

        case EDD_MULTICAST_DISABLE: //enable local reception, forwarding is not to be influenced
        {
            LSA_BOOL  bEntryExist;

            //dec. RefCounter by 1
            Status = EDDI_GenDecRefCntMC(pDDB, &pDDB->Glob.MCMACTabelleLocal, &pMCParam->MACAddr, &bEntryExist, &pMCParam->RefCnt);
            if (EDD_STS_OK != Status)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenMC - EDDI_GenDecRefCntMC, Status:0x%X", Status);
                return Status;
            }

            //"EDD_MULTICAST_ENABLE" has not been called yet for this MC-MACAdr
            if (!bEntryExist)
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ENTRY_NOT_EXIST);
                return EDD_STS_ERR_PARAM; //AS meldet auch zurueck
            }

            //As long as EDD_MULTICAST_ENABLE and EDD_MULTICAST_DISABLE have not been called symmetrically -> no action
            if (pMCParam->RefCnt > 0)
            {
                return EDD_STS_OK;
            }

            //EDD_MULTICAST_ENABLE and EDD_MULTICAST_DISABLE have been called symmetrically:
            //enable local reception for MC-MACAddresses, forwarding is not to be influenced
            EDDI_ENTER_SYNC_S();
            Status = EDDI_GenDisableMC(&pMCParam->MACAddr, pDDB);
            EDDI_EXIT_SYNC_S();
            if (EDD_STS_OK != Status)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenMC - EDDI_GenDisableMC, Status:0x%X", Status);
            }
        }
        break;

        case EDD_MULTICAST_DISABLE_ALL:
        {
            pMCParam->RefCnt = 0;
            EDDI_ENTER_SYNC_S();
            Status =  EDDI_GenDelAllDynMCMACTab(pDDB);
            EDDI_EXIT_SYNC_S();
        }
        break;

        default:
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PARAM);
            Status = EDD_STS_ERR_PARAM;
        }
        break;
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenMC<-");

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenMCFWDCtrl()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenMCFWDCtrl( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                      Result;
    EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE           pMCFWDCtrlParam;
    LSA_UINT32                                      HwPortIndex;
    LSA_UINT16                                      MACAddrPrio;
    LSA_UINT16                                      ArrayIndex;
    LSA_UINT16                                      BitMaskCnt = 0;
    EDD_RQB_MULTICAST_FWD_CTRL_TYPE                 MCFWDCtrlParam;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenMCFWDCtrl->");

    pMCFWDCtrlParam = (EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE) pRQB->pParam;

    EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenMCFWDCtrl, PortIDModeCnt:0x%X MACAddrGroup:0x%X MACAddrLow:0x%X",
                          pMCFWDCtrlParam->PortIDModeCnt, pMCFWDCtrlParam->MACAddrGroup, pMCFWDCtrlParam->MACAddrLow);

    #if defined (EDDI_CFG_ERTEC_400)  //ERTEC_400: LineDelay is added at the sender (not according to standard!)
    if (   (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_PTCP)
        || (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_PTCP_ALL))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenMCFWDCtrl MACAddrGroup, pMCFWDCtrlParam->MACAddrGroup:0x%X",
                              pMCFWDCtrlParam->MACAddrGroup);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_MAC_ADDRGROUP);
        return EDD_STS_ERR_NOT_ALLOWED;
    }
    #endif

    if ( (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_PTCP       ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_PTCP_ALL   ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_HSR_1      ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_HSR_2      ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_STBY_1     ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_STBY_2     ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_MRP_1      ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_MRP_2      ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_MRP_IN_1   ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_MRP_IN_2   ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_HSYNC      ) &&
         (pMCFWDCtrlParam->MACAddrGroup != EDD_MAC_ADDR_GROUP_RT_2_QUER  ) )
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenMCFWDCtrl MACAddrGroup, pMCFWDCtrlParam->MACAddrGroup:0x%X",
                              pMCFWDCtrlParam->MACAddrGroup);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_MAC_ADDRGROUP);
        return EDD_STS_ERR_PARAM;
    }

    /* Prio only allowed for HSR MAC Address */
    if ( (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_HSR_1      ) ||
         (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_HSR_2      ) ||
         (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_STBY_1     ) ||
         (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_STBY_2     ) )
    {
        switch (pMCFWDCtrlParam->MACAddrPrio)
        {
            case EDD_MULTICAST_FWD_PRIO_UNCHANGED:
            case EDD_MULTICAST_FWD_PRIO_NORMAL:
            case EDD_MULTICAST_FWD_PRIO_HIGH:
            case EDD_MULTICAST_FWD_PRIO_ORG:
            {
                MACAddrPrio = pMCFWDCtrlParam->MACAddrPrio;
            }
            break;
            default:
            {
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_MAC_ADDRPRIO);
                return EDD_STS_ERR_PARAM;
            }
        }
    }
    else if (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_HSYNC)
    {
        if ((EDD_MULTICAST_FWD_PRIO_UNCHANGED == pMCFWDCtrlParam->MACAddrPrio) ||
			(EDD_MULTICAST_FWD_PRIO_ORG		  == pMCFWDCtrlParam->MACAddrPrio))
        {
            MACAddrPrio = pMCFWDCtrlParam->MACAddrPrio;
        }
        else
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_MAC_ADDRPRIO);
            return EDD_STS_ERR_PARAM;
        }
    }
    else
    {
        MACAddrPrio = EDD_MULTICAST_FWD_PRIO_UNCHANGED;
    }

    if (LSA_HOST_PTR_ARE_EQUAL(pMCFWDCtrlParam->pPortIDModeArray, LSA_NULL))
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenMCFWDCtrl pPortIDModeArray == LSA_NULL");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ID_MODEARRAY);
        return EDD_STS_ERR_PARAM;
    }

    if ((0 == pMCFWDCtrlParam->PortIDModeCnt) || (pMCFWDCtrlParam->PortIDModeCnt > pDDB->PM.PortMap.PortCnt))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenMCFWDCtrl PortIDModeCnt -> pMCFWDCtrlParam->PortIDModeCnt:0x%X",
                              pMCFWDCtrlParam->PortIDModeCnt);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ID_MODECNT);
        return EDD_STS_ERR_PARAM;
    }

    for (ArrayIndex = 0; ArrayIndex < pMCFWDCtrlParam->PortIDModeCnt; ArrayIndex++)
    {
        if ((pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode != EDD_MULTICAST_FWD_ENABLE)  &&
            (pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode != EDD_MULTICAST_FWD_DISABLE) &&
            (pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode != EDD_MULTICAST_FWD_BITMASK))
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_GenMCFWDCtrl Mode, ArrayIndex -> pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode:0x%X ArrayIndex:0x%X",
                                  pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode, ArrayIndex);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_WRONG_MODE);
            return EDD_STS_ERR_PARAM;
        }

        Result = EDDI_GenChangeUsrPortID2HwPortIndex(pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].DstPortID,
                                                     pDDB,
                                                     &HwPortIndex);
        if (EDD_STS_OK != Result)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            return Result;
        }

        //EDD_MULTICAST_FWD_BITMASK is only allowed for EDD_MAC_ADDR_GROUP_RT_2_QUER
        if ( ((pMCFWDCtrlParam->MACAddrGroup                      == EDD_MAC_ADDR_GROUP_RT_2_QUER) &&
              (pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode != EDD_MULTICAST_FWD_BITMASK)  )  ||
             ((pMCFWDCtrlParam->MACAddrGroup                      != EDD_MAC_ADDR_GROUP_RT_2_QUER) &&
              (pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode == EDD_MULTICAST_FWD_BITMASK)) )
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_GenMCFWDCtrl Mode, ArrayIndex -> pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode:0x%X ArrayIndex:0x%X",
                                  pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode, ArrayIndex);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_BITMASK_GROUP);
            return EDD_STS_ERR_PARAM;
        }

        if (pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode == EDD_MULTICAST_FWD_BITMASK)
        {
            BitMaskCnt++;
        }
        else        // its  'EDD_MULTICAST_FWD_ENABLE'  OR  'EDD_MULTICAST_FWD_DISABLE'
        {
            if ( (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_HSYNC)  &&  (pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].SrcPortID != EDD_PORT_ID_AUTO))
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                      "EDDI_GenMCFWDCtrl SrcPortID, ArrayIndex -> pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].SrcPortID:0x%X ArrayIndex:0x%X",
                                      pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].SrcPortID, ArrayIndex);
                EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_HSYNC_SRC_PORTID);
                return EDD_STS_ERR_PARAM;
            }
        }
    }

    //If 1 parameter is set only, all have to have EDD_MULTICAST_FWD_BITMASK set
    if (   (BitMaskCnt)
        && (BitMaskCnt != pMCFWDCtrlParam->PortIDModeCnt))
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenMCFWDCtrl Mode, ArrayIndex -> pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode:0x%X ArrayIndex:0x%X",
                              pMCFWDCtrlParam->pPortIDModeArray[ArrayIndex].Mode, ArrayIndex);
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_BITMASK_PORTCNT);
        return EDD_STS_ERR_PARAM;
    }

    //No EDD_MULTICAST_FWD_BITMASK
    if (0 == BitMaskCnt)
    {
        if ( (pMCFWDCtrlParam->MACAddrGroup == EDD_MAC_ADDR_GROUP_PTCP) && //use max size of addressrange
             (pMCFWDCtrlParam->MACAddrLow    > 0x1F                   ) )
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                  "EDDI_GenMCFWDCtrl MACAddrLow -> pMCFWDCtrlParam->MACAddrLow:0x%X",
                                  pMCFWDCtrlParam->MACAddrLow);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ADDRLOW_SIZE);
            return EDD_STS_ERR_PARAM;
        }
    }
    pMCFWDCtrlParam->MACAddrPrio=1;
    MCFWDCtrlParam=*pMCFWDCtrlParam;
    MCFWDCtrlParam.MACAddrPrio=MACAddrPrio;
    EDDI_ENTER_SYNC_S();
    Result = EDDI_SwiPNMCFwdCtrl(&MCFWDCtrlParam,
                               pDDB,
                               EDDI_BL_USR_NONE);
    EDDI_EXIT_SYNC_S();
    return (Result);

}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenSetLineDelay()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetLineDelay( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                         Result;
    EDD_UPPER_SET_LINE_DELAY_PTR_TYPE  pSetLineDelayParam;
    LSA_UINT32                         HwPortIndexLineDelay;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetLineDelay->");

    pSetLineDelayParam = (EDD_UPPER_SET_LINE_DELAY_PTR_TYPE) pRQB->pParam;

    Result = EDDI_GenChangeUsrPortID2HwPortIndex(pSetLineDelayParam->PortID, pDDB, &HwPortIndexLineDelay);
    if (EDD_STS_OK != Result)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        return Result;
    }

    EDDI_SyncPortStmsLinedelayChange(pDDB,
                                     HwPortIndexLineDelay,
                                     pSetLineDelayParam->LineDelayInTicksMeasured,
                                     pSetLineDelayParam->CableDelayInNsMeasured);

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetLineDelay<-");

    return EDD_STS_OK;
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetRTClass2PortStatus( LSA_BOOL                 const  bAllPort,
                                                              LSA_UINT32               const  PortIndexRTClass2_PortStatus,
                                                              LSA_UINT8                const  RTClass2_PortStatus,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortIndex;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetRTClass2PortStatus->");

    if (!bAllPort)
    {
        pDDB->pLocal_SWITCH->LinkIndExtPara[PortIndexRTClass2_PortStatus].RTClass2_PortStatus = RTClass2_PortStatus;
    }
    else
    {
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].RTClass2_PortStatus = RTClass2_PortStatus;
        }
    }

    //LinkIndExt "EDDI_GenLinkInterrupt(&PortChanged[0], pDDB)" is triggered later!!

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetRTClass2PortStatus<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenGetPortParams()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetPortParams( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                 Result;
    LSA_UINT32                                 HwPortIndex;
    LSA_UINT32                                 MACIndex;
    SWI_LINK_IND_EXT_PARAM_TYPE             *  pLinkIndExtPara;
    EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE  const  pGetPortParams = (EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE)pRQB->pParam;
    EDDI_PRM_RECORD_IRT_PTR_TYPE        const  pRecordSet     = pDDB->PRM.PDIRData.pRecordSet_A;
    LSA_UINT32                          const  UsrPortIndex   = pGetPortParams->PortID - 1;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetPortParams->");

    Result = EDDI_GenChangeUsrPortID2HwPortIndex(pGetPortParams->PortID, pDDB, &HwPortIndex);
    if (EDD_STS_OK != Result)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        return Result;
    }

    //======== Portspecific MAC-address =======
    for (MACIndex = 0; MACIndex < EDD_MAC_ADDR_SIZE; MACIndex++)
    {
        pGetPortParams->MACAddr.MacAdr[MACIndex] = pDDB->Glob.PortParams[HwPortIndex].MACAddress.MacAdr[MACIndex];
    }

    pLinkIndExtPara = &pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex];

    //for Linedelay the unit is 1 tick!
    pGetPortParams->LineDelayInTicksMeasured        = pLinkIndExtPara->LineDelayInTicksMeasured;
    pGetPortParams->CableDelayInNsMeasured          = pLinkIndExtPara->CableDelayInNsMeasured;
    pGetPortParams->PortStatus                      = pLinkIndExtPara->PortStatus;
    pGetPortParams->PhyStatus                       = pLinkIndExtPara->PhyStatus;
    pGetPortParams->IRTPortStatus                   = pLinkIndExtPara->IRTPortStatus;
    pGetPortParams->RTClass2_PortStatus             = pLinkIndExtPara->RTClass2_PortStatus;
    pGetPortParams->LinkSpeedModeConfigured         = (LSA_UINT8)pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeedMode_Config;
    pGetPortParams->MRPRingPort                     = pDDB->Glob.PortParams[HwPortIndex].MRPRingPort;
    pGetPortParams->SupportsMRPInterconnPortConfig  = pDDB->Glob.PortParams[HwPortIndex].SupportsMRPInterconnPortConfig;
    pGetPortParams->IsWireless                      = pDDB->Glob.PortParams[HwPortIndex].IsWireless;
    pGetPortParams->IsMDIX                          = pDDB->Glob.PortParams[HwPortIndex].IsMDIX;
    pGetPortParams->IsPulled                        = pDDB->Glob.PortParams[HwPortIndex].IsPulled;
    pGetPortParams->AutonegCapAdvertised            = pDDB->Glob.PortParams[HwPortIndex].AutonegCapAdvertised;
    pGetPortParams->MediaType                       = pDDB->Glob.PortParams[HwPortIndex].MediaType;
    pGetPortParams->IsPOF                           = pDDB->Glob.PortParams[HwPortIndex].IsPOF;
    pGetPortParams->ShortPreamble                   = EDD_PORT_LONG_PREAMBLE;      // not supported by IRTE!
    #if defined (EDDI_CFG_FRAG_ON)
    pGetPortParams->TxFragmentation                 = pLinkIndExtPara->TxFragmentation;
    #else
    pGetPortParams->TxFragmentation                 = EDD_PORT_NO_TX_FRAGMENTATION;
    #endif

    //Red Phase
    pGetPortParams->RsvIntervalRed.Rx.BeginNs = 0;
    pGetPortParams->RsvIntervalRed.Rx.EndNs   = 0;
    pGetPortParams->RsvIntervalRed.Tx.BeginNs = 0;
    pGetPortParams->RsvIntervalRed.Tx.EndNs   = 0;

    if ((pDDB->PRM.HandleCnt == 1) && (pDDB->PRM.PDIRData.State_A == EDDI_PRM_VALID))
    {
        EDDI_IRT_BEGIN_END_DATA_TYPE  *  const  pBeginEndData = &pRecordSet->BeginEndData[UsrPortIndex];

        if (pBeginEndData->bPresent)
        {
            if (pBeginEndData->Groups.RxRedOrangePeriodBeginGroupCnt)
            {
                pGetPortParams->RsvIntervalRed.Rx.BeginNs = pBeginEndData->Groups.RxRedOrangePeriodBegin[0]; //PRM has checked: only one value allowed!
            }
            if (pBeginEndData->Groups.TxRedOrangePeriodBeginGroupCnt)
            {
                pGetPortParams->RsvIntervalRed.Tx.BeginNs = pBeginEndData->Groups.TxRedOrangePeriodBegin[0]; //PRM has checked: only one value allowed!
            }
            pGetPortParams->RsvIntervalRed.Rx.EndNs = pBeginEndData->Groups.RxRedPeriodEndMax;
            pGetPortParams->RsvIntervalRed.Tx.EndNs = pBeginEndData->Groups.TxRedPeriodEndMax;
        }

        pGetPortParams->RsvIntervalRed.Rx.EndLocalNs        = pRecordSet->BeginEndData[UsrPortIndex].EndLocalNsRx;
        pGetPortParams->RsvIntervalRed.Rx.LastLocalFrameLen = pRecordSet->BeginEndData[UsrPortIndex].LastLocalFrameLenRx;
        pGetPortParams->RsvIntervalRed.Tx.EndLocalNs        = pRecordSet->BeginEndData[UsrPortIndex].EndLocalNsTx;
        pGetPortParams->RsvIntervalRed.Tx.LastLocalFrameLen = pRecordSet->BeginEndData[UsrPortIndex].LastLocalFrameLenTx;
    }
    else
    {
        pGetPortParams->RsvIntervalRed.Rx.EndLocalNs        = 0;
        pGetPortParams->RsvIntervalRed.Rx.LastLocalFrameLen = 0;
        pGetPortParams->RsvIntervalRed.Tx.EndLocalNs        = 0;
        pGetPortParams->RsvIntervalRed.Tx.LastLocalFrameLen = 0;
    }

    //Orange Phase
    pGetPortParams->RsvIntervalOrange.Tx.BeginNs = 0;
    pGetPortParams->RsvIntervalOrange.Tx.EndNs   = 0;
    pGetPortParams->RsvIntervalOrange.Rx.BeginNs = 0;
    pGetPortParams->RsvIntervalOrange.Rx.EndNs   = 0;

    if (pDDB->pLocal_CRT)
    {
        if (pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd)
        {
            if ((pDDB->PRM.HandleCnt == 1) && (pDDB->PRM.PDIRData.State_A == EDDI_PRM_VALID))
            {
                EDDI_IRT_BEGIN_END_DATA_TYPE  *  const  pBeginEndData = &pRecordSet->BeginEndData[UsrPortIndex];

                if (pBeginEndData->bPresent)
                {
                    //pGetPortParams->RsvIntervalOrange.Tx.BeginNs = 0; //????????
                    if (pBeginEndData->Groups.TxOrangePeriodEndGroupCnt)
                    {
                        pGetPortParams->RsvIntervalOrange.Tx.EndNs = pBeginEndData->Groups.TxOrangePeriodEnd[0]; //PRM has checked: only one value allowed!
                    }
                }
            }
            else
            {
                pGetPortParams->RsvIntervalOrange.Tx.BeginNs = pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalBegin;
                pGetPortParams->RsvIntervalOrange.Tx.EndNs   = pDDB->pLocal_CRT->RTClass2Interval.ReservedIntervalEnd;
            }
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetPortParams<-");

    return EDD_STS_OK;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetIP( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                      EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_SET_IP_PTR_TYPE  pIP;
    LSA_UINT32                 i;
    EDDI_IP_TYPE               TmpIP;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetIP->");

    TmpIP.dw = 0;

    pIP = (EDD_UPPER_SET_IP_PTR_TYPE)pRQB->pParam;

    for (i = 0; i < EDD_IP_ADDR_SIZE; i++)
    {
        TmpIP.b.IP[i] = pIP->LocalIP.IPAdr[i];
    }

    pDDB->Glob.LocalIP.dw = TmpIP.dw;

    if (pDDB->Glob.LocalIP.dw)
    {
        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        if (pDDB->pLocal_RTOUDP->cConsumer)
        {
            EDDI_RtoEnableRcvIF(pDDB);
        }

        EDDI_RtoChangeIP(pDDB);
        #endif
    }

    #if defined (EDDI_CFG_REV7)
    IO_W32(IP_Address0, EDDI_HTONL(pDDB->Glob.LocalIP.dw));
    #endif

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetIP<-");
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
static LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetParam( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_GET_PARAMS_PTR_TYPE            pGetParams;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRT   = pDDB->pLocal_CRT;
    LSA_UINT32                               ctr;
    
    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetParam->");
    
    pGetParams = (EDD_UPPER_GET_PARAMS_PTR_TYPE)pRQB->pParam;
        
    pGetParams->CycleBaseFactor             = pCRT->CycleBaseFactor;
    pGetParams->ConsumerFrameIDBaseClass1   = pCRT->CfgPara.SRT.ConsumerFrameIDBaseClass1;
    pGetParams->ConsumerFrameIDBaseClass2   = pCRT->CfgPara.SRT.ConsumerFrameIDBaseClass2;
    pGetParams->ConsumerFrameIDBaseClass3   = EDDI_SRT_FRAMEID_RT_CLASS3_START;
    pGetParams->ConsumerCntClass1           = pCRT->CfgPara.SRT.ConsumerCntClass12;
    pGetParams->ConsumerCntClass2           = pCRT->CfgPara.SRT.ConsumerCntClass12;
    pGetParams->ConsumerCntClass12Combined  = EDD_CONSUMERCNT_CLASS12_COMBINED;
    pGetParams->ConsumerCntClass3           = pCRT->CfgPara.IRT.ConsumerCntClass3;
    pGetParams->ProviderCnt                 = pCRT->CfgPara.SRT.ProviderCnt;
    pGetParams->ProviderCntClass3           = pCRT->CfgPara.IRT.ProviderCnt;
    pGetParams->ProviderGroupCnt            = pCRT->ProviderList.MaxGroups;
    pGetParams->DataLenMin                  = EDD_CSRT_DATALEN_MIN;
    pGetParams->DataLenMax                  = EDD_CSRT_DATALEN_MAX;
    pGetParams->DataLenMinUDP               = EDD_CSRT_UDP_DATALEN_MIN;
    pGetParams->DataLenMaxUDP               = EDD_CSRT_UDP_DATALEN_MAX;

    pGetParams->KRAMInfo.KRAMBaseAddr              = (LSA_UINT32)pDDB->pKRam;
    pGetParams->KRAMInfo.offset_ProcessImageStart  = 0;
    pGetParams->KRAMInfo.offset_ProcessImageEnd    = pDDB->ProcessImage.offset_ProcessImageEnd;

    for (ctr = 0; ctr < EDD_IP_ADDR_SIZE; ctr++)
    {
        pGetParams->LocalIP.IPAdr[ctr] = pDDB->Glob.LocalIP.b.IP[ctr];
    }

    //pGetParams->ReservedIntervalEndMaxNs = EDDI_GenGetMaxReservedIntEnd(pDDB);
       
    pGetParams->MACAddr                  = pDDB->Glob.LLHandle.xRT.MACAddressSrc;
    pGetParams->MaxPortCnt               = (LSA_UINT16)pDDB->PM.PortMap.PortCnt;
    pGetParams->InterfaceID              = pDDB->Glob.InterfaceID;
    pGetParams->HardwareType             = pDDB->Glob.HardwareType;
    pGetParams->TraceIdx                 = pDDB->Glob.TraceIdx;
    pGetParams->MRPDefaultRoleInstance0  = pDDB->SWITCH.MRPDefaultRoleInstance0;
    pGetParams->MRPSupportedRole         = pDDB->SWITCH.MRPSupportedRole;
    pGetParams->MaxMRP_Instances         = pDDB->SWITCH.MaxMRP_Instances;
    pGetParams->MRAEnableLegacyMode      = pDDB->SWITCH.MRAEnableLegacyMode;
    pGetParams->MRPSupportedMultipleRole = pDDB->SWITCH.MRPSupportedMultipleRole;
    pGetParams->MaxMRPInterconn_Instances= pDDB->SWITCH.MaxMRPInterconn_Instances;
    pGetParams->SupportedMRPInterconnRole= pDDB->SWITCH.SupportedMRPInterconnRole;
    pGetParams->FeatureMask              =   EDD_FEATURE_SUPPORT_NO_MCMAC_SRCPORT
                                           | ( pDDB->FeatureSupport.AdditionalForwardingRulesSupported      ? EDD_FEATURE_SUPPORT_ADD_FWD_RULES_MSK : 0)
                                           | ( pDDB->FeatureSupport.ApplicationExist                        ? EDD_FEATURE_SUPPORT_APPL_EXIST_MSK    : 0)
                                           | ((EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ClusterIPSupport) ? EDD_FEATURE_SUPPORT_CLUSTERIP_MSK: 0)
                                           | ((EDD_FEATURE_ENABLE == pDDB->FeatureSupport.MRPInterconnFwdRulesSupported) ? EDD_FEATURE_SUPPORT_MRP_INTERCONN_FWD_RULES: 0);
    
    if (pGetParams->MaxPortCnt > EDD_CFG_MAX_PORT_CNT)
    {
        EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetParam, MaxPortCnt(0x%x) > EDD_CFG_MAX_PORT_CNT(0x%x)", pGetParams->MaxPortCnt, EDD_CFG_MAX_PORT_CNT);
        EDDI_Excp("EDDI_GenGetParam, MaxPortCnt > EDD_CFG_MAX_PORT_CNT", EDDI_FATAL_ERR_EXCP, 0, 0);
    }

    #if defined (EDDI_CFG_REV7)
    pGetParams->TimeResolutionInNs      = 1; /* resolution of NRT timestamp */
    #else
    pGetParams->TimeResolutionInNs      = 10; /* resolution of NRT timestamp */
    #endif

    pGetParams->MaxInterfaceCntOfAllEDD = pDDB->Glob.MaxInterfaceCntOfAllEDD;
    pGetParams->MaxPortCntOfAllEDD      = pDDB->Glob.MaxPortCntOfAllEDD;

    EDDI_GenCalcTransferEndValues(pDDB, 
                                  &pGetParams->RsvIntervalRed, 
                                  pDDB->PRM.PDIRData.pRecordSet_A, 
                                  pDDB->PRM.PDirApplicationData.pRecordSet_A, 
                                  pDDB->PRM.PDIRData.State_A, 
                                  pDDB->PRM.PDirApplicationData.State_A);        

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetParam<-");
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenGetLinkStatus()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetLinkStatus( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  const  pLinkStatus = (EDD_UPPER_GET_LINK_STATUS_PTR_TYPE)pRQB->pParam;
    LSA_RESULT                                 Result;

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenGetLinkStatus -> PortID:0x%X", pLinkStatus->PortID);

    if (pLinkStatus->PortID > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        return EDD_STS_ERR_PARAM;
    }

    //evaluate all ports
    if (EDD_PORT_ID_AUTO == pLinkStatus->PortID)
    {
        pLinkStatus->Status = pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkStatus;
        pLinkStatus->Speed  = pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkSpeed;
        pLinkStatus->Mode   = pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkMode;
    }
    else
    {
        LSA_UINT32  HwPortIndex;

        Result = EDDI_GenChangeUsrPortID2HwPortIndex(pLinkStatus->PortID, pDDB, &HwPortIndex);
        if (EDD_STS_OK != Result)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            return Result;
        }

        pLinkStatus->Status = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LastLinkStatus;
        pLinkStatus->Speed  = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LastLinkSpeed;
        pLinkStatus->Mode   = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LastLinkMode;
    }
    pLinkStatus->AddCheckEnabled = EDD_LINK_ADD_CHECK_DISABLED;
    pLinkStatus->ChangeReason    = EDD_LINK_CHANGE_REASON_NONE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetLinkStatus<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenCalcTransferEndValues()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenCalcTransferEndValues( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                              EDD_RESERVED_INTERVAL_IF_RED_TYPE  *         pTransferEndValues,                                                          
                                                              EDDI_PRM_RECORD_IRT_PTR_TYPE                 pPDirDataRecordSet,
                                                              EDDI_PRM_PDIR_APPLICATION_DATA_PTR_TYPE      pPDirApplicationDataRecordSet,
                                                              EDDI_PRM_STATE                        const  PDirDataRecordState,
                                                              EDDI_PRM_STATE                        const  PDirApplicationDataRecordState)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenCalcTransferEndValues ->");

    if (pPDirApplicationDataRecordSet->bCalculatedTransferEndValues)
    {
        EDDI_MEMCOPY(pTransferEndValues, &pPDirApplicationDataRecordSet->TransferEndValues, sizeof(EDD_RESERVED_INTERVAL_IF_RED_TYPE));
    }
    else if ((pDDB->PRM.HandleCnt == 1) && (PDirDataRecordState == EDDI_PRM_VALID) /* && (pDDB->PRM.PDirApplicationData.RecordState_A == EDDI_PRM_VALID)*/)
    {
        //Attention: We also land here if only forwarders or forwarding consumers are contained in the IRDATA (CC-QV usecase)
        LSA_UINT32  ConsumerIODataLength    = pPDirApplicationDataRecordSet->ConsumerIODataLength;
        LSA_UINT32  LocalFrameSendOffset    = 0;
        LSA_UINT32  DataLength              = 0;
        LSA_UINT32  TxFrameSendOffsetStart  = 0x7FFFFFFF;
        LSA_UINT32  TxFrameSendOffsetEnd    = 0;
        LSA_UINT32  RxFrameSendOffsetEnd    = 0;
        LSA_UINT32  TxDataLengthEnd         = 0;
        LSA_UINT32  RxDataLengthEnd         = 0;
        LSA_UINT32  LocalRxApplicationDelay = 0; 
        LSA_UINT32  Index;
        LSA_UINT32  UsrPortIndex;

        pTransferEndValues->RxEndNs                = 0;
        pTransferEndValues->RxLocalTransferEndNs   = 0;
        pTransferEndValues->TxEndNs                = 0;
        pTransferEndValues->TxLocalTransferEndNs   = 0;
        pTransferEndValues->TxLocalTransferStartNs = 0;

        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            EDDI_IRT_BEGIN_END_DATA_TYPE  *  const  pBeginEndData = &pPDirDataRecordSet->BeginEndData[UsrPortIndex];
     
            if ( pTransferEndValues->TxEndNs < pBeginEndData->Groups.TxRedPeriodEndMax )
            {
                pTransferEndValues->TxEndNs = pBeginEndData->Groups.TxRedPeriodEndMax;    
            }
            
            if ( pTransferEndValues->RxEndNs < pBeginEndData->Groups.RxRedPeriodEndMax )
            {
                pTransferEndValues->RxEndNs = pBeginEndData->Groups.RxRedPeriodEndMax;    
            } 
        }
         
        //Calc 
        for (Index = 0; Index < pPDirDataRecordSet->NumberOfFrameDataElements; Index++)
        {
            EDDI_SYNC_IRT_FRM_HANDLER_ENUM const FrameHandlerTyp = EDDI_SyncDecodeFrameHandlerType(pDDB, &pPDirDataRecordSet->pLocalPDIRDataRecord->PDIRFrameData.PDIRFrameData[Index]);
            LocalFrameSendOffset                                 = pPDirDataRecordSet->pLocalPDIRDataRecord->PDIRFrameData.PDIRFrameData[Index].FrameSendOffset;
            DataLength                                           = pPDirDataRecordSet->pLocalPDIRDataRecord->PDIRFrameData.PDIRFrameData[Index].DataLength;
   
            switch (FrameHandlerTyp)
            {
                case EDDI_SYNC_IRT_PROVIDER:
                {
                    //Find RxLocalTransfertartNs
                    if (LocalFrameSendOffset < TxFrameSendOffsetStart)
                    {
                        TxFrameSendOffsetStart = LocalFrameSendOffset;
                    }

                    //Find TxLocalTransferEndNs 
                    //Here needs to add the DataLength to find the correct FrameSendOffset, because the whole FrameSendOffset+Datalength in ns can be bigger then the 
                    //highest FrameSendOffset itself.
                    if (   (LocalFrameSendOffset + (DataLength * EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS)) 
                         > (TxFrameSendOffsetEnd + (TxDataLengthEnd * EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS)))
                    {
                        TxFrameSendOffsetEnd = LocalFrameSendOffset;
                        TxDataLengthEnd = DataLength;
                    }
                    break;
                }

                case EDDI_SYNC_IRT_CONSUMER:   
                case EDDI_SYNC_IRT_FWD_CONSUMER:
                {
                    //Find RxLocalTransferEndNs
                    //Here needs to add the DataLength to find the correct FrameSendOffset, because the whole FrameSendOffset+Datalength in ns can be bigger then the 
                    //highest FrameSendOffset itself.
                    if (   (LocalFrameSendOffset + (DataLength      * EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS)) 
                         > (RxFrameSendOffsetEnd + (RxDataLengthEnd * EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS)))
                    {
                        RxFrameSendOffsetEnd = LocalFrameSendOffset;
                        RxDataLengthEnd      = DataLength;
                    }
                    break;
                }

                case EDDI_SYNC_IRT_FORWARDER:
                {
                    //not taken into account
                    break;
                }
                
                case EDDI_SYNC_IRT_INVALID_HANDLER:
                default:
                {
                    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenCalcTransferEndValues: Not allowed or invalid FramehandlerType: 0x%x", FrameHandlerTyp);
                    EDDI_Excp("EDDI_GenCalcTransferEndValues: Not allowed or invalid FramehandlerType: 0x%x", EDDI_FATAL_ERR_EXCP, FrameHandlerTyp, 0);
                    return;                   
                }
            }
        }
       
        //Calc only for RxFrameSendOffsetEnd
        if ( ConsumerIODataLength == 0 || (PDirApplicationDataRecordState == EDDI_PRM_NOT_VALID))
        {
            ConsumerIODataLength = EDDI_SDU_MAX_LENGTH;
        }

        #if defined EDDI_CFG_REV5
        if (ConsumerIODataLength <= EDDI_SDU_MAX_LENGTH)
        {
            LocalRxApplicationDelay = (ConsumerIODataLength * ((EDDI_R5_RX_APPL_DELAY_1 + EDDI_R5_RX_APPL_DELAY_1)/2))/10 + EDDI_R5_RX_APPL_DELAY_2;
        }
        else if (ConsumerIODataLength > EDDI_SDU_MAX_LENGTH)
        {
            LocalRxApplicationDelay = EDDI_R5_RX_APPL_DELAY_MAX;
        }
        #endif

        #if defined EDDI_CFG_REV6
        if (ConsumerIODataLength <= EDDI_SDU_MAX_LENGTH)
        {
            LocalRxApplicationDelay = (ConsumerIODataLength * ((EDDI_R6_RX_APPL_DELAY_1 + EDDI_R6_RX_APPL_DELAY_1)/2))/10 + EDDI_R6_RX_APPL_DELAY_2;

        }
        else if (ConsumerIODataLength > EDDI_SDU_MAX_LENGTH)
        {
            LocalRxApplicationDelay = EDDI_R6_RX_APPL_DELAY_MAX;
        }
        #endif

        #if defined EDDI_CFG_REV7
        if (ConsumerIODataLength <= EDDI_SDU_MAX_LENGTH)
        {
            LocalRxApplicationDelay = (ConsumerIODataLength * EDDI_R7_RX_APPL_DELAY_1) + ((pDDB->PM.PortMap.PortCnt-1) * EDDI_R7_RX_APPL_DELAY_2) + EDDI_R7_RX_APPL_DELAY_3; 
        }
        else if (ConsumerIODataLength > EDDI_SDU_MAX_LENGTH)
        {
            LocalRxApplicationDelay = EDDI_R7_RX_APPL_DELAY_MAX;
        }
        #endif

        if (LIMIT_LSA_INT32_MAX < TxFrameSendOffsetStart)
        {
            EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenCalcTransferEndValues: TxFrameSendOffsetStart (0x%X) exceeds upper LSA_INT32 limit",
                TxFrameSendOffsetStart);
            EDDI_Excp("EDDI_GenCalcTransferEndValues", EDDI_FATAL_ERR_EXCP, TxFrameSendOffsetStart, 0);
        }

        pTransferEndValues->TxLocalTransferStartNs = (0x7FFFFFFF == TxFrameSendOffsetStart)?0:(LSA_INT32)TxFrameSendOffsetStart;
        pTransferEndValues->RxLocalTransferEndNs   = RxFrameSendOffsetEnd + (((( RxDataLengthEnd + EDDI_FRAME_HEADER_SIZE + EDDI_CRC32_SIZE) * EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS) - pDDB->Glob.MaxBridgeDelay) + LocalRxApplicationDelay);        
        pTransferEndValues->TxLocalTransferEndNs   = TxFrameSendOffsetEnd + (( TxDataLengthEnd + EDDI_FRAME_HEADER_SIZE + EDDI_CRC32_SIZE) * EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS); 

        EDDI_MEMCOPY(&pPDirApplicationDataRecordSet->TransferEndValues, pTransferEndValues, sizeof(EDD_RESERVED_INTERVAL_IF_RED_TYPE));

        pPDirApplicationDataRecordSet->bCalculatedTransferEndValues = LSA_TRUE;
    }
    else
    {
        pTransferEndValues->RxEndNs                = 0;
        pTransferEndValues->RxLocalTransferEndNs   = 0;
        pTransferEndValues->TxEndNs                = 0;
        pTransferEndValues->TxLocalTransferEndNs   = 0;
        pTransferEndValues->TxLocalTransferStartNs = 0;
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenCalcTransferEndValues <-");
}

/***************************************************************************/
/* F u n c t i o n:       EDDI_GenCalcStatistics_partial()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenCalcStatistics_partial(EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
    EDD_GET_STATISTICS_MIB_TYPE      *  pMIB,
    LSA_UINT16                   const  PortID,
    LSA_BOOL                     const  bUseShadowRegister)
{
    EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE  const  pShadowBuffer = (EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE)&(pHDB->sStatisticShadow[PortID]); //ptr may be invalid!
    EDDI_LOCAL_DDB_PTR_TYPE                     const  pDDB = pHDB->pDDB;
    LSA_RESULT  Result = EDD_STS_OK;
    LSA_UINT8   Index;
    LSA_UINT32  HwPortIndex;

    if ((PortID < (pDDB->PM.PortMap.PortCnt + 1)) && (PortID > 0))
    {
        //Get statistics of "real" ports
        Result = EDDI_GenChangeUsrPortID2HwPortIndex(PortID, pDDB, &HwPortIndex);

        if (EDD_STS_OK != Result)
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics_partial, PortID:0x%X", PortID);
            return Result;
        }

        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenCalcStatistics_partial");

        //MIB2 Calc

        pMIB->SupportedCounters = EDDI_MIB2_SUPPORTED_COUNTERS_PORT;

        if (bUseShadowRegister)
        {
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INOCTETS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxGoodByte = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_GBRX, pDDB);
                pMIB->InOctets = pShadowBuffer->LocalStatistics.RxGoodByte - pShadowBuffer->ShadowStatistics.sRaw.irte.RxGoodByte;
            }

            //build MIB-Values
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxUniFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRX, pDDB);
                pMIB->InUcastPkts = pShadowBuffer->LocalStatistics.RxUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUniFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INNUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRX, pDDB);
                pShadowBuffer->LocalStatistics.RxBroadMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFRX, pDDB);
                pMIB->InNUcastPkts = (pShadowBuffer->LocalStatistics.RxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame) +
                    (pShadowBuffer->LocalStatistics.RxBroadMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame);
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INMULTICASTPKTS) != 0)
                {
                    pMIB->InMulticastPkts = pShadowBuffer->LocalStatistics.RxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INBROADCASTPKTS) != 0)
                {
                    pMIB->InBroadcastPkts = pShadowBuffer->LocalStatistics.RxBroadMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame;
                }
            }
            else
            {
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INMULTICASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.RxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRX, pDDB);
                    pMIB->InMulticastPkts = pShadowBuffer->LocalStatistics.RxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INBROADCASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.RxBroadMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFRX, pDDB);
                    pMIB->InBroadcastPkts = pShadowBuffer->LocalStatistics.RxBroadMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame;
                }
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INDISCARDS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxFcwDbDropFrame = EDDI_SwiPortGetDropCtr(HwPortIndex, pDDB);
                pMIB->InDiscards = pShadowBuffer->LocalStatistics.RxFcwDbDropFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxFcwDbDropFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INERRORS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxOverflowFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOF, pDDB);
                pShadowBuffer->LocalStatistics.RxOversizeFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOSF, pDDB);
                pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_CRCER, pDDB);
                pMIB->InErrors = (pShadowBuffer->LocalStatistics.RxOversizeFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxOversizeFrame) +
                    (pShadowBuffer->LocalStatistics.RxOverflowFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxOverflowFrame) +
                    (pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxErrCrcAlignFrame);
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INUNKNOWNPROTOS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxUnUniFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRXU, pDDB);
                pShadowBuffer->LocalStatistics.RxUnMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRXU, pDDB);
                pMIB->InUnknownProtos = (pShadowBuffer->LocalStatistics.RxUnUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnUniFrame) +
                    (pShadowBuffer->LocalStatistics.RxUnMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnMulFrame);
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTOCTETS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxTotalByte = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BTX, pDDB);
                pMIB->OutOctets = pShadowBuffer->LocalStatistics.TxTotalByte - pShadowBuffer->ShadowStatistics.sRaw.irte.TxTotalByte;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxUniFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFTX, pDDB);
                pMIB->OutUcastPkts = pShadowBuffer->LocalStatistics.TxUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxUniFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTNUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFTX, pDDB);
                pShadowBuffer->LocalStatistics.TxBroadFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFTX, pDDB);
                pMIB->OutNUcastPkts = (pShadowBuffer->LocalStatistics.TxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame) +
                    (pShadowBuffer->LocalStatistics.TxBroadFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame);
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTMULTICASTPKTS) != 0)
                {
                    pMIB->OutMulticastPkts = pShadowBuffer->LocalStatistics.TxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTBROADCASTPKTS) != 0)
                {
                    pMIB->OutBroadcastPkts = pShadowBuffer->LocalStatistics.TxBroadFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame;
                }
            }
            else
            {
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTMULTICASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.TxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFTX, pDDB);
                    pMIB->OutMulticastPkts = pShadowBuffer->LocalStatistics.TxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTBROADCASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.TxBroadFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFTX, pDDB);
                    pMIB->OutBroadcastPkts = pShadowBuffer->LocalStatistics.TxBroadFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame;
                }
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTERRORS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxCollisionFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TXCOL, pDDB);
                pMIB->OutErrors = pShadowBuffer->LocalStatistics.TxCollisionFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxCollisionFrame;
            }
        }
        else
        {
            //build MIB-Values
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INOCTETS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxGoodByte = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_GBRX, pDDB);
                pMIB->InOctets = pShadowBuffer->LocalStatistics.RxGoodByte;
            }

            //build MIB-Values
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxUniFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRX, pDDB);
                pMIB->InUcastPkts = pShadowBuffer->LocalStatistics.RxUniFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INNUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRX, pDDB);
                pShadowBuffer->LocalStatistics.RxBroadMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFRX, pDDB);
                pMIB->InNUcastPkts = pShadowBuffer->LocalStatistics.RxMulFrame + pShadowBuffer->LocalStatistics.RxBroadMulFrame;
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INMULTICASTPKTS) != 0)
                {
                    pMIB->InMulticastPkts = pShadowBuffer->LocalStatistics.RxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INBROADCASTPKTS) != 0)
                {
                    pMIB->InBroadcastPkts = pShadowBuffer->LocalStatistics.RxBroadMulFrame;
                }
            }
            else
            {
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INMULTICASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.RxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRX, pDDB);
                    pMIB->InMulticastPkts = pShadowBuffer->LocalStatistics.RxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INBROADCASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.RxBroadMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFRX, pDDB);
                    pMIB->InBroadcastPkts = pShadowBuffer->LocalStatistics.RxBroadMulFrame;
                }
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INDISCARDS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxFcwDbDropFrame = EDDI_SwiPortGetDropCtr(HwPortIndex, pDDB);
                pMIB->InDiscards = pShadowBuffer->LocalStatistics.RxFcwDbDropFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INERRORS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxOverflowFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOF, pDDB);
                pShadowBuffer->LocalStatistics.RxOversizeFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOSF, pDDB);
                pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_CRCER, pDDB);
                pMIB->InErrors = pShadowBuffer->LocalStatistics.RxOversizeFrame + pShadowBuffer->LocalStatistics.RxOverflowFrame + pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INUNKNOWNPROTOS) != 0)
            {
                pShadowBuffer->LocalStatistics.RxUnUniFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRXU, pDDB);
                pShadowBuffer->LocalStatistics.RxUnMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRXU, pDDB);
                pMIB->InUnknownProtos = pShadowBuffer->LocalStatistics.RxUnUniFrame + pShadowBuffer->LocalStatistics.RxUnMulFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTOCTETS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxTotalByte = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BTX, pDDB);
                pMIB->OutOctets = pShadowBuffer->LocalStatistics.TxTotalByte;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxUniFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFTX, pDDB);
                pMIB->OutUcastPkts = pShadowBuffer->LocalStatistics.TxUniFrame;
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTNUCASTPKTS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFTX, pDDB);
                pShadowBuffer->LocalStatistics.TxBroadFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFTX, pDDB);
                pMIB->OutNUcastPkts = pShadowBuffer->LocalStatistics.TxMulFrame + pShadowBuffer->LocalStatistics.TxBroadFrame;
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTMULTICASTPKTS) != 0)
                {
                    pMIB->OutMulticastPkts = pShadowBuffer->LocalStatistics.TxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTBROADCASTPKTS) != 0)
                {
                    pMIB->OutBroadcastPkts = pShadowBuffer->LocalStatistics.TxBroadFrame;
                }
            }
            else
            {
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTMULTICASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.TxMulFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFTX, pDDB);
                    pMIB->OutMulticastPkts = pShadowBuffer->LocalStatistics.TxMulFrame;
                }
                if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTBROADCASTPKTS) != 0)
                {
                    pShadowBuffer->LocalStatistics.TxBroadFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFTX, pDDB);
                    pMIB->OutBroadcastPkts = pShadowBuffer->LocalStatistics.TxBroadFrame;
                }
            }
            if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTERRORS) != 0)
            {
                pShadowBuffer->LocalStatistics.TxCollisionFrame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TXCOL, pDDB);
                pMIB->OutErrors = pShadowBuffer->LocalStatistics.TxCollisionFrame;
            }
        }
        pMIB->OutDiscards = 0;
        pMIB->OutQLen = 0;
        pMIB->InHCOctets = 0;
        pMIB->InHCUcastPkts = 0;
        pMIB->InHCMulticastPkts = 0;
        pMIB->InHCBroadcastPkts = 0;
        pMIB->OutHCOctets = 0;
        pMIB->OutHCUcastPkts = 0;
        pMIB->OutHCMulticastPkts = 0;
        pMIB->OutHCBroadcastPkts = 0;
    }
    else if (PortID == 0)
    {
        //Get statistics of local interface (A+B)
        EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenCalcStatistics: Note: Not all statistics are supported for Port: 0 yet -> PortID:0x%X", PortID);

        pMIB->SupportedCounters = EDDI_MIB2_SUPPORTED_COUNTERS_IF;

        pMIB->InOctets = 0;
        if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_INUCASTPKTS) != 0)
        {
            pMIB->InUcastPkts = pDDB->LocalIFStats.RxGoodCtr - ((bUseShadowRegister) ? pShadowBuffer->ShadowLocalIFCtr.RxGoodCtr : 0);
        }
        pMIB->InNUcastPkts = 0;
        pMIB->InDiscards = 0;
        pMIB->InErrors = 0;

        pMIB->InUnknownProtos = 0;
        pMIB->OutOctets = 0;
        if ((pMIB->RequestedCounters & EDD_MIB_SUPPORT_OUTUCASTPKTS) != 0)
        {
            pMIB->OutUcastPkts = pDDB->LocalIFStats.TxGoodCtr - ((bUseShadowRegister) ? pShadowBuffer->ShadowLocalIFCtr.TxGoodCtr : 0);
        }
        pMIB->OutNUcastPkts = 0;

        pMIB->OutErrors = 0;
        pMIB->OutQLen = 0;
        pMIB->InMulticastPkts = 0;
        pMIB->InBroadcastPkts = 0;
        pMIB->OutMulticastPkts = 0;
        pMIB->OutBroadcastPkts = 0;
        pMIB->InHCOctets = 0;
        pMIB->InHCUcastPkts = 0;
        pMIB->InHCMulticastPkts = 0;
        pMIB->InHCBroadcastPkts = 0;
        pMIB->OutHCOctets = 0;
        pMIB->OutHCUcastPkts = 0;
        pMIB->OutHCMulticastPkts = 0;
        pMIB->OutHCBroadcastPkts = 0;

        //outdiscards is the sum of all hol-bnd-crossings
        for (Index = 1; Index<(pDDB->PM.PortMap.PortCnt + 1); Index++)
        {
            Result = EDDI_GenChangeUsrPortID2HwPortIndex(Index, pDDB, &HwPortIndex);
            if (EDD_STS_OK != Result)
            {
                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, PortID:0x%X", PortID);
                return Result;
            }
        }
        pMIB->OutDiscards = 0;
    }
    else //i.e. portID==5 => Error
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, wrong PortID:0x%X", PortID);
        Result = EDD_STS_ERR_PARAM;
    }

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenCalcStatistics()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenCalcStatistics( EDDI_LOCAL_HDB_PTR_TYPE      const  pHDB,
                                                         LSA_UINT8                    const  RawUsed,   
                                                         LSA_UINT8                    const  Reset,   
                                                         EDD_GET_STATISTICS_RAW_TYPE      *  pRaw,
                                                         EDD_GET_STATISTICS_MIB_TYPE      *  pMIB,
                                                         LSA_UINT16                   const  PortID,
                                                         LSA_BOOL                     const  bUseShadowRegister )
{
    EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE  const  pShadowBuffer  = (EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE)&(pHDB->sStatisticShadow[PortID]); //ptr may be invalid!
    EDDI_LOCAL_DDB_PTR_TYPE                     const  pDDB           = pHDB->pDDB;
    LSA_RESULT  Result        = EDD_STS_OK;
    LSA_UINT8   Index;
    LSA_UINT32  HwPortIndex;
    #if defined (EDDI_CFG_FRAG_ON)
    EDDI_NRT_TX_FRAG_DDB_TYPE                *  const  pNrtTxFragData = &pDDB->NRT.TxFragData;
    #endif
    #if defined (EDDI_CFG_DEFRAG_ON)
    EDDI_NRT_RX_FRAG_TYPE                    *  const  pNrtRxFragData = &pDDB->NRT.RxFragData;
    #endif
    
    if (   (   (EDD_STATISTICS_GET_VALUES_RAW_MIB2 != RawUsed)
            && (EDD_STATISTICS_GET_VALUES_MIB2 != RawUsed) )
        || (   (EDD_STATISTICS_DO_NOT_RESET_VALUES != Reset)
            && (EDD_STATISTICS_RESET_VALUES != Reset) ) )
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, wrong params: RawUsed (%d), Reset (%d)", RawUsed, Reset);
        Result = EDD_STS_ERR_PARAM;
    }

    if ((EDD_STATISTICS_GET_VALUES_MIB2 == RawUsed)
        && (EDD_STATISTICS_RESET_VALUES != Reset)
        && (pMIB->RequestedCounters != 0))
    {
        return (EDDI_GenCalcStatistics_partial(pHDB, pMIB, PortID, bUseShadowRegister));
    }


    if ((PortID < (pDDB->PM.PortMap.PortCnt+1)) && (PortID > 0))
    {
        //Get statistics of "real" ports
        Result = EDDI_GenChangeUsrPortID2HwPortIndex(PortID, pDDB, &HwPortIndex);

        if (EDD_STS_OK != Result)
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, PortID:0x%X", PortID);
            return Result;
        }

        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenCalcStatistics");

        //copy all HW-Register to temporary buffer
        pShadowBuffer->LocalStatistics.RxGoodByte               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_GBRX       , pDDB);
        pShadowBuffer->LocalStatistics.RxTotalByte              = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TBRX       , pDDB);
        pShadowBuffer->LocalStatistics.TxTotalByte              = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BTX        , pDDB);
        pShadowBuffer->LocalStatistics.TxFrame                  = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FTX        , pDDB);
        pShadowBuffer->LocalStatistics.RxUniFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRX       , pDDB);
        pShadowBuffer->LocalStatistics.RxUnUniFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRXU      , pDDB);
        pShadowBuffer->LocalStatistics.RxMulFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRX       , pDDB);
        pShadowBuffer->LocalStatistics.RxUnMulFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRXU      , pDDB);
        pShadowBuffer->LocalStatistics.RxBroadMulFrame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFRX       , pDDB);
        pShadowBuffer->LocalStatistics.TxUniFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFTX       , pDDB);
        pShadowBuffer->LocalStatistics.TxMulFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFTX       , pDDB);
        pShadowBuffer->LocalStatistics.TxBroadFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFTX       , pDDB);
        pShadowBuffer->LocalStatistics.Size_0_63_Frame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F0_64      , pDDB);
        pShadowBuffer->LocalStatistics.Size_64_Frame            = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F64        , pDDB);
        pShadowBuffer->LocalStatistics.Size_65_127_Frame        = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F65_127    , pDDB);
        pShadowBuffer->LocalStatistics.Size_128_255_Frame       = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F128_255   , pDDB);
        pShadowBuffer->LocalStatistics.Size_256_511_Frame       = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F256_511   , pDDB);
        pShadowBuffer->LocalStatistics.Size_512_1023_Frame      = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F512_1023  , pDDB);
        pShadowBuffer->LocalStatistics.Size_1024_1536_Frame     = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F1024_1536 , pDDB);
        pShadowBuffer->LocalStatistics.RxGoodFrame              = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_GFRX       , pDDB);
        pShadowBuffer->LocalStatistics.RxTotalFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TFRX       , pDDB);
        pShadowBuffer->LocalStatistics.RxSize_0_63_ErrCRC_Frame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FRX0_64_CRC, pDDB);
        pShadowBuffer->LocalStatistics.RxOverflowFrame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOF       , pDDB);
        pShadowBuffer->LocalStatistics.RxOversizeFrame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOSF      , pDDB);
        pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame       = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_CRCER      , pDDB);
        pShadowBuffer->LocalStatistics.RxFcwDbDropFrame         = EDDI_SwiPortGetDropCtr (HwPortIndex,                            pDDB);
        pShadowBuffer->LocalStatistics.TxCollisionFrame         = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TXCOL      , pDDB);
        pShadowBuffer->LocalStatistics.HolBndCros               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_HOLBC      , pDDB);

        //get statistics (= Local Values - Shadow Values)
        //get RAW-statistics
        if (EDD_STATISTICS_GET_VALUES_RAW_MIB2 == RawUsed)
        {
            if (bUseShadowRegister)
            {
                pRaw->irte.RxGoodByte                = pShadowBuffer->LocalStatistics.RxGoodByte - pShadowBuffer->ShadowStatistics.sRaw.irte.RxGoodByte;
                pRaw->irte.RxTotalByte               = pShadowBuffer->LocalStatistics.RxTotalByte - pShadowBuffer->ShadowStatistics.sRaw.irte.RxTotalByte;
                pRaw->irte.TxTotalByte               = pShadowBuffer->LocalStatistics.TxTotalByte - pShadowBuffer->ShadowStatistics.sRaw.irte.TxTotalByte;
                pRaw->irte.TxFrame                   = pShadowBuffer->LocalStatistics.TxFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxFrame;
                pRaw->irte.RxUniFrame                = pShadowBuffer->LocalStatistics.RxUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUniFrame;
                pRaw->irte.RxUnUniFrame              = pShadowBuffer->LocalStatistics.RxUnUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnUniFrame;
                pRaw->irte.RxMulFrame                = pShadowBuffer->LocalStatistics.RxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame;
                pRaw->irte.RxUnMulFrame              = pShadowBuffer->LocalStatistics.RxUnMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnMulFrame;
                pRaw->irte.RxBroadMulFrame           = pShadowBuffer->LocalStatistics.RxBroadMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame;
                pRaw->irte.TxUniFrame                = pShadowBuffer->LocalStatistics.TxUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxUniFrame;
                pRaw->irte.TxMulFrame                = pShadowBuffer->LocalStatistics.TxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame;
                pRaw->irte.TxBroadFrame              = pShadowBuffer->LocalStatistics.TxBroadFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame;
                pRaw->irte.Size_0_63_Frame           = pShadowBuffer->LocalStatistics.Size_0_63_Frame - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_0_63_Frame;
                pRaw->irte.Size_64_Frame             = pShadowBuffer->LocalStatistics.Size_64_Frame - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_64_Frame;
                pRaw->irte.Size_65_127_Frame         = pShadowBuffer->LocalStatistics.Size_65_127_Frame - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_65_127_Frame;
                pRaw->irte.Size_128_255_Frame        = pShadowBuffer->LocalStatistics.Size_128_255_Frame - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_128_255_Frame;
                pRaw->irte.Size_256_511_Frame        = pShadowBuffer->LocalStatistics.Size_256_511_Frame - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_256_511_Frame;
                pRaw->irte.Size_512_1023_Frame       = pShadowBuffer->LocalStatistics.Size_512_1023_Frame  - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_512_1023_Frame;
                pRaw->irte.Size_1024_1536_Frame      = pShadowBuffer->LocalStatistics.Size_1024_1536_Frame  - pShadowBuffer->ShadowStatistics.sRaw.irte.Size_1024_1536_Frame;
                pRaw->irte.RxGoodFrame               = pShadowBuffer->LocalStatistics.RxGoodFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxGoodFrame;
                pRaw->irte.RxTotalFrame              = pShadowBuffer->LocalStatistics.RxTotalFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxTotalFrame;
                pRaw->irte.RxSize_0_63_ErrCRC_Frame  = pShadowBuffer->LocalStatistics.RxSize_0_63_ErrCRC_Frame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxSize_0_63_ErrCRC_Frame;
                pRaw->irte.RxOverflowFrame           = pShadowBuffer->LocalStatistics.RxOverflowFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxOverflowFrame;
                pRaw->irte.RxOversizeFrame           = pShadowBuffer->LocalStatistics.RxOversizeFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxOversizeFrame;
                pRaw->irte.RxErrCrcAlignFrame        = pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxErrCrcAlignFrame;
                pRaw->irte.RxFcwDbDropFrame          = pShadowBuffer->LocalStatistics.RxFcwDbDropFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxFcwDbDropFrame;
                pRaw->irte.TxCollisionFrame          = pShadowBuffer->LocalStatistics.TxCollisionFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxCollisionFrame;
                pRaw->irte.HolBndCros                = pShadowBuffer->LocalStatistics.HolBndCros - pShadowBuffer->ShadowStatistics.sRaw.irte.HolBndCros;
                #if defined (EDDI_CFG_FRAG_ON)
                pRaw->irte.SentFragmentedFrames      = pNrtTxFragData->Statistic[HwPortIndex].SentFragmentedFrames - pShadowBuffer->ShadowStatistics.sRaw.irte.SentFragmentedFrames;
                pRaw->irte.SentNonFragmentedFrames   = pNrtTxFragData->Statistic[HwPortIndex].SentNonFragmentedFrames - pShadowBuffer->ShadowStatistics.sRaw.irte.SentNonFragmentedFrames;
                pRaw->irte.DiscardedTXFrames         = pNrtTxFragData->Statistic[HwPortIndex].DiscardedTXFrames - pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedTXFrames;
                #else
                pRaw->irte.SentFragmentedFrames      = 0;
                pRaw->irte.SentNonFragmentedFrames   = 0;
                pRaw->irte.DiscardedTXFrames         = 0;
                #endif  

                #if defined (EDDI_CFG_DEFRAG_ON)
                pRaw->irte.ReceivedFragmentedFrames      = pNrtRxFragData->Statistic[HwPortIndex].ReceivedFragmentedFrames - pShadowBuffer->ShadowStatistics.sRaw.irte.ReceivedFragmentedFrames;
                pRaw->irte.ReceivedNonFragmentedFrames   = pNrtRxFragData->Statistic[HwPortIndex].ReceivedNonFragmentedFrames - pShadowBuffer->ShadowStatistics.sRaw.irte.ReceivedNonFragmentedFrames;
                pRaw->irte.DiscardedRXFrames             = pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFrames - pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedRXFrames;
                pRaw->irte.DiscardedRXFragments          = pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFragments - pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedRXFragments;
                #else
                pRaw->irte.ReceivedFragmentedFrames      = 0;
                pRaw->irte.ReceivedNonFragmentedFrames   = 0;
                pRaw->irte.DiscardedRXFrames             = 0;
                pRaw->irte.DiscardedRXFragments          = 0;
                #endif            
            }
            else
            {
                pRaw->irte.RxGoodByte                = pShadowBuffer->LocalStatistics.RxGoodByte;
                pRaw->irte.RxTotalByte               = pShadowBuffer->LocalStatistics.RxTotalByte;
                pRaw->irte.TxTotalByte               = pShadowBuffer->LocalStatistics.TxTotalByte;
                pRaw->irte.TxFrame                   = pShadowBuffer->LocalStatistics.TxFrame;
                pRaw->irte.RxUniFrame                = pShadowBuffer->LocalStatistics.RxUniFrame;
                pRaw->irte.RxUnUniFrame              = pShadowBuffer->LocalStatistics.RxUnUniFrame;
                pRaw->irte.RxMulFrame                = pShadowBuffer->LocalStatistics.RxMulFrame;
                pRaw->irte.RxUnMulFrame              = pShadowBuffer->LocalStatistics.RxUnMulFrame;
                pRaw->irte.RxBroadMulFrame           = pShadowBuffer->LocalStatistics.RxBroadMulFrame;
                pRaw->irte.TxUniFrame                = pShadowBuffer->LocalStatistics.TxUniFrame;
                pRaw->irte.TxMulFrame                = pShadowBuffer->LocalStatistics.TxMulFrame;
                pRaw->irte.TxBroadFrame              = pShadowBuffer->LocalStatistics.TxBroadFrame;
                pRaw->irte.Size_0_63_Frame           = pShadowBuffer->LocalStatistics.Size_0_63_Frame;
                pRaw->irte.Size_64_Frame             = pShadowBuffer->LocalStatistics.Size_64_Frame;
                pRaw->irte.Size_65_127_Frame         = pShadowBuffer->LocalStatistics.Size_65_127_Frame;
                pRaw->irte.Size_128_255_Frame        = pShadowBuffer->LocalStatistics.Size_128_255_Frame;
                pRaw->irte.Size_256_511_Frame        = pShadowBuffer->LocalStatistics.Size_256_511_Frame;
                pRaw->irte.Size_512_1023_Frame       = pShadowBuffer->LocalStatistics.Size_512_1023_Frame;
                pRaw->irte.Size_1024_1536_Frame      = pShadowBuffer->LocalStatistics.Size_1024_1536_Frame;
                pRaw->irte.RxGoodFrame               = pShadowBuffer->LocalStatistics.RxGoodFrame;
                pRaw->irte.RxTotalFrame              = pShadowBuffer->LocalStatistics.RxTotalFrame;
                pRaw->irte.RxSize_0_63_ErrCRC_Frame  = pShadowBuffer->LocalStatistics.RxSize_0_63_ErrCRC_Frame;
                pRaw->irte.RxOverflowFrame           = pShadowBuffer->LocalStatistics.RxOverflowFrame;
                pRaw->irte.RxOversizeFrame           = pShadowBuffer->LocalStatistics.RxOversizeFrame;
                pRaw->irte.RxErrCrcAlignFrame        = pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame;
                pRaw->irte.RxFcwDbDropFrame          = pShadowBuffer->LocalStatistics.RxFcwDbDropFrame;
                pRaw->irte.TxCollisionFrame          = pShadowBuffer->LocalStatistics.TxCollisionFrame;
                pRaw->irte.HolBndCros                = pShadowBuffer->LocalStatistics.HolBndCros;

                #if defined (EDDI_CFG_FRAG_ON)
                pRaw->irte.SentFragmentedFrames      = pNrtTxFragData->Statistic[HwPortIndex].SentFragmentedFrames;
                pRaw->irte.SentNonFragmentedFrames   = pNrtTxFragData->Statistic[HwPortIndex].SentNonFragmentedFrames;
                pRaw->irte.DiscardedTXFrames         = pNrtTxFragData->Statistic[HwPortIndex].DiscardedTXFrames;
                #else
                pRaw->irte.SentFragmentedFrames      = 0;
                pRaw->irte.SentNonFragmentedFrames   = 0;
                pRaw->irte.DiscardedTXFrames         = 0;
                #endif  

                #if defined (EDDI_CFG_DEFRAG_ON)
                pRaw->irte.ReceivedFragmentedFrames      = pNrtRxFragData->Statistic[HwPortIndex].ReceivedFragmentedFrames;
                pRaw->irte.ReceivedNonFragmentedFrames   = pNrtRxFragData->Statistic[HwPortIndex].ReceivedNonFragmentedFrames;
                pRaw->irte.DiscardedRXFrames             = pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFrames;
                pRaw->irte.DiscardedRXFragments          = pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFragments;
                #else
                pRaw->irte.ReceivedFragmentedFrames      = 0;
                pRaw->irte.ReceivedNonFragmentedFrames   = 0;
                pRaw->irte.DiscardedRXFrames             = 0;
                pRaw->irte.DiscardedRXFragments          = 0;
                #endif   
            }
        }

        //MIB2 Calc
        
        pMIB->SupportedCounters = EDDI_MIB2_SUPPORTED_COUNTERS_PORT;
        
        if (bUseShadowRegister)
        {
            //build MIB-Values
            pMIB->InOctets                           = pShadowBuffer->LocalStatistics.RxTotalByte - pShadowBuffer->ShadowStatistics.sRaw.irte.RxTotalByte;
            pMIB->InUcastPkts                        = pShadowBuffer->LocalStatistics.RxUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUniFrame;
            pMIB->InNUcastPkts                       = (pShadowBuffer->LocalStatistics.RxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame) +
                                                                      (pShadowBuffer->LocalStatistics.RxBroadMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame);
            pMIB->InDiscards                         = pShadowBuffer->LocalStatistics.RxFcwDbDropFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxFcwDbDropFrame;
            pMIB->InErrors                           = (pShadowBuffer->LocalStatistics.RxOversizeFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxOversizeFrame)  +
                                                                      (pShadowBuffer->LocalStatistics.RxOverflowFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxOverflowFrame)    +
                                                                      (pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxErrCrcAlignFrame);
            pMIB->InUnknownProtos                    = (pShadowBuffer->LocalStatistics.RxUnUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnUniFrame) +
                                                                      (pShadowBuffer->LocalStatistics.RxUnMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnMulFrame);
            pMIB->OutOctets                          = pShadowBuffer->LocalStatistics.TxTotalByte - pShadowBuffer->ShadowStatistics.sRaw.irte.TxTotalByte;
            pMIB->OutUcastPkts                       = pShadowBuffer->LocalStatistics.TxUniFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxUniFrame;
            pMIB->OutNUcastPkts                      = (pShadowBuffer->LocalStatistics.TxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame) +
                                                                      (pShadowBuffer->LocalStatistics.TxBroadFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame);
            pMIB->OutDiscards                        = 0;
            pMIB->OutErrors                          = pShadowBuffer->LocalStatistics.TxCollisionFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxCollisionFrame;
            pMIB->OutQLen                            = 0;
            pMIB->InMulticastPkts                    = pShadowBuffer->LocalStatistics.RxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame;
            pMIB->InBroadcastPkts                    = pShadowBuffer->LocalStatistics.RxBroadMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame;
            pMIB->OutMulticastPkts                   = pShadowBuffer->LocalStatistics.TxMulFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame;
            pMIB->OutBroadcastPkts                   = pShadowBuffer->LocalStatistics.TxBroadFrame - pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame;
            pMIB->InHCOctets                         = 0;
            pMIB->InHCUcastPkts                      = 0;
            pMIB->InHCMulticastPkts                  = 0;
            pMIB->InHCBroadcastPkts                  = 0;
            pMIB->OutHCOctets                        = 0;
            pMIB->OutHCUcastPkts                     = 0;
            pMIB->OutHCMulticastPkts                 = 0;
            pMIB->OutHCBroadcastPkts                 = 0;
        }
        else
        {
            //build MIB-Values
            pMIB->InOctets                           = pShadowBuffer->LocalStatistics.RxTotalByte;
            pMIB->InUcastPkts                        = pShadowBuffer->LocalStatistics.RxUniFrame;
            pMIB->InNUcastPkts                       = pShadowBuffer->LocalStatistics.RxMulFrame + pShadowBuffer->LocalStatistics.RxBroadMulFrame;
            pMIB->InDiscards                         = pShadowBuffer->LocalStatistics.RxFcwDbDropFrame;
            pMIB->InErrors                           = pShadowBuffer->LocalStatistics.RxOversizeFrame + pShadowBuffer->LocalStatistics.RxOverflowFrame + pShadowBuffer->LocalStatistics.RxErrCrcAlignFrame;
            pMIB->InUnknownProtos                    = pShadowBuffer->LocalStatistics.RxUnUniFrame + pShadowBuffer->LocalStatistics.RxUnMulFrame;
            pMIB->OutOctets                          = pShadowBuffer->LocalStatistics.TxTotalByte;
            pMIB->OutUcastPkts                       = pShadowBuffer->LocalStatistics.TxUniFrame;
            pMIB->OutNUcastPkts                      = pShadowBuffer->LocalStatistics.TxMulFrame + pShadowBuffer->LocalStatistics.TxBroadFrame;
            pMIB->OutDiscards                        = 0;
            pMIB->OutErrors                          = pShadowBuffer->LocalStatistics.TxCollisionFrame;
            pMIB->OutQLen                            = 0; 
            pMIB->InMulticastPkts                    = pShadowBuffer->LocalStatistics.RxMulFrame;
            pMIB->InBroadcastPkts                    = pShadowBuffer->LocalStatistics.RxBroadMulFrame;
            pMIB->OutMulticastPkts                   = pShadowBuffer->LocalStatistics.TxMulFrame;
            pMIB->OutBroadcastPkts                   = pShadowBuffer->LocalStatistics.TxBroadFrame;
            pMIB->InHCOctets                         = 0;
            pMIB->InHCUcastPkts                      = 0;
            pMIB->InHCMulticastPkts                  = 0;
            pMIB->InHCBroadcastPkts                  = 0;
            pMIB->OutHCOctets                        = 0;
            pMIB->OutHCUcastPkts                     = 0;
            pMIB->OutHCMulticastPkts                 = 0;
            pMIB->OutHCBroadcastPkts                 = 0;
        }

        // if the values for this port shall be reset -> copy all read values to shadowbuffer
        if (EDD_STATISTICS_RESET_VALUES == Reset)
        {
            if (bUseShadowRegister)
            {
                //transfer all read values to the shadow-registers (virtual reset)
                pShadowBuffer->ShadowStatistics.sRaw.irte = pShadowBuffer->LocalStatistics;
            }
            else
            {
                EDDI_GenResetRAWStatistics(pDDB, PortID);
            }
        }
    }
    else if (PortID==0)
    {
        //Get statistics of local interface (A+B)
        EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenCalcStatistics: Note: Not all statistics are supported for Port: 0 yet -> PortID:0x%X", PortID);

        pMIB->SupportedCounters = EDDI_MIB2_SUPPORTED_COUNTERS_IF;

        pMIB->InOctets                            = 0;
        pMIB->InUcastPkts                         = pDDB->LocalIFStats.RxGoodCtr - ((bUseShadowRegister)?pShadowBuffer->ShadowLocalIFCtr.RxGoodCtr:0);
        pMIB->InNUcastPkts                        = 0;
        pMIB->InDiscards                          = 0;
        pMIB->InErrors                            = 0;

        pMIB->InUnknownProtos                     = 0;
        pMIB->OutOctets                           = 0;
        pMIB->OutUcastPkts                        = pDDB->LocalIFStats.TxGoodCtr - ((bUseShadowRegister)?pShadowBuffer->ShadowLocalIFCtr.TxGoodCtr:0);
        pMIB->OutNUcastPkts                       = 0;

        pMIB->OutErrors                           = 0;
        pMIB->OutQLen                             = 0;
        pMIB->InMulticastPkts                     = 0;
        pMIB->InBroadcastPkts                     = 0;
        pMIB->OutMulticastPkts                    = 0;
        pMIB->OutBroadcastPkts                    = 0;
        pMIB->InHCOctets                          = 0;
        pMIB->InHCUcastPkts                       = 0;
        pMIB->InHCMulticastPkts                   = 0;
        pMIB->InHCBroadcastPkts                   = 0;
        pMIB->OutHCOctets                         = 0;
        pMIB->OutHCUcastPkts                      = 0;
        pMIB->OutHCMulticastPkts                  = 0;
        pMIB->OutHCBroadcastPkts                  = 0;

        //outdiscards is the sum of all hol-bnd-crossings
        for (Index=1; Index<(pDDB->PM.PortMap.PortCnt+1); Index++ )
        {
            Result = EDDI_GenChangeUsrPortID2HwPortIndex(Index, pDDB, &HwPortIndex);
            if (EDD_STS_OK != Result)
            {
                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, PortID:0x%X", PortID);
                return Result;
            }
        }
        pMIB->OutDiscards  =  0;

        // if the values for the if-stats shall be reset -> copy all read values to shadowbuffer
        if (EDD_STATISTICS_RESET_VALUES == Reset)
        {
            if (bUseShadowRegister)
            {
                pShadowBuffer->ShadowLocalIFCtr.OutDiscards         = 0;
                pShadowBuffer->ShadowLocalIFCtr.RxGoodCtr           = pDDB->LocalIFStats.RxGoodCtr;
                pShadowBuffer->ShadowLocalIFCtr.TxGoodCtr           = pDDB->LocalIFStats.TxGoodCtr;
            }
            else
            {
                pDDB->LocalIFStats.RxGoodCtr    = 0;
                pDDB->LocalIFStats.TxGoodCtr    = 0;
                pDDB->LocalIFStats.OutDiscards  = 0;
            }
            
        }
    }
    else //i.e. portID==5 => Error
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenCalcStatistics, wrong PortID:0x%X", PortID);
        Result=EDD_STS_ERR_PARAM;
    } 
    
    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenGetStatistics()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetStatistics( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_GET_STATISTICS_PTR_TYPE  const  pGetStatistics = (EDD_UPPER_GET_STATISTICS_PTR_TYPE)pRQB->pParam;
    LSA_RESULT                                Result         = EDD_STS_OK;

    EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenGetStatistics->");

    Result = EDDI_GenCalcStatistics(pHDB, pGetStatistics->RawUsed, pGetStatistics->Reset, &pGetStatistics->sRaw, &pGetStatistics->sMIB, pGetStatistics->PortID, LSA_TRUE);

    EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenGetStatistics<-");

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetStatisticsAll( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                   EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE  const  pGetStatistics = (EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE)pRQB->pParam;
    LSA_RESULT                                    Result         = EDD_STS_OK;
    LSA_UINT16                                    Index;
    
    EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenGetStatisticsAll->");

    //Go through all Ports (with Interface)
    for (Index=0; Index<(pHDB->pDDB->PM.PortMap.PortCnt+1); Index++)
    {
        Result = EDDI_GenCalcStatistics(pHDB, EDD_STATISTICS_GET_VALUES_MIB2/*RawUsed*/, pGetStatistics->Reset, EDDI_NULL_PTR /*not needed*/, &pGetStatistics->sMIB[Index], Index, LSA_TRUE);
        if (EDD_STS_OK != Result)
        {
            EDDI_PROGRAM_TRACE_01(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenGetStatisticsAll, PortID:0x%X", Index);
            return Result;
        }
    }
    
    EDDI_SWI_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_GenGetStatisticsAll<-");

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenResetRAWStatistics()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenResetRAWStatistics( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB,
                                                                   LSA_UINT32                const  HwPortIndex )
{
    #if defined (EDDI_CFG_FRAG_ON)
    EDDI_NRT_TX_FRAG_DDB_TYPE                *  const  pNrtTxFragData = &pDDB->NRT.TxFragData;
    #endif
    #if defined (EDDI_CFG_DEFRAG_ON)
    EDDI_NRT_RX_FRAG_TYPE                    *  const  pNrtRxFragData = &pDDB->NRT.RxFragData;
    #endif

    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_GBRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_TBRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_BTX        , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_FTX        , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_UFRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_UFRXU      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_MFRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_MFRXU      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_BFRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_UFTX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_MFTX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_BFTX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F0_64      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F64        , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F65_127    , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F128_255   , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F256_511   , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F512_1023  , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_F1024_1536 , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_GFRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_TFRX       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_FRX0_64_CRC, 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_RXOF       , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_RXOSF      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_CRCER      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_FDROP      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_TXCOL      , 0, pDDB);
    EDDI_SwiMiscSetRegister(HwPortIndex, REG_STAT_CTRL_HOLBC      , 0, pDDB);

    //Reset NRT-Fragmention Statistics
    #if defined (EDDI_CFG_FRAG_ON)
    pNrtTxFragData->Statistic[HwPortIndex].SentFragmentedFrames            = 0;    
    pNrtTxFragData->Statistic[HwPortIndex].SentNonFragmentedFrames         = 0;
    pNrtTxFragData->Statistic[HwPortIndex].DiscardedTXFrames               = 0;
    #endif   
                        
    #if defined (EDDI_CFG_DEFRAG_ON)
    pNrtRxFragData->Statistic[HwPortIndex].ReceivedFragmentedFrames        = 0;   
    pNrtRxFragData->Statistic[HwPortIndex].ReceivedNonFragmentedFrames     = 0;
    pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFrames               = 0;
    pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFragments            = 0;
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenResetStatistics()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenResetStatistics( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                  EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                                  LSA_BOOL                 const  bUseShadowRegister )
{
    EDD_UPPER_RESET_STATISTICS_PTR_TYPE         const  pResetStatistics = (EDD_UPPER_RESET_STATISTICS_PTR_TYPE)pRQB->pParam;
    LSA_RESULT                                         Result = EDD_STS_OK;
    LSA_UINT32                                         HwPortIndex;
    LSA_UINT8                                          i;
    EDDI_LOCAL_DDB_PTR_TYPE                     const  pDDB = pHDB->pDDB;

    EDDI_FUNCTION_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenResetStatistics -> PortID:0x%X", pResetStatistics->PortID);

    if (   (pResetStatistics->PortID > pHDB->pDDB->PM.PortMap.PortCnt)
        && (pResetStatistics->PortID != EDD_STATISTICS_RESET_ALL))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenResetStatistics, wrong PortID:0x%X", pResetStatistics->PortID);
        return EDD_STS_ERR_PARAM;
    }

    //for "reset" we update the shadow-structure from the hw-regs
    for (i=1; i<(pHDB->pDDB->PM.PortMap.PortCnt+1); i++)
    {
        if (pResetStatistics->PortID==i || pResetStatistics->PortID==EDD_STATISTICS_RESET_ALL)
        {
            EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE     const  pShadowBuffer  = (EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE)&(pHDB->sStatisticShadow[i]);
            #if defined (EDDI_CFG_FRAG_ON)
            EDDI_NRT_TX_FRAG_DDB_TYPE                   *  const  pNrtTxFragData = &pDDB->NRT.TxFragData;
            #endif
            #if defined (EDDI_CFG_DEFRAG_ON)
            EDDI_NRT_RX_FRAG_TYPE                       *  const  pNrtRxFragData = &pDDB->NRT.RxFragData;
            #endif

            Result = EDDI_GenChangeUsrPortID2HwPortIndex(i, pDDB, &HwPortIndex);
            if (EDD_STS_OK != Result)
            {
                EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenResetStatistics, PortID:0x%X", pResetStatistics->PortID);
                return Result;
            }
            
            if (bUseShadowRegister)
            {
                //copy all HW-Register to ShadowStatistics structure
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxGoodByte               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_GBRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxTotalByte              = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TBRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.TxTotalByte              = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BTX        , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.TxFrame                  = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FTX        , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxUniFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnUniFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRXU      , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxMulFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxUnMulFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFRXU      , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxBroadMulFrame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.TxUniFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_UFTX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.TxMulFrame               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_MFTX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.TxBroadFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_BFTX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_0_63_Frame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F0_64      , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_64_Frame            = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F64        , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_65_127_Frame        = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F65_127    , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_128_255_Frame       = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F128_255   , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_256_511_Frame       = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F256_511   , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_512_1023_Frame      = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F512_1023  , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.Size_1024_1536_Frame     = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_F1024_1536 , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxGoodFrame              = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_GFRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxTotalFrame             = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TFRX       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxSize_0_63_ErrCRC_Frame = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FRX0_64_CRC, pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxOverflowFrame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOF       , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxOversizeFrame          = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_RXOSF      , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxErrCrcAlignFrame       = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_CRCER      , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.RxFcwDbDropFrame         = EDDI_SwiPortGetDropCtr (HwPortIndex,                            pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.TxCollisionFrame         = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_TXCOL      , pDDB);
                pShadowBuffer->ShadowStatistics.sRaw.irte.HolBndCros               = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_HOLBC      , pDDB);

                #if defined (EDDI_CFG_FRAG_ON)
                pShadowBuffer->ShadowStatistics.sRaw.irte.SentFragmentedFrames      = pNrtTxFragData->Statistic[HwPortIndex].SentFragmentedFrames;
                pShadowBuffer->ShadowStatistics.sRaw.irte.SentNonFragmentedFrames   = pNrtTxFragData->Statistic[HwPortIndex].SentNonFragmentedFrames;
                pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedTXFrames         = pNrtTxFragData->Statistic[HwPortIndex].DiscardedTXFrames;
                #else
                pShadowBuffer->ShadowStatistics.sRaw.irte.SentFragmentedFrames      = 0;
                pShadowBuffer->ShadowStatistics.sRaw.irte.SentNonFragmentedFrames   = 0;
                pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedTXFrames         = 0;
                #endif  

                #if defined (EDDI_CFG_DEFRAG_ON)
                pShadowBuffer->ShadowStatistics.sRaw.irte.ReceivedFragmentedFrames      = pNrtRxFragData->Statistic[HwPortIndex].ReceivedFragmentedFrames;
                pShadowBuffer->ShadowStatistics.sRaw.irte.ReceivedNonFragmentedFrames   = pNrtRxFragData->Statistic[HwPortIndex].ReceivedNonFragmentedFrames;
                pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedRXFrames             = pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFrames;
                pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedRXFragments          = pNrtRxFragData->Statistic[HwPortIndex].DiscardedRXFragments;
                #else
                pShadowBuffer->ShadowStatistics.sRaw.irte.ReceivedFragmentedFrames      = 0;
                pShadowBuffer->ShadowStatistics.sRaw.irte.ReceivedNonFragmentedFrames   = 0;
                pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedRXFrames             = 0;
                pShadowBuffer->ShadowStatistics.sRaw.irte.DiscardedRXFragments          = 0;
                #endif   
            }
            else
            {
                EDDI_GenResetRAWStatistics(pDDB, i);
            }
        }
    }

    //for "reset" we update the shadow-structure from the sw-ctrs
    if ((0 == pResetStatistics->PortID) || (EDD_STATISTICS_RESET_ALL == pResetStatistics->PortID))
    {
        if (bUseShadowRegister)
        {
            EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE  const  pShadowBuffer = (EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE)&(pHDB->sStatisticShadow[0]);

            //Clear IF
            pShadowBuffer->ShadowLocalIFCtr.OutDiscards = 0;
            pShadowBuffer->ShadowLocalIFCtr.RxGoodCtr   = pDDB->LocalIFStats.RxGoodCtr;
            pShadowBuffer->ShadowLocalIFCtr.TxGoodCtr   = pDDB->LocalIFStats.TxGoodCtr;
        }
        else
        {
            EDDI_GenResetRAWStatistics(pDDB, 0);
        }
    }

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenLinkIndProvide()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenLinkIndProvide( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                 EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                                 LSA_BOOL                     *  bIndicate )
{
    LSA_RESULT                                         Result;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE         pLinkStatus_Ind;
    LSA_BOOL                                    bIndProvideExt;
    LSA_UINT32                                  HwPortIndex = 0;
    EDDI_LOCAL_DDB_PTR_TYPE                     const  pDDB        = pHDB->pDDB;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLinkIndProvide->");

    //send an indication
    *bIndicate = LSA_TRUE;

    bIndProvideExt  = (LSA_BOOL)((EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT) ? LSA_TRUE : LSA_FALSE);
    pLinkStatus_Ind = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

    //max nr of ports exceeded
    if (pLinkStatus_Ind->PortID > pDDB->PM.PortMap.PortCnt)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        return EDD_STS_ERR_PARAM;
    }

    if (EDD_PORT_ID_AUTO == pLinkStatus_Ind->PortID)
    {
        //no AUTO allowed for IND_PROVIDE_EXT, because an average over all LineDelays can not be calculated 
        if (bIndProvideExt)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            return EDD_STS_ERR_PARAM;
        }
    }
    else
    {
        //convert PortID to EDDIPort
        Result = EDDI_GenChangeUsrPortID2HwPortIndex(pLinkStatus_Ind->PortID, pDDB, &HwPortIndex);
        if (EDD_STS_OK != Result)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            return Result;
        }

        //1st Indication
        if (LinkIndProvide_NoExist == pHDB->LinkIndProvideType[HwPortIndex])
        {
            pHDB->LinkIndProvideType[HwPortIndex] = (LSA_UINT16)((EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_LINK_STATUS_IND_PROVIDE) ? LinkIndProvide_Normal : LinkIndProvide_EXT);
        }

        //Only 1 IndicationType per Channel and Port allowed
        if (   ((LinkIndProvide_Normal == pHDB->LinkIndProvideType[HwPortIndex]) && ( bIndProvideExt))
            || ((LinkIndProvide_EXT    == pHDB->LinkIndProvideType[HwPortIndex]) && (!bIndProvideExt)))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_SEQUENCE);
            return EDD_STS_ERR_SEQUENCE;
        }
    }

    if (EDD_PORT_ID_AUTO == pLinkStatus_Ind->PortID) //ERTEC is seen as 1 card with 1 physical IF
    {
        if ((pLinkStatus_Ind->Status != pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkStatus )  ||
            (pLinkStatus_Ind->Mode   != pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkMode   )  ||
            (pLinkStatus_Ind->Speed  != pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkSpeed  ))
        {
            //Linkstatus differs from that before, indicate immediately
            pLinkStatus_Ind->Status = pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkStatus;
            pLinkStatus_Ind->Speed  = pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkSpeed;
            pLinkStatus_Ind->Mode   = pDDB->pLocal_SWITCH->AutoLastLinkStatus.LinkMode;
            return EDD_STS_OK;
        }
    }
    else
    {
        SWI_LINK_PARAM_TYPE          *  const  pLinkPx         = &pDDB->pLocal_SWITCH->LinkPx[HwPortIndex];
        SWI_LINK_IND_EXT_PARAM_TYPE  *  const  pLinkIndExtPara = &pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex];

        //EDD_SRV_LINK_STATUS_IND_PROVIDE
        //EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT
        if ((pLinkStatus_Ind->Status                   != pLinkPx->LastLinkStatus                    ) ||
            (pLinkStatus_Ind->Mode                     != pLinkPx->LastLinkMode                      ) ||
            (pLinkStatus_Ind->Speed                    != pLinkPx->LastLinkSpeed                     ) ||

            (bIndProvideExt 
            && ( 
                (pLinkStatus_Ind->LineDelayInTicksMeasured != pLinkIndExtPara->LastLineDelayInTicksMeasured) ||
                (pLinkStatus_Ind->CableDelayInNsMeasured   != pLinkIndExtPara->LastCableDelayInNsMeasured  ) ||
                (pLinkStatus_Ind->PortTxDelay              != pLinkIndExtPara->LastPortTxDelay             ) ||
                (pLinkStatus_Ind->PortRxDelay              != pLinkIndExtPara->LastPortRxDelay             ) ||
                (pLinkStatus_Ind->PortStatus               != pLinkIndExtPara->LastPortStatus              ) ||
                (pLinkStatus_Ind->PhyStatus                != pLinkIndExtPara->LastPhyStatus               ) ||
                (pLinkStatus_Ind->Autoneg                  != pLinkIndExtPara->LastAutoneg                 ) ||
                (pLinkStatus_Ind->IRTPortStatus            != pLinkIndExtPara->LastIRTPortStatus           ) ||
                (pLinkStatus_Ind->RTClass2_PortStatus      != pLinkIndExtPara->LastRTClass2_PortStatus     ) ||
                (pLinkStatus_Ind->PortState                != pLinkIndExtPara->LastPortState               ) ||
                (pLinkStatus_Ind->MAUType                  != pLinkIndExtPara->LastMAUType                 ) ||
                (pLinkStatus_Ind->MediaType                != pLinkIndExtPara->LastMediaType               ) ||
                (pLinkStatus_Ind->IsPOF                    != pLinkIndExtPara->LastIsPOF                   ) ||
                (pLinkStatus_Ind->SyncId0_TopoOk           != pLinkIndExtPara->LastSyncId0_TopoOk          ) ||
                (pLinkStatus_Ind->AddCheckEnabled          != pLinkIndExtPara->AddCheckEnabled             ) ||
                (pLinkStatus_Ind->ChangeReason             != pLinkIndExtPara->ChangeReason                ) ||
                #if defined (EDDI_CFG_FRAG_ON)
                (pLinkStatus_Ind->TxFragmentation          != pLinkIndExtPara->LastTxFragmentation         )
                #else
                (pLinkStatus_Ind->TxFragmentation                                                          )
                #endif
                )
            )
            )
        {
            //Linkstatus differs from that before, indicate immediately
            pLinkStatus_Ind->Status                   = pLinkPx->LastLinkStatus;
            pLinkStatus_Ind->Speed                    = pLinkPx->LastLinkSpeed;
            pLinkStatus_Ind->Mode                     = pLinkPx->LastLinkMode;
            pLinkStatus_Ind->LineDelayInTicksMeasured = pLinkIndExtPara->LastLineDelayInTicksMeasured;
            pLinkStatus_Ind->CableDelayInNsMeasured   = pLinkIndExtPara->LastCableDelayInNsMeasured;
            pLinkStatus_Ind->PortTxDelay              = pLinkIndExtPara->LastPortTxDelay;
            pLinkStatus_Ind->PortRxDelay              = pLinkIndExtPara->LastPortRxDelay;
            pLinkStatus_Ind->PortStatus               = pLinkIndExtPara->LastPortStatus;
            pLinkStatus_Ind->PhyStatus                = pLinkIndExtPara->LastPhyStatus;
            pLinkStatus_Ind->Autoneg                  = pLinkIndExtPara->LastAutoneg;
            pLinkStatus_Ind->IRTPortStatus            = pLinkIndExtPara->LastIRTPortStatus;
            pLinkStatus_Ind->PortState                = pLinkIndExtPara->LastPortState;
            pLinkStatus_Ind->MAUType                  = pLinkIndExtPara->LastMAUType;
            pLinkStatus_Ind->MediaType                = pLinkIndExtPara->LastMediaType;
            pLinkStatus_Ind->IsPOF                    = pLinkIndExtPara->LastIsPOF;
            pLinkStatus_Ind->RTClass2_PortStatus      = pLinkIndExtPara->LastRTClass2_PortStatus;
            pLinkStatus_Ind->SyncId0_TopoOk           = pLinkIndExtPara->LastSyncId0_TopoOk;
            pLinkStatus_Ind->ShortPreamble            = EDD_PORT_LONG_PREAMBLE;                    // not supported by IRTE
            #if defined (EDDI_CFG_FRAG_ON)
            pLinkStatus_Ind->TxFragmentation          = pLinkIndExtPara->TxFragmentation;
            #else
            pLinkStatus_Ind->TxFragmentation          = EDD_PORT_NO_TX_FRAGMENTATION;
            #endif
            pLinkStatus_Ind->AddCheckEnabled          = pLinkIndExtPara->AddCheckEnabled;
            pLinkStatus_Ind->ChangeReason             = pLinkIndExtPara->ChangeReason;

            return EDD_STS_OK;
        }
    }

    //requeue resource, don´t indicate
    *bIndicate = LSA_FALSE;

    if (EDD_PORT_ID_AUTO == pLinkStatus_Ind->PortID)
    {
        EDDI_AddToQueueEnd(pDDB, &pHDB->AutoLinkIndReq, pRQB);
    }
    else
    {
        EDDI_AddToQueueEnd(pDDB, &pHDB->PortIDLinkIndReq[HwPortIndex], pRQB);
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLinkIndProvide<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenChangeUsrPortID2HwPortIndex()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenChangeUsrPortID2HwPortIndex( LSA_UINT16                  const  UsrPortID,
                                                                      EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                      LSA_UINT32               *  const  pHwPortIndex )
{
    LSA_UINT32  index;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenChangeUsrPortID2HwPortIndex->");

    if (   (0 == UsrPortID)
        || (UsrPortID > pDDB->PM.PortMap.PortCnt))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                              "EDDI_GenChangeUsrPortID2HwPortIndex, PortID:0x%X",
                              UsrPortID);
        return EDD_STS_ERR_PARAM;
    }

    index = EDDI_PmUsrPortIDToHwPort03(pDDB, UsrPortID);

    *pHwPortIndex = index;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenChangeUsrPortID2HwPortIndex");

    return EDD_STS_OK;
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
static  LSA_UINT  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetRatingSpeedMode( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                                   SWI_LINK_STAT_TYPE       const *  const  pBestLinkState )
{
    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetRatingSpeedMode->");

    //Link_Down has no information about speed or mode
    if (EDD_LINK_DOWN == pBestLinkState->LinkStatus)
    {
        return 0;
    }

    if ((EDD_LINK_SPEED_1000 == pBestLinkState->LinkSpeed) && (EDD_LINK_MODE_FULL == pBestLinkState->LinkMode))
    {
        switch (pBestLinkState->LinkStatus)
        {
            case EDD_LINK_UP:
            {
                return 18;
            }
            case EDD_LINK_UP_CLOSED:
            {
                return 12;
            }
            case EDD_LINK_UP_DISABLED:
            {
                return 6;
            }
            default:
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 1, illegal LinkStatus (0x%X), LinkSpeed:0x%X, LinkMode:0x%X ", 
                                    pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
                EDDI_Excp("EDDI_GenGetRatingSpeedMode 1", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
                return 0;
            }
        }
    }
    else if ((EDD_LINK_SPEED_1000 == pBestLinkState->LinkSpeed) && (EDD_LINK_MODE_HALF == pBestLinkState->LinkMode))
    {
        switch (pBestLinkState->LinkStatus)
        {
            case EDD_LINK_UP:
            {
                return 17;
            }
            case EDD_LINK_UP_CLOSED:
            {
                return 11;
            }
            case EDD_LINK_UP_DISABLED:
            {
                return 5;
            }
            default:
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 2, illegal LinkStatus (0x%X), LinkSpeed:0x%X, LinkMode:0x%X ", 
                                    pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
                EDDI_Excp("EDDI_GenGetRatingSpeedMode 2", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
                return 0;
            }
        }
    }
    else if ((EDD_LINK_SPEED_100 == pBestLinkState->LinkSpeed) && (EDD_LINK_MODE_FULL == pBestLinkState->LinkMode))
    {
        switch (pBestLinkState->LinkStatus)
        {
            case EDD_LINK_UP:
            {
                return 16;
            }
            case EDD_LINK_UP_CLOSED:
            {
                return 10;
            }
            case EDD_LINK_UP_DISABLED:
            {
                return 4;
            }
            default:
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 3, illegal LinkStatus (0x%X), LinkSpeed:0x%X, LinkMode:0x%X ", 
                                    pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
                EDDI_Excp("EDDI_GenGetRatingSpeedMode 3", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
                return 0;
            }
        }
    }
    else if ((EDD_LINK_SPEED_100 == pBestLinkState->LinkSpeed) && (EDD_LINK_MODE_HALF == pBestLinkState->LinkMode))
    {
        switch (pBestLinkState->LinkStatus)
        {
            case EDD_LINK_UP:
            {
                return 15;
            }
            case EDD_LINK_UP_CLOSED:
            {
                return 9;
            }
            case EDD_LINK_UP_DISABLED:
            {
                return 3;
            }
            default:
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 4, illegal LinkStatus (0x%X), LinkSpeed:0x%X, LinkMode:0x%X ", 
                                    pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
                EDDI_Excp("EDDI_GenGetRatingSpeedMode 4", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
                return 0;
            }
        }
    }
    else if ((EDD_LINK_SPEED_10 == pBestLinkState->LinkSpeed) && (EDD_LINK_MODE_FULL == pBestLinkState->LinkMode))
    {
        switch (pBestLinkState->LinkStatus)
        {
            case EDD_LINK_UP:
            {
                return 14;
            }
            case EDD_LINK_UP_CLOSED:
            {
                return 8;
            }
            case EDD_LINK_UP_DISABLED:
            {
                return 2;
            }
            default:
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 5, illegal LinkStatus (0x%X), LinkSpeed:0x%X, LinkMode:0x%X ", 
                                    pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
                EDDI_Excp("EDDI_GenGetRatingSpeedMode 5", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
                return 0;
            }
        }
    }
    else if ((EDD_LINK_SPEED_10 == pBestLinkState->LinkSpeed) && (EDD_LINK_MODE_HALF == pBestLinkState->LinkMode))
    {
        switch (pBestLinkState->LinkStatus)
        {
            case EDD_LINK_UP:
            {
                return 13;
            }
            case EDD_LINK_UP_CLOSED:
            {
                return 7;
            }
            case EDD_LINK_UP_DISABLED:
            {
                return 1;
            }
            default:
            {
                EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 6, illegal LinkStatus (0x%X), LinkSpeed:0x%X, LinkMode:0x%X ", 
                                    pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
                EDDI_Excp("EDDI_GenGetRatingSpeedMode 6", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
                return 0;
            }
        }
    }
    else
    {
        LSA_UNUSED_ARG(pDDB);
        EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenGetRatingSpeedMode 7, illegal LinkStatus (0x%X) and LinkSpeed(0x%X) and LinkMode(0x%X) ", 
                            pBestLinkState->LinkStatus, pBestLinkState->LinkSpeed, pBestLinkState->LinkMode); 
        EDDI_Excp("EDDI_GenGetRatingSpeedMode 7", EDDI_FATAL_ERR_EXCP, pBestLinkState->LinkStatus, 0);
        return 0;
    }
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetBestAutoLinkStatus( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                      SWI_LINK_STAT_TYPE       *  const  pBestLinkState )
{
    LSA_UINT32                 UsrPortIndex;
    LSA_UINT                   BestLinkStateRating;
    LSA_UINT                   ActualLinkStateRating;
    SWI_LINK_STAT_TYPE         ActualLinkState;
    LSA_BOOL                   bClosed;
    LSA_BOOL                   bDisabled;
    LSA_UINT32          const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetBestAutoLinkStatus->");

    pBestLinkState->LinkStatus = EDD_LINK_DOWN;
    pBestLinkState->LinkSpeed  = EDD_LINK_UNKNOWN;
    pBestLinkState->LinkMode   = EDD_LINK_UNKNOWN;
    BestLinkStateRating        = 0;

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        ActualLinkState.LinkStatus  = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus;
        ActualLinkState.LinkSpeed   = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed;
        ActualLinkState.LinkMode    = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode;

        EDDI_SwiMiscGetClosedDisabled(HwPortIndex, &bClosed, &bDisabled, pDDB);

        if (EDD_LINK_UP == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus)
        {
            if (bDisabled)  //prio(Disable) is greater than prio(Closed)
            {
                ActualLinkState.LinkStatus = EDD_LINK_UP_DISABLED;
            }
            else if (bClosed)
            {
                ActualLinkState.LinkStatus = EDD_LINK_UP_CLOSED;
            }
            else
            {
                ActualLinkState.LinkStatus = EDD_LINK_UP;
            }
        }

        ActualLinkStateRating = EDDI_GenGetRatingSpeedMode(pDDB, &ActualLinkState);

        if (ActualLinkStateRating > BestLinkStateRating)
        {
            BestLinkStateRating = ActualLinkStateRating;
            *pBestLinkState     = ActualLinkState;
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetBestAutoLinkStatus<-");
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetBestPortIDLinkStatus( LSA_UINT32                  const  HwPortIndex,
                                                                        SWI_LINK_STAT_TYPE       *  const  pBestLinkState,
                                                                        EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_BOOL  bClosed;
    LSA_BOOL  bDisabled;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetBestPortIDLinkStatus->");

    //temporarily_disabled_lint - -e676 Possibly negative subscript
    pBestLinkState->LinkStatus = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus;
    pBestLinkState->LinkSpeed  = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed;
    pBestLinkState->LinkMode   = pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode;

    EDDI_SwiMiscGetClosedDisabled(HwPortIndex, &bClosed, &bDisabled, pDDB);

    //If EDD_LINK_UP is indicated, indicate also the portstates "closed" and "disabled" ->
    //"EDD_LINK_UP_CLOSED" or "EDD_LINK_UP_DISABLED".
    if (EDD_LINK_UP == pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkStatus)
    {
        if (bDisabled) //prio(Disable) is greater than prio(Closed)
        {
            pBestLinkState->LinkStatus = EDD_LINK_UP_DISABLED;
        }
        else if (bClosed)
        {
            pBestLinkState->LinkStatus = EDD_LINK_UP_CLOSED;
        }
        else
        {
            pBestLinkState->LinkStatus = EDD_LINK_UP;
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenGetBestPortIDLinkStatus<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*                            extern function definition                     */
/*===========================================================================*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenLinkInterrupt()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     pPortChanged: pointer to boolean-array with      */
/*                                      index = HwPortIndex                */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenLinkInterrupt( LSA_BOOL                 const  *  const  pPortChanged,
                                                      EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE            pLinkStatus_Ind;
    #if (EDDI_CFG_TRACE_MODE != 0)
    SWI_LINK_IND_EXT_PARAM_TYPE                           OldLinkVals;    //volatile??
    #endif
    LSA_UINT32                                            IndexHandle;
    EDDI_LOCAL_HDB_PTR_TYPE                               pHDB;
    EDD_UPPER_RQB_PTR_TYPE                                pRQB;
    SWI_LINK_STAT_TYPE                                    BestLinkState;
    LSA_UINT32                                            UsrPortIndex;
    LSA_BOOL                                              LinkChanged;
    LSA_BOOL                                              LinkExtendChange;
    SWI_LINK_STAT_TYPE                          *  const  pAutoLastLinkStatus = &pDDB->pLocal_SWITCH->AutoLastLinkStatus;
    LSA_UINT32                                     const  PortMapCnt          = pDDB->PM.PortMap.PortCnt;
    LSA_BOOL                                              bTraceOutput = LSA_TRUE;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLinkInterrupt->");

    #if (EDDI_CFG_TRACE_MODE != 0)
    OldLinkVals.Autoneg = 0; //satisfy lint!
    #endif

    //EDD_PORT_ID_AUTO
    //ERTEC is seen as 1 card with 1 physical IF
    EDDI_GenGetBestAutoLinkStatus(pDDB, &BestLinkState);

    //only indicate if LinkStatus has changed
    //and AutoLastLinkStatus is already initialized
    if (   (pAutoLastLinkStatus->LinkStatus != BestLinkState.LinkStatus)
        || (pAutoLastLinkStatus->LinkMode   != BestLinkState.LinkMode)
        || (pAutoLastLinkStatus->LinkSpeed  != BestLinkState.LinkSpeed))
    {
        //store last general LinkStatus
        pAutoLastLinkStatus->LinkStatus = BestLinkState.LinkStatus;
        pAutoLastLinkStatus->LinkSpeed  = BestLinkState.LinkSpeed;
        pAutoLastLinkStatus->LinkMode   = BestLinkState.LinkMode;

        //inform all channels, evaluate all ports (EDD_PORT_ID_AUTO)
        IndexHandle = pDDB->HDBQueue.Count+1;
        
        pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)(void *)EDDI_QueueGetNext(&pDDB->HDBQueue, 0);  //get first element
        do 
        {
            if (pHDB)
            {
                IndexHandle--;
                if (pHDB->InUse)
                {
                    //get/remove RQB from general queue
                    pRQB = EDDI_RemoveFromQueue(pDDB, &pHDB->AutoLinkIndReq);
                    if (EDDI_NULL_PTR != pRQB)
                    {
                        //indicate link change
                        pLinkStatus_Ind = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE)pRQB->pParam;

                        pLinkStatus_Ind->Status = BestLinkState.LinkStatus;
                        pLinkStatus_Ind->Speed  = BestLinkState.LinkSpeed;
                        pLinkStatus_Ind->Mode   = BestLinkState.LinkMode;

                        if (bTraceOutput)
                        {
                            EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_GenLinkInterrupt, PortID:%04X Status:%04X Speed:%04X Mode:%04X UpperHandle:%08X",
                                                  pLinkStatus_Ind->PortID, pLinkStatus_Ind->Status, pLinkStatus_Ind->Speed, pLinkStatus_Ind->Mode, pHDB->UpperHandle);
                            bTraceOutput = LSA_FALSE;
                        }

                        EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
                    }
                }
                pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)(void *)EDDI_QueueGetNext(&pDDB->HDBQueue, &pHDB->QueueLink);  //get next element
            }
            else
            {
                break;
            }
        }
        while (IndexHandle);

        if (0 == IndexHandle)
        {
            EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenLinkInterrupt, HDB QUEUE CORRUPT");
            EDDI_Excp("EDDI_GenLinkInterrupt, HDB QUEUE CORRUPT", EDDI_FATAL_ERR_EXCP, pDDB->DeviceNr, 0);
            return;
        }
    }

    //check all connected ports
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32                      const  HwPortIndex     = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);
        SWI_LINK_PARAM_TYPE          *  const  pLinkPx         = &pDDB->pLocal_SWITCH->LinkPx[HwPortIndex];
        SWI_LINK_IND_EXT_PARAM_TYPE  *  const  pLinkIndExtPara = &pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex];

        //no change occured at this port
        //temporarily_disabled_lint - -e676 Possibly negative subscript
        if (!pPortChanged[HwPortIndex])
        {
            continue;
        }

        LinkChanged      = LSA_FALSE;
        LinkExtendChange = LSA_FALSE;

        EDDI_GenGetBestPortIDLinkStatus(HwPortIndex, &BestLinkState, pDDB);

        //only indicate if current link evaluation differs from the last one
        //adapt Last-values if they are different
        //==>Trigger is derived from these!
        if (   (pLinkPx->LastLinkStatus != BestLinkState.LinkStatus)
            || (pLinkPx->LastLinkMode   != BestLinkState.LinkMode)
            || (pLinkPx->LastLinkSpeed  != BestLinkState.LinkSpeed))
        {
            //remember last portspecific LinkStatus
            pLinkPx->LastLinkStatus = BestLinkState.LinkStatus;
            pLinkPx->LastLinkSpeed  = BestLinkState.LinkSpeed;
            pLinkPx->LastLinkMode   = BestLinkState.LinkMode;

            pDDB->pProfKRamInfo->info.PHY_LinkStatus[HwPortIndex] = BestLinkState.LinkStatus;
            pDDB->pProfKRamInfo->info.PHY_LinkMode[HwPortIndex]   = BestLinkState.LinkMode;
            pDDB->pProfKRamInfo->info.PHY_LinkSpeed[HwPortIndex]  = BestLinkState.LinkSpeed;

            LinkChanged = LSA_TRUE;
        }

        {
            LSA_UINT32  AutonegCapAdvertised;
            LSA_UINT32  AutonegMappingCapability;
            LSA_UINT8   MediaType;
            LSA_UINT8   IsPOF;
            LSA_UINT8   FXTransceiverType;

            //Get current MAUType calculated from speed and mode. MediaType and Capabilities are not interesting here (are not allowed to change here!)
            EDDI_LL_GET_PHY_PARAMS(pDDB->hSysDev, HwPortIndex, BestLinkState.LinkSpeed, BestLinkState.LinkMode,
                                   &AutonegCapAdvertised,
                                   &AutonegMappingCapability,
                                   &(pLinkIndExtPara->MAUType),
                                   &MediaType,
                                   &IsPOF,
                                   &FXTransceiverType);
        }

        //only indicate if current link-ext evaluation differs from the last one
        //adapt last-values if they are different
        //==>Trigger is derived from these!

        //remember last portspecific LinkStatus
        #if (EDDI_CFG_TRACE_MODE != 0)
        OldLinkVals = *pLinkIndExtPara;
        #endif

        if (   (pLinkIndExtPara->LastLineDelayInTicksMeasured != pLinkIndExtPara->LineDelayInTicksMeasured    )
            || (pLinkIndExtPara->LastCableDelayInNsMeasured   != pLinkIndExtPara->CableDelayInNsMeasured      )
            || (pLinkIndExtPara->LastPortTxDelay              != pLinkIndExtPara->PortTxDelay                 )
            || (pLinkIndExtPara->LastPortRxDelay              != pLinkIndExtPara->PortRxDelay                 )
            || (pLinkIndExtPara->LastPortStatus               != pLinkIndExtPara->PortStatus                  )
            || (pLinkIndExtPara->LastPhyStatus                != pLinkIndExtPara->PhyStatus                   )
            || (pLinkIndExtPara->LastAutoneg                  != pLinkIndExtPara->Autoneg                     )
            || (pLinkIndExtPara->LastIRTPortStatus            != pLinkIndExtPara->IRTPortStatus               )
            || (pLinkIndExtPara->LastPortState                != pLinkIndExtPara->PortState                   )
            || (pLinkIndExtPara->LastMAUType                  != pLinkIndExtPara->MAUType                     )
            || (pLinkIndExtPara->LastMediaType                != pDDB->Glob.PortParams[HwPortIndex].MediaType )
            || (pLinkIndExtPara->LastIsPOF                    != pDDB->Glob.PortParams[HwPortIndex].IsPOF     )
            || (pLinkIndExtPara->LastRTClass2_PortStatus      != pLinkIndExtPara->RTClass2_PortStatus         )
            || (pLinkIndExtPara->LastSyncId0_TopoOk           != pLinkIndExtPara->SyncId0_TopoOk              )
            #if defined (EDDI_CFG_FRAG_ON)
            || (pLinkIndExtPara->LastTxFragmentation          != pLinkIndExtPara->TxFragmentation             )
            #endif
            || (pLinkIndExtPara->LastAddCheckEnabled          != pLinkIndExtPara->AddCheckEnabled             ) 
            || (pLinkIndExtPara->LastChangeReason             != pLinkIndExtPara->ChangeReason                ) 
            )
        {
            pLinkIndExtPara->LastLineDelayInTicksMeasured = pLinkIndExtPara->LineDelayInTicksMeasured;
            pLinkIndExtPara->LastCableDelayInNsMeasured   = pLinkIndExtPara->CableDelayInNsMeasured;
            pLinkIndExtPara->LastPortTxDelay              = pLinkIndExtPara->PortTxDelay;
            pLinkIndExtPara->LastPortRxDelay              = pLinkIndExtPara->PortRxDelay;
            pLinkIndExtPara->LastPortStatus               = pLinkIndExtPara->PortStatus;
            pLinkIndExtPara->LastPhyStatus                = pLinkIndExtPara->PhyStatus;
            pLinkIndExtPara->LastAutoneg                  = pLinkIndExtPara->Autoneg;
            pLinkIndExtPara->LastIRTPortStatus            = pLinkIndExtPara->IRTPortStatus;
            pLinkIndExtPara->LastPortState                = pLinkIndExtPara->PortState;
            pLinkIndExtPara->LastMAUType                  = pLinkIndExtPara->MAUType;
            pLinkIndExtPara->LastMediaType                = pDDB->Glob.PortParams[HwPortIndex].MediaType;
            pLinkIndExtPara->LastIsPOF                    = pDDB->Glob.PortParams[HwPortIndex].IsPOF;
            pLinkIndExtPara->LastRTClass2_PortStatus      = pLinkIndExtPara->RTClass2_PortStatus;
            pLinkIndExtPara->LastSyncId0_TopoOk           = pLinkIndExtPara->SyncId0_TopoOk;
            #if defined (EDDI_CFG_FRAG_ON)
            pLinkIndExtPara->LastTxFragmentation          = pLinkIndExtPara->TxFragmentation;
            #endif
            pLinkIndExtPara->LastAddCheckEnabled          = pLinkIndExtPara->AddCheckEnabled;
            pLinkIndExtPara->LastChangeReason             = pLinkIndExtPara->ChangeReason;
            LinkExtendChange = LSA_TRUE;
        }

        #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
        if (!pDDB->bFilterLinkIndTrace)
        #endif
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenLinkInterrupt, LinkChanged:%08X LinkExtendChange:%08X",
                                  LinkChanged, LinkExtendChange);
        }

        //no change occured
        if ((!LinkChanged) && (!LinkExtendChange))
        {
            continue;
        }

        bTraceOutput = LSA_TRUE;
        //inform all channels, EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE
        IndexHandle = pDDB->HDBQueue.Count+1;

        pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)(void *)EDDI_QueueGetNext(&pDDB->HDBQueue, 0);  //get first element
        do
        {
            if (pHDB)
            {
                IndexHandle--;
                if (pHDB->InUse)
                {
                    //only LinkChange is relevant for service EDD_SRV_LINK_STATUS_IND_PROVIDE
                    if ((pHDB->LinkIndProvideType[HwPortIndex] != LinkIndProvide_Normal) || LinkChanged)
                    {
                        pRQB = EDDI_RemoveFromQueue(pHDB->pDDB, &pHDB->PortIDLinkIndReq[HwPortIndex]);
                        if (EDDI_NULL_PTR != pRQB)
                        {
                            //indicate link change
                            pLinkStatus_Ind = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE)pRQB->pParam;

                            pLinkStatus_Ind->Status = BestLinkState.LinkStatus;

                            //*** build indication ***
                            if (   (bTraceOutput)
                                #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
                                && (!pDDB->bFilterLinkIndTrace)
                                #endif
                               )
                            {
                                EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "old: EDDI_GenLinkInterrupt, EXT -> PortID:%04X, Status:%04X, Speed:%04X, Mode:%04X, UpperHandle:%08X",
                                                      pLinkStatus_Ind->PortID, pLinkStatus_Ind->Status, pLinkStatus_Ind->Speed, pLinkStatus_Ind->Mode, pHDB->UpperHandle);
                                EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "NEW: EDDI_GenLinkInterrupt, EXT -> PortID:%04X, Status:%04X, Speed:%04X, Mode:%04X, UpperHandle:%08X",
                                                      pLinkStatus_Ind->PortID, pLinkStatus_Ind->Status, BestLinkState.LinkSpeed, BestLinkState.LinkMode, pHDB->UpperHandle);
                            }
                            else
                            {
                                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenLinkInterrupt, EXT -> UpperHandle:%08X", pHDB->UpperHandle);
                            }

                            pLinkStatus_Ind->Speed                    = BestLinkState.LinkSpeed;
                            pLinkStatus_Ind->Mode                     = BestLinkState.LinkMode;
                            pLinkStatus_Ind->LineDelayInTicksMeasured = pLinkIndExtPara->LineDelayInTicksMeasured;
                            pLinkStatus_Ind->CableDelayInNsMeasured   = pLinkIndExtPara->CableDelayInNsMeasured;
                            pLinkStatus_Ind->PortTxDelay              = pLinkIndExtPara->PortTxDelay;
                            pLinkStatus_Ind->PortRxDelay              = pLinkIndExtPara->PortRxDelay;
                            pLinkStatus_Ind->PortStatus               = pLinkIndExtPara->PortStatus;
                            pLinkStatus_Ind->PhyStatus                = pLinkIndExtPara->PhyStatus;
                            pLinkStatus_Ind->MAUType                  = pLinkIndExtPara->MAUType;
                            pLinkStatus_Ind->MediaType                = pDDB->Glob.PortParams[HwPortIndex].MediaType;
                            pLinkStatus_Ind->IsPOF                    = pDDB->Glob.PortParams[HwPortIndex].IsPOF;
                            pLinkStatus_Ind->IRTPortStatus            = pLinkIndExtPara->IRTPortStatus;
                            pLinkStatus_Ind->RTClass2_PortStatus      = pLinkIndExtPara->RTClass2_PortStatus;
                            pLinkStatus_Ind->Autoneg                  = pLinkIndExtPara->Autoneg;
                            pLinkStatus_Ind->PortState                = pLinkIndExtPara->PortState;
                            pLinkStatus_Ind->SyncId0_TopoOk           = pLinkIndExtPara->SyncId0_TopoOk;
                            #if defined (EDDI_CFG_FRAG_ON)
                            pLinkStatus_Ind->TxFragmentation          = pLinkIndExtPara->TxFragmentation;
                            #else
                            pLinkStatus_Ind->TxFragmentation          = EDD_PORT_NO_TX_FRAGMENTATION;
                            #endif

                            if (pHDB->LinkIndProvideType[HwPortIndex] == LinkIndProvide_EXT)
                            {
                                if (   (bTraceOutput)
                                    #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
                                    && (!pDDB->bFilterLinkIndTrace)
                                    #endif
                                   )
                                {
                                    #if (EDDI_CFG_TRACE_MODE != 0)
                                    #if defined (EDDI_CFG_FRAG_ON)
                                    #define  bOldTxFragmentation OldLinkVals.TxFragmentation
                                    #else
                                    #define  bOldTxFragmentation EDD_PORT_NO_TX_FRAGMENTATION
                                    #endif
                                    #endif

                                    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "old: EDDI_GenLinkInterrupt, EXT -> LineDelayInTicksHardware:%08X LineDelayInTicksMeasured:%08X PortTxDelay:%08X PortRxDelay:%08X",
                                                          OldLinkVals.LastLineDelayInTicksMeasured, OldLinkVals.LastCableDelayInNsMeasured, OldLinkVals.LastPortTxDelay, OldLinkVals.LastPortRxDelay);
                                    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "NEW: EDDI_GenLinkInterrupt, EXT -> LineDelayInTicksHardware:%08X LineDelayInTicksMeasured:%08X PortTxDelay:%08X PortRxDelay:%08X",
                                                          pLinkStatus_Ind->LineDelayInTicksMeasured, pLinkStatus_Ind->CableDelayInNsMeasured, pLinkStatus_Ind->PortTxDelay, pLinkStatus_Ind->PortRxDelay);

                                    EDDI_PROGRAM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "old: EDDI_GenLinkInterrupt, EXT -> PortStatus:%02X PhyStatus:%02X PortState:%04X Autoneg:%02X MAUType:%04X Mediatype:%02X IsPOF:%02X",
                                                          OldLinkVals.LastPortStatus, OldLinkVals.LastPhyStatus, OldLinkVals.LastPortState, OldLinkVals.LastAutoneg, OldLinkVals.LastMAUType, OldLinkVals.LastMediaType, OldLinkVals.LastIsPOF);
                                    EDDI_PROGRAM_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "NEW: EDDI_GenLinkInterrupt, EXT -> PortStatus:%02X PhyStatus:%02X PortState:%04X Autoneg:%02X MAUType:%04X Mediatype:%02X IsPOF:%02X",
                                                          pLinkStatus_Ind->PortStatus, pLinkStatus_Ind->PhyStatus, pLinkStatus_Ind->PortState, pLinkStatus_Ind->Autoneg, pLinkStatus_Ind->MAUType, pLinkStatus_Ind->MediaType, pLinkStatus_Ind->IsPOF);

                                    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "old: EDDI_GenLinkInterrupt, EXT -> IRTPortStatus:%02X RTClass2_PortStatus:%02X SyncId0_TopoOk:%02X TxFragmentation:%02X",
                                                          OldLinkVals.LastIRTPortStatus, OldLinkVals.LastRTClass2_PortStatus, OldLinkVals.LastSyncId0_TopoOk, bOldTxFragmentation);
                                    EDDI_PROGRAM_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "NEW: EDDI_GenLinkInterrupt, EXT -> IRTPortStatus:%02X RTClass2_PortStatus:%02X SyncId0_TopoOk:%02X TxFragmentation:%02X",
                                                          pLinkStatus_Ind->IRTPortStatus, pLinkStatus_Ind->RTClass2_PortStatus, pLinkStatus_Ind->SyncId0_TopoOk, pLinkStatus_Ind->TxFragmentation);
                                }
                            }

                            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_OK);
                            bTraceOutput = LSA_FALSE;
                        }
                    }
                }
                pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)(void *)EDDI_QueueGetNext(&pDDB->HDBQueue, &pHDB->QueueLink);  //get next element
            }
            else
            {
                break;
            }
        }
        while (IndexHandle);

        if (0 == IndexHandle)
        {
            EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenLinkInterrupt, HDB QUEUE CORRUPT");
            EDDI_Excp("EDDI_GenLinkInterrupt, HDB QUEUE CORRUPT", EDDI_FATAL_ERR_EXCP, pDDB->DeviceNr, 0);
            return;
        }
    } //end for

    #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
    pDDB->bFilterLinkIndTrace = LSA_FALSE;
    #endif

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLinkInterrupt<-");

    #if (EDDI_CFG_TRACE_MODE != 0)
    LSA_UNUSED_ARG(OldLinkVals.Autoneg); //satisfy lint!
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenDelAllDynMCMACTab()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenDelAllDynMCMACTab( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT  Result;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_GenDelAllDynMCMACTab->");

    Result = EDDI_GenDelAllfCntMC(&pDDB->Glob.MCMACTabelleLocal, pDDB);
    if (EDD_STS_OK != Result)
    {
        return Result;
    }

    pDDB->Glob.MCMACTabelleLocal.pBottom = EDDI_NULL_PTR;
    pDDB->Glob.MCMACTabelleLocal.pTop    = EDDI_NULL_PTR;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenDelAllDynMCMACTab<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
 * F u n c t i o n:       EDDI_GenSetupPhy()
 *
 *                        Allows manual setting of PHY-SpeedMode and Activity.
 *
 *                        ATTENTION: This functions can NOT be used if a PRM-Record
 *                                   already has configured the PHY settings!
 *
 * D e s c r i p t i o n:
 *
 * A r g u m e n t s:
 *
 * Return Value:          LSA_RESULT
 *
 ***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetupPhy( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_BOOL                     *  bIndicate  )
{
    LSA_RESULT                            Result = EDD_STS_OK;
    LSA_UINT32                            UsrPortIndex;
    LSA_UINT32                            HwPortIndex;
    LSA_UINT8                             LinkSpeedMode;
    EDDI_UPPER_SETUP_PHY_PTR_TYPE         pParamSetupPhy;
    EDDI_RQB_SWI_SET_PORT_SPEEDMODE_TYPE  UsrSpeedMode;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetupPhy->");
    
    *bIndicate = LSA_TRUE;
    
    pParamSetupPhy = (EDDI_UPPER_SETUP_PHY_PTR_TYPE)pRQB->pParam;
    
    if (   (EDD_FEATURE_ENABLE != pParamSetupPhy->PHYReset)
        && (EDD_FEATURE_DISABLE != pParamSetupPhy->PHYReset))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, invalid value for PHYReset (%d)", 
            pParamSetupPhy->PHYReset);
        return EDD_STS_ERR_PARAM;  
    }
    else if ((pDDB->PRM.bCommitExecuted) && (EDD_FEATURE_ENABLE != pParamSetupPhy->PHYReset))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, bCommitExecuted:0x%X",
                              pDDB->PRM.bCommitExecuted);
        return EDD_STS_ERR_SEQUENCE;
    }

    EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenSetupPhy, PortID:0x%X LinkSpeedMode:0x%X PHYPower:0x%X",
                          pParamSetupPhy->PortID, pParamSetupPhy->LinkSpeedMode, pParamSetupPhy->PHYPower);

    Result = EDDI_GenChangeUsrPortID2HwPortIndex(pParamSetupPhy->PortID, pDDB, &HwPortIndex);
    if (EDD_STS_OK != Result)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        return Result;
    }

    if (   (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == pDDB->Glob.PortParams[HwPortIndex].MediaType)
        && (EDD_LINK_100MBIT_FULL != pParamSetupPhy->LinkSpeedMode))
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
        EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, For EDD_MEDIATYPE_FIBER_OPTIC_CABLE is only EDD_LINK_100MBIT_FULL allowed, LinkSpeedMode:0x%X", 
                          pParamSetupPhy->LinkSpeedMode);
        return EDD_STS_ERR_PARAM;  
    }
  
    //10HDX Phybug Reset
    #if defined (EDDI_CFG_REV6)
    if (EDD_FEATURE_ENABLE == pParamSetupPhy->PHYReset)
    {       
        if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed == EDD_LINK_UNKNOWN)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, Unknown LinkSpeed:0x%X", pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkSpeed);

            return EDD_STS_ERR_SEQUENCE;
        }
        
        if (pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode == EDD_LINK_UNKNOWN)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, Unknown LinkMode:0x%X", pDDB->pLocal_SWITCH->LinkPx[HwPortIndex].LinkMode);
            return EDD_STS_ERR_SEQUENCE;
        }
           
        Result = EDDI_Gen10HDXPhyBugReset(pRQB, pDDB, HwPortIndex, bIndicate);
        if (EDD_STS_OK != Result)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
            EDDI_PRM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, For EDD_MEDIATYPE_FIBER_OPTIC_CABLE is only EDD_LINK_100MBIT_FULL allowed, LinkSpeedMode:0x%X", 
                              pParamSetupPhy->LinkSpeedMode);
            return Result;
        }
            
        return Result;                
    }
    #else
    if (EDD_FEATURE_ENABLE == pParamSetupPhy->PHYReset)
    {
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_SetupPhy);
        EDDI_PRM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GenSetupPhy, Phy Reset is only allowed for Rev6 (E200).");
        return EDD_STS_ERR_PARAM; 
    }
    #endif

    if (pParamSetupPhy->LinkSpeedMode != EDD_LINK_UNCHANGED)
    {
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            UsrSpeedMode.PortIDLinkSpeedMode[UsrPortIndex] = EDD_LINK_UNCHANGED;
        }

        //Patch only for fiber transceiver, change Autoneg to 100MBit/Full
        if (EDD_LINK_AUTONEG == pParamSetupPhy->LinkSpeedMode)
        {
            EDDI_AutoNegCapToLinkSpeedMode(HwPortIndex, &LinkSpeedMode, pDDB);

            UsrSpeedMode.PortIDLinkSpeedMode[pParamSetupPhy->PortID - 1] = LinkSpeedMode;
        }
        else
        {
            UsrSpeedMode.PortIDLinkSpeedMode[pParamSetupPhy->PortID - 1] = pParamSetupPhy->LinkSpeedMode;
        }

        //user view - no port mapping
        Result = EDDI_SwiPortSetSpMoConfig(&UsrSpeedMode, pDDB);
        if (EDD_STS_OK != Result)
        {
            return Result;
        }
    }

    if (pParamSetupPhy->PHYPower != EDDI_PHY_POWER_UNCHANGED)
    {
        //if the phy-power is switched off and the link changes from up to down, only 1 indication shall occur
        if (EDDI_PHY_POWER_OFF == pParamSetupPhy->PHYPower)
        {
            //only record change, do not generate indication
            EDDI_SwiPortSetPhyStatus(HwPortIndex, pParamSetupPhy->PHYPower, LSA_FALSE, pDDB);
        }
        Result = EDDI_TRASetPowerDown(pDDB, HwPortIndex, pParamSetupPhy->PHYPower, LSA_FALSE /*bRaw*/);
        if (EDD_STS_OK != Result)
        {
            return Result;
        }
        EDDI_SwiPortSetPhyStatus(HwPortIndex, pParamSetupPhy->PHYPower, LSA_TRUE, pDDB);
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenLEDBlink()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenLEDBlink( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                           EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_RESULT                      Status;
    LSA_UINT32                      UsrPortIndex;
    LSA_UINT16               const  OnOffDurationIn500msTicks   = (LSA_UINT16)1;
    LSA_UINT16               const  TotalBlinkDurationInSeconds = (LSA_UINT16)3;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;

    LSA_UNUSED_ARG(pRQB);

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLEDBlink->");

    EDDI_PROGRAM_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                          "EDDI_GenLEDBlink, TotalBlinkDurationInSeconds:0x%X OnOffDurationIn500msTicks:0x%X LED_IsRunning:0x%X LED_OnOffDuration_Status:0x%X LED_TotalBlink_Cnt_100ms:0x%X LED_OnOff_Cnt_100ms:0x%X LED_TotalBlink_Duration_100ms:0x%X LED_OnOff_Duration_100ms:0x%X",
                          TotalBlinkDurationInSeconds, OnOffDurationIn500msTicks,
                          pDDB->Glob.LED_IsRunning, pDDB->Glob.LED_OnOffDuration_Status,
                          pDDB->Glob.LED_TotalBlink_Cnt_100ms, pDDB->Glob.LED_OnOff_Cnt_100ms,
                          pDDB->Glob.LED_TotalBlink_Duration_100ms, pDDB->Glob.LED_OnOff_Duration_100ms);

    //LED is already flashing
    if (pDDB->Glob.LED_IsRunning)
    {
        //if timer is already running, its prolongation is only allowed when LED_OnOffDuration_In500msTicks is the same
        if (OnOffDurationIn500msTicks != ((LSA_UINT16)(pDDB->Glob.LED_OnOff_Duration_100ms / (LSA_UINT16)5)))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_ONOFF_DURATION_DIFFER);
            return EDD_STS_ERR_PARAM;
        }

        pDDB->Glob.LED_TotalBlink_Cnt_100ms = 0;
    }
    else
    {
        //Invoke init things before starting to blink and turn all LEDs on
        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRALedBlinkBegin(pDDB, HwPortIndex);
            pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRALedBlinkSetMode(pDDB, HwPortIndex, EDDI_LED_MODE_ON);
        }

        //start 1ms timer
        Status = EDDI_StartTimer(pDDB, pDDB->Glob.LED_OnOffDuration_TimerID, (LSA_UINT16)1);
        if (Status != EDD_STS_OK)
        {
            EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenLEDBlink, EDDI_StartTimer Status 0x%X LED_OnOffDuration_TimerID:0x%X", 
              Status, pDDB->Glob.LED_OnOffDuration_TimerID);
            EDDI_Excp("EDDI_GenLEDBlink, EDDI_StartTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->Glob.LED_OnOffDuration_TimerID);
            return Status;
        }

        pDDB->Glob.LED_OnOffDuration_Status = EDDI_LED_MODE_ON;

        pDDB->Glob.LED_TotalBlink_Cnt_100ms = 0;

        pDDB->Glob.LED_OnOff_Cnt_100ms      = 0;

        pDDB->Glob.LED_IsRunning            = LSA_TRUE;

        pDDB->Glob.LED_OnOff_Duration_100ms = (LSA_UINT16)(OnOffDurationIn500msTicks * (LSA_UINT16)5);
    }

    pDDB->Glob.LED_TotalBlink_Duration_100ms = (LSA_UINT16)(TotalBlinkDurationInSeconds * (LSA_UINT16)10);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenGetMaxReservedIntEnd()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_GenGetMaxReservedIntEnd( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  SafetyMargin;
    LSA_UINT32  MaxIntervalEndNs;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDIGenGetMaxReservedEnd->");

    SafetyMargin = IO_x32(NRT_SAFETY_MARGIN);

    MaxIntervalEndNs = 10 * ((pDDB->CycCount.CycleLength_10ns) -
                             ((EDDI_FRAME_BUFFER_LENGTH * 8) +
                              EDDI_GetBitField32(SafetyMargin, EDDI_SER_NRT_SAFETY_MARGIN_BIT__Bd100)));

    return MaxIntervalEndNs;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenRequest()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenRequest( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    LSA_RESULT                      Status;
    LSA_BOOL                        bIndicate;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;

    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GenRequest->ServiceID:0x%X", EDD_RQB_GET_SERVICE(pRQB));

    if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE)
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
            case EDD_SRV_MULTICAST:
                EDDI_SERSheduledRequest(pHDB->pDDB, pRQB, (LSA_UINT32)EDDI_GenRequest);
                return;
            default:
                break;
        }
    }

    bIndicate = LSA_TRUE;
    Status    = EDD_STS_OK;

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_LINK_STATUS_IND_PROVIDE:
        case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
            Status = EDDI_GenLinkIndProvide(pRQB, pHDB, &bIndicate);
            break;
        case EDD_SRV_GET_LINK_STATUS:
            Status = EDDI_GenGetLinkStatus(pRQB, pDDB);
            break;
        case EDD_SRV_MULTICAST:
            Status = EDDI_GenMC((EDD_UPPER_MULTICAST_PTR_TYPE)pRQB->pParam, pDDB);
            break;
        case EDD_SRV_GET_PARAMS:
            Status = EDDI_GenGetParam(pRQB, pDDB);
            break;
        case EDD_SRV_MULTICAST_FWD_CTRL:
            Status = EDDI_GenMCFWDCtrl(pRQB, pDDB);
            break;
        case EDD_SRV_SET_LINE_DELAY:
            Status = EDDI_GenSetLineDelay(pRQB, pDDB);
            break;
        case EDD_SRV_GET_PORT_PARAMS:
            Status = EDDI_GenGetPortParams(pRQB, pDDB);
            break;
        case EDD_SRV_SET_IP:
            EDDI_GenSetIP(pRQB, pDDB);
            break;
        case EDDI_SRV_SETUP_PHY:
            Status = EDDI_GenSetupPhy(pRQB, pDDB, &bIndicate);
            break;
        case EDD_SRV_LED_BLINK:
            Status = EDDI_GenLEDBlink(pRQB, pHDB);
            break;
        case EDD_SRV_SENDCLOCK_CHANGE:
            Status = EDDI_CycSendclockChange(pRQB, pDDB, &bIndicate);
            break;
        case EDD_SRV_SET_SYNC_TIME:
            Status = EDDI_CycSetSyncTime(pRQB, pHDB, &bIndicate);
            break;
        case EDD_SRV_SET_SYNC_STATE:
            Status = EDDI_GenSetSyncState(pRQB, pDDB);
            break;
        case EDD_SRV_SET_REMOTE_PORT_STATE:
            Status = EDDI_GenSetRemotePortState(pRQB, pDDB);
            break;
        case EDD_SRV_GET_STATISTICS:
            Status = EDDI_GenGetStatistics(pRQB, pHDB);
            break;
        case EDD_SRV_GET_STATISTICS_ALL:
            Status = EDDI_GenGetStatisticsAll(pRQB, pHDB);
            break;    
        case EDD_SRV_RESET_STATISTICS:
            Status = EDDI_GenResetStatistics(pRQB, pHDB, LSA_TRUE);
            break;
        default:
            Status = EDD_STS_ERR_SERVICE;
            break;
    }

    if (bIndicate)
    {
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_GenLEDOnOffDuration()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenLEDOnOffDuration( LSA_VOID  *  const  context )
{
    LSA_RESULT                      Status;
    LSA_UINT32                      UsrPortIndex;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLEDOnOffDuration->");

    if (EDDI_NULL_PTR == context)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenLEDOnOffDuration, EDDI_NULL_PTR == context");
        EDDI_Excp("EDDI_GenLEDOnOffDuration, EDDI_NULL_PTR == context", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (!(pDDB->Glob.LED_IsRunning))
    {
        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenLEDOnOffDuration, LSA_FALSE == pDDB->Glob.LED_IsRunning, LED_OnOffDuration_TimerID:0x%X", pDDB->Glob.LED_OnOffDuration_TimerID);
        EDDI_Excp("EDDI_GenLEDOnOffDuration, LSA_FALSE == pDDB->Glob.LED_IsRunning", EDDI_FATAL_ERR_EXCP, pDDB->Glob.LED_OnOffDuration_TimerID, 0);
        return;
    }

    pDDB->Glob.LED_TotalBlink_Cnt_100ms++;

    pDDB->Glob.LED_OnOff_Cnt_100ms++;

    //total flashing time expired
    if (pDDB->Glob.LED_TotalBlink_Cnt_100ms >= pDDB->Glob.LED_TotalBlink_Duration_100ms)
    {
        //flashing time expired
        Status = EDDI_StopTimer(pDDB, pDDB->Glob.LED_OnOffDuration_TimerID);
        switch (Status)
        {
            case EDD_STS_OK:
                break;
            default:
            {
                EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_GenLEDOnOffDuration, EDDI_StopTimer Status:0x%X, LED_OnOffDuration_TimerID:0x%X", 
                    Status, pDDB->Glob.LED_OnOffDuration_TimerID);
                EDDI_Excp("EDDI_GenLEDOnOffDuration, EDDI_StopTimer", EDDI_FATAL_ERR_EXCP, Status, pDDB->Glob.LED_OnOffDuration_TimerID);
                return;
            }
        }

        //invoke restore procedure after blink duration has finished
        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRALedBlinkSetMode(pDDB, HwPortIndex, EDDI_LED_MODE_OFF);
            pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRALedBlinkEnd(pDDB, HwPortIndex);
        }

        pDDB->Glob.LED_IsRunning                 = LSA_FALSE;
        pDDB->Glob.LED_OnOffDuration_Status      = EDDI_LED_MODE_OFF;
        pDDB->Glob.LED_TotalBlink_Cnt_100ms      = 0;
        pDDB->Glob.LED_OnOff_Cnt_100ms           = 0;
        pDDB->Glob.LED_TotalBlink_Duration_100ms = 0;
        pDDB->Glob.LED_OnOff_Duration_100ms      = 0;

        return;
    }

    //toggle LED after expiration of "LED_OnOff_Duration_100ms" -> LED On/Off
    if ((pDDB->Glob.LED_OnOff_Cnt_100ms % pDDB->Glob.LED_OnOff_Duration_100ms) == 0)
    {
        pDDB->Glob.LED_OnOffDuration_Status = (LSA_BOOL)((pDDB->Glob.LED_OnOffDuration_Status == EDDI_LED_MODE_ON) ? EDDI_LED_MODE_OFF : EDDI_LED_MODE_ON);

        //toggle all LEDs
        for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            pDDB->pLocal_SWITCH->TraFctPtrs[HwPortIndex].pTRALedBlinkSetMode(pDDB, HwPortIndex, pDDB->Glob.LED_OnOffDuration_Status);
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenLEDOnOffDuration<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenSetIRTPortStatus()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetIRTPortStatus( LSA_BOOL                 const  bAllPort,
                                                         LSA_UINT32               const  argHwPortIndex,
                                                         LSA_UINT8                const  IRTPortStatus,
                                                         EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortIndex;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetIRTPortStatus->");

    if (!bAllPort)
    {
        pDDB->pLocal_SWITCH->LinkIndExtPara[argHwPortIndex].IRTPortStatus = IRTPortStatus;
    }
    else
    {
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].IRTPortStatus = IRTPortStatus;
        }
    }

    //LinkIndExt "EDDI_GenLinkInterrupt(&PortChanged[0], pDDB)" is triggered later!!

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetIRTPortStatus<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenSetSyncState()                           */
/*                                                                         */
/* D e s c r i p t i o n: Set actual SyncState                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetSyncState( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                Result;
    EDD_UPPER_SET_SYNC_STATE_PTR_TYPE  const  pParam = (EDD_UPPER_SET_SYNC_STATE_PTR_TYPE)pRQB->pParam;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetSyncState->");

    Result = EDD_STS_OK;

    if (LSA_HOST_PTR_ARE_EQUAL(pParam, LSA_NULL))
    {
        //invalid parameters
        Result = EDD_STS_ERR_PARAM;
    }
    else
    {
        if (   (pParam->SyncState != EDD_SYNC_STATE_GOOD)
            && (pParam->SyncState != EDD_SYNC_STATE_BAD))
        {
            Result = EDD_STS_ERR_PARAM;
        }
        else
        {
            LSA_BOOL  const  bIsSyncOk = (LSA_BOOL)((pParam->SyncState == EDD_SYNC_STATE_GOOD) ? LSA_TRUE : LSA_FALSE);

            EDDI_SyncPortStmsSyncChange(pDDB, bIsSyncOk);
            Result = EDD_STS_OK;
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetSyncState<-");

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_GenSetRemotePortState()                     */
/*                                                                         */
/* D e s c r i p t i o n: Set remote port state.                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GenSetRemotePortState( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                                       Result;
    EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE  const  pParam = (EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE)pRQB->pParam;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetRemotePortState->");

    Result = EDD_STS_OK;

    if (LSA_HOST_PTR_ARE_EQUAL(pParam, LSA_NULL))
    {
        //invalid parameters
        Result = EDD_STS_ERR_PARAM;
    }
    else
    {
        LSA_UINT32  HwPortIndex;

        Result = EDDI_GenChangeUsrPortID2HwPortIndex(pParam->PortID, pDDB, &HwPortIndex);

        if (EDD_STS_OK != Result)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
        }
        else if ( (pParam->SyncId0_TopoState     != EDD_SET_REMOTE_PORT_STATE_IGNORE        ) &&
                  (pParam->SyncId0_TopoState     != EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH ) &&
                  (pParam->SyncId0_TopoState     != EDD_SET_REMOTE_PORT_STATE_TOPO_OK       ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TopoState);
            Result = EDD_STS_ERR_PARAM;
        }
        else if ( (pParam->RtClass2_TopoState    != EDD_SET_REMOTE_PORT_STATE_IGNORE         ) &&
                  (pParam->RtClass2_TopoState    != EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  ) &&
                  (pParam->RtClass2_TopoState    != EDD_SET_REMOTE_PORT_STATE_TOPO_OK        ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TopoState);
            Result = EDD_STS_ERR_PARAM;
        }
        else if ( (pParam->RtClass3_TopoState    != EDD_SET_REMOTE_PORT_STATE_IGNORE         ) &&
                  (pParam->RtClass3_TopoState    != EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  ) &&
                  (pParam->RtClass3_TopoState    != EDD_SET_REMOTE_PORT_STATE_TOPO_OK        ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TopoState);
            Result = EDD_STS_ERR_PARAM;
        }
        else if ( (pParam->RtClass3_PDEVTopoState != EDD_SET_REMOTE_PORT_STATE_IGNORE         ) &&
                  (pParam->RtClass3_PDEVTopoState != EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  ) &&
                  (pParam->RtClass3_PDEVTopoState != EDD_SET_REMOTE_PORT_STATE_TOPO_OK        ) )
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_TopoState);
            Result = EDD_STS_ERR_PARAM;
        }
        else if ((pParam->RtClass3_RemoteState  != EDD_SET_REMOTE_PORT_STATE_IGNORE        ) &&
                 (pParam->RtClass3_RemoteState  != EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF ) &&
                 (pParam->RtClass3_RemoteState  != EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX  ) &&
                 (pParam->RtClass3_RemoteState  != EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX))
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_RemoteState);
            Result = EDD_STS_ERR_PARAM;
        }
        else
        {
            EDDI_SyncPortStmsRemoteChange(pDDB,
                                          HwPortIndex,
                                          pParam->SyncId0_TopoState,
                                          pParam->RtClass2_TopoState,
                                          pParam->RtClass3_TopoState,
                                          pParam->RtClass3_PDEVTopoState,
                                          pParam->RtClass3_RemoteState);
            Result = EDD_STS_OK;
        }
    }

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GenSetRemotePortState<-");

    return Result;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_gen.c                                                   */
/*****************************************************************************/

