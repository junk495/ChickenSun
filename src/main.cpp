#include <Arduino.h>
#include <Dusk2Dawn.h>
#include "LowPower.h"
#include "RTClib.h"
#include "SPI.h"

// Test to check if GitHub works 

bool debug = 1;

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int LED = 13;
unsigned int OutOn = 0;
unsigned int WzSunrise;
unsigned int WzSunset;
unsigned int dayNow;
unsigned int monthNow;
unsigned int yearNow;
unsigned int minOfDay;

const unsigned sleepInterval = 30; // sec

unsigned int loopPeriod = 10000;
unsigned long time_now = 0;

int voltageSensorValue;
int voltageLimit = 280;
int voltageShutdownCount = 0;
float voltage;

//- §§§ eggLight PWM §§§

int freq = 5000;
int eggLightChannel =6;
bool eggLightStatus;
int eggLightValue;
int eggLightValueMax = 255;
unsigned long eggLightDimTime = 20 * 60UL * 1000;

unsigned int eggLightPreSunR = 60;
unsigned int eggLightPostSunR = 10;

unsigned int eggLightPostSunS = 60;
unsigned int eggLightPreSunS = 60;

unsigned long intervalMillisEggLight = millis();
unsigned int intervalTimeEggLight;

char timeWR[6];
char timeWS[] = "00:00";
DateTime now;

void Sunrise ();
void RTCtimer ();
void eggLightDimmer ();
void eggLightSwitch ();

void setup() {
Serial.begin(9600);
delay(500);
pinMode(LED, OUTPUT);
pinMode(eggLightChannel,OUTPUT);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2023, 1, 28, 14, 18, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2022, 11, 13, 22, 25, 0));

RTCtimer ();
delay(100);
Sunrise ();
delay(100);
eggLightDimmer ();
delay(100);
eggLightSwitch ();
delay(100);

analogWrite(eggLightChannel, 255);
analogWrite(LED, 255);
delay(2000);
analogWrite(eggLightChannel, 125);
analogWrite(LED, 125);
delay(2000);
analogWrite(eggLightChannel, LOW);
analogWrite(LED, LOW);
delay(1000);


}


void Sunrise () {

Dusk2Dawn Weinzierl (48.12465, 15.12239, 1);

WzSunrise = Weinzierl.sunrise(yearNow, monthNow, dayNow, false);
WzSunset = Weinzierl.sunset(yearNow, monthNow, dayNow, false);

Dusk2Dawn::min2str(timeWR, WzSunrise);
Dusk2Dawn::min2str(timeWS, WzSunset);


if (debug==1)  {
Serial.println("Sun Data:");  
Serial.print("Sunrise: ");
Serial.print(timeWR);
Serial.print(" - ");
Serial.println(WzSunrise);  // 418
Serial.print("Sunset:  ");
Serial.print(timeWS);
Serial.print(" - ");
Serial.println(WzSunset);   // 1004
    Serial.print(dayNow);
    Serial.print('/');
    Serial.print(monthNow);
    Serial.print('/');
    Serial.println(yearNow);
    Serial.println("------------------");
  }
}


void RTCtimer ()
{
   DateTime now = rtc.now();

  dayNow = (now.day());
  monthNow = (now.month());
  yearNow = (now.year());

  minOfDay = (now.hour())*60+(now.minute());

if (debug == 1)  {
Serial.println("Time Data:");
Serial.print("Time: ");
Serial.print(now.day(), DEC);
Serial.print(".");
Serial.print(now.month(), DEC);
Serial.print(".");
Serial.print(now.year(), DEC);
Serial.print(" - ");
Serial.print(now.hour(), DEC);
Serial.print(":");
Serial.print(now.minute(), DEC);
Serial.print(" - ");
Serial.println(minOfDay);
Serial.println("------------------");

  }
}


void eggLightSwitch (){

//if (((minOfDay > (WzSunrise-eggLightPreSunR)) && (minOfDay < (WzSunrise+eggLightPostSunR))) || ((minOfDay > (WzSunset-eggLightPreSunS)) && (minOfDay < (WzSunset+eggLightPostSunS-eggLightPreSunS))))

if (((minOfDay > (360)) && (minOfDay < (WzSunrise+eggLightPostSunR))) || ((minOfDay > (WzSunset-eggLightPreSunS)) && (minOfDay < (1080))))

{ eggLightStatus = 1;}
else {eggLightStatus = 0;}

    if (debug) Serial.print("eggLightStatus: ");
    if (debug) Serial.println(eggLightStatus);
}


void eggLightDimmer (){
  if (eggLightValueMax == 0) eggLightValueMax = 1;
intervalTimeEggLight = eggLightDimTime/eggLightValueMax;
}


void eggLight () {
  if (millis() - intervalMillisEggLight > intervalTimeEggLight) {
  if (eggLightStatus == 1 && eggLightValue < eggLightValueMax) {
    eggLightValue = eggLightValue + 1;
    if (eggLightValue >= eggLightValueMax) {
      eggLightValue = eggLightValueMax;
      //eggLightStatus = 0;
      }

    analogWrite(eggLightChannel, eggLightValue);
    if (debug) Serial.print("---eggLightValue: ");
    if (debug) Serial.println(eggLightValue);
  }
  if (eggLightStatus == 0 && eggLightValue > 0) {
    eggLightValue = eggLightValue - 1;
    if (eggLightValue < 1){
      analogWrite(eggLightChannel, LOW);
      eggLightValue = 0;
      //eggLightStatus = 1;
    }
    else analogWrite(eggLightChannel, eggLightValue);
    if (debug) Serial.print("---eggLightValue: ");
    if (debug) Serial.println(eggLightValue);
  }
  intervalMillisEggLight = millis();
  }
}


void sleep(){
if (eggLightValue == 0 && eggLightStatus == 0){
  delay(200);

    int sleepcycles = sleepInterval / 8;
    for (int i=0; i<sleepcycles; i++) {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  }
}

void batteryMonitor(){
voltageSensorValue = analogRead(A6); //read the A6 pin value
  voltage = voltageSensorValue * (5.00 / 1023.00) * 7.16; //convert the value to a true voltage.
if (voltageSensorValue < voltageLimit){
voltageShutdownCount = voltageShutdownCount+1;
if (voltageShutdownCount > 1000) voltageShutdownCount = 1000;

}
else {
  voltageShutdownCount = 0;
}


  if (debug) {
    Serial.print("Voltagesensor: ");
    Serial.println(voltageSensorValue);
    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("VoltageShutdownCount: ");
    Serial.println(voltageShutdownCount);

    Serial.println("------------------");
}
}
 

void loop() { 
if(millis() - time_now > loopPeriod){
  time_now = millis();
  if (debug)   Serial.println(" ");
  if (debug)   Serial.println("##################");
  RTCtimer ();
  Sunrise ();

  if (voltageShutdownCount < 10) {  // 320 for lead battery
  eggLightSwitch ();
  }
  else { eggLightStatus = 0;
  eggLightValue = 0;
  analogWrite(eggLightChannel, LOW);
  if (debug) Serial.println ("low Voltage") ;
  }
   batteryMonitor();

  Serial.println();
  sleep();
  }

  eggLight ();

}
  