#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define CompressGzip
	gzip -9n -c $(1) > $(2)
endef

define Image/Prepare
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(call CompressGzip,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.gz)
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/BuildKernel
	$(call Image/BuildKernel/Profile/$(PROFILE))
endef

define Image/Build
	$(call Image/Build/$(1))
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/Build/Initramfs)
endif
endef

$(eval $(call BuildImage))
