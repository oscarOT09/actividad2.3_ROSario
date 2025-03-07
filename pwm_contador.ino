#include <stdio.h>

#define EncA  33 // GPIO para señal A del encoder
#define EncB  32 // GPIO para señal B del encoder

///
#define variador  34  // Pin de entrada analógica al que está conectado el potenciómetro
#define EnA  26 // PWM para Puente H
#define In1  14 // Pin de salida digital 
#define In2  27 // Pin de salida digital

#define freq 5000//Frecuencia de PWM
#define resolution 8 //Resolución de PWM 2^8 = 256
#define PWM1_Ch 0 // Canal de PWM

float voltaje, conversion, duty;
int sensorValue = 0;        // valor leído del potenciómetro
int pwm = 0;        // valor de salida al PWM
float Vcc = 3.3;
int pot;
//char opcion;
bool motorEnMovimiento = false; // Variable de estado para el motor 
///

char opcion;
volatile bool BSet = 0;
volatile bool ASet = 0;
long contador = 0;

void IRAM_ATTR Encoder()
{
  BSet = digitalRead(EncB);
  ASet = digitalRead(EncA);
  if (BSet == ASet)
  {
    contador++; 
  }
  else
  {
    contador--;
  }
}

void setup ()
{
  Serial.begin(115200);

  pinMode(EncA, INPUT_PULLUP);    // Señal A del encoder como entrada con pull-up
  pinMode(EncB, INPUT_PULLUP);    // Señal B del encoder como entrada con pull-up
  attachInterrupt(digitalPinToInterrupt(EncA), Encoder, CHANGE); // Asignar la función Encoder a la interrupción de cambio en la señal A

  pinMode(variador, INPUT);
  pinMode(EnA, OUTPUT); 
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT); 
  // configurar funciones PWM
  ledcSetup(PWM1_Ch, freq, resolution);
  ledcAttachPin(EnA, PWM1_Ch);

}

void loop(){
  lectura();

  if (Serial.available() > 0){
    opcion = Serial.read();
    switch (opcion){
      case 'D':
      motorEnMovimiento = true; // Marca el motor como en movimiento  
      derecha();
      break;

      case 'I':
      motorEnMovimiento = true; // Marca el motor como en movimiento
      izquierda();  
      break;

      case 'S':
      motorEnMovimiento = false; // Marca el motor como detenido  
      detener();
      break;

      case 'C':
      contador = 0;
      Serial.println("Contador reiniciado.");
      break;
    }
  }

  Serial.print("Pulsos: ");
  Serial.println(contador);
  // Imprimir datos solo si el motor está en movimiento  
  if (motorEnMovimiento) {  
    imprimirdatos();    
  } 
}


void lectura ()
{
   // leer señal analógica de entrada:
  pot = analogRead(variador);
  //Serial.println(pot);
  voltaje = pot * (Vcc/4095.0); 
 
  duty = 100*voltaje/Vcc;
  // mapear el rango de la salida:
  pwm = map(pot, 0, 4095, 0, 255);
 
  // cambiar el valor de salida:
  ledcWrite(PWM1_Ch, pwm);
  //analogWrite(EnA, pwm);
}

//Función detener el motor
void detener ()
{
    
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  Serial.println("EL motor está detenido");
  delay(250);
}

//Función giro derecha del motor
void derecha ()
{
  
  
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);

}

//Función giro izquierda del motor
void izquierda ()
{
  
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
}

void imprimirdatos()
{
  
  Serial.print("EL ciclo de trabajo del pwm es:  ");
  Serial.print(duty);
  Serial.print("  %");
  Serial.print("    EL cual corresponde a:  " );
  Serial.print(voltaje);
  Serial.println("  Volts" );
}