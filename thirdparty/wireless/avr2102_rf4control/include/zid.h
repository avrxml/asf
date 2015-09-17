/**
 * @file zid.h
 *
 * @brief API for ZID profile
 *
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#if ((defined ZID_PROFILE) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef ZID_H
#define ZID_H

/* === Includes ============================================================= */

#include "rf4ce.h"
#include "nwk_internal.h"
#include "zid_build_config.h"
#include "gdp.h"
#include "nwk_config.h"

/* === Macros =============================================================== */

/**
 * ZID profile initial NIB attributes
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
/** Controller discovery duration */
#ifndef DISCOVERY_DURATION
#define DISCOVERY_DURATION              0x00186A
#endif

/** The automatic discovery response mode duration shall be set to 0x1c9c38
 *(30s). */
#ifndef AUTO_DISC_DURATION_SYM
#define AUTO_DISC_DURATION_SYM          0x1C9C38
#endif
/** Default value for the discovery repetition interval */
#ifndef DISCOVERY_REPETITION_INTERVAL
#define DISCOVERY_REPETITION_INTERVAL   0x00F424
#endif

/** Default value for maximum number of times the discovery request command is
 * sent */
#ifndef MAX_DISCOVERY_REPETITIONS
#define MAX_DISCOVERY_REPETITIONS       0x1E
#endif

/** Default value for Number of reported node descriptor received during
 * discovery process */
#ifndef MAX_REPORTED_NODE_DESCRIPTORS
#define MAX_REPORTED_NODE_DESCRIPTORS   1
#endif

/** ZID profile constants */

/** The length of time the HID adaptor will repeat a report from a
 *  remote HID class device when its idle rate is non-zero,
 *  before a direct report command frame must be received from
 *  the remote HID class device. */
#define aplcIdleRateGuardTime 1500 /* 1500 ms */
#define aplcIdleRateGuardTime_us (aplcIdleRateGuardTime * 1000UL)

/** The maximum time a HID class device shall take to construct a push
 *  attributes command frame and send it to the HID adaptor during its
 *  configuration state. */
#define aplcMaxConfigPrepTime   200 /* 200ms */
#define aplcMaxConfigPrepTime_us   (aplcMaxConfigPrepTime * 1000UL)

/** The maximum time the HID adaptor shall wait to receive a push attributes
 *  command frame from a HID class device during its configuration state. */
#define aplcMaxConfigWaitTime   300 /* ms */
#define aplcMaxConfigWaitTime_us    (aplcMaxConfigWaitTime * 1000UL)

/** The maximum time a HID class device originator shall wait for a response
 *  to a heartbeat command frame or a request to stay awake via the data
 *  pending subfield of the frame control field of an incoming frame. */
#define aplcMaxHIDRxOnWaitTime  100 /* ms */
#define aplcMaxHIDRxOnWaitTime_us  (aplcMaxHIDRxOnWaitTime * 1000UL)

/** The maximum number of non standard descriptors that can be supported on a
 *  HID class device. The HID adaptor shall be able to store this many non
 *  standard descriptors for each paired HID class device. */
#define aplcMaxNonStdDescCompsPerHID 4

/** The maximum size of a single non standard descriptor component.
 *  The HID adaptor shall be able to store descriptor components of this size
 *  for each component stored on a device. */
#define aplcMaxNonStdDescCompSize   256 /* bytes */

/** The maximum size of a non standard descriptor fragment that will fit
 *  in an over-the-air frame. */
#define aplcMaxNonStdDescFragmentSize  80 /* bytes */

/** The maximum size of a NULL report. The HID adaptor shall be able to store
 *  NULL reports of this size for each component stored on a device. */
#define aplcMaxNullReportSize 16 /* bytes */
/** The maximum time between consecutive report data transmissions.*/
#define aplcMaxReportRepeatInterval 100 /* ms */
#define aplcMaxReportRepeatInterval_us (aplcMaxReportRepeatInterval * 1000UL)

/** The maximum amount of time a device waits after receiving a successful
 *  NLME-AUTO-DISCOVERY.confirm primitive for a pair indication to arrive from
 *  the pairing initiator. */
#define aplcMaxPairIndicationWaitTime   1 /* second */
#define aplcMaxPairIndicationWaitTime_us   1200000UL /*(aplcMaxPairIndicationWaitTime
	                                             ** 1000000UL)*/

/** The maximum time a device shall wait for a response command frame following
 *  a request command frame. */
#define aplcMaxResponseWaitTime     200 /* ms */
#define aplcMaxResponseWaitTime_us  (aplcMaxResponseWaitTime * 1000UL)

/** The maximum number of standard descriptors that can be supported on a HID
 *  class device. The HID adaptor shall be able to store this many standard
 *  descriptors for each paired HID class device. */
#define aplcMaxStdDescCompsPerHID    12

/** Minimum window time permitted when using the interrupt pipe before a safe
 *  transmission is required. */
#define aplcMinIntPipeUnsafeTxWindowTime    50  /* ms */
#define aplcMinIntPipeUnsafeTxWindowTime_us (aplcMinIntPipeUnsafeTxWindowTime *	\
	1000UL)

/** The minimum value of the KeyExTransferCount parameter passed to the pair
 *  request primitive during the push button pairing procedure. */
#define aplcMinKeyExchangeTransferCount     3
#define aplcKeyExchangeTransferCount_def    0x24 /* as per ZID spec */

#define aplZIDProfileVersion_def 0x0001

/**
 * ZID device subclass
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_device_subclass_tag {
	ZID_NO_SUBCLASS     = 0x00,
	ZID_BOOT_INTERFACE_SUBCLASS
} zid_device_subclass_t;

/**
 * ZID protocol code
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_protocol_code_tag {
	PROTOCOL_CODE_NONE     = 0x00,
	PROTOCOL_CODE_KEYBOARD,
	PROTOCOL_CODE_MOUSE
} zid_protocol_code_t;

/**
 * ZID protocol
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_protocol_tag {
	PROTOCOL_BOOT     = 0x00,
	PROTOCOL_REPORT
} zid_protocol_t;

/**
 * ZID profile constants
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
#define HID_PARSER_VERSION                  0x0111
#define HID_DEVICE_SUB_CLASS                ZID_NO_SUBCLASS
#define HID_PROTOCOL_CODE                   PROTOCOL_CODE_NONE
#define HID_COUNTRY_CODE                    0x00
#define HID_DEVICE_RELEASE_NUMBER           0x0000
#define HID_VENDOR_ID                       0x1014
#define HID_PRODUCT_ID                      0x0000
#define HID_NUMBER_ENDPOINTS                0x02
#define HID_POLL_INTERVAL                   1
#define HID_NUM_STD_DESCRIPTORS             2
#define HID_STD_DESCRIPTOR_LIST_0           MOUSE
#define HID_STD_DESCRIPTOR_LIST_1           KEYBOARD
#define HID_STD_DESCRIPTOR_LIST_2           0
#define HID_STD_DESCRIPTOR_LIST_3           0
#define HID_NUM_DESCRIPTORS                 0
#define HID_NUM_NULL_REPORTS                0

/** Constant defines used for TxOption field for report data transmission mode
**/
#define TXO_CTRL_PIPE   0x80
#define TXO_INTER_PIPE  0x00

/* === Types ================================================================ */

/**
 * ZID command codes
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_cmd_code_tag {
	GET_REPORT = 0x01,
	REPORT_DATA,
	SET_REPORT
} SHORTENUM zid_cmd_code_t;

/**
 * ZID attribute Ids
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_attribute_tag {
	/** The minimum value of the KeyExTransferCount parameter passed to the
	 * pair
	 *  request primitive during the push button pairing procedure. */
	aplKeyExchangeTransferCount         = 0x82,

	/** The aplZIDProfileVersion attribute specifies the version of the ZID
	 *  profile specification to which the device was designed.*/
	aplZIDProfileVersion                = 0xA0,

	/** The aplIntPipeSUnsafeTxWindowTime attribute specifies the
	 *  maximum time, during which transmissions use the unacknowledged,
	 *  single channel service, permitted before an acknowledged, multiple
	 *  channel transmission is required. */
	aplIntPipeUnsafeTxWindowTime        = 0xA1,

	aplReportRepeatInterval             = 0xA2,
	/** Private attribute used to set the pending data bit */
	privatePendingData                  = 0xA5,

	/** The aplHIDParserVersion attribute specifies the version of the core
	 * HID
	 *  specification. */
	aplHIDParserVersion                 = 0xe0,

	/** The aplHIDDeviceSubclass attribute specifies the HID subclass code
	 * of
	 *  the HID class device. */
	aplHIDDeviceSubclass,

	/** The aplHIDProtocolCode attribute specifies the HID protocol code of
	 * the
	 *  HID class device supporting the boot protocol. */
	aplHIDProtocolCode,

	/** The aplHIDCountryCode attribute specifies the country for which the
	 *  hardware is localized. */
	aplHIDCountryCode,

	/** The aplHIDDeviceReleaseNumber attribute specifies the release number
	 *  of the device itself. */
	aplHIDDeviceReleaseNumber,

	/** The aplHIDVendorId attribute specifies the USB-IF assigned
	 * identifier
	 *  corresponding to the vendor of the device. */
	aplHIDVendorId,

	/** The aplHIDProductId attribute specifies a manufacturer assigned
	 * product
	 *  identifier for the device. */
	aplHIDProductId,

	/* The aplHIDNumEndpoints attribute specifies the number of endpoints on
	 * the device in addition to the mandatory control endpoint. */
	aplHIDNumEndpoints,

	/** The aplHIDPollInterval attribute specifies the polling interval for
	 * all
	 *  endpoint data transfers. */
	aplHIDPollInterval,

	/** The aplHIDNumStdDescComps attribute specifies the number of RF4CE
	 * ZID
	 *  profile defined standard descriptors supported by the device. */
	aplHIDNumStdDescComps,

	/** The aplHIDStdDescriptorList attribute is of length equal to the
	 * value
	 *  of the aplHIDNumStdDescComps attribute and shall specify the list of
	 *  standard RF4CE ZID profile defined descriptors supported by the
	 * device. */
	aplHIDStdDescCompsList,
	aplHIDNumNullReports,

	/** The aplHIDNumDescriptors attribute specifies the number of non
	 * standard
	 *  HID descriptors stored on the device. */
	aplHIDNumNonStdDescComps                = 0xf0,

	/** The aplHIDDescriptorSpec-i attributes specifies the actual HID
	 *  descriptors supported on the device. */
	aplHIDNonStdDescCompSpec
} SHORTENUM zid_attribute_t;

/**
 * ZID report type field values
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_report_types_tag {
	INPUT   = 0x01,
	OUTPUT,
	FEATURE
} SHORTENUM zid_report_types_t;

typedef enum zid_report_desc_tag {
	MOUSE           = 0x01,
	KEYBOARD,
	CONTACT_DATA,
	TAP_GESTURE,
	SCROLL_GESTURE,
	PINCH_GESTURE,
	ROTATE_GESTURE,
	SYNC,
	TOUCH_SENSOR_PROPERTIES,
	TAP_SUPPORT_PROPERTIES
} SHORTENUM zid_report_desc_t;

/**
 * ZID profile constants
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef struct zid_report_data_record_tag {
	/* uint8_t report_size; */
	zid_report_types_t report_type;
	zid_report_desc_t report_desc_identifier;
	void *report_data;
} zid_report_data_record_t;

typedef struct mouse_desc_tag {
	uint8_t button0;
	uint8_t button1;
	uint8_t button2;
	int8_t x_coordinate;
	int8_t y_coordinate;
} mouse_desc_t;

typedef struct keyboard_input_desc_tag {
	uint8_t modifier_keys;
	uint8_t key_code[6];
} keyboard_input_desc_t;

typedef struct keyboard_output_desc_tag {
	uint8_t num_lock;
	uint8_t caps_lock;
	uint8_t scroll_lock;
	uint8_t compose;
	uint8_t kana;
} keyboard_output_desc_t;

typedef struct touch_sensor_properties_tag {
	uint8_t no_of_additional_contacts;
	uint8_t origin;
	uint8_t reliable_index;
	uint8_t gestures;
	uint8_t resolution_x;
	uint8_t resolution_y;
	uint16_t max_coordinate_x;
	uint16_t max_coordinate_y;
	uint8_t shape;
} touch_sensor_properties_t;

typedef struct tap_support_properties_tag {
	uint8_t single_tap;
	uint8_t tap_and_a_half;
	uint8_t double_tap;
	uint8_t long_tap;
} tap_support_properties_t;

typedef struct sync_report_tag {
	uint8_t gesture;
	uint8_t contact_count;
} sync_report_t;

typedef struct contact_data_report_tag {
	uint8_t contact_type;
	uint8_t contact_index;
	uint8_t contact_state;
	uint8_t major_axis_orientation;
	uint8_t pressure;
	uint16_t location_x;
	uint16_t location_y;
	uint16_t major_axis_length;
	uint16_t minor_axis_length;
} contact_data_report_t;

typedef struct tap_gesture_report_tag {
	uint8_t type;
	uint8_t finger_count;
	uint16_t location_x;
	uint16_t location_y;
} tap_gesture_report_t;

typedef struct scroll_gesture_report_tag {
	uint8_t type;
	uint8_t finger_count;
	uint8_t direction;
	uint16_t distance;
} scroll_gesture_report_t;

typedef struct pinch_gesture_report_tag {
	uint8_t finger_present;
	uint8_t direction;
	uint16_t distance;
	uint16_t center_x;
	uint16_t center_y;
} pinch_gesture_report_t;

typedef struct rotation_gesture_report_tag {
	uint8_t finger_present;
	uint8_t direction;
	uint8_t magnitude;
} rotation_gesture_report_t;

typedef struct hid_attr_tag {
	uint16_t ParserVersion;
	uint8_t DeviceSubclass;
	uint8_t ProtocolCode;
	uint8_t CountryCode;
	uint16_t DeviceReleaseNumber;
	uint16_t VendorId;
	uint16_t ProductId;
	uint8_t NumEndpoints;
	uint8_t PollInterval;
	uint8_t NumStdDescriptors;
	zid_report_desc_t StdDescriptorList[aplcMaxStdDescCompsPerHID];
	uint8_t NumNullReports;
	uint8_t NumDescriptors;
	uint8_t DescriptorSpec[aplcMaxNonStdDescCompsPerHID][
		aplcMaxNonStdDescCompSize + 4];
#ifdef ZID_ADAPTOR
	uint8_t TouchSensorProperties[7]; /* zid_agas */
	uint8_t TapSupportProperties[4];
#endif
} hid_attr_t;

typedef struct hid_adaptor_attr_tag {
	uint16_t ParserVersion;
	uint8_t CountryCode;
	uint16_t DeviceReleaseNumber;
	uint16_t VendorId;
	uint16_t ProductId;
} hid_adaptor_attr_t;

typedef struct null_report_tag {
	zid_report_types_t report_type;
	uint8_t report_desc_identifier;
	uint8_t report_data_length;
	uint8_t report_data[32];
} null_report_t;

typedef struct pair_table_tag {
	uint16_t IntPipeUnsafeTxWindowTime;
	uint32_t LastSafeTx;

#ifdef ZID_ADAPTOR
	bool PendingData;
	hid_attr_t hid; /* HID information of connected device(s) */
	null_report_t null_report;
#endif
} pair_table_t;

/**
 * ZID information base
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef struct zid_ib_tag {
	uint8_t KeyExchangeTransferCount;
	uint16_t ZIDProfileVersion;
#ifdef ZID_DEVICE
	uint8_t ReportRepeatInterval;
	uint8_t TouchSensorProperties[7];
	uint8_t TapSupportProperties[4];
#endif
#ifdef ZID_ADAPTOR
	hid_adaptor_attr_t hid;
#endif
#ifdef ZID_DEVICE
	hid_attr_t hid; /* own/local HID data base */
	null_report_t null_report[aplcMaxNonStdDescCompsPerHID];
#endif
	pair_table_t PairingTable[nwkcMaxPairingTableEntries];
} zid_ib_t;

#if 0

/**
 * ZID data types
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_data_types_tag {
	UNSIGNED_8_BIT      = 0x20,
	UNSIGNED_16_BIT     = 0x21,
	UNSIGNED_24_BIT     = 0x22,
	DESCRIPTOR_SPEC     = 0x60,
	LIST_U_8_BIT        = 0x61,
	OCTET_LIST          = 0x62
} SHORTENUM zid_data_types_t;
#endif

/**
 * Attribute status
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum zid_attribute_status_tag {
	ATTRIBUTE_SUCCESS     = 0x00,
	ATTRIBUTE_UNSUPPORTED,
	ATTRIBUTE_ILLEGAL
} SHORTENUM zid_attribute_status_t;

typedef enum zid_standby_tag {
	STDBY_ACTIVE,
	STDBY_DISABLED,
	STDBY_ACTIVE_REQ,
	STDBY_DISABLE_REQ,
	STDBY_DISABLE_INTERN_REQ
} SHORTENUM zid_standby_t;

#if (defined RF4CE_CALLBACK_PARAM) || (defined DOXYGEN)

#if (defined ZID_DEVICE) || (defined DOXYGEN)
typedef void (*zid_heartbeat_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);
typedef void (*zid_get_report_indication_cb_t)(uint8_t PairingRef,
		zid_report_types_t zid_report_type,
		zid_report_desc_t zid_report_desc, uint8_t RxLinkQuality,
		uint8_t RxFlags);
#endif

typedef void (*zid_report_data_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);

#if (defined ZID_ADAPTOR) || (defined DOXYGEN)
typedef void (*zid_standby_confirm_cb_t)(nwk_enum_t Status, bool StdbyEnabled);
typedef void (*zid_get_report_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);
typedef void (*zid_set_report_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);
#endif

typedef void (*zid_get_attribute_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef, zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex, uint8_t AttributeSize,
		uint8_t *ZIDAttributeValue);

typedef void (*zid_set_attribute_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef,
		zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex);
typedef void (*zid_connect_confirm_cb_t)(nwk_enum_t Status, uint8_t PairingRef);
typedef void (*zid_data_confirm_cb_t)(nwk_enum_t Status, uint8_t PairingRef);

#if (defined ZID_ADAPTOR) || (defined DOXYGEN)
typedef void (*zid_heartbeat_indication_cb_t)(uint8_t PairingRef);
/* typedef void (*zid_heartbeat_indication_cb_t)(uint8_t PairingRef); */
typedef void (*zid_standby_leave_indication_cb_t)(void);
#endif

typedef void (*zid_report_data_indication_cb_t)(uint8_t PairingRef,
		uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags);

#ifdef VENDOR_DATA
typedef void (*vendor_data_ind_cb_t)(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength,
		uint8_t *nsdu, uint8_t RxLinkQuality, uint8_t RxFlags);
#endif

/**
 * struct for zrc indication callback.
 * App should use this struct to register indication callback functions
 *
 * @ingroup apiRF4Control_ZRC_API
 */
typedef struct zid_indication_callback_tag {
#if (defined ZID_ADAPTOR) || (defined DOXYGEN)
	zid_heartbeat_indication_cb_t zid_heartbeat_indication_cb;
	zid_standby_leave_indication_cb_t zid_standby_leave_indication_cb;
#endif
#if (defined VENDOR_DATA) || (defined DOXYGEN)
	vendor_data_ind_cb_t vendor_data_ind_cb;
#endif
	zid_report_data_indication_cb_t zid_report_data_indication_cb;
#if (defined ZID_DEVICE) || (defined DOXYGEN)
	zid_get_report_indication_cb_t zid_get_report_indication_cb;
#endif
} zid_indication_callback_t;

#endif /* (defined RF4CE_CALLBACK_PARAM) || (defined DOXYGEN) */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

void zid_reset(bool SetDefault);

void zid_reset_attributes(void);

/* bool get_attribute_properties(uint8_t PairingRef, zid_attribute_t
 * attribute_id, uint8_t index, zid_data_types_t *type, uint8_t *length, uint8_t
 * **value); */
bool get_attribute_properties(uint8_t PairingRef, zid_attribute_t attribute_id,
		uint8_t index, uint8_t *length, uint8_t *value);

/*
 * void zid_get_ib_from_nvm(void);
 * void zid_store_ib_to_nvm(void);
 */

bool send_generic_response(uint8_t PairingRef, gdp_response_code_t code);

#ifdef ZID_DEVICE
bool check_zid_adaptor_compatibility(uint8_t PairingRef, uint8_t payload_length,
		uint8_t *payload);

#endif

#ifdef ZID_ADAPTOR

bool zid_standby_request(bool standby_request
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR zid_standby_confirm
#endif
		);

bool zid_rec_connect_request(uint8_t RecAppCapabilities,
dev_type_t RecDevTypeList[3],
profile_id_t RecProfileIdList[7]
#ifdef RF4CE_CALLBACK_PARAM
, FUNC_PTR zid_connect_confirm
#endif
);

#endif

#ifdef ZID_DEVICE
bool zid_org_connect_request(uint8_t OrgAppCapabilities,
dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE],
profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE],
dev_type_t SearchDevType, uint8_t DiscProfileIdListSize,
profile_id_t DiscProfileIdList[PROFILE_ID_LIST_SIZE]
#ifdef RF4CE_CALLBACK_PARAM
, FUNC_PTR zid_connect_confirm
#endif
);

bool zid_heartbeat_request(uint8_t PairingRef
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR zid_heartbeat_confirm
#endif
		);

bool zid_set_null_report(uint8_t report_index, null_report_t *null_report_ptr);

#endif /* ZID_DEVICE */

bool zid_report_data_request(uint8_t PairingRef, uint8_t numReportRecords,
		zid_report_data_record_t *reportRecord, uint8_t TxOptions
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR zid_report_data_confirm
#endif
		);

#ifdef ZID_ADAPTOR
bool zid_set_report_request(uint8_t PairingRef, uint8_t payloadlength,
		uint8_t *payload, uint8_t TxOptions
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR confirm_cb
#endif
		);

#endif

bool zid_get_report_data_request(uint8_t PairingRef,
		zid_report_types_t zid_report_type,
		zid_report_desc_t zid_report_desc, \
		uint8_t TxOptions
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR zid_get_report_confirm
#endif
		);

#if (ZID_SET == 1)
bool zid_set_attribute_request(uint8_t PairingRef, zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex, uint8_t *ZIDAttributeValue
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR zid_set_attribute_confirm
#endif
		);

#endif

#if (ZID_GET == 1)
bool zid_get_attribute_request(bool OTA, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex
#ifdef RF4CE_CALLBACK_PARAM
		, FUNC_PTR zid_get_attribute_confirm
#endif
		);

/* void zid_get_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
 * zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex, uint8_t
 * *ZIDAttributeValue); */
#endif

#if (!defined RF4CE_CALLBACK_PARAM) || (defined DOXYGEN)

/**
 * @brief Handles the cmd data indications received from the network layer.
 *
 * @param PairingRef        Pairing reference
 * @param nsduLength        Length of the received data
 * @param nsdu              Actual data
 * @param RxLinkQuality     Link quality of received packet
 * @param RxFlags           Rx Flags.
 */
void zid_data_indication(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags);

#ifdef VENDOR_DATA
void vendor_data_ind(uint8_t PairingRef, uint16_t VendorId, uint8_t nsduLength,
		uint8_t *nsdu, uint8_t RxLinkQuality, uint8_t RxFlags);

#endif

/**
 * @brief Handle the data confirm (received from network layer).
 *
 * @param Status           Status received from the network layer.
 * @param PairingRef       Pairing reference
 */
void zid_data_confirm(nwk_enum_t Status, uint8_t PairingRef);

void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef);

#ifdef ZID_DEVICE
void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef);
void zid_heartbeat_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif
void zid_get_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex,
		uint8_t AttributeSize, uint8_t *ZIDAttributeValue);
void zid_set_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex);

#ifdef ZID_ADAPTOR
void zid_heartbeat_indication(uint8_t PairingRef);

void zid_report_data_indication(uint8_t PairingRef, uint8_t num_report_records,
zid_report_data_record_t * zid_report_data_record_ptr[], uint8_t RxLinkQuality,
uint8_t RxFlags);
void zid_standby_confirm(nwk_enum_t Status, bool StdbyEnable);
void zid_standby_leave_indication(void);

#endif
#endif /* #if (!defined RF4CE_CALLBACK_PARAM) */

/**
 * @brief Registering zrc indication callback
 *
 * The application needs to register the zrc indication call back functions
 * so that the corresponding indication function will get called
 * by the profile on receiving the indication from the network layer.
 *
 * @param zrc_indication_callback_t    indication callback struct
 *
 * @see zrc_indication_callback_t
 * @ingroup apiRF4Control_ZRC_API
 */
#if (defined RF4CE_CALLBACK_PARAM) || (defined DOXYGEN)

void register_zid_indication_callback(
		zid_indication_callback_t *zid_ind_callback);

#endif

#endif /* ZID_H */

#endif  /* #if ((defined ZID_PROFILE) || (defined DOXYGEN)) */

/* EOF */
