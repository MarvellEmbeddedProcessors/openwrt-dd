include $(TOPDIR)/rules.mk

PKG_NAME:=UMSD
PKG_VERSION:=2.0.4
PKG_RELEASE:=1
PKG_MAINTAINER:=Ma Jia <jiam@marvell.com>

include $(INCLUDE_DIR)/package.mk

define Package/UMSD
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Unified Marvell SOHO Driver
  DEPENDS:=+libpcap +kmod-mvmdio_uio
  PKGARCH:=all
endef

define Package/adb/description
 UMSD is a low-level SW driver for Marvell’s Packet Processor, SOHO Switch family.
 UMSD includes the following components: Driver Code (APIs Interface, product package), Reference Code (samples), and Utilities (CLI)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/host/arm/ \
		CC="aarch64-openwrt-linux-musl-gcc" \
		LD="aarch64-openwrt-linux-musl-ld" \
		MSD_USER_BASE="$(PKG_BUILD_DIR)" \
		MSD_PROJ_NAME="msdDrv" \
		MSD_TOOL_DIR="$(PKG_BUILD_DIR)/tools" \
		RELEASE="YES" \
		OS_RUN="LINUX" \
		TOPAZ_BUILD="YES" \
		PERIDOT_BUILD="YES" \
		AGATE_BUILD="YES" \
		PEARL_BUILD="YES"
endef

define Package/UMSD/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/host/arm/MCLI $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/umsd $(1)/etc/init.d/umsd
endef

$(eval $(call BuildPackage,UMSD))
