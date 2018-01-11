# NodeMCU_sensor
IOT project using NodeMCU, raspiberry pi, MQTT and more!

## Feauture:
When the door sensor is triggered, send mail to notify me.

## Hardware:
NodeMCU -- a firmware based on ESP8266 wifi-soc
Raspberry pi

## Software:
Arduino IDE

## Deploy:
1. Install mqtt on Raspberry pi
```bash
sudo apt-get install mosquitto mosquitto-clients
```

2. Test MQTT on Raspberry pi
```
mosquitto_pub  -t "ESP8266/sensor/door" -m "{LED: OFF}"
```

3. Install MQTT package for python3
```
sudo pip3 install paho-mqtt
```

4. Change the connection info accordingly, such as ssid password... Flash `esp8266_door_sensor.ino` into NodeMCU

5. Back to Raspberry pi, run the script
```
python3 mqtt_sensor_sub.py
```

6. On NodeMCU, connect `D1 (GPIO5)` to `GND` to simulate the door closed status and disconnect to simulate the opened status. 