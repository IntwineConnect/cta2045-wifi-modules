/*
*	UARTi is a driver interface for the UART ports.
*/

#include "HardwareProfile.h"
#include <stdlib.h>
#include <plib.h>
#include <peripheral\uart.h>
#include <peripheral\int.h>
#include <peripheral\system.h>
#include <peripheral\int_5xx_6xx_7xx.h>
#include <proc\p32mx575f256l.h>
#include "UARTiAPI.h"

/*

1. Select the desired basic configuration from the UART_CONFIGURATION Enumeration and set the selected options using the UARTConfigure Function. 
2. Set the desired FIFO trigger levels for receive and transmit from the UART_FIFO_MODE Enumeration and set the selected options using the UARTSetFifoMode Function. 
3. Set the line control configuration from the UART_LINE_CONTROL_MODE Enumeration and set the selected options using the UARTSetLineControl Function. 
4. Set the desired data rate using UARTSetDataRate Function. 
5. Enable the mode for operation using UART_ENABLE_MODE Enumeration and set the selected options using the UARTEnable Function. 

*/
UINT UARTiConfigure(UART_MODULE iUART, UINT32 uiDataRate)
{
	UINT	uiActualDataRate;
    	
	//step 1 - set the configuration, not available to be set on UART5
	if(iUART != UART5) UARTConfigure(iUART, UART_ENABLE_PINS_TX_RX_ONLY); 	

    //step 2 - set the fifo interrupt settings
//	UARTSetFifoMode(iUART, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
	UARTSetFifoMode(iUART, UART_INTERRUPT_ON_RX_NOT_EMPTY);
	//step 3 - set the data mode
	UARTSetLineControl(iUART, UART_DATA_SIZE_8_BITS|UART_PARITY_NONE|UART_STOP_BITS_1);
	//step 4 - set the baud (data) rate
    uiActualDataRate = UARTSetDataRate(iUART,  GetPeripheralClock(), uiDataRate);

	//step 5 - enable uart
	UARTEnable(iUART, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX)); 
	return(uiActualDataRate);
}

void UARTiDisable(UART_MODULE iUART)
{
	UARTEnable(iUART, UART_DISABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX)); 

}

/*
*	Function Name:	void UARTiWriteString(void)
*	Description:	Writes a full string out the selected uart.
*	Initiator: 		Robert Scaccia
*	Date: 			10/19/2011
*/
void UARTiWriteString(const char *ccpString, UART_MODULE iUART)
{
    static int ready_high_water = 0, completed_high_water = 0;
    int ready_count = 0, completed_count = 0;
    while(*ccpString != '\0')
    {
        while(!UARTTransmitterIsReady(iUART)) ready_count++;
        UARTSendDataByte(iUART, *ccpString);
        ccpString++;
        while(!UARTTransmissionHasCompleted(iUART)) completed_count++;
    }

    if(ready_count     > ready_high_water)     ready_high_water     = ready_count;
    if(completed_count > completed_high_water) completed_high_water = completed_count;
}

/*
*	Function Name:	void UARTiWriteBinaryData(void)
*	Description:	Writes a block of binary data to he selected uart.
*	Initiator: 		Keith Suhoza
*	Date: 			01/23/2012
*/
void UARTiWriteBinaryData(unsigned char *data_block, int length, UART_MODULE iUART)
{
  while( length)
  {
      while(!UARTTransmitterIsReady(iUART));
      UARTSendDataByte(iUART, *data_block++);
      while(!UARTTransmissionHasCompleted(iUART));
      length--;
  }
}