/*
*   INTi is a driver interface for the INTERRUPT peripheral
*/

#include <plib.h>
#include <stdio.h>
#include <peripheral\int_5xx_6xx_7xx.h>
#include <peripheral\uart.h>
#include "Assert.h"
#include "Metrics.h"
#include "INTiAPI.h"

void (*UART5RxCallbackFunction)(void) = NULL;
void (*UART2RxCallbackFunction)(void) = NULL;
void (*SPI3RxCallbackFunction)(void) = NULL;

/*
* Function:         void INTiRegisterUART5RxCallbackFunction(void (*HandlerFunction)(void))
* Description:      Register an external handler for UART5InterruptServiceRoutine
* Creation Date:    11/04/2011
* Author:           Bill Barnett
*/
void INTiRegisterUART5RxCallbackFunction(void (*HandlerFunction)(void))
{
    UART5RxCallbackFunction = HandlerFunction;
}

/*
* Function:         void INTiRegisterUART2RxCallbackFunction(void (*HandlerFunction)(void))
* Description:      Register an external handler for UART2InterruptServiceRoutine
* Creation Date:    11/04/2011
* Author:           Bill Barnett
*/
void INTiRegisterUART2RxCallbackFunction(void (*HandlerFunction)(void))
{
    UART2RxCallbackFunction = HandlerFunction;
}
/*
* Function:         void INTiRegisterSPI3CallbackFunction(void (*HandlerFunction)(void))
* Description:      Register an external handler for SPI3InterruptServiceRoutine
* Creation Date:    11/04/2011
* Author:           Bill Barnett
*/
void INTiRegisterSPI3CallbackFunction(void (*HandlerFunction)(void))
{
    SPI3RxCallbackFunction = HandlerFunction;
}

/*
* Function:         void UART5InterruptServiceRoutine(void)
* Description:      ISR for UART5InterruptServiceRoutine. NOTE: the ipl must match the priority set in the INTSetVectorPriority function
* Creation Date:    10/19/2011
* Author:           Robert Scaccia
*/

void __ISR(_UART_5_VECTOR, ipl6) UART5InterruptServiceRoutine(void)
{

    if(INTGetFlag(INT_U5RX))
    {
        if(UART5RxCallbackFunction != NULL)
        {
            (*UART5RxCallbackFunction)();
        }
        INTClearFlag(INT_U5RX);
    }

}

/*
* Function:         void UART2InterruptServiceRoutine(void)
* Description:      ISR for UART2InterruptServiceRoutine. NOTE: the ipl must match the priority set in the INTSetVectorPriority function
* Creation Date:    10/19/2011
* Author:           Robert Scaccia
*/
void __ISR(_UART_2_VECTOR, ipl5) UART2InterruptServiceRoutine(void)
{
    if(INTGetFlag(INT_U2RX))
    {
        if(UART2RxCallbackFunction != NULL)
        {
            (*UART2RxCallbackFunction)();
        }
        INTClearFlag(INT_U2RX);
    }

}

/*
* Function:         void SPI3InterruptServiceRoutine(void)
* Description:      ISR for SPI3InterruptServiceRoutine. NOTE: the ipl must match the priority set in the INTSetVectorPriority function
* Creation Date:    11/09/2011
* Author:           Bill Barnett
*/
void __ISR(_SPI_3_VECTOR, ipl4) SPI3InterruptServiceRoutine(void)
{
    int count = 1;
    if(INTGetFlag(INT_SPI3RX))
    {
        if(SPI3RxCallbackFunction != NULL)
        {
            (*SPI3RxCallbackFunction)();
        }
        INTClearFlag(INT_SPI3RX);
    }
    if(INTGetFlag(INT_SPI3E))
    {
        INTClearFlag(INT_SPI3E);
        count = 2;

        // Ruff!, mwn - add recovery for this
//        ASRT_INT(0, "SPI3 error", WATCHDOG_SPI3_ERROR);
        while(1);
    }
    if(INTGetFlag(INT_SPI3TX))
    {
        INTClearFlag(INT_SPI3TX);
        count = 3;
    }
}

void SPI3EnableInterrupts(void)
{
    // Steps to enabling SPI interrupts
    // 1) Ensure they are disabled
    // 2) Clear all interrupt flags
    // 3) Setup priorities
    // 4) Re-enable - just want Rx and Error

    SPI3DisableInterrupts();

    // Enable Rx and Error interrupts, no Tx
    INTClearFlag(INT_SPI3E);
    INTClearFlag(INT_SPI3TX);
    INTClearFlag(INT_SPI3RX);

    INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_ENABLED);
    INTEnable(INT_SOURCE_SPI_ERROR(SPI_CHANNEL3), INT_ENABLED);
}

void SPI3DisableInterrupts(void)
{
    // Enable Rx and Error interrupts, no Tx
    INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_DISABLED);
    INTEnable(INT_SOURCE_SPI_ERROR(SPI_CHANNEL3), INT_DISABLED);
}