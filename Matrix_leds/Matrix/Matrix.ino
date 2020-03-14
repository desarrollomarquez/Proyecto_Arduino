#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

 
//Vcc - Vcc
//Gnd - Gnd
//Din - Mosi (Pin 11)
//Cs  - SS (Pin 10)
//Clk - Sck (Pin 13)
 
const int pinCS = 10;
const int numberOfHorizontalDisplays = 8;
const int numberOfVerticalDisplays = 1;
 
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);



int addr = 0;
String msgin = "";
boolean s = false;
boolean f = false;
boolean p = false;

SoftwareSerial BT1(4,2); // RX, TX


 
void setup() {
 
 
 Serial.begin(9600);
 BT1.begin(9600);
 
 matrix.setIntensity(4); // Ajustar el brillo entre 0 y 15
 // Ajustar segun las necesidades
 matrix.setPosition(0, 0, 0); // El primer display esta en <0, 0>
 matrix.setPosition(1, 1, 0); // El segundo display esta en <1, 0>
 matrix.setPosition(2, 2, 0); // El tercer display esta en <2, 0>
 matrix.setPosition(3, 3, 0); // El cuarto display esta en <3, 0>
 matrix.setPosition(4, 4, 0); // El quinto display esta en <4, 0>
 matrix.setPosition(5, 5, 0); // El sexto display esta en <5, 0>
 matrix.setPosition(6, 6, 0); // El séptimo display esta en <6, 0>
 matrix.setPosition(7, 7, 0); // El octavo display esta en <7, 0>
 matrix.setPosition(8, 8, 0); // El noveno display esta en <8, 0>
 
 matrix.setRotation(0, 1);    // Posición del display
 matrix.setRotation(1, 1);    // Posición del display
 matrix.setRotation(2, 1);    // Posición del display
 matrix.setRotation(3, 1);    // Posición del display
 matrix.setRotation(4, 1);    // Posición del display
 matrix.setRotation(5, 1);    // Posición del display
 matrix.setRotation(6, 1);    // Posición del display
 matrix.setRotation(7, 1);    // Posición del display
 matrix.setRotation(8, 1);    // Posición del display
 delete_message();
 
}

String save_message(){
        String msg = "";
        for (int i = 0; i < EEPROM.length(); i++) {
              if(EEPROM.read(i) != 35){
                msg += char(EEPROM.read(i));
              }
        }
        return msg;        
  }

void delete_message(){
        for (int i = 0; i <=  EEPROM.length(); i++) {
          EEPROM.write(i, 0);
        }
        Serial.println("Se ha borrado la eeprom");
}

String print_matrix(String cadena){  
          int wait = 70; // Velocidad a la que realiza el scroll 
          int spacer = 1;
          int width = 5 + spacer; // Ancho de la fuente a 5 pixeles
               for (int i = 0; i < 6 * cadena.length() + matrix.width() - 2; i++) {
                 
                 matrix.fillScreen(LOW);
                 int letter = i / 6;
                 int x = (matrix.width() - 1) - i % 6;
                 int y = (matrix.height() - 8) / 2; // Centrar el texto
    
                 while (x + 5 >= 0 && letter >= 0) {
                   matrix.drawChar(x, y, cadena[letter], HIGH, LOW, 1);
                   letter--;
                   x -= width;
                 }
                 matrix.write(); // Muestra loscaracteres
                 delay(wait);  
                 
               }
               return "pintando..";
                              
}


void loop() {
       while (BT1.available()){
        
             EEPROM.update(addr, BT1.read());
             if (EEPROM.read(addr) == 35){                
                Serial.println("Pintar...");
                Serial.println(msgin);
                Serial.print(print_matrix(msgin));
                delete_message();
                msgin = "";
              }
              else{
                msgin += char(EEPROM.read(addr));
                addr++;
              }
        
        }     
}


  

  
