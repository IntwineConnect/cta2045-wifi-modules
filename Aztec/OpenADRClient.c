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

int TagAndLongLongVal( char *strptr, char *Tag, long long *LLValue, int base);
int TagAndIntVal( char *strptr, char *Tag, int *intValue, int base);
int StringToInt( char *strptr, int *iptr );
int StringToLongLong( char *strptr, long long *llptr, int base );
int TagAndString( char *strptr, char *Tag, char *Valptr, int maxlength, int attrib );
char *MovePastNameSpace(char *strptr);

int DeterminedSGDCapabilities( void );
void HandleTSMOpen( void );
void HandleTSMSocketObtained( void );

#if defined(STACK_USE_SSL_CLIENT)
void HandleTSMSSLStart( void );
void HandleTSMSSLHandShake( void );
#endif

void HandleTSMWriteSocket( void );
void HandleTSMProcessResponse( void );
void HandleTSMParseResponse( void );
void HandleTSMDone( void );
void ManageOpenADRQueue( void );
int  OpenADRParseGETResponse( void );
int  OpenADRParsePOSTResponse( void );
#if defined(PGE)
int  OpenADRParseFeedBackResponse( void );
int  OpenADRParseOptOutResponse( void );
#endif
void OpenADRWriteGetSocket( void );
void OpenADRWriteConfirmSocket( void );
void OpenADRWriteFeedBackSocket( void );
void OpenADRWriteOptOutSocket( void );
int  FormatEventStateConfirmXML( char *Buffer );
void HandleEventStatesXML( char *strptr );
void HandleListOfEventStateXML( char *strptr );
void ResetHeaderData( void );
void MCI_One_Second_Callback();
WORD Base64Encode(BYTE* cSourceData, WORD wSourceLen, BYTE* cDestData, WORD wDestLen);
MCIResponse TransmitCommGoodPacket( void );
MCIResponse TransmitCommBadPacket( void );
//-----------------------------------------------------------------------------

static DWORD        TCP_Timeout;
static TCP_SOCKET   MyTCPSocket = INVALID_SOCKET;

static int payLoadBufCtr=0;
static int resetModule=0;

#define INVALID_SOCKET_REBOOT 50

//-----------------------------------------------------------------------------
// State Variables
//-----------------------------------------------------------------------------

// OpenADR_TCPState
// State varaible for the TCP State Machine
// Fires events on schedule. Opens and closes sockets as needed.
//--------------------------------------------------------------
enum _OpenADR_TCPState
{
    TSM_START_DNS = 0,
    TSM_NAME_RESOLVE,
    TSM_HOME,
    TSM_OPEN_SOCKET,
    TSM_SOCKET_OBTAINED,
    #if defined (STACK_USE_SSL_CLIENT)
    TSM_SSL_START,
    TSM_SSL_HANDSHAKE,
    #endif
    TSM_WRITE_SOCKET,
    TSM_PROCESS_RESPONSE,
    TSM_PARSE_RESPONSE,
    TSM_DONE
} OpenADR_TCPState;

enum _OpenADR_TCPState OpenADR_TCPState = TSM_START_DNS;

// OpenADR Message Status
// Sequences HTTP GETs and POSTs to different URL
// OpenADR_MsgState > 0 means message in-progress.
//------------------------------------------------
enum _OpenADR_MsgState
{
    OADR_MSG_IDLE = 0,           // IDLE
    OADR_GET_EVENTSTATE,         // Send HTTP GET method to server and receive EventState XML
    OADR_POST_EVENTSTATECONFIRM, // POST EventStateConfirm XML to server.
#if defined(PGE)
    OADR_POST_FEEDBACK,          // POST FeedBack XML to server.
    OADR_POST_OPTOUT,            // POST OptOut XML to server.
#endif
    OADR_MSG_DONE                // DONE, back to IDLE.
} OpenADR_MsgState;

enum _OpenADR_MsgState OpenADR_MsgState = OADR_MSG_IDLE;

//-----------------------------------------------------------------------------

// Initialization for OpenADR communication
void InitOpenADRComm( void )
{
    int i;

    DebugCnt = 0;
    
#ifdef AC_CEA2045
    EPRI_UART_init();
#endif
    
//    TCPInitializeBuffer();

    OpenADR_MsgState = OADR_MSG_IDLE;

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
    //RsBuf = MCISendNeutral(EndShedCommand);
    SendEndShedCommand();
    
    // clear the event queue
    for (i = 0; i < OPENADR_QSIZE; i++)
    {
        eventStateQ[i].magicAddress = 0;
        eventStateQ[i].eventIdentifier[0] = '\0';
        eventStateQ[i].eventModNumber = 0;
        eventStateQ[i].starttime = 0;
        eventStateQ[i].endtime = 0;
        eventStateQ[i].executing = 0;
        eventStateQ[i].executed = 0;
    }

}

/*****************************************************************************
  Function:
    void OpenADRClient(void)

  Summary:
    Implements a DRAS Client

  Description:

  Precondition:
    TCP is initialized.

  Parameters:
    None

  Returns:
      None
  ***************************************************************************/
int OpenADRClient( void )
{
    int ip0, ip1, ip2, ip3, n;

    // No TCP while provisioning
    // Variable is set and cleared in rxMessageHandler (EPRI_UARTLayer.c)
//    if ( UARRT2Provisioning )
//        return 1;
//
    // Determine capabilities of the SGD device
//    if ( !DeterminedSGDCapabilities() )
//        return 1;
    
    // Callback from Timer 2 interrupt every 1 second.
    // Used for SGD timesync and status query.
    // Note 10ms gets added by TimeMonitorRegisterI to round up to 1s. 
    if (!ResetSGD)
    {
        ResetSGD = 1;
        TimeMonitorRegisterI(0, 990, MCI_One_Second_Callback);
    }

    // send comm status on change of state
    if (commGood && !commGoodOld)
    {
        // MCI comm good moved to OpenADRParseGETResponse
        // Typical poll rates are 1 min so this is OK.
//        TransmitCommGoodPacket();
        
    }
    else if (!commGood && commGoodOld)
    {
        RsBuf = TransmitCommBadPacket();
        if (RsBuf.AppResponseValid && RsBuf.AppResponse[4] == 0x03 && RsBuf.AppResponse[5] == OutsideCommGood[4])
        // response 0x03, 0x0E = good APP ACK means SGD is functional
            strcpy(strAvailabilityStatus, "TRUE");
        else
            strcpy(strAvailabilityStatus, "FALSE");

    }
    commGoodOld = commGood;

    // Check for active events
    ManageOpenADRQueue();

    switch(OpenADR_TCPState)
    {
        case TSM_START_DNS:
        // If IP address the skip DNS lookup
        n = sscanf( OpenADRServerName, "%d.%d.%d.%d", &ip0, &ip1, &ip2, &ip3);
        if (n == 4 && (ip0 >=0 && ip0 <=255) && (ip1 >=0 && ip1 <=255) && (ip2 >=0 && ip2 <=255) && (ip3 >=0 && ip3 <=255)) {
            strcpy(OpenADRServerIP, OpenADRServerName);
            OpenADR_TCPState = TSM_HOME;
            break;
        }

        // Obtain ownership of the DNS resolution module
        if(!DNSBeginUsage())
                break;
            // Obtain the IP address associated with the server name
            DNSResolveROM(OpenADRServerName, DNS_TYPE_A);
            DNSTimer = TickGet();
            OpenADR_TCPState = TSM_NAME_RESOLVE;
            break;

        case TSM_NAME_RESOLVE:
            // Wait for DNS resolution to complete

            if((TickGet() - DNSTimer) > (5 * TICKS_PER_SECOND)) 
            {
                if(!DNSIsResolved((IP_ADDR*)&DNS_IP_ADDR)) 
                {
                    DNSEndUsage();
                    DNSTimer = TickGet(); //Div64K();
                    OpenADR_TCPState = TSM_START_DNS;
                    break;
               }
            }
            else
            {
                break;
            }
            
            // Obtain DNS resolution result
            if(!DNSEndUsage())
            {
                // No valid IP address was returned from the DNS 
                // server.  Quit and fail for a while if host is not valid.
                DNSTimer = TickGet();
                OpenADR_TCPState = TSM_NAME_RESOLVE;
                break;
            }

            sprintf(OpenADRServerIP, "%d.%d.%d.%d", DNS_IP_ADDR.v[0], DNS_IP_ADDR.v[1], DNS_IP_ADDR.v[2], DNS_IP_ADDR.v[3]); 
            OpenADR_TCPState = TSM_HOME;
            // No break needed
        case TSM_HOME:
            // Check if time to run the EventState command
            // Blackout period prevents time slip and message collisions
            if (!TCP_blackout && !OpenADR_MsgState)
            {
                if ( (TickGet() - GetEventStateTimer) > (TICKS_PER_SECOND*PollRate) )
                {
                    sprintf(pbuf,"\r\nHOME Time = %ld", UTCTime);
                    //putsUART(pbuf);
                    GetEventStateTimer = TickGet();
                    TCP_Timeout = TickGet();
                    OpenADR_MsgState = OADR_GET_EVENTSTATE;
                    OpenADR_TCPState++;
                }
            }

            break;

        case TSM_OPEN_SOCKET:
            HandleTSMOpen();
            break;

        // Wait for the TCP socket to be established
        case TSM_SOCKET_OBTAINED:
            HandleTSMSocketObtained();
            break;

        // Need to obtain socket first??
        #if defined(STACK_USE_SSL_CLIENT)
        case TSM_SSL_START:
            HandleTSMSSLStart();
            break;

        case TSM_SSL_HANDSHAKE:
            HandleTSMSSLHandShake();
            break;
        #endif

        // Commands get sent to the ESI
        // GET, POST or PUT to the ESI
        case TSM_WRITE_SOCKET:
            HandleTSMWriteSocket();
            break;

        // Wait until all of the response has been received
        case TSM_PROCESS_RESPONSE:
            HandleTSMProcessResponse();
            break;

        // Now that the response has been received,
        //  process the response
        case TSM_PARSE_RESPONSE:
            HandleTSMParseResponse();
            break;

        case TSM_DONE:
            HandleTSMDone();
            break;
    }

    if( (TickGet() - CommTimer) > TICKS_PER_SECOND*COMM_TIMEOUT_SEC )
    {
        // TransmitCommBadPacket will be sent next time through this routine
        commGood = 0;
        commGoodOld = 1;
        CommTimer = TickGet();
        //putsUART("\r\nComm Timeout");
    }

    return 1;
}

//
// Open a TCP Socket, connect to the OpenADR server
//
void HandleTSMOpen( void )
{
    if(InvalidSocketReboot>=INVALID_SOCKET_REBOOT)
    {
        while(1);
    }
    // Connect a socket to the remote TCP server
    MyTCPSocket = TCPOpen((DWORD)&OpenADRServerIP[0], TCP_OPEN_RAM_HOST, ServerPort, TCP_PURPOSE_GENERIC_TCP_CLIENT);

    // Abort operation if no TCP socket of type TCP_PURPOSE_GENERIC_TCP_CLIENT is available
    // If this ever happens, you need to go add one to TCPIPConfig.h
    if(MyTCPSocket == INVALID_SOCKET)
        {
        //putsUART("\r\nEvent Invalid Socket....\r\n");
        TCPClose(MyTCPSocket);
            if(TickGet() - TCP_Timeout > 5*TICKS_PER_SECOND)
            {
            // Close the socket so it can be used by other modules
            TCPDisconnect(MyTCPSocket);
            MyTCPSocket = INVALID_SOCKET;
            //putsUART("\r\nTimeout:Invalid Event Socket");
            OpenADR_TCPState = TSM_DONE;
            }
        InvalidSocketReboot++;
        return;
        }
    // #if defined(STACK_USE_UART)
    //putsUART("\r\n Connecting to OpenADR Server");
    // #endif

    TCP_Timeout = TickGet();
    OpenADR_TCPState++;
}


//
// Wait for the TCP socket to be established
//
void HandleTSMSocketObtained( void )
{
    //putsUART("\r\n HandleTSMSocketObtained");

       // Wait for the remote server to accept our connection request
    if(!TCPIsConnected(MyTCPSocket))
    {
        // Time out if too much time is spent in this state
        if(TickGet() - TCP_Timeout > 5*TICKS_PER_SECOND)
        {
            // Close the socket so it can be used by other modules
            TCPDisconnect(MyTCPSocket);
            MyTCPSocket = INVALID_SOCKET;
            //putsUART("\n\rTimeout:TSM_SOCKET_OBTAINED 1");
            OpenADR_TCPState = TSM_DONE;
            InvalidSocketReboot++;
            return;
        }
        // //putsUART("\r\nCould Not Connect...");
        return;
    }

    // Make certain the socket can be written to
    if(TCPIsPutReady(MyTCPSocket) < 125u)
    {
        //putsUART("\r\nCannot write yet...");
        if(TickGet() - TCP_Timeout > 5*TICKS_PER_SECOND)
            {
            // Close the socket so it can be used by other modules
            TCPDisconnect(MyTCPSocket);
            MyTCPSocket = INVALID_SOCKET;
            //putsUART("\n\rTimeout:TSM_SOCKET_OBTAINED 2");
            OpenADR_TCPState = TSM_DONE;
            return;
            }
        return;
    }
    TCP_Timeout = TickGet();
      OpenADR_TCPState++;
}

#if defined(STACK_USE_SSL_CLIENT)
void HandleTSMSSLStart( void )
{
            if(TickGet() - TCP_Timeout > 5*TICKS_PER_SECOND)
            {
                // Close the socket so it can be used by other modules
                TCPDisconnect(MyTCPSocket);
                MyTCPSocket = INVALID_SOCKET;
                //putsUART("\n\rTimeout:TSM_SSL_START");
                OpenADR_TCPState = TSM_DONE;
                return;
            }
            if(TCPStartSSLClient(MyTCPSocket,NULL))
            {
                   //putsUART("\n\rStartSSLClient:");
                TCP_Timeout = TickGet();
                OpenADR_TCPState++;
                // Need to put timeout here....
            }
}

void HandleTSMSSLHandShake( void )
{
            if(TickGet() - TCP_Timeout > 60*TICKS_PER_SECOND)
            {
                    // Close the socket so it can be used by other modules
                    TCPDisconnect(MyTCPSocket);
                    MyTCPSocket = INVALID_SOCKET;
                    //putsUART("\n\rTimeout:TSM_SSL_HANDSHAKE");
                    OpenADR_TCPState = TSM_DONE;
                    return;
            }
            if(!TCPSSLIsHandshaking(MyTCPSocket))
            {
                //putsUART("\n\rStartSSLHandshaking:");
                *payloadptr='\0';   // Append a Null to the end of the TCP Buffer
                payLoadBufCtr=0;
                OpenADR_TCPState++;
                TCP_Timeout = TickGet();
            }
}
#endif

void HandleTSMWriteSocket()
{
    //putsUART("\r\n HandleTSMWriteSocket");

    // Reset any header data that will be received
    ResetHeaderData();

    if(TickGet() - TCP_Timeout > 5*TICKS_PER_SECOND)
    {
        // Close the socket so it can be used by other modules
        TCPDisconnect(MyTCPSocket);
        MyTCPSocket = INVALID_SOCKET;
        //putsUART("\n\rTimeout:TSM_WRITE_SOCKET");
        OpenADR_TCPState = TSM_DONE;
        return;
    }

    // Send out appropriate GET or POST
    if (OpenADR_MsgState == OADR_GET_EVENTSTATE)
    {
        // Write out the appropriate command or url
        OpenADRWriteGetSocket();
    }
    else if (OpenADR_MsgState == OADR_POST_EVENTSTATECONFIRM)
    {
        // Write out the appropriate command or url
        OpenADRWriteConfirmSocket();
    }
#if defined(PGE)
    else if (OpenADR_MsgState == OADR_POST_FEEDBACK)
    {
        // Write out the appropriate command or url
        OpenADRWriteFeedBackSocket();
    }
    else if (OpenADR_MsgState == OADR_POST_OPTOUT)
    {
        // Write out the appropriate command or url
        OpenADRWriteOptOutSocket();
    }
#endif
    // Send the packet
    ////putsUART("\r\n TCPFlush");
    TCPFlush(MyTCPSocket);
    TCP_Timeout = TickGet();

    payloadptr=TCPpayload_buf;
    *payloadptr = '\0';
    payLoadBufCtr = 0;

    FoundHTTPHeader = 0;
    FoundHTTPBody = 0;

    OpenADR_TCPState++;
}

static char HTTPString[] = "HTTP";
static char SpaceString[] = " ";
static char DateString[] = "Date:";

char    HttpLocation[50];
int     LengthOfContent;

// Place descired HTTP Headers into HttpHdrTbl
// Wait until all of the response has been received
// Add search and save of HTTP response status code and search for body
// Obtain the HTTP response. For instance, 404 OK etc. This will help with error handling.
void HandleTSMProcessResponse( void )
{
    BYTE    i;
    WORD    w;
    BYTE    vBuffer[9];
    //char    *buffstart;   // Used to tell when to set flag below.
    int     y, n;
    char    *tmpstr;
    DWORD   tempTick;
    char    reason[20];


    //putsUART("\r\n HandleTSMProcessResponse");

    // !!! Doesn't appear that G2H closes the socket
    // JK 5/25/2012 NOTE - Not sure above statement is valid for OpenADR.
    // I have never seen the code below execute. 
    if(TickGet() - TCP_Timeout > 20*TICKS_PER_SECOND)
    {
            // Close the socket so it can be used by other modules
            TCPDisconnect(MyTCPSocket);
            TCPClose(MyTCPSocket);
            MyTCPSocket = INVALID_SOCKET;
            //putsUART("\n\rTimeout:TSM_PROCESS_RESPONSE");
            OpenADR_TCPState = TSM_PARSE_RESPONSE;
            TCP_Timeout = TickGet();
            return;
    }

    // Connection close was sent. So if the connection gets closed then the server is done sending.
    if(!TCPIsConnected(MyTCPSocket))
    {
        OpenADR_TCPState = TSM_PARSE_RESPONSE;

        TCP_Timeout = TickGet();
        // Do not break;  We might still have data in the TCP RX FIFO waiting for us

    }

    // Get the Response Status-Line. See RFC 2616, section 6.
    // Get and store the HTTP response status code
    if ( !FoundHTTPHeader )
    {
        w = TCPIsGetReady(MyTCPSocket);
        tempTick = TickGet();       //save current ticks for later

        while (w && !FoundHTTPHeader)
        {
            // Get one character
            w -= TCPGetArray(MyTCPSocket, vBuffer, 1);

            // Check if response is too large
            if(payLoadBufCtr<TCPPAYLOAD_BUF_SIZE-2)
            {
                *payloadptr=vBuffer[0];
                payloadptr++;
                payLoadBufCtr++;
            }

            // Check if found end of line
            if (vBuffer[0] == '\n')
            {
                // Process the Status and reset the variables
                FoundHTTPHeader = 1;
                HTTPResponse = -1;

                // Find the status and store it
                *payloadptr = '\0';                // Null terminate the string
                tmpstr = TCPpayload_buf;
                tmpstr = strstr( tmpstr, HTTPString );
                if ( !tmpstr )
                {
                    HTTPResponse = -1;
                }
                else
                {
                    tmpstr = strstr( tmpstr, SpaceString );
                    n = sscanf( tmpstr, " %d %s\r\n", &HTTPResponse, reason );
                }

                // Reset variables
                payloadptr=TCPpayload_buf;
                *payloadptr = '\0';
                payLoadBufCtr = 0;

                // Reset variables for finding the HTTP body
                FoundHTTPBody = 0;

            }
        }
    }

    // Get headers and look for specific headers
    // If blank line encountered then move onto message-body
    if ( FoundHTTPHeader && !FoundHTTPBody )
    {
      w = TCPIsGetReady(MyTCPSocket);

        while (w && !FoundHTTPBody)
        {
            // Get one character
            w -= TCPGetArray(MyTCPSocket, vBuffer, 1);

            // Check if response is too large
            if(payLoadBufCtr<TCPPAYLOAD_BUF_SIZE-2)
            {
                *payloadptr=vBuffer[0];
                payloadptr++;
                *payloadptr = '\0';
                payLoadBufCtr++;
            }

            // Check if found blank line. This is the start of payload
            tmpstr = payloadptr - 4;
            if ( strcmp(tmpstr,"\r\n\r\n") == 0)
            {
                FoundHTTPBody = 1;

                GMTstr[0] = '0'; GMTstr[1] = '0'; GMTstr[2] = '0'; GMTstr[3] = '\0';
                // Process Date field only during the GET EventState
                if (OpenADR_MsgState == OADR_GET_EVENTSTATE)
                {
                    // Check for serve time in HTTP header
                    if ((tmpstr = strstr( TCPpayload_buf, DateString )))
                    {
                        // Parse time and convert to UTC time
                        UTCServerTime = RFC882time(tmpstr);
                        sprintf(GMTstr, "%s\r\n", strtok(tmpstr,"\r"));
                        //check for valid server time and update 
                        if (UTCServerTime)
                        {
                            // Last good Server time and ticks used to update UTCTime between server updates
                            UTCLastServerTime = UTCServerTime;  // store last good time for calcs between update
                            LastServerTicks = tempTick;
    
                            UTCTime = UTCServerTime;
                        }
                        else
                        {
                            sprintf(pbuf, "Bad Server Time:\r\n%s\r\n",GMTstr);
                            //putsUART(pbuf);
                        }
                    }
                }
                

                // Reset variables next header processing
                payloadptr=TCPpayload_buf;
                *payloadptr = '\0';
                payLoadBufCtr = 0;

            } // else, not blank line

        } // while (w)
    }

    // If found the begining of the body then
    //  go ahead and save it for processing
    if ( FoundHTTPBody )
    {
        // Get count of RX bytes waiting
        w = TCPIsGetReady(MyTCPSocket);
        // Obtain and print the server reply
        i = sizeof(vBuffer)-1;
        vBuffer[i] = '\0';

        while(w)
        {
            if(TickGet() - TCP_Timeout > 20*TICKS_PER_SECOND)
            {
                // Close the socket so it can be used by other modules
                TCPDisconnect(MyTCPSocket);
                MyTCPSocket = INVALID_SOCKET;
                //putsUART("\n\rTimeout:TSM_PROCESS_RESPONSE");
                OpenADR_TCPState = TSM_DONE;
                break;
            }

            if(w < i)
            {
                i = w;
                vBuffer[i] = '\0';
            }

            // Reads up to i bytes
            // Places the data in vBuffer
            // returns number of bytes actually read
            w -= TCPGetArray(MyTCPSocket, vBuffer, i);

            // Copy the bytes over into our buffer
            for (y = 0; y < i; y++)
            {
                 // Check for overflow
                 // OpenADR messages are roughly 300 + (1500 per eventState)
                if(payLoadBufCtr<TCPPAYLOAD_BUF_SIZE-2)
                {
                    *payloadptr=vBuffer[y];
                    payloadptr++;
                    payLoadBufCtr++;
                }
            }
        }

        if (OpenADR_TCPState == TSM_PARSE_RESPONSE)
        {
            sprintf(pbuf, "\r\n Received %d bytes from Server", payLoadBufCtr);
            //putsUART(pbuf);
            // Terminate string
            *payloadptr = '\0';
            payLoadBufCtr = 0;
        }

    }

}


// Now that the response has been received,
//  process the response
void HandleTSMParseResponse()

{
    switch(OpenADR_MsgState)
    {
        case OADR_GET_EVENTSTATE:
        // Check for valid EventState from server
        // Parse and store Events in Queue
            if (OpenADRParseGETResponse())
            {
                // Good EventStates received, so open another socket 
                // to POST the EventStateConfirm.
    
                #if defined(PGE)
                    OpenADR_MsgState = OADR_POST_FEEDBACK;
                #else
                    OpenADR_MsgState = OADR_POST_EVENTSTATECONFIRM;
                #endif

                OpenADR_TCPState = TSM_OPEN_SOCKET;
                TCP_Timeout=TickGet();

            }
            else
            {
                // HTTP error
                OpenADR_MsgState = OADR_MSG_IDLE;
                OpenADR_TCPState = TSM_DONE;
            }
            break;

        case OADR_POST_EVENTSTATECONFIRM:
            // No real processing here. Just reset the buffer
            OpenADRParsePOSTResponse();

            OpenADR_MsgState = OADR_MSG_IDLE;
            OpenADR_TCPState = TSM_DONE;
            break;

    #if defined(PGE)
        case OADR_POST_FEEDBACK:
            // No real processing here. Just reset the buffer
            OpenADRParseFeedBackResponse();

            // If MCI has signaled a OverRide Event then send a OptOut message 
            if (OptOutEvent)
            {
                OpenADR_MsgState = OADR_POST_OPTOUT;
                OpenADR_TCPState = TSM_OPEN_SOCKET;
                TCP_Timeout=TickGet();
            }
            else
            {
                OpenADR_MsgState = OADR_MSG_IDLE;
                OpenADR_TCPState = TSM_DONE;
            }
            break;

        case OADR_POST_OPTOUT:
            // No real processing here. Just reset the buffer
            OpenADRParseOptOutResponse();

            OpenADR_MsgState = OADR_MSG_IDLE;
            OpenADR_TCPState = TSM_DONE;
            OptOutEvent = 0;
            break;
    #endif
        default:
            break;
    }

}

// Shutdown state machine
void HandleTSMDone( void )
{
    int        qSize=0;
    int        pcount;                    // generic loop counters

    qSize=0;
    for (pcount = 0; pcount < OPENADR_QSIZE; pcount++)
    {
        if(eventStateQ[pcount].magicAddress == MAGIC_OPENADRQ_FLASH_VALUE)
        {
            qSize++;
        }
    }
    sprintf(pbuf,"\r\nDONE Q Size=%d\r\n", qSize);
    //putsUART(pbuf);

    InvalidSocketReboot=0;
    if(resetModule==1)
    {
        Reset();
    }

    OpenADR_MsgState = OADR_MSG_IDLE;

    OpenADR_TCPState = TSM_HOME;

}

void    ResetHeaderData( void )
{
    LengthOfContent = 0;
    HttpLocation[0] = '\0';
}

/*****************************************************************************
  OpenADR specific support routines for HandleTSMWriteSocket()
    void OpenADRWriteSocket()
        Description:
            Send out the OpenADR GET request

  ***************************************************************************/

// Companion function to HandleTSMWriteSocket()
// This function sends out the OpenADR request

void OpenADRWriteGetSocket()
{
    char    Buffer[150];

    // EPRI Default GET format with authentication
    // Setup SourceUnamePw format prior to encoding as base64
    strcpy(SourceUnamePw, drasClientID);
    strcat(SourceUnamePw, ":");
    strcat(SourceUnamePw, drasClientIDPW);

    // Base64 encode
    Base64Encode((BYTE *)SourceUnamePw, strlen(SourceUnamePw), (BYTE *)EncodedUNPW, 74);

    //********************************************
    // Begin assembly of GET string
    //********************************************

    strcpy(Buffer, "GET ");
    strcat(Buffer, EventStatePage);
#if defined(PGE)
    strcat(Buffer, "?deviceID=");
    strcat(Buffer, drasClientID);
#endif
    TCPPutString(MyTCPSocket, (BYTE*)Buffer);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)" HTTP/1.1\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Authorization: Basic ");
    TCPPutString(MyTCPSocket, (BYTE *)EncodedUNPW);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Host: ");
    TCPPutString(MyTCPSocket, (BYTE*)OpenADRServerName);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"\r\nConnection: close\r\n\r\n");

}

void OpenADRWriteConfirmSocket()
{
    char    Buffer[50];
    int    len;
    char    lenbuf[10];

    // Start formatting HTTP header
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"POST ");
    strcpy(Buffer, EventStateConfirmPage);
    TCPPutString(MyTCPSocket, (BYTE*)Buffer);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)" HTTP/1.1\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Authorization: Basic ");
    TCPPutString(MyTCPSocket, (BYTE *)EncodedUNPW);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Host: ");
    TCPPutString(MyTCPSocket, (BYTE*)OpenADRServerName);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Content-Type: application/xml\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Connection: close\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Accept: */*\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Content-Length:");

    // Put XML to send in TCPPutBuffer
    // Write out the length of XML, then the XML
    len = FormatEventStateConfirmXML(TCPPutBuffer);
    sprintf(lenbuf, "%d\r\n\r\n", len);
    TCPPutString(MyTCPSocket, (unsigned char *)lenbuf);        // lenbuf includes two cr lf
    TCPPutString(MyTCPSocket, (BYTE*)TCPPutBuffer);

}

static    char    xmlHeaderTemplate[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";

// Put the Edev xml inside of Buffer
// Return the length of buffer
int FormatEventStateConfirmXML( char *Buffer )
{
    strcpy(Buffer, xmlHeaderTemplate);
    strcat(Buffer, "<p:eventStateConfirmation ");

    strcat(Buffer, "currentTime=\"31.0\" ");

    strcat(Buffer, "drasClientID=\"");
    strcat(Buffer, drasClientID);
    strcat(Buffer, "\" ");

    strcat(Buffer, "eventIdentifier=\"");
    strcat(Buffer, eventIdentifier);
    strcat(Buffer, "\" ");

    strcat(Buffer, "eventModNumber=\"");
    strcat(Buffer, eventModNumber);
    strcat(Buffer, "\" ");

    strcat(Buffer, "eventStateID=\"");
    strcat(Buffer, eventStateID);
    strcat(Buffer, "\" ");

    strcat(Buffer, "operationModeValue=\"");
    strcat(Buffer, operationModeValue);
    strcat(Buffer, "\" ");

    /*
    if (OverRide)
        strcat(Buffer, "optInStatus=\"false\" ");
    else
        strcat(Buffer, "optInStatus=\"true\" ");
    */

    strcat(Buffer, "programName=\"");
    strcat(Buffer, programName);
    strcat(Buffer, "\" ");

    strcat(Buffer, "schemaVersion=\"\" ");
    strcat(Buffer, "xmlns:p=\"http://www.openadr.org/DRAS/EventState\" ");
    strcat(Buffer, "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ");
    strcat(Buffer, "xsi:schemaLocation=\"http://openadr.lbl.gov/src/EventState.xsd\">");

    strcat(Buffer, "</p:eventStateConfirmation>");

    return strlen(Buffer);

}

#define XMLcat(Buf,Name,Value)   strcat(Buf,"<"); strcat(Buf,Name); strcat(Buf,">"); \
                                 strcat(Buf,Value);                                  \
                                 strcat(Buf,"</"); strcat(Buf,Name); strcat(Buf,">");

//#define XMLcat(Buf,Name,Value)   strcat(Buf,"<""Name"">");strcat(Buf,Value); strcat(Buf,"</""Name"">");

#define XMLFBcat(Buf,Name,Value) strcat(Buf,"<ns0:feedback><ns0:name>");  \
                                 strcat(Buf,Name);                        \
                                 strcat(Buf,"</ns0:name><ns0:value>");    \
                                 strcat(Buf,Value);                       \
                                 strcat(Buf,"</ns0:value></ns0:feedback>");

#if defined(PGE)

/*****************************************************************************
  PGE specific Feedback message
    void OpenADRWriteFeedBackSocket()

  ***************************************************************************/
void OpenADRWriteFeedBackSocket()
{

    int     len;
    char    lenbuf[10];
    char    strLoadFactor[] = "000";
    struct  tm *gmttm;
    char    strCurrentTime[30];

    //*************************************************************************
    // Process HTTP POST Header
    //*************************************************************************

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"POST ");
    TCPPutString(MyTCPSocket, (BYTE*)FeedBackPage);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)" HTTP/1.1\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Host: ");
    TCPPutString(MyTCPSocket, (BYTE*)OpenADRServerName);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Content-Type: application/xml\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Connection: close\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Content-Length:");

    //*************************************************************************
    // Process Feedback XML first and save in TCPPutBuffer
    // XML length needed for HTTP header "Content-Length:"
    //*************************************************************************

    sprintf(strLoadFactor, "%d", LoadFactor);

    // Build XML message in TCPPutBuffer
    strcpy(TCPPutBuffer, xmlHeaderTemplate);
    strcat(TCPPutBuffer, "<ns0:ListOfFeedback xmlns:ns0=\"http://www.openadr.org/DRAS/Feedback\">\r\n");
    strcat(TCPPutBuffer, "<ns0:FeedBack>");
        XMLcat(TCPPutBuffer, "ns0:programName", "CI");
        XMLcat(TCPPutBuffer, "ns0:participantID", drasClientID);
        XMLcat(TCPPutBuffer, "ns0:DRASClientID", drasClientID);
        XMLcat(TCPPutBuffer, "ns0:eventID", drasClientID);

        gmttm = localtime(&UTCTime);
        sprintf(strCurrentTime, "%04d-%02d-%02dT%02d:%02d:%02dZ", ((gmttm->tm_year)+1900), \
                                                            ((gmttm->tm_mon)+1),     \
                                                              gmttm->tm_mday,        \
                                                              gmttm->tm_hour,        \
                                                              gmttm->tm_min,         \
                                                              gmttm->tm_sec);
        XMLcat(TCPPutBuffer, "ns0:timeStamp", strCurrentTime);

        strcat(TCPPutBuffer, "<ns0:feedback>");
            XMLcat(TCPPutBuffer, "nso:name", "SHED_AVAILABLE");
            XMLcat(TCPPutBuffer, "nso:value", strLoadFactor);
        strcat(TCPPutBuffer, "</ns0:feedback>");
    strcat(TCPPutBuffer, "</ns0:FeedBack></ns0:ListOfFeedback>");

    // Write out the length of XML, then the XML
    len = strlen(TCPPutBuffer);
    sprintf(lenbuf, "%d\r\n\r\n", len);
    TCPPutString(MyTCPSocket, (unsigned char *)lenbuf);        // lenbuf includes two cr lf
    TCPPutString(MyTCPSocket, (BYTE*)TCPPutBuffer);

}
#endif

#if defined(PGE)

/*****************************************************************************
  PGE specific OptOut message
    void OpenADRWriteOptOutSocket()

  ***************************************************************************/
void OpenADRWriteOptOutSocket()
{

    int     len;
    char    lenbuf[10];

    //*************************************************************************
    // Process HTTP POST Header
    //*************************************************************************

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"POST ");
    TCPPutString(MyTCPSocket, (BYTE*)OptOutPage);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)" HTTP/1.1\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Host: ");
    TCPPutString(MyTCPSocket, (BYTE*)OpenADRServerName);
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"\r\n");

    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Content-Type: application/xml\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Connection: close\r\n");
    TCPPutROMString(MyTCPSocket, (ROM BYTE*)"Content-Length:");

    //*************************************************************************
    // Process OptOutState XML first and save in TCPPutBuffer
    // XML length needed for HTTP header "Content-Length:"
    //*************************************************************************

    strcpy(TCPPutBuffer, xmlHeaderTemplate);
    strcat(TCPPutBuffer, "<ns0:ListOfOptOutStates xmlns:ns0=\"http://www.openadr.org/DRAS/OptOutState\">\r\n");
        strcat(TCPPutBuffer, "<ns0:optOutState>");
            XMLcat(TCPPutBuffer, "ns0:participantID", drasClientID);
            XMLcat(TCPPutBuffer, "ns0:eventID", ActiveEventID);
            strcat(TCPPutBuffer, "<ns0:DRASClients>");
                XMLcat(TCPPutBuffer, "ns0:DRASClientID", drasClientID);
            strcat(TCPPutBuffer, "</ns0:DRASClients>");
            strcat(TCPPutBuffer, "<ns0:schedule>");
                strcat(TCPPutBuffer, "<ns0:timeElement>");
                        XMLcat(TCPPutBuffer, "ns0:start", startTime);
                        XMLcat(TCPPutBuffer, "ns0:end", endTime);
                strcat(TCPPutBuffer, "</ns0:timeElement>");
            strcat(TCPPutBuffer, "</ns0:schedule>");
        strcat(TCPPutBuffer, "</ns0:optOutState>");
    strcat(TCPPutBuffer, "</ns0:ListOfOptOutStates>");

    // Write out the length of XML, then the XML
    len = strlen(TCPPutBuffer);
    sprintf(lenbuf, "%d\r\n\r\n", len);
    TCPPutString(MyTCPSocket, (unsigned char *)lenbuf);        // lenbuf includes two cr lf
    TCPPutString(MyTCPSocket, (BYTE*)TCPPutBuffer);

}
#endif

static char *xmlHeader = "xml";
static char *leftDelimeter = "<";
static char *rightDelimeter = ">";
static char *QuoteDelimeter = "\"";
static char *eventStatesStartTag = "eventStates ";
static char *eventStatesEndTag = "eventStates>";
static char *listOfEventStateText = "listOfEventState";

/*****************************************************************************
  OpenADR specific support routines for HandleTSMParseResponse()
    int OpenADRParseGETResponse()
        Description:
            Now that the response is received, process it.

  ***************************************************************************/
int OpenADRParseGETResponse()
{
    char *strptr;

    // int  i;

    //putsUART("\r\n OpenADRParseGETResponse");
    // Check for an HTTP error.
    // This could be because of a broken link
    if ((HTTPResponse<200) || (HTTPResponse>299))
    {
        //putsUART("\r\n OpenADRParseGETResponse - Bad HTTP");
        return 0;
    }

    // TCPpayload_buf is initialized in HandleTSMProcessResponse
    //  by use of payloadptr
    if ( !(strptr = strstr(TCPpayload_buf, xmlHeader)) || !(strptr = strstr(strptr, leftDelimeter)) )
    {
        //putsUART("\n\r Invalid XML Header");
        return 0;
    }

    // Should be pointing at command response
    //  Move past the left delimeter
    strptr++;

    // Check for various xml headers
    // !!! Ubilogix put one entry. How to handle generically?
    if ( *strptr == '!' )
    {
        strptr = strstr(strptr, leftDelimeter);
        strptr++;
    }

    // If name space characters are present then move strptr past them
    strptr = MovePastNameSpace(strptr);

    // Ok, now we should be pointing at the response
    if ( !strncmp(strptr, listOfEventStateText, strlen(listOfEventStateText)) )
        HandleListOfEventStateXML(strptr);
    else
        // PGE doesn't use "ListOfEventState" tag
        HandleEventStatesXML(strptr);

    //putsUART("\r\n Process of eventStates complete");

    TCPpayload_buf[0]='S';//Reset array since srtcat looks for null and starts appending.  This will blow out array size since syspwd message is 292 chars.
    TCPpayload_buf[1]='T';
    TCPpayload_buf[2]='\0';

    // JL edit
    // If we have gotten this far, then we have valid comm with the OpenADR server. We
    // need to send a Connection Status packet to the device.
    commGood = 1;
    // reset OpenADR comm status timer
    CommTimer = TickGet();
    RsBuf = TransmitCommGoodPacket();
    if (RsBuf.AppResponseValid && RsBuf.AppResponse[4] == 0x03 && RsBuf.AppResponse[5] == OutsideCommGood[4])
    // response 0x03, 0x0E = good APP ACK means SGD is functional
        strcpy(strAvailabilityStatus, "TRUE");
    else
        strcpy(strAvailabilityStatus, "FALSE");

    return 1;

}

int OpenADRParsePOSTResponse()
{
    //putsUART("\r\n OpenADRParsePOSTResponse");
//    //putsUART("\r\n Server ACK'd the eventStateConfirmation");
    // !!!
    // Should inspect OpenADR_MsgState and parse accordingly

    TCPpayload_buf[0]='S';//Reset array since srtcat looks for null and starts appending.  This will blow out array size since syspwd message is 292 chars.
    TCPpayload_buf[1]='T';
    TCPpayload_buf[2]='\0';

    return 1;
}

#if defined(PGE)

int OpenADRParseFeedBackResponse()
{
    // !!!
    // Should inspect OpenADR_MsgState and parse accordingly

    TCPpayload_buf[0]='S';//Reset array since srtcat looks for null and starts appending.  This will blow out array size since syspwd message is 292 chars.
    TCPpayload_buf[1]='T';
    TCPpayload_buf[2]='\0';

    return 1;
}

int OpenADRParseOptOutResponse()
{
    // !!!
    // Should inspect OpenADR_MsgState and parse accordingly

    TCPpayload_buf[0]='S';//Reset array since srtcat looks for null and starts appending.  This will blow out array size since syspwd message is 292 chars.
    TCPpayload_buf[1]='T';
    TCPpayload_buf[2]='\0';

    return 1;
}
#endif

void HandleListOfEventStateXML( char *strptr )
{
    char    *startptr, *endptr;
    int     eventStateIndex = 0;

    startptr = strptr;
    endptr = strptr;

    // Iterate all eventStates
    // Must find start and end tag to be considered a valid event
    while ( (eventStateIndex <= OPENADR_QSIZE) && (startptr = strstr(startptr, eventStatesStartTag)) && endptr)
    {
        // terminate end
        endptr = strstr(startptr, eventStatesEndTag);
        endptr = strstr(endptr, ">");
        if (endptr){
            *endptr = '\0';
            endptr++;
            // Store events in queue
            HandleEventStatesXML(startptr);
            eventStateIndex += 1;
            startptr = endptr;
        }
    }

}

char *operationModeScheduleText = "operationModeSchedule";
char *eventStatusText = "EventStatus";
char *operationModeValueText = "OperationModeValue";
char *eventStateIDText = "eventStateID";
char *eventIdentifierText = "eventIdentifier";
char *eventModNumberText = "eventModNumber";
char *programNameText = "programName";
char *currentTimeText = "currentTime";
char *drEventDataText = "drEventData";

char *noteTimeText = "notificationTime";
char *startTimeText = "startTime";
char *endTimeText = "endTime";

char *PriceAbsoluteText = "PRICE_ABSOLUTE";
char *LoadAmountText = "LOAD_AMOUNT";
char *PriceMultipleText = "PRICE_MULTIPLE";
char *eventIIText = "eventInfoInstances";
char *valueText = "value";
char *timeOffsetText = "timeOffset";
char *modeTimeSlotText = "modeTimeSlot";

// Handle the reception of the eventStates XML
void HandleEventStatesXML( char *strptr )
{
    int     i, j;

    char    *tmpptr;
    int     drEventDataNotFound = 0;
    float   fPriceAbs, fLoadAmt, fPriceMult;
    time_t  notificationtime, starttime, endtime, duration;
    int     ieventModNumber;
    int     iCurrentTime;

    char    tmPnotificationTime[40];
    char    tmpdrValue[10];

        tmpptr = strptr;
    
        // Check for drEventData and mark its end
        tmpptr = strptr;
        tmpptr = strstr(strptr, drEventDataText);
        if ( tmpptr )
        {
            tmpptr++;
            tmpptr = strstr(tmpptr, drEventDataText);
            if ( tmpptr )
                *tmpptr = 0;
            else
                drEventDataNotFound = 1;
        }
        else
            drEventDataNotFound = 1;

        // Parse out the mandatory stuff
        if ( !TagAndString(strptr, eventStatusText, eventStatus, 19, 0) ) strcpy(eventStatus, "NONE");
        if ( !TagAndString(strptr, eventStateIDText, eventStateID, 19, 1) ) eventStateID[0] = '\0';
        if ( !TagAndString(strptr, eventIdentifierText, eventIdentifier, 39, 1) ) eventIdentifier[0] = '\0';
        if ( !TagAndString(strptr, programNameText, programName, 19, 1) ) programName[0] = '\0';
        if ( !TagAndString(strptr, currentTimeText, currentTime, 19, 0) ) strcpy(currentTime, "0.0");
        if ( !TagAndString(strptr, modeTimeSlotText, modeTimeSlot, 19, 0) ) strcpy(modeTimeSlot, "0");
        if ( !TagAndString(strptr, timeOffsetText, timeOffset, 19, 0) ) strcpy(timeOffset, "0");

        if ( !TagAndString(strptr, eventModNumberText, eventModNumber, 19, 1) )
            eventModNumber[0] = '\0';
        else
            sscanf(eventModNumber, "%d", &ieventModNumber);

#if defined(PGE)
        // PGE - no modeslots    
        if ( !TagAndString(strptr, operationModeValueText, operationModeValue, 19, 0) )strcpy(operationModeValue, "NORMAL");
#else
        // To get LOAD_LEVEL (aka <OperationModeValue>, you have to index to first <modeslot>
       if ( (tmpptr = strstr(strptr, operationModeScheduleText)) )
            if ( !TagAndString(tmpptr, operationModeValueText, operationModeValue, 19, 0) )strcpy(operationModeValue, "NORMAL");
#endif

        // Parse out the optional smart client data in drEventData
        if ( !drEventDataNotFound )
        {
            if ( !TagAndString(strptr, noteTimeText, tmPnotificationTime, 39, 0) ) tmPnotificationTime[0] = '\0';
            if ( !TagAndString(strptr, startTimeText, startTime, 39, 0) ) startTime[0] = '\0';
            if ( !TagAndString(strptr, endTimeText, endTime, 39, 0) ) endTime[0] = '\0';
    
            // Find <PRICE_ABSOLUTE>, then retrieve <value>
            fPriceAbs = 0.0;
            if ( (tmpptr = strstr(strptr, PriceAbsoluteText)) )
                if ( TagAndString(tmpptr, valueText, tmpdrValue, 9, 0) )
                    sscanf(tmpdrValue, "%f", &fPriceAbs);

            // Find <LOAD_AMOUNT>, then retrieve <value>
            fLoadAmt = 0.0;
            if ( (tmpptr = strstr(strptr, LoadAmountText)) )
                if ( TagAndString(tmpptr, valueText, tmpdrValue, 9, 0) )
                    sscanf(tmpdrValue, "%f", &fLoadAmt);

            // Find <PRICE_MULTIPLE>, then retrieve <value>
            fPriceMult = 0.0;
            if ( (tmpptr = strstr(strptr, PriceMultipleText)) )
                if ( TagAndString(tmpptr, valueText, tmpdrValue, 9, 0) )
                    sscanf(tmpdrValue, "%f", &fPriceMult);
        }

    // Check if need to process
#if defined(PGE)
        // PGE - operationModeValue not correct until EventStatus = ACTIVE   
    if ( strcmp(eventStatus, "ACTIVE") == 0 && strcmp(eventIdentifier, "\0") != 0)
#else
    if ( strcmp(eventStatus, "NONE") != 0 && strcmp(eventIdentifier, "\0") != 0)
#endif
    {

        // Check to see if event is already in queue and needs updating.
        // if not, j points to first empty slot
        // queue slots are cleared in ManageOpenADRQueue();
        j = OPENADR_QSIZE;
        for (i = 0; i < OPENADR_QSIZE; i++)
        {
            if (strcmp(eventIdentifier, eventStateQ[i].eventIdentifier) == 0)
            // event in queue
            {
                if (ieventModNumber != eventStateQ[i].eventModNumber)
                // event has changed
                    j = i;
                else
                    j = OPENADR_QSIZE;
            i = OPENADR_QSIZE;      // force loop exit
            }
            else
                // look for first empty slot
                if ( j == OPENADR_QSIZE && eventStateQ[i].magicAddress != MAGIC_OPENADRQ_FLASH_VALUE)
                    j = i;
        }
        
        if (j < OPENADR_QSIZE)
        {            
    
            notificationtime = ISO8601time (tmPnotificationTime);
            starttime = ISO8601time (startTime);
            endtime = ISO8601time (endTime);
            duration = endtime - starttime;
    
            if ((starttime > 0) && (endtime > 0) && (starttime < endtime) && (endtime > UTCTime))
            {
                // Found active event, now we can determine local time
                #if defined(PGE)
                    Local_Time = notificationtime;
                #else
                    sscanf(currentTime, "%d", &iCurrentTime);
                    Local_Time = starttime + tzsec + iCurrentTime;
                #endif

                eventStateQ[j].magicAddress = MAGIC_OPENADRQ_FLASH_VALUE;

                strcpy(eventStateQ[j].eventStateID, eventStateID);
                strcpy(eventStateQ[j].eventIdentifier, eventIdentifier);
                strcpy(eventStateQ[j].programName, programName);
                strcpy(eventStateQ[j].eventStatus, eventStatus);
                strcpy(eventStateQ[j].LoadLevel, operationModeValue);
                eventStateQ[j].eventModNumber = ieventModNumber;

                eventStateQ[j].notificationtime = notificationtime;
                #if defined(PGE)
                    eventStateQ[j].starttime = starttime + PGE_DELAY_SECONDS;
                    eventStateQ[j].endtime = endtime + PGE_DELAY_SECONDS;
                #else
                    eventStateQ[j].starttime = starttime;
                    eventStateQ[j].endtime = endtime;
                #endif
                eventStateQ[j].duration = duration;

                eventStateQ[j].PriceAbsolute = fPriceAbs;
                eventStateQ[j].PriceMultiple = fPriceMult;
                eventStateQ[j].LoadAmount = fLoadAmt;

                eventStateQ[j].PriceRelative = -1.1;
                eventStateQ[j].LoadPercentage = -1.1;
                eventStateQ[j].GridReliability = -1.1;

            }

        //sprintf(pbuf, "\r\n*** %s\n Storing %d, %s\r\n", ctime(&UTCTime), j, eventStateQ[j].eventIdentifier);
        //putsUART(pbuf);

        }

    }

//    OpenADR_MsgState = OADR_POST_EVENTSTATECONFIRM;

}

// Manage OpenADR event Queue
// After processing, queue entry gets cleared
void ManageOpenADRQueue()
{
    int     i, j;
    INT16   tmpd, dPrice;
    float   tmpf;
    unsigned char    timeByteMCI, priceByteMCI;


    // Queue not guarenteed to be in time-order - check all
    for (i = 0; i < OPENADR_QSIZE; i++)
    {
        if (eventStateQ[i].eventIdentifier[0] != '\0' && (UTCTime >= eventStateQ[i].starttime))
        {
            // Store active event for use in OptOut message
            strcpy (ActiveEventID, &eventStateQ[i].eventIdentifier[0]);

            if ( !eventStateQ[i].executing && !eventStateQ[i].executed)
            {

                //sprintf(pbuf, "\r\n*** %s\n Executing %d, %s\r\n", ctime(&UTCTime), i, eventStateQ[i].eventIdentifier);
                //putsUART(pbuf);

                // Determine the type of command
                if (eventStateQ[i].PriceAbsolute == 0.0 )
                    CommandType = LOAD_LEVEL_COMMAND;
                else
                    CommandType = PRICE_ABSOLUTE_COMMAND;
    
                switch (CommandType)
                {
                    case LOAD_LEVEL_COMMAND:
    
                            // Need to solve a quadratic equation.
                            tmpf = (float) eventStateQ[i].duration / 2.0;
                            if (tmpf >= 0.0)
                                tmpf = sqrtf(tmpf);
                            else
                                tmpf = 0.0;
    
                            tmpf += 0.5;
                            // Bounds check. 255 = too big, 0 reserved for unknown time
                            if (tmpf >= 255.0)
                                tmpd = 255;
                            else if (tmpf < 1.0)
                                tmpd = 1;
                            else
                                tmpd = (int)tmpf;

                            timeByteMCI = (unsigned char) tmpd;
    
                        // Check how much to shed
                        if ( !strcmp(eventStateQ[i].LoadLevel, "MODERATE" ) )
                        {
                            ShedCommand[5] = timeByteMCI;
                            RsBuf = MCISendNeutral(ShedCommand);
                        }
                        else if ( !strcmp(eventStateQ[i].LoadLevel, "HIGH" ) )
                        {
                            CriticalPeakEvent[5] = timeByteMCI;
                            RsBuf = MCISendNeutral(CriticalPeakEvent);
                        }
                        else // normal
                        {
                            RsBuf = MCISendNeutral(EndShedCommand);
                        }
                        break;
    
                    case PRICE_ABSOLUTE_COMMAND:
    
                        // price in cents. Must match "digits after decimal point" SetEnergyPrice[9]
                        dPrice = (int) (eventStateQ[i].PriceAbsolute * 100);
    
                        // Check if simple or smart client                    
                        if ( SupportsPriceAbs )
                        {
                            SetEnergyPrice[4] = (unsigned char) (dPrice & 0xff00) / 256;
                            SetEnergyPrice[5] = (unsigned char) (dPrice & 0xff00);
                            SetEnergyPrice[8] = 0x02;        // digits after decimal point
                            
                            RsBuf = MCISendNeutral(SetEnergyPrice);
                        }
                        else if ( !PriceRelSupportKnown || SupportsPriceRel )
                        {
                            // Need to solve a quadratic equation.
                            tmpf = 8192 * eventStateQ[i].PriceAbsolute;
                            tmpf /= AvgPrice;
                            tmpf += 63;
                            tmpf *= 4;
                            tmpf += 3844;                        
                            if (tmpf >= 0.0)
                                tmpf = sqrtf(tmpf);
                            else
                                tmpf = 0.0;
                            tmpf -= 62;
                            tmpf /= 2;
    
                            tmpf += 0.5;
                            // Bounds check. 255 = too big, 0 reserved for unknown price
                            if (tmpf >= 255.0)
                                tmpd = 255;
                            else if (tmpf < 1.0)
                                tmpd = 1;
                            else
                                tmpd = (int)tmpf;
                            priceByteMCI = (unsigned char) tmpd;
    
                            // Does SGD support Relative Pricing??
                            // Send out the relative price command
                            // Yes = Valid App Ack (0x03,0x07)
                            // No = App NAK, Opcode1 not supported (0x04, 0x01)
                            PresentRelPrice[7] = priceByteMCI;
                            RsBuf = MCISendNeutral(PresentRelPrice);
                            if (RsBuf.AppResponseValid && RsBuf.AppResponse[4] == 0x03 && RsBuf.AppResponse[5] == PresentRelPrice[6])
                            // response 0x03, 0x07 = good APP ACK
                            {
                                RequestedPriceRel = 1;
                                SupportsPriceRel = 1;
                                PriceRelSupportKnown = 1;
                            }
                            else if (RsBuf.AppResponseValid && RsBuf.AppResponse[4] == 0x04 && RsBuf.AppResponse[5] == 0x01)
                            // response 0x04, 0x01 = APP NAK, Opcode1 not supported
                            {
                                RequestedPriceRel = 1;
                                SupportsPriceRel = 0;
                                PriceRelSupportKnown = 1;
                            }
                            else
                            // Unknown support
                            {
                                RequestedPriceRel = 1;
                                SupportsPriceRel = 1;
                                PriceRelSupportKnown = 0;
                            }
                        }
                        if ( !SupportsPriceRel )
                        {
                            if (dPrice < LowPrice)
                            {
                                RsBuf = MCISendNeutral(EndShedCommand);
                            }
                            else if (dPrice >= HighPrice)
                            {
                                RsBuf = MCISendNeutral(CriticalPeakEvent);
                            }
                            else
                            {
                                RsBuf = MCISendNeutral(ShedCommand);
                            }
                        }
                        break;
    
                    case COMMAND_NONE:
                        RsBuf = MCISendNeutral(EndShedCommand);
                    default:
                        break;
                }
    
                // Flag as executing if message was received correctly by SGD
                // Otherwise message will retry next loop
                if (RsBuf.LLResponseValid && RsBuf.AppResponseValid)
                    eventStateQ[i].executing = 1;

            }
            // Check for end condition
            else if ( eventStateQ[i].executing && !eventStateQ[i].executed && (UTCTime >= (eventStateQ[i].endtime)))
            {
                // Clear active event tag
                strcpy (ActiveEventID, "");

                // Check for concurrent events.
                int concurrentEvents = 0;
                for (j = 0; j < OPENADR_QSIZE; j++)
                {
                    if (eventStateQ[i].endtime == eventStateQ[j].starttime)
                        concurrentEvents = 1;
                }
                // No EndShed for concurrent events
                if (!concurrentEvents)
                {
                    RsBuf = MCISendNeutral(EndShedCommand);
                }
 
                eventStateQ[i].executing = 0;
                eventStateQ[i].executed = 1;
            }
            else if (!eventStateQ[i].executing && eventStateQ[i].executed && (UTCTime >= (eventStateQ[i].endtime) + 30))
            // Clear event from queue shortly after endtime
            // Event was being added again after endtime because of server delay
            {

                //sprintf(pbuf, "\r\n*** %s\n Clearing %d, %s\r\n", ctime(&UTCTime), i, eventStateQ[i].eventIdentifier);
                //putsUART(pbuf);
                
                eventStateQ[i].magicAddress = 0;
                eventStateQ[i].eventIdentifier[0] = '\0';
                eventStateQ[i].eventModNumber = 0;
                eventStateQ[i].starttime = 0;
                eventStateQ[i].endtime = 0;
                eventStateQ[i].executed = 0;

            } // end executed
        } // end event active
    } // end for all in queue

}

char *MovePastNameSpace(char *strptr)
{
    int        FoundNS = 0;
    char     *WhiteSpacePtr;
    char    *NSptr;

    NSptr = strptr;
    WhiteSpacePtr = strstr(strptr, " ");
    while ((NSptr != WhiteSpacePtr) && !FoundNS)
    {
        if (*NSptr == ':')    FoundNS = 1;
        NSptr++;
    }
    if (FoundNS) return NSptr;
    else return strptr;
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

/*****************************************************************************
    void MCI_One_Second_Callback()
        Description:
            Set as a repeating 1 second Timer 2 interrupt callback
            Must keep all calls here Non-blocking.
            Keep SGD messages to no more than 8 bytes and they will fit
            in the UART TX buffer.
  ***************************************************************************/
void MCI_One_Second_Callback()
{
    struct tm *localtm;
    int tm_sec, tm_min, tm_hour;

    // UTCTime stays at zero until first valid time parsed from HTTP response
    if (UTCTime)
    {
        UTCTime++;
    }


    // Local_Time stays at zero until first valid EventState parsed from server
    if (Local_Time)
    {
        Local_Time++;
        localtm = localtime(&Local_Time);
        tm_sec = localtm->tm_sec;
        tm_min = localtm->tm_min;
        tm_hour = localtm->tm_hour;

        // Reset stats every hour
        // Send timesync every 4AM
        if (!tm_min && !tm_sec)
        {
            if (!IdleNormal && !RunningNormal)
            {
                LoadFactor = 0;
            }
            else
            {
                LoadFactor = 1000 * RunningNormal / (IdleNormal + RunningNormal); // 1000 adds precision
                LoadFactor = (LoadFactor + 5) / 10; // Round and bring back to percentage
            }
            IdleNormal = 0;
            RunningNormal = 0;

            if (tm_hour == TIME_SYNC_HOUR)
                SendTimeSync(localtm->tm_wday, tm_hour);
        }
        // Status Query every 15 seconds
        else if ((tm_sec % 15) == 0)
        {
            //SendQueryOpState(DebugCnt);
//            DebugCnt++;
        }

        // TCP blackout period = sync time - 1 min
        // Prevents events from being processed and updating Local_time
        // Guarantees Local_time is monotonic and continuous during this period
        if ((tm_hour == TIME_SYNC_HOUR) && (tm_min == 59))
            TCP_blackout = 1;
        else
            TCP_blackout = 0;
    }

}

// Return 0 if had an error
// Assumes the digits are terminated by the character <
// Enter with strptr pointing to the first digit
int    StringToInt( char *strptr, int *iptr )
{
    char *tmpptr;
    char tmpStr[20];
    int size;

    if ( !(tmpptr = strstr( strptr, leftDelimeter )) ) return 0;
    size = (int)(tmpptr-strptr);
    strncpy(tmpStr, strptr, size);
    tmpStr[size] = '\0';

    *iptr = atoi(tmpStr);

    return 1;
}

// Return 0 if had an error
// Assumes the digits are terminated by the character <
// Enter with strptr pointing to the first digit
// base = 10 for decimal and 16 for hex
int    StringToLongLong( char *strptr, long long *llptr, int base )
{
  char *tmpptr;
  char tmpStr[20];
  int size;
  char *tmp;

  long long DebugValue;


  if ( !(tmpptr = strstr( strptr, leftDelimeter )) ) return 0;
  size = (int)(tmpptr-strptr);
  strncpy(tmpStr, strptr, size);
  tmpStr[size] = '\0';

// RWB
  //*llptr = strtoll(tmpStr, &tmp, base);
  tmp = tmpStr;
  if(base == 16)
  {
      // Assume buffer if pointing at just the value
      // or value with 0x in front of it, if 0x, just move
      // buffer pointer by 2

      if((strstr(tmpStr, "0x") != NULL))
      {
          tmp += 2;
      }
      if(sscanf(tmp, "%llx", llptr) == 0)
      {
        return(0);
      }
  }
  else
  {
      if(sscanf(tmp, "%llu", llptr) == 0)
      {
        return(0);
      }
  }

  // Daves compiles working differently than Bills
  // Dave originally used strtoll(). But Bill mentioned it didn't work for him.
  // Bill converted to sscanf(). But this only gave Dave the lower 32 bits.
  // To get around the compiler oddities the following code will run if only 32 bits are received.
  // A similar check is done in app.c. For instance, if the time has not been set then
  //  do not perform Qevent actions, which rely on the time being set.
  DebugValue = *llptr;
//  if(*llptr < 1280000000)  // Just make sure it is right after initialization.
//    {
//    *llptr = strtoll(tmpStr, &tmp, base);
//    }
  DebugValue = *llptr;


  return 1;
}

// Find Tag in strptr and then get the integer value. base is 10 for decimal
//  Return 1 if successful
int    TagAndIntVal( char *strptr, char *Tag, int *intValue, int base)
{

    int retval;
    long long tmplong;

    retval = TagAndLongLongVal(strptr, Tag, &tmplong, base);
    *intValue = (int)tmplong;
    return retval;
}


// Find Tag in strptr and then get the long long integer value. base is 10 for decimal
//  Return 1 if successful
int    TagAndLongLongVal( char *strptr, char *Tag, long long *LLValue, int base)
{
    char        *tmpstrptr;
    int            retval;

    retval = 0;

    // Find the Tag
    if ( !(tmpstrptr = strstr(strptr, Tag)) ) goto TagAndLLNotDone;
    if ( !(tmpstrptr = strstr(tmpstrptr, rightDelimeter)) ) goto TagAndLLNotDone;
    tmpstrptr++;
    if ( !(StringToLongLong(tmpstrptr, LLValue, base)) ) goto TagAndLLNotDone;

    // value is now in LLValue
    retval = 1;

TagAndLLNotDone:
    return retval;
}

// Find Tag in strptr and then copy the string value. Don't copy more than maxlength
// If href then looking for url in between the quotes.
// href is always an attribute
// Attributes are enclosed in quotes instead of > < for elements
// attrib indicates that want value in between quotes
// Else looking for value.
//  Return 1 if successful
char *hrefText = "href=\"";
int TagAndString( char *strptr, char *Tag, char *Valptr, int maxlength, int attrib )
{
    char        *tmpstrptr;
    int            retval;
    char        *tmpptr;
    int            size;

    retval = 0;

    // If find a match
    if ( !(tmpstrptr = strstr(strptr, Tag)) ) goto TagAndStrNotDone;
    if (attrib)
    {
        //if ( !(tmpstrptr = strstr(tmpstrptr, hrefText)) ) goto TagAndStrNotDone;
        if ( !(tmpstrptr = strstr(tmpstrptr, QuoteDelimeter)) ) goto TagAndStrNotDone;
    }
    else
    {
        if ( !(tmpstrptr = strstr(tmpstrptr, rightDelimeter)) ) goto TagAndStrNotDone;
    };
    tmpstrptr++;

    // tmpstrptr should now be pointing at the value
    if (attrib)
    {
        if ( !(tmpptr = strstr( tmpstrptr, QuoteDelimeter )) ) goto TagAndStrNotDone;
    }
    else
    {
        if ( !(tmpptr = strstr( tmpstrptr, leftDelimeter )) ) goto TagAndStrNotDone;
    };


    size = (int)(tmpptr-tmpstrptr);
    if ( size > maxlength-1 ) size = maxlength - 1;
    strncpy(Valptr, tmpstrptr, size);
    Valptr[size] = '\0';

    // value is now in LLValue
    retval = 1;

TagAndStrNotDone:
    return retval;
}

WORD Base64Encode(BYTE* cSourceData, WORD wSourceLen, BYTE* cDestData, WORD wDestLen)
{
	BYTE i, j;
	BYTE vOutput[4];
	WORD wOutputLen;

	wOutputLen = 0;
	while(wDestLen >= 4u)
	{
		// Start out treating the output as all padding
		vOutput[0] = 0xFF;
		vOutput[1] = 0xFF;
		vOutput[2] = 0xFF;
		vOutput[3] = 0xFF;

		// Get 3 input octets and split them into 4 output hextets (6-bits each) 
		if(wSourceLen == 0u)
			break;
		i = *cSourceData++;
		wSourceLen--;
		vOutput[0] = (i & 0xFC)>>2;
		vOutput[1] = (i & 0x03)<<4;
		if(wSourceLen)
		{
			i = *cSourceData++;
			wSourceLen--;
			vOutput[1] |= (i & 0xF0)>>4;
			vOutput[2] = (i & 0x0F)<<2;
			if(wSourceLen)
			{
				i = *cSourceData++;
				wSourceLen--;
				vOutput[2] |= (i & 0xC0)>>6;
				vOutput[3] = i & 0x3F;
			}
		}
	
		// Convert hextets into Base 64 alphabet and store result
		for(i = 0; i < 4u; i++)
		{
			j = vOutput[i];

			if(j <= 25u)
				j += 'A' - 0;
			else if(j <= 51u)
				j += 'a' - 26;
			else if(j <= 61u)
				j += '0' - 52;
			else if(j == 62u)
				j = '+';
			else if(j == 63u)
				j = '/';
			else				// Padding
				j = '=';

			*cDestData++ = j;
		}

		// Update counters
		wDestLen -= 4;
		wOutputLen += 4;
	}

	return wOutputLen;
}
