
CC ?= gcc
INC_PATH ?= $(realpath ../inc)
LIB_PATH ?= $(realpath ../lib)
LIBS := $(wildcard $(LIB_PATH)/*.a) -pthread -lrt -lm
EXTRA_CFLAGS =
CFLAGS += $(EXTRA_CFLAGS)
export CFLAGS INC_PATH LIB_PATH

EXAMPLES := cam denm itsmsg gn poti
SUBDIRS := cam denm libitsmsg libgn libpoti

.PHONY: all $(EXAMPLES)

all: $(EXAMPLES)

cam:
	make -C cam
	
denm:
	make -C denm

itsmsg: 
	make -C libitsmsg

gn: 
	make -C libgn

poti: 
	make -C libpoti


clean:
	$(foreach d, $(SUBDIRS), make -C $(d) clean;)
