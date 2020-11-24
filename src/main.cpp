#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
 
#include <HCSR04.h>
// UltraSonicDistanceSensor distanceSensor(13, 12);  // Initialize sensor that uses digital pins 13 and 12.
UltraSonicDistanceSensor distanceSensor(18, 5);  // Initialize sensor that uses digital pins 5 for trigger and 18 to echo.

//MQTT Setup Start
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
#define mqttTemp "iot2tangle/sensor/ESP32D0WDQ6/tempa"
#define mqttTemp2 "iot2tangle/sensor/ESP32D0WDQ6/tempw"
#define mqttHumDist "iot2tangle/sensor/ESP32D0WDQ6/distance"
#define mqttHumVol "iot2tangle/sensor/ESP32D0WDQ6/volume"
//MQTT Setup End

//HTTP Setup Start
#include <HTTPClient.h>
//HTTP Setup End
 
// Adafruit_BME280 bme1; // I2C
// Adafruit_BME280 bme2; // I2C
// #define DHTPIN 33
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);
 
float temp1, temp2,tempExt, hum1, hum2, humExt;
double distance, volume;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

// JSON data buffer
#include <ArduinoJson.h>
StaticJsonDocument<250> jsonDocument;
char buffer[250];

#define LED 2
#define VALVE 19

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(VALVE, OUTPUT);

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
  // set the callback function
  client.setCallback(callback);
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
   
    if (client.connect("streamsGateway")) {
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
  char totext[64];
  temp1 = 14.8;
  temp2 = temp1;
  distance = distanceSensor.measureDistanceCm(temp1);
  Serial.print("Distance: ");
  Serial.println(distance);
  
  if (distance < 0) {
    distance = 30.1;
  };
 
  double dWaterHeight = (Height + 10 - distance);
  double dSurfaceRadius = (BottomRadius + (dWaterHeight / 10));

  volume = ((Pi * dWaterHeight / 3) * ((dSurfaceRadius * dSurfaceRadius) + (BottomRadius * BottomRadius) + (dSurfaceRadius * BottomRadius)));
  strcpy(buffer, "{\"iot2tangle\":[");
	strcat(buffer,    "{\"sensor\":\"GPS\",\"data\":[");
	strcat(buffer,      "{\"latitude \": \"52.21492\" }");
	strcat(buffer,      ",{ \"longitude\": \"6.9773493\" }");
	strcat(buffer,    "] },");
	strcat(buffer,    "{ \"sensor\": \"HCSR04\", \"data\": [ ");
  sprintf(totext, "{ \"cm\": \"%.3f\" }", distance);
	strcat(buffer,totext);
	// strcat(buffer,      "{ \"cm\": \"29.1\" }");
	strcat(buffer,    "] },");
	strcat(buffer,    "{ \"sensor\": \"barrel\", \"data\": [ ");
  sprintf(totext, "{ \"volume\": \"%.6f\" }", volume);
	strcat(buffer,totext);
	//	strcat(buffer,      "{ \"volume\": \"380727.84\" }");
	strcat(buffer,    "] }");
	strcat(buffer,  "], ");
	strcat(buffer,  "\"device\": \"ESP32D0WDQ6\",");
	strcat(buffer,  "\"timestamp\": 1558511111 ");
	strcat(buffer, "}");

  // strcpy( buffer, "{\"iot2tangle\": [ { \"sensor\": \"GPS\", \"data\": [ { \"latitude \": \"52.21492\" }, { \"longitude\": \"6.9773493\" }] }, { \"sensor\": \"HCSR04\", \"data\": [ { \"cm\": \"29.1\" } ] }, { \"sensor\": \"barrel\", \"data\": [ { \"volume\": \"380727.84\" } ] } ], \"device\": \"ESP32D0WDQ6\", \"timestamp\": 1558511111 }");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED, HIGH);
  digitalWrite(VALVE, HIGH);
  Serial.println("Valve is open");

  if (!client.connected()){
    reconnect();
  }
  getValues();
 
  client.publish(mqttTemp, String(temp1).c_str(),true);
  client.publish(mqttTemp2, String(temp2).c_str(),true);
  client.publish(mqttHumDist, String(distance).c_str(),true);
  client.publish(mqttHumVol, String(volume).c_str(),true);
  
  delay(5000);
  digitalWrite(LED, LOW);
  digitalWrite(VALVE, LOW);
  Serial.println("Valve is closed");
  delay(5000);
    //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // Send HTTP POST request
      // int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // '{ "iot2tangle": [ { "sensor": "Gyroscope", "data": [ { "x": "4514" }, { "y": "244" }, { "z": "-1830" } ] }, { "sensor": "Acoustic", "data": [ { "mp": "1" } ] } ], "device": "DEVICE_ID_1", "timestamp": 1558511111 }'
      // int httpResponseCode = http.POST("{\"iot2tangle\": [ { \"sensor\": \"GPS\", \"data\": [ { \"latitude \": \"52.21492\" }, { \"longitude\": \"6.9773493\" }] }, { \"sensor\": \"HCSR04\", \"data\": [ { \"cm\": \"29.1\" } ] }, { \"sensor\": \"barrel\", \"data\": [ { \"volume\": \"380727.84\" } ] } ], \"device\": \"ESP32D0WDQ6\", \"timestamp\": 1558511111 }");
      int httpResponseCode = http.POST(buffer);

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String payload = "{}";   
      if (httpResponseCode>0) {
        payload = http.getString();
        Serial.print("HTTP Payload: ");
        Serial.println(payload);
      }        
      // Free resources

      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}