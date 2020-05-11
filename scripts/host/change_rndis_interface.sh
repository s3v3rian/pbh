#!/bin/bash

# Global Variables.
SUDO_PASS="rlinux"
FUNC_RESULT=0

function print_help_menu {

	echo "Print help menu!"

	FUNC_RESULT=0
}

function check_input_parameters {

	FUNC_RESULT=0

	if [ -z ${INTERFACE_TO_CHANGE} ]; then

		echo "Invalid interface"
		FUNC_RESULT=1
	fi
}

function load_input_parameters {

	while [ "$1" != "" ]; do

    		case $1 in 
			
			-i | --interface )
			
				shift
				INTERFACE_TO_CHANGE=$1
				;;

        		-h | --help )

		     		print_help_menu
                                exit
                                ;;

    		esac
    		shift
	done

	FUNC_RESULT=0
}

function change_interface {

#	sshpass -p $SUDO_PASS ssh root@192.168.1.3 "ifconfig usb0 192.168.2.3 255.255.255.0"

	echo $SUDO_PASS | sudo -S ifconfig $INTERFACE_TO_CHANGE 192.168.2.1/24

	echo $SUDO_PASS | sudo -S route add -net 192.168.2.0 netmask 255.255.255.0 dev $INTERFACE_TO_CHANGE

	FUNC_RESULT=0
}

function main_function {

	load_input_parameters $@

	check_input_parameters

	if [ 0 != ${FUNC_RESULT} ]; then

		echo "Exiting because of illegal parameter"
		exit 1
	fi

	change_interface

	return 0
}

main_function $@
