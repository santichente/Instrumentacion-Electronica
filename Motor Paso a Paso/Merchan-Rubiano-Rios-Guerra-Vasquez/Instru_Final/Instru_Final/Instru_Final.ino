
//Manuel Merchan
//Sergio Guerra

#include "HX711.h" //Modulo HX711 - Celda de carga

// Puertos y Variables para Motor paso a paso
#define StepPin 4
#define ms1 5
#define ms2 6
#define ms3 7

unsigned int t = 980; // DELAY

int Encoder = 0;           // Value of the Output of the encoder 
int Estado = 1;            // Current state of the encoder to recognize the pulse changes
int Pulsos = 0;            // Counter of pulses
unsigned long InitialTime = 0;
unsigned long FinalTime; 
float RPMs;
int tiempo = 0;

// Puertos y Variables para Encoder
#define Encoder_pin 2      // The pin the encoder is connected 
#define PulsosRotacion 20
int pesoLiquido = 0;

//Variables para la balanza
#define DOUT  A1
#define CLK  A0

HX711 balanza(DOUT, CLK);

//Variables para la mini bomba de agua
int Input1 = 8;    
int Input2 = 9; 
int Input3 = 10; //PWM

int Densidad = 1;
int Volumen_deseado = 220;

int OnOff = 1; 
int Adelante; 
int Detener = map(0, 0, 1023, 0, 255);
int Var = 1022;
float pesoRecipiente = 0;

void setup() {
  
  Serial.begin(9600);
  
  // Balanza
  CalibracionBalanza();
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");
  Serial.println(" ");
  Serial.println(" ");

  // Inicializamos los pines
  pinMode( Input1, OUTPUT);
  pinMode( Input2, OUTPUT);

  pinMode( ms1 , OUTPUT);
  pinMode( ms2 , OUTPUT);
  pinMode( ms3 , OUTPUT);
  //  pinMode( DirPin , OUTPUT);
  pinMode( StepPin , OUTPUT);
  pinMode( Encoder_pin , INPUT);

  ControlMotorPasoaPaso();
  tiempo=millis();
}


void loop() {
  if (pesoLiquido <= Volumen_deseado){
  for (int i = 3; i >= 0; i--)
  {
    MedidaBalanza(); //Medición actual de la Balanza
  }

  if (OnOff == 1) {
    Serial.println(" ");
    Serial.println("¡¡¡LISTO PARA DOSIFICAR!!!");
    Serial.println(" ");
    Serial.println("Coloque el recipiente y no lo retire hasta finalizar");
    Serial.println(" ");
    Serial.println(" ");

    pesoRecipiente = MedidaBalanza();

    DosificarB(pesoRecipiente); //Dosificación de volumen según Balanza
  }
  }
//  DosificarB(pesoRecipiente);
//  MedidaBalanza(); //Medición actual de la Balanza

  ////// Motor paso a paso //////
  digitalWrite(StepPin, HIGH); 
  delayMicroseconds(t);           
  digitalWrite(StepPin, LOW);  
  delayMicroseconds(t);
 
  ////// Encoder //////

  Encoder = digitalRead(Encoder_pin);  

  if(Encoder == LOW && Estado == 1)
  {
     Estado = 0;             
  }

  if(Encoder == HIGH && Estado == 0)
  {
     Pulsos++; 
     Estado = 1;     
  }
  
  if(Pulsos == PulsosRotacion)
  {
    FinalTime = millis();
    RPMs = 60000/(FinalTime - InitialTime);
    Pulsos = 0; 
    InitialTime = FinalTime;
    Serial.println("RPM = "); 
    Serial.println(RPMs); 
}
  
}

// FUNCIONES

//Función de Calibración de Balanza: Permite calibrar la medida de la balanza según un peso de calibración conocido
void CalibracionBalanza(void)
{
  Serial.begin(9600);
  balanza.begin(DOUT, CLK);
  Serial.print("Iniciando Balanza, lectura ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(860.87); // Establecemos la escala para el funcionamiento
  balanza.tare(30);  // Tarando Balanza
  pesoRecipiente = balanza.get_units(20);  
}

//Función de Medición de Balanza: Permite obtener la medida actual en peso (g) de la balanza
float MedidaBalanza(void)
{
  float peso; // variable para el peso actualmente medido en gramos
  float PL1 = 0;
  float PL2 = 0;
  float PL3 = 0;
  float promPL = 0;

  for (int i = 1; i >= 0; i--)
  {
    peso = balanza.get_units(5); // Entrega el peso actualmente medido en gramos
    if (peso < 0) peso = peso * -1;
    
    PL1 = peso;
    PL2 = PL1;
    PL3 = PL2;

    promPL = (PL1 + PL2 + PL3) / 3;
  }  

  Serial.print("Peso: ");
  Serial.print(promPL, 1);
  Serial.println(" g");
  return promPL;

}

//Función de DosificarB: Permite entregar un volumen deseado(mL) según la medida actual de peso (g) de la balanza
void DosificarB(float pesoRecipiente)
{
   pesoLiquido = MedidaBalanza() - pesoRecipiente;// Entrega el peso del liquido actualmente medido en gramos


  while ((Volumen_deseado / Densidad) + 0.5 >= pesoLiquido)
  {
    if ((Volumen_deseado*0.6) <= pesoLiquido){
      Adelante = map(420, 0, 1023, 0, 255);
    digitalWrite (Input1, HIGH);
    digitalWrite(Input2, LOW);
    analogWrite(Input3, Adelante);
    } else {
      Adelante = map(1020, 0, 1023, 0, 255);
    digitalWrite (Input1, HIGH);
    digitalWrite(Input2, LOW);
    analogWrite(Input3, Adelante);
    }
    
        
    pesoLiquido = MedidaBalanza() - pesoRecipiente;
  }


  if ((Volumen_deseado / Densidad) + 0.1 <= pesoLiquido)
  {
    OnOff = 0;
    digitalWrite (Input1, HIGH);
    digitalWrite(Input2, LOW);
    analogWrite(Input3, Detener);
  }
 Serial.print("f");
}

void ControlMotorPasoaPaso(void)
{
  digitalWrite(ms1, LOW);
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
//  digitalWrite(DirPin, LOW);
}
