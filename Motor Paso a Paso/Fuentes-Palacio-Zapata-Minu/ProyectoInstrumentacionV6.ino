//Librerias
#include "HX711.h"

//Conexiones del HX711
#define DOUT  A1
#define CLK  A0
HX711 balanza(DOUT, CLK);

//Conexiones del L293D
#define DCmotorON A5
int enA = 3;
int in1 = 0;
int in2 = 1;

//Variables Calibracion Motor
int tiempoMotorActivo;

//Varibles para medición
// Escala incluyendo valores menores a 20
float EscalaSeleccionada = -341;
float pesoMedido;

//Variables para calibracion
int Peso_conocido = 20; // en gramos | MODIFIQUE PREVIAMENTE CON EL VALOR DE PESO CONOCIDO!!!

// Puertos y Variables para Motor paso a paso
#define StepON A3
#define StepPin 4
#define DirPin 5
unsigned int t = 980; // Rango: 980 - 9800 que corresponde a 150 - 15 RPMs aprox.


// Puertos y Variables para Encoder
#define Encoder_pin 2      // The pin the encoder is connected 
#define PulsosRotacion 32  // Value of pluses per rotation

int Encoder = 0;           // Value of the Output of the encoder
int Estado = 1;            // Current state of the encoder to recognize the pulse changes
int Pulsos = 0;            // Counter of pulses
unsigned long InitialTime = 0;
unsigned long FinalTime;
float RPMs;


void setup() {
  Serial.begin(9600);               // Inicialización de la comunicación serial de 9600bps
  inicializarSistema();
  
  int pesoliquido = 45;             // Peso del líquido que se desea dosificar
  dosificadorBalanza(pesoliquido);
  
  digitalWrite(StepON, HIGH);       // Activación del motor de paso
  float revpm = 280;                // Velocidad de rotación deseada en revoluciones por minuto (RPM)        
  vueltaMotor(revpm, 50, 1);        // Se enciende el motor por cierto número de vueltas, a cierta dirección y a cierta velocidad
  delay(1000);

//vueltaMotor(revpm, 50, 0);
}
void loop()
{
    MedirBalanza();   //Medición continua de la celda de carga            
}

void inicializarSistema(void)
{
  // Inicializacion de puertos de conexion
  definicionPuertosMotorRiego();

  // Realizacion de la Tara inicial de la balanza
  inicializarBalanza();

  //Incialización de las conexiones con el motor step
  definicionPuertosMotorStep();
}

void definicionPuertosMotorRiego(void)
{
  //   inicializacion de pines del motor de riego
  pinMode(DCmotorON, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void definicionPuertosMotorStep(void)
{
  // inicializacion de pines del motor de paso y encoder
  pinMode( DirPin , OUTPUT);
  pinMode( StepPin , OUTPUT);
  pinMode( StepON , OUTPUT);
  pinMode( Encoder_pin , INPUT);
  digitalWrite(StepON, LOW);
}

// Esta función se encarga del control de la rotación del motor de paso
// Recibe como parámetro la velocidad y la dirección de giro deseadas
void girarMotor(float rpm, int direccion)
{
  if (direccion == 1)
  {
    digitalWrite(DirPin, HIGH);
  }
  else
  {
    digitalWrite(DirPin, LOW);
  }
  t = 144332 * pow(rpm, -0.996);    //Retardo entre el estado HIGH y LOW en microsegundos para la velociadad deseada (curva de calibración) 
  digitalWrite(StepPin, HIGH);
  delayMicroseconds(t);
  digitalWrite(StepPin, LOW);
  delayMicroseconds(t);
}

//Esta función se encarga de detener la rotación del motor de paso
void detenerMotorStep()
{
  digitalWrite(StepPin, LOW);  
}

//Esta función es la encargada de la rotación del motor de paso
//Recibe como parámetro la velociadad de rotación, el número de vueltas y la dirección de rotación
void vueltaMotor(float rpm, int vueltas, int direccion)
{
  
  for (int j = 0; j < vueltas; j++)
  {
    for (int i = 0; i < 200; i++)
    {
      girarMotor(rpm, direccion);
      medirEncoder();
    }
  }
}

//Esta función es la encargada de la interpretación de los pulsos de salida del encoder.
//Esto para obtener la velocidad de rotación del eje del motor de paso.
void medirEncoder(void)
{
  Encoder = digitalRead(Encoder_pin);         //Lectura de la salida del encoder

  if (Encoder == LOW && Estado == 1)
  {
    Estado = 0;
  }

  if (Encoder == HIGH && Estado == 0)
  {
    Pulsos++;
    Estado = 1;
  }
  if (Pulsos == 32)                          //32 pulsos representan una vuelta       
  {
    FinalTime = millis();
    RPMs = 60000 / (FinalTime - InitialTime); //Cálculo de la velocidad de rotación en RPM, por medio del tiempo que toma una rotación.
    Pulsos = 0;
    InitialTime = FinalTime;
    Serial.print("RPM = ");                 
    Serial.println(RPMs);                   //Muestra el dato de RPM en el monitor serial
  }
}

void inicializarBalanza(void)
{
  Serial.println("~ Inizializando Balanza ~");
  Serial.println(" ");
  Serial.println("Quitar los objetos que se encuentren encima de la balanza");
  Serial.println(" ");
  balanza.set_scale(); //Ajusta la escala a su valor por defecto que es 1
  balanza.tare(20);  //El peso actual es considerado "Tara".
  delay(500);
  Serial.println("Balanza lista, puede comenzar a pesar");
  Serial.println(" ");
  delay(200);
}

//Esta función se encarga de la medición del peso sobre la balanza, con la escala seleccionada en calibración
void MedirBalanza(void)
{
  balanza.set_scale(EscalaSeleccionada);
  pesoMedido = balanza.get_units(10); // Entrega el peso actualment medido en gramos
  if (pesoMedido < 0) pesoMedido = pesoMedido * -1;
  Serial.print("Peso: ");
  Serial.print(pesoMedido, 1);
  Serial.println(" g");
  delay(100);
}

//Esta función es la encargada de la dosificación del líquido de acuerdo al peso deseado
void dosificadorBalanza(int pesoDeseado)
{
  MedirBalanza();                           //Medición de la balanza
  encenderMotorDosificador(255);            //Encendido de la bomba de agua con un PWM de 255
  while (pesoMedido < pesoDeseado - 15)     //Mientras se esté por debajo 15 gramos del peso deseado la bomba permanece encendida
  {
    MedirBalanza();
  }
  apagarMotorDosificador();
  delay(3000);
  // En este punto empieza la dosificación
  while (pesoMedido < pesoDeseado)          //Mientras estemos debajo del peso deseado la bomba se enciende por 250ms, y se apaga. Esto se repito hasta que se alcanza el peso deseado
  {
    encenderMotorDosificador(255);
    delay(250);
    apagarMotorDosificador();
    delay(1000);
    MedirBalanza();
  }
}
//Esta función es la encargada de encender el motor DC de la bomba de agua con un PWM deseado.
void encenderMotorDosificador(int velocidad)
{
  analogWrite(DCmotorON, 255);
  analogWrite(enA, velocidad);
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
}

//Esta función se encarga de apagar el motor DC de la bomba de agua.
void apagarMotorDosificador()
{
  analogWrite(DCmotorON, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
}
