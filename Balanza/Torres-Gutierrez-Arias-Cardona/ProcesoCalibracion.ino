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
  float peso = balanza.get_units(10); // Entrega el peso actualment medido en gramos
  if(peso<0) peso = peso*-1;
  if(peso<1000){
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
  float Escala = 0;
  float PromMedicion;
  
  Serial.begin(9600);
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  Serial.println(" ");
  delay(200);
  Serial.println("No ponga ningun objeto sobre la balanza");
  Serial.println(" ");
  balanza.set_scale(); //Ajusta la escala a su valor por defecto que es 1  
  balanza.tare(30);  //El peso actual es considerado "Tara". Se toman 20 mediciones del peso actual
  delay(500);
  Serial.print("...Destarando...");
  delay(500);
  
  Serial.println(" ");
  Serial.println(" ");
  
  Serial.println("Digite la cantidad de pesos que se usarán para la calibración");
  while (Serial.available() == 0)   {} 
  input =  Serial.readString();
  cantidad_pesos_calibracion = input.toFloat(); //Obtener confirmacion.  
  Serial.println("SE USARÁN " + String(cantidad_pesos_calibracion) + " PESOS PARA CALIBRAR");

  float pesos_conocidos[cantidad_pesos_calibracion-1];
  float escalas[cantidad_pesos_calibracion-1];


  for (int i = 0; i < cantidad_pesos_calibracion; i++) {
    Serial.println(" ");
    Serial.println("Digite el valor del peso " + String(i+1) + " y coloque el peso en la balanza");
    while (Serial.available() == 0)   {}  
    input = Serial.readString();
    pesos_conocidos[i] = input.toFloat(); //Obtener confirmacion.
    

    Serial.println("Peso "+ String(i+1) + ": " + String(pesos_conocidos[i]) + "g");

    PromMedicion = 0;
    
    for(int i=3;i>=0;i--) {       
      Serial.print(" ... ");
      Serial.print(i);
      PromMedicion += balanza.get_value(30);//20 mediciones  //Obtiene el promedio de las mediciones analogas según valor ingresado
    }
    PromMedicion = PromMedicion / 4;
    
    Serial.println(" ");
    Serial.println("Retire el peso y presione ENTER para continuar");
    while (Serial.available() == 0)   {}  
    confirmar = Serial.read(); //Obtener confirmacion.  

    delay(200);

    for(int i=3;i>=0;i--) {       
      Serial.print(" ... ");
      Serial.print(i);
      delay(100);  
    }

    escalas[i] = PromMedicion/pesos_conocidos[i]; // Relación entre el promedio de las mediciones analogas con el peso conocido en gramos 
    Serial.println("");
    delay(200);
  }

  Serial.println(" ");
  Serial.println("Escalas:");
  for (int i = 0; i < cantidad_pesos_calibracion; i++) {
    Serial.println(String(i+1) + ": " + String(escalas[i]));
    Escala += escalas[i];
  }
  Serial.println(balanza.read());
  Serial.println(" ");
  Escala = Escala/cantidad_pesos_calibracion;
  Serial.println("Promedio escalas: " + String(Escala));
  balanza.set_scale(Escala); // Ajusta la escala correspondiente
  balanza.tare(50);
  Serial.println("---------Calibración finalizada---------");
  Serial.println(" ");
  Serial.println(" ");  
}
