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
 *
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

/**
 * \brief Clears the transceiver RX_END interrupt
 *
 */
#define pal_trx_irq_flag_clr_rx_end()   (CLEAR_TRX_IRQ_RX_END())

/**
 * \brief Clears the transceiver TX_END interrupt
 *
 */
#define pal_trx_irq_flag_clr_tx_end()   (CLEAR_TRX_IRQ_TX_END())

/**
 * \brief Clears the transceiver CCA_ED_END interrupt
 *
 */
#define pal_trx_irq_flag_clr_cca_ed()   (CLEAR_TRX_IRQ_CCA_ED())

/**
 * \brief Clears the transceiver AWAKE interrupt
 *
 */
#define pal_trx_irq_flag_clr_awake()    (CLEAR_TRX_IRQ_AWAKE())

#ifdef ENABLE_ALL_TRX_IRQS

/**
 * \brief Clears the transceiver AMI interrupt
 *
 */
#define pal_trx_irq_flag_clr_ami()      (CLEAR_TRX_IRQ_AMI())

/**
 * \brief Clears the transceiver BATMON interrupt
 *
 */
#define pal_trx_irq_flag_clr_batmon()   (CLEAR_TRX_IRQ_BATMON())

/**
 * \brief Clears the transceiver PLL_LOCK interrupt
 *
 */
#define pal_trx_irq_flag_clr_pll_lock() (CLEAR_TRX_IRQ_PLL_LOCK())

/**
 * \brief Clears the transceiver PLL_UNLOCK interrupt
 *
 */
#define pal_trx_irq_flag_clr_pll_unlock()   (CLEAR_TRX_IRQ_PLL_UNLOCK())

#endif  /* ENABLE_ALL_TRX_IRQS */

/* === PROTOTYPES ========================================================== */

/**
 * \brief Initializes the transceiver RX END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver RX END interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver RX END interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_rx_end(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver TX END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver TX END interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver TX END interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_tx_end(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver CCA ED END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver CCA ED END interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver CCA ED END interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_cca_ed(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver AWAKE interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AWAKE interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver AWAKE interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_awake(FUNC_PTR trx_irq_cb);

#ifdef ENABLE_ALL_TRX_IRQS

/**
 * \brief Initializes the transceiver AMI interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AMI interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver AMI interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_ami(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver BATMON interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver BATMON interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver BATMON interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_batmon(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver PLL_LOCK interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver PLL_LOCK interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver PLL_LOCK interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_pll_lock(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver PLL_UNLOCK interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver PLL_UNLOCK interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver PLL_UNLOCK interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_pll_unlock(FUNC_PTR trx_irq_cb);

/**
 * \brief Initializes the transceiver AES_READY interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AES_READY interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver AES_READY interrupt
 * \ingroup group_pal_interrupt
 */
void pal_trx_irq_init_aes_ready(FUNC_PTR trx_irq_cb);

#endif  /* ENABLE_ALL_TRX_IRQS */

#ifdef PAL_XTD_IRQ_API

/**
 * \brief Returns the current callback function for the transceiver RX END
 * interrupt
 *
 * \return Current callback function for the transceiver RX END interrupt
 * \ingroup group_pal_interrupt
 */
FUNC_PTR pal_trx_irq_get_hdlr_rx_end(void);

/**
 * \brief Returns the current callback function for the transceiver TX END
 * interrupt
 *
 * \return Current callback function for the transceiver TX END interrupt
 * \ingroup group_pal_interrupt
 */
FUNC_PTR pal_trx_irq_get_hdlr_tx_end(void);

#endif  /* PAL_XTD_IRQ_API */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup group_tal_irq_rfa1
 * @{
 */

/**
 * \brief Transceiver interrupt handler
 *
 * This function handles the transceiver generated interrupts for RX end.
 */

void trx_rx_end_handler_cb(void);

/**
 * \brief Transceiver interrupt handler
 *
 * This function handles the transceiver generated interrupts for TX end.
 */

void trx_tx_end_handler_cb(void);

/*
 * \brief Transceiver interrupt handler for awake end IRQ
 *
 * This function handles the transceiver awake end interrupt.
 */
void trx_awake_handler_cb(void);

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) || (defined DOXYGEN)

/**
 * \brief Timestamp interrupt handler
 *
 * This function handles the interrupts handling the timestamp.
 *
 * The timestamping is only required for
 * - beaconing networks or if timestamping is explicitly enabled,
 * - and if antenna diversity is NOT enabled,
 * - and if the time stamp interrupt is not explicitly disabled.
 */
void trx_irq_timestamp_handler_cb(void);

#endif

typedef void (*irq_handler_t)(void);
/* ! @} */
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_IRQ_HANDLER_H */

/* EOF */
