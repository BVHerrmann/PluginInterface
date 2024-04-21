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
/*  F i l e               &F: eddi_check.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LITTLE - BIG Endian check                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_check.h"
#include "eddi_dev.h"

#define EDDI_MODULE_ID     M_ID_EDDI_CHECK
#define LTRC_ACT_MODUL_ID  1

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define  BM_M_LLO_FrameID_13_29            EDDI_PASTE_32(1F,FF,E0,00)
#define  BM_M_LLO_FrameID_13_29_SHIFT      13

#define C_GET_FrameID(target, s32) \
EDDI_GET_xxx(target, s32, BM_M_LLO_FrameID_13_29, BM_M_LLO_FrameID_13_29_SHIFT )

#define C_SET_FrameID(target, frame_id) \
EDDI_SET_xxx(target, frame_id, BM_M_LLO_FrameID_13_29, BM_M_LLO_FrameID_13_29_SHIFT )

#define C_FRAME_ID  0x2375

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_CExcp( LSA_UINT8  * const sErr,
                                                  LSA_UINT32   const Error,
                                                  LSA_UINT32   const DW_0,
                                                  LSA_UINT32   const DW_1 );

static LSA_VOID EDDI_LOCAL_FCT_ATTR   EDDI_Check_packing( LSA_VOID );


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_Check( LSA_VOID )
{
    EDDI_SER10_LL0_COMMON_TYPE            LL0;
    LSA_UINT8                   *  const  p = (LSA_UINT8 *)(void *)&LL0;
    LSA_UINT32                            tmp_direct, bits;

    EDDI_Check_packing();

    *p       = 0x12;  /* 0001 0010  */
    *(p + 1) = 0x34;  /* 0011 0100  */
    *(p + 2) = 0x56;  /* 0101 0110  */
    *(p + 3) = 0x78;  /* 0111 1000  */

    *(p + 4) = 0x9a;  /* 1001 1010  */
    *(p + 5) = 0xbc;  /* 1011 1100  */
    *(p + 6) = 0xde;  /* 1101 1101  */
    *(p + 7) = 0xf0;  /* 1111 0000  */

    C_SET_FrameID(LL0.Value.U32_0, C_FRAME_ID);

    /**************   READ ***************/
    // Read from HW in LE  -> convert to BE

    /* Compare access to LE value */
    bits = EDDI_GetBitField32(LL0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc);
    if (bits != 0x02)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 0", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    bits =  EDDI_GetBitField32(LL0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__IdxEthType);
    if (bits != 0x01)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 1", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    bits = EDDI_GetBitField32(LL0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId);
    if (bits != C_FRAME_ID)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 2", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    bits = EDDI_GetBitField32(LL0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Len_2_0);
    if (bits != 0x03)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 3", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    bits = EDDI_GetBitField32(LL0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3);
    if (bits != 0x009a)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 4", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    bits = EDDI_GetBitField32(LL0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__AppStatSel);
    if (bits != 0x04)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 5", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    bits = EDDI_GetBitField32(LL0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext);
    if (bits != 0x1e1bd7)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 6", EDDI_FATAL_ERR_EXCP, bits, 0);
    }

    /**************   WRITE  ***************/

    EDDI_SetBitField32(&LL0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__FrameId,  0x4367);
    EDDI_SetBitField32(&LL0.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__Len_10_3, 0x8b);

    /* Direct access HW  */
    C_GET_FrameID(tmp_direct, LL0.Value.U32_0);

    if (tmp_direct != 0x4367)
    {
        EDDI_CExcp((LSA_UINT8 *)(void *)"ENDIAN Problem EDDI_Check 7", EDDI_FATAL_ERR_EXCP, tmp_direct, 0);
    }

    //EDDI_CExcp((LSA_UINT8 *)(void *)"stop", EDDI_FATAL_ERR_EXCP, bits, 0);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CExcp( LSA_UINT8   *  const  sErr,
                                                   LSA_UINT32     const  Error,
                                                   LSA_UINT32     const  DW_0,
                                                   LSA_UINT32     const  DW_1 )
{
    #ifndef BIG_ENDIAN_EMULATOR_TEST
    if (sErr)
    {
    }
    //no trace (test)
    EDDI_Excp(sErr, Error, DW_0, DW_1);
    #else
    if (sErr || Error || DW_0 || DW_1)
    {
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_Check_packing( LSA_VOID )
{
    LSA_UINT32  size, size_expected;

    size_expected = 4;
    size          = sizeof(EDDI_TYPE_LEN_FRAME_ID_TYPE);

    if (size != size_expected)    //lint !e774  BV 27/01/2016  // always false ( falls Compiler richtig packt ! )
    {
        EDDI_Excp("EDDI_Check_packing - EDDI_TYPE_LEN_FRAME_ID_TYPE", 0, size, size_expected);
        return;
    }

    size_expected = 4;
    size          = sizeof(EDDI_CRT_DATA_APDU_STATUS);

    if (size != size_expected)    //lint !e774  BV 27/01/2016  // always false ( falls Compiler richtig packt ! )
    {
        EDDI_Excp("EDDI_Check_packing - EDDI_CRT_DATA_APDU_STATUS", 0, size, size_expected);
        return;
    }

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    size_expected = 20;
    size          = sizeof(EDDI_IP_HEADER_TYPE);

    if (size != size_expected)    //lint !e774  BV 27/01/2016  // always false ( falls Compiler richtig packt ! )
    {
        EDDI_Excp("EDDI_Check_packing - EDDI_IP_HEADER_TYPE", 0, size, size_expected);
        return;
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_check.c                                                 */
/*****************************************************************************/

