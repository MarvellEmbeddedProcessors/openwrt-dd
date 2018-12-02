# Source files in this directory
TARGET = 
CSOURCES = msdTCAM.c msdAdvVct.c msdBrgFdb.c msdBrgVtu.c msdBrgStu.c \
		   msdPIRL.c msdPortRmon.c msdPortCtrl.c msdQosMap.c msdQav.c\
		   msdQueueCtrl.c msdSysCtrl.c msdPhyCtrl.c msdIMP.c msdRMU.c \
		   msdTrunk.c msdPTP.c msdLed.c

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
