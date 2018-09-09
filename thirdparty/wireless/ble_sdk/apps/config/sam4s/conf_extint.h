/**
 * \file
 *
 * \brief SAM G55 External Interrupt Driver Configuration Header
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
 *
 */
#ifndef CONF_EXTINT_H_INCLUDED
#define CONF_EXTINT_H_INCLUDED

#include <asf.h>

/**
 * Push button definitions for sleep mode and active mode
 * @{
 */
#define PIN_PUSHBUTTON_WAKEUP_PIO    PIOA
#define PIN_PUSHBUTTON_WAKEUP_MASK   PIO_PA2
#define PIN_PUSHBUTTON_WAKEUP_ID     ID_PIOA
#define PIN_PUSHBUTTON_WAKEUP_ATTR   PIO_DEFAULT
/** @} */

/** Wakeup pin for wait mode: SW0 */
#define WAKEUP_WAIT_INPUT_ID    (1u << 2)
/** Wakeup pin for backup mode: Touchscreen controller IRQ pin */
#define WAKEUP_BACKUP_INPUT_ID  (1u << 2)

void button_cb(void);
static inline void button_handler(uint32_t ul_id, uint32_t ul_mask);

/* Button Initialize */
static inline void button_init(void)
{
	/* Adjust PIO debounce filter parameters, using 10 Hz filter. */
	pio_set_debounce_filter(PIN_PUSHBUTTON_WAKEUP_PIO,
						PIN_PUSHBUTTON_WAKEUP_MASK, 10);

	/* Initialize PIO interrupt handlers, see PIO definition in board.h. */
	pio_handler_set(PIN_PUSHBUTTON_WAKEUP_PIO, PIN_PUSHBUTTON_WAKEUP_ID,
					PIN_PUSHBUTTON_WAKEUP_MASK, PIN_PUSHBUTTON_WAKEUP_ATTR,
					button_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)PIN_PUSHBUTTON_WAKEUP_ID);

	/* Enable PIO line interrupts. */
	pio_enable_interrupt(PIN_PUSHBUTTON_WAKEUP_PIO,
				PIN_PUSHBUTTON_WAKEUP_MASK);
}

/**
 * \brief Handler for button interrupt.
 *
 * \note This interrupt is for waking up from sleep mode or exiting from active
 * mode.
 */
static inline void button_handler(uint32_t ul_id, uint32_t ul_mask)
{
	if (PIN_PUSHBUTTON_WAKEUP_ID == ul_id &&
			PIN_PUSHBUTTON_WAKEUP_MASK == ul_mask) {
		button_cb();
	}
}

/**
 * \brief Read the current state of the button pin
 *
 */
static inline uint8_t button_0_input_level(void) 
{
	return ioport_get_pin_level(BUTTON_0_PIN);
}

#endif
