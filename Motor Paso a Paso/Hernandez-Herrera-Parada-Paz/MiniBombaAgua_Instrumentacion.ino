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
int Input2 = 11;     // Control pin 2 for motor 1

//Variables para calibracion de balanza
int Peso_conocido = 20;// en gramos | MODIFIQUE PREVIAMENTE CON EL VALOR DE PESO CONOCIDO!!!

HX711 balanza(DOUT, CLK);

//Variables para mini bomba de agua
int Densidad = 0.998;// en g/mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE DENSIDAD CORRESPONDIENTE!!!

int Volumen_deseado = 39;// en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!

int OnOff = 1; // Switch On/Off para la bomba (1 = On | 0 = Off)
int Adelante; // Variable PWM 1
int Atras; // Variable PWM 2
int Detener = map(0,0,5, 0, 255);
float pesoRecipiente = 0;
float voltaje_llenado_rapido=4.6 //9.461ml/s
float voltaje_llenado_lento=3.2 //5.586ml/s
int Detener = map(0, 0, 1023, 0, 255);
int Var = map(5,0,5,0,1023);

int llenar_rapido=map(voltaje_llenado_rapido,0,5,0,255)
int llenar_lento=map(voltaje_llenado_lento,0,5,0,255)

 pesoEnMedioSegundo=5.586*0.5*1000*/densidad;
boolean seguir=true;  
void setup() {

  // Balanza
 
   Serial.begin(9600);
  Serial.println("Balanza");
  Serial.print("...Destarando...");
  balanza.tare(10);  //El peso actual es considerado "Tara".
  delay(50);
  Serial.print("Terminó de destarar");
  // Inicializamos los pines
  pinMode( Input1, OUTPUT);
  pinMode( Input2, OUTPUT);
  Serial.println("Peso recipiente:");
  // pesoRecipiente = MedidaBalanza()
  //Serial.print(pesoRecipiente, 1);
  //Serial.println(" g");
  llenar_rapido()
  pesoDeseado=Volumen_deseado*Densidad;
}


void loop() {

 pesoActual=MedidaBalanza();
  Serial.print(pesoActual, 1);
   Serial.println(" g");
 if(seguir==true){
  analogWrite(Input1, llenar_lento);
 if (pesoDeseado< pesoActual+pesoEnMedioSegundo){
  dif=pesoDeseado-pesoActual;
  difvol=dif/Densidad;
  time1=difvol/5.586;
  delay(time1*1000);
  analogWrite(Input1, detener);
  
 }
 delay(500);
}
}
// FUNCIONES
//Función de llenado rapido
float llenar_rapido(void){
  volumen_a_llenar=0.8*Volumen_deseado; /ml
  tiempo_de_llenado=volumen_a_llenar/9.461;
  analogWrite(Input1, llenar_rapido);
  delay(tiempo_de_llenado*1000)
  analogWrite(Input1, detener);
}

//Función de Medición de Balanza: Permite obtener la medida actual en peso (g) de la balanza
float MedidaBalanza(void)
{
   float prom;
  prom = balanza.get_value(20);
  balanza.set_scale(asigEscala(prom));
  
  float peso = balanza.get_units(5); // Entrega el peso actualmente medido en gramos
  if(peso<0) peso = peso*-1;
  
   Serial.println(" ");
  Serial.print("Peso: ");
  Serial.print(peso, 1);
  Serial.println(" g");
  delay(100);

return peso;
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




float asigEscala (float medicion) {

  if (medicion<=57046.25) return 1954;
  else if (medicion<=137246.8) return 1960.67;
  else if (medicion<=196300) return 1963;
  else if (medicion<=294627.3) return 1970.0;
  else if (medicion<=431644.8) return 1971;
  else if (medicion<=589018.8) return 1963.4;
  else if (medicion<=788300) return 1970.5;
  else if (medicion<=924806.5) return 1967.67;
  else if (medicion<=979000) return 1957.51;
  else if (medicion<=1480000) return 1969.9;
  else if (medicion<=1554288) return 1969;
  else if (medicion<=1870283) return 1962.5;
  else return 1963;
  
}
