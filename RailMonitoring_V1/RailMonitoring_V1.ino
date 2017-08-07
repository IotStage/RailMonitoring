#include "DHT.h"
#include <SPI.h>
#include <RH_RF95.h>
#include "encrypt.h"

#define ADD 1
#define TYPE 0


DHT dht;

RH_RF95 rf95;
float frequency = 868.0;
ENCRYPT encrypt_decrypt;
unsigned char encryptkey[16]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};


//Ultrasound Sensor
int trig = 3;
int echo = 4;

//DHT pin
int pinDHT = 5;
/* Vitesse du son dans l'air en mm/us */
const float SOUND_SPEED = 340.0 / 1000;

void setup()
{ 
  Serial.begin(9600);
  initRF();
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);
  dht.setup(pinDHT);
  
 
}

void loop()
{
String trame = getTrame();
//Serial.println(trame);
sendPacket(trame);
  delay(1000);
}

void initRF(){
  Serial.println(F("Debut initialisation RF"));
  if (!rf95.init())
    Serial.println("init failed");
   // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  Serial.println("Configuration terminee");
}

void sendPacket(String paquet){
  int len = paquet.length()+1;
  char buff[len];
  //Serial.println(paquet);
  paquet.toCharArray(buff, len);
  encrypt_decrypt.btea(buff, len, encryptkey);
  //Serial.println("chiffrement");
  rf95.send(buff, len);
  Serial.println("donnees envoyees");
  
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

String getTrame(){
  float mm= getDistanceFromUltraSound();
  float ma = getTemperatureFromDHT();
  String trame="";
  trame+=TYPE;
  trame+=":";
  trame+=ADD;
  trame+=":";
  trame+=String(mm, 2);
  trame+=":";
  trame+=String(ma, 2);
  Serial.print("la trame: ");
  Serial.println(trame);
  return trame;
}

