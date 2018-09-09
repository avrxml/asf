/*****************************************************************************
 *
 * \file
 *
 * \brief AES software driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the AES on AVR32 devices.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _AES_H_
#define _AES_H_

/**
 * \defgroup group_avr32_drivers_aes AES - Advanced Encryption Standard
 *
 * Driver for the AES (Advanced Encryption Standard) module.
 * Provides functions for configuring and initiating ciphering/deciphering with AES algorithms.
 * Several modes (ECB, CBC, OFB, CFB or CTR) and key sizes (128-, 192- or 256-bit) are supported,
 * in addition to manual or DMA-based data input to the AES module.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"


//! AES Processing mode
#define AES_PMODE_DECIPHER      0         // decipher
#define AES_PMODE_CIPHER        1         // cipher


//! AES Start mode
#define AES_START_MODE_MANUAL   0         // Manual mode
#define AES_START_MODE_AUTO     1         // Auto mode
#define AES_START_MODE_DMA      2         // DMA mode

//! AES Cryptographic key size
#define AES_KEY_SIZE_128        0         // 128bit
#define AES_KEY_SIZE_192        1         // 192bit
#define AES_KEY_SIZE_256        2         // 256bit

//! AES Operation cipher mode
#define AES_ECB_MODE            0         // Electronic Code Book mode
#define AES_CBC_MODE            1         // Cipher Block Chaining mode
#define AES_OFB_MODE            2         // Output FeedBack mode
#define AES_CFB_MODE            3         // Cipher FeedBack mode
#define AES_CTR_MODE            4         // Counter mode

//! AES URAD Type
#define AES_URAT_INPUTWRITE_DMA     0     // Input Data Register written during the data processing in DMA mode
#define AES_URAT_OUTPUTREAD_PROCESS 1     // Output Data Register read during the data processing
#define AES_URAT_MRWRITE_PROCESS    2     // Mode Register written during the data processing
#define AES_URAT_OUTPUTREAD_SUBKEY  3     // Output Data Register read during the sub-keys generation
#define AES_URAT_MRWRITE_SUBKEY     4     // Mode Register written during the sub-keys generation
#define AES_URAT_READ_WRITEONLY     5     // Write-only register read access


//! AES Configuration structure.
typedef struct
{
  unsigned char   ProcessingMode;         // O=decipher data, 1=cipher data
  unsigned char   ProcessingDelay;        // [0,15]
  unsigned char   StartMode;              // 0 = Manual mode, 1 = Auto mode, 2 = DMA mode
  unsigned char   KeySize;                // 0 = 128bits, 1 = 192bits, 2 = 256bits
  unsigned char   OpMode;                 // 0 = ECB, 1 = CBC, 2 = OFB, 3 = CFB, 4 = CTR
  unsigned char   LodMode;                // 0/1
  unsigned char   CFBSize;                // 0 = 128bits, 1 = 64bits, 2 = 32bits, 3 = 16bits, 4 = 8bits
  unsigned char   CounterMeasureMask;     // [0,31], bit=0 means CounterMeasure is disabled.
}aes_config_t;


//! AES Interrupt configuration structure.
typedef struct
{
  unsigned char   datrdy;                 // ENABLE/DISABLE; Data ready interrupt
  unsigned char   urad;                   // ENABLE/DISABLE; Unspecified Register Access Detection
}aes_isrconfig_t;



/*! \brief Configure the AES
 *
 * \param  *aes         Base address of the AES
 * \param  *pAesConfig  Parameters for the AES configuration
 *
 * \note See the aes_config_t structure definition for the meaning of the parameters.
 *
 */
void aes_configure( volatile avr32_aes_t *aes, const aes_config_t *pAesConfig );


/*! \brief Configure the AES interrupts.
 *
 * \param  *aes             Base address of the AES
 * \param  *pAesIsrConfig   Parameters for the AES interrupts configuration
 *
 * \note See the aes_isrconfig_t structure definition for the meaning of the parameters.
 *
 */
extern void aes_isr_configure( volatile avr32_aes_t *aes, const aes_isrconfig_t *pAesIsrConfig );


/*! \brief Get the AES status.
 *
 * \param  *aes             Base address of the AES
 *
 * \return the content of the AES_ISR register
 *
 */
extern unsigned int aes_get_status( volatile avr32_aes_t *aes );


/*! \brief Start the cipher/decipher process [manual mode only].
 *
 * \param  *aes             Base address of the AES
 *
 */
__always_inline static void aes_start( volatile avr32_aes_t *aes )
{
  aes->cr = AVR32_AES_CR_START_MASK;
}


/*! \brief Perform a software reset of the AES.
 *
 * \param  *aes         Base address of the AES
 *
 */
__always_inline static void aes_swreset( volatile avr32_aes_t *aes )
{
  aes->cr = AVR32_AES_CR_SWRST_MASK;
}


/*! \brief Set the 128/192/256bit cryptographic key.
 *
 * \param  *aes       Base address of the AES
 * \param  *pKey      Pointer on 4/6/8 contiguous 32bit words.
 *
 * \note The key size depends on the AES configuration (KeySize parameter of the aes_config_t).
 *
 */
extern void aes_set_key( volatile avr32_aes_t *aes, const unsigned int *pKey);


/*! \brief Set the 128bit initialization vector (for the CBC, CFB, OFB & CTR cipher modes)
 *
 * \param  *aes       Base address of the AES
 * \param  *pVector   Pointer on 4 contiguous 32bit words.
 *
 */
extern void aes_set_initvector( volatile avr32_aes_t *aes, const unsigned int *pVector);


/*! \brief Write the 128/64/32/16/8bit input data.
 *
 * \param  *aes      Base address of the AES
 * \param  *pIn      Pointer on 4/2/1 contiguous 32bit words.
 *
 */
extern void aes_write_inputdata( volatile avr32_aes_t *aes, const unsigned int *pIn);


/*! \brief Read the 128/64/32/16/8bit input data.
 *
 * \param  *aes      Base address of the AES
 * \param  *pOut     Pointer on 4/2/1 pre-allocated contiguous 32bit words.
 *
 */
extern void aes_read_outputdata( volatile avr32_aes_t *aes, unsigned int *pOut);



/*! \brief Trigger the load of a new seed in the AES internal random number generator (used for security counter measures).
 *
 * \param  *aes         Base address of the AES
 *
 */
__always_inline static void aes_load_newseed(volatile avr32_aes_t *aes)
{
  aes->cr = AVR32_AES_CR_LOADSEED_MASK;
}

/**
 * \}
 */

#endif  // _AES_H_
