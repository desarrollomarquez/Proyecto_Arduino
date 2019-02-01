#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo; // Define my servo

 void setup() {
  myservo.attach(12);  // servo en el pin 2
}

void loop() {  
  for (int angulo = -180; angulo <= 0; angulo += 1) 
  { 
    myservo.write(angulo);  
    delay(10);  
  }
}

