# Source files in this directory
TARGET = 
CSOURCES = Pearl_msdTCAM.c Pearl_msdAdvVct.c Pearl_msdBrgFdb.c Pearl_msdBrgVtu.c \
	   Pearl_msdBrgStu.c Pearl_msdPIRL.c Pearl_msdPortRmon.c Pearl_msdPortCtrl.c \
	   Pearl_msdPortPrioMap.c Pearl_msdSysCtrl.c Pearl_msdPhyCtrl.c Pearl_msdQueueCtrl.c\
	   Pearl_msdIMP.c Pearl_msdRMU.c Pearl_msdTCAMIntf.c Pearl_msdAdvVctIntf.c Pearl_msdBrgFdbIntf.c \
	   Pearl_msdBrgVtuIntf.c Pearl_msdBrgStuIntf.c Pearl_msdPIRLIntf.c Pearl_msdQav.c \
	   Pearl_msdPortRmonIntf.c Pearl_msdPortCtrlIntf.c Pearl_msdPortPrioMapIntf.c \
	   Pearl_msdQueueCtrlIntf.c Pearl_msdSysCtrlIntf.c Pearl_msdPhyCtrlIntf.c \
	   Pearl_msdIMPIntf.c Pearl_msdRMUIntf.c Pearl_msdPTP.c Pearl_msdPTPIntf.c Pearl_msdLed.c

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
