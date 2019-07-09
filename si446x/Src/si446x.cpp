//Notes:
//1. SPI: 1EDGE, LOW CLOCK

#include "si446x.h"


//Constructor
si446x::si446x(GPIO_TypeDef* pinBank, SPI_HandleTypeDef* spi, uint16_t ss,
		uint16_t sdn, uint16_t irq, uint16_t gpio0, uint16_t gpio1) {
	SPI = spi;
	PIN_BANK = pinBank;
	SS = ss;
	SDN = sdn;
	IRQ = irq;
	GPIO0 = gpio0;
	GPIO1 = gpio1;
}

//Send a package
void si446x::sendPacket(uint8_t * data, uint16_t len) {

	//Clear Fifo
	clearFifoTXRX();
	//Ready State
	uint8_t comBuf[] = {CHANGE_STATE, 0x03}; //Change to ready mode
	sendCommand(comBuf, 2);
	//Clear Interupts
	clearInterupts();
	//Send to fifo
	sendDataToFifo(data, len);
	//Transmitt
	transmitOTA();
}

//Power up and program MCU
void si446x::powerUp() {
	HAL_GPIO_WritePin(PIN_BANK, SDN, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(PIN_BANK, SDN, GPIO_PIN_RESET);
	waitUntilReady();
	writeConfig();
}

//Before every command check if CTS is asserted
void si446x::waitUntilReady() {
	uint8_t comand = CTS;
	uint8_t testByte = 0x00;
	while (testByte != 0xff) {
		HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_RESET);
		HAL_SPI_Transmit(SPI, &comand, 1, spiTimeout);
		HAL_SPI_Receive(SPI, &testByte, 1, spiTimeout);
		HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
	}
}

//Send command and configuration bytes
void si446x::sendCommand(uint8_t* command, uint8_t lenCommand)
{
	waitUntilReady();
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPI, command, lenCommand, spiTimeout);
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
}

//Send a command and config bytes, get replay stream in return
void si446x::sendComandGetResponse(uint8_t* command, uint8_t lenCommand, uint8_t* response, uint8_t lenResponse)
{
	uint8_t cts[] = {CTS};
	uint8_t ff[] = {0x00};
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPI, command, lenCommand, spiTimeout);
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
	while(true)
	{
		HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_RESET);
		HAL_SPI_Transmit(SPI, cts, 1, spiTimeout);
		HAL_SPI_Receive(SPI, ff, 1, spiTimeout);
		if(ff[0] == 0xff)
		{
			HAL_SPI_Receive(SPI, response, lenResponse, spiTimeout);
			HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
			break;
		}
		HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
	}
}

//Send data to Rx FIFO
void si446x::sendDataToFifo(uint8_t *data, uint8_t len) {
	waitUntilReady();
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPI, (uint8_t*) WRITE_TX_FIFO, 1, spiTimeout);
	HAL_SPI_Transmit(SPI, data, len, spiTimeout);
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
}

//Clear all pending interupts
void si446x::clearInterupts() {
	uint8_t comBuf[] = {GET_INT_STATUS, 0x00, 0x00, 0x00};
	sendCommand(comBuf, 4);
}

//Returns current state
uint8_t si446x::getState() //TODO
{
	//TODO
	return 0xff;
}

//Transmits TX FIFO Content over the air
void si446x::transmitOTA() {
	uint8_t comBuf[] = {START_TX, 0x01, 0x80, 0x00};
	sendCommand(comBuf, sizeof(comBuf));
	HAL_Delay(500); //TODO Use nIRQ Pin ...
}

//Inits. RX Mode. When one package is received, modem will fall back to ready mode
void si446x::startRX()
{
	//Ready State
	uint8_t comBuf[] = {CHANGE_STATE, 0x03}; //Change to ready mode
	sendCommand(comBuf, 2);
	HAL_Delay(100);
	//Clear Interupts
	clearInterupts();
	//Clear FIFO TX /RX
	clearFifoTXRX();
	//Get Radio into RX Mode
	uint8_t commandRx[] = {START_RX, 0x01, 0x00, 0x00, 0x00, 0x00,0x03,0x00};
	sendCommand(commandRx, sizeof(commandRx));
}

//Returns number of Bytes in RX FIFO
uint8_t si446x::getRxFifoInfo()
{
	uint8_t comBuf[2] = {0x15, 0x00};
	uint8_t response[3] = {0x00,0x00,0x00};
	sendComandGetResponse(comBuf, 2, response, 2);
	return(response[0]);
}

//Clear RX and TX FIFO
void si446x::clearFifoTXRX() {
	uint8_t comBuf[] = { FIFO_INFO, 0x03};
	sendCommand(comBuf, sizeof(comBuf));
}

//Gets Packet out of FIFO
void si446x::getPacket(uint8_t* rxData, uint8_t len)
{
	getDataFromFifo(rxData, len); //TODO get lenght
}

//Reads len Bytes from RX FIFO
void si446x::getDataFromFifo(uint8_t *data, uint16_t len) {
	waitUntilReady();
	uint8_t comand[] = {READ_RX_FIFO};
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPI, comand, 1, spiTimeout);
	HAL_SPI_Receive(SPI, data, len, spiTimeout);
	HAL_GPIO_WritePin(PIN_BANK, SS, GPIO_PIN_SET);
}

//Writes the configs from the radio_config_Si4463.h to the radio
//TODO include patch?
void si446x::writeConfig() {
	uint8_t buf[1]= {0x00};
	uint8_t configArray[] = RADIO_CONFIGURATION_DATA_ARRAY;
	uint8_t len = configArray[0];
	int arrCounter = 1;
	uint8_t configBuffer[128] = { 0x00 };
	while (len != 0) {
		for (int i = 0; i < len; i++) {
			configBuffer[i] = configArray[arrCounter];
			arrCounter++;
		}
		sendComandGetResponse(configBuffer, len, buf, 0);
		//printHexArray(configBuffer, len); //DEBUG OUTPUT
		len = configArray[arrCounter];
		arrCounter++;
	}
}
