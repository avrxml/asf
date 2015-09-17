/**
 * \file
 *
 * \brief Metering Application Emulator for all node types
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */
#ifndef APP_EMU_COMMON_H
#define APP_EMU_COMMON_H

#include "compiler.h"

#define PROVIDER                "ATM"
#define PROVIDER2               "A"

#define APP_ID                  "APPLICATION EMULATION"
#define APP_VERSION_STR         "5.0"

#define TIME_STEP               300
#define TIME_REPEAT             2000

#define MAX_COUNTER_DATA_SIZE   PPDU_LENGTH_6_SIZE
#define TIMESTAMP_OFFSET        6
#define TIMESTAMP_SIZE          6
#define TIMESTAMP_END          11
#define STEP_TIMEOUT            2

#define DW_MSG                  0
#define UP_MSG                  1

#define STEP_0                  0
#define STEP_1                  1
#define STEP_20                 2
#define STEP_21                 3
#define STEP_30                 4
#define STEP_31                 5
#define STEP_40                 6
#define STEP_41                 7
#define STEP_42                 8
#define STEP_43                 9
#define STEP_50                10
#define STEP_51                11
#define STEP_60                12

#define LAST_STEP                STEP_60

/*
** Step 1: Initial Connection
*/
#define SERIAL_SIZE             9

/*
** Step 2: System Time Read
*/
#define TIME_REQUEST_SIZE       24
#define TIME_RESPONSE_SIZE      36

/*
** Step 3: Energy Register Read
*/
#define ENERGY_REQUEST_SIZE     24
#define ENERGY_RESPONSE_SIZE    31

/*
** Step 4: Accumulated Information Read
*/
#define INFO_REQUEST1_SIZE      75
#define INFO_RESPONSE1_SIZE     138
#define INFO_REQUEST2_SIZE      16
#define INFO_RESPONSE2_SIZE     52

/*
** Step 5: PPDU Length testing
*/
#define PPDU_LENGTH_0_SIZE      18
#define PPDU_LENGTH_1_SIZE      30
#define PPDU_LENGTH_2_SIZE      66
#define PPDU_LENGTH_3_SIZE      138
#define PPDU_LENGTH_4_SIZE      210
#define PPDU_LENGTH_5_SIZE      282
#define PPDU_LENGTH_6_SIZE      354

#define N_LENGTHS_TEST            46

/*
** Simplified version of the messages
*/
#define MSG_SIZE                9

/* DATA Request and Indication Scheme Field Constant Definition */
#define DBPSK                   0
#define DQPSK                   1
#define D8PSK                   2
#define DBPSK_PLUS_CONV         4
#define DQPSK_PLUS_CONV         5
#define D8PSK_PLUS_CONV         6

#define CON_HANDLER_INIT_VALUE  0xFFFF

#define TOTAL_NODES_REG         1

#define RX_DATA_SCHEME          DBPSK_PLUS_CONV

#define FU_TYPE                 251
#define CON_TYPE                250
#define CS432_TYPE              3

struct meter_node {
	uint16_t us_handler;
	uint8_t uc_serial[SERIAL_SIZE + 1];
	uint8_t uc_step;
	uint8_t uc_data[MAX_COUNTER_DATA_SIZE];
	uint16_t us_data_length;
	char c_timestamp[TIMESTAMP_SIZE + 1];
	uint8_t uc_timeout;
	uint8_t uc_pending;
};

struct _tm {
	uint32_t ul_tm_hour;
	uint32_t ul_tm_min;
	uint32_t ul_tm_sec;
};

/* *** Public Variables ******************************************************
 **/

#define time_set(t)            {TempInt1 = t; }
#define time_wait(t)           {TempInt1 = t; while (TempInt1) }
#define time_overflow()        (TempInt1 == 0)

#define CON_TIME_OUT           (temp_int_con == 0)

/*****************************************************************************
**                     Prototypes for Common Functions                      **
*****************************************************************************/

void app_emu_copy_timestamp(char *pc_timestamp, uint8_t *pc_base_cmd);
void app_emu_fill_string(char *c_string, uint16_t us_size, uint8_t uc_down_up,
		uint16_t us_step, char *c_timestamp);
void app_emu_update_10ms(void);

void app_emu_start(void);
void app_emu_process(void);

void app_emu_init_random(const uint8_t *puc_mac);
int  app_emu_random(void);

#endif
