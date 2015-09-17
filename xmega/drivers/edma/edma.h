/**
 * \file
 *
 * \brief AVR XMEGA Enhanced Direct Memory Access Controller driver definitions
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#ifndef EDMA_H
#define EDMA_H

#include <compiler.h>
#include <interrupt.h>
#include <pmic.h>

/**
 * \defgroup edma_group Enhanced Direct Memory Access controller (EDMA)
 *
 * See \ref xmega_edma_quickstart.
 *
 * This is a driver for configuration, enabling, disabling and use of the
 * on-chip XMEGA Enhanced Direct Memory Access (EDMA) controller.
 *
 * \section dependencies Dependencies
 *
 * The enhanced direct memory access controller depends on the following
 * modules:
 *  - \ref pmic_group for setting interrupt level.
 *  - \ref sysclk_group for peripheral clock control.
 *  - \ref sleepmgr_group for setting available sleep mode.
 *
 * @{
 */

/** Number of available EDMA channels, device dependent. */
#define EDMA_MAX_NUMBER_OF_CHANNELS 4
#define EDMA_NUMBER_OF_STANDARD_CHANNELS 2
#define EDMA_NUMBER_OF_PERIPHERAL_CHANNELS 4

/**
 * \private
 * \brief Helper macro to get the channel offset given the channel number
 *
 * \param num EDMA channel number
 *
 * \return EDMA_CH_t pointer to the \a num EDMA channel register struct
 */
#define edma_get_channel_address_from_num(num) \
		((EDMA_CH_t *)((uintptr_t)(&EDMA.CH0) +	\
		(sizeof(EDMA_CH_t) * (uint8_t)num)))

/** \brief EDMA channel number */
typedef enum edma_channel_num_enum {
	/** EDMA channel 0 */
	EDMA_CH_0 = 0,
	/** EDMA channel 1 */
	EDMA_CH_1,
	/** EDMA channel 2 */
	EDMA_CH_2,
	/** EDMA channel 3 */
	EDMA_CH_3,
} edma_channel_num_t;

/* Burst mode */
typedef enum edma_ch_burstlen_enum {
	EDMA_CH_BURSTLEN_1BYTE_gc = (0x00 << 0), /* 1-byte burst mode */
	EDMA_CH_BURSTLEN_2BYTE_gc = (0x01 << 0), /* 2-byte burst mode */
} edma_ch_burstlen_t;

/** \brief EDMA channel status */
typedef enum edma_channel_status_enum {
	/** EDMA channel is idling */
	EDMA_CH_FREE = 0,
	/** EDMA channel is programmed and ready to be enabled */
	EDMA_CH_READY,
	/** EDMA channel has been enabled and has a block transfer pending */
	EDMA_CH_PENDING,
	/** EDMA channel has been enabled and is busy doing a block transfer */
	EDMA_CH_BUSY,
	/** EDMA channel has completed a block transfer */
	EDMA_CH_TRANSFER_COMPLETED,
	/** EDMA channel failed to complete a block transfer */
	EDMA_CH_TRANSFER_ERROR,
	/** Unavailable EDMA channel (i.e. CH1 in EDMA_CHMODE_STD0_gc */
	EDMA_CH_UNAVAILABLE,
} edma_channel_status_t;

/**
 * \brief EDMA channel configuration struct
 */
struct edma_channel_config {
	/** EDMA channel control register A */
	uint8_t ctrla;
	/** EDMA channel control register B */
	uint8_t ctrlb;
	/** EDMA channel memory address control register (peripheral CH) */
	/** EDMA channel source address control register ( standard  CH) */
	uint8_t addrctrl;
	/** EDMA channel destination address control register(standard CH only)
	 **/
	uint8_t destaddrctrl;
	/** EDMA channel trigger control register */
	uint8_t trigsrc;
	union {
		/** EDMA peripheral channel block transfer count register */
		uint8_t trfcnt8;
		/** EDMA standard channel block transfer count register */
		uint16_t trfcnt16;
	};
	/** EDMA channel memory 16-bit address (peripheral CH) */
	/** EDMA channel source 16-bit address ( standard  CH) */
	uint16_t addr;
	/** EDMA channel destination 16-bit address (standard CH only) */
	uint16_t destaddr;
	/** EDMA channel status */
	edma_channel_status_t ch_status;
};

/** EDMA interrupt levels */
enum edma_int_level_t {
	EDMA_INT_LVL_OFF = 0x00,
	EDMA_INT_LVL_LO  = 0x01,
	EDMA_INT_LVL_MED = 0x02,
	EDMA_INT_LVL_HI  = 0x03,
};

void edma_enable(EDMA_CHMODE_t channelmode);
void edma_disable(void);

/** \name EDMA Controller Management */
/* @{ */

/**
 * \brief Set EDMA channel priority mode
 *
 * This functions sets the channel priority mode. Users can select between
 * fixed priority, round-robin or a mix of both.
 *
 * \param primode EDMA channel priority mode given by a EDMA_PRIMODE_t type
 */
static inline void edma_set_priority_mode(EDMA_PRIMODE_t primode)
{
	irqflags_t iflags;
	iflags = cpu_irq_save();

	EDMA.CTRL = (EDMA.CTRL & ~EDMA_PRIMODE_gm) | primode;

	cpu_irq_restore(iflags);
}

/**
 * \brief Set EDMA channel double buffering mode
 *
 * This function modifies which channels should be in double buffer mode.
 *
 * \param dbufmode Double buffer channel configuration given by a
 *                 EDMA_DBUFMODE_t type
 */
static inline void edma_set_double_buffer_mode(EDMA_DBUFMODE_t dbufmode)
{
	irqflags_t iflags;
	iflags = cpu_irq_save();

	EDMA.CTRL = (EDMA.CTRL & ~EDMA_DBUFMODE_gm) | dbufmode;

	cpu_irq_restore(iflags);
}

/** @} */

/** \name EDMA Channel Management */
/** @{ */

edma_channel_status_t edma_channel_write_config(edma_channel_num_t num,
		struct edma_channel_config *config);

edma_channel_status_t edma_channel_read_config(edma_channel_num_t num,
		struct edma_channel_config *config);

/**
 * \brief Check if EDMA channel is a standard channel
 *
 * This function checks if a EDMA channel number is a standard or peripheral
 * channel regarding the EDMA controller configuration.
 *
 * \param num EDMA channel number to query
 *
 * \retval 1  EDMA channel is a standard channel
 * \retval 0  EDMA channel is a peripheral channel
 * \retval -1 EDMA channel number is unavailable
 */
static inline int8_t edma_channel_is_standard(edma_channel_num_t num)
{
	/* Unavailable channel number */
	if ((num == 1) && ((EDMA.CTRL & EDMA_CHMODE0_bm) == EDMA_CHMODE0_bm)) {
		return -1;
	}

	if ((num == 3) && ((EDMA.CTRL & EDMA_CHMODE1_bm) == EDMA_CHMODE1_bm)) {
		return -1;
	}

	/* It is a standard channel if:
	 * - EDMA_CH_2 & EDMA_CH_0 = standard channel if EDMA_CHMODE_STD02_gc
	 * - EDMA_CH_0 = standard channel if EDMA_CHMODE_STD0_gc
	 * - EDMA_CH_2 = standard channel if EDMA_CHMODE_STD2_gc
	 * Else it is a peripheral channel.
	 */
	if (((EDMA.CTRL & EDMA_CHMODE_gm) == EDMA_CHMODE_STD02_gc) &&
			((num & 1) == 0)) {
		return 1;
	} else if (((EDMA.CTRL & EDMA_CHMODE_gm) == EDMA_CHMODE_STD0_gc) &&
				(num == EDMA_CH_0)) {
		return 1;
	} else if (((EDMA.CTRL & EDMA_CHMODE_gm) == EDMA_CHMODE_STD2_gc) &&
				(num == EDMA_CH_2)) {
		return 1;
	}

	return 0;
}

/**
 * \brief Enable a EDMA channel
 *
 * This function enables an EDMA channel, depending on the configuration the
 * EDMA channel will start the block transfer upon a
 * \ref edma_channel_trigger_block_transfer() or when other hardware modules
 * trigger the EDMA hardware.
 *
 * \pre The EDMA channel configuration must be written to the channel before
 * enabling the channel.
 *
 * \param num EDMA channel to enable
 */
static inline void edma_channel_enable(edma_channel_num_t num)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA |= EDMA_CH_ENABLE_bm;

	cpu_irq_restore(iflags);
}

/**
 * \brief Disable a EDMA channel
 *
 * This function disables a EDMA channel. If the EDMA channel was already
 * enabled the EDMA channel is not disabled before the internal transfer buffer
 * is empty and the EDMA transfer is aborted.
 *
 * \param num EDMA channel to disable
 */
static inline void edma_channel_disable(edma_channel_num_t num)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA &= ~EDMA_CH_ENABLE_bm;

	cpu_irq_restore(iflags);
}

/**
 * \brief Check if EDMA channel is enabled
 *
 * This function checks if a EDMA channel is enabled.
 *
 * \param num EDMA channel number to query
 *
 * \retval  true EDMA channel is enabled
 * \retval false EDMA channel is disabled
 */
static inline bool edma_channel_is_enabled(edma_channel_num_t num)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	bool channel_enabled = (channel->CTRLA & EDMA_CH_ENABLE_bm)
			>> EDMA_CH_ENABLE_bp;

	return channel_enabled;
}

/**
 * \brief Check if EDMA channel is busy
 *
 * This function checks if a EDMA channel is busy doing or going to do (pending)
 * a block transfer.
 *
 * \param num EDMA channel number to query
 *
 * \retval true EDMA channel is busy or have a block transfer pending
 * \retval false EDMA channel is not busy or have a block transfer pending
 */
static inline bool edma_channel_is_busy(edma_channel_num_t num)
{
	uint8_t busy_pending = EDMA.STATUS;

	busy_pending &= (1 << num) | (1 << (num + 4));
	if (busy_pending) {
		return true;
	}

	return false;
}

/**
 * \brief Get channel status
 *
 * This function returns the current channel status.
 *
 * \param num EDMA channel number
 *
 * \return Channel status given by a \ref edma_channel_status
 */
static inline edma_channel_status_t edma_get_channel_status(
		edma_channel_num_t num)
{
	/*
	 * Check the EDMA configuration to return channel availability. This
	 * does not check the channel type.
	 */
	if ((num == 1) && ((EDMA.CTRL & EDMA_CHMODE0_bm) == EDMA_CHMODE0_bm)) {
		return EDMA_CH_UNAVAILABLE;
	}

	if ((num == 3) && ((EDMA.CTRL & EDMA_CHMODE1_bm) == EDMA_CHMODE1_bm)) {
		return EDMA_CH_UNAVAILABLE;
	}

	uint8_t busy_pending    = EDMA.STATUS;
	uint8_t error_completed = EDMA.INTFLAGS;

	/*
	 * Check lower and upper nibble of INTFLAGS register to find possible
	 * error or transfer completed status.
	 */
	error_completed &= (1 << num) | (1 << (num + 4));
	if (error_completed & (1 << (num + 4))) {
		return EDMA_CH_TRANSFER_ERROR;
	} else if (error_completed & (1 << num)) {
		return EDMA_CH_TRANSFER_COMPLETED;
	}

	/*
	 * Check lower and upper nibble of STATUS register to find possible
	 * busy or pending completed status.
	 */
	busy_pending &= (1 << num) | (1 << (num + 4));
	if (busy_pending & (1 << (num + 4))) {
		return EDMA_CH_BUSY;
	} else if (busy_pending & (1 << num)) {
		return EDMA_CH_PENDING;
	}

	return EDMA_CH_FREE;
}

/**
 * \brief EDMA channel get search pointer
 *
 * This function reads the EDMA channel to get the pointer value after a search
 * operation. The function result will be the pointer on where the data has
 * matched.
 *
 * \pre The EDMA channel must be configured and enabled for this to have any
 * affect.
 *
 * \param num EDMA channel number on whitch the search operation has been done.
 *
 * \return uint8_t pointer to result of search operation
 */
static inline uint8_t *edma_channel_get_search_pointer(edma_channel_num_t num)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	uint16_t search_add = (uint16_t)(channel->ADDRL);
	search_add |= (((uint16_t)(channel->ADDRH)) << 8);

	return (uint8_t *)(search_add - 1);
}

/**
 * \brief EDMA channel trigger block transfer
 *
 * This function triggers a start of a block transfer on a given EDMA channel
 * (software trigger).
 *
 * \pre The EDMA channel must be configured and enabled for this to have any
 * affect.
 *
 * \param num EDMA channel number to trigger block transfer for
 */
static inline void edma_channel_trigger_block_transfer(edma_channel_num_t num)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA  |= EDMA_CH_TRFREQ_bm;

	cpu_irq_restore(iflags);
}

/**
 * \brief Reset a EDMA channel
 *
 * This function resets a given EDMA channel.
 *
 * \param num EDMA channel number to trigger block transfer for
 */
static inline void edma_channel_reset(edma_channel_num_t num)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA  |= EDMA_CH_RESET_bm;

	cpu_irq_restore(iflags);
}

/**
 * \brief Callback definition for EDMA channel interrupts
 *
 * \param status Status of EDMA channel block transfer
 */
typedef void (*edma_callback_t)(edma_channel_status_t status);

void edma_set_callback(edma_channel_num_t num, edma_callback_t callback);

/** @} */

/**
 * \name EDMA Channel Direct Configuration Functions
 *
 * These functions allows direct configuration on the EDMA channels, not going
 * through a \ref edma_channel_config struct. This allows update of the most
 * commonly changed EDMA channel properties.
 *
 * @{
 */

/**
 * \brief Write EDMA channel burst length to hardware
 *
 * This function writes the EDMA channel burst length directly to hardware. The
 * burst length is defined in the form of \a EDMA_CH_BURSTLEN_*_gc, where *
 * represents the available burst length.
 *
 * \param num EDMA channel number to write burst length for
 * \param burst_length EDMA channel burst length given by
 *                     a edma_ch_burstlen_t type
 */
static inline void edma_channel_write_burst_length(edma_channel_num_t num,
		edma_ch_burstlen_t burst_length)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA &= ~EDMA_CH_BURSTLEN_bm;
	channel->CTRLA |= burst_length;

	cpu_irq_restore(iflags);
}

/**
 * \brief Write EDMA standard channel transfer 8-bit count to hardware
 *
 * This function writes the EDMA peripheral channel number of bytes in each
 * block transfer to hardware.
 *
 * \param num    EDMA peripheral channel number to write transfer count for
 * \param count8 8-bit number of bytes in each block transfer
 */
static inline void edma_channel_write_transfer_count8(edma_channel_num_t num,
		uint8_t count8)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->TRFCNTL = count8;

	cpu_irq_restore(iflags);
}

/**
 * \brief Write EDMA standard channel transfer 16-bit count to hardware
 *
 * This function writes the EDMA standard channel number of bytes in each block
 * transfer to hardware.
 *
 * \param num     EDMA standard channel number to write transfer count for
 * \param count16 16-bit number of bytes in each block transfer
 */
static inline void edma_channel_write_transfer_count16(edma_channel_num_t num,
		uint16_t count16)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->TRFCNTL = (uint8_t)count16;
	channel->TRFCNTH = (uint8_t)(count16 >> 8);

	cpu_irq_restore(iflags);
}

/**
 * \brief Write EDMA channel transfer repeat mode to hardware
 *
 * This function writes the EDMA channel transfer repeat mode to hardware.
 *
 * \param num    EDMA channel number to write transfer repeat for
 * \param repeat Transfer repeat mode
 */
static inline void edma_channel_write_repeat(edma_channel_num_t num,
		bool repeat)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA &= ~EDMA_CH_REPEAT_bm;
	channel->CTRLA |=  ((uint8_t)repeat) << EDMA_CH_REPEAT_bp;

	cpu_irq_restore(iflags);
}

/**
 * \brief Write EDMA peripheral channel 16-bit memory address to hardware
 *
 * This function writes a EDMA peripheral channel memory 16-bit address to
 * hardware.
 *
 * \param num EDMA peripheral channel number to write 16-bit memory address for
 * \param memory_add 16-bit memory address
 */
static inline void edma_channel_write_memory_address(edma_channel_num_t num,
		uint16_t memory_add)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->ADDRL = (uint8_t)memory_add;
	channel->ADDRH = (uint8_t)(memory_add >> 8);

	cpu_irq_restore(iflags);
}

/**
 * \brief Write EDMA standard channel 16-bit source address to hardware
 *
 * This function writes a EDMA standard channel source 16-bit address to
 * hardware.
 *
 * \param num EDMA standard channel number to write 16-bit source address for
 * \param source 16-bit source address
 *
 * static inline void edma_channel_write_src_address(edma_channel_num_t num,
 *          uint16_t source)
 */
#define edma_channel_write_src_address edma_channel_write_memory_address

/**
 * \brief Write EDMA standard channel 16-bit destination address to hardware.
 *
 * This function writes a EDMA standard channel destination 16-bit address
 * to hardware.
 *
 * \param num EDMA standard channel number to write 16-bit destination add. for
 * \param destination 16-bit destination address
 */
static inline void edma_channel_write_dest_address(edma_channel_num_t num,
		uint16_t destination)
{
	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);

	irqflags_t iflags = cpu_irq_save();

	channel->DESTADDRL = (uint8_t)destination;
	channel->DESTADDRH = (uint8_t)(destination >> 8);

	cpu_irq_restore(iflags);
}

/** @} */

/** \name EDMA Channel Configuration Helper Functions */
/** @{ */

/**
 * \brief Set EDMA channel burst length in "configuration" variable
 *
 * This function helps the caller setting the EDMA channel burst length. The
 * burst length is defined in the form of \a EDMA_CH_BURSTLEN_*_gc, where *
 * represents the available burst length.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param burst_length EDMA channel burst length given by
 *                     a edma_ch_burstlen_t type
 */
static inline void edma_channel_set_burst_length(
		struct edma_channel_config *config, edma_ch_burstlen_t burst_length)
{
	config->ctrla &= ~EDMA_CH_BURSTLEN_bm;
	config->ctrla |= burst_length;
}

/**
 * \brief Set EDMA channel in single shot transfer mode in "configuration"
 * variable
 *
 * This function helps the caller setting the EDMA channel in single shot
 * transfer mode.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 */
static inline void edma_channel_set_single_shot(
		struct edma_channel_config *config)
{
	config->ctrla |= EDMA_CH_SINGLE_bm;
}

/**
 * \brief Unset EDMA channel from single shot transfer mode in "configuration"
 * variable
 *
 * This function helps the caller clear the EDMA channel single shot transfer
 * mode feature.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 */
static inline void edma_channel_unset_single_shot(
		struct edma_channel_config *config)
{
	config->ctrla &= ~EDMA_CH_SINGLE_bm;
}

/**
 * \brief Set EDMA channel interrupt level in "configuration" variable
 *
 * This function helps the caller setting the EDMA channel interrupt level for
 * transaction complete and channel error (same level for both).
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param level Interrupt level given by a \ref edma_int_level_t type
 */
static inline void edma_channel_set_interrupt_level(struct edma_channel_config
		*config, enum edma_int_level_t level)
{
	config->ctrlb &= ~(EDMA_CH_ERRINTLVL_gm | EDMA_CH_TRNINTLVL_gm);
	config->ctrlb |= (level << EDMA_CH_ERRINTLVL_gp) |
			(level << EDMA_CH_TRNINTLVL_gp);
}

/**
 * \brief Set EDMA peripheral channel memory address reload mode in
 * "configuration" variable
 *
 * This function helps the caller setting the EDMA peripheral channel memory
 * address reload mode.
 * The reload modes are defined in the toolchain header files in the form
 * of \a EDMA_CH_RELOAD_*_gc, where * is NONE, BLOCK, BURST, TRANSACTION.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA peripheral channel memory address reload mode given by an
 *             EDMA_CH_RELOAD_t type
 */
static inline void edma_channel_set_memory_reload_mode(
		struct edma_channel_config *config, EDMA_CH_RELOAD_t mode)
{
	config->addrctrl &= ~EDMA_CH_RELOAD_gm;
	config->addrctrl |=  mode;
}

/**
 * \brief Set EDMA standard channel source address reload mode in
 * "configuration" variable
 *
 * This function helps the caller setting the EDMA standard channel source
 * address reload mode.
 * The reload modes are defined in the toolchain header files in the form
 * of \a EDMA_CH_RELOAD_*_gc, where * is NONE, BLOCK, BURST, TRANSACTION.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA standard channel source address reload mode given by an
 *             EDMA_CH_RELOAD_t type
 *
 * static inline void edma_channel_set_src_reload_mode(
 *          struct edma_channel_config *config, EDMA_CH_RELOAD_t mode)
 */
#define edma_channel_set_src_reload_mode edma_channel_set_memory_reload_mode

/**
 * \brief Set EDMA standard channel destination address reload mode in
 * "configuration" variable
 *
 * This function helps the caller setting the EDMA standard channel destination
 * address reload mode. The reload modes are defined in the toolchain header
 * files in the form of \a EDMA_CH_RELOAD_*_gc, where * is NONE, BLOCK, BURST,
 * TRANSACTION.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA echannel destination address reload mode given by an
 *             EDMA_CH_RELOAD_t type
 */
static inline void edma_channel_set_dest_reload_mode(
		struct edma_channel_config *config, EDMA_CH_RELOAD_t mode)
{
	config->destaddrctrl &= ~EDMA_CH_RELOAD_gm;
	config->destaddrctrl |=  mode;
}

/**
 * \brief Set EDMA peripheral channel memory addressing mode in "configuration"
 * variable
 *
 * This function helps the caller setting the EDMA peripheral channel memory
 * addressing mode. The addressing modes are defined in the toolchain header
 * files in the form of \a EDMA_CH_DIR_*_gc, where * is FIXED, INC, MP1
 * (1-byte mask-match), MP2(1-byte OR-match) or MP3(2-byte match).
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA peripheral channel memory addressing mode given by an
 *             EDMA_CH_DIR_t type
 */
static inline void edma_channel_set_memory_dir_mode(
		struct edma_channel_config *config, EDMA_CH_DIR_t mode)
{
	config->addrctrl &= ~EDMA_CH_DIR_gm;
	config->addrctrl |= mode;
}

/**
 * \brief Set EDMA standard channel source addressing mode in "configuration"
 * variable
 *
 * This function helps the caller setting the EDMA standard channel source
 * addressing mode. The addressing modes are defined in the toolchain header
 * files in the form of \a EDMA_CH_DIR_*_gc, where * is FIXED or INC.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA standard channel source addressing mode given by an
 *             EDMA_CH_DIR_t type
 *
 * static inline void edma_channel_set_src_dir_mode(
 *         struct edma_channel_config *config, EDMA_CH_DIR_t mode)
 */
#define edma_channel_set_src_dir_mode edma_channel_set_memory_dir_mode

/**
 * \brief Set EDMA peripheral channel match mode in  "configuration" variable.
 *
 * This function helps the caller setting the EDMA peripheral channel match
 *mode.
 *
 * \note In case of match mode, only \a DIR[1:0] is available and the other
 *       fields of the register are not used.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA peripheral channel match mode given by an EDMA_CH_DIR_t type
 *
 * static inline void edma_channel_set_match_mode(
 *        struct edma_channel_config *config, EDMA_CH_DIR_t mode)
 */
#define edma_channel_set_match_mode edma_channel_set_memory_dir_mode

/**
 * \brief Set EDMA standard channel destination addressing mode in
 * "configuration" variable
 *
 * This function helps the caller setting the EDMA standard channel destination
 * addressing mode. The addressing modes are defined in the toolchain header
 * files in the form of \a EDMA_CH_DESTDIR_*_gc, where * is FIXED, INC, MP1
 * (1-byte mask-match), MP2(1-byte OR-match) or MP3(2-byte match).
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA echannel destination addressing mode given by an
 *             EDMA_CH_DESTDIR_t type
 */
static inline void edma_channel_set_dest_dir_mode(
		struct edma_channel_config *config, EDMA_CH_DESTDIR_t mode)
{
	config->destaddrctrl &= ~EDMA_CH_DESTDIR_gm;
	config->destaddrctrl |=  mode;
}

/**
 * \brief Set EDMA standard channel search mode in  "configuration" variable.
 *
 * This function helps the caller setting the EDMA standard channel search mode.
 *
 * \note In case of search mode, only \a DESTDIR[1:0] is available
 *       and the other fields of the register are not used.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param mode EDMA echannel search mode given by an EDMA_CH_DESTDIR_t type
 *
 * static inline void edma_channel_set_search_mode(
 *       struct edma_channel_config *config, EDMA_CH_DESTDIR_t mode)
 */
#define edma_channel_set_search_mode edma_channel_set_dest_dir_mode

/**
 * \brief Set EDMA channel trigger source  in "configuration" variable
 *
 * This function helps the caller setting the EDMA channel trigger source. The
 * trigger sources are defined in the toolchain header files in the form of \a
 * EDMA_CH_TRIGSRC_*_gc, where * represents the various sources.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param source EDMA channel trigger source given by a EDMA_CH_TRIGSRC_t type
 */
static inline void edma_channel_set_trigger_source(
		struct edma_channel_config *config, EDMA_CH_TRIGSRC_t source)
{
	config->trigsrc = source;
}

/**
 * \brief Set EDMA peripheral channel transfer count in "configuration" variable
 *
 * This function helps the caller setting the EDMA peripheral channel number of
 * bytes in each block transfer (1 up to 256 bytes).
 * \note 256 transfers are coded 0x00.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param count8 Number of bytes in each block transfer
 */
static inline void edma_channel_set_transfer_count8(
		struct edma_channel_config *config, uint8_t count8)
{
	config->trfcnt8 = count8;
}

/**
 * \brief Set EDMA standard channel transfer count in "configuration" variable
 *
 * This function helps the caller setting the EDMA standard channel number of
 * bytes in each block transfer (1 up to 65536 bytes).
 * \note 65536 transfers are coded 0x0000.
 *
 * \param config  Pointer to a \ref edma_channel_config variable
 * \param count16 Number of bytes in each block transfer
 */
static inline void edma_channel_set_transfer_count16(
		struct edma_channel_config *config, uint16_t count16)
{
	config->trfcnt16 = count16;
}

/**
 * \brief Set EDMA channel transfer repeat mode in "configuration" variable
 *
 * This function helps the caller setting the EDMA channel transfer repeat mode.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 */
static inline void edma_channel_set_repeat(struct edma_channel_config *config)
{
	config->ctrla |= EDMA_CH_REPEAT_bm;
}

/**
 * \brief Unset EDMA channel transfer repeat mode in "configuration" variable
 *
 * This function helps the caller clear the EDMA channel transfer repeat mode.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 */
static inline void edma_channel_unset_repeat(struct edma_channel_config *config)
{
	config->ctrla &= ~EDMA_CH_REPEAT_bm;
}

/**
 * \brief Set EDMA peripheral channel 16-bit memory address in "configuration"
 * variable.
 *
 * This function helps the caller setting the EDMA peripheral channel memeory
 *  16-bit address.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param source 16-bit memory address
 */
static inline void edma_channel_set_memory_address(
		struct edma_channel_config *config, uint16_t source)
{
	config->addr = source;
}

/**
 * \brief Set EDMA standard channel 16-bit source address in "configuration"
 * variable.
 *
 * This function helps the caller setting the EDMA standard channel source
 *  16-bit address.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param source 16-bit source address
 * static inline void edma_channel_set_source_address(
 *       struct edma_channel_config *config, uint16_t source)
 */
 #define edma_channel_set_source_address edma_channel_set_memory_address

/**
 * \brief Set EDMA peripheral channel match data in  "configuration" variable.
 *
 * This function helps the caller setting the EDMA peripheral channel match
 *data.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param data_1 data  for "Mask-Match" mode or
 *               data1 for "OR-Match" and "2-bytes-Search" modes
 * \param data_2 mask  for "Mask-Match" mode (active bit-mask=1)or
 *               data2 for "OR-Match" and "2-bytes-Search" modes
 */
static inline void edma_channel_set_match_data(
		struct edma_channel_config *config, uint8_t data_1, uint8_t data_2)
{
	config->addr = (((uint16_t)data_2) << 8) | (uint16_t)data_1;
}

/**
 * \brief Set EDMA standard channel 16-bit destination address in
 * "configuration" variable.
 *
 * This function helps the caller setting the EDMA standard channel destination
 * 16-bit address.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param destination 16-bit destination address
 */
static inline void edma_channel_set_destination_address(
		struct edma_channel_config *config, uint16_t destination)
{
	config->destaddr = destination;
}

/**
 * \brief Set EDMA standard channel search data in  "configuration" variable.
 *
 * This function helps the caller setting the EDMA standard channel search data.
 *
 * \param config Pointer to a \ref edma_channel_config variable
 * \param data_1 data for "Mask-Search" mode or
 *               data1 for "OR-Search" and "2-bytes-Search" modes
 * \param data_2 mask for "Mask-Search" mode (active bit-mask=1) or
 *               data2 for "OR-Search" and "2-bytes-Search" modes
 */
static inline void edma_channel_set_search_data(
		struct edma_channel_config *config, uint8_t data_1, uint8_t data_2)
{
	config->destaddr = (((uint16_t)data_2) << 8) | (uint16_t)data_1;
}

/** @} */

/** @} */

/**
 * \page xmega_edma_quickstart Quick start guide for the XMEGA EDMA driver
 *
 * This is the quick start guide for the \ref edma_group "EDMA driver", with
 * step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section edma_use_cases Advanced use cases
 * For more advanced use of the EDMA driver, see the following use cases:
 * - \subpage edma_use_case_1
 * - \subpage edma_use_case_2
 *
 * \section edma_basic_use_case Basic use case
 * The basic use case is a memory-to-memory transfer.
 * The EDMA is configured for:
 * - Burst length: 1 byte
 * - Transfer count: 100
 * - Source: Buffer located in SRAM
 * - Destination: Buffer located in SRAM
 * - Source and destination address reload mode: End of transaction
 * - Source and destination address direction mode: Increment
 *
 * In this use case data is copied from the source buffer to the destination
 * buffer in 1-byte bursts, until all data in the block is transferred. This
 * example is analogus to a <code>memcpy(destination, source,
 * sizeof(source))</code> operation performed in hardware asynchronously to the
 * CPU.
 *
 * \section edma_basic_use_case_setup Setup steps
 *
 * \subsection edma_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref sysclk_group "System Clock Manager Service (sysclk)"
 *
 * \subsection edma_basic_use_case_setup_setup_code Example code
 * Add to application C-file:
 * \code
	#define EDMA_CHMODE      EDMA_CHMODE_STD02_gc
	#define EDMA_CHANNEL     EDMA_CH_0
	#define EDMA_BUFFER_SIZE 100

	static uint8_t source[EDMA_BUFFER_SIZE];
	static uint8_t destination[EDMA_BUFFER_SIZE];

	static void edma_init(void) {

	    struct edma_channel_config my_ch_conf;
	    memset(&my_ch_conf, 0, sizeof(my_ch_conf));

	    edma_channel_set_burst_length(&my_ch_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	    edma_channel_set_transfer_count16(&my_ch_conf, EDMA_BUFFER_SIZE);

	    edma_channel_set_src_reload_mode(&my_ch_conf,
	              EDMA_CH_RELOAD_TRANSACTION_gc);
	    edma_channel_set_dest_reload_mode(&my_ch_conf,
	              EDMA_CH_RELOAD_TRANSACTION_gc);

	    edma_channel_set_src_dir_mode(&my_ch_conf, EDMA_CH_DIR_INC_gc);
	    edma_channel_set_dest_dir_mode(&my_ch_conf, EDMA_CH_DESTDIR_INC_gc);

	    edma_channel_set_source_address(&my_ch_conf,
	              (uint16_t)source);
	    edma_channel_set_destination_address(&my_ch_conf,
	              (uint16_t)destination);

	    edma_channel_set_trigger_source(&my_usart_ch_conf,
	              EDMA_CH_TRIGSRC_OFF_gc);

	    edma_enable(EDMA_CHMODE);

	    edma_channel_write_config(EDMA_CHANNEL, &my_ch_conf);
	    edma_channel_enable(EDMA_CHANNEL);
	}
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	edma_init();
\endcode
 *
 * \subsection edma_basic_use_case_setup_flow Workflow
 * -# Define the EDMA channel mode that will be set:
 *   - \code #define EDMA_CHMODE EDMA_CHMODE_STD02_gc \endcode
 * -# Define the EDMA standard channel that will be used for the transfer:
 *   - \code #define EDMA_CHANNEL EDMA_CH_0 \endcode
 * -# Define the array length that will be the used for the source and
 *    destination buffers located in SRAM:
 *   - \code #define EDMA_BUFFER_SIZE 100 \endcode
 * -# Create a pair of global arrays that will hold the source and destination
 *    data copied by the EDMA controller channel when it is triggered:
 *   - \code
	static uint8_t source[EDMA_BUFFER_SIZE];
	static uint8_t destination[EDMA_BUFFER_SIZE];
\endcode
 * -# Create a function \c edma_init() to intialize the EDMA:
 *     - \code
	static void edma_init(void) {
	    // ...
	}
\endcode
 * -# Create config struct for DEMA channel:
 *   - \code struct edma_channel_config my_ch_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&my_ch_conf, 0, sizeof(my_ch_conf)); \endcode
 * -# Configure the EDMA channel for single byte bursts, with a transfer length
 *    equal to the size of the source and destination buffers:
 *   - \code
	edma_channel_set_burst_length(&my_ch_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_transfer_count16(&my_ch_conf, EDMA_BUFFER_SIZE);
\endcode
 * -# Configure the EDMA channel to reset the source and destination addresses
 *    at the end of the complete transaction (i.e. after \c EDMA_BUFFER_SIZE
 *    bytes copied):
 *   - \code
	edma_channel_set_src_reload_mode(&my_ch_conf, EDMA_CH_RELOAD_TRANSACTION_gc);
	edma_channel_set_dest_reload_mode(&my_ch_conf,
	        EDMA_CH_RELOAD_TRANSACTION_gc);
\endcode
 * -# Configure the EDMA channel to increment the source and destination
 *    addresses after each byte transferred:
 *   - \code
	edma_channel_set_src_dir_mode(&my_ch_conf, EDMA_CH_DIR_INC_gc);
	edma_channel_set_dest_dir_mode(&my_ch_conf, EDMA_CH_DESTDIR_INC_gc);
\endcode
 * -# Configure the EDMA channel source and destination addresses:
 *   - \code
	edma_channel_set_source_address(&my_ch_conf, (uint16_t)source);
	edma_channel_set_destination_address(&my_ch_conf, (uint16_t)destination);
\endcode
 * -# Set software command as trigger for the EDMA channel:
 *   - \code
	edma_channel_set_trigger_source(&my_usart_ch_conf, EDMA_CH_TRIGSRC_OFF_gc);
\endcode
 * -# Enable the EDMA module configuring all channels in it. In this case, the
 *    EDMA will be set having standard channels, EDMA_CH_0 and EDMA_CH_2. This
 *    corresponds to <code>EDMA_CHMODE_STD02_gc</code> configuration:
 *      - \code edma_enable(EDMA_CHMODE); \endcode
 *  \attention Calling edma_enable() will result in a soft-reset of the entire
 *             EDMA module, clearing all channel configurations. If more than
 *             one EDMA channel is to be configured, this function should be
 *             called only once in the application initialization procedure
 *             only.
 * -# Write the EDMA channel configuration to the EDMA hardware and enable it so
 *    that it can be triggered to start the transfer:
 *   - \code
	edma_channel_write_config(EDMA_CHANNEL, &my_ch_conf);
	edma_channel_enable(EDMA_CHANNEL);
\endcode
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 * -# Call our EDMA init function:
 *      - \code edma_init(); \endcode
 *
 * \section edma_basic_use_case_usage Usage steps
 *
 * \subsection edma_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	edma_channel_trigger_block_transfer(EDMA_CHANNEL);
	do {} while (edma_get_channel_status(EDMA_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 *
 * \subsection edma_basic_use_case_usage_flow Workflow
 * -# Software command to start the EDMA transfer:
 *   - \code edma_channel_trigger_block_transfer(EDMA_CHANNEL); \endcode
 * -# Wait for the transfer to complete:
 *   - \code
	do {} while (edma_get_channel_status(EDMA_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 */

/**
 * \page edma_use_case_1 Use case #1 - EDMA transfer from memory to peripheral
 * using a peripheral channel.
 *
 * This use case shows how to set up a burst EDMA transfer between the main
 * system memory and a peripheral using a peripheral channel.
 *
 * In this use case, the EDMA is configured for:
 * - Burst length: 1 byte
 * - Transfer count: Depends on the source buffer length
 * - Source: Buffer located in RAM
 * - Destination: USARTC0 in RS232 mode
 * - Source address reload mode: End of transaction
 * - Source address direction mode: Increment
 * - Transfer trigger: USART data register empty (DRE)
 *
 * In this use case data is copied from a buffer in RAM to USART, each time the
 * USART data register is empty.
 * This example is analogus to <code>usart_serial_write_packet()</code>
 * function of "serial" service.
 *
 * \section edma_use_case_1_setup Setup steps
 *
 * \subsection edma_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref sysclk_group "System Clock Manager Service (sysclk)"
 * -# \ref usart_group "USART Driver"
 *
 * The USART must be configured according to the XMEGA USART driver Quick Start
 * \ref xmega_usart_quickstart "Basic use case".
 *
 * \subsection edma_use_case_1_setup_code Example code
 * Add to application C-file:
 * \code
	#define EDMA_CHMODE      EDMA_CHMODE_STD0_gc
	#define EDMA_CHANNEL     3

	static uint8_t usart_string[] = "XMEGAE5 EDMA-USART print test.\r\n";

	static void edma_init(void)
	{
	    struct edma_channel_config my_ch_conf;
	    memset(&my_ch_conf, 0, sizeof(my_ch_conf));
	    uint8_t string_size = sizeof(usart_string);

	    edma_channel_set_burst_length(&my_ch_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	    edma_channel_set_transfer_count8(&my_ch_conf, string_size);

	    edma_channel_set_memory_reload_mode(&my_ch_conf,
	              EDMA_CH_RELOAD_TRANSACTION_gc);

	    edma_channel_set_memory_dir_mode(&my_ch_conf, EDMA_CH_DIR_INC_gc);

	    edma_channel_set_memory_address(&my_ch_conf,
	          (uint16_t)usart_string);

	    edma_channel_set_trigger_source(&my_ch_conf,
	              EDMA_CH_TRIGSRC_USARTC0_DRE_gc);

	    edma_channel_set_single_shot(&my_ch_conf);

	    edma_enable(EDMA_CHMODE);

	    edma_channel_write_config(EDMA_CHANNEL, &my_ch_conf);
	    edma_channel_enable(EDMA_CHANNEL);
	}
\endcode
 *
 * Add to \c main():
 * \code
	#define USART    &USARTC0
	static usart_rs232_options_t USART_OPTIONS = {
	     .baudrate   = 19200,
	     .charlength = USART_CHSIZE_8BIT_gc,
	     .paritytype = USART_PMODE_DISABLED_gc,
	     .stopbits   = false
	};
	sysclk_init();
	usart_init_rs232(USART, &USART_OPTIONS);
	edma_init();
\endcode
 *
 *
 * \subsection edma_use_case_1_setup_flow Workflow
 * -# Define the EDMA channel mode that will be set. In this case, the EDMA will
 *    be set having 1 standard channel, EDMA_CH_0 and 2 peripheral channels,
 *    EDMA_CH_2 and EDMA_CH_3:
 *   - \code #define EDMA_CHMODE EDMA_CHMODE_STD0_gc \endcode
 * -# Define the EDMA peripheral channel that will be used for the USART
 *    transfer for convenience:
 *   - \code #define EDMA_CHANNEL 3 \endcode
 * -# Create a global array that will hold the text to print:
 *   - \code
	static uint8_t usart_string[] = "XMEGAE5 EDMA-USART print test.\r\n";
\endcode
 * -# Create a function \c edma_init() to intialize the EDMA:
 *     - \code
	static void edma_init(void)
	{
	    // ...
	}
\endcode
 * -# Create config struct for EDMA channel:
 *   - \code struct edma_channel_config my_ch_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&my_ch_conf, 0, sizeof(my_ch_conf)); \endcode
 * -# Get the array length that will be the used for the memory buffer located
 *    in RAM:
 *   - \code uint8_t string_size = sizeof(usart_string); \endcode
 * -# Configure the EDMA channel for single byte bursts with a transfer length
 *    equal to the size of the memory buffer <code>usart_string[]</code>.
 *   - \code
	edma_channel_set_burst_length(&my_ch_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_transfer_count8(&my_ch_conf, string_size);
\endcode
 * -# Configure the EDMA channel to reset the memory address at the end of the
 *    complete transaction (i.e. after \c string_size bytes copied):
 *   - \code
	edma_channel_set_memory_reload_mode(&my_ch_conf,
	        EDMA_CH_RELOAD_TRANSACTION_gc);
\endcode
 * -# Configure the EDMA channel to increment the memory address after each byte
 *    transferred:
 *   - \code
	edma_channel_set_memory_dir_mode(&my_ch_conf, EDMA_CH_DIR_INC_gc);
\endcode
 * -# Configure the EDMA channel memory address:
 *   - \code
	edma_channel_set_memory_address(&my_ch_conf, (uint16_t)usart_string);
\endcode
 * -# Set USART C0 data register empty as trigger for the EDMA channel:
 *   - \code
	edma_channel_set_trigger_source(&my_ch_conf, EDMA_CH_TRIGSRC_USARTC0_DRE_gc);
\endcode
 * -# Configure the EDMA channel in single shot mode, so that each time it is
 *    triggered it will perform one bust transfer only:
 *   - \code edma_channel_set_single_shot(&my_ch_conf); \endcode
 * -# Enable the EDMA module configuring all channels in it. In this case, the
 *    EDMA will be set having 1 standard channel, EDMA_CH_0 and 2 peripheral
 *    channels, EDMA_CH_2 and EDMA_CH_3. This corresponds to
 *    <code>EDMA_CHMODE_STD0_gc</code> configuration:
 *      - \code edma_enable(EDMA_CHMODE); \endcode
 *  \attention Calling edma_enable() will result in a soft-reset of the entire
 *             EDMA module, clearing all channel configurations. If more than
 *             one EDMA channel is to be configured, this function should be
 *             called only once in the application initialization procedure
 *             only.
 * -# Write the EDMA channel configuration to the EDMA hardware and enable it so
 *    that it can be triggered to start the transfer:
 *   - \code
	edma_channel_write_config(EDMA_CHANNEL, &my_ch_conf);
	edma_channel_enable(EDMA_CHANNEL);
\endcode
 * -# Define what USART will be used:
 *   - \code #define USART &USARTC0 \endcode
 * -# Define and fill the USART structure:
 *   - \code
	static usart_rs232_options_t USART_OPTIONS = {
	     .baudrate   = 19200,
	     .charlength = USART_CHSIZE_8BIT_gc,
	     .paritytype = USART_PMODE_DISABLED_gc,
	     .stopbits   = false
	};
\endcode
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 * -# Initialize the USART:
 *      - \code usart_init_rs232(USART, &USART_OPTIONS); \endcode
 * -# Call our EDMA init function:
 *      - \code edma_init(); \endcode
 *
 * \section edma_use_case_1_usage Usage steps
 *
 * \subsection edma_use_case_1_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	do {} while (edma_get_channel_status(EDMA_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 *
 * \subsection edma_use_case_1_usage_flow Workflow
 * -# Wait for the transfer to complete:
 *   - \code
	do {} while (edma_get_channel_status(EDMA_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 */

/**
 * \page edma_use_case_2 Use case #2 - EDMA transfer from memory to peripheral
 * using a mixt of peripheral and standard channels (an auto-determination of
 * memory buffer length is done using search feature on standard channel).
 *
 * This use case first shows how to found a pattern in memory and then how to
 * use it to set up a burst EDMA transfer between the main system memory and a
 * peripheral using a peripheral channel.
 *
 * In this use case, the EDMA is configured for:
 * - Burst length: 1 byte
 * - Transfer count: Using "Search data" EDMA feature
 * - Source: Buffer located in RAM
 * - Destination: USARTC0 in RS232 mode
 * - Source address reload mode: End of transaction
 * - Source address direction mode: Increment
 * - Transfer trigger: USART data register empty (DRE)
 *
 * In this use case data is an ASCII string. An ASCII string always ends by a
 * zero character. The EDMA Data Search will set the buffer length for
 * a transfer (copy) from the ASCII string in RAM to USART, each time the
 * USART data register is empty.
 * This example is analogus to <code>printf(usart_string)</code> function of
 * "stdio" library.
 *
 * \section edma_use_case_2_setup Setup steps
 *
 * \subsection edma_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref sysclk_group "System Clock Manager Service (sysclk)"
 * -# \ref usart_group "USART Driver"
 *
 * The USART must be configured according to the XMEGA USART driver Quick Start
 * \ref xmega_usart_quickstart "Basic use case".
 *
 * \subsection edma_use_case_2_setup_code Example code
 * Add to application C-file:
 * \code
	#define EDMA_CHMODE            EDMA_CHMODE_STD0_gc
	#define EDMA_USART_CHANNEL     2
	#define EDMA_SEARCH_CHANNEL    0
	#define EDMA_SEARCH_PATTERN    0x00

	static uint8_t usart_string[] = "XMEGAE5 EDMA-USART print test.\r\n";

	static uint8_t edma_search_data(void)
	{
	    struct edma_channel_config my_search_ch_conf;
	    memset(&my_search_ch_conf, 0, sizeof(my_search_ch_conf));

	    edma_channel_set_burst_length(&my_search_ch_conf,
	              EDMA_CH_BURSTLEN_1BYTE_gc);
	    edma_channel_set_transfer_count16(&my_search_ch_conf, 255);

	    edma_channel_set_src_reload_mode(&my_search_ch_conf,
	              EDMA_CH_RELOAD_NONE_gc);

	    edma_channel_set_src_dir_mode(&my_search_ch_conf, EDMA_CH_DIR_INC_gc);
	    edma_channel_set_search_mode(&my_search_ch_conf, EDMA_CH_DIR_MP1_gc);

	    edma_channel_set_source_address(&my_search_ch_conf,
	              (uint16_t)usart_string);
	    edma_channel_set_search_data(&my_search_ch_conf,
	              EDMA_SEARCH_PATTERN, 0xFF);

	    edma_channel_set_trigger_source(&my_search_ch_conf,
	              EDMA_CH_TRIGSRC_OFF_gc);

	    edma_channel_write_config(EDMA_SEARCH_CHANNEL, &my_search_ch_conf);
	    edma_channel_enable(EDMA_SEARCH_CHANNEL);
	    edma_channel_trigger_block_transfer(EDMA_SEARCH_CHANNEL);

	    do {} while (edma_get_channel_status(EDMA_SEARCH_CHANNEL) !=
	              EDMA_CH_TRANSFER_COMPLETED);

	    edma_channel_read_config(EDMA_SEARCH_CHANNEL, &my_search_ch_conf);

	    return ((uint8_t)(my_search_ch_conf.addr -
	                     (uint16_t)usart_string)
	                     - 1);
	}

	static void edma_usart_init(uint8_t string_size)
	{
	    struct edma_channel_config my_usart_ch_conf;
	    memset(&my_usart_ch_conf, 0, sizeof(my_usart_ch_conf));

	    edma_channel_set_burst_length(&my_usart_ch_conf,
	              EDMA_CH_BURSTLEN_1BYTE_gc);
	    edma_channel_set_transfer_count8(&my_usart_ch_conf, string_size);

	    edma_channel_set_memory_reload_mode(&my_usart_ch_conf,
	              EDMA_CH_RELOAD_TRANSACTION_gc);

	    edma_channel_set_memory_dir_mode(&my_usart_ch_conf,
	              EDMA_CH_DIR_INC_gc);

	    edma_channel_set_memory_address(&my_usart_ch_conf,
	              (uint16_t)usart_string);

	    edma_channel_set_trigger_source(&my_usart_ch_conf,
	              EDMA_CH_TRIGSRC_USARTC0_DRE_gc);

	    edma_channel_set_single_shot(&my_usart_ch_conf);

	    edma_channel_write_config(EDMA_USART_CHANNEL, &my_usart_ch_conf);
	    edma_channel_enable(EDMA_USART_CHANNEL);
	}
\endcode
 *
 * Add to \c main():
 * \code
	#define USART    &USARTC0
	static usart_rs232_options_t USART_OPTIONS = {
	     .baudrate   = 19200,
	     .charlength = USART_CHSIZE_8BIT_gc,
	     .paritytype = USART_PMODE_DISABLED_gc,
	     .stopbits   = false
	};
	sysclk_init();
	usart_init_rs232(USART, &USART_OPTIONS);
	edma_enable(EDMA_CHMODE);
	edma_usart_init(edma_search_data());
\endcode
 *
 * \subsection edma_use_case_2_setup_flow Workflow
 * -# Define the EDMA channel mode that will be set. In this case, the EDMA will
 *    be set having 1 standard channel, EDMA_CH_0 and 2 peripheral channels,
 *    EDMA_CH_2 and EDMA_CH_3:
 *   - \code #define EDMA_CHMODE EDMA_CHMODE_STD0_gc \endcode
 * -# Define the EDMA peripheral channel that will be used for the USART
 *    transfer for convenience:
 *   - \code #define EDMA_USART_CHANNEL 2 \endcode
 * -# Define the EDMA standard channel that will be used to search the character
 *    flaging the end of an ASCII string (0x00):
 *   - \code #define EDMA_SEARCH_CHANNEL 0 \endcode
 * -# Define character (or pattern) to search:
 *   - \code #define EDMA_SEARCH_PATTERN 0x00 \endcode
 * -# Create a global array that will hold the text to print:
 *   - \code
	static uint8_t usart_string[] = "XMEGAE5 EDMA-USART print test.\r\n";
\endcode
 * -# Create a function \c edma_search_data() to search the end of string. This
 *    function will return the string size (end of string character excluded):
 *     - \code
	static uint8_t edma_search_data(void)
	{
	    // ...
	}
\endcode
 * -# Create config struct for EDMA channel using for search:
 *   - \code struct edma_channel_config my_search_ch_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&my_search_ch_conf, 0, sizeof(my_search_ch_conf)); \endcode
 * -# Configure the EDMA channel for single byte bursts with a maximun of 255,
 *    that corresponds to the maximum size that the peripheral channel (used for
 *    the USART transfer) can handle. Zero, as no search limit, could be an
 *    alternative value:
 *   - \code
	edma_channel_set_burst_length(&my_search_ch_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_transfer_count16(&my_search_ch_conf, 255);
\endcode
 * -# Configure the EDMA channel to not reset the source address at the end of
 *    the complete searching. This is necessary to get the end of string
 *    character pointer. Destination reload mode setting is not required:
 *   - \code
	edma_channel_set_src_reload_mode(&my_search_ch_conf, EDMA_CH_RELOAD_NONE_gc);
\endcode
 * -# Configure the EDMA channel to increment the memory address after each
 *     computed byte:
 *   - \code
	edma_channel_set_src_dir_mode(&my_search_ch_conf, EDMA_CH_DIR_INC_gc);
\endcode
 * -# Configure the search mode performed by the EDMA channel. In this case it
 *    is the "Mask-Search" mode defined by <code>EDMA_CH_DIR_MP1_gc<\code>:
 *   - \code
	edma_channel_set_search_mode(&my_search_ch_conf, EDMA_CH_DIR_MP1_gc);
\endcode
 * -# Configure the EDMA channel source address:
 *   - \code
	edma_channel_set_source_address(&my_search_ch_conf, (uint16_t)usart_string);
\endcode
 * -# Set the search data and mask (full mask = 0xFF):
 *   - \code
	edma_channel_set_search_data(&my_search_ch_conf, EDMA_SEARCH_PATTERN, 0xFF);
\endcode
 * -# Set software command as trigger for the EDMA channel:
 *   - \code
	edma_channel_set_trigger_source(&my_search_ch_conf, EDMA_CH_TRIGSRC_OFF_gc);
\endcode
 * -# Write the EDMA channel configuration to the EDMA hardware and enable it so
 *    that it can be triggered to start the search:
 *   - \code
	edma_channel_write_config(EDMA_SEARCH_CHANNEL, &my_search_ch_conf);
	edma_channel_enable(EDMA_SEARCH_CHANNEL);
\endcode
 * -# Software command to start the EDMA search:
 *   - \code edma_channel_trigger_block_transfer(EDMA_CHANNEL); \endcode
 * -# Wait for end of search:
 *   - \code
	do {} while (edma_get_channel_status(EDMA_SEARCH_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 * -# Read the EDMA hardware to the EDMA channel configuration after search is
 *    performed:
 *   - \code
	edma_channel_read_config(EDMA_SEARCH_CHANNEL, &my_search_ch_conf);
\endcode
 * -# Compute and return the string size without taking into account the last
 *    character (end of string):
 *   - \code
	return ((uint8_t)(my_search_ch_conf.addr - (uint16_t)usart_string) - 1);
\endcode
 *
 * -# Create a function \c edma_usart_init() to intialize the USART transfer via
 *    EDMA. The string size is passed as parameter:
 *     - \code
	static void edma_usart_init(uint8_t string_size)
	{
	    // ...
	}
\endcode
 * -# Create config struct for EDMA channel using for USART transfer:
 *   - \code struct edma_channel_config my_usart_ch_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&my_usart_ch_conf, 0, sizeof(my_usart_ch_conf)); \endcode
 * -# Get the array length that will be the used for the memory buffer located
 *    in RAM:
 *   - \code uint8_t string_size = sizeof(usart_string); \endcode
 * -# Configure the EDMA channel for single byte bursts with a transfer length
 *    equal to the string size passed as parameter.
 *   - \code
	edma_channel_set_burst_length(&my_usart_ch_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_transfer_count8(&my_usart_ch_conf, string_size);
\endcode
 * -# Configure the EDMA channel to reset the memory address at the end of the
 *    complete transaction (i.e. after \c string_size bytes copied):
 *   - \code
	edma_channel_set_memory_reload_mode(&my_usart_ch_conf,
	        EDMA_CH_RELOAD_TRANSACTION_gc);
\endcode
 * -# Configure the EDMA channel to increment the memory address after each byte
 *    transferred:
 *   - \code
	edma_channel_set_memory_dir_mode(&my_usart_ch_conf, EDMA_CH_DIR_INC_gc);
\endcode
 * -# Configure the EDMA channel memory address:
 *   - \code
	edma_channel_set_memory_address(&my_usart_ch_conf, (uint16_t)usart_string);
\endcode
 * -# Set USART C0 data register empty as trigger for the EDMA channel:
 *   - \code
	edma_channel_set_trigger_source(&my_usart_ch_conf,
	        EDMA_CH_TRIGSRC_USARTC0_DRE_gc);
\endcode
 * -# Configure the EDMA channel in single shot mode, so that each time it is
 *    triggered it will perform one bust transfer only:
 *   - \code edma_channel_set_single_shot(&my_usart_ch_conf); \endcode
 * -# Write the EDMA channel configuration to the EDMA hardware and enable it so
 *    that it can be triggered to start the transfer:
 *   - \code
	edma_channel_write_config(EDMA_USART_CHANNEL, &my_usart_ch_conf);
	edma_channel_enable(EDMA_USART_CHANNEL);
\endcode
 * -# Define what USART will be used:
 *   - \code #define USART &USARTC0 \endcode
 * -# Define and fill the USART structure:
 *   - \code
	static usart_rs232_options_t USART_OPTIONS = {
	    .baudrate   = 19200,
	    .charlength = USART_CHSIZE_8BIT_gc,
	    .paritytype = USART_PMODE_DISABLED_gc,
	    .stopbits   = false
	};
\endcode
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 * -# Initialize the USART:
 *      - \code usart_init_rs232(USART, &USART_OPTIONS); \endcode
 * -# Enable the EDMA module configuring all channels in it. In this case, the
 *    EDMA will be set having 1 standard channel, EDMA_CH_0 and 2 peripheral
 *    channels, EDMA_CH_2 and EDMA_CH_3. This corresponds to
 *    <code>EDMA_CHMODE_STD0_gc</code> configuration:
 *      - \code edma_enable(EDMA_CHMODE); \endcode
 *  \attention Calling edma_enable() will result in a soft-reset of the entire
 *             EDMA module, clearing all channel configurations. If more than
 *             one EDMA channel is to be configured, this function should be
 *             called only once in the application initialization procedure
 *             only.
 * -# Call our <code>edma_usart_init()<\code> function passing as argument the
 *    string size (return value of <code>edma_search_data()<\code> function):
 *      - \code edma_usart_init(edma_search_data()); \endcode
 *
 * \section edma_use_case_2_usage Usage steps
 *
 * \subsection edma_use_case_2_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	do {} while (edma_get_channel_status(EDMA_USART_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 *
 * \subsection edma_use_case_2_usage_flow Workflow
 * -# Wait for the transfer to complete:
 *   - \code
	do {} while (edma_get_channel_status(EDMA_USART_CHANNEL) !=
	        EDMA_CH_TRANSFER_COMPLETED);
\endcode
 */

#endif /* EDMA_H */
