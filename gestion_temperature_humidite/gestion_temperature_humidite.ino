

#include "DHT.h" 
#define PinCapteur 2
 // Pin digital auquel on relie le capteur 
 #define Type DHT11 
#include <IRremote.h>  
//  Type de capteur 
 DHT dht(PinCapteur, Type); 
IRrecv IR(A1);
 decode_results results;
 int buzzer = 6;
 int ventillo = 8;
 void setup() { 
   Serial.begin(9600); 
   dht.begin(); 
   pinMode(buzzer,OUTPUT);
   pinMode(8,OUTPUT);
   IR.enableIRIn();
  }
   
 int statusVent = 0;
  void loop(){
    
    // delay(2000);    
    float humidite = dht.readHumidity(); 
    float temperature = dht.readTemperature(); 
  
    int temp = static_cast<int>(temperature); 
    int hum = static_cast<int>(humidite);  

   
   
    

    
     if(IR.decode()){
      Serial.println(IR.decodedIRData.decodedRawData);
      //delay(1500);

        if(IR.decodedIRData.decodedRawData == 3860463360  ){
          statusVent = 1;
          if (statusVent == 1){         
            digitalWrite(8, HIGH); 
            Serial.println("zero allumer"); 
          }        
        }else if(IR.decodedIRData.decodedRawData == 3125149440){
          statusVent = 0;
          if(statusVent == 0){
            digitalWrite(8, LOW); 
            Serial.println("un eteint");
          }           
        }
        IR.resume();

        
    }
    
       if(temp >35){
         digitalWrite(8, 1);
         tone(buzzer, 2000);
       }else if(temp <= 30 && statusVent == 0){
         digitalWrite(8, 0);
         noTone(buzzer);         
       }
             
       if (Serial.available() > 0){
            char p = Serial.read();    
            if(p == '1'){  
              digitalWrite(8, 1);
            }else if(p == '0'){  
              digitalWrite(8, 0); 
              statusVent = 0;              
            }
            Serial.println(p);       
        }
    

   
    
    
    // Serial.println("humidite et temperature:");
    Serial.print(hum);
    Serial.print("/");
    Serial.println(temp);
    //delay(2000);
    // Serial.print("Température:"); 
    // Serial.println(temp); 
    // Serial.println(p);
  }






// #include "DHT.h" 
// #include <IRremote.h> 
// #define PinCapteur 2   // Pin digital auquel on relie le capteur
// #define Type DHT11 

// IRrecv irrecv(A1);
// decode_results results;
//    // Type de capteur
// DHT dht(PinCapteur, Type);

// int ventillo = 4;
// int buzzer = 6;
// void setup() {
//   Serial.begin(9600); 
//   irrecv.enableIRIn();
//   dht.begin();
//   pinMode(ventillo,OUTPUT);  
//   pinMode(buzzer,OUTPUT); 
//   pinMode(12, OUTPUT);  // initialise les broches
// }

// void loop(){
//   delay(2000);   
//   float humidite = dht.readHumidity();
//   float temperature = dht.readTemperature();
//   int niveauTemp = static_cast<int>(temperature);  
//   if(niveauTemp > 30){
//       digitalWrite(ventillo,1);   
//       tone(buzzer, 900);
//   }else if (irrecv.decode(&results)) {
//       Serial.println(results.value);
    
//       if (results.value == 16718055) { tone(buzzer, 900); }
//       if (results.value == 16724175) {  noTone(buzzer); }
 
//       irrecv.resume();
//    }else{
//     digitalWrite(ventillo,0); 
//     noTone(buzzer);
//   }
//   Serial.print("humidite:"); 
//   Serial.print(humidite);
//   Serial.print("Température:"); 
//   Serial.println(temperature);
// }


