
#include "HX711.h"
#include <LiquidCrystal_I2C.h>
 
String mensaje_entrada;
const int DOUT=A1;
const int CLK=A0;
HX711 balanza;
LiquidCrystal_I2C lcd(0x27,16,2);
const int inputPin =8;
float peso;

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT);
  
  balanza.begin(DOUT, CLK);
  
  //Serial.print("Lectura del valor del ADC: ");
  //Serial.println(balanza.read());
  //Serial.println("No ponga ningún objeto sobre la balanza");
  //Serial.println("Destarando...");
  //Serial.println("...");
  
 balanza.set_scale(-515570.45); //La escala por defecto es 1
  balanza.tare(20);  //El peso actual es considerado Tara.
  //Serial.println("Listo para pesar:");

  lcd.init();
  
  //Encender la luz de fondo.
  lcd.backlight();
  lcd.clear();
  //Ajustamos el cursor del LCD
  lcd.setCursor(0,0);
  // Escribimos el Mensaje en el LCD.
   

 
 

}

void loop() 
{

 if (Serial.available()>0)
  {
    mensaje_entrada= Serial.readStringUntil('\n');
    if (mensaje_entrada.compareTo("GET_PESO")==0)
    {
      peso=digitalRead(inputPin);
      peso=balanza.get_units(20),3;
      Serial.println(peso);
      lcd.print(peso);
       
    }
  }
lcd.setCursor(0,0);
 lcd.print(' ');
    

}
