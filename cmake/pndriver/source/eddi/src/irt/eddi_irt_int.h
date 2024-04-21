#ifndef EDDI_IRT_INT_H          //reinclude-protection
#define EDDI_IRT_INT_H

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
/*  F i l e               &F: eddi_irt_int.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  IRT-                                                                     */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDD functions uses the following IDs                      */
/*                                                                           */
/*        EDD:   0x00 .. 0x0F                                                */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*        Each component uses the followin module IDs (defined in component) */
/*                                                                           */
/*        NRT:   0x80 .. 0x8F                                                */
/*        SRT:   0x90 .. 0x9F                                                */
/*        IRT:   0xA0 .. 0xAF                                                */
/*        SWI:   0xB0 .. 0xBF                                                */
/*                                                                           */
/* ========================================================================= */

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* IRT handle (channel) management-structures                                */
/*---------------------------------------------------------------------------*/
//typedef EDDI_LOCAL_MEM_PTR_TYPE  EDDI_LOCAL_HDB_COMP_IRT_PTR_TYPE;

/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* IRT-Management-Structure                                                  */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_IRT_MGMT_TYPE
{
    EDDI_CCW_CTRL_HEAD_TYPE   CtrlHeadRcv[EDDI_MAX_IRTE_PORT_CNT];
    EDDI_CCW_CTRL_HEAD_TYPE   CtrlHeadSnd[EDDI_MAX_IRTE_PORT_CNT];

} EDDI_IRT_MGMT_TYPE;

//typedef struct _EDDI_IRT_MGMT_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_IRT_MGMT_PTR_TYPE;
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* IRT_DDB_COMP-Structure                                                    */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_DDB_COMP_IRT_TYPE
{
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LSA_UINT16                         GlobalDestination;
    #endif

    EDDI_SER_IRT_BASE_LIST_PTR_TYPE    pKramIrtBase;

    EDDI_IRT_MGMT_TYPE                 CtrlHeads;    // Contains all CtrlHeaders of each Snd-Port and Rcv-Port

    LSA_UINT32                         DebugRemoveCycleTime;  // for FCW-ADD / Remove.

} EDDI_DDB_COMP_IRT_TYPE;

typedef struct _EDDI_DDB_COMP_IRT_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_COMP_IRT_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IRT_INT_H


/*****************************************************************************/
/*  end of file eddi_irt_int.h                                               */
/*****************************************************************************/
