#!/bin/sh

# udhcp
echo "install udhcpd.conf ..."
cp udhcpd.conf $UNEX_HOME/conf

# GNSS management tools
echo "install GNSS management tools ..."
cp udhcpd.conf $UNEX_HOME/conf
cp teseo3-fw-update $UNEX_HOME/bin
cp gnss-cmd $UNEX_HOME/bin
cp gnss-fw-upg $UNEX_HOME/bin
cp gnss-fw-ver $UNEX_HOME/bin
if [ -e gnss-data-dumper ]; then
cp gnss-data-dumper $UNEX_HOME/bin
fi
cp gnss-redirect $UNEX_HOME/bin
cp gnss-reset $UNEX_HOME/bin

