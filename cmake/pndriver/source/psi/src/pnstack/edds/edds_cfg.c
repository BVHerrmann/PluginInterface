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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: edds_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of EDDS using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1700
#define PSI_MODULE_ID       1700 /* PSI_MODULE_ID_EDDS_CFG */

#include "psi_int.h"

#if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

static LSA_UINT16 edds_enter_exit_id[PSI_CFG_MAX_IF_CNT + 1]; // index = hd_nr, index 0 = unused

/*=============================================================================*/

void edds_psi_startstop( int start )
{
	LSA_UINT32        idx;
	LSA_RESULT        result;
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		// alloc critical sections
		for ( idx = 1UL; idx <= PSI_CFG_MAX_IF_CNT; idx++ )
		{
			edds_enter_exit_id[idx] = PSI_LOCK_ID_INVALID;

			PSI_ALLOC_REENTRANCE_LOCK( &rsp, &edds_enter_exit_id[idx] );
			PSI_ASSERT( LSA_RET_OK == rsp );
		}

		// init EDDS
		result = edds_init();
		PSI_ASSERT( EDD_STS_OK == result );
	}
	else
	{
		// undo init EDDS
		result = edds_undo_init();
		PSI_ASSERT( EDD_STS_OK == result );

		// free critical sections
		for ( idx = 1UL; idx <= PSI_CFG_MAX_IF_CNT; idx++ )
		{
			PSI_FREE_REENTRANCE_LOCK( &rsp, edds_enter_exit_id[idx] );
			PSI_ASSERT( LSA_RET_OK == rsp );

			edds_enter_exit_id[idx] = PSI_LOCK_ID_INVALID;
		}
	}
}

/*=============================================================================*/

#ifndef EDDS_GET_PATH_INFO
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_GET_PATH_INFO(
	LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE        EDDS_LOCAL_MEM_ATTR * sys_ptr_ptr,
	EDDS_DETAIL_PTR_TYPE    EDDS_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                             path )
{
	*ret_val_ptr = psi_edds_get_path_info( sys_ptr_ptr, (void**)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_RELEASE_PATH_INFO
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RELEASE_PATH_INFO(
	LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	EDDS_DETAIL_PTR_TYPE    detail_ptr )
{
	*ret_val_ptr = psi_edds_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_UINT32                                   length )
{
    if (length != EDD_FRAME_BUFFER_LENGTH)
    {
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, PSI_MODULE_ID, 0 );
    }

	PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, upper_mem_ptr_ptr, length );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(): ptr(0x%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_RX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_RX_TRANSFER_BUFFER_MEM(): ptr(0x%08x)", upper_mem_ptr );

	PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, ret_val_ptr, upper_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_UINT32                                   length )
{
    if ( length < 60UL )
    {
        PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(): EDDS NRT TX buffer is increased from length(%u) to 60 bytes", length );
        length = 60UL; //set minimum length required by TI AM5728
    }

	PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, upper_mem_ptr_ptr, length );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(): ptr(0x%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_TX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_TX_TRANSFER_BUFFER_MEM(): ptr(0x%08x)", upper_mem_ptr );

	PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, ret_val_ptr, upper_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        DeviceBuffer-FUNCTION                                                */
/*=============================================================================*/

#ifndef EDDS_ALLOC_DEV_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_DEV_BUFFER_MEM(
    EDDS_SYS_HANDLE                              hSysDev,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_UINT32                                   length )
{
	PSI_EDDS_ALLOC_DEV_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, upper_mem_ptr_ptr, length );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_DEV_BUFFER_MEM(): ptr(0x%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_DEV_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_DEV_BUFFER_MEM(
    EDDS_SYS_HANDLE                              hSysDev,
    LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_DEV_BUFFER_MEM(): ptr(0x%08x)", upper_mem_ptr );

	PSI_EDDS_FREE_DEV_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, ret_val_ptr, upper_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        Mau Type decoding                                                    */
/*=============================================================================*/

#ifndef EDDS_CHECK_AND_DECODE_MAUTYPE
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CHECK_AND_DECODE_MAUTYPE(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDDS_SYS_HANDLE                                  hSysDev,
    EDDS_HANDLE                                      hDDB,
    LSA_UINT32                                       PortID,
    LSA_UINT16                                       MAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE                       pSpeedMode )
{
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hDDB );
	LSA_UNUSED_ARG( PortID );

    *ret_val_ptr = LSA_RET_OK;

    switch (MAUType)
    {
        case EDDS_LL_MAUTYPE_100BASETXFD:
        {
           *pSpeedMode = EDD_LINK_100MBIT_FULL;
        }
        break;

        default:
        {
            *ret_val_ptr = LSA_RET_ERR_PARAM;
        }
        break;
    }

    PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_CHECK_AND_DECODE_MAUTYPE(): ret_val(0x%08x), PortID(0x%08x), MAUType(0x%08x), SpeedMode(0x%08x)", *ret_val_ptr, PortID, MAUType, *pSpeedMode);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_GET_PHY_PARAMS
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_GET_PHY_PARAMS(
    EDDS_SYS_HANDLE             hSysDev,
    EDDS_HANDLE                 hDDB,
    LSA_UINT32                  PortID,
    LSA_UINT8                   Speed,
    LSA_UINT8                   Mode,
    EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE  pMediaType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE  pIsPOF )
{
	LSA_UNUSED_ARG( hDDB );
	
	if (PSI_EDD_IS_PORT_PULLED(hSysDev, PortID))
	{
	    // Default value for pulled ports is EDD_MAUTYPE_UNKNOWN, EDD_MEDIATYPE_UNKNOWN and non POF.
	    *pMAUType   = EDD_MAUTYPE_UNKNOWN;
	    *pMediaType = EDD_MEDIATYPE_UNKNOWN;
	    *pIsPOF     = EDD_PORT_OPTICALTYPE_ISNONPOF;
	}
	else
	{
        *pMediaType = EDD_MEDIATYPE_COPPER_CABLE; // Currently the EDDS only supports media type copper.
        *pMAUType   = EDD_MAUTYPE_UNKNOWN;
        *pIsPOF		= EDD_PORT_OPTICALTYPE_ISNONPOF;
    
        switch (Speed)
        {
            case EDD_LINK_SPEED_10:
            {
                if (EDD_LINK_MODE_HALF == Mode)
                {
                    *pMAUType = EDDS_LL_MAUTYPE_10BASETHD;
                }
                else if (EDD_LINK_MODE_FULL == Mode)
                {
                    *pMAUType = EDDS_LL_MAUTYPE_10BASETFD;
                }
            }
            break;

            case EDD_LINK_SPEED_100:
            {
                if (EDD_LINK_MODE_HALF == Mode)
                {
                    *pMAUType = EDDS_LL_MAUTYPE_100BASETXHD;
                }
                else if (EDD_LINK_MODE_FULL == Mode)
                {
                    *pMAUType = EDDS_LL_MAUTYPE_100BASETXFD;
                }
            }
            break;

            case EDD_LINK_SPEED_1000:
            {
                if (EDD_LINK_MODE_HALF == Mode)
                {
                    *pMAUType = EDDS_LL_MAUTYPE_1000BASETHD;
                }
                else if (EDD_LINK_MODE_FULL == Mode)
                {
                    *pMAUType = EDDS_LL_MAUTYPE_1000BASETFD;
                }
            }
            break;

            default:
            {
                // nothing to do and no fatal
            }
            break;
        }
	}
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        Wait-FUNCTION                                                        */
/*=============================================================================*/

#ifndef EDDS_WAIT_US
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_WAIT_US(
    LSA_UINT16 Count )
{
	PSI_WAIT_NS( (LSA_UINT64)1000 * Count );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_REQUEST_UPPER_DONE
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_REQUEST_UPPER_DONE(
	EDD_UPPER_CALLBACK_FCT_PTR_TYPE edds_request_upper_done_ptr,
	EDD_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                sys_ptr )
{
	LSA_BOOL                 bIsArpRsp = LSA_FALSE;
	EDD_RQB_TYPE     * const pRqb      = (EDD_RQB_TYPE *)upper_rqb_ptr;
	PSI_SYS_PTR_TYPE   const pSys      = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

	if ( pSys->check_arp ) // NARE ARP channel ?
	{
		if ( EDD_RQB_GET_SERVICE( upper_rqb_ptr ) == EDD_SRV_NRT_RECV ) // NRT ?
		{
			/* Check if it is an arp response frame and arp sniffing is on */
			bIsArpRsp = psi_is_rcv_arp_rsp( upper_rqb_ptr, pSys->pnio_if_nr );
		}
	}

	if ( bIsArpRsp )
	{
		/* Arp Response => forward it to nare context for arp sniffing, from there it will be forwarded to receiver context */
		/* We need to backup eddx_request_upper_done_ptr, pSys->mbx_id_rsp and pSys->pnio_if_nr for processing in nare context */
		pRqb->psi_cbf     = (PSI_EDD_REQUEST_DONE_CBF_TYPE)edds_request_upper_done_ptr;
		pRqb->psi_sys_ptr = (LSA_VOID *)pSys;

		psi_request_start( PSI_MBX_ID_NARE, psi_request_nare_ARPResponseFrameIn, upper_rqb_ptr );
	}
	else
	{
		psi_request_done ( (PSI_REQUEST_FCT)edds_request_upper_done_ptr, upper_rqb_ptr, sys_ptr ); /* No Arp Rsp => forward request to receiver */
	}
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR32_DEV
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR32_DEV(
	EDDS_SYS_HANDLE                  hSysDev,
	EDD_UPPER_MEM_PTR_TYPE           Ptr,
	LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pAddr )
{
	PSI_EDDS_PTR_TO_ADDR_DEV_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR64_DEV
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_DEV(
	EDDS_SYS_HANDLE                  hSysDev,
	EDD_UPPER_MEM_PTR_TYPE           Ptr,
	LSA_UINT64 EDDS_LOCAL_MEM_ATTR * pAddr )
{
	PSI_EDDS_PTR_TO_ADDR64_DEV_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR32_TX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR32_TX(
	EDDS_SYS_HANDLE                  hSysDev,
	EDD_UPPER_MEM_PTR_TYPE           Ptr,
	LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pAddr )
{
	PSI_EDDS_PTR_TO_ADDR_TX_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR64_TX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_TX(
	EDDS_SYS_HANDLE                  hSysDev,
	EDD_UPPER_MEM_PTR_TYPE           Ptr,
	LSA_UINT64 EDDS_LOCAL_MEM_ATTR * pAddr )
{
	PSI_EDDS_PTR_TO_ADDR64_TX_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR32_RX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR32_RX(
	EDDS_SYS_HANDLE                  hSysDev,
	EDD_UPPER_MEM_PTR_TYPE           Ptr,
	LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pAddr )
{
	PSI_EDDS_PTR_TO_ADDR_RX_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR64_RX
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_RX(
	EDDS_SYS_HANDLE                  hSysDev,
	EDD_UPPER_MEM_PTR_TYPE           Ptr,
	LSA_UINT64 EDDS_LOCAL_MEM_ATTR * pAddr )
{
	PSI_EDDS_PTR_TO_ADDR64_RX_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMSET_LOCAL
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_LOCAL(
	EDDS_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMSET_UPPER
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_UPPER(
	EDDS_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMCOPY_UPPER
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMCOPY_UPPER(
	EDD_UPPER_MEM_PTR_TYPE pDst,
	EDD_UPPER_MEM_PTR_TYPE pSrc,
	LSA_UINT32             Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_GEN_REQUEST_PROCESSED_TRIGGER(
    EDDS_SYS_HANDLE        hSysDev,
    EDD_UPPER_RQB_PTR_TYPE pRQB )
{
    PSI_EDD_SYS_PTR_TYPE const pSys = (PSI_EDD_SYS_PTR_TYPE)hSysDev;
    LSA_UINT16                 mbx_id;

	PSI_ASSERT( pSys );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDS );
	PSI_ASSERT( pSys->hd_nr && (pSys->hd_nr <= PSI_CFG_MAX_IF_CNT) );
	PSI_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == EDD_OPC_SYSTEM );

    mbx_id = pSys->mbx_id_edds_rqb;

    PSI_ASSERT( (mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4) );

	psi_request_local( mbx_id, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_RQB_ERROR
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RQB_ERROR(
	EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_EDDS, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FATAL_ERROR
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FATAL_ERROR(
	LSA_UINT16                length,
	EDDS_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "edds", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
//  Tasks:
//  Org		(Sync)		->	High (Short)
//  Nrt-Low (Com)		->	Middle
//  Rest	(Rest)		->	Low
/*=============================================================================*/

/*===========================================================================*/
/* prio:           RQB */

#ifndef EDDS_DO_NRT_CANCEL_PART2
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_NRT_CANCEL_PART2(
    EDDS_SYS_HANDLE        hSysDev,
    EDD_UPPER_RQB_PTR_TYPE pRQB )
{
    PSI_EDD_SYS_PTR_TYPE const pSys = (PSI_EDD_SYS_PTR_TYPE)hSysDev;
    LSA_UINT16                 mbx_id;

	PSI_ASSERT( pSys );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDS );
	PSI_ASSERT( pSys->hd_nr && (pSys->hd_nr <= PSI_CFG_MAX_IF_CNT) );
	PSI_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == EDD_OPC_SYSTEM );

    mbx_id = pSys->mbx_id_edds_rqb;

    PSI_ASSERT( (mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4) );

	psi_request_local( mbx_id, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  EDDS_ENTER
 *
 * function:       Protect sequence from being interrupted.
 *
 * parameters:     EDDS_SYS_HANDLE
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_ENTER
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ENTER(
    EDDS_SYS_HANDLE hSysDev )
{
    PSI_EDD_SYS_PTR_TYPE const pSys = (PSI_EDD_SYS_PTR_TYPE)hSysDev;

	PSI_ASSERT( pSys );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDS );
	PSI_ASSERT( pSys->hd_nr && (pSys->hd_nr <= PSI_CFG_MAX_IF_CNT) );

	PSI_ENTER_REENTRANCE_LOCK( edds_enter_exit_id[pSys->hd_nr] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  EDDS_EXIT
 *
 * function:       Finishes EDDS_ENTER-Sequence.
 *
 * parameters:     EDDS_SYS_HANDLE
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_EXIT
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_EXIT(
    EDDS_SYS_HANDLE hSysDev )
{
    PSI_EDD_SYS_PTR_TYPE const pSys = (PSI_EDD_SYS_PTR_TYPE)hSysDev;

	PSI_ASSERT( pSys );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDS );
	PSI_ASSERT( pSys->hd_nr && (pSys->hd_nr <= PSI_CFG_MAX_IF_CNT) );

	PSI_EXIT_REENTRANCE_LOCK( edds_enter_exit_id[pSys->hd_nr] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_ADDR_EDDS_TO_USER
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBUFFER_ADDR_EDDS_TO_USER(
    LSA_UINT16                MngmId,
    EDD_UPPER_MEM_U8_PTR_TYPE pAddr )
{
	LSA_UNUSED_ARG( MngmId );

	return pAddr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_MEMCPY
LSA_VOID EDDS_IOBUFFER_MEMCPY(
    LSA_VOID_PTR_TYPE pDst,
    LSA_VOID_PTR_TYPE pSrc,
    LSA_UINT32        Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_FATAL_ERROR
LSA_VOID EDDS_IOBUFFER_FATAL_ERROR(
    LSA_UINT16                length,
    EDDS_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "edds", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_EXCHANGE
EDDS_EXCHANGE_TYPE EDDS_IOBUFFER_EXCHANGE(
    EDDS_EXCHANGE_TYPE volatile * pTarget,
    EDDS_EXCHANGE_TYPE            Value )
{
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_IOBUFFER_EXCHANGE()" );

	return( PSI_EXCHANGE_LONG( pTarget, Value) );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_NRT_SEND_HOOK
LSA_VOID EDDS_NRT_SEND_HOOK(
    EDDS_SYS_HANDLE        hSysDev,
    EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_EDD_NRT_SEND_HOOK( (PSI_SYS_HANDLE)hSysDev, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifndef EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT(
    LSA_UINT16                       timer_id,
    LSA_UINT32                       CycleTime,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pTimerValue,
    LSA_UINT8  EDDS_LOCAL_MEM_ATTR * pRun )
{
    int             RetVal;
	LSA_UINT32      AktTimeUs;
	LSA_UINT32      Delta;
	struct timespec ActualTime;

    LSA_UNUSED_ARG( timer_id );

    RetVal = EPS_POSIX_CLOCK_GETTIME( CLOCK_MONOTONIC, &ActualTime );
    PSI_ASSERT( RetVal == 0 ); // EPS_POSIX_CLOCK_GETTIME returns with 0

	AktTimeUs = ((LSA_UINT64)ActualTime.tv_nsec + ((LSA_UINT64)ActualTime.tv_sec * (LSA_UINT64)1000000000));

	Delta = (LSA_UINT32)(AktTimeUs - *pTimerValue);

	if ( Delta < (CycleTime / 3000 * 2) )
	{
		*pRun = EDDS_CSRT_CYCLE_JITTER_CHECK_DONT_DO_CONS_CHECK;
	}
	else
	{
		*pTimerValue = AktTimeUs;
		*pRun 		 = EDDS_CSRT_CYCLE_JITTER_CHECK_DO_CONS_CHECK;
	}
}
#endif
#endif

/*=============================================================================
 * function name:  EDDS_CSRT_CONSUMER_CYCLE_START
 *
 * function:       Called after the consumer timer is started.
 *                 The timer_id was allocated previously for this consumer
 *                 check cycle and can be used to identify the cycle time.
 *
 *                 This macro will be called  after the consumer timer
 *                 is started. The Systemadaption setup the TimerValue
 *                 with the first timer value.
 *
 * parameters:     LSA_UINT16                              timer_id
 *                 LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifndef EDDS_CSRT_CONSUMER_CYCLE_START
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_START(
    LSA_UINT16                       timer_id,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pTimerValue )
{
    int             RetVal;
	struct timespec ActualTime;

    LSA_UNUSED_ARG( timer_id );

    RetVal = EPS_POSIX_CLOCK_GETTIME( CLOCK_MONOTONIC, &ActualTime );
    PSI_ASSERT( RetVal == 0 ); // EPS_POSIX_CLOCK_GETTIME returns with 0

	*pTimerValue = ActualTime.tv_nsec;
}
#endif
#endif

/*=============================================================================
 * function name:  EDDS_GET_SYSTEM_TIME_NS
 *
 * function:       
 *
 * parameters:     EDDS_SYS_HANDLE hSysDev
 *
 * return value:   LSA_UINT64 
 *===========================================================================*/
#ifndef EDDS_GET_SYSTEM_TIME_NS
LSA_UINT64 EDDS_GET_SYSTEM_TIME_NS(
    EDDS_SYS_HANDLE hSysDev )
{
	LSA_UNUSED_ARG( hSysDev );

    return ( PSI_GET_TICKS_100NS() * (LSA_UINT64)100 );
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  EDDS_NOTIFY_FILL_SETTINGS
 *
 * function:       Notify system adaption for settings of Feed-In
 *                 Load Limitation (FILL).
 *
 *                 FeedInLoadLimitationActive : 0: INACTIVE, 1: ACTIVE
 *                 IO_Configured              : 0: NO, 1: YES
 *
 * parameters:     EDDS_SYS_HANDLE hSysDev
 *                 LSA_UINT8       FeedInLoadLimitationActive
 *                 LSA_UINT8       IO_Configured
 *
 * prio:           RQB context
 *
 * return value:   EDD_RSP      EDD_STS_OK_PENDING - The scheduler will be triggered 
 *                              with the macro EDDS_DO_NOTIFY_SCHEDULER in case of
 *                              an NRT-Snd event. NRT-Send/Rcv RQBs will be finished
 *                              in RQB context.
 *
 *                              EDD_STS_OK - The scheduler will be called immediately 
 *                              from the RQB context in case of an NRT-Snd event. 
 *                              NRT-Send/Rcv RQBs will be finished in Scheduler context.
 *
 *===========================================================================*/
#ifndef EDDS_NOTIFY_FILL_SETTINGS
EDD_RSP EDDS_NOTIFY_FILL_SETTINGS(
    EDDS_SYS_HANDLE hSysDev,
    LSA_UINT8       FeedInLoadLimitationActive,
    LSA_UINT8       IO_Configured )
{
    LSA_BOOL bRetVal;

	PSI_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "EDDS_NOTIFY_FILL_SETTINGS(): FeedInLoadLimitationActive(%u) IO_Configured(%u)", FeedInLoadLimitationActive, IO_Configured);

	bRetVal = PSI_EDDS_NOTIFY_FILL_SETTINGS(hSysDev, ((EDD_SYS_FILL_INACTIVE != FeedInLoadLimitationActive)?LSA_TRUE:LSA_FALSE), ((EDD_SYS_IO_CONFIGURED_OFF != IO_Configured)?LSA_TRUE:LSA_FALSE), LSA_FALSE/*mandatory*/);

	return (bRetVal?EDD_STS_OK_PENDING:EDD_STS_OK);
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  EDDS_DO_NOTIFY_SCHEDULER
 *
 * function:       
 *
 * parameters:     EDDS_SYS_HANDLE 	hSysDev
 *
 * return value:   LSA_VOID 
 *===========================================================================*/
#ifndef EDDS_DO_NOTIFY_SCHEDULER
LSA_VOID EDDS_DO_NOTIFY_SCHEDULER(
    EDDS_SYS_HANDLE hSysDev )
{
	PSI_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "EDDS_DO_NOTIFY_SCHEDULER()");

	PSI_EDDS_DO_NOTIFY_SCHEDULER(hSysDev);
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  EDDS_DO_RETRIGGER_SCHEDULER
 *
 * function:       
 *
 * parameters:     EDDS_SYS_HANDLE  hSysDev
 *
 * return value:   LSA_VOID 
 *===========================================================================*/
#ifndef EDDS_DO_RETRIGGER_SCHEDULER
LSA_VOID EDDS_DO_RETRIGGER_SCHEDULER(
    EDDS_SYS_HANDLE hSysDev )
{
    PSI_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "EDDS_DO_RETRIGGER_SCHEDULER()");

    PSI_EDDS_DO_RETRIGGER_SCHEDULER(hSysDev);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDS && PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
