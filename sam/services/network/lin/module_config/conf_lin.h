/**
 * \file
 *
 * \brief Usart LIN driver configuration file.
 *
 * This file contains basic functions for the SAM USART, with support for all
 * modes, settings and clock speeds.
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

#ifndef CONF_LIN_H_INCLUDED
#define CONF_LIN_H_INCLUDED

#ifndef USART_LIN_VERSION
	#warning USART LIN version configuration to be used in the driver is missing. Default configuration is used.
	/** LIN Version Supported LIN_13 or LIN_20 */
	#define USART_LIN_VERSION                   LIN_2x
#endif

#ifndef LIN_NODE_NUM
	#warning USART LIN node number used in the driver is missing. Default configuration is used.
	/** LIN node number */
	#define LIN_NODE_NUM                   2
#endif

#ifndef LIN_BUFF_SIZE
	#warning Size of LIN buffer used in the driver is missing. Default configuration is used.
	/** LIN buffer size */
	#define LIN_BUFF_SIZE                   256
#endif

/* NUMBER_OF_LIN_FRAMES_NODE */
#ifndef NUMBER_OF_LIN_FRAMES_NODE
	#warning LIN frame node in the driver is missing. Default configuration is used.
	#define NUMBER_OF_LIN_FRAMES_NODE    8
#endif

#endif  /* CONF_LIN_H_INCLUDED */
