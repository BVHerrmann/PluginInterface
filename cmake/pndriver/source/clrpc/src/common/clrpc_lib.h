#ifndef CLRPC_LIB_H
#define CLRPC_LIB_H

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
/*  F i l e               &F: clrpc_lib.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Conversion functions                                                     */
/*  C-stdlib mapping functions                                               */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MAX_UUID_STRING  (36+1) /* including the '\0' */

/**************************************************************************//**
@defgroup clrpc-group-uuid-sample-functions UUID: sample functions
@ingroup clrpc-group
******************************************************************************/

/**************************************************************************//**
@ingroup clrpc-group-uuid-sample-functions
@brief Converts an UUID into a string representation.
@details
Converts the binary representation of a UUID into the defined string representation.

Format of string representation: tttttttt-tttt-vttt-rccc-nnnnnnnnnnnn \n
t..time,  v..version(3 bits), c..clock, n..node

@see [CLRPC_UUID_TYPE](@ref clrpc_uuid_type_tag)

@cond INTERNAL
For implementation details refer [/4/](@ref clrpc-ref-no-4).
@endcond

@param [in]  uuid        pointer to UUID
@param [out] uuid_string pointer to UUID string
@return                  CLRPC_OK
******************************************************************************/
LSA_UINT16
clrpc_uuid_to_string (
	CLRPC_UUID_PTR_TYPE	uuid,
	CLRPC_COMMON_MEM_U8_PTR_TYPE uuid_string
);

/**************************************************************************//**
@ingroup clrpc-group-uuid-sample-functions
@brief Converts the string representation of an UUID into binary representation.
@details
Format of string representation: tttttttt-tttt-vttt-rccc-nnnnnnnnnnnn \n
t..time,  v..version(3 bits), c..clock, n..node

For implementation details refer [/2/](@ref clrpc-ref-no-2).

@see [CLRPC_UUID_TYPE](@ref clrpc_uuid_type_tag)

@param [out] uuid        pointer to UUID
@param [in]  uuid_string pointer to UUID string
@return                  CLRPC_OK: ok \n
                         CLRPC_ERR_PARAM: wrong uuid format
******************************************************************************/
LSA_UINT16
clrpc_uuid_from_string (
	CLRPC_UUID_PTR_TYPE	uuid,
	CLRPC_COMMON_MEM_U8_PTR_TYPE uuid_string
);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_LIB_H */
