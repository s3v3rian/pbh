#!/bin/bash

sshpass -p 'rlinux' scp ../../src/bin/v2x_car s3v3rian@192.168.1.23:/home/s3v3rian/V2X/pbh/src/bin/
sshpass -p 'rlinux' scp -r ../../src/config s3v3rian@192.168.1.23:/home/s3v3rian/V2X/pbh/src/
