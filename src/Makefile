CC = /opt/poky-craton2/2.2.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
INC_PATH = lib/inc 
INC_DIR = -Ilib/inc \
	  -I../src/ \
	  -Iservices \
	  -Iservices/denm \
	  -Iservices/cam \
	  -Iservices/gps \
	  -Iboundary \
	  -Icommon \
	  -Icommon/file \
	  -Icommon/utils \
	  -Icommon/containers \
	  -Isa/processors \
	  -Isa/algs \
	  -Isa \
	  -Isim/processors \
	  -Isim
LIB_PATH = lib/
LIBS := lib/*.a -pthread -lrt -lm
LDFLAGS :=-g -L$(LIB_PATH)
CFLAGS = -O3 $(INC_DIR) -std=gnu99 -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb --sysroot=/opt/poky-craton2/2.2.1/sysroots/cortexa7t2hf-neon-poky-linux-gnueabi
SEC_FLAGS =
DEPS = $(wildcard *.h) \
      $(wildcard services/*.h) \
      $(wildcard services/denm/*.h) \
      $(wildcard services/cam/*.h) \
      $(wildcard services/gps/*.h) \
      $(wildcard boundary/*.h) \
      $(wildcard common/*.h) \
      $(wildcard common/file/*.h) \
      $(wildcard common/utils/*.h) \
      $(wildcard common/containers/*.h) \
      $(wildcard sa/processors/*.h) \
      $(wildcard sa/algs/*.h) \
      $(wildcard sa/*.h) \
      $(wildcard sim/processors/*.h) \
      $(wildcard sim/*.h)
SRC = $(wildcard *.c) \
      $(wildcard services/*.c) \
      $(wildcard services/denm/*.c) \
      $(wildcard services/cam/*.c) \
      $(wildcard services/gps/*.c) \
      $(wildcard boundary/*.c) \
      $(wildcard common/*.c) \
      $(wildcard common/file/*.c) \
      $(wildcard common/utils/*.c) \
      $(wildcard common/containers/*.c) \
      $(wildcard sa/processors/*.c) \
      $(wildcard sa/algs/*.c) \
      $(wildcard sa/*.c) \
      $(wildcard sim/processors/*.c) \
      $(wildcard sim/*.c)
OBJ = $(SRC:.c=.o)
EXECUTABLES=v2x_car

.PHONY: all

all: $(EXECUTABLES)

%.o: %.c $(DEPS)	
	$(CC) -D__EN_SECURITY_FEATURE__=0 -D__EN_DEBUG_PRINTING_FEATURE__=1 -D__EN_SERIAL_OUTPUT_FEATURE__=0 -D__EN_SIMULATOR_FEATURE__=1 -c -o $@ $< $(CFLAGS)

v2x_car: $(OBJ)	
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
