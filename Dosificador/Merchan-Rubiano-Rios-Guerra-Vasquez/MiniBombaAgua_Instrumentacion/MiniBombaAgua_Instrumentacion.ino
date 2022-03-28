/*
   Instrumentación Electrónica _ 2022-02
   Laboratorio N°1
   Profesor: Johann F. Osma
   Asistente: Santiago Tovar Perilla
   Autor: Juliana Noguera Contreras
*/

//Librerias
#include "HX711.h" //Modulo HX711 - Celda de carga

//Variables para la balanza
#define DOUT  A1
#define CLK  A0

//Variables para la mini bomba de agua
int Input1 = 9;    // Control pin 1 for motor 1
int Input2 = 10;     // Control pin 2 for motor 1

//Variables para calibracion de balanza
int Peso_conocido = 20;// en gramos | MODIFIQUE PREVIAMENTE CON EL VALOR DE PESO CONOCIDO!!!

HX711 balanza(DOUT, CLK);

//Variables para mini bomba de agua
int Densidad = 1;// en g/mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE DENSIDAD CORRESPONDIENTE!!!
int Volumen_deseado = 39;// en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!

int OnOff = 1; // Switch On/Off para la bomba (1 = On | 0 = Off)
int Adelante; // Variable PWM 1
int Atras; // Variable PWM 2
int Detener = map(0, 0, 1023, 0, 255);
int Var = 1022;
float pesoRecipiente = 0;

void setup() {

  // Balanza
  CalibracionBalanza();
  Serial.println(" ");
  Serial.println("¡¡¡LISTO PARA PESAR!!!");
  Serial.println(" ");
  Serial.println(" ");

  // Inicializamos los pines
  pinMode( Input1, OUTPUT);
  pinMode( Input2, OUTPUT);
}


void loop() {

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
    delay(300);

    pesoRecipiente = MedidaBalanza();

    DosificarB(pesoRecipiente); //Dosificación de volumen según Balanza
  }

  MedidaBalanza(); //Medición actual de la Balanza

}

// FUNCIONES

//Función de Calibración de Balanza: Permite calibrar la medida de la balanza según un peso de calibración conocido
void CalibracionBalanza(void)
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

  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Coloque un peso conocido ");
  delay(250);

  for (int i = 3; i >= 0; i--)
  {
    Serial.print(" ... ");
    Serial.print(i);
    PromMedicion = balanza.get_value(20);//20 mediciones  //Obtiene el promedio de las mediciones analogas según valor ingresado
  }

  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Retire el peso ");
  delay(250);

  for (int i = 3; i >= 0; i--)
  {
    Serial.print(" ... ");
    Serial.print(i);
    delay(1000);
  }

  Escala = PromMedicion / Peso_conocido; // Relación entre el promedio de las mediciones analogas con el peso conocido en gramos
  balanza.set_scale(Escala); // Ajusta la escala correspondiente

  Serial.println(" ");
  Serial.println(" ");
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
    peso = balanza.get_units(10); // Entrega el peso actualmente medido en gramos
    if (peso < 0) peso = peso * -1;
    
    PL1 = peso;
    PL2 = PL1;
    PL3 = PL2;

    promPL = (PL1 + PL2 + PL3) / 3;
  }  

  Serial.print("Peso: ");
  Serial.print(promPL, 1);
  Serial.println(" g");
  //delay(25);

  return promPL;

}

//Función de DosificarB: Permite entregar un volumen deseado(mL) según la medida actual de peso (g) de la balanza
void DosificarB(float pesoRecipiente)
{
  float pesoLiquido = MedidaBalanza() - pesoRecipiente;// Entrega el peso del liquido actualmente medido en gramos


  while ((Volumen_deseado / Densidad) + 0.5 >= pesoLiquido)
  {
    Adelante = map(Var, 0, 1023, 0, 255);
    analogWrite(Input1, Adelante);
        
    pesoLiquido = MedidaBalanza() - pesoRecipiente;
  }


  if ((Volumen_deseado / Densidad) + 0.1 <= pesoLiquido)
  {
    OnOff = 0;
    analogWrite(Input1, Detener);
  }
}
