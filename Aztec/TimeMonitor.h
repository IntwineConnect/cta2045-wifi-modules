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
void TimeMonitorEnable(void);
void TimeMonitorDisable(void);

// Read TICKS per SECOND, so, 100 is 100 ticks per second
// 1000ms per second divided by 100 ticks per second = 10 ms per tick
#define TIME_MONITOR_TICK_SECOND 100

#endif
