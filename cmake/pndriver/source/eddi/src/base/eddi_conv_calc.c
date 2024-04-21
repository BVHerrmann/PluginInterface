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
/*  F i l e               &F: eddi_conv_calc.c                          :F&  */
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

#include "eddi_int.h"
//#include "eddi_check.h"
#include "eddi_dev.h"
#include "eddi_swi_ucmc.h"
//#include "eddi_swi_ptp.h"

#define EDDI_MODULE_ID     M_ID_EDDI_CONV
#define LTRC_ACT_MODUL_ID  11

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if (EDDI_HSYNC_ROLE_FORWARDER >= EDDI_HSYNC_ROLE_APPL_SUPPORT)
    #error "EDDI_HSYNC_ROLE_FORWARDER has to be 1  AND  EDDI_HSYNC_ROLE_APPL_SUPPORT has to be 2"
#endif

#define EDDI_HSYNC_ROLE_COUNT  (EDDI_HSYNC_ROLE_APPL_SUPPORT + 1)

static const LSA_UINT32  EDDI_HSyncDB_NRT_FRAMES     [EDDI_HSYNC_ROLE_COUNT] = { 0, EDDI_HSYNC_FORWARDER_DB_NRT_FRAMES,      EDDI_HSYNC_APPLICATION_DB_NRT_FRAMES      };
static const LSA_UINT32  EDDI_HSyncDBS_PER_NRT_FRAME [EDDI_HSYNC_ROLE_COUNT] = { 0, EDDI_HSYNC_FORWARDER_DBS_PER_NRT_FRAME,  EDDI_HSYNC_APPLICATION_DBS_PER_NRT_FRAME  };
static const LSA_UINT32  EDDI_HSyncFCW_NRT_FRAMES    [EDDI_HSYNC_ROLE_COUNT] = { 0, EDDI_HSYNC_FORWARDER_FCW_NRT_FRAMES,     EDDI_HSYNC_APPLICATION_FCW_NRT_FRAMES     };
static const LSA_UINT32  EDDI_HSyncFCWS_PER_NRT_FRAME[EDDI_HSYNC_ROLE_COUNT] = { 0, EDDI_HSYNC_FORWARDER_FCWS_PER_NRT_FRAME, EDDI_HSYNC_APPLICATION_FCWS_PER_NRT_FRAME };
static const LSA_UINT32  EDDI_HSyncNR_OF_PORTS       [EDDI_HSYNC_ROLE_COUNT] = { 0, EDDI_HSYNC_FORWARDER_NR_OF_PORTS,        EDDI_HSYNC_APPLICATION_NR_OF_PORTS        };
static const LSA_UINT32  EDDI_HSyncSAMPLE_FACTOR     [EDDI_HSYNC_ROLE_COUNT] = { 0, EDDI_HSYNC_FORWARDER_SAMPLE_FACTOR,      EDDI_HSYNC_APPLICATION_SAMPLE_FACTOR      };


static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  Eddi_SetDelayPortParams( EDDI_UPPER_DSB_PTR_TYPE    const  pDSB,
                                                                LSA_UINT32                 const  PhyTyp,
                                                                EDDI_PHY_TRANSCEIVER_TYPE  const  PhyTransceiver,
                                                                LSA_UINT32                 const  ErtecTyp );

static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_ConvGetAcwKramSize( LSA_UINT32  const  AcwConsumerCnt );

typedef struct IOC_PI_SIZE_CALC_TYPE_
{
    LSA_UINT32 FullFrames; 
    LSA_UINT32 FullFramesIRT;
    LSA_UINT32 NetData;
    LSA_UINT32 RestNet;
    LSA_UINT32 RestNetIRT;
    LSA_UINT32 AddBytes;
    LSA_UINT32 AddBytesIRT; 
    LSA_UINT32 AlignFullFrames; 
    LSA_UINT32 AlignFullFramesIRT;
    LSA_UINT32 AlignRestFrames;
    LSA_UINT32 AlignRestFramesIRT;
    LSA_UINT32 IOCRTAll;
    LSA_UINT32 IOCIRTAll; 
    LSA_UINT32 MinFrameSizeRT;             //in
    LSA_UINT32 MinFrameSizeIRT;            //in
} IOC_PI_SIZE_CALC_TYPE;

#define EDDI_CONV_CALC_ROUNDn(val_, n_)         (((val_)+(n_)-1UL) & ((0xFFFFFFFFUL-(n_))+1))
#define EDDI_CONV_CALC_ROUNDn0(ret_, val_, n_)  {                                                                       \
                                                    LSA_UINT32 const local_var_ = EDDI_CONV_CALC_ROUNDn((val_), (n_));  \
                                                    if (local_var_) {ret_ = local_var_;}                                \
                                                    else {ret_ = 0;}                                                    \
                                                }


/***************************************************************************/
/* F u n c t i o n:       EDDI_SerGetAcwKramSize()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_ConvGetAcwKramSize( LSA_UINT32  const  AcwConsumerCnt )
{
    LSA_UINT32  shifter, RxEntries, ACWRxHeads;
    LSA_UINT32  Size, ACWTotalHeads;

    //EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SerGetAcwKramSize->");

    RxEntries = 0;
    // Calculate next binary value after ConsumerCnt
    // We start with the shifter 5 to garanty a ACWRxHeads-Value devidable by 2
    for (shifter = 5; shifter < 16 ; shifter++)
    {
        RxEntries = (LSA_UINT16)((LSA_UINT32)0x1 << shifter);

        if (RxEntries >= AcwConsumerCnt)
        {
            // Ok we found the correct Value for AcwRxHeads
            break;
        }
    }

    ACWRxHeads = (LSA_UINT16)(RxEntries >> 0x03);  // Maximum 8 Entries per Header

    //Alloc Mem for ACW-Table
    ACWTotalHeads = (LSA_UINT16)(ACWRxHeads + 2UL);  // add a reserved Head and the AcwTx-List-Head

    // Alloc and Reset KRAM-Structures
    Size = (sizeof(EDDI_SER_ACW_HEAD_TYPE) + (sizeof(EDDI_SER_ACW_HEAD_TYPE) * (ACWTotalHeads / 2)));

    return Size;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_CalcValues()                                */
/*                                                                         */
/* D e s c r i p t i o n: used for KRAMCalc                                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CalcValues( EDDI_UPPER_CONV_CALC_PTR_TYPE   pCC,
                                                  EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct )
{
    static  EDDI_CONV_CALC_TYPE  pCCLocal[EDD_INTERFACE_ID_MAX];
    LSA_UINT32                   Temp[16];    
    LSA_UINT32                   APDUSize;
    IOC_PI_SIZE_CALC_TYPE        IOCCalc;
    LSA_UINT32                   i;
    LSA_UINT32                   framefactor;
    LSA_UINT32                   IOCQV;
    LSA_BOOL            const    bSSOn = LSA_TRUE;
    #if defined (EDDI_CFG_REV6)
    LSA_BOOL            const    bSMOn = LSA_FALSE;
    #else
    LSA_BOOL            const    bSMOn = LSA_TRUE;
    #endif
    #if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT32          const    TBufferSize = sizeof(LSA_UINT32); //size of the IO-transportbuffer
    #else
    LSA_UINT32          const    TBufferSize = 0;
    #endif
    LSA_BOOL            const    bRedOn = (EDD_MRP_ROLE_NONE == pGlobalInitStruct->GenIni.MRPDefaultRoleInstance0)?LSA_FALSE:LSA_TRUE;

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CalcValues->");
    
    /*************************************************/    
    /* Precalculation checks                         */
    /*************************************************/    
    if (   (pGlobalInitStruct->InterfaceID > EDD_INTERFACE_ID_MAX)
        || (   (EDD_FEATURE_DISABLE != pGlobalInitStruct->GenIni.bIRTSupported)
            && (EDD_FEATURE_ENABLE != pGlobalInitStruct->GenIni.bIRTSupported) )
        || (   (EDD_FEATURE_DISABLE != pGlobalInitStruct->NRTIni.bMIIUsed)
            && (EDD_FEATURE_ENABLE != pGlobalInitStruct->NRTIni.bMIIUsed) )
       )
    {
        EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, InterfaceID (%d) is bigger than EDD_INTERFACE_ID_MAX (%d), or illegal value(s) for bIRTSupported (%d)/bMIIUsed(%d)", 
            pGlobalInitStruct->InterfaceID, EDD_INTERFACE_ID_MAX, pGlobalInitStruct->GenIni.bIRTSupported, pGlobalInitStruct->NRTIni.bMIIUsed);
        return EDD_STS_ERR_PARAM;
    }

    pCC = &pCCLocal[pGlobalInitStruct->InterfaceID];
    pGlobalInitStruct->pInternalUse = (EDDI_UPPER_CONV_CALC_PTR_TYPE)pCC;
    
    switch (pGlobalInitStruct->GenIni.BufferCapacityUseCase)
    {
        case EDDI_GIS_USECASE_DEFAULT:
        {
            //No restrictions
            pCC->Local.Buffercapacity_us = 500;
            break;
        }
        case EDDI_GIS_USECASE_IOC_SOC1_2P:
        {
            //SOC1 with 2 ports
            if (   (EDD_HW_TYPE_USED_SOC != pGlobalInitStruct->GenIni.HWTypeUsed)
                || (pGlobalInitStruct->GenIni.NrOfIRTForwarders > 128)
                || (pGlobalInitStruct->GenIni.NrOfPorts > 2)
                || (pGlobalInitStruct->IOCIni.NrOfRTDevices > 512)
                || (pGlobalInitStruct->IOCIni.NrOfIRTDevices > 64) )
            {
                EDDI_CRT_TRACE_05(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, Illegal parameters for usecase EDDI_GIS_USECASE_IOC_SOC1_2P: HWTypeUsed(%d), NrOfIRTForwarders(%d), NrOfPorts(%d), NrOfRTDevices(%d), NrOfIRTDevices(%d)",
                    pGlobalInitStruct->GenIni.HWTypeUsed, pGlobalInitStruct->GenIni.NrOfIRTForwarders, pGlobalInitStruct->GenIni.NrOfPorts, pGlobalInitStruct->IOCIni.NrOfRTDevices, pGlobalInitStruct->IOCIni.NrOfIRTDevices);
                return EDD_STS_ERR_PARAM;
            }
            
            pCC->Local.Buffercapacity_us = 1000;
            break;
        }
        case EDDI_GIS_USECASE_IOC_SOC1_3P:
        {
            //SOC1 with 3 ports
            if (   (EDD_HW_TYPE_USED_SOC != pGlobalInitStruct->GenIni.HWTypeUsed)
                || (pGlobalInitStruct->GenIni.NrOfIRTForwarders > 128)
                || (pGlobalInitStruct->IOCIni.NrOfRTDevices > 256)
                || (pGlobalInitStruct->IOCIni.NrOfIRTDevices > 64) )
            {
                EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, Illegal parameters for usecase EDDI_GIS_USECASE_IOC_SOC1_3P: HWTypeUsed(%d), NrOfIRTForwarders(%d), NrOfRTDevices(%d), NrOfIRTDevices(%d)",
                    pGlobalInitStruct->GenIni.HWTypeUsed, pGlobalInitStruct->GenIni.NrOfIRTForwarders, pGlobalInitStruct->IOCIni.NrOfRTDevices, pGlobalInitStruct->IOCIni.NrOfIRTDevices);
                return EDD_STS_ERR_PARAM;
            }
            
            pCC->Local.Buffercapacity_us = 1000;
            break;
        }
        case EDDI_GIS_USECASE_IOD_ERTEC400:
        {
            //ERTEC400 as IOD
            if (   (EDD_HW_TYPE_USED_ERTEC_400 != pGlobalInitStruct->GenIni.HWTypeUsed)
                || (pGlobalInitStruct->GenIni.NrOfIRTForwarders > 400)
                || (pGlobalInitStruct->IOCIni.NrOfRTDevices > 0)
                || (pGlobalInitStruct->IOCIni.NrOfIRTDevices > 0) )
            {
                EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, Illegal parameters for usecase EDDI_GIS_USECASE_IOD_ERTEC400: HWTypeUsed(%d), NrOfIRTForwarders(%d), NrOfRTDevices(%d), NrOfIRTDevices(%d)",
                    pGlobalInitStruct->GenIni.HWTypeUsed, pGlobalInitStruct->GenIni.NrOfIRTForwarders, pGlobalInitStruct->IOCIni.NrOfRTDevices, pGlobalInitStruct->IOCIni.NrOfIRTDevices);
                return EDD_STS_ERR_PARAM;
            }
            
            pCC->Local.Buffercapacity_us = 1400;
            break;
        }
        default:
        {
            EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, BufferCapacityUseCase %d not supported.", pGlobalInitStruct->GenIni.BufferCapacityUseCase);
            return EDD_STS_ERR_PARAM;
        }
    }
    pGlobalInitStruct->BufferCapacity_us = pCC->Local.Buffercapacity_us;

    /*************************************************/    
    /* Local calculations                            */
    /*************************************************/    
    #if defined (EDDI_CFG_REV5)
    pCC->Local.ProvFrameFactor = EDDI_CONV_REV5_PROVFRAMEFACTOR; 
    pCC->Local.MaxPort = EDDI_CONV_REV5_MAXPORT;
    pCC->Local.FCWCountBug = ((pGlobalInitStruct->GenIni.NrOfPorts + 2)*( pGlobalInitStruct->GenIni.NrOfPorts + 2 -1) + 1);
    pCC->Local.DBCountBug = (SWI_MAX_DB_NR_FULL_NRT_FRAME*( pGlobalInitStruct->GenIni.NrOfPorts + 2)) + 1;
    pGlobalInitStruct->IOSpace.bIsInKRAM = EDDI_GIS_IOSPACE_IS_IN_KRAM;
    APDUSize = sizeof(EDDI_CRT_DATA_APDU_STATUS);
    #elif defined (EDDI_CFG_REV6)
    pCC->Local.ProvFrameFactor = EDDI_CONV_REV6_PROVFRAMEFACTOR;
    pCC->Local.MaxPort = EDDI_CONV_REV6_MAXPORT;
    pCC->Local.FCWCountBug = 0;
    pCC->Local.DBCountBug = 0;
    pGlobalInitStruct->IOSpace.bIsInKRAM = EDDI_GIS_IOSPACE_IS_IN_KRAM;
    APDUSize = sizeof(EDDI_CRT_DATA_APDU_STATUS);
    #elif defined (EDDI_CFG_REV7)
    pCC->Local.ProvFrameFactor = EDDI_CONV_REV7_PROVFRAMEFACTOR;
    (pGlobalInitStruct->GenIni.HWSubTypeUsed == EDDI_HW_SUBTYPE_USED_SOC1)?(pCC->Local.MaxPort = EDDI_CONV_REV7_MAXPORT):(pCC->Local.MaxPort = EDDI_CONV_REV7_SOC2_MAXPORT);   
    pCC->Local.FCWCountBug = 0;
    pCC->Local.DBCountBug = 0;
    pGlobalInitStruct->IOSpace.bIsInKRAM = EDDI_GIS_IOSPACE_IS_IN_PAEARAM;
    APDUSize = 0; //sizeof(EDDI_CRT_DATA_APDU_STATUS); 
    #endif                                                        
    pCC->Local.NrOfFrames = pGlobalInitStruct->GenIni.NrOfPorts * SWI_MAX_DB_NR_FULL_NRT_FRAME;
    EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pCC->Local.NrOfFrames: %d", pCC->Local.NrOfFrames);

    pCC->Local.FCWDBCountPorts = pGlobalInitStruct->GenIni.NrOfPorts * ((((pCC->Local.Buffercapacity_us *1000)+(SWI_MIN_NRT_FRAME_SIZE * SWI_TRANS_TIME_BYTE_100MBIT_NS - 1)) / (SWI_MIN_NRT_FRAME_SIZE * SWI_TRANS_TIME_BYTE_100MBIT_NS)) + SWI_MAX_DB_NR_FULL_NRT_FRAME);  //NonOrg + Org(Ports)
       
    pCC->Local.HOL_Limit_Port_Up = (LSA_UINT16)((pCC->Local.FCWDBCountPorts/pGlobalInitStruct->GenIni.NrOfPorts - 1) - (3 * SWI_MAX_DB_NR_FULL_NRT_FRAME));
    pCC->Local.HOL_LimitDBAddValuePerPort = 0;    
    if (pCC->Local.Buffercapacity_us >= 500)
    {
        // HOL limits shall be set so that at least 2 full frames can be sent each 1 ms timeslot, and 4 full frames in adjacent timeslots
        if (pCC->Local.HOL_Limit_Port_Up < (4*SWI_MAX_DB_NR_FULL_NRT_FRAME))
        {
            //correct settings
            pCC->Local.HOL_LimitDBAddValuePerPort = 4*SWI_MAX_DB_NR_FULL_NRT_FRAME - pCC->Local.HOL_Limit_Port_Up;
            pCC->Local.HOL_Limit_Port_Up += pCC->Local.HOL_LimitDBAddValuePerPort;
        }
    }

    EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pCC->Local.FCWDBCountPorts: %d", pCC->Local.FCWDBCountPorts); 
     
    if (    (EDDI_HSYNC_ROLE_APPL_SUPPORT != pGlobalInitStruct->GenIni.HSYNCRole)  
        &&  (EDDI_HSYNC_ROLE_FORWARDER != pGlobalInitStruct->GenIni.HSYNCRole)
        &&  (EDDI_HSYNC_ROLE_NONE != pGlobalInitStruct->GenIni.HSYNCRole))
    {
        EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, Illegal parameters for usecase HSYNC_ROLE (%d)", pGlobalInitStruct->GenIni.HSYNCRole);
        return EDD_STS_ERR_PARAM;
    }
    else if (   (EDDI_HSYNC_ROLE_NONE != pGlobalInitStruct->GenIni.HSYNCRole)
             && (EDD_HW_TYPE_USED_SOC != pGlobalInitStruct->GenIni.HWTypeUsed) )
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "EDDI_CalcValues, Illegal parameters for usecase HSYNC_ROLE (%d) for given HW (%u)",
                          pGlobalInitStruct->GenIni.HSYNCRole, pGlobalInitStruct->GenIni.HWTypeUsed);
        return EDD_STS_ERR_PARAM;
    }
    //else if ( maybe check for more HW configurations)
    else
    {
        const LSA_UINT32 k = (EDDI_HSYNC_ROLE_APPL_SUPPORT == pGlobalInitStruct->GenIni.HSYNCRole) ? 2 :
                            ((EDDI_HSYNC_ROLE_FORWARDER    == pGlobalInitStruct->GenIni.HSYNCRole) ? 1 : 0);

        //Calculation of NRT FCWs
        pCC->Local.HSYNC_FCWCorrectionValuePerPort = EDDI_HSyncFCW_NRT_FRAMES[k] * EDDI_HSyncFCWS_PER_NRT_FRAME[k] * EDDI_HSyncSAMPLE_FACTOR[k];
        Temp[0] = pCC->Local.FCWDBCountPorts + pCC->Local.FCWCountBug;
        Temp[1] = pCC->Local.NrOfFrames;                                                      //Org B0 TX 
        Temp[2] = ((pCC->Local.NrOfFrames + SWI_MAX_DB_NR_FULL_NRT_FRAME) * 2);               //Org B0 Local
        Temp[3] = pCC->Local.HSYNC_FCWCorrectionValuePerPort * EDDI_HSyncNR_OF_PORTS[k];      //HSync
 
        pCC->Local.NRTFCWCount = Temp[0] + Temp[1] + Temp[2] + Temp[3];
        EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pCC->Local.NRTFCWCount: Line1:%d, Line2:%d, Line3:%d, Line4:%d", Temp[0], Temp[1], Temp[2], Temp[3]); 
    
        //Calculation of NRT DBs
        pCC->Local.HSYNC_HOL_LimitDBCorrectionValuePerPort = EDDI_HSyncDB_NRT_FRAMES[k] * EDDI_HSyncDBS_PER_NRT_FRAME[k] * EDDI_HSyncSAMPLE_FACTOR[k];
        pCC->Local.HOL_Limit_Port_Up += pCC->Local.HSYNC_HOL_LimitDBCorrectionValuePerPort;

        Temp[0] = pCC->Local.FCWDBCountPorts + pCC->Local.DBCountBug + pCC->Local.HOL_LimitDBAddValuePerPort * pGlobalInitStruct->GenIni.NrOfPorts;
        Temp[1] = SWI_MAX_DB_NR_FULL_NRT_FRAME;                                                   //Ports
        Temp[2] = ((pCC->Local.NrOfFrames + SWI_MAX_DB_NR_FULL_NRT_FRAME) * 2);                   //Org B0 Local
        Temp[3] = pCC->Local.HSYNC_HOL_LimitDBCorrectionValuePerPort * EDDI_HSyncNR_OF_PORTS[k];  //HSync
    
        pCC->Local.NRTDBCount =  Temp[0] + Temp[1] + Temp[2] + Temp[3];
        EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pCC->Local.NRTDBCount: Line1:%d, Line2:%d, Line3:%d, Line4:%d", Temp[0], Temp[1], Temp[2], Temp[3]); 
    }

    /*************************************************/    
    /* IOC PI-SIZE                                   */
    /*************************************************/    
    //PI-Size calculation: INPUTS
    IOCCalc.MinFrameSizeRT  = EDDI_CONV_MIN_FRAMESIZE_CONS12;
    IOCCalc.MinFrameSizeIRT = EDDI_CONV_MIN_FRAMESIZE_CONS3;
    framefactor             = 1;

    for (i=0;i<2;i++)
    {
        IOCQV = (0==i)?(LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfQVConsumers:(LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfQVProviders;     //QV-calculation
        IOCQV = IOCQV * (EDDI_CONV_CALC_ROUNDn((LSA_UINT32)pGlobalInitStruct->IOCIni.MaxMCRDataSize, 4) + APDUSize) * framefactor;
        EDDI_CONV_CALC_ROUNDn0(IOCQV, IOCQV, 8);                                                                                        

        IOCCalc.NetData = pGlobalInitStruct->IOCIni.LogicalAddressSpace + pGlobalInitStruct->IOCIni.NrOfSubmod;	//worstcase: every frame contains 1 byte only, these data have to be split onto the min. nr. of full frames
        IOCCalc.RestNet    = IOCCalc.NetData - (LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices;
        IOCCalc.RestNetIRT = IOCCalc.RestNet;

        IOCCalc.FullFrames    = IOCCalc.RestNet/((LSA_UINT32)pGlobalInitStruct->IOCIni.MaxIOCRDataSize-1); //nr of full frames for the remaining data
        IOCCalc.FullFramesIRT = IOCCalc.FullFrames;

        IOCCalc.AddBytes    =   EDDI_MAX(((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices - IOCCalc.FullFrames)*(IOCCalc.MinFrameSizeRT-1), 0) 
                              + ((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices * APDUSize);   //additional space needed for padding and APDUStatus
        IOCCalc.AddBytesIRT =   ((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfIRTDevices * (IOCCalc.MinFrameSizeIRT - 1))
                              + ((((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices - (LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfIRTDevices)*(IOCCalc.MinFrameSizeRT-1)))
                              + ((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices * APDUSize)
                              - ((EDDI_MIN(IOCCalc.MinFrameSizeIRT, IOCCalc.MinFrameSizeRT)-1) * IOCCalc.FullFramesIRT);

        IOCCalc.AlignFullFrames    = (EDDI_CONV_CALC_ROUNDn((LSA_UINT32)pGlobalInitStruct->IOCIni.MaxIOCRDataSize, 4) - (LSA_UINT32)pGlobalInitStruct->IOCIni.MaxIOCRDataSize) * IOCCalc.FullFrames;    //additional alignment for full frames
        IOCCalc.AlignFullFramesIRT = (EDDI_CONV_CALC_ROUNDn((LSA_UINT32)pGlobalInitStruct->IOCIni.MaxIOCRDataSize,4) - (LSA_UINT32)pGlobalInitStruct->IOCIni.MaxIOCRDataSize) * IOCCalc.FullFramesIRT;

        IOCCalc.AlignRestFrames    = (EDDI_CONV_CALC_ROUNDn(EDDI_MAX(IOCCalc.MinFrameSizeRT,1), 4) - EDDI_MAX(IOCCalc.MinFrameSizeRT, 1)) * ((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices - IOCCalc.FullFrames);
        IOCCalc.AlignRestFramesIRT = ((EDDI_CONV_CALC_ROUNDn((EDDI_MAX(EDDI_MIN(IOCCalc.MinFrameSizeRT, IOCCalc.MinFrameSizeIRT), 1)),4)) - EDDI_MAX(EDDI_MIN(IOCCalc.MinFrameSizeRT, IOCCalc.MinFrameSizeIRT), 1)) * ((LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices - IOCCalc.FullFramesIRT);

        IOCCalc.IOCRTAll  = IOCCalc.NetData + IOCCalc.AddBytes + IOCCalc.AlignFullFrames + IOCCalc.AlignRestFrames;
        IOCCalc.IOCIRTAll = IOCCalc.NetData + IOCCalc.AddBytesIRT + IOCCalc.AlignFullFramesIRT + IOCCalc.AlignRestFramesIRT;

        EDDI_CRT_TRACE_07(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, IOC: NetData:%d, RestNet:%d, FullFrames:%d, AddBytes:%d, AlignFullFrames:%d, AlignRestFrames:%d, IOCRTAll:%d", 
             IOCCalc.NetData, IOCCalc.RestNet, IOCCalc.FullFrames, IOCCalc.AddBytes, IOCCalc.AlignFullFrames, IOCCalc.AlignRestFrames, IOCCalc.IOCRTAll); 
        EDDI_CRT_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, IOC: RestNetIRT:%d, FullFramesIRT:%d, AddBytesIRT:%d, AlignFullFramesIRT:%d, AlignRestFramesIRT:%d, IOCIRTAll:%d", 
             IOCCalc.RestNetIRT, IOCCalc.FullFramesIRT, IOCCalc.AddBytesIRT, IOCCalc.AlignFullFramesIRT, IOCCalc.AlignRestFramesIRT, IOCCalc.IOCIRTAll); 
        
        if (0 == i)
        {
            //inputs
            #if (EDDI_CONV_CONS_PART_DATALEN_ERR_SIZE == 0)
            LSA_UINT32 const IOCConsPartDataLenErrIn = 0;
            #else
            LSA_UINT32 const IOCConsPartDataLenErrIn = (LSA_UINT32)pGlobalInitStruct->IOCIni.NrOfRTDevices * EDDI_CONV_CONS_PART_DATALEN_ERR_SIZE;
            #endif
            EDDI_CONV_CALC_ROUNDn0(pCC->IOC.PiSizeInput, EDDI_MAX(IOCCalc.IOCRTAll, IOCCalc.IOCIRTAll), 8);
            pCC->IOC.PiSizeInput = pCC->IOC.PiSizeInput * framefactor + IOCQV + IOCConsPartDataLenErrIn;
            EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, IOC-INPUTS: PiSizeInput:%d, IOCQV:%d", pCC->IOC.PiSizeInput, IOCQV); 
        }
        else
        {
            //outputs
            EDDI_CONV_CALC_ROUNDn0(pCC->IOC.PiSizeOutput, EDDI_MAX(IOCCalc.IOCRTAll, IOCCalc.IOCIRTAll), 8);
            pCC->IOC.PiSizeOutput = pCC->IOC.PiSizeOutput * framefactor + IOCQV;
            EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, IOC-OUTPUTS: PiSizeOutput:%d, IOCQV:%d", pCC->IOC.PiSizeOutput, IOCQV); 
        }
        
        //outputs are next
        IOCCalc.MinFrameSizeRT  = EDDI_CONV_MIN_FRAMESIZE_PROV;
        IOCCalc.MinFrameSizeIRT = EDDI_CONV_MIN_FRAMESIZE_PROV;
        framefactor             = pCC->Local.ProvFrameFactor;
    } //end for

    pCC->IOC.NrOfRtUcProvider  = pGlobalInitStruct->IOCIni.NrOfRTDevices;
    pCC->IOC.NrOfRtMcProvider  = EDDI_CONV_NR_OF_RT_MC_PROVIDER;
    pCC->IOC.NrOfIrtUcProvider = pGlobalInitStruct->IOCIni.NrOfIRTDevices;
    pCC->IOC.NrOfIrtMcProvider = pGlobalInitStruct->IOCIni.NrOfQVProviders;
    pCC->IOC.NrOfRtUcConsumer  = pGlobalInitStruct->IOCIni.NrOfRTDevices;
    pCC->IOC.NrOfRtMcConsumer  = EDDI_CONV_NR_OF_RT_MC_CONSUMER;
    pCC->IOC.NrOfIrtUcConsumer = pGlobalInitStruct->IOCIni.NrOfIRTDevices;
    pCC->IOC.NrOfIrtMcConsumer = pGlobalInitStruct->IOCIni.NrOfQVConsumers;
    
    /*************************************************/    
    /* IOD PI-SIZE                                   */
    /*************************************************/    
    if (EDD_FEATURE_ENABLE == pGlobalInitStruct->IODIni.AlternativeCalc)
    {
        /*Factor1 = "Nr of Instances" * ("SharedAR supported"?"Nr of ARs":1)
        Factor2 = ERTEC200?3:1  (hier mal vereinfacht, wegen Provider-3Puffer-System)
        InputSize    = Factor1 * Factor2 * ("InputBytes" + 2*"Nr of Submodules")
        OutputSize   = Factor1           * ("OutputBytes" + 2*"Nr of Submodules")*/
        Temp[1] = 1;
        Temp[0] = pGlobalInitStruct->IODIni.NrOfInstances * ((pGlobalInitStruct->IODIni.SharedARSupported)?pGlobalInitStruct->IODIni.NrOfARs:1);
        #if defined (EDDI_CFG_REV6)
        Temp[1] = 3; 
        #endif

        pCC->IOD.PiSizeInput = (Temp[0] * Temp[1]) * (pGlobalInitStruct->IODIni.InputBytes + 2 * pGlobalInitStruct->IODIni.NrOfSubmodules);
        EDDI_CONV_CALC_ROUNDn0(pCC->IOD.PiSizeInput, pCC->IOD.PiSizeInput, 8);

        pCC->IOD.PiSizeOutput = Temp[0] * (pGlobalInitStruct->IODIni.OutputBytes + 2 * pGlobalInitStruct->IODIni.NrOfSubmodules);
        EDDI_CONV_CALC_ROUNDn0(pCC->IOD.PiSizeOutput, pCC->IOD.PiSizeOutput, 8);
    }
    else
    {
        Temp[0] = EDDI_CONV_CALC_ROUNDn((EDDI_MAX(EDDI_MAX(EDDI_CONV_MIN_FRAMESIZE_CONS12, EDDI_CONV_MIN_FRAMESIZE_CONS3), pGlobalInitStruct->IODIni.MaxMCRDataSize) + APDUSize), 4);    //MC-Framesize
        Temp[1] = EDDI_CONV_CALC_ROUNDn((EDDI_MAX(EDDI_MAX(EDDI_CONV_MIN_FRAMESIZE_CONS12, EDDI_CONV_MIN_FRAMESIZE_CONS3), pGlobalInitStruct->IODIni.MaxICRDataSize) + APDUSize), 4);    //Input-Framesize
        Temp[2] = EDDI_CONV_CALC_ROUNDn((EDDI_MAX(EDDI_CONV_MIN_FRAMESIZE_PROV, pGlobalInitStruct->IODIni.MaxOCRDataSize) + APDUSize), 4);    //Output-Framesize
    
        pCC->IOD.PiSizeInput = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs * pCC->Local.ProvFrameFactor *  
                               (Temp[0] + Temp[1]);
        EDDI_CONV_CALC_ROUNDn0(pCC->IOD.PiSizeInput, pCC->IOD.PiSizeInput, 8);

        pCC->IOD.PiSizeOutput = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs *   
                                Temp[2];
        EDDI_CONV_CALC_ROUNDn0(pCC->IOD.PiSizeOutput, pCC->IOD.PiSizeOutput, 8);
    }

    EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pCC->IOD.PiSizeInput:%d, pCC->IOD.PiSizeOutput:%d", pCC->IOD.PiSizeInput, pCC->IOD.PiSizeOutput); 
    
    pCC->IOD.NrOfRtUcProvider  = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs;
    pCC->IOD.NrOfRtMcProvider  = 0; //no RT-MC
    pCC->IOD.NrOfIrtUcProvider = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs;
    pCC->IOD.NrOfIrtMcProvider = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs;
    pCC->IOD.NrOfRtUcConsumer  = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs;
    pCC->IOD.NrOfRtMcConsumer  = 0; //no RT-MC
    pCC->IOD.NrOfIrtUcConsumer = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs;
    pCC->IOD.NrOfIrtMcConsumer = pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs;
  
    //Provider/Consumer Count Values
    pCC->ConsumerCntClass12 = pCC->IOC.NrOfRtUcConsumer + pCC->IOC.NrOfRtMcConsumer + pCC->IOD.NrOfRtUcConsumer + pCC->IOD.NrOfRtMcConsumer;
    pCC->ProviderCnt        = pCC->IOC.NrOfRtUcProvider + pCC->IOC.NrOfRtMcProvider + pCC->IOD.NrOfRtUcProvider + pCC->IOD.NrOfRtMcProvider;
    pCC->ConsumerCntClass3  = pCC->IOC.NrOfIrtUcConsumer + pCC->IOC.NrOfIrtMcConsumer + pCC->IOD.NrOfIrtUcConsumer + pCC->IOD.NrOfIrtMcConsumer;
    pCC->ProviderCntClass3  = pCC->IOC.NrOfIrtUcProvider + pCC->IOC.NrOfIrtMcProvider + pCC->IOD.NrOfIrtUcProvider + pCC->IOD.NrOfIrtMcProvider;
    pCC->ForwarderCntClass3 = pGlobalInitStruct->GenIni.NrOfIRTForwarders;
    
    EDDI_CRT_TRACE_05(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pCC->ConsumerCntClass12:%d, pCC->ProviderCnt:%d, pCC->ConsumerCntClass3:%d, pCC->ProviderCntClass3:%d, pCC->ForwarderCntClass3:%d", pCC->ConsumerCntClass12, pCC->ProviderCnt, pCC->ConsumerCntClass3, pCC->ProviderCntClass3, pCC->ForwarderCntClass3 );
    
    //OUT Parameter 
    pGlobalInitStruct->IOSpace.PNIOC_MCInput  = (LSA_UINT16)pCC->IOC.PiSizeInput;
    pGlobalInitStruct->IOSpace.PNIOC_MCOutput = (LSA_UINT16)pCC->IOC.PiSizeOutput;
    pGlobalInitStruct->IOSpace.PNIODInput     = (LSA_UINT16)pCC->IOD.PiSizeInput;
    pGlobalInitStruct->IOSpace.PNIODOutput    = (LSA_UINT16)pCC->IOD.PiSizeOutput; 
    
    /*************************************************/    
    /* CRT SPACE                                     */
    /*************************************************/    
    Temp[0] =  (((pCC->ConsumerCntClass12 + pCC->ConsumerCntClass3) * sizeof(EDDI_SER10_TIMER_SCORB_TYPE) + 7) & 0xFFFFFFF8); 
    Temp[1] =  ((pCC->ConsumerCntClass12 + pCC->ConsumerCntClass3 + 1)/2) * 2;                                                  // Size of TimerScoreboard's + 2 * ConsumerScoreboard
    Temp[2] =  (MAX_TRACE_DIAG_ENTRIES * sizeof(EDDI_TRACE_DIAG_ENTRY_TYPE));                                                   // IRTTrace-Area
    Temp[3] =  PROF_KRAM_SIZE;                                                                                                  // Size of profile-memory (320B) 

    pGlobalInitStruct->CRTSpace = Temp[0] + Temp[1] + Temp[2] + Temp[3]; 

    EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pGlobalInitStruct->CRTSpace: Line1:%d, Line2:%d, Line3:%d, Line4:%d", Temp[0], Temp[1], Temp[2], Temp[3]); 
    
    /*************************************************/    
    /* SRT SPACE                                     */
    /*************************************************/    
    #if defined (EDDI_CFG_REV5)
    Temp[0] =  (EDDI_TREE_MAX_BINARY_REDUCTION -1) * (sizeof(EDDI_SER10_BCW_MOD_TYPE) + sizeof(EDDI_SER10_NOOP_TYPE));                            // Rev5, ACW-Phasetree
    #else
    Temp[0] =  ( (EDDI_TREE_MAX_BINARY_REDUCTION * 2 - 2 ) * 4) + (13 * sizeof(EDDI_SER10_RCW_RED_TYPE) );                                        // Rev6, Rev7 ACW-Phasetree
    #endif 

    //Temp[1] =  ((pCC->ConsumerCntClass12 / 8) * 4); 

    Temp[1] = EDDI_ConvGetAcwKramSize(pCC->ConsumerCntClass12);                                                                                   // Header for RX-List's
                                                                                                  
    Temp[2] =  (pCC->ConsumerCntClass12 * sizeof(EDDI_SER10_ACW_RCV_TYPE));                                                                       // RX-ACWs
    Temp[3] =  8;                                                                                                                                 // Header for TX-Listen
    Temp[4] =  ((pCC->ProviderCnt+1) * (sizeof(EDDI_SER10_ACW_SND_TYPE) + TBufferSize));                                                                         // TX-ACWs and transportbuffer for IO-Update
    Temp[5] =  (pCC->ConsumerCntClass3 * sizeof(EDDI_CRT_DATA_APDU_STATUS) * ((EDD_FEATURE_ENABLE == pGlobalInitStruct->GenIni.bIRTSupported)?1:0));             // AUX-APDU-Buffer
    Temp[6] =  ((LSA_UINT16)(pGlobalInitStruct->GenIni.NrOfPorts * EDDI_MIN(pCC->ProviderCnt, EDDI_CRT_PROVIDER_DEFAULT_PER_1MS)) * sizeof(EDDI_SER10_SRT_FCW_TYPE));           // SRT-FCWs (IRTE internal Element's, do not mix it up with IRT-FCWs!)
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
    Temp[7] =  ((pCC->ConsumerCntClass12 + pCC->ProviderCnt) * sizeof(EDDI_CRT_DATA_APDU_STATUS));                                               // REV5, REV6 APDU-Stati
    #else
    Temp[7] =  ((pCC->ConsumerCntClass12 + pCC->ProviderCnt) * sizeof(EDDI_CRT_PAEA_APDU_STATUS));                                               // REV7, APDU-Stati
    #endif
                                    
    pGlobalInitStruct->SRTSpace = Temp[0] + Temp[1] + Temp[2] + Temp[3] + Temp[4] + Temp[5]+ Temp[6] + Temp[7];
    
    EDDI_CRT_TRACE_08(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pGlobalInitStruct->SRTSpace: Line1:%d, Line2:%d, Line3:%d, Line4:%d, Line5:%d, Line6:%d, Line7:%d, Line8:%d", Temp[0], Temp[1], Temp[2], Temp[3], Temp[4], Temp[5], Temp[6], Temp[7]); 
    
    /*************************************************/    
    /* IRT SPACE                                     */
    /*************************************************/    
    if (EDD_FEATURE_ENABLE != pGlobalInitStruct->GenIni.bIRTSupported)
    {
        pCC->ProviderCntClass3  = 1;
        pCC->ConsumerCntClass3  = 1;
        pCC->ForwarderCntClass3 = 1;    
    }
    else
    {
        if (!pCC->ProviderCntClass3)
        {
            pCC->ProviderCntClass3 = 1;
        }
        if (!pCC->ConsumerCntClass3)
        {
            pCC->ConsumerCntClass3 = 1;
        }
        if (!pCC->ForwarderCntClass3)
        {
            pCC->ForwarderCntClass3 = 1;
        }
    }

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)                                                                        // Rev5, FCW Phasentrees all Port's
    Temp[0] = ((((EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX * 2 - 2 ) * 4) + (13 * sizeof(EDDI_SER10_RCW_RED_TYPE) )) * pGlobalInitStruct->GenIni.NrOfPorts * 2);                  
    #else
    Temp[0] = ((EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX - 1) * ( sizeof(EDDI_SER10_BCW_MOD_TYPE) + sizeof(EDDI_SER10_NOOP_TYPE) )  * pGlobalInitStruct->GenIni.NrOfPorts * 2);     // Rev6, Rev7, FCW Phasentrees all Port's
    #endif 

    Temp[1] = (pGlobalInitStruct->GenIni.NrOfPorts * 4 * 2);                                                                                // List-anchor
    
    Temp[2] = (sizeof(EDDI_SER10_NOOP_TYPE)*3 + sizeof(EDDI_SER10_FCW_RCV_TYPE) + sizeof(EDDI_SER10_SOL_TYPE)*2 + sizeof(EDDI_SER10_LMW_TYPE)*3 + sizeof(EDDI_SER10_FCW_SND_TYPE)*2 + sizeof(EDDI_SER10_EOL_TYPE)*2 ) * pGlobalInitStruct->GenIni.NrOfPorts;  // Common resources
                     
    Temp[3] = (pCC->ProviderCntClass3 * pGlobalInitStruct->GenIni.NrOfPorts * (sizeof(EDDI_SER10_FCW_SND_TYPE) + TBufferSize)) ;            // IRT TX-FCWs
    
    Temp[4] = ((pCC->ConsumerCntClass3 + pGlobalInitStruct->IOCIni.NrOfRedIRTDevices + pGlobalInitStruct->IOCIni.NrOfRedQVConsumers +       // IRT RX-FCWs incl. Resourcen for redundancy
              (pGlobalInitStruct->IODIni.NrOfInstances * pGlobalInitStruct->IODIni.NrOfARs) * 2) * sizeof(EDDI_SER10_FCW_RCV_TYPE));  

    Temp[5] = (pCC->ForwarderCntClass3 * sizeof(EDDI_SER10_FCW_RCV_FORWARDER_TYPE));                                                        // IRT Forwarder-FCWs
    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
    Temp[6] = 0; //contained in IO-space                                                                                                    // REV5, REV6 APDU-Stati
    #else
    Temp[6] = ((pCC->ConsumerCntClass3 + pCC->ProviderCntClass3) * sizeof(EDDI_CRT_PAEA_APDU_STATUS));                                      // REV7, APDU-Stati
    #endif                               

    {
        LSA_UINT32 const NrofSSPorts = ((bSSOn?1:0) * EDDI_MIN((bRedOn?2:1), pGlobalInitStruct->GenIni.NrOfPorts));
        LSA_UINT32 const NrofSMPorts = (bSMOn?pGlobalInitStruct->GenIni.NrOfPorts:0);

        Temp[7] = EDDI_MAX((bSSOn?2:0),(bSMOn?1:0)) * NrofSSPorts * sizeof(EDDI_SER10_FCW_RCV_TYPE);        //SyncInRed-RCV
        Temp[8] = NrofSMPorts * (sizeof(EDDI_SER10_FCW_SND_TYPE) + TBufferSize);                            //SyncInRed-SND
        Temp[9] = EDDI_MAX((NrofSSPorts * ((bSSOn?2:0)*2) * EDDI_CONV_CALC_ROUNDn(EDDI_SYNC_FRAME_KRAMDATA_SIZE + 4, 8)), ((NrofSSPorts + NrofSMPorts) * 2 * EDDI_CONV_CALC_ROUNDn(EDDI_SYNC_FRAME_KRAMDATA_SIZE + 4, 8)));  //Buffer
    }

    pGlobalInitStruct->IRTSpace = Temp[0] + Temp[1] + Temp[2] + Temp[3] + Temp[4] + Temp[5] + Temp[6] + Temp[7] + Temp[8] + Temp[9];

    EDDI_CRT_TRACE_10(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pGlobalInitStruct->IRTSpace: Line1:%d, Line2:%d, Line3:%d, Line4:%d, Line5:%d, Line6:%d, Line7:%d, Line8:%d, Line9:%d, Line10:%d", Temp[0], Temp[1], Temp[2], Temp[3], Temp[4], Temp[5], Temp[6], Temp[7], Temp[8], Temp[9]); 

    /*************************************************/    
    /* SWI SPACE                                     */
    /*************************************************/                                                                  
    Temp[0] = EDDI_CONV_UCMC_TABLE_MAX_ENTRY * sizeof(EDDI_SER_UCMC_TYPE);                                                                      // UC/MC-MAC Table
    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    {
        LSA_UINT32  dwSizeHeader, dwSizeAllTable;                                                                                               // MC-MAC Table
            
        //Size of table header has to be a multiple of 8Byte
        dwSizeHeader = ((sizeof(EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE) * EDDI_CONV_MC_MAX_SUBTABLES) + 7UL) & ~7UL;

        //Size of a table without header has to be a multiple of 8Byte
        //Size of all tables has to be a multiple of 8Byte + 1 entry (to dynamically add static MC entries at runtime)
        dwSizeAllTable = (sizeof(EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE) * EDDI_CONV_MC_MAX_ENTRIES_PER_SUBTABLE) * (EDDI_CONV_MC_MAX_SUBTABLES + 1);

        //Overall KRAM range for the complete MC table
        Temp[1] = dwSizeAllTable + dwSizeHeader;
    }
    #else
    Temp[1] = 0;
    #endif //(EDDI_CFG_ENABLE_MC_FDB)
    pGlobalInitStruct->SWISpace = Temp[0] + Temp[1];
    EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pGlobalInitStruct->SWISpace: Line1:%d Line2:%d", Temp[0], Temp[1]); 

    /*************************************************/
    /* NRT SPACE                                     */
    /*************************************************/
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    Temp[0] = (32 + EDDI_MAX_DCP_FILTER_STRING_BUFFER_LEN + sizeof(EDDI_R6_DCP_FILTER_TABLE_TYPE));         //ARP-Filter + DCP-Filter for 1 entry
    #else
    Temp[0] = 0;
    #endif
    Temp[1] = ((pGlobalInitStruct->GenIni.NrOfPorts + 2) * sizeof(EDDI_SER_NRT_PRIO_TYPE));                 //Prio Lists
    Temp[2] = (sizeof(EDDI_SER_NRT_HEAD_TYPE));                                                             //Emptylists-Header + ISO-handing over area
    Temp[3] = ((LSA_UINT32)pCC->Local.NRTFCWCount * sizeof(EDDI_SER_NRT_FCW_TYPE));                         //NRT-FCWs
    Temp[4] = ((LSA_UINT32)pCC->Local.NRTDBCount * sizeof(EDDI_SER_NRT_DB_TYPE));                           //NRT-DBs
    Temp[5] = ((pCC->Local.MaxPort * SER_STATISTIC_PORT_SIZE) + SER_STATISTIC_BASE_OFFSET);                 //Size of profile-memory (320B)

    pGlobalInitStruct->NRTSpace =  Temp[0] + Temp[1] + Temp[2] + Temp[3] + Temp[4] + Temp[5];

    EDDI_CRT_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, pGlobalInitStruct->NRTSpace: Line1:%d, Line2:%d, Line3:%d, Line4:%d, Line5:%d, Line6:%d", Temp[0], Temp[1], Temp[2], Temp[3], Temp[4], Temp[5]);

    /*************************************************/
    /* FINAL CALCULATION                             */
    /*************************************************/
    pGlobalInitStruct->KRAMUsed = 
                                 #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
                                   pGlobalInitStruct->CRTSpace 
                                 + pGlobalInitStruct->SRTSpace
                                 + pGlobalInitStruct->IRTSpace 
                                 + pGlobalInitStruct->SWISpace 
                                 + pGlobalInitStruct->NRTSpace
                                 + pGlobalInitStruct->IOSpace.PNIOC_MCInput
                                 + pGlobalInitStruct->IOSpace.PNIOC_MCOutput
                                 + pGlobalInitStruct->IOSpace.PNIODInput
                                 + pGlobalInitStruct->IOSpace.PNIODOutput;
                                 #else //Rev7
                                   pGlobalInitStruct->CRTSpace 
                                 + pGlobalInitStruct->SRTSpace 
                                 + pGlobalInitStruct->IRTSpace 
                                 + pGlobalInitStruct->SWISpace 
                                 + pGlobalInitStruct->NRTSpace;
                                 #endif    
   
    pGlobalInitStruct->KRAMUsed += EDDI_CONV_KRAM_RESERVE;
      
    #if defined (EDDI_CFG_REV5)    
    pGlobalInitStruct->KRAMFree = (LSA_INT32)EDDI_KRAM_SIZE_ERTEC400 - (LSA_INT32)pGlobalInitStruct->KRAMUsed;
    #elif defined (EDDI_CFG_REV6)
    pGlobalInitStruct->KRAMFree = (LSA_INT32)EDDI_KRAM_SIZE_ERTEC200 - (LSA_INT32)pGlobalInitStruct->KRAMUsed;
    #elif defined (EDDI_CFG_REV7)
    pGlobalInitStruct->KRAMFree = (LSA_INT32)EDDI_KRAM_SIZE_SOC - (LSA_INT32)pGlobalInitStruct->KRAMUsed;
    #endif

    EDDI_CRT_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, Space sizes: IOSpace (%d bytes), CRTSpace (%d bytes), SRTSpace (%d bytes), IRTSpace (%d bytes), SWISpace (%d bytes), NRTSpace (%d bytes)", 0 /*pGlobalInitStruct->IOSpace -> TODO*/, pGlobalInitStruct->CRTSpace, pGlobalInitStruct->SRTSpace, pGlobalInitStruct->IRTSpace,  pGlobalInitStruct->SWISpace,  pGlobalInitStruct->NRTSpace);
    EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_CalcValues, KRAMUsed (%d bytes), KRAMFree (%d bytes)", pGlobalInitStruct->KRAMUsed, pGlobalInitStruct->KRAMFree);

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CalcValues<-");

    return EDD_STS_OK;                                                                                                                                                                           
}    
/*---------------------- end [subroutine] ---------------------------------*/

                            
/***************************************************************************/
/* F u n c t i o n:       eddi_InitDPBWithDefaults()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_InitDPBWithDefaults( EDDI_UPPER_DPB_PTR_TYPE         pDPB,
			                                               EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct )
{
    LSA_UINT32                     UsrPortIndex;
    LSA_RESULT                     Status = EDD_STS_OK;
    EDDI_UPPER_CONV_CALC_PTR_TYPE  pCC;

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_InitDPBWithDefaults->");

    if (EDDI_NULL_PTR == pGlobalInitStruct)
    {
        EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eddi_InitDPBWithDefaults, pGlobalInitStruct is not allocated"); 
        EDDI_Excp("eddi_InitDPBWithDefaults, pGlobalInitStruct is not allocated", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_PARAM;
    }

    if (EDDI_NULL_PTR == pGlobalInitStruct->pInternalUse)
    {
        //If the structure is a null pointer, then allocate and calc it 
        Status = EDDI_CalcValues((EDDI_UPPER_CONV_CALC_PTR_TYPE)pGlobalInitStruct->pInternalUse, pGlobalInitStruct);
        if (EDD_STS_OK != Status)
        {
            EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitDPBWithDefaults, Error in EDDI_CalcValues, Status:%d", Status);
            return EDD_STS_ERR_PARAM;
        } 
    }

    pCC = (EDDI_UPPER_CONV_CALC_PTR_TYPE)pGlobalInitStruct->pInternalUse;    

    pDPB->InterfaceID                   = pGlobalInitStruct->InterfaceID;
    pDPB->IRTE_SWI_BaseAdr              = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->IRTE_SWI_BaseAdr_LBU_16Bit    = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    #if !defined (EDDI_CFG_3BIF_2PROC)
    pDPB->GSharedRAM_BaseAdr            = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    #endif
    pDPB->NRTMEM_LowerLimit             = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->NRTMEM_UpperLimit             = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->BOARD_SDRAM_BaseAddr          = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->ERTECx00_SCRB_BaseAddr        = EDDI_CONV_ERTEC_X00_SCRB;     //Param
    #if defined (EDDI_CFG_REV7)
    pDPB->PAEA_BaseAdr                  = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    #else
    pDPB->PAEA_BaseAdr                  = 0;                            //not used!
    #endif
    
    pDPB->EnableReset                   = EDD_FEATURE_ENABLE;

    pDPB->bSupportIRTflex               = EDD_FEATURE_DISABLE;
    pDPB->bDisableMaxPortDelayCheck     = EDDI_DPB_ENABLE_MAX_PORTDELAY_CHECK;

    pDPB->PortMap.PortCnt               = pGlobalInitStruct->GenIni.NrOfPorts;

    pDPB->MaxInterfaceCntOfAllEDD       = EDD_CFG_MAX_INTERFACE_CNT;
    pDPB->MaxPortCntOfAllEDD            = EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE;

    //SII configuration parameters
    pDPB->SII_IrqSelector               = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->SII_IrqNumber                 = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->SII_ExtTimerInterval          = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param
    pDPB->SII_Mode                      = EDDI_ILLEGAL_ADDR_VALUE_32;   //Param

    //I2C configuration parameters
    pDPB->I2C_Type                      = pGlobalInitStruct->GenIni.I2C_Type;

    #if defined (EDDI_CFG_REV5)
    //Info: Default portmapping CP1616: 1:1

    if (pDPB->PortMap.PortCnt > EDDI_CONV_REV5_MAXPORT)
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitDPBWithDefaults, Error PortCnt (%d) is bigger than MaxPortCnt (%d)", pDPB->PortMap.PortCnt, EDDI_CONV_REV5_MAXPORT);
        return EDD_STS_ERR_PARAM;
    }

    #elif defined (EDDI_CFG_REV6)           
    //Info: Default portmapping EB200: 1:1

    if (pDPB->PortMap.PortCnt > EDDI_CONV_REV6_MAXPORT)
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitDPBWithDefaults, Error PortCnt (%d) is bigger than MaxPortCnt (%d)", pDPB->PortMap.PortCnt, EDDI_CONV_REV6_MAXPORT);
        return EDD_STS_ERR_PARAM;
    }

    #elif defined (EDDI_CFG_REV7)                                
    //Info: Default portmapping SOC1 PCIe Card: 1:1
    //Info: Default portmapping SOC1 PCI Card: UsrPort ==> UsrPortIndex  | HWPortIndex ==> HWPort
    //                                            1             0        |      1            2
    //                                            2             1        |      2            3
    //                                            3             2        |      0            1

    if (pDPB->PortMap.PortCnt > EDDI_CONV_REV7_MAXPORT) 
    {
        EDDI_CRT_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitDPBWithDefaults, Error PortCnt (%d) is bigger than MaxPortCnt (%d)", pDPB->PortMap.PortCnt, EDDI_CONV_REV7_MAXPORT);
        return EDD_STS_ERR_PARAM;
    }

    #endif

	//HW portmapping
	//Note: here all ports are connected 1:1 -> UserPortID == HardwarePortID
    //Set Default portmapping: 1:1
    //****************************
    //UsrPort ==> UsrPortIndex  | HWPortIndex ==> HWPort
    //   1             0        |      0            1
    //   2             1        |      1            2  
    //   3             2        |      2            3
    //   4             3        |      3            4
    //
    //UsrPortID_x_to_HWPort_y[UsrPortIndex] = HwPortId;
    switch (pDPB->PortMap.PortCnt)
    {
        case 4: pDPB->PortMap.UsrPortID_x_to_HWPort_y[3] = 4UL; //lint -fallthrough
        case 3: pDPB->PortMap.UsrPortID_x_to_HWPort_y[2] = 3UL; //lint -fallthrough
        case 2: pDPB->PortMap.UsrPortID_x_to_HWPort_y[1] = 2UL; //lint -fallthrough
        case 1:
        {
            pDPB->PortMap.UsrPortID_x_to_HWPort_y[0] = 1UL;
            break;
        }
        default:
        {
            EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitDPBWithDefaults, Error PortCnt is 0");
            return EDD_STS_ERR_PARAM;
        }
    }

    for (UsrPortIndex = pDPB->PortMap.PortCnt; UsrPortIndex < EDD_CFG_MAX_PORT_CNT; UsrPortIndex++)
	{
		//UsrPortID_x_to_HWPort_y[UsrPortIndex] = HwPortId;
		pDPB->PortMap.UsrPortID_x_to_HWPort_y[UsrPortIndex] = EDDI_PORT_NOT_CONNECTED; //Initialize
	}

    #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
    {
        pDPB->KRam.offset_ProcessImageEnd = (((pCC->IOC.PiSizeInput + pCC->IOC.PiSizeOutput) + (pCC->IOD.PiSizeInput + pCC->IOD.PiSizeOutput) + (pCC->ConsumerCntClass12 + pCC->ProviderCnt + pCC->ConsumerCntClass3 + pCC->ProviderCntClass3) * sizeof(EDDI_CRT_DATA_APDU_STATUS))+7) & 0xFFFFFFF8UL;    //alignment 8 Byte
    }
    #else
    pDPB->KRam.offset_ProcessImageEnd = 0; //assuming use of PAEARam
    LSA_UNUSED_ARG(pCC); //satisfy lint!
    #endif

    #if defined (EDDI_CFG_REV5)    
    pDPB->KRam.size_reserved_for_eddi = EDDI_KRAM_SIZE_ERTEC400 - pDPB->KRam.offset_ProcessImageEnd;
    #elif defined (EDDI_CFG_REV6)
    pDPB->KRam.size_reserved_for_eddi = EDDI_KRAM_SIZE_ERTEC200 - pDPB->KRam.offset_ProcessImageEnd;
    #elif defined (EDDI_CFG_REV7)
    pDPB->KRam.size_reserved_for_eddi = EDDI_KRAM_SIZE_SOC - pDPB->KRam.offset_ProcessImageEnd;
    #endif
    
    pDPB->Bootloader.Activate_for_use_as_pci_slave  = EDD_FEATURE_ENABLE;

    //Feature support flags
    pDPB->FeatureSupport.bMRPDSupported             = EDD_FEATURE_DISABLE;
    pDPB->FeatureSupport.FragmentationtypeSupported = EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_NO;
    pDPB->FeatureSupport.IRTForwardingModeSupported = EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE;
    pDPB->FeatureSupport.MaxDFPFrames               = 0;
    #if defined (EDDI_CFG_REV7)
    pDPB->FeatureSupport.ClusterIPSupport           = EDD_FEATURE_ENABLE; 
    #else
    pDPB->FeatureSupport.ClusterIPSupport           = EDD_FEATURE_DISABLE; 
    #endif
    pDPB->FeatureSupport.MRPInterconnFwdRulesSupported  = EDD_FEATURE_DISABLE; 
    pDPB->FeatureSupport.MRPInterconnOriginatorSupported= EDD_FEATURE_DISABLE; 

    switch (pGlobalInitStruct->GenIni.HSYNCRole)
    {
        case EDDI_HSYNC_ROLE_APPL_SUPPORT:
        {
            pDPB->FeatureSupport.ApplicationExist                   = EDD_FEATURE_ENABLE;
            pDPB->FeatureSupport.AdditionalForwardingRulesSupported = EDD_FEATURE_ENABLE;
            break;
        }
        case EDDI_HSYNC_ROLE_FORWARDER:
        {
            pDPB->FeatureSupport.ApplicationExist                   = EDD_FEATURE_DISABLE;
            pDPB->FeatureSupport.AdditionalForwardingRulesSupported = EDD_FEATURE_ENABLE;
            break;
        }
        case EDDI_HSYNC_ROLE_NONE:
        {
            pDPB->FeatureSupport.ApplicationExist                   = EDD_FEATURE_DISABLE;
            pDPB->FeatureSupport.AdditionalForwardingRulesSupported = EDD_FEATURE_DISABLE;
            break;
        }
        default:
        {
            pDPB->FeatureSupport.ApplicationExist                   = EDD_FEATURE_DISABLE;
            pDPB->FeatureSupport.AdditionalForwardingRulesSupported = EDD_FEATURE_DISABLE;
            break;
        }
    }

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_InitDPBWithDefaults<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*
Array 1 = PhyTyp (BCM 52221 MII, BCM 52221 RMII, NEC (ERTEC200 int), NSC DP83849I MII, NSC DP83849I RMII, TI TLK111 MII, TI TLK111 RMII, Null)
Array 2 = MediaType (Copper, QFBR5978, AFBR59E4APZ, HFBR5803)
Array 3 = Values (PortTXDelay, PortRXDelay, MaxPortTXDelay, MaxPortRXDelay)
*/

static EDDI_PHY_DELAY_PARAMS_TYPE  const  DelayParametersERTEC400[EDDI_CONV_PHY_MAX][1+EDD_FX_TRANSCEIVER_MAX] =
{
    //                      |             Copper                      QFBR5978                   AFBR59E4APZ                HFBR5803                  QFBR5978-2
    /*BCM 5221 / MII*/      { { 1042,  254,  1052,  259}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221 / RMII*/     { { 1123,  444,  1144,  459}, { 1113,  390, 1132,  405}, { 1115,  391, 1135,  406}, { 1115,  391, 1135,  406}, { 1117,  395, 1138,  414}},
    /*BCM 5221_MC / MII*/   { {   0,     0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / RMII*/  { { 1163,  444,  1184,  459}, { 1153,  390, 1173,  405}, { 1166,  391, 1176,  406}, { 1166,  391, 1176,  406}, { 1157,  395, 1179,  414}},
    /*NEC (ERTEC200 int)*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NSC DP83849I / MII*/  { { 1040,  329,  1050,  334}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NSC DP83849I / RMII*/ { { 1140,  475,  1150,  480}, { 1143,  368, 1155,  383}, { 1145,  370, 1158,  386}, { 1145,  370, 1158,  386}, { 1147,  373, 1161,  392}},
    /*TI TLK111 / MII*/     { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / RMII*/    { { 1168,  341,  1180,  355}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*Null*/                { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}}
    /* Delay                |     TX|   RX| MaxTx| MaxRx| */
};

static EDDI_PHY_DELAY_PARAMS_TYPE  const  DelayParametersERTEC200[EDDI_CONV_PHY_MAX][1+EDD_FX_TRANSCEIVER_MAX] =
{
    //                      |             Copper                      QFBR5978                   AFBR59E4APZ                HFBR5803                  QFBR5978-2
    /*BCM 5221 / MII*/      { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221 / RMII*/     { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / MII*/   { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / RMII*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NEC (ERTEC200 int)*/  { {   31,  363,    36,  400}, {   13,  270,   18,  307}, {   13,  270,   18,  307}, {    0,    0,    0,    0}, {   17,  275,   24,  316}},
    /*NSC DP83849I / MII*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NSC DP83849I / RMII*/ { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / MII*/     { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / RMII*/    { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*Null*/                { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}}
    /* Delay                |     TX|   RX| MaxTx| MaxRx| */
};

static EDDI_PHY_DELAY_PARAMS_TYPE  const  DelayParametersSOC1[EDDI_CONV_PHY_MAX][1+EDD_FX_TRANSCEIVER_MAX] =
{
    //                      |             Copper                      QFBR5978                   AFBR59E4APZ                HFBR5803                  QFBR5978-2
    /*BCM 5221 / MII*/      { {   43,  238,    44,  239}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221 / RMII*/     { {   71,  428,    72,  439}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / MII*/   { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / RMII*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NEC (ERTEC200 int)*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NSC DP83849I / MII*/  { {   41,  313,    42,  314}, {   43,  211,   44,  212}, {   46,  213,   47,  215}, {    0,    0,    0,    0}, {   47,  216,   50,  221}},
    /*NSC DP83849I / RMII*/ { {  101,  459,   102,  460}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / MII*/     { {   41,  229,    42,  230}, {   47,  159,   48,  160}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / RMII*/    { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*Null*/                { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}}
    /* Delay                |     TX|   RX| MaxTx| MaxRx| */
};

static EDDI_PHY_DELAY_PARAMS_TYPE  const  DelayParametersSOC2[EDDI_CONV_PHY_MAX][1+EDD_FX_TRANSCEIVER_MAX] =
{
    //                      |             Copper                      QFBR5978                   AFBR59E4APZ                HFBR5803                  QFBR5978-2 |
    /*BCM 5221 / MII*/      { {   43,  238,    44,  239}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221 / RMII*/     { {   71,  428,    72,  439}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / MII*/   { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*BCM 5221_MC / RMII*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NEC (ERTEC200 int)*/  { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*NSC DP83849I / MII*/  { {   41,  313,    42,  314}, {   50,  218,   52,  220}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {   47,  216,   50,  221}},
    /*NSC DP83849I / RMII*/ { {  101,  459,   102,  460}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / MII*/     { {   41,  229,    42,  230}, {   47,  159,   48,  160}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*TI TLK111 / RMII*/    { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}},
    /*Null*/                { {    0,    0,     0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}, {    0,    0,    0,    0}}
    /* Delay                |     TX|   RX| MaxTx| MaxRx| */
};

/***************************************************************************/
/* F u n c t i o n:       Eddi_SetDelayPortParams()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  Eddi_SetDelayPortParams( EDDI_UPPER_DSB_PTR_TYPE    const  pDSB,
                                                                LSA_UINT32                 const  PhyTyp,
                                                                EDDI_PHY_TRANSCEIVER_TYPE  const  PhyTransceiver,
                                                                LSA_UINT32                 const  ErtecTyp )
{
    LSA_UINT32                            MediaTypeSelector;
	EDDI_PHY_DELAY_PARAMS_TYPE  *  const  pDelayParamsCopper = &pDSB->GlobPara.DelayParamsCopper[PhyTransceiver];
    EDDI_PHY_DELAY_PARAMS_TYPE  *  const  pDelayParamsFX_QFBR5978       = &pDSB->GlobPara.DelayParamsFX[EDD_FX_TRANSCEIVER_QFBR5978   ][PhyTransceiver];
    EDDI_PHY_DELAY_PARAMS_TYPE  *  const  pDelayParamsFX_AFBR59E4APZ    = &pDSB->GlobPara.DelayParamsFX[EDD_FX_TRANSCEIVER_AFBR59E4APZ][PhyTransceiver];
    EDDI_PHY_DELAY_PARAMS_TYPE  *  const  pDelayParamsFX_HFBR5803       = &pDSB->GlobPara.DelayParamsFX[EDD_FX_TRANSCEIVER_HFBR5803   ][PhyTransceiver];
    EDDI_PHY_DELAY_PARAMS_TYPE  *  const  pDelayParamsFX_QFBR5978_2     = &pDSB->GlobPara.DelayParamsFX[EDD_FX_TRANSCEIVER_QFBR5978_2 ][PhyTransceiver];

    typedef const EDDI_PHY_DELAY_PARAMS_TYPE AsicDelayParameter [1+EDD_FX_TRANSCEIVER_MAX];

    AsicDelayParameter * DelayParameters[4] =   {
                                                    DelayParametersERTEC400,
                                                    DelayParametersERTEC200,
                                                    DelayParametersSOC1,
                                                    DelayParametersSOC2
    };

    //delay parameters for Copper
    MediaTypeSelector = 0;

    pDelayParamsCopper->PortTxDelay            = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortTxDelay;
    pDelayParamsCopper->PortRxDelay            = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortRxDelay;
    pDelayParamsCopper->MaxPortTxDelay         = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortTxDelay;
    pDelayParamsCopper->MaxPortRxDelay         = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortRxDelay;

    //delay parameters for FX_QFBR5978
    MediaTypeSelector = 1 + EDD_FX_TRANSCEIVER_QFBR5978;
    pDelayParamsFX_QFBR5978->PortTxDelay       = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortTxDelay;
    pDelayParamsFX_QFBR5978->PortRxDelay       = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortRxDelay;
    pDelayParamsFX_QFBR5978->MaxPortTxDelay    = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortTxDelay;
    pDelayParamsFX_QFBR5978->MaxPortRxDelay    = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortRxDelay;

    //delay parameters for FX_AFBR59E4APZ
    MediaTypeSelector = 1 + EDD_FX_TRANSCEIVER_AFBR59E4APZ;
    pDelayParamsFX_AFBR59E4APZ->PortTxDelay    = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortTxDelay;
    pDelayParamsFX_AFBR59E4APZ->PortRxDelay    = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortRxDelay;
    pDelayParamsFX_AFBR59E4APZ->MaxPortTxDelay = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortTxDelay;
    pDelayParamsFX_AFBR59E4APZ->MaxPortRxDelay = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortRxDelay;

    //delay parameters for FX_HFBR5803
    MediaTypeSelector = 1 + EDD_FX_TRANSCEIVER_HFBR5803;
    pDelayParamsFX_HFBR5803->PortTxDelay       = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortTxDelay;
    pDelayParamsFX_HFBR5803->PortRxDelay       = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortRxDelay;
    pDelayParamsFX_HFBR5803->MaxPortTxDelay    = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortTxDelay;
    pDelayParamsFX_HFBR5803->MaxPortRxDelay    = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortRxDelay;

    //delay parameters for FX_QFBR5978_2
    MediaTypeSelector = 1 + EDD_FX_TRANSCEIVER_QFBR5978_2;
    pDelayParamsFX_QFBR5978_2->PortTxDelay = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortTxDelay;
    pDelayParamsFX_QFBR5978_2->PortRxDelay = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].PortRxDelay;
    pDelayParamsFX_QFBR5978_2->MaxPortTxDelay = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortTxDelay;
    pDelayParamsFX_QFBR5978_2->MaxPortRxDelay = DelayParameters[ErtecTyp][PhyTyp][MediaTypeSelector].MaxPortRxDelay;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_InitDSBWithDefaults()                       */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_InitDSBWithDefaults( EDDI_UPPER_DSB_PTR_TYPE         pDSB,
			                                               EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct )
{
    EDDI_UPPER_CONV_CALC_PTR_TYPE  pCC;
    LSA_UINT8                      Ctr;
    LSA_UINT8                      UsrPortIndex;
    LSA_RESULT                     Status = EDD_STS_OK;
    
    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_InitDSBWithDefaults->");
        
    if (EDDI_NULL_PTR == pGlobalInitStruct)
    {
        EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eddi_InitDSBWithDefaults, pGlobalInitStruct is not allocated"); 
        EDDI_Excp("eddi_InitDSBWithDefaults, pGlobalInitStruct is not allocated", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_PARAM;
    }
    
    if (EDDI_NULL_PTR == pGlobalInitStruct->pInternalUse)
    {
        //If the structure is a null pointer, then allocate and calc it 
        Status = EDDI_CalcValues((EDDI_UPPER_CONV_CALC_PTR_TYPE)pGlobalInitStruct->pInternalUse, pGlobalInitStruct);
        if (EDD_STS_OK != Status)
        {
            EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitDSBWithDefaults, Error in EDDI_CalcValues, Status:%d", Status);
            return EDD_STS_ERR_PARAM;
        } 
    }
    
    pCC = (EDDI_UPPER_CONV_CALC_PTR_TYPE)pGlobalInitStruct->pInternalUse;    
    
    //EDDI_GLOB_PARA_TYPE
    for (Ctr = 0; Ctr < EDD_MAC_ADDR_SIZE; Ctr++)
    {
        pDSB->GlobPara.xRT.MACAddressSrc.MacAdr[Ctr] = EDDI_CONV_INVALID_MAC_ADDR; //Param
    }

    pDSB->GlobPara.MRPSupportedRole         = EDDI_CONV_MRP_CAPABILITY_ROLE_CLIENT | EDDI_CONV_MRP_CAPABILITY_ROLE_MGR;
    pDSB->GlobPara.MRPDefaultRoleInstance0  = (LSA_UINT8)pGlobalInitStruct->GenIni.MRPDefaultRoleInstance0;
    pDSB->GlobPara.MaxMRP_Instances         = 1;
    pDSB->GlobPara.MRPSupportedMultipleRole = 0;
    pDSB->GlobPara.MRAEnableLegacyMode      = EDD_MRA_DISABLE_LEGACY_MODE;
    pDSB->GlobPara.MaxMRPInterconn_Instances= 0;
    pDSB->GlobPara.SupportedMRPInterconnRole= EDD_SUPPORTED_MRP_INTERCONN_ROLE_CAP_NONE;

    #if defined (EDDI_CFG_REV5)
    {
        pDSB->GlobPara.MaxBridgeDelay = EDDI_CONV_REV5_MAXBRIDGEDELAY;

        if (EDD_FEATURE_ENABLE == pGlobalInitStruct->NRTIni.bMIIUsed)
        {
            //MII delay parameters
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MII, EDDI_PHY_TRANSCEIVER_BROADCOM, EDDI_CONV_ERTEC400);
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MC_MII, EDDI_PHY_TRANSCEIVER_BROADCOM_MC, EDDI_CONV_ERTEC400);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_NEC, 0);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NSC_DP83849I_MII, EDDI_PHY_TRANSCEIVER_NSC, EDDI_CONV_ERTEC400);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_TI_TLK111_MII, EDDI_PHY_TRANSCEIVER_TI, EDDI_CONV_ERTEC400);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_USERSPEC, 0);
            #endif
        }
        else
        {
            //RMII delay parameters
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_RMII, EDDI_PHY_TRANSCEIVER_BROADCOM, EDDI_CONV_ERTEC400);
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MC_RMII, EDDI_PHY_TRANSCEIVER_BROADCOM_MC, EDDI_CONV_ERTEC400);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_NEC, 0);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NSC_DP83849I_RMII, EDDI_PHY_TRANSCEIVER_NSC, EDDI_CONV_ERTEC400);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_TI_TLK111_RMII, EDDI_PHY_TRANSCEIVER_TI, EDDI_CONV_ERTEC400);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_USERSPEC, 0);
            #endif
        }
    }
    #elif defined (EDDI_CFG_REV6)
    {
        pDSB->GlobPara.MaxBridgeDelay = EDDI_CONV_REV6_MAXBRIDGEDELAY;

        if (EDD_FEATURE_ENABLE == pGlobalInitStruct->NRTIni.bMIIUsed)
        {
            //MII delay parameters
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MII, EDDI_PHY_TRANSCEIVER_BROADCOM, EDDI_CONV_ERTEC200);
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MC_MII, EDDI_PHY_TRANSCEIVER_BROADCOM_MC, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NEC_ERTEC200_INT, EDDI_PHY_TRANSCEIVER_NEC, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NSC_DP83849I_MII, EDDI_PHY_TRANSCEIVER_NSC, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_TI_TLK111_MII, EDDI_PHY_TRANSCEIVER_TI, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_USERSPEC, 0);
            #endif
        }
        else
        {
            //RMII delay parameters
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_RMII, EDDI_PHY_TRANSCEIVER_BROADCOM, EDDI_CONV_ERTEC200);
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MC_RMII, EDDI_PHY_TRANSCEIVER_BROADCOM_MC, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NEC_ERTEC200_INT, EDDI_PHY_TRANSCEIVER_NEC, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NSC_DP83849I_RMII, EDDI_PHY_TRANSCEIVER_NSC, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_TI_TLK111_RMII, EDDI_PHY_TRANSCEIVER_TI, EDDI_CONV_ERTEC200);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_USERSPEC, 0);
            #endif
        }
    }
    #elif defined (EDDI_CFG_REV7)
    {
        LSA_UINT32  const  ErtecTyp = (EDDI_HW_SUBTYPE_USED_SOC1 == pGlobalInitStruct->GenIni.HWSubTypeUsed)?EDDI_CONV_ERTECSOC1:EDDI_CONV_ERTECSOC2;

        pDSB->GlobPara.MaxBridgeDelay = EDDI_CONV_REV7_MAXBRIDGEDELAY;

        if (EDD_FEATURE_ENABLE == pGlobalInitStruct->NRTIni.bMIIUsed)
        {
            //MII delay parameters
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MII, EDDI_PHY_TRANSCEIVER_BROADCOM, ErtecTyp);
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MC_MII, EDDI_PHY_TRANSCEIVER_BROADCOM_MC, ErtecTyp);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_NEC, 0);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NSC_DP83849I_MII, EDDI_PHY_TRANSCEIVER_NSC, ErtecTyp);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_TI_TLK111_MII, EDDI_PHY_TRANSCEIVER_TI, ErtecTyp);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_USERSPEC, 0);
            #endif
        }
        else
        {
            //RMII delay parameters
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_RMII, EDDI_PHY_TRANSCEIVER_BROADCOM, ErtecTyp);
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_BCM_5221_MC_RMII, EDDI_PHY_TRANSCEIVER_BROADCOM_MC, ErtecTyp);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_NEC, 0);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NSC_DP83849I_RMII, EDDI_PHY_TRANSCEIVER_NSC, ErtecTyp);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_TI_TLK111_RMII, EDDI_PHY_TRANSCEIVER_TI, ErtecTyp);
            #endif
            #if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
            Eddi_SetDelayPortParams(pDSB, EDDI_CONV_PHY_NULL, EDDI_PHY_TRANSCEIVER_USERSPEC, 0);
            #endif
        }
    }
    #endif //EDDI_CFG_REVx

    for (UsrPortIndex = 0; UsrPortIndex < pGlobalInitStruct->GenIni.NrOfPorts; UsrPortIndex++)
    {
        //EDDI_GLOB_PARA_TYPE
        pDSB->GlobPara.PortParams[UsrPortIndex].PhyAdr         = EDDI_ILLEGAL_ADDR_VALUE_16;                //Param
        pDSB->GlobPara.PortParams[UsrPortIndex].PhyTransceiver = EDDI_PHY_TRANSCEIVER_NOT_DEFINED;

        for (Ctr = 0; Ctr < EDD_MAC_ADDR_SIZE; Ctr++)
        {
            pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[Ctr] = EDDI_CONV_INVALID_MAC_ADDR;    //Param
        }

        if (   (UsrPortIndex == pGlobalInitStruct->GenIni.MRPDefaultRingPort1 - 1)
            || (UsrPortIndex == pGlobalInitStruct->GenIni.MRPDefaultRingPort2 - 1))
        {
            pDSB->GlobPara.PortParams[UsrPortIndex].MRPRingPort  = EDD_MRP_RING_PORT_DEFAULT;
        }
        else
        {
            pDSB->GlobPara.PortParams[UsrPortIndex].MRPRingPort  = EDD_MRP_NO_RING_PORT;
        }
        pDSB->GlobPara.PortParams[UsrPortIndex].SupportsMRPInterconnPortConfig  = EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_NO;
        pDSB->GlobPara.PortParams[UsrPortIndex].MediaType                       = EDD_MEDIATYPE_UNKNOWN;
        pDSB->GlobPara.PortParams[UsrPortIndex].IsPOF                           = EDD_PORT_OPTICALTYPE_ISNONPOF;
        pDSB->GlobPara.PortParams[UsrPortIndex].FXTransceiverType               = 0;
        pDSB->GlobPara.PortParams[UsrPortIndex].IsWireless                      = EDD_PORT_IS_NOT_WIRELESS;        //Param
        pDSB->GlobPara.PortParams[UsrPortIndex].IsMDIX                          = EDD_PORT_MDIX_ENABLED;
        pDSB->GlobPara.PortParams[UsrPortIndex].bBC5221_MCModeSet               = EDDI_PORT_BC5221_MEDIACONV_STRAPPED_HIGH;
    }

    //EDDI_SWI_PARA_TYPE
    pDSB->SWIPara.Sys_StatFDB_CntEntry  = EDDI_CONV_SYS_STAT_FDB_CNT_ENTRY;
    pDSB->SWIPara.pSys_StatFDB_CntEntry = EDDI_NULL_PTR;

    #if defined (EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    pDSB->SWIPara.bResetIRTCtrl = EDD_FEATURE_DISABLE;
    #endif    

    pDSB->SWIPara.Multicast_Bridge_IEEE802RSTP_Forward = EDD_FEATURE_ENABLE;

    //EDDI_SRT_PARA_TYPE
    {
        pDSB->SRTPara.SRT_FCW_Count = (LSA_UINT16)(pGlobalInitStruct->GenIni.NrOfPorts * EDDI_MIN(pCC->ProviderCnt, EDDI_CRT_PROVIDER_DEFAULT_PER_1MS ));
    }

    //EDDI_NRT_PARA_TYPE
    {
        pDSB->NRTPara.NRT_FCW_Count      = (LSA_UINT16)pCC->Local.NRTFCWCount;
        pDSB->NRTPara.NRT_DB_Count       = (LSA_UINT16)pCC->Local.NRTDBCount;
        pDSB->NRTPara.NRT_DB_Limit_Down  = (LSA_UINT16)(pCC->Local.DBCountBug);
        pDSB->NRTPara.NRT_FCW_Limit_Down = (LSA_UINT16)(pCC->Local.FCWCountBug);
        pDSB->NRTPara.HOL_Limit_CH_Up    = (LSA_UINT16)(pCC->Local.NrOfFrames + SWI_MAX_DB_NR_FULL_NRT_FRAME - 1);
        pDSB->NRTPara.HOL_Limit_Port_Up  = (LSA_UINT16)(pCC->Local.HOL_Limit_Port_Up & 0x0000FFFFUL);  //BV??? warum nicht >= 0x0FFF und dann = 0x0FFE ???
    }

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_InitDSBWithDefaults<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_InitCOMPWithDefaults()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_InitCOMPWithDefaults( EDDI_UPPER_CMP_INI_PTR_TYPE     pParam,
			                                                EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct )
{
    EDDI_UPPER_CONV_CALC_PTR_TYPE  pCC;
    LSA_RESULT                     Status = EDD_STS_OK;

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_InitCOMPWithDefaults->");

    if (EDDI_NULL_PTR == pGlobalInitStruct)
    {
        EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eddi_InitCOMPWithDefaults, pGlobalInitStruct is not allocated"); 
        EDDI_Excp("eddi_InitCOMPWithDefaults, pGlobalInitStruct is not allocated", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_PARAM;
    }

    if (EDDI_NULL_PTR == pGlobalInitStruct->pInternalUse)
    {
        //If the structure is a null pointer, then allocate and calc it 
        Status = EDDI_CalcValues((EDDI_UPPER_CONV_CALC_PTR_TYPE)pGlobalInitStruct->pInternalUse, pGlobalInitStruct);
        if (EDD_STS_OK != Status)
        {
            EDDI_CRT_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eddi_InitCOMPWithDefaults, Error in EDDI_CalcValues, Status:%d", Status);
            return EDD_STS_ERR_PARAM;
        } 
    }

    pCC = (EDDI_UPPER_CONV_CALC_PTR_TYPE)pGlobalInitStruct->pInternalUse;    
    
    pParam->NRT.bFeedInLoadLimitationActive                         = EDD_SYS_FILL_ACTIVE;
    pParam->NRT.bIO_Configured                                      = EDD_SYS_IO_CONFIGURED_ON;

    //Channel A and Interface 0
    pParam->NRT.u.IF.A__0.MemModeBuffer                             = MEMORY_SDRAM_ERTEC;                                                   //Param    
    pParam->NRT.u.IF.A__0.MemModeDMACW                              = MEMORY_SDRAM_ERTEC;                                                   //Param    
    pParam->NRT.u.IF.A__0.TxCntDscr                                 = EDDI_CONV_IF_A_0_TX_CNT_DSCR;        
    pParam->NRT.u.IF.A__0.RxCntDscr                                 = EDDI_CONV_IF_A_0_RX_CNT_DSCR;    
    pParam->NRT.u.IF.A__0.MaxRcvFrame_SendToUser                    = EDDI_CONV_MAX_RCV_FRAME_SEND_TO_USER;                                 //Param   
    pParam->NRT.u.IF.A__0.MaxRcvFrame_ToLookUp                      = EDDI_CONV_MAX_RCV_FRAME_TO_LOOK_UP;                                   //Param 
    pParam->NRT.u.IF.A__0.MaxRcvFragments_ToCopy                    = EDDI_CONV_MAX_RCV_FRAGMENT_FRAME_TO_COPY;

    pParam->NRT.u.IF.A__0.UserMemIDDMACWDefault                     = (EDDI_USERMEMID_TYPE)0xFFFFFFFF; 
    pParam->NRT.u.IF.A__0.UserMemIDTXDefault                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF; 
    pParam->NRT.u.IF.A__0.UserMemIDRXDefault                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;  
    pParam->NRT.u.IF.A__0.UserMemIDTXMgmtLowFrag                    = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;
    pParam->NRT.u.IF.A__0.UserMemIDTXMgmtHighFrag                   = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;
    pParam->NRT.u.IF.A__0.UserMemIDTXLowFrag                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;  
    pParam->NRT.u.IF.A__0.UserMemIDTXMidFrag                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;
    pParam->NRT.u.IF.A__0.UserMemIDTXHighFrag                       = (EDDI_USERMEMID_TYPE)0xFFFFFFFF; 
   
    pParam->NRT.u.IF.A__0.RxFilterUDP_Broadcast                     = EDD_SYS_UDP_WHITELIST_FILTER_OFF; 
    pParam->NRT.u.IF.A__0.RxFilterUDP_Unicast                       = EDD_SYS_UDP_WHITELIST_FILTER_OFF; 
    pParam->NRT.u.IF.A__0.RxFilterIFMACAddr                         = EDD_FEATURE_ENABLE;

    //Channel B and Interface 0
    pParam->NRT.u.IF.B__0.MemModeBuffer                             = MEMORY_SDRAM_ERTEC;                                                   //Param   
    pParam->NRT.u.IF.B__0.MemModeDMACW                              = MEMORY_SDRAM_ERTEC;                                                   //Param   
    pParam->NRT.u.IF.B__0.TxCntDscr                                 = EDDI_CONV_IF_B_0_TX_CNT_DSCR;        
    pParam->NRT.u.IF.B__0.RxCntDscr                                 = EDDI_CONV_IF_B_0_RX_CNT_DSCR;    
    pParam->NRT.u.IF.B__0.MaxRcvFrame_SendToUser                    = EDDI_CONV_MAX_RCV_FRAME_SEND_TO_USER;                                 //Param  
    pParam->NRT.u.IF.B__0.MaxRcvFrame_ToLookUp                      = EDDI_CONV_MAX_RCV_FRAME_TO_LOOK_UP;                                   //Param 
    pParam->NRT.u.IF.B__0.MaxRcvFragments_ToCopy                    = EDDI_CONV_MAX_RCV_FRAGMENT_FRAME_TO_COPY;
    
    pParam->NRT.u.IF.B__0.UserMemIDDMACWDefault                     = (EDDI_USERMEMID_TYPE)0xFFFFFFFF; 
    pParam->NRT.u.IF.B__0.UserMemIDTXDefault                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF; 
    pParam->NRT.u.IF.B__0.UserMemIDRXDefault                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;  
    pParam->NRT.u.IF.B__0.UserMemIDTXMgmtLowFrag                    = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;
    pParam->NRT.u.IF.B__0.UserMemIDTXMgmtHighFrag                   = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;
    pParam->NRT.u.IF.B__0.UserMemIDTXLowFrag                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;  
    pParam->NRT.u.IF.B__0.UserMemIDTXMidFrag                        = (EDDI_USERMEMID_TYPE)0xFFFFFFFF;
    pParam->NRT.u.IF.B__0.UserMemIDTXHighFrag                       = (EDDI_USERMEMID_TYPE)0xFFFFFFFF; 

    pParam->NRT.u.IF.B__0.RxFilterUDP_Broadcast                     = EDD_SYS_UDP_WHITELIST_FILTER_OFF; 
    pParam->NRT.u.IF.B__0.RxFilterUDP_Unicast                       = EDD_SYS_UDP_WHITELIST_FILTER_OFF; 
    pParam->NRT.u.IF.B__0.RxFilterIFMACAddr                         = EDD_FEATURE_ENABLE;

    pParam->CRT.bUseTransferEnd = EDD_FEATURE_ENABLE;

    pParam->CRT.SRT.ConsumerFrameIDBaseClass1 = EDDI_CONV_CONSUMER_FRAME_ID_BASE_CLASS1;
    pParam->CRT.SRT.ConsumerFrameIDBaseClass2 = EDDI_CONV_CONSUMER_FRAME_ID_BASE_CLASS2;
    pParam->CRT.SRT.ConsumerCntClass12        = (LSA_UINT16)(pCC->IOC.NrOfRtUcConsumer + pCC->IOC.NrOfRtMcConsumer + pCC->IOD.NrOfRtUcConsumer + pCC->IOD.NrOfRtMcConsumer);
    pParam->CRT.SRT.ProviderCnt               = (LSA_UINT16)(pCC->IOC.NrOfRtUcProvider + pCC->IOC.NrOfRtMcProvider + pCC->IOD.NrOfRtUcProvider + pCC->IOD.NrOfRtMcProvider);

    if (EDD_FEATURE_ENABLE == pGlobalInitStruct->GenIni.bIRTSupported)
    {
        pParam->CRT.IRT.ConsumerCntClass3 = (LSA_UINT16)(pCC->IOC.NrOfIrtUcConsumer + pCC->IOC.NrOfIrtMcConsumer + pCC->IOD.NrOfIrtUcConsumer + pCC->IOD.NrOfIrtMcConsumer);
        pParam->CRT.IRT.ProviderCnt       = (LSA_UINT16)(pCC->IOC.NrOfIrtUcProvider + pCC->IOC.NrOfIrtMcProvider + pCC->IOD.NrOfIrtUcProvider + pCC->IOD.NrOfIrtMcProvider);
        pParam->CRT.IRT.ForwarderCnt      = pGlobalInitStruct->GenIni.NrOfIRTForwarders;

        if (!pParam->CRT.IRT.ConsumerCntClass3)
        {
            pParam->CRT.IRT.ConsumerCntClass3 = 1;
        }
        if (!pParam->CRT.IRT.ProviderCnt)
        {
            pParam->CRT.IRT.ProviderCnt = 1;
        }
        if (!pParam->CRT.IRT.ForwarderCnt)
        {
            pParam->CRT.IRT.ForwarderCnt = 1;
        }
    }
    else
    {
        pParam->CRT.IRT.ConsumerCntClass3 = 1;
        pParam->CRT.IRT.ProviderCnt       = 1;
        pParam->CRT.IRT.ForwarderCnt      = 1;        
    }

    EDDI_CRT_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eddi_InitCOMPWithDefaults<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_ENABLE_MC_FDB)
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_BROADCAST;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_PROFINET;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_RT_3_DA_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_RT_3_INVALID_DA_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_RT_2_QUER_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_PTP_ANNOUNCE_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_PTP_SYNC_WITH_FU_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_PTP_FOLLOW_UP_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_PTP_SYNC_BEGIN;  
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_MRP_1_BEGIN;       
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_MRP_2_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_MRP_IC_1_BEGIN;       
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_MRP_IC_2_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_IEEE_RESERVED_BEGIN_1;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_IEEE_RESERVED_BEGIN_1_1;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_LLDP_PTP_DELAY_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_IEEE_RESERVED_BEGIN_2;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_HSR_1_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_HSR_2_BEGIN;           
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_STBY_2_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_DCP_BEGIN;
LSA_EXTERN const EDD_MAC_ADR_TYPE    MAC_CARP_MC_BEGIN;

static LSA_BOOL  EDDI_LOCAL_FCT_ATTR  eddi_CalcIndexMCRange( const EDD_MAC_ADR_TYPE * const pMAC,
			                                                       LSA_UINT16         const Range,
                                                                   LSA_UINT16       * const pMACAdrPerTable,
                                                                   LSA_UINT16         const UCMCLFSRMask )
{
    LSA_UINT16        MacCtr;
    LSA_UINT32        MCTabAll_Index;
    EDD_MAC_ADR_TYPE  MACAddress = *pMAC;

    //Step 2: Enter static addresses
    for (MacCtr = 0; MacCtr < Range; MacCtr++)
    {
        //calc index, value range 0..MC_Table_Max_Index_All
        MCTabAll_Index = EDDI_SwiUcCalcAdr(&MACAddress, UCMCLFSRMask, (EDDI_CONV_MC_MAX_SUBTABLES-1));
        if (MCTabAll_Index >= EDDI_CONV_MC_MAX_SUBTABLES)
        {
            EDDI_CRT_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "eddi_CalcIndexMCRange, MCTabAll_Index>=EDDI_CONV_MC_MAX_SUBTABLES MCTabAll_Index:0x%X, UCMCLFSRMask:0x%X, Range:0x%X, MacCtr:0x%X", 
                MCTabAll_Index, UCMCLFSRMask, Range, MacCtr); 
            EDDI_Excp("eddi_CalcIndexMCRange, MCTabAll_Index>=EDDI_CONV_MC_MAX_SUBTABLES", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, UCMCLFSRMask);
            return LSA_FALSE;
        }

        *(pMACAdrPerTable+MCTabAll_Index) += 1;
        if (*(pMACAdrPerTable+MCTabAll_Index) > EDDI_CONV_MC_MAX_ENTRIES_PER_SUBTABLE)
        {
            return LSA_FALSE;
        }

        MACAddress.MacAdr[EDD_MAC_ADDR_SIZE-1]++;
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_CalcUCMCLFSRMask()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_CalcUCMCLFSRMask( const EDDI_SWI_PARA_TYPE * const pStatMCEntries,
			                                            LSA_UINT16               * const pUCMCLFSRMask)
{
    LSA_UINT16        MACAdrPerTable[EDDI_CONV_MC_MAX_SUBTABLES];
    LSA_UINT16        UCMCLFSRMask = *pUCMCLFSRMask;
    LSA_UINT16 const  UCMCLFSRMaskStart = UCMCLFSRMask;
    LSA_UINT16        MacCtr;
    LSA_UINT32        MCTabAll_Index;
    LSA_BOOL          bFinish;
    LSA_UINT16        MACAdrPerTableMAX, MACAdrPerTableMIN;
    LSA_UINT16        MACAdrPerTableDELTA=0xFFFF;
    LSA_UINT16        UCMCLFSRMaskBest=0xFFFF;

    do 
    {
        //Step 1: Clear indexctrs
        bFinish = LSA_TRUE;
        MACAdrPerTableMIN=0xFFFF;
        MACAdrPerTableMAX=0;
        for (MCTabAll_Index = 0; MCTabAll_Index < EDDI_CONV_MC_MAX_SUBTABLES; MCTabAll_Index++)
        {
            MACAdrPerTable[MCTabAll_Index] = 0;
        }

        //Step 2: Enter static addresses
        for (;;)
        {
            if (!eddi_CalcIndexMCRange(&MAC_BROADCAST, MAC_BROADCAST_LEN, &MACAdrPerTable[0], UCMCLFSRMask)) 
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_PROFINET, MAC_PROFINET_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_RT_3_DA_BEGIN, MAC_RT_3_DA_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_RT_3_INVALID_DA_BEGIN, MAC_RT_3_INVALID_DA_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_RT_2_QUER_BEGIN, MAC_RT_3_QUER_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_PTP_ANNOUNCE_BEGIN, MAC_PTP_ANNOUNCE_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_PTP_SYNC_WITH_FU_BEGIN, MAC_PTP_SYNC_WITH_FU_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_PTP_FOLLOW_UP_BEGIN, MAC_PTP_FOLLOW_UP_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_PTP_SYNC_BEGIN, MAC_PTP_SYNC_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_MRP_1_BEGIN, MAC_MRP_1_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_MRP_2_BEGIN, MAC_MRP_2_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_MRP_IC_1_BEGIN, MAC_MRP_IC_1_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_MRP_IC_2_BEGIN, MAC_MRP_IC_2_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_IEEE_RESERVED_BEGIN_1, MAC_IEEE_RESERVED_LEN_1, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_IEEE_RESERVED_BEGIN_1_1, MAC_IEEE_RESERVED_LEN_1_1, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_LLDP_PTP_DELAY_BEGIN, MAC_LLDP_PTP_DELAY_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_IEEE_RESERVED_BEGIN_2, MAC_IEEE_RESERVED_LEN_2, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_HSR_1_BEGIN, MAC_HSR_1_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_HSR_2_BEGIN, MAC_HSR_2_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_STBY_2_BEGIN, MAC_STBY_2_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_DCP_BEGIN, MAC_DCP_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }
            if (!eddi_CalcIndexMCRange(&MAC_CARP_MC_BEGIN, MAC_CARP_MC_LEN, &MACAdrPerTable[0], UCMCLFSRMask))
            { bFinish = LSA_FALSE; break; }

            //Step 3: Enter user addresses
            for (MacCtr = 0; MacCtr < pStatMCEntries->Sys_StatFDB_CntEntry; MacCtr++)
            {
                MCTabAll_Index = EDDI_SwiUcCalcAdr(&pStatMCEntries->pSys_StatFDB_CntEntry[MacCtr].MACAddress, UCMCLFSRMask, (EDDI_CONV_MC_MAX_SUBTABLES-1));
                if (MCTabAll_Index >= EDDI_CONV_MC_MAX_SUBTABLES)
                {
                    EDDI_CRT_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "eddi_CalcUCMCLFSRMask, MCTabAll_Index>=EDDI_CONV_MC_MAX_SUBTABLES MCTabAll_Index:0x%X, UCMCLFSRMask:0x%X, MacCtr:0x%X", 
                        MCTabAll_Index, UCMCLFSRMask, MacCtr); 
                    EDDI_Excp("eddi_CalcUCMCLFSRMask, MCTabAll_Index>=EDDI_CONV_MC_MAX_SUBTABLES", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, UCMCLFSRMask);
                    return EDD_STS_ERR_EXCP;
                }
                MACAdrPerTable[MCTabAll_Index]++;
                if (MACAdrPerTable[MCTabAll_Index] > EDDI_CONV_MC_MAX_ENTRIES_PER_SUBTABLE)
                { bFinish = LSA_FALSE; break; }
            }
            break;
        }

        if (bFinish)
        {
            //Step 4: Check table
            for (MCTabAll_Index = 0; MCTabAll_Index < EDDI_CONV_MC_MAX_SUBTABLES; MCTabAll_Index++)
            {
                if (MACAdrPerTable[MCTabAll_Index] > MACAdrPerTableMAX) {MACAdrPerTableMAX = MACAdrPerTable[MCTabAll_Index];}
                if (MACAdrPerTable[MCTabAll_Index] < MACAdrPerTableMIN) {MACAdrPerTableMIN = MACAdrPerTable[MCTabAll_Index];}
            }
            //check for best values
            if (MACAdrPerTableDELTA > (MACAdrPerTableMAX - MACAdrPerTableMIN))
            {
                //found new best value
                MACAdrPerTableDELTA = MACAdrPerTableMAX - MACAdrPerTableMIN;
                UCMCLFSRMaskBest = UCMCLFSRMask;
            }
        }

        UCMCLFSRMask++;
        if (UCMCLFSRMask > 0xFFF) 
        {
            UCMCLFSRMask = 0;
        }
    }
    while (UCMCLFSRMask != UCMCLFSRMaskStart);

    *pUCMCLFSRMask = UCMCLFSRMaskBest;

    if //best value found?
       (0xFFFF != UCMCLFSRMaskBest)
    {
        return EDD_STS_OK;
    }
    else
    {
        return EDD_STS_ERR_RESOURCE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#else
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_CalcUCMCLFSRMask( const EDDI_SWI_PARA_TYPE * const pStatMCEntries,
			                                                  LSA_UINT16         * const pUCMCLFSRMask )
{
    LSA_UNUSED_ARG(pStatMCEntries);

    *pUCMCLFSRMask = EDDI_CONV_UC_MC_LFSR_MASK_R5R6;
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //(EDDI_CFG_ENABLE_MC_FDB)


/*****************************************************************************/
/*  end of file eddi_conv_calc.c                                             */
/*****************************************************************************/


