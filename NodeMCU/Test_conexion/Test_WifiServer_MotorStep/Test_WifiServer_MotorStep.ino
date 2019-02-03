// Incluímos la librería para poder utilizarla
#include <ESP8266WiFi.h>
#include <Stepper.h>

// Esto es el número de pasos en un minuto
#define STEPS 200 
// Número de pasos que queremos que de
#define NUMSTEPS 20

// Constructor, pasamos STEPS y los pines donde tengamos conectado el motor
Stepper stepper(STEPS, 2, 14, 12, 13);

void setup()
{
  // Velocidad del motor en RPM
  stepper.setSpeed(100);
}
 
void loop()
{
  //Girar una vuelta entera en un sentido
  //stepper.step(200);
  //delay(500); //Pequeña pausa
  for (int x=0; x< 8; x++) {
    stepper.step(STEPS);
    delay(500);
  }
  
 
}
