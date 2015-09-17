/**
 * \file
 *
 * \brief AVR XMEGA Advanced Encryption Standard (AES) driver
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
#include <aes.h>

//! AES interrupt callback function pointer.
static aes_callback_t aes_callback = NULL;

/**
 * \brief AES control register settings
 *
 * \note Set AES CTRL register.
 * Refer to the device manual for detailed information.
 *
 * \param decrypt    encryption/decryption direction.
 * \param auto_start Auto Start Trigger.
 * \param xor_mode   AES State XOR Load Enable.
 *
 */
void aes_configure(enum aes_dec decrypt, enum aes_auto auto_start,
	enum aes_xor xor_mode)
{
	/* Initialize AES control register */
	AES.CTRL = ((uint8_t)decrypt | (uint8_t)auto_start | (uint8_t)xor_mode);
}

/**
 * \brief Load key memory
 *
 * \note The key is a user defined value of 16 bytes in AVR XMEGA Crypto Engine.
 *
 * \param key_in     Pointer to AES key input.
 *
 */
void aes_set_key(t_key key_in)
{
	uint8_t i;
	/* Load user key buffer into AES key memory. */
	uint8_t * temp_key = key_in;

	for (i = 0; i < AES_KEY_SIZE; i++) {
		AES.KEY = *(temp_key++);
	}
}

/**
 * \brief Get key memory
 *
 * \note The key is a user defined value of 16 bytes in AVR XMEGA Crypto Engine.
 *
 * \param key_out    Pointer to AES key output.
 *
 */
void aes_get_key(t_key key_out)
{
	uint8_t i;
	/* Load AES key memory into user key buffer. */
	uint8_t * temp_key = key_out;

	for (i = 0; i < AES_KEY_SIZE; i++) {
		*(temp_key++) = AES.KEY;
	}
}

/**
 * \brief Load state memory
 *
 * \note The state is a user defined value of 16 bytes in AVR XMEGA Crypto
 * Engine.
 *
 * \param data_in    Pointer to AES input date.
 *
 */
void aes_write_inputdata(t_data data_in)
{
	uint8_t i;
	/* Load user buffer into AES state memory. */
	uint8_t *temp_state = data_in;

	for (i = 0; i < AES_DATA_SIZE; i++) {
		AES.STATE = *(temp_state++);
	}
}

/**
 * \brief Get state memory
 *
 * \note The state is a user defined value of 16 bytes in AVR XMEGA Crypto
 * Engine.
 *
 * \param data_out   Pointer to AES output date.
 *
 */
void aes_read_outputdata(t_data data_out)
{
	uint8_t i;
	/* Load AES state memory into user buffer. */
	uint8_t *temp_state = data_out;

	for (i = 0; i < AES_DATA_SIZE; i++) {
		*(temp_state++) = AES.STATE;
	}
}

/**
 * \brief AES interrupt settings
 *
 * \note Resetting AES interrupt and interrupt level.
 *
 * \param intlvl     AES interrupt level.
 *
 */
void aes_isr_configure(enum aes_intlvl intlvl)
{
	/* Remove pending AES interrupts. */
	AES.STATUS = (AES_ERROR_bm | AES_SRIF_bm);
	AES.INTCTRL = intlvl;
}

/**
 * \brief Set AES interrupt callback function
 *
 * \note Sets a new callback function for interrupts on AES.
 *
 * \param callback   Pointer to the callback function to set.
 *
 */
void aes_set_callback(aes_callback_t callback)
{
	aes_callback = callback;
}

/**
 * \brief ISR for AES
 *
 * \note Calls the callback function when enabled interrupt is triggered.
 *
 */
ISR(AES_INT_vect)
{
	if (aes_callback != NULL){
		aes_callback();
	}
}
