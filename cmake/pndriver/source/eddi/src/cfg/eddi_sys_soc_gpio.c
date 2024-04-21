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
/*  F i l e               &F: eddi_sys_soc_gpio.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* examples for SOC-specific functions.                                      */
/* - GPIO                                                                    */
/* - General                                                                 */
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

#include "eddi_inc.h"

#include "edd_cfg.h"
#include "eddi_cfg.h"
#include "edd_usr.h"
#include "eddi_usr.h"
#include "eddi_sys.h"

#if defined (EDDI_CFG_SOC)

#include <eddi_sys_reg_soc12.h>
#include <eddi_sys_soc_hw.h>

#ifdef __cplusplus //If C++ - compiler: Use C linkage
extern "C"
{
#endif

//#define SYS_SOC_USE_NO_GLOBAL_VARS
//#define SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO 47  //ISO_OUT1

struct cp_mem_tag
{
    LSA_UINT32  pIRTE;
    LSA_UINT32  pIOCC;
    LSA_UINT32  pGPIO;
    LSA_UINT32  pTIM;

} cp_mem_soc_gpio;

#if !defined (SYS_SOC_USE_NO_GLOBAL_VARS)
LSA_UINT32 p_soc_IOCC = 0;
LSA_UINT32 p_soc_IRTE = 0;
LSA_UINT32 p_soc_GPIO = 0;
LSA_UINT32 p_soc_TIM  = 0;
#endif

static LSA_UINT32 current_scf = 32;


/*=============================================================================*/
/*        System Resources                                                     */
/*=============================================================================*/
static  LSA_BOOL  EDDI_SysGetSocBasePtrs(LSA_UINT32 pIRTE, LSA_UINT32 *ppIOCC, LSA_UINT32 *ppGPIO, LSA_UINT32 *ppTIM)
{
    if (cp_mem_soc_gpio.pIRTE == pIRTE)
    {
        *ppIOCC = cp_mem_soc_gpio.pIOCC;
        *ppGPIO = cp_mem_soc_gpio.pGPIO;
        *ppTIM  = cp_mem_soc_gpio.pTIM;

        return LSA_TRUE;
    }

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:       EDDI_SysSetSocBasePtr()
*
* D e s c r i p t i o n: saves the base-ptrs for later lookup
*/
LSA_VOID  EDDI_SysSetSocBasePtr(LSA_UINT32 pSWIReg, LSA_UINT32 pSOC_IOCC, LSA_UINT32 pSOC_GPIO, LSA_UINT32 pSOC_TIM)
{
    cp_mem_soc_gpio.pIRTE      = pSWIReg;
    cp_mem_soc_gpio.pIOCC      = pSOC_IOCC;
    cp_mem_soc_gpio.pGPIO      = pSOC_GPIO;
    cp_mem_soc_gpio.pTIM       = pSOC_TIM;
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

/*=============================================================================*/
/*        GPIOs                                                                */
/*=============================================================================*/

/*=============================================================================
 * function name:  EDDI_CpSocSetPllPort()
 *
 * function:       initializes a SOC-GPIO for PLL-Out/In
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
/* Attention: needs global variables p_soc_IOCC
                                     p_soc_IRTE
                                     p_soc_GPIO !!! */
LSA_RESULT  EDDI_CpSocSetPllPort( EDDI_SYS_HANDLE         hSysDev,
                                  LSA_UINT32              pIRTE,
                                  LSA_UINT32              location,
                                  LSA_UINT16              pllmode)
{
    //The baseptrs to the IOCC/GPIO-Regs cannot be calculated directly from pIRTE, because GPIO and IOCC
    //are accessed through different BARs on SOC2, and through the same BAR on SOC1

    LSA_UINT32          pIOCC; //Baseptr to IOCC-Regs
    LSA_UINT32          pGPIO; //Baseptr to GPIO-Regs
    LSA_RESULT          retval = EDD_STS_OK;
    LSA_UINT16  const   gpio_out_nr = (LSA_UINT16)((EDDI_LOC_SOC1 == location)?SYS_SOC_SOC1_PLLGPIO_OUT:SYS_SOC_SOC2_PLLGPIO_OUT);
    LSA_UINT16  const   gpio_in_nr  = (LSA_UINT16)((EDDI_LOC_SOC1 == location)?SYS_SOC_SOC1_PLLGPIO_IN:SYS_SOC_SOC2_PLLGPIO_IN);

    LSA_UNUSED_ARG( hSysDev );

    #if !defined (SYS_SOC_USE_NO_GLOBAL_VARS)
    //Check the baseptrs for the call
    if (( pIRTE      != p_soc_IRTE ) || // not matches
        ( p_soc_IOCC == 0          ) || // reset not done ?
        ( p_soc_GPIO == 0          ))
    {
        return EDD_STS_ERR_PARAM;
    }

    pGPIO = p_soc_GPIO;
    pIOCC = p_soc_IOCC;
    #else
    //Get the baseptrs that "fit" to the IRTE-ptr
    {
        LSA_UINT32  pTIM; //Baseptr to TIM-Regs
        if (!EDDI_SysGetSocBasePtrs(pIRTE, &pIOCC, &pGPIO, &pTIM))
        {
            return EDD_STS_ERR_PARAM;
        }
    }
    #endif

    switch (pllmode)
    {
        case //switch output off
             EDDI_PRM_DEF_PLL_MODE_OFF:
            {
                if (EDDI_LOC_SOC2 == location)
                {
                    if ((gpio_out_nr>=46) && (gpio_out_nr<=53))
                    {
                        //switch port back to original I/O;
                        REG32b(pGPIO, GPIO_IOCTRL_1) = REG32b(pGPIO, GPIO_IOCTRL_1) | (1UL<<(gpio_out_nr-32));                             //input

                        if (gpio_out_nr < 48)
                        {
                            REG32b(pGPIO, GPIO_PORT_MODE_1_L) = (REG32b(pGPIO, GPIO_PORT_MODE_1_L) & (~(3UL<<((gpio_out_nr-32)*2)))); // set to altfct 0
                        }
                        else
                        {
                            REG32b(pGPIO, GPIO_PORT_MODE_1_H) = (REG32b(pGPIO, GPIO_PORT_MODE_1_H) & (~(3UL<<((gpio_out_nr-48)*2)))); // set to altfct 0
                        }

                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 0;                                  //clear MUX-Selector
                    }
                    else
                    {
                        retval = EDD_STS_ERR_PARAM;
                    }
                }
                else if (EDDI_LOC_SOC1 == location)
                {
                    if ((gpio_out_nr>=152) && (gpio_out_nr<=159))
                    {
                        //switch port back to original I/O;
                        REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) | (1UL<<(gpio_out_nr-128));                            //input
                        REG32b(pGPIO, GPIO_PORT_MODE_4_H) = (REG32b(pGPIO, GPIO_PORT_MODE_4_H) & (~(3UL<<((gpio_out_nr-144)*2))));         // set to altfct 0
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 0;                                  //clear MUX-Selector
                    }
                    else if ((gpio_out_nr>=168) && (gpio_out_nr<=175))
                    {
                        //switch port back to original I/O;
                        REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) | (1UL<<(gpio_out_nr-160));                            //input
                        REG32b(pGPIO, GPIO_PORT_MODE_5_L) = (REG32b(pGPIO, GPIO_PORT_MODE_5_L) & (~(3UL<<((gpio_out_nr-160)*2))));         // set to altfct 0
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 0;                                  //clear MUX-Selector
                    }
                    else
                    {
                        retval = EDD_STS_ERR_PARAM;
                    }
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
                if (EDDI_LOC_SOC2 == location)
                {
                    if ((gpio_out_nr>=46) && (gpio_out_nr<=53))
                    {
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nr-46)) = 0x0BUL;                                                           //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_1) = REG32b(pGPIO, GPIO_IOCTRL_1) & (~(1UL<<(gpio_out_nr-32)));                                                 //output

                        if (gpio_out_nr < 48)
                        {
                            REG32b(pGPIO, GPIO_PORT_MODE_1_L) = (REG32b(pGPIO, GPIO_PORT_MODE_1_L) & (~(3UL<<((gpio_out_nr-32)*2)))) | (1UL<<((gpio_out_nr-32)*2));  // set to altfct 1
                        }
                        else
                        {
                            REG32b(pGPIO, GPIO_PORT_MODE_1_H) = (REG32b(pGPIO, GPIO_PORT_MODE_1_H) & (~(3UL<<((gpio_out_nr-48)*2)))) | (1UL<<((gpio_out_nr-48)*2));  // set to altfct 1
                        }

                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 7;                                                   // set IRTE-mux (XPLL_EXT_OUT on Port 0)

                        #if defined (SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO)
                        //Signal-Path:     IOCC                                                       DSA_PLL
                        //             IRTE_PLL_OUT_O(0)->DSA_PLL_IN
                        //                                                DSA_PLL_IN->[DSA_PLL (reset-default is bypass-mode)]->DSA_PLL_IN_OUT
                        //             DSA_PLL_IN_OUT->ISO_OUT1 (SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO)

                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO-46)) = 0x04UL;                            //set port-mux in IOCC (DSA_PLL_IN_OUT -> ISO_OUT_1):
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_dsa_pll_in_cntrl)                                         = 0x06UL;                            //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> DSA_PLL_IN):
                        REG32b(pGPIO, GPIO_IOCTRL_1) = REG32b(pGPIO, GPIO_IOCTRL_1) & (~(1UL<<(SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO-32)));                  //output

                        if (SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO < 48)
                        {
                            REG32b(pGPIO, GPIO_PORT_MODE_1_L) = (REG32b(pGPIO, GPIO_PORT_MODE_1_L) & (~(3UL<<((SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO-32)*2)))) | (1UL<<((SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO-32)*2));  // set to altfct 1
                        }
                        else
                        {
                            REG32b(pGPIO, GPIO_PORT_MODE_1_H) = (REG32b(pGPIO, GPIO_PORT_MODE_1_H) & (~(3UL<<((SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO-48)*2)))) | (1UL<<((SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO-48)*2));  // set to altfct 1
                        }
                        #endif //defined (SYS_SOC_SET_SOC2_DSA_PLL_OUT_GPIO)
                    }
                    else
                    {
                        retval = EDD_STS_ERR_PARAM;
                    }
                }
                else if (EDDI_LOC_SOC1 == location)
                {
                    //XPLLOUT
                    if ((gpio_out_nr>=152) && (gpio_out_nr<=159))
                    {
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nr-152)) = 0x0BUL;                                                           //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) & (~(1UL<<(gpio_out_nr-128)));                                                 //output
                        REG32b(pGPIO, GPIO_PORT_MODE_4_H) = REG32b(pGPIO, GPIO_PORT_MODE_4_H) | (3UL<<((gpio_out_nr-144)*2));         // set to altfct 3
                        #if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 5;                          // set IRTE-mux (XInt_ClkCy on Port 0)
                        #else
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 7;                          // set IRTE-mux (XAplClk0 on Port 0)
                        #endif
                    }
                    else if ((gpio_out_nr>=168) && (gpio_out_nr<=175))
                    {
                        //SOC1-PCI-Board: Set EN_2P5V (GPIO112)
                        if ((gpio_out_nr>=152) && (gpio_out_nr<=159))
				        {
					        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nr-152)) = 0x0BUL;                              //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> ISO_OUT_n):
					        REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) & (~(1UL<<(gpio_out_nr-128)));                    //output
					        REG32b(pGPIO, GPIO_PORT_MODE_4_H) = REG32b(pGPIO, GPIO_PORT_MODE_4_H) | (3UL<<((gpio_out_nr-144)*2));         // set to altfct 3
				        }
				        else if ((gpio_out_nr>=168) && (gpio_out_nr<=175))
				        {
					        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nr-168)) = 0x0BUL;                              //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> ISO_OUT_n):
					        REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) & (~(1UL<<(gpio_out_nr-160)));                    //output
					        REG32b(pGPIO, GPIO_PORT_MODE_5_L) = REG32b(pGPIO, GPIO_PORT_MODE_5_L) | (3UL<<((gpio_out_nr-160)*2));         // set to altfct 3
				        }
				        else
				        {
					        retval = EDD_STS_ERR_PARAM;
				        }

                        if (current_scf <= 2)
                        {
                            #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
                            REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
						        ( REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL) | 0x5UL);       // set IRTE-mux (First 4 Bits to XPLL_EXT_OUT on Port 0)
                            #else
                            REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
						        ( REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL) | 0x6UL);       // set IRTE-mux (First 4 Bits to XPLL_EXT_OUT on Port 0)
                            #endif
                        }
                        else
                        {
                            REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
						        ( REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL) | 0x7UL);       // set IRTE-mux (First 4 Bits to XPLL_EXT_OUT on Port 0)
                        }
                    }
                    else
                    {
                        retval = EDD_STS_ERR_PARAM;
                    }

                    #if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
                    {
                        LSA_UINT32 reg;
                        LSA_UINT32 iso_port=1;
                        LSA_UINT32 gpio_out_nrx;

                        //SOC1-PCI-Board: Set EN_2P5V (GPIO112)
                        REG32b(pGPIO, GPIO_IOCTRL_3) = REG32b(pGPIO, GPIO_IOCTRL_3) & (~(1UL<<(112-96)));                       //output
                        REG32b(pGPIO, GPIO_PORT_MODE_3_H) = (REG32b(pGPIO, GPIO_PORT_MODE_3_H) & (~(3UL<<((112-112)*2))));    // set to altfct 0
                        REG32b(pGPIO, GPIO_OUT_3) = (REG32b(pGPIO, GPIO_OUT_3) | ((1UL<<((112-96)))));

                        //ZA_INT-Out
                        gpio_out_nrx = SYS_SOC_SOC1_ZAINT_OUT;
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nrx-152)) = 0x0BUL+iso_port;              //set port-mux in IOCC (IRTE_PLL_OUT_n -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) & (~(1UL<<(gpio_out_nrx-128)));             //output
                        REG32b(pGPIO, GPIO_PORT_MODE_4_H) = REG32b(pGPIO, GPIO_PORT_MODE_4_H) | (3UL<<((gpio_out_nrx-144)*2));  // set to altfct 3
                        reg = REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR);                            // set IRTE-mux (XInt_ClkCyInt on Port 1)
                        reg |= 6UL<<(4*iso_port);
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = reg;
                        iso_port++;

                        //ZM_INT-Out
                        gpio_out_nrx = SYS_SOC_SOC1_ZMINT_OUT;
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nrx-152)) = 0x0BUL+iso_port;              //set port-mux in IOCC (IRTE_PLL_OUT_n -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) & (~(1UL<<(gpio_out_nrx-128)));             //output
                        REG32b(pGPIO, GPIO_PORT_MODE_4_H) = REG32b(pGPIO, GPIO_PORT_MODE_4_H) | (3UL<<((gpio_out_nrx-144)*2));  // set to altfct 3
                        reg = REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR);                            // set IRTE-mux (XIntComp1 on Port 2)
                        reg |= 8UL<<(4*iso_port);
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = reg;
                        iso_port++;

                        //ZE_INT-Out
                        gpio_out_nrx = SYS_SOC_SOC1_ZEINT_OUT;
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nrx-168)) = 0x0BUL+iso_port;              //set port-mux in IOCC (IRTE_PLL_OUT_n -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) & (~(1UL<<(gpio_out_nrx-160)));             //output
                        REG32b(pGPIO, GPIO_PORT_MODE_5_L) = REG32b(pGPIO, GPIO_PORT_MODE_5_L) | (3UL<<((gpio_out_nrx-160)*2));  // set to altfct 3
                        reg = REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR);                            // set IRTE-mux (XIntComp2 on Port 3)
                        reg |= 9UL<<(4*iso_port);
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = reg;
                        iso_port++;

                        //ZS_INT-Out
                        gpio_out_nrx = SYS_SOC_SOC1_ZSINT_OUT;
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nrx-168)) = 0x0BUL+iso_port;              //set port-mux in IOCC (IRTE_PLL_OUT_n -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) & (~(1UL<<(gpio_out_nrx-160)));             //output
                        REG32b(pGPIO, GPIO_PORT_MODE_5_L) = REG32b(pGPIO, GPIO_PORT_MODE_5_L) | (3UL<<((gpio_out_nrx-160)*2));  // set to altfct 3
                        reg = REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR);                            // set IRTE-mux (XIntComp3 on Port 4)
                        reg |= 0xAUL<<(4*iso_port);
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = reg;
                        iso_port++;

                        //ZIT_INT-Out
                        gpio_out_nrx = SYS_SOC_SOC1_ZITINT_OUT;
                        REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio_out_nrx-168)) = 0x0BUL+iso_port;              //set port-mux in IOCC (IRTE_PLL_OUT_n -> ISO_OUT_n):
                        REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) & (~(1UL<<(gpio_out_nrx-160)));             //output
                        REG32b(pGPIO, GPIO_PORT_MODE_5_L) = REG32b(pGPIO, GPIO_PORT_MODE_5_L) | (3UL<<((gpio_out_nrx-160)*2));  // set to altfct 3
                        reg = REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR);                            // set IRTE-mux (XIntComp4 on Port 4)
                        reg |= 0xBUL<<(4*iso_port);
                        REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = reg;
                    }
                    #endif
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
                if (EDDI_LOC_SOC2 == location)
                {
                    switch (gpio_in_nr)
                    {
                        case 55:
                        {
                            REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x00UL;                                       //set port-mux in IOCC (IRTE_PLL_EXT_IN --> IRTE_PLL_IN)
                            REG32b(pGPIO, GPIO_IOCTRL_1) = REG32b(pGPIO, GPIO_IOCTRL_1) | (1UL<<(gpio_in_nr-32));                             //input
                            REG32b(pGPIO, GPIO_PORT_MODE_1_H) = (REG32b(pGPIO, GPIO_PORT_MODE_1_H) & (~(3UL<<((gpio_in_nr-48)*2))));          // set to altfct 0
                            break;
                        }
                        default:
                        {
                            retval = EDD_STS_ERR_PARAM;
                        }
                    } //end switch(gpio)
                }
                else if (EDDI_LOC_SOC1 == location)
                {
                    switch (gpio_in_nr)
                    {
                        case 94:
                        case 95:
                        {
                            if (94 == gpio_in_nr)
                            {
                                REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x01UL;                                     //set port-mux in IOCC (DSA_PLL_EXT_IN --> IRTE_PLL_IN)
                            }
                            else
                            {
                                REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x00UL;                                     //set port-mux in IOCC (IRTE_PLL_EXT_IN0 --> IRTE_PLL_IN)
                            }

                            REG32b(pGPIO, GPIO_IOCTRL_2) = REG32b(pGPIO, GPIO_IOCTRL_2) | (1UL<<(gpio_in_nr-64));                      //input
                            REG32b(pGPIO, GPIO_PORT_MODE_2_H) = (REG32b(pGPIO, GPIO_PORT_MODE_2_H) & (~(3UL<<((gpio_in_nr-80)*2))));   // set to altfct 0
                            break;
                        }

                        case 127:
                        {
                            REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x02UL;                                              //set port-mux in IOCC (IRTE_PLL_EXT_IN --> IRTE_PLL_IN)
                            REG32b(pGPIO, GPIO_IOCTRL_3) = REG32b(pGPIO, GPIO_IOCTRL_3) | (1UL<<(gpio_in_nr-96));                             //input
                            REG32b(pGPIO, GPIO_PORT_MODE_3_H) = (REG32b(pGPIO, GPIO_PORT_MODE_3_H) & (~(3UL<<((gpio_in_nr-112)*2))));         // set to altfct 0
                            break;
                        }

                        case 178:
                        {
                            REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x03UL;                                              //set port-mux in IOCC (IRTE_PLL_EXT_IN --> IRTE_PLL_IN)
                            REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) | (1UL<<(gpio_in_nr-160));                            //input
                            REG32b(pGPIO, GPIO_PORT_MODE_5_H) = (REG32b(pGPIO, GPIO_PORT_MODE_5_H) & (~(3UL<<((gpio_in_nr-176)*2))));         // set to altfct 0
                            break;
                        }

                        default:
                        {
                            retval = EDD_STS_ERR_PARAM;
                        }
                    } //end switch(gpio_in_nr)
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

/*=============================================================================*/
/*        TIMERs                                                               */
/*=============================================================================*/
#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)

typedef struct cp_soc_timer_tag
{
    void        * pTIMER_TOP;       //Baseaddress of TIMERTOP-Base (virt.)
    void        * pTIMER;           //Baseaddress of the selected timer (virt.)
    void        * pTIMERAux;        //Baseaddress of the auxiliary timer (virt.) for polling mode
    EDDI_HANDLE   hDDB;             //Lower device handle
    LSA_UINT8     TimNr;            //Timer nr used
    LSA_BOOL      PollMode;         //Polling mode or Interrupt mode
    LSA_BOOL      TimerStarted;     //Timer started or not
    LSA_UINT32    TimeOut_ns;       //Timeout time in ns
    LSA_UINT32    LastAuxVal;       //Last value of aux. timer (polling mode)
#if (0)  
    LSA_UINT32  * pKRAM;
#endif
} CP_SOC_TIMER_TYPE;

static CP_SOC_TIMER_TYPE cp_soc_timer_cb = {0};

/*=============================================================================
 * function name:  EDDI_CpSocTimerInit()
 *
 * function:       initializes the timer-handling for 1 TIMERTOP-Timer
 *
 * return value:   LSA_OK, LSA_RET_ERR_PARAM
 *===========================================================================*/
/* !!!example for implementation only !!!*/
#define CP_SOC_TIMERTOP_1TICK_NS    40UL  //each tick is 40ns
LSA_RESULT EDDI_CpSocTimerInit( LSA_VOID    *  pSOC_TIM,                    //virtual base address of TIMER_TOP registers
                                LSA_UINT8      TimNr,                       //Timer to be used (0..5)
                                LSA_BOOL       PollMode )                   //LSA_TRUE: Polling mode without pnDevDrv
{
    LSA_UINT32  TempReg;

    if (TimNr > 5)
    {
        return LSA_RET_ERR_PARAM;
    }
    if (!PollMode)
    {
        //interruptmode currently not supported
        return LSA_RET_ERR_PARAM;
    }
    else if (TimNr & 1)
    {
        //For polling mode, only even timer-nrs are allowed
        return LSA_RET_ERR_PARAM;
    }
    
    //Init controlblock
    cp_soc_timer_cb.pTIMER_TOP      = pSOC_TIM;
    cp_soc_timer_cb.pTIMER          = (LSA_VOID *)((LSA_UINT8 *)pSOC_TIM + (TimNr*0x20));
    cp_soc_timer_cb.pTIMERAux       = (LSA_VOID *)0;
    cp_soc_timer_cb.TimNr           = TimNr;
    cp_soc_timer_cb.PollMode        = PollMode;
    cp_soc_timer_cb.TimeOut_ns      = 0;
    cp_soc_timer_cb.TimerStarted    = LSA_FALSE;
#if (0)
    cp_soc_timer_cb.pKRAM           = (LSA_UINT32 *)(p_soc_IRTE + 0x100000);
#endif

    //Init timer
    TempReg = REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG);
    REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG) = TempReg & (~((1<<(6+TimNr))+(1<<TimNr)));   //TIM_n_CLK_EN = 0, TIM_n_INT_GATE_TRIG = 0
    REG32b(cp_soc_timer_cb.pTIMER, TIM_0_PRESCALER_REG) = 1;                                                //Prescaler=1 ==> each tick is 40ns
    REG32b(cp_soc_timer_cb.pTIMER, TIM_0_MODE_REG) = 0x0001;                                                // 00:INIT_BIT	= 1, reset by HW
                                                                                                            // 04:CLK_INPUT_SELECT	0 (CLK_TIMT, 50MHz)
                                                                                                            // 05:RELOAD_DISABLE	0 (Reload Mode)
                                                                                                            // 06:DIS_RLD_WHEN_WR_LDR	0 (Reload Counter when Writing Load--Register enabled)
                                                                                                            // 07:EXT_GATE_TRIG_ENABLE	0 (Internal Gate trigger)
                                                                                                            // 08:GATE_POLARITY	0 (high)
                                                                                                            // 09:GATE_EFFECT	0 (Gate Mode)
                                                                                                            // 10:TIMER_OUT_POLARITY	0 (High active)
                                                                                                            // 11-13:EVENT1_CONTROL	0 (irrelevant)
                                                                                                            // 14-16:EVENT2_CONTROL	0 (irrelevant)

    if (PollMode)
    {
        cp_soc_timer_cb.pTIMERAux = (LSA_VOID *)((LSA_UINT8 *)pSOC_TIM + ((TimNr+1)*0x20));
        TempReg = REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG);
        REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG) = TempReg & (~((1<<(6+(TimNr+1)))+(1<<(TimNr+1))));   //TIM_n_CLK_EN = 0, TIM_n_INT_GATE_TRIG = 0
        REG32b(cp_soc_timer_cb.pTIMERAux, TIM_0_PRESCALER_REG) = 1;                                             //Prescaler=1 ==> each tick is 40ns
        REG32b(cp_soc_timer_cb.pTIMERAux, TIM_0_MODE_REG) = 0x0091;                                             // 00:INIT_BIT	= 1, reset by HW
                                                                                                                // 04:CLK_INPUT_SELECT	1 (Ext trigger)
                                                                                                                // 05:RELOAD_DISABLE	0 (Reload Mode)
                                                                                                                // 06:DIS_RLD_WHEN_WR_LDR	0 (Reload Counter when Writing Load--Register enabled)
                                                                                                                // 07:EXT_GATE_TRIG_ENABLE	1 (Ext. Gate trigger)
                                                                                                                // 08:GATE_POLARITY	0 (high)
                                                                                                                // 09:GATE_EFFECT	0 (Gate Mode)
                                                                                                                // 10:TIMER_OUT_POLARITY	0 (High active)
                                                                                                                // 11-13:EVENT1_CONTROL	0 (irrelevant)
                                                                                                                // 14-16:EVENT2_CONTROL	0 (irrelevant)
        TempReg = REG32b(cp_soc_timer_cb.pTIMER_TOP, EXT_GATE_TRIG_MUX_REG) & (~(0xF << ((TimNr+1)*4)));
        REG32b(cp_soc_timer_cb.pTIMER_TOP, EXT_GATE_TRIG_MUX_REG) = TempReg | ((0xA+TimNr) << ((TimNr+1)*4));   //TimNr triggers TimNr+1
    }

    return LSA_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_CpSocTimerCmd()
 *
 * function:       Starts or stops the previously initialized cyclic timer
 *
 * return value:   LSA_OK, LSA_RET_ERR_PARAM
 *===========================================================================*/
/* !!!example for implementation only !!!*/
LSA_RESULT EDDI_CpSocTimerCmd ( LSA_UINT8      Command,                     //SYS_TIMER_CMD_START, SYS_TIMER_CMD_STOP
                                EDDI_HANDLE    hDDB,                        //Lower device handle
                                LSA_UINT32     TimeOut_ns )                 //Timeout time in ns
{
    LSA_UINT32  TimeOut;
    LSA_UINT32  TempReg;

    switch (Command)
    {
        case SYS_TIMER_CMD_START:
        {
            if (TimeOut_ns < CP_SOC_TIMERTOP_1TICK_NS)
            {
                //how shall we calc this??
                return LSA_RET_ERR_PARAM;
            }

            cp_soc_timer_cb.hDDB = hDDB;    //store lower device handle

            if (cp_soc_timer_cb.PollMode)
            {
                cp_soc_timer_cb.LastAuxVal = 0xFFFFFFFF;   
                REG32b(cp_soc_timer_cb.pTIMERAux, TIM_0_LOAD_REG) = cp_soc_timer_cb.LastAuxVal;
                REG32b(cp_soc_timer_cb.pTIMERAux, TIM_0_MODE_REG) = 0x0091;
                TempReg = REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG);
                REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG) = TempReg | ((1<<(6+(cp_soc_timer_cb.TimNr+1)))+(1<<(cp_soc_timer_cb.TimNr+1)));   //TIM_n_CLK_EN = 1, TIM_n_INT_GATE_TRIG = 1
            }

            //GO!
            TimeOut = TimeOut_ns / CP_SOC_TIMERTOP_1TICK_NS;
            REG32b(cp_soc_timer_cb.pTIMER, TIM_0_LOAD_REG) = TimeOut;
            REG32b(cp_soc_timer_cb.pTIMER, TIM_0_MODE_REG) = 0x0001;
            TempReg = REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG);
            REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG) = TempReg | ((1<<(6+cp_soc_timer_cb.TimNr))+(1<<cp_soc_timer_cb.TimNr));   //TIM_n_CLK_EN = 1, TIM_n_INT_GATE_TRIG = 1
            cp_soc_timer_cb.TimerStarted = LSA_TRUE;
            break;
        }
        case SYS_TIMER_CMD_STOP:
        {
            cp_soc_timer_cb.TimerStarted = LSA_FALSE;
            TempReg = REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG);
            REG32b(cp_soc_timer_cb.pTIMER_TOP, GATE_TRIG_CONTROL_REG) = TempReg & (~((1<<(6+cp_soc_timer_cb.TimNr))+(1<<cp_soc_timer_cb.TimNr)));   //TIM_n_CLK_EN = 0, TIM_n_INT_GATE_TRIG = 0
            break;
        }
        default:
        {
            return LSA_RET_ERR_PARAM;
        }
    }

    return LSA_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_CpSocTimerCheck()
 *
 * function:       Checks the previously started timer for timeout, and calls 
 *                 eddi_ExtTimerInterrupt
 *
 * return value:   LSA_OK, LSA_RET_ERR_PARAM
 *===========================================================================*/
/* !!!example for implementation only !!!*/
LSA_RESULT  EDDI_CpSocTimerCheck( LSA_BOOL  *  pbTimeoutOccured )
{
    LSA_UINT32 AuxVal;

    if (   !cp_soc_timer_cb.TimerStarted
        || !cp_soc_timer_cb.PollMode)
    {
        *pbTimeoutOccured = LSA_FALSE;
        return LSA_RET_ERR_SEQUENCE;
    }

    AuxVal = REG32b(cp_soc_timer_cb.pTIMERAux, TIM_0_COUNT_REG);
    if (AuxVal != cp_soc_timer_cb.LastAuxVal)
    {
        eddi_ExtTimerInterrupt(cp_soc_timer_cb.hDDB);
        //if ((cp_soc_timer_cb.LastAuxVal-AuxVal) > 1)
        //{
        //    eddi_ExtTimerInterrupt(cp_soc_timer_cb.hDDB);   //try to compensate for lost timerevent
        //}
        cp_soc_timer_cb.LastAuxVal = AuxVal;
        *pbTimeoutOccured = LSA_TRUE;
#if (0)
        *cp_soc_timer_cb.pKRAM     = REG32b(cp_soc_timer_cb.pTIMER, TIM_0_COUNT_REG);
        *(cp_soc_timer_cb.pKRAM+1) = AuxVal;
#endif
    }

    return LSA_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

#else

LSA_RESULT EDDI_CpSocTimerInit( LSA_VOID    *  pSOC_TIM,                    //virtual base address of TIMER_TOP registers
                                LSA_UINT8      TimNr,                       //Timer to be used (0..5)
                                LSA_BOOL       PollMode )                   //LSA_TRUE: Polling mode without pnDevDrv
{
    LSA_UNUSED_ARG(pSOC_TIM);
    LSA_UNUSED_ARG(TimNr);
    LSA_UNUSED_ARG(PollMode);
    return LSA_OK;
}

LSA_RESULT  EDDI_CpSocTimerCmd( LSA_UINT8    Command,                     //SYS_TIMER_CMD_START, SYS_TIMER_CMD_STOP
                                EDDI_HANDLE  hDDB,                        //Lower device handle
                                LSA_UINT32   TimeOut_ns )                 //Timeout time in ns
{
    LSA_UNUSED_ARG(Command);
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(TimeOut_ns);
    return LSA_OK;
}

LSA_RESULT  EDDI_CpSocTimerCheck( LSA_BOOL  *  pbTimeoutOccured )
{
    LSA_UNUSED_ARG(pbTimeoutOccured);
    return LSA_OK;
}

#endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX


#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#else

void  EDDI_SysSocGpioDummy( void )
{
}

#endif //EDDI_CFG_SOC


/*****************************************************************************/
/*  end of file eddi_sys_soc_gpio.c                                          */
/*****************************************************************************/
