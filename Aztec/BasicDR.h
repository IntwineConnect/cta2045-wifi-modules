/**
 * BasicDR.h
 * 
 * This file contains definitions for BasicDR.c
 */
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
#define SUCCESS                         0
#define COMMAND_NOT_IMPLEMENTED         1
#define BAD_VALUE                       2
#define COMMAND_LENGTH_ERROR            3
#define RESPONSE_LENGTH_ERROR           4
#define BUSY_CODE                       5
#define OTHER_ERROR                     6

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
    long int instantaneousRate;
    long int cumulativeAmount;
}CommodityReadData;

//define an externally visible variable for commodity data to be stored in
extern CommodityReadData commodityResponse[10];

typedef struct CommodityRelayMsg_t{
    short int httpCode;
    unsigned char responseCode;
    unsigned char nCommodities;
    CommodityReadData *commodityResponse;
}CommodityRelayMsg;

void BasicDRMessageHandler(unsigned char * msg);
void IntermediateDRMessageHandler(unsigned char *msg);

RelayMsg SendShedCommand(int eventDuration);
RelayMsg SendEndShedCommand(void);
RelayMsg SendRequestForPowerLevel(double percent, int produce);
RelayMsg SendPresentRelativePrice(double rpi);
RelayMsg SendTimeRemainingInPresentPricePeriod(int eventDuration);
RelayMsg SendCriticalPeakEvent(int eventDuration);
RelayMsg SendGridEmergency(int eventDuration);
RelayMsg SendLoadUp(int eventDuration);
RelayMsg SendQueryOpState(void);

CommodityRelayMsg SendGetCommodityRead(unsigned char, unsigned char);

unsigned char MakeDurationByte(int duration);
void ReverseByteOrder(void *ptr, int length);