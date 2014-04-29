/**
 * EPRI_Queue.c
 * 
 * Intwine Connect 11/22/2011
 * Written by Joe Logan, Osmisys LLC
 * 
 * This file handles queueing of outgoing packets for the EPRI OpenADR demo.
 */

#include "GCMSupport.h"

#include <uart.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UARTiAPI.h"
//#include <P24FJ256GB106.h>
//#include "232Comm.h"
//#include "TCPIP Stack/UART.h"

// the compiler flipped out when I attempted to do this properly.
// not wasting any more time here. this is ugly but I don't care.
char tx_buffer1[20] = "";
char tx_buffer2[20] = "";
char tx_buffer3[20] = "";
char tx_buffer4[20] = "";
char tx_buffer5[20] = "";
char tx_buffer6[20] = "";

int buffer1len = 0;
int buffer2len = 0;
int buffer3len = 0;
int buffer4len = 0;
int buffer5len = 0;
int buffer6len = 0;

int buffer_ptr = 1; // pointer variable to next item to be transmitted.


// initialize buffer
void txbuffer_initialize()
{
    // currently this function is unused...

}

// add an item to the next spot in the queue
void queue_up(unsigned char * thestring, int len)
{
     //putsUART_EPRI("queue_up called\r\n", 17);
     int counter = 0;
     // we can't use c string functions since we need to represent null characters

     switch (buffer_ptr)
     {
         case 1:
             for (counter = 0; counter < len; counter++)
             {
                 tx_buffer1[counter] = thestring[counter];
                 buffer1len = len;
             }
             break;
         case 2:
             for (counter = 0; counter < len; counter++)
             {
                 tx_buffer2[counter] = thestring[counter];
                 buffer2len = len;
             }
             break;
         case 3:
             for (counter = 0; counter < len; counter++)
             {
                 tx_buffer3[counter] = thestring[counter];
                 buffer3len = len;
             }
             break;
         case 4:
             for (counter = 0; counter < len; counter++)
             {
                 tx_buffer4[counter] = thestring[counter];
                 buffer4len = len;
             }
             break;
         case 5:
             for (counter = 0; counter < len; counter++)
             {
                 tx_buffer5[counter] = thestring[counter];
                 buffer5len = len;
             }
             break;
         case 6:
             for (counter = 0; counter < len; counter++)
             {
                 tx_buffer6[counter] = thestring[counter];
                 buffer6len = len;
             }
             break;
     }
  }

// TimeMonitor function that transmits the head of the queue
// and increments the pointer variable.
void EPRI_TimeMonitor_Callback()
{

    LATDSET = BIT_14;   // enable RS-485 TX output
    UARTiWriteString("12345678", UART2);
    LATDCLR = BIT_14;
    
    return;

     switch (buffer_ptr) 
     {
         case 1:
             UARTiWriteBinaryData((unsigned char *)tx_buffer1, buffer1len, UART2);
             buffer1len = 0;
             tx_buffer1[0] = '\0';
             break;
         case 2:
             UARTiWriteBinaryData((unsigned char *)tx_buffer2, buffer2len, UART2);
             buffer2len = 0;
             tx_buffer2[0] = '\0';
             break;
         case 3:
             UARTiWriteBinaryData((unsigned char *)tx_buffer3, buffer3len, UART2);
             buffer3len = 0;
             tx_buffer3[0] = '\0';
             break;
         case 4:
             UARTiWriteBinaryData((unsigned char *)tx_buffer4, buffer4len, UART2);
             buffer4len = 0;
             tx_buffer4[0] = '\0';
             break;
         case 5:
             UARTiWriteBinaryData((unsigned char *)tx_buffer5, buffer5len, UART2);
             buffer5len = 0;
             tx_buffer5[0] = '\0';
             break;
         case 6:
             UARTiWriteBinaryData((unsigned char *)tx_buffer6, buffer6len, UART2);
             buffer6len = 0;
             tx_buffer6[0] = '\0';
             break;
     }

//     while(!U2STAbits.TRMT);
//     
//
//    _LATE4 = 0;
//    _LATG6 = 0;

     buffer_ptr++;
     if (buffer_ptr >= 7)
     {
         buffer_ptr = 1;
     }
}





