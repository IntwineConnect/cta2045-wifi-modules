/*********************************************************************
 *
 *  Generic TCP Client Example Application
 *  Ported to GainSpan
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bill Barnett         01/19/2012  Original
 ********************************************************************/

#include "IntwineModule.h"

// Non-library support
    #include "GCMSupport.h"

// put this after all defines have been made
    #include "GCMExterns.h"

//port #include "TCPIP_Stack/TCPIP.h"
//port #include "TCPIP_Stack/UART.h"
#include "UpgradeClient.h"
#include "app.h"
#include "main.h"
#include "eeprom.h"
#include "Metrics.h"
//port #include "wificonfig.h"
#include "CBRunTime.h"
#include "Assert.h"

#include <stdlib.h>

#if defined (CT80)
	#ifdef R4
		#define DEV_TYPE 	 4
	#else
		#define DEV_TYPE 	 1
	#endif
#else
	#ifdef R4
		#define DEV_TYPE 	 5
	#else
		#define DEV_TYPE 	 2
	#endif
#endif

//Max size of recv buffer
#define RECVLEN 256
#define CHECKSUM_LEN (4)
#define MAGIC_LEN (4)
#define MAGIC_NUMBER_LEN (MAGIC_LEN + CHECKSUM_LEN)
#define TCP_TIMEOUT (15) /* 15 second TCP timeout should be ok in all cases */

// Defines the server to be accessed for this application

//static BYTE UpgradeServerName[] =	"www.thecloudbus.com";

// real ip
static BYTE UpgradeServerName[] =	"173.204.194.85";

// DownloadMaster ip (laptop at Sunscape)
//static BYTE UpgradeServerName[] =	"192.168.23.12";

// Defines the port to be accessed for this application
static WORD UpgradeServerPort = 10100;          // real dfw port
//static WORD UpgradeServerPort = 10050;        // test port

extern CB_VAR_UINT8  upgradeFlag;   // Upgrade Flag

#define PROCESS_LET_OTHERS_RUN    0
#define PROCESS_KEEP_DFW_RUNNING  1

#define FIRMWARE_CHECK_SHORT_TIME   30
#define FIRMWARE_CHECK_LONG_TIME    3600
long FirmwareCheckTimeout=FIRMWARE_CHECK_SHORT_TIME;    // keep signed, in case it underflows

enum _UpgradeTCPEnum UpdateTCPState=UTSM_HOME;
DWORD ResetTimer=0;
DWORD TCP_Timeout1=0;
TCP_SOCKET	UpdateSocket = INVALID_SOCKET;
int UpgradeTimeout=0;
#define BASE 65521
/* Adler32 checksum routine from rsync/zlib/adler32.c */
unsigned long update_adler32(unsigned long adler, BYTE nextbyte)
{
	unsigned long sum2;
	 /* split Adler-32 into component sums */
    sum2 = (adler >> 16) & 0xffff;
    adler &= 0xffff;

    /* in case user likes doing a byte at a time, keep it fast (we always do it a byte at a time) */
    adler += nextbyte;
    if (adler >= BASE)
        adler -= BASE;
    sum2 += adler;
    if (sum2 >= BASE)
        sum2 -= BASE;
    return adler | (sum2 << 16);
}

//Timeout checker code
void Check_Timeout(void) 
{
	if(TimeMonitorTickGet()-TCP_Timeout1 > 20*TIME_MONITOR_TICK_SECOND) 
	{ 
        IncrementMetrics(METRICS_DFW_SERVER_TIMEOUT_FAIL);
        SetMetrics(METRICS_DFW_LAST_TRY_FAILED, 1);

        /* Close the socket so it can be used by other modules */ 
        FirmwareCheckTimeout=FIRMWARE_CHECK_LONG_TIME;
//mwn
//        FirmwareCheckTimeout=FIRMWARE_CHECK_SHORT_TIME;
		TCPDisconnect(UpdateSocket); 
		UpdateSocket = INVALID_SOCKET; 
		UARTiDebug("\r\nTimeout", DEBUG_UPGRADE_CLIENT);
		UpgradeTimeout++;
		UpdateTCPState=UTSM_HOME; 
        CBRetryTime = CB_NORMAL_RETRY_TIME;
	}
}


// Just delete the first sector which contains the info necessary to stimulate a bootload upon a 
// restart.  This deletes the checksum, etc.
void Zero_EEPROM(void)
{
	//NAK - Zero the checksum to the EEPROM
	UARTiDebug("\r\nZero our EEPROM", DEBUG_UPGRADE_CLIENT);
	WREN();	
	WRSR(0x00);  // Despite having WP high, this appears like it needs to be cleared despite the docs indicating otherwise.
	Wait_Busy();
	WREN();  // Must be done before every write operation to program or erase.  (Not just once.)
	Wait_Busy();
	WREN_Check();
	Block_Erase_64K(0x000000);  
	Wait_Busy();
}

/*****************************************************************************
Function:
void GenericTCPClient(void)

Summary:
Implements a simple HTTP client (over TCP).

Description:
This function implements a simple HTTP client, which operates over TCP.  
The function is called periodically by the stack, and waits for BUTTON1 
to be pressed.  When the button is pressed, the application opens a TCP
connection to an Internet search engine, performs a search for the word
"Microchip" on "microchip.com", and prints the resulting HTML page to
the UART.

This example can be used as a model for many TCP and HTTP client
applications.

Precondition:
TCP is initialized.

Parameters:
None

Returns:
None
 ***************************************************************************/
int UpgradeTCPClient(void)
{
	WORD w;
	WORD recvd;
	DWORD i=0;
	BYTE vBuffer[RECVLEN];
	BYTE sBuffer[12];
	static BYTE IsFirstProcessResponse=1; //The first time we process the response we need to do magic-number processing and size-checking.
	static DWORD FirmwareAddress; //Initialize in a bit
	static DWORD Checksum;
	static DWORD RunningChecksum;
	static DWORD FWLEN=0;
	static DWORD RunningFWLEN=0;
	unsigned char eedata;
	unsigned eraseCounter=0;
	unsigned long erase=0x000000;
    char buf[100];
    DWORD time_left, cur_time;
    static BYTE happy_led_modified = 0;

    switch(UpdateTCPState)
	{
		case UTSM_HOME:
            // A bit of setup
            CBRetryTime = CB_NORMAL_RETRY_TIME;         // set time to normal, can be upped below
            IsFirstProcessResponse=1;
			FirmwareAddress = 0x08; //Skip the checksum
			//don't do this if Wifi not connected

            if(GetMetrics(METRICS_DFW_LAST_TRY_FAILED))
            {
                FirmwareCheckTimeout=FIRMWARE_CHECK_LONG_TIME;
                SetMetrics(METRICS_DFW_LAST_TRY_FAILED, 0);
            }

//            full_packet_timeout = (TIME_MONITOR_TICK_SECOND + (TIME_MONITOR_TICK_SECOND / 2));    // Used in UTSM_PROCESS_RESPONSE

            if(FirmwareCheckTimeout > 0)
			{
                cur_time = TimeMonitorTickGet();
                if((cur_time - TCP_Timeout1) > TIME_MONITOR_TICK_SECOND)
				{ 
					FirmwareCheckTimeout -= ((cur_time - TCP_Timeout1) / TIME_MONITOR_TICK_SECOND);
					TCP_Timeout1 = TimeMonitorTickGet(); //Restart the timer
				}
				return PROCESS_LET_OTHERS_RUN;       // let other code run
			}

            UARTiDebug("\r\nDFW Check in progress ....\r\n", DEBUG_UPGRADE_CLIENT);

            // Connect a socket to the remote TCP server
			UpdateSocket = TCPOpen((DWORD)&UpgradeServerName[0], TCP_OPEN_RAM_HOST, UpgradeServerPort, TCP_PURPOSE_GENERIC_TCP_CLIENT);

            // Abort operation if no TCP socket of type TCP_PURPOSE_GENERIC_TCP_CLIENT is available
			// If this ever happens, you need to go add one to TCPIPConfig.h
			if(UpdateSocket == INVALID_SOCKET)
			{
				UARTiDebug("\r\nUpdate Invalid Socket....\r\n", DEBUG_UPGRADE_CLIENT);
				TCPClose(UpdateSocket);
//mwn - determine retry strategy
//                FirmwareCheckTimeout = FIRMWARE_CHECK_SHORT_TIME;
                FirmwareCheckTimeout = FIRMWARE_CHECK_LONG_TIME;
				return PROCESS_LET_OTHERS_RUN;       // let other code run
				break;
			}

            // #if defined(STACK_USE_UART)
			UARTiDebug("\r\nUpdate Connecting using Microchip TCP API...", DEBUG_UPGRADE_CLIENT);
			// #endif
			UARTiDebug("\r\nIn Update TSM_SOCKET_OBTAINED...", DEBUG_UPGRADE_CLIENT);
            sprintf(buf, "TIME_MONITOR_TICK_SECOND = %ld\r\n", (unsigned long) TIME_MONITOR_TICK_SECOND);
            UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
			TCP_Timeout1 = TimeMonitorTickGet();
            CBRetryTime = CB_NORMAL_RETRY_TIME*10;
			UpdateTCPState++;
            return PROCESS_LET_OTHERS_RUN;       // let other code run
			break;

		case UTSM_SOCKET_OBTAINED:
			// Wait for the remote server to accept our connection request
			if(!TCPIsConnected(UpdateSocket))
			{
				// Time out if too much time is spent in this state
				Check_Timeout();
				// UARTiDebug("\r\nCould Not Connect...");
                FirmwareCheckTimeout = FIRMWARE_CHECK_LONG_TIME;
				return PROCESS_LET_OTHERS_RUN;       // let other code run
				break;
			}

// always able to send with Serial To Wifi
/*             // Make certain the socket can be written to                     */
/*             if(TCPIsPutReady(UpdateSocket) < 125u)                           */
/*             {                                                                */
/*                 UARTiDebug("\r\nCannot write yet...", DEBUG_UPGRADE_CLIENT); */
/*                 Check_Timeout();                                             */
/*                 break;                                                       */
/*             }                                                                */
			TCP_Timeout1 = TimeMonitorTickGet();
			UpdateTCPState++;
			break;	  	  

			// Need to obtain socket first??
//port #if defined(STACK_USE_SSL_CLIENT)
//port 		case UTSM_SSL_START:
//port 			
//port 			if(TCPStartSSLClient(UpdateSocket,NULL))
//port 			{
//port 				UARTiDebug("\n\rStartSSLClient:");
//port 				TCP_Timeout1 = TimeMonitorTickGet();
//port 				UpdateTCPState++; 
//port 				// Need to put timeout here....
//port 			}
//port 			break;
//port 		case UTSM_SSL_HANDSHAKE: 
//port 			Check_Timeout();	
//port 			if(!TCPSSLIsHandshaking(UpdateSocket))
//port 			{ 
//port 				UARTiDebug("\n\rStartSSLHandshaking:");
//port 				UpdateTCPState++; 
//port 				TCP_Timeout1 = TimeMonitorTickGet();
//port 			}
//port 			break;
//port #endif

		case UTSM_WRITE_SOCKET:
			// Transmit the FW version to the server
			Check_Timeout();	
			sBuffer[0] = 0xAA;
			sBuffer[1] = APP_FW_MAJOR;
			sBuffer[2] = APP_FW_MINOR;
			sBuffer[3] = APP_FW_BUILD;
			sBuffer[4] = DEV_TYPE;
			sBuffer[5] = AppConfig.MyMACAddr.v[0];
			sBuffer[6] = AppConfig.MyMACAddr.v[1];
			sBuffer[7] = AppConfig.MyMACAddr.v[2];
			sBuffer[8] = AppConfig.MyMACAddr.v[3];
			sBuffer[9] = AppConfig.MyMACAddr.v[4];
			sBuffer[10] = AppConfig.MyMACAddr.v[5];
			TCPPutROMArray(UpdateSocket, sBuffer, 11); //Send only 11 bytes to server
			UARTiDebug("\r\nFlushing...", DEBUG_UPGRADE_CLIENT);
			// Send the packet
			TCPFlushBinary(UpdateSocket, 11);

			TCP_Timeout1 = TimeMonitorTickGet();
			UpdateTCPState++;
			break;

		case UTSM_PROCESS_RESPONSE:
			// Check to see if the remote node has disconnected from us or sent us any data
			// If fw data is available, write it to the EEPROM
			Check_Timeout();
//mwn
/*             if(!TCPIsConnected(UpdateSocket))                                                */
/*             {                                                                                */
/*                 if(FirmwareAddress > 0x1000)                                                 */
/*                 {                                                                            */
/*                     UpdateTCPState=UTSM_FINALIZE_FW;                                         */
/*                 }                                                                            */
/*                 else                                                                         */
/*                 {                                                                            */
/*                     UpdateTCPState=UTSM_NAK;                                                 */
/*                 }                                                                            */
/*                 // Do not break;  We might still have data in the TCP RX FIFO waiting for us */
/*             }                                                                                */

			// Get count of RX bytes waiting
			w = TCPIsGetReady(UpdateSocket);
			if((w == 1 || w==0) && ((TimeMonitorTickGet()-TCP_Timeout1) < (10*TIME_MONITOR_TICK_SECOND))) //We need to wait to see if we get more bytes
			{
				//Data timeout of 10 seconds
				break;
			}
			else if(w==0)
			{
				//Either we ran out of FW to receive or something happened and they quit sending
				//either way, go to next stage.
				if(FirmwareAddress > 0x1000)
				{
					UpdateTCPState=UTSM_FINALIZE_FW;
				}
				else 
				{
                    sprintf(buf, "\r\n10 Second Timeout hit\r\n");
                    UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
                    IncrementMetrics(METRICS_DFW_SERVER_TIMEOUT_FAIL);
					UpdateTCPState=UTSM_NAK;
				}
				break;
			}

            if(w>0)
			{
                sprintf(buf, "\r\nTCPIsGetReady bytes = %d\r\n", w);
                UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
				//Refresh the data & TCP timeouts every time we get data.
				TCP_Timeout1=TimeMonitorTickGet();
			}	
			if(w==1 && IsFirstProcessResponse) //If we get 1 byte later it might be the end
			{
				//We failed to get new FW, so end. Do not ack or nak.
				UpdateTCPState = UTSM_DONE;
				//If there was an error, loop back around faster
				recvd = TCPGetArray(UpdateSocket, vBuffer, w>RECVLEN? RECVLEN:w);
				if (vBuffer[0] != '\0')
				{
					//Cut our loopback time to 60 seconds - NOT USED
					FirmwareCheckTimeout = FIRMWARE_CHECK_SHORT_TIME;
				}
				break; //Go directly to done.
			}
			if(IsFirstProcessResponse) //Otherwise, we got more than 1 byte and this is our first process.
			{
				if(w < 4) 
                {
					//We don't have enough to process yet - we really want to process the whole magic section at once.
					//TODO FIXME how do we handle it if we keep looping here because the server sent us, say, 4 bytes?
					break; 
				}	
			}
            while(w && ((TimeMonitorTickGet()-TCP_Timeout1) < ((TIME_MONITOR_TICK_SECOND/2)*3))) //full_packet_timeout)) //
			{
				recvd = TCPGetArray(UpdateSocket, vBuffer, w>RECVLEN? RECVLEN:w); //Trinary operator: If w is bigger than the max we can read, read the max, otherwise read w bytes.
                if(IsFirstProcessResponse == 1)
                {
                    UARTiDebug("\r\nIsFirstProcessResponse = 1", DEBUG_UPGRADE_CLIENT);
                    //mwn - should this area of code only allow 4 bytes
                    if(recvd < 4)
					{
						//This should never happen because we knew w was greater than our magic number len. Bail out!
						UpdateTCPState=UTSM_NAK;
						UARTiDebug("\r\nCould not read expected magic number bytes!", DEBUG_UPGRADE_CLIENT);
						break;
					}
					//Check the magic numbers
					if((vBuffer[0] == 0) &&	(vBuffer[1] == 0xA5) &&	(vBuffer[2] == 0x5A) &&	(vBuffer[3] == 0xBF))
					{
						UARTiDebug("\r\nGot magic number!", DEBUG_UPGRADE_CLIENT);
					}
					else if((vBuffer[0] == 0) && (vBuffer[1] == 0x22) && (vBuffer[2] == 0xAB) && (vBuffer[3] == 0xC3))
					{
						UARTiDebug("\r\nNo New Firmware!", DEBUG_UPGRADE_CLIENT);
						//Bail out
						upgradeFlag.status=0;  // Have most recent firmware - no need to keep trying
						UpdateTCPState=UTSM_DONE;
						break;
					}
					else
					{
                        IncrementMetrics(METRICS_DFW_BAD_MAGIC_NUMBER_FAIL);
						UARTiDebug("\r\nMagic number did not match expected!", DEBUG_UPGRADE_CLIENT);
						//Bail out
						UpdateTCPState=UTSM_NAK;
						break;
					}

					// Set FirstProcessResult to 2 so we process the checksum info at the start of the next packet
                    IsFirstProcessResponse = 2;
                    w -= recvd;
					//Magic checks out OK! Erase the whole chip, we have a valid FW.
					WREN();	
					WRSR(0x00);  // Despite having WP high, this appears like it needs to be cleared despite the docs indicating otherwise.
					Wait_Busy();
					/*	WREN();  // Must be done before every write operation to program or erase.  (Not just once.)
					Wait_Busy();
					WREN_Check();
					Chip_Erase();
					Wait_Busy(); */
					
					// Erase the pages for the new download.
					for(eraseCounter=0;eraseCounter<7;eraseCounter++)
					{
                        WREN();	
                        WRSR(0x00);  // Despite having WP high, this appears like it needs to be cleared despite the docs indicating otherwise.
                        Wait_Busy();
                        WREN();  // Must be done before every write operation to program or erase.  (Not just once.)
                        Wait_Busy();
                        WREN_Check();
						Block_Erase_64K(erase); 
						Wait_Busy();
						erase=erase+0x10000;
					}

					//And back to the top so we don't screw anything up
					continue;
				}
                else if(IsFirstProcessResponse == 2)
                {
                    UARTiDebug("\r\nIsFirstProcessResponse = 2", DEBUG_UPGRADE_CLIENT);

                    //Ok, so we know the magic matches. This must be a FW. Store the checksum.
					RunningChecksum = vBuffer[0];
					Checksum  = RunningChecksum <<24;
					RunningChecksum = vBuffer[1];
					Checksum |= RunningChecksum <<16;
					RunningChecksum = vBuffer[2];
					Checksum |= RunningChecksum <<8;
					Checksum |= vBuffer[3];
					RunningChecksum = 1; //Adler routine always starts with 1.
					//Set FirstProcessResult to 0 so we don't hit this section 5again
                    IsFirstProcessResponse = 0;
                    w -= 4;
                    recvd -= 4;

                    // PIC32 has checksum as part of 2nd packet
                    vBuffer[0] = vBuffer[4];
                    vBuffer[1] = vBuffer[5];
                    vBuffer[2] = vBuffer[6];
                    vBuffer[3] = vBuffer[7];
				}

                //Ok, we're in the main read-and-process part.
				w-= recvd; //We read these out, so note that.
                EEWriteBlock_AutoAddressIncrement(FirmwareAddress, recvd, &vBuffer[0]);
                FirmwareAddress += recvd;
                for(i=0; i<recvd; i++)
                {

                    //Chip must be erased first!
                    RunningChecksum = update_adler32(RunningChecksum, vBuffer[i]);
                }
			}

            // Debug - show time remaining for each packet
            time_left = ((TIME_MONITOR_TICK_SECOND/2)*3) - (TimeMonitorTickGet() - TCP_Timeout1);
            sprintf(buf, "\r\nFW:0x%08lx TR: %ld", FirmwareAddress, time_left);
            UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);

            TCP_Timeout1 = TimeMonitorTickGet();
            TimeMonitorClearWDT();

            // modify happy light (after first 4k of firmware downloaded)
            // will go back to proper happy light upon reboot or error below
            if((happy_led_modified == 0) && (FirmwareAddress > 0x1000))
            {
                TimeMonitorChangeTimeI(0, 750);
            }
			break;

		case UTSM_FINALIZE_FW:
			UARTiDebug("\r\rFinalizing...", DEBUG_UPGRADE_CLIENT);
			// For this application, we wish to stay connected, but this state will still get entered if the remote server decides to disconnect
			Check_Timeout();
			
			//TODO FIXME do we need to store the FW length anywhere?
			FWLEN = FirmwareAddress - (CHECKSUM_LEN + MAGIC_LEN + 1); //The 1 is because we over-incremented after the last element.	
				
			//First, do the checksum checking?
			if(RunningChecksum == Checksum)
			{
				//Write the checksum to the EEPROM
				Full_Byte_Program(0x0, (Checksum & 0xFF));
				Full_Byte_Program(0x1, (Checksum>>8 & 0xFF));
				Full_Byte_Program(0x2, (Checksum>>16 & 0xFF));
				Full_Byte_Program(0x3, (Checksum>>24 & 0xFF));
				//Write the size to the EEPROM
				Full_Byte_Program(0x4, (FWLEN & 0xFF));
				Full_Byte_Program(0x5, (FWLEN>>8 & 0xFF));
				Full_Byte_Program(0x6, (FWLEN>>16 & 0xFF));
				Full_Byte_Program(0x7, (FWLEN>>24 & 0xFF));
				UpdateTCPState = UTSM_VERIFY_FW;	
				//Now, verify the checksum in the EEPROM - do a full loop test
				RunningChecksum=1;
				RunningFWLEN = 0x08; //Starting position
				FWLEN+= 0x08; //Correct FWLEN by starting position
			}
			else
			{
                sprintf(buf, "\r\nReceive checksum fail\r\n");
                UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
                IncrementMetrics(METRICS_DFW_RECEIVE_CHECKSUM_FAIL);
				Zero_EEPROM();
				UpdateTCPState = UTSM_NAK;
			}
			TCP_Timeout1=TimeMonitorTickGet();

        case UTSM_VERIFY_FW:  //  This is checked once upon entry into the EEPROM - check again as a unit.
			Check_Timeout();
			if(RunningFWLEN % 0x1000 == 0)
            {
                sprintf(buf, "\r\nVerifying 0x%08lx", RunningFWLEN);
                UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
			}
			while(RunningFWLEN <= FWLEN)
			{
				//eedata=Read_Status_Register();
				//eedata=Read_ID(0x01);
                if(RunningFWLEN < 256+6)
                {
                    eedata = Read(RunningFWLEN);
                }
                else
                {
                    eedata = Read(RunningFWLEN);
                }
				RunningChecksum = update_adler32(RunningChecksum, eedata); //Requires reading the EEPROM - slow, but better than putting it all in memory!
				//Wait_Busy();
				RunningFWLEN++;
				if(RunningFWLEN % 0x800 == 0)
				{
					TCP_Timeout1=TimeMonitorTickGet();
                    TimeMonitorClearWDT();
					return PROCESS_LET_OTHERS_RUN;		   // Break the multitasking cooperative loop to do other stuff, then come back here.
				}
			}
			if(RunningChecksum != Checksum)
			{
                sprintf(buf, "\r\nVerify checksum fail\r\n");
                UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
                IncrementMetrics(METRICS_DFW_VERIFY_CHECKSUM_FAIL);
                Zero_EEPROM();
                UpdateTCPState = UTSM_NAK;
                break;
			}
			// Now, read back to make sure we wrote the Checksum and the FWLEN correctly to the EEPROMs first 8 bits.
			RunningChecksum = Read(0x03);
			RunningChecksum = RunningChecksum <<8;
			RunningChecksum = RunningChecksum | Read(0x02);
			RunningChecksum = RunningChecksum <<8;
			RunningChecksum = RunningChecksum | Read(0x01);
			RunningChecksum = RunningChecksum <<8;
			RunningChecksum = RunningChecksum | Read(0x00);

			RunningFWLEN = Read(0x07);
			RunningFWLEN = RunningFWLEN <<8;
			RunningFWLEN = RunningFWLEN | Read(0x06);
			RunningFWLEN = RunningFWLEN <<8;
			RunningFWLEN = RunningFWLEN | Read(0x05);
			RunningFWLEN = RunningFWLEN <<8;
			RunningFWLEN = RunningFWLEN | Read(0x04);

			if((RunningChecksum != Checksum) && (RunningFWLEN != (FWLEN-0x08)))
			{
                sprintf(buf, "\r\nVerify length fail\r\n");
                UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
                IncrementMetrics(METRICS_DFW_VERIFY_LENGTH_FAIL);
                Zero_EEPROM();
                UpdateTCPState = UTSM_NAK;
                break;
			}
			UpdateTCPState = UTSM_ACK;
			TCP_Timeout1=TimeMonitorTickGet();
			UARTiDebug("Code is Verified", DEBUG_UPGRADE_CLIENT);
			break;

        case UTSM_ACK:
            IncrementMetrics(METRICS_DFW_SUCCESS_COUNT);
            UARTiDebug("\rAcking...Reboot in 10", DEBUG_UPGRADE_CLIENT);
			Check_Timeout();
			if(!TCPIsConnected(UpdateSocket))
			{
				//They don't want to talk to us? We're done with them.
				UpdateTCPState = UTSM_DONE;
				break;
			}
			// Otherwise, we're still connected. Send a response.
			sBuffer[0] = 0xFF;
			sBuffer[1] = AppConfig.MyMACAddr.v[0];
			sBuffer[2] = AppConfig.MyMACAddr.v[1];
			sBuffer[3] = AppConfig.MyMACAddr.v[2];
			sBuffer[4] = AppConfig.MyMACAddr.v[3];
			sBuffer[5] = AppConfig.MyMACAddr.v[4];
			sBuffer[6] = AppConfig.MyMACAddr.v[5];
			TCPPutROMArray(UpdateSocket, sBuffer, 7); //Send only 1 byte ACK to server
			UARTiDebug("\r\nFlushing...", DEBUG_UPGRADE_CLIENT);
			// Send the packet
			TCPFlushBinary(UpdateSocket, 7); 
			UpdateTCPState = UTSM_DONE; //We're totally finished, and everything worked.	
			TCP_Timeout1 = TimeMonitorTickGet();
			ResetTimer = TimeMonitorTickGet();
		    // Set Timer to reboot part - Reset the part and use the new firmware.
			break;

        case UTSM_NAK:

            TimeMonitorChangeTimeI(0, 250);
            SetMetrics(METRICS_DFW_LAST_TRY_FAILED, 1);

            CBRetryTime = CB_NORMAL_RETRY_TIME;
			UARTiDebug("\r\nNacking...", DEBUG_UPGRADE_CLIENT);
			Check_Timeout();
			if(!TCPIsConnected(UpdateSocket))
			{
				//They've closed the socket on us - probably why we failed to read the FW right. Just end.
				UpdateTCPState = UTSM_DONE;
				break;
			}
			sBuffer[0] = 0xCF;
			sBuffer[1] = AppConfig.MyMACAddr.v[0];
			sBuffer[2] = AppConfig.MyMACAddr.v[1];
			sBuffer[3] = AppConfig.MyMACAddr.v[2];
			sBuffer[4] = AppConfig.MyMACAddr.v[3];
			sBuffer[5] = AppConfig.MyMACAddr.v[4];
			sBuffer[6] = AppConfig.MyMACAddr.v[5];
			sBuffer[7] = APP_FW_MAJOR;
			sBuffer[8] = APP_FW_MINOR;
			sBuffer[9] = APP_FW_BUILD;
			sBuffer[10] = DEV_TYPE;
			TCPPutROMArray(UpdateSocket, sBuffer, 11); //Send only 1 byte ACK to server
			UARTiDebug("\r\nFlushing...", DEBUG_UPGRADE_CLIENT);
			// Send the packet
			TCPFlushBinary(UpdateSocket, 11);
			UpdateTCPState = UTSM_DONE; //We're totally finished, and everything worked.	
			TCP_Timeout1 = TimeMonitorTickGet();
// not used in PIC24			ResetTimer = TimeMonitorTickGet();       // force reset on errors
            UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);
            break;	  	  

        case UTSM_DONE:
            CBRetryTime = CB_NORMAL_RETRY_TIME;
			FirmwareCheckTimeout=FIRMWARE_CHECK_LONG_TIME;
//mwn test
//            FirmwareCheckTimeout=FIRMWARE_CHECK_SHORT_TIME;
            UARTiDebug(buf, DEBUG_UPGRADE_CLIENT);

            UpdateTCPState = UTSM_HOME;
			TCPDisconnect(UpdateSocket);
			UpdateSocket = INVALID_SOCKET;
			UpgradeTimeout=0;

//mwn, this is typically done back in the app loop
//#ifndef __DEBUG
            if(ResetTimer != 0)
            {
//                UARTiDebug("\r\nRebooting in 5 seconds... \r\r\n\r\n\r\n\r\n\n", DEBUG_UPGRADE_CLIENT);
                ASRT_INT(0, "\r\nRebooting in 5 seconds...", WATCHDOG_DFW_COMPLETE);
                while(1);
            }
//#endif
			break;
	}
	return PROCESS_KEEP_DFW_RUNNING;   // default to only us running
}

int isUpgradeActive(void)
{
    if(UpdateTCPState != UTSM_HOME)
    {
        return(1);
    }
    return(0);
}

void showFirmwareRev(char *userbuf)
{
    char buf[100];
    sprintf(buf, "\r\nFirmware Revision: %d.%d.%d \r\n", APP_FW_MAJOR, APP_FW_MINOR, APP_FW_BUILD);
    UARTiDebug(buf, DEBUG_ALWAYS);

    if(userbuf != NULL)
    {
        sprintf(userbuf, "FWRev:%d.%d.%d", APP_FW_MAJOR, APP_FW_MINOR, APP_FW_BUILD);
    }
}

