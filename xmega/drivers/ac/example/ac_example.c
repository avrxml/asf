/**
 * \file
 *
 * \brief AVR XMEGA analog comparator example
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
 *  - ac_example.c example application
 *
 * \section ac_driver_section drivers/ac/ac.h driver
 * The analog comparator driver is described in the
 * \ref analog_comparator_group section.
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with an analog comparator (AC) can be used. This
 * example has been tested with the following setup:
 *  - Xplain evaluation kit
 *
 * \section example_description Description of the example
 * The example will configure the analog comparator B in window mode and
 * compare the input value from PIN1 (PB1) with the bandgap voltage as lower
 * reference and a scaled VCC as upper reference. The upper value has been
 * chosen by best fit to match the potmeter output from ground to VCC / 2 (1.65
 * V).
 *
 * In addition the example configures the analog comparator A two channels in
 * single trigger level mode. On the Xplain evaluation kit these channels are
 * available on the spare ADC/DAC/AC header.
 *
 * Summary of analog comparator configuration:
 *  - ACA channel 0: single mode comparing PIN0 (PA0) against bandgap voltage
 *  - ACA channel 1: single mode comparing PIN2 (PA2) against bandgap voltage
 *  - ACB: window mode comparing PIN1 (PB1) against bandgap voltage (low) and
 *         scaled VCC (high)
 *
 * This example uses the on-board LEDs to provide user feedback, the output
 * from the LEDs are as follows:
 *  - LED0: analog comparator B is above window
 *  - LED1 and LED2: analog comparator B is inside window
 *  - LED3: analog comparator B is below window
 *  - LED4: analog comparator A channel 0 is above bandgap voltage
 *  - LED5: analog comparator A channel 0 is below bandgap voltage
 *  - LED6: analog comparator A channel 1 is above bandgap voltage
 *  - LED7: analog comparator A channel 1 is below bandgap voltage
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
			(uint8_t)AC_INTLVL_MED_gc | (uint8_t)AC_HSMODE_bm),
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
			(uint8_t)AC_INTLVL_MED_gc | (uint8_t)AC_HSMODE_bm),
	.acmuxctrl      = ((uint8_t)AC_MUXNEG_BANDGAP_gc |
			(uint8_t)AC_MUXPOS_PIN2_gc),
};

/**
 * \brief Analog comparator B window configuration.
 *
 * This struct is intentionally just defined, not declared. The example code
 * will use the helper functions to setup the analog comparator in window mode.
 */
static struct ac_config acb_config;

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
			gpio_set_pin_low(LED4_GPIO);
			gpio_set_pin_high(LED5_GPIO);
		} else {
			gpio_set_pin_high(LED4_GPIO);
			gpio_set_pin_low(LED5_GPIO);
		}
	} else {
		if (status == AC_STATUS_ABOVE) {
			gpio_set_pin_low(LED6_GPIO);
			gpio_set_pin_high(LED7_GPIO);
		} else {
			gpio_set_pin_high(LED6_GPIO);
			gpio_set_pin_low(LED7_GPIO);
		}
	}
}

/**
 * \brief Update LEDs used for feedback from analog comparator in window mode
 *
 * This function will set the LEDs appropriately \a status of the window. See
 * the \ref example_description section for additional details.
 *
 * \param status Analog comparator window status given by a \ref ac_status_t
 *               value
 */
static void example_ac_update_window_leds(enum ac_status_t status)
{
	gpio_set_pin_high(LED0_GPIO);
	gpio_set_pin_high(LED1_GPIO);
	gpio_set_pin_high(LED2_GPIO);
	gpio_set_pin_high(LED3_GPIO);

	if (status == AC_STATUS_ABOVE) {
		gpio_set_pin_low(LED0_GPIO);
	}
	else if (status == AC_STATUS_INSIDE) {
		gpio_set_pin_low(LED1_GPIO);
		gpio_set_pin_low(LED2_GPIO);
	} else {
		gpio_set_pin_low(LED3_GPIO);
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
 * \brief Analog comparator B interrupt callback function
 *
 * This function is called when an interrupt has occurred on a channel in analog
 * comparator B.
 *
 * \param ac Pointer to the analog comparator (AC) base address which caused
 *           the interrupt
 * \param channel The analog comparator channel that caused the interrupt
 * \param status Analog comparator window status given by a \ref ac_status_t
 *               value
 */
static void example_acb_interrupt_callback(AC_t *ac, uint8_t channel,
		enum ac_status_t status)
{
	/*
	 * If trigger was caused by moving into above or below, switch to
	 * trigger by being inside. If trigger was caused by moving inside the
	 * window, switch to trigger on outside.
	 */
	if (status != AC_STATUS_INSIDE) {
		ac_set_interrupt_mode(&acb_config, AC_INT_MODE_INSIDE_WINDOW);
	} else {
		ac_set_interrupt_mode(&acb_config, AC_INT_MODE_OUTSIDE_WINDOW);
	}

	ac_disable(&ACB, 1);
	ac_disable(&ACB, 0);
	ac_write_config(&ACB, 0, &acb_config);
	ac_enable(&ACB, 0);
	ac_enable(&ACB, 1);

	example_ac_update_window_leds(status);
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
	ac_set_interrupt_callback(&ACB, example_acb_interrupt_callback);

	/* Write configuration of analog comparator A channel 0. */
	ac_write_config(&ACA, 0, &aca0_config);

	/* Write predefined configuration of analog comparator A channel 1. */
	ac_write_config(&ACA, 1, &aca1_config);

	/* Setup the analog comparator B in window mode. */
	ac_set_mode(&acb_config, AC_MODE_WINDOW);
	ac_set_voltage_scaler(&acb_config, 11);
	ac_set_hysteresis(&acb_config, AC_HYSMODE_LARGE_gc);
	ac_set_negative_reference(&acb_config, AC_MUXNEG_SCALER_gc);
	ac_set_positive_reference(&acb_config, AC_MUXPOS_PIN1_gc);
	ac_set_interrupt_mode(&acb_config, AC_INT_MODE_INSIDE_WINDOW);
	ac_set_interrupt_level(&acb_config, AC_INT_LVL_MED);

	/*
	 * Write configuration of analog comparator B channel 0, half of window
	 * configuration.
	 */
	ac_write_config(&ACB, 0, &acb_config);

	/* Set the lower reference of the compare window. */
	ac_set_negative_reference(&acb_config, AC_MUXNEG_BANDGAP_gc);

	/*
	 * Write configuration of analog comparator B channel 1, other half of
	 * window configuration.
	 */
	ac_write_config(&ACB, 1, &acb_config);

	/* Enable all the analog comparator channels. */
	ac_enable(&ACA, 0);
	ac_enable(&ACA, 1);
	ac_enable(&ACB, 0);
	ac_enable(&ACB, 1);

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
	status = ac_get_status(&ACB, 0);
	example_ac_update_window_leds(status);

	/*
	 * Read back analog comparator channel 0 configuration, to be used when
	 * updating the window interrupt mode in the interrupt callback
	 * function.
	 */
	ac_read_config(&ACB, 0, &acb_config);

	cpu_irq_enable();

	for (;;) {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	}
}
