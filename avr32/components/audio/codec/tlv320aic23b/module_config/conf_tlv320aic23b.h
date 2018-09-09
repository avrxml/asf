/*****************************************************************************
 *
 * \file
 *
 * \brief TLV320AIC23B stereo audio CODEC driver configuration file for AVR32.
 *
 * This file contains the possible external configuration of the TLV320AIC23B.
 *
 *                       can be used.
 *
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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


 //! Configuration of the TLV320AIC23B stereo audio CODEC driver

#ifndef _CONF_TLV320AIC23B_H_
#define _CONF_TLV320AIC23B_H_

#include "tlv320aic23b.h"

#include <avr32/io.h>
#include "intc.h"
#include "board.h"

//! Mode Selected: \c AIC23B_MODE_DAC,  \c AIC23B_MODE_ADC or \c AIC23B_MODE_CODEC.
#define AIC23B_MODE                         AIC23B_MODE_CODEC

//! Control interface: \c AIC23B_CTRL_INTERFACE_TWI or \c AIC23B_CTRL_INTERFACE_SPI.
#define AIC23B_CTRL_INTERFACE               AIC23B_CTRL_INTERFACE_TWI

//! Master clock (in Hz): \c 12000000 or \c 11289600.
#define AIC23B_MCLK_HZ                      11289600

// If the selected control interface is TWI...
#if AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_TWI

  /*! \name TWI Connections
   */
  //! @{
  #define AIC23B_TWI                          TLV320_TWI
  #define AIC23B_TWI_ADDRESS                  0x1A                    //!< TWI address: \c 0x1A if /CS state is 0 or \c 0x1B if /CS state is 1.
  #define AIC23B_TWI_MASTER_SPEED             400000
  #define AIC23B_TWI_SCL_PIN                  TLV320_TWI_SCL_PIN
  #define AIC23B_TWI_SCL_FUNCTION             TLV320_TWI_SCL_FUNCTION
  #define AIC23B_TWI_SDA_PIN                  TLV320_TWI_SDA_PIN
  #define AIC23B_TWI_SDA_FUNCTION             TLV320_TWI_SDA_FUNCTION
  //! @}

// If the selected control interface is SPI...
#elif AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_SPI

  /*! \name SPI Connections
   */
  //! @{
  #define AIC23B_SPI                          SPARE_SPI
  #define AIC23B_SPI_NPCS                     SPARE_SPI_NPCS
  #define AIC23B_SPI_MASTER_SPEED             12500000
  #define AIC23B_SPI_SCK_PIN                  SPARE_SPI_SCK_PIN
  #define AIC23B_SPI_SCK_FUNCTION             SPARE_SPI_SCK_FUNCTION
  #define AIC23B_SPI_MISO_PIN                 SPARE_SPI_MISO_PIN
  #define AIC23B_SPI_MISO_FUNCTION            SPARE_SPI_MISO_FUNCTION
  #define AIC23B_SPI_MOSI_PIN                 SPARE_SPI_MOSI_PIN
  #define AIC23B_SPI_MOSI_FUNCTION            SPARE_SPI_MOSI_FUNCTION
  #define AIC23B_SPI_NPCS_PIN                 SPARE_SPI_NPCS_PIN
  #define AIC23B_SPI_NPCS_FUNCTION            SPARE_SPI_NPCS_FUNCTION
  //! @}

#endif

/*! \name SSC Connections
 */
//! @{
#define AIC23B_SSC                          (&AVR32_SSC)
#define AIC23B_SSC_TX_PDCA_PID              AVR32_PDCA_PID_SSC_TX
#define AIC23B_SSC_TX_PDCA_CHANNEL          0
#define AIC23B_SSC_TX_PDCA_IRQ              AVR32_PDCA_IRQ_0
#define AIC23B_SSC_TX_PDCA_INT_LEVEL        AVR32_INTC_INT3
#define AIC23B_SSC_TX_CLOCK_PIN             AVR32_SSC_TX_CLOCK_0_PIN
#define AIC23B_SSC_TX_CLOCK_FUNCTION        AVR32_SSC_TX_CLOCK_0_FUNCTION
#define AIC23B_SSC_TX_DATA_PIN              AVR32_SSC_TX_DATA_0_PIN
#define AIC23B_SSC_TX_DATA_FUNCTION         AVR32_SSC_TX_DATA_0_FUNCTION
#define AIC23B_SSC_TX_FRAME_SYNC_PIN        AVR32_SSC_TX_FRAME_SYNC_0_PIN
#define AIC23B_SSC_TX_FRAME_SYNC_FUNCTION   AVR32_SSC_TX_FRAME_SYNC_0_FUNCTION

#define AIC23B_SSC_RX_PDCA_PID              AVR32_PDCA_PID_SSC_RX
#define AIC23B_SSC_RX_PDCA_CHANNEL          1
#define AIC23B_SSC_RX_PDCA_IRQ              AVR32_PDCA_IRQ_1
#define AIC23B_SSC_RX_PDCA_INT_LEVEL        AVR32_INTC_INT3
#define AIC23B_SSC_RX_DATA_PIN              AVR32_SSC_RX_DATA_0_PIN
#define AIC23B_SSC_RX_DATA_FUNCTION         AVR32_SSC_RX_DATA_0_FUNCTION
#define AIC23B_SSC_RX_FRAME_SYNC_PIN        AVR32_SSC_RX_FRAME_SYNC_0_PIN
#define AIC23B_SSC_RX_FRAME_SYNC_FUNCTION   AVR32_SSC_RX_FRAME_SYNC_0_FUNCTION
//! @}

// choose which input to select for the codec
#define AIC23B_INPUT                        AIC23B_INPUT_MIC
// #define AIC23B_INPUT                        AIC23B_INPUT_LINE
#endif  // _CONF_TLV320AIC23B_H_
