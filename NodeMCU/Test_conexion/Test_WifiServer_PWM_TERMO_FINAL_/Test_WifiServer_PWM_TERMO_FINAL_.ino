

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

//const char* ssid     = "pegajoso";
//const char* password = "pegajososiempre";

ESP8266WebServer server(80);


float temp_hot, temp_cold;  // Values read from sensor
int val = 1;
int start_cook = 0;
int tiempo_apertura = 3000;
int start_pwm = 5;
int valor_pwm = 682;
int salidaPWM = 15;  // salida de señal PWM


//Mensajes del API y LCD
String webString="";
String lcdString="";    
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

// Pin donde se conecta el bus 1-Wire
const int pinDatosDQHot  = 0;
const int pinDatosDQCold = 2;


// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObjetoHot(pinDatosDQHot);
OneWire oneWireObjetoCold(pinDatosDQCold);
DallasTemperature sensorDS18B20Hot(&oneWireObjetoHot);
DallasTemperature sensorDS18B20Cold(&oneWireObjetoCold);




void handle_root() {
  webString="Welcome API REST CSV  server: Metodos  /open -> Abrir Compuerta; /temperature -> Mostrar Temperaturas Hot y Cold; /cook/1 -> Cocinar - /cook/0 -> No cocinar";
  pushMsg(webString);
  delay(100);
}
 
void setup(void)
{
  lcd.begin();   // initializing the LCD
  lcd.backlight();
    
  // prepare GPIO15 - PWM
  pinMode(salidaPWM, OUTPUT);

  sensorDS18B20Hot.begin(); // initialize temperature sensor
  sensorDS18B20Cold.begin(); 

  Serial.begin(115200);
  
  // Connect to WiFi networkRegístrese hoy mismo 
  Serial.print("\n\r \n\rWorking to connect");
  WiFi.begin(ssid, password);
  // Wait for connection

  // Conectado; imprimimos la IP local por Serial.
  Serial.println(WiFi.localIP());
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  
  //Serial.println("");
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

 
  server.on("/open", [](){  
    val = 0;
    String json="{\"Open\":"+String((int)val)+" }";
    pushMsg(json);
  });
  
  server.on("/closed", [](){  
    val = 1;
    String json="{\"Closed\":"+String((int)val)+" }";
    pushMsg(json);
  });
  

  server.on("/cook/1", [](){  
    start_cook = 1;
    String json="{\"Cook\":"+String((int)start_cook)+" }";
    pushMsg(json);
  });

  server.on("/cook/0", [](){  
    start_cook = 0;
    String json="{\"Cook\":"+String((int)start_cook)+" }";
    pushMsg(json);
    
  });
  
  server.on("/temperature", [](){  
    gettemperatura(); // read sensor
    String json="{\"Temperature_Hot\":"+String((int)temp_hot)+"}, {\"Temperature_Cold\":"+String((int)temp_cold)+"}";
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


void pushMsg(String json){
    server.send(200, "text/plain", json);
}

void pushLCD(String msg){
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
        pushLCD("*** Open Gate ***");
        digitalWrite(13, val);    
        delay(tiempo_apertura);
        val = 1;
        digitalWrite(13, val);
        pushLCD("***Closed Gate***");
        delay(2000);
      }
  
}


void pushPWM() {
  
  if(start_cook == 1){
    
        if(temp_hot >= 60 )  
        {  
           valor_pwm = 0;
          
        }
        if(temp_hot >= 56 and temp_hot < 60 )  
         {  
            valor_pwm = valor_pwm + start_pwm;
         }
        analogWrite(salidaPWM,valor_pwm);
        delay(2000);
        lcdString =" Cooking in "+String((int)temp_hot)+"C ..";
        pushLCD(lcdString);
        delay(1000);
  }
  else{
        analogWrite(salidaPWM,0);
  }
  
}



