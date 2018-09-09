/**
 * \file serial_drv.h
 *
 * \brief Handles Serial driver functionalities
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

#ifndef SERIAL_DRV_H
#define SERIAL_DRV_H

/**
 * This module performs serial input/output functionalities via UART
 * @{
 */

/* === INCLUDES ============================================================ */

#include "compiler.h"
#include "status_codes.h"

#if (SAMG55 || SAM4S)

#define platform_start_rx()

#else
void platform_start_rx(void);
#endif

#if (SLEEP_WALKING_ENABLED && (SAMD21 || SAML21))
	#warning "Sleepwalking feature is not implemented in BluSDK SAM L21/D21"
#endif

#if (SLEEP_WALKING_ENABLED && (SAM4S))
	#error "Sleepwalking feature is not supported in SAM 4S"
#endif	

/* === PROTOTYPES ============================================================
**/

/**
 * \brief Initializes the Serial IO Module
 * \return STATUS_OK for successful initialization and FAILURE incase the IO is
 * not initialized
 */
uint8_t configure_serial_drv(uint32_t);

/**
 * \brief Transmits data via UART
 * \param data Pointer to the buffer where the data to be transmitted is present
 * \param length Number of bytes to be transmitted
 *
 * \return Number of bytes actually transmitted
 */
uint16_t serial_drv_send(uint8_t* data, uint16_t len);

/**
 * \brief Receives data from UART
 *
 * \param data pointer to the buffer where the received data is to be stored
 * \param max_length maximum length of data to be received
 *
 * \return actual number of bytes received
 */
uint8_t serial_read_data(uint8_t* data, uint16_t max_len);

uint8_t serial_read_byte(uint16_t* data);
//void configure_usart_after_patch(void);
void configure_usart_after_patch(uint32_t );
uint32_t platform_serial_drv_tx_status(void);

void platform_leave_critical_section(void);
void platform_enter_critical_section(void);

void platform_set_ble_rts_high(void);
void platform_set_ble_rts_low(void);
void platform_set_hostsleep(void);
void platform_restore_from_sleep(void);
void platform_configure_sleep_manager(void);
uint16_t serial_drive_rx_data_count(void);
#endif /* SIO2HOST_H */
