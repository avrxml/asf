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

#ifndef BOOTLOADER_H_INCLUDED
#define BOOTLOADER_H_INCLUDED

#include <asf.h>
#include <string.h>
#include "conf_bootloader.h"

/* Check configuration */

/* -- Information editor --- */
#ifdef DBG_USE_INFO_EDIT

#  ifndef DBG_USE_USART
#    error DBG_USE_USART must be defined to enable DBG console for InfoEdit
#  endif

#  ifndef DBG_INFO_EDIT_TRIGGER_PIN
#    error DBG_INFO_EDIT_TRIGGER_PIN must be defined for InfoEdit trigger
#  endif

#  ifdef TRIGGER_USE_BUTTONS
#    ifdef TRIGGER_LOAD_BUTTON
#      if TRIGGER_LOAD_BUTTON == DBG_INFO_EDIT_TRIGGER_PIN
#        error TRIGGER_LOAD_BUTTON and DBG_INFO_EDIT_TRIGGER_PIN must be different
#      endif
#    endif

#    ifdef TRIGGER_SWITCH_BUTTON
#      if TRIGGER_SWITCH_BUTTON == DBG_INFO_EDIT_TRIGGER_PIN
#        error TRIGGER_SWITCH_BUTTON and DBG_INFO_EDIT_TRIGGER_PIN must be different
#      endif
#    endif
#  endif
#endif

/* -- medias (currently SD supported) --- */
#if (!defined(MEDIA_USE_COMM) && \
	!defined(MEDIA_USE_ETH)   && \
	!defined(MEDIA_USE_SD)    && \
	!defined(MEDIA_USE_MSD)   && \
	!defined(MEDIA_USE_NAND))
#  error MEDIA_USE_SD must be defined
#endif

#ifdef MEDIA_USE_COMM
/* -- communication    */
#  ifndef COMM_USE_USART
#    error COMM_USE_USART and USART options must be defined 
#  endif
#  ifndef COMM_USE_XON_XOFF
#    error COMM_USE_XON_XOFF must be defined
#  endif
#endif
/* -- trigger          */
/* -- memory           */
#ifndef MEM_USE_FLASH
#  error MEM_USE_FLASH must be defined
#endif
/* -- regions          */


/* All include files for bootloader */
#include "debug.h"
#include "memories.h"
#include "regions.h"
#include "trigger.h"
#include "media.h"

void jump_to_app(void * code_addr);

/* Global tick in ms */
extern volatile uint32_t tick_ms;

#endif /* #ifndef BOOTLOADER_H_INCLUDED */

