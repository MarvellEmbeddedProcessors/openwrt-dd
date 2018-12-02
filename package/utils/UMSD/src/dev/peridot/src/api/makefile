# Source files in this directory
TARGET = 
CSOURCES = Peridot_msdTCAM.c Peridot_msdAdvVct.c Peridot_msdBrgFdb.c Peridot_msdBrgVtu.c \
	   Peridot_msdBrgStu.c Peridot_msdPIRL.c Peridot_msdPortRmon.c Peridot_msdPortCtrl.c \
	   Peridot_msdPortPrioMap.c Peridot_msdQueueCtrl.c Peridot_msdSysCtrl.c Peridot_msdPhyCtrl.c \
	   Peridot_msdIMP.c Peridot_msdRMU.c Peridot_msdTCAMIntf.c Peridot_msdAdvVctIntf.c Peridot_msdBrgFdbIntf.c \
	   Peridot_msdBrgVtuIntf.c Peridot_msdBrgStuIntf.c Peridot_msdPIRLIntf.c Peridot_msdQav.c \
	   Peridot_msdPortRmonIntf.c Peridot_msdPortCtrlIntf.c Peridot_msdPortPrioMapIntf.c \
	   Peridot_msdQueueCtrlIntf.c Peridot_msdSysCtrlIntf.c Peridot_msdPhyCtrlIntf.c \
	   Peridot_msdIMPIntf.c Peridot_msdRMUIntf.c Peridot_msdPTP.c Peridot_msdPTPIntf.c Peridot_msdLed.c

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
