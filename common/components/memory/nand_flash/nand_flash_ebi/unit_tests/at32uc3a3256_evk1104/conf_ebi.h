/*****************************************************************************
 *
 * \file
 *
 * \brief CONF_EBI EBI/SMC driver for AVR32 UC3.
 *
 * \note The values defined in this file are device-specific. See the device
 *       datasheet for further information.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _CONF_EBI_H_
#define _CONF_EBI_H_

#include "compiler.h"
#include "board.h"

#if (NF_SMC_USE_NCS == 3)
#define  SMC_USE_NCS3
#define  SMC_COMPONENT_CS3          NF_SMC_COMPONENT_CS
#else
#error This board is not supported
#endif

#define EBI_DATA_0                  NF_EBI_DATA_0
#define EBI_DATA_1                  NF_EBI_DATA_1
#define EBI_DATA_2                  NF_EBI_DATA_2
#define EBI_DATA_3                  NF_EBI_DATA_3
#define EBI_DATA_4                  NF_EBI_DATA_4
#define EBI_DATA_5                  NF_EBI_DATA_5
#define EBI_DATA_6                  NF_EBI_DATA_6
#define EBI_DATA_7                  NF_EBI_DATA_7

#define EBI_ADDR_21                 NF_EBI_ADDR_21
#define EBI_ADDR_22                 NF_EBI_ADDR_22

#define EBI_NANDOE                  NF_EBI_NANDOE
#define EBI_NANDWE                  NF_EBI_NANDWE

#endif  // _CONF_EBI_H_
