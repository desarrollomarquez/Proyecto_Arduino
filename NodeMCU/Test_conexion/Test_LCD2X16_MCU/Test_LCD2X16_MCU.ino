#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

Serial.begin(115200);

//Use predefined PINS consts

Wire.begin(D4, D3);

lcd.begin();

lcd.home();

lcd.setCursor(0, 0);
lcd.print("Hello, NodeMCU");
lcd.setCursor(0, 1);
lcd.print("IP:...");
}


void loop()
{
	// Do nothing here...

}
