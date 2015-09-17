/*****************************************************************************
 *
 * \file
 *
 * \brief QT60168 driver for AVR32 UC3.
 *
 * This file is the QT60168 driver.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _QT60168_H_
#define _QT60168_H_

/**
 * \defgroup group_avr32_components_touch_qt60168 TOUCH - QT60168 16-key QMatrix touch sensor
 *
 * This driver offers configuration as well as checking the status of the touch sensor.
 * The driver uses the TWI to communicate with the sensor and GPIO as a status change input from the sensor.
 * It is possible to add a callback function from the application and this will be executed each time
 * a change has been detected in the sensor state.
 *
 * \{
 */

// List of QT60168 Control Commands
#define QT60168_CMD_NULL_COMMAND          0x00
#define QT60168_CMD_ENTER_SETUPS_MODE     0x01
#define QT60168_CMD_CAL_ALL               0x03
#define QT60168_CMD_FORCE_RESET           0x04
#define QT60168_CMD_GENERAL_STATUS        0x05
#define QT60168_CMD_REPORT_1ST_KEY        0x06
#define QT60168_CMD_REPORT_ALL_KEYS       0x07
#define QT60168_CMD_ERROR_FLAGS_FOR_ALL   0x0B
#define QT60168_CMD_FMEA_STATUS           0x0C
#define QT60168_CMD_DUMP_SETUPS           0x0D
#define QT60168_CMD_EEPROM_CRC            0x0E
#define QT60168_CMD_RETURN_LAST_CMMD      0x0F
#define QT60168_CMD_INTERNAL_CODE_0X10    0x10
#define QT60168_CMD_INTERNAL_CODE_0X12    0x12
#define QT60168_CMD_DATA_FOR_1_KEY        0x40
#define QT60168_CMD_STATUS_FOR_KEY_K      0x80
#define QT60168_CMD_CAL_KEY_K             0xC0

#define QT60168_REPLY_FORCE_RESET         0xFB
#define QT60168_REPLY_ENTER_SETUPS_MODE   0xFE

#define QT60168_STATUS_KEY_DETECTED_MASK          0x01
#define QT60168_STATUS_KEY_IN_CALIBRATION_MASK    0x02
#define QT60168_STATUS_CALIBRATION_FAILED_MASK    0x04
#define QT60168_STATUS_MAIN_SYNC_ERROR_MASK       0x08
#define QT60168_STATUS_FMEA_ERROR_MASK            0x20
#define QT60168_STATUS_COMM_ERROR_MASK            0x40


#define QT60168_KEY_STATUS_CAL_FAILED     0x01
#define QT60168_KEY_STATUS_IN_CAL         0x02
#define QT60168_KEY_STATUS_LOW_SIGNAL     0x04
#define QT60168_KEY_STATUS_DETECTED       0x08
#define QT60168_KEY_STATUS_ENABLED        0x10


#define QT60168_MAX_NUMBER_OF_KEYS          24


typedef struct
{
   //! Drift Compensation and Negative Threshold.
   unsigned char ndrift_nthr;

   //! Detect Integrators.
   unsigned char fdil_ndil;

   //! Negative Recal Delay (NRD).
       unsigned char nrd;

   //! Oscilloscope Sync, Adjacent Key Suppression and Burst Length.
       unsigned char ssync_aks_bl;
} qt60168_key_cfg_t;

typedef struct
{
   qt60168_key_cfg_t key_cfg[QT60168_MAX_NUMBER_OF_KEYS];

   //! Mains Sync.
    unsigned char   msync   ;

   //! Burst Spacing.
    unsigned char   bs      ;

   //! Lower Signal Limit.
    unsigned short  lsl     ;

   //! CRC.
    unsigned char   crc     ;
}qt60168_setups_block_t;



/*! \brief Initialize the QT60168 component.
 *
 * \param cpu_hz The CPU frequency in hertz.
 */
void qt60168_init(U32 cpu_hz);

/*! \brief Test if no keys are pressed.
 *
 * \retval true    No key pressed (idle).
 * \retval false   One or more keys are pressed.
 */
bool qt60168_is_no_key_pressed( void );

/*! \brief Test if no keys are pressed.
 *
 * \param key      Key id that will be tested.
 *
 * \retval true    That key is pressed.
 * \retval false   That key is not pressed.
 */
bool qt60168_is_key_pressed(U8 key);

/*! \brief Report all key status
 *
 * \retval int where :
 *   bit0 is key 0
 *   bit1 is key 1
 *   bit2 is key 2
 *   .....
 *   ...
 *   ..
 *   bit23 is key 23
 */
int qt60168_report_all_key(void);

/*! \brief Send a command to the QT60168
 *
 * \param cmd  Command
 */
void qt60168_send_cmd(unsigned char cmd);

/*! \brief Get the QT60168 reply
 *
 * \param *data The data to read
 */
void qt60168_get_reply(unsigned short *data);

/**
 * \}
 */

#endif



