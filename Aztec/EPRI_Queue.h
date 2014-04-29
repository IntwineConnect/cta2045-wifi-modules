/**
 * File: EPRI_Queue.h
 * Desc: Queue for incoming OpenADR commands. 
 *       Queue for outgoing MCI commands. 
 *
 */
 
 #ifndef INCLUDE_EPRI_QUEUE_H
 #define INCLUDE_EPRI_QUEUE 1

 #endif
 
#define OPENADR_QSIZE 4
#define MAGIC_OPENADRQ_FLASH_VALUE   ((UINT32) 0x31466958)

void queue_up(unsigned char * thestring, int len);


typedef struct _OpenADR_Event 
{ 
    unsigned long int magicAddress;     // Magic word to check for valid command.    
    char eventStateID[32];              // Event transaction ID from server. Changes every HTTP GET
    char eventIdentifier[32];           // Event ID from server. Constant for an event
    char programName[32];               // programName from server
    char eventStatus[16];               // Event Status from server
    char LoadLevel[16];
    unsigned int  eventModNumber;       // Indication that event parameters have changed
    UINT32 starttime;                   // Starttime of event in UTC
    UINT32 endtime;                     // Endtime of event in UTC
    UINT32 duration;                    // MCI duration byte is mapped exponentially, so save calculated value for ease of use
    UINT32 notificationtime;            // When DRAS Server started notification
    float PriceAbsolute;
    float PriceRelative;
    float PriceMultiple;
    float LoadAmount;
    float LoadPercentage;
    float GridReliability;
    unsigned char executing;            // Has this been executed?
    unsigned char executed;             // Has this been executed?
} OpenADR_Event;
