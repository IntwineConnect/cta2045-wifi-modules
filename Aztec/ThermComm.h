#ifndef _THERMCOMM_H
#define _THERMCOMM_H

#include "IntwineModule.h"

#ifdef INTWINE_PROGRAMMABLE_THERMOSTAT

/*This Header File gives the Definition of the functions*/
//#include "p24fxxxx.h"
#include "ThermMsgs.h"

//PPS outputs

/*  #define SDO2_OUT     10 */
/*  #define SCK2_OUT     11 */
/*  #define SS2OUT       12 */

//PPS Inputs
 //#define SDI          SDI2
 //#define INT          INT1

//definitions of control and status registers
/*  #define SPISTAT       SPI2STAT       */
/*  #define SPIBUF        SPI2BUF        */
/*  #define SPICON1       SPI2CON1       */
/*  #define SPICON2       SPI2CON2       */
/*  #define SPICON1bits   SPI2CON1bits   */
/*  #define SPISTATbits   SPI2STATbits   */
/*  #define SPIINTENbits  IEC2bits       */
/*  #define SPIINTFLGbits IFS2bits       */
/*  #define SPIIF         SPI2IF         */
/*  #define SPIIE         SPI2IE         */
/*  #define SPIM_MODE16    0             */
/*  #define SPIM_SMP       0             */
/*  #define SPIM_CKE       1             */
/*  #define SPIM_CKP       0             */
/*  #define SPIM_SSEN      1             */
/*  #define SPIM_PPRE     (unsigned)0    */
/*  #define SPIM_SPRE     (unsigned)0    */
/*  #define SLAVE_SELECT   PORTGbits.RG6 */
//#define attention_tris TRISBbits.TRISB2
//#define attention      PORTBbits.RB2

typedef enum
{
  MESSAGE_ENGINE_IDLE                      = 0,      // nothing happening
  MESSAGE_ENGINE_CMD_EXPECT_A_RESPONSE     = 1,      // ready to send cmd w/response
  MESSAGE_ENGINE_CMD_NO_RESPONSE_EXPECTED  = 2,      // ready to send cmd w/o response

  MESSAGE_ENGINE_CMD_SENDING1              = 11,     // sending bytes now ..
  MESSAGE_ENGINE_CMD_SENDING2              = 12,     // sending bytes now ..

  MESSAGE_ENGINE_RESPONSE_READY            = 21,     // wait for/verify response
  
} MESSAGE_ENGINE_STATE;

extern MESSAGE_ENGINE_STATE iProcessThermCmdsInUse;

// Therm response handling state values
#define MSG_RESP_DONT_SEND			0
#define MSG_RESP_SEND_IF_CHANGED	1
#define MSG_RESP_FORCE_SEND			2

// Therm response handling states
#define MSG_RSP_STATE_RADIO_SLOT			0
#define MSG_RSP_STATE_MODEL_NUM      		1
#define MSG_RSP_STATE_MODE_CONFIG      		2
#define MSG_RSP_STATE_CUR_ERROR      		3
#define MSG_RSP_STATE_THERM_OP_MODE      	4
#define MSG_RSP_STATE_FAN_OP_MODE      		5
#define MSG_RSP_STATE_LED_STATE      		6
#define MSG_RSP_STATE_USER_BTN_PUSH      	7
#define MSG_RSP_STATE_USER_BTN_SCR_REQ      8
#define MSG_RSP_STATE_PRG_DATA      		9
#define MSG_RSP_STATE_CUR_TEMP      		10
#define MSG_RSP_STATE_TEMP_ABS_SETPOINTS    11
#define MSG_RSP_STATE_TARGETS      			12
#define MSG_RSP_STATE_CUR_HUMIDITY      	13
#define MSG_RSP_STATE_HUM_SETPOINT      	14
#define MSG_RSP_STATE_CUR_TIME      		15
#define MSG_RSP_STATE_CUR_DAY      			16
#define MSG_RSP_STATE_TEMP_TARG_SETBACK     17
#define MSG_RSP_STATE_TEMP_OFFSET      		18
#define MSG_RSP_STATE_TEMP_OVERRIDE      	19
#define MSG_RSP_STATE_REM_TEMP_VAL      	20
#define MSG_RSP_STATE_REM_TEMP_STATUS      	21
#define MSG_RSP_STATE_RADIO_BUS_MSG      	22
#define MSG_RSP_STATE_TEMP_TARGETS 			23

#define MSG_RSP_STATE_COUNT					24 // must equal total of state defines above

// Thermostat response Command and SubCommand combo for switch statement to process thermostat responses
#define MSG_RSP_IDLE                    0x0000  // MUST BE 0000, there are memsets to 0
#define MSG_RSP_RADIO_TEST				0x1234
#define MSG_RSP_RADIO_SLOT				0x5001
#define MSG_RSP_MODEL_NUM      			0x5003
#define MSG_RSP_MODE_CONFIG      		0x5004
#define MSG_RSP_CUR_ERROR      			0x5005
#define MSG_RSP_THERM_OP_MODE      		0x5008
#define MSG_RSP_FAN_OP_MODE      		0x5009
#define MSG_RSP_LED_STATE      			0x5020
#define MSG_RSP_PRICING_MESSAGE    		0x5034
#define MSG_RSP_USER_BTN_PUSH      		0x5048
#define MSG_RSP_USER_BTN_SCR_REQ      	0x5049
#define MSG_RSP_PRG_DATA      			0x5050
#define MSG_RSP_WRITE_PRG_DATA          0x5051
#define MSG_RSP_BATT_STATE	 			0x5054 // Not used Yet
#define MSG_RSP_REMOTE_CONTROL          0x5055
#define MSG_RSP_RADIO_BUS_MSG      		0x50F0
#define MSG_RSP_CUR_TEMP      			0x0601
#define MSG_RSP_CUR_TIME      			0x0640
#define MSG_RSP_CUR_DAY      			0x0641
#define MSG_RSP_TEMP_ABS_SETPOINTS      0x2001
#define MSG_RSP_TARGETS      			0x2011
#define MSG_RSP_TEMP_TARGETS      		0x2020
#define MSG_RSP_CUR_HUMIDITY      		0x0F01
#define MSG_RSP_HUM_SETPOINT      		0x2101
#define MSG_RSP_TEMP_TARG_SETBACK      	0x0A37
#define MSG_RSP_TEMP_OFFSET      		0x0A45
#define MSG_RSP_TEMP_OVERRIDE      		0x0A46
#define MSG_RSP_REM_TEMP_VAL      		0x0A57
#define MSG_RSP_REM_TEMP_STATUS      	0x0A58
#define MSG_RSP_DATA_LOG 				0x0A66
#define MSG_RSP_RSE_EVENT_LOG 			0x0A67

#define MSG_NO_RSP_USER_MSG_AREA_ENABLE 0x5030
#define MSG_NO_RSP_WRITE_USER_MSG_AREA  0x5031
#define MSG_NO_RSP_MSG_ANN_ENABLE       0x5038
#define MSG_NO_RSP_ENABLE_LCD_BACKLIGHT 0x5053
#define MSG_NO_RSP_DIS_TEMP_PROG_HOLD   0x0A63
#define MSG_NO_RSP_ENA_TEMP_PROG_HOLD   0x0A64

  
void SPIInit(); //Initializes the spi module
void ThermoInit(void);
void CheckSPIErrorFlag(void);
void SendToTherm(SPI_CMD_RESP_DATA cmdMsg);
void ProcessAndSendToTherm(unsigned char CmdBytes[], int nByteCount);
void ProcessThermCmds();
void ProcessThermRsps();
char Check_If_Send(int Command);

SPI_CMD_RESP_DATA UndoByteStuffing(SPI_CMD_RESP_DATA Resp);
char VerifyChecksum(SPI_CMD_RESP_DATA Resp);
void ThermGetPricingMessageBuffer(char *user_buf, int user_len);

int ThermCheckLastRsp(int check_command);

#endif

#endif
