#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../MicroChip/TCPIP_Stack/Announce.c ../MicroChip/TCPIP_Stack/ARCFOUR.c ../MicroChip/TCPIP_Stack/ARP.c ../MicroChip/TCPIP_Stack/AutoIP.c ../MicroChip/TCPIP_Stack/BerkeleyAPI.c ../MicroChip/TCPIP_Stack/BigInt.c ../MicroChip/TCPIP_Stack/BigInt_helper_PIC32.S ../MicroChip/TCPIP_Stack/Delay.c ../MicroChip/TCPIP_Stack/DNS.c ../MicroChip/TCPIP_Stack/DNSs.c ../MicroChip/TCPIP_Stack/DynDNS.c ../MicroChip/TCPIP_Stack/FileSystem.c ../MicroChip/TCPIP_Stack/FTP.c ../MicroChip/TCPIP_Stack/Hashes.c ../MicroChip/TCPIP_Stack/Helpers.c ../MicroChip/TCPIP_Stack/HTTP2.c ../MicroChip/TCPIP_Stack/ICMP.c ../MicroChip/TCPIP_Stack/IP.c ../MicroChip/TCPIP_Stack/LCDBlocking.c ../MicroChip/TCPIP_Stack/MPFS2.c ../MicroChip/TCPIP_Stack/NBNS.c ../MicroChip/TCPIP_Stack/Random.c ../MicroChip/TCPIP_Stack/Reboot.c ../MicroChip/TCPIP_Stack/RSA.c ../MicroChip/TCPIP_Stack/SMTP.c ../MicroChip/TCPIP_Stack/SNMP.c ../MicroChip/TCPIP_Stack/SNMPv3.c ../MicroChip/TCPIP_Stack/SNMPv3USM.c ../MicroChip/TCPIP_Stack/SNTP.c ../MicroChip/TCPIP_Stack/SPIEEPROM.c ../MicroChip/TCPIP_Stack/SPIFlash.c ../MicroChip/TCPIP_Stack/SPIRAM.c ../MicroChip/TCPIP_Stack/SSL.c ../MicroChip/TCPIP_Stack/StackTsk.c ../MicroChip/TCPIP_Stack/TCP.c ../MicroChip/TCPIP_Stack/TCPPerformanceTest.c ../MicroChip/TCPIP_Stack/Telnet.c ../MicroChip/TCPIP_Stack/TFTPc.c ../MicroChip/TCPIP_Stack/Tick.c ../MicroChip/TCPIP_Stack/UART2TCPBridge.c ../MicroChip/TCPIP_Stack/UDP.c ../MicroChip/TCPIP_Stack/UDPPerformanceTest.c ../MicroChip/TCPIP_Stack/ZeroconfHelper.c ../MicroChip/TCPIP_Stack/ZeroconfLinkLocal.c ../MicroChip/TCPIP_Stack/ZeroconfMulticastDNS.c ../MicroChip/TCPIP_Stack/DHCP.c ../MicroChip/TCPIP_Stack/DHCPs.c ../MicroChip/TCPIP_Stack/WiFi/WF_Eint.c ../MicroChip/TCPIP_Stack/WiFi/WF_pbkdf2.c ../MicroChip/TCPIP_Stack/WiFi/WFConnectionAlgorithm.c ../MicroChip/TCPIP_Stack/WiFi/WFConnectionManager.c ../MicroChip/TCPIP_Stack/WiFi/WFConnectionProfile.c ../MicroChip/TCPIP_Stack/WiFi/WFConsole.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIfconfig.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwconfig.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwpriv.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgHandler.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgs.c ../MicroChip/TCPIP_Stack/WiFi/WFDataTxRx.c ../MicroChip/TCPIP_Stack/WiFi/WFDebugStrings.c ../MicroChip/TCPIP_Stack/WiFi/WFEasyConfig.c ../MicroChip/TCPIP_Stack/WiFi/WFEventHandler.c ../MicroChip/TCPIP_Stack/WiFi/WFInit.c ../MicroChip/TCPIP_Stack/WiFi/WFPowerSave.c ../MicroChip/TCPIP_Stack/WiFi/WFScan.c ../MicroChip/TCPIP_Stack/WiFi/WFTxPower.c ../MicroChip/TCPIP_Stack/WiFi/WF_Spi.c ../MicroChip/TCPIP_Stack/WiFi/WFDriverCom_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFDriverRaw_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFMac_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFMgmtMsg_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFParamMsg_24G.c ../Main.c ../CustomHTTPApp.c ../WF_Config.c ../WifiGDemoMPFSImg.c ../i2c_master.c ../OpenADRClient.c ../INTiMain.c ../UARTiMain.c ../TimeMonitor.c ../EPRI_UARTLayer.c ../app_UCM.c ../Assert.c ../EPRI_SPILayer.c ../MCI_Common.c ../LinkLayerMessaging.c ../BasicDR.c ../debugging.c ../RESTAPI.c ../DeviceInfo.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/93166211/Announce.o ${OBJECTDIR}/_ext/93166211/ARCFOUR.o ${OBJECTDIR}/_ext/93166211/ARP.o ${OBJECTDIR}/_ext/93166211/AutoIP.o ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o ${OBJECTDIR}/_ext/93166211/BigInt.o ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o ${OBJECTDIR}/_ext/93166211/Delay.o ${OBJECTDIR}/_ext/93166211/DNS.o ${OBJECTDIR}/_ext/93166211/DNSs.o ${OBJECTDIR}/_ext/93166211/DynDNS.o ${OBJECTDIR}/_ext/93166211/FileSystem.o ${OBJECTDIR}/_ext/93166211/FTP.o ${OBJECTDIR}/_ext/93166211/Hashes.o ${OBJECTDIR}/_ext/93166211/Helpers.o ${OBJECTDIR}/_ext/93166211/HTTP2.o ${OBJECTDIR}/_ext/93166211/ICMP.o ${OBJECTDIR}/_ext/93166211/IP.o ${OBJECTDIR}/_ext/93166211/LCDBlocking.o ${OBJECTDIR}/_ext/93166211/MPFS2.o ${OBJECTDIR}/_ext/93166211/NBNS.o ${OBJECTDIR}/_ext/93166211/Random.o ${OBJECTDIR}/_ext/93166211/Reboot.o ${OBJECTDIR}/_ext/93166211/RSA.o ${OBJECTDIR}/_ext/93166211/SMTP.o ${OBJECTDIR}/_ext/93166211/SNMP.o ${OBJECTDIR}/_ext/93166211/SNMPv3.o ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o ${OBJECTDIR}/_ext/93166211/SNTP.o ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o ${OBJECTDIR}/_ext/93166211/SPIFlash.o ${OBJECTDIR}/_ext/93166211/SPIRAM.o ${OBJECTDIR}/_ext/93166211/SSL.o ${OBJECTDIR}/_ext/93166211/StackTsk.o ${OBJECTDIR}/_ext/93166211/TCP.o ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o ${OBJECTDIR}/_ext/93166211/Telnet.o ${OBJECTDIR}/_ext/93166211/TFTPc.o ${OBJECTDIR}/_ext/93166211/Tick.o ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o ${OBJECTDIR}/_ext/93166211/UDP.o ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o ${OBJECTDIR}/_ext/93166211/DHCP.o ${OBJECTDIR}/_ext/93166211/DHCPs.o ${OBJECTDIR}/_ext/388820833/WF_Eint.o ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o ${OBJECTDIR}/_ext/388820833/WFConsole.o ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o ${OBJECTDIR}/_ext/388820833/WFEventHandler.o ${OBJECTDIR}/_ext/388820833/WFInit.o ${OBJECTDIR}/_ext/388820833/WFPowerSave.o ${OBJECTDIR}/_ext/388820833/WFScan.o ${OBJECTDIR}/_ext/388820833/WFTxPower.o ${OBJECTDIR}/_ext/388820833/WF_Spi.o ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o ${OBJECTDIR}/_ext/388820833/WFMac_24G.o ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o ${OBJECTDIR}/_ext/1472/Main.o ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o ${OBJECTDIR}/_ext/1472/WF_Config.o ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o ${OBJECTDIR}/_ext/1472/i2c_master.o ${OBJECTDIR}/_ext/1472/OpenADRClient.o ${OBJECTDIR}/_ext/1472/INTiMain.o ${OBJECTDIR}/_ext/1472/UARTiMain.o ${OBJECTDIR}/_ext/1472/TimeMonitor.o ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o ${OBJECTDIR}/_ext/1472/app_UCM.o ${OBJECTDIR}/_ext/1472/Assert.o ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o ${OBJECTDIR}/_ext/1472/MCI_Common.o ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o ${OBJECTDIR}/_ext/1472/BasicDR.o ${OBJECTDIR}/_ext/1472/debugging.o ${OBJECTDIR}/_ext/1472/RESTAPI.o ${OBJECTDIR}/_ext/1472/DeviceInfo.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/93166211/Announce.o.d ${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d ${OBJECTDIR}/_ext/93166211/ARP.o.d ${OBJECTDIR}/_ext/93166211/AutoIP.o.d ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d ${OBJECTDIR}/_ext/93166211/BigInt.o.d ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d ${OBJECTDIR}/_ext/93166211/Delay.o.d ${OBJECTDIR}/_ext/93166211/DNS.o.d ${OBJECTDIR}/_ext/93166211/DNSs.o.d ${OBJECTDIR}/_ext/93166211/DynDNS.o.d ${OBJECTDIR}/_ext/93166211/FileSystem.o.d ${OBJECTDIR}/_ext/93166211/FTP.o.d ${OBJECTDIR}/_ext/93166211/Hashes.o.d ${OBJECTDIR}/_ext/93166211/Helpers.o.d ${OBJECTDIR}/_ext/93166211/HTTP2.o.d ${OBJECTDIR}/_ext/93166211/ICMP.o.d ${OBJECTDIR}/_ext/93166211/IP.o.d ${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d ${OBJECTDIR}/_ext/93166211/MPFS2.o.d ${OBJECTDIR}/_ext/93166211/NBNS.o.d ${OBJECTDIR}/_ext/93166211/Random.o.d ${OBJECTDIR}/_ext/93166211/Reboot.o.d ${OBJECTDIR}/_ext/93166211/RSA.o.d ${OBJECTDIR}/_ext/93166211/SMTP.o.d ${OBJECTDIR}/_ext/93166211/SNMP.o.d ${OBJECTDIR}/_ext/93166211/SNMPv3.o.d ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d ${OBJECTDIR}/_ext/93166211/SNTP.o.d ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d ${OBJECTDIR}/_ext/93166211/SPIFlash.o.d ${OBJECTDIR}/_ext/93166211/SPIRAM.o.d ${OBJECTDIR}/_ext/93166211/SSL.o.d ${OBJECTDIR}/_ext/93166211/StackTsk.o.d ${OBJECTDIR}/_ext/93166211/TCP.o.d ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d ${OBJECTDIR}/_ext/93166211/Telnet.o.d ${OBJECTDIR}/_ext/93166211/TFTPc.o.d ${OBJECTDIR}/_ext/93166211/Tick.o.d ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d ${OBJECTDIR}/_ext/93166211/UDP.o.d ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d ${OBJECTDIR}/_ext/93166211/DHCP.o.d ${OBJECTDIR}/_ext/93166211/DHCPs.o.d ${OBJECTDIR}/_ext/388820833/WF_Eint.o.d ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d ${OBJECTDIR}/_ext/388820833/WFConsole.o.d ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d ${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d ${OBJECTDIR}/_ext/388820833/WFInit.o.d ${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d ${OBJECTDIR}/_ext/388820833/WFScan.o.d ${OBJECTDIR}/_ext/388820833/WFTxPower.o.d ${OBJECTDIR}/_ext/388820833/WF_Spi.o.d ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d ${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d ${OBJECTDIR}/_ext/1472/Main.o.d ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d ${OBJECTDIR}/_ext/1472/WF_Config.o.d ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d ${OBJECTDIR}/_ext/1472/i2c_master.o.d ${OBJECTDIR}/_ext/1472/OpenADRClient.o.d ${OBJECTDIR}/_ext/1472/INTiMain.o.d ${OBJECTDIR}/_ext/1472/UARTiMain.o.d ${OBJECTDIR}/_ext/1472/TimeMonitor.o.d ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d ${OBJECTDIR}/_ext/1472/app_UCM.o.d ${OBJECTDIR}/_ext/1472/Assert.o.d ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d ${OBJECTDIR}/_ext/1472/MCI_Common.o.d ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d ${OBJECTDIR}/_ext/1472/BasicDR.o.d ${OBJECTDIR}/_ext/1472/debugging.o.d ${OBJECTDIR}/_ext/1472/RESTAPI.o.d ${OBJECTDIR}/_ext/1472/DeviceInfo.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/93166211/Announce.o ${OBJECTDIR}/_ext/93166211/ARCFOUR.o ${OBJECTDIR}/_ext/93166211/ARP.o ${OBJECTDIR}/_ext/93166211/AutoIP.o ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o ${OBJECTDIR}/_ext/93166211/BigInt.o ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o ${OBJECTDIR}/_ext/93166211/Delay.o ${OBJECTDIR}/_ext/93166211/DNS.o ${OBJECTDIR}/_ext/93166211/DNSs.o ${OBJECTDIR}/_ext/93166211/DynDNS.o ${OBJECTDIR}/_ext/93166211/FileSystem.o ${OBJECTDIR}/_ext/93166211/FTP.o ${OBJECTDIR}/_ext/93166211/Hashes.o ${OBJECTDIR}/_ext/93166211/Helpers.o ${OBJECTDIR}/_ext/93166211/HTTP2.o ${OBJECTDIR}/_ext/93166211/ICMP.o ${OBJECTDIR}/_ext/93166211/IP.o ${OBJECTDIR}/_ext/93166211/LCDBlocking.o ${OBJECTDIR}/_ext/93166211/MPFS2.o ${OBJECTDIR}/_ext/93166211/NBNS.o ${OBJECTDIR}/_ext/93166211/Random.o ${OBJECTDIR}/_ext/93166211/Reboot.o ${OBJECTDIR}/_ext/93166211/RSA.o ${OBJECTDIR}/_ext/93166211/SMTP.o ${OBJECTDIR}/_ext/93166211/SNMP.o ${OBJECTDIR}/_ext/93166211/SNMPv3.o ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o ${OBJECTDIR}/_ext/93166211/SNTP.o ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o ${OBJECTDIR}/_ext/93166211/SPIFlash.o ${OBJECTDIR}/_ext/93166211/SPIRAM.o ${OBJECTDIR}/_ext/93166211/SSL.o ${OBJECTDIR}/_ext/93166211/StackTsk.o ${OBJECTDIR}/_ext/93166211/TCP.o ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o ${OBJECTDIR}/_ext/93166211/Telnet.o ${OBJECTDIR}/_ext/93166211/TFTPc.o ${OBJECTDIR}/_ext/93166211/Tick.o ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o ${OBJECTDIR}/_ext/93166211/UDP.o ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o ${OBJECTDIR}/_ext/93166211/DHCP.o ${OBJECTDIR}/_ext/93166211/DHCPs.o ${OBJECTDIR}/_ext/388820833/WF_Eint.o ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o ${OBJECTDIR}/_ext/388820833/WFConsole.o ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o ${OBJECTDIR}/_ext/388820833/WFEventHandler.o ${OBJECTDIR}/_ext/388820833/WFInit.o ${OBJECTDIR}/_ext/388820833/WFPowerSave.o ${OBJECTDIR}/_ext/388820833/WFScan.o ${OBJECTDIR}/_ext/388820833/WFTxPower.o ${OBJECTDIR}/_ext/388820833/WF_Spi.o ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o ${OBJECTDIR}/_ext/388820833/WFMac_24G.o ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o ${OBJECTDIR}/_ext/1472/Main.o ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o ${OBJECTDIR}/_ext/1472/WF_Config.o ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o ${OBJECTDIR}/_ext/1472/i2c_master.o ${OBJECTDIR}/_ext/1472/OpenADRClient.o ${OBJECTDIR}/_ext/1472/INTiMain.o ${OBJECTDIR}/_ext/1472/UARTiMain.o ${OBJECTDIR}/_ext/1472/TimeMonitor.o ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o ${OBJECTDIR}/_ext/1472/app_UCM.o ${OBJECTDIR}/_ext/1472/Assert.o ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o ${OBJECTDIR}/_ext/1472/MCI_Common.o ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o ${OBJECTDIR}/_ext/1472/BasicDR.o ${OBJECTDIR}/_ext/1472/debugging.o ${OBJECTDIR}/_ext/1472/RESTAPI.o ${OBJECTDIR}/_ext/1472/DeviceInfo.o

# Source Files
SOURCEFILES=../MicroChip/TCPIP_Stack/Announce.c ../MicroChip/TCPIP_Stack/ARCFOUR.c ../MicroChip/TCPIP_Stack/ARP.c ../MicroChip/TCPIP_Stack/AutoIP.c ../MicroChip/TCPIP_Stack/BerkeleyAPI.c ../MicroChip/TCPIP_Stack/BigInt.c ../MicroChip/TCPIP_Stack/BigInt_helper_PIC32.S ../MicroChip/TCPIP_Stack/Delay.c ../MicroChip/TCPIP_Stack/DNS.c ../MicroChip/TCPIP_Stack/DNSs.c ../MicroChip/TCPIP_Stack/DynDNS.c ../MicroChip/TCPIP_Stack/FileSystem.c ../MicroChip/TCPIP_Stack/FTP.c ../MicroChip/TCPIP_Stack/Hashes.c ../MicroChip/TCPIP_Stack/Helpers.c ../MicroChip/TCPIP_Stack/HTTP2.c ../MicroChip/TCPIP_Stack/ICMP.c ../MicroChip/TCPIP_Stack/IP.c ../MicroChip/TCPIP_Stack/LCDBlocking.c ../MicroChip/TCPIP_Stack/MPFS2.c ../MicroChip/TCPIP_Stack/NBNS.c ../MicroChip/TCPIP_Stack/Random.c ../MicroChip/TCPIP_Stack/Reboot.c ../MicroChip/TCPIP_Stack/RSA.c ../MicroChip/TCPIP_Stack/SMTP.c ../MicroChip/TCPIP_Stack/SNMP.c ../MicroChip/TCPIP_Stack/SNMPv3.c ../MicroChip/TCPIP_Stack/SNMPv3USM.c ../MicroChip/TCPIP_Stack/SNTP.c ../MicroChip/TCPIP_Stack/SPIEEPROM.c ../MicroChip/TCPIP_Stack/SPIFlash.c ../MicroChip/TCPIP_Stack/SPIRAM.c ../MicroChip/TCPIP_Stack/SSL.c ../MicroChip/TCPIP_Stack/StackTsk.c ../MicroChip/TCPIP_Stack/TCP.c ../MicroChip/TCPIP_Stack/TCPPerformanceTest.c ../MicroChip/TCPIP_Stack/Telnet.c ../MicroChip/TCPIP_Stack/TFTPc.c ../MicroChip/TCPIP_Stack/Tick.c ../MicroChip/TCPIP_Stack/UART2TCPBridge.c ../MicroChip/TCPIP_Stack/UDP.c ../MicroChip/TCPIP_Stack/UDPPerformanceTest.c ../MicroChip/TCPIP_Stack/ZeroconfHelper.c ../MicroChip/TCPIP_Stack/ZeroconfLinkLocal.c ../MicroChip/TCPIP_Stack/ZeroconfMulticastDNS.c ../MicroChip/TCPIP_Stack/DHCP.c ../MicroChip/TCPIP_Stack/DHCPs.c ../MicroChip/TCPIP_Stack/WiFi/WF_Eint.c ../MicroChip/TCPIP_Stack/WiFi/WF_pbkdf2.c ../MicroChip/TCPIP_Stack/WiFi/WFConnectionAlgorithm.c ../MicroChip/TCPIP_Stack/WiFi/WFConnectionManager.c ../MicroChip/TCPIP_Stack/WiFi/WFConnectionProfile.c ../MicroChip/TCPIP_Stack/WiFi/WFConsole.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIfconfig.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwconfig.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwpriv.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgHandler.c ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgs.c ../MicroChip/TCPIP_Stack/WiFi/WFDataTxRx.c ../MicroChip/TCPIP_Stack/WiFi/WFDebugStrings.c ../MicroChip/TCPIP_Stack/WiFi/WFEasyConfig.c ../MicroChip/TCPIP_Stack/WiFi/WFEventHandler.c ../MicroChip/TCPIP_Stack/WiFi/WFInit.c ../MicroChip/TCPIP_Stack/WiFi/WFPowerSave.c ../MicroChip/TCPIP_Stack/WiFi/WFScan.c ../MicroChip/TCPIP_Stack/WiFi/WFTxPower.c ../MicroChip/TCPIP_Stack/WiFi/WF_Spi.c ../MicroChip/TCPIP_Stack/WiFi/WFDriverCom_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFDriverRaw_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFMac_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFMgmtMsg_24G.c ../MicroChip/TCPIP_Stack/WiFi/WFParamMsg_24G.c ../Main.c ../CustomHTTPApp.c ../WF_Config.c ../WifiGDemoMPFSImg.c ../i2c_master.c ../OpenADRClient.c ../INTiMain.c ../UARTiMain.c ../TimeMonitor.c ../EPRI_UARTLayer.c ../app_UCM.c ../Assert.c ../EPRI_SPILayer.c ../MCI_Common.c ../LinkLayerMessaging.c ../BasicDR.c ../debugging.c ../RESTAPI.c ../DeviceInfo.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX575F256L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o: ../MicroChip/TCPIP_Stack/BigInt_helper_PIC32.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.ok ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d" "${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -c -mprocessor=$(MP_PROCESSOR_OPTION) -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d"  -o ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o ../MicroChip/TCPIP_Stack/BigInt_helper_PIC32.S  -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-I".." -I"." -gdwarf-2
	
else
${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o: ../MicroChip/TCPIP_Stack/BigInt_helper_PIC32.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.ok ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d" "${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -c -mprocessor=$(MP_PROCESSOR_OPTION) -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.d"  -o ${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o ../MicroChip/TCPIP_Stack/BigInt_helper_PIC32.S  -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/93166211/BigInt_helper_PIC32.o.asm.d",-I".." -I"." -gdwarf-2
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/93166211/Announce.o: ../MicroChip/TCPIP_Stack/Announce.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Announce.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Announce.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Announce.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Announce.o.d" -o ${OBJECTDIR}/_ext/93166211/Announce.o ../MicroChip/TCPIP_Stack/Announce.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ARCFOUR.o: ../MicroChip/TCPIP_Stack/ARCFOUR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARCFOUR.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d" -o ${OBJECTDIR}/_ext/93166211/ARCFOUR.o ../MicroChip/TCPIP_Stack/ARCFOUR.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ARP.o: ../MicroChip/TCPIP_Stack/ARP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ARP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ARP.o.d" -o ${OBJECTDIR}/_ext/93166211/ARP.o ../MicroChip/TCPIP_Stack/ARP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/AutoIP.o: ../MicroChip/TCPIP_Stack/AutoIP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/AutoIP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/AutoIP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/AutoIP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/AutoIP.o.d" -o ${OBJECTDIR}/_ext/93166211/AutoIP.o ../MicroChip/TCPIP_Stack/AutoIP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o: ../MicroChip/TCPIP_Stack/BerkeleyAPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d" -o ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o ../MicroChip/TCPIP_Stack/BerkeleyAPI.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/BigInt.o: ../MicroChip/TCPIP_Stack/BigInt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/BigInt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/BigInt.o.d" -o ${OBJECTDIR}/_ext/93166211/BigInt.o ../MicroChip/TCPIP_Stack/BigInt.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Delay.o: ../MicroChip/TCPIP_Stack/Delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Delay.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Delay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Delay.o.d" -o ${OBJECTDIR}/_ext/93166211/Delay.o ../MicroChip/TCPIP_Stack/Delay.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DNS.o: ../MicroChip/TCPIP_Stack/DNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DNS.o.d" -o ${OBJECTDIR}/_ext/93166211/DNS.o ../MicroChip/TCPIP_Stack/DNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DNSs.o: ../MicroChip/TCPIP_Stack/DNSs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNSs.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNSs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DNSs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DNSs.o.d" -o ${OBJECTDIR}/_ext/93166211/DNSs.o ../MicroChip/TCPIP_Stack/DNSs.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DynDNS.o: ../MicroChip/TCPIP_Stack/DynDNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DynDNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DynDNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DynDNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DynDNS.o.d" -o ${OBJECTDIR}/_ext/93166211/DynDNS.o ../MicroChip/TCPIP_Stack/DynDNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/FileSystem.o: ../MicroChip/TCPIP_Stack/FileSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/FileSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/FileSystem.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/FileSystem.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/FileSystem.o.d" -o ${OBJECTDIR}/_ext/93166211/FileSystem.o ../MicroChip/TCPIP_Stack/FileSystem.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/FTP.o: ../MicroChip/TCPIP_Stack/FTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/FTP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/FTP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/FTP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/FTP.o.d" -o ${OBJECTDIR}/_ext/93166211/FTP.o ../MicroChip/TCPIP_Stack/FTP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Hashes.o: ../MicroChip/TCPIP_Stack/Hashes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Hashes.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Hashes.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Hashes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Hashes.o.d" -o ${OBJECTDIR}/_ext/93166211/Hashes.o ../MicroChip/TCPIP_Stack/Hashes.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Helpers.o: ../MicroChip/TCPIP_Stack/Helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Helpers.o.d" -o ${OBJECTDIR}/_ext/93166211/Helpers.o ../MicroChip/TCPIP_Stack/Helpers.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/HTTP2.o: ../MicroChip/TCPIP_Stack/HTTP2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/HTTP2.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/HTTP2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/HTTP2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/HTTP2.o.d" -o ${OBJECTDIR}/_ext/93166211/HTTP2.o ../MicroChip/TCPIP_Stack/HTTP2.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ICMP.o: ../MicroChip/TCPIP_Stack/ICMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ICMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ICMP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ICMP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ICMP.o.d" -o ${OBJECTDIR}/_ext/93166211/ICMP.o ../MicroChip/TCPIP_Stack/ICMP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/IP.o: ../MicroChip/TCPIP_Stack/IP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/IP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/IP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/IP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/IP.o.d" -o ${OBJECTDIR}/_ext/93166211/IP.o ../MicroChip/TCPIP_Stack/IP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/LCDBlocking.o: ../MicroChip/TCPIP_Stack/LCDBlocking.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/LCDBlocking.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d" -o ${OBJECTDIR}/_ext/93166211/LCDBlocking.o ../MicroChip/TCPIP_Stack/LCDBlocking.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/MPFS2.o: ../MicroChip/TCPIP_Stack/MPFS2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/MPFS2.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/MPFS2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/MPFS2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/MPFS2.o.d" -o ${OBJECTDIR}/_ext/93166211/MPFS2.o ../MicroChip/TCPIP_Stack/MPFS2.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/NBNS.o: ../MicroChip/TCPIP_Stack/NBNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/NBNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/NBNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/NBNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/NBNS.o.d" -o ${OBJECTDIR}/_ext/93166211/NBNS.o ../MicroChip/TCPIP_Stack/NBNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Random.o: ../MicroChip/TCPIP_Stack/Random.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Random.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Random.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Random.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Random.o.d" -o ${OBJECTDIR}/_ext/93166211/Random.o ../MicroChip/TCPIP_Stack/Random.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Reboot.o: ../MicroChip/TCPIP_Stack/Reboot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Reboot.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Reboot.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Reboot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Reboot.o.d" -o ${OBJECTDIR}/_ext/93166211/Reboot.o ../MicroChip/TCPIP_Stack/Reboot.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/RSA.o: ../MicroChip/TCPIP_Stack/RSA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/RSA.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/RSA.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/RSA.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/RSA.o.d" -o ${OBJECTDIR}/_ext/93166211/RSA.o ../MicroChip/TCPIP_Stack/RSA.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SMTP.o: ../MicroChip/TCPIP_Stack/SMTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SMTP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SMTP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SMTP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SMTP.o.d" -o ${OBJECTDIR}/_ext/93166211/SMTP.o ../MicroChip/TCPIP_Stack/SMTP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNMP.o: ../MicroChip/TCPIP_Stack/SNMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNMP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNMP.o.d" -o ${OBJECTDIR}/_ext/93166211/SNMP.o ../MicroChip/TCPIP_Stack/SNMP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNMPv3.o: ../MicroChip/TCPIP_Stack/SNMPv3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNMPv3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNMPv3.o.d" -o ${OBJECTDIR}/_ext/93166211/SNMPv3.o ../MicroChip/TCPIP_Stack/SNMPv3.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNMPv3USM.o: ../MicroChip/TCPIP_Stack/SNMPv3USM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d" -o ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o ../MicroChip/TCPIP_Stack/SNMPv3USM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNTP.o: ../MicroChip/TCPIP_Stack/SNTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNTP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNTP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNTP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNTP.o.d" -o ${OBJECTDIR}/_ext/93166211/SNTP.o ../MicroChip/TCPIP_Stack/SNTP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SPIEEPROM.o: ../MicroChip/TCPIP_Stack/SPIEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d" -o ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o ../MicroChip/TCPIP_Stack/SPIEEPROM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SPIFlash.o: ../MicroChip/TCPIP_Stack/SPIFlash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIFlash.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIFlash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SPIFlash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SPIFlash.o.d" -o ${OBJECTDIR}/_ext/93166211/SPIFlash.o ../MicroChip/TCPIP_Stack/SPIFlash.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SPIRAM.o: ../MicroChip/TCPIP_Stack/SPIRAM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIRAM.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIRAM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SPIRAM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SPIRAM.o.d" -o ${OBJECTDIR}/_ext/93166211/SPIRAM.o ../MicroChip/TCPIP_Stack/SPIRAM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SSL.o: ../MicroChip/TCPIP_Stack/SSL.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SSL.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SSL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SSL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SSL.o.d" -o ${OBJECTDIR}/_ext/93166211/SSL.o ../MicroChip/TCPIP_Stack/SSL.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/StackTsk.o: ../MicroChip/TCPIP_Stack/StackTsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/StackTsk.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/StackTsk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/StackTsk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/StackTsk.o.d" -o ${OBJECTDIR}/_ext/93166211/StackTsk.o ../MicroChip/TCPIP_Stack/StackTsk.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/TCP.o: ../MicroChip/TCPIP_Stack/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/TCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/TCP.o.d" -o ${OBJECTDIR}/_ext/93166211/TCP.o ../MicroChip/TCPIP_Stack/TCP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o: ../MicroChip/TCPIP_Stack/TCPPerformanceTest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d" -o ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o ../MicroChip/TCPIP_Stack/TCPPerformanceTest.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Telnet.o: ../MicroChip/TCPIP_Stack/Telnet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Telnet.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Telnet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Telnet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Telnet.o.d" -o ${OBJECTDIR}/_ext/93166211/Telnet.o ../MicroChip/TCPIP_Stack/Telnet.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/TFTPc.o: ../MicroChip/TCPIP_Stack/TFTPc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/TFTPc.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/TFTPc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/TFTPc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/TFTPc.o.d" -o ${OBJECTDIR}/_ext/93166211/TFTPc.o ../MicroChip/TCPIP_Stack/TFTPc.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Tick.o: ../MicroChip/TCPIP_Stack/Tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Tick.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Tick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Tick.o.d" -o ${OBJECTDIR}/_ext/93166211/Tick.o ../MicroChip/TCPIP_Stack/Tick.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o: ../MicroChip/TCPIP_Stack/UART2TCPBridge.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d" -o ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o ../MicroChip/TCPIP_Stack/UART2TCPBridge.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/UDP.o: ../MicroChip/TCPIP_Stack/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/UDP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/UDP.o.d" -o ${OBJECTDIR}/_ext/93166211/UDP.o ../MicroChip/TCPIP_Stack/UDP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o: ../MicroChip/TCPIP_Stack/UDPPerformanceTest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d" -o ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o ../MicroChip/TCPIP_Stack/UDPPerformanceTest.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o: ../MicroChip/TCPIP_Stack/ZeroconfHelper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d" -o ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o ../MicroChip/TCPIP_Stack/ZeroconfHelper.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o: ../MicroChip/TCPIP_Stack/ZeroconfLinkLocal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d" -o ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o ../MicroChip/TCPIP_Stack/ZeroconfLinkLocal.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o: ../MicroChip/TCPIP_Stack/ZeroconfMulticastDNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d" -o ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o ../MicroChip/TCPIP_Stack/ZeroconfMulticastDNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DHCP.o: ../MicroChip/TCPIP_Stack/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DHCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DHCP.o.d" -o ${OBJECTDIR}/_ext/93166211/DHCP.o ../MicroChip/TCPIP_Stack/DHCP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DHCPs.o: ../MicroChip/TCPIP_Stack/DHCPs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCPs.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCPs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DHCPs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DHCPs.o.d" -o ${OBJECTDIR}/_ext/93166211/DHCPs.o ../MicroChip/TCPIP_Stack/DHCPs.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WF_Eint.o: ../MicroChip/TCPIP_Stack/WiFi/WF_Eint.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Eint.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Eint.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WF_Eint.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/388820833/WF_Eint.o.d" -o ${OBJECTDIR}/_ext/388820833/WF_Eint.o ../MicroChip/TCPIP_Stack/WiFi/WF_Eint.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o: ../MicroChip/TCPIP_Stack/WiFi/WF_pbkdf2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d" -o ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o ../MicroChip/TCPIP_Stack/WiFi/WF_pbkdf2.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o: ../MicroChip/TCPIP_Stack/WiFi/WFConnectionAlgorithm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o ../MicroChip/TCPIP_Stack/WiFi/WFConnectionAlgorithm.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConnectionManager.o: ../MicroChip/TCPIP_Stack/WiFi/WFConnectionManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o ../MicroChip/TCPIP_Stack/WiFi/WFConnectionManager.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o: ../MicroChip/TCPIP_Stack/WiFi/WFConnectionProfile.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o ../MicroChip/TCPIP_Stack/WiFi/WFConnectionProfile.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsole.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsole.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsole.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsole.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsole.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsole.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsole.o ../MicroChip/TCPIP_Stack/WiFi/WFConsole.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIfconfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIfconfig.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwconfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwconfig.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwpriv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwpriv.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgHandler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgHandler.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgs.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDataTxRx.o: ../MicroChip/TCPIP_Stack/WiFi/WFDataTxRx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o ../MicroChip/TCPIP_Stack/WiFi/WFDataTxRx.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDebugStrings.o: ../MicroChip/TCPIP_Stack/WiFi/WFDebugStrings.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o ../MicroChip/TCPIP_Stack/WiFi/WFDebugStrings.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFEasyConfig.o: ../MicroChip/TCPIP_Stack/WiFi/WFEasyConfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d" -o ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o ../MicroChip/TCPIP_Stack/WiFi/WFEasyConfig.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFEventHandler.o: ../MicroChip/TCPIP_Stack/WiFi/WFEventHandler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEventHandler.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d" -o ${OBJECTDIR}/_ext/388820833/WFEventHandler.o ../MicroChip/TCPIP_Stack/WiFi/WFEventHandler.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFInit.o: ../MicroChip/TCPIP_Stack/WiFi/WFInit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFInit.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFInit.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFInit.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFInit.o.d" -o ${OBJECTDIR}/_ext/388820833/WFInit.o ../MicroChip/TCPIP_Stack/WiFi/WFInit.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFPowerSave.o: ../MicroChip/TCPIP_Stack/WiFi/WFPowerSave.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFPowerSave.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d" -o ${OBJECTDIR}/_ext/388820833/WFPowerSave.o ../MicroChip/TCPIP_Stack/WiFi/WFPowerSave.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFScan.o: ../MicroChip/TCPIP_Stack/WiFi/WFScan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFScan.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFScan.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFScan.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFScan.o.d" -o ${OBJECTDIR}/_ext/388820833/WFScan.o ../MicroChip/TCPIP_Stack/WiFi/WFScan.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFTxPower.o: ../MicroChip/TCPIP_Stack/WiFi/WFTxPower.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFTxPower.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFTxPower.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFTxPower.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFTxPower.o.d" -o ${OBJECTDIR}/_ext/388820833/WFTxPower.o ../MicroChip/TCPIP_Stack/WiFi/WFTxPower.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WF_Spi.o: ../MicroChip/TCPIP_Stack/WiFi/WF_Spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Spi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WF_Spi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WF_Spi.o.d" -o ${OBJECTDIR}/_ext/388820833/WF_Spi.o ../MicroChip/TCPIP_Stack/WiFi/WF_Spi.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFDriverCom_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFDriverCom_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFDriverRaw_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFDriverRaw_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/388820833/WFMac_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFMac_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMac_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFMac_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFMac_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFMgmtMsg_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFMgmtMsg_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFParamMsg_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFParamMsg_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/Main.o: ../Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/Main.o.d" -o ${OBJECTDIR}/_ext/1472/Main.o ../Main.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/CustomHTTPApp.o: ../CustomHTTPApp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d" -o ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o ../CustomHTTPApp.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/WF_Config.o: ../WF_Config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/WF_Config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/WF_Config.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/WF_Config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/WF_Config.o.d" -o ${OBJECTDIR}/_ext/1472/WF_Config.o ../WF_Config.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o: ../WifiGDemoMPFSImg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d" -o ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o ../WifiGDemoMPFSImg.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/i2c_master.o: ../i2c_master.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/i2c_master.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/i2c_master.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/i2c_master.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/i2c_master.o.d" -o ${OBJECTDIR}/_ext/1472/i2c_master.o ../i2c_master.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/OpenADRClient.o: ../OpenADRClient.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/OpenADRClient.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/OpenADRClient.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/OpenADRClient.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/OpenADRClient.o.d" -o ${OBJECTDIR}/_ext/1472/OpenADRClient.o ../OpenADRClient.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/INTiMain.o: ../INTiMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/INTiMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/INTiMain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/INTiMain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/INTiMain.o.d" -o ${OBJECTDIR}/_ext/1472/INTiMain.o ../INTiMain.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/UARTiMain.o: ../UARTiMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/UARTiMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/UARTiMain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/UARTiMain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/UARTiMain.o.d" -o ${OBJECTDIR}/_ext/1472/UARTiMain.o ../UARTiMain.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/TimeMonitor.o: ../TimeMonitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/TimeMonitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/TimeMonitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/TimeMonitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/1472/TimeMonitor.o.d" -o ${OBJECTDIR}/_ext/1472/TimeMonitor.o ../TimeMonitor.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o: ../EPRI_UARTLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d" -o ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o ../EPRI_UARTLayer.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/app_UCM.o: ../app_UCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/app_UCM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/app_UCM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/app_UCM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/app_UCM.o.d" -o ${OBJECTDIR}/_ext/1472/app_UCM.o ../app_UCM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/Assert.o: ../Assert.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/Assert.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Assert.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Assert.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/Assert.o.d" -o ${OBJECTDIR}/_ext/1472/Assert.o ../Assert.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o: ../EPRI_SPILayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d" -o ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o ../EPRI_SPILayer.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/MCI_Common.o: ../MCI_Common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/MCI_Common.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/MCI_Common.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MCI_Common.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/MCI_Common.o.d" -o ${OBJECTDIR}/_ext/1472/MCI_Common.o ../MCI_Common.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o: ../LinkLayerMessaging.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d" -o ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o ../LinkLayerMessaging.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/BasicDR.o: ../BasicDR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/BasicDR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BasicDR.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/BasicDR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/BasicDR.o.d" -o ${OBJECTDIR}/_ext/1472/BasicDR.o ../BasicDR.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/debugging.o: ../debugging.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/debugging.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/debugging.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/debugging.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/debugging.o.d" -o ${OBJECTDIR}/_ext/1472/debugging.o ../debugging.c      -w
	
${OBJECTDIR}/_ext/1472/RESTAPI.o: ../RESTAPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/RESTAPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/RESTAPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/RESTAPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/RESTAPI.o.d" -o ${OBJECTDIR}/_ext/1472/RESTAPI.o ../RESTAPI.c      -w
	
${OBJECTDIR}/_ext/1472/DeviceInfo.o: ../DeviceInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/DeviceInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/DeviceInfo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/DeviceInfo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/DeviceInfo.o.d" -o ${OBJECTDIR}/_ext/1472/DeviceInfo.o ../DeviceInfo.c      -w
	
else
${OBJECTDIR}/_ext/93166211/Announce.o: ../MicroChip/TCPIP_Stack/Announce.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Announce.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Announce.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Announce.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Announce.o.d" -o ${OBJECTDIR}/_ext/93166211/Announce.o ../MicroChip/TCPIP_Stack/Announce.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ARCFOUR.o: ../MicroChip/TCPIP_Stack/ARCFOUR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARCFOUR.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ARCFOUR.o.d" -o ${OBJECTDIR}/_ext/93166211/ARCFOUR.o ../MicroChip/TCPIP_Stack/ARCFOUR.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ARP.o: ../MicroChip/TCPIP_Stack/ARP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ARP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ARP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ARP.o.d" -o ${OBJECTDIR}/_ext/93166211/ARP.o ../MicroChip/TCPIP_Stack/ARP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/AutoIP.o: ../MicroChip/TCPIP_Stack/AutoIP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/AutoIP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/AutoIP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/AutoIP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/AutoIP.o.d" -o ${OBJECTDIR}/_ext/93166211/AutoIP.o ../MicroChip/TCPIP_Stack/AutoIP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o: ../MicroChip/TCPIP_Stack/BerkeleyAPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o.d" -o ${OBJECTDIR}/_ext/93166211/BerkeleyAPI.o ../MicroChip/TCPIP_Stack/BerkeleyAPI.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/BigInt.o: ../MicroChip/TCPIP_Stack/BigInt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/BigInt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/BigInt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/BigInt.o.d" -o ${OBJECTDIR}/_ext/93166211/BigInt.o ../MicroChip/TCPIP_Stack/BigInt.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Delay.o: ../MicroChip/TCPIP_Stack/Delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Delay.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Delay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Delay.o.d" -o ${OBJECTDIR}/_ext/93166211/Delay.o ../MicroChip/TCPIP_Stack/Delay.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DNS.o: ../MicroChip/TCPIP_Stack/DNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DNS.o.d" -o ${OBJECTDIR}/_ext/93166211/DNS.o ../MicroChip/TCPIP_Stack/DNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DNSs.o: ../MicroChip/TCPIP_Stack/DNSs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNSs.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DNSs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DNSs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DNSs.o.d" -o ${OBJECTDIR}/_ext/93166211/DNSs.o ../MicroChip/TCPIP_Stack/DNSs.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DynDNS.o: ../MicroChip/TCPIP_Stack/DynDNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DynDNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DynDNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DynDNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DynDNS.o.d" -o ${OBJECTDIR}/_ext/93166211/DynDNS.o ../MicroChip/TCPIP_Stack/DynDNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/FileSystem.o: ../MicroChip/TCPIP_Stack/FileSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/FileSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/FileSystem.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/FileSystem.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/FileSystem.o.d" -o ${OBJECTDIR}/_ext/93166211/FileSystem.o ../MicroChip/TCPIP_Stack/FileSystem.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/FTP.o: ../MicroChip/TCPIP_Stack/FTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/FTP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/FTP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/FTP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/FTP.o.d" -o ${OBJECTDIR}/_ext/93166211/FTP.o ../MicroChip/TCPIP_Stack/FTP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Hashes.o: ../MicroChip/TCPIP_Stack/Hashes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Hashes.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Hashes.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Hashes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Hashes.o.d" -o ${OBJECTDIR}/_ext/93166211/Hashes.o ../MicroChip/TCPIP_Stack/Hashes.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Helpers.o: ../MicroChip/TCPIP_Stack/Helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Helpers.o.d" -o ${OBJECTDIR}/_ext/93166211/Helpers.o ../MicroChip/TCPIP_Stack/Helpers.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/HTTP2.o: ../MicroChip/TCPIP_Stack/HTTP2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/HTTP2.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/HTTP2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/HTTP2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/HTTP2.o.d" -o ${OBJECTDIR}/_ext/93166211/HTTP2.o ../MicroChip/TCPIP_Stack/HTTP2.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ICMP.o: ../MicroChip/TCPIP_Stack/ICMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ICMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ICMP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ICMP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ICMP.o.d" -o ${OBJECTDIR}/_ext/93166211/ICMP.o ../MicroChip/TCPIP_Stack/ICMP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/IP.o: ../MicroChip/TCPIP_Stack/IP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/IP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/IP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/IP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/IP.o.d" -o ${OBJECTDIR}/_ext/93166211/IP.o ../MicroChip/TCPIP_Stack/IP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/LCDBlocking.o: ../MicroChip/TCPIP_Stack/LCDBlocking.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/LCDBlocking.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/LCDBlocking.o.d" -o ${OBJECTDIR}/_ext/93166211/LCDBlocking.o ../MicroChip/TCPIP_Stack/LCDBlocking.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/MPFS2.o: ../MicroChip/TCPIP_Stack/MPFS2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/MPFS2.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/MPFS2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/MPFS2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/MPFS2.o.d" -o ${OBJECTDIR}/_ext/93166211/MPFS2.o ../MicroChip/TCPIP_Stack/MPFS2.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/NBNS.o: ../MicroChip/TCPIP_Stack/NBNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/NBNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/NBNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/NBNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/NBNS.o.d" -o ${OBJECTDIR}/_ext/93166211/NBNS.o ../MicroChip/TCPIP_Stack/NBNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Random.o: ../MicroChip/TCPIP_Stack/Random.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Random.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Random.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Random.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Random.o.d" -o ${OBJECTDIR}/_ext/93166211/Random.o ../MicroChip/TCPIP_Stack/Random.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Reboot.o: ../MicroChip/TCPIP_Stack/Reboot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Reboot.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Reboot.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Reboot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Reboot.o.d" -o ${OBJECTDIR}/_ext/93166211/Reboot.o ../MicroChip/TCPIP_Stack/Reboot.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/RSA.o: ../MicroChip/TCPIP_Stack/RSA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/RSA.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/RSA.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/RSA.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/RSA.o.d" -o ${OBJECTDIR}/_ext/93166211/RSA.o ../MicroChip/TCPIP_Stack/RSA.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SMTP.o: ../MicroChip/TCPIP_Stack/SMTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SMTP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SMTP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SMTP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SMTP.o.d" -o ${OBJECTDIR}/_ext/93166211/SMTP.o ../MicroChip/TCPIP_Stack/SMTP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNMP.o: ../MicroChip/TCPIP_Stack/SNMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNMP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNMP.o.d" -o ${OBJECTDIR}/_ext/93166211/SNMP.o ../MicroChip/TCPIP_Stack/SNMP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNMPv3.o: ../MicroChip/TCPIP_Stack/SNMPv3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNMPv3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNMPv3.o.d" -o ${OBJECTDIR}/_ext/93166211/SNMPv3.o ../MicroChip/TCPIP_Stack/SNMPv3.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNMPv3USM.o: ../MicroChip/TCPIP_Stack/SNMPv3USM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNMPv3USM.o.d" -o ${OBJECTDIR}/_ext/93166211/SNMPv3USM.o ../MicroChip/TCPIP_Stack/SNMPv3USM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SNTP.o: ../MicroChip/TCPIP_Stack/SNTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNTP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SNTP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SNTP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SNTP.o.d" -o ${OBJECTDIR}/_ext/93166211/SNTP.o ../MicroChip/TCPIP_Stack/SNTP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SPIEEPROM.o: ../MicroChip/TCPIP_Stack/SPIEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SPIEEPROM.o.d" -o ${OBJECTDIR}/_ext/93166211/SPIEEPROM.o ../MicroChip/TCPIP_Stack/SPIEEPROM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SPIFlash.o: ../MicroChip/TCPIP_Stack/SPIFlash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIFlash.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIFlash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SPIFlash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SPIFlash.o.d" -o ${OBJECTDIR}/_ext/93166211/SPIFlash.o ../MicroChip/TCPIP_Stack/SPIFlash.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SPIRAM.o: ../MicroChip/TCPIP_Stack/SPIRAM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIRAM.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SPIRAM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SPIRAM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SPIRAM.o.d" -o ${OBJECTDIR}/_ext/93166211/SPIRAM.o ../MicroChip/TCPIP_Stack/SPIRAM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/SSL.o: ../MicroChip/TCPIP_Stack/SSL.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/SSL.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/SSL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/SSL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/SSL.o.d" -o ${OBJECTDIR}/_ext/93166211/SSL.o ../MicroChip/TCPIP_Stack/SSL.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/StackTsk.o: ../MicroChip/TCPIP_Stack/StackTsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/StackTsk.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/StackTsk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/StackTsk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/StackTsk.o.d" -o ${OBJECTDIR}/_ext/93166211/StackTsk.o ../MicroChip/TCPIP_Stack/StackTsk.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/TCP.o: ../MicroChip/TCPIP_Stack/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/TCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/TCP.o.d" -o ${OBJECTDIR}/_ext/93166211/TCP.o ../MicroChip/TCPIP_Stack/TCP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o: ../MicroChip/TCPIP_Stack/TCPPerformanceTest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o.d" -o ${OBJECTDIR}/_ext/93166211/TCPPerformanceTest.o ../MicroChip/TCPIP_Stack/TCPPerformanceTest.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Telnet.o: ../MicroChip/TCPIP_Stack/Telnet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Telnet.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Telnet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Telnet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Telnet.o.d" -o ${OBJECTDIR}/_ext/93166211/Telnet.o ../MicroChip/TCPIP_Stack/Telnet.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/TFTPc.o: ../MicroChip/TCPIP_Stack/TFTPc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/TFTPc.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/TFTPc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/TFTPc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/TFTPc.o.d" -o ${OBJECTDIR}/_ext/93166211/TFTPc.o ../MicroChip/TCPIP_Stack/TFTPc.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/Tick.o: ../MicroChip/TCPIP_Stack/Tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/Tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/Tick.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/Tick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/Tick.o.d" -o ${OBJECTDIR}/_ext/93166211/Tick.o ../MicroChip/TCPIP_Stack/Tick.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o: ../MicroChip/TCPIP_Stack/UART2TCPBridge.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o.d" -o ${OBJECTDIR}/_ext/93166211/UART2TCPBridge.o ../MicroChip/TCPIP_Stack/UART2TCPBridge.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/UDP.o: ../MicroChip/TCPIP_Stack/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/UDP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/UDP.o.d" -o ${OBJECTDIR}/_ext/93166211/UDP.o ../MicroChip/TCPIP_Stack/UDP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o: ../MicroChip/TCPIP_Stack/UDPPerformanceTest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o.d" -o ${OBJECTDIR}/_ext/93166211/UDPPerformanceTest.o ../MicroChip/TCPIP_Stack/UDPPerformanceTest.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o: ../MicroChip/TCPIP_Stack/ZeroconfHelper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o.d" -o ${OBJECTDIR}/_ext/93166211/ZeroconfHelper.o ../MicroChip/TCPIP_Stack/ZeroconfHelper.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o: ../MicroChip/TCPIP_Stack/ZeroconfLinkLocal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o.d" -o ${OBJECTDIR}/_ext/93166211/ZeroconfLinkLocal.o ../MicroChip/TCPIP_Stack/ZeroconfLinkLocal.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o: ../MicroChip/TCPIP_Stack/ZeroconfMulticastDNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o.d" -o ${OBJECTDIR}/_ext/93166211/ZeroconfMulticastDNS.o ../MicroChip/TCPIP_Stack/ZeroconfMulticastDNS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DHCP.o: ../MicroChip/TCPIP_Stack/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCP.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCP.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DHCP.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DHCP.o.d" -o ${OBJECTDIR}/_ext/93166211/DHCP.o ../MicroChip/TCPIP_Stack/DHCP.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/93166211/DHCPs.o: ../MicroChip/TCPIP_Stack/DHCPs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/93166211" 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCPs.o.d 
	@${RM} ${OBJECTDIR}/_ext/93166211/DHCPs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/93166211/DHCPs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/93166211/DHCPs.o.d" -o ${OBJECTDIR}/_ext/93166211/DHCPs.o ../MicroChip/TCPIP_Stack/DHCPs.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WF_Eint.o: ../MicroChip/TCPIP_Stack/WiFi/WF_Eint.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Eint.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Eint.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WF_Eint.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/388820833/WF_Eint.o.d" -o ${OBJECTDIR}/_ext/388820833/WF_Eint.o ../MicroChip/TCPIP_Stack/WiFi/WF_Eint.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o: ../MicroChip/TCPIP_Stack/WiFi/WF_pbkdf2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o.d" -o ${OBJECTDIR}/_ext/388820833/WF_pbkdf2.o ../MicroChip/TCPIP_Stack/WiFi/WF_pbkdf2.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o: ../MicroChip/TCPIP_Stack/WiFi/WFConnectionAlgorithm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConnectionAlgorithm.o ../MicroChip/TCPIP_Stack/WiFi/WFConnectionAlgorithm.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConnectionManager.o: ../MicroChip/TCPIP_Stack/WiFi/WFConnectionManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConnectionManager.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConnectionManager.o ../MicroChip/TCPIP_Stack/WiFi/WFConnectionManager.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o: ../MicroChip/TCPIP_Stack/WiFi/WFConnectionProfile.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConnectionProfile.o ../MicroChip/TCPIP_Stack/WiFi/WFConnectionProfile.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsole.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsole.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsole.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsole.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsole.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsole.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsole.o ../MicroChip/TCPIP_Stack/WiFi/WFConsole.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIfconfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleIfconfig.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIfconfig.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwconfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleIwconfig.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwconfig.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwpriv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleIwpriv.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleIwpriv.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgHandler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgHandler.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgHandler.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o: ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o.d" -o ${OBJECTDIR}/_ext/388820833/WFConsoleMsgs.o ../MicroChip/TCPIP_Stack/WiFi/WFConsoleMsgs.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDataTxRx.o: ../MicroChip/TCPIP_Stack/WiFi/WFDataTxRx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDataTxRx.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDataTxRx.o ../MicroChip/TCPIP_Stack/WiFi/WFDataTxRx.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDebugStrings.o: ../MicroChip/TCPIP_Stack/WiFi/WFDebugStrings.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDebugStrings.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDebugStrings.o ../MicroChip/TCPIP_Stack/WiFi/WFDebugStrings.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFEasyConfig.o: ../MicroChip/TCPIP_Stack/WiFi/WFEasyConfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFEasyConfig.o.d" -o ${OBJECTDIR}/_ext/388820833/WFEasyConfig.o ../MicroChip/TCPIP_Stack/WiFi/WFEasyConfig.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFEventHandler.o: ../MicroChip/TCPIP_Stack/WiFi/WFEventHandler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFEventHandler.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFEventHandler.o.d" -o ${OBJECTDIR}/_ext/388820833/WFEventHandler.o ../MicroChip/TCPIP_Stack/WiFi/WFEventHandler.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFInit.o: ../MicroChip/TCPIP_Stack/WiFi/WFInit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFInit.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFInit.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFInit.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFInit.o.d" -o ${OBJECTDIR}/_ext/388820833/WFInit.o ../MicroChip/TCPIP_Stack/WiFi/WFInit.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFPowerSave.o: ../MicroChip/TCPIP_Stack/WiFi/WFPowerSave.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFPowerSave.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFPowerSave.o.d" -o ${OBJECTDIR}/_ext/388820833/WFPowerSave.o ../MicroChip/TCPIP_Stack/WiFi/WFPowerSave.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFScan.o: ../MicroChip/TCPIP_Stack/WiFi/WFScan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFScan.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFScan.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFScan.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFScan.o.d" -o ${OBJECTDIR}/_ext/388820833/WFScan.o ../MicroChip/TCPIP_Stack/WiFi/WFScan.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFTxPower.o: ../MicroChip/TCPIP_Stack/WiFi/WFTxPower.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFTxPower.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFTxPower.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFTxPower.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFTxPower.o.d" -o ${OBJECTDIR}/_ext/388820833/WFTxPower.o ../MicroChip/TCPIP_Stack/WiFi/WFTxPower.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WF_Spi.o: ../MicroChip/TCPIP_Stack/WiFi/WF_Spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WF_Spi.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WF_Spi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WF_Spi.o.d" -o ${OBJECTDIR}/_ext/388820833/WF_Spi.o ../MicroChip/TCPIP_Stack/WiFi/WF_Spi.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFDriverCom_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDriverCom_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFDriverCom_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFDriverRaw_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFDriverRaw_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFDriverRaw_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/388820833/WFMac_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFMac_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMac_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFMac_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFMac_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFMac_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFMgmtMsg_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFMgmtMsg_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFMgmtMsg_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o: ../MicroChip/TCPIP_Stack/WiFi/WFParamMsg_24G.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/388820833" 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d 
	@${RM} ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o.d" -o ${OBJECTDIR}/_ext/388820833/WFParamMsg_24G.o ../MicroChip/TCPIP_Stack/WiFi/WFParamMsg_24G.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/Main.o: ../Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/Main.o.d" -o ${OBJECTDIR}/_ext/1472/Main.o ../Main.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/CustomHTTPApp.o: ../CustomHTTPApp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/CustomHTTPApp.o.d" -o ${OBJECTDIR}/_ext/1472/CustomHTTPApp.o ../CustomHTTPApp.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/WF_Config.o: ../WF_Config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/WF_Config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/WF_Config.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/WF_Config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/WF_Config.o.d" -o ${OBJECTDIR}/_ext/1472/WF_Config.o ../WF_Config.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o: ../WifiGDemoMPFSImg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o.d" -o ${OBJECTDIR}/_ext/1472/WifiGDemoMPFSImg.o ../WifiGDemoMPFSImg.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/i2c_master.o: ../i2c_master.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/i2c_master.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/i2c_master.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/i2c_master.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/i2c_master.o.d" -o ${OBJECTDIR}/_ext/1472/i2c_master.o ../i2c_master.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/OpenADRClient.o: ../OpenADRClient.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/OpenADRClient.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/OpenADRClient.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/OpenADRClient.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/OpenADRClient.o.d" -o ${OBJECTDIR}/_ext/1472/OpenADRClient.o ../OpenADRClient.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/INTiMain.o: ../INTiMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/INTiMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/INTiMain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/INTiMain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/INTiMain.o.d" -o ${OBJECTDIR}/_ext/1472/INTiMain.o ../INTiMain.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/UARTiMain.o: ../UARTiMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/UARTiMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/UARTiMain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/UARTiMain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/UARTiMain.o.d" -o ${OBJECTDIR}/_ext/1472/UARTiMain.o ../UARTiMain.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/TimeMonitor.o: ../TimeMonitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/TimeMonitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/TimeMonitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/TimeMonitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -MMD -MF "${OBJECTDIR}/_ext/1472/TimeMonitor.o.d" -o ${OBJECTDIR}/_ext/1472/TimeMonitor.o ../TimeMonitor.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o: ../EPRI_UARTLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o.d" -o ${OBJECTDIR}/_ext/1472/EPRI_UARTLayer.o ../EPRI_UARTLayer.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/app_UCM.o: ../app_UCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/app_UCM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/app_UCM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/app_UCM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/app_UCM.o.d" -o ${OBJECTDIR}/_ext/1472/app_UCM.o ../app_UCM.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/Assert.o: ../Assert.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/Assert.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Assert.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Assert.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/Assert.o.d" -o ${OBJECTDIR}/_ext/1472/Assert.o ../Assert.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o: ../EPRI_SPILayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o.d" -o ${OBJECTDIR}/_ext/1472/EPRI_SPILayer.o ../EPRI_SPILayer.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/MCI_Common.o: ../MCI_Common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/MCI_Common.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/MCI_Common.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MCI_Common.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/MCI_Common.o.d" -o ${OBJECTDIR}/_ext/1472/MCI_Common.o ../MCI_Common.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o: ../LinkLayerMessaging.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o.d" -o ${OBJECTDIR}/_ext/1472/LinkLayerMessaging.o ../LinkLayerMessaging.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/BasicDR.o: ../BasicDR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/BasicDR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BasicDR.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/BasicDR.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/BasicDR.o.d" -o ${OBJECTDIR}/_ext/1472/BasicDR.o ../BasicDR.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -w
	
${OBJECTDIR}/_ext/1472/debugging.o: ../debugging.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/debugging.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/debugging.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/debugging.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/debugging.o.d" -o ${OBJECTDIR}/_ext/1472/debugging.o ../debugging.c      -w
	
${OBJECTDIR}/_ext/1472/RESTAPI.o: ../RESTAPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/RESTAPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/RESTAPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/RESTAPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/RESTAPI.o.d" -o ${OBJECTDIR}/_ext/1472/RESTAPI.o ../RESTAPI.c      -w
	
${OBJECTDIR}/_ext/1472/DeviceInfo.o: ../DeviceInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/DeviceInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/DeviceInfo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/DeviceInfo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O1 -I".." -I"../Microchip/Include" -I"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/include/peripheral" -I"../Configs" -I"." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/DeviceInfo.o.d" -o ${OBJECTDIR}/_ext/1472/DeviceInfo.o ../DeviceInfo.c      -w
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_ICD3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    -mreserve=boot@0x1FC02000:0x1FC02FEF -mreserve=boot@0x1FC02000:0x1FC024FF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,--no-code-in-dinit,--no-dinit-in-serial-mem,-L"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/lib",-L".",-Map="${DISTDIR}/Aztec.X.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--no-code-in-dinit,--no-dinit-in-serial-mem,-L"../../../../../../../Program Files (x86)/Microchip/mplabc32/v2.02/pic32mx/lib",-L".",-Map="${DISTDIR}/Aztec.X.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Aztec.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
