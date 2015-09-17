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
#ifndef MEDIA_H_INCLUDED
#define MEDIA_H_INCLUDED

/** Media sources types (check first ~ last) */
typedef enum media_types {
#ifdef MEDIA_USE_SD
	MEDIA_SD,
#endif
#ifdef MEDIA_USE_MSD
	MEDIA_MSD,
#endif
#ifdef MEDIA_USE_NAND
	MEDIA_NAND,
#endif
#ifdef MEDIA_USE_ETH
	MEDIA_ETH,
#endif
#ifdef MEDIA_USE_COMM
	MEDIA_COMM,
#endif
	MEDIA_NUM_MAX
} media_types_t;
/** Media names for debug */
static const char *media_types_str[] = {
#ifdef MEDIA_USE_SD
	"SD/MMC",
#endif
#ifdef MEDIA_USE_MSD
	"MSD",
#endif
#ifdef MEDIA_USE_NAND
	"NAND"
#endif
#ifdef MEDIA_USE_ETH
	"ETH",
#endif
#ifdef MEDIA_USE_COMM
	"COMM",
#endif
};
/**
 * Return the media name
 * \return const string of media name
 */
__always_inline static const char* media_get_type_str(enum media_types media)
{
	return media_types_str[media];
}


void media_init(const char** file_check_list, const uint32_t list_size);
void media_set_file_name(const char* name);

void media_cleanup(void);

void media_select(enum media_types media);
bool media_connect(void);
void media_disconnect(void);
uint32_t media_load_file(void* addr, uint32_t size,
	uint8_t* block_buffer, uint32_t block_size,
	uint32_t (*save_data_exec)(void* dst,void* src,uint32_t size));
void media_scan_files(bool show_full_path);

#ifdef MEDIA_USE_SD
/* Media: SD card */
extern void media_sd_init(void);
extern void media_sd_cleanup(void);
extern bool media_sd_connect(void);
extern void media_sd_disconnect(void);
#endif


#ifdef MEDIA_USE_MSD
/* Media: MSD */
#define media_msd_init           0
#define media_msd_cleanup        0
#define media_msd_connect        0
#define media_msd_disconnect     0
#endif


#ifdef MEDIA_USE_NAND
/* Media: NAND */
#define media_nand_init          0
#define media_nand_cleanup       0
#define media_nand_connect       0
#define media_nand_disconnect    0
#endif


#ifdef MEDIA_USE_ETH
/* Media: ETHERNET */
#define media_eth_init           0
#define media_eth_cleanup        0
#define media_eth_connect        0
#define media_eth_disconnect     0
#endif


#ifdef MEDIA_USE_COMM
/* Media: COMM */
#define media_comm_init          0
#define media_comm_cleanup       0
#define media_comm_connect       0
#define media_comm_disconnect    0
#endif

#endif /* #ifndef MEDIA_H_INCLUDED */
