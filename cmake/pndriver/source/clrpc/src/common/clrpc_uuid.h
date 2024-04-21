#ifndef CLRPC_UUID_H
#define CLRPC_UUID_H

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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_uuid.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  UUID things, (packed structs)                                            */
/*                                                                           */
/*****************************************************************************/

/*
 * NOTE: file is included by clrpc_usr.h
 */

/*
 * NOTE: with V4.2i2.2 the packing-defines were removed (see clrpc_check_sizes(), AP00920933)
 */

/*------------------------------------------------------------------------------
// UUID type
//----------------------------------------------------------------------------*/

/* from AES, Appendix A:
 *
 * The timestamp is a 60 bit value. For UUID version 1, this is represented by Coordinated
 * Universal Time (UTC) as a count of 100-nanosecond intervals since 00:00:00.00, 15
 * October 1582 (the date of Gregorian reform to the Christian calendar).
 */

/**************************************************************************//**
@addtogroup clrpc-group-uuid-sample-functions
@{
******************************************************************************/
struct clrpc_uuid_type_tag {
	LSA_UINT32		time_low;
	LSA_UINT16		time_mid;
	LSA_UINT16		time_hi_and_version;
	LSA_UINT8		clock_seq_hi_and_reserved;
	LSA_UINT8		clock_seq_low;
	LSA_UINT8		node[6];
};

typedef struct clrpc_uuid_type_tag CLRPC_UUID_TYPE;
typedef struct clrpc_uuid_type_tag *CLRPC_UUID_PTR_TYPE;
typedef struct clrpc_uuid_type_tag const *CLRPC_UUID_CONST_PTR_TYPE;
/**@}*************************************************************************/

#define CLRPC_UUID_SIZE  (16)

/*------------------------------------------------------------------------------
// the NIL UUID
//----------------------------------------------------------------------------*/

extern CLRPC_UUID_TYPE clrpc_uuid_nil_uuid; /* defined in clrpc_uuid.c */

/*------------------------------------------------------------------------------
// prototypes
//----------------------------------------------------------------------------*/

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Comparison of two UUIDs for equality.
@details

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@param [in] uuid1 pointer to first UUID
@param [in] uuid2 pointer to second UUID
@return           1: UUIDs are equal \n
                  0: otherwise
******************************************************************************/
LSA_UINT16
clrpc_uuid_equal (
	CLRPC_UUID_CONST_PTR_TYPE	uuid1,
	CLRPC_UUID_CONST_PTR_TYPE	uuid2
	);

/**************************************************************************//**
@ingroup clrpc-group-input-functions
@brief Compares a UUID for equality with "Null UUID".
@details

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@param [in] uuid  pointer to UUID to compare with "NULL UUID"
@return           1: UUID is equal to "NULL UUID" \n
                  0: otherwise
******************************************************************************/
LSA_UINT16
clrpc_uuid_is_nil (
	CLRPC_UUID_CONST_PTR_TYPE	uuid
	);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_UUID_H */
