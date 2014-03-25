#include "HardwareProfile.h"

//#include "GCMSupport.h"
#include "DeviceState.h"
#include "eeprom.h"

#define INTWINE_SAVE_STATE_VERSION          1

void DeviceStateDataClear(void);

typedef struct
{
    int MagicNumber;             // 0xBADCAFE2
    int StateSaveVersion;        // Start at 1

} INTWINE_SAVE_STATE_HEADER;

typedef struct
{
    INTWINE_SAVE_STATE_HEADER Header;

    int iStateData[SAVE_STATE_MAX_INDEX];

} INTWINE_SAVE_STATE_VER_1;

INTWINE_SAVE_STATE_VER_1 issv1 = 
{
    { MAGIC_NUMBER_DEVICE_SAVE_STATE_INFO, INTWINE_SAVE_STATE_VERSION },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
};

void DeviceStateSet(SAVE_STATE_INDEX index, int value)
{
    int size;

    // Steps to working with a new device state
    // 1) Determine if good index and if the value has changed
    // 2) If it has:
    // 2a) Set the new value
    // 2b) Write the new structure

    // 1)
    if(index < SAVE_STATE_MAX_INDEX)
    {
        if(value != issv1.iStateData[index])
        {
            // 2a)
            issv1.iStateData[index] = value;

            // 2b)
            size = sizeof(INTWINE_SAVE_STATE_VER_1);
            SPIFlashBeginWrite(EEPROM_ADDRESS_DEVICE_SAVE_STATE_INFO);
            SPIFlashWriteArray(&issv1.Header.MagicNumber, size);
        }
    }
}

int DeviceStateGet(SAVE_STATE_INDEX index)
{
    int size;
    int retval = 0;

    // Steps to working with a new device state
    // 1) Determine if good index
    // 2) If it is:
    // 2a) Read EE
    // 2b) Verify magic number
    // 2c) Return the current value
    // 3) Clear 

    // 1)
    if(index < SAVE_STATE_MAX_INDEX)
    {
        // 2a)
        size = sizeof(INTWINE_SAVE_STATE_VER_1);
        SPIFlashReadArray(EEPROM_ADDRESS_DEVICE_SAVE_STATE_INFO, (BYTE*)&issv1.Header.MagicNumber, size);

        // 2b)
        if(issv1.Header.MagicNumber == MAGIC_NUMBER_DEVICE_SAVE_STATE_INFO)
        {
            // 2c)
            retval = issv1.iStateData[index];
        }
        else
        {
            DeviceStateDataClear();
        }
    }
    return(retval);
}

void DeviceStateDataClear(void)
{
    int size;

    // Clear/initialize device state data and nv
    // 1) Clear the device state structure
    // 2) Erase/Write empty structure

    // 1)
    size = sizeof(INTWINE_SAVE_STATE_VER_1);
    memset((char *)&issv1.Header.MagicNumber, 0, size);
    issv1.Header.MagicNumber = MAGIC_NUMBER_DEVICE_SAVE_STATE_INFO;
    issv1.Header.StateSaveVersion = INTWINE_SAVE_STATE_VERSION;

    // 2)
    SPIFlashBeginWrite(EEPROM_ADDRESS_DEVICE_SAVE_STATE_INFO);
    SPIFlashWriteArray(&issv1.Header.MagicNumber, size);
}

