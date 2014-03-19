/******************************************************************************
 *********************************************************************
 * FileName:        i2c_master.h
 * Dependencies:
 * Processor:       PIC32MX575F512L
 * Compiler:        Microchip  XC32 Compiler
 * Company:         Intwine Connect, LLC
 *
 
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Jim Kapcio                      01/24/2014    Draft
 ********************************************************************/

#include <plib.h>

BOOL InitI2C( I2C_MODULE I2C_Bus, UINT32 I2C_Clock_Freq);
BOOL StartTransfer( I2C_MODULE I2C_Bus, BOOL restart );
BOOL TransmitOneByte( I2C_MODULE I2C_Bus, UINT8 data );
void StopTransfer( I2C_MODULE I2C_Bus );
BOOL ReadI2C( I2C_MODULE I2C_Bus, BYTE Slv_Address, BYTE Reg_Address, BYTE *Data, int NumBytes );
BOOL WriteI2CByte( I2C_MODULE I2C_Bus, BYTE Slv_Address, BYTE Reg_Address, BYTE Data );
