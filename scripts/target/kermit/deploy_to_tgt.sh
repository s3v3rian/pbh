#!/usr/bin/kermit +

# Set connection parameters.
set line /dev/ttyUSB\%1
set speed 115200
set carrier-watch off
set flow-control none
set prefixing all
set parity none
set stop-bits 1
set modem none

set reliable
set window 32
set send packet-length 9024

# Connect to the target.
#remote cd /home/root/ext-fs/home/unex/
send \%2

exit
