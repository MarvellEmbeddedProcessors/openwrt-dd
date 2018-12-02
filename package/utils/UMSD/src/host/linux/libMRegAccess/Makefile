CROSS_COMPILE = 
#CROSS_COMPILE = mipsel-uclibc-
CROSS         = $(CROSS_COMPILE)
CXX       = $(CROSS_COMPILE)g++

#CFLAGS = -Wall -g 
CFLAGS = -shared
# -nostdlib
CFLAGS += -fPIC

#LD_LIBRARY_PATH = /usr/lib/x86_64-linux-gnu/
#LIBS = /usr/local/lib/libusb-1.0.so
LIBS = -lusb-1.0
#-lm -lc -ldl

cyobject = libcyusb.o
cysource= libcyusb.c
output = libMRegAccess.so
objects = libMRegAccess.o MRegAccess_v61xx.o MRegAccess_v6.o MRegAccess_v5.o Wrapper.o $(cyobject)

all: $(output) 
$(output): $(objects)
	$(CXX) $(CFLAGS)  $(objects) $(LIBS) -o $(output) -pthread
	sudo cp $(output) /usr/lib
%.o:%.cpp
	$(CXX) $(CFLAGS) -c $<
$(cyobject): $(cysource)
	$(CXX) $(CFLAGS) $(LIBS) -o $(cyobject) -c $(cysource)
clean:
	-@$(RM) $(output1) $(output) $(objects) 
	
.PHONY: clean

