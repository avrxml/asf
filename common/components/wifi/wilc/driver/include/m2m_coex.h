/**
 *
 * \file
 *
 * \brief WILC IoT Application Interface.
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

#ifndef __M2M_COEX_H__
#define __M2M_COEX_H__

#ifdef CONF_WILC_USE_3000

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "common/include/nm_common.h"
#include "driver/include/m2m_types.h"



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/**@defgroup  CoexEnums Enumeration/Typedef
 * @ingroup m2m_coex
 * @{
 */
/*!
@enum	\
	tenuCoexMode

@brief
	Enumeration for Coexistence mode 
	The following types are used to specify which mode a WILC would operate in.
	
@remarks
	The operation mode is applicable only to WILC devices that supports multiple radio standards
	e.g. Wifi and BT.
*/
typedef enum {
	M2M_COEX_MODE_WIFI            = 0x00,
	/*!< Force Wi-Fi only mode.
	*/
	M2M_COEX_MODE_BT               = 0x01,
	/*!< Force BT only mode.
	*/
	M2M_COEX_MODE_COMBO        = 0x02,
	/*!< Force Wifi/BT combo mode.
	*/
}tenuCoexMode;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup COEXAPI Function
 *   @ingroup m2m_coex
 */
#ifdef __cplusplus
     extern "C" {
#endif
 
 /**@}*/
 /** @defgroup CoexSetModeFn m2m_coex_set_mode
 *  @ingroup COEXAPI
 *   Synchronous function to set the coexistence mode.
 *   Dinitializes the host interface and frees resources used by the M2M_WIFI layer.
 *   Must be called in the application closing function, to ensure that all resources have been correctly released. 
 *   No arguments are expected to be passed in.
 */
/**@{*/
/*!
@fn	\
	NMI_API sint8  m2m_coex_set_mode(tenuCoexMode* enuCoexMode);
	
@param [in]	enuCoexMode
		Required mode of operation for coexistence.
@return		
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8  m2m_coex_set_mode(tenuCoexMode enuCoexMode);

#ifdef __cplusplus
}

#endif /* CONF_WILC_USE_3000*/
#endif
#endif /* __M2M_COEX_H__ */
