/*********************************************************************
 *
 * Time Monitor - An interrupt driver timer/scheduler feature
 *
 *********************************************************************
 * FileName:        TimerMonitor.c
 * Company:         Intwine Energy, Inc.
 ********************************************************************/
#include "HardwareProfile.h"
#include "Assert.h"
#include "Metrics.h"
#include "Power.h"
#include "TimeMonitor.h"

int tmMillisecondsPerTick = (TIME_MONITOR_TICK_PER_SECOND / 1000);
unsigned long tmTimeMonitorClock = 0;

#define MAX_TIMER_CALLBACKS 11
int tmTickDownTimeIndex0 = 0;
int tmTickDownTimeI[MAX_TIMER_CALLBACKS];
void (*tmCallbackFunctionI[MAX_TIMER_CALLBACKS])(void);

/*****************************************************************************
  Function:
	void TimeMonitorInit(void)

  Summary:
	Initializes the TimeMonitor functionality

  Description:
	Configures and starts Timer 2 for a 1ms interrupt.

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
  // TimeMonitor will use T2Interrupt/Timer 2

//  TRISGbits.TRISG6 = 0;       // Test output

  int i=0;
  for(i=0; i<MAX_TIMER_CALLBACKS; i++)
  {
      tmTickDownTimeI[i] = 0;
      tmCallbackFunctionI[i] = NULL;
  }

  T2CONbits.T32 = 0;          // 16 bit timer
  T2CONbits.TCKPS = 0;        // from 1:1 (0=1:1 (in use), 3=1:8, 6=1:64, 7=1:256)
  // Default is 1ms is 40,000 ticks at 40MHz (dfeined in HardwareProfile.h)
  PR2 = GetPeripheralClock() / TIME_MONITOR_TICK_PER_SECOND;
  TMR2 = 0;
  IFS0CLR = _IFS0_T2IF_MASK;  // Clear any pending interrupt
  IEC0SET = _IEC0_T2IE_MASK;  // Enable the interrupt
  T2CONbits.TON = 1;  // Ship it!

}

/*****************************************************************************
  Function:
	TimeMonitorRegister

  Summary:
	Allows user to register for a callback at a given time

  Description:
	User passes in the desired callback time in ms and the function to
    call when that time expires.

    Note: Given the fact that the timer (currently 1ms) may be just ready
    to interrupt or has just interrupted, the first tick after registration 
    may be anywhere from 0ms to 1ms.  Thus, one interrupt period is always
    added to the registration time.  Thus, the registration time can be considered
    as the minimum time before callback.  
    
    Therefore, a registration for 40ms will truly callback between 40ms and 41ms.

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
  TimeMonitorDisableInterrupt();

  // 2)
  tmTickDownTimeI[index] = callback_time_ms + tmMillisecondsPerTick;
  if(index == 0)
  {
      tmTickDownTimeIndex0 = tmTickDownTimeI[index];
  }

  // 3)
  tmCallbackFunctionI[index] = callback_function;

  // 4)
  TimeMonitorEnableInterrupt();
}

void TimeMonitorChangeTimeI(int index, unsigned int callback_time_ms)
{
  // Steps to registering for a callback
  // 1) Disable interrupt
  // 2) Set callback time - add 1 tick time for interrupt aliasing
  // 3) Enable interrupt
  
  // 1)
  TimeMonitorDisableInterrupt();

  // 2)
  tmTickDownTimeI[index] = callback_time_ms + tmMillisecondsPerTick;
  if(index == 0)
  {
      tmTickDownTimeIndex0 = tmTickDownTimeI[index];
  }

  // 3)
  TimeMonitorEnableInterrupt();
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
  TimeMonitorDisableInterrupt();

  // 2)
  tmTickDownTimeI[index] = 0;

  // 3)
  tmCallbackFunctionI[index] = NULL;

  // 4)
  TimeMonitorEnableInterrupt();
}


//void _ISR __attribute__((__no_auto_psv__)) _T2Interrupt(void)
void __ISR(_TIMER_2_VECTOR, ipl1) T2InterruptServiceRoutine(void)
{
/*     static int toggle=1; */
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

    // Reset interrupt flag
    IFS0CLR = _IFS0_T2IF_MASK;
}


/*****************************************************************************
  Function:
	TimeMonitorEnableInterrupt
	TimeMonitorDisableInterrupt

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
void TimeMonitorEnableInterrupt()
{
  IEC0SET = _IEC0_T2IE_MASK;  // Enable the interrupt
}
void TimeMonitorDisableInterrupt()
{
  IEC0CLR = _IEC0_T2IE_MASK;  // Disable the interrupt
}
