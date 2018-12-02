# makefile - build main object file
#
# modification history
# --------------------
# 05-08-2015,sundk	created from DSDT_3.5
#
#######################################################################

MSD_PROJ_NAME_PKG = $(MSD_PROJ_NAME)_Pearl
LIB_DIR_PACKAGE = $(MSD_USER_BASE)/dev/pearl

ifeq ($(OS_RUN),LINUX)
WORK_TO_DO = $(MSD_PROJ_NAME_PKG).o

# Include common variable definitions
include $(MSD_TOOL_DIR)/makelnx.defs
endif

exe : $(WORK_TO_DO)

#.PHONY : exe

vpath %.o    $(OBJDIR)
vpath %.lib  $(OBJDIR)

MAINDRV_PATH = $(MSD_USER_BASE)/dev/pearl/src

SRC_OBJ_DIR = $(MAINDRV_PATH)/driver \
               $(MAINDRV_PATH)/api \

OBJECTS_LIST = $(MAINDRV_PATH)/driver/$(OBJDIR)/driver.o \
               $(MAINDRV_PATH)/api/$(OBJDIR)/api.o \

OBJECTS = $(OBJECTS_LIST)

OBJDIRS	= $(subst /,\,$(dir $(OBJECTS)))
LIBDIRS	= $(subst /,\,$(LIB_DIR))

OBJDIRS_/ = $(subst \,/,$(OBJDIRS))
LIBDIRS_/ = $(subst \,/,$(LIBDIRS))

ifeq ($(OS_RUN),LINUX)
$(MSD_PROJ_NAME_PKG).o : $(OBJECTS)
	@ $(ECHO) '------------'
	@ $(ECHO) 'Building $@'
	$(LD) $(LDFLAGS) -Map $ $(LIB_DIR_PACKAGE)/$(MSD_PROJ_NAME_PKG).map -o $(LIB_DIR_PACKAGE)/$(MSD_PROJ_NAME_PKG).o $(OBJECTS)

$(OBJECTS) : FORCE
	@cd $(dir $(@D)); $(MAKE)

endif

FORCE :

.PHONY : clean

ifeq ($(OS_RUN),LINUX)
clean :
	@for i in $(SRC_OBJ_DIR); do	\
		cd $$i; $(RM) -r -f *.o *.map makedeps *obj; 	\
	done
#		cd $$i; $(RM) -f *.o *.map; 	
	@for i in $(OBJDIRS_/); do	\
		$(RM) -r -f  $$i;	\
	done
#		cd $$i..; $(MAKE) clean;	
	@cd $(LIB_DIR_PACKAGE); $(RM) *.o
	@cd $(LIB_DIR_PACKAGE); $(RM) *.map
endif

#end of file
