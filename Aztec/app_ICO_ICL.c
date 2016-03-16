#include "HardwareProfile.h"

#if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)

#ifdef DFW
#include "DFW.h"
#endif

/**
 * File: app.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//gcm #include "BerkeleyUDPTimePool.h"
//#include "time.h"
#include "TCPIP_Stack/Tick.h"
#include "app.h"
#include "eeprom.h"

#if defined(INTWINE_CONNECTED_OUTLET)
#include "icoPowerMeasurement.h"
#include "icoUserButton.h"
#endif
#if defined(INTWINE_CONNECTED_LOAD_CONTROL)
#include "iclPowerMeasurement.h"
#endif

int lostlink_flag; 

extern DWORD dwInternalTicks;

// Timers to control when actions happen.
DWORD MainSysTickTimer=0; // The Main System Tick Timer
DWORD MainSystemUNIXTime=0; // The Main System Time
DWORD AppTickTimer=0;      // App timer for power measurements

/****************************************************************************/
// Perform application related initialization here.
void AppTaskInit(void)
{
  static int timeSynch=0; // 1 if valid timeSynch, 0 if not.

// Initialize Power Measurement
#if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)
#ifdef REPORT_POWER_MEASUREMENT
  PowerMeasurementInit();
#endif
#endif

#ifdef INTWINE_CONNECTED_OUTLET
  UserButtonInit();
#endif

//  upgradeFlag.status=0x01;  // Check for an upgrade each time you reboot

}


void AppTaskLoop(void)
{
  char timestring[35];
  static int timeSynch=0; // 1 if valid timeSynch, 0 if not.

#ifdef INTWINE_CONNECTED_OUTLET
  // Perform user button task before wi-fi in case the wi-fi
  // state machine returns from this call (due to adhoc or other problem)
  UserButtonTask();
#endif


  // INSERT MAIN APPLICATION CODE HERE.
  // Get and update the time from the DR, or CB server or if activated, Time Pool Network

  if((TickGet() - MainSysTickTimer) > TICK_SECOND*5)
  {
    MainSysTickTimer=TickGet();
    //MainSystemUNIXTime=BerkeleySNTPGetUTCSeconds()+14400;
    printf("\r\nTime:%ld\r\n", MainSystemUNIXTime);
    MainSystemUNIXTime=MainSystemUNIXTime+5;
    if(MainSystemUNIXTime > 1280000000)  // Just make sure it is right after initialization.
    {
      timeSynch=1; // Valid Time.  Wait for Timesynch or Grab from Time pool.	
    }
  }


#ifdef INTWINE_CONNECTED_OUTLET
  if((TickGet() - AppTickTimer) > TICK_SECOND)
  {
	    AppTickTimer=TickGet();
        PowerAPI_Sample();
    }
#endif

  // Update power information
#if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)
#ifdef REPORT_POWER_MEASUREMENT
  PowerMeasurementTask();
#endif
#endif

}

#endif
