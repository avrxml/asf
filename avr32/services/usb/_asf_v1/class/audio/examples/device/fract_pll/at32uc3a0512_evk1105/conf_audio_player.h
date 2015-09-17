/**************************************************************************
 *
 * \file
 *
 * \brief Audio player configuration file.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _CONF_AUDIO_PLAYER_H_
#define _CONF_AUDIO_PLAYER_H_

#include "audio_mixer.h"

#include <avr32/io.h>
#include "board.h"


/*! \name System Clock Frequencies
 */
//! @{
#define SYS_CLOCK_PLL_MUL             10
#define FMCK_HZ(fout)                 ((SYS_CLOCK_PLL_MUL * (fout) + 1) / 2)
#define FCPU_HZ                       g_fcpu_hz
#define FHSB_HZ                       g_fhsb_hz
#define FPBB_HZ                       g_fpbb_hz
#define FPBA_HZ                       g_fpba_hz
//! @}

//! CPU frequency
extern uint32_t g_fcpu_hz;

//! HSB frequency
extern uint32_t g_fhsb_hz;

//! PBA frequency
extern uint32_t g_fpba_hz;

//! PBB frequency
extern uint32_t g_fpbb_hz;

//! Default heap initialization word.
#define DEFAULT_HEAP_INIT_WORD        0xA5A5A5A5

//! Time-out period of the WatchDog Timer in microseconds.
//!
//! \warning Do not choose a value smaller than the normal maximal time that can
//!          be spent between two consecutive calls of com_task().
#define WDT_TIMEOUT_PERIOD_US                   15000000

/*! \name Default DAC Settings
 * Please choose between:
 * - AUDIO_MIXER_DAC_AIC23B (external DAC mounted on EVK1105 board)
 * - AUDIO_MIXER_DAC_ABDAC  (internal ABDAC with external amplifier tap6130
 *   on EVK1105 and EVK1104)
 */
//! @{
#define DEFAULT_DACS                  AUDIO_MIXER_DAC_AIC23B
#define DEFAULT_DAC_SAMPLE_RATE_HZ    SPEAKER_FREQUENCY
#define DEFAULT_DAC_NUM_CHANNELS      2
#define DEFAULT_DAC_BITS_PER_SAMPLE   16
#define DEFAULT_DAC_SWAP_CHANNELS     false
//! @}

/*! \name USB resynchronization methods
 */
//! @{
//! Add/remove samples on the fly.
#define USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES  1

//! Pseudo adaptive Sampling Rate Conversion.
#define USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC      2

//! Use of an external clock synthesizer (e.g. the CL CS2200).
#define USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER 8
//! @}

/*! \name Method used for the USB resynchronization
 * Please choose between:
 * - USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES  (Add/remove samples on the fly)
 * - USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC      (Pseudo adaptive Sampling Rate Conversion)
 * - USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER (Use of an external clock synthesizer (e.g. the CL CS2200))
 */
#define USB_RESYNC_METHOD  USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER

#endif  // _CONF_AUDIO_PLAYER_H_
