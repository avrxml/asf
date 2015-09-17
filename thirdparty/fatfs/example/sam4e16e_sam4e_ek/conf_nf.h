/**
 * \file
 *
 * \brief NAND Flash configuration.
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

#ifndef CONF_NF_H_INCLUDED
#define CONF_NF_H_INCLUDED

/* Number of tries for erasing a block */
#define CONF_NF_ERASE_NUM_TRY           2
/* Number of tries for writing a block */
#define CONF_NF_WRITE_NUM_TRY           2
/* Number of tries for copying a block */
#define CONF_NF_COPY_NUM_TRY            2

/* Maximum number of blocks in a device. */
#define NAND_COMMON_MAX_BLOCK_NUMBER       2048

/* Maximum number of pages in one block. */
#define NAND_COMMON_MAX_PAGES_PER_BLOCK    256

/* Maximum size of the data area of one page, in bytes. */
#define NAND_COMMON_MAX_PAGE_SIZE          2048

/* Maximum size of the spare area of one page, in bytes. */
#define NAND_COMMON_MAX_PAGE_SPARE_SIZE    128

/* Maximum number of ecc bytes stored in the spare for one single page. */
#define NAND_COMMON_MAX_SPARE_ECC_BYTES    48

/* Maximum number of extra free bytes inside the spare area of a page. */
#define NAND_COMMON_MAX_SPARE_EXTRA_BYTES  78

#endif /* CONF_NF_H_INCLUDED */
