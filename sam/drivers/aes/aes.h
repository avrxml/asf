/**
 *
 * \file
 *
 * \brief AES software driver for SAM.
 *
 * This file defines a useful set of functions for the AES on SAM devices.
 *
 * Copyright (c) 2013-2014 Atmel Corporation. All rights reserved.
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

#ifndef AES_H_INCLUDED
#define AES_H_INCLUDED

/**
 * \defgroup group_sam_drivers_aes AES - Advanced Encryption Standard module
 *
 * Driver for the AES (Advanced Encryption Standard) module.
 * Provides functions for configuring and initiating ciphering/deciphering
 * with AES algorithms.
 * Several modes(ECB, CBC, OFB, CFB, CTR or GCM) and
 * key sizes(128-, 192- or 256-bit) are supported,
 * in addition to manual, auto or DMA-based data input to the AES module.
 *
 * \{
 */

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SAM4E
/** DMAC Channel HW Interface Number for AES. */
#define AES_DMA_TX_IDX 11
#define AES_DMA_RX_IDX 12
#endif

/** AES Processing mode */
enum aes_encrypt_mode {
	AES_DECRYPTION = 0,
	AES_ENCRYPTION,
};
/** AES Cryptographic key size */
enum aes_key_size {
	AES_KEY_SIZE_128 = 0,
	AES_KEY_SIZE_192,
	AES_KEY_SIZE_256,
};

/** AES START mode */
enum aes_start_mode {
	AES_MANUAL_START = 0,
	AES_AUTO_START,
	AES_IDATAR0_START,
};

/** AES Confidentiality mode */
enum aes_opmode {
	AES_ECB_MODE = 0,
	AES_CBC_MODE,
	AES_OFB_MODE,
	AES_CFB_MODE,
	AES_CTR_MODE,
	AES_GCM_MODE,
};

/** AES CFB size */
enum aes_cfb_size {
	AES_CFB_SIZE_128 = 0,
	AES_CFB_SIZE_64,
	AES_CFB_SIZE_32,
	AES_CFB_SIZE_16,
	AES_CFB_SIZE_8,
};

/** AES interrupt source type */
typedef enum aes_interrupt_source {
	AES_INTERRUPT_DATA_READY = AES_IER_DATRDY,
	AES_INTERRUPT_UNSPECIFIED_REGISTER_ACCESS = AES_IER_URAD,
#if SAM4C || SAM4CP || SAM4CM
	AES_INTERRUPT_END_OF_RECEIVE_BUFFER = AES_IER_ENDRX,
	AES_INTERRUPT_END_OF_TRANSMIT_BUFFER = AES_IER_ENDTX,
	AES_INTERRUPT_RECEIVE_BUFFER_FULL = AES_IER_RXBUFF,
	AES_INTERRUPT_TRANSMIT_BUFFER_FULL = AES_IER_TXBUFE,
#endif
} aes_interrupt_source_t;

#if SAM4E
#define AES_INTERRUPT_SOURCE_NUM 2
#elif SAM4C || SAM4CP || SAM4CM
#define AES_INTERRUPT_SOURCE_NUM 6
#endif

/**
 * \brief Interrupt callback function type for AES.
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the aes_callback_t type.
 *
 */
typedef void (*aes_callback_t)(void);

/** AES Configuration structure. */
struct aes_config {
	/* 0=decryption data, 1=encryption data */
	enum aes_encrypt_mode encrypt_mode;
	/* 0 = 128bits, 1 = 192bits, 2 = 256bits */
	enum aes_key_size key_size;
	/* 0 = MANUAL_START, 1 = AUTO_START, 2 = IDATAR0_START */
	enum aes_start_mode start_mode;
	/* 0 = ECB, 1 = CBC, 2 = OFB, 3 = CFB, 4 = CTR , 5 = GCM*/
	enum aes_opmode opmode;
	/* 0 = 128bits, 1 = 64bits, 2 = 32bits, 3 = 16bits, 4 = 8bits */
	enum aes_cfb_size cfb_size;
	/* last output data mode. */
	bool lod;
	/* GCM Automatic Tag Generation Enable */
	bool gtag_en;
	/* processing delay parameter */
	uint32_t processing_delay;
};

void aes_get_config_defaults(struct aes_config *const p_cfg);

void aes_init(Aes *const p_aes, struct aes_config *const p_cfg);

/**
 * \brief Perform a software reset of the AES.
 *
 * \param p_aes Pointer to an AES instance.
 *
 */
static inline void aes_reset(Aes *const p_aes)
{
	p_aes->AES_CR = AES_CR_SWRST;
}

/**
 * \brief Starts manual encryption/decryption process.
 *
 * \param p_aes Pointer to an AES instance.
 *
 */
static inline void aes_start(Aes *const p_aes)
{
	p_aes->AES_CR = AES_CR_START;
}

void aes_enable(void);

void aes_disable(void);

void aes_set_config(Aes *const p_aes, struct aes_config *const p_cfg);

/**
 * \brief Get the AES interrupt status.
 *
 * \param  p_aes Pointer to an AES instance.
 *
 * \return the content of the interrupt status register.
 *
 */
static inline uint32_t aes_read_interrupt_status(Aes *const p_aes)
{
	return p_aes->AES_ISR;
}

void aes_set_callback(Aes *const p_aes,
		aes_interrupt_source_t source, aes_callback_t callback,
		uint8_t irq_level);

/**
 * \brief Enable the AES interrupt.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  source Interrupt source.
 *
 */
static inline void aes_enable_interrupt(Aes *const p_aes,
		aes_interrupt_source_t source)
{
	p_aes->AES_IER = (uint32_t)source;
}

/**
 * \brief Disable the AES interrupt.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  source Interrupt source.
 *
 */
static inline void aes_disable_interrupt(
		Aes *const p_aes, aes_interrupt_source_t source)
{
	p_aes->AES_IDR = (uint32_t)source;
}

/**
 * \brief Get the AES interrupt mask status.
 *
 * \param  p_aes Pointer to an AES instance.
 *
 * \return the content of the interrupt mask register.
 *
 */
static inline uint32_t aes_read_interrupt_mask(Aes *const p_aes)
{
	return p_aes->AES_IMR;
}

void aes_write_key(Aes *const p_aes, const uint32_t *p_key);

void aes_write_initvector(Aes *const p_aes, const uint32_t *p_vector);

void aes_write_input_data(Aes *const p_aes,
		const uint32_t *p_input_data_buffer);

void aes_read_output_data(Aes *const p_aes,
		uint32_t *p_output_data_buffer);

#if SAM4C || SAM4CP || SAM4CM
Pdc *aes_get_pdc_base(Aes *p_aes);

/**
 * \brief Get the AES Additional Authenticated Data Length.
 *
 * \param  p_aes Pointer to an AES instance.
 *
 * \return the content of the AES_AADLENR register.
 *
 */
static inline uint32_t aes_read_authen_datalength(Aes *const p_aes)
{
	return p_aes->AES_AADLENR;
}

/**
 * \brief Set the AES Additional Authenticated Data Length.
 *
 * \param  p_aes Pointer to an AES instance.
 *
 * \param  length length in bytes of the AAD data that is to be processed.
 *
 */
static inline void aes_write_authen_datalength(Aes *const p_aes,
		uint32_t length)
{
	p_aes->AES_AADLENR = length;
}

/**
 * \brief Get the AES Plaintext / Ciphertext Length.
 *
 * \param  p_aes Pointer to an AES instance.
 *
 * \return the content of the AES_CLENR register.
 *
 */
static inline uint32_t aes_read_pctext_length(Aes *const p_aes)
{
	return p_aes->AES_CLENR;
}

/**
 * \brief Set the AES Plaintext / Ciphertext Length.
 *
 * \param p_aes Pointer to an AES instance.
 *
 * \param length Length in bytes of the plaintext/ciphertext data that is to be
 *processed
 *
 */
static inline void aes_write_pctext_length(Aes *const p_aes, uint32_t length)
{
	p_aes->AES_CLENR = length;
}

/**
 * \brief  Get AES GCM Encryption Counter.
 *
 * \param  p_aes Pointer to an AES instance.
 *
 * \return the content of the AES_CTRR register.
 *
 */
static inline uint32_t aes_read_gcm_counter(Aes *const p_aes)
{
	return p_aes->AES_CTRR;
}

/**
 * \brief  Get AES GCM Intermediate Hash Word.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  id  The sequency of GHASH Word
 *
 * \return the content of the AES_GHASHRx[x = 0...3] register.
 *
 */
static inline uint32_t aes_read_ghash(Aes *const p_aes, uint32_t id)
{
	return p_aes->AES_GHASHR[id];
}

/**
 * \brief  Set AES GCM Intermediate Hash Word.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  id  The sequency of GHASH Word
 * \param  ghash Intermediate GCM Hash Word x
 *
 */
static inline void aes_write_ghash(Aes *const p_aes, uint32_t id,
		uint32_t ghash)
{
	p_aes->AES_GHASHR[id] = ghash;
}

/**
 * \brief  Get AES GCM Authentication Tag Word.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  id  The sequency of GCM Authentication Tag
 *
 * \return the content of the AES_TAGRx[x = 0...3] register.
 *
 */
static inline uint32_t aes_read_tag(Aes *const p_aes, uint32_t id)
{
	return p_aes->AES_TAGR[id];
}

/**
 * \brief  Get AES GCM H Word.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  id  The sequency of GCM H Word
 *
 * \return the content of the AES_GCMHRx[x = 0...3] register.
 *
 */
static inline uint32_t aes_read_gcmh(Aes *const p_aes, uint32_t id)
{
	return p_aes->AES_GCMHR[id];
}

/**
 * \brief  Set AES GCM H Word.
 *
 * \param  p_aes Pointer to an AES instance.
 * \param  id  The sequency of GCM H Word
 * \param  hword GCM H Word x
 *
 */
static inline void aes_write_gcmh(Aes *const p_aes, uint32_t id, uint32_t hword)
{
	p_aes->AES_GCMHR[id] = hword;
}

#endif

#ifdef __cplusplus
}
#endif

/**
 * \}
 */

/**
 * \page sam_aes_quick_start Quick Start Guide for the AES driver
 *
 * This is the quick start guide for the \ref group_sam_drivers_aes, with
 * step-by-step instructions on how to configure and use the driver for
 * a specific use case.The code examples can be copied into e.g the main
 * application loop or any other function that will need to control the
 * AES module.
 *
 * \section aes_qs_use_cases Use cases
 * - \ref aes_basic
 *
 * \section aes_basic AES basic usage
 *
 * This use case will demonstrate how to initialize the AES module to
 * encryption or decryption data.
 *
 *
 * \section aes_basic_setup Setup steps
 *
 * \subsection aes_basic_prereq Prerequisites
 *
 * This module requires the following service
 * - \ref clk_group
 *
 * \subsection aes_basic_setup_code
 *
 * Add this to the main loop or a setup function:
 * \code
	struct aes_config   g_aes_cfg;
	aes_get_config_defaults(&g_aes_cfg);
	aes_init(AES, &g_aes_cfg);
	aes_enable();
\endcode
 *
 * \subsection aes_basic_setup_workflow
 *
 * -# Enable the AES module
 *  - \code aes_enable(); \endcode
 *
 * -# Set the AES interrupt and callback
 * \code
	 aes_set_callback(AES, AES_INTERRUPT_DATA_READY,
			aes_callback, 1);
\endcode
 *
 * -# Initialize the AES to ECB cipher mode
 * \code
	g_aes_cfg.encrypt_mode = AES_ENCRYPTION;
	g_aes_cfg.key_size = AES_KEY_SIZE_128;
	g_aes_cfg.start_mode = AES_AUTO_MODE;
	g_aes_cfg.opmode = AES_ECB_MODE;
	g_aes_cfg.cfb_size = AES_CFB_SIZE_128;
	g_aes_cfg.lod = false;
	aes_set_config(AES, &g_aes_cfg);
\endcode
 *
 * \section aes_basic_usage Usage steps
 *
 * \subsection aes_basic_usage_code
 *
 * We can then encrypte the plain text by
 * \code
	aes_write_key(AES, key128);
	aes_write_input_data(AES, ref_plain_text);
\endcode
 *
 * We can get the cipher text after it's ready by
 * \code
	aes_read_output_data(AES, output_data);
\endcode
 */

#endif  /* AES_H_INCLUDED */
