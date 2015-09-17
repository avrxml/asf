/*****************************************************************************
 *
 * \file
 *
 * \brief AT42QT1060 driver for AVR32 UC3.
 *
 * This file is the AT42QT1060 driver.
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

#ifndef _AT42QT1060_H_
#define _AT42QT1060_H_

/**
 * \defgroup group_avr32_components_touch_at42qt1060 TOUCH - AT42QT1060 QTouch 6-channel sensor
 *
 * This driver offers configuration as well as checking the status of the touch sensor.
 * The communication with the sensor is done via the TWI interface and a change of the sensor
 * status is signalled via a GPIO. The driver offers the possibility to add callback function
 * from the application which will be executed each time a change in the sensor is detected.
 *
 * \{
 */

#include <stdint.h>

// List of QT60168 Control Commands
#define AT42QT1060_CHIP_ID                0x00
#define AT42QT1060_VERSION                0x01
#define AT42QT1060_MINOR_VERSION          0x02
#define AT42QT1060_DETECTION_STATUS       0x04
#define AT42QT1060_INPUT_PORT_STATUS      0x05
#define AT42QT1060_CALIBRATE              0x0C
#define AT42QT1060_RESET                  0x0D
#define AT42QT1060_DRIFT_OPTION           0x0E
#define AT42QT1060_POS_RECALIBRATION_DELAY 0x0F
#define AT42QT1060_KEY_0_NTHR             0x10
#define AT42QT1060_KEY_1_NTHR             0x11
#define AT42QT1060_KEY_2_NTHR             0x12
#define AT42QT1060_KEY_3_NTHR             0x13
#define AT42QT1060_KEY_4_NTHR             0x14
#define AT42QT1060_KEY_5_NTHR             0x15
#define AT42QT1060_LP_MODE                0x16
#define AT42QT1060_IO_MASK                0x17
#define AT42QT1060_KEY_MASK               0x18
#define AT42QT1060_AKS_MASK               0x19
#define AT42QT1060_PWM_MASK               0x1A
#define AT42QT1060_DETECTION_MASK         0x1B
#define AT42QT1060_ACTIVE_LEVEL_MASK      0x1C
#define AT42QT1060_USER_OUTPUT_BUFFER     0x1D
#define AT42QT1060_DI                     0x1E
#define AT42QT1060_PWM_LEVEL              0x1F
#define AT42QT1060_KEY_0_SIGNAL_LSB       0x28
#define AT42QT1060_KEY_0_SIGNAL_MSB       0x29
#define AT42QT1060_KEY_1_SIGNAL_LSB       0x2A
#define AT42QT1060_KEY_1_SIGNAL_MSB       0x2B
#define AT42QT1060_KEY_2_SIGNAL_LSB       0x2C
#define AT42QT1060_KEY_2_SIGNAL_MSB       0x2D
#define AT42QT1060_KEY_3_SIGNAL_LSB       0x2E
#define AT42QT1060_KEY_3_SIGNAL_MSB       0x2F
#define AT42QT1060_KEY_4_SIGNAL_LSB       0x30
#define AT42QT1060_KEY_4_SIGNAL_MSB       0x31
#define AT42QT1060_KEY_5_SIGNAL_LSB       0x32
#define AT42QT1060_KEY_5_SIGNAL_MSB       0x33
#define AT42QT1060_KEY_0_REF_DATA_LSB     0x34
#define AT42QT1060_KEY_0_REF_DATA_MSB     0x35
#define AT42QT1060_KEY_1_REF_DATA_LSB     0x36
#define AT42QT1060_KEY_1_REF_DATA_MSB     0x37
#define AT42QT1060_KEY_2_REF_DATA_LSB     0x38
#define AT42QT1060_KEY_2_REF_DATA_MSB     0x39
#define AT42QT1060_KEY_3_REF_DATA_LSB     0x3A
#define AT42QT1060_KEY_3_REF_DATA_MSB     0x3B
#define AT42QT1060_KEY_4_REF_DATA_LSB     0x3C
#define AT42QT1060_KEY_4_REF_DATA_MSB     0x3D
#define AT42QT1060_KEY_5_REF_DATA_LSB     0x3E
#define AT42QT1060_KEY_5_REF_DATA_MSB     0x3F

#define AT42QT1060_KEY_0 0x01
#define AT42QT1060_KEY_1 0x02
#define AT42QT1060_KEY_2 0x04
#define AT42QT1060_KEY_3 0x08
#define AT42QT1060_KEY_4 0x10
#define AT42QT1060_KEY_5 0x20
#define AT42QT1060_CAL   0x80

#define AT42QT1060_IO_0 0x01
#define AT42QT1060_IO_1 0x02
#define AT42QT1060_IO_2 0x04
#define AT42QT1060_IO_3 0x08
#define AT42QT1060_IO_4 0x10
#define AT42QT1060_IO_5 0x20
#define AT42QT1060_IO_6 0x40

#define AT42QT1060_DEVICE_VERSION 0x03
#define AT42QT1060_DEVICE_MINOR_VERSION 0x00


/*! \brief Write data to a sensor register.
 *
 * \param reg_index Register index number
 * \param data Register data
 */
void at42qt1060_write_reg(uint8_t reg_index, uint8_t data);

/*! \brief Read register data.
 *
 * \param reg_index Register index
 * \returns Register value
 */
uint8_t at42qt1060_read_reg(uint8_t reg_index);

/*! \brief Register a pin interrupt handler.
 *
 * The detect signal from the touch sensor is used to generate a pin
 * interrupt.
 *
 * \param touch_detect_callback Pointer to callback function that should
 * be called when the interrupt triggers. Keep this function as short as
 * possible.
 */
void at42qt1060_register_int(void (*touch_detect_callback)(void));

/*! \brief Register a interrupt handler for the EIC controller.
 *
 * If the detect signal is on an input of the EIC controller this function
 * can be used to register a interrupt handler for it.
 *
 *\param touch_detect_callback Pointer to callback function that should
 * be called when the interrupt triggers. Keep this function as short as
 * possible.
 *
 */
void at42qt1060_register_eic_int(void (*touch_detect_callback)(void));

/*! \brief Performs a software reset of the touch sensor.
 */
void at42qt1060_reset(void);

/*! \brief Starts a re-calibration or the sensor.
 */
void at42qt1060_calibrate(void);

/*! \brief Sets the current power mode of the device.
 *
 * \param lp_mode Determines the number of 16ms intervals between key measurements.
 * Longer intervals between measurements -> lower power consumption but slower response
 * A value of 0 causes the device to sleep and no measurements are performed.
 * A value of 255 causes the device to enter "free-run" mode where measurements are
 * continuously taken.
 * 1 = 16ms between measurements
 * 2 = 32ms between measurements
 * ...
 */
void at42qt1060_set_low_power_mode(uint8_t lp_mode);

/*! \brief Initialize touch sensor with default configuration values.
 *
 */
void at42qt1060_init(int32_t fcpu);

/*! \brief Gets the touch detect status of the sensor.
 *
 * \returns Register content of the touch detect register.
 */
uint8_t at42qt1060_get_detect_status(void);

/**
 * \}
 */

#endif // _AT42QT1060_H_

