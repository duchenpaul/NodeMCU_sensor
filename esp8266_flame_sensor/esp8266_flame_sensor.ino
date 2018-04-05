#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "Lisa's";
const char* password =  "1qazxsw2";
const char* mqttServer = "192.168.2.103";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* topic = "ESP8266/sensor/flame";

const int sensorPin = 5;
bool isOpen = false;

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  
  Serial.begin(115200);
  //initialize the light as an output and set to LOW (off)
  pinMode(sensorPin, INPUT_PULLUP);

  // config LED [ on digitalWrite(LED_BUILTIN, LOW); off digitalWrite(LED_BUILTIN, HIGH);]
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
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
 
  client.publish(topic, "Hello from ESP8266");
  client.subscribe(topic);
 
}
 

// Callout when status changed
void changeDetectFlame(int GPIO_PIN){
  //grab the current door state
  bool sensorState = digitalRead(GPIO_PIN);    //LOW is closed HIGH is open
  Serial.println(digitalRead(GPIO_PIN));

  if(!sensorState && !isOpen){  //if door is open and the state closed, publish
    client.publish(topic,"{FlameDetected: 0}");  //send closed
    isOpen = true;
    Serial.println("Flame not detected");
  }
  else if(sensorState && isOpen){   //if door is closed and the state is open, publish
    client.publish(topic,"{FlameDetected: 1}");  //send closed
    isOpen = false;
    Serial.println("Flame detected!");
  }
}


void detectFlame(int GPIO_PIN){
  //grab the current door state
  bool sensorState = digitalRead(GPIO_PIN);    //LOW is 1 HIGH is 0
  Serial.println(digitalRead(GPIO_PIN));

  if(sensorState){  
    client.publish(topic,"{FlameDetected: 0}");  
    isOpen = true;
    Serial.println("Flame not detected");
  }
  else {  
    client.publish(topic,"{FlameDetected: 1}"); 
    isOpen = false;
    Serial.println("Flame detected!");
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
 
  
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  
  String payload_str = "";
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payload_str += (char)payload[i];
  }

  if (payload_str == "{LED: ON}")
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (payload_str == "{LED: OFF}")
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Serial.println();
  Serial.println("-----------------------");
 
}

void loop() {
  detectFlame(sensorPin);
  client.loop();
  delay(500);

}