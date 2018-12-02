# Source files in this directory
TARGET = 
CSOURCES = Topaz_msdTCAM.c Topaz_msdAdvVct.c Topaz_msdBrgFdb.c Topaz_msdBrgVtu.c \
		   Topaz_msdBrgStu.c Topaz_msdPIRL.c Topaz_msdPortRmon.c Topaz_msdIMP.c \
		   Topaz_msdPhyCtrl.c Topaz_msdPortCtrl.c Topaz_msdPortPrioMap.c Topaz_msdQav.c \
		   Topaz_msdQueueCtrl.c Topaz_msdSysCtrl.c Topaz_msdRMU.c Topaz_msdTCAMIntf.c \
		   Topaz_msdAdvVctIntf.c Topaz_msdBrgFdbIntf.c Topaz_msdBrgVtuIntf.c \
		   Topaz_msdBrgStuIntf.c Topaz_msdPIRLIntf.c Topaz_msdPortRmonIntf.c \
		   Topaz_msdIMPIntf.c Topaz_msdPhyCtrlIntf.c Topaz_msdPortCtrlIntf.c \
		   Topaz_msdPortPrioMapIntf.c Topaz_msdQueueCtrlIntf.c Topaz_msdSysCtrlIntf.c \
		   Topaz_msdRMUIntf.c Topaz_msdPTP.c Topaz_msdPTPIntf.c Topaz_msdLed.c

ASOURCES	=

# Include common variable definitions
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE	+=
EXTRA_DEFINE	+=
ADDED_CFLAGS	+=

# Include common build rules
ifeq ($(OS_RUN),LINUX)
include $(MSD_TOOL_DIR)/makelnx.rules
endif

# end of file
