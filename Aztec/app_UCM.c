#include "HardwareProfile.h"
#ifdef DFW
#include "DFW.h"
#endif

/**
 * File: app.c
 */
//gcm #include "TCPIP Stack/TCPIP.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "OpenADRClient.h"
#include "app.h"
#include "eeprom.h"
#include "TimeMonitor.h"

int lostlink_flag; 

extern DWORD dwInternalTicks;

// Timers to control when actions happen.
//now in EventTCPClient.c DWORD SendTCPMsg_Timer=0; // Only Send TCP message 2 seconds after sending UDP msg. 
DWORD SendToServerTimeout=0; // Send every heartbeat.
DWORD MainSysTickTimer=0; // The Main System Tick Timer
DWORD MainSystemUNIXTime=0; // The Main System Time
DWORD WriteBackTimer=0;  // Timer to write back temp
//cbdmDWORD TargDelayTimer=0;  // In R3, provide a bit of a delay while returning the value!
int SendToServer=0;
int SendOptOut=0;
int HANMsg_Rdy2Process=0;
extern char ipbufptr[30]; // IP Address Buffer
extern char changeIP;
extern int heartBeatTimer;

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

char  gRespRec=0; // response received flag

int active_flag=1;  // Change to zero once into main loop to refresh props or preload thermo with props
DWORD ZG_Con_Timer=0;    // Timer inialized on bootup to track is ZG AP Link is up.
DWORD SPI_Watch_Timer=0;  // Timer inialized on bootup to track is CB Stack Link is up.
int spi_timeout=1;  // used for the tstat spi in that project, default to 1 here
int Scheduling=0;   // If in middle of scheduling, do not poll tstat 

static enum _ConnectState
{
  START =0,
  SPI_WORKING,
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
} ConnectState = START;

#ifdef DFW
enum _CommState TCPCommState=UPGRADE;
#else
enum _CommState TCPCommState=DR_SERVER;
#endif

/****************************************************************************/
// Perform application related initialization here.
void AppTaskInit(void)
{

	InitOpenADRComm();

}

void AppTaskLoop(void)
{

#ifdef DFW
      DFWRun();
#else
//    if (!OpenADRClient())
//    {
//        TCPCommState = UPGRADE;
//    }
#endif

}