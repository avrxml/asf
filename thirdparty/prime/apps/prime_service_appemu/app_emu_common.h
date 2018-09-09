/**
 * \file
 *
 * \brief Metering Application Emulator for all node types
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
