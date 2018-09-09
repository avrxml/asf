/**
* \file
*
* \brief The EPD configurations
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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

#ifndef CONF_EPD_H_INCLUDED
#define CONF_EPD_H_INCLUDED

/** \brief Supported COG version
 *
 * \note
 * - place the value here to assign which COG driving waveform will be used.
 * - Options are COG_V110_G1, COG_V230_G2 */
#define COG_V110_G1

/** The SPI frequency of this kit (12MHz) */
#define COG_SPI_baudrate	12000000

/** GPIO Defines */
#define EPD_BORDER_PIN      IOPORT_CREATE_PIN (PORTF,1) /**< MEGA256RFR2 EXT1.pin4.  PF1 ADC1 */
#define EPD_DISCHARGE_PIN   IOPORT_CREATE_PIN (PORTE,2) /**< MEGA256RFR2 EXT1.pin5.  PE2 GPIO */
#define EPD_RST_PIN         IOPORT_CREATE_PIN (PORTE,3) /**< MEGA256RFR2 EXT1.pin6.  PE3 GPIO */
#define EPD_PWM_PIN         IOPORT_CREATE_PIN (PORTB,5) /**< MEGA256RFR2 EXT1.pin7.  PB5 OC1A */
#define EPD_PANELON_PIN     IOPORT_CREATE_PIN (PORTB,6) /**< MEGA256RFR2 EXT1.pin8.  PB5 OC1B */
#define EPD_BUSY_PIN        IOPORT_CREATE_PIN (PORTE,5) /**< MEGA256RFR2 EXT1.pin9.  PE5 GPIO */
#define EPD_SPI_CS_PIN      IOPORT_CREATE_PIN (PORTG,0) /**< MEGA256RFR2 EXT1.pin15. PG0 SPI chip select A */
#define EPD_SPI_SS_PIN		IOPORT_CREATE_PIN (PORTG,0) /**< MEGA256RFR2 EXT1.pin15. PG0 SPI chip select A */
#define EPD_SPI_MOSI_PIN    IOPORT_CREATE_PIN (PORTB,2) /**< MEGA256RFR2 EXT1.pin16. PB2 SPI MOSI */
#define EPD_SPI_MISO_PIN    IOPORT_CREATE_PIN (PORTB,3) /**< MEGA256RFR2 EXT1.pin17. PB3 SPI MISO */
#define EPD_SPI_CLK_PIN     IOPORT_CREATE_PIN (PORTB,1) /**< MEGA256RFR2 EXT1.pin18. PB1, SPI SCK */

/** unused pins */
//#define EPD_Temper_PIN      IOPORT_CREATE_PIN (PORTF,0); /**< MEGA256RFR2 EXT1.pin3.  PF0, ADC0 */
//#define Flash_CS_PIN        IOPORT_CREATE_PIN (PORTD,5) /**< MEGA256RFR2 EXT1.pin10. PD5 GPIO */

/** EPD Hardware module Defines */
/** PWM Define */
#define EPD_TC_TIMER_ID             &TCCR1A						/**< Timer/Counter(TC) to Timer mode */
#define EPD_TC_TIMER_IRQn           TIMER1_OVF_vect_num			/**< Timer/Counter(TC) Interrupt Number */
#define EPD_TC_TIMER_CHANNEL		TC_COMPA					/**< Timer/Counter(TC) Compare Trigger Channel */
#define EPD_TC_WAVEFORM_CHANNEL     TC_COMPA					/**< Timer/Counter(TC) Compare Trigger Channel */
#define EPD_TC_WAVEFORM_ID          &TCCR1A						/**< Timer/Counter(TC) to Waveform mode */
#define EPD_TC_WAVEFORM_PIN         EPD_PWM_PIN					/**< Timer/Counter(TC) PWM output pin */
#define EPD_TC_WAVEFORM_PIN_FLAGS   IOPORT_MODE_PULLUP \
									| IOPORT_INIT_HIGH \
									| IOPORT_DIR_OUTPUT         /**< TC output pin configuration */

#define EPD_TC_ClockSignalSel				TC_CLKSEL_DIV1_gc	/**< clkIO/1 */
#define EPD_TC_WAVEFORM_PWM_FREQUENCY		100000				/**< PWM frequency = 100K Hz */
#define EPD_TC_WAVEFORM_PWM_DUTY_CYCLE		50					/**< PWM duty cycle = 50% */

/** SPI Defines */
#define EPD_SPI_ID					&SPCR	                    /**< SPI */
#define EPD_SPI_baudrate            COG_SPI_baudrate			/**< the baud rate of SPI */
#define EPD_SPI_CLK_MUX             IOPORT_INIT_HIGH \
									| IOPORT_DIR_OUTPUT			/**< SPI Pin config: SCK on PB1 */
#define EPD_SPI_MOSI_MUX            IOPORT_INIT_HIGH \
									| IOPORT_DIR_OUTPUT			/**< SPI Pin config: MOSI on PB2 */
#define EPD_SPI_MISO_MUX            IOPORT_DIR_INPUT			/**< SPI Pin config: MISO on PB3 */
#define EPD_SPI_SS_MUX				IOPORT_INIT_HIGH \
									| IOPORT_DIR_OUTPUT			/**< SPI Pin config: SS on PG0 */
/** Temperature ADC Defines */
#define EPD_Temperature_Sensor_ADC  0 				 			/**< MUX selection on Positive ADC input channel 4 */
#define EPD_TempeScaled            2.5f  						/**< the scale of temperature circuit */
#define EPD_ADCRefVcc              1.65f 						/**< ADC ref voltage = VCC/2 = 3.3/2 */
#define EPD_ADCres                 (float)pow(2,12) 			/**< 2 ^12 */
#define EPD_DegCOffset             5.5f  						/**< the offset of SAM4L MCU */
#define EPD_ADCSampleCount         16    						/**< ADC sampling counter */

 
#endif /* CONF_EPD_H_INCLUDED */