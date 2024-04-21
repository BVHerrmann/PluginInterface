#ifndef EDDI_NRT_FRAME_H                     /* ----- reinclude-protection ----- */
#define EDDI_NRT_FRAME_H

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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_nrt_frame.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal types and data (and constants) for EDD                  */
/*  which must be used with packing alignment.                               */
/*                                                                           */
/*  This file can be overwritten during system integration, because          */
/*  the packing alignment instructions depend on the compiler.               */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* Note: It must be assured that the compiler dont inserts any padding bytes.*/
/*       The structure elements are already well aligned so typically no     */
/*       padding should be inserted/neccessary.                              */
/*       - all 16-Bit Values start on 16-Bit boundary                        */
/*       - all 32-Bit Values start on 32-Bit boundary                        */
/*       - all Structures are a multiple of 32-Bit in size                   */
/*       If and what kind of padding directive is neccessary depends on the  */
/*       compiler. Typically no directive is neccessary.                     */
/*---------------------------------------------------------------------------*/


#include "pnio_pck1_on.h"
/*===========================================================================*/
/*                           EDD-Defines                                     */
/*===========================================================================*/

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_TYPE_LEN_FRAME_ID_TYPE
{
    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_TYPE_LEN_FRAME_ID_w
    {
        LSA_UINT16  TypeLen;
        LSA_UINT16  FrameID;    /* Frame ID if SRT Frame             */
    } PNIO_PACKED_ATTRIBUTE_POST w;

    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_TYPE_LEN_FRAME_ID_b
    {
        LSA_UINT8   TypeLen_H;  /* because inet-format is big-endian */
        LSA_UINT8   TypeLen_L;  /* because inet-format is big-endian */
        LSA_UINT8   FrameID_H;  /* because inet-format is big-endian */
        LSA_UINT8   FrameID_L;  /* because inet-format is big-endian */
    } PNIO_PACKED_ATTRIBUTE_POST b;

    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_TYPE_LEN_FRAME_ID_bIP
    {
        LSA_UINT8   TypeLen_H;  /* because inet-format is big-endian */
        LSA_UINT8   TypeLen_L;  /* because inet-format is big-endian */
        LSA_UINT8   IPVersIHL;  /* IPHeader Version+IHL              */
        LSA_UINT8   IPTOS;      /* IPHeader TOS                      */
    } PNIO_PACKED_ATTRIBUTE_POST bIP;

    LSA_UINT32    LenTypeID;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_TYPE_LEN_FRAME_ID_TYPE EDDI_TYPE_LEN_FRAME_ID_TYPE;

/*---------------------------------------------------------------------------*/
/* MAC-Address union                                                         */
/* Used to access the MAC-Address in different ways.                         */
/*---------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_MAC_ADR_TYPE
{
    LSA_UINT8       MacAdr[EDD_MAC_ADDR_SIZE];
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_MAC_ADR_TYPE EDDI_MAC_ADR_TYPE;

/*---------------------------------------------------------------------------*/
/* IP-Address union                                                          */
/* Used to access the IP-Address in different ways.                          */
/*---------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_IP_TYPE
{
    LSA_UINT32     dw;

    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_IP_TYPE_w
    {
        LSA_UINT16   W0;
        LSA_UINT16   W1;
    } PNIO_PACKED_ATTRIBUTE_POST w;

    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_IP_TYPE_b
    {
        LSA_UINT8   IP[EDD_IP_ADDR_SIZE];
    } PNIO_PACKED_ATTRIBUTE_POST b;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_IP_TYPE EDDI_IP_TYPE;

/*---------------------------------------------------------------------------*/
/* IP-Header                                                                 */
/*                                                                           */
/* Offset|Len|Fields                               |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 1 |Version and IHL                      |                         */
/*     1 | 1 |TOS, Differentiated Service Code Point" (DSCP)  |              */
/*     2 | 2 |Total Length                         |                         */
/*     2 | 2 |Identification                       |                         */
/*     2 | 2 |Flags and Fragmentoffset             |                         */
/*     1 | 1 |TTL                                  |                         */
/*     1 | 1 |Protocol                             |                         */
/*     2 | 2 |Header checksum                      |                         */
/*     4 | 4 |Source IP                            |                         */
/*     4 | 4 |Destination IP                       |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/* Note: The IP-Header starts 16-Bit aligned within an Ethernetframe.        */
/*       Not 32-Bit aligned!! So dont access the SrcIP/DestIP 32-Bit wise!   */
/* Note: The "options"-field within IP-Header is not included. Maybe present */
/*       but not allowed with xRToverUDP!                                    */
/*---------------------------------------------------------------------------*/

#define EDDI_IP_HEADER_SIZE                        20

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_IP_HEADER_TYPE
{
    LSA_UINT8          VersionIHL;
    LSA_UINT8          TOS_DSCP;
    LSA_UINT16         TotalLength;
    LSA_UINT16         Identification;
    LSA_UINT16         FragmentOffset;
    LSA_UINT8          TTL;
    LSA_UINT8          Protocol;
    LSA_UINT16         Checksum;
    EDDI_IP_TYPE        SrcIP;
    EDDI_IP_TYPE        DestIP;

}  PNIO_PACKED_ATTRIBUTE_POST; /* total ip header length: 20 bytes (=160 bits) */

typedef struct _EDDI_IP_HEADER_TYPE EDDI_IP_HEADER_TYPE;

/* IP protocol codes */
#define EDDI_IP_ICMP         1       /* ICMP-Protocol              */
#define EDDI_IP_IGMP         2       /* IGMP-Protocol              */
#define EDDI_IP_TCP          6       /* TCP-Protocol               */
#define EDDI_IP_UDP          17      /* UDP-Protocol               */
#define EDDI_IP_VRRP         112     /* VRRP-Protocol              */


/*===========================================================================*/
/*                           ARP-Defines                                     */
/*===========================================================================*/

#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_DEFRAG_ON)

//temporarily_disabled_lint -esym(768, _EDDI_ARP_FRAME_TYPE::Sha)
//temporarily_disabled_lint -esym(768, _EDDI_ARP_FRAME_TYPE::Tha)
//temporarily_disabled_lint -esym(756,  EDDI_ARP_FRAME_TYPE)

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Ethernet transmission layer                                    For IPv4   */
/*                                                              and Ethernet */
/*                                                                  Bytes    */
/*   48.bit: Ethernet address of destination                          6      */
/*   48.bit: Ethernet address of sender                               6      */
/*   16.bit: Protocol type = ether_type$ADDRESS_RESOLUTION  = 0x806   2      */
/*                                                                           */
/* Ethernet packet data:                                                     */
/*   16.bit: (ar$hrd) Hardware address space (e.g., Ethernet,         2      */
/*                    Packet Radio Net.)                                     */
/*   16.bit: (ar$pro) Protocol address space.  For Ethernet           2      */
/*                    hardware, this is from the set of type                 */
/*                    fields ether_typ$&lt;protocol&gt;.                     */
/*    8.bit: (ar$hln) byte length of each hardware address            1      */
/*    8.bit: (ar$pln) byte length of each protocol address            1      */
/*   16.bit: (ar$op)  opcode (ares_op$REQUEST | ares_op$REPLY)        2      */
/*   nbytes: (ar$sha) Hardware address of sender of this              6      */
/*                    packet, n from the ar$hln field.                       */
/*   mbytes: (ar$spa) Protocol address of sender of this              4      */
/*                    packet, m from the ar$pln field.                       */
/*   nbytes: (ar$tha) Hardware address of target of this              6      */
/*                    packet (if known).                                     */
/*   mbytes: (ar$tpa) Protocol address of target.                     4      */
/*                                                                  -----    */
/*                                                                   42      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* Note: must start at least 16-Bit aligned!*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_ARP_FRAME_TYPE
{
    LSA_UINT16      Hrd;                      /* Headertype (1)         */
    LSA_UINT16      Pro;                      /* Protocoltype (0x800)   */
    LSA_UINT8       Hln;                      /* Hardware addr. len (6) */
    LSA_UINT8       Pln;                      /* Protocol len (4)       */
    LSA_UINT16      Op;                       /* Opcode (1,2)           */
    LSA_UINT8       Sha[EDD_MAC_ADDR_SIZE];   /* Sender Hardware addr.  */
    LSA_UINT8       Spa[EDD_IP_ADDR_SIZE];    /* Sender Protocol addr   */
    LSA_UINT8       Tha[EDD_MAC_ADDR_SIZE];   /* Target Hardware addr.  */
    LSA_UINT8       Tpa[EDD_IP_ADDR_SIZE];    /* Target Protocol addr.  */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_ARP_FRAME_TYPE EDDI_ARP_FRAME_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_ARP_FRAME_TYPE  EDD_UPPER_MEM_ATTR * EDDI_ARP_FRAME_PTR_TYPE;

/* field ar$hdr */
#define EDDI_ARP_HRD_ETHERNET     1

/* field ar$pro */
#define EDDI_ARP_PRO_IP           0x800

/* field ar$op */
#define EDDI_ARP_OP_REQUEST       1
//#define EDDI_ARP_OP_RESPONSE    2

/* field ar$hln */
#define EDDI_ARP_HLN              EDD_MAC_ADDR_SIZE /* 6 */

/* field ar$pln */
#define EDDI_ARP_PLN              EDD_IP_ADDR_SIZE  /* 4 */

#endif //EDDI_CFG_REV5 || EDDI_CFG_DEFRAG_ON

/*===========================================================================*/

#define EDDI_IS_BC_MAC(SrcMAC)      \
        ( (SrcMAC[0] & SrcMAC[1] & SrcMAC[2] & SrcMAC[3] & SrcMAC[4] & SrcMAC[5]) == 0xFF )

#define EDDI_IS_MC_MAC(SrcMAC)  ( (SrcMAC[0] & 0x01) )

#define EDDI_UDP_FILTER_MAC_UNICAST                     0x0
#define EDDI_UDP_FILTER_MAC_BROADCAST                   0x1
#define EDDI_UDP_FILTER_MAC_MULTICAST                   0x2

#define EDDI_UDP_FILTER_DST_PORT_NTP_SNTP               0x007B
#define EDDI_UDP_FILTER_DST_PORT_SNMP_1                 0x00A1
#define EDDI_UDP_FILTER_DST_PORT_SNMP_2                 0x00A2
#define EDDI_UDP_FILTER_DST_PORT_PNIO_EPM               0x8894
#define EDDI_UDP_FILTER_DST_PORT_DHCP_CLIENT            0x0044
#define EDDI_UDP_FILTER_DST_PORT_IANA_FREE_PORT_BEGIN   0xC000
#define EDDI_UDP_FILTER_DST_PORT_IANA_FREE_PORT_END     0xFFFF

/*---------------------------------------------------------------------------*/
/* UDP-Header                                                                */
/*                                                                           */
/* Offset|Len|Fields                               |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 2 |Source Port                          |                         */
/*     2 | 2 |Destination Port                     |                         */
/*     4 | 2 |UDP Length                           |                         */
/*     6 | 2 |UDP Checksum                         |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/* Note: The UDP-Header starts 16-Bit aligned within an Ethernetframe.       */
/*---------------------------------------------------------------------------*/

#define EDDI_UDP_HEADER_SIZE                        8

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_UDP_HEADER_TYPE
{
    LSA_UINT16         SrcPort;
    LSA_UINT16         DstPort;
    LSA_UINT16         Length;
    LSA_UINT16         Checksum;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_UDP_HEADER_TYPE EDDI_UDP_HEADER_TYPE;

/* NOTE:                                                                     */
/* Within an ethernetframe the UDP-Header always starts WORD aligned, so     */
/* the structure members can be accessed as WORDS. The UDP Header dont starts*/
/* DWORD aligned within an ethernetframe!!                                   */

#define EDDI_IP_MF           0x2000  /* More fragments flag        */
#define EDDI_IP_OFFSET       0x1FFF  /* Fragment Offset in segment */

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

typedef LSA_UINT16   EDDI_FRAME_ID_TYPE;

/* IMPORTANT NOTE:                                                           */
/* SrcIP and DestIP may not be accessed as DWORDs because they may not be    */
/* DWORD aligned (within an ethernetframe, the IP-Header dont starts DWORD   */
/* aligned but WORD aligned!!). So if the machine dont allows unaligned      */
/* accesses, WORD access shall be used to access SrcIP and DestIP.           */

/* bit fields in VersionIHL */
#define EDDI_IP_VER          0xF0    /* IP version mask            */
#define EDDI_IP_IHL          0x0F    /* IP header length mask      */

/* bit fields in FragmentOffset */
#define EDDI_IP_RS           0x8000  /* reserved                   */
#define EDDI_IP_DF           0x4000  /* Don't fragment flag        */

/* for receive-check. Fragment offset and MF = 0, others dont care*/
#if defined (EDDI_CFG_BIG_ENDIAN)
#define EDDI_IP_FLAG_OFFSET_MSK   0x3FFF
#define EDDI_IP_FLAG_OFFSET_VAL   0
#else
#define EDDI_IP_FLAG_OFFSET_MSK   0xFF3F
#define EDDI_IP_FLAG_OFFSET_VAL   0
#endif


/* TTL               */
#define EDDI_IP_TTL_MAX      64      /* maximum packet life        */


/* Defaults for xRToverUDP */
#define EDDI_IP_XRT_VER_IHL  ( 0x40 | 5 )  /* Version: 4. IHL: 5 */
//#define EDDI_IP_XRT_TOS      ( 0 )         /* TOS: 0             */

/*---------------------------------------------------------------------------*/
/* xRToverUDP general frame format (without ethernet header)                 */
/*                                                                           */
/* Note:  Both IP-Header and UPD-Header type are a multiple of 4 Byte, so    */
/*        they will fit back2back. We use "Data" to fill the header to a     */
/*        a multiple of 4-Bytes so the compiler dont need to fill 2 bytes    */
/*        for alignment!                                                     */
/*---------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_UDP_XRT_FRAME_TYPE
{
    EDDI_IP_HEADER_TYPE     Ip;         /* 20 Bytes/5 DWORDs */
    EDDI_UDP_HEADER_TYPE    Udp;        /*  8 Bytes/2 DWORDs */
    EDDI_FRAME_ID_TYPE      FrameID;    /*  2 Bytes          */
    LSA_UINT16              Data;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_UDP_XRT_FRAME_TYPE EDDI_UDP_XRT_FRAME_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_UDP_XRT_FRAME_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_UDP_XRT_FRAME_PTR_TYPE;

/* Header length of IP+UPD-Header with xRT over UDP-Frames */
#define EDDI_UDP_XRT_HEADER_SIZE  (EDDI_IP_HEADER_SIZE + EDDI_UDP_HEADER_SIZE)

#endif //EDDI_XRT_OVER_UDP_SOFTWARE

#include "pnio_pck_off.h"

/*****************************************************************************/
/*  end of file eddi_nrt_frame.h                                             */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDI_NRT_FRAME_H */
