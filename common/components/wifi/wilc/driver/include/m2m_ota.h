/**
 *
 * \file
 * \brief NMC1500 IoT OTA Interface.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef __M2M_OTA_H__
#define __M2M_OTA_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "common\include\nm_common.h"
#include "driver\include\m2m_types.h"
/**@defgroup  OTATYPEDEF  WLANTypedef
 * @ingroup WlanEnums
 */
 /* @{ */


/*!
@typedef void (*tpfOtaNotifCb) (tstrOtaUpdateInfo *);

@brief A callback to get notification about an potential OTA update.
 
@param[in] pstrOtaUpdateInfo A structure to provide notification payload.

@sa 
	tstrOtaUpdateInfo
@warning 
		the notification is not supported (Not implemented yet)

*/
typedef void (*tpfOtaNotifCb) (tstrOtaUpdateInfo * pstrOtaUpdateInfo);


/*!
@typedef void (*tpfOtaUpdateCb) (uint8 u8OtaUpdateStatusType ,uint8 u8OtaUpdateStatus);

@brief A callback to get OTA status update, the callback provide the status type and its status
	the OTA callback provide the Download status, the switch to the downloaded firmware status and Roll-back status 
 
@param[in] u8OtaUpdateStatusType possible values are listed in tenuOtaUpdateStatusType. Possible types are:
- [DL_STATUS](@ref DL_STATUS)
- [SW_STATUS](@ref SW_STATUS)
- [RB_STATUS](@ref RB_STATUS)

@param[in] u8OtaUpdateStatus possible values are listed in tenuOtaUpdateStatus. 

@see
	tenuOtaUpdateStatusType
	tenuOtaUpdateStatus
 */
typedef void (*tpfOtaUpdateCb) (uint8 u8OtaUpdateStatusType ,uint8 u8OtaUpdateStatus);
 /**@}*/
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
     extern "C" {
#endif

/** @defgroup OtaInitFn m2m_ota_init
 *  @ingroup WLANAPI
* Initialize the OTA layer by registering the update callback, the notification 
*callback is not supported at the current version 
* Calling the API is required for all the OTA API's 
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8  m2m_ota_init(tpfOtaUpdateCb  pfOtaUpdateCb,tpfOtaNotifCb  pfOtaNotifCb)

@param [in]	pfOtaUpdateCb
				OTA Update callback function
				
@param [in]	pfOtaNotifCb
				OTA notify callback function 

@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8  m2m_ota_init(tpfOtaUpdateCb  pfOtaUpdateCb,tpfOtaNotifCb  pfOtaNotifCb);
 /**@}*/
 
 /** @defgroup OtaNotifStFn m2m_ota_notif_set_url
 *  @ingroup WLANAPI
 * Set the OTA notification server URL, the functions need to be called before any check for update
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8  m2m_ota_notif_set_url(uint8 * u8Url);

@param [in]	u8Url
			 OTA notification server URL
@warning 
			Calling m2m_ota_init is required
			Notification Server is not supported in the current version (function is not implemented)
@see    
			m2m_ota_init			
@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8  m2m_ota_notif_set_url(uint8 * u8Url);
 /**@}*/
 /** @defgroup OtaNotifCheckFn m2m_ota_notif_check_for_update
 *  @ingroup WLANAPI
 * check for OTA update using the the Notification Server URL
 */
  /**@{*/
/*!
@fn	\
	NMI_API sint8  m2m_ota_notif_check_for_update(void);

@warning 
		Function is not implemented (not supported at the current version)
		
@sa   
			m2m_ota_init
			m2m_ota_notif_set_url
@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8  m2m_ota_notif_check_for_update(void);
 /**@}*/
 /** @defgroup OtaSched m2m_ota_notif_sched
*  @ingroup WLANAPI
* Schedule OTA notification Server check for update request after specific number of days
*/
  /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_ota_notif_sched(uint32 u32Period);


@param [in]	u32Period
			Period in days

@sa 
		m2m_ota_init
		m2m_ota_notif_check_for_update
		m2m_ota_notif_set_url
@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8 m2m_ota_notif_sched(uint32 u32Period);
  /**@}*/
/** @defgroup OtaStartUpdatefn m2m_ota_start_update
*  @ingroup WLANAPI
*	Request OTA start update using the downloaded URL, 
*	the OTA module will download the OTA image and ensure integrity of the image, 
*	and update the validity of the image in control structure, 
*	Switching to that image requires calling m2m_ota_switch_firmware API
*/
  /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_ota_start_update(uint8 * u8DownloadUrl);

@param [in]	u8DownloadUrl
		The download firmware URL, you get it from device info according to the application server

@warning
		Calling the m2m_ota_init is a MUST. 
		Calling this API doesnot guarantee OTA WINC image update, 
		It depends on the connection between the WINC and the Download Server, and the validity of the image
		If the API response failed that may invalidate the roll-back image if it previously valid
		as the WINC has not any internal memory except the flash roll-back image location to validate the downloaded image
				
@see		
		m2m_ota_init
		tpfOtaUpdateCb
		
@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
\section Example
   The example shows an example of how the OTA image update is carried out.
@code
static void OtaUpdateCb(uint8 u8OtaUpdateStatusType ,uint8 u8OtaUpdateStatus)
{
	if(u8OtaUpdateStatusType == DL_STATUS) {
		if(u8OtaUpdateStatus == OTA_STATUS_SUCSESS) {
			//switch to the upgraded firmware
			m2m_ota_switch_firmware();
		}
	}
	else if(u8OtaUpdateStatusType == SW_STATUS) {
		if(u8OtaUpdateStatus == OTA_STATUS_SUCSESS) {
			M2M_INFO("Now OTA successfully done");
			//start the host SW upgrade then system reset is required (Reinitialize the driver)
		}
	}
}
void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
{
	case M2M_WIFI_REQ_DHCP_CONF:
	{
		//after successfully connection, start the over air upgrade
		m2m_ota_start_update(OTA_URL);
	}
	break;
	default:
	break;
}
int main (void)
{
	tstrWifiInitParam param;
	tstr1xAuthCredentials gstrCred1x    = AUTH_CREDENTIALS;
	nm_bsp_init();
	
	m2m_memset((uint8*)&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_event_cb;
	
	//intilize the WINC Driver
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret)
	{
		M2M_ERR("Driver Init Failed <%d>\n",ret);
		while(1);
	}
	//intilize the ota module
	m2m_ota_init(OtaUpdateCb,NULL);
	//connect to AP that provide connection to the OTA server
	m2m_wifi_default_connect();

	while(1)
	{
		
		//Handle the app state machine plus the WINC event handler                                                                     
		while(m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
			
		}
		
	}
}
@endcode		

*/
NMI_API sint8 m2m_ota_start_update(uint8 * u8DownloadUrl);
  /**@}*/
/** @defgroup OtaRollbackfn m2m_ota_rollback
*  @ingroup WLANAPI
	Request OTA Roll-back to the old (other) WINC image, the WINC firmware will check the validatity of the Roll-back image
	and switch to it if it is valid.
	If the API response is success, system restart is required (re-initialize the driver with hardware rest) update the host driver version may
	be required if it is did not match the minmum version supported by the  WINC firmware.

*/
  /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_ota_rollback(void);

@sa 
	m2m_ota_init
	m2m_ota_start_update	

@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8 m2m_ota_rollback(void);
  /**@}*/
  /**@}*/
/** @defgroup OtaSwitchFirmware m2m_ota_switch_firmware
*  @ingroup WLANAPI
* Switch to the upgraded Firmware, that API will update the control structure working image to the upgraded image
  take effect will be on the next system restart
*/
  /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_ota_switch_firmware(void);

@warning
	if the API response Success, system restart is required (re-initialize the driver with hardware rest)
	update the host driver version may be required if it is not matched with WINC firmware supported minimum driver version
@sa 
	m2m_ota_init
	m2m_ota_start_update

@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8 m2m_ota_switch_firmware(void);
  /**@}*/
NMI_API sint8 m2m_ota_test(void);

#ifdef __cplusplus
}
#endif
#endif /* __M2M_OTA_H__ */
