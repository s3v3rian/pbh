#!/bin/sh

UNEX_HOME=/home/root/ext-fs/home/unex

echo "kill existed processes"
killall -2 chronyd
killall -2 gpsd
killall -2 potid
killall -2 gnd
killall -2 v2xlogd
#killall -2 v2xcastd 

killall -9 chronyd
killall -9 gpsd
killall -9 potid
killall -9 gnd
killall -9 v2xlogd
#killall -9 v2xcastd 

echo "remove original installation"
rm -rf $UNEX_HOME/bin
rm -rf /home/root/ext-fs/home/unex/conf

mkdir -p $UNEX_HOME/bin
mkdir -p $UNEX_HOME/conf
mkdir -p $UNEX_HOME/log
mkdir -p $UNEX_HOME/cert/update_cert
mkdir -p $UNEX_HOME/cert/update_cert/ca
mkdir -p $UNEX_HOME/cert/update_cert/ee

if [ -x common_install.sh ]; then
source common_install.sh
fi

# chrony
echo "install chrony ..."
cp chronyd $UNEX_HOME/bin
cp chronyc $UNEX_HOME/bin
cp chrony.conf $UNEX_HOME/conf
mkdir -p $UNEX_HOME/conf/right
cp UTC $UNEX_HOME/conf/right

# gpsd 
echo "install gpsd ..."
cp gpsd $UNEX_HOME/bin
cp gpxlogger $UNEX_HOME/bin

# logd
echo "install v2xlogd ..."
cp v2xlogd $UNEX_HOME/bin

# potid 
echo "install potid ..."
cp potid $UNEX_HOME/bin
cp mq $UNEX_HOME/bin

# gnd
echo "install gnd ..."
cp gnd.json $UNEX_HOME/conf
cp gnd $UNEX_HOME/bin

# gn-cli
if [ -e gn_cli ]; then
echo "install gn_cli ..."
cp gn_cli $UNEX_HOME/bin
fi

# v2xcast
echo "install v2xcastd ..."
cp v2xcastd $UNEX_HOME/bin
cp v2xcast_itsg5_config_example.json $UNEX_HOME/conf
cp v2xcast_service $UNEX_HOME/bin
cp v2xcast_poti_service $UNEX_HOME/bin

# change file permission
chmod 755 -R /home/root/ext-fs/home/unex/bin

# install at_startup
echo "install at_startup ..."
cp at_startup /home/root/ext-fs/usr/bin/
cp common_startup /home/root/ext-fs/usr/bin/
cp v2xservice_startup /home/root/ext-fs/usr/bin/
chmod 755 /home/root/ext-fs/usr/bin/at_startup
chmod 755 /home/root/ext-fs/usr/bin/common_startup
chmod 755 /home/root/ext-fs/usr/bin/v2xservice_startup

# default certificates and keys
echo "install default certificates & keys"
cp etsi_root.cert /home/root/ext-fs/home/unex/cert/update_cert/ca
cp etsi_aa.cert /home/root/ext-fs/home/unex/cert/update_cert/ca
cp etsi_at_cam.cert /home/root/ext-fs/home/unex/cert/update_cert/ee
cp etsi_at_cam.ecdsa.priv /home/root/ext-fs/home/unex/cert/update_cert/ee
cp etsi_at_den.cert /home/root/ext-fs/home/unex/cert/update_cert/ee
cp etsi_at_den.ecdsa.priv /home/root/ext-fs/home/unex/cert/update_cert/ee
cp etsi_at_gn_mng.cert /home/root/ext-fs/home/unex/cert/update_cert/ee
cp etsi_at_gn_mng.ecdsa.priv /home/root/ext-fs/home/unex/cert/update_cert/ee

# sync to file system
sync

echo "finish installation."
