/***********************************************************************************
   Instrumentación Electrónica _ 2022-01
   Sensado de Velocidad por Encoder
   Profesor: Johann F. Osma
   Asistente: Santiago Tovar Perilla
   Estudiantes:  
   Juan Sebastian Pinzón
   Daniel Alvarez
   Santiago Rodriguez
   Vilma Villamil
***********************************************************************************/
/***********************************************************************************
  PASOS A SEGUIR:

  1. Realice las conexiones eléctricas entre el encoder HC-020K, el driver A4988, el
     motor paso a paso y arduino.

    (NOTA: Antes del codigo se encuentra la lista de cconexiones eléctricas.
     Por favor, REVISELAS!!!)

  2. Conecte su plataforma arduino al computador por el puerto USB, abra la
     interfaz de Arduino IDE y abra el codigo de "Encoder_Instrumentacion".

  3. Elegir la placa a usar en "Herramientas >> Placa" (Arduino Uno) y el puerto a
     a trabajar "Herramientas >> Puerto" (COM# el correspondiente a la placa)

  4. Lea completamente el código y modifique los valores necesarios.

  5. Pase las librerías suministradas a la carpeta de librerías de Arduino de su computador.

  6. Suba el codigo suministrado a la placa y abra el monitor serial.

***********************************************************************************/
/***********************************************************************************
  Conexiones Eléctricas a realizar:

    HC-020K:        Arduino:
    ❶ 5V            5V Arduino
    ❷ GND           GND
    ❸ OUT           D2

    A4988:          Arduino:
    ❶ ENABLE        GND
    ❷ MS1           D5
    ❸ MS2           D6
    ❹ MS3           D7
    ❺ RESET         SLEEP
    ❻ SLEEP         RESET
    ❼ STEP          D4
    ❽ DIR           GND
    ❾ VMOT          Capacitor 100uF + 12V DC
    ❿ GND           GND
    ⓫ 2B            Motor 2da bobina B
    ⓬ 2A            Motor 2da bobina A
    ⓭ 1A            Motor 1ra bobina A
    ⓮ 1B            Motor 1ra bobina B
    ⓯ VDD           5V Arduino
    ⓰ GND           GND

    Mantenga las condiciones eléctricas para la balanza y el dosificador liquido.

***********************************************************************************/


/***************************** Librerias Externas *********************************/
#include "HX711.h"        // Modulo HX711 - Celda de carga



/**************************** Variables Calibración *******************************/
// Variables para balanza

// Variables para mini bomba de agua
int Densidad = 1;         // en g/mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE DENSIDAD CORRESPONDIENTE!!!
int Volumen_deseado = 200; // en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!

/************************ Definición de pines y variables *************************/
// Puertos y Variables para balanza
#define DOUT  A1
#define CLK  A0

float pesoRecipiente = 0;

HX711 scale(DOUT, CLK);
float calibration_factor = 870; // this calibration factor is adjusted according to my load cell
float units;
float grams;

// Puertos y Variables para mini bomba de agua
#define E1 8              // Enable Pin for motor 1 
#define Input1 9          // Control pin 1 for motor 1 valor pwm
#define Input2 10         //DIGITAL
#define Input3 11         // DIGITAL 

int OnOff = 1;            // Switch On/Off para la bomba (1 = On | 0 = Off)
int Adelante;             // Variable PWM 1
int Atras;                // Variable PWM 2
int Detener = map(0, 0, 1023, 0, 255);  // Detiene la bomba
int Var = 1022;

// Puertos y Variables para Motor paso a paso
#define StepPin 4
#define ms1 5
#define ms2 6
#define ms3 7
//#define DirPin 3 // Este pin se pondra en GND ahora

unsigned int t = 980; // Rango: 980 - 9800 que corresponde a 150 - 15 RPMs aprox.

// Puertos y Variables para Encoder
#define Encoder_pin 2      // The pin the encoder is connected 
#define PulsosRotacion 20  // Value of pluses per rotation

int Encoder = 0;           // Value of the Output of the encoder
int Estado = 1;            // Current state of the encoder to recognize the pulse changes
int Pulsos = 0;            // Counter of pulses
unsigned long InitialTime = 0;
unsigned long FinalTime;
float RPMs;




/*********************************** Setup ***************************************/

void setup() {

  Serial.begin(9600);
  //  ////// Balanza //////
  //
  CalibracionBalanza();
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");
  Serial.println(" ");
  Serial.println(" ");
  //
  //   ////// Inicializamos los pines //////
  //
  pinMode( Input1, OUTPUT);
  pinMode( Input2, OUTPUT);
  pinMode( Input3, OUTPUT);
  pinMode( E1, OUTPUT);
  pinMode( ms1 , OUTPUT);
  pinMode( ms2 , OUTPUT);
  pinMode( ms3 , OUTPUT);
  //  pinMode( DirPin , OUTPUT);
  pinMode( StepPin , OUTPUT);
  pinMode( Encoder_pin , INPUT);

  ////// Motor paso a paso //////

  //ControlMotorPasoaPaso();

}

/************************************ Loop ***************************************/
void loop() {

  ////// Balanza & Dosificador //////

  Serial.println("¡¡¡coloque el ingrediente!!!");
  for (int i = 3; i >= 0; i--)
  {
    MedidaBalanza(); //Medición actual de la Balanza
    delay(1000);
  }

  if (OnOff == 1) {

    digitalWrite(E1, HIGH);     // Activación del motor
    Serial.println(" ");
    Serial.println("¡¡¡LISTO PARA DOSIFICAR!!!");
    Serial.println(" ");
    Serial.println("Coloque el recipiente y no lo retire hasta finalizar");
    Serial.println(" ");
    delay(100);
    pesoRecipiente = MedidaBalanza();

    Serial.print("El peso del ingrediente ingresado es de:  ");
    Serial.println(pesoRecipiente);

    // GIRO DE MOTOR PASO A PASO 
    spinMotor();
    
    ////// Encoder //////
    Encoder = digitalRead(Encoder_pin);

    if (Encoder == LOW && Estado == 1)
      Estado = 0;

    else if (Encoder == HIGH && Estado == 0){
      Pulsos++;
      Estado = 1;
    }
    // CALCULO DE LOS RPMS
    if (Pulsos == PulsosRotacion){
      FinalTime = millis();
      RPMs = 60000 / (FinalTime - InitialTime);
      Pulsos = 0;
      InitialTime = FinalTime;
      Serial.print("RPM = ");
      Serial.println(RPMs);
    }
    //Dosificador:
    DosificarB(pesoRecipiente); //Dosificación de volumen según Balanza
  }
  Serial.println("FINALIZADO");
  Serial.print("El peso final es:   ");
  Serial.print(MedidaBalanza() - pesoRecipiente);
  while (true)
  {

  }

  /*///// Motor paso a paso //////


  */
}
/**
 * Función de giro del motor
 */
void spinMotor(void) {

  digitalWrite(StepPin, HIGH);
  delayMicroseconds(t);
  digitalWrite(StepPin, LOW);
  delayMicroseconds(t);
}

/*********************************** Funciones ***************************************/

//Función de Calibración de Balanza: Permite calibrar la medida de la balanza según un peso de calibración conocido
void CalibracionBalanza(void)
{

  Serial.println("HX711 calibration sketch");
  Serial.println(scale.is_ready());
  Serial.println("Remueva todo el peso del medidor.");
  Serial.println("Despues de la lectura, ponga el peso en el medidor");
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(20); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

}

//Función de Medición de Balanza: Permite obtener la medida actual en peso (g) de la balanza
float MedidaBalanza(void)
{
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  //Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  grams = units * 0.035274;
  Serial.print(units);
  Serial.print(" gramos");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  delay(100);

  return units ;
}

//Función de DosificarB: Permite entregar un volumen deseado(mL) según la medida actual de peso (g) de la balanza
void DosificarB(float pesoRecipiente)
{
  float pesoLiquido = MedidaBalanza() - pesoRecipiente;// Entrega el peso del liquido actualmente medido en gramos


  if ((Volumen_deseado / Densidad) + 0.1 >= pesoLiquido)
  {
    avanzarDosi();

    pesoLiquido = MedidaBalanza() - pesoRecipiente;
  }
  else if ((Volumen_deseado / Densidad) + 0.1 <= pesoLiquido)
  {
    analogWrite(Input1, 0);
    digitalWrite(Input2, LOW);
    digitalWrite(Input3, LOW);
  }
}
void avanzarDosi(void) {
  ///// Dosificador//////////////
  //Serial.println("Entre a dosificar xd");
  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a') {
      analogWrite(Input1, 200);
      digitalWrite(Input2, HIGH);
      digitalWrite(Input3, LOW);
    }
    else if (temp == '-' || temp == 'z') {

      analogWrite(Input1, 0);
      digitalWrite(Input2, LOW);
      digitalWrite(Input3, LOW);
    }

  }
}

/*/Función para Controlar el Motor Paso a Paso: Permite controlar el motor y los pasos de este*/
void ControlMotorPasoaPaso(void)
{

  Serial.println("Motor Control");

  digitalWrite(ms1, LOW);
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
  //  digitalWrite(DirPin, LOW);
}
