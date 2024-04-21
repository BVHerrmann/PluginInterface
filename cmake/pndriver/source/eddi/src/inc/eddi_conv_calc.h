#ifndef EDDI_CONV_CALC_H        //reinclude-protection
#define EDDI_CONV_CALC_H

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
/*  F i l e               &F: eddi_conv_calc.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.09.10    AH    Initial Version                                        */
/*****************************************************************************/

//temporarily_disabled_lint -esym(714, eddi_InitDPBWithDefaults)
//temporarily_disabled_lint -esym(714, eddi_InitDSBWithDefaults)
//temporarily_disabled_lint -esym(714, eddi_InitCOMPWithDefaults)

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/
#define  EDDI_ILLEGAL_ADDR_VALUE_32     0xFFFFFFFFUL
#define  EDDI_ILLEGAL_ADDR_VALUE_16     0xFFFFUL


/*===========================================================================*/
/*                               Structs                                     */
/*===========================================================================*/
typedef struct _EDDI_CONV_CALC_LOCAL_TYPE
{
    LSA_UINT32  ProvFrameFactor;
    LSA_UINT32  MaxPort;
    LSA_UINT32  NrOfFrames;
    LSA_UINT32  FCWCountBug;
    LSA_UINT32  DBCountBug;
    LSA_UINT32  FCWDBCountPorts;
    LSA_UINT32  NRTFCWCount;
    LSA_UINT32  NRTDBCount;
    LSA_UINT32  HOL_LimitDBAddValuePerPort;
    LSA_UINT32  HOL_Limit_Port_Up;
    LSA_UINT32  Buffercapacity_us;
    LSA_UINT32  HSYNC_HOL_LimitDBCorrectionValuePerPort;
    LSA_UINT32  HSYNC_FCWCorrectionValuePerPort;

} EDDI_CONV_CALC_LOCAL_TYPE;

typedef struct _EDDI_CONV_CALC_IOC_TYPE
{
    LSA_UINT32  PiSizeInput;
    LSA_UINT32  PiSizeOutput;
    LSA_UINT32  NrOfRtUcProvider;
    LSA_UINT32  NrOfRtMcProvider;
    LSA_UINT32  NrOfIrtUcProvider;
    LSA_UINT32  NrOfIrtMcProvider;
    LSA_UINT32  NrOfRtUcConsumer;
    LSA_UINT32  NrOfRtMcConsumer;
    LSA_UINT32  NrOfIrtUcConsumer;
    LSA_UINT32  NrOfIrtMcConsumer;

} EDDI_CONV_CALC_IOC_TYPE;

typedef struct _EDDI_CONV_CALC_IOD_TYPE
{
    LSA_UINT32  PiSizeInput;
    LSA_UINT32  PiSizeOutput;
    LSA_UINT32  NrOfRtUcProvider;
    LSA_UINT32  NrOfRtMcProvider;
    LSA_UINT32  NrOfIrtUcProvider;
    LSA_UINT32  NrOfIrtMcProvider;
    LSA_UINT32  NrOfRtUcConsumer;
    LSA_UINT32  NrOfRtMcConsumer;
    LSA_UINT32  NrOfIrtUcConsumer;
    LSA_UINT32  NrOfIrtMcConsumer;

} EDDI_CONV_CALC_IOD_TYPE;

typedef struct _EDDI_CONV_CALC_TYPE
{
    EDDI_CONV_CALC_LOCAL_TYPE  Local;
    EDDI_CONV_CALC_IOC_TYPE    IOC;
    EDDI_CONV_CALC_IOD_TYPE    IOD;
    
    LSA_UINT32                 ConsumerCntClass12; 
    LSA_UINT32                 ProviderCnt;       
    LSA_UINT32                 ConsumerCntClass3; 
    LSA_UINT32                 ProviderCntClass3; 
    LSA_UINT32                 ForwarderCntClass3;

}  EDDI_CONV_CALC_TYPE;

typedef struct _EDDI_CONV_CALC_TYPE  EDD_UPPER_MEM_ATTR  *  EDDI_UPPER_CONV_CALC_PTR_TYPE;

/*===========================================================================*/
/*                               Functions                                   */
/*===========================================================================*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_CalcValues( EDDI_UPPER_CONV_CALC_PTR_TYPE  pCC,
                                                EDDI_UPPER_GLOBAL_INI_PTR_TYPE pGlobalInitStruct );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CONV_CALC_H


/*****************************************************************************/
/*  end of file eddi_conv_calc.h                                             */
/*****************************************************************************/
