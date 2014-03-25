/*********************************************************************
 *
 * Time Monitor - An interrupt driver timer/scheduler feature
 *
 *********************************************************************
 * FileName:        TimerMonitor.c
 * Company:         Intwine Energy, Inc.
 ********************************************************************/
#include "HardwareProfile.h"

/* #include "Compiler.h"             // Automatically brings in hardware structures */
#include "TCPIP Stack/Tick.h"
#include "Assert.h"
#include "Metrics.h"
#include "Power.h"
#include "TimeMonitor.h"

int tmMillisecondsPerTick = 1000 / TIME_MONITOR_TICK_SECOND;
unsigned long tmTimeMonitorClock = 0;

#define MAX_TIMER_CALLBACKS 6
int tmTickDownTimeIndex0 = 0;
int tmTickDownTimeI[MAX_TIMER_CALLBACKS];
void (*tmCallbackFunctionI[MAX_TIMER_CALLBACKS])(void);

static volatile BOOL TimeMonitorEnabled = TRUE;


/*****************************************************************************
  Function:
	void TimeMonitorInit(void)

  Summary:
	Initializes the TimeMonitor functionality

  Description:
	Initializes callback array. Uses Timer1 configured in Tick.c

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void TimeMonitorInit(void)
{
  // TimeMonitor will use Timer1 configured in Tick.c

  int i=0;
  for(i=0; i<MAX_TIMER_CALLBACKS; i++)
  {
      tmTickDownTimeI[i] = 0;
      tmCallbackFunctionI[i] = NULL;
  }
}

/*****************************************************************************
  Function:
	TimeMonitorRegister

  Summary:
	Allows user to register for a callback at a given time

  Description:
	User passes in the desired callback time in ms and the function to
    call when that time expires.

    Note: Given the fact that the timer (currently 10ms) may be just ready
    to interrupt or has just interrupted, the first tick after registration 
    may be anywhere from 0ms to 10ms.  Thus, one interrupt period is always
    added to the registration time.  Thus, the registration time can be considered
    as the minimum time before callback.  
    
    Therefore, a registration for 40ms will truly callback between 40ms and 50ms.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
    None
    
  ***************************************************************************/

void TimeMonitorRegisterI(int index, unsigned int callback_time_ms, void (*callback_function)(void))
{
  // Steps to registering for a callback
  // 1) Disable interrupt
  // 2) Set callback time - add 1 tick time for interrupt aliasing
  // 3) Set callback function
  // 4) Enable interrupt
  
  // 1)
  TimeMonitorDisable();

  // 2)
  tmTickDownTimeI[index] = callback_time_ms + tmMillisecondsPerTick;
  if(index == 0)
  {
      tmTickDownTimeIndex0 = tmTickDownTimeI[index];
  }

  // 3)
  tmCallbackFunctionI[index] = callback_function;

  // 4)
  TimeMonitorEnable();
}

void TimeMonitorChangeTimeI(int index, unsigned int callback_time_ms)
{
  // Steps to registering for a callback
  // 1) Disable interrupt
  // 2) Set callback time - add 1 tick time for interrupt aliasing
  // 3) Enable interrupt
  
  // 1)
  TimeMonitorDisable();

  // 2)
  tmTickDownTimeI[index] = callback_time_ms + tmMillisecondsPerTick;
  if(index == 0)
  {
      tmTickDownTimeIndex0 = tmTickDownTimeI[index];
  }

  // 3)
  TimeMonitorEnable();
}


/*****************************************************************************
  Function:
	TimeMonitorCancel

  Summary:
	Allows user to cancel a previously scheduled callback

  Description:
	Clears the callback time and function

    Note: There may be a small window, prior to disabling the interrupt, 
    that the callback may still fire.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
  	None

  ***************************************************************************/

void TimeMonitorCancelI(int index)
{
  // Steps to canceling for a callback
  // 1) Disable interrupt
  // 2) Clear callback time
  // 3) Clear callback function pointer
  // 4) Enable interrupt
  
  // 1)
  TimeMonitorDisable();

  // 2)
  tmTickDownTimeI[index] = 0;

  // 3)
  tmCallbackFunctionI[index] = NULL;

  // 4)
  TimeMonitorEnable();
}


void TimeMonitorUpdate(void)
{
    int i, bp;

    // Update clock
    tmTimeMonitorClock++;

    if((tmTimeMonitorClock & 0xFFFF0000) == 0xFFFF0000)
    {
        ASRT_INT(0, "Timer overflow", WATCHDOG_TIME_MONITOR_OVERFLOW);
    }

    // Steps to processing a callback
    // 1) Determine if a registration is currently in place, tick time 1= 0
    // 2) Tick down the timer
    // 3) If the timer expires, make the callback
    // 4) Cancel the callback

    for(i=0; i<MAX_TIMER_CALLBACKS; i++)
    {
        // 1)
        if(tmTickDownTimeI[i] > 0)
        {
          // 2)
          tmTickDownTimeI[i] -= tmMillisecondsPerTick;
    
          if(tmTickDownTimeI[i] <= 0)
          {
            // 3)
            if(tmCallbackFunctionI[i] != NULL)
            {
                (*tmCallbackFunctionI[i])();
            }
            else
            {
                bp = 1;
            }
    
            // 4)
            if(i == 0)
            {
                // index 0 is special ... self re-registers
                tmTickDownTimeI[i] = tmTickDownTimeIndex0;
            }
            else
            {
                TimeMonitorCancelI(i);
            }
          }
        }
    }
}


/*****************************************************************************
  Function:
	TimeMonitorEnable
	TimeMonitorDisable

  Summary:
	Enables interrupt

  Description:
	None

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	These functions are used for interrupt/thread protection
  ***************************************************************************/
void TimeMonitorEnable()
{
    TimeMonitorEnabled = TRUE;
//  IEC0SET = _IEC0_T1IE_MASK;   // Enable the interrupt
//  IEC0bits.T2IE = 1;  // Enable the interrupt
}
void TimeMonitorDisable()
{

    TimeMonitorEnabled = FALSE;
//  IEC0CLR = _IEC0_T1IE_MASK;  // Disable the interrupt
//  IEC0bits.T2IE = 0;  // Disable the interrupt
}
