#include "rgbpanelpins.h"
#include <t3spi.h>

//Initialize T3SPI class as SPI_SLAVE
T3SPI SPI_SLAVE;

#define dataLength  256
int dmdbrightness = 128;

//Initialize the arrays for incoming data
volatile uint16_t data0[dataLength] = {};
volatile uint16_t data1[dataLength] = {};
volatile uint16_t data2[dataLength] = {};

byte one2three = 0;
int bitspercolour = 4;
int framecount=0;

void setup() {
  pinMode( LATCH, OUTPUT);
  pinMode( OE, OUTPUT);
  pinMode( CLOCK, OUTPUT);
  pinMode( BLUE0, OUTPUT);
  pinMode( RED0, OUTPUT);
  pinMode( RED1, OUTPUT);
  pinMode(GREEN0, OUTPUT);
  pinMode( GREEN1, OUTPUT);
  pinMode( BLUE1, OUTPUT);
  pinMode( A_0, OUTPUT);
  pinMode( B_1, OUTPUT);
  pinMode( C_2, OUTPUT);
  pinMode( D_3, OUTPUT);

  pinMode( DMD_ROWDATA, INPUT);
  pinMode( DMD_ROWCLK, INPUT);
  pinMode( DMD_LATCH, INPUT);
  pinMode(DMD_OE, INPUT);



  //Begin SPI in SLAVE (SCK pin, MOSI pin, MISO pin, CS pin)
  while (!digitalReadFast(DMD_ROWDATA)) {}
  SPI_SLAVE.begin_SLAVE(SCK, MOSI, MISO, CS0);
  SPI_SLAVE.dataPointer = 0;
  //Set the CTAR0_SLAVE0 (Frame Size, SPI Mode)
  SPI_SLAVE.setCTAR_SLAVE(16, SPI_MODE0);
  //Enable the SPI0 Interrupt
  NVIC_ENABLE_IRQ(IRQ_SPI0);
 // NVIC_SET_PRIORITY(IRQ_SPI0,32);
  attachInterrupt(digitalPinToInterrupt(DMD_OE), dmd_framestart, RISING);

}

void loop() {

 
 for (int a = 0; a < 16; a++) {
    updaterow(a);
                              }
            }

//Interrupt Service Routine to handle incoming data
void spi0_isr(void) {
  if (SPI_SLAVE.dataPointer == 0) {
    one2three++;
    one2three &= 3;
                                  }
  if (one2three == 0)
    SPI_SLAVE.rx16(data0, dataLength);
  if (one2three == 1)
    SPI_SLAVE.rx16(data1, dataLength);
  if (one2three == 2)
    SPI_SLAVE.rx16(data2, dataLength);
   
                    }
void dmd_framestart(void) {
      if(digitalReadFast(DMD_ROWDATA))
     
      SPI_SLAVE.dataPointer =0;
      }

void updaterow(int j) {
  int k;
  byte byte2write = 0;
  int oelatch = 8192;
  byte abcd = 0;

  byte redshade0 = 11;
  byte greenshade0 = 11;
  byte blueshade0 = 11;

  byte redshade1 = 4;
  byte greenshade1 = 4;
  byte blueshade1 = 4;

  byte redshade2 = 0;
  byte greenshade2 = 0;
  byte blueshade2 = 15;
 
  byte rs0, rs1, rs2;
  byte gs0, gs1, gs2;
  byte bs0, bs1, bs2;

  byte toppixel, bottompixel,overlaytoppixel,overlaybottompixel;
  k = j * 8;
  oelatch = 8192; //just oe high
  GPIOA_PDOR = oelatch;

  abcd = j >> 1;
  abcd |= (j & B00000001) << 3; //ecurtz' s pinout 3012
  GPIOC_PDOR = abcd;          //set abcd

 

  for (int shaded = 0; shaded < bitspercolour; shaded++) {
    rs0 = redshade0 >> shaded;
    gs0 = greenshade0 >> shaded;
    bs0 = blueshade0 >> shaded;
    rs1 = redshade1 >> shaded;
    gs1 = greenshade1 >> shaded;
    bs1 = blueshade1 >> shaded;
    rs2 = redshade2 >> shaded;
    gs2 = greenshade2 >> shaded;
    bs2 = blueshade2 >> shaded;


    for (int o = 0; o < 8; o++) {
      for (int i = 0; i < 16; i++) {
        byte2write = 0;
        toppixel = ((data0[k + o] >> (15 - i)) & 1) + ((data1[k + o] >> (15 - i)) & 1) + ((data2[k + o] >> (15 - i)) & 1); // add data0 data1 data2 together bit for bit for the top half of the frame
        bottompixel = ((data0[128 + k + o] >> (15 - i)) & 1) + ((data1[128 + k + o] >> (15 - i)) & 1) + ((data2[128 + k + o] >> (15 - i)) & 1); //add data0 data1 data2 together bit for bit for the bottom half
       
        if (toppixel == 3) {
          if (rs2 & 1)
            byte2write |= 1 << 7; //RED0
          if (gs2 & 1)
            byte2write |= 1 << 0; //GREEN0
          if (bs2 & 1)
            byte2write |= 1 << 4; //BLUE0
        }

        if (bottompixel == 3) {
          if (rs2 & 1)
            byte2write |= 1 << 2; //RED1
          if (gs2 & 1)
            byte2write |= 1 << 6; //GREEN1
          if (bs2 & 1)
            byte2write |= 1 << 5; //BLUE1
        }

        if (toppixel == 2) {
          if (rs1 & 1)
            byte2write |= 1 << 7; //RED0
          if (gs1 & 1)
            byte2write |= 1 << 0; //GREEN0
          if (bs1 & 1)
            byte2write |= 1 << 4; //BLUE0
        }

        if (bottompixel == 2) {
          if (rs1 & 1)
            byte2write |= 1 << 2; //RED1
          if (gs1 & 1)
            byte2write |= 1 << 6; //GREEN1
          if (bs1 & 1)
            byte2write |= 1 << 5; //BLUE1
        }
        if (toppixel == 1) {
          if (rs0 & 1)
            byte2write |= 1 << 7; //RED0
          if (gs0 & 1)
            byte2write |= 1 << 0; //GREEN0
          if (bs0 & 1)
            byte2write |= 1 << 4; //BLUE0
        }

        if (bottompixel == 1) {
          if (rs0 & 1)
            byte2write |= 1 << 2; //RED1
          if (gs0 & 1)
            byte2write |= 1 << 6; //GREEN1
          if (bs0 & 1)
            byte2write |= 1 << 5; //BLUE1
        }

        GPIOD_PDOR = byte2write;

        byte2write |= 1 << 1;
        GPIOD_PDOR = byte2write;
      }
    }


    oelatch = 12288; //latch and oe high
    GPIOA_PDOR = oelatch;


    digitalWriteFast(LATCH, LOW); //is slower sometimes slow is good
    oelatch = 0; //latch and oe low
    GPIOA_PDOR = oelatch;
    for (long int waitstate = 0; waitstate < (dmdbrightness << shaded); waitstate++) {
      long int dostuff = waitstate;
                                                                                      }


    oelatch = 8192; //just oe high
    GPIOA_PDOR = oelatch;
  }//shaded loop
}


