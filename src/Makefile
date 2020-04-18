CC = /opt/poky-craton2/2.2.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
INC_PATH = lib/inc 
INC_DIR = -Ilib/inc \
	  -I../src/ \
	  -Idenm \
	  -Icam \
	  -Igps \
	  -Iboundary \
	  -Icommon \
	  -Icommon/file \
	  -Icommon/utils \
	  -Icommon/containers \
	  -Isa
LIB_PATH = lib/
LIBS := lib/*.a -pthread -lrt -lm
LDFLAGS :=-g -L$(LIB_PATH)
CFLAGS = -g $(INC_DIR) -std=gnu99 -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb --sysroot=/opt/poky-craton2/2.2.1/sysroots/cortexa7t2hf-neon-poky-linux-gnueabi
SEC_FLAGS =
DEPS = $(wildcard *.h) \
      $(wildcard denm/*.h) \
      $(wildcard cam/*.h) \
      $(wildcard gps/*.h) \
      $(wildcard boundary/*.h) \
      $(wildcard common/*.h) \
      $(wildcard common/file/*.h) \
      $(wildcard common/utils/*.h) \
      $(wildcard common/containers/*.h) \
      $(wildcard sa/*.h)
SRC = $(wildcard *.c) \
      $(wildcard denm/*.c) \
      $(wildcard cam/*.c) \
      $(wildcard gps/*.c) \
      $(wildcard boundary/*.c) \
      $(wildcard common/*.c) \
      $(wildcard common/file/*.c) \
      $(wildcard common/utils/*.c) \
      $(wildcard common/containers/*.c) \
      $(wildcard sa/*.c)
OBJ = $(SRC:.c=.o)
EXECUTABLES=v2x_car

.PHONY: all

all: $(EXECUTABLES)

%.o: %.c $(DEPS)	
	$(CC) -D__EN_SECURITY_FEATURE__=1 -D__SERIAL_OUTPUT_ENABLED__=1 -D__GPS_SIMULATOR_ENABLED__=1 -c -o $@ $< $(CFLAGS)

v2x_car: $(OBJ)	
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
