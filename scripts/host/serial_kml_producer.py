#!/usr/bin/env python

import serial
import sys
import getopt

# --------------------------------------------------
# ---------------- Declare variables ---------------
# --------------------------------------------------

__serial_interface="/dev/dumbass"
__tgt_data_directory="/home/dumbass"

__default_icon_style_car="""
        <IconStyle>
            <scale>3</scale>
            <Icon>
                <href>car.jpg</href>
            </Icon>
        </IconStyle>"""

# --------------------------------------------------
# ---------------- Declare functions ---------------
# --------------------------------------------------

def read_serial_line():
    
    _dictionary_icon_style = {"tgt_dumbass":"dumbass" }

    _prefix = """<kml xmlns="http://www.opengis.net/kml/2.2">"""
    _icon_style = ""
    _balloon_style = ""
    _did_update_icon_style = 0
    _did_update_dep = 2

    _gps = serial.Serial(__serial_interface, baudrate = 115200)

    while True:

        _line = _gps.readline()
        _data = _line.split(",")

        # If this is a valid line of at least 2 then parse this bitch.
        if len(_data) >= 2:
                    
            _data_index = 0
            
            # Get the target name from first position of line.
            _target_name = _data[_data_index]

            while _data_index < len(_data):

                _data_index += 1

                if(_data_index == len(_data)):
                    break

                # Parse local event.
                if _data[_data_index] == "local_event": #index 1

                    _data_index += 1

                    _event = _data[_data_index]
                    _icon_style=""
                    _icon_style +="""
        <IconStyle>
            <scale>3</scale>
            <Icon>
                <href>%s.jpg</href>
            </Icon>
        </IconStyle>""" % (_event)

                    _dictionary_icon_style[_target_name] = _icon_style

                # Parse LLA data.
                elif _data[_data_index] == "$GPRMC": # index 1

                    _data_index += 1
                    _data_index += 1

        	    if _data[_data_index] == "A": #index 3

                        _data_index += 1
		        _latgps = float(_data[_data_index]) # index 4
                        _longps = float(_data[_data_index + 2]) # index 6
                        _data_index += 1

                        _lat = 0
                        _lon = 0

                	if _data[_data_index] == "S": #index 5 - flip to opposite hemisphere.
	        	    _latgps = -_latgps

        		_latdeg = int(_latgps/100)
	        	_latmin = _latgps - _latdeg*100
		        _lat = _latdeg + (_latmin/60)
                        _data_index += 1
                        _data_index += 1 # skip index 6.

	        	if _data[_data_index] == "W": # index 7 - flip to opposite hemisphere.
	                    _longps = -_longps

        		_londeg = int(_longps/100)
	        	_lonmin = _longps - _londeg*100
		        _lon = _londeg + (_lonmin/60)
                        _data_index += 1
                        _data_index += 1
                        

                        # Get icon style previously saved.
                        _icon_style = _dictionary_icon_style.get(_target_name, __default_icon_style_car)
                        _serial_line ="""
%s
<Document>
    <open>1</open>
    <Style id="icon_style">
    %s
    </Style>
    <LabelStyle>
    	<scale>1</scale>
    </LabelStyle>
    <Placemark>
        <name>VComm %s</name>
        <description>Live GPS data</description>
        <styleUrl>#icon_style</styleUrl>
        <Point>
            <coordinates>%s,%s,0</coordinates>
        </Point>
    </Placemark>
</Document>
</kml>""" % (_prefix, _icon_style, _target_name, _lon, _lat)

                        with open ("/home/s3v3rian/V2X/pbh/target_data/tgt_positional_data_%s.kml" % (_target_name), "w") as pos: pos.write(_serial_line)
            
            # Always save line back into dictionary.
                        _dictionary_icon_style[_target_name] = __default_icon_style_car
#            _dictionary[_target_name] = _serial_line



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
        #elif opt in ("-o", "--output_directory"):
        
    __tgt_data_directory="/home/s3v3rian/V2X/pbh/target_data/"

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
