#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=mvebu64
BOARDNAME:=Marvell 64b Boards
FEATURES:=gpio usb pci pcie nand ramdisk targz
MAINTAINER:=Vladimir Vid <vladimir.vid@sartura.hr>

KERNEL_PATCHVER:=4.4

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=Image Image.gz dtbs

DEFAULT_PACKAGES += uboot-envtools ethtool hostapd

$(eval $(call BuildTarget))
