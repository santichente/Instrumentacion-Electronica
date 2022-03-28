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

//-------------- Variable de confirmacion ------------
String confirmar = "";
String input = "";

float Escala0_100 = -827.421;
float Escala100_200 = -827.339;
float Escala200_300 = -825.173;
float Escala300_400 = -826.410;
float Escala400_500 = -826.931;
float Escala500_600 = -828.671;
float Escala600_700 = -830.720;

float pesoInicial = 0;

HX711 balanza(DOUT, CLK);

//--------------- Variables Caudal -------------------
int motorPin = 9;
int min_pwm = 175;

//Variables para mini bomba de agua
int Volumen_deseado = 0;// en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!
float caudal_Max = 10.682;
float caudal_Min = 5.116;

boolean fin = false;

void setup() {
  Calibracion(); // Se realiza la calibracion de la balanza
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");  
  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Presione ENTER para conocer el peso inicial");
  while (Serial.available() == 0)   {}  
  confirmar = Serial.read(); // Una vez se ha calibrado se inicia a pesar

  
  while(pesoInicial == 0){  // Se busca conocer el peso del reactor
    pesar();
  }
  Serial.println(" ");
  Serial.println("Peso Inicial = " + String(pesoInicial) + "g");
  
  Serial.println(" ");  
  pinMode(motorPin, OUTPUT);  // Inicializacion de la minibomba de agua

  Serial.println("Ingrese Volumen en ml");
  while (Serial.available() == 0)   {}  // Se ingresa el valor en ml de la cantidad de agua que se debe dosificar
  String input =  Serial.readString();
  Volumen_deseado = input.toFloat();
  Volumen_deseado = Volumen_deseado - 7;  // En las pruebas se halla que siempre se tiene un error de 7ml por lo cual se indica que se debe depositar 7ml menos de lo solicitado
  
  Serial.println(" ");
  Serial.println("Iniciando Dosificación...");
}

void loop() {
  float peso = pesar() - pesoInicial;
  Serial.println("peso: " + String(peso) + "g");
  if(!fin){
    DosificarB(); // Se dosifica la cantidad de agua especificada
  }else{  
    Serial.print("------ Cantidad: " + String(pesar() - pesoInicial)+ " ml"); // Se indica la cantidad de agua actual
  }
}

// Calcula el peso actual y lo calcula de acuerdo a la escala
float pesar(){
  float peso = balanza.get_units(10); // Entrega el peso actualmente medido en gramos
  if(peso<0) peso = peso*-1;
  if(peso<1000){  
    if(peso < 98){ // Se cambia la escala a menor a 100g
      balanza.set_scale(Escala0_100);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }else if (peso < 198){ // Se cambia la escala a 100 - 200
      balanza.set_scale(Escala100_200);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }else if (peso < 298){ // Se cambia la escala a 200 - 300
      balanza.set_scale(Escala200_300);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }else if (peso < 398){ // Se cambia la escala a 300 - 400
      balanza.set_scale(Escala300_400);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }else if (peso < 498){ // Se cambia la escala a 400 - 500
      balanza.set_scale(Escala400_500);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }else if (peso < 598){ // Se cambia la escala a 500 - 600
      balanza.set_scale(Escala500_600);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }else { // Se cambia la escala a mayor de 600
      balanza.set_scale(Escala600_700);
      delay(20);
      peso = balanza.get_units(10);
      if(peso<0) peso = peso*-1;
    }
    
    if (pesoInicial == 0 && peso > 80){ // Se calcula el peso del reactor
      pesoInicial = peso;
    }
    Serial.print("Peso: ");
    Serial.print(peso, 1);
    Serial.println(" g");
    delay(200);
  }
  return peso;
}

//Función de Calibración: Permite calibrar la balanza con los valores de escala especificados
void Calibracion(void){
  Serial.begin(9600);
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  Serial.println(" ");
  delay(100);
  Serial.println("Procesando escalas");
  Serial.println(" ");
  balanza.set_scale(Escala0_100);  
  balanza.tare(50);  //El peso actual es considerado "Tara". Se toman 50 mediciones del peso actual
  delay(200);
  Serial.print("...Destarando...");  
}

// Se indica a la bomba que debe entregar X cantidad de agua
void DosificarB(void)
{
  float pesoLiquido = pesar() - pesoInicial;// Peso del liquido actualmente medido en gramos
  
  while ((Volumen_deseado) + 0.5 >= pesoLiquido){ // Se entrega agua hasta que falten 0.5 gramos
    if( pesoLiquido >=  Volumen_deseado*0.8){ // Se entrega agua a la minima velocidad si ya se ha depositado mas del 80% del valor solicitado
      analogWrite(motorPin, min_pwm);
    }else{
      analogWrite(motorPin, 254); // Se entrega agua a la maxima velocidad si se ha depositado menos del 80% del valor solicitado
    }
    pesoLiquido = pesar() - pesoInicial;  
    Serial.print("------ Cantidad: " + String(pesoLiquido)+ " ml");  // Se indica la cantidad de agua depositada
  }

  if ((Volumen_deseado) + 0.1 <= pesoLiquido){ // Si se ha entregado la cantidad de agua solicitada menos 0.1ml se detiene la bomba
    analogWrite(motorPin, 0);
    fin = true;
    pesoLiquido = pesar() - pesoInicial;  
    Serial.print("------ Cantidad: " + String(pesoLiquido)+ " ml"); // Se indica la cantidad de agua depositada
  }
}
