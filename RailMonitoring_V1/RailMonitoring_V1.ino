#include "DHT.h"

DHT dht;

//Ultrasound Sensor
int trig = 2;
int echo = 3;

//DHT pin
int pinDHT = 5;
/* Vitesse du son dans l'air en mm/us */
const float SOUND_SPEED = 340.0 / 1000;

void setup()
{
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);
  dht.setup(pinDHT);
  Serial.begin(9600);
}

void loop()
{
  float cm = getDistanceFromUltraSound();
  float tmp = getTemperatureFromDHT();
  Serial.print("Distance: ");
  Serial.print(cm, 2);
  Serial.print("mm, température: ");
  Serial.println(tmp);
  delay(1000);
}


float getDistanceFromUltraSound(){
  long lecture_echo;
  float mm;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  lecture_echo = pulseIn(echo, HIGH);
  mm = lecture_echo /  2.0 * SOUND_SPEED;
  return mm;
}

float getTemperatureFromDHT(){
   delay(dht.getMinimumSamplingPeriod());
   float temperature = dht.getTemperature();
   return temperature;
}

