/**
 * @file zrc.h
 *
 * @brief API for ZRC profile includes cmd discovery and RC commands
 *
 * $Id: zrc.h 27820 2011-07-24 17:20:51Z pawan.jadia $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

#if ((defined ZRC_PROFILE) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef ZRC_H
#define ZRC_H


/* === Includes ============================================================= */

#include "rf4ce.h"

/* === Macros =============================================================== */

/**
 * ZRC profile constants
 *
 * @ingroup apiRF4CONTROL_ZRC_CONSTANTS
 */
/** Controller discovery duration */
#define DISCOVERY_DURATION                  0x00186A

/** The automatic discovery response mode duration shall be set to 0x1c9c38 (30s). */
#define AUTO_DISC_DURATION_SYM              0x1C9C38

/** Default value for the discovery repetition interval */
#define DISCOVERY_REPETITION_INTERVAL       0x00F424

/** Default value for maximum number of times the discovery request command is sent */
#define MAX_DISCOVERY_REPETITIONS           0x1E

/** Default value for Number of reported node descriptor received during discovery process */
#define MAX_REPORTED_NODE_DESCRIPTORS       1

/** The maximum time between consecutive user control repeated command frame
 * transmissions. */
#define aplcMaxKeyRepeatInterval_ms         100 /* ms */
/** Default value of the key repeat interval */
#define aplKeyRepeatInterval_def            (aplcMaxKeyRepeatInterval_ms)
/** Default value of the key repeat wait time */
#define aplKeyRepeatWaitTime_def            (2 * aplcMaxKeyRepeatInterval_ms)
/** The maximum duration that the receiver is enabled on a controller after
   pairing to receive any command discovery request command frames. */
#define aplcMaxCmdDiscRxOnDuration          200 /* ms */
/** The maximum amount of time a device waits after receiving a successful
   NLME-AUTO-DISCOVERY.confirm primitive for a pair indication to arrive from
   the pairing initiator. Unit is seconds. */
#define aplcMaxPairIndicationWaitTime       1.2 /* second */
/** The maximum amount of time a device waits after receiving a successful
   NLME-AUTO-DISCOVERY.confirm primitive for a pair indication to arrive from
   the pairing initiator. Unit is micro seconds. */
#define aplcMaxPairIndicationWaitTime_us    (aplcMaxPairIndicationWaitTime * 1000000L)
/** The maximum time a device shall wait for a response command frame following
   a request command frame. Unit is milli seconds. */
#define aplcMaxResponseWaitTime             200 /* ms */
/** The maximum time a device shall wait for a response command frame following
   a request command frame. Unit is micro seconds. */
#define aplcMaxResponseWaitTime_us          (aplcMaxResponseWaitTime * 1000L)
/** The minimum value of the KeyExTransferCount parameter passed to the pair
   request primitive during the push button pairing procedure. */
#define aplcMinKeyExchangeTransferCount     3
/** Default value of the key exchange transfer count. */
#define aplcKeyExchangeTransferCount_def    aplcMinKeyExchangeTransferCount
/** The minimum amount of time a device must wait after a successful pairing
   attempt with a target before attempting command discovery. Unit is milli seconds. */
#define aplcMinTargetBlackoutPeriod         100 /* ms */
/** The minimum amount of time a device must wait after a successful pairing
   attempt with a target before attempting command discovery. Unit is micro seconds. */
#define aplcMinTargetBlackoutPeriod_us      (aplcMinTargetBlackoutPeriod * 1000L)

/* === Types ================================================================ */

/**
 * Enums used for ZRC command codes
 *
 * @ingroup apiRF4CONTROL_ZRC_CONSTANTS
 */
typedef enum zrc_cmd_code_tag
{
    /* Reserved 0x00 */
    USER_CONTROL_IDLE          = 0x00, /* Internal use; state machine handling */
    USER_CONTROL_PRESSED       = 0x01,
    USER_CONTROL_REPEATED      = 0x02,
    USER_CONTROL_RELEASED      = 0x03,
    CMD_DISCOVERY_REQUEST      = 0x04,
    CMD_DISCOVERY_RESPONSE     = 0x05
    /* 0x06 - 0x1f Reserved */
} SHORTENUM zrc_cmd_code_t;

/**
  * Enums used for the RC command codes / CEC commands from HDMI spec.
  * (See Annex A: Mandatory command matrix)
  *
  * @ingroup apiRF4CONTROL_ZRC_CONSTANTS
  */
typedef enum cec_code_tag
{
    SELECT                      = 0x00,
    UP                          = 0x01,
    DOWN                        = 0x02,
    LEFT                        = 0x03,
    RIGHT                       = 0x04,
    RIGHT_UP                    = 0x05,
    RIGHT_DOWN                  = 0x06,
    LEFT_UP                     = 0x07,
    LEFT_DOWN                   = 0x08,
    ROOT_MENU                   = 0x09,
    SETUP_MENU                  = 0x0a,
    CONTENTS_MENU               = 0x0b,
    FAVORITE_MENU               = 0x0c,
    EXIT                        = 0x0d,
    /* 0x0e - 0x0f Reserved */
    MEDIA_TOP_MENU              = 0x10,
    MEDIA_CONTEXT_MENU          = 0x11,
    /* 0x12 - 0x1c Reserved */
    NUMBER_SHIFT                = 0x1d,
    NUMBER_11                   = 0x1e,
    NUMBER_12                   = 0x1f,
    NUMBER_0                    = 0x20,
    NUMBER_1                    = 0x21,
    NUMBER_2                    = 0x22,
    NUMBER_3                    = 0x23,
    NUMBER_4                    = 0x24,
    NUMBER_5                    = 0x25,
    NUMBER_6                    = 0x26,
    NUMBER_7                    = 0x27,
    NUMBER_8                    = 0x28,
    NUMBER_9                    = 0x29,
    DOT                         = 0x2a,
    ENTER                       = 0x2b,
    CLEAR                       = 0x2c,
    /* 0x2d - 0x2e Reserved */
    NEXT_FAVORITE               = 0x2f,
    CHANNEL_UP                  = 0x30,
    CHANNEL_DOWN                = 0x31,
    PREVIOUS_CHANNEL            = 0x32,
    SOUND_SELECT                = 0x33,
    INPUT_SELECT                = 0x34,
    DISPLAY_INFORMATION         = 0x35,
    HELP                        = 0x36,
    PAGE_UP                     = 0x37,
    PAGE_DOWN                   = 0x38,
    /* 0x39 - 0x3f Reserved */
    POWER                       = 0x40,
    VOLUME_UP                   = 0x41,
    VOLUME_DOWN                 = 0x42,
    MUTE                        = 0x43,
    PLAY                        = 0x44,
    STOP                        = 0x45,
    PAUSE                       = 0x46,
    RECORD                      = 0x47,
    REWIND                      = 0x48,
    FAST_FORWARD                = 0x49,
    EJECT                       = 0x4a,
    FORWARD                     = 0x4b,
    BACKWARD                    = 0x4c,
    STOP_RECORD                 = 0x4d,
    PAUSE_RECORD                = 0x4e,
    /* 0x4f Reserved */
    ANGLE                       = 0x50,
    SUB_PICTURE                 = 0x51,
    VIDEO_ON_DEMAND             = 0x52,
    ELECTRONIC_PROGRAM_GUIDE    = 0x53,
    TIMER_PROGRAMMING           = 0x54,
    INITIAL_CONFIGURATION       = 0x55,
    SELECT_BROADCAST_TYPE       = 0x56,
    SELECT_SOUND_PRESENTATION   = 0x57,
    /* 0x58 - 0x5f Reserved */
    PLAY_FUNCTION               = 0x60,
    PAUSE_PLAY_FUNCTION         = 0x61,
    RECORD_FUNCTION             = 0x62,
    PAUSE_RECORD_FUNCTION       = 0x63,
    STOP_FUNCTION               = 0x64,
    MUTE_FUNCTION               = 0x65,
    RESTORE_VOLUME_FUNCTION     = 0x66,
    TUNE_FUNCTION               = 0x67,
    SELECT_MEDIA_FUNCTION       = 0x68,
    SELECT_AV_INPUT_FUNCTION    = 0x69,
    SELECT_AUDIO_INPUT_FUNCTION = 0x6a,
    POWER_TOGGLE_FUNCTION       = 0x6b,
    POWER_OFF_FUNCTION          = 0x6c,
    POWER_ON_FUNCTION           = 0x6d,
    /* 0x6e - 0x70 Reserved */
    F1_BLUE                     = 0x71,
    F2_RED                      = 0x72,
    F3_GREEN                    = 0x73,
    F4_YELLOW                   = 0x74,
    F5                          = 0x75,
    DATA                        = 0x76
    /* 0x77 - 0xff Reserved */
} SHORTENUM cec_code_t;

/**
 * ZRC command frame structure
 */
typedef struct zrc_cmd_frm_tag
{
    uint8_t fcf; /**< frame control field */
    uint8_t rc_cmd; /**< remote control command */
    uint8_t payload; /**< actual payload */
} zrc_cmd_frm_t;

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

/**
 * @brief Prints RC command code text; used for example apps and debugging
 *
 * @param command   CEC command code
 *
 * @return  String describing the command
 *
 * @ingroup apiRF4Control_ZRC_API
 */
#if ((defined SIO_HUB) || (defined DOXYGEN))
char *zrc_print_rc_cmd_text(uint8_t command);
#endif

/* === Implementation ======================================================= */


/* Controller */


/**
 * @brief Initialites the command transmission from the application.
 *
 * @param PairingRef       Pairing reference
 * @param VendorId         Vendor Id information
 * @param CmdCode          ZRC cmd code (Press/Release)
 * @param CmdLength        Length of the rc cmd (rc cmd code + Payload)
 * @param Cmd              Pointer to payload data, i.e. RC command;
 *                         rc command code (rc cmd code + Payload)
 * @param TxOptions        Transmission options (Eg. ack, security etc)
 *
 * @return true            returns the status of zrc_cmd_request.
 *
 * @ingroup apiRF4Control_ZRC_API
 */
#if (!defined RF4CE_TARGET) || (defined DOXYGEN)
bool zrc_cmd_request(uint8_t PairingRef, uint16_t VendorId, zrc_cmd_code_t CmdCode,
                     uint8_t CmdLength, uint8_t *Cmd, uint8_t TxOptions);
#endif /* !defined RF4CE_TARGET */


/**
 * @brief Callback function to the application indicating the status the sent
 *        ZRC command request.
 *
 * @param Status           Status of the command request
 * @param PairingRef       Pairing reference
 * @param RcCmd            RC command code.
 *
 * @ingroup apiRF4Control_ZRC_API
 */
#if (!defined RF4CE_TARGET) || (defined DOXYGEN)
void zrc_cmd_confirm(nwk_enum_t Status, uint8_t PairingRef, cec_code_t RcCmd);
#endif /* !defined RF4CE_TARGET */


/**
 * @brief API indicating a received the command to ZRC application.
 *
 * @param PairingRef        Pairing reference of the source node
 * @param nsduLength        Length of the received data
 * @param nsdu              Pointer to payload data, i.e. RC command
 * @param RxLinkQuality     Link quality of received packet
 * @param RxFlags           Rx Flags.
 *
 * @ingroup apiRF4Control_ZRC_API
 */
#if (defined RF4CE_TARGET) || (defined ZRC_CMD_DISCOVERY) || (defined DOXYGEN)
void zrc_cmd_indication(uint8_t PairingRef, uint8_t nsduLength, uint8_t *nsdu,
                        uint8_t RxLinkQuality, uint8_t RxFlags);
#endif


/**
 * @brief Handles the cmd data indications received from the network layer.
 *       (cmd discovery and remote control commands).
 *
 * @param PairingRef        Pairing reference
 * @param nsduLength        Length of the received data
 * @param nsdu              Actual data
 * @param RxLinkQuality     Link quality of received packet
 * @param RxFlags           Rx Flags.
 */
void zrc_data_indication(uint8_t PairingRef, uint8_t nsduLength, uint8_t *nsdu,
                         uint8_t RxLinkQuality, uint8_t RxFlags);


/**
 * @brief Handle the data confirm (received from network layer).
 *
 * @param Status           Status received from the network layer.
 * @param PairingRef       Pairing reference
 */
void zrc_data_confirm(nwk_enum_t Status, uint8_t PairingRef);


/* --- Command discovery handling ------------------------------------------ */


/**
 * @brief Sends the ZRC command discovery request
 *
 * The command discovery request allows a node to query which user
 * control commands are supported on a remote node, i.e. those specified in the
 * user control pressed command frame.
 *
 * @param PairingRef Pairing reference of the device to address the request.
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else false
 *
 * @ingroup apiRF4Control_ZRC_CMD_DISC_API
 */
#if (defined ZRC_CMD_DISCOVERY) || (defined DOXYGEN)
bool zrc_cmd_disc_request(uint8_t PairingRef);
#endif


/**
 * @brief Callback ZRC command discovery confirm
 *
 * The command discovery confirm callback provides information about the sent
 * command discovery request.
 *
 * @param Status        Status of the sent command discovery request
 * @param PairingRef    Pairing reference used for the command discovery request
 * @param SupportedCmd  Pointer to 32 byte array containing the supported commands
 *
 * @ingroup apiRF4Control_ZRC_CMD_DISC_API
 */
#if (defined ZRC_CMD_DISCOVERY) || (defined DOXYGEN)
void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef, uint8_t *SupportedCmd);
#endif


/**
 * @brief Callback ZRC command discovery indication
 *
 * The command discovery indication callback indicates that a command discovery
 * request command was receiced.
 *
 * @param PairingRef    Pairing reference used for the command discovery request
 *
 * @ingroup apiRF4Control_ZRC_CMD_DISC_API
 */
#if (defined ZRC_CMD_DISCOVERY) || (defined DOXYGEN)
void zrc_cmd_disc_indication(uint8_t PairingRef);
#endif


/**
 * @brief ZRC command discovery response
 *
 * The command discovery response allows a device to answer to an incoming
 * command discovery request.
 *
 * @param PairingRef    Pairing reference used for the command discovery request
 * @param SupportedCmd  Pointer to 32 byte array containing supported CEC commands
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else false
 *
 * @ingroup apiRF4Control_ZRC_CMD_DISC_API
 */
#if (defined ZRC_CMD_DISCOVERY) || (defined DOXYGEN)
bool zrc_cmd_disc_response(uint8_t PairingRef, uint8_t *SupportedCmd);
#endif



#endif /* ZRC_H */

#endif  /* #if ((defined ZRC_PROFILE) || (defined DOXYGEN)) */

/* EOF */
