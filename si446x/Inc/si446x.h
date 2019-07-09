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

//#include "Arduino.h"
//#include "SPI.h"

#include "stm32f0xx_hal.h"


class si446x
{
  public:
	si446x(GPIO_TypeDef* pinBank, SPI_HandleTypeDef* spi, uint16_t ss, uint16_t sdn, uint16_t irq, uint16_t gpio0, uint16_t gpio1);
    void sendPacket(uint8_t* data, uint16_t len, uint8_t channel);
    void getPacket(uint8_t* rxData, uint8_t len);
    void powerUp();
    void setState(uint8_t state);
    uint8_t getState();
    void startRX(uint8_t channel);
    uint8_t getRxFifoInfo();
    
  private:
    //Variables
    uint16_t SS;
    uint16_t SDN;
    uint16_t IRQ;
    uint16_t GPIO0;
    uint16_t GPIO1;
    SPI_HandleTypeDef* SPI;
    GPIO_TypeDef* PIN_BANK;
    uint16_t spiTimeout = 1000;

    void clearInterupts();
    void waitUntilReady();
    void sendComandGetResponse(uint8_t* command, uint8_t lenCommand, uint8_t* response, uint8_t lenResponse);
    void printHexArray(uint8_t* data, uint16_t  len);
    void writeConfig();
    void sendDataToFifo(uint8_t *data, uint8_t  len);
    void transmitOTA(uint8_t channel);
    void getDataFromFifo(uint8_t *data, uint16_t  len); //TODO: Check if working
    void clearFifoTXRX();
    void sendCommand(uint8_t* command, uint8_t lenCommand);
    void setReadyState();
};

#endif
