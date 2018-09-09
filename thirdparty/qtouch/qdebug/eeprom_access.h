/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/******************************************************************************
 *
 * \file
 *
 * \brief  This file is the header file for eeprom_access
 *
 * - Compiler:           IAR and GNU GCC for AVR8
 * - Supported devices:  ATXMEGA128A1
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support:            https://www.microchip.com/support/
 *
 * \author               Microchip Technology Inc: http://www.microchip.com
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
#ifndef EEPROM_ACCESS_H
#define EEPROM_ACCESS_H

/*----------------------------------------------------------------------------
nested include files
----------------------------------------------------------------------------*/
#include "touch_api.h"

#if defined ( __ICCAVR__ )
#include <ioavr.h>
#if !defined(__XMEGA_CORE__)	// Caution
#define EEPROM_START_ADDRESS 0x0000u	/* User Eeprom address */
#else /* !defined(__ATxmega128A1_H) */
#define EEPROM_START_ADDRESS 0x1000u	/* User Eeprom address */
#define _ATXMEGA_DEVICE_ 1u
#endif /* !defined(__ATxmega128A1_H) */
#elif defined( __GNUC__ )	/* defined ( __ICCAVR__ ) */
#include <avr/io.h>
#if !defined(__AVR_XMEGA__)
#define EEPROM_START_ADDRESS 0x0000u	/* User Eeprom address */
#else /* !defined(__AVR_ATxmega128A1__) */
#define EEPROM_START_ADDRESS 0x1000u	/* User Eeprom address */
#define _ATXMEGA_DEVICE_ 1u
#endif /* !defined(__AVR_ATxmega128A1__) */
#endif /* defined( __GNUC__ ) */

/*----------------------------------------------------------------------------
manifest constants
----------------------------------------------------------------------------*/
/* Block start addresses in EEPROM */
#define EEPROM_LIB_CONFIG_START_ADDRESS  (EEPROM_START_ADDRESS + 2u)

/*! \brief write_global_settings_to_eeprom is used to 
 *  Save global parameter settings to EEPROM  
 */
void write_global_settings_to_eeprom (void);
/*! \brief write_sensor_settings_to_eeprom is used to 
 *  Save sensor parameter settings to EEPROM 
 */
void write_sensor_settings_to_eeprom (void);

#ifdef _QMATRIX_
/*! \brief write_burst_lengths_to_eeprom is used to 
 *  Save burst length settings to EEPROM
 */
void write_burst_lenghts_to_eeprom (void);
#endif
/*! \brief read_info_from_eeprom is used to 
 *  Read Info block from EEPROM
 */

uint16_t read_info_from_eeprom (void);
/*! \brief write_info_to_eeprom is used to 
 *  Save information from library and sign in the EEPROM
 */

void write_info_to_eeprom (uint16_t lib_version);
/*! \brief read_settings_from_eeprom is used to 
 *  Will read settings from EEPROM
 */

void read_settings_from_eeprom (void);

#endif /* EEPROM_ACCESS_H */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-EOF-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
