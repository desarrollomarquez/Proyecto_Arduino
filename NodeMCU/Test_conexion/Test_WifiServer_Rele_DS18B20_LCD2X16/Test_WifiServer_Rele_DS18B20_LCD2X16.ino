
/* 
NodeMCU esp8266 and 1602A LCD

Please connect 1602A using I2C backpack.
If you feel display is not showing anything, please take a screwdriver and update contrast 
using I2C potentiometer in back of LCD

LCD config
  SDA pin - D2
  SCL pin - D1
  VCC Pin - 5v
  
  
Sensor Config
  Sensor Data pin - RX
  Sensor VCC - 3.3 volt pin

  If you change above pin please update config below. 
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library >
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Stepper.h>
// Esto es el número de pasos en un minuto




// Constructor, pasamos STEPS y los pines donde tengamos conectado el motor
Stepper stepper(200, 2, 14, 12, 13);
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* host = "icaro";
const char* ssid     = "Marquez Correa";
const char* password = "Marquez8355196";

ESP8266WebServer server(80);
float humidity, temp_f, sensorValue;  // Values read from sensor
int val;
String webString="";   
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
// Pin donde se conecta el bus 1-Wire
const int pinDatosDQ = 0 ;
bool s=false;

// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

 
void handle_root() {
  server.send(200, "text/plain", "Bienvenido API  server, abrir /temperatura, /close, /open or /api");
  delay(100);
}
 
void setup(void)
{
  lcd.begin();   // initializing the LCD
  lcd.backlight();
  // prepare stepper
  stepper.setSpeed(100);
  // prepare GPIO15 - PWM
  pinMode(15, OUTPUT);
  
  sensorDS18B20.begin(); // initialize temperature sensor
  
  Serial.begin(115200);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  
  
  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);
  Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  server.on("/", handle_root);
  
  server.on("/temperatura", [](){  
    gettemperatura();       // read sensor
    webString="Temperatura: "+String((int)temp_f)+" Grados";
    server.send(200, "text/plain", webString);
  });


   server.on("/close", [](){
    compuertaStepClose();
    webString="Compuerta Cerrada";
    server.send(200, "text/plain", webString);
  });

  
  server.on("/open", [](){  
    compuertaStepOpen();
    webString="Compuerta Abierta";
    server.send(200, "text/plain", webString);
  });
  

 
 //Rest API for sensor data
  server.on("/api", [](){  
    gettemperatura();           // read sensor
    String json="{\"temperatura\":"+String((int)temp_f)+"}";
    Serial.println(json);
    server.send(200, "application/json", json);
  });
  

  server.begin();
  Serial.println("HTTP server started");
  
  
}
 
void loop(void)
{  
  server.handleClient();
  gettemperatura();
  //digitalWrite(14, val);
  pushPWM();
  lcd.setCursor(0, 0);
  String hs="IP:"+String( WiFi.localIP().toString().c_str());
  String ts="Temp: "+String((int)temp_f)+" C ";
  lcd.setCursor(0, 0);
  lcd.print(ts);
  lcd.setCursor(0, 1);
  lcd.print(hs);

 
} 


void gettemperatura() {
       unsigned long currentMillis = millis();
     if(currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        sensorDS18B20.requestTemperatures();
        temp_f =  sensorDS18B20.getTempCByIndex(0);
      }
  }

void compuertaStepOpen() {
  for (int x=0; x< 8; x++) {
    stepper.step(200);
    delay(500);
  }
}

void compuertaStepClose() {
   for (int x=0; x< 8; x++) {
    stepper.step(-200);
    delay(500);
  }
}


void pushPWM() {
    
    for (int PWM_duty = 0; PWM_duty < 1023; PWM_duty++) 
    {
      analogWrite(15, PWM_duty);
      delay(1);
    }
    delay(500);
    for (int PWM_duty = 1023; PWM_duty >= 0; PWM_duty--) 
    {
      analogWrite(15, PWM_duty);
      delay(1);
    }
    delay(500);
  }


