#ifndef EDDS_PHY_H                       /* ----- reinclude-protection ----- */
#define EDDS_PHY_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDS (EDD for Standard MAC)               :C&  */
/*                                                                           */
/*  F i l e               &F: EDDS_phy.h                                :F&  */
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
#ifdef EDDS_MESSAGE
/*  09.03.15    TH    initial version.                                       */
#endif
/*****************************************************************************/
typedef enum PHY_CYCLE_ENUM {
    PHY_READ_CYCLE,
    PHY_WRITE_CYCLE,
    PHY_WRITE_POST_RESET
}PHY_CYCLE_TYPE;

typedef PHY_CYCLE_TYPE EDD_UPPER_MEM_ATTR * PHY_CYCLE_PTR_TYPE;

typedef struct EDDS_PORT_MAP_STRUCT
{
    LSA_UINT32       PortID_x_to_MDIO_USERACCESS;
    LSA_UINT32       PortID_x_to_HardwarePort_y;
    LSA_UINT32       PhyIndex_x_to_HardwarePort_y;
    
    LSA_UINT8        TXCDelay_50ps;
    LSA_UINT8        RXCDelay_50ps; 
} EDDS_PORT_MAP_TYPE;

#define EDDS_PHY_PORT_NOT_CONNECTED       0xFFFFFFFFUL
#define EDDS_PHY_TIMEOUT_DEFAULT          1000000LLU
  
/*============================== Prototypes =================================*/

/*=============================================================================
 * function name:  EDDS_PHY_GetFunctionPointers
 *
 * function:       This is a synchron outbound function which returns
 *                 a structure of function pointers that are provided by
 *                 system adaption in order to control the PHYs. System
 *                 adaption can select the right functions by evaluating the
 *                 OUIValue parameter (vendor register value).
 *===========================================================================*/

typedef struct EDDS_PHY_INIT_STRUCT
{
        LSA_UINT32 LinkStatusRegAddress;
        LSA_UINT32 LinkStatusBitMask;
        LSA_UINT32 SpeedRegAddress;
        LSA_UINT32 SpeedBitMask;
        LSA_UINT32 DuplexityRegAddress;
        LSA_UINT32 DuplexityBitMask;
} EDDS_PHY_INIT_TYPE;
typedef EDDS_PHY_INIT_TYPE EDDS_LOWER_MEM_ATTR * EDDS_PHY_INIT_PTR_TYPE;
/* -------------------------------------------------------------------------------------------------------- */
/*      LinkStatusRegAddress            : PHY register address that contains link status information        */
/*      LinkStatusBitMask               : Bit mask to mark relevant bits for link status information        */
/*      SpeedRegAddress                 : PHY register address that contains speed status information       */
/*      SpeedBitMask                    : Bit mask to mark relevant bits for speed status information       */
/*      DuplexityRegAddress             : PHY register address that contains duplexity status information   */
/*      DuplexityBitMask                : Bit mask to mark relevant bits for duplexity status information   */
/* -------------------------------------------------------------------------------------------------------- */

typedef struct EDDS_PHY_HANDLE_STRUCT EDDS_PHY_HANDLE_TYPE;
typedef EDDS_PHY_HANDLE_TYPE EDD_UPPER_MEM_ATTR             *   EDDS_PHY_HANDLE_PTR_TYPE;
typedef EDDS_PHY_HANDLE_TYPE EDD_UPPER_MEM_ATTR     const   *   EDDS_PHY_HANDLE_PTR_TO_CONST_TYPE;

typedef struct EDDS_PHY_FCT_STRUCT
{
        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_PHY_Init)(
                EDDS_PHY_HANDLE_PTR_TYPE    pPhyHandle,
                LSA_UINT32                  HwPortID);

        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_PHY_SetPowerDown)(
                EDDS_PHY_HANDLE_PTR_TO_CONST_TYPE   pPhyHandle,
                LSA_UINT32                          HwPortID,
                LSA_BOOL                            PowerDown);

        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_PHY_LED_SetMode)(
                EDDS_PHY_HANDLE_PTR_TO_CONST_TYPE   pPhyHandle,
                LSA_UINT32                          HwPortID,
                LSA_BOOL                            LEDOn);

        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_PHY_LED_BackupMode)(
                EDDS_PHY_HANDLE_PTR_TO_CONST_TYPE   pPhyHandle,
                LSA_UINT32                          HwPortID,
                LSA_BOOL                            Restore);

        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_PHY_RecurringTask)(
                EDDS_PHY_HANDLE_PTR_TYPE    pPhyHandle,
                LSA_UINT32                  HwPortID,
                LSA_UINT64                  timeout);
    
        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_Get_PhyReg)(
                EDDS_PHY_HANDLE_PTR_TO_CONST_TYPE   pPhyHandle,
                LSA_UINT32      const               HwPortID,
                LSA_UINT16  *                       pData,
                LSA_UINT16      const               addr,
                LSA_UINT64      const               timeout);
    
        EDD_RSP LSA_FCT_PTR(EDDS_SYSTEM_OUT_FCT_ATTR, EDDS_Set_PhyReg)(
                EDDS_PHY_HANDLE_PTR_TO_CONST_TYPE    pPhyHandle,
                LSA_UINT32  const                   HwPortID,
                LSA_UINT16  const                   data,
                LSA_UINT16  const                   addr,
                LSA_UINT64  const                   timeout);
    
}EDDS_PHY_FCT_TYPE;

typedef EDDS_PHY_FCT_TYPE EDDS_LOWER_MEM_ATTR * EDDS_PHY_FCT_PTR_TYPE;

typedef struct EDDS_PHY_HANDLE_STRUCT
{
    LSA_UINT32              TraceIdx;
    EDDS_SYS_HANDLE         hSysDev; 
    EDDS_PORT_MAP_TYPE      PortMap;
    LSA_UINT32              ConfiguredPorts;
    LSA_VOID_PTR_TYPE       pBaseAddr;
    EDDS_PHY_FCT_TYPE       FuncPtrs;
    LSA_UINT32              phyAddr;
    PHY_CYCLE_TYPE          phyCycle;
    LSA_UINT8               LinkSpeed;
    LSA_UINT8               LinkMode;
    LSA_UINT8               LinkStatus;
    LSA_UINT8               LinkSpeed_Shadow;
    LSA_UINT8               LinkMode_Shadow;
    LSA_UINT8               LinkStatus_Shadow;
    LSA_UINT32              AutoNegCapAdvertised;
    LSA_UINT8               LinkSpeedModeConfigured;
    LSA_UINT8               SavedLinkStat;
    LSA_UINT8               SavedLinkPhyPowerMode;
    LSA_UINT8               SetLinkStat;
    LSA_UINT8               SetLinkPhyPowerMode;
    LSA_BOOL                Disable1000MBitSupport;
} EDDS_PHY_HANDLE_TYPE;

EDD_RSP EDDS_PHY_FinishSetup(
        EDDS_PHY_HANDLE_PTR_TYPE    pPhyHandle,
        LSA_UINT32                  HwPortID);  
	
#define EDDS_PHY_ACCESS(handle,func)    \
        handle->FuncPtrs.func(

#define EDDS_PHY_SET_PHY(handle,HwPortID,data,addr,timeout)   \
        EDDS_PHY_ACCESS(handle,EDDS_Set_PhyReg)handle,HwPortID,data,addr,timeout)

#define EDDS_PHY_GET_PHY(handle,HwPortID,data,addr,timeout)   \
        EDDS_PHY_ACCESS(handle,EDDS_Get_PhyReg)handle,HwPortID,data,addr,timeout)

#define EDDS_PHY_INIT(handle,HwPortID) \
    EDDS_PHY_ACCESS(handle,EDDS_PHY_Init)handle,HwPortID)

#define EDDS_PHY_SET_POWER_DOWN(handle,HwPortID,PowerDown)    \
        EDDS_PHY_ACCESS(handle,EDDS_PHY_SetPowerDown)handle,HwPortID,PowerDown)

#define EDDS_PHY_LED_SET_MODE(handle,HwPortID,LedOn)    \
        EDDS_PHY_ACCESS(handle,EDDS_PHY_LED_SetMode)handle,HwPortID,LedOn)

#define EDDS_PHY_LED_BACKUP_MODE(handle,HwPortID,BackupMode)    \
        EDDS_PHY_ACCESS(handle,EDDS_PHY_LED_BackupMode)handle,HwPortID,BackupMode)

#define EDDS_PHY_RECURRING_TASK(handle,HwPortID,timeout)    \
        EDDS_PHY_ACCESS(handle,EDDS_PHY_RecurringTask)handle,HwPortID,timeout)

#define EDDS_PHY_GET_MASK(addr)                     (1 << addr)
#define EDDS_PHY_SET_BIT(value,addr)                (value |= EDDS_PHY_GET_MASK(addr))
#define EDDS_PHY_CLR_BIT(value,addr)                (value &= ~EDDS_PHY_GET_MASK(addr))

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EDDS_PHY_H */
