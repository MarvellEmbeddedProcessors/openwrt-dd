#
# Copyright (C) 2013-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Mamba
  NAME:=Linksys WRT1900AC (Mamba)
  PACKAGES:= kmod-mwlwifi wpad-mini swconfig
endef

define Profile/Mamba/Description
 Package set compatible with the Linksys WRT1900AC (Mamba).
endef

$(eval $(call Profile,Mamba))
