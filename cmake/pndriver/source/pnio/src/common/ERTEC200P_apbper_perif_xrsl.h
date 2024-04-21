#ifndef ERTEC200P_APBPER_PERIF_XRSL_H
#define ERTEC200P_APBPER_PERIF_XRSL_H

#ifdef __cplusplus
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
/*  C o m p o n e n t     &C: PNExplorer                                :C&  */
/*                                                                           */
/*  F i l e               &F: ERTEC200P_apbper_perif_xrsl.h             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  P a r a m e t e r s     : Version = 2, Ports = 2, Interface = 1 */
/*****************************************************************************/


/**
@defgroup       CR_ADDRESS_1 CR_ADDRESS_1
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000000
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1              0x00000000

/**
@defgroup       CR_ADDRESS_1__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_1
@brief          CR_StartAddress  
@param Address  0x00000000
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_1__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_1
@brief          CR_EndAddress 
@param Address  0x00000000
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_1__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_1
@brief          Read returns zero data 
@param Address  0x00000000
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_1__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_1
@brief          New_Data_INT 
@param Address  0x00000000
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_1_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_2 CR_ADDRESS_2
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000004
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2              0x00000004

/**
@defgroup       CR_ADDRESS_2__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_2
@brief          CR_StartAddress  
@param Address  0x00000004
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_2__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_2
@brief          CR_EndAddress 
@param Address  0x00000004
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_2__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_2
@brief          Read returns zero data 
@param Address  0x00000004
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_2__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_2
@brief          New_Data_INT 
@param Address  0x00000004
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_2_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_3 CR_ADDRESS_3
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000008
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3              0x00000008

/**
@defgroup       CR_ADDRESS_3__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_3
@brief          CR_StartAddress  
@param Address  0x00000008
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_3__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_3
@brief          CR_EndAddress 
@param Address  0x00000008
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_3__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_3
@brief          Read returns zero data 
@param Address  0x00000008
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_3__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_3
@brief          New_Data_INT 
@param Address  0x00000008
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_3_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_4 CR_ADDRESS_4
@ingroup        Register
@brief          CR_Address  
@param Address  0x0000000C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4              0x0000000C

/**
@defgroup       CR_ADDRESS_4__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_4
@brief          CR_StartAddress  
@param Address  0x0000000C
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_4__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_4
@brief          CR_EndAddress 
@param Address  0x0000000C
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_4__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_4
@brief          Read returns zero data 
@param Address  0x0000000C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_4__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_4
@brief          New_Data_INT 
@param Address  0x0000000C
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_4_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_5 CR_ADDRESS_5
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000010
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5              0x00000010

/**
@defgroup       CR_ADDRESS_5__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_5
@brief          CR_StartAddress  
@param Address  0x00000010
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_5__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_5
@brief          CR_EndAddress 
@param Address  0x00000010
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_5__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_5
@brief          Read returns zero data 
@param Address  0x00000010
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_5__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_5
@brief          New_Data_INT 
@param Address  0x00000010
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_5_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_6 CR_ADDRESS_6
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000014
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6              0x00000014

/**
@defgroup       CR_ADDRESS_6__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_6
@brief          CR_StartAddress  
@param Address  0x00000014
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_6__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_6
@brief          CR_EndAddress 
@param Address  0x00000014
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_6__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_6
@brief          Read returns zero data 
@param Address  0x00000014
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_6__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_6
@brief          New_Data_INT 
@param Address  0x00000014
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_6_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_7 CR_ADDRESS_7
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000018
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7              0x00000018

/**
@defgroup       CR_ADDRESS_7__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_7
@brief          CR_StartAddress  
@param Address  0x00000018
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_7__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_7
@brief          CR_EndAddress 
@param Address  0x00000018
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_7__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_7
@brief          Read returns zero data 
@param Address  0x00000018
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_7__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_7
@brief          New_Data_INT 
@param Address  0x00000018
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_7_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_8 CR_ADDRESS_8
@ingroup        Register
@brief          CR_Address  
@param Address  0x0000001C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8              0x0000001C

/**
@defgroup       CR_ADDRESS_8__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_8
@brief          CR_StartAddress  
@param Address  0x0000001C
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_8__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_8
@brief          CR_EndAddress 
@param Address  0x0000001C
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_8__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_8
@brief          Read returns zero data 
@param Address  0x0000001C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_8__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_8
@brief          New_Data_INT 
@param Address  0x0000001C
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_8_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_9 CR_ADDRESS_9
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000020
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9              0x00000020

/**
@defgroup       CR_ADDRESS_9__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_9
@brief          CR_StartAddress  
@param Address  0x00000020
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_9__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_9
@brief          CR_EndAddress 
@param Address  0x00000020
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_9__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_9
@brief          Read returns zero data 
@param Address  0x00000020
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_9__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_9
@brief          New_Data_INT 
@param Address  0x00000020
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_9_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_10 CR_ADDRESS_10
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000024
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10              0x00000024

/**
@defgroup       CR_ADDRESS_10__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_10
@brief          CR_StartAddress  
@param Address  0x00000024
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_10__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_10
@brief          CR_EndAddress 
@param Address  0x00000024
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_10__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_10
@brief          Read returns zero data 
@param Address  0x00000024
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_10__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_10
@brief          New_Data_INT 
@param Address  0x00000024
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_10_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_11 CR_ADDRESS_11
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000028
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11              0x00000028

/**
@defgroup       CR_ADDRESS_11__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_11
@brief          CR_StartAddress  
@param Address  0x00000028
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_11__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_11
@brief          CR_EndAddress 
@param Address  0x00000028
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_11__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_11
@brief          Read returns zero data 
@param Address  0x00000028
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_11__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_11
@brief          New_Data_INT 
@param Address  0x00000028
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_11_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_12 CR_ADDRESS_12
@ingroup        Register
@brief          CR_Address  
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12              0x0000002C

/**
@defgroup       CR_ADDRESS_12__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_12
@brief          CR_StartAddress  
@param Address  0x0000002C
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_12__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_12
@brief          CR_EndAddress 
@param Address  0x0000002C
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_12__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_12
@brief          Read returns zero data 
@param Address  0x0000002C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_12__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_12
@brief          New_Data_INT 
@param Address  0x0000002C
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_12_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_13 CR_ADDRESS_13
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000030
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13              0x00000030

/**
@defgroup       CR_ADDRESS_13__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_13
@brief          CR_StartAddress  
@param Address  0x00000030
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_13__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_13
@brief          CR_EndAddress 
@param Address  0x00000030
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_13__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_13
@brief          Read returns zero data 
@param Address  0x00000030
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_13__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_13
@brief          New_Data_INT 
@param Address  0x00000030
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_13_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_14 CR_ADDRESS_14
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000034
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14              0x00000034

/**
@defgroup       CR_ADDRESS_14__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_14
@brief          CR_StartAddress  
@param Address  0x00000034
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_14__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_14
@brief          CR_EndAddress 
@param Address  0x00000034
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_14__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_14
@brief          Read returns zero data 
@param Address  0x00000034
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_14__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_14
@brief          New_Data_INT 
@param Address  0x00000034
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_14_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_15 CR_ADDRESS_15
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000038
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15              0x00000038

/**
@defgroup       CR_ADDRESS_15__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_15
@brief          CR_StartAddress  
@param Address  0x00000038
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_15__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_15
@brief          CR_EndAddress 
@param Address  0x00000038
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_15__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_15
@brief          Read returns zero data 
@param Address  0x00000038
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_15__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_15
@brief          New_Data_INT 
@param Address  0x00000038
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_15_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_16 CR_ADDRESS_16
@ingroup        Register
@brief          CR_Address  
@param Address  0x0000003C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16              0x0000003C

/**
@defgroup       CR_ADDRESS_16__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_16
@brief          CR_StartAddress  
@param Address  0x0000003C
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_16__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_16
@brief          CR_EndAddress 
@param Address  0x0000003C
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_16__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_16
@brief          Read returns zero data 
@param Address  0x0000003C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_16__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_16
@brief          New_Data_INT 
@param Address  0x0000003C
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_16_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_17 CR_ADDRESS_17
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000040
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17              0x00000040

/**
@defgroup       CR_ADDRESS_17__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_17
@brief          CR_StartAddress  
@param Address  0x00000040
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_17__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_17
@brief          CR_EndAddress 
@param Address  0x00000040
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_17__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_17
@brief          Read returns zero data 
@param Address  0x00000040
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_17__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_17
@brief          New_Data_INT 
@param Address  0x00000040
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_17_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_18 CR_ADDRESS_18
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000044
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18              0x00000044

/**
@defgroup       CR_ADDRESS_18__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_18
@brief          CR_StartAddress  
@param Address  0x00000044
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_18__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_18
@brief          CR_EndAddress 
@param Address  0x00000044
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_18__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_18
@brief          Read returns zero data 
@param Address  0x00000044
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_18__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_18
@brief          New_Data_INT 
@param Address  0x00000044
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_18_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_19 CR_ADDRESS_19
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000048
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19              0x00000048

/**
@defgroup       CR_ADDRESS_19__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_19
@brief          CR_StartAddress  
@param Address  0x00000048
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_19__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_19
@brief          CR_EndAddress 
@param Address  0x00000048
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_19__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_19
@brief          Read returns zero data 
@param Address  0x00000048
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_19__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_19
@brief          New_Data_INT 
@param Address  0x00000048
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_19_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_20 CR_ADDRESS_20
@ingroup        Register
@brief          CR_Address  
@param Address  0x0000004C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20              0x0000004C

/**
@defgroup       CR_ADDRESS_20__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_20
@brief          CR_StartAddress  
@param Address  0x0000004C
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_20__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_20
@brief          CR_EndAddress 
@param Address  0x0000004C
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_20__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_20
@brief          Read returns zero data 
@param Address  0x0000004C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_20__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_20
@brief          New_Data_INT 
@param Address  0x0000004C
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_20_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_21 CR_ADDRESS_21
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000050
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21              0x00000050

/**
@defgroup       CR_ADDRESS_21__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_21
@brief          CR_StartAddress  
@param Address  0x00000050
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_21__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_21
@brief          CR_EndAddress 
@param Address  0x00000050
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_21__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_21
@brief          Read returns zero data 
@param Address  0x00000050
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_21__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_21
@brief          New_Data_INT 
@param Address  0x00000050
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_21_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_22 CR_ADDRESS_22
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000054
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22              0x00000054

/**
@defgroup       CR_ADDRESS_22__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_22
@brief          CR_StartAddress  
@param Address  0x00000054
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_22__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_22
@brief          CR_EndAddress 
@param Address  0x00000054
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_22__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_22
@brief          Read returns zero data 
@param Address  0x00000054
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_22__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_22
@brief          New_Data_INT 
@param Address  0x00000054
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_22_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_23 CR_ADDRESS_23
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000058
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23              0x00000058

/**
@defgroup       CR_ADDRESS_23__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_23
@brief          CR_StartAddress  
@param Address  0x00000058
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_23__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_23
@brief          CR_EndAddress 
@param Address  0x00000058
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_23__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_23
@brief          Read returns zero data 
@param Address  0x00000058
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_23__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_23
@brief          New_Data_INT 
@param Address  0x00000058
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_23_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_24 CR_ADDRESS_24
@ingroup        Register
@brief          CR_Address  
@param Address  0x0000005C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24              0x0000005C

/**
@defgroup       CR_ADDRESS_24__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_24
@brief          CR_StartAddress  
@param Address  0x0000005C
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_24__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_24
@brief          CR_EndAddress 
@param Address  0x0000005C
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_24__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_24
@brief          Read returns zero data 
@param Address  0x0000005C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_24__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_24
@brief          New_Data_INT 
@param Address  0x0000005C
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_24_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_25 CR_ADDRESS_25
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000060
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25              0x00000060

/**
@defgroup       CR_ADDRESS_25__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_25
@brief          CR_StartAddress  
@param Address  0x00000060
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_25__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_25
@brief          CR_EndAddress 
@param Address  0x00000060
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_25__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_25
@brief          Read returns zero data 
@param Address  0x00000060
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_25__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_25
@brief          New_Data_INT 
@param Address  0x00000060
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_25_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_26 CR_ADDRESS_26
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000064
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26              0x00000064

/**
@defgroup       CR_ADDRESS_26__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_26
@brief          CR_StartAddress  
@param Address  0x00000064
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_26__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_26
@brief          CR_EndAddress 
@param Address  0x00000064
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_26__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_26
@brief          Read returns zero data 
@param Address  0x00000064
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_26__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_26
@brief          New_Data_INT 
@param Address  0x00000064
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_26_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_ADDRESS_27 CR_ADDRESS_27
@ingroup        Register
@brief          CR_Address  
@param Address  0x00000068
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27              0x00000068

/**
@defgroup       CR_ADDRESS_27__CR_STARTADDRESS CR_STARTADDRESS
@ingroup        CR_ADDRESS_27
@brief          CR_StartAddress  
@param Address  0x00000068
@param Mask     0x00000FFC
@param Shift    2
@param Access   r,w
@param Reset    0x000003FF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27__MSK_CR_STARTADDRESS       0x00000FFC
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_SHFT_CR_STARTADDRESS       2
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_RSTV_CR_STARTADDRESS       0x000003FF

/**
@defgroup       CR_ADDRESS_27__CR_ENDADDRESS CR_ENDADDRESS
@ingroup        CR_ADDRESS_27
@brief          CR_EndAddress 
@param Address  0x00000068
@param Mask     0x03FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27__MSK_CR_ENDADDRESS       0x03FF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_SHFT_CR_ENDADDRESS       16
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_RSTV_CR_ENDADDRESS       0x00000000

/**
@defgroup       CR_ADDRESS_27__ZERO_DATA ZERO_DATA
@ingroup        CR_ADDRESS_27
@brief          Read returns zero data 
@param Address  0x00000068
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27__MSK_ZERO_DATA       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_SHFT_ZERO_DATA       27
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_RSTV_ZERO_DATA       0x00000000

/**
@defgroup       CR_ADDRESS_27__NEW_DATA_INT NEW_DATA_INT
@ingroup        CR_ADDRESS_27
@brief          New_Data_INT 
@param Address  0x00000068
@param Mask     0xF0000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27__MSK_NEW_DATA_INT       0xF0000000
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_SHFT_NEW_DATA_INT       28
#define ERTEC200P_APBPER_PERIF_REG_CR_ADDRESS_27_RSTV_NEW_DATA_INT       0x00000000


/**
@defgroup       CR_STATE_1 CR_STATE_1
@ingroup        Register
@brief          CR_State    
@param Address  0x00000100
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1              0x00000100

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_data_buffer
@param Address  0x00000100
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_r2_buffer   
@param Address  0x00000100
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_1__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_free_buffer
@param Address  0x00000100
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_f2_buffer
@param Address  0x00000100
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_next_buffer
@param Address  0x00000100
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_n2_buffer
@param Address  0x00000100
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_1__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_1
@brief          buffer_number_user_buffer
@param Address  0x00000100
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_1__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_1
@brief          Redundance_Mode 
@param Address  0x00000100
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_1__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_1_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_1_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_1__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_1
@brief          EXT_BUF_Mode 
@param Address  0x00000100
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_1__MAPPING MAPPING
@ingroup        CR_STATE_1
@brief          Mapping 
@param Address  0x00000100
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_1_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_2 CR_STATE_2
@ingroup        Register
@brief          CR_State    
@param Address  0x00000104
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2              0x00000104

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_data_buffer
@param Address  0x00000104
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_r2_buffer   
@param Address  0x00000104
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_2__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_free_buffer
@param Address  0x00000104
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_f2_buffer
@param Address  0x00000104
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_next_buffer
@param Address  0x00000104
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_n2_buffer
@param Address  0x00000104
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_2__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_2
@brief          buffer_number_user_buffer
@param Address  0x00000104
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_2__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_2
@brief          Redundance_Mode 
@param Address  0x00000104
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_2__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_2_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_2_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_2__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_2
@brief          EXT_BUF_Mode 
@param Address  0x00000104
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_2__MAPPING MAPPING
@ingroup        CR_STATE_2
@brief          Mapping 
@param Address  0x00000104
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_2_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_3 CR_STATE_3
@ingroup        Register
@brief          CR_State    
@param Address  0x00000108
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3              0x00000108

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_data_buffer
@param Address  0x00000108
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_r2_buffer   
@param Address  0x00000108
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_3__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_free_buffer
@param Address  0x00000108
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_f2_buffer
@param Address  0x00000108
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_next_buffer
@param Address  0x00000108
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_n2_buffer
@param Address  0x00000108
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_3__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_3
@brief          buffer_number_user_buffer
@param Address  0x00000108
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_3__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_3
@brief          Redundance_Mode 
@param Address  0x00000108
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_3__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_3_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_3_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_3__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_3
@brief          EXT_BUF_Mode 
@param Address  0x00000108
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_3__MAPPING MAPPING
@ingroup        CR_STATE_3
@brief          Mapping 
@param Address  0x00000108
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_3_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_4 CR_STATE_4
@ingroup        Register
@brief          CR_State    
@param Address  0x0000010C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4              0x0000010C

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_data_buffer
@param Address  0x0000010C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_r2_buffer   
@param Address  0x0000010C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_4__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_free_buffer
@param Address  0x0000010C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_f2_buffer
@param Address  0x0000010C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_next_buffer
@param Address  0x0000010C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_n2_buffer
@param Address  0x0000010C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_4__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_4
@brief          buffer_number_user_buffer
@param Address  0x0000010C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_4__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_4
@brief          Redundance_Mode 
@param Address  0x0000010C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_4__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_4_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_4_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_4__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_4
@brief          EXT_BUF_Mode 
@param Address  0x0000010C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_4__MAPPING MAPPING
@ingroup        CR_STATE_4
@brief          Mapping 
@param Address  0x0000010C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_4_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_5 CR_STATE_5
@ingroup        Register
@brief          CR_State    
@param Address  0x00000110
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5              0x00000110

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_data_buffer
@param Address  0x00000110
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_r2_buffer   
@param Address  0x00000110
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_5__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_free_buffer
@param Address  0x00000110
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_f2_buffer
@param Address  0x00000110
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_next_buffer
@param Address  0x00000110
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_n2_buffer
@param Address  0x00000110
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_5__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_5
@brief          buffer_number_user_buffer
@param Address  0x00000110
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_5__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_5
@brief          Redundance_Mode 
@param Address  0x00000110
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_5__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_5_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_5_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_5__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_5
@brief          EXT_BUF_Mode 
@param Address  0x00000110
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_5__MAPPING MAPPING
@ingroup        CR_STATE_5
@brief          Mapping 
@param Address  0x00000110
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_5_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_6 CR_STATE_6
@ingroup        Register
@brief          CR_State    
@param Address  0x00000114
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6              0x00000114

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_data_buffer
@param Address  0x00000114
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_r2_buffer   
@param Address  0x00000114
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_6__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_free_buffer
@param Address  0x00000114
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_f2_buffer
@param Address  0x00000114
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_next_buffer
@param Address  0x00000114
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_n2_buffer
@param Address  0x00000114
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_6__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_6
@brief          buffer_number_user_buffer
@param Address  0x00000114
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_6__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_6
@brief          Redundance_Mode 
@param Address  0x00000114
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_6__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_6_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_6_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_6__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_6
@brief          EXT_BUF_Mode 
@param Address  0x00000114
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_6__MAPPING MAPPING
@ingroup        CR_STATE_6
@brief          Mapping 
@param Address  0x00000114
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_6_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_7 CR_STATE_7
@ingroup        Register
@brief          CR_State    
@param Address  0x00000118
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7              0x00000118

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_data_buffer
@param Address  0x00000118
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_r2_buffer   
@param Address  0x00000118
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_7__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_free_buffer
@param Address  0x00000118
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_f2_buffer
@param Address  0x00000118
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_next_buffer
@param Address  0x00000118
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_n2_buffer
@param Address  0x00000118
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_7__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_7
@brief          buffer_number_user_buffer
@param Address  0x00000118
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_7__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_7
@brief          Redundance_Mode 
@param Address  0x00000118
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_7__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_7_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_7_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_7__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_7
@brief          EXT_BUF_Mode 
@param Address  0x00000118
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_7__MAPPING MAPPING
@ingroup        CR_STATE_7
@brief          Mapping 
@param Address  0x00000118
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_7_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_8 CR_STATE_8
@ingroup        Register
@brief          CR_State    
@param Address  0x0000011C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8              0x0000011C

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_data_buffer
@param Address  0x0000011C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_r2_buffer   
@param Address  0x0000011C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_8__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_free_buffer
@param Address  0x0000011C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_f2_buffer
@param Address  0x0000011C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_next_buffer
@param Address  0x0000011C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_n2_buffer
@param Address  0x0000011C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_8__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_8
@brief          buffer_number_user_buffer
@param Address  0x0000011C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_8__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_8
@brief          Redundance_Mode 
@param Address  0x0000011C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_8__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_8_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_8_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_8__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_8
@brief          EXT_BUF_Mode 
@param Address  0x0000011C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_8__MAPPING MAPPING
@ingroup        CR_STATE_8
@brief          Mapping 
@param Address  0x0000011C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_8_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_9 CR_STATE_9
@ingroup        Register
@brief          CR_State    
@param Address  0x00000120
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9              0x00000120

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_data_buffer
@param Address  0x00000120
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_r2_buffer   
@param Address  0x00000120
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_9__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_free_buffer
@param Address  0x00000120
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_f2_buffer
@param Address  0x00000120
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_next_buffer
@param Address  0x00000120
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_n2_buffer
@param Address  0x00000120
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_9__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_9
@brief          buffer_number_user_buffer
@param Address  0x00000120
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_9__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_9
@brief          Redundance_Mode 
@param Address  0x00000120
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_9__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_9_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_9_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_9__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_9
@brief          EXT_BUF_Mode 
@param Address  0x00000120
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_9__MAPPING MAPPING
@ingroup        CR_STATE_9
@brief          Mapping 
@param Address  0x00000120
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_9_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_10 CR_STATE_10
@ingroup        Register
@brief          CR_State    
@param Address  0x00000124
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10              0x00000124

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_data_buffer
@param Address  0x00000124
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_r2_buffer   
@param Address  0x00000124
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_10__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_free_buffer
@param Address  0x00000124
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_f2_buffer
@param Address  0x00000124
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_next_buffer
@param Address  0x00000124
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_n2_buffer
@param Address  0x00000124
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_10__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_10
@brief          buffer_number_user_buffer
@param Address  0x00000124
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_10__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_10
@brief          Redundance_Mode 
@param Address  0x00000124
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_10__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_10_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_10_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_10__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_10
@brief          EXT_BUF_Mode 
@param Address  0x00000124
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_10__MAPPING MAPPING
@ingroup        CR_STATE_10
@brief          Mapping 
@param Address  0x00000124
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_10_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_11 CR_STATE_11
@ingroup        Register
@brief          CR_State    
@param Address  0x00000128
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11              0x00000128

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_data_buffer
@param Address  0x00000128
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_r2_buffer   
@param Address  0x00000128
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_11__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_free_buffer
@param Address  0x00000128
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_f2_buffer
@param Address  0x00000128
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_next_buffer
@param Address  0x00000128
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_n2_buffer
@param Address  0x00000128
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_11__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_11
@brief          buffer_number_user_buffer
@param Address  0x00000128
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_11__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_11
@brief          Redundance_Mode 
@param Address  0x00000128
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_11__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_11_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_11_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_11__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_11
@brief          EXT_BUF_Mode 
@param Address  0x00000128
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_11__MAPPING MAPPING
@ingroup        CR_STATE_11
@brief          Mapping 
@param Address  0x00000128
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_11_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_12 CR_STATE_12
@ingroup        Register
@brief          CR_State    
@param Address  0x0000012C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12              0x0000012C

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_data_buffer
@param Address  0x0000012C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_r2_buffer   
@param Address  0x0000012C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_12__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_free_buffer
@param Address  0x0000012C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_f2_buffer
@param Address  0x0000012C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_next_buffer
@param Address  0x0000012C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_n2_buffer
@param Address  0x0000012C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_12__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_12
@brief          buffer_number_user_buffer
@param Address  0x0000012C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_12__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_12
@brief          Redundance_Mode 
@param Address  0x0000012C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_12__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_12_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_12_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_12__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_12
@brief          EXT_BUF_Mode 
@param Address  0x0000012C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_12__MAPPING MAPPING
@ingroup        CR_STATE_12
@brief          Mapping 
@param Address  0x0000012C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_12_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_13 CR_STATE_13
@ingroup        Register
@brief          CR_State    
@param Address  0x00000130
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13              0x00000130

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_data_buffer
@param Address  0x00000130
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_r2_buffer   
@param Address  0x00000130
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_13__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_free_buffer
@param Address  0x00000130
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_f2_buffer
@param Address  0x00000130
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_next_buffer
@param Address  0x00000130
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_n2_buffer
@param Address  0x00000130
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_13__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_13
@brief          buffer_number_user_buffer
@param Address  0x00000130
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_13__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_13
@brief          Redundance_Mode 
@param Address  0x00000130
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_13__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_13_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_13_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_13__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_13
@brief          EXT_BUF_Mode 
@param Address  0x00000130
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_13__MAPPING MAPPING
@ingroup        CR_STATE_13
@brief          Mapping 
@param Address  0x00000130
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_13_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_14 CR_STATE_14
@ingroup        Register
@brief          CR_State    
@param Address  0x00000134
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14              0x00000134

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_data_buffer
@param Address  0x00000134
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_r2_buffer   
@param Address  0x00000134
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_14__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_free_buffer
@param Address  0x00000134
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_f2_buffer
@param Address  0x00000134
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_next_buffer
@param Address  0x00000134
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_n2_buffer
@param Address  0x00000134
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_14__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_14
@brief          buffer_number_user_buffer
@param Address  0x00000134
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_14__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_14
@brief          Redundance_Mode 
@param Address  0x00000134
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_14__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_14_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_14_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_14__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_14
@brief          EXT_BUF_Mode 
@param Address  0x00000134
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_14__MAPPING MAPPING
@ingroup        CR_STATE_14
@brief          Mapping 
@param Address  0x00000134
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_14_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_15 CR_STATE_15
@ingroup        Register
@brief          CR_State    
@param Address  0x00000138
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15              0x00000138

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_data_buffer
@param Address  0x00000138
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_r2_buffer   
@param Address  0x00000138
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_15__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_free_buffer
@param Address  0x00000138
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_f2_buffer
@param Address  0x00000138
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_next_buffer
@param Address  0x00000138
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_n2_buffer
@param Address  0x00000138
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_15__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_15
@brief          buffer_number_user_buffer
@param Address  0x00000138
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_15__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_15
@brief          Redundance_Mode 
@param Address  0x00000138
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_15__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_15_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_15_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_15__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_15
@brief          EXT_BUF_Mode 
@param Address  0x00000138
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_15__MAPPING MAPPING
@ingroup        CR_STATE_15
@brief          Mapping 
@param Address  0x00000138
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_15_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_16 CR_STATE_16
@ingroup        Register
@brief          CR_State    
@param Address  0x0000013C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16              0x0000013C

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_data_buffer
@param Address  0x0000013C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_r2_buffer   
@param Address  0x0000013C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_16__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_free_buffer
@param Address  0x0000013C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_f2_buffer
@param Address  0x0000013C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_next_buffer
@param Address  0x0000013C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_n2_buffer
@param Address  0x0000013C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_16__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_16
@brief          buffer_number_user_buffer
@param Address  0x0000013C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_16__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_16
@brief          Redundance_Mode 
@param Address  0x0000013C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_16__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_16_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_16_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_16__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_16
@brief          EXT_BUF_Mode 
@param Address  0x0000013C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_16__MAPPING MAPPING
@ingroup        CR_STATE_16
@brief          Mapping 
@param Address  0x0000013C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_16_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_17 CR_STATE_17
@ingroup        Register
@brief          CR_State    
@param Address  0x00000140
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17              0x00000140

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_data_buffer
@param Address  0x00000140
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_r2_buffer   
@param Address  0x00000140
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_17__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_free_buffer
@param Address  0x00000140
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_f2_buffer
@param Address  0x00000140
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_next_buffer
@param Address  0x00000140
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_n2_buffer
@param Address  0x00000140
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_17__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_17
@brief          buffer_number_user_buffer
@param Address  0x00000140
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_17__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_17
@brief          Redundance_Mode 
@param Address  0x00000140
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_17__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_17_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_17_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_17__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_17
@brief          EXT_BUF_Mode 
@param Address  0x00000140
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_17__MAPPING MAPPING
@ingroup        CR_STATE_17
@brief          Mapping 
@param Address  0x00000140
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_17_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_18 CR_STATE_18
@ingroup        Register
@brief          CR_State    
@param Address  0x00000144
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18              0x00000144

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_data_buffer
@param Address  0x00000144
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_r2_buffer   
@param Address  0x00000144
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_18__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_free_buffer
@param Address  0x00000144
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_f2_buffer
@param Address  0x00000144
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_next_buffer
@param Address  0x00000144
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_n2_buffer
@param Address  0x00000144
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_18__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_18
@brief          buffer_number_user_buffer
@param Address  0x00000144
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_18__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_18
@brief          Redundance_Mode 
@param Address  0x00000144
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_18__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_18_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_18_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_18__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_18
@brief          EXT_BUF_Mode 
@param Address  0x00000144
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_18__MAPPING MAPPING
@ingroup        CR_STATE_18
@brief          Mapping 
@param Address  0x00000144
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_18_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_19 CR_STATE_19
@ingroup        Register
@brief          CR_State    
@param Address  0x00000148
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19              0x00000148

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_data_buffer
@param Address  0x00000148
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_r2_buffer   
@param Address  0x00000148
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_19__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_free_buffer
@param Address  0x00000148
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_f2_buffer
@param Address  0x00000148
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_next_buffer
@param Address  0x00000148
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_n2_buffer
@param Address  0x00000148
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_19__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_19
@brief          buffer_number_user_buffer
@param Address  0x00000148
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_19__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_19
@brief          Redundance_Mode 
@param Address  0x00000148
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_19__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_19_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_19_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_19__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_19
@brief          EXT_BUF_Mode 
@param Address  0x00000148
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_19__MAPPING MAPPING
@ingroup        CR_STATE_19
@brief          Mapping 
@param Address  0x00000148
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_19_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_20 CR_STATE_20
@ingroup        Register
@brief          CR_State    
@param Address  0x0000014C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20              0x0000014C

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_data_buffer
@param Address  0x0000014C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_r2_buffer   
@param Address  0x0000014C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_20__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_free_buffer
@param Address  0x0000014C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_f2_buffer
@param Address  0x0000014C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_next_buffer
@param Address  0x0000014C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_n2_buffer
@param Address  0x0000014C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_20__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_20
@brief          buffer_number_user_buffer
@param Address  0x0000014C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_20__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_20
@brief          Redundance_Mode 
@param Address  0x0000014C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_20__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_20_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_20_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_20__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_20
@brief          EXT_BUF_Mode 
@param Address  0x0000014C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_20__MAPPING MAPPING
@ingroup        CR_STATE_20
@brief          Mapping 
@param Address  0x0000014C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_20_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_21 CR_STATE_21
@ingroup        Register
@brief          CR_State    
@param Address  0x00000150
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21              0x00000150

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_data_buffer
@param Address  0x00000150
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_r2_buffer   
@param Address  0x00000150
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_21__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_free_buffer
@param Address  0x00000150
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_f2_buffer
@param Address  0x00000150
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_next_buffer
@param Address  0x00000150
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_n2_buffer
@param Address  0x00000150
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_21__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_21
@brief          buffer_number_user_buffer
@param Address  0x00000150
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_21__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_21
@brief          Redundance_Mode 
@param Address  0x00000150
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_21__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_21_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_21_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_21__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_21
@brief          EXT_BUF_Mode 
@param Address  0x00000150
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_21__MAPPING MAPPING
@ingroup        CR_STATE_21
@brief          Mapping 
@param Address  0x00000150
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_21_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_22 CR_STATE_22
@ingroup        Register
@brief          CR_State    
@param Address  0x00000154
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22              0x00000154

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_data_buffer
@param Address  0x00000154
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_r2_buffer   
@param Address  0x00000154
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_22__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_free_buffer
@param Address  0x00000154
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_f2_buffer
@param Address  0x00000154
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_next_buffer
@param Address  0x00000154
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_n2_buffer
@param Address  0x00000154
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_22__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_22
@brief          buffer_number_user_buffer
@param Address  0x00000154
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_22__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_22
@brief          Redundance_Mode 
@param Address  0x00000154
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_22__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_22_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_22_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_22__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_22
@brief          EXT_BUF_Mode 
@param Address  0x00000154
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_22__MAPPING MAPPING
@ingroup        CR_STATE_22
@brief          Mapping 
@param Address  0x00000154
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_22_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_23 CR_STATE_23
@ingroup        Register
@brief          CR_State    
@param Address  0x00000158
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23              0x00000158

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_data_buffer
@param Address  0x00000158
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_r2_buffer   
@param Address  0x00000158
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_23__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_free_buffer
@param Address  0x00000158
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_f2_buffer
@param Address  0x00000158
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_next_buffer
@param Address  0x00000158
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_n2_buffer
@param Address  0x00000158
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_23__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_23
@brief          buffer_number_user_buffer
@param Address  0x00000158
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_23__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_23
@brief          Redundance_Mode 
@param Address  0x00000158
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_23__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_23_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_23_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_23__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_23
@brief          EXT_BUF_Mode 
@param Address  0x00000158
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_23__MAPPING MAPPING
@ingroup        CR_STATE_23
@brief          Mapping 
@param Address  0x00000158
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_23_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_24 CR_STATE_24
@ingroup        Register
@brief          CR_State    
@param Address  0x0000015C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24              0x0000015C

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_data_buffer
@param Address  0x0000015C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_r2_buffer   
@param Address  0x0000015C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_24__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_free_buffer
@param Address  0x0000015C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_f2_buffer
@param Address  0x0000015C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_next_buffer
@param Address  0x0000015C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_n2_buffer
@param Address  0x0000015C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_24__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_24
@brief          buffer_number_user_buffer
@param Address  0x0000015C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_24__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_24
@brief          Redundance_Mode 
@param Address  0x0000015C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_24__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_24_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_24_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_24__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_24
@brief          EXT_BUF_Mode 
@param Address  0x0000015C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_24__MAPPING MAPPING
@ingroup        CR_STATE_24
@brief          Mapping 
@param Address  0x0000015C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_24_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_25 CR_STATE_25
@ingroup        Register
@brief          CR_State    
@param Address  0x00000160
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25              0x00000160

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_data_buffer
@param Address  0x00000160
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_r2_buffer   
@param Address  0x00000160
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_25__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_free_buffer
@param Address  0x00000160
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_f2_buffer
@param Address  0x00000160
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_next_buffer
@param Address  0x00000160
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_n2_buffer
@param Address  0x00000160
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_25__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_25
@brief          buffer_number_user_buffer
@param Address  0x00000160
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_25__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_25
@brief          Redundance_Mode 
@param Address  0x00000160
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_25__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_25_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_25_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_25__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_25
@brief          EXT_BUF_Mode 
@param Address  0x00000160
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_25__MAPPING MAPPING
@ingroup        CR_STATE_25
@brief          Mapping 
@param Address  0x00000160
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_25_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_26 CR_STATE_26
@ingroup        Register
@brief          CR_State    
@param Address  0x00000164
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26              0x00000164

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_data_buffer
@param Address  0x00000164
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_r2_buffer   
@param Address  0x00000164
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_26__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_free_buffer
@param Address  0x00000164
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_f2_buffer
@param Address  0x00000164
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_next_buffer
@param Address  0x00000164
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_n2_buffer
@param Address  0x00000164
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_26__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_26
@brief          buffer_number_user_buffer
@param Address  0x00000164
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_26__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_26
@brief          Redundance_Mode 
@param Address  0x00000164
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_26__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_26_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_26_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_26__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_26
@brief          EXT_BUF_Mode 
@param Address  0x00000164
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_26__MAPPING MAPPING
@ingroup        CR_STATE_26
@brief          Mapping 
@param Address  0x00000164
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_26_RSTV_MAPPING       0x00000000


/**
@defgroup       CR_STATE_27 CR_STATE_27
@ingroup        Register
@brief          CR_State    
@param Address  0x00000168
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27              0x00000168

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_DATA_BUFFER BUFFER_NUMBER_DATA_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_data_buffer
@param Address  0x00000168
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_BUFFER_NUMBER_DATA_BUFFER       0x00000003
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_DATA_BUFFER       0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_DATA_BUFFER       0x00000002

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_R2_BUFFER BUFFER_NUMBER_R2_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_r2_buffer   
@param Address  0x00000168
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_27__MSK_BUFFER_NUMBER_R2_BUFFER       0x0000000C
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_R2_BUFFER       2
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_R2_BUFFER       0x00000000

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_FREE_BUFFER BUFFER_NUMBER_FREE_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_free_buffer
@param Address  0x00000168
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_BUFFER_NUMBER_FREE_BUFFER       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_FREE_BUFFER       4
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_FREE_BUFFER       0x00000001

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_F2_BUFFER BUFFER_NUMBER_F2_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_f2_buffer
@param Address  0x00000168
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_BUFFER_NUMBER_F2_BUFFER       0x000000C0
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_F2_BUFFER       6
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_F2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_NEXT_BUFFER BUFFER_NUMBER_NEXT_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_next_buffer
@param Address  0x00000168
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_BUFFER_NUMBER_NEXT_BUFFER       0x00000300
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_NEXT_BUFFER       8
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_NEXT_BUFFER       0x00000003

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_N2_BUFFER BUFFER_NUMBER_N2_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_n2_buffer
@param Address  0x00000168
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_BUFFER_NUMBER_N2_BUFFER       0x00000C00
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_N2_BUFFER       10
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_N2_BUFFER       0x00000003

/**
@defgroup       CR_STATE_27__BUFFER_NUMBER_USER_BUFFER BUFFER_NUMBER_USER_BUFFER
@ingroup        CR_STATE_27
@brief          buffer_number_user_buffer
@param Address  0x00000168
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_BUFFER_NUMBER_USER_BUFFER       0x00003000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_BUFFER_NUMBER_USER_BUFFER       12
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_BUFFER_NUMBER_USER_BUFFER       0x00000000

/**
@defgroup       CR_STATE_27__REDUNDANCE_MODE REDUNDANCE_MODE
@ingroup        CR_STATE_27
@brief          Redundance_Mode 
@param Address  0x00000168
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_27__MSK_REDUNDANCE_MODE       0x00004000
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_27_SHFT_REDUNDANCE_MODE       14
#define ERTEC200P_APBPER_PERIF_R2_REG_CR_STATE_27_RSTV_REDUNDANCE_MODE       0x00000000

/**
@defgroup       CR_STATE_27__EXT_BUF_MODE EXT_BUF_MODE
@ingroup        CR_STATE_27
@brief          EXT_BUF_Mode 
@param Address  0x00000168
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_EXT_BUF_MODE       0x00008000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_EXT_BUF_MODE       15
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_EXT_BUF_MODE       0x00000000

/**
@defgroup       CR_STATE_27__MAPPING MAPPING
@ingroup        CR_STATE_27
@brief          Mapping 
@param Address  0x00000168
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27__MSK_MAPPING       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_SHFT_MAPPING       16
#define ERTEC200P_APBPER_PERIF_REG_CR_STATE_27_RSTV_MAPPING       0x00000000


/**
@defgroup       GUARD_CONTROL_1 GUARD_CONTROL_1
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000200
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1              0x00000200

/**
@defgroup       GUARD_CONTROL_1__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_1
@brief          
@param Address  0x00000200
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_1__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_1
@brief          Guard_Type
@param Address  0x00000200
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_1__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_1
@brief          Guard_Valid
@param Address  0x00000200
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_1_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_2 GUARD_CONTROL_2
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000204
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2              0x00000204

/**
@defgroup       GUARD_CONTROL_2__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_2
@brief          
@param Address  0x00000204
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_2__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_2
@brief          Guard_Type
@param Address  0x00000204
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_2__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_2
@brief          Guard_Valid
@param Address  0x00000204
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_2_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_3 GUARD_CONTROL_3
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000208
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3              0x00000208

/**
@defgroup       GUARD_CONTROL_3__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_3
@brief          
@param Address  0x00000208
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_3__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_3
@brief          Guard_Type
@param Address  0x00000208
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_3__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_3
@brief          Guard_Valid
@param Address  0x00000208
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_3_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_4 GUARD_CONTROL_4
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x0000020C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4              0x0000020C

/**
@defgroup       GUARD_CONTROL_4__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_4
@brief          
@param Address  0x0000020C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_4__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_4
@brief          Guard_Type
@param Address  0x0000020C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_4__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_4
@brief          Guard_Valid
@param Address  0x0000020C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_4_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_5 GUARD_CONTROL_5
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000210
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5              0x00000210

/**
@defgroup       GUARD_CONTROL_5__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_5
@brief          
@param Address  0x00000210
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_5__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_5
@brief          Guard_Type
@param Address  0x00000210
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_5__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_5
@brief          Guard_Valid
@param Address  0x00000210
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_5_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_6 GUARD_CONTROL_6
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000214
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6              0x00000214

/**
@defgroup       GUARD_CONTROL_6__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_6
@brief          
@param Address  0x00000214
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_6__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_6
@brief          Guard_Type
@param Address  0x00000214
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_6__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_6
@brief          Guard_Valid
@param Address  0x00000214
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_6_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_7 GUARD_CONTROL_7
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000218
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7              0x00000218

/**
@defgroup       GUARD_CONTROL_7__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_7
@brief          
@param Address  0x00000218
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_7__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_7
@brief          Guard_Type
@param Address  0x00000218
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_7__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_7
@brief          Guard_Valid
@param Address  0x00000218
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_7_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_8 GUARD_CONTROL_8
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x0000021C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8              0x0000021C

/**
@defgroup       GUARD_CONTROL_8__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_8
@brief          
@param Address  0x0000021C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_8__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_8
@brief          Guard_Type
@param Address  0x0000021C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_8__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_8
@brief          Guard_Valid
@param Address  0x0000021C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_8_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_9 GUARD_CONTROL_9
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000220
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9              0x00000220

/**
@defgroup       GUARD_CONTROL_9__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_9
@brief          
@param Address  0x00000220
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_9__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_9
@brief          Guard_Type
@param Address  0x00000220
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_9__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_9
@brief          Guard_Valid
@param Address  0x00000220
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_9_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_10 GUARD_CONTROL_10
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000224
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10              0x00000224

/**
@defgroup       GUARD_CONTROL_10__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_10
@brief          
@param Address  0x00000224
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_10__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_10
@brief          Guard_Type
@param Address  0x00000224
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_10__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_10
@brief          Guard_Valid
@param Address  0x00000224
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_10_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_11 GUARD_CONTROL_11
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000228
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11              0x00000228

/**
@defgroup       GUARD_CONTROL_11__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_11
@brief          
@param Address  0x00000228
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_11__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_11
@brief          Guard_Type
@param Address  0x00000228
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_11__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_11
@brief          Guard_Valid
@param Address  0x00000228
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_11_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_12 GUARD_CONTROL_12
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x0000022C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12              0x0000022C

/**
@defgroup       GUARD_CONTROL_12__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_12
@brief          
@param Address  0x0000022C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_12__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_12
@brief          Guard_Type
@param Address  0x0000022C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_12__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_12
@brief          Guard_Valid
@param Address  0x0000022C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_12_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_13 GUARD_CONTROL_13
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000230
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13              0x00000230

/**
@defgroup       GUARD_CONTROL_13__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_13
@brief          
@param Address  0x00000230
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_13__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_13
@brief          Guard_Type
@param Address  0x00000230
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_13__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_13
@brief          Guard_Valid
@param Address  0x00000230
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_13_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_14 GUARD_CONTROL_14
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000234
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14              0x00000234

/**
@defgroup       GUARD_CONTROL_14__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_14
@brief          
@param Address  0x00000234
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_14__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_14
@brief          Guard_Type
@param Address  0x00000234
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_14__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_14
@brief          Guard_Valid
@param Address  0x00000234
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_14_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_15 GUARD_CONTROL_15
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000238
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15              0x00000238

/**
@defgroup       GUARD_CONTROL_15__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_15
@brief          
@param Address  0x00000238
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_15__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_15
@brief          Guard_Type
@param Address  0x00000238
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_15__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_15
@brief          Guard_Valid
@param Address  0x00000238
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_15_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_16 GUARD_CONTROL_16
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x0000023C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16              0x0000023C

/**
@defgroup       GUARD_CONTROL_16__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_16
@brief          
@param Address  0x0000023C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_16__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_16
@brief          Guard_Type
@param Address  0x0000023C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_16__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_16
@brief          Guard_Valid
@param Address  0x0000023C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_16_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_17 GUARD_CONTROL_17
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000240
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17              0x00000240

/**
@defgroup       GUARD_CONTROL_17__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_17
@brief          
@param Address  0x00000240
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_17__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_17
@brief          Guard_Type
@param Address  0x00000240
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_17__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_17
@brief          Guard_Valid
@param Address  0x00000240
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_17_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_18 GUARD_CONTROL_18
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000244
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18              0x00000244

/**
@defgroup       GUARD_CONTROL_18__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_18
@brief          
@param Address  0x00000244
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_18__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_18
@brief          Guard_Type
@param Address  0x00000244
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_18__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_18
@brief          Guard_Valid
@param Address  0x00000244
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_18_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_19 GUARD_CONTROL_19
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000248
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19              0x00000248

/**
@defgroup       GUARD_CONTROL_19__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_19
@brief          
@param Address  0x00000248
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_19__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_19
@brief          Guard_Type
@param Address  0x00000248
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_19__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_19
@brief          Guard_Valid
@param Address  0x00000248
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_19_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_20 GUARD_CONTROL_20
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x0000024C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20              0x0000024C

/**
@defgroup       GUARD_CONTROL_20__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_20
@brief          
@param Address  0x0000024C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_20__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_20
@brief          Guard_Type
@param Address  0x0000024C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_20__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_20
@brief          Guard_Valid
@param Address  0x0000024C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_20_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_21 GUARD_CONTROL_21
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000250
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21              0x00000250

/**
@defgroup       GUARD_CONTROL_21__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_21
@brief          
@param Address  0x00000250
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_21__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_21
@brief          Guard_Type
@param Address  0x00000250
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_21__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_21
@brief          Guard_Valid
@param Address  0x00000250
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_21_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_22 GUARD_CONTROL_22
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000254
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22              0x00000254

/**
@defgroup       GUARD_CONTROL_22__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_22
@brief          
@param Address  0x00000254
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_22__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_22
@brief          Guard_Type
@param Address  0x00000254
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_22__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_22
@brief          Guard_Valid
@param Address  0x00000254
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_22_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_23 GUARD_CONTROL_23
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000258
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23              0x00000258

/**
@defgroup       GUARD_CONTROL_23__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_23
@brief          
@param Address  0x00000258
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_23__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_23
@brief          Guard_Type
@param Address  0x00000258
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_23__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_23
@brief          Guard_Valid
@param Address  0x00000258
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_23_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_24 GUARD_CONTROL_24
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x0000025C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24              0x0000025C

/**
@defgroup       GUARD_CONTROL_24__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_24
@brief          
@param Address  0x0000025C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_24__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_24
@brief          Guard_Type
@param Address  0x0000025C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_24__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_24
@brief          Guard_Valid
@param Address  0x0000025C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_24_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_25 GUARD_CONTROL_25
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000260
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25              0x00000260

/**
@defgroup       GUARD_CONTROL_25__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_25
@brief          
@param Address  0x00000260
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_25__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_25
@brief          Guard_Type
@param Address  0x00000260
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_25__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_25
@brief          Guard_Valid
@param Address  0x00000260
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_25_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_26 GUARD_CONTROL_26
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000264
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26              0x00000264

/**
@defgroup       GUARD_CONTROL_26__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_26
@brief          
@param Address  0x00000264
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_26__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_26
@brief          Guard_Type
@param Address  0x00000264
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_26__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_26
@brief          Guard_Valid
@param Address  0x00000264
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_26_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       GUARD_CONTROL_27 GUARD_CONTROL_27
@ingroup        Register
@brief          Guard_Control_1 
@param Address  0x00000268
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27              0x00000268

/**
@defgroup       GUARD_CONTROL_27__GUARD_ADDRESS GUARD_ADDRESS
@ingroup        GUARD_CONTROL_27
@brief          
@param Address  0x00000268
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27__MSK_GUARD_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27_SHFT_GUARD_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27_RSTV_GUARD_ADDRESS       0x00000000

/**
@defgroup       GUARD_CONTROL_27__GUARD_TYPE GUARD_TYPE
@ingroup        GUARD_CONTROL_27
@brief          Guard_Type
@param Address  0x00000268
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27__MSK_GUARD_TYPE       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27_SHFT_GUARD_TYPE       30
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27_RSTV_GUARD_TYPE       0x00000000

/**
@defgroup       GUARD_CONTROL_27__GUARD_VALID GUARD_VALID
@ingroup        GUARD_CONTROL_27
@brief          Guard_Valid
@param Address  0x00000268
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27__MSK_GUARD_VALID       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27_SHFT_GUARD_VALID       31
#define ERTEC200P_APBPER_PERIF_REG_GUARD_CONTROL_27_RSTV_GUARD_VALID       0x00000000


/**
@defgroup       IO_MODE IO_MODE
@ingroup        Register
@brief          IO_Mode 
@param Address  0x00000300
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE              0x00000300
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_RST__VAL     0x00000000

/**
@defgroup       IO_MODE__O_INTERFACE O_INTERFACE
@ingroup        IO_MODE
@brief          O_Interface
@param Address  0x00000300
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE__MSK_O_INTERFACE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_SHFT_O_INTERFACE       0
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_RSTV_O_INTERFACE       0x00000000

/**
@defgroup       IO_MODE__I_INTERFACE I_INTERFACE
@ingroup        IO_MODE
@brief          I_Interface
@param Address  0x00000300
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE__MSK_I_INTERFACE       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_SHFT_I_INTERFACE       1
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_RSTV_I_INTERFACE       0x00000000

/**
@defgroup       IO_MODE__IO_MAPPING IO_MAPPING
@ingroup        IO_MODE
@brief          IO_Mapping 
@param Address  0x00000300
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE__MSK_IO_MAPPING       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_SHFT_IO_MAPPING       2
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_RSTV_IO_MAPPING       0x00000000

/**
@defgroup       IO_MODE__ACCESS_MODE ACCESS_MODE
@ingroup        IO_MODE
@brief          Access_Mode
@param Address  0x00000300
@param Mask     0x00000080
@param Shift    7
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE__MSK_ACCESS_MODE       0x00000080
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_SHFT_ACCESS_MODE       7
#define ERTEC200P_APBPER_PERIF_REG_IO_MODE_RSTV_ACCESS_MODE       0x00000000


/**
@defgroup       OUTDATA_SRCADDRESS OUTDATA_SRCADDRESS
@ingroup        Register
@brief          OutData_SrcAddress 
@param Address  0x00000304
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS              0x00000304

/**
@defgroup       OUTDATA_SRCADDRESS__SRC_ADDRESS SRC_ADDRESS
@ingroup        OUTDATA_SRCADDRESS
@brief          Src_Address 
@param Address  0x00000304
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS__MSK_SRC_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS_SHFT_SRC_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS_RSTV_SRC_ADDRESS       0x00000000

/**
@defgroup       OUTDATA_SRCADDRESS__LENGTH LENGTH
@ingroup        OUTDATA_SRCADDRESS
@brief          Length
@param Address  0x00000304
@param Mask     0x07FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS__MSK_LENGTH       0x07FF0000
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS_SHFT_LENGTH       16
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS_RSTV_LENGTH       0x00000000

/**
@defgroup       OUTDATA_SRCADDRESS__CR_NUMBER CR_NUMBER
@ingroup        OUTDATA_SRCADDRESS
@brief          CR_Number  
@param Address  0x00000304
@param Mask     0xF8000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS__MSK_CR_NUMBER       0xF8000000
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS_SHFT_CR_NUMBER       27
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_SRCADDRESS_RSTV_CR_NUMBER       0x00000000


/**
@defgroup       OUTDATA_DSTADDRESS OUTDATA_DSTADDRESS
@ingroup        Register
@brief          OutData_DstAddress 
@param Address  0x00000308
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_DSTADDRESS              0x00000308
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_DSTADDRESS_RST__VAL     0x00000000

/**
@defgroup       OUTDATA_DSTADDRESS__GPIO_OUTADDRESS GPIO_OUTADDRESS
@ingroup        OUTDATA_DSTADDRESS
@brief          GPIO_OUTAddress
@param Address  0x00000308
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_DSTADDRESS__MSK_GPIO_OUTADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_DSTADDRESS_SHFT_GPIO_OUTADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_DSTADDRESS_RSTV_GPIO_OUTADDRESS       0x00000000


/**
@defgroup       OUTDATA_OFFSET OUTDATA_OFFSET
@ingroup        Register
@brief          OutData_Offset 
@param Address  0x0000030C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_OFFSET              0x0000030C
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_OFFSET_RST__VAL     0x00000020

/**
@defgroup       OUTDATA_OFFSET__GPIO_OUTOFFSET GPIO_OUTOFFSET
@ingroup        OUTDATA_OFFSET
@brief          GPIO_OUTOffset
@param Address  0x0000030C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000020
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_OFFSET__MSK_GPIO_OUTOFFSET       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_OFFSET_SHFT_GPIO_OUTOFFSET       0
#define ERTEC200P_APBPER_PERIF_REG_OUTDATA_OFFSET_RSTV_GPIO_OUTOFFSET       0x00000020


/**
@defgroup       SUB_VALUE SUB_VALUE
@ingroup        Register
@brief          Sub_Value 
@param Address  0x00000310
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE              0x00000310
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_RST__VAL     0x00000000

/**
@defgroup       SUB_VALUE__SUB_MODE SUB_MODE
@ingroup        SUB_VALUE
@brief          Sub_Mode
@param Address  0x00000310
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE__MSK_SUB_MODE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_SHFT_SUB_MODE       0
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_RSTV_SUB_MODE       0x00000000

/**
@defgroup       SUB_VALUE__SUBACTIV_DIRECT SUBACTIV_DIRECT
@ingroup        SUB_VALUE
@brief          SubActiv_direct
@param Address  0x00000310
@param Mask     0x00000100
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE__MSK_SUBACTIV_DIRECT       0x00000100
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_SHFT_SUBACTIV_DIRECT       8
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_RSTV_SUBACTIV_DIRECT       0x00000000

/**
@defgroup       SUB_VALUE__SUBACTIV_STATE SUBACTIV_STATE
@ingroup        SUB_VALUE
@brief          SubActiv_state
@param Address  0x00000310
@param Mask     0x00000200
@param Shift    9
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE__MSK_SUBACTIV_STATE       0x00000200
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_SHFT_SUBACTIV_STATE       9
#define ERTEC200P_APBPER_PERIF_REG_SUB_VALUE_RSTV_SUBACTIV_STATE       0x00000000


/**
@defgroup       DEFAULT_VALUE DEFAULT_VALUE
@ingroup        Register
@brief          Default_Value 
@param Address  0x00000314
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE              0x00000314

/**
@defgroup       DEFAULT_VALUE__DEFAULTVALUE DEFAULTVALUE
@ingroup        DEFAULT_VALUE
@brief          DefaultValue 
@param Address  0x00000314
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE__MSK_DEFAULTVALUE       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE_SHFT_DEFAULTVALUE       0
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE_RSTV_DEFAULTVALUE       0x00000000

/**
@defgroup       DEFAULT_VALUE__DEFAULTVALUE_CR_NR DEFAULTVALUE_CR_NR
@ingroup        DEFAULT_VALUE
@brief          DefaultValue_CR_Nr  
@param Address  0x00000314
@param Mask     0xF8000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE__MSK_DEFAULTVALUE_CR_NR       0xF8000000
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE_SHFT_DEFAULTVALUE_CR_NR       27
#define ERTEC200P_APBPER_PERIF_REG_DEFAULT_VALUE_RSTV_DEFAULTVALUE_CR_NR       0x00000000


/**
@defgroup       LAST_VALUE LAST_VALUE
@ingroup        Register
@brief          Last_Value 
@param Address  0x00000318
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE              0x00000318

/**
@defgroup       LAST_VALUE__LASTVALUE LASTVALUE
@ingroup        LAST_VALUE
@brief          LastValue 
@param Address  0x00000318
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE__MSK_LASTVALUE       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE_SHFT_LASTVALUE       0
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE_RSTV_LASTVALUE       0x00000000

/**
@defgroup       LAST_VALUE__LASTVALUE_CR_NR LASTVALUE_CR_NR
@ingroup        LAST_VALUE
@brief          LastValue_CR_Nr  
@param Address  0x00000318
@param Mask     0xF8000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE__MSK_LASTVALUE_CR_NR       0xF8000000
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE_SHFT_LASTVALUE_CR_NR       27
#define ERTEC200P_APBPER_PERIF_REG_LAST_VALUE_RSTV_LASTVALUE_CR_NR       0x00000000


/**
@defgroup       RECORDVALUE_O RECORDVALUE_O
@ingroup        Register
@brief          RecordValue_O 
@param Address  0x0000031C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O              0x0000031C

/**
@defgroup       RECORDVALUE_O__RECORDVALUE_O RECORDVALUE_O
@ingroup        RECORDVALUE_O
@brief          RecordValue_O 
@param Address  0x0000031C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O__MSK_RECORDVALUE_O       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O_SHFT_RECORDVALUE_O       0
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O_RSTV_RECORDVALUE_O       0x00000000

/**
@defgroup       RECORDVALUE_O__RECORDVALUE_CR_NR_O RECORDVALUE_CR_NR_O
@ingroup        RECORDVALUE_O
@brief          RecordValue_CR_Nr_O  
@param Address  0x0000031C
@param Mask     0xF8000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O__MSK_RECORDVALUE_CR_NR_O       0xF8000000
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O_SHFT_RECORDVALUE_CR_NR_O       27
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_O_RSTV_RECORDVALUE_CR_NR_O       0x00000000


/**
@defgroup       RECORDVALUE_I RECORDVALUE_I
@ingroup        Register
@brief          RecordValue_I 
@param Address  0x00000320
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I              0x00000320

/**
@defgroup       RECORDVALUE_I__RECORDVALUE_I RECORDVALUE_I
@ingroup        RECORDVALUE_I
@brief          RecordValue_I 
@param Address  0x00000320
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I__MSK_RECORDVALUE_I       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I_SHFT_RECORDVALUE_I       0
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I_RSTV_RECORDVALUE_I       0x00000000

/**
@defgroup       RECORDVALUE_I__RECORDVALUE_CR_NR_I RECORDVALUE_CR_NR_I
@ingroup        RECORDVALUE_I
@brief          RecordValue_CR_Nr_I  
@param Address  0x00000320
@param Mask     0xF8000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I__MSK_RECORDVALUE_CR_NR_I       0xF8000000
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I_SHFT_RECORDVALUE_CR_NR_I       27
#define ERTEC200P_APBPER_PERIF_REG_RECORDVALUE_I_RSTV_RECORDVALUE_CR_NR_I       0x00000000


/**
@defgroup       INDATA_SRCADDRESS INDATA_SRCADDRESS
@ingroup        Register
@brief          InData_SrcAddress 
@param Address  0x00000324
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_SRCADDRESS              0x00000324
#define ERTEC200P_APBPER_PERIF_REG_INDATA_SRCADDRESS_RST__VAL     0x00000000

/**
@defgroup       INDATA_SRCADDRESS__GPIO_INADDRESS GPIO_INADDRESS
@ingroup        INDATA_SRCADDRESS
@brief          GPIO_InAddress
@param Address  0x00000324
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_SRCADDRESS__MSK_GPIO_INADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_INDATA_SRCADDRESS_SHFT_GPIO_INADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_INDATA_SRCADDRESS_RSTV_GPIO_INADDRESS       0x00000000


/**
@defgroup       INDATA_DSTADDRESS INDATA_DSTADDRESS
@ingroup        Register
@brief          InData_DstAddress 
@param Address  0x00000328
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS              0x00000328

/**
@defgroup       INDATA_DSTADDRESS__DST_ADDRESS DST_ADDRESS
@ingroup        INDATA_DSTADDRESS
@brief          Dst_Address 
@param Address  0x00000328
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS__MSK_DST_ADDRESS       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS_SHFT_DST_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS_RSTV_DST_ADDRESS       0x00000000

/**
@defgroup       INDATA_DSTADDRESS__LENGTH LENGTH
@ingroup        INDATA_DSTADDRESS
@brief          Length
@param Address  0x00000328
@param Mask     0x07FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS__MSK_LENGTH       0x07FF0000
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS_SHFT_LENGTH       16
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS_RSTV_LENGTH       0x00000000

/**
@defgroup       INDATA_DSTADDRESS__CR_NUMBER CR_NUMBER
@ingroup        INDATA_DSTADDRESS
@brief          CR_Number  
@param Address  0x00000328
@param Mask     0xF8000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS__MSK_CR_NUMBER       0xF8000000
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS_SHFT_CR_NUMBER       27
#define ERTEC200P_APBPER_PERIF_REG_INDATA_DSTADDRESS_RSTV_CR_NUMBER       0x00000000


/**
@defgroup       INDATA_OFFSET INDATA_OFFSET
@ingroup        Register
@brief          InData_Offset 
@param Address  0x0000032C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_OFFSET              0x0000032C
#define ERTEC200P_APBPER_PERIF_REG_INDATA_OFFSET_RST__VAL     0x00000020

/**
@defgroup       INDATA_OFFSET__GPIO_INOFFSET GPIO_INOFFSET
@ingroup        INDATA_OFFSET
@brief          GPIO_InOffset
@param Address  0x0000032C
@param Mask     0x00000FFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000020
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_INDATA_OFFSET__MSK_GPIO_INOFFSET       0x00000FFF
#define ERTEC200P_APBPER_PERIF_REG_INDATA_OFFSET_SHFT_GPIO_INOFFSET       0
#define ERTEC200P_APBPER_PERIF_REG_INDATA_OFFSET_RSTV_GPIO_INOFFSET       0x00000020


/**
@defgroup       SPI_OUTHEADERPROTOCOL_0 SPI_OUTHEADERPROTOCOL_0
@ingroup        Register
@brief          SPI_OutHeaderProtocol_0 
@param Address  0x00000380
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_0              0x00000380
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_0_RST__VAL     0x00000000

/**
@defgroup       SPI_OUTHEADERPROTOCOL_0__SPI_OUTHEADERPROTOCOL_0 SPI_OUTHEADERPROTOCOL_0
@ingroup        SPI_OUTHEADERPROTOCOL_0
@brief          SPI_OutHeaderProtocol_0
@param Address  0x00000380
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_0__MSK_SPI_OUTHEADERPROTOCOL_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_0_SHFT_SPI_OUTHEADERPROTOCOL_0       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_0_RSTV_SPI_OUTHEADERPROTOCOL_0       0x00000000


/**
@defgroup       SPI_OUTHEADERPROTOCOL_1 SPI_OUTHEADERPROTOCOL_1
@ingroup        Register
@brief          SPI_OutHeaderProtocol_1 
@param Address  0x00000384
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_1              0x00000384
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_1_RST__VAL     0x00000000

/**
@defgroup       SPI_OUTHEADERPROTOCOL_1__SPI_OUTHEADERPROTOCOL_1 SPI_OUTHEADERPROTOCOL_1
@ingroup        SPI_OUTHEADERPROTOCOL_1
@brief          SPI_OutHeaderProtocol_1
@param Address  0x00000384
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_1__MSK_SPI_OUTHEADERPROTOCOL_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_1_SHFT_SPI_OUTHEADERPROTOCOL_1       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERPROTOCOL_1_RSTV_SPI_OUTHEADERPROTOCOL_1       0x00000000


/**
@defgroup       SPI_OUTHEADERLENGTH SPI_OUTHEADERLENGTH
@ingroup        Register
@brief          SPI_OutHeaderLength 
@param Address  0x00000388
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERLENGTH              0x00000388
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERLENGTH_RST__VAL     0x00000000

/**
@defgroup       SPI_OUTHEADERLENGTH__SPI_LENGTH SPI_LENGTH
@ingroup        SPI_OUTHEADERLENGTH
@brief          SPI_Length
@param Address  0x00000388
@param Mask     0x0000000F
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERLENGTH__MSK_SPI_LENGTH       0x0000000F
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERLENGTH_SHFT_SPI_LENGTH       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTHEADERLENGTH_RSTV_SPI_LENGTH       0x00000000


/**
@defgroup       SPI_OUTMODE SPI_OUTMODE
@ingroup        Register
@brief          SPI_OutMode 
@param Address  0x0000038C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE              0x0000038C
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_RST__VAL     0x00000000

/**
@defgroup       SPI_OUTMODE__OUTTRANSFER_MODE OUTTRANSFER_MODE
@ingroup        SPI_OUTMODE
@brief          OutTransfer_Mode
@param Address  0x0000038C
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE__MSK_OUTTRANSFER_MODE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_SHFT_OUTTRANSFER_MODE       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_RSTV_OUTTRANSFER_MODE       0x00000000

/**
@defgroup       SPI_OUTMODE__OUTTRANSFER_DIR OUTTRANSFER_DIR
@ingroup        SPI_OUTMODE
@brief          OutTransfer_DIR
@param Address  0x0000038C
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE__MSK_OUTTRANSFER_DIR       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_SHFT_OUTTRANSFER_DIR       1
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_RSTV_OUTTRANSFER_DIR       0x00000000

/**
@defgroup       SPI_OUTMODE__OUTSTATUS OUTSTATUS
@ingroup        SPI_OUTMODE
@brief          OutStatus
@param Address  0x0000038C
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE__MSK_OUTSTATUS       0x0000FF00
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_SHFT_OUTSTATUS       8
#define ERTEC200P_APBPER_PERIF_REG_SPI_OUTMODE_RSTV_OUTSTATUS       0x00000000


/**
@defgroup       SPI_INHEADERPROTOCOL_0 SPI_INHEADERPROTOCOL_0
@ingroup        Register
@brief          SPI_InHeaderProtocol_0 
@param Address  0x00000390
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_0              0x00000390
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_0_RST__VAL     0x00000000

/**
@defgroup       SPI_INHEADERPROTOCOL_0__SPI_INHEADERPROTOCOL_0 SPI_INHEADERPROTOCOL_0
@ingroup        SPI_INHEADERPROTOCOL_0
@brief          SPI_InHeaderProtocol_0
@param Address  0x00000390
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_0__MSK_SPI_INHEADERPROTOCOL_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_0_SHFT_SPI_INHEADERPROTOCOL_0       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_0_RSTV_SPI_INHEADERPROTOCOL_0       0x00000000


/**
@defgroup       SPI_INHEADERPROTOCOL_1 SPI_INHEADERPROTOCOL_1
@ingroup        Register
@brief          SPI_InHeaderProtocol_1 
@param Address  0x00000394
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_1              0x00000394
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_1_RST__VAL     0x00000000

/**
@defgroup       SPI_INHEADERPROTOCOL_1__SPI_INHEADERPROTOCOL_1 SPI_INHEADERPROTOCOL_1
@ingroup        SPI_INHEADERPROTOCOL_1
@brief          SPI_InHeaderProtocol_1
@param Address  0x00000394
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_1__MSK_SPI_INHEADERPROTOCOL_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_1_SHFT_SPI_INHEADERPROTOCOL_1       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERPROTOCOL_1_RSTV_SPI_INHEADERPROTOCOL_1       0x00000000


/**
@defgroup       SPI_INHEADERLENGTH SPI_INHEADERLENGTH
@ingroup        Register
@brief          SPI_InHeaderLength  
@param Address  0x00000398
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERLENGTH              0x00000398
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERLENGTH_RST__VAL     0x00000000

/**
@defgroup       SPI_INHEADERLENGTH__SPI_LENGTH SPI_LENGTH
@ingroup        SPI_INHEADERLENGTH
@brief          SPI_Length
@param Address  0x00000398
@param Mask     0x0000000F
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERLENGTH__MSK_SPI_LENGTH       0x0000000F
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERLENGTH_SHFT_SPI_LENGTH       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_INHEADERLENGTH_RSTV_SPI_LENGTH       0x00000000


/**
@defgroup       SPI_INMODE SPI_INMODE
@ingroup        Register
@brief          SPI_InMode    
@param Address  0x0000039C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE              0x0000039C
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE_RST__VAL     0x00000000

/**
@defgroup       SPI_INMODE__INTRANSFER_MODE INTRANSFER_MODE
@ingroup        SPI_INMODE
@brief          InTransfer_Mode
@param Address  0x0000039C
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE__MSK_INTRANSFER_MODE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE_SHFT_INTRANSFER_MODE       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE_RSTV_INTRANSFER_MODE       0x00000000

/**
@defgroup       SPI_INMODE__INTRANSFER_DIR INTRANSFER_DIR
@ingroup        SPI_INMODE
@brief          InTransfer_DIR
@param Address  0x0000039C
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE__MSK_INTRANSFER_DIR       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE_SHFT_INTRANSFER_DIR       1
#define ERTEC200P_APBPER_PERIF_REG_SPI_INMODE_RSTV_INTRANSFER_DIR       0x00000000


/**
@defgroup       SPI_HANDSHAKE SPI_HANDSHAKE
@ingroup        Register
@brief          SPI_Handshake 
@param Address  0x000003A0
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE              0x000003A0
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_RST__VAL     0x00000000

/**
@defgroup       SPI_HANDSHAKE__HANDSHAKE_MODE HANDSHAKE_MODE
@ingroup        SPI_HANDSHAKE
@brief          Handshake_Mode
@param Address  0x000003A0
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE__MSK_HANDSHAKE_MODE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_SHFT_HANDSHAKE_MODE       0
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_RSTV_HANDSHAKE_MODE       0x00000000

/**
@defgroup       SPI_HANDSHAKE__BUSY_POL BUSY_POL
@ingroup        SPI_HANDSHAKE
@brief          Busy_POL
@param Address  0x000003A0
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE__MSK_BUSY_POL       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_SHFT_BUSY_POL       1
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_RSTV_BUSY_POL       0x00000000

/**
@defgroup       SPI_HANDSHAKE__WAIT_TIME WAIT_TIME
@ingroup        SPI_HANDSHAKE
@brief          Wait_Time
@param Address  0x000003A0
@param Mask     0x00FFFF00
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE__MSK_WAIT_TIME       0x00FFFF00
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_SHFT_WAIT_TIME       8
#define ERTEC200P_APBPER_PERIF_REG_SPI_HANDSHAKE_RSTV_WAIT_TIME       0x00000000


/**
@defgroup       GPIO_IOCTRL_0 GPIO_IOCTRL_0
@ingroup        Register
@brief          GPIO_IOCTRL_0 
@param Address  0x00000400
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_0              0x00000400
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_0_RST__VAL     0xFFFFFFFF

/**
@defgroup       GPIO_IOCTRL_0__GPIO_IOCTRL_0 GPIO_IOCTRL_0
@ingroup        GPIO_IOCTRL_0
@brief          GPIO_IOCTRL_0
@param Address  0x00000400
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_0__MSK_GPIO_IOCTRL_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_0_SHFT_GPIO_IOCTRL_0       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_0_RSTV_GPIO_IOCTRL_0       0xFFFFFFFF


/**
@defgroup       GPIO_OUT_0 GPIO_OUT_0
@ingroup        Register
@brief          GPIO_OUT_0 
@param Address  0x00000404
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_0              0x00000404
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_0__GPIO_OUT_0 GPIO_OUT_0
@ingroup        GPIO_OUT_0
@brief          GPIO_OUT_0
@param Address  0x00000404
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_0__MSK_GPIO_OUT_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_0_SHFT_GPIO_OUT_0       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_0_RSTV_GPIO_OUT_0       0x00000000


/**
@defgroup       GPIO_OUT_SET_0 GPIO_OUT_SET_0
@ingroup        Register
@brief          GPIO_OUT_SET_0 
@param Address  0x00000408
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_0              0x00000408
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_SET_0__GPIO_OUT_SET_0 GPIO_OUT_SET_0
@ingroup        GPIO_OUT_SET_0
@brief          GPIO_OUT_SET_0
@param Address  0x00000408
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_0__MSK_GPIO_OUT_SET_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_0_SHFT_GPIO_OUT_SET_0       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_0_RSTV_GPIO_OUT_SET_0       0x00000000


/**
@defgroup       GPIO_OUT_CLEAR_0 GPIO_OUT_CLEAR_0
@ingroup        Register
@brief          GPIO_OUT_CLEAR_0 
@param Address  0x0000040C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_0              0x0000040C
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_CLEAR_0__GPIO_OUT_CLEAR_0 GPIO_OUT_CLEAR_0
@ingroup        GPIO_OUT_CLEAR_0
@brief          GPIO_OUT_CLEAR_0
@param Address  0x0000040C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_0__MSK_GPIO_OUT_CLEAR_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_0_SHFT_GPIO_OUT_CLEAR_0       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_0_RSTV_GPIO_OUT_CLEAR_0       0x00000000


/**
@defgroup       GPIO_RES_DIS_0 GPIO_RES_DIS_0
@ingroup        Register
@brief          GPIO_RES_DIS_0 
@param Address  0x00000410
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_0              0x00000410
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_RES_DIS_0__GPIO_RES_DIS_0 GPIO_RES_DIS_0
@ingroup        GPIO_RES_DIS_0
@brief          GPIO_RES_DIS_0
@param Address  0x00000410
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_0__MSK_GPIO_RES_DIS_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_0_SHFT_GPIO_RES_DIS_0       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_0_RSTV_GPIO_RES_DIS_0       0x00000000


/**
@defgroup       GPIO_IN_0 GPIO_IN_0
@ingroup        Register
@brief          GPIO_IN_0 
@param Address  0x00000414
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_0              0x00000414
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_IN_0__GPIO_IN_0 GPIO_IN_0
@ingroup        GPIO_IN_0
@brief          GPIO_IN_0
@param Address  0x00000414
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_0__MSK_GPIO_IN_0       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_0_SHFT_GPIO_IN_0       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_0_RSTV_GPIO_IN_0       0x00000000


/**
@defgroup       GPIO_PORT_MODE_0_L GPIO_PORT_MODE_0_L
@ingroup        Register
@brief          GPIO_PORT_MODE_0_L 
@param Address  0x00000418
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_L              0x00000418
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_L_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_PORT_MODE_0_L GPIO_PORT_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          GPIO_PORT_MODE_0_L
@param Address  0x00000418
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_PORT_MODE_0_L       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_PORT_MODE_0_L       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_PORT_MODE_0_L       0x00000000


/**
@defgroup       GPIO_PORT_MODE_0_H GPIO_PORT_MODE_0_H
@ingroup        Register
@brief          GPIO_PORT_MODE_0_H 
@param Address  0x0000041C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_H              0x0000041C
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_H_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_PORT_MODE_0_H GPIO_PORT_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          GPIO_PORT_MODE_0_H
@param Address  0x0000041C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_PORT_MODE_0_H       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_PORT_MODE_0_H       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_PORT_MODE_0_H       0x00000000


/**
@defgroup       GPIO_IOCTRL_1 GPIO_IOCTRL_1
@ingroup        Register
@brief          GPIO_IOCTRL_1 
@param Address  0x00000420
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_1              0x00000420
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_1_RST__VAL     0xFFFFFFFF

/**
@defgroup       GPIO_IOCTRL_1__GPIO_IOCTRL_1 GPIO_IOCTRL_1
@ingroup        GPIO_IOCTRL_1
@brief          GPIO_IOCTRL_1
@param Address  0x00000420
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_1__MSK_GPIO_IOCTRL_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_1_SHFT_GPIO_IOCTRL_1       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IOCTRL_1_RSTV_GPIO_IOCTRL_1       0xFFFFFFFF


/**
@defgroup       GPIO_OUT_1 GPIO_OUT_1
@ingroup        Register
@brief          GPIO_OUT_1 
@param Address  0x00000424
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_1              0x00000424
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_1__GPIO_OUT_1 GPIO_OUT_1
@ingroup        GPIO_OUT_1
@brief          GPIO_OUT_1
@param Address  0x00000424
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_1__MSK_GPIO_OUT_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_1_SHFT_GPIO_OUT_1       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_1_RSTV_GPIO_OUT_1       0x00000000


/**
@defgroup       GPIO_OUT_SET_1 GPIO_OUT_SET_1
@ingroup        Register
@brief          GPIO_OUT_SET_1 
@param Address  0x00000428
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_1              0x00000428
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_SET_1__GPIO_OUT_SET_1 GPIO_OUT_SET_1
@ingroup        GPIO_OUT_SET_1
@brief          GPIO_OUT_SET_1
@param Address  0x00000428
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_1__MSK_GPIO_OUT_SET_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_1_SHFT_GPIO_OUT_SET_1       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_SET_1_RSTV_GPIO_OUT_SET_1       0x00000000


/**
@defgroup       GPIO_OUT_CLEAR_1 GPIO_OUT_CLEAR_1
@ingroup        Register
@brief          GPIO_OUT_CLEAR_1 
@param Address  0x0000042C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_1              0x0000042C
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_CLEAR_1__GPIO_OUT_CLEAR_1 GPIO_OUT_CLEAR_1
@ingroup        GPIO_OUT_CLEAR_1
@brief          GPIO_OUT_CLEAR_1
@param Address  0x0000042C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_1__MSK_GPIO_OUT_CLEAR_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_1_SHFT_GPIO_OUT_CLEAR_1       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_OUT_CLEAR_1_RSTV_GPIO_OUT_CLEAR_1       0x00000000


/**
@defgroup       GPIO_RES_DIS_1 GPIO_RES_DIS_1
@ingroup        Register
@brief          GPIO_RES_DIS_1 
@param Address  0x00000430
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_1              0x00000430
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_RES_DIS_1__GPIO_RES_DIS_1 GPIO_RES_DIS_1
@ingroup        GPIO_RES_DIS_1
@brief          GPIO_RES_DIS_1
@param Address  0x00000430
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_1__MSK_GPIO_RES_DIS_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_1_SHFT_GPIO_RES_DIS_1       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_RES_DIS_1_RSTV_GPIO_RES_DIS_1       0x00000000


/**
@defgroup       GPIO_IN_1 GPIO_IN_1
@ingroup        Register
@brief          GPIO_IN_1 
@param Address  0x00000434
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_1              0x00000434
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_IN_1__GPIO_IN_1 GPIO_IN_1
@ingroup        GPIO_IN_1
@brief          GPIO_IN_1
@param Address  0x00000434
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_1__MSK_GPIO_IN_1       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_1_SHFT_GPIO_IN_1       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_IN_1_RSTV_GPIO_IN_1       0x00000000


/**
@defgroup       GPIO_PORT_MODE_1_L GPIO_PORT_MODE_1_L
@ingroup        Register
@brief          GPIO_PORT_MODE_1_L 
@param Address  0x00000438
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_L              0x00000438
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_L_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_PORT_MODE_1_L GPIO_PORT_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          GPIO_PORT_MODE_1_L
@param Address  0x00000438
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_PORT_MODE_1_L       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_PORT_MODE_1_L       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_PORT_MODE_1_L       0x00000000


/**
@defgroup       GPIO_PORT_MODE_1_H GPIO_PORT_MODE_1_H
@ingroup        Register
@brief          GPIO_PORT_MODE_1_H 
@param Address  0x0000043C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_H              0x0000043C
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_H_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_PORT_MODE_1_H GPIO_PORT_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          GPIO_PORT_MODE_1_H
@param Address  0x0000043C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_PORT_MODE_1_H       0xFFFFFFFF
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_PORT_MODE_1_H       0
#define ERTEC200P_APBPER_PERIF_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_PORT_MODE_1_H       0x00000000


/**
@defgroup       SSPCR0 SSPCR0
@ingroup        Register
@brief          SSPCR0 
@param Address  0x00000500
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0              0x00000500
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_RST__VAL     0x00000000

/**
@defgroup       SSPCR0__DSS DSS
@ingroup        SSPCR0
@brief          DSS=Data Size Select
@param Address  0x00000500
@param Mask     0x0000000F
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0__MSK_DSS       0x0000000F
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_SHFT_DSS       0
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_RSTV_DSS       0x00000000

/**
@defgroup       SSPCR0__FRF FRF
@ingroup        SSPCR0
@brief          FRF=Frame Format
@param Address  0x00000500
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0__MSK_FRF       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_SHFT_FRF       4
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_RSTV_FRF       0x00000000

/**
@defgroup       SSPCR0__SPO SPO
@ingroup        SSPCR0
@brief          SPO=SCLKOUT Polarity
@param Address  0x00000500
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0__MSK_SPO       0x00000040
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_SHFT_SPO       6
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_RSTV_SPO       0x00000000

/**
@defgroup       SSPCR0__SPH SPH
@ingroup        SSPCR0
@brief          SPH=SCLKOUT Phase
@param Address  0x00000500
@param Mask     0x00000080
@param Shift    7
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0__MSK_SPH       0x00000080
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_SHFT_SPH       7
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_RSTV_SPH       0x00000000

/**
@defgroup       SSPCR0__SCR SCR
@ingroup        SSPCR0
@brief          SCR=Serial Clock Rate
@param Address  0x00000500
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0__MSK_SCR       0x0000FF00
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_SHFT_SCR       8
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_RSTV_SCR       0x00000000


/**
@defgroup       SSPCR1 SSPCR1
@ingroup        Register
@brief          SSPCR1 
@param Address  0x00000504
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1              0x00000504
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RST__VAL     0x00000000

/**
@defgroup       SSPCR1__RIE RIE
@ingroup        SSPCR1
@brief          RIE=Receive FIFO Interrupt Enable
@param Address  0x00000504
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_RIE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_RIE       0
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_RIE       0x00000000

/**
@defgroup       SSPCR1__TIE TIE
@ingroup        SSPCR1
@brief          TIE=Transmit FIFO Interrupt Enable
@param Address  0x00000504
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_TIE       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_TIE       1
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_TIE       0x00000000

/**
@defgroup       SSPCR1__RORIE RORIE
@ingroup        SSPCR1
@brief          RORIE=Receive FIFO Overrun Interrupt Enable
@param Address  0x00000504
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_RORIE       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_RORIE       2
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_RORIE       0x00000000

/**
@defgroup       SSPCR1__LBM LBM
@ingroup        SSPCR1
@brief          LBM=Loop Back Mode
@param Address  0x00000504
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_LBM       0x00000008
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_LBM       3
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_LBM       0x00000000

/**
@defgroup       SSPCR1__SSE SSE
@ingroup        SSPCR1
@brief          SSE=Synchronous Serial Port Enable
@param Address  0x00000504
@param Mask     0x00000010
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_SSE       0x00000010
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_SSE       4
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_SSE       0x00000000

/**
@defgroup       SSPCR1__MS MS
@ingroup        SSPCR1
@brief          MS=master/slave mode select
@param Address  0x00000504
@param Mask     0x00000020
@param Shift    5
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_MS       0x00000020
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_MS       5
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_MS       0x00000000

/**
@defgroup       SSPCR1__SOD SOD
@ingroup        SSPCR1
@brief          SOD=slave mode output disable
@param Address  0x00000504
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1__MSK_SOD       0x00000040
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_SHFT_SOD       6
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_RSTV_SOD       0x00000000


/**
@defgroup       SSPDR SSPDR
@ingroup        Register
@brief          SSPDR 
@param Address  0x00000508
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPDR              0x00000508
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_RST__VAL     0x00000000

/**
@defgroup       SSPDR__DATA DATA
@ingroup        SSPDR
@brief          Data
@param Address  0x00000508
@param Mask     0x0000FFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPDR__MSK_DATA       0x0000FFFF
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_SHFT_DATA       0
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_RSTV_DATA       0x00000000


/**
@defgroup       SSPSR SSPSR
@ingroup        Register
@brief          SSPSR 
@param Address  0x0000050C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR              0x0000050C
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_RST__VAL     0x00000000

/**
@defgroup       SSPSR__TFE TFE
@ingroup        SSPSR
@brief          TFE=Transmit FIFO empty
@param Address  0x0000050C
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR__MSK_TFE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_SHFT_TFE       0
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_RSTV_TFE       0x00000000

/**
@defgroup       SSPSR__TNF TNF
@ingroup        SSPSR
@brief          TNF=Transmit FIFO not full
@param Address  0x0000050C
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR__MSK_TNF       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_SHFT_TNF       1
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_RSTV_TNF       0x00000000

/**
@defgroup       SSPSR__RNE RNE
@ingroup        SSPSR
@brief          RNE=Receive FIFO not empty
@param Address  0x0000050C
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR__MSK_RNE       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_SHFT_RNE       2
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_RSTV_RNE       0x00000000

/**
@defgroup       SSPSR__RFF RFF
@ingroup        SSPSR
@brief          RFF=Receive FIFO  Full
@param Address  0x0000050C
@param Mask     0x00000008
@param Shift    3
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR__MSK_RFF       0x00000008
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_SHFT_RFF       3
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_RSTV_RFF       0x00000000

/**
@defgroup       SSPSR__BSY BSY
@ingroup        SSPSR
@brief          BSY=SSP Busy Flag
@param Address  0x0000050C
@param Mask     0x00000010
@param Shift    4
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR__MSK_BSY       0x00000010
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_SHFT_BSY       4
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_RSTV_BSY       0x00000000


/**
@defgroup       SSPCPSR SSPCPSR
@ingroup        Register
@brief          SSPCPSR 
@param Address  0x00000510
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR              0x00000510
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_RST__VAL     0x00000000

/**
@defgroup       SSPCPSR__CPSDVSR CPSDVSR
@ingroup        SSPCPSR
@brief          CPSDVSR=Clock Prescale divisor
@param Address  0x00000510
@param Mask     0x000000FF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR__MSK_CPSDVSR       0x000000FF
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_SHFT_CPSDVSR       0
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_RSTV_CPSDVSR       0x00000000


/**
@defgroup       SSPIIR_SSPICR SSPIIR_SSPICR
@ingroup        Register
@brief          SSPIIR_SSPICR 
@param Address  0x00000514
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR              0x00000514
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_RST__VAL     0x00000000

/**
@defgroup       SSPIIR_SSPICR__RIS RIS
@ingroup        SSPIIR_SSPICR
@brief          RIS=SSP Transmit FIFO service request interrupt status
@param Address  0x00000514
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR__MSK_RIS       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_SHFT_RIS       0
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_RSTV_RIS       0x00000000

/**
@defgroup       SSPIIR_SSPICR__TIS TIS
@ingroup        SSPIIR_SSPICR
@brief          TIS=SSP Receive FIFO service request interrupt status
@param Address  0x00000514
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR__MSK_TIS       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_SHFT_TIS       1
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_RSTV_TIS       0x00000000

/**
@defgroup       SSPIIR_SSPICR__RORIS RORIS
@ingroup        SSPIIR_SSPICR
@brief          RORIS=SSP Receive FIFO overrun interrupt status
@param Address  0x00000514
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR__MSK_RORIS       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_SHFT_RORIS       2
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_RSTV_RORIS       0x00000000


/**
@defgroup       SSPCR0_1 SSPCR0_1
@ingroup        Register
@brief          SSPCR0 
@param Address  0x00000520
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1              0x00000520
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_RST__VAL     0x00000000

/**
@defgroup       SSPCR0_1__DSS DSS
@ingroup        SSPCR0_1
@brief          DSS=Data Size Select
@param Address  0x00000520
@param Mask     0x0000000F
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1__MSK_DSS       0x0000000F
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_SHFT_DSS       0
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_RSTV_DSS       0x00000000

/**
@defgroup       SSPCR0_1__FRF FRF
@ingroup        SSPCR0_1
@brief          FRF=Frame Format
@param Address  0x00000520
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1__MSK_FRF       0x00000030
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_SHFT_FRF       4
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_RSTV_FRF       0x00000000

/**
@defgroup       SSPCR0_1__SPO SPO
@ingroup        SSPCR0_1
@brief          SPO=SCLKOUT Polarity
@param Address  0x00000520
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1__MSK_SPO       0x00000040
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_SHFT_SPO       6
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_RSTV_SPO       0x00000000

/**
@defgroup       SSPCR0_1__SPH SPH
@ingroup        SSPCR0_1
@brief          SPH=SCLKOUT Phase
@param Address  0x00000520
@param Mask     0x00000080
@param Shift    7
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1__MSK_SPH       0x00000080
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_SHFT_SPH       7
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_RSTV_SPH       0x00000000

/**
@defgroup       SSPCR0_1__SCR SCR
@ingroup        SSPCR0_1
@brief          SCR=Serial Clock Rate
@param Address  0x00000520
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1__MSK_SCR       0x0000FF00
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_SHFT_SCR       8
#define ERTEC200P_APBPER_PERIF_REG_SSPCR0_1_RSTV_SCR       0x00000000


/**
@defgroup       SSPCR1_1 SSPCR1_1
@ingroup        Register
@brief          SSPCR1 
@param Address  0x00000524
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1              0x00000524
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RST__VAL     0x00000000

/**
@defgroup       SSPCR1_1__RIE RIE
@ingroup        SSPCR1_1
@brief          RIE=Receive FIFO Interrupt Enable
@param Address  0x00000524
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_RIE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_RIE       0
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_RIE       0x00000000

/**
@defgroup       SSPCR1_1__TIE TIE
@ingroup        SSPCR1_1
@brief          TIE=Transmit FIFO Interrupt Enable
@param Address  0x00000524
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_TIE       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_TIE       1
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_TIE       0x00000000

/**
@defgroup       SSPCR1_1__RORIE RORIE
@ingroup        SSPCR1_1
@brief          RORIE=Receive FIFO Overrun Interrupt Enable
@param Address  0x00000524
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_RORIE       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_RORIE       2
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_RORIE       0x00000000

/**
@defgroup       SSPCR1_1__LBM LBM
@ingroup        SSPCR1_1
@brief          LBM=Loop Back Mode
@param Address  0x00000524
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_LBM       0x00000008
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_LBM       3
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_LBM       0x00000000

/**
@defgroup       SSPCR1_1__SSE SSE
@ingroup        SSPCR1_1
@brief          SSE=Synchronous Serial Port Enable
@param Address  0x00000524
@param Mask     0x00000010
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_SSE       0x00000010
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_SSE       4
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_SSE       0x00000000

/**
@defgroup       SSPCR1_1__MS MS
@ingroup        SSPCR1_1
@brief          MS=master/slave mode select
@param Address  0x00000524
@param Mask     0x00000020
@param Shift    5
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_MS       0x00000020
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_MS       5
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_MS       0x00000000

/**
@defgroup       SSPCR1_1__SOD SOD
@ingroup        SSPCR1_1
@brief          SOD=slave mode output disable
@param Address  0x00000524
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1__MSK_SOD       0x00000040
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_SHFT_SOD       6
#define ERTEC200P_APBPER_PERIF_REG_SSPCR1_1_RSTV_SOD       0x00000000


/**
@defgroup       SSPDR_1 SSPDR_1
@ingroup        Register
@brief          SSPDR 
@param Address  0x00000528
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_1              0x00000528
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_1_RST__VAL     0x00000000

/**
@defgroup       SSPDR_1__DATA DATA
@ingroup        SSPDR_1
@brief          Data
@param Address  0x00000528
@param Mask     0x0000FFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_1__MSK_DATA       0x0000FFFF
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_1_SHFT_DATA       0
#define ERTEC200P_APBPER_PERIF_REG_SSPDR_1_RSTV_DATA       0x00000000


/**
@defgroup       SSPSR_1 SSPSR_1
@ingroup        Register
@brief          SSPSR 
@param Address  0x0000052C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1              0x0000052C
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_RST__VAL     0x00000000

/**
@defgroup       SSPSR_1__TFE TFE
@ingroup        SSPSR_1
@brief          TFE=Transmit FIFO empty
@param Address  0x0000052C
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1__MSK_TFE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_SHFT_TFE       0
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_RSTV_TFE       0x00000000

/**
@defgroup       SSPSR_1__TNF TNF
@ingroup        SSPSR_1
@brief          TNF=Transmit FIFO not full
@param Address  0x0000052C
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1__MSK_TNF       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_SHFT_TNF       1
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_RSTV_TNF       0x00000000

/**
@defgroup       SSPSR_1__RNE RNE
@ingroup        SSPSR_1
@brief          RNE=Receive FIFO not empty
@param Address  0x0000052C
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1__MSK_RNE       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_SHFT_RNE       2
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_RSTV_RNE       0x00000000

/**
@defgroup       SSPSR_1__RFF RFF
@ingroup        SSPSR_1
@brief          RFF=Receive FIFO  Full
@param Address  0x0000052C
@param Mask     0x00000008
@param Shift    3
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1__MSK_RFF       0x00000008
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_SHFT_RFF       3
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_RSTV_RFF       0x00000000

/**
@defgroup       SSPSR_1__BSY BSY
@ingroup        SSPSR_1
@brief          BSY=SSP Busy Flag
@param Address  0x0000052C
@param Mask     0x00000010
@param Shift    4
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1__MSK_BSY       0x00000010
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_SHFT_BSY       4
#define ERTEC200P_APBPER_PERIF_REG_SSPSR_1_RSTV_BSY       0x00000000


/**
@defgroup       SSPCPSR_1 SSPCPSR_1
@ingroup        Register
@brief          SSPCPSR 
@param Address  0x00000530
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_1              0x00000530
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_1_RST__VAL     0x00000000

/**
@defgroup       SSPCPSR_1__CPSDVSR CPSDVSR
@ingroup        SSPCPSR_1
@brief          CPSDVSR=Clock Prescale divisor
@param Address  0x00000530
@param Mask     0x000000FF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_1__MSK_CPSDVSR       0x000000FF
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_1_SHFT_CPSDVSR       0
#define ERTEC200P_APBPER_PERIF_REG_SSPCPSR_1_RSTV_CPSDVSR       0x00000000


/**
@defgroup       SSPIIR_SSPICR_1 SSPIIR_SSPICR_1
@ingroup        Register
@brief          SSPIIR_SSPICR 
@param Address  0x00000534
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1              0x00000534
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_RST__VAL     0x00000000

/**
@defgroup       SSPIIR_SSPICR_1__RIS RIS
@ingroup        SSPIIR_SSPICR_1
@brief          RIS=SSP Transmit FIFO service request interrupt status
@param Address  0x00000534
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1__MSK_RIS       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_SHFT_RIS       0
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_RSTV_RIS       0x00000000

/**
@defgroup       SSPIIR_SSPICR_1__TIS TIS
@ingroup        SSPIIR_SSPICR_1
@brief          TIS=SSP Receive FIFO service request interrupt status
@param Address  0x00000534
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1__MSK_TIS       0x00000002
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_SHFT_TIS       1
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_RSTV_TIS       0x00000000

/**
@defgroup       SSPIIR_SSPICR_1__RORIS RORIS
@ingroup        SSPIIR_SSPICR_1
@brief          RORIS=SSP Receive FIFO overrun interrupt status
@param Address  0x00000534
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1__MSK_RORIS       0x00000004
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_SHFT_RORIS       2
#define ERTEC200P_APBPER_PERIF_REG_SSPIIR_SSPICR_1_RSTV_RORIS       0x00000000


/**
@defgroup       ADDRESS_MODE ADDRESS_MODE
@ingroup        Register
@brief          Address_Mode 
@param Address  0x00000600
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ADDRESS_MODE              0x00000600
#define ERTEC200P_APBPER_PERIF_REG_ADDRESS_MODE_RST__VAL     0x00000000

/**
@defgroup       ADDRESS_MODE__ADDRESS_MODE ADDRESS_MODE
@ingroup        ADDRESS_MODE
@brief          Addres_Mode 
@param Address  0x00000600
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ADDRESS_MODE__MSK_ADDRESS_MODE       0x00000001
#define ERTEC200P_APBPER_PERIF_REG_ADDRESS_MODE_SHFT_ADDRESS_MODE       0
#define ERTEC200P_APBPER_PERIF_REG_ADDRESS_MODE_RSTV_ADDRESS_MODE       0x00000000


/**
@defgroup       IP_VERSION IP_VERSION
@ingroup        Register
@brief          IP_Version 
@param Address  0x00000800
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IP_VERSION              0x00000800

/**
@defgroup       IP_VERSION__DEBUG_VERSION DEBUG_VERSION
@ingroup        IP_VERSION
@brief          debug_version
@param Address  0x00000800
@param Mask     0x000000FF
@param Shift    0
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IP_VERSION__MSK_DEBUG_VERSION       0x000000FF
#define ERTEC200P_APBPER_PERIF_REG_IP_VERSION_SHFT_DEBUG_VERSION       0
#define ERTEC200P_APBPER_PERIF_REG_IP_VERSION_RSTV_DEBUG_VERSION       0x00000000

/**
@defgroup       IP_VERSION__VERSION VERSION
@ingroup        IP_VERSION
@brief          version
@param Address  0x00000800
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,
@param Reset    0x00000001
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_VERSION__MSK_VERSION       0x0000FF00
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_VERSION_SHFT_VERSION       8
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_VERSION_RSTV_VERSION       0x00000001

/**
@defgroup       IP_VERSION__VERSION VERSION
@ingroup        IP_VERSION
@brief          version 
@param Address  0x00000800
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,
@param Reset    0x00000002
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_VERSION__MSK_VERSION       0x0000FF00
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_VERSION_SHFT_VERSION       8
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_VERSION_RSTV_VERSION       0x00000002

/**
@defgroup       IP_VERSION__CONFIGURATION CONFIGURATION
@ingroup        IP_VERSION
@brief          configuration 
@param Address  0x00000800
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,
@param Reset    0x00000003
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_VERSION__MSK_CONFIGURATION       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_VERSION_SHFT_CONFIGURATION       16
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_VERSION_RSTV_CONFIGURATION       0x00000003

/**
@defgroup       IP_VERSION__CONFIGURATION CONFIGURATION
@ingroup        IP_VERSION
@brief          configuration
@param Address  0x00000800
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,
@param Reset    0x00000001
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_VERSION__MSK_CONFIGURATION       0xFFFF0000
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_VERSION_SHFT_CONFIGURATION       16
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_VERSION_RSTV_CONFIGURATION       0x00000001


/**
@defgroup       IP_DEVELOPMENT IP_DEVELOPMENT
@ingroup        Register
@brief          IP_Development 
@param Address  0x00000804
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT              0x00000804

/**
@defgroup       IP_DEVELOPMENT__R_LABEL R_LABEL
@ingroup        IP_DEVELOPMENT
@brief          CC RR label    
@param Address  0x00000804
@param Mask     0x000007FF
@param Shift    0
@param Access   r,
@param Reset    0x00000014
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_DEVELOPMENT__MSK_R_LABEL       0x000007FF
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_DEVELOPMENT_SHFT_R_LABEL       0
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_DEVELOPMENT_RSTV_R_LABEL       0x00000014

/**
@defgroup       IP_DEVELOPMENT__R_LABEL R_LABEL
@ingroup        IP_DEVELOPMENT
@brief          CC RR label
@param Address  0x00000804
@param Mask     0x000007FF
@param Shift    0
@param Access   r,
@param Reset    0x00000006
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_DEVELOPMENT__MSK_R_LABEL       0x000007FF
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_DEVELOPMENT_SHFT_R_LABEL       0
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_DEVELOPMENT_RSTV_R_LABEL       0x00000006

/**
@defgroup       IP_DEVELOPMENT__INKR INKR
@ingroup        IP_DEVELOPMENT
@brief          Inkr
@param Address  0x00000804
@param Mask     0x0000F800
@param Shift    11
@param Access   r,
@param Reset    0x00000001
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_DEVELOPMENT__MSK_INKR       0x0000F800
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_DEVELOPMENT_SHFT_INKR       11
#define ERTEC200P_APBPER_PERIF_R1_REG_IP_DEVELOPMENT_RSTV_INKR       0x00000001

/**
@defgroup       IP_DEVELOPMENT__INKR INKR
@ingroup        IP_DEVELOPMENT
@brief          Inkr
@param Address  0x00000804
@param Mask     0x0000F800
@param Shift    11
@param Access   r,
@param Reset    0x00000002
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_DEVELOPMENT__MSK_INKR       0x0000F800
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_DEVELOPMENT_SHFT_INKR       11
#define ERTEC200P_APBPER_PERIF_R2_REG_IP_DEVELOPMENT_RSTV_INKR       0x00000002

/**
@defgroup       IP_DEVELOPMENT__LABEL_STRUCTURE LABEL_STRUCTURE
@ingroup        IP_DEVELOPMENT
@brief          patch
@param Address  0x00000804
@param Mask     0x00070000
@param Shift    16
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT__MSK_LABEL_STRUCTURE       0x00070000
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT_SHFT_LABEL_STRUCTURE       16
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT_RSTV_LABEL_STRUCTURE       0x00000000

/**
@defgroup       IP_DEVELOPMENT__TYPE TYPE
@ingroup        IP_DEVELOPMENT
@brief          platform 
@param Address  0x00000804
@param Mask     0x00180000
@param Shift    19
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT__MSK_TYPE       0x00180000
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT_SHFT_TYPE       19
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT_RSTV_TYPE       0x00000000

/**
@defgroup       IP_DEVELOPMENT__IDENTIFICATION IDENTIFICATION
@ingroup        IP_DEVELOPMENT
@brief          identification
@param Address  0x00000804
@param Mask     0xFFE00000
@param Shift    21
@param Access   r,
@param Reset    0x00000023
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT__MSK_IDENTIFICATION       0xFFE00000
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT_SHFT_IDENTIFICATION       21
#define ERTEC200P_APBPER_PERIF_REG_IP_DEVELOPMENT_RSTV_IDENTIFICATION       0x00000023


/**
@defgroup       ACCESS_ERROR ACCESS_ERROR
@ingroup        Register
@brief          Access_Err 
@param Address  0x00000808
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR              0x00000808

/**
@defgroup       ACCESS_ERROR__AHB_APB_ADDRESS AHB_APB_ADDRESS
@ingroup        ACCESS_ERROR
@brief          ahb_apb_address
@param Address  0x00000808
@param Mask     0x0000FFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_ACCESS_ERROR__MSK_AHB_APB_ADDRESS       0x0000FFFF
#define ERTEC200P_APBPER_PERIF_R1_REG_ACCESS_ERROR_SHFT_AHB_APB_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_R1_REG_ACCESS_ERROR_RSTV_AHB_APB_ADDRESS       0x00000000

/**
@defgroup       ACCESS_ERROR__AHB_APB_ADDRESS AHB_APB_ADDRESS
@ingroup        ACCESS_ERROR
@brief          ahb_apb_address
@param Address  0x00000808
@param Mask     0x0001FFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_ACCESS_ERROR__MSK_AHB_APB_ADDRESS       0x0001FFFF
#define ERTEC200P_APBPER_PERIF_R2_REG_ACCESS_ERROR_SHFT_AHB_APB_ADDRESS       0
#define ERTEC200P_APBPER_PERIF_R2_REG_ACCESS_ERROR_RSTV_AHB_APB_ADDRESS       0x00000000

/**
@defgroup       ACCESS_ERROR__AHB_APB_SELECT AHB_APB_SELECT
@ingroup        ACCESS_ERROR
@brief          ahb_apb_select
@param Address  0x00000808
@param Mask     0x08000000
@param Shift    27
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR__MSK_AHB_APB_SELECT       0x08000000
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_SHFT_AHB_APB_SELECT       27
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_RSTV_AHB_APB_SELECT       0x00000000

/**
@defgroup       ACCESS_ERROR__AHB_APB_SIZE AHB_APB_SIZE
@ingroup        ACCESS_ERROR
@brief          ahb_apb_size 
@param Address  0x00000808
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR__MSK_AHB_APB_SIZE       0x30000000
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_SHFT_AHB_APB_SIZE       28
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_RSTV_AHB_APB_SIZE       0x00000000

/**
@defgroup       ACCESS_ERROR__AHB_APB_RD_WR AHB_APB_RD_WR
@ingroup        ACCESS_ERROR
@brief          ahb_apb_rd_wr
@param Address  0x00000808
@param Mask     0x40000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR__MSK_AHB_APB_RD_WR       0x40000000
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_SHFT_AHB_APB_RD_WR       30
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_RSTV_AHB_APB_RD_WR       0x00000000

/**
@defgroup       ACCESS_ERROR__ERR_LOCK ERR_LOCK
@ingroup        ACCESS_ERROR
@brief          err_lock
@param Address  0x00000808
@param Mask     0x80000000
@param Shift    31
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR__MSK_ERR_LOCK       0x80000000
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_SHFT_ERR_LOCK       31
#define ERTEC200P_APBPER_PERIF_REG_ACCESS_ERROR_RSTV_ERR_LOCK       0x00000000


/**
@defgroup       BURST_CONFIG BURST_CONFIG
@ingroup        Register
@brief          Burst_Config 
@param Address  0x0000080C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_REG_BURST_CONFIG              0x0000080C

/**
@defgroup       BURST_CONFIG__BURSTMODE_COMAHB BURSTMODE_COMAHB
@ingroup        BURST_CONFIG
@brief          Am kommunikativen AHB-Slave Interface wird ein AHB RD/WR Burst -x0- nicht (Single Zugriffe) -01- fuer alle INCR-Burst-Types ausser Incr -11- fuer alle INCR-Burst-Types (INCR, INCR4...) am SC-Bus unterstuetzt.
@param Address  0x0000080C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000003
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_BURST_CONFIG__MSK_BURSTMODE_COMAHB       0x00000003
#define ERTEC200P_APBPER_PERIF_R2_REG_BURST_CONFIG_SHFT_BURSTMODE_COMAHB       0
#define ERTEC200P_APBPER_PERIF_R2_REG_BURST_CONFIG_RSTV_BURSTMODE_COMAHB       0x00000003

/**
@defgroup       BURST_CONFIG__BURSTMODE_COMAHB BURSTMODE_COMAHB
@ingroup        BURST_CONFIG
@brief          Am kommunikativen AHB-Slave Interface wird ein AHB RD/WR Burst -x0- nicht (Single Zugriffe) -01- fuer alle INCR-Burst-Types ausser Incr -11- fuer alle INCR-Burst-Types (INCR, INCR4...) am SC-Bus unterstuetzt.
@param Address  0x0000080C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_BURST_CONFIG__MSK_BURSTMODE_COMAHB       0x00000003
#define ERTEC200P_APBPER_PERIF_R1_REG_BURST_CONFIG_SHFT_BURSTMODE_COMAHB       0
#define ERTEC200P_APBPER_PERIF_R1_REG_BURST_CONFIG_RSTV_BURSTMODE_COMAHB       0x00000001

/**
@defgroup       BURST_CONFIG__BURSTMODE_APPLAHB BURSTMODE_APPLAHB
@ingroup        BURST_CONFIG
@brief          Am applikativem AHB-Slave Interface wird ein AHB RD/WR Burst -x0- nicht (Single Zugriffe) -01- fuer alle Burst-Types ausser Incr -11- fuer alle Burst-Types (INCR, INCR4...) am SC-Bus unterstuetzt.
@param Address  0x0000080C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R2_REG_BURST_CONFIG__MSK_BURSTMODE_APPLAHB       0x00000300
#define ERTEC200P_APBPER_PERIF_R2_REG_BURST_CONFIG_SHFT_BURSTMODE_APPLAHB       8
#define ERTEC200P_APBPER_PERIF_R2_REG_BURST_CONFIG_RSTV_BURSTMODE_APPLAHB       0x00000000

/**
@defgroup       BURST_CONFIG__BURSTMODE_APPLAHB BURSTMODE_APPLAHB
@ingroup        BURST_CONFIG
@brief          Am applikativem AHB-Slave Interface wird ein AHB RD/WR Burst -x0- nicht (Single Zugriffe) -01- fuer alle Burst-Types ausser Incr -11- fuer alle Burst-Types (INCR, INCR4...) am SC-Bus unterstuetzt.
@param Address  0x0000080C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_PERIF_R1_REG_BURST_CONFIG__MSK_BURSTMODE_APPLAHB       0x00000300
#define ERTEC200P_APBPER_PERIF_R1_REG_BURST_CONFIG_SHFT_BURSTMODE_APPLAHB       8
#define ERTEC200P_APBPER_PERIF_R1_REG_BURST_CONFIG_RSTV_BURSTMODE_APPLAHB       0x00000001

/**
@defgroup        PERIF_IO_RAM PERIF_IO_RAM
@ingroup         Memory
@brief           Zugriff auf das IO-RAM auf 0x2000 - 0x2FFF: Zugriff mit Address-Substitute 0x3000 - 0x3FFF: direkter RAM-Zugriff. Hinweis: Im Page-Mode erfolgt der Zugriff auf das IO-RAM ueber spezielle Adressen.
@param Address   0x00002000 
@param Length    0x00003000
@param Access    r,w
@param Version   1
@param Type      -1
*/
#define ERTEC200P_APBPER_PERIF_R1_RAM_PERIF_IO_RAM_START     0x00002000
#define ERTEC200P_APBPER_PERIF_R1_RAM_PERIF_IO_RAM_END       0x00004FFF
#define ERTEC200P_APBPER_PERIF_R1_RAM_PERIF_IO_RAM_LEN       0x00003000

/**
@defgroup        PERIF_IO_RAM PERIF_IO_RAM
@ingroup         Memory
@brief           Zugriff auf das IO-RAM auf 0x2000 - 0x2FFF: Zugriff mit Address-Substitute 0x3000 - 0x4FFF: direkter RAM-Zugriff. Hinweis: Im Page-Mode erfolgt der Zugriff auf das IO-RAM ueber spezielle Adressen.
@param Address   0x00002000 
@param Length    0x00004000
@param Access    r,w
@param Version   2
@param Type      -1
*/
#define ERTEC200P_APBPER_PERIF_R2_RAM_PERIF_IO_RAM_START     0x00002000
#define ERTEC200P_APBPER_PERIF_R2_RAM_PERIF_IO_RAM_END       0x00005FFF
#define ERTEC200P_APBPER_PERIF_R2_RAM_PERIF_IO_RAM_LEN       0x00004000

#ifdef __cplusplus
}
#endif

#endif
