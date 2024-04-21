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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg_edd.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI EDD HW system adaption                             */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      25408
#define LTRC_ACT_MODUL_ID  25408

#include "eps_sys.h"
#include "eps_cp_hw.h"
#include "eps_trc.h"

#if (((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1)) && (PSI_CFG_USE_HD_COMP == 1))
/*----------------------------------------------------------------------------*/

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

LSA_VOID PSI_EDD_NRT_SEND_HOOK(
	PSI_SYS_HANDLE    hSysDev,
	LSA_VOID_PTR_TYPE ptr_upper_rqb)
{
	/* ptr_upper_rqb is of type EDD_UPPER_RQB_PTR_TYPE */

	LSA_UNUSED_ARG(hSysDev);
	LSA_UNUSED_ARG(ptr_upper_rqb);

	/* Send hook not used in EPS */
}

/**
 * Implementation of the Out-Macro PSI_EDD_IS_PORT_PULLED for all EDD types.
 * Since the EPS currently only supports compact products (the ports are always plugged),
 * LSA_FALSE is always returned.
 * 
 * @param [in] hSysDev   - PSI handle, used to get the board
 * @param [in] HwPortID  - number of the hardware port
 * 
 * @returns LSA_TRUE   - Port is pulled
 * @returns LSA_FALSE  - Port is plugged
 * 
 * Attention:
 *     The given “HwPortID” is a HW port number from EDD and must be mapped into the User-Port-ID by system adaptation.
 */
LSA_BOOL PSI_EDD_IS_PORT_PULLED(
    PSI_SYS_HANDLE hSysDev,
    LSA_UINT32     HwPortID)
{
    EPS_BOARD_INFO_PTR_TYPE p_board;
    EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    LSA_UNUSED_ARG(HwPortID);

    PSI_ASSERT( pSys != 0 );

    p_board = eps_hw_get_board_store( pSys->hd_nr );
    PSI_ASSERT( p_board != 0 );
    
    //TODO for future releases, EPS will ask the product if the port is plugged or pulled.
    return LSA_FALSE; 
}

/**
 * The implementation of this Out-Macro is used for all EDD types to get 
 * the MediaType of the port referenced by HwPortID.
 * 
 * @param [in]     hSysDev     - PSI handle, used to get the board
 * @param [in]     HwPortID    - number of the hardware port
 * @param [in]     isPofAutoDetection - execute POF port auto detection (1: yes / 0: no)
 * @param [in/out] pMediaType  - Pointer to address for returned MediaType of port
 * @param [in/out] pIsPOF      - Pointer to address for returned POF property of port
 *
 *   pMediaType
 *       Use defines from PSI_PORT_MEDIA_TYPE.
 *
 *   pIsPOF
 *       1: Port is a real POF port
 *       0: Port is an optical port, but not POF. Or it is a copper port.
 *
 *   pPhyType
 *       Use defines from PSI_PORT_PHY_TYPE.
 *
 *   Attention:
 *       The given “HwPortID” is a HW port number from EDD.
 */
LSA_VOID PSI_EDD_GET_MEDIA_TYPE(
    PSI_SYS_HANDLE   hSysDev,
    LSA_UINT32       HwPortID, 
    LSA_UINT8        isPofAutoDetection,
    LSA_UINT8      * pMediaType,
    LSA_UINT8      * pIsPOF,
    LSA_UINT8      * pPhyType,
    LSA_UINT8      * pFXTransceiverType)
{
    LSA_UINT16                  ret_val;
    LSA_UINT8                   MediaType;
    LSA_UINT8                   IsPOF;
    LSA_UINT8                   FXTransceiverType;
    EPS_BOARD_INFO_PTR_TYPE     p_board;
    EPS_SYS_PTR_TYPE            pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    PSI_ASSERT(pMediaType != 0);
    PSI_ASSERT(pIsPOF != EDD_PORT_OPTICALTYPE_ISNONPOF);
    PSI_ASSERT(pPhyType != 0);
    PSI_ASSERT(pFXTransceiverType != 0);
    PSI_ASSERT((HwPortID >= 1) && (HwPortID <= PSI_CFG_MAX_PORT_CNT));

    p_board = eps_hw_get_board_store (pSys->hd_nr);
    PSI_ASSERT(p_board != 0);

    if (isPofAutoDetection == PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_YES)
    {
        if (PSI_MEDIA_TYPE_AUTO == p_board->port_media_type_by_HwPortId[HwPortID])
        {
            // execute POF port auto detection
            psi_pof_port_auto_detection (&ret_val, pSys->hd_nr, HwPortID, &MediaType, &IsPOF, &FXTransceiverType);

            if (LSA_RET_OK == ret_val)
            {
                // update "MediaType" and "IsPOF" in EPS_BOARD_INFO_TYPE
                p_board->port_media_type_by_HwPortId[HwPortID]  = (PSI_PORT_MEDIA_TYPE) MediaType;
                p_board->b_is_pof_port[HwPortID]                = IsPOF;
                p_board->fx_transceiver_type[HwPortID]          = FXTransceiverType;
            }
            else
            {
                EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, "PSI_EDD_GET_MEDIA_TYPE(): invalid ret_val(0x%08x) from psi_pof_port_auto_detection(), hd_nr(%u), HwPortID(%u)",
                    ret_val, pSys->hd_nr, HwPortID );
                PSI_FATAL(0);
            }
        }
    }

    // read values from Board-Info and write for user
    *pMediaType = p_board->port_media_type_by_HwPortId[HwPortID];
    *pIsPOF     = p_board->b_is_pof_port[HwPortID];
    *pPhyType   = p_board->phy_type[HwPortID];
    *pFXTransceiverType = p_board->fx_transceiver_type[HwPortID];
}
/*----------------------------------------------------------------------------*/
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
