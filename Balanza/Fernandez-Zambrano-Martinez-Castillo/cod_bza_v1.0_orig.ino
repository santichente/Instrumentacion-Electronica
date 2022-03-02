//Univerisdad de los Andes
//Instrumentación electrónica - 202210
//Dylan Zambrano
//Daniel Martinez
//Danilo Fernández
//Juan Pablo Castillo

//Balanza de instrumentación - MAIN

#include "HX711.h"

HX711 scale(A1, A0);

float calibration_factor = -104.16; //Factor de calibración característico para la galaga
float units;


void setup() {
  Serial.begin(9600);
  scale.set_scale();
  scale.tare(20);  //Tara de reseteo en punto 0
  //Mensaje de Bienvenida
  Serial.println("::: Universidad de los Andes :::");
  Serial.println("::: Instrumentación Electrónica - 2210:::");
  Serial.println("-: Balanza de instrumentación :-");

  //Cálculo de Factor cero
  long zero_factor = scale.read_average(); //referencia de la balanza en vacío (cero)
  Serial.print("Factor Cero: "); //Imprime el valor obtenido, elimina la necesidad de tarar cada vez que se cambie de objeto.
  Serial.println(zero_factor);

  //Mensaje de preparación
  Serial.println("espere...");
  Serial.println("::: ¡BALANZA VACÍA! :::");
  Serial.println("espere...");
  for(int i=3;i>=0;i--)
  {       
    Serial.print(" ... ");
    Serial.print(i);
    delay(1000);
  }
Serial.println(" ");
  Serial.println("::: ¡Coloque un objeto! :::");
  Serial.println(" ");
}

void loop() {

  scale.set_scale(calibration_factor); //Ajuste de la escala según el factor de calibración calculado (linealización de la curva característica.)
  Serial.print("Midiendo: ");
  units = scale.get_units(10)/10;
  //Eliminación de datos de ruido
  if (units < 0.5)
  {
    units = 0.00;
  }
 
//Impresión de resultado medido

  Serial.print(units,1);
  Serial.print(" g"); 
  Serial.println();

}
