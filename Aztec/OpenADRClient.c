 /*********************************************************************
 * Intwine Connect OpenADR client code
 * Dave Halasz, Joe Logan
 *
 *  Based on Microchip Generic TCP Client Example Application,
 * GenericTCPClient.c. Also includes Intwine Connect SEP 2.0
 * and t-stat code.
 *
 *  Microchip license info:
 * Software License Agreement
 *
 * Copyright (C) 2002-2008 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and
 *      ENC28J60.h ported to a non-Microchip device used in
 *      conjunction with a Microchip ethernet controller for the
 *      sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     8/01/06    Original
 ********************************************************************/
#define __GENERICTCPCLIENT_C

#define OADR_DEBUG_UART UART2

#include "HardwareProfile.h"
#include "OpenADRClient.h"
#include "TCPIP_Stack/TCPIP.h"
#include "TimeMonitor.h"
#include "EPRI_Queue.h"
#include "MCI_Common.h"
#include "BasicDR.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifdef AC_CEA2045
  #include "EPRI_UARTLayer.h"
#else
  #include "EPRI_SPILayer.h"
#endif


int tempbreakpoint;
unsigned char   DebugCnt;

// WiFi and OpenADR provisioning structure stored in EEPROM. See PROVdMain.c
extern int UARRT2Provisioning;

extern int OverRide;
extern int OptOutEvent;
extern int OptInEvent;
extern int IdleNormal;
extern int RunningNormal;

char    strAvailabilityStatus[] = "FALSE";

// RS485 response buffer. defined in EPRI_UARTLayer.c and modified there.
MCIResponse RsBuf;

int commGood; // flag to indicate if communication with the outside world is working. needed for MCI comm with SGD
int commGoodOld;  // change of state indicator

char    pbuf[120];
char    GMTstr[50];

char    drasClientID[DRAS_LEN] = "epri.Wi-Fi2MCIu1";
char    drasClientIDPW[] = "Test_1234";
int     ServerPort = 80;

int     AvgPrice = 40;
int     LowPrice = 30;
int     HighPrice = 50;   
int     SpecialPrice = 20;
int     OpMode = 0;

char    EventStatePage[EVENT_LEN] = "/RestClientWS/nossl/rest2";
char    EventStateConfirmPage[EVENT_LEN] = "/RestClientWS/nossl/restConfirm";
char    OpenADRServerName[URL_LEN] = "cdp.openadr.com";
char    FeedBackPage[EVENT_LEN] = "";
char    OptOutPage[EVENT_LEN] = "";

char    OpenADRServerIP[16] = "70.42.189.230";
IP_ADDR DNS_IP_ADDR;

// OpenADR messages are roughly 300 + (1500 per eventState)
// OPENADR_QSIZE in EPRI_Queue.h
#define TCPPAYLOAD_BUF_SIZE    300 + OPENADR_QSIZE * 1500
char    TCPpayload_buf[TCPPAYLOAD_BUF_SIZE];

int     FoundHTTPHeader;
int     FoundHTTPBody;

/*    HTTPResponse
      - 1xx: Informational - Request received, continuing process
      - 2xx: Success - The action was successfully received,
        understood, and accepted
      - 3xx: Redirection - Further action must be taken in order to
        complete the request
      - 4xx: Client Error - The request contains bad syntax or cannot
        be fulfilled
      - 5xx: Server Error - The server failed to fulfill an apparently
        valid request
      -  -1: Error finding the HTTPResponse
*/
int HTTPResponse;

char *payloadptr;   // Used to tell when to set flag below.
OpenADR_Event eventStateQ[OPENADR_QSIZE];

// XML_KEY_VAL XMLPairs[XML_PAIRS];  // Can handle up to XML_PAIRS key value pairs @ MAX_KEY / MAX_VAL characters each.
int InvalidSocketReboot=0;

DWORD    SQueryTimer;
DWORD    DelayTimer;

DWORD    GetEventStateTimer;
DWORD    CommTimer;
DWORD    DNSTimer = 0;

#define COMM_TIMEOUT_SEC    60*5

char    eventStateID[32];
char    eventIdentifier[32];
char    ActiveEventID[32];
char    programName[32];
char    eventStatus[8];            // NONE, FAR, NEAR, ACTIVE
char    operationModeValue[8];        // NORMAL, HIGH,
char    eventModNumber[20];
char    currentTime[20];
char    timeOffset[20];
char    modeTimeSlot[20];
char    notificationTime[40];
char    startTime[40];
char    endTime[40];

int     SmartClient = 1;
int     ClientTypeKnown = 0;
int     RequestedClientType = 0;

int     RequestedPriceAbs = 0;
int     SupportsPriceAbs = 0;
int     PriceAbsSupportKnown = 0;

int     RequestedPriceRel = 0;
int     SupportsPriceRel = 0;
int     PriceRelSupportKnown = 0;

int     MAX_PAYLOAD_SGD = 2;        // maximum number of Bytes in payload - default is 2

int     ResetSGD = 0;

// drEventData
int        CommandType = 0;
#define    COMMAND_NONE            0
#define    LOAD_LEVEL_COMMAND        1
#define    PRICE_ABSOLUTE_COMMAND    2

char    SourceUnamePw[50];
char    EncodedUNPW[75];

#define    POLLSLOW   60
#define    POLLMEDIUM 30
#define    POLLFAST   15
int        PollRate = POLLSLOW;

time_t  ISO8601time( char * );  // Parse OpenADR start,end times.
time_t  RFC882time( char * );   // Parse server "Date:" string.
                                // Only supports RFC822 - one of 3 time formats.

time_t  UTCTime;                // Running system time used for dispatching events
time_t  UTCServerTime;          // last server time parsed from HTTP header
time_t  Local_Time;             // Local time. Derived from starttime+tzsec+currenttime from EventState
int     tzsec;                  // Offset in seconds from GMT for last time sent to ISO8601time
int     TCP_blackout;           // Used to prevent UTC and Local_Time updates around the time sync period
time_t  UTCLastServerTime;      // Last good time
DWORD   LastServerTicks;        // Tick timer at UTCLastServerTime

unsigned int    LoadFactor;

//RS485 commands
unsigned char SmartQuery[] = { 0x00, 0x00 };


//-----------------------------------------------------------------------------

char    TCPPutBuffer[1000];

//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------
MCIResponse TransmitCommGoodPacket( void );
MCIResponse TransmitCommBadPacket( void );
//-----------------------------------------------------------------------------

static DWORD        TCP_Timeout;
static TCP_SOCKET   MyTCPSocket = INVALID_SOCKET;

static int payLoadBufCtr=0;
static int resetModule=0;

#define INVALID_SOCKET_REBOOT 50

//-----------------------------------------------------------------------------

// Initialization for OpenADR communication
void InitOpenADRComm( void )
{
    int i;

    DebugCnt = 0;
    
#ifdef AC_CEA2045
    EPRI_UART_init();
#endif
    
    GetEventStateTimer = TickGet();
    CommTimer = TickGet();

    SmartClient = 1;
    ClientTypeKnown = 0;
    RequestedClientType = 0;

    RequestedPriceAbs = 0;
    SupportsPriceAbs = 0;
    PriceAbsSupportKnown = 0;

    RequestedPriceRel = 0;
    SupportsPriceRel =  1;      //assume support and check during first event
    PriceRelSupportKnown = 0;

    ResetSGD = 0;

    Local_Time = 0;
    UTCTime = 0;
    
    LoadFactor = 0;

    commGood = 0;
    commGoodOld = 0;

    // Send EndShed at start-up
    DelayMs(500);
    SendEndShedCommand();
    
    // Send Maximum Payload Query at start-up - will modify MAX_PAYLOAD_SGD
    DelayMs(500);
    SendQueryMaximumPayloadLength();
    
}

// JL addition
// sends a "comm good" packet to the end device
MCIResponse TransmitCommGoodPacket()
{
    return MCISendNeutral(OutsideCommGood);    
}

// JL addition
// sends a "comm bad" packet to the end device
MCIResponse TransmitCommBadPacket()
{
    return MCISendNeutral(OutsideCommLost);    
}

time_t RFC882time( char *datestr )
{
    char    weekday[10], month[10];
    int     n, day, year, hour, minute, sec;
    struct tm timestruct;
    time_t  utctime;

    n = sscanf( datestr, "Date: %s %d %s %d %d:%d:%d", weekday, &day, month, &year, &hour, &minute, &sec );
    
    if (strcmp(month,"Jan") == 0)
        timestruct.tm_mon = 0;
    else if (strcmp(month,"Feb") == 0)
        timestruct.tm_mon = 1;
    else if (strcmp(month,"Mar") == 0)
        timestruct.tm_mon = 2;
    else if (strcmp(month,"Apr") == 0)
        timestruct.tm_mon = 3;
    else if (strcmp(month,"May") == 0)
        timestruct.tm_mon = 4;
    else if (strcmp(month,"Jun") == 0)
        timestruct.tm_mon = 5;
    else if (strcmp(month,"Jul") == 0)
        timestruct.tm_mon = 6;
    else if (strcmp(month,"Aug") == 0)
        timestruct.tm_mon = 7;
    else if (strcmp(month,"Sep") == 0)
        timestruct.tm_mon = 8;
    else if (strcmp(month,"Oct") == 0)
        timestruct.tm_mon = 9;
    else if (strcmp(month,"Nov") == 0)
        timestruct.tm_mon = 10;
    else if (strcmp(month,"Dec") == 0)
        timestruct.tm_mon = 11;
    else
        timestruct.tm_mon = 0;

    timestruct.tm_mday = day;
    timestruct.tm_year = year - 1900;
    timestruct.tm_hour = hour;
    timestruct.tm_min = minute;
    timestruct.tm_sec = sec;

    utctime = mktime(&timestruct);
    
    return utctime;

}

time_t ISO8601time( char *datestr )
{
    int     n, day, month, year, hour, minute, sec;
    struct tm timestruct;
    time_t  utctime;
    float   fsec;
    int     tzhours, tzmin;
    char timezone[10];

    if (strchr(datestr,'-'))
        n = sscanf( datestr, "%d-%d-%dT%d:%d:%f%s", &year, &month, &day, &hour, &minute, &fsec, timezone);
    else
        n = sscanf( datestr, "%4d%2d%2dT%d:%d:%f%s", &year, &month, &day, &hour, &minute, &fsec, timezone);
    
    
    sec = (int) fsec;

    timestruct.tm_year = year - 1900;
    timestruct.tm_mday = day;
    timestruct.tm_mon = month - 1;

    timestruct.tm_hour = hour;
    timestruct.tm_min = minute;
    timestruct.tm_sec = sec;

    utctime = mktime(&timestruct);
    
    if (utctime == -1)
        return utctime;
    
    tzsec = 0;
    if (timezone[0] != 'Z')
    {
        n = sscanf( timezone, "%d:%d", &tzhours, &tzmin );
        if (tzhours < 0)
            tzsec = (tzhours * 60 * 60) - tzmin;
        else
            tzsec = (tzhours * 60 * 60) + tzmin;
    }
    
    utctime -= tzsec;

    return utctime;

}
