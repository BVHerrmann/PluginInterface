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
/*  F i l e               &F: eddi_sys_ertec_gpio.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* examples for ERTEC-specific functions.                                    */
/* - GPIO                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  04.04.08    UL    created                                                */
/*                                                                           */
/*****************************************************************************/ 

#include "lsa_cfg.h"
#include "lsa_usr.h"
#include "lsa_sys.h"

#include "edd_cfg.h"
#include "eddi_cfg.h"

#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_ERTEC_200)

#include "edd_usr.h"
#include "eddi_usr.h"
#include "eddi_sys.h"
#include "eddi_sys_reg_ertec.h"
#include "eddi_sys_ertec_hw.h"

#include "eddi_irte.h"

#ifdef __cplusplus //If C++ - compiler: Use C linkage
extern "C"
{
#endif

//#define SYS_ERTEC_USE_NO_GLOBAL_VARS

/*=============================================================================*/
/*        Global Variables                                                     */
/*=============================================================================*/

cp_mem_t    cp_mem;
LSA_UINT32  p_ertec_IRTE;
LSA_UINT32  p_ertec_GPIO;

static LSA_UINT32 current_scf = 32;


/*=============================================================================*/
/*        System Resources                                                     */
/*=============================================================================*/


/***************************************************************************
* F u n c t i o n:       EDDI_SysGetErtecBasePtrs()
*
* D e s c r i p t i o n: reads the saved base-ptrs
*/
static  LSA_BOOL  EDDI_SysGetErtecBasePtrs( LSA_UINT32     pIRTE, 
                                            LSA_UINT32  *  ppGPIO )
{
    if (cp_mem.pIRTE == pIRTE)
    {
        *ppGPIO = cp_mem.pGPIO;

        return LSA_TRUE;
    }

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:       EDDI_SysSetErtecBasePtr()
*
* D e s c r i p t i o n: saves the base-ptrs for later use
*/
LSA_VOID  EDDI_SysSetErtecBasePtr( LSA_UINT32  pSWIReg, 
                                   LSA_BOOL    isERTEC200, 
                                   LSA_UINT32  pGPIO )
{
    cp_mem.pIRTE      = pSWIReg;
    cp_mem.isERTEC200 = isERTEC200;
    cp_mem.pGPIO      = pGPIO;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:       EDDI_SysSetSendClockChange()
*
* D e s c r i p t i o n: Set the Current CycleBaseFactor
*/
LSA_VOID  EDDI_SysSetSendClockChange( LSA_UINT32  const  scf )
{
    current_scf = scf;
}
/*---------------------- end [subroutine] ---------------------------------*/

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

/*=============================================================================*/
/*        GPIOs                                                                */
/*=============================================================================*/

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
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CpErtecSetGPIO2530Out( LSA_UINT16                  const  PDControlPllGPIOOutPut,
                                                                     EDD_UPPER_MEM_U32_PTR_TYPE         pPortModeH,
                                                                     EDD_UPPER_MEM_U32_PTR_TYPE         pIOCTRL,
                                                                     LSA_UINT32                  const  Value )
{
    LSA_RESULT  retval = EDD_STS_OK;

    switch (PDControlPllGPIOOutPut)
    {
        case 25:
        {
            /* GPIO 25, XTGEN_OUT1  */
            EDDI_CpSetBitField(pPortModeH, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI9, ALTERNATE__FUNCTION_1);
            /* GPIO 25, output  */
            EDDI_CpSetBitField(pIOCTRL,    ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI25, Value);
        }
        break;
        case 26:
        {
            /* GPIO 26, XTGEN_OUT2  */
            EDDI_CpSetBitField(pPortModeH, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI10, ALTERNATE__FUNCTION_1);
            /* GPIO 26, output  */
            EDDI_CpSetBitField(pIOCTRL, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI26, Value);
        }
        break;
        case 27:
        {
            /* GPIO 27, XTGEN_OUT3  */
            EDDI_CpSetBitField(pPortModeH, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI11, ALTERNATE__FUNCTION_1);
            /* GPIO 27, output  */
            EDDI_CpSetBitField(pIOCTRL, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI27, Value);
        }
        break;
        case 28:
        {
            /* GPIO 28, XTGEN_OUT4  */
            EDDI_CpSetBitField(pPortModeH, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI12, ALTERNATE__FUNCTION_1);
            /* GPIO 28, output  */
            EDDI_CpSetBitField(pIOCTRL, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI28, Value);
        }
        break;
        case 29:
        {
            /* GPIO 29, XTGEN_OUT5  */
            EDDI_CpSetBitField(pPortModeH, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI13, ALTERNATE__FUNCTION_1);
            /* GPIO 29, output  */
            EDDI_CpSetBitField(pIOCTRL, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI29, Value);
        }
        break;
        case 30:
        {
            /* GPIO 30, XTGEN_OUT6  */
            EDDI_CpSetBitField(pPortModeH, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI14, ALTERNATE__FUNCTION_1);
            /* GPIO 30, output  */
            EDDI_CpSetBitField(pIOCTRL, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI30, Value);
        }
        break;
        default:
        {
            retval = EDD_STS_ERR_PARAM;
        }
    }

    return retval;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_CpErtecSetPllPort()
 *
 * function:       initializes a ERTEC-GPIO for PLL-Out/In
 *
 *
 * parameters:
 *  - pIRTE         = Ptr to the IRTE-Reg-Base
 *  - location      = location (which ASIC)
 *  - pllmode       = EDDI_PRM_DEF_PLL_MODE_OFF
 *                    EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT
 *                    EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN
 *  - gpio          = SOC2: OUT: 46...53
 *                          IN:  55
 *                    SOC1: OUT: 152..159 / 168..175
 *                          IN:  95 / 127 / 178
 *                    ERTEC200/400: OUT: 25...30
 *                                   IN: 24
 *
 * return value:   LSA_RESULT (EDD_STS_OK or EDD_STS_ERR_PARAM)
 *===========================================================================*/
/* !!!example for implementation only !!!*/
/* Attention: needs global variables p_ertec_IRTE
                                     p_ertec_GPIO !!! */
LSA_RESULT  EDDI_CpErtecSetPllPort( EDDI_SYS_HANDLE  hSysDev,
                                    LSA_UINT32       pIRTE,
                                    LSA_UINT32       location,
                                    LSA_UINT16       pllmode )
{
    LSA_UINT32                              pGPIO; //Baseptr to the ERTEC-GPIOs
    LSA_RESULT                              retval      = EDD_STS_OK;
    LSA_UINT16                       const  gpio_out_nr = (LSA_UINT16)((EDDI_LOC_ERTEC200 == location)?SYS_ERTEC_ERTEC200_PLLGPIO_OUT:SYS_ERTEC_ERTEC400_PLLGPIO_OUT);
    LSA_BOOL                         const  rs485_used  = (LSA_BOOL)((EDDI_LOC_ERTEC200 == location)?SYS_ERTEC_ERTEC200_RS485_USED:SYS_ERTEC_ERTEC200_RS485_USED);

    ERTEC400_AUX_GPIO_IOCTRL_T              SwapGPIOP_IOCTRL;
    ERTEC400_AUX_GPIO_PORT_MODE_H_T         SwapGPIOP_PortModeH;
    ERTEC400_AUX_GPIO_OUT_T                 SwapGPIOP_Out;

    LSA_UNUSED_ARG(hSysDev); //satisfy lint!

    #if defined (SYS_ERTEC_USE_NO_GLOBAL_VARS)
    //Get the baseptrs that "fit" to the IRTE-ptr
    if (!EDDI_SysGetErtecBasePtrs(pIRTE, &pGPIO))
    {
        return EDD_STS_ERR_PARAM;
    }
    #else
    //Check the baseptrs for the call
    if (( pIRTE        != p_ertec_IRTE) || //not matches
        ( p_ertec_GPIO == 0))
    {
        return EDD_STS_ERR_PARAM;
    }
    pGPIO = p_ertec_GPIO;
    #endif

    SwapGPIOP_IOCTRL.reg    = *((LSA_UINT32 *)(void *)(pGPIO + EDDI_GPIO_IOCTRL));
    SwapGPIOP_Out.reg       = *((LSA_UINT32 *)(void *)(pGPIO + EDDI_GPIO_OUT));
    SwapGPIOP_PortModeH.reg = *((LSA_UINT32 *)(void *)(pGPIO + EDDI_GPIO_PORT_MODE_H));

    if ((gpio_out_nr < 25) || (gpio_out_nr > 30))
    {
        //illegal gpio!
        return EDD_STS_ERR_PARAM;
    }

    switch (pllmode)
    {
        case //switch output off
             EDDI_PRM_DEF_PLL_MODE_OFF:
            {
                if (   (EDDI_LOC_ERTEC400 == location)
                    || (EDDI_LOC_ERTEC200 == location))
                {
                    if (rs485_used)
                    {
                        /* GPIO 23, driver    */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_PortModeH.reg, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI7, ALTERNATE__GPIO))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                        /* GPIO 23, disable */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI23, 1))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                        /* GPIO 23, transmist */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC400_AUX_GPIO_OUT_BIT__GPI23, 0))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                    }

                    /* GPIO 24, XPLL_EXT_IN, high-impedance */
                    if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_PortModeH.reg, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI8, ALTERNATE__GPIO))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }
                    /* GPIO 24, output  */
                    if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI24, 1))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }

                    /* GPIO 25 - 30 */
                    if (EDD_STS_OK != EDDI_CpErtecSetGPIO2530Out (gpio_out_nr, &SwapGPIOP_PortModeH.reg, &SwapGPIOP_IOCTRL.reg, 1UL))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }

                    //Transfer settings to HW
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_IOCTRL))      = SwapGPIOP_IOCTRL.reg;
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_OUT))         = SwapGPIOP_Out.reg;
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_PORT_MODE_H)) = SwapGPIOP_PortModeH.reg;

                    //set mux-selector (OFF)
                    REG32b(pIRTE, MUX_SELECTOR) = 0;
                }
                else
                {
                    //this location is not supported yet!
                    retval = EDD_STS_ERR_PARAM;
                }
                break;
            } //end case

        case //set PLL-Output
                EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT:
            {
                if (   (EDDI_LOC_ERTEC400 == location)
                    || (EDDI_LOC_ERTEC200 == location))
                {
                    if (rs485_used)
                    {
                        /* GPIO 23, driver    */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_PortModeH.reg, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI7, ALTERNATE__GPIO))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                        /* GPIO 23, disable */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI23, 0))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                        /* GPIO 23, transmist */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC400_AUX_GPIO_OUT_BIT__GPI23, 1))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                    }

                    /* GPIO 24, XPLL_EXT_IN, high-impedance  */
                    if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_PortModeH.reg, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI8, ALTERNATE__GPIO))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }
                    /* GPIO 24, output  */
                    if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI24, 1))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }

                    /* GPIO 25 - 30 */
                    if (EDD_STS_OK != EDDI_CpErtecSetGPIO2530Out (gpio_out_nr, &SwapGPIOP_PortModeH.reg, &SwapGPIOP_IOCTRL.reg, 0UL))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }

                    //Transfer settings to HW
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_IOCTRL))      = SwapGPIOP_IOCTRL.reg;
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_OUT))         = SwapGPIOP_Out.reg;
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_PORT_MODE_H)) = SwapGPIOP_PortModeH.reg;

                    //set mux-selector. IRTEPort0..5 are hardwired to GPIO25...30 !
                    REG32b(pIRTE, MUX_SELECTOR) = 7UL << ((gpio_out_nr-25)*4);
                }
                else
                {
                    //this location is not supported yet!
                    retval = EDD_STS_ERR_PARAM;
                }

                break;
            } //end case

        case //set PLL-Input
                EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN:
            {
                if (   (EDDI_LOC_ERTEC400 == location)
                    || (EDDI_LOC_ERTEC200 == location))
                {
                    if (rs485_used)
                    {
                        /* GPIO 23, driver    */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_PortModeH.reg, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI7, ALTERNATE__GPIO))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                        /* GPIO 23, disable */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI23, 0))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                        /* GPIO 23, transmist */
                        if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_Out.reg, ERTEC400_AUX_GPIO_OUT_BIT__GPI23, 0))
                        {
                            retval = EDD_STS_ERR_PARAM;
                            break;
                        }
                    }

                    /* GPIO 24, XPLL_EXT_IN*/
                    if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_PortModeH.reg, ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI8, ALTERNATE__FUNCTION_1))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }
                    /* GPIO 24, input  */
                    if (EDD_STS_OK != EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI24, 1))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }

                    /* GPIO 25 - 30 */
                    if (EDD_STS_OK != EDDI_CpErtecSetGPIO2530Out (gpio_out_nr, &SwapGPIOP_PortModeH.reg, &SwapGPIOP_IOCTRL.reg, 0UL))
                    {
                        retval = EDD_STS_ERR_PARAM;
                        break;
                    }

                    //Transfer settings to HW
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_IOCTRL))      = SwapGPIOP_IOCTRL.reg;
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_OUT))         = SwapGPIOP_Out.reg;
                    *((LSA_UINT32*)(void *)(pGPIO + EDDI_GPIO_PORT_MODE_H)) = SwapGPIOP_PortModeH.reg;

                    //leave mux-selector as it is, settings for XPLL_OUT remain unchanged!
                    //REG32b(pIRTE, MUX_SELECTOR) = 0;
                }
                else
                {
                    //this location is not supported yet!
                    retval = EDD_STS_ERR_PARAM;
                }

                break;
            } //end case

        default:
        {
            retval = EDD_STS_ERR_PARAM;
        }
    } //end switch(pllmode)

    return retval;
}
/*---------------------- end [subroutine] ---------------------------------*/

LSA_VOID  EDDI_SysE200GpioSetActivityLinkLed( LSA_VOID )
{
    volatile  ERTEC200_GPIO_T                  *  pERTEC200_GPIO = (volatile ERTEC200_GPIO_T *) cp_mem.pGPIO;
    volatile  ERTEC200_AUX_GPIO_PORT_MODE_L_T  *  pGPIOP_PortModeL;
    volatile  ERTEC200_AUX_GPIO_IOCTRL_T       *  pGPIOP_IOCTRL;
    ERTEC200_AUX_GPIO_IOCTRL_T                    SwapGPIOP_IOCTRL;
    ERTEC200_AUX_GPIO_PORT_MODE_L_T               SwapGPIOP_PortModeL;

    if (cp_mem.isERTEC200)
    {
        pGPIOP_PortModeL = (ERTEC200_AUX_GPIO_PORT_MODE_L_T *)&pERTEC200_GPIO->x000000c_GPIO_PORT_MODE_L;
        pGPIOP_IOCTRL    = (ERTEC200_AUX_GPIO_IOCTRL_T      *)&pERTEC200_GPIO->x0000000_GPIO_IOCTRL;

        SwapGPIOP_PortModeL.reg = pGPIOP_PortModeL->reg;
        SwapGPIOP_IOCTRL.reg    = pGPIOP_IOCTRL->reg;

        //P1-XLINK-LED, Function 2
        EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI4, 1);

        //P2-XLINK-LED, Function 2
        EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI5, 1);

        //P1-XACTIVITY-LED, Function 4
        EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI6, 3);

        //P2-XACTIVITY-LED, Function 4
        EDDI_CpSetBitField(&SwapGPIOP_PortModeL.reg, ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI7, 3);

        //GPIO switch to output mode
        EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI4, 0);
        EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI5, 0);
        EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI6, 0);
        EDDI_CpSetBitField(&SwapGPIOP_IOCTRL.reg, ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI7, 0);

        pGPIOP_PortModeL->reg = SwapGPIOP_PortModeL.reg;
        pGPIOP_IOCTRL->reg    = SwapGPIOP_IOCTRL.reg;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#else
// making DACODA happy about warning 766

LSA_VOID  EDDI_SysErtecGpioDummy( LSA_PRIORITY_TYPE PriorityType )
{
    LSA_UNUSED_ARG(PriorityType);

    if ((EDD_CFG_MAX_INTERFACE_CNT + LSA_RSP_OK_OFFSET) <= EDDI_CFG_MAX_DEVICES)
    {
    }
}

#endif //EDDI_CFG_ERTEC_400 || EDDI_CFG_ERTEC_200


/*****************************************************************************/
/*  end of file eddi_sys_ertec_gpio.c                                        */
/*****************************************************************************/
