/**
 * BasicDR.h
 * 
 * This file contains definitions for BasicDR.c
 */
#ifndef __GENERIC_TYPE_DEFS_H_
#include "GenericTypeDefs.h"
#endif
#include "MCI_Common.h"

#define BASICDR

#define COMMODITY_DATA_BUFFER_LENGTH      10

#define opc1index     4
#define opc2index     5

//how long before we assume there won't be an SCG response and return a failure code to the ICG
#define RELAY_TIMEOUT_PERIOD        500      

//operational state definitions
#define IDLE_NORMAL                     0
#define RUNNING_NORMAL                  1
#define RUNNING_CURTAILED               2
#define RUNNING_HEIGHTENED              3
#define IDLE_CURTAILED                  4
#define SGD_ERROR_CONDITION             5
#define IDLE_HEIGHTENED                 6
#define CYCLING_ON                      7
#define CYCLING_OFF                     8
#define VARIABLE_FOLLOWING              9  
#define VARIABLE_NOT_FOLLOWING          10
#define IDLE_OPTED_OUT                  11
#define RUNNING_OPTED_OUT               12

//IntermediateDR response code definitions
#define IDRR_SUCCESS                         0
#define IDRR_COMMAND_NOT_IMPLEMENTED         1
#define IDRR_BAD_VALUE                       2
#define IDRR_COMMAND_LENGTH_ERROR            3
#define IDRR_RESPONSE_LENGTH_ERROR           4
#define IDRR_BUSY_CODE                       5
#define IDRR_OTHER_ERROR                     6

//application nak codes
#define APPNAK_NO_REASON                       0x00
#define APPNAK_OPCODE1_NOT_SUPPORTED          0x01
#define APPNAK_OPCODE2_INVALID              0x02
#define APPNAK_BUSY                            0x03
#define APPNAK_LENGTH_INVALID                  0x04


//Masks
#define BYTE6 0xff0000000000
#define BYTE5 0x00ff00000000
#define BYTE4 0x0000ff000000
#define BYTE3 0x000000ff0000
#define BYTE2 0x00000000ff00
#define BYTE1 0x0000000000ff

typedef struct RelayMsg_t{
    short int httpCode;
    unsigned char codeByte;
}RelayMsg;

typedef struct TempOffsetRelayMsg_t{
    short int httpCode;
    unsigned char responseCode;
    unsigned char currentOffset;
    unsigned char units;
}TempOffsetRelayMsg;

typedef struct EnergyPriceRelayMsg_t{
    short int httpCode;
    unsigned char responseCode;
    short int currentPrice;
    short int currencyCode;
    unsigned char digitsAfterPoint;
    int expirationTime;
    int nextPrice;
}EnergyPriceRelayMsg;


typedef struct TempSetpointRelayMsg_t{
    short int httpCode;
    unsigned char responseCode;
    short int deviceType;
    unsigned char units;
    short int setpoint1;
    short int setpoint2;
}TempSetpointRelayMsg;

typedef struct CommodityReadData_t{
    unsigned char commodityCode;
    unsigned long long instantaneousRate;
    unsigned long long cumulativeAmount;
    int nCommodities;
}CommodityReadData;

typedef struct DeviceInformation_t{
    unsigned short int CTAver;
    unsigned short int vendorID;
    unsigned short int deviceType;
    unsigned short int deviceRev;
    unsigned int capbmp;
    unsigned long long modelNumber;
    unsigned long long int serialNumber;
    unsigned char firmwareYear;
    unsigned char firmwareMonth;
    unsigned char firmwareDay;
    unsigned char firmwareMajor;
    unsigned char firmwareMinor;    
} DeviceInformation;

typedef struct DeviceInfoRelayMsg_t{
    short int httpCode;
    unsigned char responseCode;
    DeviceInformation DevInfo;
}DeviceInfoRelayMsg;

//define an externally visible variable for commodity data to be stored in
extern CommodityReadData commodityResponse[10];
extern DeviceInformation DeviceInfo;

typedef struct CommodityRelayMsg_t{
    short int httpCode;
    unsigned char responseCode;
    int nCommodities;
    CommodityReadData *commodityResponse;
}CommodityRelayMsg;

extern unsigned char nOptions;
extern unsigned char override;

void BasicDRMessageHandler(unsigned char * msg);
void IntermediateDRMessageHandler(unsigned char *msg);

MCIResponse TransmitCommGoodPacket( void );
MCIResponse TransmitCommBadPacket( void );

RelayMsg SendShedCommand(int eventDuration);
RelayMsg SendEndShedCommand(void);
RelayMsg SendRequestForPowerLevel(int percent);
RelayMsg SendPresentRelativePrice(double rpi);
RelayMsg SendNextPeriodRelativePrice(double rpi);
RelayMsg SendTimeRemainingInPresentPricePeriod(int eventDuration);
RelayMsg SendCriticalPeakEvent(int eventDuration);
RelayMsg SendGridEmergency(int eventDuration);
RelayMsg SendLoadUp(int eventDuration);
RelayMsg SendQueryOpState(void);
RelayMsg SendTimeSync(int day, int hour);
RelayMsg SendOutsideCommGood(void);
RelayMsg SendOutsideCommLost(void);

RelayMsg SendSetCommodityRead(unsigned char, long long, long long);
CommodityRelayMsg SendGetCommodityRead(unsigned char, unsigned char);
RelayMsg SendStartAutonomousCycling(UINT32 ID, UINT32 startTime, UINT16 duration, UINT8 dutyCycle, UINT8 startRand, UINT8 endRand, UINT8 crit);
TempOffsetRelayMsg SendGetTemperatureOffset(void);
TempSetpointRelayMsg SendGetPresentTemperature(void);
RelayMsg SendSetTemperatureOffset(unsigned char currentOffset, unsigned char units);
RelayMsg SendTerminateAutonomousCycling(UINT32 ID, UINT8 endRand);
TempSetpointRelayMsg SendGetSetPoint(void);
RelayMsg SendSetSetPoint(UINT16 deviceType, UINT8 units, UINT16 setpoint1, UINT16 setpoint2);
EnergyPriceRelayMsg SendGetEnergyPrice(void);
RelayMsg SendSetEnergyPrice(UINT16 currentPrice, UINT16 currencyCode, UINT8 digitsAfterPoint, UINT32 expirationTime, UINT32 nextPrice);
DeviceInfoRelayMsg SendInfoRequest(void);

unsigned char MakeDurationByte(int duration);
void ReverseByteOrder(void *ptr, int length);
short int HandleIDRResponseCode(unsigned char code);
void RelayTimeoutCallback(void);
BOOL CheckDataValid(void);
void BlockUntilReady(void);
void OverrideTimeoutCallback(void);