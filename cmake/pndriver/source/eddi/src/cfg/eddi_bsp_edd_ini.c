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
/*  F i l e               &F: eddi_bsp_edd_ini.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  17.07.07    JS    changed timestamp interface of ERTEC400 from           */
/*                    EDDI_NRT_CHA_IF_0 to EDDI_NRT_CHB_IF_0                 */
/*  02.08.07    JS    Channel mapping of PortB changed to new API            */
/*  09.01.08    JS    added MRPRedundantPort to DSB init                     */
/*  11.02.08    JS    added IsWireless       to DSB init                     */
/*  10.03.08    JS    Default for Multicast_Bridge_IEEE802RSTP_Forward       */
/*                    changed from FALSE to TRUE                             */
/*  17.04.08    AB    added IsMDIX to DSB init                               */
/*                                                                           */
/*****************************************************************************/ 

#include "eddi_inc.h"
#include "eddi_bsp_edd_ini.h"
#include "eddi_irte.h"

#if !defined (TEST_WITH_EDD_IN_DLL)
#include "eddi_bsp_sys_int.h"
#else
#include <stdio.h>
#endif

typedef void   (*BSP_SYS_CBF )(EDD_RQB_TYPE    * pRQB);

typedef struct BSP_BASE_ADDR_S
{
    LSA_UINT8                      * irte;
    LSA_UINT8                      * pBOARD_SDRAM_BaseAddr;
    #if defined (EDDI_CFG_SOC)
    LSA_UINT8                      * pPC_HostVirt_BaseAdrIOCC;
    #endif
    LSA_UINT8                      * pERTECx00_SCRB;

} BSP_BASE_ADDR_T;

static  void  EDDI_BspEddiSystemDevOpen(       EDD_UPPER_RQB_PTR_TYPE          pRQB,
                                         const BSP_BASE_ADDR_T         * const pBaseAddr,
                                               IRTE_SW_MAKRO_T             * * ppIRTE_SW_MAKRO_T,
                                               EDDI_UPPER_DPB_PTR_TYPE         pDPB );

static  void  EDDI_BspEddiSystemBasicIni( EDD_UPPER_RQB_PTR_TYPE      const pRQB,
                                          EDDI_MEMORY_MODE_TYPE       const MemMode,
                                          EDDI_UPPER_CMP_INI_PTR_TYPE       pComp );

static  void  EDDI_BspEddiSystemDevSetup(       EDD_UPPER_RQB_PTR_TYPE    const pRQB,
                                          const BSP_EDDI_INI_TYPE       * const pBSP_eddi_ini,
                                                EDDI_UPPER_DSB_PTR_TYPE         pDSB );

static LSA_BOOL EDDI_BSPGetAddrFromBESY( BSP_BASE_ADDR_T * const pBaseAddr );

static LSA_BOOL EDDI_BspEddiDevClose( EDD_UPPER_RQB_PTR_TYPE const pRQB );

static void SetGISValues( EDDI_UPPER_GLOBAL_INI_PTR_TYPE const pGIS,
                          LSA_UINT32                     const ProductID );

static LSA_UINT32   MD_PortCnt = 0;
EDDI_HANDLE         g_SyshDDB  = 0;


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
static  LSA_BOOL  EDDI_BSPGetAddrFromBESY( BSP_BASE_ADDR_T  *  const  pBaseAddr )
{
    // meaningful addresses must stand here
    // -> the addresses registered here are the "normal" addresses of a firmware environment
    pBaseAddr->irte                          =  (LSA_UINT8 *)0x10000000;
    pBaseAddr->pBOARD_SDRAM_BaseAddr         =  (LSA_UINT8 *)0x20000000;

    #if defined (EDDI_CFG_ERTEC_200)
    pBaseAddr->pERTECx00_SCRB                =  (LSA_UINT8 *)0x40002600;
    #elif defined (EDDI_CFG_ERTEC_400)
    pBaseAddr->pERTECx00_SCRB                =  (LSA_UINT8 *)0x40002600;
    #elif defined (EDDI_CFG_SOC)
    pBaseAddr->pPC_HostVirt_BaseAdrIOCC      =  (LSA_UINT8 *)0;
    pBaseAddr->pERTECx00_SCRB                =  (LSA_UINT8 *)0;
    #else
    pBaseAddr->pPC_HostVirt_BaseAdrIOCC      =  (LSA_UINT8 *)0;
    pBaseAddr->pERTECx00_SCRB                =  (LSA_UINT8 *)0;
    #endif

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  void  SetGISValues( EDDI_UPPER_GLOBAL_INI_PTR_TYPE  const  pGIS,
                            LSA_UINT32                      const  ProductID )
{
    LSA_UINT32  SubIndex = 0;
    LSA_UINT32  ErtecTyp = 0;

    /*
    Array 1: ERTEC Typ
    Array 2: ProductTyp
    Array 3: Values
    */

    static LSA_UINT32 const ProductGISValues[4][6][40] =
    {
        {/*Unknown*/
            /*Default*/                    {                         0/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 4/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/,   0/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_NONE/*I2C_Type*/, 256 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  8192/*NrOfSubmod*/,    0/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/, EDD_FEATURE_DISABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/}
        },
        {/*E400*/
            /*Default*/                    {EDD_HW_TYPE_USED_ERTEC_400/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 4/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/,  64/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_GPIO/*I2C_Type*/,  96 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  4096/*NrOfSubmod*/,  720/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/, EDD_FEATURE_DISABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*CPU 317-2PN with ERTEC400*/  {EDD_HW_TYPE_USED_ERTEC_400/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_GPIO/*I2C_Type*/, 128 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  8192/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/, EDD_FEATURE_DISABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*CPU 319-2PN with ERTEC400*/  {EDD_HW_TYPE_USED_ERTEC_400/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_GPIO/*I2C_Type*/, 128 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  8192/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/, EDD_FEATURE_DISABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/}
        },
        {/*E200*/
            /*Default*/                    {EDD_HW_TYPE_USED_ERTEC_200/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/,  64/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_CLIENT /*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_NONE/*I2C_Type*/, 0   /*NrOfRTDevices*/, 0 /*NrOfIRTDevices*/, 0 /*NrOfRedIRTDevices*/, 0/*NrOfQVProviders*/,  0/*NrOfQVConsumers*/,  0/*NrOfRedQVConsumers*/,    0 /*LogicalAddressSpace*/,     0/*NrOfSubmod*/,    0/*MaxIOCRDataSize*/,   0/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 248/*MaxOCRDataSize*/, 248/*MaxICRDataSize*/,  64/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*ET 200M IM153-4*/            {EDD_HW_TYPE_USED_ERTEC_200/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_CLIENT /*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_NONE/*I2C_Type*/, 0   /*NrOfRTDevices*/, 0 /*NrOfIRTDevices*/, 0 /*NrOfRedIRTDevices*/, 0/*NrOfQVProviders*/,  0/*NrOfQVConsumers*/,  0/*NrOfRedQVConsumers*/,    0 /*LogicalAddressSpace*/,     0/*NrOfSubmod*/,    0/*MaxIOCRDataSize*/,   0/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 220/*MaxOCRDataSize*/, 220/*MaxICRDataSize*/,  64/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*PN/PN Coupler 158-3AD00*/    {EDD_HW_TYPE_USED_ERTEC_200/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_NONE/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_CLIENT /*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/,      EDDI_HSYNC_ROLE_NONE/*HSYNCRole*/, EDDI_GIS_USECASE_DEFAULT/*BufferCapacityUseCase*/,        EDDI_I2C_TYPE_NONE/*I2C_Type*/, 0   /*NrOfRTDevices*/, 0 /*NrOfIRTDevices*/, 0 /*NrOfRedIRTDevices*/, 0/*NrOfQVProviders*/,  0/*NrOfQVConsumers*/,  0/*NrOfRedQVConsumers*/,    0 /*LogicalAddressSpace*/,     0/*NrOfSubmod*/,    0/*MaxIOCRDataSize*/,   0/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 436/*MaxOCRDataSize*/, 436/*MaxICRDataSize*/,  64/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/}
        },
        {/*SOC1*/
            /*Default*/                    {      EDD_HW_TYPE_USED_SOC/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_SOC1/*HWSubTypeUsed*/, 3/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/, EDDI_HSYNC_ROLE_FORWARDER/*HSYNCRole*/, EDDI_GIS_USECASE_IOC_SOC1_3P/*BufferCapacityUseCase*/, EDDI_I2C_TYPE_SOC1_HW/*I2C_Type*/, 256 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  8192/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 2/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*CPU 317-2PN with SOC1*/      {      EDD_HW_TYPE_USED_SOC/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_SOC1/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/, EDDI_HSYNC_ROLE_FORWARDER/*HSYNCRole*/, EDDI_GIS_USECASE_IOC_SOC1_2P/*BufferCapacityUseCase*/, EDDI_I2C_TYPE_SOC1_HW/*I2C_Type*/, 128 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  8192/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 4/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*CPU 319-2PN with SOC1*/      {      EDD_HW_TYPE_USED_SOC/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_SOC1/*HWSubTypeUsed*/, 2/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/, EDDI_HSYNC_ROLE_FORWARDER/*HSYNCRole*/, EDDI_GIS_USECASE_IOC_SOC1_2P/*BufferCapacityUseCase*/, EDDI_I2C_TYPE_SOC1_HW/*I2C_Type*/, 256 /*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 8192 /*LogicalAddressSpace*/,  8192/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 4/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*CPU 515 / 516*/              {      EDD_HW_TYPE_USED_SOC/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_SOC1/*HWSubTypeUsed*/, 3/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/, EDDI_HSYNC_ROLE_FORWARDER/*HSYNCRole*/, EDDI_GIS_USECASE_IOC_SOC1_3P/*BufferCapacityUseCase*/, EDDI_I2C_TYPE_SOC1_HW/*I2C_Type*/, 2048/*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 32768/*LogicalAddressSpace*/, 16384/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 4/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*SOC1 PCIe card*/             {      EDD_HW_TYPE_USED_SOC/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_SOC1/*HWSubTypeUsed*/, 3/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_MANAGER/*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/, EDDI_HSYNC_ROLE_FORWARDER/*HSYNCRole*/, EDDI_GIS_USECASE_IOC_SOC1_3P/*BufferCapacityUseCase*/, EDDI_I2C_TYPE_SOC1_HW/*I2C_Type*/, 2048/*NrOfRTDevices*/, 64/*NrOfIRTDevices*/, 64/*NrOfRedIRTDevices*/, 1/*NrOfQVProviders*/, 16/*NrOfQVConsumers*/, 16/*NrOfRedQVConsumers*/, 32768/*LogicalAddressSpace*/, 16384/*NrOfSubmod*/, 1440/*MaxIOCRDataSize*/, 256/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 4/*NrOfARs*/, 720/*MaxOCRDataSize*/, 720/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/},
            /*ET 200SP IM155-6PN/3 HF*/    {      EDD_HW_TYPE_USED_SOC/*HWTypeUsed*/, EDDI_HW_SUBTYPE_USED_SOC1/*HWSubTypeUsed*/, 3/*NrOfPorts*/, EDD_FEATURE_ENABLE/*bIRTSupported*/, 128/*NrOfIRTForwarders*/, EDD_MRP_ROLE_CAP_CLIENT /*MRPDefaultRoleInstance0*/, 1/*MRPDefaultRingPort1*/, 2/*MRPDefaultRingPort2*/, EDDI_HSYNC_ROLE_FORWARDER/*HSYNCRole*/, EDDI_GIS_USECASE_IOC_SOC1_3P/*BufferCapacityUseCase*/, EDDI_I2C_TYPE_SOC1_HW/*I2C_Type*/, 0   /*NrOfRTDevices*/, 0 /*NrOfIRTDevices*/, 0 /*NrOfRedIRTDevices*/, 0/*NrOfQVProviders*/, 0 /*NrOfQVConsumers*/, 0 /*NrOfRedQVConsumers*/,    0 /*LogicalAddressSpace*/,     0/*NrOfSubmod*/,    0/*MaxIOCRDataSize*/,   0/*MaxMCRDataSize*/, 1/*NrOfInstances*/, 4/*NrOfARs*/, 500/*MaxOCRDataSize*/, 500/*MaxICRDataSize*/, 256/*MaxMCRDataSize*/, EDD_FEATURE_DISABLE/*AlternativeCalc*/, 0/*NrOfSubmodules*/, 0/*OutputBytes*/, 0/*InputBytes*/, EDD_FEATURE_DISABLE/*SharedARSupported*/,  EDD_FEATURE_ENABLE/*bMIIUsed*/, 0/*InterfaceID*/, 0 /*InitializeTheRest*/}
        }
    };

    #if defined (EDDI_CFG_ERTEC_400)
    ErtecTyp = EDD_HW_TYPE_USED_ERTEC_400;
    #elif defined (EDDI_CFG_ERTEC_200)
    ErtecTyp = EDD_HW_TYPE_USED_ERTEC_200;
    #elif defined (EDDI_CFG_SOC)
    ErtecTyp = EDD_HW_TYPE_USED_SOC;
    #endif
    
    pGIS->GenIni.HWTypeUsed              =             ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.HWSubTypeUsed           = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    
    pGIS->GenIni.NrOfPorts               = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.bIRTSupported           = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.NrOfIRTForwarders       = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    
    pGIS->GenIni.MRPDefaultRoleInstance0 = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.MRPDefaultRingPort1     = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.MRPDefaultRingPort2     = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.HSYNCRole               = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.BufferCapacityUseCase   = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->GenIni.I2C_Type                = (LSA_UINT32)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
                                       
    pGIS->IOCIni.NrOfRTDevices           = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.NrOfIRTDevices          = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.NrOfRedIRTDevices       = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.NrOfQVProviders         = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.NrOfQVConsumers         = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.NrOfRedQVConsumers      = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.LogicalAddressSpace     = ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.NrOfSubmod              = ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.MaxIOCRDataSize         = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IOCIni.MaxMCRDataSize          = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];

    pGIS->IODIni.NrOfInstances           = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.NrOfARs                 = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.MaxOCRDataSize          = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.MaxICRDataSize          = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.MaxMCRDataSize          = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];

    pGIS->IODIni.AlternativeCalc         = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.NrOfSubmodules          = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.OutputBytes             = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.InputBytes              = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];
    pGIS->IODIni.SharedARSupported       = (LSA_UINT16)ProductGISValues[ErtecTyp][ProductID][SubIndex++];

    pGIS->NRTIni.bMIIUsed                = (LSA_UINT8) ProductGISValues[ErtecTyp][ProductID][SubIndex++];                                  

    pGIS->InterfaceID                    = ProductGISValues[ErtecTyp][ProductID][SubIndex++];
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/*   MemMode :  memory type being used for sending and                     */
/*              receiving NRT frames                                       */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_BSPEddiInit( BSP_EDDI_INI_TYPE  *  const  pBSP_eddi_ini,
                            IRTE_SW_MAKRO_T    *  *      ppIRTE_SW_MAKRO_T )
{
    LSA_RESULT              Status = EDD_STS_OK;
    EDD_UPPER_RQB_PTR_TYPE  pRQB;
    LSA_BOOL                bRet;
    BSP_BASE_ADDR_T         BaseAddr;
    EDDI_DPB_TYPE           DPB;
    EDDI_DSB_TYPE           DSB;
    EDDI_RQB_COMP_INI_TYPE  COMP;  
    EDDI_GLOBAL_INI_TYPE    GIS;
    
    GIS.pInternalUse = LSA_NULL;
    
    LSA_UNUSED_ARG(Status);
    
    bRet = EDDI_BSPGetAddrFromBESY(&BaseAddr);
    if (!bRet)
    {
        return LSA_FALSE;
    }

    #if !defined (TEST_WITH_EDD_IN_DLL)
    Status = eddi_init();
    if (Status != EDD_STS_OK)
    {
        return LSA_FALSE;
    }
    #endif

    EDDI_ALLOC_UPPER_MEM_LOCAL((void * *)&pRQB, (LSA_UINT16)sizeof(EDD_RQB_TYPE));
    if (pRQB == (void *)0)
    {
        return LSA_FALSE;
    }

    EDDI_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, (LSA_UINT16)(sizeof(EDDI_DSB_TYPE) + 0x100));
    if (pRQB->pParam == (void *)0)
    {
        return LSA_FALSE;
    }

    EDDI_MEMSET(&DPB,  (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DPB_TYPE)); 
    EDDI_MEMSET(&COMP, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_RQB_COMP_INI_TYPE)); 
    EDDI_MEMSET(&DSB,  (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DSB_TYPE));   

    //Setup Convenience Function's
    SetGISValues(&GIS, 0/*default*/);

    Status = eddi_InitDPBWithDefaults(&DPB, &GIS);
    if (EDD_STS_OK != Status)
    {
        printf("Error: eddi_InitDPBWithDefaults()");
        return LSA_FALSE;  
    }

    Status = eddi_InitCOMPWithDefaults(&COMP, &GIS);
    if (EDD_STS_OK != Status)
    {
        printf("Error: eddi_InitCOMPWithDefaults()");
        return LSA_FALSE;
    }

    Status = eddi_InitDSBWithDefaults(&DSB, &GIS);
    if (EDD_STS_OK != Status)
    {
        printf("Error: eddi_InitDSBWithDefaults()");
        return LSA_FALSE;
    }

    if (GIS.KRAMFree <= 0)
    {
        printf("Error: Not enough KRAM %d", GIS.KRAMFree);
        return LSA_FALSE;    
    } 

    EDDI_BspEddiSystemDevOpen(pRQB, &BaseAddr, ppIRTE_SW_MAKRO_T, &DPB);
    if (EDD_RQB_GET_RESPONSE(pRQB) != EDD_STS_OK)
    {
        return LSA_FALSE;
    }

    EDDI_BspEddiSystemBasicIni(pRQB, pBSP_eddi_ini->MemMode, &COMP);
    if (EDD_RQB_GET_RESPONSE(pRQB) != EDD_STS_OK)
    {
        return LSA_FALSE;
    }

    EDDI_BspEddiSystemDevSetup(pRQB, pBSP_eddi_ini, &DSB);
    if (EDD_RQB_GET_RESPONSE(pRQB) != EDD_STS_OK)
    {
        return LSA_FALSE;
    }

    #if !defined (TEST_WITH_EDD_IN_DLL)
    //register ISR -> can take place in former times!
    EDDI_BSPSetSystemInterruptServiceFunction(EDDI_BSPISR);

    bRet = EDDI_BSPSysIntEnable();
    #endif //TEST_WITH_EDD_IN_DLL

    return bRet;
}
/*---------------------- end [subroutine] ---------------------------------*/


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
LSA_BOOL EDDI_BSPEddiDeinit( void )
{
    #if !defined (TEST_WITH_EDD_IN_DLL)
    LSA_RESULT              Status;
    #endif
    EDD_UPPER_RQB_PTR_TYPE  pRQB;
    LSA_UINT16              ret16_val;
    LSA_BOOL                bRet;

    EDDI_ALLOC_UPPER_MEM_LOCAL((void **)&pRQB, (LSA_UINT16)sizeof(EDD_RQB_TYPE) );
    if (pRQB == (void *)0)
    {
        return LSA_FALSE;
    }

    EDDI_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, (LSA_UINT16)(sizeof(EDDI_DSB_TYPE) + 0x100) );
    if (pRQB->pParam == (void *)0)
    {
        return LSA_FALSE;
    }

    bRet = EDDI_BspEddiDevClose(pRQB);

    EDDI_FREE_UPPER_MEM_LOCAL(&ret16_val, pRQB->pParam );
    if (ret16_val != EDD_STS_OK)
    {
        return LSA_FALSE;
    }
    EDDI_FREE_UPPER_MEM_LOCAL(&ret16_val, pRQB );
    if (ret16_val != EDD_STS_OK)
    {
        return LSA_FALSE;
    }

    #if !defined (TEST_WITH_EDD_IN_DLL)
    Status = eddi_undo_init();
    if (Status != EDD_STS_OK)
    {
        return LSA_FALSE;
    }
    #endif

    return bRet;
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  LSA_BOOL  EDDI_BspEddiDevClose( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_RQB_DDB_REL_TYPE  *  pDevClose;
    LSA_UINT32                Ctr;

    pDevClose       = (EDDI_RQB_DDB_REL_TYPE *)pRQB->pParam;
    pDevClose->hDDB = g_SyshDDB;
    pDevClose->Cbf  = (BSP_SYS_CBF)0;   //close without CBF ("synchronous close") ==> needs to be polled!

    EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
    pRQB->Service = EDDI_SRV_DEV_CLOSE;

    #if defined (TEST_WITH_EDD_IN_DLL)
    printf ("*** %-18s ***\n", "Close Device");
    #endif

    //Attention: example only!
    for (Ctr=0; Ctr<0xFFFFUL; Ctr++)
    {
        //poll until finished
        eddi_system(pRQB);
        if (EDD_STS_OK == EDD_RQB_GET_RESPONSE(pRQB))
        {
            break;
        }
        //wait(1000);
    }
    
    if (EDD_RQB_GET_RESPONSE(pRQB) != EDD_STS_OK)
    {
        //printf("F_eddi_close - Error DEV Close!!, Response = 0x%lx\n", (P16_DWORD)pRQB->response);
        return LSA_FALSE;
    }
    else
    {
        //printf("F_eddi_close -> DEV_CLOSE - Ok !!\n");
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  void  EDDI_BspEddiSystemDevOpen(       EDD_UPPER_RQB_PTR_TYPE            pRQB,
                                         const BSP_BASE_ADDR_T         *  const  pBaseAddr,
                                               IRTE_SW_MAKRO_T              * *  ppIRTE_SW_MAKRO_T,
                                               EDDI_UPPER_DPB_PTR_TYPE           pDPB )
{
    EDDI_RQB_DDB_INI_TYPE  *  ini;

    /* The InterfaceID is a number uniquely identifying the interface (device) and should correspond to the 
       case labelling. This number together with the port number is used by CM for the interface submodule numbering. 
       EDDI only propagates the InterfaceID without using it. It can be read by the service EDD_SRV_GET_PARAMS. 
       Each EDD(I)-device needs to get it´s own unique InterfaceID. 
       Possible values: 0..15. Derives from the interface-subslot nr (0x8ipp), where i is the InterfaceID and pp the port number. */

    pDPB->InterfaceID = 0;

    // same base-address as the processor / IRTE sees the local memory of the PCI card
    // -> chipselect is hardcoded in EDDI for the PCI cards
    // see also: eddi_dev.c -> DEV_loc2_sdram_adr_local_to_asic

    pDPB->BOARD_SDRAM_BaseAddr = (LSA_UINT32)(void *)pBaseAddr->pBOARD_SDRAM_BaseAddr;

    //allocate shared memory for 2proc mem
    #if !defined (EDDI_CFG_3BIF_2PROC)
    EDDI_ALLOC_UPPER_MEM_LOCAL((void * *)&pDPB->GSharedRAM_BaseAdr, (LSA_UINT16)sizeof(EDDI_GSHAREDMEM_TYPE));
    #endif

    // same base-address as the processor / IRTE sees the local memory of the PCI card
    // 0 MByte: Irte Switch Struktur -> see eddi_irte.h
    // 1.Mbyte: KRAM -> 192/64 kByte
    // -> chipselect is hardcoded in EDDI for the PCI cards
    // i.e.: eddi_dev.c -> DEV_eddi_kram_adr_local_to_asic_dma
    pDPB->IRTE_SWI_BaseAdr             = (LSA_UINT32)(void *)pBaseAddr->irte;
    pDPB->NRTMEM_LowerLimit            = 0;             /* Lower Limit of NRT-Memory (virt.) */
    pDPB->NRTMEM_UpperLimit            = 0xFFFFFFFFUL;  /* Upper Limit of NRT-Memory (virt.) */

    /* ERTEC 200 special case -> EDD accesses the IRTE, KRAM, SDRAM  via the ERTEC 200 LBU interface of the ERTEC 200
       for that special case the EDD needs an additional pointer to the IRTE address space

        The 4 LBU address spaces have to be configured( by the hardware developper) as follows

        New: -> IRTE_SWI_BaseAdr_LBU_16Bit:  pointer for correct 16/8 Bit access
        Old: -> IRTE_SWI_BaseAdr          :  pointer for correct 32   Bit access
        Old: -> BOARD_SDRAM_BaseAddr      :  pointer for correct 16/8 Bit access
        Old: -> ERTECx00_SCRB             :  pointer for correct 32   Bit access
    */

    pDPB->IRTE_SWI_BaseAdr_LBU_16Bit   = (LSA_UINT32)0; // LBU access modell not selected
    /* - LBU access modell selected
       - allowed only for an ERTEC 200
    pDPB->IRTE_SWI_BaseAdr_LBU_16Bit = pDPB->IRTE_SWI_BaseAdr + 0x200000;
    */

    // both addresses are not relevant for a pure firmware solution */
    pDPB->ERTECx00_SCRB_BaseAddr        = (LSA_UINT32)(void *)pBaseAddr->pERTECx00_SCRB;

    /* This parameter should be set to LSA_FALSE, otherwise the EDD doesn´t executes a reset to the
       IRTE-Switch part of the respective hardware (ERTEC400, ERTEC200). If this reset is not executed,
       the behavior of the IRTE-Switch is undefined and could reach in predictable effects. If this
       parameter is set to LSA_TRUE (no reset to the IRTE), the user is responsible that the IRTE-Switch
       part of the hardware has an initial state which is accepted by the EDDI. This requires very exact
       hardware knowledges about register settings of the IRTE-Switch. */
    //pDPB->EnableReset = EDD_FEATURE_ENABLE;
    //pDPB->EnableReset = EDD_FEATURE_DISABLE; //Use Case NRK !

    // deactivate the ERTEC 400 bootloader for the following use case:
    // -> no firmware (EDD on Host processor), device will be used as PCI Slave
    pDPB->Bootloader.Activate_for_use_as_pci_slave = EDD_FEATURE_ENABLE;

    /*!< Base address of the 144 kByte PAEA-RAM (SOC1/2).
         Setting an address enables IO-Control and moves the process image to the PAEA-RAM.
         In the case of using IOC, the variable pDPB->KRam.offset_ProcessImageEnd defines the memory that can
         be used by EDDI_ERTEC for StateBuffers (8 byte each).  */
    #if defined (EDDI_CFG_SOC)
    pDPB->PAEA_BaseAdr = (LSA_UINT32)pBaseAddr->pPC_HostVirt_BaseAdrIOCC;
    #else
    pDPB->PAEA_BaseAdr = 0;
    #endif

    //SII configuration parameters
    pDPB->SII_IrqSelector       = EDDI_SII_IRQ_HP;          //EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
    pDPB->SII_IrqNumber         = EDDI_SII_IRQ_1;           //EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    pDPB->SII_ExtTimerInterval  = 500UL;    //in us         //0, 250, 500, 1000 in us
    #endif
    #if defined (EDDI_CFG_SII_FLEX_MODE)
    pDPB->SII_Mode              = EDDI_SII_STANDARD_MODE;   //EDDI_SII_STANDARD_MODE or EDDI_SII_POLLING_MODE
    #endif

    EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
    pRQB->Service = EDDI_SRV_DEV_OPEN;

    ini = (EDDI_RQB_DDB_INI_TYPE *)(pRQB->pParam);

    ini->Cbf    = (BSP_SYS_CBF)0; // synchron
    ini->pDPB   = pDPB;

    #if defined (TEST_WITH_EDD_IN_DLL)
    printf("*** %-18s ***\n\n", "DeviceOpen");
    #endif

    eddi_system(pRQB);

    g_SyshDDB = ini->hDDB;

    *ppIRTE_SW_MAKRO_T = (IRTE_SW_MAKRO_T *)(void *)pDPB->IRTE_SWI_BaseAdr;

    MD_PortCnt = pDPB->PortMap.PortCnt;
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  void  EDDI_BspEddiSystemBasicIni( EDD_UPPER_RQB_PTR_TYPE       const  pRQB,
                                          EDDI_MEMORY_MODE_TYPE        const  MemMode,
                                          EDDI_UPPER_CMP_INI_PTR_TYPE         pCompIni )
{
    EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);

    pRQB->Service  = EDDI_SRV_DEV_COMP_INI;

    pRQB->pParam   = (LSA_VOID *)pCompIni;

    pCompIni->Cbf  = (BSP_SYS_CBF)0; //synchron

    pCompIni->hDDB = g_SyshDDB;


    //CH A0 **************************************************************************************
    pCompIni->NRT.u.IF.A__0.MaxRcvFrame_ToLookUp                  = 10;
    pCompIni->NRT.u.IF.A__0.MaxRcvFrame_SendToUser                = 4;
    pCompIni->NRT.u.IF.A__0.MaxRcvFragments_ToCopy                = 10;

    // in special cases it could be useful to adjust the memory mode per channel
    pCompIni->NRT.u.IF.A__0.MemModeBuffer  = MemMode;
    pCompIni->NRT.u.IF.A__0.MemModeDMACW   = MemMode;

    //CH B0 **************************************************************************************
    //pIF->MaxRcvFrame_ToLookUp                = 0; // no receive load limitation
    pCompIni->NRT.u.IF.B__0.MaxRcvFrame_ToLookUp                  = 10;
    pCompIni->NRT.u.IF.B__0.MaxRcvFrame_SendToUser                = 4;
    pCompIni->NRT.u.IF.B__0.MaxRcvFragments_ToCopy                = 10;
    
    // in special cases it could be useful to adjust the memory mode per channel
    pCompIni->NRT.u.IF.B__0.MemModeBuffer  = MemMode;
    pCompIni->NRT.u.IF.B__0.MemModeDMACW   = MemMode;

    #if defined (BSP_DEFFERED_ALLOC)    // special use cases:
                                        // -> edd on ARM-processor, RxBuffer from host, host at initialization time not present
                                        // -> only this combination of memory mode is allowed !
    pCompIni->NRT.u.IF.B__0.MemModeBuffer  = MEMORY_SHARED_MEM;  // fix
    pCompIni->NRT.u.IF.B__0.MemModeDMACW   = MEMORY_SDRAM_ERTEC; // fix
    
    // Attention: before open_channel on CHB_IF_0 you have to call the function
    // <eddi_SetHostAddressForDeferredAlloc>  -> see eddi_sys.h
    #endif

    //ini SWITCH-Parameter  for SWITCH-COMP-INI
    //================= Configuration of SWITCH ===============================

    // ERTEC 200 Workaround for internal PHY : PHY doesn´t recognize cable defect, if only one line is broken
    pCompIni->PhyBugfix.MysticCom.bActivate               = LSA_TRUE;   // general Activation
    pCompIni->PhyBugfix.MysticCom.bReplaceDefaultSettings = LSA_FALSE;  // internal default settings
    //  ini->phy_bugfix.MysticCom.Tp_RcvFrame_InMs      = 30;
    //  ini->phy_bugfix.MysticCom.Diff_ESD              = 15;
    //  ini->phy_bugfix.MysticCom.T_LinkDown_NoRcv_InMs = 0; // switched off
    //  ini->phy_bugfix.MysticCom.T_LinkDown_NoRcv_InMs = 1000 * 60 * 3; // 3 minutes

    #if defined (TEST_WITH_EDD_IN_DLL)
    printf("\n*** %-18s ***\n", "BASIC_INI");
    #endif

    // ini CYCLE-Parameter
    // example: adjust cycle (i.e. without reduction)
    // always apply that ClkCy_Period * AplClk_Divider = cycle length in 10ns ticks

    pCompIni->CycleComp.ClkCy_Period         = 3125;
    pCompIni->CycleComp.AplClk_Divider       = 32;
    // !! not release ClkCy_Period Interrupt -> arises with the thousandfold frequency of new cycle
    pCompIni->CycleComp.ClkCy_Int_Divider    = pCompIni->CycleComp.AplClk_Divider;

    // no Delay at ClkCy Interrupt
    pCompIni->CycleComp.ClkCy_Delay          = 0x00000000;
    // no Delay at ClkCy_Int Interrupt
    pCompIni->CycleComp.ClkCy_Int_Delay      = 0x00000000;

    pCompIni->CycleComp.CompVal1Int          = EDD_FEATURE_DISABLE;
    pCompIni->CycleComp.CompVal2Int          = EDD_FEATURE_DISABLE;
    pCompIni->CycleComp.CompVal3Int          = EDD_FEATURE_DISABLE;
    pCompIni->CycleComp.CompVal4Int          = EDD_FEATURE_DISABLE;

    // release CompVal5Int
    pCompIni->CycleComp.CompVal5Int          = LSA_FALSE; /* for testing release with LSA_TRUE --> no function at the moment in edd */

    pCompIni->CycleComp.CompVal1             = 0x00000000;
    pCompIni->CycleComp.CompVal2             = 0x00000000;
    pCompIni->CycleComp.CompVal3             = 0x00000000;
    pCompIni->CycleComp.CompVal4             = 0x00000000;

    // CompVal5Int zur Zyklusmitte
    // pCompIni->CycleComp.CompVal5          = (CycleTimeIn_us * 100) / 2; // For testing only --> reserved for further use in EDD
    pCompIni->CycleComp.CompVal5             = 0; // Deactivated

    /* For controlling the signals XPLL_Ext_Out and XPLL_EXT_IN to synchronize, following steps are necessary                     */
    /* EDDI_CYCLE_INI_PLL_MODE_OFF or EDDI_CYCLE_INI_PLL_MODE_XPLL_EXT_OUT must be selected                                       */

    /* During the ERTEC is running the register AplClk_Divider will adjust to the CycleLength, the register ClkCy_Period is fix   */
    /* The phase of the synchronize signal XPLL_Ext_Out -> T(XPLL_Ext_Out) = T(Register[ClkCy_Period]) * Register[AplClk_Divider] */
    /* The register ClkCy_Period will be set on 3125 and register AplClk_Divider on CycleBaseFactor                               */
    pCompIni->CycleComp.PDControlPllMode       = EDDI_CYCLE_INI_PLL_MODE_NOTUSED;

    /* RToverUDP */
    /*  Type of Service can be used to modify priority and Quality of Service for
                                   RToverUDP frames -> Default = 0x00.

                                   Bit    0: Reserved        -> 0

                                   Bit    1: Cost            -> 0 -- normal
                                                                  -- minimize monetary cost

                                   Bit    2: Reliability     -> 0 -- normal
                                                                1 -- high

                                   Bit    3: Throughput      -> 0 -- normal
                                                                1 -- high

                                   Bit    4: Delay Sensitive -> 0 -- normal
                                                                1 -- minimize delay

                                   Bit 5..7: Precedence      -> 0 -- Routine
                                                                1 -- Priority
                                                                2 -- Immediate
                                                                3 -- Flash
                                                                4 -- Flash Override
                                                                5 -- CRITIC/ECP
                                                                6 -- Internetwork Control
                                                                7 -- Network Control         */

    #if defined (TEST_WITH_EDD_IN_DLL)
    printf("*** %-18s ***\n", "RT_INI");
    #endif

    eddi_system(pRQB);

    // values of the internal structure pCompIni->phy_bugfix can be read backwards
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  void  EDDI_BspEddiSystemRtIni( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    eddi_system(pRQB);
}
/*---------------------- end [subroutine] ---------------------------------*/


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
static  void  EDDI_BspEddiSystemDevSetup(       EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                          const BSP_EDDI_INI_TYPE        *  const  pBSP_eddi_ini,
                                                EDDI_UPPER_DSB_PTR_TYPE            pDSB )
{
    //Device-VLAN-ID
    LSA_UINT32                  UsrPortIndex, Ctr;
    EDDI_RQB_DDB_SETUP_TYPE  *  ini;
    
    EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
    pRQB->Service = EDDI_SRV_DEV_SETUP;

    ini = (EDDI_RQB_DDB_SETUP_TYPE *)pRQB->pParam;

    ini->Cbf  = (BSP_SYS_CBF)0; //synchron
    ini->hDDB = g_SyshDDB;

    ini->pDSB = pDSB;

    //ini DSB Device-SetupBlock
    /*================ Configuration of AsyncReceive ========================*/

    /* a PHY adress is attached to each PHY
       if port-mapping is switched on, be certain that the mapping
       happens from user-view and nor from hardware-view */
    for (Ctr = 0; Ctr < EDD_CFG_MAX_PORT_CNT; Ctr++)
    {
        pDSB->GlobPara.PortParams[Ctr].PhyAdr = EDDI_PhyAdr_NOT_CONNECTED;
    }

    pDSB->GlobPara.PortParams[0].PhyAdr = pBSP_eddi_ini->PortParam[0].PhyAdr;
    pDSB->GlobPara.PortParams[1].PhyAdr = pBSP_eddi_ini->PortParam[1].PhyAdr;
    pDSB->GlobPara.PortParams[2].PhyAdr = pBSP_eddi_ini->PortParam[2].PhyAdr;
    pDSB->GlobPara.PortParams[3].PhyAdr = pBSP_eddi_ini->PortParam[3].PhyAdr;

    /* for delay measurement the signal propagation delay in send / receive direction is specified
       in ns through PHY and MAC. The signal propagation delay has to be determined by a explicit
       measurement or a data sheet of the manufacturer */

    /* for the following hardware variants the signal propagation delay has been determined
       It applies to find out the corresponding hardware variant and to comment out the code-sequence */
    for (UsrPortIndex = 0; UsrPortIndex < MD_PortCnt; UsrPortIndex++)
    {
        /* port-specific sourceMAC-address */
        pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[0] = 0x08;
        pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[1] = 0x09;
        pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[2] = 0x09;
        pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[3] = 0x09;
        pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[4] = 0x09;
        pDSB->GlobPara.PortParams[UsrPortIndex].MACAddress.MacAdr[5] = (LSA_UINT8)(UsrPortIndex + 1);

        /****** Attention **************************************
           for AutonegCapAdvertised 
           and AutonegMappingCap
           see new output-function EDDI_LL_GET_PHY_PARAMS !!!
           
           The "old" parameters  MediaType,
                                 AutonegCapAdvertised,
                                 AutonegMappingCapability
           do not exist here anymore!
        ********************************************************/

        pDSB->GlobPara.PortParams[UsrPortIndex].MRPRingPort = EDD_MRP_NO_RING_PORT; /* No MRP R-Port */

        pDSB->GlobPara.PortParams[UsrPortIndex].IsMDIX      = EDD_PORT_MDIX_DISABLED; /* When Auto-Negotiation is off, MDIX is disabled */
    }

    pDSB->GlobPara.MRPDefaultRoleInstance0  = EDD_MRP_ROLE_NONE; /* EDD_MRP_ROLE_MANAGER or EDD_MRP_ROLE_CLIENT or _NONE */
    pDSB->GlobPara.MRPSupportedRole = EDD_MRP_ROLE_CAP_MANAGER + EDD_MRP_ROLE_CAP_CLIENT + EDD_MRP_ROLE_CAP_AUTOMANAGER; /* MRP capabilities */

    pDSB->GlobPara.MaxMRP_Instances = 1;
    pDSB->GlobPara.MRPSupportedMultipleRole = 0;

    /*================ Configuration of Ethernet-Port =======================*/

    //end of ini DSB

    #if defined (TEST_WITH_EDD_IN_DLL)
    printf("*** %-18s ***\n", "DeviceSetup");
    #endif

    eddi_system(pRQB);

    #if defined (TEST_WITH_EDD_IN_DLL)
    printf("*** DeviceSetup: free_internal_KRAM_buffer = 0x%lx ***\n", pDSB->free_internal_KRAM_buffer);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/****************************************************************************/
/*  end of file eddi_bsp_edd_ini.c                                          */
/****************************************************************************/
