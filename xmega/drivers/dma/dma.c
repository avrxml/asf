/**
 * \file
 *
 * \brief AVR XMEGA Direct Memory Access Controller driver
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <compiler.h>
#include <status_codes.h>
#include <sleepmgr.h>
#include <sysclk.h>
#include <dma.h>

/**
 * \private
 * \brief DMA private data struct
 */
struct dma_data_struct {
	/** Callback on complete */
	dma_callback_t callback;
};

/**
 * \internal
 * \brief DMA private data for each channel
 */
struct dma_data_struct dma_data[DMA_NUMBER_OF_CHANNELS];

/**
 * \brief Enable DMA controller
 *
 * \note This function will do a soft reset of the DMA controller, clearing all
 * previous configuration.
 */
void dma_enable(void)
{
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_DMA);
	sleepmgr_lock_mode(SLEEPMGR_IDLE);

	/* Reset DMA controller just to make sure everything is from scratch */
	DMA.CTRL = DMA_RESET_bm;
	DMA.CTRL = DMA_ENABLE_bm;
}

/**
 * \brief Disable DMA controller
 */
void dma_disable(void)
{
	DMA.CTRL = 0;
	sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_DMA);
	sleepmgr_unlock_mode(SLEEPMGR_IDLE);
}

/**
 * \brief Set callback for given DMA channel
 *
 * \param num \ref dma_channel_num_t
 * \param callback \ref dma_callback_t
 */
void dma_set_callback(dma_channel_num_t num, dma_callback_t callback)
{
	dma_data[num].callback = callback;
}

/**
 * \internal
 * \brief Common DMA channel interrupt handler
 *
 * Calls the channel callback with the channel status code. The following
 * status codes are possible:
 * - DMA_CH_TRANSFER_COMPLETED: Transfer completed successfully
 * - DMA_CH_TRANSFER_ERROR: Fault in transfer
 *
 * The optional callback used by the interrupt handler is set by the
 * dma_set_callback() function.
 *
 * \param num DMA channel number to handle interrupt for
 */
static void dma_interrupt(const dma_channel_num_t num)
{
	enum dma_channel_status status;
	DMA_CH_t *channel;

	channel = dma_get_channel_address_from_num(num);
	status  = dma_get_channel_status(num);

	/* Clear all interrupt flags to be sure */
	channel->CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;

	if (dma_data[num].callback) {
		dma_data[num].callback(status);
	}
}

/**
 * \internal
 * \brief DMA channel 0 interrupt handler
 */
ISR(DMA_CH0_vect)
{
	dma_interrupt(0);
}

#if DMA_NUMBER_OF_CHANNELS > 1
/**
 * \internal
 * \brief DMA channel 1 interrupt handler
 */
ISR(DMA_CH1_vect)
{
	dma_interrupt(1);
}
#endif

#if DMA_NUMBER_OF_CHANNELS > 2
/**
 * \internal
 * \brief DMA channel 2 interrupt handler
 */
ISR(DMA_CH2_vect)
{
	dma_interrupt(2);
}
#endif

#if DMA_NUMBER_OF_CHANNELS > 3
/**
 * \internal
 * \brief DMA channel 3 interrupt handler
 */
ISR(DMA_CH3_vect)
{
	dma_interrupt(3);
}
#endif

/**
 * \brief Write DMA channel configuration to hardware
 *
 * This function will write the DMA channel configuration, provided by a
 * \ref dma_channel_config.
 *
 * \param num DMA channel number to write configuration to
 * \param config Pointer to a DMA channel config, given by a
 *               \ref dma_channel_config
 */
void dma_channel_write_config(dma_channel_num_t num,
		struct dma_channel_config *config)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

#ifdef CONFIG_HAVE_HUGEMEM
	channel->DESTADDR0 = (uint32_t)config->destaddr;
	channel->DESTADDR1 = (uint32_t)config->destaddr >> 8;
	channel->DESTADDR2 = (uint32_t)config->destaddr >> 16;
#else
	channel->DESTADDR0 = (uint32_t)config->destaddr16;
	channel->DESTADDR1 = (uint32_t)config->destaddr16 >> 8;
#  if XMEGA_A || XMEGA_AU
	channel->DESTADDR2 = 0;
#  endif
#endif

#ifdef CONFIG_HAVE_HUGEMEM
	channel->SRCADDR0 = (uint32_t)config->srcaddr;
	channel->SRCADDR1 = (uint32_t)config->srcaddr >> 8;
	channel->SRCADDR2 = (uint32_t)config->srcaddr >> 16;
#else
	channel->SRCADDR0 = (uint32_t)config->srcaddr16;
	channel->SRCADDR1 = (uint32_t)config->srcaddr16 >> 8;
#  if XMEGA_A || XMEGA_AU
	channel->SRCADDR2 = 0;
#  endif
#endif

	channel->ADDRCTRL = config->addrctrl;
	channel->TRIGSRC = config->trigsrc;
	channel->TRFCNT = config->trfcnt;
	channel->REPCNT = config->repcnt;

	channel->CTRLB = config->ctrlb;

	/* Make sure the DMA channel is not enabled before dma_channel_enable()
	 * is called.
	 */
#if XMEGA_A || XMEGA_AU
	channel->CTRLA = config->ctrla & ~DMA_CH_ENABLE_bm;
#else
	channel->CTRLA = config->ctrla & ~DMA_CH_CHEN_bm;
#endif

	cpu_irq_restore(iflags);
}

/**
 * \brief Read DMA channel configuration from hardware
 *
 * This function will read the DMA channel configuration into a
 * \ref dma_channel_config.
 *
 * \param num DMA channel number to read configuration from
 * \param config Pointer to a DMA channel config, given by a
 *               \ref dma_channel_config
 */
void dma_channel_read_config(dma_channel_num_t num,
		struct dma_channel_config *config)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();
#ifdef CONFIG_HAVE_HUGEMEM
	uint32_t address;
#else
	uint16_t address;
#endif

	address = channel->DESTADDR0;
	address |= channel->DESTADDR1 << 8;
#ifdef CONFIG_HAVE_HUGEMEM
	address |= (uint32_t)channel->DESTADDR2 << 16;
	config->destaddr = (hugemem_ptr_t)address;
#else
	config->destaddr16 = address;
#endif

	address = channel->SRCADDR0;
	address |= channel->SRCADDR1 << 8;
#ifdef CONFIG_HAVE_HUGEMEM
	address |= (uint32_t)channel->SRCADDR2 << 16;
	config->srcaddr = (hugemem_ptr_t)address;
#else
	config->srcaddr16 = address;
#endif

	config->addrctrl = channel->ADDRCTRL;
	config->trigsrc  = channel->TRIGSRC;
	config->trfcnt   = channel->TRFCNT;
	config->repcnt   = channel->REPCNT;

	config->ctrlb = channel->CTRLB;
	config->ctrla = channel->CTRLA;

	cpu_irq_restore(iflags);
}
