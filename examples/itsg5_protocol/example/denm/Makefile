CC ?= gcc
INC_PATH ?= $(realpath ../inc)
LIB_PATH ?= $(realpath ../lib)
LIBS := $(wildcard $(LIB_PATH)/*.a) -pthread -lrt -lm
LDFLAGS :=-g -L$(LIB_PATH)
CFLAGS +=-g -I$(INC_PATH)

EXAMPLES=denm_send_recv denm_security_send_recv

.PHONY: all

all: $(EXAMPLES)

#%.o: %.c
#	$(CC) $(CFLAGS) -o $@ -c $< 

denm_send_recv.o: denm_send_recv.c
	$(CC) $(CFLAGS) -D__EN_SECURITY_FEATURE__=0 -o $@ -c $< 

denm_send_recv: denm_send_recv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

denm_security_send_recv.o: denm_send_recv.c
	$(CC) $(CFLAGS) -D__EN_SECURITY_FEATURE__=1 -o $@ -c $< 

denm_security_send_recv: denm_security_send_recv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *~ *.o $(EXAMPLES)
