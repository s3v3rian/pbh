QT =
CONFIG =

LIBS += -L/home/s3v3rian/V2X/pbh/src/lib/ -lpthread -lrt -lm -litsg5_protocol -ltomcrypt -ltommath

DEFINES += __EN_SECURITY_FEATURE__=0
DEFINES += __EN_SIMULATOR_FEATURE__=1
DEFINES += __EN_DEBUG_PRINTING_FEATURE__=1
DEFINES += __EN_SERIAL_OUTPUT_FEATURE__=0

INCLUDEPATH += threads.h \
    boundary/ \
    common/ \
    common/file/ \
    common/utils/ \
    common/containers/ \
    services/cam/ \
    services/denm/ \
    services/spatem/ \
    services/mapem/ \
    services/rtcmem/ \
    services/gps/ \
    sa/processors/ \
    sa/algs/ \
    sa/ \
    sim/processors/ \
    sim/ \
    lib/inc/ \

SOURCES = startup.c \
    boundary/ethernet_boundary_writer.c \
    boundary/serial_boundary_writer.c \
    sa/processors/its_msg_processor_valeran_rsu.c \
    sa/processors/its_msg_processor_traffic_light.c \
    sim/processors/sim_processor_traffic_light.c \
    threads.c \
    common/sa_database.c \
    common/ring_buffers.c \
    common/file/csv_infra.c \
    common/file/ini_infra.c \
    common/utils/geo_utils.c \
    common/utils/print_utils.c \
    common/containers/spsc_array_queue.c \
    common/containers/blocked_array_queue.c \
    common/containers/array_queue.c \
    common/containers/ring_buffer.c \
    services/cam/cam_mngr.c \
    services/denm/denm_mngr.c \
    services/mapem/mapem_mngr.c \
    services/gps/gps_poti.c \
    services/gps/nmea_infra. \
    sa/processors/its_msg_processor.c \
    sa/processors/its_msg_processor_commercial.c \
    sa/processors/its_msg_processor_passenger.c \
    sa/algs/alg_haversine.c \
    sa/sa_mngr.c \
    sim/processors/sim_processor_commercial.c \
    sim/sim_mngr.c \
    sim/gps_sim.c \

