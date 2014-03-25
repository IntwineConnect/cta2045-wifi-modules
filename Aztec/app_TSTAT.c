#include "IntwineModule.h"

#ifdef INTWINE_PROGRAMMABLE_THERMOSTAT

// Non-library support
    #include "GCMSupport.h"

// put this after all defines have been made
    #include "GCMExterns.h"

/**
 * File: app.c
 */
//gcm #include "TCPIP Stack/TCPIP.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//gcm #include "TCPIP Stack/ZGLinkMgrII.h"
//gcm #include "TCPIP Stack/UART.h"
//gcm #include "TCPIP Stack/ZGCommon.h"
#include "EventTCPClient.h"
//gcm #include "BerkeleyUDPTimePool.h"
//gcm #include "time.h"
//gcm #include "netstatus.h"
//gcm #include "wificonfig.h"
#include "app.h"
#include "eventq.h"
#include "eeprom.h"
#include "Schedule.h"
#include "ThermComm.h"
#include "CBRunTime.h"
//#include "TimeMonitor.h"
//gcm #include "UpgradeClient.h"

#include "CBDataDevice_TSTAT.h"
#include "CBDataManager.h"
#include "Log_Manager.h"
#include "DFW.h"

//#ifdef ENABLE_SEP
static int sep_startup_counter=0;
static int sep_last_poll_time=0;
static int sep_current_time=0;
//#endif

int lostlink_flag=1; 

    #include "TimeMonitor.h"
void ProcessThermCommandLoop();
void test_start_comm_timeout(void);

// comm test
int start_comm_timeout_expired = 1;         // send right away
int comm_test_loop_completed = 0;

// Timers to control when actions happen.
// Used for sending property updates to the server.

char  gRespRec=0; // response received flag
// now in EventTCPClient.cDWORD SendTCPMsg_Timer=0; // Only Send TCP message 2 seconds after sending UDP msg. 
DWORD SendToServerTimeout=0; // Send every heartbeat.
DWORD MainSysTickTimer=0; // The Main System Tick Timer
DWORD MainSystemUNIXTime=0; // The Main System Time
DWORD WriteBackTimer=0;  // Timer to write back temp
DWORD TargDelayTimer=0;  // In R3, provide a bit of a delay while returning the value!
int SendToServer=0;
int SendOptOut=0;
int HANMsg_Rdy2Process=0;
extern char ipbufptr[30]; // IP Address Buffer
extern char changeIP;
extern int heartBeatTimer;
extern char h_flag;
//CB Vars
extern long int seq;
extern CB_VAR_UINT8   upgradeFlag;   // Upgrade Flag
extern CB_STATUS_STATE CBStatus; 

//DR Event Variables:
extern int InvalidSocketReboot;
//20120417 noeventq extern EVENT_Q eventQ[EVENTQSize];
char SendLocalHB=0;
int TCP_Message;
char eventInProgress=0;
DWORD SendCancelCommand=0; 
int cancelCommandCtr=0;
char cancelCommand=0;
char TryGetNextEvent=FALSE;
extern int eventRcvdFlag;  // If set, then go back and run another TCP Get real quick.
extern    DWORD SPI_Timer;
DWORD MainResetTimer=0;
char *curOptouthash;
extern int UpgradeTimeout;

int active_flag=1;  // Change to zero once into main loop to refresh props or preload thermo with props
DWORD ZG_Con_Timer=0;    // Timer inialized on bootup to track is ZG AP Link is up.
DWORD SPI_Watch_Timer=0;  // Timer inialized on bootup to track is CB Stack Link is up.
int spi_timeout=0;    // Initialized on bootup to check for SPI failure
int Scheduling=0;   // If in middle of scheduling, do not poll tstat 
char firstTime=1;   // Cleared the first time you get the targets in R4

extern int EventTCPClient( void );

/* void ProcessPricingMessage(void); */

static enum _ConnectState
{
    START =0,
    SPI_WORKING,
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
} ConnectState = START;

enum _CommState TCPCommState=DR_SERVER;

/** Check the thermostat status.
 *
 * We use a timer to throttle reads, so the host does not get overwhelmed.
 */
void ThermoCheckStatus(void)
{
    static int minute_counter=0;
    static int minute15_counter=0;
    static char group1_flag=0;
    static DWORD CheckTimer=0;
    DWORD cur_time;
    int run_loop = 0;
    static int poll_time_thermostat = POLL_TIME_THERMOSTAT_POLL_FIRST;

    // char pMsgText[]="Intwine Demonstration";	
//	char pRadioName[]="Intwine";	
//	char pNetworkName[]="Intwine";	

    cur_time = TimeMonitorTickGet();
    if(((cur_time - CheckTimer) >= (TIME_MONITOR_TICK_SECOND * 120)) && (CBStatus>CB_CHECK_PERSISTENT_ACK) && (Scheduling))
    {
        Scheduling=0;
    }

    if(CBStatus>CB_CHECK_PERSISTENT_ACK && !group1_flag)
    {
        // Group 1 read	
//cbdm		group1.status=0x01;  // "holdabs=0;wrtu=0;wrtu1=0;wrtp0=0;wrtp1=0;led=0;"
//cbdm		group1.crud='r';
//cbdm		group1.seqId=seq++;
//cbdm		CheckTimer=TimeMonitorTickGet();
//cbdm		group1_flag=1;

//JW stop sync with server after connectivity issue        cbSendAttributeToServerById(kCBAtt_group1);
        CheckTimer=cur_time;
        group1_flag=1;  

    }

//	if((cur_time - CheckTimer >= (TIME_MONITOR_TICK_SECOND * 7)) && (CBStatus>CB_CHECK_PERSISTENT_ACK) && (!Scheduling) && group1.val==0x00)

    if((cur_time - CheckTimer) >= (TIME_MONITOR_TICK_SECOND * poll_time_thermostat)) // last known was 30 seconds
    {
        if(CBStatus>CB_CHECK_PERSISTENT_ACK)
        {
            if(!Scheduling)
            {
                if(group1_flag==1)
                {
                   if(cbGetVal8(kCBAtt_group1)==0x00)
                   {
                       run_loop = 1;
                       poll_time_thermostat = POLL_TIME_THERMOSTAT_POLL_THE_REST;
                   }
                }
            }
        }
    }
    
//    if((cur_time - CheckTimer >= (TIME_MONITOR_TICK_SECOND * 15)) && (CBStatus>CB_CHECK_PERSISTENT_ACK) && (!Scheduling) && ((group1_flag==1) && (cbGetVal8(kCBAtt_group1)==0x00)))
    if(run_loop)
    {
        UARTiDebug("\rCheck TSTAT", DEBUG_ORIG);
#if defined (CT80)
        if(lostlink_flag==1)  // Set to run these commands once on bootup
        {
            cmd_WriteUserMsgArea(1, " ");   // Remove Error Messages
            lostlink_flag=0;
        }
//        else if(active_flag==1)  // Set to run these commands once on bootup
        if(active_flag==1)  // Set to run these commands once on bootup
#else
        if(active_flag==1)  // Set to run these commands once on bootup
#endif 
        {
#if defined (CT80)

            //cmd_WriteRadioNetworkName(pRadioName, pNetworkName); // 0x5041
            //cmd_UserMsgArea(0x01); // Turn on display			
#endif

/*       cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log */
            cmd_RadioTempOffset(); // Indicate present power usage of avg. of 375mW. Hardcoded Hex values in function.
            // cmd_ReadDataLog();  // Read Data Log so it will post all zeros to the datalog property
//      cmd_PricingMsgArea(0x01);
            // cmd_EnableRadioTempAbsSetpoints(); 
#ifdef USER_BUTTON_ENABLE_REMOTE_CONTROL
            cmd_RemoteControl(0x02);           // Enable remote control, once
#endif

//NOT REALLY USED      cmd_ReadRadioSlot();
            active_flag=0;
        }
        else
        {
//            cmd_ReadCurErr();           // Clear any previous errors
            cmd_ReadCurTemp();         // Check temperature.
            cmd_ReadCurHumidity();        // Check humidity
            cmd_ThermOpMode(CMD_READ, 0x00);  // Check thermostat mode
            // cmd_RadioTempAbsSetpoints(CMD_READ, 0x00,0x00);		// Check absolute cool and heat targets
            cmd_ReadCurTempTargets();     // Check cool and heat targets (these update on abs targets)
            cmd_FanOpMode(CMD_READ, 0x00);    // Check fan op mode
            if(changeIP)
            {
#if defined (CT80)
                ThermoLine2MsgAreaQ(ipbufptr, 5, 0);
#endif
                changeIP=0;
            }
            // cmd_LEDState(CMD_READ, 0x00);		// Read LEDs
            // cmd_ReadCurErr(); 						// Read the current error status
#if defined (CT80)
            cmd_HumiditySetpoint(CMD_READ, 0x00); // Read and post humidity setpoint
#endif

            // cmd_ReadTempOverride(); 			// Read temp override byte.
            cmd_ReadThermModeCfg();
            cmd_EventLog_DisableReadEnable();

            //  Routine to shut down processor for ~ 2minutes while watchdog expires...
            /* 
            PORTBbits.RB8=1;   // Shut off radio...
            LATFbits.LATF5 = 0;
            Sleep();
              PORTBbits.RB8=0;   */
            // Currently device goes down to 0.43mA the first time it goes here.  The upon restart it jumps to a lower amount.  
            // This may be due to the device getting wonky or the watchdog not being cleared etc...it stays at 0.43mA, 
            /// then jumps to 40, then resets to the normal 120mA...
            // Also shut down USB?  Probably is shut down by sleep, but not sure...

        }
        CheckTimer = cur_time;
        // This set to update on bootup or once every 2000 minutes.
        // CheckTime(); // Arrayent says must call < once a second. Move to outside loop, but not to conflict with test...
        minute_counter++;  // Increment minute counter 
        minute15_counter++;  // Increment 15 minute counter 

    }
    else
    {
//#ifdef ENABLE_SEP
        sep_current_time = cur_time;
        if((sep_current_time - sep_last_poll_time) > (TIME_MONITOR_TICK_SECOND * 10))
        {
            if(EnableSEP())
            {
                if(CBStatus<=CB_CHECK_PERSISTENT_ACK)
                {
                    // Not on cloudbus, do the following:
                    cmd_ReadCurTemp();               // Check temperature.
                    cmd_ReadCurHumidity();            // Check humidity
                    cmd_ThermOpMode(CMD_READ, 0x00);  // Check thermostat mode
                    cmd_ReadCurTempTargets();         // Check cool and heat targets (these update on abs targets)
                    cmd_FanOpMode(CMD_READ, 0x00);    // Check fan op mode
                    cmd_ReadThermModeCfg();           // Check hold mode
    
    /*                   ProcessPricingMessage(); */
    
                    sep_last_poll_time = sep_current_time;
                }
            }
            sep_startup_counter++;
        }
    }
//#endif
}

//cbdm/** Check If Unit is Connected to WiFi */
//cbdm// this does not appear to be used
//cbdmUINT8 CheckActive(UINT8* valPtr)
//cbdm{
//cbdm  // Target has changed so send update message to server.
//cbdm  activer.status=0x01;
//cbdm  activer.crud='u'; 
//cbdm  activer.seqId=seq++;
//cbdm  // cmd_WriteUserMsgArea(0, "Testing Comms...");
//cbdm  return true;
//cbdm}
//cbdm

// This function is designed to take ASCII data (representing HEX in ASCII) from the 
// Arrayent message and converting it into actual HEX for the thermostat to process.
// Returns a thermostat message structure.
// not in use
//cbdm CMD_RESP_DATA AsciiToHex1(UINT8* valPtr)
//cbdm {
//cbdm   UINT8   maxlen;
//cbdm   int i=0;
//cbdm   unsigned char HNib;
//cbdm   unsigned char LNib;
//cbdm   CMD_RESP_DATA ThermCmd;
//cbdm 
//cbdm   // Calculate length of data message.  If to long, truncate.
//cbdm   maxlen = strlen((char *)valPtr);
//cbdm 
//cbdm   for(i=0;i<(maxlen/2);i++)
//cbdm   {
//cbdm 
//cbdm     if(*valPtr > 0x40)
//cbdm     {
//cbdm       HNib = ((*valPtr - 0x37) << 4) & 0xF0;
//cbdm     }
//cbdm     else
//cbdm     {
//cbdm       HNib = ((*valPtr - 0x30) << 4) & 0xF0;
//cbdm     }
//cbdm 
//cbdm     valPtr=valPtr+1;
//cbdm 
//cbdm     if(*valPtr > 0x40)
//cbdm     {
//cbdm       LNib = (*valPtr - 0x37) & 0x0F;
//cbdm     }
//cbdm     else
//cbdm     {
//cbdm       LNib = (*valPtr - 0x30) & 0x0F;
//cbdm     }
//cbdm 
//cbdm     ThermCmd.data[i] = HNib | LNib;
//cbdm 
//cbdm     valPtr=valPtr+1;
//cbdm   }
//cbdm 
//cbdm   ThermCmd.data_len = maxlen/2;
//cbdm 
//cbdm   return ThermCmd; 
//cbdm 
//cbdm }
//cbdm 
//cbdmvoid ThermoSetCmd(char *valPtr)
//cbdm{
//cbdm  CMD_RESP_DATA ThermCmd;
//cbdm
//cbdm  // Load Hex command into datastructure
//cbdm  ThermCmd = AsciiToHex((BYTE *)valPtr);
//cbdm  // Send Command1 Structure to Thermostat
//cbdm  cmd_GenericCmd(ThermCmd);
//cbdm  return;
//cbdm}
//cbdm
// Write User Message ->Used to pull line num from property.
//cbdmvoid ThermoWriteUser(char *valPtr)
//cbdm{
//cbdm#ifdef CT80
//cbdm  ThermoLine1MsgAreaQ("add", valPtr,10,0);
//cbdm#endif
//cbdm  return;
//cbdm}
// Write Pricing Message
//cbdmvoid ThermoWritePricing(char *valPtr, UINT8 linenum)
//cbdm{
//cbdm  cmd_WritePricingMsgArea(linenum, (char *)valPtr);
//cbdm  return;
//cbdm}

/** Set thermostat time
 *
 * @param msgData  Pointer to message buffer from server.
 *                 Property should be in format "mode N", where
 *                 N is integer value for mode.
 * @param msgLen   Length of the message buffer.
 */ 
//rwb
/* void ThermoSetTime(long int time)                              */
/* {                                                              */
/*   UINT8 timemsb , timelsb;                                     */
/*   // Extract the time variable from the property.              */
/*   timelsb=time;                                                */
/*   timemsb=time >> 8;                                           */
/*   // Figure out what mode to set the thermostat.               */
/*   UARTiDebug("ThermoSetTime by SEP\r\n", DEBUG_ORIG);          */
/* //  cmd_CurTime(CMD_WRITE,timemsb, timelsb);                   */
/*   cbAttrSetByIdAutoSend32(kCBAtt_timem, (unsigned long) time); */
/* }                                                              */
/*                                                                */
/** Set the thermostat mode, from property set by server.
 *
 * @param msgData  Pointer to message buffer from server.
 *                 Property should be in format "mode N", where
 *                 N is integer value for mode.
 * @param msgLen   Length of the message buffer.
 */
//rwb
// moved to CBDataDevice_TSTAT.c
/* void ThermoSetMode(UINT8 *valPtr)                       */
/* {                                                       */
/*     UINT8   mode;                                       */
/*     // Extract the mode variable from the property.     */
/*     mode = atoi((char *)valPtr);                        */
/*      // Figure out what mode to set the thermostat.     */
/*     UARTiDebug("ThermoSetMode by SEP\r\n", DEBUG_ORIG); */
/*     cmd_ThermOpMode(CMD_WRITE, mode);                   */
/*     cbAttrSetByIdAutoSend8(kCBAtt_setMode, mode);       */
/* }                                                       */
/*                                                         */
//cbdm
//cbdm/** Set the thermostat mode, from property set by server. */
//cbdmvoid ThermoSetLed(UINT8 *valPtr)
//cbdm{
//cbdm  UINT8   nled;
//cbdm  // Extract the mode variable from the property.
//cbdm  nled = atoi((char *)valPtr);
//cbdm  thermoLed.val = nled;  // This might be in here for the install check.
//cbdm  thermoLed.status=0x01;
//cbdm  thermoLed.crud='u';
//cbdm  thermoLed.seqId=seq++;
//cbdm  cmd_LEDState(CMD_WRITE, nled);
//cbdm}
//cbdm

/** Rewrite thermostat absolute temp settings based on timing parameter from server. */
//rwb
/* void ThermoHoldAbs(UINT8 *valPtr)                                                                                             */
/* {                                                                                                                             */
/*     int holdabs_counterval;                                                                                                   */
/*   // Set global holdabs_counter which decrements on polling cycle to provide a timer that rewrites the absolute temperatures. */
/*                                                                                                                               */
/*   holdabs_counterval = atol((char *)valPtr);                                                                                  */
/*   UARTiDebug("ThermoHoldAbs by SEP \r\n", DEBUG_ORIG);                                                                        */
/*   UARTiDebug("\r\r Get Hold", DEBUG_ORIG);                                                                                    */
/*                                                                                                                               */
/* #ifndef R4                                                                                                                    */
/*   h_flag=0;  // Set it equal to zero for R3 face panel check.                                                                 */
/*   TargDelayTimer=TimeMonitorTickGet();                                                                                                   */
/* #endif                                                                                                                        */
/*                                                                                                                               */
/*   if(holdabs_counterval==540)                                                                                                 */
/*   {                                                                                                                           */
/*     UARTiDebug("\r\r Set Hold", DEBUG_ORIG);                                                                                  */
/*     cmd_EnableTempProgHold();                                                                                                 */
/*   }                                                                                                                           */
/*   else                                                                                                                        */
/*   {                                                                                                                           */
/*     cmd_DisTempProgHold();                                                                                                    */
/*   }                                                                                                                           */
/*   cmd_ReadThermModeCfg();                                                                                                     */
/*   cbAttrSetByIdAutoSend16(kCBAtt_holdEnabled, holdabs_counterval);                                                            */
/*                                                                                                                               */
/* }                                                                                                                             */
/*                                                                                                                               */
//rwb
/* void ThermoSetDay(UINT8 *valPtr)                      */
/* {                                                     */
/*   UINT8   day;                                        */
/*   // Extract the mode variable from the property.     */
/*   day = atoi((char *)valPtr);                         */
/*   UARTiDebug("ThermoSetDay by SEP \r\n", DEBUG_ORIG); */
/* //  cmd_CurDay(CMD_WRITE, day);                       */
/*   cbAttrSetByIdAutoSend8(kCBAtt_thermoDay, day);      */
/* }                                                     */
//cbdm
//cbdm/** Set the target setback temp, from property set by server. */
//cbdmvoid ThermoSetSetback(UINT8 *valPtr)
//cbdm{
//cbdm  UINT8 nsetback;
//cbdm  nsetback = atoi((char *)valPtr);
//cbdm  nsetback=nsetback*2;   // Multiply by two to get a whole degree setback.
//cbdm  cmd_RadioTempTargetSetback(CMD_WRITE, nsetback);
//cbdm  cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by server
//cbdm  setback.status=0x01;
//cbdm  setback.crud='u';
//cbdm  setback.seqId=seq++;
//cbdm}
//cbdm
//cbdm
//cbdm/** Set the abs target temperatures, from property set by server. */
//rwb
/* void ThermoSetAbsTargets(UINT8 *valPtr)                                                                                      */
/* {                                                                                                                            */
/*   UINT8   targetCool;                                                                                                        */
/*   UINT8   targetHeat;                                                                                                        */
/*   char buf[100];                                                                                                             */
/*                                                                                                                              */
/* #ifndef R4                                                                                                                   */
/*   h_flag=0;  // Set it equal to zero for R3 face panel check.                                                                */
/*   TargDelayTimer=TimeMonitorTickGet();                                                                                                  */
/* #endif                                                                                                                       */
/*                                                                                                                              */
/*   unsigned long tempTargetsval;                                                                                              */
/*   int holdabs_counterval;                                                                                                    */
/* //  BYTE setbackval;                                                                                                         */
/*                                                                                                                              */
/*   tempTargetsval = atol((char *)valPtr);                                                                                     */
/*   cbAttrSetByIdAutoSend32(kCBAtt_tempTargets, tempTargetsval);                                                               */
/*                                                                                                                              */
/*   holdabs_counterval = cbGetVal16(kCBAtt_holdEnabled);                                                                       */
/* //  setbackval = cbGetVal8(kCBAtt_setback);                                                                                  */
/*                                                                                                                              */
/*  // thermoAbsTargets.status=0x01; */
/*  // thermoAbsTargets.crud='u';    */
/*  // thermoAbsTargets.seqId=seq++; */
/*                                                                                                                              */
/*   UARTiDebug("\r\r Set TARGETS....", DEBUG_ORIG);                                                                            */
/*   if(tempTargetsval>0)                                                                                                       */
/*   {                                                                                                                          */
/*     targetHeat = tempTargetsval;                                                                                             */
/*     targetCool = tempTargetsval >> 8;                                                                                        */
/*                                                                                                                              */
/* //    targetHeat=targetHeat-setbackval;                                                                                      */
/* //    targetCool=targetCool+setbackval;                                                                                      */
/*                                                                                                                              */
/*     sprintf(buf, "\r\r ThermoSetAbsTargets by SEP: 0x%04lx (Cool:%d, Heat:%d)\r\n", tempTargetsval, targetCool, targetHeat); */
/*     UARTiDebug(buf, DEBUG_ORIG);                                                                                             */
/*                                                                                                                              */
/*     targetHeat = targetHeat << 1;                                                                                            */
/*     targetCool = targetCool << 1;                                                                                            */
/* #ifndef R4                                                                                                                   */
/*     if(firstTime==1)                                                                                                         */
/*     {                                                                                                                        */
/*       if(holdabs_counterval > 0)  // If we have a hold value in place, then write the temps, else don't                      */
/*       {                                                                                                                      */
/* //using 20:20        cmd_EnableRadioTempAbsSetpoints();                                                                      */
/*         cmd_RadioTempTargetSetpoints(CMD_WRITE,targetCool, targetHeat);                                                      */
/*         // cmd_ReadCurTempTargets();    // This sends latest information back up to "targets" prop which is read by server   */
/*       }                                                                                                                      */
/*                                                                                                                              */
/*     }                                                                                                                        */
/*     else                                                                                                                     */
/*     {                                                                                                                        */
/* //using 20:20      cmd_EnableRadioTempAbsSetpoints();                                                                        */
/*       cmd_RadioTempTargetSetpoints(CMD_WRITE,targetCool, targetHeat);                                                        */
/*       // cmd_ReadCurTempTargets();  // This sends latest information back up to "targets" prop which is read by server       */
/*     }                                                                                                                        */
/*     firstTime=0;                                                                                                             */
/* #else                                                                                                                        */
/*     UARTiDebug("\r\r Set TARGETS 2....", DEBUG_ORIG);                                                                        */
/*     if(firstTime==1)                                                                                                         */
/*     {                                                                                                                        */
/*       if(holdabs_counterval)                                                                                                 */
/*       {                                                                                                                      */
/*         UARTiDebug("\r\r Set TARGETS 3....", DEBUG_ORIG);                                                                    */
/* //using 20:20        cmd_EnableRadioTempAbsSetpoints();                                                                      */
/*         cmd_RadioTempTargetSetpoints(CMD_WRITE,targetCool, targetHeat);                                                      */
/*         cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by server         */
/*       }                                                                                                                      */
/*     }                                                                                                                        */
/*     else                                                                                                                     */
/*     {                                                                                                                        */
/*       UARTiDebug("\r\r Set TARGETS 4....", DEBUG_ORIG);                                                                      */
/* //using 20:20      cmd_EnableRadioTempAbsSetpoints();                                                                        */
/*       cmd_RadioTempTargetSetpoints(CMD_WRITE,targetCool,targetHeat);                                                         */
/*       cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by server           */
/*     }                                                                                                                        */
/*     firstTime=0;                                                                                                             */
/* #endif                                                                                                                       */
/*   }                                                                                                                          */
/* }                                                                                                                            */
/*                                                                                                                              */
/** Set the abs target temperatures, from property set by server. */
/*
void ThermoSetTempTargets(UINT8 *valPtr)
{
    UINT8   targetCool;
    UINT8   targetHeat;

  // holdabs_counter=0; // Reset counter to default of 10 minutes of hold time.

  thermoTempTargets.val = atol(valPtr);
  thermoTempTargets.status=0x01;
  thermoTempTargets.crud='u';
  thermoTempTargets.seqId=seq++;

  if(thermoTempTargets.val>0)
  {
    targetHeat = thermoTempTargets.val;	
    targetCool = thermoTempTargets.val >> 8;
    
    targetHeat=targetHeat-setback.val;
    targetCool=targetCool+setback.val;
    
    targetHeat = targetHeat << 1;
    targetCool = targetCool << 1;
    cmd_RadioTempTargetSetpoints(CMD_WRITE,targetCool, targetHeat);
    cmd_ReadCurTempTargets();	// This sends latest information back up to "targets" prop which is read by server
  }
}
*/

//rwb
// moved to CBDataDevice_TSTAT.c
/* void ThermoSetFanMode(UINT8 *valPtr)                                                  */
/* {                                                                                     */
/*   UINT8 fanmode;                                                                      */
/*   fanmode = atoi((char *)valPtr);                                                     */
/*   UARTiDebug("ThermoSetFanMode by SEP \r\n", DEBUG_ORIG);                             */
/*   cmd_FanOpMode(CMD_WRITE,fanmode);  // if zero, this function parses and bytestuffs. */
/*   cbAttrSetByIdAutoSend8(kCBAtt_fanMode, fanmode);                                    */
/* }                                                                                     */
//cbdm
//cbdmvoid ThermoSaveEnergy(char *valPtr)
//cbdm{
//cbdm  UINT8 nsavenrg;
//cbdm  nsavenrg = atoi((char *)valPtr);
//cbdm  savenrg.val=nsavenrg;
//cbdm  savenrg.status=0x01;
//cbdm  savenrg.crud='u';
//cbdm  savenrg.seqId=seq++;
//cbdm  cmd_SaveEnergyMode(nsavenrg);
//cbdm}
//cbdm
//cbdmvoid ThermoLock(UINT8 *valPtr)
//cbdm{
//cbdm  UINT8 lock;
//cbdm  lock = atoi((char *)valPtr);
//cbdm  if(lock==0) cmd_DisRadioUtilityLock();
//cbdm  if(lock==1)
//cbdm  {
//cbdm    cmd_EnableRadioUtilityLock(); 
//cbdm    thermoLock.val=lock;  // Set var to global variable.
//cbdm    thermoLock.status=0x01;
//cbdm    thermoLock.crud='u';
//cbdm    thermoLock.seqId=seq++;
//cbdm    thermoLockmin.val=0;  // Refresh lock counter (set to remove after 15 minutes...)
//cbdm    cmd_WriteUserMsgArea(0, "Thermostat is Locked");
//cbdm  }
//cbdm}
//cbdm
//cbdmvoid ThermoEnaDisRemoteControl(char *valPtr) // Turn on and off Remote Control
//cbdm{
//cbdm  UINT8 rem;
//cbdm  rem = atoi((char *)valPtr);
//cbdm  if(rem==0) cmd_RemoteControl(0x01); // SetRemoteControl ON
//cbdm  if(rem==1)cmd_RemoteControl(0x02);  // SetRemoteControl OFF
//cbdm  rem=remote.val;  // Set new fan mode into global variable.
//cbdm  remote.status=0x01;
//cbdm  remote.crud='u';
//cbdm  remote.seqId=seq++;
//cbdm}
//cbdm
//cbdmvoid ThermoSetRemoteTemp(char *valPtr) 
//cbdm{
//cbdm  UINT8 rem;
//cbdm  rem = (atoi((char *)valPtr)*2); // Multiply by two to get real value!
//cbdm  cmd_RemoteTemp(CMD_WRITE, rem);
//cbdm  rem=rmttemp.val; // Set new Remote Temp Value into global
//cbdm  // .. rem above is not global
//cbdm  rmttemp.status=0x01;
//cbdm  rmttemp.crud='u';
//cbdm  rmttemp.seqId=seq++;  
//cbdm}
//cbdm
//cbdmvoid ThermoSetHsetptControl(char *valPtr)  // Turn on and off Remote Control
//cbdm{
//cbdm  UINT8 nhsetpt;
//cbdm  nhsetpt = atoi((char *)valPtr);
//cbdm  cmd_HumiditySetpoint(CMD_WRITE, nhsetpt);
//cbdm  nhsetpt=hsetpt.val;  // Set new humidity setpt into global variable.
//cbdm  // .. nhsetpt above is not global
//cbdm  hsetpt.status=0x01;
//cbdm  hsetpt.crud='u';
//cbdm  hsetpt.seqId=seq++;
//cbdm}
//cbdm
//cbdmvoid ThermoSetUserOverride(char *valPtr) // Control User Override
//cbdm{ 
//cbdm  UINT8 nuover;
//cbdm  nuover = atoi((char *)valPtr);
//cbdm  cmd_IncDecUserOverride(nuover);
//cbdm  override.val=nuover;
//cbdm  override.status=0x01;
//cbdm  override.crud='u';
//cbdm  override.seqId=seq++;
//cbdm} 
//cbdm
//cbdmvoid ThermoSetAbsEn(char *valPtr) // Control Abs Temp Enable
//cbdm{ 
//cbdm  UINT8 nabsen;
//cbdm  nabsen = atoi((char *)valPtr);
//cbdm#ifndef R4
//cbdm
//cbdm  //	if(nabsen==1)cmd_EnableRadioTempAbsSetpoints();
//cbdm  //	else cmd_DisableRadioTempAbsSetpoints();
//cbdm#endif 
//cbdm  absen.val = nabsen;
//cbdm  absen.status=0x01;
//cbdm  absen.crud='u';
//cbdm  absen.seqId=seq++;
//cbdm#ifdef R4	
//cbdm  thermoTargetTemps.val=0;
//cbdm  cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by server
//cbdm#endif
//cbdm} 
//cbdm
//cbdmvoid ThermoEnLog(char *valPtr) // Enable Data Logging
//cbdm{ 
//cbdm  enlog.val = atoi((char *)valPtr);
//cbdm  enlog.status=0x01;
//cbdm  enlog.crud='u';
//cbdm  enlog.seqId=seq++;
//cbdm  // if enlog==1 log data, else don't
//cbdm} 

void ThermoReset()
{ 
// add "ResetVal" to CBDM and call a function to reset
    // all attributes back to thei default value

//cbdm  thermoCurrTemp.val = 0;
//cbdm  thermoMode.val = 9;
//cbdm  thermoTargetTemps.val = 0;
//cbdm  thermoAbsTargets.val = 0;
//cbdm  thermoCurrHumidity.val = 0;
//cbdm  thermoFanMode.val = 9;
//cbdm  thermoCmdState.val = 0;
//cbdm  thermoDay.val = 9;
//cbdm  thermoLed.val = 9;
//cbdm  thermoError.val = 0;
//cbdm  thermo.val = 9;
//cbdm  thermoLock.val=0;
//cbdm  thermoLockmin.val=0;
//cbdm  active.val = 9;
//cbdm  DLogBuffer.val[1] = 'Q';
//cbdm  tempDLogBuffer.val[1] = 'Q';
//cbdm  ELogBuffer.val[1] = 'Q';
//cbdm  tempELogBuffer.val[1] = 'Q';
//cbdm  HoldMode.val = 0;
//cbdm  holdabs_counter.val = 0;
//cbdm  rssi.val=0;
//cbdm  batt.val=0;
//cbdm  remote.val=0;
//cbdm  hsetpt.val=0;
//cbdm  override.val=0;
//cbdm  rmttemp.val=0;
//cbdm  rmttemps.val=0;
//cbdm  absen.val=0;
//cbdm  lostlink_flag=0;
//cbdm  enlog.val=0;
//cbdm  W.val=9;
//cbdm  W2.val=9;
//cbdm  W3.val=9;
//cbdm  Y.val=9;
//cbdm  Y2.val=9;
//cbdm  G.val=9;
//cbdm
//cbdm#ifndef WAIT_TO_IMPLEMENT
//cbdm  ConfigBuffer.val[1] = 'Q';
//cbdm  tempConfigBuffer.val[1] = 'Q';
//cbdm  slot.val=0;
//cbdm#endif

    firstTime=1;

}

//not in use void ThermoProcessMacro(char *valPtr)
//not in use {
//not in use   int i=0;
//not in use   static unsigned char ProgOpMode;
//not in use   static unsigned char ProgSetPointEnabled = 0;
//not in use   static unsigned int ProgAbsSetpoints;
//not in use   unsigned char targetHeat;
//not in use   unsigned char targetCool;
//not in use 
//not in use   unsigned long thermoAbsTargetsVal = cbGetVal32(kCBAtt_thermoAbsTargets);
//not in use   unsigned char thermoCurrTempVal   = cbGetVal8(kCBAtt_thermoCurrTemp);
//not in use   int           holdabs_counterVal  = cbGetVal16(kCBAtt_holdEnabled);
//not in use   unsigned char thermoModeVal       = cbGetVal8(kCBAtt_thermoMode);
//not in use 
//not in use   // Read the current mode and store in the mode.
//not in use   // cmd_ThermOpMode(CMD_READ, 0x00);
//not in use 
//not in use   if(!strncmp((char*)(valPtr), "ProgHeatBegin", 13))
//not in use   {
//not in use #ifdef R4
//not in use     {
//not in use       Scheduling=1;  // Tell main poll loop to stop running until scheduling is done.
//not in use       return;
//not in use     }
//not in use #endif
//not in use     // store therm op mode and abs setpoints
//not in use     ProgOpMode = thermoModeVal;  // Not polling for that value so on first boot, messed up. 
//not in use     ProgAbsSetpoints = thermoAbsTargetsVal;
//not in use     // store abs setpoints enabled setting
//not in use     // ProgSetpointEnabled = ???
//not in use     // set abs setpoints so both heat and cool are off (heat lower than cur temp and cool lower than cool temp)
//not in use     targetHeat = (thermoCurrTempVal - 2)<<1;
//not in use     targetCool = (thermoCurrTempVal + 2)<<1;
//not in use     cmd_EnableRadioTempAbsSetpoints(); // enable abs setpoints
//not in use     cmd_RadioTempAbsSetpoints(CMD_WRITE, targetCool, targetHeat, 0);
//not in use     cmd_ThermOpMode(CMD_WRITE, 0x01); // 0x5008 to set mode (heat or cool) to match program sched to be changed
//not in use     // end of macro - write 7 days from web server
//not in use #ifdef R4
//not in use     if(holdabs_counterVal)
//not in use     {
//not in use       ProgSetPointEnabled = 1;
//not in use     }
//not in use     else
//not in use     {
//not in use       ProgSetPointEnabled=0;
//not in use     }
//not in use #endif
//not in use   }
//not in use   else if(!strncmp((char*)(valPtr), "ProgHeatEnd", 11))
//not in use   {
//not in use #ifdef R4
//not in use     {
//not in use       return;
//not in use     }
//not in use #endif
//not in use     targetHeat = ProgAbsSetpoints;  
//not in use     targetCool = ProgAbsSetpoints >> 8;
//not in use     targetHeat = targetHeat << 1;
//not in use     targetCool = targetCool << 1;
//not in use     cmd_RadioTempAbsSetpoints(CMD_WRITE, targetCool, targetHeat, 0);// restore abs setpoints
//not in use #ifndef R4
//not in use     if(ProgSetPointEnabled == 0)// restore abs setpoint enable
//not in use     {
//not in use       cmd_DisableRadioTempAbsSetpoints();
//not in use     }
//not in use     else
//not in use     {
//not in use       cmd_EnableRadioTempAbsSetpoints(); 
//not in use     }
//not in use #else
//not in use     cmd_DisableRadioTempAbsSetpoints();
//not in use #endif
//not in use   }
//not in use   else if(!strncmp((char*)(valPtr), "ProgCoolBegin", 13))
//not in use   {
//not in use #ifdef R4
//not in use     {
//not in use       return;
//not in use     }
//not in use #endif
//not in use     // store therm op mode and abs setpoints
//not in use     // store abs setpoints enabled setting
//not in use     // ProgSetpointEnabled = ???
//not in use     // set abs setpoints so both heat and cool are off (heat lower than cur temp and cool lower than cool temp)
//not in use     targetHeat = (thermoCurrTempVal - 2)<<1;
//not in use     targetCool = (thermoCurrTempVal + 2)<<1;
//not in use     cmd_RadioTempAbsSetpoints(CMD_WRITE, targetCool, targetHeat, 0);
//not in use     cmd_EnableRadioTempAbsSetpoints(); // enable abs setpoints
//not in use     cmd_ThermOpMode(CMD_WRITE, 0x02); // 0x5008 to set mode (heat or cool) to match program sched to be changed
//not in use     // end of macro - write 7 days from web server
//not in use   }
//not in use   else if(!strncmp((char*)(valPtr), "ProgCoolEnd", 11))
//not in use   {
//not in use #ifdef R4
//not in use     {
//not in use       Scheduling=0;  // Tell main poll loop to stop running until scheduling is done.
//not in use       return;
//not in use     }
//not in use #endif
//not in use     targetHeat = ProgAbsSetpoints;  
//not in use     targetCool = ProgAbsSetpoints >> 8;
//not in use     targetHeat = targetHeat << 1;
//not in use     targetCool = targetCool << 1;
//not in use 
//not in use #ifdef R4
//not in use     if(ProgSetPointEnabled)
//not in use     {
//not in use 
//not in use       cmd_RadioTempAbsSetpoints(CMD_WRITE, targetCool, targetHeat, 0);// restore 0x2020 
//not in use     }
//not in use     cmd_DisableRadioTempAbsSetpoints();
//not in use #endif
//not in use 
//not in use     cmd_ThermOpMode(CMD_WRITE, ProgOpMode); // restore therm op mode
//not in use 
//not in use #ifndef R4
//not in use     if(ProgSetPointEnabled == 0)// restore abs setpoint enable
//not in use     {
//not in use       cmd_DisableRadioTempAbsSetpoints();
//not in use     }
//not in use     else
//not in use     {
//not in use       cmd_EnableRadioTempAbsSetpoints(); 
//not in use     }
//not in use #endif
//not in use 
//not in use 
//not in use     Scheduling=0;
//not in use   }
//not in use 
//not in use   else if(!strncmp((char*)(valPtr), "ReadProgH", 9))
//not in use   {
//not in use //cbdm does not appear to be used    schedsel.val[0]='h';  
//not in use     for(i=0;i<7;i++)
//not in use     {
//not in use       cmd_ReadThermProgData(i,1);
//not in use     }
//not in use   }
//not in use   else if(!strncmp((char*)(valPtr), "ReadProgC", 9))
//not in use   {
//not in use //cbdm does not appear to be used    schedsel.val[0]='c';  
//not in use     for(i=0;i<7;i++)
//not in use     {
//not in use       cmd_ReadThermProgData(i,2);
//not in use     }
//not in use   }
//not in use   /* 	else if (!strncmp((char*)(valPtr), "ReadELog", 8))
//not in use     {	
//not in use     cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // disable event log before reading
//not in use     cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00); // read first entry
//not in use   }
//not in use   else if (!strncmp((char*)(valPtr), "canceldr", 8))
//not in use     {	
//not in use     cmd_RadioTempTargetSetback(CMD_WRITE, 0x00);  // Setback is 0
//not in use     cmd_DisRadioUtilityLock(); // Set Utility Lock
//not in use     thermoLock.val=0;
//not in use   }  */
//not in use }

    #ifdef CT80 
void ThermoLine1MsgAreaQ(char *crud, char *displayBytes, int dispSec, int qLoc)
{
    static int lineQctr=0;
    static char lineQ[5][32];
    static int lineQdispSec[5];
    static DWORD DispTimer=0;
    int i=0;

    if(!strcmp(crud,"init"))
    {
        for(i=0;i<5;i++)
        {
            lineQ[i][0]='\0';
        }
    }
    else if(!strcmp(crud,"delete"))
    {
        lineQ[qLoc][0]='\0';
        lineQdispSec[qLoc]=dispSec;
        cmd_WriteUserMsgArea(0, " ");
    }
    else if(!strcmp(crud,"add"))  // If not equal to display
    {
        memcpy(lineQ[qLoc],displayBytes,strlen(displayBytes));
        lineQ[qLoc][strlen(displayBytes)]='\0';
        lineQdispSec[qLoc]=dispSec;
    }
    else if(!strcmp(displayBytes,"display"))  // If not equal to display
    {
        //
    }
    else
    {
    }

    if((TimeMonitorTickGet() - DispTimer) > TIME_MONITOR_TICK_SECOND*lineQdispSec[lineQctr])
    {
        if((lineQ[lineQctr][0] != '\0') && (lineQ[lineQctr][0] != 0x20))
        {
            cmd_WriteUserMsgArea(0, lineQ[lineQctr]);
        }
        lineQctr++;
        if(lineQctr >=5 )
        {
            lineQctr=0;
        }
        DispTimer=TimeMonitorTickGet();  
    }
}

void ThermoLine2MsgAreaQ(char *displayBytes, int dispSec, int qLoc)
{
    static int lineQctr=0;
    static char lineQ[5][32];
    static int lineQdispSec[5];
    static DWORD DispTimer=0;
    int i=0;

    if(!strcmp(displayBytes,"init"))
    {
        for(i=0;i<5;i++)
        {
            lineQ[i][0]='\0';
        }
    }
    else if(!strcmp(displayBytes,"delete"))
    {
        lineQ[qLoc][0]='\0';
        lineQdispSec[qLoc]=dispSec;
        cmd_WriteUserMsgArea(0, " ");
    }
    else if(strcmp(displayBytes,"display")) // If not equal to display
    {
        memcpy(lineQ[qLoc],displayBytes,strlen(displayBytes));
        lineQ[qLoc][strlen(displayBytes)]='\0';
        lineQdispSec[qLoc]=dispSec;
    }

    if((TimeMonitorTickGet() - DispTimer) > TIME_MONITOR_TICK_SECOND*lineQdispSec[lineQctr])
    {
        if(lineQ[lineQctr][0] != '\0')
        {
            cmd_WriteUserMsgArea(1, lineQ[lineQctr]);
        }
        lineQctr++;
        if(lineQctr >=5 )
        {
            lineQctr=0;
        }
        DispTimer=TimeMonitorTickGet();  
    }
}
    #endif
// This processes non-universal (application specific) CloudBUS properties and is called by the 
// main CloudBUS routines
void CBProcessRcvdMessages(char *valPtr)
{

//  unsigned char msg[100];

//cbdm	// Time after midnight
//cbdm	valPtr=Val("timem");
//cbdm	if(valPtr!=NULL && (*valPtr != '*'))
//cbdm	{
//cbdm		long int timem;
//cbdm		UARTiDebug("\rSET TIME");
//cbdm		UARTiDebug((char *)valPtr);
//cbdm		timem=atol(valPtr);
//cbdm		ThermoSetTime(timem);
//cbdm		// Device will never update this
//cbdm	}
//cbdm
//cbdm  // The day: Value is encoded: 0 = Monday, 1 = Tueday to 6 = Sunday.
//cbdm  valPtr=Val(PROP_NAME_DAY);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    int day;
//cbdm    UARTiDebug("day=");
//cbdm    UARTiDebug((char *)valPtr);
//cbdm    day=atoi((char *)valPtr);
//cbdm    ThermoSetDay(valPtr);
//cbdm    // Device will never update this.
//cbdm  }

//reset not used
//cbdm  valPtr=Val("reset");
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    UARTiDebug("reset=");
//cbdm    UARTiDebug((char *)valPtr);
//cbdm    ThermoReset();
//cbdm    // Sent from cloud - send an update with the time it reset in UTC
//cbdm    sprintf(msg,"reset=%ld;",CB_UTC_Time('r', 0));
//cbdm    reset.status=0x01;
//cbdm    reset.crud='u';
//cbdm    reset.seqId=seq++;
//cbdm  }

//cbdm  valPtr=Val(PROP_NAME_MODE);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    if(setback.val)
//cbdm    {
//cbdm      SendOptOut=1;
//cbdm    }
//cbdm    ThermoSetMode(valPtr);
//cbdm    cmd_ReadCurTempTargets();     // Check cool and heat targets (these update on abs targets)
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // Disable Event Log so we can Read It
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00);  // Read Event Log
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log so we can log again.
//cbdm  }

    valPtr=Val("event");
    if(valPtr!=NULL && (*valPtr != '*'))
    {
        UARTiDebug("Event ", DEBUG_ORIG);
        UARTiDebug((char *)valPtr, DEBUG_ORIG);
        UARTiDebug("End Event \r\n", DEBUG_ORIG);
        // Process Event Here.
    }

    // This needs to be processed above the setting of the abstargets.
//cbdm  valPtr=Val(PROP_NAME_HOLDABS);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoHoldAbs(valPtr);  // Write the Hold Value. 
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_ABSTARGETS);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    UARTiDebug("Targets");
//cbdm    UARTiDebug((char *)valPtr);
//cbdm    UARTiDebug("End Targets \r\n");
//cbdm    if(setback.val)
//cbdm    {
//cbdm      SendOptOut=1;
//cbdm    }
//cbdm    ThermoSetAbsTargets(valPtr);   
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // Disable Event Log so we can Read It
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00);  // Read Event Log
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log so we can log again.
//cbdm  }
/*
  valPtr=Val(PROP_NAME_TEMP_TARGETS);
  if(valPtr!=NULL && (*valPtr != '*'))
  {
    UARTiDebug("Temp Targets");
    UARTiDebug((char *)valPtr);
    UARTiDebug("End Targets \r\n");
    if(setback.val)
    {
      SendOptOut=1;
    }
    ThermoSetTempTargets(valPtr);   
//    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // Disable Event Log so we can Read It
//    cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00);  // Read Event Log
//    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log so we can log again.
  }
*/
//cbdm	valPtr=Val(PROP_NAME_ACTIVE);
//cbdm	if(valPtr!=NULL && (*valPtr != '*'))
//cbdm	{
//cbdm		// ThermoReset();
//cbdm		UARTiDebug("active=");
//cbdm		UARTiDebug((char *)valPtr);
//cbdm		active.status=0x01;
//cbdm		active.crud='u';
//cbdm		active.seqId=seq++;
//cbdm	} 
//cbdm
//cbdm  valPtr=Val(PROP_NAME_SETBACK);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSetSetback(valPtr);
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // Disable Event Log so we can Read It
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00);  // Read Event Log
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log so we can log again.
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_FANMODE);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSetFanMode(valPtr);
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // Disable Event Log so we can Read It
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00);  // Read Event Log
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log so we can log again.
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_LED);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSetLed(valPtr);
//cbdm  }
//cbdm
//cbdm not used  valPtr=Val(PROP_NAME_DAY0);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY1);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY2);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY3);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY4);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY5);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY6);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_DAY7);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
//cbdm not used  valPtr=Val(PROP_NAME_CMD3);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    ThermoSetCmd(valPtr);
//cbdm not used  }
//cbdm not used
    // ADD MORE CMDS HERE.....
//cbdm not used  valPtr=Val(PROP_NAME_CHKSPI);
//cbdm not used  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm not used  {
//cbdm not used    cmd_LEDState(CMD_READ, 0x00);   // Read LEDs
//cbdm not used  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_MACRO);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoProcessMacro(valPtr); // Macro property value may trigger multiple sequential commands to be sent to tstat
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_LOCK);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoLock(valPtr); // Lock the thermostat
//cbdm  }

//cbdm  valPtr=Val(PROP_NAME_WRTU0);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoWriteUser(valPtr); // Write User Message Line 0
//cbdm  }
//cbdm
//cbdm  /* valPtr=Val(PROP_NAME_WRTU1);
//cbdm  if(valPtr!=NULL)
//cbdm  {
//cbdm      ThermoWriteUser(valPtr,0x01); // Write User Message Line 1
//cbdm    } */
//cbdm
//cbdm  valPtr=Val(PROP_NAME_WRTP0);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoWritePricing(valPtr,0x00); // Write Pricing Message
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_WRTP1);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoWritePricing(valPtr,0x01); // Write Pricing Message
//cbdm  }

//cbdm  valPtr=Val(PROP_NAME_REMOTE);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoEnaDisRemoteControl(valPtr); // Turn on and off Remote Control
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_HSETPT);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSetHsetptControl(valPtr); // Control Humidity Setpoint
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_RMTTEMP);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSetRemoteTemp(valPtr); // Control Remote Temp Setpoint
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_UOVER);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSetUserOverride(valPtr); // Inc. / Dec. User Override.
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_ABSEN);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm#ifdef R4
//cbdm    return;
//cbdm#endif
//cbdm    ThermoSetAbsEn(valPtr); // Set Abs Temp Enable / Disable
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x01); // Disable Event Log so we can Read It
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_READ, 0x00);  // Read Event Log
//cbdm    cmd_ReadSetEnableEventDataLog(CMD_WRITE, 0x02); // Enable Event Log so we can log again.
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_ENLOG);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoEnLog(valPtr); // Set Loggin Enable / Disable
//cbdm  }
//cbdm
//cbdm  valPtr=Val(PROP_NAME_SAVENRG);
//cbdm  if(valPtr!=NULL && (*valPtr != '*'))
//cbdm  {
//cbdm    ThermoSaveEnergy(valPtr);
//cbdm  }
} 

/****************************************************************************/
// Perform application related initialization here.
void AppTaskInit(void)
{

    // Initialize thermostat state.
    ThermoInit();
    ZG_Con_Timer = TimeMonitorTickGet();
    SPI_Watch_Timer = TimeMonitorTickGet();
    TCP_Message=SEND_HB;
//20120417 noeventq     ReadEventQ();
//20120417 noeventq     WriteEventQ(); // Kluge to write back zeros to the EEPROM the first time it will be filled with FFs.
//20120417 noeventq     ReadEventQ(); // Kluge to read them back into memory.
    SwitchToLongHeartBeat(1); // Make this update for first 15 minutes.

//#ifdef ENABLE_SEP
    if(EnableSEP())
    {
        InitSEP20Comm();
    }
//#endif

//gcm  InitHomeAreaNet();
#ifdef CT80
#ifndef THERMOSTAT_COMMUNICATION_TEST
    cmd_UserMsgArea(0x01); // Turn on display	
    ThermoLine1MsgAreaQ("init", "", 0, 0);
#endif
#endif

//  ThermoReset();
//MWN   upgradeFlag.status=0x01;  // Check for an upgrade each time you reboot
    return;
}



void AppTaskLoop(void)
{
//not currently used  int i,j; // generic counter
    char timestring[35];
    static int timeSynch=0; // 1 if valid timeSynch, 0 if not.
    static DWORD schedule_process_time = 0;
//    static DWORD schedule_just_set_time = 0;
    DWORD cur_time;

#ifndef THERMOSTAT_COMMUNICATION_TEST
    switch(ConnectState)
    {
        case START:
            if((TimeMonitorTickGet() - SPI_Watch_Timer >= TIME_MONITOR_TICK_SECOND*20))
            {
                //	asm("reset");
            }
            if(spi_timeout==1)
            {
#ifdef CT80
                cmd_WriteUserMsgArea(1, "SPI WORKING");
                cmd_RadioMsgAnn(0x00);  // Turn off beeper
#endif

                //cmd_WriteUserMsgArea(1, ipbufptr);
                ConnectState=SPI_WORKING;
            }
            ProcessThermCmds();  // Always process SPI Commands
//mwn-RWB, should this really return
            return;
            break;

        case SPI_WORKING:
            if(ZG_IS_CONNECTED() && (ZG_GET_MODE() == kZGLMNetworkModeInfrastructure) )
            {
                ConnectState=WIFI_CONNECTED;
                cmd_WriteUserMsgArea(1, "WIFI CONNECTED");
                //cmd_WriteUserMsgArea(1, ipbufptr);
            }
            else if(ZG_IS_CONNECTED() && (ZG_GET_MODE() == kZGLMNetworkModeAdhoc) )
            {
                ConnectState=WIFI_CONNECTED;
#ifdef CT80
                cmd_WriteUserMsgArea(1, "WIFI IN ADHOC MODE");
#endif
                return;
            }
            else if(((TimeMonitorTickGet() - ZG_Con_Timer > TIME_MONITOR_TICK_SECOND*20)))
            {
#ifdef CT80
                cmd_WriteUserMsgArea(1, "Searching for AP...");  //Added Feb. 15th so you know it is working...
#endif
                ConnectState=WIFI_DISCONNECTED;  // Added Jan. 26th.
            }
            break;
        case WIFI_CONNECTED:
            if(!ZG_IS_CONNECTED())
            {
#ifdef CT80
                cmd_WriteUserMsgArea(1, "WIFI_DISCONNECTED");
#endif
                ConnectState=WIFI_DISCONNECTED;
            }
            if(ZG_GET_MODE() == kZGLMNetworkModeAdhoc)  // Not going to ever have a CB Connection in AdHoc mode, so just bug out....
            {
                return;
            }
            break;
        case WIFI_DISCONNECTED:
#ifndef R4
//cbdm      holdabs_counter.val=0;  // If you lose link, shut down Internet control immediately
            cbAttrSetById16(kCBAtt_holdEnabled, 0);
#endif

//cbdm      thermoLock.val=0;   // If you lose link, shut down Internet control immediately 
//cbdm      cbAttrSetById8(kCBAtt_thermoLock, 0);
#if defined (CT80)
            cmd_WriteUserMsgArea(1, "Not Setup or Lost AP");
#endif

//      cmd_DisRadioUtilityLock(); // If you lose link, shut down Internet control immediately 
            //cmd_WriteUserMsgArea(1, ipbufptr);
            //cmd_WriteUserMsgArea(1, "Or Lost AP Link - Reset AP");
//not used      cmd_WriteRadioStatus(0x00, (char)rssi.val);
            // cmdxxxx Take down radio link on TSTAT
            active_flag=1;   // Go through and get initial props again.
            lostlink_flag=1; // Write blanks to user area in screen once back online
            ZG_Con_Timer=TimeMonitorTickGet();
            ConnectState=SPI_WORKING;
            break;
    }   


    // INSERT MAIN APPLICATION CODE HERE.
    // Get and update the time from the DR, or CB server or if activated, Time Pool Network
    if((TimeMonitorTickGet() - MainSysTickTimer) > TIME_MONITOR_TICK_SECOND*5)
    {
        MainSysTickTimer=TimeMonitorTickGet();
        //MainSystemUNIXTime=BerkeleySNTPGetUTCSeconds()+14400;
        sprintf(timestring,"\rTime:%ld", MainSystemUNIXTime);
        UARTiDebug(timestring, DEBUG_ORIG);
        MainSystemUNIXTime=MainSystemUNIXTime+5;
        if(MainSystemUNIXTime > 1280000000)  // Just make sure it is right after initialization.
        {
            timeSynch=1; // Valid Time.  Wait for Timesynch or Grab from Time pool.	
        }
    }

//refactor tstat   //  EVENT MANAGER
//refactor tstat   // Check CommandQ to execute load control events.  Only process if timeSynch is valid and not currently in process of sending to server.
//refactor tstat   if(timeSynch)
//refactor tstat   {
//refactor tstat     for(i=0;i<EVENTQSize;i++)
//refactor tstat     {
//refactor tstat         //If event is past due, delete
//refactor tstat       if((eventQ[i].endtime<=MainSystemUNIXTime) && (eventQ[i].magicAddress==MAGIC_NUMBER_EVENT_Q_INFO))
//refactor tstat       {
//refactor tstat         eventInProgress=0; // Event no longer in progress
//refactor tstat         // delete message from Q
//refactor tstat         DeleteEvent(i);
//refactor tstat 
//refactor tstat //rwb g2hdemo
//refactor tstat         cmd_EnableLCDBacklight();
//refactor tstat         ThermoHoldAbs("0");
//refactor tstat 
//refactor tstat //g2hdemo        // Reset the heartbeat state machine so the serial interrupts don't conflict.
//refactor tstat //g2hdemo        // On any load shed, set the default to 
//refactor tstat //g2hdemo        cmd_DisRadioUtilityLock(); 
//refactor tstat //g2hdemo        cmd_RadioTempTargetSetback(CMD_WRITE, 0);
//refactor tstat //g2hdemo        cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat //g2hdemo        WriteEventQ();
//refactor tstat //g2hdemo
//refactor tstat //g2hdemo        // Send setback prop up to the CB server HERE
//refactor tstat //g2hdemo
//refactor tstat //g2hdemo        sprintf(timestring,"EndTime: %ld Done", eventQ[i].endtime);
//refactor tstat //g2hdemo        UARTiDebug(timestring);
//refactor tstat //g2hdemo//cbdm        setback.val=0;
//refactor tstat //g2hdemo        UARTiDebug("Setback == 0");
//refactor tstat //g2hdemo#ifdef CT80
//refactor tstat //g2hdemo        ThermoLine1MsgAreaQ("delete","",0, 1);
//refactor tstat //g2hdemo#endif
//refactor tstat //g2hdemo//cbdm        setback.status=0x01;
//refactor tstat //g2hdemo//cbdm        setback.crud='u';
//refactor tstat //g2hdemo//cbdm        setback.seqId=seq++;
//refactor tstat //g2hdemo        cbAttrSetByIdForceSend8(kCBAtt_setback, 0);
//refactor tstat 
//refactor tstat         // Send setback prop up to the CB server HERE
//refactor tstat       }
//refactor tstat       //If load control event sent over DR PORTAL and is ready to execute, EXECUTE IT.
//refactor tstat       if(eventQ[i].exetime<=MainSystemUNIXTime && (!strcmp((char *)eventQ[i].cmdType,"setback")) && (eventQ[i].executed==0))
//refactor tstat       {
//refactor tstat         eventInProgress=1;
//refactor tstat 
//refactor tstat //g2hdemo//cbdm        setback.val=eventQ[i].setback;
//refactor tstat //g2hdemo        cbAttrSetById8(kCBAtt_setback, eventQ[i].setback);
//refactor tstat //g2hdemo
//refactor tstat //g2hdemo        // On any load event done, set the setback to 0		
//refactor tstat //g2hdemo//cbdm        cmd_RadioTempTargetSetback(CMD_WRITE, (setback.val*2));  // (char)eventQ[i].setback
//refactor tstat //g2hdemo        cmd_RadioTempTargetSetback(CMD_WRITE, (cbGetVal8(kCBAtt_setback)*2));  // (char)eventQ[i].setback
//refactor tstat //g2hdemo        cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat //g2hdemo        sprintf(timestring,"Demand Event! Setback=%ddeg", eventQ[i].setback);
//refactor tstat //g2hdemo#ifdef CT80
//refactor tstat //g2hdemo        ThermoLine1MsgAreaQ("add", timestring, 10, 1);
//refactor tstat //g2hdemo#endif
//refactor tstat //g2hdemo
//refactor tstat 
//refactor tstat         // Tell firmware this is executed, but do not save this change to the flash, so if there is a power fail, this 
//refactor tstat         // happens again (whatever the event is...)				
//refactor tstat         eventQ[i].executed=1;
//refactor tstat         SendLocalHB=1;
//refactor tstat 
//refactor tstat         // Send setback prop up to the CB server HERE
//refactor tstat       }
//refactor tstat       //If load control event sent over CLOUDBUS and is ready to execute, EXECUTE IT.
//refactor tstat       if((eventQ[i].exetime<=MainSystemUNIXTime) && (eventQ[i].optState=='i') && (eventQ[i].executed==0) && (eventQ[i].magicAddress==MAGIC_NUMBER_EVENT_Q_INFO))
//refactor tstat       {
//refactor tstat         eventInProgress=1;
//refactor tstat         eventQ[i].executed=1;
//refactor tstat         eventQ[i].executedo=0;
//refactor tstat //cbdm        setback.val=eventQ[i].setback;
//refactor tstat         cbAttrSetByIdForceSend8(kCBAtt_setback, eventQ[i].setback);
//refactor tstat         // On any load event done, set the setback to 0		
//refactor tstat //cbdm        cmd_RadioTempTargetSetback(CMD_WRITE, (setback.val*2));  // (char)eventQ[i].setback
//refactor tstat         cmd_RadioTempTargetSetback(CMD_WRITE, (cbGetVal8(kCBAtt_setback)*2));  // (char)eventQ[i].setback
//refactor tstat         cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat         sprintf(timestring,"Demand Event! Setback=%ddeg", eventQ[i].setback);
//refactor tstat         UARTiDebug(timestring, DEBUG_ORIG);
//refactor tstat #ifdef CT80
//refactor tstat         ThermoLine1MsgAreaQ("add", timestring, 10, 1);
//refactor tstat #endif
//refactor tstat         SendLocalHB=1;
//refactor tstat         // Send setback prop up to the CB server HERE
//refactor tstat //cbdm        setback.status=0x01;
//refactor tstat //cbdm        setback.crud='u';
//refactor tstat //cbdm        setback.seqId=seq++;
//refactor tstat 
//refactor tstat       }
//refactor tstat       if((eventQ[i].optState=='d') && (eventQ[i].magicAddress==MAGIC_NUMBER_EVENT_Q_INFO))
//refactor tstat       {
//refactor tstat //cbdm        setback.val=0;
//refactor tstat         cbAttrSetByIdForceSend8(kCBAtt_setback, 0);
//refactor tstat         cmd_RadioTempTargetSetback(CMD_WRITE, 0);  // (char)eventQ[i].setback
//refactor tstat         cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat         sprintf(timestring,"Delete Event!");
//refactor tstat         UARTiDebug(timestring, DEBUG_ORIG);
//refactor tstat #ifdef CT80
//refactor tstat         ThermoLine1MsgAreaQ("add", timestring, 10, 1);
//refactor tstat #endif
//refactor tstat         SendLocalHB=1;
//refactor tstat         // Send setback prop up to the CB server HERE
//refactor tstat //cbdm        setback.status=0x01;
//refactor tstat //cbdm        setback.crud='u';
//refactor tstat //cbdm        setback.seqId=seq++;
//refactor tstat         DeleteEvent(i);
//refactor tstat         WriteEventQ();
//refactor tstat       }
//refactor tstat       //If optout event sent over DEMAND, CLOUDBUS and is ready to execute		
//refactor tstat       if((eventQ[i].optState=='o') && (eventQ[i].magicAddress==MAGIC_NUMBER_EVENT_Q_INFO) && (eventQ[i].executedo==0))
//refactor tstat       {
//refactor tstat //cbdm        setback.val=eventQ[i].osetback;
//refactor tstat //cbdm        setback.status=0x01;
//refactor tstat //cbdm        setback.crud='u';
//refactor tstat //cbdm        setback.seqId=seq++;
//refactor tstat         cbAttrSetByIdForceSend8(kCBAtt_setback, eventQ[i].setback);
//refactor tstat         eventQ[i].executedo=1;
//refactor tstat         eventQ[i].executed=0;
//refactor tstat         cmd_RadioTempTargetSetback(CMD_WRITE, (eventQ[i].osetback*2));  // (char)eventQ[i].setback
//refactor tstat         cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat         sprintf(timestring,"Demand Evt! Setback=%ddeg", eventQ[i].osetback);
//refactor tstat         UARTiDebug(timestring, DEBUG_ORIG);
//refactor tstat #ifdef CT80
//refactor tstat         ThermoLine1MsgAreaQ("add", timestring, 10, 1);
//refactor tstat #endif
//refactor tstat       }
//refactor tstat       if(!strcmp((char *)eventQ[i].cmdType,"optout"))
//refactor tstat       {
//refactor tstat         // If this is a CloudBUS event process seperately from a DR event.
//refactor tstat         if(eventQ[i].optState)
//refactor tstat         {
//refactor tstat           // If Event is already in progress and not a DRU, cancel demand event real time on the device.
//refactor tstat           if(eventQ[i].exetime<=MainSystemUNIXTime)
//refactor tstat           {
//refactor tstat             // On any load shed, set the default to 
//refactor tstat             cmd_RadioTempTargetSetback(CMD_WRITE, 0);
//refactor tstat             cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat             // Send up to the Arrayent server.
//refactor tstat             sprintf(timestring,"EndTime: %ld Done", eventQ[i].endtime);
//refactor tstat             UARTiDebug(timestring, DEBUG_ORIG);
//refactor tstat //cbdm            setback.val=0;
//refactor tstat             cbAttrSetByIdForceSend8(kCBAtt_setback, 0);
//refactor tstat             UARTiDebug("Setback == 0", DEBUG_ORIG);
//refactor tstat             UARTiDebug("OptOut - Cancel event", DEBUG_ORIG);
//refactor tstat //cbdm            setback.status=0x01;
//refactor tstat //cbdm            setback.crud='u';
//refactor tstat //cbdm            setback.seqId=seq++;
//refactor tstat             // Send setback prop up to the CB server HERE
//refactor tstat #ifdef CT80
//refactor tstat             ThermoLine1MsgAreaQ("delete", "", 0, 1);
//refactor tstat #endif
//refactor tstat           }
//refactor tstat           // Whether started or not, delete both events from the Q to prevent any further running.
//refactor tstat           DeleteEvent(i);
//refactor tstat           DeleteEvent(j);
//refactor tstat           UARTiDebug("Delete Opt Out from Onboard Event Q", DEBUG_ORIG);
//refactor tstat           SwitchToLongHeartBeat(1);
//refactor tstat           WriteEventQ();
//refactor tstat           break;
//refactor tstat         }
//refactor tstat         // Check to see if any other events with the eventId are in the eventQ 
//refactor tstat         // that are ready to be run, but have not executed yet.
//refactor tstat         // (i.e. load events ready to run that were sent over ip.....)
//refactor tstat         // Match up the optout eventId with the staged or running eventId
//refactor tstat         else
//refactor tstat         {
//refactor tstat           for(j=0;j<8;j++)
//refactor tstat           {
//refactor tstat             //If a event iD is matched in the Q, that must mean a loadshed event was previous sent over ip and is in the Q.
//refactor tstat             //You must find this and delete it.  
//refactor tstat             if(eventQ[i].eventId==eventQ[j].eventId && (j != i) && eventQ[i].eventId!=0 && eventQ[j].eventId!=0)
//refactor tstat             {
//refactor tstat               // If Event is already in progress and not a DRU, cancel demand event real time on the device.
//refactor tstat               if(eventQ[i].exetime<=MainSystemUNIXTime)
//refactor tstat               {
//refactor tstat                 // On any load shed, set the default to 
//refactor tstat                 cmd_RadioTempTargetSetback(CMD_WRITE, 0);
//refactor tstat                 cmd_ReadCurTempTargets(); // This sends latest information back up to "targets" prop which is read by serverUARTiDebug("Turn OFF Relay");
//refactor tstat                 // Send up to the Arrayent server.
//refactor tstat                 sprintf(timestring,"EndTime: %ld Done", eventQ[i].endtime);
//refactor tstat                 UARTiDebug(timestring, DEBUG_ORIG);
//refactor tstat //cbdm                setback.val=0;
//refactor tstat                 cbAttrSetById8(kCBAtt_setback, 0);
//refactor tstat                 UARTiDebug("Setback == 0", DEBUG_ORIG);
//refactor tstat                 UARTiDebug("OptOut - Cancel event", DEBUG_ORIG);
//refactor tstat 
//refactor tstat                 // Send setback prop up to the CB server HERE
//refactor tstat #ifdef CT80	
//refactor tstat                 ThermoLine1MsgAreaQ("delete", "", 0, 1);
//refactor tstat #endif
//refactor tstat               }
//refactor tstat               // Whether started or not, delete both events from the Q to prevent any further running.
//refactor tstat               DeleteEvent(i);
//refactor tstat               DeleteEvent(j);
//refactor tstat               UARTiDebug("Delete Opt Out from Onboard Event Q", DEBUG_ORIG);
//refactor tstat               SwitchToLongHeartBeat(1);
//refactor tstat               WriteEventQ();
//refactor tstat               break;
//refactor tstat             }
//refactor tstat           }
//refactor tstat         }
//refactor tstat       }
//refactor tstat       else
//refactor tstat       {
//refactor tstat         // do nothing
//refactor tstat       }
//refactor tstat     }
//refactor tstat   }
//refactor tstat   // This kicks the heartbeat timer as long as it is the windows of T-OPTOUT_HEARTBEAT_WINDOW to endtime+TIME_PROPGATION_DELAY
//refactor tstat   for(i=0;i<EVENTQSize;i++)
//refactor tstat   {
//refactor tstat     // If a loadshed command is about to kick off soon, or the DRU WINDOW has started.
//refactor tstat     // make sure you try and grab opt-outs ahead of it kicking off so you can remove them from the Q
//refactor tstat     if((((eventQ[i].exetime-OPTOUT_HEARTBEAT_WINDOW)<=MainSystemUNIXTime) || (((eventQ[i].endtime+TIME_PROPAGATION_OFFSET)<=MainSystemUNIXTime))) && (eventQ[i].exetime!=0))
//refactor tstat     {
//refactor tstat       SwitchToLongHeartBeat(1);
//refactor tstat       heartBeatTimer=SHORT_HEARTBEAT_TIMER;
//refactor tstat     }
//refactor tstat   }

    SwitchToLongHeartBeat(0);  // Executes this countdown everytime through loop.
    if(!AppConfig.Flags.bInConfigMode && ZG_QUERY_IS_CONNECTED() && (ZG_GET_MODE() == kZGLMNetworkModeInfrastructure))
    {
        // If return 0 then not busy. So go ahead and switch to UPGRADE.
//#ifdef ENABLE_SEP
#ifdef DFW
        if(sep_startup_counter >= 2)
        {
            DFWRun();
        }
#else
        if(EnableSEP())
        {
            if(sep_startup_counter >= 2)
            {
                if(!EventTCPClient())
                {
                    TCPCommState = UPGRADE;
                }
            }
            else
            {
                //need to investigate if this can go here          TCPCommState = UPGRADE;
            }
        }
#endif
    }

//daves sep above  SwitchToLongHeartBeat(0);  // Executes this countdown everytime through loop.
//daves sep above  if(!AppConfig.Flags.bInConfigMode && (ZG_GET_MODE() == kZGLMNetworkModeInfrastructure))
//daves sep above  {
//daves sep above    switch(TCPCommState)
//daves sep above    {
//daves sep above      case UPGRADE:
//daves sep above        if(!upgradeFlag.status)
//daves sep above        {
//daves sep above          TCPCommState=UPGRADE; // DR_SERVER
//daves sep above          break;
//daves sep above        }

//daves sep above        if(UpgradeTCPClient())
//daves sep above        {
//daves sep above          TCPCommState=UPGRADE;  // DR_SERVER
//daves sep above        }
//daves sep above        if(UpgradeTimeout>1)  // If this times out, jump to next state.
//daves sep above        {
//daves sep above          TCPCommState=UPGRADE;  // DR_SERVER
//daves sep above          UpgradeTimeout=0;
//daves sep above        }
//daves sep above        break;
//daves sep above      case DR_SERVER:
//daves sep above        // If retrying to talk with the DR server and not getting it, punt...
//daves sep above        if(InvalidSocketReboot % 4)
//daves sep above        {
//daves sep above          TCPCommState=UPGRADE;
//daves sep above          break;
//daves sep above        }
//daves sep above        // Run Heartbeat attempt every 30 seconds.
//daves sep above        if(((TimeMonitorTickGet() - SendToServerTimeout) > TIME_MONITOR_TICK_SECOND*heartBeatTimer))
//daves sep above        {
//daves sep above          SendToServerTimeout=TimeMonitorTickGet(); 
//daves sep above          SendToServer=1;
//daves sep above        }
//daves sep above        // If main state machine is ready, process.  Would like to put in main state machine but not sure if compiler likes a switch within a switch.
//daves sep above        if(SendToServer || TryGetNextEvent)
//daves sep above        {
//daves sep above          TCP_Message=SEND_HB;
//daves sep above          if(((TimeMonitorTickGet() - SendTCPMsg_Timer) > TIME_MONITOR_TICK_SECOND*3))
//daves sep above          {
//daves sep above            EventTCPClient(TCP_Message);  // This is the main TCP thread that needs to run. UARTMsg_Rdy 2Process Deasserted once this function is complete.
//daves sep above          }
//daves sep above        }
//daves sep above        if(!SendToServer && !TryGetNextEvent )
//daves sep above        {
//daves sep above          TCPCommState=UPGRADE;
//daves sep above        }
//daves sep above        break;
//daves sep above        /* case PROCESS_HOMEAREA:
//daves sep above            ProcessHomeAreaNet();
//daves sep above        break; */
//daves sep above    }   
//daves sep above  }

#endif  // #ifndef THERMOSTAT_COMMUNICATION_TEST

    // Check thermostat status.
#if defined (CT80)
    ThermoLine1MsgAreaQ("display", "", 0, 0);
    // ThermoLine2MsgAreaQ("display", 0, 0);
#endif
#ifndef THERMOSTAT_COMMUNICATION_TEST
    ThermoCheckStatus();
#endif
    ProcessThermCmds();
    if(!AppConfig.Flags.bInConfigMode && (ZG_GET_MODE() == kZGLMNetworkModeInfrastructure))
    {
        ProcessThermRsps();
//gcm    ProcessHomeAreaNet();
    }

#ifndef THERMOSTAT_COMMUNICATION_TEST
  // Check for activity log items to send
  if(cbIsAttributeIdle(kCBAtt_actlog))
  {
      cbAttrSetByIdAutoSendBuf(kCBAtt_actlog, (char *)LOG_GetNextEntry());
  }

    cur_time = TimeMonitorTickGet();

    if((cur_time - schedule_process_time) > (TIME_MONITOR_TICK_SECOND*POLL_TIME_SCHEDULE_POLL))
    {
        schedule_process_time = cur_time;
        ScheduleProcess(cbGetVal8(kCBAtt_setMode));         // this will get the schedule
    }
/*     else                                                                                                            */
/*     {                                                                                                               */
/*         if(ScheduleSetByCloudbus())                                                                                 */
/*         {                                                                                                           */
/*             schedule_just_set_time = cur_time;                                                                      */
/*         }                                                                                                           */
/*         else if(schedule_just_set_time != 0)                                                                        */
/*         {                                                                                                           */
/*             if((cur_time - schedule_just_set_time) > (TIME_MONITOR_TICK_SECOND*60)) // measured at about 35 seconds */
/*             {                                                                                                       */
/*                 // short circuiting schedule read, reset long loop and short loop                                   */
/*                 schedule_just_set_time = 0;                                                                         */
/*                 schedule_process_time = cur_time;                                                                   */
/*                 ScheduleProcess(cbGetVal8(kCBAtt_setMode));         // this will get the schedule                   */
/*             }                                                                                                       */
/*         }                                                                                                           */
/*     }                                                                                                               */
#endif

/*   static int read_curr_temp_timer = 0; */
/*   int cur_time = 0;                    */
/*   cur_time = TimeMonitorTickGet();                                      */
/*   if(read_curr_temp_timer == 0)                              */
/*   {                                                          */
/*       read_curr_temp_timer = cur_time;                       */
/*   }                                                          */
/*                                                              */
/*   if((cur_time - read_curr_temp_timer) > (TIME_MONITOR_TICK_SECOND * 10)) */
/*   {                                                          */
/*       cmd_ReadCurTemp();         // Check temperature.      */
/*       read_curr_temp_timer = 0;                              */
/*   }                                                          */

/* #define PRICING_MESSAGE_SIZE 20                              */
/* void ProcessPricingMessage()                                 */
/* {                                                            */
/*     char buf[PRICING_MESSAGE_SIZE];                          */
/*     unsigned long poweri=0, cenergy=0;                       */
/*                                                              */
/* #ifndef INTWINE_PCT_TEST_PRICING_AREA                        */
/*     return; // until ready for this                          */
/* #endif                                                       */
/*     // Steps to processing pricing message                   */
/*     // 1) Get the previous pricing message                   */
/*     // 2) Update CloudBus values                             */
/*     // 3) Perform a read for the next message                */
/*                                                              */
/*     // 1)                                                    */
/*     ThermGetPricingMessageBuffer(buf, PRICING_MESSAGE_SIZE); */
/*                                                              */
/*     // 2)                                                    */
/*     poweri = 45;                                             */
/*     cenergy = 12044;                                         */
/*                                                              */
/*     //                                                       */
/*     cmd_ReadPricingMsgArea();                                */
/*                                                              */
/* }                                                            */

    // TStat test the communication engine
#ifdef THERMOSTAT_COMMUNICATION_TEST
    // Timing of this test
    // 1) Wait 10 seconds 

    if(comm_test_loop_completed)
    {
        TimeMonitorRegisterI(1, 1000, test_start_comm_timeout);
        comm_test_loop_completed = 0;
    }
    
    if(start_comm_timeout_expired)
    {
        // after 10 seconds (startup stuff above that we don't care about should be done)
        ProcessThermCommandLoop(0);
        start_comm_timeout_expired = 0;
    }
#endif
}

#ifdef THERMOSTAT_COMMUNICATION_TEST
void test_start_comm_timeout(void)
{
    start_comm_timeout_expired = 1;
}

extern unsigned long comm_rsp_target;
extern int comm_rsp_nsetHumidity;

void ProcessThermCommandLoop()
{
    static int loop_count = 0;
    static int data_setting_loop_count = 0;
    static unsigned char verify_therm_mode=0; 
    static int verify_temperature=0; 
    static int verify_fan_mode=0; 
    static int verify_hold_mode=0; 
    static int verify_day = 0;
    static int verify_time = 0;
    static int verify_humidity_set_point = 20;
    char buf[50];

    static int enable_remote_control_once = 1;

    if(enable_remote_control_once)
    {
        enable_remote_control_once = 0;
        cmd_RemoteControl(0x02);           // Enable remote control, once
    }

// Scenario 2 testing:
/*     cmd_ReadCurTemp();         // Check temperature. */
/*     cmd_ReadCurHumidity();                           */
/*     cmd_ReadCurTemp();         // Check temperature. */
/*     cmd_ReadCurHumidity();                           */
/*     cmd_ReadCurTemp();         // Check temperature. */
/*     cmd_ReadCurHumidity();                           */
/*     comm_test_loop_completed = 1;                    */
/*     return;                                          */

    // beep once per loop
// Scenario 4 testing:
/*     cmd_RadioMsgAnn(1);           */
/*     cmd_RadioMsgAnn(0);           */
/*     cmd_RadioMsgAnn(0);           */
/*     cmd_RadioMsgAnn(0);           */
/*     cmd_RadioMsgAnn(0);           */
/*     cmd_RadioMsgAnn(0);           */
/*     comm_test_loop_completed = 1; */
/*     return;                       */


// Original/just a lot of messages test:

    cmd_RadioMsgAnn(0);
    cmd_RadioMsgAnn(0);

    //case 0:
    cmd_EnableLCDBacklight();

    //case 1:
    cmd_ReadCurTemp();         // Check temperature.
    cmd_ReadCurTemp();         // Check temperature.

    //case 2: // get humidity
    cmd_ReadCurHumidity();

    verify_humidity_set_point++;
    if(verify_humidity_set_point > 90)
    {
        verify_humidity_set_point = 20;
    }
    cmd_HumiditySetpoint(CMD_WRITE, verify_humidity_set_point);

    //case 3:
    if(data_setting_loop_count < 30)
    {
        verify_therm_mode = 1;
    }
    else
    {
        verify_therm_mode = 2;
    }
    cmd_ThermOpMode(CMD_WRITE, verify_therm_mode);

//auto-verify        //case 4: // get tstat mode
//auto-verify        cmd_ThermOpMode(CMD_READ,0);

    //case 5:
    if(data_setting_loop_count < 30)
    {
        verify_temperature = 60 + data_setting_loop_count;
    }
    else
    {
        verify_temperature = 30 + data_setting_loop_count;
    }
    cmd_RadioTempTargetSetpoints(CMD_WRITE, verify_temperature*2, verify_temperature*2);

//auto-verify        //case 6: // get temp targets
//auto-verify        cmd_ReadCurTempTargets();

    //case 7:
    if(data_setting_loop_count < 30)
    {
        verify_fan_mode = 1;
    }
    else
    {
        verify_fan_mode = 2;
    }
    cmd_FanOpMode(CMD_WRITE, verify_fan_mode);

//auto-verify        //case 8: // get fan mode 
//auto-verify        cmd_FanOpMode(CMD_READ,0);

    //case 9:
    if((data_setting_loop_count % 2) == 0)
    {
        verify_hold_mode = 0;
        cmd_DisTempProgHold();
    }
    else
    {
        verify_hold_mode = 1;
        cmd_EnableTempProgHold();
    }

//auto-verify        //case 10: // get hold mode
//auto-verify        cmd_ReadThermModeCfg();

    //case 11: // void cmd_CurDay(char bReadWrite, char CurDay) // 0x0641
    verify_day++;
    if(verify_day > 6)
    {
        verify_day = 0;
    }
    cmd_CurDay(CMD_WRITE, verify_day);

//auto-verify        //case 12:
//auto-verify        cmd_CurDay(CMD_READ, 0);

    //case 13: // void cmd_CurTime(char bReadWrite, char MostSigTimeByte, char LeastSigTimeByte) // 0x0640
    verify_time++;
    if(verify_time > 0x059F)
    {
        verify_time = 0;
    }
    cmd_CurTime(CMD_WRITE, verify_time>>8, verify_time&0xff);

//auto-verify        //case 14:
//auto-verify        cmd_CurTime(CMD_READ, 0, 0);

    //case 15:

    //case 17: // void cmd_HumiditySetpoint(char bReadWrite, char HumSetPoint) // 0x2101
//auto-verify        //case 18:
//auto-verify        cmd_HumiditySetpoint(CMD_READ, 0);

    // restart data items (temp/mode/fan/hold)
    data_setting_loop_count++;
    if(data_setting_loop_count > 59)
    {
        data_setting_loop_count = 0;
    }

    cmd_UserMsgArea(1); // enable

    if((data_setting_loop_count % 2) == 0)
    {
        sprintf(buf, "#------ Loop: %06d ----#", ++loop_count);
        cmd_WriteUserMsgArea(0, buf);
    }
    else
    {
        sprintf(buf, "$----- Index: %02d --------$", data_setting_loop_count);
        cmd_WriteUserMsgArea(1, buf);
    }

    //case 16:
    cmd_RadioTempOffset(); // Indicate present power usage of avg. of 375mW. Hardcoded Hex values in function.

    cmd_GetUnsolicitedMsg();

}
#endif

//original tstat comm engine   extern unsigned long comm_rsp_target;
//original tstat comm engine   extern int comm_rsp_nsetHumidity;
//original tstat comm engine   
//original tstat comm engine   void ProcessThermCommandLoop()
//original tstat comm engine   {
//original tstat comm engine       unsigned int cur_time=0, elapsed_time=0, response_received=0;
//original tstat comm engine       static unsigned int command_sent_time=0;
//original tstat comm engine       static unsigned int last_message_was_timeout=0;
//original tstat comm engine       static int waiting_on_response=0;
//original tstat comm engine       static int error_time_read_lower_count=0;   // count number of sequential times count is lower
//original tstat comm engine   
//original tstat comm engine       static unsigned char verify_therm_mode=0; 
//original tstat comm engine       static int verify_temperature=0; 
//original tstat comm engine       static int verify_fan_mode=0; 
//original tstat comm engine       static int verify_hold_mode=0; 
//original tstat comm engine       static int verify_day = 0;
//original tstat comm engine       static int verify_time = 0;
//original tstat comm engine       static int verify_humidity_set_point = 20;
//original tstat comm engine   
//original tstat comm engine       static int data_setting_loop_count = 0;
//original tstat comm engine       static int message_index = 11;
//original tstat comm engine       static int message_timeouts = 0;
//original tstat comm engine   
//original tstat comm engine       unsigned char rb8;
//original tstat comm engine       int rb16;
//original tstat comm engine       unsigned long rb32;
//original tstat comm engine       char buf[100];
//original tstat comm engine   
//original tstat comm engine       cur_time = TimeMonitorTimeMonitorTickGet();
//original tstat comm engine       if((waiting_on_response == 0) && (iProcessThermCmdsInUse == MESSAGE_ENGINE_IDLE))
//original tstat comm engine   
//original tstat comm engine       {
//original tstat comm engine           tmm[message_index].message_count++;
//original tstat comm engine           if((tmm[message_index].message_count % 5000) == 0)
//original tstat comm engine           {
//original tstat comm engine               waiting_on_response = 0; //bp
//original tstat comm engine           }
//original tstat comm engine   
//original tstat comm engine           switch(message_index)
//original tstat comm engine           {
//original tstat comm engine               case 0:
//original tstat comm engine                   cmd_EnableLCDBacklight();
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   //waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 1:
//original tstat comm engine                   cmd_ReadCurTemp();         // Check temperature.
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;                waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 2: // get humidity
//original tstat comm engine                   cmd_ReadCurHumidity();
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 3:
//original tstat comm engine                   // void cmd_ThermOpMode(char bReadWrite, char OpMode) // 0x5008  // 0=off, 1=heat, 2=cool
//original tstat comm engine                   // 0 to 29 - heat
//original tstat comm engine                   // 30 to 59 - cool
//original tstat comm engine                   if(data_setting_loop_count < 30)
//original tstat comm engine                   {
//original tstat comm engine                       verify_therm_mode = 1;
//original tstat comm engine                   }
//original tstat comm engine                   else
//original tstat comm engine                   {
//original tstat comm engine                       verify_therm_mode = 2;
//original tstat comm engine                   }
//original tstat comm engine                   cmd_ThermOpMode(CMD_WRITE, verify_therm_mode);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine   //                waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 4: // get tstat mode
//original tstat comm engine                   cmd_ThermOpMode(CMD_READ,0);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 5:
//original tstat comm engine                   // void cmd_RadioTempTargetSetpoints(char bReadWrite, char CoolTemp, char HeatTemp) // 0x2020 // degrees faren times two.
//original tstat comm engine                   if(data_setting_loop_count < 30)
//original tstat comm engine                   {
//original tstat comm engine                       verify_temperature = 60 + data_setting_loop_count;
//original tstat comm engine                   }
//original tstat comm engine                   else
//original tstat comm engine                   {
//original tstat comm engine                       verify_temperature = 30 + data_setting_loop_count;
//original tstat comm engine                   }
//original tstat comm engine                   cmd_RadioTempTargetSetpoints(CMD_WRITE, verify_temperature*2, verify_temperature*2);
//original tstat comm engine   
//original tstat comm engine                   verify_temperature *= 0x0101;
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine   //                waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 6: // get temp targets
//original tstat comm engine                   cmd_ReadCurTempTargets();
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 7:
//original tstat comm engine                   // void cmd_FanOpMode(char bReadWrite, char OpMode) // 0x5009 // 0=auto, 1=auto/circulate, 2=on
//original tstat comm engine                   if(data_setting_loop_count < 30)
//original tstat comm engine                   {
//original tstat comm engine                       verify_fan_mode = 1;
//original tstat comm engine                   }
//original tstat comm engine                   else
//original tstat comm engine                   {
//original tstat comm engine                       verify_fan_mode = 2;
//original tstat comm engine                   }
//original tstat comm engine                   cmd_FanOpMode(CMD_WRITE, verify_fan_mode);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine   //                waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 8: // get fan mode 
//original tstat comm engine                   cmd_FanOpMode(CMD_READ,0);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 9:
//original tstat comm engine                   // void cmd_DisTempProgHold() // 0x0A63
//original tstat comm engine                   // void cmd_EnableTempProgHold() // 0x0A64
//original tstat comm engine                   if((data_setting_loop_count % 2) == 0)
//original tstat comm engine                   {
//original tstat comm engine                       verify_hold_mode = 0;
//original tstat comm engine                       cmd_DisTempProgHold();
//original tstat comm engine                   }
//original tstat comm engine                   else
//original tstat comm engine                   {
//original tstat comm engine                       verify_hold_mode = 1;
//original tstat comm engine                       cmd_EnableTempProgHold();
//original tstat comm engine                   }
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   //waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 10: // get hold mode
//original tstat comm engine                   cmd_ReadThermModeCfg();
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 11: // void cmd_CurDay(char bReadWrite, char CurDay) // 0x0641
//original tstat comm engine                   verify_day++;
//original tstat comm engine                   if(verify_day > 6)
//original tstat comm engine                   {
//original tstat comm engine                       verify_day = 0;
//original tstat comm engine                   }
//original tstat comm engine                   cmd_CurDay(CMD_WRITE, verify_day);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 12:
//original tstat comm engine                   cmd_CurDay(CMD_READ, 0);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 13: // void cmd_CurTime(char bReadWrite, char MostSigTimeByte, char LeastSigTimeByte) // 0x0640
//original tstat comm engine                   verify_time++;
//original tstat comm engine                   if(verify_time > 0x059F)
//original tstat comm engine                   {
//original tstat comm engine                       verify_time = 0;
//original tstat comm engine                   }
//original tstat comm engine                   cmd_CurTime(CMD_WRITE, verify_time>>8, verify_time&0xff);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 14:
//original tstat comm engine                   cmd_CurTime(CMD_READ, 0, 0);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 15:
//original tstat comm engine                   cmd_RadioMsgAnn(1);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 16:
//original tstat comm engine                   cmd_RadioTempOffset(); // Indicate present power usage of avg. of 375mW. Hardcoded Hex values in function.
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 17: // void cmd_HumiditySetpoint(char bReadWrite, char HumSetPoint) // 0x2101
//original tstat comm engine                   verify_humidity_set_point++;
//original tstat comm engine                   if(verify_humidity_set_point > 90)
//original tstat comm engine                   {
//original tstat comm engine                       verify_humidity_set_point = 20;
//original tstat comm engine                   }
//original tstat comm engine                   cmd_HumiditySetpoint(CMD_WRITE, verify_humidity_set_point);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_NO_RSP_WAITING;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine               case 18:
//original tstat comm engine                   cmd_HumiditySetpoint(CMD_READ, 0);
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_CMD_WITH_RSP_WAITING;
//original tstat comm engine                   waiting_on_response = 1;
//original tstat comm engine                   break;
//original tstat comm engine   
//original tstat comm engine           }
//original tstat comm engine           command_sent_time = cur_time;
//original tstat comm engine   
//original tstat comm engine           if(waiting_on_response == 0)
//original tstat comm engine           {
//original tstat comm engine               message_index++;
//original tstat comm engine               if(message_index >= TSTAT_MESSAGE_METRICS_COUNT)
//original tstat comm engine               {
//original tstat comm engine                   message_index = 0;
//original tstat comm engine               }
//original tstat comm engine           }
//original tstat comm engine   
//original tstat comm engine       }
//original tstat comm engine       else
//original tstat comm engine       {
//original tstat comm engine           if(cur_time > command_sent_time)
//original tstat comm engine           {
//original tstat comm engine               elapsed_time = cur_time - command_sent_time;
//original tstat comm engine   
//original tstat comm engine               switch(message_index)
//original tstat comm engine               {
//original tstat comm engine                   case 0: // enable backlight
//original tstat comm engine                       break;
//original tstat comm engine                   case 1: // get temp
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_CUR_TEMP);
//original tstat comm engine                       break;
//original tstat comm engine                   case 2: // get humidity
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_CUR_HUMIDITY);
//original tstat comm engine                       break;
//original tstat comm engine                   case 3: // set tstat mode
//original tstat comm engine                       //response_received = ThermCheckLastRsp(MSG_RSP_THERM_OP_MODE);
//original tstat comm engine                       break;
//original tstat comm engine                   case 4: // get tstat mode
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_THERM_OP_MODE);
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           rb8 = cbGetVal8(kCBAtt_setMode);
//original tstat comm engine                           if(verify_therm_mode == rb8)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_THERM_OP_MODE\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine                   case 5: // set temp targets
//original tstat comm engine                       //response_received = ThermCheckLastRsp(MSG_RSP_TEMP_TARGETS);
//original tstat comm engine                       break;
//original tstat comm engine                   case 6: // get temp targets
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_TARGETS);
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           rb32 = cbGetVal32(kCBAtt_tempTargets);
//original tstat comm engine                           if(verify_temperature == rb32)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_TARGETS\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine                   case 7: // set fan mode 
//original tstat comm engine                       //response_received = ThermCheckLastRsp(MSG_RSP_FAN_OP_MODE);
//original tstat comm engine                       break;
//original tstat comm engine                   case 8: // get fan mode 
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_FAN_OP_MODE);
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           rb8 = cbGetVal8(kCBAtt_fanMode);
//original tstat comm engine                           if(verify_fan_mode == rb8)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_FAN_OP_MODE\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine                   case 9: // set hold
//original tstat comm engine                       break;
//original tstat comm engine                   case 10: // get hold mode
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_MODE_CONFIG);
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           rb16 = cbGetVal16(kCBAtt_holdEnabled);
//original tstat comm engine                           if(verify_hold_mode == rb16)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_MODE_CONFIG\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 11: // set day
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 12:
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_CUR_DAY);
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           rb8 = cbGetVal8(kCBAtt_thermoDay);
//original tstat comm engine                           if(verify_day == rb8)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_CUR_DAY\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                           message_index = 10; // will increment to 11 and do Day again
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 13: // set time
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 14:
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_CUR_TIME);
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           if(verify_time == comm_rsp_target)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_CUR_TIME\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 15: // radio ann
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 16: // radio temp offset
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 17: // set humidity set point
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine                   case 18:
//original tstat comm engine                       response_received = ThermCheckLastRsp(MSG_RSP_HUM_SETPOINT);;
//original tstat comm engine                       if(response_received)
//original tstat comm engine                       {
//original tstat comm engine                           if(verify_humidity_set_point == comm_rsp_nsetHumidity)
//original tstat comm engine                           {
//original tstat comm engine                               tmm[message_index].data_verification_success++;
//original tstat comm engine   
//original tstat comm engine                               // restart data items (temp/mode/fan/hold)
//original tstat comm engine                               data_setting_loop_count++;
//original tstat comm engine                               if(data_setting_loop_count > 59)
//original tstat comm engine                               {
//original tstat comm engine                                   data_setting_loop_count = 0;
//original tstat comm engine                               }
//original tstat comm engine                           }
//original tstat comm engine                           else
//original tstat comm engine                           {
//original tstat comm engine                               UARTiDebug("\r\n\r\nFail to verify MSG_RSP_HUM_SETPOINT\r\n", DEBUG_THERMOSTAT);
//original tstat comm engine                               tmm[message_index].data_verification_fail++;
//original tstat comm engine                               message_index -= 2; // retry
//original tstat comm engine                           }
//original tstat comm engine                       }
//original tstat comm engine                       break;
//original tstat comm engine   
//original tstat comm engine               }
//original tstat comm engine   
//original tstat comm engine               if(response_received)
//original tstat comm engine               {
//original tstat comm engine                   tmm[message_index].high_water_response_time = check_high_water(elapsed_time, tmm[message_index].high_water_response_time);
//original tstat comm engine                   tmm[message_index].low_water_response_time  = check_low_water(elapsed_time, tmm[message_index].low_water_response_time);
//original tstat comm engine                   waiting_on_response = 0;
//original tstat comm engine                   last_message_was_timeout = 0;
//original tstat comm engine   
//original tstat comm engine                   message_index++;
//original tstat comm engine                   if(message_index >= TSTAT_MESSAGE_METRICS_COUNT)
//original tstat comm engine                   {
//original tstat comm engine                       message_index = 0;
//original tstat comm engine                   }
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;
//original tstat comm engine   
//original tstat comm engine               }
//original tstat comm engine               else if(elapsed_time > (TIME_MONITOR_TICK_SECOND * 10))
//original tstat comm engine               {
//original tstat comm engine                   sprintf(buf, "\r\n\r\n Timeout: %d - Message Index: %d\r\n", ++message_timeouts, message_index);
//original tstat comm engine                   UARTiDebug(buf, DEBUG_THERMOSTAT);
//original tstat comm engine   
//original tstat comm engine                   // assume no response coming, kick off a new command
//original tstat comm engine                   if(last_message_was_timeout)
//original tstat comm engine                   {
//original tstat comm engine                       tmm[message_index].subsequent_message_timeouts++;
//original tstat comm engine                   }
//original tstat comm engine                   last_message_was_timeout = 1;
//original tstat comm engine                   tmm[message_index].message_timeouts++;
//original tstat comm engine                   waiting_on_response = 0;
//original tstat comm engine   
//original tstat comm engine                   message_index++;
//original tstat comm engine                   if(message_index >= TSTAT_MESSAGE_METRICS_COUNT)
//original tstat comm engine                   {
//original tstat comm engine                       message_index = 0;
//original tstat comm engine                   }
//original tstat comm engine                   iProcessThermCmdsInUse = MESSAGE_ENGINE_IDLE;
//original tstat comm engine               }
//original tstat comm engine               error_time_read_lower_count = 0;
//original tstat comm engine           }
//original tstat comm engine           else if(cur_time < command_sent_time)
//original tstat comm engine           {
//original tstat comm engine               tmm[message_index].inverted_time_readings++;
//original tstat comm engine               error_time_read_lower_count++;
//original tstat comm engine               if(error_time_read_lower_count > 2)
//original tstat comm engine               {
//original tstat comm engine                   // >2, go ahead and assume the higher read was in error, set base time to current
//original tstat comm engine                   tmm[message_index].inverted_time_reading_corrections++;
//original tstat comm engine                   command_sent_time = cur_time;
//original tstat comm engine                   error_time_read_lower_count = 0;
//original tstat comm engine               }
//original tstat comm engine           }
//original tstat comm engine       }
//original tstat comm engine   }
//original tstat comm engine   
#endif


