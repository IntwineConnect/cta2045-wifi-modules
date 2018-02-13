 /*********************************************************************
 * Intwine Connect OpenADR client code
 * Dave Halasz, Joe Logan
 *
 *  Based on Microchip Generic TCP Client Example Application,
 * GenericTCPClient.c. Also includes Intwine Connect SEP 2.0
 * and t-stat code.
 *
 *  Microchip license info:
 * Software License Agreement
 *
 * Copyright (C) 2002-2008 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and
 *      ENC28J60.h ported to a non-Microchip device used in
 *      conjunction with a Microchip ethernet controller for the
 *      sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     8/01/06    Original
 ********************************************************************/
#define __GENERICTCPCLIENT_C

#include "HardwareProfile.h"
#include "OpenADRClient.h"
#include "TCPIP_Stack/TCPIP.h"
#include "TimeMonitor.h"
#include "EPRI_Queue.h"
#include "MCI_Common.h"
#include "BasicDR.h"

#ifdef AC_CEA2045
  #include "EPRI_UARTLayer.h"
#else
  #include "EPRI_SPILayer.h"
#endif

int commGood; // flag to indicate if communication with the outside world is working. needed for MCI comm with SGD
int commGoodOld;  // change of state indicator
int MAX_PAYLOAD_SGD = 2;        // maximum number of Bytes in payload - default is 2

//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------
MCIResponse TransmitCommGoodPacket( void );
MCIResponse TransmitCommBadPacket( void );

//-----------------------------------------------------------------------------

// Initialization for OpenADR communication
void InitOpenADRComm( void )
{
    
#ifdef AC_CEA2045
    EPRI_UART_init();
#endif
    commGood = 0;
    commGoodOld = 0;

    // Send EndShed at start-up
    DelayMs(500);
    SendEndShedCommand();
    
    // Send Maximum Payload Query at start-up - will modify MAX_PAYLOAD_SGD
    DelayMs(500);
    SendQueryMaximumPayloadLength();
    
}

// JL addition
// sends a "comm good" packet to the end device
MCIResponse TransmitCommGoodPacket()
{
    return MCISendNeutral(OutsideCommGood);    
}

// JL addition
// sends a "comm bad" packet to the end device
MCIResponse TransmitCommBadPacket()
{
    return MCISendNeutral(OutsideCommLost);    
}
