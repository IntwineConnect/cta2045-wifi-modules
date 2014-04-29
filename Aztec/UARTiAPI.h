/*
* Methods for the UARTi public interface.
*/
#ifndef UART_I_API_H
#define UART_I_API_H

UINT    UARTiConfigure      (UART_MODULE, UINT32);
void    UARTiDisable(UART_MODULE iUART);

#define UART4       UART1B

#if defined(_UART4)
#define OpenUART4(config1, config2, ubrg)	(U4BRG = (ubrg), U4MODE = (config1), U4STA = (config2))
#endif

#if defined(_UART4)
#define putcUART4(c)           do{while(!U4STAbits.TRMT); WriteUART4((int)(c));}while(0)
#endif

#if defined(_UART4)
#define WriteUART4(data)	U4TXREG = (data)
#endif

#if defined(_UART4)
#define ReadUART4()		(U4RXREG)
#endif

#if defined(_UART4)
#define CloseUART4()	    U4MODECLR = (1<<_U4MODE_UARTEN_POSITION);
#endif

#if defined(_UART4)
#define DataRdyUART4()		(U4STAbits.URXDA)
#endif

#if defined(_UART4)
#define BusyUART4()		(!U4STAbits.TRMT)
#endif

#if defined(_UART4)
#define UART4EnableAutoAddr(address) ((U4STA |= (1 << _U4STA_ADM_EN_POSITION) | (address << _U4STA_ADDR_POSITION) | (1 << _U4STA_ADDEN_POSITION)))
#endif

#if defined(_UART4)
#define UART4SendBreak()	(U4STASET = (1<<_U4STA_UTXBRK_POSITION), WriteUART4(0))
#endif

#endif

