/**
 * \file
 *
 * \brief ATTS.
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

#ifndef ATTS_TASK_H_INCLUDED
#define ATTS_TASK_H_INCLUDED

#include "nmi_types.h"
#include "cmn_defs.h"
#include "msg_if.h"

uint8_t atts_add_svc_req_handler(uint16_t *start_hdl, uint16_t total_size, uint8_t nb_att_uuid_16,
		uint8_t nb_att_uuid_32, uint8_t nb_att_uuid_128);
uint8_t atts_add_attribute_req_handler (uint16_t start_hdl, uint16_t max_data_size, uint16_t perm,
		uint8_t uuid_len, uint8_t *uuid, uint16_t *handle);
uint8_t atts_svc_get_permission_req_handler(uint16_t start_hdl, uint8_t *perm);
uint8_t atts_svc_set_permission_req_handler (uint16_t start_hdl, uint8_t perm);
uint8_t atts_att_get_permission_req_handler(uint16_t handle, uint16_t *perm);
uint8_t atts_att_set_permission_req_handler (uint16_t handle, uint16_t perm);
uint8_t atts_att_get_value_req_handler (uint16_t handle, uint16_t *length, uint8_t *value);
uint8_t atts_att_set_value_req_handler(uint16_t handle, uint16_t length, uint8_t *value);
uint8_t atts_destroy_db_req_handler(uint16_t gap_hdl, uint16_t gatt_hdl);


/// Characteristic Value Descriptor
struct atts_char_desc
{
    /// properties
    uint8_t prop;
    /// attribute handle
    uint8_t attr_hdl[ATT_UUID_16_LEN];
    /// attribute type
    uint8_t attr_type[ATT_UUID_16_LEN];
};

/// Attribute description (used to create database)
typedef struct
 {
     /// UUID length
	uint8_t uuid_len;
	/// UUID value
	uint8_t *uuid;
     /// Attribute permission
	uint16_t perm;
     /// Maximum length of the element
	uint16_t max_length;
     /// Current length of the element
	uint16_t length;
     /// Element value array
	uint8_t* value;
 }atts_desc;


/// ATTS task states
enum
{
    /// IDLE state
    ATTS_IDLE,
    /// Indicating state (wait for message confirmation)
    ATTS_INDICATE,
    /// Waiting for Resource device initialization
    ATTS_RESOURCE_INIT,
    /// total number of ATTS states.
    ATTS_STATE_MAX
};

/// ATTS messages
enum
{
    /// Handle value notification request
    ATTS_HDL_VAL_NTF_REQ = 0x2800,
    /// Handle value indication request
    ATTS_HDL_VAL_IND_REQ,
    /// Response timeout indication
    ATTS_RTX_IND,
    /// Optional status/error indication
    ATTS_REQ_ERROR_IND,

    /* Database Management */
    /// Add service in database request
    ATTS_ADD_SVC_REQ,
    /// Add service in database response
    ATTS_ADD_SVC_RSP,
    /// Add attribute in service database request
    ATTS_ADD_ATTRIBUTE_REQ,
    /// Add attribute in service database response
    ATTS_ADD_ATTRIBUTE_RSP,

    /* Service management */
    /// Get permission settings of service request
    ATTS_SVC_GET_PERMISSION_REQ,
    /// Get permission settings of service response
    ATTS_SVC_GET_PERMISSION_RSP,
    /// Set permission settings of service request
    ATTS_SVC_SET_PERMISSION_REQ,
    /// Set permission settings of service response
    ATTS_SVC_SET_PERMISSION_RSP,

    /* Attribute management */
    /// Get permission settings of attribute request
    ATTS_ATT_GET_PERMISSION_REQ,
    /// Get permission settings of attribute response
    ATTS_ATT_GET_PERMISSION_RSP,
    /// Set permission settings of attribute request
    ATTS_ATT_SET_PERMISSION_REQ,
    /// Set permission settings of attribute response
    ATTS_ATT_SET_PERMISSION_RSP,

    /// Get attribute value request
    ATTS_ATT_GET_VALUE_REQ,
    /// Get attribute value response
    ATTS_ATT_GET_VALUE_RSP,
    /// Set attribute value request
    ATTS_ATT_SET_VALUE_REQ,
    /// Set attribute value response
    ATTS_ATT_SET_VALUE_RSP,


    /* Debug messages */
    /// DEBUG ONLY: Destroy Attribute database request
    ATTS_DESTROY_DB_REQ,
    /// DEBUG ONLY: Destroy Attribute database response
    ATTS_DESTROY_DB_RSP,
    /// DEBUG ONLY: Retrieve list of services request
    ATTS_GET_SVC_LIST_REQ,
    /// DEBUG ONLY: Retrieve list of services response
    ATTS_GET_SVC_LIST_RSP
};

/// Common 16-bit Universal Unique Identifier
enum {
    ATT_INVALID_UUID = 0,
    /*----------------- SERVICES ---------------------*/
    /// Generic Access Profile
    ATT_SVC_GENERIC_ACCESS                      = 0x1800,
    /// Attribute Profile
    ATT_SVC_GENERIC_ATTRIBUTE,
    /// Immediate alert Service
    ATT_SVC_IMMEDIATE_ALERT,
    /// Link Loss Service
    ATT_SVC_LINK_LOSS,
    /// Tx Power Service
    ATT_SVC_TX_POWER,
    /// Current Time Service Service
    ATT_SVC_CURRENT_TIME,
    /// Reference Time Update Service
    ATT_SVC_REF_TIME_UPDATE,
    /// Next DST Change Service
    ATT_SVC_NEXT_DST_CHANGE,
    /// Glucose Service
    ATT_SVC_GLUCOSE                             = 0x1808,
    /// Health Thermometer Service
    ATT_SVC_HEALTH_THERMOM                      = 0x1809,
    /// Device Information Service
    ATT_SVC_DEVICE_INFO                         = 0x180A,
    /// Heart Rate Service
    ATT_SVC_HEART_RATE                          = 0x180D,
    /// Phone Alert Status Serice
    ATT_SVC_PHONE_ALERT_STATUS,
    /// Battery Service
    ATT_SVC_BATTERY_SERVICE,
    /// Blood Pressure Service
    ATT_SVC_BLOOD_PRESSURE                      = 0x1810,
    /// Alert Notification Service
    ATT_SVC_ALERT_NTF                           = 0x1811,
    /// HID Servuce
    ATT_SVC_HID                                 = 0x1812,
    /// Scan Parameters Service
    ATT_SVC_SCAN_PARAMETERS                     = 0x1813,
    /// Running Speed and Cadence Service
    ATT_SVC_RUNNING_SPEED_CADENCE               = 0x1814,
    /// Cycling Speed and Cadence Service
    ATT_SVC_CYCLING_SPEED_CADENCE               = 0x1816,


    /*------------------- UNITS ---------------------*/
    /// No defined unit
    ATT_UNIT_UNITLESS                       = 0x2700,
    /// Length Unit - Metre
    ATT_UNIT_METRE,
    //Mass unit - Kilogram
    ATT_UNIT_KG,
    ///Time unit - second
    ATT_UNIT_SECOND,
    ///Electric current unit - Ampere
    ATT_UNIT_AMPERE,
    ///Thermodynamic Temperature unit - Kelvin
    ATT_UNIT_KELVIN,
    /// Amount of substance unit - mole
    ATT_UNIT_MOLE,
    ///Luminous intensity unit - candela
    ATT_UNIT_CANDELA,
    ///Area unit - square metres
    ATT_UNIT_SQ_METRE                       = 0x2710,
    ///Colume unit - cubic metres
    ATT_UNIT_CUBIC_METRE,
    ///Velocity unit - metres per second
    ATT_UNIT_METRE_PER_SECOND,
    ///Acceleration unit - metres per second squared
    ATT_UNIT_METRES_PER_SEC_SQ,
    ///Wavenumber unit - reciprocal metre
    ATT_UNIT_RECIPROCAL_METRE,
    ///Density unit - kilogram per cubic metre
    ATT_UNIT_DENS_KG_PER_CUBIC_METRE,
    ///Surface density unit - kilogram per square metre
    ATT_UNIT_KG_PER_SQ_METRE,
    ///Specific volume unit - cubic metre per kilogram
    ATT_UNIT_CUBIC_METRE_PER_KG,
    ///Current density unit - ampere per square metre
    ATT_UNIT_AMPERE_PER_SQ_METRE,
    ///Magnetic field strength unit - Ampere per metre
    ATT_UNIT_AMPERE_PER_METRE,
    ///Amount concentration unit - mole per cubic metre
    ATT_UNIT_MOLE_PER_CUBIC_METRE,
    ///Mass Concentration unit - kilogram per cubic metre
    ATT_UNIT_MASS_KG_PER_CUBIC_METRE,
    ///Luminance unit - candela per square metre
    ATT_UNIT_CANDELA_PER_SQ_METRE,
    ///Refractive index unit
    ATT_UNIT_REFRACTIVE_INDEX,
    ///Relative permeability unit
    ATT_UNIT_RELATIVE_PERMEABILITY,
    ///Plane angle unit - radian
    ATT_UNIT_RADIAN                         = 0x2720,
    ///Solid angle unit - steradian
    ATT_UNIT_STERADIAN,
    ///Frequency unit - Hertz
    ATT_UNIT_HERTZ,
    ///Force unit - Newton
    ATT_UNIT_NEWTON,
    ///Pressure unit - Pascal
    ATT_UNIT_PASCAL,
    ///Energy unit - Joule
    ATT_UNIT_JOULE,
    ///Power unit - Watt
    ATT_UNIT_WATT,
    ///electric Charge unit - Coulomb
    ATT_UNIT_COULOMB,
    ///Electric potential difference - Volt
    ATT_UNIT_VOLT,
    ///Capacitance unit - Farad
    ATT_UNIT_FARAD,
    ///electric resistance unit - Ohm
    ATT_UNIT_OHM,
    ///Electric conductance - Siemens
    ATT_UNIT_SIEMENS,
    ///Magnetic flux unit - Weber
    ATT_UNIT_WEBER,
    ///Magnetic flux density unit - Tesla
    ATT_UNIT_TESLA,
    ///Inductance unit - Henry
    ATT_UNIT_HENRY,
    ///Temperature unit - degree Celsius
    ATT_UNIT_CELSIUS,
    ///Luminous flux unit - lumen
    ATT_UNIT_LUMEN,
    //Illuminance unit - lux
    ATT_UNIT_LUX,
    ///Activity referred to a radionuclide unit - becquerel
    ATT_UNIT_BECQUEREL,
    ///Absorbed dose unit - Gray
    ATT_UNIT_GRAY,
    ///Dose equivalent unit - Sievert
    ATT_UNIT_SIEVERT,
    ///Catalytic activity unit - Katal
    ATT_UNIT_KATAL,
    ///Synamic viscosity unit - Pascal second
    ATT_UNIT_PASCAL_SECOND                      = 0x2740,
    ///Moment of force unit - Newton metre
    ATT_UNIT_NEWTON_METRE,
    ///surface tension unit - Newton per metre
    ATT_UNIT_NEWTON_PER_METRE,
    ///Angular velocity unit - radian per second
    ATT_UNIT_RADIAN_PER_SECOND,
    ///Angular acceleration unit - radian per second squared
    ATT_UNIT_RADIAN_PER_SECOND_SQ,
    ///Heat flux density unit - Watt per square metre
    ATT_UNIT_WATT_PER_SQ_METRE,
    ///HEat capacity unit - Joule per Kelvin
    ATT_UNIT_JOULE_PER_KELVIN,
    ///Specific heat capacity unit - Joule per kilogram kelvin
    ATT_UNIT_JOULE_PER_KG_KELVIN,
    ///Specific Energy unit - Joule per kilogram
    ATT_UNIT_JOULE_PER_KG,
    ///Thermal conductivity - Watt per metre Kelvin
    ATT_UNIT_WATT_PER_METRE_KELVIN,
    ///Energy Density unit - joule per cubic metre
    ATT_UNIT_JOULE_PER_CUBIC_METRE,
    ///Electric field strength unit - volt per metre
    ATT_UNIT_VOLT_PER_METRE,
    ///Electric charge density unit - coulomb per cubic metre
    ATT_UNIT_COULOMB_PER_CUBIC_METRE,
    ///Surface charge density unit - coulomb per square metre
    ATT_UNIT_SURF_COULOMB_PER_SQ_METRE,
    ///Electric flux density unit - coulomb per square metre
    ATT_UNIT_FLUX_COULOMB_PER_SQ_METRE,
    ///Permittivity unit - farad per metre
    ATT_UNIT_FARAD_PER_METRE,
    ///Permeability unit - henry per metre
    ATT_UNIT_HENRY_PER_METRE,
    ///Molar energy unit - joule per mole
    ATT_UNIT_JOULE_PER_MOLE,
    ///Molar entropy unit - joule per mole kelvin
    ATT_UNIT_JOULE_PER_MOLE_KELVIN,
    ///Exposure unit - coulomb per kilogram
    ATT_UNIT_COULOMB_PER_KG,
    ///Absorbed dose rate unit - gray per second
    ATT_UNIT_GRAY_PER_SECOND,
    ///Radiant intensity unit - watt per steradian
    ATT_UNIT_WATT_PER_STERADIAN,
    ///Radiance unit - watt per square meter steradian
    ATT_UNIT_WATT_PER_SQ_METRE_STERADIAN,
    ///Catalytic activity concentration unit - katal per cubic metre
    ATT_UNIT_KATAL_PER_CUBIC_METRE,
    ///Time unit - minute
    ATT_UNIT_MINUTE                         = 0x2760,
    ///Time unit - hour
    ATT_UNIT_HOUR,
    ///Time unit - day
    ATT_UNIT_DAY,
    ///Plane angle unit - degree
    ATT_UNIT_ANGLE_DEGREE,
    ///Plane angle unit - minute
    ATT_UNIT_ANGLE_MINUTE,
    ///Plane angle unit - second
    ATT_UNIT_ANGLE_SECOND,
    ///Area unit - hectare
    ATT_UNIT_HECTARE,
    ///Volume unit - litre
    ATT_UNIT_LITRE,
    ///Mass unit - tonne
    ATT_UNIT_TONNE,
    ///Pressure unit - bar
    ATT_UNIT_BAR                            = 0x2780,
    ///Pressure unit - millimetre of mercury
    ATT_UNIT_MM_MERCURY,
    ///Length unit - angstrom
    ATT_UNIT_ANGSTROM,
    ///Length unit - nautical mile
    ATT_UNIT_NAUTICAL_MILE,
    ///Area unit - barn
    ATT_UNIT_BARN,
    ///Velocity unit - knot
    ATT_UNIT_KNOT,
    ///Logarithmic radio quantity unit - neper
    ATT_UNIT_NEPER,
    ///Logarithmic radio quantity unit - bel
    ATT_UNIT_BEL,
    ///Length unit - yard
    ATT_UNIT_YARD                           = 0x27A0,
    ///Length unit - parsec
    ATT_UNIT_PARSEC,
    ///length unit - inch
    ATT_UNIT_INCH,
    ///length unit - foot
    ATT_UNIT_FOOT,
    ///length unit - mile
    ATT_UNIT_MILE,
    ///pressure unit - pound-force per square inch
    ATT_UNIT_POUND_FORCE_PER_SQ_INCH,
    ///velocity unit - kilometre per hour
    ATT_UNIT_KM_PER_HOUR,
    ///velocity unit - mile per hour
    ATT_UNIT_MILE_PER_HOUR,
    ///angular velocity unit - revolution per minute
    ATT_UNIT_REVOLUTION_PER_MINUTE,
    ///energy unit - gram calorie
    ATT_UNIT_GRAM_CALORIE,
    ///energy unit - kilogram calorie
    ATT_UNIT_KG_CALORIE,
    /// energy unit - kilowatt hour
    ATT_UNIT_KILOWATT_HOUR,
    ///thermodynamic temperature unit - degree Fahrenheit
    ATT_UNIT_FAHRENHEIT,
    ///percentage
    ATT_UNIT_PERCENTAGE,
    ///per mille
    ATT_UNIT_PER_MILLE,
    ///period unit - beats per minute)
    ATT_UNIT_BEATS_PER_MINUTE,
    ///electric charge unit - ampere hours
    ATT_UNIT_AMPERE_HOURS,
    ///mass density unit - milligram per decilitre
    ATT_UNIT_MILLIGRAM_PER_DECILITRE,
    ///mass density unit - millimole per litre
    ATT_UNIT_MILLIMOLE_PER_LITRE,
    ///time unit - year
    ATT_UNIT_YEAR,
    ////time unit - month
    ATT_UNIT_MONTH,


    /*---------------- DECLARATIONS -----------------*/
    /// Primary service Declaration
    ATT_DECL_PRIMARY_SERVICE                     = 0x2800,
    /// Secondary service Declaration
    ATT_DECL_SECONDARY_SERVICE,
    /// Include Declaration
    ATT_DECL_INCLUDE,
    /// Characteristic Declaration
    ATT_DECL_CHARACTERISTIC,


    /*----------------- DESCRIPTORS -----------------*/
    /// Characteristic extended properties
    ATT_DESC_CHAR_EXT_PROPERTIES                 = 0x2900,
    /// Characteristic user description
    ATT_DESC_CHAR_USER_DESCRIPTION,
    /// Client characteristic configuration
    ATT_DESC_CLIENT_CHAR_CFG,
    /// Server characteristic configuration
    ATT_DESC_SERVER_CHAR_CFG,
    /// Characteristic Presentation Format
    ATT_DESC_CHAR_PRES_FORMAT,
    /// Characteristic Aggregate Format
    ATT_DESC_CHAR_AGGREGATE_FORMAT,
    /// Valid Range
    ATT_DESC_VALID_RANGE,
    /// External Report Reference
    ATT_DESC_EXT_REPORT_REF,
    /// Report Reference
    ATT_DESC_REPORT_REF,


    /*--------------- CHARACTERISTICS ---------------*/
    /// Device name
    ATT_CHAR_DEVICE_NAME                        = 0x2A00,
    /// Appearance
    ATT_CHAR_APPEARANCE                         = 0x2A01,
    /// Privacy flag
    ATT_CHAR_PRIVACY_FLAG                       = 0x2A02,
    /// Reconnection address
    ATT_CHAR_RECONNECTION_ADDR                  = 0x2A03,
    /// Peripheral preferred connection parameters
    ATT_CHAR_PERIPH_PREF_CON_PARAM              = 0x2A04,
    /// Service handles changed
    ATT_CHAR_SERVICE_CHANGED                    = 0x2A05,
    /// Alert Level characteristic
    ATT_CHAR_ALERT_LEVEL                        = 0x2A06,
    /// Tx Power Level
    ATT_CHAR_TX_POWER_LEVEL                     = 0x2A07,
    /// Date Time
    ATT_CHAR_DATE_TIME                          = 0x2A08,
    /// Day of Week
    ATT_CHAR_DAY_WEEK                           = 0x2A09,
    /// Day Date Time
    ATT_CHAR_DAY_DATE_TIME                      = 0x2A0A,
    /// Exact time 256
    ATT_CHAR_EXACT_TIME_256                     = 0x2A0C,
    /// DST Offset
    ATT_CHAR_DST_OFFSET                         = 0x2A0D,
    /// Time zone
    ATT_CHAR_TIME_ZONE                          = 0x2A0E,
    /// Local time Information
    ATT_CHAR_LOCAL_TIME_INFO                    = 0x2A0F,
    /// Time with DST
    ATT_CHAR_TIME_WITH_DST                      = 0x2A11,
    /// Time Accuracy
    ATT_CHAR_TIME_ACCURACY                      = 0x2A12,
    ///Time Source
    ATT_CHAR_TIME_SOURCE                        = 0x2A13,
    /// Reference Time Information
    ATT_CHAR_REFERENCE_TIME_INFO                = 0x2A14,
    /// Time Update Control Point
    ATT_CHAR_TIME_UPDATE_CNTL_POINT             = 0x2A16,
    /// Time Update State
    ATT_CHAR_TIME_UPDATE_STATE                  = 0x2A17,
    /// Glucose Measurement
    ATT_CHAR_GLUCOSE_MEAS                       = 0x2A18,
    /// Battery Level
    ATT_CHAR_BATTERY_LEVEL                      = 0x2A19,
    /// Temperature Measurement
    ATT_CHAR_TEMPERATURE_MEAS                   = 0x2A1C,
    /// Temperature Type
    ATT_CHAR_TEMPERATURE_TYPE                   = 0x2A1D,
    /// Intermediate Temperature
    ATT_CHAR_INTERMED_TEMPERATURE               = 0x2A1E,
    /// Measurement Interval
    ATT_CHAR_MEAS_INTERVAL                      = 0x2A21,
    /// Boot Keyboard Input Report
    ATT_CHAR_BOOT_KB_IN_REPORT                  = 0x2A22,
    /// System ID
    ATT_CHAR_SYS_ID                             = 0x2A23,
    /// Model Number String
    ATT_CHAR_MODEL_NB                           = 0x2A24,
    /// Serial Number String
    ATT_CHAR_SERIAL_NB                          = 0x2A25,
    /// Firmware Revision String
    ATT_CHAR_FW_REV                             = 0x2A26,
    /// Hardware revision String
    ATT_CHAR_HW_REV                             = 0x2A27,
    /// Software Revision String
    ATT_CHAR_SW_REV                             = 0x2A28,
    /// Manufacturer Name String
    ATT_CHAR_MANUF_NAME                         = 0x2A29,
    /// IEEE Regulatory Certification Data List
    ATT_CHAR_IEEE_CERTIF                        = 0x2A2A,
    /// CT Time
    ATT_CHAR_CT_TIME                            = 0x2A2B,
    /// Scan Refresh
    ATT_CHAR_SCAN_REFRESH                       = 0x2A31,
    /// Boot Keyboard Output Report
    ATT_CHAR_BOOT_KB_OUT_REPORT                 = 0x2A32,
    /// Boot Mouse Input Report
    ATT_CHAR_BOOT_MOUSE_IN_REPORT               = 0x2A33,
    /// Glucose Measurement Context
    ATT_CHAR_GLUCOSE_MEAS_CTX                   = 0x2A34,
    /// Blood Pressure Measurement
    ATT_CHAR_BLOOD_PRESSURE_MEAS                = 0x2A35,
    /// Intermediate Cuff Pressure
    ATT_CHAR_INTERMEDIATE_CUFF_PRESSURE         = 0x2A36,
    /// Heart Rate Measurement
    ATT_CHAR_HEART_RATE_MEAS                    = 0x2A37,
    /// Body Sensor Location
    ATT_CHAR_BODY_SENSOR_LOCATION               = 0x2A38,
    /// Heart Rate Control Point
    ATT_CHAR_HEART_RATE_CNTL_POINT              = 0x2A39,
    /// Alert Status
    ATT_CHAR_ALERT_STATUS                       = 0x2A3F,
    /// Ringer Control Point
    ATT_CHAR_RINGER_CNTL_POINT                  = 0x2A40,
    /// Ringer Setting
    ATT_CHAR_RINGER_SETTING                     = 0x2A41,
    /// Alert Category ID Bit Mask
    ATT_CHAR_ALERT_CAT_ID_BIT_MASK              = 0x2A42,
    /// Alert Category ID
    ATT_CHAR_ALERT_CAT_ID                       = 0x2A43,
    /// Alert Notification Control Point
    ATT_CHAR_ALERT_NTF_CTNL_PT                  = 0x2A44,
    /// Unread Alert Status
    ATT_CHAR_UNREAD_ALERT_STATUS                = 0x2A45,
    /// New Alert
    ATT_CHAR_NEW_ALERT                          = 0x2A46,
    /// Supported New Alert Category
    ATT_CHAR_SUP_NEW_ALERT_CAT                  = 0x2A47,
    /// Supported Unread Alert Category
    ATT_CHAR_SUP_UNREAD_ALERT_CAT               = 0x2A48,
    /// Blood Pressure Feature
    ATT_CHAR_BLOOD_PRESSURE_FEATURE             = 0x2A49,
    /// HID Information
    ATT_CHAR_HID_INFO                           = 0x2A4A,
    /// Report Map
    ATT_CHAR_REPORT_MAP                         = 0x2A4B,
    /// HID Control Point
    ATT_CHAR_HID_CTNL_PT                        = 0x2A4C,
    /// Report
    ATT_CHAR_REPORT                             = 0x2A4D,
    /// Protocol Mode
    ATT_CHAR_PROTOCOL_MODE                      = 0x2A4E,
    /// Scan Interval Window
    ATT_CHAR_SCAN_INTV_WD                       = 0x2A4F,
    /// PnP ID
    ATT_CHAR_PNP_ID                             = 0x2A50,
    /// Glucose Feature
    ATT_CHAR_GLUCOSE_FEATURE                    = 0x2A51,
    /// Record access control point
    ATT_CHAR_REC_ACCESS_CTRL_PT                 = 0x2A52,
    /// RSC Measurement
    ATT_CHAR_RSC_MEAS                           = 0x2A53,
    /// RSC Feature
    ATT_CHAR_RSC_FEAT                           = 0x2A54,
    /// SC Control Point
    ATT_CHAR_SC_CNTL_PT                         = 0x2A55,
    /// CSC Measurement
    ATT_CHAR_CSC_MEAS                           = 0x2A5B,
    /// CSC Feature
    ATT_CHAR_CSC_FEAT                           = 0x2A5C,
    /// Sensor Location
    ATT_CHAR_SENSOR_LOC                         = 0x2A5D,

    /* Test UUIDs */
    /// Manufacturer Service
    ATT_SVC_MANUF                               = 0xB000,
    /// Last define
    ATT_LAST
};

#define ATTS_ATT_16(type) {(uint8_t)((type)), (uint8_t)((type) >> 8)}

#define ATTS_ATT_32(type) {(uint8_t)((type)), (uint8_t)((type) >> 8),\
        						(uint8_t)((type)>>16), (uint8_t)((type) >> 24)}

#define ATTS_CHAR_16(prop, handle, type) {(prop),                                           \
                                       (uint8_t)((handle)), (uint8_t)((handle) >> 8),  \
                                       (uint8_t)((type)), (uint8_t)((type) >> 8)}

#define ATTS_CHAR_32(prop, handle, type) {(prop),                                           \
                                       (uint8_t)((handle)), (uint8_t)((handle) >> 8),  \
                                       (uint8_t)((type)), (uint8_t)((type) >> 8),\
                                       (uint8_t)((type)>>16), (uint8_t)((type) >> 24)}


/// Length, number, offset defines
#define ATT_SVC_VALUE_MAX_LEN                   0x0030
#define ATT_CHAR_NAME_MAX_LEN                   0x0030
#define ATT_UUID_16_LEN                         0x0002
#define ATT_UUID_32_LEN                         0x0004
#define ATT_UUID_128_LEN                        0x0010
#define ATT_MAX_NUM_EXT_PROFILE                 0x000A
#define ATT_MAX_NUM_INCL_PROFILE                0x000A
/// offset - l2cap header and ATT code
#define ATT_PDU_DATA_OFFSET                     0x05
/// Characteristic Properties Bit
#define ATT_CHAR_PROP_BCAST                     0x01
#define ATT_CHAR_PROP_RD                        0x02
#define ATT_CHAR_PROP_WR_NO_RESP                0x04
#define ATT_CHAR_PROP_WR                        0x08
#define ATT_CHAR_PROP_NTF                       0x10
#define ATT_CHAR_PROP_IND                       0x20
#define ATT_CHAR_PROP_AUTH                      0x40
#define ATT_CHAR_PROP_EXT_PROP                  0x80
/// Invalid Attribute Handle
#define ATT_INVALID_SEARCH_HANDLE               0x0000
#define ATT_INVALID_HANDLE                      0x0000
/// Read Information Request
#define ATT_UUID_FILTER_0                       0x00
#define ATT_UUID_FILTER_2                       0x02
#define ATT_UUID_FILTER_16                      0x10
/// Read Information Response
#define ATT_FORMAT_16BIT_UUID                   0x01
#define ATT_FORMAT_128BIT_UUID                  0x02
/// For No fix length PDU
#define ATT_CODE_LEN                            0x0001
#define ATT_CODE_AND_DATA_LEN                   0x0002
#define ATT_CODE_AND_HANDLE_LEN                 0x0003
#define ATT_CODE_AND_HANDLE_LEN_AND_OFFSET      0x0005
#define ATT_SIGNATURE_LEN                       0x0C

/// extended characteristics
#define ATT_EXT_RELIABLE_WRITE                  0x0001
#define ATT_EXT_WRITABLE_AUX                    0x0002
#define ATT_EXT_RFU                             0xFFFC

/// Least acceptable MTU value wrt BT Core Specs
#define ATT_LEAST_ACCEPTABLE_MTU                0x0017

/// Long value length 512 bytes
#define ATTM_MAX_LONG_VALUE                     0x0200

/// Maximum number of reliable writes for long char
#define ATTM_MAX_RELIABLE_WRITE                      ((ATTM_MAX_LONG_VALUE/(ATT_DEFAULT_MTU-5))+1)

/// Maximum content of reliable write
#define ATTM_RELIABLE_WRITE                          0x10

#endif /* ATTS_TASK_H_INCLUDED */
