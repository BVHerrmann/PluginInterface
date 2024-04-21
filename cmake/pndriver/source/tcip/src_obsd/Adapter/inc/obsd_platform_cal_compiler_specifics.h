/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_cal_compiler_specifics.h
 *
 * @brief       C(ompiler) A(daption) L(ayer)
 * @details     include file with compiler specific definitions
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V1.04
 * @date        created: 13.03.2013
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef __OBSD_PLATFORM_CAL_COMPILER_SPECIFICS_H__
#define __OBSD_PLATFORM_CAL_COMPILER_SPECIFICS_H__

#include "obsd_platform_cal_stdarg.h"

/* v===================== alignment ================================================================v */

#define __packed                    /* have special PNIO packing instead                              */

/* ^===================== alignment ================================================================^ */

/* v===================== bitfields ================================================================v */
/**
    @brief          macro to get the lower nibble of a byte.
    @details        macro to get the lower nibble (bits 0-3) of a byte (with 2 nibbles).
    @param[in]      Byte that holds 2 nibbles
    @return         Returns a byte that holds (in bits 0-3) the lower nibble of the byte given as parameter.
*/
#define CAL_GET_LOW_NIBBLE_OF_BYTE(Byte)     ((u_int8_t) ( (Byte)     & 0x0Fu))

/**
    @brief          macro to get the higher nibble of a byte.
    @details        macro to get the higher nibble (Bits 4-7) of a byte (with 2 nibbles).
    @param[in]      Byte that holds 2 nibbles
    @return         Returns a byte that holds (in bits 0-3) the higher nibble of the byte given as parameter.
*/
#define CAL_GET_HIGH_NIBBLE_OF_BYTE(Byte)    ((u_int8_t) (((Byte)>>4) & 0x0Fu))

/**
    @brief          macro to set the lower nibble of a byte to a given value.
    @details        macro that sets the lower nibble (bits 0-3) of a byte (with 2 nibbles) to a given value.
    @param[in]      Byte that holds 2 nibbles
    @param[in]      Value with the nibble to be written to the lower nibble of the given byte
    @return         Returns a byte with the lower nibble set to the given value.
*/
#define CAL_SET_LOW_NIBBLE_OF_BYTE(Byte, Val)     ((Byte) = (u_int8_t)((Byte) & 0xF0u)      | ((Val)  & 0x0F))

/**
    @brief          macro to set the higher nibble of a byte to a given value.
    @details        macro that sets the higher nibble (bits 4-7) of a byte (with 2 nibbles) to a given value.
    @param[in]      Byte that holds 2 nibbles
    @param[in]      Value with the nibble to be written to the higher nibble of the given byte
    @return         Returns a byte with the higher nibble set to the given value.
*/
#define CAL_SET_HIGH_NIBBLE_OF_BYTE(Byte, Val)    ((Byte) = (u_int8_t)(((Val) & 0x0Fu) <<4) | ((Byte) & 0x0F))

/* ^===================== bitfields ================================================================^ */

/* v===================== types ====================================================================v */
/**
    @brief          vararg list type primary used in userland code.
    @note           Some compilers (like GCC versions above 3) have builtins for varargs.
                    HM: GCC version 6 doesn't accept "char*" anymore (warning: conflicting types for built-in function)
*/
#if defined _MSC_VER || defined TOOL_CHAIN_GREENHILLS_ARM
typedef char *      __va_list;
#else
typedef __builtin_va_list      __va_list;
#endif

/* ^===================== types ====================================================================^ */


/* v===================== compiler stuff ===========================================================v */
#ifdef _MSC_VER
#define   MSVC_COMPILER _MSC_VER

#define __func__ __FUNCTION__                           /* msvc has no C99 __func__                   */

#if _MSC_VER == 1400                                    /* MSVC 2005 needs _SIZE_T_DEFINED_ defined   */
#define _SIZE_T_DEFINED_                                /* MSVC 2010 needs _SIZE_T_DEFINED_ undefined,*/
                                                        /* otherwise we miss the size_t type in VS2010*/
#endif

#ifndef __STDC__
#define __STDC__                                        /* let OBSD Stack assume ANSI C compatibility */
#endif

/**
    @brief          vararg list type used in userland and kernel code.
    @note           Should always be mapped to __va_list.
*/
typedef __va_list	va_list;


#endif /* _MSC_VER */

/* ^===================== compiler stuff ===========================================================^ */


#endif /* __OBSD_PLATFORM_CAL_COMPILER_SPECIFICS_H__ */

