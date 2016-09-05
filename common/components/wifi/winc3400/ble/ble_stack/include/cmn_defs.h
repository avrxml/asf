/**
 * \file
 *
 * \brief CMN.
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

#ifndef CMN_DEFS_H_INCLUDED
#define CMN_DEFS_H_INCLUDED

/// Builds the task identifier from the type and the index of that task.
#define KE_BUILD_ID(type, index) ( (uint16_t)(((index) << 8)|(type)) )

/// Retrieves task index number from task id.
#define KE_IDX_GET(NMI_Uint16) (((NMI_Uint16) >> 8) & 0xFF)

#define TASK_EXTERN 		TASK_GTL
#define API_PKT_ID			0x05
#define HDR_LEN				9

#define NMI_CHAR_MAX_LEN		(2)

#define KEY_LEN             0x10

/// Attribute access types
enum
{
    /// Read Access
    PERM_ACCESS_RD           = 0,
    /// Write Access
    PERM_ACCESS_WR           = 3,
    /// Indication Access
    PERM_ACCESS_IND          = 6,
    /// Notification Access
    PERM_ACCESS_NTF          = 9,
    /// Check Encryption key size
    PERM_ACCESS_EKS          = 12,
    /// Hide/Show attribute
    PERM_ACCESS_HIDE         = 13,
    /// Write Signed Enabled attribute
    PERM_ACCESS_WRITE_SIGNED = 14
};

/// Attribute access types mask
enum
 {
    /// Read Access Mask
    PERM_ACCESS_MASK_RD           = 0x0007,
    /// Write Access Mask
    PERM_ACCESS_MASK_WR           = 0x0038,
    /// Indication Access Mask
    PERM_ACCESS_MASK_IND          = 0x01C0,
    /// Notification Access Mask
    PERM_ACCESS_MASK_NTF          = 0x0E00,
    /// Check Encryption key size Mask
    PERM_ACCESS_MASK_EKS          = 0x1000,
    /// Hide/Show attribute Mask
    PERM_ACCESS_MASK_HIDE         = 0x2000,
    /// Write Signed Enabled attribute Mask
    PERM_ACCESS_MASK_WRITE_SIGNED = 0x4000
 };

/// Service access types
enum
{
    /// Service Access
    PERM_ACCESS_SVC      = 0,
    /// Check Encryption key size for service Access
    PERM_ACCESS_SVC_EKS  = 3,
    /// Hide/Show service Access
    PERM_ACCESS_SVC_HIDE = 4
};

/// Service access types mask
enum
 {
    /// Service Access Mask
    PERM_ACCESS_MASK_SVC       = 0x07,
    /// Check Encryption key size for service Access Mask
    PERM_ACCESS_MASK_SVC_EKS   = 0x08,
    /// Hide/Show service Access Mask
    PERM_ACCESS_MASK_SVC_HIDE  = 0x10
 };

/// Attribute & Service access rights
enum
{
    /// Disable access
    PERM_RIGHT_DISABLE  = 0,
    /// Enable access
    PERM_RIGHT_ENABLE   = 1,
    /// Access Requires Unauthenticated link
    PERM_RIGHT_UNAUTH   = 2,
    /// Access Requires Authenticated link
    PERM_RIGHT_AUTH     = 3,
    /// Access Requires authorization
    PERM_RIGHT_AUTHZ    = 4
};

/// Attribute & Service access rights mask
enum
{
    /// Disable access mask
    PERM_RIGHT_MASK_DISABLE  = 0x3,
    /// Enable access mask
    PERM_RIGHT_MASK_ENABLE   = 0x3,
    /// Access Requires Unauthenticated lin kmask
    PERM_RIGHT_MASK_UNAUTH   = 0x3,
    /// Access Requires Authenticated link mask
    PERM_RIGHT_MASK_AUTH     = 0x3,
    /// Access Requires authorization mask
    PERM_RIGHT_MASK_AUTHZ    = 0x4
};

#define TRUE 1
#define FALSE 0

#define PERM(access, right) \
    (((PERM_RIGHT_ ## right) << (PERM_ACCESS_ ## access)) & (PERM_ACCESS_MASK_ ## access))

/// Tasks types.
enum
{
    TASK_NONE = 0xFF,

    // BT Controller Tasks
    TASK_LM           = 56   ,
    TASK_LC           = 57   ,
    TASK_LB           = 58   ,
    TASK_LD           = 59   ,

    // Link Layer Tasks
    TASK_LLM          = 0   ,
    TASK_LLC          = 1   ,
    TASK_LLD          = 2   ,

    TASK_HCI          = 60  ,
    TASK_HCIH         = 61  ,

    TASK_DBG          = 3   ,

    TASK_L2CM         = 4   ,
    TASK_L2CC         = 5   ,
    TASK_SMPM         = 6   ,
    TASK_SMPC         = 7   ,
    TASK_ATTM         = 8   ,   // Attribute Protocol Manager Task
    TASK_ATTC         = 9   ,   // Attribute Protocol Client Task
    TASK_ATTS         = 10  ,   // Attribute Protocol Server Task
    TASK_GATTM        = 11  ,   // Generic Attribute Profile Manager Task
    TASK_GATTC        = 12  ,   // Generic Attribute Profile Controller Task
    TASK_GAPM         = 13  ,   // Generic Access Profile Manager
    TASK_GAPC         = 14  ,   // Generic Access Profile Controller
    TASK_PROXM        = 15  ,   // Proximity Monitor Task
    TASK_PROXR        = 16  ,   // Proximity Reporter Task
    TASK_FINDL        = 17  ,   // Find Me Locator Task
    TASK_FINDT        = 18  ,   // Find Me Target Task
    TASK_HTPC         = 19  ,   // Health Thermometer Collector Task
    TASK_HTPT         = 20  ,   // Health Thermometer Sensor Task
    TASK_ACCEL        = 21  ,   // Accelerometer Sensor Task
    TASK_BLPS         = 22  ,   // Blood Pressure Sensor Task
    TASK_BLPC         = 23  ,   // Blood Pressure Collector Task
    TASK_HRPS         = 24  ,   // Heart Rate Sensor Task
    TASK_HRPC         = 25  ,   // Heart Rate Collector Task
    TASK_TIPS         = 26  ,   // Time Server Task
    TASK_TIPC         = 27  ,   // Time Client Task
    TASK_DISS         = 28  ,   // Device Information Service Server Task
    TASK_DISC         = 29  ,   // Device Information Service Client Task
    TASK_SCPPS        = 30  ,   // Scan Parameter Profile Server Task
    TASK_SCPPC        = 31  ,   // Scan Parameter Profile Client Task
    TASK_BASS         = 32  ,   // Battery Service Server Task
    TASK_BASC         = 33  ,   // Battery Service Client Task
    TASK_HOGPD        = 34  ,   // HID Device Task
    TASK_HOGPBH       = 35  ,   // HID Boot Host Task
    TASK_HOGPRH       = 36  ,   // HID Report Host Task
    TASK_GLPS         = 37  ,   // Glucose Profile Sensor Task
    TASK_GLPC         = 38  ,   // Glucose Profile Collector Task
    TASK_NBPS         = 39  ,   // Nebulizer Profile Server Task
    TASK_NBPC         = 40  ,   // Nebulizer Profile Client Task
    TASK_RSCPS        = 41  ,   // Running Speed and Cadence Profile Server Task
    TASK_RSCPC        = 42  ,   // Running Speed and Cadence Profile Collector Task
    TASK_CSCPS        = 43  ,   // Cycling Speed and Cadence Profile Server Task
    TASK_CSCPC        = 44  ,   // Cycling Speed and Cadence Profile Client Task
    TASK_ANPS         = 45  ,   // Alert Notification Profile Server Task
    TASK_ANPC         = 46  ,   // Alert Notification Profile Client Task
    TASK_PASPS        = 47  ,   // Phone Alert Status Profile Server Task
    TASK_PASPC        = 48  ,   // Phone Alert Status Profile Client Task

    TASK_CPPS         = 49  ,   // Cycling Power Profile Server Task
    TASK_CPPC         = 50  ,   // Cycling Power Profile Client Task

    TASK_DISPLAY      = 51  ,
    TASK_APP          = 52  ,

    TASK_LANS         = 53  ,   // Location and Navigation Profile Server Task
    TASK_LANC         = 54  ,   // Location and Navigation Profile Client Task

    TASK_WIFIPROV     = 55  ,   // WiFi Provisioning Profile Module Task

    TASK_GTL          = 63  ,
    TASK_MAX          = 64
};


///Advertising filter policy
enum adv_filter_policy
{
    ///Allow both scan and connection requests from anyone
    ADV_ALLOW_SCAN_ANY_CON_ANY    = 0x00,
    ///Allow both scan req from White List devices only and connection req from anyone
    ADV_ALLOW_SCAN_WLST_CON_ANY,
    ///Allow both scan req from anyone and connection req from White List devices only
    ADV_ALLOW_SCAN_ANY_CON_WLST,
    ///Allow scan and connection requests from White List devices only
    ADV_ALLOW_SCAN_WLST_CON_WLST,
    ///Enumeration end value for advertising filter policy value check
    ADV_ALLOW_SCAN_END
};

#define ATT_SIGNATURE_LEN                       0x0C

/// Length, number, offset defines
#define ATT_SVC_VALUE_MAX_LEN                   0x0030
#define ATT_CHAR_NAME_MAX_LEN                   0x0030
#define ATT_UUID_16_LEN                         0x0002
#define ATT_UUID_32_LEN                         0x0004
#define ATT_UUID_128_LEN                        0x0010
#define ATT_MAX_NUM_EXT_PROFILE                 0x000A
#define ATT_MAX_NUM_INCL_PROFILE                0x000A

/// Maximum Transmission Unit
#define ATT_DEFAULT_MTU                                 0x0017

/// Maximum value length
#define ATTM_MAX_VALUE                               (ATT_DEFAULT_MTU+1)


/// Maximum content of reliable write
#define ATTM_RELIABLE_WRITE                          0x10

/// Nb of handles for multiple handle operation
#define ATT_NB_MULT_HDLS                                0x0008

/// desired UUID
struct att_desired_type
{
    /// Size of the UUID
    uint16_t value_size;
    /// actual UUID
    uint8_t value[ATT_UUID_128_LEN];
};


/// UUID with different length Structure
struct att_uuid_type
{
    /// Size of the UUID
	uint8_t value_size;
    /// expected response size - read multiple
	uint8_t expect_resp_size;
    /// actual UUID
	uint8_t value[ATT_UUID_128_LEN];
};

/// Attribute data holder
struct att_info_data
{
    /// data length
	uint16_t len;
    /// each result length
    uint8_t each_len;
    /// data
    uint8_t data[1];
};

#endif /* CMN_DEFS_H_INCLUDED */
