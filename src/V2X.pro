QT =
CONFIG =

LIBS += -L/home/s3v3rian/V2X/pbh/src/lib/ -lpthread -lrt -lm -litsg5_protocol -ltomcrypt -ltommath

DEFINES += __EN_SECURITY_FEATURE__=1
DEFINES += __GPS_SIMULATOR_ENABLED__=1

INCLUDEPATH += threads.h \
    boundary/ \
    cam/ \
    common/ \
    common/file/ \
    common/utils/ \
    common/containers/ \
    denm/ \
    gps/ \
    sa \
    lib/inc/ \

SOURCES = startup.c \
    threads.c \
    boundary/serial_ouput.c \
    cam/cam_infra.c \
    cam/cam_mngr.c \
    common/file/csv_infra.c \
    common/file/ini_infra.c \
    common/utils/geo_utils.c \
    common/containers/spsc_array_queue.c \
    denm/denm_infra.c \
    denm/denm_mngr.c \
    gps/gps_sim.c \
    gps/nmea_infra. \
    sa/sa_mngr.c \

