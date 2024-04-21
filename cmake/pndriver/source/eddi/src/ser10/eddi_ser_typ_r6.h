#ifndef EDDI_SER_TYP_R6_H       //reinclude-protection
#define EDDI_SER_TYP_R6_H

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
/*  F i l e               &F: eddi_ser_typ_r6.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                           Switch-ASIC-Structs                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*====================== EDDI_SER_SCORE_BOARD_TYPES =========================*/
/*===========================================================================*/
#define EDDI_SER10_TIMER_SCORB_BIT__WatchdogTimer                EDDI_BIT_MASK_PARA(12, 0)
#define EDDI_SER10_TIMER_SCORB_BIT__ProvDataValid                EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER10_TIMER_SCORB_BIT__DataStatusChanged            EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER10_TIMER_SCORB_BIT__ProvSwitchedOn               EDDI_BIT_MASK_PARA(15,15)
#define EDDI_SER10_TIMER_SCORB_BIT__DataHoldTimer                EDDI_BIT_MASK_PARA(28,16)
#define EDDI_SER10_TIMER_SCORB_BIT__Reserved                     EDDI_BIT_MASK_PARA(31,29)
#define EDDI_SER10_TIMER_SCORB_BIT__ClearMask                    EDDI_BIT_MASK_PARA(31,13)

#include "pnio_pck4_on.h"
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_TIMER_SCORB_TYPE
{
    LSA_UINT32                  Reg[1];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_TIMER_SCORB_TYPE EDDI_SER10_TIMER_SCORB_TYPE;

#include "pnio_pck_off.h"

/*===========================================================================*/
/*====================== EDDI_SER_MC_TYPE ===================================*/
/*===========================================================================*/
#define EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr               EDDI_BIT_MASK_PARA( 8, 0)
#define EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__Res                      EDDI_BIT_MASK_PARA(11, 9)
#define EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index            EDDI_BIT_MASK_PARA(15,12)

#include "pnio_pck2_on.h"
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE
{
    LSA_UINT16                         Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_MC_TABLE_HEADER_ENTRY_PTR_TYPE;
#include "pnio_pck_off.h"

#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DestPort            EDDI_BIT_MASK_PARA( 5, 0)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Reserved2           EDDI_BIT_MASK_PARA( 7, 6)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__S                   EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__FU                  EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Reserved1           EDDI_BIT_MASK_PARA(11,10)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DCP                 EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio                EDDI_BIT_MASK_PARA(14,13)
#define EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__PauseFilter         EDDI_BIT_MASK_PARA(15,15)

#define EDDI_SER_MC_TABLE_MAC_ENTRY_SIZE  8  //64Bit

#include "pnio_pck4_on.h"
PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE
{
    EDDI_SER10_UINT64_TYPE            Value;
    LSA_UINT8                         Byte[EDDI_SER_MC_TABLE_MAC_ENTRY_SIZE];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE   EDDI_LOWER_MEM_ATTR * EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE;
#include "pnio_pck_off.h"

#include "pnio_pck2_on.h"
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_MC_TABLE_DESC_ENTRY_TYPE
{
    LSA_UINT16                           SubTable_Offset;       // Offset to SubTable, related to Base_Adress
    LSA_UINT16                           SubTable_LFSR;         //
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pSubTable_Base_Adress; //Ptr to start of SubTable

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_MC_TABLE_DESC_ENTRY_TYPE EDDI_SER_MC_TABLE_DESC_ENTRY_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_MC_TABLE_DESC_ENTRY_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_MC_TABLE_DESC_ENTRY_PTR_TYPE;
#include "pnio_pck_off.h"

/*====================================*/
/*====== EDDI_SER_DRIFT_COR_TYPE =====*/
/*====================================*/
#define EDDI_SER_DRIFT_COR_BIT__CntCycl                 EDDI_BIT_MASK_PARA( 30, 0)
#define EDDI_SER_DRIFT_COR_BIT__Inkrement               EDDI_BIT_MASK_PARA( 31, 31)

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_TYP_R6_H


/*****************************************************************************/
/*  end of file eddi_ser_typ_r6.h                                            */
/*****************************************************************************/
