//Written by Andreas Kellermann, 2019
//
//
//

#include "si446x.h"
si446x rf(10, 5, 3, 14, 4);


void setup() {
  /*
  digitalWrite(SS, HIGH);
  SPI.begin();
  //SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  

  pinMode(SDN, OUTPUT);
  pinMode(GPIO0, INPUT);
  pinMode(SS, OUTPUT);

  //Activate Si4463
  digitalWrite(SDN, HIGH);
  delay(500);
  digitalWrite(SDN, LOW);
  delay(100); //Power up delay

  writeConfig();
  */
  
  Serial.begin(115200);
  rf.powerUp();
  
}

void loop() {
  //byte buf[30] = {0x99};
  //rf.sendPacket(buf, sizeof(buf));
  //delay(25);

  
  

  /*
  //SEND
  //Load and send example package
  clearFifo();
  setReadyState();
  clearInterupts();
  sendDataToFifo(buf, 60);
  transmitFifo();
  */
  
  
  //RECEIVE
  rf.setRX();
  delay(1000);
  byte data[60] = {0x00};
  rf.getPackage(data, 10);
  for(int i = 0; i< 60; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  delay(500);
}

/*
//Before every command check if MCU is ready
void waitUntilReady(void)
{
  byte buf = 0x44;
  while(buf != 0xff)
  {
    digitalWrite(SS, LOW);
    SPI.transfer(0x44);
    buf = SPI.transfer(0x00);
    digitalWrite(SS, HIGH);
  }
}

//Read a register (NOT A PROPERTY!!!)
void requestRegister(byte command, byte *buf, int len)
{
  sendDataToMcu((byte*) command, 1);
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(0x44); //You have to send 0x44 to get data!!!
  SPI.transfer(0x00); //First response has to be 0xff (checked before)
  for(int i=0; i<len; i++) //Get all the data reqiered
  {
    buf[i] = SPI.transfer(0xff);
  }
  digitalWrite(SS, HIGH);
}

void clearInterupts()
{
  byte comBuf[] = {0x15, 0x03}; 
  sendDataToMcu(comBuf, sizeof(comBuf));
}

void sendDataToFifo(byte *data, int len)
{
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(0x66); //Write to fifo command
  SPI.transfer(len, 1);
  SPI.transfer(data, len);  
  digitalWrite(SS, HIGH);
}

void clearFifo()
{
  byte comBuf[] = {0x34, 0x03}; 
  sendDataToMcu(comBuf, sizeof(comBuf));
}

void setReadyState()
{
  byte comBuf[] = {0x15, 0x03}; 
  sendDataToMcu(comBuf, 2);
}

byte getState()

void transmitFifo()
{
  byte comBuf[] = {0x31, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00}; 
  sendDataToMcu(comBuf, sizeof(comBuf));
  //delay(50); //TODO Delay Needed??
  waitUntilReady();
}

void receiveFifo()
{
  waitUntilReady();
  sendDataToMcu((byte*) 0x32, 1);
  digitalWrite(SS, LOW);
  delay(1000); //TODO better interupt function here... nIRQ Pin???
}

void getDataFromFifo(byte *data, int len)
{
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(0x77); //Write to fifo command
  for(int i=0; i<len; i++)
  {
    data[i] = SPI.transfer(0x00); 
  } 
  digitalWrite(SS, HIGH);
}

//Send Raw string to MCU
void sendDataToMcu(byte* data, int len)
{
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(data, len); 
  digitalWrite(SS, HIGH);
}


//Print an array in formated hex
//Used for Debug only
void printHexArray(byte* data, int len)
{
  for(int i=0; i<len; i++)
  {
    Serial.print(data[i],HEX);
    Serial.print(' ');
  }
  Serial.println();
}

//Will write the configs from the radio_config_Si4463.h to the radio
//TODO include patch?
void writeConfig()
{
  byte configArray[] = RADIO_CONFIGURATION_DATA_ARRAY;
  byte len = configArray[0];
  int arrCounter = 1;
  byte configBuffer[128] = {0x00};
  while(len != 0)
  {
    for(int i=0; i<len; i++)
    {
      configBuffer[i] = configArray[arrCounter];
      arrCounter++;
    }
    sendDataToMcu(configBuffer, len); //Send next config array to si
    //printHexArray(configBuffer, len); //DEBUG OUTPUT
    len = configArray[arrCounter];
    arrCounter++;
  }
}
*/
