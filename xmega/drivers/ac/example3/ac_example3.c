/**
 * \file
 *
 * \brief AVR XMEGA analog comparator example
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

/**
 * \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the analog comparator driver.
 *
 * \section files Main Files
 *  - ac.c analog comparator driver implementation
 *  - ac.h analog comparator driver definitions
 *  - ac_example3.c example application
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
 * The example will configure the analog comparator B in window mode and
 * compare the input value from PIN1 (PA1) with the bandgap voltage as lower
 * reference and a scaled VCC as upper reference. The upper value has been
 * chosen by best fit to match the potmeter output from ground to VCC / 2 (1.65
 * V).
 *
 * Summary of analog comparator configuration:
 *  - ACB: window mode comparing PIN1 (PB1) against bandgap voltage (low) and
 *         scaled VCC (high)
 *
 * This example uses the on-board LEDs to provide user feedback, the output
 * from the LEDs are as follows:
 *  - LED0: analog comparator B is above window
 *  - LED1 and LED2: analog comparator B is inside window
 *  - LED3: analog comparator B is below window
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
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
 * \brief Analog comparator window configuration.
 *
 * This struct is intentionally just defined, not declared. The example code
 * will use the helper functions to setup the analog comparator in window mode.
 */
static struct ac_config aca_config;


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
 * \brief Analog comparator interrupt callback function
 *
 * This function is called when an interrupt has occurred on a channel in analog
 * comparator.
 *
 * \param ac Pointer to the analog comparator (AC) base address which caused
 *           the interrupt
 * \param channel The analog comparator channel that caused the interrupt
 * \param status Analog comparator window status given by a \ref ac_status_t
 *               value
 */
static void example_aca_interrupt_callback(AC_t *ac, uint8_t channel,
		enum ac_status_t status)
{
	/*
	 * If trigger was caused by moving into above or below, switch to
	 * trigger by being inside. If trigger was caused by moving inside the
	 * window, switch to trigger on outside.
	 */
	if (status != AC_STATUS_INSIDE) {
		ac_set_interrupt_mode(&aca_config, AC_INT_MODE_INSIDE_WINDOW);
	} else {
		ac_set_interrupt_mode(&aca_config, AC_INT_MODE_OUTSIDE_WINDOW);
	}

	ac_disable(&ACA, 1);
	ac_disable(&ACA, 0);
	ac_write_config(&ACA, 0, &aca_config);
	ac_enable(&ACA, 0);
	ac_enable(&ACA, 1);

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


	/* Setup the analog comparator B in window mode. */
	ac_set_mode(&aca_config, AC_MODE_WINDOW);
	ac_set_voltage_scaler(&aca_config, 11);
	ac_set_hysteresis(&aca_config, AC_HYSMODE_LARGE_gc);
	ac_set_negative_reference(&aca_config, AC_MUXNEG_SCALER_gc);
	ac_set_positive_reference(&aca_config, AC_MUXPOS_PIN1_gc);
	ac_set_interrupt_mode(&aca_config, AC_INT_MODE_INSIDE_WINDOW);
	ac_set_interrupt_level(&aca_config, AC_INT_LVL_MED);

	/*
	 * Write configuration of analog comparator B channel 0, half of window
	 * configuration.
	 */
	ac_write_config(&ACA, 0, &aca_config);

	/* Set the lower reference of the compare window. */
	ac_set_negative_reference(&aca_config, AC_MUXNEG_BANDGAP_gc);

	/*
	 * Write configuration of analog comparator B channel 1, other half of
	 * window configuration.
	 */
	ac_write_config(&ACA, 1, &aca_config);

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
	example_ac_update_window_leds(status);

	/*
	 * Read back analog comparator channel 0 configuration, to be used when
	 * updating the window interrupt mode in the interrupt callback
	 * function.
	 */
	ac_read_config(&ACA, 0, &aca_config);

	cpu_irq_enable();

	for (;;) {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	}
}
