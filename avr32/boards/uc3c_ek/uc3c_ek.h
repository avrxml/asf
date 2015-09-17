/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3C UC3C_EK board header file.
 *
 * This file contains definitions and services related to the features of the
 * UC3C_EK board.
 *
 * To use this board, define BOARD=UC3C_EK.
 *
 * Copyright (C) 2009-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _UC3C_EK_H_
#define _UC3C_EK_H_

#include "compiler.h"

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


#ifdef AVR32_SCIF_101_H_INCLUDED
#define AVR32_SCIF_OSCCTRL0_STARTUP_2048_RCOSC  0x00000003
#define AVR32_SCIF_OSCCTRL0_STARTUP_16384_RCOSC 0x00000006
#define AVR32_SCIF_OSCCTRL32_STARTUP_8192_RCOSC 0x00000002
#endif


/*! \name Oscillator Definitions
 */
//! @{

#define FOSC32          AVR32_SCIF_OSC32_FREQUENCY              //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_SCIF_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

// Osc0 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc0 crystal is mounted on your STK600
#define FOSC0           16000000                                //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_SCIF_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// Osc1 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc1 crystal is mounted on your board.
// #define FOSC1           12000000                              //!< Osc1 frequency: Hz.
// #define OSC1_STARTUP    AVR32_SCIF_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}

#define BOARD_OSC0_HZ           16000000
#define BOARD_OSC0_STARTUP_US   2000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

/*! \name SDRAM Definitions
 */
//! @{

//! Part header file of used SDRAM(s).
#define SDRAM_PART_HDR  "mt48lc16m16a2tg7e/mt48lc16m16a2tg7e.h"

//! Data bus width to use the SDRAM(s) with (16 or 32 bits; always 16 bits on
//! UC3).
#define SDRAM_DBW       16
//! @}


/*! \name USB Definitions
 */
//! @{
//! Multiplexed pin used for USB_ID: AVR32_USBC_ID_x.
//! To be selected according to the AVR32_USBC_ID_x_PIN and
//! AVR32_USBC_USB_ID_FUNCTION definitions from <avr32/uc3cxxxx.h>.
#define USB_ID                             AVR32_USBC_ID_

//! Multiplexed pin used for USB_VBOF: AVR32_USBC_VBOF_x.
//! To be selected according to the AVR32_USBC_VBOF_x_PIN and
//! AVR32_USBC_USB_VBOF_FUNCTION definitions from <avr32/uc3cxxxx.h>.
#define USB_VBOF                           AVR32_USBC_VBOF

//! Active level of the USB_VBOF output pin.
# define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
# define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PB7

//! @}


//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs
 */
//! @{
#  define LED0_GPIO   AVR32_PIN_PA08
#  define LED1_GPIO   AVR32_PIN_PD23
#  define LED2_GPIO   AVR32_PIN_PC13
#  define LED3_GPIO   AVR32_PIN_PD22
//! @}

/*! \name PWM Channels of LEDs
 */
//! @{
#define LED0_PWM      (-1)
#define LED1_PWM      (-1)
#define LED2_PWM      (-1)
#define LED3_PWM      (-1)
//! @}

/*! \name PWM Functions of LEDs
 */
//! @{
/* TODO: Implement PWM functionality */
#define LED0_PWM_FUNCTION   (-1)//AVR32_PWM_0_FUNCTION
#define LED1_PWM_FUNCTION   (-1)//AVR32_PWM_1_FUNCTION
#define LED2_PWM_FUNCTION   (-1)
#define LED3_PWM_FUNCTION   (-1)
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_MONO0_GREEN   LED0
#define LED_MONO1_GREEN   LED1
#define LED_MONO2_GREEN   LED2
#define LED_MONO3_GREEN   LED3
//! @}

/*! \name GPIO Connections of Push Buttons
 */
//! @{
#define GPIO_PUSH_BUTTON_0            AVR32_PIN_PA14
#define GPIO_PUSH_BUTTON_0_PRESSED    0
#define GPIO_PUSH_BUTTON_1            AVR32_PIN_PA29
#define GPIO_PUSH_BUTTON_1_PRESSED    0
//! @}

/*! \name GPIO Connections of J33
 */
//! @{
#  define UH   AVR32_PIN_PB09
#  define UL   AVR32_PIN_PB08
#  define VH   AVR32_PIN_PB11
#  define VL   AVR32_PIN_PB10
#  define WH   AVR32_PIN_PB13
#  define WL   AVR32_PIN_PB12
#  define XH   AVR32_PIN_PB15
#  define XL   AVR32_PIN_PB14
#  define ShV  AVR32_PIN_PA21
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI                 (&AVR32_SPI1)
#define AT45DBX_SPI_NPCS            1
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI1_SCK_0_1_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI1_SCK_0_1_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI1_MISO_0_1_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI1_MISO_0_1_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI1_MOSI_0_1_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI1_MOSI_0_1_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI1_NPCS_1_2_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI1_NPCS_1_2_FUNCTION
//! @}

/*! \name GPIO and SPI Connections of the SD/MMC Connector
 */
//! @{
#define SD_MMC_SPI_MEM_CNT          1
#define SD_MMC_0_CD_GPIO            AVR32_PIN_PA28
#define SD_MMC_0_CD_DETECT_VALUE    1
#define SD_MMC_0_WP_GPIO            AVR32_PIN_PD30
#define SD_MMC_0_WP_DETECT_VALUE    0
#define SD_MMC_SPI_0_CS             3

// Keep it for SD MMC stack ASF V1.7
#define SD_MMC_CARD_DETECT_PIN      SD_MMC_0_CD_GPIO
#define SD_MMC_WRITE_PROTECT_PIN    SD_MMC_0_WP_GPIO
#define SD_MMC_SPI_NPCS             SD_MMC_SPI_0_CS

#define SD_MMC_SPI                  (&AVR32_SPI1)
#define SD_MMC_SPI_SCK_PIN          AVR32_SPI1_SCK_0_1_PIN
#define SD_MMC_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_1_FUNCTION
#define SD_MMC_SPI_MISO_PIN         AVR32_SPI1_MISO_0_1_PIN
#define SD_MMC_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_1_FUNCTION
#define SD_MMC_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_1_PIN
#define SD_MMC_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_1_FUNCTION
#define SD_MMC_SPI_NPCS_PIN         AVR32_SPI1_NPCS_3_2_PIN
#define SD_MMC_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_3_2_FUNCTION
//! @}


//! @}

/*! \name ET024006DHU TFT display
 */
//! @{

#define ET024006DHU_TE_PIN              AVR32_PIN_PD19
#define ET024006DHU_RESET_PIN           AVR32_PIN_PD16
#define ET024006DHU_BL_PIN              AVR32_TC0_B0_0_2_PIN
#define ET024006DHU_BL_FUNCTION         AVR32_TC0_B0_0_2_FUNCTION
#define ET024006DHU_DNC_PIN             AVR32_EBI_ADDR_22_PIN
#define ET024006DHU_DNC_FUNCTION        AVR32_EBI_ADDR_22_FUNCTION
#define ET024006DHU_EBI_NCS_PIN         AVR32_EBI_NCS_PIN
#define ET024006DHU_EBI_NCS_FUNCTION    AVR32_EBI_NCS_FUNCTION

#define ET024006_IFACE_MODE             ET024006_IFACE_MODE_EBI
#define ET024006_CMD_ADDR               ((volatile unsigned short int *) AVR32_EBI_CS0_ADDRESS)
// The select signal connected to address line 22,
// this implies an address offset of 0x400000.
#define ET024006_PARAM_ADDR             ((volatile unsigned short int *) (AVR32_EBI_CS0_ADDRESS + 0x00400000))


//! @}
/*! \name Optional SPI connection to the TFT
 */
//! @{

#define ET024006DHU_SPI                  (&AVR32_SPI1)
#define ET024006DHU_SPI_NPCS             1
#define ET024006DHU_SPI_SCK_PIN          AVR32_SPI1_SCK_0_1_PIN
#define ET024006DHU_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_1_FUNCTION
#define ET024006DHU_SPI_MISO_PIN         AVR32_SPI1_MISO_0_1_PIN
#define ET024006DHU_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_1_FUNCTION
#define ET024006DHU_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_1_PIN
#define ET024006DHU_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_1_FUNCTION
#define ET024006DHU_SPI_NPCS_PIN         AVR32_SPI1_NPCS_2_2_PIN
#define ET024006DHU_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_2_2_FUNCTION

//! @}


/*! \name Audio amplifier connection to the DAC
 */
//! @{

#define TPA6130_TWI                  (&AVR32_TWIM0_ADDRESS)
#define TPA6130_TWI_SCL_PIN          AVR32_TWIMS0_TWCK_0_1_PIN
#define TPA6130_TWI_SCL_FUNCTION     AVR32_TWIMS0_TWCK_0_1_FUNCTION
#define TPA6130_TWI_SDA_PIN          AVR32_TWIMS0_TWD_0_0_PIN
#define TPA6130_TWI_SDA_FUNCTION     AVR32_TWIMS0_TWD_0_0_FUNCTION

/*! \name USART connection to the UC3B board controller
 */
//! @{
#define USART                        (&AVR32_USART2)
#define USART_RXD_PIN                AVR32_USART2_RXD_0_1_PIN
#define USART_RXD_FUNCTION           AVR32_USART2_RXD_0_1_FUNCTION
#define USART_TXD_PIN                AVR32_USART2_TXD_0_1_PIN
#define USART_TXD_FUNCTION           AVR32_USART2_TXD_0_1_FUNCTION
#define USART_IRQ                    AVR32_USART2_IRQ
#define USART_IRQ_GROUP              AVR32_USART2_IRQ_GROUP
#define USART_SYSCLK                 SYSCLK_USART2
//! @}

#define ADC_VEXT_PIN         AVR32_PKGANA_ADCIN5_0_0_PIN
#define ADC_VEXT_FUNCTION    AVR32_PKGANA_ADCIN5_0_0_FUNCTION

/*! \name LCD Connections of the ET024006DHU display
 */
//! @{
#define ET024006DHU_SMC_USE_NCS           0
#define ET024006DHU_SMC_COMPONENT_CS      "smc_et024006dhu.h"

#define ET024006DHU_EBI_DATA_0    AVR32_EBI_DATA_0
#define ET024006DHU_EBI_DATA_1    AVR32_EBI_DATA_1
#define ET024006DHU_EBI_DATA_2    AVR32_EBI_DATA_2
#define ET024006DHU_EBI_DATA_3    AVR32_EBI_DATA_3
#define ET024006DHU_EBI_DATA_4    AVR32_EBI_DATA_4
#define ET024006DHU_EBI_DATA_5    AVR32_EBI_DATA_5
#define ET024006DHU_EBI_DATA_6    AVR32_EBI_DATA_6
#define ET024006DHU_EBI_DATA_7    AVR32_EBI_DATA_7
#define ET024006DHU_EBI_DATA_8    AVR32_EBI_DATA_8
#define ET024006DHU_EBI_DATA_9    AVR32_EBI_DATA_9
#define ET024006DHU_EBI_DATA_10   AVR32_EBI_DATA_10
#define ET024006DHU_EBI_DATA_11   AVR32_EBI_DATA_11
#define ET024006DHU_EBI_DATA_12   AVR32_EBI_DATA_12
#define ET024006DHU_EBI_DATA_13   AVR32_EBI_DATA_13
#define ET024006DHU_EBI_DATA_14   AVR32_EBI_DATA_14
#define ET024006DHU_EBI_DATA_15   AVR32_EBI_DATA_15

#define ET024006DHU_EBI_ADDR_21   AVR32_EBI_ADDR_22

#define ET024006DHU_EBI_NWE       AVR32_EBI_NWE0
#define ET024006DHU_EBI_NRD       AVR32_EBI_NRD
#define ET024006DHU_EBI_NCS       AVR32_EBI_NCS_0
//! @}

/*! \name Resistive touch Connections of the ET024006DHU display
 */
//! @{
#define RTOUCH_ADC_IRQ            AVR32_ADCIFA_SEQUENCER0_IRQ

//! IO pin used for XL line.
#define RTOUCH_XH_PIN            AVR32_ADCIN1_PIN //PA5
#define RTOUCH_XH_PIN_FUNCTION   AVR32_ADCIN1_FUNCTION
#define RTOUCH_ADC_XH_CHANNEL    AVR32_ADCIFA_INP_ADCIN1

//! IO pin used for XH line.
#define RTOUCH_XL_PIN            AVR32_ADCIN15_PIN //PA13
#define RTOUCH_XL_PIN_FUNCTION   AVR32_ADCIN15_FUNCTION
#define RTOUCH_ADC_XL_CHANNEL    AVR32_ADCIFA_INN_ADCIN15

//! IO pin used for YL line.
#define RTOUCH_YL_PIN            AVR32_ADCIN13_PIN //PA24
#define RTOUCH_YL_PIN_FUNCTION   AVR32_ADCIN13_FUNCTION
#define RTOUCH_ADC_YL_CHANNEL    AVR32_ADCIFA_INN_ADCIN13

//! IO pin used for YH line.
#define RTOUCH_YH_PIN            AVR32_ADCIN0_PIN //PA4
#define RTOUCH_YH_PIN_FUNCTION   AVR32_ADCIN0_FUNCTION
#define RTOUCH_ADC_YH_CHANNEL    AVR32_ADCIFA_INP_ADCIN0

//! AVR32 ADC module used for touch panel sampling.
#define RTOUCH_ADC               AVR32_ADCIFA
//! @}

#endif  // _UC3C_EK_H_
