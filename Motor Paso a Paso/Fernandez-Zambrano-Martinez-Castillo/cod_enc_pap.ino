//Univerisdad de los Andes
//Instrumentación electrónica - 202210
//Dylan Zambrano
//Daniel Martinez
//Danilo Fernández
//Juan Pablo Castillo

//Subsistema Tacómetro-Paso_a_Paso

//Definición de pines
#define StepPin 12
#define ms1 2
#define ms2 3
#define ms3 4


//#define DirPin 11 // Pin de dirección deshabilitado.


// Puertos y Variables para Encoder
#define Encoder_pin 7      // Pin del tacómetro (encoder)
#define PulsosRotacion 20  // Definición de ticks en la rueda de medición.

int Encoder = 0;           // Valor de salida del encoder
int Estado = 1;            // Estado del encoder (on/0ff)
int Pulsos = 0;            // Contador de pulsos
unsigned long InitialTime = 0; //Marca de tiempo inicial
unsigned long FinalTime; //marca de tiempo final
float RPMs; //valor que almacena las frecuencia de rotación

void setup() {

  Serial.begin(9600);

//inicialización de pines de microstepping en nivel bajo (full-step)
  pinMode( ms1 , OUTPUT);
  pinMode( ms2 , OUTPUT);
  pinMode( ms3 , OUTPUT);
//inicialización de pines de salida
  pinMode( StepPin , OUTPUT);
  pinMode( Encoder_pin , INPUT);
}

void loop() {

//Lectuta de datos digitales desde el módulo encoder HC020K.
Encoder = digitalRead(Encoder_pin);  
//definición de los estados para el conteo de pulsos (ticks)
  if(Encoder == LOW && Estado == 1)
  {
     Estado = 0;             
  }
//si la salida es alta, se suma +1 al contador
  if(Encoder == HIGH && Estado == 0)
  {
     Pulsos++; 
     Estado = 1;     
  }

//si el contador de pulsos llega al valor de ticks en la rueda, se calcula el tiempo que tomó una revolución.
  if(Pulsos == PulsosRotacion)
  {
    //cómputo de la frecuencia de rotación
    FinalTime = millis();
    RPMs = 60000/(FinalTime - InitialTime);
    Pulsos = 0; 
    InitialTime = FinalTime;
    
    //impresión del cómputo en la consola.
    Serial.print("RPM = ");
    Serial.println(RPMs);  
}

//Actuación del motor paso a paso.

  //inicio del pulso de señalización
  digitalWrite(StepPin, HIGH);
  //duración del escalón en ms.
  delayMicroseconds(1950);
  //fin del escalón.
  digitalWrite(StepPin, LOW);



}
