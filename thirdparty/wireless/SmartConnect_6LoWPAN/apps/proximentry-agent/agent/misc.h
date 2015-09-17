/*******************************************************************************
 *
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file misc.h
 * @brief
 * */


#ifndef MISC_H
#define MISC_H

#include <stdbool.h>
#include "common.h"
#include "uip.h"

void hexdump(const void *data, size_t data_len);
enum led_state_e {LED_OFF, LED_ON, LED_BLINK};
void panic(void) _NO_RETURN_;
void triple_blink(void);
char * _PURE_ LTRIM(char *p);
char *RTRIM(char *p);
char *TRIM(char *p);

char *get_device_name(void);

typedef struct device_hw_info_s {
	char device_id[9];
	char device_serial_no[33];
} device_hw_info_t;

const device_hw_info_t *get_hw_sw_info(void) _CONST_;



#define EUI64_STR_LEN 3*8+1
#define IPV6_STR_LEN 8*4+7+1

char *ipv6_addr_to_str(char *ipv6_str,  uip_ipaddr_t *ipv6);
char *eui64_to_str(char *eui64_str,  uint8_t *eui64);
uint16_t get_nodeid(void); //TODO: refactoring
#endif /* MISC_H */
