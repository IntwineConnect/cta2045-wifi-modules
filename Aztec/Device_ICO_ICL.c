/********************************************************************************
 * Add Intwine information here
 ********************************************************************************/

#include "HardwareProfile.h"

#if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)

#include "LOG_Manager.h"
#include "Device_ICO_ICL.h"
#include "ADE7753.h"    // pick up meter reading for sending when relay state changes
#include "Entek_Lib.h"
#include "DeviceState.h"    // pick up NV saving of coil state


void PlugStripOneTimeSetup(void)
{
    int restore_state;

    restore_state = DeviceStateGet(SAVE_STATE_ONE_TIME_SETUP);
    if(restore_state != ICO_ONE_TIME_SETUP_MAGIC_NUMBER)
    {
        PlugStripSetRelay(1, kWhoStartup, NULL);
        DeviceStateSet(SAVE_STATE_ONE_TIME_SETUP, ICO_ONE_TIME_SETUP_MAGIC_NUMBER);
    }
}

void PlugStripRestoreState(void)
{
    int restore_state;
    restore_state = DeviceStateGet(SAVE_STATE_COIL_STATE);
    PlugStripSetRelay(restore_state, kWhoStartup, NULL);
}

static UINT8 cur_state = 0;

void PlugStripSetRelay(UINT8 state, LogWhodunnit_e who, char *extra)
{


  // Toggle feature for SmartPlug user button
  if(state == 2)
  {
//    cur_state = cbGetVal8(kCBAtt_relayState);
    if(cur_state == 0)
    {
      state = 1;
    }
    else
    {
      state = 0;
    }
  }

  // Set proper coil state
  if(state == 0)      // 0 = off
  {
#ifdef INTWINE_CONNECTED_OUTLET
      LATBCLR = 0x0004;
      ICO_RED_ON();
      ICO_GRN_OFF();
//      LOG_ICO(kLoadTurnedOff, who, extra);
#else
      #ifdef INTWINE_CONNECTED_LOAD_CONTROL
      Entek_Open();
//      LOG_ICL(kLoadTurnedOff, who, extra);
      #endif
#endif
  }
  else
  {

#ifdef INTWINE_CONNECTED_OUTLET
      LATBSET = 0x0004;
      ICO_RED_OFF();
      ICO_GRN_ON();
//      LOG_ICO(kLoadTurnedOn, who, extra);
#else
    #ifdef INTWINE_CONNECTED_LOAD_CONTROL
      Entek_Close();
//      LOG_ICL(kLoadTurnedOn, who, extra);
    #endif
#endif
  }

//  cbAttrSetByIdAutoSend8(kCBAtt_relayState, state);
  DeviceStateSet(SAVE_STATE_COIL_STATE, state);
}


#endif // #if defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)

