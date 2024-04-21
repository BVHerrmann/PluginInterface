#ifndef EDDI_I2C_H              //reinclude-protection
#define EDDI_I2C_H

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
/*  F i l e               &F: eddi_i2c.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI I2C functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/

/*===========================================================================*/
/*                              macros                                       */
/*===========================================================================*/
#define EDDI_ENTER_I2C_S()                  EDDI_ENTER_I2C()
#define EDDI_EXIT_I2C_S()                   EDDI_EXIT_I2C()

#define EDDI_I2C_SDA_HIGHZ()                (EDDI_I2C_SDA_LOW_HIGHZ(pDDB->hSysDev, EDDI_I2C_SWITCH_PORT_ON))
#define EDDI_I2C_SDA_LOW()                  (EDDI_I2C_SDA_LOW_HIGHZ(pDDB->hSysDev, EDDI_I2C_SWITCH_PORT_OFF))
#define EDDI_I2C_CLK_HIGHZ()                (EDDI_I2C_SCL_LOW_HIGHZ(pDDB->hSysDev, EDDI_I2C_SWITCH_PORT_ON))
#define EDDI_I2C_CLK_LOW()                  (EDDI_I2C_SCL_LOW_HIGHZ(pDDB->hSysDev, EDDI_I2C_SWITCH_PORT_OFF))

#define EDDI_I2C_SDA_READ_BIT(pReadValue)   (EDDI_I2C_SDA_READ(pDDB->hSysDev, pReadValue))

#define EDDI_I2C_WAIT_US(Count)             (EDDI_WAIT_10_NS(pDDB->hSysDev, (100UL * Count)))

/*===========================================================================*/
/*                              structures                                   */
/*===========================================================================*/
typedef struct _EDDI_DDB_COMP_I2C_TYPE
{
    LSA_UINT32      I2C_Type;                                       //EDDI_I2C_TYPE_NONE or EDDI_I2C_TYPE_GPIO or EDDI_I2C_TYPE_SOC1_HW
    LSA_UINT16      I2CMuxSelectStored;                             //stored I2CMuxSelect from last I2C transfer (0 = invalid)
    LSA_BOOL        bI2CMuxSelectIgnore;                            //for test purposes, ignore I2CMuxSelectStored and call EDDP_I2C_SELECT() every time
    LSA_BOOL        bI2CAlreadyInitialized[EDD_CFG_MAX_PORT_CNT+1]; //boolean indicates whether I2C device is already initialized (index = PortId)

} EDDI_DDB_COMP_I2C_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_I2C_H


/*****************************************************************************/
/*  end of file eddi_i2c.h                                                   */
/*****************************************************************************/
