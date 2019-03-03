#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
 
const char* ssid     = "Marquez Correa";
const char* password = "Marquez8355196";
 
void setup()
{
   Serial.begin(115200);
   delay(10);
 
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) { delay(200); }
   
   // Iniciar mDNS a direccion esp8266.local
   if (!MDNS.begin("esp8266")) 
   {             
     Serial.println("Error iniciando mDNS");
   }
   Serial.println("mDNS iniciado");
}
 
void loop()
{
}
