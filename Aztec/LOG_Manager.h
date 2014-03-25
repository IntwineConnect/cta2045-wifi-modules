/**
 * File: LOG_Manager.h
 * Desc: Logging function definitions.
 *
 * Author: David Nikkel (david@thenikkels.com)
 */

#ifndef LOGMANAGER_H
#define LOGMANAGER_H

//define this to include the self test functions
//#define LOG_MANAGER_TESTING

//sizing for the circular buffer
#define LOG_BUFFER_DEPTH 	7
#define LOG_BUFFER_LENGTH	50

//ASCII is the form the end log will take. It makes sense for the ascii value
//being sent to be the value of the enumeration.
typedef enum
{
  kDeviceICL          = 0x4C, //'L' (Load Control Switch)
  kDeviceICO          = 0x53, //'S' (Smart Outlet)
  kDevicePCT          = 0x54, //'T' (Thermostat)
} DeviceEnum_e;

typedef enum
{
  kLoadTurnedOff      = 0x30, //'0'
  kLoadTurnedOn       = 0x31, //'1'
  kRawPowerReadings   = 0x32, //'2'
} LogICxDataItems_e;

typedef enum
{
  kModeSetToOff       = 0x30, //'0'
  kModeSetToHeat      = 0x31, //'1'
  kModeSetToCool      = 0x32, //'2'
  kHoldEnabled        = 0x33, //'3'
  kHoldDisabled       = 0x34, //'4'
  kHeatTempSet        = 0x35, //'5'
  kCoolTempSet        = 0x36, //'6'
  kFanSetToAuto       = 0x37, //'7'
  kFanSetToOn         = 0x38, //'8'
} LogPCTDataItems_e;

typedef enum
{
  kAppendNewTemp      = 0x41, //'A'
  kWhoCloudbus        = 0x43, //'C'
  kWhoHEM             = 0x48, //'H'
  kWhoUtility         = 0x55, //'U'
  kWhoUserButton      = 0x42, //'B'
  kWhoStartup         = 0x53, //'S'
  kWhoMyself          = 0x4D, //'M'
} LogWhodunnit_e;


//functions
#define PCT_DATA_UNUSED 0x44556677
void LOG_PCT(LogPCTDataItems_e action, LogWhodunnit_e who, int data, char *extra);
void LOG_ICO(LogICxDataItems_e action, LogWhodunnit_e who, char *extra);
void LOG_ICL(LogICxDataItems_e action, LogWhodunnit_e who, char *extra);
void LOG_ICx(DeviceEnum_e devType, LogICxDataItems_e action, LogWhodunnit_e who, char *extra);

//log any string you want. However, anything over 50 characters (including the NULL)
//wil get truncated.
void LOG_FREEFORM(const char* logString);

void         LOG_Initialize();
int          LOG_GetLogCount(void);
int          LOG_GetOverflowCount();
void         LOG_ClearOverlowCount();
const char * LOG_GetNextEntry(void);


/************************
* Test only functions!
*************************/
#ifdef LOG_MANAGER_TESTING
int LOG_StartTest(void);
#endif


#endif
/*********************** END OF FILE ************************/
