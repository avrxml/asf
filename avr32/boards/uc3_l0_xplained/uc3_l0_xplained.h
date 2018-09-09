/*****************************************************************************
 *
 * \file
 *
 * \brief UC3-L0-Xplained board header file.
 *
 * This file contains definitions and services related to the features of the
 * UC3-L0-Xplained board.
 *
 * To use this board, define BOARD=UC3_L0_XPLAINED
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef UC3_L0_XPLAINED_H_
#define UC3_L0_XPLAINED_H_


#include "compiler.h"


#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup at32ucl0_xpld_config UC3-L0 Xplained Board Configuration
 * @{
 */

//! \name Oscillator Definitions
// @{

/*! \brief System oscillator frequencies (Hz.) and startup times (periods).
 *
 * RCOsc has no custom calibration by default. Set the following definition
 * to the appropriate value if a custom RCOsc calibration has been applied
 * to your part.
 */

//!< RCOsc frequency: Hz.

#define FRCOSC                  (AVR32_SCIF_RCOSC_FREQUENCY)

//!< Osc32 frequency (Hz.) and startup time (RCOsc periods).

#define FOSC32                  (32768)
#define OSC32_STARTUP           (AVR32_SCIF_OSCCTRL32_STARTUP_131072_RCOSC)

#define BOARD_OSC32_IS_XTAL     true
#define BOARD_OSC32_HZ          FOSC32
#define BOARD_OSC32_STARTUP_US  (920000)
#define BOARD_OSC32_PINSEL      (0)


// The UC3-L0-Xplained does not have an Osc0 crystal.

#define FOSC0                   ((int) "NONE")
//#define OSC0_STARTUP          (AVR32_SCIF_OSCCTRL0_STARTUP_2048_RCOSC)

// @}


//! Number of LEDs.

#define LED_COUNT               5
#define LED_FIRST_PWMA_LED      0
#define LED_PWMA_COUNT          5


//! \name UC3-L0-Xplained LED to GPIO pin mappings.
// @{
#define LED0_GPIO               AVR32_PIN_PA11
#define LED1_GPIO               AVR32_PIN_PB04
#define LED2_GPIO               AVR32_PIN_PA13      // RGB red
#define LED3_GPIO               AVR32_PIN_PA17      // RGB green
#define LED4_GPIO               AVR32_PIN_PA20      // RGB blue
// @}

//! \name UC3-L0-Xplained LED polarity mappings.
// @{
#define LED0_POLARITY           0
#define LED1_POLARITY           0
#define LED2_POLARITY           1
#define LED3_POLARITY           1
#define LED4_POLARITY           1
// @}

//! \name PWM channels and functions of LEDs
// @{
#define LED0_PWMA               (32)
#define LED1_PWMA               (32)
#define LED2_PWMA               AVR32_PWMA_13_PIN
#define LED3_PWMA               AVR32_PWMA_17_PIN
#define LED4_PWMA               AVR32_PWMA_20_PIN

#define LED_RED_PWMA            AVR32_PWMA_13_PIN
#define LED_GREEN_PWMA          AVR32_PWMA_17_PIN
#define LED_BLUE_PWMA           AVR32_PWMA_20_PIN

#define LED_RED_FUNCTION        AVR32_PWMA_13_FUNCTION
#define LED_GREEN_FUNCTION      AVR32_PWMA_17_FUNCTION
#define LED_BLUE_FUNCTION       AVR32_PWMA_20_FUNCTION

#define LED_PWMA_FUNCTION       AVR32_PWMA_0_FUNCTION
#define LED0_PWMA_FUNCTION      (32)
#define LED1_PWMA_FUNCTION      (32)
#define LED2_PWMA_FUNCTION      AVR32_PWMA_13_FUNCTION
#define LED3_PWMA_FUNCTION      AVR32_PWMA_17_FUNCTION
#define LED4_PWMA_FUNCTION      AVR32_PWMA_20_FUNCTION
// @}


//! \name Color Identifiers of LEDs to Use with LED Functions
// @{
#define LED_MONO0_ORANGE        (LED0)
#define LED_MONO1_ORANGE        (LED1)
#define LED_RGB0_RED            (LED2)
#define LED_RGB0_GREEN          (LED3)
#define LED_RGB0_BLUE           (LED4)
// @}

#define GPIO_PUSH_BUTTON_0      (LED0_GPIO)     // Same as LED0 !!
#define GPIO_PUSH_BUTTON_0_PRESSED (0)


//! \brief Bit mask for the PWMA channels connected to the LEDs

#define LED_PWMA_CHANNELS_MASK  ((1<<LED0_PWM)|(1<<LED1_PWM)| \
                                 (1<<LED2_PWM)|(1<<LED3_PWM)|(1<<LED4_PWM))


//! \name GPIO Connections of Push Buttons
// @{
#define GPIO_WAKE_BUTTON            (AVR32_PIN_PA11)
#define GPIO_WAKE_BUTTON_PRESSED    (0)
// @}


//! \name SPI Connections of the AT45DBX Data Flash Memory
// @{
#define AT45DBX_SPI                 (&AVR32_SPI)
#define AT45DBX_SPI_NPCS            0
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI_NPCS_0_0_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI_NPCS_0_0_FUNCTION
// @}


//! \name USART connections to GPIO for Virtual Com Port
// @{
#define USART                    (&AVR32_USART2)
#define USART_RXD_PIN            AVR32_USART2_RXD_0_1_PIN
#define USART_RXD_FUNCTION       AVR32_USART2_RXD_0_1_FUNCTION
#define USART_TXD_PIN            AVR32_USART2_TXD_0_1_PIN
#define USART_TXD_FUNCTION       AVR32_USART2_TXD_0_1_FUNCTION
#define USART_IRQ                AVR32_USART2_IRQ
#define USART_IRQ_GROUP          AVR32_USART2_IRQ_GROUP
#define USART_SYSCLK             SYSCLK_USART2
// @}

//! \name USART connections to GPIO on J4 header
// @{
#define USART0                   (&AVR32_USART1)
#define USART0_RXD_PIN           AVR32_USART1_RXD_0_1_PIN
#define USART0_RXD_FUNCTION      AVR32_USART1_RXD_0_1_FUNCTION
#define USART0_TXD_PIN           AVR32_USART1_TXD_0_1_PIN
#define USART0_TXD_FUNCTION      AVR32_USART1_TXD_0_1_FUNCTION
#define USART0_IRQ               AVR32_USART1_IRQ
#define USART0_IRQ_GROUP         AVR32_USART1_IRQ_GROUP
#define USART0_SYSCLK            SYSCLK_USART1
// @}

/** \name MXT143E Xplained top module
 * @{
 */
#define MXT143E_XPLAINED_PDCA_PID      AVR32_PDCA_PID_SPI0_TX
#define MXT143E_XPLAINED_SPI           &AVR32_SPI
#define MXT143E_XPLAINED_TWI           &AVR32_TWIM0
#define MXT143E_XPLAINED_CS            (AVR32_PIN_PA08)
#define MXT143E_XPLAINED_SCK           (AVR32_SPI_SCK_0_1_PIN)
#define MXT143E_XPLAINED_SCK_FUNCTION  (AVR32_SPI_SCK_0_1_FUNCTION)
#define MXT143E_XPLAINED_MOSI          (AVR32_SPI_MOSI_0_1_PIN)
#define MXT143E_XPLAINED_MOSI_FUNCTION (AVR32_SPI_MOSI_0_1_FUNCTION)
#define MXT143E_XPLAINED_MISO          (AVR32_SPI_MISO_0_1_PIN)
#define MXT143E_XPLAINED_MISO_FUNCTION (AVR32_SPI_MISO_0_1_FUNCTION)
#define MXT143E_XPLAINED_CHG           (AVR32_PIN_PB11)
#define MXT143E_XPLAINED_DC            (AVR32_PIN_PB10)
#define MXT143E_XPLAINED_BACKLIGHT     (AVR32_PIN_PB07)
#define MXT143E_XPLAINED_LCD_RESET     (AVR32_PIN_PB06)
/** @} */

/*! \name TWI Master connections to GPIO
 *
 * \todo
 * The TWIM pins that are mapped here should correspond with the TWIM
 * interfaces selected in the conf_board.h file.  In addition, the
 * default values below map TWIM the pins on the Xplaned J1 block.
 * Modify the mappings or add appropriate conditional compilation as
 * needed.
 *
 * @{
 */
#define TWIMS0_TWD_PIN              AVR32_TWIMS0_TWD_0_1_PIN
#define TWIMS0_TWD_FUNCTION         AVR32_TWIMS0_TWD_0_1_FUNCTION
#define TWIMS0_TWCK_PIN             AVR32_TWIMS0_TWCK_0_3_PIN
#define TWIMS0_TWCK_FUNCTION        AVR32_TWIMS0_TWCK_0_3_FUNCTION
// @}


//! \name ADC Connection for the NTC temperature sensor
// @{
#define ADC_TEMPERATURE_CHANNEL     (4)
#define ADC_TEMPERATURE_PIN         AVR32_ADCIFB_AD_4_PIN
#define ADC_TEMPERATURE_FUNCTION    AVR32_ADCIFB_AD_4_FUNCTION
// @}

//!< Validate board support for the common sensor service.

#define                             COMMON_SENSOR_PLATFORM

/** @} */ // at32ucl0_xpld_config group

#ifdef __cplusplus
}
#endif

#endif  // UC3_L0_XPLAINED_H_
