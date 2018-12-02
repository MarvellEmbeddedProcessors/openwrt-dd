#Makefile for Topaz API test program

include ./makedef

CFLAGS += $(MSD_INCLUDE) -DUSE_SEMAPHORE $(TOPAZ) $(PERIDOT) $(AGATE) $(PEARL) $(UNITTEST)
LDFLAGS += -fPIC -L$(MSD_PATH)/host/lib/libcli -lcli -lpcap

EXEC = MCLI
MSD_LIB = $(MSD_PATH)/library/$(MSD_PROJ_NAME).o

all: $(EXEC) $(MSD_LIB)

$(MSD_LIB): 
	make -C $(MSD_PATH)

$(EXEC): $(SAMPLE_OBJS) $(UNITTEST_OBJS) $(SWTEST_OBJS) $(MSD_LIB)
	make -C $(MSD_PATH)/host/lib/libcli;
	$(CC) $(CFLAGS) -o $(EXEC) $(SAMPLE_OBJS) $(UNITTEST_OBJS) $(SWTEST_OBJS) $(MSD_LIB) $(LDFLAGS)
	@cp ../../sample/IMPSampleFiles/*.ihx .
	-@mkdir obj
	-@mv *.o obj/
clean: 
	make -C $(MSD_PATH) clean;
	make -C $(MSD_PATH)/host/lib/libcli clean;
	-@rm -f $(EXEC) *.o
	-@rm -rf obj/
	-@rm -f *.ihx

