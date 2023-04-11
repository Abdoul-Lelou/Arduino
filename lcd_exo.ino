#include <Adafruit_LiquidCrystal.h>

#include "Wire.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C LCD(0x27,16,2); // définit le type d'écran lcd 16 x 2



void setup() {
   LCD.init(); // initialisation de l'afficheur
   LCD.backlight();
   
   LCD.setCursor(1, 0);
   LCD.print("Bonjour");
  
   LCD.setCursor(0, 1);
   LCD.print("Monde");
}

void loop() {
   LCD.noDisplay();
   delay(1000);
   LCD.display();
   delay(1000);
}