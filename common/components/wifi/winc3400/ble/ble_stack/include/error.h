/**
 * \file
 *
 * \brief Error.
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

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "cmn_defs.h"

at_ble_status_t at_ble_att_error(uint8_t att_error_code);

at_ble_status_t at_ble_gap_error(uint8_t gap_error_code);

#define IS_ERR(__status__) (__status__ != AT_BLE_SUCCESS)

#define ERRORCHECK(__status__) do{\
	if(IS_ERR(__status__))\
	{\
		printf("ERRORCHECK(%d)\n", __status__);\
		goto ERRORHANDLER;\
	}\
}while(0)

#define ERRORREPORT(__status__, __err__) do{\
	printf("NMI_ERRORREPORT(%d)\n", __err__);\
	__status__ = __err__;\
	goto ERRORHANDLER;\
}while(0)

#define  NULLCHECK(__status__, __ptr__)	do{\
	if(__ptr__ == NULL)\
	{\
		ERRORREPORT(__status__, AT_BLE_FAILURE);\
	}\
}while(0)

#define CATCH(__status__) \
ERRORHANDLER :\
if(IS_ERR(__status__)) \

#endif /* ERROR_H_INCLUDED */
