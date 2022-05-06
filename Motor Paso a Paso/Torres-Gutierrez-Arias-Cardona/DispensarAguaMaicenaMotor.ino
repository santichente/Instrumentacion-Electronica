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

//----------- Variables del motor paso a paso --------
#define StepPin 4
#define ms1 5
#define ms2 6
#define ms3 7
unsigned int t = 980; // Rango: 980 - 9800 que corresponde a 150 - 15 RPMs aprox.
#define Encoder_pin 2      // The pin the encoder is connected 
#define PulsosRotacion 20  // Value of pluses per rotation
int Encoder = 0;           // Value of the Output of the encoder 
int Estado = 1;            // Current state of the encoder to recognize the pulse changes
int Pulsos = 0;            // Counter of pulses
unsigned long InitialTime = 0;
unsigned long FinalTime; 
float RPMs;
float rpmIngresado = 0;

//----- Escalas de la calibracion de la balanza ------
float Escala0_100 = -827.421;
float Escala100_200 = -827.339;
float Escala200_300 = -825.173;
float Escala300_400 = -826.410;
float Escala400_500 = -826.931;
float Escala500_600 = -828.671;
float Escala600_700 = -830.720;

//-------------- Variables de los pesos --------------
float pesoRecipiente = 0; // se almacena el peso del recipiente medido
float pesoMaicena = 0; // se calcula el peso de la maicena depositado
float pesoIdealMaicena = 0; // es el peso que se espera depositar
float pesoRecipienteMaicena = 0; // peso del recipiente mas el peso de la maicena
float pesoTotal = 0; // peso total del sistema

HX711 balanza(DOUT, CLK);

//--------------- Variables Caudal -------------------
int motorPin = 9;
int min_pwm = 175;

//---------- Variables Mini bomba de agua ------------
int Volumen_deseado = 0;// en mL valor ingresado por el usuario
float caudal_Max = 10.682;
float caudal_Min = 5.116;

boolean fin = false; // indica si ya se ha depositado el agua requerida
boolean actStepMotor = false; // indica si se activa o no el motor paso a paso

void setup() {
  Serial.begin(9600);
  //========== set up motor paso a paso =========
  pinMode( ms1 , OUTPUT);
  pinMode( ms2 , OUTPUT);
  pinMode( ms3 , OUTPUT);
  pinMode( StepPin , OUTPUT);
  pinMode( Encoder_pin , INPUT);

  //=============== set up balanza ==============
  Calibracion(); // Se realiza la calibracion de la balanza
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");  
  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Coloque el recipiente sobre la balanza y Presione ENTER");
  while (Serial.available() == 0)   {}  
  confirmar = Serial.read(); // Una vez se ha calibrado se inicia a pesar

  while(pesoRecipiente == 0){  // Se busca conocer el peso del reactor
    pesar();
  }
  Serial.println(" ");
  Serial.println("Peso Recipiente = " + String(pesoRecipiente) + "g");

  Serial.println(" ");
  Serial.println("Ingrese la cantidad de maicena en gramos e inicie a depositar la maicena");
  while (Serial.available() == 0)   {}  // Se ingresa el valor en gramos de la maicena
  String input =  Serial.readString();
  pesoIdealMaicena = input.toFloat();

  //============= Añadir Maicena ===================
  pesoMaicena = pesar() - pesoRecipiente;
  Serial.println("Peso Maicena" + String(pesoMaicena));

  // se calcula el peso actual de la maicena depositada y se espera hasta que se haya depositado el peso deseado o mas para continuar
  while(pesoMaicena <= pesoIdealMaicena){ 
    pesoMaicena = pesar() - pesoRecipiente;
    pesoRecipienteMaicena = pesar();
    Serial.println("Peso Maicena" + String(pesoMaicena));
  }

  //============ Minibomba y dosificacion ===========
  Serial.println(" ");  
  pinMode(motorPin, OUTPUT);  // Inicializacion de la minibomba de agua

  Serial.println("Ingrese Volumen en ml");
  while (Serial.available() == 0)   {}  // Se ingresa el valor en ml de la cantidad de agua que se debe dosificar
  input =  Serial.readString();
  Volumen_deseado = input.toFloat();
  Volumen_deseado = Volumen_deseado + 38;  // En las pruebas se halla que siempre se tiene un error lineal de 38ml
                                           // por lo cual se indica que se debe depositar 38ml menos de lo solicitado
  Serial.println(" ");
  Serial.println("Iniciando Dosificación...");

  Serial.print("recipiente " + String(pesoRecipiente) + "\t maicena " + String(pesoMaicena));
}

void loop() {
  float pesoAgua = pesar() - pesoRecipienteMaicena;
  Serial.println("peso agua: " + String(pesoAgua) + "g");
  if(!fin){ // se dosifica hasta que se haya completado el volumen requerido
    
    DosificarB(); // Se dosifica la cantidad de agua especificada
    
  }else{  // una vez se ha dosificado el agua, se dan los valores de los pesos obtenidos
    
    pesoTotal = pesar();
    Serial.println("recipiente " + String(pesoRecipiente) + "\t maicena " + String(pesoMaicena)  + "\t agua " + String(pesoAgua) + "\t total " + String(pesoTotal));
    Serial.println("Agua y maicena " + String(pesoAgua+pesoMaicena)); 

    //===================== Se pregunta el numero de RPMs a los que se quiere revolver ======================
    Serial.println("Ingrese valor de RPMs");
    while (Serial.available() == 0)   {}  // Se ingresa el valor de RPMs
    input =  Serial.readString();
    rpmIngresado = input.toDouble();
    float calc_tiempo = float(149094) * pow(rpmIngresado, -1.002); // funcion obtenida de la caracterizacion del motor
    t = int(calc_tiempo);
    Serial.println('Valor de t: ' + String(t));
    ControlMotorPasoaPaso();
    actStepMotor = true;
    
  } if(actStepMotor){
      while(true){
          activarMotorPasoPaso();
      }
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
    
    if (pesoRecipiente == 0 && peso > 80){ // Se calcula el peso del reactor. Solo si este no se ha calculado antes
      pesoRecipiente = peso;
    }
    Serial.print("Pesar: ");
    Serial.print(peso, 1);
    Serial.println(" g");
    delay(200);
  }
  return peso;
}

//Función de Calibración: Permite calibrar la balanza con los valores de escala especificados
void Calibracion(void){
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  Serial.println(" ");
  delay(100);
  Serial.println("Procesando escalas");
  Serial.println(" ");
  balanza.set_scale(Escala0_100);  // Inicialmente se asigna la menor escala y posteriormente se ajusta para mejorar la precision
  balanza.tare(50);  //El peso actual es considerado "Tara". Se toman 50 mediciones del peso actual
  delay(200);
  Serial.print("...Destarando...");  
}

// Se indica a la bomba que debe entregar X cantidad de agua
void DosificarB(void){
  float pesoLiquido = pesar() - pesoRecipienteMaicena;// Peso del liquido actualmente medido en gramos
  
  while ((Volumen_deseado) + 0.5 >= pesoLiquido){ // Se entrega agua hasta que falten 0.5 gramos
    if( pesoLiquido >=  Volumen_deseado*0.8){ // Se entrega agua a la minima velocidad si ya se ha depositado mas del 80% del valor solicitado
      analogWrite(motorPin, min_pwm);
    }else{
      analogWrite(motorPin, 254); // Se entrega agua a la maxima velocidad si se ha depositado menos del 80% del valor solicitado
    }
    pesoLiquido = pesar() - pesoRecipienteMaicena;  
    Serial.print("--- Cantidad: " + String(pesoLiquido)+ " ml");  // Se indica la cantidad de agua depositada
  }

  if ((Volumen_deseado) + 0.1 <= pesoLiquido){ // Si se ha entregado la cantidad de agua solicitada menos 0.1ml se detiene la bomba
    analogWrite(motorPin, 0);
    fin = true;
    pesoLiquido = pesar() - pesoRecipienteMaicena;  
    Serial.print("------ Cantidad: " + String(pesoLiquido)+ " ml"); // Se indica la cantidad de agua depositada
  }
}

void activarMotorPasoPaso(void){
  digitalWrite(StepPin, HIGH); 
  delayMicroseconds(t);           
  digitalWrite(StepPin, LOW);  
  delayMicroseconds(t);
  
  Encoder = digitalRead(Encoder_pin);  
  if(Encoder == LOW && Estado == 1)  {
     Estado = 0;             
  }
  if(Encoder == HIGH && Estado == 0)  {
     Pulsos++; 
     Estado = 1;     
  }
  if(Pulsos == PulsosRotacion)  {
    FinalTime = millis();
    RPMs = 60000/(FinalTime - InitialTime);
    Pulsos = 0; 
    InitialTime = FinalTime;
    Serial.print("RPM = "); // Se muestra en el puerto serial el valor medido de RPMs
    Serial.println(RPMs);
  }
}

//Función para Controlar el Motor Paso a Paso: Permite controlar el motor y los pasos de este
void ControlMotorPasoaPaso(void) {
  Serial.println("Motor Control");
  digitalWrite(ms1, LOW);
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
}
