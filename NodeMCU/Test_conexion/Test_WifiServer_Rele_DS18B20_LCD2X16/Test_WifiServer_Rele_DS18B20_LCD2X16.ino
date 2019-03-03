
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
#include <PID_v1.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* host = "icaro";
const char* ssid     = "Marquez Correa";
const char* password = "Marquez8355196";

//const char* ssid     = "pegajoso";
//const char* password = "pegajososiempre";

ESP8266WebServer server(80);


float temp_hot, temp_cold;  // Values read from sensor
int val = 1;
int tiempo_apertura = 3000;
int start_pwm;
int valor_pwm;
int salidaPWM = 15;  // salida de señal PWM

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters PID
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

//Mensajes del API y LCD
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
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

void handle_root() {
  webString="Bienvenido API REST CSV  server: Metodos  /open -> Abrir Compuerta; /temperaturas -> Mostrar Temperaturas Hot y Cold; /cook/1 -> Cocinar - /cook/0 -> No cocinar";
  pushMsg(webString);
  delay(100);
}
 
void setup(void)
{
  lcd.begin();   // initializing the LCD
  lcd.backlight();
    
  // prepare GPIO16 - PWM
  pinMode(16, OUTPUT);
  pinMode(salidaPWM, OUTPUT);
  pinMode(13, OUTPUT);

  Setpoint = 100; // velocidad referencia
  
  sensorDS18B20Hot.begin(); // initialize temperature sensor
  sensorDS18B20Cold.begin(); 

   //Activar el PID
  myPID.SetSampleTime(20);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0,1024);
  
  Serial.begin(115200);
  
  // Connect to WiFi networkRegístrese hoy mismo 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  IPAddress local_ip(192, 168, 1, 1); //Asigna la dirección IP 
  IPAddress gateway(192, 168, 1, 1); 
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println(ssid);
  Serial.print("Access Point - Nueva direccion IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  //Serial.println("");
  //Serial.println("Reading Server");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
  
  
  
  //Rest API
  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);
  Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  server.on("/", handle_root);
  
  server.on("/temperaturas", [](){  
    gettemperatura();       // read sensor
    String json="Temperatura_Hot: "+String((int)temp_hot)+" Grados"+"Temperatura_Cold: "+String((int)temp_cold)+" Grados";
    pushMsg(json);
  });

 
  server.on("/open", [](){  
    val = 0;
    String json="Compuerta Open: "+String((int)val);
    pushMsg(json);
  });

  server.on("/cook/1", [](){  
    start_pwm = 1;
    String json="Cocinar: "+String((int)start_pwm);
    pushMsg(json);
  });

  
  server.on("/cook/0", [](){  
    start_pwm = 0;
    String json="No Cocinar: "+String((int)start_pwm);
    pushMsg(json);
    
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

  Setpoint = 100; // velocidad referencia

  double gap = abs(Setpoint-temp_hot); //distance away from setpoint
  
  if(gap<60)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.SetTunings(aggKp, aggKi, aggKd);
  }
  
  myPID.Compute();
  analogWrite(salidaPWM,Output);
  //pushLCD(String((int)Output));
  delay(1000);

      
}



