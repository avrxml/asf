/**
 * \file
 *
 * \brief MEGA-1284P Xplained Example
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
 * \section board MEGA-1284P Xplained board
 * \image html kit_front.JPG
 * \section intro Introduction
 * This example demonstrates some of the basic features of the MEGA-1284P
 * Xplained kit. This includes sleep modes, reading the light sensor
 * and the ntc sensor.
 *
 * \section files Files:
 * - mega_1284p_xplained_example.c: MEGA-1284P Xplained example application
 * - mega_1284p_xplained_example.h: MEGA-1284P Xplained example header file
 * - init.c: Initialization driver
 * - init.h: Initialization driver
 * - usart.c: USART driver
 * - usart.h: USART driver
 *
 * \section exampledescription Brief description of the example application
 * The example application have two main modes: \ref demo_mode "demo mode"
 * or \ref terminal_mode "terminal mode".
 * Please refer to the \ref application_description "detailed description"
 * for more information.
 *
 * \section compinfo Compilation Info
 * This software was written for the <A href="http://gcc.gnu.org/">GNU GCC</A>
 * and <A href="http://www.iar.com/">IAR</A> for AVR. \n
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

/**
 * \defgroup mega_1284p_xplained_group MEGA-1284P Xplained example application
 *
 * \section application_description Description of the example
 * This is the example application used to demonstrate the MEGA-1284P Xplained
 * kit. Basically it has two main modes of operation: demo mode or
 * terminal mode.
 * \subsection application_flowchart Example application flowchart
 * \image html application_flowchart.JPG
 * \subsection demo_mode Demo mode
 * This mode lets the user select in which mode the ATmega1284P should operate:
 * Active, power-save (with the 32kHz crystal connected to the TOSC pins
 * enabled) or power-down. The user can easily measure the power consumption
 * in the different modes by connecting an ampere meter to the two-pins power
 * header. Please refer to the hardware user guide for more details.
 *
 * The user can also try out capacitive touch sensing by pushing the QTB0 touch
 * button on the board. LED1 will turn on when QTB0 is touched and switch off
 * whenever QTB0 is not touched.
 *
 * By shorting MISO (pin 7) and GND (pin 9) of the J1 header the ATmega1284P
 * enters light sensor demo mode. The light sensor is then used to adjust the
 * light intensity of the four leds (LED0:3). Normal demo mode will be resumed
 * as soon as this short is removed.
 * \subsection demo_mode_flowchart Demo mode flowchart
 * \image html demo_mode_flowchart.JPG
 *
 *
 * \subsection terminal_mode Terminal mode
 * This mode uses USART1 of the ATmega1284P to control the application.
 * Start hyperterminal (or the terminal program of choice) and connect to the
 * COM port where the XPLAINED Virtual Com Port is connected.
 * Select baud rate: 57k6, data bits: 8, parity: none, stop bits: 1 and
 * flow control: none.
 *
 * To start this mode push and hold SW1 while pressing the reset button on the
 * MEGA-1284P Xplained board. Supported commands and a description will
 * then be printed.
 * \subsection terminal_mode_flowchart Terminal mode flowchart
 * \image html terminal_mode_flowchart.JPG
 *
 * @{
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "avr_compiler.h"
#include "usart.h"
#include "init.h"
#include "touch_api.h"

#define ADC_NUM_OVERSAMPLING            16

#define ASCII_BACKSPACE                 8
#define ASCII_CR                        13
#define ASCII_SPACE                     32
#define MAX_CMD_BUFFER_LEN              32

#define CMD_PROMPT                      "\r\nMEGA-1284P Xplained>"

#define FLASHCOUNT                      100

#define IDLE                            0x01  //!< Idle SMCR setting
#define POWER_DOWN                      0x05  //!< Power-Down SMCR setting
#define POWER_SAVE                      0x07  //!< Power-Save SMCR setting
#define STANDBY                         0x0D  //!< Standby SMCR setting
#define EXTENDED_STANDBY                0x0F  //!< Extended standby SMCR setting

#define NUM_COMMANDS                    12

// Forward declarations of command functions
static void print_help (void);
static void flash_leds_cmd (void);
static void read_ntc (void);
static void read_light_sensor (void);
static void light_sensor_demo (void);
static void crystal_start_cmd (void);
static void crystal_stop_cmd (void);
static void idle_mode_cmd (void);
static void power_save_cmd (void);
static void power_down_cmd (void);
static void standby_cmd (void);
static void ext_standby_cmd (void);

//! Struct to hold all available commands, their help text and function to call
struct {
	char cmd[20];        // Command string
	char help[70];       // Help text
	void (* func)(void); // Function to call when command is entered
} commands[NUM_COMMANDS] = {
	{ "help",                "Print this help",                                       print_help },
	{ "flash leds",          "Toggles leds connected to PORTB0:3",                    flash_leds_cmd },
	{ "read ntc",            "Returns NTC ADC code (ADC7)",                           read_ntc },
	{ "read light sensor",   "Returns Light Sensor ADC code (ADC6)",                  read_light_sensor },
	{ "light sensor demo",   "LED3:0 are dimmed according to the light sensor value", light_sensor_demo },
	{ "start 32khz crystal", "Starts the 32.768kHz crystal",                          crystal_start_cmd },
	{ "stop 32khz crystal",  "Stops the 32.768kHz crystal",                           crystal_stop_cmd },
	{ "idle",                "MCU in idle mode. Wake up by pressing SW0",             idle_mode_cmd },
	{ "power-save",          "MCU in Power-Save mode. Wake up by pressing SW0",       power_save_cmd },
	{ "power-down",          "MCU in Power-Down mode. Wake up by pressing SW0",       power_down_cmd },
	{ "standby",             "MCU in Standby mode. Wake up by pressing SW0",          standby_cmd },
	{ "extended standby",    "MCU in Extended Standby mode. Wake up by pressing SW0", ext_standby_cmd },
};

//! Global variable used to identify if demo mode is enabled
static volatile bool light_sensor_demo_mode = false;

//! Macro used for touch key detection
#define QT_KEY_DETECT() \
	(qt_measure_data.qt_touch_status.sensor_states[0] & 0x01)

//! Number of ports using touch
#define NUMBER_OF_PORTS 1

//! Touch acquisition timer period in msec.
uint16_t qt_measurement_period_msec = 25;

//! Flag set by timer1 overflow ISR when it's time to measure touch
static volatile bool time_to_measure_touch = false;

//! Current time, set by timer1 overflow ISR
static volatile uint16_t current_time_ms_touch = 0;

#if defined( __GNUC__ )
/** Make sure printf knows where to print. The macro fdev_setup_stream()
 * is used to prepare a user-supplied FILE buffer for operation with stdio.
 */
FILE usart1_str = FDEV_SETUP_STREAM((int(*)(char, FILE *))usart1_putchar, NULL, _FDEV_SETUP_WRITE);
#endif

//! ADC sources enum
enum adc_sources {
	FILTER_OUTPUT = 0x05, //!< RC filter on ADC Channel 5, PA5
	LIGHT_SENSOR = 0x06,  //!< Light Sensor on ADC Channel 6, PA6
	NTC = 0x07           //!< NTC sensor on ADC Channel 7, PA7
};

/**
 * \brief PCINT8,9,10 ISR used for wake-up from sleep only
 */
ISR(PCINT1_vect)
{
	return;
}

/**
 * \brief TIMER1 overflow ISR used to dim LEDs in light sensor demo mode.
 *
 * LEDs are turned off at overflow ISR and turned on in compare match B ISR.
 * The compare value is adjusted according to the light sensor readings.
 * Brighter light will result in lower light sensor reading (ADC code)
 * which results in shorter time to compare match B interrupt. This means
 * that LEDs will be on longer than off at bright light and vice versa
 * at dark light.
 */
ISR(TIMER1_OVF_vect)
{
	// Turn off LEDs
	PORTB |= (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
	// PORTB0 set to input to enable sensing on SW0
	DDRB &= ~(1 << DDB0);

	//! \note Wait one cycle to be sure PINB0 value has stabilized.
	asm("nop");
	// Stop demo mode if SW0 is pushed
	if (!(PINB & (1 << PINB0))) {
		light_sensor_demo_mode = false;
	}
}

/**
 * \brief TIMER1 compare B ISR used to dim LEDS in light sensor demo mode.
 *
 * LEDs are turned off at overflow ISR and turned on in compare match B ISR.
 * The compare value is adjusted according to the light sensor readings.
 * Brighter light will result in lower light sensor reading (ADC code)
 * which results in shorter time to compare match B interrupt. This means
 * that LEDs will be on longer than off at bright light and vice versa
 * at dark light.
 */
ISR(TIMER1_COMPB_vect)
{
	// Turn on LEDs (make sure PORTB0 is set as output)
	DDRB |= (1 << DDB0);
	PORTB &= ~((1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0));
}

/**
 * \brief TIMER1 compare A ISR used to time touch measurement.
 *
 * The TIMER1 compare A interrupt is used to time the touch acquisition.
 * The period of the timer is set by qt_measurement_period_msec.
 */
ISR(TIMER1_COMPA_vect)
{
	// Set flag, it's time to measure touch
	time_to_measure_touch = true;

	// Update the current time
	current_time_ms_touch += qt_measurement_period_msec;
}

/**
 * \brief TIMER2 overflow ISR used to indicate 32kHz crystal running.
 *
 * All LEDs (LED0:3) are toggled to indicate that the 32kHz crystal is running.
 */
ISR(TIMER2_OVF_vect)
{
	// Toggle LEDs
	PINB |= (1 << PINB3) | (1 << PINB2) | (1 << PINB1) | (1 << PINB0);
}

/**
 * \brief Function to flash leds flashcount amount of times.
 *
 * \param flashcount Number of times to flash LEDs
 */
static void flash_leds(uint8_t flashcount)
{
	uint8_t i;

	for (i = 0; i < flashcount; i++) {
		// toggle LEDs
		PINB |= (1 << PINB3) | (1 << PINB2) | (1 << PINB1) | (1 << PINB0);
		// wait a while
		delay_us(20000);
	}
	// make sure LEDs are off before we leave
	PORTB |= (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
}

/**
 * \brief Function to start ADC conversion.
 *
 * This function will do ADC_NUM_OVERSAMPLINGx oversampling. Per default it
 * will perform 16x oversampling.
 *
 * \param channel    ADC channel number
 *
 * \return adc_data  Result from ADC reading of selected ADC channel
 */
static uint16_t read_adc(enum adc_sources source)
{
  	uint16_t adc_data = 0;

	// Clear old source and setup new source to sample from
	ADMUX &= 0xE0;
	ADMUX |= source;

	for (int i = 0 ; i < ADC_NUM_OVERSAMPLING ; i++) {
		//Start single conversion
		ADCSRA |= (1<<ADSC);

		//Wait for conversion complete
		while(!(ADCSRA & (1<<ADIF)));

		//Read ADC conversion result
		adc_data += ADC;
	}

	adc_data = adc_data/ADC_NUM_OVERSAMPLING;

	return adc_data;
}

/**
 * \brief Function to read NTC ADC value.
 *
 */
static void read_ntc(void)
{
	uint16_t ntc_data;

	// Read NTC sensor
	ntc_data = read_adc(NTC);

	// Send NTC_data to PC
	printf("NTC ADC value: %4d\r\n", ntc_data);
}

/**
 * \brief Function to read Light sensor ADC value.
 */
static void read_light_sensor(void)
{
	uint16_t light_sensor_data;
	// Read LIGHT SENSOR
	light_sensor_data = read_adc(LIGHT_SENSOR);

	// Send LIGHT_SENSOR_data to PC
	printf("LIGHT SENSOR ADC value: %4d\r\n", light_sensor_data);
}

/**
 * \brief Function to start 32.768kHz crystal connected to TOSC1 and TOSC2.
 */
static void start32crystal(void)
{
	// Make sure clock to timer2 is enabled
	PRR0 &= ~(1 << PRTIM2);

	// Start Timer 2, prescale 8
	TCCR2B = (0 << WGM22) | (0 << CS22) | (1 << CS21) | (0 << CS20);

	// Enable clocking of timer2 using 32.768kHz crystal
	ASSR = (1 << AS2);
}

/**
 * \brief Function to stop 32.768kHz crystal connected to TOSC1 and TOSC2.
 */
static void stop32crystal(void)
{
	// Disable clocking of timer2 using 32.768kHz crystal
	ASSR = (0 << AS2);

	// Stop Timer 2
	TCCR2B = (0 << WGM22) | (0 << CS22) | (0 << CS21) | (0 << CS20);

	// Turn off clock to timer2 as it's not in use anymore
	PRR0 |= (1 << PRTIM2);
}

/**
 * \brief Function to enter sleep
 *
 * \param sleepmode Sets sleep mode to enter
 */
static void enter_sleep(uint8_t sleepmode)
{
	uint8_t temp_timsk1;
	uint8_t temp_timsk2;
	uint8_t temp_portb;
	uint8_t temp_ddrb;
	uint8_t temp_prr0;

	// Push temp values and disable interrupts
	temp_timsk1 = TIMSK1;
	temp_timsk2 = TIMSK2;
	temp_portb = PORTB;
	temp_ddrb = DDRB;
	TIMSK1 = 0x00;
	TIMSK2 = 0x00;
	temp_prr0 = PRR0;

	// Enable PORTB0 as input with pull-up enabled
	PORTB |= (1 << PORTB0);
	DDRB &= ~(1 << DDB0);

	// Clear Pin Change Interrupt flag 1
	PCIFR = (1 << PCIF1);

	// Enable pinchange interrupt on SW0 (PCINT8/PB0)
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT10) | (1 << PCINT9) |(1 << PCINT8);

	// Configure sleep mode
	SMCR = sleepmode;

	// Enable power saving features before going to sleep
	power_reduction_enable();

	// Go to sleep
	sleep_enter();

	// Clear SMCR to avoid entering sleep unintended
	SMCR = 0x00;

	// Pull temp values
	PRR0 = temp_prr0;

	// Make sure interrupts flags are reset
	TIFR1 =(1 << OCF1B) | (1 << OCF1A) | (1 << TOV1);
	TIFR2 = (1 << TOV2);

	// Pull temp values
	TIMSK1 = temp_timsk1;
	TIMSK2 = temp_timsk2;
	PORTB = temp_portb;
	DDRB = temp_ddrb;
}

/**
 * \brief Function to handle 'flash leds' command
 */
static void flash_leds_cmd (void)
{
	flash_leds(FLASHCOUNT);
}

/**
 * \brief Function to handle 'light sensor demo' command.
 */
static void light_sensor_demo (void)
{
	timer1_lightdemo_init();
	adc_init();
	light_sensor_demo_mode = true;
	printf("Light sensor demo mode enabled.\r\n");
	printf("Push SW0 to exit light sensor demo.\r\n");

	/*
	* Run light sensor demo as long as light_sensor_demo_mode is true.
	* The light sensor demo mode is disabled in the timer1 overflow ISR.
	*/
	while(light_sensor_demo_mode) {
		// Update compare B value to adjust PWM according to light sensor value
		OCR1B = read_adc(LIGHT_SENSOR);
	}
	stop_timer1_lightdemo();
	printf("Light sensor demo mode stopped.\r\n");
}

/**
 * \brief Function to handle 'start 32kHz crystal' command
 */
static void crystal_start_cmd (void)
{
	start32crystal();
	// Enable timer2 overflow interrupt
	TIMSK2 |= (1<<TOIE2);
	printf("32.768kHz crystal started.\r\n");
}

/**
 * \brief Function to handle 'stop 32kHz crystal' command
 */
static void crystal_stop_cmd (void)
{
	stop32crystal();
	// Disable timer2 overflow interrupt
	TIMSK2 &= ~(1<<TOIE2);
	// make sure LEDs are off before we leave
	PORTB |= (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
	printf("32.768kHz crystal stopped.\r\n");
}

/**
 * \brief Function to handle 'idle' command
 */
static void idle_mode_cmd (void)
{
	printf("Entering idle mode. Push SW0 to wake-up.\r\n");
	while (!(UCSR1A & (1 << TXC1)));
	enter_sleep(IDLE);
	printf("Woke up from idle mode.\r\n");
}

/**
 * \brief Function to handle 'power-save' command
 */
static void power_save_cmd (void)
{
	printf("Entering power-save mode. Push SW0 to wake-up.\r\n");
	while (!(UCSR1A & (1 << TXC1)));
	enter_sleep(POWER_SAVE);
	printf("Woke up from power-save mode.\r\n");
}

/**
 * \brief Function to handle 'power-down' command
 */
static void power_down_cmd (void)
{
	printf("Entering power-down mode. Push SW0 to wake-up.\r\n");
	while (!(UCSR1A & (1 << TXC1)));
	enter_sleep(POWER_DOWN);
	printf("Woke up from power-down mode.\r\n");
}

/**
 * \brief Function to handle 'standby' command
 */
static void standby_cmd (void)
{
	printf("Entering standby mode. Push SW0 to wake-up.\r\n");
	while (!(UCSR1A & (1 << TXC1)));
	enter_sleep(STANDBY);
	printf("Woke up from standby mode.\r\n");
}

/**
 * \brief Function to handle 'extended standby' command
 */
static void ext_standby_cmd (void)
{
	printf("Entering extended standby mode.\r\n");
	printf("Push SW0 to wake-up.\r\n");
	while (!(UCSR1A & (1 << TXC1)));
	enter_sleep(EXTENDED_STANDBY);
	printf("Woke up from extended standby mode.\r\n");
}

/**
 * \brief Function to print supported commands.
 */
static void print_help()
{
	uint8_t cnt;

	printf("\r\n------------------------------------------------------------------------------\r\n");
	printf("#  COMMAND:             DESCRIPTION:\r\n");
	printf("------------------------------------------------------------------------------\r\n");
	for (cnt = 0; cnt < NUM_COMMANDS; cnt++) {
		printf("%-2d %-20s %s\r\n", cnt+1, commands[cnt].cmd, commands[cnt].help);
	}
	printf("------------------------------------------------------------------------------\r\n");
	printf("To perform a command, enter command number or command string\r\n");
}


/**
 * \brief Function used to find and execute commands in terminal mode.
 *
 */
static void process_command(const char *command)
{
	uint8_t cnt;
	uint8_t num_cmd = atoi(command);

	for (cnt = 0; cnt < NUM_COMMANDS; cnt++) {
		if ((strcmp(command, commands[cnt].cmd) == 0) || (num_cmd == cnt+1)) {
			commands[cnt].func();
			return;
		}
	}
	printf("Invalid command. Type 'help' for a list of available commands.\r\n");
}

/** brief Function to execute Demo mode.
 *
 * This mode lets the user select which mode the ATmega1284P should operate:
 * Active, power-save (with the 32kHz crystal connected to the TOSC pins
 * enabled) or power-down. It also includes a light sensor demo mode that
 * uses the light sensor to control the LEDs.
 *
 * Please refer to the \ref demo_mode "detailed description" for more
 * information and flow chart.
 */
static void execute_demo_mode(void)
{
	// Status flags to indicate the re-burst for touch library
	uint16_t status_flag = 0;
	uint16_t burst_flag = 0;

	io_init_demo_mode();
	touch_init();
	sei();

	while (1) {
		if (time_to_measure_touch) {

			time_to_measure_touch = false;
			MCUCR |= (1 << PUD);

			do {
				// One time measure touch sensors
				status_flag = qt_measure_sensors(current_time_ms_touch);
				burst_flag = status_flag & QTLIB_BURST_AGAIN;
				if (QT_KEY_DETECT()) {
					PORTB &= ~(1 << PORTB3);
				}
				else {
					PORTB |= (1 << PORTB3);
				}

			} while (burst_flag);
		MCUCR &= ~(1 << PUD);
		}

		if (!(PINB & (1 << PINB0))) {
			// Turn LED0 on to indicate active mode
			DDRB |= (1 << DDB3) | (1 << DDB0);
			DDRB &= ~((1 << DDB2) | (1 << DDB1));
			PORTB &= ~(1 << PORTB0);
			PORTB |= (1 << PORTB2) | (1 << PORTB1);
		}

		if (!(PINB & (1 << PINB1))){
			// Turn LED2 on to indicate power-save mode
			DDRB |= (1 << DDB3) | (1 << DDB1);
			DDRB &= ~((1 << DDB2) | (1 << DDB0));
			PORTB &= ~(1 << PORTB1);
			PORTB |= (1 << PORTB2) | (1 << PORTB0);

			/*
			 * Start asynchronous clocking of Timer/Counter 2 using
			 * 32.768kHz crystal as clock source.
			 */
			start32crystal();
			enter_sleep(POWER_SAVE);
			stop32crystal();
		}

		if (!(PINB & (1 << PINB2))) {
			// Turn LED3 on to indicate power-down mode
			DDRB |= (1 << DDB3) | (1 << DDB2);
			DDRB &= ~(1 << DDB1) & ~(1 << DDB0);
			PORTB &= ~(1 << PORTB2);
			PORTB |= (1 << PORTB1) | (1 << PORTB0);
			enter_sleep(POWER_DOWN);
		}

		if (!(PINB & (1 << PINB6))) {
			// Disable pinchange interrupts (SW0:2)
			PCICR = (0 << PCIE1);

			adc_init();
			timer1_lightdemo_init();

			// Enable output for LED control of LED0:3
			DDRB |= (1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0);

			// Run light sensor demo as long as PORTB6 is pulled low
			while (!(PINB & (1 << PINB6))) {

				/*
				 * Update compare B value to adjust PWM according
				 * to light sensor value
				 */
				OCR1B = read_adc(LIGHT_SENSOR);
			}

			stop_timer1_lightdemo();

			// Set back to input for demo mode
			DDRB &= ~(1 << DDB2) & ~(1 << DDB1) & ~(1 << DDB0);

			touch_init();

			// Clear Pin Change Interrupt flag 1
			PCIFR = (1 << PCIF1);
			// Enable pinchange interrupts (SW0:2)
			PCICR = (1 << PCIE1);
		}
	}
}

/** brief Function to execute terminal mode.
 *
 * This mode uses USART1 of the ATmega1284P to control the application.
 * Please refef to the  \ref terminal_mode "detailed description" for more
 * information.
 *
 */
static void execute_terminal_mode(void)
{
	// Variable used for USART communication
	uint8_t data = 0;
	// Command buffer pointer used for USART communication
	uint8_t cmd_buf_ptr = 0;
	// Command buffer used for USART communication
	char cmd_buffer[MAX_CMD_BUFFER_LEN];

	// misc initialization
	io_init_terminal_mode();
	usart1_init();
	adc_init();
	// Enable interrupts
	sei();

	flash_leds(20);
	print_help();
	printf(CMD_PROMPT);

	while (1) {

		// Read and handle incoming data on usart1
		data = usart1_getchar();
		switch (data)	{

		case ASCII_CR:
			if (cmd_buf_ptr > 0) {
				usart1_putchar('\n');
				cmd_buffer[cmd_buf_ptr] = '\0';
				process_command(cmd_buffer);
				cmd_buf_ptr = 0;
			}
			printf(CMD_PROMPT);
			break;

		case ASCII_BACKSPACE:
			if (cmd_buf_ptr == 0) {
				break;
			}
			printf("\b \b");
			cmd_buf_ptr--;
			break;

		default:
			if (cmd_buf_ptr >= MAX_CMD_BUFFER_LEN || data < ASCII_SPACE ) {
				// Break if command string is too long/ignore non-printable characters
				break;
			}
			cmd_buffer[cmd_buf_ptr++] = data;
			usart1_putchar(data);
			break;
		}
	}
}

/** brief Example code that demonstrates the some of the basic features
 * of the MEGA-1284P Xplained kit.
 *
 * This example code shows some of the basic features of the
 * MEGA-1284P Xplained kit. This includes sleep modes, reading the light sensor
 * and the ntc sensor.
 *
 * Please refer to the \ref application_description "detailed description"
 * for more information.
 */

int main(void)
{
#if defined( __GNUC__ )
	/* Setup stdout to point to the correct usart (USART1). This is needed to
	 * use the fdev_setup_stream() macro in GCC.
	 */
	stdout = &usart1_str;
#endif

	// Make sure to use lowest possible power consumption
	power_reduction_enable();

	// Enter terminal mode if SW1 is pressed
	if (!(PINB & (1 << PORTB1))) {
		execute_terminal_mode();
	}	else {
		execute_demo_mode();
	}
}

 //! @}
