#include <ESP8266WiFi.h> 
 
const char* ssid     = "pegajoso";
const char* password = "pegajososiempre";
 
IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
 
void setup()
{
   Serial.begin(115200);
   delay(10);
   Serial.println();
 
   WiFi.softAP(ssid, password);
   WiFi.softAPConfig(ip, gateway, subnet);
   Serial.print("Iniciado AP:\t");
   Serial.println(ssid);
 
   Serial.print("IP address:\t");
   Serial.println(WiFi.softAPIP());
}
 
void loop() { }
