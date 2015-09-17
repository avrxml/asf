/**
 * \file
 *
 * \brief UC3 MCI driver
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

#include <asf.h>
#include "conf_board.h"
#include "sd_mmc_protocol.h"
#include "sysclk.h"
#include "mci.h"

/**
 * \ingroup avr32_drivers_mci
 * \defgroup avr32_drivers_mci_internal MultiMedia Card Interface (MCI)
 *implementation
 *
 * @{
 */

/* These defines are missing from or wrong in the toolchain header files
 * uc3cxxx.h */
#ifndef AVR32_MCI_MR_FBYTE_MASK
#  define AVR32_MCI_MR_FBYTE_MASK    AVR32_MCI_MR_PDCFBYTE_MASK
#endif


/**
 * Registers masks definitions
 * @{
 */
#define AVR32_MCI_DTOR_DTOMUL_1048576_MASK \
		(AVR32_MCI_DTOR_DTOMUL_1048576 << AVR32_MCI_DTOR_DTOMUL_OFFSET)
#define AVR32_MCI_CSTOR_CSTOMUL_1048576_MASK \
		(AVR32_MCI_CSTOR_CSTOMUL_1048576 << AVR32_MCI_CSTOR_CSTOMUL_OFFSET)
#define AVR32_MCI_CSTOR_CSTOCYC_2_MASK \
		( 2 << AVR32_MCI_CSTOR_CSTOCYC_OFFSET)
#define AVR32_MCI_DTOR_DTOCYC_2_MASK \
		( 2 << AVR32_MCI_DTOR_DTOCYC_OFFSET)
#define AVR32_MCI_SDCR_SDCBUS_1_BIT_MASK \
		( AVR32_MCI_SDCR_SDCBUS_1_BIT << AVR32_MCI_SDCR_SDCBUS_OFFSET)
#define AVR32_MCI_SDCR_SDCBUS_4_BIT_MASK \
		( AVR32_MCI_SDCR_SDCBUS_4_BIT << AVR32_MCI_SDCR_SDCBUS_OFFSET)
#define AVR32_MCI_SDCR_SDCBUS_8_BIT_MASK \
		( AVR32_MCI_SDCR_SDCBUS_8_BIT << AVR32_MCI_SDCR_SDCBUS_OFFSET)
#define AVR32_MCI_SDCR_SDCSEL_0_MASK \
		( 0 << AVR32_MCI_SDCR_SDCSEL_OFFSET)
#define AVR32_MCI_SDCR_SDCSEL_1_MASK \
		( 1 << AVR32_MCI_SDCR_SDCSEL_OFFSET)
#define AVR32_MCI_CMDR_TRCMD_START_TRANS_MASK \
		( AVR32_MCI_CMDR_TRCMD_START_TRANS << AVR32_MCI_CMDR_TRCMD_OFFSET)
#define AVR32_MCI_CMDR_TRDIR_READ_MASK \
		( AVR32_MCI_CMDR_TRDIR_READ << AVR32_MCI_CMDR_TRDIR_OFFSET)
#define AVR32_MCI_CMDR_TRTYP_SDIO_BYTE_MASK \
		( AVR32_MCI_CMDR_TRTYP_SDIO_BYTE << AVR32_MCI_CMDR_TRTYP_OFFSET)
#define AVR32_MCI_CMDR_TRTYP_SDIO_BLOCK_MASK \
		( AVR32_MCI_CMDR_TRTYP_SDIO_BLOCK << AVR32_MCI_CMDR_TRTYP_OFFSET)
#define AVR32_MCI_CMDR_TRTYP_STREAM_MASK \
		( AVR32_MCI_CMDR_TRTYP_STREAM << AVR32_MCI_CMDR_TRTYP_OFFSET)
#define AVR32_MCI_CMDR_TRTYP_BLOCK_MASK \
		( AVR32_MCI_CMDR_TRTYP_BLOCK << AVR32_MCI_CMDR_TRTYP_OFFSET)
#define AVR32_MCI_CMDR_TRTYP_MULTI_BLOCK_MASK \
		( AVR32_MCI_CMDR_TRTYP_MULTI_BLOCK << AVR32_MCI_CMDR_TRTYP_OFFSET)
/* ! @} */

/* Check configurations */
#if (!defined SD_MMC_MCI_MEM_CNT) || (SD_MMC_MCI_MEM_CNT == 0)
#  warning SD_MMC_MCI_MEM_CNT must be defined in board.h file.
#  define SD_MMC_MCI_MEM_CNT 1
#endif
#ifndef CONF_BOARD_SD_MMC_MCI
#  warning CONF_BOARD_SD_MMC_MCI must be defined in conf_board.h file.
#endif
#if (SD_MMC_MCI_MEM_CNT > 2)
#  warning Wrong definition for SD_MMC_MCI_MEM_CNT in board.h, \
   this part has a maximum of 2 slots on MCI.
#endif
#ifndef SD_MMC_MCI_SLOT_0_SIZE
#  warning SD_MMC_MCI_SLOT_0_SIZE must be defined in board.h.
#  define SD_MMC_MCI_SLOT_0_SIZE 1
#endif
#ifndef SD_MMC_MCI_SLOT_0_DATA0_PIN
#  warning SD_MMC_MCI_SLOT_0_DATA0_PIN must be defined in board.h.
#  define SD_MMC_MCI_SLOT_0_DATA0_PIN 0
#endif
#ifndef SD_MMC_MCI_SLOT_0_DATA3_PIN
#  warning SD_MMC_MCI_SLOT_0_DATA3_PIN must be defined in board.h.
#  define SD_MMC_MCI_SLOT_0_DATA3_PIN 0
#endif
#if (SD_MMC_MCI_MEM_CNT == 2)
#  ifndef SD_MMC_MCI_SLOT_1_SIZE
#    warning SD_MMC_MCI_SLOT_1_SIZE must be defined in board.h.
#    define SD_MMC_MCI_SLOT_1_SIZE 1
#  endif
#  ifndef SD_MMC_MCI_SLOT_1_DATA0_PIN
#    warning SD_MMC_MCI_SLOT_1_DATA0_PIN must be defined in board.h.
#    define SD_MMC_MCI_SLOT_1_DATA0_PIN 0
#  endif
#  ifndef SD_MMC_MCI_SLOT_1_DATA3_PIN
#    warning SD_MMC_MCI_SLOT_1_DATA3_PIN must be defined in board.h.
#    define SD_MMC_MCI_SLOT_1_DATA3_PIN 0
#  endif
#endif

/* Enable debug information for SD/MMC SPI module */
#ifdef MCI_DEBUG
#include <stdio.h>
#  define mci_debug(...)      printf(__VA_ARGS__)
#else
#  define mci_debug(...)
#endif

#ifndef CONF_MCI_DMACA_CHANNEL
#  define CONF_MCI_DMACA_CHANNEL 1
#endif

/* ! Current byte position of the transfer started by mci_adtc_start() */
static uint32_t mci_transfert_pos;

/* ! Size block requested by last call of mci_adtc_start() */
static uint16_t mci_block_size;

/* ! Total number of block requested by last mci_adtc_start() */
static uint16_t mci_nb_block;

static void mci_reset(void);
static void mci_set_speed(uint32_t speed, uint32_t pbb_hz);
static bool mci_wait_busy(void);
static bool mci_wait_busy_on_line(void);
static bool mci_send_cmd_execute(uint32_t cmdr, sdmmc_cmd_def_t cmd,
		uint32_t arg);

/**
 * \brief Reset the MCI interface
 */
static void mci_reset(void)
{
	uint32_t mr = AVR32_MCI.mr;
	uint32_t dtor = AVR32_MCI.dtor;
	uint32_t sdcr = AVR32_MCI.sdcr;
	uint32_t cstor = AVR32_MCI.cstor;
	uint32_t cfg = AVR32_MCI.cfg;
	AVR32_MCI.cr = AVR32_MCI_CR_SWRST_MASK;
	AVR32_MCI.mr = mr;
	AVR32_MCI.dtor = dtor;
	AVR32_MCI.sdcr = sdcr;
	AVR32_MCI.cstor = cstor;
	AVR32_MCI.cfg = cfg;
	AVR32_MCI.dma = 0;
	/* Enable the MCI and the Power Saving */
	AVR32_MCI.cr = AVR32_MCI_CR_MCIEN_MASK | AVR32_MCI_PWSEN_MASK;
}

/**
 * \brief Set speed of the MCI clock.
 *
 * \param speed    MCI clock speed in Hz.
 * \param pbb_hz   PBB clock speed in Hz.
 */
static void mci_set_speed(uint32_t speed, uint32_t pbb_hz)
{
  uint32_t clkdiv;
  uint32_t rest;

	/* Speed = PBB clock / (2 * (CLKDIV + 1)) */
	if (speed > 0) {
		clkdiv = pbb_hz / (2 * speed);
		rest = pbb_hz % (2 * speed);
		if (rest > 0) {
			/* Ensure that the card speed not be higher
			 * than expected. */
			clkdiv++;
    }

		if (clkdiv > 0) {
      clkdiv -= 1;
    }
	} else {
    clkdiv = 0;
  }

	AVR32_MCI.mr &= ~AVR32_MCI_MR_CLKDIV_MASK;
	AVR32_MCI.mr |= clkdiv << AVR32_MCI_MR_CLKDIV_OFFSET;
}

/** \brief Wait the end of busy signal on data line
 *
 * \return true if success, otherwise false
 */
static bool mci_wait_busy(void)
{
	uint32_t busy_wait = 1000000;
	uint32_t sr;

	do {
		sr = AVR32_MCI.sr;
		if (busy_wait-- == 0) {
			mci_debug("%s: timeout\n\r", __func__);
			mci_reset();
			return false;
		}
	} while (!((sr & AVR32_MCI_SR_NOTBUSY_MASK)
			&& ((sr & AVR32_MCI_SR_DTIP_MASK) == 0)));
	return true;
}

/** \brief Wait the end of busy signal on data line through GPIO
 *
 * \return true if success, otherwise false
 */
static bool mci_wait_busy_on_line(void)
{
	uint32_t busy_wait = 10000000;
	if (AVR32_MCI.sdcr & AVR32_MCI_SDCR_SDCSEL_MASK) {
		/* Slot 0 */
		do {
			if (busy_wait-- == 0) {
				mci_debug("%s: timeout\n\r", __func__);
				mci_reset();
				return false;
			}
		} while (!(gpio_get_pin_value(SD_MMC_MCI_SLOT_0_DATA0_PIN)));
	} else {
#if (SD_MMC_MCI_MEM_CNT == 2)
		/* Slot 1 */
		do {
			if (busy_wait-- == 0) {
				mci_debug("%s: timeout\n\r", __func__);
				mci_reset();
				return false;
			}
		} while (!(gpio_get_pin_value(SD_MMC_MCI_SLOT_1_DATA0_PIN)));
#endif
	}

	return true;
}

/** \brief Send a command
 *
 * \param cmdr       CMDR register bit mask to use for this command
 * \param cmd        Command definition
 * \param arg        Argument of the command
 *
 * \return true if success, otherwise false
 */
static bool mci_send_cmd_execute(uint32_t cmdr, sdmmc_cmd_def_t cmd,
		uint32_t arg)
{
	uint32_t sr;

	if (SDMMC_MCI_CMD0_GO_IDLE_STATE == cmd) {
		gpio_enable_gpio_pin(SD_MMC_MCI_SLOT_0_DATA3_PIN);
		gpio_configure_pin(SD_MMC_MCI_SLOT_0_DATA3_PIN,
				GPIO_DIR_INPUT | GPIO_PULL_UP);
#if (SD_MMC_MCI_MEM_CNT == 2)
		gpio_enable_gpio_pin(SD_MMC_MCI_SLOT_1_DATA3_PIN);
		gpio_configure_pin(SD_MMC_MCI_SLOT_1_DATA3_PIN,
				GPIO_DIR_INPUT | GPIO_PULL_UP);
#endif
	}

	cmdr |= (cmd << AVR32_MCI_CMDR_CMDNB_OFFSET)
			& AVR32_MCI_CMDR_CMDNB_MASK;
	if (cmd & SDMMC_RESP_PRESENT) {
		cmdr |= AVR32_MCI_CMDR_MAXLAT_MASK;
		if (cmd & SDMMC_RESP_136) {
			cmdr |= AVR32_MCI_CMDR_RSPTYP_136_BIT_RESP
					<< AVR32_MCI_CMDR_RSPTYP_OFFSET;
		} else if (cmd & SDMMC_RESP_BUSY) {
			cmdr |= AVR32_MCI_CMDR_RSPTYP_48_BIT_RESP_WITH_BUSY
					<< AVR32_MCI_CMDR_RSPTYP_OFFSET;
		} else {
			cmdr |= AVR32_MCI_CMDR_RSPTYP_48_BIT_RESP
					<< AVR32_MCI_CMDR_RSPTYP_OFFSET;
		}
	}

	if (cmd & SDMMC_CMD_OPENDRAIN) {
		cmdr |= AVR32_MCI_CMDR_OPDCMD_MASK;
	}

	/* Write argument */
	AVR32_MCI.argr = arg;
	/* Write and start command */
	AVR32_MCI.cmdr = cmdr;

	/* Wait end of command
	 * Note: no need of timeout, because it is include in MCI */
	do {
		sr = AVR32_MCI.sr;
		if (cmd & SDMMC_RESP_CRC) {
			if (sr & (AVR32_MCI_SR_CSTOE_MASK | AVR32_MCI_SR_RTOE_MASK
					| AVR32_MCI_SR_RENDE_MASK | AVR32_MCI_SR_RCRCE_MASK
					| AVR32_MCI_SR_RDIRE_MASK | AVR32_MCI_SR_RINDE_MASK)) {
				mci_debug("%s: CMD 0x%08x sr 0x%08x error\n\r",
						__func__, cmd, sr);
				mci_reset();
				return false;
			}
		} else {
			if (sr & (AVR32_MCI_SR_CSTOE_MASK | AVR32_MCI_SR_RTOE_MASK
					| AVR32_MCI_SR_RENDE_MASK
					| AVR32_MCI_SR_RDIRE_MASK | AVR32_MCI_SR_RINDE_MASK)) {
				mci_debug("%s: CMD 0x%08x sr 0x%08x error\n\r",
						__func__, cmd, sr);
				mci_reset();
				return false;
			}
		}
	} while (!(sr & AVR32_MCI_SR_CMDRDY_MASK));

	if (cmd & SDMMC_RESP_BUSY) {
		if (!mci_wait_busy()) {
			return false;
		}
	}

	if (SDMMC_MCI_CMD0_GO_IDLE_STATE == cmd) {
		/* Disable GPIO control */
		volatile avr32_gpio_port_t *gpio_port;
		gpio_port = &AVR32_GPIO.port[SD_MMC_MCI_SLOT_0_DATA3_PIN >> 5];
		gpio_port->gperc = 1 << (SD_MMC_MCI_SLOT_0_DATA3_PIN & 0x1F);
#if (SD_MMC_MCI_MEM_CNT == 2)
		gpio_port = &AVR32_GPIO.port[SD_MMC_MCI_SLOT_1_DATA3_PIN >> 5];
		gpio_port->gperc = 1 << (SD_MMC_MCI_SLOT_1_DATA3_PIN & 0x1F);
#endif
	}

	return true;
}

/* ------------------------------------------------------------------- */
/* --------------------- PUBLIC FUNCTIONS ---------------------------- */

void mci_init(void)
{
	sysclk_enable_pbb_module(SYSCLK_MCI);
	sysclk_enable_hsb_module(SYSCLK_DMACA);

	/* Set the Data Timeout Register to 2 Mega Cycles */
	AVR32_MCI.dtor = AVR32_MCI_DTOR_DTOMUL_1048576_MASK |
			AVR32_MCI_DTOR_DTOCYC_2_MASK;
	/* Set Completion Signal Timeout to 2 Mega Cycles */
	AVR32_MCI.cstor = AVR32_MCI_CSTOR_CSTOMUL_1048576_MASK |
			AVR32_MCI_CSTOR_CSTOCYC_2_MASK;
	/* Setup configuration register */
	AVR32_MCI.cfg = AVR32_MCI_FIFOMODE_ASAP |
			AVR32_MCI_FERRCTRL_WHEN_STATUS_READ;

	/* Set power saving to maximum value */
	AVR32_MCI.mr = AVR32_MCI_MR_PWSDIV_MASK |
			AVR32_MCI_MR_CLKDIV_MASK;
	AVR32_MCI.sdcr = AVR32_MCI_SDCR_SDCBUS_1_BIT_MASK;

	/* Enable the MCI and the Power Saving */
	AVR32_MCI.cr = AVR32_MCI_CR_MCIEN_MASK | AVR32_MCI_PWSEN_MASK;
}

uint8_t mci_get_bus_width(uint8_t slot)
{
	switch (slot) {
	case 0:
		return SD_MMC_MCI_SLOT_0_SIZE;

#if (SD_MMC_MCI_MEM_CNT == 2)
	case 1:
		return SD_MMC_MCI_SLOT_1_SIZE;

#endif
	default:
		return 0; /* Slot number wrong */
	}
}

bool mci_is_high_speed_capable(void)
{
	return true;
}

void mci_select_device(uint8_t slot, uint32_t clock, uint8_t bus_width,
		bool high_speed)
{
	uint32_t mci_slot = AVR32_MCI_SDCR_SDCSEL_0_MASK;
	uint32_t mci_bus_width = AVR32_MCI_SDCR_SDCBUS_1_BIT_MASK;

	if (high_speed) {
		AVR32_MCI.cfg |= AVR32_MCI_CFG_HSMODE_MASK;
	} else {
		AVR32_MCI.cfg &= ~AVR32_MCI_CFG_HSMODE_MASK;
}

	mci_set_speed(clock, sysclk_get_pbb_hz());

	switch (slot) {
	case 0:
		mci_slot = AVR32_MCI_SDCR_SDCSEL_0_MASK;
		break;

#if (SD_MMC_MCI_MEM_CNT == 2)
	case 1:
		mci_slot = AVR32_MCI_SDCR_SDCSEL_1_MASK;
		break;

#endif
	default:
		Assert(false); /* Wrong slot number */
	}

	switch (bus_width) {
	case 1:
		mci_bus_width = AVR32_MCI_SDCR_SDCBUS_1_BIT_MASK;
		break;

	case 4:
		mci_bus_width = AVR32_MCI_SDCR_SDCBUS_4_BIT_MASK;
		break;

	case 8:
		mci_bus_width = AVR32_MCI_SDCR_SDCBUS_8_BIT_MASK;
		break;

	default:
		Assert(false); /* Wrong bus width */
	}
	/* Set the SD/MMC Card Register */
	AVR32_MCI.sdcr = mci_slot | mci_bus_width;
}

void mci_deselect_device(uint8_t slot)
{
	UNUSED(slot);
	AVR32_MCI.mr = AVR32_MCI_MR_PWSDIV_MASK | AVR32_MCI_MR_CLKDIV_MASK;
	AVR32_MCI.sdcr = AVR32_MCI_SDCR_SDCBUS_1_BIT_MASK;
}

void mci_send_clock(void)
{
	/* Configure command */
	AVR32_MCI.mr &= ~(AVR32_MCI_MR_WRPROOF_MASK |
			AVR32_MCI_MR_RDPROOF_MASK  | AVR32_MCI_MR_FBYTE_MASK);
	/* Write argument */
	AVR32_MCI.argr = 0;
	/* Write and start initialization command */
	AVR32_MCI.cmdr = (AVR32_MCI_CMDR_RSPTYP_NO_RESP
			<< AVR32_MCI_CMDR_RSPTYP_OFFSET)
			| (AVR32_MCI_CMDR_SPCMD_INIT_CMD
			<< AVR32_MCI_CMDR_SPCMD_OFFSET)
			| AVR32_MCI_CMDR_OPDCMD_MASK;
	/* Wait end of initialization command */
	while (!(AVR32_MCI.sr & AVR32_MCI_SR_CMDRDY_MASK)) {
	}
}

bool mci_send_cmd(sdmmc_cmd_def_t cmd, uint32_t arg)
{
	/* Configure command */
	AVR32_MCI.mr &= ~(AVR32_MCI_MR_WRPROOF_MASK |
			AVR32_MCI_MR_RDPROOF_MASK | AVR32_MCI_MR_FBYTE_MASK);
	AVR32_MCI.blkr = 0;
	return mci_send_cmd_execute(0, cmd, arg);
}

uint32_t mci_get_response(void)
  {
	return AVR32_MCI.rspr0;
}

void mci_get_response_128(uint8_t *response)
    {
	uint32_t response_32;

	for (uint8_t i = 4; i > 0; i--) {
		response_32 = AVR32_MCI.rspr0;
		*response = (response_32 >> 24);
		response++;
		*response = (response_32 >> 16) & 0xFF;
		response++;
		*response = (response_32 >>  8) & 0xFF;
		response++;
		*response = (response_32) & 0xFF;
		response++;
	}
}

bool mci_adtc_start(sdmmc_cmd_def_t cmd, uint32_t arg, uint16_t block_size,
		uint16_t nb_block, bool access_block)
{
	uint32_t cmdr;

	if (access_block) {
		/* Enable DMA for MCI */
		if (!(block_size % 16)) {
			AVR32_MCI.dma = AVR32_MCI_DMA_DMAEN_MASK
					| (AVR32_MCI_DMA_CHKSIZE_16_BYTES
					<< AVR32_MCI_DMA_CHKSIZE_OFFSET);
		} else {
			AVR32_MCI.dma = AVR32_MCI_DMA_DMAEN_MASK
					| (AVR32_MCI_DMA_CHKSIZE_1_BYTE
					<< AVR32_MCI_DMA_CHKSIZE_OFFSET);
		}
	} else {
		/* Disable DMA for MCI */
		AVR32_MCI.dma = 0;
	}

	/* Enabling Read/Write Proof allows to stop the MCI Clock during
	 * read/write access if the internal FIFO is full.
	 * This will guarantee data integrity, not bandwidth. */
	AVR32_MCI.mr |= AVR32_MCI_MR_WRPROOF_MASK | AVR32_MCI_MR_RDPROOF_MASK;
	/* Force byte transfer if needed */
	if (block_size & 0x3) {
		AVR32_MCI.mr |= AVR32_MCI_MR_FBYTE_MASK;
	} else {
		AVR32_MCI.mr &= ~AVR32_MCI_MR_FBYTE_MASK;
	}

	if (cmd & SDMMC_CMD_WRITE) {
		cmdr = AVR32_MCI_CMDR_TRCMD_START_TRANS_MASK;
	} else {
		cmdr = AVR32_MCI_CMDR_TRCMD_START_TRANS_MASK
				| AVR32_MCI_CMDR_TRDIR_READ_MASK;
	}

	if (cmd & SDMMC_CMD_SDIO_BYTE) {
		cmdr |= AVR32_MCI_CMDR_TRTYP_SDIO_BYTE_MASK;
		AVR32_MCI.blkr = block_size << AVR32_MCI_BLKR_BCNT_OFFSET;
	} else {
		AVR32_MCI.blkr = (block_size << AVR32_MCI_BLKR_BLKLEN_OFFSET) |
				(nb_block << AVR32_MCI_BLKR_BCNT_OFFSET);
		if (cmd & SDMMC_CMD_SDIO_BLOCK) {
			cmdr |= AVR32_MCI_CMDR_TRTYP_SDIO_BLOCK_MASK;
		} else if (cmd & SDMMC_CMD_STREAM) {
			cmdr |= AVR32_MCI_CMDR_TRTYP_STREAM_MASK;
		} else if (cmd & SDMMC_CMD_SINGLE_BLOCK) {
			cmdr |= AVR32_MCI_CMDR_TRTYP_BLOCK_MASK;
		} else if (cmd & SDMMC_CMD_MULTI_BLOCK) {
			cmdr |= AVR32_MCI_CMDR_TRTYP_MULTI_BLOCK_MASK;
		} else {
			Assert(false); /* Incorrect flags */
		}
    }

	mci_transfert_pos = 0;
	mci_block_size = block_size;
	mci_nb_block = nb_block;

	return mci_send_cmd_execute(cmdr, cmd, arg);
}

bool mci_adtc_stop(sdmmc_cmd_def_t cmd, uint32_t arg)
    {
	if (!mci_send_cmd_execute(AVR32_MCI_CMDR_TRCMD_STOP_TRANS, cmd, arg)) {
		return false;
	}

	/* Workaround on AT32UC3A3 <= Rev. D
	 * See Datasheet 32072G section errata 3.8 MCI */
	return mci_wait_busy_on_line();
    }

bool mci_read_word(uint32_t *value)
{
	uint32_t sr;

	Assert((mci_block_size * mci_nb_block) > mci_transfert_pos);

	mci_transfert_pos += 4;
	/* Wait end of transfer
	 * Note: no need of timeout, because it is include in MCI */
	do {
		sr = AVR32_MCI.sr;
		if (sr & (AVR32_MCI_SR_UNRE_MASK | AVR32_MCI_SR_OVRE_MASK | \
				AVR32_MCI_SR_DTOE_MASK | AVR32_MCI_SR_DCRCE_MASK)) {
			mci_debug("%s: DMA sr 0x%08x error\n\r",
					__func__, sr);
			mci_reset();
			return false;
  }

		if ((mci_block_size * mci_nb_block) > mci_transfert_pos) {
			/* It is not the end of all transfers */
			/* but wait that RDR register is free */
			if (sr & AVR32_MCI_SR_RXRDY_MASK) {
				break;
			}
		}
	} while (!(sr & AVR32_MCI_SR_XFRDONE_MASK));
	*value = AVR32_MCI.rdr;
	*value = le32_to_cpu(*value);
	return true;
}

bool mci_write_word(uint32_t value)
{
	uint32_t sr;

	Assert((mci_block_size * mci_nb_block) > mci_transfert_pos);

	mci_transfert_pos += 4;
	/* Wait end of transfer
	 * Note: no need of timeout, because it is include in MCI */
	do {
		sr = AVR32_MCI.sr;
		if (sr & (AVR32_MCI_SR_UNRE_MASK | AVR32_MCI_SR_OVRE_MASK | \
				AVR32_MCI_SR_DTOE_MASK | AVR32_MCI_SR_DCRCE_MASK)) {
			mci_debug("%s: DMA sr 0x%08x error\n\r",
					__func__, sr);
			mci_reset();
			return false;
		}

		if ((mci_block_size * mci_nb_block) > mci_transfert_pos) {
			/* It is not the end of all transfers
			 * but wait that TDR register is free */
			if (sr & AVR32_MCI_SR_TXRDY_MASK) {
				break;
			}
		}
	} while (!(sr & AVR32_MCI_SR_NOTBUSY_MASK)
			&& !(AVR32_MCI.sr & AVR32_MCI_SR_XFRDONE_MASK));
	AVR32_MCI.tdr = cpu_to_le32(value);
	return true;
}

bool mci_start_read_blocks(void *dest, uint16_t nb_block)
{
	uint32_t nb_data;
	bool transfert_byte;

	nb_data = nb_block * mci_block_size;
	transfert_byte = ((AVR32_MCI.mr & AVR32_MCI_MR_FBYTE_MASK) != 0)
			|| (((uint32_t)dest & 0x3) != 0);

	Assert(nb_data <= ((mci_block_size * mci_nb_block) - mci_transfert_pos));
	Assert(nb_data <= (transfert_byte ?
			AVR32_DMACA_CTL1H_BLOCK_TS_MASK
			>> AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET :
			((AVR32_DMACA_CTL1H_BLOCK_TS_MASK
			>> AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) * 4)));

	Assert(!(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET)));

	/* Enable the DMACA */
	AVR32_DMACA.dmacfgreg = 1 << AVR32_DMACA_DMACFGREG_DMA_EN_OFFSET;
	/* Linked list ptrs: not used. */
	AVR32_DMACA.llp1 = 0x00000000;
	if (transfert_byte) {
		/* Channel 1 Ctrl register low */
		AVR32_DMACA.ctl1l =
		(0 << AVR32_DMACA_CTL1L_INT_EN_OFFSET)       | // Do not enable interrupts
		(0 << AVR32_DMACA_CTL1L_DST_TR_WIDTH_OFFSET) | // Dst transfer width: 8 bits
		(0 << AVR32_DMACA_CTL1L_SRC_TR_WIDTH_OFFSET) | // Src transfer width: 8 bits
		(0 << AVR32_DMACA_CTL1L_DINC_OFFSET)         | // Dst address increment: increment
		(0 << AVR32_DMACA_CTL1L_SINC_OFFSET)         | // Src address increment: increment
		(0 << AVR32_DMACA_CTL1L_DST_MSIZE_OFFSET)    | // Dst burst transaction len: 1 data items
		(0 << AVR32_DMACA_CTL1L_SRC_MSIZE_OFFSET)    | // Src burst transaction len: 1 data items
		(0 << AVR32_DMACA_CTL1L_S_GATH_EN_OFFSET)    | // Source gather: disabled
		(0 << AVR32_DMACA_CTL1L_D_SCAT_EN_OFFSET)    | // Destination scatter: disabled
		(2 << AVR32_DMACA_CTL1L_TT_FC_OFFSET)        | // transfer type:P2M, flow controller: DMACA
		(1 << AVR32_DMACA_CTL1L_DMS_OFFSET)          | // Dest master: HSB master 2
		(0 << AVR32_DMACA_CTL1L_SMS_OFFSET)          | // Source master: HSB master 1
		(0 << AVR32_DMACA_CTL1L_LLP_D_EN_OFFSET)     | // Not used
		(0 << AVR32_DMACA_CTL1L_LLP_S_EN_OFFSET);      // Not used
		/* Channel 1 Ctrl register high */
		AVR32_DMACA.ctl1h = nb_data
				<< AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET; // Block transfer size
	} else {
		AVR32_DMACA.ctl1l =
		(0 << AVR32_DMACA_CTL1L_INT_EN_OFFSET)       | // Do not enable interrupts
		(2 << AVR32_DMACA_CTL1L_DST_TR_WIDTH_OFFSET) | // Dst transfer width: 32 bits
		(2 << AVR32_DMACA_CTL1L_SRC_TR_WIDTH_OFFSET) | // Src transfer width: 32 bits
		(0 << AVR32_DMACA_CTL1L_DINC_OFFSET)         | // Dst address increment: increment
		(0 << AVR32_DMACA_CTL1L_SINC_OFFSET)         | // Src address increment: increment
		(3 << AVR32_DMACA_CTL1L_DST_MSIZE_OFFSET)    | // Dst burst transaction len: 16 data items
		(3 << AVR32_DMACA_CTL1L_SRC_MSIZE_OFFSET)    | // Src burst transaction len: 16 data items
		(0 << AVR32_DMACA_CTL1L_S_GATH_EN_OFFSET)    | // Source gather: disabled
		(0 << AVR32_DMACA_CTL1L_D_SCAT_EN_OFFSET)    | // Destination scatter: disabled
		(2 << AVR32_DMACA_CTL1L_TT_FC_OFFSET)        | // transfer type:P2M, flow controller: DMACA
		(1 << AVR32_DMACA_CTL1L_DMS_OFFSET)          | // Dest master: HSB master 2
		(0 << AVR32_DMACA_CTL1L_SMS_OFFSET)          | // Source master: HSB master 1
		(0 << AVR32_DMACA_CTL1L_LLP_D_EN_OFFSET)     | // Not used
		(0 << AVR32_DMACA_CTL1L_LLP_S_EN_OFFSET);      // Not used
		// Channel 1 Ctrl register high
		AVR32_DMACA.ctl1h = ((nb_data / 4)
				<< AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET); // Block transfer size
	}

	/* Channel 1 Config register low
	 * Destination handshaking: ignored because the dst is memory.
	 * Source handshaking: hw handshaking
	 */
	AVR32_DMACA.cfg1l = (0 << AVR32_DMACA_CFG1L_HS_SEL_DST_OFFSET) |
			(0 << AVR32_DMACA_CFG1L_HS_SEL_SRC_OFFSET);

	/* Channel 1 Config register high
	 * Dest hw handshaking itf: ignored because the dst is memory.
	 * Source hw handshaking itf:
	 */
	AVR32_DMACA.cfg1h = (0 << AVR32_DMACA_CFG1H_DEST_PER_OFFSET) |
			(AVR32_DMACA_CH_MMCI_RX << AVR32_DMACA_CFG1H_SRC_PER_OFFSET);

	/* Src Address: the MCI registers. */
	AVR32_DMACA.sar1 = (uint32_t)&AVR32_MCI.fifo;

	/* Dst Address: the OutputData[] array. */
	AVR32_DMACA.dar1 = (uint32_t)dest;

	/* Enable Channel 1 : start the process. */
	AVR32_DMACA.chenreg = ((2 << AVR32_DMACA_CHENREG_CH_EN_OFFSET)
			| (2 << AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

	mci_transfert_pos += nb_data;
	return true;
}

bool mci_wait_end_of_read_blocks(void)
{
	uint32_t sr;

	/* Wait end of transfer
	 * Note: no need of timeout, because it is included in MCI */
	do {
		sr = AVR32_MCI.sr;
		if (sr & (AVR32_MCI_SR_UNRE_MASK | AVR32_MCI_SR_OVRE_MASK | \
				AVR32_MCI_SR_DTOE_MASK | AVR32_MCI_SR_DCRCE_MASK)) {
			mci_debug("%s: DMA sr 0x%08x error\n\r",
					__func__, sr);
			/* Disable Channel 1 */
			AVR32_DMACA.chenreg = 2 << AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET;
			mci_reset();
			return false;
}

		if ((mci_block_size * mci_nb_block) > mci_transfert_pos) {
			/* It is not the end of all the transfers,
			 * so just wait the end of the DMA */
			if (!(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET))) {
				return true;
			}
		}
	} while (!(sr & AVR32_MCI_SR_XFRDONE_MASK));
	return true;
}

bool mci_start_write_blocks(const void *src, uint16_t nb_block)
{
	uint32_t nb_data;
	bool transfert_byte;

	nb_data = nb_block * mci_block_size;
	transfert_byte = ((AVR32_MCI.mr & AVR32_MCI_MR_FBYTE_MASK) != 0)
			|| (((uint32_t)src & 0x3) != 0);

	Assert(nb_data <= ((mci_block_size * mci_nb_block) - mci_transfert_pos));
	Assert(nb_data <= (transfert_byte ?
			AVR32_DMACA_CTL1H_BLOCK_TS_MASK
			>> AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET :
			((AVR32_DMACA_CTL1H_BLOCK_TS_MASK
			>> AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) * 4)));

	Assert(!(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET)));

	/* Enable the DMACA */
	AVR32_DMACA.dmacfgreg = 1 << AVR32_DMACA_DMACFGREG_DMA_EN_OFFSET;
	/* Linked list ptrs: not used. */
	AVR32_DMACA.llp1 = 0x00000000;

	if (transfert_byte) {
		/* Channel 1 Ctrl register low */
		AVR32_DMACA.ctl1l =
		(0 << AVR32_DMACA_CTL1L_INT_EN_OFFSET)       | // Do not enable interrupts
		(0 << AVR32_DMACA_CTL1L_DST_TR_WIDTH_OFFSET) | // Dst transfer width: 8 bits
		(0 << AVR32_DMACA_CTL1L_SRC_TR_WIDTH_OFFSET) | // Src transfer width: 8 bits
		(0 << AVR32_DMACA_CTL1L_DINC_OFFSET)         | // Dst address increment: increment
		(0 << AVR32_DMACA_CTL1L_SINC_OFFSET)         | // Src address increment: increment
		(0 << AVR32_DMACA_CTL1L_DST_MSIZE_OFFSET)    | // Dst burst transaction len: 1 data items
		(0 << AVR32_DMACA_CTL1L_SRC_MSIZE_OFFSET)    | // Src burst transaction len: 1 data items
		(0 << AVR32_DMACA_CTL1L_S_GATH_EN_OFFSET)    | // Source gather: disabled
		(0 << AVR32_DMACA_CTL1L_D_SCAT_EN_OFFSET)    | // Destination scatter: disabled
		(1 << AVR32_DMACA_CTL1L_TT_FC_OFFSET)        | // transfer type:M2P, flow controller: DMACA
		(0 << AVR32_DMACA_CTL1L_DMS_OFFSET)          | // Dest master: HSB master 1
		(1 << AVR32_DMACA_CTL1L_SMS_OFFSET)          | // Source master: HSB master 2
		(0 << AVR32_DMACA_CTL1L_LLP_D_EN_OFFSET)     | // Not used
		(0 << AVR32_DMACA_CTL1L_LLP_S_EN_OFFSET);      // Not used
		// Channel 1 Ctrl register high
		AVR32_DMACA.ctl1h = nb_data
				<< AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET; // Block transfer size
	} else {
		/* Channel 1 Ctrl register low */
		AVR32_DMACA.ctl1l =
		(0 << AVR32_DMACA_CTL1L_INT_EN_OFFSET)       | // Do not enable interrupts
		(2 << AVR32_DMACA_CTL1L_DST_TR_WIDTH_OFFSET) | // Dst transfer width: 32 bits
		(2 << AVR32_DMACA_CTL1L_SRC_TR_WIDTH_OFFSET) | // Src transfer width: 32 bits
		(0 << AVR32_DMACA_CTL1L_DINC_OFFSET)         | // Dst address increment: increment
		(0 << AVR32_DMACA_CTL1L_SINC_OFFSET)         | // Src address increment: increment
		(3 << AVR32_DMACA_CTL1L_DST_MSIZE_OFFSET)    | // Dst burst transaction len: 16 data items
		(3 << AVR32_DMACA_CTL1L_SRC_MSIZE_OFFSET)    | // Src burst transaction len: 16 data items
		(0 << AVR32_DMACA_CTL1L_S_GATH_EN_OFFSET)    | // Source gather: disabled
		(0 << AVR32_DMACA_CTL1L_D_SCAT_EN_OFFSET)    | // Destination scatter: disabled
		(1 << AVR32_DMACA_CTL1L_TT_FC_OFFSET)        | // transfer type:M2P, flow controller: DMACA
		(0 << AVR32_DMACA_CTL1L_DMS_OFFSET)          | // Dest master: HSB master 1
		(1 << AVR32_DMACA_CTL1L_SMS_OFFSET)          | // Source master: HSB master 2
		(0 << AVR32_DMACA_CTL1L_LLP_D_EN_OFFSET)     | // Not used
		(0 << AVR32_DMACA_CTL1L_LLP_S_EN_OFFSET);      // Not used
		// Channel 1 Ctrl register high
		AVR32_DMACA.ctl1h = ((nb_data / 4)
				<< AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET); // Block transfer size
	}

	/* Channel 1 Config register low
	 * Destination handshaking: hw handshaking
	 * Source handshaking: ignored because the dst is memory.
	 */
	AVR32_DMACA.cfg1l = (0 << AVR32_DMACA_CFG1L_HS_SEL_DST_OFFSET) |
			(0 << AVR32_DMACA_CFG1L_HS_SEL_SRC_OFFSET);

	/* Channel 1 Config register high
	 * Dest hw handshaking itf:
	 * Source hw handshaking itf: ignored because the dst is memory.
	 */
	AVR32_DMACA.cfg1h = (AVR32_DMACA_CH_MMCI_TX
			<< AVR32_DMACA_CFG1H_DEST_PER_OFFSET) |
			(0 << AVR32_DMACA_CFG1H_SRC_PER_OFFSET);

	/* Dst Address: the OutputData[] array. */
	AVR32_DMACA.sar1 = (uint32_t)src;

	/* Src Address: the MCI registers. */
	AVR32_DMACA.dar1 = (uint32_t)&AVR32_MCI.fifo;

	/* Enable Channel 1 : start the process. */
	AVR32_DMACA.chenreg = ((2 << AVR32_DMACA_CHENREG_CH_EN_OFFSET)
			| (2 << AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

	mci_transfert_pos += nb_data;
	return true;
}

bool mci_wait_end_of_write_blocks(void)
{
	uint32_t sr;

	/* Wait end of transfer
	 * Note: no need of timeout, because it is include in MCI, see DTOE bit. */
	do {
		sr = AVR32_MCI.sr;
		if (sr & (AVR32_MCI_SR_UNRE_MASK | AVR32_MCI_SR_OVRE_MASK | \
				AVR32_MCI_SR_DTOE_MASK | AVR32_MCI_SR_DCRCE_MASK)) {
			mci_debug("%s: DMA sr 0x%08x error\n\r",
					__func__, sr);
			/* Disable Channel 1 */
			AVR32_DMACA.chenreg = 2 << AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET;
			mci_reset();
			return false;
		}

		if ((mci_block_size * mci_nb_block) > mci_transfert_pos) {
			/* It is not the end of all transfers
			 * then just wait end of DMA */
			if (!(AVR32_DMACA.chenreg & (2 << AVR32_DMACA_CHENREG_CH_EN_OFFSET))) {
				return true;
			}
}
	} while (!(sr & AVR32_MCI_SR_NOTBUSY_MASK)
			&& !(sr & AVR32_MCI_SR_XFRDONE_MASK));
	Assert(AVR32_MCI.sr & AVR32_MCI_SR_FIFOEMPTY_MASK);
	Assert(!(AVR32_DMACA.chenreg & (2 << AVR32_DMACA_CHENREG_CH_EN_OFFSET)));
	return true;
}

/* ! @} */
