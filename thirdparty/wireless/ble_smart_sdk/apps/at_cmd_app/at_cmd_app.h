#ifndef __MAIN_H__
#define __MAIN_H__

/***************************
INCLUDES
***************************/
#include "at_ble_api.h"
//#include "uart_config.h"
//#include "gpio_config.h"
#include "at_cmd.h"


/***************************
DEFINES
***************************/
#define CORP_NAME       "Atmel"
#define DEVICE_NAME     CORP_NAME" BLE Device"


/***************************
MACROS
***************************/
#define CHECK_ERROR(VAR,LABEL)      if(AT_BLE_SUCCESS != VAR) \
    { \
        goto LABEL; \
    }

/***************************
VARIABLES
***************************/
volatile unsigned char app_stack_patch[2048];

uint8_t platform_event_handler(uint16_t, uint8_t *, uint16_t);
at_ble_status_t init_ble_stack(void);

#endif	//#ifndef __MAIN_H__
