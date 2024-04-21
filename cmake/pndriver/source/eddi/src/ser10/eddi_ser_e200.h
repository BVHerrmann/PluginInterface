#ifndef EDDI_SER_E200_H         //reinclude-protection
#define EDDI_SER_E200_H

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
/*  F i l e               &F: eddi_ser_e200.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                       internal Register of SER10                          */
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
/*                                 typedefs                                  */
/*===========================================================================*/

/********************************/
/********** ERTEC 200 ***********/
/********************************/

/**** System-Control-Register-Block ****/
#if defined (EDDI_CFG_ERTEC_200)

//Config_Reg
#define ERTEC200_AUX_CONFIG_REG_BIT__Reserved1                EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_1                 EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_2                 EDDI_BIT_MASK_PARA( 2, 2)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_3                 EDDI_BIT_MASK_PARA( 3, 3)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_4                 EDDI_BIT_MASK_PARA( 4, 4)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_5                 EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_6                 EDDI_BIT_MASK_PARA( 6, 6)
#define ERTEC200_AUX_CONFIG_REG_BIT__Reserved2                EDDI_BIT_MASK_PARA(31, 7)

//PHY_CONFIG
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB                  EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_FX_MODE                  EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE                 EDDI_BIT_MASK_PARA( 4, 2)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN              EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC200_AUX_PHY_CONFIG_BIT__res0                        EDDI_BIT_MASK_PARA( 7, 6)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB                  EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_FX_MODE                  EDDI_BIT_MASK_PARA( 9, 9)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE                 EDDI_BIT_MASK_PARA(12,10)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN              EDDI_BIT_MASK_PARA(13,13)
#define ERTEC200_AUX_PHY_CONFIG_BIT__res1                        EDDI_BIT_MASK_PARA(15,14)
#define ERTEC200_AUX_PHY_CONFIG_BIT__PHY_RES_SEL                 EDDI_BIT_MASK_PARA(16,16)
#define ERTEC200_AUX_PHY_CONFIG_BIT__res2                        EDDI_BIT_MASK_PARA(31,17)

//PHY_STATUS
#define ERTEC200_AUX_PHY_STATUS_BIT__P1_PWRUPRST              EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_PHY_STATUS_BIT__res0                     EDDI_BIT_MASK_PARA( 7, 1)
#define ERTEC200_AUX_PHY_STATUS_BIT__P2_PWRUPRST              EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC200_AUX_PHY_STATUS_BIT__res1                     EDDI_BIT_MASK_PARA(15, 9)
#define ERTEC200_AUX_PHY_STATUS_BIT__res3                     EDDI_BIT_MASK_PARA(31,16)

#endif //EDDI_CFG_ERTEC_200

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_E200_H


/*****************************************************************************/
/*  end of file eddi_ser_e200.h                                              */
/*****************************************************************************/

