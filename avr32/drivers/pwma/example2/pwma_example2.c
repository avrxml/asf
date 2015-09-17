/**
 * \file
 *
 * \brief Example to illustrate the Peripheral Event System in PWMA Module 
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
/* Include the example configuration file */
#include "conf_example.h"

/* FUNCTION PROTOTYPES */
/*! \brief Initializes and enables the ACIFB Module */
static void ac_init(void);

/*! \brief Initializes and enables the PWMA Module */
static void pwma_init(void);

/* GLOBAL VARIABLES DECLARATION */
/*! \brief Global variable to Store the PWMA module address */
volatile avr32_pwma_t *pwma = &AVR32_PWMA;

/*! \brief Global variable to Store the ACIFB module address */
volatile avr32_acifb_t *acifb = &AVR32_ACIFB;

/*! \brief Variable to read the TOP value */
volatile uint16_t read_top_value = 0;

/**
 * \brief ACIFB ISR Routine
 * \param acifb_irq                         ISR Routine name
 * \param AVR32_ACIFB_IRQ_GROUP             ACIFB Interrupt Group
 * \param EXAMPLE_ACIFB_INTERRUPT_PRIORITY  Priority level of the interrupt
 */
ISR(acifb_irq, AVR32_ACIFB_IRQ_GROUP, EXAMPLE_ACIFB_INTERRUPT_PRIORITY) 
{
	/* Initialize the event count to 1 */
	static uint16_t event_count = 1;

	/* Clear the ACIFB interrupt */
	acifb->icr = EXAMPLE_ACIFB_INTERRUPT_MASK;

	/* 
	 * Increment the count value by 1 if the positive event occurs
	 * and Decrement the same by 1 if negative event occurs.
	 * This is to keep track of the change in PWMA duty cycle value
	 * which is read only.
	 */
	if (acifb->sr & EXAMPLE_ACIFB_CHANNEL_MASK)
		event_count++;
	else if (!(acifb->sr & EXAMPLE_ACIFB_CHANNEL_MASK))
		event_count--;

	/*
	 * If event count reaches 0, load the duty cycle value and 
	 * event count with the TOP value. If event count reaches the TOP value
	 * reset the duty cycle value and event count to 0.
	 */
	if (event_count == 0){
		pwma_set_channels_value(pwma,(1<<EXAMPLE_PWMA_CHANNEL_ID),
								read_top_value);
		event_count = read_top_value;
	}
	else if (event_count > read_top_value){
		pwma_set_channels_value(pwma,(1<<EXAMPLE_PWMA_CHANNEL_ID),0);
		event_count =0;
	}

}


/**
 * \brief Main Application Routine                          \n
 *  -Initialize the system clocks                           \n
 *   Note: The clock should be configured in "conf_clock.h" \n
 *  -Initialize the PWMA Module                             \n
 *  -Read the TOP value                                     \n
 *  -Initialize the ACIFB Module                            \n
 *  -Initialize the delay service with CPU frequency        \n
 *  -Enter into sleep mode                                  \n
 */
int main (void)
{

	/* Initialize the system clocks */
	sysclk_init();

	/* Initialize the PWMA Module */
	pwma_init();

	/* Read the TOP value */
	read_top_value = pwma->CR.top;

	/* Initialize the ACIFB Module */
	ac_init();

	/* Initialize the delay routine with the CPU frequency */
	delay_init(sysclk_get_cpu_hz());

	/* Application routine */
	while(1){
		/* Enter into sleep mode */
		pm_sleep(AVR32_PM_SMODE_FROZEN);
	}

} /* End of main() */

/**
 * \brief Initialize the ACIFB Module with the User Triggered Mode  \n
 *  -Start the GCLK for ACIFB                                       \n
 *  -Configure and enable the ACIFB with user triggered mode        \n
 *  -Enable the positive event generation in ACIFB channel          \n
 *  -Enable and register the ACIFB interrupt                        \n
 */
static void ac_init(void)
{

	/*
	 * Calculate the division factor value to be loaded and
	 * Enable the Generic clock
	 */
	uint32_t div = div_ceil(sysclk_get_pba_hz(), EXAMPLE_AC_GCLK_FREQUENCY);
	genclk_enable_config(EXAMPLE_AC_GCLK_ID,EXAMPLE_AC_GCLK_SRC,div);

	/* ACIFB Pin and Function Map */
	static const gpio_map_t ACIFB_GPIO_MAP = {
		{EXAMPLE_ACIFBP_PIN, EXAMPLE_ACIFBP_FUNCTION},
		{EXAMPLE_ACIFBN_PIN, EXAMPLE_ACIFBN_FUNCTION},
	};

	/* Enable the ACIFB functions of the GPIO pins */
	gpio_enable_module(ACIFB_GPIO_MAP,
			sizeof(ACIFB_GPIO_MAP) / sizeof(ACIFB_GPIO_MAP[0]));

	/* ACIFB Configuration */
	const acifb_t acifb_opt = {
		.sut = 6, 
		.actest = TESTMODE_OFF,
		.eventen = false
	};

	/* ACIFB Channel Configuration */
	const acifb_channel_t acifb_channel_opt = {
		/* Filter length */
		.filter_len = 0,
		/* Hysteresis value */
		.hysteresis_value = 0,
		/* Output event when ACOUT is one */
		.event_negative = true,
		/* Output event when ACOUT is one */
		.event_positive = true,
		/* Set the positive input */
		.positive_input = PI_ACP,
		/* Set the negative input */
		.negative_input = NI_ACN,
		/* Set the comparator mode */
		.mode = MODE_CONTINUOUS,
		/* Enable Interrupt */
		.interrupt_settings = IS_COMP_DONE,
		/* Analog comparator channel number */
		.ac_n = EXAMPLE_ACIFB_CHANNEL
	};

	/* Configure the ACIFB peripheral */
	acifb_setup_and_enable(acifb, &acifb_opt);

	/* Configure the ACIFB channel with event generation enabled */
	acifb_channels_setup(acifb, &acifb_channel_opt, EXAMPLE_AC_NB_CHANNELS);
	
	/* Disable global interrupts */
	cpu_irq_disable();

	/*
	 * Initialize the interrupt vectors
	 * Note: This function adds nothing for IAR as the interrupts are
	 * handled by the IAR compiler itself. It provides an abstraction
	 * between GCC & IAR compiler to use interrupts.
	 * Refer function implementation in interrupt_avr32.h
	 */
	irq_initialize_vectors();

	/*
	 * Register the ACIFB interrupt handler
	 * Note: This function adds nothing for IAR as the interrupts are
	 * handled by the IAR compiler itself. It provides an abstraction
	 * between GCC & IAR compiler to use interrupts.
	 * Refer function implementation in interrupt_avr32.h
	 */
	irq_register_handler(&acifb_irq, AVR32_ACIFB_IRQ, 
						EXAMPLE_ACIFB_INTERRUPT_PRIORITY);

	/* Enable the Timebase overflow interrupt */
	acifb->ier = EXAMPLE_ACIFB_INTERRUPT_MASK;

	/* Enable global interrupt */
	cpu_irq_enable();

}

/**
 * \brief Initialize the PWMA Module 
 *  -Start the GCLK for PWMA Module
 *  -Configure and Enable the PWMA Module
 *  -Load the duty cycle value
 *  -Enable the input peripheral event
 */
static void pwma_init(void) 
{

	bool config_status = FAIL;
	bool set_value_status = FAIL;

	/*
	 * Calculate the division factor value to be loaded and
	 * Enable the Generic clock
	 */
	uint32_t div;
	div = div_ceil((sysclk_get_pba_hz()) , EXAMPLE_PWMA_GCLK_FREQUENCY);
	genclk_enable_config(EXAMPLE_PWMA_GCLK_ID,EXAMPLE_PWMA_GCLK_SOURCE,div);

	/* Enable the PWMA pin */
	gpio_enable_module_pin(EXAMPLE_PWMA_PIN,EXAMPLE_PWMA_FUNCTION);

	/*
	 * Configure and Enable the PWMA Module. The ERROR LED will be turned on
	 * if the configuration fails because of invalid argument.
	 */
	config_status = pwma_config_enable(pwma,EXAMPLE_PWMA_OUTPUT_FREQUENCY,
										EXAMPLE_PWMA_GCLK_FREQUENCY,0);

	/* Error in configuring the PWMA module */
	if (config_status == FAIL){
		while (1) {
			delay_ms(10);
			gpio_tgl_gpio_pin(ERROR_LED);
		}
	}


	/*
	 * Load the duty cycle value to the channel (Interlinked multi 
	 * value mode).The ERROR LED will be turned on, if the 
	 * configuration fails because of invalid argument.
	 */
	set_value_status = pwma_set_channels_value(pwma,
									(1<<EXAMPLE_PWMA_CHANNEL_ID),0);

	/* Error in loading the duty cycle value */
	if (set_value_status == FAIL){
		while (1) {
			delay_ms(10);
			gpio_tgl_gpio_pin(ERROR_LED);
		}
	}

	/* 
	 * Enable the input peripheral event for the corresponding channel
	 * Note: The channels supporting the input peripheral events are device 
	 * specific. Please refer to the Module Configuration of PWMA module in 
	 * the respective device datasheet.
	 */
	pwma_enable_input_peripheral_event(pwma,(1<<EXAMPLE_PWMA_CHANNEL_ID));
}

//Application Documentation
/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the PWMA driver.\n It also gives an example
 * of the usage of the PWMA module.\n
 *
 * This example will generate peripheral event from Analog Comparator(ACIFB).
 * When positive input(Vinp) of ACIFB is greater than the negative input(Vinn),
 * the event will be generated from ACIFB and the duty cycle value of PWMA 
 * will be increased by one. When negative input(Vinn) of ACIFB is greater than 
 * the positive input(Vinn), the event will be generated from ACIFB and the
 * duty cycle value of PWMA will be increased by one. The event_count variable
 * will keep track of the number of positive/negative event by generating 
 * the interrupt from ACIFB for each comparison. In the ISR, the duty cycle 
 * value and the event_count values will be reset to either zero 
 * [or] TOP value accordingly.
 *
 * The output PWMA frequency configured in this example is 1MHz and the SPREAD
 * is loaded with the value of 0. Initially the duty cycle value will be loaded
 * with 0. By giving Vinp > Vinn [or] Vinn>Vinp in ACIFB , the output frequency 
 * of 1MHz  with duty cycle value varying from 0 to TOP value can be seen on 
 * the Oscilloscope. Once the duty cycle reaches the TOP value, the duty cycle 
 * value will be loaded with 0 and starts again. Once it reaches zero,
 * it will be loaded with TOP value and starts again.
 * 
 * \section files Main files
 * - pwma.c           : PWMA Driver file
 * - pwma.h           : PWMA Header file
 * - conf_clock.h     : Clock configuration
 * - conf_example.h   : Example configuration
 * - pwma_example1.c  : Main Application file of PWMA EXAMPLE1
 *
 * \section referenceinfo References
 * - Respective device datasheet
 * - Respective Board Schematics 
 * - Schematics of the respective Routing card {for STK600 Board)
 *
 * \section compinfo Compiler Support
 * This example application supports
 * - GNU GCC for AVR32
 * - IAR EWAVR32 for UC3L064 device 
 *
 * \section deviceinfo Device support
 * - AT32UC3L0 Series
 * - ATUC[64/128/256]L3/L4 devices
 *
 * \section testinfo Test Information
 * This example has been tested with the following boards:
 * - UC3L-EK evaluation kit with an AT32UC3L064 MCU.
 * - UC3L0-Xplained kit with an AT32UC3L064 MCU.
 * - STK600 + RCUC3L0 routing card with an AT32UC3L0256 MCU.
 * - STK600 + RCUC3L3U routing card with an ATUC256L3U MCU.
 *
 * \section setupinfo Setup Information
 * - CPU Clock Source : DFLL(48MHz) with internal RCSYS as source
 * - CPU Speed        : 12MHz
 * - PBA Speed        : 12MHz
 * - PBB Speed        : 12MHz
 * - PWMA_GCLK Source : PBA Clock
 * - PWMA_GCLK Speed  : 12MHz
 * - ACIFB_GCLK Speed : 10KHz
 *
 * \section setupinfo Hardware Setup Information
 *
 * In UC3L0-XPLAINED KIT: \n
 *  - PWMA Channel used : channel 11
 *  - ACIFB Channel used : channel 4
 *  - ACIFB Input Voltage pins: \n
 *    ----------------------------     \n
 *   | Input pin  |  Pin on Board |    \n
 *    ----------------------------     \n
 *   | Vinp(PB00)  |  J4.Pin5     |    \n
 *   | Vinn(PB02)  |  J4.Pin7     |    \n
 *    ----------------------------     \n
 *  - The output can be measured with Oscilloscope as given below:\n
 *   --------------------------------------------------------------   \n
 *    PWMA Channel     |   LED on Board |    OUTPUT pins on Board     \n
 *   --------------------------------------------------------------   \n
 *    channel 11(PA11) |     LED0       | Pin 8(PWMA7) of Header J3   \n 
 *   --------------------------------------------------------------   \n
 *  - The LED2(LED_RED) will be turned on in case of any failure 
 *    in the PWMA configuration.
 *
 *
 * In UC3L-EK KIT: \n
 *  - PWMA Channel used : channel 9
 *  - ACIFB Channel used : channel 3
 *  - ACIFB Input Voltage pins: \n
 *    -----------------------------     \n
 *   | Input pin  |  Pin on Board  |    \n
 *    -----------------------------     \n
 *   | Vinp(PA04)  |  J14.Pin 9    |    \n
 *   | Vinn(PB09)  |  J15.Pin 14   |    \n
 *    -----------------------------     \n
 *  - The output can be measured with Oscilloscope as given below:\n
 *   ----------------------------------  \n
 *    PWMA Channel      |  OUTPUT pin    \n
 *   ----------------------------------  \n
 *    channel 11 (PA11) |  J14.Pin 4     \n
 *   ----------------------------------  \n
 *  - The LED2 will be turned on in case of any failure in the 
 *    PWMA configuration.
 *
 *
 * For STK600 + RCUC3L0 routing card with an AT32UC3L0256 MCU:
 *  - PWMA Channel used : channel 11
 *  - ACIFB Channel used : channel 4
 *  - ACIFB Input Voltage pins: \n
 *    ---------------------------     \n
 *   | Input pin  |  In STK600   |    \n
 *    ---------------------------     \n
 *   | Vinp(PB00)  |  PortD.PD0  |    \n
 *   | Vinn(PB02)  |  PortD.PD2  |    \n
 *    ---------------------------     \n
 *  - The output can be measured with Oscilloscope as given below:\n
 *   ----------------------------------  \n
 *    PWMA Channel      |  In STK600     \n
 *   ----------------------------------  \n
 *    channel 11 (PA11) |  PortB.PB3     \n
 *   ----------------------------------  \n
 *  - The port PA08 (portB.PB0 in STK600) can be connected to any of 
 *    the LEDs on the board. This will be turned on in case of any 
 *    failure in the PWMA configuration. \n
 *
 *
 * For STK600 + RCUC3L3U routing card with an ATUC256L3U MCU:
 *  - PWMA Channel used : channel 11
 *  - ACIFB Channel used : channel 4
 *  - ACIFB Input Voltage pins: \n
 *    ---------------------------     \n
 *   | Input pin  |  In STK600   |    \n
 *    ---------------------------     \n
 *   | Vinp(PB00)  |  PortD.PD0  |    \n
 *   | Vinn(PB02)  |  PortD.PD2  |    \n
 *    ---------------------------     \n
 *  - The output can be measured with Oscilloscope as given below:\n
 *   ----------------------------------  \n
 *    PWMA Channel      |  In STK600     \n
 *   ----------------------------------  \n
 *    channel 11 (PA11) |  PortB.PB3     \n
 *   ----------------------------------  \n
 *  - The port PA08 (portB.PB0 in STK600) can be connected to any of 
 *    the LEDs on the board. This will be turned on in case of any 
 *    failure in the PWMA configuration. \n
 *
 * \section contactinfo Contact Information
 * For further information, visit :
 * <A href="http://www.atmel.com/products/AVR32/"> Atmel AVR32 </A>.\n
 * Technical support & faq, visit :
 * <A href="http://www.atmel.com/design-support/"> Support and FAQ </A>.
 *
 * \author
 * Atmel Corporation : http://www.atmel.com \n
 */


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
