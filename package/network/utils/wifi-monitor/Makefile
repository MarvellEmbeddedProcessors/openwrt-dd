#
# Copyright (C) 2014-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wifi-monitor
PKG_VERSION:=1
PKG_RELEASE=1

PKG_LICENSE:=BSD 3-Clause
PKG_LICENSE_FILES:=LICENCE

PKG_MAINTAINER:=Jinhua Zhang <zhangjh@marvell.com>
PKG_BUILD_PARALLEL:=1

TARGET_CFLAGS := \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-D_GNU_SOURCE

include $(INCLUDE_DIR)/package.mk

define Package/wifi-monitor
  	SECTION:=net
	CATEGORY:=Network
	TITLE:=WiFi State Monitor
	DEPENDS:=+libpthread +libnl-tiny +libubus +libubox +@KERNEL_DEVMEM +@BUSYBOX_CONFIG_DEVMEM
endef

define Package/wifi-monitor/description
	WiFi State Monitor
endef

define Package/wifi-monitor/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifi_monitor $(1)/usr/bin/
	$(INSTALL_BIN) ./files/etc/init.d/wifi_monitor $(1)/etc/init.d/wifi_monitor
endef

$(eval $(call BuildPackage,wifi-monitor))
