#include "HardwareProfile.h"

/* This file is specific to the SmartPlug project, but can easily be tailored
   to another project */

// Now tailoring to the Gainspan module (GCM = Gainspan Control Module)
// Spi Channel = 3

#include "spi.h"                     // Pick up SPI APIs
//#include <ports.h>                   // Pick up BIT masks
#include "TimeMonitor.h"
#include "Metrics.h"

void spiConfigurePins(unsigned char spi_channel);
void spiInit(unsigned char spi_channel);
unsigned char spiSendRecvChar(unsigned char spi_channel, unsigned char c);

int spi_channel_in_use = 0xff;

void spiConfigurePins(unsigned char spi_channel)
{
// no pin mapping for PIC32
/*                                                                 */
  switch(spi_channel)
  {
    case 1:
      break;

    case 2:
/*       // SPI2 used as slave (to master device)                  */
/*       //RPINR22bits.SDI2R  =  19; // RP19 is mapped to SDI2     */
/*       //RPINR22bits.SCK2R  =  26; // RP26 is mapped to SCK2     */
/*       //RPINR23bits.SS2R   =  21; // RP21 is slave select       */
/*       //RPOR13bits.RP27R   =  10; // RP27 is mapped to SDO2     */
/*                                                                 */
/*       // SPI2 used as master                                    */
/*       RPINR22bits.SDI2R  =  19;  // RP19/RG8 is mapped to SDI2  */
/*       RPOR13bits.RP27R   =  10;  // RP27/RG9 is mapped to SDO2  */
/*       RPOR13bits.RP26R   =  11;  // RP26/RG7 is mapped to SCK2  */
/*       // do not map CS2 as this is a master device              */
/*       //RPOR10bits.RP21R   =  12;  // RP21/RG6 is slave select  */
/*                                                                 */
      break;

    case 3:
      TRISDCLR = BIT_14;
      break;
  }
}

void spiInit(unsigned char spi_channel)
{
  switch(spi_channel)
  {
    case 1:
      break;

    case 3:
      SPI3BUF = 0;
      SPI3STAT = 0;

      // Baud rate is Fpb / (2 * (SPIxBRG+1)) at 9 bits = max dividor of 1024
// mwn - determine the correct value for this
      SPI3BRG = 5; // 512;

      // SPI3CON - bits
      // 3    2    2    1    1    1    0    0
      // 1    7    3    9    5    1    7    3
      // 000x xxxx xxxx xxxx xxxx xxxx xxxx xxxx - no Framed SPI support
      // xxx0 0000 0000 00xx xxxx xxxx xxxx xxxx - reserved
      // xxxx xxxx xxxx xx0x xxxx xxxx xxxx xxxx - no Framed SPI support
      // xxxx xxxx xxxx xxx0 xxxx xxxx xxxx xxxx - reserved
      // xxxx xxxx xxxx xxxx 1xxx xxxx xxxx xxxx - Enabled
      // xxxx xxxx xxxx xxxx x0xx xxxx xxxx xxxx - No Freeze
      // xxxx xxxx xxxx xxxx xx0x xxxx xxxx xxxx - Continue in idle mode
      // xxxx xxxx xxxx xxxx xxx0 xxxx xxxx xxxx - SDOx control by SPI module
      // xxxx xxxx xxxx xxxx xxxx 00xx xxxx xxxx - 8 bit data width
      // xxxx xxxx xxxx xxxx xxxx xx0x xxxx xxxx - Input sampled at middle
      // xxxx xxxx xxxx xxxx xxxx xxx0 xxxx xxxx - CKE = 0 (clock edge, 0=change from idle to active, 1 = change from active to idle)
      // xxxx xxxx xxxx xxxx xxxx xxxx 0xxx xxxx - SSEN = 0
      // xxxx xxxx xxxx xxxx xxxx xxxx x0xx xxxx - CKP = 0 (clock polarity, 0 = idle low, active high)
      // xxxx xxxx xxxx xxxx xxxx xxxx xx1x xxxx - MSTEN = 1
      // xxxx xxxx xxxx xxxx xxxx xxxx xxx0 xxxx - SDIx control by SPI module
      // xxxx xxxx xxxx xxxx xxxx xxxx xxxx 0000 - used for enhanced buffer mode

      // 0000 0000 0000 0000 1000 0000 0010 0000 = 0x00008020
      SPI3CON = 0x00008020;

      break;

    case 2:
      SPI2BUF = 0;
      SPI2STAT = 0;
      SPI2CON = 0x00008120;         // same as SPI3
      break;

  }
}

void spiEnable(unsigned char spi_channel)
{
  switch(spi_channel)
  {
    case 1:
      break;

    case 2:
    case 3:
      spi_channel_in_use = spi_channel;
      spiConfigurePins(spi_channel);
      spiInit(spi_channel);
      break;
  }
}

unsigned char spiSendRecvChar(unsigned char spi_channel, unsigned char c)
{
  unsigned char spi_stat, rc;
//  char buf[50];
  static int char_count = 0;

  SPI3STATbits.SPIROV = 0;  // clear overflow

  switch(spi_channel)
  {
    case 1:
      break;

    case 2:
      char_count++;
      SPI2BUF = c;

      // wait for unsigned char to transfer
// CAN'T HAVE LOOP MONITORING WITH INTERRUPTS SHUT DOWN
      spi_stat = SPI2STAT;
      while(SPI2STATbits.SPITBF)
      {
        spi_stat = SPI2STAT;
      }

      // wait for receive byte to come back
      spi_stat = SPI2STAT;
      while(!SPI2STATbits.SPIRBF)
      {
        spi_stat = SPI2STAT;
      }
      rc = SPI2BUF;
      return(rc);
      break;

    case 3:
      char_count++;
      SPI3BUF = c;

      // wait for unsigned char to transfer
      spi_stat = SPI3STAT;
      while(SPI3STATbits.SPITBF)
      {
        spi_stat = SPI3STAT;
      }

      // wait for receive byte to come back
      spi_stat = SPI3STAT;
      while(!SPI3STATbits.SPIRBF)
      {
        spi_stat = SPI3STAT;
      }
      rc = SPI3BUF;
      return(rc);
      break;

  }

  return(0);
}

/***********************************************
 * read a meter chip register from a specified address 
 * This is a blcking routine.  No error checking.
 * It will lockup if the SPI is not enabled.
 ************************************************/  
DWORD ReadMeterChip(BYTE addr, BYTE numbits){
  BYTE firstbyte = 0x3f & addr;//force read address
/*   BYTE temp; */
  BYTE numRdBytes;
  BYTE remainder;
  DWORD ReadData=0;
  BYTE count;
//  unsigned long delay=0;

  // Steps to writing data to meter chip
  // 1) Calculate number of bytes to read from chip, partials are rounded up
  // 2) Send mode (0x00 = read) and address bits
  // 3) Send correct number of bytes, while reading back (and throwing out) received data

  TimeMonitorDisable();
  numRdBytes = numbits/8;
  remainder = numbits %8;  //number of bits in partial byte
  if(remainder>0) numRdBytes +=1;

  LATDbits.LATD14 = 0;  // Chip select low
//rwb  GPIO_SETL = BIT(3);// Set *CS High to deselect the meter chip
  //Set *CS low
//rwb  GPIO_CLRL = BIT(3);// Set *CS Low to select the meter chip
  //SPI Send Address
/*   SC2_DATA = firstbyte; //Send mode and address bits                              */
/*   while ((SC2_SPISTAT & SC_SPITXIDLE)!= SC_SPITXIDLE){//wait for byte to transmit */
/*   }                                                                               */
/*   temp = SC2_DATA;  //Read byte from FIFO */
  spiSendRecvChar(spi_channel_in_use, firstbyte);    // ignore return byte

  for(count=0;count<numRdBytes;count++)
  {
    ReadData *= (DWORD)256;
/*       SC2_DATA = 0x00;//Send Dummy byte to get the SPI read                           */
/*       while ((SC2_SPISTAT & SC_SPITXIDLE)!= SC_SPITXIDLE){//wait for byte to transmit */
/*       }                                                                               */
/*       temp = SC2_DATA;  //Read byte from FIFO                                         */
/*       ReadData += (DWORD)temp; */
//    for(delay=0; delay<500000; delay++);      
    ReadData += (DWORD)spiSendRecvChar(spi_channel_in_use, 0xAA);    // send dummy 0x00 
  }
/*   GPIO_SETL = BIT(3);// Set *CS High to deselect the meter chip */
  LATDbits.LATD14 = 1;  // Chip select high
  TimeMonitorEnable();
  return ReadData;
}

/***********************************************
 * write a given value to a meter chip register at a specified address 
 * This is a blocking routine.  No error checking.
 * It will lockup if the SPI is not enabled.
 *************************************************/
void WriteMeterChip(BYTE addr, BYTE numbits, DWORD value){
  BYTE temp;
  BYTE firstbyte = 0x3f & addr;//force address
  BYTE numWrBytes;
  BYTE remainder;
  DWORD WriteData=0;
  BYTE count;

  // Steps to writing data to meter chip
  // 1) Calculate number of bytes to send to chip, partials are rounded up
  // 2) Send mode (0x80 = write) and address bits
  // 3) Send correct number of bytes, while reading back (and throwing out) received data

  TimeMonitorDisable();

  // 1)
  numWrBytes = numbits/8;
  remainder = numbits %8;  //number of bits in partial byte
  if(remainder>0) numWrBytes +=1;

  // 2)
  firstbyte |= 0x80; //specify write operation
  LATDbits.LATD14 = 0;  // Chip select low
/*   GPIO_SETL = BIT(3);// Set *CS High to deselect the meter chip */
/*   //Set *CS low                                                 */
/*   GPIO_CLRL = BIT(3);// Set *CS Low to select the meter chip    */
/*     //SPI Send Address                                          */
/*   SC2_DATA = firstbyte; //Send mode and address bits                              */
/*   while ((SC2_SPISTAT & SC_SPITXIDLE)!= SC_SPITXIDLE){//wait for byte to transmit */
/*   }                                                                               */
/*   temp = SC2_DATA;  //Read byte from FIFO                                         */
  spiSendRecvChar(spi_channel_in_use, firstbyte);

  for(count=numWrBytes; count>0;count--)
  {
    WriteData = value ;
    //position the byte to send in the least significant byte position
    WriteData = WriteData >>((count-1)*8);
    temp = (BYTE) (WriteData & 0x000000ff);
//      APP_INFO_PRINT("write temp is %x\r\n", temp);
/*       SC2_DATA = temp;//Send Dummy byte to get the SPI read                           */
/*       while ((SC2_SPISTAT & SC_SPITXIDLE)!= SC_SPITXIDLE){//wait for byte to transmit */
/*       }                                                                               */
/*       temp = SC2_DATA;  //Read byte from FIFO                                         */
    spiSendRecvChar(spi_channel_in_use, WriteData);
  }

  LATDbits.LATD14 = 1;  // Chip select high
/*   GPIO_SETL = BIT(3);// Set *CS High to deselect the meter chip */

  TimeMonitorEnable();
}


