#include "IntwineModule.h"

#ifdef INTWINE_PROGRAMMABLE_THERMOSTAT

#include "Schedule.h"
#include "ThermComm.h"
#include "app.h"

#include "INTiAPI.h"
#include "SPIiAPI.h"
#include "Metrics.h"

void SpiInit(void);
void setAttention(int attn);
int  getAttention(void);

void ThermCmdRspDebugName(char *header, SPI_CMD_RESP_DATA *cmd, int index);
void ThermCmdRspDebugOutput(char *header, SPI_CMD_RESP_DATA *cmd);

int iThermEngineRecovery_FDFD = 0;
int iThermEngineRecovery_FEFD = 0;
int iThermEngineRecovery_FAFirstChar = 0;

// Allow for synchronization of Thermostat communication
// - If a command requires a response, must wait for it
// - If the response does not come (times out), or if a different response is sent (will still be seen as timeout)
//   this will allow for a re-send, most importantly, in the same order
MESSAGE_ENGINE_STATE iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;

// Offsets within thermostat messages for data items.
    #define THERMO_MSG_RSP_DATA_OFFSET  4
    #define THERMO_MSG_DATA_ESCAPE      0xFB
    #define THERMO_MSG_RSP_EXTRA_OFFSET 6
    #define THERMO_MSG_RSP_ABSCOOL_OFFSET  4
    #define THERMO_MSG_RSP_ABSHEAT_OFFSET  5
    #define THERMO_MSG_RSP_COOL_OFFSET  4
    #define THERMO_MSG_RSP_HEAT_OFFSET  5
    #define THERMO_MSG_RSP_HUMIDITY_OFFSET		4
    #define THERMO_MSG_RSP_LED_OFFSET		4

    #define MAX_PRICE_MESSAGE_SIZE 16
char PricingMessageBuffer[MAX_PRICE_MESSAGE_SIZE+1];           // two lines (2x8) read, allow NULL
void ThermGetPricingMessageBuffer(char *user_buf, int user_len);

int iLastResponseProcessed = 0;

int iCheckForTstatChipSelectAssertion = 0;          // Tests Chip Select with Attention High
unsigned int iChipSelectAssertionTime = 0;
int iUserButtonReceived = 0;                        // UserButton unsol response received ..
int iUserButtonReceivedDuringCommand = 0;           // .. during the command (wifi-to-tstat)
unsigned int iConsecutiveFillCharacters = 0;       // consecutive fill without new command

typedef struct
{
    unsigned char cmd1;
    unsigned char cmd2;
    unsigned char command_name[50];    
} THERM_COMMAND_LOOKUP;

THERM_COMMAND_LOOKUP ThermCommandLookup[] = 
{
//  Cmd1  Cmd2  12345678901234567892123456789312345678941234567895 (50 byte max appears sufficient)
    { 0x50, 0x01, "Read Radio Slot Number"}, 
    { 0x50, 0x03, "Read Thermostat Model Number String"}, 
    { 0x50, 0x04, "Read Current Thermostat Mode and Configuration"}, 
    { 0x50, 0x05, "Read Current Error Status"}, 
    { 0x50, 0x08, "Read/Set Thermostat Operating Mode"}, 
    { 0x50, 0x09, "Read/Set Fan Operating Mode"}, 
    { 0x50, 0x20, "Read/Set Energy Status LEDs"}, 
    { 0x50, 0x30, "Enable/Disable User Message Area"}, 
    { 0x50, 0x31, "Write User Message Area"}, 
    { 0x50, 0x32, "Enable/Disable Pricing Message Area"}, 
    { 0x50, 0x33, "Write Pricing message to CT30e"}, 
    { 0x50, 0x34, "Read Pricing message from CT30e"}, 
    { 0x50, 0x38, "Enable/Disable Radio Message Annunciation"}, 
    { 0x50, 0x40, "Write Radio Status"}, 
    { 0x50, 0x41, "Write Radio Name and Network Name"}, 
    { 0x50, 0x45, "Define User Button"}, 
    { 0x50, 0x46, "Write User Button Screen Text String"}, 
    { 0x50, 0x47, "Enable User Button and User Button Screen"}, 
    { 0x50, 0x48, "Unsolicited User Button Received"}, 
    { 0x50, 0x49, "Request/Release User Button Screen"}, 
    { 0x50, 0x50, "Read Thermostat Program Data"}, 
    { 0x50, 0x51, "Write Thermostat Program Data"}, 
    { 0x50, 0x52, "Enable/Disable Save Energy Mode"}, 
    { 0x50, 0x53, "Enable/Disable LCD Backlight"}, 
    { 0x50, 0x54, "Thermostat Power Status"}, 
    { 0x50, 0x55, "Enable/Disable Remote Control Mode"}, 
    { 0x50, 0x69, "Set Wifi Sleeping Mode and time"}, 
    { 0x50, 0x6A, "Read Thermostat Status"}, 
    { 0x50, 0x6B, "Read/ Set Heat Pump Emer"}, 
    { 0x50, 0x6C, "Read/ Set Detect Temp Change in Room Temp"}, 
    { 0x50, 0x6D, "Read HVAC Setup"}, 
    { 0x50, 0x6E, "Enable/Disable watchdog Timer"}, 
    { 0x06, 0x01, "Read Current Temperature"}, 
    { 0x20, 0x01, "Read/Set Radio Temperature Absolute Setpoints"}, 
    { 0x20, 0x02, "Disable Radio Temperature Absolute Setpoints"}, 
    { 0x20, 0x03, "Enable Radio Temperature Absolute Setpoints"}, 
    { 0x20, 0x11, "Read Current Temperature Targets"}, 
    { 0x20, 0x20, "Read/Set Temporary Target Temperature"}, 
    { 0x0F, 0x01, "Read Current Humidity"}, 
    { 0x21, 0x01, "Read/Set Humidity Setpoint"}, 
    { 0x06, 0x40, "Read/Set Current Time"}, 
    { 0x06, 0x41, "Read/Set Current Day"}, 
    { 0x0A, 0x37, "Read/Set Radio Temperature Target Setback"}, 
    { 0x0A, 0x45, "Read/Set Radio Power Usage"}, 
    { 0x0A, 0x46, "Read Temperature Override Status"}, 
    { 0x0A, 0x57, "Read/Set-enable Remote Temperature"}, 
    { 0x0A, 0x58, "Read Remote Temperature Status"}, 
    { 0x0A, 0x59, "Disable Remote Temperature"}, 
    { 0x0A, 0x61, "Disable Radio Utility Lock"}, 
    { 0x0A, 0x62, "Enable Radio Utility Lock"}, 
    { 0x0A, 0x63, "Disable Temperature Program Hold"}, 
    { 0x0A, 0x64, "Enable Temperature Program Hold"}, 
    { 0x0A, 0x65, "Increment/Decrement User Override"}, 
    { 0x0A, 0x66, "Read Data Log"}, 
    { 0x0A, 0x67, "Read/Set-enable Event data log"}, 
    { 0x0A, 0xDE, "Restore default setting"}, 
    { 0x50, 0xF1, "Send Message to Slot 1"}, 
    { 0x50, 0xF2, "Send Message to Slot 2"}, 
    { 0x50, 0x60, "Read/Set filter timeout period"}, 
    { 0x50, 0x61, "Read filter status and expiry flag"}, 
    { 0x50, 0x62, "Set UV bulb"}, 
    { 0x50, 0x63, "Set Humidity"}, 
    { 0x50, 0x64, "Set sleeping time"}, 
    { 0x50, 0x65, "CT30 Awake Radio Module"}, 
    { 0x50, 0x66, "Read battery status"}, 
    { 0x50, 0x67, "Radio Set Calibration data"}, 
    { 0x50, 0x68, "Read Radio Device Version Number"}, 
    { 0xfd, 0xfd, "Pad bytes to get Unsolicited Message"},
    { 0xff, 0xff, "Unknown Command ff ff"}, 
};

DWORD SPI_Timer = 0;

#define THERM_RSP_BUFSIZE 3      // Size of circular command buffer.
#define THERM_CMD_BUFSIZE 20     // Size of circular command buffer.

SPI_CMD_RESP_DATA ThermResp[THERM_RSP_BUFSIZE];     // Bill 20111003 - debugging showed that just 1 is sufficient
int nRespHead=0;
int nRespTail=0;
int nRespIndex=0;

SPI_CMD_RESP_DATA ThermCmd[THERM_CMD_BUFSIZE];      // Bill 20111003 - debugging showed up to 24 queued at same time
int nCmdHead=0;
int nCmdTail=0;
int nCmdIndex=0;

int debugTotalSPIMessageQueued = 0;
int debugTotalSPIMessageSent = 0;
int debugTotalHighWaterQueued = 0;
int debugCmdRespSendHighWaterBytes = 0;

int debugTotalReceiveQueued = 0;
int debugTotalReceiveProcessed = 0;
int debugTotalReceiveHighWaterQueued = 0;

//extern long int seq;
extern int spi_timeout;
char SPIErrorMsg[]= "SPI Retry 1";
char GetCurErrMsg[8] = {0xFA,0xF9,0x50,0x05,0xFB,0x20,0x55,0xFA};
int nErrMsgIndex=0;

extern DWORD WriteBackTimer;
extern DWORD TargDelayTimer;
// On first loop send up initial properties to server.

int SPI_Ledflag=1;
int SPIErrorFlag=0;
int SPIReportErrorFlag=0;
char h_flag=0;

#define SPI_DEBUG_BUFFER_SIZE 50
int  SPIDebugRxIndex = 0;
unsigned char SPIDebugRxBuf[SPI_DEBUG_BUFFER_SIZE];
int  SPIDebugTxIndex = 0;
unsigned char SPIDebugTxBuf[SPI_DEBUG_BUFFER_SIZE];

extern unsigned short iMessageInProgress;

// Recovery - 300ms timeout after message starts for each character received
// 1) Keep re-arming timeout for every character received
// 2) Callbacks sets iThermMessageReceiveTimeout variable
int iThermMessageReceiveTimeout = 0;
void ThermMessageReceiveTimeout();

// Recovery - on receipt of 0xFE, stop current command, send cmd_ReadCurErr, re-send current command
// 1) Set only allow the recovery to run once per successful command sent
// 2) Call cmd_ReadCurErr_push() which will push it to the top of the list
// 3) Additionally, SendToTherm_push is also needed
int iFE_OKToRunRecovery = 1;


static int iLastPutCData = 0;
void my_SpiChnPutC(SpiChannel chn, unsigned int data)
{
    iLastPutCData = data;
    SpiChnPutC(chn, data);
}

//rwb1
#define SEQUENTIAL_FILL_CHARACTER_RECOVERY_COUNT 20
int spi3_interrupt_count = 0;
void SPI3RxInterruptProcessing(void)
{
    unsigned char ucX = 0xFE;
    spi3_interrupt_count++;
    static unsigned int resp_receive_seq = 0;
    unsigned char b0, b1, b2;

    spi_timeout=1;  // Let main loop know you are interrupt so it doesn't fail on bootup.
    SPI_Ledflag=2;  
    ucX = SpiChnReadC(SPI_CHANNEL3);

    if(SPIDebugRxIndex < SPI_DEBUG_BUFFER_SIZE)
    {
        SPIDebugRxBuf[SPIDebugRxIndex] = ucX;
        SPIDebugRxIndex++;
    }
    else
    {
        SPIDebugRxIndex = SPI_DEBUG_BUFFER_SIZE;
    }

    if(ucX == 0xFD)
    {
        // throw out fill characters

// Now that the GetUnsolicitedMsg is NOT being put onto the queue, bec
        // detect comm that is somehow stuck sending pad bytes both ways
        if(iLastPutCData == 0xFD)
        {
            iConsecutiveFillCharacters++;
            if(iConsecutiveFillCharacters >= SEQUENTIAL_FILL_CHARACTER_RECOVERY_COUNT)
            {
                iThermEngineRecovery_FDFD++;
                SPI3DisableInterrupts();
                setAttention(1); //make attention high
                if(iMessageInProgress == MSG_RSP_USER_BTN_PUSH)
                {
                    iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;
                }
                // else let the current message timeout
            }
        }
        else
        {
            iConsecutiveFillCharacters = 0;
        }
    }
    else if(ucX == 0xFE)  // We got an error.  Do not handle (yet) just get out....
    {
// need to handle user button during FE recovery, yuk!
/*         if(iFE_OKToRunRecovery)                    */
/*         {                                          */
/*             iFE_OKToRunRecovery = 0;               */
/*             iConsecutiveFillCharacters = 0;        */
/*             SPI3DisableInterrupts();              */
/*             setAttention(1); //make attention high */
/*             cmd_ReadCurErr_push();                 */
/*         }                                          */
/*         else                                       */
/*         {                                          */
            if(iLastPutCData == 0xFD)
            {
                iConsecutiveFillCharacters++;;
                if(iConsecutiveFillCharacters >= SEQUENTIAL_FILL_CHARACTER_RECOVERY_COUNT)
                {
                    iThermEngineRecovery_FEFD++;
                    SPI3DisableInterrupts();
                    setAttention(1); //make attention high
    
                    // don't set engine idle, let any current message timeout
                    // iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;
                }
            }
            else
            {
                iConsecutiveFillCharacters = 0;
            }
/*         } */
    }
    else if(ucX == 0xFA)
    {
        iConsecutiveFillCharacters = 0;

        // This happens upon reciept of a final response from the thermostat.  
        // This puts finalizes loading the last "FA" into the message and setting it's length
        SPIErrorFlag=0; // clear error flag if necessary to go back to sending commands after error recovery
        nErrMsgIndex = 0;

        ThermResp[nRespHead].seq = resp_receive_seq++;
        ThermResp[nRespHead].data[nRespIndex++] = ucX;
        ThermResp[nRespHead].data_len = nRespIndex;
        // Reset response Index. Also putting in Process Therm Cmds....
        nRespIndex = 0;

#if defined(USER_BUTTON_SCENARIO_1) || defined(USER_BUTTON_SCENARIO_2)
        if(iCheckForTstatChipSelectAssertion != 0)
        {
            iCheckForTstatChipSelectAssertion = 0;
        }
#endif
        if(nCmdIndex > 1)
        {
            if((iUserButtonReceived == 1) &&
               (iMessageInProgress != MSG_RSP_USER_BTN_PUSH) &&
               (ThermCmd[nCmdTail].expectResp) &&
               (iUserButtonReceivedDuringCommand == 1))
            {
                // Continue sending pad bytes and receiving bytes when:
                // 1) Just received a User Button 0x50 0x48 response
                // 2) The current message is not solely a GetUnsolictedMsg (USER_BTN_PUSH)
                // 3) The command whose command pad area is being use for UserButton is expecting a result
                //    Otherwise, we can be done
                // 4) And, the UserButton response did not use the real Response area
                //    (0x20 0x11 had UserButton in response area and 0x20 0x11 data never followed)
                my_SpiChnPutC(SPI_CHANNEL3, 0xFD); // Response still expected, load in dummy value to send back
                ThermCmd[nCmdTail].expectResp = 0; // Don't want to re-enable padding on the real response
            }
            else
            {
                iFE_OKToRunRecovery = 1;        // good response, re-arm FE recovery
                SPI3DisableInterrupts();
                setAttention(1); //make attention high   
    
                if(iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_SENDING1)
                {
                    iProcessThermCmdsInUse = MESSAGE_ENGINE_RESPONSE_READY;
                }
                if(iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_SENDING2)
                {
                    iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;
                }
            }
            nRespHead += 1;
            debugTotalReceiveQueued++;
            if(nRespHead >= THERM_RSP_BUFSIZE)
            {
                nRespHead = 0;
            }
            return;
        }
        else
        {
            // sometimes the FA is a UserButton FA that was stuck because the earlier command was
            // not long enough to retrieve the entire UserButton response, so, if nCmdIndex <=1, this is the case
            // Just finish up the response, but don't stop the current command
            
            // basically, just do nothing and things will proceed
            iThermEngineRecovery_FAFirstChar++;
            nRespHead += 1;
            debugTotalReceiveQueued++;
            if(nRespHead >= THERM_RSP_BUFSIZE)
            {
                nRespHead = 0;
            }
        }
    }
    else
    {
        iConsecutiveFillCharacters = 0;

        ThermResp[nRespHead].data[nRespIndex++] = ucX; // add character to response msg
        // If the thermostat response is too long, and it is not an "FD"
        // Then the module aborts reciept of the message.  This may be an edge or some weird condition.
        if(nRespIndex > 52)
        {
            SPI3DisableInterrupts();

            ThermResp[nRespHead].seq = resp_receive_seq++;
            ThermResp[nRespHead].data[nRespIndex++] = ucX;
            ThermResp[nRespHead].data_len = nRespIndex;
            nRespIndex = 0;

            setAttention(1); //make attention high   
        }
        else
        {
            b0 = ThermResp[nRespHead].data[0];
            b1 = ThermResp[nRespHead].data[1];
            b2 = ThermResp[nRespHead].data[2];

            if( ((b0 == 0xF9) && (b1 == 0x50) && (b2 == 0x48)) ||
                ((b0 == 0xF9) && (b1 == 0x0A) && (b2 == 0x46)) )
            {
                iUserButtonReceived = 1;
                if(nCmdIndex < ThermCmd[nCmdTail].data_len)
                {
                    iUserButtonReceivedDuringCommand = 1;   // latch if received during command
                }
            }
        }
    }

//-------------------------------------------------------------------------------

    // mwn, changed this to greater than (was >= ) and causing attention to be de-asserted too quickly
    if(nCmdIndex >= ThermCmd[nCmdTail].data_len)
    {
        // Clearing our ThermCmd Cmd/SubCommand so that Check_If_Send won't see this 
        // as it loops through remaining commands that need to be processed.
//mwn fix this, not sure how the pad message was getting cleared
/*         if((ThermCmd[nCmdTail].data[0] != 0xFD) && */
/*            (ThermCmd[nCmdTail].data[1] != 0xFD) && */
/*            (ThermCmd[nCmdTail].data[2] != 0xFD))   */
/*         {                                          */
            ThermCmd[nCmdTail].data[0]=0x00;
            ThermCmd[nCmdTail].data[1]=0x00;
            ThermCmd[nCmdTail].data[2]=0x00;
/*         } */

        if(ThermCmd[nCmdTail].expectResp)
        {
            my_SpiChnPutC(SPI_CHANNEL3, 0xFD); // Response expected, load in dummy value to send back
        }
        // If no response is expected, you are done. Set attn high and leave routine.
        else
        {
            SPI3DisableInterrupts();
            setAttention(1); //make attention high  
            nRespIndex = 0; // just in case some characters were put in the buffer

            if(iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_SENDING1)
            {
                iProcessThermCmdsInUse = MESSAGE_ENGINE_RESPONSE_READY;
//                UARTiDebug("Shouldn't be here2!", DEBUG_SHOW_ALL);
//                while(1);
            }
            if(iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_SENDING2)
            {
                iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;
            }
        }     
    }
    // Normal operation just fills the SPI buffer with the command. 
    else
    {
        SPIDebugTxBuf[nCmdIndex] = ThermCmd[nCmdTail].data[nCmdIndex];
        SPIDebugTxIndex++;

// may not play nice in interrupt - must investigate
//        TimeMonitorStartOfLoop();
        while(!SPI3STATbits.SPITBE);
/*         {                                                       */
/*             TimeMonitorProcessLoop(4000, WATCHDOG_SPI_MISREAD); */
/*         }                                                       */
        SPI3BUF = ThermCmd[nCmdTail].data[nCmdIndex++];
    }
}


/* Initialize communications with thermostat.
 * No variables currently need initialization.
 */
void ThermoInit(void)
{
    int i=0;
    /*
    char pMsgText[]="Intwine Demonstration";	
    char pRadioName[]="Intwine";	
    char pNetworkName[]="Intwine";	
    */
    for(i=0 ; i<THERM_CMD_BUFSIZE; i++)
    {
        // Clearing out command and subcommand so that we start fresh (concept used in Check_If_Send since we loop to find latest command)
        ThermCmd[i].seq=0;
        ThermCmd[i].inUse=0;
        ThermCmd[i].data[0]=0;
        ThermCmd[i].data[1]=0;
        ThermCmd[i].data[2]=0;
    }

    ThermEngineCmdInit();

    strcpy(PricingMessageBuffer, "");

    INTiRegisterSPI3CallbackFunction(SPI3RxInterruptProcessing);

    setAttention(1); //make attention high   

//    cmd_ReadCurErr();               // Clear any errors

    // cmd_ReadModelNum(); // 5003  This is read at first to send out a test message to the server.  
    // There is about 4 second delay between this and starting to run main loop.
    return;
}

void CheckSPIErrorFlag(void)
{

    char pMsgText[]="Recovered SPI Err";  
    char pMsgText2[]="SPI Error 2"; 

    if(SPIReportErrorFlag==1)
    {
        cmd_WriteUserMsgArea(0, pMsgText);
        cmd_WriteUserMsgArea(0x01, SPIErrorMsg);
        SPIReportErrorFlag=0;
    }
    if(SPIReportErrorFlag==2)
    {
        cmd_WriteUserMsgArea(0, pMsgText2);
        SPIReportErrorFlag=0;
    }
    return;
}


void SendToTherm(SPI_CMD_RESP_DATA cmdMsg)
{
    int i=0;
    static int j=0;
    char test[40];
    static unsigned int seq=0;

//    nCmdHead += 1;

    // The engine part of the commands can only have 1 queued
    // The UserButton responses can throw this off
    // This does not need to be interrupt protected
    // 20120319 RWB - Well, need to keep the real processing engine
    // in sync, for command timeouts etc.  So, due to UserButton responses, etc.
    // we have gotten out of sync.  Go ahead and throw away a message, 
    // by simply re-writing one
    nCmdHead = nCmdTail + 1;
    nCmdHead = nCmdHead % THERM_CMD_BUFSIZE;
    memset((char *)&ThermCmd[nCmdHead].seq, 0, sizeof(SPI_CMD_RESP_DATA));

    debugTotalSPIMessageQueued++;
    if((debugTotalSPIMessageQueued - debugTotalSPIMessageSent) > debugTotalHighWaterQueued)
    {
        debugTotalHighWaterQueued = debugTotalSPIMessageQueued - debugTotalSPIMessageSent;
    }

    if(cmdMsg.data_len > debugCmdRespSendHighWaterBytes)
    {
        debugCmdRespSendHighWaterBytes = cmdMsg.data_len;
    }

    ThermCmd[nCmdHead].seq = seq++;
    for(i=0;i<cmdMsg.data_len;i++)
    {
        ThermCmd[nCmdHead].data[i] = cmdMsg.data[i];
    }
    ThermCmd[nCmdHead].data_len = cmdMsg.data_len;
    ThermCmd[nCmdHead].expectResp = cmdMsg.expectResp;
    ThermCmd[nCmdHead].inUse=1;
    sprintf(test,"\rTC:%d (0x%02x:0x%02x:0x%02x)",j++, ThermCmd[nCmdHead].data[0], ThermCmd[nCmdHead].data[1], ThermCmd[nCmdHead].data[2]);
    UARTiDebug((char*)test, DEBUG_THERMOSTAT);
}

void SendToTherm_push(SPI_CMD_RESP_DATA cmdMsg)
{
    int i=0;
    static int j=0;
    char test[40];
    static unsigned int seq=0;

/*     nCmdHead += 1;                           */
/*     nCmdHead = nCmdHead % THERM_CMD_BUFSIZE; */
    nCmdTail -= 1;
    if(nCmdTail < 0)
    {
        nCmdTail = (THERM_CMD_BUFSIZE-1);
    }

/*     ThermCmd[nCmdHead].seq = seq++;                                                                                                         */
/*     for(i=0;i<cmdMsg.data_len;i++)                                                                                                          */
/*     {                                                                                                                                       */
/*         ThermCmd[nCmdHead].data[i] = cmdMsg.data[i];                                                                                        */
/*     }                                                                                                                                       */
/*     ThermCmd[nCmdHead].data_len = cmdMsg.data_len;                                                                                          */
/*     ThermCmd[nCmdHead].expectResp = cmdMsg.expectResp;                                                                                      */
/*     ThermCmd[nCmdHead].inUse=1;                                                                                                             */
/*     sprintf(test,"\rTP:%d (0x%02x:0x%02x:0x%02x)",j++, ThermCmd[nCmdHead].data[0], ThermCmd[nCmdHead].data[1], ThermCmd[nCmdHead].data[2]); */

    ThermCmd[nCmdTail].seq = seq++;
    for(i=0;i<cmdMsg.data_len;i++)
    {
        ThermCmd[nCmdTail].data[i] = cmdMsg.data[i];
    }
    ThermCmd[nCmdTail].data_len = cmdMsg.data_len;
    ThermCmd[nCmdTail].expectResp = cmdMsg.expectResp;
    ThermCmd[nCmdTail].inUse=1;
    sprintf(test,"\rTP:%d (0x%02x:0x%02x:0x%02x)",j++, ThermCmd[nCmdTail].data[0], ThermCmd[nCmdTail].data[1], ThermCmd[nCmdTail].data[2]);

    UARTiDebug((char*)test, DEBUG_THERMOSTAT);
}


/*  This function runs every 1 seconds, else it exits 
May need to check and see if timer is overlapping and getting stuck */
void ProcessThermCmds()
{
    int bp;
    char cJunk;
    DWORD dwCurrentTime;
    int time_expired = 0;
    static DWORD throttling_time = 0;  // needs to be TIME_MONITOR_TICK_SECOND/2, but having compiler issue

    if(throttling_time == 0)
    {
        throttling_time = TIME_MONITOR_TICK_SECOND/2;
    }

    // See if anything needs put into real tstat command queue
    ProcessThermEngineCmds();

    // wait for at least .5 seconds and then a message waiting
    dwCurrentTime = TimeMonitorTickGet();
//    if((dwCurrentTime - SPI_Timer) >= (TIME_MONITOR_TICK_SECOND*3))
    if((dwCurrentTime - SPI_Timer) >= throttling_time) // (TIME_MONITOR_TICK_SECOND/2))
    {
        time_expired = 1;
    }

    if(time_expired)
    {
        if((iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_EXPECT_A_RESPONSE) ||
           (iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_NO_RESPONSE_EXPECTED))
        {
            SPI_Timer = dwCurrentTime;
            // This begins to run when command is inserted. 
            // SPI interrupt runs when char is in the buffer
#ifdef NO_GET_ATTENTION_ASSERT
            if(getAttention() == 0) // attention still on from last command. attempt to recover from error.
            {
                setAttention(1); //make attention high   
                ASRT(0, "ProcessThermCmds getAttention still 0");
                while(1); // ruff, this should never happen given new command processing layer above this one
    /*             setAttention(1);                    */
    /*             SPI3DisableInterrupts();           */
    /*                                                 */
    /*             SPIErrorFlag=0; // clear error flag */
    /*             nErrMsgIndex = 0;                   */
    /*             nRespIndex=0;                       */
    /*             nCmdIndex=0;                        */
    /*                                                 */
    /*             ThermCmd[nCmdTail].data[0]=0xFA;    */
    /*             ThermCmd[nCmdTail].data_len=1;      */
    /*             ThermCmd[nCmdTail].expectResp=0;    */
    /*             if(nCmdTail == 0)                   */
    /*             {                                   */
    /*                 nCmdTail = THERM_CMD_BUFSIZE;   */
    /*             }                                   */
    /*             else                                */
    /*             {                                   */
    /*                 nCmdTail--;                     */
    /*             }                                   */
            }
            else
#endif
            if(nCmdHead != nCmdTail)
            {
                // This routine among other things enable interrupts to the SPI module.
                // Ints. are turned off when command or response is recieved, only to be enabled here again.
                SPIDebugRxIndex = 0;
                memset(SPIDebugRxBuf, 0, SPI_DEBUG_BUFFER_SIZE);
                SPIDebugTxIndex = 0;
                memset(SPIDebugTxBuf, 0, SPI_DEBUG_BUFFER_SIZE);
    
                // 23.3.2.2 Slave Mode Operation steps
                SPI3DisableInterrupts();           // 1. 
                SPI3CON = 0x00000000;               // 2.
                cJunk = SPI3BUF;                    // 3.
                IFS0bits.SPI3EIF  = 0;              // 4.
                IFS0bits.SPI3RXIF = 0;
                IFS0bits.SPI3TXIF = 0;
                SPI3EnableInterrupts();            // 4.
    
                SPI3STATCLR = BIT_6;                // 5.
    
                // 0000 0000 0000 0000 1000 0001 1000 0000 = 0x00008180
                SPIiInitialize(0x00008180);
    
                debugTotalSPIMessageSent++;
                memset((char *)&ThermCmd[nCmdTail].seq, 0, sizeof(SPI_CMD_RESP_DATA));
//                ThermCmd[nCmdTail].inUse=0;
                nCmdTail += 1;
                nCmdTail = nCmdTail % THERM_CMD_BUFSIZE;
    
                iUserButtonReceived = 0;
                iUserButtonReceivedDuringCommand = 0;   // clear this detection
                iConsecutiveFillCharacters = 0;

                // check for corrupted (not sure how yet) message
                if(ThermCmd[nCmdTail].data[0] != 0xF9)
                {
                    if((ThermCmd[nCmdTail].data[0] == 0xFD) &&
                       (ThermCmd[nCmdTail].data[0] == 0xFD) &&
                       (ThermCmd[nCmdTail].data[0] == 0xFD))
                    {
                        // it's a get unsolicited, leave be
                    }
                    else
                    {
                        bp = 1; // wtf
                    }
                }

                // setup large throttling time for tstat program messages
                if((ThermCmd[nCmdTail].data[1] == 0x50) &&
                   ((ThermCmd[nCmdTail].data[2] == 0x50) || (ThermCmd[nCmdTail].data[2] == 0x51)))
                {
                    throttling_time = (TIME_MONITOR_TICK_SECOND); // 2x normal time for schedule commands
                }
                else
                {
                    throttling_time = (TIME_MONITOR_TICK_SECOND/2);
                }
                
                iUserButtonReceived = 0;                // Dump hex code to the debug uart
                ThermCmdRspDebugName("\r\nThermCmd: ", &ThermCmd[nCmdTail], nCmdTail);
                ThermCmdRspDebugOutput("Data: ", &ThermCmd[nCmdTail]);
    
                nCmdIndex = 1;
                my_SpiChnPutC(SPI_CHANNEL3, ThermCmd[nCmdTail].data[0]);
    //            SPI3BUF = ThermCmd[nCmdTail].data[0];
    
                if(iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_EXPECT_A_RESPONSE)
                {
                    iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_SENDING1;
                }
                if(iProcessThermCmdsInUse == MESSAGE_ENGINE_CMD_NO_RESPONSE_EXPECTED)
                {
                    iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_SENDING2;
                }
    
                setAttention(0);//make attention low
    
                bp = 1;
            }
        }
        else if(iProcessThermCmdsInUse == MESSAGE_ENGINE_IDLE)
        {
#if defined(USER_BUTTON_SCENARIO_1) || defined(USER_BUTTON_SCENARIO_2)
            // Comm is idle, check for unsolicited messages
            // If attention high and chip select low
            if((PORTB & BIT_2) && !(PORTD & BIT_14))
            {
                if(iChipSelectAssertionTime == 0)
                {
                    iChipSelectAssertionTime = dwCurrentTime;
                }
                if((dwCurrentTime - iChipSelectAssertionTime) >= (TIME_MONITOR_TICK_SECOND * 60))
                {
                    ASRT_INT(0, "Chip select asserted too long", WATCHDOG_THERM_CS_HELD_LOW);
                }

                // Send on 5 consecutive bit patterns, only (used to be >= 5 ... Baaaadddd!)
                iCheckForTstatChipSelectAssertion++;
                if(iCheckForTstatChipSelectAssertion == 5)
                {
                    cmd_GetUnsolicitedMsg();
                }
            }
            else
            {
                iCheckForTstatChipSelectAssertion = 0;
                iChipSelectAssertionTime = 0;
            }
#endif
        }
    }
}

int comm_rsp_nsetHumidity = 0;
unsigned long comm_rsp_target = 0;

void ProcessThermRsps()
{
    int nCommand, bp;
    static int firstConfigRead=0;

// put local copies here and work from them
// although the attributes are never sent, they are used to determine values of other attributes
    static char ConfigBuffer[11];
    static char tempConfigBuffer[11];
    static char nHeatTargetVal=0;
    static char nCoolTargetVal=0;
    static char DLogBufferVal[11];
    static char tempDLogBufferVal[11] = "xxx";    // initialize to x's to force first update
    static char ELogBufferVal[11];
    static char tempELogBufferVal[11] = "xxx";    // initialize to x's to force first update
    DWORD dwCurrentTime;

    dwCurrentTime = TimeMonitorTickGet();

    // Check if a reply from the server is expected.
    if(0) // (replyExpected)
    {
        /* INT8 resp = SessionGetResponse();
        // See if we got something back.
        if (resp != STATUS_TX_STILL_RUNNING)
        {
            replyExpected = FALSE;
        } */
    }
    else  // if no reply is expected then we are free to process and send another message
    {
        if(nRespTail != nRespHead)
        {
            ThermResp[nRespTail] = UndoByteStuffing(ThermResp[nRespTail]); // Undo byte stuffing

            // Validate checksum?
            nCommand = ThermResp[nRespTail].data[1];
            nCommand = nCommand << 8;
            nCommand = nCommand & 0xFF00;
            nCommand = nCommand + ThermResp[nRespTail].data[2];
            // Loop through and make sure new command is not in buffer to be sent?

            // Dump hex code to the debug uart
            ThermCmdRspDebugName("\r\nThermRsp: ", &ThermResp[nRespTail], 0);
            ThermCmdRspDebugOutput("Data: ", &ThermResp[nRespTail]);

            iLastResponseProcessed = nCommand;
            switch(nCommand)
            {
                case MSG_RSP_RADIO_SLOT: //0x5001
                    {
                        break;
                    }
                case MSG_RSP_MODEL_NUM: //0x5003
                    {
                        break;
                    }
                case MSG_RSP_MODE_CONFIG: //0x5004
                    {
                        char *ptr = &ConfigBuffer[0];
                        int i=4;
                        char process=0;

//                        if((TimeMonitorTickGet() - TargDelayTimer < (TIME_MONITOR_TICK_SECOND * 6)))
                        if((dwCurrentTime - TargDelayTimer < (TIME_MONITOR_TICK_SECOND * 6)))
                        {
                            UARTiDebug("\r\r HOLDDELAY!!!!!!", DEBUG_THERMOSTAT);
                            return;
                        }

                        if(firstConfigRead>1)
                        {
                            UARTiDebug("\r\r RETURN CFG", DEBUG_THERMOSTAT);
                            firstConfigRead++;
                            return;
                        }
                        UARTiDebug("\r\r READ CFG", DEBUG_THERMOSTAT);
                        for(i=4; i<12; i++)
                        {
                            *ptr++ = ThermResp[nRespTail].data[i];  // Copies message into ptr.
                        }


                        *ptr = '\0';  // Appends pointer memory truncator.

                        // Target has changed so send update message to server.
                        for(i=0; i<8; i++)
                        {
                            if(tempConfigBuffer[i]!= ConfigBuffer[i])
                            {
                                process=1;
                                break;
                            }
                        } 


                        if(process)
                        {
                            UARTiDebug("\r\r SEND C_LOG", DEBUG_THERMOSTAT);
                            if(ConfigBuffer[2])
                            {
                                cbAttrSetByIdForceSend16(kCBAtt_holdEnabled, 1);
                            }
                            else
                            {
                                cbAttrSetByIdForceSend16(kCBAtt_holdEnabled, 0);
                            }

                            for(i=0; i<8; i++)
                            {
                                tempConfigBuffer[i]=ConfigBuffer[i];
                            } 
                            tempConfigBuffer[8]='\0';
                        }
                        break;
                    }
                case MSG_RSP_CUR_ERROR: //0x5005
                    {
                        int nErr=0;
                        nErr = ThermResp[nRespTail].data[4];
                        if(nErr != 0)
                        {
                            bp = 1;
                        }
                        break;
                    }
                case MSG_RSP_THERM_OP_MODE: //0x5008
                    {
                        int nMode;
                        // Get the current mode.
                        nMode = ThermResp[nRespTail].data[THERMO_MSG_RSP_DATA_OFFSET];
                        cbAttrSetByIdAutoSend8(kCBAtt_setMode, nMode);
                        break;
                    }
                case MSG_RSP_FAN_OP_MODE: //0x5009
                    {
                        int nMode;
                        nMode = ThermResp[nRespTail].data[4];
                        cbAttrSetByIdAutoSend8(kCBAtt_fanMode, nMode);
                        break;
                    }
                case MSG_RSP_LED_STATE: //0x5020
                    {
                        int nLed;
                        nLed = ThermResp[nRespTail].data[THERMO_MSG_RSP_LED_OFFSET];
                        break;
                    }

/***** start of new pricing message response *****/

                case MSG_RSP_PRICING_MESSAGE: //0x5034
                    {
                        char *ptr = &PricingMessageBuffer[0];
                        int i=4;

                        for(i=4; i<(20); i++)
                        {
                            *ptr++ = ThermResp[nRespTail].data[i];  // Copies message into ptr.
                        }

                        *ptr = '\0';  // Appends pointer memory truncator.
                        break;
                    }
/***** end of new pricing message response *****/

                case MSG_RSP_USER_BTN_PUSH: //0x5048
                    {
#if defined(USER_BUTTON_SCENARIO_1) || defined(USER_BUTTON_SCENARIO_2)
                        bp = 1;
#endif
                        break;
                    }
                case MSG_RSP_USER_BTN_SCR_REQ: //0x5049
                    {
                        break;
                    }
                case MSG_RSP_PRG_DATA: //0x5050
                    {
                        int nDay=0;
                        int i=0;
                        char sSched[70];   // 7 days * (4 time + 2 temp + 2 ;) = 56
                        int time=0; 
                        char asciitime[10];
                        int temp;
                        char asciitemp[10];
                        sSched[0]='\0';

                        nDay = ThermResp[nRespTail].data[6];
                        
                        for(i=0;i<21;i=i+3) //program data is 21 bytes long, 3 bytes for each segment in each day
                        {
                            //sSched[i] = ThermResp[nRespTail].data[i+7];
                            if(ThermResp[nRespTail].data[i+8]==0xFA) // Check after CRC if end of message....
                                break;
                            time = ThermResp[nRespTail].data[i+7];
                            time = time<<8;
                            time = time & 0xFF00;
                            time = time | ThermResp[nRespTail].data[i+8];
                            temp = ((ThermResp[nRespTail].data[i+9]) / 2) & 0xFF;

                            schedSetModeDay(nDay, i/3, time, temp);

                            sprintf(asciitime,"%d", time);
                            sprintf(asciitemp,"%d", temp);
                            strcat(sSched,asciitime);
                            strcat(sSched,";");
                            strcat(sSched,asciitemp);
                            strcat(sSched,";");
                        }

                        if(nDay == 6)
                        {
                            schedStructToString();
                        }

                        if(nDay == 0)
                        {
                            //	sprintf(propMsgBuffer, PROP_NAME_SCHEDMON "%c %s", schedsel.val[0], sSched);
                        }
                        else if(nDay == 1)
                        {
                            //	 sprintf(propMsgBuffer, PROP_NAME_SCHEDTUE "%c %s", schedsel.val[0], sSched);
                            //strcat(propMsgBuffer,sSched);
                        }
                        else if(nDay == 2)
                        {
                            //	sprintf(propMsgBuffer, PROP_NAME_SCHEDWED "%c %s", schedsel.val[0], sSched);
                            //strcat(propMsgBuffer,sSched);
                        }
                        else if(nDay == 3)
                        {
                            //	sprintf(propMsgBuffer, PROP_NAME_SCHEDTHU "%c %s", schedsel.val[0], sSched);
                            //strcat(propMsgBuffer,sSched);
                        }
                        else if(nDay == 4)
                        {
                            //	sprintf(propMsgBuffer, PROP_NAME_SCHEDFRI "%c %s", schedsel.val[0], sSched);
                            //strcat(propMsgBuffer,sSched);
                        }
                        else if(nDay == 5)
                        {
                            //	sprintf(propMsgBuffer, PROP_NAME_SCHEDSAT "%c %s", schedsel.val[0], sSched);
                            //strcat(propMsgBuffer,sSched);
                        }
                        else if(nDay == 6)
                        {
                            //    sprintf(propMsgBuffer, PROP_NAME_SCHEDSUN "%c %s", schedsel.val[0], sSched);
                            //strcat(propMsgBuffer,sSched);	
                        }


                        else
                        {
                            break; // error of some kind - don't send message
                        }

//					SendAESEncCBMsg(propMsgBuffer,'u',1); 			

                        break;
                    }
                case MSG_RSP_BATT_STATE: //0x5052
                    {
                        break;
                    }
                case MSG_RSP_REMOTE_CONTROL: //0x5055
                    {
                        break;
                    }
                case MSG_RSP_CUR_TEMP: //0x0601
                    {
                        int nTemp;
                        // Reported temperature is multiplied by 2.  Divide by 2 to get actual.
                        nTemp = ThermResp[nRespTail].data[THERMO_MSG_RSP_DATA_OFFSET] >> 1;
                        cbAttrSetByIdAutoSend8(kCBAtt_houseTemp, nTemp);
                        break;
                    }
                case MSG_RSP_TEMP_ABS_SETPOINTS: //0x2001
                    {
                    }
                case MSG_RSP_TARGETS: //0x2011
                    {
                        unsigned int nTargetTemps;  // So we can compare to global
                        if((dwCurrentTime - TargDelayTimer < (TIME_MONITOR_TICK_SECOND * 6)))
                        {
                            UARTiDebug("\r\r TARGDELAY!!!!!!", DEBUG_THERMOSTAT);
                            return;
                        }
                        nCoolTargetVal = ThermResp[nRespTail].data[THERMO_MSG_RSP_COOL_OFFSET] >> 1;
                        nHeatTargetVal = ThermResp[nRespTail].data[THERMO_MSG_RSP_HEAT_OFFSET] >> 1;
                        nTargetTemps = nCoolTargetVal;
                        nTargetTemps = nTargetTemps << 8;
                        nTargetTemps = nTargetTemps + nHeatTargetVal;
                        cbAttrSetByIdAutoSend32(kCBAtt_tempTargets, nTargetTemps);
                        UARTiDebug("\r\n\r\n UPDATING TARGETS \r\n", DEBUG_THERMOSTAT);
                        break;
                    }
                case MSG_RSP_CUR_HUMIDITY: //0x0F01
                    {
                        int nHumidity;
                        nHumidity = ThermResp[nRespTail].data[THERMO_MSG_RSP_HUMIDITY_OFFSET];
                        cbAttrSetByIdAutoSend8(kCBAtt_thermoCurrHumidity, nHumidity);
                        break;
                    }
                case MSG_RSP_HUM_SETPOINT: //0x2101
                    {
                        comm_rsp_nsetHumidity = ThermResp[nRespTail].data[4];
                        break;
                    }
                case MSG_RSP_CUR_TIME: //0x0640
                    {
                        UINT8       targetmsb;
                        UINT8   targetlsb;

                        targetmsb = ThermResp[nRespTail].data[4];
                        targetlsb = ThermResp[nRespTail].data[5];

                        comm_rsp_target=targetmsb;
                        comm_rsp_target = comm_rsp_target << 8;
                        comm_rsp_target = comm_rsp_target | targetlsb;
                        break;
                    }
                case MSG_RSP_CUR_DAY: //0x0641
                    {

                        int nDay;

                        nDay = ThermResp[nRespTail].data[4];
                        cbAttrSetByIdAutoSend8(kCBAtt_thermoDay, nDay);
                        break;
                    }
                case MSG_RSP_TEMP_TARG_SETBACK: //0x0A37
                    {
                        break;
                    }
                case MSG_RSP_TEMP_OFFSET: //0x0A45
                    {
                        break;
                    }
                case MSG_RSP_TEMP_OVERRIDE: //0x0A46
                    {
                        int nOverride;
                        nOverride = ThermResp[nRespTail].data[4];
                        break;
                    }
                case MSG_RSP_REM_TEMP_VAL: //0x0A57
                    {
                        int nRmtTemp;
                        nRmtTemp = ThermResp[nRespTail].data[4];
                        break;
                    }
                case MSG_RSP_REM_TEMP_STATUS: //0x0A58
                    {
                        int nRmtTemps;
                        nRmtTemps = ThermResp[nRespTail].data[4];
                        break;
                    }
                case MSG_RSP_DATA_LOG: //0x0A66
                    {
                        char *ptr = &DLogBufferVal[0];
                        int i=4;
                        char update=0;

                        for(i=4; i<12; i++)
                        {
                            *ptr++ = ThermResp[nRespTail].data[i];  // Copies message into ptr.
                        }

                        *ptr = '\0';  // Appends pointer memory truncator.

                        // Target has changed so send update message to server.
                        for(i=0;i<8;i++) // If anything except the time variable has changed....
                        {
                            if(DLogBufferVal[i]!= tempDLogBufferVal[i])
                            {
                                update=1;
                                break;
                            }
                        }

                        if(update)
                        {
                            for(i=0; i<8 ; i++)
                            {
                                tempDLogBufferVal[i]=DLogBufferVal[i];
                            } 
                            tempDLogBufferVal[8]='\0';
                        }
                        break;
                    }
                case MSG_RSP_RSE_EVENT_LOG: //0x0A67
                    {
                        char *ptr = &ELogBufferVal[0];
                        int i=4;
                        char update=0;
                        for(i=4; i<12; i++)
                        {
                            *ptr++ = ThermResp[nRespTail].data[i];  // Copies message into ptr.
                        }

                        *ptr = '\0';  // Appends pointer memory truncator.
                        UARTiDebug("\rELOG", DEBUG_THERMOSTAT);
                        // UARTiDebug((char*)ELogBuffer.val);
                        // Target has changed so send update message to server.
                        // Put IF back in below once persistence is enabled, else send up new value everytime generic cmd is issued.
                        for(i=2;i<8;i++) // If anything except the time variable has changed....
                        {
                            if(ELogBufferVal[i] != tempELogBufferVal[i])
                            {
                                update=1;
                                UARTiDebug("\r\rUPDATE ELOG\r\r", DEBUG_THERMOSTAT);
                                break;
                            }
                        }
                        if(update)
                        {
                            cbAttrSetByIdAutoSend8(kCBAtt_cW, ELogBufferVal[3]&0x01);
                            cbAttrSetByIdAutoSend8(kCBAtt_cW2, (ELogBufferVal[3]&0x02) >> 1);
                            cbAttrSetByIdAutoSend8(kCBAtt_cW3, (ELogBufferVal[3]&0x04) >> 2);
                            cbAttrSetByIdAutoSend8(kCBAtt_cY, (ELogBufferVal[3]&0x08) >> 3);
                            cbAttrSetByIdAutoSend8(kCBAtt_cY2, (ELogBufferVal[3]&0x10) >> 4);
                            cbAttrSetByIdAutoSend8(kCBAtt_cG, (ELogBufferVal[3]&0x20) >> 5);

                            for(i=0; i<8 ; i++)
                            {
                                tempELogBufferVal[i]=ELogBufferVal[i];
                            } 
                            tempELogBufferVal[8]='\0';
                        }
                        break;
                    }
                case MSG_RSP_RADIO_BUS_MSG: //0x50F0
                    {
                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            // clear out to ensure nothing is "re-processed"
            ThermResp[nRespTail].data[0] = 0;
            ThermResp[nRespTail].data[1] = 0;
            ThermResp[nRespTail].data[2] = 0;

            nRespTail += 1;
            debugTotalReceiveProcessed++;
            if((debugTotalReceiveQueued-debugTotalReceiveProcessed) > debugTotalReceiveHighWaterQueued)
            {
                debugTotalReceiveHighWaterQueued = debugTotalReceiveQueued-debugTotalReceiveProcessed;
            }
            if((debugTotalReceiveProcessed-debugTotalReceiveQueued) > debugTotalReceiveHighWaterQueued)
            {
                debugTotalReceiveHighWaterQueued = debugTotalReceiveProcessed - debugTotalReceiveQueued;
            }
            nRespTail = nRespTail % THERM_RSP_BUFSIZE;

        }
    }   
}

SPI_CMD_RESP_DATA UndoByteStuffing(SPI_CMD_RESP_DATA Resp)
{
    SPI_CMD_RESP_DATA UnStuffed;
    int nCounter=0;
    int i=0;

    memset((char *)&UnStuffed.seq, 0, sizeof(SPI_CMD_RESP_DATA));

    UnStuffed.seq        = Resp.seq;
    UnStuffed.inUse      = Resp.inUse;
    UnStuffed.expectResp = Resp.expectResp;
    UnStuffed.data[nCounter++] = Resp.data[0]; // copy F9
    for(i=1;i<(Resp.data_len-1);i++)
    {
        if(Resp.data[i] == 0xFB)
        {
            i++;
            UnStuffed.data[nCounter++] = Resp.data[i] - 0x20;
        }
        else
        {
            UnStuffed.data[nCounter++] = Resp.data[i];
        }
    }
    UnStuffed.data[nCounter++] = Resp.data[Resp.data_len-1]; // add 0xFA

    UnStuffed.data_len = nCounter;
    return(UnStuffed);
}


char VerifyChecksum(SPI_CMD_RESP_DATA Resp)
{
    return(1);
}

char Check_If_Send(int Command)
{
    int i;  
    int j;
    for(i=0; i<THERM_CMD_BUFSIZE; i++)
    {
        j=ThermCmd[i].data[1] << 8;
        j=j & 0xFF00;
        j=j + ThermCmd[i].data[2];
        if(j==Command)
        {
            return 0;
        }
    }
    return 1;
}


int last_attention = 0;

void setAttention(int attn)
{
    if(attn)
    {
        LATBSET = 0x0004;
    }
    else
    {
        LATBCLR = 0x0004;
    }
    last_attention = attn;
}

int getAttention(void)
{
    return(last_attention);
}

void ThermCmdRspDebugOutput(char *header, SPI_CMD_RESP_DATA *cmd)
{
    char b1[10], b2[100];
    unsigned char *ptr;
    int loop, loop2, loop2_size;

    ptr = cmd->data;

    // print hex code here
    sprintf(b2, "\r\n%s", header);
    UARTiDebug(b2, DEBUG_THERMOSTAT);
    for(loop=0; loop<cmd->data_len; loop+=16)
    {
        loop2_size = cmd->data_len - loop;
        if(loop2_size > 16) loop2_size = 16;
        for(loop2=0; loop2<loop2_size; loop2++)
        {
            sprintf(b1, "0x%02x ", *ptr++);
            UARTiDebug(b1, DEBUG_THERMOSTAT);
        }
        UARTiDebug("\r\n", DEBUG_THERMOSTAT);
    }
}

void ThermCmdRspDebugName(char *header, SPI_CMD_RESP_DATA *cmd, int index)
{
    THERM_COMMAND_LOOKUP *tcl;
    int found = 0;
    char buf[100];

    tcl = ThermCommandLookup;
    UARTiDebug(header, DEBUG_THERMOSTAT);
    while((tcl->cmd1 != 0xff) && (!found))
    {
        if((tcl->cmd1 == cmd->data[1]) && (tcl->cmd2 == cmd->data[2]))
        {
            sprintf(buf, "seq:%05d idx:%02d %s", cmd->seq, index, tcl->command_name);
            UARTiDebug(buf, DEBUG_THERMOSTAT);
            found = 1;
        }
        tcl++;
    }

    if(!found)
    {
        UARTiDebug("Unknown Command", DEBUG_THERMOSTAT);
    }
}

void ThermGetPricingMessageBuffer(char *user_buf, int user_len)
{
    int len;

    // Steps to getting pricing message buffer
    // 1) Determine the less of the a) message buffer received, b) user buffer, c) message buffer expected
    // 2) Copy this amount to user buffer
    // 3) Null-terminate the user buffer

    // 1)
    len = strlen(PricingMessageBuffer);
    if(user_len < len)
    {
        len = user_len;
    }
    if(MAX_PRICE_MESSAGE_SIZE < len)
    {
        len = MAX_PRICE_MESSAGE_SIZE;
    }

    // 2)
    memcpy(user_buf, PricingMessageBuffer, len);

    // 3)
    user_buf[len] = 0;
}

int ThermCheckLastRsp(int check_command)
{
    if(check_command == iLastResponseProcessed)
    {
        iLastResponseProcessed = 0;
        return(1);
    }
    else if(iLastResponseProcessed != 0)
    {
        iLastResponseProcessed = 0;
    }
    return(0);
}

void ThermMessageReceiveTimeout()
{
    iThermMessageReceiveTimeout = 1;
}

#endif

