/**
 * \file
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

#ifndef MULTI_LANGUAGE_DISPLAY_H_INCLUDED
#define MULTI_LANGUAGE_DISPLAY_H_INCLUDED

#include "compiler.h"

/* Interface functions. */
void multi_language_show_switch_info(void);
void multi_language_show_start_info(void);
void multi_language_show_temperature_info(void);
void multi_language_show_light_info(void);
void multi_language_show_end_info(void);
void multi_language_show_no_sd_info(void);
void multi_language_show_sd_info(void);
void multi_language_show_normal_card_info(void);
void multi_language_show_high_capacity_card_info(void);
void multi_language_show_unknow_card_info(void);
void multi_language_show_card_size_info(char *p_string,
		uint32_t sd_card_size);
void multi_language_show_no_fatfs_info(void);
void multi_language_show_no_files_info(void);
void multi_language_show_browse_info(void);
void multi_language_show_file_name(uint8_t page,
		const char *string);

#endif /* MULTI_LANGUAGE_DISPLAY_H_INCLUDED*/
