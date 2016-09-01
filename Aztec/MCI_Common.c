/**
 *MCI_Common.c
 * global definitions for MCI messages
 */
#include "HardwareProfile.h"
#include "EPRI_UARTLayer.h"
#include "EPRI_SPILayer.h"
#include "MCI_Common.h"

unsigned char TimeSyncMsg[8] = { 0x08, 0x01, 0x00, 0x02, 0x16, 0x00, 0x00, 0x00};


//-----------------------------------------------------------------------------
// MCI Basic DR command. Message Type = 0x08, 0x01
//---------------------------------
// Refer to "Modular Communication Interface Specification for Demand Response"
// Bytes 1,2 are Message Type
// Bytes 3,4 are length msb, lsb.
// Bytes 5,6 are Opcode1, Opcode2
// Bytes 7,8 are checksum, calculated by MCISend()

unsigned char  ShedCommand[8] = { 0x08, 0x01, 0x00, 0x02, 0x01, 0x00, 0xff, 0xff };

unsigned char  EndShedCommand[8] = { 0x08, 0x01, 0x00, 0x02, 0x02, 0x00, 0xff, 0xff };

unsigned char AppAckMsg[RX_BUF_SIZE] = { 0x08, 0x01, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00};

unsigned char RequestPowerLevel[8] = { 0x08, 0x01, 0x00, 0x02, 0x06, 0x00, 0xff, 0xff };

unsigned char PresentRelativePrice[8] = { 0x08, 0x01, 0x00, 0x02, 0x07, 0x00, 0xff, 0xff };

unsigned char NextPeriodRelativePrice[8] = { 0x08, 0x01, 0x00, 0x02, 0x08, 0x00, 0xff, 0xff };

unsigned char TimeRemainingInPresentPricePeriod[8] = { 0x08, 0x01, 0x00, 0x02, 0x09, 0x00, 0xff, 0xff };

unsigned char CriticalPeakEvent[8] = { 0x08, 0x01, 0x00, 0x02, 0x0a, 0x00, 0xff, 0xff };

unsigned char GridEmergency[8] = { 0x08, 0x01, 0x00, 0x02, 0x0b, 0x00, 0xff, 0xff };

unsigned char QueryOpState[8] = { 0x08, 0x01, 0x00, 0x02, 0x12, 0x00, 0x00, 0x00};

// Set Present Relative Price
unsigned char PresentRelPrice[8] = { 0x08, 0x01, 0x00, 0x02, 0x07, 0x01, 0xff, 0xff };
// Outside Comm Connection Status, Found / Good Connection
unsigned char OutsideCommGood[8] = { 0x08, 0x01, 0x00, 0x02, 0x0E, 0x01, 0xff, 0xff };
// Outside Comm Connection Status, No / Lost Connection
unsigned char OutsideCommLost[8] = { 0x08, 0x01, 0x00, 0x02, 0x0E, 0x00, 0xff, 0xff };

unsigned char LoadUp[8] = { 0x08, 0x01, 0x00, 0x02, 0x17, 0x00, 0xff, 0xff };

//-----------------------------------------------------------------------------
// MCI Intermediate DR Application. Message Type = 0x08, 0x02
//--------------------------------------------

// SetEnergyPrice
// Bytes 7,8 are Current Price
// Bytes 9,10 are Currency code = 840 for US dollars (0x348)
// Byte 11 is digits after decimal place, currently fixed at 2 (cents)
unsigned char  SetEnergyPrice[] = { 0x08, 0x02, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x03, 0x48, 0x02, 0xff, 0xff };
unsigned char GetEnergyPrice[] = {0x08, 0x02, 0x00, 0x02, 0x03, 0x00, 0xff, 0xff};
unsigned char GetCommodityReadGeneral[] = {0x08, 0x02, 0x00, 0x02, 0x06, 0x00, 0xff, 0xff};
unsigned char GetCommodityRead[] = {0x08, 0x02, 0x00, 0x03, 0x06, 0x00, 0x00, 0xff, 0xff};
unsigned char StartAutonomousCycling[] = {0x08, 0x02, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0xff, 0xff};
unsigned char TerminateAutonomousCycling[] = {0x08, 0x02, 0x00, 0x07, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};
unsigned char SetTemperatureOffset[] = {0x08, 0x02, 0x00, 0x04, 0x03, 0x02, 0xff, 0xff};
unsigned char GetTemperatureOffset[] = {0x08, 0x02, 0x00, 0x02, 0x03, 0x02, 0x00, 0x00, 0xff, 0xff};
unsigned char GetSetPoint[] = {0x08, 0x02, 0x00, 0x02, 0x03, 0x03, 0xff, 0xff};
unsigned char SetSetPoint[] = {0x08, 0x02, 0x00, 0x09, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};

short int httpCode = 0;
unsigned char codeByte = DEFAULT_RETURN_CODE;
unsigned char responseCode;
unsigned char nCommodities;
unsigned char currentOffset;
unsigned char units;
short int deviceType;
short int setpoint1;
short int setpoint2;
short int currentPrice;
short int currencyCode;
unsigned char digitsAfterPoint;
int expirationTime;
int nextPrice;

BOOL ResponseReadyFlag = 0;

unsigned char dl_ack[2] = {0x06, 0x00};
unsigned char dl_nak[2] = {0x15, 0x03};

/**
 * Construct and send a MCI packet either UART or SPI (whichever is enabled) blocking.
 *
 * @param msgtype1 Msg type byte, MSB
 * @param msgtype2 msg type byte, LSB
 * @param payload the message payload
 */
MCIResponse MCISendNeutral(unsigned char * msg)
{
    MCIResponse retval;
#ifdef DC_CEA2045
    retval = MCISendSPI(msg);
#else
    retval = MCISend(msg);
#endif
    
    return retval;
}

/**
 * Send a Data Layer ACK via the UART.
 */
void DL_Ack()
{
#ifdef DC_CEA2045
    EPRI_SPI_write(dl_ack, 2);
#else
    EPRI_UART_write(dl_ack, 2);
#endif
}

/**
 * Send a Data Layer NACK via the UART.
 * 
 * @param: code- gives the NACK type code
 */
void DL_Nak(unsigned char code)
{
    dl_nak[1] = code;
#ifdef DC_CEA2045
    EPRI_SPI_write(dl_nak,2);
#else
    EPRI_UART_write(dl_nak, 2);
#endif
}

/**
 * Send a App Layer ACK via the UART.
 */
void App_Ack(unsigned char * message, int len)
{
#ifdef DC_CEA2045
    EPRI_SPI_write(message, len);
#else
    EPRI_UART_write(message, len);
#endif
}