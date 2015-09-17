/**
 * \file
 *
 * \brief API driver for component ADS7843.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "ads7843.h"
#include "conf_ads7843.h"
#include "board.h"
#include "ioport.h"
#include "pio_handler.h"
#include "pio.h"
#include "spi_master.h"
#include "conf_spi_master.h"

/** PD0 */
#define ADS_CTRL_PD0              (1 << 0)
/** PD1 */
#define ADS_CTRL_PD1              (1 << 1)
/** SER/DFR */
#define ADS_CTRL_DFR              (1 << 2)
/** Mode */
#define ADS_CTRL_EIGHT_BITS_MOD   (1 << 3)
/** Start Bit */
#define ADS_CTRL_START            (1 << 7)
/** Address setting */
#define ADS_CTRL_SWITCH_SHIFT     4

/** Get X position command */
#define CMD_Y_POSITION ((1 << ADS_CTRL_SWITCH_SHIFT) | ADS_CTRL_START |\
		ADS_CTRL_PD0 | ADS_CTRL_PD1)

/** Get Y position command */
#define CMD_X_POSITION ((5 << ADS_CTRL_SWITCH_SHIFT) | ADS_CTRL_START |\
		ADS_CTRL_PD0 | ADS_CTRL_PD1)

/** Enable penIRQ */
#define CMD_ENABLE_PENIRQ  ((1 << ADS_CTRL_SWITCH_SHIFT) | ADS_CTRL_START)

#define ADS7843_TIMEOUT        5000000
#define ADS7843_BUFSIZE        3

/** Frequence rate for sending one bit */
#define ADS7843_SPI_BAUDRATE   1000000

/** 2us min (tCSS) <=> 200/100 000 000 = 2us */
#define DELAY_BEFORE_SPCK          200
/** 5us min (tCSH) <=> (32 * 15) / (100 000 000) = 5us */
#define DELAY_BETWEEN_CONS_COM     0xf

/** @cond 0*/
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/** @endcond*/

/**
 * \brief Send a command to the ADS7843 touch controller.
 *
 * \param uc_cmd command to send.
 *
 * \return Command result.
 */
static uint32_t ads7843_send_cmd(uint8_t uc_cmd)
{
	uint32_t uResult = 0;
	volatile uint32_t i;
	uint8_t data;
	uint32_t timeout = SPI_TIMEOUT;

	/** (volatile declaration needed for code optimisation by compiler) */
	volatile uint8_t bufferRX[ADS7843_BUFSIZE];
	volatile uint8_t bufferTX[ADS7843_BUFSIZE];

	bufferRX[0] = 0;
	bufferRX[1] = 0;
	bufferRX[2] = 0;

	bufferTX[0] = uc_cmd;
	bufferTX[1] = 0;
	bufferTX[2] = 0;

	for(i = 0; i < ADS7843_BUFSIZE; i++){
		timeout = SPI_TIMEOUT;
		while (!spi_is_tx_ready(BOARD_ADS7843_SPI_BASE)) {
			if (!timeout--) {
				return SPI_ERROR_TIMEOUT;
			}
		}
		spi_write_single(BOARD_ADS7843_SPI_BASE, bufferTX[i]);
	}
	for(i = 0; i < ADS7843_BUFSIZE; i++){
		timeout = SPI_TIMEOUT;
		while (!spi_is_rx_full(BOARD_ADS7843_SPI_BASE)) {
			if (!timeout--) {
				return SPI_ERROR_TIMEOUT;
			}
		}
		spi_read_single(BOARD_ADS7843_SPI_BASE, &data);
		bufferRX[i] = data;
	}

	uResult = (uint32_t)bufferRX[1] << 8;
	uResult |= (uint32_t)bufferRX[2];
	uResult = uResult >> 4;

	return uResult;
}

uint32_t ads7843_is_pressed(void)
{
	return (ioport_get_pin_level(BOARD_ADS7843_IRQ_GPIO) ==
			IOPORT_PIN_LEVEL_LOW);
}

void ads7843_set_handler(void (*p_handler) (uint32_t, uint32_t))
{
	/** Initialize interrupts */
	pio_handler_set_pin(BOARD_ADS7843_IRQ_GPIO,
			BOARD_ADS7843_IRQ_FLAGS,
			(void (*)(uint32_t, uint32_t)) p_handler
			);

	/** Enable the interrupt */
	pio_enable_pin_interrupt(BOARD_ADS7843_IRQ_GPIO);
}

void ads7843_enable_interrupt(void)
{
	/** Enable the interrupt */
	pio_enable_pin_interrupt(BOARD_ADS7843_IRQ_GPIO);
}

void ads7843_disable_interrupt(void)
{
	/** Disable the interrupt */
	pio_disable_pin_interrupt(BOARD_ADS7843_IRQ_GPIO);
}

void ads7843_get_raw_point(uint32_t *p_x, uint32_t *p_y)
{
	/** Disable interrupt to quickly evaluate the coordinates */
	pio_disable_pin_interrupt(BOARD_ADS7843_IRQ_GPIO);

	/** Get X position */
	*p_x = ads7843_send_cmd(CMD_X_POSITION);

	/** Get Y position */
	*p_y = ads7843_send_cmd(CMD_Y_POSITION);

	/** Switch to full power mode */
	ads7843_send_cmd(CMD_ENABLE_PENIRQ);

	/** Re-enable interrupt */
	pio_enable_pin_interrupt(BOARD_ADS7843_IRQ_GPIO);
}

uint32_t ads7843_init(void)
{
	volatile uint32_t uDummy;
	struct spi_device ADS7843_SPI_DEVICE_CFG = {
		/** Board specific chip select configuration*/
#ifdef BOARD_ADS7843_SPI_NPCS
		.id = BOARD_ADS7843_SPI_NPCS
#else
#warning The board TouchScreen chip select definition is missing. Default configuration is used.
		.id = 0
#endif
	};

	spi_master_init(BOARD_ADS7843_SPI_BASE);
	spi_master_setup_device(BOARD_ADS7843_SPI_BASE, &ADS7843_SPI_DEVICE_CFG,
			SPI_MODE_0, ADS7843_SPI_BAUDRATE, 0);
	spi_select_device(BOARD_ADS7843_SPI_BASE, &ADS7843_SPI_DEVICE_CFG);
	spi_enable(BOARD_ADS7843_SPI_BASE);

	for (uDummy = 0; uDummy < 100000; uDummy++) {
	}

	ads7843_send_cmd(CMD_ENABLE_PENIRQ);

	return 0;
}

/** @cond 0*/
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/** @endcond*/

/**
 * \}
 */
