/**
 * @file tal.h
 *
 * @brief This file contains TAL API function declarations
 *
 * $Id: tal.h 27079 2011-06-08 05:44:08Z pawan.jadia $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

/* Prevent double inclusion */
#ifndef TAL_H
#define TAL_H

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "tal_config.h"
#include "bmm.h"
#include "stack_config.h"
#include "return_val.h"
#include "tal_types.h"
#include "mac_build_config.h"

/* === EXTERNALS =========================================================== */

/* PIB values stored in TAL */
/**
 * CCA Mode
 */
extern uint8_t tal_pib_CCAMode;

/**
 * Current RF channel to be used for all transmissions and receptions.
 */
extern uint8_t tal_pib_CurrentChannel;

/**
 * Supported channels
 */
extern uint32_t tal_pib_SupportedChannels;

/**
 * 64-bit (IEEE) address of the node.
 */
extern uint64_t tal_pib_IeeeAddress;

/**
 * The maximum number of back-offs the CSMA-CA algorithm will attempt
 * before declaring a CSMA_CA failure.
 */
extern uint8_t tal_pib_MaxCSMABackoffs;

/**
 * The minimum value of the backoff exponent BE in the CSMA-CA algorithm.
 */
extern uint8_t tal_pib_MinBE;

/**
 * 16-bit PAN ID
 */
extern uint16_t tal_pib_PANId;

/**
 * Indicates if the node is a PAN coordinator or not.
 */
extern bool tal_pib_PrivatePanCoordinator;

/**
 * 16-bit short address of the node.
 */
extern uint16_t tal_pib_ShortAddress;

/**
 * Default value of transmit power of transceiver
 * using IEEE defined format of phyTransmitPower.
 */
extern uint8_t tal_pib_TransmitPower;

#if defined(BEACON_SUPPORT)
/**
 * Indication of whether battery life extension is enabled or not.
 */
extern bool tal_pib_BattLifeExt;

/**
 * Beacon order
 */
extern uint8_t tal_pib_BeaconOrder;

/**
 * Superframe order
 */
extern uint8_t tal_pib_SuperFrameOrder;

/**
 * Holds the time at which last beacon was transmitted or received.
 */
extern uint32_t tal_pib_BeaconTxTime;
#endif  /* BEACON_SUPPORT */

/**
 * Current channel page.
 */
extern uint8_t tal_pib_CurrentPage;

/**
 * Maximum number of symbols in a frame:
 * = phySHRDuration + ceiling([aMaxPHYPacketSize + 1] x phySymbolsPerOctet)
 */
extern uint16_t tal_pib_MaxFrameDuration;

/**
 * Duration of the synchronization header (SHR) in symbols for the current PHY.
 */
extern uint8_t tal_pib_SHRDuration;

/**
 * Number of symbols per octet for the current PHY.
 */
extern uint8_t tal_pib_SymbolsPerOctet;

/**
 * The maximum value of the backoff exponent BE in the CSMA-CA algorithm.
 */
extern uint8_t tal_pib_MaxBE;

/**
 * The maximum number of retries allowed after a transmission failure.
 */
extern uint8_t tal_pib_MaxFrameRetries;

#if defined(PROMISCUOUS_MODE)
/**
 * Promiscuous Mode
 */
extern bool tal_pib_PromiscuousMode;
#endif


/* === TYPES =============================================================== */

/**
 * MAC Message types
 */
typedef enum
{
/* MAC Command Frames (table 67) */
    /* Command Frame Identifier for Association Request */
    ASSOCIATIONREQUEST          = (0x01),
    /* Command Frame Identifier for Association Response */
    ASSOCIATIONRESPONSE,
    /* Command Frame Identifier for Disassociation Notification */
    DISASSOCIATIONNOTIFICATION,
    /* Command Frame Identifier for Data Request */
    DATAREQUEST,
    /* Command Frame Identifier for PANID Conflict Notification */
    PANIDCONFLICTNOTIFICAION,
    /* Command Frame Identifier for Orphan Notification */
    ORPHANNOTIFICATION,
    /* Command Frame Identifier for Beacon Request */
    BEACONREQUEST,
    /* Command Frame Identifier for Coordinator Realignment */
    COORDINATORREALIGNMENT,

/*
 * These are not MAC command frames but listed here as they are needed
 * in the msgtype field
 */
    /* Message is a directed orphan realignment command */
    ORPHANREALIGNMENT,
    /* Message is a beacon frame (in response to a beacon request cmd) */
    BEACON_MESSAGE,
    /* Message type field value for implicite poll without request */
    DATAREQUEST_IMPL_POLL,
    /* Message type field value for Null frame */
    NULL_FRAME,
    /* Message type field value for MCPS message */
    MCPS_MESSAGE
#ifdef ENABLE_RTB
    ,
    /* RTB Message Types */
    /* Message type field value for RTB Range Request frame */
    RTB_CMD_RANGE_REQ,
    /* Message type field value for RTB Range Accept frame */
    RTB_CMD_RANGE_ACPT,
    /* Message type field value for RTB PMU Time Sync Request frame */
    RTB_CMD_PMU_TIME_SYNC_REQ,
    /* Message type field value for RTB PMU Start frame */
    RTB_CMD_PMU_PMU_START,
    /* Message type field value for RTB Result Request frame */
    RTB_CMD_RESULT_REQ,
    /* Message type field value for RTB Result Confirm frame */
    RTB_CMD_RESULT_CONF,
    /* Message type field value for RTB Remote Range Request frame */
    RTB_CMD_REMOTE_RANGE_REQ,
    /* Message type field value for RTB Remote Range Confirm frame */
    RTB_CMD_REMOTE_RANGE_CONF
#endif  /* ENABLE_RTB */
} SHORTENUM frame_msgtype_t;



/**
 * @brief Globally used frame information structure
 *
 * @ingroup apiMacTypes
 */
typedef struct
#if !defined(DOXYGEN)
frame_info_tag
#endif
{
/** Message type of frame */
    frame_msgtype_t msg_type;
/** Pointer to buffer header of frame */
    buffer_t *buffer_header;
/** MSDU handle */
    uint8_t msduHandle;
/** Indirect frame persistence time */
    uint16_t persistence_time;
#if (MAC_INDIRECT_DATA_FFD == 1)
/** Indirect frame transmission ongoing */
    bool indirect_in_transit;
#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
/** Timestamp information of frame
  * The timestamping is only required for beaconing networks
  * or if timestamping is explicitly enabled.
  */
    uint32_t time_stamp;
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */
/** Pointer to MPDU */
    uint8_t *mpdu;
} frame_info_t;


/**
 * Sleep Mode supported by transceiver
 */
typedef enum sleep_mode_tag
{
    SLEEP_MODE_1
} SHORTENUM sleep_mode_t;


/**
 * CSMA Mode supported by transceiver
 */
typedef enum csma_mode_tag
{
    NO_CSMA_NO_IFS,
    NO_CSMA_WITH_IFS,
    CSMA_UNSLOTTED,
    CSMA_SLOTTED
} csma_mode_t;


/**
 * PIB attribute value type
 */
typedef union
{
/** PIB Attribute Bool */
    bool pib_value_bool;
/** PIB Attribute 8-bit */
    uint8_t pib_value_8bit;
/** PIB Attribute 16-bit */
    uint16_t pib_value_16bit;
/** PIB Attribute 32-bit */
    uint32_t pib_value_32bit;
/** PIB Attribute 64-bit */
    uint64_t pib_value_64bit;
} pib_value_t;

/* === MACROS ============================================================== */

/* RF bands: */
/**
 * 868 / 910 MHz (channels 0 through 10)
 * using BPSK
 */
#define BAND_900                            (0)

/**
 * 2.4 GHz (channels 11 through 26)
 */
#define BAND_2400                           (1)

#if (TAL_TYPE == AT86RF230A) || (TAL_TYPE == AT86RF230B) || (TAL_TYPE == AT86RF231) ||\
    (TAL_TYPE == ATMEGARF_TAL_1) || (TAL_TYPE == AT86RF232) || (TAL_TYPE == AT86RF233)
#define RF_BAND                             BAND_2400
#elif (TAL_TYPE == AT86RF212)
#define RF_BAND                             BAND_900
#else
#error "Missing RF_BAND define"
#endif

#if (RF_BAND == BAND_2400)
/**
 * 4 bits form one symbol since O-QPSK is used
 */
/* Symbols per octet */
#define SYMBOLS_PER_OCTET                   (2)
/* Number of symbols included in the preamble */
#define NO_SYMBOLS_PREAMBLE                 (8)
/* Number of symbols included in the SFD field */
#define NO_SYMBOLS_SFD                      (2)

#elif (RF_BAND == BAND_900)
/**
 * Depending on the channel page either
 * 1 bit forms one symbol (BPSK in channel page 0) or
 * 4 bit form one symbol (O-QPSK in channel page 2).
 */
/**
 * Symbols per octet
 */
#define SYMBOLS_PER_OCTET                   (tal_pib_CurrentPage == 0 ? 8 : 2)
/**
 * Number of symbols included in the preamble
 */
#define NO_SYMBOLS_PREAMBLE                 (tal_pib_CurrentPage == 0 ? 32 : 8)
/**
 * Number of symbols included in the SFD field
 */
#define NO_SYMBOLS_SFD                      (tal_pib_CurrentPage == 0 ? 8 : 2)
#else   /* "MAC-2003" */
/**
 * 1 bit forms one symbol since BPSK is used
 */
/* Symbols per octet */
#define SYMBOLS_PER_OCTET                   (8)
/* Number of symbols included in the preamble */
#define NO_SYMBOLS_PREAMBLE                 (32)
/* Number of symbols included in the SFD field */
#define NO_SYMBOLS_SFD                      (8)

#error "Unsupported RF band"
#endif

/**
 * Number of symbols forming the synchronization header (SHR) for the current PHY.
 * This value is the base for the PHY PIB attribute phySHRDuration.
 */
#define NO_OF_SYMBOLS_PREAMBLE_SFD          (NO_SYMBOLS_PREAMBLE + NO_SYMBOLS_SFD)


/**
 * Maximum number of symbols in a frame for the current PHY.
 * This value is the base for the PHY PIB attribute phyMaxFrameDuration.
 */
#define MAX_FRAME_DURATION                  \
            (NO_OF_SYMBOLS_PREAMBLE_SFD + (aMaxPHYPacketSize + 1) * SYMBOLS_PER_OCTET)

/**
 * The maximum time in symbols for a 32 bit timer
 */
#define MAX_SYMBOL_TIME                     (0x0FFFFFFF)

/**
 * Symbol mask for ingnoring most significant nibble
 */
#define SYMBOL_MASK                         (0x0FFFFFFF)

/* Custom attribute used by TAL */

/**
 * Attribute id of mac_i_pan_coordinator PIB
 */
#define mac_i_pan_coordinator               (0x0B)

/**
 * Conversion of symbols to microseconds
 */
#if (RF_BAND == BAND_2400)
    #define TAL_CONVERT_SYMBOLS_TO_US(symbols)      ((uint32_t)(symbols) << 4)
#else   /* (RF_BAND == BAND_900) */
    #define TAL_CONVERT_SYMBOLS_TO_US(symbols)                                                        \
        (tal_pib_CurrentPage == 0 ?                                                                   \
            (tal_pib_CurrentChannel == 0 ? ((uint32_t)(symbols) * 50) : ((uint32_t)(symbols) * 25)) : \
            (tal_pib_CurrentChannel == 0 ? ((uint32_t)(symbols) * 40) : ((uint32_t)(symbols) << 4))   \
        )
#endif  /* #if (RF_BAND == BAND_2400) */

/**
 * Conversion of microseconds to symbols
 */
#if (RF_BAND == BAND_2400)
    #define TAL_CONVERT_US_TO_SYMBOLS(time)         ((time) >> 4)
#else   /* (RF_BAND == BAND_900) */
    #define TAL_CONVERT_US_TO_SYMBOLS(time)                                 \
        (tal_pib_CurrentPage == 0 ?                                         \
            (tal_pib_CurrentChannel == 0 ? ((time) / 50) : ((time) / 25)) : \
            (tal_pib_CurrentChannel == 0 ? ((time) / 40) : ((time) >> 4))   \
        )
#endif  /* #if (RF_BAND == BAND_2400) */

/*
 * Beacon Interval formula: BI = aBaseSuperframeDuration 2^BO\f$0
 * where \f$0 <= BO <= 14. Note: Beacon interval calculated is in
 * symbols.
 */
/**
 * Beacon Interval time in symbols
 */
#define TAL_GET_BEACON_INTERVAL_TIME(BO) \
        ((1UL * aBaseSuperframeDuration) << (BO))

/*
 * Superframe Duration formula: \f$BI = aBaseSuperframeDuration 2^SO\f$
 * where \f$0 <= SO <= BO\f$
 */
/**
 * Superframe Duration time in symbols
 */
#define TAL_GET_SUPERFRAME_DURATION_TIME(SO) \
        ((1UL * aBaseSuperframeDuration) << (SO))


/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TAL task handling
 *
 * This function
 * - Checks and allocates the receive buffer.
 * - Processes the TAL incoming frame queue.
 * - Implements the TAL state machine.
 * @ingroup apiTalApi
 */
void tal_task(void);

/**
 * @brief Initializes the TAL
 *
 * This function is called to initialize the TAL. The transceiver is
 * initialized, the TAL PIBs are set to their default values, and the TAL state
 * machine is set to TAL_IDLE state.
 *
 * @return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF and the
 *                 current device part number and version number are correct;
 *         FAILURE otherwise
 * @ingroup apiTalApi
 */
retval_t tal_init(void);

/**
 * @brief Resets TAL state machine and sets the default PIB values if requested
 *
 * @param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 *
 * @return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF
 *         FAILURE otherwise
 * @ingroup apiTalApi
 */
retval_t tal_reset(bool set_default_pib);

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1) || defined(DOXYGEN)
/**
 * @brief Starts ED Scan
 *
 * This function starts an ED Scan for the scan duration specified by the
 * MAC layer.
 *
 * @param scan_duration Specifies the ED scan duration in symbols
 *
 * @return MAC_SUCCESS - ED scan duration timer started successfully
 *         TAL_BUSY - TAL is busy servicing the previous request from MAC
 *         TAL_TRX_ASLEEP - Transceiver is currently sleeping
 *         FAILURE otherwise
 * @ingroup apiTalApi
 */
retval_t tal_ed_start(uint8_t scan_duration);

/**
 * User call back function for finished ED Scan
 *
 * @param energy_level Measured energy level during ED Scan
 * @ingroup apiTalApi
 */
void tal_ed_end_cb(uint8_t energy_level);
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/**
 * @brief Gets a TAL PIB attribute
 *
 * This function is called to retrieve the transceiver information base
 * attributes.
 *
 * @param[in] attribute TAL infobase attribute ID
 * @param[out] value TAL infobase attribute value
 *
 * @return MAC_UNSUPPORTED_ATTRIBUTE if the TAL infobase attribute is not found
 *         MAC_SUCCESS otherwise
 * @ingroup apiTalApi
 */
retval_t tal_pib_get(uint8_t attribute, uint8_t *value);

/**
 * @brief Sets a TAL PIB attribute
 *
 * This function is called to set the transceiver information base
 * attributes.
 *
 * @param attribute TAL infobase attribute ID
 * @param value TAL infobase attribute value to be set
 *
 * @return MAC_UNSUPPORTED_ATTRIBUTE if the TAL info base attribute is not found
 *         TAL_BUSY if the TAL is not in TAL_IDLE state. An exception is
 *         macBeaconTxTime which can be accepted by TAL even if TAL is not
 *         in TAL_IDLE state.
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 *         TAL_TRX_ASLEEP if trx is in SLEEP mode and access to trx is required
 * @ingroup apiTalApi
 */
retval_t tal_pib_set(uint8_t attribute, pib_value_t *value);

/**
 * @brief Switches receiver on or off
 *
 * This function switches the receiver on (PHY_RX_ON) or off (PHY_TRX_OFF).
 *
 * @param state New state of receiver
 *
 * @return TAL_BUSY if the TAL state machine cannot switch receiver on or off,
 *         TRX_OFF if receiver has been switched off, or
 *         RX_ON otherwise.
 *
 * @ingroup apiTalApi
 */
uint8_t tal_rx_enable(uint8_t state);

/**
 * User call back function for frame reception
 *
 * @param rx_frame Pointer to received frame structure of type frame_info_t
 *                 or to received frame array
 * @ingroup apiTalApi
 */
void tal_rx_frame_cb(frame_info_t *rx_frame);

#ifdef ENABLE_RTB
/**
 * User call back function for frame reception in case RTB is used
 *
 * @param rx_frame Pointer to received frame structure of type frame_info_t
 *                 or to received frame array
 * @ingroup apiRtbApi
 */
void rtb_rx_frame_cb(frame_info_t *rx_frame);
#endif  /* ENABLE_RTB */

#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) || defined(DOXYGEN)
/**
 * @brief Beacon frame transmission
 *
 * @param tx_frame Pointer to the frame_info_t structure
 * @ingroup apiTalApi
 */
void tal_tx_beacon(frame_info_t *tx_frame);
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

/**
 * @brief Requests to TAL to transmit frame
 *
 * This function is called by the MAC to deliver a frame to the TAL
 * to be transmitted by the transceiver.
 *
 * @param tx_frame Pointer to the frame_info_t structure or
 *                 to frame array to be transmitted
 * @param csma_mode Indicates mode of csma-ca to be performed for this frame
 * @param perform_frame_retry Indicates whether to retries are to be performed for
 *                            this frame
 *
 * @return MAC_SUCCESS  if the TAL has accepted the data from the MAC for frame
 *                 transmission
 *         TAL_BUSY if the TAL is busy servicing the previous MAC request
 * @ingroup apiTalApi
 */
retval_t tal_tx_frame(frame_info_t *tx_frame, csma_mode_t csma_mode, bool perform_frame_retry);

/**
 * User call back function for frame transmission
 *
 * @param status Status of frame transmission attempt
 * @param frame Pointer to frame structure of type frame_info_t
 * @ingroup apiTalApi
 */
void tal_tx_frame_done_cb(retval_t status, frame_info_t *frame);

/**
 * @brief Sets the transceiver to sleep
 *
 * This function sets the transceiver to sleep state.
 *
 * @param mode Defines sleep mode of transceiver SLEEP or PHY_TRX_OFF)
 *
 * @return   TAL_BUSY - The transceiver is busy in TX or RX
 *           MAC_SUCCESS - The transceiver is put to sleep
 *           TAL_TRX_ASLEEP - The transceiver is already asleep
 *           MAC_INVALID_PARAMETER - The specified sleep mode is not supported
 * @ingroup apiTalApi
 */
retval_t tal_trx_sleep(sleep_mode_t mode);

/**
 * @brief Wakes up the transceiver from sleep
 *
 * This function awakes the transceiver from sleep state.
 *
 * @return   TAL_TRX_AWAKE - The transceiver is already awake
 *           MAC_SUCCESS - The transceiver is woken up from sleep
 *           FAILURE - The transceiver did not wake-up from sleep
 * @ingroup apiTalApi
 */
retval_t tal_trx_wakeup(void);

/**
 * @brief Generates a 16-bit random number used as initial seed for srand()
 *
 * This function generates a 16-bit random number by means of using the
 * Random Number Generator from the transceiver.
 * The Random Number Generator generates 2-bit random values. These 2-bit
 * random values are concatenated to the required 16-bit random seed.
 * The generated random 16-bit number is feed into function srand()
 * as initial seed.
 * The transceiver state is initally set to RX_ON.
 * After the completion of the random seed generation, the
 * trancseiver is set to TRX_OFF.
 *
 * @ingroup apiTalApi
 */
void tal_generate_rand_seed(void);

/**
 * @brief Adds two time values
 *
 * This function adds two time values
 *
 * @param a Time value 1
 * @param b Time value 2
 *
 * @return value of a + b
 * @ingroup apiTalApi
 */
static inline uint32_t tal_add_time_symbols(uint32_t a, uint32_t b)
{
    return ((a + b) & SYMBOL_MASK);
}


/**
 * @brief Subtract two time values
 *
 * This function subtracts two time values taking care of roll over.
 *
 * @param a Time value 1
 * @param b Time value 2
 *
 * @return value a - b
 * @ingroup apiTalApi
 */
static inline uint32_t tal_sub_time_symbols(uint32_t a, uint32_t b)
{
    if (a > b)
    {
        return ((a - b) & SYMBOL_MASK);
    }
    else
    {
        /* This is a roll over case */
        return (((MAX_SYMBOL_TIME - b) + a) & SYMBOL_MASK);
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_H */
/* EOF */
