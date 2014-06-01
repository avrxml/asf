/**
 * @file tal.h
 *
 * @brief This file contains TAL API function declarations
 *
 * Copyright (c) 2013-2014-2014 Atmel Corporation. All rights reserved.
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
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
 */

/*
 * Copyright (c) 2013-2014, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_H
#define TAL_H

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "tal_config.h"
#include "tal_generic.h"
#include "bmm.h"
#include "stack_config.h"
#include "return_val.h"
#include "tal_types.h"
#include "mac_build_config.h"
#include "qmm.h"

/**
 * \defgroup group_tal  Transceiver Abstraction Layer
 * The Transceiver Abstraction Layer (TAL) implements the transceiver specific
 * functionalities and
 * provides interfaces to the upper layers (like IEEE 802.15.4 MAC )and  uses
 * the services of PAL.
 *
 */

/* === EXTERNALS =========================================================== */
__PACK__DATA__
/* Structure implementing the PIB values stored in TAL */
typedef struct tal_pib_tag {
	/**
	 * 64-bit (IEEE) address of the node.
	 */
	uint64_t IeeeAddress;

	/**
	 * Supported channels
	 */
	uint32_t SupportedChannels;

#if defined(BEACON_SUPPORT)

	/**
	 * Holds the time at which last beacon was transmitted or received.
	 */
	uint32_t BeaconTxTime;
#endif  /* BEACON_SUPPORT */

	/**
	 * 16-bit short address of the node.
	 */
	uint16_t ShortAddress;

	/**
	 * 16-bit PAN ID
	 */
	uint16_t PANId;

	/**
	 * Maximum number of symbols in a frame:
	 * = phySHRDuration + ceiling([aMaxPHYPacketSize + 1] x
	 * phySymbolsPerOctet)
	 */
	uint16_t MaxFrameDuration;

	/**
	 * CCA Mode
	 */
	uint8_t CCAMode;

	/**
	 * Current RF channel to be used for all transmissions and receptions.
	 */
	uint8_t CurrentChannel;

	/**
	 * The maximum number of back-offs the CSMA-CA algorithm will attempt
	 * before declaring a CSMA_CA failure.
	 */
	uint8_t MaxCSMABackoffs;

	/**
	 * The minimum value of the backoff exponent BE in the CSMA-CA
	 * algorithm.
	 */
	uint8_t MinBE;

	/**
	 * Indicates if the node is a PAN coordinator or not.
	 */
	bool PrivatePanCoordinator;

	/**
	 * Default value of transmit power of transceiver
	 * using IEEE defined format of phyTransmitPower.
	 */
	uint8_t TransmitPower;

#if defined(BEACON_SUPPORT)

	/**
	 * Indication of whether battery life extension is enabled or not.
	 */
	bool BattLifeExt;

	/**
	 * Beacon order
	 */
	uint8_t BeaconOrder;

	/**
	 * Superframe order
	 */
	uint8_t SuperFrameOrder;
#endif  /* BEACON_SUPPORT */

	/**
	 * Current channel page.
	 */
	uint8_t CurrentPage;

	/**
	 * Duration of the synchronization header (SHR) in symbols for the
	 * current PHY.
	 */
	uint8_t SHRDuration;

	/**
	 * Number of symbols per octet for the current PHY.
	 */
	uint8_t SymbolsPerOctet;

	/**
	 * The maximum value of the backoff exponent BE in the CSMA-CA
	 * algorithm.
	 */
	uint8_t MaxBE;

	/**
	 * The maximum number of retries allowed after a transmission failure.
	 */
	uint8_t MaxFrameRetries;

#if defined(PROMISCUOUS_MODE)

	/**
	 * Promiscuous Mode
	 */
	bool PromiscuousMode;
#endif
} tal_pib_t;

#if (defined SW_CONTROLLED_CSMA) && (defined TX_OCTET_COUNTER)

/**
 * Counter of transmitted octets
 */
extern uint32_t tal_tx_octet_cnt;
#endif

extern tal_pib_t tal_pib;

/* === TYPES =============================================================== */

/**
 * MAC Message types
 */
typedef enum {
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

	GTSREQUEST,

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
	RTB_CMD_REMOTE_RANGE_CONF,
	/* Message type field value for RTB Remote Range Band Results frame */
	RTB_CMD_REMOTE_RANGE_BAND_RESULTS
#endif  /* ENABLE_RTB */
} SHORTENUM frame_msgtype_t;

/**
 * \brief Globally used frame information structure
 *
 */
typedef struct
#if !defined(__DOXYGEN__)
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
	/** Indirect frame transmission ongoing */
	bool indirect_in_transit;
#ifdef MAC_SECURITY_ZIP
	/** MAC Payload Pointer */
	uint8_t *mac_payload;
#endif

#ifdef GTS_SUPPORT
	queue_t *gts_queue;
#endif /* GTS_SUPPORT */
#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)

	/** Timestamp information of frame
	 * The time stamping is only required for beaconing networks
	 * or if time stamping is explicitly enabled.
	 */
	uint32_t time_stamp;
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */
	/** Pointer to MPDU */
	uint8_t *mpdu;
} frame_info_t;

/**
 * Sleep Mode supported by transceiver
 */
typedef enum sleep_mode_tag {
	SLEEP_MODE_1
#ifdef ENABLE_DEEP_SLEEP
	,
	DEEP_SLEEP_MODE
#endif
} SHORTENUM sleep_mode_t;

/**
 * CSMA Mode supported by transceiver
 */
typedef enum csma_mode_tag {
	NO_CSMA_NO_IFS,
	NO_CSMA_WITH_IFS,
	CSMA_UNSLOTTED,
	CSMA_SLOTTED
} csma_mode_t;
__PACK__RST_DATA__
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

#if (TAL_TYPE == AT86RF230A) || (TAL_TYPE == AT86RF230B) || (TAL_TYPE == \
	AT86RF231) || \
	(TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == AT86RF232) || (TAL_TYPE == \
	AT86RF233) || \
	(TAL_TYPE == ATMEGARFA2) || (TAL_TYPE == ATMEGARFR2)
/** RF band */
#define RF_BAND                             BAND_2400
#elif (TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)
#define RF_BAND                             BAND_900
#else
#error "Missing RF_BAND define"
#endif

/*
 * Channel numbers and channel masks for scanning.
 */
#if (RF_BAND == BAND_2400) || defined(__DOXYGEN__)
/** Minimum channel */
#define MIN_CHANNEL                 (11)
/** Maximum channel */
#define MAX_CHANNEL                 (26)
/** Valid channel masks for scanning */
#define VALID_CHANNEL_MASK          (0x07FFF800UL)
#else   /* 900 MHz */
#define MIN_CHANNEL                 (0)
#define MAX_CHANNEL                 (10)
#define VALID_CHANNEL_MASK          (0x000007FFUL)
#endif

#if (RF_BAND == BAND_2400)

/*
 * 4 bits form one symbol since O-QPSK is used
 */
/** Symbols per octet */
#define SYMBOLS_PER_OCTET                   (2)
/** Number of symbols included in the preamble */
#define NO_SYMBOLS_PREAMBLE                 (8)
/** Number of symbols included in the SFD field */
#define NO_SYMBOLS_SFD                      (2)

#elif (RF_BAND == BAND_900)

/*
 * Depending on the channel page either
 * 1 bit forms one symbol (BPSK in channel page 0) or
 * 4 bit form one symbol (O-QPSK in channel page 2).
 */

/**
 * Symbols per octet
 */
#define SYMBOLS_PER_OCTET                   (tal_pib.CurrentPage == 0 ? 8 : 2)

/**
 * Number of symbols included in the preamble
 */
#define NO_SYMBOLS_PREAMBLE                 (tal_pib.CurrentPage == 0 ? 32 : 8)

/**
 * Number of symbols included in the SFD field
 */
#define NO_SYMBOLS_SFD                      (tal_pib.CurrentPage == 0 ? 8 : 2)
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
 * Number of symbols forming the synchronization header (SHR) for the current
 * PHY.
 * This value is the base for the PHY PIB attribute phySHRDuration.
 */
#define NO_OF_SYMBOLS_PREAMBLE_SFD          (NO_SYMBOLS_PREAMBLE + \
	NO_SYMBOLS_SFD)

/**
 * Maximum number of symbols in a frame for the current PHY.
 * This value is the base for the PHY PIB attribute phyMaxFrameDuration.
 */
#define MAX_FRAME_DURATION \
	(NO_OF_SYMBOLS_PREAMBLE_SFD + \
	(aMaxPHYPacketSize + 1) * SYMBOLS_PER_OCTET)

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
#define TAL_CONVERT_SYMBOLS_TO_US(symbols) \
	(tal_pib.CurrentPage == 0 ? \
	(tal_pib.CurrentChannel == \
	0 ? ((uint32_t)(symbols) * 50) : ((uint32_t)(symbols) *	\
	25)) : \
	(tal_pib.CurrentChannel == \
	0 ? ((uint32_t)(symbols) * 40) : ((uint32_t)(symbols) << \
	4)) \
	)
#endif  /* #if (RF_BAND == BAND_2400) */

/**
 * Conversion of microseconds to symbols
 */
#if (RF_BAND == BAND_2400)
#define TAL_CONVERT_US_TO_SYMBOLS(time)         ((time) >> 4)
#else   /* (RF_BAND == BAND_900) */
#define TAL_CONVERT_US_TO_SYMBOLS(time)	\
	(tal_pib.CurrentPage == 0 ? \
	(tal_pib.CurrentChannel == 0 ? ((time) / 50) : ((time) / 25)) :	\
	(tal_pib.CurrentChannel == 0 ? ((time) / 40) : ((time) >> 4)) \
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

#if (TAL_TYPE == ATMEGARFA1)

/*
 * Confirmation of a register write access. This operation is needed for most
 * TRX registers to finish a register write access. It is only needed when TRX
 * is in one of the PLL states. See also datasheet, section "Register access".
 */
#define CONF_REG_WRITE()   do {	\
		trx_reg_write(RG_PART_NUM, PART_NUM); \
} \
	while (0)
#endif /* TAL_TYPE == ATMEGA128RFA1 */

/* === PROTOTYPES ========================================================== */

/**
 * \ingroup group_tal
 * \defgroup group_tal_api General TAL API's
 * Following are the list of API's used across all transceivers, but there might
 * be variations in the function implementation.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes the TAL
 *
 * This function is called to initialize the TAL. The transceiver is
 * initialized, the TAL PIBs are set to their default values, and the TAL state
 * machine is set to TAL_IDLE state.
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF and the
 *                 current device part number and version number are correct;
 *         FAILURE otherwise
 * \ingroup group_tal_init
 */
retval_t tal_init(void);

/**
 * \brief Resets TAL state machine and sets the default PIB values if requested
 *
 * \param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF
 *         FAILURE otherwise
 * \ingroup group_tal_init
 */
retval_t tal_reset(bool set_default_pib);

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1) || defined(__DOXYGEN__)

/**
 * \brief Starts ED Scan
 *
 * This function starts an ED Scan for the scan duration specified by the
 * MAC layer.
 *
 * \param scan_duration Specifies the ED scan duration in symbols
 *
 * \return MAC_SUCCESS - ED scan duration timer started successfully
 *         TAL_BUSY - TAL is busy servicing the previous request from MAC
 *         TAL_TRX_ASLEEP - Transceiver is currently sleeping
 *         FAILURE otherwise
 * \ingroup group_tal_ed
 */
retval_t tal_ed_start(uint8_t scan_duration);

/**
 * User call back function for finished ED Scan
 *
 * \param energy_level Measured energy level during ED Scan
 * \ingroup group_tal_ed
 */
void tal_ed_end_cb(uint8_t energy_level);

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/**
 * \brief Gets a TAL PIB attribute
 *
 * This function is called to retrieve the transceiver information base
 * attributes.
 *
 * \param[in] attribute TAL infobase attribute ID
 * \param[out] value TAL infobase attribute value
 *
 * \return MAC_UNSUPPORTED_ATTRIBUTE if the TAL infobase attribute is not found
 *         MAC_SUCCESS otherwise
 * \ingroup group_tal_pib
 */
retval_t tal_pib_get(uint8_t attribute, uint8_t *value);

/**
 * \brief Sets a TAL PIB attribute
 *
 * This function is called to set the transceiver information base
 * attributes.
 *
 * \param attribute TAL infobase attribute ID
 * \param value TAL infobase attribute value to be set
 *
 * \return MAC_UNSUPPORTED_ATTRIBUTE if the TAL info base attribute is not found
 *         TAL_BUSY if the TAL is not in TAL_IDLE state. An exception is
 *         macBeaconTxTime which can be accepted by TAL even if TAL is not
 *         in TAL_IDLE state.
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 *         TAL_TRX_ASLEEP if trx is in SLEEP mode and access to trx is required
 * \ingroup group_tal_pib
 */
retval_t tal_pib_set(uint8_t attribute, pib_value_t *value);

/**
 * \brief Switches receiver on or off
 *
 * This function switches the receiver on (PHY_RX_ON) or off (PHY_TRX_OFF).
 *
 * \param state New state of receiver
 *
 * \return TAL_BUSY if the TAL state machine cannot switch receiver on or off,
 *         TRX_OFF if receiver has been switched off, or
 *         RX_ON otherwise.
 *
 * \ingroup group_tal_rx
 */
uint8_t tal_rx_enable(uint8_t state);

/**
 * User call back function for frame reception
 *
 * \param rx_frame Pointer to received frame structure of type frame_info_t
 *                 or to received frame array
 * \ingroup group_tal_rx
 */
void tal_rx_frame_cb(frame_info_t *rx_frame);

#ifdef ENABLE_RTB

/**
 * User call back function for frame reception in case RTB is used
 *
 * \param rx_frame Pointer to received frame structure of type frame_info_t
 *                 or to received frame array
 * \ingroup group_tal_rx
 */
void rtb_rx_frame_cb(frame_info_t *rx_frame);

#endif  /* ENABLE_RTB */

#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) || \
	defined(__DOXYGEN__)

/**
 * \brief Beacon frame transmission
 *
 * \param tx_frame Pointer to the frame_info_t structure
 * \ingroup group_tal_tx
 */
void tal_tx_beacon(frame_info_t *tx_frame);

#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

/**
 * \brief Requests to TAL to transmit frame
 *
 * This function is called by the MAC to deliver a frame to the TAL
 * to be transmitted by the transceiver.
 *
 * \param tx_frame Pointer to the frame_info_t structure or
 *                 to frame array to be transmitted
 * \param csma_mode Indicates mode of csma-ca to be performed for this frame
 * \param perform_frame_retry Indicates whether to retries are to be performed
 * for
 *                            this frame
 *
 * \return MAC_SUCCESS  if the TAL has accepted the data from the MAC for frame
 *                 transmission
 *         TAL_BUSY if the TAL is busy servicing the previous MAC request
 * \ingroup group_tal_tx
 */
retval_t tal_tx_frame(frame_info_t *tx_frame, csma_mode_t csma_mode,
		bool perform_frame_retry);

/**
 * User call back function for frame transmission
 *
 * \param status Status of frame transmission attempt
 * \param frame Pointer to frame structure of type frame_info_t
 * \ingroup group_tal_tx
 */
void tal_tx_frame_done_cb(retval_t status, frame_info_t *frame);

/**
 * \brief Sets the transceiver to sleep
 *
 * This function sets the transceiver to sleep state.
 *
 * \param mode Defines sleep mode of transceiver SLEEP or PHY_TRX_OFF)
 *
 * \return   TAL_BUSY - The transceiver is busy in TX or RX
 *           MAC_SUCCESS - The transceiver is put to sleep
 *           TAL_TRX_ASLEEP - The transceiver is already asleep
 *           MAC_INVALID_PARAMETER - The specified sleep mode is not supported
 * \ingroup group_tal_pm
 */
retval_t tal_trx_sleep(sleep_mode_t mode);

/**
 * \brief Wakes up the transceiver from sleep
 *
 * This function awakes the transceiver from sleep state.
 *
 * \return   TAL_TRX_AWAKE - The transceiver is already awake
 *           MAC_SUCCESS - The transceiver is woken up from sleep
 *           FAILURE - The transceiver did not wake-up from sleep
 * \ingroup group_tal_pm
 */
retval_t tal_trx_wakeup(void);

/**
 * \brief Adds two time values
 *
 * This function adds two time values
 *
 * \param a Time value 1
 * \param b Time value 2
 *
 * \return value of a + b
 *
 */
static inline uint32_t tal_add_time_symbols(uint32_t a, uint32_t b)
{
	return ((a + b) & SYMBOL_MASK);
}

/**
 * \brief Subtract two time values
 *
 * This function subtracts two time values taking care of roll over.
 *
 * \param a Time value 1
 * \param b Time value 2
 *
 * \return value a - b
 *
 */
static inline uint32_t tal_sub_time_symbols(uint32_t a, uint32_t b)
{
	if (a > b) {
		return ((a - b) & SYMBOL_MASK);
	} else {
		/* This is a roll over case */
		return (((MAX_SYMBOL_TIME - b) + a) & SYMBOL_MASK);
	}
}

#ifdef __cplusplus
} /* extern "C" */
#endif
/* ! @} */

#endif /* TAL_H */

/* EOF */
