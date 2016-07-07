/**
 *MCI_Common.h
 *global declarations for MCI messages
 */


#ifndef __GENERIC_TYPE_DEFS_H_
  #include "GenericTypeDefs.h"
#endif

#ifndef __MCI_COMMON_H
 #define __MCI_COMMON_H


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
extern unsigned char      CriticalPeakEvent[8];
// Set Present Relative Price
extern unsigned char PresentRelPrice[8];
// Outside Comm Connection Status, Found / Good Connection
extern unsigned char OutsideCommGood[8];
// Outside Comm Connection Status, No / Lost Connection
extern unsigned char OutsideCommLost[8];

//-----------------------------------------------------------------------------
// MCI Intermediate DR Application. Message Type = 0x08, 0x02
//--------------------------------------------

extern unsigned char  SetEnergyPrice[13];


//SCG response ready for return to ICG
/*
typedef struct 
{
    short int httpCode;
    unsigned char code1;
    unsigned char code2;
    BOOL ResponseReadyFlag;
    
} RelayMsgData;

extern RelayMsgData ICG_RelayMsgData;
*/

extern short int httpCode;
extern unsigned char codeByte;
extern BOOL ResponseReadyFlag;



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