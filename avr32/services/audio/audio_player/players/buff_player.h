/**************************************************************************
 *
 * \file
 *
 * \brief Bufferization player.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _BUFF_PLAYER_H_
#define _BUFF_PLAYER_H_

#include <stddef.h>
#include "compiler.h"


extern bool buff_player_create(uint8_t nb_buffers, size_t buffer_size);
extern void *buff_player_malloc(size_t buffer_size);
extern void buff_player_dacs_setup(uint32_t sample_rate_hz,
                                   uint8_t num_channels,
                                   uint8_t bits_per_sample,
                                   bool swap_channels);
extern bool buff_player_dacs_output(void *sample_buffer, size_t sample_length);
extern void buff_player_dacs_end_of_pcm_buf(void);
extern void buff_player_dacs_flush(bool force);
extern void buff_player_close(void);
extern void buff_player_mute_audio(bool mute);

#endif  // _BUFF_PLAYER_H_
