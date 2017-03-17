#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/armada-a8k-8040-db
  NAME:=Marvell Armada 8040 DB (DB-88F8040-A1)
  CPU_TYPE:=cortex-a53
  CPU_SUBTYPE:=neon-vfpv4
  PACKAGES:= kmod-marvell-sata kmod-scsi-generic kmod-fs-ext4 kmod-fs-msdos kmod-fs-ntfs kmod-usb-core kmod-usb-ohci kmod-usb-uhci kmod-usb-storage kmod-usb-storage-extras kmod-usb2 kmod-usb3 usbutils kmod-mmc mmc-utils kmod-mtd-rw kmod-nbd pciutils fdisk lsblk kmod-ipsec
endef

define Profile/armada-a8k-8040-db/Description
  Package set compatible with the Armada 88F8040 Development Board.
endef

define Profile/armada-a7k-7040-db
  NAME:=Marvell Armada 7040 DB (DB-88F7040-A1)
  CPU_TYPE:=cortex-a53
  CPU_SUBTYPE:=neon-vfpv4
  PACKAGES:= kmod-marvell-sata kmod-scsi-generic kmod-fs-ext4 kmod-fs-msdos kmod-fs-ntfs kmod-usb-core kmod-usb-ohci kmod-usb-uhci kmod-usb-storage kmod-usb-storage-extras kmod-usb2 kmod-usb3 usbutils kmod-mmc mmc-utils kmod-mtd-rw kmod-nbd pciutils fdisk lsblk kmod-ipsec
endef

define Profile/armada-a7k-7040-db/Description
  Package set compatible with the Armada 88F7040 Development Board.
endef

define Profile/armada-3720-db
  NAME:=Marvell Armada 3720 DB (DB-88F3720-DDR3)
  CPU_TYPE:=cortex-a53
  CPU_SUBTYPE:=neon-vfpv4
  PACKAGES:=kmod-scsi-generic kmod-fs-ext4 kmod-fs-msdos kmod-fs-ntfs kmod-usb-core kmod-usb-ohci kmod-usb-uhci kmod-usb-storage kmod-usb-storage-extras kmod-usb2 kmod-usb3 usbutils kmod-mmc mmc-utils kmod-mtd-rw kmod-nbd pciutils e2fsprogs dosfstools fdisk lsblk kmod-ipsec
endef

define Profile/armada-3720-db/Description
  Package set compatible with the Armada 3720 Development Board (DB-88F3720-DDR3)
endef

$(eval $(call Profile,armada-a8k-8040-db))
$(eval $(call Profile,armada-a7k-7040-db))
$(eval $(call Profile,armada-3720-db))
