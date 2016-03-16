#include "HardwareProfile.h"

#ifdef INTWINE_CONNECTED_OUTLET

#include "TCPIP_Stack/Tick.h"
#include "LOG_Manager.h"

#define BUTTON_STATE_IDLE                 0
#define BUTTON_STATE_DOWN_DEBOUNCING      1
#define BUTTON_STATE_DOWN                 2
#define BUTTON_STATE_UP_DEBOUNCING        3
#define BUTTON_STATE_UP                   4

#define BUTTON_UP                         1       // Net is pulled up
#define BUTTON_DOWN                       0       // Button pulls net to ground

#define BUTTON_DEBOUNCE_TIME              (TIME_MONITOR_TICK_SECOND/50) // 20ms

static int ButtonState;

/********************************************************************************
 * Function:      UserButtonTask
 * Performs:      Performs monitor and debounce button
 * Return values: void
 * Notes:         None
 ********************************************************************************/
void UserButtonTask(void)
{
  static DWORD debounce_time = 0;
  static DWORD ButtonTimer = 0;

  int switch_value;
  DWORD elapsed_task_time = 0;
  DWORD current_time = 0;
  DWORD button_debounce_time = TICK_SECOND / 10;


  // Steps to processing the button
  // 1) read the current state
  // 2) get the elapsed time since this task was last called
  // 3) move into the processing for the state machine

  // 1)
  if(PORTE & BIT_8)
  {
       switch_value = BUTTON_UP;
  }
  else
  {
      switch_value = BUTTON_DOWN;
  }

  // 2)
  current_time = TickGet();                         // Get current time
  elapsed_task_time = current_time - ButtonTimer;   // Time from last task call
  ButtonTimer = current_time;                       // Save current time for next task call

  // 3)
  switch(ButtonState)
  {
    case BUTTON_STATE_IDLE:
      // set initial state without debugging
      if(switch_value == BUTTON_UP)
      {
        ButtonState = BUTTON_STATE_UP;
      }
      else
      {
        ButtonState = BUTTON_STATE_DOWN;
      }
      break;

    case BUTTON_STATE_UP:
      // If button changes state, go to debounce state
      if(switch_value == BUTTON_DOWN)
      {
        debounce_time = 0;
        ButtonState = BUTTON_STATE_DOWN_DEBOUNCING;
      }
      break;

    case BUTTON_STATE_DOWN_DEBOUNCING:
      // steps to debouncing
      // a) if button still down, great, accumulate time
      // b) debounce time exceeded, change state (b1)
      // c) button goes up, revert to button up state

      // a)
      if(switch_value == BUTTON_DOWN)
      {
        // b)
        debounce_time += elapsed_task_time;
        if(debounce_time >= button_debounce_time)
        {
          // b1)
          ButtonState = BUTTON_STATE_DOWN;
          PlugStripSetRelay(2, kWhoUserButton, NULL);
        }
      }
      else
      {
        // c)
        ButtonState = BUTTON_STATE_UP;
      }
      break;

    case BUTTON_STATE_DOWN:
      // If button changes state, go to debounce state
      if(switch_value == BUTTON_UP)
      {
        debounce_time = 0;
        ButtonState = BUTTON_STATE_UP_DEBOUNCING;
      }
      break;

    case BUTTON_STATE_UP_DEBOUNCING:
      // steps to debouncing
      // a) if button still up, great, accumulate time
      // b) debounce time exceeded, change state (b1)
      // c) button goes down, revert to button down state

      // a)
      if(switch_value == BUTTON_UP)
      {
        // b)
        debounce_time += elapsed_task_time;
        if(debounce_time >= button_debounce_time)
        {
          // b1)
          ButtonState = BUTTON_STATE_UP;
        }
      }
      else
      {
        // c)
        ButtonState = BUTTON_STATE_DOWN;
      }
      break;

  }
}

/********************************************************************************
 * Function:      UserButtonInit
 * Performs:      Sets up GPIO bit and button state machine
 * Return values: void
 * Notes:         None
 ********************************************************************************/
void UserButtonInit(void)
{
  DWORD i = 0;

  ButtonState = BUTTON_STATE_IDLE;

  TRISESET = 0x0100;   // Set RE8 as an input to read the button

  i++; // bp                                 

//  RadiodRegisterCallbackFunction(UserButtonTask);

}

#endif // #ifdef INTWINE_CONNECTED_OUTLET


