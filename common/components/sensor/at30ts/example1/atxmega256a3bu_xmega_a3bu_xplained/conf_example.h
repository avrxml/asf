/**
 * \file
 *
 * \brief Example specific configurations
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

#ifndef CONF_EXAMPLE_H_INCLUDED
#define CONF_EXAMPLE_H_INCLUDED

/**
 * \section intro Introduction
 * This example application is created for AT30TSE758 on 
 * the ATAVRTempSensorX board. When checking this code on other devices,
 * define the preprocessor symbol corresponding to the selected Sensor 
 * device, in the project properties
 * eg:
 * _AT30TSE002B_ _AT30TSE758_
 * Connect pull up for TWI, on A3BU Xplained Kit or TempSensor Xplained
 */
 
/*! \name GPIO Connections and device addresses.
 */
/* ! @{ */

#define LED_LOW         J1_PIN5
#define LED_HIGH        J1_PIN7
#define LED_CRIT        J1_PIN3
#define LED_NORM        J1_PIN2
#define EVENT_PIN       J2_PIN6

/* ! @} */

/* PATTERN_TEST_LENGTH value should be less 
 * than the size of 'ibuf' in main() 
 */
#define PATTERN_TEST_LENGTH            7
#define TWI_SPEED                      50000

#ifdef _AT30TSE758_
#define EXAMPLE_TS_DEVICE              AT30TSE758
#define EXAMPLE_TS_DEVICE_ADDR         AT30TSE758_ADDR_LOCAL
#elif defined _AT30TSE002B_
#define EXAMPLE_TS_DEVICE              AT30TSE002B
#define EXAMPLE_TS_DEVICE_ADDR         AT30TSE002B_ADDR_LOCAL
#endif

/* EE_TEST_ADDR max value for AT30TSE758 is (1024 -  PATTERN_TEST_LENGTH) */
/* EE_TEST_ADDR max value for AT30TSE002B is (256 -  PATTERN_TEST_LENGTH) */
#define EE_TEST_ADDR                   130

/* Development kits */
#ifdef _AT30TK175_

/* Temperature register address on AT30TK175 board */
#define AT30TSE002B_ADDR               0x1B
#define AT30TS75_ADDR                  0x48
#define AT30TS750_ADDR                 0x49
#define AT30TSE758_ADDR                0x4A
/* EEPROM Address */
#define AT30TSE002B_EEPROM_ADDR        0x53
#define AT30TSE758_EEPROM_ADDR         0x52

#elif defined _ATAVRTEMPSENSORX_

/* Local (main board): temperature register address */
#define AT30TSE002B_ADDR_LOCAL         0x18
#define AT30TSE758_ADDR_LOCAL          0x48

/* Local (main board): EEPROM Address */
#define AT30TSE002B_EEPROM_ADDR_LOCAL  0x50
#define AT30TSE758_EEPROM_ADDR_LOCAL   0x50

#endif

#endif // CONF_EXAMPLE_H_INCLUDED
