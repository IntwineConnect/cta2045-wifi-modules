#include "HardwareProfile.h"

#ifdef INTWINE_CONNECTED_OUTLET

#include "Power.h"
#include "spi.h"
#include "TimeMonitor.h"

// CTL specified additions:
typedef unsigned long int32u;
typedef unsigned char int8u;

unsigned int CalibrationDataOfUnit1[80] = {		//S/N:11490731
    //00230024002400250025FFFFFFFFFFFF
    0x0023, 0x0024, 0x0024, 0x0025, 0x0025, 0xFFFF, 0xFFFF, 0xFFFF,

    //00260024002400250025FFFFFFFFFFFF
    0x0026, 0x0024, 0x0024, 0x0025, 0x0025, 0xFFFF, 0xFFFF, 0xFFFF,

    //FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,

    //FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,

    //0DC1E4BFACFF87B59CC1FFFFFFFFFFFF (byte swapped)
    0xC10D, 0xBFE4, 0xFFAC, 0xB587, 0xC19C, 0xFFFF, 0xFFFF, 0xFFFF,

    //DAC67E4DC2F08F3A9E51FFFFFFFFFFFF (byte swapped)
    0xC6DA, 0x4D7E, 0xF0C2, 0x3A8F, 0x519E, 0xFFFF, 0xFFFF, 0xFFFF,

    //06C3D4F4F7FA4F0E13FDFFFFFFFFFFFF (byte swapped)
    0xC306, 0xF4D4, 0xFAF7, 0x0E4F, 0xFD13, 0xFFFF, 0xFFFF, 0xFFFF,

    //39BFE14C2ED2281A230EFFFFFFFFFFFF (byte swapped)
    0xBF39, 0x4CE1, 0xD22E, 0x1A28, 0x0E23, 0xFFFF, 0xFFFF, 0xFFFF,

    //E2BBA149C6D2472FC93DFFFFFFFFFFFF (byte swapped)f
    0xBBE2, 0x49A1, 0xD2C6, 0x2F47, 0x3DC9, 0xFFFF, 0xFFFF, 0xFFFF,

    //000600001702FB0FEF88FFFFFFFFFFFF (some byte swapped)
    0x0006, 0x0000, 0x0217, 0x0FFB, 0xEF88, 0xFFFF, 0xFFFF, 0xFFFF,
};

extern int8u power_regsdata[];
extern int32u powerdata;
unsigned char flag; //must be used

void COR_AP_init_lowcontrol(void);
int32u COR_AL_dealpower(void);
int8u COR_Read7753(int8u addr,int32u *pnt,int8u cnt);

int iPowerInitialized = 0;

void PowerAPI_Initialize(void)
{
    // CTL specified code additions:
    COR_AP_init_lowcontrol();
    iPowerInitialized = 1;
}

DWORD dwSampledPower = 0, dwSampledEnergy = 0;
void  PowerAPI_Sample(void)
{
    int32u power_read;

    if(iPowerInitialized)
    {
        COR_Read7753(0x03,&powerdata,0x03);//Read 3 bytes active energy from RAENERGY register
        power_read = COR_AL_dealpower();
    
        dwSampledPower = power_read;
        dwSampledEnergy += power_read;
    }
}

void PowerAPI_Read(DWORD *inst_power, DWORD *accum_energy)
{
    // Steps to reading power
    // 1) Return energy if valid pointer
    // 2) Return power if valid pointer

    // 1)
    TimeMonitorDisable();
    if(accum_energy != NULL)
    {
        *accum_energy = dwSampledEnergy;
        dwSampledEnergy = 0;
    }

    // 2)
    if(inst_power != NULL)
    {
        *inst_power = dwSampledPower;
    }
    TimeMonitorEnable();
}

void PowerAPI_SetupCalibrationData(unsigned short *calInfo)
{
    unsigned short *short_ptr;
    int i;

    short_ptr = calInfo;
    for(i=0; i<77; i++)
    {
        CalibrationDataOfUnit1[i] = *short_ptr++;
    }
}

#define POWER_TEMP_ARRAY_SIZE  10
void PowerAPI_TestApp(void)
{
    int array_count = 0;
    DWORD powerTEMParray[POWER_TEMP_ARRAY_SIZE], powerTEMP, energyTEMP, TotalEnergy=0;
    DWORD powerTEMPtime[POWER_TEMP_ARRAY_SIZE];
    DWORD energyTEMParray[POWER_TEMP_ARRAY_SIZE];
    DWORD cur_time = 0, last_time = 0;

    spiEnable(SPI_CHANNEL3);
    PowerAPI_Initialize();

	while(1)
    {
        cur_time = TickGet();
        if((cur_time - last_time) >= TIME_MONITOR_TICK_SECOND*1)
        {
            last_time=cur_time;

            PowerAPI_Read(&powerTEMP, &energyTEMP);
            TotalEnergy += energyTEMP;

            powerTEMParray[array_count] = powerTEMP;
            energyTEMParray[array_count] = energyTEMP;
            powerTEMPtime[array_count] = cur_time;
            array_count++;
            if(array_count > (POWER_TEMP_ARRAY_SIZE-1))
            {
                array_count = 0;
            }
        }
    }
}

#endif
