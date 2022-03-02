
 
 //Librerias
#include "HX711.h"

//Variables
#define DOUT  A1
#define CLK  A0

HX711 balanza(DOUT, CLK);


void setup() {
  Serial.begin(9600);
  Serial.println("Balanza");
  balanza.tare(10);  //El peso actual es considerado "Tara", que idealmente será asignado como el peso en 0 gramos.
  delay(50);
  Serial.print("...Destarando...");  //Se indica para evidenciar el proceso de asignación de "Tara".
}

void loop() {
  float prom;
  prom = balanza.get_value(20); //La variable "prom" permite identificar la señal que envía la celda de carga al verse afectada por una deformación.
  balanza.set_scale(asigEscala(prom)); //Se asigna la escala para la masa actual llamando la función "asigEscala()" con el parámetro de ingreso "prom", para así devolver la escala calibrada para esta masa.
  
  float peso = balanza.get_units(5); // Entrega el peso actualmente medido en gramos.
  if(peso<0) peso = peso*-1; //Permite realizar la corrección si el montaje de la balanza no se encuentra en tensión en el lado correcto.
  
  Serial.println(" ");
  Serial.print("Peso: ");
  Serial.print(peso, 1);
  Serial.println(" g"); //Se muestra en el monitor serial la masa del peso medido actualmente.
  delay(100);

}

float asigEscala (float medicion) { //La función asigna la escala respectiva según el rango en el que se encuentre la señal que envía la celda de carga.

  if (medicion<=57046.25) return 1954;
  else if (medicion<=137246.8) return 1960.67;
  else if (medicion<=196300) return 1963;
  else if (medicion<=294627.3) return 1970.0;
  else if (medicion<=431644.8) return 1971;
  else if (medicion<=589018.8) return 1963.4;
  else if (medicion<=788300) return 1970.5;
  else if (medicion<=924806.5) return 1967.67;
  else if (medicion<=979000) return 1957.51;
  else if (medicion<=1480000) return 1969.9;
  else if (medicion<=1554288) return 1969;
  else if (medicion<=1870283) return 1962.5;
  else return 1963;
  
}
