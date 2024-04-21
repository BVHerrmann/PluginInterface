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
/*  F i l e               &F: edds_nrt_rcv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDS. Receive            */
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

#define LTRC_ACT_MODUL_ID  130
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_NRT_RCV */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+  Functionname          :    EDDS_NRTRxIndicate                          +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ...    *pBuffer      +*/
/*+                             LSA_UINT32                     Length       +*/
/*+                             LSA_UINT32                     PortID       +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pBuffer    : Address for pointer to rx-buffer (input and output)       +*/
/*+  Length     : Length of received data.(input)                           +*/
/*+  PortID     : Port the frame was received from (input)                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE         No free Rx-buffer            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called when a nrt-ARP/IP frame was recvd.+*/
/*+               The function is called within int-handler and has to      +*/
/*+               free the buffer and provide a new one.                    +*/
/*+                                                                         +*/
/*+               We put the buffer into the nrt-rx-ring-queue and return   +*/
/*+               an empty buffer. if no queue entry is available we drop   +*/
/*+               the frame. The queued frame must be handled with          +*/
/*+               NRTRxReady() to be removed from the queue.                +*/
/*+                                                                         +*/
/*+ Important:    We dont check for initialized component! The component    +*/
/*+               have to be initialized before calling this function.      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



//JB 17/11/2014 checked where called
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRxIndicate(
        EDDS_LOCAL_DDB_PTR_TYPE                          pDDB,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR      *pBuffer,
        LSA_UINT32                                       Length,
        LSA_UINT32                                       queueIdx,
        LSA_UINT32                                       PortID,
        LSA_UINT32                                       State)
{
    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_MEM_U8_PTR_TYPE       pBuffer2;
    EDDS_NRT_DSCR_TYPE_PTR          pDscr;
    EDDS_NRT_DSCR_MGM_TYPE_PTR      pRxDscr;
    LSA_UINT32                      TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_NRT_TRACE_05(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTRxIndicate(pDDB: 0x%X,*pBuffer: 0x%X, Len: %d, queueIdx: %d, PortID: %d)",
                           pDDB,
                           *pBuffer,
                           Length,
                           queueIdx,
                           PortID);

    pNRTComp     = pDDB->pNRT;
    pRxDscr      = &pNRTComp->RxDscrMgm[queueIdx];
    pDscr        = &pRxDscr->pRxDscrStart[pRxDscr->RxFillCnt];

    if ( pDscr->State == EDDS_NRT_DSCR_STATE_FREE )
    {
        pBuffer2       =  pDscr->pBuffer;
        pDscr->pBuffer = *pBuffer;
        pDscr->Length  =  Length;
        pDscr->PortID  =  PortID;
        pDscr->State   =  State;
        *pBuffer       =  pBuffer2;

        if ( pRxDscr->RxFillCnt == (pRxDscr->RxDscrCnt-1) )
            pRxDscr->RxFillCnt = 0;
        else
            pRxDscr->RxFillCnt++;

        Status =  EDD_STS_OK;
    }
    else
    {
        EDDS_NRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDS_NRTRxIndicate: Silent discarding frame (queueIdx: %d)",queueIdx);

        Status = EDD_STS_ERR_RESOURCE;
        ++(pNRTComp->Stats.RxSilentDiscarded[queueIdx]);
    }

    EDDS_NRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTRxIndicate(*pBuffer: 0x%X, queueIdx: %d), Status: 0x%X",
                           *pBuffer,
                           queueIdx,
                           Status);

    LSA_UNUSED_ARG(TraceIdx);
}




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTRxInitDscrLookUpCounter             +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pNRTComp   : Pointer to NRT-Component management                  (in) +*/
/*+  Result     : none                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialize rx look up counter.                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 can not be a null pointer (previously taken from DDB)
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRxResetDscrLookUpCounter(
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp)
{
    LSA_UINT16 idx;

    /* reset queue look up counter */
    pNRTComp->RxQueueLookUpCnt = 0;

    /* reset dscr look up counter */
    for (idx = 0 ; idx < EDDS_NRT_BUFFER_MAX_IDX ; idx++)
    {
        pNRTComp->RxDscrMgm[idx].RxDscrLookUpCnt = 0;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTRxGetDscrFromRing                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp   +*/
/*+                             EDDS_NRT_DSCR_MGM_TYPE_PTR       pDscrMgm   +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE      pBufferType+*/
/*+  Result                :    NRT_DSCR_TYPE_PTR                           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pNRTComp   : Pointer to NRT-Component management                  (in) +*/
/*+  pDscrMgm   : Pointer to Pointer for Descriptor Management pointer (out)+*/
/*+  pBufferType: UINT32 Pointer for Filtertype if frame was present   (out)+*/
/*+                EDDS_NRT_BUFFER_ASRT_ALARM_IDX                           +*/
/*+                EDDS_NRT_BUFFER_IP_IDX                                   +*/
/*+                EDDS_NRT_BUFFER_ASRT_OTHER_IDX                           +*/
/*+                EDDS_NRT_BUFFER_OTHER_IDX                                +*/
/*+  Result     : Pointer to Descriptor or LSA_NULL                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if a frame is present in any receive-descriptor    +*/
/*+                                                                         +*/
/*+               if so a pointer to the descriptor and filter is returned  +*/
/*+               if not LSA_NULL.                                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 checked where called
static EDDS_NRT_DSCR_TYPE_PTR EDDS_LOCAL_FCT_ATTR EDDS_NRTRxGetDscrFromRing(
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE                 pNRTComp,
    EDDS_NRT_DSCR_MGM_TYPE_PTR EDDS_LOCAL_MEM_ATTR * pDscrMgm,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                      pBufferType)
{

    EDDS_NRT_DSCR_TYPE_PTR pDscr = LSA_NULL;
    LSA_UINT32             CheckCnt;
    LSA_UINT32             idx;

    /* pre run over all queues regarding max lookup counter for every queue */
    for (idx = pNRTComp->RxQueueLookUpCnt ; idx < EDDS_NRT_BUFFER_MAX_IDX ; idx++)
    {
        EDDS_NRT_DSCR_MGM_TYPE_PTR pRxDscrMgm = &pNRTComp->RxDscrMgm[idx];

        /* get next dscr to be checked */
        CheckCnt = pRxDscrMgm->RxCheckCnt;
        pDscr    = &pRxDscrMgm->pRxDscrStart[CheckCnt];

        /* dscr available? */
        if ( EDDS_NRT_DSCR_STATE_FREE != pDscr->State )
        {
            /* dscr queue lookup counter reached its limit? */
            if (pRxDscrMgm->RxDscrLookUpCnt < pRxDscrMgm->RxDscrLookUpCntMax)
            {
                *pDscrMgm    = pRxDscrMgm;
                *pBufferType = idx;

                pRxDscrMgm->RxDscrLookUpCnt++;

                return (pDscr);
            }
            else
            {
                /* limit reached, proceed with next queue */
                pNRTComp->RxQueueLookUpCnt++;
            }
        }
        else
        {
            /* no more dscr available at this queue, proceed with next queue */
            pNRTComp->RxQueueLookUpCnt++;
        }
    }

    /* post run, process queues beginning with highest priority */
    for (idx = 0 ; idx < EDDS_NRT_BUFFER_MAX_IDX ; idx++)
    {
        EDDS_NRT_DSCR_MGM_TYPE_PTR pRxDscrMgm = &pNRTComp->RxDscrMgm[idx];

        /* get next dscr to be checked */
        CheckCnt = pRxDscrMgm->RxCheckCnt;
        pDscr    = &pRxDscrMgm->pRxDscrStart[CheckCnt];

        /* dscr available? */
        if ( EDDS_NRT_DSCR_STATE_FREE != pDscr->State )
        {
            *pDscrMgm    = pRxDscrMgm;
            *pBufferType = idx;
            return (pDscr);
        }
    }

    return (LSA_NULL);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTGetFilterLenFrame                   +*/
/*+  Input/Output          :    EDD_UPPER_MEM_U8_PTR_TYPE   pBuffer         +*/
/*+                             NLSA_UINT32                 Length          +*/
/*+                             LSA_BOOL                    VLANTagPresent  +*/
/*+                             EDDS_LOCAL_MEM_U32_PRT_TYPE pFilterType     +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuffer    : Pointer to Framesbuffer with LEN-Frame                    +*/
/*+  Length     : Length of Framebuffer                                     +*/
/*+  VLANTagPresent: LSA_TURE VLAN Tag within frame                         +*/
/*+  pFilterType: Address for Filtertype (output)                           +*/
/*+                NRT_FILTER_LEN_DCP_IDX                                   +*/
/*+                NRT_FILTER_LEN_TSYNC_IDX                                 +*/
/*+                NRT_FILTER_LEN_OTHER_IDX                                 +*/
/*+                NRT_FILTER_LEN_STDBY_IDX                                 +*/
/*+                NRT_FILTER_LEN_HSR_IDX                                   +*/
/*+  Result     : LSA_TRUE : Valid Frame present, FilterType set.           +*/
/*+               LSA_FALSE: (not set yet) no valid Frame                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks the Frame of LEN-Type for LEN-Filters              +*/
/*+                * DCP                                                    +*/
/*+                * Timesync                                               +*/
/*+                * Other                                                  +*/
/*+                                                                         +*/
/*+   6   6   2     1     1     1        5                                  +*/
/*+ +---+---+----+-----+-----+-----+-------------+-------                   +*/
/*+ |DA |SA |LEN |DSAP |SSAP |CTRL |   SNAP ID   | Data                     +*/
/*+ |   |   |    |     |     |     +--3---+---2--+                          +*/
/*+ |   |   |    |     |     |     | OUI  | Type |                          +*/
/*+ +---+---+----+-----+-----+-----+------+------+-------                   +*/
/*+                                                                         +*/
/*+        DSAP    SSAP    CTRL   OUI                    Type               +*/
/*+ DCP:    AAh    AAh      3h    8 0 6 (Siemens)        01FDh              +*/
/*+ Time    AAh    AAh      3h    8 0 6 (Siemens)        0100h              +*/
/*+ Stdby   AAh    AAh      3h    8 0 6 (Siemens)        01FBh              +*/
/*+ HSR     AAh    AAh      3h    8 80h 63h (Hirschmann) 0800h              +*/
/*+                                                                         +*/
/*+ Note that an optional VLAN-Tag after LEN is checked and supported       +*/
/*+ Note that pBuffer has to be DWORD-aligned!! (not checked)               +*/
/*+                                                                         +*/
/*+ The frame has to be of LEN-Type (not checked!)                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 out param
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTGetFilterLenFrame(
    const EDD_UPPER_MEM_U8_PTR_TYPE const pBuffer, 
    LSA_UINT32                  Length,
    LSA_BOOL                    VLANTagPresent,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pFilterType)
{

    LSA_UINT16  Type;
    LSA_BOOL    Siemens = LSA_FALSE;

    Type   = EDDS_FRAME_INVALID_TYPE;

    /* --------------------------------------------------------------------------*/
    /* first we check for a present VLAN-Tag which will have to be handled       */
    /* --------------------------------------------------------------------------*/

    if ( VLANTagPresent )
    {
        /* check for minimum length of frame */
        if ( Length >= EDDS_FRAME_SNAP_HEADER_LEN_VLAN )
        {
            /* check for SNAP-Header settings for SIEMENS */
            if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_VLAN] == EDDS_FRAME_SNAP_DSAP_SSAP_FILTER ) &&
                 (((EDD_UPPER_MEM_U32_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_VLAN] == EDDS_FRAME_SNAP_CTRL_OUI_FILTER_SIEMENS ) //lint !e835 define-based behavior
				 )  //lint !e845 !e826 JB 19/11/2014 define-based behaviour | pointer cast on purpose | no unknown null-ptr
            {
                /* Get Type of SNAP-ID */
                Type = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_VLAN]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
                Siemens = LSA_TRUE;
            }else             /* check for SNAP-Header settings for SIEMENS */
                if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_VLAN] == EDDS_FRAME_SNAP_DSAP_SSAP_DEF_FILTER ) &&
                     (((EDD_UPPER_MEM_U32_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_VLAN] == EDDS_FRAME_SNAP_CTRL_OUI_FILTER_SIEMENS ) //lint !e835 define-based behaviour
					 ) //lint !e845 !e826 JB 19/11/2014 define-based behaviour | pointer cast on purpose | no unknown null-ptr
                {
                    /* Get Type of SNAP-ID */
                    Type = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_VLAN]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
                    Siemens = LSA_TRUE;
                }else
                	/* check for SNAP-Header settings for HIRSCMANN */
                	if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_VLAN] == EDDS_FRAME_SNAP_DSAP_SSAP_FILTER ) &&
                			(((EDD_UPPER_MEM_U32_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_VLAN] == EDDS_FRAME_SNAP_CTRL_OUI_FILTER_HIRSCHMANN ) //lint !e835 define-based behaviour
							) //lint !e845 !e826 JB 19/11/2014 define-based behaviour | pointer cast on purpose | no unknown null-ptr
					{
						/* Get Type of SNAP-ID */
						Type = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_VLAN]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
					}
        }
    }
    else /* no VLAN-Tag present */
    {
        /* check for minimum length of frame */
        if ( Length >= EDDS_FRAME_SNAP_HEADER_LEN_NO_VLAN )
        {
            /* check for SNAP-Header settings for SIEMENS */
            if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_NO_VLAN] == EDDS_FRAME_SNAP_DSAP_SSAP_FILTER) &&
                 (((EDD_UPPER_MEM_U32_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_NO_VLAN] == EDDS_FRAME_SNAP_CTRL_OUI_FILTER_SIEMENS ) //lint !e835  define-based behaviour
				 ) //lint !e845 !e826 JB 19/11/2014 define-based behaviour | pointer cast on purpose | no unknown null-ptr
            {
                /* Get Type of SNAP-ID */
                Type = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_NO_VLAN]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
                Siemens = LSA_TRUE;
            }else
                if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_NO_VLAN] == EDDS_FRAME_SNAP_DSAP_SSAP_DEF_FILTER) &&
                     (((EDD_UPPER_MEM_U32_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_NO_VLAN] == EDDS_FRAME_SNAP_CTRL_OUI_FILTER_SIEMENS ) //lint !e835 define-based behaviour
					 ) //lint !e845 !e826 JB 19/11/2014 define-based behaviour | pointer cast on purpose | no unknown null-ptrn
                {
                    /* Get Type of SNAP-ID */
                    Type = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_NO_VLAN]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
                    Siemens = LSA_TRUE;
                }else
                	/* check for SNAP-Header settings for HIRSCMANN */
                	if ( (((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_NO_VLAN] == EDDS_FRAME_SNAP_DSAP_SSAP_FILTER) &&
                			(((EDD_UPPER_MEM_U32_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_NO_VLAN] == EDDS_FRAME_SNAP_CTRL_OUI_FILTER_HIRSCHMANN ) //lint !e835 define-based behaviour
							) //lint !e845 !e826 JB 19/11/2014 define-based behaviour | pointer cast on purpose | no unknown null-ptr
                		{
							/* Get Type of SNAP-ID */
							Type = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_NO_VLAN]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
						}

        }
    }

    /* --------------------------------------------------------------------------*/
    /* now we set the Filtertype depending on Frametype                          */
    /* --------------------------------------------------------------------------*/
	if ( Siemens )
	{
	    switch (Type)
	    {
    	    case EDDS_FRAME_DCP_TYPE:
        	    *pFilterType = EDDS_NRT_FILTER_LEN_DCP_IDX;
	            break;
    	    case EDDS_FRAME_TSYNC_TYPE:
        	    *pFilterType = EDDS_NRT_FILTER_LEN_TSYNC_IDX;
	            break;
    	    case EDDS_FRAME_STDBY_TYPE:
        	    *pFilterType = EDDS_NRT_FILTER_LEN_STDBY_IDX;
	            break;
    	    case EDDS_FRAME_SINEC_FWL_TYPE:
    	    	*pFilterType = EDDS_NRT_FILTER_LEN_SINEC_FWL_IDX;
    	    	break;
    	    default:
        	    *pFilterType = EDDS_NRT_FILTER_LEN_OTHER_IDX;
	            break;
		}
	}
	else /* hirschmann or invalid */
	{
	    switch (Type)
	    {
    	    case EDDS_FRAME_HSR_TYPE:
        	    *pFilterType = EDDS_NRT_FILTER_LEN_HSR_IDX;
	            break;
    	    default:
        	    *pFilterType = EDDS_NRT_FILTER_LEN_OTHER_IDX;
	            break;
		}
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTFrameMemcmp                         +*/
/*+  Input/Output          :    EDD_UPPER_MEM_U8_PTR_TYPE   pFrame          +*/
/*+                             EDDS_LOCAL_MEM_U8_PRT_TYPE  pLocal          +*/
/*+                             LSA_UINT32                  Length          +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pFrame         : Pointer to Framebuffer                                +*/
/*+  pLocal         : Pointer to local buffer                               +*/
/*+  Length         : Length of buffer in bytes                             +*/
/*+  Result         : LSA_TRUE : Buffers equal                              +*/
/*+                   LSA_FALSE: Buffers not equal                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Compares two buffers.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_NRTFrameMemcmp(
    const EDD_UPPER_MEM_U8_PTR_TYPE     pFrame,
    const EDDS_LOCAL_MEM_U8_PTR_TYPE    pLocal,
    LSA_UINT32                          Length)
{
    while ((Length) && (*pFrame++ == *pLocal++))
    {
        Length--;
    }

    if ( Length ) return(LSA_FALSE);

    return(LSA_TRUE);
}

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTGetFilterXRToUDPFrame               +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pBuffer         +*/
/*+                             NLSA_UINT32                 Length          +*/
/*+                             LSA_BOOL                    VLANTagPresent  +*/
/*+                             EDDS_LOCAL_MEM_U32_PRT_TYPE pFilterType     +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuffer       : Pointer to Framesbuffer with xRToverUDP-Frame          +*/
/*+  Length        : Length of Framebuffer                                  +*/
/*+  VLANTagPresent: LSA_TURE VLAN Tag within frame                         +*/
/*+  pFilterType   : Address for Filtertype (output)                        +*/
/*+                   NRT_FILTER_XRT_UDP_DCP_IDX                            +*/
/*+                   NRT_FILTER_XRT_UDP_DCP_HELLO_IDX                      +*/
/*+  Result        : LSA_TRUE : Valid Frame present, FilterType set.        +*/
/*+                  LSA_FALSE: (not set yet) no valid Frame                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks the xRT over UDP Ethernet frame for different      +*/
/*+               FrameID types                                             +*/
/*+                * DCP                                                    +*/
/*+                                                                         +*/
/*+               For DCP frames there is a optional IDENTIFY-TLV-Filter.   +*/
/*+                                                                         +*/
/*+               Note: IP/UDP-validations already done in ISR!             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_NRTGetFilterXRToUDPFrame(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pBuffer,
    LSA_UINT32                  Length,
    LSA_BOOL                    VLANTagPresent,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pFilterType)
{

    LSA_BOOL                   Result;
    LSA_UINT16                 FrameID;
    EDDS_UDP_XRT_FRAME_PTR_TYPE pHeader;

    Result = LSA_TRUE;

    if ( VLANTagPresent )
    {
        pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pBuffer[EDDS_FRAME_HEADER_SIZE_WITH_VLAN];
    }
    else
    {
        pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pBuffer[EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN];
    }

    FrameID = EDDS_NTOHS(pHeader->FrameID); /* get FrameID */

    /* check for DCP */
    if (( FrameID >= EDD_SRT_FRAMEID_ASRT_DCP_HELLO ) &&
        ( FrameID <= EDD_SRT_FRAMEID_ASRT_DCP_MC_RSP  ))
    {
        if ( FrameID == EDD_SRT_FRAMEID_ASRT_DCP_HELLO )
        {
            *pFilterType = EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX;
            /* Check DCP-Filter .. */
            /* not implemented yet. Frameformat not spezified yet */
        }
        else
        {
            *pFilterType = EDDS_NRT_FILTER_XRT_UDP_DCP_IDX;
            /* Check DCP-Filter .. */
            /* not implemented yet. Frameformat not spezified yet */
        }
    }
    else
    {
        /* Note: PTP not with UDP ! */
        /* no supported FrameID     */
        Result = LSA_FALSE;
    }

    LSA_UNUSED_ARG(Length);
    LSA_UNUSED_ARG(pDDB);

    return(Result);

}
#endif

/**
 * \brief       Functionname        EDDS_getFilterType
 * \detail      Description         figure out the FilterType for the given Ethernet Frame \n
 *                                  <b>called from scheduler context</b>
 *
 * @param[in]   pDDB                Pointer to DeviceDescriptionBlock
 * @param[in]   BufferType
 * @param[in]   pDscr
 * @param[in]   VLANTagPresent      LSA_TRUE VLAN Tag within frame
 *
 * @return      EDDS_NRT_FILTER_ARP_IDX
                EDDS_NRT_FILTER_LEN_DCP_IDX
                EDDS_NRT_FILTER_LEN_TSYNC_IDX
                EDDS_NRT_FILTER_LEN_OTHER_IDX
                EDDS_NRT_FILTER_OTHER_IDX
                EDDS_NRT_FILTER_XRT_DCP_IDX
                EDDS_NRT_FILTER_OTHER_LLDP_IDX
                EDDS_NRT_FILTER_XRT_ASRT_IDX
                EDDS_NRT_FILTER_XRT_UDP_ASRT_IDX
                EDDS_NRT_FILTER_XRT_UDP_DCP_IDX
                EDDS_NRT_FILTER_XRT_PTP_SYNC_IDX
                EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX
                EDDS_NRT_FILTER_XRT_PTP_DELAY_IDX
                EDDS_NRT_FILTER_OTHER_MRP_IDX
                EDDS_NRT_FILTER_IP_ICMP_IDX
                EDDS_NRT_FILTER_IP_IGMP_IDX
                EDDS_NRT_FILTER_IP_TCP_IDX
                EDDS_NRT_FILTER_IP_UDP_IDX
                EDDS_NRT_FILTER_IP_VRRP_IDX
                EDDS_NRT_FILTER_IP_OTHER_IDX
                EDDS_NRT_FILTER_RARP_IDX
                EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX
                EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX
                EDDS_NRT_FILTER_LEN_STDBY_IDX
                EDDS_NRT_FILTER_LEN_HSR_IDX
                EDDS_NRT_FILTER_LEN_SINEC_FWL_IDX
                EDDS_NRT_FILTER_NOT_USED
 */
static LSA_UINT32 EDDS_NRTGetFilterType(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
    LSA_UINT32                      BufferType,
    EDDS_NRT_DSCR_TYPE_PTR_TO_CONST pDscr,
    LSA_BOOL                        VLANTagPresent)
{
    EDD_UPPER_MEM_U8_PTR_TYPE pBuffer;
    LSA_UINT32  FilterType;

    FilterType              = EDDS_NRT_FILTER_NOT_USED;
    pBuffer                 = pDscr->pBuffer;

    switch(BufferType)
    {
        case EDDS_NRT_BUFFER_IP_IDX:
        /* Second-Stage filtering of IP/ARP frames */
        /* EDD_NRT_FRAME_IP_ICMP                   */
        /* EDD_NRT_FRAME_IP_IGMP                   */
        /* EDD_NRT_FRAME_IP_TCP                    */
        /* EDD_NRT_FRAME_IP_UDP                    */
        /* EDD_NRT_FRAME_IP_VRRP                   */
        /* EDD_NRT_FRAME_IP_OTHER                  */
        /* EDD_NRT_FRAME_ARP                       */
        /* EDD_NRT_FRAME_RARP                      */
        {
            LSA_UINT16              FrameType;

            FrameType = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[(VLANTagPresent?EDDS_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET:EDDS_FRAME_LEN_TYPE_WORD_OFFSET)]; //lint !e826  JB 17/11/2014 pointer cast on purpose - *pBuffer (unknown-name)
            switch(FrameType)
            {
                case EDDS_RARP_TAG:
                    FilterType = EDDS_NRT_FILTER_RARP_IDX;
                    break;
                case EDDS_ARP_TAG:
                    FilterType = EDDS_NRT_FILTER_ARP_IDX;
                    break;
                case EDDS_IP_TAG:
                /* access "protocol" field within frame */
                {
                    EDDS_IP_FRAME_PTR_TYPE  pIPHeader;
                    pIPHeader = (EDDS_IP_FRAME_PTR_TYPE) &pBuffer[(VLANTagPresent?EDDS_FRAME_HEADER_SIZE_WITH_VLAN:EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN)];  //lint !e826 JB 17/11/2014 pointer cast on purpose
                    switch (pIPHeader->Protocol)
                    {
                        case EDDS_IP_ICMP:
                            FilterType = EDDS_NRT_FILTER_IP_ICMP_IDX;
                            break;
                        case EDDS_IP_IGMP:
                            FilterType = EDDS_NRT_FILTER_IP_IGMP_IDX;
                            break;
                        case EDDS_IP_TCP:
                            FilterType = EDDS_NRT_FILTER_IP_TCP_IDX;
                            break;
                        case EDDS_IP_UDP:
                            FilterType = EDDS_NRT_FILTER_IP_UDP_IDX;
                            break;
                        case EDDS_IP_VRRP:
                            FilterType = EDDS_NRT_FILTER_IP_VRRP_IDX;
                            break;
                        default:
                            FilterType = EDDS_NRT_FILTER_IP_OTHER_IDX;
                            break;
                    }
                    break;
                }
                default:
                    FilterType = EDDS_NRT_FILTER_NOT_USED;
                    break;
            }
            break;
        }
        case EDDS_NRT_BUFFER_ASRT_ALARM_IDX:
            /* Second-Stage filtering of ASRT ALARM frames */
            /* EDD_NRT_FRAME_ASRT                          */
            /* EDD_NRT_FRAME_UDP_ASRT                      */
            if ( pDscr->State == EDDS_NRT_DSCR_STATE_IN_USE_XRT )
            {
                FilterType = EDDS_NRT_FILTER_XRT_ASRT_IDX;
            }
            else /* EDDS_NRT_DSCR_STATE_IN_USE_UDP */
            {
                FilterType = EDDS_NRT_FILTER_XRT_UDP_ASRT_IDX;
            }
            break;
        case EDDS_NRT_BUFFER_OTHER_IDX:
            /* Second-Stage filtering of OTHER-Type Frames */
            /* EDD_NRT_FRAME_LEN_DCP                       */
            /* EDD_NRT_FRAME_LEN_TSYNC                     */
            /* EDD_NRT_FRAME_LEN_OTHER                     */
            /* EDD_NRT_FRAME_LEN_STDBY                     */
            /* EDD_NRT_FRAME_OTHER                         */
            /* EDD_NRT_FRAME_LLDP                          */
            /* EDD_NRT_FRAME_MRP (MRP)                     */
            if ( pDscr->State == EDDS_NRT_DSCR_STATE_IN_USE_OTHER_TYPE )
            {
                LSA_UINT32 FrameType;
                FrameType = ((EDD_UPPER_MEM_U16_PTR_TYPE)pBuffer)[(VLANTagPresent?EDDS_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET:EDDS_FRAME_LEN_TYPE_WORD_OFFSET)]; //lint !e826  JB 19/11/2014 pointer cast on purpose | no unknown null-ptr
                switch (FrameType)
                {
                    case EDDS_LLDP_TAG:
                        FilterType = EDDS_NRT_FILTER_OTHER_LLDP_IDX;
                        break;
                    case EDDS_MRP_TAG:
                        FilterType = EDDS_NRT_FILTER_OTHER_MRP_IDX;
                        break;
                    case EDDS_HSYNC_TAG:
                        FilterType = EDDS_NRT_FILTER_HSYNC_IDX;
                        break;
                    default:
                        FilterType = EDDS_NRT_FILTER_OTHER_IDX;
                        break;
                }
            }
            else /* EDDS_NRT_DSCR_STATE_IN_USE_OTHER_LEN */
            {
                EDDS_NRTGetFilterLenFrame(pDscr->pBuffer,
                                          pDscr->Length,
                                          VLANTagPresent,
                                          &FilterType);
            }
            break;
        case EDDS_NRT_BUFFER_ASRT_OTHER_IDX:
            /* Second-Stage filtering of ASRT-Other frames */
            /* EDD_NRT_FRAME_DCP                           */
            /* EDD_NRT_FRAME_UDP_DCP                       */
            /* EDDS_NRT_FRAME_PTP                          */
            /* EDD_NRT_FRAME_MRP (MRRT)                    */
            if ( pDscr->State == EDDS_NRT_DSCR_STATE_IN_USE_XRT )
            {
                LSA_UINT16 FrameID;
                FrameID = EDDS_NTOHS(((EDD_UPPER_MEM_U16_PTR_TYPE)(pBuffer))[(VLANTagPresent?EDDS_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET:EDDS_FRAME_XRT_FRAME_ID_WORD_OFFSET)]);  //lint !e826  JB 19/11/2014 pointer cast on purpose | *pBuffer (unknown) | no unknown null-ptr
                /* aSRT ALARM Ethernet frames */
                if (  ( FrameID >= EDD_SRT_FRAMEID_ASRT_DCP_HELLO   )
                   && ( FrameID <= EDD_SRT_FRAMEID_ASRT_DCP_MC_RSP  )
                   )
                {
                    if ( FrameID == EDD_SRT_FRAMEID_ASRT_DCP_HELLO )
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX;
                    }
                    else
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_DCP_IDX;
                    }
                }
                else
                {
                    if(  ( (EDDS_PTCP_SYNC_FOLLOWUP_START <= FrameID) && (EDDS_PTCP_SYNC_FOLLOWUP_END >= FrameID) )
                      || ( (EDDS_PTCP_SYNC_START <= FrameID) && (EDDS_PTCP_SYNC_END >= FrameID) )
                      || ( (EDDS_PTCP_FOLLOWUP_START <= FrameID) && (EDDS_PTCP_FOLLOWUP_END >= FrameID) )
                      )
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_PTP_SYNC_IDX;
                    }
                    else if( (( EDDS_PTCP_DELAY_START <= FrameID  ) && ( EDDS_PTCP_DELAY_END >= FrameID  )))
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_PTP_DELAY_IDX;
                    }
                    else if( (( EDDS_PTCP_ANNO_START <= FrameID  ) && ( EDDS_PTCP_ANNO_END >= FrameID  )))
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX;
                    }
                    else if ( FrameID == EDD_MRRT_FRAMEID ) /* check for MRRT */
                    {
                        FilterType = EDDS_NRT_FILTER_OTHER_MRP_IDX;
                    }
                    else
                    {
                        FilterType = EDDS_NRT_FILTER_NOT_USED;
                    }
                }
            }
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            else /* EDDS_NRT_DSCR_STATE_IN_USE_UDP */
            {
                /* aSRT over UDP ALARM frames */
                EDDS_UDP_XRT_FRAME_PTR_TYPE pHeader;
                LSA_UINT16                  FrameID;
                pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pBuffer[(VLANTagPresent?EDDS_FRAME_HEADER_SIZE_WITH_VLAN:EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN)];
                FrameID = EDDS_NTOHS(pHeader->FrameID); /* get FrameID */
                /* check for DCP */
                if (( FrameID >= EDD_SRT_FRAMEID_ASRT_DCP_HELLO ) &&
                    ( FrameID <= EDD_SRT_FRAMEID_ASRT_DCP_MC_RSP  ))
                {
                    if ( FrameID == EDD_SRT_FRAMEID_ASRT_DCP_HELLO )
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX;
                        /* Check DCP-Filter .. */
                        /* not implemented yet. Frameformat not spezified yet */
                    }
                    else
                    {
                        FilterType = EDDS_NRT_FILTER_XRT_UDP_DCP_IDX;
                        /* Check DCP-Filter .. */
                        /* not implemented yet. Frameformat not spezified yet */
                    }
                }
                else
                {
                    /* Note: PTP not with UDP ! */
                    /* no supported FrameID     */
                    FilterType = EDDS_NRT_FILTER_NOT_USED;
                }
            }
            #endif
            break;
        default:
            FilterType = EDDS_NRT_FILTER_NOT_USED;
            break;
    } /* end switch(BufferType) */

    LSA_UNUSED_ARG(pDDB);
    return FilterType;
}

/**
 * \brief       Functionname        EDDS_NRTFilterARPFrame
 * \detail      Description         checks if the Ethernet Frame matches a set ARP filter \n
 *                                  <b>called from RQB and scheduler context</b>
 *
 * @param[in]   pDDB                Pointer to DeviceDescriptionBlock
 * @param[in]   pBuffer             Pointer to Framebuffer with xRToverUDP-Frame
 * @param[in]   VLANTagPresent      LSA_TRUE VLAN Tag within frame
 *
 * @return      LSA_TRUE            the frame matches a set filter and has to be forwarded to application
 *              LSA_FALSE           the frame doesn't match any filter and can be dropped
 */
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_NRTFilterARPFrame(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pBuffer,
    LSA_BOOL                    VLANTagPresent)
{
    LSA_BOOL                valid_frame;
    EDDS_IP_FRAME_PTR_TYPE  pIPHeader;
    LSA_UINT32              TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    /* by default we assume that the frame matches a filter */
    valid_frame = LSA_TRUE;

    if ( VLANTagPresent )
    {
        pIPHeader = (EDDS_IP_FRAME_PTR_TYPE) &pBuffer[EDDS_FRAME_HEADER_SIZE_WITH_VLAN];  //lint !e826 JB 17/11/2014 pointer cast on purpose
    }
    else
    {
        pIPHeader = (EDDS_IP_FRAME_PTR_TYPE) &pBuffer[EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN]; //lint !e826 JB 17/11/2014 pointer cast on purpose
    }

    /*-------------------------------------------------------------------*/
    /* Filtering ARP-Frames if a filter is present. Only ARP request with*/
    /* a target IP present within the filter table will be forwarded.    */
    /* But gratuitous ARP will never be filtered out and always forwarded*/
    /*-------------------------------------------------------------------*/
    if ( pDDB->pNRT->ARPFilter.InUse )
    {
        EDDS_ARP_FRAME_PTR_TYPE pARPFrame;
        EDDS_ARP_FRAME_FILTER_PTR pARPFilter;
        LSA_UINT32             i,j,n;

        pARPFrame  = (EDDS_ARP_FRAME_PTR_TYPE) ((LSA_UINT8*)pIPHeader);  //lint !e826 pIPHeader also points to start of ARP content
        pARPFilter = &pDDB->pNRT->ARPFilter.ARPFilter;

        /* check for a valid ARP-Request to apply the filter */
        if ( (pARPFrame->Hln == EDDS_ARP_HLN                    ) &&
             (pARPFrame->Hrd == EDDS_HTONS(EDDS_ARP_HRD_ETHERNET) ) && //lint !e572 !e778 correct shift operation
             (pARPFrame->Op  == EDDS_HTONS(EDDS_ARP_OP_REQUEST) ) && //lint !e572 !e778 correct shift operation
             (pARPFrame->Pln == EDDS_ARP_PLN                    ) &&
             (pARPFrame->Pro == EDDS_HTONS(EDDS_ARP_PRO_IP)     )) //lint !e572 !e778 correct shift operation
        {
            /* Copy IP address from slow frame-memory to faster one           */
            /* and check for a gratuitous ARP request                         */
            EDD_IP_ADR_TYPE         ARP_Spa;  /* sender IP of ARP */
            EDD_IP_ADR_TYPE         ARP_Tpa;  /* target IP of ARP */

            j=0;
            for ( i=0; i<EDD_IP_ADDR_SIZE; i++)
            {
                ARP_Spa.IPAdr[i] = pARPFrame->Spa[i];
                ARP_Tpa.IPAdr[i] = pARPFrame->Tpa[i];
                if ( ARP_Spa.IPAdr[i] == ARP_Tpa.IPAdr[i]) j++; /* counter for Gratuitous ARP check */
            }

            /* First we check for gratuitous ARP. . Gratuitous ARP requests   */
            /* are ARP request with identically TargetIP and SenderIP address */
            /* within the ARP request. These request maybe sent by the owner  */
            /* of the IP address to force an update of the ARP cache within   */
            /* targets.                                                       */

            if ( j != EDD_IP_ADDR_SIZE ) /* not a gratiutious arp ? */
            {
                EDD_IP_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pIPAddr;
                /* we assume that the frame doesn't match the filter. this will be undone if we find a match */
                valid_frame = LSA_FALSE;

                n = 0;   /* filter counter */

                /* check all IP-addresses for a matching Target IP-address   */
                while (  ( !valid_frame )
                      && ( n<pARPFilter->FilterCnt)
                      )
                {
                    pIPAddr = &pARPFilter->IpAddrForLookup[n]; /* current ip address */
                    i = 0;
                    while ((i<EDD_IP_ADDR_SIZE) &&
                           (ARP_Tpa.IPAdr[i] == pIPAddr->IPAdr[i]))
                    {
                        i++;
                    }

                    if ( i == EDD_IP_ADDR_SIZE)
                    {
                        /* filter matches. Tpa equal */
                        valid_frame = LSA_TRUE;
                        break;  /* immediatly stop looking if ip address found */
                    }
                    n++; /* next filter */
                } /* while */

                if ( !valid_frame )
                {
                    /* no matching filter found! */
                    EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE, "EDDS_NRTGetFilterIPARPFrame: ARP request filtered!");
                    pDDB->pNRT->Stats.RxARPFilterDrop++;
                }
            }
            else
            {
                /* gratuitous will not be filtered! */
                EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_NRTGetFilterIPARPFrame: gratuitous ARP not filtered!");
                valid_frame = LSA_TRUE;
            }
        }
        else
        {
            /* no (valid) ARP-request. not be filtered! */
            EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_NRTGetFilterIPARPFrame: ARP is no (valid) ARP request. not filtered!");
            valid_frame = LSA_TRUE;
        }
    }

    LSA_UNUSED_ARG(TraceIdx);

    return valid_frame;
}

/**
 * \brief       Functionname        EDDS_NRTFilterXRTFrame
 * \detail      Description         checks if the Ethernet Frame matches a set DCP/DCPHELLO filter \n
 *                                  For DCP frames there is a optional IDENTIFY-TLV-Filter. \n
 *                                  <b>called from RQB and scheduler context</b>
 *
 * @param[in]   pDDB                Pointer to DeviceDescriptionBlock
 * @param[in]   pBuffer             Pointer to Framebuffer with xRToverUDP-Frame
 * @param[in]   Length              Length of Framebuffer
 * @param[in]   VLANTagPresent      LSA_TRUE VLAN Tag within frame
 * @param[in]   FilterType          Filtertype (NRT_FILTER_XRT_UDP_DCP_IDX or NRT_FILTER_XRT_UDP_DCP_HELLO_IDX)
 * @param[out]  pFrameFilterInfo    Address for FrameFilterInfo
 *
 * @return      LSA_TRUE            the frame matches a set filter and has to be forwarded to application
 *              LSA_FALSE           the frame doesn't match any filter and can be dropped
 */
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_NRTFilterXRTFrame(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pBuffer,
    LSA_UINT32                  Length,
    LSA_BOOL                    VLANTagPresent,
    LSA_UINT32                  FilterType,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pFrameFilterInfo)
{
    LSA_BOOL                    valid_frame;
    LSA_UINT16                  FrameID;
    EDD_UPPER_MEM_U8_PTR_TYPE   pTLVFrame;
    EDDS_NRT_DCP_HELLO_FILTER_MGM_PTR pHelloFilterEntry;
    LSA_UINT8                    Adjust;
    LSA_UINT16                   FrameDCPLength;
    LSA_UINT16                   FilterValueLength;
    LSA_UINT32                   i;
    LSA_UINT32                   TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    /* by default we assume that the frame does match the filter */
    valid_frame = LSA_TRUE;

    *pFrameFilterInfo = 0;
    if ( VLANTagPresent )
        FrameID = EDDS_NTOHS(((EDD_UPPER_MEM_U16_PTR_TYPE)(pBuffer))[EDDS_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET]);  //lint !e826  JB 19/11/2014 pointer cast on purpose | *pBuffer (unknown) | no unknown null-ptr
    else
        FrameID = EDDS_NTOHS(((EDD_UPPER_MEM_U16_PTR_TYPE)(pBuffer))[EDDS_FRAME_XRT_FRAME_ID_WORD_OFFSET]);  //lint !e826  JB 19/11/2014 pointer cast on purpose | *pBuffer (unknown) | no unknown null-ptr

    switch(FilterType)
    {
        case EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX:
        {
            /* ---------------------------------------------------------------*/
            /* Check HELLO Filtertable                                        */
            /* Only HELLO Frames with a StationName stored within FilterTable */
            /* will be forwarded. The StationName TLV has to be the first TLV */
            /* within the frame.                                              */
            /* ---------------------------------------------------------------*/

            /* by default we assume that the frame doesn't match the filter */
            valid_frame = LSA_FALSE;

            if ( VLANTagPresent ) Adjust = EDD_VLAN_TAG_SIZE;
            else  Adjust = 0;

            /* Note: Start of Length-Field has to be WORD-aligned!!! */
            FrameDCPLength = EDDS_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)(&pBuffer[EDDS_FRAME_DCP_LEN_OFFSET+Adjust])); //lint !e826 JB 17/11/2014 pointer cast on purpose

            /* ----------------------------------------------------------*/
            /* Validate FrameDCPlength against frame-length              */
            /* ----------------------------------------------------------*/
            if ( (LSA_UINT32)(FrameDCPLength +  EDDS_FRAME_DCP_UC_HEADER_SIZE + Adjust)  <= Length )
            {
                /* frame ok. now check if we have a TLV   */
                /* Length big enough to hold a TLV-Field? */

                if ( FrameDCPLength >= EDDS_DCP_TLV_TLSIZE )
                {
                    pTLVFrame  = &pBuffer[EDDS_FRAME_DCP_TLV_OFFSET+Adjust]; /* first TLV */

                    /* check for NameOfStation TLV */
                    if (( pTLVFrame[EDDS_DCP_TLV_TYPE_OFFSET] == EDDS_DCP_DEVICE_PROPERTIES_OPTION  ) &&
                        ( pTLVFrame[EDDS_DCP_TLV_TYPE_OFFSET+1] == EDDS_DCP_SUBOPTION_NAME_OF_STATION ))  //lint !e835 JB 17/11/2014 define-based behaviour
                    {
                        /* get length */
                        FilterValueLength = EDDS_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)(&pTLVFrame[EDDS_DCP_TLV_LENGTH_OFFSET])); //lint !e826 JB 17/11/2014 pointer cast on purpose

                        /* length ok? (Value contains Blockinfo) */
                        if ( FilterValueLength >= EDDS_DCP_TLV_RES_BLOCKINFO_SIZE )
                        {
                            FilterValueLength  -= EDDS_DCP_TLV_RES_BLOCKINFO_SIZE;                          /* Length of NameOfStation  */
                            pTLVFrame          += EDDS_DCP_TLV_VALUE_OFFSET+EDDS_DCP_TLV_RES_BLOCKINFO_SIZE; /* Pointer to NameOfStation */

                            /* Now search filter table for a matching entry */

                            i = 0;
                            pHelloFilterEntry = &pDDB->pNRT->pDCPHelloFilter->Filter[0];

                            /* loop till matching filter found or end of filtertable */
                            while (  (!valid_frame)
                                  && (i<EDD_DCP_MAX_DCP_HELLO_FILTER)
                                  )
                            {
                                /* if filterentry in use and length matches.. */
                                if ( (pHelloFilterEntry->InUse) &&
                                     (pHelloFilterEntry->StationNameLen == FilterValueLength ))
                                {
                                    /* compare nameofstation */
                                    if (EDDS_NRTFrameMemcmp(pTLVFrame,pHelloFilterEntry->StationName,FilterValueLength))
                                    {
                                        *pFrameFilterInfo = pHelloFilterEntry->UserID;
                                        valid_frame = LSA_TRUE;
                                    }
                                }

                                pHelloFilterEntry++;
                                i++;
                            }

                            if ( !valid_frame )
                            {
                                EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                                      "EDDS_NRTGetFilterXRTFrame: Filtered out DCP-HELLO frame (RxDCPFilterDrop)");

                                pDDB->pNRT->Stats.RxDCPFilterDrop++;
                            }
                        }
                        else
                        {
                            EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                                  "EDDS_NRTGetFilterXRTFrame: Invalid DCP HELLO frame (RxDCPFilterInvalidFrame)");
                            pDDB->pNRT->Stats.RxDCPFilterInvalidFrame++;
                        }
                    }
                }
            }
            else /* invalid length */
            {
                EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                      "EDDS_NRTGetFilterXRTFrame: Invalid DCP HELLO frame (RxDCPFilterInvalidFrame)");

                pDDB->pNRT->Stats.RxDCPFilterInvalidFrame++;
            }
            break;
        }
        case EDDS_NRT_FILTER_XRT_DCP_IDX:
        {
            LSA_BOOL FilterMatch;
            EDD_UPPER_MEM_U8_PTR_TYPE    pTLVFilter;

            /* by default we assume that the frame does match the filter */
            valid_frame = LSA_TRUE;

            /* Check DCP-Filter .. */
            pTLVFilter = (EDD_UPPER_MEM_U8_PTR_TYPE)pDDB->pNRT->DCPFilter[0].pDCP_TLVFilter;

            /* if at least one DCP Filter is present .. */
            if ( ! LSA_HOST_PTR_ARE_EQUAL(pTLVFilter,LSA_NULL))
            {
                if ( VLANTagPresent ) Adjust = EDD_VLAN_TAG_SIZE;
                else  Adjust = 0;

                /* ---------------------------------------------------------------*/
                /* Filtering is done as follows:                                  */
                /* We have to check the TLV-Filter if..                           */
                /* * it is a IDENTIFY MC-Request  (FrameID check)                 */
                /* * AND a TLV is present (length > 0)                            */
                /* * AND the T-field is equal to the a filter T-Field.            */
                /* If all is true we check if the LV matches the Filter           */
                /* Note: The buffer has a size of at least 60 bytes.              */
                /* ---------------------------------------------------------------*/

                if ( FrameID == EDD_SRT_FRAMEID_ASRT_DCP_MC_REQ)
                {
                    /* Note: Start of Length-Field has to be WORD-aligned!!! */
                    FrameDCPLength = EDDS_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)(&pBuffer[EDDS_FRAME_DCP_LEN_OFFSET+Adjust])); //lint !e826 JB 17/11/2014 pointer cast on purpose

                    /* ----------------------------------------------------------*/
                    /* Validate FrameDCPlength against frame-length              */
                    /* ----------------------------------------------------------*/
                    if ( (LSA_UINT32)(FrameDCPLength +  EDDS_FRAME_DCP_MC_HEADER_SIZE + Adjust)  <= Length )
                    {
                        /* frame ok. now check if we have a TLV   */
                        /* Length big enough to hold a TLV-Field? */

                        if ( FrameDCPLength >= EDDS_DCP_TLV_TLSIZE )
                        {
                            i = 0;
                            FilterMatch = LSA_FALSE; /* no filter fit yet */

                            /* check all filters. If at least one filter type matches the T-field of the   */
                            /* first TLV at least on filter must match. If not the frame gets discarded    */
                            /* If no filter-type matches the T -field at all, no filter will be applied.   */
                            do
                            {
                                pTLVFrame  = &pBuffer[EDDS_FRAME_DCP_TLV_OFFSET+Adjust]; /* first TLV */

                                /* check if Type of TLV-field matches Filter-TLV. Note that first TLV is   */
                                /* WORD-aligned so we can access with 16-Bit.                              */

                                /* compare type-field */
                                if ( EDDS_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)(&pTLVFrame[EDDS_DCP_TLV_TYPE_OFFSET])) == pDDB->pNRT->DCPFilter[i].Type ) //lint !e826 JB 17/11/2014 pointer cast on purpose
                                {
                                    FilterValueLength    = pDDB->pNRT->DCPFilter[i].ValueLength;

                                    /* compare length-field */
                                    if ( EDDS_NTOHS(*(EDD_UPPER_MEM_U16_PTR_TYPE)(&pTLVFrame[EDDS_DCP_TLV_LENGTH_OFFSET])) == FilterValueLength ) //lint !e826 JB 17/11/2014 pointer cast on purpose
                                    {

                                        pTLVFrame     += EDDS_DCP_TLV_VALUE_OFFSET; /* point to Value-Field */
                                        pTLVFilter    += EDDS_DCP_TLV_VALUE_OFFSET; /* point to Value-Field */

                                        /* now check Value - Field. if FilterValueLength is 0 we continue   */
                                        while ( FilterValueLength && (*pTLVFrame++ == *pTLVFilter++))
                                        {
                                            FilterValueLength--;
                                        }

                                        if ( FilterValueLength ) /* Value doesnt match */
                                        {
                                            valid_frame = LSA_FALSE;
                                        }
                                        else
                                        {
                                            /* We found a matching Filter. Dont discard */
                                            FilterMatch = LSA_TRUE;
                                            valid_frame = LSA_TRUE;
                                        }
                                    }
                                    else /* Length doesnt match */
                                    {
                                        valid_frame = LSA_FALSE;
                                    }
                                } /* Correct Type */

                                i++;
                                if ( i < EDDS_NRT_DCP_FILTER_CNT ) pTLVFilter = (EDD_UPPER_MEM_U8_PTR_TYPE)pDDB->pNRT->DCPFilter[i].pDCP_TLVFilter;

                            }
                            while ( ( !FilterMatch ) &&
                                    ( i< EDDS_NRT_DCP_FILTER_CNT ) &&
                                    ( ! LSA_HOST_PTR_ARE_EQUAL(pTLVFilter,LSA_NULL)));

                            if ( !valid_frame )
                            {
                                EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                                      "EDDS_NRTGetFilterXRTFrame: Filtered out DCP frame (RxDCPFilterDrop)");

                                pDDB->pNRT->Stats.RxDCPFilterDrop++;
                            }


                        } /* TLV present */
                    }
                    else /* invalid length */
                    {

                        EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_UNEXP,
                                              "EDDS_NRTGetFilterXRTFrame: Invalid DCP frame (RxDCPFilterInvalidFrame)");

                        valid_frame = LSA_FALSE;
                        pDDB->pNRT->Stats.RxDCPFilterInvalidFrame++;
                    }
                } /* IDENTIFY */
            } /* Filter active */
            break;
        }
        default:
            break;
    }

    LSA_UNUSED_ARG(TraceIdx);

    return valid_frame;
}

/**
 * \brief       Functionname        EDDS_applyFilterType
 * \detail      Description         checks if the Ethernet Frame matches a set filter \n
 *                                  <b>called from scheduler context</b>
 *
 * @param[in]   pDDB                Pointer to DeviceDescriptionBlock
 * @param[in]   pDscr
 * @param[in]   VLANTagPresent      LSA_TRUE VLAN Tag within frame
 * @param[in]   FilterType          Filtertype (@see edds_nrt_inc.h)
 * @param[out]  pFrameFilterInfo    Address for FrameFilterInfo
 *
 * @return      LSA_TRUE            the frame matches a set filter and has to be forwarded to application
 *              LSA_FALSE           the frame doesn't match any filter and can be dropped
 */
static LSA_BOOL EDDS_NRTApplyFilterType(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
    EDDS_NRT_DSCR_TYPE_PTR_TO_CONST pDscr,
    LSA_BOOL                        VLANTagPresent,
    LSA_UINT32                      FilterType,
    EDDS_LOCAL_MEM_U32_PTR_TYPE     pFrameFilterInfo)
{
    LSA_BOOL valid_frame;
    EDD_UPPER_MEM_U8_PTR_TYPE pBuffer;

    valid_frame = LSA_TRUE;
    pBuffer = pDscr->pBuffer;

    switch(FilterType)
    {
        /* Third-Stage filtering of IP/ARP frames  */
        /* EDD_NRT_FRAME_IP_ICMP                   */
        /* EDD_NRT_FRAME_IP_IGMP                   */
        /* EDD_NRT_FRAME_IP_TCP                    */
        /* EDD_NRT_FRAME_IP_UDP                    */
        /* EDD_NRT_FRAME_IP_VRRP                      */
        /* EDD_NRT_FRAME_IP_OTHER                  */
        /* EDD_NRT_FRAME_ARP                       */
        /* EDD_NRT_FRAME_RARP                      */
        case EDDS_NRT_FILTER_RARP_IDX:
            break;
        case EDDS_NRT_FILTER_ARP_IDX:
            valid_frame = EDDS_NRTFilterARPFrame(pDDB,pDscr->pBuffer,VLANTagPresent);
            break;
        case EDDS_NRT_FILTER_IP_ICMP_IDX:
        case EDDS_NRT_FILTER_IP_IGMP_IDX:
        case EDDS_NRT_FILTER_IP_TCP_IDX:
        case EDDS_NRT_FILTER_IP_UDP_IDX:
        case EDDS_NRT_FILTER_IP_VRRP_IDX:
        case EDDS_NRT_FILTER_IP_OTHER_IDX:
            break;

        /* Third-Stage filtering of ASRT ALARM frames  */
        /* EDD_NRT_FRAME_ASRT                          */
        /* EDD_NRT_FRAME_UDP_ASRT                      */
        case EDDS_NRT_FILTER_XRT_ASRT_IDX:
        case EDDS_NRT_FILTER_XRT_UDP_ASRT_IDX:
            break;

        /* EDD_NRT_FRAME_OTHER                         */
        /* EDD_NRT_FRAME_LLDP                          */
        /* EDD_NRT_FRAME_MRP (MRP)                     */
        /* EDD_NRT_FRAME_HSYNC                         */
        case EDDS_NRT_FILTER_OTHER_LLDP_IDX:
        case EDDS_NRT_FILTER_OTHER_MRP_IDX:
        case EDDS_NRT_FILTER_OTHER_IDX:
        case EDDS_NRT_FILTER_HSYNC_IDX:
            break;

        /* Third-Stage filtering of OTHER-Type Frames  */
        /* EDD_NRT_FRAME_LEN_DCP                       */
        /* EDD_NRT_FRAME_LEN_TSYNC                     */
        /* EDD_NRT_FRAME_LEN_OTHER                     */
        /* EDD_NRT_FRAME_LEN_STDBY                     */
        case EDDS_NRT_FILTER_LEN_DCP_IDX:
        case EDDS_NRT_FILTER_LEN_TSYNC_IDX:
        case EDDS_NRT_FILTER_LEN_STDBY_IDX:
        case EDDS_NRT_FILTER_LEN_SINEC_FWL_IDX:
        case EDDS_NRT_FILTER_LEN_OTHER_IDX:
        case EDDS_NRT_FILTER_LEN_HSR_IDX:
            break;

        /* Third-Stage filtering of ASRT-Other frames  */
        /* EDD_NRT_FRAME_DCP                           */
        /* EDD_NRT_FRAME_UDP_DCP                       */
        /* EDDS_NRT_FRAME_PTP                          */
        /* EDD_NRT_FRAME_MRP (MRRT)                    */
        case EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX:
        case EDDS_NRT_FILTER_XRT_DCP_IDX:
            valid_frame = EDDS_NRTFilterXRTFrame(pDDB,
                                                 pDscr->pBuffer,
                                                 pDscr->Length,
                                                 VLANTagPresent,
                                                 FilterType,
                                                 pFrameFilterInfo);
            break;
        case EDDS_NRT_FILTER_XRT_PTP_SYNC_IDX:
        case EDDS_NRT_FILTER_XRT_PTP_DELAY_IDX:
        case EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX:
        //case EDDS_NRT_FILTER_OTHER_MRP_IDX: was in two different filter sections previously ???
            break;
        #ifdef EDDS_XRT_OVER_UDP_INCLUDE
        /* EDDS_NRT_DSCR_STATE_IN_USE_UDP              */
        case EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX:
        case EDDS_NRT_FILTER_XRT_UDP_DCP_IDX:
            break;
        #endif
        /* default */
        case EDDS_NRT_FILTER_NOT_USED:
        default:
            valid_frame = LSA_FALSE;
            break;

    }

    LSA_UNUSED_ARG(pBuffer);
    return valid_frame;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTRxHandleDscr                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Results    : LSA_TRUE:    a frame was processed. call NRTReceive again +*/
/*+               LSA_FALSE:   no frame present.                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Is called after one or more frames where received and put +*/
/*+               into the nrt-rx-ring queue. We search the queue and       +*/
/*+               finish one request waiting for frames. We will use the    +*/
/*+               Filter infos to select the corresponding request.         +*/
/*+                                                                         +*/
/*+               The Buffer from the request (pBuffer) is exchanged with   +*/
/*+               a buffer from the actual descriptor ring. (zero copy      +*/
/*+               interface). So ther requestor dont gets his original      +*/
/*+               buffer returned!!!                                        +*/
/*+                                                                         +*/
/*+               If the function returns with LSA_TRUE the function has    +*/
/*+               to be called again till it returns LSA_FALSE.             +*/
/*+                                                                         +*/
/*+               We dont loop inside because the caller may want to inter- +*/
/*+               cept and do some higher prior tasks.                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 checked where first called
////WARNING: if you call this function, make sure not to use pDscr as null ptr!
static LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRxHandleDscr(
    EDDS_LOCAL_DDB_PTR_TYPE    pDDB,
    EDDS_NRT_DSCR_TYPE_PTR     pDscr,
    EDDS_NRT_DSCR_MGM_TYPE_PTR pDscrMgm,
    LSA_UINT32                 BufferType)
{
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE    pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE              pRQB;
    LSA_UINT32                          RxQueueIdx;
    EDDS_LOCAL_HDB_PTR_TYPE             pHDB;
    EDD_UPPER_NRT_RECV_PTR_TYPE         pRQBRcv;
    LSA_UINT32                          FilterType;
    LSA_UINT32                          FrameFilterInfo;
    LSA_BOOL                            valid_frame;
    LSA_BOOL                            VLANTagPresent;
    LSA_UINT32                          TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_SCHED_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "IN :EDDS_NRTRxHandleDscr(pDDB: 0x%X)",
        pDDB);

    VLANTagPresent  = LSA_FALSE;
    FrameFilterInfo = 0;
    pNRTComp        = pDDB->pNRT;

    /* Check if VLAN-Tag present. Needed later on .. */
    /* pBuffer is 32-Bit aligned!                    */

    if ( EDDS_FRAME_GET_LEN_TYPE(pDscr->pBuffer) == EDDS_VLAN_TAG )  //lint !e826 JB 17/11/2014 pointer cast on purpose
    {
        VLANTagPresent = LSA_TRUE;
    }


    // first we get the filter type
    FilterType = EDDS_NRTGetFilterType(pDDB, BufferType, pDscr, VLANTagPresent);

    // then we have to check if the filter is actually used
    // if filter is not used, just drop the frame immediately
    if(EDDS_NRT_FILTER_NOT_USED != FilterType && pNRTComp->FilterInfo[FilterType].InUse)
    {
        /* ------------------------------------------------------------------*/
        /* get handle of requestor                                           */
        /* ------------------------------------------------------------------*/
        pHDB = pNRTComp->FilterInfo[FilterType].pHDB;
        /* --------------------------------------------------------------*/
        /* get queue idx to be used and unqueue the oldest request.      */
        /* --------------------------------------------------------------*/
        RxQueueIdx = pHDB->pNRT->RxQueueIdx;

        // Filter is used.
        // if there is a cancel in progress or we dont have free resources
        // we drop the frame and increment the dropcount for the filtertype
        if(  !(pHDB->pNRT->CancelInProgress)
          && !(LSA_HOST_PTR_ARE_EQUAL(pNRTComp->RxReq[RxQueueIdx].pBottom, LSA_NULL))
          )
        {
            #ifdef EDDS_CFG_FILTER_FRAME_QUEUE_NRT_RCV_ENABLE
            if(  (EDDS_NRT_FILTER_ARP_IDX == FilterType)
              || (EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX == FilterType)
              || (EDDS_NRT_FILTER_XRT_DCP_IDX == FilterType)
              )
            {
                valid_frame = LSA_TRUE;
            }
            else
            #endif // EDDS_CFG_FILTER_FRAME_QUEUE_NRT_RCV_ENABLE
            {
                // apply Filter to Frame
                valid_frame = EDDS_NRTApplyFilterType(pDDB, pDscr, VLANTagPresent, FilterType, &FrameFilterInfo);
            }
            // Frame is valid finish it!
            if(valid_frame)
            {
                EDDS_RQB_REM_BLOCK_BOTTOM(pNRTComp->RxReq[RxQueueIdx].pBottom,
                                          pNRTComp->RxReq[RxQueueIdx].pTop,
                                          pRQB);

                pHDB->pNRT->RxQueueReqCnt--;                /*  requests in queue  */


                pRQBRcv = (EDD_UPPER_NRT_RECV_PTR_TYPE) pRQB->pParam;

                if(!pDDB->pGlob->ZeroCopyInterface)
                {
                    EDDS_MEMCOPY_UPPER(pRQBRcv->pBuffer, pDscr->pBuffer, pDscr->Length);
                    pRQBRcv->IOCount         = (LSA_UINT16) pDscr->Length;
                    pRQBRcv->PortID          = (LSA_UINT16) pDscr->PortID;
                    pRQBRcv->FrameFilter     = pNRTComp->FilterInfo[FilterType].FrameFilter;
                    pRQBRcv->FrameFilterInfo = FrameFilterInfo;

                    if(EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX == FilterType)
                    {
                        int c;
                        for(c=0;c<EDD_CFG_MAX_PORT_CNT;++c)
                        {
                            pRQBRcv->doSwFwd[c] = EDD_NRT_RECV_DO_FWD_BY_UPPER_LAYER;
                        }
                    }
                }
                else
                {
                    EDD_UPPER_MEM_U8_PTR_TYPE       pBuffer;
                    /* ----------------------------------------------------------*/
                    /* we exchange the bufferpointer and finish the request.     */
                    /* ----------------------------------------------------------*/

                    pBuffer                  = pRQBRcv->pBuffer;
                    pRQBRcv->IOCount         = (LSA_UINT16) pDscr->Length;
                    pRQBRcv->PortID          = (LSA_UINT16) pDscr->PortID;
                    pRQBRcv->pBuffer         = pDscr->pBuffer;
                    pRQBRcv->FrameFilter     = pNRTComp->FilterInfo[FilterType].FrameFilter;
                    pRQBRcv->FrameFilterInfo = FrameFilterInfo;

                    if(EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX == FilterType)
                    {
                        int c;
                        for(c=0;c<EDD_CFG_MAX_PORT_CNT;++c)
                        {
                            pRQBRcv->doSwFwd[c]      = EDD_NRT_RECV_DO_FWD_BY_UPPER_LAYER;
                        }
                    }

                    pDscr->pBuffer           = pBuffer;  /* use pBuffer from request */
                }

                if ( VLANTagPresent )
                {
                    /* Note. IOCount is at least 60 Bytes because of Min-Ethernetframesize */
                    pRQBRcv->UserDataLength = pRQBRcv->IOCount - EDDS_FRAME_HEADER_SIZE_WITH_VLAN;
                    pRQBRcv->UserDataOffset = EDDS_FRAME_HEADER_SIZE_WITH_VLAN;
                }
                else
                {
                    /* Note. IOCount is at least 60 Bytes because of Min-Ethernetframesize */
                    pRQBRcv->UserDataLength = pRQBRcv->IOCount - EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN;
                    pRQBRcv->UserDataOffset = EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN;
                }

                /* set number of remaining rx-requests */

                pRQBRcv->RequestCnt = pHDB->pNRT->RxQueueReqCnt;
                pRQBRcv->RxTime     = 0; /* not supported */

                /* finish request */

                EDDS_SCHED_TRACE_08(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                      "EDDS_NRTRxHandleDscr: pHDB: 0x%X, HDBIndex: %d, pRQB: 0x%X, pBuffer: 0x%X, Port: %d, IOCount: %d, Filter: 0x%X, RQBCnt: %d)",
                                      pHDB,
                                      pHDB->HDBIndex,
                                      pRQB,
                                      pRQBRcv->pBuffer,
                                      pRQBRcv->PortID,
                                      pRQBRcv->IOCount,
                                      pRQBRcv->FrameFilter,
                                      pRQBRcv->RequestCnt);

                #ifdef EDDS_CFG_FILTER_FRAME_QUEUE_NRT_RCV_ENABLE
                /* ARP DCP and DCP_HELLO Frames weren't filtered yet                                */
                /* the frame buffer is copied to the RQB                                            */
                /* we push the RQB into the RQB-Context where the filter will be applied            */
                /* the RQB-Context will push the RQB back to the RQB-Queue if the frame is dropped  */
                // ARP
                if(EDDS_NRT_FILTER_ARP_IDX == FilterType)
                {
                    EDDS_RQB_LIST_TYPE_PTR refFilterARPFrameQueue = &pDDB->pGlob->FilterARPFrameQueue;
                    EDDS_RQB_PUT_BLOCK_TOP(refFilterARPFrameQueue->pBottom,refFilterARPFrameQueue->pTop,pRQB);
                    pDDB->pGlob->FilterARPFrameTriggerPending = LSA_TRUE;
                }
                // DCP
                else if (EDDS_NRT_FILTER_XRT_DCP_IDX == FilterType)
                {
                    EDDS_RQB_LIST_TYPE_PTR refFilterDCPFrameQueue = &pDDB->pGlob->FilterDCPFrameQueue;
                    EDDS_RQB_PUT_BLOCK_TOP(refFilterDCPFrameQueue->pBottom,refFilterDCPFrameQueue->pTop,pRQB);
                    pDDB->pGlob->FilterDCPFrameTriggerPending = LSA_TRUE;
                }
                // DCP_HELLO
                else if (EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX == FilterType)
                {
                    EDDS_RQB_LIST_TYPE_PTR refFilterDCPHELLOFrameQueue = &pDDB->pGlob->FilterDCPHELLOFrameQueue;
                    EDDS_RQB_PUT_BLOCK_TOP(refFilterDCPHELLOFrameQueue->pBottom,refFilterDCPHELLOFrameQueue->pTop,pRQB);
                    pDDB->pGlob->FilterDCPHELLOFrameTriggerPending = LSA_TRUE;
                }
                else
                #endif // EDDS_CFG_FILTER_FRAME_QUEUE_NRT_RCV_ENABLE
                {
                    #ifdef EDDS_CFG_FINISHEDGENERALREQUESTQUEUE_NRT_RCV_ENABLE
                    {
                        EDDS_RQB_LIST_TYPE_PTR refFinishedGeneralRequestQueue = &pDDB->pGlob->GeneralRequestFinishedQueue;
                        EDD_RQB_SET_RESPONSE(pRQB,EDD_STS_OK);
                        EDDS_RQB_PUT_BLOCK_TOP(refFinishedGeneralRequestQueue->pBottom,refFinishedGeneralRequestQueue->pTop,pRQB);
                        pDDB->pGlob->GeneralRequestFinishedTriggerPending = LSA_TRUE;
                    }
                    #else
                    {
                        EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
                    }
                    #endif //EDDS_CFG_FINISHEDGENERALREQUESTQUEUE_NRT_RCV_ENABLE
                }
            }

        }
        else
        {
            // drop the frame and increment the counter for the frame
            EDDS_SCHED_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                "EDDS_NRTRxHandleDscr: Dropping frame (no RQBs present): pHDB: 0x%X, FilterType: 0x%X",
                pHDB,
                FilterType);
            switch (FilterType)
            {
                case EDDS_NRT_FILTER_ARP_IDX:
                case EDDS_NRT_FILTER_RARP_IDX:
                case EDDS_NRT_FILTER_IP_ICMP_IDX:
                case EDDS_NRT_FILTER_IP_IGMP_IDX:
                case EDDS_NRT_FILTER_IP_TCP_IDX:
                case EDDS_NRT_FILTER_IP_UDP_IDX:
                case EDDS_NRT_FILTER_IP_VRRP_IDX:
                case EDDS_NRT_FILTER_IP_OTHER_IDX:
                    pNRTComp->Stats.RxARPIP_Drop++;
                    break;
                case EDDS_NRT_FILTER_OTHER_IDX:
                    pNRTComp->Stats.RxOther_Drop++;
                    break;
                case EDDS_NRT_FILTER_LEN_DCP_IDX:
                    pNRTComp->Stats.RxLenDCP_Drop++;
                    break;
                case EDDS_NRT_FILTER_LEN_TSYNC_IDX:
                    pNRTComp->Stats.RxLenTSYNC_Drop++;
                    break;
                case EDDS_NRT_FILTER_LEN_OTHER_IDX:
                case EDDS_NRT_FILTER_LEN_STDBY_IDX:
                case EDDS_NRT_FILTER_LEN_HSR_IDX:
                case EDDS_NRT_FILTER_LEN_SINEC_FWL_IDX:
                    pNRTComp->Stats.RxLenOther_Drop++;
                    break;
                case EDDS_NRT_FILTER_XRT_DCP_IDX:
                case EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX:
                    pNRTComp->Stats.RxXRTDCP_Drop++;
                    break;
                case EDDS_NRT_FILTER_OTHER_LLDP_IDX:
                    pNRTComp->Stats.RxLLDP_Drop++;
                    break;
                case EDDS_NRT_FILTER_XRT_ASRT_IDX:
                    pNRTComp->Stats.RxASRTAlarm_Drop++;
                    break;
                case EDDS_NRT_FILTER_XRT_UDP_ASRT_IDX:
                    pNRTComp->Stats.RxUDP_ASRTAlarm_Drop++;
                    break;
                case EDDS_NRT_FILTER_XRT_UDP_DCP_IDX:
                case EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX:
                    pNRTComp->Stats.RxUDP_XRTDCP_Drop++;
                    break;
                case EDDS_NRT_FILTER_XRT_PTP_SYNC_IDX:
                case EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX:
                case EDDS_NRT_FILTER_XRT_PTP_DELAY_IDX:
                    pNRTComp->Stats.RxPTP_Drop++;
                    break;
                case EDDS_NRT_FILTER_OTHER_MRP_IDX:
                    pNRTComp->Stats.RxMRP_Drop++;
                    break;
                default:
                    break;
            }
        }
    }

    if ( pDscrMgm->RxCheckCnt == (pDscrMgm->RxDscrCnt-1) )
    {
        pDscrMgm->RxCheckCnt = 0;
    }
    else
    {
        pDscrMgm->RxCheckCnt++;
    }

    pDscr->State  = EDDS_NRT_DSCR_STATE_FREE; /* set to free */

    EDDS_SCHED_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
        "OUT:EDDS_NRTRxHandleDscr");

    LSA_UNUSED_ARG(TraceIdx);
}

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRxHandleFrames(
    EDDS_LOCAL_DDB_PTR_TYPE    pDDB)
{
    LSA_BOOL Ready = LSA_FALSE;
    EDDS_NRT_DSCR_TYPE_PTR pDscr;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDDS_NRT_DSCR_MGM_TYPE_PTR pDscrMgm;
    LSA_UINT32 BufferType, lookedUp;
    LSA_UINT32 TraceIdx;

    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_SCHED_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTRxHandleFrames(pDDB: 0x%X)",
                           pDDB);

    pNRTComp = pDDB->pNRT;
    lookedUp = 0;

    while ( ! Ready )
    {
        /* get a dscr from dscr queues */
        pDscr = EDDS_NRTRxGetDscrFromRing(pNRTComp, &pDscrMgm, &BufferType);

        /* valid dscr element present? */
        if ( !LSA_HOST_PTR_ARE_EQUAL(pDscr, LSA_NULL) )
        {
            /* handle dscr element (exchange buffer, get channel handle, callback ...) */
            EDDS_NRTRxHandleDscr(pDDB, pDscr, pDscrMgm, BufferType);

            lookedUp++;
        }
        /* no dscr available, abort */
        else
        {
            Ready = LSA_TRUE;
        }
    }

    /* reset look up counter */
    EDDS_NRTRxResetDscrLookUpCounter(pNRTComp);

    EDDS_SCHED_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTRxHandleFrames(frames sent to user: %d)",
                           lookedUp);

    LSA_UNUSED_ARG(TraceIdx);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTRecv                                +*/
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
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_ALIGN                                         +*/
/*+               EDD_STS_ERR_CHANNEL_USE                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_NRT_RECV-Request                          +*/
/*+               Checks for Parameters and if ok queues request in rx-queue+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 pRQB, pHDB checked before function call
//WARNING: be careful when using this function, make sure not to use pRQB,pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTRecv(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                         Status;
    EDD_UPPER_NRT_RECV_PTR_TYPE        pRQBRcv;
    LSA_UINT32                         RxQueueIdx;
    LSA_UINT32                         TraceIdx;

    EDDS_LOCAL_DDB_PTR_TYPE     	   pDDB;

    pDDB      = pHDB->pDDB;
    TraceIdx = pDDB->pGlob->TraceIdx;

    EDDS_NRT_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTRecv(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRQBRcv    = (EDD_UPPER_NRT_RECV_PTR_TYPE) pRQB->pParam;

    //initialize IOCount (RQ 1839107, Task 1839257)
    pRQBRcv->IOCount = 0;

    if (LSA_HOST_PTR_ARE_EQUAL( pRQBRcv->pBuffer, LSA_NULL))
    {

        EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_NRTRecv: Invalid parameter (pBuffer is NULL)");

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /* check for buffer-alignment */

        if ( ((LSA_UINT32) (pRQBRcv->pBuffer)) & EDDS_RX_FRAME_BUFFER_ALIGN )
        {

            EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_NRTRecv: EDD_SRV_NRT_RECV: Invalid alignment of pBuffer address");

            Status = EDD_STS_ERR_ALIGN;
        }
    }

    if ( Status == EDD_STS_OK)
    {

        /* pNRT-ptr must be valid here if this component uses NRT. */

        RxQueueIdx = pHDB->pNRT->RxQueueIdx;

        if  (( RxQueueIdx != EDDS_NRT_FILTER_NOT_USED     ) &&
             ( RxQueueIdx <= EDDS_NRT_MAX_FILTER_IDX      ))
        {
            /* Because this sequence runs in RQB context and must not */
            /* be interrupted by scheduler we have to use EDDS_ENTER */

            EDDS_ENTER(pDDB->hSysDev);

            EDDS_RQB_PUT_BLOCK_TOP( pDDB->pNRT->RxReq[RxQueueIdx].pBottom,
                                    pDDB->pNRT->RxReq[RxQueueIdx].pTop,
                                    pRQB);

            pHDB->pNRT->RxQueueReqCnt++;/* number of requests in queue  */

            EDDS_EXIT(pDDB->hSysDev);

            EDDS_NRT_TRACE_06(TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                  "EDDS_NRTRecv: Receive RQB queued: pHDB: 0x%X, HDBIndex: %d, pRQB: 0x%X, pBuffer: 0x%X, Filter: 0x%X, RQBCnt: %d)",
                                  pHDB,
                                  pHDB->HDBIndex,
                                  pRQB,
                                  pRQBRcv->pBuffer,
                                  pDDB->pNRT->FilterInfo[RxQueueIdx].FrameFilter,
                                  pHDB->pNRT->RxQueueReqCnt);

        }
        else
        {

            EDDS_NRT_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_NRTRecv: Channel has no filter to receive NRT frames");

            Status = EDD_STS_ERR_CHANNEL_USE;

        }
    }

    EDDS_NRT_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTRecv(), Status: 0x%X",
                           Status);

    LSA_UNUSED_ARG(TraceIdx);

    return(Status);
}


/*****************************************************************************/
/*  end of file NRT_RCV.C                                                    */
/*****************************************************************************/



