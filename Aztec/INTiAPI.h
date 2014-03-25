/*
* Methods for the INTi private interface.
*/
#ifndef INT_I_API_H
#define INT_I_API_H

void INTiRegisterUART5RxCallbackFunction(void (*HandlerFunction)(void));
void INTiRegisterUART2RxCallbackFunction(void (*HandlerFunction)(void));
void INTiRegisterSPI3CallbackFunction(void (*HandlerFunction)(void));

void SPI3DisableInterrupts(void);
void SPI3EnableInterrupts(void);

void INTiReportErrors(void);

#endif
