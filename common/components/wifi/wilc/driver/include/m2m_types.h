/**
 *
 * \file
 *
 * \brief NMC1500 IoT Application Interface Internal Types.
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

#ifndef __M2M_WIFI_TYPES_H__
#define __M2M_WIFI_TYPES_H__


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef	_BOOT_
#ifndef _FIRMWARE_
#include "common/include/nm_common.h"
#else
#include "m2m_common.h"
#endif
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/**@defgroup  WlanDefines Defines
 * @ingroup m2m_wifi
 */
/**@{*/
#define MAKE_VERSION(major, minor, patch) (((uint16)(major) << 8) | ((minor << 4) | (patch) ))

/*======*======*======*======*
		FIRMWARE VERSION NO INFO
 *======*======*======*======*/

#define M2M_FIRMWARE_VERSION_MAJOR_NO 					(3)
/*!< Firmware Major release version number.
*/


#define M2M_FIRMWARE_VERSION_MINOR_NO					(5)
/*!< Firmware Minor release version number.
*/

#define M2M_FIRMWARE_VERSION_PATCH_NO					(0)
/*!< Firmware patch release version number.
*/

/*======*======*======*======*
  SUPPORTED DRIVER VERSION NO INFO
 *======*======*======*======*/

#define M2M_DRIVER_VERSION_MAJOR_NO 					(18)
/*!< Driver Major release version number.
*/


#define M2M_DRIVER_VERSION_MINOR_NO						(0)
/*!< Driver Minor release version number.
*/

#define M2M_DRIVER_VERSION_PATCH_NO						(14)
/*!< Driver patch release version number.
*/


#if !defined(M2M_FIRMWARE_VERSION_MAJOR_NO) || !defined(M2M_FIRMWARE_VERSION_MINOR_NO)
#error Undefined version number
#endif

#define M2M_BUFFER_MAX_SIZE								(1600UL - 4)
/*!< Maximum size for the shared packet buffer.
 */


#define M2M_MAC_ADDRES_LEN                                 				6
/*!< The size fo 802 MAC address.
 */

#define M2M_ETHERNET_HDR_OFFSET							34+2
/*!< The offset of the Ethernet header within the WLAN Tx Buffer. 
Should be word aligned
 */


#define M2M_ETHERNET_HDR_LEN								14
/*!< Length of the Etherenet header in bytes.
*/


#define M2M_MAX_SSID_LEN 									33
/*!< Maximum size for the Wi-Fi SSID including the NULL termination.
 */


#define M2M_MAX_PSK_LEN           								65
/*!< Maximum size for the WPA PSK including the NULL termination.
 */
#if defined(M2M_WILC1000)
#define M2M_MAX_PMK_LEN           						40
/*!< Maximum size for the PMK value.
 */
 
#define M2M_ENTERPRISE_CERT_MAX_LENGTH_IN_BYTES     	4000
/*!< Maximum size for the Certificate that would be download to the WILC1000 Memory.
THIS VALUE CANNOT BE CHANGED
 */
#endif

#define M2M_DEVICE_NAME_MAX								48
/*!< Maximum Size for the device name including the NULL termination.
 */
 

#define M2M_LISTEN_INTERVAL 								1
/*!< The STA uses the Listen Interval parameter to indicate to the AP how
	many beacon intervals it shall sleep before it retrieves the queued frames
	from the AP. 
*/


#define M2M_1X_USR_NAME_MAX								21
/*!< The maximum size of the user name including the NULL termination. 
	It is used for RADIUS authentication in case of connecting the device to
	an AP secured with WPA-Enterprise. 
*/


#define M2M_1X_PWD_MAX									41
/*!< The maximum size of the password including the NULL termination. 
	It is used for RADIUS authentication in case of connecting the device to
	an AP secured with WPA-Enterprise. 
*/

#define M2M_CUST_IE_LEN_MAX								252
/*!< The maximum size of IE (Information Element).
*/


#define M2M_CONFIG_CMD_BASE									1
/*!< The base value of all the host configuration commands opcodes.
*/
#define M2M_SERVER_CMD_BASE									20
/*!< The base value of all the power save mode host commands codes.
*/
#define M2M_STA_CMD_BASE									40
/*!< The base value of all the station mode host commands opcodes.
*/
#define M2M_AP_CMD_BASE										80
/*!< The base value of all the Access Point mode host commands opcodes.
*/
#define M2M_P2P_CMD_BASE									90
/*!< The base value of all the P2P mode host commands opcodes.
*/
#define M2M_OTA_CMD_BASE									100
/*!< The base value of all the OTA mode host commands opcodes.
*/


#define WEP_40_KEY_SIZE 								((uint8)5)
/*!< Indicate the wep key size in bytes for 40 bit hex passphrase.
*/
#define WEP_104_KEY_SIZE 								((uint8)13)
/*!< Indicate the wep key size in bytes for 104 bit hex passphrase.
*/
#define WEP_40_KEY_STRING_SIZE 							((uint8)10)
/*!< Indicate the wep key size in bytes for 40 bit string passphrase.
*/
#define WEP_104_KEY_STRING_SIZE 						((uint8)26)
/*!< Indicate the wep key size in bytes for 104 bit string passphrase.
*/
#define WEP_KEY_MAX_INDEX								((uint8)4)
/*!< Indicate the max key index value for WEP authentication
*/
#define M2M_SCAN_MIN_NUM_SLOTS							(2)
/*!< The min. number of scan slots performed by the WINC board.
*/
#define M2M_SCAN_MIN_SLOT_TIME							(20)
/*!< The min. duration in miliseconds of a scan slots performed by the WINC board.
*/
#define M2M_SCAN_FAIL 									((uint8)1)
/*!< Indicate that the WINC board has failed to perform the scan operation.
*/
#define M2M_JOIN_FAIL	 								((uint8)2)
/*!< Indicate that the WINC board has failed to join the BSS .
*/
#define M2M_AUTH_FAIL 									((uint8)3)
/*!< Indicate that the WINC board has failed to authenticate with the AP.
*/
#define M2M_ASSOC_FAIL 									((uint8)4)
/*!< Indicate that the WINC board has failed to associate with the AP.
*/
#define M2M_LINK_LOSS 									((uint8)5)
/*!< Indicate that the AP/STA is out of range.
*/
#define M2M_STATION_IS_LEAVING							((uint8)6)
/*!< Indicate that the station connected to WILC1000 is leaving
*/


#define M2M_SCAN_ERR_WIFI   	 						((sint8)-2)
/*!< currently not used.
*/
#define M2M_SCAN_ERR_IP      							((sint8)-3)
/*!< currently not used.
*/
#define M2M_SCAN_ERR_AP      							((sint8)-4)	
/*!< currently not used.
*/
#define M2M_SCAN_ERR_P2P      							((sint8)-5)
/*!< currently not used.
*/
#define M2M_SCAN_ERR_WPS      							((sint8)-6)	
/*!< currently not used.
*/


#define M2M_DEFAULT_CONN_EMPTY_LIST					((sint8)-20)
/*!<
	A failure response that indicates an empty network list as 
	a result to the function call m2m_default_connect.
*/


#define M2M_DEFAULT_CONN_SCAN_MISMATCH				((sint8)-21)
/*!<
	A failure response that indicates that no one of the cached networks 
	was found in the scan results, as a result to the function call m2m_default_connect.
*/


/*======*======*======*======*
	MONTIORING MODE DEFINITIONS
 *======*======*======*======*/

#define M2M_WIFI_FRAME_TYPE_ANY							0xFF
/*!< Set monitor mode to receive any of the frames types
*/
#define M2M_WIFI_FRAME_SUB_TYPE_ANY						0xFF
/*!< Set monitor mode to receive frames with any sub type
*/

/*======*======*======*======*
	OTA DEFINITIONS
 *======*======*======*======*/
 
#define OTA_ROLLB_STATUS_VALID				(0x12526285)
/*!< 
	Magic value updated in the Control structure in case of ROLLACK image Valid
*/
#define OTA_ROLLB_STATUS_INVALID			(0x23987718)
/*!< 
	Magic value updated in the Control structure in case of ROLLACK image InValid
*/
#define OTA_MAGIC_VALUE						(0x1ABCDEF9)
/*!< 
	Magic value set at the beginning of the OTA image header
*/
#define OTA_SHA256_DIGEST_SIZE 				(32)
/*!< 
	Sha256 digest size in the ota image, 
	the sha256 digest is set at the beginning of image before the OTA header 
*/

#define OTA_SUCCESS 						(0)
/*!<
	OTA Success status 
*/
#define OTA_ERR_WORKING_IMAGE_LOAD_FAIL		((sint8)-1)
/*!<
	Failure to load the firmware image
*/
#define OTA_ERR_INVAILD_CONTROL_SEC			((sint8)-2)
/*!<
	Control structure is being crypated  
*/
#define M2M_ERR_OTA_SWITCH_FAIL     		((sint8)-3)
/*!<
	switching to the updated image failed as may be the image is invalid 
*/
#define M2M_ERR_OTA_START_UPDATE_FAIL     	((sint8)-4)
/*!<
	Ota update fail due to multiple reasons 
	- Connection failure
	- Image integrity fail  
	
*/
#define M2M_ERR_OTA_ROLLBACK_FAIL     		((sint8)-5)
/*!<
	Roll-back failed due to Roll-back image is not valid 
*/
#define M2M_ERR_OTA_INVAILD_FLASH_SIZE     	((sint8)-6)
/*!<
	The ota Support at least 4MB flash size, if the above error will appear if the current flash is less than 4M
*/
#define M2M_ERR_OTA_INVAILD_ARG		     	((sint8)-7)
/*!<
	Invalid argument in any OTA Function
*/
/**@}*/

/**
* @addtogroup WlanEnums
*/
 /**@{*/ 
/*!
@enum	\
	tenuM2mConfigCmd
	
@brief
	This enum contains all the host commands used to configure the WINC board.
	
*/
typedef enum {
	M2M_WIFI_REQ_RESTART = M2M_CONFIG_CMD_BASE,
	/*!<
		Restart the WINC MAC layer, it's doesn't restart the IP layer.
	*/
	M2M_WIFI_REQ_SET_MAC_ADDRESS,
	/*!<
		Set the WINC mac address (not possible for production efused boards).
	*/
	M2M_WIFI_REQ_CURRENT_RSSI,
	/*!<
		Request the current connected AP RSSI.
	*/
	M2M_WIFI_RESP_CURRENT_RSSI,
	/*!<
		Rsponse to M2M_WIFI_REQ_CURRENT_RSSI with the RSSI value.
	*/
	M2M_WIFI_REQ_SET_DEVICE_NAME,
	/*!<
		Set the WINC device name property.
	*/
#ifndef M2M_WILC1000
	M2M_WIFI_REQ_START_PROVISION_MODE,
	/*!<
		Start the provisioning mode for the M2M Device.
	*/
#endif
	M2M_WIFI_RESP_PROVISION_INFO,
	/*!<
		Send the provisioning information to the host.
	*/
#ifndef M2M_WILC1000
	M2M_WIFI_REQ_STOP_PROVISION_MODE,
	/*!<
		Stop the current running provision mode.
	*/
#endif
	M2M_WIFI_REQ_SET_SYS_TIME,
	/*!<
		Set time of day from host.
	*/
#ifndef M2M_WILC1000
	M2M_WIFI_REQ_ENABLE_SNTP_CLIENT,
	/*!<
		Enable the simple network time protocol to get the
		time from the internet. this is required for security purposes.
	*/
	M2M_WIFI_REQ_DISABLE_SNTP_CLIENT,
	/*!<
		Disable the simple network time protocol for applications that
		do not need it.
	*/
#endif
	M2M_WIFI_RESP_MEMORY_RECOVER,
	M2M_WIFI_REQ_CUST_INFO_ELEMENT,
	/*!< Add Custom ELement to Beacon Managament Frame.
	*/
	M2M_WIFI_RESP_FIRMWARE_STRTED,
	/*!< respone message to indicate the firmware succefully started.
	*/
	M2M_WIFI_REQ_SET_TX_POWER,
	/*!<  API to set TX power. 
	*/
}tenuM2mConfigCmd;

/*!
@enum	\
	tenuM2mServerCmd
	
@brief
	This enum contains all the WINC commands while in PS mode.
	These command are currently not supported.
*/
typedef enum {
	M2M_WIFI_REQ_CLIENT_CTRL = M2M_SERVER_CMD_BASE,
	M2M_WIFI_RESP_CLIENT_INFO,
	M2M_WIFI_REQ_SERVER_INIT
}tenuM2mServerCmd;


/*!
@enum	\
	tenuM2mStaCmd
	
@brief
	This enum contains all the WINC commands while in Station mode.
*/
typedef enum {
	M2M_WIFI_REQ_CONNECT = M2M_STA_CMD_BASE,
	/*!< Connect with AP command.
	*/
	M2M_WIFI_REQ_DEFAULT_CONNECT,
	/*!< Connect with default AP command.
	*/
	M2M_WIFI_RESP_DEFAULT_CONNECT,
	/*!< Connect with default AP response.
	*/
	M2M_WIFI_REQ_GET_CONN_INFO,
	/*!< Request connection information command.
	*/
	M2M_WIFI_RESP_CONN_INFO,
	/*!< Request connection information response.
	*/
	M2M_WIFI_REQ_DISCONNECT,
	/*!< Request to disconnect from AP command.
	*/
	M2M_WIFI_RESP_CON_STATE_CHANGED,
	/*!< Connection state changed response.
	*/
	M2M_WIFI_REQ_SLEEP,
	/*!< Set PS mode command.
	*/
	M2M_WIFI_REQ_SCAN,
	/*!< Request scan command.
	*/
	M2M_WIFI_REQ_WPS_SCAN,
	/*!< Request WPS scan command.
	*/
	M2M_WIFI_RESP_SCAN_DONE,
	/*!< Scan complete notification response.
	*/
	M2M_WIFI_REQ_SCAN_RESULT,
	/*!< Request Scan results command.
	*/
	M2M_WIFI_RESP_SCAN_RESULT,
	/*!< Request Scan results resopnse.
	*/
	M2M_WIFI_REQ_WPS,
	/*!< Request WPS start command.
	*/

	M2M_WIFI_REQ_START_WPS,
	/*!< This command is for internal use by the WINC and 
		should not be used by the host driver.
	*/
	M2M_WIFI_REQ_DISABLE_WPS,
	/*!< Request to disable WPS command.
	*/
	M2M_WIFI_REQ_DHCP_CONF,
	/*!< Response indicating that IP address was obtained.
	*/
	M2M_WIFI_RESP_IP_CONFIGURED,
	/*!< This command is for internal use by the WINC and 
		should not be used by the host driver.
	*/
	M2M_WIFI_RESP_IP_CONFLICT,
	/*!< Response indicating a conflict in obtained IP address.
		The user should re attempt the DHCP request.
	*/

	M2M_WIFI_REQ_ENABLE_MONITORING,
	/*!< Request to enable monitor mode  command.
	*/
	M2M_WIFI_REQ_DISABLE_MONITORING,
	/*!< Request to disable monitor mode  command.
	*/
	M2M_WIFI_RESP_WIFI_RX_PACKET,
	/*!< Indicate that a packet was received in monitor mode.
	*/
	M2M_WIFI_REQ_SEND_WIFI_PACKET,
	/*!< Send packet in monitor mode.
	*/
	M2M_WIFI_REQ_LSN_INT,
	/*!< Set WiFi listen interval.
	*/
	M2M_WIFI_REQ_SEND_ETHERNET_PACKET,
	/*!< Send ethernet packet in bypass mode.
	*/
	M2M_WIFI_RESP_ETHERNET_RX_PACKET,
	/*!< Receive ethernet packet in bypass mode.
	*/
	M2M_WIFI_REQ_SET_SCAN_OPTION,
	/*!< Set Scan options "slot time, slot number .. etc" .
	*/
	M2M_WIFI_REQ_SET_SCAN_REGION,
	/*!< Set scan region.
	*/
	M2M_WIFI_REQ_SET_SCAN_LIST,
	/*!< Set scan channel list.
	*/
	M2M_WIFI_REQ_DOZE,
	/*!< Used to force the WINC to sleep in manual PS mode.
	*/
	M2M_WIFI_REQ_SET_MAC_MCAST,
	/*!< Set the WINC multicast filters.
	*/
#if defined(M2M_WILC1000) && defined(CONCURRENT_INTERFACES)
	M2M_WIFI_REQ_SET_IFC_ID,
	/*!< Notification that the last packet sent to the firmware successfully.
	and the firmware is ready to receive the next packet
	*/
	M2M_WIFI_RESP_PACKET_SENT,
#endif
#if defined(M2M_WILC1000)
	M2M_WIFI_REQ_CERT_ADD_CHUNK,
	M2M_WIFI_REQ_CERT_DOWNLOAD_DONE,
	M2M_WIFI_REQ_CHG_MONITORING_CHNL,
#endif

} tenuM2mStaCmd;


/*!
@enum	\
	tenuM2mP2pCmd
	
@brief
	This enum contains all the WINC commands while in P2P mode.
*/
typedef enum {
	M2M_WIFI_REQ_P2P_INT_CONNECT = M2M_P2P_CMD_BASE,
	/*!< This command is for internal use by the WINC and 
		should not be used by the host driver.
	*/
	M2M_WIFI_REQ_ENABLE_P2P,
	/*!< Enable P2P mode command.
	*/
	M2M_WIFI_REQ_DISABLE_P2P,
	/*!< Disable P2P mode command.
	*/
	M2M_WIFI_REQ_P2P_REPOST
	/*!< This command is for internal use by the WINC and 
		should not be used by the host driver.
	*/
}tenuM2mP2pCmd;


/*!
@enum	\
	tenuM2mApCmd
	
@brief
	This enum contains all the WINC commands while in AP mode.
*/
typedef enum {
	M2M_WIFI_REQ_ENABLE_AP = M2M_AP_CMD_BASE,
	/*!< Enable AP mode command.
	*/
	M2M_WIFI_REQ_DISABLE_AP,
	/*!< Disable AP mode command.
	*/
#ifdef M2M_WILC1000
	M2M_WIFI_REQ_AP_ASSOC_INFO,
	/*!< info about the associated stations.
	*/
	M2M_WIFI_RESP_AP_ASSOC_INFO,
	/*!< Assoc  information response.
	*/
	M2M_WIFI_REQ_AP_BLACK_LIST,
	/*!< req. to add/remove station to black list.
	*/
#endif
}tenuM2mApCmd;

/*!
@enum	\
	tenuM2mOtaCmd
	
@brief

*/
typedef enum {
	M2M_OTA_REQ_NOTIF_SET_URL = M2M_OTA_CMD_BASE,
	M2M_OTA_REQ_NOTIF_CHECK_FOR_UPDATE,
	M2M_OTA_REQ_NOTIF_SCHED,
	M2M_OTA_REQ_START_UPDATE,
	M2M_OTA_REQ_SWITCH_FIRMWARE,
	M2M_OTA_REQ_ROLLBACK,
	M2M_OTA_RESP_NOTIF_UPDATE_INFO,
	M2M_OTA_RESP_UPDATE_STATUS,
	M2M_OTA_REQ_TEST
}tenuM2mOtaCmd;


/*!
@enum	\
	tenuM2mIpCmd
	
@brief

*/
typedef enum {
	/* Request IDs corresponding to the IP GROUP. */
	M2M_IP_REQ_STATIC_IP_CONF = ((uint8) 10)
} tenuM2mIpCmd;


/*!
@enum	\
	tenuM2mConnState
	
@brief
	Wi-Fi Connection State.
*/
typedef enum {
	M2M_WIFI_DISCONNECTED = 0,
	/*!< Wi-Fi state is disconnected.
	*/
	M2M_WIFI_CONNECTED,
	/*!< Wi-Fi state is connected.
	*/
	M2M_WIFI_UNDEF = 0xff
	/*!< Undefined Wi-Fi State.
	*/
}tenuM2mConnState;

/*!
@enum	\
	tenuM2mSecType
	
@brief
	Wi-Fi Supported Security types.
*/
typedef enum {
	M2M_WIFI_SEC_INVALID = 0,
	/*!< Invalid security type.
	*/
	M2M_WIFI_SEC_OPEN,
	/*!< Wi-Fi network is not secured.
	*/
	M2M_WIFI_SEC_WPA_PSK,
	/*!< Wi-Fi network is secured with WPA/WPA2 personal(PSK).
	*/
	M2M_WIFI_SEC_WEP,
	/*!< Security type WEP (40 or 104) OPEN OR SHARED.
	*/
	M2M_WIFI_SEC_802_1X
	/*!< Wi-Fi network is secured with WPA/WPA2 Enterprise.IEEE802.1x user-name/password authentication.
	 */
}tenuM2mSecType;


/*!
@enum	\
	tenuM2mSecType
	
@brief
	Wi-Fi Supported SSID types.
*/
typedef enum {
	SSID_MODE_VISIBLE = 0,
	/*!< SSID is visible to others.
	*/
	SSID_MODE_HIDDEN
	/*!< SSID is hidden.
	*/
}tenuM2mSsidMode;

/*!
@enum	\
	tenuM2mScanCh
	
@brief
	Wi-Fi RF Channels.
*/
typedef enum {
	M2M_WIFI_CH_1 = ((uint8) 0),
	M2M_WIFI_CH_2,
	M2M_WIFI_CH_3,
	M2M_WIFI_CH_4,
	M2M_WIFI_CH_5,
	M2M_WIFI_CH_6,
	M2M_WIFI_CH_7,
	M2M_WIFI_CH_8,
	M2M_WIFI_CH_9,
	M2M_WIFI_CH_10,
	M2M_WIFI_CH_11,
	M2M_WIFI_CH_12,
	M2M_WIFI_CH_13,
	M2M_WIFI_CH_14,
	M2M_WIFI_CH_ALL = ((uint8) 255)
}tenuM2mScanCh;

/*!
@enum	\
	tenuM2mScanRegion
	
@brief
	Wi-Fi RF Channels.
*/
typedef enum {
	NORTH_AMERICA = ((uint8) 11),
	ASIA		=       ((uint8) 14)
}tenuM2mScanRegion;


/*!
@enum	\
	tenuPowerSaveModes
	
@brief
	Power Save Modes.
*/
typedef enum {
	M2M_NO_PS,
	/*!< Power save is disabled.
	*/
	M2M_PS_AUTOMATIC,
	/*!< Power save is done automatically by the WINC.
		This mode doesn't disable all of the WINC modules and 
		use higher amount of power thant the H_AUTOMATIC and 
		the DEEP_AUTOMATIC modes..
	*/
	M2M_PS_H_AUTOMATIC,
	/*!< Power save is done automatically by the WINC.
		Achieve higher power save than the AUTOMATIC mode
		by shutiing down more parts of the WINC board.
	*/
	M2M_PS_DEEP_AUTOMATIC,
	/*!< Power save is done automatically by the WINC.
		Achieve the highest possible power save.
	*/
	M2M_PS_MANUAL
	/*!< Power save is done manually by the user.
	*/
}tenuPowerSaveModes;


/*!
@enum	\
	tenuWPSTrigger
	
@brief
	WPS Triggering Methods.
*/
typedef enum{
	WPS_PIN_TRIGGER = 0,
	/*!< WPS is triggered in PIN method.
	*/
	WPS_PBC_TRIGGER = 4
	/*!< WPS is triggered via push button.
	*/
}tenuWPSTrigger;


/*!
@struct	\
	tstrM2mWifiWepParams

@brief
	WEP security key parameters.
*/
typedef struct{
	uint8	u8KeyIndx;
	/*!< Wep key Index.
	*/
	uint8	u8KeySz;
	/*!< Wep key Size.
	*/
	uint8	au8WepKey[WEP_104_KEY_STRING_SIZE + 1];
	/*!< WEP Key represented as a NULL terminated ASCII string.
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes to keep the structure word alligned.
	*/
}tstrM2mWifiWepParams;


/*!
@struct	\
	tstr1xAuthCredentials

@brief
	Credentials for the user to authenticate with the AAA server (WPA-Enterprise Mode IEEE802.1x).
*/
typedef struct{
	uint8	au8UserName[M2M_1X_USR_NAME_MAX];
	/*!< User Name. It must be Null terminated string.
	*/
	uint8	au8Passwd[M2M_1X_PWD_MAX];
	/*!< Password corresponding to the user name. It must be Null terminated string.
	*/
}tstr1xAuthCredentials;


/*!
@union	\
	tuniM2MWifiAuth

@brief
	Wi-Fi Security Parameters for all supported security modes.
*/
typedef union{
	uint8				au8PSK[M2M_MAX_PSK_LEN];
	
#if defined(M2M_WILC1000)	
	/*!< PMK key if it is calculated in the host.
	*/
	uint8				au8PMK[M2M_MAX_PMK_LEN];
#endif
	/*!< Pre-Shared Key in case of WPA-Personal security.
	*/
	tstr1xAuthCredentials	strCred1x;
	/*!< Credentials for RADIUS server authentication in case of WPA-Enterprise security.
	*/
	tstrM2mWifiWepParams	strWepInfo;
	/*!< WEP key parameters in case of WEP security.
	*/
}tuniM2MWifiAuth;


/*!
@struct	\
	tstrM2MWifiSecInfo

@brief
	Authentication credentials to connect to a Wi-Fi network.
*/
typedef struct{
	tuniM2MWifiAuth		uniAuth;
	/*!< Union holding all possible authentication parameters corresponding the current security types.
	*/
	uint8				u8SecType;
	/*!< Wi-Fi network security type. See tenuM2mSecType for supported security types.
	*/
#if defined(M2M_WILC1000)
	uint8				u8IsPMKUsed;
	/*!< set to true if the PMK is calculated on the host .
	*/
	#define __PADDING__		(4 - ((sizeof(tuniM2MWifiAuth) + 2) % 4))
#else
	#define __PADDING__		(4 - ((sizeof(tuniM2MWifiAuth) + 1) % 4))
#endif
	

	uint8				__PAD__[__PADDING__];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWifiSecInfo;


/*!
@struct	\
	tstrM2mWifiConnect

@brief	
	Wi-Fi Connect Request
*/
typedef struct{
	tstrM2MWifiSecInfo		strSec;
	/*!< Security parameters for authenticating with the AP.
	*/
	uint16				u16Ch;
	/*!< RF Channel for the target SSID.
	*/
	uint8				au8SSID[M2M_MAX_SSID_LEN];
	/*!< SSID of the desired AP. It must be NULL terminated string.
	*/
#define __CONN_PAD_SIZE__		(4 - ((sizeof(tstrM2MWifiSecInfo) + M2M_MAX_SSID_LEN + 2) % 4))
	uint8				__PAD__[__CONN_PAD_SIZE__];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mWifiConnect;


/*!
@struct	\
	tstrM2MWPSConnect

@brief	
	WPS Configuration parameters

@sa 
	tenuWPSTrigger
*/
typedef struct {
	uint8 	u8TriggerType;
	/*!< WPS triggering method (Push button or PIN)
	*/
	char         acPinNumber[8];
	/*!< WPS PIN No (for PIN method)
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWPSConnect;


/*!
@struct	\
	tstrM2MWPSInfo

@brief	WPS Result

	This structure is passed to the application in response to a WPS request. If the WPS session is completed successfully, the
	structure will have Non-ZERO authentication type. If the WPS Session fails (due to error or timeout) the authentication type
	is set to ZERO.

@sa
	tenuM2mSecType
	
@todo
	Use different error codes to differentaite error types.
*/
typedef struct{
	uint8	u8AuthType;
	/*!< Network authentication type.
	*/
	uint8   	u8Ch;
	/*!< RF Channel for the AP.
	*/
	uint8	au8SSID[M2M_MAX_SSID_LEN];
	/*!< SSID obtained from WPS.
	*/
	uint8	au8PSK[M2M_MAX_PSK_LEN];
	/*!< PSK for the network obtained from WPS.
	*/
}tstrM2MWPSInfo;


/*!
@struct	\
	tstrM2MDefaultConnResp

@brief
	Response error of the m2m_default_connect

@sa
	M2M_DEFAULT_CONN_SCAN_MISMATCH
	M2M_DEFAULT_CONN_EMPTY_LIST
*/
typedef struct{
	sint8		s8ErrorCode;
	/*!<
		Default connect error code. possible values are:
		- M2M_DEFAULT_CONN_EMPTY_LIST
		- M2M_DEFAULT_CONN_SCAN_MISMATCH
	*/
	uint8	__PAD24__[3];
}tstrM2MDefaultConnResp;

/*!
@struct	\
	tstrM2MScan
	
@brief	
	Wi-Fi Scan Request

@sa 
	tenuM2mScanCh
*/
typedef struct {
	uint8   u8NumOfSlot;
	/*!< The min number of slots is 2 for every channel, 
	every slot the soc will send Probe Req on air, and wait/listen for PROBE RESP/BEACONS for the u16slotTime
	*/
	uint8   u8SlotTime;
	/*!< the time that the Soc will wait on every channel listening to the frames on air
		when that time increaseed number of AP will increased in the scan results
		min time is 10 ms and the max is 250 ms 
	*/
	uint8 __PAD16__[2];

}tstrM2MScanOption;

/*!
@struct	\
	tstrM2MScanRegion
	
@brief	
	Wi-Fi channel regulation region information.

@sa 
	tenuM2mScanRegion
*/
typedef struct {
	uint8   u8ScanRegion;
	/*|< Specifies the number of channels allowed in the region (e.g. North America = 11 ... etc.).
	*/
	uint8 __PAD24__[3];

}tstrM2MScanRegion;

/*!
@struct	\
	tstrM2MScanList
	
@brief	
	Wi-Fi channel list regulation.

@sa 
	tenuM2mScanList
*/
typedef struct {
	uint8   u8ChannelListCount;
	/*|< Specifies the number of channels allowed.
	*/
	uint8 au8ChannelList[14];
	/*|< Channel List.
	*/
	uint8 __PAD24__[1];

}tstrM2MScanList;


/*!
@struct	\
	tstrM2MScan
	
@brief	
	Wi-Fi Scan Request

@sa 
	tenuM2mScanCh
*/
typedef struct {
	uint8 	u8ChNum;
	/*!< The Wi-Fi RF Channel number
	*/
#ifdef M2M_WILC1000
	uint8 	au8SSID[M2M_MAX_SSID_LEN];
	/*!< SSID of the favorite AP	
	*/
#endif
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/

}tstrM2MScan;


/*!
@struct	\	
	tstrM2mScanDone

@brief	
	Wi-Fi Scan Result
*/
typedef struct{
	uint8 	u8NumofCh;
	/*!< Number of found APs
	*/
	sint8 	s8ScanState;
	/*!< Scan status
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mScanDone;


/*!
@struct	\
	tstrM2mReqScanResult

@brief	Scan Result Request

	The Wi-Fi Scan results list is stored in Firmware. The application can request a certain scan result by its index.
*/
typedef struct {
	uint8 	u8Index;
	/*!< Index of the desired scan result
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mReqScanResult;


/*!
@struct	\
	tstrM2mWifiscanResult

@brief	Wi-Fi Scan Result

	Information corresponding to an AP in the Scan Result list identified by its order (index) in the list.
*/
typedef struct {
	uint8 	u8index; 
	/*!< AP index in the scan result list.
	*/
	sint8 	s8rssi; 
	/*!< AP signal strength.
	*/
	uint8 	u8AuthType; 
	/*!< AP authentication type.
	*/
	uint8 	u8ch; 
	/*!< AP RF channel.
	*/
	uint8	au8BSSID[6];
	/*!< BSSID of the AP.
	*/
	uint8 	au8SSID[M2M_MAX_SSID_LEN]; 
	/*!< AP ssid.
	*/
	#ifdef M2M_WILC1000
	uint8 	au8DeviceName[M2M_DEVICE_NAME_MAX]; 
	#endif
	uint8 	_PAD8_;
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mWifiscanResult;


/*!
@struct	\		
	tstrM2mWifiStateChanged

@brief		
	Wi-Fi Connection State

@sa			
	M2M_WIFI_DISCONNECTED, M2M_WIFI_CONNECTED, M2M_WIFI_REQ_CON_STATE_CHANGED
*/
typedef struct {
	uint8	u8CurrState;
	/*!< Current Wi-Fi connection state
	*/
	uint8  u8ErrCode;
	/*!< Error type
	*/
#if defined(M2M_WILC1000)
	uint8  u8IfcId;
	uint8  u8MAcAddr[6];
	uint8	__PAD24__[3];
#else
	uint8	__PAD16__[2];
#endif
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mWifiStateChanged;


/*!
@struct	\
	tstrM2mPsType

@brief		
	Power Save Configuration

@sa
	tenuPowerSaveModes
*/
typedef struct{
	uint8 	u8PsType;
	/*!< Power save operating mode
	*/
	uint8 	u8BcastEn;
	/*!<
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mPsType;

/*!
@struct	\
	tstrM2mSlpReqTime

@brief		
	Manual power save request sleep time 

*/
typedef struct {
	/*!< Sleep time in ms
	*/
	uint32 u32SleepTime;

} tstrM2mSlpReqTime;


/*!
@struct	\
	tstrM2mLsnInt

@brief	Listen interval 

	It is the value of the Wi-Fi STA listen interval for power saving. It is given in 
	units of Beacon period. Periodically after the listen interval fires, the STA is
	wakeup requesting any buffered frames for it from the AP (Ps-POLL).	
*/
typedef struct {
	uint16 	u16LsnInt;
	/*!< Listen interval in Beacon period count.
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mLsnInt;


/*!
@struct	\
	tstrM2MWifiMonitorModeCtrl

@brief	Wi-Fi Monitor Mode Filter

	This structure sets the filtering criteria for Wlan packets when monitoring mode is enable. The received packets
	matching the filtering parameters, are passed directly to the application.
*/
typedef struct{
	uint8	u8ChannelID;
	/* !< RF Channel ID.
	*/
	uint8	u8FrameType;
	/*!< It must use values from tenuWifiFrameType. 
	*/
	uint8	u8FrameSubtype;
	/*!< It must use values from tenuSubTypes.
	*/
	uint8	au8SrcMacAddress[6];
	/* ZERO means DO NOT FILTER Source address.
	*/
	uint8	au8DstMacAddress[6];
	/* ZERO means DO NOT FILTER Destination address.
	*/
	uint8	au8BSSID[6];
	/* ZERO means DO NOT FILTER BSSID.
	*/
	uint8 u8EnRecvHdr;
	/*
	 Enable recv the full hder before the payload*/	
	uint8	__PAD24__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWifiMonitorModeCtrl;


/*!
@struct	\
	tstrM2MWifiRxPacketInfo

@brief	Wi-Fi RX Frame Header

	The M2M application has the ability to allow Wi-Fi monitoring mode for receiving all Wi-Fi Raw frames matching
	a well defined filtering criteria. When a target Wi-Fi packet is received, the header information are extracted and
	assigned in this structure.
*/
typedef struct{
	uint8	u8FrameType;
	/*!< It must use values from tenuWifiFrameType. 
	*/
	uint8	u8FrameSubtype;
	/*!< It must use values from tenuSubTypes.
	*/
	uint8	u8ServiceClass;
	/*!< Service class from Wi-Fi header.
	*/
	uint8	u8Priority;
	/*!< Priority from Wi-Fi header.
	*/
	uint8	u8HeaderLength;
	/*!< Frame Header length.
	*/
	uint8	u8CipherType;
	/*!< Encryption type for the rx packet.
	*/
	uint8	au8SrcMacAddress[6];
	/* !< ZERO means DO NOT FILTER Source address.
	*/
	uint8	au8DstMacAddress[6];
	/* !< ZERO means DO NOT FILTER Destination address.
	*/
	uint8	au8BSSID[6];
	/* !< ZERO means DO NOT FILTER BSSID.
	*/
	uint16	u16DataLength;
	/*!< Data payload length (Header excluded).
	*/
	uint16	u16FrameLength;
	/*!< Total frame length (Header + Data).
	*/
	uint32	u32DataRateKbps;
	/*!< Data Rate in Kbps.
	*/
	sint8		s8RSSI;
	/*!< RSSI.
	*/	
	uint16	u16SeqNo;
		/*!< Frame Seq Number.
	*/
	uint8	__PAD24__[1];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWifiRxPacketInfo;


/*!
@struct	\
	tstrM2MWifiTxPacketInfo

@brief	Wi-Fi Tx Packet Info

	The M2M Application has the ability to compose a RAW Wi-Fi frames (under the application responsibility). When
	transmitting a Wi-Fi packet, the application must supply the firmware with this structure for sending the target frame.
*/
typedef struct{
	uint16	u16PacketSize;
	/*!< Wlan frame length.
	*/
	uint16	u16HeaderLength;
	/*!< Wlan frame header length.
	*/
#ifdef M2M_WILC1000
	uint8	u8IfcId;
	/*!< Interface ID to which this frame belongs.
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
#endif
}tstrM2MWifiTxPacketInfo;


/*!
 @struct	\	
 	tstrM2MP2PConnect

 @brief		
 	Set the device to operate in the Wi-Fi Direct (P2P) mode.
*/
typedef struct {
	uint8 	u8ListenChannel; 
	/*!< P2P Listen Channel (1, 6 or 11)
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MP2PConnect;

/*!
@struct	\
	tstrM2MAPConfig

@brief	AP Configuration

	This structure holds the configuration parameters for the M2M AP mode. It should be set by the application when
	it requests to enable the M2M AP operation mode. The M2M AP mode currently supports only WEP security (with
	the NO Security option availabe of course).
*/
typedef struct {
	/*!<
		Configuration parameters for the WiFi AP.
	*/
	uint8 	au8SSID[M2M_MAX_SSID_LEN]; 
	/*!< AP SSID
	*/
	uint8 	u8ListenChannel; 
	/*!< Wi-Fi RF Channel which the AP will operate on
	*/
	uint8	u8KeyIndx; 
	/*!< Wep key Index
	*/
	uint8	u8KeySz;
#ifdef M2M_WILC1000
	
	uint8				u8IsPMKUsed;
	/*!< set to true if the PMK is calculated on the host .
	*/
	
	union
	{		
		uint8				au8PSK[M2M_MAX_PSK_LEN];
		/*!< WPA key 
		*/
		uint8				au8PMK[M2M_MAX_PMK_LEN];
		/*!< PMK if used , used by the driver only not by user 
		*/
	};
#endif
	/*!< Wep key Size 
	*/
	uint8	au8WepKey[WEP_104_KEY_STRING_SIZE + 1]; 
	/*!< Wep key 
	*/
	uint8 	u8SecType; 
	/*!< Security type: Open or WEP only in the current implementation
	*/
	uint8 	u8SsidHide;
	/*!< SSID Status "Hidden(1)/Visible(0)"
	*/
	uint8	au8DHCPServerIP[4];
	/*!< Ap IP server address
	*/
#ifdef M2M_WILC1000
	uint8	__PAD24__[1];
#else
	uint8	__PAD24__[3];
#endif
	/*!< Padding bytes for forcing alignment
	*/
}tstrM2MAPConfig;


/*!
@struct	\	
	tstrM2mServerInit

@brief	
	PS Server initialization.
*/
typedef struct {
	uint8 	u8Channel;
	/*!< Server Listen channel
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mServerInit;


/*!
@struct	\	
	tstrM2mClientState

@brief	
	PS Client State.
*/
typedef struct {
	uint8 	u8State;
	/*!< PS Client State
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mClientState;


/*!
@struct	\	
	tstrM2Mservercmd

@brief
	PS Server Cmd
*/
typedef struct {
	uint8	u8cmd; 
	/*!< PS Server Cmd
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2Mservercmd;


/*!
@struct	\
	tstrM2mSetMacAddress
	
@brief		
	Sets the MAC address from application. It is only intended for testing purpose.
	This method is not used for production SW. Production SW reads MAC Address from EFUSE.

@note		
	It's recommended to call this only before calling connect
*/
typedef struct {
	uint8 	au8Mac[6]; 
	/*!< MAC address array 
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mSetMacAddress;



/*!
@struct	\
 	tstrM2MDeviceNameConfig
 	
@brief	Device name

	It is assigned by the application. It is used mainly for Wi-Fi Direct device 
	discovery and WPS device information.
*/
typedef struct {
	uint8 	au8DeviceName[M2M_DEVICE_NAME_MAX];
	/*!< NULL terminated device name
	*/
}tstrM2MDeviceNameConfig;


/*!
@struct	\	
 	tstrM2MIPConfig
 	
@brief		
 	Static IP configuration.

@note
 	All member IP addresses are expressed in Network Byte Order (eg. "192.168.10.1" will be expressed as 0x010AA8C0).
 */
typedef struct {
	uint32 	u32StaticIP;
	/*!< The static IP assigned to the device.
	*/
	uint32 	u32Gateway;
	/*!< IP of the Default internet gateway.
	*/
	uint32 	u32DNS;
	/*!< IP for the DNS server.
	*/
	uint32 	u32SubnetMask;
	/*!< Subnet mask for the local area network.
	*/
} tstrM2MIPConfig;

/*!
@struct	\	
 	tstrM2mIpRsvdPkt
 	
@brief		
 	Received Packet Size and Data Offset

 */
typedef struct{
	uint16	u16PktSz;
	uint16	u16PktOffset;
	#ifdef M2M_WILC1000
	uint8	u8IfcId;
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
	#endif
} tstrM2mIpRsvdPkt;


/*!
@struct	\	
 	tstrM2MProvisionModeConfig
 	
@brief		
 	M2M Provisioning Mode Configuration
 */

typedef struct {
	tstrM2MAPConfig	strApConfig;
	/*!<
		Configuration parameters for the WiFi AP.
	*/
	char				acHttpServerDomainName[64];
	/*!<
		The device domain name for HTTP provisioning.
	*/
	uint8			u8EnableRedirect;
	/*!<
		A flag to enable/disable HTTP redirect feature for the HTTP Provisioning server. If the Redirect is enabled,
		all HTTP traffic (http://URL) from the device associated with WINC AP will be redirected to the HTTP Provisioning Web page.
		- 0 : Disable HTTP Redirect.
		- 1 : Enable HTTP Redirect. 
	*/
	uint8			__PAD24__[3];
}tstrM2MProvisionModeConfig;

/*!
@struct	\	
 	tstrM2MProvisionInfo
 	
@brief		
 	M2M Provisioning Information obtained from the HTTP Provisioning server.
 */
typedef struct{
	uint8	au8SSID[M2M_MAX_SSID_LEN];
	/*!<
		Provisioned SSID.
	*/
	uint8	au8Password[M2M_MAX_PSK_LEN];
	/*!<
		Provisioned Password.
	*/
	uint8	u8SecType;
	/*!<
		Wifi Security type.
	*/
	uint8	u8Status;
	/*!<
		Provisioning status. It must be checked before reading the provisioning information. It may be
		- M2M_SUCCESS 	: Provision successful.
		- M2M_FAIL		: Provision Failed.
	*/
}tstrM2MProvisionInfo;

/*!
@struct	\	
 	tstrM2MConnInfo
 	
@brief		
 	M2M Provisioning Information obtained from the HTTP Provisioning server.
 */
typedef struct{
	char		acSSID[M2M_MAX_SSID_LEN];
	/*!< AP connection SSID name  */
	uint8	u8SecType;
	/*!< Security type */
#ifndef M2M_WILC1000
	uint8	au8IPAddr[4];
	/*!< Connection IP address */
#endif
	sint8		s8RSSI;
	/*!< Connection RSSI signal */
	uint8	__PAD8__;
	/*!< Padding bytes for forcing 4-byte alignment */
}tstrM2MConnInfo;

/*!
@struct	\	
 	tstrM2MAssocEntryInfo
 	
@brief		
 	M2M association information of of an assoicated entry in the AP mode.
 */
typedef struct{
	uint8	BSSID[6];
	/*!<BSSID of the connected station*/
	sint8		s8RSSI;
	/*!< Connection RSSI signal */
}tstrM2MAssocEntryInfo;

/*!
@struct	\	
 	tstrM2MAPAssocInfo
 	
@brief		
 	M2M association information of the connected station in the AP mode.
 */
typedef struct{
	uint8	u8NoConnSta;
	/*!<No of connected stations*/
	tstrM2MAssocEntryInfo astrM2MAssocEntryInfo[8];
	/*!< array of the associated stations*/
}tstrM2MAPAssocInfo;

/*!
@struct	\	
 	tstrOtaInitHdr
 	
@brief		
 	OTA Image Header 
 */

typedef struct{
	uint32 u32OtaMagicValue;
	/*!< Magic value kept in the OTA image after the 
	sha256 Digest buffer to define the Start of OTA Header */
	uint32 u32OtaPayloadSzie;
	/*!<
	The Total OTA image payload size, include the sha256 key size
	*/

}tstrOtaInitHdr;

/*!
@struct	\	
 	tstrOtaControlSec
 	
@brief		
 	Control section structure is used to define the working image and 
	the validity of the roll-back image and its offset, also both firmware versions is kept in that structure.
	
 */

typedef struct {
	uint32 u32OtaMagicValue;
/*!<
	Magic value used to ensure the structure is valid or not 
*/
	uint32 u32OtaFormatVersion;
/*!<
	Control structure format version, the value will be incremented in case of structure changed or updated
*/
	uint32 u32OtaSequenceNumber;
/*!<
	Sequence number is used while update the control structure to keep track of how many times that section updated 
*/
	uint32 u32OtaLastCheckTime;
/*!<
	Last time OTA check for update
*/
	uint32 u32OtaCurrentworkingImagOffset;
/*!<
	Current working offset in flash 
*/
	uint32 u32OtaCurrentworkingImagFirmwareVer;
/*!<
	current working image version ex 18.0.1
*/
	uint32 u32OtaRollbackImageOffset;
/*!<
	Roll-back image offset in flash 
*/
	uint32 u32OtaRollbackImageValidStatus;
/*!<
	roll-back image valid status 
*/
	uint32 u32OtaRollbackImagFirmwareVer;
/*!<
	Roll-back image version (ex 18.0.3)
*/
	uint32 u32OtaControlSecCrc;
/*!<
	Crc for the control structure to ensure validity 
*/
} tstrOtaControlSec;

/*!
@enum	\
	tenuOtaUpdateStatus

@brief
	OTA return status 
*/
typedef enum {
	OTA_STATUS_SUCSESS        = 0,   
	/*!< OTA Success with not errors. */
	OTA_STATUS_FAIL           = 1,  
	/*!< OTA generic fail. */
	OTA_STATUS_INVAILD_ARG    = 2,
	/*!< Invalid or malformed download URL. */
	OTA_STATUS_INVAILD_RB_IMAGE    = 3,
	/*!< Invalid rollback image. */
	OTA_STATUS_INVAILD_FLASH_SIZE    = 4,
	/*!< The OTA support at least 4MB flash size, 
		failure to provide atleast 4MB from the flash will result in this error  
	*/
	OTA_STATUS_AlREADY_ENABLED    = 5,
	/*!< An OTA operation is already enabled. */
	OTA_STATUS_UPDATE_INPROGRESS    = 6,
	/*!< An OTA operation update is in progress */
} tenuOtaUpdateStatus;
/*!
@enum	\
	tenuOtaUpdateStatusType

@brief
	OTA update Status type
*/
typedef enum { 

	DL_STATUS        = 1,   
	/*!< Download OTA file status
	*/
	SW_STATUS        = 2,   
	/*!< Switching to the upgrade firmware status
	*/
	RB_STATUS        = 3,   
	/*!< Roll-back status
	*/
}tenuOtaUpdateStatusType;


/*!
@struct	\
	tstrOtaUpdateStatusResp

@brief	
	OTA Update Information

@sa 
	tenuWPSTrigger
*/
typedef struct {
	uint8	u8OtaUpdateStatusType;
	/*!< 
		Status type tenuOtaUpdateStatusType
	*/
	uint8	u8OtaUpdateStatus;
	/*!< 
	OTA_SUCCESS 						
	OTA_ERR_WORKING_IMAGE_LOAD_FAIL		
	OTA_ERR_INVAILD_CONTROL_SEC			
	M2M_ERR_OTA_SWITCH_FAIL     		
	M2M_ERR_OTA_START_UPDATE_FAIL     	
	M2M_ERR_OTA_ROLLBACK_FAIL     		
	M2M_ERR_OTA_INVAILD_FLASH_SIZE     	
	M2M_ERR_OTA_INVAILD_ARG		     
	*/
	uint8 _PAD16_[2];
}tstrOtaUpdateStatusResp;


/*!
@struct	\
	tstrOtaUpdateInfo

@brief	
	OTA Update Information

@sa 
	tenuWPSTrigger
*/
typedef struct {
	uint32	u8NcfUpgradeVersion;
	/*!< NCF OTA Upgrade Version 
	*/
	uint32	u8NcfCurrentVersion;
	/*!< NCF OTA Current firmware version 
	*/
	uint32	u8NcdUpgradeVersion;
	/*!< NCD (host) upgraded version (if the u8NcdRequiredUpgrade == true)
	*/
	uint8	u8NcdRequiredUpgrade;
	/*!< NCD Required upgrade to the above version 
	*/
	uint8 	u8DownloadUrlOffset;
	/*!< Download URL offset in the received packet
	*/
	uint8 	u8DownloadUrlSize;
	/*!< Download URL size in the received packet
	*/
	uint8	__PAD8__;
	/*!< Padding bytes for forcing 4-byte alignment
	*/
} tstrOtaUpdateInfo;


/*!
@struct	\	
	tstrSystemTime

@brief
	Used for time storage.
*/
typedef struct{
	uint16	u16Year;
	uint8	u8Month;
	uint8	u8Day;
	uint8	u8Hour;
	uint8	u8Minute;
	uint8	u8Second;
}tstrSystemTime;

/*!
@struct	\	
 	tstrM2MMulticastMac
 	
@brief		
 	M2M add/remove multicast mac address
 */
 typedef struct {
	uint8 au8macaddress[M2M_MAC_ADDRES_LEN];
	/*!<
		Mac address needed to be added or removed from filter.
	*/
	uint8 u8AddRemove;
	/*!<
		set by 1 to add or 0 to remove from filter.
	*/
	uint8	__PAD8__;
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MMulticastMac;

/*!
@struct	\	
 	tstrM2MIfId
 	
@brief		
 	Set Intrface ID
 */
 typedef struct {
	uint8 u8IfcId;
	/*!<
		Interface ID.
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MIfId;
 /*!
 @enum	 \
	 tenuM2mTxPwrLevel
	 
 @brief
	 
 */
 typedef enum {
	 TX_PWR_HIGH = ((uint8) 1),
	 /*!< PPA Gain 6dbm  PA Gain 18dbm */
	 TX_PWR_MED,
	 /*!< PPA Gain 6dbm  PA Gain 12dbm */
	 TX_PWR_LOW,
	 /*!< PPA Gain 6dbm  PA Gain 6dbm */
 }tenuM2mTxPwrLevel;
 
 /*!
 @struct \	 
	 tstrM2mTxPwrLevel
 
 @brief
	 Tx power level 
 */
 typedef struct {
	 uint8	 u8TxPwrLevel; 
	 /*!< Tx power level
	 */
	 uint8	 __PAD24__[3];
	 /*!< Padding bytes for forcing 4-byte alignment
	 */
 }tstrM2mTxPwrLevel;


 /**@}*/

#endif
