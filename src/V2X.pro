QT =
CONFIG =

LIBS += -L/home/s3v3rian/V2X/pbh/src/lib/ -lpthread -lrt -lm -litsg5_protocol -ltomcrypt -ltommath

#INCLUDEPATH = /home/s3v3rian/V2X/pbh/src/inc/ \
# /opt/poky-craton2/2.2.1/sysroots/x86_64-pokysdk-linux/usr/lib/arm-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/6.2.0/include/ \
# /usr/include/ \

DEFINES += __EN_SECURITY_FEATURE__=1
DEFINES += __GPS_SIMULATOR_ENABLED__=1

INCLUDEPATH += /home/s3v3rian/V2X/pbh/src/ \
    /home/s3v3rian/V2X/pbh/src/inc/ \

SOURCES = threads.c \
    nmea_infra.c \
    cam_infra.c \
