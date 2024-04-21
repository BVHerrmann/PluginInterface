# if KERNELRELEASE is not defined, we've been called directly from the command line.
# Invoke the kernel build system.
ifeq (${KERNELRELEASE},)
	KERNEL_SOURCE := /home/siemens/soc1/SoC1_Linux/bsp/kernel_rootfs/buildroot-2016.05/output/build/linux-4.4.12	
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} M=${PWD} modules

clean:
	${MAKE} -C ${KERNEL_SOURCE} M=${PWD} clean

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.

# obj-y -> we are linking directly to the kernel -> only the *.o will be generated
# obj-m -> we are NOT linking directly to the kernel -> a *.ko will be generated
else
	obj-y := linux_irte_drv.o
endif