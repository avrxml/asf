/**
 * \file
 *
 * \brief Initialization file used to init all I/O modules used in the example
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "init.h"

/**
 * \defgroup init_driver_group Initialization driver
 *
 * The init driver is used to initialize IO (demo or terminal mode),
 * power reduction features, USART1, ADC, Timer1 (demo or terminal mode)
 * and touch sensing (QTB0)
 *
 * @{
 */

extern TOUCH_DATA_T SNS_array[2][2];
extern TOUCH_DATA_T SNSK_array[2][2];

/**
 * \brief Function to setup IO for terminal mode.
 *
 * Configure all I/O as input pull-up enabled to make sure all I/Os have
 * a defined level except LEDs (PB3:0) which is configured as outputs.
 * Light sensor and NTC sensor (PA6 and PA7): input pull-up disable.
 */
void io_init_terminal_mode(void)
{
	DDRA = 0x00;
	DDRB = (1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0);
	DDRC = 0x00;
	DDRD = 0x00;
	PORTA = (1 << PORTA5) | (1 << PORTA4) | (1 << PORTA3) | (1 << PORTA2)
		| (1 << PORTA1) | (1 << PORTA0);
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
}

/**
 * \brief Function to setup IO for demo mode.
 *
 * Configure all I/O as input pull-up enabled to make sure all I/Os have
 * a defined level.
 * Light sensor and NTC sensor (PA6 and PA7): input pull-up disable.
 * Enable pinchange interrupt for SW0:2 used to toggle through operating modes
 */
void io_init_demo_mode(void)
{
	DDRA = 0x00;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	PORTA = (1 << PORTA5) | (1 << PORTA4) | (1 << PORTA3)
		| (1 << PORTA2) | (1 << PORTA1) | (1 << PORTA0);
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;

	// Enable pinchange interrupt on PCINT8, PCINT9 and PCINT10
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT10) | (1 << PCINT9) |(1 << PCINT8);
}

/**
 * \brief Function to reduce power consumption.
 *
 * This function will turn off clocks to all IO modules except for
 * Timer2 which is used in power-save mode.
 * The function will also shutdown analog modules like the AC and ADC.
 */
void power_reduction_enable(void)
{
	// Disable Analog Comparator
	ACSR |= (1 << ACD);

	// Turn off ADC
	ADCSRA &= ~(1 << ADEN);

	// Turn off internal bandgap
	ADMUX = 0x00;

	// Digital Input disable
	DIDR0 = 0xFF;

	// Turn off clock to all I/Os except for timer2 (used in power-save mode)
	PRR0 = (1 << PRTWI) | (0 << PRTIM2) | (1 << PRTIM0) | (1 << PRUSART1)
		| (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);
}

/** \brief Function to setup USART1.
 *
 * USART1 is used to communicate with the boardcontroller (AT32UC3256B1).
 *
 * USART1 configuration:
 * - 57600 baud
 * - No parity
 * - 1 stop bit
 * - 8-bit character size
 */
void usart1_init(void)
{
	// Make sure I/O clock to USART1 is enabled
	PRR0 &= ~(1 << PRUSART1);

	// Set baud rate to 57.6k at fOSC = 11.0592 MHz
	UBRR1 = 0x0B;

	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR1A = (1 << TXC1) | (0 << U2X1);

	// Enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);

	// Asynchronous mode, no parity, 1 stop bit, character size = 8-bit
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10) | (0 << UCPOL1);
}

/** \brief Function to setup ADC.
 *
 * ADC configuration:
 * - Internal 2.56V bandgap voltage with 100nF cap at AREF pin as reference
 * - 172.8kHz ADC frequency
 */
void adc_init(void)
{
	// Make sure I/O clock to the ADC is enabled
	PRR0 &= ~(1 << PRADC);

	// Digital Input enable
	DIDR0 = 0x00;

	// Use internal 2.56V bandgap reference with external 100nF cap at AREF pin
	ADMUX = (1 << REFS1) | (1 << REFS0);

	// Enable ADC, ADC frequency set to 11.0592MHz/64 = 172.8kHz
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
}

/** \brief Function to setup timer1 for light sensor demo.
 *
 * In this mode Timer1 is used to generate PWM signal on the LEDs.
 *
 * Timer1 configuration:
 * - Fast PWM
 * - Clear OC1B on compare match, set OC1B at bottom (non-inverting mode)
 * - clkIO/8 prescaling
 * - ICR1 set as TOP
 * - Output compare match B and timer1 overflow interrupts enabled
 */
void timer1_lightdemo_init(void)
{
	// Make sure I/O clock to timer1 is enabled
	PRR0 &= ~(1 << PRTIM1);

	// OC1B output enabled (non-inverting mode), fast PWM ICR1 TOP enabled
	TCCR1A = (1 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (0 << WGM10);

	// Start timer0, clkIO/8 prescaling
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12)
		| (1 << CS11) | (0 << CS10);

	// Adjust top value to match Light sensor output
	ICR1 = 0x3FF;

	// Enable output compare B match and timer1 overflow interrupt
	TIMSK1 = (1 << OCIE1B) | (1 << TOIE1);

}

/** \brief Function to stop timer1 used in the light sensor demo.
 *
 * All timer1 registers are set to default state
 */
void stop_timer1_lightdemo(void)
{
	// Set TCCR1A to default value
	TCCR1A = 0x00;

	// Set TCCR1B to default value
	TCCR1B = 0x00;

	// Clear TCNT1
	TCNT1 = 0x00;

	// Set ICR1 to default value
	ICR1 = 0x00;

	// Disable output compare B match and timer1 overflow interrupt
	TIMSK1 &= ~((1 << OCIE1B) | (1 << TOIE1));

	// Turn off I/O clock to timer1
	PRR0 |= (1 << PRTIM1);

	// Turn off LEDs
	PORTB |= (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
}

/** \brief Function to setup timer1 as a time base for the touch lib.
 *
 * In this mode Timer1 is used to generate PWM signal on the LEDs.
 *
 * Timer1 configuration:
 * - CTC mode (clear on compare match A)
 * - clkIO/8 prescaling
 */
void timer1_init(void)
{
	// Make sure I/O clock to timer1 is enabled
	PRR0 &= ~(1 << PRTIM1);

	// Set timer compare value (how often timer ISR will fire)
	OCR1A = (TICKS_PER_MS * qt_measurement_period_msec);

	// Enable timer1 output compare match A ISR
	TIMSK1 = (1 << OCIE1A);

	// Timer1 prescaler = system clock / 8
	TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);

	// Timer1 mode = CTC (count up to compare value, then reset)
	TCCR1B |= (1 << WGM12);
}

/** \brief Function to set values for touch sensing
 *
 * The function will fill the default threshold values in the
 * configuration data structure. The user can change the values
 * of these parameters to fit the application.
 */
void qt_set_parameters( void )
{
	//  Threshold values for touch sensing
	qt_config_data.qt_di              = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
	qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
}

/** \brief Function to setup touch button.
 *
 * This function will configure touch sensing for the QTB0 touch button.
 * LED1 is used as touch indication. Touch detected: LED1 on, touch not
 * detected: LED1 off.
 */
void touch_init(void)
{
	// Configure touch button (QTB0) pin mapping
	SNS_array[0][0] = 0x40;
	SNS_array[0][1] = 0x00;
	SNS_array[1][0] = 0x00;
	SNS_array[1][1] = 0x00;
	SNSK_array[0][0] = 0x80;
	SNSK_array[0][1] = 0x00;
	SNSK_array[1][0] = 0x00;
	SNSK_array[1][1] = 0x00;

	timer1_init();

	// PORTB3 (LED1) output default high (LED1 off)
	DDRB |= (1 << DDB3);
	PORTB |= (1 << PORTB3);

	// Configure the Sensor
	qt_enable_key(CHANNEL_0, NO_AKS_GROUP, 30, HYST_6_25);

	// Initialize touch sensing
	qt_init_sensing();

	/* Set the parameters like recalibration threshold,
	 * Max_On_Duration etc in this function by the user
	 */
	qt_set_parameters( );

	/* This function is called after the library has made
	 * capacitive measurements, but before it has processed them.
	 * The user can use this hook to apply filter functions to the
	 * measured signal values.(Possibly to fix sensor layout faults)
	 */
	qt_filter_callback = 0;
}

 //! @}
