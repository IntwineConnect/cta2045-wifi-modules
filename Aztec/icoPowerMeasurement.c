#include "HardwareProfile.h"

#ifdef INTWINE_CONNECTED_OUTLET

#include "TCPIP_Stack/Tick.h"
#include "ADE7753.h"
#include "eeprom.h"
#include "Power.h"
#include "TimeMonitor.h"

    #define VOLTAGE_DELTA 30       // (10 = 1 volt)  Report when voltage changes by this amount (volts)
    #define CURRENT_DELTA 2000     // (1000 = 1 amp) Report when current changes by this amount (amps)
    #define POWER_DELTA   20       // (10 = 1 watt)  Report when power changes by this amount (watts)
    #define ENERGY_DELTA  2000     // (1000 = 1 watt-hours) Report when energy (accumulated power) changes by this amount (watt-hours)

    #define ABSOLUTE_REPORTING_TIME  60   // Reporting time if delta values have not fired off the reporting of attributes
    #define MINIMUM_REPORTING_TIME   10   // Can only report every 10 seconds, even if values are changing rapidly, keeps data rate lower

    #define ENERGY_NONVOLATILE_SAVE_TIME  600   // Save energy to NV every 10 minutes

DWORD RestoreCumulativeEnergy(); //DWORD address);
void StoreCumulativeEnergy(DWORD Energy);

static DWORD calc_cumulative_energy_mwh = 0;    // global so it can be restore in Init, used in Task

typedef struct
{
	DWORD status;
	DWORD AEnergy;
	DWORD LAEnergy;
	DWORD Voltage;
	DWORD Current;
	DWORD Waveform;
	DWORD DeciVolts;
	DWORD MilliAmps;
	DWORD DeciWatts;
	DWORD MilliwattHour;
	char logStatement[50];
	
} EnergyStorage_t;	

#define STORE_COUNT 10
/* static EnergyStorage_t store[STORE_COUNT]; */
/* static int storeCounter = 0;               */

/********************************************************************************
 * Function:      PowerMeasurementInit
 * Performs:      Sets up all necessary registers, variables, etc. for power measurement
 * Return values: void
 * Notes:         None
 ********************************************************************************/
void PowerMeasurementInit()
{

#ifndef TEST_ICO_POWER_MEASUREMENT
#ifdef NO_ICO_POWER_MEASUREMENT
    return;
#endif
#endif

#ifndef TEST_ICO_POWER_MEASUREMENT
    initMeterChip();
#endif

    calc_cumulative_energy_mwh = RestoreCumulativeEnergy();
}


/********************************************************************************
 * Function:      PowerMeasurementTask
 * Performs:      Performs the repetitive power measuring, monitoring the time and
 *                updating attributes as required
 * Return values: void
 * Notes:         None
 ********************************************************************************/
void PowerMeasurementTask()
{
    static DWORD TickTimer = 0, LastSentTime = 0, LastNVTime = 0; // , zeroCrossingTimeout = 0, lastReadTime = 0;
    static DWORD last_voltage_dv=0, last_current_ma=0, last_instantaneous_power_dw=0, last_cumulative_energy_mwh=0;

    static int send_attributes_to_server = 0;   // set to static to latch a change and wait for minimum time to expire
    DWORD calc_voltage_dv=0, calc_current_ma=0, calc_instantaneous_power_dw=0;
    DWORD time_diff=0;
    char buf[50];
    
    DWORD powerReading=0, energyReading=0, curtime;
    static DWORD powerReadingArray[10], array_index=0;;
    static DWORD energyReadingArray[10];
    static DWORD timeReadingArray[10];


#ifndef TEST_ICO_POWER_MEASUREMENT
#ifdef NO_ICO_POWER_MEASUREMENT
    return;
#else
    DWORD read_voltage, read_current;
#endif
#endif

    // 1)
    DWORD status = 0;
    const int readingBufLen = 50;
    char readingBuf[readingBufLen];
    
    //looking for designated number of zero crossing. Or, a timeout (2x the expected duration), in which case we fake it.
//    if(status & (0x1 << 2)) // || sendFakeData)	//CYCEND flag
    curtime = TickGet();
    if((curtime - TickTimer) >= TIME_MONITOR_TICK_SECOND*1)
    {
        TickTimer=curtime;

        // 2)
#ifdef TEST_ICO_POWER_MEASUREMENT

        // test code
        last_voltage_dv = 1200;
        last_current_ma = 500;
        last_instantaneous_power_dw = ((last_voltage_dv * last_current_ma) / 1000);   // deciwatts
        calc_cumulative_energy_mwh += 1;
        last_cumulative_energy_mwh = calc_cumulative_energy_mwh;

#else
#ifndef NO_ICO_POWER_MEASUREMENT

        ReadCurrentAndVoltage(&read_current, &read_voltage);
        PowerAPI_Read(&powerReading, &energyReading);

        powerReadingArray[array_index] = powerReading;
        energyReadingArray[array_index] = energyReading;
        timeReadingArray[array_index]  = curtime;
        array_index++;
        if(array_index > 9)
        {
            array_index = 0;
        }

        // 3)
        // See SmartPlug.xlsx for all the constants below
        calc_voltage_dv = (((read_voltage / 100) * 21740) / 100000);                  // decivolts
        calc_current_ma = ((read_current * 144) / 100000);                     // milliamps
        
        calc_instantaneous_power_dw = powerReading * 10;

        printf("\r\nPower Reading - register: 0x%08lx", powerReading);
        sprintf(buf, "\r\nInstantaneous Power dw calc: %ld", calc_instantaneous_power_dw);

        if(energyReading > 0)
        {
        	calc_cumulative_energy_mwh += (energyReading * 100 / 360);	//incoming is w*1s, need mwh
        }
        
        // 4)
        if(abs((int)(last_voltage_dv - calc_voltage_dv)) > VOLTAGE_DELTA)
        {
            send_attributes_to_server = 1;
        }
        if(abs((int)(last_current_ma - calc_current_ma)) > CURRENT_DELTA)
        {
            send_attributes_to_server = 1;
        }
        if(abs((int)(last_instantaneous_power_dw - calc_instantaneous_power_dw)) > POWER_DELTA)
        {
            send_attributes_to_server = 1;
        }
        if(abs((int)(last_cumulative_energy_mwh - calc_cumulative_energy_mwh)) > ENERGY_DELTA)
        {
            send_attributes_to_server = 1;
        }

        last_voltage_dv = calc_voltage_dv;
        last_current_ma = calc_current_ma;
        last_instantaneous_power_dw = calc_instantaneous_power_dw;
        last_cumulative_energy_mwh = calc_cumulative_energy_mwh;
        
        /*
        //load up the test array
		store[storeCounter].AEnergy 		= read_cumulative_energy;
		store[storeCounter].LAEnergy 		= read_instantaneous_power;
		store[storeCounter].Voltage 		= read_voltage;
		store[storeCounter].Current 		= read_current;
		store[storeCounter].status 			= status;
		ReadWaveform(&(store[storeCounter].Waveform));
		
		store[storeCounter].DeciVolts		= calc_voltage_dv;
		store[storeCounter].MilliAmps		= calc_current_ma;
		store[storeCounter].DeciWatts		= calc_instantaneous_power_dw;
		store[storeCounter].MilliwattHour	= calc_cumulative_energy_mwh;
		*/
		
		
		//null terminator
		readingBuf[readingBufLen-1] = 0;
		sprintf(readingBuf, "%04x,%08x,%08x,%08x,%08x",
			(unsigned int)status, 
			(unsigned int)read_voltage, 
			(unsigned int)read_current, 
			(unsigned int)powerReading, 
			(unsigned int)energyReading);
			
		//strcpy(store[storeCounter].logStatement, readingBuf);
		//storeCounter++;
			
		//send up the logged values. This is to help with calibration, and expect it to be removed later.
//minimize		LOG_ICO(kRawPowerReadings, kWhoMyself, readingBuf);
			
		/*
		if (storeCounter >= 10)
		{
			storeCounter = 0;
		}
		*/	

#endif
#endif

        // 5)
        time_diff = (TickTimer - LastSentTime) / TIME_MONITOR_TICK_SECOND;
        if( (send_attributes_to_server && (time_diff >= MINIMUM_REPORTING_TIME)) ||
            (time_diff >= ABSOLUTE_REPORTING_TIME))
        {
//            cbAttrSetByIdForceSend32(kCBAtt_voltage, last_voltage_dv);
//            cbAttrSetByIdForceSend32(kCBAtt_current, last_current_ma);
//            cbAttrSetByIdForceSend32(kCBAtt_instantaneousPower, last_instantaneous_power_dw);
//            cbAttrSetByIdForceSend32(kCBAtt_cumulativeEnergy, last_cumulative_energy_mwh/100);  // report dw, not mwh

            LastSentTime = TickTimer;
            send_attributes_to_server = 0;
        }

        // 6)
        if((TickGet() - LastNVTime) > TIME_MONITOR_TICK_SECOND*ENERGY_NONVOLATILE_SAVE_TIME)
        {
            StoreCumulativeEnergy(calc_cumulative_energy_mwh);
            LastNVTime = TickGet();
        }
    }
}


//DWORD RestoreCumulativeEnergy(DWORD address)
DWORD RestoreCumulativeEnergy(void)
{
    DWORD cumulative_energy;
    short *ramPtr;
    unsigned long int FirmwareAddress=EEPROM_ADDRESS_POWER_MEASUREMENT_INFO; //0x90000;

    DWORD magic_number = MAGIC_NUMBER_POWER_MEASUREMENT_INFO;

#ifndef TEST_ICO_POWER_MEASUREMENT
#ifdef NO_ICO_POWER_MEASUREMENT
    return(0);
#endif
#endif

    SPIFlashReadArray(FirmwareAddress, (BYTE*)&magic_number, 4);
	FirmwareAddress+=4;
    if(magic_number == MAGIC_NUMBER_POWER_MEASUREMENT_INFO)
    {
        // If magic number, get the data
        SPIFlashReadArray(FirmwareAddress, (BYTE*) &cumulative_energy, 4);
    }
    else
    {
        cumulative_energy = 0;
        StoreCumulativeEnergy(cumulative_energy);
    }

    putsUART("\r\nRead Cumulative Energy");
    return(cumulative_energy);
}

void StoreCumulativeEnergy(DWORD Energy)
{
    unsigned long int FirmwareAddress=EEPROM_ADDRESS_POWER_MEASUREMENT_INFO; //0x90000;

    DWORD magic_number = MAGIC_NUMBER_POWER_MEASUREMENT_INFO;

#ifndef TEST_ICO_POWER_MEASUREMENT
#ifdef NO_ICO_POWER_MEASUREMENT
    return;
#endif
#endif

	//set "MAGIC" number
	SPIFlashBeginWrite(FirmwareAddress);
	SPIFlashWriteArray((BYTE*) &magic_number, 4);
	//Write Cal Info
	SPIFlashWriteArray((BYTE*) &Energy, 4);

    putsUART("Write Cumulative Energy");
}

void ResetCumulativeEnergy(void)
{
    StoreCumulativeEnergy(0);
}


#endif // #ifdef INTWINE_CONNECTED_OUTLET
