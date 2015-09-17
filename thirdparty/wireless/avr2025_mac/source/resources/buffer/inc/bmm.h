/**
 * @file bmm.h
 *
 * @brief This file contains the Buffer Management Module definitions.
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef BMM_INTERFACE_H
#define BMM_INTERFACE_H

/* === Includes ============================================================ */

#include <stdint.h>
#include <stdbool.h>

/**
 * \defgroup group_resources  Resource Management
 * The Resource Management provides access to resources  to the stack or the
 * application.
 *  @{
 */

/**
 * \defgroup group_bmm  Buffer Management
 * Buffer Management (large and small buffers): provides services for
 * dynamically
 * allocating and freeing memory buffers.
 *  @{
 */

/* === Macros ============================================================== */

/**
 * This macro provides the pointer to the corresponding body of the supplied
 * buffer header.
 */

#define BMM_BUFFER_POINTER(buf)  ((buf)->body)

/* === Types =============================================================== */

/**
 * @brief Buffer structure holding information of each buffer.
 *
 */
__PACK__DATA__
typedef struct
#if !defined(__DOXYGEN__)
		buffer_tag
#endif
{
	/** Pointer to the buffer body */
	uint8_t *body;

	/** Pointer to next free buffer */
	struct buffer_tag *next;
} buffer_t;
__PACK__RST_DATA__
/* === Externals =========================================================== */

/* === Prototypes ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the buffer module.
 *
 * This function initializes the buffer module.
 * This function should be called before using any other functionality
 * of buffer module.
 *
 * @ingroup apiResApi
 */
void bmm_buffer_init(void);

/**
 * @brief Allocates a buffer
 *
 * This function allocates a buffer and returns a pointer to the buffer.
 * The same pointer should be used while freeing the buffer.User should
 * call BMM_BUFFER_POINTER(buf) to get the pointer to buffer user area.
 *
 * @param size size of buffer to be allocated.
 *
 * @return pointer to the buffer allocated,
 *  NULL if buffer not available.
 *
 * @ingroup apiResApi
 */
#if defined(ENABLE_LARGE_BUFFER)
buffer_t *bmm_buffer_alloc(uint16_t size);

#else
buffer_t *bmm_buffer_alloc(uint8_t size);

#endif

/**
 * @brief Frees up a buffer.
 *
 * This function frees up a buffer. The pointer passed to this function
 * should be the pointer returned during buffer allocation. The result is
 * unpredictable if an incorrect pointer is passed.
 *
 * @param pbuffer Pointer to buffer that has to be freed.
 *
 * @ingroup apiResApi
 */
void bmm_buffer_free(buffer_t *pbuffer);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BMM_INTERFACE_H */

/* EOF */
