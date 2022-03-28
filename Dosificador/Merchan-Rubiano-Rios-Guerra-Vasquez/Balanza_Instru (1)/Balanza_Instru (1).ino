// Tutorial Trasmisor de Celda de Carga HX711, 
// Balanza Digital. Naylamp Mechatronics - Perú. (n.d.). 
// Retrieved February 24, 2022, from 
// https://naylampmechatronics.com/blog/25_tutorial-trasmisor-de-celda-de-carga-hx711-balanza-digital.html 

// Integrantes: Felipe Rubiano, Manuel Merchan ,Jason Vazquez , David Rios, Sergio Guerra

#include "HX711.h"

const int DOUT=A1;
const int CLK=A0;

HX711 balanza;

void setup() {
  Serial.begin(9600);
  balanza.begin(DOUT, CLK);
  Serial.print("Iniciando Balanza, lectura ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(860.87); // Establecemos la escala para el funcionamiento
  balanza.tare(30);  // Tarando Balanza
  
  Serial.println("Listo para pesar");  
}

void loop() {
  Serial.print("Peso: ");
  Serial.print(balanza.get_units(20),2);
  Serial.println(" g");
  delay(500);
}
