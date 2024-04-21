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
/*  F i l e               &F: edds_tx.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Tx-handling functions                       */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  05.02.03    JS    Optional MACRO-Call on tx-timeout for debugging        */
/*  12.02.04    JS    added support for multiple send priorities (3)         */
/*  10.03.04    JS    added support for tasking-compiler V2.0                */
/*  05.04.04    JS    nested comments corrected                              */
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*  16.04.04    JS    always use different nrt-send prios/queues             */
/*  21.01.05    JS    added Rx-Overload support                              */
/*  31.01.05    JS    LTRC support                                           */
/*  06.07.05    JS    added multiport support                                */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  15.12.06    JS    added  EDDS_CFG_ORG_PRIO_IS_HIGHEST (undocumented yet) */
/*                    (if defined, ORG-Prio NRT will be send prior of cyclic */
/*                     frames, if not, cyclic frames have highest priority)  */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  6
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SEQ */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_OPTIMIZE_TASKING
#if defined (TOOL_CHAIN_TASKING_TRICORE) && (C_COMPILER >= TSK_V20_0)
#pragma optimize 3
#else
#pragma optimize 2
#endif
#endif

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitSts                            +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+  Result:        EDD_STS_OK              : frame sent finished           +*/
/*+                 EDD_STS_OK_NO_DATA      : no frame was sent             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks for finished transmit.  It have to be+*/
/*+               called after a transmit interrupt occured.                +*/
/*+               If a frame was transmitted, the corresponding component   +*/
/*+               (SRT,NRT etc) is called so it can do its finishing tasks  +*/
/*+               (e.g. inidication to user)                                +*/
/*+                                                                         +*/
/*+               This function should be called as long as it returns      +*/
/*+               EDD_STS_OK. After each call with EDD_STS_OK returnd,      +*/
/*+               EDDS_TransmitTrigger() should be called.                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_TransmitSts(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB)
{
    LSA_RESULT                      Status;
	EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitSts(pDDB: 0x%X)",
                           pDDB);

    pGlob =  pDDB->pGlob;

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SEND_STS(pDDB: 0x%X)",pDDB);

    if ( (Status = EDDS_LL_SEND_STS(pDDB)) != EDD_STS_OK_NO_DATA )
    {
        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND_STS(). Status: 0x%X",Status);

        /* ---------------------------------------------------------------------*/
        /* check if there is a transmission underway (should always be true)    */
        /* ---------------------------------------------------------------------*/

        if ( pGlob->pTxInfo[pGlob->TxCheckIdx].InUse )
        {

            /* -----------------------------------------------------------------*/
            /* Now call the component the sent frame was from. so the component */
            /* can finish the actions neccessary with the sending of the frame  */
            /* -----------------------------------------------------------------*/

            switch (pGlob->pTxInfo[pGlob->TxCheckIdx].TxCompID)
            {
                case EDDS_TXCOMPID_NRT_PRIO_0:
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_0,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_1:
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_1,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_2:
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_2,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_3_HSYNC_LOW:
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_3_HSYNC_LOW,Status);
                    break;
                case EDDS_TXCOMPID_NRT_PRIO_4_HSYNC_HIGH:
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_4_HSYNC_HIGH,Status);
                    break;                    
                case EDDS_TXCOMPID_NRT_PRIO_5:
                    Status = EDDS_NRTTxReady(pDDB,EDDS_NRT_TX_PRIO_5,Status);
                    break;

                case EDDS_TXCOMPID_SRT_Z_XRT:
                    if (pDDB->pGlob->IsSrtUsed)
                    {
                        Status = EDDS_RTProviderTxReadyCyclicXRT(pDDB,Status);
                    }
                    break;
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                case EDDS_TXCOMPID_SRT_Z_UDP:
                    if (pDDB->IsSrtUsed)
                    {
                        Status = EDDS_RTProviderTxReadyCyclicUDP(pDDB,Status);
                    }
                    break;
                    #endif

                default:
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
                    break;
            }


            pGlob->pTxInfo[pGlob->TxCheckIdx].InUse = LSA_FALSE;
            pGlob->TxInProgCnt--;

            /* -----------------------------------------------------------------*/
            /* advance to next entry.                                           */
            /* -----------------------------------------------------------------*/

            if ( pGlob->TxCheckIdx < (pGlob->TxBuffCnt-1) )
            {
                pGlob->TxCheckIdx++;
            }
            else
            {
                pGlob->TxCheckIdx = 0;
            }

        }
        else pGlob->Errors.TxResource++;    /* should not occur  */

        Status = EDD_STS_OK;  //lint !e838 JB 02/12/2014 on purpose

    }
    else
    {
        EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND_STS(). Status: EDD_STS_OK_NO_DATA");
    }

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitSts(), Status: 0x%X",
                           Status);

    return(Status);

}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitCheckForSend                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pPortID      +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pTxCompID    +*/
/*+                             LSA_UINT32                     MaxLength    +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB                 : Pointer to DDB                                  +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+  pPortID    : Address for LSA_UINT32 of PortID        (output)          +*/
/*+  pTxCompID  : Address for LSA_UINT32 of Component ID (input/output)     +*/
/*+             : Input:  ComponentID to start test for pending tx-requests +*/
/*+               Output: ComponentID a TxFrame was found if something pres.+*/
/*+                                                                         +*/
/*+               Typically the first call uses TXCOMPID_SRT_Z and further  +*/
/*+               calls dont change the returned value. So further calls    +*/
/*+               continue at the last component a valid frame was present. +*/
/*+                                                                         +*/
/*+  MaxLengthA : Max length of frame for NRT/aSRT-Frames                   +*/
/*+                                                                         +*/
/*+  Result     :   LSA_TRUE : Something was present for sending            +*/
/*+                 LSA_FALSE: Nothing was present for sending.             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks if a component has something to      +*/
/*+               send.                                                     +*/
/*+                                                                         +*/
/*+               This is done in an priorized order:                       +*/
/*+                                                                         +*/
/*+               Component checked:             pTxCompID:                 +*/
/*+                                                                         +*/
/*+               zyclic  SRT                    TXCOMPID_SRT_Z_XRT         +*/
/*+               zyclic  SRT                    TXCOMPID_SRT_Z_UDP         +*/
/*+               NRT Prio 3                     TXCOMPID_NRT_PRIO_3        +*/
/*+               NRT Prio 2                     TXCOMPID_NRT_PRIO_2        +*/
/*+               NRT Prio 1                     TXCOMPID_NRT_PRIO_1        +*/
/*+               NRT Prio 0                     TXCOMPID_NRT_PRIO_0        +*/
/*+                                                                         +*/
/*+               Check starts with the component (=Prio) given in          +*/
/*+               pTxCompID. On first call use TXCOMPID_SRT_Z_xxx,on further+*/
/*+               call within one sendcycle use the returned value, so      +*/
/*+               the function continues at the point of return.            +*/
/*+                                                                         +*/
/*+               e.g.:                                                     +*/
/*+                                                                         +*/
/*+               TxCompID = TXCOMPID_SRT_Z_XRT;                            +*/
/*+                                                                         +*/
/*+               while (free send resources)                               +*/
/*+               {                                                         +*/
/*+                 while (EDDS_TransmitCheckForSend(....,&TxCompID))       +*/
/*+                 {                                                       +*/
/*+                   .. send the pBuffer returned                          +*/
/*+                 }                                                       +*/
/*+               }                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 pDDB checked within EDDS_ENTER_CHECK_REQUEST
//JB 11/11/2014 pTXComp checked before calling this function - pPortID out param
//WARNING be careful when using this function, make sure not to use pDDB/pTXCompID as null ptr
static LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_TransmitCheckForSend(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pPortID,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pTxCompID)
{
    LSA_BOOL    DoSend;

    EDDS_SCHED_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitCheckForSend(pDDB: 0x%X,pTxCompID: 0x%X)",
                           pDDB,
                           pTxCompID);

    /* --------------------------------------------------------------------------*/
    /* a little speed up for cyclic SRT which is the most important part.        */
    /* --------------------------------------------------------------------------*/

    if (pDDB->pGlob->IsSrtUsed)
    {
        switch ( *pTxCompID )
        {
            case  EDDS_TXCOMPID_SRT_Z_XRT:
                if ( EDDS_RTProviderTxQueryCyclicXRT(pDDB,pBuffer,pLength) == EDD_STS_OK )
                {
                    *pPortID = EDD_PORT_ID_AUTO;
                    return(LSA_TRUE); /* dirty but a litte bit faster */
                }
                /* no break, fall through */
                //lint -fallthrough
                //JB 12/11/2014 see comment above
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            case  EDDS_TXCOMPID_SRT_Z_UDP:
                if ( EDDS_RTProviderTxQueryCyclicUDP(pDDB,pBuffer,pLength) == EDD_STS_OK )
                {
                    *pPortID   = EDD_PORT_ID_AUTO;
                    *pTxCompID = EDDS_TXCOMPID_SRT_Z_UDP;  //@fixme am2219 remove this line
                    return(LSA_TRUE); /* dirty but a litte bit faster */
                }
                /* no break, fall through */
                //lint -fallthrough
                //JB 12/11/2014 see comment above
    #endif
            default:
            {
                *pPortID = EDD_PORT_ID_AUTO;
            }
                break;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* Now we check for acyclic frame to send.                                   */
    /* --------------------------------------------------------------------------*/

    DoSend = LSA_FALSE;

    /* NOTE: Processing of Hsync prio queue 3 and 4 only with Hsync support enabled. */
    /* NOTE: Without Hsync support enabled no Hsync frames can be injected. */
    
    /* there is a limitation for NRT packets */
    if((!pDDB->pNRT->TxPacketsCycleQuota) || (pDDB->pNRT->TxPacketsCycleQuota > pDDB->pNRT->actTxPacketsCycle))
    {
        switch ( *pTxCompID )
        {
            case EDDS_TXCOMPID_SRT_Z_XRT:
            case EDDS_TXCOMPID_SRT_Z_UDP:
                /* fall through */
            case EDDS_TXCOMPID_NRT_PRIO_5:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_5,pBuffer,pLength,pPortID) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_5;
                    break;
                } /* else we fall through to next level .. */
                //lint -fallthrough
                //JB 12/11/2014 see comment above

            case EDDS_TXCOMPID_NRT_PRIO_4_HSYNC_HIGH:
                /* check if we are allowed to send NRT acylic frames ..*/
                if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ApplicationExist)
                {
                    if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_4_HSYNC_HIGH,pBuffer,pLength,pPortID) == EDD_STS_OK )
                    {
                        DoSend     = LSA_TRUE;
                        *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_4_HSYNC_HIGH;
                        break;
                    } /* else we fall through to next level .. */
                }
                //lint -fallthrough
                //JB 12/11/2014 see comment above
                
            case EDDS_TXCOMPID_NRT_PRIO_3_HSYNC_LOW:
                /* check if we are allowed to send NRT acylic frames ..*/
                if(EDD_FEATURE_ENABLE == pDDB->FeatureSupport.ApplicationExist)
                {
                    if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_3_HSYNC_LOW,pBuffer,pLength,pPortID) == EDD_STS_OK )
                    {
                        DoSend     = LSA_TRUE;
                        *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_3_HSYNC_LOW;
                        break;
                    } /* else we fall through to next level .. */
                }
                //lint -fallthrough
                //JB 12/11/2014 see comment above                
                
                /* no break, fall through */
            case EDDS_TXCOMPID_NRT_PRIO_2:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_2,pBuffer,pLength,pPortID) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_2;
                    break;
                } /* else we fall through to next level .. */
                //lint -fallthrough
                //JB 12/11/2014 see comment above

                /* no break, fall through */
            case EDDS_TXCOMPID_NRT_PRIO_1:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_1,pBuffer,pLength,pPortID) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_1;
                    break;
                } /* else we fall through to next level .. */
                //lint -fallthrough
                //JB 12/11/2014 see comment above

                /* no break, fall through */
            case EDDS_TXCOMPID_NRT_PRIO_0:
                /* check if we are allowed to send NRT acylic frames ..*/
                if  ( EDDS_NRTTxQuery(pDDB,EDDS_NRT_TX_PRIO_0,pBuffer,pLength,pPortID) == EDD_STS_OK )
                {
                    DoSend     = LSA_TRUE;
                    *pTxCompID = EDDS_TXCOMPID_NRT_PRIO_0;
                    break;
                }

                break;  /* no more acyclic resources. break at all */
            default:
                break;
        } /* switch */
        if(DoSend)
        {
            ++pDDB->pNRT->actTxPacketsCycle;
        }
    }
    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitCheckForSend(DoSend: %d)",
                           DoSend);

    return(DoSend);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitTrigger                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function checks if a new transmit can be initiated   +*/
/*+               (if a transmit buffer is free) and if so it will query    +*/
/*+               all components (NRT,SRT etc) for pending transmit request +*/
/*+               This is done in an priorized order:                       +*/
/*+                                                                         +*/
/*+               zyclic  SRT                                               +*/
/*+               azyklic SRT highprio                                      +*/
/*+               azyklic SRT lowprio                                       +*/
/*+               NRT                                                       +*/
/*+                                                                         +*/
/*+               This function should be called after a frame was transm-  +*/
/*+               mitted (after transmitbuffers become free) or  when a     +*/
/*+               component gets a new transmit-request from user.          +*/
/*+                                                                         +*/
/*+               If there are no free transmitbuffers, nothing is done.    +*/
/*+                                                                         +*/
/*+               Note: this function must not be called from isr-handler   +*/
/*+                     because there may be conflicts with reentrance      +*/
/*+                     when the function is called in request-handler too. +*/
/*+                                                                         +*/
/*+               If a new frame is send, and the timeout is not already    +*/
/*+               running, we start the timeout.                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING: be careful while using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TransmitTrigger(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_BOOL                doNrtOnly)
{

    EDD_UPPER_MEM_PTR_TYPE          pBuffer;
    LSA_UINT32                      Length;
    LSA_UINT32                      FramesToSend;
    LSA_UINT32                      TxCompID;
    LSA_RESULT                      Status;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
    LSA_UINT32                      PortID;

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitTrigger(pDDB: 0x%X)",
                           pDDB);

    FramesToSend  = 0;

    pGlob    =  pDDB->pGlob;

    /* --------------------------------------------------------------------------*/
    /* Now look for free resources and pending send requests..                   */
    /* --------------------------------------------------------------------------*/

    if ( pGlob->TxInProgCnt < pGlob->TxBuffCnt )    /* only if buffers are available */
    {
        /* -----------------------------------------------------------------------*/
        /* Query as long as free tx-resources are present and something is to send*/
        /* -----------------------------------------------------------------------*/

        if(doNrtOnly)
        {
            TxCompID = EDDS_TXCOMPID_NRT_PRIO_5; /* start with NRT prio 5 */
        }
        else
        {
            TxCompID = EDDS_TXCOMPID_SRT_Z_XRT; /* start with cyclic SRT */
        }


        while (( ! pGlob->pTxInfo[pGlob->TxFreeIdx].InUse  ) &&
               ( EDDS_TransmitCheckForSend(pDDB,&pBuffer,&Length,&PortID,&TxCompID) ))
               /* a limitation of send NRT packets per cycle is given for FILL */
        {
            /* add transfered byte regarding ethernet frame trailer and header.
             * \note    Actually, no extra check in XRT sent routines EDDS_RTProviderTxQueryCyclicXRT
             *          and EDDS_RTProviderTxQueryCyclicUDP. This is acceptable, because
             *          ES has to plan provider bandwith with 100MBit ethernet connection.
             *          The check with frame length is done only for NRT frames in EDDS_NRTTxQuery:
             *          <code>(pDDB->pGlob->actTxBytesCycle + length) <= pDDB->pGlob->TxMaxByteCntPerCycle )</code>
             */
            pGlob->actTxBytesCycle += (Length + EDDS_ETHERNET_FRAME_HEADER_TRAILER_LENGTH);

            ++FramesToSend;

            pGlob->pTxInfo[pGlob->TxFreeIdx].TxCompID = TxCompID;
            pGlob->pTxInfo[pGlob->TxFreeIdx].InUse    = LSA_TRUE;

            /* -----------------------------------------------------------------*/
            /* advance to next entry.                                           */
            /* -----------------------------------------------------------------*/

            if ( pGlob->TxFreeIdx < (pGlob->TxBuffCnt-1) )
            {
                pGlob->TxFreeIdx++;
            }
            else
            {
                pGlob->TxFreeIdx = 0;
            }

            #ifdef EDDS_CFG_DO_INTERNAL_FATAL_CHECK
            if  (( Length == 0 ) ||
                 ( LSA_HOST_PTR_ARE_EQUAL(pBuffer, LSA_NULL) ))
            {
                EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                                EDDS_MODULE_ID,
                                __LINE__);
            }
            #endif

            EDDS_LOWER_TRACE_05(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SEND(pDDB: 0x%X, TxCompID: %u, pBuffer: 0x%X, Len: %d, Port: %d)",
                                pDDB,
                                TxCompID,
                                pBuffer,
                                Length,
                                PortID);

            Status = EDDS_LL_SEND(pDDB,pBuffer,Length,PortID);

            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND(). Status: 0x%X",Status);

            /* -----------------------------------------------------------------*/
            /* we dont expect an error here. if so something goes wrong inside  */
            /* the driver which has to be fixed, so we signal an fatal error    */
            /* -----------------------------------------------------------------*/

            switch (Status)
            {
                case  EDD_STS_OK:

                    pGlob->TxInProgCnt++;

                    break;

                case  EDD_STS_ERR_RESOURCE:

                    EDDS_FatalError(EDDS_FATAL_ERR_SEND,
                                    EDDS_MODULE_ID,
                                    __LINE__); /* does not return!*/

                    break;

                case  EDD_STS_ERR_PARAM:
                default:
                    EDDS_FatalError(EDDS_FATAL_ERR_SEND,
                                    EDDS_MODULE_ID,
                                    __LINE__); /* does not return!*/

                    break;

            } /* switch */

        } /* while */

        if ( FramesToSend )
        {
            EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SEND_TRIGGER(pDDB: 0x%X) %d frames triggered; actual %d/%d byte per cycle utilized.",
                                                pDDB,
                                                FramesToSend,
                                                pGlob->actTxBytesCycle,
                                                pGlob->TxMaxByteCntPerCycle );

            if( pGlob->actTxBytesCycle > pGlob->TxMaxByteCntPerCycle )
            {

                EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"==> EDDS_LL_SEND_TRIGGER(pDDB: 0x%X) send byte limit exceeded by %d bytes.",
                                    pDDB,
                                    pGlob->actTxBytesCycle - pGlob->TxMaxByteCntPerCycle );
            }

            EDDS_LL_SEND_TRIGGER(pDDB);

            EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SEND_TRIGGER()");
        }
    }

    EDDS_SCHED_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitTrigger(Frames to send: %d)",
                           FramesToSend);


}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TransmitShutdown                       +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    -----------                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after the LLIF was shutdown. (in Shutdown-Request) +*/
/*+                                                                         +*/
/*+               We stop Tx-Timer and finish all pending tx                +*/
/*+                                                                         +*/
/*+               Note that all handles already closed, so no user waits    +*/
/*+               for this tx to be ready!                                  +*/
/*+                                                                         +*/
/*+ JB 11/11/2014 pDDB checked within EDDS_GetDDB                           +*/
/*+ WARNING: caller must have checked pDDB!                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TransmitShutdown(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB)
{
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    EDDS_SYSTEM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TransmitShutdown(pDDB: 0x%X)",
                           pDDB);

    pGlob =  pDDB->pGlob;

    /* --------------------------------------------------------------------------*/
    /* Now we are no more interrupted by high-prio context, because tx is stopped*/
    /* --------------------------------------------------------------------------*/
	while ( pGlob->pTxInfo[pGlob->TxCheckIdx].InUse  )
	{
		/* -----------------------------------------------------------------*/
		/* Now call the component the sent frame was from. so the component */
		/* can finish the actions neccessary with the sending of the frame  */
		/* Note: Currently it is not possible that there are any NRT-Request*/
		/*       pending on shutdown!                                       */
		/* -----------------------------------------------------------------*/

		switch (pGlob->pTxInfo[pGlob->TxCheckIdx].TxCompID)
		{
			case EDDS_TXCOMPID_NRT_PRIO_0:
			case EDDS_TXCOMPID_NRT_PRIO_1:
			case EDDS_TXCOMPID_NRT_PRIO_2:
			case EDDS_TXCOMPID_NRT_PRIO_3_HSYNC_LOW:
			case EDDS_TXCOMPID_NRT_PRIO_4_HSYNC_HIGH:
			case EDDS_TXCOMPID_NRT_PRIO_5:
			    EDDS_SYSTEM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
			                           "***:EDDS_TransmitShutdown() TxCheckIdx %d has already a TxCompID %d NRT Send request?!",
			                           pGlob->TxCheckIdx,pGlob->pTxInfo[pGlob->TxCheckIdx].TxCompID);
			    break;

			/* formally clean up:
			 * pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress
			 * pCSRTComp->Prov.ProviderCycle.Xrt.ActTxInProgress
			 */
			case EDDS_TXCOMPID_SRT_Z_XRT:
			    if (pDDB->pGlob->IsSrtUsed)
			    {
                    LSA_RESULT Status;
                    Status = EDDS_RTProviderTxReadyCyclicXRT(pDDB,EDD_STS_OK);
                    if(EDD_STS_OK != Status)
                    {
                        EDDS_SYSTEM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                             "EDDS_TransmitShutdown(): call to EDDS_RTProviderTxReadyCyclicXRT failed: 0x%X",
                                             Status);
                    }
			    }
				break;

				#ifdef EDDS_XRT_OVER_UDP_INCLUDE
			case EDDS_TXCOMPID_SRT_Z_UDP:
			    if (pDDB->pGlob->IsSrtUsed)
			    {
                    EDDS_RTProviderTxReadyCyclicUDP(pDDB,EDD_STS_OK);
				}
				break;
				#endif

			default:
				EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
								EDDS_MODULE_ID,
								__LINE__);
				break;
		}

		pGlob->pTxInfo[pGlob->TxCheckIdx].InUse = LSA_FALSE;

		/* -----------------------------------------------------------------*/
		/* advance to next entry.                                           */
		/* -----------------------------------------------------------------*/

		if ( pGlob->TxCheckIdx < (pGlob->TxBuffCnt-1) )
		{
			pGlob->TxCheckIdx++;
		}
		else
		{
			pGlob->TxCheckIdx = 0;
		}

	} /* while */

    EDDS_SYSTEM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TransmitTrigger()");

}


/*****************************************************************************/
/*  end of file edds_tx.c                                                    */
/*****************************************************************************/

