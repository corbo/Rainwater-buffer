#include <Arduino.h>
#include <WiFi.h>
 
const char* ssid     = "VFNL-EF9900"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* password = "53X6JJE324KTX"; 

#include <HCSR04.h>
UltraSonicDistanceSensor distanceSensor(13, 12);  // Initialize sensor that uses digital pins 13 and 12.


//MQTT Setup Start
#include <PubSubClient.h>
#define mqtt_server "192.168.1.15"
WiFiClient espClient;
PubSubClient client(espClient);
#define mqttTemp1 "growShed/temp1"
#define mqttHum1 "growShed/hum1"
#define mqttTemp2 "growShed/temp2"
#define mqttHum2 "growShed/hum2"
#define mqttTempExt "growShed/tempExt"
#define mqttHumExt "growShed/humExt"
#define mqttHumDist "growShed/distance"
//MQTT Setup End
 
// Adafruit_BME280 bme1; // I2C
// Adafruit_BME280 bme2; // I2C
// #define DHTPIN 33
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);
 
float temp1, temp2,tempExt, hum1, hum2, humExt;
double distance;

#define LED 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  // begin Wifi connect
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //end Wifi connect
 
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  // Loop until we're reconnected
  int counter = 0;
  while (!client.connected()) {
    if (counter==5){
      ESP.restart();
    }
    counter+=1;
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   
    if (client.connect("growTentController")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void getValues() {
  double distance = distanceSensor.measureDistanceCm(temp1);

  // temp1 = bme1.readTemperature();
  // temp2 = bme2.readTemperature();
  // tempExt = dht.readTemperature();
  // hum1 = bme1.readHumidity();
  // hum2 = bme2.readHumidity();
  // humExt = dht.readHumidity();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED, HIGH);
  Serial.println("LED is on");

  if (!client.connected()){
    reconnect();
  }
  getValues();
 
  client.publish(mqttTemp1, String(temp1).c_str(),true);
  client.publish(mqttHum1, String(hum1).c_str(),true);
  client.publish(mqttTemp2, String(temp2).c_str(),true);
  client.publish(mqttHum2, String(hum2).c_str(),true);
  client.publish(mqttTempExt, String(tempExt).c_str(),true);
  client.publish(mqttHumExt, String(humExt).c_str(),true);
  client.publish(mqttHumDist, String(distance).c_str(),true);
  
  delay(5000);
  delay(1000);
  digitalWrite(LED, LOW);
  Serial.println("LED is off");
  delay(1000);
}