#ifndef _TIME_MONITOR_H
#define _TIME_MONITOR_H

/********************************************************************************
 * Add Intwine information here
 ********************************************************************************/

void TimeMonitorInit(void);
void TimeMonitorRegister(unsigned int callback_time_ms, void (*callback_function)(void));
void TimeMonitorCancel();
void TimeMonitorRegisterI(int index, unsigned int callback_time_ms, void (*callback_function)(void));
void TimeMonitorChangeTimeI(int index, unsigned int callback_time_ms);
void TimeMonitorCancelI(int index);
void TimeMonitorUpdate(void);
void TimeMonitorEnableInterrupt(void);
void TimeMonitorDisableInterrupt(void);

// Read TICKS per SECOND, so, 1000 is a 1ms tick resolution
#define TIME_MONITOR_TICK_PER_SECOND 1000

#endif
