
#ifndef CLP_CALC64_H                     /* ----- reinclude-protection ----- */
#define CLP_CALC64_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: clp_calc64.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*  64 Bit calculation toolbox                                               */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  2005-12-14  P03.09.00.00_00.01.00.00 lrg  from file edd_calc64.h         */
/*  2008-01-14  P04.00.00.00_00.70.01.02 lrg  #ifdef CLP_CALC64              */
/*              CLP_LIMIT_LSA_INT32_MAX/MIN -> clp_sys.h                     */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

#ifdef CLP_CALC64

typedef struct _CLP_UINT64 
{
  LSA_UINT32   High;
  LSA_UINT32   Low;
} CLP_UINT64;

typedef struct _CLP_INT64 
{
  LSA_INT32    High;
  LSA_UINT32   Low;
} CLP_INT64;

#define CLP_MIN(_a,_b) (((_a) < (_b)) ? (_a) : (_b))

#define CLP_ADD_UI64_AND_UI32_WITH_CARRY(_high, _low, _add_ui32, _carry) \
        { \
          LSA_UINT32 const _min = CLP_MIN(_add_ui32, _low);\
          (_carry) = 0UL;                          \
          (_low)  = (_low) + (_add_ui32);          \
          if ((_low) < _min )                      \
          {                                        \
            (_high)  = (_high) + (1UL);            \
            if ((_high) == 0)                      \
            {                                      \
              _carry = 1UL;                        \
            }                                      \
          }                                        \
        }                                          \

// Bitmask to get MSB-Bit (Sign-Bit) of an int32-Value;
#define CLP_SIGN_BIT31   0x80000000UL

// Check if High_Value is a negative value
#define CLP_INT64_IS_NEGATIVE(_highPart) ((_highPart) & CLP_SIGN_BIT31))

#define CLP_GET_HIGH16_OF_UI32(_ui32)\
        ((_ui32) >> 16) 

#define CLP_GET_LOW16_OF_UI32(_ui32)\
        ((_ui32) & 0x0000FFFFUL)

#define CLP_SHIFT_LEFT_64BIT(_high, _low, _count, _carry) \
        {                                             \
          (_carry) =  (_high)  >> (32UL - (_count));  \
          (_high)  =  (_high)  << (_count);                        \
          (_high)  =  (_high)  | ( (_low) >> (32UL - (_count)) );  \
          (_low)   =  (_low)   << (_count);                        \
        }                                                    

LSA_VOID   CLP_LOCAL_FCT_ATTR clp_Calc64_IntMul_32x32to64 (LSA_INT32   const i32_factor_a, 
                                                           LSA_INT32   const i32_factor_b, 
                                                           CLP_INT64 * const pRes);

LSA_UINT32 CLP_LOCAL_FCT_ATTR clp_Calc64_IntAdd_64add64to64 (CLP_INT64   const a, 
                                                             CLP_INT64   const b, 
                                                             CLP_INT64 * const pRes);

LSA_VOID   CLP_LOCAL_FCT_ATTR clp_Calc64_ui64_div_i32_i64 (CLP_UINT64  const a,    
                                                           LSA_INT32   const b, 
                                                           CLP_INT64 * const pRes);

LSA_INT32  CLP_LOCAL_FCT_ATTR clp_Calc64_Limit_i64_to_i32 (CLP_INT64 i64_Value);

LSA_VOID   CLP_LOCAL_FCT_ATTR clp_Calc64_UintDiv_64div32to64 (CLP_UINT64   const a,    
                                                              LSA_UINT32   const b, 
                                                              CLP_UINT64 * const pRes, 
                                                              LSA_UINT32 * const pModulo);
                                
LSA_VOID   CLP_LOCAL_FCT_ATTR clp_Calc64_UintMul_32mul32to64(LSA_UINT32   const a, 
                                                             LSA_UINT32   const b, 
                                                             CLP_UINT64 * const pRes);

#endif  /* of CLP_CALC64 */
/*****************************************************************************/
/*  end of file clp_calc64.h                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of CLP_CALC64_H */
