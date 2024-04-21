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
/*  F i l e               &F: eddi_Tra_Userspec.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-system input-functions                      */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#if defined (EDDI_NULL_PTR)     //make DACODA happy for including a not needed file
#endif                          //taking it out, the compiler complains about an empty file


#if defined (TOOL_CHAIN_MS_WINDOWS)
#pragma warning( disable : 4206 ) //empty file
#endif

#if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)

#include "eddi_dev.h"
#include "eddi_Tra.h"

#define EDDI_MODULE_ID     M_ID_TRA_USERSPEC
#define LTRC_ACT_MODUL_ID  508

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  Transceiver: XXXXXXXXXXXXXX                                               */
/*               ==============                                               */
/*                                                                            */
/*                                                                            */
/*  Library for PHY specific routines                                         */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/


/*===========================================================================*/
/*                                  typedefs                                 */
/*===========================================================================*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetPowerDown_Userspec()                  */
/*                                                                         */
/* D e s c r i p t i o n: Switch PHY to PowerDown                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex,
                                                                LSA_UINT8                const  PHYPower,
                                                                LSA_BOOL                 const  bRaw)

{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetPowerDown_Userspec->HwPortIndex:0x%X PHYPower:0x%X", HwPortIndex, PHYPower);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
    LSA_UNUSED_ARG(PHYPower); //satisfy lint!
    LSA_UNUSED_ARG(bRaw); //satisfy lint!

    //implement function here

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAStartPhy_Userspec()                      */
/*                                                                         */
/* D e s c r i p t i o n: Actions to take before PHY-reset is released     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_BOOL                 const  bPlugCommit)
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_TRAStartPhy_Userspec->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
    LSA_UNUSED_ARG(bPlugCommit); //satisfy lint!

    //implement function here
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAGetSpMoAutoNegOn_Userspec()              */
/*                                                                         */
/* D e s c r i p t i o n: Gets Speed/Mode for Autoneg ON                   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          SWI_AUTONEG_GET_SPEEDMODE_TYPE                   */
/*                                                                         */
/***************************************************************************/
SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                        LSA_UINT32               const  HwPortIndex,
                                                                                        SER_SWI_LINK_PTR_TYPE    const  pBasePx )
{
    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_TRAGetSpMoAutoNegOn_Userspec->HwPortIndex:0x%X", HwPortIndex);

    if (pDDB || HwPortIndex || pBasePx)
    {
        pBasePx->LinkSpeed = EDD_LINK_UNKNOWN;
        pBasePx->LinkMode  = EDD_LINK_UNKNOWN;
    }

    //implement function here

    return SWI_AUTONEG_GET_SPEEDMODE_LINKDOWN;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRACheckSpeedModeCapability_Userspec()      */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_Userspec( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pAutonegCapability )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRACheckSpeedModeCapability_Userspec->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pAutonegCapability); //satisfy lint!

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            /* todo */
        }
        break;

        //Fiber
        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
        {
            /* todo */
        }
        break;

        //other values are not allowed
        default:
        {
            EDDI_Excp("EDDI_TRACheckSpeedModeCapability_Userspec, MediaType", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRAAutonegMappingCapability_Userspec()      */
/*                                                                         */
/* D e s c r i p t i o n: Reports the possible Speed/Mode capabilities of  */
/*                        the PHY                                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_Userspec( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pConfiguredLinkSpeedAndMode )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRAAutonegMappingCapability_Userspec->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pConfiguredLinkSpeedAndMode); //satisfy lint!

    switch (pDDB->Glob.PortParams[HwPortIndex].MediaType)
    {
        //Copper
        case EDD_MEDIATYPE_COPPER_CABLE:
        {
            /* todo */
        } break;

        //Fiber
        case EDD_MEDIATYPE_FIBER_OPTIC_CABLE:
        {
            /* todo */
        } break;

        //other values are not allowed
        default:
        {
            EDDI_Excp("EDDI_TRAAutonegMappingCapability_Userspec, MediaType", EDDI_FATAL_ERR_EXCP, pDDB->Glob.PortParams[HwPortIndex].MediaType, 0);
            return EDD_STS_ERR_EXCP;
        }
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetMDIX_Userspec()                       */
/*                                                                         */
/* D e s c r i p t i o n: Controls the MDIX feature of the PHY.            */
/*                        (Manual control of crossover)                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_BOOL                 const  bMDIX )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetMDIX_Userspec->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
    LSA_UNUSED_ARG(bMDIX); //satisfy lint!

    //implement function here

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRARestoreAutoMDIX_Userspec()               */
/*                                                                         */
/* D e s c r i p t i o n: Restores the AutoMDIX feature of the PHY.        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  HwPortIndex )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRARestoreAutoMDIX_Userspec->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    //implement function here

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TraSetFXMode_Userspec()                     */
/*                                                                         */
/* D e s c r i p t i o n: Controls the FXMode feature of the PHY.          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bFXModeON )
{
    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TraSetFXMode_Userspec->HwPortIndex:0x%X  FXModeON:0x%X", HwPortIndex, bFXModeON);

    LSA_UNUSED_ARG(pDDB);           //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex);    //satisfy lint!
    LSA_UNUSED_ARG(bFXModeON);      //satisfy lint!

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_TRASetSoftReset_Userspec()                  */
/*                                                                         */
/* D e s c r i p t i o n: Set Softreset of the PHY.                        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex )
{
    EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_TRASetSoftReset_Userspec->HwPortIndex:0x%X", HwPortIndex);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    //implement function here

    return EDD_STS_ERR_NOT_IMPL;
}
/*---------------------- end [subroutine] ---------------------------------*/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  LED blink (show location) implementation for user specific PHY            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBeginExternal_Userspec()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_BEGIN(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_USERSPEC);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetModeExternal_Userspec()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_UINT32               const  HwPortIndex,
                                                                         LSA_UINT16               const  LEDMode )
{
    EDDI_LL_LED_BLINK_SET_MODE(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_USERSPEC, LEDMode);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEndExternal_Userspec()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() after blinking of LED(s) has finished.
 *                 Can be used e.g. to re-enable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                     LSA_UINT32               const  HwPortIndex )
{
    EDDI_LL_LED_BLINK_END(pDDB->hSysDev, HwPortIndex, EDDI_PHY_TRANSCEIVER_USERSPEC);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)

/*=============================================================================
 * function name:  EDDI_TRALedBlinkBegin_Userspec()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() before blinking of LED(s) starts.
 *                 Can be used e.g. to disable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    //implement function here
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkSetMode_Userspec()
 *
 * function:       The EDDI calls this function repeatedly within the service
 *                 EDD_SRV_LED_BLINK() in order to turn on and
 *                 turn off the LED(s) alternately.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex,
                                                                 LSA_UINT16               const  LEDMode )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!
    LSA_UNUSED_ARG(LEDMode); //satisfy lint!

    //implement function here
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name:  EDDI_TRALedBlinkEnd_Userspec()
 *
 * function:       The EDDI calls this function within the service
 *                 EDD_SRV_LED_BLINK() after blinking of LED(s) has finished.
 *                 Can be used e.g. to re-enable the link/activity LED function of
 *                 a PHY if necessary.
 *
 * parameters:     
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex )
{
    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    LSA_UNUSED_ARG(HwPortIndex); //satisfy lint!

    //implement function here
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC

#endif //EDDI_CFG_PHY_TRANSCEIVER_USERSPEC


/*****************************************************************************/
/*  end of file eddi_Tra_Userspec.c                                          */
/*****************************************************************************/

