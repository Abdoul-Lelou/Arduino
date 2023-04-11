#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

Servo myservo;
const int ROW_NUM = 4;     //four rows
const int COLUMN_NUM = 4;  //four columns
const int LED_ON = 11;
//LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte pin_rows[ROW_NUM] = { 9, 8, 7, 6 };       //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = { 5, 4, 3, 2 };  //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

String txtMsg = "";
int pos = 0;
int ess= 2;
void setup() {
  lcd.init();  // initialisation de l’afficheur
  lcd.clear();
  lcd.backlight();      // active le rétro-éclairage
  lcd.setCursor(0, 1);  // se positionner à la première ligne
  pinMode(LED_ON, OUTPUT);
  myservo.attach(13); 
  pinMode(12, OUTPUT);
  pinMode(A1, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // lcd.print("okk");
  char key = keypad.getKey();

 Serial.println(txtMsg);
  if (key)  // On appuie sur une touche du clavier matriciel
  {
    lcd.clear();
    // lcd.print(key);
    // Serial.print(key);
      int num = int(key);
      txtMsg += key;
    // Serial.println(txtMsg);

        if (txtMsg == "157") {
      
          Serial.println(txtMsg);
          Serial.println("Ouverture");
          lcd.print("Ouverture");
          for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
              // in steps of 1 degree
              myservo.write(pos);              // tell servo to go to position in variable 'pos'
              // delay(10);                       // waits 15ms for the servo to reach the position
          }
          
          digitalWrite(12, 1);
          delay(3000);
          
          
          Serial.println("Fermeture");
          lcd.clear();
          lcd.print("Fermeture");
          for (pos = 90; pos < 180; pos+= 1) { // goes from 180 degrees to 0 degrees
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            // delay(10);                       // waits 15ms for the servo to reach the position
          }    
          digitalWrite(12, 0);      
          txtMsg ="";
        }
        // if(ess == 0){
        //   noTone(A1); 
        //   ess = 2;  
        // }     
          
        }

      
    }  