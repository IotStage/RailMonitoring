#define BAUDRATE 115200 //Baudrate to use to communicate with dragino box

//If you use Dragino Yun Mesh Firmware , uncomment below lines. 
//#define BAUDRATE 250000


#include <Bridge.h> //library to use to communicate with dragino box
#include <SPI.h> //SPI is a library that is needed to make the Rf transmitter work
#include <RH_RF95.h> //Library for Lora
#include <HttpClient.h> //Library to send data to an Url
#include <Console.h> //library to use to communicate with dragino box
//#include <String.h>
#include "encrypt.h" //library to make the encryption of the data

HttpClient client;
//declaration of the variable for http client
#

String ADDRESSE_SERVER ="10.130.1.200";
String PORT_SERVER ="8888";

//int HEART_LED=A2;

int e;
String paquet="";

RH_RF95 rf95; //declaration of the variable for Lora
ENCRYPT encrypt_decrypt; //declaration of the variable for the encryption
unsigned char encryptkey[16]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};


int led = A2; //led to use to show data is received
float frequency = 868.0; //Frequency that is used for Lora

//This is the part of the code whe we do all the setup it is done once by the micro controller

void setup()
{
  // Open //Console communications and wait for port to open:
  ////Console.begin(115200);
  pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  Console.begin();
  
  while (!Console) ; // Wait for //Console port to be available
  Console.println("Start Sketch");
  if (!rf95.init())
    //Console.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  clignoterLED();
  Console.print("Configuration terminee");
}
// the loop is what will be executed by the micro controller continously

void loop(void)
{
  
 listenMsgEntrant();
 //listenMsgSortant();
}
//function to put data in the url
void saveData(String sensor){
  String url="";
  url="http://"+ADDRESSE_SERVER+":"+PORT_SERVER+"/railMonitoring/pushDatas/";
  url+=sensor;
  Console.print("url=");
  Console.println(url);
  long debut = millis();
 client.get(url);

  while (client.available()) {
    char c = client.read();
    Console.print(c);
  }
  Console.print("send data done. ");
  Console.println(millis() - debut);
  Console.println("secondes");
  Console.flush();
}

//Function to receive data from Lora
void listenMsgEntrant(void){
  if (rf95.waitAvailableTimeout(100)) //Lora waiting for data every 100ms
  {
    clignoterLED();
    char buf[RH_RF95_MAX_MESSAGE_LEN];//buffer to store message
    uint8_t len = sizeof(buf);// data length
    if (rf95.recv(buf, &len))//check if receive data is correct 
    {    
      Console.println("A new Packet has been received");
      Console.println(buf);
      clignoterLED();
      Console.println("decrypt message ");          
      encrypt_decrypt.btea(buf, -len, encryptkey);
      //Console.println(buf);
      String donnees="";
      for(int i=0; i<len-1; i++){
        donnees+=(char)buf[i]; 
      }
      saveData(donnees); //call the function to save data
      Console.println(donnees);
      Console.print("data size");
      Console.println(len);
    }
  }
}

void clignoterLED(){
  digitalWrite(led, HIGH);
  delay(50);
  digitalWrite(led, LOW);
  delay(50);
}



