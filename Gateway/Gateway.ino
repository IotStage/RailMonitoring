#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines. 
//#define BAUDRATE 250000


#include <Bridge.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <HttpClient.h>
#include <Console.h>
#include <String.h>
#include "encrypt.h"

HttpClient client;
#define ADDRESSE_GATEWAY 4
#define ADDRESSE_RELAI 9
#define INFO 0
#define CTL 1
#define SUP 0
#define CMD 1

String ADDRESSE_SERVER ="10.130.1.200";
String PORT_SERVER ="8888";

//int HEART_LED=A2;

int e;
String paquet="";

RH_RF95 rf95;
ENCRYPT encrypt_decrypt;
unsigned char encryptkey[16]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};


int led = A2;
float frequency = 868.0;

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

void loop(void)
{
  
 listenMsgEntrant();
 //listenMsgSortant();
}

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

void listenMsgEntrant(void){
  if (rf95.waitAvailableTimeout(100))
  {
    clignoterLED();
    char buf[RH_RF95_MAX_MESSAGE_LEN];//buffer to store the server response message
    uint8_t len = sizeof(buf);// data length
    if (rf95.recv(buf, &len))//check if receive data is correct 
    {    
      Console.println("un nouveau paquet");
      Console.println(buf);
      clignoterLED();
      Console.println("decryptage du message ");          
      encrypt_decrypt.btea(buf, -len, encryptkey);
      //Console.println(buf);
      String donnees="";
      for(int i=0; i<len-1; i++){
        donnees+=(char)buf[i]; 
      }
      saveData(donnees);
      Console.println(donnees);
      Console.print("taille des donnees");
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



