/*
*   INTi is a driver interface for the INTERRUPT peripheral
*/

#include <plib.h>
#include <stdio.h>
#include <peripheral/int_5xx_6xx_7xx.h>
#include <peripheral/uart.h>
#include "Assert.h"
#include "INTiAPI.h"
#include "HardwareProfile.h"

void (*UART1RxCallbackFunction)(void) = NULL;
void (*SPI3RxCallbackFunction)(void) = NULL;
void (*SPI3TxCallbackFunction)(void) = NULL;

/*
* Function:         void INTiRegister1RxCallbackFunction(void (*HandlerFunction)(void))
* Description:      Register an external handler for 1InterruptServiceRoutine
* Creation Date:    11/04/2011
* Author:           Bill Barnett
*/
void INTiRegisterUART1RxCallbackFunction(void (*HandlerFunction)(void))
{
    UART1RxCallbackFunction = HandlerFunction;
}
/*
* Function:         void INTiRegisterSPI3RxCallbackFunction(void (*HandlerFunction)(void))
* Description:      Register an external handler for SPI3InterruptServiceRoutine
* Creation Date:    11/04/2011
* Author:           Bill Barnett
*/
void INTiRegisterSPI3RxCallbackFunction(void (*HandlerFunction)(void))
{
    SPI3RxCallbackFunction = HandlerFunction;
}

/*
* Function:         void INTiRegisterSPI3TxCallbackFunction(void (*HandlerFunction)(void))
* Description:      Register an external handler for SPI3InterruptServiceRoutine
* Creation Date:    11/04/2011
* Author:           Bill Barnett
*/
void INTiRegisterSPI3TxCallbackFunction(void (*HandlerFunction)(void))
{
    SPI3TxCallbackFunction = HandlerFunction;
}

/*
* Function:         void 1InterruptServiceRoutine(void)
* Description:      ISR for UART1InterruptServiceRoutine. NOTE: the ipl must match the priority set in the INTSetVectorPriority function
* Creation Date:    10/19/2011
* Author:           Robert Scaccia
*/
#if defined(AC_CEA2045)
void __ISR(_UART_1_VECTOR, ipl5) UART1InterruptServiceRoutine(void)
{
    if(INTGetFlag(INT_U1RX))
    {
        if(UART1RxCallbackFunction != NULL)
        {
            (*UART1RxCallbackFunction)();
        }
        INTClearFlag(INT_U1RX);
    }

}
#endif

/*
* Function:         void SPI3InterruptServiceRoutine(void)
* Description:      ISR for SPI3InterruptServiceRoutine. NOTE: the ipl must match the priority set in the INTSetVectorPriority function
* Creation Date:    11/09/2011
* Author:           Bill Barnett
*/
#if defined(DC_CEA2045)
void __ISR(_SPI_3_VECTOR, ipl4) SPI3InterruptServiceRoutine(void)
{
    if(INTGetFlag(INT_SPI3) && INTGetEnable(INT_SPI3)){
        int count = 0;
        if(INTGetFlag(INT_SPI3RX) && INTGetEnable(INT_SPI3RX))
        {
            if(SPI3RxCallbackFunction != NULL)
            {
                (*SPI3RxCallbackFunction)();
            }
            INTClearFlag(INT_SPI3RX);
            count |= 1;
        }
        if(INTGetFlag(INT_SPI3E) && INTGetEnable(INT_SPI3E))
        {
            // TODO escalate?
            if(SPI3STAT >> 8 & 0x1 == 0x1){
                SPI3STATCLR = 1 << 8;
                SPI3BUF=0xFF;
            }
            if(SPI3STAT >> 6 & 0x1 == 0x1){
                SPI3BUF;
                SPI3STATCLR = 1 << 6;
            }

            INTClearFlag(INT_SPI3E);
            count |= 2;
        }
        if(INTGetFlag(INT_SPI3TX) && INTGetEnable(INT_SPI3TX))
        {
            if(SPI3TxCallbackFunction != NULL)
            {
                (*SPI3TxCallbackFunction)();
            }
            INTClearFlag(INT_SPI3TX);
            count |= 3;
        }
    } else {
        // Wat
        Nop();
    }
    INTClearFlag(INT_SPI3); //maybE? 
}
#endif