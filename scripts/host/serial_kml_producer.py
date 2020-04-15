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

def read_serial_line():
    
    _dictionary = {"tgt_dumbass":"dumbass" }

    _gps = serial.Serial(__serial_interface, baudrate = 115200)
#    _serial_line="""<kml xmlns="http://www.opengis.net/kml/2.2">"""
    while True:
        	_line = _gps.readline()
        	_data = _line.split(",")
                if len(_data) >= 2:
                        _target_name = _data[0]

                        _is_key_found=False
                        for _key in _dictionary:
                            if _key == _target_name:
                                _is_key_found=True
                                break

                        if _is_key_found == False:
                            print "Added new one!"
                            _dictionary[_target_name] = """<kml xmlns="http://www.opengis.net/kml/2.2">"""

                        _serial_line = _dictionary.get(_target_name)

                        if _data[1] == "EVENT":
                            _event = _data[2]
                            _serial_line +="""
    <Style id="balloon_style">
        <IconStyle>
            <scale>3</scale>
            <Icon>
                <href>%s.jpg</href>
            </Icon>
        </IconStyle>
    </Style>""" % (_event)
                            _dictionary[_target_name] = _serial_line

                        elif _data[1] == "$GPRMC":
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

                                        _serial_line +="""
    <Placemark>
        <name>VComm %s</name>
        <description>Live GPS data</description>
        <styleUrl>#balloon_style</styleUrl>
        <Point>
            <coordinates>%s,%s,0</coordinates>
        </Point>
    </Placemark>
</kml>""" % (_target_name, _lon, _lat)

        		        	with open ("/home/s3v3rian/V2X/pbh/target_data/tgt_positional_data_%s.kml" % (_target_name), "w") as pos: pos.write(_serial_line)
                                        _serial_line="""<kml xmlns="http://www.opengis.net/kml/2.2">"""
                                        _dictionary[_target_name] = _serial_line


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
    read_serial_line()
    

if __name__=="__main__":
    main(sys.argv[1:])
