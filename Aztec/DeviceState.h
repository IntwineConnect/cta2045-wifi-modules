#ifndef DEVICESTATE_H
#define DEVICESTATE_H

typedef enum
{
    // Each device can start at 0 (since they are on separate devices)

    SAVE_STATE_COIL_STATE       = 0,        // Used for both ICO and ICL
    SAVE_STATE_LAST_LCL_MRID    = 1,
    SAVE_STATE_ONE_TIME_SETUP   = 2,

    SAVE_STATE_ICL_XXX      = 0,


    SAVE_STATE_PCT_XXX      = 0,


    SAVE_STATE_MAX_INDEX    = 30,

} SAVE_STATE_INDEX;

void DeviceStateSet(SAVE_STATE_INDEX index, int value);
int  DeviceStateGet(SAVE_STATE_INDEX index);

#define ICO_ONE_TIME_SETUP_MAGIC_NUMBER 0x77778899

#endif  //#ifndef DEVICESTATE_H

