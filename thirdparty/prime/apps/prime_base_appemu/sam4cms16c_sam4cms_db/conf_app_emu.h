/**
 * \file
 *
 * \brief Example configuration for the Metering Application Emulator
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
 */

#ifndef CONF_APP_EMU_H
#define CONF_APP_EMU_H

/* Enable debug printouts */
#define APPEMU_DEBUG

/* APPEMU: ON/OFF JUMPER CONFIG */
#define PIN_APPEMU_GPIO      (PIO_PC3_IDX)
#define PIN_APPEMU_FLAGS     (PIO_INPUT)
#define PIN_APPEMU           {PIO_PC3, PIOC, ID_PIOC, PIO_INPUT, PIO_DEBOUNCE |	\
			     			  PIO_PULLUP}
#define PIN_APPEMU_MASK      PIO_PC3
#define PIN_APPEMU_PIO       PIOC
#define PIN_APPEMU_ID        ID_PIOC
#define PIN_APPEMU_TYPE      PIO_INPUT
#define PIN_APPEMU_ATTR      PIO_DEBOUNCE | PIO_PULLUP

#endif /* CONF_APP_EMU_H */
