

#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Keypad.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

#define DHT_SENSOR_PIN  33 // ESP3 pin GIOP33 connected to DHT11 sensor
#define Type DHT11
#define LIGHT_SENSOR_PIN 36
#define SS_PIN  5  // ESP32 pin GIOP5 
#define RST_PIN 27 // ESP32 pin GIOP27 
#define ROW_NUM     4 // four rows
#define COLUMN_NUM  4 // four columns
#define AOUT_PIN 39 // ESP32 pin GIOP39 (ADC0) that connects to AOUT pin of moisture sensor
#define FAN_PIN  25  // ESP32 pin GIOP25, which connects to the IN pin of relay
#define TRIG_PIN   15 // ESP32 pin GIOP26 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN   2 // ESP32 pin GIOP25 connected to Ultrasonic Sensor's ECHO pin
#define BUZZER_PIN 14 // ESP32 pin GIOP17 connected to Piezo Buzzer's pin
#define DISTANCE_THRESHOLD 50 // centimeters

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo,servoDoor;
DHT dht(DHT_SENSOR_PIN, Type); 
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

int servoPin = 26;
int Moteur=16;
const String password = "7890"; // change your password here
int ledPin = 4;
bool ledState = 1;
float duration_us, distance_cm;
// byte pin_rows[ROW_NUM]      = {22, 21, 17, 16}; // GIOP22, GIOP21, GIOP17, GIOP16 connect to the row pins
// byte pin_column[COLUMN_NUM] = {4, 0, 2, 15};   // GIOP4, GIOP0, GIOP2, GIOP15 connect to the column pins
// Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
String input_password;
String msgRfid= "0";
int statusMoteur = 0;
int statusBuzzer = 0;
int statusToit = 0;
int statusDoor = 0;



void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522 
  dht.begin();
  lcd.init(); // initialize the lcd
  lcd.backlight(); 


  servoDoor.attach(12);
  myservo.setPeriodHertz(50); 
  myservo.attach(servoPin);
  input_password.reserve(32); // maximum input characters is 33, change if needed

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 0);

  pinMode(TRIG_PIN, OUTPUT);   // set ESP32 pin to output mode
  pinMode(ECHO_PIN, INPUT);    // set ESP32 pin to input mode
  pinMode(BUZZER_PIN, OUTPUT); // set ESP32 pin to output mode
  pinMode(Moteur, OUTPUT);
  pinMode(FAN_PIN, OUTPUT); 
}

void loop() {

//  digitalWrite(FAN_PIN, 1); 
    int humSol = analogRead(AOUT_PIN); // read the analog value from sensor
    int analogValue = analogRead(LIGHT_SENSOR_PIN);
    char character;
    String contents = "";

    if (Serial.available() > 0) {
        // Read the incoming data and convert it to an integer
        character = Serial.read();
        contents.concat(character);
        if (contents != "" && contents == "1") {
        // Serial.println("allumer");
        digitalWrite(ledPin, 1);
        }else if (contents != "" && contents == "0") {
        // Serial.println("eteindre");
        digitalWrite(ledPin, 0);
        }else if (contents != "" && contents == "2") {
        // Serial.println("allumer");
           digitalWrite(Moteur, 1); 
        }else if (contents != "" && contents == "3") {
        // Serial.println("eteindre");
           digitalWrite(Moteur, 0); 
           digitalWrite(FAN_PIN, HIGH);
        }else if (contents != "" && contents == "4") {
        // Serial.println("allumer");
           digitalWrite(FAN_PIN, 1);
        }else if (contents != "" && contents == "5") {
        // Serial.println("eteindre");
           digitalWrite(FAN_PIN, 0);
        }else if (contents != "" && contents == "6") {
        // Serial.println("ouverture");
            servoDoor.write(180); 
            statusDoor = 1;
        }else if (contents != "" && contents == "7") {
        // Serial.println("fermeture");
            servoDoor.write(0);
            statusDoor = 0; 
        }

    }


    //ouverture toit
    if (analogValue <= 700) {
      myservo.write(180); 
      statusToit = 1;
    } else if (analogValue > 700) {
      myservo.write(0);
      statusToit = 0; 
    }

    if(humSol >= 3500){
      digitalWrite(Moteur, 1);
    }else if(humSol <= 18){
      digitalWrite(Moteur, 0);
    }

    
      
    String content= "";
    msgRfid= "0";

    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
      if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        for (int i = 0; i < rfid.uid.size; i++) {
          content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
          content.concat(String(rfid.uid.uidByte[i],HEX));
        }
        content.toUpperCase();
      
        if(content.substring(1) == "D3 C1 BC 2E"){
            msgRfid= "1";
            // delay(1000);
        }
        else{
            msgRfid= "0";
            // delay(1000);
        }

        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
      }
    }


    // StaticJsonBuffer<200> jsonBuffer;
    float humidite = dht.readHumidity(); 
    float temperature = dht.readTemperature(); 
      
    int temp = static_cast<int>(temperature); 
    int hum = static_cast<int>(humidite);  


    // Create a JSON object
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    // Add some data to the object
    root.set("temp",temp);
    root.set("hum", hum);
    root.set("rfid", msgRfid);
    root.set("lum", analogValue);
    root.set("sol", humSol);
    root.set("buzzer", statusBuzzer);
    root.set("toit", statusToit);
    root.set("door", statusDoor);


    // Serialize the JSON object to a string
    String jsonStr;
    root.printTo(jsonStr);

    // Send the JSON string to Node.js through serial
    Serial.println(jsonStr);

    ultraSon();
    startFan(hum);
    lc2Displayer(temp,hum);

    // delay(1000);  
        
  
}

void ultraSon(){
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  if (distance_cm < DISTANCE_THRESHOLD){
    tone(BUZZER_PIN, 900);
    statusBuzzer = 1;
  }else{
      noTone(BUZZER_PIN);
      statusBuzzer = 0;
  }

  // print the value to Serial Monitor
  // Serial.print("distance: ");
  // Serial.print(distance_cm);
  // Serial.println(" cm");
}

void lc2Displayer(int temp, int hum) {
  lcd.clear();              // clear display
  lcd.setCursor(0, 0);      // move cursor to   (0, 0)
  lcd.print("Temp:");       // print message at (0, 0)
  lcd.print(temp);
  lcd.print("°"); 
  lcd.setCursor(0, 1);      // move cursor to   (0, 1)
  lcd.print("Hum:"); // print message at (0, 1)
  lcd.print(hum);
  lcd.print("%"); 
  delay(2000);                  // display the above for two seconds

  // lcd.clear();                  // clear display
  // lcd.setCursor(3, 0);          // move cursor to   (3, 0)
  // lcd.print("DIYables");        // print message at (3, 0)
  // lcd.setCursor(0, 1);          // move cursor to   (0, 1)
  // lcd.print("www.diyables.io"); // print message at (0, 1)
  // delay(2000);                      // display the above for two seconds
}


void startFan(int hum){
  if(hum >=100){
      digitalWrite(FAN_PIN, 0);
    }else if(hum < 70){
      digitalWrite(FAN_PIN, 1);
    }
}


void authRfid(){
   String content= "";

  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      for (int i = 0; i < rfid.uid.size; i++) {
        // Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        // Serial.print(rfid.uid.uidByte[i], HEX);
        content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(rfid.uid.uidByte[i],HEX));
      }
      content.toUpperCase();
      String msg= "";
      if(content.substring(1) == "D3 C1 BC 2E"){
          msg= "1";
          Serial.println(msg);
          delay(1000);
      }else{
          msg= "0";
          Serial.println(msg);
          delay(1000);
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

void tempHum(){
    StaticJsonBuffer<200> jsonBuffer;
    float humidite = dht.readHumidity(); 
    float temperature = dht.readTemperature(); 
      
    int temp = static_cast<int>(temperature); 
    int hum = static_cast<int>(humidite);  

    char json[] = "{}"; 
    JsonObject& root = jsonBuffer.parseObject(json);
    bool t = false;
    if(!root.success()) {
      Serial.println("no");
    }else{
      
       
      root["temp"] = temp;
      root["hum"] = hum;

      Serial.println();
      root.prettyPrintTo(Serial);
    }
  
  delay(2000);    
}

void keyPad(){
  // char key = keypad.getKey();

  // if (key) {
  //   Serial.println(key);

  //   if (key == '*') {
  //     input_password = ""; // clear input password
  //   } else if (key == '#') {
  //     if (password == input_password) {
  //       // Serial.println("The password is correct, ACCESS!");
  //       // DO YOUR WORK HERE
  //       Serial.println("ACCESS!");
  //       servo();
  //     } else {
  //       // Serial.println("The password is incorrect,DENIED!");
  //       Serial.println("DENIED!");
  //     }

  //     input_password = ""; // clear input password
  //   } else {
  //     input_password += key; // append new character to input password string
  //   }
    
  // }
}

void servo(){
  myservo.write(0); 
  delay(2000);
  myservo.write(90);
  delay(2000);
}

void photoSensor(){
  int analogValue = analogRead(LIGHT_SENSOR_PIN);

  Serial.print("Analog Value = ");
  Serial.println(analogValue);   // the raw analog reading

  // We'll have a few threshholds, qualitatively determined
  delay(3000);
  if (analogValue < 40) {
    Serial.println(" => Dark");
  } else if (analogValue < 800) {
    Serial.println(" => Dim");
  } else if (analogValue < 2000) {
    Serial.println(" => Light");
  } else if (analogValue < 3200) {
    Serial.println(" => Bright");
  } else {
    Serial.println(" => Very bright");
  }

  delay(3000);
}


