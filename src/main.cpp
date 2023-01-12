#include <Arduino.h>



bool debug = 1;



const int LED = 13;


int freq = 5000;
int eggLightChannel = 11;
int eggLightValueMax = 255;


unsigned long intervalMillisEggLight = millis();
unsigned int intervalTimeEggLight;





void setup() {
if (debug) Serial.begin(9600);

pinMode(LED, OUTPUT);
pinMode(eggLightChannel,OUTPUT);


 

analogWrite(eggLightChannel, 255);
delay(2000);
analogWrite(eggLightChannel, 125);
delay(2000);
analogWrite(eggLightChannel, LOW);
delay(1000);

analogWrite(LED, 255);
delay(2000);
analogWrite(LED, 125);
delay(2000);
analogWrite(LED, LOW);
delay(1000);



}






 

void loop() { 


}
  