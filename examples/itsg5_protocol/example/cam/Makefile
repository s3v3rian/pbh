CC ?= gcc
INC_PATH ?= $(realpath ../inc)
LIB_PATH ?= $(realpath ../lib)
LIBS := $(wildcard $(LIB_PATH)/*.a) -pthread -lrt -lm
LDFLAGS :=-g -L$(LIB_PATH)
CFLAGS +=-g -I$(INC_PATH)
SEC_FLAGS =

EXAMPLES=cam_send_recv cam_security_send_recv

.PHONY: all

all: $(EXAMPLES)

#%.o: %.c
#	$(CC) $(CFLAGS) -o $@ -c $< 	


cam_send_recv.o: cam_send_recv.c
	$(CC) $(CFLAGS) -D__EN_SECURITY_FEATURE__=0 -o $@ -c $< 

cam_send_recv: cam_send_recv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

cam_security_send_recv.o: cam_send_recv.c
	$(CC) $(CFLAGS) -D__EN_SECURITY_FEATURE__=1 -o $@ -c $< 

cam_security_send_recv: cam_security_send_recv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *~ *.o $(EXAMPLES)
