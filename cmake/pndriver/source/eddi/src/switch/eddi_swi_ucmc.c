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
/*  F i l e               &F: eddi_swi_ucmc.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Ser10 File Data Base for EDDI.                   */
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
#include "eddi_swi_ucmc.h"

#define EDDI_MODULE_ID     M_ID_SWI_UCMC
#define LTRC_ACT_MODUL_ID  317

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define FDB_TABLE_LENGTH_BITS 12  // Bit size of FDB_Table_Length Register 


/*=============================================================================
* function name: EDDI_SwiUcSetFDBPrio()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*               
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcSetFDBPrio( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB, 
                                                       LSA_UINT16                const  RQBPrio,
                                                       LSA_UINT8                     *  pFDBPrio )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcSetFDBPrio->RQBPrio:0x%X *pFDBPrio:0x%X", RQBPrio, *pFDBPrio);

    *pFDBPrio = 0;

    switch (RQBPrio)
    {
        case EDDI_SWI_FDB_PRIO_3:
            *pFDBPrio = SER_FDB_PRIO3;
            break;
        case EDDI_SWI_FDB_PRIO_ORG:
            *pFDBPrio = SER_FDB_ORG_PRIO;
            break;
        case EDDI_SWI_FDB_PRIO_FLUSS:
            *pFDBPrio = SER_FDB_FLUSS_PRIO;
            break;
        case EDDI_SWI_FDB_PRIO_DEFAULT:
            *pFDBPrio = SER_FDB_DEFAULT_PRIO;
            break;
        default:
            return EDD_STS_ERR_PARAM;
    }

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcSetFDBPrio<-RQBPrio:0x%X *pFDBPrio:0x%X", RQBPrio, *pFDBPrio);

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcGetFDBPrio()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBPrio( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                     LSA_UINT8                const  FDBPrio,
                                                     LSA_UINT16                   *  pRQBPrio )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcGetFDBPrio->FDBPrio:0x%X *pRQBPrio:0x%X", FDBPrio, *pRQBPrio);

    switch (FDBPrio)
    {
        case SER_FDB_PRIO3:
            *pRQBPrio  = EDDI_SWI_FDB_PRIO_3;
            break;
        case SER_FDB_ORG_PRIO:
            *pRQBPrio  = EDDI_SWI_FDB_PRIO_ORG;
            break;
        case SER_FDB_FLUSS_PRIO:
            *pRQBPrio  = EDDI_SWI_FDB_PRIO_FLUSS;
            break;
        case SER_FDB_DEFAULT_PRIO:
            *pRQBPrio  = EDDI_SWI_FDB_PRIO_DEFAULT;
            break;
        default:
            EDDI_Excp("EDDI_SwiUcGetFDBPrio, FDBPrio", EDDI_FATAL_ERR_EXCP, FDBPrio, 0);
            break;
    }

    LSA_UNUSED_ARG(pDDB);
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcGetFDBPrio<-FDBPrio:0x%X *pRQBPrio:0x%X", FDBPrio, *pRQBPrio);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcGetFDBAdress()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBAdress( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pFDBEntry,
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE         pRQBFDBAdress )
{
    LSA_INT32  Index;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcGetFDBAdress->");

    for (Index = 0; Index < EDD_MAC_ADDR_SIZE; Index++)
    {
        pRQBFDBAdress->MacAdr[Index] = pFDBEntry->MacAdr[Index];
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: SERCalcAdr()
*
* function:  calculate index
*
* parameters:  macAdr = MAC-Address of FDB
*     mask   = feedback mask reg (CTRL-Reg)
*     range  = fdb range
*
* return value:
* comment:      This routine replaces the lfsr (line feedback shift register)
*               which is not supported by the ser10 hardware.
*               Any access like read or write to the FDB has to done to FDB
*               by the call of the SERCalcAdr instead of the hardware lfsr.
*               Bit 11 is feedbacked to an XOR connection mask with CTRL-Reg.
*
*                       +---+---+---+---+---+---+---+---+---+---+---+---+
*             CTRL-Reg  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| 11|   -> Maske 4a4
*                       +---+---+---+---+---+---+---+---+---+---+---+---+
*                        |   |                                         |
*                        |   +-----------+                             |
*                        |               |                             |
*  +-------------------*---------------*-----------------------------* |
*  |                   | |             | |                           | |
*  |                  +---+           +---+                         +---+
*  |                  | & |           | & |                         | & |
*  |                  +---+           +---+                         +---+
*  |                    |               |                             |
*  |                    |               |                             |
*  |          +---+   +---+   +---+   +---+   +---+                 +---+   +---+
*  |   LFSR   |in |-->|XOR|-->| 0 |-->|XOR|-->| 1 |....          -->|XOR|-->| 11|--+
*  |          +---+   +---+   +---+   +---+   +---+                 +---+   +---+  |
*  |                                                                               |
*  +-------------------------------------------------------------------------------+
*
*
*==========================================================================*/
LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_SwiUcCalcAdr( const EDD_MAC_ADR_TYPE EDDI_LOCAL_MEM_ATTR * const  pMAC,
                                                        LSA_UINT32                             const  Mask,
                                                        LSA_UINT32                             const  TableLength )                                                                                             
{
    LSA_UINT32  Lfsr;
    LSA_UINT32  MacAdrByteN;
    LSA_UINT32  BitN;
    LSA_UINT32  Index;
    LSA_UINT8   Value;

    Lfsr = (LSA_UINT32) - 1L;

    for (MacAdrByteN = 0; MacAdrByteN < EDD_MAC_ADDR_SIZE; MacAdrByteN++)
    {
        Value = pMAC->MacAdr[MacAdrByteN];

        for (BitN = 0; BitN < 8; BitN++)
        {
            if (Value & 1)
            {
                Lfsr |= 1;
            }
            else
            {
                Lfsr &= ~1;
            }

            // use 12 not 11 because we shift mac addr bit in possion 0
            if ((Lfsr >> FDB_TABLE_LENGTH_BITS) & 1)
            {
                Lfsr = Lfsr ^ Mask;
            }
            else
            {
                Lfsr = 0 ^ Lfsr;
            }
            // make a shift
            Lfsr <<= 1;
            // next bit from mac addr
            Value >>= 1;
        }
    }
    // undo last shift operation
    Lfsr >>= 1;

    // max index limited by fdb length
    Index = Lfsr & TableLength;

    return Index;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_swi_ucmc.c                                              */
/*****************************************************************************/
