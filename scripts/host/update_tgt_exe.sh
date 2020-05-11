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

	if [ -z ${TARGET_INTERFACE} ]; then

		echo "Invalid interface"
		FUNC_RESULT=1
	fi
}

function load_input_parameters {

	while [ "$1" != "" ]; do

    		case $1 in 
			
			-i | --interface )
			
				shift
				TARGET_INTERFACE=$1
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

function deploy_to_target {

	sshpass -p $SUDO_PASS scp ../../src/bin/v2x_car root@$TARGET_INTERFACE:/home/root/ext-fs/home/unex/v2x/bin/

	sshpass -p $SUDO_PASS ssh root@$TARGET_INTERFACE sync

	FUNC_RESULT=0
}

function main_function {

	load_input_parameters $@

	check_input_parameters

	if [ 0 != ${FUNC_RESULT} ]; then

		echo "Exiting because of illegal parameter"
		exit 1
	fi

	deploy_to_target

	return 0
}

main_function $@
