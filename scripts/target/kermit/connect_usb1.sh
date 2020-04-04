#!/usr/bin/kermit +

# Set connection parameters.
set line /dev/ttyUSB1
set speed 115200
set carrier-watch off
set flow-control none
set prefixing all
set parity none
set stop-bits 1
set modem none

# Connect to the target.
connect
#server cd /home/root/ext-fs/home/unex/v2x/bin/

# Disconnect from the target.
#exit
