#ifndef POWER_H
#define POWER_H

void  PowerAPI_Initialize(void);                                // Initialize the power reading system
void  PowerAPI_Read(DWORD *inst_power, DWORD *accum_energy);    // Returns actual power and energy used
void  PowerAPI_SetupCalibrationData(unsigned short *);          // Setup calibration data
void  PowerAPI_Sample(void);                                    // Sample actual power and energy used

void  PowerAPI_TestApp(void);                                   // Test application

#endif  //#ifndef POWER_H



