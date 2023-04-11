
#include <Adafruit_LiquidCrystal.h>

#include "Wire.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C LCD(0x27,16,2); // définit le type d'écran lcd 16 x 2

 int buttonWasUp = 2;
 int ledEnabled = 7;


void setup() {
  pinMode(ledEnabled, OUTPUT);
  pinMode(buttonWasUp, INPUT);   
}

void loop() {
  int state = 0;
  LCD.init();
  // int btn = digitalRead(buttonWasUp);
  // if(btn == 1) state = 1;
  
  //  LCD.noDisplay();
    if(digitalRead(buttonWasUp) == 1){
      state = 1; 
        while(state == 1){
                
            digitalWrite(ledEnabled, 1); 
            LCD.init(); // initialisation de l'afficheur
            LCD.backlight();
          
            LCD.setCursor(1, 0);
            LCD.print("Alert urgence");
            
            LCD.setCursor(0, 1);
            LCD.print("Chambre 2");
            // int btn = digitalRead(buttonWasUp);
            if(digitalRead(2) == 1){

             digitalWrite(ledEnabled, 0); 
            //  LCD.init(); // initialisation d
             state = 0;
             LCD.display();          
            }
          }        
    }    
  //  delay(1000);
  //  LCD.display();
  //  delay(1000);
}