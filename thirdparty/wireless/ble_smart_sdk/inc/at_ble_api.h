/****************************************************************************
  \file at_ble_api.h
 
  \brief Includes signatures and datatypes for Atmel BLE API for Applications
 
  Copyright (c) 2016, Atmel Corporation. All rights reserved.
  Released under NDA
  Licensed under Atmel's Limited License Agreement.
 
 
  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 
  Atmel Corporation: http://www.atmel.com
 
******************************************************************************/


#ifndef __AT_BLE_API_H__
#define __AT_BLE_API_H__

#include "stdint.h"
#include "stdbool.h"
#include "at_ble_errno.h"


///@cond IGNORE_DOXYGEN
#define AT_BLE_EXPORTS

#ifdef WIN32
#ifdef AT_BLE_EXPORTS
#define AT_BLE_API __declspec(dllexport)
#else  //AT_BLE_EXPORTS
#define AT_BLE_API __declspec(dllimport)
#endif  //AT_BLE_EXPORTS
#else //WIN32
#define AT_BLE_API
#endif //WIN32
///@endcond

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus


/****************************************************************************************
*                                   Macros                                              *
****************************************************************************************/

/** @brief BLE address length. */
#define AT_BLE_ADDR_LEN                     (6)

/** @brief BLE security key maximum length. */
#define AT_BLE_MAX_KEY_LEN                  (16)

/** @brief BLE device name maximum length. */
#define AT_BLE_MAX_NAME_LEN             (0x20)

/**@brief GAP Security Key Length. */
#define AT_BLE_PASSKEY_LEN                  (6)

/** @brief Maximum size of advertising data in octets. */
#define  AT_BLE_ADV_MAX_SIZE                (31)

/** @brief UUID 16 size in bytes */
#define AT_BLE_UUID_16_LEN                  (2)

/** @brief UUID 32 size in bytes */
#define AT_BLE_UUID_32_LEN                  (4)

/** @brief UUID 128 size in bytes */
#define AT_BLE_UUID_128_LEN                 (16)

// GAP Advertising interval max. and min.
#define AT_BLE_ADV_INTERVAL_MIN        0x0020 /**< Minimum Advertising interval in 625 us units, i.e. 20 ms. */
#define AT_BLE_ADV_NONCON_INTERVAL_MIN 0x00A0 /**< Minimum Advertising interval in 625 us units for non connectable mode, i.e. 100 ms. */
#define AT_BLE_ADV_INTERVAL_MAX        0x4000 /**< Maximum Advertising interval in 625 us units, i.e. 10.24 s. */

// Scanning interval max and min
#define AT_BLE_SCAN_INTERVAL_MIN       0x0004 /**<Minimum scanning interval in 625 us units , i.e. 2.5 ms. */
#define AT_BLE_SCAN_INTERVAL_MAX       0x4000 /**<Maximum scanning interval in 625 us units , i.e. 10.24 s. */

// Scanning window max and min
#define AT_BLE_SCAN_WINDOW_MIN         0x0004 /**<Minimum scanning window in 625 us units , i.e. 2.5 ms. */
#define AT_BLE_SCAN_WINDOW_MAX         0x4000 /**<Maximum scanning window in 625 us units , i.e. 10.24 s. */


//Slave preferred params
#define AT_BLE_SLV_PREF_CON_INTV_MIN   0x0140 /**<Minimum Slave Preferred Connection Interval. */
#define AT_BLE_SLV_PREF_CON_INTV_MAX   0x0140 /**<Maximum Slave Preferred Connection Interval. */
#define AT_BLE_SLV_PREF_CON_LATENCY    0x0000 /**<Slave Preferred Connection Latency. */
#define AT_BLE_SLV_PREF_SUPV_TO        0x01F4 /**<Slave Preferred Connection Supervision Timeout. */

/// Connection interval min (N*1.250ms)
#define AT_CNX_INTERVAL_MIN             (6)         //(0x06)
/// Connection interval max (N*1.250ms)
#define AT_CNX_INTERVAL_MAX             (3200)      //(0xC80)
/// Connection latency min (N*cnx evt)
#define AT_CNX_LATENCY_MIN              (0)         //(0x00)
/// Connection latency Max (N*cnx evt
#define AT_CNX_LATENCY_MAX              (499)       //(0x1F3)
/// Supervision TO min (N*10ms)
#define AT_CNX_SUP_TO_MIN               (10)        //(0x0A)
/// Supervision TO Max (N*10ms)
#define AT_CNX_SUP_TO_MAX               (3200)      //(0xC80)
/// Minimal MTU value
#define AT_MTU_VAL_MIN                  (23)        //(0x17)
/// Maximal MTU value
#define AT_MTU_VAL_MAX                  (512)      //(0x200)
/// Recommended MTU value
#define AT_MTU_VAL_RECOMMENDED          (512)       //(0x200)
/// Minimal Renew duration value (150 seconds); resolution of 10 mSeconds (N*10ms)
#define AT_RENEW_DUR_VAL_MIN            (0x3A98)    //(15000)
/// ATT MAximum Attribute Length
#define AT_BLE_MAX_ATT_LEN          512
/// Maximum number of channel mapping
#define AT_BLE_CH_MAP_LEN           (0x05)
/// Default value used to automatic allocate handle
#define AT_BLE_AUTO_ALLOC_HANDLE    (0x0000)
/// Maximum number of connections at the same time
/// Direct test mode defines
#define AT_BLE_HCI_DTM_EVENT 0xBB
#define AT_BLE_MAX_CONCURRENT_CONNS (8)
/// LE credit based max packet size
#define AT_BLE_LECB_MAX_PKT_SIZE    0x200

/****************************************************************************************
*                                   Basic types                                         *
****************************************************************************************/
/**
* Initialization Configuration parameters
*/

typedef struct
{
    uint8_t *memStartAdd;   /**< Memory pool start address*/
    uint32_t memSize;       /**< Assigned memory size*/
} at_ble_mempool_t;

typedef struct
{
    at_ble_mempool_t	memPool; /**< Memory pool that library can use for storing data base related data */
	at_ble_mempool_t    event_mem_pool; /**< Memory pool that the library should use to queue events */
	at_ble_mempool_t   	event_params_mem_pool; /**< Memory pool that library should use to save queued events parameters */
    void 				*plf_config; /**< Platform Configuration*/
} at_ble_init_config_t;

/**
* BLE connection handle
*/
typedef uint16_t at_ble_handle_t;

/****************************************************************************************
*                                   Enumerations                                                                    *
****************************************************************************************/

/// Enumeration for BLE Status
typedef at_ble_err_status_t at_ble_status_t;


/// Enumeration for GAP Parameters
typedef enum
{
    // Timer related
    AT_BLE_GAP_GEN_DISC_ADV_MIN = 0,       //!< Minimum time to remain advertising, when in Discoverable mode (mSec).  Setting this parameter to 0 turns off the time-out (default).
    AT_BLE_GAP_LIM_ADV_TIMEOUT,            //!< Maximum time to remain advertising, when in Limited Discoverable mode. In seconds (default 180 seconds)
    AT_BLE_GAP_GEN_DISC_SCAN,              //!< Minimum time to perform scanning, when performing General Discovery proc (mSec)
    AT_BLE_GAP_LIM_DISC_SCAN,              //!< Minimum time to perform scanning, when performing Limited Discovery proc (mSec)
    AT_BLE_GAP_CONN_EST_ADV_TIMEOUT,       //!< Advertising time-out, when performing Connection Establishment proc (mSec)
    AT_BLE_GAP_CONN_PARAM_TIMEOUT,         //!< Link Layer connection parameter update notification timer, connection parameter update proc (mSec)

    // Constants
    AT_BLE_GAP_LIM_DISC_ADV_INT_MIN,       //!< Minimum advertising interval, when in limited discoverable mode (n * 0.625 mSec)
    AT_BLE_GAP_LIM_DISC_ADV_INT_MAX,       //!< Maximum advertising interval, when in limited discoverable mode (n * 0.625 mSec)
    AT_BLE_GAP_GEN_DISC_ADV_INT_MIN,       //!< Minimum advertising interval, when in General discoverable mode (n * 0.625 mSec)
    AT_BLE_GAP_GEN_DISC_ADV_INT_MAX,       //!< Maximum advertising interval, when in General discoverable mode (n * 0.625 mSec)
    AT_BLE_GAP_CONN_ADV_INT_MIN,           //!< Minimum advertising interval, when in Connectable mode (n * 0.625 mSec)
    AT_BLE_GAP_CONN_ADV_INT_MAX,           //!< Maximum advertising interval, when in Connectable mode (n * 0.625 mSec)
    AT_BLE_GAP_CONN_SCAN_INT,              //!< Scan interval used during Link Layer Initiating state, when in Connectable mode (n * 0.625 mSec)
    AT_BLE_GAP_CONN_SCAN_WIND,             //!< Scan window used during Link Layer Initiating state, when in Connectable mode (n * 0.625 mSec)
    AT_BLE_GAP_CONN_HIGH_SCAN_INT,         //!< Scan interval used during Link Layer Initiating state, when in Connectable mode, high duty scan cycle scan parameters (n * 0.625 mSec)
    AT_BLE_GAP_CONN_HIGH_SCAN_WIND,        //!< Scan window used during Link Layer Initiating state, when in Connectable mode, high duty scan cycle scan parameters (n * 0.625 mSec)
    AT_BLE_GAP_GEN_DISC_SCAN_INT,          //!< Scan interval used during Link Layer Scanning state, when in General Discovery proc (n * 0.625 mSec)
    AT_BLE_GAP_GEN_DISC_SCAN_WIND,         //!< Scan window used during Link Layer Scanning state, when in General Discovery proc (n * 0.625 mSec)
    AT_BLE_GAP_LIM_DISC_SCAN_INT,          //!< Scan interval used during Link Layer Scanning state, when in Limited Discovery proc (n * 0.625 mSec)
    AT_BLE_GAP_LIM_DISC_SCAN_WIND,         //!< Scan window used during Link Layer Scanning state, when in Limited Discovery proc (n * 0.625 mSec)
    AT_BLE_GAP_CONN_EST_ADV,               //!< Advertising interval, when using Connection Establishment proc (n * 0.625 mSec). Obsolete - Do not use.
    AT_BLE_GAP_CONN_EST_INT_MIN,           //!< Minimum Link Layer connection interval, when using Connection Establishment proc (n * 1.25 mSec)
    AT_BLE_GAP_CONN_EST_INT_MAX,           //!< Maximum Link Layer connection interval, when using Connection Establishment proc (n * 1.25 mSec)
    AT_BLE_GAP_CONN_EST_SCAN_INT,          //!< Scan interval used during Link Layer Initiating state, when using Connection Establishment proc (n * 0.625 mSec)
    AT_BLE_GAP_CONN_EST_SCAN_WIND,         //!< Scan window used during Link Layer Initiating state, when using Connection Establishment proc (n * 0.625 mSec)
    AT_BLE_GAP_CONN_EST_SUPERV_TIMEOUT,    //!< Link Layer connection supervision time-out, when using Connection Establishment proc (n * 10 mSec)
    AT_BLE_GAP_CONN_EST_LATENCY,           //!< Link Layer connection slave latency, when using Connection Establishment proc (in number of connection events)
    AT_BLE_GAP_CONN_EST_MIN_CE_LEN,        //!< Local informational parameter about MIN length of connection needed, when using Connection Establishment proc (n * 0.625 mSec)
    AT_BLE_GAP_CONN_EST_MAX_CE_LEN,        //!< Local informational parameter about MAX length of connection needed, when using Connection Establishment proc (n * 0.625 mSec)
    AT_BLE_GAP_PRIVATE_ADDR_INT,           //!< Minimum Time Interval between private (resolvable) address changes. In minutes (default 15 minutes)
    AT_BLE_GAP_CONN_PAUSE_CENTRAL,         //!< Central idle timer. In seconds (default 1 second)
    AT_BLE_GAP_CONN_PAUSE_PERIPHERAL      //!< Minimum time upon connection establishment before the peripheral starts a connection update procedure. In seconds (default 5 seconds)
} at_ble_parameters_t;


/// GAP Advertising types
typedef enum
{
    AT_BLE_ADV_TYPE_UNDIRECTED    = 0x00,   /**< Connectable undirected. */
    AT_BLE_ADV_TYPE_DIRECTED,               /**< Connectable high duty cycle directed advertising. */
    AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED,   /**< Scannable undirected. */
    AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED,     /**< Non connectable undirected. */
    AT_BLE_ADV_TYPE_DIRECTED_LDC,           /**< Connectable low duty cycle directed advertising. */
    AT_BLE_ADV_TYPE_SCAN_RESPONSE           /** only used in @ref AT_BLE_SCAN_INFO event to signify a scan response*/
} at_ble_adv_type_t;

///Advertising channels enables
typedef enum
{
    ///Byte value for advertising channel map for channel 37 enable
    AT_BLE_ADV_CHNL_37_EN                = 0x01,
    ///Byte value for advertising channel map for channel 38 enable
    AT_BLE_ADV_CHNL_38_EN,
    ///Byte value for advertising channel map for channel 39 enable
    AT_BLE_ADV_CHNL_39_EN                = 0x04,
    ///Byte value for advertising channel map for channel 37, 38 and 39 enable
    AT_BLE_ADV_ALL_CHNLS_EN              = 0x07,
    ///Enumeration end value for advertising channels enable value check
    AT_BLE_ADV_CHNL_END
} at_ble_adv_channel_map_t;


///TX Power levels
typedef enum
{
    AT_BLE_TX_PWR_LVL_NEG_55_DB = 0x00,
    AT_BLE_TX_PWR_LVL_NEG_20_DB = 0x01,
    AT_BLE_TX_PWR_LVL_NEG_14_DB = 0x02,
    AT_BLE_TX_PWR_LVL_NEG_11_DB = 0x03,
    AT_BLE_TX_PWR_LVL_NEG_09_DB = 0x04,
    AT_BLE_TX_PWR_LVL_NEG_07_DB = 0x05,
    AT_BLE_TX_PWR_LVL_NEG_06_DB = 0x06,
    AT_BLE_TX_PWR_LVL_NEG_05_DB = 0x07,
    AT_BLE_TX_PWR_LVL_NEG_04_DB = 0x08,
    AT_BLE_TX_PWR_LVL_NEG_03_DB = 0x09,
    AT_BLE_TX_PWR_LVL_NEG_02_DB = 0x0B,
    AT_BLE_TX_PWR_LVL_NEG_01_DB = 0x0D,
    AT_BLE_TX_PWR_LVL_ZERO_DB   = 0x11,
    AT_BLE_TX_PWR_LVL_POS_01_DB = 0x15,
    AT_BLE_TX_PWR_LVL_POS_02_DB = 0x20,
    AT_BLE_TX_PWR_LVL_POS_03_DB = 0x3B
} at_ble_tx_power_level_t;

/**@brief Events delivered from BLE stack to the application layer
*/
typedef enum
{
    /* GAP events */
    /** Undefined event received  */
    AT_BLE_UNDEFINED_EVENT,
    /** Scan info needs to be delivered either adv data or scan response data. \n
     * Refer to @ref at_ble_scan_info_t
     */
    AT_BLE_SCAN_INFO,
    /** Scan report received at the end of scan period if @ref AT_BLE_SCAN_GEN_DISCOVERY or @ref AT_BLE_SCAN_LIM_DISCOVERY are used. \n
     * Refer to @ref at_ble_scan_report_t
     */
    AT_BLE_SCAN_REPORT,
    /** Advertising report received if error has occurred or timeout happened.
    * Refer to @ref at_ble_adv_report_t
    */
    AT_BLE_ADV_REPORT,
    /** Used random address. \n
     *  Refer to @ref at_ble_rand_addr_changed_t
     */
    AT_BLE_RAND_ADDR_CHANGED,
    /** connected to a peer device. \n
     *  Refer to at_ble_connected_t
     */
    AT_BLE_CONNECTED,
    /** peer device connection terminated. \n
     *  Refer to @ref at_ble_disconnected_t and @ref at_ble_disconnect_reason_t for reason of disconnection.
     *  If returned reason is not one of @ref at_ble_disconnect_reason_t, so check for error code @ref at_ble_status_t
     */
    AT_BLE_DISCONNECTED,
    /** connection parameters updated. It is requires to call @ref at_ble_conn_update_reply function to send response back if needed.\n
     * Refer to @ref at_ble_conn_param_update_done_t
     */
    AT_BLE_CONN_PARAM_UPDATE_DONE,
    /** peer device asks for connection parameters update. \n
    *  Refer to at_ble_conn_param_update_request_t
    */
    AT_BLE_CONN_PARAM_UPDATE_REQUEST,
    /** Pairing procedure is completed. \n
     *  Refer to at_ble_pair_done_t
     */
    AT_BLE_PAIR_DONE,
    /** A central device asks for Pairing. \n
     * Refer to at_ble_pair_request_t
     */
    AT_BLE_PAIR_REQUEST,
    /** Slave security request. \n
     *  Refer to at_ble_slave_sec_request_t
     */
    AT_BLE_SLAVE_SEC_REQUEST,
    /** A passkey or OOB data is requested as part of pairing procedure. \n
     * Refer to @ref at_ble_pair_key_request_t
     */
    AT_BLE_PAIR_KEY_REQUEST,
    /** Encryption is requested by a master device. \n
     *  Refer to at_ble_encryption_request_t
     */
    AT_BLE_ENCRYPTION_REQUEST,
    /** Encryption status changed. \n
     *  Refer to at_ble_encryption_status_changed_t
     */
    AT_BLE_ENCRYPTION_STATUS_CHANGED,
    /** Resolve random address status. \n
     *  Refer to at_ble_resolv_rand_addr_status_t
     */
    AT_BLE_RESOLV_RAND_ADDR_STATUS,
    /** Signature counters new values indication
     *  Refer to at_ble_sign_counter_t
     */
    AT_BLE_SIGN_COUNTERS_IND,
    /** peer attribute info received
      * Refer to @ref at_ble_peer_att_info_ind_t
      */
    AT_BLE_PEER_ATT_INFO_IND,
    /** peer device channel map received
      * Refer to @ref at_ble_channel_map_t
      */
    AT_BLE_CON_CHANNEL_MAP_IND,
    /* GATT Client events */
    /** A primary service is found. \n
     * Refer to @ref at_ble_primary_service_found_t
     */
    AT_BLE_PRIMARY_SERVICE_FOUND,
    /** An included service is found . \n
     * Refer to @ref at_ble_included_service_found_t
     */
    AT_BLE_INCLUDED_SERVICE_FOUND,
    /** A Characteristic is found. \n
     * Refer to @ref at_ble_characteristic_found_t
     */
    AT_BLE_CHARACTERISTIC_FOUND,
    /** A descriptor is found. \n
     * Refer to @ref at_ble_descriptor_found_t
     */
    AT_BLE_DESCRIPTOR_FOUND,
    /** A discover operation has completed. \n
     * Refer to @ref at_ble_discovery_complete_t
     */
    AT_BLE_DISCOVERY_COMPLETE,
    /** Characteristic read procedure is done. \n
     * Refer to @ref at_ble_characteristic_read_response_t
     */
    AT_BLE_CHARACTERISTIC_READ_RESPONSE,
    /** Characteristic read by UUID procedure is done. \n
     *  Legacy event use AT_BLE_CHARACTERISTIC_READ_RESPONSE
     *  Refer to at_ble_characteristic_read_response_t
     */
    AT_BLE_CHARACTERISTIC_READ_BY_UUID_RESPONSE = AT_BLE_CHARACTERISTIC_READ_RESPONSE,

    /** Characteristic multiple read procedure is done. \n
      * Legacy event use AT_BLE_CHARACTERISTIC_READ_RESPONSE
      * Refer to @ref at_ble_characteristic_read_response_t
      */
    AT_BLE_CHARACTERISTIC_READ_MULTIBLE_RESPONSE,
    /** Characteristic write procedure is done. \n
      * Refer to @ref at_ble_characteristic_write_response_t
      */
    AT_BLE_CHARACTERISTIC_WRITE_RESPONSE,
    /** A Notification is received. \n
      * Refer to @ref at_ble_notification_recieved_t
      */
    AT_BLE_NOTIFICATION_RECIEVED,
    /** An Indication is received. \n
     * Refer to @ref at_ble_indication_recieved_t
     */
    AT_BLE_INDICATION_RECIEVED,
    /* GATT Server events */
    /** The firmware confirmed that an Notification PDU has been sent over the air. \n
      * Refer to @ref at_ble_cmd_complete_event_t
      */
    AT_BLE_NOTIFICATION_CONFIRMED,
    /** The peer confirmed that it has received an Indication. \n
     * Refer to @ref at_ble_cmd_complete_event_t
     */
    AT_BLE_INDICATION_CONFIRMED,
    /** The peer has changed a characteristic value. \n
      * Refer to @ref at_ble_characteristic_changed_t
      */
    AT_BLE_CHARACTERISTIC_CHANGED,
    /** The peer has changed a characteristic configuration descriptor value. \n
      * Refer to @ref at_ble_characteristic_configuration_changed_t
      */
    AT_BLE_CHARACTERISTIC_CONFIGURATION_CHANGED,
    /** the service changed indication had been sent . \n
      * Refer to @ref at_ble_cmd_complete_event_t
      */
    AT_BLE_SERVICE_CHANGED_INDICATION_SENT,
    /** The peer asks for a write Authorization. \n
      * Refer to @ref at_ble_characteristic_write_request_t
      */
    AT_BLE_WRITE_AUTHORIZE_REQUEST,
    /**  peer sends an indication of the new MTU. \n
      * Refer to @ref at_ble_mtu_changed_ind_t
      */
    AT_BLE_MTU_CHANGED_INDICATION,
    /** MTU Exchange completed. \n
     * Refer to @ref at_ble_cmd_complete_event_t
     */
    AT_BLE_MTU_CHANGED_CMD_COMPLETE,
    /** write command complete. \n
     * Refer to @ref at_ble_cmd_complete_event_t
     */
    AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP,
    /** The peer asks for a read Authorization. \n
     * Refer to @ref at_ble_read_authorize_request_t
     */
    AT_BLE_READ_AUTHORIZE_REQUEST,

    /* L2CAP events */
    /** Connection request is received from server. \n
     * Refer to @ref at_ble_lecb_conn_req_t
     */
    AT_BLE_LECB_CONN_REQ,
    /** Peer connected successfully. \n
     * Refer to @ref at_ble_lecb_connected_t
     */
    AT_BLE_LECB_CONNECTED,
    /** Peer disconnected. \n
     * Refer to @ref at_ble_lecb_disconnected_t
     */
    AT_BLE_LECB_DISCONNECTED,
    /** Indication when peer device added credit. \n
     * Refer to @ref at_ble_lecb_add_credit_ind_t
     */
    AT_BLE_LECB_ADD_CREDIT_IND,
    /** Response from local device to data send command. \n
     * Refer to @ref at_ble_lecb_send_rsp_t
     */
    AT_BLE_LECB_SEND_RESP,
    /** Data received from peer device. \n
     * Refer to @ref at_ble_lecb_data_recv_t
     */
    AT_BLE_LECB_DATA_RECIEVED,


    /* HTPT Health Thermometer Profile events */
    /** Inform APP of database creation status. \n
      * Refer to @ref at_ble_htpt_create_db_cfm_t
      */
    AT_BLE_HTPT_CREATE_DB_CFM,
    /** Error indication to APP. \n
      * Refer to @ref at_ble_prf_server_error_ind_t
      */
    AT_BLE_HTPT_ERROR_IND,
    /** Automatically sent to the APP after a disconnection with the peer device to confirm disabled profile. \n
      * Refer to @ref at_ble_htpt_disable_ind_t
      */
    AT_BLE_HTPT_DISABLE_IND,
    /** Temperature value confirm to APP. \n
      * Refer to @ref at_ble_htpt_temp_send_cfm_t
      */
    AT_BLE_HTPT_TEMP_SEND_CFM,
    /** Inform APP of new measurement interval value. \n
      * Refer to @ref at_ble_htpt_meas_intv_chg_ind_t
      */
    AT_BLE_HTPT_MEAS_INTV_CHG_IND,
    /** Inform APP of new configuration value. \n
      * Refer to @ref at_ble_htpt_cfg_indntf_ind_t
      */
    AT_BLE_HTPT_CFG_INDNTF_IND,

    /** HTPT profile enable confirmation. \n
      * Refer to @ref at_ble_htpt_enable_rsp_t
      */
    AT_BLE_HTPT_ENABLE_RSP,
    /** Response to APP for measurement interval update request. \n
      * Refer to @ref at_ble_htpt_meas_intv_upd_rsp_t
      */
    AT_BLE_HTPT_MEAS_INTV_UPD_RSP,
    /** Inform APP of new measurement interval value requested by a peer device. \n
      * Refer to @ref at_ble_htpt_meas_intv_chg_req_t
      */
    AT_BLE_HTPT_MEAS_INTV_CHG_REQ,
    /* DTM events */
    /** inform app about DTM command test status
     *  Refer to @ref at_ble_dtm_t
     */
    AT_BLE_LE_TEST_STATUS,
    /** inform app about the RX packets report
     *  Refer to @ref at_ble_dtm_t
     */
    AT_BLE_LE_PACKET_REPORT,
    /* Custom user defined events */
    /** A user-defined event is delivered to the system */
    AT_BLE_CUSTOM_EVENT,

    AT_BLE_DEVICE_READY,

	/** A platform event mostly from peripheral is delivered to application system */
	AT_PLATFORM_EVENT, 
	
    AT_BLE_EVENT_MAX

} at_ble_events_t;

/**@brief BLE can accept to kinds of addresses, either public or random addresses
*/
typedef enum
{
    /** a public static address */
    AT_BLE_ADDRESS_PUBLIC,
    /** a random static address */
    AT_BLE_ADDRESS_RANDOM_STATIC,
    /** resolvable private random address */
    AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE,
    /** non-resolvable private random address */
    AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE ,

} at_ble_addr_type_t;


/**@brief disconnection reasons
*/
typedef enum
{
    AT_BLE_AUTH_FAILURE = 0x05,
	AT_BLE_SUPERVISION_TIMEOUT = 0x08,
    AT_BLE_TERMINATED_BY_USER = 0x13,
    AT_BLE_REMOTE_DEV_TERM_LOW_RESOURCES,
    AT_BLE_REMOTE_DEV_POWER_OFF,
	AT_BLE_CON_TERM_BY_LOCAL_HOST,
    AT_BLE_UNSUPPORTED_REMOTE_FEATURE = 0x1A,
    AT_BLE_PAIRING_WITH_UNIT_KEY_NOT_SUP = 0x29,
    AT_BLE_UNACCEPTABLE_INTERVAL = 0x3B,
} at_ble_disconnect_reason_t;

/**@brief a device IO capabilities
*/
typedef enum
{
    /** Can only display */
    AT_BLE_IO_CAP_DISPLAY_ONLY,
    /** Can Display and get a Yes/No input from user*/
    AT_BLE_IO_CAP_DISPLAY_YES_NO,
    /** Has only a keyboard */
    AT_BLE_IO_CAP_KB_ONLY,
    /** Has no input and no output */
    AT_BLE_IO_CAP_NO_INPUT_NO_OUTPUT,
    /** Has both a display and a keyboard */
    AT_BLE_IO_CAP_KB_DISPLAY,

} at_ble_iocab_t;

/**@brief Security keys distribution list
*/
typedef enum
{
    /** No keys to distribute */
    AT_BLE_KEY_DIST_NONE = 0x00,
    /** Distribute Encryption Key */
    AT_BLE_KEY_DIST_ENC = (1 << 0),
    /** Distribute ID Key */
    AT_BLE_KEY_DIST_ID  = (1 << 1),
    /** Distribute Signature Key */
    AT_BLE_KEY_DIST_SIGN = (1 << 2),
    /** Distribute All Keys */
    AT_BLE_KEY_DIS_ALL = AT_BLE_KEY_DIST_ENC
                         | AT_BLE_KEY_DIST_ID
                         | AT_BLE_KEY_DIST_SIGN,

} at_ble_key_dis_t;

/**@brief Security authentication level
*/
typedef enum
{
    /** no security */
    AT_BLE_NO_SEC = 0x00,
    /** Gap Mode 1 Level 1, Unauthenticated pairing with encryption, Man in the middle protection not checked, a LTK shall be exchanged */
    AT_BLE_MODE1_L1_NOAUTH_PAIR_ENC,
    /** Gap Mode 1 Level 2, Authenticated pairing with encryption, Man in the middle protection shall be set to 1, a LTK shall be exchanged */
    AT_BLE_MODE1_L2_AUTH_PAIR_ENC,
    /**  Gap Mode 2 Level 1, Unauthenticated pairing with data signing, Man in the middle protection not checked , a CSRK shall be exchanged */
    AT_BLE_MODE2_L1_NOAUTH_DATA_SGN,
    /**  Gap Mode 2 Level 2, Authentication pairing with data signing, Man in the middle protection shall be set to 1, a CSRK shall be exchanged */
    AT_BLE_MODE2_L2_AUTH_DATA_SGN,

} at_ble_auth_levels_t;

/**@brief UUID Type
*/
typedef enum
{
    /**  16 Bit UUID */
    AT_BLE_UUID_16 = 0,
    /**  32 Bit UUID */
    AT_BLE_UUID_32 = 1,
    /**  128 Bit UUID */
    AT_BLE_UUID_128 = 2,
    /**  Invalid UUID */
    AT_BLE_UUID_INVALID = 3,
} at_ble_uuid_type_t;

/**@brief Characteristic properties
*/
/*
typedef enum{
    AT_BLE_CHAR_BROADCST = (1 << 0),
    AT_BLE_CHAR_READ = (1 << 1),
    AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE = (1 << 2),
    AT_BLE_CHAR_WRITE = (1 << 3),
    AT_BLE_CHAR_NOTIFY = (1 << 4),
    AT_BLE_CHAR_INDICATE = (1 << 5),
    AT_BLE_CHAR_SIGNED_WRITE = (1 << 6),
    AT_BLE_CHAR_RELIABLE_WRITE = (1 << 7),
    AT_BLE_CHAR_WRITEABLE_AUX = (1 << 8),
}at_ble_char_properties_t;
*/
typedef uint16_t at_ble_char_properties_t;
#define AT_BLE_CHAR_BROADCST (1 << 0)
#define AT_BLE_CHAR_READ (1 << 1)
#define AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE (1 << 2)
#define AT_BLE_CHAR_WRITE (1 << 3)
#define AT_BLE_CHAR_NOTIFY (1 << 4)
#define AT_BLE_CHAR_INDICATE (1 << 5)
#define AT_BLE_CHAR_SIGNED_WRITE (1 << 6)
#define AT_BLE_CHAR_RELIABLE_WRITE (1 << 7)
#define AT_BLE_CHAR_WRITEABLE_AUX (1 << 8)
/**@brief Advertising Filter Policy
*/
typedef enum
{
    /** Allow both scan and connection requests from anyone */
    AT_BLE_ADV_FP_ANY,
    /** Allow both scan request from White List devices only and connection request from anyone */
    AT_BLE_ADV_FP_FILTER_SCANREQ,
    /** Allow both scan request from anyone and connection request from White List devices only */
    AT_BLE_ADV_FP_FILTER_CONNREQ,
    /** Allow scan and connection requests from White List devices only */
    AT_BLE_ADV_FP_FILTER_BOTH
} at_ble_filter_type_t;

/**@brief Advertising Mode
*/
typedef enum
{
    /// Mode in non-discoverable
    AT_BLE_ADV_NON_DISCOVERABLE,
    /// Mode in general discoverable, AD type general flag in Flags set to 1.
    AT_BLE_ADV_GEN_DISCOVERABLE,
    /// Mode in limited discoverable, AD type limited flag in Flags set to 1 (This mode is automatically stopped after 180 sec of activity)
    AT_BLE_ADV_LIM_DISCOVERABLE,
    /// Broadcaster mode which is a non discoverable and non connectable mode.
    AT_BLE_ADV_BROADCASTER_MODE
} at_ble_adv_mode_t;

/**@brief Scan modes used at @ref at_ble_scan_start
*/
typedef enum
{
    /** Send advertising report about device that advertise in limited or general mode ,the operation stops after 10 sec of activity */
    AT_BLE_SCAN_GEN_DISCOVERY,
    /** Send advertising report about device that advertise in limited mode ,the operation stops after 10 sec of activity */
    AT_BLE_SCAN_LIM_DISCOVERY,
    /** Send any advertising report ,this operation can only be stopped by @ref at_ble_scan_stop */
    AT_BLE_SCAN_OBSERVER_MODE
} at_ble_scan_mode_t;

/**@brief Scan types used at @ref at_ble_scan_start
*/
typedef enum
{
    /** No SCAN_REQ packets shall be sent */
    AT_BLE_SCAN_PASSIVE,
    /** SCAN_REQ packets may be sent */
    AT_BLE_SCAN_ACTIVE
} at_ble_scan_type_t;


typedef uint8_t at_ble_attr_permissions_t;

#define AT_BLE_ATTR_NO_PERMISSIONS               0x00
#define AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR   0x01
#define AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR  0x02
#define AT_BLE_ATTR_READABLE_NO_AUTHN_REQ_AUTHR  0x03
#define AT_BLE_ATTR_READABLE_REQ_AUTHN_REQ_AUTHR 0x04
#define AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR   0x10
#define AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR  0x20
#define AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR  0x30
#define AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR 0x40

typedef enum
{
    AT_BLE_PRES_FORMAT_BOOLEAN = 0X01,
    AT_BLE_PRES_FORMAT_2BIT = 0X02,
    AT_BLE_PRES_FORMAT_NIBBLE = 0X03,
    AT_BLE_PRES_FORMAT_UINT8 = 0X04,
    AT_BLE_PRES_FORMAT_UINT12 = 0X05,
    AT_BLE_PRES_FORMAT_UINT16 = 0X06,
    AT_BLE_PRES_FORMAT_UINT24 = 0X07,
    AT_BLE_PRES_FORMAT_UINT32 = 0X08,
    AT_BLE_PRES_FORMAT_UINT48 = 0X09,
    AT_BLE_PRES_FORMAT_UINT64 = 0X0A,
    AT_BLE_PRES_FORMAT_UINT128 = 0X0B,
    AT_BLE_PRES_FORMAT_SINT8 = 0X0C,
    AT_BLE_PRES_FORMAT_SINT12 = 0X0D,
    AT_BLE_PRES_FORMAT_SINT16 = 0X0E,
    AT_BLE_PRES_FORMAT_SINT24 = 0X0F,
    AT_BLE_PRES_FORMAT_SINT32 = 0X10,
    AT_BLE_PRES_FORMAT_SINT48 = 0X11,
    AT_BLE_PRES_FORMAT_SINT64 = 0X12,
    AT_BLE_PRES_FORMAT_SINT128 = 0X13,
    AT_BLE_PRES_FORMAT_FLOAT32 = 0X14,
    AT_BLE_PRES_FORMAT_FLOAT64 = 0X15,
    AT_BLE_PRES_FORMAT_SFLOAT = 0X16,
    AT_BLE_PRES_FORMAT_FLOAT = 0X17,
    AT_BLE_PRES_FORMAT_DUINT16 = 0X18,
    AT_BLE_PRES_FORMAT_UTF8S = 0X19,
    AT_BLE_PRES_FORMAT_UTF16S = 0X1A,
    AT_BLE_PRES_FORMAT_STRUCT = 0X1B,
} at_ble_char_pres_format_t;

/**@brief Pairing Key Type
*/
typedef enum
{
    /** A passkey is required @ref at_ble_passkey_type_t for more details */
    AT_BLE_PAIR_PASSKEY,
    /** Out of band key is required */
    AT_BLE_PAIR_OOB
} at_ble_pair_key_type_t;
/**@brief Passkey Type
*/
typedef enum
{
    /** A passkey entry required */
    AT_BLE_PAIR_PASSKEY_ENTRY,
    /** A passkey needs to be generated and displayed as part of pairing procedure */
    AT_BLE_PAIR_PASSKEY_DISPLAY
} at_ble_passkey_type_t;

typedef enum
{
    /** No Man In The Middle protection(MITM) , No Bonding */
    AT_BLE_AUTH_NO_MITM_NO_BOND,
    /** No MITM , Bonding */
    AT_BLE_AUTH_NO_MITM_BOND = 0x01,
    /** MITM , No Bonding */
    AT_BLE_AUTH_MITM_NO_BOND = 0x04,
    /** MITM and Bonding */
    AT_BLE_AUTH_MITM_BOND = (AT_BLE_AUTH_MITM_NO_BOND |
                             AT_BLE_AUTH_NO_MITM_BOND)
} at_ble_auth_t;

/// Authorization setting
typedef enum
{
    /// Authorization not set, application informed when authorization requested
    GAP_AUTHZ_NOT_SET = 0x00,
    /// Authorization request automatically accepted
    GAP_AUTHZ_ACCEPT  = 0x01,
    /// Authorization request automatically rejected
    GAP_AUTHZ_REJECT  = 0x02
} at_ble_author_t;

typedef enum
{
    /** Disable write access */
    AT_BLE_WRITE_DISABLE = 0,
    /** Enable write access */
    AT_BLE_WRITE_ENABLE,
    /** Write access require unauthenticated link */
    AT_BLE_WRITE_UNAUTH,
    /** Write access require authenticated link */
    AT_BLE_WRITE_AUTH
} at_ble_att_write_perm_t;

///LE Credit based security levels
typedef enum
{
    /// No Security
    AT_BLE_LECB_DISABLE = 0x00,
    /// Enable Security
    AT_BLE_LECB_ENABLE = 0x01,
    /// Minimum Unauthenticated Link (Just Works Pairing)
    AT_BLE_LECB_UNAUTH = 0x02,
    /// Authenticated Link (Pin code Pairing)
    AT_BLE_LECB_AUTH = 0x03,
} at_ble_lecb_sec_level_t;

///LE Credit based connection status codes
typedef enum
{
    /// Connection successful
    AT_BLE_LECB_SUCCESS = 0x00,
    /// Connection refused LE PSM not supported
    AT_BLE_LECB_FAIL_PSM_NOT_SUPPORTED = 0x02,
    /// Connection refused no resources available
    AT_BLE_LECB_FAIL_NO_AVAIL_RESOURCES = 0x04,
    /// Connection refused insufficient authentication
    AT_BLE_LECB_FAIL_INSUFF_AUTHEN = 0x05,
    /// Connection refused insufficient authorization
    AT_BLE_LECB_FAIL_INSUFF_AUTHOR = 0x06,
    /// Connection refused insufficient encryption key size
    AT_BLE_LECB_FAIL_INSUFF_EKS = 0x07,
    /// Connection refused insufficient encryption
    AT_BLE_LECB_FAIL_INSUFF_ENC = 0x08,
} at_ble_lecb_status;



/// Connection type
typedef enum
{
    ///Discovery type connection
    PRF_CON_DISCOVERY = 0x00,
    /// Normal type connection
    PRF_CON_NORMAL    = 0x01
} at_ble_prf_con_type_t;

/// Discovery status
typedef at_ble_status_t at_ble_gattc_discovery_complete_status_t;

/**@brief HTPT Database Configuration Flags
*/
typedef enum
{
    ///Indicate if Temperature Type Char. is supported
    HTPT_TEMP_TYPE_CHAR_SUP        = 0x01,
    ///Indicate if Intermediate Temperature Char. is supported
    HTPT_INTERM_TEMP_CHAR_SUP      = 0x02,
    ///Indicate if Measurement Interval Char. is supported
    HTPT_MEAS_INTV_CHAR_SUP        = 0x04,
    ///Indicate if Measurement Interval Char. supports indications
    HTPT_MEAS_INTV_IND_SUP         = 0x08,
    ///Indicate if Measurement Interval Char. is writable
    HTPT_MEAS_INTV_WR_SUP          = 0x10,

    /// All Features supported
    HTPT_ALL_FEAT_SUP              = 0x1F,
} at_ble_htpt_db_config_flag;

/**@brief Service Multi Instantiated
*/
typedef enum
{
    ///Task that manage service is not multi-instantiated
    AT_BLE_SI = 0x00,
    ///Task that manage service is multi-instantiated
    AT_BLE_MI = 0x01
} at_ble_multi_inst;

/**@brief Check Encryption Key Size
*/
typedef enum
{
    ///Key must not be 16 bit
    AT_BLE_NO_EKS = 0x00,
    ///Key must be 16 bit
    AT_BLE_EKS = 0x02
} at_ble_eks;

/**@brief Service Security Level
*/

typedef enum
{
    /// Service can be browsed but attributes cannot be accessed
    HTPT_DISABLE = 0x00,
    /// Service can be browsed and attributes can be accessed according to attribute permission
    HTPT_ENABLE = 0x04,
    /// Service can be used with minimum Unauthenticated Link (Just Work Pairing)
    HTPT_UNAUTH = 0x08,
    /// Service can be used with Authenticated Link (Pin code Pairing)
    HTPT_AUTH = 0x0C
} at_ble_htpt_sec_level;

/**@brief Attribute Table Indexes
*/
typedef enum
{
    HTPT_TEMP_MEAS_CHAR,
    HTPT_TEMP_TYPE_CHAR,
    HTPT_INTERM_TEMP_CHAR,
    HTPT_MEAS_INTV_CHAR,

    HTPT_CHAR_MAX
} at_ble_htpt_attr_index;

/**@brief Temperature Measurement Flags field bit values
*/
typedef enum
{
    /// Temperature Units Flag - Celsius
    HTPT_FLAG_CELSIUS             = 0x00,
    /// Temperature Units Flag - Fahrenheit
    HTPT_FLAG_FAHRENHEIT,
    /// Time Stamp Flag
    HTPT_FLAG_TIME,
    /// Temperature Type Flag
    HTPT_FLAG_TYPE                 = 0x04
} at_ble_htpt_temp_flags;


/**@brief Temperature Type Description
*/
typedef enum
{
    /// Armpit
    HTP_TYPE_ARMPIT                   = 0x01,
    /// Body (general)
    HTP_TYPE_BODY                     = 0x02,
    /// Ear (usually ear lobe)
    HTP_TYPE_EAR                      = 0x03,
    /// Finger
    HTP_TYPE_FINGER                   = 0x04,
    /// Gastro-intestinal Tract
    HTP_TYPE_GASTRO_INTESTINAL_TRACT  = 0x05,
    /// Mouth
    HTP_TYPE_MOUTH                    = 0x06,
    /// Rectum
    HTP_TYPE_RECTUM                   = 0x07,
    /// Toe
    HTP_TYPE_TOE                      = 0x08,
    /// Tympanum (ear drum)
    HTP_TYPE_TYMPANUM                 = 0x09
} at_ble_htpt_temp_type;

/**@brief Connection Type
*/
typedef enum
{
    HTPT_CONFIG_CONN = 0,
    HTPT_NORMAL_CONN = 1
} at_ble_htpt_conn_type;

/**@brief Enable/Disable Notification/Indication
*/
typedef enum
{
    /// Stop notification/indication
    HTPT_STOP_NTFIND = 0x0000,
    /// Start notification
    HTPT_START_NTF,
    /// Start indication
    HTPT_START_IND,
} at_ble_htpt_ntfind;


/**@brief Enable Notification/Indication for HTPT characteristics
*/
typedef enum
{
    /// Stable measurement interval indication enabled
    HTPT_CFG_STABLE_MEAS_IND    = (1 << 0),
    /// Intermediate measurement notification enabled
    HTPT_CFG_INTERM_MEAS_NTF    = (1 << 1),
    /// Measurement interval indication
    HTPT_CFG_MEAS_INTV_IND      = (1 << 2),
} at_ble_htpt_ntf_ind_cfg;

/// Constant defining the role
typedef enum
{
    /// No role set yet
    AT_BLE_ROLE_NONE        = 0x00,
    /// Observer role
    AT_BLE_ROLE_OBSERVER    = 0x01,
    /// Broadcaster role
    AT_BLE_ROLE_BROADCASTER = 0x02,
    /// Master/Central role
    AT_BLE_ROLE_CENTRAL     = (0x04 | AT_BLE_ROLE_OBSERVER),
    /// Peripheral/Slave role
    AT_BLE_ROLE_PERIPHERAL  = (0x08 | AT_BLE_ROLE_BROADCASTER),
    /// Device has all role, both peripheral and central
    AT_BLE_ROLE_ALL         = (AT_BLE_ROLE_CENTRAL | AT_BLE_ROLE_PERIPHERAL),
    ///Enumeration end value for role value check
    AT_BLE_ROLE_END
} at_ble_dev_role_t;

/// Constant defining the connection modes of operations
typedef enum
{
    /// Direct connection operation
    AT_BLE_CONNECTION_DIRECT = 1,
    /// Automatic connection operation
    AT_BLE_CONNECTION_AUTO,
} at_ble_conn_mode_t;

typedef enum
{
    AT_BLE_DEVICE_VERSION,
    AT_BLE_DEVICE_ADDRESS,
    //AT_BLE_DEVICE_ADV_TX_PWR, /*Not supported*/
    //AT_BLE_DEVICE_MEM_INFO,   /*Not supported*/
} at_ble_get_dev_info_op_t;


/// request operation type - application interface
enum at_ble_operation
{
    /*              Attribute Client Flags              */
    /* No Operation (if nothing has been requested)     */
    /* ************************************************ */
    /// No operation
    AT_BLE_NO_OP                                    = 0x00,

    /* Operation flags for MTU Exchange                 */
    /* ************************************************ */
    /// Perform MTU exchange
    AT_BLE_MTU_EXCH,

    /*      Operation flags for discovery operation     */
    /* ************************************************ */
    /// Discover all services
    AT_BLE_DISC_ALL_SVC,
    /// Discover services by UUID
    AT_BLE_DISC_BY_UUID_SVC,
    /// Discover included services
    AT_BLE_DISC_INCLUDED_SVC,
    /// Discover all characteristics
    AT_BLE_DISC_ALL_CHAR,
    /// Discover characteristic by UUID
    AT_BLE_DISC_BY_UUID_CHAR,
    /// Discover characteristic descriptor
    AT_BLE_DISC_DESC_CHAR,

    /* Operation flags for reading attributes           */
    /* ************************************************ */
    /// Read attribute
    AT_BLE_READ,
    /// Read long attribute
    AT_BLE_READ_LONG,
    /// Read attribute by UUID
    AT_BLE_READ_BY_UUID,
    /// Read multiple attribute
    AT_BLE_READ_MULTIPLE,

    /* Operation flags for writing/modifying attributes */
    /* ************************************************ */
    /// Write attribute
    AT_BLE_WRITE,
    /// Write no response
    AT_BLE_WRITE_NO_RESPONSE,
    /// Write signed
    AT_BLE_WRITE_SIGNED,
    /// Execute write
    AT_BLE_EXEC_WRITE,

    /* Operation flags for registering to peer device   */
    /* events                                           */
    /* ************************************************ */
    /// Register to peer device events
    AT_BLE_REGISTER,
    /// Unregister from peer device events
    AT_BLE_UNREGISTER,

    /* Operation flags for sending events to peer device*/
    /* ************************************************ */
    /// Send an attribute notification
    AT_BLE_NOTIFY,
    /// Send an attribute indication
    AT_BLE_INDICATE,
    /// Send a service changed indication
    AT_BLE_SVC_CHANGED,

    /// Last flag
    AT_BLE_LAST
};

typedef enum
{
    SECONDARY_SERVICE,
    PRIMARY_SERVICE
} at_ble_service_type_t;
/** @brief GAPC Retrieve information command request type
*/
enum gapc_get_info_operation
{
    /** Retrieve name of peer device */
    AT_BLE_GET_PEER_NAME = 2,
    /** Get Peer device appearance */
    AT_BLE_GET_PEER_APPEARANCE = 5,
    /** Get Peer device Slaved Preferred Parameters */
    AT_BLE_GET_PEER_SLV_PREF_PARAMS = 6,
    /** Retrieve Connection Channel MAP */
    AT_BLE_GET_CON_CHANNEL_MAP = 8,
};

/**@ingroup dtm_group
 *@brief Direct test mode Transmitter test Packet Payload Type (DTM)
 */
typedef enum
{
    ///Pseudo-random 9 TX test payload type
    PAYL_PSEUDO_RAND_9            = 0x00,
    ///11110000 TX test payload type
    PAYL_11110000,
    ///10101010 TX test payload type
    PAYL_10101010,
    ///Pseudo-random 15 TX test payload type
    PAYL_PSEUDO_RAND_15,
    ///All 1s TX test payload type
    PAYL_ALL_1,
    ///All 0s TX test payload type
    PAYL_ALL_0,
    ///00001111 TX test payload type
    PAYL_00001111,
    ///01010101 TX test payload type
    PAYL_01010101,
    ///Enumeration end value for TX test payload type value check
    PAYL_END
} at_ble_dtm_payload_type_t;

/**@brief Direct test mode opcodes (DTM)
*/
enum dtm_op_codes
{
    AT_BLE_RESET_CMD_OPCODE = 0x0C03,
    AT_BLE_RX_TEST_CMD_OPCODE = 0x201D,
    AT_BLE_TX_TEST_CMD_OPCODE = 0x201E,
    AT_BLE_TEST_END_CMD_OPCODE = 0x201F
};

/****************************************************************************************
*                                   Structures                                          *
****************************************************************************************/
/**@brief Blue-tooth Low Energy address Type. */
typedef struct
{
    at_ble_addr_type_t type;          /**< See @ref at_ble_addr_type_t */
    uint8_t addr[AT_BLE_ADDR_LEN];    /**< 48-bit address, LSB format. */
} at_ble_addr_t, at_ble_rand_addr_changed_t;

/**@brief Identity Resolving Key. */
typedef struct
{
    uint8_t irk[AT_BLE_MAX_KEY_LEN];   /**< Array containing IRK with length @ref AT_BLE_MAX_KEY_LEN*/
} at_ble_gap_irk_t;

/** @brief Slave preferred connection parameters
*/
typedef struct
{
    /// Slave preferred minimum connection interval
    uint16_t             con_intv_min;
    /// Slave preferred maximum connection interval
    uint16_t             con_intv_max;
    /// Slave preferred Connection latency
    uint16_t             con_latency;
    /// Slave preferred Link supervision time-out
    uint16_t             superv_to;
} at_ble_spcp_t;

typedef struct
{
    /// Device Name write permission requirements for peer device. @see at_ble_att_write_perm_t
    uint8_t b2NamePerm;
    /// Device Appearance write permission requirements for peer device. @see at_ble_att_write_perm_t
    uint8_t b2AppearancePerm;
    /// Slave Preferred Connection Parameters present in GAP attribute database.
    uint8_t b1EnableSpcs;
    /// Service change feature present in GATT attribute database.
    uint8_t b1EnableServiceChanged;
    /// Unused 2 bits; For Future use
    uint8_t b2Rfu;
} at_ble_att_cfg_t;

/// Peer device request to modify local device info such as name or appearance
typedef struct
{
    // Requested information
    //- GAPC_DEV_NAME: Device Name
    // - GAPC_DEV_APPEARANCE: Device Appearance Icon
    uint8_t req;

    /// device information data
    union dev_info
    {
        /// Device name
        struct device_name
        {
            /// name length
            uint16_t length;
            /// name value
            uint8_t value[AT_BLE_MAX_NAME_LEN];
        } dev_name;
        /// Appearance Icon
        uint16_t appearance;
    } info;
} at_ble_dev_info;

/**@brief White-list structure. */
typedef struct
{
    at_ble_addr_t    **ppAddrs;        /**< Pointer to array of device address pointers, pointing to addresses to be used in white-list. NULL if none are given. */
    uint8_t             addrCount;      /**< Count of device addresses in array. */
    at_ble_gap_irk_t     **ppIrks;         /**< Pointer to array of Identity Resolving Key (IRK) pointers, each pointing to an IRK in the white-list. NULL if none are given. */
    uint8_t             irkCount;       /**< Count of IRKs in array. */
} at_ble_gap_whitelist_t;


/** @brief Connection parameters
*/
typedef struct
{
    /// Minimum of connection interval
    uint16_t             con_intv_min;
    /// Maximum of connection interval
    uint16_t             con_intv_max;
    /// Connection latency
    uint16_t             con_latency;
    /// Link supervision time-out
    uint16_t             superv_to;
    /// Minimum CE length
    uint16_t             ce_len_min;
    /// Maximum CE length
    uint16_t             ce_len_max;

} at_ble_connection_params_t;

/** @brief UUID type
*/
typedef struct
{
    at_ble_uuid_type_t type;
    /**<
        either a 16-Bit UUID or a 128-Bit UUID
    */
    uint8_t uuid[AT_BLE_UUID_128_LEN];
    /**<
        If type is 16-Bit, only bytes [0] and [1] are used, otherwise all bytes are used in case of type 128-Bit.\n
        LSB format.
    */
} at_ble_uuid_t;

/** @brief Device configuration
*/
typedef struct
{
    /// Device Role, @ref at_ble_dev_role_t
    at_ble_dev_role_t role;

    // -------------- Privacy Configuration -----------------------
    /// Duration before regenerate device address when privacy is enabled.
    /// Resolution of 10 mSeconds where the minimum value is @ref AT_RENEW_DUR_VAL_MIN
    uint16_t renew_dur;
    /// Device Address,  @ref at_ble_addr_t
    at_ble_addr_t address;
    /// Device IRK used for resolvable random BD address generation (LSB first)
    uint8_t irk[AT_BLE_MAX_KEY_LEN];

    // -------------- ATT Database Configuration -----------------------
    /// Attribute database configuration, @ref at_ble_att_cfg_t
    at_ble_att_cfg_t  att_cfg;
    /// GAP service start handle; use @ref AT_BLE_AUTO_ALLOC_HANDLE for automatic allocation value
    uint16_t gap_start_hdl;
    /// GATT service start handle; use @ref AT_BLE_AUTO_ALLOC_HANDLE for automatic allocation value
    uint16_t gatt_start_hdl;
    /// Maximum MTU, minimum allowed value is @ref AT_MTU_VAL_MIN and maximum is @ref AT_MTU_VAL_MAX
    uint16_t max_mtu;
} at_ble_dev_config_t;

/** @brief Channel Map Structure
*/
typedef struct
{
    // Status
    at_ble_status_t status;
    /// Connection handle
    at_ble_handle_t conn_handle;
    /// 5-byte channel map array
    uint8_t map[AT_BLE_CH_MAP_LEN];
} at_ble_channel_map_t;

/** @brief Characteristic presentation format
*/
typedef struct
{
    at_ble_char_pres_format_t format; /**< Value format */
    int8_t exponent; /**< Value Exponent */
    uint16_t unit; /**<  as defined in GATT spec Part G, Section 3.3.3.5.4 */
    uint8_t name_space; /**<  as defined in GATT spec Part G, Section 3.3.3.5.5 */
    uint16_t description; /**<  as defined in GATT spec Part G, Section 3.3.3.5.6 */

} at_ble_char_presentation_t;

/** @brief Characteristic structure
*/
typedef struct
{
    at_ble_handle_t char_val_handle; /**< Here the stack will store the char. value handle for future use */
    at_ble_uuid_t uuid; /**< Characteristic UUID */
    at_ble_char_properties_t properties; /**< Characteristic properties, values for Client Characteristic Configuration Descriptor and Server Characteristic Configuration Descriptor will be decided from this value*/

    uint8_t *init_value; /**< initial value of this characteristic  */
    uint16_t value_init_len; /**< initial value length */
    uint16_t value_max_len; /**< maximum possible length of the char. value */
    at_ble_attr_permissions_t value_permissions; /**< Value permissions */ //TODO: can this value be deduced from properties field ?

    uint8_t *user_desc; /**< a user friendly description, this value will be stored in the relevant descriptor, if no user description is desired set to NULL */
    uint16_t user_desc_len; /**< the user friendly description length, this value will be stored in the relevant descriptor, if no user description is desired set to 0*/
    uint16_t user_desc_max_len; /**< Maximum possible length for the user friendly description, this value will be stored in the relevant descriptor, if no user description is desired set to 0 */

    at_ble_char_presentation_t *presentation_format; /**< Characteristic presentation format, this value will be stored in the relevant descriptor, if no presentation format is necessary set to NULL */
    at_ble_attr_permissions_t user_desc_permissions;
    at_ble_attr_permissions_t client_config_permissions;
    at_ble_attr_permissions_t server_config_permissions;
    at_ble_handle_t user_desc_handle;
    at_ble_handle_t client_config_handle;
    at_ble_handle_t server_config_handle;


} at_ble_characteristic_t;

typedef struct
{

    uint16_t desc_val_length; /**<  descriptor value length */
    at_ble_attr_permissions_t perm; /**<  descriptor permissions */
    at_ble_uuid_t uuid; /**<  descriptor uuid */
    at_ble_handle_t handle; /**< Here the stack will store the handle for future use */
} at_ble_generic_att_desc_t;

typedef struct
{
    at_ble_attr_permissions_t perm; /**<  descriptor permissions */
    at_ble_handle_t handle; /**< Here the stack will store the handle for future use */
} at_ble_server_config_desc_t;


typedef struct
{
    uint8_t *user_description;  /**< a user friendly description, this value will be stored in the relevant descriptor, if no user description is desired set to NULL */
    uint16_t len; /**< the user friendly description length, this value will be stored in the relevant descriptor, if no user description is desired set to 0*/
    at_ble_handle_t handle; /**< user descriptor handle */
    at_ble_attr_permissions_t permissions; /**< user descriptor permissions */
} at_ble_user_desc_t;

typedef struct
{
    at_ble_handle_t handle; /**< Here the stack will store the char. value handle for future use */
    at_ble_uuid_t uuid; /**< Characteristic UUID */
    at_ble_char_properties_t properties; /**< Characteristic properties, values for Client Characteristic Configuration Descriptor and Server Characteristic Configuration Descriptor will be decided from this value*/

    uint8_t *init_value;  /**< initial value of this characteristic  */
    uint16_t len; /**<  value length */
    at_ble_attr_permissions_t permissions; /**< Value permissions */ //TODO: can this value be deduced from properties field ?

} at_ble_char_val_t;


typedef struct
{
    at_ble_char_val_t char_val; /**< characteristics value related info */
    at_ble_user_desc_t user_desc; /**< user descriptor related info */
    at_ble_char_presentation_t   *presentation_format; /**< Characteristic presentation format, this value will be stored in the relevant descriptor, if no presentation format is necessary set to NULL */
    at_ble_server_config_desc_t  client_config_desc; /**< client config descriptor related info */
    at_ble_server_config_desc_t  server_config_desc; /**< server config descriptor related info */
    at_ble_generic_att_desc_t    *additional_desc_list; /**< generic descriptor related info */
    uint16_t additional_desc_count; /**< count of generic descriptors*/
} at_ble_chr_t;
/** @brief Encapsulation for a characteristic and its length
*/
typedef struct
{
    ///Characteristic handle
    uint16_t att_handle;
    /** Characteristic length : must be the real length of the Characteristic
     *  every requested handle shall be delivered as one indication @ref at_ble_characteristic_read_response_t
     * if Length of handle 1 is larger than the real length, the handle value will contain his length + length from the other requested handles
     */
    uint16_t att_len;
} at_ble_char_list;

/** @brief Service Definition
*/
typedef struct
{
    at_ble_uuid_t uuid; /**< UUID of the included Service, only 16 bit UUID are allowed */
    at_ble_handle_t service_handle; /**< The included service handle */
    at_ble_handle_t end_group_handle; /**< End group handle */

} at_ble_included_service_t;


typedef struct
{
    at_ble_handle_t service_handle; /**< The included service handle */
} at_ble_inc_srv_t;

typedef struct
{
    at_ble_service_type_t type; /**< Service type (PRIMARY or SECONDARY )*/
    at_ble_attr_permissions_t perm; /**< Service permissions*/
    at_ble_handle_t handle;  /**< Here the stack will store the handle for future use */
    at_ble_uuid_t uuid; /**< UUID of the  Service*/
    at_ble_inc_srv_t *inc_list;  /**< Included  Services list*/
    uint16_t included_count;/**< Included  Services count*/
    at_ble_chr_t *char_list; /**< Characteristics list*/
    uint16_t char_count;/**< Characteristics count*/
} at_ble_service_t;

/** @brief Pairing Features
*/
typedef struct
{
    at_ble_iocab_t io_cababilities;     /**< IO capabilities of this device, value of @ref at_ble_iocab_t */
    bool mitm_protection;               /**< If true then Man-In-The-Middle protection must be provided */
    bool bond;                          /**< If True then bonding is requested */
    bool oob_avaiable;                  /**< If true then out-of-band data is available */
    //Deprecated variable
    uint8_t min_key_size;               /**< Minimum key size accepted */
    uint8_t max_key_size;               /**< Maximum key size accepted */
    at_ble_key_dis_t initiator_keys;    /**< Security keys offered to peer device, value of @ref at_ble_key_dis_t */
    at_ble_key_dis_t responder_keys;    /**< Security keys needed from peer device, value of @ref at_ble_key_dis_t*/
    at_ble_auth_levels_t desired_auth;  /**< The desired authentication level to be reached, value of @ref at_ble_auth_levels_t */
} at_ble_pair_features_t;

/** @brief Long Term Key
*/
typedef struct
{
    /// Long Term Key array of @ref AT_BLE_MAX_KEY_LEN
    uint8_t     key[AT_BLE_MAX_KEY_LEN];
    /// Encryption Diversifier
    uint16_t    ediv;
    /// Random Number (64 bit)
    uint8_t     nb[8];
    /// Encryption key size (7 to 16)
    uint8_t     key_size;
} at_ble_LTK_t;

/** @brief Connection Signature Resolution Key
*/
typedef struct
{
    ///Array of @ref AT_BLE_MAX_KEY_LEN for CSRKey
    uint8_t key[AT_BLE_MAX_KEY_LEN];
} at_ble_CSRK_t;

/** @brief IRKey
*/
typedef struct
{
    /// Identity Resolving Key array of @ref AT_BLE_MAX_KEY_LEN
    uint8_t key[AT_BLE_MAX_KEY_LEN];
    /// Device BD Address
    at_ble_addr_t addr;
} at_ble_IRK_t;

/** @brief signature counter
*/
typedef struct
{
    ///Connection Handle
    at_ble_handle_t stConnHandle;
    ///Local signature counter value
    uint32_t        u32LocalCounter;
    ///Remote signature counter value
    uint32_t        u32PeerCounter;
} at_ble_sign_counter_t;

/** @brief Connection Signature information
*/
typedef struct
{
    ///Structure of local and peer signature counters and their handle, @ref at_ble_sign_counter_t
    at_ble_sign_counter_t   stSignCounter;
    ///Remote CSRK value
    at_ble_CSRK_t           stPeerCsrk;
    ///Local CSRK value
    at_ble_CSRK_t           stLocalCsrk;
} at_ble_signature_info_t;

/** @brief Scan Information
*/
typedef struct
{
    ///Type of Advertising
    at_ble_adv_type_t type;
    ///Advertiser(Broadcaster) address
    at_ble_addr_t dev_addr;
    ///Advertising data; This array will be zeros in case of adv_data_len < @ref AT_BLE_ADV_MAX_SIZE
    uint8_t adv_data[AT_BLE_ADV_MAX_SIZE];
    ///Advertising data length
    uint8_t adv_data_len;
    ///Signed variable holds RSSI value of Advertiser(Broadcaster)
    int8_t  rssi;
} at_ble_scan_info_t;

/** @brief Scan report status
*/
typedef struct
{
    ///Status by the end of scan operation, refer to @ref at_ble_status_t
    at_ble_status_t status;
} at_ble_scan_report_t;

/** @brief Advertising report status
*/
typedef struct
{
    ///Status of advertising operation, refer to @ref at_ble_status_t
    at_ble_status_t status;
} at_ble_adv_report_t;

/**@brief AT_BLE_CONNECTED message format. This message is sent to the APP when the link request is complete.
*/
typedef struct
{
    ///Peer address
    at_ble_addr_t peer_addr;
    ///connection handle
    at_ble_handle_t handle;
    ///connection status, refer to @ref at_ble_status_t
    at_ble_status_t conn_status;
	///Structure to save slave connection parameters
	struct  
	{
		/// Connection interval
		uint16_t con_interval;
		/// Connection latency
		uint16_t con_latency;
		/// Link supervision timeout
		uint16_t sup_to;
	}conn_params;
} at_ble_connected_t;

/**@brief Handle and Status of disconnected peer
 */
typedef struct
{
    ///connection handle
    at_ble_handle_t handle;
    ///disconnection reason, refer to @ref at_ble_disconnect_reason_t
    uint8_t reason;
} at_ble_disconnected_t;

typedef struct
{
    at_ble_handle_t handle;
    at_ble_status_t status;
    uint16_t con_intv;
    uint16_t con_latency;
    uint16_t superv_to;

} at_ble_conn_param_update_done_t;

typedef struct
{
    at_ble_handle_t handle;
    uint16_t con_intv_min;
    uint16_t con_intv_max;
    uint16_t con_latency;
    uint16_t superv_to;
} at_ble_conn_param_update_request_t;

typedef struct
{
    at_ble_handle_t handle;
    uint16_t rx_value;

} at_ble_rx_power_value_t;

/** @brief Pairing done message
*/
typedef struct
{
    at_ble_auth_t auth;
    at_ble_handle_t handle;
    at_ble_status_t status;
    at_ble_LTK_t peer_ltk;
    at_ble_CSRK_t peer_csrk;
    at_ble_IRK_t peer_irk;

} at_ble_pair_done_t;

typedef struct
{
    at_ble_handle_t handle;
    at_ble_auth_t peer_features;
} at_ble_pair_request_t;

typedef struct
{
    at_ble_handle_t handle;
    at_ble_pair_key_type_t type;
    at_ble_passkey_type_t passkey_type;

} at_ble_pair_key_request_t;

typedef struct
{
    at_ble_handle_t handle;
    at_ble_status_t status;
    bool bond;
    bool mitm_protection;

} at_ble_slave_sec_request_t;

typedef struct
{
    at_ble_handle_t handle;
    /// Encryption Diversifier
    uint16_t ediv;
    /// Random Number (64 bit)
    uint8_t     nb[8];

} at_ble_encryption_request_t;

typedef struct
{
    at_ble_handle_t handle;
    at_ble_status_t status;
    at_ble_auth_t   authen;
} at_ble_encryption_status_changed_t;

typedef struct
{
    at_ble_handle_t handle;
    uint8_t         operation;
    at_ble_status_t status;

} at_ble_gapc_cmp_evt_t;

typedef struct
{
    at_ble_status_t status;
    /// IRK that correctly solved the random address
    uint8_t irk[AT_BLE_MAX_KEY_LEN];
    ///returned address
    uint8_t resolved_addr[AT_BLE_ADDR_LEN];
} at_ble_resolv_rand_addr_status_t;


typedef struct
{
    at_ble_handle_t conn_handle;
    /// refer to @ref at_ble_operation
    uint8_t operation;
    /// Status of the request
    at_ble_status_t status;
} at_ble_cmd_complete_event_t;

typedef at_ble_cmd_complete_event_t at_ble_discovery_complete_t  ;

typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t start_handle;
    at_ble_handle_t end_handle;
    at_ble_uuid_t service_uuid;

} at_ble_primary_service_found_t;

typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t attr_handle;
    at_ble_handle_t start_handle;
    at_ble_handle_t end_handle;
    at_ble_uuid_t service_uuid;

} at_ble_included_service_found_t;

typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t char_handle;
    at_ble_handle_t value_handle;
    uint8_t properties;
    at_ble_uuid_t char_uuid;

} at_ble_characteristic_found_t;

typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t desc_handle;
    at_ble_uuid_t desc_uuid;

} at_ble_descriptor_found_t;


typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t char_handle;
    uint16_t        char_len;
    uint16_t        char_offset;
    uint8_t         char_value[AT_BLE_MAX_ATT_LEN];
    at_ble_status_t status;
    /// refer to @ref at_ble_operation
    uint8_t operation;
} at_ble_characteristic_read_response_t;

typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t char_handle;
} at_ble_characteristic_read_req_t;

/**@brief Structure received when write to a characteristic is required.
*/
typedef struct
{
	///Connection handle
    at_ble_handle_t conn_handle;
	///Characteristic handle
    at_ble_handle_t char_handle;
	///Data offset
    uint16_t offset;
	///Data length
    uint16_t length;
	///Data with maximum length @ref AT_BLE_MAX_ATT_LEN
    uint8_t value[AT_BLE_MAX_ATT_LEN];
} at_ble_characteristic_write_request_t;

typedef struct
{
	///Characteristic handle
    at_ble_handle_t char_handle;
} at_ble_att_info_req_t;

typedef struct
{
	///Connection handle
    at_ble_handle_t conn_handle;
	///Characteristic handle
    at_ble_handle_t char_handle;
	///Status of write operation, refer to @ref at_ble_status_t
    at_ble_status_t status;
} at_ble_characteristic_write_response_t;

typedef struct
{
	///Connection handle
    at_ble_handle_t conn_handle;
    /// length of packet to send
    uint8_t         char_len;
    /// characteristic handle
    at_ble_handle_t char_handle;
    /// data value
    uint8_t         char_value[AT_BLE_MAX_ATT_LEN];
} at_ble_notification_recieved_t;

typedef struct
{
	///Connection handle
    at_ble_handle_t conn_handle;
    /// length of packet to send
    uint8_t         char_len;
    /// characteristic handle
    at_ble_handle_t char_handle;
    /// data value with maximum length @ref AT_BLE_MAX_ATT_LEN
    uint8_t         char_value[AT_BLE_MAX_ATT_LEN];
} at_ble_indication_recieved_t;

typedef struct
{
	///Connection handle
    at_ble_handle_t conn_handle;
	///Characteristic handle
    at_ble_handle_t char_handle;
	///Status of indicating operation, refer to @ref at_ble_status_t
    at_ble_status_t status;
} at_ble_indication_confirmed_t;

typedef struct
{
    uint8_t conn_handle;
    at_ble_handle_t char_handle;
    uint16_t char_offset;
    uint16_t char_len;
    uint8_t char_new_value[AT_BLE_MAX_ATT_LEN];
    at_ble_status_t status;
} at_ble_characteristic_changed_t;

typedef struct
{
    at_ble_handle_t conn_handle;
    uint16_t        cfg;
} at_ble_characteristic_configuration_changed_t;

typedef struct
{
    at_ble_handle_t conn_handle;
    at_ble_handle_t char_handle;
    at_ble_status_t status;

} at_ble_read_authorize_request_t;


/// Parameters of the @ref AT_BLE_LECB_CONN_REQ message
typedef struct
{
    /// LE Protocol/Service Multiplexer
    uint16_t le_psm;
    /// Destination Credit for the LE Credit Based Connection
    uint16_t dest_credit;
    /// Maximum SDU size
    uint16_t max_sdu;
    /// Destination CID
    uint16_t dest_cid;
} at_ble_lecb_conn_req_t;


/// Parameters of the @ref AT_BLE_LECB_CONNECTED message
typedef struct
{
    /// LE Protocol/Service Multiplexer
    uint16_t le_psm;
    /// Destination Credit for the LE Credit Based Connection
    uint16_t dest_credit;
    /// Maximum SDU size
    uint16_t max_sdu;
    /// Destination CID
    uint16_t dest_cid;
    /// Status
    uint8_t status;
} at_ble_lecb_connected_t;

/// Parameters of the @ref AT_BLE_LECB_DISCONNECTED message
typedef struct
{
    /// LE Protocol/Service Multiplexer
    uint16_t le_psm;
    /// Reason
    uint16_t reason;
    /// Status
    uint8_t status;
} at_ble_lecb_disconnected_t;


/// Parameters of the @ref AT_BLE_LECB_ADD_CREDIT_IND message
typedef struct
{
    /// LE Protocol/Service Multiplexer
    uint16_t le_psm;
    /// Source Credit for the LE Credit Based Connection
    uint16_t src_credit;
    /// Destination Credit for the LE Credit Based Connection
    uint16_t dest_credit;
} at_ble_lecb_add_credit_ind_t;

/// Parameters of the @ref AT_BLE_LECB_SEND_RESP message
typedef struct
{
    /// Status of request.
    uint8_t status;
    /// Destination channel ID
    uint16_t dest_cid;
    /// Destination credit
    uint16_t dest_credit;
} at_ble_lecb_send_rsp_t;

/// Parameters of the @ref AT_BLE_LECB_DATA_RECIEVED message
typedef struct
{
    /// Source channel ID
    uint16_t src_cid;
    /// Source remaining credit
    uint16_t src_credit;
    /// Data length
    uint16_t len;
    /// data
    uint8_t data[AT_BLE_LECB_MAX_PKT_SIZE];
} at_ble_lecb_data_recv_t;


/// Message structure used to inform APP that an error has occurred in the profile server role task
typedef struct
{
    /// Connection Handle
    uint16_t conhdl;
    /// Message ID
    uint16_t msg_id;
    /// Status
    uint8_t status;
} at_ble_prf_server_error_ind_t;
/// Time profile information
typedef struct
{
    /// year time element
    uint16_t year;
    /// month time element
    uint8_t month;
    /// day time element
    uint8_t day;
    /// hour time element
    uint8_t hour;
    /// minute time element
    uint8_t min;
    /// second time element
    uint8_t sec;
} at_ble_prf_date_time_t;

/// Parameters of the Health thermometer service database
typedef struct
{
    /// Health thermometer Feature (@see enum htpt_features)
    uint8_t features;
    /// Temperature Type Value
    uint8_t temp_type;
    /// Measurement Interval Valid Range - Minimal Value
    uint16_t valid_range_min;
    /// Measurement Interval Valid Range - Maximal Value
    uint16_t valid_range_max;
    /// Measurement interval (latest known interval range)
    uint16_t meas_intv;
} at_ble_htpt_db_cfg;

/** @brief Parameters of the @ref AT_BLE_HTPT_CREATE_DB_CFM message
*/
typedef struct
{
    ///Status
    uint8_t status;
    //Service handle
    at_ble_handle_t start_handle;
} at_ble_htpt_create_db_cfm_t;

/** @brief Parameters of the @ref AT_BLE_HTPT_ENABLE_RSP message
*/
typedef struct
{
    ///Connection handle
    uint8_t conhdl;
    ///Status
    at_ble_status_t status;
} at_ble_htpt_enable_rsp_t;

/** @brief Parameters of the @ref AT_BLE_HTPT_DISABLE_IND message
*/
typedef struct
{
    ///Connection handle
    uint16_t conhdl;

    ///Temperature measurement indication configuration
    uint16_t temp_meas_ind_en;
    ///Intermediate temperature notification configuration
    uint16_t interm_temp_ntf_en;
    ///Measurement interval indication configuration
    uint16_t meas_intv_ind_en;
    ///Measurement interval
    uint16_t meas_intv;
} at_ble_htpt_disable_ind_t;

/** @brief Parameters of the @ref AT_BLE_HTPT_TEMP_SEND_CFM message
*/
typedef struct
{
    ///Status
    at_ble_status_t status;
} at_ble_htpt_temp_send_cfm_t;
/** @brief Parameters of the @ref AT_BLE_HTPT_MEAS_INTV_UPD_RSP message
*/
typedef struct
{
    ///Status
    at_ble_status_t status;
} at_ble_htpt_meas_intv_upd_rsp_t;

/** @brief Parameters of the @ref AT_BLE_HTPT_MEAS_INTV_CHG_REQ message
*/
typedef struct
{
    ///Measurement interval
    uint16_t intv;
} at_ble_htpt_meas_intv_chg_req_t;

/** @brief Parameters of the @ref AT_BLE_HTPT_MEAS_INTV_CHG_IND message
*/
typedef struct
{
    uint16_t intv;
} at_ble_htpt_meas_intv_chg_ind_t;

/** @brief Parameters of the @ref AT_BLE_HTPT_CFG_INDNTF_IND message
*/
typedef struct
{
    ///Connection handle
    uint8_t conhdl;
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint8_t ntf_ind_cfg;
} at_ble_htpt_cfg_indntf_ind_t;

/** @brief MTU changed value
*/
typedef struct
{
    ///Connection handle
    uint16_t conhdl;
    // new MTU value sent form peer
    uint16_t mtu_value;

} at_ble_mtu_changed_ind_t;

/** @brief  Retrieve information  command Structure
*/
typedef struct
{
    /// needed operation, @ref gapc_get_info_operation
    uint8_t operation;
} at_ble_gapc_get_info_cmd_t;

/** @brief device name
*/
typedef struct
{
    uint16_t  length;
    uint8_t   value[AT_BLE_MAX_NAME_LEN];
} at_ble_gap_dev_name_t;


/** @brief Retrieve information Structure
*/
typedef union
{
    at_ble_gap_dev_name_t name;
    uint16_t              appearance;
    at_ble_spcp_t         slv_params;
} at_ble_gapc_dev_info_val_t;

/** @brief peer attribute info structure
 *  GAPC_GET_DEV_INFO_REQ_IND
*/
typedef struct
{
    /* Connection handle */
    at_ble_handle_t conn_handle;

    /** uint8_t req types refer to @ref gapc_get_info_operation */
    uint8_t  req;
    /** attribute handle*/
    uint16_t att_handle;
    /* Device information data  */
    at_ble_gapc_dev_info_val_t info;
} at_ble_peer_att_info_ind_t;

/** @brief Device info DB
 *  @ref at_ble_set_gap_deviceinfo
*/
typedef struct
{
    /// Device name
    at_ble_gap_dev_name_t       name;
    /// Appearance configuration @ref at_ble_att_write_perm_t
    uint16_t                    appearance;
    /// Slave preferred connection parameters
    at_ble_spcp_t               slv_params;
    /// Device name write permission for peer device @ref at_ble_att_write_perm_t
    at_ble_att_write_perm_t     dev_name_perm;
} at_ble_gap_deviceinfo_t;

/**@ingroup dtm_group
* @brief data type of DMT status and report, "number_of_packets" will be used only in case of receiving
 *         the event @ref AT_BLE_LE_PACKET_REPORT after calling the API
 *
 *  @sa @ref at_ble_dtm_stop_test
*/
typedef struct
{
    /** op-code for the current operation refer to @ref dtm_op_codes */
    uint16_t op_code;
    /** status of the current command (events @ref AT_BLE_LE_PACKET_REPORT and @ref AT_BLE_LE_TEST_STATUS ) */
    uint8_t  status;
    /** Number of packets received (Read this variable only after calling @ref at_ble_dtm_stop_test api  with Event @ref AT_BLE_LE_PACKET_REPORT) */
    uint16_t number_of_packets;
} at_ble_dtm_t;
/****************************************************************************************
*         Documentation File Modules Structures Groups                                       *
****************************************************************************************/
// GAP APIs
// -> Device Configuration APIs
// -> Address Management APIs
// -> Advertising Procedure APIs
// -> Scanning Procedure APIs
// -> Connecting Procedure APIs
// -> Security APIs
// -> Misc APIs
/**
 * @defgroup gap_group GAP APIs
* @brief This group includes all GAP APIs; Device Configuration, Address Management, Connections, ...
* @{
*/
/**
 *  @defgroup gap_dev_config_group Device Configuration APIs
 *  @brief This group includes GAP device configurations APIs
 */
/**
*  @defgroup gap_addr_mgmt_group Address Management APIs
*  @brief This group includes GAP address APIs
*/
/**
*  @defgroup gap_adv_group Advertising Procedure APIs
*  @brief This group includes advertising APIs
*/
/**
*  @defgroup gap_scan_group Scanning Procedure APIs
* @brief This group includes scanning APIs
*/
/**
*  @defgroup gap_conn_group Connecting Procedure APIs
*  @brief This group includes connections APIs
*/
/**
*  @defgroup gap_sec_group Security APIs
*  @brief This group includes security APIs
*/
/**
*  @defgroup gap_misc_group Misc APIs
*  @brief This group includes other GAP related APIs
*/
/**
*  @defgroup gap_whitelist_group White-List APIs
*  @brief This group includes GAP White-List related APIs
*/
/** @}*/


// GAT Client APIs
/**
* @defgroup gatt_client_group GATT Client APIs
* @brief This group includes all GATT Client APIs; Discover, Read, Write, ...
* @{
*/
/** @}*/


// GAT Server APIs
/**
* @defgroup gatt_server_group GATT Server APIs
* @brief This group includes all GATT Server APIs; Set value, Get value, Notify, Indicate, ...
* @{
*/
/** @}*/


// L2CAP APIs
/**
* @defgroup l2cap_group L2CAP APIs
* @brief This group includes all L2CAP layer APIs
* @{
*/
/** @}*/


// HTPT APIs
/**
* @defgroup htpt_group HTPT APIs
* @brief This group includes all HTPT APIs; Database Creation, Send Temperature, ...
* @{
*/
/** @}*/


// MISC APIs
/**
* @defgroup misc_group Misc APIs
* @brief This group includes APIs which are not belong to any task
* @{
*/
/** @}*/


// Error Codes
/**
* @defgroup error_codes_group Error codes
* @brief This group includes all error codes which was handled by Atmel APIs through different levels.
* @{
*/
/** @}*/


/**
* @defgroup dtm_group Direct test Mode APIs (DTM)
* @brief This group includes all Direct test mode APIs.
* @{
*/
/** @}*/

/****************************************************************************************
*                                       Functions                                                                           *
****************************************************************************************/

/** @ingroup gap_dev_config_group
  * @brief Initialize bus interface, reset the link layer and the host ,
  * Initialize memory required for storing data base related data
  *
  * @param[in] args Pointer to specific data depends on API version
  * - 4.1: Pointer to structure for interface initialization using at_ble_init_config_t structure
  *
  * @warning This function must be the first to be called.
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_init(at_ble_init_config_t *args);

/** @ingroup gap_dev_config_group
  * @brief Reset the link layer.
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_ll_reset(void);

/** @ingroup gap_dev_config_group
  * @brief Set GAP attribute data base (Appearance , slave preferred connection parameters ,
  * device name write permissions)
  *
  * @param[in] gap_deviceinfo GAP device information, @see at_ble_gap_deviceinfo_t
  *
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
  * Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_gap_deviceinfo(at_ble_gap_deviceinfo_t  *gap_deviceinfo);

/** @ingroup gap_dev_config_group
  * @brief Retrieve information about peer device or about the current active link
  * expected events are @ref AT_BLE_PEER_ATT_INFO_IND @ref AT_BLE_CON_CHANNEL_MAP_IND
  *
  * @param[in] operation  requested operation , @see at_ble_gapc_get_info_cmd_t
  * @param[in] conn_handle Handle to the peer device
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
  * Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_gap_get_peer_deviceinfo(uint16_t conn_handle, at_ble_gapc_get_info_cmd_t *operation);

/** @ingroup gap_dev_config_group
  * @brief Set attribute permissions configuration (Device Appearance permissions, slave preferred connection parameters,
  * Device Name permissions, Service Change feature)
  *
  * @param[in] dev_att_cfg pointer to attributes configuration
  *
  * @sa at_ble_att_cfg_t
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
  * Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_att_config(at_ble_att_cfg_t  *dev_att_cfg);

/** @ingroup gap_dev_config_group
 * @brief Set device name
 *
 * @param[in] dev_name the new device name
 * @param[in] len name length
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_device_name_set(uint8_t *dev_name, uint8_t len);

/** @ingroup gap_dev_config_group
  * @brief Set device Identity Resolving Key
  *
  * @param[in] irk device IRK used for resolvable random BD address generation
  * @param[in] interval duration before regenerate device address in 10 ms units
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
  * Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_privacy_key(at_ble_gap_irk_t *irk , uint16_t interval);

/** @ingroup gap_addr_mgmt_group
 * @brief Set device BT Address
 *
 * @param[in] address the new BT address
 *
 * @pre In case of using resolvable random address, @ref at_ble_set_dev_config  is used to set device IRK key otherwise default randomness IRK key values will be used.
 *
 * @note
 *     - If address type is @ref AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE or @ref AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE, the
 * given address is ignored and address will be auto generated.
 *     - If address type is random , then @ref AT_BLE_RAND_ADDR_CHANGED event is triggered after first air operation
 * either advertising, connecting, or scanning to inform application about the used random address.
 *     - The default duration before regenerating device address is @ref AT_RENEW_DUR_VAL_MIN, to change renew address duration use @ref at_ble_set_dev_config.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_addr_set(at_ble_addr_t *address);

/** @ingroup gap_addr_mgmt_group
 *@brief Get device BT Address
 *
 * @param[out] address pointer to save returned BT Address
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_addr_get(at_ble_addr_t *address);

/** @ingroup misc_group
 * @brief extracts the next event form the event queue
 *
 * @param[out] event The received event
 * @param[out] params Received event data
 * @param[in] timeout Time in millisecond this function will block waiting of an event (0xFFFFFFFF to block indefinitely)
 * in this case, if the user wishes to unblock the function @ref at_ble_event_user_defined_post can be used
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 *
 * @note BTLC uses timer resolution 1 ms, while SAMB uses it with 10 ms resolution.
 *
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_event_get(at_ble_events_t *event, void *params,
                                 uint32_t timeout);

/** @ingroup misc_group
 * @brief Posts a user defined custom event into the event queue, allowing the user code
 *        to process other events in the same event handling loop. Events will be received with the
 *        as @ref AT_BLE_CUSTOM_EVENT
 *
 * @param[in] params Parameters sent to the custom event
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_event_user_defined_post(void *params);

/** @ingroup gap_whitelist_group
 *
 * @brief Adds a peer device address to the white-list
 *
 * @param[in] address       peer device address
 *
 * @note This command can be used at any time except when:
 *       the advertising filter policy uses the white list and advertising is enabled.
 *       the scanning filter policy uses the white list and scanning is enabled.
 *       the initiator filter policy uses the white list and a create connection command
 *         is outstanding.
 *
 * @note The only addresses that should be added to the white-list is Public device address or random static device address
 *
 * @note In case of connection mode is AUTO, the white-list will be cleared
 *
 * @note To get scan results from only peers added to white list,
 *          make sure that \"filter_whitelist\" argument of @ref at_ble_scan_start is true.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_whitelist_add(at_ble_addr_t *address);

/** @ingroup gap_whitelist_group
 *
 * @brief Removes a peer device address from the white-list
 *
 * @param[in] address       peer device address
 *
 * @note This command can be used at any time except when:
 *       the advertising filter policy uses the white list and advertising is enabled.
 *       the scanning filter policy uses the white list and scanning is enabled.
 *       the initiator filter policy uses the white list and a create connection command
 *         is outstanding.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_whitelist_remove(at_ble_addr_t *address);

/** @ingroup gap_whitelist_group
 *
 * @brief Clear the white-list
 *
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_whitelist_clear(void);

/** @ingroup gap_whitelist_group
 *
 * @brief Get total number of white list entries that can be stored in the Controller.
 *
 * @param[out] size       The white-list size
 *
 * @note the returned size is the maximum size of Public address white list only, Randome static address white list has the same size too
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_get_whitelist_size(uint8_t *size);

/** @ingroup gap_adv_group
 *@brief Set, clear or update advertisement and scan response data.
 *
 *
 * @note
 * - To clear the advertisement data and set it to a 0-length packet, simply provide a valid pointer (pAdvData/pSrData) with its corresponding
 *        length (dlen/srdlen) set to 0.
 * - The call will fail if pAdvData and pSrData are both NULL since this would have no effect.
 *
 * @param[in] adv_data    Raw data to be placed in advertisement packet. If NULL, no changes are made to the current advertisement packet data.
 * @param[in] adv_data_len      Data length for adv_data. Max size 28 bytes, 3 bytes are reserved to set advertising AD type flags , shall not be set in advertising data.
 * @param[in] scan_resp_data Raw data to be placed in scan response packet. If NULL, no changes are made to the current scan response packet data.
 * @param[in] scan_response_data_len    Data length for scan_resp_data. Max size 31 bytes.
 *
 * @note
 * The First three bytes in advertising data have been reserved in order to Fill Flags(0x01) AD type. This data is set
 * according to advertising mode selected.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_adv_data_set(uint8_t const *const  adv_data,
                                    uint8_t  adv_data_len,
                                    uint8_t const *const  scan_resp_data,
                                    uint8_t  scan_response_data_len
                                   );


/** @ingroup gap_adv_group
 *@brief Start advertising (GAP Discoverable, Connectable modes, Broadcast Procedure).
 *
 * @param[in] type Advertising type, see @ref at_ble_adv_type_t for more info
 * @param[in] mode either non-discoverable,General, Limited or Broadcaster , @ref at_ble_adv_mode_t for more details
 * @param[in] peer_addr For @ref AT_BLE_ADV_TYPE_DIRECTED mode only, known peer address
 * @param[in] filtered policy @ref at_ble_filter_type_t for more details
 * @param[in] interval Advertising interval between @ref AT_BLE_ADV_INTERVAL_MIN and @ref AT_BLE_ADV_INTERVAL_MAX in 0.625 ms units (20ms to 10.24s). This parameter must be set to 0 if type equals @ref AT_BLE_ADV_TYPE_DIRECTED
 * @param[in] timeout Advertising time-out between 0x0001 and 0x028F in scale of Seconds, 0x0000 disables time-out. This parameter must be set to 0 if type equals @ref AT_BLE_ADV_TYPE_DIRECTED.\n
 *            Timeout only valid with @ref AT_BLE_ADV_GEN_DISCOVERABLE and @ref AT_BLE_ADV_LIM_DISCOVERABLE.
 * @param[in] disable_randomness if True the device will advertise at the exact interval without adding the random 10msec
 *
 * @pre If it is required to change BD address, at_ble_addr_set should be used otherwise default address will be used
 *
 * @note AT_BLE_ADV_REPORT will be received with error code in case of stack can not start advertising; refer to @ref at_ble_status_t
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_adv_start(at_ble_adv_type_t type, at_ble_adv_mode_t mode ,
                                 at_ble_addr_t *peer_addr, at_ble_filter_type_t filtered,
                                 uint16_t interval, uint16_t timeout, bool disable_randomness);

/** @ingroup gap_adv_group
 *@brief Set advertising channel using one of @ref at_ble_adv_channel_map_t.
 *
 * @note Default value is @ref AT_BLE_ADV_ALL_CHNLS_EN
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_adv_channel_Map(at_ble_adv_channel_map_t ch);

/** @ingroup gap_adv_group
 *@brief Stop advertising (GAP Discoverable, Connectable modes, Broadcast Procedure).
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_adv_stop(void);

/** @ingroup gap_scan_group
 *@brief Start scan operation
 *
 * While the scan operation is ongoing, the application will always receive @ref AT_BLE_SCAN_INFO
 * event per found device. \n
 * In case of using @ref AT_BLE_SCAN_GEN_DISCOVERY or @ref AT_BLE_SCAN_LIM_DISCOVERY,
 * also @ref AT_BLE_SCAN_REPORT event will be received at the end of scan process or error occurred when trying to start scanning procedure
 *
 * @param[in] interval Scan interval in 625us units, a value between @ref AT_BLE_ADV_INTERVAL_MIN and @ref AT_BLE_ADV_INTERVAL_MAX
 * @param[in] window   Scan window in 625us units, value between @ref AT_BLE_SCAN_WINDOW_MIN and @ref AT_BLE_SCAN_INTERVAL_MAX
 * @param[in] timeout Scan time-out, between 0x0001 and 0x028F in scale of Seconds, 0x0000 disables time-out.
 * @param[in] type  Controls the type of scan to perform either Passive or Active @ref at_ble_scan_type_t for more details.
 * @param[in] mode     Either General, Limited or Observer only, @ref at_ble_scan_mode_t for more details
 * @param[in] filter_whitelist     If true, get scan results only from white-listed devices added by @ref at_ble_whitelist_add
 *                                 otherwise scan results will be got from any advertising device.
 *                                 This filter should not be used with @ref AT_BLE_ADV_GEN_DISCOVERABLE and @ref AT_BLE_ADV_LIM_DISCOVERABLE modes ONLY.
 * @param[in] filter_dublicates   If true, scan event will be generated only once per device, if false multiple events will be issued
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_scan_start(uint16_t interval, uint16_t window,
                                  uint16_t timeout, at_ble_scan_type_t type , at_ble_scan_mode_t mode,
                                  bool filter_whitelist, bool filter_dublicates);

/** @ingroup gap_scan_group
 *@brief Stops an ongoing scan operation
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_scan_stop(void);

/** @ingroup gap_conn_group
 *@brief Set connection mode parameter
 *
 * @param[in] mode selected mode, one of @ref at_ble_conn_mode_t
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_conn_mode(at_ble_conn_mode_t mode);

/** @ingroup gap_conn_group
 *@brief Connect to a peer device
 *
 * Connecting to a peer device, implicitly starting the necessary scan operation then
 * connecting if a device in the peers list is found in case of using default mode @ref AT_BLE_CONNECTION_AUTO.
 * In case of using @ref AT_BLE_CONNECTION_DIRECT mode, device will try to connect directly to provided peer address only without scanning.
 * This API returns immediately after the operation is programmed, actual connection establishment or failure is marked with
 * @ref AT_BLE_CONNECTED event
 *
 * @param[in] peers List of peers' addresses that the device will connect to one of them
 * @param[in] peer_count Number peers trying to connect with. In case of using @ref AT_BLE_CONNECTION_DIRECT mode, peer_count should be 1.
 * @param[in] scan_interval  Scan interval in 625us units
 * @param[in] scan_window   Scan window in 625us units
 * @param[in] connection_params Parameters of the established connection
 *
 * @note Default connection mode is @ref AT_BLE_CONNECTION_AUTO
 *
 * @sa at_ble_set_conn_mode
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_connect(at_ble_addr_t peers[], uint8_t peer_count,
                               uint16_t scan_interval, uint16_t scan_window, at_ble_connection_params_t *connection_params);

/** @ingroup gap_conn_group
 * @brief Cancels an ongoing connection attempt
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_connect_cancel(void);

/**@ingroup gap_sec_group
 * @brief Set specific link security configuration and bonding data
 *
 * @param[in] signature_info    signature_info, more info at @ref at_ble_signature_info_t
 * @param[in] authen            Authentication value, one of @ref at_ble_auth_t
 * @param[in] service_changed   Service Changed Indication enabled
 *
 * @note
 * - This function must be called before signed write operation to exchange CSR Keys first.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_send_sec_config(at_ble_signature_info_t *signature_info, at_ble_auth_t authen, bool service_changed);

/** @ingroup gap_dev_config_group
  * @brief Set device configuration
  *
  * @param[in] config Pointer to configurations data structure
  *
  * @note Default values would be applied in case of not assign another configurations values.
  *     - Role              : @ref AT_BLE_ROLE_ALL
  *     - Renew Duration    : @ref AT_RENEW_DUR_VAL_MIN
  *     - Address           : 0x1032547698BA
  *     - IRKey             : 0x1032547698BADCFE1101321541761981BA1DC1FE
  *     - Address Type      : @ref AT_BLE_ADDRESS_PUBLIC
  *     - Attribute Config  : 0x20, which means:
  *             -# Name Permission          : @ref AT_BLE_WRITE_DISABLE
  *             -# Appearance Permission    : @ref AT_BLE_WRITE_DISABLE
  *             -# Enable SPCS              : false
  *             -# Enable Service Changed   : true
  *             -# RFU                      : @ref AT_BLE_WRITE_DISABLE
  *     - GAP Start Handle  : @ref AT_BLE_AUTO_ALLOC_HANDLE
  *     - GATT Start Handle : @ref AT_BLE_AUTO_ALLOC_HANDLE
  *     - Maximum MTU       : @ref AT_MTU_VAL_RECOMMENDED
  *
  * @sa @ref at_ble_dev_config_t
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_dev_config(at_ble_dev_config_t *config);

/** @ingroup gap_dev_config_group
  *@brief Set channel map
  *
  * @param[in] map pointer to mapping values
  *
  * @sa at_ble_channel_map_t
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_set_channel_map(at_ble_channel_map_t *map);

/** @ingroup gap_conn_group
 *@brief Disconnect a connected peer device and receive response through @ref AT_BLE_DISCONNECTED event
 *
 *
 * @param[in] handle handle of the connection to be terminated
 * @param[in] reason disconnection reason, more info at @ref at_ble_disconnect_reason_t
 *
 * @note
 * - At @ref AT_BLE_DISCONNECTED event, if returned reason is not one of @ref at_ble_disconnect_reason_t,
 *   so check the reason against error codes @ref at_ble_status_t because this means something wrong was happened
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_disconnect(at_ble_handle_t handle, at_ble_disconnect_reason_t reason);


/** @ingroup gap_conn_group
 *@brief Update the connection parameters of an ongoing connection. \n
 * Connection parameter update command can be used by both master and slave of the connection. \n
 * For master of the connection, new connection parameters will be applied immediately.\n
 * For slave of the connection, a connection update message request will be send to master. Then
 * master will be able to accept or refuse those parameters within 30 seconds otherwise link is automatically disconnected.
 * 
 * @note 
 * This API returns after programming the new values but before they take effect,
 * actual effect of the parameters is marked by the event @ref AT_BLE_CONN_PARAM_UPDATE_DONE
 *
 * @param[in] handle handle of the connection to be updated
 * @param[in] connection_params new parameters to be used
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_connection_param_update(at_ble_handle_t handle,
        at_ble_connection_params_t *connection_params);

/** @ingroup gap_conn_group
  * @brief Reply to connection parameters update request @ref AT_BLE_CONN_PARAM_UPDATE_REQUEST
  *
  * @param[in] handle handle of the connection to be updated
  * @param[in] accept accept or refuse connection parameters proposed by peer slave device
  * @param[in] ce_len_min Minimum CE length
  * @param[in] ce_len_max Maximum CE length
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_conn_update_reply(at_ble_handle_t handle,
        bool accept,
        uint16_t ce_len_min,
        uint16_t ce_len_max);

/** @ingroup gap_sec_group
 *@brief Starts Pairing procedure for a given connection
 *
 * This API will initiate the authentication procedure, given local device requirements and
 * IO capabilities the authentication will proceed in a number of ways, progress is monitored and
 * controlled via the events : @ref AT_BLE_PAIR_KEY_REQUEST ,@ref AT_BLE_PAIR_DONE
 *
 * In the central role, this function will send an SMP Pairing Request,
 * otherwise in the peripheral role, this function will reply to
 * @ref AT_BLE_PAIR_REQUEST.
 *
 * @note
 * - The authentication procedure with this API shall only be initiated after a connection has been established in master mode
 *   or as a response in case of receiving AT_BLE_PAIR_REQUEST event in slave mode.
 * - Bonding information(information exchanged during pairing)such as key, authentication level
 *   should be stored by application in order to be reused during another connection @ref at_ble_send_sec_config .
 * - If the device acts as a master, so initiator_keys is the device itself and responder_keys for peer and in case
 *   of acting as a slave, responder_keys for the device itself and initiator_keys for peer.
 * - According to IO capabilities or Out Of Band (OOB), if it is possible to perform a pairing using PIN code or OOB data then
 *   @ref AT_BLE_PAIR_KEY_REQUEST event will be received by application.
 * - If no security message is exchange during more than 30s, bonding procedure is cancelled and no new bond
 *   procedure can be started for this link.
 *   For your security and in case of a timeout error, it is optional for the application to disconnect the current connection.
 *
 * @param[in] conn_handle handle of the ongoing connection
 * @param[in] features local device requirements and IO capabilities, should be NULL to reject the pairing request in case of pairing as SLAVE connection ONLY.
 * @param[in] ltk Local device Long Term Key, null if not available
 * @param[in] csrk Local device Connection Signature Resolution Key, null if not available
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t.
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_authenticate(at_ble_handle_t conn_handle, at_ble_pair_features_t *features,
                                    at_ble_LTK_t *ltk, at_ble_CSRK_t *csrk);

/** @ingroup gap_sec_group
  *@brief Send slave security request to master.
  *
  * @param[in] conn_handle handle of the ongoing connection
  * @param[in] mitm_protection Boolean value for Man-In-The-Middle protection FALSE -> Disable, TRUE -> Enable
  * @param[in] bond
  *
  * @note
  * - This function should be called in slave mode only to notify master to initiate a security request.
  *   In this case, the application should receive @ref AT_BLE_SLAVE_SEC_REQUEST and send @ref at_ble_authenticate.
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t.
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_send_slave_sec_request(at_ble_handle_t conn_handle, bool mitm_protection, bool bond);

/** @ingroup gap_sec_group
 *@brief Provides a passkey or OOB data that was requested via @ref AT_BLE_PAIR_KEY_REQUEST event, If
 * key type is @ref AT_BLE_PAIR_PASSKEY, then a 6-byte ASCII string (digit 0..9 only).
 * If key type is @ref AT_BLE_PAIR_OOB , then a 16-byte OOB key value in Little Endian format
 *
 * @param[in] conn_handle handle of the ongoing connection
 * @param[in] type type of requested key, must match the one asked for in @ref AT_BLE_PAIR_KEY_REQUEST
 * @param[in] key the key requested, should be NULL to reject the pairing TK exchange and terminate pairing procedure.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t.
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_pair_key_reply(at_ble_handle_t conn_handle, at_ble_pair_key_type_t type, uint8_t *key);

/** @ingroup gap_sec_group
 *@brief Starts encryption, once encryption starts @ref AT_BLE_ENCRYPTION_STATUS_CHANGED event is delivered
 *
 * @param[in] conn_handle handle of the connection to be updated
 * @param[in] key LTK key used for encryption
 * @param[in] auth authentication level , this information must be stored in device database
 * after each pairing process @ref at_ble_pair_done_t
 *
 * @note
 * - This procedure can be initiated only by master of the connection.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_encryption_start(at_ble_handle_t conn_handle, at_ble_LTK_t *key , at_ble_auth_t auth);

/** @ingroup gap_sec_group
 *@brief Responds to encryption start request from master device @ref AT_BLE_ENCRYPTION_REQUEST, once encryption starts @ref AT_BLE_ENCRYPTION_STATUS_CHANGED event is delivered
 *
 * @param[in] conn_handle handle of the connection to be updated
 * @param[in] key_found If true then a valid key is found in device database and will be used
 * @param[in] auth authentication level
 * @param[in] key LTK key used for encryption
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_encryption_request_reply(at_ble_handle_t conn_handle, at_ble_auth_t auth, bool key_found, at_ble_LTK_t *key);

/** @ingroup gap_addr_mgmt_group
 *@brief Handles request of resolving a resolvable random address ,@ref AT_BLE_RESOLV_RAND_ADDR_STATUS event is used to inform
 * which key has been used to perform resolution for the returned address.
 *
 * @param[in] nb_key number of provided IRK
 * @param[in] rand_addr pointer to the random address
 * @param[in] irk_key array of IRK used for address resolution
 *
 * @note
 * -  irk_key argument, should be known after exchanging keys at security level (bonding).
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_random_address_resolve(uint8_t nb_key, at_ble_addr_t *rand_addr, uint8_t *irk_key);

/** @ingroup gap_misc_group
 *@brief Sets TX power value
 *
 * @param[in] power  TX power value @ref at_ble_tx_power_level_t
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_tx_power_set(at_ble_tx_power_level_t power);

/** @ingroup gap_misc_group
 *@brief Gets TX power value
 *
 * @param[in] power TX power value @ref at_ble_tx_power_level_t
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_tx_power_get(at_ble_tx_power_level_t *power);

/** @ingroup gap_misc_group
 *@brief Gets RX power of a given connection
 *
 * @param[in] conn_handle handle of the connection
 * @param[out] rx_power RX power value
 *
 * @warning Not Supported in release version 2.0
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_rx_power_get(at_ble_handle_t conn_handle, int8_t *rx_power);

/** @ingroup gap_misc_group
 *@brief Gets BTLC1000 Chip ID
 *
 * @param[out] chip_id BTLC1000 chip id
 *
 * @warning Not Supported before release version 2.5
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_chip_id_get(uint32_t *chip_id);
/** @ingroup gap_misc_group
 *@brief Raeds 32 bit from BTLC1000
 *
 * @param[in] address Address to read from
 * @param[out] value Value stored in BTLC1000 memory
 *
 * @warning Not Supported before release version 2.5
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t read_32_from_BTLC1000(uint32_t address, uint32_t *value);

/** @ingroup gap_misc_group
 *@brief Gets BTLC1000 Firmware version
 *
 * @param[out] fw_version BTLC1000 firmware version
 *
 * @warning Not Supported before release version 2.5
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_firmware_version_get(uint32_t *fw_version);

/** @ingroup gatt_client_group
 *@brief Discover all Primary services in a peer device
 *
 * Discover all Primary services in a peer device from a specified start_handle to a specified end_handle, whenever a service is found
 * @ref AT_BLE_PRIMARY_SERVICE_FOUND event is sent and @ref AT_BLE_DISCOVERY_COMPLETE is sent at the end of discover operation.
 *
 * @param[in] conn_handle  handle of the connected peer
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_primary_service_discover_all(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle);

/** @ingroup gatt_client_group
 *@brief Discover Primary service of a given UUID declared and located in a peer device
 *
 * Search will go from start_handle to end_handle, whenever a service of matching UUID
 * is found @ref AT_BLE_PRIMARY_SERVICE_FOUND event is sent and @ref AT_BLE_DISCOVERY_COMPLETE is sent at end of discover operation.
 *
 * @param[in] conn_handle  handle of the connected peer
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 * @param[in] uuid         UUID of the service to be found, with LSB byte First in UUID value member
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_primary_service_discover_by_uuid(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle, at_ble_uuid_t *uuid);

/** @ingroup gatt_client_group
 *@brief Discover all included services declared in a connected device
  *
  * Search will go from start_handle to end_handle, whenever a service is found
  * @ref AT_BLE_INCLUDED_SERVICE_FOUND event is sent and @ref AT_BLE_DISCOVERY_COMPLETE is sent at end of discover operation.
  *
  * @param[in] conn_handle  handle of the connection
  * @param[in] start_handle start of the searched range
  * @param[in] end_handle   end of the searched range
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_ATT_ATTRIBUTE_NOT_FOUND,
  * Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_included_service_discover_all(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle);

/** @ingroup gatt_client_group
 *@brief Discover all Characteristics declared in a connected device
 *
 * Search will go from start_handle to end_handle, whenever a characteristic is found
 * @ref AT_BLE_CHARACTERISTIC_FOUND event is sent and @ref AT_BLE_DISCOVERY_COMPLETE is sent at end of discover operation.
 *
 * @param[in] conn_handle  handle of the connection
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_ATT_ATTRIBUTE_NOT_FOUND,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_discover_all(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle);

/** @ingroup gatt_client_group
 *@brief Discover all Characteristics of a given UUID declared in a connected device
 *
 * search will go from start_handle to end_handle, whenever a characteristic of matching UUID
 * is found @ref AT_BLE_CHARACTERISTIC_FOUND event is sent and @ref AT_BLE_DISCOVERY_COMPLETE is sent at end of discover operation.
 *
 * @param[in] conn_handle  handle of the connection
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 * @param[in] uuid         UUID of the characteristic to be found, with LSB byte First in UUID value member
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_ATT_ATTRIBUTE_NOT_FOUND,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_discover_by_uuid(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle, at_ble_uuid_t *uuid);

/** @ingroup gatt_client_group
 *@brief Discover all Descriptors declared in a connected device
 *
 * Search will go from start_handle to end_handle, whenever a descriptor is found
 * @ref AT_BLE_DESCRIPTOR_FOUND event is sent and @ref AT_BLE_DISCOVERY_COMPLETE is sent at end of discover operation.
 *
 * @param[in] conn_handle  handle of the connection
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_ATT_ATTRIBUTE_NOT_FOUND,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_descriptor_discover_all(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle);

/** @ingroup gatt_client_group
 *@brief Read a characteristic given its UUID
 *
 * search will go from start_handle to end_handle, whenever a descriptor with given UUID
 * is found its value will be read and reported via @ref AT_BLE_CHARACTERISTIC_READ_BY_UUID_RESPONSE event
 *
 * @param[in] conn_handle  handle of the connection
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 * @param[in] uuid UUID of the read characteristic, with LSB byte First in UUID value member
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_ATT_ATTRIBUTE_NOT_FOUND,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_read_by_uuid(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle, at_ble_handle_t end_handle, at_ble_uuid_t *uuid);

/** @ingroup gatt_client_group
 *@brief Read a characteristic given its handle
 *
 * Read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @param[in] offset offset at where to start reading
 * @param[in] length    = 0                                then the response will retrieve the full length of the characteristic (read all)
 *                      > 0 && < full length               then the passed length will be red by
 *                      == full length || > full length    full length will be retrieved
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_read(at_ble_handle_t conn_handle, at_ble_handle_t char_handle, uint16_t offset, uint16_t length);

/**@ingroup gatt_client_group
 *@brief Long read a characteristic given its handle
 *
 * read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @param[in] offset offset at where to start reading
 * @param[in] length   * @param[in] length      = 0        then the response will retrieve the full length of the characteristic (read all)
 *                     > 0 && < full length                then the passed length will be red by
 *                     == full length || > full length     full length will be retrieved (read all)
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_read_long(at_ble_handle_t conn_handle, at_ble_handle_t char_handle, uint16_t offset, uint16_t length);

/** @ingroup gatt_client_group
  *@brief Read a list of characteristics given their handles
  *
  * read values will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE  event
  * after delivering all requested value the the user will receive @ref AT_BLE_CHARACTERISTIC_READ_MULTIBLE_RESPONSE to
  * indicate end of procedure
  *
  * @param[in] conn_handle handle of the connection
  * @param[in] char_handle_list list of structures of characteristic handles and their lengths
  * @param[in] char_handle_count number of characteristic handles fro more info, refer to @ref at_ble_char_list
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_read_multiple(at_ble_handle_t conn_handle,  at_ble_char_list *char_handle_list, uint8_t char_handle_count);

/** @ingroup gatt_client_group
 *@brief Write a characteristic value in peer device and write completion will be
 * reported via @ref AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP event after firmware handling.
 *
 * In case of signed write and write without response @ref AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP is triggered as soon as packet has been sent over the air.
 * In case of write with response @ref AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP is triggered if error happened or when response is received from peer device.
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @param[in] offset offset at where to start write
 * @param[in] length length of data to write
 * @param[in] data a buffer where write data is stored
 * @param[in] signed_write if true, perform a signed write
 * @param[in] with_response if true, perform a write with response procedure, otherwise perform write with no response procedure
 *
 * @note
 * - If data length > MTU-3 then the write operation will be converted to long write instead of normal write operation,
 *   this scenario will be taken over by BLE stack firmware.
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_write(at_ble_handle_t conn_handle, at_ble_handle_t char_handle, uint16_t offset, uint16_t length, uint8_t *data,
        bool signed_write, bool with_response);

/** @ingroup gatt_client_group
 *@brief add a new prepared write request
 *
 * Adds a new request to a pending prepared write operation, or starts a new prepared write
 * operation if none was started before
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @param[in] offset offset at where to start write
 * @param[in] length length of data to write
 * @param[in] data a buffer where write data is stored
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_reliable_write_prepare(at_ble_handle_t conn_handle, at_ble_handle_t char_handle, uint16_t offset, uint16_t length, uint8_t *data);

/** @ingroup gatt_client_group
 *@brief Executes a pending prepared write operation
 *
 * send write Execute to peer device to Execute pending prepared write operation, write completion will be reported
 * via @ref AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP event.
 *
 * @param[in] conn_handle handle of the connection
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_reliable_write_execute(at_ble_handle_t conn_handle);

/** @ingroup gatt_client_group
 *@brief Cancels a pending prepared reliable write operation
 *
 * Cancels a pending prepared reliable write operation
 *
 * @param[in] conn_handle handle of the connection
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_reliable_write_cancel(at_ble_handle_t conn_handle);

/** @ingroup gatt_client_group
 *@brief send the MTU exchange command to peer device
 *
 * Sends the MTU Exchange command to peer device to start exchanging the MTu maximum
 * allowed length (Octet). if the peer device changes his MTU value according to the sender's MTU. the sender will
 * receive the indication @ref AT_BLE_MTU_CHANGED_INDICATION  with the exchanged MTU value then receive
 * @ref AT_BLE_MTU_CHANGED_CMD_COMPLETE . if the peer device didn't accept the MTU value @ref AT_BLE_MTU_CHANGED_CMD_COMPLETE will be sent.
 *
 * @param[in] conn_handle  handle of the connection
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_exchange_mtu(at_ble_handle_t conn_handle);

/** @ingroup gatt_server_group
 *@brief Defines a new  service along with its included services and characteristics
 *
 * @param[in] svc The primary service complete description

* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_service_define(at_ble_service_t *svc);

/** @ingroup gatt_server_group
 *@brief Defines a new Primary service along with its included services and characteristics
 *
 * @param[in] uuid The primary service UUID
 * @param[out] service_handle the Service handle will be returned here
 * @param[in] included_service_list an array of included service references
 * @param[in] included_service_count number of elements in included_service_list
 * @param[in, out] charactristic_list an array of characteristics included in the service, this array is update with respective characteristics handles.
 * @param[in] charactristic_count number of elements in charactristic_list
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_primary_service_define(at_ble_uuid_t *uuid, at_ble_handle_t *service_handle,
        at_ble_included_service_t *included_service_list, uint16_t included_service_count,
        at_ble_characteristic_t *charactristic_list, uint16_t charactristic_count);

/** @ingroup gatt_server_group
 *@brief Defines a new Secondary service along with its included services and characteristics
 *
 * @param[in] uuid The secondary service UUID
 * @param[out] service_handle the Service handle will be returned here
 * @param[in] included_service_list an array of included service references
 * @param[in] included_service_count number of elements in included_service_list
 * @param[in, out] charactristic_list an array of characteristics included in the service, this array is update with respective characteristics handles.
 * @param[in] charactristic_count number of elements in charactristic_list
 *
 * @note
 * - Secondary Services are only relevant in the context of the entity that references them,
 * It is therefore forbidden to add a secondary service declaration that is not referenced by another service later in the ATT table
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_secondary_service_define(at_ble_uuid_t *uuid, at_ble_handle_t *service_handle,
        at_ble_included_service_t *included_service_list, uint16_t included_service_count,
        at_ble_characteristic_t *charactristic_list, uint16_t charactristic_count);

/** @ingroup gatt_server_group
 *@brief Sets A characteristic value
 *
 * @param[in] handle Characteristic value handle
 * @param[in] value new value
 * @param[in] len Value length, in bytes
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_value_set(at_ble_handle_t handle, uint8_t *value, uint16_t len);

/** @ingroup gatt_server_group
 *@brief Reads A characteristic value
 *
 * @param[in] handle Characteristic value handle
 * @param[out] value read value will be returned here
 * @param[out] len desired read length, in bytes
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_characteristic_value_get(at_ble_handle_t handle, uint8_t *value, uint16_t *len);
/**@brief Sets A characteristic value
 *
 * @param[in] handle Descriptor value handle
 * @param[in] value new value
 * @param[in] len Value length, in bytes
 *
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_descriptor_value_set(at_ble_handle_t handle,
        uint8_t *value, uint16_t len);

/**@brief Reads A Descriptor value
*
* @param[in] handle Characteristic value handle
* @param[out] value read value will be returned here
* @param[out] len actual read length, in bytes
*
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
  */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_descriptor_value_get(at_ble_handle_t handle, uint8_t *value, uint16_t *len);

/** @ingroup gatt_server_group
 *@brief Sends a Notification
 * AT_BLE_NOTIFICATION_CONFIRMED is received as soon as notification PDU has been sent over the air
 *
 * @param[in] conn_handle handle of the connection to be notified
 * @param[in] attr_handle handle of the attribute originating the notification
 *
 * @pre If the value has been updated, it is required to use @ref at_ble_characteristic_value_set first
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_notification_send(at_ble_handle_t conn_handle, at_ble_handle_t attr_handle);

/** @ingroup gatt_server_group
 *@brief Sends an Indication
 * AT_BLE_INDICATION_CONFIRMED is received when indication has been correctly received by peer device.
 *
 * @param[in] conn_handle handle of the connection to be notified
 * @param[in] attr_handle handle of the attribute originating the indication
 *
 * @pre If the value has been updated, it is required to use @ref at_ble_characteristic_value_set first
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_indication_send(at_ble_handle_t conn_handle, at_ble_handle_t attr_handle);

/** @ingroup gatt_server_group
 *@brief Sends a Service changed Indication
 *
 * @ref AT_BLE_SERVICE_CHANGED_INDICATION_SENT should be received indicating status of sending the indication
 *
 * @param[in] conn_handle handle of the connection to which the indication will be sent
 * @param[in] start_handle start of affected handle range
 * @param[in] end_handle end of affected handle range
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_send_service_changed_indication(at_ble_handle_t conn_handle,
        at_ble_handle_t start_handle,
        at_ble_handle_t end_handle);

/** @ingroup gatt_server_group
 *@brief Replies to a read authorization request requested by by @ref AT_BLE_READ_AUTHORIZE_REQUEST event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] attr_handle handle of the attribute to read
 * @param[in] grant_authorization if True, Authorization is granted
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_read_authorize_reply(at_ble_handle_t conn_handle,
        at_ble_handle_t attr_handle, bool grant_authorization);

/** @ingroup gatt_server_group
 *@brief Replies to a write authorization request requested by by @ref AT_BLE_WRITE_AUTHORIZE_REQUEST event
 *
 * @param[in] param handle @ref at_ble_characteristic_write_request_t struct
 * @param[in] status @ref AT_BLE_SUCCESS to grant write, Otherwise @ref at_ble_status_t
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_write_authorize_reply(at_ble_characteristic_write_request_t *param, at_ble_status_t status);


/**@ingroup l2cap_group
 * @brief Initialize a credit based channel to receive a credit based connection request, this API is blocking
 *
 * Server opens a channel to receive credit based connection requests
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] sec_level security level refer to @ref at_ble_lecb_sec_level_t
 * @param[in] le_psm LE protocol/service multiplexer, valid dynamic range falls between 0x80 - 0xFF or a fixed
 *            SIG assigned psm may be used between 0x00-0x7F
 * @param[in] cid channel id should be in the range of dynamically allocated channels 0x40 - 0x7F
 * @param[in] initial_credit initial credit allocated for the LE credit based connection
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_create(at_ble_handle_t conn_handle, at_ble_lecb_sec_level_t sec_level, uint16_t le_psm, uint16_t cid,
                                   uint16_t initial_credit);


/**@ingroup l2cap_group
 * @brief Close an initialized credit based channel, this API is blocking
 *
 * Server destroys an already initialized credit based channel
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] le_psm LE protocol/service multiplexer
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_destroy(at_ble_handle_t conn_handle, uint16_t le_psm);

/**@ingroup l2cap_group
 * @brief Initiate a credit based connection
 *
 * Client initiates a credit based connection procedure to a server on a specific channel, in response
 * @ref AT_BLE_LECB_CONNECTED should be received indicating successful or failed connection
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] le_psm LE Protocol/Service Multiplexer
 * @param[in] cid channel id
 * @param[in] intial_credit initial credit allocated for the LE credit based connection
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_connect(at_ble_handle_t conn_handle, uint16_t le_psm, uint16_t cid, uint16_t intial_credit);

/**@ingroup l2cap_group
 * @brief Server confirms if a credit based connection is completed successfully
 *
 * Server accepts or rejects an incoming credit based connection request , this API should be used when a connection
 * request is received @ref AT_BLE_LECB_CONN_REQ
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] le_psm LE Protocol/Service Multiplexer
 * @param[in] status status of operation refer to at_ble_lecb_status
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_cfm(at_ble_handle_t conn_handle, uint16_t le_psm, at_ble_lecb_status status);


/**@ingroup l2cap_group
 * @brief Terminate a credit based connection
 *
 * Server or client disconnects a credit based link, in response event @ref AT_BLE_LECB_DISCONNECTED should
 * be received
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] le_psm LE Protocol/Service Multiplexer
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_disconnect(at_ble_handle_t conn_handle, uint16_t le_psm);


/**@ingroup l2cap_group
 * @brief Inform peer device that local device can receive more packets, this is a blocking API
 *
 * When a local device increments its credit it notifies the peer device that it's able to receive
 * more packets (for example after it has finished processing one or more packets) , the peer device
 * receives an @ref AT_BLE_LECB_ADD_CREDIT_IND event.
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] le_psm LE Protocol/Service Multiplexer
 * @param[in] credit RX credit
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_add_credit(at_ble_handle_t conn_handle, uint16_t le_psm, uint16_t credit);

/**@ingroup l2cap_group
 * @brief Send data.
 *
 * Sends a data packet to peer with length less than @ref AT_BLE_LECB_MAX_PKT_SIZE, in response
 * @ref AT_BLE_LECB_SEND_RESP should be received indicating a free available buffer to send.
 * On the other side the receiving device gets an @ref AT_BLE_LECB_DATA_RECIEVED event.
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] cid channel id
 * @param[in] len data length
 * @param[in] data data pointer
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_lecb_send(at_ble_handle_t conn_handle, uint16_t cid, uint16_t len, uint8_t *data);



/** @ingroup htpt_group
 *@brief This API message shall be used to add an instance of the Health Thermometer service into the database.
 * This should be done during the initialization phase of the device , this is a blocking call and no events will
 * be returned
 *
 * Note : allocated profile will be freed if device is reset or configuration changed @ref gap_dev_config_group
 *
 * @param[in] features Indicate if optional features are supported or not, see @ref at_ble_htpt_db_config_flag
 * @param[in] temp_type type of temperature as defined in org.bluetooth.characteristic.temperature_type
 * @param[in] valid_range_min Minimal measurement interval value
 * @param[in] valid_range_max Maximal measurement interval value
 * @param[in] meas_intv Latest known value for measurement interval
 * @param[in] sec_lvl can be : Disable(0) , Enable(1), Unauth(2), Auth(3) see @ref at_ble_htpt_sec_level
 * @param[out] start_handle : handle of health thermometer service if creation succeeded
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t

 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_htpt_create_db(at_ble_htpt_db_config_flag features,
                                      at_ble_htpt_temp_type  temp_type,
                                      uint16_t valid_range_min,
                                      uint16_t valid_range_max,
                                      uint16_t meas_intv,
                                      at_ble_htpt_sec_level sec_lvl,
                                      at_ble_handle_t *start_handle
                                     );


/** @ingroup htpt_group
 *@brief This API message can be used after the connection with a peer device has been established in order to
 * restore known device bond data, in response a @ref AT_BLE_HTPT_ENABLE_RSP event should be received.
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] ntf_ind_cfg Enable notifications or indications for profile characteristics see @ref at_ble_htpt_ntf_ind_cfg
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t

 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_htpt_enable(at_ble_handle_t conn_handle,
                                   at_ble_htpt_ntf_ind_cfg ntf_ind_cfg
                                  );


/** @ingroup htpt_group
 *@brief This message is used by the application (which handles the temperature device driver and
 * measurements) to send a temperature measurement through the Thermometer role.
 * In response a @ref AT_BLE_HTPT_TEMP_SEND_CFM event should be received
 *
 * @param[in] temp temperature value
 * @param[in] time_stamp The time of reading according to the format defined in @ref at_ble_prf_date_time_t
 * @param[in] flags temp flag see @ref at_ble_htpt_temp_flags
 * @param[in] type temp type see @ref at_ble_htpt_temp_type
 * @param[in] flag_stable_meas stable or intermediate type of temperature  (True stable meas, else false),
 * intermediate type is used for display purposes while the measurement is in progress
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_htpt_temp_send(uint32_t temp,
                                      at_ble_prf_date_time_t *time_stamp,
                                      at_ble_htpt_temp_flags  flags,
                                      at_ble_htpt_temp_type  type,
                                      bool flag_stable_meas);


/** @ingroup htpt_group
 *@brief This message is used by the application to order the HTPT profile to generate an indication (if enabled)
 * of the Measurement Interval Char. This can be done as the application desires, at each connection, or if the
 * measurement interval value has been modified locally (interface for this is not provided since a normal thermometer
 * would have very few configurable UI elements and configuration should be done through Collector)
 * In response a @ref AT_BLE_HTPT_MEAS_INTV_UPD_RSP event should be received
 *
 * @param[in] meas_intv Measurement Interval value
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_htpt_meas_intv_update(uint16_t meas_intv);


/**@brief This message is used by application to confirm that measurement interval  modification is accepted or not
 * by the application.Write confirmation is sent back to peer devices that requests modification and if accepted,
 * all other devices connected which have configured to receive measurement interval change indication will be informed
 * about this modification.
 *
 * @param[in] conn_handle Connection handle
 * @param[in] status operation status code
 *
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
 * Otherwise the function shall return @ref at_ble_status_t
 */
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_htpt_meas_intv_chg_cfm(at_ble_handle_t conn_handle, at_ble_status_t status);

/** @ingroup  dtm_group
*  @brief       pack and send Direct test mode reset command, after calling this API you should wait for @ref AT_BLE_LE_TEST_STATUS event
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_dtm_reset(void);

/** @ingroup dtm_group
*  @brief       pack and send Direct test mode RX test start command, after calling this API you should wait for
*  @ref         AT_BLE_LE_TEST_STATUS event
*
*  @param[in]   frequency_index frequency index to do RX test on (from 0 to 39)
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_dtm_rx_test_start(uint8_t frequency_index);

/** @ingroup dtm_group
*  @brief       Pack and send Direct test mode TX test start command, after calling this API you should wait for @ref AT_BLE_LE_TEST_STATUS event
*  @param[in]   frequency_index frequency index to do TX test on (from 0 to 39)
*  @param[in]   data_length payload length (from 0 to 36)
*  @param[in]   payload packet payload type (from 0 to 7)
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_dtm_tx_test_start(uint8_t frequency_index, uint8_t data_length, at_ble_dtm_payload_type_t payload);

/** @ingroup dtm_group
*  @brief       pack and send Direct test mode stop test command, after calling this API you should wait for @ref AT_BLE_LE_PACKET_REPORT event
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_dtm_stop_test(void);

/* utility functions, might be removed later*/
uint8_t at_ble_uuid_type2len(at_ble_uuid_type_t type);
at_ble_uuid_type_t at_ble_uuid_len2type(uint8_t len);

/**
* @defgroup calib_group Calibration APIs
* @brief This group includes all the Calibration related APIs.
* @{
*/
/** @}*/

/** @ingroup calib_group
*  @brief       Configures periodic Vbat/Vtemp calibration: enable/disable, number of ADC samples needed for averaging, frequency of the calibration in seconds
*  @param[in]   calib_enable enables calibration if set to 1 and disables it if set to 0
*  @param[in]   no_samples number of ADC samples needed to calculate the average ADC output, recommended range from 1 to 16 sample.
*  @param[in]   cal_freq frequency of the calibration in seconds, recommended range from 1 to 60 seconds
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_calib_config(int calib_enable, uint32_t no_samples, uint32_t cal_freq);

/** @ingroup calib_group
*  @brief       Retrieves voltage value in volts
*
*  @param[in]   voltage variable passed to the function to store the retrieved voltage value
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_calib_get_voltage(float* voltage);

/** @ingroup calib_group
*  @brief       Retrieves temperature value in Celsius
*
*  @param[in]   temp variable passed to the function to store the retrieved temperature value
*
* @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS,
* Otherwise the function shall return @ref at_ble_status_t
*/
///@cond IGNORE_DOXYGEN
AT_BLE_API
///@endcond
at_ble_status_t at_ble_calib_get_temp(int* temp);
#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__AT_BLE_API_H__
