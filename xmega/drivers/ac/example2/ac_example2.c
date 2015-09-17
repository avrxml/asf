/**
 * \file
 *
 * \brief AVR XMEGA analog comparator example
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
 */

/**
 * \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the analog comparator driver.
 *
 * \section files Main Files
 *  - ac.c analog comparator driver implementation
 *  - ac.h analog comparator driver definitions
 *  - ac_example2.c example application
 *
 * \section ac_driver_section drivers/ac/ac.h driver
 * The analog comparator driver is described in the
 * \ref analog_comparator_group section.
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with an analog comparator (AC) can be used. This
 * example has been tested with the following setup:
 *  - STK600 wiht RC032X routing card for ATxmega32E5. When using this setup,
 * the LEDs should be connected to PORTC and swithes should be connected
 * to PORTA (using the alternate #define CONF_BOARD_USE_ALTERNATE_LED_PORT
 * in conf_board.h for stk600-rc032x.h).
 *
 * \section example_description Description of the example
 * The example will configure the analog comparator in single trigger level mode
 * and compare the input values from PIN0 (PA0) and PIN2 (PA2) with the bandgap
 * voltage.
 *
 * Summary of analog comparator configuration:
 *  - ACA channel 0: single mode comparing PIN0 (PA0) against bandgap voltage
 *  - ACA channel 1: single mode comparing PIN2 (PA2) against bandgap voltage
 *
 * This example uses the on-board LEDs to provide user feedback, the output
 * from the LEDs are as follows:
 *  - LED0: analog comparator A channel 0 is above bandgap voltage
 *  - LED1: analog comparator A channel 0 is below bandgap voltage
 *  - LED2: analog comparator A channel 1 is above bandgap voltage
 *  - LED3: analog comparator A channel 1 is below bandgap voltage
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 *  - \ref analog_comparator_group
 *  - \ref gpio_group for LED control
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "asf.h"

/**
 * \brief delay for \a ms milliseconds
 *
 * \param ms number of milliseconds to busy wait
 */
static void mdelay(uint16_t ms)
{
        uint32_t count;

        // Approximate the number of loop iterations needed.
        count = sysclk_get_cpu_hz() / 1000;
        count *= ms;
        count /= 6;

        do {
                asm("");
        } while (--count);
}

/**
 * \brief Analog comparator A channel 0 configuration.
 *
 * This struct is intentionally just defined, not declared. The example code
 * will use the helper functions to setup the channel configuration.
 */
static struct ac_config aca0_config = {
	.acctrl         = ((uint8_t)AC_INTMODE_BOTHEDGES_gc |
			(uint8_t)AC_INTLVL_MED_gc ),
	.acmuxctrl      = ((uint8_t)AC_MUXNEG_BANDGAP_gc |
			(uint8_t)AC_MUXPOS_PIN0_gc),
};


/**
 * \brief Analog comparator A channel 1 configuration.
 *
 * This struct configures the an analog comparator channel with the following
 * settings:
 * - Falling edge interrupt mode
 * - Medium interrupt level
 * - High-speed mode
 * - No hysteresis
 * - PIN2 as negative input to the multiplexer
 * - Bandgap as positive input to the multiplexer
 */
static struct ac_config aca1_config = {
	.acctrl         = ((uint8_t)AC_INTMODE_BOTHEDGES_gc |
			(uint8_t)AC_INTLVL_MED_gc),
	.acmuxctrl      = ((uint8_t)AC_MUXNEG_BANDGAP_gc |
			(uint8_t)AC_MUXPOS_PIN2_gc),
};

/**
 * \brief Update LEDs used for feedback from analog comparator channel in
 *        single trigger level mode
 *
 * This function will set the LEDs appropriately for the \a channel that caused
 * the interrupt. See the \ref example_description section for additional
 * details.
 *
 * \param channel The analog comparator channel to update status for
 * \param status Analog comparator channel status given by a \ref ac_status_t
 *               value
 */
static void example_ac_update_single_leds(uint8_t channel,
		enum ac_status_t status)
{
	if (channel == 0) {
		if (status == AC_STATUS_ABOVE) {
			gpio_set_pin_low(LED0_GPIO);
			gpio_set_pin_high(LED1_GPIO);
		} else {
			gpio_set_pin_high(LED0_GPIO);
			gpio_set_pin_low(LED1_GPIO);
		}
	} else {
		if (status == AC_STATUS_ABOVE) {
			gpio_set_pin_low(LED2_GPIO);
			gpio_set_pin_high(LED3_GPIO);
		} else {
			gpio_set_pin_high(LED2_GPIO);
			gpio_set_pin_low(LED3_GPIO);
		}
	}
}

/**
 * \brief Analog comparator A interrupt callback function
 *
 * This function is called when an interrupt has occurred on a channel in analog
 * comparator A.
 *
 * \param ac Pointer to the analog comparator (AC) base address which caused
 *           the interrupt
 * \param channel The analog comparator channel that caused the interrupt
 * \param status Analog comparator channel status given by a \ref ac_status_t
 *               value
 */
static void example_aca_interrupt_callback(AC_t *ac, uint8_t channel,
		enum ac_status_t status)
{
	example_ac_update_single_leds(channel, status);
}


/**
 * \brief Main function.
 */
int main(void)
{
	enum ac_status_t status;

	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();

	ac_set_interrupt_callback(&ACA, example_aca_interrupt_callback);

	/* Write configuration of analog comparator A channel 0. */
	ac_write_config(&ACA, 0, &aca0_config);

	/* Write predefined configuration of analog comparator A channel 1. */
	ac_write_config(&ACA, 1, &aca1_config);

	/* Enable all the analog comparator channels. */
	ac_enable(&ACA, 0);
	ac_enable(&ACA, 1);

	/*
	 * Wait for analog comparator to stabilize (muxing, voltage scaler,
	 * bandgap voltage, etc).
	 */
	mdelay(1);

	/* Check comparator status and initialize the LEDs. */
	status = ac_get_status(&ACA, 0);
	example_ac_update_single_leds(0, status);
	status = ac_get_status(&ACA, 1);
	example_ac_update_single_leds(1, status);

	cpu_irq_enable();

	for (;;) {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	}
}
