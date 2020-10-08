#!/usr/bin/env python

import serial
import socket
import sys
import getopt
import subprocess
from MqttClient import MqttHttpClient

# --------------------------------------------------
# ---------------- Declare variables ---------------
# --------------------------------------------------

__udp_interface="127.0.0.1"
__tgt_data_directory="/home/dumbass"

# --------------------------------------------------
# ---------------- Declare functions ---------------
# --------------------------------------------------

def read_data_line():
    
    _dictionary_icon_style = {"tgt_dumbass":"dumbass" }
    _dictionary_balloon_style = {"tgt_dumbass":"dumbass" }

    _prefix = """<kml xmlns="http://www.opengis.net/kml/2.2">"""
    _icon_style = ""
    _balloon_style = ""

    try:
        _serial_accessor = serial.Serial('/dev/ttyUSB0', 9600)
    except:
        print("FAILED TO OPEN USB0")
    _sock_fd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    _sock_fd.bind((__udp_interface, 12355))

    while True:

        _line, _addr = _sock_fd.recvfrom(1024)
        print _line
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
                if _data[_data_index] == "local_event" and _target_name == "T666": #index 1

                    _data_index += 1

                    _event = _data[_data_index]
                    if _event == "signal_violation":
                        _icon_style +="""
        <IconStyle>
            <scale>3</scale>
            <Icon>
                <href>%s.jpg</href>
            </Icon>
        </IconStyle>""" % (_event)
   
                    elif _event == "nothing":
                        _icon_style += """
        <IconStyle>
            <scale>1.5</scale>
            <Icon>
                <href>tl_car_%s.jpg</href>
            </Icon>
        </IconStyle>""" % (_target_name)

                    _dictionary_icon_style[_target_name] = _icon_style

                elif _data[_data_index] == "$VSU":

                    _data_index += 1

                    _license = _data[_data_index]
                    _balloon_style = """
        <BalloonStyle>
     	    <bgColor>fffffffb</bgColor>
            <text><![CDATA[
                <b><font color="#CC0000" size="+3">$[name]</font></b>
                <br/><br/>
                <font face="Courier" size="+4">$[description]</font>
		<br/><br/>
                License Plate Number: %s 
                <br/><br/>
                ]]></text>
        </BalloonStyle>""" % (_license)
    
                    _dictionary_balloon_style[_target_name] = _balloon_style

                elif _data[_data_index] == "local_event" and _target_name == "T111": #index 1

                    _data_index += 1

                    _event = _data[_data_index]
                    
                    if _event == "green_light":   
		        _serial_accessor.write("C0")

                    elif _event == "red_light":
                        _serial_accessor.write("C1")

                    if _event == "signal_violation":
                        _icon_style +="""
        <IconStyle>
            <scale>3</scale>
            <Icon>
                <href>triangle.jpg</href>
            </Icon>
        </IconStyle>"""
		    
#                        subprocess.call(["/home/s3v3rian/V2X/pbh/scripts/host/lightON.ps1"])

                    elif _event == "red_light_crossing":
                        res = mqtt.publish(topic, message)
			print(res) # expecting 0 result for success
			sys.exit()
                        print("RECEIVED RED LIGHT CROSSING EVENT")
    			
                    else:
                        _icon_style += """
        <IconStyle>
            <scale>3</scale>
            <Icon>
                <href>%s.jpg</href>
            </Icon>
        </IconStyle>""" % (_event)
            
#                        subprocess.call(["/home/s3v3rian/V2X/pbh/scripts/host/lightOFF.ps1"])

                    _dictionary_icon_style[_target_name] = _icon_style

                elif _data[_data_index] == "remote_event":

                    _data_index += 1

                    _event = _data[_data_index]

                    if _event == "red_light_crossing":

                        res = mqtt.publish(topic, message)
			#print(res) # expecting 0 result for success
                        print("RSU RECEIVED RED LIGHT CROSSING EVENT")
    			
                    else:
			
                        _icon_style += """

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
                        _icon_style = _dictionary_icon_style.get(_target_name, """
        <IconStyle>
            <scale>1.5</scale>
            <Icon>
                <href>tl_car_%s.jpg</href>
            </Icon>
        </IconStyle>""" % (_target_name))

                        # Get balloon style previously saved.
                        _balloon_style = _dictionary_balloon_style.get(_target_name, """
        <BalloonStyle>
        </BalloonStyle>""")

                        _serial_line ="""
%s
<Document>
    <open>1</open>
    <Style id="icon_style">
    %s
    %s
    </Style>
    
    <Placemark>
        <name>VComm %s</name>
        <description>eScooter Data</description>
        <styleUrl>#icon_style</styleUrl>
	<Point>
            <coordinates>%s,%s,0</coordinates>
        </Point>
    </Placemark>
</Document>
</kml>""" % (_prefix, _balloon_style, _icon_style, _target_name, _lon, _lat)

                        with open ("/home/s3v3rian/V2X/pbh/target_data/tgt_positional_data_%s.kml" % (_target_name), "w") as pos: pos.write(_serial_line)
            
            # Always save line back into dictionary.
                        _dictionary_icon_style[_target_name] = """
        <IconStyle>
            <scale>2</scale>
            <Icon>
                <href>tl_car_%s.jpg</href>
            </Icon>
        </IconStyle>""" % (_target_name)

            # Always save line back into dictionary.
                        _dictionary_balloon_style[_target_name] = """
        <BalloonStyle>
        </BalloonStyle>"""
#            _dictionary[_target_name] = _serial_line

    _serial_accessor.close()


def print_parameters():
    print "Using parameters: \nUDP interface: ",__udp_interface


def load_parameters(argv):
    global __udp_interface

    try:
        opts,args = getopt.getopt(argv,"hi:n:",["interface=","name="])
    except getopt.GetoptError:
        print "Failed to parse options"
        return 2
    for opt, arg in opts:
        if opt == '-h':
            print 'script.py -i 192.168.1.1'
            return 1
        elif opt in ("-i", "--interface"):
            __udp_interface=arg
        #elif opt in ("-o", "--output_directory"):
        
    __tgt_data_directory="/home/s3v3rian/V2X/pbh/target_data/"

    return 0


def main(argv):
    LOAD_RESULT=load_parameters(argv)
    global mqtt
    global topic
    global message
    topic = "app/events"
    message = "red_line_crossing"
    mqtt = MqttHttpClient()
    #topic = "app/events"
    #message = "red_line_crossing"
    #res = mqtt.publish(topic, message)
    #print(res)
    #sys.exit()
    if 1 == LOAD_RESULT:
        sys.exit()

    if 0 != LOAD_RESULT:
        print "Failure loading parameters!"
        sys.exit()

    print_parameters()
    read_data_line()

if __name__=="__main__":
    main(sys.argv[1:])
