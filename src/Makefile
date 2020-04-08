CC = /opt/poky-craton2/2.2.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
INC_PATH = /home/s3v3rian/V2X/pbh/src/lib/inc /home/s3v3rian/V2X/pbh/src/denm /home/s3v3rian/V2X/pbh/src/cam /home/s3v3rian/V2X/pbh/src/gps /home/s3v3rian/V2X/pbh/src/common /home/s3v3rian/V2X/pbh/src/common/utils /home/s3v3rian/V2X/pbh/src/common/file /home/s3v3rian/V2X/pbh/src/boundary /home/s3v3rian/V2X/pbh/src/sa
LIB_PATH = /home/s3v3rian/V2X/pbh/src/lib/
LIBS := /home/s3v3rian/V2X/pbh/src/lib/*.a -pthread -lrt -lm
LDFLAGS :=-g -L/home/s3v3rian/V2X/pbh/src/lib/
CFLAGS = -g -I/home/s3v3rian/V2X/pbh/src/lib/inc -I/home/s3v3rian/V2X/pbh/src/ -I/home/s3v3rian/V2X/pbh/src/denm -I/home/s3v3rian/V2X/pbh/src/cam -I/home/s3v3rian/V2X/pbh/src/gps -I/home/s3v3rian/V2X/pbh/src/common -I/home/s3v3rian/V2X/pbh/src/common/utils -I/home/s3v3rian/V2X/pbh/src/common/file -I/home/s3v3rian/V2X/pbh/src/boundary -I/home/s3v3rian/V2X/pbh/src/sa -std=gnu99 -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb --sysroot=/opt/poky-craton2/2.2.1/sysroots/cortexa7t2hf-neon-poky-linux-gnueabi
SEC_FLAGS =
DEPS = threads.h common/globals.h cam/cam_mngr.h denm/denm_mngr.h gps/nmea_infra.h cam/cam_infra.h denm/denm_infra.h gps/gps_sim.h common/file/csv_infra.h boundary/serial_output.h sa/sa_mngr.h
OBJ = startup.o threads.o cam/cam_mngr.o denm/denm_mngr.o gps/nmea_infra.o cam/cam_infra.o denm/denm_infra.o gps/gps_sim.o common/file/csv_infra.o boundary/serial_output.o sa/sa_mngr.o
EXECUTABLES=v2x_car

.PHONY: all

all: $(EXECUTABLES)

%.o: %.c $(DEPS)	
	$(CC) -D__EN_SECURITY_FEATURE__=1 -D__GPS_SIMULATOR_ENABLED__=1 -c -o $@ $< $(CFLAGS)

v2x_car: $(OBJ)	
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
