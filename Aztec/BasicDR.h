/**
 * BasicDR.h
 * 
 * This file contains definitions for BasicDR.c
 */

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

typedef struct RelayMsg_t{
    short int httpCode;
    unsigned char codeByte;
}RelayMsg;


void BasicDRMessageHandler(unsigned char * msg);
RelayMsg SendShedCommand(int eventDuration);
RelayMsg SendEndShedCommand(void);
RelayMsg SendRequestForPowerLevel(double percent, int produce);
RelayMsg SendPresentRelativePrice(double rpi);
RelayMsg SendTimeRemainingInPresentPricePeriod(int eventDuration);
RelayMsg SendCriticalPeakEvent(int eventDuration);
RelayMsg SendGridEmergency(int eventDuration);
RelayMsg SendLoadUp(int eventDuration);
RelayMsg SendQueryOpState(void);


unsigned char MakeDurationByte(int duration);
