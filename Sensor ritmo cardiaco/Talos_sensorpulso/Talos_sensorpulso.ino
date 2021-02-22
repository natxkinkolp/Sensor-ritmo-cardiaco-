
/*
 * Talos Electronics 
 */

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);   
int pulsePin = 0;                   // Sensor de Pulso cardiaco en A0
volatile int BPM;                   // Pulsaciones por minuto
volatile int Signal;                // Entrada de datos del sensor de pulsos
volatile int IBI = 600;             // tiempo entre pulsaciones
volatile boolean Pulse = false;     // Verdadero cuando la onda de pulsos es alta, falso cuando es Baja
volatile boolean QS = false;        // Verdadero cuando el Arduino Busca un pulso del Corazon

void setup(){ 
 pinMode(13, OUTPUT); 
 lcd.init();                        
 lcd.backlight();                           
 lcd.clear();                                
 Serial.begin(9600);                
 interruptSetup();                  
}

void loop(){
 int pulso = analogRead(A0);          
 if (pulso >= 530) {                  
    digitalWrite(13, HIGH);
 }  
 else{
    digitalWrite(13, LOW);
 }  
   lcd.setCursor(1,0);
   lcd.print("BPM= ");
   lcd.print(BPM);
   lcd.print("   "); 
   //Serial.print("PPM = ");  Serial.println(BPM); 
   Serial.println(pulso);                          
  if (QS == true){                       
    QS = false;                           
  }
}
