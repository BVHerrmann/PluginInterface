#ifndef EDDI_BF_H               //reinclude-protection
#define EDDI_BF_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  F i l e               &F: eddi_bf.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/* Bitfield definitions as functions                                         */
/*****************************************************************************/
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField8Bit( LSA_UINT8 const Value,
                                                         LSA_UINT8 const BitHigh,
                                                         LSA_UINT8 const BitLow );

LSA_UINT16 EDD_UPPER_IN_FCT_ATTR  EDDI_GetBF16Host( LSA_UINT16 const Reg,
                                                    LSA_UINT32 const BitHigh,
                                                    LSA_UINT32 const BitLow );

#if !defined (EDDI_INLINE) || defined (EDDI_INLINE_SUFFIX)

#if defined (EDDI_INLINE_SUFFIX)
#define EDDI_INLINE_SUFFIX_ EDDI_INLINE_SUFFIX
#else
#define EDDI_INLINE_SUFFIX_
#endif
#if defined (EDDI_INLINE)
#define EDDI_INLINE_ EDDI_INLINE
#else
#define EDDI_INLINE_
#endif

EDDI_INLINE_ LSA_UINT16 EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField16( LSA_UINT16 const Reg,
                                                                   LSA_UINT32 const BitHigh,
                                                                   LSA_UINT32 const BitLow ) EDDI_INLINE_SUFFIX_;

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
EDDI_INLINE_ LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SetBitField16( LSA_UINT16 * const pReg,
                                                                  LSA_UINT32   const BitHigh,
                                                                  LSA_UINT32   const BitLow,
                                                                  LSA_UINT16   const value ) EDDI_INLINE_SUFFIX_;
#endif

EDDI_INLINE_ LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField32( LSA_UINT32 const Reg,
                                                                    LSA_UINT32 const BitHigh,
                                                                    LSA_UINT32 const BitLow ) EDDI_INLINE_SUFFIX_;

EDDI_INLINE_ LSA_VOID    EDD_UPPER_IN_FCT_ATTR  EDDI_SetBitField32( LSA_UINT32 * const pReg,
                                                                    LSA_UINT32   const BitHigh,
                                                                    LSA_UINT32   const BitLow,
                                                                    LSA_UINT32   const value ) EDDI_INLINE_SUFFIX_;

EDDI_INLINE_ LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_GetBitField32NoSwap( LSA_UINT32 const Val,
                                                                          LSA_UINT32 const BitHigh,
                                                                          LSA_UINT32 const BitLow ) EDDI_INLINE_SUFFIX_;
#undef EDDI_INLINE_SUFFIX_ 
#undef EDDI_INLINE_
#endif //!defined (EDDI_INLINE) || defined (EDDI_INLINE_SUFFIX)

#if defined (EDDI_INLINE)
/*****************************************************************************/
/* Bitfield definitions as macros                                            */
/*****************************************************************************/
EDDI_INLINE LSA_VOID  EDDI_SetBitField32( LSA_UINT32 * const pReg,
                                          LSA_UINT32   const BitHigh,
                                          LSA_UINT32   const BitLow,
                                          LSA_UINT32   const Value )
{                                                                                                                   
    LSA_UINT32 const MaskLocal = ((0xFFFFFFFFUL << (31-BitHigh)) >> ((31-BitHigh+BitLow)))<<BitLow;  
    LSA_UINT32 const RegLocal  = (EDDI_IRTE2HOST32(*pReg) & (~MaskLocal)) | ((Value << BitLow) & MaskLocal);            
    *pReg = EDDI_HOST2IRTE32(RegLocal);                                                                        
}

EDDI_INLINE LSA_UINT32  EDDI_GetBitField32( LSA_UINT32 const Reg,
                                            LSA_UINT32 const BitHigh,
                                            LSA_UINT32 const BitLow )
{                                                                                                                   
    LSA_UINT32 const MaskLocal = 0xFFFFFFFFUL >> (31-BitHigh+BitLow);
    return ((EDDI_HOST2IRTE32(Reg) >> BitLow) & MaskLocal);                                                                        
}

EDDI_INLINE LSA_UINT32 EDDI_GetBitField32NoSwap( LSA_UINT32 const Val,
                                                 LSA_UINT32 const BitHigh,
                                                 LSA_UINT32 const BitLow )
{                                                                                                                   
    LSA_UINT32 const MaskLocal = 0xFFFFFFFFUL >> (31-BitHigh+BitLow);
    return ((Val >> BitLow) & MaskLocal);                                                                        
}

EDDI_INLINE LSA_VOID EDDI_SetBitField16( LSA_UINT16 * const pReg,
                                         LSA_UINT32   const BitHigh,
                                         LSA_UINT32   const BitLow,
                                         LSA_UINT16   const Value )
{                                                                                                                   
    LSA_UINT16 const MaskLocal = ((0xFFFF << (15-BitHigh)) >> ((15-BitHigh+BitLow)))<<BitLow;  
    LSA_UINT16 const RegLocal  = (EDDI_IRTE2HOST16(*pReg) & (~MaskLocal)) | ((Value << BitLow) & MaskLocal);
    *pReg = EDDI_HOST2IRTE16(RegLocal);                                                                        
}

EDDI_INLINE LSA_UINT16 EDDI_GetBitField16( LSA_UINT16 const Reg,
                                           LSA_UINT32 const BitHigh,
                                           LSA_UINT32 const BitLow )
{                                                                                                                   
    LSA_UINT16 const MaskLocal = 0xFFFF >> (15-BitHigh+BitLow);
    return ((EDDI_HOST2IRTE16(Reg) >> BitLow) & MaskLocal);                                                                        
}
#endif //defined (EDDI_INLINE)

#define EDDI_BF16_BIT_MAX                       15
#define EDDI_MASK_16(a, b)                      (0x##a##b)
#define EDDI_BIT_MAX                            31
#define EDDI_MASK_32(a, b, c, d)                (0x##a##b##c##d##L)

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_BF_H


/*****************************************************************************/
/*  end of file eddi_bf.h                                                    */
/*****************************************************************************/
