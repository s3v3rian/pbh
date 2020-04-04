#!/bin/bash

#make CC=/opt/poky-craton2/2.2.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc EXTRA_CFLAGS="-std=gnu99 -Wall -Wextra -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb --sysroot=/opt/poky-craton2/2.2.1/sysroots/cortexa7t2hf-neon-poky-linux-gnueabi"

cd /home/s3v3rian/V2X/pbh/src/

make

cd ..

mv src/v2x* src/exe/

./clean.sh

sync
