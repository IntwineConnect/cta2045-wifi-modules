 /*********************************************************************
 *
 *  Hardware specific definitions for:
 *    -  Aztec Board
 *          ~ PIC32MX575F512L
 *          ~ MRF24WG0MA
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    Compiler.h
 * Processor:       PIC32MX575F512L
 * Compiler:        Microchip  XC32 Compiler
 * Company:         Intwine Connect, LLC
 *
 
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Amy Ong                         10/22/2012    Created for WiFi G Demo Board to conform to existing MLA file structures
 * Jim Kapcio                      10/22/2012    Modified for Aztec Board
 ********************************************************************/

#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

#define MRF24WG

// Set configuration fuses (but only in Main.c where THIS_IS_STACK_APPLICATION is defined)
#if defined(THIS_IS_STACK_APPLICATION)
    #pragma config CP = OFF             //Prevents boot and program Flash memory from being read or modified by an external programming device.
    #pragma config BWP = OFF            //Prevents boot Flash memory from being modified during code execution.
    #pragma config PWP = OFF            //Prevents selected program Flash memory pages from being modified during code execution. The PWP bits represent the 1’s complement of the number of write-protected program Flash memory pages.
    #pragma config WDTPS = PS8192       //Allows the WDT module to be controlled by software
    #pragma config FPBDIV = DIV_1       //Peripheral Bus Clock Divisor Default Value bits
    #pragma config POSCMOD = HS         //Primary Oscillator Configuration bits (set to HIGH SPEED)
    #pragma config UPLLEN = ON          //USB PLL Enable bit
    #pragma config UPLLIDIV = DIV_6     //PLL Input Divider bits for 24MHz

// Intwine source crystal is 24MHz
    #pragma config FPLLIDIV = DIV_6     // 24MHz / 6 = 4MHz (in the desired range for PLL)
    #pragma config FPLLMUL = MUL_20     // times 20 = 80MHz
    #pragma config FPLLODIV = DIV_2     // divide by 2 = 40MHz

// Set up for Two-Speed Start-Up (Section 6.3.5)
    #pragma config FNOSC = PRIPLL       //Oscillator Selection bits - set to Primary Oscillator (POSC) with PLL module (XT+PLL, HS+PLL, EC+PLL)
    #pragma config IESO = ON            //Internal External Switchover bit ON
    #pragma config FCKSM = CSECME       //Clock Switching Enabled, Clock Monitoring Enabled

    #ifdef __DEBUG
        #pragma config FWDTEN = OFF         //Allows the WDT module to be controlled by software
    #else
        #pragma config FWDTEN = ON          //Allows the WDT module to be controlled by software
    #endif
#endif

#define GetSystemClock()        (40000000ul)                // 24MHz/(FPLLIDIV=DIV_6)*(FPLLMUL=MUL_20)/(FPLLODIV=DIV_2)
#define GetInstructionClock()   (GetSystemClock()/1)
#define GetPeripheralClock()    (GetInstructionClock()/1)   // Set your divider according to your Peripheral Bus Frequency configuration fuse setting


// ====================================================
//   Hardware mappings
// ====================================================


//----------------------------
// LED and Button I/O pins
//----------------------------
#define LED0_TRIS           (TRISEbits.TRISE2)  // Ref E2 Green
#define LED0_IO             (LATEbits.LATE2)
#define LED1_TRIS           (TRISEbits.TRISE0)  // Ref E0 Yellow
#define LED1_IO             (LATEbits.LATE0)
#define LED2_TRIS           (TRISEbits.TRISE1)  // Ref E1 Red
#define LED2_IO             (LATEbits.LATE1)

#define LEDS_ON()           {LED0_ON(); LED1_ON(); LED2_ON();}
#define LEDS_OFF()          {LED0_OFF(); LED1_OFF(); LED2_OFF();}
#define LED0_ON()           LATESET = BIT_2;
#define LED0_OFF()          LATECLR = BIT_2;
#define LED0_INV()          LATEINV = BIT_2;

#define LED1_ON()           LATESET = BIT_0;
#define LED1_OFF()          LATECLR = BIT_0;
#define LED1_INV()          LATEINV = BIT_0;

#define LED2_ON()           LATESET = BIT_1;
#define LED2_OFF()          LATECLR = BIT_1;
#define LED2_INV()          LATEINV = BIT_1;

#define SW0_TRIS            (TRISEbits.TRISE9)
#define SW0_IO              (PORTEbits.RE9)

#define VBAT_TRIS           (TRISBbits.TRISB0)     // Battery level ADC input

// Added to support EZ_CONFIG_STORE
// 25LC256 I/O pins
//#define EEPROM_CS_TRIS        (TRISGbits.TRISG9)
#define EEPROM_CS_IO        (LATGbits.LATG9)
#define EEPROM_SCK_TRIS     (TRISGbits.TRISG6)
#define EEPROM_SDI_TRIS     (TRISGbits.TRISG7)
#define EEPROM_SDO_TRIS     (TRISGbits.TRISG8)
#define EEPROM_SPI_IF       (IFS1bits.SPI2RXIF)
#define EEPROM_SSPBUF       (SPI2BUF)
#define EEPROM_SPICON1      (SPI2CON)
#define EEPROM_SPICON1bits  (SPI2CONbits)
#define EEPROM_SPIBRG       (SPI2BRG)
#define EEPROM_SPISTAT      (SPI2STAT)
#define EEPROM_SPISTATbits  (SPI2STATbits)

// SST25VF080B I/O pins
#define SPIFLASH_CS_TRIS      (TRISGbits.TRISG9)
#define SPIFLASH_CS_IO        (LATGbits.LATG9)
#define SPIFLASH_SCK_TRIS     (TRISGbits.TRISG6)
#define SPIFLASH_SDI_TRIS     (TRISGbits.TRISG7)
#define SPIFLASH_SDO_TRIS     (TRISGbits.TRISG8)
#define SPIFLASH_SPI_IF       (IFS1bits.SPI2RXIF)
#define SPIFLASH_SSPBUF       (SPI2BUF)
#define SPIFLASH_SPICON1      (SPI2CON)
#define SPIFLASH_SPICON1bits  (SPI2CONbits)
#define SPIFLASH_SPIBRG       (SPI2BRG)
#define SPIFLASH_SPISTAT      (SPI2STAT)
#define SPIFLASH_SPISTATbits  (SPI2STATbits)


//----------------------------
// MRF24WG0MA/B WiFi I/O pins
//----------------------------

#define WF_CS_TRIS          (TRISDbits.TRISD9)
#define WF_CS_IO            (LATDbits.LATD9)
#define WF_SDI_TRIS         (TRISCbits.TRISC4)
#define WF_SCK_TRIS         (TRISDbits.TRISD10)
#define WF_SDO_TRIS         (TRISDbits.TRISD0)
#define WF_RESET_TRIS       (TRISEbits.TRISE7)
#define WF_RESET_IO         (LATEbits.LATE7)

// NOTE:
//   Wifi comm demo declares  WF_INT_VECTOR as   _EXTERNAL_1_VECTOR and used in WF_Eint.c.
//   Wifi G demo adopts generic approach taken by demo/console/ezconfig by defining MRF24W_IN_SPI1.
//     _EXTERNAL_1_VECTOR  = MRF24W_IN_SPI1
#define MRF24W_IN_SPI1

#define WF_INT_TRIS         (TRISAbits.TRISA14)  // INT3
#define WF_INT_IO           (PORTAbits.RA14)

#define WF_HIBERNATE_TRIS   (TRISAbits.TRISA15)
#define WF_HIBERNATE_IO     (PORTAbits.RA15)

#define WF_INT_EDGE         (INTCONbits.INT3EP)
#define WF_INT_IE           (IEC0bits.INT3IE)
#define WF_INT_IF           (IFS0bits.INT3IF)
#define WF_INT_IE_CLEAR     IEC0CLR
#define WF_INT_IF_CLEAR     IFS0CLR
#define WF_INT_IE_SET       IEC0SET
#define WF_INT_IF_SET       IFS0SET
#define WF_INT_BIT          0x00008000
#define WF_INT_IPCSET       IPC3SET
#define WF_INT_IPCCLR       IPC3CLR
#define WF_INT_IPC_MASK     0xFF000000
#define WF_INT_IPC_VALUE    0x0C000000

#define WF_SSPBUF           (SPI1BUF)
#define WF_SPISTAT          (SPI1STAT)
#define WF_SPISTATbits      (SPI1STATbits)

#define WF_SPICON1          (SPI1CON)
#define WF_SPICON1bits      (SPI1CONbits)
#define WF_SPI_IE_CLEAR     IEC0CLR
#define WF_SPI_IF_CLEAR     IFS0CLR
#define WF_SPI_INT_BITS     0x03800000
#define WF_SPI_BRG          (SPI1BRG)
#define WF_MAX_SPI_FREQ     (10000000ul)    // Hz

//----------------------------
// UART to Telnet Mapping
//----------------------------
//#define BusyUART()          (TelnetOutFree() ? 0 : (StackTask(), 1))
//#define putcUART            putchar
//#define putrsUART(a)        fputs((const char*)a,(FILE *)stdout)
//#define putsUART(a)         fputs((const char*)a,(FILE *)stdout)
//#define DataRdyUART()       TelnetInChars()
//#define ReadUART()          TelnetGet()

//----------------------------
// Sensor Port Mapping
//----------------------------
#define SENSOR_UART         UART2

//modified for UART support on Wifi-Comm Demo Board (for use of (STACK_USE_UART)-macro) 
#define UARTTX_TRIS   (TRISFbits.TRISF5) 
#define UARTTX_IO   (PORTFbits.RF5) 
#define UARTRX_TRIS   (TRISFbits.TRISF4) 
#define UARTRX_IO   (PORTFbits.RF4) 
#define UBRG    U2BRG 
#define UMODE    U2MODE 
#define USTA    U2STA 
#define BusyUART()          BusyUART2() 
#define putcUART            do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0) 
#define putrsUART(a)        putrsUART2(a) 
#define putsUART(a)         putsUART2(a) 
#define DataRdyUART()      DataRdyUART2() 
#define ReadUART()         ReadUART2()

#endif // #ifndef HARDWARE_PROFILE_H
