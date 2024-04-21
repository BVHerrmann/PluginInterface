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
/*  F i l e               &F: eddi_bf.c                                 :F&  */
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
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
//#include "eddi_dev.h"

#define EDDI_MODULE_ID     M_ID_EDDI_BF
#define LTRC_ACT_MODUL_ID  17

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if !defined (EDDI_INLINE)

static const LSA_UINT16   MD_EDDI_BF16_SINGLE_MASKS[] =
{
    EDDI_PASTE_16(00,01),
    EDDI_PASTE_16(00,02),
    EDDI_PASTE_16(00,04),
    EDDI_PASTE_16(00,08),
    EDDI_PASTE_16(00,10),
    EDDI_PASTE_16(00,20),
    EDDI_PASTE_16(00,40),
    EDDI_PASTE_16(00,80),

    EDDI_PASTE_16(01,00),
    EDDI_PASTE_16(02,00),
    EDDI_PASTE_16(04,00),
    EDDI_PASTE_16(08,00),
    EDDI_PASTE_16(10,00),
    EDDI_PASTE_16(20,00),
    EDDI_PASTE_16(40,00),
    EDDI_PASTE_16(80,00)
};

static const LSA_UINT16   MD_EDDI_BF16_MULTI_MASKS[] =
{
    EDDI_MASK_16(00,01),
    EDDI_MASK_16(00,03),
    EDDI_MASK_16(00,07),
    EDDI_MASK_16(00,0F),
    EDDI_MASK_16(00,1F),
    EDDI_MASK_16(00,3F),
    EDDI_MASK_16(00,7F),
    EDDI_MASK_16(00,FF),

    EDDI_MASK_16(01,FF),
    EDDI_MASK_16(03,FF),
    EDDI_MASK_16(07,FF),
    EDDI_MASK_16(0F,FF),
    EDDI_MASK_16(1F,FF),
    EDDI_MASK_16(3F,FF),
    EDDI_MASK_16(7F,FF),
    EDDI_MASK_16(FF,FF)
};

static const LSA_UINT32   EDDI_SINGLE_MASKS_32[] =
{
    EDDI_PASTE_32(00,00,00,01),
    EDDI_PASTE_32(00,00,00,02),
    EDDI_PASTE_32(00,00,00,04),
    EDDI_PASTE_32(00,00,00,08),
    EDDI_PASTE_32(00,00,00,10),
    EDDI_PASTE_32(00,00,00,20),
    EDDI_PASTE_32(00,00,00,40),
    EDDI_PASTE_32(00,00,00,80),

    EDDI_PASTE_32(00,00,01,00),
    EDDI_PASTE_32(00,00,02,00),
    EDDI_PASTE_32(00,00,04,00),
    EDDI_PASTE_32(00,00,08,00),
    EDDI_PASTE_32(00,00,10,00),
    EDDI_PASTE_32(00,00,20,00),
    EDDI_PASTE_32(00,00,40,00),
    EDDI_PASTE_32(00,00,80,00),

    EDDI_PASTE_32(00,01,00,00),
    EDDI_PASTE_32(00,02,00,00),
    EDDI_PASTE_32(00,04,00,00),
    EDDI_PASTE_32(00,08,00,00),
    EDDI_PASTE_32(00,10,00,00),
    EDDI_PASTE_32(00,20,00,00),
    EDDI_PASTE_32(00,40,00,00),
    EDDI_PASTE_32(00,80,00,00),

    EDDI_PASTE_32(01,00,00,00),
    EDDI_PASTE_32(02,00,00,00),
    EDDI_PASTE_32(04,00,00,00),
    EDDI_PASTE_32(08,00,00,00),
    EDDI_PASTE_32(10,00,00,00),
    EDDI_PASTE_32(20,00,00,00),
    EDDI_PASTE_32(40,00,00,00),
    EDDI_PASTE_32(80,00,00,00)
};

static const LSA_UINT32   EDDI_MULTI_MASKS_32[] =
{
    EDDI_MASK_32(00,00,00,01),
    EDDI_MASK_32(00,00,00,03),
    EDDI_MASK_32(00,00,00,07),
    EDDI_MASK_32(00,00,00,0F),
    EDDI_MASK_32(00,00,00,1F),
    EDDI_MASK_32(00,00,00,3F),
    EDDI_MASK_32(00,00,00,7F),
    EDDI_MASK_32(00,00,00,FF),

    EDDI_MASK_32(00,00,01,FF),
    EDDI_MASK_32(00,00,03,FF),
    EDDI_MASK_32(00,00,07,FF),
    EDDI_MASK_32(00,00,0F,FF),
    EDDI_MASK_32(00,00,1F,FF),
    EDDI_MASK_32(00,00,3F,FF),
    EDDI_MASK_32(00,00,7F,FF),
    EDDI_MASK_32(00,00,FF,FF),

    EDDI_MASK_32(00,01,FF,FF),
    EDDI_MASK_32(00,03,FF,FF),
    EDDI_MASK_32(00,07,FF,FF),
    EDDI_MASK_32(00,0F,FF,FF),
    EDDI_MASK_32(00,1F,FF,FF),
    EDDI_MASK_32(00,3F,FF,FF),
    EDDI_MASK_32(00,7F,FF,FF),
    EDDI_MASK_32(00,FF,FF,FF),

    EDDI_MASK_32(01,FF,FF,FF),
    EDDI_MASK_32(03,FF,FF,FF),
    EDDI_MASK_32(07,FF,FF,FF),
    EDDI_MASK_32(0F,FF,FF,FF),
    EDDI_MASK_32(1F,FF,FF,FF),
    EDDI_MASK_32(3F,FF,FF,FF),
    EDDI_MASK_32(7F,FF,FF,FF),
    EDDI_MASK_32(FF,FF,FF,FF)
};

#endif //EDDI_INLINE

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
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField8Bit( LSA_UINT8  const  Value,
                                                         LSA_UINT8  const  BitHigh,
                                                         LSA_UINT8  const  BitLow )
{
    LSA_UINT8  const  BitMask = (LSA_UINT8)(((LSA_UINT32)0xFF >> (7 - BitHigh)) & ((LSA_UINT32)0xFF << BitLow));

    if (BitLow > BitHigh)
    {
        EDDI_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "EDDI_GetBitField32 - range: BitLow %d BitHigh %d", BitLow , BitHigh); 
        EDDI_Excp("EDDI_GetBitField8 - range", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }
    else
    {
        return ((Value & BitMask) >> BitLow);
    }
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
LSA_UINT16  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBF16Host( const  LSA_UINT16  Reg,
                                                     const  LSA_UINT32  BitHigh,
                                                     const  LSA_UINT32  BitLow )
{
    LSA_UINT16  ret;
    LSA_UINT16  tmp;

    tmp = EDDI_VALTOLE16(Reg);

    ret = EDDI_GetBitField16(tmp, BitHigh, BitLow);

    return ret;
}
/*---------------------- end [subroutine] ---------------------------------*/

#if !defined (EDDI_INLINE)

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
LSA_UINT16  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField16( const  LSA_UINT16  Reg,
                                                       const  LSA_UINT32  BitHigh,
                                                       const  LSA_UINT32  BitLow )
{
    LSA_UINT32         ret;
    LSA_UINT16         BitMask;
    LSA_INT32   const  range = (LSA_INT32)(BitHigh - BitLow);

    if (range == 0)
    {
        ret = (Reg & MD_EDDI_BF16_SINGLE_MASKS[BitHigh]) >> BitLow;
        return (LSA_UINT16)ret;
    }

    if ((range < 0) || (range > EDDI_BF16_BIT_MAX))
    {
        EDDI_Excp("EDDI_GetBitField16 - range", range, 0, 0);
        return 0;
    }

    BitMask = (LSA_UINT16)((LSA_UINT32)MD_EDDI_BF16_MULTI_MASKS[range] << BitLow);

    ret = Reg & EDDI_HOST2IRTE16(BitMask);

    ret = EDDI_HOST2IRTE16(ret) >> BitLow;
    return (LSA_UINT16)ret;
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
#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SetBitField16( LSA_UINT16       * const pReg,
                                                     const LSA_UINT32         BitHigh,
                                                     const LSA_UINT32         BitLow,
                                                     const LSA_UINT16         value )
{
    LSA_UINT16         BitMask;
    LSA_UINT16         stmp32 = (LSA_UINT16)((LSA_UINT32)value << BitLow);
    LSA_INT32   const  range  = (LSA_INT32)(BitHigh - BitLow);

    stmp32  = EDDI_HOST2IRTE16(stmp32);

    if (range == 0)
    {
        *pReg  = (LSA_UINT16)((*pReg & ~MD_EDDI_BF16_SINGLE_MASKS[BitHigh]) | ( stmp32 & MD_EDDI_BF16_SINGLE_MASKS[BitHigh] ));
        return;
    }

    if ((range < 0) || (range > EDDI_BF16_BIT_MAX))
    {
        EDDI_Excp("EDDI_SetBitField16 - range", (LSA_UINT32)range, 0, 0);
        return;
    }

    BitMask = (LSA_UINT16)((LSA_UINT32)MD_EDDI_BF16_MULTI_MASKS[range] << BitLow);

    BitMask = EDDI_HOST2IRTE16(BitMask);

    *pReg = (LSA_UINT16)((*pReg & ~(BitMask)) | (stmp32 & BitMask));
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_GetBitField32()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField32( LSA_UINT32  const  Reg,
                                                       LSA_UINT32  const  BitHigh,
                                                       LSA_UINT32  const  BitLow )
{
    LSA_UINT32         ret;
    LSA_UINT32         BitMask;
    LSA_UINT32  const  range = BitHigh - BitLow;

    if (range == 0)
    {
        ret = Reg & EDDI_SINGLE_MASKS_32[BitHigh];
        ret = EDDI_HOST2IRTE32(ret) >> BitLow;
        return ret;
    }
    else if (range <= EDDI_BIT_MAX)
    {
        BitMask = EDDI_MULTI_MASKS_32[range] << BitLow;
        ret = Reg & EDDI_HOST2IRTE32(BitMask);
        ret = EDDI_HOST2IRTE32(ret) >> BitLow;
        return ret;
    }
    else
    {
        EDDI_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "EDDI_GetBitField32, invalid range: BitLow %d BitHigh %d", BitLow , BitHigh); 
        EDDI_Excp("EDDI_GetBitField32, invalid range", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************
* F u n c t i o n:       EDDI_SetBitField32()
*
* D e s c r i p t i o n: Converts value from host to little endian format
*                        (if necessary) and stores it into the specified
*                        bitfield of an little endian (!) 32Bit IRTE-
*                        Register or KRAM-Entry.
*
* A r g u m e n t s:     pReg:    Pointer to the 32Bit Register
*                                 (the referenced value is allways seen in
*                                 little endian format)
*                        BitHigh: Highest Bit of the specified bitfield
*                        BitLow:  Lowest  Bit of the specified bitfield
*                        value:   new value of the bitfield in host format.
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SetBitField32( LSA_UINT32  *  const  pReg,
                                                     LSA_UINT32     const  BitHigh,
                                                     LSA_UINT32     const  BitLow,
                                                     LSA_UINT32     const  value )
{
    LSA_UINT32         BitMask;
    LSA_UINT32         stmp32;
    LSA_UINT32  const  range = BitHigh - BitLow;

    stmp32 = EDDI_HOST2IRTE32(value << BitLow);

    if (range == 0)
    {
        *pReg = (*pReg & ~EDDI_SINGLE_MASKS_32[BitHigh]) | (stmp32 & EDDI_SINGLE_MASKS_32[BitHigh]);
        return;
    }
    else if (range <= EDDI_BIT_MAX)
    {
        BitMask = EDDI_HOST2IRTE32(EDDI_MULTI_MASKS_32[range] << BitLow);
        *pReg = (*pReg & ~(BitMask)) | (stmp32 & BitMask);
        return;
    }
    else
    {
        EDDI_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "EDDI_SetBitField32, invalid range: BitLow %d BitHigh %d", BitLow , BitHigh); 
        EDDI_Excp("EDDI_SetBitField32, invalid range", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       EDDI_GetBitField32()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField32NoSwap( LSA_UINT32  const  Val,
                                                             LSA_UINT32  const  BitHigh,
                                                             LSA_UINT32  const  BitLow )
{
    LSA_UINT32         ret;
    LSA_UINT32         BitMask;
    LSA_UINT32  const  range = BitHigh - BitLow;

    if (range == 0)
    {
        ret = Val & EDDI_SINGLE_MASKS_32[BitHigh];
        ret = ret >> BitLow;
        return ret;
    }
    else if (range <= EDDI_BIT_MAX)
    {
        BitMask = EDDI_MULTI_MASKS_32[range] << BitLow;
        ret = Val & BitMask;
        ret = ret >> BitLow;
        return ret;
    }
    else
    {
        return 0;
    }
}

#endif //EDDI_INLINE


/*****************************************************************************/
/*  end of file eddi_bf.c                                                    */
/*****************************************************************************/

