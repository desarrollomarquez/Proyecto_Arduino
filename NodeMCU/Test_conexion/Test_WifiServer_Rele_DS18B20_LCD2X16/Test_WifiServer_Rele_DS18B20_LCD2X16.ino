
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

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* host = "icaro";
const char* ssid     = "Marquez Correa";
const char* password = "Marquez8355196";

ESP8266WebServer server(80);
float humidity, temp_f, sensorValue;  // Values read from sensor
int val = 1;
int tiempo_apertura = 3000;
String webString="";   
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
// Pin donde se conecta el bus 1-Wire
const int pinDatosDQ = 2 ;
bool s=false;

// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

 
void handle_root() {
  webString="Bienvenido API  server, abrir /temperatura, /close, /open or /api";
  pushMsg(webString);
  delay(100);
}
 
void setup(void)
{
  lcd.begin();   // initializing the LCD
  lcd.backlight();
  

  
  // prepare GPIO16 - PWM
  pinMode(16, OUTPUT);
  pinMode(13, OUTPUT);
  
  sensorDS18B20.begin(); // initialize temperature sensor
  
  Serial.begin(115200);
  
  // Connect to WiFi networkRegístrese hoy mismo 

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
  
  
  
  //Rest API
  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);
  Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  server.on("/", handle_root);
  
  server.on("/temperatura", [](){  
    gettemperatura();       // read sensor
    webString="Temperatura: "+String((int)temp_f)+" Grados";
    pushMsg(webString);
  });

 
  server.on("/open", [](){  
    val = 0;
    webString="Compuerta Abierta: "+String((int)val);
    pushMsg(webString);
  });

  
  server.on("/api", [](){  
    gettemperatura(); // read sensor
    String json="{\"temperatura\":"+String((int)temp_f)+"}";
    Serial.println(json);
    pushMsg(json);
  });
  
  server.begin();
  Serial.println("HTTP server started");
  
  
}
 
void loop(void)
{  
  server.handleClient();
  gettemperatura(); 
  pushLCD();
  pushOpen();
  pushPWM();
  
 
} 


void pushLCD(){
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

void pushOpen(){  
      if(val == 0){
        digitalWrite(13, val);    
        delay(tiempo_apertura);
        val = 1;
      }
      else{
       digitalWrite(13, val); 
       webString="Compuerta Cerrada: "+String((int)val);
       pushMsg(webString);
      }
}

void pushMsg(String json){
    server.send(200, "text/plain", json);
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


