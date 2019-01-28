#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

Serial.begin(115200);

//Use predefined PINS consts
pinMode(2, OUTPUT);
Wire.begin(D4, D3);

lcd.begin();

lcd.home();

lcd.print("Hello, NodeMCU");

}

void loop() {

    
    for (int PWM_duty = 0; PWM_duty < 1023; PWM_duty++) 
    {
      analogWrite(2, PWM_duty);
      delay(1);
    }
    delay(500);
    for (int PWM_duty = 1023; PWM_duty >= 0; PWM_duty--) 
    {
      analogWrite(2, PWM_duty);
      delay(1);
    }
    delay(500);

}
