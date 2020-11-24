const char* ssid     = "<Your SSID>"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* password = "<Your Wifi password>"; 

const char* devicename = "ESP32D0WDQ6";
#define mqtt_server "192.168.1.15"

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.1.21:8080/sensor_data";

const double Pi = 3.1415926535897932384626433;

// Metrics of the rainbarrel, in this case a cut off conical bucket
//   Radius at the maximum waterlevel
const float TopRadius = 45.75;
//   Radius at the bottom
const float BottomRadius = 35.001;
//   Height when water is at maximum waterlevel
const float Height = 98.5;
