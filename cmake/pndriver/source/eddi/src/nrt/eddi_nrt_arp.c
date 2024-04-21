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
/*  F i l e               &F: eddi_nrt_arp.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
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

#define EDDI_MODULE_ID     M_ID_NRT_ARP
#define LTRC_ACT_MODUL_ID  200

#if defined (M_ID_NRT_ARP) //satisfy lint!
#endif

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"

#include "eddi_nrt_arp.h"

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#include "eddi_lock.h"
#endif

/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTInitARPFilter()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_NRTInitARPFilter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    NRT_ARP_FILTER_PTR_TYPE  const  pARPFilter = &pDDB->pLocal_NRT->ARPFilter;
    LSA_UINT32 i;

    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTInitARPFilter->");

    pARPFilter->FilterCnt = 0; //= deactive ARP-Filter
    for (i = 0; i < EDDI_MAX_ARP_FILTER; i++)
    { 
        pARPFilter->Handle[i] = EDDI_ARP_FILTER_INVALID_HANDLE;
        #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON) 
        pARPFilter->IPAddrAsU32SW[i] = 0;
        #endif
        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
        pARPFilter->IPAddrAsU32HW[i] = 0;
        #endif
    }

#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)
    {
        pARPFilter->RefCnt     = 0;
        pARPFilter->ARPPassCtr = 0;
        pARPFilter->ARPDropCtr = 0;
    }
    #endif
    
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
    {
        LSA_UINT32  NRTTransCtrl;

        pARPFilter->pDev_ARP_Filter_Table = &pDDB->pKramFixMem->ARP_FilterTable;

        pDDB->KramRes.ArpFilterTableLen = sizeof(NRT_ARP_FILTER_TABLE_TYPE);

        //calculate and init HW ARP filter table address in asic-format with the right endianess
        {
            LSA_UINT32  const  TmpU32 = DEV_kram_adr_to_asic_register(pARPFilter->pDev_ARP_Filter_Table, pDDB);

            IO_x32(ARP_TABLE_BASE) = EDDI_HOST2IRTE32(TmpU32);
        }

        //set ARP-table-length to minimum
        IO_x32(ARP_TABLE_LENGTH) = 0;

        //HW-ARP-Filter is dynamically switched ON/OFF via bit Enable-ARP-Filter in IRTE-register NRT-Transfer-Control!

        EDDI_ENTER_CRITICAL_S(); //ATTENTION: Register-Read-Modify-Write!

        NRTTransCtrl = IO_x32(NRT_TRANS_CTRL);

        //deactivate ARP-Filter in IRTE-register NRT-Transfer-Control
        EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_ARP_Filter, 0);

        IO_x32(NRT_TRANS_CTRL) = NRTTransCtrl;

        EDDI_EXIT_CRITICAL_S();
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetARPFilter()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_ARP_FILTER)
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetARPFilter( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                       EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE  const  pRqbParam = (EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE)pRQB->pParam;
    LSA_RESULT                                    Status    = EDD_STS_OK;

    EDDI_NRT_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSetARPFilter->");

    if (pRqbParam->UserIndex >= EDDI_MAX_ARP_FILTER)
    {
        EDDI_NRT_TRACE_02(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTSetARPFilter, UserIndex  (%d) bigger than MAX (%d)", 
            pRqbParam->UserIndex , EDDI_MAX_ARP_FILTER);
        return EDD_STS_ERR_PARAM;
    }

    switch (pRqbParam->Mode)
    {
        case EDD_ARP_FILTER_OFF:
        {
            Status = EDDI_NRTSetARPFilterONOFF(pRqbParam, pHDB, LSA_FALSE /*bON*/);
            break;
        }

        case EDD_ARP_FILTER_ON:
        {

            Status = EDDI_NRTSetARPFilterONOFF(pRqbParam, pHDB, LSA_TRUE /*bON*/);
            break;
        }

        default:
            Status = EDD_STS_ERR_PARAM;
            break;
    }

    return Status;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/



/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTSetARPFilter()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_ARP_FILTER)
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_NRTSetARPFilterONOFF( EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE  const  pRqbParam,
                                                            EDDI_LOCAL_HDB_PTR_TYPE                const  pHDB,
                                                            LSA_BOOL                               const  bON)
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB       = pHDB->pDDB;
    NRT_ARP_FILTER_PTR_TYPE  const  pARPFilter = &pDDB->pLocal_NRT->ARPFilter;
    LSA_UINT32                      FilterCount;
    EDD_IP_ADR_TYPE       *  const  pRqbIPAddr = &pRqbParam->IPAddr;
    LSA_UINT32               const  UserIndex = pRqbParam->UserIndex;

    EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_NRTSetARPFilter->Turn ON/OFF (%d) ARP filter for Handle 0x%X, FilterIndex %d.", 
        bON, pHDB->Handle, UserIndex  );

    if (   (EDDI_ARP_FILTER_INVALID_HANDLE != pARPFilter->Handle[UserIndex])
        && (pHDB->Handle != pARPFilter->Handle[UserIndex]) )
    {
        EDDI_NRT_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_NRTSetARPFilter->Filter %d is owned by handle 0x%X, cannot be overwritten by handle 0x%X", 
            UserIndex, pARPFilter->Handle[UserIndex], pHDB->Handle);
        return EDD_STS_ERR_RESOURCE;
    }
    else
    {
        if (bON)
        {
            //New filter or overwrite existent filter
            EDDI_IP_TYPE  IPAdrLocal;

            #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON) 
            //Attention: inverse endianess as the HW filter!
            IPAdrLocal.b.IP[0] = pRqbIPAddr->IPAdr[0];
            IPAdrLocal.b.IP[1] = pRqbIPAddr->IPAdr[1];
            IPAdrLocal.b.IP[2] = pRqbIPAddr->IPAdr[2];
            IPAdrLocal.b.IP[3] = pRqbIPAddr->IPAdr[3];
            pARPFilter->IPAddrAsU32SW[UserIndex] = IPAdrLocal.dw;
            #endif
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
            IPAdrLocal.b.IP[0] = pRqbIPAddr->IPAdr[3];
            IPAdrLocal.b.IP[1] = pRqbIPAddr->IPAdr[2];
            IPAdrLocal.b.IP[2] = pRqbIPAddr->IPAdr[1];
            IPAdrLocal.b.IP[3] = pRqbIPAddr->IPAdr[0];
            pARPFilter->IPAddrAsU32HW[UserIndex] = IPAdrLocal.dw;
            #endif
            pARPFilter->Handle[UserIndex] = pHDB->Handle;
        }
        else
        {
            //Delete filter
            #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON) 
            pARPFilter->IPAddrAsU32SW[UserIndex] = 0;
            #endif
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
            pARPFilter->IPAddrAsU32HW[UserIndex] = 0;
            #endif
            pARPFilter->Handle[UserIndex] = EDDI_ARP_FILTER_INVALID_HANDLE;
        }
    }

    /********************************/
    /*          SW filter           */
    /********************************/
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON) 
    {
        //ARP-Filter is completely implemented in SW!
        volatile  LSA_UINT32  ArpFilterRefCnt = pARPFilter->RefCnt;
        LSA_UINT32  i;
        //RefCnt == even: ARP-Filter-Update currently not in progress!
        //RefCnt == odd:  ARP-Filter-Update currently in progress!

        if //RefCnt odd/invalid?
           (ArpFilterRefCnt & 1UL)
        {
            EDDI_Excp("EDDI_NRTSetARPFilterOn, invalid RefCnt", EDDI_FATAL_ERR_EXCP, ArpFilterRefCnt, 0);
            return EDD_STS_ERR_EXCP;
        }

        //set RefCnt = odd: ARP-Filter-Update currently in progress!
        ArpFilterRefCnt++;
        pARPFilter->RefCnt = ArpFilterRefCnt;

        //resort and copy IP Addresses
        FilterCount = 0;
        for (i=0; i<EDDI_MAX_ARP_FILTER; i++)
        {
            if (EDDI_ARP_FILTER_INVALID_HANDLE != pARPFilter->Handle[i])
            {
                EDDI_IP_TYPE     EDDI_LOCAL_MEM_ATTR  *  const  pARPFilterIPAddr = &pARPFilter->IPAddr[FilterCount];
                #if (EDD_IP_ADDR_SIZE == 4)
                //Set or replace filter
                pARPFilterIPAddr->dw = pARPFilter->IPAddrAsU32SW[i];
                #else
                #error "EDDI: Check Implementation!"
                #endif
                FilterCount++;
            }
        }

        //set RefCnt = even: ARP-Filter-Update finished!
        ArpFilterRefCnt++;
        pARPFilter->RefCnt = ArpFilterRefCnt;
    }
    #endif
    
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
    {
        //ARP-Filter is completely implemented in HW!
        LSA_UINT32  i;
        #if (EDD_IP_ADDR_SIZE == 4)
        LSA_UINT32  *  pARPFilterEntry = (LSA_UINT32 *)(void *)(pARPFilter->pDev_ARP_Filter_Table);
        #endif

        //resort and copy IP Addresses
        FilterCount = 0;
        for (i=0; i<EDDI_MAX_ARP_FILTER; i++)
        {
            if (EDDI_ARP_FILTER_INVALID_HANDLE != pARPFilter->Handle[i])
            {
                #if (EDD_IP_ADDR_SIZE == 4)
                //Set or replace filter
                //copy IP Address to KRAM consistent
                *pARPFilterEntry =  pARPFilter->IPAddrAsU32HW[i];
                #else
                #error "EDDI: Check Implementation!"
                #endif
                pARPFilterEntry++;
                FilterCount++;
            }
        }

        if  /* all filters off */
            (0 == FilterCount)
        {
            #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_CFG_DEFRAG_ON)
            LSA_UINT32  NRTTransCtrl;

            EDDI_ENTER_CRITICAL_S(); //ATTENTION: Register-Read-Modify-Write!

            NRTTransCtrl = IO_x32(NRT_TRANS_CTRL);

            //deactivate ARP-Filter in IRTE-register NRT-Transfer-Control
            EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_ARP_Filter, 0);

            IO_x32(NRT_TRANS_CTRL) = NRTTransCtrl;

            EDDI_EXIT_CRITICAL_S();
            #endif
        }
        else if /* Filtercount changed */
                (FilterCount != pARPFilter->FilterCnt)
        {
            //Update filtercount
            IO_W32(ARP_TABLE_LENGTH, (FilterCount - 1UL));
        }

        if //ARP-Filter not active yet?
           (  !pARPFilter->FilterCnt
            && (FilterCount > 0) )
        {
            LSA_UINT32  NRTTransCtrl;

            EDDI_ENTER_CRITICAL_S(); //ATTENTION: Register-Read-Modify-Write!

            NRTTransCtrl = IO_x32(NRT_TRANS_CTRL);

            //activate ARP-Filter in IRTE-register NRT-Transfer-Control
            EDDI_SetBitField32(&NRTTransCtrl, EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_ARP_Filter, 1);

            IO_x32(NRT_TRANS_CTRL) = NRTTransCtrl;

            EDDI_EXIT_CRITICAL_S();
        }
    }
    #endif

    pARPFilter->FilterCnt = FilterCount; //= active ARP-Filter
    return EDD_STS_OK;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTCheckARPFilter()                         */
/*                                                                         */
/* D e s c r i p t i o n: checks ARP-Filter in SW                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:    FilterIndex:                                           */
/*                  - EDDI_NRT_FILTER_INDEX_GARBAGE: frame is filtered     */
/*                  - EDDI_NRT_FILTER_INDEX_ARP:     frame is not filtered */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_DISABLE_ARP_FILTER)
#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_NRTCheckARPFilter( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         NRT_ARP_FILTER_PTR_TYPE  const  pARPFilter,
                                                         EDDI_ARP_FRAME_PTR_TYPE  const  pARPFrame )
{
    //Filtering ARP-Frames. Only ARP requests with a target IP present within the ARP filter table will be forwarded.
    //But gratuitous ARPs will never be filtered and always be forwarded.

    //check for a valid ARP-Request to apply the filter
    if (   (pARPFrame->Hln == EDDI_ARP_HLN)
        && (pARPFrame->Hrd == EDDI_HTONS(EDDI_ARP_HRD_ETHERNET))
        && (pARPFrame->Op  == EDDI_HTONS(EDDI_ARP_OP_REQUEST))
        && (pARPFrame->Pln == EDDI_ARP_PLN)
        && (pARPFrame->Pro == EDDI_HTONS(EDDI_ARP_PRO_IP)))
    {
        LSA_UINT32    Ctr;
        EDDI_IP_TYPE  ARP_Spa; //sender IP of ARP
        EDDI_IP_TYPE  ARP_Tpa; //target IP of ARP

        ARP_Spa.dw = 0; //to satisfy compiler!
        ARP_Tpa.dw = 0; //to satisfy compiler!

        //copy IP Addresses by byte-transfers
        for (Ctr = 0; Ctr < (LSA_UINT32)EDD_IP_ADDR_SIZE; Ctr++)
        {
            ARP_Spa.b.IP[Ctr] = pARPFrame->Spa[Ctr];
            ARP_Tpa.b.IP[Ctr] = pARPFrame->Tpa[Ctr];
        }

        //First we check for gratuitous ARPs. Gratuitous ARP requests
        //are ARP requests with identical TargetIP and SenderIP addresses
        //within the ARP request. These requests may be sent by the owner
        //of the IP address to force an update of the ARP cache within targets.

        if //sender IP different to target IP?
           #if (EDD_IP_ADDR_SIZE == 4)
           (ARP_Spa.dw != ARP_Tpa.dw)
           #else
           #error "EDDI: Check Implementation!"
           #endif
        {
            volatile  LSA_UINT32  ArpFilterRefCnt1;

            //consistency-check by RefCnt is necessary because this code is not completely protected by ENTER-macros!

            //RefCnt == even: ARP-Filter-Update currently not in progress!
            //RefCnt == odd:  ARP-Filter-Update currently in progress!

            ArpFilterRefCnt1 = pARPFilter->RefCnt;

            if //RefCnt1 odd (ARP-Filter-Update currently in progress)?
               (ArpFilterRefCnt1 & 1UL)
            {
                //ignore ARP-Filter => no filtering!
                EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTCheckARPFilter, ARP-Filter-Update currently in progress.");
            }
            else //ARP-Filter-Update currently not in progress
            {
                volatile  LSA_UINT32  ArpFilterRefCnt2;

                //check ARP-Filter-Entries
                {
                    LSA_UINT32  const  FilterCnt = pARPFilter->FilterCnt;

                    for (Ctr = 0; Ctr < FilterCnt; Ctr++)
                    {
                        EDDI_IP_TYPE  EDDI_LOCAL_MEM_ATTR  *  const  pARPFilterIPAddr = &pARPFilter->IPAddr[Ctr];

                        if //target IP Address found in ARP-Filter-Entry?
                           #if (EDD_IP_ADDR_SIZE == 4)
                           (ARP_Tpa.dw == pARPFilterIPAddr->dw)
                           #else
                           #error "EDDI: Check Implementation!"
                           #endif
                        {
                            //ARP frame is not filtered
                            pARPFilter->ARPPassCtr++;
                            //no filtering!
                            return EDDI_NRT_FILTER_INDEX_ARP;
                        }
                    }
                }

                //target IP Address not found in ARP-Filter-Entry:
                //  - ARP-Filter-consistency must still be checked!

                //check ARP-Filter-consistency
                ArpFilterRefCnt2 = pARPFilter->RefCnt;

                if //ARP-Filter inconsistent?
                   (ArpFilterRefCnt2 != ArpFilterRefCnt1)
                {
                    //ignore ARP-Filter => no filtering!
                    EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTCheckARPFilter, ARP-Filter inconsistent.");
                }
                else //ARP-Filter consistent
                {
                    pARPFilter->ARPDropCtr++;
                    //filtering!
                    return EDDI_NRT_FILTER_INDEX_GARBAGE;
                }
            }
        }
        else //gratuitous ARP. not filtered!
        {
            EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTCheckARPFilter, gratuitous ARP. not filtered.");
        }
    }
    else //no valid ARP-request. not filtered!
    {
        EDDI_NRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTCheckARPFilter, no valid ARP request. not filtered.");
    }

    //no filtering!
    LSA_UNUSED_ARG(pDDB);
    return EDDI_NRT_FILTER_INDEX_ARP;
}
#endif //EDDI_CFG_REV5 || EDDI_CFG_DEFRAG_ON
#endif //EDDI_CFG_DISABLE_ARP_FILTER
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_nrt_arp.c                                               */
/*****************************************************************************/

