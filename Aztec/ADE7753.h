#ifdef INTWINE_CONNECTED_OUTLET

#include "GenericTypeDefs.h"

typedef struct _PwrChipReg {
    const BYTE writeable;
    const BYTE numbits;
    const char *Name;
} PwrChipReg;

//calibration
typedef enum
{
	kMeterGain1	=	0,
	kMeterGain2,
	kMeterGain4,
	kMeterGain8,
	kMeterGain16
} MeterGainValues_e;

//only 5 are valid, the rest are dead space - this makes it compatible with the
//calibration data coming from the manufacturing process
#define GAIN_COUNT	8	

typedef struct
{
	UINT16 CH1OS[GAIN_COUNT];
	UINT16 CH2OS[GAIN_COUNT];
	
	UINT16 CH1IRMSOS[GAIN_COUNT];	//not currently used
	UINT16 CH2IRMSOS[GAIN_COUNT];	//not currently used
	
	UINT16 APOS[5][GAIN_COUNT];	//[channel2 gain val][channel 1 gain val]
	
	UINT16 PHCAL;
	UINT16 CFNUM;
	UINT16 CFDEN;
	UINT16 WGAIN;
	
	UINT16 checksum;	
	
} ADE7753_CalInfo_t;


extern PwrChipReg const PwrChip[];

PwrChipReg *PwrChipCmd(BYTE ChipReg);

void initSPI(void);
void initMeterChip(void);
void ReadRms(DWORD *Current, DWORD *Voltage);
void ReadActiveEnergy(DWORD *Energy);
void printDWORDhex(DWORD Value);  
DWORD RdAccumulatedEnergy(void);
DWORD Rd_CLRAccumulatedEnergy(void);        
void WriteAPOSreg(int offset);
void DualRead(void);
void Setup_Pwr_read(void);

//void ReadRmsAndActiveEnergy(DWORD *Current, DWORD *Voltage, DWORD *Energy);
void ReadRmsAndActiveEnergy(DWORD *Current, DWORD *Voltage, long *AccumEnergy, long *InstantPower);
void ReadCurrentAndVoltage(DWORD *Current, DWORD *Voltage);
void ReadActiveAccumEnergy(long *Energy);
void ReadInstantaneousPower(long *Power);
DWORD ReadStatusRegister();


int StoreCalDataFromString(const char * pDataString);

#endif
