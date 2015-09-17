/**
 * \file
 *
 * \brief AVR XMEGA Enhanced Direct Memory Access Controller driver
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
#include <compiler.h>
#include <status_codes.h>
#include <sleepmgr.h>
#include <sysclk.h>
#include <edma.h>

/**
 * \private
 * \brief EDMA private data struct
 */
struct edma_data_struct {
	/** Callback on complete */
	edma_callback_t callback;
};

/**
 * \internal
 * \brief EDMA private data for each channel
 */
struct edma_data_struct edma_data[EDMA_MAX_NUMBER_OF_CHANNELS];

/**
 * \brief Enable EDMA controller
 *
 * \note Before enabling the EDMA controller, this function will do a soft
 *  reset, clearing all previous configurations.
 *
 * \param channelmode Channel configuration mode given by a EDMA_CHMODE_t type
 */
void edma_enable(EDMA_CHMODE_t channelmode)
{

	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EDMA);
	sleepmgr_lock_mode(SLEEPMGR_IDLE);

	/* Reset DMA controller just to make sure everything is from scratch */
	EDMA.CTRL = EDMA_RESET_bm;
	EDMA.CTRL = EDMA_ENABLE_bm | channelmode;
}

/**
 * \brief Disable EDMA controller
 *
 * \note This function disables the EDMA controller, clearing all previous
 * configurations.
 */
void edma_disable(void)
{
	/* Disable */
	EDMA.CTRL = 0x00;
	/* Wait for effective operation */
	while (EDMA.CTRL & EDMA_ENABLE_bm) {
	}
	/* Reset all fields */
	EDMA.CTRL = 0x00;

	sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EDMA);
	sleepmgr_unlock_mode(SLEEPMGR_IDLE);
}

/**
 * \brief Set callback for given EDMA channel
 *
 * \param num \ref edma_channel_num_t
 * \param callback \ref edma_callback_t
 */
void edma_set_callback(edma_channel_num_t num, edma_callback_t callback)
{
	edma_data[num].callback = callback;
}

/**
 * \internal
 * \brief Common EDMA channel interrupt handler
 *
 * Calls the channel callback with the channel status code. The following
 * status codes are possible:
 * - EDMA_CH_TRANSFER_COMPLETED: Transfer completed successfully
 * - EDMA_CH_TRANSFER_ERROR: Fault in transfer
 *
 * The optional callback used by the interrupt handler is set by the
 * edma_set_callback() function.
 *
 * \param num EDMA channel number to handle interrupt for
 */
static void edma_interrupt(const edma_channel_num_t num)
{
	edma_channel_status_t status;
	EDMA_CH_t *channel;

	channel = edma_get_channel_address_from_num(num);
	status  = edma_get_channel_status(num);

	/* Clear all interrupt flags to be sure */
	channel->CTRLB |= EDMA_CH_TRNIF_bm | EDMA_CH_ERRIF_bm;

	if (edma_data[num].callback) {
		edma_data[num].callback(status);
	}
}

/**
 * \internal
 * \brief eDMA channel 0 interrupt handler
 */
ISR(EDMA_CH0_vect)
{
	edma_interrupt(EDMA_CH_0);
}

#if EDMA_MAX_NUMBER_OF_CHANNELS > EDMA_CH_1
/**
 * \internal
 * \brief DMA channel 1 interrupt handler
 */
ISR(EDMA_CH1_vect)
{
	edma_interrupt(EDMA_CH_1);
}
#endif

#if EDMA_MAX_NUMBER_OF_CHANNELS > EDMA_CH_2
/**
 * \internal
 * \brief DMA channel 2 interrupt handler
 */
ISR(EDMA_CH2_vect)
{
	edma_interrupt(EDMA_CH_2);
}
#endif

#if EDMA_MAX_NUMBER_OF_CHANNELS > EDMA_CH_3
/**
 * \internal
 * \brief DMA channel 3 interrupt handler
 */
ISR(EDMA_CH3_vect)
{
	edma_interrupt(EDMA_CH_3);
}
#endif

/**
 * \brief Write EDMA channel configuration from "configuration" variable to
 * hardware
 *
 * This function will write the EDMA channel configuration, provided by a
 * \ref edma_channel_config.
 *
 * \param num EDMA channel number to write configuration to
 * \param config Pointer to a EDMA channel config, given by a
 *               \ref edma_channel_config
 * \retval  edma_channel_config (see edma_channel_status_enum)
 */
edma_channel_status_t edma_channel_write_config(edma_channel_num_t num,
		struct edma_channel_config *config)
{
	/* Channel type checking (standard, peripheral or unavailable) */
	int8_t std_channel = edma_channel_is_standard(num);
	if (std_channel == -1) {
		return EDMA_CH_UNAVAILABLE;
	}

	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	/* Memory address or source address */
	channel->ADDRL = (uint8_t)(config->addr);
	channel->ADDRH = (uint8_t)((config->addr) >> 8);

	/* Memory address or source address control */
	channel->ADDRCTRL = config->addrctrl;

	/* If standard channel - destination address
	 *                     - destination address control
	 *                     - 16-bit transfer count
	 *          Else       - 8-bit transfer count
	 */
	if (std_channel == 1) {
		channel->DESTADDRL = (uint8_t)(config->destaddr);
		channel->DESTADDRH = (uint8_t)((config->destaddr) >> 8);
		channel->DESTADDRCTRL = config->destaddrctrl;
		channel->TRFCNTL = (uint8_t)(config->trfcnt16);
		channel->TRFCNTH = (uint8_t)((config->trfcnt16) >> 8);
	} else {
		channel->TRFCNTL = config->trfcnt8;
	}

	/* Trigger control */
	channel->TRIGSRC = config->trigsrc;

	/* Control registers A and B
	 * Make sure the EDMA channel is not enabled and flags are off before
	 * edma_channel_enable() is called.
	 */
	channel->CTRLB = config->ctrlb | (EDMA_CH_ERRIF_bm | EDMA_CH_TRNIF_bm);
	channel->CTRLA = config->ctrla & ~EDMA_CH_ENABLE_bm;

	cpu_irq_restore(iflags);

	return EDMA_CH_READY;
}

/**
 * \brief Read EDMA channel configuration from hardware to "configuration"
 * variable
 *
 * This function will synchronize the EDMA channel configuration
 * \ref edma_channel_config with the hardware content.
 *
 * \param num EDMA channel number to read configuration from
 * \param config Pointer to a EDMA channel config, given by a
 *               \ref edma_channel_config
 * \retval  edma_channel_config (see edma_channel_status_enum)
 */
edma_channel_status_t edma_channel_read_config(edma_channel_num_t num,
		struct edma_channel_config *config)
{
	/* Channel type checking (standard, peripheral or unavailable) */
	int8_t std_channel = edma_channel_is_standard(num);
	if (std_channel == -1) {
		return EDMA_CH_UNAVAILABLE;
	}

	EDMA_CH_t *channel = edma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	/* Memory address or source address */
	config->addr  =  ((uint16_t)(channel->ADDRL));
	config->addr |= (((uint16_t)(channel->ADDRH)) << 8);

	/* Memory address or source address control */
	config->addrctrl = channel->ADDRCTRL;

	/* If standard channel - destination address
	 *                     - destination address control
	 *                     - 16-bit transfer count
	 *          Else       - 8-bit transfer count
	 */
	if (std_channel) {
		config->destaddr  =  ((uint16_t)(channel->DESTADDRL));
		config->destaddr |= (((uint16_t)(channel->DESTADDRH)) << 8);
		config->destaddrctrl = channel->DESTADDRCTRL;
		config->trfcnt16  =  ((uint16_t)(channel->TRFCNTL));
		config->trfcnt16 |= (((uint16_t)(channel->TRFCNTH)) << 8);
	} else {
		config->trfcnt8 = channel->TRFCNTL;
	}

	/* Trigger control */
	config->trigsrc = channel->TRIGSRC;

	/* Control registers A and B */
	config->ctrlb = channel->CTRLB;
	config->ctrla = channel->CTRLA;

	cpu_irq_restore(iflags);

	/* Check if error or transfer completed status is on. */
	if ((config->ctrlb) & EDMA_CH_ERRIF_bm) {
		return EDMA_CH_TRANSFER_ERROR;
	} else if ((config->ctrlb) & EDMA_CH_TRNIF_bm) {
		return EDMA_CH_TRANSFER_COMPLETED;
	}

	/* Check if busy or pending completed status is on. */
	if ((config->ctrlb) & EDMA_CH_CHBUSY_bm) {
		return EDMA_CH_BUSY;
	} else if ((config->ctrlb) & EDMA_CH_CHPEND_bm) {
		return EDMA_CH_PENDING;
	}

	return EDMA_CH_FREE;
}
