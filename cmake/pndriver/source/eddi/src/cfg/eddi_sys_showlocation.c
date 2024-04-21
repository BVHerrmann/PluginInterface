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
/*  F i l e               &F: eddi_sys_showlocation.c                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Example implementations of "Show Location"                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  25.08.08    AB    created                                                */
/*                                                                           */
/*****************************************************************************/ 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lsa_cfg.h"
#include "lsa_usr.h"
#include "lsa_sys.h"

#include "eddi_inc.h"
#include "eddi_irte.h"

#include "eddi_sys_reg_ertec.h"
#include "eddi_sys_ertec_hw.h"
#include "eddi_sys_showlocation.h"

#if defined (EDDI_CFG_ERTEC_200)

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  LED blink (show location) implementation for EB200 card (NEC PHY)         */
/*                                                                            */
/******************************************************************************/

/***************************************************************************
* F u n c t i o n:       EDDI_CpSetBitField()
*
* D e s c r i p t i o n: Converts value from host to little endian format
*                        (if necessary) and stores it into the specified
*                        bitfield of an little endian (!) 32Bit IRTE-
*                        Register or KRAM-Entry.
*
* A r g u m e n t s:     pReg:    Pointer to the 32Bit Register
*                                 (the referenced value is allways seen in
*                                 little endian format)
*                        BitHigh: Highest Bit of the specified bitfield
*                        BitLow:  Lowest  Bit of the specified bitfield
*                        value:   new value of the bitfield in host format.
*
* Return Value:          LSA_RESULT
*
***************************************************************************/
static  LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  EDDI_CpSetBitField( LSA_UINT32  *  const  pReg,
                                                               LSA_UINT32     const  BitHigh,
                                                               LSA_UINT32     const  BitLow,
                                                               LSA_UINT32     const  value )
{
    LSA_UINT32         BitMask;
    LSA_UINT32         stmp32;
    LSA_UINT32  const  range = BitHigh - BitLow;

    stmp32 = value << BitLow;
    EDDI_SWAP_VALUE32(stmp32);

    if (range == 0)
    {
        *pReg = (*pReg & ~EDDI_SINGLE_MASKS_32[BitHigh]) | (stmp32 & EDDI_SINGLE_MASKS_32[BitHigh]);
        return EDD_STS_OK;
    }
    else if (range <= EDDI_BIT_MAX)
    {
        BitMask = EDDI_MULTI_MASKS_32[range] << BitLow;
        EDDI_SWAP_VALUE32(BitMask);
        *pReg = (*pReg & ~(BitMask)) | (stmp32 & BitMask);
        return EDD_STS_OK;
    }
    else
    {
        return EDD_STS_ERR_PARAM;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetPortModeEB200( LSA_UINT32  const  HwPortIndex,
                                                      LSA_BOOL    const  bSwitchToStandardFunction )
{
    volatile ERTEC200_GPIO_T                  *  pERTEC200_GPIO = (volatile ERTEC200_GPIO_T *) cp_mem.pGPIO;
    volatile ERTEC200_AUX_GPIO_PORT_MODE_L_T  *  pGPIOP_PortModeL;
    volatile ERTEC200_AUX_GPIO_IOCTRL_T       *  pGPIOP_IOCTRL;
    ERTEC200_AUX_GPIO_IOCTRL_T                   SwapGPIOP_IOCTRL;
    ERTEC200_AUX_GPIO_PORT_MODE_L_T              SwapGPIOP_PortModeL;

    pGPIOP_PortModeL  = (ERTEC200_AUX_GPIO_PORT_MODE_L_T  *)&pERTEC200_GPIO->x000000c_GPIO_PORT_MODE_L;
    pGPIOP_IOCTRL     = (ERTEC200_AUX_GPIO_IOCTRL_T       *)&pERTEC200_GPIO->x0000000_GPIO_IOCTRL;

    SwapGPIOP_PortModeL.reg = pGPIOP_PortModeL->reg;
    SwapGPIOP_IOCTRL.reg    = pGPIOP_IOCTRL->reg;

    switch (HwPortIndex)
    {
        case 0:
        {   //Link LED
            EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI4, (bSwitchToStandardFunction) ? 0: 1);
            EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI4, 0); //adjust GPIO-OUT
            //Activity LED
            EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI6, (bSwitchToStandardFunction) ? 0: 3);
            EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI6, 0); //adjust GPIO-OUT
        }
        break;

        case 1:
        {   //Link LED
            EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI5, (bSwitchToStandardFunction) ? 0: 1);
            EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI5, 0); //adjust GPIO-OUT
            //Activity LED
            EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI7, (bSwitchToStandardFunction) ? 0: 3);
            EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI7, 0); //adjust GPIO-OUT
        }
        break;

        default:
        {
            // Error! Wrong HwPortIndex!
            return;
        }
    }

    pGPIOP_PortModeL->reg = SwapGPIOP_PortModeL.reg;
    pGPIOP_IOCTRL->reg    = SwapGPIOP_IOCTRL.reg;
}
/*---------------------- end [subroutine] ---------------------------------*/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SetLEDOnOffEB200( LSA_UINT32  const  HwPortIndex,
                                                      LSA_UINT16  const  LEDMode )
{
    volatile ERTEC200_GPIO_T          *  pERTEC200_GPIO = (volatile ERTEC200_GPIO_T *) cp_mem.pGPIO;
    volatile ERTEC200_AUX_GPIO_OUT_T  *  pGPIOP_Out;
    ERTEC200_AUX_GPIO_OUT_T              SwapGPIOP_Out;

    pGPIOP_Out = (ERTEC200_AUX_GPIO_OUT_T  *)&pERTEC200_GPIO->x0000004_GPIO_OUT;

    SwapGPIOP_Out.reg = pGPIOP_Out->reg;

    switch (HwPortIndex)
    {
        case 0:
        {
            EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC200_AUX_GPIO_OUT_BIT__GPI4, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1);//Link LED
            EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC200_AUX_GPIO_OUT_BIT__GPI6, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1);//Activity LED
        }
        break;

        case 1:
        {
            EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC200_AUX_GPIO_OUT_BIT__GPI5, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1);//Link LED
            EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC200_AUX_GPIO_OUT_BIT__GPI7, (LEDMode == EDDI_LED_MODE_ON) ? 0 : 1);//Activity LED
        }
        break;

        default:
        {
            // Error! Wrong HwPortIndex !
            return;
        }
    }

    pGPIOP_Out->reg = SwapGPIOP_Out.reg;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_ERTEC_200


/*****************************************************************************/
/*  end of file eddi_sys_showlocation.c                                      */
/*****************************************************************************/
