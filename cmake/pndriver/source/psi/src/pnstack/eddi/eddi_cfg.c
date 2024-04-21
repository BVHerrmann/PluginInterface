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
/*  F i l e               &F: eddi_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of EDDI using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1500
#define PSI_MODULE_ID       1500 /* PSI_MODULE_ID_EDDI_CFG */

#include "psi_int.h"

#if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))

#include "eddi_mau_types.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*---------------------------------------------------------------------------*/

enum _EDDI_ENTER_EXIT
{ 
    COM,
    REST,
    SYNC,
    SND_CHA0,
    SND_CHB0,
    RCV_CHA0,
    RCV_CHB0,
    IO,
    IO_KRAM,
    SII_CRITICAL,        // only used if SII is activated
    I2C,
    EDDI_CRITICAL,
    ISO_APPL,
    LOCK_MAX
};

static LSA_UINT16 eddi_enter_exit_id[LOCK_MAX];
static LSA_UINT16 eddi_enter_exit_id_critical;

/*=============================================================================*/

void eddi_psi_startstop( int start )
{
    int               idx;
    LSA_RESULT        result;
    LSA_RESPONSE_TYPE rsp;

    if ( start )
    {
        // Alloc the critical section
        for ( idx = 0; idx < LOCK_MAX; idx++ )
        {
            eddi_enter_exit_id[idx] = PSI_LOCK_ID_INVALID;

            if(idx == ISO_APPL || idx == SII_CRITICAL)
            {                
                PSI_ALLOC_REENTRANCE_LOCK_PRIO_PROTECTED( &rsp, &eddi_enter_exit_id[idx] );
                PSI_ASSERT( rsp == LSA_RET_OK );
            }
            else
            {
                PSI_ALLOC_REENTRANCE_LOCK( &rsp, &eddi_enter_exit_id[idx] );
                PSI_ASSERT( rsp == LSA_RET_OK );
            }
        }

        // Alloc critical IR Lock
        PSI_ALLOC_IR_LOCK( &rsp, &eddi_enter_exit_id_critical );
        PSI_ASSERT( rsp == LSA_RET_OK );

        // Initialize EDDI
        result = eddi_init();
        PSI_ASSERT( EDD_STS_OK == result );
    }
    else
    {
        // Undo init EDD
        result = eddi_undo_init();
        PSI_ASSERT( EDD_STS_OK == result );

        // Free the critical section
        for ( idx = 0; idx < LOCK_MAX; idx++ )
        {
            PSI_FREE_REENTRANCE_LOCK( &rsp, eddi_enter_exit_id[idx] );
            PSI_ASSERT( rsp == LSA_RET_OK );

            eddi_enter_exit_id[idx] = PSI_LOCK_ID_INVALID;
        }

        // Free critical IR Lock
        PSI_FREE_IR_LOCK( &rsp, eddi_enter_exit_id_critical );
        PSI_ASSERT( rsp == LSA_RET_OK );

        eddi_enter_exit_id_critical = PSI_LOCK_ID_INVALID;
    }
}

/*=============================================================================*/

#ifndef EDDI_GET_PATH_INFO
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR EDDI_GET_PATH_INFO( 
    LSA_UINT16            EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
    LSA_SYS_PTR_TYPE      EDDI_LOCAL_MEM_ATTR  *  sys_ptr_ptr,
    EDDI_DETAIL_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                             path )
{
    *ret_val_ptr = psi_eddi_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_RELEASE_PATH_INFO
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR EDDI_RELEASE_PATH_INFO(
    LSA_RESULT            EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr,
    EDDI_DETAIL_PTR_TYPE                          detail_ptr )
{
    *ret_val_ptr = psi_eddi_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/

#ifndef EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM(
    EDDI_SYS_HANDLE                             hSysDev,
    EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    EDDI_USERMEMID_TYPE                         UserMemID )
{  
    PSI_EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM(hSysDev, (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, length, (PSI_USR_NRTMEM_ID_FKT_TYPE *)UserMemID);
    //lint --e(818) Pointer parameter 'UserMemID' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_FREE_DEV_SDRAM_ERTEC_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_FREE_DEV_SDRAM_ERTEC_MEM(
    EDDI_SYS_HANDLE                                hSysDev,
    LSA_UINT16               EDDI_LOCAL_MEM_ATTR * ret_val_ptr,
    EDDI_DEV_MEM_PTR_TYPE                          lower_mem_ptr,
    EDDI_USERMEMID_TYPE                            UserMemID )
{
    PSI_EDDI_FREE_DEV_SDRAM_ERTEC_MEM(hSysDev, ret_val_ptr, (LSA_VOID_PTR_TYPE *)lower_mem_ptr, (PSI_USR_NRTMEM_ID_FKT_TYPE *)UserMemID);
    //lint --e(818) Pointer parameter 'UserMemID' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ALLOC_DEV_SHARED_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_ALLOC_DEV_SHARED_MEM(
    EDDI_SYS_HANDLE                             hSysDev,
    EDDI_DEV_MEM_PTR_TYPE EDDI_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    EDDI_USERMEMID_TYPE                         UserMemID )
{
    PSI_EDDI_ALLOC_DEV_SHARED_MEM(hSysDev, (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, length, (PSI_USR_NRTMEM_ID_FKT_TYPE *)UserMemID);
    //lint --e(818) Pointer parameter 'UserMemID' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_FREE_DEV_SHARED_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_FREE_DEV_SHARED_MEM(
    EDDI_SYS_HANDLE             hSysDev,
    LSA_UINT16                  EDDI_LOCAL_MEM_ATTR * ret_val_ptr,
    EDDI_DEV_MEM_PTR_TYPE       lower_mem_ptr,
    EDDI_USERMEMID_TYPE         UserMemID )
{
    PSI_EDDI_FREE_DEV_SHARED_MEM(hSysDev, ret_val_ptr, (LSA_VOID_PTR_TYPE *)lower_mem_ptr, (PSI_USR_NRTMEM_ID_FKT_TYPE *)UserMemID);
    //lint --e(818) Pointer parameter 'UserMemID' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_REQUEST_UPPER_DONE
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_REQUEST_UPPER_DONE(
    EDD_UPPER_CALLBACK_FCT_PTR_TYPE eddi_request_upper_done_ptr,
    EDD_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                sys_ptr )
{
    LSA_BOOL bIsArpRsp = LSA_FALSE;

    EDD_RQB_TYPE* pRqb = (EDD_RQB_TYPE*)upper_rqb_ptr;
    PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
    PSI_ASSERT( pSys != LSA_NULL );

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
        pRqb->psi_cbf     = (PSI_EDD_REQUEST_DONE_CBF_TYPE)eddi_request_upper_done_ptr;
        pRqb->psi_sys_ptr = (LSA_VOID *)pSys;

        psi_request_start(PSI_MBX_ID_NARE, psi_request_nare_ARPResponseFrameIn, upper_rqb_ptr);
    }
    else
    {
        psi_request_done((PSI_REQUEST_FCT)eddi_request_upper_done_ptr, upper_rqb_ptr, sys_ptr); /* No Arp Rsp => forward request to receiver */
    }
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        TIMER-FUNCTIONS                                                      */
/*=============================================================================*/

#ifndef EDDI_ALLOC_TIMER
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_ALLOC_TIMER(
    LSA_RESULT  EDDI_LOCAL_MEM_ATTR        * ret_val_ptr,
    LSA_TIMER_ID_TYPE  EDDI_LOCAL_MEM_ATTR * timer_id_ptr,
    LSA_UINT16                               timer_type,
    LSA_UINT16                               time_base )
{
    if ((time_base == LSA_TIME_BASE_1MS) || (time_base == LSA_TIME_BASE_10MS))
    {
        PSI_ALLOC_TIMER_TGROUP1( ret_val_ptr, timer_id_ptr, timer_type, time_base, eddi_timeout );
    }
    else
    {
        PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, eddi_timeout );
    }
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_START_TIMER
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_START_TIMER(
    LSA_RESULT        EDDI_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_TIMER_ID_TYPE                       timer_id,
    LSA_USER_ID_TYPE                        user_id,
    LSA_UINT16                              time )
{
    PSI_START_TIMER( ret_val_ptr, timer_id, user_id, time  );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_STOP_TIMER
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_STOP_TIMER(
    LSA_RESULT  EDDI_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id )
{
    PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_FREE_TIMER
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_FREE_TIMER(
    LSA_RESULT  EDDI_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id )
{
    PSI_FREE_TIMER( ret_val_ptr, timer_id  );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_MEMSET
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_MEMSET(
    EDDI_LOCAL_MEM_PTR_TYPE pMem,
    LSA_UINT8               Value,
    LSA_UINT32              Length )
{
    PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_MEMCOPY
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_MEMCOPY(
    EDDI_DEV_MEM_PTR_TYPE   pDest,
    EDDI_LOCAL_MEM_PTR_TYPE pSrc,
    LSA_UINT                Size )
{
    PSI_MEMCPY( pDest, pSrc, Size );
    //lint --e(818) Pointer parameter 'pSrc' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_RQB_ERROR
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_RQB_ERROR(
    EDD_UPPER_RQB_PTR_TYPE pRQB )
{
    PSI_RQB_ERROR( LSA_COMP_ID_EDDI, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_FATAL_ERROR
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_FATAL_ERROR(
    LSA_UINT16                length,
    EDDI_FATAL_ERROR_PTR_TYPE error_ptr )
{
    PSI_FATAL_ERROR( "eddi", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifdef EDDI_CFG_DO_EV_TIMER_REST
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_DO_EV_TIMER_REST( EDD_UPPER_RQB_PTR_TYPE  pRQB)
{
    psi_request_local( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/* EDDI locking                                                                */
/*=============================================================================*/

#ifndef EDDI_ENTER_COM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_ENTER_COM( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[COM] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_COM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_EXIT_COM( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[COM] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_REST
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_REST( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[REST] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_REST
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_REST( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[REST] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_SYNC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SYNC( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[SYNC] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_SYNC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SYNC( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[SYNC] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_SND_CHA0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SND_CHA0( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[SND_CHA0] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_SND_CHA0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SND_CHA0 (LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[SND_CHA0] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_SND_CHB0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SND_CHB0( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[SND_CHB0] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_SND_CHB0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SND_CHB0 (LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[SND_CHB0] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_RCV_CHA0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_RCV_CHA0( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[RCV_CHA0] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_RCV_CHA0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_RCV_CHA0 (LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[RCV_CHA0] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_RCV_CHB0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_RCV_CHB0( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[RCV_CHB0] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_RCV_CHB0
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_RCV_CHB0 (LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[RCV_CHB0] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_IO
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_IO( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[IO] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_IO
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_IO( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[IO] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_IO_KRAM
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_IO_KRAM( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[IO_KRAM] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_IO_KRAM
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_IO_KRAM( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[IO_KRAM] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_CRITICAL
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_CRITICAL( LSA_VOID )
{
    PSI_ENTER_IR_LOCK( eddi_enter_exit_id_critical );  // enable IR-Lock
}
#else
#error "by design a function!"
#endif


#ifndef EDDI_EXIT_CRITICAL
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_CRITICAL( LSA_VOID )
{
    PSI_EXIT_IR_LOCK( eddi_enter_exit_id_critical );  // disable IR-Lock
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_I2C
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_I2C( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[I2C] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDI_EXIT_I2C
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_I2C( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[I2C] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_LL_XPLL_SETPLLMODE_OUT
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_XPLL_SETPLLMODE_OUT(
    LSA_RESULT            * result,
    EDDI_SYS_HANDLE         hSysDev,
    LSA_UINT32              pIRTE,
    LSA_UINT32              location,
    LSA_UINT16              pllmode )
{
    PSI_EDDI_LL_XPLL_SETPLLMODE_OUT( result, hSysDev, pIRTE, location, pllmode );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_LL_GET_PHY_PARAMS
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_GET_PHY_PARAMS(
    EDDI_SYS_HANDLE   hSysDev,
    LSA_UINT32        HwPortIndex,
    LSA_UINT8         Speed,
    LSA_UINT8         Mode,
    LSA_UINT32      * pAutonegCapAdvertised,
    LSA_UINT32      * pAutonegMappingCap,
    LSA_UINT16      * pMAUType,
    LSA_UINT8       * pMediaType,
    LSA_UINT8       * pIsPOF,
    LSA_UINT8       * pFXTransceiverType )
{
    // returns the current phy-type and media-type (both normally static) and the MAU-Type (from Speed, Mode and the known PhyType)
    LSA_UINT8               MediaType, IsPOF, PhyType, FXTransceiverType;
    LSA_UINT16              MAUType = EDD_MAUTYPE_UNKNOWN;
    PSI_EDD_SYS_PTR_TYPE    pSys    = (PSI_EDD_SYS_PTR_TYPE)hSysDev;

    PSI_ASSERT( pSys != 0 );
    PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
    PSI_ASSERT( HwPortIndex < EDD_CFG_MAX_PORT_CNT );

    if (PSI_EDD_IS_PORT_PULLED(hSysDev, HwPortIndex))
    {
        // default value for pulled port must be EDD_MEDIATYPE_UNKNOWN
        *pMAUType            = EDD_MAUTYPE_UNKNOWN;
        *pMediaType          = EDD_MEDIATYPE_UNKNOWN;
        *pIsPOF              = EDD_PORT_OPTICALTYPE_ISNONPOF;
        *pFXTransceiverType  = EDD_FX_TRANSCEIVER_UNKNOWN;

        //TODO * pAutonegMappingCap     = ???
        //TODO * pAutonegCapAdvertised  = ???     
    }
    switch (Speed)
    {
        case EDD_LINK_SPEED_10:
        {
            if ( EDD_LINK_MODE_HALF == Mode )
            {
                MAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_10BASETXHD;
            }
            else if ( EDD_LINK_MODE_FULL == Mode )
            {
                MAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_10BASETXFD;
            }
            break;
        }
        case EDD_LINK_SPEED_100:
        {
            if ( EDD_LINK_MODE_HALF == Mode )
            {
                MAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASETXHD;
            }
            else if ( EDD_LINK_MODE_FULL == Mode )
            {
                MAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASETXFD;
            }
            break;
        }
        case EDD_LINK_SPEED_1000:
        {
            if ( EDD_LINK_MODE_HALF == Mode )
            {
                MAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASETHD;
            }
            else if ( EDD_LINK_MODE_FULL == Mode )
            {
                MAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASETFD;
            }
            break;
        }
        default: break;
    }

    *pMAUType = MAUType;

    PSI_EDD_GET_MEDIA_TYPE(
        hSysDev, 
        (HwPortIndex+1), /* +1 because here [0..3] */
        PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO, 
        &MediaType, 
        &IsPOF, 
        &PhyType,
        &FXTransceiverType);

    *pFXTransceiverType = FXTransceiverType;

    switch (MediaType)
    {
        case PSI_MEDIA_TYPE_FIBER: // Fiber port?
        {
            if (IsPOF)
            {
                //********************************************
                //***** Settings for a POF port (Example) ****
                //********************************************
                if ((EDD_LINK_SPEED_100 == Speed) && (EDD_LINK_MODE_FULL == Mode)) //parameters plausible?
                {
                    *pMAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASEPXFD;
                }
                else // parameters not plausible
                {
                    *pMAUType = EDD_MAUTYPE_UNKNOWN;
                }

                *pAutonegCapAdvertised = EDD_AUTONEG_CAP_NONE;
                *pAutonegMappingCap    = EDD_LINK_100MBIT_FULL;
                *pMediaType            = EDD_MEDIATYPE_FIBER_OPTIC_CABLE;
                *pIsPOF                   = EDD_PORT_OPTICALTYPE_ISPOF;
            }
            else // FX port
            {
                //********************************************
                //***** Settings for a FX port (Example) *****
                //********************************************
                if ((EDD_LINK_SPEED_100 == Speed) && (EDD_LINK_MODE_FULL == Mode)) //parameters plausible?
                {
                    *pMAUType = EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASEFXFD;
                }
                else //parameters not plausible
                {
                    *pMAUType = EDD_MAUTYPE_UNKNOWN;
                }

                *pAutonegCapAdvertised = EDD_AUTONEG_CAP_NONE;
                *pAutonegMappingCap    = EDD_LINK_100MBIT_FULL;
                *pMediaType            = EDD_MEDIATYPE_FIBER_OPTIC_CABLE;
                *pIsPOF                = EDD_PORT_OPTICALTYPE_ISNONPOF;
            }

            break;
        }

        case PSI_MEDIA_TYPE_WIRELESS: // Wireless port?
        {
            #ifdef PSI_MESSAGE
            #pragma PSI_MESSAGE("TODO correct settings for wireless, if known!")
            #endif
            // TODO: check this, wireless connection only behind the PHY??
            *pAutonegCapAdvertised = EDD_AUTONEG_CAP_10BASET + EDD_AUTONEG_CAP_10BASETFD + EDD_AUTONEG_CAP_100BASETX + EDD_AUTONEG_CAP_100BASETXFD;
            *pAutonegMappingCap    = EDD_LINK_AUTONEG;
            *pMediaType            = EDD_MEDIATYPE_RADIO_COMMUNICATION;
            *pIsPOF                   = EDD_PORT_OPTICALTYPE_ISNONPOF;
            break;
        }

        case PSI_MEDIA_TYPE_COPPER: // electrical port?
        {
            //********************************************
            //*** Settings for a COPPER port (Example) ***
            //********************************************
            // as this is an example only, the MediaType is set to copper (0x01)
            // AutoNegCapAdvertised is set to the typical supported modes of our ethernet-plcs and CPs
            *pAutonegCapAdvertised = EDD_AUTONEG_CAP_10BASET + EDD_AUTONEG_CAP_10BASETFD + EDD_AUTONEG_CAP_100BASETX + EDD_AUTONEG_CAP_100BASETXFD;
            *pAutonegMappingCap    = EDD_LINK_AUTONEG;
            *pMediaType            = EDD_MEDIATYPE_COPPER_CABLE;
            *pIsPOF                   = EDD_PORT_OPTICALTYPE_ISNONPOF;
            break;
        }

        default:
        {
            // invalid MediaType
            *pMediaType = EDD_MEDIATYPE_UNKNOWN;
            PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "EDDI_LL_GET_PHY_PARAMS(): invalid MediaType(0x%X)", MediaType);
            PSI_FATAL_COMP( LSA_COMP_ID_EDDI, PSI_MODULE_ID, 0 );
        }
        break;
    }

    #if (PSI_DEBUG == 0)
    // suppress compiler warnings
    LSA_UNUSED_ARG(pSys);
    #endif
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_LL_CHECK_PHY_PARAMS
LSA_RESULT  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_CHECK_PHY_PARAMS(
    EDDI_SYS_HANDLE   hSysDev,
    LSA_UINT32        HwPortIndex,
    LSA_UINT32        AutonegCapAdvertised,
    LSA_UINT32        AutonegMappingCap,
    LSA_UINT16        MAUType,
    LSA_UINT8       * pSpeed,
    LSA_UINT8       * pMode )
{
    // checks MAUType against AutonegCapAdvertised and returns LinkSpeed and LinkMode
    LSA_UINT8            LinkSpeed                = EDD_LINK_UNKNOWN;
    LSA_UINT8            LinkMode                 = EDD_LINK_UNKNOWN;
    PSI_EDD_SYS_PTR_TYPE pSys                     = (PSI_EDD_SYS_PTR_TYPE)hSysDev;
    LSA_BOOL             bOpticalPortNeeded       = LSA_FALSE;
    LSA_RESULT           Result                   = EDD_STS_OK;
    LSA_UINT8            MediaType, IsPOF, PhyType, FXTransceiverType;
    LSA_BOOL             bPsiHdIsOpticalPort;

    PSI_ASSERT( pSys );
    PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
    PSI_ASSERT( HwPortIndex < EDD_CFG_MAX_PORT_CNT );

    LSA_UNUSED_ARG( AutonegMappingCap );
    LSA_UNUSED_ARG( AutonegCapAdvertised );

    switch (MAUType)
    {
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASEFXFD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASELX10:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASEPXFD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_100;
            LinkMode                  = EDD_LINK_MODE_FULL;
            bOpticalPortNeeded        = LSA_TRUE;
            break;
        }
        
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASEFXHD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_100;
            LinkMode                  = EDD_LINK_MODE_HALF;
            bOpticalPortNeeded        = LSA_TRUE;
            break;
        }
        
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_10BASETXHD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_10BASEFLHD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_10;
            LinkMode                  = EDD_LINK_MODE_HALF;
            break;
        }
        
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_10BASETXFD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_10BASEFLFD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_10;
            LinkMode                  = EDD_LINK_MODE_FULL;
            break;
        }
        
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASETXHD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_100;
            LinkMode                  = EDD_LINK_MODE_HALF;
            break;
        }
        
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_100BASETXFD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_100;
            LinkMode                  = EDD_LINK_MODE_FULL;
            break;
        }
        
        // no GBIT supported!
        #if (0)
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASEXHD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASELXHD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASESXHD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_1000;
            LinkMode                  = EDD_LINK_MODE_HALF;
            break;
        }

        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASEXFD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASELXFD:
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASESXFD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_1000;
            LinkMode                  = EDD_LINK_MODE_FULL;
            break;
        }

        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASETHD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_1000;
            LinkMode                  = EDD_LINK_MODE_HALF;
            break;
        }
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_1000BASETFD:
        {
            LinkSpeed                 = EDD_LINK_SPEED_1000;
            LinkMode                  = EDD_LINK_MODE_FULL;
            break;
        }
        case EDDI_PRM_PDPORT_DATA_MAUTYPE_10GigBASEFX:
        {
            LinkSpeed                 = EDD_LINK_SPEED_10000;
            LinkMode                  = EDD_LINK_MODE_FULL;
            break;
        }
        #endif
        
        default:
        {
            Result = EDD_STS_ERR_PARAM;
            break;
        }
    } //switch (MAUType)
  
    if (PSI_EDD_IS_PORT_PULLED(hSysDev, HwPortIndex))
    {
        // default values for pulled ports must be (refer to EDDI_SYS.DOC)
        // Speed: EDD_LINK_UNKNOWN
        // Mode: EDD_LINK_UNKNOWN
        *pSpeed = EDD_LINK_UNKNOWN;
        *pMode  = EDD_LINK_UNKNOWN;
        Result  = EDD_STS_ERR_PARAM;
    }
    else
    {
        PSI_EDD_GET_MEDIA_TYPE(
            hSysDev, 
            (HwPortIndex+1), /* +1 because here [0..3] */
            PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO, 
            &MediaType, 
            &IsPOF,                 /* not used here */
            &PhyType,               /* not used here */
            &FXTransceiverType);    /* not used here */

        if (MediaType == PSI_MEDIA_TYPE_FIBER)
        {
            bPsiHdIsOpticalPort = LSA_TRUE;
        }
        else
        {
            bPsiHdIsOpticalPort = LSA_FALSE;
        }
        
        if(  ( bPsiHdIsOpticalPort && !bOpticalPortNeeded) 
           ||(!bPsiHdIsOpticalPort &&  bOpticalPortNeeded) ) 
        {
            //Port is plugged, but MAUType does not fit to MediaType of port
            Result = EDD_STS_ERR_PARAM;
        }
    }

    *pSpeed = LinkSpeed;
    *pMode  = LinkMode;

    #if (PSI_DEBUG == 0)
    // suppress compiler warnings
    LSA_UNUSED_ARG(pSys);
    #endif

    return (Result);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_LL_LED_BLINK_BEGIN
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_LED_BLINK_BEGIN(
    EDDI_SYS_HANDLE  hSysDev,
    LSA_UINT32       HwPortIndex,
    LSA_UINT32       PhyTransceiver )
{
    PSI_EDDI_LL_LED_BLINK_BEGIN( hSysDev, HwPortIndex, PhyTransceiver );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_LL_LED_BLINK_END
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_LED_BLINK_END(
    EDDI_SYS_HANDLE  hSysDev,
    LSA_UINT32       HwPortIndex,
    LSA_UINT32       PhyTransceiver )
{
    PSI_EDDI_LL_LED_BLINK_END( hSysDev, HwPortIndex, PhyTransceiver );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_LL_LED_BLINK_SET_MODE
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_LED_BLINK_SET_MODE(
    EDDI_SYS_HANDLE  hSysDev,
    LSA_UINT32       HwPortIndex,
    LSA_UINT32       PhyTransceiver,
    LSA_UINT16       LEDMode )
{
    PSI_EDDI_LL_LED_BLINK_SET_MODE( hSysDev, HwPortIndex, PhyTransceiver, LEDMode );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA(
    EDDI_SYS_HANDLE     hSysDev,
    LSA_VOID          * const   p,
    LSA_UINT32          const   location )
{
    return ( PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA( hSysDev, p, location ) );
    //lint --e(818) Pointer parameter 'p' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC( 
    EDDI_SYS_HANDLE     hSysDev,
    LSA_VOID          * const   p,
    LSA_UINT32          const   location )
{
    return ( PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC( hSysDev, p, location ) );
    //lint --e(818) Pointer parameter 'p' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC(
    EDDI_SYS_HANDLE     hSysDev,
    LSA_VOID          * const   p,
    LSA_UINT32          const   location )
{
    return ( PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC( hSysDev, p, location ) );
    //lint --e(818) Pointer parameter 'p' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DEV_SHAREDMEM_OFFSET
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_SHAREDMEM_OFFSET( 
    EDDI_SYS_HANDLE     hSysDev,
    LSA_VOID          * const   p,
    LSA_UINT32          const   location )
{
    return ( PSI_EDDI_DEV_SHAREDMEM_OFFSET( hSysDev, p, location ) );
    //lint --e(818) Pointer parameter 'p' could be declared as pointing to const - we are unable to change the API
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL
LSA_UINT32  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(
    EDDI_SYS_HANDLE             hSysDev,
    LSA_UINT32                  const  p,
    LSA_UINT32                  const  location )
{
    return ( PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL( hSysDev, p, location) );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_WAIT_10_NS
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_WAIT_10_NS( 
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      ticks_10ns )
{
    LSA_UNUSED_ARG( hSysDev );
    PSI_WAIT_NS( (LSA_UINT64)10 * ticks_10ns );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SIGNAL_SENDCLOCK_CHANGE( 
    EDDI_SYS_HANDLE hSysDev,
    LSA_UINT32      CycleBaseFactor,
    LSA_UINT8       Mode )
{
    PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE( hSysDev, CycleBaseFactor, Mode );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#ifndef EDDI_DO_EV_INTERRUPT_PRIO1_AUX
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO1_AUX( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    psi_request_local( PSI_MBX_ID_EDDI_AUX, (PSI_REQUEST_FCT)eddi_request, pRQB );
}
#else
#error "by design a function!"
#endif
#endif

/*=============================================================================*/

#ifndef EDDI_DO_EV_INTERRUPT_PRIO2_ORG
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO2_ORG( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    psi_request_local( PSI_MBX_ID_EDDI_NRT_ORG, (PSI_REQUEST_FCT)eddi_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DO_EV_INTERRUPT_PRIO3_REST
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO3_REST( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    psi_request_local( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW( EDD_UPPER_RQB_PTR_TYPE  pRQB )
{
    psi_request_local( PSI_MBX_ID_EDDI_NRT_LOW, (PSI_REQUEST_FCT)eddi_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_SII_USER_INTERRUPT_HANDLING_STARTED
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SII_USER_INTERRUPT_HANDLING_STARTED( EDDI_HANDLE  hDDB )
{
    PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED( (PSI_EDD_HDDB)hDDB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED( EDDI_HANDLE  hDDB )
{
    LSA_UNUSED_ARG( hDDB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_ENTER_SII_CRITICAL
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_SII_CRITICAL( LSA_VOID )
{
    #if defined EDDI_CFG_SII_POLLING_MODE // no real interrupts enabled
    PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[SII_CRITICAL] );  // enable ThreadLock
    #else
        #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK // real interrupts but everything is done in user mode
        PSI_ENTER_REENTRANCE_LOCK( eddi_enter_exit_id[SII_CRITICAL] );  // enable ThreadLock
        #else
        PSI_ENTER_IR_LOCK( eddi_enter_exit_id_critical );  // enable IR-Lock
        #endif
    #endif
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDI_EXIT_SII_CRITICAL
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_SII_CRITICAL( LSA_VOID )
{
    #if defined EDDI_CFG_SII_POLLING_MODE // no real interrupts enabled
    PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[SII_CRITICAL] );  // disable ThreadLock
    #else
        #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK // real interrupts but everything is done in user mode
        PSI_EXIT_REENTRANCE_LOCK( eddi_enter_exit_id[SII_CRITICAL] );  // disable ThreadLock
        #else
        PSI_EXIT_IR_LOCK( eddi_enter_exit_id_critical );  // disable IR-Lock
        #endif
    #endif
}
#else
#error "by design a function!"
#endif

/*============== these macros are only needed, when IOH is used ===============*/
#if (defined(PSI_CFG_USE_IOH) && (PSI_CFG_USE_IOH == 1))
#if ( defined(EDDI_CFG_APPLSYNC_SHARED) || defined(EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED) || defined(EDDI_CFG_APPLSYNC_SEPARATE) )
#ifndef EDDI_ENTER_APPLSYNC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_APPLSYNC(LSA_VOID)
{
    PSI_ENTER_REENTRANCE_LOCK(eddi_enter_exit_id[ISO_APPL]);
}
#else
#error "by design a function!"
#endif
#endif

/*=============================================================================*/

#if ( defined(EDDI_CFG_APPLSYNC_SHARED) || defined(EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED) || defined(EDDI_CFG_APPLSYNC_SEPARATE) )
#ifndef EDDI_EXIT_APPLSYNC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_APPLSYNC(LSA_VOID)
{
    PSI_EXIT_REENTRANCE_LOCK(eddi_enter_exit_id[ISO_APPL]);
}
#else
#error "by design a function!"
#endif
#endif
#endif //(defined(PSI_CFG_USE_IOH) && (PSI_CFG_USE_IOH == 1))

/*----------------------------------------------------------------------------*/

#ifndef EDDI_I2C_SCL_LOW_HIGHZ
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SCL_LOW_HIGHZ( 
    EDDI_SYS_HANDLE  const  hSysDev,
    LSA_UINT8        const  Level )
{
    PSI_EDDI_I2C_SCL_LOW_HIGHZ( hSysDev, Level );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef EDDI_I2C_SDA_LOW_HIGHZ
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SDA_LOW_HIGHZ( 
    EDDI_SYS_HANDLE  const  hSysDev,
    LSA_UINT8        const  Level )
{
    PSI_EDDI_I2C_SDA_LOW_HIGHZ( hSysDev, Level );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef EDDI_I2C_SDA_READ
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SDA_READ( 
    EDDI_SYS_HANDLE                    const  hSysDev,
    LSA_UINT8         EDDI_LOCAL_MEM_ATTR  *  const  value_ptr )
{
    PSI_EDDI_I2C_SDA_READ( hSysDev, value_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef EDDI_I2C_SELECT
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_I2C_SELECT( 
    LSA_UINT8         EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
    EDDI_SYS_HANDLE                   const  hSysDev,
    LSA_UINT16                        const  PortId,
    LSA_UINT16                        const  I2CMuxSelect )
{
    PSI_EDDI_I2C_SELECT( ret_val_ptr, hSysDev, PortId, I2CMuxSelect );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef EDDI_LL_I2C_WRITE_OFFSET_SOC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_I2C_WRITE_OFFSET_SOC(
    LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
    EDDI_SYS_HANDLE                     const  hSysDev,
    LSA_UINT8                           const  I2CDevAddr,
    LSA_UINT8                           const  I2COffsetCnt,
    LSA_UINT8                           const  I2COffset1,
    LSA_UINT8                           const  I2COffset2,
    LSA_UINT32                          const  Size,
    LSA_UINT8  EDD_UPPER_MEM_ATTR    *  const  pBuf )
{
    PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC( ret_val_ptr, hSysDev, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef EDDI_LL_I2C_READ_OFFSET_SOC
LSA_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_LL_I2C_READ_OFFSET_SOC( 
    LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
    EDDI_SYS_HANDLE                     const  hSysDev,
    LSA_UINT8                           const  I2CDevAddr,
    LSA_UINT8                           const  I2COffsetCnt,
    LSA_UINT8                           const  I2COffset1,
    LSA_UINT8                           const  I2COffset2,
    LSA_UINT32                          const  Size,
    LSA_UINT8  EDD_UPPER_MEM_ATTR    *  const  pBuf )
{
    PSI_EDDI_LL_I2C_READ_OFFSET_SOC( ret_val_ptr, hSysDev, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef EDDI_NRT_SEND_HOOK
LSA_VOID  EDDI_NRT_SEND_HOOK( EDDI_SYS_HANDLE        hSysDev,
                              EDD_UPPER_RQB_PTR_TYPE pRQB )
{
    PSI_EDD_NRT_SEND_HOOK( hSysDev, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )

#ifndef EDDI_SYNC_CACHE_NRT_RECEIVE
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_SYNC_CACHE_NRT_RECEIVE( EDD_UPPER_MEM_U8_PTR_TYPE    pBuffer,
                                                                LSA_UINT32                   Length )
{
    LSA_UNUSED_ARG( pBuffer );
    LSA_UNUSED_ARG( Length );
}
#else
#error "by design a function!"
#endif


/*----------------------------------------------------------------------------*/

#ifndef EDDI_SYNC_CACHE_NRT_RECEIVE_JIT
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_SYNC_CACHE_NRT_RECEIVE_JIT( EDD_UPPER_MEM_U8_PTR_TYPE    pBuffer,
                                                                    LSA_UINT32                   Length )
{
    PSI_CACHE_INV( pBuffer, Length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/


#ifndef EDDI_SYNC_CACHE_NRT_SEND
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_SYNC_CACHE_NRT_SEND(    EDD_UPPER_MEM_U8_PTR_TYPE    pBuffer,
                                                                LSA_UINT32                   Length)
{
    PSI_CACHE_SYNC( pBuffer, Length );
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_NRT_CACHE_SYNC


/*----------------------------------------------------------------------------*/

#ifndef EDDI_DETECTED_RECEIVE_LIMIT
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR EDDI_DETECTED_RECEIVE_LIMIT( const EDDI_HANDLE const hDDB,
                                                               EDDI_SYS_HANDLE         hSysDev,
                                                               LSA_UINT32              NRTChannel )
{
    LSA_UNUSED_ARG( hDDB );
    LSA_UNUSED_ARG( hSysDev );
    LSA_UNUSED_ARG( NRTChannel );
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_EDDI  && PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
