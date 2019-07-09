#include "si446x.h"

si446x::si446x(int ss, int sdn, int irq, int gpio0, int gpio1)
{
  SS=ss; 
  SDN=sdn; 
  IRQ=irq; 
  GPIO0=gpio0; 
  GPIO1=gpio1;
}

void si446x::sendPacket(byte* data, int len)
{
    clearFifoTXRX();
    setReadyState();
    clearInterupts();
    sendDataToFifo(data, len);
    transmitOTA();
}

//Power up and program MCU
void si446x::powerUp()
{
  pinMode(SDN, OUTPUT);
  pinMode(IRQ, INPUT);
  pinMode(GPIO0, INPUT);
  pinMode(GPIO1, INPUT);
  pinMode(SS, OUTPUT);
  SPI.begin();
  digitalWrite(SDN, HIGH);
  delay(10);
  digitalWrite(SDN, LOW);
  writeConfig();
}

//Before every command check if MCU is ready
void si446x::waitUntilReady(void)
{
  byte buf = 0x00;
  while(buf != 0xff)
  {
    digitalWrite(SS, LOW);
    SPI.transfer(CTS);
    buf = SPI.transfer(0x00);
    digitalWrite(SS, HIGH);
  }
}

//Read a register (NOT A PROPERTY!!!)
void si446x::requestRegister(byte command, byte *buf, int len)
{
  sendDataToMcu((byte*) command, 1);
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(CTS); //You have to send CTS to get data!!!
  SPI.transfer(0x00); //First response has to be 0xff (checked before)
  for(int i=0; i<len; i++) //Get all the data reqiered
  {
    buf[i] = SPI.transfer(0x00);
  }
  digitalWrite(SS, HIGH);
}

void si446x::clearFifoTXRX()
{
  byte comBuf[] = {FIFO_INFO, 0x03}; //Clear both Fifos
  sendDataToMcu(comBuf, sizeof(comBuf));
}

void si446x::sendDataToFifo(byte *data, int len)
{
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(WRITE_TX_FIFO); //Write to fifo command
  SPI.transfer(len, 1);
  SPI.transfer(data, len);  
  digitalWrite(SS, HIGH);
}

void si446x::clearInterupts()
{
  byte comBuf[] = {GET_INT_STATUS, 0x00, 0x00, 0xff}; 
  sendDataToMcu(comBuf, sizeof(comBuf));
}

void si446x::setReadyState()
{
  byte comBuf[] = {CHANGE_STATE, 0x03}; //Change to ready mode
  sendDataToMcu(comBuf, 2);
}

byte si446x::getState() //TODO
{
  //TODO
  return 0xff;
}

void si446x::transmitOTA()
{
  byte comBuf[] = {START_TX, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00}; //TODO 0b00100000 makes radio fall into spi state when transmission is complete... Maybe more modes?
  sendDataToMcu(comBuf, sizeof(comBuf));
  waitUntilReady();
}

void si446x::setRX()
{
  setReadyState();
  si446x::clearInterupts();
  clearFifoTXRX();
  uint8_t comandBuf[] = {START_RX, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00};
  sendDataToMcu(comandBuf, sizeof(comandBuf));
}

void si446x::getPackage(uint8_t* buf, uint8_t len)
{
  getDataFromFifo(buf, len);
}


void si446x::getDataFromFifo(byte *data, int len)
{
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(READ_RX_FIFO);
  for(int i=0; i<len; i++)
  {
    data[i] = SPI.transfer(0x00); 
  } 
  digitalWrite(SS, HIGH);
}

//Send Raw string to MCU
void si446x::sendDataToMcu(byte* data, int len)
{
  waitUntilReady();
  digitalWrite(SS, LOW);
  SPI.transfer(data, len); 
  digitalWrite(SS, HIGH);
}


//Print an array in formated hex
//Used for Debug only
void si446x::printHexArray(byte* data, int len)
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
void si446x::writeConfig()
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
