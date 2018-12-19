#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress ip(192, 168, 1, 222);
IPAddress gateway(192, 168, 1, 249);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(192, 168, 1, 249);

char* ssid = "Marquez Correa";
char* password = "Marquez8355196";

ESP8266WebServer server(80);

void wifi_init(){
  WiFi.config(ip, gateway, subnet, DNS);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
  }
  Serial.println();
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Fail connecting");
    delay(5000);
    ESP.restart();
  }

      // Start the server
  server.begin();
  Serial.println("Server started");
  
  // prepare GPIO14
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);
  
  Serial.print("   OK  ");
  Serial.print("Module IP: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  WiFi.hostname("Station_Tester_02");
Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());

  
}

void setup() {
  Serial.begin(115200);
  wifi_init();
}

void loop() {
  // put your main code here, to run repeatedly:

  

}
