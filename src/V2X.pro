QT =
CONFIG =

LIBS += -L/home/s3v3rian/V2X/pbh/src/lib/ -lpthread -lrt -lm -litsg5_protocol -ltomcrypt -ltommath

DEFINES += __EN_SECURITY_FEATURE__=1
DEFINES += __GPS_SIMULATOR_ENABLED__=1

INCLUDEPATH += threads.h \
    boundary/ \
    common/ \
    common/file/ \
    common/utils/ \
    common/containers/ \
    services/cam/ \
    services/denm/ \
    services/gps/ \
    sa \
    lib/inc/ \

SOURCES = startup.c \
    threads.c \
    boundary/serial_ouput.c \
    common/ring_buffers.c \
    common/file/csv_infra.c \
    common/file/ini_infra.c \
    common/utils/geo_utils.c \
    common/utils/print_utils.c \
    common/containers/spsc_array_queue.c \
    common/containers/blocked_array_queue.c \
    common/containers/array_queue.c \
    services/cam/cam_infra.c \
    services/cam/cam_mngr.c \
    services/denm/denm_infra.c \
    services/denm/denm_mngr.c \
    services/gps/gps_poti.c \
    services/gps/gps_sim.c \
    services/gps/nmea_infra. \
    sa/sa_database.c \
    sa/sa_mngr.c \

