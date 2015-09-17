/**
 * \file
 *
 * \brief Connection of the AT25DFx SerialFlash to SPI interface driver.
 *
 * This file manages the connection of the AT25DFx SerialFlash driver to an
 * SPI service (could be spi master service or usart in spi mode). The SPI
 * service selection depends on  AT25DFX_USES_SPI_MASTER_SERVICE or
 * AT25DFX_USES_USART_SPI_SERVICE in conf_at25dfx.h.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef AT25DFX_HAL_SPI_H_INCLUDED
#define AT25DFX_HAL_SPI_H_INCLUDED

#include "conf_at25dfx.h"
#include "status_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(AT25DFX_SPI_MODULE)
#warning AT25DFX_SPI_MODULE not defined, set default to 0
#define AT25DFX_SPI_MODULE 0
#endif
#if !defined(AT25DFX_CS)
#warning AT25DFX_CS not defined, set default to 0
#define AT25DFX_CS 0
#endif

void at25dfx_spi_init(void);
void at25dfx_spi_select_device(uint8_t mem_id);
void at25dfx_spi_deselect_device(uint8_t mem_id);
status_code_t at25dfx_spi_write_byte(uint8_t data);
status_code_t at25dfx_spi_read_byte(uint8_t *data);
status_code_t at25dfx_spi_read_packet(void const *data, size_t len);
status_code_t at25dfx_spi_write_packet(void const *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* AT25DFX_HAL_SPI_H_INCLUDED */
