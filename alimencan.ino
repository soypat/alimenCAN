// RUN BLYNK! C:\Program Files (x86)\Arduino\libraries\Blynk\scripts\blynk-ser.bat 
const char statLlenandoTarroAgua[] = "Llenando Agua  ";
const char welcomeMessage[] = "AlimenCAN    Com";

#define pinTrigCom A0 //Definimos pines analogos 
#define pinEchoCom A1
#define pinEchoAgua A2
#define pinTrigAgua A3
//RTC(tachos)
#define pinTarroCom A5
#define pinTarroAgua A4
const int pinEV = 6;
const int pinPulsador = 7;


int rC = 0, rA = 0, tC = 0, tA = 0;
bool openingServo=false;
long  pC = 0, pA = 0;//niveles de reservorio y tarros

unsigned long printtime, servoTime, evTime, pressTime,plevelTime;


#define BLYNK_PRINT DebugSerial
#include <SoftwareSerial.h>
SoftwareSerial DebugSerial(2, 3); // RX, TX
#include <BlynkSimpleStream.h>
#define PIN_UPTIME V5
#define PIN_SERVO V1
BlynkTimer timer;
BLYNK_READ(PIN_UPTIME);
BLYNK_READ(PIN_SERVO);
char auth[] = "9f66febeb73a41bca21e3be3e3c6b4c0";
void nivel_tarroC()
{
//  int sensorData;
//  sensorData = analogRead(A0); //reading the sensor on A0
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5,pC);
}
void nivel_tarroA()
{
  Blynk.virtualWrite(V4,pA);
}
int value=-100;
void debug_value()
{
  Blynk.virtualWrite(V3,value);
}

//void opener()
//{
//   Blynk.virtualWrite(V3,pA);
//   openingServo=V1;
//}
BLYNK_WRITE(V1) // Button Widget writes to Virtual Pin V5 
{
  if(param.asInt() == 1) {     // if Button sends 1
    openServo(); 
    servoTime=millis();// start the function
  }
}
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5 , 4, 3, 2);
BLYNK_WRITE(V2) // Button Widget writes to Virtual Pin V5 
{
  if(param.asInt() == 1) {     // if Button sends 1
    
    fill(); 
    evTime=millis();// start the function
    lcd_deploy();
//    lcd.begin(16,2);
//    lcd.print(welcomeMessage);
  }
}
// BLYNKBLYNKBLYNKBLYNKBLYNKBLYNKBLYNK


#include <Servo.h>
#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;
//#include <CurieTime.h>

// RESERVORIOS(ultrasound)


const unsigned long maxEvOpenTime = 4000;
const unsigned long minPressTime= 333;


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

const unsigned long minDispTime = 2100;


Servo servo_13; //pin del servo
const unsigned long posCerrada = 90, posAbierta = 10, maxOpenTime = 4000;

const int maxMedicionAgua = 28;

boolean buttonPress = true;
boolean evON=false;



int comidasPorDia = 1;


long sumAgua = 0, sumCom = 0;
int Nagua = 0, Ncom = 0, Nnominal = 20;



void setup()
{
//  setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
  lcd.begin(16, 2);
  lcd.print(welcomeMessage);
  print_Status();
  // Debug console
  DebugSerial.begin(9600);
  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  timer.setInterval(500L, nivel_tarroA);
  timer.setInterval(500L, nivel_tarroC);
  timer.setInterval(150L, debug_value);
  servo_13.attach(13);
  pinMode(pinTrigCom, OUTPUT);
  pinMode(pinEchoCom, INPUT);
  pinMode(pinTrigAgua, OUTPUT);
  pinMode(pinEchoAgua, INPUT);
  pinMode(pinTarroAgua, INPUT); //t2
  pinMode(pinTarroCom, INPUT); //t1
  pinMode(pinEV, OUTPUT);
  pinMode(pinPulsador, INPUT);
  
}

void loop()
{
  int sensorData;
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
    if ((buttonPress == false) && (digitalRead(pinPulsador) == true) && (millis() - pressTime) > minPressTime) {
    pressTime = millis();
    buttonPress = true;
//    DateTime now = rtc.now();
//    value=now.minute();
  }
    if ( (millis() - printtime) > minDispTime || buttonPress == true) {
    print_Status();
//    value=2;
  }
//  Serial.println(pC);
//  delay(1000);
  pC =(1.0-nivel(pinTrigCom, pinEchoCom)/33.0)*100.0;
  pA =(1.0-nivel(pinTrigAgua, pinEchoAgua)/28.0)*100.0;

  if (millis()-servoTime>2000){closeServo();}
  if (evON and millis()-evTime>1400) {
    digitalWrite(pinEV,LOW);
    lcd_deploy();
    evON=false;
  }

}

long nivel(int pinTrig, int pinEcho) //rutina del ultrasonido
{
  long centimetros, porcentaje;
  pinMode(pinTrig, OUTPUT);  // Clear the trigger
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  pinMode(pinTrig, INPUT);
  // Reads the pin, and returns the sound wave travel time in microseconds
  centimetros = 0.01723 * pulseIn(pinEcho, HIGH);
  return centimetros;
}

//FUNCIONES MAQUINA ALIMENcan

void openServo() {
  servo_13.write(posAbierta);
  servoTime = millis();
}

void closeServo() {
  servo_13.write(posCerrada);

}

void fill() {
  evTime = millis();
  digitalWrite(pinEV, HIGH);
  evON=true;
}
void lcd_deploy() {
  lcd.begin(16,2);
  lcd.print(welcomeMessage);
  print_Status();
}

void print_Status() {
//  time_t t = now(); 
//  DateTime now = now();
//  printtime = millis();
//  lcd.setCursor(0, 1);
//  lcd.print("");
//  lcd.print(hour(), DEC);
//  lcd.print(':');
//  if (minute() < 10) {
//    lcd.print("0");
//    lcd.print(minute(), DEC);
//  } else {
//    lcd.print(minute(), DEC);
//  }
//  //    lcd.print(now.minute(), DEC);
//  lcd.print(':');
//  if (now.second() < 10) {
//    lcd.print("0");
//    lcd.print(second(), DEC);
//  } else {
//    lcd.print(second(), DEC);
//  }
  if (buttonPress == true) {

    value=comidasPorDia;
    lcd.setCursor(14, 1);
    buttonPress = false;
    lcd.print("");
    lcd.print(comidasPorDia);
    comidasPorDia = comidasPorDia + 1;
    if (comidasPorDia > 5) {
      comidasPorDia = 1;
    }
  }
}
