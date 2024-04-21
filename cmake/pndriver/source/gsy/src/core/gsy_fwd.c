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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_fwd.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Forwarding module                                                        */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  29.03.05    JS    initial version.                                       */
/*  2005-06-02  P03.06.00.00_00.01.00.00  lrg  Prototype for PT2             */
/*  2005-06-06  P03.06.00.00_00.01.00.01  lrg  gsy_FwdUserForwardCtrl()      */
/*                                             Mode in Trace                 */
/*  21.06.05    JS    Master entrys remain within management till timed out. */
/*                    So we can test for sequence id.                        */
/*  01.07.05    JS    cast removed                                           */
/*  16.09.05    JS    added brackets to avoid compiler error with TRACE      */
/*  10.10.05    JS    added picosecond support.                              */
/*  24.10.05    JS    gsy_FwdLinkStatus added.                               */
/*                    SendEnabled added (send only if link not DOWN/DISABLED */
/*                    or HALF-DUPLEX.                                        */
/*  2005-11-21  P03.08.00.00_00.01.02.00  lrg  {{...}} init MAC Addresses    */
/*  16.12.05    JS    seperated queueing macros to avoid GCC compiler error. */
/*  23.01.06    JS    added some type casts to prevent GCC compiler warnings */
/*  2006-06-28  P03.12.00.00_00.02.01.02 lrg GSY_FILE_SYSTEM_EXTENSION()     */
/*  06.07.06    JS    forwarding only if linedelay set for destination port  */
/*  2006-08-31  P03.12.00.00_00.25.03.02  lrg  GSY_ERROR_TRACE_              */
/*  2006-09-06  P03.12.00.00_00.25.04.02  lrg  Announce-Handling: TODO       */
/*  22.09.06    JS    added announce forwarding                              */
/*                    timer-free handling on channel close changed to avoid  */
/*                    a conflict with incoming timer events while closing.   */
/*  2006-10-04  P03.12.00.00_00.30.01.02 041006lrg003: GSY_ERROR_TRACE_02    */
/*              -> GSY_ERROR_TRACE_04  (ArtsRQ 371299)                       */
/*  2006-10-18  P03.12.00.00_00.30.02.02 181006lrg001: Port-MAC-Adresse      */
/*  01.12.06    JS    remove old FCBs if waiting for FU with Announce arrived*/
/*  27.03.07    JS    bugfix. uninitialized pointer (pFCB) within            */
/*                    gsy_FwdCancelFUWaitingFCB                              */
/*  2007-06-28 P04.00.00.00_00.30.01.00 280607lrg001: RTSync: generate FU,   */
/*             enable/disable MACaddress for sync frame without FollowUp...  */
/*  2007-08-29  P04.00.00.00_00.30.04.02 290807lrg001: FwdModeIn, FwdModeOut */
/*              GSY_FWD_ENABLE => GSY_FWD_MODE_ENABLE                        */
/*              GSY_FWD_MODE_ENABLE=GSY_FWD_MODE_OUTPUT & GSY_FWD_MODE_INPUT */
/*  2007-09-07  P04.00.00.00_00.30.05.02 070907lrg002: ~GSY_FWD_MODE_OUTPUT  */
/*  2007-09-24  P04.00.00.00_00.30.06.02 240907lrg001: Enable/ Disable       */
/*              Announce-Multicast-Address                                   */
/*  2007-10-04  P04.00.00.00_00.40.01.02 041007lrg003: Announce SeqId        */
/*  2007-11-23  P04.00.00.00_00.60.01.02 lrg: #if 0 entfernt etc.            */
/*  2007-12-20  P04.00.00.00_00.60.02.02 201207lrg001: Send local port MAC   */
/*              with SyncFU and FU frame if FollowUp generated.              */
/*  2007-12-20  P04.00.00.00_00.60.02.02 080108lrg001: 201207lrg001 only     */
/*              when GSY_FWD_PORT_SRC_MAC_ADDR is defined and                */
/*              100108lrg001: Only if SRC-MAC is equal to MasterInterfaceMAC */
/*  2008-05-26  P04.00.00.00_00.80.02.02 260508lrg001: ArtsRQ AP00644447     */
/*              ignore received frames if forwarding channel is not          */
/*              completely initialized                                       */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-07-24  P04.01.00.00_00.01.02.03 240708lrg001: ArtsRQ AP00676859     */
/*              Check ClockTopoOk at output port                .            */
/*  2008-10-13  P04.01.00.00_00.01.03.02 131008lrg001: Casts for C++         */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-10  P04.01.00.00_00.02.03.02 101208lrg001: Announce mit          */
/*              gsy_DataSend() versenden                                     */
/*  2009-01-15  P04.01.00.00_00.02.03.03 150109lrg001: PortId and PduLen     */
/*              exchanged at sending of announce frame                       */
/*  2009-03-13  P04.00.01.00_00.03.02.02 020309lrg001: ArtsRQ AP00791468     */
/*              Do not call fatal error if send response is not OK           */
/*  2009-04-08  P04.01.00.00_00.03.03.02 lrg: EDD_HW_TYPE_USED_ERTEC_200P    */
/*  2009-04-23  P04.01.00.00_00.03.04.02 lrg: GSY_HW_TYPE_ERTEC_200P         */
/*  2009-08-31  P04.02.00.00_00.01.04.02 310809lrg001: GSY_FWD_MODE_INPUT    */
/*              to forward announce frames only to the user                  */
/*  2009-11-04  P04.02.00.00_00.02.01.02 041109lrg001: use actual port count */
/*  2010-01-15  P05.00.00.00_00.01.03.02 110110lrg001: ArtsRQ AP00848562     */
/*              Handle SyncFrame even if !DoFwd to notify SeqId. Trace.      */
/*  2010-03-03  P05.00.00.00_00.01.03.03 lrg: Announce channel               */
/*  2010-04-01  P05.00.00.00_00.01.04.02 010410lrg001: Arts-RQ AP00952155    */
/*              use configured port count for trace                          */
/*  2010-09-15  P05.00.00.00_00.01.08.02 150910lrg002: Arts-RQ AP00992003    */
/*              forward announce if not done by HW                           */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-07-25  P05.00.00.00_00.05.38.02 lrg: GSY_PROC_ removed from Trace   */
/*  2011-09-27  P05.00.00.00_00.05.56.02 AP01191740 Trace parameter count    */
/*  2012-05-14  P05.00.00.00_00.05.95.02 AP01357571 FwdTimerEvent            */
/*  2012-06-11  P05.00.00.00_00.05.97.02 AP01368288 Compiler Warnings        */
/*  2013-05-23  P05.02.00.00_00.04.24.02 AP01539291 Linkerfehler bei         */
/*              Verwendung von C++ compiler (Fkt. ptr als Parameter)         */
/*  2014-05-20  P06.00.00.00_00. TODO gl 966426: [GSY] Die Synchronisation  */
/*              über EDDL und i210 soll ermöglicht werden                    */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] Announce-MAC-Adressen */
/*              nur bei Masterprojektierung aktivieren                       */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID  5
#define GSY_MODULE_ID      LTRC_ACT_MODUL_ID /* GSY_MODULE_ID_FWD */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"
GSY_FILE_SYSTEM_EXTENSION(GSY_MODULE_ID)

/*===========================================================================*/
/*                              local defines                                */
/*===========================================================================*/
#define GSY_FWD_PORT_SRC_MAC_ADDR   //080108lrg001: activate Patch

/*===========================================================================*/
/*                              local macros                                 */
/*===========================================================================*/

#define GSY_MEM_FREE_FAIL_CHECK(RetVal) \
{                                                         \
if( RetVal != LSA_RET_OK )                            \
{                                                     \
gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FREE_MEM, LSA_NULL, 0); \
}                                                     \
}

#define GSY_ASSERT_NULL_PTR(pPtr) \
{                                                         \
if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
{                                                     \
gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0); \
}                                                     \
}

#define GSY_ASSERT_NULL_FCT_PTR(pFct) \
{                                                         \
if( (0 == pFct) )          \
{                                                     \
gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0); \
}                                                     \
}

/* pValue has to be a UINT8 - Ptr! */
#define GSY_SET_U32(pValue,Value) \
{                                       \
*(pValue)   = (LSA_UINT8) ((Value) >> 24 );  \
*(pValue+1) = (LSA_UINT8) ((Value) >> 16 );  \
*(pValue+2) = (LSA_UINT8) ((Value) >> 8  );  \
*(pValue+3) = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
/*
#define GSY_SET_U16(pValue,Value) \
{                                       \
*(pValue)   = (LSA_UINT8) ((Value) >> 8  );  \
*(pValue+1) = (LSA_UINT8) ((Value));         \
}*/

/*===========================================================================*/
/*                              local constants                              */
/*===========================================================================*/

/* 110707lrg001: gsy_FwdSubDelay() not longer needed because the new DelayNS field is int
*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdLinkStatus                                   +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+                     LSA_UINT8               Status                      +*/
/*+                     LSA_UINT8               Mode                        +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID (1..x)                                 +*/
/*+  Status                 : Status of Link.                               +*/
/*+                           EDD_LINK_UP                                   +*/
/*+                           EDD_LINK_DOWN                                 +*/
/*+                           EDD_LINK_UP_CLOSED                            +*/
/*+                           EDD_LINK_UP_DISABLED                          +*/
/*+                                                                         +*/
/*+  Mode                   : Status of Link.                               +*/
/*+                           EDD_LINK_MODE_HALF                            +*/
/*+                           EDD_LINK_MODE_FULL                            +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set LinkStatus for forwarding.                            +*/
/*+               Forwarding will only send a frame if link is not DOWN,    +*/
/*+               DISABLED or HALF-DUPLEX.                                  +*/
/*+               Note: Forwarding channel has to be init!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdLinkStatus(
    GSY_CH_SYS_PTR          pChSys,
    LSA_UINT16              PortID,
    LSA_UINT8               Status,
    LSA_UINT8               Mode,
    LSA_BOOL                TopoOk)
{
    LSA_UINT16             Response;

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdLinkStatus() pChSys=%X Port=%u Status=%u",
                          pChSys, PortID, Status);

    Response = GSY_RSP_OK;

    GSY_ASSERT_NULL_PTR(pChSys->Fwd.pPortMgm); /* check if init */

    if ((PortID == 0 ) || (PortID > pChSys->Fwd.PortCnt ))
    {
        GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"gsy_FwdLinkStatus() *** Ignoring invalid Port=%u", PortID);
        Response = GSY_RSP_ERR_PARAM;
    }
    else
    {
        if (( Status == EDD_LINK_DOWN )        ||
            ( Status == EDD_LINK_UP_DISABLED ) ||
            ( Mode   == EDD_LINK_MODE_HALF   ))
        {
            pChSys->Fwd.pPortMgm[PortID-1].SendEnabled = LSA_FALSE;
        }
        else
        {
            pChSys->Fwd.pPortMgm[PortID-1].SendEnabled = LSA_TRUE;
        }
        /* 240708lrg001: Get TopoOk
        */
        pChSys->Fwd.pPortMgm[PortID-1].ClockTopoOk = TopoOk;
    }
    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdLinkStatus() Response=0x%x TopoOk=%u",
                          Response, TopoOk);

    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdMcControl                                    +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     GSY_LOWER_RQB_PTR_TYPE  pRbl                        +*/
/*+                     LSA_BOOL                Enable                      +*/
/*+                     LSA_UINT16             *pRespone                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  pRbl                   : Pointer to EDD MC-RQB. LSA_NULL if none       +*/
/*+  Enable                 : LSA_TRUE: Enable MC                           +*/
/*+                           LSA_FALSE DisableMC                           +*/
/*+  pResponse              : Response                                      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result        :    LSA_FALSE: Ready, no MC-Request send to EDD         +*/
/*+                     LSA_TRUE : Tried to send MC-Request, check Response +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sends MC enable/disable request to EDD                    +*/
/*+                                                                         +*/
/*+               Note: uses  pChSys->Fwd.MCSetIdx. Preset!                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL gsy_FwdMcControl(
    GSY_CH_SYS_PTR                    pChSys,
    GSY_LOWER_RQB_PTR_TYPE            pRbl,
    LSA_BOOL                          Enable,
    LSA_UINT16                      * pResponse)
{
    LSA_BOOL Ready;
	LSA_BOOL ChAnno = LSA_FALSE;

    GSY_MAC_ADR_TYPE    *pMac;
    GSY_MAC_ADR_TYPE    AddrSync = {GSY_MULTICAST_ADDR_SYNC};
//  GSY_MAC_ADR_TYPE    AddrAnno = {GSY_MULTICAST_ADDR_ANNOUNCE};	//1126470
    GSY_MAC_ADR_TYPE    AddrSyncFu = {GSY_MULTICAST_ADDR_FUSYNC};
    GSY_MAC_ADR_TYPE    AddrFu = {GSY_MULTICAST_ADDR_SYNC_FU};

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdMcControl() pChSys=%X, pRbl=%X, Enable=%u",
                          pChSys, pRbl, Enable);

    Ready      = LSA_TRUE;
    *pResponse = GSY_RSP_OK;

    /* search for next MC to enable/disable */

    while (( pChSys->Fwd.MCSetIdx < GSY_FWD_MAC_ADDR_CNT ) &&
           (
               ( LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[pChSys->Fwd.MCSetIdx].pMasterTable, LSA_NULL) ) ||
               (( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].SyncEnabled == Enable ) &&
//              ( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].AnnoEnabled == Enable ) &&
                ( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].SyncFuEnabled == Enable ) &&
                ( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].FuEnabled == Enable ))
           ))
    {
        pChSys->Fwd.MCSetIdx++;
    }

    if (pChSys->Fwd.MCSetIdx < GSY_FWD_MAC_ADDR_CNT)
    {
        /* EDD Multicast Enable/Disable..*/

#ifdef GSY_MESSAGE
        /* 280607lrg001: MAC address for sync frame without FollowUp
         * 240907lrg001: MAC address for announce frame
         * 310809lrg001: TODO: Only enable MAC address for announce frame or SyncId != 0/1 on sync hardware
        */
#endif /* GSY_MESSAGE */

/*      if ( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].AnnoEnabled != Enable )
        {
            GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Start setting Announce MC(Idx=%u)!",pChSys->Fwd.MCSetIdx);
            pChSys->Fwd.MCSetSync = GSY_FWD_ANNO_SET;
            pMac = &AddrAnno;
			ChAnno = LSA_TRUE;
        }
        else
*/		
        {
	        if ( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].SyncEnabled != Enable )
	        {
	            GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Start setting Sync MC(Idx=%u)!",pChSys->Fwd.MCSetIdx);
	            pChSys->Fwd.MCSetSync = GSY_FWD_SYNC_SET;
	            pMac = &AddrSync;
	        }
	        else if ( pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].SyncFuEnabled != Enable )
	        {
	            GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Start setting SyncFu MC(Idx=%u)!",pChSys->Fwd.MCSetIdx);
	            pChSys->Fwd.MCSetSync = GSY_FWD_SYNCFU_SET;
	            pMac = &AddrSyncFu;
	        }
	        else
	        {
	            GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"Start setting FU   MC(Idx=%u)!",pChSys->Fwd.MCSetIdx);
	            pChSys->Fwd.MCSetSync = GSY_FWD_FU_SET;
	            pMac = &AddrFu;
	        }
		}
        pMac->MacAdr[5] |= (LSA_UINT8)pChSys->Fwd.MCSetIdx;
        *pResponse = gsy_MucAddrSet(pChSys,pRbl, pMac->MacAdr, Enable, ChAnno);
    }
    else /* nothing to open (most likely a wrong config, but we continue)..*/
    {
        Ready = LSA_FALSE;
    }
    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdMcControl() Response=0x%x Ready=%u",
                          *pResponse, Ready);

    return(Ready);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdFCBGetFree                                   +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+  Result        :    GSY_FWD_FCB_PTR_TYPE                                +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get a free FCB                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static GSY_FWD_FCB_PTR_TYPE GSY_LOCAL_FCT_ATTR gsy_FwdFCBGetFree(
    GSY_CH_SYS_PTR          pChSys)
{
    GSY_FWD_FCB_PTR_TYPE  pFCB;

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdFCBGetFree() pChSys=%X", pChSys);

    GSY_REM_BLOCK_BOTTOM_USEQUEUE(pChSys->Fwd.FCBFreeQueue.pBottom,
                                  pChSys->Fwd.FCBFreeQueue.pTop,
                                  pFCB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pFCB,LSA_NULL) )
    {
        pFCB->Status  = GSY_FWD_FCB_STS_FREE;
        pFCB->Sending = LSA_FALSE;
        pFCB->pMaster = LSA_NULL;
        pFCB->pPort   = LSA_NULL;
        pChSys->Fwd.FCBInUseCnt++;

        GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Get free FCB.  Now in use count=%u", pChSys->Fwd.FCBInUseCnt);
    }

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdFCBGetFree() pFCB=%X", pFCB);

    return(pFCB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdFCBRequeue                                   +*/
/*+  Input/Output  :    GSY_FWD_FCB_PTR_TYPE    pFCB                        +*/
/*+                     LSA_UINT32              Status                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pFCB                   : Pointer to FCB to move to other queues        +*/
/*+  Status                 : New FCB Status                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Moves FCB to other queues depending on current Status and +*/
/*+               desired Status                                            +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+   Note: pMaster and pPort must be setup on entry!                       +*/
/*+   Note: "Sending" within pFCB must be FALSE!!                           +*/
/*+   Note: FCB must not be within "FCBSendQueue"!                          +*/
/*+                                                                         +*/
/*+   Typically:                                                            +*/
/*+                                                                         +*/
/*+   GSY_FWD_FCB_STS_FREE            ->  GSY_FWD_FCB_STS_SYNC_SEND         +*/
/*+   GSY_FWD_FCB_STS_SYNC_SEND       ->  GSY_FWD_FCB_STS_FU_WAITING        +*/
/*+   GSY_FWD_FCB_STS_SYNC_SEND       ->  GSY_FWD_FCB_STS_FU_SEND           +*/
/*+   GSY_FWD_FCB_STS_FU_WAITING      ->  GSY_FWD_FCB_STS_FU_SEND           +*/
/*+   GSY_FWD_FCB_STS_FU_SEND         ->  GSY_FWD_FCB_STS_FREE              +*/
/*+                                                                         +*/
/*+   GSY_FWD_FCB_STS_FREE            ->  GSY_FWD_FCB_STS_ANNOUNCE_SEND     +*/
/*+   GSY_FWD_FCB_STS_ANNOUNCE_SEND   ->  GSY_FWD_FCB_STS_FREE              +*/
/*+                                                                         +*/
/*+   maybe this, if we abort handling:                                     +*/
/*+                                                                         +*/
/*+   GSY_FWD_FCB_STS_SYNC_SEND       ->  GSY_FWD_FCB_STS_FREE              +*/
/*+   GSY_FWD_FCB_STS_FU_WAITING      ->  GSY_FWD_FCB_STS_FREE              +*/
/*+                                                                         +*/
/*+   Not expected:                                                         +*/
/*+   GSY_FWD_FCB_STS_FREE            ->  GSY_FWD_FCB_STS_FREE              +*/
/*+   GSY_FWD_FCB_STS_FREE            ->  GSY_FWD_FCB_STS_FU_WAITING        +*/
/*+   equal to equal                                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdFCBRequeue(
    GSY_FWD_FCB_PTR_TYPE    pFCB,
    LSA_UINT32              Status)
{
    GSY_FWD_FCB_PTR_TYPE  pFCB2;
    LSA_BOOL              PutToMasterQueue;

    if (LSA_HOST_PTR_ARE_EQUAL(pFCB,LSA_NULL))
    {
	    GSY_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdFCBRequeue() pFCB=%X", pFCB);
        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
    }

    GSY_FUNCTION_TRACE_02(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdFCBRequeue() pFCB=%X Status=%u",
                          pFCB, Status);

    PutToMasterQueue = LSA_TRUE;

    if (LSA_HOST_PTR_ARE_EQUAL(pFCB->pMaster,LSA_NULL) ||
        LSA_HOST_PTR_ARE_EQUAL(pFCB->pPort,LSA_NULL))
    {
	    GSY_ERROR_TRACE_03(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdFCBRequeue() pFCB=%X pMaster=%X pPort=%X",
                          pFCB, pFCB->pMaster, pFCB->pPort);
        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
    }


    GSY_FWD_TRACE_03(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Requeue FCB pMaster=%X FCB-Status=%u to %u",
                     pFCB->pMaster, pFCB->Status, Status);

    /* --------------------------------------------------------------------------*/
    /* remove from current queues                                                */
    /* --------------------------------------------------------------------------*/

    switch (pFCB->Status )
    {
        case GSY_FWD_FCB_STS_FREE:
            /* nothing to do. This means the FCB is not in a queue yet */
            break;

        case GSY_FWD_FCB_STS_SYNC_SEND:
        case GSY_FWD_FCB_STS_FU_WAITING:
        case GSY_FWD_FCB_STS_FU_SEND:
        case GSY_FWD_FCB_STS_ANNOUNCE_SEND:

            PutToMasterQueue = LSA_FALSE;

            /* we only remove from this queue if we need to */

            if ( Status == GSY_FWD_FCB_STS_FREE )
            {
                GSY_REM_BLOCK_USEQUEUE(pFCB->pMaster->FCBQueue.pBottom,
                                       pFCB->pMaster->FCBQueue.pTop,
                                       pFCB,
                                       pFCB2);       //lint !e774 !e845 Boolean within 'if' always evaluates to False, sanity check in the macro is not verbose, HM 20.05.2016

                GSY_IS_VALID_PTR(pFCB);
                if (! LSA_HOST_PTR_ARE_EQUAL(pFCB2,pFCB))
                {
					GSY_ERROR_TRACE_03(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdFCBRequeue() pFCB=%X pFCB2=%X pFCB->Status=%u",
									pFCB, pFCB2, pFCB->Status);
                    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                }

                /* if no more FCBs pending within Master FWD is finished */
                if ( --pFCB->pMaster->FCBCnt == 0 )
                {
                    GSY_FWD_TRACE_03(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Master fwd finished: pMaster=%X SyncId=%u MIdx=%u",
                                     pFCB->pMaster, pFCB->pMaster->ThisProt, pFCB->pMaster->ThisIdx);

                    pFCB->pMaster->Status = GSY_FWD_MASTER_STS_FWD_FINISHED;
                }
            }
            break;


        default:
            /* unexpected */
			GSY_ERROR_TRACE_02(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdFCBRequeue() pFCB=%X pFCB->Status=%u",
							pFCB, pFCB->Status);
            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
            break;
    }

    /* --------------------------------------------------------------------------*/
    /* put to new queues                                                         */
    /* --------------------------------------------------------------------------*/

    switch (Status )
    {
        case GSY_FWD_FCB_STS_FREE:

            GSY_PUT_BLOCK_TOP_USEQUEUE(pFCB->pChSys->Fwd.FCBFreeQueue.pBottom,
                                       pFCB->pChSys->Fwd.FCBFreeQueue.pTop,
                                       pFCB);

            pFCB->pChSys->Fwd.FCBInUseCnt--;

            if ( pFCB->Status != GSY_FWD_FCB_STS_FU_WAITING )
            {
                pFCB->pPort->pChSys->Fwd.FCBPortCnt--;
            }
            GSY_FWD_TRACE_01(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Free FCB. In use now=%u",pFCB->pChSys->Fwd.FCBInUseCnt);
            break;

        case GSY_FWD_FCB_STS_SYNC_SEND:
        case GSY_FWD_FCB_STS_FU_SEND:

            if (PutToMasterQueue)
            {
                GSY_PUT_BLOCK_TOP_USEQUEUE(pFCB->pMaster->FCBQueue.pBottom,
                                           pFCB->pMaster->FCBQueue.pTop,
                                           pFCB);

                pFCB->pMaster->FCBCnt++;
            }
            GSY_PUT_BLOCK_TOP_SENDQUEUE(pFCB->pPort->FCBSendQueue.pBottom,
                                        pFCB->pPort->FCBSendQueue.pTop,
                                        pFCB);

            if (( pFCB->Status != GSY_FWD_FCB_STS_SYNC_SEND ) &&
                ( pFCB->Status != GSY_FWD_FCB_STS_FU_SEND   ))
            {
                pFCB->pPort->pChSys->Fwd.FCBPortCnt++;
            }
            break;

        case GSY_FWD_FCB_STS_FU_WAITING:

            if (PutToMasterQueue)
            {
                GSY_PUT_BLOCK_TOP_USEQUEUE(pFCB->pMaster->FCBQueue.pBottom,
                                           pFCB->pMaster->FCBQueue.pTop,
                                           pFCB);

                pFCB->pMaster->FCBCnt++;

            }
            pFCB->pPort->pChSys->Fwd.FCBPortCnt--;
            break;

        case GSY_FWD_FCB_STS_ANNOUNCE_SEND:

            if (PutToMasterQueue)
            {
                GSY_PUT_BLOCK_TOP_USEQUEUE(pFCB->pMaster->FCBQueue.pBottom,
                                           pFCB->pMaster->FCBQueue.pTop,
                                           pFCB);

                pFCB->pMaster->FCBCnt++;

            }
            GSY_PUT_BLOCK_TOP_SENDQUEUE(pFCB->pPort->FCBSendQueue.pBottom,
                                        pFCB->pPort->FCBSendQueue.pTop,
                                        pFCB);

            pFCB->pPort->pChSys->Fwd.FCBPortCnt++;
            break;

        default:
            /* unexpected */
			GSY_ERROR_TRACE_03(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdFCBRequeue() pFCB=%X pFCB->Status=%u Status=%u",
							pFCB, pFCB->Status, Status);
            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
            break;
    }

    pFCB->Status = Status;

    GSY_FUNCTION_TRACE_00(pFCB->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdFCBRequeue()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdCancelMasterFUWaitingFCB                     +*/
/*+  Input/Output  :    GSY_FWD_MASTER_PTR_TYPE pMaster                     +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Cancels/frees all FCBs of Master waiting for FU           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID gsy_FwdCancelMasterFUWaitingFCB(
    GSY_CH_SYS_PTR          pChSys,
    GSY_FWD_MASTER_PTR_TYPE pMaster)
{
    GSY_FWD_FCB_PTR_TYPE    pFCB;
    GSY_FWD_FCB_PTR_TYPE    pFCB1;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdCancelMasterFUWaitingFCB() pChSys=%X pMaster=%X", pChSys, pMaster);

    pFCB = pMaster->FCBQueue.pBottom;

    /* free all FCBs of this Master waiting for FU to arrive.
    */
    while (! LSA_HOST_PTR_ARE_EQUAL(pFCB, LSA_NULL))
    {
        pFCB1 = pFCB;
        pFCB = pFCB->UseQueue.pNext;

        if (pFCB1->Status == GSY_FWD_FCB_STS_FU_WAITING)
        {
            GSY_FWD_TRACE_04(pFCB1->pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Cancel master FU waiting pFCB=%X pMaster=%X MIdx=%u Port=%u",
                             pFCB1, pMaster, pMaster->ThisIdx, pFCB1->pPort->ThisPortID);

            /* ReQueue FCB. No further handling
            */
            gsy_FwdFCBRequeue(pFCB1,GSY_FWD_FCB_STS_FREE);
        }
    }
    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdCancelMasterFUWaitingFCB()");
	LSA_UNUSED_ARG(pChSys);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdCancelFUWaitingFCB                           +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Cancels/frees all FCBs of the pChSys which wait for FU.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID gsy_FwdCancelFUWaitingFCB(
    GSY_CH_SYS_PTR                    pChSys)
{
    GSY_FWD_MASTER_PTR_TYPE pMaster;
    LSA_UINT32              i,j;
    GSY_FWD_FCB_PTR_TYPE    pFCB = LSA_NULL;    // 051206lrg002 Compiler warning
    GSY_FWD_FCB_PTR_TYPE    pFCB1;

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdCancelFUWaitingFCB() pChSys=%X", pChSys);

    /* search all protocols
    */
    for ( i=0; i<GSY_FWD_MAC_ADDR_CNT; i++)
    {
        /* used protocol
        */
        if (! LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[i].pMasterTable,LSA_NULL))
        {
            /* search all masters
            */
            for ( j=0; j< GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; j++)
            {
                pMaster = &gsy_data.Fwd.ProtMasterMgm[i].pMasterTable->Master[j];

                /* if we already received SYNC and wait for FU to arrive
                */
                if  (( pMaster->Status == GSY_FWD_MASTER_STS_FWD_SYNC_RECEIVED ) ||
                     ( pMaster->Status == GSY_FWD_MASTER_STS_FWD_ANNOUNCE_RECEIVED ))
                {
                    pFCB = pMaster->FCBQueue.pBottom;

                    /* free all FCBs of this Master and channel waiting for FU to arrive.
                    */
                    while (! LSA_HOST_PTR_ARE_EQUAL(pFCB, LSA_NULL))
                    {
                        pFCB1 = pFCB;
                        pFCB  = pFCB->UseQueue.pNext;

                        if (( LSA_HOST_PTR_ARE_EQUAL(pChSys,pFCB1->pChSys)) &&
                            ( pFCB1->Status == GSY_FWD_FCB_STS_FU_WAITING ))
                        {
                            /* ReQueue FCB. No further handling
                            */
							GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Cancel all FU waiting pFCB=%X pMaster=%X MIdx=%u Port=%u",
                            				pFCB1, pMaster, pMaster->ThisIdx, pFCB1->pPort->ThisPortID);
                            gsy_FwdFCBRequeue(pFCB1,GSY_FWD_FCB_STS_FREE);
                        }
                    }
                }
            }
        }
    }
    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdCancelFUWaitingFCB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdSendTrigger                                  +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID (1..PortCnt)                           +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for pending FCBs on port an starts sending if so.  +*/
/*+               If the FCB is out of date we enqueue it.                  +*/
/*+               With FU we calculate the delay field.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID gsy_FwdSendTrigger(
    GSY_CH_SYS_PTR                    pChSys,
    LSA_UINT16                        PortID)
{
    GSY_FWD_PORT_PTR_TYPE     pPort;
    GSY_FWD_FCB_PTR_TYPE      pFCB;
    GSY_FWD_MASTER_PTR_TYPE   pMaster;
    GSY_LOWER_RQB_PTR_TYPE    pRbl;
    GSY_LOWER_TXMEM_PTR_TYPE  pTxMem;
    LSA_BOOL                  Ready,Ok;
    LSA_UINT32                NewSign = 0;      /* default positiv */
    //LSA_UINT32                NewDelay1NS = 0;
    LSA_INT32                 NewDelayNS = 0;
    //LSA_UINT32                NewDelay10NS = 0;
    LSA_UINT32                LocalDelay = 0;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdSendTrigger() pChSys=0x%X Port=%u",
                          pChSys, PortID);

    pPort = &pChSys->Fwd.pPortMgm[PortID-1];

    Ready = LSA_FALSE;

    /* loop till no more FCBs pending or all tx-resources used     */
    /* NOTE: we currently have only on tx-resource! if we got more */
    /*       we must change pFCBInProgress handling (need a queue!)*/

    while (( ! Ready ) && ( pPort->TxRQBQueue.Count ))
    {
        GSY_REM_BLOCK_BOTTOM_SENDQUEUE(pPort->FCBSendQueue.pBottom,
                                       pPort->FCBSendQueue.pTop,
                                       pFCB);

        if (! LSA_HOST_PTR_ARE_EQUAL(pFCB,LSA_NULL) )
        {
            pMaster = pFCB->pMaster;

            switch (pFCB->Status)
            {
                    /*---------------------------------------------------------------*/
                case GSY_FWD_FCB_STS_SYNC_SEND:
                    /*---------------------------------------------------------------*/

                    if (( pMaster->SequenceID  != pFCB->SequenceID ) ||           /* outdated */
                        ( pMaster->MasterSeqID != pFCB->MasterSeqID) ||
                        (0 == (pPort->FwdPortMode[pFCB->MACAddrID] & GSY_FWD_MODE_OUTPUT))) /* port not open anymore 070907lrg002 */
                    {

                        GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: SYNC-FCB out of date: discard! Port=%u MSeqId=%u FCBSeqId=%u FwdMode=%u",
                                         pPort->ThisPortID, pMaster->SequenceID, pFCB->SequenceID, pPort->FwdPortMode[pFCB->MACAddrID]);

                        /* UnQueue FCB. No further handling */
                        gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                    }
                    else
                    {
                        pRbl = pPort->TxRQBQueue.pFirst;

                        GSY_ASSERT_NULL_PTR(pRbl);

                        pTxMem = GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);

                        GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->Sync.Frame,
                                            (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pTxMem,
                                            pMaster->Sync.Len);

                        /* 181006lrg001: Port-MAC-Adresse benutzen
                         * 030707lrg001: not longer
                         * 201207lrg001: Only if FollowUp generated
                         * 100108lrg001: Only if SRC-MAC is equal to MasterInterfaceMAC
                        */
                        #ifdef GSY_FWD_PORT_SRC_MAC_ADDR
                        if (pMaster->Status == GSY_FWD_MASTER_STS_FWD_FU_GENERATED)
                        {
                            GSY_LOWER_TX_PDU_PTR pPdu = (GSY_LOWER_TX_PDU_PTR)pTxMem;
                            if ((GSY_MACADDR_CMP(pPdu->PduHdr.SrcMacAdr, pPdu->PtcpData.Sync.TLV.Subdomain.MasterSourceAddress)) && (0 != PortID))
                                pPdu->PduHdr.SrcMacAdr = pChSys->Port[PortID-1].MACAddr;
                        }
                        #endif
                        GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sending SYNC: pChSys=%X pRbl=%X Port=%u SeqId=%u Len=%u",
                                         pChSys, pRbl, PortID, pMaster->SequenceID, pMaster->Sync.Len);

                        /* dequeue this rqb */
                        gsy_DequeLower(&pPort->TxRQBQueue,pRbl);
                        /* start sending with timestamp */
                        gsy_SyncSend(pChSys,pRbl,pMaster->Sync.Len,PortID,(LSA_UINT16)(GSY_FRAMEID_FUSYNC_MIN | pMaster->ThisProt));

                        pPort->pFCBInProgress = pFCB;
                        pFCB->Sending = LSA_TRUE;
                        #ifdef GSY_FWD_CNF_TIME
                        pFCB->SndTimeStamp = GSY_GET_TIMESTAMP(pChSys->pSys); //lrgtmp
                        #endif
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case GSY_FWD_FCB_STS_FU_SEND:
                    /*---------------------------------------------------------------*/

                    if (( pMaster->SequenceID  != pFCB->SequenceID ) ||          /* outdated */
                        ( pMaster->MasterSeqID != pFCB->MasterSeqID) ||
                        (0 == (pPort->FwdPortMode[pFCB->MACAddrID] & GSY_FWD_MODE_OUTPUT))) /* port not open anymore 070907lrg002 */
                    {
                        GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU-FCB out of date: discard! Port=%u MSeqId=%u FCBSeqId=%u FwdMode=%u",
                                         pPort->ThisPortID, pMaster->SequenceID, pFCB->SequenceID, pPort->FwdPortMode[pFCB->MACAddrID]);

                        /* UnQueue FCB. No further handling */
                        gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                    }
                    else
                    {
                        Ok = LSA_TRUE;

                        /* ----------------------------------------------------------------------------*/
                        /* if we dont have full hw-support we have to handle delays depending on       */
                        /* correction mode                                                             */
                        /* ----------------------------------------------------------------------------*/

                        if ( pChSys->Fwd.DelayCorrectionMode != GSY_FWD_DELAY_CORRECTION_FULL_HW_SUPPORT )
                        {
                            /* 120707lrg001: If a send timestamp was in the sync send RQB...
                            */
                            if (pFCB->SyncSendTimeValid)
                            {
                                /* local Sync send Delay */
                                LocalDelay  = pFCB->SyncSendTimeStamp - pMaster->SyncRecvTimeStamp;

                                /* we limit the local delay to avoid 32 Bit overflow. */
                                if ( LocalDelay <= pChSys->Fwd.MaxLocalDelay)
                                {
                                    LSA_INT32  DriftNS; /* drift maybe negative !*/
                                    LSA_UINT32 HelpNS  = 0;
                                    //LSA_UINT32 HelpDelay10NS;
                                    LSA_UINT32 HelpDelay1NS;
                                    LSA_INT32  Help;

                                    /* convert local dealy to ns */
                                    LocalDelay = LocalDelay * pChSys->RxTxNanos;

                                    /* ------------------------------------------------------------------------*/
                                    /* calculate drift correction for local delay                              */
                                    /* ------------------------------------------------------------------------*/

                                    DriftNS = gsy_DriftCorrection(pChSys, LocalDelay, pFCB->MACAddrID);

                                    GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU drift corr.: pChSys=%X Port=%u SyncId=%u BridgeDelay=%u drift=%d",
                                                     pChSys, PortID, pMaster->ThisProt, LocalDelay, DriftNS);

                                    switch ( pChSys->Fwd.DelayCorrectionMode )
                                    {
                                        case GSY_FWD_DELAY_CORRECTION_ERTEC400:
                                            /*---------------------------------------------------------------------*/
                                            /* For ERTEC 400:                                                      */
                                            /* Calculate: DriftCorrection + RxLineDelay - HWLineDelay              */
                                            /*            (maybe negativ!)                                         */
                                            /*---------------------------------------------------------------------*/
                                            GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU ERTEC400   : pChSys=%X Port=%u Ld=%d HwLd=%d",
                                                             pChSys, PortID, pMaster->LineDelayNS, pFCB->LineDelayHWNS);

                                            /* Note: all values are << 32 Bit limit, so no oveflow should occur    */

                                            Help = (LSA_INT32)(pMaster->LineDelayNS) + DriftNS - (LSA_INT32)(pFCB->LineDelayHWNS);

                                            if ( Help < 0 )
                                            {
                                                HelpNS   = (LSA_UINT32)(- Help);
                                                NewSign  = 1; /* negativ */
                                            }
                                            else
                                            {
                                                HelpNS = (LSA_UINT32)(Help);
                                            }
                                            break;

                                        case GSY_FWD_DELAY_CORRECTION_ERTEC200_DC:
                                            /*---------------------------------------------------------------------*/
                                            /* For ERTEC 200:                                                      */
                                            /* Calculate: DriftCorrection                                          */
                                            /*            (maybe negativ!)                                         */
                                            /*---------------------------------------------------------------------*/
                                            GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU ERTEC200   : pChSys=%X Port=%u",
                                                             pChSys, PortID);

                                            /* Note: all values are << 32 Bit limit, so no oveflow should occur    */

                                            Help = DriftNS;

                                            if ( Help < 0 )
                                            {
                                                HelpNS   = (LSA_UINT32)(- Help);
                                                NewSign  = 1; /* negativ */
                                            }
                                            else
                                            {
                                                HelpNS = (LSA_UINT32)(Help);
                                            }
                                            break;

                                        default: /* GSY_FWD_DELAY_CORRECTION_NO_HW_SUPPORT */
                                            /*---------------------------------------------------------------------*/
                                            /* For Standard (no HW-Support)                                        */
                                            /* Calculate: LocalDelay + DriftCorrection + RxLineDelay               */
                                            /*            (always positiv even if DriftNS is negativ, because      */
                                            /*             LocalDelay is always > DriftNS!)                        */
                                            /*---------------------------------------------------------------------*/
                                            GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU no HW      : pChSys=%X Port=%u Ld=%d",
                                                             pChSys, PortID, pMaster->LineDelayNS);

                                            HelpNS = LocalDelay + DriftNS + pMaster->LineDelayNS;   //lint !e737 Loss of sign, see Calculate-comment above, HM 13.06.2016

                                            if ( HelpNS < LocalDelay ) /* overflow */
                                            {
                                                GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: Delay overflow occured!");
                                                HelpNS = 0xFFFFFFFF;  /* set to maximum */
                                            }
                                            break;
                                    }
                                    /* ------------------------------------------------------------------------*/
                                    /* calculate new FU-Delay field (maybe positiv or negativ)                 */
                                    /* ------------------------------------------------------------------------*/

                                    /* split into 10/1NS fields
                                     * 110707lrg001: not longer needed because the new DelayNS field is int
                                    */

                                    /* Check for overflow
                                    */
                                    if (pMaster->FollowUp.DelayNS < 0)
                                        HelpDelay1NS = (LSA_UINT32)(-pMaster->FollowUp.DelayNS);
                                    else
                                        HelpDelay1NS = (LSA_UINT32)(pMaster->FollowUp.DelayNS);
                                    if (HelpNS <= (GSY_MAX_NANOS_PER_INT32 - HelpDelay1NS))
                                    {
                                        /* OK, add or sub the local correction to/from the accumulated value
                                        */
                                        if (NewSign != 0)
                                            NewDelayNS = pMaster->FollowUp.DelayNS - (LSA_INT32)(HelpNS);
                                        else
                                            NewDelayNS = pMaster->FollowUp.DelayNS + (LSA_INT32)(HelpNS);
                                    }
                                    else
                                    {
                                        /* Delay exceeds maximum or minimum int32 value;
                                         * Discard FollowUp frame: UnQueue FCB. No further handling
                                        */
                                        GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: Delay (old=%d new=%u neg=%u) exceeds max/min! Discard FU SeqId=%u",
                                                         pMaster->FollowUp.DelayNS, HelpNS, NewSign, pMaster->SequenceID);
                                        pChSys->Fwd.Stats.DelayExceededLimitCnt++;
                                        gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                                        Ok = LSA_FALSE;
                                    }
                                    /* ------------------------------------------------------------------------*/
                                }
                                else
                                {
                                    /* Delay exceeds limits; Discard.  */
                                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: Delay=%u (ticks) exceeds limit! Discard FU SeqId=%u",
                                                     LocalDelay, pMaster->SequenceID);
                                    pChSys->Fwd.Stats.DelayExceededLimitCnt++;
                                    /* UnQueue FCB. No further handling */
                                    gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                                    Ok = LSA_FALSE;
                                }
                            }
                            else
                            {
                                /* No valid Timesstamp for Sync frame; Discard FU.
                                */
                                GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"FWD: No valid Timestamp for sync frame! Discard FU Port=%u SeqId=%u SyncId=%u",
                                                   PortID, pMaster->SequenceID, pMaster->ThisProt);
                                pChSys->Fwd.Stats.DelayExceededLimitCnt++;

                                /* UnQueue FCB. No further handling
                                */
                                gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                                Ok = LSA_FALSE;
                            }
                        }
                        else
                        {
                            /* Full HW-Support. No need to change Delay within FU. use original values
                            */
                            NewDelayNS = pMaster->FollowUp.DelayNS;
                        }

                        /* ----------------------------------------------------------------------------*/
                        /* If OK send out FU-Frame                                                     */
                        /* ----------------------------------------------------------------------------*/

                        if ( Ok )
                        {
                            /* ------------------------------------------------------------------------*/
                            /* Update Delay fields                                                     */
                            /* Note: The delay field wil be overwritten with each sent followup frame  */
                            /* ------------------------------------------------------------------------*/

                            /* set sign flag
                             * 110707lrg001: not longer needed because new DelayNS field is int
                            */
                            GSY_SET_U32(&pMaster->FollowUp.Frame[pMaster->FollowUp.DelayNSOffset], (LSA_UINT32)NewDelayNS);

                            GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Setting FU delay for Port=%u SyncId=%u: LocalDelay=%d OldDelay=%d NewDelay=%d",
                                             PortID, pMaster->ThisProt, LocalDelay, pMaster->FollowUp.DelayNS, NewDelayNS);

                            pRbl = pPort->TxRQBQueue.pFirst;

                            GSY_ASSERT_NULL_PTR(pRbl);

                            pTxMem = GSY_LOWER_RQB_SEND_PTR_GET(pRbl);

                            GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->FollowUp.Frame,
                                                (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pTxMem,
                                                pMaster->FollowUp.Len);

                            /* 181006lrg001: Port-MAC-Adresse benutzen
                             * 030707lrg001: not longer
                             * 201207lrg001: Only if FollowUp generated
                             * 100108lrg001: Only if SRC-MAC is equal to MasterInterfaceMAC
                            */
                            #ifdef GSY_FWD_PORT_SRC_MAC_ADDR
                            if (pMaster->Status == GSY_FWD_MASTER_STS_FWD_FU_GENERATED)
                            {
                                GSY_LOWER_TX_PDU_PTR pPdu = (GSY_LOWER_TX_PDU_PTR)pTxMem;
                                if ((GSY_MACADDR_CMP(pPdu->PduHdr.SrcMacAdr, pPdu->PtcpData.SyncFu.Subdomain.MasterSourceAddress)) && (0 != PortID))
                                    pPdu->PduHdr.SrcMacAdr = pChSys->Port[PortID-1].MACAddr;
                            }
                            #endif
                            GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sending FU: pChSys=%X pRbl=%X Port=%u SeqId=%u Len=%d",
                                             pChSys, pRbl, PortID, pMaster->SequenceID, pMaster->FollowUp.Len);

                            /* dequeue this rqb */
                            gsy_DequeLower(&pPort->TxRQBQueue,pRbl);
                            /* start sending without timestamp */
                            gsy_SyncSend(pChSys,pRbl,pMaster->FollowUp.Len,PortID,(LSA_UINT16)(GSY_FRAMEID_FU_MIN | pMaster->ThisProt));

                            pPort->pFCBInProgress = pFCB;
                            pFCB->Sending = LSA_TRUE;
                            #ifdef GSY_FWD_CNF_TIME
                            pFCB->SndTimeStamp = GSY_GET_TIMESTAMP(pChSys->pSys); //lrgtmp
                            #endif
                        }
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case GSY_FWD_FCB_STS_ANNOUNCE_SEND:
                    /*---------------------------------------------------------------*/

                    if (((0 == (pPort->FwdPortMode[pFCB->MACAddrID] & GSY_FWD_MODE_OUTPUT)))
                    &&  (0 == (pPort->FwdPortMode[pFCB->MACAddrID] & GSY_FWD_MODE_INPUT)))
                    {
						 /* port not open anymore 070907lrg002: 0 ==
						  * 010310lrg001: not GSY_FWD_MODE_OUTPUT and not GSY_FWD_MODE_INPUT
						 */
                        GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: ANNOUNCE-FCB out of date: discard! Port=%u FwdMode=%u",
                                         pPort->ThisPortID, pPort->FwdPortMode[pFCB->MACAddrID]);

                        /* UnQueue FCB. No further handling */
                        gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                    }
                    else
                    {
                        pRbl = pPort->TxRQBQueue.pFirst;

                        GSY_ASSERT_NULL_PTR(pRbl);

                        pTxMem = GSY_LOWER_RQB_SEND_PTR_GET(pRbl);

                        GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->Announce.Frame,
                                            (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pTxMem,
                                            pMaster->Announce.Len);

                        /* 181006lrg001: Port-MAC-Adresse benutzen
                         * 030707lrg001: not longer
                        pPdu = (GSY_LOWER_TX_PDU_PTR)pTxMem;
                        pPdu->PduHdr.SrcMacAdr = pChSys->Port[PortID-1].MACAddr;
                        */
                        GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sending ANNOUNCE: pChSys=%X pRbl=%X Port=%u SeqId=%u Len=%u",
                                         pChSys, pRbl, PortID, pFCB->SequenceID, pMaster->Announce.Len);

                        /* dequeue this rqb */
                        gsy_DequeLower(&pPort->TxRQBQueue,pRbl);

                        /* start sending without timestamp
                         * 101208lrg001: Send Announce with gsy_DataSend()
                         * 150109lrg001: PortId and PduLen exchanged
                         * 110210lrg001: Send Announce with gsy_AnnoSend()
                         *
                        */
                        gsy_AnnoSend(pChSys, pRbl, PortID, pMaster->Announce.Len);

                        pPort->pFCBInProgress = pFCB;
                        pFCB->Sending = LSA_TRUE;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                default:
                    /*---------------------------------------------------------------*/
                    /* unexpected state */
                    GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"gsy_FwdSendTrigger(). Unexpected FCB State=%u!",pFCB->Status);
                    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                    break;
            }
        }
        else
        {
            /* no more FCB present. ready */
            Ready = LSA_TRUE;
        }
    }

    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdSendTrigger()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdSendShutdown                                 +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID (1..PortCnt)                           +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for pending FCBs on port and abort (release) them. +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID gsy_FwdSendShutdown(
    GSY_CH_SYS_PTR                    pChSys,
    LSA_UINT16                        PortID)
{
    GSY_FWD_PORT_PTR_TYPE     pPort;
    GSY_FWD_FCB_PTR_TYPE      pFCB;
    LSA_BOOL                  Ready;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdSendShutdown() pChSys=%X Port=%u", pChSys, PortID);

    pPort = &pChSys->Fwd.pPortMgm[PortID-1];

    Ready = LSA_FALSE;

    while ( ! Ready )
    {
        GSY_REM_BLOCK_BOTTOM_SENDQUEUE(pPort->FCBSendQueue.pBottom,
                                       pPort->FCBSendQueue.pTop,
                                       pFCB);

        if (! LSA_HOST_PTR_ARE_EQUAL(pFCB,LSA_NULL) )
        {
            /* ReQueue FCB. No further handling */
            gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
        }
        else
        {
            /* no more FCB present. ready */
            Ready = LSA_TRUE;
        }
    }
    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdSendShutdown()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdFreeMgm                                      +*/
/*+  Input/Output  :    LSA_VOID                                            +*/
/*+  Result        :    LSA_VOID                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees forwarding mangagement.                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdFreeMgm(LSA_VOID)
{
    LSA_UINT32 i;
    LSA_UINT16 ret_val;

    GSY_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdFreeMgm()");

    for ( i=0; i<GSY_FWD_MAC_ADDR_CNT; i++)
    {
        if (! LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[i].pMasterTable, LSA_NULL) )
        {
            GSY_FREE_LOCAL_MEM(&ret_val,gsy_data.Fwd.ProtMasterMgm[i].pMasterTable);
            GSY_MEM_FREE_FAIL_CHECK(ret_val);
        }
    }
    GSY_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdFreeMgm()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdInit                                         +*/
/*+  Input/Output  :    LSA_UINT32     ProtocolSupport;                     +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ProtocolSupport        : Bitfield   Support protocol                   +*/
/*+                                                                         +*/
/*+                           Bit  = 0   Dont support protocol              +*/
/*+                           Bit  = 1   support protocol                   +*/
/*+                                                                         +*/
/*+                           Bit  DST-MAC               protocol           +*/
/*+                                                                         +*/
/*+                           0  : 01-0E-CF-00-04-00/20  PNIO-Clock sync    +*/
/*+                           1  : 01-0E-CF-00-04-01/21  PNIO-Time sync     +*/
/*+                           :                                             +*/
/*+                           1F : 01-0E-CF-00-04-1F/3F                     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes forwarding module. Called on GSY startup.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdInit(
    LSA_UINT32       ProtocolSupport)
{
    LSA_UINT16 Response;
    LSA_UINT32 i,j,k;

    GSY_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdInit() ProtSupport=0x%x",
                          ProtocolSupport);

    GSY_MEMSET_LOCAL(&gsy_data.Fwd,0,sizeof(gsy_data.Fwd));

    i = 0;
    k = 1;
    Response = GSY_RSP_OK;

    gsy_data.Fwd.Timer.Running = LSA_FALSE;

    while ( ( i<GSY_FWD_MAC_ADDR_CNT ) && ( Response == GSY_RSP_OK ))
    {
        if ( ProtocolSupport & k)
        {

            GSY_FWD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"gsy_FwdInit() SyncId=%u supported",i);

            gsy_data.Fwd.ProtCnt++;

            GSY_ALLOC_LOCAL_MEM((GSY_LOCAL_MEM_PTR_TYPE*)&gsy_data.Fwd.ProtMasterMgm[i].pMasterTable,sizeof(GSY_FWD_MASTER_PER_PROT_TYPE));

            if ( LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[i].pMasterTable, LSA_NULL) )
            {
                Response = GSY_RSP_ERR_RESOURCE;
                GSY_ERROR_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"gsy_FwdInit() Allocating memory failed!");
            }
            else
            {
                GSY_MEMSET_LOCAL(gsy_data.Fwd.ProtMasterMgm[i].pMasterTable,0,sizeof(GSY_FWD_MASTER_PER_PROT_TYPE));

                for ( j=0; j<GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; j++)
                {
                    gsy_data.Fwd.ProtMasterMgm[i].pMasterTable->Master[j].Status  = GSY_FWD_MASTER_STS_IDLE;
                    gsy_data.Fwd.ProtMasterMgm[i].pMasterTable->Master[j].ThisIdx  = j;
                    gsy_data.Fwd.ProtMasterMgm[i].pMasterTable->Master[j].ThisProt = i;
//                  gsy_data.Fwd.ProtMasterMgm[i].FwdUserMode = GSY_FWD_MODE_DISABLE;	310809lrg001
                }
            }
        }
        i++;
        k <<= 1;
    }

    if ( Response != GSY_RSP_OK )
    {
        gsy_FwdFreeMgm();
    }

    GSY_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdInit() Response=0x%x",
                          Response);

    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdExit                                         +*/
/*+  Input/Output  :    LSA_VOID                                            +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes forwarding module. Called on GSY startup.     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdExit(LSA_VOID)
{
    LSA_UINT16 Response;

    GSY_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdExit()");

    gsy_FwdFreeMgm();

    Response = GSY_RSP_OK;

    GSY_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdExit() Response=0x%x", Response);

    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdFreeChannelMgm                               +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+  Result        :    LSA_VOID                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: frees channel management.                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdFreeChannelMgm(
    GSY_CH_SYS_PTR          pChSys)
{
    LSA_UINT16              ret_val;
    GSY_LOWER_RQB_PTR_TYPE  pRbl;
    GSY_FWD_PORT_PTR_TYPE   pPort;
    LSA_UINT32              i;

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdFreeChannelMgm() pChSys=0x%x",
                          pChSys);

    /* free all FCBs
    */
    if (! LSA_HOST_PTR_ARE_EQUAL(pChSys->Fwd.pFCBAlloc, LSA_NULL) )
    {
        GSY_FREE_LOCAL_MEM(&ret_val,pChSys->Fwd.pFCBAlloc);
        GSY_MEM_FREE_FAIL_CHECK(ret_val);
        pChSys->Fwd.pFCBAlloc = LSA_NULL;
    }

    /* free Port-Management
    */
    if (! LSA_HOST_PTR_ARE_EQUAL(pChSys->Fwd.pPortMgm, LSA_NULL) )
    {
        pPort = pChSys->Fwd.pPortMgm;

        /* free all TxRqbs with Tx-Buffers
        */
        for ( i=0; i< pChSys->Fwd.PortCnt; i++)
        {
            while (pPort->TxRQBQueue.Count )
            {
                pRbl = pPort->TxRQBQueue.pFirst;
                gsy_DequeLower(&pPort->TxRQBQueue, pRbl);
                gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
            }
            while (pPort->AnnoRQBQueue.Count )
            {
                pRbl = pPort->AnnoRQBQueue.pFirst;
                gsy_DequeLower(&pPort->AnnoRQBQueue, pRbl);
                gsy_FreeLowerRb(pChSys, pRbl, LSA_TRUE);
            }
            pPort++;
        }
        GSY_FREE_LOCAL_MEM(&ret_val,pChSys->Fwd.pPortMgm);
        GSY_MEM_FREE_FAIL_CHECK(ret_val);
        pChSys->Fwd.pPortMgm = LSA_NULL;
    }

    /* delete pChSys from global management      */
    /* (pChSys must be unique!)                  */
    /* may not be present if we abort on startup */

    i = 0;

    while (i<GSY_MAX_SYS_CHANNELS)
    {
        if ( LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.EDDSysChannel[i], pChSys) )
        {
            gsy_data.Fwd.EDDSysChannel[i] = LSA_NULL;
            gsy_data.Fwd.EDDSysChannelCnt--;
        }
        i++;
    }

    /* free timer if this was the last channel */
    /* function checks inside if timer in use  */

    if (gsy_data.Fwd.EDDSysChannelCnt == 0)
    {
        (LSA_VOID)gsy_TimerClean(&gsy_data.Fwd.Timer);
    }

    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdFreeChannelMgm()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdChannelInit                                  +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortCnt                     +*/
/*+                     GSY_FWD_INIT_PTR_TYPE   pPortInit                   +*/
/*+                     LSA_VOID LSA_FCT_PTR(...Cbf                         +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortCnt                : Number of Ports supported by this Channel     +*/
/*+  pPortInit              : Pointer to array (PortCnt number) of          +*/
/*+                           per Port initialization data.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+       FwdMode             Bit  = 0   Forwarding disabled                +*/
/*+                           Bit  = 1   Forwarding enabled                 +*/
/*+                                                                         +*/
/*+                           Bit  DST-MAC               protocol           +*/
/*+                                                                         +*/
/*+                           0  : 01-0E-CF-00-04-00/20  PNIO-Clock sync    +*/
/*+                           1  : 01-0E-CF-00-04-01/21  PNIO-Time sync     +*/
/*+                           :                                             +*/
/*+                           1F : 01-0E-CF-00-04-1F/3F                     +*/
/*+                                                                         +*/
/*+  Cbf                    : Callbackfunction called on finish.            +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_RESOURCE                                      +*/
/*+               GSY_RSP_ERR_PARAM                                         +*/
/*+               GSY_RSP_OK_ACTIVE                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes systemchannel management of forwarding module.+*/
/*+               Called by GSY after opening the EDD-Systemchannel.        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdChannelInit(
    GSY_CH_SYS_PTR          pChSys,
    LSA_UINT16              PortCnt,
    GSY_FWD_INIT_PTR_TYPE   pPortInit,
	GSY_CBF_TYPE			Cbf)		//AP01539291
{
    LSA_UINT16            Response;
    GSY_FWD_FCB_PTR_TYPE  pFCB;
    LSA_UINT32            FCBCnt;
    LSA_UINT32            FCBSize;
    GSY_FWD_PORT_PTR_TYPE pPort;
    LSA_UINT32            PortMgmSize;
    LSA_UINT32            i,y,k;
    GSY_FWD_INIT_PTR_TYPE pPortInitHelp;
    LSA_BOOL              Ready;

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdChannelInit() pChSys=%X PortCnt=%u pPortInit=%X",
                          pChSys, PortCnt, pPortInit);

    Response = GSY_RSP_OK;

    if ( ( PortCnt == 0 ) ||
         ( LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL)          ) ||
         ( (0 == Cbf)   ) ||
         ( LSA_HOST_PTR_ARE_EQUAL(pPortInit, LSA_NULL)       ) ||
         ( pChSys->RxTxNanos == 0 ))  /* timestamps needed ! */
    {
		GSY_ERROR_TRACE_03((LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL) ? 0 : pChSys->TraceIdx), LSA_TRACE_LEVEL_ERROR, 
                          "*** gsy_FwdChannelInit() Invalid parameter! pChSys=%X PortCnt=%u pPortInit=%X",
                          pChSys, PortCnt, pPortInit);
        Response = GSY_RSP_ERR_PARAM;
    }
    else
    {
        GSY_MEMSET_LOCAL(&pChSys->Fwd,0,sizeof(pChSys->Fwd));

        pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;

        /* ----------------------------------------------------------------------*/
        /* Set Delay correction depending on hardware.                           */
        /* ----------------------------------------------------------------------*/

        switch (pChSys->HardwareType & GSY_HW_TYPE_MSK)
        {
            case GSY_HW_TYPE_STANDARD:
            case GSY_HW_TYPE_I210:			//lrgi210
            	pChSys->Fwd.DelayCorrectionMode = GSY_FWD_DELAY_CORRECTION_NO_HW_SUPPORT;
                break;
            case GSY_HW_TYPE_ERTEC_400:
                pChSys->Fwd.DelayCorrectionMode = GSY_FWD_DELAY_CORRECTION_ERTEC400;
                break;
            case GSY_HW_TYPE_ERTEC_200:

                #ifndef GSY_CFG_FWD_ERTEC_200_DRIFT_CORRECTION
                pChSys->Fwd.DelayCorrectionMode = GSY_FWD_DELAY_CORRECTION_FULL_HW_SUPPORT;
                #else
                pChSys->Fwd.DelayCorrectionMode = GSY_FWD_DELAY_CORRECTION_ERTEC200_DC;
                #endif
                break;
            case GSY_HW_TYPE_SOC:
            case GSY_HW_TYPE_ERTEC_200P:
            case GSY_HW_TYPE_HERA:    
                pChSys->Fwd.DelayCorrectionMode = GSY_FWD_DELAY_CORRECTION_FULL_HW_SUPPORT;
                break;
            default:
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelInit() HardwareType=0x%x not supported!",
                		          pChSys->HardwareType & GSY_HW_TYPE_MSK);
                Response = GSY_RSP_ERR_PARAM;
                break;
        }

        /* ----------------------------------------------------------------------*/
        /* Init MC-Enabled infos                                                 */
        /* ----------------------------------------------------------------------*/

        for (i=0; i<GSY_FWD_MAC_ADDR_CNT; i++)
        {
            pChSys->Fwd.MCEnabled[i].SyncEnabled = LSA_FALSE;
            pChSys->Fwd.MCEnabled[i].FuEnabled   = LSA_FALSE;
            pChSys->Fwd.MCEnabled[i].AnnoEnabled = LSA_FALSE; /* 240907lrg001 */
            pChSys->Fwd.MCEnabled[i].SyncFuEnabled = LSA_FALSE;
            pChSys->Fwd.MCEnabled[i].FwdUserMode = GSY_FWD_MODE_DISABLE;	//310809lrg001
        }

        /* ----------------------------------------------------------------------*/
        /* Allocate PORT-Management for this channel                             */
        /* ----------------------------------------------------------------------*/

        PortMgmSize = PortCnt * sizeof(GSY_FWD_PORT_TYPE);

        pChSys->Fwd.PortCnt = PortCnt;

        if ( PortMgmSize <= 0xFFFF )
        {
            GSY_ALLOC_LOCAL_MEM((GSY_LOCAL_MEM_PTR_TYPE*)&pPort,(LSA_UINT16) PortMgmSize);

            if ( LSA_HOST_PTR_ARE_EQUAL(pPort, LSA_NULL) )
            {
                Response = GSY_RSP_ERR_RESOURCE;
				GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelInit() Allocating memory failed!");
            }
            else
            {
                GSY_MEMSET_LOCAL(pPort,0,PortMgmSize);

                pChSys->Fwd.pPortMgm      = pPort;
                pChSys->Fwd.MaxLocalDelay = 0xFFFFFFFFUL / pChSys->RxTxNanos;

                pPortInitHelp = pPortInit;

                for (i=0; i<PortCnt; i++) /* all ports */
                {
                    #ifdef GSY_FWD_DBG_LINE_DELAY_SET_ON_INIT
                    pPort->Status      = GSY_FWD_PORT_STS_LINE_DELAY_SET;
                    pPort->LineDelayNS = i * 5 + 3000 + 3;
                    #else
                    pPort->Status     = GSY_FWD_PORT_STS_LINE_DELAY_NOT_SET;
                    #endif
                    pPort->ThisPortID  = (LSA_UINT16) (i+1);
                    pPort->SendEnabled = LSA_TRUE; /* default */
                    pPort->pChSys      = pChSys;

                    pPort->CallCbfAfterSend = LSA_FALSE;

                    /* 240708lrg001: No forwarding til ExtLinkInd with TopoOk=TRUE
                    */
                    pPort->ClockTopoOk = LSA_FALSE;

                    /* Init forwarding options */

                    k = 1;

                    for ( y=0; y<GSY_FWD_MAC_ADDR_CNT; y++) /* all macs/protocols */
                    {
                        pPort->FwdPortMode[y] = ( pPortInitHelp->FwdMode & k) ? GSY_FWD_MODE_ENABLE:GSY_FWD_MODE_DISABLE;
                        k <<= 1;
                    }

                    pPortInitHelp++;
                    pPort++;
                }
            }
        }
        else
        {
            Response = GSY_RSP_ERR_RESOURCE;
			GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelInit() Portmanagement exceeds 64k!");
        }

        /* ----------------------------------------------------------------------*/
        /* Allocate Transmit buffers per Port                                    */
        /* ----------------------------------------------------------------------*/

        if ( Response == GSY_RSP_OK )
        {
            y = PortCnt;
            pPort = pChSys->Fwd.pPortMgm;

            while ( y && ( Response == GSY_RSP_OK ))
            {
                Response = gsy_AllocEthSend(pChSys,&pPort->TxRQBQueue, LSA_NULL, LSA_FALSE);
				if (GSY_RSP_OK == Response)
	                Response = gsy_AllocEthSend(pChSys,&pPort->AnnoRQBQueue, LSA_NULL, LSA_TRUE);
                y--;
                pPort++;
            }
        }

        /* ----------------------------------------------------------------------*/
        /* Allocate FCBs for this channel                                        */
        /* ----------------------------------------------------------------------*/

        if ( Response == GSY_RSP_OK )
        {
            /* calculate total FCBs needed */
            /* we allocate one FCB per Port, Protocol and Master */
            FCBCnt = gsy_data.Fwd.ProtCnt * GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL * PortCnt;

            pChSys->Fwd.FCBCnt  = FCBCnt;

            FCBSize = sizeof(GSY_FWD_FCB_TYPE) * FCBCnt;

            if ( FCBSize <= 0xFFFF )
            {
                GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"gsy_FwdChannelInit() Allocating %u FCBs",FCBCnt);

                GSY_ALLOC_LOCAL_MEM((GSY_LOCAL_MEM_PTR_TYPE*)&pFCB,(LSA_UINT16) FCBSize);

                if ( LSA_HOST_PTR_ARE_EQUAL(pFCB, LSA_NULL) )
                {
                    Response = GSY_RSP_ERR_RESOURCE;
					GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelInit() Allocating memory failed!");
                }
                else
                {
                    GSY_MEMSET_LOCAL(pFCB,0,FCBSize);

                    pChSys->Fwd.pFCBAlloc = pFCB; /* Store for later release */

                    while ( FCBCnt ) /* all FCBs */
                    {
                        pFCB->pChSys = pChSys;
                        pFCB->Status = GSY_FWD_FCB_STS_FREE;

                        GSY_PUT_BLOCK_TOP_USEQUEUE(pChSys->Fwd.FCBFreeQueue.pBottom,
                                                   pChSys->Fwd.FCBFreeQueue.pTop,
                                                   pFCB);
                        pFCB++;
                        FCBCnt--;
                    }
                }
            }
            else
            {
                Response = GSY_RSP_ERR_RESOURCE;
            }
        }

        /* ----------------------------------------------------------------------*/
        /* ALLOC Timer for FU-Timeout if this is the first channel               */
        /* ----------------------------------------------------------------------*/

        if ( gsy_data.Fwd.EDDSysChannelCnt == 0 )
        {

            gsy_data.Fwd.Timer.pChSys       = pChSys;

            if ( gsy_TimerAlloc(&gsy_data.Fwd.Timer,LSA_TIMER_TYPE_CYCLIC,LSA_TIME_BASE_1S) != LSA_RET_OK )
            {
                Response = GSY_RSP_ERR_RESOURCE;
            }
            else
            {
                gsy_data.Fwd.Timer.Running      = LSA_FALSE;
                gsy_data.Fwd.Timer.pTimeoutProc = gsy_FwdTimerEvent;
            }
        }

        /* ----------------------------------------------------------------------*/
        /* Start opening multicast MACs within EDD                               */
        /* ----------------------------------------------------------------------*/

        if ( Response == GSY_RSP_OK )
        {
            pChSys->Fwd.MCSetIdx = 0;

            if ( gsy_FwdMcControl(pChSys,LSA_NULL,LSA_TRUE, &Response) )
            {
                if ( Response == GSY_RSP_OK )
                {
                    pChSys->Fwd.Cbf    = Cbf;
                    pChSys->Fwd.Status = GSY_FWD_CHANNEL_OPENING;
                    Response = GSY_RSP_OK_ACTIVE;
                }
                else
                {
                    /* MC-Enable failed */
					GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelInit() Enable Multicast MAC failed!");
                }
            }
            else /* nothing to open (most likely a wrong config, but we continue)..*/
            {
                pChSys->Fwd.Status = GSY_FWD_CHANNEL_OPEN;
            }
        }

        if (( Response != GSY_RSP_OK ) &&
            ( Response != GSY_RSP_OK_ACTIVE ))
        {
			GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelInit() Channel init failed! Response=0x%x",
   		    		          Response);
            gsy_FwdFreeChannelMgm(pChSys);
        }
        else
        {
            i = 0;
            Ready = LSA_FALSE;

            /* find a free entry to store this pChSys
            */
            while ((i<GSY_MAX_SYS_CHANNELS) && (! Ready))
            {
                if ( LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.EDDSysChannel[i], LSA_NULL) )
                {
                    gsy_data.Fwd.EDDSysChannel[i] = pChSys;
                    gsy_data.Fwd.EDDSysChannelCnt++;
                    Ready = LSA_TRUE;
                }
                i++;
            }

            if ( ! Ready ) /* found no free entry */
            {
				GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdChannelInit() No free Systemchannel entry!");
                gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
            }
        }

        /* if we finish synchron start timer if needed
        */
        if (( Response == GSY_RSP_OK ) && ( ! gsy_data.Fwd.Timer.Running ))
        {
            gsy_TimerStart(&gsy_data.Fwd.Timer,1);
        }
    }
    GSY_FUNCTION_TRACE_01((LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL) ? 0 : pChSys->TraceIdx), LSA_TRACE_LEVEL_CHAT, 
                          "  < gsy_FwdChannelInit() Response=0x%x",
                          Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdDisableActiveMC                              +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_OK_ACTIVE                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts disabling all active MC-MACs for channel .         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdDisableActiveMC(
    GSY_CH_SYS_PTR  pChSys)
{
    LSA_UINT16 Response;

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdDisableActiveMC() pChSys=%X",
                          pChSys);

    Response = GSY_RSP_OK;

    /* start Disable all active MC addresses */

    pChSys->Fwd.MCSetIdx = 0;

    if ( gsy_FwdMcControl(pChSys,LSA_NULL,LSA_FALSE,&Response) )
    {
        if ( Response == GSY_RSP_OK )
        {
            Response = GSY_RSP_OK_ACTIVE;
        }
        else
        {
            /* MC-Disable failed */
			GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdChannelInit() Disable Multicast MAC failed! Response=0x%x",
		    		          Response);
            gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_MC_REQUEST, LSA_NULL, 0);
        }
    }

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdDisableActiveMC() Response=0x%x",
                          Response);

    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdChannelExit                                  +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_VOID  LSA_FCT_PTR(GSY...) Cbf(...)              +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  Cbf                    : Callbackfunction called on finish.            +*/
/*+                                                                         +*/
/*+                           Cbf(pChSys,Response)                          +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_OK_ACTIVE                                         +*/
/*+               GSY_RSP_ERR_PARAM                                         +*/
/*+               GSY_RSP_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Exits forwarding for channel. If we have to wait for some +*/
/*+               pending events, we set GSY_RSP_OK_ACTIVE and call the Cbf +*/
/*+               after we finished.                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdChannelExit(
    GSY_CH_SYS_PTR  pChSys,
	GSY_CBF_TYPE	Cbf)		//AP01539291
{
    LSA_UINT16 Response;

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/, "  > gsy_FwdChannelExit() pChSys=%X",
                          pChSys);

    Response = GSY_RSP_OK;

    if ( ( LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL)       ) ||
         ( (0 == Cbf)                                     ))
    {
        Response = GSY_RSP_ERR_PARAM;
    }
    else
    {
        if (( pChSys->Fwd.Status == GSY_FWD_CHANNEL_OPEN ) &&
            ( 0 == pChSys->Fwd.Cbf)                      )
        {
            pChSys->Fwd.Cbf = Cbf;
            pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSING;

            /* if FCBs from this channel are in use we       */
            /* cancel all FCBs currently waiting for FU      */

            if ( pChSys->Fwd.FCBInUseCnt )
            {
                /* Unqueue all FCBs waiting for FU to arrive */
                /* this prevents timeout to occur, so we     */
                /* dont have to do a special handling within */
                /* timeout function on closing channel.      */

                gsy_FwdCancelFUWaitingFCB(pChSys);

                if (pChSys->Fwd.FCBInUseCnt)
                {
                    /* wait for no FCBs in use anymore */
                    Response = GSY_RSP_OK_ACTIVE;
                    GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"gsy_FwdChannelExit() Must wait for %u FCB(s)", pChSys->Fwd.FCBInUseCnt);
                }
            }

            if ( Response != GSY_RSP_OK_ACTIVE )
            {
                /* no Channel FCBs in use anymore    */

                /* Check if any port of this channel has some queued FCBs (even from */
                /* other systemchannel!!)                                            */
                /* This should not be if GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL */
                /* is defined.                                                       */

                if ( pChSys->Fwd.FCBPortCnt )
                {
                    #ifdef GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL
                    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                    #else
                    Response = GSY_RSP_OK_ACTIVE;
                    #endif
                }
                else
                {
                    /* start Disable all active MC addresses */
                    Response = gsy_FwdDisableActiveMC(pChSys);
                }
            }

            if ( Response == GSY_RSP_OK )
            {
                LSA_BOOL MustWait;

                MustWait = LSA_FALSE;

                /* if this is the last channel and the timer is currently underway */
                /* we may have to wait..                                           */
                if ( gsy_data.Fwd.EDDSysChannelCnt == 1 )
                {
                    /* try to stop and clear timer */
                    if ( gsy_TimerClean(&gsy_data.Fwd.Timer) != LSA_RET_OK )
                    {
                        MustWait = LSA_TRUE;
                        GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnote*/,"gsy_FwdChannelExit() Must wait for timer");
                        gsy_data.Fwd.pCloseChSys = pChSys;
                        Response = GSY_RSP_OK_ACTIVE;
                    }
                }

                if ( ! MustWait )
                {
                    gsy_FwdFreeChannelMgm(pChSys);
                    pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;
                    pChSys->Fwd.Cbf = 0;
                }
            }
        }
        else
        {
		    GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdChannelExit() pChSys=%X Channel not open! Cannot close",
         		                 pChSys);
            Response = GSY_RSP_ERR_SEQUENCE;
        }
    }
    GSY_FUNCTION_TRACE_01((LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL) ? 0 : pChSys->TraceIdx), LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/, 
                          "  < gsy_FwdChannelExit() Response=0x%x",
                          Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdLineDelaySet                                 +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+                     LSA_UINT32              LineDelay                   +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID (1..x)                                 +*/
/*+  LineDelay              : Linedelay in ns.                              +*/
/*+                           0: no linedelay present.                      +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_RESOURCE                                      +*/
/*+               GSY_RSP_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets Linedelay for Port                                   +*/
/*+                                                                         +*/
/*+               Note: LineDelayHW is currently only used with ERTEC 400   +*/
/*+                     because we have to correct the delay within         +*/
/*+                     forwarding.                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdLineDelaySet(
    GSY_CH_SYS_PTR          pChSys,
    LSA_UINT16              PortID,
    LSA_UINT32              LineDelay)
{
    LSA_UINT16 Response;

    Response = GSY_RSP_OK;

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdLineDelaySet() pChSys=%X Port=%u LineDelay=%u",
                          pChSys, PortID, LineDelay);

    if ( pChSys->Fwd.Status == GSY_FWD_CHANNEL_OPEN )
    {
        /* Check for valid parameters */
        if ((PortID == 0 ) || (PortID > pChSys->Fwd.PortCnt ))
        {
            Response = GSY_RSP_ERR_PARAM;
		    GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdLineDelaySet() pChSys=%X LineDelay=%u: Invalid Port=%u",
        	                  pChSys, LineDelay, PortID);
        }
        else
        {
            pChSys->Fwd.pPortMgm[PortID-1].LineDelayNS   = LineDelay;

            if ( LineDelay )
                pChSys->Fwd.pPortMgm[PortID-1].Status = GSY_FWD_PORT_STS_LINE_DELAY_SET;
            else
                pChSys->Fwd.pPortMgm[PortID-1].Status = GSY_FWD_PORT_STS_LINE_DELAY_NOT_SET;
        }
    }
    else
    {
        Response = GSY_RSP_ERR_SEQUENCE;
	    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdLineDelaySet() pChSys=%X Channel not open! Status=%u",
       	                  pChSys, pChSys->Fwd.Status);
    }
    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdLineDelaySet() Response=0x%x",
                          Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdPortForwardCtrl                              +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+                     LSA_UINT32              FwdMode                     +*/
/*+                     LSA_VOID  LSA_FCT_PTR(GSY...) Cbf(...)              +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID (1..x)                                 +*/
/*+  FwdMode                : Related protocols                             +*/
/*+                           Bit  = 0   Forwarding disabled                +*/
/*+                           Bit  = 1   Forwarding enabled                 +*/
/*+                                                                         +*/
/*+                           Bit  DST-MAC               protocol           +*/
/*+                                                                         +*/
/*+                           0  : 01-0E-CF-00-04-00/20  PNIO-Clock sync    +*/
/*+                           1  : 01-0E-CF-00-04-01/21  PNIO-Time sync     +*/
/*+                           :                                             +*/
/*+                           1F : 01-0E-CF-00-04-1F/3F                     +*/
/*+                                                                         +*/
/*+  Cbf                    : Callbackfunction called on finish.            +*/
/*+                           Cbf(pChSys,Response)                          +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_OK_ACTIVE                                         +*/
/*+               GSY_RSP_ERR_RESOURCE                                      +*/
/*+               GSY_RSP_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Controls forwarding of Sync and FollowUp frames from in   +*/
/*+               and output ports.                                         +*/
/*+               If the function has to be done asynchron,                 +*/
/*+               GSY_RSP_OK_ACTIVE is returned and the Cbf is called after +*/
/*+               the function is finished                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdPortForwardCtrl(
    GSY_CH_SYS_PTR          pChSys,
    LSA_UINT16              PortID,
    LSA_UINT32              FwdModeIn,
    LSA_UINT32              FwdModeOut,
	GSY_CBF_TYPE			Cbf)		//AP01539291
{
    LSA_UINT16             Response;
    GSY_FWD_PORT_PTR_TYPE  pPort;
    GSY_FWD_FCB_PTR_TYPE   pFCB;
    GSY_FWD_FCB_PTR_TYPE   pFCB1;
    GSY_FWD_FCB_PTR_TYPE   pFCB2;
    LSA_UINT32             i,k;

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdPortForwardCtrl() pChSys=%X Port=%u FwdModeOut=0x%08x",
                          pChSys, PortID, FwdModeOut);

    Response = GSY_RSP_OK;

    if (( pChSys->Fwd.Status == GSY_FWD_CHANNEL_OPEN ) &&
        ( 0 == pChSys->Fwd.Cbf                       )) /* not already a async call running */
    {
        /* Check for valid parameters
        */
        if ((PortID == 0 ) || (PortID > pChSys->Fwd.PortCnt ))
        {
            Response = GSY_RSP_ERR_PARAM;
		    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdPortForwardCtrl() pChSys=%X: Invalid Port=%u",
        	                  pChSys, PortID);
        }
        else
        {
            pPort = &pChSys->Fwd.pPortMgm[PortID-1];

            k = 1; /* protocol msk */

            for ( i=0; i< GSY_FWD_MAC_ADDR_CNT; i++)
            {
                pPort->FwdPortMode[i] = GSY_FWD_MODE_DISABLE;
                if (FwdModeOut & k)
                    pPort->FwdPortMode[i] |= GSY_FWD_MODE_OUTPUT;
                if (FwdModeIn & k)
                    pPort->FwdPortMode[i] |= GSY_FWD_MODE_INPUT;

                if ( ! LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[i].pMasterTable,LSA_NULL) &&  /* protocol used */
                     (! ( FwdModeOut & k)) )  /* disable port for protocol */
                {

                    /* Note: It may be possible, that we currently have queued FCBs */
                    /*       for this MACId. We search and unqueue them. If we      */
                    /*       currently send a frame, we have to wait till finished. */

                    pFCB = pPort->FCBSendQueue.pBottom;

                    while (! LSA_HOST_PTR_ARE_EQUAL(pFCB, LSA_NULL))
                    {
                        pFCB1 = pFCB;
                        pFCB  = pFCB->SendQueue.pNext;

                        if ( pFCB1->MACAddrID == i )
                        {
                            GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"gsy_FwdPortForwardCtrl() Removeing pFCB=%X from Port=%u SyncId=%u",
                                             pFCB1, PortID, i);

                            /* Remove FCB from SendQueue */
                            GSY_REM_BLOCK_SENDQUEUE(pPort->FCBSendQueue.pBottom,
                                                    pPort->FCBSendQueue.pTop,
                                                    pFCB1,
                                                    pFCB2);       //lint !e774 !e845 Boolean within 'if' always evaluates to False, sanity check in the macro is not verbose, HM 20.05.2016

                            /* ReQueue FCB. No further handling */
                            gsy_FwdFCBRequeue(pFCB1,GSY_FWD_FCB_STS_FREE);

                        }
                    }
                    /* if we currently send a frame for this MACAddrID we have to   */
                    /* wait till it is finished.                                    */

                    if ((! LSA_HOST_PTR_ARE_EQUAL(pPort->pFCBInProgress, LSA_NULL)) &&
                        ( pPort->pFCBInProgress->MACAddrID == i ))
                    {
                        pChSys->Fwd.Cbf              = Cbf;
                        pPort->CallCbfAfterSend      = LSA_TRUE;
                        Response = GSY_RSP_OK_ACTIVE;

                        GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdPortForwardCtrl(). Have to wait for finished sending! Port=%u SyncId=%u",
                                         PortID, i);
                    }
                }
                k <<= 1;
            }
        }
    }
    else
    {
        Response = GSY_RSP_ERR_SEQUENCE;
	    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdPortForwardCtrl() pChSys=%X: Channel not open! Status=%u",
       	                  pChSys, pChSys->Fwd.Status);
    }
    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdPortForwardCtrl() FwdModeIn=0x%08x Response=0x%x",
                          FwdModeIn, Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdUserForwardCtrl                              +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT32              Mode                        +*/
/*+                     LSA_UINT32              MACAddrID                   +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  Mode                   : GSY_FWD_ENABLE                                +*/
/*+                           GSY_FWD_DISABLE                               +*/
/*+  MACAddrID              : Releted address ID                            +*/
/*+                           ID   DST-MAC               protocol           +*/
/*+                                                                         +*/
/*+                           0  : 01-0E-CF-00-04-00/20  PNIO-Clock sync    +*/
/*+                           1  : 01-0E-CF-00-04-01/21  PNIO-Time sync     +*/
/*+                           :                                             +*/
/*+                           1F : 01-0E-CF-00-04-1F/3F                     +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_RESOURCE                                      +*/
/*+               GSY_RSP_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Controls forwarding of Sync and FollowUp frames to user   +*/
/*+               If Enabled: Forwaring only if LineDelay of Port is set and+*/
/*+                           forwarding for port is enabled!               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdUserForwardCtrl(
    GSY_CH_SYS_PTR          pChSys,
    LSA_UINT32              Mode,
    LSA_UINT32              MACAddrID)
{
    LSA_UINT16 Response;

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdUserForwardCtrl() pChSys=%X SyncId=%u Mode=%u",
                          pChSys, MACAddrID, Mode);

    Response = GSY_RSP_OK;

    if ( pChSys->Fwd.Status == GSY_FWD_CHANNEL_OPEN )
    {
        /* Check for valid parameters
        */
        if (( MACAddrID >= GSY_FWD_MAC_ADDR_CNT) ||
            ((Mode != GSY_FWD_MODE_ENABLE) && (Mode != GSY_FWD_MODE_DISABLE) && (Mode != GSY_FWD_MODE_INPUT)))
        {
            Response = GSY_RSP_ERR_PARAM;
		    GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdUserForwardCtrl() pChSys=%X Invalid parameter: SyncId=%u Mode=%u",
        		               pChSys, MACAddrID, Mode);
        }
        else
        {
//          gsy_data.Fwd.ProtMasterMgm[MACAddrID].FwdUserMode = Mode;
            pChSys->Fwd.MCEnabled[MACAddrID].FwdUserMode = Mode;	//310809lrg001

        }
    }
    else
    {
        Response = GSY_RSP_ERR_SEQUENCE;
	    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_FwdUserForwardCtrl() pChSys=%X Channel not open! Status=%u",
       		               pChSys, pChSys->Fwd.Status);
    }

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdUserForwardCtrl() Response=0x%x",
                          Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdHandleSyncFrame                              +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+                     GSY_FWD_FRAME_INFO_TYPE pFrameInfo                  +*/
/*+                     GSY_TLV_INFO_PTR_TYPE   pTLVInfo                    +*/
/*+  Result        :    LSA_VOID                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID of received frame                      +*/
/*+  pFrameInfo             : Frame infos                                   +*/
/*+  pTLVInfo               : TLV   infos                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles received Sync frames                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdHandleSyncFrame(
    GSY_CH_SYS_PTR                      pChSys,
    LSA_UINT16                          PortID,
    GSY_FWD_FRAME_INFO_CONST_PTR_TYPE   pFrameInfo,
    GSY_TLV_INFO_CONST_PTR_TYPE         pTLVInfo)
{
    GSY_FWD_MASTER_PTR_TYPE pMaster;
    GSY_FWD_MASTER_PTR_TYPE pFreeMaster;
    GSY_FWD_FCB_PTR_TYPE    pFCB;
    GSY_FWD_PORT_PTR_TYPE   pPort;
    LSA_UINT32              i,j;
    LSA_BOOL                Ready;
    //GSY_PDU_HDR GSY_LOCAL_MEM_ATTR * pMACHdr;
    GSY_CH_SYS_PTR          pChSysHelp;
    GSY_PTCP_DATA           *pPduFu, *pPduSync;
    LSA_UINT16              NewFrameId;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdHandleSyncFrame() pChSys=%X Port=%u",
                          pChSys, PortID);

    pMaster     = &gsy_data.Fwd.ProtMasterMgm[pFrameInfo->MACAddrId].pMasterTable->Master[0];
    pFreeMaster = LSA_NULL;

    /* 030707lrg001: Support SyncFrame without FollowUp
     *
    if ( pTLVInfo->Param.Sync.FUFlag == LSA_TRUE )
    {
    */
    /* Search for Master entry */

    i       = 0;
    Ready   = LSA_FALSE;

    while (( ! Ready ) && ( i < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL ))
    {
        if ( pMaster->Status != GSY_FWD_MASTER_STS_IDLE )
        {

            if (GSY_MACADDR_CMP(pMaster->SrcMACAddr, pTLVInfo->Param.Sync.Master.MacAddr))
            {
                /* Master already present. If we got no FollowUP yet, or we are finished we
                 * (re)start with this SyncFrame. Also after announce
                 * 030707lrg001: not if FU or SYNC without FU received
                */
                if ((pMaster->Status != GSY_FWD_MASTER_STS_FWD_FU_RECEIVED)
                    &&  (pMaster->Status != GSY_FWD_MASTER_STS_FWD_FU_GENERATED))
                {
                    LSA_UINT16 Delta;

                    /* check if this is a new (greater) Seq-ID */

                    /* The SeqID is older as the current one, if the value          */
                    /* is within 4096 digits behind the current one. because of the */
                    /* wraparound we have to do a little math..                     */
                    /*                                                              */
                    /*                                                              */
                    /* 0                                 0xFFFF                     */
                    /* |--------xxxxxxO...........N--------|                        */
                    /*          |older|                                             */
                    /*           4096                                               */
                    /*                                                              */
                    /* |..N-----xxxxxxO....................|                        */
                    /*          |older|                                             */
                    /*           4096                                               */
                    /* |xxxxO..............N-------------xx|                        */
                    /*  lder|                            |o                         */
                    /*                                                              */
                    /*  N: new SeqID                                                */
                    /*  O: old SeqID                                                */
                    /*                                                              */
                    /*  Delta = (LSA_UINT16) (N-O)                                  */
                    /*                                                              */
                    /*  Note: a SeqID equal (Delta == 0) is handled as old          */
                    /*        this may occure if we have a shortcut                 */

                    Delta = (LSA_UINT16) (pTLVInfo->Param.Sync.SequenceID - pMaster->SequenceID);
                    if ( (Delta == 0) || (Delta > 0xF000) ) /* between -4095..0 is old */
                    {
                        /* SeqID older or equal. Discard Sync */
                        pFreeMaster = LSA_NULL;
                        pChSys->Fwd.Stats.SyncOldSeqIDCnt++;
                        GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: New Sync with old SeqId=%u arrived! Delta=%u Discard Sync",
                                           pTLVInfo->Param.Sync.SequenceID, Delta);
                    }
                    else if (pFrameInfo->DoFwd)     //030707lrg001
                    {
                        pFreeMaster = pMaster;
                        pChSys->Fwd.Stats.SyncRestartCnt++;

                        if ( pMaster->Status == GSY_FWD_MASTER_STS_FWD_SYNC_RECEIVED )
                        {
                            GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: New Sync before FollowUP arrived! Restart with Sync SeqId=%u",
                                             pTLVInfo->Param.Sync.SequenceID);

                            /* cancel all old FCBs waiting for FU */
                            gsy_FwdCancelMasterFUWaitingFCB(pChSys, pMaster);
                        }
                        else
                        {
                            GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: New Sync arrived: Handle Sync");
                        }
                    }
                    /* 030707lrg001: Notify SequenceID
                    */
                    else
                    {
						/* 120110lrg
                        pMaster->SequenceID = pTLVInfo->Param.Sync.SequenceID;
                        pFreeMaster = LSA_NULL;
                        GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Notify SYNC: pMaster=%X Port=%u SeqId=%u SyncId=%u",
                        			pMaster, PortID, pMaster->SequenceID, pMaster->ThisProt);
						*/
                        pFreeMaster = pMaster;
                    }
                }
                else
                {
                    /* New Sync while FollowUP in progress. */
                    /* Discard this Sync frame.             */
                    pFreeMaster = LSA_NULL;
                    pChSys->Fwd.Stats.SyncDiscardFUInProgressCnt++;
                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: MasterStatus=%u: New Sync arrived while FollowUP in progress! Discard SeqId=%u",
                                       pMaster->Status, pTLVInfo->Param.Sync.SequenceID);
                }
                Ready = LSA_TRUE;
            }
        }
        else
        {
            /* save first found free master */

            if (LSA_HOST_PTR_ARE_EQUAL(pFreeMaster, LSA_NULL))
            {
                GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Saving idle pMaster=%X", pMaster);
                pFreeMaster = pMaster;
            }
        }

        i++;
        pMaster++;
    }

    if (!LSA_HOST_PTR_ARE_EQUAL(pFreeMaster, LSA_NULL))
    {
        pMaster = pFreeMaster;

        GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Master is (now) in use: pMaster=%X SyncId=%u MIdx=%u",
                         pMaster, pMaster->ThisProt, pMaster->ThisIdx);

        pMaster->SrcMACAddr = pTLVInfo->Param.Sync.Master.MacAddr;

        pPort = &pChSys->Fwd.pPortMgm[PortID-1];

        pMaster->pChSys = pChSys;
        pMaster->PortID = PortID;
        pMaster->SequenceID = pTLVInfo->Param.Sync.SequenceID;
        pMaster->MasterSeqID++;
        pMaster->LineDelayNS = pPort->LineDelayNS;
        pMaster->SyncRecvTimeStamp = pFrameInfo->TimeStamp;
        pMaster->TimeoutCnt = gsy_data.Fwd.TimerCnt;		// 120110lrg for FU/Fwd-Timeout

        if (!pFrameInfo->DoFwd)
        {
            GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Notify SYNC: Port=%u SeqId=%u",
            				PortID, pMaster->SequenceID);
        }
//      if (pFrameInfo->DoFwd)
		else
        {
            /* 030707lrg001: Forwarding allowed by EDD
            */
            GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pFrameInfo->pMemRx,
                                (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->Sync.Frame,
                                pFrameInfo->IOCount);

            pMaster->Sync.Len   = pFrameInfo->IOCount;

            /* Insert our Source MAC into Frame
             * 030707lrg001: not longer
            pMACHdr = (GSY_PDU_HDR GSY_LOCAL_MEM_ATTR * ) pMaster->Sync.Frame;
            pMACHdr->SrcMacAdr = pChSys->MACAddr;
            */
            if (pTLVInfo->Param.Sync.FUFlag != LSA_FALSE)
            {
                pMaster->Status     = GSY_FWD_MASTER_STS_FWD_SYNC_RECEIVED;
//120110lrg     pMaster->TimeoutCnt = gsy_data.Fwd.TimerCnt;  /* for FU/Fwd-Timeout */

                GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Store SYNC_FU: pChSys=%X Port=%u SeqId=%u TS=0x%X",
                                 pChSys, pMaster->PortID, pMaster->SequenceID, pMaster->SyncRecvTimeStamp);
            }
            else
            {
                /* 030707lrg001: SyncFrame without FollowUp received:
                 *               modify FrameId and generate FU frame
                */
                GSY_MAC_ADR_TYPE    AddrSyncFu = {GSY_MULTICAST_ADDR_FUSYNC};
                GSY_MAC_ADR_TYPE    AddrFu = {GSY_MULTICAST_ADDR_SYNC_FU};
                AddrSyncFu.MacAdr[5] |= (LSA_UINT8)pFrameInfo->MACAddrId;
                AddrFu.MacAdr[5] |= (LSA_UINT8)pFrameInfo->MACAddrId;

                pMaster->Status     = GSY_FWD_MASTER_STS_FWD_FU_GENERATED;
//120110lrg     pMaster->TimeoutCnt = gsy_data.Fwd.TimerCnt;  /* for FU/Fwd-Timeout */

                /* Cut away APDU status and set DstMAC address and FrameId to Sync with FU
                */
                pMaster->Sync.Len = (LSA_UINT16)(pFrameInfo->PduOffset + pTLVInfo->Length);
                GSY_LOWER_FRAME_MACADR_SET(&pMaster->Sync.Frame[0], AddrSyncFu.MacAdr);
                NewFrameId = GSY_SWAP16(GSY_FRAMEID_FUSYNC | (LSA_UINT16)pFrameInfo->MACAddrId);
                *((LSA_UINT16*)(void*)(&pMaster->Sync.Frame[0] + pFrameInfo->PduOffset)) = NewFrameId;

                /* Copy sync PDU header to FU buffer and set FU MC-MAC and FrameId
                */
                GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->Sync.Frame,
                                    (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->FollowUp.Frame,
                                    (LSA_UINT16)pFrameInfo->PduOffset);
                GSY_LOWER_FRAME_MACADR_SET(&pMaster->FollowUp.Frame[0], AddrFu.MacAdr);
                NewFrameId = GSY_SWAP16(GSY_FRAMEID_SYNC_FU | (LSA_UINT16)pFrameInfo->MACAddrId);
                *((LSA_UINT16*)(void*)(&pMaster->FollowUp.Frame[0] + pFrameInfo->PduOffset)) = NewFrameId;

                /* Copy sync SeqId and TLVs to new FU
                */
                pPduFu = (GSY_PTCP_DATA*)(void*)(&(pMaster->FollowUp.Frame[0]) + pFrameInfo->PduOffset + 2);
                pPduSync = (GSY_PTCP_DATA*)(void*)(&(pMaster->Sync.Frame[0]) + pFrameInfo->PduOffset + 2);
                GSY_MEMSET_LOCAL(pPduFu, 0, GSY_SYNC_FU_RX_SIZE);
                pPduFu->SyncFu.PtcpHdr.SequenceID = pPduSync->Sync.PtcpHdr.SequenceID;
                pPduFu->SyncFu.Subdomain = pPduSync->Sync.TLV.Subdomain;
                pPduFu->SyncFu.Time = pPduSync->Sync.TLV.Time;
                pPduFu->SyncFu.End = pPduSync->Sync.TLV.End;

                pMaster->FollowUp.Len = (LSA_UINT16)(pFrameInfo->PduOffset + 2 + GSY_SYNC_FU_RX_SIZE);
                pMaster->FollowUp.DelayNS = 0;
                pMaster->FollowUp.DelayNSOffset = pTLVInfo->Param.Sync.DelayNSOffset  + pFrameInfo->PduOffset;

                GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Store SYNC: pChSys=%X Port=%u SeqId=%u TS=0x%X",
                                 pChSys, pMaster->PortID, pMaster->SequenceID, pMaster->SyncRecvTimeStamp);
            }

            /* start forwarding this sync frame to all open output ports         */
            /* IMPORTANT: Dont undef GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL */
            /*            Because ChannelExit dont supports this                 */

            #ifndef GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL
            for ( i=0; i<GSY_MAX_SYS_CHANNELS; i++)
            {
                pChSysHelp = gsy_data.Fwd.EDDSysChannel[i];
                #else
            {
                pChSysHelp = pChSys; /* only forward on receive channel */
                #endif
                if ( pChSysHelp->Fwd.Status == GSY_FWD_CHANNEL_OPEN )
                {
                    for ( j=0; j< pChSysHelp->Fwd.PortCnt; j++)
                    {
                        pPort = &pChSysHelp->Fwd.pPortMgm[j];

                        /* if port is open for forwarding and send is enabled for port
                         * and linedelay is set for port
                         * 030707lrg001: and EDD allows forwarding
                         * 240708lrg001: and TopoOk is set by ExtLinkIndication or SyncId is not GSY_SYNCID_CLOCK
                        */
                        GSY_FWD_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Port=%u: DoFwdPort=%u FwdPortMode=0x%X Status=%u SendEnabled=%u TopoOk=%u",
                                         j+1, pFrameInfo->DoFwdPort[j], pPort->FwdPortMode[pFrameInfo->MACAddrId], pPort->Status, pPort->SendEnabled, pChSys->Port[j].ClockSyncOk);

                        if (( pFrameInfo->DoFwdPort[j]) &&
                            ( pPort->FwdPortMode[pFrameInfo->MACAddrId] & GSY_FWD_MODE_OUTPUT ) &&
                            ( pPort->Status == GSY_FWD_PORT_STS_LINE_DELAY_SET ) &&
                            ( pPort->SendEnabled ) &&
                            ((pPort->ClockTopoOk) || (pFrameInfo->MACAddrId != GSY_SYNCID_CLOCK)))
                        {
                            /* if not the input channel and port */
                            if ( (j != (LSA_UINT32) (PortID-1)) || (! LSA_HOST_PTR_ARE_EQUAL(pChSysHelp,pChSys)))
                            {
                                pFCB = gsy_FwdFCBGetFree(pChSysHelp);

                                if ( ! LSA_HOST_PTR_ARE_EQUAL(pFCB,LSA_NULL) )
                                {
                                    /* FillUp FCB with Data and queue it to Master and */
                                    /* Destination port                                */

                                    pFCB->MACAddrID         = pFrameInfo->MACAddrId;
                                    pFCB->pMaster           = pMaster;
                                    pFCB->pPort             = pPort;
                                    pFCB->Sending           = LSA_FALSE;
                                    pFCB->SequenceID        = pTLVInfo->Param.Sync.SequenceID;
                                    pFCB->MasterSeqID       = pMaster->MasterSeqID;
                                    pFCB->SyncSendTimeStamp = 0;

                                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Queue SYNC send FCB to pChSys=%X Port=%u",
                                                     pChSysHelp, j+1);

                                    /* Queue FCB */
                                    gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_SYNC_SEND);

                                    /* trigger send for destination port.*/
                                    gsy_FwdSendTrigger(pChSysHelp,(LSA_UINT16) (j+1));

                                }
                                else
                                {
                                    pChSysHelp->Fwd.Stats.OutOfFCBCnt++;
                                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"FWD: No free FCB! Cannot forward to pChSys=%X Port=%u",
                                                     pChSysHelp, j+1);
                                }
                            }
                        }
                    }
                }
            }
        }

        /* if no FCBs queued, Master fwd is finished (out of FCB resources or not forwarded ) */

        if ( pMaster->FCBCnt == 0 )
        {
            GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Master fwd finished (no forwarding occured): pMaster=%X SyncId=%u MIdx=%u",
                             pMaster,
                             pMaster->ThisProt,
                             pMaster->ThisIdx);

            pMaster->Status = GSY_FWD_MASTER_STS_FWD_FINISHED;
        }
    }
    else
    {
        if ( ! Ready )
        {
            pChSys->Fwd.Stats.NoFreeMasterSlotCnt++;
            GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"FWD: no free Master slot for SYNC!");
        }
    }

    /* 030707lrg001: Support SyncFrame without FollowUp
     *
    }
    else
    {
        GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sync whithout FollowUP. Discarded!");
        * Sync without FollowUp not supported yet ! *
        * discard.                                  *
    }
    */

    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdHandleSyncFrame()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdHandleFUFrame                                +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+                     GSY_FWD_FRAME_INFO_TYPE pFrameInfo                  +*/
/*+                     GSY_TLV_INFO_PTR_TYPE   pTLVInfo                    +*/
/*+  Result        :    LSA_VOID                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID of received frame                      +*/
/*+  pFrameInfo             : Frame infos                                   +*/
/*+  pTLVInfo               : TLV   infos                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles received FollowUp frames                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdHandleFUFrame(
    GSY_CH_SYS_PTR                      pChSys,
    LSA_UINT16                          PortID,
    GSY_FWD_FRAME_INFO_CONST_PTR_TYPE   pFrameInfo,
    GSY_TLV_INFO_CONST_PTR_TYPE         pTLVInfo)
{
    GSY_FWD_MASTER_PTR_TYPE pMaster;
    GSY_FWD_FCB_PTR_TYPE    pFCB;
    GSY_FWD_FCB_PTR_TYPE    pFCB1;
    LSA_UINT32              i;
    LSA_BOOL                Ready;
    //GSY_PDU_HDR GSY_LOCAL_MEM_ATTR * pMACHdr;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdHandleFUFrame() pChSys=%X, Port=%u",
                          pChSys,PortID);


    i       = 0;
    Ready   = LSA_FALSE;
    pMaster = &gsy_data.Fwd.ProtMasterMgm[pFrameInfo->MACAddrId].pMasterTable->Master[0];

    /* Search for Master entry */

    while (( ! Ready ) && ( i < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL ))
    {
        if ( pMaster->Status == GSY_FWD_MASTER_STS_FWD_SYNC_RECEIVED )  /* only if we await FU */
        {

            if (GSY_MACADDR_CMP(pMaster->SrcMACAddr, pTLVInfo->Param.FollowUp.Master.MacAddr))
            {
                /* Master present. Check Port and Sequence ID */

                if (( pMaster->PortID == PortID ) &&
                    ( pMaster->SequenceID == pTLVInfo->Param.FollowUp.SequenceID ))
                {
                    Ready = LSA_TRUE;
                }
                else
                {

                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: FU wrong Port=%u or SeqID=%u! Discard.",
                                     PortID, pTLVInfo->Param.FollowUp.SequenceID);

                    pChSys->Fwd.Stats.FUDontMatchPortOrSeqIDCnt++;
                }
            }
        }

        i++;
        if ( ! Ready ) pMaster++;
    }

    if ( Ready ) /* Sync Master found */
    {
        /* Found proper Sync Frame */

        /* Copy FU Frame to Master Management for later use */

        GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pFrameInfo->pMemRx,
                            (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->FollowUp.Frame,
                            pFrameInfo->IOCount);

        pMaster->FollowUp.Len              = pFrameInfo->IOCount;

        /* 110707lrg001: DelaySign etc. not longer used...
        */
        pMaster->FollowUp.DelayNS         = pTLVInfo->Param.FollowUp.DelayNS;
        pMaster->FollowUp.DelayNSOffset   = pTLVInfo->Param.FollowUp.DelayNSOffset  + pFrameInfo->PduOffset;

        pMaster->Status = GSY_FWD_MASTER_STS_FWD_FU_RECEIVED;

        /* Insert our Source MAC into Frame
         * 030707lrg001: not longer
        pMACHdr = (GSY_PDU_HDR GSY_LOCAL_MEM_ATTR * ) pMaster->FollowUp.Frame;
        pMACHdr->SrcMacAdr = pChSys->MACAddr;
        */
        GSY_FWD_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Store FU: pChSys=%X pMaster=%X SyncId=%u MIdx=%u Port=%u SeqId=%u",
                         pChSys, pMaster, pMaster->ThisProt, pMaster->ThisIdx, PortID, pMaster->SequenceID);

        /* Check all FCBs if Sync already send out. */
        /* If so we queue FU for send.              */

        pFCB = pMaster->FCBQueue.pBottom;

        while (! LSA_HOST_PTR_ARE_EQUAL(pFCB, LSA_NULL) )
        {

            pFCB1 = pFCB;
            pFCB  = pFCB->UseQueue.pNext;

            if ( pFCB1->Status == GSY_FWD_FCB_STS_FU_WAITING )
            {

                /* not expected */
                if ( pFCB1->pChSys->Fwd.Status != GSY_FWD_CHANNEL_OPEN )
                {
				    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdHandleFUFrame() pChSys=%X Channel not open! Status=%u",
       					               pFCB1->pChSys, pFCB1->pChSys->Fwd.Status);
                    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                }

                if ( pFCB1->pPort->SendEnabled ) /* only if port is free for send */
                {

                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sync already sent. Queue FU to pChSys=%X Port=%u",
                                     pChSys,
                                     pFCB1->pPort->ThisPortID);

                    /* ReQueue FCB.*/
                    /* Note: Delay field will be updated just before sending */
                    gsy_FwdFCBRequeue(pFCB1,GSY_FWD_FCB_STS_FU_SEND);

                    /* trigger send for destination port.*/
                    gsy_FwdSendTrigger(pChSys,pFCB1->pPort->ThisPortID);
                }
                else
                {
                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: Sync already sent but Send (now) disabled! SeqId=%u Port=%u",
                                     pMaster->SequenceID,
                                     pFCB1->pPort->ThisPortID);

                    /* ReQueue FCB. No further handling */
                    gsy_FwdFCBRequeue(pFCB1,GSY_FWD_FCB_STS_FREE);

                }

            }
            else
            {
                GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sync not sent yet. SeqId=%u Port=%u",
                                 pMaster->SequenceID, pFCB1->pPort->ThisPortID);
            }
        }
    }
    else
    {
        GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: No proper master/sync found for FU! Port=%u SeqId=%u",
                         PortID, pTLVInfo->Param.FollowUp.SequenceID);

        pChSys->Fwd.Stats.FUNoProperSyncFoundCnt++;
    }

    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdHandleFUFrame()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdHandleAnnounceFrame                          +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     LSA_UINT16              PortID                      +*/
/*+                     GSY_FWD_FRAME_INFO_TYPE pFrameInfo                  +*/
/*+                     GSY_TLV_INFO_PTR_TYPE   pTLVInfo                    +*/
/*+  Result        :    LSA_VOID                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  PortID                 : PortID of received frame                      +*/
/*+  pFrameInfo             : Frame infos                                   +*/
/*+  pTLVInfo               : TLV   infos                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles received Announce frames                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdHandleAnnounceFrame(
    GSY_CH_SYS_PTR                      pChSys,
    LSA_UINT16                          PortID,
    GSY_FWD_FRAME_INFO_CONST_PTR_TYPE   pFrameInfo,
    GSY_TLV_INFO_CONST_PTR_TYPE         pTLVInfo)
{
    GSY_FWD_MASTER_PTR_TYPE pMaster;
    GSY_FWD_MASTER_PTR_TYPE pFreeMaster;
    GSY_FWD_FCB_PTR_TYPE    pFCB;
    GSY_FWD_PORT_PTR_TYPE   pPort;
    LSA_UINT32              i,j;
    LSA_BOOL                Ready;
    //GSY_PDU_HDR GSY_LOCAL_MEM_ATTR * pMACHdr;
    GSY_CH_SYS_PTR          pChSysHelp;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdHandleAnnounceFrame() pChSys=%X Port=%u",
                          pChSys, PortID);


    pMaster = &gsy_data.Fwd.ProtMasterMgm[pFrameInfo->MACAddrId].pMasterTable->Master[0];
    pFreeMaster = LSA_NULL;

    /* Search for Master entry */

    i       = 0;
    Ready   = LSA_FALSE;

    while (( ! Ready ) && ( i < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL ))
    {
        if ( pMaster->Status != GSY_FWD_MASTER_STS_IDLE )
        {

            if (GSY_MACADDR_CMP(pMaster->SrcMACAddr, pTLVInfo->Param.Announce.Master.MacAddr))
            {
                /* Master already present. */
                /* Note: The Sync and Announce Seq-Id are the same one!       */
                /*       so we continue on transition of sync/fu <-> announce */
                /* If we currently handling FUs we ignore this announce and   */
                /* finish FU-handling first.                                  */
                /* 041007lrg003: No SeqId check at Announce                   */
                pFreeMaster = pMaster;
                GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: New Announce arrived: Handle Announce");
                Ready = LSA_TRUE;
            }
        }
        else
        {
            /* save first found free master
            */
            if ( LSA_HOST_PTR_ARE_EQUAL(pFreeMaster, LSA_NULL ))
            {
                pFreeMaster = pMaster;
            }
        }

        i++;
        pMaster++;
    }

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pFreeMaster, LSA_NULL ))
    {
        pMaster = pFreeMaster;

        pMaster->SrcMACAddr = pTLVInfo->Param.Announce.Master.MacAddr;

        pPort = &pChSys->Fwd.pPortMgm[PortID-1];

        pMaster->pChSys            = pChSys;
        pMaster->PortID            = PortID;
        pMaster->LineDelayNS       = pPort->LineDelayNS;
        pMaster->SyncRecvTimeStamp = 0;  /* not used */

        GSY_COPY_COMMON_MEM((LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pFrameInfo->pMemRx,
                            (LSA_VOID  GSY_COMMON_MEM_ATTR *  ) pMaster->Announce.Frame,
                            pFrameInfo->IOCount);

        pMaster->Announce.Len   = pFrameInfo->IOCount;

        /* Insert our Source MAC into Frame
         * 030707lrg001: not longer
        pMACHdr = (GSY_PDU_HDR GSY_LOCAL_MEM_ATTR * ) pMaster->Announce.Frame;
        pMACHdr->SrcMacAdr = pChSys->MACAddr;

        090110lrg001: do not overwrite Status
        pMaster->Status     = GSY_FWD_MASTER_STS_FWD_ANNOUNCE_RECEIVED;
        */
        pMaster->TimeoutCnt = gsy_data.Fwd.TimerCnt;  /* for Timeout */
/*
        GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Master is (now) in use (pMaster: 0x%X, SyncId=%u, MIdx=%d)",
                         pMaster,
                         pMaster->ThisProt,
                         pMaster->ThisIdx);
*/
        GSY_FWD_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Store ANNOUNCE: pChSys=%X pMaster=%X SyncId=%u MIdx=%u Port=%u SeqId=%u",
                         pChSys, pMaster, pMaster->ThisProt, pMaster->ThisIdx, pMaster->PortID, pTLVInfo->Param.Announce.SequenceID);

        /* start forwarding this announce frame to all open output ports     */
        /* IMPORTANT: Dont undef GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL */
        /*            Because ChannelExit dont supports this                 */

        #ifndef GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL
        for ( i=0; i<GSY_MAX_SYS_CHANNELS; i++)
        {
            pChSysHelp = gsy_data.Fwd.EDDSysChannel[i];
            #else
        {
            pChSysHelp = pChSys; /* only forward on receive channel */
            #endif
            if ( pChSysHelp->Fwd.Status == GSY_FWD_CHANNEL_OPEN )
            {
                for ( j=0; j< pChSysHelp->Fwd.PortCnt; j++)
                {
                    pPort = &pChSysHelp->Fwd.pPortMgm[j];

                    /* Send frame if linedelay is set for port and send is enabled
                     * and port is open for forwarding
					 * 010310lrg001: send announce if only egress boundary is set at output port
                    */
                    if ((pPort->Status == GSY_FWD_PORT_STS_LINE_DELAY_SET) && pPort->SendEnabled
                    &&  ((pPort->FwdPortMode[pFrameInfo->MACAddrId] & GSY_FWD_MODE_OUTPUT)
					 ||  (pPort->FwdPortMode[pFrameInfo->MACAddrId] & GSY_FWD_MODE_INPUT)))
                    {
                        /* if not the input channel and port
                        */
                        if ( (j != (LSA_UINT32) (PortID-1)) || (! LSA_HOST_PTR_ARE_EQUAL(pChSysHelp,pChSys)))
                        {
                            pFCB = gsy_FwdFCBGetFree(pChSysHelp);

                            if ( ! LSA_HOST_PTR_ARE_EQUAL(pFCB,LSA_NULL) )
                            {
                                /* FillUp FCB with Data and queue it to Master and Destination port
                                */
                                pFCB->MACAddrID         = pFrameInfo->MACAddrId;
                                pFCB->pMaster           = pMaster;
                                pFCB->pPort             = pPort;
                                pFCB->Sending           = LSA_FALSE;
                                pFCB->SequenceID        = pTLVInfo->Param.Announce.SequenceID;
                                pFCB->MasterSeqID       = pMaster->MasterSeqID;
                                pFCB->SyncSendTimeStamp = 0;

                                GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Queue ANNOUNCE send FCB to pChSys=%X, Port=%u",
                                                 pChSysHelp,
                                                 j+1);

                                /* Queue FCB
                                */
                                gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_ANNOUNCE_SEND);

                                /* trigger send for destination port.
                                */
                                gsy_FwdSendTrigger(pChSysHelp,(LSA_UINT16) (j+1));
                            }
                            else
                            {
                                pChSysHelp->Fwd.Stats.OutOfFCBCnt++;
                                GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"FWD: No free FCB! Cannot forward to pChSys=%X Port=%u",
                                                   pChSysHelp, j+1);
                            }
                        }
                    }
                }
            }
        }

        /* if no FCBs queued, Master fwd is finished (out of FCB resources or not forwarded )
        */
        if ( pMaster->FCBCnt == 0 )
        {
            GSY_FWD_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Master fwd finished (no forwarding occured): pMaster=%X SyncId=%u MIdx=%u",
                             pMaster, pMaster->ThisProt, pMaster->ThisIdx);

            pMaster->Status = GSY_FWD_MASTER_STS_FWD_FINISHED;
        }
    }
    else
    {
        if (!Ready)
        {
            pChSys->Fwd.Stats.NoFreeMasterSlotCnt++;
            GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"FWD: no free Master slot for ANNOUNCE!");
        }
    }

    GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdHandleAnnounceFrame()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdRcvIndication                                +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     GSY_LOWER_RQB_PTR_TYPE  pRbl                        +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  pRbl                   : Ponter to EDD Recv-RQB (EDD_SRV_SYNC_RECV)    +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_PARAM                                         +*/
/*+               GSY_RSP_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates recveived Sync or FollowUp frame                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdRcvIndication(
    GSY_CH_SYS_PTR          pChSys,
    GSY_LOWER_RQB_PTR_TYPE  pRbl,
    LSA_BOOL                ReprovideSync)
{
    LSA_UINT16               Response;
    GSY_FWD_PORT_PTR_TYPE    pPort;
    GSY_FWD_FRAME_INFO_TYPE  FrameInfo;			//AP01368288 Compiler Warnings
    LSA_UINT16               PortID;
    GSY_TLV_INFO_TYPE        TLVInfo;
    LSA_BOOL                 Reprovide = LSA_TRUE;      //260508lrg001
    LSA_UINT16               ii;

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdRcvIndication() pChSys=%X pRbl=%X",
                          pChSys, pRbl);

    Response = GSY_RSP_OK;
	GSY_MEMSET_LOCAL(&FrameInfo, 0x00, sizeof(FrameInfo));	// AP01368288 Compiler Warnings

    if ( pChSys->Fwd.Status == GSY_FWD_CHANNEL_OPEN )
    {
        //  Reprovide = LSA_TRUE;   260508lrg001

        switch ( GSY_LOWER_RQB_RESPONSE_GET(pRbl))
        {
                /*-------------------------------------------------------------------*/
            case GSY_LOWER_RSP_OK:
                /*-------------------------------------------------------------------*/

                FrameInfo.pMemRx    = (GSY_LOWER_RXBYTE_PTR)GSY_LOWER_RQB_RECV_SYNC_PTR_GET(pRbl);
                FrameInfo.PduOffset = GSY_LOWER_RQB_RECV_SYNC_DATA_OFFSET_GET(pRbl);
                FrameInfo.pPduRx    = (GSY_LOWER_RXBYTE_PTR)(FrameInfo.pMemRx + GSY_LOWER_RQB_RECV_SYNC_DATA_OFFSET_GET(pRbl));
                FrameInfo.IOCount   = GSY_LOWER_RQB_RECV_SYNC_LEN_GET(pRbl);
                FrameInfo.PduLen    = GSY_LOWER_RQB_RECV_SYNC_DATA_LEN_GET(pRbl);
                FrameInfo.TimeStamp = GSY_LOWER_RQB_RECV_SYNC_TIME_GET(pRbl);

                PortID = GSY_LOWER_RQB_RECV_SYNC_PORT_GET(pRbl);

                /* Check for valid parameters */
                if ((PortID == 0 ) || (PortID > pChSys->Fwd.PortCnt ))
                {
				    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdRcvIndication() pChSys=%X Invalid Port=%u",
                				       pChSys, PortID);
                    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                }

                pPort = &pChSys->Fwd.pPortMgm[PortID-1];

                /* Get DST-MAC Addr - ID (only last 5 Bits of MAC )
                FrameInfo.MACAddrId = GSY_FRAME_GET_DST_MACID(FrameInfo.pMemRx);
                 */
                /* Get SyncID (only least sinificant 5 Bits of FrameId)
                */
                FrameInfo.MACAddrId = GSY_SYNCID_GET_FROM_PDU(FrameInfo.pPduRx);

                /* Check if protocol / MAC supported. should be, because other MACs are disabled
                */
                if (!LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[FrameInfo.MACAddrId].pMasterTable,LSA_NULL))
                {
                    /* only forward if LineDelay of input port is set
                    */
                    if (pPort->Status == GSY_FWD_PORT_STS_LINE_DELAY_SET)
                    {
                        /* forwarding of input port enabled ?
						 * 010310lrg001: handle announce if only ingress boundary is set at input port
                        */
                        if ((pPort->FwdPortMode[FrameInfo.MACAddrId] & GSY_FWD_MODE_INPUT)
						||  ((pPort->FwdPortMode[FrameInfo.MACAddrId] & GSY_FWD_MODE_OUTPUT) && !ReprovideSync))
                        {
                            /* validate and get parameters
                             * lrgi210: TimeStamp
                            */
                            if ( gsy_TLVGetInfo(pChSys,FrameInfo.pPduRx,FrameInfo.PduLen,FrameInfo.TimeStamp,PortID,&TLVInfo))
                            {
                                /* 050707lrg001: Get the lower layer forwarding information
                                */
                                GSY_LOWER_RQB_RECV_SYNC_DOFWD_GET(pRbl, &FrameInfo.DoFwdPort[0], pChSys->Fwd.PortCnt);
                                FrameInfo.DoFwd = LSA_FALSE;
                                for (ii = 0; ii < pChSys->Fwd.PortCnt; ii++)
                                {
                                    if (FrameInfo.DoFwdPort[ii])
                                    {
                                        FrameInfo.DoFwd = LSA_TRUE;
                                        break;
                                    }
                                }
//								if (pChSys->Fwd.PortCnt > 3)	//041109lrg001: use actual port count for trace
								#if (GSY_CH_MAX_PORTS > 3)		//010410lrg001: use configured port count for trace
								{
                                	GSY_FWD_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: DoFwd=%u to port[1...4]=[%u-%u-%u-%u] RateReady=%u",
                                                 FrameInfo.DoFwd, FrameInfo.DoFwdPort[0],FrameInfo.DoFwdPort[1],
                                                 FrameInfo.DoFwdPort[2],FrameInfo.DoFwdPort[3],TLVInfo.RateReady);
								}
								#elif (GSY_CH_MAX_PORTS > 2)
								{
                                	GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: DoFwd=%u to port[1..3]=[%u-%u-%u] RateReady=%u",
                                                 FrameInfo.DoFwd, FrameInfo.DoFwdPort[0],FrameInfo.DoFwdPort[1],
                                                 FrameInfo.DoFwdPort[2],TLVInfo.RateReady);
								}
								#elif (GSY_CH_MAX_PORTS > 1)
								{
                                	GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: DoFwd=%u to port[1,2]=[%u-%u] RateReady=%u",
                                                 FrameInfo.DoFwd, FrameInfo.DoFwdPort[0],FrameInfo.DoFwdPort[1],
                                                 TLVInfo.RateReady);
								}
								#endif

								GSY_FWD_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Received FrameType=%u: pChSys=%X Port=%u SyncId=%u SeqId=%u",
												TLVInfo.Type, pChSys, PortID, FrameInfo.MACAddrId, TLVInfo.Param.Sync.SequenceID);

                                /* 090707lrg001: Forwarding of Sync and FU to any port only if Rate is ready
                                */
                                switch (TLVInfo.Type )
                                {
                                    case GSY_TLV_TYPE_SYNC:

										/* 110110lrg001: AP00848562: Handle SyncFrame even if !DoFwd to notify SeqId
										*/
		                                if (TLVInfo.RateReady)
										{
	                                        gsy_FwdHandleSyncFrame(pChSys, PortID, &FrameInfo, &TLVInfo);
										}
                                        break;

                                    case GSY_TLV_TYPE_FOLLOWUP:

										if ((TLVInfo.RateReady) && (FrameInfo.DoFwd))
										{
											gsy_FwdHandleFUFrame(pChSys, PortID, &FrameInfo, &TLVInfo);
										}
                                        break;

                                    case GSY_TLV_TYPE_ANNOUNCE:

										/* 150910lrg002
										if (pChSys->Fwd.MCEnabled[FrameInfo.MACAddrId].FwdUserMode == GSY_FWD_MODE_INPUT)
		                                {
		                                    // Frame only for user: the user reprovides the Rbl

		                                    gsy_FwdFrameUserForward(pChSys, pRbl, &TLVInfo);
		                                    Reprovide = LSA_FALSE;
		                                }
										else if (FrameInfo.MACAddrId > 1)	150910lrg002 */
										if (FrameInfo.DoFwd)				//150910lrg002: forward announce if not done by HW
										{
											gsy_FwdHandleAnnounceFrame(pChSys, PortID, &FrameInfo, &TLVInfo);
										}

                                        break;

                                    default:
                                        /* unexpected */
									    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdRcvIndication() pChSys=%X Unexpected TLV Type=%u",
            									          pChSys, TLVInfo.Type);
                                        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);

                                        break;

                                } /* switch */

                                /* --------------------------------------------------------------*/
                                /* Check if we should forward the frame to User                  */
                                /* --------------------------------------------------------------*/

                                if ((pChSys->Fwd.MCEnabled[FrameInfo.MACAddrId].FwdUserMode == GSY_FWD_MODE_ENABLE)
								||  (pChSys->Fwd.MCEnabled[FrameInfo.MACAddrId].FwdUserMode == GSY_FWD_MODE_INPUT))		//150910lrg002
                                {
                                    /* The user gets the frame and handles the whole Requestblock
                                     * The user reprovides the Rbl!
                                    */
                                    (LSA_VOID)gsy_FwdFrameUserForward(pChSys, pRbl, &TLVInfo);
                                    Reprovide = LSA_FALSE;
                                }
                            }
                            else
                            {
                                /* invalid TLV */
                                GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Invalid TLV");
                                pChSys->Fwd.Stats.InvalidTLVCnt++;
                            }
                        }
                        else
                        {
                            GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Forwarding disabled SyncId=%d, Port=%u",FrameInfo.MACAddrId,pPort->ThisPortID);
                        }
                    }
                    else
                    {
                        GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: LineDelay not set for Port=%u!",pPort->ThisPortID);
                    }
                }
                else
                {
                    GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: Not supported SyncId=%u!",FrameInfo.MACAddrId);
                }
                break;

                /*-------------------------------------------------------------------*/
            case GSY_LOWER_RSP_OK_CANCEL:
            case GSY_LOWER_RSP_ERR_RX:
                /*-------------------------------------------------------------------*/
                /* ignore rx */
                GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"gsy_FwdRcvIndication(). Rx-Error=0x%x!",
								GSY_LOWER_RQB_RESPONSE_GET(pRbl));
                break;

                /*-------------------------------------------------------------------*/
            default:
                /*-------------------------------------------------------------------*/
                /* unexpected */
			    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdRcvIndication() pChSys=%X Unexpected lower layer error=0x%x",
						          pChSys, GSY_LOWER_RQB_RESPONSE_GET(pRbl));
                gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                break;

        } /* switch */

    }
    else
    {
        /* unexpected */
        GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdRcvIndication() Channel not open: ignoring Frame!");
        //  gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);     260508lrg001
    }


    if ( Reprovide ) //260508lrg001
    {
        /* reprovide the Rbl 101208lrg001: gsy_DataRecv()
        */
        if (ReprovideSync)
            gsy_SyncRecv(pChSys, pRbl, 0);
        else
            gsy_AnnoRecv(pChSys, pRbl, 0);
    }

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdRcvIndication() Response=0x%x", Response);
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdSendConfirmation                             +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     GSY_LOWER_RQB_PTR_TYPE  pRbl                        +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  pRbl                   : Ponter to EDD Send-RQB (EDD_SRV_SYNC_SEND     +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_PARAM                                         +*/
/*+               GSY_RSP_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates sent Sync or FollowUp frames                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdSendConfirmation(
    GSY_CH_SYS_PTR          pChSys,
    GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
    LSA_UINT16              Response;
    GSY_FWD_FCB_PTR_TYPE    pFCB;
    LSA_UINT16              PortID;
    GSY_FWD_PORT_PTR_TYPE   pPort;
    GSY_FWD_MASTER_PTR_TYPE pMaster;
    LSA_VOID        LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf)
    (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response);

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdSendConfirmation() pChSys=%X, pRbl=%X", pChSys, pRbl);

    Response = GSY_RSP_OK;

    PortID = GSY_LOWER_RQB_SEND_SYNC_PORT_GET(pRbl);

    /* Check for valid parameters */
    if ((PortID == 0 ) || (PortID > pChSys->Fwd.PortCnt ))
    {
	    GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdSendConfirmation() pChSys=%X Invalid Port=%u", pChSys, PortID);
        gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
    }

    pPort = &pChSys->Fwd.pPortMgm[PortID-1];

    /* put rbl back to queue */
    gsy_EnqueLower(&pPort->TxRQBQueue, pRbl);

    /* Get FCB asigned to send */
    pFCB = pPort->pFCBInProgress;
    pFCB->Sending = LSA_FALSE; /* sending finished */

    pPort->pFCBInProgress = LSA_NULL;

    switch ( pChSys->Fwd.Status )
    {
            /*-----------------------------------------------------------------------*/
        case GSY_FWD_CHANNEL_OPEN:
            /*-----------------------------------------------------------------------*/

            pMaster = pFCB->pMaster;

            switch ( GSY_LOWER_RQB_RESPONSE_GET(pRbl))
            {
                    /*---------------------------------------------------------------*/
                case GSY_LOWER_RSP_OK:
                    /*---------------------------------------------------------------*/

                    #ifdef GSY_FWD_CNF_TIME
                    pFCB->CnfTimeStamp = GSY_GET_TIMESTAMP(pChSys->pSys) - pFCB->SndTimeStamp;
                    pFCB->CnfTimeStamp *= pChSys->RxTxNanos;
                    if (pFCB->CnfTimeStamp > 10000000)
                        GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"gsy_FwdSendConfirmation: CnfTime=%ums! Status=%u SeqId=%u Port=%u",
                                           pFCB->CnfTimeStamp / 1000000, pFCB->Status, pFCB->SequenceID, pFCB->pPort->ThisPortID);
                    #endif
                    /* Depening on FCB State we have to do different actions */

                    switch (pFCB->Status )
                    {
                        case GSY_FWD_FCB_STS_SYNC_SEND:
                            /* Check if this Sync is still up to date by comparing to*/
                            /* actual SequenceID within Master. If not we can abort  */
                            /* here and release the FCB                              */

                            if (( pMaster->SequenceID != pFCB->SequenceID ) ||               /* outdated */
                                (0 == (pPort->FwdPortMode[pFCB->MACAddrID] & GSY_FWD_MODE_OUTPUT))) /* port not open anymore 070907lrg002 */
                            {
                                GSY_FWD_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sent SYNC (outdated): pChSys=%X pRbl=%X Port=%u SyncId=%u SeqIdMaster/FCB=%u/%u",
                                                 pChSys, pRbl, pFCB->pPort->ThisPortID, pMaster->ThisProt, pMaster->SequenceID, pFCB->SequenceID);

                                /* ReQueue FCB. No further handling */
                                gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                            }
                            else
                            {
                                /* Sync sent. We got the timestamp for FollowUp from RQB */
                                /* Save TimeStamp for later use on sending FollowUP frame*/

                                pFCB->SyncSendTimeValid = GSY_LOWER_RQB_SEND_SYNC_TS_VALID_GET(pRbl);
                                pFCB->SyncSendTimeStamp = GSY_LOWER_RQB_SEND_SYNC_TIME_GET(pRbl);
                                pFCB->LineDelayHWNS     = GSY_LOWER_RQB_SEND_SYNC_LINE_DELAY_HW_GET(pRbl) * pChSys->RxTxNanos;

                                GSY_FWD_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sent SYNC: pChSys=%X pRbl=%X Port=%u SyncId=%u TS=0x%x HwLd=%u",
                                                 pChSys, pRbl, pFCB->pPort->ThisPortID,	pMaster->ThisProt,
                                                 pFCB->SyncSendTimeStamp, pFCB->LineDelayHWNS);

                                #ifdef GSY_FWD_CNF_TIME
                                if (pFCB->CnfTimeStamp > 10000000)
                                    GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"--- gsy_FwdSendConfirmation() SndTime=%u ns!",
                                                     (pFCB->SyncSendTimeStamp-pFCB->SndTimeStamp)*pChSys->RxTxNanos);
                                #endif

                                /* Check if FollowUp still arrived. 090707lrg001 or Sync without FU */
                                /* If so we start sending FollowUp by queuing FCB for send..        */

                                if (( pMaster->Status == GSY_FWD_MASTER_STS_FWD_FU_RECEIVED )
                                    ||  (pMaster->Status == GSY_FWD_MASTER_STS_FWD_FU_GENERATED))
                                {

                                    if ( pFCB->pPort->SendEnabled ) /* only if port is free for send */
                                    {
                                        GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU already received. Queue FU to pChSys=%X Port=%u",
                                                         pChSys, pFCB->pPort->ThisPortID);

                                        /* ReQueue FCB.*/
                                        /* Note: Delay field will be updated just befor sending */
                                        gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FU_SEND);
                                    }
                                    else
                                    {
                                        GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU already received but Send (now) disabled: pChSys=%X Port=%u",
                                                         pChSys, pFCB->pPort->ThisPortID);

                                        /* ReQueue FCB. No further handling */
                                        gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                                    }
                                }
                                else
                                {
                                    GSY_FWD_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: FU not received yet. Queue FU to pChSys=%X Port=%u",
                                                     pChSys, pFCB->pPort->ThisPortID);

                                    /* we wait for FU to arrive */
                                    gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FU_WAITING);
                                }
                            }
                            break;

                        case GSY_FWD_FCB_STS_FU_SEND:

                            GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sent FU: pChSys=%X pRbl=%X Port=%u SyncId=%u",
                                             pChSys, pRbl, pFCB->pPort->ThisPortID,	pMaster->ThisProt);

                            /* FU sent. finshed. Free FCB:                           */
                            /* UnQueue FCB. No further handling                      */
                            gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);

                            break;

                        case GSY_FWD_FCB_STS_ANNOUNCE_SEND:
                            GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: Sent ANNOUNCE: pChSys=%X pRbl=%X Port=%u SyncId=%u",
                                             pChSys, pRbl, pFCB->pPort->ThisPortID,	pMaster->ThisProt);

                            /* ANNOUNCE sent. finshed. Free FCB:                     */
                            /* UnQueue FCB. No further handling                      */
                            gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);

                            break;

                        default:
                            /* unexpected state */
                            GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"gsy_FwdSendConfirmation() pChSys=%X Unexpected FCB State=%u!",
					                           pChSys, pFCB->Status);
                            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                            break;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case GSY_LOWER_RSP_OK_CANCEL:
                case GSY_LOWER_RSP_TIMEOUT:
                case GSY_LOWER_RSP_ERR_TX:
                    /*---------------------------------------------------------------*/

                    GSY_FWD_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"FWD: pChSys=%X Port=%u pRbl=%X: Tx-Error=0x%x!",
                                     pChSys, pFCB->pPort->ThisPortID, pRbl, GSY_LOWER_RQB_RESPONSE_GET(pRbl));

                    switch (pFCB->Status )
                    {
                        case GSY_FWD_FCB_STS_SYNC_SEND:
                        case GSY_FWD_FCB_STS_FU_SEND:
                        case GSY_FWD_FCB_STS_ANNOUNCE_SEND:
                            /* ReQueue FCB. No further handling */
                            gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);
                            break;
                        default:
                            /* unexpected state */
                            GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"gsy_FwdSendConfirmation() pChSys=%X Unexpected FCB State=%u!",
					                           pChSys, pFCB->Status);
                            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                            break;
                    } /* switch */

                    break;

                    /*---------------------------------------------------------------*/
                default:
                    /*---------------------------------------------------------------*/
                    /* 020309lrg001: Arts-RQ AP00791468 Not FATAL (for mirror mode)
                    */
                    GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_FwdSendConfirmation() pChSys=%X Port=%u: Send response=0x%x",
                                       pChSys, pFCB->pPort->ThisPortID, GSY_LOWER_RQB_RESPONSE_GET(pRbl));

                    /* Send error: ReQueue FCB. No further handling
                    */
                    gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);

                    //              gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                    break;

            } /* switch */

            /* NOTE: We currently dont check if the Channel the FCB belongs to   */
            /*       is currently closing. This has to be done if we forward     */
            /*       not only within one channel! not implemented                */
            /*       yet!! GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL must be   */
            /*       defined!                                                    */

            /* Triggers send for pending FCBs on port.                           */

            gsy_FwdSendTrigger(pChSys,PortID);

            /* check if we have to call Cbf because of PortforwardControl        */

            if ( pPort->CallCbfAfterSend )
            {
                Cbf = pChSys->Fwd.Cbf;
                pChSys->Fwd.Cbf         = 0;
                pPort->CallCbfAfterSend = LSA_FALSE;
                GSY_ASSERT_NULL_FCT_PTR(Cbf);
                Cbf(pChSys,GSY_RSP_OK); /* call CBF, we finished */
            }

            break;

            /*-----------------------------------------------------------------------*/
        case GSY_FWD_CHANNEL_CLOSING:
            /*-----------------------------------------------------------------------*/

            /* ReQueue FCB. No further handling */
            gsy_FwdFCBRequeue(pFCB,GSY_FWD_FCB_STS_FREE);

            /* unqueue all pending FCBs for port, because we are shutting down.  */
            gsy_FwdSendShutdown(pChSys,PortID);

            /* if all FCBs are free we continue closing the system channel ..    */

            /* Note: its important to define                                     */
            /*       GSY_CFG_ONLY_FORWARD_ON_SAME_SYSTEM_CHANNEL                 */
            /*       because we assume all FCBs queued ONLY within the own       */
            /*       channel (and not within ports of other channel! )           */

            if (( ! pChSys->Fwd.FCBInUseCnt ) && ( ! pChSys->Fwd.FCBPortCnt ))
            {
                /* start Disable all active MC addresses */
                Response = gsy_FwdDisableActiveMC(pChSys);

                if ( Response != GSY_RSP_OK_ACTIVE )
                {
                    gsy_FwdFreeChannelMgm(pChSys);
                    pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;
                    Cbf = pChSys->Fwd.Cbf;
                    pChSys->Fwd.Cbf = 0;
                    GSY_ASSERT_NULL_FCT_PTR(Cbf);
                    Cbf(pChSys,GSY_RSP_OK); /* call CBF, we finished */
                }
            }

            break;

            /*-----------------------------------------------------------------------*/
        default:
            /*-----------------------------------------------------------------------*/
            /* unexpected state
            */
            GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"gsy_FwdSendConfirmation() pChSys=%X Unexpected Channel State=%u!",
            				pChSys, pChSys->Fwd.Status);
            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
            break;
    }

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdSendConfirmation() Response=0x%x FCBInUseCnt=%u",
    					Response,pChSys->Fwd.FCBInUseCnt);
    return(Response);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdMCConfirmation                               +*/
/*+  Input/Output  :    GSY_CH_SYS_PTR          pChSys                      +*/
/*+                     GSY_LOWER_RQB_PTR_TYPE  pRbl                        +*/
/*+  Result        :    LSA_UINT16                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pChSys                 : Pointer to Systemchannel (to EDD)             +*/
/*+  pRbl                   : Pointer to EDD MC-RQB                         +*/
/*+                                                                         +*/
/*+  Result     : GSY_RSP_OK                                                +*/
/*+               GSY_RSP_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates EDD-Multicast Confirmation                      +*/
/*+                                                                         +*/
/*+               - May issus another MC Request                            +*/
/*+               - May finish Channel init / exit                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_FwdMCConfirmation(
    GSY_CH_SYS_PTR          pChSys,
    LSA_UINT16              Response)
{
    LSA_BOOL   Enable;
    LSA_VOID   LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf)
    (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response);

    GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdMCConfirmation() pChSys=%X Response=0x%x",
                          pChSys, Response);

    Enable = LSA_TRUE;

    switch (pChSys->Fwd.Status)
    {
        case GSY_FWD_CHANNEL_CLOSING:
            Enable = LSA_FALSE;
            /* fall through */
            //lint -fallthrough

        case GSY_FWD_CHANNEL_OPENING:

//             = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

            switch ( Response )
            {
                case GSY_LOWER_RSP_OK:

                    /* 280607lrg001: MAC address for sync frame without FollowUp
                     * 240907lrg001: MAC address for announce frame
                    */
                    if (GSY_FWD_SYNC_SET == pChSys->Fwd.MCSetSync)
                    {
                        pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].SyncEnabled = Enable;
                    }
/* 1126470
                    else if (GSY_FWD_ANNO_SET == pChSys->Fwd.MCSetSync)
                    {
                        pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].AnnoEnabled = Enable;
                    }
*/
                    else if (GSY_FWD_SYNCFU_SET == pChSys->Fwd.MCSetSync)
                    {
                        pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].SyncFuEnabled = Enable;
                    }
                    else
                    {
                        pChSys->Fwd.MCEnabled[pChSys->Fwd.MCSetIdx].FuEnabled = Enable;
                    }

                    /* enable / disable next MC address
                    */
                    if ( gsy_FwdMcControl(pChSys, LSA_NULL, Enable, &Response) )
                    {
                        if ( Response != GSY_RSP_OK )
                        {
                            /* currently we set a fatal error. maybe later we do some differnt error handling
                            */
							GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdMCConfirmation() pChSys=%X MC Enable=%u failed! Response=0x%x",
							                  pChSys, Enable, Response);
                            gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_MC_REQUEST, LSA_NULL, 0);
                        }
                    }
                    else /* ready. no more to do. call cbf from Channel Init/Exit */
                    {
//                        gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE); /* free lower RQB :IM: Anno channel */

                        if (0 == pChSys->Fwd.Cbf)
                        {
							GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdMCConfirmation() pChSys=%X No CBF present!",
							                  pChSys);
                            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
                        }

                        if (Enable )
                        {
                            if ( ! gsy_data.Fwd.Timer.Running )
                            {
                                gsy_TimerStart(&gsy_data.Fwd.Timer,1);
                            }

                            pChSys->Fwd.Status = GSY_FWD_CHANNEL_OPEN;

                            Cbf = pChSys->Fwd.Cbf;
                            pChSys->Fwd.Cbf = 0;

                            GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdMCConfirmation() Channel Open. Calling CBF=0x%X", Cbf);

                            Cbf(pChSys,GSY_RSP_OK); /* call CBF, we finished */

                        }
                        else
                        {
                            LSA_BOOL MustWait;

                            MustWait = LSA_FALSE;

                            if ( gsy_data.Fwd.EDDSysChannelCnt == 1 )
                            {
                                if ( gsy_TimerClean(&gsy_data.Fwd.Timer) != LSA_RET_OK )
                                {
                                    MustWait = LSA_TRUE;
                                    GSY_FWD_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdMCConfirmation() Must wait for timer");
                                    gsy_data.Fwd.pCloseChSys = pChSys;
                                }
                            }

                            if ( ! MustWait )
                            {
                                gsy_FwdFreeChannelMgm(pChSys);
                                pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;

                                Cbf = pChSys->Fwd.Cbf;
                                pChSys->Fwd.Cbf = 0;

                                GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdMCConfirmation() Channel Closed. Calling CBF=0x%X",Cbf);

                                Cbf(pChSys,GSY_RSP_OK); /* call CBF, we finished */
                            }
                        }
                    }
                    break;

                default:
					GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdMCConfirmation() pChSys=%X previous MC Enable/Disable failed! Response=0x%x",
					                  pChSys, Response);

                    /* currently we set a fatal error. maybe later we do some differnt error handling
                    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_MC_REQUEST, LSA_NULL, 0);
                    */
                    gsy_FwdFreeChannelMgm(pChSys);
                    pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;

                    Cbf = pChSys->Fwd.Cbf;
                    pChSys->Fwd.Cbf = 0;

                    GSY_FWD_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdMCConfirmation() Channel ERROR. Calling CBF=0x%X", Cbf);

                    Cbf(pChSys,GSY_RSP_ERR_LOWER_LAYER); /* call CBF, we finished */
                    break;
            }
            break;

        default:
            /* unexpected */
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdMCConfirmation() pChSys=%X Invalid Channel State=%u",
			                  pChSys, pChSys->Fwd.Status);
            gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
            break;
    } /* switch */

    GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdMCConfirmation() Response=0x%x", Response);

    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    gsy_FwdTimerEvent                                   +*/
/*+  Input/Output  :    GSY_TIMER_PTR   pTimer                              +*/
/*+  Result        :    LSA_VOID                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called on timer event (1s timer)                          +*/
/*+                                                                         +*/
/*+               Check all protocols for pending masters for FU not        +*/
/*+               arrived and abort the handling, so the master gets free   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FwdTimerEvent(
    GSY_TIMER_PTR   pTimer)
{
    LSA_VOID   LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,Cbf)
    (GSY_CH_SYS_PTR  pChSys, LSA_UINT16 Response);

    GSY_FWD_MASTER_PTR_TYPE pMaster;
    LSA_UINT32              i, j, TraceIdx = 0;
    GSY_CH_SYS_PTR          pChSys = gsy_data.Fwd.pCloseChSys;

    GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT, "  > gsy_FwdTimerEvent() pTimer=%X",
                          pTimer);

    gsy_data.Fwd.TimerCnt++;

    /* --------------------------------------------------------------------------*/
    /* if we just closing and wait for timer we finish close channel             */
    /* --------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
    {
		TraceIdx = pChSys->TraceIdx;
        if (gsy_TimerClean(&gsy_data.Fwd.Timer) == LSA_RET_OK)
        {
            gsy_FwdFreeChannelMgm(pChSys);
            pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;

            Cbf = pChSys->Fwd.Cbf;
            pChSys->Fwd.Cbf = 0;

            GSY_FWD_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdTimerEvent() Channel Closed. Calling CBF (0x%X)",Cbf);

            gsy_data.Fwd.pCloseChSys = LSA_NULL;

            Cbf(pChSys, GSY_RSP_OK); /* call CBF, we finished */
        }
        else
        {
            GSY_FWD_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_NOTE,"gsy_FwdMCConfirmation() Must wait for timer again");
        }
    }
    else
    {
        /* search all protocols */
        for ( i=0; i<GSY_FWD_MAC_ADDR_CNT; i++)
        {
            /* used protocol */
            if (! LSA_HOST_PTR_ARE_EQUAL(gsy_data.Fwd.ProtMasterMgm[i].pMasterTable,LSA_NULL))
            {
                for ( j=0; j< GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; j++)
                {
                    pMaster = &gsy_data.Fwd.ProtMasterMgm[i].pMasterTable->Master[j];
				    if (!LSA_HOST_PTR_ARE_EQUAL(pMaster->pChSys, LSA_NULL))
				    {
						TraceIdx = pMaster->pChSys->TraceIdx;
				    }

                    /* if we already received SYNC and we timed out, or fwd is finished */

                    switch ( pMaster->Status )
                    {
                            /* -----------------------------------------------------------*/
                            /* Master FCBs will be freed, if no FU arrives..              */
                            /* -----------------------------------------------------------*/
                        case GSY_FWD_MASTER_STS_FWD_SYNC_RECEIVED:
                        case GSY_FWD_MASTER_STS_FWD_ANNOUNCE_RECEIVED:

                            if ((gsy_data.Fwd.TimerCnt - pMaster->TimeoutCnt) > GSY_CFG_MASTER_FU_TIMEOUT_IN_S)
                            {
                                GSY_FWD_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: pMaster=%X (received) timed out! pChSys=%X Port=%u SeqID=%u).",
                                                 pMaster, pMaster->pChSys, pMaster->PortID, pMaster->SequenceID);

                                /* free all FCBs of this Master waiting for FU to arrive.
								 * 160512lrg: AP01357571 FwdTimer: pChSys -> pMaster->pChSys
                                */
                                gsy_FwdCancelMasterFUWaitingFCB(pMaster->pChSys, pMaster);


                                if ( pMaster->FCBCnt )
                                {
                                    GSY_FWD_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: Sync/Announce still sending. FCBs in use=%u",pMaster->FCBCnt);
                                }
                            }

                            if ( pMaster->Status != GSY_FWD_MASTER_STS_FWD_FINISHED )
                            {
                                break;
                            }

                            /* else fall through !!!! maybe we just timeout for master too
                            */
                            //lint -fallthrough

                            /* -----------------------------------------------------------*/
                            /* Master entrys will be freed after a timeout occured        */
                            /* -----------------------------------------------------------*/

                        case GSY_FWD_MASTER_STS_FWD_FINISHED:

                            if ( (gsy_data.Fwd.TimerCnt - pMaster->TimeoutCnt) > GSY_CFG_MASTER_TIMEOUT_IN_S )
                            {

                                GSY_FWD_TRACE_05(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"FWD: pMaster=%X (finished) timed out! pChSys=%X SyncId=%u Midx=%d SeqID=%u",
                                                 pMaster, pMaster->pChSys, i, j, pMaster->SequenceID);

                                pMaster->Status = GSY_FWD_MASTER_STS_IDLE;

                                /* No FCBs should be pending !
                                */
                                if (pMaster->FCBCnt)
								{
									GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_FwdTimerEvent() Timeout pMaster=%X: pending FCBCnt=%u!",
													pMaster, pMaster->FCBCnt);
                                	gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
								}
                            }
                            break;


                        default:
                            break;

                    } /* switch */
                }
            }
        }
    }

    GSY_FUNCTION_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "  < gsy_FwdTimerEvent()");
    LSA_UNUSED_ARG(pTimer);
}

/*****************************************************************************/
/*  end of file GSY_FWD.C                                                    */
/*****************************************************************************/
