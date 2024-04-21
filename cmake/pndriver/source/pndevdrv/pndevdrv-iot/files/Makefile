VIEW		      := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
SRC               := $(shell pwd)
SOURCE_SYS        := pndevdrv/PnDev_Driver/src/PnDev_Driver.sys
SOURCE_COMMON     := pndevdrv/PnDev_Driver/common
SOURCE_SYS_ABS    := $(SRC)/pndevdrv/PnDev_Driver/src/PnDev_Driver.sys
SOURCE_COMMON_ABS := $(SRC)/pndevdrv/PnDev_Driver/common

ccflags-y += 	-g -o2 -Wframe-larger-than=10000\
		-DPNDEV_DRIVER_SYS	\
		-DPNDEV_OS_LINUX \
		-DPNDEV_SUPPORT_IX1000 \
		-I$(VIEW)/pndevdrv/PnDev_Driver/common/

TARGET = PnDevDrv_32


ifneq ($(KERNELRELEASE),)

$(TARGET)-objs += $(SOURCE_SYS)/os.o
$(TARGET)-objs += $(SOURCE_SYS)/Basic.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_IX1000.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_IsrDpc.o
$(TARGET)-objs += $(SOURCE_SYS)/Board_Res.o
$(TARGET)-objs += $(SOURCE_SYS)/Board.o
$(TARGET)-objs += $(SOURCE_SYS)/Device.o
$(TARGET)-objs += $(SOURCE_SYS)/General.o
$(TARGET)-objs += $(SOURCE_SYS)/Ioctl.o
$(TARGET)-objs += $(SOURCE_SYS)/IsrDpc.o
$(TARGET)-objs += $(SOURCE_SYS)/String.o
$(TARGET)-objs += $(SOURCE_COMMON)/PnCore.o
$(TARGET)-objs += $(SOURCE_COMMON)/os_Env.o

obj-m	:= $(TARGET).o

else

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) LDDINC=$(SRC) modules

endif


install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) LDDINC=$(SRC) clean
	rm -rf *.o *~ core .depend .*.cmd $(TARGET).ko *.mod.c .tmp_versions *.symvers *.order
	
	rm -rf $(SOURCE_SYS_ABS)/*.o $(SOURCE_SYS_ABS)/*~ core $(SOURCE_SYS_ABS)/*.depend $(SOURCE_SYS_ABS)/.*.cmd $(SOURCE_SYS_ABS)/*.ko $(SOURCE_SYS_ABS)/*.mod.c $(SOURCE_SYS_ABS)/*.tmp_versions $(SOURCE_SYS_ABS)/*.symvers $(SOURCE_SYS_ABS)/*.order

	rm -rf $(SOURCE_COMMON_ABS)/*.o $(SOURCE_COMMON_ABS)/*~ core $(SOURCE_COMMON_ABS)/*.depend $(SOURCE_COMMON_ABS)/.*.cmd $(SOURCE_COMMON_ABS)/*.ko $(SOURCE_COMMON_ABS)/*.mod.c $(SOURCE_COMMON_ABS)/*.tmp_versions $(SOURCE_COMMON_ABS)/*.symvers $(SOURCE_COMMON_ABS)/*.order

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
