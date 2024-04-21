#ifndef EDDI_SER_TYP_R5_H       //reinclude-protection
#define EDDI_SER_TYP_R5_H

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
/*  F i l e               &F: eddi_ser_typ_r5.h                         :F&  */
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

#define SER_BITFIELD_16   LSA_UINT16

#define EDDI_SER10_TIMER_SCORB_BIT__WatchdogTimer                EDDI_BIT_MASK_PARA(12, 0)
#define EDDI_SER10_TIMER_SCORB_BIT__ProdDataValid                EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER10_TIMER_SCORB_BIT__DataStatusChanged            EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER10_TIMER_SCORB_BIT__ProdSwitchedOn               EDDI_BIT_MASK_PARA(15,15)
#define EDDI_SER10_TIMER_SCORB_BIT__ClearMask                    EDDI_BIT_MASK_PARA(16,13)

#include "pnio_pck2_on.h"
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_TIMER_SCORB_TYPE
{
    LSA_UINT16                  Reg;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_TIMER_SCORB_TYPE EDDI_SER10_TIMER_SCORB_TYPE;

#include "pnio_pck_off.h"

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_TYP_R5_H


/*****************************************************************************/
/*  end of file eddi_ser_typ_r5.h                                            */
/*****************************************************************************/
