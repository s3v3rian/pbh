
CC ?= gcc
INC_PATH ?= $(realpath ../inc)
LIB_PATH ?= $(realpath ../lib)
LIBS := $(wildcard $(LIB_PATH)/*.a) -pthread -lrt -lm
LDFLAGS :=-g -L$(LIB_PATH)
CFLAGS +=-g -I$(INC_PATH)

EXAMPLES=example_itsmsg_main

.PHONY: all

all: $(EXAMPLES)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 
	
example_itsmsg_main: example_itsmsg_main.o example_itsmsg_encode.o example_itsmsg_decode.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
#	./example_itsmsg_main

clean:
	rm -f *~ *.o $(EXAMPLES)

