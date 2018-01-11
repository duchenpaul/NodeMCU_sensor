import paho.mqtt.client as mqtt
import subprocess, time, os

config = {
	'host' : '192.168.2.103',
	'port' : 1883,
	'keepalive' :  60,
}

topic = 'ESP8266/sensor/door'

def send_alart(door_status):
	subject = "Door status is " + door_status
	content = '\"' + subject + " on " + time.strftime("%Y-%m-%d %X", time.localtime()) + '\"'
	print("Sending mail to notify " + subject)
	cmd = os.path.dirname(os.path.realpath(__file__)) + "/send_mail.py " + '\"' + subject + '\"' + " " + " " + content
	print(cmd)
	subprocess.call([os.path.dirname(os.path.realpath(__file__)) + "/send_mail.py", '\"' + subject + '\"', content], executable=None, shell=False)


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(topic)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+": "+msg.payload.decode('utf-8'))
    message = msg.payload.decode('utf-8')
    if message == '{doorIsOpen: 1}':
    	send_alart('ON')
    elif message == '{doorIsOpen: 0}':
    	send_alart('OFF')


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message


client.connect(**config)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
