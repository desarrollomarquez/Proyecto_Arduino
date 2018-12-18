#include <ESP8266WiFi.h>

char* ssid = "Marquez Correa";
char* password = "Marquez8355196";

IPAddress staticIP(192,168,1,22);
IPAddress gateway(192,168,1,9);
IPAddress subnet(255,255,255,0);

WiFiServer server(80);

void setup(void)
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Conectando a %s\n", ssid);
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
    // Start the server
  server.begin();
  Serial.println("Server started");
  
  // prepare GPIO14
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);


  
  Serial.println();
  Serial.print("Conectado, dirección IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
   // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/rele/0") != -1)
    val = 0;
  else if (req.indexOf("/rele/1") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(14, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\r\n<html>\r\nEl rele esta ";
  s += (val)?"activado":"desactivado";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
  
  
  
  }
