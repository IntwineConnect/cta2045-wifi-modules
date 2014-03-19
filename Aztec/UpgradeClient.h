int UpgradeTCPClient(void);
void Full_Byte_Program(unsigned long Dst, unsigned char byte);
#define APP_FW_MAJOR 4
#define APP_FW_MINOR 2
#define APP_FW_BUILD 0

extern enum _UpgradeTCPEnum
{
	UTSM_HOME = 0,
	UTSM_SOCKET_OBTAINED,
/*     #if defined (STACK_USE_SSL_CLIENT) */
/*     UTSM_SSL_START,                    */
/*     UTSM_SSL_HANDSHAKE,                */
/*     #endif                             */
	UTSM_WRITE_SOCKET,
	UTSM_PROCESS_RESPONSE,
	UTSM_FINALIZE_FW,
	UTSM_VERIFY_FW,
	UTSM_ACK,
	UTSM_NAK,
	UTSM_DONE
} UpgradeTCPState; 

int isUpgradeActive(void);
void showFirmwareRev(char *buf);

