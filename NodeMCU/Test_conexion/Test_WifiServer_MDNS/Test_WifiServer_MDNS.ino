#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


ESP8266WebServer server(80);


void setup() {
        MDNS.begin("myesp");
        Serial.begin(115200);   //Start serial connection
        WiFi.begin("Marquez Correa", "Marquez8355196"); //Connect to WiFi network

        while (WiFi.status() != WL_CONNECTED) {  //Wait for the connection to the WiFi network 
        delay(500);
        Serial.print(".");
        }

        if (MDNS.begin("myesp")) {  //Start mDNS
          Serial.println("MDNS started");
        }
        
        server.on("/", handleRoot);  //Associate handler function to path

        server.begin();                           //Start server
        Serial.println("HTTP server started");
}

void loop() { 
  
    server.handleClient();
  
  }

void handleRoot() {

    server.send(200, "text/plain", "Hello resolved by mDNS!");

}


