/*
 * \file
 *
 * \brief WINC1500 Exosite Example.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
#ifndef DEBUG_CONF_H_INCLUDE
#define DEBUG_CONF_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif
	
/* Debug */
#define DEBUG(...)		printf(__VA_ARGS__)

#define DEBUG_EOL    "\r\n"
#define DEBUG_STRING_HEADER "-- WINC1500 chip & Exosite IoT Demo --"DEBUG_EOL \
"-- "BOARD_NAME " --"DEBUG_EOL	\
"-- Compiled: "__DATE__ " "__TIME__ " --"DEBUG_EOL

#define DEBUG_CONF_WIFI			"WINC1500: "
#define DEBUG_CONF_SOCKET		"SOCKET: "
#define DEBUG_CONF_IO1_BOARD	"IO1_BOARD: "
#define DEBUG_CONF_EXOSITE		"EXOSITE: "

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_CONF_H_INCLUDE */