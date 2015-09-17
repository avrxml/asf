/**
 * \file
 *
 * \brief Board configuration
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED


#define CONF_BOARD_AT86RFX
#define EXT2_CONFIG
#define AT86RFX_SPI                  EXT2_SPI_MODULE
#define AT86RFX_RST_PIN              EXT2_PIN_7
#define AT86RFX_MISC_PIN             EXT2_PIN_12
#define AT86RFX_IRQ_PIN              EXT2_PIN_9
#define AT86RFX_SLP_PIN              EXT2_PIN_10
#define AT86RFX_SPI_CS               EXT2_PIN_15
#define AT86RFX_SPI_MOSI             EXT2_PIN_16
#define AT86RFX_SPI_MISO             EXT2_PIN_17
#define AT86RFX_SPI_SCK              EXT2_PIN_18
#define AT86RFX_CSD                  EXT2_PIN_5
#define AT86RFX_CPS                  EXT2_PIN_8



#define AT86RFX_SPI_SERCOM_MUX_SETTING   EXT2_SPI_SERCOM_MUX_SETTING
#define AT86RFX_SPI_SERCOM_PINMUX_PAD0   EXT2_SPI_SERCOM_PINMUX_PAD0
#define AT86RFX_SPI_SERCOM_PINMUX_PAD1   PINMUX_UNUSED
#define AT86RFX_SPI_SERCOM_PINMUX_PAD2   EXT2_SPI_SERCOM_PINMUX_PAD2
#define AT86RFX_SPI_SERCOM_PINMUX_PAD3   EXT2_SPI_SERCOM_PINMUX_PAD3


#define AT86RFX_IRQ_CHAN       EXT2_IRQ_INPUT
#define AT86RFX_IRQ_PINMUX     EXT2_IRQ_PINMUX
#define IC_TYPE             (0x00)

#define MCU_SOC_NAME        "ATSAMR21G18A"

#define AT86RFX_SPI_BAUDRATE		 4000000UL



#endif /* CONF_BOARD_H_INCLUDED */
