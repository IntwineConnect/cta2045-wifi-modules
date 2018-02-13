/**
 *MCI_Common.h
 *global declarations for MCI messages
 */


#ifndef __GENERIC_TYPE_DEFS_H_
  #include "GenericTypeDefs.h"
#endif

#ifndef __MCI_COMMON_H
 #define __MCI_COMMON_H

//BASIC
#define SHED_CODE                               0x01
#define END_SHED_CODE                           0x02
#define APP_ACK_CODE                            0x03
#define APP_NAK_CODE                            0x04
#define REQUEST_POWER_LEVEL_CODE                0x06
#define PRESENT_RELATIVE_PRICE_CODE             0x07
#define NEXT_PERIOD_RELATIVE_PRICE_CODE         0x08
#define TIME_IN_PRICE_PERIOD_CODE               0x09
#define CRITICAL_PEAK_EVENT_CODE                0x0a
#define GRID_EMERGENCY_CODE                     0x0b
#define GRID_GUIDANCE_CODE                      0x0c
#define OUTSIDE_COMM_CODE                       0x0e
#define CUSTOMER_OVERRIDE_CODE                  0x11
#define QUERY_OP_STATE_CODE                     0x12
#define RESPONSE_OP_STATE_CODE                  0x13
#define SLEEP_CODE                              0x14
#define WAKE_REFRESH_REQUEST_CODE               0x15
#define SIMPLE_TIME_SYNC_CODE                   0x16
#define LOAD_UP_CODE                            0x17
#define PENDING_EVENT_CODE                      0x18
#define PENDING_EVENT_TYPE_CODE                 0x19

//INTERMEDIATE
#define GET_COMMODITY_READ_CODE                 0x06
#define GET_COMMODITY_READ_REPLY_CODE           0x80
#define SET_COMMODITY_READ_CODE                 0x06
#define SET_COMMODITY_READ_REPLY_CODE           0x80
#define START_AUTONOMOUS_CYCLING_CODE           0x04
#define START_AUTONOMOUS_CYCLING_REPLY_CODE     0x80
#define TERMINATE_AUTONOMOUS_CYCLING_CODE       0x04
#define TERMINATE_AUTONOMOUS_CYCLING_REPLY_CODE 0x81
#define SET_TEMPERATURE_OFFSET_CODE             0x03
#define SET_TEMPERATURE_OFFSET_REPLY_CODE       0x82
#define GET_TEMPERATURE_OFFSET_CODE             0x03
#define GET_TEMPERATURE_OFFSET_REPLY_CODE       0x82
#define GET_PRESENT_TEMPERATURE_CODE            0x03
#define GET_PRESENT_TEMPERATURE_REPLY_CODE      0x84
#define SET_SET_POINT_CODE                      0x03
#define SET_SET_POINT_REPLY_CODE                0x83
#define GET_SET_POINT_CODE                      0x03
#define GET_SET_POINT_REPLY_CODE                0x83
#define SET_ENERGY_PRICE_CODE                   0x00
#define SET_ENERGY_PRICE_REPLY_CODE             0x80
#define GET_ENERGY_PRICE_CODE                   0x03
#define GET_ENERGY_PRICE_REPLY_CODE             0x80
#define INFO_REQUEST_CODE                       0x01
#define INFO_REQUEST_REPLY_CODE                 0x81

#define DEFAULT_RETURN_CODE                     0xff


extern unsigned char TimeSyncMsg[8];
extern unsigned char QueryOpState[8];


extern unsigned char EndShedCommand[8];
extern unsigned char ShedCommand[8];
extern unsigned char RequestPowerLevel[8];
extern unsigned char PresentRelativePrice[8];
extern unsigned char NextPeriodRelativePrice[8];
extern unsigned char TimeRemainingInPresentPricePeriod[8];
extern unsigned char CriticalPeakEvent[8];
extern unsigned char GridEmergency[8];
extern unsigned char LoadUp[8];
// Set Critical Peak Event
extern unsigned char CriticalPeakEvent[8];
// Set Present Relative Price
extern unsigned char PresentRelPrice[8];
// Outside Comm Connection Status, Found / Good Connection
extern unsigned char OutsideCommGood[8];
// Outside Comm Connection Status, No / Lost Connection
extern unsigned char OutsideCommLost[8];
extern unsigned char TimeSync[8];

//-----------------------------------------------------------------------------
// MCI Intermediate DR Application. Message Type = 0x08, 0x02
//--------------------------------------------

extern unsigned char SetEnergyPrice[13];
extern unsigned char GetEnergyPrice[8];
extern unsigned char SetCommodityRead[21];
extern unsigned char GetCommodityRead[9];
extern unsigned char GetCommodityReadGeneral[8];
extern unsigned char StartAutonomousCycling[22];
extern unsigned char TerminateAutonomousCycling[13];
extern unsigned char SetTemperatureOffset[10];
extern unsigned char GetTemperatureOffset[8];
extern unsigned char GetPresentTemperature[8];
extern unsigned char GetSetPoint[8];
extern unsigned char SetSetPoint[16];
extern unsigned char InfoRequest[8];


// These are potentially set from an interrupt (timeout), so they must be volatile.
volatile extern short int httpCode;
extern unsigned char codeByte;
volatile extern BOOL ResponseReadyFlag;
extern unsigned char responseCode;
extern unsigned char nCommodities;
extern unsigned char currentOffset;
extern unsigned char units;
extern short int deviceType;
extern short int setpoint1;
extern short int setpoint2;
extern short int currentPrice;
extern short int currencyCode;
extern unsigned char digitsAfterPoint;
extern int expirationTime;
extern int nextPrice;

#define RX_BUF_SIZE 300
#define TX_MSG_SIZE 10

extern unsigned char AppAckMsg[RX_BUF_SIZE];



typedef struct
{
   UINT8   numTries;            // Number of times the command was sent before it completed.
   UINT8   numBytesReceived;    // Total bytes received in last message. Includes Checksum
   UINT8   LLResponseValid;     // flag that indicates we have something
   UINT8   LLResponse[11];      // the received packet
   UINT8   AppResponseValid;    // flag that indicates we have something
   UINT8   AppResponse[11];     // the received packet
} MCIResponse;

extern unsigned char dl_ack[2];
extern unsigned char dl_nak[2];

MCIResponse MCISendNeutral(unsigned char * msg);


#endif