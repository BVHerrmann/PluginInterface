SUMMARY = "PROFINET Device Driver for accessing the hardware and handling the interrupts"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"


inherit module

SRC_URI = "file://Makefile \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Basic.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Basic.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Board.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Board_IX1000.c \
	       file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Board_IX1000.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Board_Intel.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Board_IsrDpc.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Board_Res.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Device.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/General.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Inc.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Ioctl.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/IsrDpc.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/os.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/os.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/os_SysDll.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/precomp.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Private.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/SysDll.h \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/String.c \
           file://pndevdrv/PnDev_Driver/src/PnDev_Driver.sys/Trace.h \
           file://pndevdrv/PnDev_Driver/common/PnCore.c \
           file://pndevdrv/PnDev_Driver/common/PnCore.h \
           file://pndevdrv/PnDev_Driver/common/PnCore_Inc.h \
           file://pndevdrv/PnDev_Driver/common/pndev_adapt.h \
           file://pndevdrv/PnDev_Driver/common/pndev_config.h \
           file://pndevdrv/PnDev_Driver/common/os_Env.c \
           file://pndevdrv/PnDev_Driver/common/os_Env.h \
           file://pndevdrv/PnDev_Driver/common/PnDev_Driver_Inc.h \
           file://pndevdrv/PnDev_Driver/common/PnDev_DriverU.h \
           file://pndevdrv/PnDev_Driver/common/pndev_np_common.h \
           file://pndevdrv/PnDev_Driver/common/pndev_targetver.h \
           file://pndevdrv/PnDev_Driver/common/PnDev_Util.h \
           file://pndevdrv/PnDev_Driver/common/precomp.h \
           file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
