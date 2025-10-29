#include <LiquidCrystal.h>
int Contrast = 60;
LiquidCrystal lcd(12,  11, 5, 4, 3, 2);
String mensaje = "";
const int botonPin = 7;
int estadoBoton = 0;

void setup()
{
  analogWrite(6, Contrast);
  lcd.begin(16,  2);
  Serial.begin(9600);
  pinMode(botonPin, INPUT_PULLUP);
}
void loop()
{

  while (Serial.available() > 0) {
  char c = Serial.read();
  if (c == '\n') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(mensaje);
    Serial.print("Mostrando: ");
    Serial.println(mensaje);
    mensaje = ""; 
   } else {
     mensaje += c;
   }
 }
  estadoBoton = digitalRead(botonPin);
  if (estadoBoton == LOW) {
      Serial.println("0");
      lcd.setCursor(0, 1);
      lcd.print("Boton: 0");
      delay(300); 
    } else {
      lcd.setCursor(0, 1);
      Serial.println("1");

      lcd.print("Boton: 1");
    }

  delay(500);
}
