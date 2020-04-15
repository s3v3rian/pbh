#!/bin/bash

function main() {

	WS_DIR=$PWD/../../
	TMP_DIR=/tmp/

	echo "Preparing target directory..."

	# Prepare target directory.
	mkdir $TMP_DIR/v2x/

	cp -R $WS_DIR/src/bin/ $TMP_DIR/v2x/
	cp -R $WS_DIR/src/config/ $TMP_DIR/v2x/bin/

	echo "Packaging target directory..."

	cd $TMP_DIR
	tar -cf v2x_tgt.tar v2x
	cd -

	echo "Copying to target ttyUSB$1..."
	$PWD/../target/kermit/deploy_to_tgt.sh $1 $TMP_DIR/v2x_tgt.tar

	echo "Doing cleanup..."

	# Cleanup.
	rm -R $TMP_DIR/v2x*
}

main $@
