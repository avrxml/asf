/*****************************************************************************
*
* \file
*
* \brief GPIO example application for AVR32 using the peripheral bus interface.
*
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
*****************************************************************************/

/** \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the GPIO driver.
 *
 * The General Purpose Input/Output manages the I/O pins of the microcontroller.
 * Each I/O line
 * may be dedicated as a general-purpose I/O or be assigned to a function of an
 * embedded peripheral.
 * This assures effective optimization of the pins of a product.
 *
 * The given example covers various uses of the GPIO controller and demonstrates
 * different GPIO functionalities using the peripheral bus interface. It uses a
 * LED and a button.
 *
 * This interface operates with lower clock frequencies (fPB <= fCPU), and its
 * timing is not deterministic since it needs to access a shared bus which may
 * be heavily loaded.
 *
 * \section files Main Files
 * - gpio.c: GPIO driver;
 * - gpio.h: GPIO driver header file;
 * - gpio_peripheral_bus_example.c: GPIO example application using the
 * peripheral bus.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a GPIO module can be used. This example has been
 * tested
 * with the following setup:
 *   - EVK1100, EVK1101, UC3C_EK, EVK1104 or EVK1105 evaluation kits
 *   - STK600+RCUC3L0 routing card connect :
 *            - STK600.PORTA.PA4 to STK600.LEDS.LED0
 *            - STK600.PORTA.PA5 to STK600.LEDS.LED1
 *            - STK600.PORTD.PD2 to STK600.SWITCHES.SW0
 *   - STK600+RCUC3L4 routing card connect :
 *            - STK600.PORTC.PC3 to STK600.LEDS.LED0
 *            - STK600.PORTD.PD7 to STK600.LEDS.LED1
 *            - STK600.PORTD.PD6 to STK600.SWITCHES.SW0
 *     Press and release SW0 to turn LED1 on/off. LED0 is automatically toggled
 *     by the application
 *   - STK600+RCUC3D routing card connect :
 *            - STK600.PORTA.PA4 to STK600.LEDS.LED0
 *            - STK600.PORTA.PA5 to STK600.LEDS.LED1
 *            - STK600.PORTD.PE2 to STK600.SWITCHES.SW0
 *     Press and release SW0 to turn LED1 on/off. LED0 is automatically toggled
 *     by the application
 *   - AT32UC3L-EK: the A LED toggles forever and use the WAKE button to toggle
 *     the B LED.
 *
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include "conf_gpio_peripheral_bus_example.h"

/** \brief Main function to alter the state of the pin \c GPIO_PIN_EXAMPLE.
 */
int main(void)
{
	uint32_t state = 0;
	uint32_t i;

	while (1) {
		switch (state) {
		case 0:

			/* Access with GPIO driver gpio.c with clear and set
			 * access. */
			gpio_clr_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state++;
			break;

		case 1:
			gpio_set_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state++;
			break;

		case 2:

			/* Note that it is also possible to use the GPIO toggle
			 * feature. */
			gpio_tgl_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state++;
			break;

		case 3:
		default:
			gpio_tgl_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state = 0;
			break;
		}

		/* Poll push button value. */
		for (i = 0; i < 1000; i += 4) {
			if (gpio_get_pin_value(GPIO_PIN_EXAMPLE_3) == false) {
				gpio_clr_gpio_pin(GPIO_PIN_EXAMPLE_2);
			} else {
				gpio_set_gpio_pin(GPIO_PIN_EXAMPLE_2);
			}
		}
	}
}
