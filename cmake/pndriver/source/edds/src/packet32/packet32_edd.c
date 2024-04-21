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
/*  F i l e               &F: packet32_edd.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level functions for EDDS (WinPCAP/Packet32)  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  26.07.10    MA    initial version.                                       */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  17
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID  /* Ids 16..31 reserved for LL */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */  //#JB_TODO lint warning cause this file is not used, but it sets EDDS_CFG_HW_PACKET32 (windows)
#include "edds_int.h"

/*===========================================================================*/
/*                              global Variables                             */
/*===========================================================================*/

#include "packet32_inc.h"

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*===========================================================================*/
/*                           static helper functions                         */
/*===========================================================================*/


//JB 14/11/2014 helper function, handle checked where this function is called
//WARNING: be careful when using this function, make sure not to use handle as null ptr!
static LSA_RESULT packet32_set_power_state(
    PACKET32_LL_HANDLE_TYPE const * handle,
    LSA_UINT8                       PHYPower )
{
    LSA_RESULT Status = EDD_STS_OK;

    if(EDDS_PHY_POWER_UNCHANGED != PHYPower)
    {
        //  PnP and PM OIDs
        #ifndef OID_PNP_CAPABILITIES
        #define OID_PNP_CAPABILITIES             0xFD010100
        #endif
        #ifndef OID_PNP_SET_POWER
        #define OID_PNP_SET_POWER                0xFD010101
        #endif
        #ifndef OID_PNP_QUERY_POWER
        #define OID_PNP_QUERY_POWER              0xFD010102
        #endif
        #ifndef OID_PNP_ADD_WAKE_UP_PATTERN
        #define OID_PNP_ADD_WAKE_UP_PATTERN      0xFD010103
        #endif
        #ifndef OID_PNP_REMOVE_WAKE_UP_PATTERN
        #define OID_PNP_REMOVE_WAKE_UP_PATTERN   0xFD010104
        #endif
        #ifndef OID_PNP_WAKE_UP_PATTERN_LIST
        #define OID_PNP_WAKE_UP_PATTERN_LIST     0xFD010105
        #endif
        #ifndef OID_PNP_ENABLE_WAKE_UP
        #define OID_PNP_ENABLE_WAKE_UP           0xFD010106
        #endif

        struct
        {
            PACKET_OID_DATA pkt;
            LSA_UINT8 _more_data[sizeof(DEVICE_POWER_STATE)];
        } oid_buf;

        oid_buf.pkt.Oid = OID_PNP_SET_POWER;
        oid_buf.pkt.Length = sizeof(oid_buf._more_data);
        *((DEVICE_POWER_STATE*)oid_buf.pkt.Data) = (DEVICE_POWER_STATE) (EDDS_PHY_POWER_ON == PHYPower ? NdisDeviceStateD0 : NdisDeviceStateD2)
        ; //lint !e826 JB 14711/2014 pointer cast on purpose

        if (PacketRequest(handle->pAdapter, LSA_TRUE/*set*/, &oid_buf.pkt))
        {
            Status = EDD_STS_OK;
        }
        else
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "packet32_set_power_state: PacketRequest() failed");
            Status = EDD_STS_ERR_HW;
        }
    }

    return (Status);
}



//JB 14/11/2014 helper function, checked where called
//WARNING: be careful when using this function, make sure not to use handle or pResult as null ptr!
static NDIS_MEDIA_STATE EDDS_LOCAL_FCT_ATTR packet32_get_connect_status(
    PACKET32_LL_HANDLE_TYPE const * handle,
    LSA_RESULT*                     pResult )
{
    NDIS_MEDIA_STATE media_state = NdisMediaStateDisconnected;

    struct
    {
        PACKET_OID_DATA pkt;
        LSA_UINT8 _more_data[sizeof(NDIS_MEDIA_STATE)];
    } oid_buf;

    oid_buf.pkt.Oid = OID_GEN_MEDIA_CONNECT_STATUS;
    oid_buf.pkt.Length = sizeof(oid_buf._more_data);

    if (PacketRequest(handle->pAdapter, LSA_FALSE/*get*/, &oid_buf.pkt))
    {
        media_state = *((NDIS_MEDIA_STATE*)oid_buf.pkt.Data);  //lint !e826 JB 14/11/2014 pointer cast on purpose
        *pResult = EDD_STS_OK;
    }
    else
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_get_connect_status: PacketRequest() failed");
        *pResult = EDD_STS_ERR_HW;
    }

    return (media_state);

}

static EDD_RSP packet32_sendQueuePut(
    PACKET32_LL_HANDLE_TYPE* handle,
    LSA_VOID_PTR_TYPE        pBuffer,
    LSA_UINT32               length )
{
    LSA_UINT32  tempNext;
    EDD_RSP     retVal;

    tempNext    = handle->sendQueue.index_next;
    retVal      = EDD_STS_OK;

    if ((pBuffer != LSA_NULL))
    {
        //increment tempNext?
        if (tempNext == handle->sendQueue.size - 1)
        {
            //tempNext wrap
            tempNext = 0;
        }
        else
        {
            tempNext++;
        }

        //is queue full
        if (tempNext == handle->sendQueue.index_start)
        {
            //yes queue is full
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "packet32_sendQueuePut: queue is full");
            retVal = EDD_STS_ERR_RESOURCE;
        }
        else
        {
            handle->sendQueue.pElement[handle->sendQueue.index_next].pBuffer = (LSA_UINT8*) pBuffer;
            handle->sendQueue.pElement[handle->sendQueue.index_next].length  = length;
            handle->sendQueue.index_next = tempNext;
        }
    }
    else
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_sendQueuePut: invalid pointer to Buffer");
        retVal = EDD_STS_ERR_PARAM;
    }

    return retVal;
}

static EDD_RSP packet32_sendQueueGet(
    PACKET32_LL_HANDLE_TYPE* handle,
    LSA_UINT8**              ppBuffer,
    LSA_UINT32*              pLength )
{
    EDD_RSP retVal = EDD_STS_OK;

    if (handle->sendQueue.index_next != handle->sendQueue.index_start)
    {
        //queue is not empty
        *ppBuffer = (LSA_UINT8*) (handle->sendQueue.pElement[handle->sendQueue.index_start].pBuffer);
        handle->sendQueue.pElement[handle->sendQueue.index_start].pBuffer = LSA_NULL;

        *pLength = handle->sendQueue.pElement[handle->sendQueue.index_start].length;

        if (handle->sendQueue.index_start == handle->sendQueue.size - 1)
        {
            handle->sendQueue.index_start = 0;
        }
        else
        {
            handle->sendQueue.index_start++;
        }
    }
    else
    {
        //queue is empty
        retVal = EDD_STS_OK_NO_DATA;
    }

    return retVal;
}

/* helper function to get the initial phy power status and configured link speed mode */
static LSA_RESULT packet32_get_startup_link_param(
    PACKET32_LL_HANDLE_TYPE const * const handle,
    EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE       pHwParam)
{
    LSA_RESULT Status = EDD_STS_OK;

    if( 1 > pHwParam->PortCnt )
    {
        EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_get_startup_link_param: invalid PortCnt(%d)", pHwParam->PortCnt);
        Status = EDD_STS_ERR_PARAM;
    }

    pHwParam->MACAddress = handle->MACAddress;

    if (EDD_STS_OK == Status)
    {
        LSA_UINT32 i;
        for(i=0;i<EDDS_PACKET32_SUPPORTED_PORTS;++i)
        {
            pHwParam->PHYPower[i]      = EDDS_PHY_POWER_ON;
            pHwParam->LinkSpeedMode[i] = EDD_LINK_AUTONEG;
            pHwParam->IsWireless[i]    = EDD_PORT_IS_NOT_WIRELESS;
            pHwParam->MACAddressPort[i] = handle->MACAddressPort[i];
        }
    }

    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    packet32_SendThread                         +*/
/*+  Input/Output          :    LPVOID                        lpParam       +*/
/*+  Result                :    DWORD                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  lpParam               :    Pointer to LLIF specific management         +*/
/*+                             structure                                   +*/
/*+                                                                         +*/
/*+  Result EDD_STS_OK        : everything went fine                        +*/
/*+         EDD_STS_ERR_ABORT : something went totally wrong                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: thread that feeds the packet32 driver with send packets   +*/
/*+                                                                         +*/
/*+               Errors that occur during WINAPI mechanisms lead to        +*/
/*+               EDDS_FatalError                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static DWORD WINAPI packet32_SendThread( LPVOID lpParam )
{
    DWORD dwWaitResult;
    PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*) lpParam;
    EDD_UPPER_MEM_PTR_TYPE pBufferAddr = LSA_NULL;
    LSA_UINT32 Length;
    LPPACKET lpPacket;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :packet32_SendThread()");

    while(handle->thread_data.alive)
    {
        do
        {
            /* Auto-reset is used for this event. */
            dwWaitResult = WaitForSingleObject(handle->thread_data.hEvent,INFINITE);
            if(WAIT_FAILED == dwWaitResult)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "packet32_SendThread: WaitForSingleObject Event failed");
                EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                                EDDS_MODULE_ID,
                                __LINE__);
            }
        } while(WAIT_OBJECT_0 != dwWaitResult); //lint !e835 define-based behaviour of WAIT_OBJECT_0

        lpPacket = handle->sendQueue.lpPacket;

        while (EDD_STS_OK == packet32_sendQueueGet(handle, (LSA_UINT8**) &pBufferAddr, &Length))
        {
            PacketInitPacket(lpPacket, pBufferAddr, Length);

            if (LSA_FALSE != PacketSendPacket(handle->pAdapter, lpPacket, LSA_TRUE))
            {
                // lock mutex
                do
                {
                    dwWaitResult = WaitForSingleObject(handle->thread_data.hMutex,INFINITE);
                    if(WAIT_FAILED == dwWaitResult)
                    {
                        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "packet32_SendThread: WaitForSingleObject Mutex failed");
                        EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                                        EDDS_MODULE_ID,
                                        __LINE__);
                    }

                }while(WAIT_OBJECT_0 != dwWaitResult); //lint !e835 define-based behaviour of WAIT_OBJECT_0
                // do critical work
                ++handle->sendPackets;
                // release mutex
                if( !ReleaseMutex(handle->thread_data.hMutex) )
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "packet32_SendThread: Release Mutex failed");
                    EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
                }
            }

            pBufferAddr = LSA_NULL;
        }
    }

    handle->thread_data.alive = LSA_FALSE;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:packet32_SendThread()");

    return (DWORD)EDD_STS_OK;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    packet32_setup_SendThread                   +*/
/*+  Input/Output          :    PACKET32_LL_HANDLE_TYPE     * handle        +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  handle                :    Pointer to LLIF specific management         +*/
/*+                             structure                                   +*/
/*+                                                                         +*/
/*+  Result EDD_STS_OK        : everything went fine                        +*/
/*+         EDD_STS_ERR_ABORT : something went totally wrong                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: initializes mutex, event and thread handles for the       +*/
/*+               softwareDMA_Thread                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static EDD_RSP packet32_setup_SendThread(PACKET32_LL_HANDLE_TYPE* handle)
{
    EDD_RSP Response = EDD_STS_OK;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :packet32_setup_SendThread()");

    //init mutex
    handle->thread_data.hMutex = CreateMutex(
        NULL,               // default security attributes
        FALSE,              // initially not owned
        NULL);              // unnamed mutex
    if(NULL == handle->thread_data.hMutex)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_setup_SendThread: could not create mutex");
        Response = EDD_STS_ERR_ABORT;
    }

    if(EDD_STS_OK == Response)
    {
        //init event
        handle->thread_data.hEvent = CreateEvent(
            NULL,               // default security attributes
            FALSE,               // auto-reset event
            FALSE,              // initial state is nonsignaled
            NULL);              // object name

        if(NULL == handle->thread_data.hEvent)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "packet32_setup_SendThread: could not create event");
            Response = EDD_STS_ERR_ABORT;
        }

        if(EDD_STS_OK == Response)
        {
            //init thread
            handle->thread_data.alive = LSA_TRUE;
            handle->thread_data.hThread = CreateThread(
                NULL,               // default security
                0,                  // default stack size
                packet32_SendThread, // name of the thread function
                (LPVOID)handle,     // thread parameters
                0,                  // default startup flags
                &handle->thread_data.dwThreadId);

            if(NULL == handle->thread_data.hThread)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "packet32_setup_winapi: could not create thread");
                Response = EDD_STS_ERR_ABORT;
            }
        }
    }

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:packet32_setup_SendThread()");

    return Response;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    packet32_shutdown_SendThread                +*/
/*+  Input/Output          :    PACKET32_LL_HANDLE_TYPE     * handle        +*/
/*+  Result                :    void                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  handle                :    Pointer to LLIF specific management         +*/
/*+                             structure                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: terminates the softwareDMA_Thread and closes all handles  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static EDD_RSP packet32_shutdown_SendThread(PACKET32_LL_HANDLE_TYPE const * handle)
{
    EDD_RSP Response = EDD_STS_OK;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
          "IN :packet32_shutdown_SendThread()");

    // terminate thread
    /* terminating the thread is not a regular proccess. therefore it is ok to call
     * TerminateThread instead of sending an event to the thread in order to close it
    */
    if( !TerminateThread(handle->thread_data.hThread,0) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_shutdown_SendThread: could not terminate thread");
        Response = EDD_STS_ERR_HW;
    }

    //close handles
    if( !CloseHandle(handle->thread_data.hThread) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_shutdown_SendThread: could not close thread handle");
        Response = EDD_STS_ERR_HW;
    }
    if( !CloseHandle(handle->thread_data.hEvent) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_shutdown_SendThread: could not close event handle");
        Response = EDD_STS_ERR_HW;
    }
    if( !CloseHandle(handle->thread_data.hMutex) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "packet32_shutdown_SendThread: could not close mutex handle");
        Response = EDD_STS_ERR_HW;
    }

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:packet32_shutdown_SendThread()");

    return Response;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    packet32_destinationIsUnknownUCMac          +*/
/*+  Input/Output          :    PACKET32_LL_HANDLE_TYPE * handle            +*/
/*+  Input/Output          :    LSA_UINT8               * pFrameBuffer      +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  handle                :    Pointer to LLIF specific management         +*/
/*+                             structure                                   +*/
/*+  pFrameBuffer          :    Pointer to ethernet frame                   +*/
/*+                                                                         +*/
/*+  Result LSA_TRUE       : destination addr is a unknown uc               +*/
/*+         LSA_FALSE      : da is either if, port or not a uc mac          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: the function evaluates if the destination mac address     +*/
/*+                  of the assigned frame buffer is an unknown unicast     +*/
/*+                  mac address. Unknown means:                            +*/
/*+                    - not the interface mac address                      +*/
/*+                    - not the port mac address                           +*/
/*+                    - neither multicast nor broadcast mac address        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_BOOL packet32_destinationIsUnknownUCMac(
    PACKET32_LL_HANDLE_TYPE* handle,
    LSA_UINT8 const *        pFrameBuffer)
{
    LSA_BOOL isUnknownUCMac;

    isUnknownUCMac = LSA_TRUE;

    /* check only uc frames */
    if( !(pFrameBuffer[0] & 0x01) )
    {
        LSA_UINT8* pCompareMAC;

        /* check interface mac */
        pCompareMAC = handle->MACAddress.MacAdr;
        if( (pCompareMAC[5] == pFrameBuffer[5]) &&
                    (pCompareMAC[4] == pFrameBuffer[4]) &&
                    (pCompareMAC[3] == pFrameBuffer[3]) &&
                    (pCompareMAC[2] == pFrameBuffer[2]) &&
                    (pCompareMAC[1] == pFrameBuffer[1]) &&
                    (pCompareMAC[0] == pFrameBuffer[0]))
        {
            isUnknownUCMac = LSA_FALSE;
        }

        /* check port mac */
        if(isUnknownUCMac)
        {
            pCompareMAC = handle->MACAddressPort[0].MacAdr;
            if( (pCompareMAC[5] == pFrameBuffer[5]) &&
                    (pCompareMAC[4] == pFrameBuffer[4]) &&
                    (pCompareMAC[3] == pFrameBuffer[3]) &&
                    (pCompareMAC[2] == pFrameBuffer[2]) &&
                    (pCompareMAC[1] == pFrameBuffer[1]) &&
                    (pCompareMAC[0] == pFrameBuffer[0]))
            {
                isUnknownUCMac = LSA_FALSE;
            }
        }
    }
    else /* not unicast */
    {
        /* multicast & broadcast are filtered by edds */
        isUnknownUCMac = LSA_FALSE;
    }
    return (isUnknownUCMac);
}

/**
 * \brief       helper function to determine weather or not a mac address is broadcast (0xFF:0xFF:0xFF:0xFF:0xFF:0xFF)
 * 
 * \param[in]   mac     MAC address to be check
 * 
 * \return      bool'sch
 */
static LSA_BOOL packet32_IsBcstMac(EDD_MAC_ADR_TYPE mac)
{
    LSA_UINT32 i;
    LSA_BOOL ret = LSA_TRUE;
    
    for(i=0;i<EDD_MAC_ADDR_SIZE;++i)
    {
        if(mac.MacAdr[i] != 0xFF)
        {
            ret = LSA_FALSE;
            break;
        }
    }
    
    return ret;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_OPEN                            +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_HANDLE                   hDDB          +*/
/*+                             EDDS_UPPER_DPB_PTR_TYPE       pDPB          +*/
/*+                             LSA_UINT32                    TraceIdx      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  hDDB         : Handle to Device Description Block (Device-management)  +*/
/*+  pDPB         : Pointer to Device Parameter Block                       +*/
/*+  TraceIdx     : Trace index to use with this EDDS LLIF instance         +*/
/*+                                                                         +*/
/*+  Result       :         EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_ALIGN                               +*/
/*+                         others.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialize LL Ethernetcontroller using DPB parameters     +*/
/*+                                                                         +*/
/*+               See DPB documentation.                                    +*/
/*+                                                                         +*/
/*+               global:                                                   +*/
/*+                                                                         +*/
/*+               pDPB->RxBufferCnt     : number of rx-buffers/descriptors  +*/
/*+               pDPB->TxBufferCnt     : number of tx-buffers/descriptors  +*/
/*+               pDPB->RxBufferSize    : rx-buffers size                   +*/
/*+               pDPB->RxBufferOffset  : rx-buffer offset                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_OPEN(
    LSA_VOID_PTR_TYPE                 pLLManagement,
    EDDS_HANDLE				          hDDB,
    EDDS_UPPER_DPB_PTR_TO_CONST_TYPE  pDPB,
    LSA_UINT32				          TraceIdx,
    EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE  pCaps )
{
    LSA_VOID_PTR_TYPE pSendBuffer;
    LSA_VOID_PTR_TYPE pRecvBuffer;
    PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN :PACKET32_LL_OPEN()");

    LSA_UNUSED_ARG(hDDB);

    handle->pAdapter = PacketOpenAdapter(pDPB->pLLParameter);
    handle->hSysDev  = pDPB->hSysDev;

    handle->TraceIdx = TraceIdx;

    handle->RxDropCnt = 0;
    
    handle->LL_Stats.InOctets = 0;
    handle->LL_Stats.InUcastPkts = 0;
    handle->LL_Stats.InNUcastPkts = 0;
    handle->LL_Stats.OutOctets = 0;
    handle->LL_Stats.OutUcastPkts = 0;
    handle->LL_Stats.OutNUcastPkts = 0;
    handle->LL_Stats.InMulticastPkts = 0;
    handle->LL_Stats.InBroadcastPkts = 0;
    handle->LL_Stats.OutMulticastPkts = 0;
    handle->LL_Stats.OutBroadcastPkts = 0;
    handle->LL_Stats.InHCOctets = 0;
    handle->LL_Stats.InHCUcastPkts = 0;
    handle->LL_Stats.InHCMulticastPkts = 0;
    handle->LL_Stats.InHCBroadcastPkts = 0;
    handle->LL_Stats.OutHCOctets = 0;
    handle->LL_Stats.OutHCUcastPkts = 0;
    handle->LL_Stats.OutHCMulticastPkts = 0;
    handle->LL_Stats.OutHCBroadcastPkts = 0;

    if(EDD_FRAME_BUFFER_LENGTH < (EDD_IEEE_FRAME_LENGTH_WITH_CRC+EDD_VLAN_TAG_SIZE)) //lint !e506 !e774 JB 14/11/2014 define-based behaviour
    {
        EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_OPEN: invalid EDD_FRAME_BUFFER_LENGTH(%d). has to be smaller than EDD_IEEE_FRAME_LENGTH_WITH_CRC+EDD_VLAN_TAG_SIZE(%d)",
            EDD_FRAME_BUFFER_LENGTH,
            (EDD_IEEE_FRAME_LENGTH_WITH_CRC+EDD_VLAN_TAG_SIZE));
        return EDD_STS_ERR_ABORT;
    }

    if ((0 == handle->pAdapter) || (INVALID_HANDLE_VALUE == handle->pAdapter->hFile))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_OPEN: cannot open adapter");
        return EDD_STS_ERR_HW;
    }

    /* allocate send packet structure */
    /**@todo avoid copy frames from pSendBuffer to edds tx buffer */
    EDDS_ALLOC_LOCAL_FAST_MEM((&pSendBuffer),LLIF_CFG_SEND_BUFFER_SIZE);

    if (0 != pSendBuffer)
    {
        EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(handle->hSysDev,&handle->pSendBuffer,EDD_FRAME_BUFFER_LENGTH);

        if(0 != handle->pSendBuffer)
        {
            if(0 != (handle->pSendPacketStruct = PacketAllocatePacket()))
            {
                PacketInitPacket(handle->pSendPacketStruct,pSendBuffer,LLIF_CFG_SEND_BUFFER_SIZE);
            }
            else
            {
                LSA_UINT16 retval;

                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "PACKET32_LL_OPEN: cannot allocate handle->pSendPacketStruct");

                EDDS_FREE_TX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pSendBuffer);
                if(EDD_STS_OK != retval)
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "PACKET32_LL_OPEN: Could not free memory");
                }
                handle->pSendBuffer = 0;

                EDDS_FREE_LOCAL_FAST_MEM(&retval,pSendBuffer);
                if(EDD_STS_OK != retval)
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "PACKET32_LL_OPEN: Could not free memory");
                }

                return EDD_STS_ERR_RESOURCE;
            }
        }
        else
        {
            LSA_UINT16 retval;

            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_OPEN: cannot allocate handle->pSendBuffer");

            EDDS_FREE_LOCAL_FAST_MEM(&retval,pSendBuffer);
            if(EDD_STS_OK != retval)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "PACKET32_LL_OPEN: Could not free memory");
            }
            
            return EDD_STS_ERR_RESOURCE;
        }
    }
    else
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_OPEN: cannot allocate send packet structure.");
        return EDD_STS_ERR_RESOURCE;
    }

    /* allocate receive packet structure */
    /**@todo avoid copy frames from pRecvBuffer to edds tx buffer */
    EDDS_ALLOC_LOCAL_FAST_MEM((&pRecvBuffer),LLIF_CFG_RECV_BUFFER_SIZE);

    if (0 != pRecvBuffer)
    {
        EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&handle->pRecvBuffer,EDD_FRAME_BUFFER_LENGTH);

        if(0 != handle->pRecvBuffer)
        {
            if(0 != (handle->pRecvPacketStruct = PacketAllocatePacket()))
            {
                PacketInitPacket(handle->pRecvPacketStruct,pRecvBuffer,LLIF_CFG_RECV_BUFFER_SIZE);
                handle->pRecvPacket = 0;
            }
            else
            {
                LSA_UINT16 retval;

                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "PACKET32_LL_OPEN: cannot allocate handle->pRecvPacketStruct");

                EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pRecvBuffer);
                if(EDD_STS_OK != retval)
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "PACKET32_LL_OPEN: Could not free memory");
                }
                handle->pRecvBuffer = 0;

                EDDS_FREE_LOCAL_FAST_MEM(&retval,pRecvBuffer);
                if(EDD_STS_OK != retval)
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                        "PACKET32_LL_OPEN: Could not free memory");
                }

                return EDD_STS_ERR_RESOURCE;
            }
        }
        else
        {
            LSA_UINT16 retval;

            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_OPEN: cannot allocate handle->pRecvBuffer");

            EDDS_FREE_LOCAL_FAST_MEM(&retval,pRecvBuffer);
            if(EDD_STS_OK != retval)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "PACKET32_LL_OPEN: Could not free memory");
            }

            return EDD_STS_ERR_RESOURCE;
        }
    }
    else
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_OPEN: cannot allocate receive packet structure");
        return EDD_STS_ERR_RESOURCE;
    }

    /* get caps */
    pCaps->HWFeatures = 0;
    pCaps->PortCnt = EDDS_PACKET32_SUPPORTED_PORTS;
    pCaps->MaxFrameSize = 1536;

    handle->linkState         = NdisMediaStateDisconnected;
    handle->sendPackets       = 0;

    EDDS_ALLOC_LOCAL_MEM( &(handle->sendQueue.pElement), (pDPB->TxBufferCnt+1) * ( sizeof( *(handle->sendQueue.pElement) ) ) );
    if (LSA_NULL == handle->sendQueue.pElement)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_OPEN: cannot allocate handle->sendQueue.pElement");
        return EDD_STS_ERR_RESOURCE;
    }
    else
    {
        EDDS_MEMSET_LOCAL((handle->sendQueue.pElement),0,(pDPB->TxBufferCnt+1) * ( sizeof( *(handle->sendQueue.pElement) ) ));

        handle->sendQueue.index_next    = 0;
        handle->sendQueue.index_start   = 0;
        handle->sendQueue.size          = pDPB->TxBufferCnt+1;
        handle->sendQueue.lpPacket      = handle->pSendBuffer;
    }

  EDDS_LL_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT:PACKET32_LL_OPEN()");

  return (EDD_STS_OK);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SETUP                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_DSB_PTR_TYPE         pDSB        +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pDSB       : Pointer to Device Setup Block                             +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                         EDD_STS_ERR_HW                                  +*/
/*+                         others.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup LL Ethernetcontroller and start it.                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SETUP(	LSA_VOID_PTR_TYPE pLLManagement,
    EDDS_UPPER_DSB_PTR_TYPE pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam)

{
    LSA_UINT idx;
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_SETUP()");

    if (!PacketSetLoopbackBehavior(handle->pAdapter, NPF_DISABLE_LOOPBACK))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SETUP: cannot disable loopback capture.");
        return EDD_STS_ERR_HW;
    }

    if (!PacketSetHwFilter(handle->pAdapter, NDIS_PACKET_TYPE_PROMISCUOUS))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SETUP: cannot set promiscuous mode.");
        return EDD_STS_ERR_HW;
    }

    if (!PacketSetBuff(handle->pAdapter, LLIF_CFG_KERNEL_BUFFER_SIZE))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SETUP: cannot set internal adapter buffer.");
        return EDD_STS_ERR_RESOURCE;
    }

    /* do not block on read */
    if (!PacketSetReadTimeout(handle->pAdapter, INFINITE))  //lint !e569 JB 14/11/2014 max val
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SETUP: cannot set read timeout to infinite.");
        return EDD_STS_ERR_HW;
    }

    /* each send packet is not repeated on wire */
    if (!PacketSetNumWrites(handle->pAdapter, 1))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SETUP: cannot disable packet duplication.");
        return EDD_STS_ERR_HW;
    }

    /* interface mac address */
    {
        LSA_UINT32 index;

        struct
        {
            PACKET_OID_DATA pkt;
            unsigned char more[6];  //lint !e754 a OID request needs extra memory for response, this is not included in PACKET_OID_DATA
        } buf;

        buf.pkt.Oid = OID_802_3_CURRENT_ADDRESS;
        buf.pkt.Length = sizeof(buf.more); //EDD_MAC_ADDR_SIZE;
        if (PacketRequest(handle->pAdapter, LSA_FALSE/*get*/, &buf.pkt)
                && buf.pkt.Length == EDD_MAC_ADDR_SIZE)
        {
            for(index=0;index<EDD_MAC_ADDR_SIZE;++index)
            {
                handle->MACAddress.MacAdr[index]  = buf.pkt.Data[index];  //lint !e661 !e662 JB 14/11/2014 intended behaviour
            }
            if( (0 == pDSB->MACAddress[0].MacAdr[0]) && (0 == pDSB->MACAddress[0].MacAdr[1]) && (0 == pDSB->MACAddress[0].MacAdr[2]) &&
                (0 == pDSB->MACAddress[0].MacAdr[3]) && (0 == pDSB->MACAddress[0].MacAdr[4]) && (0 == pDSB->MACAddress[0].MacAdr[5]) )
            {
                pDSB->MACAddress[0] = handle->MACAddress;
            }
        }
        else
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_SETUP: PacketRequest() failed.");
            return EDD_STS_ERR_HW;
        }
    }

    /* port mac addresses */
    for(idx = 1 ; idx <= EDDS_PACKET32_SUPPORTED_PORTS; ++idx)
    {
        if( (0 != pDSB->MACAddress[idx].MacAdr[0]) || (0 != pDSB->MACAddress[idx].MacAdr[1]) || (0 != pDSB->MACAddress[idx].MacAdr[2]) ||
            (0 != pDSB->MACAddress[idx].MacAdr[3]) || (0 != pDSB->MACAddress[idx].MacAdr[4]) || (0 != pDSB->MACAddress[idx].MacAdr[5]) )
        {
            handle->MACAddressPort[idx-1] = pDSB->MACAddress[idx];
        }
        else
        {
            EDDS_MEMSET_UPPER(handle->MACAddressPort[idx-1].MacAdr,0,sizeof(handle->MACAddressPort[idx-1].MacAdr));
        }
    }

    handle->isLinkStateChanged = LSA_FALSE;
    handle->newPHYPower = EDDS_PHY_POWER_UNCHANGED;
    for(idx=0;idx < EDDS_PACKET32_SUPPORTED_PORTS; idx++)
    {
    	handle->PortStatus[idx] = EDD_PORT_PRESENT;
    }

    Status = packet32_setup_SendThread(handle);

    /* get initial phy power status and configured link speed mode */
    if(EDD_STS_OK == Status)
    {
        Status = packet32_get_startup_link_param(handle,pHwParam);
    }

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_SETUP()");

    return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SHUTDOWN                        +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Shutdown LL Ethernetcontroller. Stops it                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SHUTDOWN(LSA_VOID_PTR_TYPE pLLManagement)
{
    PACKET32_LL_HANDLE_TYPE* handle;
    LSA_RESULT Status;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_SHUTDOWN()");

    Status = packet32_shutdown_SendThread(handle);

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
          "OUT:PACKET32_LL_SHUTDOWN()");

    return Status;

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_CLOSE                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes LL Ethernetcontroller. Stops it                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement)
{
    EDD_RSP Response = EDD_STS_OK;
    LSA_UINT16 retval = EDD_STS_OK;
    LSA_VOID_PTR_TYPE pSendBuffer;
    LSA_VOID_PTR_TYPE pRecvBuffer;
    PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_CLOSE()");

    if(0 != handle->pRecvPacketStruct)
    {
        EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pRecvBuffer);
        if(EDD_STS_OK != retval)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_CLOSE: Could not free memory");
            Response = EDD_STS_ERR_RESOURCE;
        }

        pRecvBuffer = handle->pRecvPacketStruct->Buffer;
        EDDS_FREE_LOCAL_FAST_MEM(&retval, pRecvBuffer );
        if(EDD_STS_OK != retval)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_CLOSE: Could not free memory");
            Response = EDD_STS_ERR_RESOURCE;
        }

        PacketFreePacket(handle->pRecvPacketStruct);
        handle->pRecvPacketStruct = 0;
    }

    if(0 != handle->pSendPacketStruct)
    {
        EDDS_FREE_TX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pSendBuffer);
        if(EDD_STS_OK != retval)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_CLOSE: Could not free memory");
            Response = EDD_STS_ERR_RESOURCE;
        }

        pSendBuffer = handle->pSendPacketStruct->Buffer;
        EDDS_FREE_LOCAL_FAST_MEM(&retval, pSendBuffer);
        if(EDD_STS_OK != retval)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_CLOSE: Could not free memory");
            Response = EDD_STS_ERR_RESOURCE;
        }

        PacketFreePacket(handle->pSendPacketStruct);
        handle->pSendPacketStruct = 0;
    }

    PacketCloseAdapter(handle->pAdapter);
    handle->pAdapter = 0;

    EDDS_FREE_LOCAL_MEM(&retval, handle->sendQueue.pElement);
    if (EDD_STS_OK != retval)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_CLOSE: Could not free memory");
        Response = EDD_STS_ERR_RESOURCE;
    }

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
          "OUT:PACKET32_LL_CLOSE()");

    return Response;

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    PACKET32_LL_RECV                                    +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                     EDDS_LOCAL_MEM_U32_PTR_TYPE           pLength       +*/
/*+                     EDDS_LOCAL_MEM_U32_PTR_TYPE           pPortID       +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to pointer to buffer with Frame-netto-data       +*/
/*+                received if EDD_STS_OK.                                  +*/
/*+  pLength    :  Pointer to address with length of frame received.        +*/
/*+  pPortID    :  Pointer to address with PortID of frame received.        +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                EDD_STS_OK_NO_DATA                                       +*/
/*+                EDD_STS_ERR_RX                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions checks if a frame was received. If not it  +*/
/*+               returns EDD_STS_OK_NO_DATA. If a frame was received, the  +*/
/*+               functions returns pBufferAddr and pLength and sets        +*/
/*+               EDD_STS_OK or EDD_STS_ERR_RX. The buffer returned must    +*/
/*+               not be accessed anymore by the LLIF! The EDDS calls       +*/
/*+               PACKET32_LL_RECV_PROVIDE() to provide a new receive buffer+*/
/*+               to the LL. EDD_STS_ERR_RX is set, if the frame has a      +*/
/*+               error (e.g. wrong CRC). The EDDS discards this frame      +*/
/*+                                                                         +*/
/*+               Errors that occur during WINAPI mechanisms lead to        +*/
/*+               EDDS_FatalError                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV(	LSA_VOID_PTR_TYPE pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID)
{
    PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;
    LSA_RESULT Status = EDD_STS_OK;
    LSA_BOOL recv_again = LSA_FALSE;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
          "IN :PACKET32_LL_RECV()");

    *pPortID = 1; /* only one Port supported */

    do {
        recv_again = LSA_FALSE;
        
        if(0 == handle->pRecvPacket)
        {
            switch(WaitForSingleObject(PacketGetReadEvent(handle->pAdapter), 0))
            {
                case WAIT_OBJECT_0:  //lint !e835 JB 14/11/2014  define based (declared within WinBase.h)
                    Status = EDD_STS_OK;
                    break;
                case WAIT_TIMEOUT:
                    Status = EDD_STS_OK_NO_DATA;
                    break;
                default:
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_FATAL,
                        "PACKET32_LL_RECV: WaitForSingleObject failed");
                    EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                        EDDS_MODULE_ID,
                        __LINE__);
                    break;
            }
    
            if(EDD_STS_OK == Status)
            {
                if(!PacketReceivePacket(handle->pAdapter,handle->pRecvPacketStruct,LSA_TRUE))
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_FATAL,
                        "PACKET32_LL_RECV: PacketReceivePacket failed");
                    EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                        EDDS_MODULE_ID,
                        __LINE__);
                }
                else
                {
                    // move to first bpf_hdr in receive buffer
                    handle->pRecvPacket = (struct bpf_hdr*)handle->pRecvPacketStruct->Buffer;
                }
            }
        }
        else
        {
            Status = EDD_STS_OK;
        }
    
        if(EDD_STS_OK == Status)
        {
            LSA_UINT	off/*=0*/;
            LSA_UINT8*	buf = (LSA_UINT8*)handle->pRecvPacket;
    
            // get address of frame data
            off = handle->pRecvPacket->bh_hdrlen;
            *pBufferAddr = handle->pRecvBuffer;
            if(0 != *pBufferAddr)
            {
                // length of frame data
                *pLength = handle->pRecvPacket->bh_caplen;
    
                if( EDD_FRAME_BUFFER_LENGTH < *pLength )
                {
                    handle->RxDropCnt++;
                    EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_WARN,
                        "PACKET32_LL_RECV(): frame with len=%u dropped (overall=%u).",
                    *pLength, handle->RxDropCnt);
                     recv_again = LSA_TRUE;                
                }
                else
                {
                    EDDS_MEMCOPY_UPPER(*pBufferAddr,&buf[off],*pLength);
    
                    /* check if frame has an unknown destination addr */
                    if(packet32_destinationIsUnknownUCMac(handle,(LSA_UINT8*)*pBufferAddr))
                    {
                        /* packet32 llif drops all packets with unknown unicast destination mac address */ 
                        handle->RxDropCnt++;
                        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_WARN,
                            "PACKET32_LL_RECV(): frame with unknown unicast destination dropped.");
                        recv_again = LSA_TRUE;
                    }
                    else
                    {
                        /* packet32 llif handles packets with following destination mac addresses: */
                        /* interface mac */
                        /* port mac */
                        /* multicast & broadcast mac (will be filtered by edds) */
                        handle->pRecvBuffer = 0;
                    }
                }
    
                // move to next bpf_hdr in receive buffer
                off = Packet_WORDALIGN(off+handle->pRecvPacket->bh_caplen);
                handle->pRecvPacket = (struct bpf_hdr*)&buf[off];  //lint !e826 JB 14/11/2014 pointer cast on purpose
    
                // more frames in buffer ?
                if( handle->pRecvPacketStruct->ulBytesReceived <= (DWORD)(((LSA_UINT8*)handle->pRecvPacket) - (LSA_UINT8*)handle->pRecvPacketStruct->Buffer))
                {
                    handle->pRecvPacket = 0;
                }
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "PACKET32_LL_RECV: Invalid address of frame data");
                Status = EDD_STS_ERR_RESOURCE;
            }
        }
    }   while (recv_again);

    if(EDD_STS_OK == Status)
    {
        LSA_UINT8* pMac_t;
        LSA_UINT32 i;
        EDD_MAC_ADR_TYPE dstMac;
        EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT :PACKET32_LL_RECV(), Status: 0x%X, pBufferAddr: 0x%X, pLength: %lu",
            Status, *pBufferAddr, *pLength);
        /* received a good frame: handle sw statistics: */
        pMac_t = (LSA_UINT8*) *pBufferAddr;
        for(i=0;i<EDD_MAC_ADDR_SIZE;++i)
        {
            dstMac.MacAdr[i] = *pMac_t;
            ++pMac_t;
        }
        
        if(0x1 == (0x1 & dstMac.MacAdr[0]))
        {
            ++handle->LL_Stats.InNUcastPkts;
            if(packet32_IsBcstMac(dstMac))
            {
                ++handle->LL_Stats.InBroadcastPkts;
                ++handle->LL_Stats.InHCBroadcastPkts;
            }
            else
            {
                ++handle->LL_Stats.InMulticastPkts;
                ++handle->LL_Stats.InHCMulticastPkts;
            }
        }
        else
        {
            ++handle->LL_Stats.InUcastPkts;
            ++handle->LL_Stats.InHCUcastPkts;
        }
        
        handle->LL_Stats.InOctets += *pLength;
        handle->LL_Stats.InHCOctets += *pLength;
    }
    else if (EDD_STS_OK_NO_DATA == Status)
    {
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT :PACKET32_LL_RECV(), Status: 0x%X, 0x41 means EDD_STS_OK_NO_DATA",
        Status);
    }
    else
    {
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_WARN,
        "OUT:PACKET32_LL_RECV(), Status: 0x%X", Status);
    }

    return (Status);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    PACKET32_LL_RECV_PROVIDE                            +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDD_UPPER_MEM_PTR_TYPE                pBufferAddr   +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to receive buffer. The receive buffer is         +*/
/*+                EDD_FRAME_BUFFER_LENGTH in size.                         +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions provides a new receive buffer to the LL.   +*/
/*+               This function is called after the LL returned a buffer    +*/
/*+               with PACKET32_LL_RECV() to the EDDS.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV_PROVIDE(
    LSA_VOID_PTR_TYPE pLLManagement, EDD_UPPER_MEM_PTR_TYPE pBufferAddr)
{
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_RECV_PROVIDE(pBufferAddr: 0x%X)",
        pBufferAddr);

    handle->pRecvBuffer = pBufferAddr;

    Status = EDD_STS_OK;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_RECV_PROVIDE(), Status: 0x%X",
        Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    PACKET32_LL_RECV_TRIGGER                            +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDD_UPPER_MEM_PTR_TYPE        pBufferAddr           +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions (re-)triggers the EDDS hw to receive       +*/
/*+               frames. It's implemented to reduce register accesses      +*/
/*+               within the receive mechanism of the EDDS.                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+  Note:        There is no need for to implement this function for the   +*/
/*+               packet32 Lower Layer (-> empty function!)                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV_TRIGGER(
    LSA_VOID_PTR_TYPE pLLManagement)
{
    PACKET32_LL_HANDLE_TYPE* handle;
    
    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_RECV_TRIGGER()");

    LSA_UNUSED_ARG(pLLManagement); /* only for tracing issues */

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_RECV_TRIGGER()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SEND                            +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_MEM_PTR pBufferAddr              +*/
/*+                             LSA_UINT32        Length                    +*/
/*+                             LSA_UINT32        PortID                    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to buffer with Frame-netto-data to send          +*/
/*+                This address will be used and put to the controller!     +*/
/*+                (The data is not copied!) So the buffer must not be      +*/
/*+                accessed until it is released!                           +*/
/*+  Length     :  Number of Bytes pBuffer points to  (max. 1514/1518 Byte) +*/
/*+  PortID     :  PortID to send                                           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Start sending a Frame. The frame (pBufferAddr) is given   +*/
/*+               to  the low-level function for further handling. Length   +*/
/*+               is the length of the frame. The frame has to include      +*/
/*+               mac-addresses, type/len, and data. FCS is added be low-   +*/
/*+               level function (normally be controller)                   +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE pBufferAddr,
        LSA_UINT32 Length,
        LSA_UINT32 PortID)
{
    PACKET32_LL_HANDLE_TYPE* handle;
    LSA_RESULT Status;

    handle = (PACKET32_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_SEND(pBufferAddr: 0x%X, Length: 0x%X, PortID: %d)",
        pBufferAddr, Length, PortID);

    Status = packet32_sendQueuePut(handle, pBufferAddr, Length);
    
    if(EDD_STS_OK == Status)
    {
        /* sent a frame, update statistics */
        LSA_UINT8* pMAC_t;
        LSA_UINT32 i;
        EDD_MAC_ADR_TYPE dstMAC;
        
        pMAC_t = (LSA_UINT8*) pBufferAddr;
        
        for(i=0;i<EDD_MAC_ADDR_SIZE;++i)
        {
            dstMAC.MacAdr[i] = *pMAC_t;
            ++pMAC_t;
        }
        
        if(0x1 == (0x1 & dstMAC.MacAdr[0]))
        {
            ++handle->LL_Stats.OutNUcastPkts;

            if(packet32_IsBcstMac(dstMAC))
            {
                ++handle->LL_Stats.OutBroadcastPkts;
                ++handle->LL_Stats.OutHCBroadcastPkts;
            }
            else
            {
                ++handle->LL_Stats.OutMulticastPkts;
                ++handle->LL_Stats.OutHCMulticastPkts;
            }
        }
        else
        {
            ++handle->LL_Stats.OutUcastPkts;
            ++handle->LL_Stats.OutHCUcastPkts;
        }
        
        handle->LL_Stats.OutOctets += Length + 4;
        handle->LL_Stats.OutHCOctets += ((LSA_UINT64) Length + 4);
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_SEND(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(PortID);

    return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SEND_TRIGGER                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts transmission of frames applied by PACKET32_LL_SEND.+*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+               Errors that occur during WINAPI mechanisms lead to        +*/
/*+               EDDS_FatalError                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_TRIGGER(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN :PACKET32_LL_SEND_TRIGGER()");

    if (! SetEvent(handle->thread_data.hEvent) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SEND_TRIGGER: could not set event");
        EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT:PACKET32_LL_SEND_TRIGGER()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SEND_STS                        +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK           : frame successul transm.  +*/
/*+                         EDD_STS_OK_NO_DATA   : no frame transmitted     +*/
/*+                         ... transmit errors                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for finished transmissions. If a transmission was  +*/
/*+               finished, EDD_STS_OK is returned. If no (more) finished   +*/
/*+               EDD_STS_OK_NO_DATA is returned.                           +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+               Errors that occur during WINAPI mechanisms lead to        +*/
/*+               EDDS_FatalError                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_STS(LSA_VOID_PTR_TYPE pLLManagement)
{
    LSA_RESULT Status = EDD_STS_OK;
    DWORD dwWaitResult;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_SEND_STS()");

    // lock mutex
    do
    {
        dwWaitResult = WaitForSingleObject(handle->thread_data.hMutex,INFINITE);
        if(WAIT_FAILED == dwWaitResult)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_SEND_STS: WaitForSingleObject \"Mutex\" failed");
            EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                EDDS_MODULE_ID,
                __LINE__);
        }
    }while(WAIT_OBJECT_0 != dwWaitResult); //lint !e835 define-based behaviour of WAIT_OBJECT_0
    // do critical work
    if(0 != handle->sendPackets)
    {
        Status = EDD_STS_OK;
        --handle->sendPackets;
    }else
    {
        Status = EDD_STS_OK_NO_DATA;
    }
    // release mutex
    if( !ReleaseMutex(handle->thread_data.hMutex) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SEND_STS: Release Mutex failed");
        EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
            EDDS_MODULE_ID,
            __LINE__);
    }

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_SEND_STS(), Status: 0x%X",
        Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_GET_STATS                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+                             EDDS_LOCAL_STATISTICS_PTR_TYPE  pStats      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : PortID to read statistics for                             +*/
/*+               0: local interface                                        +*/
/*+               1..x: Port                                                +*/
/*+                                                                         +*/
/*+               Note: if only one port present, local interface is equal  +*/
/*+                     to the port statistic                               +*/
/*+                                                                         +*/
/*+  pStats     : Pointer to Statistics Structure to be filled              +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                           :                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets statistics from Low-Level-functions                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_STATS(LSA_VOID_PTR_TYPE pLLManagement,
    LSA_UINT32 PortID, EDDS_LOCAL_STATISTICS_PTR_TYPE pStats)
{
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_GET_STATS(PortID: 0x%X, pStats: 0x%X)",
        PortID,
        pStats);

    LSA_UNUSED_ARG(handle);
    if( 1 < PortID)
    {
        /* PortID not expected */
        EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_GET_STATS: PortID(%d) not expected", PortID);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /**@todo count send/receive bytes and other statistc data
        if(0!=PacketGetStatsEx(handle->pAdapter,&stats))
        {
          Status = EDD_STS_OK;
        }else
        {
          Status = EDD_STS_ERR_HW;
        }
        */
        pStats->SupportedCounters = PACKET32_MIB2_SUPPORTED_COUNTERS;
        pStats->InOctets = handle->LL_Stats.InOctets;
        pStats->InUcastPkts = handle->LL_Stats.InUcastPkts;
        pStats->InNUcastPkts = handle->LL_Stats.InNUcastPkts;
        pStats->InDiscards = 0;
        pStats->InErrors = 0;
        pStats->InUnknownProtos = 0;
        pStats->OutOctets = handle->LL_Stats.OutOctets;
        pStats->OutUcastPkts = handle->LL_Stats.OutUcastPkts;
        pStats->OutNUcastPkts = handle->LL_Stats.OutNUcastPkts;
        pStats->OutDiscards = 0;
        pStats->OutErrors = 0;
        pStats->OutQLen = 0;
		/* RFC2863_COUNTER */
		pStats->InMulticastPkts = handle->LL_Stats.InMulticastPkts;
        pStats->InBroadcastPkts = handle->LL_Stats.InBroadcastPkts;
        pStats->OutMulticastPkts = handle->LL_Stats.OutMulticastPkts;
        pStats->OutBroadcastPkts = handle->LL_Stats.OutBroadcastPkts;
        pStats->InHCOctets = handle->LL_Stats.InHCOctets;
        pStats->InHCUcastPkts = handle->LL_Stats.InHCUcastPkts;
        pStats->InHCMulticastPkts = handle->LL_Stats.InHCMulticastPkts;
        pStats->InHCBroadcastPkts = handle->LL_Stats.InHCBroadcastPkts;
        pStats->OutHCOctets = handle->LL_Stats.OutHCOctets;
        pStats->OutHCUcastPkts = handle->LL_Stats.OutHCUcastPkts;
        pStats->OutHCMulticastPkts = handle->LL_Stats.OutHCMulticastPkts;
        pStats->OutHCBroadcastPkts = handle->LL_Stats.OutHCBroadcastPkts;
		/* RFC2863_COUNTER - End */

        Status = EDD_STS_OK;
    }
    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_GET_STATS(), Status: 0x%X",
        Status);

    return (Status);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      : PACKET32_LL_GET_LINK_STATE                         +*/
/*+  Input/Output      : LSA_VOID_PTR_TYPE                  pLLManagement   +*/
/*+                      EDD_UPPER_GET_LINK_STATUS_PTR_TYPE   pLinkStat,    +*/
/*+                      EDDS_LOCAL_MEM_U16_PTR_TYPE  pMAUType,             +*/
/*+                      EDDS_LOCAL_MEM_U8_PTR_TYPE   pMediaType,           +*/
/*+                      EDDS_LOCAL_MEM_U8_PTR_TYPE   pIsPOF,               +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE  pPortStatus,          +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE  pAutonegCapAdvertised)+*/
/*+                                                                         +*/
/*+  Result             :    LSA_RESULT                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to get link status from  (1..x)                      +*/
/*+  pLinkStat  : Pointer to LinkStatus to be filled                        +*/
/*+                                                                         +*/
/*+  pMAUType   : Pointer to address for actual MAUType                     +*/
/*+               EDD_MAUTYPE_UNKNOWN (=0) if unknown. Else IEC MAUType     +*/
/*+                                                                         +*/
/*+  pMediaType : Pointer to address for actual MediaType                   +*/
/*+               EDD_MEDIATYPE_UNKNOWN (=0) if unknown. Else IEC MediaType +*/
/*+                                                                         +*/
/*+  pIsPOF     : is it a real POF port                                     +*/
/*+                                                                         +*/
/*+  pPortStatus: Pointer to address for actual PortStatus (see edd_usr.h)  +*/
/*+                                                                         +*/
/*+  pAutonegCapAdvertised:    Pointer to address for actual autoneg        +*/
/*+                            advertised capabilities.                     +*/
/*+                            Note: 0 if no autoneg supported.             +*/
/*+                            EDDS_AUTONEG_CAP_xxx see edd_usr.h           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+             :           EDD_STS_ERR_PARAM                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets LinkStatus from Low-Level-functions                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_LINK_STATE(
    LSA_VOID_PTR_TYPE                   pLLManagement,
    LSA_UINT32                          PortID,
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStat,
    EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pIsPOF,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pPortStatus,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pAutonegCapAdvertised,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pLinkSpeedModeConfigured)
{
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_GET_LINK_STATE(PortID: %d)",
        PortID);

    pLinkStat->Status = (NdisMediaStateDisconnected == handle->linkState) ? EDD_LINK_DOWN : EDD_LINK_UP;
    pLinkStat->Mode   = EDD_LINK_UNKNOWN;
    pLinkStat->Speed  = EDD_LINK_UNKNOWN;

    *pMAUType              = EDD_MAUTYPE_UNKNOWN;
    *pMediaType            = EDD_MEDIATYPE_UNKNOWN;
    *pIsPOF                = 0;
    *pAutonegCapAdvertised = 0; // when not supported or known set to 0

    Status = EDD_STS_OK;

    /* only one Port supported */
    if (PortID != 1)
    {
        /* PortID not expected */
        EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_GET_LINK_STATE: PortID(%d) not expected", PortID);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        pLinkStat->PortID = (LSA_UINT16)PortID; // set port id

        *pLinkSpeedModeConfigured = EDD_LINK_AUTONEG;
        *pPortStatus           = handle->PortStatus[PortID-1];

        /* only when link is UP */
        if( EDD_LINK_UP == pLinkStat->Status )
        {
            struct
            {
                PACKET_OID_DATA pkt;
                LSA_UINT8 _more_data[sizeof(NDIS_CO_LINK_SPEED)];
            } oid_buf;

            /**@todo implement NDIS requests to get real capabilities */
            oid_buf.pkt.Oid    = OID_GEN_LINK_SPEED;
            oid_buf.pkt.Length = sizeof(oid_buf._more_data);

            if( PacketRequest(handle->pAdapter, LSA_FALSE/*get*/, &oid_buf.pkt) )
            {
                NDIS_CO_LINK_SPEED* speed = (NDIS_CO_LINK_SPEED*)oid_buf.pkt.Data;  //lint !e826

                *pMediaType            = EDD_MEDIATYPE_COPPER_CABLE;
                *pIsPOF                = 0;
                *pPortStatus           = EDD_PORT_PRESENT;
                *pAutonegCapAdvertised = EDD_AUTONEG_CAP_10BASET     |
                EDD_AUTONEG_CAP_10BASETFD   |
                EDD_AUTONEG_CAP_100BASETX   |
                EDD_AUTONEG_CAP_100BASETXFD |
                EDD_AUTONEG_CAP_1000BASETFD;

                switch(speed->Outbound)
                {
                    case 100000 :
                        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
                        pLinkStat->Speed = EDD_LINK_SPEED_10;
                        *pMAUType        = EDDS_LL_MAUTYPE_10BASETFD;
                        break;
                    case 1000000 :
                        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
                        pLinkStat->Speed = EDD_LINK_SPEED_100;
                        *pMAUType        = EDDS_LL_MAUTYPE_100BASETXFD;
                        break;
                    case 10000000 :
                        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
                        pLinkStat->Speed = EDD_LINK_SPEED_1000;
                        *pMAUType        = EDDS_LL_MAUTYPE_1000BASETFD;
                        break;
                    case 100000000 :
                        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
                        pLinkStat->Speed = EDD_LINK_SPEED_10000;
                        *pMAUType        = EDDS_LL_MAUTYPE_10GBASET;
                        break;
                    default:
                        EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "PACKET32_LL_GET_LINK_STATE: value of speed->Outbound(%d) unexpected", speed->Outbound);
                        Status = EDD_STS_ERR_RESOURCE;
                }
                //Status = EDD_STS_OK;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "PACKET32_LL_GET_LINK_STATE: PacketRequest failed");
                Status = EDD_STS_ERR_HW;
            }
        }
    }

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_GET_LINK_STATE(), Status: 0x%X",
        Status);

    return (Status);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SET_LINK_STATE                  +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             LSA_UINT8                           LinkStat+*/
/*+                             LSA_UINT8                           PHYPower+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set Link for                                      +*/
/*+  pLinkStat  : LinkStatus to be setup       [IN/OUT]                     +*/
/*+               EDD_LINK_AUTONEG                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+               EDD_LINK_UNCHANGED     (do not change LinkStat)           +*/
/*+  pPHYPower  : Power of PHY                 [IN/OUT]                     +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+               EDDS_PHY_POWER_UNCHANGED(do not change power)             +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Link to given values (only if hardware is setup)      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+               The actual LinkState and PHYPower is returned.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SET_LINK_STATE(
    LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT8 * const pLinkStat,
    LSA_UINT8 * const pPHYPower)
{
    LSA_RESULT Status;
    LSA_UINT8 LinkStat;
    LSA_UINT8 PHYPower;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;
    LinkStat = *pLinkStat;
    PHYPower = * pPHYPower;

    EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, Power: 0x%X)",
        PortID,
        LinkStat,
        PHYPower);
    Status = EDD_STS_OK;

    /* check MAUTYPE */
    switch (LinkStat)
    {
        case EDD_LINK_AUTONEG:

            // set phy power (do nothing for EDDS_PHY_POWER_UNCHANGED)
            handle->newPHYPower = PHYPower;
            handle->isLinkStateChanged = LSA_TRUE;
            
            /* write back values */
            *pLinkStat = LinkStat;
            *pPHYPower = PHYPower;
            break;

        default:
            EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_SET_LINK_STATE: LinkStat is wrong, LinkStat: 0x%X",
                LinkStat);
            Status = EDD_STS_ERR_PARAM;
            break;
    }

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_SET_LINK_STATE(), Status: 0x%X",
        Status);

    LSA_UNUSED_ARG(PortID);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_MC_ENABLE                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Enable              +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE      : no more free MC entrys        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Enables Multicastaddress                                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_ENABLE(LSA_VOID_PTR_TYPE pLLManagement,
    EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_MC_ENABLE(pMCAddr: 0x%X)",
        pMCAddr);

    LSA_UNUSED_ARG(pMCAddr);

    LSA_UNUSED_ARG(handle);
    /**@todo implement with Packet32 filters */
    Status = EDD_STS_OK;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_MC_ENABLE(), Status: 0x%X",
        Status);

    return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_MC_DISABLE                      +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                        DisableAll  +*/
/*+                             EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  DisableAll  :LSA_TRUE: frees all Multicast addresses. pMCAddr          +*/
/*+                         not used.                                       +*/
/*+               LSA_FALSE:free pMACAddr only                              +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Disable if          +*/
/*+               DisableAll = LSA_FALSE                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Disables Multicastaddress                                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_DISABLE(LSA_VOID_PTR_TYPE pLLManagement,
    LSA_BOOL DisableAll, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_MC_DISABLE(DisableAll: 0x%X, pMCAddr: 0x%X)",
        DisableAll,
        pMCAddr);

    LSA_UNUSED_ARG(handle);
    LSA_UNUSED_ARG(DisableAll);
    LSA_UNUSED_ARG(pMCAddr);

    /**@todo implement with Packet32 filters */
    Status = EDD_STS_OK;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_MC_DISABLE(), Status: 0x%X",
        Status);

    return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_LED_BACKUP_MODE                 +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Backup "Show Location" LED(s) context in internal         +*/
/*+               management structure.                                     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_BACKUP_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_LED_BACKUP_MODE()");

    LSA_UNUSED_ARG(handle);

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_LED_BACKUP_MODE()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_LED_RESTORE_MODE                +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: RESTORE "Show Location" LED(s) context from internal      +*/
/*+               management structure.                                     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_RESTORE_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_LED_RESTORE_MODE()");

    LSA_UNUSED_ARG(handle);

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_LED_RESTORE_MODE()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_LED_SET_MODE                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                      LEDOn         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  LEDOn      : Mode of LED to set (LSA_TRUE/LSA_FALSE)                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets "Show Location" LED(s)	ON or OFF 										+*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_SET_MODE(
    LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn)
{
    PACKET32_LL_HANDLE_TYPE* handle;

    handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_LED_SET_MODE(%d)",
        LEDOn);

    LSA_UNUSED_ARG(handle);
    LSA_UNUSED_ARG(LEDOn);

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_LED_SET_MODE()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SWITCH_CHANGE_PORT              +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT16                    PortID        +*/
/*+                             LSA_UINT8                     isPulled      +*/
/*+  Result                :    LSA_UINT32                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement : Pointer to LLIF specific management structure          +*/
/*+  PortID        : Port to set state for                                  +*/
/*+  isPulled      : If EDD_PORTMODULE_IS_PULLED set state to pulled        +*/
/*+                  If EDD_PORTMODULE_IS_PLUGGED set state to plugged      +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_CHANGE_PORT(
    LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT16 PortID, LSA_UINT8 isPulled)
{
    PACKET32_LL_HANDLE_TYPE * handle;
    LSA_RESULT Status;
    
    handle = (PACKET32_LL_HANDLE_TYPE*) pLLManagement;
    Status = EDD_STS_OK;

    EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_SWITCH_CHANGE_PORT_STATE() isPulled(%d) for PortID(%d)", isPulled, PortID);

    if(PortID < 1 || PortID > EDDS_PACKET32_SUPPORTED_PORTS)
    {
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "PACKET32_LL_SWITCH_CHANGE_PORT_STATE: invalid PortID(%d)", PortID);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        if(EDD_PORTMODULE_IS_PULLED == isPulled)
        {
            handle->PortStatus[PortID-1] = EDD_PORT_NOT_PRESENT;
        }
        else
        {
            handle->PortStatus[PortID-1] = EDD_PORT_PRESENT;
        }
    }
    
    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_SWITCH_CHANGE_PORT_STATE()");
    
    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_RECURRING_TASK                  +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                      hwTimeSlicing +*/
/*+  Result                :    LSA_UINT32                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+  Result     : EDDS_LL_RECURRING_TASK_NOTHING or                         +*/
/*+               different EDDS_LL_RECURRING_TASK_* values as a bitmap     +*/
/*+                                                                         +*/
/*+  pLLManagement : Pointer to LLIF specific management structure          +*/
/*+  hwTimeSclicing: If LSA_TRUE, LLIF should timeslice hardware accesses   +*/
/*+                  in this function (function is called cyclically, with  +*/
/*+                  a critical high frequency).                            +*/
/*+                  If LSA_FALSE, all recurring task hw accesses should be +*/
/*+                  done in one action (function is called cyclically,     +*/
/*+                  but with a lower, much less critically frequency).     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Recurring tasks of the LLIF can be processed in this      +*/
/*+               function. It returns a resultBitmap to give EDDS a hint   +*/
/*+               what was done (e.g. update of statistic counter, link has +*/
/*+               changed or a chip reset in progress).                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT32 EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECURRING_TASK(
    LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL hwTimeSlicing)
{
    NDIS_MEDIA_STATE linkState;
    LSA_RESULT Status;
    PACKET32_LL_HANDLE_TYPE* handle;
    LSA_UINT32 resultBitmap = EDDS_LL_RECURRING_TASK_NOTHING;

    handle = (PACKET32_LL_HANDLE_TYPE*) pLLManagement;

    LSA_UNUSED_ARG(hwTimeSlicing);

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "IN :PACKET32_LL_RECURRING_TASK(): handle=0x%X", handle);

    /* write before read */
    /* linkstat */
    if(handle->isLinkStateChanged)
    {
        Status = packet32_set_power_state(handle, handle->newPHYPower);
        handle->isLinkStateChanged = LSA_FALSE;

        if(EDD_STS_OK != Status)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                "PACKET32_LL_RECURRING_TASK(): error setting link!");
        }
    }

    linkState = packet32_get_connect_status(handle, &Status);

    if (EDD_STS_OK == Status)
    {
        /* only signal a link change to EDDS, if link has changed */
        if(handle->linkState != linkState)
        {
            resultBitmap |= EDDS_LL_RECURRING_TASK_LINK_CHANGED;
            handle->linkState = linkState;
        }
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "OUT:PACKET32_LL_RECURRING_TASK(): resultBitmap == %08X",resultBitmap);

    return resultBitmap;
}


/*****************************************************************************/
/*  end of file packet32_edd.c                                               */
/*****************************************************************************/

