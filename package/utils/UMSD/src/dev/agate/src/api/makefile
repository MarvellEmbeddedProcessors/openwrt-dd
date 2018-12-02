# Source files in this directory
TARGET = 
CSOURCES = Agate_msdTCAM.c Agate_msdAdvVct.c Agate_msdBrgFdb.c Agate_msdBrgVtu.c \
	   Agate_msdBrgStu.c Agate_msdPIRL.c Agate_msdPortRmon.c Agate_msdPortCtrl.c \
	   Agate_msdPortPrioMap.c Agate_msdSysCtrl.c Agate_msdPhyCtrl.c Agate_msdQueueCtrl.c\
	   Agate_msdIMP.c Agate_msdRMU.c Agate_msdTCAMIntf.c Agate_msdAdvVctIntf.c Agate_msdBrgFdbIntf.c \
	   Agate_msdBrgVtuIntf.c Agate_msdBrgStuIntf.c Agate_msdPIRLIntf.c Agate_msdQav.c \
	   Agate_msdPortRmonIntf.c Agate_msdPortCtrlIntf.c Agate_msdPortPrioMapIntf.c \
	   Agate_msdQueueCtrlIntf.c Agate_msdSysCtrlIntf.c Agate_msdPhyCtrlIntf.c \
	   Agate_msdIMPIntf.c Agate_msdRMUIntf.c Agate_msdPTP.c Agate_msdPTPIntf.c Agate_msdLed.c

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
