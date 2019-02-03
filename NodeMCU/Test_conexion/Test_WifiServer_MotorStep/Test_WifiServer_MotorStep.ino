// Incluímos la librería para poder utilizarla
#include <ESP8266WiFi.h>
#include <Stepper.h>

// Esto es el número de pasos en un minuto
#define STEPS 4096 
// Número de pasos que queremos que de
#define NUMSTEPS 20

// Constructor, pasamos STEPS y los pines donde tengamos conectado el motor
Stepper stepper(STEPS, 2, 14, 12, 13);

void setup() {
  // Asignamos la velocidad en RPM (Revoluciones por Minuto)
  stepper.setSpeed(5);
}

void loop() {
  // Movemos el motor un número determinado de pasos
  stepper.step(NUMSTEPS);
  delay(200);
}
