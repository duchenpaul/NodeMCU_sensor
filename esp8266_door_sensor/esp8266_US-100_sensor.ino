#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Lisa's";
const char* password =  "1qazxsw2";

const char* mqttServer = "192.168.2.103";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* topic = "ESP8266/sensor/US-100";


unsigned int echoPin = 13;   // 将Arduino 的Pin2 连接至US-100 的Echo/RX
unsigned int trigPin = 12;   // 将Arduino 的Pin3 连接至US-100 的Trig/TX

WiFiClient espClient;
PubSubClient client(espClient);

void setup(){ 
  Serial.begin(9600);       // 测量结果将通过此串口输出至PC 上的串口监视器
  pinMode(echoPin, INPUT);  // 设置echoPin 为输入模式。
  pinMode(trigPin, OUTPUT); // 设置trigPin 为输出模式。
 
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
  unsigned long time_echo_us = 0;
  unsigned long dist_mm = 0;
  
  // 通过Trig/Pin 发送脉冲，触发US-100 测距
  digitalWrite(trigPin, LOW);   // 先拉低，以确保脉冲识别正确
  delayMicroseconds(2);         // 等待2us
  digitalWrite(trigPin, HIGH);  // 开始通过Trig/Pin 发送脉冲
  delayMicroseconds(12);        // 设置脉冲宽度为12us (>10us)
  digitalWrite(trigPin, LOW);   // 结束脉冲
  
  time_echo_us = pulseIn(echoPin, HIGH);          // 计算US-100 返回的脉冲宽度
  if((time_echo_us < 60000) && (time_echo_us > 1))// 脉冲有效范围(1, 60000).
  {
    // dist_mm = (time_echo_us * 0.34mm/us) / 2 (mm)
    dist_mm = time_echo_us*5/29;        // 通过脉冲宽度计算距离.
    
    Serial.print("Distance is: ");      // 输出结果至串口监视器
    Serial.print(dist_mm, DEC);         // 输出结果至串口监视器
    Serial.println("mm");               // 输出结果至串口监视器
    return int(dist_mm);
  }
  else {
    measure_distance(); 
  }

}

int dist = 0; 

void loop(){
  dist = measure_distance();
  // convert to char
  char str[12];
  sprintf(str, "%d", dist);
  
  client.loop();
  client.publish(topic, str);
  delay(500); // 每秒测量2次
}