CC = /opt/poky-craton2/2.2.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
INC_PATH = /home/rdev/V2X/pbh/src/inc /home/rdev/V2X/pbh/src/
LIB_PATH = /home/rdev/V2X/pbh/src/lib/
LIBS := /home/rdev/V2X/pbh/src/lib/*.a -pthread -lrt -lm
LDFLAGS :=-g -L/home/rdev/V2X/pbh/src/lib/
CFLAGS = -g -I/home/rdev/V2X/pbh/src/inc -I/home/rdev/V2X/pbh/src/ -std=gnu99 -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb --sysroot=/opt/poky-craton2/2.2.1/sysroots/cortexa7t2hf-neon-poky-linux-gnueabi
SEC_FLAGS =
DEPS = globals.h cam_mngr.h denm_mngr.h nmea_infra.h cam_infra.h denm_infra.h gps_sim.h csv_infra.h serial_output.h sa_mngr.h
OBJ = threads.o cam_mngr.o denm_mngr.o nmea_infra.o cam_infra.o denm_infra.o gps_sim.o csv_infra.o serial_output.o sa_mngr.o
EXECUTABLES=v2x_car

.PHONY: all

all: $(EXECUTABLES)

%.o: %.c $(DEPS)	
	$(CC) -D__EN_SECURITY_FEATURE__=1 -D__GPS_SIMULATOR_ENABLED__=1 -c -o $@ $< $(CFLAGS)

v2x_car: $(OBJ)	
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:	rm -f *~ *.o $(EXECUTABLES)
