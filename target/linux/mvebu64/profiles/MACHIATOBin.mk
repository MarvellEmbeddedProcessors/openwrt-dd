#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/armada-a8k-MACHIATOBin
  NAME:=MACHIATOBin (SolidRun Armada 8040 Community Board)
  CPU_TYPE:=cortex-a53
  CPU_SUBTYPE:=neon-vfpv4
  PACKAGES= kmod-usb-core kmod-usb2 kmod-usb3 kmod-usb-storage kmod-mmc mmc-utils pciutils
endef

define Profile/armada-a8k-MACHIATOBin/Description
 Package set compatible with the MACHIATOBin 8040 Community Board.
endef

$(eval $(call Profile,armada-a8k-MACHIATOBin))
