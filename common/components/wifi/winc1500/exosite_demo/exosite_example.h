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
#ifndef ATMEL_EXOSITE_H_INCLUDE
#define ATMEL_EXOSITE_H_INCLUDE

#include <compiler.h>
#include "iot/http/http_client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*exosite_http_cb)(struct http_client_module *module_inst, int type, union http_client_data *data);

/*
*	\brief Initialize exosite-demo.
*/
bool	exosite_example_init(exosite_http_cb cb);

/*
*	\brief Send message to exosite server for activiate.
*
*	\param activate_data	device information data. Data format is "vendor=xxx&model=xxx&sn=xxx"
*
*	\return	true send message to server successfully.
*/
bool	exosite_example_activiate(char * activate_data);

/*
*	\brief Send message to exosite server for Io1 sensor information and request led state value.
*
*
*	\return	true send message to server. ex> temperature, light voltage. 
*/
bool	exosite_example_read_and_write(char * write_data, char * cik);

#ifdef __cplusplus
}
#endif

#endif /* ATMEL_EXOSITE_H_ */