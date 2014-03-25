/**
 * File: app.h
 * Desc: Definitions
 *
 */
#ifndef APP_H
#define APP_H 1

// Intwine Legacy DR Portal Information
#define CBDEVICE "tstat"		  // Valid:  dru, tstat, splug 
#define CBDEVICENETWORK "IP"  // Could also be AMI.  Tells device where to synch time from.
#define SEND_HB 0x01
#define SEND_ERR 0x02
#define TIMESYNCHTIMEOUT 60  // If not a valid timesynch in TIMESYNCHTIMEOUT seconds, send an error message to the server. 
#define WEEKLY_REBOOT_TIMER 604800 // Do a system reset every week in case device go awol
#define SHORT_HEARTBEAT_TIMER 30 // Should be 30.  This is how often the heartbeat is done.  This will dynamically change if event is in progress.
#define LONG_HEARTBEAT_TIMER 30 //  Should be 900 This is how often the heartbeat is done.  This will dynamically change if event is in progress.
#define OPTOUT_HEARTBEAT_WINDOW 30 // Should be 900 This means that 15 minutes before a scheduled event, the opt outs become faster. Since we switch to SHORT_HEARTBEAT_TIMER
#define LONG_HEARTBEAT_DELAY_INVOKE 900 // Should be at least 900 to be within window.  After and event has completed this is how long we do heartbeats for
#define TIME_ZONE_OFFSET 18000
#define TIME_PROPAGATION_OFFSET 1620 //  27 minutes of time.  15 minutes + or - start time + 12 minutes max time diversity.

extern enum _CommState
{
	DR_SERVER = 0,
	UPGRADE,
	SEND_OPTOUT,
	PROCESS_HOMEAREA
} CommState;

// Server properties names and lengths.
//#define PROP_NAME_DAY   "day"    // Get Response from Set Day Command

#define PROP_BUFFER_SIZE    100      // Size of property message buffer.
#define RESP_BUFFER_SIZE    100      // Size of resp message buffer.  Note this is currenctly the AES block size, but can change.

/** Initialize the application state machine.
 *
 */
void AppTaskInit(void);
void UsbConfigLoop(void);

/** Advance the application state machine.
 *
 */
void AppTaskLoop(void);
void CBProcessRcvdMessages(char *valPtr);

#endif // APP_H
