#include "at_cmd.h"
#include "at_ble_api.h"
 
static strAtCMD_Handler gaAt_CMD_handler_fn[AT_MAX_COMMANDS_COUNT]=	/*Handler and #ofParamters*/
{
	{"INIT"			,atCmd_Scan_Handler,0},
	{"SCAN"			,atCmd_Scan_Handler,0},
	{"STOP"			,atCmd_Scan_Handler,0},
	{"CFG_DEF"		,atCmd_ConfigDefault_Handler,0},
};

void init_at_cmd_handler(strAtCMD_Handler *pstrAtCMD_Handler)
{
//	uint8 u8LoopCntr=0;
//	for(u8LoopCntr=0; u8LoopCntr<AT_MAX_COMMANDS_COUNT;u8LoopCntr++) {
//		pstrAtCMD_Handler[u8LoopCntr] = gaAt_CMD_handler_fn[u8LoopCntr];
//	}
	//The following steps are bad and shall be changed 
	//another time and if there is large number of commands
	uint8_t u8LoopCntr=0;
	{
		strAtCMD_Handler dummy = {"RESET"			,atCmd_Reset_Handler,0};
		gaAt_CMD_handler_fn[u8LoopCntr] = dummy;
		pstrAtCMD_Handler[u8LoopCntr++] = dummy;
	}
	{
		strAtCMD_Handler dummy = {"SCAN"			,atCmd_Scan_Handler,0};
		gaAt_CMD_handler_fn[u8LoopCntr] = dummy;
		pstrAtCMD_Handler[u8LoopCntr++] = dummy;
	}
	{
		strAtCMD_Handler dummy = {"STOP"			,atCmd_Stop_Handler,0};
		gaAt_CMD_handler_fn[u8LoopCntr] = dummy;
		pstrAtCMD_Handler[u8LoopCntr++] = dummy;
	} 
	{
		strAtCMD_Handler dummy = {"CFG_DEF"			,atCmd_ConfigDefault_Handler,0};
		gaAt_CMD_handler_fn[u8LoopCntr] = dummy;
		pstrAtCMD_Handler[u8LoopCntr++] = dummy;
	} 
	gaAt_CMD_handler_fn[0] = gaAt_CMD_handler_fn[0]; //to ignore warning only
}

void print_cmd_handler_arr(strAtCMD_Handler *astrAtCMD_Handler)
{
	uint8_t loopCntr=0;
	printf("\r\n%-12s\t%-20s\t%s\r\n","#","CMD","Handler Ptr.");
	printf("-------------------------------------------------------\r\n");
	for(loopCntr=0; loopCntr<AT_MAX_COMMANDS_COUNT;loopCntr++) {
		printf("%02d%-10s\t%-20s\t%p\r\n",loopCntr+1," ",
		astrAtCMD_Handler[loopCntr].au8AtCMD,astrAtCMD_Handler[loopCntr].at_cmd_handler);
	}
	printf("-------------------------------------------------------\r\n\r\n");
}

int8_t get_index_cmdHandler(tstrAt_cmd_content strCmd, strAtCMD_Handler *atCMD_Handler)
{
	int8_t ret = -1;
	uint8_t loopCntr;
	uint8_t cmdLength = strlen((const char *)strCmd.au8Cmd);
	printf("Searching for CMD handler...\r\n");
	for(loopCntr = 0; loopCntr<AT_MAX_COMMANDS_COUNT; loopCntr++) {
		if(cmdLength != strlen((const char *)atCMD_Handler[loopCntr].au8AtCMD))
			continue;
		if(!memcmp(strCmd.au8Cmd,atCMD_Handler[loopCntr].au8AtCMD,cmdLength)) {
			/*pf_at_cmd_handler = atCMD_Handler[loopCntr].at_cmd_handler;*/
			ret  = loopCntr; 
			break;
		}
	}
	
	if(AT_MAX_COMMANDS_COUNT > loopCntr) {
		/*ret = pf_at_cmd_handler(strCmd);*/
	} else {
		ret = -1;
	}
	
	return ret;
}

int8_t atCmd_Reset_Handler(tstrAt_cmd_content data, void* moreData, uint8_t useStoredValue)
{
	printf("Reset Handler\r\n");
	return (at_ble_status_t)at_ble_ll_reset();
}

int8_t atCmd_ConfigDefault_Handler(tstrAt_cmd_content data, void* moreData, uint8_t useStoredValue)
{
	at_ble_dev_config_t stDevConfig;
	printf("Config Handler\r\n");
	
	memset((uint8_t *)&stDevConfig, 0, sizeof(stDevConfig));
	
	//Set device configuration
    ////Device role
    stDevConfig.role = AT_BLE_ROLE_ALL;
    ////device renew duration
    stDevConfig.renew_dur = AT_RENEW_DUR_VAL_MIN;
    ////device address type
    //stDevConfig.address = 0;
    ////Attributes
    stDevConfig.att_cfg.b2NamePerm = AT_BLE_WRITE_DISABLE;
    stDevConfig.att_cfg.b2AppearancePerm = AT_BLE_WRITE_DISABLE;
    stDevConfig.att_cfg.b1EnableSpcs = 0;
    stDevConfig.att_cfg.b1EnableServiceChanged = 0;
    stDevConfig.att_cfg.b2Rfu = AT_BLE_WRITE_DISABLE;
    ////Handles
    stDevConfig.gap_start_hdl = AT_BLE_AUTO_ALLOC_HANDLE;
    stDevConfig.gatt_start_hdl = AT_BLE_AUTO_ALLOC_HANDLE;
    ////MTU
    stDevConfig.max_mtu = AT_MTU_VAL_RECOMMENDED;
	
	return (at_ble_status_t)at_ble_set_dev_config(&stDevConfig);
}

int8_t atCmd_Scan_Handler(tstrAt_cmd_content data, void* moreData, uint8_t useStoredValue)
{
#define AT_BLE_INQ_SCAN_INTV    (0x0140)
#define AT_BLE_INQ_SCAN_WIND    (0x0050)
	printf("Scan Handler\r\n");
	return (at_ble_status_t)at_ble_scan_start(AT_BLE_INQ_SCAN_INTV, AT_BLE_INQ_SCAN_WIND, 0, AT_BLE_SCAN_ACTIVE, AT_BLE_SCAN_OBSERVER_MODE, false, true);
}

int8_t atCmd_Stop_Handler(tstrAt_cmd_content data, void* moreData, uint8_t useStoredValue)
{
	printf("Stop Handler\r\n");
	return (at_ble_status_t)at_ble_scan_stop();
}
