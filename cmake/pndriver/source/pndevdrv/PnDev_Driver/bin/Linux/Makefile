VIEW:= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../../../)
SOURCE_SYS			:= ../../src/PnDev_Driver.sys
SOURCE_COMMON		:= $(SOURCE_SYS)/../../common
SOURCE_SYS_ABS		:= $(VIEW)/pndevdrv/PnDev_Driver/src/PnDev_Driver.sys
SOURCE_COMMON_ABS	:= $(VIEW)/pndevdrv/PnDev_Driver/common

ccflags-y += -g -o0 -Wframe-larger-than=1000\
				-DPNDEV_DRIVER_SYS	\
				-DPNDEV_SUPPORT_I210 -DPNDEV_SUPPORT_I210SFP -DPNDEV_SUPPORT_I82574 -DPNDEV_SUPPORT_IX1000 -DPNDEV_SUPPORT_OTHERS	\
				-DPNDEV_OS_LINUX -I$(VIEW)/pndevdrv/PnDev_Driver/common/ \
				-I$(VIEW)/pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/
		
#for Kenerl version < 3.16
#ccflags-y += -DPNDEV_LINUX_MSI_BLOCK
        
ifdef IS_32		
    TARGET = PnDevDrv_32
else
    TARGET = PnDevDrv_64
endif

ifneq ($(KERNELRELEASE),)

$(TARGET)-objs += $(SOURCE_SYS)/os.o
$(TARGET)-objs += $(SOURCE_SYS)/Basic.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_I210.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_I210SFP.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_I82574.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_IX1000.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_Micrel.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_AM5728.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_Irte.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_IrtePnip.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_IsrDpc.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_Pnip.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_Res.o
$(TARGET)-objs += $(SOURCE_SYS)/Board.o
$(TARGET)-objs += $(SOURCE_SYS)/Device.o
$(TARGET)-objs += $(SOURCE_SYS)/Flash_Nand.o
$(TARGET)-objs += $(SOURCE_SYS)/Flash_Nor.o
$(TARGET)-objs += $(SOURCE_SYS)/Flash_SPI.o
$(TARGET)-objs += $(SOURCE_SYS)/Flash.o
$(TARGET)-objs += $(SOURCE_SYS)/General.o
$(TARGET)-objs += $(SOURCE_SYS)/Ioctl.o
$(TARGET)-objs += $(SOURCE_SYS)/IsrDpc.o
$(TARGET)-objs += $(SOURCE_SYS)/String.o
$(TARGET)-objs += $(SOURCE_COMMON)/PnCore.o
$(TARGET)-objs += $(SOURCE_COMMON)/os_Env.o

obj-m	:= $(TARGET).o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(VIEW)/pndevdrv/PnDev_Driver/bin/Linux LDDINC=$(PWD) modules

endif


install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(VIEW)/pndevdrv/PnDev_Driver/bin/Linux LDDINC=$(PWD) clean
	rm -rf *.o *~ core .depend .*.cmd $(TARGET).ko *.mod.c .tmp_versions *.symvers *.order
	
	rm -rf $(SOURCE_SYS_ABS)/*.o $(SOURCE_SYS_ABS)/*~ core $(SOURCE_SYS_ABS)/*.depend $(SOURCE_SYS_ABS)/.*.cmd $(SOURCE_SYS_ABS)/*.ko $(SOURCE_SYS_ABS)/*.mod.c $(SOURCE_SYS_ABS)/*.tmp_versions $(SOURCE_SYS_ABS)/*.symvers $(SOURCE_SYS_ABS)/*.order

	rm -rf $(SOURCE_COMMON_ABS)/*.o $(SOURCE_COMMON_ABS)/*~ core $(SOURCE_COMMON_ABS)/*.depend $(SOURCE_COMMON_ABS)/.*.cmd $(SOURCE_COMMON_ABS)/*.ko $(SOURCE_COMMON_ABS)/*.mod.c $(SOURCE_COMMON_ABS)/*.tmp_versions $(SOURCE_COMMON_ABS)/*.symvers $(SOURCE_COMMON_ABS)/*.order

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
