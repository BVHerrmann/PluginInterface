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
/*  F i l e               &F: eps_c66x_bootloader.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Firmware Loader for C66x Core on TI AM57xx                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20091
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <eps_sys.h>        /* Types / Prototypes         */
#include <eps_trc.h>
#include <eps_rtos.h>        /* Types / Prototypes / Funcs        */ 
#include <eps_app.h>

#include "eps_c66x_shm.h"

#include "eps_pndrvif.h"
#include "am5728_c66_drv.h"

#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
/** Clock adjustment:
 *      SYS_CLK:            20MHz
 *      DSP_CLK_DIVIDER:    5
 *      DSP_CLK_MULTIPLIER: 150
 *      => DSP CLK:         600MHz
 *      
 *      Register Settings:
 *      DPLL_DIV:           4 (Divider - 1)
 *      DPLL_MULT:          150
 *      CM_CLKSEL_DPLL_DSP: (DPLL_DIV << 0) | (DPLL_MULT << 8)
 *                          0x00009604
 */
#define DSP_CLKSEL                                              (0x00009604) 


/** physical base address of C66 Ctrl register memory */
#define EPS_PN_AM5728_C66_PHY_BASE_PTR                          (0x4a002000U)
/** size of C66 Ctrl register memory */
#define EPS_PN_AM5728_C66_SIZE                                  (0x00f00000U)


// Register masks and values
#define BOARD_TI_AM5728_PRCM_MODULEMODE_AUTO                       (1U)
#define BOARD_TI_AM5728_PRCM_MODULEMODE_MASK                       (3U)

#define BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKTRCTRL_MASK              (0x00000003U)
#define BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKTRCTRL_SHIFT             (0U)
#define BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKACTIVITY_DSP1_GFCLK_MASK (0x00000100U)
#define BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_LRST_SHIFT           (0U)
#define BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_SHIFT 			   (1U)
#define BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_LRST_SHIFT             (0x0U)
#define BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT                  (0x01U)

#define BOARD_TI_AM5728_PRCM_CD_CLKTRNMODES_SW_WAKEUP              (2U)


/** Entry address for DSP1 applications in the OCMC_RAM1 memory region where trampoline is located  */
#define BOARD_TI_AM5728_DSP1_ENTRY_ADDR                            (0x40330400U)
/** DSP1_PRM.PM_DSP1_PWRSTCTRL       */
#define BOARD_TI_AM5728_DSP1_PRM_PM_DSP1_PWRSTCTRL_REG              (0x4ae06400U)
/** DSP1_PRM.RM_DSP1_RSTST       */
#define BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG                  (0x4ae06414U)
/** DSP1_PRM.RM_DSP1_RSTCTRL         */
#define BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTCTRL_REG                (0x4ae06410U)
/** CM_CORE_AON__CKGEN.CM_CLKSEL_DPLL_DSP        */
#define BOARD_TI_AM5728_CM_CORE_AON_CM_CLKSEL_DPLL_DSP_REG          (0x4a005240U)
/** CM_CORE_AON__DSP1.CM_DSP1_CLKSTCTRL      */
#define BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_CLKSTCTRL_REG           (0x4a005400U)
/** CM_CORE_AON__DSP1.CM_DSP1_DSP1_CLKCTRL       */
#define BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_DSP1_CLKCTRL_REG        (0x4a005420U)
/** CTRL_MODULE_CORE.CTRL_CORE_CONTROL_DSP1_RST_VECT     */
#define BOARD_TI_AM5728_CTRL_MODULE_CORE_CTRL_CORE_CONTROL_DSP1_RST_VECT_REG        (0x4a00255CU)

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
/**
** DSP instructions to be copied to the Trampoline to avoid the alignment
** requirement.
*/
LSA_UINT32 dsp1Instruction[10] =
{
    0x0500002a,         /**< MVK.S2  destAddr, B10      */
    0x0500006a,         /**< MVKH.S2 destAddr, B10      */
    0x00280362,         /**< B.S2    B10                */
    0x00006000,         /**< NOP     4                  */
    0x00000000,         /**< NOP                        */
    0x00000000          /**< NOP                        */
};

// mapped register address
LSA_UINT8* g_pMapRegBase;

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

/**
 * Write word to memory
 * 
 * @param addr [in]     Address to write to
 * @param value [in]    Value to write
 */
static inline LSA_VOID AM5728_WRITE_REG32(LSA_UINT32 addr, LSA_UINT32 value)
{
    *(volatile LSA_UINT32 *) (addr - EPS_PN_AM5728_C66_PHY_BASE_PTR + ((LSA_UINT32)g_pMapRegBase)) = value;
    // Mem barrier
    asm("    dsb");
    return;
}

/**
 * Read word from address
 * @param addr [in] Address to read from
 * @return Read word from memory
 */
static inline LSA_UINT32 AM5728_READ_REG32(LSA_UINT32 addr)
{
	LSA_UINT32 regVal = *(volatile LSA_UINT32 *) (addr - EPS_PN_AM5728_C66_PHY_BASE_PTR + ((LSA_UINT32)g_pMapRegBase));
	// Mem barrier
	asm("    dsb");
    return (regVal);
}

/**
 * Write bitfield to memory
 * @param addr [in] Address to write to
 * @param mask [in] Mask
 * @param shift [in] Shift
 * @param value [in] Value
 */
static inline LSA_VOID AM5728_WRITE_FIELD32(LSA_UINT32 addr,
		LSA_UINT32 mask,
		LSA_UINT32 shift,
		LSA_UINT32 value)
{
    LSA_UINT32 address = addr - EPS_PN_AM5728_C66_PHY_BASE_PTR + ((LSA_UINT32)g_pMapRegBase);
	LSA_UINT32 regVal = *(volatile LSA_UINT32 *) address;
    regVal &= (~mask);
    regVal |= (value << shift) & mask;
    *(volatile LSA_UINT32 *) address = regVal;
    // Mem barrier
    asm("    dsb");
    return;
}

/**
 * Read bitfield from memory
 * @param addr [in] Address to read from
 * @param mask [in] Mask
 * @param shift [in] Shift
 * @return Read value
 */
static inline LSA_UINT32 AM5728_READ_FIELD32(LSA_UINT32 addr,
		LSA_UINT32 mask,
		LSA_UINT32 shift)
{
    LSA_UINT32 address = addr - EPS_PN_AM5728_C66_PHY_BASE_PTR + ((LSA_UINT32)g_pMapRegBase);
	LSA_UINT32 regVal = *(volatile LSA_UINT32 *) address;
    regVal = (regVal & mask) >> shift;
    // Mem barrier
    asm("    dsb");
    return (regVal);
}


/**
 * Set entry point address in trampoline code. Trampoline code is used to avoid the alignment
 * requirement for productive code.
 * 
 * @param entryPoint dsp1 entry point address
 * @param pDspInstr pointer to the trampoline code which is written as absolut startup
 */
static LSA_VOID eps_c66x_bootloader_entryPointSet(LSA_UINT32 entryPoint, LSA_UINT32 *pDspInstr)
{
	LSA_UINT32 entryVal = 0U;
	LSA_UINT32 dspOpcode = 0U;

    entryVal = (entryPoint & 0x0000FFFF);
    dspOpcode = *pDspInstr;
    /*
    ** Mask and update the lower 16 bits of entry address within the MVK
    ** instruction opcode.
    */
    *pDspInstr = (((dspOpcode) & ~(0x007FFF80)) | (( (entryVal) << 0x7) & 0x007FFF80));

    entryVal = ((entryPoint & 0xFFFF0000) >> 16);
    dspOpcode = *(pDspInstr + 1);
    /*
    ** Mask and update the upper 16 bits of entry address within the MVK
    ** instruction opcode.
    */
    *(pDspInstr + 1) = (((dspOpcode) & ~(0x007FFF80)) | (( (entryVal) << 0x7) & 0x007FFF80));
}


/**
 *  Enable c66 module power and clock
 */
static LSA_VOID eps_c66x_bootloader_DSP1_ClkEnable(LSA_VOID)
{
    // set dsp clock to 600MHz (see DSP_CLKSEL)
    AM5728_WRITE_REG32(BOARD_TI_AM5728_CM_CORE_AON_CM_CLKSEL_DPLL_DSP_REG, DSP_CLKSEL);
    
    // Enable DSP1 module clock
    AM5728_WRITE_REG32((BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_DSP1_CLKCTRL_REG), BOARD_TI_AM5728_PRCM_MODULEMODE_AUTO);
    // Check DSP1 module clock enable status
    while(BOARD_TI_AM5728_PRCM_MODULEMODE_AUTO !=
        (AM5728_READ_REG32(BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_DSP1_CLKCTRL_REG) & BOARD_TI_AM5728_PRCM_MODULEMODE_MASK));

    // Enable the DSP1 module
    AM5728_WRITE_FIELD32(BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_CLKSTCTRL_REG, 
                         BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKTRCTRL_MASK,
                         BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKTRCTRL_SHIFT,
                         BOARD_TI_AM5728_PRCM_CD_CLKTRNMODES_SW_WAKEUP);
    
    // CheckDSP1 module enable status 
    while(BOARD_TI_AM5728_PRCM_CD_CLKTRNMODES_SW_WAKEUP != AM5728_READ_FIELD32(BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_CLKSTCTRL_REG,
                                                                BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKTRCTRL_MASK,
                                                                BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKTRCTRL_SHIFT));
    
    // Check clock activity - ungated
    while(BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKACTIVITY_DSP1_GFCLK_MASK != 
            (AM5728_READ_REG32(BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_CLKSTCTRL_REG) & BOARD_TI_AM5728_DSP1_CLKSTCTRL_CLKACTIVITY_DSP1_GFCLK_MASK));
}

/**
 * Reset c66x core on TI AM5728
 */
static LSA_VOID eps_c66x_bootloader_DSP1_Reset(LSA_VOID)
{
	LSA_UINT32 retVal;
	LSA_UINT32 resetCtrlMask;
	LSA_UINT32 rstStsMask;

	/* SYSTEM Reset */
    {
		resetCtrlMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_SHIFT;
		AM5728_WRITE_FIELD32((BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTCTRL_REG), resetCtrlMask, BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_SHIFT, 0x1U);
    }

	/* Local Reset */
    {
        resetCtrlMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_LRST_SHIFT;
        AM5728_WRITE_FIELD32((BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTCTRL_REG), resetCtrlMask, BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_LRST_SHIFT, 0x1U);
        // get status
        rstStsMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_LRST_SHIFT;
        retVal = (LSA_UINT32) AM5728_READ_FIELD32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG, rstStsMask,
                                               BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_LRST_SHIFT);
    	if(0x1U == retVal)
    	{
    		/* clear Status */
    		rstStsMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_LRST_SHIFT;
    		AM5728_WRITE_FIELD32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG, rstStsMask, BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_LRST_SHIFT, (LSA_UINT32) 0x1U);
    	}
    }
	/* System Reset */
    {
        rstStsMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT;
        retVal = (LSA_UINT32) AM5728_READ_FIELD32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG, rstStsMask,
                                               BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT);
    	if(0x1U == retVal)
    	{
    		/* Clear Status */
    		rstStsMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT;
    	    AM5728_WRITE_FIELD32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG, rstStsMask, BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT, (LSA_UINT32) 0x1U);
    	}
    }

    /* reset release	*/
    {
        resetCtrlMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_SHIFT;
        AM5728_WRITE_FIELD32((BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTCTRL_REG), resetCtrlMask, BOARD_TI_AM5728_DSP1_RSTCTRL_RST_DSP1_SHIFT, 0x0U);

        rstStsMask = (LSA_UINT32) 1U << BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT;
        while(0x1U != ((LSA_UINT32) AM5728_READ_FIELD32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG, rstStsMask,
                                                     BOARD_TI_AM5728_DSP1_RSTST_RST_DSP1_SHIFT)));
    }
}

/**
 * Start c66x core on TI AM5728
 * 
 * @param programEntryAddress The entry address of the code
 */
static LSA_VOID eps_c66x_bootloader_start_core(LSA_UINT32 programEntryAddress)
{
    // Set Power ON DSP1
    AM5728_WRITE_REG32(BOARD_TI_AM5728_DSP1_PRM_PM_DSP1_PWRSTCTRL_REG, 0x3U);
    
    // Enable Clock for DSP1
    eps_c66x_bootloader_DSP1_ClkEnable();

    // Reset DSP1 module
    eps_c66x_bootloader_DSP1_Reset();

    // Set the Entry point of the application in the trampoline code.
    eps_c66x_bootloader_entryPointSet(programEntryAddress, dsp1Instruction);

    /*
    ** Copy the dsp opcodes to create a trampoline at the specified memory
    ** location which is aligned at 0x400. This helps us in removing the
    ** entry address alignment limitation.
    */
    memcpy((LSA_VOID *) BOARD_TI_AM5728_DSP1_ENTRY_ADDR, (LSA_VOID *)dsp1Instruction,
        sizeof(dsp1Instruction));

    // DSP L2RAM
    // Set the Entry point
    AM5728_WRITE_REG32(BOARD_TI_AM5728_CTRL_MODULE_CORE_CTRL_CORE_CONTROL_DSP1_RST_VECT_REG, (BOARD_TI_AM5728_DSP1_ENTRY_ADDR >> 0xAU));

    // Reset de-assertion for DSPSS
    AM5728_WRITE_REG32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTCTRL_REG, 0x1);
    // Check the reset state: DSPSS
    while ((AM5728_READ_REG32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG) & 0x2) != 0x2);

    // Reset de-assertion for DSP CPUs
    AM5728_WRITE_REG32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTCTRL_REG, 0x0);
    // Check the reset state: DSPSS Core, Cache and Slave interface
    while ((AM5728_READ_REG32(BOARD_TI_AM5728_DSP1_PRM_RM_DSP1_RSTST_REG) & 0x3) != 0x3);

    // Check module mode
    while ((AM5728_READ_REG32(BOARD_TI_AM5728_CM_CORE_AON_CM_DSP1_DSP1_CLKCTRL_REG) & 0x30000) != 0x0);
    
    // Startup done. DSP1 is running
}



/**
 * Prepare loading of c66x co-processor and start core.
 * Firmware has to be loaded to memory before this function is called.
 *
 * @param programEntryAddress [in] program start address to jump to after booting up
 * 
 * @return LSA_RET_ERR_RESOURCE Loading c66x core failed
 * @return LSA_RET_OK           C66x loaded and started successfully 
 */
LSA_RESULT eps_c66x_bootloader_dsp1( LSA_UINT32 programEntryAddress )
{
    LSA_INT32 PnFd;
    LSA_INT32 ret_val = EPS_PN_DRV_RET_OK;
    
    if (am5728_c66_drv_install() != EPS_PN_DRV_RET_OK)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_c66x_bootloader_dsp1() - Installing c66 driver in adonis failed.");
        return LSA_RET_ERR_RESOURCE;
    }
   
    PnFd = open(AM5728_C66_DRV_NAME, O_RDWR);
    
    /* Mapping for base address of c66 ctrl register memory */
    // go to kernel-mode to save the base address (physical address)
    ret_val = ioctl( PnFd, 
                     EPS_AM5728_C66_DRV_IOCTL_SRV_MMAP_REG_BASE_ADR, 
                     (LSA_VOID*)EPS_PN_AM5728_C66_PHY_BASE_PTR );
    if (ret_val != EPS_PN_DRV_RET_OK)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_c66x_bootloader_dsp1() - Mapping c66 ctrl register mem failed.");
        return LSA_RET_ERR_RESOURCE;
    }
    
    // map the physical address in kernel-mode to virtual address in user-mode
    g_pMapRegBase = (LSA_UINT8*) mmap( NULL, 
                                     EPS_PN_AM5728_C66_SIZE, 
                                     PROT_READ | PROT_WRITE, 
                                     MAP_SHARED, 
                                     PnFd, 
                                     0 );

   
    // Initialize the shared mem section
    eps_c66x_shm_init(); 
        
    // start the c66 core
    eps_c66x_bootloader_start_core(programEntryAddress);

    // write magic value to start the c66 firmware execution
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_c66x_bootloader_dsp1() - Enabling c66 to boot up.");
    eps_c66x_shm_write_c66xReleaseCmd();

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_c66x_bootloader_dsp1() - Waiting for c66 to boot up.");
    
    // wait for the c66 to be booted up
    eps_c66x_shm_wait_c66xReadyCmd();
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_c66x_bootloader_dsp1() - c66 booted up successfully.");

    return LSA_RET_OK;
}

