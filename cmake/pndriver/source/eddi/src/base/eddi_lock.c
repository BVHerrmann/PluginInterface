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
/*  F i l e               &F: eddi_lock.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-Device and handle-management functions       */
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

#include "eddi_dev.h"
#include "eddi_lock.h"
#include "eddi_ext.h"

#define EDDI_MODULE_ID     M_ID_EDDI_LOCK
#define LTRC_ACT_MODUL_ID  15

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static void EDDI_LOCAL_FCT_ATTR  EDDI_EnterSNDA0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );
static void EDDI_LOCAL_FCT_ATTR  EDDI_EnterSNDB0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );

static void EDDI_LOCAL_FCT_ATTR  EDDI_EnterRCVA0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );
static void EDDI_LOCAL_FCT_ATTR  EDDI_EnterRCVB0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );

static void EDDI_LOCAL_FCT_ATTR  EDDI_ExitSNDA0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );
static void EDDI_LOCAL_FCT_ATTR  EDDI_ExitSNDB0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );

static void EDDI_LOCAL_FCT_ATTR  EDDI_ExitRCVA0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );
static void EDDI_LOCAL_FCT_ATTR  EDDI_ExitRCVB0( EDDI_NRT_CHX_SS_IF_TYPE * const  pIF );


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
LSA_VOID  EDDI_EnterIntern( LSA_VOID )
{
    if (!g_pEDDI_Info ->EDDI_Lock_Sema_Intern)
    {
        g_pEDDI_Info ->EDDI_Lock_Sema_Intern++;
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_EnterIntern, Semaphore!=0: 0x%X", g_pEDDI_Info ->EDDI_Lock_Sema_Intern);
        EDDI_Excp("EDDI_EnterIntern, !g_pEDDI_Info ->EDDI_Lock_Sema_Intern", EDDI_FATAL_ERR_EXCP, 0, 0);
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
LSA_VOID  EDDI_ExitIntern( LSA_VOID )
{
    g_pEDDI_Info ->EDDI_Lock_Sema_Intern--;

    if (g_pEDDI_Info ->EDDI_Lock_Sema_Intern == 0)
    {
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_ExitIntern, Semaphore==0: 0x%X", g_pEDDI_Info ->EDDI_Lock_Sema_Intern);
        EDDI_Excp("EDDI_ExitIntern, !g_pEDDI_Info ->EDDI_Lock_Sema_Intern", EDDI_FATAL_ERR_EXCP, 0, 0);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_LockClose( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_ENTER_REST_S();
    EDDI_ENTER_SYNC_S();
    EDDI_ENTER_COM_S();

    EDDI_EnterSNDA0(&pDDB->NRT.IF[EDDI_NRT_CHA_IF_0]);
    EDDI_EnterSNDB0(&pDDB->NRT.IF[EDDI_NRT_CHB_IF_0]);

    EDDI_EnterRCVA0(&pDDB->NRT.IF[EDDI_NRT_CHA_IF_0]);
    EDDI_EnterRCVB0(&pDDB->NRT.IF[EDDI_NRT_CHB_IF_0]);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_UnlockClose( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_ExitRCVB0(&pDDB->NRT.IF[EDDI_NRT_CHB_IF_0]);
    EDDI_ExitRCVA0(&pDDB->NRT.IF[EDDI_NRT_CHA_IF_0]);

    EDDI_ExitSNDB0(&pDDB->NRT.IF[EDDI_NRT_CHB_IF_0]);
    EDDI_ExitSNDA0(&pDDB->NRT.IF[EDDI_NRT_CHA_IF_0]);

    EDDI_EXIT_COM_S();
    EDDI_EXIT_SYNC_S();
    EDDI_EXIT_REST_S();
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_EnterSNDA0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    EDDI_ENTER_SND_CHA0();

    if (!pIF->Lock_Sema_SND)
    {
        pIF->Lock_Sema_SND++;
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_ENTER_SND_A0, Semaphore!=0: 0x%X", pIF->Lock_Sema_SND);
        EDDI_Excp("EDDI_ENTER_SND_A0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_SND);
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_EnterSNDB0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    EDDI_ENTER_SND_CHB0();

    if (!pIF->Lock_Sema_SND)
    {
        pIF->Lock_Sema_SND++;
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_ENTER_SND_B0, Semaphore!=0: 0x%X", pIF->Lock_Sema_SND);
        EDDI_Excp("EDDI_ENTER_SND_B0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_SND);
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_EnterRCVA0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    EDDI_ENTER_RCV_CHA0();

    if (!pIF->Lock_Sema_RCV)
    {
        pIF->Lock_Sema_RCV++;
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_ENTER_RCV_A0, Semaphore!=0: 0x%X", pIF->Lock_Sema_RCV);
        EDDI_Excp("EDDI_ENTER_RCV_A0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_RCV);
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_EnterRCVB0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    EDDI_ENTER_RCV_CHB0();

    if (!pIF->Lock_Sema_RCV)
    {
        pIF->Lock_Sema_RCV++;
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_ENTER_RCV_B0, Semaphore!=0: 0x%X", pIF->Lock_Sema_RCV);
        EDDI_Excp("EDDI_ENTER_RCV_B0", EDDI_FATAL_ERR_EXCP, pIF , pIF->Lock_Sema_RCV);
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_ExitSNDA0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    pIF->Lock_Sema_SND--;

    if (!pIF->Lock_Sema_SND)
    {
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_EXIT_SND_A0, Semaphore==0: 0x%X", pIF->Lock_Sema_SND);
        EDDI_Excp("EDDI_EXIT_SND_A0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_SND);
    }

    EDDI_EXIT_SND_CHA0();
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_ExitSNDB0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    pIF->Lock_Sema_SND--;

    if (!pIF->Lock_Sema_SND)
    {
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_EXIT_SND_B0, Semaphore==0: 0x%X", pIF->Lock_Sema_SND);
        EDDI_Excp("EDDI_EXIT_SND_B0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_SND);
    }

    EDDI_EXIT_SND_CHB0();
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_ExitRCVA0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    pIF->Lock_Sema_RCV--;

    if (!pIF->Lock_Sema_RCV)
    {
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_EXIT_RCV_A0, Semaphore==0: 0x%X", pIF->Lock_Sema_RCV);
        EDDI_Excp("EDDI_EXIT_RCV_A0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_RCV);
        return;
    }

    EDDI_EXIT_RCV_CHA0();
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
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_ExitRCVB0( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    pIF->Lock_Sema_RCV--;

    if (!pIF->Lock_Sema_RCV)
    {
    }
    else
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_EXIT_RCV_B0, Semaphore==0: 0x%X", pIF->Lock_Sema_RCV);
        EDDI_Excp("EDDI_EXIT_RCV_B0", EDDI_FATAL_ERR_EXCP, pIF, pIF->Lock_Sema_RCV);
        return;
    }

    EDDI_EXIT_RCV_CHB0();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IniNrtLockFct()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
void  EDDI_LOCAL_FCT_ATTR  EDDI_IniNrtLockFct( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF )
{
    switch (pIF->QueueIdx)
    {
        case EDDI_NRT_CHA_IF_0:
        {
            pIF->Tx.LockFct   = EDDI_EnterSNDA0;
            pIF->Tx.UnLockFct = EDDI_ExitSNDA0;

            pIF->Rx.LockFct   = EDDI_EnterRCVA0;
            pIF->Rx.UnLockFct = EDDI_ExitRCVA0;
            return;
        }

        case EDDI_NRT_CHB_IF_0:
        {
            pIF->Tx.LockFct   = EDDI_EnterSNDB0;
            pIF->Tx.UnLockFct = EDDI_ExitSNDB0;

            pIF->Rx.LockFct   = EDDI_EnterRCVB0;
            pIF->Rx.UnLockFct = EDDI_ExitRCVB0;
            return;
        }

        default:
        {
            EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_IniNrtLockFct, invalid QueueIdx 0x%X", pIF->QueueIdx);
            EDDI_Excp("EDDI_IniNrtLockFct, invalid pIF->QueueIdx:", EDDI_FATAL_ERR_EXCP, pIF->QueueIdx, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTLockIFRx()                               */
/*                                                                         */
/* D e s c r i p t i o n: attention: no lock is set when                   */
/*                                   returning EDDI_NULL_PTR!              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_LOCAL_HDB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTLockIFRx( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT                  Status;
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;
    EDDI_LOCAL_HDB_PTR_TYPE     pHDB;
    LSA_UINT32                  RefCnt;

    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);

    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return EDDI_NULL_PTR;
    }

    EDDI_PROGRAM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTLockIFRx->");

    RefCnt = pHDB->RefCnt;
    pIF    = pHDB->pIF;

    if (!(pHDB->UsedComp & EDDI_COMP_NRT))
    {
        //EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE); -> unsafe handle
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_CHANNEL_USE);
        EDDI_RQB_ERROR(pRQB);
        return EDDI_NULL_PTR;
    }

    pIF->Rx.LockFct(pIF);

    //now that LOCK is applied -> recheck handle -- CLOSE!!

    if (pHDB->RefCnt != RefCnt)
    {
        pIF->Rx.UnLockFct(pIF);
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return EDDI_NULL_PTR;
    }

    pRQB->internal_context   = pHDB; //store pHDB for NRT-Rcv
    pRQB->internal_context_1 = 0;

    //Clear Detail Error
    pHDB->pDDB->ErrDetail.bSet = 0;

    return pHDB;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_NRTLockIFTx()                               */
/*                                                                         */
/* D e s c r i p t i o n: attention: no lock is set when                   */
/*                                   returning EDDI_NULL_PTR!              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_LOCAL_HDB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTLockIFTx( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    LSA_RESULT                  Status;
    EDDI_NRT_CHX_SS_IF_TYPE  *  pIF;
    EDDI_LOCAL_HDB_PTR_TYPE     pHDB;
    LSA_UINT32                  RefCnt;
    
    Status = EDDI_HandleGetHDB(EDD_RQB_GET_HANDLE_LOWER(pRQB), &pHDB);
    
    if (Status != EDD_STS_OK)
    {
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return EDDI_NULL_PTR;
    }

    EDDI_PROGRAM_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_NRTLockIFTx->");

    RefCnt = pHDB->RefCnt;
    pIF    = pHDB->pIF;

    if (!(pHDB->UsedComp & EDDI_COMP_NRT))
    {
        //EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE);  -> unsafe handle
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_CHANNEL_USE);
        EDDI_RQB_ERROR(pRQB);
        return EDDI_NULL_PTR;
    }

    pIF->Tx.LockFct(pIF);

    //now that LOCK is applied -> recheck handle -- CLOSE!!

    if (pHDB->RefCnt != RefCnt)
    {
        pIF->Tx.UnLockFct(pIF);
        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_ERR_PARAM);
        EDDI_RQB_ERROR(pRQB);
        return EDDI_NULL_PTR;
    }

    pRQB->internal_context   = pHDB; //store pHDB for NRT-Snd
    pRQB->internal_context_1 = 0;

    //Clear Detail Error
    pHDB->pDDB->ErrDetail.bSet = 0;

    return pHDB;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_lock.c                                                  */
/*****************************************************************************/

