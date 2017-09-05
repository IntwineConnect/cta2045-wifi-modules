/*
* Methods for the INTi private interface.
*/
#ifndef INT_I_API_H
#define INT_I_API_H

void INTiRegisterUART2RxCallbackFunction(void (*HandlerFunction)(void));
void INTiRegisterSPI3RxCallbackFunction(void (*HandlerFunction)(void));
void INTiRegisterSPI3TxCallbackFunction(void (*HandlerFunction)(void));

#endif
