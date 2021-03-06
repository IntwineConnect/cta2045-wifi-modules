/*********************************************************************
 *
 *  Main Application Entry Point
 *
 *********************************************************************
 * FileName:        Main.c
 * Dependencies:    Compiler.h
 * Processor:       PIC32MX575F512L
 * Compiler:        Microchip  XC32 Compiler
 * Company:         Intwine Connect, LLC
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2013 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *  ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *  used in conjunction with a Microchip ethernet controller for
 *  the sole purpose of interfacing with the ethernet controller.
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
 * File Description:
 * Change History:
 * Date         Comment
 * ----------   -----------------------------------------
 * 10/22/2012    Initial release  Wifi G Demo        (MRF24WG)
 ********************************************************************/
/*
  * This macro uniquely defines this file as the main entry point.
  * There should only be one such definition in the entire project,
  * and this file must define the AppConfig variable as described below.
  */
#define THIS_IS_STACK_APPLICATION

#include "HardwareProfile.h"
#include "MainDemo.h"
#include "i2c_master.h"
#include "INTiAPI.h"
#include "UARTiAPI.h"
#include "TimeMonitor.h"
#include "MCI_Common.h"
#include "BasicDR.h"

#if defined( WF_CONSOLE )
#include "TCPIP_Stack/WFConsole.h"
#include "IperfApp.h"
#endif

#ifdef AC_CEA2045
  #include "EPRI_UARTLayer.h"
#else
  #include "EPRI_SPILayer.h"
#endif

APP_CONFIG AppConfig;

int commGood; // flag to indicate if communication with the outside world is working. needed for MCI comm with SGD
int commGoodOld;  // change of state indicator
int MAX_PAYLOAD_SGD = 0;        // maximum number of Bytes in payload - default is 2

static unsigned short wOriginalAppConfigChecksum;    // Checksum of the ROM defaults for AppConfig

UINT8 g_scan_done = 0;        // WF_PRESCAN   This will be set wheneven event scan results are ready.
UINT8 g_prescan_waiting = 1;  // WF_PRESCAN   This is used only to allow POR prescan once.

// Private helper functions.
static void InitAppConfig(void);
static void InitializeBoard(void);
extern void WF_Connect(void);
char sbuf[40];

// Used for re-directing printf when uart 2 is used by CEA2045
// Remap UART in HardwareProfile.h
void _mon_putc(char c)
{
    unsigned int ci;
    ci = (unsigned int) c;
    putcUART(ci);
}


 static enum {
      EXCEP_IRQ = 0,            // interrupt
      EXCEP_AdEL = 4,            // address error exception (load or ifetch)
      EXCEP_AdES,                // address error exception (store)
      EXCEP_IBE,                // bus error (ifetch)
      EXCEP_DBE,                // bus error (load/store)
      EXCEP_Sys,                // syscall
      EXCEP_Bp,                // breakpoint
      EXCEP_RI,                // reserved instruction
      EXCEP_CpU,                // coprocessor unusable
      EXCEP_Overflow,            // arithmetic overflow
      EXCEP_Trap,                // trap (possible divide by zero)
      EXCEP_IS1 = 16,            // implementation specfic 1
      EXCEP_CEU,                // CorExtend Unuseable
      EXCEP_C2E                // coprocessor 2
  } _excep_code;
  
  volatile static unsigned int _epc_code;
  volatile static unsigned int _excep_addr;
  
  void _general_exception_handler(void)
  {
      asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
      asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));
  
      _excep_code = (_excep_code & 0x0000007C) >> 2;
      
    Nop();
    Nop();
    while(1);       // Watchdog will reset
}

// ************************************************************
// Main application entry point.
// ************************************************************
int main(void)
{
    //test variables
    BOOL FirstTime = TRUE;
    RelayMsg retval;
    int i = 0;
    
    
    // end test variables
    
    static DWORD t = 0;
    static DWORD dwLastIP = 0;
#if defined (EZ_CONFIG_STORE)
    static DWORD ButtonPushStart = 0;
#endif
    UINT8         channelList[] = MY_DEFAULT_CHANNEL_LIST_PRESCAN;  // WF_PRESCAN
    tWFScanResult bssDesc;
#if 0
    INT8 TxPower;   // Needed to change MRF24WG transmit power.
#endif

    UINT8 Si7005ID, si7005Reg, Si7005Data[2] = {0,0};
    INT16 Temp;
    
    Temp = 0;

    // Initialize application specific hardware
    InitializeBoard();

    // Read ID register
    si7005Reg = 17;
    ReadI2C( I2C_BUS, SI7005_ADDRESS, si7005Reg, Si7005Data, 1);

    // 1ms tick for timed callback functions
    TimeMonitorInit();

    // Initialize TCP/IP stack timer
    TickInit();

    #if defined(STACK_USE_MPFS2)
    // Initialize the MPFS File System
    // Generate a WifiGDemoMPFSImg.c file using the MPFS utility (refer to Convert WebPages to MPFS.bat)
    // that gets compiled into source code and programmed into the flash of the uP.
    MPFSInit();
    #endif
    // Init Flash
    // TCP/IP Stack only inits this if MPFS_USE_SPI_FLASH is defined
    SPIFlashInit();

    // Initialize Stack and application related NV variables into AppConfig.
    InitAppConfig();

    // Initialize core stack layers (MAC, ARP, TCP, UDP) and
    // application modules (HTTP, SNMP, etc.)
    StackInit();

#if 0
    // Below is used to change MRF24WG transmit power.
    // This has been verified to be functional (Jan 2013)
    if (AppConfig.networkType == WF_SOFT_AP)
    {
        WF_TxPowerGetMax(&TxPower);
        WF_TxPowerSetMax(TxPower);
    }
#endif

    #if defined ( EZ_CONFIG_SCAN )
    // Initialize WiFi Scan State Machine NV variables
    WFInitScan();
    #endif

    // WF_PRESCAN: Pre-scan before starting up as SoftAP mode
    WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);
    WF_CASetChannelList(channelList, sizeof(channelList));

    if (WFStartScan() == WF_SUCCESS) {
       SCAN_SET_DISPLAY(SCANCXT.scanState);
       SCANCXT.displayIdx = 0;
    }

    // Needed to trigger g_scan_done
    WFRetrieveScanResult(0, &bssDesc);

    #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
    // Initialize Zeroconf Link-Local state-machine, regardless of network type.
    ZeroconfLLInitialize();
    #endif

    #if defined(STACK_USE_ZEROCONF_MDNS_SD)
    // Initialize DNS Host-Name from TCPIPConfig.h, regardless of network type.
    mDNSInitialize(MY_DEFAULT_HOST_NAME);
    mDNSServiceRegister(
            // (const char *) AppConfig.NetBIOSName,        // base name of the service. Ensure uniformity with CheckHibernate().
            (const char *) "DemoWebServer",          // base name of the service. Ensure uniformity with CheckHibernate().
            "_http._tcp.local",                      // type of the service
            80,                                      // TCP or UDP port, at which this service is available
            ((const BYTE *)"path=/index.htm"),       // TXT info
            1,                                       // auto rename the service when if needed
            NULL,                                    // no callback function
            NULL                                     // no application context
            );
    mDNSMulticastFilterRegister();
    #endif

    #if defined(WF_CONSOLE)
    // Initialize the WiFi Console App
    WFConsoleInit();
    #endif

    // Now that all items are initialized, begin the co-operative
    // multitasking loop.  This infinite loop will continuously
    // execute all stack-related tasks, as well as your own
    // application's functions.  Custom functions should be added
    // at the end of this loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must be broken
    // down into smaller pieces so that other tasks can have CPU time.
    LED1_OFF()
    LED2_OFF()
    while(1)
    {
        ClearWDT();
              
#ifdef DC_CEA2045
        SPI_Driver_Task();  
#endif
#ifdef AC_CEA2045        
        EPRI_UART_init();
#endif        
        if(FirstTime == TRUE)
        {
            LED0_INV()
        }        
         
         if (AppConfig.networkType == WF_SOFT_AP || AppConfig.networkType == WF_INFRASTRUCTURE) {
            if (g_scan_done) {
                if (g_prescan_waiting) {
                     SCANCXT.displayIdx = 0;
                     while (IS_SCAN_STATE_DISPLAY(SCANCXT.scanState)) {
                         WFDisplayScanMgr();
                     }

                     #if defined(WF_CS_TRIS)
                     WF_Connect();
                     #endif
                     g_scan_done = 0;
                     g_prescan_waiting = 0;
                }
            }
         }         

        #if defined (EZ_CONFIG_STORE)
        // Hold SW0 for 4 seconds to reset to defaults.
        if (SW0_IO == 0u) {  // Button is pressed
            if (ButtonPushStart == 0)  //Just pressed
                ButtonPushStart = TickGet();
            else
                if(TickGet() - ButtonPushStart > 4*TICK_SECOND) {
                    LEDS_ON();                           
                    RestoreWifiConfig();
                }
        }
        else
        {
            ButtonPushStart = 0; //Button release reset the clock
        }

        if (AppConfig.saveSecurityInfo)
        {
            // set true by WF_ProcessEvent after connecting to a new network
            // get the security info, and if required, push the PSK to EEPROM
            if ((AppConfig.SecurityMode == WF_SECURITY_WPA_WITH_PASS_PHRASE) ||
                (AppConfig.SecurityMode == WF_SECURITY_WPA2_WITH_PASS_PHRASE) ||
                (AppConfig.SecurityMode == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE))
            {
                // only need to save when doing passphrase
                tWFCPElements profile;
                UINT8 connState;
                UINT8 connID;
                WF_CMGetConnectionState(&connState, &connID);
                WF_CPGetElements(connID, &profile);

                memcpy((char*)AppConfig.SecurityKey, (char*)profile.securityKey, 32);
                AppConfig.SecurityMode--; // the calc psk is exactly one below for each passphrase option
                AppConfig.SecurityKeyLength = 32;

                SaveAppConfig(&AppConfig);
            }

            AppConfig.saveSecurityInfo = FALSE;
        }
        #endif // EZ_CONFIG_STORE

        // Blink LED0 twice per sec when unconfigured, once per sec after config
        if((TickGet() - t >= TICK_SECOND/(4ul - (CFGCXT.isWifiDoneConfigure*2ul))))
        {
            t = TickGet();
            LED0_INV();
        }
        
        if(AppConfig.networkType == WF_INFRASTRUCTURE)  {
            if(AppConfig.connected == FALSE)
            { // not connected
                WF_Connect();
/*                UINT8 connState;
                UINT8 connID;
                WF_CMGetConnectionState(&connState, &connID);
                WF_CPSetNetworkType(connID, WF_INFRASTRUCTURE);
                WF_CMConnect(connID);
 */
            } else { 
               // connected
            }
        }
       
        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

        // This task invokes each of the core stack application tasks
        StackApplications();

        #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
        ZeroconfLLProcess();
        #endif

        #if defined(STACK_USE_ZEROCONF_MDNS_SD)
        mDNSProcess();
        #endif

        // Process application specific tasks here.
        // Any custom modules or processing you need to do should
        // go here.
        #if defined(WF_CONSOLE)
        WFConsoleProcess();
        WFConsoleProcessEpilogue();
        #endif

        // If the local IP address has changed (ex: due to DHCP lease change)
        // write the new IP address to the LCD display, UART, and Announce
        // service
        if(dwLastIP != AppConfig.MyIPAddr.Val)
        {
            dwLastIP = AppConfig.MyIPAddr.Val;
            DisplayIPValue(AppConfig.MyIPAddr);

            #if defined(STACK_USE_ANNOUNCE)
            AnnounceIP();
            #endif

            #if defined(STACK_USE_ZEROCONF_MDNS_SD)
            mDNSFillHostRecord();
            #endif
        }
        
        if(FirstTime == TRUE) {           
            commGood = 0;
            commGoodOld = 0;

            // Send EndShed at start-up
            SendEndShedCommand();           
                                           
            FirstTime = FALSE;
        }
        
        if(MAX_PAYLOAD_SGD < 2) {
            
            if(i < 200) {
                DelayMs(2);
                i++;
            }
            else {
                MAX_PAYLOAD_SGD = 2;
            
                // Send Maximum Payload Query at start-up - will modify MAX_PAYLOAD_SGD
                SendQueryMaximumPayloadLength();                   
            }
        }

    }
}

/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.  It is a generic initialization
    routine for many of the Microchip development boards, using definitions
    in HardwareProfile.h to determine specific initialization.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void InitializeBoard(void)
{

    // Si7005 Temp/Humidity on I2C2
    SI7005_TRIS = 0;
    SI7005_IO = 1;      // Pulse Si7005 CS inactive per data sheet.
    SI7005_IO = 0;
    SI7005_IO = 1;      // Si7005 CS inactive. Power down
    InitI2C( I2C_BUS, I2C_CLOCK_FREQ );


    // Enable optimal performance
    SYSTEMConfigPerformance(GetSystemClock());

    // Use 1:1 CPU Core:Peripheral clocks
    mOSCSetPBDIV(OSC_PB_DIV_1);

    // Disable JTAG port so we get our I/O pins back, but first
    // wait 50ms so if you want to reprogram the part with
    // JTAG, you'll still have a tiny window before JTAG goes away.
    // The PIC32 Starter Kit debuggers use JTAG and therefore must not
    // disable JTAG.
    DelayMs(50);
    DDPCONbits.JTAGEN = 0;

    // LEDs
    LEDS_OFF();
    LED0_TRIS = 0;
    LED1_TRIS = 0;
    LED2_TRIS = 0;

#ifdef DC_CEA2045
    //SPI ATTN
    SPI_ATTN_INACTIVE
#endif
    
    // Push Button
    SW0_TRIS = 1;

    // Flash Control
    TRISBCLR = BIT_4;   // HOLD#
    LATBSET = BIT_4;

    TRISBCLR = BIT_9;   // WP#
    LATBSET = BIT_9;

    AD1PCFG = 0xFFFF;   // all PORTB = Digital (as opposed to analog)

    // Shutdown Change Notice?? Can't really find good documentation on this
    CNEN = 0x00000000;
    CNCON = 0x00000000;

#ifdef AC_CEA2045
    UARTiConfigure(UART1, 19200);
#endif
    UARTiConfigure(UART2, 19200);
    
    // Note: Interrupt priority, 1 is lowest priority to 7 which is highest priority

    // Enable the interrupt sources
    // IPL6 = SPI3 CHIP SELECT (DC_CEA2045)
    // IPL5 = UART2 (AC_CEA2045)
    // IPL4 = SPI3 (DC_CEA2045)
    // IPL3 = SPI1 (MRF24WG) See StackInit()
    // IPL2 = Timer1 See TickInit()
    // IPL1 = Timer2 TimeMonitor
    // Note: WiFi Module hardware Initialization handled by StackInit() Library Routine
    // Note: Timer1 Initialization handled by TickInit() Library Routine
    // Note: Timer2 Initialization handled by TimeMonitorInit()

    IFS0CLR = 0xffffffff;
    IFS1CLR = 0xffffffff;
    IFS2CLR = 0xffffffff;
    
#ifdef DC_CEA2045
    // Drive Module Detect line low to tell the SGD that the UCM is present
    LATCCLR = BIT_1; TRISCbits.TRISC1 = 0;
    
    // Configure change notification interrupts for SPI CSn line
    SPI_CS_TRIS = 1;
    CN_TURN_ON                 //turn on CN module
    SPI_CS_INT_ENABLE     //configure SPI chip select pin for CN interrupts 
    
    INTSetVectorPriority(INT_CHANGE_NOTICE_VECTOR, INT_PRIORITY_LEVEL_6);
    INTSetVectorSubPriority(INT_CHANGE_NOTICE_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTClearFlag(INT_CN);
    INTEnable(INT_CN, INT_ENABLED);
#endif

#if defined(AC_CEA2045)            
    INTSetVectorPriority(INT_UART_1_VECTOR, INT_PRIORITY_LEVEL_5);
    INTSetVectorSubPriority(INT_UART_1_VECTOR,INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_SOURCE_UART_RX(UART1), INT_ENABLED);
#endif

#if defined(DC_CEA2045) || defined(INTWINE_CONNECTED_OUTLET) || defined(INTWINE_CONNECTED_LOAD_CONTROL)
    INTSetVectorPriority(INT_SPI_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_SPI_3_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTClearFlag(INT_SPI3RX);
    INTClearFlag(INT_SPI3TX);
    INTClearFlag(INT_SPI3E);
#endif
    
    TimeMonitorInit();

    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_1);  
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);	
    
    // Enable multi-vectored interrupts
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableSystemMultiVectoredInt();
    INTEnableInterrupts();

/****************************************************************************
 Bits RF4 and RF4 are multifunction:
 1. UART1 on AC_CEA2045
 2. RED and GREEN LEDs on ICO
 3. Tri-stated on TSTAT
****************************************************************************/

#ifdef AC_CEA2045
    // Set RS-485 DE enable to inactive low
    TRISDCLR = BIT_14;
    LATDSET = BIT_14;
    LATDSET = BIT_15;
    // TX inactive high
    TRISFCLR = BIT_8;
    LATFSET = BIT_8;
#endif
    
#ifdef DC_CEA2045
    // Initialize SPI I/Os and state machine
    SPI_Driver_Task();
#endif

    SI7005_IO = 0;  // Enable Si7005 and delay 15 ms
    DelayMs(15);

#ifdef putrsUART
    putrsUART("\r\n\r\n\r\nAztec Init Complete\r\n");
#endif
}

static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void InitAppConfig(void)
{
#if defined(SPIFLASH_CS_TRIS)
        unsigned char vNeedToSaveDefaults = 0;
#endif

        while(1)
        {
            // Start out zeroing all AppConfig bytes to ensure all fields are
            // deterministic for checksum generation
            memset((void*)&AppConfig, 0x00, sizeof(AppConfig));

            AppConfig.Flags.bIsDHCPEnabled = TRUE;
            AppConfig.Flags.bInConfigMode = TRUE;
            memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));

            // SoftAP on certain setups with IP 192.168.1.1 has problem with DHCP client assigning new IP address on redirection.
            // 192.168.1.1 is a common IP address with most APs. This is still under investigation.
            // For now, assign this as 192.168.1.3

            AppConfig.MyIPAddr.Val = 192ul | 168ul<<8ul | 1ul<<16ul | 3ul<<24ul;
            AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
            AppConfig.MyMask.Val = 255ul | 255ul<<8ul | 0ul<<16ul | 0ul<<24ul;
            AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
            AppConfig.MyGateway.Val = AppConfig.MyIPAddr.Val;
            AppConfig.PrimaryDNSServer.Val = AppConfig.MyIPAddr.Val;
            AppConfig.SecondaryDNSServer.Val = AppConfig.MyIPAddr.Val;

            // Load the default NetBIOS Host Name
            memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
            FormatNetBIOSName(AppConfig.NetBIOSName);

        #if defined(WF_CS_TRIS)
            // Load the default SSID Name
            WF_ASSERT(sizeof(MY_DEFAULT_SSID_NAME)-1 <= sizeof(AppConfig.MySSID));
            memcpypgm2ram(AppConfig.MySSID, (ROM void*)MY_DEFAULT_SSID_NAME, sizeof(MY_DEFAULT_SSID_NAME));
            AppConfig.SsidLength = sizeof(MY_DEFAULT_SSID_NAME) - 1;
            AppConfig.SecurityMode = MY_DEFAULT_WIFI_SECURITY_MODE;
            if (AppConfig.SecurityMode == WF_SECURITY_WEP_40)
            {
                AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;
                memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_40, sizeof(MY_DEFAULT_WEP_KEYS_40) - 1);
                AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_40) - 1;
            }
            else if (AppConfig.SecurityMode == WF_SECURITY_WEP_104)
            {
                AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;
                memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_104, sizeof(MY_DEFAULT_WEP_KEYS_104) - 1);
                AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_104) - 1;
            }
            AppConfig.networkType = MY_DEFAULT_NETWORK_TYPE;
            AppConfig.dataValid = 0;
        #endif

            // Compute the checksum of the AppConfig defaults as loaded from ROM
            wOriginalAppConfigChecksum = CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig));

#if defined(SPIFLASH_CS_TRIS)
            NVM_VALIDATION_STRUCT NVMValidationStruct;

            // Check to see if we have a flag set indicating that we need to
            // save the ROM default AppConfig values.
            if(vNeedToSaveDefaults)
                SaveAppConfig(&AppConfig);

            // Read the NVMValidation record and AppConfig struct out of EEPROM/Flash
                SPIFlashReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
                SPIFlashReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));

            // Check EEPROM/Flash validitity.  If it isn't valid, set a flag so
            // that we will save the ROM default values on the next loop
            // iteration.
            if((NVMValidationStruct.wConfigurationLength != sizeof(AppConfig)) ||
               (NVMValidationStruct.wOriginalChecksum != wOriginalAppConfigChecksum) ||
               (NVMValidationStruct.wCurrentChecksum != CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig))))
            {
                // Check to ensure that the vNeedToSaveDefaults flag is zero,
                // indicating that this is the first iteration through the do
                // loop.  If we have already saved the defaults once and the
                // EEPROM/Flash still doesn't pass the validity check, then it
                // means we aren't successfully reading or writing to the
                // EEPROM/Flash.  This means you have a hardware error and/or
                // SPI configuration error.
                if(vNeedToSaveDefaults)
                {
                    while(1);
                }

                // Set flag and restart loop to load ROM defaults and save them
                vNeedToSaveDefaults = 1;
                continue;
            }

            // If we get down here, it means the EEPROM/Flash has valid contents
            // and either matches the ROM defaults or previously matched and
            // was run-time reconfigured by the user.  In this case, we shall
            // use the contents loaded from EEPROM/Flash.
            break;
#endif
            break;

        }


    #if defined (EZ_CONFIG_STORE)
        // Set configuration for ZG from NVM
        /* Set security type and key if necessary, convert from app storage to ZG driver */

        if (AppConfig.dataValid)
            CFGCXT.isWifiDoneConfigure = 1;

        AppConfig.saveSecurityInfo = FALSE;
    #endif // EZ_CONFIG_STORE

}

/****************************************************************************
  Function:
    void DisplayIPValue(IP_ADDR IPVal)

  Description:
    This routine formats and prints the current IP Address.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void DisplayIPValue(IP_ADDR IPVal)
{
    sprintf(sbuf, "%u.%u.%u.%u\r\n", IPVal.v[0], IPVal.v[1], IPVal.v[2], IPVal.v[3]);
    putsUART(sbuf);
}

/****************************************************************************
  Function:
    void SaveAppConfig(const APP_CONFIG *ptrAppConfig)

  Description:
    This routine saves the AppConfig into EEPROM.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void SaveAppConfig(const APP_CONFIG *ptrAppConfig)
{
    NVM_VALIDATION_STRUCT NVMValidationStruct;

    // Get proper values for the validation structure indicating that we can use
    // these EEPROM/Flash contents on future boot ups
    NVMValidationStruct.wOriginalChecksum = wOriginalAppConfigChecksum;
    NVMValidationStruct.wCurrentChecksum = CalcIPChecksum((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
    NVMValidationStruct.wConfigurationLength = sizeof(APP_CONFIG);

#if defined(SPIFLASH_CS_TRIS)
    // Write the validation struct and current AppConfig contents to EEPROM/Flash
    SPIFlashBeginWrite(0x0000);
    SPIFlashWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
    SPIFlashWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
#endif
}

#if defined (EZ_CONFIG_STORE)
/****************************************************************************
  Function:
    void RestoreWifiConfig(void)

  Description:
    This routine performs reboot when SW0 is pressed.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void RestoreWifiConfig(void)
{
#if defined(SPIFLASH_CS_TRIS)
    SPIFlashBeginWrite(0x0000);
    SPIFlashWrite(0xFF);
    SPIFlashWrite(0xFF);
#endif
    // reboot here...
    //LED_PUT(0x00);
    while(SW0_IO == 0u);
    while(1);       // Watchdog will reset
//    Reset();
}
#endif // EZ_CONFIG_STORE
