/**
 * \file app_range_mode.h
 *
 * \brief Range Measurement mode related prototypes and defines
 * - Performance Analyzer application
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* Prevent double inclusion */
#ifndef APP_RANGE_MODE_H
#define APP_RANGE_MODE_H

/**
 * \ingroup group_perf_analyzer
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
void range_test_tx_on_init(void *arg);

/**
 * \brief Application task handling Range Measurement when the node is in
 * RANGE_TEST_TX_ON state, where both TX and RX is enabled
 */
void range_test_tx_on_task(void);

/**
 * \brief Callback that is called if data has been received by trx when the
 *  node is in Range Measurement mode i.e RANGE_TEST_TX_ON or RANGE_TEST_TX_OFF
 * \param frame Pointer to received frame
 */
void range_test_rx_cb(frame_info_t *frame);

/**
 * \brief To exit the Application task from RANGE_TEST_TX_ON state,
 *  where both TX and RX is enabled
 */
void range_test_tx_on_exit(void);

/* RANGE_TEST_TX_OFF state functions */

/**
 * \brief Application task handling Range Measurement when the node is in
 *  RANGE_TEST_TX_OFF state, where no TX is enabled and only RX is ON.
 */
void range_test_tx_off_task(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ! \} */
#endif /* APP_STATE_H */
/* EOF */
