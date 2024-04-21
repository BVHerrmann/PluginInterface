#ifndef EDDI_BSP_EDD_INI_H      //reinclude-protection
#define EDDI_BSP_EDD_INI_H

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
/*  F i l e               &F: eddi_bsp_edd_ini.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines, internal constants, types, data, macros and prototyping for     */
/*  EDDI.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/ 

#include "eddi_sys.h"
#include "eddi_irte.h"

#if defined (TOOL_CHAIN_MS_WINDOWS)
#pragma warning(disable : 4505 ) /* not referenced local function removed*/
#endif

extern EDDI_HANDLE           g_SyshDDB;

typedef struct _BSP_EDDI_INI_PORT_TYPE
{
    LSA_UINT16               PhyAdr;

} BSP_EDDI_INI_PORT_TYPE;

typedef struct _BSP_EDDI_INI_TYPE
{
    EDDI_MEMORY_MODE_TYPE       MemMode;
    EDD_MAC_ADR_TYPE          * pBSP_own_MacAdress;
    BSP_EDDI_INI_PORT_TYPE      PortParam[EDD_CFG_MAX_PORT_CNT];

} BSP_EDDI_INI_TYPE;

LSA_BOOL EDDI_BSPEddiInit( BSP_EDDI_INI_TYPE * const pBSP_eddi_ini,
                           IRTE_SW_MAKRO_T       * * pIRTE_SW_MAKRO_T );

LSA_BOOL EDDI_BSPEddiDeinit( void );

#if defined (TEST_WITH_EDD_IN_DLL)

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <conio.h>

#define EDDI_MEMSET memset
int B_taste( char   *info );

#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_BSP_EDD_INI_H


/*****************************************************************************/
/*  end of file eddi_bsp_edd_ini.h                                           */
/*****************************************************************************/

