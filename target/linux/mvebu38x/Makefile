#
# Copyright (C) 2012-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mvebu38x
BOARDNAME:=Marvell Armada 38x
FEATURES:=fpu usb pci pcie gpio nand squashfs
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=vfpv3
MAINTAINER:=Vladimir Vid <vladimir.vid@sartura.hr>

KERNEL_PATCHVER:=4.4

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += uboot-envtools uboot-mvebu38x-clearfog

$(eval $(call BuildTarget))
