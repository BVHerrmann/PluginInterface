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
/*  F i l e               &F: edds_srt_usr.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDS.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  144
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SRT_USR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

#include "edds_iobuf_int.h"     /* IOBuffer internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                local constants                            */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#if (EDDS_CFG_TRACE_MODE != 0)
static LSA_VOID  EDDS_LOCAL_FCT_ATTR  EDDS_RTTraceRequest(EDD_UPPER_RQB_PTR_TYPE 	pRQB,
                                                    EDDS_LOCAL_HDB_PTR_TYPE pHDB);
#endif /* if (EDDS_CFG_TRACE_MODE != 0) */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotElementGet                       +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets a free KnotElement Index. Returns SRT_CHAIN_IDX_END  +*/
/*+               if no free element present.                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
static LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTKnotElementGet(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp) {

    LSA_UINT16 Idx;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            ">>> EDDS_RTKnotElementGet(pCSRTComp: 0x%X)", pCSRTComp);

    Idx = pCSRTComp->Knot.KnotElementFree.BottomIdx;

    if (Idx != EDDS_SRT_CHAIN_IDX_END) {

        pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[Idx];

        EDDS_ASSERT(!pKnotElement->InUse);

        /*------------------------------------------------------------- */
        /* Remove Knotelement at bottom from free-chain list            */
        /*------------------------------------------------------------- */

        /* if this was the last free element. set Idx to END-tag       */
        if (pKnotElement->UsedFreeLink.Next == EDDS_SRT_CHAIN_IDX_END) {
            pCSRTComp->Knot.KnotElementFree.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Knot.KnotElementFree.TopIdx = EDDS_SRT_CHAIN_IDX_END;
        } else {
            /* not the last element */
            pCSRTComp->Knot.KnotElementFree.BottomIdx =
                    pKnotElement->UsedFreeLink.Next;
            pCSRTComp->Knot.pKnotElementTbl[pKnotElement->UsedFreeLink.Next].UsedFreeLink.Prev =
                    EDDS_SRT_CHAIN_IDX_END;
        }

        /*------------------------------------------------------------- */
        /* Put Element to used-chain list (end)                         */
        /*------------------------------------------------------------- */

        /* nothing in queue ? */
        if (pCSRTComp->Knot.KnotElementUsed.TopIdx == EDDS_SRT_CHAIN_IDX_END) {
            pCSRTComp->Knot.KnotElementUsed.BottomIdx = Idx;
            pCSRTComp->Knot.KnotElementUsed.TopIdx = Idx;
            pKnotElement->UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
            pKnotElement->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        } else {
            pKnotElement->UsedFreeLink.Prev =
                    pCSRTComp->Knot.KnotElementUsed.TopIdx;
            pKnotElement->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.KnotElementUsed.TopIdx].UsedFreeLink.Next =
                    Idx;
            pCSRTComp->Knot.KnotElementUsed.TopIdx = Idx;
        }

        pKnotElement->ProviderSendListXRT.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
        pKnotElement->ProviderSendListXRT.TopIdx = EDDS_SRT_CHAIN_IDX_END;
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
        pKnotElement->ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
        pKnotElement->ProviderSendListUDP.TopIdx = EDDS_SRT_CHAIN_IDX_END;
#endif
        pKnotElement->ConsumerCheckList.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
        pKnotElement->ConsumerCheckList.TopIdx = EDDS_SRT_CHAIN_IDX_END;
        pKnotElement->ConsumerCnt = 0;
        pKnotElement->ProviderCnt = 0;
        pKnotElement->ProviderByteCnt = 0;
        pKnotElement->InUse = LSA_TRUE;
    }

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "<<< EDDS_RTKnotElementGet(), Idx: %d", Idx);

    return (Idx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotElementFree                      +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT16                  Index           +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees a KnotElement if in use. Dont checks if consumer    +*/
/*+               provider still using it (must be checked outside!)        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTKnotElementFree(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT16 Index) {

    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
    LSA_UINT16 Prev;
    LSA_UINT16 Next;

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTKnotElementFree(pCSRTComp: 0x%X, Idx: %d)", pCSRTComp,
            Index);
    pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[Index];

    if (pKnotElement->InUse) {
        pKnotElement->InUse = LSA_FALSE;

        /*---------------------------------------------------------------------------*/
        /* Now we can remove  the element  from the InUse-chain.                     */
        /*---------------------------------------------------------------------------*/

        Prev = pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Prev;
        Next = pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Next;

        if ((Prev == EDDS_SRT_CHAIN_IDX_END)
                || (Next == EDDS_SRT_CHAIN_IDX_END))

                {
            if (Prev == EDDS_SRT_CHAIN_IDX_END) {
                if (Next == EDDS_SRT_CHAIN_IDX_END) {
                    /* only one in list */
                    pCSRTComp->Knot.KnotElementUsed.BottomIdx =
                            EDDS_SRT_CHAIN_IDX_END;
                    pCSRTComp->Knot.KnotElementUsed.TopIdx =
                            EDDS_SRT_CHAIN_IDX_END;
                } else {
                    /* first one in list, but not only one */
                    pCSRTComp->Knot.KnotElementUsed.BottomIdx = Next;
                    pCSRTComp->Knot.pKnotElementTbl[Next].UsedFreeLink.Prev =
                            Prev;
                }
            } else {
                /* last one in list but not only one */
                pCSRTComp->Knot.pKnotElementTbl[Prev].UsedFreeLink.Next = Next;
                pCSRTComp->Knot.KnotElementUsed.TopIdx = Prev;
            }
        } else {
            /* in middle of list */
            pCSRTComp->Knot.pKnotElementTbl[Prev].UsedFreeLink.Next = Next;
            pCSRTComp->Knot.pKnotElementTbl[Next].UsedFreeLink.Prev = Prev;
        }

        /*-----------------------------------------------------------------------*/
        /* Put this element to the end of the free-chain..                       */
        /*-----------------------------------------------------------------------*/

        if (pCSRTComp->Knot.KnotElementFree.TopIdx == EDDS_SRT_CHAIN_IDX_END) /* nothing in chain ? */
        {
            pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Prev =
                    EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Next =
                    EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Knot.KnotElementFree.BottomIdx = Index;
            pCSRTComp->Knot.KnotElementFree.TopIdx = Index;
        } else {
            pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Prev =
                    pCSRTComp->Knot.KnotElementFree.TopIdx;
            pCSRTComp->Knot.pKnotElementTbl[Index].UsedFreeLink.Next =
                    EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.KnotElementFree.TopIdx].UsedFreeLink.Next =
                    Index;
            pCSRTComp->Knot.KnotElementFree.TopIdx = Index;
        }

        pKnotElement->InUse = LSA_FALSE;

    }

    EDDS_CRT_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTKnotElementFree()");

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotFree                             +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                             EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_RESOURCE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Free a Knot Element and removes it from KnotTable if no   +*/
/*+               consumer/provider usess it anymore.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTKnotFree(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT32 KnotTableIdx) {

    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
    LSA_UINT16 Idx;

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTKnotFree(pCSRTComp: 0x%X, KTIdx: %d)", pCSRTComp,
            KnotTableIdx);
    EDDS_ASSERT(KnotTableIdx < pCSRTComp->Knot.KnotCnt);

    Idx = pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx;

    if (Idx != EDDS_SRT_IDX_NOT_USED) /* knot table index used ?*/
    {
        pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[Idx];

        if ((pKnotElement->InUse) && (!pKnotElement->ConsumerCnt)
                && (!pKnotElement->ProviderCnt)) {
            /* first set to unused, then we free it                  */
            /* so we have no problem with interrupting this sequence */

            pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx =
                    EDDS_SRT_IDX_NOT_USED;
            EDDS_RTKnotElementFree(pCSRTComp, Idx);
        }
    }

    EDDS_CRT_TRACE_00(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTKnotElementGet()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    EDDS_RTKnotGet                              +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                             EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_RESOURCE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Fills a KnotTableIdx within pKnotTable with a KnotElement +*/
/*+               If none is free a error is returned.                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when callling this function, make sure not to use pCSRTComp as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTKnotGet(
        EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        LSA_UINT32 KnotTableIdx) {

    LSA_UINT32 Idx;
    LSA_RESULT Status;

    EDDS_CRT_TRACE_02(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTKnotGet(pCSRTComp: 0x%X, KTIdx: %d)", pCSRTComp,
            KnotTableIdx);

    Status = EDD_STS_OK;
    EDDS_ASSERT(KnotTableIdx < pCSRTComp->Knot.KnotCnt);

    /* Not already a Knot element present?*/
    if (pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx
            == EDDS_SRT_IDX_NOT_USED) {
        /* get a free Knot element. We should always because we */
        /* allocated enough in startup.                         */
        Idx = EDDS_RTKnotElementGet(pCSRTComp);
        if (Idx == EDDS_SRT_CHAIN_IDX_END) {
            Status = EDD_STS_ERR_RESOURCE;
            pCSRTComp->Stats.NoMoreKnotElements++;
        } else {
            pCSRTComp->Knot.pKnotTbl[KnotTableIdx].KnotElementIdx =
                    (LSA_UINT16) Idx;
        }
    }

    EDDS_CRT_TRACE_01(EDDS_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTKnotElementGet(), Status: 0x%X", Status);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    EDDS_RTGetAndCheckReductionFactor                 +*/
/*+  Input/Output    :    LSA_UINT16                      ReductionRatio    +*/
/*+                       LSA_BOOL                        Udp               +*/
/*+  Result          :    LSA_UINT16                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ReductionRatio: Provider or Consumer ReductionRatio (1,2,4,8,16..128.. +*/
/*+  Udp           : LSA_TRUE: xRtoverUDP, LSA_FALSE: Xrt                   +*/
/*+                                                                         +*/
/*+  Result:      ReductionFactor.                                          +*/
/*+               0: invalid ReductionRatio                                 +*/
/*+               1..x                                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates the ReductionFactor from the ReductionRatio    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTGetAndCheckReductionFactor(
LSA_UINT16 ReductionRatio,
LSA_BOOL Udp) {

    LSA_UINT16 ReductionFactor;
    LSA_UINT16 Help;
    LSA_UINT32 Min, Max;

    if (Udp) {
        Min = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN;
        Max = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX;
    } else {
        Min = EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN;
        Max = EDD_CSRT_CYCLE_REDUCTION_RATIO_MAX;
    }

    ReductionFactor = 0;

    if ((ReductionRatio >= Min) && (ReductionRatio <= Max)) {
        Help = ReductionRatio;
        ReductionFactor = 1;

        while (!(Help & 1)) /* till first bit is on bit position 0 */
        {
            ReductionFactor++;
            Help >>= 1;
        }

        if (Help != 1)
            ReductionFactor = 0; /* not a valid Ratio */
    }

    return (ReductionFactor);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    EDDS_RTAllocRxFrameBuffer                         +*/
/*+  Input/Output    :    EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                       LSA_UINT32                      BufferSize        +*/
/*+                       LSA_UINT32                      BufferOffset      +*/
/*+  Result          :    LSA_RESULT                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBufferAddr  : Pointer to address for Framebufferpointer               +*/
/*+  BufferSize   : Size of one Receive-buffer                              +*/
/*+  BufferOffset : Offset in Framebuffer where frames will be stored       +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates a RxFramebuffer and checks for alignment.       +*/
/*+               (the buffers match the rx-buffer requirements)            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked before calling this function
//WARNING: be careful when calling this function, make sure not to use pBufferAddr as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTAllocRxFrameBuffer(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBufferAddr)
{

    EDD_UPPER_MEM_PTR_TYPE pBuffer;
    LSA_RESULT Status;
    LSA_UINT16 RetVal;

    Status = EDD_STS_OK;

    EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer,EDD_FRAME_BUFFER_LENGTH);

    if (LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
    {
        Status = EDD_STS_ERR_RESOURCE;
        *pBufferAddr = LSA_NULL;
    }
    else
    {
        if ( (LSA_UINT32)((pBuffer)) & EDDS_RX_FRAME_BUFFER_ALIGN ) /* alignment check of virtual address */
        {
            Status = EDD_STS_ERR_ALIGN;
            EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,pBuffer); //lint !e774 JB 09/12/2014 system adaption
            EDDS_FREE_CHECK(RetVal);
        }
        else
        {
            *pBufferAddr = ((EDD_UPPER_MEM_U8_PTR_TYPE)pBuffer);
        }
    }

    return(Status);

}
//lint +esym(715,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    EDDS_RTAllocTxFrameBuffer                         +*/
/*+  Input/Output    :    EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                       LSA_UINT32                      BufferSize        +*/
/*+  Result          :    LSA_RESULT                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBufferAddr  : Pointer to address for Framebufferpointer               +*/
/*+  BufferSize   : Size of one Transmit-buffer                             +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates a TxFramebuffer (DWORD-Aligned)                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pBufferAddr as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTAllocTxFrameBuffer(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * pBufferAddr,
LSA_UINT32 BufferSize)
{

    EDD_UPPER_MEM_PTR_TYPE pBuffer;
    LSA_RESULT Status;
    LSA_UINT16 RetVal;

    Status = EDD_STS_OK;

    EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer,(LSA_UINT16) BufferSize);

    if (LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
    {
        Status = EDD_STS_ERR_RESOURCE;
    }
    else
    {
        if ( (LSA_UINT32)((pBuffer)) & EDDS_TX_SRT_FRAME_BUFFER_ALIGN ) /* alignment check of virtual address */
        {
            Status = EDD_STS_ERR_ALIGN;
            EDDS_FREE_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,pBuffer);  //lint !e774 JB 09/12/2014 system adaption
            EDDS_FREE_CHECK(RetVal);
        }
        else
        {
             *pBufferAddr = pBuffer;
        }

    }

    return(Status);

}
//lint +esym(715,pDDB)



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname    :    EDDS_RTFreeTxFrameBuffer                          +*/
/*+  Input/Output    :    EDD_UPPER_MEM_PTR_TYPE       *pBuffer             +*/
/*+  Result          :    LSA_VOID                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuffer      : Pointer to Framebuffer to free                          +*/
/*+  RxBufferOffset Offset in Framebuffer for adjustment.                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees a framebuffer. Checks for LSA_NULL                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTFreeTxFrameBuffer(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB,
        EDD_UPPER_MEM_PTR_TYPE  pBuffer)
{
    EDD_UPPER_MEM_U8_PTR_TYPE pDummy;
    LSA_UINT16 RetVal;

    if (! LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
    {
        /* calculate the pointer used when allocated (- BufferOffset!)   */

        pDummy = ((EDD_UPPER_MEM_U8_PTR_TYPE)(pBuffer) );

        EDDS_FREE_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,(EDD_UPPER_MEM_PTR_TYPE)pDummy); //lint !e774 !e715 JB 09/12/2014 system adaption
        EDDS_FREE_CHECK(RetVal);
    }

}
//lint +esym(715,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTFreeCSRTManagement                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DeviceDescriptionBlock                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function frees all memory                            +*/
/*+               It checks for already freed parts in CSRT (LSA_NULL)      +*/
/*+               free errors will be ignored or cause an fatal error       +*/
/*+               (depends on CFG-Variable)                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTFreeCSRTManagement(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    LSA_UINT16 RetVal = LSA_RET_OK;
    LSA_UINT32 i;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    LSA_UINT32 TraceIdx;


    TraceIdx = pDDB->pGlob->TraceIdx;


    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTFreeCSRTManagement(pDDB: 0x%X)", pDDB);

    pCSRTComp = pDDB->pSRT;

    /*----------------------------------------------------------------------------*/
    /* Free Provider management part                                              */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvTbl, LSA_NULL)) {
        EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Prov.pProvTbl);
        EDDS_FREE_CHECK(RetVal);

        pCSRTComp->Prov.pProvTbl = LSA_NULL;
    }

    /*----------------------------------------------------------------------------*/
    /* Free provider cr management part                                           */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvCrTbl, LSA_NULL)) {
        /* free provider-frame buffers. LSA_NULL is checked ! */

        for (i = 0; i < pCSRTComp->Cfg.ProviderCnt; i++) {
            EDDS_RTFreeTxFrameBuffer(pDDB,
                    (EDD_UPPER_MEM_PTR_TYPE) pCSRTComp->Prov.pProvCrTbl[i].pBuffer[0]);
            pCSRTComp->Prov.pProvCrTbl[i].pBuffer[0] = LSA_NULL;

            EDDS_RTFreeTxFrameBuffer(pDDB,
                    (EDD_UPPER_MEM_PTR_TYPE) pCSRTComp->Prov.pProvCrTbl[i].pBuffer[1]);
            pCSRTComp->Prov.pProvCrTbl[i].pBuffer[1] = LSA_NULL;

            EDDS_RTFreeTxFrameBuffer(pDDB,
                    (EDD_UPPER_MEM_PTR_TYPE) pCSRTComp->Prov.pProvCrTbl[i].pBuffer[2]);
            pCSRTComp->Prov.pProvCrTbl[i].pBuffer[2] = LSA_NULL;
        }
    }

    /*----------------------------------------------------------------------------*/
    /* Free provider group table                                                  */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvGroupTbl, LSA_NULL)) {
        EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Prov.pProvGroupTbl);
        EDDS_FREE_CHECK(RetVal);

        pCSRTComp->Prov.pProvGroupTbl = LSA_NULL;
    }

    /*----------------------------------------------------------------------------*/
    /* Free consumer management part (part one)                                   */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsTbl, LSA_NULL)) {
        EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Cons.pConsTbl);
        EDDS_FREE_CHECK(RetVal);

        pCSRTComp->Cons.pConsTbl = LSA_NULL;
        pCSRTComp->Cons.pConsTbl2 = LSA_NULL;
    }

    /*----------------------------------------------------------------------------*/
    /* Free consumer management part (part two)                                   */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsFrameTbl, LSA_NULL)) {
        EDDS_FREE_LOCAL_FAST_MEM(&RetVal, pCSRTComp->Cons.pConsFrameTbl);
        EDDS_FREE_CHECK(RetVal);

        pCSRTComp->Cons.pConsFrameTbl = LSA_NULL;
        pCSRTComp->Cons.pConsFrameTbl2 = LSA_NULL;
    }

    /*----------------------------------------------------------------------------*/
    /* Frees consumer cr management part                                          */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsCrTbl, LSA_NULL)) {
        /* free provider-frame buffers. LSA_NULL is checked ! */

        for (i = 0; i < pCSRTComp->Cons.ConsumerCnt; i++) {
            LSA_UINT8 idx;

            // NOTE:
            // Buffers of the internal EDDS receive resources and IO buffers (2 buffers allocated
            // in EDDS_RTInitCSRTManagement) are exchanged among each other. In other words: Buffers
            // in pBuffer can be IO buffers and/or buffers from the internal receive rescources
            // of the EDDS. The buffer in pBuffer that is currently used as receive buffer is
            // always NULL. Only 1 buffer in pBuffer is used as receive resource.
            // Because all buffers from the internal receive resource and IO buffers are allocated
            // from the same memory it doesnt matter who free's the buffers as long as all buffers
            // are freed at shutdown. Simply free the buffers referenced in pBuffer. Possible
            // IO buffers previously transfered as receive resource are freed when the EDDS free's
            // it's internal receive resources.

            EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr = &pCSRTComp->Cons.pConsCrTbl[i];

            for (idx = 0; idx < 3; idx++) {
                if ( LSA_NULL != pCr->pBuffer[idx])
                {
                    EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev, &RetVal,
                            (EDD_UPPER_MEM_PTR_TYPE)pCr->pBuffer[idx]);
                    EDDS_FREE_CHECK(RetVal);

                    pCSRTComp->Cons.pConsCrTbl[i].pBuffer[idx] = LSA_NULL;
                }
            }
        }
    }
    
    /*---------------------------------------------------------------------------*/
    /* Frees Primary AR IDs Table                                                 */
    /*----------------------------------------------------------------------------*/
    // not necessary because the call of EDDS_IOBUFFER_FREE_MNGM_MEM(pDDB->hSysDev, &result, pCSRTComp->pIOBufferManagement)
    // down there, frees among other the Primary AR IDs 
    /*----------------------------------------------------------------------------*/
    /* Free management info block for IOBuffer                                    */
    /*----------------------------------------------------------------------------*/
    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->pIOBufferManagement, LSA_NULL))
    {
        LSA_UINT16 result;

        EDDS_IOBUFFER_FREE_MNGM_MEM(pDDB->hSysDev, &result, pCSRTComp->pIOBufferManagement);
        if(LSA_RET_OK != result)
        {
            EDDS_FatalError(EDDS_FATAL_ERR_MEM_FREE, EDDS_MODULE_ID, __LINE__);
        }
        
        pCSRTComp->pIOBufferManagement = LSA_NULL;
        pCSRTComp->Cons.pConsCrTbl = LSA_NULL;
        pCSRTComp->Prov.pProvCrTbl = LSA_NULL;
    }

    /*----------------------------------------------------------------------------*/
    /* Frees consumer ClearOnMiss buffer                                          */
    /*----------------------------------------------------------------------------*/
    if ( LSA_NULL != pCSRTComp->Cons.pClearOnMissBuffer)
    {
        EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev, &RetVal,
                (EDD_UPPER_MEM_PTR_TYPE)pCSRTComp->Cons.pClearOnMissBuffer);
        EDDS_FREE_CHECK(RetVal);
        pCSRTComp->Cons.pClearOnMissBuffer = LSA_NULL;
    }

    /*----------------------------------------------------------------------------*/
    /* Frees knot-table-management                                                */
    /*----------------------------------------------------------------------------*/

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotTbl, LSA_NULL)) {

        EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Knot.pKnotTbl);
        EDDS_FREE_CHECK(RetVal);

        pCSRTComp->Knot.pKnotTbl = LSA_NULL;
    }

    if (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotElementTbl, LSA_NULL)) {

        EDDS_FREE_LOCAL_MEM(&RetVal, pCSRTComp->Knot.pKnotElementTbl);
        EDDS_FREE_CHECK(RetVal);

        pCSRTComp->Knot.pKnotElementTbl = LSA_NULL;
    }

    EDDS_FreeInternalRQB(&pCSRTComp->Indication.ConsIndTriggerRQB);

    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTFreeCSRTManagement()");

    LSA_UNUSED_ARG(TraceIdx);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTInitCSRTManagement                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                             EDDS_DDB_COMP_CSRT_TYPE_PTR   pCSRTComp     +*/
/*+                             EDDS_CSRT_INI_PTR_TYPE        pCSRTIni      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DeviceDescriptionBlock                       +*/
/*+  pCSRTComp    : Pointer to CSRT Management which will be filled         +*/
/*+  pCSRTIni     : Pointer to init-parameter                               +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NO_TIMER                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function allocates and initializes all structures    +*/
/*+               within  CSRTComp.  CSRTComp has to be preinitialized with +*/
/*+               0 on entry!!                                              +*/
/*+                                                                         +*/
/*+               Consumer-management in fast memory                        +*/
/*+               Consumer-management in normal memory                      +*/
/*+               Provider-management in normal memory                      +*/
/*+               Knot    -management in normal memory                      +*/
/*+               All transferbuffers for consumer/provider will be alloc.  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 can not be a null pointer (previously taken from DDB) - pMgmInfoBlock = pCSRTComp-> ...
//WARNING: be careful when calling this function, make sure not to use pCSRTComp or pCSRTInit as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTInitCSRTManagement(
        EDDS_LOCAL_DDB_PTR_TYPE pDDB, EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp,
        EDDS_UPPER_DPB_CSRT_PTR_TYPE pCSRTIni)
{
    LSA_RESULT Status;
    LSA_UINT32 Size;
    LSA_UINT16 i;
    LSA_UINT32 Help;
    LSA_UINT32 TraceIdx;

    EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR pMgmInfoBlock = LSA_NULL;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTInitCSRTManagement(pDDB: 0x%X,pCSRTComp: 0x%X, pCSRTIni: 0x%X)",
            pDDB, pCSRTComp, pCSRTIni);

    Status = EDD_STS_OK;

    /*-----------------------------------------------------------------------*/
    /* check for valid parameters                                            */
    /* cyclic SRT                                                            */
    /*   - ConsumerCnt have to be > 0                                        */
    /*   - ProviderCnt have to be > 0 & <= EDDS_SRT_PROVIDER_CNT_MAX         */
    /*   - ProviderCnt+ConsumerCnt is limit to 0xFFFF (save space)           */
    /*   - CycleBaseFactor     range (only 32,64,128..1024  supported yet)   */
    /*                         Note: if we change this, the timestamp calc   */
    /*                               may needs to be changed!!               */
    /*   - FrameID-Range for Consumer have to be in valid range              */
    /*   - ProviderCycleDropCnt range                                        */
    /*                                                                       */
    /*-----------------------------------------------------------------------*/

    /* ********************************************************************* */
    /* scheduler cycle: check for 1,2,4,8,16,32 ... value                    */
    /* ********************************************************************* */
    if( !( pDDB->pGlob->schedulerCycleIO &&
            !( pDDB->pGlob->schedulerCycleIO & ( pDDB->pGlob->schedulerCycleIO-1)))
    )
    {
        Status = EDD_STS_ERR_PARAM;

        EDDS_CRT_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDS_RTInitCSRTManagement: ERROR -> SchedulerCycleIO(%d) is not binary (2^n)!",
                pDDB->pGlob->schedulerCycleIO);
    }

    if ( (0 == pDDB->pGlob->schedulerCycleIO) || (EDDS_MAX_SCHEDULER_CYCLE_IO < pDDB->pGlob->schedulerCycleIO) )
    {
        Status = EDD_STS_ERR_PARAM;

        EDDS_NRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_ERROR,"SRTInitCSRTManagement: schedulerCycleIO with %d is not allowed!", pDDB->pGlob->schedulerCycleIO);
    }

    if(EDD_STS_OK == Status)
    {

        // scheduler cycle
        pCSRTComp->schedulerModuloFactorIO = EDDS_RTCalculateSchedulerReduction(pCSRTIni->CycleBaseFactor, pDDB->pGlob->schedulerCycleIO);

        if (
                (pCSRTIni->ProviderCnt > EDDS_SRT_PROVIDER_CNT_MAX)
                ||
                ((pCSRTIni->CycleBaseFactor != 32) && /*  1ms */
                (pCSRTIni->CycleBaseFactor != 64) && /*  2ms */
                (pCSRTIni->CycleBaseFactor != 128) && /*  4ms */
                (pCSRTIni->CycleBaseFactor != 256) && /*  8ms */
                (pCSRTIni->CycleBaseFactor != 512) && /* 16ms */
                (pCSRTIni->CycleBaseFactor != 1024) /* 32ms */
                )
                || ((pCSRTIni->ConsumerCntClass2 != 0)
                        && /* if class2 present, check range */
                        ((pCSRTIni->ConsumerFrameIDBaseClass2
                                < EDD_SRT_FRAMEID_CSRT_START1)
                                || (pCSRTIni->ConsumerFrameIDBaseClass2
                                        + pCSRTIni->ConsumerCntClass2 - 1
                                        > EDD_SRT_FRAMEID_CSRT_STOP1)))
                || ((pCSRTIni->ConsumerCntClass1 != 0)
                        && /* if class1/4 present, check range */
                        ((pCSRTIni->ConsumerFrameIDBaseClass1
                                < EDD_SRT_FRAMEID_CSRT_START2)
                                || (pCSRTIni->ConsumerFrameIDBaseClass1
                                        + pCSRTIni->ConsumerCntClass1 - 1
                                        > EDD_SRT_FRAMEID_CSRT_STOP2)))
                || (pCSRTIni->ProviderCycleDropCnt
                        > EDDS_SRT_PROVIDER_CYCLE_DROP_MAX)
                || ((pCSRTIni->CycleBaseFactor / 32) < pDDB->pGlob->schedulerCycleIO)
                // schedulerModuloFactor: check for 1,2,4,8,16,32 ... value
                || !(pCSRTComp->schedulerModuloFactorIO && !(pCSRTComp->schedulerModuloFactorIO & (pCSRTComp->schedulerModuloFactorIO-1)))
        ) {
            EDDS_CRT_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDS_RTInitCSRTManagement: Some CSRTIni Parameters are invalid!");
            Status = EDD_STS_ERR_PARAM;
        } else {
            /* store values in management structure */

            pCSRTComp->HandleCnt = 0;
            pCSRTComp->Cfg = *pCSRTIni;
            pCSRTComp->schedulerCounter = 0;

            /* calculate derived values..           */

            if (pCSRTComp->Cfg.ConsumerCntClass2) {
                pCSRTComp->Cons.ConsumerFrameIDBase1 =
                        pCSRTComp->Cfg.ConsumerFrameIDBaseClass2;
                pCSRTComp->Cons.ConsumerFrameIDTop1 =
                        pCSRTComp->Cfg.ConsumerFrameIDBaseClass2
                                + pCSRTComp->Cfg.ConsumerCntClass2 - 1;
            } else {
                pCSRTComp->Cons.ConsumerFrameIDBase1 = 0; /* not used */
                pCSRTComp->Cons.ConsumerFrameIDTop1 = 0; /* not used */
            }

            if (pCSRTComp->Cfg.ConsumerCntClass1) {
                pCSRTComp->Cons.ConsumerFrameIDBase2 =
                        pCSRTComp->Cfg.ConsumerFrameIDBaseClass1;
                pCSRTComp->Cons.ConsumerFrameIDTop2 =
                        pCSRTComp->Cfg.ConsumerFrameIDBaseClass1
                                + pCSRTComp->Cfg.ConsumerCntClass1 - 1;
            } else {
                pCSRTComp->Cons.ConsumerFrameIDBase2 = 0; /* not used */
                pCSRTComp->Cons.ConsumerFrameIDTop2 = 0; /* not used */
            }


            pCSRTComp->Cons.ConsumerCnt = pCSRTComp->Cfg.ConsumerCntClass2
                    + pCSRTComp->Cfg.ConsumerCntClass1; /* > 0! */

            /* calculate shift factor to calculate 31.25us based time stamp from CycleCounter */
            /* so we can calculate the Timestamp like this:                                   */
            /* Timestamp =ActualCycleCounter << CycleStampAdjustShift                         */
            /* NOTE: This assumes we have 2-potenz timebase (e.g.1,2,4 ms). If this changes   */
            /*       we cant shift but have to multiply!                                      */

            Help = pCSRTComp->Cfg.CycleBaseFactor;
            i = 0;
            while (!(Help & 0x1)) {
                i++;
                Help >>= 1;
            }

            pCSRTComp->CycleStampAdjustShift = i;

    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            pCSRTComp->IPParams.TTL = EDD_DEFAULT_TTL;
    #endif

        }




        if (EDD_STS_OK == Status)
        {
            LSA_UINT32 szIOBufferManagement = sizeof(EDDS_CSRT_MGM_INFOBLOCK_TYPE);
            LSA_UINT32 szConsCrTbl          = sizeof(EDDS_CSRT_CONSUMER_CR_TYPE) * pCSRTComp->Cons.ConsumerCnt;
            LSA_UINT32 szProvCrTbl          = sizeof(EDDS_CSRT_PROVIDER_CR_TYPE) * pCSRTComp->Cfg.ProviderCnt;
            LSA_UINT32 szPrimaryARIDTbl     = sizeof(pMgmInfoBlock->PrimaryARIDSetTable);
            /*----------------------------------------------------------------------------*/
            /* Allocate management info block for IOBuffer                                */
            /* Allocate consumer parts for CR in shared memory                            */
            /* Allocate provider parts for CR in shared memory                            */
            /*----------------------------------------------------------------------------*/
            EDDS_IOBUFFER_ALLOC_MNGM_MEM(pDDB->hSysDev, (EDD_UPPER_MEM_PTR_TYPE*)&pCSRTIni->hIOBufferManagement, szIOBufferManagement + szConsCrTbl + szProvCrTbl );

            if((LSA_HOST_PTR_ARE_EQUAL(pCSRTIni->hIOBufferManagement, LSA_NULL)))
            {
                Status = EDD_STS_ERR_RESOURCE;
            }
            else
            {
                pMgmInfoBlock = (EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR) pCSRTIni->hIOBufferManagement;
                pMgmInfoBlock->ConsumerCrCnt = H_TO_LE_S((LSA_UINT16)pCSRTComp->Cons.ConsumerCnt); // convert to LittleEndian
                pMgmInfoBlock->ProviderCrCnt = H_TO_LE_S(pCSRTComp->Cfg.ProviderCnt); // convert to LittleEndian
                pMgmInfoBlock->pConsumerCrTableOffset  = szIOBufferManagement;
                pMgmInfoBlock->pProviderCrTableOffset  = szIOBufferManagement + szConsCrTbl;
                
                
                pCSRTComp->pIOBufferManagement = pCSRTIni->hIOBufferManagement;
                pCSRTComp->Cons.pConsCrTbl = (EDDS_CSRT_CONSUMER_CR_TYPE_PTR)((EDD_UPPER_MEM_U8_PTR_TYPE)pCSRTComp->pIOBufferManagement 
                                + pMgmInfoBlock->pConsumerCrTableOffset); //lint !e826 cast is necessary (JB 04/2016)
                pCSRTComp->Prov.pProvCrTbl = (EDDS_CSRT_PROVIDER_CR_TYPE_PTR)((EDD_UPPER_MEM_U8_PTR_TYPE)pCSRTComp->pIOBufferManagement 
                                + pMgmInfoBlock->pProviderCrTableOffset); //lint !e826 cast is necessary (JB 04/2016)
                
                // set all values to 0 for Provider and Consumer
                EDDS_MEMSET_LOCAL(pCSRTComp->Cons.pConsCrTbl, 0, szConsCrTbl);
                EDDS_MEMSET_LOCAL(pCSRTComp->Prov.pProvCrTbl, 0, szProvCrTbl);
                
                EDDS_MEMSET_LOCAL( &(pMgmInfoBlock->PrimaryARIDSetTable), 0, szPrimaryARIDTbl);
            }
        }

        /*----------------------------------------------------------------------------*/
        /* Initialize consumer parts                                                  */
        /*----------------------------------------------------------------------------*/

        if (Status == EDD_STS_OK) {

            pCSRTComp->Cons.ConsumerCycle.CycleInProgress = LSA_FALSE;
            pCSRTComp->Cons.ConsumerCycle.MinInterval =
                    EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN; /* dont care */
            pCSRTComp->Cons.ConsumerCycle.MaxInterval =
                    EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN; /* dont care */

            /* allocate consumer parts in fast memory. */

            /* We use one array holding all Consumers of both arrays.        */
            /* The array2 Consumers are located behind the array1 Consumers  */
            /* so the ConsumerID is still the index into the table           */
            Size = sizeof(EDDS_CSRT_CONSUMER_FRAME_TYPE)
                    * pCSRTComp->Cons.ConsumerCnt;

            EDDS_ALLOC_LOCAL_FAST_MEM(
                    ((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Cons.pConsFrameTbl),
                    Size);

            if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsFrameTbl, LSA_NULL)) {
                Status = EDD_STS_ERR_RESOURCE;
            } else {
                EDDS_MEMSET_LOCAL(pCSRTComp->Cons.pConsFrameTbl, 0, Size);
                /* Help-Pointer for better accesss/debug */
                pCSRTComp->Cons.pConsFrameTbl2 =
                        &pCSRTComp->Cons.pConsFrameTbl[pCSRTComp->Cfg.ConsumerCntClass2];
            }

            /* allocate consumer parts in normal memory */

            if (Status == EDD_STS_OK) {
                /* We use one array holding both xRT and xRToverUDP Consumers    */
                /* The xRToverUDP Consumers are located behind the xRT Consumers */
                /* so the ConsumerID is still the index into the table           */
                Size = sizeof(EDDS_CSRT_CONSUMER_TYPE)
                        * pCSRTComp->Cons.ConsumerCnt;

                EDDS_ALLOC_LOCAL_MEM(
                        ((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Cons.pConsTbl), Size);

                if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Cons.pConsTbl, LSA_NULL)) {
                    Status = EDD_STS_ERR_RESOURCE;
                } else {
                    EDDS_MEMSET_LOCAL(pCSRTComp->Cons.pConsTbl, 0, Size);
                    /* Help-Pointer for better accesss/debug */
                    pCSRTComp->Cons.pConsTbl2 =
                            &pCSRTComp->Cons.pConsTbl[pCSRTComp->Cfg.ConsumerCntClass2];
                }
            }

            if (Status == EDD_STS_OK) {
                /* init parts of management. we init the free-chain with */
                /* all consumer.                                         */

                pCSRTComp->Cons.ConsumerFree.BottomIdx = 0;
                pCSRTComp->Cons.ConsumerFree.TopIdx =
                        (LSA_UINT16) (pCSRTComp->Cons.ConsumerCnt - 1);

                pCSRTComp->Cons.ConsumerUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */
                pCSRTComp->Cons.ConsumerUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */

                for (i = 0; i < pCSRTComp->Cons.ConsumerCnt; i++) {

                    pCSRTComp->Cons.pConsTbl[i].UsedFreeLink.Next = i + 1;

                    if (i == 0)
                        pCSRTComp->Cons.pConsTbl[i].UsedFreeLink.Prev =
                                EDDS_SRT_CHAIN_IDX_END;
                    else
                        pCSRTComp->Cons.pConsTbl[i].UsedFreeLink.Prev = i - 1;

                    pCSRTComp->Cons.pConsTbl[i].Type =
                            EDDS_CSRT_CONSUMER_TYPE_NOT_USED;
                    pCSRTComp->Cons.pConsFrameTbl[i].State =
                            EDDS_SRT_CONS_STATE_INIT;
                }

                /* last chain element */

                pCSRTComp->Cons.pConsTbl[pCSRTComp->Cons.ConsumerCnt - 1].UsedFreeLink.Next =
                        EDDS_SRT_CHAIN_IDX_END;

            }

            /* ----------------------------------------------------------------------*/
            /* Allocate consumer-frame-buffers. Two buffers for each consumer.       */
            /* Buffers are located in shared memory.                                 */
            /* ----------------------------------------------------------------------*/

            i = 0;
            while (i < pCSRTComp->Cons.ConsumerCnt) {
                // all values were set to 0 on initialization
                //pCSRTComp->Cons.pConsCrTbl[i].CrUsed   = EDDS_CSRT_CONSUMER_CR_UNUSED;
                //pCSRTComp->Cons.pConsCrTbl[i].IsLocked = EDDS_CSRT_CONSUMER_CR_UNLOCKED;
                //pCSRTComp->Cons.pConsCrTbl[i].Type     = EDDS_CSRT_CONSUMER_TYPE_NOT_USED;

                // To be aware of endianess issues take byte that is pointed to
                // by User, XChange and EDDS. Access to byte by address is endianess
                // independent. All needed informations are stored in that byte.

                //pCSRTComp->Cons.pConsCrTbl[i].User    = 0;
                //pCSRTComp->Cons.pConsCrTbl[i].XChange = 0;
                //pCSRTComp->Cons.pConsCrTbl[i].EDDS    = 0;

                // NOTE:
                // Information like index and new flag is always stored in one byte in
                // the EXCHANGE variable at its base address. This applies to EDDS and
                // IOBuffer when accessing these variables. Access to this byte by byte
                // pointer does not need any consideration of endianess.
                //
                // example:
                // Value stored in memory for Xchange with reference to buffer 1 and
                // new flag is 0x8100. Index and new flag can be retrieved by accessing
                // Xchange with its address casted to a byte pointer independently from
                // host endianess.
                //
                // index = (*((LSA_UINT8*)&Xchange)) & ~MASK

                // *((LSA_UINT8*)&pCSRTComp->Cons.pConsCrTbl[i].User)    = 0;
                *((LSA_UINT8*) &pCSRTComp->Cons.pConsCrTbl[i].XChange) = 1;
                *((LSA_UINT8*) &pCSRTComp->Cons.pConsCrTbl[i].EDDS) = 2;

                if (Status == EDD_STS_OK) {
                    Status = EDDS_RTAllocRxFrameBuffer(pDDB,
                            &pCSRTComp->Cons.pConsCrTbl[i].pBuffer[0]);
                }
                if (Status == EDD_STS_OK) {
                    Status = EDDS_RTAllocRxFrameBuffer(pDDB,
                            &pCSRTComp->Cons.pConsCrTbl[i].pBuffer[1]);
                }

                i++;
            }

            /*----------------------------------------------------------------------------*/
            /* Allocate ClearOnMiss buffer                                                */
            /*----------------------------------------------------------------------------*/
            if (Status == EDD_STS_OK) {
                Status = EDDS_RTAllocRxFrameBuffer(pDDB,
                        &pCSRTComp->Cons.pClearOnMissBuffer);
            }
        }

        /*----------------------------------------------------------------------------*/
        /* Initialize provider parts                                                  */
        /*----------------------------------------------------------------------------*/

        if (Status == EDD_STS_OK) {

            /* Set statevariables of Sender             */
            pCSRTComp->Prov.ProviderCycle.Xrt.Type = EDDS_CSRT_PROVIDER_TYPE_XRT;
            pCSRTComp->Prov.ProviderCycle.Xrt.ActProvIdx = EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Prov.ProviderCycle.Xrt.MinInterval =
                    EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN;
            pCSRTComp->Prov.ProviderCycle.Xrt.MaxInterval =
                    EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN;

            /* setup some variables (already 0) */
            //pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress    = LSA_FALSE;
            //pCSRTComp->Prov.ProviderCycle.Xrt.CycleSendStarted   = LSA_FALSE;
            //pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend        = LSA_FALSE;
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            /* Set statevariables of Sender             */
            pCSRTComp->Prov.ProviderCycle.Udp.Type = EDDS_CSRT_PROVIDER_TYPE_UDP;
            pCSRTComp->Prov.ProviderCycle.Udp.ActProvIdx = EDDS_SRT_CHAIN_IDX_END;
            pCSRTComp->Prov.ProviderCycle.Udp.MinInterval = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN;
            pCSRTComp->Prov.ProviderCycle.Udp.MaxInterval = EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN;

            /* setup some variables (already 0) */
            //pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress    = LSA_FALSE;
            //pCSRTComp->Prov.ProviderCycle.Udp.CycleSendStarted   = LSA_FALSE;
            //pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend        = LSA_FALSE;
    #endif

            /* allocate provider parts in normal memory */

            Size = sizeof(EDDS_CSRT_PROVIDER_TYPE) * pCSRTComp->Cfg.ProviderCnt;

            EDDS_ALLOC_LOCAL_MEM(
                    ((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Prov.pProvTbl), Size);

            if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvTbl, LSA_NULL)) {
                Status = EDD_STS_ERR_RESOURCE;
            } else {
                EDDS_MEMSET_LOCAL(pCSRTComp->Prov.pProvTbl, 0, Size);

                /* init parts of management. we init the free-chain with */
                /* all consumer.                                         */

                pCSRTComp->Prov.ProviderFree.BottomIdx = 0;
                pCSRTComp->Prov.ProviderFree.TopIdx = pCSRTComp->Cfg.ProviderCnt
                        - 1;

                pCSRTComp->Prov.ProviderUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */
                pCSRTComp->Prov.ProviderUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END; /* no provider used yet */

                for (i = 0; i < pCSRTComp->Cfg.ProviderCnt; i++) {

                    pCSRTComp->Prov.pProvTbl[i].UsedFreeLink.Next = i + 1;

                    if (i == 0)
                        pCSRTComp->Prov.pProvTbl[i].UsedFreeLink.Prev =
                                EDDS_SRT_CHAIN_IDX_END;
                    else
                        pCSRTComp->Prov.pProvTbl[i].UsedFreeLink.Prev = i - 1;

                    pCSRTComp->Prov.pProvTbl[i].Type =
                            EDDS_CSRT_PROVIDER_TYPE_NOT_USED;
                    pCSRTComp->Prov.pProvTbl[i].Properties = 0;
                    pCSRTComp->Prov.pProvTbl[i].Active = LSA_FALSE;
                }

                /* last chain element */

                pCSRTComp->Prov.pProvTbl[pCSRTComp->Cfg.ProviderCnt - 1].UsedFreeLink.Next =
                        EDDS_SRT_CHAIN_IDX_END;

            }

            /* ----------------------------------------------------------------------*/
            /* allocate provider-frame-buffers. three buffers for each provider.     */
            /* (we allocate for provider the same buffers which we use for consumer  */
            /*  so we are able to exchange provider with consumer-buffers if needed) */
            /* ----------------------------------------------------------------------*/

            i = 0;
            while (i < pCSRTComp->Cfg.ProviderCnt) {
                // already 0
                //pCSRTComp->Prov.pProvCrTbl[i].CrUsed   = EDDS_CSRT_CONSUMER_CR_UNUSED;
                //pCSRTComp->Prov.pProvCrTbl[i].IsLocked = EDDS_CSRT_CONSUMER_CR_UNLOCKED;
                //pCSRTComp->Prov.pProvCrTbl[i].Type     = EDDS_CSRT_PROVIDER_TYPE_NOT_USED;

                // To be aware of endianess issues take byte that is pointed to
                // by User, XChange and EDDS. Access to byte by address is endianess
                // independent. All needed informations are stored in that byte.

                //pCSRTComp->Prov.pProvCrTbl[i].User    = 0;
                //pCSRTComp->Prov.pProvCrTbl[i].XChange = 0;
                //pCSRTComp->Prov.pProvCrTbl[i].EDDS    = 0;
                //pCSRTComp->Prov.pProvCrTbl[i].Latest  = 0;

                // NOTE:
                // Information like index and new flag is always stored in one byte in
                // the EXCHANGE variable at its base address. This applies to EDDS and
                // IOBuffer when accessing these variables. Access to this byte by byte
                // pointer does not need any consideration of endianess.
                //
                // example:
                // Value stored in memory for Xchange with reference to buffer 1 and
                // new flag is 0x8100. Index and new flag can be retrieved by accessing
                // Xchange with its address casted to a byte pointer independently from
                // host endianess.
                //
                // index = (*((LSA_UINT8*)&Xchange)) & ~MASK

                //*((LSA_UINT8*)&pCSRTComp->Prov.pProvCrTbl[i].User)    = 0;
                *((LSA_UINT8*) &pCSRTComp->Prov.pProvCrTbl[i].XChange) = 1;
                *((LSA_UINT8*) &pCSRTComp->Prov.pProvCrTbl[i].EDDS) = 2;
                *((LSA_UINT8*) &pCSRTComp->Prov.pProvCrTbl[i].Latest) = 1;

                if (Status == EDD_STS_OK) {
                    Status = EDDS_RTAllocTxFrameBuffer(pDDB,
                            &pCSRTComp->Prov.pProvCrTbl[i].pBuffer[0],
                            EDD_FRAME_BUFFER_LENGTH);
                }

                if (Status == EDD_STS_OK) {
                    Status = EDDS_RTAllocTxFrameBuffer(pDDB,
                            &pCSRTComp->Prov.pProvCrTbl[i].pBuffer[1],
                            EDD_FRAME_BUFFER_LENGTH);
                }

                if (Status == EDD_STS_OK) {
                    Status = EDDS_RTAllocTxFrameBuffer(pDDB,
                            &pCSRTComp->Prov.pProvCrTbl[i].pBuffer[2],
                            EDD_FRAME_BUFFER_LENGTH);
                }

                i++;
            }

            /* Alloc and initialize ProviderGroup-Array */

            if (Status == EDD_STS_OK) {

                /* allocate provider Group-Array in normal memory */

                Size = sizeof(EDDS_CSRT_PROV_GROUP_TYPE)
                        * EDD_CFG_CSRT_MAX_PROVIDER_GROUP;

                EDDS_ALLOC_LOCAL_MEM(
                        ((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Prov.pProvGroupTbl),
                        Size);

                if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Prov.pProvGroupTbl, LSA_NULL)) {
                    Status = EDD_STS_ERR_RESOURCE;
                } else {
                    /* Initialize Group-data. */

                    EDDS_MEMSET_LOCAL(pCSRTComp->Prov.pProvGroupTbl,
                            EDD_CSRT_DSTAT_GLOBAL_INIT_VALUE, Size);

                    // the same initialization as a loop
                    //for (i=0; i<EDD_CFG_CSRT_MAX_PROVIDER_GROUP; i++)
                    //{
                    //    /* Initialize Global Provider State for all Groups */
                    //
                    //    pCSRTComp->Prov.pProvGroupTbl[i].ProviderDataStatus  = EDD_CSRT_DSTAT_GLOBAL_INIT_VALUE;
                    //}
                }
            }
        }

        /*----------------------------------------------------------------------------*/
        /* Initialize knot-management for provider and consumer                       */
        /*----------------------------------------------------------------------------*/

        if (Status == EDD_STS_OK) {
            /* caclulate number of Knot-table-entrys:           */
            /* The number is fixed.                             */
            /* 2^(EDDS_CSRT_Cycle_REDUCTION_RATIO_FACTOR_MAX)-1 */

            pCSRTComp->Knot.KnotCnt =
                    EDDSGet2Potenz(EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX) - 1;

            Size = sizeof(EDDS_CSRT_KNOT_TYPE) * pCSRTComp->Knot.KnotCnt;

            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "EDDS_RTInitCSRTManagement: Allocating %d knots",
                    pCSRTComp->Knot.KnotCnt);

            EDDS_ALLOC_LOCAL_MEM(
                    ((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Knot.pKnotTbl), Size);

            if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotTbl, LSA_NULL)) {
                Status = EDD_STS_ERR_RESOURCE;
            } else {
                for (i = 0; i < pCSRTComp->Knot.KnotCnt; i++) {
                    pCSRTComp->Knot.pKnotTbl[i].KnotElementIdx =
                            EDDS_SRT_IDX_NOT_USED;
                }

                /* we allocate ConsumerCnt+ProviderCnt Knotelements. If this is more then */
                /* Knots we use KnotCnt (because this is the maximum we need.             */

                pCSRTComp->Knot.KnotElementCnt = pCSRTComp->Cons.ConsumerCnt
                        + pCSRTComp->Cfg.ProviderCnt + 1; /* one more for changephase handling */

                if (pCSRTComp->Knot.KnotElementCnt > pCSRTComp->Knot.KnotCnt)
                    pCSRTComp->Knot.KnotElementCnt = pCSRTComp->Knot.KnotCnt;

                Size = sizeof(EDDS_CSRT_KNOT_ELEMENT_TYPE)
                        * pCSRTComp->Knot.KnotElementCnt;

                EDDS_ALLOC_LOCAL_MEM(
                        ((EDDS_LOCAL_MEM_PTR_TYPE *)&pCSRTComp->Knot.pKnotElementTbl),
                        Size);

                if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Knot.pKnotElementTbl,
                        LSA_NULL)) {
                    Status = EDD_STS_ERR_RESOURCE;
                } else {
                    EDDS_MEMSET_LOCAL(pCSRTComp->Knot.pKnotElementTbl, 0, Size);

                    pCSRTComp->Knot.KnotElementFree.BottomIdx = 0;
                    pCSRTComp->Knot.KnotElementFree.TopIdx =
                            (LSA_UINT16) (pCSRTComp->Knot.KnotElementCnt - 1);

                    pCSRTComp->Knot.KnotElementUsed.BottomIdx =
                            EDDS_SRT_CHAIN_IDX_END; /* no knot element used yet */
                    pCSRTComp->Knot.KnotElementUsed.TopIdx = EDDS_SRT_CHAIN_IDX_END; /* no knot element used yet */

                    /* initialize send and check-lists */
                    for (i = 0; i < pCSRTComp->Knot.KnotElementCnt; i++) {
                        pCSRTComp->Knot.pKnotElementTbl[i].InUse = LSA_FALSE;

                        pCSRTComp->Knot.pKnotElementTbl[i].UsedFreeLink.Next = i
                                + 1;

                        if (i == 0)
                            pCSRTComp->Knot.pKnotElementTbl[i].UsedFreeLink.Prev =
                                    EDDS_SRT_CHAIN_IDX_END;
                        else
                            pCSRTComp->Knot.pKnotElementTbl[i].UsedFreeLink.Prev = i
                                    - 1;

                        pCSRTComp->Knot.pKnotElementTbl[i].ConsumerCheckList.BottomIdx =
                                EDDS_SRT_CHAIN_IDX_END;
                        pCSRTComp->Knot.pKnotElementTbl[i].ConsumerCheckList.TopIdx =
                                EDDS_SRT_CHAIN_IDX_END;
                        pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListXRT.BottomIdx =
                                EDDS_SRT_CHAIN_IDX_END;
                        pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListXRT.TopIdx =
                                EDDS_SRT_CHAIN_IDX_END;
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                        pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                        pCSRTComp->Knot.pKnotElementTbl[i].ProviderSendListUDP.TopIdx = EDDS_SRT_CHAIN_IDX_END;
    #endif
                    }

                    /* last chain element */
                    pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.KnotElementCnt
                            - 1].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
                }
            }
        }

        /*----------------------------------------------------------------------------*/
        /* alloc internal RQB  for triggering a consumer indication check.            */
        /*----------------------------------------------------------------------------*/

        if (Status == EDD_STS_OK) {
            pCSRTComp->Indication.ConsIndTriggerRQB.pRQB = EDDS_AllocInternalRQB();

            if (LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Indication.ConsIndTriggerRQB.pRQB,
                    LSA_NULL)) {
                Status = EDD_STS_ERR_RESOURCE;
            } else
                EDDS_SetupInternalRQBParams(&pCSRTComp->Indication.ConsIndTriggerRQB,
                EDDS_SRV_DEV_TRIGGER, LSA_NULL, /* dont care */
                pDDB,
                EDDS_TRIGGER_CONS_INDICATION, 0);
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTInitCSRTManagement(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTInitComponent                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_DPB                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_CMP_SRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_COMP_SRT_INI           +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDDS_STS_ERR_RESOURCES          +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the SRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//check pDDB

//JB 18/11/2014 checked where called
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTInitComponent(
        EDD_UPPER_RQB_PTR_TYPE      pRQB,
        EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{
    LSA_RESULT Status = EDD_STS_OK;
    EDDS_UPPER_DPB_CSRT_PTR_TYPE    pCRT;
    EDDS_UPPER_DPB_NRT_PTR_TYPE     pNRT;
    EDDS_UPPER_DDB_INI_PTR_TYPE     pRqbDDB;
    EDDS_UPPER_DPB_PTR_TYPE         pDPB;
    LSA_UINT32                      TraceIdx;

    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pSRTComp;
    LSA_UINT16 RetVal;

    EDDS_IS_VALID_PTR(pDDB);

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTInitComponent(pRQB: 0x%X)", pRQB);

    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                "EDDS_RTInitComponent: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        pRqbDDB = (EDDS_UPPER_DDB_INI_PTR_TYPE)pRQB->pParam;
        pDPB    = pRqbDDB->pDPB;

        pCRT    = &pDPB->CSRT;
        pNRT    = &pDPB->NRT;

        /* check if not already initialized. If so report error */

        if (!LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL)) {
            EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                    "EDDS_RTInitComponent: SRT already initialized!");
            Status = EDD_STS_ERR_SEQUENCE;
        } else {

            /* if we have no CRT used, we do not need to alloc memory */

            /* could CRT be used? */
            // from EDDS 6.0 (NEA), it is allowed to have provider=0 and consumer=0 -> no CRT could be used
            if ((0 == pCRT->ConsumerCntClass2) &&
                (0 == pCRT->ConsumerCntClass1) && /* both 0 not allowed */
                (0 == pCRT->ProviderCnt))
            {
                pDDB->pGlob->IsSrtUsed = LSA_FALSE;
                pDDB->pGlob->IO_Configured = LSA_FALSE;
                pDDB->pGlob->IO_ConfiguredDefault = LSA_FALSE;
                pDDB->pSRT = LSA_NULL;
                pCRT->hIOBufferManagement = LSA_NULL;
                Status = EDD_STS_OK;
            }
            else
            {
                pDDB->pGlob->IsSrtUsed = LSA_TRUE;
            }

            // check whether IO_Configured = True And IO_Supported = False
            // then reject
            if (   (EDD_SYS_IO_CONFIGURED_ON == pNRT->IO_Configured)
                && !pDDB->pGlob->IsSrtUsed)
            {
                EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RTInitComponent: IO_Configured=TRUE and IO_Supported=FALSE is not allowed!");
                Status = EDD_STS_ERR_PARAM;
            }
            else
            {

                // only allocate SRT, if needed

                if ( pDDB->pGlob->IsSrtUsed)
                {
                    EDDS_ALLOC_LOCAL_FAST_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pSRTComp),
                            sizeof(EDDS_DDB_COMP_SRT_TYPE));

                    if (LSA_HOST_PTR_ARE_EQUAL(pSRTComp, LSA_NULL)) 
                    {  
                        EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                                          "EDDS_RTInitComponent: Allocating SRT-memory failed!");
                        Status = EDD_STS_ERR_RESOURCE;
                    } 
                    else 
                    {
                        /* init with 0 */

                        EDDS_MEMSET_LOCAL(pSRTComp, 0,
                                sizeof(EDDS_DDB_COMP_SRT_TYPE));

                        pDDB->pSRT = pSRTComp;

                        /*----------------------------------------------------*/
                        /* Setup cSRT parts..                                 */
                        /*----------------------------------------------------*/

                        Status = EDDS_RTInitCSRTManagement(pDDB, pSRTComp,
                                pCRT);

                        /* is SRT setup and supported */
                        if (Status == EDD_STS_OK)
                        {
                            // IO_Configured is taken from NRT part of DPB
                            pDDB->pGlob->IO_Configured = (EDD_SYS_IO_CONFIGURED_OFF != pNRT->IO_Configured)?LSA_TRUE:LSA_FALSE;
                            pDDB->pGlob->IO_ConfiguredDefault = (EDD_SYS_IO_CONFIGURED_OFF != pNRT->IO_Configured)?LSA_TRUE:LSA_FALSE;

                            // set NRT values corresponding parameterset - NRT and CRT
                            pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_ASRT_ALARM_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntASRTAlarm[EDDS_DPB_PARAMETERSET_CRT_MODE];
                            pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_IP_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntARPIP[EDDS_DPB_PARAMETERSET_CRT_MODE];
                            pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_ASRT_OTHER_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntASRTOther[EDDS_DPB_PARAMETERSET_CRT_MODE];
                            pDDB->pNRT->RxDscrMgm [EDDS_NRT_BUFFER_OTHER_IDX].RxDscrLookUpCntMax = pDDB->pNRT->RxBufferCntOther[EDDS_DPB_PARAMETERSET_CRT_MODE];
                        }

                        if (Status != EDD_STS_OK)
                        {
                            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                                    "EDDS_RTInitComponent: SRT initalization failed! (0x%X)",
                                    Status);

                            EDDS_RTFreeCSRTManagement(pDDB);

                            EDDS_FREE_LOCAL_FAST_MEM(&RetVal, pSRTComp);
                            EDDS_FREE_CHECK(RetVal);

                            pDDB->pSRT = LSA_NULL;
                        }
                    }
                }
            }
        }

        EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_RTInitComponent finished. Response: 0x%X",Status);
    }

    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTInitComponent()");

    LSA_UNUSED_ARG(TraceIdx);

    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTRelComponent                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_COMP_SRT_REL       +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_CMP_SRT_REL_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_COMP_NRT_REL           +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDDS_STS_ERR_RESOURCES          +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will release the SRT-Component management   +*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be freed.                                            +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the any channel      +*/
/*+               uses this component and the device is still setup         +*/
/*+               (device-shutdown has to be called first!)                 +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//check pDDB

//JB 18/11/2014 checked where called
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTRelComponent(EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT16 RetVal;
    LSA_UINT32 TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTRelComponent(pDDB: 0x%X)", pDDB);

    if (!LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL))
    {
        /* check for open channels using this component. */
        /* if open channels, return with error.          */
        /* If hardware is setup (i.e. no device-shutdown */
        /* called yet) we return with error.             */

        if ((!pDDB->pGlob->HWIsSetup)
                && (!pDDB->pSRT->HandleCnt)) {
            /* Free all CSRT and ASRT resources */

            EDDS_RTFreeCSRTManagement(pDDB);

            EDDS_FREE_LOCAL_FAST_MEM(&RetVal, pDDB->pSRT);
            EDDS_FREE_CHECK(RetVal);

            pDDB->pSRT = LSA_NULL;

        } else {
            EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "EDDS_RTRelComponent: Handles still open and/or hardware still setup!");
            Status = EDD_STS_ERR_SEQUENCE;
        }
    }
    else
        Status = EDD_STS_OK;

    EDDS_CRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_RTRelComponent finished. Response: 0x%X",Status);

    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT:EDDS_RTRelComponent()");

    LSA_UNUSED_ARG(TraceIdx);

    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTOpenChannelCyclic                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+                             EDDS_UPPER_CDB_CSRT_PTR_TYPE pCDB           +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel (<= EDDS_CFG_MAX_CHANNELS)        +*/
/*+  pCDB       : Pointer to filled ChannelDescriptionBlock parameters      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_CHANNEL_USE aSRT already in used by a channel +*/
/*+               EDDS_STS_ERR_RESOURCES  Alloc failed                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Opens Channel for cyclic SRT. The Handle will specify     +*/
/*+               the channel and pDDB the Device. pCDB has all SRT         +*/
/*+               parameters to use for this channel                        +*/
/*+                                                                         +*/
/*+               It is not checked if the handle is already open!          +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Only on Channel can use cyclic SRT!                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTOpenChannelCyclic(
        EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTOpenChannelCyclic(pDDB: 0x%X, pHDB: 0x%X, HDBIndex: %d)",
            pDDB, pHDB, pHDB->HDBIndex);

    pCSRTComp = pDDB->pSRT;
    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /* we support only one cSRT-Channel for each device !                        */
    /*---------------------------------------------------------------------------*/

    if (pCSRTComp->HandleCnt)
        Status = EDD_STS_ERR_CHANNEL_USE;
    else {
        pCSRTComp->HandleCnt++; /* cnt of currently open handles  */

#ifdef EDDS_MESSAGE
        /*--------------------------------------------------------------------*/
        /* Start provider-timer here because this cycle will be needed for    */
        /* NRT for some BUG-Workarounds for CPU317 and for CycleStamps.       */
        /*--------------------------------------------------------------------*/
#endif

    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTOpenChannelCyclic(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTCloseChannelCyclic                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel to be closed                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE         Channel has pending requests +*/
/*+               EDD_STS_ERR_RESOURCE         Error on freeing memory      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an  cyclic SRT-Channel. if there are any pending   +*/
/*+               requests the channel can not be closed.                   +*/
/*+                                                                         +*/
/*+               This function is only called if the handle uses aSRT!     +*/
/*+               (not checked!)                                            +*/
/*+               The SRT-Management for the device has to be setup!        +*/
/*+                                                                         +*/
/*+               Note:  If EDD_STS_ERR_RESOURCE is returned the channel is +*/
/*+                      closed but we had an error while freeing memory.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTCloseChannelCyclic(
        EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    EDD_UPPER_RQB_PTR_TYPE pRQB;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTCloseChannelCyclic(pDDB: 0x%X, pHDB: 0x%X, HDBIndex: %d)",
            pDDB, pHDB, pHDB->HDBIndex);

    Status = EDD_STS_OK;

    pCSRTComp = pDDB->pSRT;

    /* ----------------------------------------------------------------------*/
    /* check for something in use.                                           */
    /* ----------------------------------------------------------------------*/

    if (pCSRTComp->Cons.ConsumerUsedCnt || pCSRTComp->Prov.ProviderUsedCnt) {
        Status = EDD_STS_ERR_SEQUENCE;
    } else {
        /* ------------------------------------------------------------------*/
        /* Cancel all Inidication RQBs                                       */
        /* ------------------------------------------------------------------*/

        /* This sequence must not be protected. With closing the RT */
        /* channel all providers and consumers must be removed.     */

        while (!LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Indication.IndicationRequestQueue.pBottom,
                LSA_NULL)) {
            EDDS_RQB_REM_BLOCK_BOTTOM(pCSRTComp->Indication.IndicationRequestQueue.pBottom,
                    pCSRTComp->Indication.IndicationRequestQueue.pTop, pRQB);

            if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) {
                EDDS_RequestFinish(pHDB, pRQB, EDD_STS_OK_CANCEL);
            }
        }

        pCSRTComp->HandleCnt--; /* cnt of currently open handles */

    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTCloseChannelCyclic(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTIndicationProvide                    +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Queues new inidication rqb for cyclic SRT.                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTIndicationProvide(
        EDD_UPPER_RQB_PTR_TYPE  pRQB,
        EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE pInd;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTIndicationProvide(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
            pRQB, pDDB, pHDB);

    /* Because this sequence runs in RQB context and must not */
    /* be interrupted by scheduler we have to use EDDS_ENTER */

    EDDS_ENTER(pDDB->hSysDev);

    /* Queue the new indication resource (the whole RQB!)                */

    EDDS_RQB_PUT_BLOCK_TOP(pDDB->pSRT->Indication.IndicationRequestQueue.pBottom,
            pDDB->pSRT->Indication.IndicationRequestQueue.pTop, pRQB);

    pInd = ((EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE) pRQB->pParam);
    pInd->Count = 0; /* setup Count with 0 */
    pInd->DebugInfo[0] = EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDS;
    pInd->DebugInfo[1] = 0;
    pInd->DebugInfo[2] = 0;
    pInd->DebugInfo[3] = 0;
    pInd->DebugInfo[4] = 0;
    pInd->DebugInfo[5] = 0;
    pInd->DebugInfo[6] = 0;
    pInd->DebugInfo[7] = 0;
    pInd->DebugInfo[8] = 0;
    pInd->DebugInfo[9] = 0;
    pInd->DebugInfo[10] = 0;
    pInd->DebugInfo[11] = 0;

    EDDS_EXIT(pDDB->hSysDev);

    EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTIndicationProvide()");

    LSA_UNUSED_ARG(TraceIdx);

    return (EDD_STS_OK);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTGetAPDUStatus                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get APDU Status for spezified provider or consumer        +*/
/*+               For a consumer there may be no status present, because no +*/
/*+               frame was received yet.                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
static LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTGetAPDUStatus(
        EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv;
    EDDS_CSRT_CONSUMER_TYPE_PTR pCons;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR pConsFrame;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTGetAPDUStatus(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
            pRQB, pDDB, pHDB);

    Status      = EDD_STS_ERR_PARAM;
    pRqbParam   = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE) pRQB->pParam;
    pCSRTComp   = pDDB->pSRT;

    switch (pRqbParam->Type) {
    case EDD_CSRT_TYPE_PROVIDER:
        if (pRqbParam->ID < pCSRTComp->Cfg.ProviderCnt) {
            pProv = &pCSRTComp->Prov.pProvTbl[pRqbParam->ID];
            if ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED != pProv->Type) {
                pRqbParam->Present = EDD_CSRT_APDU_STATUS_PRESENT;

                if (pProv->Type == EDDS_CSRT_PROVIDER_TYPE_XRT)
                    pRqbParam->APDUStatus.CycleCnt =
                            (LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter)
                                    << pCSRTComp->CycleStampAdjustShift);
                else
                    pRqbParam->APDUStatus.CycleCnt =
                            (LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter)
                                    << (pCSRTComp->CycleStampAdjustShift - 5));

                pRqbParam->APDUStatus.DataStatus =
                        (((pProv->DataStatus
                                | pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus)
                                & EDD_CSRT_DSTAT_WIN_MASK)
                                | (pProv->DataStatus
                                        & pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus));
                pRqbParam->APDUStatus.TransferStatus = 0;
                Status = EDD_STS_OK;
            }
        }
        break;
    case EDD_CSRT_TYPE_CONSUMER:
        if (pRqbParam->ID < pCSRTComp->Cons.ConsumerCnt) {

            pCons = &pCSRTComp->Cons.pConsTbl[pRqbParam->ID];
            pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[pRqbParam->ID];

            if ( EDDS_CSRT_CONSUMER_TYPE_NOT_USED != pCons->Type) {
                /* Only if we already received a valid Consumer frame */
                /* State is set within ISR-Handler on receiving the   */
                /* first valid frame.                                 */
                if (pConsFrame->State >= EDDS_SRT_CONS_STATE_ON_3) {
                    pRqbParam->Present = EDD_CSRT_APDU_STATUS_PRESENT;

                    /* Because this sequence runs in RQB context and must not */
                    /* be interrupted by scheduler we have to use EDDS_ENTER */

                    EDDS_ENTER(pDDB->hSysDev);

                    pRqbParam->APDUStatus.CycleCnt = pConsFrame->CycleCounter;

                    pRqbParam->APDUStatus.DataStatus = pConsFrame->DataStatus;
                    EDDS_EXIT(pDDB->hSysDev);

                    pRqbParam->APDUStatus.TransferStatus = 0;
                    Status = EDD_STS_OK;
                } else /* no valid frame received yet */
                {
                    pRqbParam->Present = EDD_CSRT_APDU_STATUS_NOT_PRESENT;
                    pRqbParam->APDUStatus.CycleCnt = 0;
                    pRqbParam->APDUStatus.DataStatus = 0;
                    pRqbParam->APDUStatus.TransferStatus = 0;
                    Status = EDD_STS_OK;
                }
            }
        }
        break;
    default:
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                "EDDS_RTGetAPDUStatus(): Invalid Type (0x%X)!",
                pRqbParam->Type);
        break;
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTGetAPDUStatus(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTSendclockChange                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_SENDCLOCK_CHANGE request.                 +*/
/*+               We cant change sendclock so we only check if the desired  +*/
/*+               sendclock matches the current one.                        +*/
/*+               Changing the limits not supported and not checked.        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB - pRqbParam taken from pDDB (taken from pHDB)
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTSendclockChange(
        EDD_UPPER_RQB_PTR_TYPE pRQB, EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE pRqbParam;
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pCSRTComp;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT16 ShiftValue = 0;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTSendclockChange(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
            pRQB, pDDB, pHDB);

    Status = EDD_STS_OK;
    pRqbParam = (EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE) pRQB->pParam;

    pCSRTComp = pDDB->pSRT;

    if ( LSA_HOST_PTR_ARE_EQUAL(pRqbParam, LSA_NULL))
    {

        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
             "EDDS_RTSendclockChange: Invalid parameter pRqbParam %d",
             pRqbParam);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        switch(pRqbParam->CycleBaseFactor)
        {
        case 0:
            // Do nothing
            break;
        case 32:
        case 64:
        case 128:
        case 256:
        case 512:
        case 1024:
            ShiftValue = EDDS_RTCalculateCycleStampAjdustShift(pRqbParam->CycleBaseFactor);
            break;

        default:
            EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                 "EDDS_RTSendclockChange: Invalid parameter pRqbParam %d",
                 pRqbParam);
            Status = EDD_STS_ERR_PARAM;
            break;
        }

        if ((EDD_STS_OK == Status) &&
            (0 			!= pRqbParam->CycleBaseFactor) )
        {
            if ((0 != pCSRTComp->Cons.ConsumerActCnt) ||
                      (0 != pCSRTComp->Prov.ProviderActCnt))
            {
                  EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                      "EDDS_RTSendclockChange: There are consumer or provider active. ConsumerActCnt %d, ProviderActCnt %d",
                      pCSRTComp->Cons.ConsumerActCnt,
                      pCSRTComp->Prov.ProviderActCnt);
                  Status = EDD_STS_ERR_SEQUENCE;
            }

            if ( (pRqbParam->CycleBaseFactor / 32) < pDDB->pGlob->schedulerCycleIO)
            {
                  EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                      "EDDS_RTSendclockChange: Invalid parameter CycleBaseFactor %d is smaller than schedulerCycle %d",
                      pRqbParam->CycleBaseFactor,
                      pDDB->pGlob->schedulerCycleIO);
                  Status = EDD_STS_ERR_PARAM;
            }

            if ((EDD_STS_OK == Status) && (0 != ShiftValue) )
            {
                  pCSRTComp->Cfg.CycleBaseFactor = pRqbParam->CycleBaseFactor;

                  pCSRTComp->CycleStampAdjustShift = ShiftValue;

                  // scheduler cycle
                  pCSRTComp->schedulerModuloFactorIO = EDDS_RTCalculateSchedulerReduction(pRqbParam->CycleBaseFactor, pDDB->pGlob->schedulerCycleIO);
            }
        }
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTSendclockChange(), Status: 0x%X", Status);

    LSA_UNUSED_ARG(TraceIdx);

    return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTRequestCyclic                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all cSRT-Requests with OPCODE-Type  +*/
/*+               EDD_OPC_REQUEST and service EDDS_SRV_SRT_xxxx.            +*/
/*+                                                                         +*/
/*+               If we have a parameter error we will set EDD_STS_ERR_PARAM+*/
/*+               and call the handle-callbackfunction and abort the request+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SERVICE is set if we get an invalid Service.  +*/
/*+               EDD_STS_ERR_CHANNEL_USE is set if the service is invalid  +*/
/*+               for this channel (i.e. not used by this channel)          +*/
/*+                                                                         +*/
/*+               If the request is valid, it is queued or executed.        +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                     SRT-Component already initialized (not checked)     +*/
/*+                     Channel uses this component (not checked)           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pRQB or pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTRequestCyclic(EDD_UPPER_RQB_PTR_TYPE pRQB,
        EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    LSA_RESULT Status;
    LSA_BOOL Indicate;
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :EDDS_RTRequestCyclic(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    Indicate = LSA_TRUE;
    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /* Alle requests need an param-ptr, so check it.                             */
    /*---------------------------------------------------------------------------*/
    if (LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL)) {
        EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                ">>> SRT-Request: Service: 0x%X, RQB->pParam is NULL",
                EDD_RQB_GET_SERVICE(pRQB));
        Status = EDD_STS_ERR_PARAM;
    } else {

        /*----------------------------------------------------------------*/
        /* If no providers or consumers are configured                    */
        /* then we do not do any requests but will                        */
        /* respond with error                                             */
        /*----------------------------------------------------------------*/
        if (! pDDB->pGlob->IsSrtUsed )
        {
            EDDS_CRT_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    ">>> SRT-Request: No providers and no consumers configured! SRT services are not allowed!");
            Status = EDD_STS_ERR_SERVICE;
        }
        else
        {
#if (EDDS_CFG_TRACE_MODE != 0)
            EDDS_RTTraceRequest(pRQB, pHDB);
#endif

            switch (EDD_RQB_GET_SERVICE(pRQB)) {
            /*-------------------------------------------------------------------*/
            /* Provide indication resources                                      */
            /* On success the rqb will stay within edds!                         */
            /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_IND_PROVIDE :
                Status = EDDS_RTIndicationProvide(pRQB, pHDB);
                if (Status == EDD_STS_OK)
                    Indicate = LSA_FALSE;
                break;
                /*-------------------------------------------------------------------*/
                /* set provider state                                                */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_PROVIDER_SET_STATE :
                Status = EDDS_RTProviderSetState(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* add a provider                                                    */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_PROVIDER_ADD :
                Status = EDDS_RTProviderADD(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* control provider                                                  */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_PROVIDER_CONTROL :
                Status = EDDS_RTProviderControl(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* remove  provider                                                  */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_PROVIDER_REMOVE :
                Status = EDDS_RTProviderRemove(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* add consumer                                                      */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_CONSUMER_ADD :
                Status = EDDS_RTConsumerAdd(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* control consumer                                                  */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_CONSUMER_CONTROL :
                Status = EDDS_RTConsumerControl(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* remove consumer                                                   */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_CONSUMER_REMOVE :
                Status = EDDS_RTConsumerRemove(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* provider change phase                                             */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE :
                Status = EDDS_RTProviderChangePhase(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* get apdu-status                                                   */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_GET_APDU_STATUS :
                Status = EDDS_RTGetAPDUStatus(pRQB, pHDB);
                break;
                /*-------------------------------------------------------------------*/
                /* Unknown/Unsupported Request                                       */
                /*-------------------------------------------------------------------*/
            case EDD_SRV_SRT_PROVIDER_XCHANGE_BUF :
            case EDD_SRV_SRT_CONSUMER_XCHANGE_BUF :
            default:
                EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        ">>> EDDS_RTRequestCyclic: Invalid Service (pRQB: 0x%X, Service: 0x%X)",
                        pRQB, EDD_RQB_GET_SERVICE(pRQB));
                Status = EDD_STS_ERR_SERVICE;
                break;

            } /* switch */
        }
    } /* if */

    if (Indicate) {
        EDDS_RequestFinish(pHDB, pRQB, Status);
    }

    EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:EDDS_RTRequestCyclic(), Status: 0x%X", EDD_STS_OK);

    LSA_UNUSED_ARG(TraceIdx);

}

#if (EDDS_CFG_TRACE_MODE != 0)
/******************************************************************************
 *  Function:    EDDS_RTTraceRequest()  //for Tracing only
 *
 *  Description: Only Creates TraceEntries for some Services.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static LSA_VOID  EDDS_LOCAL_FCT_ATTR  EDDS_RTTraceRequest(	EDD_UPPER_RQB_PTR_TYPE 	pRQB,
                        EDDS_LOCAL_HDB_PTR_TYPE pHDB)
{
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    LSA_UINT32 TraceIdx;

    pDDB = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    /* param must be not NULL */
    if (! LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
            case EDD_SRV_SRT_IND_PROVIDE:
            {
                EDDS_CRT_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_IND_PROVIDE(pRQB:0x%X)", pRQB);
                break;
            }

            case EDD_SRV_SRT_PROVIDER_ADD:
            {
                EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE const pProviderAdd = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_PROVIDER_ADD(pRQB:0x%X): FrameID:0x%X Properties:0x%X",
                                  pRQB, pProviderAdd->FrameID, pProviderAdd->Properties);
                EDDS_CRT_TRACE_08(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_PROVIDER_ADD(pRQB:0x%X): Phase:0x%X PhaseSequence:0x%X ReductionRatio:0x%X DataLen:0x%X GroupID:0x%X UserID:0x%X CRNumber:0x%X",
                                  pRQB, pProviderAdd->CyclePhase, pProviderAdd->CyclePhaseSequence, pProviderAdd->CycleReductionRatio,
                                  pProviderAdd->DataLen, pProviderAdd->GroupID, pProviderAdd->UserID,
                                  pProviderAdd->IOParams.CRNumber);
                break;
            }

            case EDD_SRV_SRT_PROVIDER_REMOVE:
            {
                EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE  const  pProviderRemove = (EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_PROVIDER_REMOVE(pRQB:0x%X): ProviderID:0x%X", pRQB, pProviderRemove->ProviderID);
                break;
            }

            case EDD_SRV_SRT_PROVIDER_CONTROL:
            {
                EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE const pProviderControl = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_PROVIDER_CONTROL(pRQB:0x%X): ProviderID:0x%X Mode:0x%X",
                                  pRQB, pProviderControl->ProviderID, pProviderControl->Mode);

                if(EDD_PROV_CONTROL_MODE_ACTIVATE & pProviderControl->Mode)
                {
                    EDDS_CRT_TRACE_06(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_PROVIDER_CONTROL(pRQB:0x%X): FrameID: 0x%X Phase: 0x%X PhaseSequence:0x%X ReductionRatio:0x%X CRNumber:0x%X",
                                      pRQB, pProviderControl->FrameID, pProviderControl->CyclePhase, pProviderControl->CyclePhaseSequence,
                                      pProviderControl->CycleReductionRatio, pProviderControl->IOParams.CRNumber);
                }
                if(EDD_PROV_CONTROL_MODE_STATUS & pProviderControl->Mode)
                {
                    EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_PROVIDER_CONTROL(pRQB:0x%X): Mask:0x%X Status:0x%X",
                                      pRQB, pProviderControl->Mask, pProviderControl->Status);
                }
                if(EDD_PROV_CONTROL_MODE_AUTOSTOP & pProviderControl->Mode)
                {
                    EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_PROVIDER_CONTROL(pRQB:0x%X): AutoStopConsumerID:0x%X",
                                      pRQB, pProviderControl->AutoStopConsumerID);
                }
                break;
            }

            case EDD_SRV_SRT_PROVIDER_SET_STATE:
            {
                EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE const pProvSetState = (EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_PROVIDER_SET_STATE(pRQB:0x%X): GroupID:0x%X Status:0x%X Mask:0x%X",
                                      pRQB, pProvSetState->GroupID, pProvSetState->Status, pProvSetState->Mask);
                break;
            }

            case EDD_SRV_SRT_PROVIDER_CHANGE_PHASE:
            {
                EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE const pProvSetState = (EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_PROVIDER_CHANGE_PHASE(pRQB:0x%X): ProviderID:0x%X Phase:0x%X PhaseSequence:0x%X",
                                      pRQB, pProvSetState->ProviderID, pProvSetState->CyclePhase, pProvSetState->CyclePhaseSequence);
                break;
            }

            case EDD_SRV_SRT_CONSUMER_ADD:
            {
                EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE const pConsumerAdd = (EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_CONSUMER_ADD(pRQB:0x%X): FrameID:0x%X Properties:0x%X",
                                  pRQB, pConsumerAdd->FrameID, pConsumerAdd->Properties);
                EDDS_CRT_TRACE_08(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_CONSUMER_ADD(pRQB:0x%X): Phase:0x%X ReductionRatio:0x%X DataLen:0x%X DHF:0x%X UserID:0x%X CRNumber:0x%X ClearOnMISS:0x%X",
                                  pRQB, pConsumerAdd->CyclePhase, pConsumerAdd->CycleReductionRatio,
                                  pConsumerAdd->DataLen, pConsumerAdd->DataHoldFactor, pConsumerAdd->UserID,
                                  pConsumerAdd->IOParams.CRNumber, pConsumerAdd->IOParams.ClearOnMISS);
                break;
            }

            case EDD_SRV_SRT_CONSUMER_REMOVE:
            {
                EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE const pConsumerRemove = (EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_CONSUMER_REMOVE(pRQB:0x%X): ConsumerID:0x%X", pRQB, pConsumerRemove->ConsumerID);
                break;
            }

            case EDD_SRV_SRT_CONSUMER_CONTROL:
            {
                EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE const pConsumerControl = (EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_CONSUMER_CONTROL(pRQB:0x%X): ConsumerID:0x%X Mode:0x%X",
                                  pRQB, pConsumerControl->ConsumerID, pConsumerControl->Mode);

                if(EDD_CONS_CONTROL_MODE_ACTIVATE & pConsumerControl->Mode)
                {
                    EDDS_CRT_TRACE_05(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      ">>> Request SRT_CONSUMER_CONTROL(pRQB:0x%X): Phase:0x%X ReductionRatio:0x%X CRNumber:0x%X ClearOnMISS:0x%X",
                                      pRQB, pConsumerControl->CyclePhase, pConsumerControl->CycleReductionRatio,
                                      pConsumerControl->IOParams.CRNumber, pConsumerControl->IOParams.ClearOnMISS);
                }
                break;
            }

            case EDD_SRV_SRT_GET_APDU_STATUS:
            {
                EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE const pGetApdu = (EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE)pRQB->pParam;

                EDDS_CRT_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,
                                  ">>> Request SRT_GET_APDU_STATUS(pRQB:0x%X): Type:0x%X ID:0x%X",
                                  pRQB, pGetApdu->Type, pGetApdu->ID);
                break;
            }

            default:
                break;
        }
    }
    LSA_UNUSED_ARG(TraceIdx);
}
#endif /* if (EDDS_CFG_TRACE_MODE != 0) */

/**
*  \brief        \b EDDS_RTCalculateCycleStampAjdustShift.
*  _____
*
*  \details      Determine adjust shift value for calculating the CycleCounter on runtime. \n\n
*                [IN] LSA_UINT16 CycleBaseFactor - Current CycleBaseFactor for which the shift value should be calculated for. \n
*
*  \note         Every I/O cycle (see CBF) is counted. The current CycleCounter is calcualted with "count << CycleStampAdjustShift". \n
*                By this function a valid CycleBaseFactor is expected.
*
*  \author       XXX
*  \date         2015-XX-XX
*/
LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTCalculateCycleStampAjdustShift( LSA_UINT16 CycleBaseFactor)
{
    /* CBF    ShiftValue */
    /* 32        5       */
    /* 64        6       */
    /* 128       7       */
    /* 256       8       */
    /* 512       9       */
    /* 1024      10      */

    LSA_UINT16 CycleStampAdjustShift = 0;

    while (!(CycleBaseFactor & 0x1))
    {
        CycleStampAdjustShift++;
        CycleBaseFactor >>= 1;
    }

    return CycleStampAdjustShift;
}

/**
*  \brief        \b EDDS_RTCalculateSchedulerReduction.
*  _____
*
*  \details      Calculate the scheduler reduction for a CycleBaseFactor depending on the current scheduler cycle. \n\n
*                [IN] LSA_UINT16 CycleBaseFactor - Current CycleBaseFactor. \n
*                [IN] LSA_UINT16 schedulerCycle  - Current scheduler cycle. \n
*
*  \note         CycleBaseFactor is a multiple of 31,25us. \n
*
*  \author       XXX
*  \date         2015-XX-XX
*/
LSA_UINT16 EDDS_LOCAL_FCT_ATTR EDDS_RTCalculateSchedulerReduction( LSA_UINT16 CycleBaseFactor, LSA_UINT16 schedulerCycle)
{
    return (CycleBaseFactor / (32 * schedulerCycle));
}

/*****************************************************************************/
/*  end of file SRT_USR.C                                                    */
/*****************************************************************************/

