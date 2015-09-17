/**
 * \file
 *
 * \brief Example to illustrate the basic PWMA Function 
 * of generating waveform to two different PWMA channels.
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


/*! \name Example Configuration */
/*!@{ */

/**
 * \def EXAMPLE_PWMA_GCLK_ID
 * \brief GCLK number for PWMA Module
 */
 
/**
 * \def EXAMPLE_PWMA_GCLK_FREQUENCY
 * \brief Output frequency of the PWMA Generic clock
 */

/**
 * \def EXAMPLE_PWMA_GCLK_SOURCE
 * \brief Source for PWMA Generic clock
 */

/**
 * \def EXAMPLE_PWMA_OUTPUT_FREQUENCY
 * \brief PWMA output frequency
 */

/**
 * \def EXAMPLE_PWMA_CHANNEL_ID1
 * \brief Channel index number of PWMA Module
 */

/**
 * \def EXAMPLE_PWMA_PIN1
 * \brief Corresponding GPIO pin number of the EXAMPLE_PWMA_CHANNEL_ID1
 */
 
/**
 * \def EXAMPLE_PWMA_FUNCTION1
 * \brief Corresponding pin function number of EXAMPLE_PWMA_PIN1
 */

/**
 * \def EXAMPLE_PWMA_CHANNEL_ID2
 * \brief Channel index number of PWMA Module
 */

 /**
 * \def EXAMPLE_PWMA_PIN2
 * \brief Corresponding GPIO pin number of the EXAMPLE_PWMA_CHANNEL_ID2
 */

 /**
 * \def EXAMPLE_PWMA_FUNCTION2
 * \brief Corresponding pin function number of EXAMPLE_PWMA_PIN2
 */
 
 /**
  * \def ERROR_LED
  * \brief LED Shows the status of the PWMA configuration.
  */
 
 /**
 * \def PWMA_SPREAD
 * \brief SPREAD value to be loaded to the PWMA Module
 */
 
 /** 
  * \def PWMA_INTERRUPT_PRIORITY
  * \brief Priority level of the PWMA interrupt
  */
/*!@} */

#if BOARD == UC3_L0_XPLAINED
#  define EXAMPLE_PWMA_GCLK_ID          AVR32_SCIF_GCLK_PWMA
#  define EXAMPLE_PWMA_GCLK_FREQUENCY   48000000
#  define EXAMPLE_PWMA_GCLK_SOURCE      AVR32_SCIF_GC_USES_CLK_PBA
#  define EXAMPLE_PWMA_OUTPUT_FREQUENCY 2000000
#  define EXAMPLE_PWMA_CHANNEL_ID1      11 
#  define EXAMPLE_PWMA_PIN1             AVR32_PWMA_11_PIN
#  define EXAMPLE_PWMA_FUNCTION1        AVR32_PWMA_11_FUNCTION
#  define EXAMPLE_PWMA_CHANNEL_ID2      27 
#  define EXAMPLE_PWMA_PIN2             AVR32_PWMA_27_PIN
#  define EXAMPLE_PWMA_FUNCTION2        AVR32_PWMA_27_FUNCTION
#  define ERROR_LED                     LED2_GPIO 
#  define PWMA_SPREAD                   10
#  define PWMA_INTERRUPT_PRIORITY       0
#elif BOARD == UC3L_EK  
#  define EXAMPLE_PWMA_GCLK_ID          AVR32_SCIF_GCLK_PWMA
#  define EXAMPLE_PWMA_GCLK_FREQUENCY   48000000
#  define EXAMPLE_PWMA_GCLK_SOURCE      AVR32_SCIF_GC_USES_CLK_PBA
#  define EXAMPLE_PWMA_OUTPUT_FREQUENCY 2000000
#  define EXAMPLE_PWMA_CHANNEL_ID1      21
#  define EXAMPLE_PWMA_PIN1             AVR32_PWMA_21_PIN
#  define EXAMPLE_PWMA_FUNCTION1        AVR32_PWMA_21_FUNCTION
#  define EXAMPLE_PWMA_CHANNEL_ID2      33
#  define EXAMPLE_PWMA_PIN2             AVR32_PWMA_33_PIN
#  define EXAMPLE_PWMA_FUNCTION2        AVR32_PWMA_33_FUNCTION
#  define ERROR_LED                     LED2_GPIO
#  define PWMA_SPREAD                   10
#  define PWMA_INTERRUPT_PRIORITY       0
#elif BOARD == STK600_RCUC3L0
#  define EXAMPLE_PWMA_GCLK_ID          AVR32_SCIF_GCLK_PWMA
#  define EXAMPLE_PWMA_GCLK_FREQUENCY   48000000
#  define EXAMPLE_PWMA_GCLK_SOURCE      AVR32_SCIF_GC_USES_CLK_PBA
#  define EXAMPLE_PWMA_OUTPUT_FREQUENCY 2000000
#  define EXAMPLE_PWMA_CHANNEL_ID1      9
#  define EXAMPLE_PWMA_PIN1             AVR32_PWMA_9_PIN
#  define EXAMPLE_PWMA_FUNCTION1        AVR32_PWMA_9_FUNCTION
#  define EXAMPLE_PWMA_CHANNEL_ID2      11 
#  define EXAMPLE_PWMA_PIN2             AVR32_PWMA_11_PIN
#  define EXAMPLE_PWMA_FUNCTION2        AVR32_PWMA_11_FUNCTION
#  define ERROR_LED                     AVR32_PIN_PA08
#  define PWMA_SPREAD                   10
#  define PWMA_INTERRUPT_PRIORITY       0
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_PWMA_GCLK_ID          AVR32_SCIF_GCLK_PWMA
#  define EXAMPLE_PWMA_GCLK_FREQUENCY   48000000
#  define EXAMPLE_PWMA_GCLK_SOURCE      AVR32_SCIF_GCCTRL_OSCSEL_CLK_PBA
#  define EXAMPLE_PWMA_OUTPUT_FREQUENCY 2000000
#  define EXAMPLE_PWMA_CHANNEL_ID1      0
#  define EXAMPLE_PWMA_PIN1             AVR32_PWMA_0_1_PIN
#  define EXAMPLE_PWMA_FUNCTION1        AVR32_PWMA_0_1_FUNCTION
#  define EXAMPLE_PWMA_CHANNEL_ID2      2
#  define EXAMPLE_PWMA_PIN2             AVR32_PWMA_2_2_PIN
#  define EXAMPLE_PWMA_FUNCTION2        AVR32_PWMA_2_2_FUNCTION
#  define ERROR_LED                     AVR32_PIN_PA08
#  define PWMA_SPREAD                   10
#  define PWMA_INTERRUPT_PRIORITY       0
#elif BOARD == STK600_RCUC3L3
#  define EXAMPLE_PWMA_GCLK_ID          AVR32_SCIF_GCLK_PWMA
#  define EXAMPLE_PWMA_GCLK_FREQUENCY   48000000
#  define EXAMPLE_PWMA_GCLK_SOURCE      AVR32_SCIF_GCCTRL_OSCSEL_CLK_PBA
#  define EXAMPLE_PWMA_OUTPUT_FREQUENCY 2000000
#  define EXAMPLE_PWMA_CHANNEL_ID1      9
#  define EXAMPLE_PWMA_PIN1             AVR32_PWMA_9_0_PIN
#  define EXAMPLE_PWMA_FUNCTION1        AVR32_PWMA_9_0_FUNCTION
#  define EXAMPLE_PWMA_CHANNEL_ID2      11 
#  define EXAMPLE_PWMA_PIN2             AVR32_PWMA_11_PIN
#  define EXAMPLE_PWMA_FUNCTION2        AVR32_PWMA_11_FUNCTION
#  define ERROR_LED                     AVR32_PIN_PA08
#  define PWMA_SPREAD                   10
#  define PWMA_INTERRUPT_PRIORITY       0
#endif

/* GLOBAL VARIABLES DECLARATION */
/*! \brief Global variable to Store the PWMA module address */
volatile avr32_pwma_t *pwma = &AVR32_PWMA;

/**
 * \brief TOFL ISR Routine
 * \param tofl_irq                 ISR Routine name
 * \param AVR32_PWMA_IRQ_GROUP     Group under which the PWMA interrupt lies
 * \param PWMA_INTERRUPT_PRIORITY  Priority level of the interrupt
 */
ISR(tofl_irq, AVR32_PWMA_IRQ_GROUP, PWMA_INTERRUPT_PRIORITY) 
{
	/* Clear the PWMA TOFL interrupt */
	pwma->scr=AVR32_PWMA_SCR_TOFL_MASK;
	/* End of ISR routine */
}

/**
 * \brief Main Application Routine
 *  -Initialize the system clocks                               \n
 *  -Configure and Enable the PWMA Generic clock                \n
 *  -Configure and Enable the PWMA Module                       \n
 *  -Load Duty cycle value using Interlinked multi-value mode   \n
 *  -Register and Enable the Interrupt                          \n
 *  -Enter into sleep mode                                      \n
 */
int main (void)
{
	uint32_t div;
	bool config_status = FAIL;
	bool set_value_status = FAIL;

	/*
	 * Duty cycle value to be loaded. As Two channels are used 
	 * in this example, two values has been assigned. Initialize the unused
	 * channel duty cycle value to 0, as it may take some garbage values 
	 * and will return FAIL while updating duty cycle as the value might 
	 * exceed the limit NOTE : Maximum four values can be loaded at a time, 
	 * as the channel limitation for Interlinked multi-value mode is four.
	 */ 
	uint16_t duty_cycle[] = {11,5,0,0};

	/* PWMA Pin and Function Map */
	static const gpio_map_t PWMA_GPIO_MAP = {
		{EXAMPLE_PWMA_PIN1, EXAMPLE_PWMA_FUNCTION1},
		{EXAMPLE_PWMA_PIN2, EXAMPLE_PWMA_FUNCTION2},
	};

	/* Enable the PWMA Pins */
	gpio_enable_module(PWMA_GPIO_MAP,
			sizeof(PWMA_GPIO_MAP) / sizeof(PWMA_GPIO_MAP[0]));

	/*
	 * Calculate the division factor value to be loaded and
	 * Enable the Generic clock
	 */
	div = div_ceil((sysclk_get_pba_hz()) , EXAMPLE_PWMA_GCLK_FREQUENCY);
	genclk_enable_config(EXAMPLE_PWMA_GCLK_ID,EXAMPLE_PWMA_GCLK_SOURCE,div);

	/*
	 * Initialize the System clock 
	 * Note: Clock should be configured in conf_clock.h
	 */
	sysclk_init();
	
	/* Initialize the delay routines */
	delay_init(sysclk_get_cpu_hz());

	/*
	 * Configure and Enable the PWMA Module. The LED will turned if 
	 * configuration fails because of invalid argument.
	 */
	config_status = pwma_config_enable(pwma, EXAMPLE_PWMA_OUTPUT_FREQUENCY,
								EXAMPLE_PWMA_GCLK_FREQUENCY, PWMA_SPREAD);

	/* Error in configuring the PWMA module */
	if (config_status == FAIL){
		while (1) {
			delay_ms(10);
			gpio_tgl_gpio_pin(ERROR_LED);
		}
	}

	/* Load the duty cycle values using Interlinked multi-value mode */
	set_value_status = pwma_set_multiple_values(pwma, 
			((EXAMPLE_PWMA_CHANNEL_ID1<<0) | (EXAMPLE_PWMA_CHANNEL_ID2<<8)),
			(uint16_t*)&duty_cycle);

	/* Error in loading the duty cycle value */
	if (set_value_status == FAIL){
		while (1) {
			delay_ms(10);
			gpio_tgl_gpio_pin(ERROR_LED);
		}
	}

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
	irq_register_handler(&tofl_irq, AVR32_PWMA_IRQ, PWMA_INTERRUPT_PRIORITY);

	/* Enable the Timebase overflow interrupt */
	pwma->ier = AVR32_PWMA_IER_TOFL_MASK;

	/* Enable global interrupt */
	cpu_irq_enable();

	/* Go to sleep mode */
	while(1){
		/* Enter into sleep mode */
		pm_sleep(AVR32_PM_SMODE_FROZEN);
	}
}  /* End of main() */


/* Application Documentation */
/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the PWMA driver.\n It also gives an example
 * of the usage of the PWMA module.\n
 *
 * This example will generate PWMA output waveform on two different channels
 * and generates interrupt when overflow occurs in the timebase counter. It
 * updates the duty cycle value using Interlinked multi-value mode This can
 * update four different channels simultaneously with four different 
 * duty cycle values. The CPU will enter in to sleep mode (Frozen). \n
 *
 * The output PWMA frequency configured in this example is 2MHz and the SPREAD
 * is loaded with the value of 10. So, the output frequency will vary from
 * 1.4MHz to 3.4MHz. The duty cycle value of one channel is 11 which is 50% 
 * duty cycle. For 2nd channel, the duty cycle value is 5, which is 25%. 
 * 
 * \section files Main files
 * - pwma.c           : PWMA Driver file
 * - pwma.h           : PWMA Header file
 * - conf_clock.h     : Clock configuration
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
 * - ATUC[64/128]D3/D4 devices
 *
 * \section testinfo Test Information
 * This example has been tested with the following boards:
 * - UC3L-EK evaluation kit with an AT32UC3L064 MCU.
 * - UC3L0-Xplained kit with an AT32UC3L064 MCU.
 * - STK600 + RCUC3L0 routing card with an AT32UC3L0256 MCU.
 * - STK600 + RCUC3D routing card with an ATUC128D3 MCU.
 * - STK600 + RCUC3L3U routing card with an ATUC256L3U MCU.
 *
 * \section setupinfo Setup Information
 * - CPU Clock Source : DFLL(48MHz) with internal RCSYS as source
 *                      PLL(48MHz) with OSC0 as source for UC128D3 MCU \n
 * - CPU Speed        : 48MHz
 * - PBA Speed        : 48MHz
 * - PBB Speed        : 48MHz
 * - PWMA_GCLK Source : PBA Clock
 * - PWMA_GCLK Speed  : 48MHz
 *
 * \section setupinfo Hardware Setup Information
 *
 * In UC3L0-XPLAINED KIT: \n
 *  - PWMA Channels used : channel 11 and channel 27
 *  - The PWMA output channel is routed to LED0 and LED1 respectively.
 *  - The output can also be measured with Oscilloscope as given below:\n
 *   --------------------------------------------------------------   \n
 *  | PWMA Channel   |   LED on Board |    OUTPUT pins on Board    |  \n
 *   --------------------------------------------------------------   \n
 *  | channel 11     |     LED0       | Pin 8(PWMA7) of Header J3  |  \n
 *  | channel 27     |     LED1       | Pin 7(PWMA6) of Header J3  |  \n
 *   --------------------------------------------------------------   \n
 *  - The LED2 will be turned on in case of any failure in the PWMA 
 *    configuration.
 *
 * In UC3L-EK KIT: \n
 *  - PWMA Channels used : channel 21 and channel 33
 *  - The PWMA output channel is routed to LED0 and LED1 respectively.
 *  - The output can also be measured with Oscilloscope as given below:\n
 *   --------------------------------------------------------------   \n
 *  | PWMA Channel   |   LED on Board |    OUTPUT pins on Board    |  \n
 *   --------------------------------------------------------------   \n
 *  | channel 21     |     LED0       | Pin 7(PA21) of Header J15  |  \n
 *  | channel 33     |     LED1       | Pin 16(PB10) of Header J15 |  \n
 *   --------------------------------------------------------------   \n
 *  - The LED2 will be turned on in case of any failure in the configuration \n
 *
 * For STK600 + RCUC3L0 routing card with an AT32UC3L0256 MCU:
 *  - PWMA Channels used : channel 9 and channel 11.
 *  - Pin mapping in STK600:
 *   -----------------------------------------   \n
 *  | PWMA Channel  |  Port No. |  In STK600  |  \n
 *   -----------------------------------------   \n
 *  | channel 9     |   PA09    |  PortB.PB1  |  \n
 *  | channel 11    |   PA11    |  PortB.PB3  |  \n
 *   -----------------------------------------   \n
 *  - Output can be measured through oscilloscope or it can be  connected
 *    to LEDs on the STK600 board.
 *  - The port PA08 (portB.PB0 in STK600) can be connected to any of 
 *    the LEDs on the board. This will be turned on in case of any 
 *    failure in the PWMA configuration. \n
 *
 * For STK600 + RCUC3D routing card with an ATUC128D3 MCU:
 *  - PWMA Channels used : channel 0 and channel 2
 *  - Before start programming, switch the clock to external(EXT) in
 *    the STK600 board. And in programming window, write the Oscillator 
 *    frequency as 12MHz in the Board Settings options.
 *  - Pin mapping in STK600:
 *   -----------------------------------------   \n
 *  | PWMA Channel  |  Port No. |  In STK600  |  \n
 *   -----------------------------------------   \n
 *  | channel 0     |   PA11    |  PortB.PB3  |  \n
 *  | channel 2     |   PA13    |  PortB.PB5  |  \n
 *   -----------------------------------------   \n
 *  - Output can be measured through oscilloscope or it can be  connected
 *    to LEDs on the STK600 board \n
 *  - The port PA08 (portB.PB0 in STK600) can be connected to any of 
 *    the LEDs on the board. This will be turned on in case of any 
 *    failure in the PWMA configuration. \n
 *
 * For STK600 + RCUC3L3U routing card with an ATUC256L3U MCU:
 *  - PWMA Channels used : channel 0 and channel 2
 *  - Pin mapping in STK600:
 *   -----------------------------------------   \n
 *  | PWMA Channel  |  Port No. |  In STK600  |  \n
 *   -----------------------------------------   \n
 *  | channel 9    |   PA09    |  PortB.PB1   |  \n
 *  | channel 11   |   PA11    |  PortB.PB3   |  \n
 *   -----------------------------------------   \n
 *  - Output can be measured through oscilloscope or it can be  connected
 *    to LEDs on the STK600 board \n
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
