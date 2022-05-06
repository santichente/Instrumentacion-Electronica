/*
 * Instrumentación Electrónica _ 2022-01
 * Laboratorio N°1 
 * Profesor: Johann F. Osma
 * Asistente: Sanatiago Tovar Perilla
 * Coautora: Juliana Noguera Contreras
 */

 //Librerias
#include "HX711.h"

//Variables
#define DOUT  A1
#define CLK  A0

//Variables para calibracion
int Peso_conocido = 156;// en gramos | MODIFIQUE PREVIAMENTE CON EL VALOR DE PESO CONOCIDO!!!

HX711 balanza(DOUT, CLK);

void setup() {
  
  Calibracion();
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");  
  Serial.println(" ");
  Serial.println(" ");
}


void loop() {

  float peso = balanza.get_units(10); // Entrega el peso actualment medido en gramos
  if(peso<0) peso = peso*-1;
  
  Serial.print("Peso: ");
  Serial.print(peso, 1);
  Serial.println(" g");
  delay(100);
}

// FUNCIONES

//Función de Calibración: Permite calibrar la medida de la balanza según un peso de calibración conocido
void Calibracion(void)
{
  float Escala;
  float PromMedicion;
  
  Serial.begin(9600);
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  Serial.println(" ");
  delay(100);
  Serial.println("No ponga ningun objeto sobre la balanza");
  Serial.println(" ");
  balanza.set_scale(); //Ajusta la escala a su valor por defecto que es 1  
  balanza.tare(20);  //El peso actual es considerado "Tara".
  delay(50);
  Serial.print("...Destarando...");
  delay(250);
  

  
  delay(250);

    for(int i=3;i>=0;i--)
  {       
    Serial.print(" ... ");
    Serial.print(i);
    delay(1000);  
  }

  Escala = PromMedicion/Peso_conocido; // Relación entre el promedio de las mediciones analogas con el peso conocido en gramos 
  balanza.set_scale(-959.86); // Ajusta la escala correspondiente

  Serial.println(" ");
  Serial.println(" ");  
}
