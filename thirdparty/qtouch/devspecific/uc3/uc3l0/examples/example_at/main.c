/**
 * \file
 *
 * \brief Autonomous QTouch Example with CAT module using SleepWalking feature
 *        in AT32UC3L series.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/

/*! Compiler information */
#if !((defined(__GNUC__) && defined(__AVR32__)) || (defined(__ICCAVR32__)\
	|| defined(__AAVR32__)))
#error 'This compiler is not supported at the moment.'
#endif

/*! Includes for ASF */
#include <stdbool.h>
#include <stdint.h>
#include "compiler.h"
#include "board.h"
#include "conf_example.h"
/*! Include ASF drivers */
#include "ast.h"
#include "gpio.h"
#include "pm_uc3l.h"
#include "scif_uc3l.h"
#if defined(__GNUC__)
#include "intc.h"
#endif
/*! Include ASF services */
#include "sysclk.h"
#include "sleepmgr.h"
#include "status_codes.h"

/*! Includes for Touch Library. */
#include "touch_api_at32uc3l.h"

/*! Includes for Touch Debug interface. */
#if DEF_TOUCH_QDEBUG_ENABLE == 1
#include "QDebugSettings.h"
#include "QDebug_at32uc3l.h"
#endif

/*! \name Basic Configuration for Example */
/*!@{ */

/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief DFLL using internal RCSYS as source. Clock source used by
 *        sysclk_init() to intialize system clock.\n
 *        Defined in conf_clock.h
 *
 * When using a different frequency setting, the following parameters must
 * be changed accordingly to ensure proper QTouch operation.
 *    - AT_CAT_CLK_DIV.
 *    - TOUCH_SPREAD_SPECTRUM_MAX_DEV, when Spread spectrum is enabled.
 *    - PBA_HZ, when using qdebug/SPI_Master.c
 *    - TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ, when using qdebug/SERIAL.c
 */

/**
 * \def STATUS_LED
 * \brief LED1_GPIO is used as status LED.
 */

/**
 * \def ERROR_LED
 * \brief LED0_GPIO is used as error status LED.
 */

/**
 * \def POWER_SUPPLY_MODE_1_8V
 * \brief Enable when operated in 1.8V VDDIN
 */

/**
 * \def AST_CLOCK_SOURCE
 * \brief 32KHz external oscillator is used as AST source
 */

/**
 * \def AST_PRESCALER
 * \brief Prescaler is set to AST_PSEL_32KHZ_1HZ to run at 1Hz
 */

/**
 * \def AST_TRIGGER_PRESCALER
 * \brief Periodic interrupt prescaler is set to 10 for trigger in every 62.5ms
 */

/**
 * \def AT_SP_SELECTED
 * \brief CS pin Pair for Autonomous touch
 */

/**
 * \def AT_CAT_CLK_DIV
 * \brief Prescaler to set CAT module Autonomous Touch burst frequency
 */
/*!@} */

/*! \name APPLICATION SPECIFIC CONFIGURATION */
/*!@{ */
/* Touch Status is displayed by this LED. */
#define STATUS_LED LED1_GPIO
/* Error Status is displayed by this LED. */
#define ERROR_LED  LED0_GPIO

/*
 * Power Supply Mode -> 1.8V or 3.3V operation
 * CAUTION: Internal voltage regulator cannot be disabled in 3.3V operation
 * and the application will hang when trying to do so. Hence, this should
 * not be set to true when in 3.3V supply mode.
 */
#define POWER_SUPPLY_MODE_1_8V  false

/* AST Configuration */
/* Configure the AST Clock Source */
#define AST_CLOCK_SOURCE        AST_OSC_32KHZ
/* Configure AST to run at 1Hz frequency */
#define AST_PRESCALER           AST_PSEL_32KHZ_1HZ

/*
 * Configure AST to generate periodic trigger for CAT module every 62.5ms
 * 1000* (2 ^ (prescaler + 1)) / 32768 = 62.5 -> prescaler = 10
 */
#define AST_TRIGGER_PRESCALER   10

/*
 * Number of used GPIO pins
 * CAT Input pins              -  2
 * Oscillator Input pins       -  2
 * LEDS used                   -  2
 * SPI pins(if QDebug enabled) -  4
 */
#if DEF_TOUCH_QDEBUG_ENABLE == 0
#  define NB_GPIO_USED_PINS     6
#else
#  define NB_GPIO_USED_PINS     10
#define QDEBUG_USART            QDEBUG_SPI_USART
#endif
/*!@} */

/* GLOBAL VARIABLES DECLARATION */

/**
 * Flag set by touch_at_status_change_interrupt_callback() function when a
 * fresh touch status is available.
 */
volatile int8_t autonomous_qtouch_in_touch = -1;

/**
 * Dummy variable for QDebug protocol for compatibility with QMatrix and QTouch
 */
uint16_t measurement_period_ms;

/*! \brief Array to hold the used GPIO pins */
unsigned int gpio_used_pins[NB_GPIO_USED_PINS] = {
	EXAMPLE_CAT_SNS_PIN,
	EXAMPLE_CAT_SNSK_PIN,
	STATUS_LED,
	ERROR_LED,
#if DEF_TOUCH_QDEBUG_ENABLE != 0
	QDEBUG_SPI_SCK_PIN,
	QDEBUG_SPI_MISO_PIN,
	QDEBUG_SPI_MOSI_PIN,
	QDEBUG_SPI_NSS_PIN,
#endif
#if BOARD_OSC32_PINSEL
	AVR32_SCIF_XIN32_2_0_PIN,
	AVR32_SCIF_XOUT32_2_0_PIN
#else
	AVR32_SCIF_XIN32_0_PIN,
	AVR32_SCIF_XOUT32_0_PIN
#endif
};

/* STATIC VARIABLES DECLARATION */

/**
 * Autonomous QTouch Configuration structure provided as input to Touch Library
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_at_config_t at_config = {
	{
		AT_SP_SELECTED     /* Autonomous Touch Sense pair selected. */
	},
	{
		AT_CAT_CLK_DIV,    /* Autonomous Touch Burst timing clock
		                    * divider. */
		AT_CHLEN,          /* chlen; ATCFG0 Register. */
		AT_SELEN,          /* selen; ATCFG0 Register. */
		AT_DISHIFT,        /* dishift; ATCFG1 Register. */
		AT_ENABLE_EXTERNAL_SYNC, /* sync; ATCFG1 Register. */
		AT_ENABLE_SPREAD_SPECTRUM, /* spread; ATCFG1 Register. */
		AT_DILEN,          /* dilen; ATCFG1 Register. */
		AT_MAX_ACQ_COUNT,  /* max; MGCFG1 Register. */
		{
			AT_FILTER,     /* Autonomous Touch Filter; ATCFG2
			                * register. */
			AT_OUTSENS,    /* Out-of-Touch Sensitivity; ATCFG2
			                * register. */
			AT_SENSE,      /* Sensitivity; ATCFG2 register. */
			AT_PTHR,       /* Positive Recalibration Threshold;
			                * ATCFG3 register. */
			AT_PDRIFT,     /* Positive Drift Compensation; ATCFG3
			                * register. */
			AT_NDRIFT,     /* Negative Drift Compensation; ATCFG3
			                * register. */
		}
	}
};

/**
 * General configuration options common to QMatrix, QTouch and Autonomous Touch
 * provided as input to Touch library.
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_general_config_t general_config = {
	/*
	 * Specify the sync pin option, if included in the hardware design.
	 * Refer general_pin_options_t.
	 */
	TOUCH_SYNC_PIN_OPTION,
	/* uint8_t  maxdev; SSCFG Register. Max deviation of spread spectrum. */
	TOUCH_SPREAD_SPECTRUM_MAX_DEV,
	/* uint32_t csares; CSA Resistor control register. */
	TOUCH_CSARES,
	/* uint32_t csbres;  CSA Resistor control register. */
	TOUCH_CSBRES
};

/**
 * Touch Library input configuration structure.
 */
touch_config_t touch_config = {
	NULL,              /* Pointer to QMatrix configuration structure. */
	&at_config,        /* Pointer to Autonomous Touch configuration
	                    * structure. */
	NULL,              /* Pointer to QTouch Group A configuration structure. */
	NULL,              /* Pointer to QTouch Group B configuration structure. */
	&general_config    /* Pointer to General configuration structure. */
};

/* FUNCTION PROTOTYPES */

/**
 * \brief Initialize the touch library and touch sensors
 */
static status_code_t touch_api_init(void);

/**
 * \brief Autonomous QTouch Group status change callback function example
 * prototype.
 */
static void touch_at_status_change_interrupt_callback(touch_at_status
		*p_at_status);

/**
 * \brief Initialize Asynchronous Timer to trigger CAT module at regular
 * intervals.
 */
#if DEF_TOUCH_QDEBUG_ENABLE == 0
static status_code_t ast_init(void);

#endif

/**
 * \brief Initialize the power saving measures to lower power consumption
 */
static void power_save_measures_init(void);

/* FUNCTION DEFINITIONS */

/**
 * \brief Main Application Routine
 *  - Initialize the system clocks
 *  - Initialize the sleep manager
 *  - Initialize the power save measures
 *  - Initialize the touch library and sensors
 *  - Initialize the AST to trigger CAT at regular intervals
 *  - Go to STATIC sleep mode and wake up on a touch status change
 */
int main(void)
{
#if DEF_TOUCH_QDEBUG_ENABLE == 1
	uint32_t delay_counter;
#endif
	uint32_t i;
	/* Switch on the STATUS LED */
	gpio_clr_gpio_pin(STATUS_LED);
	/* Switch off the error LED. */
	gpio_set_gpio_pin(ERROR_LED);

	/*
	 * Initialize the system clock.
	 * Note: Clock settings are specified in conf_clock.h
	 */
	sysclk_init();

	/*
	 * Initialize the sleep manager.
	 * Note: CONFIG_SLEEPMGR_ENABLE should have been defined in conf_sleepmgr.h
	 */
	sleepmgr_init();
	/* Lock required sleep mode. */
	sleepmgr_lock_mode(SLEEPMGR_STATIC);

	/* Initialize the power saving features */
	power_save_measures_init();

	/* Switch off the error LED. */
	gpio_set_gpio_pin(ERROR_LED);

#if DEF_TOUCH_QDEBUG_ENABLE == 0
	/* Initialize the AST peripheral */
	if (ast_init() != STATUS_OK) {
		/* Error initializing the AST peripheral */
		while (1) {
			for (i = 0; i < 10000; i++) {
			}
			gpio_tgl_gpio_pin(ERROR_LED);
		}
	}

#endif

	/* Initialize the touch library */
	if (touch_api_init() != STATUS_OK) {
		/* Error initializing the touch sensors */
		while (1) {
			for (i = 0; i < 10000; i++) {
			}
			gpio_tgl_gpio_pin(ERROR_LED);
		}
	}

#if DEF_TOUCH_QDEBUG_ENABLE == 1
	/* Enable PBA clock for AST clock to switch its source */
	sysclk_enable_peripheral_clock(QDEBUG_USART);
	/* 
	 * Initialize the QDebug interface.
	 * QT600 USB Bridge two-way QDebug communication.
	 */
	QDebug_Init();
#endif

	/* Turn OFF the Status LED */
	gpio_set_gpio_pin(STATUS_LED);

	/* Loop forever */
	while (1) {
#if DEF_TOUCH_QDEBUG_ENABLE == 1
		/* Process any commands received from QTouch Studio. */
		QDebug_ProcessCommands();

		/*
		 * Send out the Touch debug information data each time when
		 * Touch measurement process is completed.
		 * param - 0x000A -> Enable TOUCH_STATUS_CHANGE &
		 * TOUCH_CHANNEL_REF_CHANGE
		 * qt_lib_flags always for Autonomous QTouch.
		 */
		QDebug_SendData(0x000A);

		/* Delay to avoid sending the data to QTouch Studio too frequently. */
		for (delay_counter = 0u; delay_counter < 1200u;
				delay_counter++) {
		}

#else
		/* Enable Asynchronous Wakeup for CAT module */
		pm_asyn_wake_up_enable(AVR32_PM_AWEN_CATWEN_MASK);
		/* Disable GPIO clock after waking from sleep mode */
		sysclk_disable_pba_module(SYSCLK_GPIO);

		/* Enter STATIC sleep mode */
		sleepmgr_enter_sleep();

		/* Disable Asynchronous Wakeup for CAT module. */
		pm_asyn_wake_up_disable(AVR32_PM_AWEN_CATWEN_MASK);

		/* Clear All AST Interrupt request and clear SR */
		ast_clear_all_status_flags(&AVR32_AST);

		/**
		 * When woken up by Autonomous QTouch interrupt, the
		 * touch_at_status_change_interrupt_callback() is called that
		 * updates the autonomous_qtouch_in_touch status flag.
		 */

		/* Host application code goes here */
#endif
	}
} /* End of main() */

/**
 * \brief Autonomous QTouch status change interrupt callback function.
 * This callback function is called by the Touch library in the
 * CAT Autonomous QTouch status change Interrupt context, each time
 * there is a status change in the Autonomous Touch sensor.
 *
 * \param  p_at_status: Autonomous QTouch status.
 *         p_at_status->status_change: Autonomous QTouch status change.
 *         p_at_status->base_count: Autonomous QTouch base count value.
 *         p_at_status->current_count: Autonomous QTouch current count value.
 * \note   1. CAUTION - This callback function is called in the CAT Autonomous
 * QTouch Status change INTERRUPT SERVICE ROUTINE by the Touch Library.
 * 2. The Autonomous QTouch Status change callback is called both for
 * an IN_TOUCH status change and an OUT_OF_TOUCH status change.
 */
void touch_at_status_change_interrupt_callback(touch_at_status *p_at_status)
{
	/* Enable GPIO clock after waking from sleep mode */
	sysclk_enable_pba_module(SYSCLK_GPIO);
	if (p_at_status->status_change == IN_TOUCH) {
		autonomous_qtouch_in_touch = 1u;

#if DEF_TOUCH_QDEBUG_ENABLE == 0
		/* Turn ON LED0 once Autonomous QTouch sense is detected. */
		gpio_clr_gpio_pin(STATUS_LED);
#endif
	} else {
		autonomous_qtouch_in_touch = 0u;
#if DEF_TOUCH_QDEBUG_ENABLE == 0
		/* Turn ON LED0 once Autonomous QTouch sense is detected. */
		gpio_set_gpio_pin(STATUS_LED);
#endif
	}
} /* End of touch_at_status_change_interrupt_callback() */

/**
 * \brief Touch Library & Sensors Intialization
 * - Register the CAT interrupt with priority level 3
 * - Initialize the touch library
 * - Intilialize the Autonomous touch sensor
 *
 * \retval STATUS_OK         Configuration success
 * \retval ERR_INVALID_ARG   Error in configuration parameters
 */
static status_code_t touch_api_init()
{
	touch_ret_t touch_ret = TOUCH_SUCCESS;
	/* Enable CAT PBA clock */
	sysclk_enable_pba_module(SYSCLK_CAT);
	/* Disable global interrupts */
	cpu_irq_disable();

	/*
	 * Initialize the interrupt vectors
	 * Note: This function does nothing for IAR as the interrupts are
	 *handled
	 * by the IAR compiler itself. It provides an abstraction between GCC &
	 * IAR compiler to use interrupts.
	 * Refer function implementation in interrupt_avr32.h
	 */
	irq_initialize_vectors();

	/*
	 * Register the Touch Library CAT interrupt handler to the interrupt
	 * controller.
	 * Note: The Touch Library CAT interrupt level for the case
	 * of IAR is fixed to Interrupt level 3. This function does nothing for
	 * IAR as the interrupts are handled by the IAR compiler itself. It
	 * provides an abstraction between GCC & IAR compiler to use interrupts.
	 * Refer function implementation in interrupt_avr32.h
	 */
	irq_register_handler(&touch_acq_done_irq, AVR32_CAT_IRQ, 3);
	/* Enable global interrupt */
	cpu_irq_enable();

	/* Initialize touch library and CAT module for Autonomous QTouch
	 * operation. */
	touch_ret = touch_at_sensor_init(&touch_config);
	/* Check API Error return code. */
	if (touch_ret != TOUCH_SUCCESS) {
		return ERR_INVALID_ARG;
	}

	/*
	 * Enable Autonomous QTouch sensor for continuous acquisition. IN_TOUCH
	 * or OUT_OF_TOUCH status is continuously updated until the sensor is
	 * disabled using the touch_at_sensor_disable() API.
	 */
	touch_ret
		= touch_at_sensor_enable(touch_at_status_change_interrupt_callback);
	/* Check API Error return code. */
	if (touch_ret != TOUCH_SUCCESS) {
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
} /* End of touch_api_init() */

/**
 * \brief  Asynchronous Timer Initialization
 * - Start the 32KHz Oscillator
 * - Initializes the AST module with periodic trigger events
 *
 * \retval STATUS_OK   Configuration OK
 * \retval ERR_BUSY    Error in configuring the AST module
 */
#if DEF_TOUCH_QDEBUG_ENABLE == 0
static status_code_t ast_init()
{
	/* Initial Count value to write in AST */
	unsigned long ast_counter = 0;
	/* Set the prescaler to set a periodic trigger from AST */
	avr32_ast_pir0_t pir = {
		.insel = AST_TRIGGER_PRESCALER
	};
	/* Set the OSC32 parameters */
	scif_osc32_opt_t osc32_opt = {
#if BOARD_OSC32_IS_XTAL
		.mode = SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR,
#else
		.mode = SCIF_OSC_MODE_EXT_CLK,
#endif
		.startup = OSC32_STARTUP_8192,
		.pinsel = BOARD_OSC32_PINSEL,
		.en1k = false,
		.en32k = true
	};

	/* Enable the 32KHz Oscillator */
	scif_start_osc32(&osc32_opt, true);
	/* Enable the Peripheral Event System Clock */
	sysclk_enable_hsb_module(SYSCLK_EVENT);
	/* Enable PBA clock for AST clock to switch its source */
	sysclk_enable_pba_module(SYSCLK_AST);
	/* Initialize the AST in counter mode */
	if (!ast_init_counter(&AVR32_AST, AST_CLOCK_SOURCE, AST_PRESCALER,
			ast_counter)) {
		return ERR_BUSY;        /* Check AST timer */
	}

	/* Initialize the periodic value register with the prescaler */
	ast_set_periodic0_value(&AVR32_AST, pir);
	/* Enable the AST periodic event */
	ast_enable_periodic0(&AVR32_AST);

	/* Clear All AST Interrupt request and clear SR */
	ast_clear_all_status_flags(&AVR32_AST);

	/* Enable the AST */
	ast_enable(&AVR32_AST);

	/* Disable PBA clock for AST after switching its source to OSC32 */
	sysclk_disable_pba_module(SYSCLK_AST);

	return STATUS_OK;
} /* End of ast_init() */

#endif

/**
 * \brief  Low Power Configuration
 * Initializes the power saving measures to reduce power consumption
 * - Enable pull ups on GPIO pins
 * - Disable the clocks to unwanted modules
 * - Disable internal voltage regulator when in 1.8V supply mode
 */
void power_save_measures_init()
{
	uint8_t i;
	uint32_t gpio_mask[AVR32_GPIO_PORT_LENGTH] = {0};

	/*
	 * Enable internal pull-ups on all unused GPIO pins
	 * Note: Pull-ups on Oscillator or JTAG pins can be enabled only if they
	 * are not used as an oscillator or JTAG pin respectively.
	 */
	for (i = 0; i < (sizeof(gpio_used_pins) / sizeof(uint32_t)); i++) {
		gpio_mask[gpio_used_pins[i] >>
		5] |= 1 << (gpio_used_pins[i] & 0x1F);
	}
	for (i = 0; i < AVR32_GPIO_PORT_LENGTH; i++) {
		gpio_configure_group(i, gpio_mask[i],
				GPIO_PULL_UP | GPIO_DIR_INPUT);
	}
	/* Disable OCD clock which is not disabled by sysclk service */
	sysclk_disable_cpu_module(SYSCLK_OCD);
#if POWER_SUPPLY_MODE_1_8V

	/*
	 * When using 1.8V Single supply mode, the Voltage Regulator can be
	 * shut-down using the code below, in-order to save power.
	 * See Voltage Regulator Calibration Register in datasheet for more
	 *info.
	 * CAUTION: When using 3.3V Single supply mode, the Voltage Regulator
	 * cannot be shut-down and the application will hang in this loop.
	 */
	uint32_t tmp = (AVR32_SCIF.vregcr);
	tmp &= (~(1 << 5));
	AVR32_SCIF.unlock = 0xAA000000 | AVR32_SCIF_VREGCR;
	AVR32_SCIF.vregcr = tmp;
	/* Wait until internal voltage regulator is disabled. */
	while ((AVR32_SCIF.vregcr & 0x20)) {
	}
#endif
} /* End of power_save_measures_init() */

/* Application Documentation */

/**
 * \defgroup
 *group_avr32_services_qtouch_devspecific_uc3_uc3l0_examples_example_at
 * \mainpage
 * \section intro Introduction
 *  The Example projects demonstrates:
 *  - Sleepwalking Feature in AT32UC3L devices : \n
 *    SleepWalking feature allows to reduce the power consumption of the
 *    peripherals in sleep modes. It allows the UC3L Event System to handle
 *    asynchronous events in various sleep modes by requesting a module local
 *    clock for the duration of the Event processing. When the event processing
 *    ends, the clock request is disserted and the module goes back to sleep.
 *
 *  - Autonomous QTouch in Deep Sleep modes using Sleepwalking feature: \n
 *    The example is used to test the sleepwalking feature in deep Sleep mode
 *    since an external 32KHz crystal is available on this board.
 *    The Asynchronous timer is configured to periodically generate an
 *    peripheral event. When the device is set to STATIC Sleep mode, the
 *    Asynchronous Timer (AST) uses the 32KHz external clock. A single
 *    peripheral event from AST triggers one Autonomous QTouch measurement.
 *    This happens periodically using the Sleepwalking feature without waking
 *    the entire CPU.  The CPU is not woken up unless there is a IN_TOUCH or
 *    OUT_OF_TOUCH detection in the Autonomous QTouch sensor.
 *
 *  - QTouch Studio mode: \n
 *    When the define DEF_TOUCH_QDEBUG_ENABLE in the touch_config_at32uc3l.h
 *    file is set to 1, this example project can be used to connect to QTouch
 *    studio. STK600 board and QT600 USB Bridge can be used to verify this. The
 *    CPU will not enter sleep mode in this configuration.
 *
 * \section files Main Files
 * - conf_clock.h: Clock Configuration
 * - touch_config_at32uc3l.h: Touch Library Configuration
 * - main.c: Autonomous Touch Example Application
 *
 * \section supportinfo For support queries on,
 *  - QTouch Library usage
 *  - Capacitive Touch Sensor Tuning
 *  - Capacitive Touch Schematic design
 *  - Capacitive Touch Sensor design
 *  refer to http://www.atmel.com/design-support/
 *
 * \section referenceinfo QTouch Library Documentation
 *  - Refer doc8207: Atmel QTouch Library User Guide.
 *  - Refer QTAN0062: QTouch and QMatrix Sensitivity Tuning for Keys,
 *    Sliders and Wheels. -> For Capacitive Touch Sensor tuning guidelines.
 *  - Refer doc10620.pdf: Touch Sensors Design Guide. -> For Capacitive Touch
 *    Sensor design.
 *
 * \section libinfo Touch Library Configuration Information
 *  - The Example application uses a CPU, PBA and PBB clock of 48MHz.
 *    When using a different frequency setting, the following parameters must
 *    be changed accordingly to ensure proper QTouch operation.
 *    - AT_CAT_CLK_DIV.
 *    - TOUCH_SPREAD_SPECTRUM_MAX_DEV, when Spread spectrum is enabled.
 *    - PBA_HZ, when using qdebug/SPI_Master.c
 *    - TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ, when using qdebug/SERIAL.c
 *
 *  - For the case of Autonomous QTouch, the following settings can be modified
 *    by the QTouch Studio.
 *    - Detect Threshold (Sense level) - AT_SENSE.
 *    - Detect Integration - AT_FILTER.
 *    - Negative Drift rate - AT_NDRIFT.
 *    - Positive Drift rate - AT_PDRIFT.
 *
 *  - When two or more acquisition methods are used, care must be taken such
 *    that a given port pin is not used by more than one method at the same
 *    time.  The following pin configuration options available in
 *    touch_config_at32uc3l.h must be carefully chosen to avoid overlapping.
 *    - QMatrix Pin Configuration Options.
 *    - Autonomous QTouch Pin Configuration Options.
 *    - QTouch Group A Pin Configuration Options.
 *    - QTouch Group B Pin Configuration Options.
 *    - Touch Sync Pin option.
 *
 * \section compinfo Compiler Support
 * The example application supports:
 *  - IAR EWAVR32
 *  - GNU GCC for AVR32
 *
 * \section deviceinfo Device Support
 *  - AT32UC3L0 Series
 *
 * \section testinfo Test Information
 * This example has been tested with the following setup:
 *  - AVR Studio 5.0
 *  - IAR32 Embedded Workbench
 *  - STK600 development board, UC3-L0-XPLAINED & UC3L-EK evaluation kits
 *  - 1.8V and 3.3V Supply mode
 *
 * \section setupinfo Setup Information
 *  - CPU Clock Source  : DFLL with internal RCSYS as its source
 *  - AST Clock Source  : 32KHz external oscillator
 *  - CPU Speed         : 48MHz
 *  - PBA Speed         : 48MHz
 *  - PBB Speed         : 48MHz
 *  - AST Trigger Period: Every 62.5ms
 *  - UC3L-EK: Sense Pair 3 (SP3) is used \n
 *    Important Note: The (csa3/csb3) Touch channel connections are
 *    multiplexed with the JTAG pins. So, when using the JTAG debugging mode,
 *    these Touch channel connections MUST be removed. In the Flash mode, this
 *    will not cause any issues. \n
 *    ------------------------------------------------  \n
 *     CAT CSA/CSB name - UC3L-EK board Port-pin name   \n
 *    ------------------------------------------------  \n
 *    KEY 1 \n
 *    csa3 (SNS3)      -   PA02  //Pin 3 on JTAG Header \n
 *    csb3 (SNSK3)     -   PA03  //Pin 9 on JTAG Header \n
 *
 *  - UC3-L0-XPLAINED: Sense Pair 15 (SP15) is used     \n
 *    ------------------------------------------------  \n
 *     CAT CSA/CSB name - UC3L-EK board Port-pin name   \n
 *    ------------------------------------------------  \n
 *    QT0 Button \n
 *    csa15 (SNS15)    -   PB12  //Pin 5 on J3 Header   \n
 *    csb15 (SNSK15)   -   PB09  //Pin 6 on J3 Header   \n
 *
 *  - STK600-RCUC3L0: Sense Pair 15 (SP15) is used \n
 *    ------------------------------------------------  \n
 *     CAT CSA/CSB name - STK600 board Port-pin name    \n
 *    ------------------------------------------------  \n
 *    KEY 1 \n
 *    csa15 (SNS15)    -   pe4 \n
 *    csb15 (SNSK15)   -   pe1 \n
 *    ------------------------------------------------  \n
 *     QT600 USB Bridge \n
 *     'TOUCH DATA' Header Pin name - STK600 board Port-pin name \n
 *    ------------------------------------------------  \n
 *    PA22 - 'TOUCH DATA' header pin 8 - clk     - pc6  \n
 *    PA21 - 'TOUCH DATA' header pin 7 - miso    - pc5  \n
 *    PA20 - 'TOUCH DATA' header pin 6 - mosi    - pc4  \n
 *    PA14 - 'TOUCH DATA' header pin 5 - nss     - pb6  \n
 *
 * \section contactinfo Contact Information
 * For further information, visit :
 * <A href="http://www.atmel.com/products/AVR32/"> Atmel AVR32 </A>.\n
 * Technical support & faq, visit :
 * <A href="http://www.atmel.com/design-support/"> Support and FAQ </A>.
 *
 * \author  Atmel Corporation: http://www.atmel.com \n
 */
