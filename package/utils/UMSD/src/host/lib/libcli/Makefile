# Build dynamic library by default
#DYNAMIC_LIB ?= 1
# Build static library by default
STATIC_LIB ?= 1

UNAME = $(shell sh -c 'uname -s 2>/dev/null || echo not')
DESTDIR =

LIB = libcli.so
LIB_STATIC = libcli.a

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
ARFLAGS = rcs
DEBUG = -g
OPTIM = -O3
override CFLAGS += $(DEBUG) $(OPTIM) -Wall -std=c99 -pedantic -Wformat-security -Wno-format-zero-length -Werror -Wwrite-strings -Wformat -fdiagnostics-show-option -Wextra -Wsign-compare -Wcast-align -Wno-unused-parameter
override LDFLAGS += -shared
override LIBPATH += -L.

ifeq ($(UNAME),Darwin)
override LDFLAGS += -Wl,-install_name,$(LIB)
else
override LDFLAGS += -Wl,-soname,$(LIB)
LIBS = -lcrypt
endif

ifeq (1,$(DYNAMIC_LIB))
TARGET_LIBS += $(LIB)
endif
ifeq (1,$(STATIC_LIB))
TARGET_LIBS += $(LIB_STATIC)
endif

all: $(TARGET_LIBS)

$(LIB): libcli.o
	$(CC) -o $(LIB) $^ $(LDFLAGS) $(LIBS)

$(LIB_STATIC): libcli.o
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -o $@ -c $<

libcli.o: libcli.h

clean:
	rm -f *.o $(LIB)* $(LIB_STATIC) 
