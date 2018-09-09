/**
 * \file
 *
 * \brief AT32UC3A EVK1105 board header file.
 *
 * This file contains definitions and services related to the features of the
 * EVK1105 board rev. B.
 *
 * To use this board, define BOARD=EVK1105.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _EVK1105_H_
#define _EVK1105_H_

#ifdef EVK1105_REV3 // only valid for EVK1105 rev3, not in production.
#  include "evk1105_rev3.h"
#else

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

#define FOSC1           11289600                              //!< Osc1 frequency: Hz
#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}

/* These are documented in services/basic/clock/uc3a0_a1/osc.h */
#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC1_HZ           11289600
#define BOARD_OSC1_STARTUP_US   17000
#define BOARD_OSC1_IS_XTAL      true

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
//! AVR32_USBB_USB_ID_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#define AVR32_USBB_USB_ID_0_2_PIN          21
#define AVR32_USBB_USB_ID_0_2_FUNCTION     2
#define USB_ID                      AVR32_USBB_USB_ID_0_2

//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#  define USB_VBOF                    AVR32_USBB_USB_VBOF_0_1


//! Active level of the USB_VBOF output pin.
#  define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
#  define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PX15

//! @}


//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs
 */
//! @{
#  define LED0_GPIO   AVR32_PIN_PB27
#  define LED1_GPIO   AVR32_PIN_PB28
#  define LED2_GPIO   AVR32_PIN_PA05
#  define LED3_GPIO   AVR32_PIN_PA06
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_MONO0_GREEN   LED0
#define LED_MONO1_GREEN   LED1
#define LED_MONO2_GREEN   LED2
#define LED_MONO3_GREEN   LED3
//! @}

/*! \name PWM Channels of LEDs
 */
//! @{
#define LED0_PWM      4
#define LED1_PWM      5
#define LED2_PWM      (-1)
#define LED3_PWM      (-1)
//! @}

/*! \name PWM Functions of LEDs
 */
//! @{
#define LED0_PWM_FUNCTION   AVR32_PWM_4_1_FUNCTION
#define LED1_PWM_FUNCTION   AVR32_PWM_5_1_FUNCTION
#define LED2_PWM_FUNCTION   (-1)
#define LED3_PWM_FUNCTION   (-1)
//! @}

//! External interrupt connection of touch sensor.
#define QT1081_EIC_EXTINT_PIN       AVR32_EIC_EXTINT_1_PIN
#define QT1081_EIC_EXTINT_FUNCTION  AVR32_EIC_EXTINT_1_FUNCTION
#define QT1081_EIC_EXTINT_IRQ       AVR32_EIC_IRQ_1
#define QT1081_EIC_EXTINT_INT       AVR32_EIC_INT1
/*! \name Touch sensor low power mode select
 */
#define QT1081_LP_MODE    AVR32_PIN_PB29

/*! \name GPIO Connections of touch buttons
 */
//! @{
#define QT1081_TOUCH_SENSOR_0           AVR32_PIN_PB22
#define QT1081_TOUCH_SENSOR_0_PRESSED   1
#define QT1081_TOUCH_SENSOR_1           AVR32_PIN_PB23
#define QT1081_TOUCH_SENSOR_1_PRESSED   1
#define QT1081_TOUCH_SENSOR_2           AVR32_PIN_PB24
#define QT1081_TOUCH_SENSOR_2_PRESSED   1
#define QT1081_TOUCH_SENSOR_3           AVR32_PIN_PB25
#define QT1081_TOUCH_SENSOR_3_PRESSED   1
#define QT1081_TOUCH_SENSOR_4           AVR32_PIN_PB26
#define QT1081_TOUCH_SENSOR_4_PRESSED   1

#define QT1081_TOUCH_SENSOR_ENTER           QT1081_TOUCH_SENSOR_4
#define QT1081_TOUCH_SENSOR_ENTER_PRESSED   QT1081_TOUCH_SENSOR_4_PRESSED
#define QT1081_TOUCH_SENSOR_LEFT            QT1081_TOUCH_SENSOR_3
#define QT1081_TOUCH_SENSOR_LEFT_PRESSED    QT1081_TOUCH_SENSOR_3_PRESSED
#define QT1081_TOUCH_SENSOR_RIGHT           QT1081_TOUCH_SENSOR_2
#define QT1081_TOUCH_SENSOR_RIGHT_PRESSED   QT1081_TOUCH_SENSOR_2_PRESSED
#define QT1081_TOUCH_SENSOR_UP              QT1081_TOUCH_SENSOR_0
#define QT1081_TOUCH_SENSOR_UP_PRESSED      QT1081_TOUCH_SENSOR_0_PRESSED
#define QT1081_TOUCH_SENSOR_DOWN            QT1081_TOUCH_SENSOR_1
#define QT1081_TOUCH_SENSOR_DOWN_PRESSED    QT1081_TOUCH_SENSOR_1_PRESSED
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI                 (&AVR32_SPI0)
#define AT45DBX_SPI_NPCS            0
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI0_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI0_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI0_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI0_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI0_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI0_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI0_NPCS_0_0_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI0_NPCS_0_0_FUNCTION
//! @}

/*! \name GPIO and SPI Connections of the SD/MMC Connector
 */
//! @{
#define SD_MMC_SPI_MEM_CNT          1
#define SD_MMC_0_CD_GPIO            AVR32_PIN_PA02
#define SD_MMC_0_CD_DETECT_VALUE    1
#define SD_MMC_0_WP_GPIO            AVR32_PIN_PA18
#define SD_MMC_0_WP_DETECT_VALUE    0
#define SD_MMC_SPI_0_CS             1

// Keep it for SD MMC stack ASF V1.7
#define SD_MMC_CARD_DETECT_PIN      SD_MMC_0_CD_GPIO
#define SD_MMC_WRITE_PROTECT_PIN    SD_MMC_0_WP_GPIO
#define SD_MMC_SPI_NPCS             SD_MMC_SPI_0_CS

#define SD_MMC_SPI                  (&AVR32_SPI0)
#define SD_MMC_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define SD_MMC_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define SD_MMC_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define SD_MMC_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define SD_MMC_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define SD_MMC_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define SD_MMC_SPI_NPCS_PIN         AVR32_SPI0_NPCS_1_0_PIN
#define SD_MMC_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_1_0_FUNCTION
//! @}


/*! \name TWI expansion
 */
//! @{
#define EXPANSION_TWI                   (&AVR32_TWI)
#define EXPANSION_RESET                 AVR32_PIN_PX16
#define EXPANSION_TWI_SCL_PIN           AVR32_TWI_SCL_0_0_PIN
#define EXPANSION_TWI_SCL_FUNCTION      AVR32_TWI_SCL_0_0_FUNCTION
#define EXPANSION_TWI_SDA_PIN           AVR32_TWI_SDA_0_0_PIN
#define EXPANSION_TWI_SDA_FUNCTION      AVR32_TWI_SDA_0_0_FUNCTION
//! @}

/*! \name Wireless expansion
 */

#define WEXPANSION_EXTINT_PIN           AVR32_EIC_EXTINT_8_PIN
#define WEXPANSION_EXTINT_FUNCTION      AVR32_EIC_EXTINT_8_FUNCTION
#define WEXPANSION_GPIO1                AVR32_PIN_PB30
#define WEXPANSION_GPIO2                AVR32_PIN_PB31

#define WEXPANSION_SPI                  (&AVR32_SPI0)
#define WEXPANSION_SPI_NPCS             2
#define WEXPANSION_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define WEXPANSION_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define WEXPANSION_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define WEXPANSION_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define WEXPANSION_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define WEXPANSION_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define WEXPANSION_SPI_NPCS_PIN         AVR32_SPI0_NPCS_2_0_PIN
#define WEXPANSION_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_2_0_FUNCTION

//! @}

/*! \name ET024006DHU TFT display
 */
//! @{

#define ET024006DHU_TE_PIN              AVR32_PIN_PX19
#define ET024006DHU_RESET_PIN           AVR32_PIN_PX22
#define ET024006DHU_BL_PIN              AVR32_PWM_6_PIN
#define ET024006DHU_BL_FUNCTION         AVR32_PWM_6_FUNCTION
#define ET024006DHU_DNC_PIN             AVR32_EBI_ADDR_21_1_PIN
#define ET024006DHU_DNC_FUNCTION        AVR32_EBI_ADDR_21_1_FUNCTION
#define ET024006DHU_EBI_NCS_PIN         AVR32_EBI_NCS_0_1_PIN
#define ET024006DHU_EBI_NCS_FUNCTION    AVR32_EBI_NCS_0_1_FUNCTION

#define ET024006_IFACE_MODE             ET024006_IFACE_MODE_EBI
#define ET024006_CMD_ADDR               ((volatile unsigned short int *) AVR32_EBI_CS0_ADDRESS)
// The select signal connected to address line 21,
// this implies an address offset of 0x200000.
#define ET024006_PARAM_ADDR             ((volatile unsigned short int *) (AVR32_EBI_CS0_ADDRESS + 0x00200000))

//! @}
/*! \name Optional SPI connection to the TFT
 */
//! @{

#define ET024006DHU_SPI                  (&AVR32_SPI0)
#define ET024006DHU_SPI_NPCS             3
#define ET024006DHU_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define ET024006DHU_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define ET024006DHU_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define ET024006DHU_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define ET024006DHU_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define ET024006DHU_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define ET024006DHU_SPI_NPCS_PIN         AVR32_SPI1_NPCS_3_0_PIN
#define ET024006DHU_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_3_0_FUNCTION

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
#define TPA6130_ABDAC_PDCA_IRQ_GROUP AVR32_PDCA_IRQ_GROUP
#define TPA6130_ABDAC_PDCA_INT_LEVEL AVR32_INTC_INT3

#define TPA6130_TWI                  (&AVR32_TWI)
#define TPA6130_TWI_SCL_PIN          AVR32_TWI_SCL_0_0_PIN
#define TPA6130_TWI_SCL_FUNCTION     AVR32_TWI_SCL_0_0_FUNCTION
#define TPA6130_TWI_SDA_PIN          AVR32_TWI_SDA_0_0_PIN
#define TPA6130_TWI_SDA_FUNCTION     AVR32_TWI_SDA_0_0_FUNCTION

//! }@
/*! \name TI TLV320AIC23B sound chip
 */
//! @{
#define TLV320_SSC                           (&AVR32_SSC)
#define TLV320_SSC_TX_CLOCK_PIN              AVR32_SSC_TX_CLOCK_0_PIN
#define TLV320_SSC_TX_CLOCK_FUNCTION         AVR32_SSC_TX_CLOCK_0_FUNCTION
#define TLV320_SSC_TX_DATA_PIN               AVR32_SSC_TX_DATA_0_PIN
#define TLV320_SSC_TX_DATA_FUNCTION          AVR32_SSC_TX_DATA_0_FUNCTION
#define TLV320_SSC_TX_FRAME_SYNC_PIN         AVR32_SSC_TX_FRAME_SYNC_0_PIN
#define TLV320_SSC_TX_FRAME_SYNC_FUNCTION    AVR32_SSC_TX_FRAME_SYNC_0_FUNCTION

#define TLV320_TWI                   (&AVR32_TWI)
#define TLV320_TWI_SCL_PIN           AVR32_TWI_SCL_0_0_PIN
#define TLV320_TWI_SCL_FUNCTION      AVR32_TWI_SCL_0_0_FUNCTION
#define TLV320_TWI_SDA_PIN           AVR32_TWI_SDA_0_0_PIN
#define TLV320_TWI_SDA_FUNCTION      AVR32_TWI_SDA_0_0_FUNCTION

#define TLV320_PM_GCLK_PIN         AVR32_PM_GCLK_0_0_PIN
#define TLV320_PM_GCLK_FUNCTION    AVR32_PM_GCLK_0_0_FUNCTION
//! @}

/*! \name USART connection to the UC3B board controller
 */
//! @{
#define USART                               (&AVR32_USART0)
#define USART_RXD_PIN                       AVR32_USART0_RXD_0_0_PIN
#define USART_RXD_FUNCTION                  AVR32_USART0_RXD_0_0_FUNCTION
#define USART_TXD_PIN                       AVR32_USART0_TXD_0_0_PIN
#define USART_TXD_FUNCTION                  AVR32_USART0_TXD_0_0_FUNCTION
#define USART_IRQ                           AVR32_USART0_IRQ
#define USART_IRQ_GROUP                     AVR32_USART0_IRQ_GROUP
#define USART_SYSCLK                        SYSCLK_USART0
//! @}

#define ADC_VEXT_PIN         AVR32_ADC_AD_7_PIN
#define ADC_VEXT_FUNCTION    AVR32_ADC_AD_7_FUNCTION

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

#define ET024006DHU_EBI_ADDR_21   AVR32_EBI_ADDR_21_1

#define ET024006DHU_EBI_NWE       AVR32_EBI_NWE0_0
#define ET024006DHU_EBI_NRD       AVR32_EBI_NRD_0
#define ET024006DHU_EBI_NCS       AVR32_EBI_NCS_0_1
//! @}


/*! \name MACB connections to the DP83848 external phy controller
 */
//! @{

//! GPIO connection of the MAC PHY PWR_DOWN/INT signal for the external phy controller
#define MACB_INTERRUPT_PIN          AVR32_PIN_PA26
#define EXTPHY_MACB_INTERRUPT_PIN   MACB_INTERRUPT_PIN // Added for homogeneity

#define EXTPHY_MACB                 (&AVR32_MACB)
#define EXTPHY_MACB_MDC_PIN         AVR32_MACB_MDC_0_PIN
#define EXTPHY_MACB_MDC_FUNCTION    AVR32_MACB_MDC_0_FUNCTION
#define EXTPHY_MACB_MDIO_PIN        AVR32_MACB_MDIO_0_PIN
#define EXTPHY_MACB_MDIO_FUNCTION   AVR32_MACB_MDIO_0_FUNCTION
#define EXTPHY_MACB_RXD_0_PIN       AVR32_MACB_RXD_0_PIN
#define EXTPHY_MACB_RXD_0_FUNCTION  AVR32_MACB_RXD_0_FUNCTION
#define EXTPHY_MACB_RXD_1_PIN       AVR32_MACB_RXD_1_PIN
#define EXTPHY_MACB_RXD_1_FUNCTION  AVR32_MACB_RXD_1_FUNCTION
#define EXTPHY_MACB_TXD_0_PIN       AVR32_MACB_TXD_0_PIN
#define EXTPHY_MACB_TXD_0_FUNCTION  AVR32_MACB_TXD_0_FUNCTION
#define EXTPHY_MACB_TXD_1_PIN       AVR32_MACB_TXD_1_PIN
#define EXTPHY_MACB_TXD_1_FUNCTION  AVR32_MACB_TXD_1_FUNCTION
#define EXTPHY_MACB_TX_EN_PIN       AVR32_MACB_TX_EN_0_PIN
#define EXTPHY_MACB_TX_EN_FUNCTION  AVR32_MACB_TX_EN_0_FUNCTION
#define EXTPHY_MACB_RX_ER_PIN       AVR32_MACB_RX_ER_0_PIN
#define EXTPHY_MACB_RX_ER_FUNCTION  AVR32_MACB_RX_ER_0_FUNCTION
#define EXTPHY_MACB_RX_DV_PIN       AVR32_MACB_RX_DV_0_PIN
#define EXTPHY_MACB_RX_DV_FUNCTION  AVR32_MACB_RX_DV_0_FUNCTION
#define EXTPHY_MACB_TX_CLK_PIN      AVR32_MACB_TX_CLK_0_PIN
#define EXTPHY_MACB_TX_CLK_FUNCTION AVR32_MACB_TX_CLK_0_FUNCTION

//! Phy Address (set through strap options)
#define EXTPHY_PHY_ADDR             0x01

//! @}

#endif

#endif  // _EVK1105_H_
