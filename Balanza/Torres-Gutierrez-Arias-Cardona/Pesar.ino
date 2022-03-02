/*
 * Instrumentación Electrónica _ 2022-01
 * Laboratorio N°1 
 * Profesor: Johann F. Osma
 * Asistente: Sanatiago Tovar Perilla
 * Coautora: Juliana Noguera Contreras
 */

//--------------------- LIBRERIAS -------------------- 
#include "HX711.h"

//--------------------  VARIABLES -------------------- 
#define DOUT  A1
#define CLK  A0

//-------------- Variables para calibracion ---------- 
int Peso_conocido = 50;// en gramos | MODIFIQUE PREVIAMENTE CON EL VALOR DE PESO CONOCIDO!!!
int cantidad_pesos_calibracion = 0; 

//-------------- Variable de confirmacion ------------
String confirmar = "";
String input = "";

float Escala0_300 = -817.42;
float Escala300_700 = -804.13;
float Escala700_1000 = -806.86;

HX711 balanza(DOUT, CLK);

void setup() {
  Calibracion();
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");  
  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Presione ENTER para iniciar a pesar");
  while (Serial.available() == 0)   {}  
  confirmar = Serial.read(); //Obtener confirmacion.  
  Serial.println(" ");  
}

void loop() {
  float peso = balanza.get_units(10); // Entrega el peso actualmente medido en gramos
  if(peso<0) peso = peso*-1;
  if(peso<1000){
    if(peso<298){
      balanza.set_scale(Escala0_300);
    }else if(peso<698){
      balanza.set_scale(Escala300_700);
    }else{
      balanza.set_scale(Escala700_1000);
    }
    Serial.print("Peso: ");
    Serial.print(peso, 1);
    Serial.println(" g");
    delay(200);
  }
}

// FUNCIONES

//Función de Calibración: Permite calibrar la medida de la balanza según un peso de calibración conocido
void Calibracion(void)
{
  Serial.begin(9600);
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  Serial.println(" ");
  delay(200);
  Serial.println("Procesando escalas");
  Serial.println(" ");
  balanza.set_scale(Escala0_300);  
  balanza.tare(50);  //El peso actual es considerado "Tara". Se toman 20 mediciones del peso actual
  delay(500);
  Serial.print("...Destarando...");  
}
