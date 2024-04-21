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
/*  F i l e               &F: eddi_isr_co.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-interrupthandler                             */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_EDDI_ISR_CO
#define LTRC_ACT_MODUL_ID  22

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"

//#include "eddi_profile.h"

#include "eddi_isr_nrt.h"
#include "eddi_isr_err.h"
#include "eddi_isr_rest.h"
#include "eddi_nrt_ts.h"
#include "eddi_nrt_rx.h"
#include "eddi_swi_ext.h"

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
#include "eddi_rto_udp.h"
#endif

static LSA_VOID  EDDI_LOCAL_FCT_ATTR    EDDI_IntDummy( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                       LSA_UINT32              const para_1 );

static LSA_VOID  EDDI_LOCAL_FCT_ATTR    EDDI_IntEmpty( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                       LSA_UINT32              const para_1 );


/***************************************************************************/
/* F u n c t i o n:       EDDI_IniIsrServiceTable()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IniIsrServiceTable( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                   Ctr;
    LSA_UINT32                   Anzahl1;
    volatile  LSA_UINT32  const  Anzahl2 = EDDI_MAX_ISR;
    EDDI_ISR_T            const  ISR_Table[] =
    {
        //IRTE-HW-interrupts IRT:
        { EDDI_IntDummy,                0 /*reserved*/                  },  /* Bit 0  */
        { EDDI_IntDummy,                EDDI_INT_StartOp                },  /* Bit 1  */
        { EDDI_IntDummy,                EDDI_INT_IRT_TransfEnd          },  /* Bit 2  */
        { EDDI_IntDummy,                EDDI_INT_DMAIn                  },  /* Bit 3  */
        { EDDI_IntDummy,                EDDI_INT_DMAOut                 },  /* Bit 4  */
        { EDDI_IntDummy,                EDDI_INT_InFault                },  /* Bit 5  */
        { EDDI_IntDummy,                EDDI_INT_OutFault               },  /* Bit 6  */
        { EDDI_IntDummy,                EDDI_INT1_rcv_IRT_SRT           },  /* Bit 7  */
        { EDDI_IntDummy,                EDDI_INT2_rcv_IRT_SRT           },  /* Bit 8  */
        { EDDI_IntDummy,                EDDI_INT1_snd_IRT_SRT           },  /* Bit 9  */
        { EDDI_IntDummy,                EDDI_INT2_snd_IRT_SRT           },  /* Bit 10 */
        { EDDI_IntDummy,                EDDI_INT_fatal_List_Err         },  /* Bit 11 */
        { EDDI_IntDummy,                EDDI_INT_AplClk                 },  /* Bit 12 */
        { EDDI_IntDummy,                EDDI_INT_Clkcy_Int              },  /* Bit 13 */
        { EDDI_IntDummy,                EDDI_INT_Clkcy                  },  /* Bit 14 */
        { EDDI_INTFifoDataLost,         0                               },  /* Bit 15 */
        { EDDI_INTTCWDone,              0                               },  /* Bit 16 */
        { EDDI_IntDummy,                EDDI_INT_Comp_1                 },  /* Bit 17 */
        { EDDI_IntDummy,                EDDI_INT_Comp_2                 },  /* Bit 18 */
        { EDDI_IntDummy,                EDDI_INT_Comp_3                 },  /* Bit 19 */
        { EDDI_IntDummy,                EDDI_INT_Comp_4                 },  /* Bit 20 */
        { EDDI_IntDummy,                EDDI_INT_Comp_5                 },  /* Bit 21 */
        { EDDI_INTACWListTerm,          0                               },  /* Bit 22 */
        { EDDI_IntDummy,                EDDI_INT_iSRT_DurationFault     },  /* Bit 23 */
        { EDDI_IntDummy,                EDDI_INT_Out_of_Sync_Rcv        },  /* Bit 24 */
        { EDDI_IntDummy,                EDDI_INT_Out_of_Sync_Snd        },  /* Bit 25 */
        { EDDI_IntDummy,                EDDI_INT_PS_Scoreboard_Skipped  },  /* Bit 26 */
        { EDDI_IntDummy,                EDDI_INT_PLL_ext_IN             },  /* Bit 27 */
        { EDDI_INTSRTCycleSkipped,      0                               },  /* Bit 28 */
        { EDDI_IntDummy,                EDDI_INT_ReadyForDMAIn          },  /* Bit 29 */
        { EDDI_IntDummy,                EDDI_INT_DMAOutDone             },  /* Bit 30 */
        { EDDI_INTPSScoreboardChanged,  0                               },  /* Bit 31 */
        
        //IRTE-HW-interrupts NRT:
        //Channel A0 always active!
        { EDDI_INTNRTTxDone,            EDDI_NRT_CHA_IF_0               },  /* Bit 0  */
        { EDDI_NRTRxDoneInt,            EDDI_NRT_CHA_IF_0               },  /* Bit 1  */
        //Channel A1 not supported!
        { EDDI_IntDummy,                EDDI_INT_Snd_CHA1               },  /* Bit 2  */
        { EDDI_IntDummy,                EDDI_INT_Rcv_CHA1               },  /* Bit 3  */
        //Channel B0 always active!
        { EDDI_INTNRTTxDone,            EDDI_NRT_CHB_IF_0               },  /* Bit 4  */
        { EDDI_NRTRxDoneInt,            EDDI_NRT_CHB_IF_0               },  /* Bit 5  */
        //Channel B1 not supported!
        { EDDI_IntDummy,                EDDI_INT_Snd_CHB1               },  /* Bit 6  */
        { EDDI_IntDummy,                EDDI_INT_Rcv_CHB1               },  /* Bit 7  */
        { EDDI_IntDummy,                EDDI_INT_Fatal_NRTList_Err      },  /* Bit 8  */
        { EDDI_SWILinkInterrupt,        0                               },  /* Bit 9  */
        { EDDI_IntDummy,                EDDI_INT_Trigger_Unit           },  /* Bit 10 */
        { EDDI_IntDummy,                EDDI_INT_ClockCnt_Wrap          },  /* Bit 11 */
        { EDDI_IntDummy,                EDDI_INT_HP                     },  /* Bit 12 */
        { EDDI_IntDummy,                EDDI_INT_SP                     },  /* Bit 13 */
        { EDDI_IntDummy,                EDDI_INT_Host_Access_Err        },  /* Bit 14 */
        { EDDI_NRTTimeStampInt,         EDDI_TS_CALLER_TS_INT           },  /* Bit 15 */
        { EDDI_INTInstructionDone,      0                               },  /* Bit 16 */
        { EDDI_INTOverflowHOL,          0                               },  /* Bit 17 */
        { EDDI_IntDummy,                EDDI_INT_Underflow_NRT          },  /* Bit 18 */
        { EDDI_IntDummy,                EDDI_INT_SRT_FCW_empty          },  /* Bit 19 */
        { EDDI_IntDummy,                EDDI_INT_NRT_FCW_empty          },  /* Bit 20 */
        { EDDI_IntDummy,                EDDI_INT_NRT_DB_empty           },  /* Bit 21 */
        { EDDI_IntDummy,                EDDI_INT_no_Table_Entry         },  /* Bit 22 */
        { EDDI_IntDummy,                EDDI_INT_a                      },  /* Bit 23 */
        { EDDI_IntDummy,                EDDI_INT_b                      },  /* Bit 24 */
        { EDDI_IntDummy,                EDDI_INT_c                      },  /* Bit 25 */
        { EDDI_IntDummy,                EDDI_INT_d                      },  /* Bit 26 */
        { EDDI_IntDummy,                EDDI_INT_e                      },  /* Bit 27 */
        { EDDI_IntDummy,                EDDI_INT_f                      },  /* Bit 28 */
        { EDDI_IntDummy,                EDDI_INT_g                      },  /* Bit 29 */
        { EDDI_IntDummy,                EDDI_INT_h                      },  /* Bit 30 */
        { EDDI_IntDummy,                EDDI_INT_i                      },  /* Bit 31 */

        //EDDI-SW-interrupts:
        { EDDI_NewCycleReduced,         0                               },  //EDDI_SW_INT_NewCycleReduced
        { EDDI_NRTReloadAllSendLists,   0                               },  //EDDI_SW_INT_Reload_NRTSendLists
        { EDDI_IntDummy,                0x42UL /*reserved*/             },
        { EDDI_IntDummy,                0x43UL /*reserved*/             },

        #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
        { EDDI_RtoSnd,                  0                               }   //EDDI_SW_INT_RToverUDP
        #else
        { EDDI_IntEmpty,                0                               }
        #endif
    };

    Anzahl1 = sizeof(ISR_Table) / sizeof(EDDI_ISR_T);
    if (Anzahl1 != Anzahl2)
    {
        EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_IniIsrServiceTable, invalid length of ISR-Table, Anzahl1:0x%X, Anzahl2:0x%X", 
            Anzahl1, Anzahl2);
        EDDI_Excp("EDDI_IniIsrServiceTable, invalid length of ISR-Table", EDDI_FATAL_ERR_EXCP, Anzahl1, Anzahl2);
        return;
    }
    for (Ctr = 0; Ctr < Anzahl1; Ctr++)
    {
        pDDB->ISR_Table[Ctr] = ISR_Table[Ctr];
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IntDummy( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  para_1 )
{
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_IntDummy->");

    EDDI_Excp("EDDI_IntDummy, this interrupt must not occur here, pDDB: EDDIIntSource:", EDDI_FATAL_ERR_ISR, pDDB, para_1);

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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IntEmpty( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  para_1 )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(para_1); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_isr_co.c                                                */
/*****************************************************************************/
