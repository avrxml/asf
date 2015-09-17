/**
 * \file tal_internal.h
 *
 * \brief This header file contains types and variable definition that are used
 * within the TAL only.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 *
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_INTERNAL_H
#define TAL_INTERNAL_H

/* === INCLUDES ============================================================ */

#include "bmm.h"
#include "qmm.h"
#if (defined MAC_SECURITY_ZIP || defined MAC_SECURITY_2006)
#include "tal.h"
#endif
#ifdef BEACON_SUPPORT
#include "tal_slotted_csma.h"
#endif  /* BEACON_SUPPORT */
#ifdef ENABLE_DEBUG_PINS
#include "pal_config.h"
#endif
#include "mac_build_config.h"

/**
 * \ingroup group_tal
 * \defgroup group_tal_rfr2 ATMEGARFR2 Transceiver Abstraction Layer
 * The ATmega256RFR2 is a low-power CMOS 8-bit microcontroller based on the AVR
 * enhanced RISC architecture combined with a high data rate transceiver for the
 * 2.4 GHz
 *  ISM band. It is derived from the ATmega1281 microcontroller and the
 * AT86RF231 radio transceiver.
 * The Transceiver Abstraction Layer (TAL) implements the transceiver specific
 * functionalities and
 * provides interfaces to the upper layers (like IEEE 802.15.4 MAC )and  uses
 * the services of PAL.
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_state_machine_rfr2  TAL State Machine
 * The different operating states of the Transceiver are controlled by the TAL
 * state machine.
 *
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_init_rfr2  TAL Initialization and reset
 * Performs initialization and reset functionalities of the transceiver
 *
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_ed_rfr2   TAL Energy Detection
 * Performs the ED scan functionalities.
 *
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_irq_rfr2  Transceiver Interrupt Handling
 * Handles Transceiver related Interrupts.
 *
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_pib_rfr2   TAL PIB Storage
 * The PIB(Pan Information Base) attributes related to the TAL are Stored and
 * handled  by the TAL PIB storage.
 *
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_tx_rfr2   TAL Frame Transmission Unit
 * The Frame Transmission Unit generates and transmits the frames using PAL .
 *
 */

/**
 * \ingroup group_tal_tx_rfr2
 * \defgroup group_tal_tx_csma_rfr2   TAL CSMA/CA Module
 * Performs channel access mechanism for frame transmission
 * For Detailed information refer  CSMA-CA algorithm section of IEEE Std
 * 802.15.4-2006
 *
 */

/**
 * \ingroup group_tal_rfr2
 * \defgroup group_tal_rx_rfr2   TAL Frame Reception Unit
 * The Frame Reception Unit reads/uploads the incoming frames .
 *
 */

/* === TYPES =============================================================== */

/** TAL states */
#if ((defined BEACON_SUPPORT) && (MAC_SCAN_ED_REQUEST_CONFIRM == 1))
typedef enum tal_state_tag {
	TAL_IDLE           = 0,
	TAL_TX_AUTO        = 1,
	TAL_TX_DONE        = 2,
	TAL_SLOTTED_CSMA   = 3,
	TAL_ED_RUNNING     = 4,
	TAL_ED_DONE        = 5
} SHORTENUM tal_state_t;
#endif

#if ((!defined BEACON_SUPPORT) && (MAC_SCAN_ED_REQUEST_CONFIRM == 1))
typedef enum tal_state_tag {
	TAL_IDLE           = 0,
	TAL_TX_AUTO        = 1,
	TAL_TX_DONE        = 2,
	TAL_ED_RUNNING     = 4,
	TAL_ED_DONE        = 5
} SHORTENUM tal_state_t;
#endif

#if ((defined BEACON_SUPPORT) && (MAC_SCAN_ED_REQUEST_CONFIRM == 0))
typedef enum tal_state_tag {
	TAL_IDLE           = 0,
	TAL_TX_AUTO        = 1,
	TAL_TX_DONE        = 2,
	TAL_SLOTTED_CSMA   = 3
} SHORTENUM tal_state_t;
#endif

#if ((!defined BEACON_SUPPORT) && (MAC_SCAN_ED_REQUEST_CONFIRM == 0))
typedef enum tal_state_tag {
	TAL_IDLE           = 0,
	TAL_TX_AUTO        = 1,
	TAL_TX_DONE        = 2
} SHORTENUM tal_state_t;
#endif

/** Transceiver interrupt reasons */
typedef enum trx_irq_reason_tag {
	/** No interrupt is indicated by IRQ_STATUS register */
	TRX_NO_IRQ                      = (0x00),

	/** PLL goes to lock-state. */
	TRX_IRQ_PLL_LOCK                = (0x01),

	/** Signals an unlocked PLL */
	TRX_IRQ_PLL_UNLOCK              = (0x02),

	/** Signals begin of a receiving frame */
	TRX_IRQ_RX_START                = (0x04),

	/** Signals completion of a frame reception. */
	TRX_IRQ_RX_END                  = (0x08),

	/** Signals the end of a CCA or ED measurement. */
	TRX_IRQ_CCA_ED_READY            = (0x10),

	/** Signals an address match. */
	TRX_IRQ_AMI                     = (0x20),

	/** Signals the completion of a frame transmission. */
	TRX_IRQ_TX_END                  = (0x40),

	/**
	 * Indicates that the radio transceiver reached TRX_OFF state
	 * after P_ON, RESET, or SLEEP states.
	 */
	TRX_IRQ_AWAKE                   = (0x80)
} trx_irq_reason_t;

/* === EXTERNALS =========================================================== */

/* Global TAL variables */
extern tal_state_t tal_state;
extern tal_trx_status_t tal_trx_status;
extern frame_info_t *mac_frame_ptr;
extern queue_t tal_incoming_frame_queue;
extern uint8_t *tal_frame_to_tx;
extern buffer_t *tal_rx_buffer;
extern bool tal_rx_on_required;
extern uint8_t last_frame_length;
extern volatile bool tal_awake_end_flag;

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
extern uint32_t tal_rx_timestamp;
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

#ifdef BEACON_SUPPORT
extern csma_state_t tal_csma_state;
#if (MAC_START_REQUEST_CONFIRM == 1)
extern uint8_t transaction_duration_periods;
#endif
#endif  /* BEACON_SUPPORT */

#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
extern bool tal_beacon_transmission;
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

/* === MACROS ============================================================== */

#ifndef _BV

/**
 * Bit value -- compute the bitmask for a bit position
 */
#define _BV(x) (1 << (x))
#endif

/**
 * Conversion of number of PSDU octets to duration in microseconds
 */
#ifdef HIGH_DATA_RATE_SUPPORT
#define TAL_PSDU_US_PER_OCTET(octets) \
	( \
		tal_pib.CurrentPage == 0 ? ((uint16_t)(octets) * 32) : \
		( \
			tal_pib.CurrentPage == 2 ? ((uint16_t)(octets) * 16) : \
			( \
				tal_pib.CurrentPage == \
				16 ? ((uint16_t)(octets) * \
				8) : ((uint16_t)(octets) * 4) \
			) \
		) \
	)
#else   /* #ifdef not HIGH_DATA_RATE_SUPPORT */
#define TAL_PSDU_US_PER_OCTET(octets)       ((uint16_t)(octets) * 32)
#endif

/*
 * Debug synonyms
 * These debug defines are only applicable if
 * the build switch "-DENABLE_DEBUG_PINS" is set.
 * The implementation of the debug pins is located in
 * pal_config.h
 */
#ifdef ENABLE_DEBUG_PINS
#define PIN_BEACON_START()              TST_PIN_0_HIGH()
#define PIN_BEACON_END()                TST_PIN_0_LOW()
#define PIN_CSMA_START()                TST_PIN_1_HIGH()
#define PIN_CSMA_END()                  TST_PIN_1_LOW()
#define PIN_BACKOFF_START()             TST_PIN_2_HIGH()
#define PIN_BACKOFF_END()               TST_PIN_2_LOW()
#define PIN_CCA_START()                 TST_PIN_3_HIGH()
#define PIN_CCA_END()                   TST_PIN_3_LOW()
#define PIN_TX_START()                  TST_PIN_4_HIGH()
#define PIN_TX_END()                    TST_PIN_4_LOW()
#define PIN_ACK_WAITING_START()         TST_PIN_5_HIGH()
#define PIN_ACK_WAITING_END()           TST_PIN_5_LOW()
#define PIN_WAITING_FOR_BEACON_START()  TST_PIN_6_HIGH()
#define PIN_WAITING_FOR_BEACON_END()    TST_PIN_6_LOW()
#define PIN_BEACON_LOSS_TIMER_START()
#define PIN_BEACON_LOSS_TIMER_END()
#define PIN_ACK_OK_START()              TST_PIN_7_HIGH()
#define PIN_ACK_OK_END()                TST_PIN_7_LOW()
#define PIN_NO_ACK_START()              TST_PIN_8_HIGH()
#define PIN_NO_ACK_END()                TST_PIN_8_LOW()
#else
#define PIN_BEACON_START()
#define PIN_BEACON_END()
#define PIN_CSMA_START()
#define PIN_CSMA_END()
#define PIN_BACKOFF_START()
#define PIN_BACKOFF_END()
#define PIN_CCA_START()
#define PIN_CCA_END()
#define PIN_TX_START()
#define PIN_TX_END()
#define PIN_ACK_WAITING_START()
#define PIN_ACK_WAITING_END()
#define PIN_WAITING_FOR_BEACON_START()
#define PIN_WAITING_FOR_BEACON_END()
#define PIN_BEACON_LOSS_TIMER_START()
#define PIN_BEACON_LOSS_TIMER_END()
#define PIN_ACK_OK_START()
#define PIN_ACK_OK_END()
#define PIN_NO_ACK_START()
#define PIN_NO_ACK_END()
#endif

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
#define TRX_IRQ_DEFAULT         (TRX_IRQ_RX_START | TRX_IRQ_RX_END | \
	TRX_IRQ_TX_END)
#else
#define TRX_IRQ_DEFAULT         (TRX_IRQ_RX_END | TRX_IRQ_TX_END)
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */
#define TRX_IRQ_AWAKE_ONLY      (TRX_IRQ_AWAKE)
#define TRX_IRQ_CCA_ED_ONLY     (TRX_IRQ_CCA_ED_READY)
#define TRX_IRQ_NONE            (0)

/* === PROTOTYPES ========================================================== */

/*
 * Prototypes from tal.c
 */

/**
 * \brief Sets transceiver state
 *
 * \param trx_cmd needs to be one of the trx commands
 *
 * \return current trx state
 * \ingroup group_tal_state_machine
 */
tal_trx_status_t set_trx_state(trx_cmd_t trx_cmd);

#ifdef ENABLE_FTN_PLL_CALIBRATION

/**
 * \brief PLL calibration and filter tuning timer callback
 *
 * \param parameter Unused callback parameter
 */
void calibration_timer_handler_cb(void *parameter);

#endif  /* ENABLE_FTN_PLL_CALIBRATION */

/*
 * Prototypes from tal_ed.c
 */
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)

/**
 * \brief Scan done
 *
 * This function updates the max_ed_level and invokes the callback function
 * tal_ed_end_cb().
 *
 * \ingroup group_tal_ed
 */
void ed_scan_done(void);

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/**
 * \name Transceiver Access Macros
 *  \sa \b Transceiver \b macros section of group_pal_gpio
 * @{
 */

/**
 * \brief Writes data into a transceiver register
 *
 * This macro writes a value into transceiver register.
 *
 * \param addr Address of the trx register
 * \param data Data to be written to trx register
 * \ingroup group_pal_trx
 */
#define trx_reg_write(addr, data) \
	(*(volatile uint8_t *)(addr)) = (data)

/**
 * \brief Reads current value from a transceiver register
 *
 * This macro reads the current value from a transceiver register.
 *
 * \param addr Specifies the address of the trx register
 * from which the data shall be read
 * \ingroup group_pal_trx
 * \return value of the register read
 */
#define trx_reg_read(addr) \
	(*(volatile uint8_t *)(addr))

/**
 * \brief Reads frame buffer of the transceiver
 *
 * This macro reads the frame buffer of the transceiver.
 *
 * \param[out] data Pointer to the location to store frame
 * \param[in] length Number of bytes to be read from the frame
 * buffer.
 * \ingroup group_pal_trx
 */
#define trx_frame_read(data, length) \
	memcpy((data), (void *)&TRXFBST, (length))

/**
 * \brief Writes data into frame buffer of the transceiver
 *
 * This macro writes data into the frame buffer of the transceiver
 *
 * \param[in] data Pointer to data to be written into frame buffer
 * \param[in] length Number of bytes to be written into frame buffer
 * \ingroup group_pal_trx
 */
#define trx_frame_write(data, length) \
	memcpy((void *)&TRXFBST, (data), (length))

#ifndef __DOXYGEN__
#define _trx_bit_read(addr, mask, pos) \
	(((*(volatile uint8_t *)(addr)) & (mask)) >> (pos))
#endif

/**
 * \brief Subregister read
 *
 * \param   arg Subregister
 *
 * \return  Value of the read subregister
 * \ingroup group_pal_trx
 */
#define trx_bit_read(arg) \
	_trx_bit_read(arg)

#ifndef __DOXYGEN__
#define _trx_bit_write(addr, mask, pos, val) do { \
		(*(volatile uint8_t *)(addr)) \
			= ((*(volatile uint8_t *)(addr)) & ~(mask)) | \
				(((val) << (pos)) & (mask)); \
} \
	while (0)
#endif

/**
 * \brief Subregister write
 *
 * \param[in]   arg1  Subregister
 * \param[out]  val  Data, which is muxed into the register
 * \ingroup group_pal_trx
 */
#define trx_bit_write(arg1, val) \
	_trx_bit_write(arg1, val)

/* ! @} */

/**
 * \name Transceiver Access  Macros
 * @{
 */

/*
 * Set TRX GPIO pins.
 */
#define TRX_RST_HIGH()                      (TRXPR |= _BV(TRXRST))  /**< Set
	                                                             * Reset
	                                                             * Bit.
	                                                             **/
#define TRX_RST_LOW()                       (TRXPR &= ~_BV(TRXRST)) /**< Clear
	                                                             * Reset
	                                                             * Bit.
	                                                             **/
#define TRX_SLP_TR_HIGH()                   (TRXPR |= _BV(SLPTR))   /**< Set
	                                                             * Sleep/TR
	                                                             * Bit. */
#define TRX_SLP_TR_LOW()                    (TRXPR &= ~_BV(SLPTR))  /**< Clear
	                                                             * Sleep/TR
	                                                             * Bit. */

/* ! @} */

/**
 * \name Transceiver IRQ Macros
 * @{
 */
/** Clears TRX TX_END interrupt. */
#define CLEAR_TRX_IRQ_TX_END()          (IRQ_STATUS = _BV(TX_END))

/** Clear RX TIME STAMP interrupt. */
#define CLEAR_TRX_IRQ_TSTAMP()          (IRQ_STATUS = _BV(RX_START))

/** Clear TRX RX_END interrupt. */
#define CLEAR_TRX_IRQ_RX_END()          (IRQ_STATUS = _BV(RX_END))

/** Clear TRX CCA_ED_DONE interrupt. */
#define CLEAR_TRX_IRQ_CCA_ED()          (IRQ_STATUS = _BV(CCA_ED_DONE_EN))

/** Clear AMI interrupt. */
#define CLEAR_TRX_IRQ_AMI()             (IRQ_STATUS = _BV(AMI))

/** Clear TRX BAT LOW interrupt. */
#define CLEAR_TRX_IRQ_BATMON()          (BATMON = _BV(BAT_LOW))

/** Clear TRX AWAKE interrupt. */
#define CLEAR_TRX_IRQ_AWAKE()           (IRQ_STATUS = _BV(AWAKE))

/** Clear TRX PLL LOCK interrupt. */
#define CLEAR_TRX_IRQ_PLL_LOCK()        (IRQ_STATUS = _BV(PLL_LOCK))

/** Clear TRX PLL UNLOCK interrupt. */
#define CLEAR_TRX_IRQ_PLL_UNLOCK()      (IRQ_STATUS = _BV(PLL_UNLOCK))

/*
 * Note: TRX AES RDY is cleared on read/write of
 * AES_STATE, which has to be done in the AES
 * application.
 */
/* ! @} */

#endif /* TAL_INTERNAL_H */
