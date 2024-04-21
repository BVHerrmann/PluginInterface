
#ifndef SNMPX_LIB_H                      /* ----- reinclude-protection ----- */
#define SNMPX_LIB_H

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
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)             :C&  */
/*                                                                           */
/*  F i l e               &F: snmpx_lib.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Library of SNMPX                                                         */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* Low- and High Byte, Word, Swapping */
#ifndef SNMPX_HIBYTE
#define SNMPX_HIBYTE(u16_)  ((LSA_UINT8)(((LSA_UINT16)(u16_)) >> 8))
#endif
#ifndef SNMPX_LOBYTE
#define SNMPX_LOBYTE(u16_)  ((LSA_UINT8)(u16_))
#endif
#ifndef SNMPX_HIWORD
#define SNMPX_HIWORD(u32_)  ((LSA_UINT16)(((LSA_UINT32)(u32_)) >> 16))
#endif
#ifndef SNMPX_LOWORD
#define SNMPX_LOWORD(u32_)  ((LSA_UINT16)(u32_))
#endif


/* Macros for converting High- and Lowbytes/words
 * to a unsigned 16/32-Bit-Value */
#ifndef SNMPX_WORD
#define SNMPX_WORD(l8_,h8_)     \
((LSA_UINT16)((((LSA_UINT16)(l8_)) | (((LSA_UINT16)(h8_)) << 8))))
#endif

#ifndef SNMPX_DWORD
#define SNMPX_DWORD(l16_,h16_)  \
((LSA_UINT32)((((LSA_UINT32)(l16_)) | (((LSA_UINT32)(h16_)) << 16))))
#endif


#ifndef SNMPX_BIG_ENDIAN

/* Macros for exchanging the High- and the Lowbytes of a unsigned 16-Bit-Word */
#ifndef SNMPX_SWAP16
#define SNMPX_SWAP16(u16_)  \
((LSA_UINT16)(((((LSA_UINT16)(u16_)) >> 8) | (((LSA_UINT16)(u16_)) << 8))))
#endif
#ifndef SNMPX_SWAPW
#define SNMPX_SWAPW(src16ptr_, dst16ptr_) \
{*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+1); \
*((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+0);}
#endif

/* Macros for exchanging the High- and the Low-Words (and their High- and Lowbytes)
 * of a unsigned 32-Bit-Value: 0x44332211 -> 0x11223344 */

#ifndef SNMPX_SWAP32
#define SNMPX_SWAP32(u32_)  \
((LSA_UINT32)(SNMPX_SWAP16(SNMPX_HIWORD(u32_)) | (SNMPX_SWAP16(SNMPX_LOWORD(u32_)) << 16)))
#endif
#ifndef SNMPX_SWAPD
#define SNMPX_SWAPD(src32ptr_, dst32ptr_) \
{*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+3); \
*((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+2); \
*((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+1); \
*((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+0);}
#endif

#else   /* SNMPX_BIG_ENDIAN defined: */

/* No swapping nessesary for SNMPX send/receive */
#ifndef SNMPX_SWAP16
#define SNMPX_SWAP16(u16_)  (u16_)
#endif
#ifndef SNMPX_SWAP32
#define SNMPX_SWAP32(u32_)  (u32_)
#endif
#ifndef SNMPX_SWAPW
#define SNMPX_SWAPW(src16ptr_, dst16ptr_) \
{*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+0); \
*((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+1);}
#endif
#ifndef SNMPX_SWAPD
#define SNMPX_SWAPD(src32ptr_, dst32ptr_) \
{*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+0); \
*((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+1); \
*((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+2); \
*((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+3);}
#endif

#endif  /* SNMPX_BIG_ENDIAN defined */

/*===========================================================================*/
/*                           Access to snmp objects                          */
/*===========================================================================*/

#define  SNMPX_OIDCPY(dest,src,len) SNMPX_MEMCPY((char*)dest,(char*)src,((LSA_UINT32)(len)*sizeof(SNMPX_OID_TYPE)))
#define  SNMPX_OIDCMP(buf1,buf2,len) SNMPX_MEMCMP((char*)buf1,(char*)buf2,((LSA_UINT32)(len)*sizeof(SNMPX_OID_TYPE)))


/*===========================================================================*/
/*                           Access to network bytes                         */
/*===========================================================================*/

/*------------------------------------------------------------------------------
    read data from PDU / write data to PDU
  ----------------------------------------------------------------------------*/

#define snmpx_pdu_set_u8(dp_, u8_)      \
{SNMPX_PUT8_HTON ((dp_), 0, (u8_)); (dp_) += 1;}

#define snmpx_pdu_get_u8(u8_, dp_)      \
{u8_ = SNMPX_GET8_NTOH ((dp_), 0); (dp_) += 1;}

#define snmpx_pdu_set_u16(dp_, u16_)        \
{SNMPX_PUT16_HTON ((dp_), 0, (u16_)); (dp_) += 2;}

#define snmpx_pdu_get_u16(u16_, dp_)        \
{u16_ = SNMPX_GET16_NTOH ((dp_), 0); (dp_) += 2;}

#define snmpx_pdu_set_u32(dp_, u32_)        \
{SNMPX_PUT32_HTON (dp_, 0, u32_); (dp_) += 4;}

#define snmpx_pdu_get_u32(u32_, dp_)        \
{u32_ = SNMPX_GET32_NTOH ((dp_), 0); (dp_) += 4;}

#define snmpx_pdu_set_ip(dp_, ip_) \
{SNMPX_MEMCPY (dp_, ip_, SNMPX_IP_ADDR_SIZE); (dp_) += SNMPX_IP_ADDR_SIZE;}

#define snmpx_pdu_get_ip(ip_, dp_) \
{SNMPX_MEMCPY (ip_, dp_, SNMPX_IP_ADDR_SIZE); (dp_) += SNMPX_IP_ADDR_SIZE;}

#define snmpx_pdu_set_mem(dp_, strn_, len_) \
{SNMPX_MEMCPY (dp_, strn_, len_); (dp_) += len_;}

#define snmpx_pdu_get_mem(strn_, dp_, len_) \
{SNMPX_MEMCPY (strn_, dp_, len_); (dp_) += len_;}

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*----------------------------------------------------------------------------*/


/*****************************************************************************/
/*  end of file snmpx_lib.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_LIB_H */
