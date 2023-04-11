/*
   Code 16 - Edurobot.ch, destiné à l'Arduino
   Objectif: Éteindre une LED dès que la luminosité est suffisante
*/
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <AccelStepper.h>
#include <Stepper.h>
#include <Keypad.h>//https://github.com/Chris--A/Keypad
#include <Servo.h>
const int stepsPerRevolution=2038;
Stepper mystepper= Stepper(stepsPerRevolution, 18, 20, 19, 21);

//***** EN-TÊTE DÉCLARATIF *****
#define motorPin1  18      // IN1 on the ULN2003 driver
 #define motorPin2  19      // IN2 on the ULN2003 driver
 #define motorPin3  20     // IN3 on the ULN2003 driver
 #define motorPin4  21     // IN4 on the ULN2003 driver
// // Définit le type d'interface AccelStepper; Moteur 4 fils en mode demi-pas:
#define MotorInterfaceType 8
// // Initialisation avec la séquence de broches IN1-IN3-IN2-IN4 pour utiliser la bibliothèque AccelStepper avec le moteur pas à pas 28BYJ-48:
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);

 int lightPin ;  //On renomme la broche A0 en "lightPin"
 int ledPin = 13;  //On renomme la broche 12 en "ledPin"


// Creer un objet de tupe Servo
  Servo myservo;
//Creer une variable de stockage de la position de l'angle du cervo-moteur:
int angle = 0;
//Constants
//Definir le numéro de port du cervo-moteur:
#define servoPin 12 // On branche le servo-moteur (broche de signal) à la pin 10 de l'arduino
#define ROWS 4
#define COLS 4                              
#define DLENGTH 3  // On fixe ici le nombre maximal de caractères à saisir pour les code (ouverture comme fermeture)
//Parameters
// kp4x4Keys = les caractères du keypad
const char kp4x4Keys[ROWS][COLS]  = {{'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};
byte rowKp4x4Pin [4] = {10, 9, 8, 7}; // Les pins arduino dédiées aux caractères de ligne
byte colKp4x4Pin [4] = {6, 5, 4, 3}; //Les pins arduino dédiées aux caractères de colonne
char digits[DLENGTH]; // On Déclare un tableau nommé "digits" de taille 5 carateres (DLENGTH=5)
char codeOuv[]="3AC"; // LE code d'ouverture porte
char codeFerm[]="C3A"; // LE code de fermeture porte

int index=0; // On fixe le premier indice du tableau (indice de la premiere case du tableau "digits") à zero (0)

//Variables
Keypad monkeyPad  = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);
const byte ledvPin = 14;
const byte interruptPin = 2;
volatile byte state = LOW; 

// Set DHT pin:
#define DHTPIN 15
#define FANPIN 53
// Set DHT type, uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
DHT dht = DHT(DHTPIN, DHTTYPE);

void setup()
{
    lcd.backlight();// turn on lcd backlight
    lcd.init();// initialize lcd
    pinMode(FANPIN, OUTPUT);
    dht.begin();
    pinMode(lightPin, INPUT);
 // Définit le nombre maximum de pas par seconde:
    stepper.setMaxSpeed(1000);
// // Définit l'accélération maximale en pas par seconde ^ 2:
    stepper.setAcceleration(1000);
    pinMode (ledPin, OUTPUT);
    pinMode (interruptPin, INPUT);
     pinMode (ledvPin, OUTPUT);
    Serial.begin(9600);
    // attachInterrupt(digitalPinToInterrupt(btnOff), Lafct, FALLING);
     stepper.moveTo(0); // Course à la position avec vitesse et accélération définies:
      stepper.runToPosition();
       //rattacher le cervo (myservo) à une pin arduino (pin 10):
  myservo.attach(servoPin);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE); 
}

void loop()
{
   OuverFermeture_Porte();
    gestionToitLampe();
   GestionVentillation();
     
}

void blink()
{
  state = !state;
}

void gestionToitLampe()
{
  while(state==LOW)

  {
digitalWrite(ledvPin, LOW);    
    int seuilbas = 250; 
    int lumiere = analogRead (lightPin);
    if (lumiere > seuilbas)  //Si la luminosité est plus élevée que le seuil.. .
    {
        digitalWrite (ledPin, LOW);  //... alors on éteint la LED.
        stepper.moveTo(-300); // Course à la position avec vitesse et accélération définies:
        stepper.runToPosition(); 
    }
    else // Lumiere est peu
    {
       digitalWrite (ledPin, HIGH);
        stepper.moveTo(300);
        stepper.runToPosition();
    }


    Serial.println(analogRead (lightPin)); delay(1000);
}
}

// Ici je déclare une fonction qui gère l'ouverture et la fermeture de la porte en fonction du code saisi
void OuverFermeture_Porte() { 
 
  while(state==HIGH)

  {
   //// On lit et on recueille le caractère saisi dans la variable nommée "custoKey"
  char customKey = monkeyPad.getKey(); // "monkeyPad" désigne l'instance de Keypad (notre keypad alors) et 
  //"getKey", est une methode de la bibliothèque keypad

  if (customKey) { // Si on appui sur une touche, on enregistre sur le tableau "digits" et on passe à la case suivante jusquà ce qu'il fasse 5 caracteres
    Serial.println(customKey);
    digits[index]=customKey;
    index=index+1;
    if(index==DLENGTH){ // On affiche maintenant les caractères saisi que l'orsque ça fait 5 caractères
    char customKey = monkeyPad.getKey();
      Serial.println(digits);
      index=0; // Ici on reinitialise le compteur de chiffres saisis à zéro apresqu'il ait atteint 5 caractères
      if(strcmp(digits,codeOuv)==0){
        Serial.println(F("Code bon! ouverte"));
        // Ce qui va se passer quant le bon code est saisi
//     // Ordonner au servo de tourner à un angle particulier (90° ici), marquer une pause d'une minute et de refaire 180°
        myservo.write(180);
        // delay(1000);
        // myservo.write(-180);
      }

      // Si le code de femeture
      if(strcmp(digits,codeFerm)==0){
        Serial.println(F("Code bon! tu ferme"));
        // Ce qui va se passer quant le bon code est saisi
//     // Ordonner au servo de tourner à un angle particulier (90° ici), marquer une pause d'une minute et de refaire 180°
        myservo.write(-180);
        // delay(1000);
        // myservo.write(-180);
      }

    } 
  }
  }
 
}

//FONCTION QUI GÈRE LA VENTILLATION
void GestionVentillation()
{
  // Wait a few seconds between measurements:
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read the humidity in %:
  float h = dht.readHumidity();
  // Read the temperature as Celsius:
  float t = dht.readTemperature();
  // Read the temperature as Fahrenheit:
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again):
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }
  lcd.clear();
   lcd.setCursor(0,0);// set the cursor on the first row and column
   lcd.print("Humidity=");
   lcd.print((float)dht.readHumidity());//print the humidity
   lcd.print("%");
   lcd.setCursor(0,1);//set the cursor on the second row and first column
   lcd.print("Temp=");
   lcd.print((float)dht.readTemperature());//print the temperature
   lcd.print("Celsius");
   delay(2000);
   lcd.clear();

  if(h<70.00)
  {
    digitalWrite (FANPIN, HIGH); 
  }
  else if(h>=70.00)
  {
    digitalWrite (FANPIN, LOW); 
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" \xC2\xB0");
  Serial.println("C | ");

}

