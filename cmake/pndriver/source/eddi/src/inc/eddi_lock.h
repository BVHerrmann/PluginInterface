#ifndef EDDI_LOCK_H             //reinclude-protection
#define EDDI_LOCK_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_lock.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines, internal constants, types, data, macros and prototyping for     */
/*  EDDI.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  23.06.08    JS    added ENTER_CRITICAL and EXIT_CRITICAL                 */
/*                                                                           */
/*****************************************************************************/

#define EDDI_ENTER_COM_S()                                  \
    EDDI_ENTER_COM();                                       \
    if (!g_pEDDI_Info ->EDDI_Lock_Sema_COM)                    \
    {                                                       \
        g_pEDDI_Info ->EDDI_Lock_Sema_COM++;                   \
    }                                                       \
    else                                                    \
    {                                                       \
        /* no trace entries in macros possible (tracescanner) */\
        EDDI_Excp("EDDI_ENTER_COM -> ", EDDI_FATAL_ERR_EXCP, g_pEDDI_Info ->EDDI_Lock_Sema_COM, 0); \
    }

#define EDDI_EXIT_COM_S()                                   \
    g_pEDDI_Info ->EDDI_Lock_Sema_COM--;                       \
    if (g_pEDDI_Info ->EDDI_Lock_Sema_COM == 0)                \
    {                                                       \
    }                                                       \
    else                                                    \
    {                                                       \
        /* no trace entries in macros possible (tracescanner) */\
        EDDI_Excp("EDDI_EXIT_COM -> ", EDDI_FATAL_ERR_EXCP, g_pEDDI_Info ->EDDI_Lock_Sema_COM, 0); \
    }                                                       \
    EDDI_EXIT_COM()

#define EDDI_ENTER_SYNC_S()                     \
    EDDI_ENTER_SYNC();                          \
    if (!g_pEDDI_Info ->EDDI_Lock_Sema_SYNC)       \
    {                                           \
        g_pEDDI_Info ->EDDI_Lock_Sema_SYNC++;      \
    }                                           \
    else                                        \
    {                                           \
        /* no trace entries in macros possible (tracescanner) */\
        EDDI_Excp("EDDI_ENTER_SYNC -> !g_pEDDI_Info ->EDDI_Lock_Sema_SYNC", EDDI_FATAL_ERR_EXCP, 0, g_pEDDI_Info ->EDDI_Lock_Sema_SYNC); \
    }

#define EDDI_EXIT_SYNC_S()                      \
    g_pEDDI_Info ->EDDI_Lock_Sema_SYNC--;          \
    if (g_pEDDI_Info ->EDDI_Lock_Sema_SYNC == 0)   \
    {                                           \
    }                                           \
    else                                        \
    {                                           \
        /* no trace entries in macros possible (tracescanner) */\
        EDDI_Excp("EDDI_EXIT_SYNC -> !g_pEDDI_Info ->EDDI_Lock_Sema_SYNC", EDDI_FATAL_ERR_EXCP, g_pEDDI_Info ->EDDI_Lock_Sema_SYNC, 0); \
    }                                           \
    EDDI_EXIT_SYNC()


#define EDDI_ENTER_SYNC_S_SPECIAL()                    \
    if (g_pEDDI_Info ->EDDI_Lock_Sema_SYNC == 0)          \
    {                                                  \
        EDDI_ENTER_SYNC();                             \
        if (!g_pEDDI_Info ->EDDI_Lock_Sema_SYNC_Special)  \
        {                                              \
            g_pEDDI_Info ->EDDI_Lock_Sema_SYNC_Special++; \
        }                                              \
        else                                           \
        {                                              \
        /* no trace entries in macros possible (tracescanner) */\
            EDDI_Excp("EDDI_ENTER_SYNC -> !g_pEDDI_Info ->EDDI_Lock_Sema_SYNC_Special", EDDI_FATAL_ERR_EXCP, 0, g_pEDDI_Info ->EDDI_Lock_Sema_SYNC_Special); \
        }                                              \
    }

#define EDDI_EXIT_SYNC_S_SPECIAL()                     \
    if (g_pEDDI_Info ->EDDI_Lock_Sema_SYNC_Special)       \
    {                                                  \
        g_pEDDI_Info ->EDDI_Lock_Sema_SYNC_Special--;     \
        EDDI_EXIT_SYNC();                              \
    }                                                                                                


#define EDDI_ENTER_REST_S()                     \
    EDDI_ENTER_REST();                          \
    if (!g_pEDDI_Info ->EDDI_Lock_Sema_REST)       \
    {                                           \
        g_pEDDI_Info ->EDDI_Lock_Sema_REST++;      \
    }                                           \
    else                                        \
    {                                           \
        /* no trace entries in macros possible (tracescanner) */\
        EDDI_Excp("EDDI_ENTER_REST -> !g_pEDDI_Info ->EDDI_Lock_Sema_REST", EDDI_FATAL_ERR_EXCP, 0, g_pEDDI_Info ->EDDI_Lock_Sema_REST); \
    }

#define EDDI_EXIT_REST_S()                      \
    g_pEDDI_Info ->EDDI_Lock_Sema_REST--;          \
    if (g_pEDDI_Info ->EDDI_Lock_Sema_REST == 0)   \
    {                                           \
    }                                           \
    else                                        \
    {                                           \
        /* no trace entries in macros possible (tracescanner) */\
        EDDI_Excp("EDDI_EXIT_REST -> !g_pEDDI_Info ->EDDI_Lock_Sema_REST", EDDI_FATAL_ERR_EXCP, g_pEDDI_Info ->EDDI_Lock_Sema_REST, 0); \
    }                                           \
    EDDI_EXIT_REST()

#define EDDI_ENTER_CRITICAL_S() EDDI_ENTER_CRITICAL()
#define EDDI_EXIT_CRITICAL_S()  EDDI_EXIT_CRITICAL()

LSA_VOID  EDDI_EnterIntern( LSA_VOID );
LSA_VOID  EDDI_ExitIntern( LSA_VOID );

void  EDDI_LOCAL_FCT_ATTR  EDDI_IniNrtLockFct( EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF );

EDDI_LOCAL_HDB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTLockIFRx( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );
EDDI_LOCAL_HDB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_NRTLockIFTx( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_LockClose( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_UnlockClose( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_LOCK_H


/*****************************************************************************/
/*  end of file eddi_lock.h                                                  */
/*****************************************************************************/
