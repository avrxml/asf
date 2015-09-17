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
#ifndef AES_H
#define AES_H

#include <compiler.h>
/**
 * \defgroup aes_group Advanced Encryption Standard (AES)
 *
 * See \ref xmega_aes_quickstart
 *
 * This is a driver for the AVR XMEGA AES crypto module. It provides functions
 * for configuring the AES modules, reseting and starting AES encryption and
 * decryption operations.
 *
 * \note
 * 1. After the AES has been configured, data and key must be loaded into
 * register before any operations can be done.\n
 * 2. The functions for loading configurations/data/key are not protected
 * against interrupts.
 *
 * \section xmega_aes_quickstart_guide Quick start guide
 * See \ref xmega_aes_quickstart
 *
 * @{
 */

/**
 * \name AES Configuration Define
 *
 * @{
 */

/**
 * \brief Length of one key block.
 * Always 128-bits (16 bytes).
 */
#define AES_KEY_SIZE	16

//! AES key block type
typedef uint8_t t_key[AES_KEY_SIZE];

/**
 * \brief Length of one data block.
 * Always 128-bits (16 bytes).
 */
#define AES_DATA_SIZE	16

//! AES data block type
typedef uint8_t t_data[AES_DATA_SIZE];

//! AES encryption/decryption direction settings
enum aes_dec{
	AES_ENCRYPT,                     //!< encryption.
	AES_DECRYPT = AES_DECRYPT_bm,    //!< decryption.
};

//! AES Auto Start Trigger settings
enum aes_auto{
	AES_MANUAL,                      //!< manual start mode.
	AES_AUTO = AES_AUTO_bm,          //!< auto start mode.
};

//! AES State XOR Load Enable settings
enum aes_xor{
	AES_XOR_OFF,                    //!< state direct load.
	AES_XOR_ON = AES_XOR_bm,        //!< state XOR load.
};

//! AES Interrupt Enable / Level settings
enum aes_intlvl{
	AES_INTLVL_OFF = AES_INTLVL_OFF_gc,  //!< Interrupt Disabled
	AES_INTLVL_LO  = AES_INTLVL_LO_gc,   //!< Low Level
	AES_INTLVL_MED = AES_INTLVL_MED_gc,  //!< Medium Level
	AES_INTLVL_HI  = AES_INTLVL_HI_gc,   //!< High Level
};

//! AES interrupt callback function pointer.
typedef void (*aes_callback_t)(void);

/** @} */

/**
 * \name AES Function Define
 *
 * @{
 */

//! Start a decryption/encryption operation.
static inline void aes_start(void)
{
	AES.CTRL |= AES_START_bm;
}
//! Reset all registers in AES module.
static inline void aes_software_reset(void)
{
	AES.CTRL = AES_RESET_bm;
}

/**
 * \brief Check if the AES module is busy.
 *
 * This functions determines whether the AES module is busy by checking if it
 * is currently performing a encryption/decryption operation or an error has
 * occurred. If neither of these cases are true, the module is determined to be
 * busy.
 *
 * If an error has occurred, the flag must be cleared with the function
 * \ref aes_clear_error_flag().
 */
static inline bool aes_is_busy(void)
{
	return !(AES.STATUS & (AES_SRIF_bm | AES_ERROR_bm));
}

//! Check if the AES module has encountered an error.
static inline bool aes_is_error(void)
{
	return (AES.STATUS & AES_ERROR_bm);
}

/**
 * \brief Clear the AES state ready interrupt flag.
 *
 * \sa aes_software_reset() to reset entire module and all flags.
 */
static inline void aes_clear_interrupt_flag(void)
{
	AES.STATUS |= AES_SRIF_bm;
}

/**
 * \brief Clear the AES error flag.
 *
 * \sa aes_software_reset() to reset entire module and all flags.
 */
static inline void aes_clear_error_flag(void)
{
	AES.STATUS |= AES_ERROR_bm;
}

void aes_configure(enum aes_dec decrypt, enum aes_auto auto_start,
	enum aes_xor xor_mode);

void aes_set_key(t_key k_in);

void aes_get_key(t_key k_out);

void aes_write_inputdata(t_data d_in);

void aes_read_outputdata(t_data d_out);

void aes_isr_configure(enum aes_intlvl intlvl);

void aes_set_callback(aes_callback_t callback);

/** @} */

/** @} */

/**
 * \page xmega_aes_quickstart Quick start guide for XMEGA AES driver
 *
 * This is the quick start guide for the \ref aes_group "AES Driver", with
 * step-by-step instructions on how to configure and use the driver for
 * specific use cases.
 *
 * The section described below can be compiled into e.g. the main application
 * loop or any other function that might use the AES functionality.
 *
 * \section xmega_aes_quickstart_basic Basic use case of the AES driver
 * In our basic use case, the AES driver is used to manually encrypt a block
 * of code with AES encryption using a predefined key, and subsequently
 * decrypt it using the same key.
 *
 * \subsection xmega_aes_quickstart_prereq Prerequisites
 * The \ref sysclk_group module is required to enable the clock to the
 * AES module.
 *
 * \section xmega_aes_quickstart_setup Setup
 * When the \ref sysclk_group module has been included, it must be initialized:
 * \code
	sysclk_init();
\endcode
 *
 * Subsequently, the clock to the AES module must be started:
 * \code
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_AES);
\endcode
 * \note The example code below assumes that this setup has been done.
 *
 * \section xmega_aes_quickstart_use_case Use case
 *
 * \subsection xmega_aes_quickstart_use_case_example_code Example code
 *
 * \code
	 t_key encryption_key = {
	     0x30, 0x70, 0x97, 0x1A, 0xB7, 0xCE, 0x45, 0x06,
	     0x3F, 0xD2, 0x57, 0x3F, 0x49, 0xF5, 0x42, 0x0D
	 };

	 t_data encryption_data = {
	     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	     0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
	 };

	 t_data encrypted_data;
	 t_data decrypted_data;

	 void encrypt_decrypt() {
	     aes_software_reset();
	     aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);

	     aes_set_key(encryption_key);
	     aes_write_inputdata(encryption_data);

	     aes_start();

	     do {
	         // Wait until AES is finished or an error occurs.
	     } while (aes_is_busy());

	     aes_read_outputdata(encrypted_data);

	     aes_configure(AES_DECRYPT, AES_MANUAL, AES_XOR_OFF);
	     aes_write_inputdata(encrypted_data);

	     aes_start();

	     do {
	         // Wait until AES is finished or an error occurs.
	     } while (aes_is_busy());

	     aes_read_outputdata(decrypted_data);
	 }
\endcode
 *
 * \subsection xmega_aes_quickstart_use_case_workflow Workflow
 *
 * We first define our encryption key, and some encryption data,
 * along with variables to hold encrypted and decrypted data.
 *
 * \code
	 t_key encryption_key = {
	     0x30, 0x70, 0x97, 0x1A, 0xB7, 0xCE, 0x45, 0x06,
	     0x3F, 0xD2, 0x57, 0x3F, 0x49, 0xF5, 0x42, 0x0D
	 };

	 t_data encryption_data = {
	     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	     0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
	 };

	 t_data encrypted_data;
	 t_data decrypted_data;
\endcode
 * \note These variables are of type \ref t_key and \ref t_data, which are
 *       defined as an \ref AES_KEY_SIZE and \ref AES_DATA_SIZE large block
 *       of unsigned 8 bit integers.
 *
 * Inside our function, we first do a call to \ref aes_software_reset(),
 * to make sure that the AES driver is ready for use:
 * \code
	aes_software_reset();
\endcode
 *
 * We configure the driver for manual encryption, with no XOR-ing:
 * \code
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);
\endcode
 *
 * We tell the driver where the key and the encryption data is located:
 * \code
	aes_set_key(encryption_key);
	aes_write_inputdata(encryption_data);
\endcode
 *
 * We are now ready to start encryption, and since we have selected
 * manual triggering, we trigger the module to start by calling
 * \ref aes_start():
 * \code
	aes_start();
\endcode
 *
 * We then wait until the module is finished, by checking the status register:
 * \code
	do {
	    // Wait until AES is finished or an error occurs.
	} while (aes_is_busy());
\endcode
 *
 * When it is done, we can read out our result using
 * \ref aes_read_outputdata():
 * \code
	aes_read_outputdata(encrypted_data);
\endcode
 *
 * Our encrypted data is now stored in the variable encrypted_data.
 *
 * We will now decrypt the data, and the procedure is very similar. We first
 * set up the driver to do decryption, manually triggered with no XOR-ing:
 * \code
	aes_configure(AES_DECRYPT, AES_MANUAL, AES_XOR_OFF);
\endcode
 *
 * We tell it where our encrypted data is located:
 * \code
	aes_write_inputdata(encrypted_data);
\endcode
 * \note As we have not called a software reset, the key is still stored in the
 *       module, so we do not need to do it again.
 *
 * And tell it to start decryption:
 * \code
	aes_start();
\endcode
 *
 * And wait until it is finished:
 * \code
	do {
	    // Wait until AES is finished or an error occurs.
	} while (aes_is_busy());
\endcode
 *
 * We read the output:
 * \code
	aes_read_outputdata(decrypted_data);
\endcode
 *
 * The decrypted data is now stored in the variable decrypted_data, and is
 * identical to that stored in the variable encryption_data.
 */
#endif  // AES_H

