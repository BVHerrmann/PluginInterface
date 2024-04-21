/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: eddi_sys_soc_i2c.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* examples for SOC-specific functions.                                      */
/* - I2C                                                                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  11.04.13    TB    created                                                */
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

#include "eddi_sys_reg_soc12.h"
#include "eddi_sys_soc_hw.h"

#ifdef __cplusplus //If C++ - compiler: Use C linkage
extern "C"
{
#endif

//#define EDDI_SYS_SOC_I2C_FAST_MODE
//#define EDDI_SYS_SOC_USE_GLOBAL_VARS

struct cp_mem_tag
{
    LSA_UINT32   pIRTE;
    LSA_UINT32   pI2C;
    LSA_UINT32   pSCRB;
    LSA_UINT32   pGPIO;
    EDDI_HANDLE  hDDB;
    LSA_BOOL     bBasePointersInitialized;
    LSA_BOOL     bI2CSetupExecuted;

} cp_mem = {0};

#if defined (EDDI_SYS_SOC_USE_GLOBAL_VARS)
LSA_UINT32   p_soc_IRTE;
LSA_UINT32   p_soc_I2C;
LSA_UINT32   p_soc_SCRB;
LSA_UINT32   p_soc_GPIO;
EDDI_HANDLE  p_soc_hDDB;
#endif

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

// ***** wait 2us
#if !defined (EDDI_SYS_SOC_I2C_WAIT_2US)
#define EDDI_SYS_SOC_I2C_WAIT_2US(hDDB)     (EDDI_WAIT_10_NS(hDDB, (100UL * 2UL)))
#endif

// ***** states returned by HW into the status register
#define EDDI_SYS_SOC_I2C_STATE_START_0      0x08    //START condition transmitted
#define EDDI_SYS_SOC_I2C_STATE_START_N      0x10    //Repeated START condition transmitted
#define EDDI_SYS_SOC_I2C_STATE_ADR_WR       0x18    //Address + write bit transmitted, ACK received
#define EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK  0x28    //Data byte transmitted in master mode, ACK received
#define EDDI_SYS_SOC_I2C_STATE_WR_DATA_NAK  0x30    //Data byte transmitted in master mode, ACK not received
#define EDDI_SYS_SOC_I2C_STATE_ADR_RD       0x40    //Address + read bit transmitted, ACK received
#define EDDI_SYS_SOC_I2C_STATE_RD_DATA_ACK  0x50    //Data byte received in master mode, ACK transmitted
#define EDDI_SYS_SOC_I2C_STATE_RD_DATA_NAK  0x58    //Data byte received in master mode, NAK transmitted
#define EDDI_SYS_SOC_I2C_STATE_IDLE         0xF8    //No relevant status information, IFLG=0

// ***** commands to the HW statemachine of the MI2C-IP
#define EDDI_SYS_SOC_I2C_CMD_IDLE           (SOC_REG_I2C_MI2_CNTR__MSK_IEN | SOC_REG_I2C_MI2_CNTR__MSK_ENAB)
#define EDDI_SYS_SOC_I2C_CMD_START          (EDDI_SYS_SOC_I2C_CMD_IDLE | SOC_REG_I2C_MI2_CNTR__MSK_STA)
#define EDDI_SYS_SOC_I2C_CMD_WR_DATA        (EDDI_SYS_SOC_I2C_CMD_IDLE | 0)
#define EDDI_SYS_SOC_I2C_CMD_RD_DATA_ACK    (EDDI_SYS_SOC_I2C_CMD_IDLE | SOC_REG_I2C_MI2_CNTR__MSK_AAK)
#define EDDI_SYS_SOC_I2C_CMD_RD_DATA_NAK    (EDDI_SYS_SOC_I2C_CMD_IDLE | 0)
#define EDDI_SYS_SOC_I2C_CMD_STOP           (EDDI_SYS_SOC_I2C_CMD_IDLE | SOC_REG_I2C_MI2_CNTR__MSK_STP)

#define EDDI_SYS_SOC_I2C_BUS_HIGHZ          (SOC_REG_I2C_SW_I2C_CTRL__MSK_SDA_I | SOC_REG_I2C_SW_I2C_CTRL__MSK_SCL_I | SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SDA_O | SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SCL_O)

/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/

// ***** check macros after I2C command
#define EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt)                     {ToCnt = 0x1000UL; while (ToCnt) {if (REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) & SOC_REG_I2C_MI2_CNTR__MSK_IFLG) break; ToCnt--;}}
#define EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, Stat)  {ToCnt = 0x1000UL; while (ToCnt) {if ((StatusReg = REG32b(pI2C, SOC_REG_I2C_MI2_STAT)) == Stat) break; ToCnt--;}}


/***************************************************************************
* F u n c t i o n:       EDDI_SysSocGetBasePtrs()
*
* D e s c r i p t i o n: reads the base-ptrs
*/
#if !defined (EDDI_SYS_SOC_USE_GLOBAL_VARS)
static  LSA_BOOL  EDDI_SysSocGetBasePtrs( LSA_UINT32   *  const  ppI2C,
                                          LSA_UINT32   *  const  ppSCRB,
                                          LSA_UINT32   *  const  ppGPIO,
                                          EDDI_HANDLE  *  const  phDDB )
{
    *ppI2C  = cp_mem.pI2C;
    *ppSCRB = cp_mem.pSCRB;
    *ppGPIO = cp_mem.pGPIO;
    *phDDB  = cp_mem.hDDB;

    return LSA_TRUE;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:       EDDI_SysSocSetBasePtr()
*
* D e s c r i p t i o n: saves the base-ptrs for later use
*/
#if !defined (EDDI_SYS_SOC_USE_GLOBAL_VARS)
LSA_VOID  EDDI_SysSocSetBasePtr( LSA_UINT32   const  pSOC_IRTE,
                                 LSA_UINT32   const  pSOC_I2C,
                                 LSA_UINT32   const  pSOC_SCRB,
                                 LSA_UINT32   const  pSOC_GPIO,
                                 EDDI_HANDLE  const  hDDB )
{
    cp_mem.pIRTE    = pSOC_IRTE;
    cp_mem.pI2C     = pSOC_I2C;
    cp_mem.pSCRB    = pSOC_SCRB;
    cp_mem.pGPIO    = pSOC_GPIO;
    cp_mem.hDDB     = hDDB;

    cp_mem.bBasePointersInitialized = LSA_TRUE;
}
#else
LSA_VOID  EDDI_SysSocSetBasePtr( LSA_UINT32   const  pSOC_IRTE,
                                 LSA_UINT32   const  pSOC_I2C,
                                 LSA_UINT32   const  pSOC_SCRB,
                                 LSA_UINT32   const  pSOC_GPIO,
                                 EDDI_HANDLE  const  hDDB )
{
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SysSocI2cFixupBus()                    +*/
/*+                                                                         +*/
/*+  Input/Output          :                                                +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Results    : LSA_TRUE                                                  +*/
/*+               LSA_FALSE                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Description: Toggle the SCL line until SDA read HighZ.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL  EDDI_SysSocI2cFixupBus( EDDI_HANDLE  const  hDDB,
                                  LSA_UINT32   const  pI2C )
{
    LSA_UINT32  Reg, Bus, Timeout;

    Timeout = 100UL;
    Bus     = REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) & EDDI_SYS_SOC_I2C_BUS_HIGHZ;
    Reg     = REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1);

    while (Timeout && (Bus != EDDI_SYS_SOC_I2C_BUS_HIGHZ))
    {
        // ***** Toggle SCL if HW state machine hangs until bus is HighZ
        Reg ^= SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE;
        REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1) = Reg;
        Reg = REG32b(pI2C, SOC_REG_I2C_EX_CTRL_1); //dummy-read to finish write access
        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
        if (Reg & SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE)
        {   //toggle until Bus == Fh and Reg without SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE bit set!
            Bus = 0;
        }
        else
        {
            Bus = REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) & EDDI_SYS_SOC_I2C_BUS_HIGHZ;
        }
        Timeout--;
    }

    if (Timeout) //Timeout not yet elapsed?
    {
        return LSA_TRUE;
    }
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SysSocI2cDoReset()                     +*/
/*+                                                                         +*/
/*+  Input/Output          :                                                +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Results    : LSA_TRUE                                                  +*/
/*+               LSA_FALSE                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Description: Resets the MI2C module within the SOC1.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_BOOL  EDDI_SysSocI2cDoReset( EDDI_HANDLE        hDDB,
                                         LSA_UINT32  const  pI2C )
{
    LSA_UINT16  Timeout = 500;

    REG32b(pI2C, SOC_REG_I2C_MI2C_XADDR)          = 0;
    REG32b(pI2C, SOC_REG_I2C_MI2C_SOFTWARE_RESET) = SOC_REG_I2C_MI2C_SOFTWARE_RESET__MSK_SOFTRESET;
    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

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
/*---------------------- end [subroutine] ---------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SysSocI2cInit                          +*/
/*+                                                                         +*/
/*+  Input/Output          :                                                +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_HW                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Description: Initialization of most common I2C registers.              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static  LSA_RESULT  EDDI_SysSocI2cInit( LSA_UINT32   const  pI2C,
                                        LSA_UINT32   const  pSCRB,
                                        LSA_UINT32   const  pGPIO,
                                        EDDI_HANDLE  const  hDDB )
{
    LSA_UINT32  Val;

    // ***** set I2C clock rate
    #if defined (EDDI_SYS_SOC_I2C_FAST_MODE)
    REG32b(pSCRB, CCR_I2C_0) = 12UL; //12 -> 400kBit/s (384,62kBit/s)
    #else
    REG32b(pSCRB, CCR_I2C_0) = 49UL; //49 -> 100kBit/s = default
    #endif
    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

    if (!EDDI_SysSocI2cDoReset(hDDB, pI2C))
    {
        return EDD_STS_ERR_HW;
    }

    Val = REG32b(pI2C, SOC_REG_I2C_SW_I2C_CTRL) & EDDI_SYS_SOC_I2C_BUS_HIGHZ;
    if (Val != EDDI_SYS_SOC_I2C_BUS_HIGHZ)
    {
        if (!EDDI_SysSocI2cFixupBus(hDDB, pI2C))
        {
            return EDD_STS_ERR_HW;
        }
    }

    //activate I2C-GPIOs of SOC1 (select alternate function 0)
    REG32b(pGPIO, GPIO_PORT_MODE_5_H) = (REG32b(pGPIO, GPIO_PORT_MODE_5_H) & (~(3UL<<((SYS_SOC_SOC1_I2C_GPIO_SCL-176)*2))) | (1UL<<((SYS_SOC_SOC1_I2C_GPIO_SCL-176)*2)));
    REG32b(pGPIO, GPIO_PORT_MODE_5_H) = (REG32b(pGPIO, GPIO_PORT_MODE_5_H) & (~(3UL<<((SYS_SOC_SOC1_I2C_GPIO_SDA-176)*2))) | (1UL<<((SYS_SOC_SOC1_I2C_GPIO_SDA-176)*2)));

    cp_mem.bI2CSetupExecuted = LSA_TRUE;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: EDDI_SysSocI2cReadOffset()
 *
 * function:      API-function: reads n bytes from the I2C device
 *
 * parameters:    ...
 *
 * return value:  LSA_RESULT
 *
 *===========================================================================*/
LSA_RESULT  EDDI_SysSocI2cReadOffset( EDDI_SYS_HANDLE			     const  hSysDev,      //UpperDeviceHandle (DeviceOpen input parameter)
	                                  LSA_UINT8			             const  I2CDevAddr,   //0...127
	                                  LSA_UINT8			             const  I2COffsetCnt, //0...2
	                                  LSA_UINT8			             const  I2COffset1,   //optional: 0...255
	                                  LSA_UINT8			             const  I2COffset2,   //optional: 0...255
	                                  LSA_UINT32                     const  Size,      	  //in bytes, 1...300
	                                  LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf )    	  //pointer to buffer
{
    LSA_RESULT   Result;
    LSA_UINT32   Cmd;
    LSA_UINT32   State;                    
    LSA_UINT32   Len    = Size;                    
    LSA_UINT32   ToCnt;
    LSA_UINT32   StatusReg;
    LSA_UINT32   BufIdx = 0;                    
    LSA_UINT32   pI2C;  //Baseptr to I2C-Regs
    LSA_UINT32   pSCRB; //Baseptr to SCRB-Regs
    LSA_UINT32   pGPIO; //Baseptr to GPIO-Regs
    EDDI_HANDLE  hDDB;  //Lower Handle of EDDI

    LSA_UNUSED_ARG(hSysDev);

    if (!cp_mem.bBasePointersInitialized)
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    #if defined (EDDI_SYS_SOC_USE_GLOBAL_VARS)
    //Check the baseptrs for the call
    if (   (p_soc_I2C  == 0)	//reset not done?
        || (p_soc_SCRB == 0)    //reset not done?
        || (p_soc_GPIO == 0)    //reset not done?
        || (p_soc_hDDB == 0))   //reset not done?
    {
        return EDD_STS_ERR_PARAM;
    }

    pI2C  = p_soc_I2C;
    pSCRB = p_soc_SCRB;
    pGPIO = p_soc_GPIO;
    hDDB  = p_soc_hDDB;
    #else
    if (!EDDI_SysSocGetBasePtrs(&pI2C, &pSCRB, &pGPIO, &hDDB))
    {
        return EDD_STS_ERR_PARAM;
    }
    #endif

    if //I2C device not yet initialized?
       (!cp_mem.bI2CSetupExecuted)
    {
        Result = EDDI_SysSocI2cInit(pI2C, pSCRB, pGPIO, hDDB);
        if (Result != EDD_STS_OK)
        {
            return EDD_STS_ERR_HW;
        }
    }

    //PageRead: START, I2CDevAddr, ByteAddr, Rd, Rd, ... , [STOP]

    while (Len)
    {
        if (!BufIdx)
        {
            // ***** send START:  IFLG bit will be set, Code=08h
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_START;                //Start
            EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
            if (ToCnt) //Timeout not yet elapsed?
            {
                EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_START_0); //START condition transmitted
            }
            if (!ToCnt) //Timeout elapsed?
            {
                // ***** try to restart the HW state machine
                EDDI_SysSocI2cDoReset(hDDB, pI2C);
                return EDD_STS_ERR_HW;
            }

            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

            // ***** send DEV-Address + Ack:  IFLG bit will be set, Code=18h
            REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = (I2CDevAddr & (LSA_UINT8)0xFE);
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;             //DEV
            EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
            if (ToCnt) //Timeout not yet elapsed?
            {
                EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_ADR_WR); //Address + write bit transmitted, ACK received
            }
            if (!ToCnt) //Timeout elapsed?
            {
                REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;             //Stop
                EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                return EDD_STS_ERR_PROTOCOL;
            }

            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

            switch //by I2COffsetCnt (already checked by value)
                   (I2COffsetCnt)
            {
                case 1:
                {
                    // ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
                    REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
                    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;     //E2
                    EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
                    if (ToCnt) //Timeout not yet elapsed?
                    {
                        EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK); //Data byte transmitted in master mode, ACK received
                    }
                    if (!ToCnt) //Timeout elapsed?
                    {
                        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;     //Stop
                        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                        return EDD_STS_ERR_PROTOCOL;
                    }

                    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

                    break;
                }
                case 2:
                {
                    // ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
                    REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
                    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;     //E2
                    EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
                    if (ToCnt) //Timeout not yet elapsed?
                    {
                        EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK); //Data byte transmitted in master mode, ACK received
                    }
                    if (!ToCnt) //Timeout elapsed?
                    {
                        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;     //Stop
                        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                        return EDD_STS_ERR_PROTOCOL;
                    }

                    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

                    // ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
                    REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset2;
                    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;     //E2
                    EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
                    if (ToCnt) //Timeout not yet elapsed?
                    {
                        EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK); //Data byte transmitted in master mode, ACK received
                    }
                    if (!ToCnt) //Timeout elapsed?
                    {
                        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;     //Stop
                        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                        return EDD_STS_ERR_PROTOCOL;
                    }

                    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

                    break;
                }
                default: break;
            }

            // ***** send next START:  IFLG bit will be set, Code=10h
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_START;                //Start 
            EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
            if (ToCnt) //Timeout not yet elapsed?
            {
                EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_START_N); //Repeated START condition transmitted
            }
            if (!ToCnt) //Timeout elapsed?
            {
                REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = 0xFF;
                REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_STOP;            //Stop
                EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                return EDD_STS_ERR_PROTOCOL;
            }

            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

            // ***** send DEV-Address + Ack:  IFLG bit will be set, Code=40h
            REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = (I2CDevAddr | 1);
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_STATE_ADR_RD;            //DEV
            EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
            if (ToCnt) //Timeout not yet elapsed?
            {
                EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_ADR_RD); //Address + read bit transmitted, ACK received
            }
            if (!ToCnt) //Timeout elapsed?
            {
                REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;             //Stop
                EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                return EDD_STS_ERR_PROTOCOL;
            }

            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
        }

        if (Len == 1UL)
        {   // ***** last byte
            Cmd   = EDDI_SYS_SOC_I2C_CMD_RD_DATA_NAK;       //send Dummy + Send Nak: IFLG bit will be set, Code=50h
            State = EDDI_SYS_SOC_I2C_STATE_RD_DATA_NAK;     //Data byte received in master mode, NAK transmitted
        }
        else
        {   // ***** pending byte exists
            Cmd   = EDDI_SYS_SOC_I2C_CMD_RD_DATA_ACK;       //send Dummy + Send Ack: IFLG bit will be set, Code=50h
            State = EDDI_SYS_SOC_I2C_STATE_RD_DATA_ACK;     //Data byte received in master mode, ACK transmitted
        }

        //send Dummy + Send Nak/Ack:  IFLG bit will be set, Code=50h/58h
        REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = 0xFF;
        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = Cmd;                                          //D[?]
        EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
        pBuf[BufIdx] = (LSA_UINT8)REG32b(pI2C, SOC_REG_I2C_MI2C_DATA);
        if (ToCnt) //Timeout not yet elapsed?
        {
            EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, State);                    //Data byte received in master mode, NAK/ACK transmitted
        }
        if (!ToCnt) //Timeout elapsed?
        {
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;                 //Stop
            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
            return EDD_STS_ERR_PROTOCOL;
        }

        Len--;
        BufIdx++;
    } //end of while loop

    // ***** send Stop:  IFLG bit will not be set, Code=F8h
    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;                         //Stop
    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
    EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_IDLE);      //No relevant status information, IFLG=0
    if (!ToCnt) //Timeout elapsed?
    {
        return EDD_STS_ERR_PROTOCOL;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: EDDI_SysSocI2cWriteOffset()
 *
 * function:      API-function: writes n bytes to the I2C device
 *
 * parameters:    ...
 *
 * return value:  LSA_RESULT
 *
 *===========================================================================*/
LSA_RESULT  EDDI_SysSocI2cWriteOffset( EDDI_SYS_HANDLE			      const  hSysDev,      //UpperDeviceHandle (DeviceOpen input parameter)
	                                   LSA_UINT8			          const  I2CDevAddr,   //0...127
	                                   LSA_UINT8			          const  I2COffsetCnt, //0...2
	                                   LSA_UINT8			          const  I2COffset1,   //optional: 0...255
	                                   LSA_UINT8			          const  I2COffset2,   //optional: 0...255
	                                   LSA_UINT32                     const  Size,         //in bytes, 1...300
	                                   LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf )    	   //pointer to buffer
{
    LSA_RESULT   Result;
    LSA_UINT32   Len    = Size;
    LSA_UINT32   ToCnt;
    LSA_UINT32   StatusReg;
    LSA_UINT32   BufIdx = 0;
    LSA_UINT32   pI2C;  //Baseptr to I2C-Regs
    LSA_UINT32   pSCRB; //Baseptr to SCRB-Regs
    LSA_UINT32   pGPIO; //Baseptr to GPIO-Regs
    EDDI_HANDLE  hDDB;  //Lower Handle of EDDI

    LSA_UNUSED_ARG(hSysDev);

    if (!cp_mem.bBasePointersInitialized)
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    #if defined (EDDI_SYS_SOC_USE_GLOBAL_VARS)
    //Check the baseptrs for the call
    if (   (p_soc_I2C  == 0)	//reset not done?
        || (p_soc_SCRB == 0)    //reset not done?
        || (p_soc_GPIO == 0)    //reset not done?
        || (p_soc_hDDB == 0))   //reset not done?
    {
        return EDD_STS_ERR_PARAM;
    }

    pI2C  = p_soc_I2C;
    pSCRB = p_soc_SCRB;
    pGPIO = p_soc_GPIO;
    hDDB  = p_soc_hDDB;
    #else
    if (!EDDI_SysSocGetBasePtrs(&pI2C, &pSCRB, &pGPIO, &hDDB))
    {
        return EDD_STS_ERR_PARAM;
    }
    #endif

    if //I2C device not yet initialized?
       (!cp_mem.bI2CSetupExecuted)
    {
        Result = EDDI_SysSocI2cInit(pI2C, pSCRB, pGPIO, hDDB);
        if (Result != EDD_STS_OK)
        {
            return EDD_STS_ERR_HW;
        }
    }

    //PageWrite: START, I2CDevAddr, ByteAddr, Wr, Wr, ... , STOP

    while (Len)
    {
        if (!BufIdx)
        {
            // ***** send START:  IFLG bit will be set, Code=08h
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_START;                //Start
            EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
            if (ToCnt) //Timeout not yet elapsed?
            {
                EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_START_0); //START condition transmitted
            }
            if (!ToCnt) //Timeout elapsed?
            {
                // ***** try to restart the HW state machine
                EDDI_SysSocI2cDoReset(hDDB, pI2C);
                return EDD_STS_ERR_HW;
            }

            // ***** send DEV-Address + Ack:  IFLG bit will be set, Code=18h
            REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2CDevAddr;
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;             //DEV
            EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
            if (ToCnt) //Timeout not yet elapsed?
            {
                EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_ADR_WR); //Address + write bit transmitted, ACK received
            }
            if (!ToCnt) //Timeout elapsed?
            {
                REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;             //Stop
                EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                return EDD_STS_ERR_PROTOCOL;
            }

            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

            switch //by I2COffsetCnt (already checked by value)
                   (I2COffsetCnt)
            {
                case 1:
                {
                    // ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
                    REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
                    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;     //E2
                    EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
                    if (ToCnt) //Timeout not yet elapsed?
                    {
                        EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK); //Data byte transmitted in master mode, ACK received
                    }
                    if (!ToCnt) //Timeout elapsed?
                    {
                        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;     //Stop
                        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                        return EDD_STS_ERR_PROTOCOL;
                    }

                    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

                    break;
                }
                case 2:
                {
                    // ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
                    REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset1;
                    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;     //E2
                    EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
                    if (ToCnt) //Timeout not yet elapsed?
                    {
                        EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK); //Data byte transmitted in master mode, ACK received
                    }
                    if (!ToCnt) //Timeout elapsed?
                    {
                        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;     //Stop
                        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                        return EDD_STS_ERR_PROTOCOL;
                    }

                    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

                    // ***** send E2-Address + Ack:  IFLG bit will be set, Code=28h
                    REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = I2COffset2;
                    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;		//E2
                    EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
                    if (ToCnt) //Timeout not yet elapsed?
                    {
                        EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK); //Data byte transmitted in master mode, ACK received
                    }
                    if (!ToCnt) //Timeout elapsed?
                    {
                        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;     //Stop
                        EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
                        return EDD_STS_ERR_PROTOCOL;
                    }

                    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);

                    break;
                }
                default: break;
            }
        }

        // ***** send DataByte + Ack:  IFLG bit will be set, Code=28h
        REG32b(pI2C, SOC_REG_I2C_MI2C_DATA) = pBuf[BufIdx];
        REG32b(pI2C, SOC_REG_I2C_MI2_CNTR)  = EDDI_SYS_SOC_I2C_CMD_WR_DATA;                 //D[?]
        EDDI_SYS_SOC_I2C_WAIT_IFLG(pI2C, ToCnt);
        if (ToCnt) //Timeout not yet elapsed?
        {
            EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_WR_DATA_ACK);  //Data byte transmitted in master mode, ACK received
        }
        if (!ToCnt) //Timeout elapsed?
        {
            REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;                 //Stop
            EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
            return EDD_STS_ERR_PROTOCOL;
        }

        Len--;
        BufIdx++;
    } //end of while loop

    // ***** send Stop:  IFLG bit will not be set, Code=F8h
    REG32b(pI2C, SOC_REG_I2C_MI2_CNTR) = EDDI_SYS_SOC_I2C_CMD_STOP;                         //Stop
    EDDI_SYS_SOC_I2C_WAIT_2US(hDDB);
    EDDI_SYS_SOC_I2C_WAIT_STATUS(pI2C, ToCnt, StatusReg, EDDI_SYS_SOC_I2C_STATE_IDLE);      //No relevant status information, IFLG=0
    if (!ToCnt) //Timeout elapsed?
    {
        return EDD_STS_ERR_PROTOCOL;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#else

void  EDDI_SysSocI2cDummy( void )
{
}

#endif //EDDI_CFG_SOC


/*****************************************************************************/
/*  end of file eddi_sys_soc_i2c.c                                           */
/*****************************************************************************/

