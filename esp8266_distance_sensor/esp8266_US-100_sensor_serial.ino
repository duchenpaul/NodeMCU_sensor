/*
Detect distance using serial mode, short the jumper.
Disconnect the tx rx wire while uploading.
reconnect after finishes


*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>;
 
const int US100_TX = 13;
const int US100_RX = 15;
 

const char* ssid = "Lisa's";
const char* password =	"1qazxsw2";

const char* mqttServer = "192.168.2.103";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* topic = "ESP8266/US-100";
const char* temperature_topic = "ESP8266/sensor/US-100/temperature";
const char* distance_topic = "ESP8266/sensor/US-100/distance";

WiFiClient espClient;
PubSubClient client(espClient);
SoftwareSerial serialUS100(US100_RX, US100_TX);

int temp = 0;
unsigned int mmDist = 0;
unsigned int MSByteDist = 0;
unsigned int LSByteDist = 0;

void setup(){ 
	Serial.begin(9600);			// 测量结果将通过此串口输出至PC 上的串口监视器
	serialUS100.begin(9600);
 
	WiFi.begin(ssid, password);
	 
	while (WiFi.status() != WL_CONNECTED) {
	delay(500);
	Serial.print("Connecting to WiFi..");
	}
	 
	Serial.println("Connected to the WiFi network");

	client.setServer(mqttServer, mqttPort);
	// client.setCallback(callback);
 
	while (!client.connected()) {
		Serial.println("Connecting to MQTT...");
 
		if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
			Serial.println("connected");	
 
		} else {
 
			Serial.print("failed with state ");
			Serial.print(client.state());
			delay(2000);
 
		}
	}
 
	client.publish(topic, "Hello from ESP8266/US-100");
	client.publish(topic, "Measure distance, unit: mm");
	client.subscribe(topic);

}


int measure_distance() {
	serialUS100.flush(); 
	serialUS100.write(0x55);
	delay(500);
	if(serialUS100.available() >= 2)
		{
			MSByteDist = serialUS100.read();
			LSByteDist	= serialUS100.read();
			mmDist	= MSByteDist * 256 + LSByteDist; 
			if((mmDist > 1) && (mmDist < 10000))
			{
				Serial.print("Distance: ");
				Serial.print(mmDist, DEC);
				Serial.println(" mm");
				return mmDist;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
}

int measure_temperature() {
	serialUS100.flush();
	serialUS100.write(0x50); 
	delay(500);
	if(serialUS100.available() >= 1) 
	 	{
			temp = serialUS100.read();
			if((temp > 1) && (temp < 130)) 
			{
				temp -= 45; 
				Serial.print("Temperature: ");
				Serial.print(temp, DEC);
				Serial.println(" 'C.");
				return temp;
			}
		}
	else {
		return -1;
	}
} 

void publish_data(int a, const char* topic, char* type) {
	// convert to char
	char str[12];
	// sprintf(str, "{%s: %d}", topic, a);
	sprintf(str, "{'%s': '%d'}", type, a);
	// client.loop();
	client.publish(topic, str);
}

void loop(){
	publish_data(measure_distance(), distance_topic, "distance");
	publish_data(measure_temperature(), temperature_topic, "temperature");
	// delay(500); // 每秒测量2次

}