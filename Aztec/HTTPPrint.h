/*********************************************************************
 * FileName:          HTTPPrint.h
 * Description:       Provides callback headers and resolution for user's custom
 *                         HTTP Application.
 * Processor:         PIC32MX695F512H
 * Compiler:          Microchip  XC32 Compiler
 * 
 * This file is automatically generated by the MPFS Utility
 * ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS GENERATOR
 *
 * Software License Agreement
 *
 * Copyright (C) 2013 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
  * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product ("Device") which is 
 *      integrated into Licensee's product; or 
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
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
 *********************************************************************/

#ifndef __HTTPPRINT_H
#define __HTTPPRINT_H

#include "TCPIP_Stack/TCPIP.h"

#if defined(STACK_USE_HTTP2_SERVER)

extern HTTP_STUB httpStubs[MAX_HTTP_CONNECTIONS];
extern BYTE curHTTPID;

void HTTPPrint(DWORD callbackID);
void HTTPPrint_Demoversion(void);
void HTTPPrint_ssid(void);
void HTTPPrint_curPrivacy(void);
void HTTPPrint_wlan(void);
void HTTPPrint_ipaddr(void);
void HTTPPrint_led(WORD);
void HTTPPrint_nextSSID(void);
void HTTPPrint_prevSSID(void);
void HTTPPrint_prevWLAN(void);
void HTTPPrint_nextWLAN(void);
void HTTPPrint_scan(void);
void HTTPPrint_fwver(void);
void HTTPPrint_bssCount(void);
void HTTPPrint_valid(void);
void HTTPPrint_name(void);
void HTTPPrint_privacy(void);
void HTTPPrint_strength(void);
void HTTPPrint_btn(WORD);
void HTTPPrint_aplist(void);

void HTTPPrint(DWORD callbackID)
{
    // Index needs to correspond with HTTPPrint.idx
     
	switch(callbackID)
	{
        case 0x00000000:
			HTTPPrint_Demoversion();
			break;
        case 0x00000001:
			HTTPPrint_ssid();
			break;
        case 0x00000002:
			HTTPPrint_curPrivacy();
			break;
        case 0x00000003:
			HTTPPrint_wlan();
			break;
        case 0x00000004:
			HTTPPrint_ipaddr();
			break;
        case 0x00000005:
			HTTPPrint_led(0);
			break;
        case 0x00000006:
			HTTPPrint_nextSSID();
			break;
        case 0x00000007:
			HTTPPrint_prevSSID();
			break;
        case 0x00000008:
			HTTPPrint_prevWLAN();
			break;
        case 0x00000009:
			HTTPPrint_nextWLAN();
			break;
        case 0x0000000a:
			HTTPPrint_scan();
			break;
        case 0x0000000b:
			HTTPPrint_fwver();
			break;
        case 0x0000000c:
			HTTPPrint_bssCount();
			break;
        case 0x0000000d:
			HTTPPrint_valid();
			break;
        case 0x0000000e:
			HTTPPrint_name();
			break;
        case 0x0000000f:
			HTTPPrint_privacy();
			break;
        case 0x00000010:
			HTTPPrint_strength();
			break;
        case 0x00000011:
			HTTPPrint_led(1);
			break;
        case 0x00000012:
			HTTPPrint_led(2);
			break;
        case 0x00000013:
			HTTPPrint_btn(0);
			break;
        case 0x00000014:
			HTTPPrint_aplist();
			break;
		default:
			// Output notification for undefined values
			TCPPutROMArray(sktHTTP, (ROM BYTE*)"!DEF", 4);
	}

	return;
}

void HTTPPrint_(void)
{
	TCPPut(sktHTTP, '~');
	return;
}

#endif

#endif
