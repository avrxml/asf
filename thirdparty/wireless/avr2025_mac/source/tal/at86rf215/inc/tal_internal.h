/**
 * @file tal_internal.h
 *
 * @brief This header file contains types and variable definition that are used
 *        within the TAL only.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_INTERNAL_H
#define TAL_INTERNAL_H

/* === INCLUDES ============================================================ */

#include "tal_config.h"
#include "bmm.h"
#include "qmm.h"
#include "mac_build_config.h"
#include "tal.h"
#include "tal_rf215.h"
#if (defined IQ_RADIO) || (defined INCLUDE_INTERN_REGS)
#    include "at86rf215_internal.h"
#endif

/* === TYPES =============================================================== */

/**
 * Enumeration for tal_state
 */
typedef enum tal_state_tag {
	TAL_IDLE,
	TAL_SLEEP,
	TAL_RESET,
	TAL_WAKING_UP,
	TAL_TX,
	TAL_ED_SCAN
#if (defined ENABLE_TFA) || (defined TFA_CCA) || (defined TFA_CW)
	,
	TAL_TFA_CW_RX,
	TAL_TFA_CW,
	TAL_TFA_CCA
#endif
#ifdef SUPPORT_MODE_SWITCH
	,
	TAL_NEW_MODE_RECEIVING,
	TAL_ACK_TRANSMITTING
#endif
} SHORTENUM tal_state_t;

/**
 * Enumeration for tx_state
 */
typedef enum tx_state_tag {
	TX_IDLE,
	TX_BACKOFF,
	TX_CCATX,
	TX_TX,
	TX_WAITING_FOR_ACK
	,
	TX_DEFER
#if (defined BASIC_MODE) || (defined SUPPORT_MODE_SWITCH)
	,
	TX_CCA
#endif
#ifdef SUPPORT_MODE_SWITCH
	,
	TX_MS_PPDU,
	TX_WAIT_FOR_NEW_MODE_TRANSMITTING,
	TX_MS_NEW_MODE_TRANSMITTING
#endif
} SHORTENUM tx_state_t;

/**
 * Enumeration use as parameter for function transmit_frame()
 */
typedef enum cca_use_tag {
	NO_CCA,
	WITH_CCA
} SHORTENUM cca_use_t;

/* === EXTERNALS =========================================================== */

/* Global TAL variables */
extern tal_state_t tal_state[NUM_TRX];
extern tx_state_t tx_state[NUM_TRX];
extern const uint8_t timer_cb_parameter[NUM_TRX];
extern bool ack_transmitting[NUM_TRX];
extern int8_t tal_current_ed_val[NUM_TRX];
extern frame_info_t *mac_frame_ptr[NUM_TRX];
extern queue_t tal_incoming_frame_queue[NUM_TRX];
extern uint8_t *tal_frame_to_tx[NUM_TRX];
extern buffer_t *tal_rx_buffer[NUM_TRX];
extern bool tal_buf_shortage[NUM_TRX];
extern rf_cmd_state_t trx_state[NUM_TRX];
extern rf_cmd_state_t trx_default_state[NUM_TRX];
extern uint32_t rxe_txe_tstamp[NUM_TRX];
extern uint8_t txc[NUM_TRX][2];
extern bool frame_buf_filled[NUM_TRX];
#if (defined ENABLE_TSTAMP) || (defined MEASURE_ON_AIR_DURATION)
extern uint32_t fs_tstamp[NUM_TRX];
#endif
extern frame_info_t *rx_frm_info[NUM_TRX];
#ifdef BASIC_MODE
extern uint8_t *rx_frm_ptr[NUM_TRX];
extern uint16_t last_txframe_length[NUM_TRX];
#endif
#ifdef SUPPORT_MODE_SWITCH
extern bool csm_active[NUM_TRX];
#endif
extern volatile bb_irq_t tal_bb_irqs[NUM_TRX];
extern volatile rf_irq_t tal_rf_irqs[NUM_TRX];

/* === MACROS ============================================================== */

/** Defines to handle register offset */
#define CALC_REG_OFFSET(var)                uint16_t offset \
		= RF_BASE_ADDR_OFFSET * var
#define GET_REG_ADDR (reg)offset + reg

/*
 * Time gap between each poll access in microseconds.
 * If the value is equal to zero, no time gap is applied.
 */
#define POLL_TIME_GAP                       10

/* Maximum PLL lock duration in us */
#define MAX_PLL_LOCK_DURATION               200

/* Maximum settling duration after PLL has been freezed */
#define PLL_FRZ_SETTLING_DURATION           20

/**
 * Register value for default transmit power
 */
#define DEFAULT_TX_PWR_REG                  20

#define DEFAULT_FRAME_TYPES         ((1 << FCF_FRAMETYPE_BEACON) | \
	(1 << FCF_FRAMETYPE_DATA) | (1 << FCF_FRAMETYPE_MAC_CMD))
#define ACK_FRAME_TYPE_ONLY         (1 << FCF_FRAMETYPE_ACK)

/*
 * Processing delay offsets (in ns) for ToF correction
 *
 * Note: These offsets are only valid for Legacy-OQPSK using 250kBps.
 * All other modulation schemes with the corresponding variations
 * need to be calibrated if required.
 */
#if (defined RF215v1)
#   define TOF_PROC_DELAY_OFFSET_SUB_GHZ_NS     (583230)
#   define TOF_PROC_DELAY_OFFSET_2_4_GHZ_NS     (573643)
#elif (defined RF215v3)
#   define TOF_PROC_DELAY_OFFSET_SUB_GHZ_NS     (583158)
#   define TOF_PROC_DELAY_OFFSET_2_4_GHZ_NS     (573537)
#else
/* Nothing defined yet. */
#endif

#define INVALID_TOF_VALUE                       (0xFFFFFFFF)

/* === PROTOTYPES ========================================================== */

/*
 * Prototypes from tal.c
 */
void switch_to_rx(trx_id_t trx_id);
void switch_to_txprep(trx_id_t trx_id);
void wait_for_txprep(trx_id_t trx_id);
void stop_tal_timer(trx_id_t trx_id);
void cancel_any_reception(trx_id_t trx_id);

#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
void stop_rpc(trx_id_t trx_id);
void start_rpc(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_ftn.c
 */
#ifdef ENABLE_FTN_PLL_CALIBRATION
void start_ftn_timer(trx_id_t trx_id);
void stop_ftn_timer(trx_id_t trx_id);

#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#ifdef RF215v1
void calibrate_LO(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_init.c
 */
void trx_config(trx_id_t trx_id);

/*
 * Prototypes from tal_pib.c
 */
void calculate_pib_values(trx_id_t trx_id);
retval_t config_phy(trx_id_t trx_id);
void init_tal_pib(trx_id_t trx_id);
void write_all_tal_pib_to_trx(trx_id_t trx_id);

#if (defined SUPPORT_FSK) && ((defined SUPPORT_FSK_RAW_MODE) ||	\
	(defined SUPPORT_MODE_SWITCH))
void configure_raw_mode(trx_id_t trx_id, bool enable);

#endif

/*
 * Prototypes from tal_ed.c
 */
void handle_ed_end_irq(trx_id_t trx_id);
void set_ed_sample_duration(trx_id_t trx_id, uint16_t sample_duration_us);
uint8_t scale_ed_value(int8_t ed);
void stop_ed_scan(trx_id_t trx_id);

/*
 * Prototypes from tal_4g_utils.c
 */
uint16_t get_cca_duration_us(trx_id_t trx_id);
int8_t get_cca_thres(trx_id_t trx_id);
uint16_t ceiling_sym(trx_id_t trx_id, uint16_t duration_us);
uint16_t get_AckWaitDuration_us(trx_id_t trx_id);
uint16_t get_AckTiming_us(trx_id_t trx_id);
void get_ch_freq0_spacing(trx_id_t trx_id, uint32_t *freq, uint32_t *spacing);
uint8_t phr_duration_sym(trx_id_t trx_id);
uint16_t calculate_cca_duration_us(trx_id_t trx_id);
float get_data_rate(trx_id_t trx_id);
uint8_t shr_duration_sym(trx_id_t trx_id);

#ifdef MEASURE_ON_AIR_DURATION
uint16_t get_ack_duration_sym(trx_id_t trx_id);

#endif
retval_t get_supported_channels_tuple(trx_id_t trx_id, uint32_t *value);

/*
 * Prototypes from tal_phy_cfg.c
 */
retval_t conf_trx_modulation(trx_id_t trx_id);

#ifdef SUPPORT_FSK
void set_sfd(trx_id_t trx_id);
void set_fsk_pibs(trx_id_t trx_id);
retval_t conf_fsk_data_rate(trx_id_t trx_id, fsk_sym_rate_t rate);
void config_fsk_rpc(trx_id_t trx_id, fsk_sym_rate_t sym_rate);

#ifdef SUPPORT_MODE_SWITCH
retval_t conf_fsk(trx_id_t trx_id);

#endif
#endif

/*
 * Prototypes from tal_fe.c
 */
#ifdef SUPPORT_FSK
retval_t fsk_rfcfg(fsk_mod_type_t mod_type, fsk_sym_rate_t srate,
		mod_idx_t mod_idx, trx_id_t trx_id);

#endif
#ifdef SUPPORT_OFDM
retval_t ofdm_rfcfg(ofdm_option_t ofdm_opt, trx_id_t trx_id);

#endif
#if (defined SUPPORT_OQPSK) || (defined SUPPORT_LEGACY_OQPSK)
retval_t oqpsk_rfcfg(oqpsk_chip_rate_t chip_rate, trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_mode_switch.c
 */
#ifdef SUPPORT_MODE_SWITCH
void init_mode_switch(void);
void prepare_actual_transmission(trx_id_t trx_id);
void restore_previous_phy(trx_id_t trx_id);
void set_csm(trx_id_t trx_id);
void tx_ms_ppdu(trx_id_t trx_id);
void handle_rx_ms_packet(trx_id_t trx_id);
void save_current_phy(trx_id_t trx_id);

#endif /* #ifdef SUPPORT_MODE_SWITCH */

/*
 * Prototypes from tal_rx.c
 */
#ifdef BASIC_MODE
void complete_rx_transaction(trx_id_t trx_id);
void upload_frame(trx_id_t trx_id);
void handle_rx_end_irq(trx_id_t trx_id);
void process_incoming_frame(trx_id_t trx_id, buffer_t *buf);
void ack_transmission_done(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_auto_rx.c
 */
#ifndef BASIC_MODE
void complete_rx_transaction(trx_id_t trx_id);
void handle_rx_end_irq(trx_id_t trx_id);
void process_incoming_frame(trx_id_t trx_id, buffer_t *buf);
void ack_transmission_done(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_tx.c
 */
#ifdef BASIC_MODE
void transmit_frame(trx_id_t trx_id);
void handle_tx_end_irq(trx_id_t trx_id);
void tx_done_handling(trx_id_t trx_id, retval_t status);

#endif

/*
 * Prototypes from tal_auto_tx.c
 */
#ifndef BASIC_MODE
void transmit_frame(trx_id_t trx_id, cca_use_t cca);
void handle_tx_end_irq(trx_id_t trx_id);
void tx_done_handling(trx_id_t trx_id, retval_t status);
void continue_deferred_transmission(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_csma.c
 */
#ifdef BASIC_MODE
void cca_done_handling(trx_id_t trx_id);
void csma_start(trx_id_t trx_id);
void csma_continue(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_auto_csma.c
 */
#ifndef BASIC_MODE
void cca_done_handling(trx_id_t trx_id);
void csma_start(trx_id_t trx_id);
void csma_continue(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_ack.c
 */
#ifdef BASIC_MODE
bool is_ack_required(trx_id_t trx_id);
bool is_frame_an_ack(trx_id_t trx_id);
void schedule_ack_transmission(trx_id_t trx_id);
bool handle_ack_reception(trx_id_t trx_id);
void ack_timout_cb(void *parameter);

#endif

/*
 * Prototypes from tal_auto_ack.c
 */
#ifndef BASIC_MODE
bool is_frame_an_ack(trx_id_t trx_id);
void start_ack_wait_timer(trx_id_t trx_id);
bool is_ack_valid(trx_id_t trx_id);

#endif

/*
 * Prototypes from tal_irq_handler.c
 */
void trx_irq_handler_cb(void);

#ifdef IQ_RADIO
void rf_irq_handler_cb(void);
void bb_irq_handler_cb(void);

#endif
#if (defined ENABLE_TSTAMP) || (defined DOXYGEN)
void trx_irq_timestamp_handler_cb(void);

#endif  /* #if (defined ENABLE_TSTAMP) || (defined DOXYGEN) */

/*
 * Prototypes from tfa_batmon.c
 */
#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined TFA_BAT_MON_IRQ)
void handle_batmon_irq(void);

#endif

#endif /* TAL_INTERNAL_H */
