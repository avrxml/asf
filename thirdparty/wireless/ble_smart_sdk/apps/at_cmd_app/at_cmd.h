#ifndef __AT_CMD_H__
#define __AT_CMD_H__


/***************************
INCLUDES
***************************/
//#include "uart_config.h"
#include <asf.h>
#include <string.h>

/***************************
DEFINES
***************************/
#define AT_HEADER					"AT+"

#define AT_MAX_COMMANDS_COUNT		(4)
//#define AT_MAX_CFG_CMD_COUNT		(3)

#define AT_MAX_PARAMS_COUNT			(1)
//#define AT_MIN_CMD_LENGTH			(4)
#define AT_MAX_CMD_LENGTH			(15)
#define AT_MAX_PARAM_LENGTH			(1)

#define AT_MAX_RX_BUFF_SIZE 		((AT_MAX_PARAM_LENGTH * AT_MAX_PARAMS_COUNT) + AT_MAX_CMD_LENGTH + sizeof(AT_HEADER))

/***************************
MACROS
***************************/
#define AT_SEND_ERROR(val,cmdTxt)	printf("\r\n+ERROR: %d\r\n,%s\r\n", val, (char *)cmdTxt)
#define AT_SEND_OK(cmdTxt)				printf("\r\n+OK\r\n,%s\r\n", (char *)cmdTxt)


/***************************
DATATYPES
***************************/
enum at_cmd_status
{
	AT_CMD_STATUS_IDLE	= 0,
	AT_CMD_STATUS_CHECK,
	AT_CMD_STATUS_RUN
};

enum at_cmd_index {
	AT_INDEX_INIT   = 0,
	AT_INDEX_SCAN,
	AT_INDEX_STOP
};

typedef struct at_cmd_parse_chrs
{
	uint8_t u8Header_sep[2];
	//uint8 u8Num_ofParams_sep[2];
	//uint8 u8Params_enc_Start_sep[2];
	//uint8 u8Params_enc_End_sep[2];
	//uint8 u8Params_sep[2];
	//uint8 u8Help_param_sep[3];
	//uint8 in_param_sep[2];
}tstrAt_cmd_parse_chrs;

typedef struct cmd
{
	uint8_t au8Cmd[AT_MAX_CMD_LENGTH];
	uint8_t u8NumOfParameters;
	uint8_t au8ParamsList[AT_MAX_PARAMS_COUNT][AT_MAX_PARAM_LENGTH];
}tstrAt_cmd_content;

typedef int8_t (*tpf_at_cmd_handler)(tstrAt_cmd_content, void *, uint8_t);

typedef struct str_At_Cmd
{
	uint8_t au8AtCMD[AT_MAX_CMD_LENGTH];
	tpf_at_cmd_handler at_cmd_handler;
	uint8_t u8NoOfParamters;
}strAtCMD_Handler;


/***************************
FUNCTIONS DECLARATIONS
***************************/
//int8_t parse_cmd(uint8_t *, tstrAt_cmd_content *, tstrAt_cmd_parse_chrs);
int8_t get_index_cmdHandler(tstrAt_cmd_content, strAtCMD_Handler *);

void init_at_cmd_handler(strAtCMD_Handler *);
void print_cmd_handler_arr(strAtCMD_Handler *);
/***************************
HANDLERS
***************************/
int8_t atCmd_Reset_Handler(tstrAt_cmd_content, void *, uint8_t);
int8_t atCmd_Scan_Handler (tstrAt_cmd_content, void *, uint8_t);
int8_t atCmd_Stop_Handler (tstrAt_cmd_content, void *, uint8_t);
int8_t atCmd_ConfigDefault_Handler(tstrAt_cmd_content, void*, uint8_t);

#endif //#ifndef __AT_CMD_H__
