
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

/* System includes */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/eventfd.h>				/* eventfd  */
#include <linux/sched.h>				/* current->pid for sending events with eventfd in kernel */
#include <linux/interrupt.h>
#include <linux/pid.h>					/* pid for sending events with eventfd in kernel */
#include <linux/fdtable.h>				/* for sending events with eventfd in kernel */
#include <linux/rcupdate.h>				/* locking when looking up user eventfd */



/*********************************************************************
   Defines
*********************************************************************/

// return values
#define	LINUX_IRTE_DRV_RET_OK		0
#define	LINUX_IRTE_DRV_RET_ERR		-1

// IOCTLs
#define	LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_ENABLE			0
#define	LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_DISABLE			1
#define	LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_ENABLE		2
#define	LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_DISABLE	3

#define LINUX_IRTE_DRV_IOCTL_SW_IRQ0_ENABLE					4
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ1_ENABLE					5
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ2_ENABLE					6
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ3_ENABLE					7
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ4_ENABLE					8
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ5_ENABLE					9
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ6_ENABLE					10
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ7_ENABLE					11
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ8_ENABLE					12
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ9_ENABLE					13
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ10_ENABLE				14
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ11_ENABLE				15
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ12_ENABLE				16
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ13_ENABLE				17
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ14_ENABLE				18
#define LINUX_IRTE_DRV_IOCTL_SW_IRQ15_ENABLE				19

#define LINUX_IRTE_DRV_IOCTL_SW_IRQ_DISABLE					20

// needed to install the driver
#define LINUX_IRTE_DRV_CLASS_NAME	"linux_irte_drv_cls"
#define LINUX_IRTE_DRV_DEVICE_NAME	"linux_irte_drv"

/* Check IRQ Nums against Asic Specs */
#define LINUX_IRTE_DRV_IRQ_NUM0_SP_SOC1_BIT  8
#define LINUX_IRTE_DRV_IRQ_NUM1_SP_SOC1_BIT  9

#define LINUX_IRTE_DRV_ICU_BASE                 32
#define LINUX_IRTE_DRV_IRQ_NUM0_SP_SOC1         (LINUX_IRTE_DRV_ICU_BASE + 8)
#define LINUX_IRTE_DRV_IRQ_NUM1_SP_SOC1         (LINUX_IRTE_DRV_ICU_BASE + 9)
#define LINUX_IRTE_DRV_SW_IRQ_0                 (LINUX_IRTE_DRV_ICU_BASE + 144) // SW_INT_0 of 16 SW Interrupts

#define LINUX_IRTE_DRV_IRQ_NUM0_DEV_NAME	"linux_irte_drv_irq0"
#define LINUX_IRTE_DRV_IRQ_NUM1_DEV_NAME	"linux_irte_drv_irq1"
#define LINUX_IRTE_DRV_SW_IRQ_DEV_NAMES   {	"linux_irte_drv_sw_irq0", \
											"linux_irte_drv_sw_irq1", \
											"linux_irte_drv_sw_irq2", \
											"linux_irte_drv_sw_irq3", \
											"linux_irte_drv_sw_irq4", \
											"linux_irte_drv_sw_irq5", \
											"linux_irte_drv_sw_irq6", \
											"linux_irte_drv_sw_irq7", \
											"linux_irte_drv_sw_irq8", \
											"linux_irte_drv_sw_irq9", \
											"linux_irte_drv_sw_irq10", \
											"linux_irte_drv_sw_irq11", \
											"linux_irte_drv_sw_irq12", \
											"linux_irte_drv_sw_irq13", \
											"linux_irte_drv_sw_irq14", \
											"linux_irte_drv_sw_irq15" }

// asic specific defines
#define LINUX_IRTE_DRV_IRTE_BASE_ADDR					0x1D200000
#define LINUX_IRTE_DRV_IRTE_SIZE						0x00200000


// for EPS_PN_DRV_ISOCHRONOUS_IRQ_ICU_ENABLE
#define LINUX_IRTE_DRV_ISO_IRT_NEWCYCLE_MASK         0x01 	// mask for interrupt request of newCycle in Interrupt-MaskRegister-IRT-IRQ1
#define LINUX_IRTE_DRV_ISO_IRT_TRANSFEREND_MASK      0x4 	// mask for interrupt request of TransferEnd in Interrupt-MaskRegister-IRT-IRQ1
#define LINUX_IRTE_DRV_ISO_IRT_TRANSFEREND_BIT       2   	// bit number of TransferEnd in Interrupt-MaskRegister-IRT-IRQ1
#define LINUX_IRTE_DRV_ISO_EOI_VALUE                 0xF 	// value for EOI-IRQ1-Register

// read/write access to IRTE register
#define LINUX_IRTE_DRV_REG32_READ(address)           (   *(volatile unsigned int* const) ((unsigned int)address) )
#define LINUX_IRTE_DRV_REG32_WRITE(address, value)   ( ( *(volatile unsigned int* const) ((unsigned int)address) ) = value)

/*********************************************************************
   Forward declarations
*********************************************************************/
static int  __init linux_irte_drv_module_init(void);
static void __exit linux_irte_drv_module_exit(void);

static void linux_irte_drv_init_global_data(void);
static int  linux_irte_drv_get_eventfd_file(int lEventfdIn, struct file** fpOut);
static void linux_irte_drv_free_irqs(void);

int linux_irte_drv_open(struct inode *inodep, struct file *filep);
int linux_irte_drv_close(struct inode *inodep, struct file *filep);
long linux_irte_drv_ioctl(struct file *file, unsigned int ioctl_enum, unsigned long ioctl_param);

/*********************************************************************
   Module settings
*********************************************************************/
module_init(linux_irte_drv_module_init);
module_exit(linux_irte_drv_module_exit);
MODULE_LICENSE("GPL");

/*********************************************************************
   Driver functions
*********************************************************************/

static const struct file_operations linux_irte_drv_fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= linux_irte_drv_ioctl,
	.open				= linux_irte_drv_open,
	.release 			= linux_irte_drv_close,
};

static struct miscdevice linux_irte_drv_miscdev = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = LINUX_IRTE_DRV_DEVICE_NAME,
    .fops   = &linux_irte_drv_fops,
};

/*********************************************************************
   Global Variables
*********************************************************************/
static struct class*  	linux_irte_drv_class  		= NULL;
static struct device* 	linux_irte_drv_char_device 	= NULL;
static int				linux_irte_drv_open_cnt 	= 0;
static const char*		linux_irte_drv_sw_irq_names[]	= LINUX_IRTE_DRV_SW_IRQ_DEV_NAMES;

struct
{
	unsigned int        uIrqNum0_SP;
	unsigned int        uIrqNum1_SP;
	unsigned int        uIrq_SW[16];

	struct file*		pEventFile0;
	struct file*		pEventFile1;
	struct file*		pEventFileSW[16];

	unsigned char*      pBase_irte;

	struct
	{
		unsigned int*     Irr_Irt;
		unsigned int*     Irr_Nrt;
		unsigned int*     Mask_Irt;
		unsigned int*     Mask_Nrt;
		unsigned int*     Ir_Irt;
		unsigned int*     Ir_Nrt;
		unsigned int*     Ack_Irt;
		unsigned int*     Ack_Nrt;
		unsigned int*     Eoi;
	} IcuAsicSP0;

	struct
	{
		unsigned int*     Irr_Irt;
		unsigned int*     Irr;
		unsigned int*     Mask_Irt;
		unsigned int*     Mask_Nrt;
		unsigned int*     Ir;
		unsigned int*     Ack_Irt;
		unsigned int*     Ack_Nrt;
		unsigned int*     Eoi;
	} IcuAsicSP1;

} g_IrteDrvData;

/*********************************************************************
   Functions
*********************************************************************/

/**
 * Module init - will be called when driver is loaded
 *
 * @param void
 * @return LINUX_IRTE_DRV_RET_ERR
 * @return LINUX_IRTE_DRV_RET_OK
 */
static int __init linux_irte_drv_module_init(void)
{
	int ret_val = 0;

	// Register the misc device
	ret_val = misc_register(&linux_irte_drv_miscdev);
	if (ret_val)
	{
	  printk("could not register misc device\n");
	  return ret_val;
	}

	// Register the device class
	linux_irte_drv_class = class_create(THIS_MODULE, LINUX_IRTE_DRV_CLASS_NAME);

	if (IS_ERR(linux_irte_drv_class))
		// Check for error and clean up if there is
	{
		misc_deregister(&linux_irte_drv_miscdev);
		printk(KERN_ALERT "linux_irte_drv: Failed to register device class\n");
		return LINUX_IRTE_DRV_RET_ERR;
	}

	// Register the device driver
	linux_irte_drv_char_device = device_create(linux_irte_drv_class, NULL, MISC_MAJOR, NULL, LINUX_IRTE_DRV_DEVICE_NAME);

	if (IS_ERR(linux_irte_drv_char_device))
		// Clean up if there is an error
	{
		class_destroy(linux_irte_drv_class);
		misc_deregister(&linux_irte_drv_miscdev);
		printk(KERN_ALERT "linux_irte_drv: Failed to create the device\n");
		return LINUX_IRTE_DRV_RET_ERR;
	}

	// setting up the data for the driver
	linux_irte_drv_init_global_data();

	printk("linux_irte_drv: Install done (Major: %d)\n", MISC_MAJOR);
	return LINUX_IRTE_DRV_RET_OK;
}

/**
 * Module exit - will be called when driver is unloaded
 *
 * @param void
 * @return void
 */
static void __exit linux_irte_drv_module_exit(void)
{
	// allocate our interrupt lines
	linux_irte_drv_free_irqs();

	device_destroy		(linux_irte_drv_class, MISC_MAJOR);
	class_unregister	(linux_irte_drv_class);
	class_destroy		(linux_irte_drv_class);
	misc_deregister		(&linux_irte_drv_miscdev);

	printk("linux_irte_drv: Uninstall done\n");
}

/**
 * Init global data
 *
 * @param void
 * @return void
 */
static void linux_irte_drv_init_global_data(void)
{
	int i;
	// mark IRQ0/IRQ1 as unused
	g_IrteDrvData.uIrqNum0_SP 			= 0;
	g_IrteDrvData.uIrqNum1_SP 			= 0;
	g_IrteDrvData.pEventFile0			= NULL;
	g_IrteDrvData.pEventFile1			= NULL;
	
	// mark SW IRQs as unused
	for(i = 0; i < 16; i++)
	{
		g_IrteDrvData.uIrq_SW[i] 		= 0;
		g_IrteDrvData.pEventFileSW[i]	= NULL;
	}

	g_IrteDrvData.pBase_irte 			= ioremap(LINUX_IRTE_DRV_IRTE_BASE_ADDR, LINUX_IRTE_DRV_IRTE_SIZE);

	g_IrteDrvData.IcuAsicSP0.Ack_Irt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17438);
	g_IrteDrvData.IcuAsicSP0.Ack_Nrt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x1743C);
	g_IrteDrvData.IcuAsicSP0.Eoi        = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17448);
	g_IrteDrvData.IcuAsicSP0.Ir_Irt     = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17428);
	g_IrteDrvData.IcuAsicSP0.Ir_Nrt     = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x1742C);
	g_IrteDrvData.IcuAsicSP0.Irr_Irt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17440);
	g_IrteDrvData.IcuAsicSP0.Irr_Nrt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17444);
	g_IrteDrvData.IcuAsicSP0.Mask_Irt   = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17018);
	g_IrteDrvData.IcuAsicSP0.Mask_Nrt   = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x1701C);

	g_IrteDrvData.IcuAsicSP1.Ack_Irt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17438);
	g_IrteDrvData.IcuAsicSP1.Ack_Nrt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x1743C);
	g_IrteDrvData.IcuAsicSP1.Eoi        = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x1744C);
	g_IrteDrvData.IcuAsicSP1.Ir         = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17434);
	g_IrteDrvData.IcuAsicSP1.Irr_Irt    = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17440);
	g_IrteDrvData.IcuAsicSP1.Irr        = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17444);
	g_IrteDrvData.IcuAsicSP1.Mask_Irt   = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17020);
	g_IrteDrvData.IcuAsicSP1.Mask_Nrt   = (unsigned int*)(g_IrteDrvData.pBase_irte + 0x17024);
}

/**
 * Driver open - will be called when driver is opened by an application
 *
 * @return LINUX_IRTE_DRV_RET_OK
 */
int linux_irte_drv_open(struct inode *inodep, struct file *filep)
{
    // count opens of the driver
    linux_irte_drv_open_cnt++;
    printk("linux_irte_drv: %i OPENED!\n", linux_irte_drv_open_cnt);
    
    return LINUX_IRTE_DRV_RET_OK;
}

/**
 * Driver close - will be called when driver is closed by an application
 *
 * @return LINUX_IRTE_DRV_RET_OK
 */
int linux_irte_drv_close(struct inode *inodep, struct file *filep)
{
	linux_irte_drv_open_cnt--;

	printk("linux_irte_drv: CLOSED!\n");

	// if it is closed more than it was opened, reset the global counter
	if(linux_irte_drv_open_cnt < 0)
		linux_irte_drv_open_cnt = 0;

	return LINUX_IRTE_DRV_RET_OK;
}

/**
 * ISR for standard interrupts
 *
 * @return IRQ_HANDLED
 */
irq_handler_t linux_irte_drv_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	struct eventfd_ctx* pEventfdCtx     = NULL;

	if (g_IrteDrvData.pEventFile0) {
		// get eventfd context
		pEventfdCtx = eventfd_ctx_fileget(g_IrteDrvData.pEventFile0);

	    // Increment userspace program's eventfd's counter by eEventIn
	    eventfd_signal (pEventfdCtx, 1);
	    eventfd_ctx_put(pEventfdCtx);
	}

	return (irq_handler_t) IRQ_HANDLED;
}

/**
 * ISR for ISO interrupts
 *
 * @return IRQ_HANDLED
 */
irq_handler_t linux_irte_drv_iso_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	struct eventfd_ctx* pEventfdCtx     = NULL;

	if (g_IrteDrvData.pEventFile1) {
		// get eventfd context
		pEventfdCtx = eventfd_ctx_fileget(g_IrteDrvData.pEventFile1);

	    // Increment userspace program's eventfd's counter by eEventIn
	    eventfd_signal (pEventfdCtx, 1);
	    eventfd_ctx_put(pEventfdCtx);
	}

	return (irq_handler_t) IRQ_HANDLED;
}

/**
 * ISR for SW interrupts
 *
 * @return IRQ_HANDLED
 */
irq_handler_t linux_irte_drv_sw_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	struct eventfd_ctx* pEventfdCtx		= NULL;
	int					lSWIntNr		= irq - LINUX_IRTE_DRV_SW_IRQ_0;
    
	if(lSWIntNr >= 0 && lSWIntNr < 16) {
		if(g_IrteDrvData.pEventFileSW[lSWIntNr]) {
			// get eventfd context
			pEventfdCtx = eventfd_ctx_fileget(g_IrteDrvData.pEventFileSW[lSWIntNr]);
    
			// Increment userspace program's eventfd's counter by eEventIn
			eventfd_signal (pEventfdCtx, 1);
			eventfd_ctx_put(pEventfdCtx);
        }
    }

    return (irq_handler_t) IRQ_HANDLED;
}


/**
 * IOCTL function
 *
 * @param ioctl_enum IOCTL which will be called
 * @param ioctl_param Parameter passed to the IOCTL
 *
 * @return LINUX_IRTE_DRV_RET_ERR
 * @return LINUX_IRTE_DRV_RET_OK
 */
long linux_irte_drv_ioctl(	struct file 	*file,
							unsigned int 	ioctl_enum,
							unsigned long 	ioctl_param)
{
int ioctl_ret = LINUX_IRTE_DRV_RET_OK;

	switch (ioctl_enum)
	{
		case LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_ENABLE:
		{
			int lEventfd = 0;

			lEventfd = (int) ioctl_param;
			
		    // IRQ not used yet?
		    if(g_IrteDrvData.uIrqNum0_SP == 0)
		    {
				// get the eventfd
				ioctl_ret = linux_irte_drv_get_eventfd_file(lEventfd, &g_IrteDrvData.pEventFile0);
	
				if(ioctl_ret)
				{
					printk(KERN_ALERT "linux_irte_drv: can't get kernel File for lEventfd %d - errcode %d\n", lEventfd, ioctl_ret);
				}
				else
				{
					// mark IRQ as used
					g_IrteDrvData.uIrqNum0_SP = LINUX_IRTE_DRV_IRQ_NUM0_SP_SOC1;
					
					ioctl_ret = request_irq(    g_IrteDrvData.uIrqNum0_SP,        	// interrupt number
												(irq_handler_t)linux_irte_drv_isr, 	// ISR
												IRQF_NO_SUSPEND,                   	// fast interrupts: are executed with interrupts disabled
												LINUX_IRTE_DRV_IRQ_NUM0_DEV_NAME,  	// name of interrupt owner in /proc/interrupts, make name unique
												NULL);                             	// identifier to deregister shared interrupts               
					if(ioctl_ret)
					{
						printk(KERN_ALERT "linux_irte_drv: can't get g_IrteDrvData.uIrqNum0_SP: %i - errcode %d , EIO %d , EINVAL %d\n", g_IrteDrvData.uIrqNum0_SP, ioctl_ret, EIO, EINVAL);
						g_IrteDrvData.uIrqNum0_SP = 0;
					}
				}
		    }		    
		    else
		    {
		        ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
		        printk(KERN_ALERT "linux_irte_drv: g_IrteDrvData.uIrqNum0_SP %i is already enabled\n", g_IrteDrvData.uIrqNum0_SP);
		    }
			
			break;
		}

		case LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_DISABLE:
		{
		    // IRQ used?
            if(g_IrteDrvData.uIrqNum0_SP == LINUX_IRTE_DRV_IRQ_NUM0_SP_SOC1)
		    {
		        // release used IRQ
		        free_irq(g_IrteDrvData.uIrqNum0_SP, NULL);

		        // mark IRQ as unused
		        g_IrteDrvData.uIrqNum0_SP = 0;
		        g_IrteDrvData.pEventFile0 = NULL;
		    }
		    else
		    {
		        ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
		        printk(KERN_ALERT "linux_irte_drv: g_IrteDrvData.uIrqNum0_SP %i is not enabled\n", g_IrteDrvData.uIrqNum0_SP);
		    }
            
			break;
		}

		case LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_ENABLE:
		{
			int lEventfd = 0;

			lEventfd = (int) ioctl_param;

		    // IRQ not used yet?
		    if(g_IrteDrvData.uIrqNum1_SP == 0)
		    {
				// get the eventfd
				ioctl_ret = linux_irte_drv_get_eventfd_file(lEventfd, &g_IrteDrvData.pEventFile1);
	
				if(ioctl_ret)
				{
					printk(KERN_ALERT "linux_irte_drv: can't get kernel File for lEventfd %d - errcode %d\n", lEventfd, ioctl_ret);
				}
				else
				{
					// mark IRQ as used
					g_IrteDrvData.uIrqNum1_SP = LINUX_IRTE_DRV_IRQ_NUM1_SP_SOC1;
					
					ioctl_ret = request_irq(	g_IrteDrvData.uIrqNum1_SP,        		// interrupt number
												(irq_handler_t)linux_irte_drv_iso_isr, 	// ISR
												IRQF_NO_SUSPEND,                   		// fast interrupts: are executed with interrupts disabled
												LINUX_IRTE_DRV_IRQ_NUM1_DEV_NAME,  		// name of interrupt owner in /proc/interrupts, make name unique
												NULL);                             		// identifier to deregister shared interrupts               
					if(ioctl_ret)
					{
						printk(KERN_ALERT "linux_irte_drv: can't get g_IrteDrvData.uIrqNum1_SP: %i - errcode %d , EIO %d , EINVAL %d\n", g_IrteDrvData.uIrqNum1_SP, ioctl_ret, EIO, EINVAL);
						g_IrteDrvData.uIrqNum1_SP = 0;
					}
                    else
                    {
                    	unsigned int RegVal = 0;
	
						// read the current interrupt request register
						RegVal = LINUX_IRTE_DRV_REG32_READ(g_IrteDrvData.IcuAsicSP1.Mask_Irt);
						// set the newCycle
						RegVal |= LINUX_IRTE_DRV_ISO_IRT_NEWCYCLE_MASK;
						// and write the value back to register
						LINUX_IRTE_DRV_REG32_WRITE(g_IrteDrvData.IcuAsicSP1.Mask_Irt, RegVal);
						// write the EOI value
						LINUX_IRTE_DRV_REG32_WRITE(g_IrteDrvData.IcuAsicSP1.Eoi, LINUX_IRTE_DRV_ISO_EOI_VALUE);
					}
				}	 
		    }
		    else
		    {
		        ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
		        printk(KERN_ALERT "linux_irte_drv: g_IrteDrvData.uIrqNum1_SP %i is already enabled\n", g_IrteDrvData.uIrqNum1_SP);
		    }

			break;
		}

		case LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_DISABLE:
		{
			unsigned int RegVal = 0;

		    // IRQ used?
            if(g_IrteDrvData.uIrqNum1_SP == LINUX_IRTE_DRV_IRQ_NUM1_SP_SOC1)
		    {
		        // release used IRQ
		        free_irq(g_IrteDrvData.uIrqNum1_SP, NULL);

		        // mark IRQ as unused
		        g_IrteDrvData.uIrqNum1_SP = 0;
		        g_IrteDrvData.pEventFile1 = NULL;
		        
				// stop the TransferEnd interrupt (clear the interrupt request)

				// read the current interrupt request register
				RegVal = LINUX_IRTE_DRV_REG32_READ(g_IrteDrvData.IcuAsicSP1.Mask_Irt);
				// clear the newCycle
				RegVal &= ~(LINUX_IRTE_DRV_ISO_IRT_NEWCYCLE_MASK);
				// and write the value back to register
				LINUX_IRTE_DRV_REG32_WRITE(g_IrteDrvData.IcuAsicSP1.Mask_Irt, RegVal);
				// clear the EOI value
				LINUX_IRTE_DRV_REG32_WRITE(g_IrteDrvData.IcuAsicSP1.Eoi, 0x0);
		    }
		    else
		    {
		        ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
		        printk(KERN_ALERT "linux_irte_drv: g_IrteDrvData.uIrqNum1_SP %i is not enabled\n", g_IrteDrvData.uIrqNum1_SP);
		    }

			break;
		}
		
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ0_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ1_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ2_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ3_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ4_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ5_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ6_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ7_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ8_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ9_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ10_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ11_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ12_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ13_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ14_ENABLE:
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ15_ENABLE:
		{
		    int lSWIntNr    = (int) (ioctl_enum - LINUX_IRTE_DRV_IOCTL_SW_IRQ0_ENABLE);
		    int lEventfd    = (int) ioctl_param;
		    
		    // valid lSWIntNr and SW IRQ not used yet?
		    if( (lSWIntNr >= 0 && lSWIntNr < 16) &&
		         g_IrteDrvData.uIrq_SW[lSWIntNr] == 0)
		    {
		        // get the eventfd
		        ioctl_ret = linux_irte_drv_get_eventfd_file(lEventfd, &g_IrteDrvData.pEventFileSW[lSWIntNr]);
		        
		        if(ioctl_ret)
		        {
		            printk(KERN_ALERT "linux_irte_drv: can't get kernel File for lEventfd %d - errcode %d\n", lEventfd, ioctl_ret);
		        }
		        else
		        {
		            // mark SW IRQ as used
		            g_IrteDrvData.uIrq_SW[lSWIntNr] = (LINUX_IRTE_DRV_SW_IRQ_0 + lSWIntNr);
		            
		            ioctl_ret = request_irq(    g_IrteDrvData.uIrq_SW[lSWIntNr],        // interrupt number
		                                        (irq_handler_t)linux_irte_drv_sw_isr,   // ISR
		                                        IRQF_NO_SUSPEND,                        // fast interrupts: are executed with interrupts disabled
		                                        linux_irte_drv_sw_irq_names[lSWIntNr],  // name of interrupt owner in /proc/interrupts, make name unique
		                                        NULL);                                  // identifier to deregister shared interrupts
		                            
		            if(ioctl_ret)
		            {
		                printk(KERN_ALERT "linux_irte_drv: can't get g_IrteDrvData.uIrq_SW: %i - errcode %d , EIO %d , EINVAL %d\n", g_IrteDrvData.uIrq_SW[lSWIntNr], ioctl_ret, EIO, EINVAL);
		                g_IrteDrvData.uIrq_SW[lSWIntNr] = 0;
		            }
		        }
		    }
		    else
		    {
		        ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
		        printk(KERN_ALERT "linux_irte_drv: SW INT %i is not vaild or already enabled\n", lSWIntNr);
		    }
		    break;
		}
        
		case LINUX_IRTE_DRV_IOCTL_SW_IRQ_DISABLE:
		{
		    int lSWIntNr;

		    lSWIntNr = (int) ioctl_param;
		    
		    // valid lSWIntNr and SW IRQ used?
		    if( (lSWIntNr >= 0 && lSWIntNr < 16) &&
		         g_IrteDrvData.uIrq_SW[lSWIntNr] == (LINUX_IRTE_DRV_SW_IRQ_0 + lSWIntNr))
		    {
		        // release used SW IRQ
		        free_irq(g_IrteDrvData.uIrq_SW[lSWIntNr], NULL);

		        // mark SW IRQ as unused
		        g_IrteDrvData.uIrq_SW[lSWIntNr] = 0;
		        g_IrteDrvData.pEventFileSW[lSWIntNr] = NULL;
		    }
		    else
		    {
		        ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
		        printk(KERN_ALERT "linux_irte_drv: SW INT %i is not vaild or not enabled\n", lSWIntNr);
		    }
		    break;
		}

		default:
		{
			ioctl_ret = LINUX_IRTE_DRV_RET_ERR;
			break;
		}
	}

	return ioctl_ret;
}

/**
 * Gets a file pointer from a eventfd
 *
 * @param lEventfdIn Number of eventfd
 * @param fpOut file pointer to the eventfd given
 *
 * @return LINUX_IRTE_DRV_RET_ERR
 * @return LINUX_IRTE_DRV_RET_OK
 */
static int linux_irte_drv_get_eventfd_file(int lEventfdIn, struct file** fpOut)
{
	int 			ret_val 		= LINUX_IRTE_DRV_RET_OK;
	struct file* 	pEventfdFile    = NULL;     // eventfd's file struct

    if(lEventfdIn != -1)
    {
		// fcheck_files has to be protected by rcu or file lock!
		rcu_read_lock();

		// Resolve pointer to the userspace program's eventfd's file struct
		pEventfdFile = fcheck_files(    current->files,
										lEventfdIn);
		rcu_read_unlock();

		if(pEventfdFile != NULL)
		{
			// Return resolved pointer to the userspace program's eventfd's file struct
			*fpOut = pEventfdFile;
		}
		else
		{
			ret_val = LINUX_IRTE_DRV_RET_ERR;
		}
    }

    return ret_val;
}

/**
 * Frees the needed interrupts
 *
 * @return void
 */
static void linux_irte_drv_free_irqs(void)
{
	int i;
	// release our IRQs
	if(g_IrteDrvData.uIrqNum0_SP == LINUX_IRTE_DRV_IRQ_NUM0_SP_SOC1)
	{
		free_irq(g_IrteDrvData.uIrqNum0_SP, NULL);
		// mark IRQ as unused
		g_IrteDrvData.uIrqNum0_SP = 0;
		g_IrteDrvData.pEventFile0 = NULL;
	}
	if(g_IrteDrvData.uIrqNum1_SP == LINUX_IRTE_DRV_IRQ_NUM1_SP_SOC1)
	{
		free_irq(g_IrteDrvData.uIrqNum1_SP, NULL);
		// mark IRQ as unused
		g_IrteDrvData.uIrqNum1_SP = 0;
		g_IrteDrvData.pEventFile1 = NULL;
	}

	for(i = 0; i < 16; i++)
	{
		// release used SW IRQs
		if(g_IrteDrvData.uIrq_SW[i] == (LINUX_IRTE_DRV_SW_IRQ_0 + i))
		{
			free_irq(g_IrteDrvData.uIrq_SW[i], NULL);
			// mark SW IRQ as unused
			g_IrteDrvData.uIrq_SW[i] = 0;
			g_IrteDrvData.pEventFileSW[i] = NULL;
		}
	}
}
