/**
 * \file
 *
 * \brief Configuration of the USI example application
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

#ifndef CONF_EXAMPLE_INCLUDED
#define CONF_EXAMPLE_INCLUDED

//! Maximum length of the input command
#define MAX_COMMAND_LENGTH  20

//! LED blinking rate in msec
#define COUNT_MS_SWAP_LED   500

//! \name TC configuration for msec timer
// @{
#define ID_TC_1MS		    ID_TC3
#define TC_1MS			    TC1
#define TC_1MS_CHN		  0
#define TC_1MS_IRQn		  TC3_IRQn
#define TC_1MS_Handler	TC3_Handler
// @}

#define STRING_EOL    "\r"
#define STRING_HEADER "-- ATMEL PLC USI Example Application --\r\n" \
		" -- "BOARD_NAME" --\r\n" \
		" -- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#define PROGRAM_MAIN_MENU "Introduce command:\n"STRING_EOL



#endif /* CONF_EXAMPLE_INCLUDED */
