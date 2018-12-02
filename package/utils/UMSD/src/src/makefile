# makefile - build main object file
#
# modification history
# --------------------
# 25-05-2016,sundk	created from MSD_Peridot
#
#######################################################################

TARGET = $(MSD_PROJ_NAME)

ifeq ($(OS_RUN),LINUX)
WORK_TO_DO = $(MSD_PROJ_NAME).o

# Include common variable definitions
include $(MSD_TOOL_DIR)/makelnx.defs
endif

ifeq ($(TOPAZ_BUILD), YES)
TOPAZ_CLEAN = 
endif

vpath %.o    $(OBJDIR)
vpath %.lib  $(OBJDIR)

MAINDRV_PATH = $(SRC_BASE_PATH)

SRC_OBJ_DIR = $(MAINDRV_PATH)/driver \
               $(MAINDRV_PATH)/api \
               $(MAINDRV_PATH)/platform \
               $(MAINDRV_PATH)/utils

ifeq ($(TOPAZ_BUILD), YES)
SRC_OBJ_DIR += $(MSD_USER_BASE)/dev/topaz \
		$(MSD_USER_BASE)/dev/topaz/src/api \
	        $(MSD_USER_BASE)/dev/topaz/src/driver	 
endif

ifeq ($(PERIDOT_BUILD), YES)
SRC_OBJ_DIR += $(MSD_USER_BASE)/dev/peridot \
		$(MSD_USER_BASE)/dev/peridot/src/api \
		$(MSD_USER_BASE)/dev/peridot/src/driver	 
endif

ifeq ($(AGATE_BUILD), YES)
SRC_OBJ_DIR += $(MSD_USER_BASE)/dev/agate \
		$(MSD_USER_BASE)/dev/agate/src/api \
		$(MSD_USER_BASE)/dev/agate/src/driver
endif

ifeq ($(PEARL_BUILD), YES)
SRC_OBJ_DIR += $(MSD_USER_BASE)/dev/pearl \
		$(MSD_USER_BASE)/dev/pearl/src/api \
		$(MSD_USER_BASE)/dev/pearl/src/driver
endif
			   

OBJECTS_LIST = $(MAINDRV_PATH)/driver/$(OBJDIR)/driver.o \
               $(MAINDRV_PATH)/api/$(OBJDIR)/api.o \
               $(MAINDRV_PATH)/platform/$(OBJDIR)/platform.o \
               $(MAINDRV_PATH)/utils/$(OBJDIR)/utils.o

OBJECTS = $(OBJECTS_LIST)

OBJDIRS	= $(subst /,\,$(dir $(OBJECTS)))
LIBDIRS	= $(subst /,\,$(LIB_DIR))

OBJDIRS_/ = $(subst \,/,$(OBJDIRS))
LIBDIRS_/ = $(subst \,/,$(LIBDIRS))

ifeq ($(OS_RUN),LINUX)
$(MSD_PROJ_NAME).o : $(OBJECTS) $(PACKAGE_OBJECT_TOPAZ) $(PACKAGE_OBJECT_PERIDOT) $(PACKAGE_OBJECT_AGATE) $(PACKAGE_OBJECT_PEARL)
	@ $(ECHO) '------------'
	@ $(ECHO) 'Building $@'
	$(LD) $(LDFLAGS) -Map $(LIB_DIR)/$(MSD_PROJ_NAME).map -o $(LIB_DIR)/$(MSD_PROJ_NAME).o $(OBJECTS) $(PACKAGE_OBJECT_TOPAZ) $(PACKAGE_OBJECT_PERIDOT) $(PACKAGE_OBJECT_AGATE) $(PACKAGE_OBJECT_PEARL)

$(OBJECTS) : FORCE
	@cd $(dir $(@D)); $(MAKE)
	
$(PACKAGE_OBJECT_TOPAZ): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/topaz/src;

$(PACKAGE_OBJECT_PERIDOT): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/peridot/src;

$(PACKAGE_OBJECT_AGATE): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/agate/src;

$(PACKAGE_OBJECT_PEARL): FORCE
	$(MAKE) -C $(MSD_USER_BASE)/dev/pearl/src;

endif

FORCE :

.PHONY : clean

ifeq ($(OS_RUN),LINUX)
clean : 
	@for i in $(SRC_OBJ_DIR); do	\
		cd $$i; $(RM) -r -f *.o *.map makedeps *obj; 	\
	done
	
	@cd $(LIB_DIR); $(RM) *.o
	@cd $(LIB_DIR); $(RM) *.map
endif

#end of file
