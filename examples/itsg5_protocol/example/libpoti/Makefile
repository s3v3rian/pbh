CC ?= gcc
INC_PATH ?= $(realpath ../inc)
LIB_PATH ?= $(realpath ../lib)
LIBS := $(wildcard $(LIB_PATH)/*.a) -pthread -lrt -lm
LDFLAGS :=-g
CFLAGS +=-g -I$(INC_PATH)

EXAMPLES=example_poti

.PHONY: all

all: $(EXAMPLES)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 
	
example_poti: example_poti.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *~ *.o $(EXAMPLES)
