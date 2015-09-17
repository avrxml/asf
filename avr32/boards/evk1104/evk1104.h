/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3A3 EVK1104 board header file.
 *
 * This file contains definitions and services related to the features of the
 * EVK1104 board rev. A.
 *
 * To use this board, define BOARD=EVK1104.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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


#ifndef _EVK1104_H_
#define _EVK1104_H_

#include "compiler.h"


#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__

/*! \name Oscillator Definitions
 */
//! @{

#define FOSC32          32768                                 //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

#define FOSC0           12000000                              //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

#define FOSC1           11289600                              //!< Osc1 frequency: Hz.
#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}

/* These are documented in services/basic/clock/uc3a3/osc.h */
#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC1_HZ           11289600
#define BOARD_OSC1_STARTUP_US   17000
#define BOARD_OSC1_IS_XTAL      true
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

//! Multiplexed pin used for USB_ID: AVR32_USBB_USB_ID_x_x.
//! To be selected according to the AVR32_USBB_USB_ID_x_x_PIN and
//! AVR32_USBB_USB_ID_x_x_FUNCTION definitions from <avr32/uc3bxxxx.h>.
#define USB_ID                      AVR32_USBB_USB_ID_0_2

//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3bxxxx.h>.
#define USB_VBOF                    AVR32_USBB_USB_VBOF_0_2

//! Active level of the USB_VBOF output pin.
#define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
#define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PA20

//! @}

//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PX16
#define LED1_GPIO   AVR32_PIN_PX50
#define LED2_GPIO   AVR32_PIN_PX54
#define LED3_GPIO   AVR32_PIN_PX51
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_MONO0_GREEN   LED0
#define LED_MONO1_GREEN   LED1
#define LED_MONO2_GREEN   LED2
#define LED_MONO3_GREEN   LED3
//! @}

/*! \name GPIO Connections of the SW2 Push Button
 */
//! @{
#define GPIO_PUSH_BUTTON_SW2          AVR32_PIN_PB10
#define GPIO_PUSH_BUTTON_SW2_PRESSED  0
//! @}

/*! \name QTouch 60168 buttons definition
 *! Usage: qt60168_is_key_pressed(QT60168_TOUCH_SENSOR_WHEEL_x)
 */
//! @{
#define QT60168_TOUCH_NUMBER_OF_SENSORS 16
#define QT60168_TOUCH_SENSOR_BUTTON_0   6
#define QT60168_TOUCH_SENSOR_BUTTON_1   7
#define QT60168_TOUCH_SENSOR_BUTTON_2   14
#define QT60168_TOUCH_SENSOR_BUTTON_3   15
#define QT60168_TOUCH_SENSOR_WHEEL_0    0
#define QT60168_TOUCH_SENSOR_WHEEL_1    1
#define QT60168_TOUCH_SENSOR_WHEEL_2    2
#define QT60168_TOUCH_SENSOR_WHEEL_3    3
#define QT60168_TOUCH_SENSOR_WHEEL_4    4
#define QT60168_TOUCH_SENSOR_WHEEL_5    5
#define QT60168_TOUCH_SENSOR_WHEEL_6    8
#define QT60168_TOUCH_SENSOR_WHEEL_7    9
#define QT60168_TOUCH_SENSOR_WHEEL_8    10
#define QT60168_TOUCH_SENSOR_WHEEL_9    11
#define QT60168_TOUCH_SENSOR_WHEEL_10   12
#define QT60168_TOUCH_SENSOR_WHEEL_11   13
#define QT60168_TOUCH_SENSOR_WHEEL_UP    QT60168_TOUCH_SENSOR_WHEEL_0
#define QT60168_TOUCH_SENSOR_WHEEL_DOWN  QT60168_TOUCH_SENSOR_WHEEL_6
#define QT60168_TOUCH_SENSOR_WHEEL_RIGHT QT60168_TOUCH_SENSOR_WHEEL_3
#define QT60168_TOUCH_SENSOR_WHEEL_LEFT  QT60168_TOUCH_SENSOR_WHEEL_9
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI                 (&AVR32_SPI0)
#define AT45DBX_SPI_NPCS            1
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI0_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI0_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI0_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI0_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI0_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI0_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI0_NPCS_1_0_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI0_NPCS_1_0_FUNCTION
//! @}


/*! \name SPI Connections of the QT60168 touch component.
 */
//! @{
#define QT60168_SPI                 (&AVR32_SPI0)
#define QT60168_SPI_NCPS            0
#define QT60168_SPI_SCK_PIN         AVR32_SPI0_SCK_0_0_PIN
#define QT60168_SPI_SCK_FUNCTION    AVR32_SPI0_SCK_0_0_FUNCTION
#define QT60168_SPI_MISO_PIN        AVR32_SPI0_MISO_0_0_PIN
#define QT60168_SPI_MISO_FUNCTION   AVR32_SPI0_MISO_0_0_FUNCTION
#define QT60168_SPI_MOSI_PIN        AVR32_SPI0_MOSI_0_0_PIN
#define QT60168_SPI_MOSI_FUNCTION   AVR32_SPI0_MOSI_0_0_FUNCTION
#define QT60168_SPI_NPCS0_PIN       AVR32_SPI0_NPCS_0_2_PIN
#define QT60168_SPI_NPCS0_FUNCTION  AVR32_SPI0_NPCS_0_2_FUNCTION
#define QT60168_DRDY_PIN            AVR32_PIN_PX11
//! @}

/*! \name MCI Connections of the SD/MMC Slots
 */
//! @{
#define SD_SLOT_MCI                        (&AVR32_MCI)
/*! Number of slot connected on HSMCI interface */
#define SD_MMC_MCI_MEM_CNT                 2

//! 8-bits connector pin
#define SD_MMC_MCI_SLOT_0_SIZE             8
#define SD_MMC_MCI_SLOT_0_DATA0_PIN        SD_SLOT_8BITS_DATA0_PIN
#define SD_MMC_MCI_SLOT_0_DATA3_PIN        SD_SLOT_8BITS_DATA3_PIN
#define SD_SLOT_8BITS                      0
#define SD_SLOT_8BITS_CLK_PIN              AVR32_MCI_CLK_0_PIN
#define SD_SLOT_8BITS_CLK_FUNCTION         AVR32_MCI_CLK_0_FUNCTION
#define SD_SLOT_8BITS_CMD_PIN              AVR32_MCI_CMD_0_PIN
#define SD_SLOT_8BITS_CMD_FUNCTION         AVR32_MCI_CMD_0_FUNCTION
#define SD_SLOT_8BITS_DATA0_PIN            AVR32_MCI_DATA_0_PIN
#define SD_SLOT_8BITS_DATA0_FUNCTION       AVR32_MCI_DATA_0_FUNCTION
#define SD_SLOT_8BITS_DATA1_PIN            AVR32_MCI_DATA_1_PIN
#define SD_SLOT_8BITS_DATA1_FUNCTION       AVR32_MCI_DATA_1_FUNCTION
#define SD_SLOT_8BITS_DATA2_PIN            AVR32_MCI_DATA_2_PIN
#define SD_SLOT_8BITS_DATA2_FUNCTION       AVR32_MCI_DATA_2_FUNCTION
#define SD_SLOT_8BITS_DATA3_PIN            AVR32_MCI_DATA_3_PIN
#define SD_SLOT_8BITS_DATA3_FUNCTION       AVR32_MCI_DATA_3_FUNCTION
#define SD_SLOT_8BITS_DATA4_PIN            AVR32_MCI_DATA_4_PIN
#define SD_SLOT_8BITS_DATA4_FUNCTION       AVR32_MCI_DATA_4_FUNCTION
#define SD_SLOT_8BITS_DATA5_PIN            AVR32_MCI_DATA_5_PIN
#define SD_SLOT_8BITS_DATA5_FUNCTION       AVR32_MCI_DATA_5_FUNCTION
#define SD_SLOT_8BITS_DATA6_PIN            AVR32_MCI_DATA_6_PIN
#define SD_SLOT_8BITS_DATA6_FUNCTION       AVR32_MCI_DATA_6_FUNCTION
#define SD_SLOT_8BITS_DATA7_PIN            AVR32_MCI_DATA_7_PIN
#define SD_SLOT_8BITS_DATA7_FUNCTION       AVR32_MCI_DATA_7_FUNCTION
#define SD_MMC_0_CD_GPIO                   AVR32_PIN_PB11
#define SD_MMC_0_CD_DETECT_VALUE           0
#define SD_SLOT_8BITS_CARD_DETECT          SD_MMC_0_CD_GPIO
#define SD_SLOT_8BITS_CARD_DETECT_VALUE    SD_MMC_0_CD_DETECT_VALUE
#define SD_MMC_0_WP_GPIO                   AVR32_PIN_PX57
#define SD_MMC_0_WP_DETECT_VALUE           1
#define SD_SLOT_8BITS_WRITE_PROTECT        SD_MMC_0_WP_GPIO
#define SD_SLOT_8BITS_WRITE_PROTECT_VALUE  SD_MMC_0_WP_DETECT_VALUE

//! 4-bits connector pin
#define SD_MMC_MCI_SLOT_1_SIZE             4
#define SD_MMC_MCI_SLOT_1_DATA0_PIN        SD_SLOT_4BITS_DATA0_PIN
#define SD_MMC_MCI_SLOT_1_DATA3_PIN        SD_SLOT_4BITS_DATA3_PIN
#define SD_SLOT_4BITS                      1
#define SD_SLOT_4BITS_CLK_PIN              AVR32_MCI_CLK_0_PIN
#define SD_SLOT_4BITS_CLK_FUNCTION         AVR32_MCI_CLK_0_FUNCTION
#define SD_SLOT_4BITS_CMD_PIN              AVR32_MCI_CMD_1_0_PIN
#define SD_SLOT_4BITS_CMD_FUNCTION         AVR32_MCI_CMD_1_0_FUNCTION
#define SD_SLOT_4BITS_DATA0_PIN            AVR32_MCI_DATA_8_0_PIN
#define SD_SLOT_4BITS_DATA0_FUNCTION       AVR32_MCI_DATA_8_0_FUNCTION
#define SD_SLOT_4BITS_DATA1_PIN            AVR32_MCI_DATA_9_0_PIN
#define SD_SLOT_4BITS_DATA1_FUNCTION       AVR32_MCI_DATA_9_0_FUNCTION
#define SD_SLOT_4BITS_DATA2_PIN            AVR32_MCI_DATA_10_0_PIN
#define SD_SLOT_4BITS_DATA2_FUNCTION       AVR32_MCI_DATA_10_0_FUNCTION
#define SD_SLOT_4BITS_DATA3_PIN            AVR32_MCI_DATA_11_0_PIN
#define SD_SLOT_4BITS_DATA3_FUNCTION       AVR32_MCI_DATA_11_0_FUNCTION
#define SD_MMC_1_CD_GPIO                   AVR32_PIN_PB08
#define SD_MMC_1_CD_DETECT_VALUE           0
#define SD_SLOT_4BITS_CARD_DETECT          SD_MMC_1_CD_GPIO
#define SD_SLOT_4BITS_CARD_DETECT_VALUE    SD_MMC_1_CD_DETECT_VALUE
#define SD_MMC_1_WP_GPIO                   AVR32_PIN_PB06
#define SD_MMC_1_WP_DETECT_VALUE           1
#define SD_SLOT_4BITS_WRITE_PROTECT        SD_MMC_1_WP_GPIO
#define SD_SLOT_4BITS_WRITE_PROTECT_VALUE  SD_MMC_1_WP_DETECT_VALUE
//! @}

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

#define ET024006DHU_EBI_ADDR_21   AVR32_EBI_ADDR_21

#define ET024006DHU_EBI_NWE       AVR32_EBI_NWE0_0
#define ET024006DHU_EBI_NRD       AVR32_EBI_NRD_0
#define ET024006DHU_EBI_NCS       AVR32_EBI_NCS_0

#define ET024006DHU_TE_PIN        AVR32_PIN_PA00
#define ET024006DHU_RESET_PIN     AVR32_PIN_PX15

#define ET024006DHU_BL_PIN           AVR32_PIN_PA09
#define ET024006DHU_BL_FUNCTION      AVR32_TC1_A1_0_FUNCTION
#define ET024006DHU_BL_TC_CHANNEL_ID 1
#define ET024006DHU_BL_TC_INSTANCE   &AVR32_TC1


#define ET024006_IFACE_MODE       ET024006_IFACE_MODE_EBI
#define ET024006_CMD_ADDR         ((volatile unsigned short int *) AVR32_EBI_CS0_ADDRESS)
// The select signal connected to address line 21,
// this implies an address offset of 0x200000.
#define ET024006_PARAM_ADDR       ((volatile unsigned short int *) (AVR32_EBI_CS0_ADDRESS + 0x00200000))

//! @}

/*! \name Optional SPI connection to the ET024006DHU display
 */
//! @{

#define ET024006DHU_SPI                  (&AVR32_SPI0)
#define ET024006DHU_SPI_NPCS             2
#define ET024006DHU_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define ET024006DHU_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define ET024006DHU_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define ET024006DHU_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define ET024006DHU_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define ET024006DHU_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define ET024006DHU_SPI_NPCS_PIN         AVR32_SPI0_NPCS_2_0_PIN
#define ET024006DHU_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_2_0_FUNCTION

//! @}


/*! \name Nand Flash Connections
 */
//! @{
#define NF_SMC_USE_NCS           3
#define NF_SMC_COMPONENT_CS      "smc_nf.h"

#define NF_EBI_DATA_0    AVR32_EBI_DATA_0
#define NF_EBI_DATA_1    AVR32_EBI_DATA_1
#define NF_EBI_DATA_2    AVR32_EBI_DATA_2
#define NF_EBI_DATA_3    AVR32_EBI_DATA_3
#define NF_EBI_DATA_4    AVR32_EBI_DATA_4
#define NF_EBI_DATA_5    AVR32_EBI_DATA_5
#define NF_EBI_DATA_6    AVR32_EBI_DATA_6
#define NF_EBI_DATA_7    AVR32_EBI_DATA_7

#define NF_EBI_ADDR_21   AVR32_EBI_ADDR_21   // CLE
#define NF_EBI_ADDR_22   AVR32_EBI_ADDR_22   // ALE
#define NF_EBI_NANDOE    AVR32_EBI_NANDOE_0  // RD
#define NF_EBI_NANDWE    AVR32_EBI_NANDWE_0  // RD

// These are not under EBI control.
#ifndef NF_ADDON
#  define NF_DEV_0_CS_PIN           AVR32_PIN_PX53
#  define NF_DEV_1_CS_PIN           AVR32_PIN_PX52
#else
   // the NandFlash Add-On board is used
#  define NF_DEV_0_CS_PIN           AVR32_PIN_PX40
#  define NF_DEV_1_CS_PIN           AVR32_PIN_PX22
#  define NF_DEV_2_CS_PIN           AVR32_PIN_PX19
#  define NF_DEV_3_CS_PIN           AVR32_PIN_PX15
#endif
#define NF_RDYBSY_PIN             AVR32_PIN_PA01
#define NF_WP_PIN                 AVR32_PIN_PA07

/** Address for transferring command bytes to the nandflash. */
#define BOARD_NF_COMMAND_ADDR   (AVR32_EBI_CS3_ADDRESS + 0x00200000)
/** Address for transferring address bytes to the nandflash. */
#define BOARD_NF_ADDRESS_ADDR   (AVR32_EBI_CS3_ADDRESS + 0x00400000)
/** Address for transferring data bytes to the nandflash. */
#define BOARD_NF_DATA_ADDR      AVR32_EBI_CS3_ADDRESS

/** Nandflash chip enable pin definition. */
#define PIN_NF_CE_IDX    NF_DEV_0_CS_PIN

/** Nandflash ready/busy pin definition. */
#define PIN_NF_RB_IDX    NF_RDYBSY_PIN

//! @}


/*! \name Audio amplifier connection to the DAC
 */
//! @{
#define TPA6130_ABDAC             (&AVR32_ABDAC)

#define TPA6130_DATA0_PIN         AVR32_ABDAC_DATA_0_1_PIN
#define TPA6130_DATA0_FUNCTION    AVR32_ABDAC_DATA_0_1_FUNCTION
#define TPA6130_DATAN0_PIN        AVR32_ABDAC_DATAN_0_1_PIN
#define TPA6130_DATAN0_FUNCTION   AVR32_ABDAC_DATAN_0_1_FUNCTION
#define TPA6130_DATA1_PIN         AVR32_ABDAC_DATA_1_1_PIN
#define TPA6130_DATA1_FUNCTION    AVR32_ABDAC_DATA_1_1_FUNCTION
#define TPA6130_DATAN1_PIN        AVR32_ABDAC_DATAN_1_1_PIN
#define TPA6130_DATAN1_FUNCTION   AVR32_ABDAC_DATAN_1_1_FUNCTION

#define TPA6130_ABDAC_PDCA_PID       AVR32_PDCA_PID_ABDAC_TX
#define TPA6130_ABDAC_PDCA_CHANNEL   0
#define TPA6130_ABDAC_PDCA_IRQ       AVR32_PDCA_IRQ_0
#define TPA6130_ABDAC_PDCA_IRQ_GROUP 3
#define TPA6130_ABDAC_PDCA_INT_LEVEL AVR32_INTC_INT1

#define TPA6130_TWI                  (&AVR32_TWIM0)
#define TPA6130_TWI_IRQ              AVR32_TWIM0_IRQ
#define TPA6130_TWI_SCL_PIN          AVR32_TWIMS0_TWCK_0_0_PIN
#define TPA6130_TWI_SCL_FUNCTION     AVR32_TWIMS0_TWCK_0_0_FUNCTION
#define TPA6130_TWI_SDA_PIN          AVR32_TWIMS0_TWD_0_0_PIN
#define TPA6130_TWI_SDA_FUNCTION     AVR32_TWIMS0_TWD_0_0_FUNCTION
//! }@

/*! \name USART connection to the UC3B board controller
 */
//! @{
#define USART                               (&AVR32_USART1)
#define USART_RXD_PIN                       AVR32_USART1_RXD_0_0_PIN
#define USART_RXD_FUNCTION                  AVR32_USART1_RXD_0_0_FUNCTION
#define USART_TXD_PIN                       AVR32_USART1_TXD_0_0_PIN
#define USART_TXD_FUNCTION                  AVR32_USART1_TXD_0_0_FUNCTION
#define USART_IRQ                           AVR32_USART1_IRQ
#define USART_IRQ_GROUP                     AVR32_USART1_IRQ_GROUP
#define USART_SYSCLK                        SYSCLK_USART1
//! @}


/*! \name ADC Connection of the Light Sensor
 */
//! @{
#define ADC_LIGHT_CHANNEL           0
#define ADC_LIGHT_PIN               AVR32_ADC_AD_0_PIN
#define ADC_LIGHT_FUNCTION          AVR32_ADC_AD_0_FUNCTION
//! @}

#endif  // _EVK1104_H_
