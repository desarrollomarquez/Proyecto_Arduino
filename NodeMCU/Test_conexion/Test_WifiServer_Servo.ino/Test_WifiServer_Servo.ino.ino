#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo; // Define my servo

  int button= 13 ; // define the button pin
  int val=0 ; // set button state

void setup()
{
  myservo.attach(12); // servo digital pin 9
  pinMode(button, INPUT); // define pin mode
}

void loop()
{
  val = digitalRead(button);
if (val==HIGH)
  {
  myservo.write(90);   // Turn Servo Left to 90 degrees
  }
else
  {
  myservo.write(0);   // Turn Servo Left to 0 degrees 
  }
}

