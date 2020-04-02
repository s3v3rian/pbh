#!/usr/bin/env python

import serial
import sys
import getopt

# --------------------------------------------------
# ---------------- Declare variables ---------------
# --------------------------------------------------

__serial_interface="/dev/dumbass"

# --------------------------------------------------
# ---------------- Declare functions ---------------
# --------------------------------------------------

def read_serial_data():
    _gps = serial.Serial(__serial_interface, baudrate = 115200)
    while True:
        	_line = _gps.readline()
        	_data = _line.split(",")
                if len(_data) >= 8:
                        _target_name = _data[0]
                       	if _data[1] == "$GPRMC":
        	            	if _data[3] == "A":
    
		                	_latgps = float(_data[4])
                			if _data[5] == "S":
	        	        		_latgps = -_latgps

        		        	_latdeg = int(_latgps/100)
	        	        	_latmin = _latgps - _latdeg*100
		                	_lat = _latdeg + (_latmin/60)

        		        	_longps = float(_data[6])
	        	        	if _data[7] == "W":
		                		_longps = -_longps

        		        	_londeg = int(_longps/100)
	        	        	_lonmin = _longps - _londeg*100
		                	_lon = _londeg + (_lonmin/60)

        		        	with open ("/home/rdev/V2X/pbh/target_data/tgt_positional_data_%s.kml" % (_target_name), "w") as pos: pos.write(
"""<kml xmlns="http://www.opengis.net/kml/2.2" xmlns:gx="http://www.google.com/kml/ext/2.2">
<Placemark>
    <name>VComm %s</name>
    <description>Live GPS data</description>
    <Point>
        <coordinates>%s,%s,0</coordinates>
    </Point>
</Placemark></kml>""" % (_target_name, _lon, _lat))


def print_parameters():
    print "Using parameters: \nSerial interface: ",__serial_interface


def load_parameters(argv):
    global __serial_interface

    try:
        opts,args = getopt.getopt(argv,"hi:n:",["interface=","name="])
    except getopt.GetoptError:
        print "Failed to parse options"
        return 2
    for opt, arg in opts:
        if opt == '-h':
            print 'script.py -i /tty/USB0'
            return 1
        elif opt in ("-i", "--interface"):
            __serial_interface=arg

    return 0


def main(argv):
    LOAD_RESULT=load_parameters(argv)

    if 1 == LOAD_RESULT:
        sys.exit()

    if 0 != LOAD_RESULT:
        print "Failure loading parameters!"
        sys.exit()

    print_parameters()
    read_serial_data()
    

if __name__=="__main__":
    main(sys.argv[1:])
