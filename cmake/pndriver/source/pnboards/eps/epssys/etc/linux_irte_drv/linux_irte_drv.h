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
/*  C o m p o n e n t     &C: eps                                       :C&  */
/*                                                                           */
/*  F i l e               &F: linux_irte_drv.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Linux IRTE driver for SoC1
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
/*****************************************************************************/
#ifndef LINUX_IRTE_DRV_H_
#define LINUX_IRTE_DRV_H_

/*********************************************************************
   Defines
*********************************************************************/
#define LINUX_IRTE_DRV_NAME     	"/dev/linux_irte_drv"

// return values
#define	LINUX_IRTE_DRV_RET_OK		0
#define	LINUX_IRTE_DRV_RET_ERR		-1

// IOCTLs
#define	LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_ENABLE			0
#define	LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_DISABLE			1
#define	LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_ENABLE		2
#define	LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_DISABLE	3

#define LINUX_IRTE_DRV_IOCTL_SW_IRQ0_ENABLE                 4
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ1_ENABLE                 5
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ2_ENABLE                 6
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ3_ENABLE                 7
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ4_ENABLE                 8
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ5_ENABLE                 9
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ6_ENABLE                 10
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ7_ENABLE                 11
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ8_ENABLE                 12
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ9_ENABLE                 13
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ10_ENABLE                14
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ11_ENABLE                15
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ12_ENABLE                16
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ13_ENABLE                17
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ14_ENABLE                18
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ15_ENABLE                19

#define LINUX_IRTE_DRV_IOCTL_SW_IRQ_DISABLE                 20

#endif /*LINUX_IRTE_DRV_H_*/
