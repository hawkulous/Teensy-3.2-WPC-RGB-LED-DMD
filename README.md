# Teensy-3.2-WPC-RGB-LED-DMD
Very basic project to display WPC DMD images on RGB LED matrix with Teensy 3.2 in 12bit colour.
All the relevant connections can be deduced from the header file. 
The panels I used for the project is GRB, just edit the header file to change to RGB or other.
The program uses t3spi library. https://github.com/btmcmahan/Teensy-3.0-SPI-Master---Slave
Replace the relevant Functions in  t3spi.cpp with the following.

//TRANSMIT & RECEIVE PACKET OF 16 BIT DATA
void T3SPI::txrx16(volatile uint16_t *dataOUT, volatile uint16_t *dataIN, int length, bool CTARn, uint8_t PCS){ 
	ctar=CTARn;
	for (int i=0; i < length; i++){
		SPI0_MCR |= SPI_MCR_CLR_RXF;
		SPI_WRITE_16(dataOUT[i], CTARn, PCS);
		SPI_WAIT();
		delayMicroseconds(1);
		dataIN[i]=SPI0_POPR;
		}
	packetCT++;
}

void T3SPI::rx8(volatile uint8_t *dataIN, int length){
	dataIN[dataPointer] = SPI0_POPR;
	dataPointer++;
	if (dataPointer == length){
		dataPointer=0;
		packetCT++;}
	SPI0_SR |= SPI_SR_RFDF;
}

void T3SPI::rx16(volatile uint16_t *dataIN, int length){
	dataIN[dataPointer] = SPI0_POPR;
	dataPointer++;
	if (dataPointer == length){
           
		dataPointer=0;
		packetCT++;}
	SPI0_SR |= SPI_SR_RFDF;
}

//Special rx16 for pinball program
