/**
 * \file
 *
 * \brief Application CDC Definitions
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef CDC_H_INCLUDED
#define CDC_H_INCLUDED

#include "keyboard.h"
#include "udc.h"
#include "udi_cdc.h"

//! \brief Serial number to be passed to USB stack
extern uint8_t cdc_serial_number[];

//! \brief Start up the needed parts for USB CDC
void cdc_start(void);

/**
 * \brief Called by CDC interface to open/close port communication
 *
 * \param enable Open if true, and close if false
 */
void cdc_set_dtr(bool enable);

/**
 * \brief Called by CDC interface
 *
 * Callback running when USB Host enable CDC interface
 *
 * \retval true if cdc startup is successfully done
 */
static inline bool cdc_enable(void)
{
	return true;
}

/**
 * \brief Called by CDC interface
 *
 * Callback running when USB Host disable cdc interface
 */
static inline void cdc_disable(void)
{
}

//! \brief Read character from CDC interface and translate to keycode
keycode_t cdc_getkey(void);

#endif /* CDC_H_INCLUDED */
