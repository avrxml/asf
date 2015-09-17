/**
 * @file tal_irq_handler.h
 *
 * @brief This header file contains the interrupt handling definitions
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_IRQ_HANDLER_H
#define TAL_IRQ_HANDLER_H

/* === INCLUDES ============================================================ */

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup group_tal_irq_233
 * @{
 */

/**
 * \brief Transceiver interrupt handler
 *
 * This function handles the transceiver generated interrupts.
 */
void trx_irq_handler_cb(void);

/**
 * \brief Transceiver interrupt handler for awake end IRQ
 *
 * This function handles the transceiver awake end interrupt.
 */

void trx_irq_awake_handler_cb(void);

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) || (defined __DOXYGEN__)

/**
 * \brief Timestamp interrupt handler
 *
 * This function handles the interrupts handling the timestamp.
 */
void trx_irq_timestamp_handler_cb(void);

#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) || (defined
         *__DOXYGEN__) */

/* ! @} */
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_IRQ_HANDLER_H */

/* EOF */
