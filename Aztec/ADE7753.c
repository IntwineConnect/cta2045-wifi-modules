#include "HardwareProfile.h"
#include "TCPIP Stack/TCPIP.h"
#include "eeprom.h"

#ifdef INTWINE_CONNECTED_OUTLET
#define DUMP_CAL_DATA_ON_STARTUP
#define USE_CALIBRATION
#define USE_WGAIN

#define CAL_DATA_BYTE_SWAP
#ifdef CAL_DATA_BYTE_SWAP
unsigned short cal_data_byte_swap(unsigned short val);
#endif

#include "ADE7753.h"
#include "spi.h"
#include "Power.h"

int spi_channel = 3;

//#define TEST_WITH_DC_VOLTAGE

PwrChipReg const PwrChip[42] =
{
  { 0, 24, "WAVEFORM"   },   // Reg01 (index 0)
  { 0, 24, "AENERGY"    },   // Reg02
  { 0, 24, "RAENERGY"   },   // Reg03
  { 0, 24, "LAENERGY"   },   // Reg04
  { 0, 24, "VAENERGY"   },   // Reg05
  { 0, 24, "RVAENERGY"  },   // Reg06
  { 0, 24, "LVAENERGY"  },   // Reg07
  { 0, 24, "LVARENERDY" },   // Reg08
  { 1, 16, "MODE"       },   // Reg09
  { 1, 16, "IRQEN"      },   // Reg0A
  { 0, 16, "STATUS"     },   // Reg0B
  { 0, 16, "RSTATUS"    },   // Reg0C
  { 1,  8, "CH1OS"      },   // Reg0D
  { 1,  8, "CH2OS"      },   // Reg0E
  { 1,  8, "GAIN"       },   // Reg0F
  { 1,  6, "PHCAL"      },   // Reg10
  { 1, 16, "APHOS"      },   // Reg11
  { 1, 12, "WGAIN"      },   // Reg12
  { 1,  8, "WDIV"       },   // Reg13
  { 1, 12, "CFNUM"      },   // Reg14
  { 1, 12, "CFDEN"      },   // Reg15
  { 0, 24, "IRMS"       },   // Reg16
  { 0, 24, "VRMS"       },   // Reg17
  { 1, 12, "IRMSOS"     },   // Reg18
  { 1, 12, "VRMSOS"     },   // Reg19
  { 1, 12, "VAGAIN"     },   // Reg2A
  { 1,  8, "VADIV"      },   // Reg2B
  { 1, 16, "LINECYC"    },   // Reg2C
  { 1, 12, "ZXTOUT"     },   // Reg2D
  { 1,  8, "SAGCYC"     },   // Reg2E
  { 1,  8, "SAGLVL"     },   // Reg2F
  { 1,  8, "IPKLVL"     },   // Reg20
  { 1,  8, "VPKLVL"     },   // Reg21
  { 0, 24, "IPEAK"      },   // Reg22
  { 0, 24, "RSTIPEAK"   },   // Reg23
  { 0, 24, "VPEAK"      },   // Reg24
  { 0, 24, "RSTVPEAK"   },   // Reg25
  { 0,  8, "TEMP"       },   // Reg26
  { 0, 16, "PERIOD"     },   // Reg27

  // ... 28 thru 3C not used

  { 1,  8, "TMODE"      },   // REG3D (index 39)
  { 0,  6, "CHKSUM"     },   // REG3E
  { 0,  8, "DIEREV"     }    // REG3F

};

#define NO_BYTE_SWAP    0
#define BYTE_SWAP       1
int ReadMeterCalConstants(ADE7753_CalInfo_t* pCalInfo, int byte_swap);
int StoreMeterCalConstants(ADE7753_CalInfo_t* pCalInfo);


/*************************************************
 * This function returns a pointer to the powerchip register data for a given command
 *************************************************/
PwrChipReg *PwrChipCmd(BYTE ChipReg){
  if((ChipReg>0) && (ChipReg <0x28))
  {
    return(PwrChipReg *)&PwrChip[ChipReg-1];
  }
  if((ChipReg>0x3c) && (ChipReg <0x40))
  {
    return(PwrChipReg *)&PwrChip[ChipReg-22];
  }
  return 0;
}

void ReadCurrentAndVoltage(DWORD *Current, DWORD *Voltage)
{
  BYTE addr;
  DWORD rststatus;
  PwrChipReg *Register;
  DWORD read_current, read_voltage;

  // Current and Voltage are to be read at a Zero Crossing to minimize noise
  // From the ADE7753 pdf file:
  // The zero-crossing detection also drives the ZX flag in the interrupt status register.
  // The ZX flag is set to Logic 0 on the rising and falling edge of the voltage waveform.
  // It stays low until the status register is read with reset.
  // An active low in the IRQ output also appears if the corresponding bit in the interrupt 
  // enable register is set to Logic 1.
  // Steps to wait on the Zero Crossing
  // 1) Clear the interrupt register (read 0x0c)
  // 2) Wait for ZX bit to go
  // 3) Read current
  // 4) Read voltage

  // 1) Reset the ZX flag to 1 by reading the status register
  addr = 0x0c;
  Register= PwrChipCmd(addr);
  rststatus = ReadMeterChip(addr, Register->numbits);

  // 2) This will lockup if there are no zero crossings
#ifndef TEST_WITH_DC_VOLTAGE
  do
  {
    //read and reset the IRQ register
    rststatus = ReadMeterChip(addr, Register->numbits);
  } while(rststatus & 0x10);
#endif

  // 3)
  addr = 0x16;
  Register= PwrChipCmd(addr);
  read_current = ReadMeterChip(addr, Register->numbits);
  *Current = read_current;

  // 4)
  addr = 0x17;
  Register= PwrChipCmd(addr);
  read_voltage = ReadMeterChip(addr, Register->numbits);
  *Voltage = read_voltage;

  addr++;
}

void initMeterChip(void)
{
  ADE7753_CalInfo_t calInfo;
  int read_cal_constants_return = 0;

  // Steps for setting up meter chip
  // 1) Configure and Enable the SPI on channel 2
  // 2) Perform any calibration adjustments here??
  // 3) Set the mode (channel 2)
  // 4) Setup chip interrupt functions

  // 1)
  spiEnable(spi_channel);                  // Initialize and Enable
  read_cal_constants_return = ReadMeterCalConstants(&calInfo, BYTE_SWAP);

  //do calibration values here
  //get items from EEPROM
  if(read_cal_constants_return == 0)
  {
      putrsUART("\r\nCalibration constants read successfully.");
      //only overwrite the default values for the device is we read cal values from EEPROM

      //memcpy(&CalibrationDataOfUnit1, &calInfo, sizeof(ADE7753_CalInfo_t));
      // memcpy does not work because ADE7753_CalInfo_t is short based, not int based
      // copy shorts to ints
      PowerAPI_SetupCalibrationData((unsigned short *)&calInfo);
  }
  else
  {
      putrsUART("\r\nCalibration constant READ ERROR !");
  }
  
  //Init the register of ADE7753.
  PowerAPI_Initialize();

}

DWORD ReadStatusRegister()
{
	BYTE addr = 0x0b;
	return ReadMeterChip(addr, PwrChipCmd(addr)->numbits);	
}	

//calibration functions
int StoreMeterCalConstants(ADE7753_CalInfo_t* pCalInfo)
{
	int eeWriteAddress = EEPROM_ADDRESS_ICO_CAL_INFO;
	int calStructSize, i;
	UINT8 *pCalInfo8 = (UINT8*)pCalInfo;
	DWORD ico_magic_num = MAGIC_NUMBER_ICO_CAL_INFO;
	
	calStructSize = sizeof(ADE7753_CalInfo_t);
	
	//erase block first
//	WREN();  // Must be done before every write operation to program or erase.  (Not just once.)
//	Wait_Busy();
//	WREN_Check();
//	Block_Erase_4K(eeWriteAddress); 
//	Wait_Busy();
	SPIFlashEraseSector(eeWriteAddress);
	
	//set "MAGIC" number
	SPIFlashBeginWrite(eeWriteAddress);
	SPIFlashWriteArray((BYTE*) &ico_magic_num, 4);
	//Write Cal Info
	SPIFlashWriteArray((BYTE*) &pCalInfo, sizeof(ADE7753_CalInfo_t));

	return 0;
//	//set "MAGIC" number
//	Full_DWord_Program(eeWriteAddress, MAGIC_NUMBER_ICO_CAL_INFO);
//	eeWriteAddress+=4;
//	
//	//write byte by byte to eeprom
//	//replace with block auto-increment write?
//	for (i =0; i < calStructSize; i++)
//	{
//		Full_Byte_Program(eeWriteAddress++, pCalInfo8[i]);
//	}
	
}

// Read calibration constants from EEPROM
int ReadMeterCalConstants(ADE7753_CalInfo_t* pCalInfo, int byte_swap)
{
	int calStructSize, i;
	int eeReadAddress = EEPROM_ADDRESS_ICO_CAL_INFO;
	UINT8 *pCalInfo8 = (UINT8*)pCalInfo;
    int gain_count, j;
    DWORD magic = 0;
	
	//verify magic number
//	int magic = DWord_Read(eeReadAddress);
    SPIFlashReadArray(eeReadAddress, (BYTE*)&magic, 4);
	eeReadAddress+=4;
	
	if (magic != MAGIC_NUMBER_ICO_CAL_INFO)
	{
		//not seeing valid data in EEPROM
		return -1;
	}

    SPIFlashReadArray(eeReadAddress, (BYTE*) pCalInfo8, sizeof(ADE7753_CalInfo_t));
//	
//	calStructSize = sizeof(ADE7753_CalInfo_t);	
//	for (i =0; i < calStructSize; i++)
//	{
//		//in the store function, everything is written byte by byte. Reading the
//		//same way to avoid any endian issues.		
//		pCalInfo8[i] = Read(eeReadAddress++);
//		ClearWDT();
//	}
	
#ifdef CAL_DATA_BYTE_SWAP

    if(byte_swap == BYTE_SWAP)
    {
        // The following are more than 8 bits and need byte-swapped
        // APOS
        // WGAIN
        // CFNUM
        // CFDEN
        for(j=0; j<5; j++)
        {
            for(gain_count=0; gain_count<5; gain_count++)
            {
                pCalInfo->APOS[j][gain_count] = cal_data_byte_swap(pCalInfo->APOS[j][gain_count]);
            }
        }
    
        pCalInfo->CFNUM = cal_data_byte_swap(pCalInfo->CFNUM);
        pCalInfo->CFDEN = cal_data_byte_swap(pCalInfo->CFDEN);
        pCalInfo->WGAIN = cal_data_byte_swap(pCalInfo->WGAIN);
    }

#endif

#ifdef DUMP_CAL_DATA_ON_STARTUP

    char buf[100], buf2[20];
    
    putrsUART("\r\nCalibration values:");
    sprintf(buf, "\r\nCH1OS = ");
    for(gain_count=0; gain_count<5; gain_count++)
    {
        sprintf(buf2, "0x%04x ", pCalInfo->CH1OS[gain_count]);
        strcat(buf, buf2);
    }
    putsUART(buf);

    sprintf(buf, "\r\nCH2OS = ");
    for(gain_count=0; gain_count<5; gain_count++)
    {
        sprintf(buf2, "0x%04x ", pCalInfo->CH2OS[gain_count]);
        strcat(buf, buf2);
    }
    putsUART(buf);

    sprintf(buf, "\r\nCH1IRMSOS = ");
    for(gain_count=0; gain_count<5; gain_count++)
    {
        sprintf(buf2, "0x%04x ", pCalInfo->CH1IRMSOS[gain_count]);
        strcat(buf, buf2);
    }
    putsUART(buf);

    sprintf(buf, "\r\nCH2IRMSOS = ");
    for(gain_count=0; gain_count<5; gain_count++)
    {
        sprintf(buf2, "0x%04x ", pCalInfo->CH2IRMSOS[gain_count]);
        strcat(buf, buf2);
    }
    putsUART(buf);

    for(j=0; j<5; j++)
    {
        sprintf(buf, "\r\nAPOS[%d] = ", j);
        for(gain_count=0; gain_count<5; gain_count++)
        {
            sprintf(buf2, "0x%04x ", pCalInfo->APOS[j][gain_count]);
            strcat(buf, buf2);
        }
        putsUART(buf);
    }

    sprintf(buf, "\r\nPHCAL = 0x%04x", pCalInfo->PHCAL);
    putsUART(buf);
    sprintf(buf, "\r\nCFNUM = 0x%04x", pCalInfo->CFNUM);
    putsUART(buf);
    sprintf(buf, "\r\nCFDEN = 0x%04x", pCalInfo->CFDEN);
    putsUART(buf);
    sprintf(buf, "\r\nWGAIN = 0x%04x", pCalInfo->WGAIN);
    putsUART(buf);
    sprintf(buf, "\r\n");
    putsUART(buf);
#endif

    return 0;
}

int StoreCalDataFromString(const char * pDataString)
{
	int i, stringLength, value;
	stringLength = strlen(pDataString);
	char tempBuf[5];
	ADE7753_CalInfo_t calInfo;
	UINT16 *pCalInfo16 = (UINT16*)(&calInfo);
	
	if (stringLength != (sizeof(ADE7753_CalInfo_t) * 2))
	{
		//the length of the string needs to match the structure we are storing it in
		return 1;
	}
	
	//atoi?
	memset(tempBuf, 0, sizeof(tempBuf));
	for (i = 0; i < stringLength; i+=4)	//4 hex characters per 16 bit value
	{
		memcpy(tempBuf, pDataString, 4);
		//look at it 16 bits at a time
		sscanf(tempBuf, "%x", &value);
		
		*pCalInfo16 = value;
		pCalInfo16++;
		
		pDataString+=4;
	}
	StoreMeterCalConstants(&calInfo);
	
    //verification... readback the string from nv
    ADE7753_CalInfo_t vCalInfo;
    int result;
    
    ReadMeterCalConstants(&vCalInfo, NO_BYTE_SWAP);
    result = memcmp((void*)(&vCalInfo), (void*)(&calInfo), sizeof(ADE7753_CalInfo_t));
    if (result != 0)
    {
        putsUART("Error storing Meter Calibration constants\r\n");
		return 1;
    }

	return 0;
}

#ifdef CAL_DATA_BYTE_SWAP
unsigned short cal_data_byte_swap(unsigned short val)
{
    unsigned char b0, b1;
    unsigned short retval;

    b0 = (val & 0x00FF);
    b1 = (val & 0xFF00) >> 8;
    retval = (b0 << 8) | b1;
    return(retval);
}
#endif

#endif
