from time import sleep

from MqttClient import MqttHttpClient

mqtt = MqttHttpClient()
topic = "app/events"
message = "red_line_crossing"
for x in range(40):
    res = mqtt.publish(topic, message)
    print "{} Event publish res:{}".format(x+1, res)
    sleep(5)
