#ifndef EDDS_INT_H                    /* ----- reinclude-protection ----- */
#define EDDS_INT_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_int.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping for      */
/*  edds.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/
/**
 * \defgroup EDDS
 */
/**
 * \defgroup LLIF
 * \ingroup EDDS
 */

#include "edds_nrt_inc.h"
#include "edds_srt_inc.h"

#include "edds_dev.h"
#include "edds_llif.h"

#include "edds_dbg.h"

#include "edds_nrt_ext.h"
#include "edds_srt_ext.h"

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDDS functions uses the following IDs                     */
/*                                                                           */
/*        EDDS:  0    ..  15                                                 */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*        LL:    16   ..  31                                                 */
/*                                                                           */
/*        Each component uses the following module IDs (defined in component)*/
/*                                                                           */
/*        NRT:   128  ..  143                                                */
/*        SRT:   144  ..  159                                                */
/*                                                                           */
/* ========================================================================= */

/* EDDS module IDs used in C-Files */

#define EDDS_MODULE_ID_EDD_SYS             1
#define EDDS_MODULE_ID_EDD_DEV             2
#define EDDS_MODULE_ID_EDD_GLB             3
#define EDDS_MODULE_ID_EDD_DBG             4
#define EDDS_MODULE_ID_EDD_TIM             5
#define EDDS_MODULE_ID_EDD_TX              6
#define EDDS_MODULE_ID_EDD_ISR             7
#define EDDS_MODULE_ID_EDD_GEN             8
#define EDDS_MODULE_ID_EDD_PRM             9
#define EDDS_MODULE_ID_EDD_USR             10
#define EDDS_MODULE_ID_EDD_IO              11
#define EDDS_MODULE_ID_EDD_MC_FILTER       13

#define EDDS_MODULE_ID_EDD_NRT_USR         128
#define EDDS_MODULE_ID_EDD_NRT_SND         129
#define EDDS_MODULE_ID_EDD_NRT_RCV         130

#define EDDS_MODULE_ID_EDD_SRT_USR         144
#define EDDS_MODULE_ID_EDD_SRT_PROV        145
#define EDDS_MODULE_ID_EDD_SRT_CONS        146

#define EDDS_MODULE_ID_EDD_LL              16

/*===========================================================================*/
/* some internal config settings.                                            */
/*===========================================================================*/

/* define to do more detailed (slower) check for valid hDDB (e.g. in RQBs)   */

#undef EDDS_CFG_DO_INTERNAL_FATAL_CHECK /* define to do some more internal   */
/* fatal error checks                */

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct edds_data_tag
{
    LSA_FATAL_ERROR_TYPE  edds_error;
    /*  TYPE2                 var2;
     *  TYPE3                 var3;
     *           and so on
     */
} EDDS_DATA_TYPE;


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

/* for backwardcompatibility */

#define EDDS_RQB_SET_STATUS(_pRQB, _Value) EDD_RQB_SET_RESPONSE(_pRQB,_Value)
#define EDDS_RQB_GET_STATUS(_pRQB)        EDD_RQB_GET_RESPONSE(_pRQB)

/* determine maximum */
#define EDDS_MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDSGet2Potenz                              +*/
/*+  Input                      LSA_UINT32       Exponent.                  +*/
/*+  Result                :    2^Exponent                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculate 2^Exponent.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define EDDSGet2Potenz(Exponent) ((LSA_UINT32)1 << (Exponent))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ Conversion of Network byteorder to Hostbyteorder                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef EDDS_CFG_BIG_ENDIAN

/* flips a byte pattern of 0102 to 0201 */
#define EDDS_FLIP16(C) ((LSA_UINT16) ( (((LSA_UINT16) (C))>>8) | ( ((LSA_UINT16)(C)) << 8)) )
/* flips a byte pattern of 01020304 to 04030201 */
#define EDDS_FLIP32(C) ((LSA_UINT32) (((C) >> 24) | (((C) & 0x00ff0000) >> 8) | (((C) & 0x0000ff00) <<  8) | (((C) << 24))) )

#else
#define EDDS_FLIP16(C)     (C)
#define EDDS_FLIP32(C)     (C)
#endif

#ifndef EDDS_NTOHS
#define EDDS_NTOHS(Value) ( EDDS_FLIP16(Value) )/* Network TO Host Short */
#endif

#ifndef EDDS_NTOHL
#define EDDS_NTOHL(Value) ( EDDS_FLIP32(Value) )/* Network TO Host Long */
#endif

#ifndef EDDS_HTONS
#define EDDS_HTONS(Value) ( EDDS_FLIP16(Value) )/* Host To Network Short */
#endif

#ifndef EDDS_HTONL
#define EDDS_HTONL(Value) ( EDDS_FLIP32(Value) )/* Host TO Network Long */
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ Conversion of IOBuffer byteorder from/to Hostbyteorder                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef EDDS_CFG_BIG_ENDIAN
	#define EDDS_SWAP16(C) ((LSA_UINT16) ( (((LSA_UINT16) (C))>>8) | ( ((LSA_UINT16)(C)) << 8)) )
	#define EDDS_SWAP32(C) ((LSA_UINT32) (((C) >> 24) | (((C) & 0x00ff0000) >> 8) | (((C) & 0x0000ff00) <<  8) | (((C) << 24))) )
#else
	#define EDDS_SWAP16(C) (C)
	#define EDDS_SWAP32(C) (C)
#endif

#ifndef LE_TO_H_S
#define LE_TO_H_S(Value) ( EDDS_SWAP16(Value) )  /* Little Endian to Host Short */
#endif

#ifndef LE_TO_H_L
#define LE_TO_H_L(Value) ( EDDS_SWAP32(Value) )  /* Little Endian to Host Long  */
#endif

#ifndef H_TO_LE_S
#define H_TO_LE_S(Value) ( EDDS_SWAP16(Value) )  /* Host to Little Endian Short */
#endif

#ifndef H_TO_LE_L
#define H_TO_LE_L(Value) ( EDDS_SWAP32(Value) )  /* Host to Little Endian Long  */
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ access to Network byteordered variables                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U16(pValue) ((LSA_UINT16) (((LSA_UINT16)(*(pValue)) << 8 ) + *((pValue)+1)))

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U32(pValue) ((LSA_UINT32) (((LSA_UINT32)(*(pValue))     << 24) + \
((LSA_UINT32)(*((pValue)+1)) << 16) + \
((LSA_UINT32)(*((pValue)+2)) << 8)  + \
*((pValue)+3)))

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U16_INCR(pValue,Value) \
{                                      \
Value = EDDS_GET_U16(pValue);      \
pValue += sizeof(LSA_UINT16);      \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U32_INCR(pValue,Value) \
{                                      \
Value = EDDS_GET_U32(pValue);      \
pValue += sizeof(LSA_UINT32);      \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_SET_U8_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_SET_U16_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8 );   \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_SET_U32_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 24 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 16 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8  );  \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/*===========================================================================*/
/*                              other macros                                 */
/*===========================================================================*/

#define EDDS_MACADDR_IS_0(_s1) \
((_s1.MacAdr[0] == 0) && (_s1.MacAdr[1] == 0) &&    \
(_s1.MacAdr[2] == 0) && (_s1.MacAdr[3] == 0) &&    \
(_s1.MacAdr[4] == 0) && (_s1.MacAdr[5] == 0))

#define EDDS_MACADDR_IS_MC(_s1) \
((_s1.MacAdr[0] & 0x01) == 0x01)

/*===========================================================================*/
/*                           chain macros for rqb                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_RQB_QUEUE_TEST_EMPTY
 *
 * function:       test if RQB Queue is empty
 *
 * parameters:     EDDS_RQB_LIST_TYPE * RQB_Queue
 *
 * return value:   LSA_TRUE     Queue is empty
 *                 LSA_FALSE    Queue is not empty
 *
 *===========================================================================*/

#define EDDS_RQB_QUEUE_TEST_EMPTY(_RQB_QUEUE)    \
    (   LSA_HOST_PTR_ARE_EQUAL((_RQB_QUEUE)->pBottom,LSA_NULL)     \
    &&  LSA_HOST_PTR_ARE_EQUAL((_RQB_QUEUE)->pTop,LSA_NULL)  \
    )                                               \

/*=============================================================================
 * function name:  EDDS_RQB_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_RQB_PUT_BLOCK_TOP(_Bottom, _Top, _Block)           \
{                                                               \
\
EDD_RQB_SET_NEXT_RQB_PTR(_Block, LSA_NULL);                 \
EDD_RQB_SET_PREV_RQB_PTR(_Block, _Top);                     \
\
if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
{                                                           \
_Top    = _Block;                                       \
_Bottom = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
EDD_RQB_SET_NEXT_RQB_PTR(_Top, _Block);                 \
_Top        = _Block;                                   \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:  EDDS_RQB_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_RQB_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)        \
{                                                               \
EDD_RQB_SET_NEXT_RQB_PTR(_Block, _Bottom);                  \
EDD_RQB_SET_PREV_RQB_PTR(_Block, LSA_NULL);                 \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
{                                                           \
_Bottom = _Block;                                       \
_Top    = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
EDD_RQB_SET_PREV_RQB_PTR(_Bottom, _Block);              \
_Bottom         = _Block;                               \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_RQB_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block)           \
{                                                               \
_Block = _Top;                                              \
if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
{                                                           \
_Top = EDD_RQB_GET_PREV_RQB_PTR(_Top);                  \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
_Bottom = LSA_NULL;                                 \
else                                                    \
EDD_RQB_SET_NEXT_RQB_PTR(_Top, LSA_NULL);           \
}                                                           \
}                                                               \

/*=============================================================================
 * function name:   EDDS_RQB_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)        \
{                                                               \
_Block = _Bottom;                                           \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
{                                                           \
_Bottom = EDD_RQB_GET_NEXT_RQB_PTR((_Bottom));          \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
_Top = LSA_NULL;                                    \
else                                                    \
EDD_RQB_SET_PREV_RQB_PTR((_Bottom), LSA_NULL);      \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_RQB_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_BLOCK(_Bottom, _Top, _Handle, _Block)      \
{                                                               \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
_Block = LSA_NULL;                                      \
else                                                        \
{                                                           \
_Block = _Bottom;                                       \
\
while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
(! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
_Block = EDD_RQB_GET_NEXT_RQB_PTR(_Block);          \
\
if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
_Block = LSA_NULL;                                  \
else                                                    \
{                                                       \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
{                                                   \
EDDS_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
}                                                   \
else                                                \
{                                                   \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
{                                               \
EDDS_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
}                                               \
else                                            \
{                                               \
EDD_RQB_SET_NEXT_RQB_PTR(EDD_RQB_GET_PREV_RQB_PTR(_Block),EDD_RQB_GET_NEXT_RQB_PTR(_Block)); \
EDD_RQB_SET_PREV_RQB_PTR(EDD_RQB_GET_NEXT_RQB_PTR(_Block),EDD_RQB_GET_PREV_RQB_PTR(_Block)); \
}                                               \
}                                                   \
}                                                       \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_RQB_REM_VALID_BLOCK
 *
 * function:        remove valid block from queue
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_VALID_BLOCK(_Bottom, _Top, _Block)         \
{                                                               \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
{                                                   \
EDDS_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
}                                                   \
else                                                \
{                                                   \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
{                                               \
EDDS_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
}                                               \
else                                            \
{                                               \
EDD_RQB_SET_NEXT_RQB_PTR(EDD_RQB_GET_PREV_RQB_PTR(_Block),EDD_RQB_GET_NEXT_RQB_PTR(_Block)); \
EDD_RQB_SET_PREV_RQB_PTR(EDD_RQB_GET_NEXT_RQB_PTR(_Block),EDD_RQB_GET_PREV_RQB_PTR(_Block)); \
}                                               \
}                                                   \
}                                                               \

/*=============================================================================
 * function name:   EDDS_RQB_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)   \
{                                                                   \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
((_Handle) == 0) )                                          \
_Block = LSA_NULL;                                          \
else                                                            \
{                                                               \
_Block = _Bottom;                                           \
\
while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
(!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
_Block = EDD_RQB_GET_NEXT_RQB_PTR(_Block);              \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
_Block = LSA_NULL;                                      \
}                                                               \
}                                                                   \

/*=============================================================================
 * function name:   EDDS_RQB_SEARCH_QUEUE_LOWER_HANDLE
 *
 * function:        Search RQB-Queue for RQB with Handle.
 *
 * parameters:      EDD_UPPER_RQB_PTR_TYPE         pRQB
 *                  EDDS_LOCAL_HDB_PTR_TYPE        pHDB
 * return value:    LSA_VOID
 *
 * pRQB             points to start of RQB-queue (maybe NULL)
 * pHDB             HDB to be searched within RQB-queue
 *
 * Description:     On return pRQB points to NULL if no entry was found, else
 *                  to the RQB with the spezified HDB. On entry pRQB may
 *                  already be NULL.
 *===========================================================================*/

#define EDDS_RQB_SEARCH_QUEUE_LOWER_HANDLE(pRQB,pHDB)               \
{                                                                   \
while   ((! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) &&              \
( ! LSA_HOST_PTR_ARE_EQUAL(EDD_RQB_GET_HANDLE_LOWER(pRQB), pHDB) )) \
{                                                                   \
pRQB = EDD_RQB_GET_NEXT_RQB_PTR(pRQB);                              \
}                                                                   \
}                                                                   \




/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_PUT_BLOCK_TOP(_Bottom, _Top, _Block)               \
{                                                               \
\
_Block->pNext = LSA_NULL;                                   \
_Block->pPrev = _Top;                                       \
\
if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
{                                                           \
_Top    = _Block;                                       \
_Bottom = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
_Top->pNext = _Block;                                   \
_Top        = _Block;                                   \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:  EDDS_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)            \
{                                                               \
_Block->pNext = _Bottom;                                    \
_Block->pPrev = LSA_NULL;                                   \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
{                                                           \
_Bottom = _Block;                                       \
_Top    = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
_Bottom->pPrev  = _Block;                               \
_Bottom         = _Block;                               \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_REM_BLOCK_TOP
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_REM_BLOCK_TOP(_Bottom, _Top, _Block)               \
{                                                               \
_Block = _Top;                                              \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
{                                                           \
_Top = _Top->pPrev;                                     \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
_Bottom = LSA_NULL;                                 \
else                                                    \
_Top->pNext = LSA_NULL;                             \
}                                                           \
}                                                               \

/*=============================================================================
 * function name:   EDDS_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)            \
{                                                               \
_Block = _Bottom;                                           \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
{                                                           \
_Bottom = _Bottom->pNext;                               \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
_Top = LSA_NULL;                                    \
else                                                    \
_Bottom->pPrev = LSA_NULL;                          \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_REM_BLOCK(_Bottom, _Top, _Handle, _Block)          \
{                                                               \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
_Block = LSA_NULL;                                      \
else                                                        \
{                                                           \
_Block = _Bottom;                                       \
\
while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
(! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
_Block = _Block->pNext;                             \
\
if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
_Block = LSA_NULL;                                  \
else                                                    \
{                                                       \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
{                                                   \
EDDS_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
}                                                   \
else                                                \
{                                                   \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
{                                               \
EDDS_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
}                                               \
else                                            \
{                                               \
_Block->pPrev->pNext = _Block->pNext;       \
_Block->pNext->pPrev = _Block->pPrev;       \
}                                               \
}                                                   \
}                                                       \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)       \
{                                                                   \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
((_Handle) == 0) )                                          \
_Block = LSA_NULL;                                          \
else                                                            \
{                                                               \
_Block = _Bottom;                                           \
\
while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
(!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
_Block = (_Block)->pNext;                               \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
_Block = LSA_NULL;                                      \
}                                                               \
}                                                                   \
    

/*===========================================================================*/
/*                             bit manipulation                              */
/*===========================================================================*/
/* masks bit(s) */
#define EDDS_BIT(_bit)                                      ( (LSA_UINT32) 1 << (_bit) )
#define EDDS_BITS(_start, _end)                             ( ( ( (LSA_UINT32) 1 << ( (LSA_UINT32) 1 + (_end) - (_start) ) ) - (LSA_UINT32) 1 ) << (_start) )

/* returns the selected bit(s) [starting at pos. 0 -> GET_BITS(0x12, 2, 4) = 0x4;] */
#define EDDS_GET_BIT(_value, _bit)                          ( ( (_value) & EDDS_BIT(_bit) ) >> (_bit) )
#define EDDS_GET_BITS(_value, _start, _end)                 ( ( (_value) & EDDS_BITS(_start, _end) ) >> (_start) )
    
/* sets the selected bit(s) */
#define EDDS_SET_BIT(_lvalue, _bit)                          (_lvalue) |= EDDS_BIT(_bit)
#define EDDS_SET_BITS(_lvalue, _start, _end)                 (_lvalue) |= EDDS_BITS(_start, _end)

/* clears the selected bit(s) */
#define EDDS_CLR_BIT(_lvalue, _bit)                          (_lvalue) &= ~EDDS_BIT(_bit)
#define EDDS_CLR_BITS(_lvalue, _start, _end)                 (_lvalue) &= ~EDDS_BITS(_start, _end)

/* write the given value into the given bit(s) (high bits that are out of range are cut off! */
/* e.g. _lvalue 0, _start 0, _end 15, _value_bits 0xFFFF1234 --> _lvalue 0x1234 */
#define EDDS_WRITE_BITS_FOR_MASK(_start, _end, _value)       ( ( (_value) << (_start) ) & EDDS_BITS(_start, _end) )
    
/* write OR clears a single bit for a bit mask */
#define EDDS_WRITE_BIT_FOR_MASK(_bit, _value_bit)            ( (_value_bit) ? EDDS_BIT(_bit) : 0 )
       
#define EDDS_WRITE_BITS(_lvalue, _start, _end, _value_bits)  ( _lvalue =  EDDS_WRITE_BITS_FOR_MASK(_start, _end, _value_bits) | (_lvalue & ~EDDS_BITS(_start, _end) ) )
/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* in edds_dev.c */

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleAcquire( EDDS_LOCAL_DDB_PTR_TYPE const pDDB, EDDS_LOCAL_HDB_PTR_TYPE * ppHDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleRelease( EDDS_LOCAL_HDB_PTR_TYPE pHDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_CreateDDB( EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB,
                                                            EDDS_UPPER_DPB_PTR_TO_CONST_TYPE const              pDPB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_CloseDDB( EDDS_HANDLE hDDB);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_GetDDB( EDDS_HANDLE const hDDB,
                                                         EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleGetHDB( EDD_HANDLE_LOWER_TYPE const LowerHandle,
                                                               EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppHDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleIni(LSA_VOID);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleClose(LSA_VOID);

LSA_EXTERN  LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_IsAnyHandleInUse(EDDS_DDB_TYPE * const pDDB);

/* in edds_tx.c */

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_TransmitSts(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB);

LSA_EXTERN LSA_VOID   EDDS_LOCAL_FCT_ATTR EDDS_TransmitTrigger(EDDS_LOCAL_DDB_PTR_TYPE pDDB, LSA_BOOL doNrtOnly);

LSA_EXTERN LSA_VOID   EDDS_LOCAL_FCT_ATTR EDDS_TransmitShutdown(EDDS_LOCAL_DDB_PTR_TYPE pDDB);

/* in edds_tim.c */

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ResetAllTimer(EDDS_LOCAL_DDB_PTR_TYPE pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_StartTimer(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    EDDS_TIMER_ENUM timerID,
    LSA_UINT64 firstExpireTime,
    LSA_UINT32 cycleTime,
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, TimeoutMethod)(EDDS_LOCAL_DDB_PTR_TYPE pDDB)
);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_StopTimer(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    EDDS_TIMER_ENUM timerID
);

/* in edds_glb.c */

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_ScheduledRequestFinish(EDD_UPPER_RQB_PTR_TYPE      pRQB);

LSA_EXTERN  LSA_VOID    EDDS_LOCAL_FCT_ATTR EDDS_RequestFinish( EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
                                                                EDD_UPPER_RQB_PTR_TYPE      pRQB,
                                                                LSA_RESULT                  Status);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CallCbf(
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB),
    EDD_UPPER_RQB_PTR_TYPE      pRQB);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CallChannelCbf(
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB),
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE            pSys);

LSA_EXTERN  EDD_UPPER_RQB_PTR_TYPE      EDDS_LOCAL_FCT_ATTR EDDS_AllocInternalRQB(LSA_VOID);

LSA_EXTERN  LSA_VOID                    EDDS_LOCAL_FCT_ATTR EDDS_FreeInternalRQB(EDDS_INTERNAL_REQUEST_PTR_TYPE const pInternalRQB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_SetupInternalRQBParams(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pInternalRQB,
    EDD_SERVICE                     Service,
    EDD_HANDLE_LOWER_TYPE           LowerHandle,
    EDDS_HANDLE                     hDDB,
    LSA_UINT32                      ID,
    LSA_UINT32                      Param);


LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_InsertSrcMac(
    EDD_MAC_ADR_TYPE  EDDS_LOCAL_MEM_ATTR *pMAC,
    EDD_UPPER_MEM_PTR_TYPE              pBuffer);

LSA_EXTERN EDDS_FAR_FCT LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_FatalError( EDDS_FATAL_ERROR_TYPE Error,
                                                                      LSA_UINT16             ModuleID,
                                                                      LSA_UINT32             Line);


LSA_EXTERN LSA_BOOL  EDDS_LOCAL_FCT_ATTR EDDS_FindLinkAutoMode(
    EDDS_LOCAL_DDB_PTR_TYPE  pDDB);

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_CalculateTxRxBandwith(
    EDDS_LOCAL_DDB_PTR_TYPE  pDDB);

/* in edds_gen.c */

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_GeneralRequest(EDD_UPPER_RQB_PTR_TYPE      pRQB,
                                                                EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_SwitchRequest(EDD_UPPER_RQB_PTR_TYPE       pRQB,
                                                              EDDS_LOCAL_HDB_PTR_TYPE       pHDB);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandlePHYEvent(
    EDDS_LOCAL_DDB_PTR_TYPE  const   pDDB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DebugRequest(EDD_UPPER_RQB_PTR_TYPE      pRQB,
                                                              EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadIndicateEvent(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PRMDiagCycleCBF(
    EDDS_LOCAL_DDB_PTR_TYPE     const pDDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetMRPState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandleLinkIndTrigger(
        EDDS_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_IndicateExtLinkChange(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  PortID);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatistics(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatisticsAll(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleHighRQB_byScheduler(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    EDD_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleRQB_byScheduler(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    EDD_UPPER_RQB_PTR_TYPE pRQB);

/* in edds_dbg.c */

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ReentranceCheckInit(LSA_VOID);

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckRequest(
    EDD_UPPER_RQB_PTR_TYPE pRQB,LSA_UINT32 *pReentrPrio);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckRequest(
    LSA_UINT32 ReentrPrio);

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckISR(LSA_VOID);
LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckISR(LSA_VOID);
LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckTimer(LSA_VOID);
LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckTimer(LSA_VOID);

/* in edds_prm.c */

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmIndicateAll(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_PrmCommit_Finish(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmCopySetB2A(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmSetBToNotPresent(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmAlloc(
    EDDS_PRM_PTR_TYPE     pPrm);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmFree(
    EDDS_PRM_PTR_TYPE     pPrm);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmInit(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmIndication(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmWrite(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmRead(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmCommit(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmEnd(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmPrepare(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmChangePort_PlugPrepare(
	EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
	LSA_UINT16					PortID);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmChangePort_Statemachine(
	EDDS_LOCAL_DDB_PTR_TYPE           pDDB,
	LSA_UINT16						  PortID,
	EDDS_PRMCHANGEPORT_TRIGGER_TYPE   Trigger);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_Prm_Statemachine(
	EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
	EDDS_PRM_TRIGGER_TYPE       Trigger);

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_SchedulerPrmChangePort(
	EDDS_LOCAL_HDB_PTR_TYPE 	pHDB,
	EDD_UPPER_RQB_PTR_TYPE      pRQB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmChangePort(
	EDD_UPPER_RQB_PTR_TYPE      pRQB,
	EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
	LSA_BOOL                  * pIndicate);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagFrameDropHandler(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  DropCnt);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SchedulerUpdateCycleTime(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_Init(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_Statemachine(
    EDDS_LOCAL_DDB_PTR_TYPE const         pDDB,
    EDDS_MC_MAC_FILTER_TRIGGER_TYPE const Trigger);

LSA_EXTERN LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_IsEnabled(
    EDDS_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_EXTERN LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_IsActive(
    EDDS_LOCAL_DDB_PTR_TYPE const pDDB);

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_InsertMcMac(
    EDDS_LOCAL_DDB_PTR_TYPE const     pDDB,
    EDD_MAC_ADR_TYPE mcMAC);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_DeleteMcMac(
    EDDS_LOCAL_DDB_PTR_TYPE const     pDDB,
    EDD_MAC_ADR_TYPE mcMAC);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_Reset(
    EDDS_LOCAL_DDB_PTR_TYPE const     pDDB);

LSA_EXTERN LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_McSWFilter_IsEnabledMcMac(
    EDDS_LOCAL_DDB_PTR_TYPE const     pDDB,
    LSA_UINT8 const *                 pMACAddr);

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined EDD_UPPER_IN_FCT_ATTR
#error "EDD_UPPER_IN_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDD_UPPER_OUT_FCT_ATTR
#error "EDD_UPPER_OUT_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_SYSTEM_IN_FCT_ATTR
#error "EDDS_SYSTEM_IN_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_SYSTEM_OUT_FCT_ATTR
#error "EDDS_SYSTEM_OUT_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_LOCAL_FCT_ATTR
#error "EDDS_LOCAL_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDD_UPPER_RQB_ATTR
#error "EDD_UPPER_RQB_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDD_UPPER_MEM_ATTR
#error "EDD_UPPER_MEM_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_SYSTEM_MEM_ATTR
#error "EDDS_SYSTEM_MEM_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_LOCAL_MEM_ATTR
#error "EDDS_LOCAL_MEM_ATTR not defined in file edd_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif



/*****************************************************************************/
/*  end of file EDDS_INT.H                                                */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_INT_H */
