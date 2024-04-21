#ifndef CM_LIB_H
#define CM_LIB_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_lib.h                                  :F&  */
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

/*
 * included in cm_usr.h
 */


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Conversion of short integer from "host byte order" to "network byte order" (MSB first).
@details
For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] host_short value in host byte order
@return                value in network byte order
******************************************************************************/
#ifndef CM_HTON16
LSA_UINT16
CM_HTON16(
	LSA_UINT16 host_short
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Conversion of long integer from "host byte order" to "network byte order" (MSB first).
@details
For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] host_long value in host byte order
@return               value in network byte order
******************************************************************************/
#ifndef CM_HTON32
LSA_UINT32
CM_HTON32(
	LSA_UINT32 host_long
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Conversion of long long integer from "host byte order" to "network byte order" (MSB first).
@details
For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] host_long_long value in host byte order
@return                    value in network byte order
******************************************************************************/
#ifndef CM_HTON64
LSA_UINT64
CM_HTON64(
	LSA_UINT64 host_long_long
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Conversion of short integer from "network byte order" to "host byte order" (MSB first).
@details
For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] network_short value in network byte order
@return                   value in host byte order
******************************************************************************/
#ifndef CM_NTOH16
LSA_UINT16
CM_NTOH16(
	LSA_UINT16 network_short
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Conversion of long integer from "network byte order" to "host byte order" (MSB first).
@details
For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] network_long value in network byte order
@return                  value in host byte order
******************************************************************************/
#ifndef CM_NTOH32
LSA_UINT32
CM_NTOH32(
	LSA_UINT32 network_long
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Writing of 8-bit values at arbitrarily "aligned" addresses.
@details
Put and convert from host byte order to network byte order.
The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [out] base   base
@param [in]  offset offset
@param [in]  val    value
******************************************************************************/
#ifndef CM_PUT8_HTON
LSA_VOID
CM_PUT8_HTON(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset,
	LSA_UINT8 val
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Reading of 8-bit values at arbitrarily "aligned" addresses.
@details
Get and convert from network byte order to host byte order.
The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] base   base
@param [in] offset offset
@return            converted value
******************************************************************************/
#ifndef CM_GET8_NTOH
LSA_UINT8
CM_GET8_NTOH(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Writing of 16-bit values at arbitrarily "aligned" addresses.
@details
Put and convert from host byte order to network byte order.

16-bit values are simultaneously converted from the the host byte order into network byte order (MSB first).

Background: Some process architectures cannot execute mis-aligned 16-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [out] base   base
@param [in]  offset offset
@param [in]  val    value
******************************************************************************/
#ifndef CM_PUT16_HTON
LSA_VOID
CM_PUT16_HTON(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset,
	LSA_UINT16 val
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Reading of 16-bit values at arbitrarily "aligned" addresses.
@details
Get and convert from network byte order to host byte order.

16-bit values are simultaneously converted from the network byte order (MSB first) into host byte order.

Background: Some process architectures cannot execute mis-aligned 16-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] base   base
@param [in] offset offset
@return            converted value
******************************************************************************/
#ifndef CM_GET16_NTOH
LSA_UINT16
CM_GET16_NTOH(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Writing of 32-bit values at arbitrarily "aligned" addresses.
@details
Put and convert from host byte order to network byte order.

32-bit values are simultaneously converted from the host byte order into the network byte order (MSB first).

Background: Some process architectures cannot execute mis-aligned 32-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [out] base   base
@param [in]  offset offset
@param [in]  val    value
******************************************************************************/
#ifndef CM_PUT32_HTON
LSA_VOID
CM_PUT32_HTON(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset,
	LSA_UINT32 val
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Reading of 32bit values at arbitrarily "aligned" addresses.
@details
Get and convert from network byte order to host byte order.

32-bit values are simultaneously converted from the network byte order (MSB first) into the host byte order.

Background: Some process architectures cannot execute mis-aligned 16-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset). A read of a 32-bit value must therefore, possibly be executed as 4 individual byte accesses.

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [in] base   base
@param [in] offset offset
@return            converted value
******************************************************************************/
#ifndef CM_GET32_NTOH
LSA_UINT32
CM_GET32_NTOH(
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Writing of 64-bit values at arbitrarily "aligned" addresses.
@details
Put and convert from host byte order to network byte order.

64-bit values are simultaneously converted from the host byte order into the network byte order (MSB first).

Background: Some process architectures cannot execute mis-aligned 64-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [out] base   base
@param [in]  offset offset
@param [in]  val    value
******************************************************************************/
#ifndef CM_PUT64_HTON
LSA_VOID
CM_PUT64_HTON(
	LSA_VOID_PTR_TYPE base,
	LSA_INT offset,
	LSA_UINT64 val
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Writing of 32-bit values at arbitrarily "aligned" addresses.
@details
Put and convert from host byte order to network byte order.

32-bit values are simultaneously converted from the host byte order into the network byte order (MSB first).

Background: Some process architectures cannot execute mis-aligned 32-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [out] base   base
@param [in]  offset offset
@param [in]  uuid   destination uuid
******************************************************************************/
#ifndef CM_PUT_UUID_HTON
LSA_VOID
CM_PUT_UUID_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset,
	const CLRPC_UUID_TYPE * const uuid
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Reading of 32bit values at arbitrarily "aligned" addresses.
@details
Get and convert from network byte order to host byte order.

32-bit values are simultaneously converted from the network byte order (MSB first) into the host byte order.

Background: Some process architectures cannot execute mis-aligned 16-bit accesses. Hence, all accesses to such structures are executed by the following macros.

The functions must be written in such a way that the accesses are executed correctly for all convenient values of (base + offset). A read of a 32-bit value must therefore, possibly be executed as 4 individual byte accesses.

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)

@param [out] uuid  destination uuid
@param [in] base   base
@param [in] offset offset
******************************************************************************/
#ifndef CM_GET_UUID_NTOH
LSA_VOID
CM_GET_UUID_NTOH( /* get and convert from network byte order to host byte order */
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_VOID_PTR_TYPE base,
	LSA_UINT32 offset
);
#endif


/**************************************************************************//**
@defgroup cm-product-components-output-functions-standard-lib-c Standard C-Lib output functions
@ingroup cm-product-components-output-functions
@brief LSA modules are "self-contained". Even functions of Standard C-Lib can be addressed only by macros
@details
The functionality corresponds to the appropriate functions of C-Lib (memset, memcpy, etc.).

For prototypes, refer to the file .../cfg/cm_lib.h

For macros, refer to the file .../cfg/cm_cfg.h

For examples, refer to the file .../cfg/cm_cfg_lib.c (officially not a part of CM!)
******************************************************************************/


/**************************************************************************//**
@ingroup cm-product-components-output-functions-standard-lib-c
@brief macros for standard C-lib functions
@details
We will implement variants for mem-attribs when needed. All known platforms are "flat".

@param [out] ptr destination pointer
@param [in]  val value
@param [in]  len length
******************************************************************************/
#ifndef CM_MEMSET
LSA_VOID
CM_MEMSET(
	LSA_VOID_PTR_TYPE  ptr,
	LSA_INT            val,
	LSA_UINT           len
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions-standard-lib-c
@brief macros for standard C-lib functions
@details
We will implement variants for mem-attribs when needed. All known platforms are "flat".

@param [out] dst destination pointer
@param [in]  src source destination pointer
@param [in]  len length
******************************************************************************/
#ifndef CM_MEMCPY
LSA_VOID
CM_MEMCPY(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions-standard-lib-c
@brief macros for standard C-lib functions
@details
We will implement variants for mem-attribs when needed. All known platforms are "flat".

@param [out] dst destination pointer
@param [in]  src source destination pointer
@param [in]  len length
******************************************************************************/
#ifndef CM_MEMMOVE
LSA_VOID
CM_MEMMOVE(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len
);
#endif

/**************************************************************************//**
@ingroup cm-product-components-output-functions-standard-lib-c
@brief macros for standard C-lib functions
@details
We will implement variants for mem-attribs when needed. All known platforms are "flat".

@param [out] dst destination pointer
@param [in]  src source destination pointer
@param [in]  len length
@return      return code
******************************************************************************/
#ifndef CM_MEMCMP
LSA_INT
CM_MEMCMP(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Get the current time stamp in micro seconds since startup or since a specific day (e.g. 1.1.1970)
@details
Macro to get the current time stamp in micro seconds. Every call should give an ascending timestamp.
In the worst case, the next call should show the same value as before.

There is no overflow expected (within the next years).

The granularity of the value must be microsecond.

> If there is no microsecond resolution is available, a timer with a lower resolution might be used.

@internal
The difference between 2 timestamps is used to measure the fast startup, see StartupStatisticsRecord.
@endinternal

@param [out] current_time_stamp_in_us
******************************************************************************/
#if CM_CFG_MAX_CLIENTS
#ifndef CM_GET_TIMESTAMP
LSA_VOID
CM_GET_TIMESTAMP(
	LSA_UINT64 *current_time_stamp_in_us
);
#endif
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_LIB_H */
