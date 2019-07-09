//Si446x Library
//Andreas Kellermann, 2019
//

#ifndef SI443XX_H_
#define SI443XX_H_

//Radio Config file generated with the Silicon Labs WDS Tool
//Radio Config file has to be slightly modified
//1. Comment out the include line of patch.h
//2. Comment out the first line of the configuration array (with the patch)
//3. I have not figured out how to load the patch.h file so boot up without patch, replace eg: "#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0x8C, 0xBA, 0x80" the "0x81=0b10000001" with 0x01 and diesable patch loading
#include "radio_config.h" 

//Register Definitions
#include "si446x_def.h"

#include "Arduino.h"
#include "SPI.h"


class si446x
{
  public: 
    
    si446x(int ss, int sdn, int irq, int gpio0, int gpio1);
    void sendPacket(byte* data, int len);
    void powerUp();
    void setState(byte state);
    byte getState();
    void setReadyState();
    void getPackage(uint8_t* buf, uint8_t len);
    void setRX();
    
  private:
    //Variables
    int SS;
    int SDN;
    int IRQ;
    int GPIO0;
    int GPIO1;

    //Functions
    void clearInterupts();
    void waitUntilReady(void);
    void printHexArray(byte* data, int len);
    void sendDataToMcu(byte* data, int len);
    void writeConfig();
    void sendDataToFifo(byte *data, int len);
    void transmitOTA();
    void receiveOTA(); //TODO: Check if working
    void getDataFromFifo(byte *data, int len); //TODO: Check if working   
    void requestRegister(byte command, byte *buf, int len);
    void clearFifoTXRX();
};








#endif
