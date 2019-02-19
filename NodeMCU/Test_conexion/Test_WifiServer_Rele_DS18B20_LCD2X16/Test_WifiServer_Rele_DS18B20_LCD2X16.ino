
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
float temp_hot, temp_cold;  // Values read from sensor
int val = 1;
int tiempo_apertura = 3000;
int start_pwm;
int PWM_duty = 0;
String webString="";
String lcdString="";    
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
// Pin donde se conecta el bus 1-Wire
const int pinDatosDQHot  = 0;
const int pinDatosDQCold = 2;
bool s=false;

// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObjetoHot(pinDatosDQHot);
OneWire oneWireObjetoCold(pinDatosDQCold);
DallasTemperature sensorDS18B20Hot(&oneWireObjetoHot);
DallasTemperature sensorDS18B20Cold(&oneWireObjetoCold);

 
void handle_root() {
  webString="Bienvenido API REST  server: Metodos  /open -> Abrir Compuerta; /temperaturas -> Mostrar Temperaturas Hot y Cold";
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
  
  sensorDS18B20Hot.begin(); // initialize temperature sensor
  sensorDS18B20Cold.begin(); 
  
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
  
  server.on("/temperaturas", [](){  
    gettemperatura();       // read sensor
    webString="Temperatura_Hot: "+String((int)temp_hot)+" Grados"+"Temperatura_Cold: "+String((int)temp_cold)+" Grados";
    pushMsg(webString);
  });

 
  server.on("/open", [](){  
    val = 0;
    webString="Compuerta Open: "+String((int)val);
    pushMsg(webString);
  });

  server.on("/cook/1", [](){  
    start_pwm = 1;
    webString="Cocinar: "+String((int)start_pwm);
    pushMsg(webString);
  });

  
  server.on("/cook/0", [](){  
    start_pwm = 0;
    webString="No Cocinar: "+String((int)start_pwm);
    pushMsg(webString);
    
  });
  
  server.on("/api", [](){  
    gettemperatura(); // read sensor
    String json="{\"temperatura_hot\":"+String((int)temp_hot)+"}, {\"temperatura_cold\":"+String((int)temp_cold)+"}";
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
  pushOpen();
  pushPWM();
  
 
} 


void pushLCD(String msg){
      lcd.setCursor(0, 0);
      String hs="IP:"+String( WiFi.localIP().toString().c_str());
      lcd.setCursor(0, 0);
      lcd.print(msg);
      lcd.setCursor(0, 1);
      lcd.print(hs);  
}

void gettemperatura() {
     unsigned long currentMillis = millis();
     if(currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        sensorDS18B20Hot.requestTemperatures();
        temp_hot =  sensorDS18B20Hot.getTempCByIndex(0);
        sensorDS18B20Cold.requestTemperatures();
        temp_cold =  sensorDS18B20Cold.getTempCByIndex(0);
      }
      lcdString ="Hot:"+String((int)temp_hot)+"C"+" Cold:"+String((int)temp_cold)+"C";
      pushLCD(lcdString);
}

void pushOpen(){  
      if(val == 0){
        digitalWrite(13, val);    
        delay(tiempo_apertura);
        val = 1;
        pushLCD(" Compuerta Open ");
        
      }
      else{
       digitalWrite(13, val); 
       webString="Compuerta Close: "+String((int)val);
       pushMsg(webString);
      }
}

void pushMsg(String json){
    server.send(200, "text/plain", json);
}

void pushPWM() {

     if(start_pwm == 1){
           
           if(temp_hot <= 42){
              analogWrite(15, PWM_duty);
              delay(1000);    
              PWM_duty++;
            }  
        
      }
      if(start_pwm == 0){
           PWM_duty = 0;
           analogWrite(15, 0);
           delay(10);
      }
      
}


