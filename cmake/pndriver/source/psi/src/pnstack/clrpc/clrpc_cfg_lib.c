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
/*  F i l e               &F: clrpc_cfg_lib.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CLRPC                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1101
#define PSI_MODULE_ID       1101 /* PSI_MODULE_ID_CLRPC_CFG_LIB */

#include "psi_int.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*=============================================================================*/
/* converts a uuid into a string                                               */
/* uuid string: tttttttt-tttt-vttt-rccc-nnnnnnnnnnnn                           */
/*=============================================================================*/

LSA_UINT16 clrpc_uuid_to_string (
	CLRPC_UUID_PTR_TYPE                 uuid,
	CLRPC_COMMON_MEM_U8_PTR_TYPE        uuid_string )
{
    LSA_INT     RetVal_Printf;
    LSA_UINT16  RetVal_Clrpc;

    PSI_ASSERT (!LSA_HOST_PTR_ARE_EQUAL(uuid, LSA_NULL));
	PSI_ASSERT (!LSA_HOST_PTR_ARE_EQUAL(uuid_string, LSA_NULL));

    RetVal_Printf = PSI_SPRINTF( uuid_string, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		                         uuid->time_low, uuid->time_mid, uuid->time_hi_and_version,
		                         uuid->clock_seq_hi_and_reserved, uuid->clock_seq_low,
		                         uuid->node[0], uuid->node[1], uuid->node[2],
		                         uuid->node[3], uuid->node[4], uuid->node[5] );

    if (RetVal_Printf >= 0)
        RetVal_Clrpc = CLRPC_OK;
    else
        RetVal_Clrpc = CLRPC_ERR_PARAM;

    return RetVal_Clrpc;
    //lint --e(818) Pointer parameter 'uuid' could be declared as pointing to const - we are unable to change the API
}


/*=============================================================================*/
/* converts a uuid from string to uuid-format                                  */
/*	uuid string: tttttttt-tttt-vttt-rccc-nnnnnnnnnnnn                          */
/*=============================================================================*/

LSA_UINT16 clrpc_uuid_from_string (
	CLRPC_UUID_PTR_TYPE	                uuid,
	CLRPC_COMMON_MEM_U8_PTR_TYPE        uuid_string )
{
	LSA_UINT16 ret_val;
	LSA_INT  i;
	LSA_UINT32 time_low;
	LSA_INT  time_mid;
	LSA_INT  time_hi_and_version;
	LSA_INT  clock_seq_hi_and_reserved;
	LSA_INT  clock_seq_low;
	LSA_INT  node[6];
	LSA_INT  n;

	if (LSA_HOST_PTR_ARE_EQUAL(uuid_string, LSA_NULL) || *uuid_string == '\0')
	{
        PSI_MEMSET(uuid, 0, sizeof *uuid);
		ret_val = CLRPC_OK;
	}
	/* tttttttt-tttt-cccc-cccc-nnnnnnnnnnnn */
	else if ( PSI_STRLEN(uuid_string) != 36
		|| uuid_string[8] != '-' || uuid_string[13] != '-'
		|| uuid_string[18] != '-' || uuid_string[23] != '-')
	{
		PSI_SYSTEM_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "wrong uuid-fmt(...) strlen(%u)"
			, PSI_STRLEN(uuid_string)
			);
		ret_val = CLRPC_ERR_PARAM;
	}
	else
	{
		i = PSI_SSCANF_UUID( uuid_string, "%8lx-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x%n",
			&time_low,
			&time_mid,
			&time_hi_and_version,
			&clock_seq_hi_and_reserved,
			&clock_seq_low,
			&node[0], &node[1], &node[2], &node[3], &node[4], &node[5],
			&n
			);
		if (i != 11 || n != 36)
		{
			PSI_SYSTEM_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
				, "wrong uuid-fmt(...), sscanf-rc(%u) n(%u)"
				, i, n
				);
			ret_val = CLRPC_ERR_PARAM;
		}
		else
		{
			uuid->time_low                   = (LSA_UINT32) time_low;
			uuid->time_mid                   = (LSA_UINT16) time_mid;
			uuid->time_hi_and_version        = (LSA_UINT16) time_hi_and_version;
			uuid->clock_seq_hi_and_reserved  = (LSA_UINT8) clock_seq_hi_and_reserved;
			uuid->clock_seq_low              = (LSA_UINT8) clock_seq_low;
			uuid->node[0]                    = (LSA_UINT8) node[0];
			uuid->node[1]                    = (LSA_UINT8) node[1];
			uuid->node[2]                    = (LSA_UINT8) node[2];
			uuid->node[3]                    = (LSA_UINT8) node[3];
			uuid->node[4]                    = (LSA_UINT8) node[4];
			uuid->node[5]                    = (LSA_UINT8) node[5];

			ret_val = CLRPC_OK;
		}
	}

	return (ret_val);
    //lint --e(818) Pointer parameter 'uuid_string' could be declared as pointing to const - we are unable to change the API
}

#if (PSI_CFG_USE_LD_COMP == 1)

/*=============================================================================*/
/*  reads the value of a 100ns-counter                                         */
/*=============================================================================*/

#ifndef CLRPC_GET_100NS_COUNTER
LSA_VOID CLRPC_GET_100NS_COUNTER(
    LSA_UINT16  * ret_val_ptr,
    LSA_UINT64  * counter_value_ptr)
{
    PSI_ASSERT(counter_value_ptr != LSA_NULL);

    *counter_value_ptr = PSI_GET_TICKS_100NS();
    *ret_val_ptr = LSA_RET_OK;
}
#endif

/*=============================================================================
 * function name:  CLRPC_SWAP_U16, CLRPC_SWAP_U32 and CLRPC_SWAP_UUID
 *
 * function:       swap "in place"
 *                 between "little" and "big" endian or vice versa
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_SWAP_U16
LSA_VOID CLRPC_SWAP_U16(
	LSA_UINT16 * short_ptr ) /* correct alignment! */
{
	(void)PSI_TEST_ALIGN2(short_ptr);
	PSI_SWAP_U16(short_ptr);
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_SWAP_U32
LSA_VOID CLRPC_SWAP_U32(
	LSA_UINT32 * long_ptr ) /* correct alignment! */
{
	(void)PSI_TEST_ALIGN4(long_ptr);
	PSI_SWAP_U32(long_ptr);
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_SWAP_UUID
LSA_VOID CLRPC_SWAP_UUID(
	CLRPC_UUID_PTR_TYPE uuid_ptr )/* correct alignment! */
{
	PSI_SWAP_U32(&uuid_ptr->time_low);
	PSI_SWAP_U16(&uuid_ptr->time_mid);
	PSI_SWAP_U16(&uuid_ptr->time_hi_and_version);
}
#else
# error "by design a function"
#endif


/*=============================================================================
 * function name:  CLRPC_HTONS ... host to network byte order, short integer
 *                 CLRPC_HTONL ... host to network byte order, long  integer
 *                 CLRPC_NTOHS ... network to host byte order, short integer
 *                 CLRPC_NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_CLRPC_Detailspec.doc
 *
 *===========================================================================*/

#ifndef CLRPC_HTONS
LSA_UINT16 CLRPC_HTONS(
	LSA_UINT16 host_short )
{
    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    //lint --e(835) A zero has been given as [left/right] argument to operator '<<'
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_BIG )
	{
		PSI_SWAP_U16(&host_short);
	}

	return( host_short );
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_HTONL
LSA_UINT32 CLRPC_HTONL(
	LSA_UINT32 host_long
) {
    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    //lint --e(835) A zero has been given as [left/right] argument to operator '<<'
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_BIG )
	{
		PSI_SWAP_U32(&host_long);
	}

	return( host_long );
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_NTOHS
LSA_UINT16 CLRPC_NTOHS(
	LSA_UINT16 network_short )
{
    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    //lint --e(835) A zero has been given as [left/right] argument to operator '<<'
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_BIG )
	{
		PSI_SWAP_U16(&network_short);
	}

	return( network_short);
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_NTOHL
LSA_UINT32 CLRPC_NTOHL(
	LSA_UINT32 network_long )
{
    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    //lint --e(835) A zero has been given as [left/right] argument to operator '<<'
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_BIG )
	{
		PSI_SWAP_U32(&network_long);
	}

	return( network_long );
}
#else
# error "by design a function"
#endif


/*=============================================================================
 * function name:  CLRPC_ PUT/GET 16_/UUID_ LITTLE
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifndef CLRPC_PUT16_LITTLE
LSA_VOID CLRPC_PUT16_LITTLE( /* put, little endian byte order */
	LSA_VOID_PTR_TYPE ptr,
	LSA_UINT16        val )
{
    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_LITTLE )
	{
		PSI_SWAP_U16(&val);
	}

	PSI_MEMCPY( ptr, &val, sizeof(val) );
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_GET16_LITTLE
LSA_UINT16 CLRPC_GET16_LITTLE( /* get, little endian byte order */
	LSA_VOID_PTR_TYPE ptr )
{
	LSA_UINT16 tmp;
	PSI_MEMCPY(&tmp, ptr, sizeof(tmp));

    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_LITTLE )
	{
		PSI_SWAP_U16(&tmp);
	}

	return( tmp );
    //lint --e(818) Pointer parameter 'ptr' could be declared as pointing to const - we are unable to change the API
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_PUT_UUID_LITTLE
LSA_VOID CLRPC_PUT_UUID_LITTLE( /* put, convert into little endian byte order */
	LSA_VOID_PTR_TYPE     ptr,
	CLRPC_UUID_PTR_TYPE   uuid_ptr ) /* correct alignment! */
{
	CLRPC_UUID_TYPE tmp = *uuid_ptr;

    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_LITTLE )
	{
		CLRPC_SWAP_UUID(&tmp);
	}

	PSI_MEMCPY(ptr, &tmp, sizeof(tmp));
    //lint --e(818) Pointer parameter 'uuid_ptr' could be declared as pointing to const - we are unable to change the API
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_GET_UUID_LITTLE
LSA_VOID CLRPC_GET_UUID_LITTLE( /* get, convert from little endian byte order */
	CLRPC_UUID_PTR_TYPE   uuid_ptr, /* correct alignment! */
	LSA_VOID_PTR_TYPE     ptr )
{
	PSI_MEMCPY(uuid_ptr, ptr, sizeof(*uuid_ptr));

    //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the "CLRPC_CFG_DREP_INT" can be changed by system adaption
    if( CLRPC_CFG_DREP_INT != CLRPC_DREP_INT_LITTLE )
	{
		CLRPC_SWAP_UUID(uuid_ptr);
	}
    //lint --e(818) Pointer parameter 'ptr' could be declared as pointing to const - we are unable to change the API
}
#else
# error "by design a function"
#endif

/*=============================================================================
 * function name:  CLRPC_MEMSET, CLRPC_MEMCPY,
 *
 * function:       macros for standard C-lib functions
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat"
 *
 *===========================================================================*/

#ifndef CLRPC_MEMSET
LSA_VOID CLRPC_MEMSET(
	LSA_VOID_PTR_TYPE   ptr,
	LSA_INT             val,
	LSA_UINT            len )
{
	PSI_MEMSET(ptr, val, len);
}
#else
# error "by design a function"
#endif

#ifndef CLRPC_MEMCPY
LSA_VOID CLRPC_MEMCPY(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len )
{
	PSI_MEMCPY(dst, src, len);
    //lint --e(818) Pointer parameter 'src' could be declared as pointing to const - we are unable to change the API
}
#else
# error "by design a function"
#endif

#endif // PSI_CFG_USE_LD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
