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
/*  F i l e               &F: psi_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI system LSA interface                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3
#define PSI_MODULE_ID       3 /* PSI_MODULE_ID_PSI_SYS */

#include "psi_int.h"
#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

// EDD-Init is done within one FW
static LSA_BOOL psi_edd_init_done = LSA_FALSE;

/*---------------------------------------------------------------------------*/
/* PN-Stack-Init of all components except EDDx                               */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_pn_stack_init( LSA_VOID )
{
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_pn_stack_init()" );

    #if (PSI_CFG_USE_HD_COMP == 1)
	acp_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	clrpc_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	cm_psi_startstop( 1 );
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	dcp_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_GSY == 1) && (PSI_CFG_USE_HD_COMP == 1))
	gsy_psi_startstop(1);
    #endif

	#if ((PSI_CFG_USE_HSA == 1) && (PSI_CFG_USE_LD_COMP == 1))
	hsa_psi_startstop(1);
	#endif

    #if ((PSI_CFG_USE_SOCKAPP == 1) && (PSI_CFG_USE_LD_COMP == 1))
    sockapp_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))
	ioh_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	lldp_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_HD_COMP == 1))
	mrp_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	nare_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	oha_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_DNS == 1) && (PSI_CFG_USE_LD_COMP == 1))
	dns_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_POF == 1) && (PSI_CFG_USE_HD_COMP == 1))
	pof_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	snmpx_psi_startstop(1);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	sock_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_TCIP == 1) && (PSI_CFG_USE_LD_COMP == 1))
	tcip_psi_startstop(1);
    #endif
}

/*---------------------------------------------------------------------------*/
/* PN-Stack-Undo-Init of all components except EDDx                          */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_pn_stack_undo_init( LSA_VOID )
{
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_pn_stack_undo_init()" );

    /* Note: in case of PNTRC component is used for tracing, the component must be undo init last */
    #if (PSI_CFG_USE_HD_COMP == 1)
	acp_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	clrpc_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	cm_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	dcp_psi_startstop(0);
    #endif

    #if ((PSI_CFG_USE_GSY == 1) && (PSI_CFG_USE_HD_COMP == 1))
	gsy_psi_startstop (0);
    #endif

	#if ((PSI_CFG_USE_HSA == 1) && (PSI_CFG_USE_LD_COMP == 1))
	hsa_psi_startstop(0);
	#endif

    #if ((PSI_CFG_USE_SOCKAPP == 1) && (PSI_CFG_USE_LD_COMP == 1))
    sockapp_psi_startstop(0);
    #endif

    #if ((PSI_CFG_USE_IOH == 1) && (PSI_CFG_USE_LD_COMP == 1))
	ioh_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	lldp_psi_startstop(0);
    #endif

    #if ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_HD_COMP == 1))
	mrp_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
	nare_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	oha_psi_startstop(0);
    #endif

    #if ((PSI_CFG_USE_DNS == 1) && (PSI_CFG_USE_LD_COMP == 1))
	dns_psi_startstop(0);
    #endif

    #if ((PSI_CFG_USE_POF == 1) && (PSI_CFG_USE_HD_COMP == 1))
	pof_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	snmpx_psi_startstop(0);
    #endif

    #if (PSI_CFG_USE_LD_COMP == 1)
	sock_psi_startstop(0);
    #endif

    #if ((PSI_CFG_USE_TCIP == 1) && (PSI_CFG_USE_LD_COMP == 1))
	tcip_psi_startstop(0);
    #endif
}

/*---------------------------------------------------------------------------*/
/* EDD-Init                                                                  */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_edd_init( LSA_VOID )
{
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_edd_init()" );

    // only one call within one FW allowed
    PSI_ASSERT(!psi_edd_init_done);

    #if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))
	eddi_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))
	eddp_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
	edds_psi_startstop(1);
    #endif

    #if ((PSI_CFG_USE_EDDT == 1) && (PSI_CFG_USE_HD_COMP == 1))
	eddt_psi_startstop(1);
    #endif

    psi_edd_init_done = LSA_TRUE;
}

/*---------------------------------------------------------------------------*/
/* EDD-Undo-Init                                                             */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_edd_undo_init( LSA_VOID )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_edd_undo_init(): edd_init_done(%u)", psi_edd_init_done );

    // is EDD-Undo-Init already done in this FW?
    if (psi_edd_init_done)
    {
        #if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))
	    eddi_psi_startstop(0);
        #endif

        #if ((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))
	    eddp_psi_startstop(0);
        #endif

        #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
	    edds_psi_startstop(0);
        #endif

        #if ((PSI_CFG_USE_EDDT == 1) && (PSI_CFG_USE_HD_COMP == 1))
	    eddt_psi_startstop(0);
        #endif
    }

    psi_edd_init_done = LSA_FALSE;
}

/*---------------------------------------------------------------------------*/
/* system startup initialization of PSI                                      */
/*---------------------------------------------------------------------------*/
static LSA_UINT16 psi_enter_exit_id = PSI_LOCK_ID_INVALID;

LSA_UINT16 psi_init( LSA_VOID )
{
	LSA_RESPONSE_TYPE rsp = 0;
	
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_init()" );

	PSI_ALLOC_REENTRANCE_LOCK(&rsp, &psi_enter_exit_id );

#if (PSI_CFG_USE_LD_COMP == 1)
	psi_ld_init();
#endif
#if (PSI_CFG_USE_HD_COMP == 1)
	psi_hd_init();
#endif
#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))
	psi_res_calc_init();
#endif
	psi_mbx_sys_init();

    #if (PSI_CFG_USE_HIF == 1)
	hif_psi_startstop(1);
    #endif

    return (LSA_RET_OK);
}

/*---------------------------------------------------------------------------*/
/* system shutdown deinitialization of PSI                                   */
/*---------------------------------------------------------------------------*/
LSA_UINT16 psi_undo_init( LSA_VOID )
{
	LSA_RESPONSE_TYPE rsp = 0;

	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_undo_init()" );

	/* First quit the Threads to finish any open processes */
	psi_mbx_sys_undo_init();

    #if (PSI_CFG_USE_HIF == 1)
	hif_psi_startstop(0);
    #endif

#if (PSI_CFG_USE_HD_COMP == 1)
    psi_hd_undo_init();
#endif
#if (PSI_CFG_USE_LD_COMP == 1)
	psi_ld_undo_init();
#endif
#if ((PSI_CFG_USE_LD_COMP == 1) && (PSI_CFG_USE_HD_COMP == 1))
	psi_res_calc_undo_init();
#endif

	PSI_FREE_REENTRANCE_LOCK(&rsp, psi_enter_exit_id );

	return (LSA_RET_OK);
}

/*---------------------------------------------------------------------------*/
/*  FATAL Handling                                                           */
/*---------------------------------------------------------------------------*/
static LSA_FATAL_ERROR_TYPE     psi_glob_fatal_error;

LSA_VOID psi_fatal_error(
	LSA_UINT16         comp_id,
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32         error_code_0,
	LSA_UINT32         error_code_1,
	LSA_UINT32         error_code_2,
	LSA_UINT32         error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data )
{
	psi_glob_fatal_error.lsa_component_id  = comp_id;
	psi_glob_fatal_error.module_id         = module_id;
	psi_glob_fatal_error.line              = line;

	psi_glob_fatal_error.error_code[0]     = error_code_0;
	psi_glob_fatal_error.error_code[1]     = error_code_1;
	psi_glob_fatal_error.error_code[2]     = error_code_2;
	psi_glob_fatal_error.error_code[3]     = error_code_3;

	psi_glob_fatal_error.error_data_length = error_data_len;
	psi_glob_fatal_error.error_data_ptr    = error_data;

	PSI_FATAL_ERROR( "PSI", (LSA_UINT16)(sizeof(psi_glob_fatal_error)), &psi_glob_fatal_error);
}

/*---------------------------------------------------------------------------*/
/*  Alloc / Free memory                                                      */
/*---------------------------------------------------------------------------*/
LSA_VOID_PTR_TYPE psi_alloc_local_mem(
    LSA_UINT32 length )
{
	LSA_USER_ID_TYPE    dummy_id;
	LSA_VOID_PTR_TYPE   mem_ptr = LSA_NULL;

    PSI_INIT_USER_ID_UNION(dummy_id);

    PSI_ALLOC_LOCAL_MEM( &mem_ptr, dummy_id, length, 0, LSA_COMP_ID_PSI, PSI_MTYPE_LOCAL_MEM );

	if (psi_is_null(mem_ptr))
	{
		PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_alloc_local_mem(): failed, mem_ptr(0x%08x), length(%u)",
			mem_ptr, length );
		PSI_FATAL(0);
	}

	return (mem_ptr);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_free_local_mem(
    LSA_VOID_PTR_TYPE mem_ptr )
{
	LSA_UINT16 ret_val;

	PSI_ASSERT(mem_ptr != LSA_NULL);

	PSI_FREE_LOCAL_MEM( &ret_val, mem_ptr, 0, LSA_COMP_ID_PSI, PSI_MTYPE_LOCAL_MEM );

	if (ret_val != LSA_RET_OK)
	{
		PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, "psi_free_local_mem(): failed, mem_ptr(0x%08x), ret_val(%u/%#x)",
            mem_ptr, ret_val, ret_val );
		PSI_FATAL(0);
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
LSA_BOOL psi_get_mrp_default_rports(
	PSI_HD_INPUT_PTR_TYPE   hd_input_ptr,
	LSA_UINT8 *             rport1_ptr,
	LSA_UINT8 *             rport2_ptr )
{
	LSA_UINT16      idx;
	LSA_UINT8       port[30] = {0};
	LSA_UINT16      found = 0;
	LSA_BOOL        result = LSA_FALSE;

	PSI_ASSERT(hd_input_ptr != LSA_NULL);

	// lookup in HW port entries for user MRP ring ports matching this IF
	for ( idx = 0; idx < hd_input_ptr->nr_of_ports; idx++ )
	{
		if (hd_input_ptr->hw_port[idx].mrp_ringport_type == PSI_RING_PORT_DEFAULT)
		{
			port[found] = (LSA_UINT8)hd_input_ptr->hw_port[idx].user_nr;
			found++;
		}
	}

    // MRP ringports avaliable ?
	if (found == 0)
	{
		*rport1_ptr = 0;  // No MRP port
		*rport2_ptr = 0;  // No MRP port

		result = LSA_FALSE;
	}
	else
	{
        // check valid setting for default ringports
		PSI_ASSERT(found == 2);

		*rport1_ptr = port[0];
		*rport2_ptr = port[1];

		result = LSA_TRUE;
	}

	return (result);

    //lint --e(818) Pointer parameter 'hd_input_ptr' could be declared as pointing to const - we are unable to change the API
}

/*---------------------------------------------------------------------------*/
PSI_HD_PORT_INPUT_PTR_TYPE psi_get_hw_port_entry_for_user_port(
	LSA_UINT16              usr_port_nr,
	PSI_HD_INPUT_PTR_TYPE   hd_input_ptr )
{
	LSA_UINT16                  idx;
	PSI_HD_PORT_INPUT_PTR_TYPE  entry_ptr = LSA_NULL;

	PSI_ASSERT(hd_input_ptr != LSA_NULL);

	// lookup in HW port entries for matching entry
	for ( idx = 0; idx < hd_input_ptr->nr_of_ports; idx++ )
	{
        // HW port entry matches ?
		if (hd_input_ptr->hw_port[idx].user_nr == usr_port_nr)
		{
			entry_ptr = &hd_input_ptr->hw_port[idx];
			break;
		}
	}

	return (entry_ptr);
}

/*------------------------------------------------------------------------------
//	Pointer-Test, used in module internal macros: is_null() and is_not_null()
//----------------------------------------------------------------------------*/
void * PSI_TEST_POINTER( void const * ptr )
{
    if (ptr != LSA_NULL)
    { /* especially 0xCCCCCCCC */
        //lint --e(774) Boolean within 'if' always evaluates to False - check the input parameter 'ptr'
        PSI_ASSERT( ptr != 0 ); /* 0 is never ok if not LSA_NULL */
        //PSI_ASSERT( (unsigned long)ptr < 0x80000000L ); /* at least true for NT */
	}
	return (void *)ptr;
}

void * PSI_TEST_ALIGN2( void const * ptr )
{
	ptr = PSI_TEST_POINTER(ptr);
	PSI_ASSERT( ((unsigned long)ptr & 0x1) == 0 );
	return (void *)ptr;
}

void * PSI_TEST_ALIGN4( void const * ptr )
{
	ptr = PSI_TEST_POINTER(ptr);
	PSI_ASSERT( ((unsigned long)ptr & 0x3) == 0 );
	return (void *)ptr;
}

#ifndef PSI_SWAP_U16
LSA_VOID PSI_SWAP_U16(
	LSA_UINT16 * short_ptr ) /* correct alignment! */
{
	LSA_UINT16 temp;
	(void)PSI_TEST_ALIGN2(short_ptr);
	temp = *short_ptr;
    //lint --e(734) Loss of precision (assignment) (24 bits to 16 bits) - the 'temp' must be 16 bits and it is not possibly to mask it because of other lint warnings
    *short_ptr = (temp >> 8) | (temp << 8);
}
#else
#error "by design a function"
#endif

#ifndef PSI_SWAP_U32
LSA_VOID PSI_SWAP_U32(
	LSA_UINT32 * long_ptr ) /* correct alignment! */
{
	LSA_UINT32 temp;
	(void)PSI_TEST_ALIGN4(long_ptr);
	temp = *long_ptr;
	*long_ptr = (temp >> 24)
		| ((temp >> 8) & 0x0000ff00)
		| ((temp << 8) & 0x00ff0000)
		| (temp << 24);
}
#else
#error "by design a function"
#endif

#ifndef PSI_SWAP_U64
LSA_VOID PSI_SWAP_U64( LSA_UINT64 * long_long_ptr )
{
    LSA_UINT64 temp = *long_long_ptr;

    *long_long_ptr = (((LSA_UINT64)(temp) << 56) | 
    (((LSA_UINT64)(temp) << 40) & 0xff000000000000ULL) | 
    (((LSA_UINT64)(temp) << 24) & 0xff0000000000ULL) | 
    (((LSA_UINT64)(temp) << 8)  & 0xff00000000ULL) | 
    (((LSA_UINT64)(temp) >> 8)  & 0xff000000ULL) | 
    (((LSA_UINT64)(temp) >> 24) & 0xff0000ULL) | 
    (((LSA_UINT64)(temp) >> 40) & 0xff00ULL) | 
    ((LSA_UINT64)(temp)  >> 56));
}
#else
#error "by design a function"
#endif

/*=============================================================================
 * function name:  PSI_HTON16 ... host to network byte order, short integer
 *                 PSI_HTON32 ... host to network byte order, long  integer
 *                 PSI_NTOH16 ... network to host byte order, short integer
 *                 PSI_NTOH32 ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 *===========================================================================*/
#ifndef PSI_HTON16
LSA_UINT16 PSI_HTON16(
	LSA_UINT16 host_short )
{
    #ifdef LSA_HOST_ENDIANESS_LITTLE  
	PSI_SWAP_U16(&host_short);
    #endif

	return( host_short );
}
#else
#error "by design a function"
#endif

#ifndef PSI_HTON32
LSA_UINT32 PSI_HTON32(
	LSA_UINT32 host_long )
{
    #ifdef LSA_HOST_ENDIANESS_LITTLE  
	PSI_SWAP_U32(&host_long);
    #endif

	return( host_long );
}
#else
#error "by design a function"
#endif

#ifndef PSI_HTON64
LSA_UINT64 PSI_HTON64(
	LSA_UINT64 host_long_long )
{
    #ifdef LSA_HOST_ENDIANESS_LITTLE  
	PSI_SWAP_U64(&host_long_long);
    #endif

	return( host_long_long );
}
#else
#error "by design a function"
#endif

#ifndef PSI_NTOH16
LSA_UINT16 PSI_NTOH16(
	LSA_UINT16 network_short )
{
    #ifdef LSA_HOST_ENDIANESS_LITTLE  
	PSI_SWAP_U16(&network_short);
    #endif

	return( network_short);
}
#else
#error "by design a function"
#endif

#ifndef PSI_NTOH32
LSA_UINT32 PSI_NTOH32(
	LSA_UINT32 network_long )
{
    #ifdef LSA_HOST_ENDIANESS_LITTLE  
	PSI_SWAP_U32(&network_long);
    #endif 

	return( network_long );
}
#else
#error "by design a function"
#endif

#ifndef PSI_NTOH64
LSA_UINT64 PSI_NTOH64(
	LSA_UINT64 network_long_long )
{
    #ifdef LSA_HOST_ENDIANESS_LITTLE  
    PSI_SWAP_U64(&network_long_long);
    #endif

	return( network_long_long);
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/
/* PSI ENTER / EXIT                                                           */
/*----------------------------------------------------------------------------*/
#ifndef PSI_ENTER
LSA_VOID PSI_ENTER( LSA_VOID )
{
    PSI_ENTER_REENTRANCE_LOCK( psi_enter_exit_id );
}
#else
#error "by design a function"
#endif

#ifndef PSI_EXIT
LSA_VOID PSI_EXIT( LSA_VOID )
{
    PSI_EXIT_REENTRANCE_LOCK( psi_enter_exit_id );
}
#else
#error "by design a function"
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
