//Univerisdad de los Andes
//Instrumentación electrónica - 202210
//Dylan Zambrano
//Daniel Martinez
//Danilo Fernández
//Juan Pablo Castillo

//Balanza de instrumentación - MAIN

#include "HX711.h"

HX711 scale(A1, A0);

float calibration_factor = -104.16; // this calibration factor is adjusted according to our load cell
float units;
float volumenDeseado = 250; //en mL
float densidad = 1;

int Input1 = 9;    // Control pin 1 for motor 1
int Input2 = 10;     // Control pin 2 for motor 1
boolean fin = false;
boolean init_pausa = false;
int Var = 1023;
int Detener = map(0, 0, 1023, 0, 255);

void setup() {
  Serial.begin(9600);
  scale.set_scale();
  scale.tare(20);  //Reset the scale to 0
  Serial.println("::: Universidad de los Andes :::");
  Serial.println("::: Instrumentación Electrónica - 2210:::");
  Serial.println("-: Balanza de instrumentación :-");
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Factor Cero: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  Serial.println("espere...");
  Serial.println("::: ¡BALANZA VACÍA! :::");
  Serial.println("espere...");
  for (int i = 3; i >= 0; i--)
  {
    Serial.print(" ... ");
    Serial.print(i);
    delay(1000);
  }
  Serial.println(" ");
  Serial.println("::: ¡Coloque un objeto! :::");
  Serial.println(" ");
  pinMode( Input1, OUTPUT);
  pinMode( Input2, OUTPUT);
}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Midiendo: ");
  units = scale.get_units(10) / 10;
  if (units < 0.5)
  {
    units = 0.00;
  }
  Serial.print(units, 1);
  Serial.print(" g");
  Serial.println();
  Serial.println(Var);
  onoff();
}

void onoff() {
  int Adelante = map(Var, 0, 1023, 0, 255);
  if (fin == false) {
    analogWrite(Input1, Adelante);

    float volumen = units; // densidad
    float lim_prop1 = volumenDeseado * 0.4;
    float lim_prop2 = volumenDeseado * 0.6;
    float lim_prop3 = volumenDeseado * 0.8;
    float lim_prop4 = volumenDeseado * 0.97;
    if (volumenDeseado < 100) {
      Var = 512;
      if(volumenDeseado < 25){
      Var=300;
      }      
    }
    if (volumenDeseado >= 100) {
      if (volumen >= lim_prop1) {
        Var = 1023 * 0.8;
      }
      if (volumen >= lim_prop2) {
        Var = 1023 * 0.6;
      }
    }
    if (volumen >= lim_prop3) {
      Var = Var - 12;
    }
    if (volumen >= lim_prop4) {
      Var = 0;

    }

    if (Var <= 0) {
      Var = 0;
      digitalWrite(Input1, LOW);
      fin = true;
    }
  }
}
