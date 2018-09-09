/**
 * \file app_range_mode.h
 *
 * \brief Range Measurement mode related prototypes and defines
 * - Performance Analyzer application for AT86RF215
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 */

/*
 * Copyright (c) 2015-2018, Microchip Technology Inc All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef APP_RANGE_MODE_H
#define APP_RANGE_MODE_H

/**
 * \ingroup group_perf_analyzer_2
 * \defgroup group_range_measure  Range Measurement
 * Performs the  functionalities of Range measurement mode,
 * Evaluate the Communication coverage of the transceiver.
 * \{
 */

/* === Includes ============================================================= */

/* === Macros =============================================================== */
/* RANGE_TEST_TX_ON state functions */

/**
 * \brief Initialization of Range Measurement mode in RANGE_TX_ON State
 *
 * \param arg arguments to init RANGE_TX_ON state
 */
void range_test_tx_on_init(trx_id_t trx, void *arg);

/**
 * \brief Application task handling Range Measurement when the node is in
 * RANGE_TEST_TX_ON state, where both TX and RX is enabled
 */
void range_test_tx_on_task(trx_id_t trx);

/**
 * \brief Callback that is called if data has been received by trx when the
 *  node is in Range Measurement mode i.e RANGE_TEST_TX_ON or RANGE_TEST_TX_OFF
 * \param frame Pointer to received frame
 */
void range_test_rx_cb(trx_id_t trx, frame_info_t *frame);

/**
 * \brief To exit the Application task from RANGE_TEST_TX_ON state,
 *  where both TX and RX is enabled
 */
void range_test_tx_on_exit(trx_id_t trx);

/* RANGE_TEST_TX_OFF state functions */

/**
 * \brief Application task handling Range Measurement when the node is in
 *  RANGE_TEST_TX_OFF state, where no TX is enabled and only RX is ON.
 */
void range_test_tx_off_task(trx_id_t trx);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ! \} */
#endif /* APP_RANGE_MODE_H */
/* EOF */
