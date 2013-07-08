/**
 *
 * \file
 *
 * \brief AES software driver for SAM.
 *
 * This file defines a useful set of functions for the AES on SAM devices.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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

#include "aes.h"
#include "sysclk.h"
#include "sleepmgr.h"

/**
 * \internal
 * \brief AES callback function pointer
 */
aes_callback_t aes_callback_pointer[AES_INTERRUPT_SOURCE_NUM];

/**
 * \brief Initializes a AES configuration structure to defaults.
 *
 *  Initializes a given AES configuration structure to a set of
 *  known default values. This function should be called on all new
 *  instances of these configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is as follows:
 *  - Encryption.
 *  - 128 key size.
 *  - Manual mode.
 *  - ECB mode.
 *  - Last output data mode equal false.
 *  - No extra delay.
 *
 *  \param p_cfg Pointer to configuration structure to initialize to
 * default values.
 */
void aes_get_config_defaults(struct aes_config *const p_cfg)
{
	/* Sanity check arguments */
	Assert(p_cfg);

	/* Default configuration values */
	p_cfg->encrypt_mode = AES_ENCRYPTION;
	p_cfg->key_size = AES_KEY_SIZE_128;
	p_cfg->start_mode = AES_MANUAL_MODE;
	p_cfg->opmode = AES_ECB_MODE;
	p_cfg->cfb_size = AES_CFB_SIZE_128;
	p_cfg->lod = false;
	p_cfg->processing_delay = 0;
}

/**
 * \brief Initialize the AES module.
 *
 * \param p_aes Base address of the AES instance.
 * \param p_cfg Pointer to AES configuration.
 *
 */
void aes_init(Aes *const p_aes, struct aes_config *const p_cfg)
{
	/* Sanity check arguments */
	Assert(p_aes);
	Assert(p_cfg);

	/* Enable clock for AES */
	sysclk_enable_peripheral_clock(ID_AES);

	/* Perform a software reset */
	aes_reset(p_aes);

	/* Initialize the AES with new configurations */
	aes_set_config(p_aes, p_cfg);

	/* Disable clock for AES */
	sysclk_disable_peripheral_clock(ID_AES);
}

/**
 * \brief Enable the AES.
 *
 */
void aes_enable(void)
{
	sysclk_enable_peripheral_clock(ID_AES);
	sleepmgr_lock_mode(SLEEPMGR_SLEEP_WFI);
}

/**
 * \brief Disable the AES.
 *
 */
void aes_disable(void)
{
	sysclk_disable_peripheral_clock(ID_AES);
	sleepmgr_unlock_mode(SLEEPMGR_SLEEP_WFI);
}

/**
 * \brief Configure the AES.
 *
 * \param p_aes Base address of the AES instance.
 * \param p_cfg Pointer to AES configuration.
 *
 */
void aes_set_config(Aes *const p_aes, struct aes_config *const p_cfg)
{
	uint32_t ul_mode = 0;

	/* Set processing mode */
	if (p_cfg->encrypt_mode) {
		ul_mode |= AES_MR_CIPHER;
	}

	/* Active dual buffer in DMA mode */
	if (p_cfg->start_mode == AES_DMA_MODE) {
		ul_mode |= AES_MR_DUALBUFF_ACTIVE;
	}

	/* Set start mode */
	ul_mode |= (p_cfg->start_mode << AES_MR_SMOD_Pos);

	/* Set key size */
	ul_mode |= (p_cfg->key_size << AES_MR_KEYSIZE_Pos);

	/* Set Confidentiality mode */
	ul_mode |= (p_cfg->opmode << AES_MR_OPMOD_Pos);

	/* Set CFB size */
	ul_mode |= (p_cfg->cfb_size << AES_MR_CFBS_Pos);

	if (p_cfg->lod) {
		ul_mode |= AES_MR_LOD;
	}

	ul_mode |= AES_MR_PROCDLY(p_cfg->processing_delay);

	ul_mode |= AES_MR_CKEY(0xE);

	p_aes->AES_MR = ul_mode;
}

/**
 * \brief Write the 128/192/256bit cryptographic key.
 *
 * \param  p_aes Base address of the AES instance.
 * \param  *p_key Pointer on 4/6/8 contiguous 32bit words.
 *
 * \note The key size depends on the AES configuration.
 *
 */
void aes_write_key(Aes *const p_aes, const uint32_t *p_key)
{
	uint32_t i, key_length = 0;

	switch ((p_aes->AES_MR & AES_MR_KEYSIZE_Msk) >>
			AES_MR_KEYSIZE_Pos) {
	case 0: /* 128bit cryptographic key */
		key_length = 4;
		break;

	case 1: /* 192bit cryptographic key */
		key_length = 6;
		break;

	case 2: /* 256bit cryptographic key */
		key_length = 8;
		break;

	default:
		break;
	}

	for (i = 0; i < key_length; i++) {
		p_aes->AES_KEYWR[i] = *p_key;
		p_key++;
	}
}

/**
 * \brief Write the initialization vector (for the CBC, CFB, OFB & CTR
 * cipher modes)
 *
 * \param  p_aes Base address of the AES instance.
 * \param  *p_vector Pointer on 4 contiguous 32bit words.
 *
 */
void aes_write_initvector(Aes *const p_aes, const uint32_t *p_vector)
{
	uint32_t i;

	for (i = 0; i < 4; i++) {
		p_aes->AES_IVR[i] = *p_vector;
		p_vector++;
	}
}

/**
 * \brief Write the input data.
 *
 * \param  p_aes Base address of the AES instance.
 * \param  *p_input_data_buffer Pointer to input buffer.
 *
 */
void aes_write_input_data(Aes *const p_aes,
		const uint32_t *p_input_data_buffer)
{
	uint32_t i;

	for (i = 0; i < 4; i++) {
		p_aes->AES_IDATAR[i] = *p_input_data_buffer;
		p_input_data_buffer++;
	}
}

/**
 * \brief Read the output data.
 *
 * \param  p_aes Base address of the AES instance.
 * \param  *p_output_data_buffer Pointer to output buffer.
 *
 */
void aes_read_output_data(Aes *const p_aes,
		uint32_t *p_output_data_buffer)
{
	uint32_t i;

	for (i = 0; i < 4; i++) {
		*p_output_data_buffer = p_aes->AES_ODATAR[i];
		p_output_data_buffer++;
	}
}

/**
 * \brief Set callback for AES
 *
 * \param p_aes Base address of the AES instance.
 * \param source Interrupt source.
 * \param callback callback function pointer.
 * \param irq_level interrupt level.
 */
void aes_set_callback(Aes *const p_aes,
		aes_interrupt_source_t source, aes_callback_t callback,
		uint8_t irq_level)
{
	if (source == AES_INTERRUPT_DATA_READY) {
		aes_callback_pointer[0] = callback;
	} else if (source == AES_INTERRUPT_UNSPECIFIED_REGISTER_ACCESS) {
		aes_callback_pointer[1] = callback;
	}
	irq_register_handler((IRQn_Type)AES_IRQn, irq_level);
	aes_enable_interrupt(p_aes, source);
}

/**
 * \brief The AES interrupt handler.
 */
void AES_Handler(void)
{
	uint32_t status = aes_read_interrupt_status(AES);
	uint32_t mask = aes_read_interrupt_mask(AES);

	if ((status & AES_ISR_DATRDY) && (mask & AES_IMR_DATRDY)) {
		if(aes_callback_pointer[0]) {
			aes_callback_pointer[0]();
		}
	}

	if ((status & AES_ISR_URAD) && (mask & AES_IMR_URAD)) {
		if(aes_callback_pointer[1]) {
			aes_callback_pointer[1]();
		}
	}
}

