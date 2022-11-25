#include <Arduino.h>
#include <Dusk2Dawn.h>
#include "LowPower.h"
#include "RTClib.h"
#include "SPI.h"


bool debug1 = 1;

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int LED = 13;
const int Out = 12;
const unsigned INTERVAL = 8;
unsigned int OutOn = 0;
unsigned int WzSunrise;
unsigned int WzSunset;
unsigned int dayNow;
unsigned int monthNow;
unsigned int yearNow;
unsigned int minOfDay;

//- §§§ eggLight PWM §§§

int freq = 5000;
int eggLightChannel = 11;
//int resolution = 10;
bool eggLightStatus = 1;
int eggLightValue;
int eggLightValueMax = 255;
unsigned long eggLightDimTime = 20 * 60UL * 1000;
unsigned long intervalMillisEggLight = millis();
unsigned int intervalTimeEggLight;

char timeWR[6];
char timeWS[] = "00:00";
DateTime now;

void Sunrise ();
void RTCtimer ();
void eggLightDimmer ();

void setup() {
Serial.begin(9600);

pinMode(LED, OUTPUT);

//TCCR2B = TCCR2B & (B11111000) | (B00000010); // for PWM frequency of 3921.16 Hz for D3 and D11

pinMode(eggLightChannel,OUTPUT);
eggLightDimmer ();
analogWrite(eggLightChannel, 255);
delay(2000);
analogWrite(eggLightChannel, 125);
delay(2000);
analogWrite(eggLightChannel, LOW);
delay(1000);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

RTCtimer ();
Sunrise ();

delay (1000);

}


void Sunrise () {

Dusk2Dawn Weinzierl (48.12465, 15.12239, 1);

WzSunrise = Weinzierl.sunrise(yearNow, monthNow, dayNow, true);
WzSunset = Weinzierl.sunset(yearNow, monthNow, dayNow, true);

Dusk2Dawn::min2str(timeWR, WzSunrise);
Dusk2Dawn::min2str(timeWS, WzSunset);


Serial.println(WzSunrise);  // 418
Serial.println(WzSunset);   // 1004

  }




void RTCtimer ()
{
   DateTime now = rtc.now();

  dayNow = (now.day(), DEC);
  monthNow = (now.month(), DEC);
  yearNow = (now.year(), DEC);

  minOfDay = (now.hour())*60+(now.minute());
  Serial.println (minOfDay);





    delay(1000);
}

void LightSwitch (){
if (minOfDay < WzSunrise) {
  digitalWrite(LED, HIGH);
}


}


void eggLightDimmer (){
  if (eggLightValueMax == 0) eggLightValueMax = 1;
intervalTimeEggLight = eggLightDimTime/eggLightValueMax;

  
}

void eggLight () {
  if (millis() - intervalMillisEggLight > intervalTimeEggLight) {

  if (eggLightStatus == 1 && eggLightValue < eggLightValueMax) {
    eggLightValue = eggLightValue + 5;
    if (eggLightValue >= eggLightValueMax) {
      eggLightValue = eggLightValueMax;
      eggLightStatus = 0;
      }

    analogWrite(eggLightChannel, eggLightValue);
    if (debug1) Serial.print("eggLightValue: ");
    if (debug1) Serial.println(eggLightValue);
  }

  if (eggLightStatus == 0 && eggLightValue > 0) {
    eggLightValue = eggLightValue - 5;
    if (eggLightValue < 1){
      analogWrite(eggLightChannel, LOW);
      eggLightValue = 0;
      eggLightStatus = 1;
    }
    else analogWrite(eggLightChannel, eggLightValue);
    if (debug1) Serial.print("eggLightValue: ");
    if (debug1) Serial.println(eggLightValue);

  }

  intervalMillisEggLight = millis();
}
}


void loop() { 

RTCtimer ();
Sunrise ();
Serial.println();

LightSwitch ();
eggLight ();
delay(1000);

}

  