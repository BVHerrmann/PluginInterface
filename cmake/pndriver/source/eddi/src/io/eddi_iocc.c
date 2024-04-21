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
/*  F i l e               &F: eddi_io_iso.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Functions for IOCC support                                               */
/*                                                                           */
/*****************************************************************************/

#include "eddi_iocc_int.h"

#define EDDI_IOCC_MODULE_ID     M_ID_IO_IOCC
#define LTRC_ACT_MODUL_ID  510

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_IO_IOCC) //satisfy lint!
#endif

#if defined (EDDI_CFG_SOC)

/*===========================================================================*/
/*                              Global Vars                                  */
/*===========================================================================*/
static EDDI_IOCC_INFO_TYPE  * g_pEDDI_IOCC_Info;

/*===========================================================================*/
/*                              Local Protos                                 */
/*===========================================================================*/
static LSA_RESULT  eddi_IOCC_InstanceInit(EDDI_IOCC_CHANNEL_CB_TYPE * pChannel);
static LSA_RESULT eddi_IOCC_StartRead(EDDI_IOCC_CHANNEL_CB_TYPE * const   pChannel,
                                      LSA_UINT32                  const   DataLength,
                                      LSA_UINT32                  const   LLPhysAddr,
                                      LSA_UINT32                  const   P_Dest,
                                      LSA_UINT16                  const   NrOfLLLines);
static LSA_RESULT eddi_IOCC_StartWrite(EDDI_IOCC_CHANNEL_CB_TYPE * const   pChannel,
                                       LSA_UINT32                  const   DataLength,
                                       LSA_UINT32                  const   LLPhysAddr,
                                       LSA_UINT16                  const   NrOfLLLines);


/*===========================================================================*/
/*                              Local Vars                                   */
/*===========================================================================*/

/*===========================================================================*/
/*                              static functions                             */
/*===========================================================================*/
static LSA_VOID eddi_IOCC_MemcopyDstAligned (LSA_UINT8 * const pDst, LSA_UINT8 * const pSrc, LSA_UINT32 const DataLength)
{
    if ((LSA_UINT32)pSrc & 0x3)
    {
        //src is not aligned :(
        LSA_UINT32  LenLocal = DataLength;
        LSA_UINT8 * pDstLocalu8 = pDst; //Always 32bit aligned
        LSA_UINT8 * pSrcLocalu8 = pSrc;
        while (LenLocal)
        {
            *pDstLocalu8++ = *pSrcLocalu8++;
            LenLocal--;
        }
    }
    else
    {
        //src is aligned
        LSA_UINT32  LenLocal = DataLength>>2;
        LSA_UINT32 * pDstLocalu32 = (LSA_UINT32 *)((LSA_VOID *)pDst); //Always 32bit aligned
        LSA_UINT32 * pSrcLocalu32 = (LSA_UINT32 *)((LSA_VOID *)pSrc);

        while (LenLocal)
        {
            *pDstLocalu32++ = *pSrcLocalu32++;
            LenLocal--;
        }

        {
            //Some compilers(e.g. GCC) do not condone casting
            LSA_UINT8 * pDstLocalu8 = (LSA_UINT8 *)pDstLocalu32;
            LSA_UINT8 * pSrcLocalu8 = (LSA_UINT8 *)pSrcLocalu32;
            switch (DataLength & 0x3)
            {
                case 3:
                {
                    *pDstLocalu8++ = *pSrcLocalu8++;
                }
                //lint -fallthrough
                case 2:
                {
                    *pDstLocalu8++ = *pSrcLocalu8++;
                }
                //lint -fallthrough
                case 1:
                {
                    *pDstLocalu8 = *pSrcLocalu8;
                    break;
                }
                default: break;
            }
        }
    }
}

/*===========================================================================*/
/*                              Public functions                             */
/*===========================================================================*/
/**************************************************************************/
/* eddi_IOCC_init                                                         */
/* see prototype for description                                          */
/**************************************************************************/
LSA_VOID eddi_IOCC_Init(LSA_VOID)
{
    LSA_UINT8 *  pMem;
    LSA_UINT8   Channel;
    LSA_UINT32  i;
    

    //allocate memory for all instances
    EDDI_IOCC_ALLOC_LOCAL_MEM((LSA_VOID * *)&pMem, sizeof(EDDI_IOCC_INFO_TYPE));
    if (pMem)
    {
        g_pEDDI_IOCC_Info = (EDDI_IOCC_INFO_TYPE *)((LSA_VOID *)pMem);

        //clear all
        for (i=0; i<sizeof(EDDI_IOCC_INFO_TYPE); i++)
        {
            *pMem++ = 0;
        }

        //init necessary parts only
        g_pEDDI_IOCC_Info->Excp.Error = EDDI_IOCC_FATAL_NO_ERROR;
        for (i = 0; i < EDDI_IOCC_CFG_MAX_INSTANCES; i++)
        {
            g_pEDDI_IOCC_Info->Instance[i].bUsed = LSA_FALSE;
            for (Channel = 0; Channel < EDDI_IOCC_INT_CFG_IOCC_CHANNELS; Channel++)
            {
                g_pEDDI_IOCC_Info->Instance[i].Channel[Channel].IOCCHandle = (EDDI_IOCC_LOWER_HANDLE_TYPE *)0;
                g_pEDDI_IOCC_Info->Instance[i].Channel[Channel].pInstance  = &(g_pEDDI_IOCC_Info->Instance[i]);
                g_pEDDI_IOCC_Info->Instance[i].Channel[Channel].ChannelIdx = Channel;
            }
        }
    }
    else
    {
        EDDI_IOCC_Excp("eddi_IOCC_Init -> no memory", EDDI_IOCC_FATAL_ERR_EXCP, 0, 0);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_Deinit                                                       */
/* see prototype for description                                          */
/**************************************************************************/
LSA_VOID eddi_IOCC_Deinit(LSA_VOID)
{
    LSA_UINT16  RetVal;

    //free instance memory
    EDDI_IOCC_FREE_LOCAL_MEM(&RetVal, (LSA_VOID *)g_pEDDI_IOCC_Info);
    if (EDD_STS_OK != RetVal)
    {
        EDDI_IOCC_Excp("eddi_IOCC_Deinit -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_Setup                                                        */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_Setup( EDDI_IOCC_LOWER_HANDLE_TYPE      * pIOCCHandle,
                            EDDI_IOCC_UPPER_HANDLE_TYPE        UpperHandle,
                            LSA_UINT32                 const   InstanceHandle,
                            LSA_UINT8                  const   IOCCChannel,
                            LSA_VOID                 * const   pIOCC,
                            LSA_UINT8                * const   pDestBase,
                            LSA_UINT32                 const   DestBasePhysAddr,
                            LSA_UINT8                * const   pExtLinkListBase,
                            LSA_UINT32                 const   ExtLinkListBasePhysAddr,
                            LSA_UINT8             *  * const   ppIntLinkListMem,
                            LSA_UINT32               * const   pIntLinkListMemSize,
                            LSA_UINT8             *  * const   ppWB,
                            LSA_UINT32                 const   MbxOffset,
                            EDDI_IOCC_ERROR_TYPE     * const   pErrorInfo)
{
    LSA_UINT8   Instance;
    LSA_UINT8   UseInstance = EDDI_IOCC_CFG_MAX_INSTANCES;
    EDDI_IOCC_INSTANCE_CB_PTR_TYPE  pInstance;
    EDDI_IOCC_CHANNEL_CB_TYPE     * pChannel;

    //some param checks
    if (   (0 == IOCCChannel) || (IOCCChannel > EDDI_IOCC_INT_CFG_IOCC_CHANNELS)
        || (0 != (MbxOffset & 3UL))
        || (0 == pErrorInfo)
        || (0 == pIOCC)
        || (0 == pDestBase)
        )
    {
        if (pErrorInfo)
        {
            EDDI_IOCC_FILL_ERROR_INFO(pErrorInfo, IOCCChannel, MbxOffset, pIOCC, pDestBase);
        }
        return (EDD_STS_ERR_PARAM);
    }
    else if (   (0 == pExtLinkListBase)
                || (0 == ppIntLinkListMem)
                || (0 == pIntLinkListMemSize)
            )
    {
        EDDI_IOCC_FILL_ERROR_INFO(pErrorInfo, pExtLinkListBase, ppIntLinkListMem, pIntLinkListMemSize, 0);
        return (EDD_STS_ERR_PARAM);
    }

    //search instance (free or used)
    for (Instance = 0; Instance < EDDI_IOCC_CFG_MAX_INSTANCES; Instance++)
    {
        if (!g_pEDDI_IOCC_Info->Instance[Instance].bUsed
            && (EDDI_IOCC_CFG_MAX_INSTANCES == UseInstance))
        {
            //first free instance found
            UseInstance = Instance;
        }
        else if (InstanceHandle == g_pEDDI_IOCC_Info->Instance[Instance].InstanceHandle)
        {
            //already used instance found, only valid if used
            if (g_pEDDI_IOCC_Info->Instance[Instance].bUsed)
            {
                UseInstance = Instance;
                break;
            }
        }
    }

    if /* found something */
        (UseInstance < EDDI_IOCC_CFG_MAX_INSTANCES)
    {
        pInstance = &g_pEDDI_IOCC_Info->Instance[UseInstance];

        if /* channel still unused */
            (!pInstance->Channel[IOCCChannel-1].IOCCHandle)
        {
            LSA_RESULT  Retval;

            pChannel = &pInstance->Channel[IOCCChannel-1];

            //Try to initialize channel
            pChannel->DestBasePhysAddr  = DestBasePhysAddr;
            pChannel->LockCtr           = 0;
            pChannel->AddRemoveCtr      = 0;
            pChannel->MbxOffset         = MbxOffset;
            pChannel->pDestBase         = pDestBase;
            pChannel->pErrorInfo        = pErrorInfo;
            pChannel->pIOCC             = pIOCC;
            pChannel->UpperHandle       = UpperHandle;
            pChannel->bCopy2WB          = LSA_TRUE;

            //if an error occurs, error info is already filled in in eddi_IOCC_InstanceInit 
            Retval = eddi_IOCC_InstanceInit(pChannel);
            if (EDD_STS_OK == Retval)
            {
                pInstance->bUsed          = LSA_TRUE;                           //(re)claim instance
                pInstance->InstanceHandle = InstanceHandle;                     
                pChannel->IOCCHandle = (EDDI_IOCC_LOWER_HANDLE_TYPE)pChannel;   //claim channel
                *pIOCCHandle         = pChannel->IOCCHandle;                    //return lower handle

                //store additional params
                pChannel->ExtLinkListBasePhysAddr = ExtLinkListBasePhysAddr;
                pChannel->pExtLinkListBase        = pExtLinkListBase;

                *ppIntLinkListMem    = pInstance->pIntLinkListMem;
                *pIntLinkListMemSize = (EDDI_IOCC_HOST_LL_inst_ll_ram_end + 1 - EDDI_IOCC_HOST_LL_inst_ll_ram_start) - (4*4);

                if (ppWB)
                {
                    //Return address of WB
                    *ppWB = pChannel->pWB;
                    pChannel->bCopy2WB = LSA_FALSE;
                }
            }
            return (Retval);
        }
        else
        {
            EDDI_IOCC_FILL_ERROR_INFO(pErrorInfo, 0, 0, 0, 0);
            return (EDD_STS_ERR_RESOURCE);
        }
    }
    else
    {
        EDDI_IOCC_FILL_ERROR_INFO(pErrorInfo, 0, 0, 0, 0);
        return (EDD_STS_ERR_RESOURCE);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/**************************************************************************/
/* eddi_IOCC_Shutdown                                                     */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_Shutdown (EDDI_IOCC_LOWER_HANDLE_TYPE const IOCCHandle)
{
    //check handle
    if ((EDDI_IOCC_LOWER_HANDLE_TYPE)0 == IOCCHandle)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_Shutdown -> IOCCHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else
    {
        EDDI_IOCC_CHANNEL_CB_TYPE  * const pChannel = (EDDI_IOCC_CHANNEL_CB_TYPE *)IOCCHandle;
        
        if (pChannel->IOCCHandle != (EDDI_IOCC_LOWER_HANDLE_TYPE)IOCCHandle)
        {
            //corrupted handle
            EDDI_IOCC_Excp("eddi_IOCC_Shutdown -> IOCCHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
            return (EDD_STS_ERR_EXCP);
        }
        else
        {
            if /* channel shutdown under lock */
               (pChannel->LockCtr)
            {
                EDDI_IOCC_Excp("eddi_IOCC_Shutdown -> channel still locked", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, pChannel->LockCtr);
                return (EDD_STS_ERR_EXCP);
            }
            else
            {
                EDDI_IOCC_INSTANCE_CB_PTR_TYPE  const pInstance = pChannel->pInstance;
                
                //free channel
                pChannel->IOCCHandle = (EDDI_IOCC_LOWER_HANDLE_TYPE)0;
                
                //check if last channel closed
                if ( ((EDDI_IOCC_LOWER_HANDLE_TYPE)0 == pInstance->Channel[0].IOCCHandle) && ((EDDI_IOCC_LOWER_HANDLE_TYPE)0 == pInstance->Channel[1].IOCCHandle) )
                {
                    //free instance
                    pInstance->bUsed = LSA_FALSE;
                }
                return (EDD_STS_OK);
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_CreateLinkList                                               */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_CreateLinkList(EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                    EDDI_IOCC_LL_HANDLE_TYPE     *         pLLHandle,
                                    LSA_UINT8                      const   LinkListType, 
                                    LSA_UINT8                      const   LinkListMode, 
                                    LSA_UINT16                     const   NrOfLines)
{
    EDDI_IOCC_LINKLIST_CB_TYPE      * pLinkListCB;
    EDDI_IOCC_LINKLIST_ELEMENT_TYPE * pLinkListElements;
    LSA_UINT32                      * pLinkList;
    LSA_UINT16                        RetVal;

    //check handle
    if ((EDDI_IOCC_LOWER_HANDLE_TYPE)0 == IOCCHandle)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_CreateLinkList -> IOCCHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else
    {
        EDDI_IOCC_CHANNEL_CB_TYPE  * const pChannel = (EDDI_IOCC_CHANNEL_CB_TYPE *)IOCCHandle;
        LSA_UINT16                         i;
        
        if (pChannel->IOCCHandle != (EDDI_IOCC_LOWER_HANDLE_TYPE)IOCCHandle)
        {
            //corrupted handle
            EDDI_IOCC_Excp("eddi_IOCC_CreateLinkList -> IOCCHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
            return (EDD_STS_ERR_EXCP);
        }

        //check params
        if (   ((LinkListType != EDDI_IOCC_LINKLIST_TYPE_INTERNAL) && (LinkListType != EDDI_IOCC_LINKLIST_TYPE_EXTERNAL))
            || ((LinkListMode != EDDI_IOCC_LINKLIST_MODE_READ) && (LinkListMode != EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE) && (LinkListMode != EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP))
            || (NrOfLines > EDDI_IOCC_MAX_NR_LINES) 
            || (0 == NrOfLines)
            || ((EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE == LinkListMode) && !pChannel->bCopy2WB)
            )
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, LinkListType, LinkListMode, NrOfLines, pChannel->bCopy2WB);
            return (EDD_STS_ERR_PARAM);
        }

        //allocate memory
        EDDI_IOCC_ALLOC_LOCAL_MEM((LSA_VOID * *)&pLinkListCB, sizeof(EDDI_IOCC_LINKLIST_CB_TYPE));
        if (!pLinkListCB)
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, 0, 0, 0, 0);
            return (EDD_STS_ERR_RESOURCE);
        }

        //Fill elements
        pLinkListCB->LinkListMode = LinkListMode;
        pLinkListCB->LinkListType = LinkListType;
        pLinkListCB->NrOfLines    = NrOfLines;
        pLinkListCB->NrOfUsedLines = 0;
        pLinkListCB->DataLengthAll = 0;
        pLinkListCB->pChannel     = pChannel;
        pLinkListCB->LLHandle     = (EDDI_IOCC_LL_HANDLE_TYPE)pLinkListCB;
    
        //Allocate LinkList elements
        EDDI_IOCC_ALLOC_LOCAL_MEM((LSA_VOID * *)&pLinkListElements, NrOfLines*((sizeof(EDDI_IOCC_LINKLIST_ELEMENT_TYPE)+3)&(~0x3UL)));
        if (!pLinkListElements)
        {
            EDDI_IOCC_FREE_LOCAL_MEM(&RetVal, (LSA_VOID *)pLinkListCB); //Cleanup: Free CB
            if (EDD_STS_OK != RetVal)
            {
                EDDI_IOCC_Excp("eddi_IOCC_CreateLinkList -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
                return (EDD_STS_ERR_EXCP);
            }
            else
            {
                EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, 0, 0, 0, 0);
                return (EDD_STS_ERR_RESOURCE);
            }
        }

        //Allocate LinkList
        EDDI_IOCC_ALLOC_LINKLIST_MEM(pChannel->UpperHandle, (LSA_VOID * *)&pLinkList, NrOfLines*4, LinkListType);
        if (!pLinkList)
        {
            EDDI_IOCC_FREE_LOCAL_MEM(&RetVal, (LSA_VOID *)pLinkListCB); //Cleanup: Free CB
            if (EDD_STS_OK != RetVal)
            {
                EDDI_IOCC_Excp("eddi_IOCC_CreateLinkList -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
                return (EDD_STS_ERR_EXCP);
            }
            else
            {
                EDDI_IOCC_FREE_LOCAL_MEM(&RetVal, (LSA_VOID *)pLinkListElements); //Cleanup: Free elements
                if (EDD_STS_OK != RetVal)
                {
                    EDDI_IOCC_Excp("eddi_IOCC_CreateLinkList -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
                    return (EDD_STS_ERR_EXCP);
                }
                else
                {
                    EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, 0, 0, 0, 0);
                    return (EDD_STS_ERR_RESOURCE);
                }
            }
        }

        //Initialize LinkList
        pLinkListCB->pLinkListElements = pLinkListElements; //1 element is 1 administrative data set for 1 LL line
        pLinkListCB->pLinkList         = pLinkList;
        pLinkListCB->pDest             = (LSA_UINT8 *)0xFDFDFDFDUL;
        pLinkListCB->DestPhysAddr      = 0xFDFDFDFDUL;
        if (EDDI_IOCC_LINKLIST_TYPE_INTERNAL == LinkListType)
        {
            //internal LinkList
            pLinkListCB->LinkListPhysAddr = EDDI_IOCC_BASE_AHB + (LSA_UINT32)pLinkList - (LSA_UINT32)pChannel->pIOCC;
        }
        else
        {
            //external LinkList
            pLinkListCB->LinkListPhysAddr = pChannel->ExtLinkListBasePhysAddr + ((LSA_UINT32)pLinkList - (LSA_UINT32)pChannel->pExtLinkListBase);
        }

        for (i=0; i<NrOfLines; i++)
        {
            //associate LinkList lines to elements
            pLinkListElements->pLLLine = pLinkList;
            pLinkListElements++;
            pLinkList++;
        }
    }

    *pLLHandle = pLinkListCB->LLHandle;

    return (EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_CreateLinkList                                               */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_DeleteLinkList(EDDI_IOCC_LL_HANDLE_TYPE const LLHandle)
{
    EDDI_IOCC_LINKLIST_CB_TYPE * const pLinkListCB = (EDDI_IOCC_LINKLIST_CB_TYPE *)LLHandle;
    LSA_UINT16                         RetVal;

    //check handle
    if ((EDDI_IOCC_LINKLIST_CB_TYPE *)0 == pLinkListCB)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_DeleteLinkList -> LLHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, LLHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else if (pLinkListCB->LLHandle != LLHandle)
    {
        //corrupted handle
        EDDI_IOCC_Excp("eddi_IOCC_DeleteLinkList -> LLHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, LLHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }

    //Free LinkList
    EDDI_IOCC_FREE_LINKLIST_MEM(pLinkListCB->pChannel->UpperHandle, &RetVal, (LSA_VOID *)pLinkListCB->pLinkList, pLinkListCB->LinkListType);
    if (EDD_STS_OK != RetVal)
    {
        EDDI_IOCC_Excp("eddi_IOCC_DeleteLinkList -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
        return (EDD_STS_ERR_EXCP);
    }

    //Free elements
    EDDI_IOCC_FREE_LOCAL_MEM(&RetVal, (LSA_VOID *)pLinkListCB->pLinkListElements);
    if (EDD_STS_OK != RetVal)
    {
        EDDI_IOCC_Excp("eddi_IOCC_DeleteLinkList -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
        return (EDD_STS_ERR_EXCP);
    }
    
    //Free CB
    EDDI_IOCC_FREE_LOCAL_MEM(&RetVal, (LSA_VOID *)pLinkListCB);
    if (EDD_STS_OK != RetVal)
    {
        EDDI_IOCC_Excp("eddi_IOCC_DeleteLinkList -> failure", EDDI_IOCC_FATAL_ERR_EXCP, RetVal, 0);
        return (EDD_STS_ERR_EXCP);
    }
    
    return (EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_AppendToLinkList                                             */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_AppendToLinkList(EDDI_IOCC_LL_HANDLE_TYPE     const   LLHandle,
                                      LSA_UINT32                   const   PAEARAMOffset,
                                      LSA_UINT16                   const   DataLength,
                                      LSA_UINT8                  * const   pMem,
                                      LSA_VOID                   * const   UserIOHandle)
{
    EDDI_IOCC_LINKLIST_CB_TYPE * const pLinkListCB = (EDDI_IOCC_LINKLIST_CB_TYPE *)LLHandle;
    LSA_UINT32                         LinkListLine;
    EDDI_IOCC_LINKLIST_ELEMENT_TYPE  * pLinkListElement;

    //check handle
    if ((EDDI_IOCC_LINKLIST_CB_TYPE *)0 == pLinkListCB)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_AppendToLinkList -> LLHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, LLHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else if (pLinkListCB->LLHandle != LLHandle)
    {
        //corrupted handle
        EDDI_IOCC_Excp("eddi_IOCC_AppendToLinkList -> LLHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, LLHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }

    pLinkListElement = pLinkListCB->pLinkListElements + pLinkListCB->NrOfUsedLines;

    //checks
    if (    (0 == DataLength)
         || (DataLength > EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE)
         || (PAEARAMOffset > (EDDI_IOCC_PA_EA_DIRECT_end - EDDI_IOCC_PA_EA_DIRECT_start)) )
    {
        EDDI_IOCC_FILL_ERROR_INFO(pLinkListCB->pChannel->pErrorInfo, 0, PAEARAMOffset, DataLength, 0);
        return(EDD_STS_ERR_PARAM);
    }
    else if (   (pLinkListCB->NrOfUsedLines == pLinkListCB->NrOfLines)
             || ((pLinkListCB->DataLengthAll + DataLength) > EDDI_IOCC_MAX_GROUP_TRANSFER_SIZE)
            )
    {
        EDDI_IOCC_FILL_ERROR_INFO(pLinkListCB->pChannel->pErrorInfo, 0, 0, DataLength, pLinkListCB->DataLengthAll);
        return(EDD_STS_ERR_RESOURCE);
    }
    else if (EDDI_IOCC_LINKLIST_MODE_READ == pLinkListCB->LinkListMode)
    {
        //pMem has to be in Dest range
        //All pMem have to be the same
        if (   ((LSA_UINT32)pMem < (LSA_UINT32)pLinkListCB->pChannel->pDestBase) 
            || ( (pLinkListCB->NrOfUsedLines) && (pMem != pLinkListCB->pDest))
           )
        {
            EDDI_IOCC_FILL_ERROR_INFO(pLinkListCB->pChannel->pErrorInfo, pMem, pLinkListCB->pChannel->pDestBase, pLinkListCB->pDest, 0);
            return(EDD_STS_ERR_PARAM);
        }
    }
    else if (EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP == pLinkListCB->LinkListMode)
    {
        //All pMem have to be the same
        if ((pLinkListCB->NrOfUsedLines) && (pMem != pLinkListCB->pLinkListElements->pSrc))
        {
            EDDI_IOCC_FILL_ERROR_INFO(pLinkListCB->pChannel->pErrorInfo, pMem, pLinkListCB->pLinkListElements->pSrc, 0, 0);
            return(EDD_STS_ERR_PARAM);
        }
    }

    //Try to get semaphore
    if (0 == pLinkListCB->pChannel->ChannelIdx)
    {
        EDDI_ENTER_IOCC_CH1_S(pLinkListCB->pChannel);
    }
    else
    {
        EDDI_ENTER_IOCC_CH2_S(pLinkListCB->pChannel);
    }
    
    if (pLinkListCB->pChannel->AddRemoveCtr)
    {
        //Lock problem
        EDDI_IOCC_Excp("eddi_IOCC_AppendToLinkList -> Lock problem", EDDI_IOCC_FATAL_ERR_EXCP, pLinkListCB->pChannel->AddRemoveCtr, 0);
        return (EDD_STS_ERR_EXCP);
    }
    pLinkListCB->pChannel->AddRemoveCtr++;

    if (EDDI_IOCC_LINKLIST_MODE_READ == pLinkListCB->LinkListMode)
    {
        if (0 == pLinkListCB->NrOfUsedLines)
        {
            pLinkListCB->pDest        = pMem;
            pLinkListCB->DestPhysAddr = pLinkListCB->pChannel->DestBasePhysAddr + (LSA_UINT32)(pMem - pLinkListCB->pChannel->pDestBase);  //phys dest address
        }
        pLinkListElement->pSrc    = (LSA_UINT8 *)0xFDFDFDFDUL; //sanity
    }
    else
    {
        pLinkListElement->pSrc = pMem;  //EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP: Only first one counts
    }

    //Create LL line
    EDDI_IOCC_SET_LL_ENTRY(&LinkListLine, PAEARAMOffset, DataLength);
    *(pLinkListElement->pLLLine)    = LinkListLine;
    pLinkListElement->UserIOHandle  = UserIOHandle;
    pLinkListElement->DataLength    = DataLength;
    pLinkListCB->NrOfUsedLines++;
    pLinkListCB->DataLengthAll     += DataLength; 

    pLinkListCB->pChannel->AddRemoveCtr--;
    if (0 == pLinkListCB->pChannel->ChannelIdx)
    {
        EDDI_EXIT_IOCC_CH1_S(pLinkListCB->pChannel);
    }
    else
    {
        EDDI_EXIT_IOCC_CH2_S(pLinkListCB->pChannel);
    }
    
    return (EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/

/**************************************************************************/
/* eddi_IOCC_RemoveFromLinkList                                           */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_RemoveFromLinkList(EDDI_IOCC_LL_HANDLE_TYPE     const   LLHandle,
                                        LSA_VOID         const     * const   UserIOHandle)
{
    EDDI_IOCC_LINKLIST_CB_TYPE * const pLinkListCB = (EDDI_IOCC_LINKLIST_CB_TYPE *)LLHandle;
    EDDI_IOCC_LINKLIST_ELEMENT_TYPE  * pLinkListElement;
    LSA_UINT16                         FoundIdx; 

    //check handle
    if ((EDDI_IOCC_LINKLIST_CB_TYPE *)0 == pLinkListCB)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_RemoveFromLinkList -> LLHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, LLHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else if (pLinkListCB->LLHandle != LLHandle)
    {
        //corrupted handle
        EDDI_IOCC_Excp("eddi_IOCC_RemoveFromLinkList -> LLHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, LLHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }

    //Search element
    pLinkListElement = pLinkListCB->pLinkListElements;
    for (FoundIdx = 0; FoundIdx < pLinkListCB->NrOfUsedLines; FoundIdx++)
    {
        if (pLinkListElement->UserIOHandle == UserIOHandle)
        {
            break;
        }
        pLinkListElement++;
    }

    if /* not found */
       (FoundIdx >= pLinkListCB->NrOfUsedLines)
    {
        EDDI_IOCC_FILL_ERROR_INFO(pLinkListCB->pChannel->pErrorInfo, UserIOHandle, pLinkListCB->NrOfUsedLines, 0, 0);
        return(EDD_STS_ERR_PARAM);
    }

    //Try to get semaphore
    if (0 == pLinkListCB->pChannel->ChannelIdx)
    {
        EDDI_ENTER_IOCC_CH1_S(pLinkListCB->pChannel);
    }
    else
    {
        EDDI_ENTER_IOCC_CH2_S(pLinkListCB->pChannel);
    }
    
    if (pLinkListCB->pChannel->AddRemoveCtr)
    {
        //Lock problem
        EDDI_IOCC_Excp("eddi_IOCC_AppendToLinkList -> Lock problem", EDDI_IOCC_FATAL_ERR_EXCP, pLinkListCB->pChannel->AddRemoveCtr, 0);
        return (EDD_STS_ERR_EXCP);
    }
    pLinkListCB->pChannel->AddRemoveCtr++;

    //adapt infos. if the last line is to be removed, than that is all
    if (   (0 == pLinkListCB->NrOfUsedLines)
        || (pLinkListCB->DataLengthAll < pLinkListElement->DataLength) )
    {
        if (0 == pLinkListCB->pChannel->ChannelIdx)
        {
            EDDI_EXIT_IOCC_CH1_S(pLinkListCB->pChannel);
        }
        else
        {
            EDDI_EXIT_IOCC_CH2_S(pLinkListCB->pChannel);
        }
        //corrupted data
        EDDI_IOCC_Excp("eddi_IOCC_RemoveFromLinkList -> NrOfUsedLines/DataLengthAll corrupted", EDDI_IOCC_FATAL_ERR_EXCP, pLinkListCB->NrOfUsedLines, pLinkListCB->DataLengthAll);
        return (EDD_STS_ERR_EXCP);
    }
    
    pLinkListCB->NrOfUsedLines--;
    pLinkListCB->DataLengthAll -= pLinkListElement->DataLength;
     
    if /* not last line */
       (FoundIdx < pLinkListCB->NrOfUsedLines)
    {
        EDDI_IOCC_LINKLIST_ELEMENT_TYPE  * pLinkListElementNext = pLinkListElement+1;

        //copy all elements from pLinkListElement+1 to end one up
        for (; FoundIdx < pLinkListCB->NrOfUsedLines; FoundIdx++)
        {
            //copy LinkList line contents
            *pLinkListElement->pLLLine = *pLinkListElementNext->pLLLine;    

            //copy the rest. The association "element<->line" has to stay untouched
            pLinkListElement->UserIOHandle  = pLinkListElementNext->UserIOHandle;
            pLinkListElement->DataLength    = pLinkListElementNext->DataLength;
            pLinkListElement->pSrc          = pLinkListElementNext->pSrc;
            pLinkListElement++;
            pLinkListElementNext++;
        }
    }

    pLinkListCB->pChannel->AddRemoveCtr--;
    if (0 == pLinkListCB->pChannel->ChannelIdx)
    {
        EDDI_EXIT_IOCC_CH1_S(pLinkListCB->pChannel);
    }
    else
    {
        EDDI_EXIT_IOCC_CH2_S(pLinkListCB->pChannel);
    }
    
    return (EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_SingleRead                                                   */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_SingleRead(EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                LSA_UINT32                     const   DataLength,
                                LSA_UINT32                     const   PAEARAMOffset,
                                LSA_UINT8                    * const   pDest)
{
    EDDI_IOCC_CHANNEL_CB_TYPE  * const pChannel = (EDDI_IOCC_CHANNEL_CB_TYPE *)IOCCHandle;

    #if !defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)
    //check handle
    if ((EDDI_IOCC_LOWER_HANDLE_TYPE)0 == IOCCHandle)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_SingleRead -> IOCCHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else
    {
        if (pChannel->IOCCHandle != (EDDI_IOCC_LOWER_HANDLE_TYPE)IOCCHandle)
        {
            //corrupted handle
            EDDI_IOCC_Excp("eddi_IOCC_SingleRead -> IOCCHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
            return (EDD_STS_ERR_EXCP);
        }
    }
    #endif //!defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)

    if /* DataLen valid */
        ((0 == DataLength)
            || (DataLength > EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE)
            || (PAEARAMOffset > (EDDI_IOCC_PA_EA_DIRECT_end - EDDI_IOCC_PA_EA_DIRECT_start))
            || ((LSA_UINT32)pDest < (LSA_UINT32)pChannel->pDestBase) )
    {
        EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, DataLength, PAEARAMOffset, pDest, pChannel->pDestBase);
        return(EDD_STS_ERR_PARAM);
    }
    else
    {
        LSA_UINT32  const P_Data = pChannel->DestBasePhysAddr + (LSA_UINT32)(pDest - pChannel->pDestBase);  //phys dest address
        LSA_RESULT  Result;
        //*** READ ***
        //direction is PAEA ==> RAM (read)

        if (0 == pChannel->ChannelIdx)
        {
            EDDI_ENTER_IOCC_CH1_S(pChannel);
        }
        else
        {
            EDDI_ENTER_IOCC_CH2_S(pChannel);
        }
        /* adapt linklist-entry */
        EDDI_IOCC_SET_LL_ENTRY(pChannel->pLLEntryRead, PAEARAMOffset, DataLength);
        pChannel->MbxVal++;
        Result = eddi_IOCC_StartRead(pChannel, DataLength, pChannel->LLEntryReadPhysAddr, P_Data /*P_Dest*/, 1 /*NrOfLLLines*/);
        /* Attention: EDDI_EXIT_IOCC_CHx_S is called in eddi_IOCC_StartRead !*/

        return (Result);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_SingleWrite                                                  */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_SingleWrite (EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                  LSA_UINT32                     const   DataLength, 
                                  LSA_UINT32                     const   PAEARAMOffset,
                                  LSA_UINT8                    * const   pSrc)
{
    EDDI_IOCC_CHANNEL_CB_TYPE  * const pChannel = (EDDI_IOCC_CHANNEL_CB_TYPE *)IOCCHandle;

    #if !defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)
    //check handle
    if ((EDDI_IOCC_LOWER_HANDLE_TYPE)0 == IOCCHandle)
    {
        //Nullpointer
        EDDI_IOCC_Excp("eddi_IOCC_SingleWrite -> IOCCHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
        return (EDD_STS_ERR_EXCP);
    }
    else
    {
        if (pChannel->IOCCHandle != (EDDI_IOCC_LOWER_HANDLE_TYPE)IOCCHandle)
        {
            //corrupted handle
            EDDI_IOCC_Excp("eddi_IOCC_SingleWrite -> IOCCHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, IOCCHandle, 0);
            return (EDD_STS_ERR_EXCP);
        }
    }
    #endif //!defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)

    if /* DataLen valid */
        ((0 == DataLength)
            || (DataLength > EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE)
            || (PAEARAMOffset > (EDDI_IOCC_PA_EA_DIRECT_end - EDDI_IOCC_PA_EA_DIRECT_start)))
    {
        EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, DataLength, PAEARAMOffset, 0, 0);
        return(EDD_STS_ERR_PARAM);
    }
    else
    {
        LSA_RESULT        Result;

        //*** WRITE ***
        //direction is WriteBuffer ==> PAEA (write)
        if (0 == pChannel->ChannelIdx)
        {
            EDDI_ENTER_IOCC_CH1_S(pChannel);
        }
        else
        {
            EDDI_ENTER_IOCC_CH2_S(pChannel);
        }
        /* adapt linklist-entry */
        EDDI_IOCC_SET_LL_ENTRY(pChannel->pLLEntryWrite, PAEARAMOffset, DataLength);

        //copy channel and fill params
        if (pChannel->bCopy2WB)
        {
            eddi_IOCC_MemcopyDstAligned (pChannel->pWB, (LSA_UINT8 *)pSrc, DataLength);
        }
        Result = eddi_IOCC_StartWrite(pChannel, DataLength, pChannel->LLEntryWritePhysAddr, 1 /*NrOfLLLines*/);
        /* Attention: EDDI_EXIT_IOCC_CHx_S is called in eddi_IOCC_StartWrite !*/

        return (Result);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_MultipleRead                                                 */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_MultipleRead (EDDI_IOCC_LL_HANDLE_TYPE             * pLLHandle,
                                   LSA_UINT32                     const   NrOfLinkLists)
{
    EDDI_IOCC_CHANNEL_CB_TYPE  * pChannel;
    EDDI_IOCC_LINKLIST_CB_TYPE * pLLCB;
    LSA_RESULT                   Result = EDD_STS_ERR_PARAM;
    LSA_UINT16                   CurrentLL;

    for (CurrentLL=0; CurrentLL < NrOfLinkLists; CurrentLL++ )
    {
        #if !defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)
        //check handle
        if ((EDDI_IOCC_LL_HANDLE_TYPE)0 == pLLHandle)
        {
            //Nullpointer
            EDDI_IOCC_Excp("eddi_IOCC_MultipleRead -> IOCCHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, pLLHandle, 0);
            return (EDD_STS_ERR_EXCP);
        }
        else
        {
            pLLCB    = (EDDI_IOCC_LINKLIST_CB_TYPE *)*pLLHandle;
            pChannel = pLLCB->pChannel;
            if (pLLCB->LLHandle != *pLLHandle)
            {
                //corrupted handle
                EDDI_IOCC_Excp("eddi_IOCC_MultipleRead -> IOCCHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, pLLCB, 0);
                return (EDD_STS_ERR_EXCP);
            }
            else if (   (EDDI_IOCC_LINKLIST_MODE_READ != pLLCB->LinkListMode)
                     || (0 == pLLCB->NrOfUsedLines) )
            {
                EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, pLLCB->LinkListMode, pLLCB->NrOfUsedLines, 0, 0);
                return(EDD_STS_ERR_PARAM);
            }
        }
        #else  //!defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)
        pLLCB    = (EDDI_IOCC_LINKLIST_CB_TYPE *)*pLLHandle;
        pChannel = pLLCB->pChannel;
        #endif //!defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)


        if /* Append or Remove operation is running */
           (pChannel->AddRemoveCtr)
        {
            return (EDD_STS_OK_PENDING);
        }

        //*** READ ***
        //direction is PAEA ==> RAM (read)
        if (0 == pChannel->ChannelIdx)
        {
            EDDI_ENTER_IOCC_CH1_S(pChannel);
        }
        else
        {
            EDDI_ENTER_IOCC_CH2_S(pChannel);
        }
        /* adapt linklist-entry */
        pChannel->MbxVal++;
        Result = eddi_IOCC_StartRead(pChannel, pLLCB->DataLengthAll, pLLCB->LinkListPhysAddr, pLLCB->DestPhysAddr, pLLCB->NrOfUsedLines);
        /* Attention: EDDI_EXIT_IOCC_CH1_S is called in eddi_IOCC_StartRead !*/
        if (EDD_STS_OK != Result)
        {
            break;
        }
        
        pLLHandle++;    //Choose next LinkList handle
    }
    return (Result);
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_MultipleWrite                                                */
/* see prototype for description                                          */
/**************************************************************************/
LSA_RESULT eddi_IOCC_MultipleWrite (EDDI_IOCC_LL_HANDLE_TYPE             * pLLHandle,
                                    LSA_UINT32                     const   NrOfLinkLists)
{
    EDDI_IOCC_CHANNEL_CB_TYPE  * pChannel;
    EDDI_IOCC_LINKLIST_CB_TYPE * pLLCB;
    LSA_RESULT                   Result = EDD_STS_ERR_PARAM;
    LSA_UINT16                   CurrentLL;

    for (CurrentLL=0; CurrentLL < NrOfLinkLists; CurrentLL++ )
    {
        #if !defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)
        //check handle
        if ((EDDI_IOCC_LL_HANDLE_TYPE)0 == pLLHandle)
        {
            //Nullpointer
            EDDI_IOCC_Excp("eddi_IOCC_MultipleWrite -> IOCCHandle is 0", EDDI_IOCC_FATAL_ERR_EXCP, pLLHandle, 0);
            return (EDD_STS_ERR_EXCP);
        }
        else
        {
            pLLCB    = (EDDI_IOCC_LINKLIST_CB_TYPE *)*pLLHandle;
            pChannel = pLLCB->pChannel;
            if (pLLCB->LLHandle != *pLLHandle)
            {
                //corrupted handle
                EDDI_IOCC_Excp("eddi_IOCC_MultipleWrite -> IOCCHandle corrupted", EDDI_IOCC_FATAL_ERR_EXCP, pLLCB, 0);
                return (EDD_STS_ERR_EXCP);
            }
            else if (   (EDDI_IOCC_LINKLIST_MODE_READ == pLLCB->LinkListMode)
                     || (0 == pLLCB->NrOfUsedLines) )
            {
                EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, pLLCB->LinkListMode, pLLCB->NrOfUsedLines, 0, 0);
                return(EDD_STS_ERR_PARAM);
            }
        }
        #else //(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)
        pLLCB    = (EDDI_IOCC_LINKLIST_CB_TYPE *)*pLLHandle;
        pChannel = pLLCB->pChannel;
        #endif //!defined(EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS)

        if /* Append or Remove operation is running */
           (pChannel->AddRemoveCtr)
        {
            return (EDD_STS_OK_PENDING);
        }

        //*** WRITE ***
        //direction is WriteBuffer ==> PAEA (write)
        if (0 == pChannel->ChannelIdx)
        {
            EDDI_ENTER_IOCC_CH1_S(pChannel);
        }
        else
        {
            EDDI_ENTER_IOCC_CH2_S(pChannel);
        }

        //copy channel and fill params
        //Attention: IOCC counts the written data and compares them to the value entered in EDDI_IOCC_HOST_LL_inst_hwp0_nmb_data/_hwp1_nmb_data
        if (EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP == pLLCB->LinkListMode)
        {
            if (pChannel->bCopy2WB)
            {
                //Copy all data at once to the WriteBuffer. pScr of the 1st LinkList element is the src (virt.).
                eddi_IOCC_MemcopyDstAligned (pChannel->pWB, (LSA_UINT8 *)pLLCB->pLinkListElements->pSrc, pLLCB->DataLengthAll);
            }
        }
        else
        {
            //Copy every IO object to the WriteBuffer
            LSA_UINT16  CurrentLine;
            LSA_UINT16  DataLength = 0;
            EDDI_IOCC_LINKLIST_ELEMENT_TYPE  * pCurrentLinkListElement = pLLCB->pLinkListElements;
            LSA_UINT8 * pWB = pChannel->pWB;             //virt WriteBuffer address

            for (CurrentLine = 0; CurrentLine < pLLCB->NrOfUsedLines; CurrentLine++)
            {
                {
                    LSA_UINT32  LenLocal = pCurrentLinkListElement->DataLength;
                    LSA_UINT8 * pSrcLocal = (LSA_UINT8 *)pCurrentLinkListElement->pSrc;
                    while (LenLocal)
                    {
                        *pWB++ = *pSrcLocal++;
                        LenLocal--;
                    }
                }
                DataLength += pCurrentLinkListElement->DataLength;
                //P_WB += pCurrentLinkListElement->DataLength /*Got incremented in loop!*/;
                pCurrentLinkListElement++;
            }
            if (DataLength != pLLCB->DataLengthAll)
            {
                if (0 == pChannel->ChannelIdx)
                {
                    EDDI_EXIT_IOCC_CH1_S(pChannel);
                }
                else
                {
                    EDDI_EXIT_IOCC_CH2_S(pChannel);
                }
                //corrupted administrative data
                EDDI_IOCC_Excp("eddi_IOCC_MultipleWrite -> DataLength != pLLCB->DataLengthAll", EDDI_IOCC_FATAL_ERR_EXCP, DataLength, pLLCB->DataLengthAll);
                return (EDD_STS_ERR_EXCP);
            }
        }

        Result = eddi_IOCC_StartWrite(pChannel, pLLCB->DataLengthAll, pLLCB->LinkListPhysAddr, pLLCB->NrOfUsedLines);
        /* Attention: EDDI_EXIT_IOCC_CHx_S is called in eddi_IOCC_StartWrite !*/
        if (EDD_STS_OK != Result)
        {
            break;
        }
        
        pLLHandle++;    //Choose next LinkList handle
    }

    return (Result);
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************/
/* eddi_IOCC_CalcDirectAccessAddress                                      */
/* see prototype for description                                          */
/**************************************************************************/
LSA_VOID * eddi_IOCC_CalcDirectAccessAddress (EDDI_IOCC_LOWER_HANDLE_TYPE   const   IOCCHandle,
                                              LSA_VOID                    *  const  pPAEARAM)
{
    //direct-access address = (offset to paearam-start)*4 + paearam-base-addr.
    LSA_UINT32 const P_PAEABase = (LSA_UINT32)(((EDDI_IOCC_CHANNEL_CB_TYPE *)IOCCHandle)->pPAEABase);
    LSA_UNUSED_ARG(IOCCHandle);

    return ( (LSA_VOID *)((((LSA_UINT32)pPAEARAM - P_PAEABase)<<2) + P_PAEABase) );
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*                              Local  functions                             */
/*===========================================================================*/
/*=============================================================================
* function name:  eddi_IOCC_InstanceInit()
*
* function:       initializes one complete IOCC instance
*
* return value:   EDD_STS_OK, EDD_STS_ERR_HW
*===========================================================================*/
static LSA_RESULT  eddi_IOCC_InstanceInit(EDDI_IOCC_CHANNEL_CB_TYPE * pChannel)
{
    LSA_UINT32  P_IOCC; //virt IOCC addr for reg access
    LSA_RESULT  Retval = EDD_STS_OK;

    //*** Setup addresses
    pChannel->pPAEABase     = (void *)((LSA_UINT32)(pChannel->pIOCC) + EDDI_IOCC_PA_EA_DIRECT_start);       //PAEARAM virt. addr.
    pChannel->pMbx          = (LSA_UINT32 *)((LSA_UINT32)(pChannel->pDestBase) + pChannel->MbxOffset);                //Mbx virt. addr
    pChannel->MbxPhysAddr   = pChannel->MbxOffset + pChannel->DestBasePhysAddr;                         //calculate AHB-Adr. from pMbx. For calculation-details see EDDI_CpSocIoccStartSingleTransfer
    *(pChannel->pMbx)       = 0;
    pChannel->MbxVal        = pChannel->MbxPhysAddr;

    P_IOCC = (LSA_UINT32)(pChannel->pIOCC);

    //Channel 0 gets LL entries 0 and 1, channel 1 gets LL entries 2 and 3
    pChannel->LLEntryReadPhysAddr   = EDDI_IOCC_BASE_AHB + EDDI_IOCC_HOST_LL_inst_ll_ram_start + (4 * pChannel->ChannelIdx);
    pChannel->pLLEntryRead          = (LSA_UINT32 *)(P_IOCC + EDDI_IOCC_HOST_LL_inst_ll_ram_start + (4 * pChannel->ChannelIdx));     
    pChannel->LLEntryWritePhysAddr          = EDDI_IOCC_BASE_AHB + EDDI_IOCC_HOST_LL_inst_ll_ram_start + (4 * (pChannel->ChannelIdx + 2));               
    pChannel->pLLEntryWrite                 = (LSA_UINT32 *)(P_IOCC + EDDI_IOCC_HOST_LL_inst_ll_ram_start + (4 * (pChannel->ChannelIdx + 2)));     
    pChannel->pInstance->pIntLinkListMem    = (LSA_UINT8 *)(P_IOCC + EDDI_IOCC_HOST_LL_inst_ll_ram_start + (4 * 4)); //start of free LL mem
    pChannel->pWB                           = (LSA_UINT8 *)(P_IOCC + ((0 == pChannel->ChannelIdx)?EDDI_IOCC_HOST_LL_inst_hwp0_ram_start:EDDI_IOCC_HOST_LL_inst_hwp1_ram_start));

    //*** init IOCC
    if /* instance not initialized yet */
        (!pChannel->pInstance->bUsed)
    {
        //reset iocc
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = EDDI_IOCC_HOST_LL_inst_debug_param_err
                                                            + EDDI_IOCC_HOST_LL_inst_debug_dlength_err
                                                            + EDDI_IOCC_HOST_LL_inst_debug_paea_err
                                                            + EDDI_IOCC_HOST_LL_inst_debug_size_err
                                                            + EDDI_IOCC_HOST_LL_inst_debug_addr_err;

        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_cntrl) = 0;
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_cntrl) = 0;
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp0_cntrl) = 0;
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp1_cntrl) = 0;

        // AHB-Adr. of LL-RAM! has to be entered independent wether LL resides in DDR2 or in LL-RAM!
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_ll_ram_adr_offset) = (EDDI_IOCC_BASE_AHB + EDDI_IOCC_HOST_LL_inst_ll_ram_start);

        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_sync_reset) = EDDI_IOCC_HOST_LL_inst_sync_reset_hwp0
                                                                 + EDDI_IOCC_HOST_LL_inst_sync_reset_hrp0
                                                                 + EDDI_IOCC_HOST_LL_inst_sync_reset_hwp1
                                                                 + EDDI_IOCC_HOST_LL_inst_sync_reset_hrp1;

        do
        {
            EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_sync_reset) = EDDI_IOCC_HOST_LL_inst_sync_reset_hwp0
                                                                     + EDDI_IOCC_HOST_LL_inst_sync_reset_hrp0
                                                                     + EDDI_IOCC_HOST_LL_inst_sync_reset_hwp1
                                                                     + EDDI_IOCC_HOST_LL_inst_sync_reset_hrp1;
        } while (0 != EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status));

        if (0 != EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug)) // no errors ?
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), 0, 0, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug));
            Retval = EDD_STS_ERR_HW;
        }
    }

    return(Retval);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
* function name:  eddi_IOCC_StartRead()
*
* function:       Reads data from PAEARAM to application mem
*
* return value:   EDD_STS_OK, EDD_STS_ERR_HW
*===========================================================================*/
/* Attention: Function is called under lock, EDDI_EXIT_IOCC_CHx_S is called inside this function! */
static LSA_RESULT eddi_IOCC_StartRead(EDDI_IOCC_CHANNEL_CB_TYPE * const   pChannel,
                                      LSA_UINT32                  const   DataLength,
                                      LSA_UINT32                  const   LLPhysAddr,
                                      LSA_UINT32                  const   P_Dest,
                                      LSA_UINT16                  const   NrOfLLLines)
{
    LSA_UINT32  const P_IOCC = (LSA_UINT32)pChannel->pIOCC;                                 //virt IOCC addr for reg access
    LSA_UINT32  volatile DebugReg;

    if (0 == pChannel->ChannelIdx)
    {
        #if !defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)
        if /* channel 0 not free */
            (0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hrp0_used + EDDI_IOCC_HOST_LL_inst_status_hrp0_acc_run)))
        {
            EDDI_EXIT_IOCC_CH1_S(pChannel);
            return(EDD_STS_OK_PENDING);
        }

        DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        if  /* errors from previous accesses */
            (0 != DebugReg)
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), 0 /*P_Dest*/, 0/*DataLength*/, DebugReg);
            // try to clear debug register
            EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = DebugReg;
            EDDI_EXIT_IOCC_CH1_S(pChannel);
            return(EDD_STS_ERR_HW);
        }
        #endif //!defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)

        //fill params
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_dest_adr) = P_Dest;                           //dest (AHB-addr!)
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_ll_nl)    = NrOfLLLines;                      //nr of ll-entries
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_ll_sl)    = LLPhysAddr;                       //ptr to ll-entry (AHB)
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_mb_access)= 0;                                //Mailbox-size is uint32
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_mb_data)  = pChannel->MbxVal;                 //use new mbx-value
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_mb_adr)   = pChannel->MbxPhysAddr;            //Mbx-Addr. (AHB!)
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp0_cntrl)    = EDDI_IOCC_HOST_LL_inst_hrp0_cntrl_mb_en + EDDI_IOCC_HOST_LL_inst_hrp0_cntrl_wrtrig;                                //use mailbox, all regs have to be written to start transfer!

        #if defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH1_S(pChannel);
        #endif

        //wait for completion
        do
        {
            DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        } while (   (0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hrp0_used + EDDI_IOCC_HOST_LL_inst_status_hrp0_acc_run)))
                 && (0 == DebugReg));

        //It could happen that IOCC says "ready" although some data still reside in bridges, FIFOs etc.
        //To prevent this, the mbx is written as last transfer to shared mem, and the value is waited for.
        do
        {
        } while (   (*(pChannel->pMbx) != pChannel->MbxVal)
                 && (0 == DebugReg));
        #if !defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH1_S(pChannel);
        #endif
    }
    else
    {
        #if !defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)
        if /* channel 1 not free */
            (0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hrp1_used + EDDI_IOCC_HOST_LL_inst_status_hrp1_acc_run)))
        {
            EDDI_EXIT_IOCC_CH2_S(pChannel);
            return(EDD_STS_OK_PENDING);
        }

        DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        if  /* errors from previous accesses */
            (0 != DebugReg)
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), 0 /*P_Dest*/, 0/*DataLength*/, DebugReg);
            // try to clear debug register
            EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = DebugReg;
            EDDI_EXIT_IOCC_CH2_S(pChannel);
            return(EDD_STS_ERR_HW);
        }
        #endif //!defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)

        //fill params
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_dest_adr) = P_Dest;                           //dest (AHB-addr!)
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_ll_nl)    = NrOfLLLines;                      //nr of ll-entries
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_ll_sl)    = LLPhysAddr;                       //ptr to ll-entry (AHB)
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_mb_access)= 0;                                //Mailbox-size is uint32
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_mb_data)  = pChannel->MbxVal;                 //use new mbx-value
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_mb_adr)   = pChannel->MbxPhysAddr;            //Mbx-Addr. (AHB!)
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hrp1_cntrl)    = EDDI_IOCC_HOST_LL_inst_hrp1_cntrl_mb_en + EDDI_IOCC_HOST_LL_inst_hrp1_cntrl_wrtrig; //use mailbox, all regs have to be written to start transfer!

        #if defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH2_S(pChannel);
        #endif

        //wait for completion
        do
        {
            DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        } while (   (0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hrp1_used + EDDI_IOCC_HOST_LL_inst_status_hrp1_acc_run)))
                 && (0 == DebugReg));
            
        //It could happen that IOCC says "ready" although some data still reside in bridges, FIFOs etc.
        //To prevent this, the mbx is written as last transfer to shared mem, and the value is waited for.
        do
        {
        } while (   (*(pChannel->pMbx) != pChannel->MbxVal)
                 && (0 == DebugReg));
        #if !defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH2_S(pChannel);
        #endif
    }

    if /* no errors */
        (0 == DebugReg)
    {
        return(EDD_STS_OK);
    }
    else
    {
        LSA_UINT32 const DebugRegLoc = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), P_Dest, DataLength, DebugRegLoc);
        // try to clear debug register
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = DebugRegLoc;
        return(EDD_STS_ERR_HW);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name:  eddi_IOCC_StartWrite()
*
* function:       Reads data from WriteBuffer to PAEARAM
*
* return value:   EDD_STS_OK, EDD_STS_ERR_HW
*===========================================================================*/
/* Attention: Function is called under lock, EDDI_EXIT_IOCC_CHx_S is called inside this function! */
static LSA_RESULT eddi_IOCC_StartWrite(EDDI_IOCC_CHANNEL_CB_TYPE * const   pChannel,
                                       LSA_UINT32                  const   DataLength,
                                       LSA_UINT32                  const   LLPhysAddr,
                                       LSA_UINT16                  const   NrOfLLLines)
{
    LSA_UINT32  const P_IOCC = (LSA_UINT32)pChannel->pIOCC;             //virt IOCC addr for reg access
    LSA_UINT32  volatile DebugReg;

    if (0 == pChannel->ChannelIdx)
    {
        #if !defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)
        if /* channel 0 not free */
            (0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hwp0_used + EDDI_IOCC_HOST_LL_inst_status_hwp0_acc_run)))
        {
            EDDI_EXIT_IOCC_CH1_S(pChannel);
            return(EDD_STS_OK_PENDING);
        }

        DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        if  /* errors from previous accesses */
            (0 != DebugReg)
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), 0 /*P_Dest*/, 0/*DataLength*/, DebugReg);
            // try to clear debug register
            EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = DebugReg;
            EDDI_EXIT_IOCC_CH1_S(pChannel);
            return(EDD_STS_ERR_HW);
        }
        #endif //!defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)

        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp0_nmb_data)  = DataLength;      //transfer length
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp0_ll_nl)     = NrOfLLLines;     //nr of ll-entries
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp0_ll_sl)     = LLPhysAddr;      //ptr to first ll-entry
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp0_cntrl)     = 0;               //no mbx used

        #if defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH1_S(pChannel);
        #endif

        //wait for completion
        do
        {
            DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        }
        while (   ( 0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hwp0_used + EDDI_IOCC_HOST_LL_inst_status_hwp0_acc_run)) )
               && ( 0 == DebugReg  ) );

        #if !defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH1_S(pChannel);
        #endif
    }
    else
    {
        #if !defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)
        if /* channel 0 not free */
            (0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hwp1_used + EDDI_IOCC_HOST_LL_inst_status_hwp1_acc_run)))
        {
            EDDI_EXIT_IOCC_CH2_S(pChannel);
            return(EDD_STS_OK_PENDING);
        }

        DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        if  /* errors from previous accesses */
            (0 != DebugReg)
        {
            EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), 0 /*P_Dest*/, 0/*DataLength*/, DebugReg);
            // try to clear debug register
            EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = DebugReg;
            EDDI_EXIT_IOCC_CH2_S(pChannel);
            return(EDD_STS_ERR_HW);
        }
        #endif //!defined (EDDI_IOCC_CFG_DISABLE_USE_CHECK)

        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp1_nmb_data)  = DataLength;      //transfer length
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp1_ll_nl)     = NrOfLLLines;     //nr of ll-entries
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp1_ll_sl)     = LLPhysAddr;      //ptr to first ll-entry
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_hwp1_cntrl)     = 0;               //no mbx used

        #if defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH2_S(pChannel);
        #endif

        //wait for completion
        do
        {
            DebugReg = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        }
        while (   ( 0 != (EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status) & (EDDI_IOCC_HOST_LL_inst_status_hwp1_used + EDDI_IOCC_HOST_LL_inst_status_hwp1_acc_run)) )
               && ( 0 == DebugReg  ) );

        #if !defined(EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER)
        EDDI_EXIT_IOCC_CH2_S(pChannel);
        #endif
    }

    if /* no errors */
        (0 == DebugReg)
    {
        return(EDD_STS_OK);
    }
    else
    {
        volatile LSA_UINT32 const DebugRegLoc = EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug);
        EDDI_IOCC_FILL_ERROR_INFO(pChannel->pErrorInfo, EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_status), 0, DataLength, DebugRegLoc);
        // try to clear debug register
        EDDI_IOCC_REG32b(P_IOCC, EDDI_IOCC_HOST_LL_inst_debug) = DebugRegLoc ;
        return(EDD_STS_ERR_HW);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_FatalError()                           +*/
/*+  Input/Output               EDDI_FATAL_ERROR_TYPE Error                 +*/
/*+  Input/Output          :    LSA_UINT16            ModuleID              +*/
/*+                             LSA_UINT16            Line                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : EDD-fatal-errorcode                             +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via EDDI_FATAL_ERROR macro.           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID  EDDI_IOCC_FatalError( LSA_UINT32     const  Line,
                                       LSA_UINT8   *  const  sFile,
                                       LSA_UINT32     const  ModuleID,
                                       LSA_UINT8   *  const  sErr,
                                       LSA_UINT32     const  Error,
                                       LSA_UINT32     const  DW_0,
                                       LSA_UINT32     const  DW_1 )
{
    LSA_FATAL_ERROR_TYPE     LSAError;
    LSA_UINT8             *  p;
    LSA_UINT32               Ctr, start;

    if (g_pEDDI_IOCC_Info->Excp.Error != EDDI_IOCC_FATAL_NO_ERROR)
    {
        return;
    }

    g_pEDDI_IOCC_Info->Excp.Error    = Error;
    g_pEDDI_IOCC_Info->Excp.Line     = Line;
    g_pEDDI_IOCC_Info->Excp.ModuleID = ModuleID;

    LSAError.lsa_component_id  = LSA_COMP_ID_EDD_ERTEC;
    LSAError.module_id         = (LSA_UINT16)ModuleID;
    LSAError.line              = (LSA_UINT16)Line;
    LSAError.error_code[0]     = Error;
    LSAError.error_code[1]     = EDDI_IOCC_MODULE_ID;
    LSAError.error_code[2]     = DW_0;
    LSAError.error_code[3]     = DW_1;
    LSAError.error_data_length = EDDI_IOCC_MAX_EXCP_INFO;

    p = &g_pEDDI_IOCC_Info->Excp.sInfo[0];
    for (Ctr=0; Ctr<EDDI_IOCC_MAX_EXCP_INFO; Ctr++)
    {
        *p++ = 0;
    }

    p = sFile;

    if (p)
    {
        start = 0;
        for (Ctr = 0; Ctr < 256; Ctr++)
        {
            if (*p == '\\')
            {
                start = 0;
                p++;
                continue;
            }

            if ((*p == 0) || (start == (EDDI_IOCC_MAX_EXCP_INFO - 5)))
            {
                g_pEDDI_IOCC_Info->Excp.sInfo[start] = ' ';
                start++;
                g_pEDDI_IOCC_Info->Excp.sInfo[start] = '=';
                start++;
                g_pEDDI_IOCC_Info->Excp.sInfo[start] = '>';
                start++;
                g_pEDDI_IOCC_Info->Excp.sInfo[start] = ' ';
                start++;
                break;
            }

            g_pEDDI_IOCC_Info->Excp.sInfo[start] = *p;
            p++;
            start++;
        }

        if (sErr)
        {
            p = sErr;
        }
        else
        {
            p = (LSA_UINT8 *)(void *)"No Message specified!";
        }

        for (Ctr = start; Ctr < (EDDI_IOCC_MAX_EXCP_INFO - 1); Ctr++)
        {
            if (*p == 0)
            {
                break;
            }

            g_pEDDI_IOCC_Info->Excp.sInfo[Ctr] = *p;
            p++;
        }
    }
    else //EDDI_CFG_NO_FATAL_FILE_INFO
    {
        start = 0;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'N';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'o';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = ' ';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'F';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'i';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'l';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'e';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = ' ';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'I';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'n';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'f';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = 'o';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = ' ';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = '=';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = '>';
        start++;
        g_pEDDI_IOCC_Info->Excp.sInfo[start] = ' ';
    }

    LSAError.error_data_ptr = g_pEDDI_IOCC_Info->Excp.sInfo;

    EDDI_IOCC_FATAL_ERROR((LSA_UINT16)sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);
}
/*---------------------- end [subroutine] ---------------------------------*/
#else //(EDDI_CFG_SOC)
LSA_VOID eddi_IOCC_Init(LSA_VOID)
{
}

LSA_VOID eddi_IOCC_Deinit(LSA_VOID)
{
}

LSA_RESULT eddi_IOCC_Setup( EDDI_IOCC_LOWER_HANDLE_TYPE      * pIOCCHandle,
                            EDDI_IOCC_UPPER_HANDLE_TYPE        UpperHandle,
                            LSA_UINT32                 const   InstanceHandle,
                            LSA_UINT8                  const   IOCCChannel,
                            LSA_VOID                 * const   pIOCC,
                            LSA_UINT8                * const   pDestBase,
                            LSA_UINT32                 const   DestBasePhysAddr,
                            LSA_UINT8                * const   pExtLinkListBase,
                            LSA_UINT32                 const   ExtLinkListBasePhysAddr,
                            LSA_UINT8             *  * const   ppIntLinkListMem,
                            LSA_UINT32               * const   pIntLinkListMemSize,
                            LSA_UINT8             *  * const   ppWB,
                            LSA_UINT32                 const   MbxOffset,
                            EDDI_IOCC_ERROR_TYPE     * const   pErrorInfo)
{
    LSA_UNUSED_ARG(pIOCCHandle);
    LSA_UNUSED_ARG(UpperHandle);
    LSA_UNUSED_ARG(InstanceHandle);
    LSA_UNUSED_ARG(IOCCChannel);
    LSA_UNUSED_ARG(pIOCC);
    LSA_UNUSED_ARG(pDestBase);
    LSA_UNUSED_ARG(DestBasePhysAddr);
    LSA_UNUSED_ARG(pExtLinkListBase);
    LSA_UNUSED_ARG(ExtLinkListBasePhysAddr);
    LSA_UNUSED_ARG(ppIntLinkListMem);
    LSA_UNUSED_ARG(pIntLinkListMemSize);
    LSA_UNUSED_ARG(ppWB);
    LSA_UNUSED_ARG(MbxOffset);
    LSA_UNUSED_ARG(pErrorInfo);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_Shutdown (EDDI_IOCC_LOWER_HANDLE_TYPE const IOCCHandle)
{
    LSA_UNUSED_ARG(IOCCHandle);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_CreateLinkList(EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                    EDDI_IOCC_LL_HANDLE_TYPE     *         pLLHandle,
                                    LSA_UINT8                      const   LinkListType, 
                                    LSA_UINT8                      const   LinkListMode, 
                                    LSA_UINT16                     const   NrOfLines)
{
    LSA_UNUSED_ARG(IOCCHandle);
    LSA_UNUSED_ARG(pLLHandle);
    LSA_UNUSED_ARG(LinkListType);
    LSA_UNUSED_ARG(LinkListMode);
    LSA_UNUSED_ARG(NrOfLines);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_DeleteLinkList(EDDI_IOCC_LL_HANDLE_TYPE const LLHandle)
{
    LSA_UNUSED_ARG(LLHandle);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_AppendToLinkList(EDDI_IOCC_LL_HANDLE_TYPE     const   LLHandle,
                                      LSA_UINT32                   const   PAEARAMOffset, 
                                      LSA_UINT16                   const   DataLength,
                                      LSA_UINT8                  * const   pMem,
                                      LSA_VOID                   * const   UserIOHandle)
{
    LSA_UNUSED_ARG(LLHandle);
    LSA_UNUSED_ARG(PAEARAMOffset);
    LSA_UNUSED_ARG(DataLength);
    LSA_UNUSED_ARG(pMem);
    LSA_UNUSED_ARG(UserIOHandle);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_RemoveFromLinkList(EDDI_IOCC_LL_HANDLE_TYPE     const   LLHandle,
                                        LSA_VOID         const     * const   UserIOHandle)
{
    LSA_UNUSED_ARG(LLHandle);
    LSA_UNUSED_ARG(UserIOHandle);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_SingleRead (EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                 LSA_UINT32                     const   DataLength, 
                                 LSA_UINT32                     const   PAEARAMOffset,
                                 LSA_UINT8                    * const   pDest)
{
    LSA_UNUSED_ARG(IOCCHandle);
    LSA_UNUSED_ARG(DataLength);
    LSA_UNUSED_ARG(PAEARAMOffset);
    LSA_UNUSED_ARG(pDest);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_SingleWrite (EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                  LSA_UINT32                     const   DataLength, 
                                  LSA_UINT32                     const   PAEARAMOffset,
                                  LSA_UINT8                    * const   pSrc)
{
    LSA_UNUSED_ARG(IOCCHandle);
    LSA_UNUSED_ARG(DataLength);
    LSA_UNUSED_ARG(PAEARAMOffset);
    LSA_UNUSED_ARG(pSrc);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_MultipleRead (EDDI_IOCC_LL_HANDLE_TYPE             * pLLHandle,
                                   LSA_UINT32                     const   NrOfLinkLists)
{
    LSA_UNUSED_ARG(pLLHandle);
    LSA_UNUSED_ARG(NrOfLinkLists);
    return (EDD_STS_ERR_SERVICE);
}

LSA_RESULT eddi_IOCC_MultipleWrite (EDDI_IOCC_LL_HANDLE_TYPE             * pLLHandle,
                                    LSA_UINT32                     const   NrOfLinkLists)
{
    LSA_UNUSED_ARG(pLLHandle);
    LSA_UNUSED_ARG(NrOfLinkLists);
    return (EDD_STS_ERR_SERVICE);
}

LSA_VOID * eddi_IOCC_CalcDirectAccessAddress (EDDI_IOCC_LOWER_HANDLE_TYPE   const   IOCCHandle,
                                              LSA_VOID                    * const   pPAEARAM)
{
    LSA_UNUSED_ARG(IOCCHandle);
    LSA_UNUSED_ARG(pPAEARAM);
    return (0);
}
#endif //EDDI_CFG_SOC


/*****************************************************************************/
/*  end of file eddi_iocc.c                                                  */
/*****************************************************************************/
