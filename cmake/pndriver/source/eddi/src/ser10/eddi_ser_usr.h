#ifndef EDDI_SER_USR_H          //reinclude-protection
#define EDDI_SER_USR_H

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
/*  F i l e               &F: eddi_ser_usr.h                            :F&  */
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
/*  01.03.04    ZR    new structure for SERIniProviderFcw/SERIniProviderFcw  */
/*                                                                           */
/*****************************************************************************/

/*========================================================================= */
/*                                                                          */
/* defines                                                                  */
/*                                                                          */
/*========================================================================= */
/*MC-Tabelle*/
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define g_MC_MAX_Table_Number  256
#define g_MC_Table_Length_MAC  16
#define g_MC_Table_Index_FREE  0xFFFF
#define g_MC_Table_Ptr_FREE    0x1FF
#endif

/*========================================================================= */
/*                                                                          */
/* Typdefinitions                                                           */
/*                                                                          */
/*========================================================================= */

/*--------------------------------------------------------------------------*/
/* EDDI_SERSetup-Struct                                                     */
/*--------------------------------------------------------------------------*/
typedef struct _SER_TOP_DMACW_ADR_TYPE
{
    LSA_UINT32   Tx;
    LSA_UINT32   Rx;

} SER_TOP_DMACW_ADR_TYPE;

typedef struct _SER_VLAN_TYPE
{
    LSA_UINT16                    Table_Length;  // Size of VLAN addresstable
    LSA_UINT16                    LFSR_Mask;     // Activated feedback paths of the LFSR of the der VLAN addresstable
    LSA_UINT16                    Max_CntLink;   // Max. nr of links
    LSA_UINT16                    MacSearchRange; // maximum search range of free entry in table
    LSA_BOOL                      TagMode;

} SER_VLAN_TYPE;

typedef struct _SER_HANDLE_TYPE
{
    LSA_UINT32                    AlterAddressTableWait;  //REV6
    //SWI
    EDDI_SER_NRT_HEAD_PTR_TYPE    pCTRLDevBaseAddr;
    LSA_UINT16                    NRT_FCW_Count;
    LSA_UINT16                    NRT_DB_Count;

    LSA_UINT16                    NRT_FCW_Limit_Up;
    LSA_UINT16                    NRT_FCW_Limit_Down;

    LSA_UINT16                    NRT_DB_Limit_Up;
    LSA_UINT16                    NRT_DB_Limit_Down;

    LSA_UINT16                    HOL_Limit_Port_Up;
    LSA_UINT16                    HOL_Limit_Port_Down;

    LSA_UINT32                    HOL_Limit_CH_Up;
    LSA_UINT32                    HOL_Limit_CH_Down;

    EDDI_SER_NRT_FREE_CCW_PTR_TYPE pCCWDevBaseAddr;

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    LSA_BOOL                      bMCTableEnabled;
    EDDI_SER_MC_TABLE_DESC_ENTRY_PTR_TYPE   pMC_SubTableDescr;
    EDDI_SER_MC_TABLE_HEADER_ENTRY_PTR_TYPE pDev_MC_Table_Base;       //Position of the MC addresstable (points to header)
    #endif

    LSA_UINT16                    UCMC_Table_Length;    //Size of UCMC addresstable
    LSA_UINT16                    UCMC_LFSR_Mask;       //Activated feedback paths of the LFSR of the der UCMC addresstable
    LSA_UINT16                    UCMC_Table_Range;     //Size of neighbourhood to search
    EDDI_SER_UCMC_PTR_TYPE        pDev_UCMC_Table_Base; //Position of the UCMC addresstable
    LSA_UINT32                    UCMC_Table_KRAM_Size;

    EDDI_SER_STATS_PTR_TYPE       pDev_StatisticBaseAddr;

    //NRT
    SER_TOP_DMACW_ADR_TYPE        TopAdrDMACW[EDDI_NRT_IF_CNT];

    EDDI_XRT_ADDR_TYPE            xRT; //source-MAC-address for xRT-telegrams

    SER_VLAN_TYPE                 vlan;

} SER_HANDLE_TYPE;

typedef struct _SER_HANDLE_TYPE  EDD_UPPER_MEM_ATTR        *       SER_HANDLE_PTR;
typedef struct _SER_HANDLE_TYPE  EDD_UPPER_MEM_ATTR const  * CONST_SER_HANDLE_PTR;

/*========================================================================= */

/*--------------------------------------------------------------------------*/
/* PHY_SMI-Struct                                                           */
/*--------------------------------------------------------------------------*/
typedef struct _SER_SWI_LINK_TYPE
{
    LSA_BOOL   Changed;
    LSA_UINT8  LinkStatus;
    LSA_UINT8  LinkSpeed;
    LSA_UINT8  LinkMode;

} SER_SWI_LINK_TYPE;

typedef struct _SER_SWI_LINK_TYPE EDD_UPPER_MEM_ATTR * SER_SWI_LINK_PTR_TYPE;

/*========================================================================= */

/*--------------------------------------------------------------------------*/
/* IniFcw-Struct                                                            */
/*--------------------------------------------------------------------------*/
typedef struct _SER_IRT_INI_FCW_TYPE
{
    LSA_UINT16               FrameID;
    LSA_UINT16               DataLen;
    LSA_UINT32               Time;

} SER_IRT_INI_FCW_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_USR_H


/*****************************************************************************/
/*  end of file eddi_ser_usr.h                                               */
/*****************************************************************************/
