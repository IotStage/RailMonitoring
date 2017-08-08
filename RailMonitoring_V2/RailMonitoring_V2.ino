#include "DHT.h" // Library that goes with the temperature sensor
#include <SPI.h> //SPI is a library that is needed to make the Rf transmitter work
#include <RH_RF95.h> //Library for Lora
#include "encrypt.h" //library to make the encryption of the data

#define ADD 1 //this is where you specify the address of the station
#define TYPE 0 //by default the type of the frame is 0. It can be 1, 2 or 3 dependending on the conditions


DHT dht; //declaration of the variable for the temperature
RH_RF95 rf95; //declaration of the variable for Lora
float frequency = 868.0; //Frequency that is used for Lora
ENCRYPT encrypt_decrypt; //declaration of the variable for the encryption
//encrypt_key contains the key 
unsigned char encryptkey[16]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};


//Ultrasound Sensor
//We define here the pins on the arduino board for the ultrasound sensor
int trig = 3; 
int echo = 4;


//DHT pin
//
//We define here the pin on the arduino board for the temperature sensor
int pinDHT = 5;
/* Sound speed on air in mm/us */
const float SOUND_SPEED = 340.0 / 1000;

//This is the part of the code whe we do all the setup it is done once by the micro controller
void setup()
{ 
  Serial.begin(9600); //This is made to begin the serial port so that data can be read.
  initRF(); //we call the function that initialize the Lora Module
  //The lines below are made to setup the two sensors
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);
  dht.setup(pinDHT);
  
 
}
// the loop is what will be executed by the micro controller continously
void loop()
{
  int cpt=0;
  while(cpt<30){
String trame = getTrame(); //Here we put the frame on a variable named trame
if(trame[0]!='0'){ //if the frame type is different from '0' 
Serial.println(trame[0]); // We print the frame type
sendPacket(trame);} // and send it to the gateway because there is a problem
 delay(1000);
 cpt++; //the cpt variable is made to have a loop. With cpt going to 30 the loop is made to be 1mn
}
Serial.println("data"); //at the end of the loop we send periodic data.
String trame = getTrame();
sendPacket(trame);
Serial.println(trame[0]);
delay(1000);
}

void initRF(){
  Serial.println(F("Initializing Lora"));
  if (!rf95.init())
    Serial.println("init failed");
   // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  Serial.println("Configuration is done");
}

void sendPacket(String packet){
  int len = packet.length()+1; //size of packet
  char buff[len]; //create buffer 
  packet.toCharArray(buff, len); //store packet in buffer
  encrypt_decrypt.btea(buff, len, encryptkey); //encryption of data
  rf95.send(buff, len); //here we send data
  Serial.println("Data Sent");
  
}
//this function is made to get data from ultra sound sensor
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
//this function is made to get data from temperature sensor
float getTemperatureFromDHT(){
   delay(dht.getMinimumSamplingPeriod());
   float temperature = dht.getTemperature();
   return temperature;
}
//Function to get the frame
String getTrame(){
  int type = TYPE;
  // frame type
  float mm= getDistanceFromUltraSound();
  //store ultra sound data in mm
  float ma = getTemperatureFromDHT();
  //store temperature data in ma
  if(ma>30)
  type=1;
  //test on temperature
  if(mm<400)
  type=2;
  //test on ultrasound
  if(ma<30 && mm<400)
  type=3;
  //test on temperature and ultrasound
  
  String trame="";
  //Frame formation
  trame+=type;
  trame+=":";
  trame+=ADD;
  trame+=":";
  trame+=String(mm, 2);
  trame+=":";
  trame+=String(ma, 2);
  Serial.print("The frame is: ");
  Serial.println(trame);
  return trame;
}

