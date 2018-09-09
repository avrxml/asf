/**
 * \file
 *
 * \brief AVR XMEGA Data Encryption Standard (DES) driver
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
#ifndef DES_H
#define DES_H

#include <compiler.h>
/**
 * \defgroup des_group Data Encryption Standard (DES)
 *
 * See \ref xmega_des_quickstart
 *
 * This is a driver for the AVR XMEGA DES core instruction. It provides
 * functions for des, 3des and CBC des encryption and decryption operations.
 *
 * \section des_satrt_condition DES Start Setting
 * DES is supported by a DES instruction in the AVR XMEGA core. The 8-byte key
 * and 8-byte data blocks must be loaded into the Register file, and then DES
 * must be executed 16 times to encrypt/decrypt the data block.
 *
 * \note The functions for loading configurations/data/key are not protected
 * against interrupts.
 *
 * \section Quick start guide
 * See \ref xmega_des_quickstart
 *
 * @{
 */

/**
 *  \brief  Function that does a DES decryption on one 64-bit data block.
 *
 *  \param  ciphertext  Pointer to the ciphertext that shall be decrypted.
 *  \param  plaintext   Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param  key         Pointer to the DES key.
 *
 */
void des_decrypt(uint8_t *ciphertext, uint8_t *plaintext, uint8_t *key);


/**
 *  \brief  Function that does a DES encryption on one 64-bit data block.
 *
 *  \param  plaintext  Pointer to the plaintext that shall be encrypted.
 *  \param  ciphertext Pointer to where in memory the ciphertext (answer) shall be stored.
 *  \param  key        Pointer to the DES key.
 *
 */
void des_encrypt(uint8_t *plaintext, uint8_t *ciphertext, uint8_t *key);


/**
 *  \brief  Function that does a 3DES encryption on one 64-bit data block.
 *
 *  \param  plaintext   Pointer to the plaintext that shall be encrypted.
 *  \param  ciphertext  Pointer to where in memory the ciphertext (answer) shall be stored.
 *  \param  keys        Pointer to the array of the 3 DES keys.
 *
 */
void des_3des_encrypt(uint8_t *plaintext, uint8_t *ciphertext, uint8_t *keys);


/**
 *  \brief  Function that does a 3DES decryption on one 64-bit data block.
 *
 *  \param  ciphertext  Pointer to the ciphertext that shall be decrypted.
 *  \param  plaintext   Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param  keys        Pointer to the array of the 3 DES keys.
 *
 */
void des_3des_decrypt(uint8_t *ciphertext, uint8_t *plaintext, uint8_t *keys);


/**
 *  \brief  Function that does DES Cipher Block Chaining encryption on a
 *          given number of 64-bit data block.
 *
 *  \param   plaintext    Pointer to the plaintext that shall be encrypted.
 *  \param   ciphertext   Pointer to where in memory the ciphertext(answer) shall be stored.
 *  \param   keys         Pointer to the array of the one or three DES keys needed.
 *  \param   init         Pointer to initial vector used in in CBC.
 *  \param   triple_DES   Bool that indicate if 3DES or DES shall be used.
 *  \param   block_length Value that tells how many blocks to encrypt.
 *
 */
void des_cbc_encrypt(uint8_t *plaintext, uint8_t *ciphertext,
                     uint8_t *keys, uint8_t *init,
                     bool triple_DES, uint16_t block_length);


/**
 *  \brief  Function that does DES Cipher Block Chaining decryption on a
 *          given number of 64-bit data block.
 *
 *  \param   ciphertext    Pointer to the ciphertext that shall be decrypted.
 *  \param   plaintext     Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param   keys          Pointer to the array of the one or three DES keys needed.
 *  \param   init          Pointer to initial vector used in in CBC.
 *  \param   triple_DES    Bool that indicate if 3DES or DES shall be used.
 *  \param   block_length  Value that tells how many blocks to encrypt.
 *
 *  \note    The pointer to the ciphertext and plaintext must not be the same
 *           because the CBC algorithm uses previous values of the ciphertext
 *           to calculate the plaintext.
 */
void des_cbc_decrypt(uint8_t *ciphertext, uint8_t *plaintext,
                     uint8_t *keys, uint8_t *init,
                     bool triple_DES, uint16_t block_length);

/* @}*/

/**
 * \page xmega_des_quickstart Quick start guide for XMEGA DES driver
 *
 * This is the quick start guide for the \ref des_group "DES Driver", with
 * step-by-step instructions on how to configure and use the driver for
 * specific use cases.
 *
 * The section described below can be compiled into e.g. the main application
 * loop or any other function that might use the DES functionality.
 *
 * \section xmega_des_quickstart_basic Basic use case of the DES driver
 *
 * In our basic use case, the DES driver is used to encrypt a 64 * 8 byte
 * block of data with DES and 3DES encryption using a predefined key, and
 * subsequently decrypt it. The DES driver does DES encryption and decryption
 * on 64 bits of data at once, using a 64 bit key. 3DES encryption and
 * decryption is done using three 64 bit keys. Therefore, the functions
 * are called on 8 byte chunks of the code.
 *
 * \subsection xmega_des_quickstart_prereq Prerequisites
 * There are no prerequisites for using this code.
 *
 * \section xmega_des_quickstart_setup Setup
 * There is no setup needed for this driver, all encryption/decryption
 * functions can be used instantly.
 *
 * \section xmega_des_quickstart_use_case Use case
 *
 * \subsection xmega_des_quickstart_use_case_example_code Example code
 *
 * \code
	 #define DES_BLOCK_SIZE 8
	 #define DATA_BLOCKS    64
	 #define DATA_SIZE      DATA_BLOCKS * DES_BLOCK_SIZE

	 uint8_t keys[DES_BLOCK_SIZE * 3] = {
	     0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
	     0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2,
	     0xFE, 0x98, 0x6C, 0x38, 0x23, 0xFC, 0x2D, 0x23
	 };

	 uint16_t i;

	 uint8_t data[DATA_SIZE];
	 uint8_t encrypted_data_des[DATA_SIZE];
	 uint8_t encrypted_data_3des[DATA_SIZE];

	 uint8_t decrypted_data_des[DATA_SIZE];
	 uint8_t decrypted_data_3des[DATA_SIZE];

	 for (i = 0; i < DATA_SIZE; i++) {
	     data[i] = (uint8_t)i;
	 }

	 for (i = 0; i < DATA_BLOCKS; i++) {
	     uint8_t *current_data_block = (data + DES_BLOCK_SIZE * i);
	     uint8_t *current_des_enc_block =
	             (encrypted_data_des + DES_BLOCK_SIZE * i);
	     uint8_t *current_3des_enc_block =
	             (encrypted_data_3des + DES_BLOCK_SIZE * i);

	     des_encrypt(current_data_block, current_des_enc_block, keys);
	     des_3des_encrypt(current_data_block, current_3des_enc_block, keys);
	 }

	 for (i = 0; i < DATA_BLOCKS; i++) {
	     uint8_t *current_des_enc_block =
	             (encrypted_data_des + DES_BLOCK_SIZE * i);
	     uint8_t *current_3des_enc_block =
	             (encrypted_data_3des + DES_BLOCK_SIZE * i);
	     uint8_t *current_des_dec_block =
	             (decrypted_data_des + DES_BLOCK_SIZE * i);
	     uint8_t *current_3des_dec_block =
	             (decrypted_data_3des + DES_BLOCK_SIZE * i);

	     des_decrypt(current_des_enc_block, current_des_dec_block, keys);
	     des_3des_decrypt(current_3des_enc_block, current_3des_dec_block, keys);
	 }
\endcode
 *
 * \subsection xmega_des_quickstart_use_case_workflow Workflow
 *
 * We first define the size of a DES block (8 bytes, 64 bits), the amount of
 * data blocks we will use, and the total size of our data block:
 * \code
	#define DES_BLOCK_SIZE 8
	#define DATA_BLOCKS    64
	#define DATA_SIZE      DATA_BLOCKS * DES_BLOCK_SIZE
\endcode
 *
 * The DES and 3DES functions require keys to be able to encrypt data, so we
 * create three 64 bit (8 * 8 bit) keys for them to use:
 * \code
	uint8_t keys[DES_BLOCK_SIZE * 3] = {
	    0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
	    0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2,
	    0xFE, 0x98, 0x6C, 0x38, 0x23, 0xFC, 0x2D, 0x23
	};
\endcode
 * \note Only the first key is used for the DES routine.
 *
 * We create a data block which we will encrypt, and destination blocks for the
 * encrypted and decrypted data:
 * \code
	 uint16_t i;

	 uint8_t data[DATA_SIZE];
	 uint8_t encrypted_data_des[DATA_SIZE];
	 uint8_t encrypted_data_3des[DATA_SIZE];

	 uint8_t decrypted_data_des[DATA_SIZE];
	 uint8_t decrypted_data_3des[DATA_SIZE];
\endcode
 *
 * We fill our data block with some data:
 * \code
	for (i = 0; i < DATA_SIZE; i++) {
	    data[i] = (uint8_t)i;
	}
\endcode
 *
 * Now we will encrypt our data with both DES and 3DES encryption. The
 * encrypted values will be stored in the variables encrypted_data_des and
 * encrypted_data_3des.
 * We loop through all the blocks:
 * \code
	for (i = 0; i < DATA_BLOCKS; i++) {
\endcode
 *
 * And create pointers to blocks currently being processed:
 * \code
	uint8_t *current_data_block = (data + DES_BLOCK_SIZE * i);
	uint8_t *current_des_enc_block =
	        (encrypted_data_des + DES_BLOCK_SIZE * i);
	uint8_t *current_3des_enc_block =
	        (encrypted_data_3des + DES_BLOCK_SIZE * i);
\endcode
 *
 * Finally we call \ref des_encrypt() and \ref des_3des_encrypt() on the
 * data:
 * \code
	    des_encrypt(current_data_block, current_des_enc_block, keys);
	    des_3des_encrypt(current_data_block, current_3des_enc_block, keys);
	}
\endcode
 * \note Only the first key is used in the \ref des_encrypt() function.
 *
 * The encrypted data is now stored in the two variables. We will now decrypt
 * the data in a similar fashion, storing the result in the variables
 * decrypted_data_des and decrypted_data_3des.
 *
 * Looping through the blocks:
 * \code
	for (i = 0; i < DATA_BLOCKS; i++) {
\endcode
 *
 * Pointing to the current blocks being processed:
 * \code
	uint8_t *current_des_enc_block =
	        (encrypted_data_des + DES_BLOCK_SIZE * i);
	uint8_t *current_3des_enc_block =
	        (encrypted_data_3des + DES_BLOCK_SIZE * i);
	uint8_t *current_des_dec_block =
	        (decrypted_data_des + DES_BLOCK_SIZE * i);
	uint8_t *current_3des_dec_block =
	        (decrypted_data_3des + DES_BLOCK_SIZE * i);
\endcode
 *
 * And calling the decrypt functions:
 * \code
	    des_decrypt(current_des_enc_block, current_des_dec_block, keys);
	    des_3des_decrypt(current_3des_enc_block, current_3des_dec_block, keys);
	}
\endcode
 *
 */

#endif /* DES_H */

