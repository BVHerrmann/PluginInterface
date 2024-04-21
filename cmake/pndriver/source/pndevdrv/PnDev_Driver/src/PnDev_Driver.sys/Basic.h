/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License version 2     */
/* as published by the Free Software Foundation; or, when distributed        */
/* separately from the Linux kernel or incorporated into other               */
/* software packages, subject to the following license:                      */
/*                                                                           */
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: Basic.h                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for basic functions
//
//****************************************************************************/

#ifndef __Basic_h__
#define __Basic_h__

	#ifdef __cplusplus		// if C++ compiler: Use C linkage
	extern "C"
	{
	#endif

	//########################################################################
	//  Defines
	//########################################################################

	// const in "C" not available

	#define MAXUINT32   ((UINT32)~((UINT32)0))									// BaseTsd.h

	//------------------------------------------------------------------------
	#define PNDEV_UINT32_SET_BIT_0	(1u<<0)
	#define	PNDEV_UINT32_SET_BIT_1	(1u<<1)
	#define	PNDEV_UINT32_SET_BIT_2	(1u<<2)
	#define	PNDEV_UINT32_SET_BIT_3	(1u<<3)
	#define	PNDEV_UINT32_SET_BIT_4	(1u<<4)
	#define	PNDEV_UINT32_SET_BIT_5	(1u<<5)
	#define	PNDEV_UINT32_SET_BIT_6	(1u<<6)
	#define	PNDEV_UINT32_SET_BIT_7	(1u<<7)
	#define	PNDEV_UINT32_SET_BIT_8	(1u<<8)
	#define	PNDEV_UINT32_SET_BIT_9	(1u<<9)
	#define	PNDEV_UINT32_SET_BIT_10	(1u<<10)
	#define	PNDEV_UINT32_SET_BIT_11	(1u<<11)
	#define	PNDEV_UINT32_SET_BIT_12	(1u<<12)
	#define	PNDEV_UINT32_SET_BIT_13	(1u<<13)
	#define	PNDEV_UINT32_SET_BIT_14	(1u<<14)
	#define	PNDEV_UINT32_SET_BIT_15	(1u<<15)
	#define	PNDEV_UINT32_SET_BIT_16	(1u<<16)
	#define	PNDEV_UINT32_SET_BIT_17	(1u<<17)
	#define	PNDEV_UINT32_SET_BIT_18	(1u<<18)
	#define	PNDEV_UINT32_SET_BIT_19	(1u<<19)
	#define	PNDEV_UINT32_SET_BIT_20	(1u<<20)
	#define	PNDEV_UINT32_SET_BIT_21	(1u<<21)
	#define	PNDEV_UINT32_SET_BIT_22	(1u<<22)
	#define	PNDEV_UINT32_SET_BIT_23	(1u<<23)
	#define	PNDEV_UINT32_SET_BIT_24	(1u<<24)
	#define	PNDEV_UINT32_SET_BIT_25	(1u<<25)
	#define	PNDEV_UINT32_SET_BIT_26	(1u<<26)
	#define	PNDEV_UINT32_SET_BIT_27	(1u<<27)
	#define	PNDEV_UINT32_SET_BIT_28	(1u<<28)
	#define	PNDEV_UINT32_SET_BIT_29	(1u<<29)
	#define	PNDEV_UINT32_SET_BIT_30	(1u<<30)
	#define	PNDEV_UINT32_SET_BIT_31	(1u<<31)

	//------------------------------------------------------------------------
	#define	PNDEV_UINT32_CLR_BIT_0	~(1u<<0)
	#define	PNDEV_UINT32_CLR_BIT_1	~(1u<<1)
	#define	PNDEV_UINT32_CLR_BIT_2	~(1u<<2)
	#define	PNDEV_UINT32_CLR_BIT_3	~(1u<<3)
	#define	PNDEV_UINT32_CLR_BIT_4	~(1u<<4)
	#define	PNDEV_UINT32_CLR_BIT_5	~(1u<<5)
	#define	PNDEV_UINT32_CLR_BIT_6	~(1u<<6)
	#define	PNDEV_UINT32_CLR_BIT_7	~(1u<<7)
	#define	PNDEV_UINT32_CLR_BIT_8	~(1u<<8)
	#define	PNDEV_UINT32_CLR_BIT_9	~(1u<<9)
	#define	PNDEV_UINT32_CLR_BIT_10	~(1u<<10)
	#define	PNDEV_UINT32_CLR_BIT_11	~(1u<<11)
	#define	PNDEV_UINT32_CLR_BIT_12	~(1u<<12)
	#define	PNDEV_UINT32_CLR_BIT_13	~(1u<<13)
	#define	PNDEV_UINT32_CLR_BIT_14	~(1u<<14)
	#define	PNDEV_UINT32_CLR_BIT_15	~(1u<<15)
	#define	PNDEV_UINT32_CLR_BIT_16	~(1u<<16)
	#define	PNDEV_UINT32_CLR_BIT_17	~(1u<<17)
	#define	PNDEV_UINT32_CLR_BIT_18	~(1u<<18)
	#define	PNDEV_UINT32_CLR_BIT_19	~(1u<<19)
	#define	PNDEV_UINT32_CLR_BIT_20	~(1u<<20)
	#define	PNDEV_UINT32_CLR_BIT_21	~(1u<<21)
	#define	PNDEV_UINT32_CLR_BIT_22	~(1u<<22)
	#define	PNDEV_UINT32_CLR_BIT_23	~(1u<<23)
	#define	PNDEV_UINT32_CLR_BIT_24	~(1u<<24)
	#define	PNDEV_UINT32_CLR_BIT_25	~(1u<<25)
	#define	PNDEV_UINT32_CLR_BIT_26	~(1u<<26)
	#define	PNDEV_UINT32_CLR_BIT_27	~(1u<<27)
	#define	PNDEV_UINT32_CLR_BIT_28	~(1u<<28)
	#define	PNDEV_UINT32_CLR_BIT_29	~(1u<<29)
	#define	PNDEV_UINT32_CLR_BIT_30	~(1u<<30)
	#define	PNDEV_UINT32_CLR_BIT_31	~(1u<<31)

	//########################################################################
	//  Structures
	//########################################################################

	typedef struct _uLIST_HEADER
	{
		void*	pNext;
		void*	pPrev;
	}
	uLIST_HEADER;

	//########################################################################
	//  Function prototypes
	//########################################################################

	void	fnInitList(					uLIST_HEADER*	pListIn);
	BOOLEAN fnListEmpty(				uLIST_HEADER*	pListIn);
	void	fnPutBlkToList(				uLIST_HEADER*	pListIn,
						   PFD_MEM_ATTR	uLIST_HEADER*	pBlockIn);
	void	fnRemoveBlkFromList(		uLIST_HEADER*	pListIn,
										uLIST_HEADER*	pBlockIn);
	void*	fnGetFirstBlkFromList(		uLIST_HEADER*	pListIn);
	BOOLEAN	fnBlkAtList(				uLIST_HEADER*	pListIn,
										uLIST_HEADER*	pBlockIn);
	//------------------------------------------------------------------------
	UINT64	fnGetAlignedValue(			UINT64			lValueIn,
										UINT32			lAlignment);
	//------------------------------------------------------------------------
	UINT32	fnCalculateCRC32(			UINT8*			pData,
										UINT32          lLength);

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ compiler: End of C linkage
	}
	#endif

#endif
