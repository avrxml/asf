/**
 * @file stb_armcrypto.h
 *
 * @brief Declarations for security tool box required for ARM crypto engine
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
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef STB_ARMCRYPTO_H
#define STB_ARMCRYPTO_H

#if defined(STB_ARMCRYPTO) || defined(__DOXYGEN__)

/* === Includes =========================================================== */

#include "pal.h"

/* === Macros ============================================================= */

/**
 * Size of AES blocks
 */
#define AES_BLOCKSIZE               (16)

/**
 * Size of AES key
 */
#define AES_KEYSIZE                 (16)

/* Values for SR_AES_DIR */
#ifndef AES_DIR_ENCRYPT

/**
 * Defines AES direction as encryption
 */
#define AES_DIR_ENCRYPT              (0)
#endif
#ifndef AES_DIR_DECRYPT

/**
 * Defines AES direction as decryption
 */
#define AES_DIR_DECRYPT              (1)
#endif

/* === Types ============================================================== */

/* === Externals ========================================================== */

/* === Prototypes ========================================================= */

#ifdef __cplusplus
extern "C" {
#endif

void sal_aes_restart(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifdef STB_ARMCRYPTO */

#endif /* STB_ARMCRYPTO_H */
/* EOF */
