/**
 * \file
 *
 * \brief Growl Client Interface.
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

#ifndef __NMI_GROWL_H__
#define __NMI_GROWL_H__

#include "common/include/nm_common.h"

#define PROWL_CLIENT					1
#define NMA_CLIENT						2

#define GROWL_EVENT_MAX_LENGTH			16
#define GROWL_DESCRIPTION_MAX_LENGTH	72
#define GROWL_APPNAME_MAX_LENGTH		16

/* Possible Error codes could be returned by the nma server. */
#define GROWL_SUCCESS					20			/*!< NMA notification sent successfully. Actual returned code = "200". */

#define GROWL_ERR_BAD_REQUEST			40			/*!< The sent notification has a format error. Actual returned code = "400". */
#define GROWL_ERR_NOT_AUTHORIZED		41			/*!< The API Key supplied with the request is invalid. Actual returned code = "401". */
#define GROWL_ERR_NOT_ACCEPTED			42			/*!< Maximum number of API calls per hour exceeded. Actual returned code = "402". */
#define GROWL_ERR_API_EXCEED			46			/*!< Actual returned code = "4069". */
#define GROWL_ERR_NOT_APPROVED			49			/*!< Actual returned code = "409". */
#define GROWL_ERR_SERVER_ERROR			50			/*!< Internal server error. Actual returned code = "500". */

#define GROWL_ERR_LOCAL_ERROR			30			/*!< An error occured on the m2m device due to internal problem. */

#define GROWL_ERR_CONN_FAILED			10			/*AFTER NUMBER OF ITERATION*/
#define GROWL_ERR_RESOLVE_DNS 			11

#define GROWL_RETRY						12

/*!
*  @typedef			tpfNmaResponseCallback
*  @brief				Callback invoked when the Nma notification response is received from the the NMA server.
*  @param [in]		u8ResponseCode
*						Response status code. It may be one of the following:
*						NMA_SUCCESS
*						NMA_ERR_BAD_REQUEST
*						NMA_ERR_NOT_AUTHORIZED
*						NMA_ERR_NOT_ACCEPTED
*						NMA_ERR_SERVER_ERROR
*  @author			Ahmed Ezzat
*  @date				2 Aug 2012
*  @version			1.0
*/

#ifdef __cplusplus
extern "C"
{
#endif

/*!
*  @fn				void NMI_GrowlInit(void)
*  @brief
*  @author			Dina El Sissy
*  @date			2 Jan 2013
*  @version			1.0
*/
NMI_API void NMI_GrowlInit(uint8 *pu8PrwKey,uint8 *pu8NmaKey);

/*!
*  @fn				sint8 NMI_SendNotification(char	*pcApp,char *pcEvent, char *pcDescription)
*  @brief			Send a specific notification to a Registered Nma device.
*  @param [in]		client
*						The name of teh Growl Client NMA or Prowl.
*  @param [in]		pcApp
*						Application which generate the notification.
*  @param [in]		pcEvent
*						Event triggered the notification.
*  @param [in]		pcDescription
*						Message describes the event happened.
*  @author			Ahmed Ezzat
*  @date			16 July 2012
*  @version			1.0
*/
NMI_API sint8 NMI_GrowlSendNotification
(
	uint8					u8ClientName,
	uint8					*pu8App,
	uint8					*pu8Event,
	uint8					*pu8Description,
	uint8					bUseSSL
);

NMI_API void GrowlCb(uint8 u8Code, uint8 u8ClientID);
NMI_API void NMI_GrowldeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __NMI_GROWL_H__ */
