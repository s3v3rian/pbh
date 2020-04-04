CC ?= gcc
INC_PATH ?= $(realpath ../inc)
LIB_PATH ?= $(realpath ../lib)
LIBS := $(wildcard $(LIB_PATH)/*.a) -pthread -lrt -lm
LDFLAGS :=-g
CFLAGS +=-g -I$(INC_PATH)

EXAMPLES = example_btp_sender example_btp_receiver example_btp_gbc_sender example_btp_gbc_receiver example_btp_concurrent_local example_btp_concurrent_remote example_btp_security_sign_sender example_btp_security_verify_receiver
.PHONY: all

all: $(EXAMPLES)

#%.o: %.c
#	$(CC) $(CFLAGS) -o $@ -c $< 
	
example_btp_sender: example_btp_main.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

example_btp_receiver: example_btp_main.c
	$(CC) $(CFLAGS) -D__BTP_RECEIVER__ $(LDFLAGS) -o $@ $^ $(LIBS)

example_btp_gbc_sender: example_btp_gbc_main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

example_btp_gbc_receiver: example_btp_gbc_main.c
	$(CC) $(CFLAGS) -D__BTP_RECEIVER__ $(LDFLAGS) -o $@ $^ $(LIBS)

example_btp_concurrent_local: example_btp_concurrent_main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	
example_btp_concurrent_remote: example_btp_concurrent_main.c
	$(CC) $(CFLAGS) -D__BTP_REMOTE__ $(LDFLAGS) -o $@ $^ $(LIBS)

example_btp_security_sign_sender: example_btp_security_sign_verify_main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

example_btp_security_verify_receiver: example_btp_security_sign_verify_main.c
	$(CC) $(CFLAGS) -D__BTP_RECEIVER__ $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *~ *.o $(EXAMPLES)
