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
/*  F i l e               &F: eddi_bsp_sys_int.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/ 

#include "eddi_inc.h"

#include "eddi_bsp_edd_ini.h"

#include "eddi_bsp_sys_int.h"

#define BSP_EOI_VALUE       0xF

static LSA_UINT32           New_Cycle_Reduction_Ctr;
static LSA_UINT32           New_Cycle_Reduction_Factor;

static LSA_UINT32   GD_mask_nrt;
static LSA_UINT32   GD_mask_irt;


/***************************************************************************/
/* F u n c t i o n:       EDDI_BSPSysIntEnable()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_BSPSysIntEnable( LSA_VOID )
{
    //At this time the system has to know the "real" ISR (e.g. EDDI_BSPISR)
    return LSA_TRUE;
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
LSA_VOID  EDDI_BSPSetSystemInterruptServiceFunction( EDDI_BSPISR_FCT  const  bsp_isr )
{
    if (bsp_isr)
    {
    }

    //register ISR to the system!
}
/*---------------------- end [subroutine] ---------------------------------*/


#define SHEDULE_DPC(irte, NRT_req, IRT_REQ, dpc)

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
LSA_VOID  EDDI_BSPISR( volatile  IRTE_SW_MAKRO_T  *  const  irte )
{
    //System Interrupt Context!

    //call SII-ISR
	eddi_interrupt(g_SyshDDB);

    #if !defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
    //EOI
    eddi_interrupt_set_eoi(g_SyshDDB, (LSA_UINT32)BSP_EOI_VALUE);
    #endif

    LSA_UNUSED_ARG(irte); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/


/****************************************************************************/
/*  end of file eddi_bsp_sys_int.c                                          */
/****************************************************************************/

