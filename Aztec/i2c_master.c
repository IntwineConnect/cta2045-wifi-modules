/******************************************************************************
 * FileName:        I2C_master.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *********************************************************************
 * File Description: I2C Peripheral Lib Demo
 *
 * Change History:
 * Name        Date            Changes
 * A Rafiq     2/7/07          Initial Version
 * JM          2/7/09          Updated
 *
 * $Id: i2c_master.c 9558 2008-07-04 10:08:49Z deshmukha $
 *
 * Demo Code Description:
 * This demo code excersize the i2c channel 1 by interfacing
 * to 24LC256 serial eeprom.  The code writes to a location
 * and then verify the contents of the eeprom.  I2C Channel 1
 * is hooked up to 24LC256.  Address lines of the eeprom are
 * all tied to Vss.
 ********************************************************************/
#include <plib.h>
#include "HardwareProfile.h"
#include "i2c_master.h"

#include <peripheral/i2c.h>
#include "I2CPrivate.h"

// ****************************************************************************
// ****************************************************************************
// Local Support Routines
// ****************************************************************************
// ****************************************************************************


I2C_RESULT I2CSendByteJK ( I2C_MODULE id, BYTE data )
{
	INT32 sendstat;
	int i;
	I2C_REGISTERS * const i2cRegisters = i2cBase[id];

    // Send the byte
	IdleI2C2();
    i2cRegisters->I2CxTRN = data;

    // Check for collisions

	for (i = 0; i < 100; i++) Nop();
	sendstat = i2cRegisters->I2CxSTAT;
	sendstat = i2cRegisters->I2CxSTAT & _I2CSTAT_BCL_MASK;
	
	if( i2cRegisters->I2CxSTAT & (_I2CSTAT_BCL_MASK|_I2CSTAT_IWCOL_MASK) )
    {
		return(I2C_MASTER_BUS_COLLISION); 
    }
	else
    {
		return(I2C_SUCCESS);
    }
}
/*******************************************************************************
  Function:
    BOOL InitI2C( I2C_MODULE i2c_bus )

  Summary:
    Initialize the selected I2C Module.

  Description:
    Initialize the selected I2C Module.

  Precondition:


  Parameters:
    i2c_bus
    
  Returns:
    TRUE    - If successful
    FALSE   - If bus clock frequency can't be met

  *****************************************************************************/

BOOL InitI2C( I2C_MODULE I2C_Bus, UINT32 I2C_Clock_Freq )
{
    UINT32              actualClock;

    // Set the I2C baudrate
    actualClock = I2CSetFrequency(I2C_Bus, GetPeripheralClock(), I2C_Clock_Freq);
    if ( abs(actualClock-I2C_Clock_Freq) > I2C_Clock_Freq/10 )
    {
        printf("Error: I2C1 clock frequency (%u) error exceeds 10%%.\n", (unsigned)actualClock);
        return FALSE;
    }
    else 
    {
        // Enable the I2C bus
        I2CEnable(I2C_Bus, TRUE);
        return TRUE;
    }

}


/****************************************************************************
  Function:
    BOOL ReadI2C( I2C_MODULE I2C_Bus, BYTE Slv_Address, BYTE Reg_Address, BYTE NumBytes )

  Description:
    This routine reads 1 byte from I2C bus. Slave Address is 7 bit.
    Register address is 1 byte.

  Precondition:
    None

  Parameters:
    I2C_Bus - I2C bus port
    Slv_Address - I2C Slave Address of device
    Reg_Address - Register address for read

  Returns:
    TRUE    - If successful
    FALSE   - Any error

  Remarks:
    None
  ***************************************************************************/
BOOL ReadI2C( I2C_MODULE I2C_Bus, BYTE Slv_Address, BYTE Reg_Address, BYTE *Data, int NumBytes ) 
{
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;
    BOOL                Success = TRUE;
    BYTE                i2cbyte;

    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, Slv_Address, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = Reg_Address;
    DataSz = 2;
    
    // Start the transfer to read the Slave.
    if( !StartTransfer(I2C_Bus, FALSE) )
    {
        Success = FALSE;
    }
    
    // Address the Slave.
    Index = 0;
    while( Success & (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(I2C_Bus, i2cData[Index]))
        {
            Index++;
        }
        else
        {
            Success = FALSE;
        }

        // Verify that the byte was acknowledged
        if(!I2CByteWasAcknowledged(I2C_Bus))
        {
            printf("Error: Sent byte was not acknowledged\n");
            Success = FALSE;
        }
    }

    // Restart and send the Slave's internal address to switch to a read transfer
    if(Success)
    {
        // Send a Repeated Started condition
        if( !StartTransfer(I2C_Bus, TRUE) )
        {
            Success = FALSE;
        }

        // Transmit the address with the READ bit set
        I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, Slv_Address, I2C_READ);
        if (TransmitOneByte(I2C_Bus, SlaveAddress.byte))
        {
            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged(I2C_Bus))
            {
                printf("Error: Sent byte was not acknowledged\n");
                Success = FALSE;
            }
        }
        else
        {
            Success = FALSE;
        }
    }

    // Read the data from the desired address
    Index = 0;
    DataSz = NumBytes;
    while( Success & (Index < DataSz) )
    {
        if(I2CReceiverEnable(I2C_Bus, TRUE) == I2C_RECEIVE_OVERFLOW)
        {
            printf("Error: I2C Receive Overflow\n");
            Success = FALSE;
        }
        else
        {
            while(!I2CReceivedDataIsAvailable(I2C_Bus));
            i2cbyte = I2CGetByte(I2C_Bus);
			*Data++ = i2cbyte;
			Index++;
        }

    }

    // End the transfer (stop here if an error occured)
    StopTransfer(I2C_Bus);

    return Success;

}


/****************************************************************************
  Function:
    BOOL WriteI2CByte( I2C_MODULE I2C_Bus, BYTE Slv_Address, BYTE Reg_Address, BYTE Data)

  Description:
    This routine writes 1 byte to the I2C bus. Slave Address is 7 bit.
    Register address is 1 byte.

  Precondition:
    None

  Parameters:
    I2C_Bus - I2C bus port
    Slv_Address - I2C Slave Address of device
    Reg_Address - Register address for write
    Data - byte to write

  Returns:
    TRUE    - If successful
    FALSE   - If a collision occured during Start signaling

  Remarks:
    None
  ***************************************************************************/
BOOL WriteI2CByte( I2C_MODULE I2C_Bus, BYTE Slv_Address, BYTE Reg_Address, BYTE Data) 
{
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;
    BOOL                Success = TRUE;

    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, Slv_Address, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = Reg_Address;
    i2cData[2] = Data;
    DataSz = 3;

    // Start the transfer to read the Slave.
    IdleI2C2(); 
    if( !StartTransfer(I2C_Bus, FALSE) )
    {
        Success = FALSE;
    }
    
    // Address the Slave, send register address and data.
    Index = 0;
    while( Success & (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(I2C_Bus, i2cData[Index]))
        {
            // Advance to the next byte
            Index++;
        }
        else
        {
            Success = FALSE;
        }

        // Verify that the byte was acknowledged
        if(!I2CByteWasAcknowledged(I2C_Bus))
        {
            printf("Error: Sent byte was not acknowledged\n");
            Success = FALSE;
        }
    }

    // End the transfer (stop here if an error occured)
    StopTransfer(I2C_Bus);

    return Success;

}


/*******************************************************************************
  Function:
    BOOL StartTransfer( I2C_MODULE I2C_Bus, BOOL restart )

  Summary:
    Starts (or restarts) a transfer to/from the Slave.

  Description:
    This routine starts (or restarts) a transfer to/from the Slave, waiting (in
    a blocking loop) until the start (or re-start) condition has completed.

  Precondition:
    The I2C module must have been initialized.

  Parameters:
    restart - If FALSE, send a "Start" condition
            - If TRUE, send a "Restart" condition
    
  Returns:
    TRUE    - If successful
    FALSE   - If a collision occured during Start signaling
    
  Example:
    <code>
    StartTransfer(FALSE);
    </code>

  Remarks:
    This is a blocking routine that waits for the bus to be idle and the Start
    (or Restart) signal to complete.
  *****************************************************************************/

BOOL StartTransfer( I2C_MODULE I2C_Bus, BOOL restart )
{
    I2C_STATUS  status;
    I2C_RESULT  result;

    // Send the Start (or Restart) signal
    if(restart)
    {
        IdleI2C2(); 
        I2CRepeatStart(I2C_Bus);
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        IdleI2C2(); 
        while( !I2CBusIsIdle(I2C_Bus) );
        if((result = (I2CStart(I2C_Bus))) != I2C_SUCCESS)
        {
            printf("Error: Bus collision during transfer Start\n");
            return FALSE;
        }
    }

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(I2C_Bus);

    } while ( !(status & I2C_START) );

    return TRUE;
}


/*******************************************************************************
  Function:
    BOOL TransmitOneByte( I2C_MODULE I2C_Bus, UINT8 data )

  Summary:
    This transmits one byte to the Slave.

  Description:
    This transmits one byte to the Slave, and reports errors for any bus
    collisions.

  Precondition:
    The transfer must have been previously started.

  Parameters:
    data    - Data byte to transmit

  Returns:
    TRUE    - Data was sent successfully
    FALSE   - A bus collision occured

  Example:
    <code>
    TransmitOneByte(0xAA);
    </code>

  Remarks:
    This is a blocking routine that waits for the transmission to complete.
  *****************************************************************************/

BOOL TransmitOneByte( I2C_MODULE I2C_Bus, UINT8 data )
{

    BOOL Success = FALSE;
    INT32 status;

    IdleI2C2();
    // Wait for the transmitter to be ready
    while(!I2CTransmitterIsReady(I2C_Bus))
        IdleI2C2();

    // Transmit the byte
    if(I2CSendByteJK(I2C_Bus, data) == I2C_MASTER_BUS_COLLISION)
    {
        printf("Error: I2C Master Bus Collision\n");
        return FALSE;
    }

    status = I2C2STAT;

    // Wait for the transmission to finish
    while(!Success)
    {
        Success = I2CTransmissionHasCompleted(I2C_Bus);
    }
    status = I2C2STAT;

    return TRUE;

}


/*******************************************************************************
  Function:
    void StopTransfer( I2C_MODULE I2C_Bus, void )

  Summary:
    Stops a transfer to/from the Slave.

  Description:
    This routine Stops a transfer to/from the Slave, waiting (in a 
    blocking loop) until the Stop condition has completed.

  Precondition:
    The I2C module must have been initialized & a transfer started.

  Parameters:
    None.
    
  Returns:
    None.
    
  Example:
    <code>
    StopTransfer();
    </code>

  Remarks:
    This is a blocking routine that waits for the Stop signal to complete.
  *****************************************************************************/

void StopTransfer( I2C_MODULE I2C_Bus )
{
    I2C_STATUS  status;

    // Send the Stop signal
    I2CStop(I2C_Bus);

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(I2C_Bus);

    } while ( !(status & I2C_STOP) );
}



