#!/bin/bash

function main() {

	sudo screen -h 100000 /dev/ttyUSB$1 115200
}

main $@
