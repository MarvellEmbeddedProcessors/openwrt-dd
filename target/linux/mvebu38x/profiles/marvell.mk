#
# Copyright (C) 2013-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/385-RD
  NAME:=Marvell Armada 385 RD (RD-88F6820-AP)
  PACKAGES:=
endef

define Profile/385-RD/Description
 Package set compatible with the Armada 385 reference design board (RD-88F6820-AP).
endef

$(eval $(call Profile,385-RD))

define Profile/385-DB-AP
  NAME:=Marvell Armada 385 DB AP (DB-88F6820-AP)
  PACKAGES:=
endef

define Profile/385-DB-AP/Description
 Package set compatible with the Armada 385 access point development board (DB-88F6820-AP).
endef

$(eval $(call Profile,385-DB-AP))
