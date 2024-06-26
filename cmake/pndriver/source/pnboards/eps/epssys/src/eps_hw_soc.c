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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_hw_soc.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS SOC1 adaption                                                        */
/*                                                                           */
/*****************************************************************************/

#define EPS_MODULE_ID      20020
#define LTRC_ACT_MODUL_ID  20020

/* EPS includes */
#include <eps_sys.h>	          /* Types / Prototypes / Funcs */

#if (PSI_CFG_USE_EDDI == 1) && defined (PSI_CFG_EDDI_CFG_SOC)
/*---------------------------------------------------------------------------*/

#include <eps_trc.h>	          /* Tracing                                  */
#include <eps_locks.h>            /* EPS Locks                                */
#include <eps_tasks.h>            /* EPS us_sleep                             */
#include <eps_cp_hw.h>            /* EPS CP PSI adaption                      */
#include <eps_cp_mem.h>           /* EPS CP PSI adaption                      */
#include <edd_inc.h>              /* EDD / HW Includes                        */
#include <eddi_sys_reg_soc12.h>   /* EDD / HW Includes                        */
#include <eps_hw_soc.h>           /* EPS SOC Blink functions                  */

#ifdef EPS_CFG_USE_EXTERNAL_LED
#include <eps_app.h>              /* EPS Application Api                      */
#endif

#include <eps_register.h>
/*  Ext Timer */
//#define EPS_SOC_TIMERTOP_1TICK_NS       40UL        //each tick is 40ns
//#define EPS_SOC_TIMER_NR                0

/*  GPIO Makros */
#define SOC_GPIO_PORT_PORT_MODE(x)      (x / 16)    //for Port-Mode-registers

/*  I2C  */
#define EPS_SOC_I2C_GPIO_SCL            184
#define EPS_SOC_I2C_GPIO_SDA            185

#define EPS_SOC_GPIO_I2C_MUX_S0         68          //SOC1 PCIe board
#define EPS_SOC_GPIO_I2C_MUX_S1         69          //SOC1 PCIe board

// ***** states returned by HW into the status register
#define EPS_SOC_I2C_STATE_START_0      0x08         //START condition transmitted
#define EPS_SOC_I2C_STATE_START_N      0x10         //Repeated START condition transmitted
#define EPS_SOC_I2C_STATE_ADR_WR       0x18         //Address + write bit transmitted, ACK received
#define EPS_SOC_I2C_STATE_WR_DATA_ACK  0x28         //Data byte transmitted in master mode, ACK received
//#define EPS_SOC_I2C_STATE_WR_DATA_NAK  0x30         //Data byte transmitted in master mode, ACK not received
#define EPS_SOC_I2C_STATE_ADR_RD       0x40         //Address + read bit transmitted, ACK received
#define EPS_SOC_I2C_STATE_RD_DATA_ACK  0x50         //Data byte received in master mode, ACK transmitted
#define EPS_SOC_I2C_STATE_RD_DATA_NAK  0x58         //Data byte received in master mode, NAK transmitted
#define EPS_SOC_I2C_STATE_IDLE         0xF8         //No relevant status information, IFLG=0

// ***** commands to the HW statemachine of the MI2C-IP
#define EPS_SOC_I2C_CMD_IDLE           (SOC_REG_I2C_MI2_CNTR__MSK_IEN | SOC_REG_I2C_MI2_CNTR__MSK_ENAB)
#define EPS_SOC_I2C_CMD_START          (EPS_SOC_I2C_CMD_IDLE | SOC_REG_I2C_MI2_CNTR__MSK_STA)
#define EPS_SOC_I2C_CMD_WR_DATA        (EPS_SOC_I2C_CMD_IDLE | 0)
#define EPS_SOC_I2C_CMD_RD_DATA_ACK    (EPS_SOC_I2C_CMD_IDLE | SOC_REG_I2C_MI2_CNTR__MSK_AAK)
#define EPS_SOC_I2C_CMD_RD_DATA_NAK    (EPS_SOC_I2C_CMD_IDLE | 0)
#define EPS_SOC_I2C_CMD_STOP           (EPS_SOC_I2C_CMD_IDLE | SOC_REG_I2C_MI2_CNTR__MSK_STP)

#define EPS_SOC_I2C_BUS_HIGHZ          (SOC_REG_I2C_SW_I2C_CTRL__MSK_SDA_I | SOC_REG_I2C_SW_I2C_CTRL__MSK_SCL_I | SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SDA_O | SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SCL_O)

// ***** register and bits in NSC PhyTranceiver for external blink functions
#define EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR             24  //18h
#define EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__ACTLED         0
#define EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__LNKLED         1
#define EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_ACTLED     3
#define EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_LNKLED     4

/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/

// ***** check macros after I2C command
#define EPS_SOC_I2C_WAIT_IFLG( pI2C, ToCnt)             {ToCnt = 0x1000; while(ToCnt) { if (REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) & SOC_REG_I2C_MI2_CNTR__MSK_IFLG) break; ToCnt--; }}
#define EPS_SOC_I2C_WAIT_STATUS( pI2C, ToCnt, Stat )    {ToCnt = 0x1000; while(ToCnt) { if (REG32b(pI2C, SOC_REG_I2C_MI2_STAT) == Stat) break; ToCnt--; }}

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/

// Partitioning of the CRT and NRT memory for SOC1, all values in percent
#define CP_MEM_SOC1_PERCENT_CRT                     4
#define CP_MEM_SOC1_PERCENT_NRT                     96

#define CP_MEM_SOC1_PERCENT_ALL (CP_MEM_SOC1_PERCENT_CRT + CP_MEM_SOC1_PERCENT_NRT)

#if (CP_MEM_SOC1_PERCENT_ALL != 100)
#error "Sum has to be 100 percent!"
#endif

// Partitioning of the CRT memory for SOC1, all values in percent
#define CP_MEM_SOC1_PERCENT_CRT_SLOW                100

#define CP_MEM_SOC1_PERCENT_CRT_ALL (CP_MEM_SOC1_PERCENT_CRT_SLOW)

#if (CP_MEM_SOC1_PERCENT_CRT_ALL != 100)
#error "Sum has to be 100 percent!"
#endif

// Partitioning of the NRT memory for SOC1, all values in percent
#define CP_MEM_SOC1_PERCENT_NRT_TX                  24
#define CP_MEM_SOC1_PERCENT_NRT_RX                  35
#define CP_MEM_SOC1_PERCENT_NRT_DMACW_DEFAULT       4           //global dmacw pool
#define CP_MEM_SOC1_PERCENT_NRT_TX_LOW_FRAG         11          //size for nrt tx pool, lowprio fragments
#define CP_MEM_SOC1_PERCENT_NRT_TX_MID_FRAG         11          //size for nrt tx pool, midprio fragments
#define CP_MEM_SOC1_PERCENT_NRT_TX_HIGH_FRAG        11          //size for nrt tx pool, highprio fragments
#define CP_MEM_SOC1_PERCENT_NRT_TX_MGMTLOW_FRAG     2           //size for nrt tx pool, mgmtlowprio fragments
#define CP_MEM_SOC1_PERCENT_NRT_TX_MGMTHIGH_FRAG    2           //size for nrt tx pool, mgmthighprio fragments

#define CP_MEM_SOC1_PERCENT_NRT_ALL (  CP_MEM_SOC1_PERCENT_NRT_TX + CP_MEM_SOC1_PERCENT_NRT_RX \
                                     + CP_MEM_SOC1_PERCENT_NRT_DMACW_DEFAULT                                                                            \
                                     + CP_MEM_SOC1_PERCENT_NRT_TX_LOW_FRAG + CP_MEM_SOC1_PERCENT_NRT_TX_MID_FRAG + CP_MEM_SOC1_PERCENT_NRT_TX_HIGH_FRAG \
                                     + CP_MEM_SOC1_PERCENT_NRT_TX_MGMTLOW_FRAG + CP_MEM_SOC1_PERCENT_NRT_TX_MGMTHIGH_FRAG)

#if (CP_MEM_SOC1_PERCENT_NRT_ALL != 100)
#error "Sum has to be 100 percent!"
#endif

/*----------------------------------------------------------------------------*/
/*  Types                                                                     */
/*----------------------------------------------------------------------------*/

typedef struct eps_soc_hw_store_tag
{
	LSA_BOOL   init_done;                // Entry initialized yes/no
	LSA_UINT16 hw_enter_exit_id;         // Lock for LED of POF GPIO access
	LSA_BOOL   i2c_init_done;            // I2C init done
} EPS_SOC_HW_STORE_TYPE;

/*----------------------------------------------------------------------------*/
/*  Prototypes                                                                */
/*----------------------------------------------------------------------------*/

static LSA_VOID eps_get_gpio_addr( LSA_UINT32 Gpio_nr, LSA_UINT8 * pGpioBase, LSA_UINT32 * * ppGpioPortMode, LSA_UINT32 * * ppGpioCtrl, LSA_UINT32 * * ppGpioOut );
static LSA_VOID eps_i2c_undo_init_SOC( LSA_UINT16 boardNr, LSA_UINT32 pI2C );

/* external blink functions for NSC PhyTranceivers*/
static LSA_VOID eps_phy_led_blink_begin_SOC_NSC( EDDI_SYS_HANDLE hSysDev, LSA_UINT32 HwPortIndex);
static LSA_VOID eps_phy_led_blink_end_SOC_NSC( EDDI_SYS_HANDLE hSysDev, LSA_UINT32 HwPortIndex);
static LSA_VOID eps_phy_led_blink_set_mode_SOC_NSC( EDDI_SYS_HANDLE hSysDev, LSA_UINT32 HwPortIndex, LSA_UINT16 LEDMode);

/*----------------------------------------------------------------------------*/
/*  Variables                                                                 */
/*----------------------------------------------------------------------------*/

static LSA_BOOL              g_Eps_soc_hw_init_done = LSA_FALSE;
static EPS_SOC_HW_STORE_TYPE g_Eps_soc_hw_store[PSI_CFG_MAX_IF_CNT+1];

/*----------------------------------------------------------------------------*/

static LSA_VOID eps_get_gpio_addr( LSA_UINT32 Gpio_nr, LSA_UINT8 * pGpioBase, LSA_UINT32 * * ppGpioPortMode, LSA_UINT32 * * ppGpioCtrl, LSA_UINT32 * * ppGpioOut )
{
	// set CNTR and OUT RegAddr by GPIO-Nr

	EPS_ASSERT(SOC_GPIO_PORT(Gpio_nr) <= 5);
	EPS_ASSERT(LSA_NULL != pGpioBase);
	EPS_ASSERT(LSA_NULL != ppGpioPortMode);
	EPS_ASSERT(LSA_NULL != ppGpioCtrl);
	EPS_ASSERT(LSA_NULL != ppGpioOut);

	switch (SOC_GPIO_PORT_PORT_MODE(Gpio_nr))
	{
		case 0:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_0_L);
			break;
		case 1:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_0_H);
			break;
		case 2:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_1_L);
			break;
		case 3:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_1_H);
			break;
		case 4:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_2_L);
			break;
		case 5:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_2_H);
			break;
		case 6:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_3_L);
			break;
		case 7:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_3_H);
			break;
		case 8:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_4_L);
			break;
		case 9:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_4_H);
			break;
		case 10:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_5_L);
			break;
		case 11:
			*ppGpioPortMode = (LSA_UINT32 *)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_PORT_MODE_5_H);
			break;
		default:
			*ppGpioPortMode = LSA_NULL;
			break;
	}

	switch (SOC_GPIO_PORT(Gpio_nr))
	{
		case 0:
			*ppGpioCtrl  = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_IOCTRL_0);
			*ppGpioOut   = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_OUT_0);
			break;
		case 1:
			*ppGpioCtrl  = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_IOCTRL_1);
			*ppGpioOut   = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_OUT_1);
			break;
		case 2:
			*ppGpioCtrl  = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_IOCTRL_2);
			*ppGpioOut   = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_OUT_2);
			break;
		case 3:
			*ppGpioCtrl  = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_IOCTRL_3);
			*ppGpioOut   = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_OUT_3);
			break;
		case 4:
			*ppGpioCtrl  = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_IOCTRL_4);
			*ppGpioOut   = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_OUT_4);
			break;
		case 5:
			*ppGpioCtrl  = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_IOCTRL_5);
			*ppGpioOut   = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(pGpioBase + GPIO_OUT_5);
			break;
		default:
			*ppGpioCtrl  = LSA_NULL;
			*ppGpioOut   = LSA_NULL;
			break;
	}
	EPS_ASSERT(LSA_NULL != *ppGpioPortMode);
	EPS_ASSERT(LSA_NULL != *ppGpioCtrl);
	EPS_ASSERT(LSA_NULL != *ppGpioOut);
}

/*----------------------------------------------------------------------------*/
/*  Initialisation                                                            */
/*----------------------------------------------------------------------------*/
LSA_VOID eps_init_hw_SOC( LSA_UINT16 boardNr )
{	
	LSA_RESPONSE_TYPE rsp;

	if ( !g_Eps_soc_hw_init_done )  // Basic init of global vars done ?
	{
		LSA_UINT16 idx;

		for ( idx = 0; idx <= PSI_CFG_MAX_IF_CNT; idx++ )
		{
		    g_Eps_soc_hw_store[idx].hw_enter_exit_id = EPS_LOCK_ID_INVALID;
		    g_Eps_soc_hw_store[idx].i2c_init_done    = LSA_FALSE;
		    g_Eps_soc_hw_store[idx].init_done        = LSA_FALSE;
		}

		g_Eps_soc_hw_init_done = LSA_TRUE;
	}

	EPS_ASSERT( !g_Eps_soc_hw_store[boardNr].init_done );

	g_Eps_soc_hw_store[boardNr].hw_enter_exit_id = EPS_LOCK_ID_INVALID;
	g_Eps_soc_hw_store[boardNr].i2c_init_done    = LSA_FALSE;
		
	rsp = eps_alloc_critical_section( &g_Eps_soc_hw_store[boardNr].hw_enter_exit_id, LSA_FALSE);
	EPS_ASSERT( rsp == LSA_RET_OK );

	g_Eps_soc_hw_store[boardNr].init_done = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_undo_init_hw_SOC( LSA_UINT16 boardNr )
{
	LSA_RESPONSE_TYPE rsp;

	EPS_ASSERT( g_Eps_soc_hw_store[boardNr].init_done );

	if ( g_Eps_soc_hw_store[boardNr].i2c_init_done ) // reset I2C ?
	{
		LSA_UINT32 pI2C;

		EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store( boardNr );
		EPS_ASSERT( pBoard != 0 );
		EPS_ASSERT( pBoard->eddi.is_valid != 0 );

		// get base pointer to I2C
		EPS_ASSERT( pBoard->eddi.i2c_base_ptr != 0 ); 
		pI2C = (LSA_UINT32)pBoard->eddi.i2c_base_ptr;

		// set I2c to intial state
		eps_i2c_undo_init_SOC( boardNr, pI2C );
	}

	rsp = eps_free_critical_section( g_Eps_soc_hw_store[boardNr].hw_enter_exit_id );
	EPS_ASSERT( rsp == LSA_RET_OK );

	g_Eps_soc_hw_store[boardNr].hw_enter_exit_id = EPS_LOCK_ID_INVALID;
	g_Eps_soc_hw_store[boardNr].i2c_init_done    = LSA_FALSE;

	g_Eps_soc_hw_store[boardNr].init_done = LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
/*  PLL setting                                                               */
/*----------------------------------------------------------------------------*/

LSA_RESULT eps_set_pllport_SOC( LSA_UINT16 board_nr,
								LSA_UINT32 irte_ptr,
						        LSA_UINT32 location,
								LSA_UINT16 pllmode,
						        LSA_UINT16 gpio )
{
	// Note: The baseptrs to the IOCC/GPIO-Regs cannot be calculated directly from pIRTE
	//       because GPIO and IOCC are accessed through different BARs on SOC2,
	//       and through the same BAR on SOC1

	LSA_UINT32 pIOCC;
	LSA_UINT32 pGPIO;
	LSA_UINT32 pIRTE;

	LSA_RESULT retval = EDD_STS_OK;

	EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store( board_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr != 0 );

	pGPIO = (LSA_UINT32)(pBoard->eddi.apb_periph_gpio.base_ptr);
	pIRTE = (LSA_UINT32)pBoard->eddi.irte.base_ptr;
	pIOCC = (LSA_UINT32)pBoard->eddi.iocc.base_ptr;

	EPS_ASSERT( pGPIO != 0 );
	EPS_ASSERT( pIRTE != 0 );
	EPS_ASSERT( pIOCC != 0 );

	// Check the baseptrs for the call
	if (( irte_ptr      != (LSA_UINT32)pIRTE ) || // not matches
		( EDDI_LOC_SOC1 != location ))            // SOC 1 board ?
	{
		return EDD_STS_ERR_PARAM;
	}

	EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_set_pllport_SOC(): board_nr(%u) irte_ptr(0x%08X) location(%u) pllmode(%u) gpio(%u) cycle_base_factor(%u) ",
		board_nr, irte_ptr, location, pllmode, gpio,  pBoard->eddi.cycle_base_factor);

	eps_enter_critical_section( g_Eps_soc_hw_store[board_nr].hw_enter_exit_id );

	switch( pllmode ) // mode
	{
	    case EDDI_PRM_DEF_PLL_MODE_OFF: //switch output off 
		{
			if (( gpio >= 152 ) && ( gpio <= 159 ))
			{
				//switch port back to original I/O;
				REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) | (1UL<<(gpio-128));                       //input
				REG32b(pGPIO, GPIO_PORT_MODE_4_H) = (REG32b(pGPIO, GPIO_PORT_MODE_4_H) & (~(3UL<<((gpio-144)*2))));    // set to altfct 0
				REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
					( REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL)); //clear MUX-Selector (First 4 Bits)
			}
			else if (( gpio >= 168 ) && ( gpio <= 175 ))
			{
				//switch port back to original I/O;
				REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) | (1UL<<(gpio-160));                       //input
				REG32b(pGPIO, GPIO_PORT_MODE_5_L) = (REG32b(pGPIO, GPIO_PORT_MODE_5_L) & (~(3UL<<((gpio-160)*2))));    // set to altfct 0
				REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
					(REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL)); //clear MUX-Selector (First 4 Bits)
			}
			else
			{
				retval = EDD_STS_ERR_PARAM;
			}
		}
		break;
	    case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT: //set PLL-Output
		{
			if (( gpio>=152 ) && ( gpio<=159 ))
			{
				REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio-152)) = 0x0BUL;                              //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> ISO_OUT_n):
				REG32b(pGPIO, GPIO_IOCTRL_4) = REG32b(pGPIO, GPIO_IOCTRL_4) & (~(1UL<<(gpio-128)));                    //output
				REG32b(pGPIO, GPIO_PORT_MODE_4_H) = REG32b(pGPIO, GPIO_PORT_MODE_4_H) | (3UL<<((gpio-144)*2));         // set to altfct 3
			}
			else if (( gpio>=168 ) && ( gpio<=175 ))
			{
				REG32b(pIOCC, U_IOCC_HOST_LL_inst_iso_out_0_cntrl+4*(gpio-168)) = 0x0BUL;                              //set port-mux in IOCC (IRTE_PLL_OUT_O(0) -> ISO_OUT_n):
				REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) & (~(1UL<<(gpio-160)));                    //output
				REG32b(pGPIO, GPIO_PORT_MODE_5_L) = REG32b(pGPIO, GPIO_PORT_MODE_5_L) | (3UL<<((gpio-160)*2));         // set to altfct 3
			}
			else
			{
				retval = EDD_STS_ERR_PARAM;
			}

			#if( (defined (PSI_CFG_EDDI_CFG_APPLSYNC_SHARED)) || (defined (PSI_CFG_EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)) || (defined (PSI_CFG_EDDI_CFG_APPLSYNC_SEPARATE)))
            if (pBoard->eddi.cycle_base_factor <= 16)
            {
                REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
					(( REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL)) | 0x5UL);       // set IRTE-mux (First 4 Bits to XPLL_EXT_OUT on Port 0)
            }
            else
            #endif
            {
                REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) = 
					(( REG32b(pIRTE, U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR) & ~(0xFUL)) | 0x7UL);       // set IRTE-mux (First 4 Bits to XPLL_EXT_OUT on Port 0)
            }


		}
		break;
	    case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN: //set PLL-Input
		{
			switch (gpio)
			{
			    case 94:
			    case 95:
				{
					if (94 == gpio)
					{
						REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x01UL;                                   //set port-mux in IOCC (DSA_PLL_EXT_IN --> IRTE_PLL_IN)
					}
					else
					{
						REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x00UL;                                   //set port-mux in IOCC (IRTE_PLL_EXT_IN0 --> IRTE_PLL_IN)
					}

					REG32b(pGPIO, GPIO_IOCTRL_2) = REG32b(pGPIO, GPIO_IOCTRL_2) | (1UL<<(gpio-64));                      //input
					REG32b(pGPIO, GPIO_PORT_MODE_2_H) = (REG32b(pGPIO, GPIO_PORT_MODE_2_H) & (~(3UL<<((gpio-80)*2))));   // set to altfct 0
				}
				break;

			    case 127:
				{
					REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x02UL;                                        //set port-mux in IOCC (PB_PLL_EXT_IN --> IRTE_PLL_IN)
					REG32b(pGPIO, GPIO_IOCTRL_3) = REG32b(pGPIO, GPIO_IOCTRL_3) | (1UL<<(gpio-96));                       //input
					REG32b(pGPIO, GPIO_PORT_MODE_3_H) = (REG32b(pGPIO, GPIO_PORT_MODE_3_H) & (~(3UL<<((gpio-112)*2))));   // set to altfct 0
				}
				break;

			    case 178:
				{
					REG32b(pIOCC, U_IOCC_HOST_LL_inst_irte_pll_in_cntrl) = 0x03UL;                                        //set port-mux in IOCC (IRTE_PLL_EXT_IN1 --> IRTE_PLL_IN)
					REG32b(pGPIO, GPIO_IOCTRL_5) = REG32b(pGPIO, GPIO_IOCTRL_5) | (1UL<<(gpio-160));                      //input
					REG32b(pGPIO, GPIO_PORT_MODE_5_H) = (REG32b(pGPIO, GPIO_PORT_MODE_5_H) & (~(3UL<<((gpio-176)*2))));   // set to altfct 0
				}
				break;

			    default:
				{
					retval = EDD_STS_ERR_PARAM;
				}
			} //end switch(gpio)
		}
		break;
	    default:
		{
			retval = EDD_STS_ERR_PARAM;
		}

	} // end switch(pllmode)

	eps_exit_critical_section( g_Eps_soc_hw_store[board_nr].hw_enter_exit_id );

	return retval;
}

/*----------------------------------------------------------------------------*/
/*  POF LED settings                                                          */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_pof_set_led_SOC( LSA_UINT16 portID, LSA_BOOL on, LSA_SYS_PTR_TYPE sysPtr )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE         pSys;
	EPS_ASSERT( sysPtr != 0 );

	pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );
	EPS_ASSERT(( portID >= 1 ) && ( portID <= pBoard->nr_of_ports ));

	LSA_UNUSED_ARG( on );

    #ifdef EPS_MESSAGE
    #pragma EPS_MESSAGE("TODO: Add SOC specific POF LED setting here")
    #endif
}

/*----------------------------------------------------------------------------*/
/*  I2C adaption                                                              */
/*----------------------------------------------------------------------------*/

static LSA_BOOL eps_i2c_fixup_bus_SOC( LSA_UINT16 boardNr, LSA_UINT32 pI2C )
{
	// Note: Toggle the SCL line until SDA read HighZ
	LSA_UINT32 Reg, Bus, Timeout;

	Timeout = 100;
	Bus     = (REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) & EPS_SOC_I2C_BUS_HIGHZ);
	Reg     = REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1);

    LSA_UNUSED_ARG(boardNr);

	while (Timeout && (Bus != EPS_SOC_I2C_BUS_HIGHZ))
	{
		// ***** Toggle SCL if HW state machine hangs until bus is HighZ
		Reg ^= SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE;
		REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1) = Reg;

		eps_tasks_usleep(2);

		Bus = (REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) & EPS_SOC_I2C_BUS_HIGHZ);

		if (Reg & SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE)
		{   //toggle until Bus == FF and Reg without SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE bit !!!
			Bus = EPS_SOC_I2C_BUS_HIGHZ-1;
		}

		Timeout--;
	}

	if (Bus == EPS_SOC_I2C_BUS_HIGHZ) //&& !(Reg & SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
static LSA_BOOL eps_i2c_do_reset_SOC( LSA_UINT32 pI2C )
{
	// Resets the MI2C module within the SOC1
	LSA_UINT16 Timeout = 500;

	REG32b( pI2C, SOC_REG_I2C_MI2C_XADDR)          = 0;
	REG32b( pI2C, SOC_REG_I2C_MI2C_SOFTWARE_RESET) = SOC_REG_I2C_MI2C_SOFTWARE_RESET__MSK_SOFTRESET;
	eps_tasks_usleep(2);

	while (Timeout)
	{
		if (REG32b(pI2C, SOC_REG_I2C_MI2C_XADDR))
		{
			return LSA_TRUE;
		}

		Timeout--;
	}

	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
static LSA_RESULT eps_i2c_init_SOC( LSA_UINT16 boardNr, LSA_UINT32 pI2C, LSA_UINT32 pSCRB, LSA_UINT32 pGPIO )
{
	// Initialization of most common I2C registers
	LSA_UINT32 Val;

	// ***** set 100kBit/s for I2C
	REG32b(pSCRB, CCR_I2C_0) = 49UL;
	// ***** enabled with 0 !!!!
	REG32b(pI2C, SOC_REG_I2C_SW_I2C_EN) = (0<<SOC_REG_I2C_SW_I2C_EN_SHFT_I2C_PARAMENABLE);
	// ***** set lines to tristate
	REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) = (SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SCL_O | SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SDA_O);
	// ***** Mode=per action, Disable Wr/Rd
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1) = 0;
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_2) = 0;
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_3) = 0;
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_4) = 0;

	Val = (REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) & EPS_SOC_I2C_BUS_HIGHZ);

	if ( Val != EPS_SOC_I2C_BUS_HIGHZ )
	{
		if (!eps_i2c_fixup_bus_SOC( boardNr, pI2C ))
		{
			return EDD_STS_ERR_HW;
		}
	}

	// ***** reset mi2c module
	REG32b(pI2C, SOC_REG_I2C_MI2C_SOFTWARE_RESET) = SOC_REG_I2C_MI2C_SOFTWARE_RESET__MSK_SOFTRESET;
	eps_tasks_usleep(2);

	//activate I2C-GPIOs of SOC1 (select alternate function 0)
	REG32b(pGPIO, GPIO_PORT_MODE_5_H) = ((REG32b(pGPIO, GPIO_PORT_MODE_5_H) & (~(3UL<<((EPS_SOC_I2C_GPIO_SCL-176)*2)))) | (1UL<<((EPS_SOC_I2C_GPIO_SCL-176)*2)));
	REG32b(pGPIO, GPIO_PORT_MODE_5_H) = ((REG32b(pGPIO, GPIO_PORT_MODE_5_H) & (~(3UL<<((EPS_SOC_I2C_GPIO_SDA-176)*2)))) | (1UL<<((EPS_SOC_I2C_GPIO_SDA-176)*2)));

	g_Eps_soc_hw_store[boardNr].i2c_init_done = LSA_TRUE;

	return EDD_STS_OK;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID eps_i2c_undo_init_SOC( LSA_UINT16 boardNr, LSA_UINT32 pI2C )
{
	// DeInitialization of most common I2C registers

	// ***** disabled with 1 !!!!
	REG32b(pI2C, SOC_REG_I2C_SW_I2C_EN) = (1<<SOC_REG_I2C_SW_I2C_EN_SHFT_I2C_PARAMENABLE);

	// ***** Mode=per action, Disable Wr/Rd
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1) = 0;
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_2) = 0;
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_3) = 0;
	REG32b(pI2C, SOC_REG_I2C_EX_CTRL_4) = 0;

	g_Eps_soc_hw_store[boardNr].i2c_init_done = LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_scl_low_highz_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	LSA_UNUSED_ARG( sysPtr );
	LSA_UNUSED_ARG( outPut );

	EPS_FATAL("not expected call for a SOC1 board");
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_sda_low_highz_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	LSA_UNUSED_ARG( sysPtr );
	LSA_UNUSED_ARG( outPut );

	EPS_FATAL("not expected call for a SOC1 board");
}

/*----------------------------------------------------------------------------*/
LSA_UINT8 eps_i2c_sda_read_SOC( LSA_SYS_PTR_TYPE sysPtr )
{
	LSA_UNUSED_ARG( sysPtr );

	EPS_FATAL("not expected call for a SOC1 board");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return 0;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_sda_in_out_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	LSA_UNUSED_ARG( sysPtr );
	LSA_UNUSED_ARG( outPut );

	EPS_FATAL("not expected call for a SOC1 board");
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_scl_in_out_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	LSA_UNUSED_ARG( sysPtr );
	LSA_UNUSED_ARG( outPut );

	EPS_FATAL("not expected call for a SOC1 board");
}

/*----------------------------------------------------------------------------*/
LSA_BOOL eps_i2c_select_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT16 PortId, LSA_UINT16 I2CMuxSelect ) // implementation for SOC1 PCIe board
{
	LSA_UINT8  * pGPIO;
	LSA_UINT32 * pGPIO_PortMode;
	LSA_UINT32 * pGPIO_IOCTRL;
	LSA_UINT32 * pGPIO_OUT;
	LSA_UINT32   hw_port_id;

	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( PortId && (PortId <= pBoard->nr_of_ports) );
	EPS_ASSERT( I2CMuxSelect && (I2CMuxSelect <= PSI_CFG_MAX_PORT_CNT) );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	hw_port_id = pBoard->port_map[PortId].hw_port_id;

    //The I2C device must be selected via a HW multiplexer controlled by signals I2C_MUX_S0 and I2C_MUX_S1.
    //
    //  I2C_MUX_S0  I2C_MUX_S1  selection
    //  ---------------------------------------------------
    //  0	        0		    Transceiver Port 1 (HwPortId)
    //  1	        0		    Transceiver Port 2 (HwPortId)
    //  0	        1		    Transceiver Port 3 (HwPortId)
    //  1	        1		    Transceiver Port 4 (HwPortId) (not used on SOC)

	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;

	eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S0, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);

	// set EPS_SOC_GPIO_I2C_MUX_S0 to GPIO Mode and Output
	*pGPIO_PortMode &= ~(SOC_GPIO_MODE_MASK(EPS_SOC_GPIO_I2C_MUX_S0));
	*pGPIO_IOCTRL &= ~(SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S0));

	eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S1, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);

	// set EPS_SOC_GPIO_I2C_MUX_S1 to GPIO Mode and Output
	*pGPIO_PortMode &= ~(SOC_GPIO_MODE_MASK(EPS_SOC_GPIO_I2C_MUX_S1));
	*pGPIO_IOCTRL &= ~(SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S1));

	switch (hw_port_id)
	{
		case 1: //= HwPortId 1
			eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S0, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);
			*pGPIO_OUT &= ~(SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S0));			// I2C_MUX_S0...0
			eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S1, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);
			*pGPIO_OUT &= ~(SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S1));			// I2C_MUX_S1...0
			break;
		case 2: //= HwPortId 2
			eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S0, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);
			*pGPIO_OUT |= (SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S0));			    // I2C_MUX_S0...1
			eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S1, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);
			*pGPIO_OUT &= ~(SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S1));			// I2C_MUX_S1...0
			break;
		case 3: //= HwPortId 3
			eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S0, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);
			*pGPIO_OUT &= ~(SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S0));			// I2C_MUX_S0...0
			eps_get_gpio_addr(EPS_SOC_GPIO_I2C_MUX_S1, pGPIO, &pGPIO_PortMode, &pGPIO_IOCTRL, &pGPIO_OUT);
			*pGPIO_OUT |= (SOC_GPIO_MASK(EPS_SOC_GPIO_I2C_MUX_S1));			    // I2C_MUX_S1...1
			break;
		default: break;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT eps_i2c_ll_write_offset_SOC(
	LSA_SYS_PTR_TYPE   sysPtr,
	LSA_UINT8          I2CDevAddr,
	LSA_UINT8          I2COffsetCnt,
	LSA_UINT8          I2COffset1,
	LSA_UINT8          I2COffset2,
	LSA_UINT32         Size,
	LSA_UINT8        * pBuf )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	LSA_RESULT              Result;
	LSA_UINT32              Len    = Size;
	LSA_UINT8               BufIdx = 0;
	LSA_UINT32              ToCnt;  // Timeout count
	LSA_UINT32              pI2C;   // Baseptr to I2C-Regs
	LSA_UINT32              pSCRB;  // Baseptr to SCRB-Regs
	LSA_UINT32              pGPIO;  // Baseptr to GPIO-Regs

	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );

	pI2C  = (LSA_UINT32)pBoard->eddi.i2c_base_ptr;
	pSCRB = (LSA_UINT32)pBoard->eddi.apb_periph_scrb.base_ptr;
	pGPIO = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	//Check the baseptrs for the call
	if ((pI2C  == 0) || (pSCRB == 0) || (pGPIO == 0)) //open not done?
	{
		return EDD_STS_ERR_PARAM;
	}

	if (!g_Eps_soc_hw_store[pSys->hd_nr].i2c_init_done) //I2C device not yet initialized?
	{
		Result = eps_i2c_init_SOC( pSys->hd_nr, pI2C, pSCRB, pGPIO );
		if ( Result != EDD_STS_OK )
		{
			return EDD_STS_ERR_HW;
		}
	}

    if (Len == 0)
    {
        return EDD_STS_OK;
    }

	// PageWrite: START, I2CDevAddr, ByteAddr, Wr, Wr, ... , STOP

	// ***** send START:  IFLG bit will be set, Code=08h
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_START;             //Start
	EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
	if (ToCnt) //Timeout not yet elapsed?
	{
		EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_START_0);    //START condition transmitted
	}
	if (!ToCnt) //Timeout elapsed?
	{
		// ***** try to restart the HW state machine
		eps_i2c_do_reset_SOC(pI2C);
		return EDD_STS_ERR_HW;
	}

	// ***** send DEV-Address + Ack:  IFLG bit will be set, Code=18h
	REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2CDevAddr;
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;          //DEV
	EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
	if (ToCnt) //Timeout not yet elapsed?
	{
		EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_ADR_WR);     //Address + write bit transmitted, ACK received
	}
	if (!ToCnt) //Timeout elapsed?
	{
	    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;          //Stop
		eps_tasks_usleep(2);
		return EDD_STS_ERR_PROTOCOL;
	}

	eps_tasks_usleep(2);

	switch (I2COffsetCnt) //by I2COffsetCnt (already checked by value)
	{
		case 1:
		{
			// ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
			REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
			REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;              //E2
			EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
			if (ToCnt) //Timeout not yet elapsed?
			{
				EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
			}
			if (!ToCnt) //Timeout elapsed?
			{
	            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
				eps_tasks_usleep(2);
				return EDD_STS_ERR_PROTOCOL;
			}

			eps_tasks_usleep(2);

			break;
		}
		case 2:
		{
			// ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
			REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
			REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;              //E2
			EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
			if (ToCnt) //Timeout not yet elapsed?
			{
				EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
			}
			if (!ToCnt) //Timeout elapsed?
			{
	            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
				eps_tasks_usleep(2);
				return EDD_STS_ERR_PROTOCOL;
			}

			eps_tasks_usleep(2);

			// ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
			REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset2;
			REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;		        //E2
			EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
			if (ToCnt) //Timeout not yet elapsed?
			{
				EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
			}
			if (!ToCnt) //Timeout elapsed?
			{
	            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
				eps_tasks_usleep(2);
				return EDD_STS_ERR_PROTOCOL;
			}

			eps_tasks_usleep(2);

			break;
		}
		default: break;
	}

	while (Len)
	{
		// ***** send DataByte + Ack:  IFLG bit will be set, Code=28h
		REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = pBuf[BufIdx];
		REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;              //D[?]
		EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
		if (ToCnt) //Timeout not yet elapsed?
		{
			EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
		}
		if (!ToCnt) //Timeout elapsed?
		{
	        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
		    eps_tasks_usleep(2);
		    return EDD_STS_ERR_PROTOCOL;
		}

		Len--;
		BufIdx++;

	} //end of while loop

	// ***** send Stop:  IFLG bit will not be set, Code=F8h
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;      //Stop
	eps_tasks_usleep(2);
	EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_IDLE);   //No relevant status information, IFLG=0
	if (!ToCnt) //Timeout elapsed?
	{
		return EDD_STS_ERR_PROTOCOL;
	}

	return EDD_STS_OK;
    //lint --e(818) Pointer parameter 'pBuf' could be declared as pointing to const - we are unable to change the API
}

/*----------------------------------------------------------------------------*/
LSA_RESULT eps_i2c_ll_read_offset_SOC( 
	LSA_SYS_PTR_TYPE   sysPtr,
	LSA_UINT8          I2CDevAddr,
	LSA_UINT8          I2COffsetCnt,
	LSA_UINT8          I2COffset1,
	LSA_UINT8          I2COffset2,
	LSA_UINT32         Size,
	LSA_UINT8        * pBuf )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	LSA_RESULT              Result;
	LSA_UINT32              Cmd;
	LSA_UINT32              State;                    
	LSA_UINT32              Len    = Size;                    
	LSA_UINT8               BufIdx = 0;                    
	LSA_UINT32              ToCnt;  // Timeout count
	LSA_UINT32              pI2C;   // Baseptr to I2C-Regs
	LSA_UINT32              pSCRB;  // Baseptr to SCRB-Regs
	LSA_UINT32              pGPIO;  // Baseptr to GPIO-Regs

	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );

	pI2C  = (LSA_UINT32)pBoard->eddi.i2c_base_ptr;
	pSCRB = (LSA_UINT32)pBoard->eddi.apb_periph_scrb.base_ptr;
	pGPIO = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	//Check the baseptrs for the call
	if ((pI2C == 0) || (pSCRB == 0) || (pGPIO == 0)) //open not done?
	{
		return EDD_STS_ERR_PARAM;
	}

	if (!g_Eps_soc_hw_store[pSys->hd_nr].i2c_init_done) //I2C device not yet initialized?
	{
		Result = eps_i2c_init_SOC( pSys->hd_nr, pI2C, pSCRB, pGPIO );
		if ( Result != EDD_STS_OK )
		{
			return EDD_STS_ERR_HW;
		}
	}

    if (Len == 0)
    {
        return EDD_STS_OK;
    }

	//PageRead: START, I2CDevAddr, ByteAddr, Rd, Rd, ... , [STOP]

	// ***** send START:  IFLG bit will be set, Code=08h
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_START;             //Start
	EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
	if (ToCnt) //Timeout not yet elapsed?
	{
		EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_START_0);    //START condition transmitted
	}
	if (!ToCnt) //Timeout elapsed?
	{
        LSA_BOOL bResult;
		// ***** try to restart the HW state machine
        bResult= eps_i2c_do_reset_SOC(pI2C);
        LSA_UNUSED_ARG(bResult);
		return EDD_STS_ERR_HW;
	}

	eps_tasks_usleep(2);

	// ***** send DEV-Address + Ack:  IFLG bit will be set, Code=18h
	REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = (I2CDevAddr & (LSA_UINT8)0xFE);
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;          //DEV
	EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
	if (ToCnt) //Timeout not yet elapsed?
	{
		EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_ADR_WR);     //Address + write bit transmitted, ACK received
	}
	if (!ToCnt) //Timeout elapsed?
	{
	    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;          //Stop
		eps_tasks_usleep(2);
		return EDD_STS_ERR_PROTOCOL;
	}

	eps_tasks_usleep(2);

	switch (I2COffsetCnt) //by I2COffsetCnt (already checked by value)
	{
		case 1:
		{
			// ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
			REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
			REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;              //E2
			EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
			if (ToCnt) //Timeout not yet elapsed?
			{
				EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
			}
			if (!ToCnt) //Timeout elapsed?
			{
	            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
				eps_tasks_usleep(2);
				return EDD_STS_ERR_PROTOCOL;
			}

			eps_tasks_usleep(2);

			break;
		}
		case 2:
		{
			// ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
			REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
			REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;              //E2
			EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
			if (ToCnt) //Timeout not yet elapsed?
			{
				EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
			}
			if (!ToCnt) //Timeout elapsed?
			{
	            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
				eps_tasks_usleep(2);
				return EDD_STS_ERR_PROTOCOL;
			}

			eps_tasks_usleep(2);

			// ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
			REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset2;
			REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_WR_DATA;              //E2
			EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
			if (ToCnt) //Timeout not yet elapsed?
			{
				EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_WR_DATA_ACK);    //Data byte transmitted in master mode, ACK received
			}
			if (!ToCnt) //Timeout elapsed?
			{
	            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;              //Stop
				eps_tasks_usleep(2);
				return EDD_STS_ERR_PROTOCOL;
			}

			eps_tasks_usleep(2);

			break;
		}
		default: break;
	}

	// ***** send next START:  IFLG bit will be set, Code=10h
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_START;             //Start 
	EPS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
	if (ToCnt) //Timeout not yet elapsed?
	{
		EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_START_N);    //Repeated START condition transmitted
	}
	if (!ToCnt) //Timeout elapsed?
	{
        REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = 0xFF;
	    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_CMD_STOP;         //Stop
		eps_tasks_usleep(2);
		return EDD_STS_ERR_PROTOCOL;
	}

	eps_tasks_usleep(2);

	// ***** send DEV-Address + Ack:  IFLG bit will be set, Code=40h
	REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = (I2CDevAddr | 1);
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EPS_SOC_I2C_STATE_ADR_RD;         //DEV
	EPS_SOC_I2C_WAIT_IFLG( pI2C, ToCnt );
	if (ToCnt) //Timeout not yet elapsed?
	{
		EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_ADR_RD);     //Address + read bit transmitted, ACK received
	}
	if (!ToCnt) //Timeout elapsed?
	{
	    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;          //Stop
		eps_tasks_usleep(2);
		return EDD_STS_ERR_PROTOCOL;
	}

	eps_tasks_usleep(2);

	while (Len)
	{
		if (Len == 1UL)
		{   // ***** last byte
			Cmd   = EPS_SOC_I2C_CMD_RD_DATA_NAK;            //send Dummy + Send Nak: IFLG bit will be set, Code=50h
			State = EPS_SOC_I2C_STATE_RD_DATA_NAK;          //Data byte received in master mode, NAK transmitted
		}
		else
		{   // ***** pending byte exists
			Cmd   = EPS_SOC_I2C_CMD_RD_DATA_ACK;            //send Dummy + Send Ack: IFLG bit will be set, Code=50h
			State = EPS_SOC_I2C_STATE_RD_DATA_ACK;          //Data byte received in master mode, ACK transmitted
		}

		//send Dummy + Send Nak/Ack:  IFLG bit will be set, Code=50h/58h
		REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = 0xFF;
		REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = Cmd;          //D[?]
		EPS_SOC_I2C_WAIT_IFLG( pI2C, ToCnt );
		pBuf[BufIdx] = (LSA_UINT8)REG32b(pI2C, SOC_REG_I2C_MI2C_DATA);
		if (ToCnt) //Timeout not yet elapsed?
		{
			EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, State);    //Data byte received in master mode, NAK/ACK transmitted
		}
		if (!ToCnt) //Timeout elapsed?
		{
	        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;  //Stop
			eps_tasks_usleep(2);
			return EDD_STS_ERR_PROTOCOL;
		}

		Len--;
		BufIdx++;

	} //end of while loop

	// ***** send Stop:  IFLG bit will not be set, Code=F8h
	REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EPS_SOC_I2C_CMD_STOP;      //Stop
	eps_tasks_usleep(2);
	EPS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, EPS_SOC_I2C_STATE_IDLE);   //No relevant status information, IFLG=0
	if (!ToCnt) //Timeout elapsed?
	{
		return EDD_STS_ERR_PROTOCOL;
	}

	return EDD_STS_OK;
}
/*----------------------------------------------------------------------------*/
LSA_VOID eps_soc_FillCrtMemParams( EPS_BOARD_INFO_TYPE * const pBoardInfo, const EPS_BOARD_MEM_TYPE * const host_shared_ram)
{
    LSA_UINT32 const crt_mem_size_all = EPS_CALC_MEM_SIZE(pBoardInfo->eddi.sdram.size, CP_MEM_SOC1_PERCENT_CRT);     // nrt memory on the soc1 device

    // setup CyclicRealTime memory pools
    #if defined EPS_CFG_USE_IO_HOSTMEM
    // use CRT mem pools on host shared ram. May be LSA_NULL and "0", if hostram is not available.
    pBoardInfo->crt_slow_mem.base_ptr    = host_shared_ram->base_ptr;
    pBoardInfo->crt_slow_mem.phy_addr    = host_shared_ram->phy_addr;
    pBoardInfo->crt_slow_mem.size        = EPS_CALC_MEM_SIZE(crt_mem_size_all, CP_MEM_SOC1_PERCENT_CRT_SLOW);
    #else
    LSA_UNUSED_ARG(host_shared_ram);
    // use CRT mem pools on board sd ram
    pBoardInfo->crt_slow_mem.base_ptr    = pBoardInfo->eddi.sdram.base_ptr;
    pBoardInfo->crt_slow_mem.phy_addr    = pBoardInfo->eddi.sdram.phy_addr;
    pBoardInfo->crt_slow_mem.size        = EPS_CALC_MEM_SIZE(crt_mem_size_all, CP_MEM_SOC1_PERCENT_CRT_SLOW);
    #endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_soc_FillNrtMemParams( LSA_VOID * const pBoardInfoVoid )
{
    LSA_UINT32                  pre_mem_size;
    EPS_BOARD_INFO_TYPE * const pboardInfo       = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const nrt_mem_size_all = EPS_CALC_MEM_SIZE(pboardInfo->eddi.sdram.size, CP_MEM_SOC1_PERCENT_NRT);     // nrt memory on the soc1 device

    #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
    EPS_ASSERT(nrt_mem_size_all == EPS_CALC_MEM_SIZE(pboardInfo->eddi.sdram_cached.size, CP_MEM_SOC1_PERCENT_NRT));
    #endif

    // nrt memory on the soc1 device begins after crt memory section
    pre_mem_size = EPS_CALC_MEM_SIZE(pboardInfo->eddi.sdram.size, CP_MEM_SOC1_PERCENT_CRT);

    #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
    // use cached NRT memory pools
    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->eddi.sdram_cached.base_ptr + pre_mem_size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->eddi.sdram_cached.phy_addr + pre_mem_size;
    #else
    // use uncached NRT memory pools
    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->eddi.sdram.base_ptr + pre_mem_size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->eddi.sdram.phy_addr + pre_mem_size;
    #endif // PSI_CFG_USE_NRT_CACHE_SYNC
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_RX);

    #ifdef EPS_MESSAGE
    #pragma EPS_MESSAGE("@@@@ TODO remove reserved space for IOCC MailBoX if integrated in IOH")
    #endif

    pboardInfo->eddi.fragmentation_supported              = LSA_TRUE;

    // always uncached memory has to be used for the memory pool nrt_mem_dmacw_default
    pre_mem_size += (pboardInfo->nrt_tx_mem.size + pboardInfo->nrt_rx_mem.size);
    pboardInfo->eddi.nrt_mem_dmacw_default.base_ptr       = pboardInfo->eddi.sdram.base_ptr + pre_mem_size;
    pboardInfo->eddi.nrt_mem_dmacw_default.phy_addr       = pboardInfo->eddi.sdram.phy_addr + pre_mem_size;
    pboardInfo->eddi.nrt_mem_dmacw_default.size           = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_DMACW_DEFAULT);
    pboardInfo->eddi.user_mem_fcts_dmacw_default.Alloc    = eps_cp_mem_eddi_nrt_dmacw_default_alloc;
    pboardInfo->eddi.user_mem_fcts_dmacw_default.Free     = eps_cp_mem_eddi_nrt_dmacw_default_free;

    pboardInfo->eddi.user_mem_fcts_rx_default.Alloc       = eps_cp_mem_eddi_nrt_rx_alloc;
    pboardInfo->eddi.user_mem_fcts_rx_default.Free        = eps_cp_mem_eddi_nrt_rx_free;

    pboardInfo->eddi.user_mem_fcts_tx_default.Alloc       = eps_cp_mem_eddi_nrt_tx_alloc;
    pboardInfo->eddi.user_mem_fcts_tx_default.Free        = eps_cp_mem_eddi_nrt_tx_free;

    // use cached or uncached memory for all following NRT memory pools

    pre_mem_size += pboardInfo->eddi.nrt_mem_dmacw_default.size;
    #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
    // use cached NRT memory
    pboardInfo->eddi.nrt_mem_tx_low_frag.base_ptr         = pboardInfo->eddi.sdram_cached.base_ptr + pre_mem_size;
    pboardInfo->eddi.nrt_mem_tx_low_frag.phy_addr         = pboardInfo->eddi.sdram_cached.phy_addr + pre_mem_size;
    #else
    // use uncached NRT memory
    pboardInfo->eddi.nrt_mem_tx_low_frag.base_ptr         = pboardInfo->eddi.sdram.base_ptr + pre_mem_size;
    pboardInfo->eddi.nrt_mem_tx_low_frag.phy_addr         = pboardInfo->eddi.sdram.phy_addr + pre_mem_size;
    #endif // PSI_CFG_USE_NRT_CACHE_SYNC
    pboardInfo->eddi.nrt_mem_tx_low_frag.size             = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_TX_LOW_FRAG);
    pboardInfo->eddi.user_mem_fcts_tx_low_frag.Alloc      = eps_cp_mem_eddi_nrt_tx_low_frag_alloc;
    pboardInfo->eddi.user_mem_fcts_tx_low_frag.Free       = eps_cp_mem_eddi_nrt_tx_low_frag_free;

    pboardInfo->eddi.nrt_mem_tx_mid_frag.base_ptr         = pboardInfo->eddi.nrt_mem_tx_low_frag.base_ptr + pboardInfo->eddi.nrt_mem_tx_low_frag.size;
    pboardInfo->eddi.nrt_mem_tx_mid_frag.phy_addr         = pboardInfo->eddi.nrt_mem_tx_low_frag.phy_addr + pboardInfo->eddi.nrt_mem_tx_low_frag.size;
    pboardInfo->eddi.nrt_mem_tx_mid_frag.size             = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_TX_MID_FRAG);
    pboardInfo->eddi.user_mem_fcts_tx_mid_frag.Alloc      = eps_cp_mem_eddi_nrt_tx_mid_frag_alloc;
    pboardInfo->eddi.user_mem_fcts_tx_mid_frag.Free       = eps_cp_mem_eddi_nrt_tx_mid_frag_free;

    pboardInfo->eddi.nrt_mem_tx_high_frag.base_ptr        = pboardInfo->eddi.nrt_mem_tx_mid_frag.base_ptr + pboardInfo->eddi.nrt_mem_tx_mid_frag.size;
    pboardInfo->eddi.nrt_mem_tx_high_frag.phy_addr        = pboardInfo->eddi.nrt_mem_tx_mid_frag.phy_addr + pboardInfo->eddi.nrt_mem_tx_mid_frag.size;
    pboardInfo->eddi.nrt_mem_tx_high_frag.size            = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_TX_HIGH_FRAG);
    pboardInfo->eddi.user_mem_fcts_tx_high_frag.Alloc     = eps_cp_mem_eddi_nrt_tx_high_frag_alloc;
    pboardInfo->eddi.user_mem_fcts_tx_high_frag.Free      = eps_cp_mem_eddi_nrt_tx_high_frag_free;

    pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.base_ptr     = pboardInfo->eddi.nrt_mem_tx_high_frag.base_ptr + pboardInfo->eddi.nrt_mem_tx_high_frag.size;
    pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.phy_addr     = pboardInfo->eddi.nrt_mem_tx_high_frag.phy_addr + pboardInfo->eddi.nrt_mem_tx_high_frag.size;
    pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.size         = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_TX_MGMTLOW_FRAG);
    pboardInfo->eddi.user_mem_fcts_tx_mgmtlow_frag.Alloc  = eps_cp_mem_eddi_nrt_tx_mgmtlow_frag_alloc;
    pboardInfo->eddi.user_mem_fcts_tx_mgmtlow_frag.Free   = eps_cp_mem_eddi_nrt_tx_mgmtlow_frag_free;

    pboardInfo->eddi.nrt_mem_tx_mgmthigh_frag.base_ptr    = pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.base_ptr + pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.size;
    pboardInfo->eddi.nrt_mem_tx_mgmthigh_frag.phy_addr    = pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.phy_addr + pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.size;
    pboardInfo->eddi.nrt_mem_tx_mgmthigh_frag.size        = EPS_CALC_MEM_SIZE(nrt_mem_size_all, CP_MEM_SOC1_PERCENT_NRT_TX_MGMTHIGH_FRAG);
    pboardInfo->eddi.user_mem_fcts_tx_mgmthigh_frag.Alloc = eps_cp_mem_eddi_nrt_tx_mgmthigh_frag_alloc;
    pboardInfo->eddi.user_mem_fcts_tx_mgmthigh_frag.Free  = eps_cp_mem_eddi_nrt_tx_mgmthigh_frag_free;
}

/**
* wrapper for usage of external SOC1 LED blink begin functions
*
* This function is stored in PSI and called in PSI_EDDI_LL_LED_BLINK_BEGIN.
* When EPS_CFG_USE_EXTERNAL_LED is set, the APPlication Function is called.
* Otherwise  the eps_phy_led_blink_begin_SOC function is called.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex				Hardware Port index that shall blink
* @param    [in] PhyTransceiver				Type of PhyTransceiver
*/
LSA_VOID EPS_SOC1_LED_BLINK_BEGIN(
    LSA_SYS_PTR_TYPE    hSysDev,
    LSA_UINT32          HwPortIndex,
    LSA_UINT32          PhyTransceiver
)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_SOC1_LED_BLINK_BEGIN(): call EPS_APP_LL_LED_BLINK_BEGIN for HWPortIndex(%u)", HwPortIndex);
    EPS_APP_LL_LED_BLINK_BEGIN(hSysDev, HwPortIndex, PhyTransceiver);
#else
    #if(EPS_CFG_USE_EDDI == 1)
    eps_phy_led_blink_begin_SOC(hSysDev, HwPortIndex, PhyTransceiver);
    #else
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortIndex);
    LSA_UNUSED_ARG(PhyTransceiver);
    #endif //(EPS_CFG_USE_EDDI == 1)
#endif
}

/**
* wrapper for usage of external SOC1 LED blink end functions
*
* This function is stored in PSI and called in PSI_EDDI_LL_LED_BLINK_END.
* When EPS_CFG_USE_EXTERNAL_LED is set, the APPlication Function is called.
* Otherwise the eps_phy_led_blink_end_SOC function is called.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex				Hardware Port index that shall blink
* @param    [in] PhyTransceiver				Type of PhyTransceiver
*/
LSA_VOID EPS_SOC1_LED_BLINK_END(
    LSA_SYS_PTR_TYPE    hSysDev,
    LSA_UINT32          HwPortIndex,
    LSA_UINT32          PhyTransceiver)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_SOC1_LED_BLINK_END(): call EPS_APP_LL_LED_BLINK_END for HWPortIndex(%u)", HwPortIndex);
    EPS_APP_LL_LED_BLINK_END(hSysDev, HwPortIndex, PhyTransceiver);
#else
    #if(EPS_CFG_USE_EDDI == 1)
    eps_phy_led_blink_end_SOC(hSysDev, HwPortIndex, PhyTransceiver);
    #else
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortIndex);
    LSA_UNUSED_ARG(PhyTransceiver);
    #endif //(EPS_CFG_USE_EDDI == 1)
#endif
}

/**
* wrapper for usage of external SOC1 LED blink set mode functions
*
* This function is stored in PSI and called in PSI_EDDI_LL_LED_BLINK_SET_MODE.
* When EPS_CFG_USE_EXTERNAL_LED is set, the APPlication Function is called.
* Otherwise the eps_phy_led_blink_set_mode_SOC function is called.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
* @param    [in] PhyTransceiver             Type of PhyTransceiver
* @param    [in] mode                       LEDmode
*/
LSA_VOID EPS_SOC1_LED_BLINK_SET_MODE(
    LSA_SYS_PTR_TYPE    hSysDev,
    LSA_UINT32          HwPortIndex,
    LSA_UINT32          PhyTransceiver,
    LSA_UINT16          mode)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_SOC1_LED_BLINK_SET_MODE(): call EPS_APP_LL_LED_BLINK_SET_MODE for HWPortIndex(%u)", HwPortIndex);
    EPS_APP_LL_LED_BLINK_SET_MODE(hSysDev, HwPortIndex, PhyTransceiver, mode);
#else
    #if(EPS_CFG_USE_EDDI == 1)
    eps_phy_led_blink_set_mode_SOC(hSysDev, HwPortIndex, PhyTransceiver, mode);
    #else
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortIndex);
    LSA_UNUSED_ARG(PhyTransceiver);
    LSA_UNUSED_ARG(mode);
    #endif //(EPS_CFG_USE_EDDI == 1)
#endif
}

#if ( EPS_CFG_USE_EDDI == 1 )
/**
* eps function to begin blink on SOC boards
*
* When the external blink functions are used, this function starts blinking SOC
* PhyTransceivers on board. Depending on the Phy Type, the phy functions are called.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
* @param    [in] PhyTransceiver             Type of PhyTransceiver
*/
LSA_VOID eps_phy_led_blink_begin_SOC(
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      HwPortIndex,
    LSA_UINT32      PhyTransceiver)
{
    switch (PhyTransceiver)
    {
        case EDDI_PHY_TRANSCEIVER_NSC:
            eps_phy_led_blink_begin_SOC_NSC(hSysDev, HwPortIndex);
            break;
        default:
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,
                "eps_phy_led_blink_begin_SOC(): unsupported PhyTransceiver(%u) for SOC board",
                PhyTransceiver);
            EPS_FATAL("eps_phy_led_blink_begin_SOC1: unsupported PhyTransceiver");
    }
}

/**
* eps function to end blink on SOC boards
*
* When the external blink functions are used, this function stops blinking SOC
* PhyTransceivers on board. Depending on the Phy Type, the phy functions are called.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
* @param    [in] PhyTransceiver             Type of PhyTransceiver
*/
LSA_VOID eps_phy_led_blink_end_SOC(
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      HwPortIndex,
    LSA_UINT32      PhyTransceiver)
{
    switch (PhyTransceiver)
    {
        case EDDI_PHY_TRANSCEIVER_NSC:
            eps_phy_led_blink_end_SOC_NSC(hSysDev, HwPortIndex);
            break;
        default:
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,
                "eps_phy_led_blink_begin_SOC(): unsupported PhyTransceiver(%u) for SOC board",
                PhyTransceiver);
            EPS_FATAL("eps_phy_led_blink_begin_SOC: unsupported PhyTransceiver");
    }
}

/**
* eps function to set blink mode on SOC boards
*
* When the external blink functions are used, this function sets blink mode SOC for
* PhyTransceivers on board. Depending on the Phy Type, the phy functions are called.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
* @param    [in] PhyTransceiver             Type of PhyTransceiver
* @param    [in] mode                       LED function mode
*/
LSA_VOID eps_phy_led_blink_set_mode_SOC(
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      HwPortIndex,
    LSA_UINT32      PhyTransceiver,
    LSA_UINT16      mode)
{
    switch (PhyTransceiver)
    {
        case EDDI_PHY_TRANSCEIVER_NSC:
            eps_phy_led_blink_set_mode_SOC_NSC(hSysDev, HwPortIndex, mode);
            break;
        default:
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,
                "eps_phy_led_blink_set_mode_SOC(): unsupported PhyTransceiver(%u) for SOC board",
                PhyTransceiver);
            EPS_FATAL("eps_phy_led_blink_set_mode_SOC: unsupported PhyTransceiver");
    }
}

/**
* eps function to start blinking on SOC boards with NSC phys
*
* When the external blink functions are used, this function starts blinking SOC
* NSC PhyTransceivers on board. There is currently nothing to do here.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
*/
static LSA_VOID eps_phy_led_blink_begin_SOC_NSC(
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      HwPortIndex)
{
    //no actions necessary!
    LSA_UNUSED_ARG(hSysDev); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
}

/**
* eps function to stop blinking on SOC boards with NSC phys
*
* When the external blink functions are used, this function stops blinking SOC
* PhyTransceivers on board. The logic is copied from EDDI.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
*/
static LSA_VOID eps_phy_led_blink_end_SOC_NSC(
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      HwPortIndex)
{
    LSA_UINT32  PhyLEDDirectControlReg;
    EDDI_HANDLE hDDB;
    EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    EPS_ASSERT(pSys != 0);
    psi_hd_get_edd_handle(pSys->hd_nr, &hDDB);

    //disable force-led-function via PHY register LED Direct Control
    eddi_ll_PhyRead(hSysDev, hDDB, HwPortIndex, EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, &PhyLEDDirectControlReg);
    PhyLEDDirectControlReg &= ~(1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_LNKLED);
    PhyLEDDirectControlReg &= ~(1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_ACTLED);
    eddi_ll_PhyWrite(hSysDev, hDDB, HwPortIndex, EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, &PhyLEDDirectControlReg);
}

/**
* eps function to set blink mode on SOC boards with NSC phys
*
* When the external blink functions are used, this functions sets blink mode for SOC
* PhyTransceivers on board. The logic is copied from EDDI.
*
* @param    [in] hSysDev                    sysdev pointer
* @param    [in] HwPortIndex                Hardware Port index that shall blink
* @param    [in] mode                       LED function mode
*/
static LSA_VOID eps_phy_led_blink_set_mode_SOC_NSC(
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      HwPortIndex,
    LSA_UINT16      LEDMode)
{
    LSA_UINT32  PhyLEDDirectControlReg;
    EDDI_HANDLE hDDB;
    EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    EPS_ASSERT(pSys != 0);
    psi_hd_get_edd_handle(pSys->hd_nr, &hDDB);

    //control force-led-function via PHY register LED Direct Control
    eddi_ll_PhyRead(hSysDev, hDDB, HwPortIndex, EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, &PhyLEDDirectControlReg);
    PhyLEDDirectControlReg |= (1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_LNKLED);
    PhyLEDDirectControlReg |= (1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_ACTLED);
    PhyLEDDirectControlReg = (LEDMode == EDDI_LED_MODE_ON) ? (PhyLEDDirectControlReg & ~(1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__LNKLED)) : (PhyLEDDirectControlReg | (1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__LNKLED)); //0 = LED On ;
    PhyLEDDirectControlReg = (LEDMode == EDDI_LED_MODE_ON) ? (PhyLEDDirectControlReg & ~(1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__ACTLED)) : (PhyLEDDirectControlReg | (1 << EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_BIT__ACTLED)); //0 = LED On ;
    eddi_ll_PhyWrite(hSysDev, hDDB, HwPortIndex, EPS_SOC_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR, &PhyLEDDirectControlReg);
}

#endif // ( EPS_CFG_USE_EDDI == 1 )

/*----------------------------------------------------------------------------*/
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
