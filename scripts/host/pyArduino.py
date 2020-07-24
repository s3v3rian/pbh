import serial

arduinoData = serial.Serial('/dev/ttyUSB0',9600)


def led_on();
	arduinoData.write('1')

def led_off();
	arduinoData.write('0')

t=0
while (t<3000000):
	t+=1

led_on()

print("done")

#arduino --upload VComm4/VComm4.ino --port /dev/ttyUSB0
