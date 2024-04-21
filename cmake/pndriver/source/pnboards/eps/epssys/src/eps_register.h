#ifndef EPS_REGISTER_H_
#define EPS_REGISTER_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_register.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Register access macros                                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Macros                                                                   */
/*****************************************************************************/

// Cast a pointer to void*. 
// Usage: 
// { uint8_t * ptr; struct A* value = (struct A*) EPS_CAST_TO_VOID_PTR(ptr); }
// { uint8_t * ptr; uint16_t value = (struct A*) EPS_CAST_TO_VOID_PTR(ptr); }
// Note: The cast is done to avoid the lint warning 826: Suspicious pointer-to-pointer conversion (area too small). 
// A Clean solution would be to use void* to pass pointers. Due to API restrictions, this is sometimes not possible.
#define EPS_CAST_TO_VOID_PTR(Value) ((void*) (Value))

#define EPS_REG32_READ(Address) \
			*((volatile LSA_UINT32*)EPS_CAST_TO_VOID_PTR(Address))

#define EPS_REG32_WRITE(Address, Value)  \
			*((volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR (Address)) = (LSA_UINT32)(Value)

#define EPS_REG32_READ_OFFSET(Base, Offset) \
			EPS_REG32_READ(((volatile LSA_UINT8*)EPS_CAST_TO_VOID_PTR(Base)) + (Offset))

#define EPS_REG32_WRITE_OFFSET(Base, Offset, Value)  \
			EPS_REG32_WRITE(((volatile LSA_UINT8*)(Base) + (Offset)), Value)

// clr bits in selected BitMask    
#define EPS_REG32_MSK_CLR_WRITE(Address, BitMask)    \
			EPS_REG32_WRITE(Address, EPS_REG32_READ(Address) & (BitMask));

// set bits in selected BitMask
#define EPS_REG32_MSK_SET_WRITE(Address, BitMask)    \
			EPS_REG32_WRITE(Address, EPS_REG32_READ(Address) | (BitMask));



#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_REGISTER_H_ */

